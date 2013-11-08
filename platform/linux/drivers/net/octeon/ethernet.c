/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2007 Cavium Networks
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>

#include <net/dst.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-ipd.h>
#include <asm/octeon/cvmx-srio.h>
#include <asm/octeon/cvmx-helper.h>

#include <asm/octeon/cvmx-gmxx-defs.h>
#include <asm/octeon/cvmx-smix-defs.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-proc.h"
#include "ethernet-mem.h"
#include "ethernet-rx.h"
#include "ethernet-tx.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"


#if defined(CONFIG_OCTEON_NUM_PACKET_BUFFERS) \
	&& CONFIG_OCTEON_NUM_PACKET_BUFFERS
int num_packet_buffers = CONFIG_OCTEON_NUM_PACKET_BUFFERS;
#else
int num_packet_buffers = 1024;
#endif
module_param(num_packet_buffers, int, 0444);
MODULE_PARM_DESC(num_packet_buffers, "\n"
	"\tNumber of packet buffers to allocate and store in the\n"
	"\tFPA. By default, 1024 packet buffers are used unless\n"
	"\tCONFIG_OCTEON_NUM_PACKET_BUFFERS is defined.");

int pow_receive_group = 15;
module_param(pow_receive_group, int, 0444);
MODULE_PARM_DESC(pow_receive_group, "\n"
	"\tPOW group to receive packets from. All ethernet hardware\n"
	"\twill be configured to send incomming packets to this POW\n"
	"\tgroup. Also any other software can submit packets to this\n"
	"\tgroup for the kernel to process.");

int pow_send_group = -1;
module_param(pow_send_group, int, 0644);
MODULE_PARM_DESC(pow_send_group, "\n"
	"\tPOW group to send packets to other software on. This\n"
	"\tcontrols the creation of the virtual device pow0.\n"
	"\talways_use_pow also depends on this value.");

int always_use_pow;
module_param(always_use_pow, int, 0444);
MODULE_PARM_DESC(always_use_pow, "\n"
	"\tWhen set, always send to the pow group. This will cause\n"
	"\tpackets sent to real ethernet devices to be sent to the\n"
	"\tPOW group instead of the hardware. Unless some other\n"
	"\tapplication changes the config, packets will still be\n"
	"\treceived from the low level hardware. Use this option\n"
	"\tto allow a CVMX app to intercept all packets from the\n"
	"\tlinux kernel. You must specify pow_send_group along with\n"
	"\tthis option.");

char pow_send_list[128] = "";
module_param_string(pow_send_list, pow_send_list, sizeof(pow_send_list), 0444);
MODULE_PARM_DESC(pow_send_list, "\n"
	"\tComma separated list of ethernet devices that should use the\n"
	"\tPOW for transmit instead of the actual ethernet hardware. This\n"
	"\tis a per port version of always_use_pow. always_use_pow takes\n"
	"\tprecedence over this list. For example, setting this to\n"
	"\t\"eth2,spi3,spi7\" would cause these three devices to transmit\n"
	"\tusing the pow_send_group.");

static int disable_core_queueing = 1;
module_param(disable_core_queueing, int, 0444);
MODULE_PARM_DESC(disable_core_queueing, "\n"
		"\t\tWhen set the networking core's tx_queue_len is set to zero.  This\n"
		"\t\tallows packets to be sent without lock contention in the packet scheduler\n"
		"\t\tresulting in some cases in improved throughput.");

int max_rx_cpus = -1;
module_param(max_rx_cpus, int, 0444);
MODULE_PARM_DESC(max_rx_cpus, "\n"
	"\t\tThe maximum number of CPUs to use for packet reception.\n"
	"\t\tUse -1 to use all available CPUs.");

int rx_napi_weight = 32;
module_param(rx_napi_weight, int, 0444);
MODULE_PARM_DESC(rx_napi_weight, "The NAPI WEIGHT parameter.");

/*
 * The offset from mac_addr_base that should be used for the next port
 * that is configured.  By convention, if any mgmt ports exist on the
 * chip, they get the first mac addresses, The ports controlled by
 * this driver are numbered sequencially following any mgmt addresses
 * that may exist.
 */
static unsigned int cvm_oct_mac_addr_offset;

/**
 * cvm_oct_poll_queue - Workqueue for polling operations.
 */
struct workqueue_struct *cvm_oct_poll_queue;

/**
 * cvm_oct_poll_queue_stopping - flag to indicate polling should stop.
 *
 * Set to one right before cvm_oct_poll_queue is destroyed.
 */
atomic_t cvm_oct_poll_queue_stopping = ATOMIC_INIT(0);

/*
 * cvm_oct_by_port is an array of every ethernet device owned by this driver indexed by
 * the ipd input port number.
 */
struct octeon_ethernet *cvm_oct_by_port[TOTAL_NUMBER_OF_PORTS] __cacheline_aligned;

/*
 * cvm_oct_by_srio_mbox is indexed by the SRIO mailbox.
 */
struct octeon_ethernet *cvm_oct_by_srio_mbox[2][4];

/*
 * cvm_oct_list is a list of all cvm_oct_private_t created by this driver.
 */
LIST_HEAD(cvm_oct_list);

static void cvm_oct_rx_refill_worker(struct work_struct *work);
static DECLARE_DELAYED_WORK(cvm_oct_rx_refill_work, cvm_oct_rx_refill_worker);

static void cvm_oct_rx_refill_worker(struct work_struct *work)
{
	/*
	 * FPA 0 may have been drained, try to refill it if we need
	 * more than num_packet_buffers / 2, otherwise normal receive
	 * processing will refill it.  If it were drained, no packets
	 * could be received so cvm_oct_napi_poll would never be
	 * invoked to do the refill.
	 */
	cvm_oct_rx_refill_pool(num_packet_buffers / 2);

	if (!atomic_read(&cvm_oct_poll_queue_stopping))
		queue_delayed_work(cvm_oct_poll_queue,
				   &cvm_oct_rx_refill_work, HZ);
}

static void cvm_oct_periodic_worker(struct work_struct *work)
{
    unsigned long pre_rx_crc_errors;
    unsigned long pre_rx_frame_errors;
	struct octeon_ethernet *priv = container_of(work,
						    struct octeon_ethernet,
						    port_periodic_work.work);

	if (priv->poll)
		priv->poll(priv->netdev);

    pre_rx_crc_errors = priv->netdev->stats.rx_crc_errors;
    pre_rx_frame_errors = priv->netdev->stats.rx_frame_errors;

	priv->netdev->netdev_ops->ndo_get_stats(priv->netdev);
#define MAX_ERR_PKT_CNT_PER_SEC   64
    /* add by zhuj for walkaround the port with abormal situations */
    if(((priv->netdev->stats.rx_crc_errors - pre_rx_crc_errors) > MAX_ERR_PKT_CNT_PER_SEC)
        || ((priv->netdev->stats.rx_frame_errors - pre_rx_frame_errors) > MAX_ERR_PKT_CNT_PER_SEC))
    {
        /* reset phy */
        printk("Reset the abnormal octen network port.\n");
        phy_reset(priv->phydev);
    }

	if (!atomic_read(&cvm_oct_poll_queue_stopping))
		queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
 }

static int cvm_oct_num_output_buffers;

static __init void cvm_oct_configure_common_hw(void)
{
	/* Setup the FPA */
	cvmx_fpa_enable();
	cvm_oct_mem_fill_fpa(CVMX_FPA_PACKET_POOL, CVMX_FPA_PACKET_POOL_SIZE,
			     num_packet_buffers);
	cvm_oct_mem_fill_fpa(CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE,
			     num_packet_buffers);
	if (CVMX_FPA_OUTPUT_BUFFER_POOL != CVMX_FPA_PACKET_POOL) {
		cvm_oct_num_output_buffers = 4 * octeon_pko_get_total_queues();
		cvm_oct_mem_fill_fpa(CVMX_FPA_OUTPUT_BUFFER_POOL,
				     CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, cvm_oct_num_output_buffers);
	}

	if (USE_RED)
		cvmx_helper_setup_red(num_packet_buffers / 4,
				      num_packet_buffers / 8);

}

/**
 * cvm_oct_register_callback -  Register a intercept callback for the named device.
 *
 * It returns the net_device structure for the ethernet port. Usign a
 * callback of NULL will remove the callback. Note that this callback
 * must not disturb scratch. It will be called with SYNCIOBDMAs in
 * progress and userspace may be using scratch. It also must not
 * disturb the group mask.
 *
 * @device_name: Device name to register for. (Example: "eth0")
 * @callback: Intercept callback to set.
 *
 * Returns the net_device structure for the ethernet port or NULL on failure.
 */
struct net_device *cvm_oct_register_callback(const char *device_name, cvm_oct_callback_t callback)
{
	struct octeon_ethernet *priv;

	list_for_each_entry(priv, &cvm_oct_list, list) {
		if (strcmp(device_name, priv->netdev->name) == 0) {
			priv->intercept_cb = callback;
			wmb();
			return priv->netdev;
		}
	}
	return NULL;
}
EXPORT_SYMBOL(cvm_oct_register_callback);

/**
 * cvm_oct_free_work- Free a work queue entry
 *
 * @work_queue_entry: Work queue entry to free
 *
 * Returns Zero on success, Negative on failure.
 */
int cvm_oct_free_work(void *work_queue_entry)
{
	cvmx_wqe_t *work = work_queue_entry;

	int segments = work->word2.s.bufs;
	union cvmx_buf_ptr segment_ptr = work->packet_ptr;

	while (segments--) {
		union cvmx_buf_ptr next_ptr = *(union cvmx_buf_ptr *)cvmx_phys_to_ptr(segment_ptr.s.addr - 8);
		if (unlikely(!segment_ptr.s.i))
			cvmx_fpa_free(cvm_oct_get_buffer_ptr(segment_ptr),
				      segment_ptr.s.pool,
				      DONT_WRITEBACK(CVMX_FPA_PACKET_POOL_SIZE / 128));
		segment_ptr = next_ptr;
	}
	cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));

	return 0;
}
EXPORT_SYMBOL(cvm_oct_free_work);

/* Lock to protect racy cvmx_pko_get_port_status() */
static DEFINE_SPINLOCK(cvm_oct_tx_stat_lock);

/**
 * cvm_oct_common_get_stats - get the low level ethernet statistics
 * @dev:    Device to get the statistics from
 *
 * Returns Pointer to the statistics
 */
static struct net_device_stats *cvm_oct_common_get_stats(struct net_device *dev)
{
	unsigned long flags;
	cvmx_pip_port_status_t rx_status;
	cvmx_pko_port_status_t tx_status;
	u64 current_tx_octets;
	u32 current_tx_packets;
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->port < CVMX_PIP_NUM_INPUT_PORTS) {
		if (octeon_is_simulation()) {
			/* The simulator doesn't support statistics */
			memset(&rx_status, 0, sizeof(rx_status));
			memset(&tx_status, 0, sizeof(tx_status));
		} else {
			cvmx_pip_get_port_status(priv->port, 1, &rx_status);

			spin_lock_irqsave(&cvm_oct_tx_stat_lock, flags);
			cvmx_pko_get_port_status(priv->port, 0, &tx_status);
			current_tx_packets = tx_status.packets;
			current_tx_octets = tx_status.octets;
			/*
			 * The tx_packets counter is 32-bits as are
			 * all these variables.  No truncation
			 * necessary.
			 */
			tx_status.packets = current_tx_packets - priv->last_tx_packets;
			/*
			 * The tx_octets counter is only 48-bits, so
			 * we need to truncate in case there was a
			 * wrap-around
			 */
			tx_status.octets = (current_tx_octets - priv->last_tx_octets) & 0xffffffffffffull;
			priv->last_tx_packets = current_tx_packets;
			priv->last_tx_octets = current_tx_octets;
			spin_unlock_irqrestore(&cvm_oct_tx_stat_lock, flags);
		}

		dev->stats.rx_packets += rx_status.inb_packets;
		dev->stats.tx_packets += tx_status.packets;
		dev->stats.rx_bytes += rx_status.inb_octets;
		dev->stats.tx_bytes += tx_status.octets;
		dev->stats.multicast += rx_status.multicast_packets;
		dev->stats.rx_crc_errors += rx_status.inb_errors;
		dev->stats.rx_frame_errors += rx_status.fcs_align_err_packets;

		/*
		 * The drop counter must be incremented atomically
		 * since the RX tasklet also increments it.
		 */
		atomic64_add(rx_status.dropped_packets,
			     (atomic64_t *)&dev->stats.rx_dropped);
	}

	return &dev->stats;
}

/**
 * cvm_oct_common_change_mtu - change the link MTU
 * @dev:     Device to change
 * @new_mtu: The new MTU
 *
 * Returns Zero on success
 */
static int cvm_oct_common_change_mtu(struct net_device *dev, int new_mtu)
{
    /* modified by zhuj for removing the configuration to the end of cvm_oct_init_module, 2011-04-20 */
#if 0
	struct octeon_ethernet *priv = netdev_priv(dev);
	int interface = INTERFACE(priv->port);
	int index = INDEX(priv->port);
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	int vlan_bytes = 4;
#else
	int vlan_bytes = 0;
#endif

	/*
	 * Limit the MTU to make sure the ethernet packets are between
	 * 64 bytes and 65535 bytes.
	 */
	if ((new_mtu + 14 + 4 + vlan_bytes < 64)
	    || (new_mtu + 14 + 4 + vlan_bytes > 65392)) {
		pr_err("MTU must be between %d and %d.\n",
		       64 - 14 - 4 - vlan_bytes, 65392 - 14 - 4 - vlan_bytes);
		return -EINVAL;
	}
	dev->mtu = new_mtu;

	if ((interface < 2)
	    && (cvmx_helper_interface_get_mode(interface) !=
		CVMX_HELPER_INTERFACE_MODE_SPI)) {
		/* Add ethernet header and FCS, and VLAN if configured. */
		int max_packet = new_mtu + 14 + 4 + vlan_bytes;

		if (OCTEON_IS_MODEL(OCTEON_CN3XXX)
		    || OCTEON_IS_MODEL(OCTEON_CN58XX)) {
			/* Signal errors on packets larger than the MTU */
			cvmx_write_csr(CVMX_GMXX_RXX_FRM_MAX(index, interface),
				       max_packet);
		} else {
			/*
			 * Set the hardware to truncate packets larger
			 * than the MTU and smaller the 64 bytes.
			 */
			union cvmx_pip_frm_len_chkx frm_len_chk;
			frm_len_chk.u64 = 0;
			frm_len_chk.s.minlen = 64;
			frm_len_chk.s.maxlen = max_packet;
			cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(interface), frm_len_chk.u64);
		}
		/*
		 * Set the hardware to truncate packets larger than
		 * the MTU. The jabber register must be set to a
		 * multiple of 8 bytes, so round up.
		 */
		cvmx_write_csr(CVMX_GMXX_RXX_JABBER(index, interface),
			       (max_packet + 7) & ~7u);
	}
#else
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	int vlan_bytes = 4;
#else
	int vlan_bytes = 0;
#endif

	/*
	 * Limit the MTU to make sure the ethernet packets are between
	 * 64 bytes and 65535 bytes.
	 */
	if ((new_mtu + 14 + 4 + vlan_bytes < 64)
	    || (new_mtu + 14 + 4 + vlan_bytes > 65392)) {
		pr_err("MTU must be between %d and %d.\n",
		       64 - 14 - 4 - vlan_bytes, 65392 - 14 - 4 - vlan_bytes);
		return -EINVAL;
	}
	dev->mtu = new_mtu;
#endif
	return 0;
}

/**
 * cvm_oct_common_set_multicast_list - set the multicast list
 * @dev:    Device to work on
 */
static void cvm_oct_common_set_multicast_list(struct net_device *dev)
{
	union cvmx_gmxx_prtx_cfg gmx_cfg;
	struct octeon_ethernet *priv = netdev_priv(dev);
	int interface = INTERFACE(priv->port);
	int index = INDEX(priv->port);

	if ((interface < 2)
	    && (cvmx_helper_interface_get_mode(interface) !=
		CVMX_HELPER_INTERFACE_MODE_SPI)) {
		union cvmx_gmxx_rxx_adr_ctl control;
		control.u64 = 0;
		control.s.bcst = 1;	/* Allow broadcast MAC addresses */

		if (dev->mc_list || (dev->flags & IFF_ALLMULTI) ||
		    (dev->flags & IFF_PROMISC))
			/* Force accept multicast packets */
			control.s.mcst = 2;
		else
			/* Force reject multicat packets */
			control.s.mcst = 1;

		if (dev->flags & IFF_PROMISC)
			/*
			 * Reject matches if promisc. Since CAM is
			 * shut off, should accept everything.
			 */
			control.s.cam_mode = 0;
		else
			/* Filter packets based on the CAM */
			control.s.cam_mode = 1;

		gmx_cfg.u64 =
		    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface),
			       gmx_cfg.u64 & ~1ull);

		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CTL(index, interface),
			       control.u64);
		if (dev->flags & IFF_PROMISC)
			cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN
				       (index, interface), 0);
		else
			cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN
				       (index, interface), 1);

		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface),
			       gmx_cfg.u64);
	}
}

/**
 * cvm_oct_common_set_mac_address - set the hardware MAC address for a device
 * @dev:    The device in question.
 * @addr:   Address structure to change it too.

 * Returns Zero on success
 */
static int cvm_oct_common_set_mac_address(struct net_device *dev, void *addr)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	union cvmx_gmxx_prtx_cfg gmx_cfg;
	int interface = INTERFACE(priv->port);
	int index = INDEX(priv->port);

	memcpy(dev->dev_addr, addr + 2, 6);

	if ((interface < 2)
	    && (cvmx_helper_interface_get_mode(interface) !=
		CVMX_HELPER_INTERFACE_MODE_SPI)) {
		int i;
		uint8_t *ptr = addr;
		uint64_t mac = 0;
		for (i = 0; i < 6; i++)
			mac = (mac << 8) | (uint64_t) (ptr[i + 2]);

		gmx_cfg.u64 =
		    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface),
			       gmx_cfg.u64 & ~1ull);

		cvmx_write_csr(CVMX_GMXX_SMACX(index, interface), mac);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM0(index, interface), ptr[2]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM1(index, interface), ptr[3]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM2(index, interface), ptr[4]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM3(index, interface), ptr[5]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM4(index, interface), ptr[6]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM5(index, interface), ptr[7]);
		cvm_oct_common_set_multicast_list(dev);
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
	}
	return 0;
}

/**
 * cvm_oct_common_init - per network device initialization
 * @dev:    Device to initialize
 *
 * Returns Zero on success
 */
int cvm_oct_common_init(struct net_device *dev)
{
	unsigned long flags;
	cvmx_pko_port_status_t tx_status;
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct sockaddr sa;
	u64 mac = ((u64)(octeon_bootinfo->mac_addr_base[0] & 0xff) << 40) |
		((u64)(octeon_bootinfo->mac_addr_base[1] & 0xff) << 32) |
		((u64)(octeon_bootinfo->mac_addr_base[2] & 0xff) << 24) |
		((u64)(octeon_bootinfo->mac_addr_base[3] & 0xff) << 16) |
		((u64)(octeon_bootinfo->mac_addr_base[4] & 0xff) << 8) |
		(u64)(octeon_bootinfo->mac_addr_base[5] & 0xff);

	mac += cvm_oct_mac_addr_offset;
	sa.sa_data[0] = (mac >> 40) & 0xff;
	sa.sa_data[1] = (mac >> 32) & 0xff;
	sa.sa_data[2] = (mac >> 24) & 0xff;
	sa.sa_data[3] = (mac >> 16) & 0xff;
	sa.sa_data[4] = (mac >> 8) & 0xff;
	sa.sa_data[5] = mac & 0xff;

	if (cvm_oct_mac_addr_offset >= octeon_bootinfo->mac_addr_count)
		printk(KERN_DEBUG "%s: Using MAC outside of the assigned range:"
			" %02x:%02x:%02x:%02x:%02x:%02x\n", dev->name,
			sa.sa_data[0] & 0xff, sa.sa_data[1] & 0xff,
			sa.sa_data[2] & 0xff, sa.sa_data[3] & 0xff,
			sa.sa_data[4] & 0xff, sa.sa_data[5] & 0xff);
	cvm_oct_mac_addr_offset++;

	if (priv->num_tx_queues) {
		dev->features |= NETIF_F_SG;
		if (USE_HW_TCPUDP_CHECKSUM)
			dev->features |= NETIF_F_IP_CSUM;
	}

	/* We do our own locking, Linux doesn't need to */
	dev->features |= NETIF_F_LLTX;
	SET_ETHTOOL_OPS(dev, &cvm_oct_ethtool_ops);

	cvm_oct_phy_setup_device(dev);
	dev->netdev_ops->ndo_set_mac_address(dev, &sa);
	dev->netdev_ops->ndo_change_mtu(dev, dev->mtu);

	spin_lock_irqsave(&cvm_oct_tx_stat_lock, flags);
	cvmx_pko_get_port_status(priv->port, 0, &tx_status);
	priv->last_tx_packets = tx_status.packets;
	priv->last_tx_octets = tx_status.octets;
	/*
	 * Zero out stats for port so we won't mistakenly show
	 * counters from the bootloader.
	 */
	memset(&dev->stats, 0, sizeof(struct net_device_stats));
	spin_unlock_irqrestore(&cvm_oct_tx_stat_lock, flags);

	return 0;
}

void cvm_oct_common_uninit(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->phydev)
		phy_disconnect(priv->phydev);
}

static const struct net_device_ops cvm_oct_npi_netdev_ops = {
	.ndo_init		= cvm_oct_common_init,
	.ndo_uninit		= cvm_oct_common_uninit,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};

static const struct net_device_ops cvm_oct_xaui_netdev_ops = {
	.ndo_init		= cvm_oct_xaui_init,
	.ndo_uninit		= cvm_oct_xaui_uninit,
	.ndo_open		= cvm_oct_xaui_open,
	.ndo_stop		= cvm_oct_xaui_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_sgmii_netdev_ops = {
	.ndo_init		= cvm_oct_sgmii_init,
	.ndo_uninit		= cvm_oct_sgmii_uninit,
	.ndo_open		= cvm_oct_sgmii_open,
	.ndo_stop		= cvm_oct_sgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_spi_netdev_ops = {
	.ndo_init		= cvm_oct_spi_init,
	.ndo_uninit		= cvm_oct_spi_uninit,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_rgmii_netdev_ops = {
	.ndo_init		= cvm_oct_rgmii_init,
	.ndo_uninit		= cvm_oct_rgmii_uninit,
	.ndo_open		= cvm_oct_rgmii_open,
	.ndo_stop		= cvm_oct_rgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
static const struct net_device_ops cvm_oct_xaui_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_xaui_init,
	.ndo_uninit		= cvm_oct_xaui_uninit,
	.ndo_open		= cvm_oct_xaui_open,
	.ndo_stop		= cvm_oct_xaui_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_sgmii_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_sgmii_init,
	.ndo_uninit		= cvm_oct_sgmii_uninit,
	.ndo_open		= cvm_oct_sgmii_open,
	.ndo_stop		= cvm_oct_sgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_spi_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_spi_init,
	.ndo_uninit		= cvm_oct_spi_uninit,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_rgmii_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_rgmii_init,
	.ndo_uninit		= cvm_oct_rgmii_uninit,
	.ndo_open		= cvm_oct_rgmii_open,
	.ndo_stop		= cvm_oct_rgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#endif
#ifdef CONFIG_RAPIDIO
static const struct net_device_ops cvm_oct_srio_netdev_ops = {
	.ndo_init		= cvm_oct_srio_init,
	.ndo_start_xmit		= cvm_oct_xmit_srio,
	.ndo_set_mac_address	= cvm_oct_srio_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_srio_change_mtu,
	.ndo_get_stats		= cvm_oct_srio_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#endif
static const struct net_device_ops cvm_oct_pow_netdev_ops = {
	.ndo_init		= cvm_oct_common_init,
	.ndo_start_xmit		= cvm_oct_xmit_pow,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};

extern void octeon_mdiobus_force_mod_depencency(void);

static int num_devices_extra_wqe;
#define PER_DEVICE_EXTRA_WQE (MAX_OUT_QUEUE_DEPTH)

static void cvm_oct_override_pko_queue_priority(int pko_port, uint64_t priorities[16])
{
	int i;

	if (octeon_pko_lockless()) {
		for (i = 0; i < 16; i++)
			priorities[i] = 8;
	}
}


static int __init cvm_oct_init_module(void)
{
	int num_interfaces;
	int interface;
	int fau = FAU_NUM_PACKET_BUFFERS_TO_FREE;
	int qos;

	octeon_mdiobus_force_mod_depencency();
	pr_notice("octeon-ethernet %s\n", OCTEON_ETHERNET_VERSION);

	cvmx_override_pko_queue_priority = cvm_oct_override_pko_queue_priority;

	if (OCTEON_IS_MODEL(OCTEON_CN52XX) || OCTEON_IS_MODEL(OCTEON_CN63XX))
		cvm_oct_mac_addr_offset = 2; /* First two are the mgmt ports. */
	else if (OCTEON_IS_MODEL(OCTEON_CN56XX))
		cvm_oct_mac_addr_offset = 1; /* First one is the mgmt port. */
	else
		cvm_oct_mac_addr_offset = 0;

	cvm_oct_poll_queue = create_singlethread_workqueue("octeon-ethernet");
	if (cvm_oct_poll_queue == NULL) {
		pr_err("octeon-ethernet: Cannot create workqueue");
		return -ENOMEM;
	}

	cvm_oct_proc_initialize();
	cvm_oct_configure_common_hw();

	cvmx_helper_initialize_packet_io_global();

	/* Change the input group for all ports before input is enabled */
	num_interfaces = cvmx_helper_get_number_of_interfaces();
	for (interface = 0; interface < num_interfaces; interface++) {
		int num_ports = cvmx_helper_ports_on_interface(interface);
		int port;

		for (port = cvmx_helper_get_ipd_port(interface, 0);
		     port < cvmx_helper_get_ipd_port(interface, num_ports);
		     port++) {
			union cvmx_pip_prt_tagx pip_prt_tagx;
			pip_prt_tagx.u64 = cvmx_read_csr(CVMX_PIP_PRT_TAGX(port));
			pip_prt_tagx.s.grp = pow_receive_group;
			cvmx_write_csr(CVMX_PIP_PRT_TAGX(port), pip_prt_tagx.u64);
		}
	}

	cvmx_helper_ipd_and_packet_input_enable();

	/*
	 * Initialize the FAU used for counting packet buffers that
	 * need to be freed.
	 */
	cvmx_fau_atomic_write32(FAU_NUM_PACKET_BUFFERS_TO_FREE, 0);

	if ((pow_send_group != -1)) {
		struct net_device *dev;
		pr_info("\tConfiguring device for POW only access\n");
		dev = alloc_etherdev(sizeof(struct octeon_ethernet));
		if (dev) {
			/* Initialize the device private structure. */
			struct octeon_ethernet *priv = netdev_priv(dev);

			priv->netdev = dev;
			dev->netdev_ops = &cvm_oct_pow_netdev_ops;
			priv->num_tx_queues = 0;
			priv->imode = CVMX_HELPER_INTERFACE_MODE_DISABLED;
			priv->port = CVMX_PIP_NUM_INPUT_PORTS;
			strcpy(dev->name, "pow%d");

			if (register_netdev(dev) < 0) {
				pr_err("Failed to register ethernet device for POW\n");
				kfree(dev);
			} else {
				list_add_tail(&priv->list, &cvm_oct_list);
				cvm_oct_by_port[CVMX_PIP_NUM_INPUT_PORTS] = priv;
				pr_info("%s: POW send group %d, receive group %d\n",
					dev->name, pow_send_group,
					pow_receive_group);
			}
		} else {
			pr_err("Failed to allocate ethernet device for POW\n");
		}
	}

	for (interface = 0; interface < num_interfaces; interface++) {
		cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
		int num_ports = cvmx_helper_ports_on_interface(interface);
		int port;

		if (imode == CVMX_HELPER_INTERFACE_MODE_SRIO)
			num_ports = 4;

		for (port = cvmx_helper_get_ipd_port(interface, 0);
		     port < cvmx_helper_get_ipd_port(interface, num_ports);
		     port++) {
			struct octeon_ethernet *priv;
			int base_queue;
			struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
			if (!dev) {
				pr_err("Failed to allocate ethernet device for port %d\n", port);
				continue;
			}

			if (disable_core_queueing)
				dev->tx_queue_len = 0;

			/* Initialize the device private structure. */
			priv = netdev_priv(dev);
			priv->netdev = dev;

			INIT_DELAYED_WORK(&priv->port_periodic_work,
					  cvm_oct_periodic_worker);
			priv->imode = imode;

			if (imode == CVMX_HELPER_INTERFACE_MODE_SRIO) {
                                int mbox = port - cvmx_helper_get_ipd_port(interface, 0);
				cvmx_srio_tx_message_header_t tx_header;
				tx_header.u64 = 0;
				tx_header.s.tt = 1;
				tx_header.s.ssize = 0xe;
				tx_header.s.mbox = mbox;
				tx_header.s.lns = 1;
                                tx_header.s.intr = 1;
				priv->srio_tx_header = tx_header.u64;
				priv->port = cvmx_helper_get_ipd_port(interface, mbox >> 1);
				base_queue = cvmx_pko_get_base_queue(priv->port) + (mbox & 1);
				priv->num_tx_queues = 1;
				cvm_oct_by_srio_mbox[interface - 4][mbox] = priv;
			} else {
				priv->port = port;
				base_queue = cvmx_pko_get_base_queue(priv->port);
				priv->num_tx_queues = cvmx_pko_get_num_queues(priv->port);
			}

			for (qos = 0; qos < priv->num_tx_queues; qos++) {
				priv->tx_queue[qos].queue = base_queue + qos;
				fau = fau - sizeof(uint32_t);
				priv->tx_queue[qos].fau = fau;
				cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
			}

			switch (priv->imode) {

			/* These types don't support ports to IPD/PKO */
			case CVMX_HELPER_INTERFACE_MODE_DISABLED:
			case CVMX_HELPER_INTERFACE_MODE_PCIE:
			case CVMX_HELPER_INTERFACE_MODE_PICMG:
				break;

			case CVMX_HELPER_INTERFACE_MODE_NPI:
				dev->netdev_ops = &cvm_oct_npi_netdev_ops;
				strcpy(dev->name, "npi%d");
				break;

			case CVMX_HELPER_INTERFACE_MODE_XAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
				if (octeon_pko_lockless()) {
					dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
					priv->flags |= OCTEON_ETHERNET_FLAG_TX_LOCKLESS;
				} else
#endif
					dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
				strcpy(dev->name, "xaui%d");
				break;

			case CVMX_HELPER_INTERFACE_MODE_LOOP:
				dev->netdev_ops = &cvm_oct_npi_netdev_ops;
				strcpy(dev->name, "loop%d");
				break;

			case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
				if (octeon_pko_lockless()) {
					dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
					priv->flags |= OCTEON_ETHERNET_FLAG_TX_LOCKLESS;
				} else
#endif
					dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
				strcpy(dev->name, "eth%d");
				break;

			case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
				if (octeon_pko_lockless()) {
					dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
					priv->flags |= OCTEON_ETHERNET_FLAG_TX_LOCKLESS;
				} else
#endif
					dev->netdev_ops = &cvm_oct_spi_netdev_ops;
				strcpy(dev->name, "spi%d");
				break;

			case CVMX_HELPER_INTERFACE_MODE_RGMII:
			case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
				if (octeon_pko_lockless()) {
					dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
					priv->flags |= OCTEON_ETHERNET_FLAG_TX_LOCKLESS;
				} else
#endif
					dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
				strcpy(dev->name, "eth%d");
				break;
#ifdef CONFIG_RAPIDIO
			case CVMX_HELPER_INTERFACE_MODE_SRIO:
				dev->netdev_ops = &cvm_oct_srio_netdev_ops;
				strcpy(dev->name, "rio%d");
				break;
#endif
			}

			if (!dev->netdev_ops) {
				kfree(dev);
			} else if (register_netdev(dev) < 0) {
				pr_err("Failed to register ethernet device for interface %d, port %d\n",
					 interface, priv->port);
				kfree(dev);
			} else {
				list_add_tail(&priv->list, &cvm_oct_list);
				cvm_oct_by_port[priv->port] = priv;
				/*
				 * Each transmit queue will need its
				 * own MAX_OUT_QUEUE_DEPTH worth of
				 * WQE to track the transmit skbs.
				 */
				cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
				num_devices_extra_wqe++;

				queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
			}

        	int interface = INTERFACE(priv->port);
        	int index = INDEX(priv->port);
        	if ((interface < 2)
        	    && (cvmx_helper_interface_get_mode(interface) !=
        		CVMX_HELPER_INTERFACE_MODE_SPI)) {
        		/* Add ethernet header and FCS, and VLAN if configured. */
        		int max_packet = 0x4000;

        		if (OCTEON_IS_MODEL(OCTEON_CN3XXX)
        		    || OCTEON_IS_MODEL(OCTEON_CN58XX)) {
        			/* Signal errors on packets larger than the MTU */
        			cvmx_write_csr(CVMX_GMXX_RXX_FRM_MAX(index, interface),
        				       max_packet);
        		} else {
        			/*
        			 * Set the hardware to truncate packets larger
        			 * than the MTU and smaller the 64 bytes.
        			 */
        			union cvmx_pip_frm_len_chkx frm_len_chk;
        			frm_len_chk.u64 = 0;
        			frm_len_chk.s.minlen = 64;
        			frm_len_chk.s.maxlen = max_packet;
        			cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(interface), frm_len_chk.u64);
        		}
        		/*
        		 * Set the hardware to truncate packets larger than
        		 * the MTU. The jabber register must be set to a
        		 * multiple of 8 bytes, so round up.
        		 */
        		cvmx_write_csr(CVMX_GMXX_RXX_JABBER(index, interface),
        			       (max_packet + 7) & ~7u);
        	}
		}
	}

	cvm_oct_rx_initialize();
	queue_delayed_work(cvm_oct_poll_queue, &cvm_oct_rx_refill_work, HZ);

	return 0;
}

static void __exit cvm_oct_cleanup_module(void)
{
	struct octeon_ethernet *priv;
	struct octeon_ethernet *tmp;

	/* Disable POW interrupt */
	cvmx_write_csr(CVMX_POW_WQ_INT_THRX(pow_receive_group), 0);

	cvmx_ipd_disable();

	atomic_inc_return(&cvm_oct_poll_queue_stopping);
	cancel_delayed_work_sync(&cvm_oct_rx_refill_work);

	cvm_oct_rx_shutdown();

	/* Free the ethernet devices */
	list_for_each_entry_safe_reverse(priv, tmp, &cvm_oct_list, list) {
		list_del(&priv->list);
		cancel_delayed_work_sync(&priv->port_periodic_work);
		unregister_netdev(priv->netdev);
		cvm_oct_by_port[priv->port] = NULL;
		kfree(priv->netdev);
	}

	destroy_workqueue(cvm_oct_poll_queue);

	cvm_oct_proc_shutdown();

	cvmx_helper_shutdown_packet_io_global();

	/* Free the HW pools */
	cvm_oct_mem_empty_fpa(CVMX_FPA_PACKET_POOL, CVMX_FPA_PACKET_POOL_SIZE, num_packet_buffers);
	cvm_oct_mem_empty_fpa(CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE, num_packet_buffers);
	cvm_oct_mem_empty_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, num_devices_extra_wqe * PER_DEVICE_EXTRA_WQE);

	if (CVMX_FPA_OUTPUT_BUFFER_POOL != CVMX_FPA_PACKET_POOL)
		cvm_oct_mem_empty_fpa(CVMX_FPA_OUTPUT_BUFFER_POOL, CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, cvm_oct_num_output_buffers);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon ethernet driver.");
module_init(cvm_oct_init_module);
module_exit(cvm_oct_cleanup_module);
