/*************************************************************************
* Cavium Octeon Ethernet Driver
*
* Author: Cavium Networks info@caviumnetworks.com
*
* Copyright (c) 2010  Cavium Networks (support@cavium.com). All rights
* reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*
*     * Neither the name of Cavium Networks nor the names of
*       its contributors may be used to endorse or promote products
*       derived from this software without specific prior written
*       permission.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
* OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
* RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
* POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
* OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
*************************************************************************/
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <net/dst.h>
#include <net/sock.h>
#include <linux/rio.h>
#include "../../rapidio/rio.h"

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-srio.h>
#include <asm/octeon/cvmx-sriox-defs.h>
#include <asm/octeon/cvmx-sriomaintx-defs.h>

#include "octeon-ethernet.h"
#include "ethernet-tx.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"

/**
 * Get ethernet counters for port/mailbox
 *
 * @param dev
 *
 * @return
 */
struct net_device_stats *cvm_oct_srio_get_stats(struct net_device *dev)
{
	return &dev->stats;
}

/**
 * Change the MAC address
 *
 * @param dev
 * @param addr
 *
 * @return
 */
int cvm_oct_srio_set_mac_address(struct net_device *dev, void *addr)
{
	/* FIXME: Should this be allowed? Should it change our device ID? */
	memcpy(dev->dev_addr, addr + 2, 6);
	return 0;
}

/**
 * Change the MTU
 *
 * @param dev
 * @param new_mtu
 *
 * @return
 */
int cvm_oct_srio_change_mtu(struct net_device *dev, int new_mtu)
{
	/*
	 * Limit the MTU to make sure the ethernet packets are between
	 * 1 byte and 4096-14 bytes.
	 */
	if ((new_mtu < 1) || (new_mtu > 4096 - 14)) {
		printk("%s: MTU must be between %d and %d.\n", dev->name, 1, 4096-14);
		return -EINVAL;
	}
	dev->mtu = new_mtu;
	return 0;
}

/**
 * Packet transmit for a SRIO port
 *
 * @param skb    Packet to send
 * @param dev    Device info structure
 *
 * @return Always returns zero
 */
int cvm_oct_xmit_srio(struct sk_buff *skb, struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_srio_tx_message_header_t tx_header;
	uint64_t dest_mac;

	if (unlikely(skb->len > 4096)) {
		dev_kfree_skb(skb);
		DEBUGPRINT("%s: TX packet larger than 4096 bytes. Dropped.\n", dev->name);
		return 0;
	}

	tx_header.u64 = priv->srio_tx_header;
	/* Use the socket priority if it is available */
	if (skb->sk) {
		if (skb->sk->sk_priority < 0)
			tx_header.s.prio = 0;
		else if (skb->sk->sk_priority > 3)
			tx_header.s.prio = 3;
		else
			tx_header.s.prio = skb->sk->sk_priority;
	}

	/* Extract the destination MAC address from the packet */
	dest_mac = *(uint64_t *)skb->data >> 16;

	/* If this is a broadcast/multicast we must manually send to everyone */
	if (dest_mac>>40) {
		struct rio_dev *rdev;
		struct sk_buff *new_skb;
		int srio_port = (priv->port - 40) >> 1;

		/* The RIO device list must be protected by a global lock */
		spin_lock(&rio_global_list_lock);
		list_for_each_entry(rdev, &rio_devices, global_list) {
			/* Skip devices not on my rio port */
			if (rdev->net->hport->id != srio_port)
				continue;
			/* Skip switches */
			if (rdev->destid == 0xffff)
				continue;
			/* Create a new SKB since each packet will have different data */
			new_skb = skb_copy(skb, GFP_ATOMIC);
			if (new_skb) {
				tx_header.s.did = rdev->destid;
				*(uint64_t *)__skb_push(new_skb, 8) = tx_header.u64;
				cvm_oct_xmit(new_skb, dev);
			} else {
				DEBUGPRINT("%s: SKB allocation failed\n", dev->name);
				break;
			}
		}
		spin_unlock(&rio_global_list_lock);

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	} else {
		/* Use the low two bytes of the destination MAC as the SRIO destination */
		tx_header.s.did = *(uint16_t *)(skb->data + 4);
		if (unlikely(skb_headroom(skb) < 8)) {
			struct sk_buff *new_skb = skb_copy(skb, GFP_ATOMIC);
			dev_kfree_skb(skb);
			if (!new_skb) {
				DEBUGPRINT("%s: SKB didn't have room for SRIO header and allocation failed\n", dev->name);
				return NETDEV_TX_OK;
			}
			skb = new_skb;
		}

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
		*(uint64_t *)__skb_push(skb, 8) = tx_header.u64;
		return cvm_oct_xmit(skb, dev);
	}
}

/**
 * Initialize SRIO
 *
 * @param dev
 *
 * @return
 */
int cvm_oct_srio_init(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	int srio_port = (priv->port - 40) >> 1;
	uint32_t devid;
	struct sockaddr sa;
	cvmx_sriox_status_reg_t srio_status_reg;

	dev->features |= NETIF_F_LLTX; /* We do our own locking, Linux doesn't need to */

	SET_ETHTOOL_OPS(dev, &cvm_oct_ethtool_ops);

	/* Make sure register access is allowed */
	srio_status_reg.u64 = cvmx_read_csr(CVMX_SRIOX_STATUS_REG(srio_port));
	if (!srio_status_reg.s.access)
		return 0;

	cvmx_srio_config_read32(srio_port, 0, -1, 1, 0, CVMX_SRIOMAINTX_PRI_DEV_ID(srio_port), &devid);

	sa.sa_data[0] = 0;
	sa.sa_data[1] = 0;
	sa.sa_data[2] = 0;
	sa.sa_data[3] = 0;
	if (devid >> 16) {
		sa.sa_data[4] = 0;
		sa.sa_data[5] = (devid >> 16) & 0xff;
	} else {
		sa.sa_data[4] = (devid >> 8) & 0xff;
		sa.sa_data[5] = devid & 0xff;
	}

	dev->netdev_ops->ndo_set_mac_address(dev, &sa);
	dev->netdev_ops->ndo_change_mtu(dev, dev->mtu);

	return 0;
}
