/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2009 Cavium Networks
 */

#include <linux/capability.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/phy.h>
#include <linux/spinlock.h>
#include <linux/net_tstamp.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-clock.h>
#include <asm/octeon/cvmx-mixx-defs.h>
#include <asm/octeon/cvmx-agl-defs.h>
#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-helper-board.h>

#define DRV_NAME "octeon_mgmt"
#define DRV_VERSION "2.0"
#define DRV_DESCRIPTION \
	"Cavium Networks Octeon MII (management) port Network Driver"

#define OCTEON_MGMT_NAPI_WEIGHT 16

/*
 * Ring sizes that are powers of two allow for more efficient modulo
 * opertions.
 */
#define OCTEON_MGMT_RX_RING_SIZE 512
#define OCTEON_MGMT_TX_RING_SIZE 128

/* Allow 8 bytes for vlan and FCS. */
#define OCTEON_MGMT_RX_HEADROOM (ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN)

union mgmt_port_ring_entry {
	u64 d64;
	struct {
		u64    reserved_62_63:2;
		/* Length of the buffer/packet in bytes */
		u64    len:14;
		/* For TX, signals that the packet should be timestamped */
		u64    tstamp:1;
		/* The RX error code */
		u64    code:7;
#define RING_ENTRY_CODE_DONE 0xf
#define RING_ENTRY_CODE_MORE 0x10
		/* Physical address of the buffer */
		u64    addr:40;
	} s;
};

struct octeon_mgmt {
	struct net_device *netdev;
	int port;
	int irq;
	int has_rx_tstamp;
	u64 *tx_ring;
	dma_addr_t tx_ring_handle;
	unsigned int tx_next;
	unsigned int tx_next_clean;
	unsigned int tx_current_fill;
	/* The tx_list lock also protects the ring related variables */
	struct sk_buff_head tx_list;

	/* RX variables only touched in napi_poll.  No locking necessary. */
	u64 *rx_ring;
	dma_addr_t rx_ring_handle;
	unsigned int rx_next;
	unsigned int rx_next_fill;
	unsigned int rx_current_fill;
	struct sk_buff_head rx_list;

	spinlock_t lock;
	unsigned int last_duplex;
	unsigned int last_link;
	unsigned int last_speed;
	struct device *dev;
	struct napi_struct napi;
	struct tasklet_struct tx_clean_tasklet;
	struct phy_device *phydev;
};

static void octeon_mgmt_set_rx_irq(struct octeon_mgmt *p, int enable)
{
	int port = p->port;
	union cvmx_mixx_intena mix_intena;
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);
	mix_intena.u64 = cvmx_read_csr(CVMX_MIXX_INTENA(port));
	mix_intena.s.ithena = enable ? 1 : 0;
	cvmx_write_csr(CVMX_MIXX_INTENA(port), mix_intena.u64);
	spin_unlock_irqrestore(&p->lock, flags);
}

static void octeon_mgmt_set_tx_irq(struct octeon_mgmt *p, int enable)
{
	int port = p->port;
	union cvmx_mixx_intena mix_intena;
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);
	mix_intena.u64 = cvmx_read_csr(CVMX_MIXX_INTENA(port));
	mix_intena.s.othena = enable ? 1 : 0;
	cvmx_write_csr(CVMX_MIXX_INTENA(port), mix_intena.u64);
	spin_unlock_irqrestore(&p->lock, flags);
}

static inline void octeon_mgmt_enable_rx_irq(struct octeon_mgmt *p)
{
	octeon_mgmt_set_rx_irq(p, 1);
}

static inline void octeon_mgmt_disable_rx_irq(struct octeon_mgmt *p)
{
	octeon_mgmt_set_rx_irq(p, 0);
}

static inline void octeon_mgmt_enable_tx_irq(struct octeon_mgmt *p)
{
	octeon_mgmt_set_tx_irq(p, 1);
}

static inline void octeon_mgmt_disable_tx_irq(struct octeon_mgmt *p)
{
	octeon_mgmt_set_tx_irq(p, 0);
}

static unsigned int ring_max_fill(unsigned int ring_size)
{
	return ring_size - 8;
}

static unsigned int ring_size_to_bytes(unsigned int ring_size)
{
	return ring_size * sizeof(union mgmt_port_ring_entry);
}

static void octeon_mgmt_rx_fill_ring(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;

	while (p->rx_current_fill < ring_max_fill(OCTEON_MGMT_RX_RING_SIZE)) {
		unsigned int size;
		union mgmt_port_ring_entry re;
		struct sk_buff *skb;

		/* CN56XX pass 1 needs 8 bytes of padding.  */
		size = netdev->mtu + OCTEON_MGMT_RX_HEADROOM + 8 + NET_IP_ALIGN;

		skb = netdev_alloc_skb(netdev, size);
		if (!skb)
			break;
		skb_reserve(skb, NET_IP_ALIGN);
		__skb_queue_tail(&p->rx_list, skb);

		re.d64 = 0;
		re.s.len = size;
		re.s.addr = dma_map_single(p->dev, skb->data,
					   size,
					   DMA_FROM_DEVICE);

		/* Put it in the ring.  */
		p->rx_ring[p->rx_next_fill] = re.d64;
		dma_sync_single_for_device(p->dev, p->rx_ring_handle,
					   ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
					   DMA_BIDIRECTIONAL);
		p->rx_next_fill =
			(p->rx_next_fill + 1) % OCTEON_MGMT_RX_RING_SIZE;
		p->rx_current_fill++;
		/* Ring the bell.  */
		cvmx_write_csr(CVMX_MIXX_IRING2(port), 1);
	}
}

static ktime_t ptp_to_ktime(uint64_t ptptime)
{
	ktime_t ktimebase;
	uint64_t ptpbase;
	unsigned long flags;

	local_irq_save(flags);
	/* Fill the icache with the code */
	ktime_get_real();
	/* Flush all pending operations */
	CVMX_SYNC;
	/* Read the time and PTP clock as close together as possible. It is
	    important that this sequence take the same amount of time to
	    reduce jitter */
	ktimebase = ktime_get_real();
	ptpbase = octeon_read_ptp_csr(CVMX_MIO_PTP_CLOCK_HI);
	local_irq_restore(flags);

	return ktime_sub_ns(ktimebase, ptpbase - ptptime);
}

static void octeon_mgmt_clean_tx_buffers(struct octeon_mgmt *p)
{
	int port = p->port;
	union cvmx_mixx_orcnt mix_orcnt;
	union mgmt_port_ring_entry re;
	struct sk_buff *skb;
	int cleaned = 0;
	unsigned long flags;

	mix_orcnt.u64 = cvmx_read_csr(CVMX_MIXX_ORCNT(port));
	while (mix_orcnt.s.orcnt) {
		spin_lock_irqsave(&p->tx_list.lock, flags);

		mix_orcnt.u64 = cvmx_read_csr(CVMX_MIXX_ORCNT(port));

		if (mix_orcnt.s.orcnt == 0) {
			spin_unlock_irqrestore(&p->tx_list.lock, flags);
			break;
		}

		dma_sync_single_for_cpu(p->dev, p->tx_ring_handle,
					ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
					DMA_BIDIRECTIONAL);

		re.d64 = p->tx_ring[p->tx_next_clean];
		p->tx_next_clean =
			(p->tx_next_clean + 1) % OCTEON_MGMT_TX_RING_SIZE;
		skb = __skb_dequeue(&p->tx_list);

		mix_orcnt.u64 = 0;
		mix_orcnt.s.orcnt = 1;

		/* Acknowledge to hardware that we have the buffer.  */
		cvmx_write_csr(CVMX_MIXX_ORCNT(port), mix_orcnt.u64);
		p->tx_current_fill--;

		spin_unlock_irqrestore(&p->tx_list.lock, flags);

		dma_unmap_single(p->dev, re.s.addr, re.s.len,
				 DMA_TO_DEVICE);

		/* Read the hardware TX timestamp if one was recorded */
		if (unlikely(re.s.tstamp)) {
			struct skb_shared_hwtstamps ts;
			/* Read the timestamp */
			uint64_t ns = cvmx_read_csr(CVMX_MIXX_TSTAMP(p->port));
			/* Remove the timestamp from the FIFO */
			cvmx_write_csr(CVMX_MIXX_TSCTL(p->port), 0);
			/* Tell the kernel about the timestamp */
			ts.syststamp = ptp_to_ktime(ns);
			ts.hwtstamp = ns_to_ktime(ns);
			skb_tstamp_tx(skb, &ts);
		}

		dev_kfree_skb_any(skb);
		cleaned++;

		mix_orcnt.u64 = cvmx_read_csr(CVMX_MIXX_ORCNT(port));
	}

	if (cleaned && netif_queue_stopped(p->netdev))
		netif_wake_queue(p->netdev);
}

static void octeon_mgmt_clean_tx_tasklet(unsigned long arg)
{
	struct octeon_mgmt *p = (struct octeon_mgmt *)arg;
	octeon_mgmt_clean_tx_buffers(p);
	octeon_mgmt_enable_tx_irq(p);
}

static void octeon_mgmt_update_rx_stats(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	unsigned long flags;
	u64 drop, bad;

	/* These reads also clear the count registers.  */
	drop = cvmx_read_csr(CVMX_AGL_GMX_RXX_STATS_PKTS_DRP(port));
	bad = cvmx_read_csr(CVMX_AGL_GMX_RXX_STATS_PKTS_BAD(port));

	if (drop || bad) {
		/* Do an atomic update. */
		spin_lock_irqsave(&p->lock, flags);
		netdev->stats.rx_errors += bad;
		netdev->stats.rx_dropped += drop;
		spin_unlock_irqrestore(&p->lock, flags);
	}
}

static void octeon_mgmt_update_tx_stats(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	unsigned long flags;

	union cvmx_agl_gmx_txx_stat0 s0;
	union cvmx_agl_gmx_txx_stat1 s1;

	/* These reads also clear the count registers.  */
	s0.u64 = cvmx_read_csr(CVMX_AGL_GMX_TXX_STAT0(port));
	s1.u64 = cvmx_read_csr(CVMX_AGL_GMX_TXX_STAT1(port));

	if (s0.s.xsdef || s0.s.xscol || s1.s.scol || s1.s.mcol) {
		/* Do an atomic update. */
		spin_lock_irqsave(&p->lock, flags);
		netdev->stats.tx_errors += s0.s.xsdef + s0.s.xscol;
		netdev->stats.collisions += s1.s.scol + s1.s.mcol;
		spin_unlock_irqrestore(&p->lock, flags);
	}
}

/*
 * Dequeue a receive skb and its corresponding ring entry.  The ring
 * entry is returned, *pskb is updated to point to the skb.
 */
static u64 octeon_mgmt_dequeue_rx_buffer(struct octeon_mgmt *p,
					 struct sk_buff **pskb)
{
	union mgmt_port_ring_entry re;

	dma_sync_single_for_cpu(p->dev, p->rx_ring_handle,
				ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
				DMA_BIDIRECTIONAL);

	re.d64 = p->rx_ring[p->rx_next];
	p->rx_next = (p->rx_next + 1) % OCTEON_MGMT_RX_RING_SIZE;
	p->rx_current_fill--;
	*pskb = __skb_dequeue(&p->rx_list);

	dma_unmap_single(p->dev, re.s.addr,
			 ETH_FRAME_LEN + OCTEON_MGMT_RX_HEADROOM,
			 DMA_FROM_DEVICE);

	return re.d64;
}


static int octeon_mgmt_receive_one(struct octeon_mgmt *p)
{
	int port = p->port;
	struct net_device *netdev = p->netdev;
	union cvmx_mixx_ircnt mix_ircnt;
	union mgmt_port_ring_entry re;
	struct sk_buff *skb;
	struct sk_buff *skb2;
	struct sk_buff *skb_new;
	union mgmt_port_ring_entry re2;
	int rc = 1;

	re.d64 = octeon_mgmt_dequeue_rx_buffer(p, &skb);
	if (likely(re.s.code == RING_ENTRY_CODE_DONE)) {
		/* A good packet, send it up. */
		skb_put(skb, re.s.len);
good:
		/* Process the RX timestamp if it was recorded */
		if (p->has_rx_tstamp) {
			/* The first 8 bytes are the timestamp */
			uint64_t ns = *(uint64_t*)skb->data;
			struct skb_shared_hwtstamps *ts;
			ts = skb_hwtstamps(skb);
			ts->hwtstamp = ns_to_ktime(ns);
			ts->syststamp = ptp_to_ktime(ns);
			__skb_pull(skb, 8);
		}
		skb->protocol = eth_type_trans(skb, netdev);
		netdev->stats.rx_packets++;
		netdev->stats.rx_bytes += skb->len;
		netdev->last_rx = jiffies;
		netif_receive_skb(skb);
		rc = 0;
	} else if (re.s.code == RING_ENTRY_CODE_MORE) {
		/*
		 * Packet split across skbs.  This can happen if we
		 * increase the MTU.  Buffers that are already in the
		 * rx ring can then end up being too small.  As the rx
		 * ring is refilled, buffers sized for the new MTU
		 * will be used and we should go back to the normal
		 * non-split case.
		 */
		skb_put(skb, re.s.len);
		do {
			re2.d64 = octeon_mgmt_dequeue_rx_buffer(p, &skb2);
			if (re2.s.code != RING_ENTRY_CODE_MORE
				&& re2.s.code != RING_ENTRY_CODE_DONE)
				goto split_error;
			skb_put(skb2,  re2.s.len);
			skb_new = skb_copy_expand(skb, 0, skb2->len,
						  GFP_ATOMIC);
			if (!skb_new)
				goto split_error;
			if (skb_copy_bits(skb2, 0, skb_tail_pointer(skb_new),
					  skb2->len))
				goto split_error;
			skb_put(skb_new, skb2->len);
			dev_kfree_skb_any(skb);
			dev_kfree_skb_any(skb2);
			skb = skb_new;
		} while (re2.s.code == RING_ENTRY_CODE_MORE);
		goto good;
	} else {
		/* Some other error, discard it. */
		dev_kfree_skb_any(skb);
		/*
		 * Error statistics are accumulated in
		 * octeon_mgmt_update_rx_stats.
		 */
	}
	goto done;
split_error:
	/* Discard the whole mess. */
	dev_kfree_skb_any(skb);
	dev_kfree_skb_any(skb2);
	while (re2.s.code == RING_ENTRY_CODE_MORE) {
		re2.d64 = octeon_mgmt_dequeue_rx_buffer(p, &skb2);
		dev_kfree_skb_any(skb2);
	}
	netdev->stats.rx_errors++;

done:
	/* Tell the hardware we processed a packet.  */
	mix_ircnt.u64 = 0;
	mix_ircnt.s.ircnt = 1;
	cvmx_write_csr(CVMX_MIXX_IRCNT(port), mix_ircnt.u64);
	return rc;
}

static int octeon_mgmt_receive_packets(struct octeon_mgmt *p, int budget)
{
	int port = p->port;
	unsigned int work_done = 0;
	union cvmx_mixx_ircnt mix_ircnt;
	int rc;


	mix_ircnt.u64 = cvmx_read_csr(CVMX_MIXX_IRCNT(port));
	while (work_done < budget && mix_ircnt.s.ircnt) {

		rc = octeon_mgmt_receive_one(p);
		if (!rc)
			work_done++;

		/* Check for more packets. */
		mix_ircnt.u64 = cvmx_read_csr(CVMX_MIXX_IRCNT(port));
	}

	octeon_mgmt_rx_fill_ring(p->netdev);

	return work_done;
}

static int octeon_mgmt_napi_poll(struct napi_struct *napi, int budget)
{
	struct octeon_mgmt *p = container_of(napi, struct octeon_mgmt, napi);
	struct net_device *netdev = p->netdev;
	unsigned int work_done = 0;

	work_done = octeon_mgmt_receive_packets(p, budget);

	if (work_done < budget) {
		/* We stopped because no more packets were available. */
		napi_complete(napi);
		octeon_mgmt_enable_rx_irq(p);
	}
	octeon_mgmt_update_rx_stats(netdev);

	return work_done;
}

/* Reset the hardware to clean state.  */
static void octeon_mgmt_reset_hw(struct octeon_mgmt *p)
{
	union cvmx_mixx_ctl mix_ctl;
	union cvmx_mixx_bist mix_bist;
	union cvmx_agl_gmx_bist agl_gmx_bist;

	mix_ctl.u64 = 0;
	cvmx_write_csr(CVMX_MIXX_CTL(p->port), mix_ctl.u64);
	do {
		mix_ctl.u64 = cvmx_read_csr(CVMX_MIXX_CTL(p->port));
	} while (mix_ctl.s.busy);
	mix_ctl.s.reset = 1;
	cvmx_write_csr(CVMX_MIXX_CTL(p->port), mix_ctl.u64);
	cvmx_read_csr(CVMX_MIXX_CTL(p->port));
	octeon_io_clk_delay(64);

	mix_bist.u64 = cvmx_read_csr(CVMX_MIXX_BIST(p->port));
	if (mix_bist.u64)
		dev_warn(p->dev, "MIX failed BIST (0x%016llx)\n",
			(unsigned long long)mix_bist.u64);

	agl_gmx_bist.u64 = cvmx_read_csr(CVMX_AGL_GMX_BIST);
	if (agl_gmx_bist.u64)
		dev_warn(p->dev, "AGL failed BIST (0x%016llx)\n",
			 (unsigned long long)agl_gmx_bist.u64);
}

struct octeon_mgmt_cam_state {
	u64 cam[6];
	u64 cam_mask;
	int cam_index;
};

static void octeon_mgmt_cam_state_add(struct octeon_mgmt_cam_state *cs,
				      unsigned char *addr)
{
	int i;

	for (i = 0; i < 6; i++)
		cs->cam[i] |= (u64)addr[i] << (8 * (cs->cam_index));
	cs->cam_mask |= (1ULL << cs->cam_index);
	cs->cam_index++;
}

static void octeon_mgmt_set_rx_filtering(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	int i;
	union cvmx_agl_gmx_rxx_adr_ctl adr_ctl;
	union cvmx_agl_gmx_prtx_cfg agl_gmx_prtx;
	unsigned long flags;
	unsigned int prev_packet_enable;
	unsigned int cam_mode = 1; /* 1 - Accept on CAM match */
	unsigned int multicast_mode = 1; /* 1 - Reject all multicast.  */
	struct octeon_mgmt_cam_state cam_state;
	struct dev_addr_list *list;
	struct list_head *pos;
	int available_cam_entries;

	memset(&cam_state, 0, sizeof(cam_state));

	if ((netdev->flags & IFF_PROMISC) || netdev->uc.count > 7) {
		cam_mode = 0;
		available_cam_entries = 8;
	} else {
		/*
		 * One CAM entry for the primary address, leaves seven
		 * for the secondary addresses.
		 */
		available_cam_entries = 7 - netdev->uc.count;
	}

	if (netdev->flags & IFF_MULTICAST) {
		if (cam_mode == 0 || (netdev->flags & IFF_ALLMULTI)
		    || netdev->mc_count  > available_cam_entries)
			multicast_mode = 2; /* 2 - Accept all multicast.  */
		else
			multicast_mode = 0; /* 0 - Use CAM.  */
	}

	if (cam_mode == 1) {
		/* Add primary address. */
		octeon_mgmt_cam_state_add(&cam_state, netdev->dev_addr);
		list_for_each(pos, &netdev->uc.list) {
			struct netdev_hw_addr *hw_addr;
			hw_addr = list_entry(pos, struct netdev_hw_addr, list);
			octeon_mgmt_cam_state_add(&cam_state, hw_addr->addr);
		}
	}
	if (multicast_mode == 0) {
		i = netdev->mc_count;
		list = netdev->mc_list;
		while (i--) {
			octeon_mgmt_cam_state_add(&cam_state, list->da_addr);
			list = list->next;
		}
	}


	spin_lock_irqsave(&p->lock, flags);

	/* Disable packet I/O. */
	agl_gmx_prtx.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));
	prev_packet_enable = agl_gmx_prtx.s.en;
	agl_gmx_prtx.s.en = 0;
	cvmx_write_csr(CVMX_AGL_GMX_PRTX_CFG(port), agl_gmx_prtx.u64);


	adr_ctl.u64 = 0;
	adr_ctl.s.cam_mode = cam_mode;
	adr_ctl.s.mcst = multicast_mode;
	adr_ctl.s.bcst = 1;     /* Allow broadcast */

	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CTL(port), adr_ctl.u64);

	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM0(port), cam_state.cam[0]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM1(port), cam_state.cam[1]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM2(port), cam_state.cam[2]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM3(port), cam_state.cam[3]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM4(port), cam_state.cam[4]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM5(port), cam_state.cam[5]);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_ADR_CAM_EN(port), cam_state.cam_mask);

	/* Restore packet I/O. */
	agl_gmx_prtx.s.en = prev_packet_enable;
	cvmx_write_csr(CVMX_AGL_GMX_PRTX_CFG(port), agl_gmx_prtx.u64);

	spin_unlock_irqrestore(&p->lock, flags);
}

static int octeon_mgmt_set_mac_address(struct net_device *netdev, void *addr)
{
	struct sockaddr *sa = addr;

	if (!is_valid_ether_addr(sa->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(netdev->dev_addr, sa->sa_data, ETH_ALEN);

	octeon_mgmt_set_rx_filtering(netdev);

	return 0;
}

static int octeon_mgmt_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	int size_without_fcs = new_mtu + OCTEON_MGMT_RX_HEADROOM;

	/*
	 * Limit the MTU to make sure the ethernet packets are between
	 * 64 bytes and 16383 bytes.
	 */
	if (size_without_fcs < 64 || size_without_fcs > 16383) {
		dev_warn(p->dev, "MTU must be between %d and %d.\n",
			 64 - OCTEON_MGMT_RX_HEADROOM,
			 16383 - OCTEON_MGMT_RX_HEADROOM);
		return -EINVAL;
	}

	netdev->mtu = new_mtu;

	cvmx_write_csr(CVMX_AGL_GMX_RXX_FRM_MAX(port), size_without_fcs);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_JABBER(port),
		       (size_without_fcs + 7) & 0xfff8);

	return 0;
}

static irqreturn_t octeon_mgmt_interrupt(int cpl, void *dev_id)
{
	struct net_device *netdev = dev_id;
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	union cvmx_mixx_isr mixx_isr;

	mixx_isr.u64 = cvmx_read_csr(CVMX_MIXX_ISR(port));

	/* Clear any pending interrupts */
	cvmx_write_csr(CVMX_MIXX_ISR(port), mixx_isr.u64);
	cvmx_read_csr(CVMX_MIXX_ISR(port));

	if (mixx_isr.s.irthresh) {
		octeon_mgmt_disable_rx_irq(p);
		napi_schedule(&p->napi);
	}
	if (mixx_isr.s.orthresh) {
		octeon_mgmt_disable_tx_irq(p);
		tasklet_schedule(&p->tx_clean_tasklet);
	}

	return IRQ_HANDLED;
}

static int octeon_mgmt_ioctl_hwtstamp(struct net_device *netdev,
	struct ifreq *rq, int cmd)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	struct hwtstamp_config config;
	union cvmx_mio_ptp_clock_cfg ptp;
	int have_hw_timestamps = 0;

	if (copy_from_user(&config, rq->ifr_data, sizeof(config)))
		return -EFAULT;

	if (config.flags) /* reserved for future extensions */
		return -EINVAL;

	/* Check the status of hardware for tiemstamps */
	if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		/* Get the current state of the PTP clock */
		ptp.u64 = octeon_read_ptp_csr(CVMX_MIO_PTP_CLOCK_CFG);
		if (!ptp.s.ext_clk_en) {
			/*
			 * The clock has not been configured to use an
			 * external source.  Program it to use the main clock
			 * reference.
			 */
			unsigned long long clock_comp = (NSEC_PER_SEC << 32) /
				octeon_get_io_clock_rate();
			if (!ptp.s.ptp_en)
				cvmx_write_csr(CVMX_MIO_PTP_CLOCK_COMP, clock_comp);
			pr_info("PTP Clock: Using sclk reference at %lld Hz\n",
				(NSEC_PER_SEC << 32) / clock_comp);
		} else {
			/* The clock is already programmed to use a GPIO */
			unsigned long long clock_comp = octeon_read_ptp_csr(
				CVMX_MIO_PTP_CLOCK_COMP);
			pr_info("PTP Clock: Using GPIO %d at %lld Hz\n",
				ptp.s.ext_clk_in,
				(NSEC_PER_SEC << 32) / clock_comp);
		}

		/* Enable the clock if it wasn't done already */
		if (!ptp.s.ptp_en) {
			ptp.s.ptp_en = 1;
			cvmx_write_csr(CVMX_MIO_PTP_CLOCK_CFG, ptp.u64);
		}
		have_hw_timestamps = 1;
	}

	if (!have_hw_timestamps)
		return -EINVAL;

	switch (config.tx_type) {
		case HWTSTAMP_TX_OFF:
		case HWTSTAMP_TX_ON:
			break;
		default:
			return -ERANGE;
	}

	switch (config.rx_filter) {
		case HWTSTAMP_FILTER_NONE:
			p->has_rx_tstamp = 0;
			if (have_hw_timestamps) {
				union cvmx_agl_gmx_rxx_frm_ctl rxx_frm_ctl;
				rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_AGL_GMX_RXX_FRM_CTL(p->port));
				rxx_frm_ctl.s.ptp_mode = 0;
				cvmx_write_csr(CVMX_AGL_GMX_RXX_FRM_CTL(p->port), rxx_frm_ctl.u64);
			}
			break;
		case HWTSTAMP_FILTER_ALL:
		case HWTSTAMP_FILTER_SOME:
		case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
			p->has_rx_tstamp = have_hw_timestamps;
			config.rx_filter = HWTSTAMP_FILTER_ALL;
			if (p->has_rx_tstamp) {
				union cvmx_agl_gmx_rxx_frm_ctl rxx_frm_ctl;
				rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_AGL_GMX_RXX_FRM_CTL(p->port));
				rxx_frm_ctl.s.ptp_mode = 1;
				cvmx_write_csr(CVMX_AGL_GMX_RXX_FRM_CTL(p->port), rxx_frm_ctl.u64);
			}
			break;
		default:
			return -ERANGE;
	}

	if (copy_to_user(rq->ifr_data, &config, sizeof(config)))
		return -EFAULT;

	return 0;
}

static int octeon_mgmt_ioctl(struct net_device *netdev,
			     struct ifreq *rq, int cmd)
{
	struct octeon_mgmt *p = netdev_priv(netdev);

	switch (cmd) {
	case SIOCSHWTSTAMP:
		return octeon_mgmt_ioctl_hwtstamp(netdev, rq, cmd);
	default:
		if (p->phydev)
			return phy_mii_ioctl(p->phydev, if_mii(rq), cmd);
		return -EINVAL;
	}
}

static void octeon_mgmt_disable_link(int port)
{
	union cvmx_agl_gmx_prtx_cfg prtx_cfg;

	/* Disable GMX before we make any changes. */
	prtx_cfg.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));
	prtx_cfg.s.en = 0;
	prtx_cfg.s.tx_en = 0;
	prtx_cfg.s.rx_en = 0;
	cvmx_write_csr(CVMX_AGL_GMX_PRTX_CFG(port), prtx_cfg.u64);

	if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		int i;
		for (i = 0; i < 10; i++) {
			prtx_cfg.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));
			if (prtx_cfg.s.tx_idle == 1 || prtx_cfg.s.rx_idle == 1)
				break;
			mdelay(1);
			i++;
		}
	}
}

static void octeon_mgmt_enable_link(int port)
{
	union cvmx_agl_gmx_prtx_cfg prtx_cfg;

	/* Restore the GMX enable state only if link is set */
	prtx_cfg.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));
	prtx_cfg.s.tx_en = 1;
	prtx_cfg.s.rx_en = 1;
	prtx_cfg.s.en = 1;
	cvmx_write_csr(CVMX_AGL_GMX_PRTX_CFG(port), prtx_cfg.u64);
}

static void octeon_mgmt_update_link(int port, struct phy_device *phydev)
{
	union cvmx_agl_gmx_prtx_cfg prtx_cfg;

	prtx_cfg.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));

	if (!phydev->link)
		prtx_cfg.s.duplex = 1;
	else
		prtx_cfg.s.duplex = phydev->duplex;

	switch (phydev->speed) {
	case 10:
		prtx_cfg.s.speed = 0;
		prtx_cfg.s.slottime = 0;

		if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
			prtx_cfg.s.burst = 1;
			prtx_cfg.s.speed_msb = 1;
		}
		break;
	case 100:
		prtx_cfg.s.speed = 0;
		prtx_cfg.s.slottime = 0;

		if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
			prtx_cfg.s.burst = 1;
			prtx_cfg.s.speed_msb = 0;
		}
		break;
	case 1000:
		/* 1000 MBits is only supported on 6XXX chips */
		if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
			prtx_cfg.s.speed = 1;
			prtx_cfg.s.speed_msb = 0;
			/* Only matters for half-duplex */
			prtx_cfg.s.slottime = 1;
			prtx_cfg.s.burst = phydev->duplex;
		}
		break;
	case 0:  /* No link */
	default:
		break;
	}

	/* Write the new GMX setting with the port still disabled. */
	cvmx_write_csr(CVMX_AGL_GMX_PRTX_CFG(port), prtx_cfg.u64);

	/* Read GMX CFG again to make sure the config is completed. */
	prtx_cfg.u64 = cvmx_read_csr(CVMX_AGL_GMX_PRTX_CFG(port));

	if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		cvmx_agl_gmx_txx_clk_t agl_clk;
		cvmx_agl_prtx_ctl_t prtx_ctl;

		prtx_ctl.u64 = cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));
		agl_clk.u64 = cvmx_read_csr(CVMX_AGL_GMX_TXX_CLK(port));
		/* MII (both speeds) and RGMII 1000 speed. */
		agl_clk.s.clk_cnt = 1;
		if (prtx_ctl.s.mode == 0) { /* RGMII mode */
			if (phydev->speed == 10)
				agl_clk.s.clk_cnt = 50;
			else if (phydev->speed == 100)
				agl_clk.s.clk_cnt = 5;
		}
		cvmx_write_csr(CVMX_AGL_GMX_TXX_CLK(port), agl_clk.u64);
	}
}

static void octeon_mgmt_adjust_link(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	unsigned long flags;
	int link_changed = 0;

	if (!p->phydev)
		return;

	spin_lock_irqsave(&p->lock, flags);


	if (!p->phydev->link && p->last_link)
		link_changed = -1;

	if (p->phydev->link
	    && (p->last_duplex != p->phydev->duplex
		|| p->last_link != p->phydev->link
		|| p->last_speed != p->phydev->speed)) {
		octeon_mgmt_disable_link(port);
		link_changed = 1;
		octeon_mgmt_update_link(port, p->phydev);
		octeon_mgmt_enable_link(port);
	}

	p->last_link = p->phydev->link;
	p->last_speed = p->phydev->speed;
	p->last_duplex = p->phydev->duplex;

	spin_unlock_irqrestore(&p->lock, flags);

	if (link_changed != 0) {
		if (link_changed > 0) {
			netif_carrier_on(netdev);
			pr_info("%s: Link is up - %d/%s\n", netdev->name,
				p->phydev->speed,
				DUPLEX_FULL == p->phydev->duplex ?
				"Full" : "Half");
		} else {
			netif_carrier_off(netdev);
			pr_info("%s: Link is down\n", netdev->name);
		}
	}
}

static int octeon_mgmt_init_phy(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	char phy_id[20];
	int phy_addr;
	u32 flags;

	if (octeon_is_simulation()) {
		/* No PHYs in the simulator. */
		netif_carrier_on(netdev);
		return 0;
	}

	phy_addr = cvmx_helper_board_get_mii_address(p->port +
					CVMX_HELPER_BOARD_MGMT_IPD_PORT);
	if (phy_addr != -1) {
		snprintf(phy_id, sizeof(phy_id), PHY_ID_FMT, "0", phy_addr);

		flags = 0;

		/*
		 * The 88E1118 Ethernet PHYs on the EBB6300 need the
		 * M1118_DEV_FLAGS_CLK_TRANSITION flag set.
		 */
		if (octeon_bootinfo->board_type == CVMX_BOARD_TYPE_EBB6300)
			flags |= 3;

		p->phydev = phy_connect(netdev, phy_id, octeon_mgmt_adjust_link,
					flags, PHY_INTERFACE_MODE_MII);

		if (IS_ERR(p->phydev)) {
			p->phydev = NULL;
			return -1;
		}
	}

	return 0;
}

static int octeon_mgmt_open(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	union cvmx_mixx_ctl mix_ctl;
	union cvmx_agl_gmx_inf_mode agl_gmx_inf_mode;
	union cvmx_mixx_oring1 oring1;
	union cvmx_mixx_iring1 iring1;
	union cvmx_agl_gmx_rxx_frm_ctl rxx_frm_ctl;
	union cvmx_mixx_irhwm mix_irhwm;
	union cvmx_mixx_orhwm mix_orhwm;
	union cvmx_mixx_intena mix_intena;
	struct sockaddr sa;

	/* Allocate ring buffers.  */
	p->tx_ring = kzalloc(ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
			     GFP_KERNEL);
	if (!p->tx_ring)
		return -ENOMEM;
	p->tx_ring_handle =
		dma_map_single(p->dev, p->tx_ring,
			       ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
			       DMA_BIDIRECTIONAL);
	p->tx_next = 0;
	p->tx_next_clean = 0;
	p->tx_current_fill = 0;


	p->rx_ring = kzalloc(ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
			     GFP_KERNEL);
	if (!p->rx_ring)
		goto err_nomem;
	p->rx_ring_handle =
		dma_map_single(p->dev, p->rx_ring,
			       ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
			       DMA_BIDIRECTIONAL);

	p->rx_next = 0;
	p->rx_next_fill = 0;
	p->rx_current_fill = 0;

	octeon_mgmt_reset_hw(p);

	mix_ctl.u64 = cvmx_read_csr(CVMX_MIXX_CTL(port));

	/* Bring it out of reset if needed. */
	if (mix_ctl.s.reset) {
		mix_ctl.s.reset = 0;
		cvmx_write_csr(CVMX_MIXX_CTL(port), mix_ctl.u64);
		do {
			mix_ctl.u64 = cvmx_read_csr(CVMX_MIXX_CTL(port));
		} while (mix_ctl.s.reset);
	}

	if (OCTEON_IS_MODEL(OCTEON_CN5XXX)) {
		agl_gmx_inf_mode.u64 = 0;
		agl_gmx_inf_mode.s.en = 1;
		cvmx_write_csr(CVMX_AGL_GMX_INF_MODE, agl_gmx_inf_mode.u64);
	}
	if (OCTEON_IS_MODEL(OCTEON_CN56XX_PASS1_X)
		|| OCTEON_IS_MODEL(OCTEON_CN52XX_PASS1_X)) {
		/*
		 * Force compensation values, as they are not
		 * determined properly by HW
		 */
		union cvmx_agl_gmx_drv_ctl drv_ctl;

		drv_ctl.u64 = cvmx_read_csr(CVMX_AGL_GMX_DRV_CTL);
		if (port) {
			drv_ctl.s.byp_en1 = 1;
			drv_ctl.s.nctl1 = 6;
			drv_ctl.s.pctl1 = 6;
		} else {
			drv_ctl.s.byp_en = 1;
			drv_ctl.s.nctl = 6;
			drv_ctl.s.pctl = 6;
		}
		cvmx_write_csr(CVMX_AGL_GMX_DRV_CTL, drv_ctl.u64);
	}

	oring1.u64 = 0;
	oring1.s.obase = p->tx_ring_handle >> 3;
	oring1.s.osize = OCTEON_MGMT_TX_RING_SIZE;
	cvmx_write_csr(CVMX_MIXX_ORING1(port), oring1.u64);

	iring1.u64 = 0;
	iring1.s.ibase = p->rx_ring_handle >> 3;
	iring1.s.isize = OCTEON_MGMT_RX_RING_SIZE;
	cvmx_write_csr(CVMX_MIXX_IRING1(port), iring1.u64);

	memcpy(sa.sa_data, netdev->dev_addr, ETH_ALEN);
	octeon_mgmt_set_mac_address(netdev, &sa);

	octeon_mgmt_change_mtu(netdev, netdev->mtu);

	/*
	 * Enable the port HW. Packets are not allowed until
	 * cvmx_mgmt_port_enable() is called.
	 */
	mix_ctl.u64 = 0;
	mix_ctl.s.crc_strip = 1;    /* Strip the ending CRC */
	mix_ctl.s.en = 1;           /* Enable the port */
	mix_ctl.s.nbtarb = 0;       /* Arbitration mode */
	/* MII CB-request FIFO programmable high watermark */
	mix_ctl.s.mrq_hwm = 1;
	cvmx_write_csr(CVMX_MIXX_CTL(port), mix_ctl.u64);

	/* Read the PHY to find the mode of the interface. */
	if (octeon_mgmt_init_phy(netdev)) {
		dev_err(p->dev, "Cannot initialize PHY on MIX%d.\n", p->port);
		goto err_noirq;
	}

	/* Set the mode of the interface, RGMII/MII. */
	if (OCTEON_IS_MODEL(OCTEON_CN6XXX) && p->phydev) {
		cvmx_agl_prtx_ctl_t agl_prtx_ctl;
		int rgmii_mode = (p->phydev->supported &
				  (SUPPORTED_1000baseT_Half | SUPPORTED_1000baseT_Full)) != 0;

		agl_prtx_ctl.u64 = cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));
		agl_prtx_ctl.s.mode = rgmii_mode ? 0 : 1;
		cvmx_write_csr(CVMX_AGL_PRTX_CTL(port),	agl_prtx_ctl.u64);

		/*
		 * MII clocks counts are based on the 125Mhz
		 * reference, which has an 8nS period. So our delays
		 * need to be multiplied by this factor.
		 */
#define NS_PER_PHY_CLK 8

		/* Take the DLL and clock tree out of reset */
		agl_prtx_ctl.u64 = cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));
		agl_prtx_ctl.s.clkrst = 0;
		if (rgmii_mode) {
			agl_prtx_ctl.s.dllrst = 0;
			agl_prtx_ctl.s.clktx_byp = 0;
		}
		cvmx_write_csr(CVMX_AGL_PRTX_CTL(port),	agl_prtx_ctl.u64);
		cvmx_read_csr(CVMX_AGL_PRTX_CTL(port)); /* Force write out before wait */

		/*
		 * Wait for the DLL to lock. External 125 MHz
		 * reference clock must be stable at this point.
		 */
		ndelay(256 * NS_PER_PHY_CLK);

		/* Enable the interface */
		agl_prtx_ctl.u64 = cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));
		agl_prtx_ctl.s.enable = 1;
		cvmx_write_csr(CVMX_AGL_PRTX_CTL(port), agl_prtx_ctl.u64);

		/* Read the value back to force the previous write */
		agl_prtx_ctl.u64 = cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));

		/* Enable the componsation controller */
		agl_prtx_ctl.s.comp = 1;
		agl_prtx_ctl.s.drv_byp = 0;
		cvmx_write_csr(CVMX_AGL_PRTX_CTL(port),	agl_prtx_ctl.u64);
		/* Force write out before wait. */
		cvmx_read_csr(CVMX_AGL_PRTX_CTL(port));

		/* For compensation state to lock. */
		ndelay(1040 * NS_PER_PHY_CLK);
	}

	octeon_mgmt_rx_fill_ring(netdev);

	/* Clear statistics. */
	/* Clear on read. */
	cvmx_write_csr(CVMX_AGL_GMX_RXX_STATS_CTL(port), 1);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_STATS_PKTS_DRP(port), 0);
	cvmx_write_csr(CVMX_AGL_GMX_RXX_STATS_PKTS_BAD(port), 0);

	cvmx_write_csr(CVMX_AGL_GMX_TXX_STATS_CTL(port), 1);
	cvmx_write_csr(CVMX_AGL_GMX_TXX_STAT0(port), 0);
	cvmx_write_csr(CVMX_AGL_GMX_TXX_STAT1(port), 0);

	/* Clear any pending interrupts */
	cvmx_write_csr(CVMX_MIXX_ISR(port), cvmx_read_csr(CVMX_MIXX_ISR(port)));

	if (request_irq(p->irq, octeon_mgmt_interrupt, 0, netdev->name,
			netdev)) {
		dev_err(p->dev, "request_irq(%d) failed.\n", p->irq);
		goto err_noirq;
	}

	/* Interrupt every single RX packet */
	mix_irhwm.u64 = 0;
	mix_irhwm.s.irhwm = 0;
	cvmx_write_csr(CVMX_MIXX_IRHWM(port), mix_irhwm.u64);

	/* Interrupt when we have 1 or more packets to clean.  */
	mix_orhwm.u64 = 0;
	mix_orhwm.s.orhwm = 0;
	cvmx_write_csr(CVMX_MIXX_ORHWM(port), mix_orhwm.u64);

	/* Enable receive and transmit interrupts */
	mix_intena.u64 = 0;
	mix_intena.s.ithena = 1;
	mix_intena.s.othena = 1;
	cvmx_write_csr(CVMX_MIXX_INTENA(port), mix_intena.u64);

	/* Enable packet I/O. */

	rxx_frm_ctl.u64 = 0;
	rxx_frm_ctl.s.ptp_mode = p->has_rx_tstamp;
	rxx_frm_ctl.s.pre_align = 1;
	/*
	 * When set, disables the length check for non-min sized pkts
	 * with padding in the client data.
	 */
	rxx_frm_ctl.s.pad_len = 1;
	/* When set, disables the length check for VLAN pkts */
	rxx_frm_ctl.s.vlan_len = 1;
	/* When set, PREAMBLE checking is  less strict */
	rxx_frm_ctl.s.pre_free = 1;
	/* Control Pause Frames can match station SMAC */
	rxx_frm_ctl.s.ctl_smac = 0;
	/* Control Pause Frames can match globally assign Multicast address */
	rxx_frm_ctl.s.ctl_mcst = 1;
	/* Forward pause information to TX block */
	rxx_frm_ctl.s.ctl_bck = 1;
	/* Drop Control Pause Frames */
	rxx_frm_ctl.s.ctl_drp = 1;
	/* Strip off the preamble */
	rxx_frm_ctl.s.pre_strp = 1;
	/*
	 * This port is configured to send PREAMBLE+SFD to begin every
	 * frame.  GMX checks that the PREAMBLE is sent correctly.
	 */
	rxx_frm_ctl.s.pre_chk = 1;
	cvmx_write_csr(CVMX_AGL_GMX_RXX_FRM_CTL(port), rxx_frm_ctl.u64);

	/* Configure the port duplex, speed and enables */
	octeon_mgmt_disable_link(p->port);
	if (p->phydev)
		octeon_mgmt_update_link(p->port, p->phydev);
	octeon_mgmt_enable_link(p->port);

	p->last_link = 0;
	p->last_speed = 0;
	/*
	 * PHY is not present in simulator. The carrier is enabled
	 * while initializing the phy for simulator, leave it enabled.
	 */
	if (p->phydev) {
		netif_carrier_off(netdev);
		phy_start_aneg(p->phydev);
	}

	netif_wake_queue(netdev);
	napi_enable(&p->napi);

	return 0;
err_noirq:
	octeon_mgmt_reset_hw(p);
	dma_unmap_single(p->dev, p->rx_ring_handle,
			 ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
			 DMA_BIDIRECTIONAL);
	kfree(p->rx_ring);
err_nomem:
	dma_unmap_single(p->dev, p->tx_ring_handle,
			 ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
			 DMA_BIDIRECTIONAL);
	kfree(p->tx_ring);
	return -ENOMEM;
}

static int octeon_mgmt_stop(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);

	napi_disable(&p->napi);
	netif_stop_queue(netdev);

	if (p->phydev)
		phy_disconnect(p->phydev);

	netif_carrier_off(netdev);

	octeon_mgmt_reset_hw(p);


	free_irq(p->irq, netdev);

	/* dma_unmap is a nop on Octeon, so just free everything.  */
	skb_queue_purge(&p->tx_list);
	skb_queue_purge(&p->rx_list);

	dma_unmap_single(p->dev, p->rx_ring_handle,
			 ring_size_to_bytes(OCTEON_MGMT_RX_RING_SIZE),
			 DMA_BIDIRECTIONAL);
	kfree(p->rx_ring);

	dma_unmap_single(p->dev, p->tx_ring_handle,
			 ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
			 DMA_BIDIRECTIONAL);
	kfree(p->tx_ring);

	return 0;
}

static int octeon_mgmt_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);
	int port = p->port;
	union mgmt_port_ring_entry re;
	unsigned long flags;
	int rv = NETDEV_TX_BUSY;

	re.d64 = 0;
	re.s.tstamp = skb_tx(skb)->hardware;
	re.s.len = skb->len;
	re.s.addr = dma_map_single(p->dev, skb->data,
				   skb->len,
				   DMA_TO_DEVICE);

	spin_lock_irqsave(&p->tx_list.lock, flags);

	if (unlikely(p->tx_current_fill >= ring_max_fill(OCTEON_MGMT_TX_RING_SIZE) - 1)) {
		spin_unlock_irqrestore(&p->tx_list.lock, flags);
		netif_stop_queue(netdev);
		spin_lock_irqsave(&p->tx_list.lock, flags);
	}

	if (unlikely(p->tx_current_fill >=
		     ring_max_fill(OCTEON_MGMT_TX_RING_SIZE))) {
		spin_unlock_irqrestore(&p->tx_list.lock, flags);
		dma_unmap_single(p->dev, re.s.addr, re.s.len,
				 DMA_TO_DEVICE);
		goto out;
	}

	__skb_queue_tail(&p->tx_list, skb);

	/* Put it in the ring.  */
	p->tx_ring[p->tx_next] = re.d64;
	p->tx_next = (p->tx_next + 1) % OCTEON_MGMT_TX_RING_SIZE;
	p->tx_current_fill++;

	spin_unlock_irqrestore(&p->tx_list.lock, flags);

	dma_sync_single_for_device(p->dev, p->tx_ring_handle,
				   ring_size_to_bytes(OCTEON_MGMT_TX_RING_SIZE),
				   DMA_BIDIRECTIONAL);

	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += skb->len;

	/* Ring the bell.  */
	cvmx_write_csr(CVMX_MIXX_ORING2(port), 1);

	netdev->trans_start = jiffies;
	rv = NETDEV_TX_OK;
out:
	octeon_mgmt_update_tx_stats(netdev);
	return rv;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void octeon_mgmt_poll_controller(struct net_device *netdev)
{
	struct octeon_mgmt *p = netdev_priv(netdev);

	octeon_mgmt_receive_packets(p, 16);
	octeon_mgmt_update_rx_stats(netdev);
	return;
}
#endif

static void octeon_mgmt_get_drvinfo(struct net_device *netdev,
				    struct ethtool_drvinfo *info)
{
	strncpy(info->driver, DRV_NAME, sizeof(info->driver));
	strncpy(info->version, DRV_VERSION, sizeof(info->version));
	strncpy(info->fw_version, "N/A", sizeof(info->fw_version));
	strncpy(info->bus_info, "N/A", sizeof(info->bus_info));
	info->n_stats = 0;
	info->testinfo_len = 0;
	info->regdump_len = 0;
	info->eedump_len = 0;
}

static int octeon_mgmt_get_settings(struct net_device *netdev,
				    struct ethtool_cmd *cmd)
{
	struct octeon_mgmt *p = netdev_priv(netdev);

	if (p->phydev)
		return phy_ethtool_gset(p->phydev, cmd);

	return -EINVAL;
}

static int octeon_mgmt_set_settings(struct net_device *netdev,
				    struct ethtool_cmd *cmd)
{
	struct octeon_mgmt *p = netdev_priv(netdev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (p->phydev)
		return phy_ethtool_sset(p->phydev, cmd);

	return -EINVAL;
}

static const struct ethtool_ops octeon_mgmt_ethtool_ops = {
	.get_drvinfo = octeon_mgmt_get_drvinfo,
	.get_link = ethtool_op_get_link,
	.get_settings = octeon_mgmt_get_settings,
	.set_settings = octeon_mgmt_set_settings
};

static const struct net_device_ops octeon_mgmt_ops = {
	.ndo_open =			octeon_mgmt_open,
	.ndo_stop =			octeon_mgmt_stop,
	.ndo_start_xmit =		octeon_mgmt_xmit,
	.ndo_set_rx_mode = 		octeon_mgmt_set_rx_filtering,
	.ndo_set_multicast_list =	octeon_mgmt_set_rx_filtering,
	.ndo_set_mac_address =		octeon_mgmt_set_mac_address,
	.ndo_do_ioctl = 		octeon_mgmt_ioctl,
	.ndo_change_mtu =		octeon_mgmt_change_mtu,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller =		octeon_mgmt_poll_controller,
#endif
};

static int __init octeon_mgmt_probe(struct platform_device *pdev)
{
	struct resource *res_irq;
	struct net_device *netdev;
	struct octeon_mgmt *p;
	int i;

	netdev = alloc_etherdev(sizeof(struct octeon_mgmt));
	if (netdev == NULL)
		return -ENOMEM;

	dev_set_drvdata(&pdev->dev, netdev);
	p = netdev_priv(netdev);
	netif_napi_add(netdev, &p->napi, octeon_mgmt_napi_poll,
		       OCTEON_MGMT_NAPI_WEIGHT);

	p->netdev = netdev;
	p->dev = &pdev->dev;

	p->has_rx_tstamp = 0;
	p->port = pdev->id;
	snprintf(netdev->name, IFNAMSIZ, "mgmt%d", p->port);

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq)
		goto err;

	p->irq = res_irq->start;
	spin_lock_init(&p->lock);

	skb_queue_head_init(&p->tx_list);
	skb_queue_head_init(&p->rx_list);
	tasklet_init(&p->tx_clean_tasklet,
		     octeon_mgmt_clean_tx_tasklet, (unsigned long)p);

	netdev->netdev_ops = &octeon_mgmt_ops;
	netdev->ethtool_ops = &octeon_mgmt_ethtool_ops;


	/* The mgmt ports get the first N MACs.  */
	for (i = 0; i < 6; i++)
		netdev->dev_addr[i] = octeon_bootinfo->mac_addr_base[i];
	netdev->dev_addr[5] += p->port;

	if (p->port >= octeon_bootinfo->mac_addr_count)
		dev_err(&pdev->dev,
			"Error %s: Using MAC outside of the assigned range: "
			"%02x:%02x:%02x:%02x:%02x:%02x\n", netdev->name,
			netdev->dev_addr[0], netdev->dev_addr[1],
			netdev->dev_addr[2], netdev->dev_addr[3],
			netdev->dev_addr[4], netdev->dev_addr[5]);

	if (register_netdev(netdev))
		goto err;

	dev_info(&pdev->dev, "Version " DRV_VERSION "\n");
	return 0;
err:
	free_netdev(netdev);
	return -ENOENT;
}

static int __exit octeon_mgmt_remove(struct platform_device *pdev)
{
	struct net_device *netdev = dev_get_drvdata(&pdev->dev);

	unregister_netdev(netdev);
	free_netdev(netdev);
	return 0;
}

static struct platform_driver octeon_mgmt_driver = {
	.driver = {
		.name		= "octeon_mgmt",
		.owner		= THIS_MODULE,
	},
	.probe		= octeon_mgmt_probe,
	.remove		= __exit_p(octeon_mgmt_remove),
};

extern void octeon_mdiobus_force_mod_depencency(void);

static int __init octeon_mgmt_mod_init(void)
{
	/* Force our mdiobus driver module to be loaded first. */
	octeon_mdiobus_force_mod_depencency();
	return platform_driver_register(&octeon_mgmt_driver);
}

static void __exit octeon_mgmt_mod_exit(void)
{
	platform_driver_unregister(&octeon_mgmt_driver);
}

module_init(octeon_mgmt_mod_init);
module_exit(octeon_mgmt_mod_exit);

MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR("David Daney");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
