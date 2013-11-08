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

#undef CVM_OCT_XMIT
#undef CVM_OCT_PKO_LOCK_TYPE

#ifdef CVM_OCT_LOCKLESS
#define CVM_OCT_XMIT cvm_oct_xmit_lockless
#define CVM_OCT_PKO_LOCK_TYPE CVMX_PKO_LOCK_NONE
#else
#define CVM_OCT_XMIT cvm_oct_xmit
#define CVM_OCT_PKO_LOCK_TYPE CVMX_PKO_LOCK_CMD_QUEUE
#endif

/**
 * cvm_oct_xmit - transmit a packet
 * @skb:    Packet to send
 * @dev:    Device info structure
 *
 * Returns Always returns NETDEV_TX_OK
 */
int
CVM_OCT_XMIT
(struct sk_buff *skb, struct net_device *dev)
{
	cvmx_pko_command_word0_t pko_command;
	union cvmx_buf_ptr hw_buffer;
	uint64_t old_scratch;
	uint64_t old_scratch2;
	int qos;
	int i;
	enum {QUEUE_HW, QUEUE_WQE, QUEUE_DROP} queue_type;
	struct octeon_ethernet *priv = netdev_priv(dev);
	int32_t queue_depth;
	int32_t buffers_to_free;
	unsigned long flags;
	cvmx_wqe_t *work = NULL;
#if REUSE_SKBUFFS_WITHOUT_FREE
	unsigned char *fpa_head;
#endif

	/*
	 * Prefetch the private data structure.  It is larger than one
	 * cache line.
	 */
	prefetch(priv);

	if (USE_ASYNC_IOBDMA) {
		/* Save scratch in case userspace is using it */
		CVMX_SYNCIOBDMA;
		old_scratch = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
		old_scratch2 = cvmx_scratch_read64(CVMX_SCR_SCRATCH + 8);

		/*
		 * Fetch and increment the number of packets to be
		 * freed.
		 */
		cvmx_fau_async_fetch_and_add32(CVMX_SCR_SCRATCH + 8,
					       FAU_NUM_PACKET_BUFFERS_TO_FREE,
					       0);

	}


#ifdef CVM_OCT_LOCKLESS
	qos = cvmx_get_core_num();
#else
	/*
	 * The check on CVMX_PKO_QUEUES_PER_PORT_* is designed to
	 * completely remove "qos" in the event neither interface
	 * supports multiple queues per port.
	 */
	if ((CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 > 1) ||
		(CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 > 1)) {
		qos = GET_SKBUFF_QOS(skb);
		if (qos <= 0)
			qos = 0;
		else if (qos >= priv->num_tx_queues)
			qos = 0;
	} else
		qos = 0;
#endif

	if (USE_ASYNC_IOBDMA) {
		cvmx_fau_async_fetch_and_add32(CVMX_SCR_SCRATCH,
					       priv->tx_queue[qos].fau, 1);
	}

	/*
	 * We have space for 6 segment pointers, If there will be more
	 * than that, we must linearize.
	 */
	if (unlikely(skb_shinfo(skb)->nr_frags > 5)) {
		if (unlikely(__skb_linearize(skb))) {
			queue_type = QUEUE_DROP;
			goto skip_xmit;
		}
	}

#ifndef CVM_OCT_LOCKLESS
	/*
	 * The CN3XXX series of parts has an errata (GMX-401) which
	 * causes the GMX block to hang if a collision occurs towards
	 * the end of a <68 byte packet. As a workaround for this, we
	 * pad packets to be 68 bytes whenever we are in half duplex
	 * mode. We don't handle the case of having a small packet but
	 * no room to add the padding.  The kernel should always give
	 * us at least a cache line
	 */
	if ((skb->len < 64) && OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		union cvmx_gmxx_prtx_cfg gmx_prt_cfg;
		int interface = INTERFACE(priv->port);
		int index = INDEX(priv->port);

		if (interface < 2) {
			/* We only need to pad packet in half duplex mode */
			gmx_prt_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
			if (gmx_prt_cfg.s.duplex == 0) {
				int add_bytes = 64 - skb->len;
				if ((skb_tail_pointer(skb) + add_bytes) <= skb_end_pointer(skb))
					memset(__skb_put(skb, add_bytes), 0, add_bytes);
			}
		}
	}
#endif
	/* Build the PKO command */
	pko_command.u64 = 0;
	pko_command.s.n2 = 1;	/* Don't pollute L2 with the outgoing packet */
	pko_command.s.segs = 1;
	pko_command.s.total_bytes = skb->len;
	/* Use fau0 to decrement the number of packets queued */
	pko_command.s.size0 = CVMX_FAU_OP_SIZE_32;
	pko_command.s.subone0 = 1;
	pko_command.s.reg0 = priv->tx_queue[qos].fau;
	pko_command.s.dontfree = 1;

	/* Build the PKO buffer pointer */
	hw_buffer.u64 = 0;
	if (skb_shinfo(skb)->nr_frags == 0) {
		hw_buffer.s.addr = XKPHYS_TO_PHYS((u64)skb->data);
		hw_buffer.s.pool = 0;
		hw_buffer.s.size = skb->len;
	} else {
		hw_buffer.s.addr = XKPHYS_TO_PHYS((u64)skb->data);
		hw_buffer.s.pool = 0;
		hw_buffer.s.size = skb_headlen(skb);
		CVM_OCT_SKB_CB(skb)[0] = hw_buffer.u64;
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			struct skb_frag_struct *fs = skb_shinfo(skb)->frags + i;
			hw_buffer.s.addr = XKPHYS_TO_PHYS((u64)(page_address(fs->page) + fs->page_offset));
			hw_buffer.s.size = fs->size;
			CVM_OCT_SKB_CB(skb)[i + 1] = hw_buffer.u64;
		}
		hw_buffer.s.addr = XKPHYS_TO_PHYS((u64)CVM_OCT_SKB_CB(skb));
		hw_buffer.s.size = skb_shinfo(skb)->nr_frags + 1;
		pko_command.s.segs = skb_shinfo(skb)->nr_frags + 1;
		pko_command.s.gather = 1;
		goto dont_put_skbuff_in_hw;
	}

	/*
	 * See if we can put this skb in the FPA pool. Any strange
	 * behavior from the Linux networking stack will most likely
	 * be caused by a bug in the following code. If some field is
	 * in use by the network stack and get carried over when a
	 * buffer is reused, bad thing may happen.  If in doubt and
	 * you dont need the absolute best performance, disable the
	 * define REUSE_SKBUFFS_WITHOUT_FREE. The reuse of buffers has
	 * shown a 25% increase in performance under some loads.
	 */
#if REUSE_SKBUFFS_WITHOUT_FREE
	/* Timestamps are returned in the WQE, so we can't reuse the buffer */
	if (unlikely(priv->flags & (OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW |
				    OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW)))
		goto dont_put_skbuff_in_hw;

	fpa_head = skb->head + 256 - ((unsigned long)skb->head & 0x7f);
	if (unlikely(skb->data < fpa_head)) {
		/*
		 * printk("TX buffer beginning can't meet FPA
		 * alignment constraints\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely((skb_end_pointer(skb) - fpa_head) < CVMX_FPA_PACKET_POOL_SIZE)) {
		/*
		   printk("TX buffer isn't large enough for the FPA\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb_shared(skb))) {
		/*
		   printk("TX buffer sharing data with someone else\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb_cloned(skb))) {
		/*
		   printk("TX buffer has been cloned\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb_header_cloned(skb))) {
		/*
		   printk("TX buffer header has been cloned\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb->destructor)) {
		/*
		   printk("TX buffer has a destructor\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb_shinfo(skb)->nr_frags)) {
		/*
		   printk("TX buffer has fragments\n");
		 */
		goto dont_put_skbuff_in_hw;
	}
	if (unlikely(skb->truesize != sizeof(*skb) + skb_end_pointer(skb) - skb->head)) {
		/*
		   printk("TX buffer truesize has been changed\n");
		 */
		goto dont_put_skbuff_in_hw;
	}

	/*
	 * We can use this buffer in the FPA.  We don't need the FAU
	 * update anymore
	 */
	pko_command.s.dontfree = 0;

	hw_buffer.s.back = ((unsigned long)skb->data >> 7) - ((unsigned long)fpa_head >> 7);
	*(struct sk_buff **)(fpa_head - sizeof(void *)) = skb;

	/*
	 * The skbuff will be reused without ever being freed. We must
	 * cleanup a bunch of core things.
	 */
	dst_release(skb_dst(skb));
	skb_dst_set(skb, NULL);
#ifdef CONFIG_XFRM
	secpath_put(skb->sp);
	skb->sp = NULL;
#endif
	nf_reset(skb);

#ifdef CONFIG_NET_SCHED
	skb->tc_index = 0;
#ifdef CONFIG_NET_CLS_ACT
	skb->tc_verd = 0;
#endif /* CONFIG_NET_CLS_ACT */
#endif /* CONFIG_NET_SCHED */
#endif /* REUSE_SKBUFFS_WITHOUT_FREE */

dont_put_skbuff_in_hw:

	/* Check if we can use the hardware checksumming */
	if (USE_HW_TCPUDP_CHECKSUM && (skb->protocol == htons(ETH_P_IP)) &&
	    (ip_hdr(skb)->version == 4) && (ip_hdr(skb)->ihl == 5) &&
	    ((ip_hdr(skb)->frag_off == 0) || (ip_hdr(skb)->frag_off == 1 << 14))
	    && ((ip_hdr(skb)->protocol == IPPROTO_TCP) || (ip_hdr(skb)->protocol == IPPROTO_UDP))) {
		/* Use hardware checksum calc */
		pko_command.s.ipoffp1 = sizeof(struct ethhdr) + 1;
		if (unlikely(priv->imode == CVMX_HELPER_INTERFACE_MODE_SRIO))
			pko_command.s.ipoffp1 += 8;
	}

	if (USE_ASYNC_IOBDMA) {
		/* Get the number of skbuffs in use by the hardware */
		CVMX_SYNCIOBDMA;
		queue_depth = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
		buffers_to_free = cvmx_scratch_read64(CVMX_SCR_SCRATCH + 8);
	} else {
		/* Get the number of skbuffs in use by the hardware */
		queue_depth = cvmx_fau_fetch_and_add32(priv->tx_queue[qos].fau, 1);
		buffers_to_free = cvmx_fau_fetch_and_add32(FAU_NUM_PACKET_BUFFERS_TO_FREE, 0);
	}

	/*
	 * If we're sending faster than the receive can free them then
	 * don't do the HW free.
	 */
	if (unlikely(buffers_to_free < -100))
		pko_command.s.dontfree = 1;

	/* Drop this packet if we have too many already queued to the HW */
	if (unlikely(queue_depth >= MAX_OUT_QUEUE_DEPTH)) {
		if (dev->tx_queue_len != 0) {
			netif_stop_queue(dev);
		} else {
			/* If not using normal queueing.  */
			queue_type = QUEUE_DROP;
			goto skip_xmit;
		}
	}

	if (pko_command.s.dontfree)
		queue_type = QUEUE_WQE;
	else
		queue_type = QUEUE_HW;

	if (queue_type == QUEUE_WQE) {
		work = cvmx_fpa_alloc(CVMX_FPA_TX_WQE_POOL);
		if (unlikely(!work)) {
			DEBUGPRINT("%s: Failed WQE allocate\n", dev->name);
			queue_type = QUEUE_DROP;
			goto skip_xmit;
		}

		pko_command.s.rsp = 1;
		pko_command.s.wqp = 1;
		work->hw_chksum = 0;
		/*
		 * work->unused will carry the qos for this packet,
		 * this allows us to find the proper FAU when freeing
		 * the packet.  We decrement the FAU when the WQE is
		 * replaced in the pool.
		 */
		pko_command.s.reg0 = 0;
		work->unused = (uint8_t)qos;

		work->next_ptr = 0;
		work->len = 0;
		work->tag_type = CVMX_POW_TAG_TYPE_NULL;
		work->tag = 0;
		work->word2.u64 = 0;
		work->word2.s.software = 1;
		work->ipprt = priv->port;
		/* Use a different queue for return packet than rx so a
		    high load on one port is spread across multiple
		    queues */
		work->qos = (~priv->port) & 7;
		work->grp = pow_receive_group;
		work->packet_ptr.u64 = (unsigned long)skb;
	}

	local_irq_save(flags);

	cvmx_pko_send_packet_prepare(priv->port, priv->tx_queue[qos].queue, CVM_OCT_PKO_LOCK_TYPE);

	/* Send the packet to the output queue */
	if (queue_type == QUEUE_WQE) {
		uint64_t word2 = cvmx_ptr_to_phys(work);
		if (priv->flags & OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW) {
			/* The first 8 bytes work->packet_data will get the timestamp */
			*(uint64_t *)work->packet_data = ktime_to_ns(ktime_get_real());
		}
		if (priv->flags & OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW) {
			/* The first 8 bytes work->packet_data will get the timestamp */
			*(uint64_t *)work->packet_data = 0;
			word2 |= 1ull<<40; /* Bit 40 controls timestamps */
		}
		if (unlikely(cvmx_pko_send_packet_finish3(priv->port,
							  priv->tx_queue[qos].queue, pko_command, hw_buffer,
							  word2, CVM_OCT_PKO_LOCK_TYPE))) {
				queue_type = QUEUE_DROP;
				cvmx_fpa_free(work, CVMX_FPA_TX_WQE_POOL, 0);
				DEBUGPRINT("%s: Failed to send the packet with wqe\n", dev->name);
		}
	} else {
		if (unlikely(cvmx_pko_send_packet_finish(priv->port,
							 priv->tx_queue[qos].queue,
							 pko_command, hw_buffer,
							 CVM_OCT_PKO_LOCK_TYPE))) {
			DEBUGPRINT("%s: Failed to send the packet\n", dev->name);
			queue_type = QUEUE_DROP;
		}
	}
	local_irq_restore(flags);

skip_xmit:
	switch (queue_type) {
	case QUEUE_DROP:
		cvmx_fau_atomic_add32(priv->tx_queue[qos].fau, -1);
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
		break;
	case QUEUE_HW:
		cvmx_fau_atomic_add32(FAU_NUM_PACKET_BUFFERS_TO_FREE, -1);
		break;
	case QUEUE_WQE:
		/* Cleanup is done on the RX path when the WQE returns */
		break;
	default:
		BUG();
	}

	if (USE_ASYNC_IOBDMA) {
		CVMX_SYNCIOBDMA;
		/* Restore the scratch area */
		cvmx_scratch_write64(CVMX_SCR_SCRATCH, old_scratch);
		cvmx_scratch_write64(CVMX_SCR_SCRATCH + 8, old_scratch2);
	}
	return NETDEV_TX_OK;
}
