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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cache.h>
#include <linux/cpumask.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/prefetch.h>
#include <linux/smp.h>
#include <linux/ktime.h>
#include <net/dst.h>
#ifdef CONFIG_XFRM
#include <linux/xfrm.h>
#include <net/xfrm.h>
#endif /* CONFIG_XFRM */

#include <asm/atomic.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-wqe.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-pow.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-srio.h>
#include <asm/octeon/cvmx-scratch.h>

#include <asm/octeon/cvmx-gmxx-defs.h>

#include "ethernet-defines.h"
#include "ethernet-mem.h"
#include "ethernet-rx.h"
#include "octeon-ethernet.h"
#include "ethernet-util.h"


struct cvm_napi_wrapper {
	struct napi_struct napi;
	int available;
} ____cacheline_aligned_in_smp;

static struct cvm_napi_wrapper cvm_oct_napi[NR_CPUS] __cacheline_aligned_in_smp;

struct cvm_oct_core_state {
	int baseline_cores;
	/*
	 * We want to read this without having to acquire the lock,
	 * make it volatile so we are likely to get a fairly current
	 * value.
	 */
	volatile int active_cores;
	/*
	 * cvm_napi_wrapper.available and active_cores must be kept
	 * consistent with this lock.
	 */
	spinlock_t lock;
} ____cacheline_aligned_in_smp;

static struct cvm_oct_core_state core_state __cacheline_aligned_in_smp;

static void cvm_oct_enable_napi(void *arg)
{
	struct napi_struct *napi = arg;
	napi_schedule(napi);
}

static void cvm_oct_enable_one_cpu(void)
{
	int v;
	int cpu;
	unsigned long flags;

	spin_lock_irqsave(&core_state.lock, flags);
	/* ... if a CPU is available, Turn on NAPI polling for that CPU.  */
	for_each_online_cpu(cpu) {
		if (cvm_oct_napi[cpu].available > 0) {
			cvm_oct_napi[cpu].available--;
			core_state.active_cores++;
			spin_unlock_irqrestore(&core_state.lock, flags);
			v = smp_call_function_single(cpu, cvm_oct_enable_napi,
						     &cvm_oct_napi[cpu].napi, 0);
			if (v)
				panic("Can't enable NAPI.");
			goto out;
		}
	}
	spin_unlock_irqrestore(&core_state.lock, flags);
out:
	return;
}

static void cvm_oct_no_more_work(struct napi_struct *napi)
{
	struct cvm_napi_wrapper *nr = container_of(napi, struct cvm_napi_wrapper, napi);
	int current_active;
	unsigned long flags;


	spin_lock_irqsave(&core_state.lock, flags);

	core_state.active_cores--;
	current_active = core_state.active_cores;
	nr->available++;
	BUG_ON(nr->available != 1);

	spin_unlock_irqrestore(&core_state.lock, flags);

	if (current_active == 0) {
		/*
		 * No more CPUs doing processing, enable interrupts so
		 * we can start processing again when there is
		 * something to do.
		 */
		enable_irq(OCTEON_IRQ_WORKQ0 + pow_receive_group);
	}
}

/**
 * cvm_oct_do_interrupt - interrupt handler.
 *
 * The interrupt occurs whenever the POW has packets in our group.
 *
 */
static irqreturn_t cvm_oct_do_interrupt(int cpl, void *dev_id)
{
	int cpu = smp_processor_id();
	unsigned long flags;

	/* Disable the IRQ and start napi_poll. */
	disable_irq_nosync(OCTEON_IRQ_WORKQ0 + pow_receive_group);

	spin_lock_irqsave(&core_state.lock, flags);

	/* ... and NAPI better not be running on this CPU.  */
	BUG_ON(cvm_oct_napi[cpu].available != 1);
	cvm_oct_napi[cpu].available--;

	/* There better be cores available...  */
	core_state.active_cores++;
	BUG_ON(core_state.active_cores > core_state.baseline_cores);

	spin_unlock_irqrestore(&core_state.lock, flags);

	cvm_oct_enable_napi(&cvm_oct_napi[cpu].napi);

	return IRQ_HANDLED;
}

/**
 * cvm_oct_check_rcv_error - process receive errors
 * @work: Work queue entry pointing to the packet.
 *
 * Returns Non-zero if the packet can be dropped, zero otherwise.
 */
static inline int cvm_oct_check_rcv_error(cvmx_wqe_t *work)
{
	if ((work->word2.snoip.err_code == 10) && (work->len <= 64)) {
		/*
		 * Ignore length errors on min size packets. Some
		 * equipment incorrectly pads packets to 64+4FCS
		 * instead of 60+4FCS.  Note these packets still get
		 * counted as frame errors.
		 */
	} else
	    if (USE_10MBPS_PREAMBLE_WORKAROUND
		&& ((work->word2.snoip.err_code == 5)
		    || (work->word2.snoip.err_code == 7))) {

		/*
		 * We received a packet with either an alignment error
		 * or a FCS error. This may be signalling that we are
		 * running 10Mbps with GMXX_RXX_FRM_CTL[PRE_CHK}
		 * off. If this is the case we need to parse the
		 * packet to determine if we can remove a non spec
		 * preamble and generate a correct packet.
		 */
		int interface = cvmx_helper_get_interface_num(work->ipprt);
		int index = cvmx_helper_get_interface_index_num(work->ipprt);
		union cvmx_gmxx_rxx_frm_ctl gmxx_rxx_frm_ctl;
		gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
		if (gmxx_rxx_frm_ctl.s.pre_chk == 0) {

			uint8_t *ptr = cvmx_phys_to_ptr(work->packet_ptr.s.addr);
			int i = 0;

			while (i < work->len - 1) {
				if (*ptr != 0x55)
					break;
				ptr++;
				i++;
			}

			if (*ptr == 0xd5) {
				/*
				   DEBUGPRINT("Port %d received 0xd5 preamble\n", work->ipprt);
				 */
				work->packet_ptr.s.addr += i + 1;
				work->len -= i + 5;
			} else if ((*ptr & 0xf) == 0xd) {
				/*
				   DEBUGPRINT("Port %d received 0x?d preamble\n", work->ipprt);
				 */
				work->packet_ptr.s.addr += i;
				work->len -= i + 4;
				for (i = 0; i < work->len; i++) {
					*ptr = ((*ptr & 0xf0) >> 4) | ((*(ptr + 1) & 0xf) << 4);
					ptr++;
				}
			} else {
				DEBUGPRINT("Port %d unknown preamble, packet dropped\n",
				     work->ipprt);
				/* cvmx_helper_dump_packet(work); */
				cvm_oct_free_work(work);
				return 1;
			}
		}
	} else {
#if 0 /* removed by zhuj for bug15016, 2011-06-30 */
		DEBUGPRINT("Port %d receive error code %d, packet dropped\n",
			   work->ipprt, work->word2.snoip.err_code);
#endif
		cvm_oct_free_work(work);
		return 1;
	}

	return 0;
}

/**
 * Convert a hardware PTP timestamp into a kernel timestamp.
 *
 * @param ptptime 64 bit PTP timestamp, normally in nanoseconds
 *
 * @return ktime_t
 */
static ktime_t cvm_oct_ptp_to_ktime(uint64_t ptptime)
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

/**
 * cvm_oct_napi_poll - the NAPI poll function.
 * @napi: The NAPI instance, or null if called from cvm_oct_poll_controller
 * @budget: Maximum number of packets to receive.
 *
 * Returns the number of packets processed.
 */
static int cvm_oct_napi_poll(struct napi_struct *napi, int budget)
{
	const int	coreid = cvmx_get_core_num();
	uint64_t	old_group_mask;
	uint64_t	old_scratch;
	int		rx_count = 0;
	int		did_work_request = 0;
	int		packet_not_copied;

	char		*p = (char *)cvm_oct_by_port;
	/* Prefetch cvm_oct_device since we know we need it soon */
	prefetch(&p[0]);
	prefetch(&p[SMP_CACHE_BYTES]);
	prefetch(&p[2 * SMP_CACHE_BYTES]);

	if (USE_ASYNC_IOBDMA) {
		/* Save scratch in case userspace is using it */
		CVMX_SYNCIOBDMA;
		old_scratch = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
	}

	/* Only allow work for our group (and preserve priorities) */
	old_group_mask = cvmx_read_csr(CVMX_POW_PP_GRP_MSKX(coreid));
	cvmx_write_csr(CVMX_POW_PP_GRP_MSKX(coreid),
		       (old_group_mask & ~0xFFFFull) | 1 << pow_receive_group);

	if (USE_ASYNC_IOBDMA) {
		cvmx_pow_work_request_async(CVMX_SCR_SCRATCH, CVMX_POW_NO_WAIT);
		did_work_request = 1;
	}

	while (rx_count < budget) {
		struct sk_buff *skb = NULL;
		struct sk_buff **pskb = NULL;
		enum cvm_oct_callback_result callback_result;
		int skb_in_hw;
		cvmx_wqe_t *work;

		if (USE_ASYNC_IOBDMA && did_work_request)
			work = cvmx_pow_work_response_async(CVMX_SCR_SCRATCH);
		else
			work = cvmx_pow_work_request_sync(CVMX_POW_NO_WAIT);

		prefetch(work);
		did_work_request = 0;
		if (work == NULL) {
			union cvmx_pow_wq_int wq_int;
			wq_int.u64 = 0;
			wq_int.s.iq_dis = 1 << pow_receive_group;
			wq_int.s.wq_int = 1 << pow_receive_group;
			cvmx_write_csr(CVMX_POW_WQ_INT, wq_int.u64);
			break;
		}
		pskb = (struct sk_buff **)(cvm_oct_get_buffer_ptr(work->packet_ptr) - sizeof(void *));
		prefetch(pskb);

		if (USE_ASYNC_IOBDMA && rx_count < (budget - 1)) {
			cvmx_pow_work_request_async_nocheck(CVMX_SCR_SCRATCH, CVMX_POW_NO_WAIT);
			did_work_request = 1;
		}

		if (rx_count == 0) {
			/*
			 * First time through, see if there is enough
			 * work waiting to merit waking another
			 * CPU.
			 */
			union cvmx_pow_wq_int_cntx counts;
			int backlog;
			int cores_in_use = core_state.active_cores;

			counts.u64 = cvmx_read_csr(CVMX_POW_WQ_INT_CNTX(pow_receive_group));
			backlog = counts.s.iq_cnt + counts.s.ds_cnt;
			if (backlog > budget * cores_in_use &&
			    napi != NULL &&
			    cores_in_use < core_state.baseline_cores)
				cvm_oct_enable_one_cpu();
		}

		/*
		 * If WORD2[SOFTWARE] then this WQE is a complete for
		 * a TX packet.
		 */
		if (work->word2.s.software) {
			struct octeon_ethernet *priv;
			union skb_shared_tx *shtx;
			int packet_qos = work->unused;
			skb = (struct sk_buff *)work->packet_ptr.u64;
			priv = netdev_priv(skb->dev);
			if (!netif_running(skb->dev))
				netif_wake_queue(skb->dev);
			shtx = skb_tx(skb);
			if (unlikely(shtx->hardware)) {
				if (priv->flags & OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_HW) {
					uint64_t ns = *(uint64_t*)work->packet_data;
					struct skb_shared_hwtstamps ts;
					ts.syststamp = cvm_oct_ptp_to_ktime(ns);
					ts.hwtstamp = ns_to_ktime(ns);
					skb_tstamp_tx(skb, &ts);
				}
				if (priv->flags & OCTEON_ETHERNET_FLAG_TX_TIMESTAMP_SW) {
					uint64_t ns = *(uint64_t*)work->packet_data;
					struct skb_shared_hwtstamps ts;
					ts.syststamp = ns_to_ktime(ns);
					ts.hwtstamp = ns_to_ktime(0);
					skb_tstamp_tx(skb, &ts);
				}
			}

			dev_kfree_skb_any(skb);

			cvmx_fpa_free(work, CVMX_FPA_TX_WQE_POOL, DONT_WRITEBACK(1));

			/*
			 * We are done with this one, adjust the queue
			 * depth.
			 */
			cvmx_fau_atomic_add32(priv->tx_queue[packet_qos].fau, -1);
			continue;
		}

		skb_in_hw = USE_SKBUFFS_IN_HW && work->word2.s.bufs == 1;
		if (likely(skb_in_hw)) {
			skb = *pskb;
			prefetch(&skb->head);
			prefetch(&skb->len);
		}
		prefetch(cvm_oct_by_port[work->ipprt]);

		/* Immediately throw away all packets with receive errors */
		if (unlikely(work->word2.snoip.rcv_error)) {
			if (cvm_oct_check_rcv_error(work))
				continue;
		}

		/*
		 * We can only use the zero copy path if skbuffs are
		 * in the FPA pool and the packet fits in a single
		 * buffer.
		 */
		if (likely(skb_in_hw)) {
			skb->data = skb->head + work->packet_ptr.s.addr - cvmx_ptr_to_phys(skb->head);
			prefetch(skb->data);
			skb->len = work->len;
			skb_set_tail_pointer(skb, skb->len);
			packet_not_copied = 1;
		} else {
			/*
			 * We have to copy the packet. First allocate
			 * an skbuff for it.
			 */
			skb = dev_alloc_skb(work->len);
			if (!skb) {
				DEBUGPRINT("Port %d failed to allocate skbuff, packet dropped\n",
					   work->ipprt);
				cvm_oct_free_work(work);
				continue;
			}

			/*
			 * Check if we've received a packet that was
			 * entirely stored in the work entry.
			 */
			if (unlikely(work->word2.s.bufs == 0)) {
				uint8_t *ptr = work->packet_data;

				if (likely(!work->word2.s.not_IP)) {
					/*
					 * The beginning of the packet
					 * moves for IP packets.
					 */
					if (work->word2.s.is_v6)
						ptr += 2;
					else
						ptr += 6;
				}
				memcpy(skb_put(skb, work->len), ptr, work->len);
				/* No packet buffers to free */
			} else {
				int segments = work->word2.s.bufs;
				union cvmx_buf_ptr segment_ptr = work->packet_ptr;
				int len = work->len;

				while (segments--) {
					union cvmx_buf_ptr  next_ptr;
					int segment_size;

					next_ptr = *(union cvmx_buf_ptr *)cvmx_phys_to_ptr(segment_ptr.s.addr - 8);

			/*
			 * Octeon Errata PKI-100: The segment size is
			 * wrong. Until it is fixed, calculate the
			 * segment size based on the packet pool
			 * buffer size. When it is fixed, the
			 * following line should be replaced with this
			 * one: int segment_size =
			 * segment_ptr.s.size;
			 */
					segment_size = CVMX_FPA_PACKET_POOL_SIZE -
						(segment_ptr.s.addr - (((segment_ptr.s.addr >> 7) - segment_ptr.s.back) << 7));
					/*
					 * Don't copy more than what
					 * is left in the packet.
					 */
					if (segment_size > len)
						segment_size = len;
					/* Copy the data into the packet */
					memcpy(skb_put(skb, segment_size),
					       cvmx_phys_to_ptr(segment_ptr.s.addr),
					       segment_size);
					len -= segment_size;
					segment_ptr = next_ptr;
				}
			}
			packet_not_copied = 0;
		}

		if (likely((work->ipprt < TOTAL_NUMBER_OF_PORTS) && cvm_oct_by_port[work->ipprt])) {
			struct octeon_ethernet *priv = cvm_oct_by_port[work->ipprt];
#ifdef CONFIG_RAPIDIO
			if (unlikely(priv->imode == CVMX_HELPER_INTERFACE_MODE_SRIO)) {
				const cvmx_srio_rx_message_header_t *rx_header = (const cvmx_srio_rx_message_header_t *)skb->data;
                                __skb_pull(skb, sizeof(cvmx_srio_rx_message_header_t));
				priv = cvm_oct_by_srio_mbox[(work->ipprt - 40) >> 1][rx_header->word0.s.mbox];

				atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_packets);
				atomic64_add(skb->len, (atomic64_t *)&priv->netdev->stats.rx_bytes);
			}
#endif
			/*
			 * Only accept packets for devices that are
			 * currently up.
			 */
			if (likely(priv->netdev->flags & IFF_UP)) {
				if (priv->flags & OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_SW) {
					struct skb_shared_hwtstamps *ts;
					ts = skb_hwtstamps(skb);
					ts->syststamp = ktime_get_real();
					ts->hwtstamp = ns_to_ktime(0);
				}
				if (priv->flags & OCTEON_ETHERNET_FLAG_RX_TIMESTAMP_HW) {
					/* The first 8 bytes are the timestamp */
					uint64_t ns = *(uint64_t*)skb->data;
					struct skb_shared_hwtstamps *ts;
					ts = skb_hwtstamps(skb);
					ts->hwtstamp = ns_to_ktime(ns);
					ts->syststamp = cvm_oct_ptp_to_ktime(ns);
					__skb_pull(skb, 8);
				}
				skb->protocol = eth_type_trans(skb, priv->netdev);
				skb->dev = priv->netdev;

				if (unlikely(work->word2.s.not_IP || work->word2.s.IP_exc || work->word2.s.L4_error))
					skb->ip_summed = CHECKSUM_NONE;
				else
					skb->ip_summed = CHECKSUM_UNNECESSARY;

				/* Increment RX stats for virtual ports */
				if (work->ipprt >= CVMX_PIP_NUM_INPUT_PORTS) {
					atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_packets);
					atomic64_add(skb->len, (atomic64_t *)&priv->netdev->stats.rx_bytes);
				}
				if (priv->intercept_cb) {
					callback_result = priv->intercept_cb(priv->netdev, work, skb);
					switch (callback_result) {
					case CVM_OCT_PASS:
						netif_receive_skb(skb);
						rx_count++;
						break;
					case CVM_OCT_DROP:
						dev_kfree_skb_irq(skb);
						atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_dropped);
						break;
					case CVM_OCT_TAKE_OWNERSHIP_WORK:
						/*
						 * Interceptor took
						 * our work, but we
						 * need to free the
						 * skbuff
						 */
						if (USE_SKBUFFS_IN_HW && likely(packet_not_copied)) {
							/*
							 * We can't free the skbuff since its data is
							 * the same as the work. In this case we don't
							 * do anything
							 */
						} else {
							dev_kfree_skb_irq(skb);
						}
						break;
					case CVM_OCT_TAKE_OWNERSHIP_SKB:
						/* Interceptor took our packet */
						break;
					}
				} else {
					netif_receive_skb(skb);
					callback_result = CVM_OCT_PASS;
					rx_count++;
				}
			} else {
				/* Drop any packet received for a device that isn't up */
				/*
				DEBUGPRINT("%s: Device not up, packet dropped\n",
					   dev->name);
				*/
				atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_dropped);
				dev_kfree_skb_irq(skb);
				callback_result = CVM_OCT_DROP;
			}
		} else {
			/*
			 * Drop any packet received for a device that
			 * doesn't exist.
			 */
			DEBUGPRINT("Port %d not controlled by Linux, packet dropped\n",
				   work->ipprt);
			dev_kfree_skb_irq(skb);
			callback_result = CVM_OCT_DROP;
		}
		/* We only need to free the work if the interceptor didn't
		   take over ownership of it */
		if (callback_result != CVM_OCT_TAKE_OWNERSHIP_WORK) {
			/*
			 * Check to see if the skbuff and work share
			 * the same packet buffer.
			 */
			if (USE_SKBUFFS_IN_HW && likely(packet_not_copied)) {
				/*
				 * This buffer needs to be replaced,
				 * increment the number of buffers we
				 * need to free by one.
				 */
				cvmx_fau_atomic_add32(FAU_NUM_PACKET_BUFFERS_TO_FREE, 1);
				cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));
			} else {
				cvm_oct_free_work(work);
			}
		}
	}
	/* Restore the original POW group mask */
	cvmx_write_csr(CVMX_POW_PP_GRP_MSKX(coreid), old_group_mask);
	if (USE_ASYNC_IOBDMA) {
		/* Restore the scratch area */
		cvmx_scratch_write64(CVMX_SCR_SCRATCH, old_scratch);
	}
	cvm_oct_rx_refill_pool(0);

	if (rx_count < budget && napi != NULL) {
		/* No more work */
		napi_complete(napi);
		cvm_oct_no_more_work(napi);
	}
	return rx_count;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
/**
 * cvm_oct_poll_controller - poll for receive packets
 * device.
 *
 * @dev:    Device to poll. Unused
 */
void cvm_oct_poll_controller(struct net_device *dev)
{
	cvm_oct_napi_poll(NULL, 16);
}
#endif

void cvm_oct_rx_initialize(void)
{
	int i;
	struct net_device *dev_for_napi = NULL;
	union cvmx_pow_wq_int_thrx int_thr;
	union cvmx_pow_wq_int_pc int_pc;

	if (list_empty(&cvm_oct_list))
		panic("No net_devices were allocated.");

	dev_for_napi = list_first_entry(&cvm_oct_list,
					struct octeon_ethernet,
					list)->netdev;

	if (max_rx_cpus > 1  && max_rx_cpus < num_online_cpus())
		core_state.baseline_cores = max_rx_cpus;
	else
		core_state.baseline_cores = num_online_cpus();

	for_each_possible_cpu(i) {
		cvm_oct_napi[i].available = 1;
		netif_napi_add(dev_for_napi, &cvm_oct_napi[i].napi,
			       cvm_oct_napi_poll, rx_napi_weight);
		napi_enable(&cvm_oct_napi[i].napi);
	}
	/*
	 * Before interrupts are enabled, no RX processing will occur,
	 * so we can initialize all those things out side of the
	 * lock.
	 */
	spin_lock_init(&core_state.lock);

	/* Register an IRQ hander for to receive POW interrupts */
	i = request_irq(OCTEON_IRQ_WORKQ0 + pow_receive_group,
			cvm_oct_do_interrupt, 0, dev_for_napi->name, &cvm_oct_list);

	if (i)
		panic("Could not acquire Ethernet IRQ %d\n",
		      OCTEON_IRQ_WORKQ0 + pow_receive_group);

	disable_irq_nosync(OCTEON_IRQ_WORKQ0 + pow_receive_group);

	int_thr.u64 = 0;
	int_thr.s.tc_en = 1;
	int_thr.s.tc_thr = 1;
	/* Enable POW interrupt when our port has at least one packet */
	cvmx_write_csr(CVMX_POW_WQ_INT_THRX(pow_receive_group), int_thr.u64);

	int_pc.u64 = 0;
	int_pc.s.pc_thr = 5;
	cvmx_write_csr(CVMX_POW_WQ_INT_PC, int_pc.u64);


	/* Scheduld NAPI now.  This will indirectly enable interrupts. */
	cvm_oct_enable_one_cpu();
}

void cvm_oct_rx_shutdown(void)
{
	int i;
	/* Shutdown all of the NAPIs */
	for_each_possible_cpu(i)
		netif_napi_del(&cvm_oct_napi[i].napi);

	/* Free the interrupt handler */
	free_irq(OCTEON_IRQ_WORKQ0 + pow_receive_group, &cvm_oct_list);

}
