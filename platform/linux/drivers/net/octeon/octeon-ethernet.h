/**********************************************************************
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
**********************************************************************/

/*
 * External interface for the Cavium Octeon ethernet driver.
 */
#ifndef OCTEON_ETHERNET_H
#define OCTEON_ETHERNET_H

#include <asm/octeon/cvmx-helper.h>

#include <asm/octeon/octeon-ethernet-user.h>

/**
 * This is the definition of the Ethernet driver's private
 * driver state stored in netdev_priv(dev).
 */
struct octeon_ethernet {
	/* PKO hardware output port */
	int port;
	/* My netdev. */
	struct net_device *netdev;
	/* My location in the cvm_oct_list */
	struct list_head list;
	/*
	 * Type of port. This is one of the enums in
	 * cvmx_helper_interface_mode_t
	 */
	int imode;

	/* Flags controlling aspects of the device */
	int flags;
#define OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW (1<<0)
#define OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW (1<<1)
#define OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_SW (1<<2)
#define OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_HW (1<<3)
#define OCTEON_ETHERNET_FLAG_TX_LOCKLESS     (1<<4)

	/* Optional intecept callback defined above */
	cvm_oct_callback_t      intercept_cb;

	/* Number of elements in tx_queue below */
	int                     num_tx_queues;

	/* SRIO ports add this header for the SRIO block */
	uint64_t                srio_tx_header;

	struct {
		/* PKO hardware queue for the port */
		int	queue;
		/* Hardware fetch and add to count outstanding tx buffers */
		int	fau;
	} tx_queue[16];

	struct phy_device *phydev;
	/* MII bus the PHY is connected to */
	struct mii_bus *mii_bus;
	unsigned int last_link;
	/* Last negotiated link state */
	uint64_t link_info;
	/* Called periodically to check link status */
	void (*poll) (struct net_device *dev);
	struct delayed_work	port_periodic_work;
	struct work_struct	port_work;	/* may be unused. */
	u64 last_tx_octets;
	u32 last_tx_packets;
};

int cvm_oct_free_work(void *work_queue_entry);

extern int cvm_oct_rgmii_init(struct net_device *dev);
extern void cvm_oct_rgmii_uninit(struct net_device *dev);
extern int cvm_oct_rgmii_open(struct net_device *dev);
extern int cvm_oct_rgmii_stop(struct net_device *dev);

extern int cvm_oct_sgmii_init(struct net_device *dev);
extern void cvm_oct_sgmii_uninit(struct net_device *dev);
extern int cvm_oct_sgmii_open(struct net_device *dev);
extern int cvm_oct_sgmii_stop(struct net_device *dev);

extern int cvm_oct_spi_init(struct net_device *dev);
extern void cvm_oct_spi_uninit(struct net_device *dev);
extern int cvm_oct_xaui_init(struct net_device *dev);
extern void cvm_oct_xaui_uninit(struct net_device *dev);
extern int cvm_oct_xaui_open(struct net_device *dev);
extern int cvm_oct_xaui_stop(struct net_device *dev);

extern int cvm_oct_srio_init(struct net_device *dev);
extern int cvm_oct_xmit_srio(struct sk_buff *skb, struct net_device *dev);
extern int cvm_oct_srio_set_mac_address(struct net_device *dev, void *addr);
extern int cvm_oct_srio_change_mtu(struct net_device *dev, int new_mtu);
extern struct net_device_stats *cvm_oct_srio_get_stats(struct net_device *dev);

extern int cvm_oct_common_init(struct net_device *dev);
extern void cvm_oct_common_uninit(struct net_device *dev);

extern void cvm_oct_set_carrier(struct octeon_ethernet *priv, cvmx_helper_link_info_t link_info);

extern int always_use_pow;
extern int pow_send_group;
extern int pow_receive_group;
extern char pow_send_list[];
extern struct list_head cvm_oct_list;
extern struct octeon_ethernet *cvm_oct_by_port[];
extern struct octeon_ethernet *cvm_oct_by_srio_mbox[2][4];

extern struct workqueue_struct *cvm_oct_poll_queue;
extern atomic_t cvm_oct_poll_queue_stopping;

extern int max_rx_cpus;
extern int rx_napi_weight;

#endif
