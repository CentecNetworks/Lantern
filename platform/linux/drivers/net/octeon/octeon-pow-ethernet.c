/*
 *   Octeon POW Ethernet Driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005-2007 Cavium Networks
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-fpa.h>
#include <asm/octeon/cvmx-pow.h>
#include <asm/octeon/cvmx-wqe.h>

#define NUM_GROUPS      16	/* Total number of groups in Octeon */
#define VIRTUAL_PORT    63	/* Value to put in work->ipprt */
#define IP_PROTOCOL_TCP 6	/* IP header protocol for TCP */
#define IP_PROTOCOL_UDP 0x11	/* IP header protocol for UDP */

#define DEBUGPRINT(format, ...) do { if (printk_ratelimit()) 		\
					printk(format, ##__VA_ARGS__);	\
				} while (0)

/* These are the Octeon CSR IO addresses we are going to need */
#define OCTEON_POW_WQ_INT_THRX(offset)  (0x8001670000000080ull+((offset)*8))
#define OCTEON_POW_WQ_INT               (0x8001670000000200ull)
#define OCTEON_POW_PP_GRP_MSKX(offset)  (0x8001670000000000ull+((offset)*8))
#define OCTEON_POW_WORK_SUBMIT(wqe)     (0x8001610000000000ull | (wqe))
#define OCTEON_POW_WORK_REQUEST(wait)   (0x8001600000000000ull | (wait<<3))
#define OCTEON_FPA_ALLOC(pool)          (0x8001280000000000ull | ((uint64_t)pool<<40))
#define OCTEON_FPA_FREE(pool, address)  (0x8001280000000000ull | ((uint64_t)pool<<40) | (address))
#define OCTEON_IPD_PACKET_MBUFF_SIZE    (0x80014F0000000010ull)
#define OCTEON_IPD_WQE_FPA_QUEUE        (0x80014F0000000020ull)
#define OCTEON_IPD_CTL_STATUS           (0x80014F0000000018ull)

int receive_group = -1;
module_param(receive_group, int, 0444);
MODULE_PARM_DESC(receive_group,
		 " 0-16 POW group to receive packets from. This must be unique in\n"
		 "\t\tthe system. If you don't specify a value, the core ID will\n"
		 "\t\tbe used.");

int broadcast_groups;
module_param(broadcast_groups, int, 0644);
MODULE_PARM_DESC(broadcast_groups,
		 " Bitmask of groups to send broadcasts to. This MUST be specified.\n"
		 "\t\tWARNING: Be careful to not send broadcasts to groups that aren't\n"
		 "\t\tread otherwise you may fill the POW and stop receiving packets.\n");

/**
 * This is the definition of the Ethernet driver's private
 * driver state.
 */
typedef struct {
	struct net_device_stats stats;	/* Device statistics */
} device_private_t;

static int fpa_wqe_pool = 1;	/* HW FPA pool to use for work queue entries */
static int fpa_packet_pool;	/* HW FPA pool to use for packet buffers */
static int fpa_packet_pool_size = 2048;	/* Size of the packet buffers */
static struct net_device *global_device;


/**
 * Given a packet data address, return a pointer to the
 * beginning of the packet buffer.
 *
 * @param packet_ptr Packet data hardware address
 * @return Packet buffer pointer
 */
static inline void *get_buffer_ptr(cvmx_buf_ptr_t packet_ptr)
{
	return phys_to_virt(((packet_ptr.s.addr >> 7) -
			     packet_ptr.s.back) << 7);
}


/**
 * Get a new block from the FPA
 *
 * @param pool   Pool to get the block from
 * @return Pointer to the block or NULL on failure
 */
static inline void *fpa_alloc(uint64_t pool)
{
	uint64_t address = cvmx_read_csr(OCTEON_FPA_ALLOC(pool));
	if (address)
		return phys_to_virt(address);
	else
		return NULL;
}


/**
 * Free a block allocated with a FPA pool.  Provides required memory
 * ordering in cases where memory block was modified by core.
 *
 * @param ptr    Block to free
 * @param pool   Pool to put it in
 * @param num_cache_lines
 *               Cache lines to invalidate
 */
static inline void fpa_free(void *ptr, int pool, int num_cache_lines)
{
	wmb();
	cvmx_write_csr(OCTEON_FPA_FREE(pool, virt_to_phys(ptr)),
		       num_cache_lines);
}


/**
 * Submits work to an input queue.  This function updates the work queue entry in DRAM to match
 * the arguments given.
 * Note that the tag provided is for the work queue entry submitted, and is unrelated to the tag that
 * the core currently holds.
 *
 * @param wqp      pointer to work queue entry to submit.  This entry is updated to match the other parameters
 * @param tag      tag value to be assigned to work queue entry
 * @param tag_type type of tag
 * @param qos      Input queue to add to.
 * @param grp      group value for the work queue entry.
 */
static inline void pow_work_submit(cvmx_wqe_t *wqp, uint32_t tag,
				   uint64_t tag_type, uint64_t qos,
				   uint64_t grp)
{
	uint64_t tag_req =
		(4ull << 44) | (qos << 39) | (grp << 35) | (tag_type << 32) |
		tag;
	wmb();
	cvmx_write_csr(OCTEON_POW_WORK_SUBMIT(virt_to_phys(wqp)), tag_req);
}


/**
 * Synchronous work request.  Requests work from the POW.
 * This function does NOT wait for previous tag switches to complete,
 * so the caller must ensure that there is not a pending tag switch.
 *
 * @param wait   When set, call stalls until work becomes avaiable, or times out.
 *               If not set, returns immediately.
 *
 * @return Returns the WQE pointer from POW. Returns NULL if no work was available.
 */
static inline cvmx_wqe_t *pow_work_request_sync(int wait)
{
	int64_t result = cvmx_read_csr(OCTEON_POW_WORK_REQUEST(wait));
	if (result < 0)
		return NULL;
	else
		return (cvmx_wqe_t *) phys_to_virt(result);
}


/**
 * Free a work queue entry received in a intercept callback.
 *
 * @param work_queue_entry
 *               Work queue entry to free
 * @return Zero on success, Negative on failure.
 */
static int free_work(cvmx_wqe_t *work)
{
	int segments = work->word2.s.bufs;
	cvmx_buf_ptr_t segment_ptr = work->packet_ptr;

	while (segments--) {
		cvmx_buf_ptr_t next_ptr =
			*(cvmx_buf_ptr_t *) phys_to_virt(segment_ptr.s.addr -
							   8);
		if (unlikely(!segment_ptr.s.i))
			fpa_free(get_buffer_ptr(segment_ptr),
				 segment_ptr.s.pool, 0);
		segment_ptr = next_ptr;
	}
	fpa_free(work, fpa_wqe_pool, 0);

	return 0;
}


/**
 * Packet transmit to the POW
 *
 * @param skb    Packet to send
 * @param dev    Device info structure
 * @return Always returns zero
 */
static int packet_transmit(struct sk_buff *skb, struct net_device *dev)
{
	device_private_t *priv;
	cvmx_wqe_t *work = NULL;
	void *packet_buffer = NULL;
	void *copy_location;
	int send_group_mask;
	int send_group;

        priv = (device_private_t*) netdev_priv(dev);

	/* Any unknown MAC address goes to all groups in the module param
	   broadcast_groups. Known MAC addresses use the low order dest mac
	   byte as the group number */
	if ((*(uint64_t *) (skb->data) >> 16) < 0x01ff)
		send_group_mask = 1 << (skb->data[5] & (NUM_GROUPS - 1));
	else
		send_group_mask = broadcast_groups;
	send_group_mask &= ~(1 << receive_group);

	/* It is ugly, but we need to send multiple times for broadcast
	   packets. The hardware doesn't support submitting work to multiple
	   groups */
	for (send_group = 0; send_group < NUM_GROUPS; send_group++) {
		/* Don't transmit to groups not in our send_group_mask */
		if (likely((send_group_mask & (1 << send_group)) == 0))
			continue;

		/* Get a work queue entry */
		work = fpa_alloc(fpa_wqe_pool);
		if (unlikely(work == NULL)) {
			DEBUGPRINT
				("%s: Failed to allocate a work queue entry\n",
				 dev->name);
			goto fail;
		}

		/* Get a packet buffer */
		packet_buffer = fpa_alloc(fpa_packet_pool);
		if (unlikely(packet_buffer == NULL)) {
			DEBUGPRINT("%s: Failed to allocate a packet buffer\n",
				   dev->name);
			goto fail;
		}

		/* Calculate where we need to copy the data to. We need to
		   leave 8 bytes for a next pointer (unused). Then we need to
		   align the IP packet src and dest into the same 64bit word. */
		copy_location = packet_buffer + sizeof(uint64_t) + 6;

		/* Fail if the packet won't fit in a single buffer */
		if (unlikely
		    (copy_location + skb->len >
		     packet_buffer + fpa_packet_pool_size)) {
			DEBUGPRINT("%s: Packet too large for FPA buffer\n",
				   dev->name);
			goto fail;
		}

		memcpy(copy_location, skb->data, skb->len);

		/* Fill in some of the work queue fields. We may need to add
		   more if the software at the other end needs them */
		work->hw_chksum = skb->csum;
		work->len = skb->len;
		work->ipprt = VIRTUAL_PORT;
		work->qos = 0;
		work->grp = send_group;
		work->tag_type = 2;
		work->tag = 0;
		work->word2.u64 = 0;	/* Default to zero. Sets of zero later
					   are commented out */
		work->word2.s.bufs = 1;
		work->packet_ptr.u64 = 0;
		work->packet_ptr.s.addr = virt_to_phys(copy_location);
		work->packet_ptr.s.pool = fpa_packet_pool;
		work->packet_ptr.s.size = fpa_packet_pool_size;
		work->packet_ptr.s.back = (copy_location - packet_buffer) >> 7;

		if (skb->protocol == htons(ETH_P_IP)) {
			work->word2.s.ip_offset = 14;
			#if 0
			work->word2.s.vlan_valid = 0;	/* FIXME */
			work->word2.s.vlan_cfi = 0;	/* FIXME */
			work->word2.s.vlan_id = 0;	/* FIXME */
			work->word2.s.dec_ipcomp = 0;	/* FIXME */
			#endif
			work->word2.s.tcp_or_udp =
				(ip_hdr(skb)->protocol == IP_PROTOCOL_TCP) ||
				(ip_hdr(skb)->protocol == IP_PROTOCOL_UDP);
			#if 0
			work->word2.s.dec_ipsec = 0; /* FIXME */
			work->word2.s.is_v6 = 0; /* We only support IPv4
						    right now */
			work->word2.s.software = 0; /* Hardware would set to
						       zero */
			work->word2.s.L4_error = 0; /* No error, packet is
						       internal */
			#endif
			work->word2.s.is_frag = !((ip_hdr(skb)->frag_off == 0)
						  || (ip_hdr(skb)->frag_off ==
						      1 << 14));
			#if 0
			work->word2.s.IP_exc = 0; /* Assume Linux is sending
						     a good packet */
			#endif
			work->word2.s.is_bcast =
				(skb->pkt_type == PACKET_BROADCAST);
			work->word2.s.is_mcast =
				(skb->pkt_type == PACKET_MULTICAST);
			#if 0
			work->word2.s.not_IP = 0; /* This is an IP packet */
			work->word2.s.rcv_error = 0; /* No error, packet is
							internal */
			work->word2.s.err_code = 0;  /* No error, packet is
							internal */
			#endif

			/* When copying the data, include 4 bytes of the
			   ethernet header to align the same way hardware does */
			memcpy(work->packet_data, skb->data + 10,
			       sizeof(work->packet_data));
		} else {
			#if 0
			work->word2.snoip.vlan_valid = 0; /* FIXME */
			work->word2.snoip.vlan_cfi = 0;   /* FIXME */
			work->word2.snoip.vlan_id = 0;    /* FIXME */
			work->word2.snoip.software = 0;   /* Hardware would
							     set to zero */
			#endif
			work->word2.snoip.is_rarp =
				skb->protocol == htons(ETH_P_RARP);
			work->word2.snoip.is_arp =
				skb->protocol == htons(ETH_P_ARP);
			work->word2.snoip.is_bcast =
				(skb->pkt_type == PACKET_BROADCAST);
			work->word2.snoip.is_mcast =
				(skb->pkt_type == PACKET_MULTICAST);
			work->word2.snoip.not_IP = 1;	/* IP was done up above */
			#if 0
			work->word2.snoip.rcv_error = 0; /* No error, packet
							    is internal */
			work->word2.snoip.err_code = 0;  /* No error, packet
							    is internal */
			#endif
			memcpy(work->packet_data, skb->data,
			       sizeof(work->packet_data));
		}

		/* Submit the packet to the POW */
		pow_work_submit(work, work->tag, work->tag_type, work->qos,
				work->grp);
		work = NULL;
		packet_buffer = NULL;
	}

	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;

fail:
	if (work)
		fpa_free(work, fpa_wqe_pool, 0);
	if (packet_buffer)
		fpa_free(packet_buffer, fpa_packet_pool, 0);
	priv->stats.tx_dropped++;
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}


/**
 * Interrupt handler. The interrupt occurs whenever the POW
 * transitions from 0->1 packets in our group.
 *
 * @param cpl
 * @param dev_id
 * @param regs
 * @return
 */
static irqreturn_t do_interrupt(int cpl, void *dev_id)
{
	const uint64_t coreid = cvmx_get_core_num();
	struct net_device *dev = (struct net_device *) dev_id;
	device_private_t *priv;
	uint64_t old_group_mask;
	cvmx_wqe_t *work;
	struct sk_buff *skb;

	priv = (device_private_t*) netdev_priv(dev);

	/* Make sure any userspace operations are complete */
	asm volatile ("synciobdma" : : : "memory");

	/* Acknowledge the interrupt */
	cvmx_write_csr(OCTEON_POW_WQ_INT, 0x10001 << receive_group);

	/* Only allow work for our group */
	old_group_mask = cvmx_read_csr(OCTEON_POW_PP_GRP_MSKX(coreid));
	cvmx_write_csr(OCTEON_POW_PP_GRP_MSKX(coreid), 1 << receive_group);

	while (1) {
		work = pow_work_request_sync(0);
		if (work == NULL)
			break;

		/* Silently drop packets that have the wrong input port */
		if (work->ipprt != VIRTUAL_PORT) {
			free_work(work);
			continue;
		}

		/* Silently drop packets if we aren't up */
		if ((dev->flags & IFF_UP) == 0) {
			free_work(work);
			continue;
		}

		/* Throw away all packets with receive errors */
		if (unlikely(work->word2.snoip.rcv_error)) {
			DEBUGPRINT
				("%s: Receive error code %d, packet dropped\n",
				 dev->name, work->word2.snoip.err_code);
			free_work(work);
			priv->stats.rx_errors++;
			continue;
		}

		/* We have to copy the packet. First allocate an skbuff for it */
		skb = dev_alloc_skb(work->len);
		if (!skb) {
			DEBUGPRINT
				("%s: Failed to allocate skbuff, packet dropped\n",
				 dev->name);
			free_work(work);
			priv->stats.rx_dropped++;
			continue;
		}

		/* Check if we've received a packet that was entirely stored
		   the work entry. This is untested */
		if (unlikely(work->word2.s.bufs == 0)) {
			DEBUGPRINT
				("%s: Received a work with work->word2.s.bufs=0, untested\n",
				 dev->name);
			memcpy(skb_put(skb, work->len), work->packet_data,
			       work->len);
		} else {
			int segments = work->word2.s.bufs;
			cvmx_buf_ptr_t segment_ptr = work->packet_ptr;
			int len = work->len;
			while (segments--) {
				cvmx_buf_ptr_t next_ptr =
					*(cvmx_buf_ptr_t *)
					phys_to_virt(segment_ptr.s.addr - 8);
				/* Octeon Errata PKI-100: The segment size is
				   wrong. Until it is fixed, calculate the
				   segment size based on the packet pool buffer
				   size. When it is fixed, the following line
				   should be replaced with this one: int
				   segment_size = segment_ptr.s.size; */
				int segment_size =
					fpa_packet_pool_size -
					(segment_ptr.s.addr -
					 (((segment_ptr.s.addr >> 7) -
					   segment_ptr.s.back) << 7));
				/* Don't copy more than what is left in the
				   packet */
				if (segment_size > len)
					segment_size = len;
				/* Copy the data into the packet */
				memcpy(skb_put(skb, segment_size),
				       phys_to_virt(segment_ptr.s.addr),
				       segment_size);
				/* Reduce the amount of bytes left to copy */
				len -= segment_size;
				segment_ptr = next_ptr;
			}
		}
		free_work(work);
		skb->protocol = eth_type_trans(skb, dev);
		skb->dev = dev;
		skb->ip_summed = CHECKSUM_NONE;
		priv->stats.rx_bytes += skb->len;
		priv->stats.rx_packets++;
		netif_rx(skb);
	}

	/* Restore the original POW group mask */
	cvmx_write_csr(OCTEON_POW_PP_GRP_MSKX(coreid), old_group_mask);
	return IRQ_HANDLED;
}


#ifdef CONFIG_NET_POLL_CONTROLLER
/**
 * This is called when the kernel needs to manually poll the
 * device. For Octeon, this is simply calling the interrupt
 * handler. We actually poll all the devices, not just the
 * one supplied.
 *
 * @param dev    Device to poll. Unused
 */
static void device_poll_controller(struct net_device *dev)
{
	do_interrupt(0, dev);
}
#endif


/**
 * Open a device for use. Device should be able to send and
 * receive packets after this is called.
 *
 * @param dev    Device to bring up
 * @return Zero on success
 */
static int device_open(struct net_device *dev)
{
	/* Clear the statistics whenever the interface is brought up */
	device_private_t *priv = (device_private_t *) netdev_priv(dev);
	memset(&priv->stats, 0, sizeof(priv->stats));
	return 0;
}


/**
 * Stop an ethernet device. No more packets should be
 * received from this device.
 *
 * @param dev    Device to bring down
 * @return Zero on success
 */
static int device_close(struct net_device *dev)
{
	/* Nothing to do */
	return 0;
}


/**
 * Get the low level ethernet statistics
 *
 * @param dev    Device to get the statistics from
 * @return Pointer to the statistics
 */
static struct net_device_stats *device_get_stats(struct net_device *dev)
{
	device_private_t *priv = (device_private_t *) netdev_priv(dev);
	return &priv->stats;
}


/**
 * Per network device initialization
 *
 * @param dev    Device to initialize
 * @return Zero on success
 */
static int device_init(struct net_device *dev)
{
	dev->features |= NETIF_F_LLTX;	/* We do our own locking, Linux doesn't
					   need to */
	dev->dev_addr[0] = 0;
	dev->dev_addr[1] = 0;
	dev->dev_addr[2] = 0;
	dev->dev_addr[3] = 0;
	dev->dev_addr[4] = 1;
	dev->dev_addr[5] = cvmx_get_core_num();
	return 0;
}

static const struct net_device_ops octeon_pow_netdev_ops ={
	.ndo_init = device_init,
	.ndo_open = device_open,
	.ndo_stop = device_close,
	.ndo_start_xmit = packet_transmit,
	.ndo_get_stats = device_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller =  device_poll_controller,
#endif
};


/**
 * Module/ driver initialization. Creates the linux network
 * devices.
 *
 * @return Zero on success
 */
static int __init ethernet_pow_init(void)
{
	device_private_t *priv;

	if ((receive_group >= NUM_GROUPS)) {
		printk("\n\nERROR: Invalid receive group. Must be 0-%d\n",
		       NUM_GROUPS - 1);
		return -1;
	}

	if (!broadcast_groups) {
		printk("\n\nERROR: You must specify a broadcast group mask.\n");
		return -1;
	}

	if ((broadcast_groups & ((1 << NUM_GROUPS) - 1)) != broadcast_groups) {
		printk("\n\nERROR: Invalid broadcast group mask.\n");
		return -1;
	}

	printk("Octeon POW only ethernet driver\n");

	/* If a receive group isn't specified, default to the core id */
	if (receive_group < 0)
		receive_group = cvmx_get_core_num();

	/* Setup is complete, create the virtual ethernet devices */
	global_device = alloc_etherdev(sizeof(device_private_t));
	if (global_device == NULL) {
		printk("\n\nERROR: Failed to allocate ethernet device\n");
		return -1;
	}

	global_device->netdev_ops = &octeon_pow_netdev_ops;
	strcpy(global_device->name, "oct%d");

	/* Initialize the device private structure. */
	priv = (device_private_t *) netdev_priv(global_device);
	memset(priv, 0, sizeof(device_private_t));

	/* Spin waiting for another core to setup all the hardware */
	printk("Waiting for another core to setup the IPD hardware...");
	while ((cvmx_read_csr(OCTEON_IPD_CTL_STATUS) & 1) == 0)
		mdelay(100);

	printk("Done\n");

	/* Read the configured size of the FPA packet buffers. This way we
	   don't need changes if someone chooses to use a different buffer size */
	fpa_packet_pool_size =
		(cvmx_read_csr(OCTEON_IPD_PACKET_MBUFF_SIZE) & 0xfff) * 8;

	/* Read the work queue pool */
	fpa_wqe_pool = cvmx_read_csr(OCTEON_IPD_WQE_FPA_QUEUE) & 7;

	if (register_netdev(global_device) < 0) {
		printk("\n\nERROR: Failed to register ethernet device\n");
		kfree(global_device);
		return -1;
	}

	/* Register an IRQ hander for to receive POW interrupts */
	if (request_irq(OCTEON_IRQ_WORKQ0 + receive_group, do_interrupt, IRQF_SHARED,
			"POW Ethernet", global_device)) {
	}

	/* Enable POW interrupt when our port has at least one packet */
	cvmx_write_csr(OCTEON_POW_WQ_INT_THRX(receive_group), 0x1001);
	return 0;
}


/**
 * Module / driver shutdown
 *
 * @return Zero on success
 */
static void __exit ethernet_pow_cleanup(void)
{
	/* Disable POW interrupt */
	cvmx_write_csr(OCTEON_POW_WQ_INT_THRX(receive_group), 0);

	/* Free the interrupt handler */
	free_irq(OCTEON_IRQ_WORKQ0 + receive_group, global_device);

	/* Free the ethernet devices */
	unregister_netdev(global_device);
	kfree(global_device);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon internal only POW ethernet driver.");
module_init(ethernet_pow_init);
module_exit(ethernet_pow_cleanup);
