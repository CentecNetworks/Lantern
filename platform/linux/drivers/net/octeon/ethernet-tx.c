/*********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2010 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
*********************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <net/dst.h>
#ifdef CONFIG_XFRM
#include <linux/xfrm.h>
#include <net/xfrm.h>
#endif /* CONFIG_XFRM */

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-wqe.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>
#include <asm/octeon/cvmx-helper.h>

#include <asm/octeon/cvmx-gmxx-defs.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-tx.h"
#include "ethernet-util.h"


#define CVM_OCT_SKB_CB(skb)	((u64 *)((skb)->cb))

/*
 * You can define GET_SKBUFF_QOS() to override how the skbuff output
 * function determines which output queue is used. The default
 * implementation always uses the base queue for the port. If, for
 * example, you wanted to use the skb->priority fieid, define
 * GET_SKBUFF_QOS as: #define GET_SKBUFF_QOS(skb) ((skb)->priority)
 */
#ifndef GET_SKBUFF_QOS
#define GET_SKBUFF_QOS(skb) 0
#else

#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
#error "GET_SKBUFF_QOS doesn't work with LOCKLESS"
#endif

#endif

#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
#define CVM_OCT_LOCKLESS 1
#include "ethernet-xmit.c"
#endif

#undef CVM_OCT_LOCKLESS
#include "ethernet-xmit.c"

/**
 * cvm_oct_xmit_pow - transmit a packet to the POW
 * @skb:    Packet to send
 * @dev:    Device info structure

 * Returns Always returns zero
 */
int cvm_oct_xmit_pow(struct sk_buff *skb, struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	void               *packet_buffer;
	void               *copy_location;
	cvmx_buf_ptr_t     *buffer_ptr;
	int remaining_bytes = skb->len;
	int offset          = 0;
	int length;

	/* Get a work queue entry */
	cvmx_wqe_t *work = cvmx_fpa_alloc(CVMX_FPA_WQE_POOL);
	if (unlikely(work == NULL)) {
		DEBUGPRINT("%s: Failed to allocate a work queue entry\n", dev->name);
		dev->stats.tx_dropped++;
		dev_kfree_skb(skb);
		return 0;
	}

	/* Fill in some of the work queue fields. We may need to add more
	   if the software at the other end needs them */
	work->hw_chksum     = skb->csum;
	work->len           = skb->len;
	work->ipprt         = priv->port;
	work->qos           = priv->port & 0x7;
	work->grp           = pow_send_group;
	work->tag_type      = CVMX_HELPER_INPUT_TAG_TYPE;
	work->tag           = pow_send_group; /* FIXME */
	work->word2.u64     = 0;    /* Default to zero. Sets of zero later are commented out */
	work->word2.s.bufs  = 0;
	work->packet_ptr.u64 = 0;

	buffer_ptr = &work->packet_ptr;

	/* Get a packet buffer */
	packet_buffer = cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
	if (unlikely(packet_buffer == NULL)) {
		DEBUGPRINT("%s: Failed to allocate a packet buffer\n",
			   dev->name);
		cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));
		dev->stats.tx_dropped++;
		dev_kfree_skb(skb);
		return 0;
	}

	/* Calculate where we need to copy the data to. We need to leave 8 bytes
	   for a next pointer. */
	copy_location = packet_buffer + sizeof(cvmx_buf_ptr_t);

	/* We also need to include any configure skip. Then we need to align
	   the IP packet src and dest into the same 64bit word. The below
	   calculation may add a little extra, but that doesn't hurt */
	copy_location += ((CVMX_HELPER_FIRST_MBUFF_SKIP+7)&0xfff8) + 6;

	do {
		length = CVMX_FPA_PACKET_POOL_SIZE;
		length -= copy_location - packet_buffer;

		/* Copy the remaining bytes in the last buffer. */
		if (remaining_bytes < length)
			length = remaining_bytes;

		/* We have to copy the packet since whoever processes this packet
		   will free it to a hardware pool. We can't use the trick of
		   counting outstanding packets like in cvm_oct_xmit */
		memcpy(copy_location, skb->data + offset, length);

		buffer_ptr->s.addr = cvmx_ptr_to_phys(copy_location);
		buffer_ptr->s.pool = CVMX_FPA_PACKET_POOL;
		buffer_ptr->s.size = length;
		buffer_ptr->s.back = (copy_location - packet_buffer) >> 7;

		/* Increment the number of packet pointers used. */
		work->word2.s.bufs++;

		offset += length;
		remaining_bytes -= length;

		/* Create new packet buffer to fill the remaining data. */
		if (remaining_bytes) {
			packet_buffer = cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
			if (unlikely(packet_buffer == NULL)) {
				DEBUGPRINT("%s: Failed to allocate a packet buffer\n",
					   dev->name);
				cvm_oct_free_work(work);
				dev->stats.tx_dropped++;
				dev_kfree_skb(skb);
				return 0;
			}
			buffer_ptr = copy_location - sizeof(cvmx_buf_ptr_t);
			copy_location = packet_buffer + sizeof(cvmx_buf_ptr_t);
		}
	} while (remaining_bytes);

	if (skb->protocol == htons(ETH_P_IP)) {
		work->word2.s.ip_offset     = 14;
#if 0
		work->word2.s.vlan_valid  = 0; /* FIXME */
		work->word2.s.vlan_cfi    = 0; /* FIXME */
		work->word2.s.vlan_id     = 0; /* FIXME */
		work->word2.s.dec_ipcomp  = 0; /* FIXME */
#endif
		work->word2.s.tcp_or_udp    = (ip_hdr(skb)->protocol == IPPROTO_TCP) || (ip_hdr(skb)->protocol == IPPROTO_UDP);
#if 0
		work->word2.s.dec_ipsec   = 0; /* FIXME */
		work->word2.s.is_v6       = 0; /* We only support IPv4 right now */
		work->word2.s.software    = 0; /* Hardware would set to zero */
		work->word2.s.L4_error    = 0; /* No error, packet is internal */
#endif
		work->word2.s.is_frag       = !((ip_hdr(skb)->frag_off == 0) || (ip_hdr(skb)->frag_off == 1<<14));
#if 0
		work->word2.s.IP_exc      = 0;  /* Assume Linux is sending a good packet */
#endif
		work->word2.s.is_bcast      = (skb->pkt_type == PACKET_BROADCAST);
		work->word2.s.is_mcast      = (skb->pkt_type == PACKET_MULTICAST);
#if 0
		work->word2.s.not_IP      = 0; /* This is an IP packet */
		work->word2.s.rcv_error   = 0; /* No error, packet is internal */
		work->word2.s.err_code    = 0; /* No error, packet is internal */
#endif

		/* When copying the data, include 4 bytes of the ethernet header to
		   align the same way hardware does */
		memcpy(work->packet_data, skb->data + 10, sizeof(work->packet_data));
	} else {
#if 0
		work->word2.snoip.vlan_valid  = 0; /* FIXME */
		work->word2.snoip.vlan_cfi    = 0; /* FIXME */
		work->word2.snoip.vlan_id     = 0; /* FIXME */
		work->word2.snoip.software    = 0; /* Hardware would set to zero */
#endif
		work->word2.snoip.is_rarp       = skb->protocol == htons(ETH_P_RARP);
		work->word2.snoip.is_arp        = skb->protocol == htons(ETH_P_ARP);
		work->word2.snoip.is_bcast      = (skb->pkt_type == PACKET_BROADCAST);
		work->word2.snoip.is_mcast      = (skb->pkt_type == PACKET_MULTICAST);
		work->word2.snoip.not_IP        = 1; /* IP was done up above */
#if 0
		work->word2.snoip.rcv_error   = 0; /* No error, packet is internal */
		work->word2.snoip.err_code    = 0; /* No error, packet is internal */
#endif
		memcpy(work->packet_data, skb->data, sizeof(work->packet_data));
	}

	/* Submit the packet to the POW */
	cvmx_pow_work_submit(work, work->tag, work->tag_type, work->qos, work->grp);
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	dev_kfree_skb(skb);
	return 0;
}
/**
 * cvm_oct_transmit_qos - transmit a work queue entry out of the ethernet port.
 *
 * Both the work queue entry and the packet data can optionally be
 * freed. The work will be freed on error as well.
 *
 * @dev: Device to transmit out.
 * @work_queue_entry: Work queue entry to send
 * @do_free: True if the work queue entry and packet data should be
 *           freed. If false, neither will be freed.
 * @qos: Index into the queues for this port to transmit on. This is
 *       used to implement QoS if their are multiple queues per
 *       port. This parameter must be between 0 and the number of
 *       queues per port minus 1. Values outside of this range will be
 *       change to zero.
 *
 * Returns Zero on success, negative on failure.
 */
int cvm_oct_transmit_qos(struct net_device *dev,
			 void *work_queue_entry,
			 int do_free,
			 int qos)
{
	unsigned long			flags;
	cvmx_buf_ptr_t			hw_buffer;
	cvmx_pko_command_word0_t	pko_command;
	int				dropped;
	struct octeon_ethernet		*priv = netdev_priv(dev);
	cvmx_wqe_t			*work = work_queue_entry;
	cvmx_pko_lock_t lock_type;

	if (!(dev->flags & IFF_UP)) {
		DEBUGPRINT("%s: Device not up\n", dev->name);
		if (do_free)
			cvm_oct_free_work(work);
		return -1;
	}

	if (priv->flags & OCTEON_ETHERNET_FLAG_TX_LOCKLESS) {
		qos = cvmx_get_core_num();
		lock_type = CVMX_PKO_LOCK_NONE;
	} else {
		/*
		 * The check on CVMX_PKO_QUEUES_PER_PORT_* is designed to
		 * completely remove "qos" in the event neither interface
		 * supports multiple queues per port
		 */
		if ((CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 > 1) ||
			(CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 > 1)) {
			if (qos <= 0)
				qos = 0;
			else if (qos >= priv->num_tx_queues)
				qos = 0;
		} else
			qos = 0;
		lock_type = CVMX_PKO_LOCK_CMD_QUEUE;
	}

	/* Start off assuming no drop */
	dropped = 0;

	local_irq_save(flags);

	cvmx_pko_send_packet_prepare(priv->port, priv->tx_queue[qos].queue, lock_type);

	/* Build the PKO buffer pointer */
	hw_buffer.u64 = 0;
	hw_buffer.s.addr = work->packet_ptr.s.addr;
	hw_buffer.s.pool = CVMX_FPA_PACKET_POOL;
	hw_buffer.s.size = CVMX_FPA_PACKET_POOL_SIZE;
	hw_buffer.s.back = work->packet_ptr.s.back;

	/* Build the PKO command */
	pko_command.u64 = 0;
	pko_command.s.n2 = 1; /* Don't pollute L2 with the outgoing packet */
	pko_command.s.dontfree = !do_free;
	pko_command.s.segs = work->word2.s.bufs;
	pko_command.s.total_bytes = work->len;

	/* Check if we can use the hardware checksumming */
	if (unlikely(work->word2.s.not_IP || work->word2.s.IP_exc))
		pko_command.s.ipoffp1 = 0;
	else
		pko_command.s.ipoffp1 = sizeof(struct ethhdr) + 1;

	/* Send the packet to the output queue */
	if (unlikely(cvmx_pko_send_packet_finish(priv->port, priv->tx_queue[qos].queue, pko_command, hw_buffer, lock_type))) {
		DEBUGPRINT("%s: Failed to send the packet\n", dev->name);
		dropped = -1;
	}
	local_irq_restore(flags);

	if (unlikely(dropped)) {
		if (do_free)
			cvm_oct_free_work(work);
		dev->stats.tx_dropped++;
	} else
	if (do_free)
		cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));

	return dropped;
}
EXPORT_SYMBOL(cvm_oct_transmit_qos);

