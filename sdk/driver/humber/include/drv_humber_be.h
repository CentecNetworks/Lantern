/**
 @file drv_humber_be.h

 @date 2010-02-25

 @version v5.1

 The file contains all tables and regs' struct declaration(big edian)
*/

#ifndef _DRV_HUMBER_BE_H_
#define _DRV_HUMBER_BE_H_

#include "kal.h"

/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/

struct nl9k_ctl_reg_ltr_s
{
    uint32 bsr0_31_to0  :32;

    uint32 bsr0_63_to32 :32;

    uint32 rsv0         :16;
    uint32 bsr0_79_to64 :16;

    uint32 rsv1         :32;

    uint32 bsr1_31_to0  :32;

    uint32 bsr1_63_to32 :32;

    uint32 rsv2         :16;
    uint32 bsr1_79_to64 :16;

    uint32 rsv3         :32;

    uint32 psr0_31_to0  :32;

    uint32 psr0_63_to32 :32;

    uint32 rsv4         :16;
    uint32 psr0_79_to64 :16;

    uint32 rsv5         :32;

    uint32 psr1_31_to0  :32;

    uint32 psr1_63_to32 :32;

    uint32 rsv6         :16;
    uint32 psr1_79_to64 :16;

    uint32 rsv7         :32;

    uint32 psr2_31_to0  :32;

    uint32 psr2_63_to32 :32;

    uint32 rsv8         :16;
    uint32 psr2_79_to64 :16;

    uint32 rsv9         :32;

    uint32 psr3_31_to0  :32;

    uint32 psr3_63_to32 :32;

    uint32 rsv10         :16;
    uint32 psr3_79_to64 :16;

    uint32 rsv11         :32;

    uint32 kcr_31_to0   :32;

    uint32 kcr_63_to32  :32;

    uint32 rsv12         :16;
    uint32 kcr_79_to64  :16;

    uint32 rsv13         :32;

    uint32 rsv14         :32;

    uint32 rsv15         :32;

    uint32 rsv16         :32;

    uint32 rsv17        :32;

    uint32 rsv18        :32;

    uint32 rsv19        :32;

    uint32 rsv20        :32;

    uint32 rsv21        :32;

    uint32 rsv22        :32;

    uint32 rsv23        :32;

    uint32 rsv24        :32;

    uint32 rsv25        :32;

    uint32 rsv26        :32;

    uint32 rsv27        :32;

    uint32 rsv28        :32;

    uint32 rsv29        :32;

    uint32 rsv30        :32;

    uint32 rsv31        :32;

    uint32 rsv32        :32;

    uint32 rsv33        :32;

    uint32 rsv34        :32;

    uint32 rsv35        :32;

    uint32 rsv36        :32;

    uint32 rsv37        :32;

    uint32 rsv38        :32;

    uint32 rsv39        :32;

    uint32 rsv40        :32;

    uint32 rsv41        :32;

    uint32 rsv44        :32;

    uint32 rsv45        :32;

    uint32 rsv46        :32;

    uint32 rsv47        :32;

    uint32 rsv48        :32;

    uint32 rsv49        :32;

    uint32 rsv50        :32;

    uint32 rsv51        :32;


};
typedef struct nl9k_ctl_reg_ltr_s nl9k_ctl_reg_ltr_t;

struct nl9k_ctl_reg_bcr_s
{
    uint32 rsv0       :28;
    uint32 blk_width  :3;
    uint32 blk_en     :1;

    uint32 rsv1       :32;

    uint32 rsv2       :32;

    uint32 rsv3       :32;
};
typedef struct nl9k_ctl_reg_bcr_s nl9k_ctl_reg_bcr_t;


struct nl9k_ctl_reg_bmr_s
{
    uint32 bmr_31_to0   :32;

    uint32 bmr_63_to32  :32;

    uint32 rsv0         :16;
    uint32 bmr_79_to64  :16;

    uint32 rsv1         :32;
};
typedef struct nl9k_ctl_reg_bmr_s nl9k_ctl_reg_bmr_t;


struct buf_retrv_config_s	/* 0 */
{

	uint32 rsv_0	:31;
	uint32 buf_retrv_pkt_msg_drain_enable	:1;

	uint32 rsv_1	:31;
	uint32 buf_retrv_buf_ptr_drain_enable	:1;

	uint32 rsv_2	:16;
	uint32 fl_ctrl_sel	:4;
	uint32 rsv_3	:2;
	uint32 extra_credit_used	:2;
	uint32 rsv_4	:3;
	uint32 parity_enable	:1;
	uint32 buf_retrv_ipe_en	:1;
	uint32 buf_retrv_oam_en	:1;
	uint32 buf_retrv_fabric_en	:1;
	uint32 buf_retrv_network_en	:1;

	uint32 rsv_5	:25;
	uint32 epe_credit_adjust_fifoa_full_thd	:7;
};
typedef struct buf_retrv_config_s buf_retrv_config_t;

struct buf_retrv_state_s	/* 1 */
{

	uint32 rsv_0	:16;
	uint32 buf_arb_state	:2;
	uint32 bufq_state	:2;
	uint32 rsv_1	:1;
	uint32 bufq_state_start	:1;
	uint32 link_state	:2;
	uint32 pkt_arb_state	:2;
	uint32 pktq_state	:2;
	uint32 rsv_2	:3;
	uint32 pktq_state_start	:1;
};
typedef struct buf_retrv_state_s buf_retrv_state_t;

struct buf_retrv_buf_ptr_intf_config_s	/* 2 */
{

	uint32 rsv_0	:14;
	uint32 cfg_buf_ptr_slot	:2;
	uint32 rsv_1	:16;
};
typedef struct buf_retrv_buf_ptr_intf_config_s buf_retrv_buf_ptr_intf_config_t;

struct buf_retrv_credit_module_config_s	/* 3 */
{

	uint32 rsv_0	:31;
	uint32 credit_module_enable	:1;

	uint32 rsv_1	:28;
	uint32 credit_fifo_full_threshold	:4;
};
typedef struct buf_retrv_credit_module_config_s buf_retrv_credit_module_config_t;

struct buf_retrv_debug_stats_s	/* 4 */
{

	uint32 rsv_0	:28;
	uint32 frq_mgr_pkt_cnt	:4;

	uint32 rsv_1	:24;
	uint32 frq_mgr_buf_cnt	:8;

	uint32 rsv_2	:28;
	uint32 frpb_qdr_err_cnt	:4;

	uint32 rsv_3	:28;
	uint32 fr_buf_store_err_cnt	:4;

	uint32 rsv_4	:28;
	uint32 to_met_fifo_pkt_cnt	:4;

	uint32 rsv_5	:24;
	uint32 to_met_fifo_buf_cnt	:8;

	uint32 rsv_6	:28;
	uint32 to_epe_sop_cnt	:4;

	uint32 rsv_7	:28;
	uint32 to_epe_eop_cnt	:4;

	uint32 rsv_8	:28;
	uint32 to_voq_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 to_voq_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 to_oam_sop_cnt	:4;

	uint32 rsv_11	:28;
	uint32 to_oam_eop_cnt	:4;

	uint32 rsv_12	:28;
	uint32 to_ipe_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 to_ipe_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 rtn_all_pkt_cnt	:4;

	uint32 rsv_15	:24;
	uint32 rtn_all_buf_cnt	:8;

	uint32 rsv_16	:28;
	uint32 hdr_crc_err_cnt	:4;

	uint32 rsv_17	:28;
	uint32 tx_sop_cnt	:4;

	uint32 rsv_18	:28;
	uint32 tx_eop_cnt	:4;
};
typedef struct buf_retrv_debug_stats_s buf_retrv_debug_stats_t;

struct buf_retrv_interrupt_fatal_s	/* 5 */
{

	uint32 value_set_fatal	:32;

	uint32 value_reset_fatal	:32;

	uint32 mask_set_fatal	:32;

	uint32 mask_reset_fatal	:32;
};
typedef struct buf_retrv_interrupt_fatal_s buf_retrv_interrupt_fatal_t;

struct buf_retrv_overrun_port_s	/* 6 */
{

	uint32 rsv_0	:7;
	uint32 pkt_channel_overrun_port_num	:9;
	uint32 rsv_1	:7;
	uint32 buf_channel_overrun_port_num	:9;
};
typedef struct buf_retrv_overrun_port_s buf_retrv_overrun_port_t;

struct buf_retrv_fabric_threshold_s	/* 7 */
{

	uint32 rsv_0	:20;
	uint32 fabric_stall_threshold0	:12;

	uint32 rsv_1	:20;
	uint32 fabric_stall_threshold1	:12;

	uint32 rsv_2	:20;
	uint32 fabric_stall_threshold2	:12;

	uint32 rsv_3	:20;
	uint32 fabric_stall_threshold3	:12;
};
typedef struct buf_retrv_fabric_threshold_s buf_retrv_fabric_threshold_t;

struct buf_retrv_fabric_min_max_s	/* 8 */
{

	uint32 rsv_0	:4;
	uint32 fabric_max0	:12;
	uint32 rsv_1	:4;
	uint32 fabric_min0	:12;

	uint32 rsv_2	:4;
	uint32 fabric_max1	:12;
	uint32 rsv_3	:4;
	uint32 fabric_min1	:12;

	uint32 rsv_4	:4;
	uint32 fabric_max2	:12;
	uint32 rsv_5	:4;
	uint32 fabric_min2	:12;

	uint32 rsv_6	:4;
	uint32 fabric_max3	:12;
	uint32 rsv_7	:4;
	uint32 fabric_min3	:12;
};
typedef struct buf_retrv_fabric_min_max_s buf_retrv_fabric_min_max_t;

struct buf_retrv_fabric_count_s	/* 9 */
{

	uint32 rsv_0	:20;
	uint32 fabric_count0	:12;

	uint32 rsv_1	:20;
	uint32 fabric_count1	:12;

	uint32 rsv_2	:20;
	uint32 fabric_count2	:12;

	uint32 rsv_3	:20;
	uint32 fabric_count3	:12;
};
typedef struct buf_retrv_fabric_count_s buf_retrv_fabric_count_t;

struct buf_retrv_fabric_total_count_s	/* 10 */
{

	uint32 rsv_0	:20;
	uint32 fabric_total_count	:12;
};
typedef struct buf_retrv_fabric_total_count_s buf_retrv_fabric_total_count_t;

struct buf_retrv_credit_config_s	/* 11 */
{

	uint32 rsv_0	:24;
	uint32 credit_config0	:8;

	uint32 rsv_1	:24;
	uint32 credit_config1	:8;

	uint32 rsv_2	:24;
	uint32 credit_config2	:8;

	uint32 rsv_3	:24;
	uint32 credit_config3	:8;

	uint32 rsv_4	:24;
	uint32 credit_config4	:8;

	uint32 rsv_5	:24;
	uint32 credit_config5	:8;

	uint32 rsv_6	:24;
	uint32 credit_config6	:8;

	uint32 rsv_7	:24;
	uint32 credit_config7	:8;
};
typedef struct buf_retrv_credit_config_s buf_retrv_credit_config_t;

struct buf_retrv_buf_credit_config_s	/* 12 */
{

	uint32 rsv_0	:4;
	uint32 buf_threshold3	:4;
	uint32 rsv_1	:4;
	uint32 buf_threshold2	:4;
	uint32 rsv_2	:4;
	uint32 buf_threshold1	:4;
	uint32 rsv_3	:4;
	uint32 buf_threshold0	:4;
};
typedef struct buf_retrv_buf_credit_config_s buf_retrv_buf_credit_config_t;

struct buf_retrv_fl_ctrl_info_s	/* 13 */
{

	uint32 fl_ctrl_info	:32;
};
typedef struct buf_retrv_fl_ctrl_info_s buf_retrv_fl_ctrl_info_t;

struct buf_retrv_pkt_buf_intf_config_s	/* 14 */
{

	uint32 rsv_0	:7;
	uint32 rcd_credit_config	:9;
	uint32 rsv_1	:7;
	uint32 pkt_buf_track_fifo_full_threshold	:5;
	uint32 pkt_buf_req_fifo_full_threshold	:4;

	uint32 rsv_2	:27;
	uint32 pb_ctl_rd_credit_config	:5;
};
typedef struct buf_retrv_pkt_buf_intf_config_s buf_retrv_pkt_buf_intf_config_t;

struct buf_retrv_buf_ptr_config_s	/* 15 */
{

	uint32 rsv_0	:28;
	uint32 buf_ptr_in_fifo_full_threshold	:4;

	uint32 rsv_1	:5;
	uint32 cfg_oam_burst_cnt	:3;
	uint32 rsv_2	:5;
	uint32 cfg_ipe_burst_cnt	:3;
	uint32 rsv_3	:5;
	uint32 cfg_fabric_burst_cnt	:3;
	uint32 rsv_4	:5;
	uint32 cfg_epe_burst_cnt	:3;
};
typedef struct buf_retrv_buf_ptr_config_s buf_retrv_buf_ptr_config_t;

struct buf_retrv_intf_fifo_ctl_s	/* 16 */
{

	uint32 rsv_0	:27;
	uint32 ipe_quad_slot_num	:5;
};
typedef struct buf_retrv_intf_fifo_ctl_s buf_retrv_intf_fifo_ctl_t;

struct buf_retrv_init_ctl_s	/* 17 */
{

	uint32 rsv_0	:15;
	uint32 buf_init_done	:1;
	uint32 rsv_1	:15;
	uint32 buf_init_en	:1;

	uint32 rsv_2	:15;
	uint32 fabric_init_done	:1;
	uint32 rsv_3	:7;
	uint32 network_init_done	:1;
	uint32 rsv_4	:7;
	uint32 credit_init_en	:1;
};
typedef struct buf_retrv_init_ctl_s buf_retrv_init_ctl_t;

struct buf_retrv_intf_fifo_credit_s	/* 18 */
{

	uint32 rsv_0	:1;
	uint32 oam_intf_credit	:7;
	uint32 rsv_1	:1;
	uint32 ipe_intf_credit	:7;
	uint32 rsv_2	:1;
	uint32 fabric_intf_credit	:7;
	uint32 net_intf_credit	:8;

	uint32 rsv_3	:25;
	uint32 cpu_intf_credit	:7;
};
typedef struct buf_retrv_intf_fifo_credit_s buf_retrv_intf_fifo_credit_t;

struct buf_retrv_intf_mem_config_s	/* 19 */
{

	uint32 rsv_0	:7;
	uint32 cfg_epe_intf_end	:9;
	uint32 rsv_1	:7;
	uint32 cfg_epe_intf_start	:9;

	uint32 rsv_2	:7;
	uint32 cfg_voq_intf_end	:9;
	uint32 rsv_3	:7;
	uint32 cfg_voq_intf_start	:9;

	uint32 rsv_4	:7;
	uint32 cfg_ipe_intf_end	:9;
	uint32 rsv_5	:7;
	uint32 cfg_ipe_intf_start	:9;

	uint32 rsv_6	:7;
	uint32 cfg_oam_intf_end	:9;
	uint32 rsv_7	:7;
	uint32 cfg_oam_intf_start	:9;

	uint32 rsv_8	:7;
	uint32 cfg_cpu_intf_end	:9;
	uint32 rsv_9	:7;
	uint32 cfg_cpu_intf_start	:9;
};
typedef struct buf_retrv_intf_mem_config_s buf_retrv_intf_mem_config_t;

struct buf_retrv_intf_mem_status_s	/* 20 */
{

	uint32 rsv_0	:6;
	uint32 epe_intf_head	:10;
	uint32 rsv_1	:6;
	uint32 epe_intf_tail	:10;

	uint32 rsv_2	:6;
	uint32 fabric_intf_head	:10;
	uint32 rsv_3	:6;
	uint32 fabric_intf_tail	:10;

	uint32 rsv_4	:6;
	uint32 ipe_intf_head	:10;
	uint32 rsv_5	:6;
	uint32 ipe_intf_tail	:10;

	uint32 rsv_6	:6;
	uint32 oam_intf_head	:10;
	uint32 rsv_7	:6;
	uint32 oam_intf_tail	:10;

	uint32 rsv_8	:6;
	uint32 cpu_intf_head	:10;
	uint32 rsv_9	:6;
	uint32 cpu_intf_tail	:10;
};
typedef struct buf_retrv_intf_mem_status_s buf_retrv_intf_mem_status_t;

struct buffer_retrieve_ctl_s	/* 21 */
{

	uint32 rsv_0	:31;
	uint32 color_map_en	:1;
};
typedef struct buffer_retrieve_ctl_s buffer_retrieve_ctl_t;

struct buffer_retrieve_header_version_s	/* 22 */
{

	uint32 dest_chip31_to16_header_version	:32;

	uint32 dest_chip15_to0_header_version	:32;
};
typedef struct buffer_retrieve_header_version_s buffer_retrieve_header_version_t;

struct buf_retrv_rcd_credit_debug_s	/* 23 */
{

	uint32 rsv_0	:23;
	uint32 rcd_credit_used	:9;
};
typedef struct buf_retrv_rcd_credit_debug_s buf_retrv_rcd_credit_debug_t;

struct buf_retrv_pkt_ptr_wt_config_s	/* 24 */
{

	uint32 rsv_0	:1;
	uint32 oam_pkt_weight_config	:7;
	uint32 rsv_1	:9;
	uint32 fabric_pkt_weight_config	:7;
	uint32 rsv_2	:1;
	uint32 network_pkt_weight_config	:7;

	uint32 rsv_3	:9;
	uint32 network_pktsg_weight_config	:7;
	uint32 rsv_4	:9;
	uint32 network_pkt1g_weight_config	:7;
};
typedef struct buf_retrv_pkt_ptr_wt_config_s buf_retrv_pkt_ptr_wt_config_t;

struct buf_retrv_buf_ptr_wt_config_s	/* 25 */
{

	uint32 rsv_0	:1;
	uint32 oam_buf_weight_config	:7;
	uint32 rsv_1	:1;
	uint32 lpbk_buf_weight_config	:7;
	uint32 rsv_2	:1;
	uint32 fabric_buf_weight_config	:7;
	uint32 rsv_3	:1;
	uint32 network_buf_weight_config	:7;

	uint32 rsv_4	:9;
	uint32 network_bufe_loop_weight_config	:7;
	uint32 rsv_5	:1;
	uint32 network_bufsg_weight_config	:7;
	uint32 rsv_6	:1;
	uint32 network_buf1g_weight_config	:7;
};
typedef struct buf_retrv_buf_ptr_wt_config_s buf_retrv_buf_ptr_wt_config_t;

struct buf_retrv_net_max_credit_debug_s	/* 26 */
{

	uint32 rsv_0	:26;
	uint32 net_max_credit_chan	:6;

	uint32 rsv_1	:24;
	uint32 net_max_credit	:8;

	uint32 rsv_2	:31;
	uint32 epe_adjust_fifo_stall_record	:1;
};
typedef struct buf_retrv_net_max_credit_debug_s buf_retrv_net_max_credit_debug_t;

struct buf_store_share_resrc_info_s	/* 27 */
{

	uint32 rsv_0	:16;
	uint32 shared_resrc_threshold	:16;

	uint32 rsv_1	:16;
	uint32 shared_resrc_cnt	:16;
};
typedef struct buf_store_share_resrc_info_s buf_store_share_resrc_info_t;

struct buf_store_ctrl_s	/* 28 */
{

	uint32 rsv_0	:19;
	uint32 bay_id	:5;
	uint32 sgmac_en	:1;
	uint32 bay_id_chk_disable	:1;
	uint32 rsv_1	:2;
	uint32 mcast_met_fifo_enable	:1;
	uint32 resrc_id_use_local_phy_port	:1;
	uint32 mergee_loop_resrc_id	:1;
	uint32 local_switching_disable	:1;
};
typedef struct buf_store_ctrl_s buf_store_ctrl_t;

struct buf_store_channel_info_ctrl_s	/* 29 */
{

	uint32 rsv_0	:31;
	uint32 channel_info_init	:1;

	uint32 rsv_1	:31;
	uint32 channel_info_init_done	:1;
};
typedef struct buf_store_channel_info_ctrl_s buf_store_channel_info_ctrl_t;

struct buf_store_resrc_threshold_ram_ctrl_s	/* 30 */
{

	uint32 rsv_0	:31;
	uint32 resrc_threshold_ram_init	:1;

	uint32 rsv_1	:31;
	uint32 resrc_threshold_ram_init_done	:1;
};
typedef struct buf_store_resrc_threshold_ram_ctrl_s buf_store_resrc_threshold_ram_ctrl_t;

struct buf_store_resrc_cnt_ctrl_s	/* 31 */
{

	uint32 rsv_0	:31;
	uint32 resrc_cnt_ram_init	:1;

	uint32 rsv_1	:31;
	uint32 resrc_cnt_ram_init_done	:1;
};
typedef struct buf_store_resrc_cnt_ctrl_s buf_store_resrc_cnt_ctrl_t;

struct buf_store_stall_threshold_ram_ctrl_s	/* 32 */
{

	uint32 rsv_0	:31;
	uint32 stall_threshold_ram_init	:1;

	uint32 rsv_1	:31;
	uint32 stall_threshold_ram_init_done	:1;
};
typedef struct buf_store_stall_threshold_ram_ctrl_s buf_store_stall_threshold_ram_ctrl_t;

struct buf_store_force_local_ctrl_s	/* 33 */
{

	uint32 rsv_0	:10;
	uint32 dest_map_mask0	:22;

	uint32 rsv_1	:10;
	uint32 dest_map_value0	:22;

	uint32 rsv_2	:10;
	uint32 dest_map_mask1	:22;

	uint32 rsv_3	:10;
	uint32 dest_map_value1	:22;

	uint32 rsv_4	:10;
	uint32 dest_map_mask2	:22;

	uint32 rsv_5	:10;
	uint32 dest_map_value2	:22;

	uint32 rsv_6	:10;
	uint32 dest_map_mask3	:22;

	uint32 rsv_7	:10;
	uint32 dest_map_value3	:22;
};
typedef struct buf_store_force_local_ctrl_s buf_store_force_local_ctrl_t;

struct buf_store_free_list_control_s	/* 34 */
{

	uint32 rsv_0	:17;
	uint32 free_list_head_ptr	:15;

	uint32 rsv_1	:17;
	uint32 free_list_tail_ptr	:15;

	uint32 rsv_2	:1;
	uint32 min_free_list_buf_cnt	:15;
	uint32 rsv_3	:1;
	uint32 free_list_buf_cnt	:15;

	uint32 rsv_4	:5;
	uint32 free_buf_fifo_depth	:3;
	uint32 rsv_5	:11;
	uint32 single_buf_release_fifo0_depth	:5;
	uint32 rsv_6	:3;
	uint32 single_buf_release_fifo1_depth	:5;

	uint32 rsv_7	:17;
	uint32 prefetch_fifo0_depth	:7;
	uint32 rsv_8	:1;
	uint32 prefetch_fifo1_depth	:7;
};
typedef struct buf_store_free_list_control_s buf_store_free_list_control_t;

struct buf_store_link_list_table_ctrl_s	/* 35 */
{

	uint32 rsv_0	:17;
	uint32 buf_ptr_table_init_addr_num	:15;

	uint32 rsv_1	:31;
	uint32 buf_ptr_table_init	:1;

	uint32 rsv_2	:31;
	uint32 buf_ptr_table_init_done	:1;
};
typedef struct buf_store_link_list_table_ctrl_s buf_store_link_list_table_ctrl_t;

struct buf_store_gmac_stall_ctrl_s	/* 36 */
{

	uint32 buf_store_gmac_low_stall_disable	:32;

	uint32 rsv_0	:16;
	uint32 buf_store_gmac_high_stall_disable	:16;
};
typedef struct buf_store_gmac_stall_ctrl_s buf_store_gmac_stall_ctrl_t;

struct buf_store_stall_status_s	/* 37 */
{

	uint32 rsv_0	:24;
	uint32 buf_store_xgmac_stall_record	:8;

	uint32 rsv_1	:31;
	uint32 buf_store_cpu_mac_stall_record	:1;

	uint32 buf_store_gmac_high_stall_record	:32;

	uint32 rsv_2	:16;
	uint32 buf_store_gmac_low_stall_record	:16;

	uint32 rsv_3	:28;
	uint32 buf_store_fabric_cas_stall_record	:4;
};
typedef struct buf_store_stall_status_s buf_store_stall_status_t;

struct buf_store_credit_s	/* 38 */
{

	uint32 rsv_0	:27;
	uint32 pb_ctl_credit_threshold	:5;
};
typedef struct buf_store_credit_s buf_store_credit_t;

struct buf_store_xgmac_stall_ctrl_s	/* 39 */
{

	uint32 rsv_0	:3;
	uint32 buf_store_xgmac7_stall_disable	:1;
	uint32 rsv_1	:3;
	uint32 buf_store_xgmac6_stall_disable	:1;
	uint32 rsv_2	:3;
	uint32 buf_store_xgmac5_stall_disable	:1;
	uint32 rsv_3	:3;
	uint32 buf_store_xgmac4_stall_disable	:1;
	uint32 rsv_4	:3;
	uint32 buf_store_xgmac3_stall_disable	:1;
	uint32 rsv_5	:3;
	uint32 buf_store_xgmac2_stall_disable	:1;
	uint32 rsv_6	:3;
	uint32 buf_store_xgmac1_stall_disable	:1;
	uint32 rsv_7	:3;
	uint32 buf_store_xgmac0_stall_disable	:1;
};
typedef struct buf_store_xgmac_stall_ctrl_s buf_store_xgmac_stall_ctrl_t;

struct buf_store_cpu_mac_stall_ctrl_s	/* 40 */
{

	uint32 rsv_0	:31;
	uint32 buf_store_cpu_mac_stall_disable	:1;
};
typedef struct buf_store_cpu_mac_stall_ctrl_s buf_store_cpu_mac_stall_ctrl_t;

struct buf_store_fabric_stall_ctrl_s	/* 41 */
{

	uint32 rsv_0	:19;
	uint32 buf_store_fabric3_stall_disable	:1;
	uint32 rsv_1	:3;
	uint32 buf_store_fabric2_stall_disable	:1;
	uint32 rsv_2	:3;
	uint32 buf_store_fabric1_stall_disable	:1;
	uint32 rsv_3	:3;
	uint32 buf_store_fabric0_stall_disable	:1;
};
typedef struct buf_store_fabric_stall_ctrl_s buf_store_fabric_stall_ctrl_t;

struct buf_store_met_fifo_stall_ctrl_s	/* 42 */
{

	uint32 rsv_0	:18;
	uint32 met_fifo_from_fabric_force_mcast_to_high	:1;
	uint32 met_fifo_from_fabric_force_ucast_to_high	:1;
	uint32 rsv_1	:3;
	uint32 met_fifo_from_fabric_force_enable	:1;
	uint32 rsv_2	:4;
	uint32 met_fifo_mcast_high_stall_enable	:1;
	uint32 met_fifo_mcast_low_stall_enable	:1;
	uint32 met_fifo_ucast_high_stall_enable	:1;
	uint32 met_fifo_ucast_low_stall_enable	:1;
};
typedef struct buf_store_met_fifo_stall_ctrl_s buf_store_met_fifo_stall_ctrl_t;

struct buf_store_misc_ctrl_s	/* 43 */
{

	uint32 rsv_0	:27;
	uint32 resrc_mgr_disable	:1;
	uint32 mismatched_len_error_chk_enable	:1;
	uint32 over_len_error_chk_enable	:1;
	uint32 under_len_error_chk_enable	:1;
	uint32 drain_enable	:1;

	uint32 rsv_1	:22;
	uint32 max_buf_cnt	:6;
	uint32 rsv_2	:3;
	uint32 max_buf_cnt_chk_enable	:1;

	uint32 rsv_3	:2;
	uint32 max_pkt_size	:14;
	uint32 rsv_4	:2;
	uint32 min_pkt_size	:14;
};
typedef struct buf_store_misc_ctrl_s buf_store_misc_ctrl_t;

struct buf_store_link_list_slot_s	/* 44 */
{

	uint32 rsv_0	:22;
	uint32 buf_ptr_pkt_release_wr_slot_num	:2;
	uint32 rsv_1	:2;
	uint32 buf_ptr_link_buf_wr_slot_num	:2;
	uint32 rsv_2	:2;
	uint32 buf_ptr_free_buf_rd_slot_num	:2;
};
typedef struct buf_store_link_list_slot_s buf_store_link_list_slot_t;

struct buf_store_fifo_ctrl_s	/* 45 */
{

	uint32 rsv_0	:9;
	uint32 resrc_return_fifoa_full_thrd_for_abort	:7;
	uint32 rsv_1	:9;
	uint32 resrc_return_fifoa_full_thrd_for_release	:7;

	uint32 rsv_2	:9;
	uint32 pkt_release_wr_fifoa_full_thrd_for_abort	:7;
	uint32 rsv_3	:9;
	uint32 pkt_release_wr_fifoa_full_thrd_for_release	:7;

	uint32 rsv_4	:25;
	uint32 link_buf_wr_fifo_almost_full_threshold	:7;

	uint32 rsv_5	:26;
	uint32 pkt_release_fifo_almost_full_threshold	:6;

	uint32 rsv_6	:26;
	uint32 pb_wr_track_fifoa_full_thrd	:6;

	uint32 rsv_7	:27;
	uint32 msg_out_fifoa_full_thrd	:5;

	uint32 rsv_8	:25;
	uint32 link_buf_bank_offset_fifoa_full_thrd	:7;

	uint32 rsv_9	:23;
	uint32 ipe_eop_channel_info_fifoa_full_thrd	:9;
};
typedef struct buf_store_fifo_ctrl_s buf_store_fifo_ctrl_t;

struct buf_store_msg_out_drop_ctl_s	/* 46 */
{

	uint32 rsv_0	:12;
	uint32 oam_ucast_high_drop_en	:1;
	uint32 oam_ucast_low_drop_en	:1;
	uint32 oam_mcast_high_drop_en	:1;
	uint32 oam_mcast_low_drop_en	:1;
	uint32 cpu_ucast_high_drop_en	:1;
	uint32 cpu_ucast_low_drop_en	:1;
	uint32 cpu_mcast_high_drop_en	:1;
	uint32 cpu_mcast_low_drop_en	:1;
	uint32 e_loop_ucast_high_drop_en	:1;
	uint32 e_loop_ucast_low_drop_en	:1;
	uint32 e_loop_mcast_high_drop_en	:1;
	uint32 e_loop_mcast_low_drop_en	:1;
	uint32 fabric_ucast_high_drop_en	:1;
	uint32 fabric_ucast_low_drop_en	:1;
	uint32 fabric_mcast_high_drop_en	:1;
	uint32 fabric_mcast_low_drop_en	:1;
	uint32 ipe_ucast_high_drop_en	:1;
	uint32 ipe_ucast_low_drop_en	:1;
	uint32 ipe_mcast_high_drop_en	:1;
	uint32 ipe_mcast_low_drop_en	:1;
};
typedef struct buf_store_msg_out_drop_ctl_s buf_store_msg_out_drop_ctl_t;

struct buf_store_msg_out_drop_thrd_sel_s	/* 47 */
{

	uint32 rsv_0	:12;
	uint32 oam_ucast_high_drop_thrd_sel	:1;
	uint32 oam_ucast_low_drop_thrd_sel	:1;
	uint32 oam_mcast_high_drop_thrd_sel	:1;
	uint32 oam_mcast_low_drop_thrd_sel	:1;
	uint32 cpu_ucast_high_drop_thrd_sel	:1;
	uint32 cpu_ucast_low_drop_thrd_sel	:1;
	uint32 cpu_mcast_high_drop_thrd_sel	:1;
	uint32 cpu_mcast_low_drop_thrd_sel	:1;
	uint32 e_loop_ucast_high_drop_thrd_sel	:1;
	uint32 e_loop_ucast_low_drop_thrd_sel	:1;
	uint32 e_loop_mcast_high_drop_thrd_sel	:1;
	uint32 e_loop_mcast_low_drop_thrd_sel	:1;
	uint32 fabric_ucast_high_drop_thrd_sel	:1;
	uint32 fabric_ucast_low_drop_thrd_sel	:1;
	uint32 fabric_mcast_high_drop_thrd_sel	:1;
	uint32 fabric_mcast_low_drop_thrd_sel	:1;
	uint32 ipe_ucast_high_drop_thrd_sel	:1;
	uint32 ipe_ucast_low_drop_thrd_sel	:1;
	uint32 ipe_mcast_high_drop_thrd_sel	:1;
	uint32 ipe_mcast_low_drop_thrd_sel	:1;
};
typedef struct buf_store_msg_out_drop_thrd_sel_s buf_store_msg_out_drop_thrd_sel_t;

struct buf_store_input_stats_s	/* 48 */
{

	uint32 rsv_0	:28;
	uint32 pkt_in_fake_sop_ipe_body_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pkt_in_eop_ipe_body_cnt	:4;

	uint32 rsv_2	:28;
	uint32 pkt_in_pkt_err_ipe_body_cnt	:4;

	uint32 rsv_3	:28;
	uint32 pkt_in_data_err_ipe_body_cnt	:4;

	uint32 rsv_4	:28;
	uint32 pkt_in_sop_ipe_hdr_cnt	:4;

	uint32 rsv_5	:28;
	uint32 pkt_in_data_err_ipe_hdr_cnt	:4;

	uint32 rsv_6	:28;
	uint32 pkt_in_sop_fabric_rxq_cnt	:4;

	uint32 rsv_7	:28;
	uint32 pkt_in_eop_fabric_rxq_cnt	:4;

	uint32 rsv_8	:28;
	uint32 pkt_in_pkt_err_fabric_rxq_cnt	:4;

	uint32 rsv_9	:28;
	uint32 pkt_in_data_err_fabric_rxq_cnt	:4;

	uint32 rsv_10	:28;
	uint32 pkt_in_sope_loop_cnt	:4;

	uint32 rsv_11	:28;
	uint32 pkt_in_eope_loop_cnt	:4;

	uint32 rsv_12	:28;
	uint32 pkt_in_pkt_erre_loop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 pkt_in_data_erre_loop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 pkt_in_sop_cpu_cnt	:4;

	uint32 rsv_15	:28;
	uint32 pkt_in_eop_cpu_cnt	:4;

	uint32 rsv_16	:28;
	uint32 pkt_in_pkt_err_cpu_cnt	:4;

	uint32 rsv_17	:28;
	uint32 pkt_in_data_err_cpu_cnt	:4;

	uint32 rsv_18	:28;
	uint32 pkt_in_sop_oam_cnt	:4;

	uint32 rsv_19	:28;
	uint32 pkt_in_eop_oam_cnt	:4;

	uint32 rsv_20	:28;
	uint32 pkt_in_pkt_err_oam_cnt	:4;

	uint32 rsv_21	:28;
	uint32 pkt_in_data_err_oam_cnt	:4;
};
typedef struct buf_store_input_stats_s buf_store_input_stats_t;

struct buf_store_stats_ctrl_s	/* 49 */
{

	uint32 rsv_0	:15;
	uint32 oam_stats_upd_en	:1;
	uint32 rsv_1	:3;
	uint32 cpu_stats_upd_en	:1;
	uint32 rsv_2	:3;
	uint32 e_loop_stats_upd_en	:1;
	uint32 rsv_3	:3;
	uint32 fabric_stats_upd_en	:1;
	uint32 rsv_4	:3;
	uint32 ipe_stats_upd_en	:1;
};
typedef struct buf_store_stats_ctrl_s buf_store_stats_ctrl_t;

struct buf_store_silent_drop_stats_s	/* 50 */
{

	uint32 rsv_0	:28;
	uint32 pkt_silent_drop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pkt_silent_drop_pkt_error_cnt	:4;

	uint32 rsv_2	:28;
	uint32 pkt_silent_drop_data_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 pkt_silent_drop_under_len_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 pkt_silent_drop_over_len_error_cnt	:4;

	uint32 rsv_5	:28;
	uint32 pkt_silent_drop_no_resrc_yes_buf_cnt	:4;

	uint32 rsv_6	:28;
	uint32 pkt_silent_drop_yes_resrc_no_buf_cnt	:4;

	uint32 rsv_7	:28;
	uint32 pkt_silent_drop_no_resrc_no_buf_cnt	:4;

	uint32 rsv_8	:28;
	uint32 pkt_silent_drop_bay_id_mismatch_err_cnt	:4;

	uint32 rsv_9	:28;
	uint32 pkt_silent_drop_miss_sop_err_cnt	:4;
};
typedef struct buf_store_silent_drop_stats_s buf_store_silent_drop_stats_t;

struct buf_store_abort_stats_s	/* 51 */
{

	uint32 rsv_0	:28;
	uint32 pkt_abort_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pkt_abort_pkt_error_cnt	:4;

	uint32 rsv_2	:28;
	uint32 pkt_abort_data_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 pkt_abort_framing_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 pkt_abort_no_buf_cnt	:4;

	uint32 rsv_5	:28;
	uint32 pkt_abort_mismatch_len_error_cnt	:4;

	uint32 rsv_6	:28;
	uint32 pkt_abort_over_max_buf_len_error_cnt	:4;
};
typedef struct buf_store_abort_stats_s buf_store_abort_stats_t;

struct buf_store_ipe_sop_abort_stats_s	/* 52 */
{

	uint32 rsv_0	:28;
	uint32 pkt_silent_drop_at_ipe_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pkt_abort_at_ipe_sop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 pkt_abort_at_ipe_sop_data_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 pkt_abort_at_ipe_sop_under_len_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 pkt_abort_at_ipe_sop_over_len_error_cnt	:4;

	uint32 rsv_5	:28;
	uint32 pkt_abort_at_ipe_sop_hard_discard_cnt	:4;

	uint32 rsv_6	:28;
	uint32 pkt_abort_at_ipe_sop_no_resrc_cnt	:4;

	uint32 rsv_7	:28;
	uint32 pkt_abort_at_ipe_sop_bay_id_mismatch_err_cnt	:4;
};
typedef struct buf_store_ipe_sop_abort_stats_s buf_store_ipe_sop_abort_stats_t;

struct buf_store_stall_drop_stats_s	/* 53 */
{

	uint32 rsv_0	:28;
	uint32 ipe_ucast_drop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 ipe_mcast_drop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 fabric_ucast_drop_cnt	:4;

	uint32 rsv_3	:28;
	uint32 fabric_mcast_drop_cnt	:4;

	uint32 rsv_4	:28;
	uint32 e_loop_ucast_drop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 e_loop_mcast_drop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 cpu_ucast_drop_cnt	:4;

	uint32 rsv_7	:28;
	uint32 cpu_mcast_drop_cnt	:4;

	uint32 rsv_8	:28;
	uint32 oam_ucast_drop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 oam_mcast_drop_cnt	:4;
};
typedef struct buf_store_stall_drop_stats_s buf_store_stall_drop_stats_t;

struct buf_store_output_stats_s	/* 54 */
{

	uint32 rsv_0	:28;
	uint32 to_met_fifo_pkt_cnt	:4;

	uint32 rsv_1	:24;
	uint32 to_met_fifo_buf_cnt	:8;

	uint32 rsv_2	:28;
	uint32 fr_met_fifo_release_pkt_cnt	:4;

	uint32 rsv_3	:24;
	uint32 fr_met_fifo_release_buf_cnt	:8;
};
typedef struct buf_store_output_stats_s buf_store_output_stats_t;

struct buf_store_input_fifo_wrr_weight_s	/* 55 */
{

	uint32 rsv_0	:24;
	uint32 input_fifo_wrr_weight_fabric_rxq	:8;

	uint32 rsv_1	:24;
	uint32 input_fifo_wrr_weight_ipe	:8;

	uint32 rsv_2	:24;
	uint32 input_fifo_wrr_weighte_loop	:8;

	uint32 rsv_3	:24;
	uint32 input_fifo_wrr_weight_oam	:8;
};
typedef struct buf_store_input_fifo_wrr_weight_s buf_store_input_fifo_wrr_weight_t;

struct buf_store_interrupt_s	/* 56 */
{

	uint32 mask_set_fatal0	:32;

	uint32 mask_reset_fatal0	:32;

	uint32 value_set_fatal0	:32;

	uint32 value_reset_fatal0	:32;

	uint32 mask_set_fatal1	:32;

	uint32 mask_reset_fatal1	:32;

	uint32 value_set_fatal1	:32;

	uint32 value_reset_fatal1	:32;

	uint32 rsv_0	:24;
	uint32 mask_set_fatal2	:8;

	uint32 rsv_1	:24;
	uint32 mask_reset_fatal2	:8;

	uint32 rsv_2	:24;
	uint32 value_set_fatal2	:8;

	uint32 rsv_3	:24;
	uint32 value_reset_fatal2	:8;
};
typedef struct buf_store_interrupt_s buf_store_interrupt_t;

struct buf_store_pb_ctl_credit_used_s	/* 57 */
{

	uint32 rsv_0	:27;
	uint32 pb_ctl_credit_used	:5;
};
typedef struct buf_store_pb_ctl_credit_used_s buf_store_pb_ctl_credit_used_t;

struct buf_store_pb_ctl_credit_run_out_cnt_s	/* 58 */
{

	uint32 rsv_0	:24;
	uint32 pb_ctl_credit_run_out_cnt	:8;
};
typedef struct buf_store_pb_ctl_credit_run_out_cnt_s buf_store_pb_ctl_credit_run_out_cnt_t;

struct buf_store_ecc_ctl_s	/* 59 */
{

	uint32 rsv_0	:23;
	uint32 cfg_report_single_bit_error	:1;
	uint32 rsv_1	:3;
	uint32 cfg_ecc_correct_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_ecc_check_en	:1;
};
typedef struct buf_store_ecc_ctl_s buf_store_ecc_ctl_t;

struct buf_store_ecc_error_stats_s	/* 60 */
{

	uint32 rsv_0	:16;
	uint32 cfg_ecc_single_bit_count	:8;
	uint32 cfg_ecc_multiple_bit_count	:8;
};
typedef struct buf_store_ecc_error_stats_s buf_store_ecc_error_stats_t;

struct cpu_mac_ctl_s	/* 61 */
{

	uint32 rsv_0	:7;
	uint32 egress_fifo_afull_thrd	:9;
	uint32 rsv_1	:11;
	uint32 egress_add_en	:1;
	uint32 rsv_2	:3;
	uint32 ingress_remove_en	:1;
};
typedef struct cpu_mac_ctl_s cpu_mac_ctl_t;

struct cpu_mac_drain_enable_s	/* 62 */
{

	uint32 rsv_0	:31;
	uint32 egress_drain_enable	:1;
};
typedef struct cpu_mac_drain_enable_s cpu_mac_drain_enable_t;

struct cpu_mac_pause_ctrl_s	/* 63 */
{

	uint32 rsv_0	:23;
	uint32 gmac_pause_frame_dis	:1;
	uint32 rsv_1	:7;
	uint32 reset_gmac	:1;

	uint32 gmac_pause_timer_out	:32;
};
typedef struct cpu_mac_pause_ctrl_s cpu_mac_pause_ctrl_t;

struct cpu_mac_cpu_channel_s	/* 64 */
{

	uint32 rsv_0	:24;
	uint32 cfg_cpu_channel	:8;
};
typedef struct cpu_mac_cpu_channel_s cpu_mac_cpu_channel_t;

struct cpu_mac_type_s	/* 65 */
{

	uint32 cpu_mac_reserved	:16;
	uint32 cpu_mac_type	:16;
};
typedef struct cpu_mac_type_s cpu_mac_type_t;

struct clear_cnt_on_read_s	/* 66 */
{

	uint32 rsv_0	:31;
	uint32 clear_cnt_on_read	:1;
};
typedef struct clear_cnt_on_read_s clear_cnt_on_read_t;

struct cpu_mac_sa_s	/* 67 */
{

	uint32 cpu_mac_sa31_to0	:32;

	uint32 rsv_0	:16;
	uint32 cpu_mac_sa47_to32	:16;
};
typedef struct cpu_mac_sa_s cpu_mac_sa_t;

struct cpu_mac_da_s	/* 68 */
{

	uint32 cpu_mac0_da31_to0	:32;

	uint32 rsv_0	:16;
	uint32 cpu_mac0_da47_to32	:16;

	uint32 cpu_mac1_da31_to0	:32;

	uint32 rsv_1	:16;
	uint32 cpu_mac1_da47_to32	:16;

	uint32 cpu_mac2_da31_to0	:32;

	uint32 rsv_2	:16;
	uint32 cpu_mac2_da47_to32	:16;

	uint32 cpu_mac3_da31_to0	:32;

	uint32 rsv_3	:16;
	uint32 cpu_mac3_da47_to32	:16;

	uint32 cpu_mac4_da31_to0	:32;

	uint32 rsv_4	:16;
	uint32 cpu_mac4_da47_to32	:16;

	uint32 cpu_mac5_da31_to0	:32;

	uint32 rsv_5	:16;
	uint32 cpu_mac5_da47_to32	:16;

	uint32 cpu_mac6_da31_to0	:32;

	uint32 rsv_6	:16;
	uint32 cpu_mac6_da47_to32	:16;

	uint32 cpu_mac7_da31_to0	:32;

	uint32 rsv_7	:16;
	uint32 cpu_mac7_da47_to32	:16;
};
typedef struct cpu_mac_da_s cpu_mac_da_t;

struct cpu_mac_debug_stats_s	/* 69 */
{

	uint32 rsv_0	:8;
	uint32 from_buf_retrv_byte_cnt	:8;
	uint32 rsv_1	:4;
	uint32 from_buf_retrv_pkt_err_cnt	:4;
	uint32 from_buf_retrv_eop_cnt	:4;
	uint32 from_buf_retrv_sop_cnt	:4;

	uint32 rsv_2	:8;
	uint32 to_buf_store_byte_cnt	:8;
	uint32 rsv_3	:4;
	uint32 to_buf_store_pkt_err_cnt	:4;
	uint32 to_buf_store_eop_cnt	:4;
	uint32 to_buf_store_sop_cnt	:4;
};
typedef struct cpu_mac_debug_stats_s cpu_mac_debug_stats_t;

struct cpumac_gmac_mac_mode_s	/* 70 */
{

	uint32 rsv_0	:30;
	uint32 speed_mode	:2;
};
typedef struct cpumac_gmac_mac_mode_s cpumac_gmac_mac_mode_t;

struct cpumac_gmac_tx_ctrl_s	/* 71 */
{

	uint32 rsv_0	:12;
	uint32 full_threshold	:7;
	uint32 tx_threshold	:7;
	uint32 tx_err_mask	:1;
	uint32 append_crc_enable	:1;
	uint32 pad_enable	:1;
	uint32 tx_force_send_pause	:1;
	uint32 tx_flow_ctrl_enable	:1;
	uint32 tx_enable	:1;
};
typedef struct cpumac_gmac_tx_ctrl_s cpumac_gmac_tx_ctrl_t;

struct cpumac_gmac_rx_ctrl_s	/* 72 */
{

	uint32 rsv_0	:25;
	uint32 crc_error_mask	:1;
	uint32 runt_rcv_enable	:1;
	uint32 len_field_chk_enable	:1;
	uint32 crc_chk_enable	:1;
	uint32 rx_flow_ctrl_enable	:1;
	uint32 rx_pause_bypass	:1;
	uint32 rx_enable	:1;
};
typedef struct cpumac_gmac_rx_ctrl_s cpumac_gmac_rx_ctrl_t;

struct cpumac_gmac_pre_length_s	/* 73 */
{

	uint32 rsv_0	:27;
	uint32 pre_length	:5;
};
typedef struct cpumac_gmac_pre_length_s cpumac_gmac_pre_length_t;

struct cpumac_gmac_pkt_length_s	/* 74 */
{

	uint32 rsv_0	:24;
	uint32 min_pkt_len	:8;
};
typedef struct cpumac_gmac_pkt_length_s cpumac_gmac_pkt_length_t;

struct cpumac_gmac_interrupt_s	/* 75 */
{

	uint32 rsv_0	:20;
	uint32 value_set	:12;

	uint32 rsv_1	:20;
	uint32 value_reset	:12;

	uint32 rsv_2	:20;
	uint32 mask_set	:12;

	uint32 rsv_3	:20;
	uint32 mask_reset	:12;
};
typedef struct cpumac_gmac_interrupt_s cpumac_gmac_interrupt_t;

struct cpumac_gmac_pause_ctrl_s	/* 76 */
{

	uint32 rsv_0	:16;
	uint32 pause_quanta	:16;
};
typedef struct cpumac_gmac_pause_ctrl_s cpumac_gmac_pause_ctrl_t;

struct cpumac_gmac_vlan_type_s	/* 77 */
{

	uint32 rsv_0	:16;
	uint32 vlan_type	:16;
};
typedef struct cpumac_gmac_vlan_type_s cpumac_gmac_vlan_type_t;

struct cpu_mac_packet_len_mtu1_s	/* 78 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct cpu_mac_packet_len_mtu1_s cpu_mac_packet_len_mtu1_t;

struct cpu_mac_packet_len_mtu2_s	/* 79 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct cpu_mac_packet_len_mtu2_s cpu_mac_packet_len_mtu2_t;

struct cpu_mac_dot1q_delta_bytes_s	/* 80 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct cpu_mac_dot1q_delta_bytes_s cpu_mac_dot1q_delta_bytes_t;

struct cpu_mac_init_s	/* 81 */
{

	uint32 rsv_0	:31;
	uint32 cpu_mac_init	:1;
};
typedef struct cpu_mac_init_s cpu_mac_init_t;

struct cpu_mac_init_done_s	/* 82 */
{

	uint32 rsv_0	:31;
	uint32 cpu_mac_init	:1;
};
typedef struct cpu_mac_init_done_s cpu_mac_init_done_t;

struct cpu_mac_stats_update_ctrl_s	/* 83 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct cpu_mac_stats_update_ctrl_s cpu_mac_stats_update_ctrl_t;

struct cpu_mac_parity_enable_s	/* 84 */
{

	uint32 rsv_0	:31;
	uint32 parity_en	:1;
};
typedef struct cpu_mac_parity_enable_s cpu_mac_parity_enable_t;

struct cpu_mac_status_over_write_s	/* 85 */
{

	uint32 rsv_0	:27;
	uint32 update_fifo_status	:1;
	uint32 rsv_1	:3;
	uint32 status_over_write	:1;
};
typedef struct cpu_mac_status_over_write_s cpu_mac_status_over_write_t;

struct cpu_mac_status_over_write_old_snap_s	/* 86 */
{

	uint32 rsv_0	:27;
	uint32 update_fifo_status	:1;
	uint32 rsv_1	:3;
	uint32 status_over_write	:1;
};
typedef struct cpu_mac_status_over_write_old_snap_s cpu_mac_status_over_write_old_snap_t;

struct cpu_mac_status_over_write_new_snap_s	/* 87 */
{

	uint32 rsv_0	:27;
	uint32 update_fifo_status	:1;
	uint32 rsv_1	:3;
	uint32 status_over_write	:1;
};
typedef struct cpu_mac_status_over_write_new_snap_s cpu_mac_status_over_write_new_snap_t;

struct cpu_mac_max_init_cnt_s	/* 88 */
{

	uint32 rsv_0	:26;
	uint32 max_init_cnt	:6;
};
typedef struct cpu_mac_max_init_cnt_s cpu_mac_max_init_cnt_t;

struct cpu_mac_stats_ram_parity_error_addr_s	/* 89 */
{

	uint32 rsv_0	:26;
	uint32 cpu_mac_stats_mem_parity_err_addr	:6;
};
typedef struct cpu_mac_stats_ram_parity_error_addr_s cpu_mac_stats_ram_parity_error_addr_t;

struct cpu_mac_pause_off_enable_s	/* 90 */
{

	uint32 rsv_0	:31;
	uint32 pause_off_enable	:1;
};
typedef struct cpu_mac_pause_off_enable_s cpu_mac_pause_off_enable_t;

struct e_loop_ctl_s	/* 98 */
{

	uint32 rsv_0	:8;
	uint32 stall_loop_threshold	:8;
	uint32 rsv_1	:8;
	uint32 drop_log_threshold	:8;

	uint32 rsv_2	:24;
	uint32 drop_loop_threshold	:8;

	uint32 rsv_3	:28;
	uint32 buf_store_credit	:4;

	uint32 rsv_4	:31;
	uint32 parity_check_en	:1;
};
typedef struct e_loop_ctl_s e_loop_ctl_t;

struct e_loop_drain_enable_s	/* 99 */
{

	uint32 rsv_0	:31;
	uint32 e_loop_drain_enable	:1;
};
typedef struct e_loop_drain_enable_s e_loop_drain_enable_t;

struct e_loop_interrupt_fatal_s	/* 100 */
{

	uint32 rsv_0	:31;
	uint32 value_set_fatal	:1;

	uint32 rsv_1	:31;
	uint32 value_reset_fatal	:1;

	uint32 rsv_2	:31;
	uint32 mask_set_fatal	:1;

	uint32 rsv_3	:31;
	uint32 mask_reset_fatal	:1;
};
typedef struct e_loop_interrupt_fatal_s e_loop_interrupt_fatal_t;

struct e_loop_debug_stats_s	/* 101 */
{

	uint32 rsv_0	:28;
	uint32 pkt_in_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pkt_out_cnt	:4;

	uint32 rsv_2	:28;
	uint32 dropped_pkt_cnt	:4;

	uint32 rsv_3	:28;
	uint32 early_terminated_pkt_cnt	:4;
};
typedef struct e_loop_debug_stats_s e_loop_debug_stats_t;

struct e_loop_credit_state_s	/* 102 */
{

	uint32 rsv_0	:28;
	uint32 credit_used	:4;
};
typedef struct e_loop_credit_state_s e_loop_credit_state_t;

struct e_loop_parity_fail_record_s	/* 103 */
{

	uint32 eloop_mem_parity_fail	:1;
	uint32 rsv_0	:7;
	uint32 eloop_mem_parity_fail_addr	:8;
	uint32 rsv_1	:16;
};
typedef struct e_loop_parity_fail_record_s e_loop_parity_fail_record_t;

struct epe_acl_qos_ctl_s	/* 104 */
{

	uint32 rsv_0	:16;
	uint32 acl_qos_lookup_ctl0	:16;

	uint32 rsv_1	:16;
	uint32 acl_qos_lookup_ctl1	:16;

	uint32 rsv_2	:16;
	uint32 acl_qos_lookup_ctl2	:16;

	uint32 rsv_3	:18;
	uint32 qos_high_priority	:1;
	uint32 rsv_4	:1;
	uint32 oam_obey_acl_qos	:1;
	uint32 dual_acl_lookup	:1;
	uint32 acl_use_packet_vlan	:1;
	uint32 merge_mac_ip_acl_key	:1;
	uint32 rsv_5	:8;

	uint32 rsv_6	:25;
	uint32 acl_lookup_result_ctl038_to32	:7;

	uint32 acl_lookup_result_ctl031_to0	:32;

	uint32 rsv_7	:25;
	uint32 acl_lookup_result_ctl138_to32	:7;

	uint32 acl_lookup_result_ctl131_to0	:32;

	uint32 rsv_8	:25;
	uint32 acl_lookup_result_ctl238_to32	:7;

	uint32 acl_lookup_result_ctl231_to0	:32;

	uint32 rsv_9	:25;
	uint32 qos_lookup_result_ctl038_to32	:7;

	uint32 qos_lookup_result_ctl031_to0	:32;

	uint32 rsv_10	:25;
	uint32 qos_lookup_result_ctl138_to32	:7;

	uint32 qos_lookup_result_ctl131_to0	:32;

	uint32 rsv_11	:25;
	uint32 qos_lookup_result_ctl238_to32	:7;

	uint32 qos_lookup_result_ctl231_to0	:32;
};
typedef struct epe_acl_qos_ctl_s epe_acl_qos_ctl_t;

struct epe_acl_qos_count_s	/* 105 */
{

	uint32 rsv_0	:28;
	uint32 pkt_info_recv_cnt	:4;

	uint32 rsv_1	:12;
	uint32 discard_recv_cnt	:4;
	uint32 rsv_2	:12;
	uint32 discard_trans_cnt	:4;

	uint32 rsv_3	:12;
	uint32 tb_info_arb_error_cnt	:4;
	uint32 rsv_4	:12;
	uint32 tcam_error_cnt	:4;

	uint32 rsv_5	:12;
	uint32 tb_info_arb_req_cnt	:4;
	uint32 rsv_6	:12;
	uint32 tb_info_arb_result_cnt	:4;

	uint32 rsv_7	:12;
	uint32 tcam_req_cnt	:4;
	uint32 rsv_8	:12;
	uint32 tcam_result_cnt	:4;
};
typedef struct epe_acl_qos_count_s epe_acl_qos_count_t;

struct epe_acl_qos_debug_s	/* 106 */
{

	uint32 rsv_0	:18;
	uint32 acl_key_gen_track_fifo_afull_threshold	:6;
	uint32 rsv_1	:3;
	uint32 acl_ds_retrieve_track_fifo_afull_threshold	:5;
};
typedef struct epe_acl_qos_debug_s epe_acl_qos_debug_t;

struct epe_acl_qos_lfsr_s	/* 107 */
{

	uint32 rsv_0	:8;
	uint32 random_seed	:24;
};
typedef struct epe_acl_qos_lfsr_s epe_acl_qos_lfsr_t;

struct intr_value_set_s	/* 108 */
{

	uint32 rsv_0	:18;
	uint32 intr_value_set	:14;
};
typedef struct intr_value_set_s intr_value_set_t;

struct intr_value_reset_s	/* 109 */
{

	uint32 rsv_0	:18;
	uint32 intr_value_reset	:14;
};
typedef struct intr_value_reset_s intr_value_reset_t;

struct intr_mask_set_s	/* 110 */
{

	uint32 rsv_0	:18;
	uint32 intr_mask_set	:14;
};
typedef struct intr_mask_set_s intr_mask_set_t;

struct intr_mask_reset_s	/* 111 */
{

	uint32 rsv_0	:18;
	uint32 intr_mask_reset	:14;
};
typedef struct intr_mask_reset_s intr_mask_reset_t;

struct epe_classification_info_credit_value_s	/* 112 */
{

	uint32 rsv_0	:27;
	uint32 info_credit_value	:5;
};
typedef struct epe_classification_info_credit_value_s epe_classification_info_credit_value_t;

struct epe_classification_count0_s	/* 113 */
{

	uint32 rsv_0	:28;
	uint32 pkt_info_recv_cnt	:4;
};
typedef struct epe_classification_count0_s epe_classification_count0_t;

struct epe_classification_count1_s	/* 114 */
{

	uint32 rsv_0	:12;
	uint32 discard_recv_cnt	:4;
	uint32 rsv_1	:12;
	uint32 discard_trans_cnt	:4;
};
typedef struct epe_classification_count1_s epe_classification_count1_t;

struct epe_classification_count2_s	/* 115 */
{

	uint32 rsv_0	:12;
	uint32 policing_req_cnt	:4;
	uint32 rsv_1	:12;
	uint32 policing_result_cnt	:4;
};
typedef struct epe_classification_count2_s epe_classification_count2_t;

struct epe_classification_interrupt_fatal_s	/* 116 */
{

	uint32 rsv_0	:26;
	uint32 intr_mask_set	:6;

	uint32 rsv_1	:26;
	uint32 intr_mask_reset	:6;

	uint32 rsv_2	:26;
	uint32 intr_value_set	:6;

	uint32 rsv_3	:26;
	uint32 intr_value_reset	:6;
};
typedef struct epe_classification_interrupt_fatal_s epe_classification_interrupt_fatal_t;

struct epe_ipg_ctl_s	/* 117 */
{

	uint32 rsv_0	:24;
	uint32 ipg0	:8;

	uint32 rsv_1	:24;
	uint32 ipg1	:8;

	uint32 rsv_2	:24;
	uint32 ipg2	:8;

	uint32 rsv_3	:24;
	uint32 ipg3	:8;
};
typedef struct epe_ipg_ctl_s epe_ipg_ctl_t;

struct epe_classification_debug_s	/* 118 */
{

	uint32 rsv_0	:26;
	uint32 classfication_track_fifoa_full_threshold	:6;
};
typedef struct epe_classification_debug_s epe_classification_debug_t;

struct epe_classification_drain_enable_s	/* 119 */
{

	uint32 rsv_0	:31;
	uint32 epe_classification_drain_enable	:1;
};
typedef struct epe_classification_drain_enable_s epe_classification_drain_enable_t;

struct epe_classification_ctl_s	/* 120 */
{

	uint32 vlan_flow_policer_base	:8;
	uint32 port_policer_base	:8;
	uint32 local_phy_port_mask	:4;
	uint32 rsv_0	:2;
	uint32 vlan_flow_policer_shift	:2;
	uint32 oam_bypass_policing_discard	:1;
	uint32 flow_policer_first	:1;
	uint32 rsv_1	:4;
	uint32 port_policer_shift	:2;

	uint32 rsv_2	:24;
	uint32 service_policer_base	:8;
};
typedef struct epe_classification_ctl_s epe_classification_ctl_t;

struct epe_hdr_adjust_ctl_s	/* 121 */
{

	uint32 rsv_0	:26;
	uint32 vrf_id_en	:1;
	uint32 flow_id_en	:1;
	uint32 cfg_service_id_en	:1;
	uint32 vpls_src_port_en	:1;
	uint32 sgmac_header_en	:1;
	uint32 rsv_1	:1;
};
typedef struct epe_hdr_adjust_ctl_s epe_hdr_adjust_ctl_t;

struct epe_hdr_adjust_sgmac_ctl_s	/* 122 */
{

	uint32 rsv_0	:8;
	uint32 sgmac_local_phy_port	:8;
	uint32 rsv_1	:3;
	uint32 chip_id	:5;
	uint32 rsv_2	:8;

	uint32 to_sgmac_en	:32;
};
typedef struct epe_hdr_adjust_sgmac_ctl_s epe_hdr_adjust_sgmac_ctl_t;

struct epe_hdr_adjust_misc_ctl_s	/* 123 */
{

	uint32 rsv_0	:10;
	uint32 min_pkt_len	:6;
	uint32 rsv_1	:11;
	uint32 buf_retrv_intf_fifoa_full_threshold	:5;
};
typedef struct epe_hdr_adjust_misc_ctl_s epe_hdr_adjust_misc_ctl_t;

struct epe_hdr_adjust_drain_enable_s	/* 124 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct epe_hdr_adjust_drain_enable_s epe_hdr_adjust_drain_enable_t;

struct epe_hdr_adjust_interrupt_s	/* 125 */
{

	uint32 rsv_0	:28;
	uint32 value_set	:4;

	uint32 rsv_1	:28;
	uint32 value_reset	:4;

	uint32 rsv_2	:28;
	uint32 mask_set	:4;

	uint32 rsv_3	:28;
	uint32 mask_reset	:4;
};
typedef struct epe_hdr_adjust_interrupt_s epe_hdr_adjust_interrupt_t;

struct epe_hdr_adjust_stall_info_s	/* 126 */
{

	uint32 rsv_0	:28;
	uint32 stall_info	:4;
};
typedef struct epe_hdr_adjust_stall_info_s epe_hdr_adjust_stall_info_t;

struct epe_hdr_adjust_fsm_state_s	/* 127 */
{

	uint32 rsv_0	:30;
	uint32 fsm_state	:2;
};
typedef struct epe_hdr_adjust_fsm_state_s epe_hdr_adjust_fsm_state_t;

struct epe_hdr_adjust_debug_stats_s	/* 128 */
{

	uint32 rsv_0	:28;
	uint32 pkt_recv_cnt	:4;

	uint32 rsv_1	:28;
	uint32 discard_recv_cnt	:4;

	uint32 rsv_2	:28;
	uint32 discard_trans_cnt	:4;

	uint32 rsv_3	:28;
	uint32 min_pkt_len_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 pkt_crc_error_cnt	:4;
};
typedef struct epe_hdr_adjust_debug_stats_s epe_hdr_adjust_debug_stats_t;

struct epe_hdr_adjust_random_seed_load_s	/* 129 */
{

	uint32 rsv_0	:17;
	uint32 rand_seed_value	:15;

	uint32 rsv_1	:31;
	uint32 rand_seed_load	:1;
};
typedef struct epe_hdr_adjust_random_seed_load_s epe_hdr_adjust_random_seed_load_t;

struct epe_hdr_adjust_parity_fail_record_s	/* 130 */
{

	uint32 rsv_0	:15;
	uint32 ds_dest_phy_port_parity_fail	:1;
	uint32 rsv_1	:8;
	uint32 ds_dest_phy_port_parity_fail_addr	:8;
};
typedef struct epe_hdr_adjust_parity_fail_record_s epe_hdr_adjust_parity_fail_record_t;

struct epe_hdr_adjust_disable_crc_chk_s	/* 131 */
{

	uint32 rsv_0	:31;
	uint32 disable_crc_chk	:1;
};
typedef struct epe_hdr_adjust_disable_crc_chk_s epe_hdr_adjust_disable_crc_chk_t;

struct epe_hdr_edit_ctl_s	/* 132 */
{

	uint32 flow_id_en	:1;
	uint32 sgmac_header_en	:1;
	uint32 protocol_based_stats_en	:1;
	uint32 log_on_discard	:5;
	uint32 log_port_discard	:1;
	uint32 interface_stats_en	:1;
	uint32 phb_stats_en	:1;
	uint32 phb_per_port_stats_en	:1;
	uint32 rsv_0	:1;
	uint32 rx_ether_oam_critical	:1;
	uint32 phb_port_stats_shift	:2;
	uint32 phb_stats_base	:16;

	uint32 rsv_1	:16;
	uint32 error_layer4_offset	:8;
	uint32 rsv_2	:5;
	uint32 vrf_id_en	:1;
	uint32 vpls_src_port_en	:1;
	uint32 service_id_en	:1;

	uint32 oam_discard_bitmap	:32;
};
typedef struct epe_hdr_edit_ctl_s epe_hdr_edit_ctl_t;

struct epe_hdr_edit_interrupt_fatal_s	/* 133 */
{

	uint32 rsv_0	:9;
	uint32 intr_value_set	:23;

	uint32 rsv_1	:9;
	uint32 intr_value_reset	:23;

	uint32 rsv_2	:9;
	uint32 intr_mask_set	:23;

	uint32 rsv_3	:9;
	uint32 intr_mask_reset	:23;
};
typedef struct epe_hdr_edit_interrupt_fatal_s epe_hdr_edit_interrupt_fatal_t;

struct epe_hdr_edit_debug_s	/* 134 */
{

	uint32 rsv_0	:23;
	uint32 pkt_data_fifoa_full_thrd	:9;

	uint32 rsv_1	:24;
	uint32 pkt_ctl_fifoa_full_thrd	:8;

	uint32 rsv_2	:29;
	uint32 l2_edit_fifoa_full_thrd	:3;

	uint32 rsv_3	:23;
	uint32 new_hdr_fifoa_full_thrd	:9;

	uint32 rsv_4	:27;
	uint32 compact_out_fifoa_full_thrd	:5;

	uint32 rsv_5	:30;
	uint32 reserved_credit_cnt	:2;

	uint32 rsv_6	:23;
	uint32 pkt_info_acl_fifoa_full_thrd	:9;
};
typedef struct epe_hdr_edit_debug_s epe_hdr_edit_debug_t;

struct epe_hdr_edit_drain_enable_s	/* 135 */
{

	uint32 rsv_0	:31;
	uint32 epe_hdr_edit_drain_enable	:1;
};
typedef struct epe_hdr_edit_drain_enable_s epe_hdr_edit_drain_enable_t;

struct epe_hdr_edit_stats_s	/* 136 */
{

	uint32 rsv_0	:16;
	uint32 pkt_info_discard_cnt	:16;

	uint32 rsv_1	:16;
	uint32 loopback_cnt	:16;

	uint32 rsv_2	:16;
	uint32 exception_cnt	:16;

	uint32 rsv_3	:16;
	uint32 complete_discard_cnt	:16;

	uint32 rsv_4	:28;
	uint32 epe_trans_pkt_cnt	:4;

	uint32 rsv_5	:28;
	uint32 fr_acl_qos_pkt_info_cnt	:4;

	uint32 rsv_6	:28;
	uint32 fr_cla_pkt_info_cnt	:4;

	uint32 rsv_7	:28;
	uint32 fr_hdr_proc_new_hdr_cnt	:4;
};
typedef struct epe_hdr_edit_stats_s epe_hdr_edit_stats_t;

struct epe_hdr_edit_misc_ctl_s	/* 137 */
{

	uint32 rsv_0	:15;
	uint32 crc_replace_en	:1;
	uint32 rsv_1	:7;
	uint32 discard_type_stats_ra_init_done	:1;
	uint32 rsv_2	:8;
};
typedef struct epe_hdr_edit_misc_ctl_s epe_hdr_edit_misc_ctl_t;

struct epe_hdr_edit_parity_fail_record_s	/* 138 */
{

	uint32 rsv_0	:7;
	uint32 ds_l2_edit_lpbk_mem_parity_fail	:1;
	uint32 ds_l2_edit_lpbk_mem_parity_fail_addr	:8;
	uint32 rsv_1	:7;
	uint32 epe_header_edit_sgmac_priority_map_mem_parity_fail	:1;
	uint32 epe_header_edit_sgmac_priority_map_mem_parity_fail_addr	:8;
};
typedef struct epe_hdr_edit_parity_fail_record_s epe_hdr_edit_parity_fail_record_t;

struct epe_hdr_edit_state_s	/* 139 */
{

	uint32 rsv_0	:29;
	uint32 cur_st	:3;
};
typedef struct epe_hdr_edit_state_s epe_hdr_edit_state_t;

struct epe_l2_ether_type_s	/* 140 */
{

	uint32 rsv_0	:16;
	uint32 epe_l2_ether_type0	:16;

	uint32 rsv_1	:16;
	uint32 epe_l2_ether_type1	:16;

	uint32 rsv_2	:16;
	uint32 epe_l2_ether_type2	:16;

	uint32 rsv_3	:16;
	uint32 epe_l2_ether_type3	:16;

	uint32 rsv_4	:16;
	uint32 epe_l2_ether_type4	:16;

	uint32 rsv_5	:16;
	uint32 epe_l2_ether_type5	:16;

	uint32 rsv_6	:16;
	uint32 epe_l2_ether_type6	:16;

	uint32 rsv_7	:16;
	uint32 epe_l2_ether_type7	:16;
};
typedef struct epe_l2_ether_type_s epe_l2_ether_type_t;

struct epe_l2_router_mac_sa_s	/* 141 */
{

	uint32 rsv_0	:16;
	uint32 epe_l2_router_mac_sa0_upper	:16;

	uint32 epe_l2_router_mac_sa0_lower	:24;
	uint32 rsv_1	:8;

	uint32 rsv_2	:16;
	uint32 epe_l2_router_mac_sa1_upper	:16;

	uint32 epe_l2_router_mac_sa1_lower	:24;
	uint32 rsv_3	:8;

	uint32 rsv_4	:16;
	uint32 epe_l2_router_mac_sa2_upper	:16;

	uint32 epe_l2_router_mac_sa2_lower	:24;
	uint32 rsv_5	:8;
};
typedef struct epe_l2_router_mac_sa_s epe_l2_router_mac_sa_t;

struct epe_l2_snap_ctl_s	/* 142 */
{

	uint32 rsv_0	:8;
	uint32 epe_l2_snap_ctl_oui_value	:24;
};
typedef struct epe_l2_snap_ctl_s epe_l2_snap_ctl_t;

struct epe_l3_ip_identification_s	/* 143 */
{

	uint32 epe_l3_ip_identification0	:16;
	uint32 epe_l3_ip_identification1	:16;
};
typedef struct epe_l3_ip_identification_s epe_l3_ip_identification_t;

struct epe_hdr_proc_debug_s	/* 144 */
{

	uint32 rsv_0	:9;
	uint32 hdr_proc_pkt_info_fifoa_full_thrd	:7;
	uint32 rsv_1	:8;
	uint32 hdr_procpr_fifoa_full_thrd	:8;

	uint32 rsv_2	:9;
	uint32 hdr_proc_ds_edit_intf_fifoa_full_thrd	:7;
	uint32 rsv_3	:3;
	uint32 hdr_proc_vlan_status_intf_fifoa_full_thrd	:5;
	uint32 rsv_4	:3;
	uint32 hdr_proc_stp_state_intf_fifoa_full_thrd	:5;
};
typedef struct epe_hdr_proc_debug_s epe_hdr_proc_debug_t;

struct epe_hdr_proc_cnt_s	/* 145 */
{

	uint32 rsv_0	:12;
	uint32 pkt_info_cnt	:4;
	uint32 rsv_1	:12;
	uint32 parser_result_cnt	:4;

	uint32 rsv_2	:12;
	uint32 ds_l3_cnt	:4;
	uint32 rsv_3	:12;
	uint32 ds_l2_cnt	:4;

	uint32 rsv_4	:12;
	uint32 in_pkt_info_discard_cnt	:4;
	uint32 rsv_5	:12;
	uint32 out_pkt_info_discard_cnt	:4;

	uint32 rsv_6	:12;
	uint32 ds_vlan_status_cnt	:4;
	uint32 rsv_7	:12;
	uint32 ds_stp_state_cnt	:4;

	uint32 rsv_8	:12;
	uint32 ds_l3_edit_err_cnt	:4;
	uint32 rsv_9	:12;
	uint32 ds_l2_edit_err_cnt	:4;

	uint32 rsv_10	:12;
	uint32 ds_vlan_status_err_cnt	:4;
	uint32 rsv_11	:12;
	uint32 ds_stp_state_err_cnt	:4;

	uint32 rsv_12	:12;
	uint32 pkt_info_seq_mismatch_cnt	:4;
	uint32 rsv_13	:12;
	uint32 ds_edit_seq_mismatch_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tb_info_arb_err_cnt	:4;
};
typedef struct epe_hdr_proc_cnt_s epe_hdr_proc_cnt_t;

struct epe_hdr_proc_intr_value_set_s	/* 146 */
{

	uint32 rsv_0	:21;
	uint32 value_set	:11;
};
typedef struct epe_hdr_proc_intr_value_set_s epe_hdr_proc_intr_value_set_t;

struct epe_hdr_proc_intr_value_reset_s	/* 147 */
{

	uint32 rsv_0	:21;
	uint32 value_reset	:11;
};
typedef struct epe_hdr_proc_intr_value_reset_s epe_hdr_proc_intr_value_reset_t;

struct epe_hdr_proc_intr_mask_set_s	/* 148 */
{

	uint32 rsv_0	:21;
	uint32 mask_set	:11;
};
typedef struct epe_hdr_proc_intr_mask_set_s epe_hdr_proc_intr_mask_set_t;

struct epe_hdr_proc_intr_mask_reset_s	/* 149 */
{

	uint32 rsv_0	:21;
	uint32 mask_reset	:11;
};
typedef struct epe_hdr_proc_intr_mask_reset_s epe_hdr_proc_intr_mask_reset_t;

struct epe_hdr_proc_drain_enable_s	/* 150 */
{

	uint32 rsv_0	:31;
	uint32 epe_hdr_proc_drain_enable	:1;
};
typedef struct epe_hdr_proc_drain_enable_s epe_hdr_proc_drain_enable_t;

struct epe_pkt_proc_ctl_s	/* 151 */
{

	uint32 rsv_0	:9;
	uint32 pbb_bv_oam_on_egs_pip_en	:1;
	uint32 pbb_bsi_oam_on_egs_cbp_en	:1;
	uint32 terminate_pbb_bv_oam_on_egs_pip	:1;
	uint32 use_global_ctag_cos	:1;
	uint32 global_ctag_cos	:3;
	uint32 discard_same_ip_addr	:1;
	uint32 discard_same_mac_addr	:1;
	uint32 rsv_1	:1;
	uint32 pt_udp_checksum_en	:1;
	uint32 pt_multicast_address_en	:1;
	uint32 parser_length_error_mode	:2;
	uint32 cbp_tci_res2_check_en	:1;
	uint32 mirror_escape_cam_en	:1;
	uint32 oam_bypass_egress_stp	:1;
	uint32 route_obey_stp	:1;
	uint32 oam_bypass_egress_filter	:1;
	uint32 discard_tunnel_ttl0	:1;
	uint32 discard_mpls_tag_ttl0	:1;
	uint32 discard_mpls_ttl0	:1;
	uint32 discard_route_ttl0	:1;

	uint32 bfd_udp_port1	:16;
	uint32 bfd_udp_port0	:16;

	uint32 rsv_2	:23;
	uint32 bfd_en	:1;
	uint32 cpu_port	:8;
};
typedef struct epe_pkt_proc_ctl_s epe_pkt_proc_ctl_t;

struct epe_hdr_proc_stall_s	/* 152 */
{

	uint32 rsv_0	:28;
	uint32 tcam_arb_stall	:1;
	uint32 hdr_edit_pkt_info_stall	:1;
	uint32 hdr_edit_l3_stall	:1;
	uint32 hdr_edit_l2_stall	:1;
};
typedef struct epe_hdr_proc_stall_s epe_hdr_proc_stall_t;

struct epe_hdr_proc_state_s	/* 153 */
{

	uint32 rsv_0	:25;
	uint32 cur_tb_info_prep_st	:3;
	uint32 rsv_1	:2;
	uint32 cur_pop_st	:2;
};
typedef struct epe_hdr_proc_state_s epe_hdr_proc_state_t;

struct epe_l2_tpid_ctl_s	/* 154 */
{

	uint32 cvlan_tpid	:16;
	uint32 rsv_0	:16;

	uint32 i_tag_tpid	:16;
	uint32 bvlan_tpid	:16;

	uint32 svlan_tpid0	:16;
	uint32 svlan_tpid1	:16;

	uint32 svlan_tpid2	:16;
	uint32 svlan_tpid3	:16;
};
typedef struct epe_l2_tpid_ctl_s epe_l2_tpid_ctl_t;

struct epe_l3_mpls_seq_num_s	/* 155 */
{

	uint32 epe_l3_mpls_seq_num0	:16;
	uint32 epe_l3_mpls_seq_num1	:16;
};
typedef struct epe_l3_mpls_seq_num_s epe_l3_mpls_seq_num_t;

struct epe_pbb_ctl_s	/* 156 */
{

	uint32 pbb_oui_value	:24;
	uint32 pip_mac_sa47_to40	:8;

	uint32 pip_mac_sa39_to8	:32;
};
typedef struct epe_pbb_ctl_s epe_pbb_ctl_t;

struct ds_l3_edit_mpls_seq_num_mem_ctrl_s	/* 157 */
{

	uint32 rsv_0	:31;
	uint32 ds_l3_edit_seq_num_mem_init_done	:1;

	uint32 rsv_1	:31;
	uint32 ds_l3_edit_seq_num_mem_init	:1;
};
typedef struct ds_l3_edit_mpls_seq_num_mem_ctrl_s ds_l3_edit_mpls_seq_num_mem_ctrl_t;

struct ds_l3_edit_tunnel_v6_ip_mem_parity_ctl_s	/* 158 */
{

	uint32 rsv_0	:23;
	uint32 ds_l3_edit_tunnel_v6_ip_mem_parity_fail	:1;
	uint32 ds_l3_edit_tunnel_v6_ip_mem_parity_fail_addr	:8;
};
typedef struct ds_l3_edit_tunnel_v6_ip_mem_parity_ctl_s ds_l3_edit_tunnel_v6_ip_mem_parity_ctl_t;

struct ds_l3_edit_tunnel_v4_ip_sa_parity_ctl_s	/* 159 */
{

	uint32 rsv_0	:23;
	uint32 ds_l3_edit_tunnel_v4_ip_sa_mem_parity_fail	:1;
	uint32 rsv_1	:2;
	uint32 ds_l3_edit_tunnel_v4_ip_sa_mem_parity_fail_addr	:6;
};
typedef struct ds_l3_edit_tunnel_v4_ip_sa_parity_ctl_s ds_l3_edit_tunnel_v4_ip_sa_parity_ctl_t;

struct epe_mirror_escape_cam_s	/* 160 */
{

	uint32 rsv_0	:16;
	uint32 mac_da_value01	:16;

	uint32 mac_da_value00	:32;

	uint32 rsv_1	:16;
	uint32 mac_da_mask01	:16;

	uint32 mac_da_mask00	:32;

	uint32 rsv_2	:16;
	uint32 mac_da_value11	:16;

	uint32 mac_da_value10	:32;

	uint32 rsv_3	:16;
	uint32 mac_da_mask11	:16;

	uint32 mac_da_mask10	:32;
};
typedef struct epe_mirror_escape_cam_s epe_mirror_escape_cam_t;

struct epe_next_hop_config_s	/* 161 */
{

	uint32 rsv_0	:20;
	uint32 ds_aggregate_credit_value	:4;
	uint32 rsv_1	:3;
	uint32 info_credit_value	:5;
};
typedef struct epe_next_hop_config_s epe_next_hop_config_t;

struct epe_next_hop_debug_s	/* 162 */
{

	uint32 rsv_0	:20;
	uint32 ds_vlan_pkt_info_fifoa_full_thrd	:4;
	uint32 rsv_1	:3;
	uint32 ds_next_hop_pkt_info_fifoa_full_thrd	:5;

	uint32 rsv_2	:4;
	uint32 fr_share_ds_aggregate_rd_valid_cnt	:4;
	uint32 fr_share_ds_vlan_rd_valid_cnt	:4;
	uint32 fr_tb_info_arb_next_hop_rd_valid_cnt	:4;
	uint32 rsv_3	:8;
	uint32 to_share_ds_vlan_req_cnt	:4;
	uint32 to_tb_info_arb_next_hop_req_cnt	:4;

	uint32 rsv_4	:24;
	uint32 to_share_ds_vlan_status_req_cnt	:4;
	uint32 to_tb_info_arb_access_edit_cnt	:4;

	uint32 rsv_5	:12;
	uint32 to_hdr_proc_discard_cnt	:4;
	uint32 rsv_6	:12;
	uint32 fr_hdr_adj_discard_cnt	:4;
};
typedef struct epe_next_hop_debug_s epe_next_hop_debug_t;

struct epe_next_hop_ctl_s	/* 163 */
{

	uint32 rsv_0	:2;
	uint32 vlan_ptr_bits_num	:2;
	uint32 oam_bypass_vlan_tx	:1;
	uint32 rsv_1	:1;
	uint32 deny_duplicate_mirror	:1;
	uint32 rsv_2	:1;
	uint32 route_obey_isolate	:1;
	uint32 oam_obey_l2_match	:1;
	uint32 oam_bypass_port_tx	:1;
	uint32 next_hop_src_vlan_en	:1;
	uint32 mirror_obey_discard	:1;
	uint32 discard_vpls_tunnel_match	:1;
	uint32 discard_bridge_l2_match	:1;
	uint32 force_bridge_l3_match	:1;
	uint32 rsv_3	:4;
	uint32 ds_stp_state_shift	:2;
	uint32 rsv_4	:6;
	uint32 edit_ptr_bits_num	:4;

	uint32 rsv_5	:4;
	uint32 ds_next_hop_table_base	:12;
	uint32 rsv_6	:4;
	uint32 ds_vlan_status_table_base	:12;

	uint32 rsv_7	:4;
	uint32 ds_l3_edit_table_base	:12;
	uint32 rsv_8	:4;
	uint32 ds_l2_edit_table_base	:12;

	uint32 rsv_9	:2;
	uint32 ds_next_hop_internal_base	:18;
	uint32 ds_vlan_table_base	:12;
};
typedef struct epe_next_hop_ctl_s epe_next_hop_ctl_t;

struct next_hop_intr_value_set_s	/* 164 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct next_hop_intr_value_set_s next_hop_intr_value_set_t;

struct next_hop_intr_value_reset_s	/* 165 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct next_hop_intr_value_reset_s next_hop_intr_value_reset_t;

struct next_hop_intr_mask_set_s	/* 166 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct next_hop_intr_mask_set_s next_hop_intr_mask_set_t;

struct next_hop_intr_mask_reset_s	/* 167 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct next_hop_intr_mask_reset_s next_hop_intr_mask_reset_t;

struct drain_enable_s	/* 168 */
{

	uint32 rsv_0	:31;
	uint32 epe_next_hop_drain_enable	:1;
};
typedef struct drain_enable_s drain_enable_t;

struct running_credit_s	/* 169 */
{

	uint32 rsv_0	:20;
	uint32 ds_link_aggregate_group_credit	:4;
	uint32 rsv_1	:3;
	uint32 running_credit	:5;
};
typedef struct running_credit_s running_credit_t;

struct parity_check_en_s	/* 170 */
{

	uint32 rsv_0	:31;
	uint32 parity_check_en	:1;
};
typedef struct parity_check_en_s parity_check_en_t;

struct parity_fail_record_s	/* 171 */
{

	uint32 ds_dest_port_mem_parity_fail	:1;
	uint32 rsv_0	:7;
	uint32 ds_dest_port_mem_parity_fail_addr	:8;
	uint32 edit_priority_map_mem_parity_fail	:1;
	uint32 rsv_1	:4;
	uint32 edit_priority_map_mem_parity_fail_addr	:11;

	uint32 rsv_2	:16;
	uint32 ds_dest_interface_mem_parity_fail	:1;
	uint32 rsv_3	:5;
	uint32 ds_dest_interface_mem_parity_fail_addr	:10;

	uint32 rsv_4	:16;
	uint32 ds_vpls_port_mem_parity_fail	:1;
	uint32 rsv_5	:7;
	uint32 ds_vpls_port_mem_parity_fail_addr	:8;
};
typedef struct parity_fail_record_s parity_fail_record_t;

struct epe_stats_ctl_epe_phb_intf_s	/* 172 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_epe_phb_intf	:1;
	uint32 stats_hold_epe_phb_intf	:1;
	uint32 clear_on_read_epe_phb_intf	:1;
	uint32 stats_base_ptr_epe_phb_intf	:16;
};
typedef struct epe_stats_ctl_epe_phb_intf_s epe_stats_ctl_epe_phb_intf_t;

struct epe_stats_init_epe_phb_intf_s	/* 173 */
{

	uint32 rsv_0	:31;
	uint32 init_epe_phb_intf	:1;
};
typedef struct epe_stats_init_epe_phb_intf_s epe_stats_init_epe_phb_intf_t;

struct epe_stats_init_done_epe_phb_intf_s	/* 174 */
{

	uint32 rsv_0	:31;
	uint32 init_done_epe_phb_intf	:1;
};
typedef struct epe_stats_init_done_epe_phb_intf_s epe_stats_init_done_epe_phb_intf_t;

struct epe_stats_ctl_epe_overall_fwd_s	/* 175 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_epe_overall_fwd	:1;
	uint32 stats_hold_epe_overall_fwd	:1;
	uint32 clear_on_read_epe_overall_fwd	:1;
	uint32 stats_base_ptr_epe_overall_fwd	:16;
};
typedef struct epe_stats_ctl_epe_overall_fwd_s epe_stats_ctl_epe_overall_fwd_t;

struct epe_stats_init_epe_overall_fwd_s	/* 176 */
{

	uint32 rsv_0	:31;
	uint32 init_epe_overall_fwd	:1;
};
typedef struct epe_stats_init_epe_overall_fwd_s epe_stats_init_epe_overall_fwd_t;

struct epe_stats_init_done_epe_overall_fwd_s	/* 177 */
{

	uint32 rsv_0	:31;
	uint32 init_done_epe_overall_fwd	:1;
};
typedef struct epe_stats_init_done_epe_overall_fwd_s epe_stats_init_done_epe_overall_fwd_t;

struct epe_stats_intr_value_set_s	/* 178 */
{

	uint32 rsv_0	:14;
	uint32 intr_value_set	:18;
};
typedef struct epe_stats_intr_value_set_s epe_stats_intr_value_set_t;

struct epe_stats_intr_value_reset_s	/* 179 */
{

	uint32 rsv_0	:14;
	uint32 intr_value_reset	:18;
};
typedef struct epe_stats_intr_value_reset_s epe_stats_intr_value_reset_t;

struct epe_stats_intr_mask_set_s	/* 180 */
{

	uint32 rsv_0	:14;
	uint32 intr_mask_set	:18;
};
typedef struct epe_stats_intr_mask_set_s epe_stats_intr_mask_set_t;

struct epe_stats_intr_mask_reset_s	/* 181 */
{

	uint32 rsv_0	:14;
	uint32 intr_mask_reset	:18;
};
typedef struct epe_stats_intr_mask_reset_s epe_stats_intr_mask_reset_t;

struct epe_stats_debug_stats_s	/* 182 */
{

	uint32 rsv_0	:4;
	uint32 port_log_drop_cnt	:4;
	uint32 rsv_1	:4;
	uint32 fwd_drop_cnt	:4;
	uint32 rsv_2	:4;
	uint32 intf_drop_cnt	:4;
	uint32 rsv_3	:4;
	uint32 phb_drop_cnt	:4;
};
typedef struct epe_stats_debug_stats_s epe_stats_debug_stats_t;

struct epe_stats_ctl_epe_port_log_s	/* 183 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_epe_port_log	:1;
	uint32 stats_hold_epe_port_log	:1;
	uint32 clear_on_read_epe_port_log	:1;
	uint32 stats_base_ptr_epe_port_log	:16;
};
typedef struct epe_stats_ctl_epe_port_log_s epe_stats_ctl_epe_port_log_t;

struct epe_stats_init_epe_port_log_s	/* 184 */
{

	uint32 rsv_0	:31;
	uint32 init_epe_port_log	:1;
};
typedef struct epe_stats_init_epe_port_log_s epe_stats_init_epe_port_log_t;

struct epe_stats_init_done_epe_port_log_s	/* 185 */
{

	uint32 rsv_0	:31;
	uint32 init_done_epe_port_log	:1;
};
typedef struct epe_stats_init_done_epe_port_log_s epe_stats_init_done_epe_port_log_t;

struct fabric_cas_intr_ram_s	/* 186 */
{

	uint32 rsv_0	:7;
	uint32 intr_wr_ptr	:9;
	uint32 rsv_1	:7;
	uint32 intr_rd_ptr	:9;
};
typedef struct fabric_cas_intr_ram_s fabric_cas_intr_ram_t;

struct fabric_cas_ctl_s	/* 187 */
{

	uint32 rsv_0	:13;
	uint32 wr_ptr_delta	:3;
	uint32 rsv_1	:2;
	uint32 maximum_slice_num	:10;
	uint32 rsv_2	:2;
	uint32 header_version	:2;
};
typedef struct fabric_cas_ctl_s fabric_cas_ctl_t;

struct fabric_cas_link_stall_select_s	/* 188 */
{

	uint32 rsv_0	:28;
	uint32 link_stall_select	:4;
};
typedef struct fabric_cas_link_stall_select_s fabric_cas_link_stall_select_t;

struct buf_store_stall_s	/* 189 */
{

	uint32 rsv_0	:28;
	uint32 running_buf_store_stall	:4;
};
typedef struct buf_store_stall_s buf_store_stall_t;

struct fabric_cas_cell_ptr_debug_s	/* 190 */
{

	uint32 rsv_0	:29;
	uint32 cell_rd_seq_at_first_wr	:3;
};
typedef struct fabric_cas_cell_ptr_debug_s fabric_cas_cell_ptr_debug_t;

struct fabric_cas_misc_ctl_s	/* 191 */
{

	uint32 rsv_0	:25;
	uint32 voq_data_fifoa_full_thrd	:7;
};
typedef struct fabric_cas_misc_ctl_s fabric_cas_misc_ctl_t;

struct fabric_cas_interrupt_s	/* 192 */
{

	uint32 mask_set0	:32;

	uint32 mask_reset0	:32;

	uint32 value_set0	:32;

	uint32 value_reset0	:32;

	uint32 rsv_0	:28;
	uint32 mask_set1	:4;

	uint32 rsv_1	:28;
	uint32 mask_reset1	:4;

	uint32 rsv_2	:28;
	uint32 value_set1	:4;

	uint32 rsv_3	:28;
	uint32 value_reset1	:4;
};
typedef struct fabric_cas_interrupt_s fabric_cas_interrupt_t;

struct fabric_cas_rxq_stall_s	/* 193 */
{

	uint32 rsv_0	:30;
	uint32 running_rxq_cas_stall_all	:1;
	uint32 running_rxq_cas_stall_lp	:1;
};
typedef struct fabric_cas_rxq_stall_s fabric_cas_rxq_stall_t;

struct fabric_cas_cell_stall_cfg_s	/* 194 */
{

	uint32 rsv_0	:28;
	uint32 pre_cell_stall_cfg	:4;
};
typedef struct fabric_cas_cell_stall_cfg_s fabric_cas_cell_stall_cfg_t;

struct fabric_cas_stall_stats_s	/* 195 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_log	:4;

	uint32 rsv_1	:28;
	uint32 rxq_cas_stall_all_pulse_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rxq_cas_stall_lp_pulse_cnt	:4;
};
typedef struct fabric_cas_stall_stats_s fabric_cas_stall_stats_t;

struct fabric_crb_ctl_s	/* 196 */
{

	uint32 rsv_0	:30;
	uint32 drain_enable	:1;
	uint32 cell_reorder_disable	:1;
};
typedef struct fabric_crb_ctl_s fabric_crb_ctl_t;

struct fabric_crb_input_stats_s	/* 197 */
{

	uint32 rsv_0	:12;
	uint32 input_idle_cell_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_valid_cell_cnt	:4;
};
typedef struct fabric_crb_input_stats_s fabric_crb_input_stats_t;

struct fabric_crb_output_stats_s	/* 198 */
{

	uint32 rsv_0	:12;
	uint32 output_idle_cell_cnt	:4;
	uint32 rsv_1	:12;
	uint32 output_valid_cell_cnt	:4;
};
typedef struct fabric_crb_output_stats_s fabric_crb_output_stats_t;

struct fabric_crb_input_soc_cnt_s	/* 199 */
{

	uint32 rsv_0	:28;
	uint32 input_soc_cnt	:4;
};
typedef struct fabric_crb_input_soc_cnt_s fabric_crb_input_soc_cnt_t;

struct fabric_crb_input_eoc_cnt_s	/* 200 */
{

	uint32 rsv_0	:28;
	uint32 input_eoc_cnt	:4;
};
typedef struct fabric_crb_input_eoc_cnt_s fabric_crb_input_eoc_cnt_t;

struct fabric_crb_output_soc_cnt_s	/* 201 */
{

	uint32 rsv_0	:28;
	uint32 output_soc_cnt	:4;
};
typedef struct fabric_crb_output_soc_cnt_s fabric_crb_output_soc_cnt_t;

struct fabric_crb_output_eoc_cnt_s	/* 202 */
{

	uint32 rsv_0	:28;
	uint32 output_eoc_cnt	:4;
};
typedef struct fabric_crb_output_eoc_cnt_s fabric_crb_output_eoc_cnt_t;

struct fabric_crb_input_slice_cnt_s	/* 203 */
{

	uint32 rsv_0	:28;
	uint32 input_slice_cnt	:4;
};
typedef struct fabric_crb_input_slice_cnt_s fabric_crb_input_slice_cnt_t;

struct fabric_crb_interrupt_s	/* 204 */
{

	uint32 rsv_0	:31;
	uint32 value_set	:1;

	uint32 rsv_1	:31;
	uint32 value_reset	:1;

	uint32 rsv_2	:31;
	uint32 mask_set	:1;

	uint32 rsv_3	:31;
	uint32 mask_reset	:1;
};
typedef struct fabric_crb_interrupt_s fabric_crb_interrupt_t;

struct fabric_crb_output_slice_cnt_s	/* 205 */
{

	uint32 rsv_0	:28;
	uint32 output_slice_cnt	:4;
};
typedef struct fabric_crb_output_slice_cnt_s fabric_crb_output_slice_cnt_t;

struct fabric_crb_state_s	/* 206 */
{

	uint32 rsv_0	:29;
	uint32 cell_gen_state	:3;
};
typedef struct fabric_crb_state_s fabric_crb_state_t;

struct fabric_dsf_link_ctrl_s	/* 207 */
{

	uint32 rsv_0	:16;
	uint32 link_enable	:16;

	uint32 rsv_1	:20;
	uint32 look_for_first_cell_timer_expire_value	:12;

	uint32 rsv_2	:20;
	uint32 link_skew_timer_expire_value	:12;

	uint32 rsv_3	:8;
	uint32 cell_to_cell_timer_start_to_check_value	:8;
	uint32 rsv_4	:8;
	uint32 cell_to_cell_timer_expire_value	:8;
};
typedef struct fabric_dsf_link_ctrl_s fabric_dsf_link_ctrl_t;

struct fabric_dsf_misc_ctrl_s	/* 208 */
{

	uint32 rsv_0	:22;
	uint32 maximum_slice_num	:10;

	uint32 rsv_1	:3;
	uint32 cell_stall_chip_id_at_loopback_mode	:5;
	uint32 rsv_2	:3;
	uint32 src_chip_id_at_loopback_mode	:5;
	uint32 rsv_3	:7;
	uint32 loopback_test_mode	:1;
	uint32 rsv_4	:7;
	uint32 loopback_mode	:1;

	uint32 rsv_5	:31;
	uint32 parity_enable	:1;
};
typedef struct fabric_dsf_misc_ctrl_s fabric_dsf_misc_ctrl_t;

struct fabric_dsf_interrupt_s	/* 209 */
{

	uint32 mask_set0	:32;

	uint32 mask_reset0	:32;

	uint32 value_set0	:32;

	uint32 value_reset0	:32;

	uint32 mask_set1	:32;

	uint32 mask_reset1	:32;

	uint32 value_set1	:32;

	uint32 value_reset1	:32;

	uint32 rsv_0	:11;
	uint32 mask_set2	:21;

	uint32 rsv_1	:11;
	uint32 mask_reset2	:21;

	uint32 rsv_2	:11;
	uint32 value_set2	:21;

	uint32 rsv_3	:11;
	uint32 value_reset2	:21;
};
typedef struct fabric_dsf_interrupt_s fabric_dsf_interrupt_t;

struct fabric_dsf_cell_stats_s	/* 210 */
{

	uint32 rsv_0	:28;
	uint32 fr_fabric_lrx_cell_count_link0	:4;

	uint32 rsv_1	:28;
	uint32 fr_fabric_lrx_cell_count_link1	:4;

	uint32 rsv_2	:28;
	uint32 fr_fabric_lrx_cell_count_link2	:4;

	uint32 rsv_3	:28;
	uint32 fr_fabric_lrx_cell_count_link3	:4;

	uint32 rsv_4	:28;
	uint32 fr_fabric_lrx_cell_count_link4	:4;

	uint32 rsv_5	:28;
	uint32 fr_fabric_lrx_cell_count_link5	:4;

	uint32 rsv_6	:28;
	uint32 fr_fabric_lrx_cell_count_link6	:4;

	uint32 rsv_7	:28;
	uint32 fr_fabric_lrx_cell_count_link7	:4;

	uint32 rsv_8	:28;
	uint32 fr_fabric_lrx_cell_count_link8	:4;

	uint32 rsv_9	:28;
	uint32 fr_fabric_lrx_cell_count_link9	:4;

	uint32 rsv_10	:28;
	uint32 fr_fabric_lrx_cell_count_link10	:4;

	uint32 rsv_11	:28;
	uint32 fr_fabric_lrx_cell_count_link11	:4;

	uint32 rsv_12	:28;
	uint32 fr_fabric_lrx_cell_count_link12	:4;

	uint32 rsv_13	:28;
	uint32 fr_fabric_lrx_cell_count_link13	:4;

	uint32 rsv_14	:28;
	uint32 fr_fabric_lrx_cell_count_link14	:4;

	uint32 rsv_15	:28;
	uint32 fr_fabric_lrx_cell_count_link15	:4;

	uint32 rsv_16	:28;
	uint32 to_fabric_crb_cell_count_link0	:4;

	uint32 rsv_17	:28;
	uint32 to_fabric_crb_cell_count_link1	:4;

	uint32 rsv_18	:28;
	uint32 to_fabric_crb_cell_count_link2	:4;

	uint32 rsv_19	:28;
	uint32 to_fabric_crb_cell_count_link3	:4;

	uint32 rsv_20	:28;
	uint32 to_fabric_crb_cell_count_link4	:4;

	uint32 rsv_21	:28;
	uint32 to_fabric_crb_cell_count_link5	:4;

	uint32 rsv_22	:28;
	uint32 to_fabric_crb_cell_count_link6	:4;

	uint32 rsv_23	:28;
	uint32 to_fabric_crb_cell_count_link7	:4;

	uint32 rsv_24	:28;
	uint32 to_fabric_crb_cell_count_link8	:4;

	uint32 rsv_25	:28;
	uint32 to_fabric_crb_cell_count_link9	:4;

	uint32 rsv_26	:28;
	uint32 to_fabric_crb_cell_count_link10	:4;

	uint32 rsv_27	:28;
	uint32 to_fabric_crb_cell_count_link11	:4;

	uint32 rsv_28	:28;
	uint32 to_fabric_crb_cell_count_link12	:4;

	uint32 rsv_29	:28;
	uint32 to_fabric_crb_cell_count_link13	:4;

	uint32 rsv_30	:28;
	uint32 to_fabric_crb_cell_count_link14	:4;

	uint32 rsv_31	:28;
	uint32 to_fabric_crb_cell_count_link15	:4;

	uint32 rsv_32	:28;
	uint32 fr_fabric_lrx_cell_error_count_link0	:4;

	uint32 rsv_33	:28;
	uint32 fr_fabric_lrx_cell_error_count_link1	:4;

	uint32 rsv_34	:28;
	uint32 fr_fabric_lrx_cell_error_count_link2	:4;

	uint32 rsv_35	:28;
	uint32 fr_fabric_lrx_cell_error_count_link3	:4;

	uint32 rsv_36	:28;
	uint32 fr_fabric_lrx_cell_error_count_link4	:4;

	uint32 rsv_37	:28;
	uint32 fr_fabric_lrx_cell_error_count_link5	:4;

	uint32 rsv_38	:28;
	uint32 fr_fabric_lrx_cell_error_count_link6	:4;

	uint32 rsv_39	:28;
	uint32 fr_fabric_lrx_cell_error_count_link7	:4;

	uint32 rsv_40	:28;
	uint32 fr_fabric_lrx_cell_error_count_link8	:4;

	uint32 rsv_41	:28;
	uint32 fr_fabric_lrx_cell_error_count_link9	:4;

	uint32 rsv_42	:28;
	uint32 fr_fabric_lrx_cell_error_count_link10	:4;

	uint32 rsv_43	:28;
	uint32 fr_fabric_lrx_cell_error_count_link11	:4;

	uint32 rsv_44	:28;
	uint32 fr_fabric_lrx_cell_error_count_link12	:4;

	uint32 rsv_45	:28;
	uint32 fr_fabric_lrx_cell_error_count_link13	:4;

	uint32 rsv_46	:28;
	uint32 fr_fabric_lrx_cell_error_count_link14	:4;

	uint32 rsv_47	:28;
	uint32 fr_fabric_lrx_cell_error_count_link15	:4;
};
typedef struct fabric_dsf_cell_stats_s fabric_dsf_cell_stats_t;

struct fabric_dsf_state_s	/* 211 */
{

	uint32 rsv_0	:24;
	uint32 rcv_fifo_fsm_ps0	:8;

	uint32 rsv_1	:24;
	uint32 rcv_fifo_fsm_ps1	:8;

	uint32 rsv_2	:24;
	uint32 rcv_fifo_fsm_ps2	:8;

	uint32 rsv_3	:24;
	uint32 rcv_fifo_fsm_ps3	:8;

	uint32 rsv_4	:24;
	uint32 rcv_fifo_fsm_ps4	:8;

	uint32 rsv_5	:24;
	uint32 rcv_fifo_fsm_ps5	:8;

	uint32 rsv_6	:24;
	uint32 rcv_fifo_fsm_ps6	:8;

	uint32 rsv_7	:24;
	uint32 rcv_fifo_fsm_ps7	:8;

	uint32 rsv_8	:24;
	uint32 rcv_fifo_fsm_ps8	:8;

	uint32 rsv_9	:24;
	uint32 rcv_fifo_fsm_ps9	:8;

	uint32 rsv_10	:24;
	uint32 rcv_fifo_fsm_ps10	:8;

	uint32 rsv_11	:24;
	uint32 rcv_fifo_fsm_ps11	:8;

	uint32 rsv_12	:24;
	uint32 rcv_fifo_fsm_ps12	:8;

	uint32 rsv_13	:24;
	uint32 rcv_fifo_fsm_ps13	:8;

	uint32 rsv_14	:24;
	uint32 rcv_fifo_fsm_ps14	:8;

	uint32 rsv_15	:24;
	uint32 rcv_fifo_fsm_ps15	:8;

	uint32 rsv_16	:26;
	uint32 rd_ctrl_fsm_ps	:6;
};
typedef struct fabric_dsf_state_s fabric_dsf_state_t;

struct fabric_dsf_src_link_num_chg_enable_s	/* 212 */
{

	uint32 rsv_0	:31;
	uint32 src_link_num_chg_enable	:1;
};
typedef struct fabric_dsf_src_link_num_chg_enable_s fabric_dsf_src_link_num_chg_enable_t;

struct fabric_gts_intr_ram_s	/* 213 */
{

	uint32 rsv_0	:5;
	uint32 intr_cas_ptr	:11;
	uint32 rsv_1	:5;
	uint32 intr_expired_ptr	:11;

	uint32 rsv_2	:21;
	uint32 intr_dsf_ptr	:11;
};
typedef struct fabric_gts_intr_ram_s fabric_gts_intr_ram_t;

struct fabric_gts_ctl_s	/* 214 */
{

	uint32 rsv_0	:9;
	uint32 time_out_slice_num	:7;
	uint32 rsv_1	:2;
	uint32 maximum_slice_num	:10;
	uint32 rsv_2	:3;
	uint32 loopback_mode	:1;
};
typedef struct fabric_gts_ctl_s fabric_gts_ctl_t;

struct fabric_gts_init_s	/* 215 */
{

	uint32 rsv_0	:31;
	uint32 init	:1;
};
typedef struct fabric_gts_init_s fabric_gts_init_t;

struct fabric_gts_init_done_s	/* 216 */
{

	uint32 rsv_0	:31;
	uint32 init	:1;
};
typedef struct fabric_gts_init_done_s fabric_gts_init_done_t;

struct fabric_gts_intr_value_set_s	/* 217 */
{

	uint32 rsv_0	:21;
	uint32 intr_value_set	:11;
};
typedef struct fabric_gts_intr_value_set_s fabric_gts_intr_value_set_t;

struct fabric_gts_intr_value_reset_s	/* 218 */
{

	uint32 rsv_0	:21;
	uint32 intr_value_reset	:11;
};
typedef struct fabric_gts_intr_value_reset_s fabric_gts_intr_value_reset_t;

struct fabric_gts_intr_mask_set_s	/* 219 */
{

	uint32 rsv_0	:21;
	uint32 intr_mask_set	:11;
};
typedef struct fabric_gts_intr_mask_set_s fabric_gts_intr_mask_set_t;

struct fabric_gts_intr_mask_reset_s	/* 220 */
{

	uint32 rsv_0	:21;
	uint32 intr_mask_reset	:11;
};
typedef struct fabric_gts_intr_mask_reset_s fabric_gts_intr_mask_reset_t;

struct fabric_gts_intr_status_s	/* 221 */
{

	uint32 rsv_0	:26;
	uint32 intr_status	:6;
};
typedef struct fabric_gts_intr_status_s fabric_gts_intr_status_t;

struct fabric_gts_time_out_flag_s	/* 222 */
{

	uint32 rsv_0	:28;
	uint32 priority_time_out_flag	:4;
};
typedef struct fabric_gts_time_out_flag_s fabric_gts_time_out_flag_t;

struct fabric_gts_stats_s	/* 223 */
{

	uint32 rsv_0	:28;
	uint32 to_voq_grant_cnt	:4;
};
typedef struct fabric_gts_stats_s fabric_gts_stats_t;

struct fabric_rts_chan_ctrl_s	/* 224 */
{

	uint32 channel_enable0	:32;

	uint32 channel_enable1	:32;

	uint32 channel_enable2	:32;

	uint32 channel_enable3	:32;
};
typedef struct fabric_rts_chan_ctrl_s fabric_rts_chan_ctrl_t;

struct fabric_rts_weight_ctrl_s	/* 225 */
{

	uint32 rsv_0	:22;
	uint32 rts_wsp_cfg0	:10;

	uint32 rsv_1	:22;
	uint32 rts_wsp_cfg1	:10;

	uint32 rsv_2	:22;
	uint32 rts_wsp_cfg2	:10;

	uint32 rsv_3	:22;
	uint32 rts_wsp_cfg3	:10;
};
typedef struct fabric_rts_weight_ctrl_s fabric_rts_weight_ctrl_t;

struct fabric_rts_both_rts_valid_s	/* 226 */
{

	uint32 rsv_0	:31;
	uint32 rts_cas_both_rts_valid	:1;
};
typedef struct fabric_rts_both_rts_valid_s fabric_rts_both_rts_valid_t;

struct fabric_rts_loopback_mode_s	/* 227 */
{

	uint32 rsv_0	:11;
	uint32 loopback_cell_stall_chip_id	:5;
	uint32 rsv_1	:15;
	uint32 loopback_mode	:1;
};
typedef struct fabric_rts_loopback_mode_s fabric_rts_loopback_mode_t;

struct fabric_rts_random_seed_s	/* 228 */
{

	uint32 rsv_0	:8;
	uint32 random_seed	:24;
};
typedef struct fabric_rts_random_seed_s fabric_rts_random_seed_t;

struct fabric_rts_cfg_wsp_type_s	/* 229 */
{

	uint32 rsv_0	:28;
	uint32 cfg_wsp_type	:4;
};
typedef struct fabric_rts_cfg_wsp_type_s fabric_rts_cfg_wsp_type_t;

struct fabric_rts_interrupt_fatal_s	/* 230 */
{

	uint32 rsv_0	:28;
	uint32 mask_set	:4;

	uint32 rsv_1	:28;
	uint32 mask_reset	:4;

	uint32 rsv_2	:28;
	uint32 value_set	:4;

	uint32 rsv_3	:28;
	uint32 value_reset	:4;
};
typedef struct fabric_rts_interrupt_fatal_s fabric_rts_interrupt_fatal_t;

struct fabric_rts_drain_enable_s	/* 231 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct fabric_rts_drain_enable_s fabric_rts_drain_enable_t;

struct fabric_rxq_chan_ctrl_s	/* 232 */
{

	uint32 chan_enable0	:32;

	uint32 chan_enable1	:32;

	uint32 chan_enable2	:32;

	uint32 chan_enable3	:32;
};
typedef struct fabric_rxq_chan_ctrl_s fabric_rxq_chan_ctrl_t;

struct fabric_rxq_cell_stats_s	/* 233 */
{

	uint32 rsv_0	:12;
	uint32 in_cell_count_priority1	:4;
	uint32 rsv_1	:12;
	uint32 in_cell_count_priority0	:4;

	uint32 rsv_2	:12;
	uint32 in_cell_count_priority3	:4;
	uint32 rsv_3	:12;
	uint32 in_cell_count_priority2	:4;

	uint32 rsv_4	:28;
	uint32 in_cell_count_total_good	:4;

	uint32 rsv_5	:12;
	uint32 in_cell_count_total_drop	:4;
	uint32 rsv_6	:12;
	uint32 in_cell_count_total_idle	:4;

	uint32 rsv_7	:12;
	uint32 out_cell_count_priority1	:4;
	uint32 rsv_8	:12;
	uint32 out_cell_count_priority0	:4;

	uint32 rsv_9	:12;
	uint32 out_cell_count_priority3	:4;
	uint32 rsv_10	:12;
	uint32 out_cell_count_priority2	:4;

	uint32 rsv_11	:28;
	uint32 out_cell_count_total	:4;
};
typedef struct fabric_rxq_cell_stats_s fabric_rxq_cell_stats_t;

struct fabric_rxq_pkt_stats_s	/* 234 */
{

	uint32 rsv_0	:12;
	uint32 out_sop_count_priority1	:4;
	uint32 rsv_1	:12;
	uint32 out_sop_count_priority0	:4;

	uint32 rsv_2	:12;
	uint32 out_sop_count_priority3	:4;
	uint32 rsv_3	:12;
	uint32 out_sop_count_priority2	:4;

	uint32 rsv_4	:28;
	uint32 out_sop_count_total	:4;

	uint32 rsv_5	:12;
	uint32 out_eop_count_priority1	:4;
	uint32 rsv_6	:12;
	uint32 out_eop_count_priority0	:4;

	uint32 rsv_7	:12;
	uint32 out_eop_count_priority3	:4;
	uint32 rsv_8	:12;
	uint32 out_eop_count_priority2	:4;

	uint32 rsv_9	:28;
	uint32 out_eop_count_total	:4;
};
typedef struct fabric_rxq_pkt_stats_s fabric_rxq_pkt_stats_t;

struct fabric_rxq_interrupt_fatal_s	/* 235 */
{

	uint32 rsv_0	:24;
	uint32 mask_set	:8;

	uint32 rsv_1	:24;
	uint32 mask_reset	:8;

	uint32 rsv_2	:24;
	uint32 value_set	:8;

	uint32 rsv_3	:24;
	uint32 value_reset	:8;
};
typedef struct fabric_rxq_interrupt_fatal_s fabric_rxq_interrupt_fatal_t;

struct fabric_rxq_misc_ctrl_s	/* 236 */
{

	uint32 rsv_0	:31;
	uint32 seq_check_disable	:1;

	uint32 rsv_1	:31;
	uint32 garb_clean_disable	:1;

	uint32 rsv_2	:28;
	uint32 buf_store_credit_value	:4;

	uint32 rsv_3	:6;
	uint32 pkt_data_fifoa_full_thrd_lp	:10;
	uint32 rsv_4	:6;
	uint32 pkt_data_fifoa_full_thrd_all	:10;

	uint32 rsv_5	:22;
	uint32 pkt_data_fifo_full_threshold	:10;

	uint32 rsv_6	:31;
	uint32 drain_enable	:1;
};
typedef struct fabric_rxq_misc_ctrl_s fabric_rxq_misc_ctrl_t;

struct fabric_rxq_wr_ctrl_res_word_mem_ctrl_s	/* 237 */
{

	uint32 rsv_0	:23;
	uint32 wr_ctrl_res_word_mem_init_done	:1;
	uint32 rsv_1	:7;
	uint32 wr_ctrl_res_word_mem_init	:1;
};
typedef struct fabric_rxq_wr_ctrl_res_word_mem_ctrl_s fabric_rxq_wr_ctrl_res_word_mem_ctrl_t;

struct fabric_rxq_parity_fail_record_s	/* 238 */
{

	uint32 rd_ctrl_res_word_mem_parity_fail	:1;
	uint32 rsv_0	:8;
	uint32 rd_ctrl_res_word_mem_parity_fail_addr	:7;
	uint32 rsv_1	:7;
	uint32 wr_ctrl_res_word_mem_parity_fail	:1;
	uint32 rsv_2	:1;
	uint32 wr_ctrl_res_word_mem_parity_fail_addr	:7;
};
typedef struct fabric_rxq_parity_fail_record_s fabric_rxq_parity_fail_record_t;

struct fabric_ser_ltx_link_enable_s	/* 239 */
{

	uint32 rsv_0	:16;
	uint32 fabric_ltx_enable	:16;
};
typedef struct fabric_ser_ltx_link_enable_s fabric_ser_ltx_link_enable_t;

struct fabric_ser_ltx_prbs_enable_s	/* 240 */
{

	uint32 rsv_0	:16;
	uint32 fabric_ltx_prbs_enable	:16;
};
typedef struct fabric_ser_ltx_prbs_enable_s fabric_ser_ltx_prbs_enable_t;

struct fabric_ser_ltx_prbs_rst_s	/* 241 */
{

	uint32 rsv_0	:16;
	uint32 fabric_ltx_prbs_rst	:16;
};
typedef struct fabric_ser_ltx_prbs_rst_s fabric_ser_ltx_prbs_rst_t;

struct fabric_ser_ltx_frame_sync_symbol_s	/* 242 */
{

	uint32 rsv_0	:14;
	uint32 frame_sync_symbol	:18;
};
typedef struct fabric_ser_ltx_frame_sync_symbol_s fabric_ser_ltx_frame_sync_symbol_t;

struct fabric_ser_ltx_idle_symbol1_s	/* 243 */
{

	uint32 rsv_0	:14;
	uint32 fabric_idle_symbol1	:18;
};
typedef struct fabric_ser_ltx_idle_symbol1_s fabric_ser_ltx_idle_symbol1_t;

struct fabric_ser_ltx_fifo_threshold_s	/* 244 */
{

	uint32 rsv_0	:27;
	uint32 tx_fifoa_full_threshold	:5;
};
typedef struct fabric_ser_ltx_fifo_threshold_s fabric_ser_ltx_fifo_threshold_t;

struct fabric_ser_lrx_link_enable_s	/* 245 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_enable	:16;
};
typedef struct fabric_ser_lrx_link_enable_s fabric_ser_lrx_link_enable_t;

struct fabric_ser_lrx_loopback_s	/* 246 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_loopback	:16;
};
typedef struct fabric_ser_lrx_loopback_s fabric_ser_lrx_loopback_t;

struct fabric_ser_lrx_crc_check_disable_s	/* 247 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_crc_check_disable	:16;
};
typedef struct fabric_ser_lrx_crc_check_disable_s fabric_ser_lrx_crc_check_disable_t;

struct fabric_ser_lrx_reset_prbs_s	/* 248 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_reset_prbs	:16;
};
typedef struct fabric_ser_lrx_reset_prbs_s fabric_ser_lrx_reset_prbs_t;

struct fabric_ser_lrx_bit_order_invert_s	/* 249 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_bit_order_invert	:16;
};
typedef struct fabric_ser_lrx_bit_order_invert_s fabric_ser_lrx_bit_order_invert_t;

struct fabric_ser_lrx_force_sync_s	/* 250 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_force_sync	:16;
};
typedef struct fabric_ser_lrx_force_sync_s fabric_ser_lrx_force_sync_t;

struct fabric_ser_lrx_bit_polarity_invert_s	/* 251 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_bit_polarity_invert	:16;
};
typedef struct fabric_ser_lrx_bit_polarity_invert_s fabric_ser_lrx_bit_polarity_invert_t;

struct fabric_ser_lrx_frame_boundary_idle_cnt_s	/* 252 */
{

	uint32 rsv_0	:22;
	uint32 frame_boundary_idle_cnt	:10;
};
typedef struct fabric_ser_lrx_frame_boundary_idle_cnt_s fabric_ser_lrx_frame_boundary_idle_cnt_t;

struct fabric_ser_lrx_prbs_enable_s	/* 253 */
{

	uint32 rsv_0	:16;
	uint32 fabric_lrx_prbs_enable	:16;
};
typedef struct fabric_ser_lrx_prbs_enable_s fabric_ser_lrx_prbs_enable_t;

struct fabric_ser_lrx_prbs_err_count_s	/* 254 */
{

	uint32 rsv_0	:8;
	uint32 prbs_err_count1	:8;
	uint32 rsv_1	:8;
	uint32 prbs_err_count0	:8;

	uint32 rsv_2	:8;
	uint32 prbs_err_count3	:8;
	uint32 rsv_3	:8;
	uint32 prbs_err_count2	:8;

	uint32 rsv_4	:8;
	uint32 prbs_err_count5	:8;
	uint32 rsv_5	:8;
	uint32 prbs_err_count4	:8;

	uint32 rsv_6	:8;
	uint32 prbs_err_count7	:8;
	uint32 rsv_7	:8;
	uint32 prbs_err_count6	:8;

	uint32 rsv_8	:8;
	uint32 prbs_err_count9	:8;
	uint32 rsv_9	:8;
	uint32 prbs_err_count8	:8;

	uint32 rsv_10	:8;
	uint32 prbs_err_count11	:8;
	uint32 rsv_11	:8;
	uint32 prbs_err_count10	:8;

	uint32 rsv_12	:8;
	uint32 prbs_err_count13	:8;
	uint32 rsv_13	:8;
	uint32 prbs_err_count12	:8;

	uint32 rsv_14	:8;
	uint32 prbs_err_count15	:8;
	uint32 rsv_15	:8;
	uint32 prbs_err_count14	:8;
};
typedef struct fabric_ser_lrx_prbs_err_count_s fabric_ser_lrx_prbs_err_count_t;

struct fabric_ser_syn_sync_pulse_gen_timer_s	/* 255 */
{

	uint32 rsv_0	:24;
	uint32 cfg_sync_pulse_gen_timer	:8;
};
typedef struct fabric_ser_syn_sync_pulse_gen_timer_s fabric_ser_syn_sync_pulse_gen_timer_t;

struct fabric_ser_syn_sync_mask_timer_s	/* 256 */
{

	uint32 rsv_0	:16;
	uint32 cfg_sync_mask_timer	:16;
};
typedef struct fabric_ser_syn_sync_mask_timer_s fabric_ser_syn_sync_mask_timer_t;

struct fabric_ser_syn_maximum_slice_num_s	/* 257 */
{

	uint32 rsv_0	:22;
	uint32 maximum_slice_num	:10;
};
typedef struct fabric_ser_syn_maximum_slice_num_s fabric_ser_syn_maximum_slice_num_t;

struct fabric_ser_syn_cell_slot_cycle_count_s	/* 258 */
{

	uint32 rsv_0	:24;
	uint32 cfg_cell_slot_cycle_count	:8;
};
typedef struct fabric_ser_syn_cell_slot_cycle_count_s fabric_ser_syn_cell_slot_cycle_count_t;

struct fabric_ser_syn_cpu_auto_sync_pulse_timer_s	/* 259 */
{

	uint32 rsv_0	:8;
	uint32 cfg_cpu_auto_sync_pulse_timer	:24;
};
typedef struct fabric_ser_syn_cpu_auto_sync_pulse_timer_s fabric_ser_syn_cpu_auto_sync_pulse_timer_t;

struct fabric_ser_syn_cpu_sync_pulse_enable_s	/* 260 */
{

	uint32 rsv_0	:31;
	uint32 cfg_cpu_sync_pulse_enable	:1;
};
typedef struct fabric_ser_syn_cpu_sync_pulse_enable_s fabric_ser_syn_cpu_sync_pulse_enable_t;

struct fabric_ser_syn_cpu_sync_pulse_step_s	/* 261 */
{

	uint32 rsv_0	:31;
	uint32 cfg_cpu_sync_pulse_step	:1;
};
typedef struct fabric_ser_syn_cpu_sync_pulse_step_s fabric_ser_syn_cpu_sync_pulse_step_t;

struct fabric_ser_syn_master_mode_s	/* 262 */
{

	uint32 rsv_0	:31;
	uint32 cfg_sync_master_mode	:1;
};
typedef struct fabric_ser_syn_master_mode_s fabric_ser_syn_master_mode_t;

struct fabric_ser_syn_force_first_sync_s	/* 263 */
{

	uint32 rsv_0	:7;
	uint32 cfg_force_first_sync_enable	:1;
	uint32 rsv_1	:8;
	uint32 cfg_force_first_sync_timer	:16;
};
typedef struct fabric_ser_syn_force_first_sync_s fabric_ser_syn_force_first_sync_t;

struct fabric_ser_force_signal_detect_s	/* 264 */
{

	uint32 rsv_0	:16;
	uint32 force_signal_detect	:16;
};
typedef struct fabric_ser_force_signal_detect_s fabric_ser_force_signal_detect_t;

struct fabric_ser_soft_reset_ltx_s	/* 265 */
{

	uint32 rsv_0	:16;
	uint32 soft_reset_ltx	:16;
};
typedef struct fabric_ser_soft_reset_ltx_s fabric_ser_soft_reset_ltx_t;

struct fabric_ser_soft_reset_lrx_s	/* 266 */
{

	uint32 rsv_0	:16;
	uint32 soft_reset_lrx	:16;
};
typedef struct fabric_ser_soft_reset_lrx_s fabric_ser_soft_reset_lrx_t;

struct fabric_ser_soft_reset_syn_s	/* 267 */
{

	uint32 rsv_0	:31;
	uint32 soft_reset_syn	:1;
};
typedef struct fabric_ser_soft_reset_syn_s fabric_ser_soft_reset_syn_t;

struct fabric_ser_ltx_pop_threshold_s	/* 268 */
{

	uint32 rsv_0	:27;
	uint32 fabric_ltx_pop_threshold	:5;
};
typedef struct fabric_ser_ltx_pop_threshold_s fabric_ser_ltx_pop_threshold_t;

struct fabric_ser_ltx_idle_symbol2_s	/* 269 */
{

	uint32 rsv_0	:14;
	uint32 fabric_idle_symbol2	:18;
};
typedef struct fabric_ser_ltx_idle_symbol2_s fabric_ser_ltx_idle_symbol2_t;

struct fabric_ser_ltx_use_prbs7_s	/* 270 */
{

	uint32 rsv_0	:31;
	uint32 fabric_ltx_use_prbs7	:1;
};
typedef struct fabric_ser_ltx_use_prbs7_s fabric_ser_ltx_use_prbs7_t;

struct fabric_ser_ltx_test_pattern_s	/* 271 */
{

	uint32 rsv_0	:30;
	uint32 fabric_ltx_test_pattern	:2;
};
typedef struct fabric_ser_ltx_test_pattern_s fabric_ser_ltx_test_pattern_t;

struct fabric_ser_lrx_cell_boundary_idle_cnt_s	/* 272 */
{

	uint32 rsv_0	:24;
	uint32 cell_boundary_idle_cnt	:8;
};
typedef struct fabric_ser_lrx_cell_boundary_idle_cnt_s fabric_ser_lrx_cell_boundary_idle_cnt_t;

struct fabric_ser_lrx_sig_det_active_value_s	/* 273 */
{

	uint32 rsv_0	:31;
	uint32 fabric_lrx_sig_det_active_value	:1;
};
typedef struct fabric_ser_lrx_sig_det_active_value_s fabric_ser_lrx_sig_det_active_value_t;

struct fabric_ser_ltx_fsm_state_s	/* 274 */
{

	uint32 rsv_0	:1;
	uint32 ltx_fsm_state7	:3;
	uint32 rsv_1	:1;
	uint32 ltx_fsm_state6	:3;
	uint32 rsv_2	:1;
	uint32 ltx_fsm_state5	:3;
	uint32 rsv_3	:1;
	uint32 ltx_fsm_state4	:3;
	uint32 rsv_4	:1;
	uint32 ltx_fsm_state3	:3;
	uint32 rsv_5	:1;
	uint32 ltx_fsm_state2	:3;
	uint32 rsv_6	:1;
	uint32 ltx_fsm_state1	:3;
	uint32 rsv_7	:1;
	uint32 ltx_fsm_state0	:3;

	uint32 rsv_8	:1;
	uint32 ltx_fsm_state15	:3;
	uint32 rsv_9	:1;
	uint32 ltx_fsm_state14	:3;
	uint32 rsv_10	:1;
	uint32 ltx_fsm_state13	:3;
	uint32 rsv_11	:1;
	uint32 ltx_fsm_state12	:3;
	uint32 rsv_12	:1;
	uint32 ltx_fsm_state11	:3;
	uint32 rsv_13	:1;
	uint32 ltx_fsm_state10	:3;
	uint32 rsv_14	:1;
	uint32 ltx_fsm_state9	:3;
	uint32 rsv_15	:1;
	uint32 ltx_fsm_state8	:3;
};
typedef struct fabric_ser_ltx_fsm_state_s fabric_ser_ltx_fsm_state_t;

struct fabric_ser_lrx_fsm_state_s	/* 275 */
{

	uint32 rsv_0	:1;
	uint32 lrx_fsm_state7	:3;
	uint32 rsv_1	:1;
	uint32 lrx_fsm_state6	:3;
	uint32 rsv_2	:1;
	uint32 lrx_fsm_state5	:3;
	uint32 rsv_3	:1;
	uint32 lrx_fsm_state4	:3;
	uint32 rsv_4	:1;
	uint32 lrx_fsm_state3	:3;
	uint32 rsv_5	:1;
	uint32 lrx_fsm_state2	:3;
	uint32 rsv_6	:1;
	uint32 lrx_fsm_state1	:3;
	uint32 rsv_7	:1;
	uint32 lrx_fsm_state0	:3;

	uint32 rsv_8	:1;
	uint32 lrx_fsm_state15	:3;
	uint32 rsv_9	:1;
	uint32 lrx_fsm_state14	:3;
	uint32 rsv_10	:1;
	uint32 lrx_fsm_state13	:3;
	uint32 rsv_11	:1;
	uint32 lrx_fsm_state12	:3;
	uint32 rsv_12	:1;
	uint32 lrx_fsm_state11	:3;
	uint32 rsv_13	:1;
	uint32 lrx_fsm_state10	:3;
	uint32 rsv_14	:1;
	uint32 lrx_fsm_state9	:3;
	uint32 rsv_15	:1;
	uint32 lrx_fsm_state8	:3;
};
typedef struct fabric_ser_lrx_fsm_state_s fabric_ser_lrx_fsm_state_t;

struct fabric_ser_interrupt_s	/* 276 */
{

	uint32 mask_set0	:32;

	uint32 mask_reset0	:32;

	uint32 value_set0	:32;

	uint32 value_reset0	:32;

	uint32 mask_set1	:32;

	uint32 mask_reset1	:32;

	uint32 value_set1	:32;

	uint32 value_reset1	:32;
};
typedef struct fabric_ser_interrupt_s fabric_ser_interrupt_t;

struct fabric_ser_syn_received_count_s	/* 277 */
{

	uint32 rsv_0	:4;
	uint32 syn_received_count3	:4;
	uint32 rsv_1	:4;
	uint32 syn_received_count2	:4;
	uint32 rsv_2	:4;
	uint32 syn_received_count1	:4;
	uint32 rsv_3	:4;
	uint32 syn_received_count0	:4;

	uint32 rsv_4	:4;
	uint32 syn_received_count7	:4;
	uint32 rsv_5	:4;
	uint32 syn_received_count6	:4;
	uint32 rsv_6	:4;
	uint32 syn_received_count5	:4;
	uint32 rsv_7	:4;
	uint32 syn_received_count4	:4;

	uint32 rsv_8	:4;
	uint32 syn_received_count11	:4;
	uint32 rsv_9	:4;
	uint32 syn_received_count10	:4;
	uint32 rsv_10	:4;
	uint32 syn_received_count9	:4;
	uint32 rsv_11	:4;
	uint32 syn_received_count8	:4;

	uint32 rsv_12	:4;
	uint32 syn_received_count15	:4;
	uint32 rsv_13	:4;
	uint32 syn_received_count14	:4;
	uint32 rsv_14	:4;
	uint32 syn_received_count13	:4;
	uint32 rsv_15	:4;
	uint32 syn_received_count12	:4;
};
typedef struct fabric_ser_syn_received_count_s fabric_ser_syn_received_count_t;

struct to_dsf_link_status_s	/* 278 */
{

	uint32 rsv_0	:16;
	uint32 to_dsf_link_status	:16;
};
typedef struct to_dsf_link_status_s to_dsf_link_status_t;

struct fabric_voq_chan_ctrl_s	/* 279 */
{

	uint32 chan_enable0	:32;

	uint32 chan_enable1	:32;

	uint32 chan_enable2	:32;

	uint32 chan_enable3	:32;

	uint32 rsv_0	:31;
	uint32 chan_info_init	:1;

	uint32 rsv_1	:31;
	uint32 chan_info_init_done	:1;
};
typedef struct fabric_voq_chan_ctrl_s fabric_voq_chan_ctrl_t;

struct fabric_voq_free_cell_ctrl_s	/* 280 */
{

	uint32 rsv_0	:23;
	uint32 free_cell_head_ptr	:9;

	uint32 rsv_1	:23;
	uint32 free_cell_tail_ptr	:9;

	uint32 rsv_2	:22;
	uint32 free_cell_count	:10;

	uint32 rsv_3	:28;
	uint32 pre_free_fifo_depth	:4;
};
typedef struct fabric_voq_free_cell_ctrl_s fabric_voq_free_cell_ctrl_t;

struct fabric_voq_cell_table_ctrl_s	/* 281 */
{

	uint32 rsv_0	:23;
	uint32 cell_table_init_end_addr	:9;

	uint32 rsv_1	:31;
	uint32 cell_table_init	:1;

	uint32 rsv_2	:31;
	uint32 cell_table_init_done	:1;
};
typedef struct fabric_voq_cell_table_ctrl_s fabric_voq_cell_table_ctrl_t;

struct fabric_voq_cell_buffer_ctrl_s	/* 282 */
{

	uint32 rsv_0	:22;
	uint32 cell_buffer_threshold	:10;

	uint32 rsv_1	:31;
	uint32 cell_buffer_init	:1;

	uint32 rsv_2	:31;
	uint32 cell_buffer_init_done	:1;

	uint32 rsv_3	:22;
	uint32 cell_buffer_init_wr_addr_end	:10;
};
typedef struct fabric_voq_cell_buffer_ctrl_s fabric_voq_cell_buffer_ctrl_t;

struct fabric_voq_cell_stats_s	/* 283 */
{

	uint32 rsv_0	:4;
	uint32 in_cell_count_priority3	:4;
	uint32 rsv_1	:4;
	uint32 in_cell_count_priority2	:4;
	uint32 rsv_2	:4;
	uint32 in_cell_count_priority1	:4;
	uint32 rsv_3	:4;
	uint32 in_cell_count_priority0	:4;

	uint32 rsv_4	:28;
	uint32 in_cell_count_total	:4;

	uint32 rsv_5	:4;
	uint32 out_cell_count_good_priority3	:4;
	uint32 rsv_6	:4;
	uint32 out_cell_count_good_priority2	:4;
	uint32 rsv_7	:4;
	uint32 out_cell_count_good_priority1	:4;
	uint32 rsv_8	:4;
	uint32 out_cell_count_good_priority0	:4;

	uint32 rsv_9	:28;
	uint32 out_cell_count_total_good	:4;

	uint32 rsv_10	:4;
	uint32 out_cell_count_idle_case100	:4;
	uint32 rsv_11	:4;
	uint32 out_cell_count_idle_case011	:4;
	uint32 rsv_12	:4;
	uint32 out_cell_count_idle_case010	:4;
	uint32 rsv_13	:4;
	uint32 out_cell_count_idle_case001	:4;

	uint32 rsv_14	:12;
	uint32 out_cell_count_idle_case111	:4;
	uint32 rsv_15	:4;
	uint32 out_cell_count_idle_case110	:4;
	uint32 rsv_16	:4;
	uint32 out_cell_count_idle_case101	:4;

	uint32 rsv_17	:28;
	uint32 out_cell_count_total_idle	:4;
};
typedef struct fabric_voq_cell_stats_s fabric_voq_cell_stats_t;

struct fabric_voq_pkt_stats_s	/* 284 */
{

	uint32 rsv_0	:12;
	uint32 in_sop_count_priority1	:4;
	uint32 rsv_1	:12;
	uint32 in_sop_count_priority0	:4;

	uint32 rsv_2	:12;
	uint32 in_sop_count_priority3	:4;
	uint32 rsv_3	:12;
	uint32 in_sop_count_priority2	:4;

	uint32 rsv_4	:28;
	uint32 in_sop_count_total	:4;

	uint32 rsv_5	:12;
	uint32 in_eop_count_priority1	:4;
	uint32 rsv_6	:12;
	uint32 in_eop_count_priority0	:4;

	uint32 rsv_7	:12;
	uint32 in_eop_count_priority3	:4;
	uint32 rsv_8	:12;
	uint32 in_eop_count_priority2	:4;

	uint32 rsv_9	:28;
	uint32 in_eop_count_total	:4;
};
typedef struct fabric_voq_pkt_stats_s fabric_voq_pkt_stats_t;

struct fabric_voq_interrupt_s	/* 285 */
{

	uint32 rsv_0	:17;
	uint32 mask_set	:15;

	uint32 rsv_1	:17;
	uint32 mask_reset	:15;

	uint32 rsv_2	:17;
	uint32 value_set	:15;

	uint32 rsv_3	:17;
	uint32 value_reset	:15;
};
typedef struct fabric_voq_interrupt_s fabric_voq_interrupt_t;

struct gmacwrapper_gmac_mac_mode_s	/* 286 */
{

	uint32 rsv_0	:30;
	uint32 speed_mode	:2;
};
typedef struct gmacwrapper_gmac_mac_mode_s gmacwrapper_gmac_mac_mode_t;

struct gmacwrapper_gmac_tx_ctrl_s	/* 287 */
{

	uint32 rsv_0	:12;
	uint32 full_threshold	:7;
	uint32 tx_threshold	:7;
	uint32 tx_err_mask	:1;
	uint32 append_crc_enable	:1;
	uint32 pad_enable	:1;
	uint32 tx_force_send_pause	:1;
	uint32 tx_flow_ctrl_enable	:1;
	uint32 tx_enable	:1;
};
typedef struct gmacwrapper_gmac_tx_ctrl_s gmacwrapper_gmac_tx_ctrl_t;

struct gmacwrapper_gmac_rx_ctrl_s	/* 288 */
{

	uint32 rsv_0	:25;
	uint32 crc_error_mask	:1;
	uint32 runt_rcv_enable	:1;
	uint32 len_field_chk_enable	:1;
	uint32 crc_chk_enable	:1;
	uint32 rx_flow_ctrl_enable	:1;
	uint32 rx_pause_bypass	:1;
	uint32 rx_enable	:1;
};
typedef struct gmacwrapper_gmac_rx_ctrl_s gmacwrapper_gmac_rx_ctrl_t;

struct gmacwrapper_gmac_pre_length_s	/* 289 */
{

	uint32 rsv_0	:27;
	uint32 pre_length	:5;
};
typedef struct gmacwrapper_gmac_pre_length_s gmacwrapper_gmac_pre_length_t;

struct gmacwrapper_gmac_pkt_length_s	/* 290 */
{

	uint32 rsv_0	:24;
	uint32 min_pkt_len	:8;
};
typedef struct gmacwrapper_gmac_pkt_length_s gmacwrapper_gmac_pkt_length_t;

struct gmacwrapper_gmac_interrupt_s	/* 291 */
{

	uint32 rsv_0	:24;
	uint32 value_set	:8;

	uint32 rsv_1	:24;
	uint32 value_reset	:8;

	uint32 rsv_2	:24;
	uint32 mask_set	:8;

	uint32 rsv_3	:24;
	uint32 mask_reset	:8;
};
typedef struct gmacwrapper_gmac_interrupt_s gmacwrapper_gmac_interrupt_t;

struct gmacwrapper_gmac_pause_ctrl_s	/* 292 */
{

	uint32 rsv_0	:16;
	uint32 pause_quanta	:16;
};
typedef struct gmacwrapper_gmac_pause_ctrl_s gmacwrapper_gmac_pause_ctrl_t;

struct gmacwrapper_gmac_vlan_type_s	/* 293 */
{

	uint32 rsv_0	:16;
	uint32 vlan_type	:16;
};
typedef struct gmacwrapper_gmac_vlan_type_s gmacwrapper_gmac_vlan_type_t;

struct gmac_pcs_config1_s	/* 294 */
{

	uint32 rsv_0	:30;
	uint32 sgmii100m_mode_cfg	:1;
	uint32 sgmii1g_mode_cfg	:1;
};
typedef struct gmac_pcs_config1_s gmac_pcs_config1_t;

struct gmac_pcs_config2_s	/* 295 */
{

	uint32 rsv_0	:24;
	uint32 force_sync	:1;
	uint32 force_signal_detect	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 lpbk_enable	:1;
	uint32 sgmii_nibble_repeat_cfg	:1;
	uint32 sgmii_endian_mode_cfg	:1;
	uint32 sig_det_active_value	:1;
};
typedef struct gmac_pcs_config2_s gmac_pcs_config2_t;

struct gmac_prbs_cfg_s	/* 296 */
{

	uint32 rsv_0	:30;
	uint32 pcs_cfg_prbs_rst	:1;
	uint32 pcs_cfg_prbs_enable	:1;
};
typedef struct gmac_prbs_cfg_s gmac_prbs_cfg_t;

struct gmac_prbs_err_cnt_s	/* 297 */
{

	uint32 rsv_0	:24;
	uint32 prbs_err_cnt_high	:8;
};
typedef struct gmac_prbs_err_cnt_s gmac_prbs_err_cnt_t;

struct gmac8_b10b_err_cnt_s	/* 298 */
{

	uint32 rsv_0	:24;
	uint32 code_err_cnt	:8;
};
typedef struct gmac8_b10b_err_cnt_s gmac8_b10b_err_cnt_t;

struct gmac_pcs_status_s	/* 299 */
{

	uint32 rsv_0	:30;
	uint32 sync_status	:1;
	uint32 signal_detect	:1;
};
typedef struct gmac_pcs_status_s gmac_pcs_status_t;

struct gmac_pcs_soft_rst_s	/* 300 */
{

	uint32 rsv_0	:28;
	uint32 gmii_tx_soft_rst	:1;
	uint32 gmii_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
};
typedef struct gmac_pcs_soft_rst_s gmac_pcs_soft_rst_t;

struct gmac_clk_divider_s	/* 301 */
{

	uint32 rsv_0	:29;
	uint32 clk_divider	:2;
	uint32 rst_clk_divider	:1;
};
typedef struct gmac_clk_divider_s gmac_clk_divider_t;

struct gmac_oam_test_master_cfg_s	/* 302 */
{

	uint32 rsv_0	:30;
	uint32 non_oam_discard	:1;
	uint32 oam_test_en	:1;
};
typedef struct gmac_oam_test_master_cfg_s gmac_oam_test_master_cfg_t;

struct gmac_oam_test_slave_cfg_s	/* 303 */
{

	uint32 rsv_0	:31;
	uint32 remote_loopback_en	:1;
};
typedef struct gmac_oam_test_slave_cfg_s gmac_oam_test_slave_cfg_t;

struct gmac_ptp_en_s	/* 304 */
{

	uint32 rsv_0	:30;
	uint32 ptp_en	:1;
	uint32 tx_ptp_error_en	:1;
};
typedef struct gmac_ptp_en_s gmac_ptp_en_t;

struct gmac_ptp_status_s	/* 305 */
{

	uint32 rsv_0	:28;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
};
typedef struct gmac_ptp_status_s gmac_ptp_status_t;

struct hash_ds_ctl_lookup_ctl_s	/* 306 */
{

	uint32 rsv_0	:13;
	uint32 mac_da_bits_num	:3;
	uint32 rsv_1	:9;
	uint32 mac_da_table_base	:7;

	uint32 rsv_2	:13;
	uint32 mac_sa_bits_num	:3;
	uint32 rsv_3	:9;
	uint32 mac_sa_table_base	:7;

	uint32 rsv_4	:13;
	uint32 ipv4_ucast_bits_num	:3;
	uint32 rsv_5	:9;
	uint32 ipv4_ucast_table_base	:7;

	uint32 rsv_6	:13;
	uint32 ipv4_mcast_bits_num	:3;
	uint32 rsv_7	:9;
	uint32 ipv4_mcast_table_base	:7;

	uint32 rsv_8	:13;
	uint32 ipv6_ucast_bits_num	:3;
	uint32 rsv_9	:9;
	uint32 ipv6_ucast_table_base	:7;

	uint32 rsv_10	:13;
	uint32 ipv6_mcast_bits_num	:3;
	uint32 rsv_11	:9;
	uint32 ipv6_mcast_table_base	:7;

	uint32 rsv_12	:13;
	uint32 ipv4_ucast_rpf_bit_num	:3;
	uint32 rsv_13	:9;
	uint32 ipv4_ucast_rpf_table_base	:7;

	uint32 rsv_14	:13;
	uint32 ipv6_ucast_rpf_bit_num	:3;
	uint32 rsv_15	:9;
	uint32 ipv6_ucast_rpf_table_base	:7;
};
typedef struct hash_ds_ctl_lookup_ctl_s hash_ds_ctl_lookup_ctl_t;

struct hash_ds_ctl_interrupt_fatal_s	/* 307 */
{

	uint32 rsv_0	:22;
	uint32 mask_set	:10;

	uint32 rsv_1	:22;
	uint32 mask_reset	:10;

	uint32 rsv_2	:22;
	uint32 value_set	:10;

	uint32 rsv_3	:22;
	uint32 value_reset	:10;
};
typedef struct hash_ds_ctl_interrupt_fatal_s hash_ds_ctl_interrupt_fatal_t;

struct hash_ds_ctl_parity_record_s	/* 308 */
{

	uint32 rsv_0	:7;
	uint32 hash_table98k_parity_fail	:1;
	uint32 rsv_1	:7;
	uint32 hash_table98k_parity_fail_addr	:17;

	uint32 rsv_2	:7;
	uint32 hash_table50k_parity_fail	:1;
	uint32 rsv_3	:8;
	uint32 hash_table50k_parity_fail_addr	:16;
};
typedef struct hash_ds_ctl_parity_record_s hash_ds_ctl_parity_record_t;

struct hash_ds_ctl_stats_s	/* 309 */
{

	uint32 rsv_0	:4;
	uint32 to_tcam_arb_ext_index_cnt	:4;
	uint32 rsv_1	:4;
	uint32 to_tcam_arb_int_index_cnt	:4;
	uint32 rsv_2	:4;
	uint32 fr_tcam_arb_ext_key_cnt	:4;
	uint32 rsv_3	:4;
	uint32 fr_tcam_arb_int_key_cnt	:4;

	uint32 rsv_4	:20;
	uint32 to_tb_info_rd_valid_cnt	:4;
	uint32 rsv_5	:4;
	uint32 fr_tb_info_rd_cnt	:4;
};
typedef struct hash_ds_ctl_stats_s hash_ds_ctl_stats_t;

struct hash_ds_ctl_init_ctl_s	/* 310 */
{

	uint32 rsv_0	:15;
	uint32 hash_tab_init_start_addr	:17;

	uint32 rsv_1	:15;
	uint32 hash_tab_init_end_addr	:17;

	uint32 rsv_2	:31;
	uint32 hash_tab_init_en	:1;

	uint32 rsv_3	:31;
	uint32 hash_tab_init_done	:1;
};
typedef struct hash_ds_ctl_init_ctl_s hash_ds_ctl_init_ctl_t;

struct hash_ds_ctl_misc_ctl_s	/* 311 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;

	uint32 rsv_1	:31;
	uint32 hash_tab_init_value_sel	:1;
};
typedef struct hash_ds_ctl_misc_ctl_s hash_ds_ctl_misc_ctl_t;

struct hash_ds_ctl_cpu_key_req_s	/* 312 */
{

	uint32 cpu_key_req_valid	:1;
	uint32 rsv_0	:14;
	uint32 cpu_key_req_wr	:1;
	uint32 rsv_1	:7;
	uint32 cpu_key_req_lu	:1;
	uint32 rsv_2	:7;
	uint32 cpu_key_req_del	:1;

	uint32 rsv_3	:13;
	uint32 cpu_key_type	:3;
	uint32 cpu_vrf_id	:16;

	uint32 cpu_ip127_to96	:32;

	uint32 cpu_ip95_to64	:32;

	uint32 cpu_ip63_to32	:32;

	uint32 cpu_ip31_to0	:32;
};
typedef struct hash_ds_ctl_cpu_key_req_s hash_ds_ctl_cpu_key_req_t;

struct hash_ds_ctl_cpu_key_status_s	/* 313 */
{

	uint32 cpu_key_hit	:1;
	uint32 rsv_0	:14;
	uint32 cpu_lu_index	:17;
};
typedef struct hash_ds_ctl_cpu_key_status_s hash_ds_ctl_cpu_key_status_t;

struct sup_if_control_s	/* 314 */
{

	uint32 rsv_0	:31;
	uint32 cfg_parity_check_en	:1;
};
typedef struct sup_if_control_s sup_if_control_t;

struct sup_if_parity_error_s	/* 315 */
{

	uint32 rsv_0	:16;
	uint32 cfg_parity_error_count	:16;
};
typedef struct sup_if_parity_error_s sup_if_parity_error_t;

struct device_id_s	/* 316 */
{

	uint32 rsv_0	:16;
	uint32 device_id_core	:8;
	uint32 rsv_1	:4;
	uint32 device_rev_core	:4;
};
typedef struct device_id_s device_id_t;

struct pll_lock_out_s	/* 317 */
{

	uint32 rsv_0	:15;
	uint32 if6g_pll_lock	:1;
	uint32 rsv_1	:3;
	uint32 if4g_pll_lock	:1;
	uint32 rsv_2	:3;
	uint32 qdr_pll_lock	:1;
	uint32 rsv_3	:3;
	uint32 tcam_pll_lock	:1;
	uint32 rsv_4	:3;
	uint32 core_pll_lock	:1;
};
typedef struct pll_lock_out_s pll_lock_out_t;

struct pll_lock_dbg_s	/* 318 */
{

	uint32 rsv_0	:8;
	uint32 mon_qdr_pll_lock_err_cnt	:8;
	uint32 mon_tcam_pll_lock_err_cnt	:8;
	uint32 mon_core_pll_lock_err_cnt	:8;

	uint32 rsv_1	:16;
	uint32 mon_if6g_pll_lock_err_cnt	:8;
	uint32 mon_if4g_pll_lock_err_cnt	:8;
};
typedef struct pll_lock_dbg_s pll_lock_dbg_t;

struct reset_int_related_s	/* 319 */
{

	uint32 reset_sup_gmac_reg15	:1;
	uint32 reset_sup_gmac15	:1;
	uint32 reset_sup_gmac_reg14	:1;
	uint32 reset_sup_gmac14	:1;
	uint32 reset_sup_gmac_reg13	:1;
	uint32 reset_sup_gmac13	:1;
	uint32 reset_sup_gmac_reg12	:1;
	uint32 reset_sup_gmac12	:1;
	uint32 reset_sup_gmac_reg11	:1;
	uint32 reset_sup_gmac11	:1;
	uint32 reset_sup_gmac_reg10	:1;
	uint32 reset_sup_gmac10	:1;
	uint32 reset_sup_gmac_reg9	:1;
	uint32 reset_sup_gmac9	:1;
	uint32 reset_sup_gmac_reg8	:1;
	uint32 reset_sup_gmac8	:1;
	uint32 reset_sup_gmac_reg7	:1;
	uint32 reset_sup_gmac7	:1;
	uint32 reset_sup_gmac_reg6	:1;
	uint32 reset_sup_gmac6	:1;
	uint32 reset_sup_gmac_reg5	:1;
	uint32 reset_sup_gmac5	:1;
	uint32 reset_sup_gmac_reg4	:1;
	uint32 reset_sup_gmac4	:1;
	uint32 reset_sup_gmac_reg3	:1;
	uint32 reset_sup_gmac3	:1;
	uint32 reset_sup_gmac_reg2	:1;
	uint32 reset_sup_gmac2	:1;
	uint32 reset_sup_gmac_reg1	:1;
	uint32 reset_sup_gmac1	:1;
	uint32 reset_sup_gmac_reg0	:1;
	uint32 reset_sup_gmac0	:1;

	uint32 reset_sup_gmac_reg31	:1;
	uint32 reset_sup_gmac31	:1;
	uint32 reset_sup_gmac_reg30	:1;
	uint32 reset_sup_gmac30	:1;
	uint32 reset_sup_gmac_reg29	:1;
	uint32 reset_sup_gmac29	:1;
	uint32 reset_sup_gmac_reg28	:1;
	uint32 reset_sup_gmac28	:1;
	uint32 reset_sup_gmac_reg27	:1;
	uint32 reset_sup_gmac27	:1;
	uint32 reset_sup_gmac_reg26	:1;
	uint32 reset_sup_gmac26	:1;
	uint32 reset_sup_gmac_reg25	:1;
	uint32 reset_sup_gmac25	:1;
	uint32 reset_sup_gmac_reg24	:1;
	uint32 reset_sup_gmac24	:1;
	uint32 reset_sup_gmac_reg23	:1;
	uint32 reset_sup_gmac23	:1;
	uint32 reset_sup_gmac_reg22	:1;
	uint32 reset_sup_gmac22	:1;
	uint32 reset_sup_gmac_reg21	:1;
	uint32 reset_sup_gmac21	:1;
	uint32 reset_sup_gmac_reg20	:1;
	uint32 reset_sup_gmac20	:1;
	uint32 reset_sup_gmac_reg19	:1;
	uint32 reset_sup_gmac19	:1;
	uint32 reset_sup_gmac_reg18	:1;
	uint32 reset_sup_gmac18	:1;
	uint32 reset_sup_gmac_reg17	:1;
	uint32 reset_sup_gmac17	:1;
	uint32 reset_sup_gmac_reg16	:1;
	uint32 reset_sup_gmac16	:1;

	uint32 reset_sup_gmac_reg47	:1;
	uint32 reset_sup_gmac47	:1;
	uint32 reset_sup_gmac_reg46	:1;
	uint32 reset_sup_gmac46	:1;
	uint32 reset_sup_gmac_reg45	:1;
	uint32 reset_sup_gmac45	:1;
	uint32 reset_sup_gmac_reg44	:1;
	uint32 reset_sup_gmac44	:1;
	uint32 reset_sup_gmac_reg43	:1;
	uint32 reset_sup_gmac43	:1;
	uint32 reset_sup_gmac_reg42	:1;
	uint32 reset_sup_gmac42	:1;
	uint32 reset_sup_gmac_reg41	:1;
	uint32 reset_sup_gmac41	:1;
	uint32 reset_sup_gmac_reg40	:1;
	uint32 reset_sup_gmac40	:1;
	uint32 reset_sup_gmac_reg39	:1;
	uint32 reset_sup_gmac39	:1;
	uint32 reset_sup_gmac_reg38	:1;
	uint32 reset_sup_gmac38	:1;
	uint32 reset_sup_gmac_reg37	:1;
	uint32 reset_sup_gmac37	:1;
	uint32 reset_sup_gmac_reg36	:1;
	uint32 reset_sup_gmac36	:1;
	uint32 reset_sup_gmac_reg35	:1;
	uint32 reset_sup_gmac35	:1;
	uint32 reset_sup_gmac_reg34	:1;
	uint32 reset_sup_gmac34	:1;
	uint32 reset_sup_gmac_reg33	:1;
	uint32 reset_sup_gmac33	:1;
	uint32 reset_sup_gmac_reg32	:1;
	uint32 reset_sup_gmac32	:1;

	uint32 reset_sup_ipe	:1;
	uint32 reset_sup_fabric_interface	:1;
	uint32 reset_sup_epe	:1;
	uint32 reset_supe_loop	:1;
	uint32 reset_sup_buf_retrv	:1;
	uint32 reset_sup_buf_store	:1;
	uint32 reset_sup_oam	:1;
	uint32 reset_sup_cpu_mac	:1;
	uint32 reset_sup_sg_mac3	:1;
	uint32 reset_sup_sg_mac2	:1;
	uint32 reset_sup_sg_mac1	:1;
	uint32 reset_sup_sg_mac0	:1;
	uint32 reset_sup_xgmac3	:1;
	uint32 reset_sup_xgmac2	:1;
	uint32 reset_sup_xgmac1	:1;
	uint32 reset_sup_xgmac0	:1;
	uint32 reset_sup_mux_agg3	:1;
	uint32 reset_sup_mux_agg2	:1;
	uint32 reset_sup_mux_agg1	:1;
	uint32 reset_sup_mux_agg0	:1;
	uint32 reset_sup_quad_mac_app11	:1;
	uint32 reset_sup_quad_mac_app10	:1;
	uint32 reset_sup_quad_mac_app9	:1;
	uint32 reset_sup_quad_mac_app8	:1;
	uint32 reset_sup_quad_mac_app7	:1;
	uint32 reset_sup_quad_mac_app6	:1;
	uint32 reset_sup_quad_mac_app5	:1;
	uint32 reset_sup_quad_mac_app4	:1;
	uint32 reset_sup_quad_mac_app3	:1;
	uint32 reset_sup_quad_mac_app2	:1;
	uint32 reset_sup_quad_mac_app1	:1;
	uint32 reset_sup_quad_mac_app0	:1;

	uint32 rsv_0	:4;
	uint32 reset_sup_reg_decode_ipe_epe_grp	:1;
	uint32 reset_sup_reg_decode_share_grp	:1;
	uint32 reset_sup_reg_decode_bsr_grp	:1;
	uint32 reset_sup_reg_decode_gmac_grp	:1;
	uint32 reset_sup_reg_decode_net_grp	:1;
	uint32 reset_sup_reg_decode_fabric_grp	:1;
	uint32 reset_sup_reg_decode_xgmac_grp	:1;
	uint32 reset_sup_reg_decode_core	:1;
	uint32 reset_sup_hash_ctl	:1;
	uint32 reset_sup_ptp_engine	:1;
	uint32 reset_sup_pb_ctl	:1;
	uint32 reset_sup_qdr_ctl	:1;
	uint32 reset_sup_ext_ddr_ctl	:1;
	uint32 reset_sup_tcam_ctl_ext	:1;
	uint32 reset_sup_tcam_ctl_int	:1;
	uint32 reset_sup_qdr_arb	:1;
	uint32 reset_sup_tb_info_arb	:1;
	uint32 reset_sup_tcam_arb	:1;
	uint32 reset_sup_shared_ds	:1;
	uint32 reset_sup_stp_state	:1;
	uint32 reset_sup_statistics	:1;
	uint32 reset_supq_mgr	:1;
	uint32 reset_sup_policing	:1;
	uint32 reset_sup_parser	:1;
	uint32 reset_sup_net_tx	:1;
	uint32 reset_sup_net_rx	:1;
	uint32 reset_sup_met_fifo	:1;
	uint32 reset_sup_mac_mux	:1;
};
typedef struct reset_int_related_s reset_int_related_t;

struct fatal_intr0_value_set_s	/* 320 */
{

	uint32 fatal_intr0_value_set	:32;
};
typedef struct fatal_intr0_value_set_s fatal_intr0_value_set_t;

struct fatal_intr0_value_reset_s	/* 321 */
{

	uint32 fatal_intr0_value_reset	:32;
};
typedef struct fatal_intr0_value_reset_s fatal_intr0_value_reset_t;

struct fatal_intr0_mask_set_s	/* 322 */
{

	uint32 fatal_intr0_mask_set	:32;
};
typedef struct fatal_intr0_mask_set_s fatal_intr0_mask_set_t;

struct fatal_intr0_mask_reset_s	/* 323 */
{

	uint32 fatal_intr0_mask_reset	:32;
};
typedef struct fatal_intr0_mask_reset_s fatal_intr0_mask_reset_t;

struct fatal_intr1_value_set_s	/* 324 */
{

	uint32 fatal_intr1_value_set	:32;
};
typedef struct fatal_intr1_value_set_s fatal_intr1_value_set_t;

struct fatal_intr1_value_reset_s	/* 325 */
{

	uint32 fatal_intr1_value_reset	:32;
};
typedef struct fatal_intr1_value_reset_s fatal_intr1_value_reset_t;

struct fatal_intr1_mask_set_s	/* 326 */
{

	uint32 fatal_intr1_mask_set	:32;
};
typedef struct fatal_intr1_mask_set_s fatal_intr1_mask_set_t;

struct fatal_intr1_mask_reset_s	/* 327 */
{

	uint32 fatal_intr1_mask_reset	:32;
};
typedef struct fatal_intr1_mask_reset_s fatal_intr1_mask_reset_t;

struct fatal_intr2_value_set_s	/* 328 */
{

	uint32 fatal_intr2_value_set	:32;
};
typedef struct fatal_intr2_value_set_s fatal_intr2_value_set_t;

struct fatal_intr2_value_reset_s	/* 329 */
{

	uint32 fatal_intr2_value_reset	:32;
};
typedef struct fatal_intr2_value_reset_s fatal_intr2_value_reset_t;

struct fatal_intr2_mask_set_s	/* 330 */
{

	uint32 fatal_intr2_mask_set	:32;
};
typedef struct fatal_intr2_mask_set_s fatal_intr2_mask_set_t;

struct fatal_intr2_mask_reset_s	/* 331 */
{

	uint32 fatal_intr2_mask_reset	:32;
};
typedef struct fatal_intr2_mask_reset_s fatal_intr2_mask_reset_t;

struct fatal_intr3_value_set_s	/* 332 */
{

	uint32 fatal_intr3_value_set	:32;
};
typedef struct fatal_intr3_value_set_s fatal_intr3_value_set_t;

struct fatal_intr3_value_reset_s	/* 333 */
{

	uint32 fatal_intr3_value_reset	:32;
};
typedef struct fatal_intr3_value_reset_s fatal_intr3_value_reset_t;

struct fatal_intr3_mask_set_s	/* 334 */
{

	uint32 fatal_intr3_mask_set	:32;
};
typedef struct fatal_intr3_mask_set_s fatal_intr3_mask_set_t;

struct fatal_intr3_mask_reset_s	/* 335 */
{

	uint32 fatal_intr3_mask_reset	:32;
};
typedef struct fatal_intr3_mask_reset_s fatal_intr3_mask_reset_t;

struct humbersup_normal_intr_value_set_s	/* 336 */
{

	uint32 rsv_0	:14;
	uint32 normal_intr_value_set	:18;
};
typedef struct humbersup_normal_intr_value_set_s humbersup_normal_intr_value_set_t;

struct humbersup_normal_intr_value_reset_s	/* 337 */
{

	uint32 rsv_0	:14;
	uint32 normal_intr_value_reset	:18;
};
typedef struct humbersup_normal_intr_value_reset_s humbersup_normal_intr_value_reset_t;

struct humbersup_normal_intr_mask_set_s	/* 338 */
{

	uint32 rsv_0	:14;
	uint32 normal_intr_mask_set	:18;
};
typedef struct humbersup_normal_intr_mask_set_s humbersup_normal_intr_mask_set_t;

struct humbersup_normal_intr_mask_reset_s	/* 339 */
{

	uint32 rsv_0	:14;
	uint32 normal_intr_mask_reset	:18;
};
typedef struct humbersup_normal_intr_mask_reset_s humbersup_normal_intr_mask_reset_t;

struct core_pll_control_s	/* 340 */
{

	uint32 rsv_0	:31;
	uint32 cfg_core_pll_reset	:1;

	uint32 rsv_1	:15;
	uint32 cfg_core_pll_sleep	:1;
	uint32 rsv_2	:7;
	uint32 cfg_core_pll_stop_clkb	:1;
	uint32 rsv_3	:7;
	uint32 cfg_core_pll_stop_clka	:1;

	uint32 rsv_4	:6;
	uint32 cfg_core_pll_tune	:10;
	uint32 rsv_5	:7;
	uint32 cfg_core_pll_intfbk	:1;
	uint32 rsv_6	:7;
	uint32 cfg_core_pll_bypass	:1;

	uint32 rsv_7	:3;
	uint32 cfg_core_pll_pre_div	:5;
	uint32 rsv_8	:7;
	uint32 cfg_core_pll_mult	:9;
	uint32 cfg_core_pll_rangeb	:4;
	uint32 cfg_core_pll_rangea	:4;
};
typedef struct core_pll_control_s core_pll_control_t;

struct hss4g_pll_control_s	/* 341 */
{

	uint32 rsv_0	:31;
	uint32 cfg_hss4g_pll_reset	:1;

	uint32 rsv_1	:15;
	uint32 cfg_hss4g_pll_mcent	:1;
	uint32 rsv_2	:3;
	uint32 cfg_hss4g_pll_out_en_dif	:1;
	uint32 rsv_3	:3;
	uint32 cfg_hss4g_pll_out_en_se	:1;
	uint32 rsv_4	:7;
	uint32 cfg_hss4g_pll_bypass	:1;

	uint32 rsv_5	:4;
	uint32 cfg_hss4g_pll_n2	:4;
	uint32 rsv_6	:4;
	uint32 cfg_hss4g_pll_n1	:4;
	uint32 rsv_7	:2;
	uint32 cfg_hss4g_pllm	:6;
	uint32 cfg_hss4g_pllc	:8;

	uint32 rsv_8	:20;
	uint32 cfg_hss4g_pllv	:4;
	uint32 rsv_9	:5;
	uint32 cfg_hss4g_pllp	:3;
};
typedef struct hss4g_pll_control_s hss4g_pll_control_t;

struct hss6g_pll_control_s	/* 342 */
{

	uint32 rsv_0	:31;
	uint32 cfg_hss6g_pll_reset	:1;

	uint32 rsv_1	:15;
	uint32 cfg_hss6g_pll_mcent	:1;
	uint32 rsv_2	:3;
	uint32 cfg_hss6g_pll_out_en_dif	:1;
	uint32 rsv_3	:3;
	uint32 cfg_hss6g_pll_out_en_se	:1;
	uint32 rsv_4	:7;
	uint32 cfg_hss6g_pll_bypass	:1;

	uint32 rsv_5	:4;
	uint32 cfg_hss6g_pll_n2	:4;
	uint32 rsv_6	:4;
	uint32 cfg_hss6g_pll_n1	:4;
	uint32 rsv_7	:2;
	uint32 cfg_hss6g_pllm	:6;
	uint32 cfg_hss6g_pllc	:8;

	uint32 rsv_8	:20;
	uint32 cfg_hss6g_pllv	:4;
	uint32 rsv_9	:5;
	uint32 cfg_hss6g_pllp	:3;
};
typedef struct hss6g_pll_control_s hss6g_pll_control_t;

struct ddr_pll_control_s	/* 343 */
{

	uint32 rsv_0	:31;
	uint32 cfg_table_pll_reset	:1;

	uint32 rsv_1	:15;
	uint32 cfg_table_pll_sleep	:1;
	uint32 rsv_2	:7;
	uint32 cfg_table_pll_stop_clkb	:1;
	uint32 rsv_3	:7;
	uint32 cfg_table_pll_stop_clka	:1;

	uint32 rsv_4	:6;
	uint32 cfg_table_pll_tune	:10;
	uint32 rsv_5	:7;
	uint32 cfg_table_pll_intfbk	:1;
	uint32 rsv_6	:7;
	uint32 cfg_table_pll_bypass	:1;

	uint32 rsv_7	:3;
	uint32 cfg_table_pll_pre_div	:5;
	uint32 rsv_8	:7;
	uint32 cfg_table_pll_mult	:9;
	uint32 cfg_table_pll_rangeb	:4;
	uint32 cfg_table_pll_rangea	:4;
};
typedef struct ddr_pll_control_s ddr_pll_control_t;

struct tcam_pll_control_s	/* 344 */
{

	uint32 rsv_0	:31;
	uint32 cfg_tcam_pll_reset	:1;

	uint32 rsv_1	:15;
	uint32 cfg_tcam_pll_sleep	:1;
	uint32 rsv_2	:7;
	uint32 cfg_tcam_pll_stop_clkb	:1;
	uint32 rsv_3	:7;
	uint32 cfg_tcam_pll_stop_clka	:1;

	uint32 rsv_4	:6;
	uint32 cfg_tcam_pll_tune	:10;
	uint32 rsv_5	:7;
	uint32 cfg_tcam_pll_intfbk	:1;
	uint32 rsv_6	:7;
	uint32 cfg_tcam_pll_bypass	:1;

	uint32 rsv_7	:3;
	uint32 cfg_tcam_pll_pre_div	:5;
	uint32 rsv_8	:7;
	uint32 cfg_tcam_pll_mult	:9;
	uint32 cfg_tcam_pll_rangeb	:4;
	uint32 cfg_tcam_pll_rangea	:4;
};
typedef struct tcam_pll_control_s tcam_pll_control_t;

struct fabric_syn_clk_control_s	/* 345 */
{

	uint32 rsv_0	:31;
	uint32 cfg_select_clock_fabric_sync	:1;
};
typedef struct fabric_syn_clk_control_s fabric_syn_clk_control_t;

struct humber_intr_enable_s	/* 346 */
{

	uint32 rsv_0	:31;
	uint32 humber_intr_enable	:1;
};
typedef struct humber_intr_enable_s humber_intr_enable_t;

struct clk_dbg_rst_s	/* 347 */
{

	uint32 rsv_0	:15;
	uint32 cfg_hss_clk_dbg_rst	:1;
	uint32 rsv_1	:3;
	uint32 cfg_tcam_pll_clk_dbg_rst	:1;
	uint32 rsv_2	:3;
	uint32 cfg_qdr_pll_clk_dbg_rst	:1;
	uint32 rsv_3	:3;
	uint32 cfg_core_pll_clk_dbg_rst	:1;
	uint32 rsv_4	:3;
	uint32 cfg_if_pll_clk_dbg_rst	:1;
};
typedef struct clk_dbg_rst_s clk_dbg_rst_t;

struct hss_access_parameter_s	/* 348 */
{

	uint32 cfg_hss_rd_out_valid_cycles	:8;
	uint32 cfg_hss_wr_hold_ipw_cycles	:8;
	uint32 cfg_hss_wr_pulse_width_cycles	:8;
	uint32 cfg_hss_wr_setup_cycles	:8;
};
typedef struct hss_access_parameter_s hss_access_parameter_t;

struct hss_access_s	/* 349 */
{

	uint32 hss_req	:1;
	uint32 rsv_0	:6;
	uint32 hss_read_data_valid	:1;
	uint32 rsv_1	:3;
	uint32 hss_req_type	:1;
	uint32 rsv_2	:3;
	uint32 hss_sel_rx	:1;
	uint32 rsv_3	:6;
	uint32 hss_addr	:10;
};
typedef struct hss_access_s hss_access_t;

struct hss_write_data_s	/* 350 */
{

	uint32 rsv_0	:16;
	uint32 hss_write_data	:16;
};
typedef struct hss_write_data_s hss_write_data_t;

struct hss_read_data_s	/* 351 */
{

	uint32 rsv_0	:16;
	uint32 hss_read_data	:16;
};
typedef struct hss_read_data_s hss_read_data_t;

struct hss_f0_tx_mon_s	/* 352 */
{

	uint32 rsv_0	:15;
	uint32 mon_f0_tx_hss_reset_out	:1;
	uint32 rsv_1	:7;
	uint32 mon_f0_tx_hss_pll_lock	:1;
	uint32 rsv_2	:7;
	uint32 sw_if_f0_tx_hss_prt_ready	:1;

	uint32 mon_f0_txp_prbs_sync	:1;
	uint32 mon_f0_txp_prbs_err	:1;
	uint32 mon_f0_txo_prbs_sync	:1;
	uint32 mon_f0_txo_prbs_err	:1;
	uint32 mon_f0_txn_prbs_sync	:1;
	uint32 mon_f0_txn_prbs_err	:1;
	uint32 mon_f0_txm_prbs_sync	:1;
	uint32 mon_f0_txm_prbs_err	:1;
	uint32 mon_f0_txl_prbs_sync	:1;
	uint32 mon_f0_txl_prbs_err	:1;
	uint32 mon_f0_txk_prbs_sync	:1;
	uint32 mon_f0_txk_prbs_err	:1;
	uint32 mon_f0_txj_prbs_sync	:1;
	uint32 mon_f0_txj_prbs_err	:1;
	uint32 mon_f0_txi_prbs_sync	:1;
	uint32 mon_f0_txi_prbs_err	:1;
	uint32 mon_f0_txh_prbs_sync	:1;
	uint32 mon_f0_txh_prbs_err	:1;
	uint32 mon_f0_txg_prbs_sync	:1;
	uint32 mon_f0_txg_prbs_err	:1;
	uint32 mon_f0_txf_prbs_sync	:1;
	uint32 mon_f0_txf_prbs_err	:1;
	uint32 mon_f0_txe_prbs_sync	:1;
	uint32 mon_f0_txe_prbs_err	:1;
	uint32 mon_f0_txd_prbs_sync	:1;
	uint32 mon_f0_txd_prbs_err	:1;
	uint32 mon_f0_txc_prbs_sync	:1;
	uint32 mon_f0_txc_prbs_err	:1;
	uint32 mon_f0_txb_prbs_sync	:1;
	uint32 mon_f0_txb_prbs_err	:1;
	uint32 mon_f0_txa_prbs_sync	:1;
	uint32 mon_f0_txa_prbs_err	:1;
};
typedef struct hss_f0_tx_mon_s hss_f0_tx_mon_t;

struct hss_f0_rx_mon_s	/* 353 */
{

	uint32 rsv_0	:15;
	uint32 mon_f0_rx_hss_reset_out	:1;
	uint32 rsv_1	:7;
	uint32 mon_f0_rx_hss_pll_lock	:1;
	uint32 rsv_2	:7;
	uint32 sw_if_f0_rx_hss_prt_ready	:1;

	uint32 mon_f0_rxp_prbs_sync	:1;
	uint32 mon_f0_rxp_prbs_err	:1;
	uint32 mon_f0_rxo_prbs_sync	:1;
	uint32 mon_f0_rxo_prbs_err	:1;
	uint32 mon_f0_rxn_prbs_sync	:1;
	uint32 mon_f0_rxn_prbs_err	:1;
	uint32 mon_f0_rxm_prbs_sync	:1;
	uint32 mon_f0_rxm_prbs_err	:1;
	uint32 mon_f0_rxl_prbs_sync	:1;
	uint32 mon_f0_rxl_prbs_err	:1;
	uint32 mon_f0_rxk_prbs_sync	:1;
	uint32 mon_f0_rxk_prbs_err	:1;
	uint32 mon_f0_rxj_prbs_sync	:1;
	uint32 mon_f0_rxj_prbs_err	:1;
	uint32 mon_f0_rxi_prbs_sync	:1;
	uint32 mon_f0_rxi_prbs_err	:1;
	uint32 mon_f0_rxh_prbs_sync	:1;
	uint32 mon_f0_rxh_prbs_err	:1;
	uint32 mon_f0_rxg_prbs_sync	:1;
	uint32 mon_f0_rxg_prbs_err	:1;
	uint32 mon_f0_rxf_prbs_sync	:1;
	uint32 mon_f0_rxf_prbs_err	:1;
	uint32 mon_f0_rxe_prbs_sync	:1;
	uint32 mon_f0_rxe_prbs_err	:1;
	uint32 mon_f0_rxd_prbs_sync	:1;
	uint32 mon_f0_rxd_prbs_err	:1;
	uint32 mon_f0_rxc_prbs_sync	:1;
	uint32 mon_f0_rxc_prbs_err	:1;
	uint32 mon_f0_rxb_prbs_sync	:1;
	uint32 mon_f0_rxb_prbs_err	:1;
	uint32 mon_f0_rxa_prbs_sync	:1;
	uint32 mon_f0_rxa_prbs_err	:1;
};
typedef struct hss_f0_rx_mon_s hss_f0_rx_mon_t;

struct hss_f0_tx_ctl_s	/* 354 */
{

	uint32 rsv_0	:31;
	uint32 cfg_f0_tx_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_f0_tx_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_f0_tx_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_f0_tx_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_f0_tx_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_f0_tx_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_f0_tx_hss_rst_config2	:1;
	uint32 rsv_7	:3;
	uint32 cfg_f0_tx_hss_lof_req	:1;
	uint32 rsv_8	:2;
	uint32 cfg_f0_tx_hss_div_sel	:2;

	uint32 cfg_f0_txp_prbs_rst	:1;
	uint32 cfg_f0_txp_prbs_en	:1;
	uint32 cfg_f0_txo_prbs_rst	:1;
	uint32 cfg_f0_txo_prbs_en	:1;
	uint32 cfg_f0_txn_prbs_rst	:1;
	uint32 cfg_f0_txn_prbs_en	:1;
	uint32 cfg_f0_txm_prbs_rst	:1;
	uint32 cfg_f0_txm_prbs_en	:1;
	uint32 cfg_f0_txl_prbs_rst	:1;
	uint32 cfg_f0_txl_prbs_en	:1;
	uint32 cfg_f0_txk_prbs_rst	:1;
	uint32 cfg_f0_txk_prbs_en	:1;
	uint32 cfg_f0_txj_prbs_rst	:1;
	uint32 cfg_f0_txj_prbs_en	:1;
	uint32 cfg_f0_txi_prbs_rst	:1;
	uint32 cfg_f0_txi_prbs_en	:1;
	uint32 cfg_f0_txh_prbs_rst	:1;
	uint32 cfg_f0_txh_prbs_en	:1;
	uint32 cfg_f0_txg_prbs_rst	:1;
	uint32 cfg_f0_txg_prbs_en	:1;
	uint32 cfg_f0_txf_prbs_rst	:1;
	uint32 cfg_f0_txf_prbs_en	:1;
	uint32 cfg_f0_txe_prbs_rst	:1;
	uint32 cfg_f0_txe_prbs_en	:1;
	uint32 cfg_f0_txd_prbs_rst	:1;
	uint32 cfg_f0_txd_prbs_en	:1;
	uint32 cfg_f0_txc_prbs_rst	:1;
	uint32 cfg_f0_txc_prbs_en	:1;
	uint32 cfg_f0_txb_prbs_rst	:1;
	uint32 cfg_f0_txb_prbs_en	:1;
	uint32 cfg_f0_txa_prbs_rst	:1;
	uint32 cfg_f0_txa_prbs_en	:1;

	uint32 rsv_9	:16;
	uint32 cfg_f0_txp_ts	:1;
	uint32 cfg_f0_txo_ts	:1;
	uint32 cfg_f0_txn_ts	:1;
	uint32 cfg_f0_txm_ts	:1;
	uint32 cfg_f0_txl_ts	:1;
	uint32 cfg_f0_txk_ts	:1;
	uint32 cfg_f0_txj_ts	:1;
	uint32 cfg_f0_txi_ts	:1;
	uint32 cfg_f0_txh_ts	:1;
	uint32 cfg_f0_txg_ts	:1;
	uint32 cfg_f0_txf_ts	:1;
	uint32 cfg_f0_txe_ts	:1;
	uint32 cfg_f0_txd_ts	:1;
	uint32 cfg_f0_txc_ts	:1;
	uint32 cfg_f0_txb_ts	:1;
	uint32 cfg_f0_txa_ts	:1;
};
typedef struct hss_f0_tx_ctl_s hss_f0_tx_ctl_t;

struct hss_f0_rx_ctl_s	/* 355 */
{

	uint32 rsv_0	:31;
	uint32 cfg_f0_rx_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_f0_rx_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_f0_rx_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_f0_rx_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_f0_rx_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_f0_rx_hss_rec_cal	:1;
	uint32 rsv_6	:1;
	uint32 cfg_f0_rx_hss_rst_config	:3;
	uint32 rsv_7	:2;
	uint32 cfg_f0_rx_hss_ac_mode	:1;
	uint32 cfg_f0_rx_hss_lof_req	:1;
	uint32 rsv_8	:2;
	uint32 cfg_f0_rx_hss_div_sel	:2;

	uint32 cfg_f0_rxh_data_sync	:1;
	uint32 cfg_f0_rxh_prbs_frc_err	:1;
	uint32 cfg_f0_rxh_prbs_rst	:1;
	uint32 cfg_f0_rxh_prbs_en	:1;
	uint32 cfg_f0_rxg_data_sync	:1;
	uint32 cfg_f0_rxg_prbs_frc_err	:1;
	uint32 cfg_f0_rxg_prbs_rst	:1;
	uint32 cfg_f0_rxg_prbs_en	:1;
	uint32 cfg_f0_rxf_data_sync	:1;
	uint32 cfg_f0_rxf_prbs_frc_err	:1;
	uint32 cfg_f0_rxf_prbs_rst	:1;
	uint32 cfg_f0_rxf_prbs_en	:1;
	uint32 cfg_f0_rxe_data_sync	:1;
	uint32 cfg_f0_rxe_prbs_frc_err	:1;
	uint32 cfg_f0_rxe_prbs_rst	:1;
	uint32 cfg_f0_rxe_prbs_en	:1;
	uint32 cfg_f0_rxd_data_sync	:1;
	uint32 cfg_f0_rxd_prbs_frc_err	:1;
	uint32 cfg_f0_rxd_prbs_rst	:1;
	uint32 cfg_f0_rxd_prbs_en	:1;
	uint32 cfg_f0_rxc_data_sync	:1;
	uint32 cfg_f0_rxc_prbs_frc_err	:1;
	uint32 cfg_f0_rxc_prbs_rst	:1;
	uint32 cfg_f0_rxc_prbs_en	:1;
	uint32 cfg_f0_rxb_data_sync	:1;
	uint32 cfg_f0_rxb_prbs_frc_err	:1;
	uint32 cfg_f0_rxb_prbs_rst	:1;
	uint32 cfg_f0_rxb_prbs_en	:1;
	uint32 cfg_f0_rxa_data_sync	:1;
	uint32 cfg_f0_rxa_prbs_frc_err	:1;
	uint32 cfg_f0_rxa_prbs_rst	:1;
	uint32 cfg_f0_rxa_prbs_en	:1;

	uint32 cfg_f0_rxp_data_sync	:1;
	uint32 cfg_f0_rxp_prbs_frc_err	:1;
	uint32 cfg_f0_rxp_prbs_rst	:1;
	uint32 cfg_f0_rxp_prbs_en	:1;
	uint32 cfg_f0_rxo_data_sync	:1;
	uint32 cfg_f0_rxo_prbs_frc_err	:1;
	uint32 cfg_f0_rxo_prbs_rst	:1;
	uint32 cfg_f0_rxo_prbs_en	:1;
	uint32 cfg_f0_rxn_data_sync	:1;
	uint32 cfg_f0_rxn_prbs_frc_err	:1;
	uint32 cfg_f0_rxn_prbs_rst	:1;
	uint32 cfg_f0_rxn_prbs_en	:1;
	uint32 cfg_f0_rxm_data_sync	:1;
	uint32 cfg_f0_rxm_prbs_frc_err	:1;
	uint32 cfg_f0_rxm_prbs_rst	:1;
	uint32 cfg_f0_rxm_prbs_en	:1;
	uint32 cfg_f0_rxl_data_sync	:1;
	uint32 cfg_f0_rxl_prbs_frc_err	:1;
	uint32 cfg_f0_rxl_prbs_rst	:1;
	uint32 cfg_f0_rxl_prbs_en	:1;
	uint32 cfg_f0_rxk_data_sync	:1;
	uint32 cfg_f0_rxk_prbs_frc_err	:1;
	uint32 cfg_f0_rxk_prbs_rst	:1;
	uint32 cfg_f0_rxk_prbs_en	:1;
	uint32 cfg_f0_rxj_data_sync	:1;
	uint32 cfg_f0_rxj_prbs_frc_err	:1;
	uint32 cfg_f0_rxj_prbs_rst	:1;
	uint32 cfg_f0_rxj_prbs_en	:1;
	uint32 cfg_f0_rxi_data_sync	:1;
	uint32 cfg_f0_rxi_prbs_frc_err	:1;
	uint32 cfg_f0_rxi_prbs_rst	:1;
	uint32 cfg_f0_rxi_prbs_en	:1;
};
typedef struct hss_f0_rx_ctl_s hss_f0_rx_ctl_t;

struct hss_n0_mon_s	/* 356 */
{

	uint32 rsv_0	:18;
	uint32 mon_n0_hss_ready	:1;
	uint32 mon_n0_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n0_hss_reset_out	:1;
	uint32 mon_n0_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n0_hss_eye_result	:7;

	uint32 mon_n0_txh_prbs_sync	:1;
	uint32 mon_n0_txh_prbs_err	:1;
	uint32 mon_n0_txg_prbs_sync	:1;
	uint32 mon_n0_txg_prbs_err	:1;
	uint32 mon_n0_txf_prbs_sync	:1;
	uint32 mon_n0_txf_prbs_err	:1;
	uint32 mon_n0_txe_prbs_sync	:1;
	uint32 mon_n0_txe_prbs_err	:1;
	uint32 mon_n0_txd_prbs_sync	:1;
	uint32 mon_n0_txd_prbs_err	:1;
	uint32 mon_n0_txc_prbs_sync	:1;
	uint32 mon_n0_txc_prbs_err	:1;
	uint32 mon_n0_txb_prbs_sync	:1;
	uint32 mon_n0_txb_prbs_err	:1;
	uint32 mon_n0_txa_prbs_sync	:1;
	uint32 mon_n0_txa_prbs_err	:1;
	uint32 mon_n0_rxh_prbs_sync	:1;
	uint32 mon_n0_rxh_prbs_err	:1;
	uint32 mon_n0_rxg_prbs_sync	:1;
	uint32 mon_n0_rxg_prbs_err	:1;
	uint32 mon_n0_rxf_prbs_sync	:1;
	uint32 mon_n0_rxf_prbs_err	:1;
	uint32 mon_n0_rxe_prbs_sync	:1;
	uint32 mon_n0_rxe_prbs_err	:1;
	uint32 mon_n0_rxd_prbs_sync	:1;
	uint32 mon_n0_rxd_prbs_err	:1;
	uint32 mon_n0_rxc_prbs_sync	:1;
	uint32 mon_n0_rxc_prbs_err	:1;
	uint32 mon_n0_rxb_prbs_sync	:1;
	uint32 mon_n0_rxb_prbs_err	:1;
	uint32 mon_n0_rxa_prbs_sync	:1;
	uint32 mon_n0_rxa_prbs_err	:1;
};
typedef struct hss_n0_mon_s hss_n0_mon_t;

struct hss_n1_mon_s	/* 357 */
{

	uint32 rsv_0	:18;
	uint32 mon_n1_hss_ready	:1;
	uint32 mon_n1_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n1_hss_reset_out	:1;
	uint32 mon_n1_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n1_hss_eye_result	:7;

	uint32 mon_n1_txh_prbs_sync	:1;
	uint32 mon_n1_txh_prbs_err	:1;
	uint32 mon_n1_txg_prbs_sync	:1;
	uint32 mon_n1_txg_prbs_err	:1;
	uint32 mon_n1_txf_prbs_sync	:1;
	uint32 mon_n1_txf_prbs_err	:1;
	uint32 mon_n1_txe_prbs_sync	:1;
	uint32 mon_n1_txe_prbs_err	:1;
	uint32 mon_n1_txd_prbs_sync	:1;
	uint32 mon_n1_txd_prbs_err	:1;
	uint32 mon_n1_txc_prbs_sync	:1;
	uint32 mon_n1_txc_prbs_err	:1;
	uint32 mon_n1_txb_prbs_sync	:1;
	uint32 mon_n1_txb_prbs_err	:1;
	uint32 mon_n1_txa_prbs_sync	:1;
	uint32 mon_n1_txa_prbs_err	:1;
	uint32 mon_n1_rxh_prbs_sync	:1;
	uint32 mon_n1_rxh_prbs_err	:1;
	uint32 mon_n1_rxg_prbs_sync	:1;
	uint32 mon_n1_rxg_prbs_err	:1;
	uint32 mon_n1_rxf_prbs_sync	:1;
	uint32 mon_n1_rxf_prbs_err	:1;
	uint32 mon_n1_rxe_prbs_sync	:1;
	uint32 mon_n1_rxe_prbs_err	:1;
	uint32 mon_n1_rxd_prbs_sync	:1;
	uint32 mon_n1_rxd_prbs_err	:1;
	uint32 mon_n1_rxc_prbs_sync	:1;
	uint32 mon_n1_rxc_prbs_err	:1;
	uint32 mon_n1_rxb_prbs_sync	:1;
	uint32 mon_n1_rxb_prbs_err	:1;
	uint32 mon_n1_rxa_prbs_sync	:1;
	uint32 mon_n1_rxa_prbs_err	:1;
};
typedef struct hss_n1_mon_s hss_n1_mon_t;

struct hss_n2_mon_s	/* 358 */
{

	uint32 rsv_0	:18;
	uint32 mon_n2_hss_ready	:1;
	uint32 mon_n2_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n2_hss_reset_out	:1;
	uint32 mon_n2_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n2_hss_eye_result	:7;

	uint32 mon_n2_txh_prbs_sync	:1;
	uint32 mon_n2_txh_prbs_err	:1;
	uint32 mon_n2_txg_prbs_sync	:1;
	uint32 mon_n2_txg_prbs_err	:1;
	uint32 mon_n2_txf_prbs_sync	:1;
	uint32 mon_n2_txf_prbs_err	:1;
	uint32 mon_n2_txe_prbs_sync	:1;
	uint32 mon_n2_txe_prbs_err	:1;
	uint32 mon_n2_txd_prbs_sync	:1;
	uint32 mon_n2_txd_prbs_err	:1;
	uint32 mon_n2_txc_prbs_sync	:1;
	uint32 mon_n2_txc_prbs_err	:1;
	uint32 mon_n2_txb_prbs_sync	:1;
	uint32 mon_n2_txb_prbs_err	:1;
	uint32 mon_n2_txa_prbs_sync	:1;
	uint32 mon_n2_txa_prbs_err	:1;
	uint32 mon_n2_rxh_prbs_sync	:1;
	uint32 mon_n2_rxh_prbs_err	:1;
	uint32 mon_n2_rxg_prbs_sync	:1;
	uint32 mon_n2_rxg_prbs_err	:1;
	uint32 mon_n2_rxf_prbs_sync	:1;
	uint32 mon_n2_rxf_prbs_err	:1;
	uint32 mon_n2_rxe_prbs_sync	:1;
	uint32 mon_n2_rxe_prbs_err	:1;
	uint32 mon_n2_rxd_prbs_sync	:1;
	uint32 mon_n2_rxd_prbs_err	:1;
	uint32 mon_n2_rxc_prbs_sync	:1;
	uint32 mon_n2_rxc_prbs_err	:1;
	uint32 mon_n2_rxb_prbs_sync	:1;
	uint32 mon_n2_rxb_prbs_err	:1;
	uint32 mon_n2_rxa_prbs_sync	:1;
	uint32 mon_n2_rxa_prbs_err	:1;
};
typedef struct hss_n2_mon_s hss_n2_mon_t;

struct hss_n3_mon_s	/* 359 */
{

	uint32 rsv_0	:18;
	uint32 mon_n3_hss_ready	:1;
	uint32 mon_n3_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n3_hss_reset_out	:1;
	uint32 mon_n3_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n3_hss_eye_result	:7;

	uint32 mon_n3_txh_prbs_sync	:1;
	uint32 mon_n3_txh_prbs_err	:1;
	uint32 mon_n3_txg_prbs_sync	:1;
	uint32 mon_n3_txg_prbs_err	:1;
	uint32 mon_n3_txf_prbs_sync	:1;
	uint32 mon_n3_txf_prbs_err	:1;
	uint32 mon_n3_txe_prbs_sync	:1;
	uint32 mon_n3_txe_prbs_err	:1;
	uint32 mon_n3_txd_prbs_sync	:1;
	uint32 mon_n3_txd_prbs_err	:1;
	uint32 mon_n3_txc_prbs_sync	:1;
	uint32 mon_n3_txc_prbs_err	:1;
	uint32 mon_n3_txb_prbs_sync	:1;
	uint32 mon_n3_txb_prbs_err	:1;
	uint32 mon_n3_txa_prbs_sync	:1;
	uint32 mon_n3_txa_prbs_err	:1;
	uint32 mon_n3_rxh_prbs_sync	:1;
	uint32 mon_n3_rxh_prbs_err	:1;
	uint32 mon_n3_rxg_prbs_sync	:1;
	uint32 mon_n3_rxg_prbs_err	:1;
	uint32 mon_n3_rxf_prbs_sync	:1;
	uint32 mon_n3_rxf_prbs_err	:1;
	uint32 mon_n3_rxe_prbs_sync	:1;
	uint32 mon_n3_rxe_prbs_err	:1;
	uint32 mon_n3_rxd_prbs_sync	:1;
	uint32 mon_n3_rxd_prbs_err	:1;
	uint32 mon_n3_rxc_prbs_sync	:1;
	uint32 mon_n3_rxc_prbs_err	:1;
	uint32 mon_n3_rxb_prbs_sync	:1;
	uint32 mon_n3_rxb_prbs_err	:1;
	uint32 mon_n3_rxa_prbs_sync	:1;
	uint32 mon_n3_rxa_prbs_err	:1;
};
typedef struct hss_n3_mon_s hss_n3_mon_t;

struct hss_n4_mon_s	/* 360 */
{

	uint32 rsv_0	:18;
	uint32 mon_n4_hss_ready	:1;
	uint32 mon_n4_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n4_hss_reset_out	:1;
	uint32 mon_n4_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n4_hss_eye_result	:7;

	uint32 mon_n4_txh_prbs_sync	:1;
	uint32 mon_n4_txh_prbs_err	:1;
	uint32 mon_n4_txg_prbs_sync	:1;
	uint32 mon_n4_txg_prbs_err	:1;
	uint32 mon_n4_txf_prbs_sync	:1;
	uint32 mon_n4_txf_prbs_err	:1;
	uint32 mon_n4_txe_prbs_sync	:1;
	uint32 mon_n4_txe_prbs_err	:1;
	uint32 mon_n4_txd_prbs_sync	:1;
	uint32 mon_n4_txd_prbs_err	:1;
	uint32 mon_n4_txc_prbs_sync	:1;
	uint32 mon_n4_txc_prbs_err	:1;
	uint32 mon_n4_txb_prbs_sync	:1;
	uint32 mon_n4_txb_prbs_err	:1;
	uint32 mon_n4_txa_prbs_sync	:1;
	uint32 mon_n4_txa_prbs_err	:1;
	uint32 mon_n4_rxh_prbs_sync	:1;
	uint32 mon_n4_rxh_prbs_err	:1;
	uint32 mon_n4_rxg_prbs_sync	:1;
	uint32 mon_n4_rxg_prbs_err	:1;
	uint32 mon_n4_rxf_prbs_sync	:1;
	uint32 mon_n4_rxf_prbs_err	:1;
	uint32 mon_n4_rxe_prbs_sync	:1;
	uint32 mon_n4_rxe_prbs_err	:1;
	uint32 mon_n4_rxd_prbs_sync	:1;
	uint32 mon_n4_rxd_prbs_err	:1;
	uint32 mon_n4_rxc_prbs_sync	:1;
	uint32 mon_n4_rxc_prbs_err	:1;
	uint32 mon_n4_rxb_prbs_sync	:1;
	uint32 mon_n4_rxb_prbs_err	:1;
	uint32 mon_n4_rxa_prbs_sync	:1;
	uint32 mon_n4_rxa_prbs_err	:1;
};
typedef struct hss_n4_mon_s hss_n4_mon_t;

struct hss_n5_mon_s	/* 361 */
{

	uint32 rsv_0	:18;
	uint32 mon_n5_hss_ready	:1;
	uint32 mon_n5_hss_pll_lock	:1;
	uint32 rsv_1	:2;
	uint32 mon_n5_hss_reset_out	:1;
	uint32 mon_n5_hss_eye_done	:1;
	uint32 rsv_2	:1;
	uint32 mon_n5_hss_eye_result	:7;

	uint32 mon_n5_txh_prbs_sync	:1;
	uint32 mon_n5_txh_prbs_err	:1;
	uint32 mon_n5_txg_prbs_sync	:1;
	uint32 mon_n5_txg_prbs_err	:1;
	uint32 mon_n5_txf_prbs_sync	:1;
	uint32 mon_n5_txf_prbs_err	:1;
	uint32 mon_n5_txe_prbs_sync	:1;
	uint32 mon_n5_txe_prbs_err	:1;
	uint32 mon_n5_txd_prbs_sync	:1;
	uint32 mon_n5_txd_prbs_err	:1;
	uint32 mon_n5_txc_prbs_sync	:1;
	uint32 mon_n5_txc_prbs_err	:1;
	uint32 mon_n5_txb_prbs_sync	:1;
	uint32 mon_n5_txb_prbs_err	:1;
	uint32 mon_n5_txa_prbs_sync	:1;
	uint32 mon_n5_txa_prbs_err	:1;
	uint32 mon_n5_rxh_prbs_sync	:1;
	uint32 mon_n5_rxh_prbs_err	:1;
	uint32 mon_n5_rxg_prbs_sync	:1;
	uint32 mon_n5_rxg_prbs_err	:1;
	uint32 mon_n5_rxf_prbs_sync	:1;
	uint32 mon_n5_rxf_prbs_err	:1;
	uint32 mon_n5_rxe_prbs_sync	:1;
	uint32 mon_n5_rxe_prbs_err	:1;
	uint32 mon_n5_rxd_prbs_sync	:1;
	uint32 mon_n5_rxd_prbs_err	:1;
	uint32 mon_n5_rxc_prbs_sync	:1;
	uint32 mon_n5_rxc_prbs_err	:1;
	uint32 mon_n5_rxb_prbs_sync	:1;
	uint32 mon_n5_rxb_prbs_err	:1;
	uint32 mon_n5_rxa_prbs_sync	:1;
	uint32 mon_n5_rxa_prbs_err	:1;
};
typedef struct hss_n5_mon_s hss_n5_mon_t;

struct hss_n0_ctl_s	/* 362 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n0_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n0_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n0_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n0_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n0_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n0_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n0_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n0_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n0_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n0_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n0_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n0_hss_eye_pr_up	:1;
	uint32 cfg_n0_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n0_hss_eye_pr_center	:1;
	uint32 cfg_n0_hss_eye_pr_bump32	:1;
	uint32 cfg_n0_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n0_hss_eye_mode_sel	:3;
	uint32 cfg_n0_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n0_rxa_prbs_en	:1;
	uint32 cfg_n0_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n0_rxa_prbs_frc_err	:1;
	uint32 cfg_n0_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n0_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n0_rxa_eq	:2;
	uint32 cfg_n0_rxa_qrt_clk_en	:1;
	uint32 cfg_n0_rxa_data_sync	:1;
	uint32 cfg_n0_rxa_sig_det_en	:1;
	uint32 cfg_n0_rxa_sig_lev	:5;
	uint32 cfg_n0_rxa_width	:2;
	uint32 cfg_n0_rxa_rate	:2;
	uint32 cfg_n0_rxa_data_loop	:1;
	uint32 cfg_n0_rxa_cdr_mode	:1;
	uint32 cfg_n0_rxa_bypass	:1;
	uint32 cfg_n0_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n0_rxb_prbs_en	:1;
	uint32 cfg_n0_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n0_rxb_prbs_frc_err	:1;
	uint32 cfg_n0_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n0_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n0_rxb_eq	:2;
	uint32 cfg_n0_rxb_qrt_clk_en	:1;
	uint32 cfg_n0_rxb_data_sync	:1;
	uint32 cfg_n0_rxb_sig_det_en	:1;
	uint32 cfg_n0_rxb_sig_lev	:5;
	uint32 cfg_n0_rxb_width	:2;
	uint32 cfg_n0_rxb_rate	:2;
	uint32 cfg_n0_rxb_data_loop	:1;
	uint32 cfg_n0_rxb_cdr_mode	:1;
	uint32 cfg_n0_rxb_bypass	:1;
	uint32 cfg_n0_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n0_rxc_prbs_en	:1;
	uint32 cfg_n0_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n0_rxc_prbs_frc_err	:1;
	uint32 cfg_n0_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n0_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n0_rxc_eq	:2;
	uint32 cfg_n0_rxc_qrt_clk_en	:1;
	uint32 cfg_n0_rxc_data_sync	:1;
	uint32 cfg_n0_rxc_sig_det_en	:1;
	uint32 cfg_n0_rxc_sig_lev	:5;
	uint32 cfg_n0_rxc_width	:2;
	uint32 cfg_n0_rxc_rate	:2;
	uint32 cfg_n0_rxc_data_loop	:1;
	uint32 cfg_n0_rxc_cdr_mode	:1;
	uint32 cfg_n0_rxc_bypass	:1;
	uint32 cfg_n0_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n0_rxd_prbs_en	:1;
	uint32 cfg_n0_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n0_rxd_prbs_frc_err	:1;
	uint32 cfg_n0_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n0_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n0_rxd_eq	:2;
	uint32 cfg_n0_rxd_qrt_clk_en	:1;
	uint32 cfg_n0_rxd_data_sync	:1;
	uint32 cfg_n0_rxd_sig_det_en	:1;
	uint32 cfg_n0_rxd_sig_lev	:5;
	uint32 cfg_n0_rxd_width	:2;
	uint32 cfg_n0_rxd_rate	:2;
	uint32 cfg_n0_rxd_data_loop	:1;
	uint32 cfg_n0_rxd_cdr_mode	:1;
	uint32 cfg_n0_rxd_bypass	:1;
	uint32 cfg_n0_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n0_rxe_prbs_en	:1;
	uint32 cfg_n0_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n0_rxe_prbs_frc_err	:1;
	uint32 cfg_n0_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n0_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n0_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n0_rxe_data_sync	:1;
	uint32 cfg_n0_rxe_sig_det_en	:1;
	uint32 cfg_n0_rxe_sig_lev	:5;
	uint32 cfg_n0_rxe_width	:2;
	uint32 cfg_n0_rxe_rate	:2;
	uint32 cfg_n0_rxe_data_loop	:1;
	uint32 cfg_n0_rxe_cdr_mode	:1;
	uint32 cfg_n0_rxe_bypass	:1;
	uint32 cfg_n0_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n0_rxf_prbs_en	:1;
	uint32 cfg_n0_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n0_rxf_prbs_frc_err	:1;
	uint32 cfg_n0_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n0_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n0_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n0_rxf_data_sync	:1;
	uint32 cfg_n0_rxf_sig_det_en	:1;
	uint32 cfg_n0_rxf_sig_lev	:5;
	uint32 cfg_n0_rxf_width	:2;
	uint32 cfg_n0_rxf_rate	:2;
	uint32 cfg_n0_rxf_data_loop	:1;
	uint32 cfg_n0_rxf_cdr_mode	:1;
	uint32 cfg_n0_rxf_bypass	:1;
	uint32 cfg_n0_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n0_rxg_prbs_en	:1;
	uint32 cfg_n0_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n0_rxg_prbs_frc_err	:1;
	uint32 cfg_n0_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n0_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n0_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n0_rxg_data_sync	:1;
	uint32 cfg_n0_rxg_sig_det_en	:1;
	uint32 cfg_n0_rxg_sig_lev	:5;
	uint32 cfg_n0_rxg_width	:2;
	uint32 cfg_n0_rxg_rate	:2;
	uint32 cfg_n0_rxg_data_loop	:1;
	uint32 cfg_n0_rxg_cdr_mode	:1;
	uint32 cfg_n0_rxg_bypass	:1;
	uint32 cfg_n0_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n0_rxh_prbs_en	:1;
	uint32 cfg_n0_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n0_rxh_prbs_frc_err	:1;
	uint32 cfg_n0_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n0_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n0_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n0_rxh_data_sync	:1;
	uint32 cfg_n0_rxh_sig_det_en	:1;
	uint32 cfg_n0_rxh_sig_lev	:5;
	uint32 cfg_n0_rxh_width	:2;
	uint32 cfg_n0_rxh_rate	:2;
	uint32 cfg_n0_rxh_data_loop	:1;
	uint32 cfg_n0_rxh_cdr_mode	:1;
	uint32 cfg_n0_rxh_bypass	:1;
	uint32 cfg_n0_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n0_txa_prbs_en	:1;
	uint32 cfg_n0_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n0_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n0_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n0_txa_drv_ampl	:3;
	uint32 cfg_n0_txa_drv_slew	:4;
	uint32 cfg_n0_txa_coef	:4;
	uint32 cfg_n0_txa_width	:2;
	uint32 cfg_n0_txa_rate	:2;
	uint32 cfg_n0_txa_byp_data	:1;
	uint32 cfg_n0_txa_ts	:1;
	uint32 cfg_n0_txa_bypass	:1;
	uint32 cfg_n0_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n0_txb_prbs_en	:1;
	uint32 cfg_n0_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n0_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n0_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n0_txb_drv_ampl	:3;
	uint32 cfg_n0_txb_drv_slew	:4;
	uint32 cfg_n0_txb_coef	:4;
	uint32 cfg_n0_txb_width	:2;
	uint32 cfg_n0_txb_rate	:2;
	uint32 cfg_n0_txb_byp_data	:1;
	uint32 cfg_n0_txb_ts	:1;
	uint32 cfg_n0_txb_bypass	:1;
	uint32 cfg_n0_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n0_txc_prbs_en	:1;
	uint32 cfg_n0_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n0_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n0_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n0_txc_drv_ampl	:3;
	uint32 cfg_n0_txc_drv_slew	:4;
	uint32 cfg_n0_txc_coef	:4;
	uint32 cfg_n0_txc_width	:2;
	uint32 cfg_n0_txc_rate	:2;
	uint32 cfg_n0_txc_byp_data	:1;
	uint32 cfg_n0_txc_ts	:1;
	uint32 cfg_n0_txc_bypass	:1;
	uint32 cfg_n0_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n0_txd_prbs_en	:1;
	uint32 cfg_n0_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n0_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n0_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n0_txd_drv_ampl	:3;
	uint32 cfg_n0_txd_drv_slew	:4;
	uint32 cfg_n0_txd_coef	:4;
	uint32 cfg_n0_txd_width	:2;
	uint32 cfg_n0_txd_rate	:2;
	uint32 cfg_n0_txd_byp_data	:1;
	uint32 cfg_n0_txd_ts	:1;
	uint32 cfg_n0_txd_bypass	:1;
	uint32 cfg_n0_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n0_txe_prbs_en	:1;
	uint32 cfg_n0_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n0_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n0_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n0_txe_drv_ampl	:3;
	uint32 cfg_n0_txe_drv_slew	:4;
	uint32 cfg_n0_txe_coef	:4;
	uint32 cfg_n0_txe_width	:2;
	uint32 cfg_n0_txe_rate	:2;
	uint32 cfg_n0_txe_byp_data	:1;
	uint32 cfg_n0_txe_ts	:1;
	uint32 cfg_n0_txe_bypass	:1;
	uint32 cfg_n0_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n0_txf_prbs_en	:1;
	uint32 cfg_n0_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n0_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n0_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n0_txf_drv_ampl	:3;
	uint32 cfg_n0_txf_drv_slew	:4;
	uint32 cfg_n0_txf_coef	:4;
	uint32 cfg_n0_txf_width	:2;
	uint32 cfg_n0_txf_rate	:2;
	uint32 cfg_n0_txf_byp_data	:1;
	uint32 cfg_n0_txf_ts	:1;
	uint32 cfg_n0_txf_bypass	:1;
	uint32 cfg_n0_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n0_txg_prbs_en	:1;
	uint32 cfg_n0_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n0_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n0_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n0_txg_drv_ampl	:3;
	uint32 cfg_n0_txg_drv_slew	:4;
	uint32 cfg_n0_txg_coef	:4;
	uint32 cfg_n0_txg_width	:2;
	uint32 cfg_n0_txg_rate	:2;
	uint32 cfg_n0_txg_byp_data	:1;
	uint32 cfg_n0_txg_ts	:1;
	uint32 cfg_n0_txg_bypass	:1;
	uint32 cfg_n0_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n0_txh_prbs_en	:1;
	uint32 cfg_n0_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n0_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n0_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n0_txh_drv_ampl	:3;
	uint32 cfg_n0_txh_drv_slew	:4;
	uint32 cfg_n0_txh_coef	:4;
	uint32 cfg_n0_txh_width	:2;
	uint32 cfg_n0_txh_rate	:2;
	uint32 cfg_n0_txh_byp_data	:1;
	uint32 cfg_n0_txh_ts	:1;
	uint32 cfg_n0_txh_bypass	:1;
	uint32 cfg_n0_txh_pwr_dwn	:1;
};
typedef struct hss_n0_ctl_s hss_n0_ctl_t;

struct hss_n1_ctl_s	/* 363 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n1_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n1_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n1_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n1_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n1_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n1_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n1_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n1_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n1_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n1_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n1_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n1_hss_eye_pr_up	:1;
	uint32 cfg_n1_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n1_hss_eye_pr_center	:1;
	uint32 cfg_n1_hss_eye_pr_bump32	:1;
	uint32 cfg_n1_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n1_hss_eye_mode_sel	:3;
	uint32 cfg_n1_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n1_rxa_prbs_en	:1;
	uint32 cfg_n1_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n1_rxa_prbs_frc_err	:1;
	uint32 cfg_n1_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n1_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n1_rxa_eq	:2;
	uint32 cfg_n1_rxa_qrt_clk_en	:1;
	uint32 cfg_n1_rxa_data_sync	:1;
	uint32 cfg_n1_rxa_sig_det_en	:1;
	uint32 cfg_n1_rxa_sig_lev	:5;
	uint32 cfg_n1_rxa_width	:2;
	uint32 cfg_n1_rxa_rate	:2;
	uint32 cfg_n1_rxa_data_loop	:1;
	uint32 cfg_n1_rxa_cdr_mode	:1;
	uint32 cfg_n1_rxa_bypass	:1;
	uint32 cfg_n1_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n1_rxb_prbs_en	:1;
	uint32 cfg_n1_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n1_rxb_prbs_frc_err	:1;
	uint32 cfg_n1_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n1_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n1_rxb_eq	:2;
	uint32 cfg_n1_rxb_qrt_clk_en	:1;
	uint32 cfg_n1_rxb_data_sync	:1;
	uint32 cfg_n1_rxb_sig_det_en	:1;
	uint32 cfg_n1_rxb_sig_lev	:5;
	uint32 cfg_n1_rxb_width	:2;
	uint32 cfg_n1_rxb_rate	:2;
	uint32 cfg_n1_rxb_data_loop	:1;
	uint32 cfg_n1_rxb_cdr_mode	:1;
	uint32 cfg_n1_rxb_bypass	:1;
	uint32 cfg_n1_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n1_rxc_prbs_en	:1;
	uint32 cfg_n1_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n1_rxc_prbs_frc_err	:1;
	uint32 cfg_n1_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n1_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n1_rxc_eq	:2;
	uint32 cfg_n1_rxc_qrt_clk_en	:1;
	uint32 cfg_n1_rxc_data_sync	:1;
	uint32 cfg_n1_rxc_sig_det_en	:1;
	uint32 cfg_n1_rxc_sig_lev	:5;
	uint32 cfg_n1_rxc_width	:2;
	uint32 cfg_n1_rxc_rate	:2;
	uint32 cfg_n1_rxc_data_loop	:1;
	uint32 cfg_n1_rxc_cdr_mode	:1;
	uint32 cfg_n1_rxc_bypass	:1;
	uint32 cfg_n1_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n1_rxd_prbs_en	:1;
	uint32 cfg_n1_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n1_rxd_prbs_frc_err	:1;
	uint32 cfg_n1_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n1_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n1_rxd_eq	:2;
	uint32 cfg_n1_rxd_qrt_clk_en	:1;
	uint32 cfg_n1_rxd_data_sync	:1;
	uint32 cfg_n1_rxd_sig_det_en	:1;
	uint32 cfg_n1_rxd_sig_lev	:5;
	uint32 cfg_n1_rxd_width	:2;
	uint32 cfg_n1_rxd_rate	:2;
	uint32 cfg_n1_rxd_data_loop	:1;
	uint32 cfg_n1_rxd_cdr_mode	:1;
	uint32 cfg_n1_rxd_bypass	:1;
	uint32 cfg_n1_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n1_rxe_prbs_en	:1;
	uint32 cfg_n1_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n1_rxe_prbs_frc_err	:1;
	uint32 cfg_n1_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n1_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n1_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n1_rxe_data_sync	:1;
	uint32 cfg_n1_rxe_sig_det_en	:1;
	uint32 cfg_n1_rxe_sig_lev	:5;
	uint32 cfg_n1_rxe_width	:2;
	uint32 cfg_n1_rxe_rate	:2;
	uint32 cfg_n1_rxe_data_loop	:1;
	uint32 cfg_n1_rxe_cdr_mode	:1;
	uint32 cfg_n1_rxe_bypass	:1;
	uint32 cfg_n1_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n1_rxf_prbs_en	:1;
	uint32 cfg_n1_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n1_rxf_prbs_frc_err	:1;
	uint32 cfg_n1_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n1_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n1_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n1_rxf_data_sync	:1;
	uint32 cfg_n1_rxf_sig_det_en	:1;
	uint32 cfg_n1_rxf_sig_lev	:5;
	uint32 cfg_n1_rxf_width	:2;
	uint32 cfg_n1_rxf_rate	:2;
	uint32 cfg_n1_rxf_data_loop	:1;
	uint32 cfg_n1_rxf_cdr_mode	:1;
	uint32 cfg_n1_rxf_bypass	:1;
	uint32 cfg_n1_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n1_rxg_prbs_en	:1;
	uint32 cfg_n1_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n1_rxg_prbs_frc_err	:1;
	uint32 cfg_n1_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n1_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n1_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n1_rxg_data_sync	:1;
	uint32 cfg_n1_rxg_sig_det_en	:1;
	uint32 cfg_n1_rxg_sig_lev	:5;
	uint32 cfg_n1_rxg_width	:2;
	uint32 cfg_n1_rxg_rate	:2;
	uint32 cfg_n1_rxg_data_loop	:1;
	uint32 cfg_n1_rxg_cdr_mode	:1;
	uint32 cfg_n1_rxg_bypass	:1;
	uint32 cfg_n1_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n1_rxh_prbs_en	:1;
	uint32 cfg_n1_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n1_rxh_prbs_frc_err	:1;
	uint32 cfg_n1_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n1_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n1_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n1_rxh_data_sync	:1;
	uint32 cfg_n1_rxh_sig_det_en	:1;
	uint32 cfg_n1_rxh_sig_lev	:5;
	uint32 cfg_n1_rxh_width	:2;
	uint32 cfg_n1_rxh_rate	:2;
	uint32 cfg_n1_rxh_data_loop	:1;
	uint32 cfg_n1_rxh_cdr_mode	:1;
	uint32 cfg_n1_rxh_bypass	:1;
	uint32 cfg_n1_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n1_txa_prbs_en	:1;
	uint32 cfg_n1_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n1_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n1_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n1_txa_drv_ampl	:3;
	uint32 cfg_n1_txa_drv_slew	:4;
	uint32 cfg_n1_txa_coef	:4;
	uint32 cfg_n1_txa_width	:2;
	uint32 cfg_n1_txa_rate	:2;
	uint32 cfg_n1_txa_byp_data	:1;
	uint32 cfg_n1_txa_ts	:1;
	uint32 cfg_n1_txa_bypass	:1;
	uint32 cfg_n1_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n1_txb_prbs_en	:1;
	uint32 cfg_n1_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n1_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n1_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n1_txb_drv_ampl	:3;
	uint32 cfg_n1_txb_drv_slew	:4;
	uint32 cfg_n1_txb_coef	:4;
	uint32 cfg_n1_txb_width	:2;
	uint32 cfg_n1_txb_rate	:2;
	uint32 cfg_n1_txb_byp_data	:1;
	uint32 cfg_n1_txb_ts	:1;
	uint32 cfg_n1_txb_bypass	:1;
	uint32 cfg_n1_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n1_txc_prbs_en	:1;
	uint32 cfg_n1_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n1_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n1_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n1_txc_drv_ampl	:3;
	uint32 cfg_n1_txc_drv_slew	:4;
	uint32 cfg_n1_txc_coef	:4;
	uint32 cfg_n1_txc_width	:2;
	uint32 cfg_n1_txc_rate	:2;
	uint32 cfg_n1_txc_byp_data	:1;
	uint32 cfg_n1_txc_ts	:1;
	uint32 cfg_n1_txc_bypass	:1;
	uint32 cfg_n1_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n1_txd_prbs_en	:1;
	uint32 cfg_n1_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n1_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n1_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n1_txd_drv_ampl	:3;
	uint32 cfg_n1_txd_drv_slew	:4;
	uint32 cfg_n1_txd_coef	:4;
	uint32 cfg_n1_txd_width	:2;
	uint32 cfg_n1_txd_rate	:2;
	uint32 cfg_n1_txd_byp_data	:1;
	uint32 cfg_n1_txd_ts	:1;
	uint32 cfg_n1_txd_bypass	:1;
	uint32 cfg_n1_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n1_txe_prbs_en	:1;
	uint32 cfg_n1_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n1_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n1_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n1_txe_drv_ampl	:3;
	uint32 cfg_n1_txe_drv_slew	:4;
	uint32 cfg_n1_txe_coef	:4;
	uint32 cfg_n1_txe_width	:2;
	uint32 cfg_n1_txe_rate	:2;
	uint32 cfg_n1_txe_byp_data	:1;
	uint32 cfg_n1_txe_ts	:1;
	uint32 cfg_n1_txe_bypass	:1;
	uint32 cfg_n1_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n1_txf_prbs_en	:1;
	uint32 cfg_n1_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n1_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n1_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n1_txf_drv_ampl	:3;
	uint32 cfg_n1_txf_drv_slew	:4;
	uint32 cfg_n1_txf_coef	:4;
	uint32 cfg_n1_txf_width	:2;
	uint32 cfg_n1_txf_rate	:2;
	uint32 cfg_n1_txf_byp_data	:1;
	uint32 cfg_n1_txf_ts	:1;
	uint32 cfg_n1_txf_bypass	:1;
	uint32 cfg_n1_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n1_txg_prbs_en	:1;
	uint32 cfg_n1_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n1_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n1_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n1_txg_drv_ampl	:3;
	uint32 cfg_n1_txg_drv_slew	:4;
	uint32 cfg_n1_txg_coef	:4;
	uint32 cfg_n1_txg_width	:2;
	uint32 cfg_n1_txg_rate	:2;
	uint32 cfg_n1_txg_byp_data	:1;
	uint32 cfg_n1_txg_ts	:1;
	uint32 cfg_n1_txg_bypass	:1;
	uint32 cfg_n1_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n1_txh_prbs_en	:1;
	uint32 cfg_n1_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n1_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n1_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n1_txh_drv_ampl	:3;
	uint32 cfg_n1_txh_drv_slew	:4;
	uint32 cfg_n1_txh_coef	:4;
	uint32 cfg_n1_txh_width	:2;
	uint32 cfg_n1_txh_rate	:2;
	uint32 cfg_n1_txh_byp_data	:1;
	uint32 cfg_n1_txh_ts	:1;
	uint32 cfg_n1_txh_bypass	:1;
	uint32 cfg_n1_txh_pwr_dwn	:1;
};
typedef struct hss_n1_ctl_s hss_n1_ctl_t;

struct hss_n2_ctl_s	/* 364 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n2_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n2_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n2_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n2_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n2_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n2_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n2_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n2_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n2_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n2_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n2_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n2_hss_eye_pr_up	:1;
	uint32 cfg_n2_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n2_hss_eye_pr_center	:1;
	uint32 cfg_n2_hss_eye_pr_bump32	:1;
	uint32 cfg_n2_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n2_hss_eye_mode_sel	:3;
	uint32 cfg_n2_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n2_rxa_prbs_en	:1;
	uint32 cfg_n2_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n2_rxa_prbs_frc_err	:1;
	uint32 cfg_n2_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n2_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n2_rxa_eq	:2;
	uint32 cfg_n2_rxa_qrt_clk_en	:1;
	uint32 cfg_n2_rxa_data_sync	:1;
	uint32 cfg_n2_rxa_sig_det_en	:1;
	uint32 cfg_n2_rxa_sig_lev	:5;
	uint32 cfg_n2_rxa_width	:2;
	uint32 cfg_n2_rxa_rate	:2;
	uint32 cfg_n2_rxa_data_loop	:1;
	uint32 cfg_n2_rxa_cdr_mode	:1;
	uint32 cfg_n2_rxa_bypass	:1;
	uint32 cfg_n2_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n2_rxb_prbs_en	:1;
	uint32 cfg_n2_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n2_rxb_prbs_frc_err	:1;
	uint32 cfg_n2_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n2_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n2_rxb_eq	:2;
	uint32 cfg_n2_rxb_qrt_clk_en	:1;
	uint32 cfg_n2_rxb_data_sync	:1;
	uint32 cfg_n2_rxb_sig_det_en	:1;
	uint32 cfg_n2_rxb_sig_lev	:5;
	uint32 cfg_n2_rxb_width	:2;
	uint32 cfg_n2_rxb_rate	:2;
	uint32 cfg_n2_rxb_data_loop	:1;
	uint32 cfg_n2_rxb_cdr_mode	:1;
	uint32 cfg_n2_rxb_bypass	:1;
	uint32 cfg_n2_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n2_rxc_prbs_en	:1;
	uint32 cfg_n2_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n2_rxc_prbs_frc_err	:1;
	uint32 cfg_n2_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n2_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n2_rxc_eq	:2;
	uint32 cfg_n2_rxc_qrt_clk_en	:1;
	uint32 cfg_n2_rxc_data_sync	:1;
	uint32 cfg_n2_rxc_sig_det_en	:1;
	uint32 cfg_n2_rxc_sig_lev	:5;
	uint32 cfg_n2_rxc_width	:2;
	uint32 cfg_n2_rxc_rate	:2;
	uint32 cfg_n2_rxc_data_loop	:1;
	uint32 cfg_n2_rxc_cdr_mode	:1;
	uint32 cfg_n2_rxc_bypass	:1;
	uint32 cfg_n2_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n2_rxd_prbs_en	:1;
	uint32 cfg_n2_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n2_rxd_prbs_frc_err	:1;
	uint32 cfg_n2_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n2_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n2_rxd_eq	:2;
	uint32 cfg_n2_rxd_qrt_clk_en	:1;
	uint32 cfg_n2_rxd_data_sync	:1;
	uint32 cfg_n2_rxd_sig_det_en	:1;
	uint32 cfg_n2_rxd_sig_lev	:5;
	uint32 cfg_n2_rxd_width	:2;
	uint32 cfg_n2_rxd_rate	:2;
	uint32 cfg_n2_rxd_data_loop	:1;
	uint32 cfg_n2_rxd_cdr_mode	:1;
	uint32 cfg_n2_rxd_bypass	:1;
	uint32 cfg_n2_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n2_rxe_prbs_en	:1;
	uint32 cfg_n2_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n2_rxe_prbs_frc_err	:1;
	uint32 cfg_n2_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n2_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n2_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n2_rxe_data_sync	:1;
	uint32 cfg_n2_rxe_sig_det_en	:1;
	uint32 cfg_n2_rxe_sig_lev	:5;
	uint32 cfg_n2_rxe_width	:2;
	uint32 cfg_n2_rxe_rate	:2;
	uint32 cfg_n2_rxe_data_loop	:1;
	uint32 cfg_n2_rxe_cdr_mode	:1;
	uint32 cfg_n2_rxe_bypass	:1;
	uint32 cfg_n2_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n2_rxf_prbs_en	:1;
	uint32 cfg_n2_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n2_rxf_prbs_frc_err	:1;
	uint32 cfg_n2_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n2_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n2_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n2_rxf_data_sync	:1;
	uint32 cfg_n2_rxf_sig_det_en	:1;
	uint32 cfg_n2_rxf_sig_lev	:5;
	uint32 cfg_n2_rxf_width	:2;
	uint32 cfg_n2_rxf_rate	:2;
	uint32 cfg_n2_rxf_data_loop	:1;
	uint32 cfg_n2_rxf_cdr_mode	:1;
	uint32 cfg_n2_rxf_bypass	:1;
	uint32 cfg_n2_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n2_rxg_prbs_en	:1;
	uint32 cfg_n2_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n2_rxg_prbs_frc_err	:1;
	uint32 cfg_n2_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n2_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n2_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n2_rxg_data_sync	:1;
	uint32 cfg_n2_rxg_sig_det_en	:1;
	uint32 cfg_n2_rxg_sig_lev	:5;
	uint32 cfg_n2_rxg_width	:2;
	uint32 cfg_n2_rxg_rate	:2;
	uint32 cfg_n2_rxg_data_loop	:1;
	uint32 cfg_n2_rxg_cdr_mode	:1;
	uint32 cfg_n2_rxg_bypass	:1;
	uint32 cfg_n2_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n2_rxh_prbs_en	:1;
	uint32 cfg_n2_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n2_rxh_prbs_frc_err	:1;
	uint32 cfg_n2_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n2_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n2_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n2_rxh_data_sync	:1;
	uint32 cfg_n2_rxh_sig_det_en	:1;
	uint32 cfg_n2_rxh_sig_lev	:5;
	uint32 cfg_n2_rxh_width	:2;
	uint32 cfg_n2_rxh_rate	:2;
	uint32 cfg_n2_rxh_data_loop	:1;
	uint32 cfg_n2_rxh_cdr_mode	:1;
	uint32 cfg_n2_rxh_bypass	:1;
	uint32 cfg_n2_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n2_txa_prbs_en	:1;
	uint32 cfg_n2_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n2_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n2_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n2_txa_drv_ampl	:3;
	uint32 cfg_n2_txa_drv_slew	:4;
	uint32 cfg_n2_txa_coef	:4;
	uint32 cfg_n2_txa_width	:2;
	uint32 cfg_n2_txa_rate	:2;
	uint32 cfg_n2_txa_byp_data	:1;
	uint32 cfg_n2_txa_ts	:1;
	uint32 cfg_n2_txa_bypass	:1;
	uint32 cfg_n2_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n2_txb_prbs_en	:1;
	uint32 cfg_n2_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n2_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n2_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n2_txb_drv_ampl	:3;
	uint32 cfg_n2_txb_drv_slew	:4;
	uint32 cfg_n2_txb_coef	:4;
	uint32 cfg_n2_txb_width	:2;
	uint32 cfg_n2_txb_rate	:2;
	uint32 cfg_n2_txb_byp_data	:1;
	uint32 cfg_n2_txb_ts	:1;
	uint32 cfg_n2_txb_bypass	:1;
	uint32 cfg_n2_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n2_txc_prbs_en	:1;
	uint32 cfg_n2_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n2_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n2_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n2_txc_drv_ampl	:3;
	uint32 cfg_n2_txc_drv_slew	:4;
	uint32 cfg_n2_txc_coef	:4;
	uint32 cfg_n2_txc_width	:2;
	uint32 cfg_n2_txc_rate	:2;
	uint32 cfg_n2_txc_byp_data	:1;
	uint32 cfg_n2_txc_ts	:1;
	uint32 cfg_n2_txc_bypass	:1;
	uint32 cfg_n2_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n2_txd_prbs_en	:1;
	uint32 cfg_n2_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n2_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n2_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n2_txd_drv_ampl	:3;
	uint32 cfg_n2_txd_drv_slew	:4;
	uint32 cfg_n2_txd_coef	:4;
	uint32 cfg_n2_txd_width	:2;
	uint32 cfg_n2_txd_rate	:2;
	uint32 cfg_n2_txd_byp_data	:1;
	uint32 cfg_n2_txd_ts	:1;
	uint32 cfg_n2_txd_bypass	:1;
	uint32 cfg_n2_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n2_txe_prbs_en	:1;
	uint32 cfg_n2_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n2_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n2_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n2_txe_drv_ampl	:3;
	uint32 cfg_n2_txe_drv_slew	:4;
	uint32 cfg_n2_txe_coef	:4;
	uint32 cfg_n2_txe_width	:2;
	uint32 cfg_n2_txe_rate	:2;
	uint32 cfg_n2_txe_byp_data	:1;
	uint32 cfg_n2_txe_ts	:1;
	uint32 cfg_n2_txe_bypass	:1;
	uint32 cfg_n2_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n2_txf_prbs_en	:1;
	uint32 cfg_n2_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n2_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n2_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n2_txf_drv_ampl	:3;
	uint32 cfg_n2_txf_drv_slew	:4;
	uint32 cfg_n2_txf_coef	:4;
	uint32 cfg_n2_txf_width	:2;
	uint32 cfg_n2_txf_rate	:2;
	uint32 cfg_n2_txf_byp_data	:1;
	uint32 cfg_n2_txf_ts	:1;
	uint32 cfg_n2_txf_bypass	:1;
	uint32 cfg_n2_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n2_txg_prbs_en	:1;
	uint32 cfg_n2_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n2_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n2_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n2_txg_drv_ampl	:3;
	uint32 cfg_n2_txg_drv_slew	:4;
	uint32 cfg_n2_txg_coef	:4;
	uint32 cfg_n2_txg_width	:2;
	uint32 cfg_n2_txg_rate	:2;
	uint32 cfg_n2_txg_byp_data	:1;
	uint32 cfg_n2_txg_ts	:1;
	uint32 cfg_n2_txg_bypass	:1;
	uint32 cfg_n2_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n2_txh_prbs_en	:1;
	uint32 cfg_n2_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n2_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n2_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n2_txh_drv_ampl	:3;
	uint32 cfg_n2_txh_drv_slew	:4;
	uint32 cfg_n2_txh_coef	:4;
	uint32 cfg_n2_txh_width	:2;
	uint32 cfg_n2_txh_rate	:2;
	uint32 cfg_n2_txh_byp_data	:1;
	uint32 cfg_n2_txh_ts	:1;
	uint32 cfg_n2_txh_bypass	:1;
	uint32 cfg_n2_txh_pwr_dwn	:1;
};
typedef struct hss_n2_ctl_s hss_n2_ctl_t;

struct hss_n3_ctl_s	/* 365 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n3_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n3_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n3_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n3_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n3_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n3_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n3_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n3_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n3_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n3_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n3_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n3_hss_eye_pr_up	:1;
	uint32 cfg_n3_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n3_hss_eye_pr_center	:1;
	uint32 cfg_n3_hss_eye_pr_bump32	:1;
	uint32 cfg_n3_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n3_hss_eye_mode_sel	:3;
	uint32 cfg_n3_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n3_rxa_prbs_en	:1;
	uint32 cfg_n3_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n3_rxa_prbs_frc_err	:1;
	uint32 cfg_n3_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n3_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n3_rxa_eq	:2;
	uint32 cfg_n3_rxa_qrt_clk_en	:1;
	uint32 cfg_n3_rxa_data_sync	:1;
	uint32 cfg_n3_rxa_sig_det_en	:1;
	uint32 cfg_n3_rxa_sig_lev	:5;
	uint32 cfg_n3_rxa_width	:2;
	uint32 cfg_n3_rxa_rate	:2;
	uint32 cfg_n3_rxa_data_loop	:1;
	uint32 cfg_n3_rxa_cdr_mode	:1;
	uint32 cfg_n3_rxa_bypass	:1;
	uint32 cfg_n3_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n3_rxb_prbs_en	:1;
	uint32 cfg_n3_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n3_rxb_prbs_frc_err	:1;
	uint32 cfg_n3_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n3_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n3_rxb_eq	:2;
	uint32 cfg_n3_rxb_qrt_clk_en	:1;
	uint32 cfg_n3_rxb_data_sync	:1;
	uint32 cfg_n3_rxb_sig_det_en	:1;
	uint32 cfg_n3_rxb_sig_lev	:5;
	uint32 cfg_n3_rxb_width	:2;
	uint32 cfg_n3_rxb_rate	:2;
	uint32 cfg_n3_rxb_data_loop	:1;
	uint32 cfg_n3_rxb_cdr_mode	:1;
	uint32 cfg_n3_rxb_bypass	:1;
	uint32 cfg_n3_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n3_rxc_prbs_en	:1;
	uint32 cfg_n3_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n3_rxc_prbs_frc_err	:1;
	uint32 cfg_n3_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n3_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n3_rxc_eq	:2;
	uint32 cfg_n3_rxc_qrt_clk_en	:1;
	uint32 cfg_n3_rxc_data_sync	:1;
	uint32 cfg_n3_rxc_sig_det_en	:1;
	uint32 cfg_n3_rxc_sig_lev	:5;
	uint32 cfg_n3_rxc_width	:2;
	uint32 cfg_n3_rxc_rate	:2;
	uint32 cfg_n3_rxc_data_loop	:1;
	uint32 cfg_n3_rxc_cdr_mode	:1;
	uint32 cfg_n3_rxc_bypass	:1;
	uint32 cfg_n3_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n3_rxd_prbs_en	:1;
	uint32 cfg_n3_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n3_rxd_prbs_frc_err	:1;
	uint32 cfg_n3_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n3_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n3_rxd_eq	:2;
	uint32 cfg_n3_rxd_qrt_clk_en	:1;
	uint32 cfg_n3_rxd_data_sync	:1;
	uint32 cfg_n3_rxd_sig_det_en	:1;
	uint32 cfg_n3_rxd_sig_lev	:5;
	uint32 cfg_n3_rxd_width	:2;
	uint32 cfg_n3_rxd_rate	:2;
	uint32 cfg_n3_rxd_data_loop	:1;
	uint32 cfg_n3_rxd_cdr_mode	:1;
	uint32 cfg_n3_rxd_bypass	:1;
	uint32 cfg_n3_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n3_rxe_prbs_en	:1;
	uint32 cfg_n3_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n3_rxe_prbs_frc_err	:1;
	uint32 cfg_n3_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n3_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n3_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n3_rxe_data_sync	:1;
	uint32 cfg_n3_rxe_sig_det_en	:1;
	uint32 cfg_n3_rxe_sig_lev	:5;
	uint32 cfg_n3_rxe_width	:2;
	uint32 cfg_n3_rxe_rate	:2;
	uint32 cfg_n3_rxe_data_loop	:1;
	uint32 cfg_n3_rxe_cdr_mode	:1;
	uint32 cfg_n3_rxe_bypass	:1;
	uint32 cfg_n3_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n3_rxf_prbs_en	:1;
	uint32 cfg_n3_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n3_rxf_prbs_frc_err	:1;
	uint32 cfg_n3_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n3_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n3_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n3_rxf_data_sync	:1;
	uint32 cfg_n3_rxf_sig_det_en	:1;
	uint32 cfg_n3_rxf_sig_lev	:5;
	uint32 cfg_n3_rxf_width	:2;
	uint32 cfg_n3_rxf_rate	:2;
	uint32 cfg_n3_rxf_data_loop	:1;
	uint32 cfg_n3_rxf_cdr_mode	:1;
	uint32 cfg_n3_rxf_bypass	:1;
	uint32 cfg_n3_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n3_rxg_prbs_en	:1;
	uint32 cfg_n3_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n3_rxg_prbs_frc_err	:1;
	uint32 cfg_n3_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n3_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n3_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n3_rxg_data_sync	:1;
	uint32 cfg_n3_rxg_sig_det_en	:1;
	uint32 cfg_n3_rxg_sig_lev	:5;
	uint32 cfg_n3_rxg_width	:2;
	uint32 cfg_n3_rxg_rate	:2;
	uint32 cfg_n3_rxg_data_loop	:1;
	uint32 cfg_n3_rxg_cdr_mode	:1;
	uint32 cfg_n3_rxg_bypass	:1;
	uint32 cfg_n3_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n3_rxh_prbs_en	:1;
	uint32 cfg_n3_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n3_rxh_prbs_frc_err	:1;
	uint32 cfg_n3_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n3_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n3_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n3_rxh_data_sync	:1;
	uint32 cfg_n3_rxh_sig_det_en	:1;
	uint32 cfg_n3_rxh_sig_lev	:5;
	uint32 cfg_n3_rxh_width	:2;
	uint32 cfg_n3_rxh_rate	:2;
	uint32 cfg_n3_rxh_data_loop	:1;
	uint32 cfg_n3_rxh_cdr_mode	:1;
	uint32 cfg_n3_rxh_bypass	:1;
	uint32 cfg_n3_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n3_txa_prbs_en	:1;
	uint32 cfg_n3_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n3_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n3_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n3_txa_drv_ampl	:3;
	uint32 cfg_n3_txa_drv_slew	:4;
	uint32 cfg_n3_txa_coef	:4;
	uint32 cfg_n3_txa_width	:2;
	uint32 cfg_n3_txa_rate	:2;
	uint32 cfg_n3_txa_byp_data	:1;
	uint32 cfg_n3_txa_ts	:1;
	uint32 cfg_n3_txa_bypass	:1;
	uint32 cfg_n3_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n3_txb_prbs_en	:1;
	uint32 cfg_n3_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n3_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n3_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n3_txb_drv_ampl	:3;
	uint32 cfg_n3_txb_drv_slew	:4;
	uint32 cfg_n3_txb_coef	:4;
	uint32 cfg_n3_txb_width	:2;
	uint32 cfg_n3_txb_rate	:2;
	uint32 cfg_n3_txb_byp_data	:1;
	uint32 cfg_n3_txb_ts	:1;
	uint32 cfg_n3_txb_bypass	:1;
	uint32 cfg_n3_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n3_txc_prbs_en	:1;
	uint32 cfg_n3_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n3_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n3_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n3_txc_drv_ampl	:3;
	uint32 cfg_n3_txc_drv_slew	:4;
	uint32 cfg_n3_txc_coef	:4;
	uint32 cfg_n3_txc_width	:2;
	uint32 cfg_n3_txc_rate	:2;
	uint32 cfg_n3_txc_byp_data	:1;
	uint32 cfg_n3_txc_ts	:1;
	uint32 cfg_n3_txc_bypass	:1;
	uint32 cfg_n3_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n3_txd_prbs_en	:1;
	uint32 cfg_n3_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n3_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n3_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n3_txd_drv_ampl	:3;
	uint32 cfg_n3_txd_drv_slew	:4;
	uint32 cfg_n3_txd_coef	:4;
	uint32 cfg_n3_txd_width	:2;
	uint32 cfg_n3_txd_rate	:2;
	uint32 cfg_n3_txd_byp_data	:1;
	uint32 cfg_n3_txd_ts	:1;
	uint32 cfg_n3_txd_bypass	:1;
	uint32 cfg_n3_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n3_txe_prbs_en	:1;
	uint32 cfg_n3_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n3_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n3_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n3_txe_drv_ampl	:3;
	uint32 cfg_n3_txe_drv_slew	:4;
	uint32 cfg_n3_txe_coef	:4;
	uint32 cfg_n3_txe_width	:2;
	uint32 cfg_n3_txe_rate	:2;
	uint32 cfg_n3_txe_byp_data	:1;
	uint32 cfg_n3_txe_ts	:1;
	uint32 cfg_n3_txe_bypass	:1;
	uint32 cfg_n3_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n3_txf_prbs_en	:1;
	uint32 cfg_n3_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n3_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n3_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n3_txf_drv_ampl	:3;
	uint32 cfg_n3_txf_drv_slew	:4;
	uint32 cfg_n3_txf_coef	:4;
	uint32 cfg_n3_txf_width	:2;
	uint32 cfg_n3_txf_rate	:2;
	uint32 cfg_n3_txf_byp_data	:1;
	uint32 cfg_n3_txf_ts	:1;
	uint32 cfg_n3_txf_bypass	:1;
	uint32 cfg_n3_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n3_txg_prbs_en	:1;
	uint32 cfg_n3_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n3_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n3_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n3_txg_drv_ampl	:3;
	uint32 cfg_n3_txg_drv_slew	:4;
	uint32 cfg_n3_txg_coef	:4;
	uint32 cfg_n3_txg_width	:2;
	uint32 cfg_n3_txg_rate	:2;
	uint32 cfg_n3_txg_byp_data	:1;
	uint32 cfg_n3_txg_ts	:1;
	uint32 cfg_n3_txg_bypass	:1;
	uint32 cfg_n3_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n3_txh_prbs_en	:1;
	uint32 cfg_n3_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n3_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n3_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n3_txh_drv_ampl	:3;
	uint32 cfg_n3_txh_drv_slew	:4;
	uint32 cfg_n3_txh_coef	:4;
	uint32 cfg_n3_txh_width	:2;
	uint32 cfg_n3_txh_rate	:2;
	uint32 cfg_n3_txh_byp_data	:1;
	uint32 cfg_n3_txh_ts	:1;
	uint32 cfg_n3_txh_bypass	:1;
	uint32 cfg_n3_txh_pwr_dwn	:1;
};
typedef struct hss_n3_ctl_s hss_n3_ctl_t;

struct hss_n4_ctl_s	/* 366 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n4_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n4_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n4_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n4_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n4_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n4_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n4_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n4_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n4_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n4_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n4_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n4_hss_eye_pr_up	:1;
	uint32 cfg_n4_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n4_hss_eye_pr_center	:1;
	uint32 cfg_n4_hss_eye_pr_bump32	:1;
	uint32 cfg_n4_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n4_hss_eye_mode_sel	:3;
	uint32 cfg_n4_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n4_rxa_prbs_en	:1;
	uint32 cfg_n4_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n4_rxa_prbs_frc_err	:1;
	uint32 cfg_n4_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n4_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n4_rxa_eq	:2;
	uint32 cfg_n4_rxa_qrt_clk_en	:1;
	uint32 cfg_n4_rxa_data_sync	:1;
	uint32 cfg_n4_rxa_sig_det_en	:1;
	uint32 cfg_n4_rxa_sig_lev	:5;
	uint32 cfg_n4_rxa_width	:2;
	uint32 cfg_n4_rxa_rate	:2;
	uint32 cfg_n4_rxa_data_loop	:1;
	uint32 cfg_n4_rxa_cdr_mode	:1;
	uint32 cfg_n4_rxa_bypass	:1;
	uint32 cfg_n4_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n4_rxb_prbs_en	:1;
	uint32 cfg_n4_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n4_rxb_prbs_frc_err	:1;
	uint32 cfg_n4_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n4_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n4_rxb_eq	:2;
	uint32 cfg_n4_rxb_qrt_clk_en	:1;
	uint32 cfg_n4_rxb_data_sync	:1;
	uint32 cfg_n4_rxb_sig_det_en	:1;
	uint32 cfg_n4_rxb_sig_lev	:5;
	uint32 cfg_n4_rxb_width	:2;
	uint32 cfg_n4_rxb_rate	:2;
	uint32 cfg_n4_rxb_data_loop	:1;
	uint32 cfg_n4_rxb_cdr_mode	:1;
	uint32 cfg_n4_rxb_bypass	:1;
	uint32 cfg_n4_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n4_rxc_prbs_en	:1;
	uint32 cfg_n4_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n4_rxc_prbs_frc_err	:1;
	uint32 cfg_n4_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n4_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n4_rxc_eq	:2;
	uint32 cfg_n4_rxc_qrt_clk_en	:1;
	uint32 cfg_n4_rxc_data_sync	:1;
	uint32 cfg_n4_rxc_sig_det_en	:1;
	uint32 cfg_n4_rxc_sig_lev	:5;
	uint32 cfg_n4_rxc_width	:2;
	uint32 cfg_n4_rxc_rate	:2;
	uint32 cfg_n4_rxc_data_loop	:1;
	uint32 cfg_n4_rxc_cdr_mode	:1;
	uint32 cfg_n4_rxc_bypass	:1;
	uint32 cfg_n4_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n4_rxd_prbs_en	:1;
	uint32 cfg_n4_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n4_rxd_prbs_frc_err	:1;
	uint32 cfg_n4_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n4_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n4_rxd_eq	:2;
	uint32 cfg_n4_rxd_qrt_clk_en	:1;
	uint32 cfg_n4_rxd_data_sync	:1;
	uint32 cfg_n4_rxd_sig_det_en	:1;
	uint32 cfg_n4_rxd_sig_lev	:5;
	uint32 cfg_n4_rxd_width	:2;
	uint32 cfg_n4_rxd_rate	:2;
	uint32 cfg_n4_rxd_data_loop	:1;
	uint32 cfg_n4_rxd_cdr_mode	:1;
	uint32 cfg_n4_rxd_bypass	:1;
	uint32 cfg_n4_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n4_rxe_prbs_en	:1;
	uint32 cfg_n4_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n4_rxe_prbs_frc_err	:1;
	uint32 cfg_n4_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n4_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n4_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n4_rxe_data_sync	:1;
	uint32 cfg_n4_rxe_sig_det_en	:1;
	uint32 cfg_n4_rxe_sig_lev	:5;
	uint32 cfg_n4_rxe_width	:2;
	uint32 cfg_n4_rxe_rate	:2;
	uint32 cfg_n4_rxe_data_loop	:1;
	uint32 cfg_n4_rxe_cdr_mode	:1;
	uint32 cfg_n4_rxe_bypass	:1;
	uint32 cfg_n4_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n4_rxf_prbs_en	:1;
	uint32 cfg_n4_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n4_rxf_prbs_frc_err	:1;
	uint32 cfg_n4_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n4_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n4_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n4_rxf_data_sync	:1;
	uint32 cfg_n4_rxf_sig_det_en	:1;
	uint32 cfg_n4_rxf_sig_lev	:5;
	uint32 cfg_n4_rxf_width	:2;
	uint32 cfg_n4_rxf_rate	:2;
	uint32 cfg_n4_rxf_data_loop	:1;
	uint32 cfg_n4_rxf_cdr_mode	:1;
	uint32 cfg_n4_rxf_bypass	:1;
	uint32 cfg_n4_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n4_rxg_prbs_en	:1;
	uint32 cfg_n4_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n4_rxg_prbs_frc_err	:1;
	uint32 cfg_n4_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n4_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n4_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n4_rxg_data_sync	:1;
	uint32 cfg_n4_rxg_sig_det_en	:1;
	uint32 cfg_n4_rxg_sig_lev	:5;
	uint32 cfg_n4_rxg_width	:2;
	uint32 cfg_n4_rxg_rate	:2;
	uint32 cfg_n4_rxg_data_loop	:1;
	uint32 cfg_n4_rxg_cdr_mode	:1;
	uint32 cfg_n4_rxg_bypass	:1;
	uint32 cfg_n4_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n4_rxh_prbs_en	:1;
	uint32 cfg_n4_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n4_rxh_prbs_frc_err	:1;
	uint32 cfg_n4_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n4_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n4_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n4_rxh_data_sync	:1;
	uint32 cfg_n4_rxh_sig_det_en	:1;
	uint32 cfg_n4_rxh_sig_lev	:5;
	uint32 cfg_n4_rxh_width	:2;
	uint32 cfg_n4_rxh_rate	:2;
	uint32 cfg_n4_rxh_data_loop	:1;
	uint32 cfg_n4_rxh_cdr_mode	:1;
	uint32 cfg_n4_rxh_bypass	:1;
	uint32 cfg_n4_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n4_txa_prbs_en	:1;
	uint32 cfg_n4_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n4_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n4_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n4_txa_drv_ampl	:3;
	uint32 cfg_n4_txa_drv_slew	:4;
	uint32 cfg_n4_txa_coef	:4;
	uint32 cfg_n4_txa_width	:2;
	uint32 cfg_n4_txa_rate	:2;
	uint32 cfg_n4_txa_byp_data	:1;
	uint32 cfg_n4_txa_ts	:1;
	uint32 cfg_n4_txa_bypass	:1;
	uint32 cfg_n4_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n4_txb_prbs_en	:1;
	uint32 cfg_n4_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n4_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n4_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n4_txb_drv_ampl	:3;
	uint32 cfg_n4_txb_drv_slew	:4;
	uint32 cfg_n4_txb_coef	:4;
	uint32 cfg_n4_txb_width	:2;
	uint32 cfg_n4_txb_rate	:2;
	uint32 cfg_n4_txb_byp_data	:1;
	uint32 cfg_n4_txb_ts	:1;
	uint32 cfg_n4_txb_bypass	:1;
	uint32 cfg_n4_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n4_txc_prbs_en	:1;
	uint32 cfg_n4_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n4_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n4_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n4_txc_drv_ampl	:3;
	uint32 cfg_n4_txc_drv_slew	:4;
	uint32 cfg_n4_txc_coef	:4;
	uint32 cfg_n4_txc_width	:2;
	uint32 cfg_n4_txc_rate	:2;
	uint32 cfg_n4_txc_byp_data	:1;
	uint32 cfg_n4_txc_ts	:1;
	uint32 cfg_n4_txc_bypass	:1;
	uint32 cfg_n4_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n4_txd_prbs_en	:1;
	uint32 cfg_n4_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n4_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n4_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n4_txd_drv_ampl	:3;
	uint32 cfg_n4_txd_drv_slew	:4;
	uint32 cfg_n4_txd_coef	:4;
	uint32 cfg_n4_txd_width	:2;
	uint32 cfg_n4_txd_rate	:2;
	uint32 cfg_n4_txd_byp_data	:1;
	uint32 cfg_n4_txd_ts	:1;
	uint32 cfg_n4_txd_bypass	:1;
	uint32 cfg_n4_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n4_txe_prbs_en	:1;
	uint32 cfg_n4_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n4_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n4_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n4_txe_drv_ampl	:3;
	uint32 cfg_n4_txe_drv_slew	:4;
	uint32 cfg_n4_txe_coef	:4;
	uint32 cfg_n4_txe_width	:2;
	uint32 cfg_n4_txe_rate	:2;
	uint32 cfg_n4_txe_byp_data	:1;
	uint32 cfg_n4_txe_ts	:1;
	uint32 cfg_n4_txe_bypass	:1;
	uint32 cfg_n4_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n4_txf_prbs_en	:1;
	uint32 cfg_n4_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n4_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n4_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n4_txf_drv_ampl	:3;
	uint32 cfg_n4_txf_drv_slew	:4;
	uint32 cfg_n4_txf_coef	:4;
	uint32 cfg_n4_txf_width	:2;
	uint32 cfg_n4_txf_rate	:2;
	uint32 cfg_n4_txf_byp_data	:1;
	uint32 cfg_n4_txf_ts	:1;
	uint32 cfg_n4_txf_bypass	:1;
	uint32 cfg_n4_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n4_txg_prbs_en	:1;
	uint32 cfg_n4_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n4_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n4_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n4_txg_drv_ampl	:3;
	uint32 cfg_n4_txg_drv_slew	:4;
	uint32 cfg_n4_txg_coef	:4;
	uint32 cfg_n4_txg_width	:2;
	uint32 cfg_n4_txg_rate	:2;
	uint32 cfg_n4_txg_byp_data	:1;
	uint32 cfg_n4_txg_ts	:1;
	uint32 cfg_n4_txg_bypass	:1;
	uint32 cfg_n4_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n4_txh_prbs_en	:1;
	uint32 cfg_n4_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n4_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n4_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n4_txh_drv_ampl	:3;
	uint32 cfg_n4_txh_drv_slew	:4;
	uint32 cfg_n4_txh_coef	:4;
	uint32 cfg_n4_txh_width	:2;
	uint32 cfg_n4_txh_rate	:2;
	uint32 cfg_n4_txh_byp_data	:1;
	uint32 cfg_n4_txh_ts	:1;
	uint32 cfg_n4_txh_bypass	:1;
	uint32 cfg_n4_txh_pwr_dwn	:1;
};
typedef struct hss_n4_ctl_s hss_n4_ctl_t;

struct hss_n5_ctl_s	/* 367 */
{

	uint32 rsv_0	:31;
	uint32 cfg_n5_hss_reset	:1;

	uint32 rsv_1	:3;
	uint32 cfg_n5_hss_pdwn_pll	:1;
	uint32 rsv_2	:3;
	uint32 cfg_n5_hss_pll_byp	:1;
	uint32 rsv_3	:3;
	uint32 cfg_n5_hss_prbs_en	:1;
	uint32 rsv_4	:3;
	uint32 cfg_n5_hss_resync_clk_in	:1;
	uint32 rsv_5	:3;
	uint32 cfg_n5_hss_rec_cal	:1;
	uint32 rsv_6	:3;
	uint32 cfg_n5_rg	:1;
	uint32 rsv_7	:3;
	uint32 cfg_n5_hss_ref_clk_gt400	:1;
	uint32 rsv_8	:2;
	uint32 cfg_n5_hss_div_sel	:2;

	uint32 rsv_9	:7;
	uint32 cfg_n5_hss_eye_enable	:1;
	uint32 rsv_10	:3;
	uint32 cfg_n5_hss_eye_reset	:1;
	uint32 rsv_11	:2;
	uint32 cfg_n5_hss_eye_pr_up	:1;
	uint32 cfg_n5_hss_eye_pr_dn	:1;
	uint32 rsv_12	:5;
	uint32 cfg_n5_hss_eye_pr_center	:1;
	uint32 cfg_n5_hss_eye_pr_bump32	:1;
	uint32 cfg_n5_hss_eye_pat_sel	:1;
	uint32 rsv_13	:1;
	uint32 cfg_n5_hss_eye_mode_sel	:3;
	uint32 cfg_n5_hss_eye_link_sel	:4;

	uint32 rsv_14	:2;
	uint32 cfg_n5_rxa_prbs_en	:1;
	uint32 cfg_n5_rxa_prbs_rst	:1;
	uint32 rsv_15	:2;
	uint32 cfg_n5_rxa_prbs_frc_err	:1;
	uint32 cfg_n5_rxa_prbs_wrap	:1;
	uint32 rsv_16	:1;
	uint32 cfg_n5_rxa_prbs	:3;
	uint32 rsv_17	:2;
	uint32 cfg_n5_rxa_eq	:2;
	uint32 cfg_n5_rxa_qrt_clk_en	:1;
	uint32 cfg_n5_rxa_data_sync	:1;
	uint32 cfg_n5_rxa_sig_det_en	:1;
	uint32 cfg_n5_rxa_sig_lev	:5;
	uint32 cfg_n5_rxa_width	:2;
	uint32 cfg_n5_rxa_rate	:2;
	uint32 cfg_n5_rxa_data_loop	:1;
	uint32 cfg_n5_rxa_cdr_mode	:1;
	uint32 cfg_n5_rxa_bypass	:1;
	uint32 cfg_n5_rxa_pwr_dwn	:1;

	uint32 rsv_18	:2;
	uint32 cfg_n5_rxb_prbs_en	:1;
	uint32 cfg_n5_rxb_prbs_rst	:1;
	uint32 rsv_19	:2;
	uint32 cfg_n5_rxb_prbs_frc_err	:1;
	uint32 cfg_n5_rxb_prbs_wrap	:1;
	uint32 rsv_20	:1;
	uint32 cfg_n5_rxb_prbs	:3;
	uint32 rsv_21	:2;
	uint32 cfg_n5_rxb_eq	:2;
	uint32 cfg_n5_rxb_qrt_clk_en	:1;
	uint32 cfg_n5_rxb_data_sync	:1;
	uint32 cfg_n5_rxb_sig_det_en	:1;
	uint32 cfg_n5_rxb_sig_lev	:5;
	uint32 cfg_n5_rxb_width	:2;
	uint32 cfg_n5_rxb_rate	:2;
	uint32 cfg_n5_rxb_data_loop	:1;
	uint32 cfg_n5_rxb_cdr_mode	:1;
	uint32 cfg_n5_rxb_bypass	:1;
	uint32 cfg_n5_rxb_pwr_dwn	:1;

	uint32 rsv_22	:2;
	uint32 cfg_n5_rxc_prbs_en	:1;
	uint32 cfg_n5_rxc_prbs_rst	:1;
	uint32 rsv_23	:2;
	uint32 cfg_n5_rxc_prbs_frc_err	:1;
	uint32 cfg_n5_rxc_prbs_wrap	:1;
	uint32 rsv_24	:1;
	uint32 cfg_n5_rxc_prbs	:3;
	uint32 rsv_25	:2;
	uint32 cfg_n5_rxc_eq	:2;
	uint32 cfg_n5_rxc_qrt_clk_en	:1;
	uint32 cfg_n5_rxc_data_sync	:1;
	uint32 cfg_n5_rxc_sig_det_en	:1;
	uint32 cfg_n5_rxc_sig_lev	:5;
	uint32 cfg_n5_rxc_width	:2;
	uint32 cfg_n5_rxc_rate	:2;
	uint32 cfg_n5_rxc_data_loop	:1;
	uint32 cfg_n5_rxc_cdr_mode	:1;
	uint32 cfg_n5_rxc_bypass	:1;
	uint32 cfg_n5_rxc_pwr_dwn	:1;

	uint32 rsv_26	:2;
	uint32 cfg_n5_rxd_prbs_en	:1;
	uint32 cfg_n5_rxd_prbs_rst	:1;
	uint32 rsv_27	:2;
	uint32 cfg_n5_rxd_prbs_frc_err	:1;
	uint32 cfg_n5_rxd_prbs_wrap	:1;
	uint32 rsv_28	:1;
	uint32 cfg_n5_rxd_prbs	:3;
	uint32 rsv_29	:2;
	uint32 cfg_n5_rxd_eq	:2;
	uint32 cfg_n5_rxd_qrt_clk_en	:1;
	uint32 cfg_n5_rxd_data_sync	:1;
	uint32 cfg_n5_rxd_sig_det_en	:1;
	uint32 cfg_n5_rxd_sig_lev	:5;
	uint32 cfg_n5_rxd_width	:2;
	uint32 cfg_n5_rxd_rate	:2;
	uint32 cfg_n5_rxd_data_loop	:1;
	uint32 cfg_n5_rxd_cdr_mode	:1;
	uint32 cfg_n5_rxd_bypass	:1;
	uint32 cfg_n5_rxd_pwr_dwn	:1;

	uint32 rsv_30	:2;
	uint32 cfg_n5_rxe_prbs_en	:1;
	uint32 cfg_n5_rxe_prbs_rst	:1;
	uint32 rsv_31	:2;
	uint32 cfg_n5_rxe_prbs_frc_err	:1;
	uint32 cfg_n5_rxe_prbs_wrap	:1;
	uint32 rsv_32	:1;
	uint32 cfg_n5_rxe_prbs	:3;
	uint32 rsv_33	:2;
	uint32 cfg_n5_rxe_eq	:2;
	uint32 rsv_34	:1;
	uint32 cfg_n5_rxe_data_sync	:1;
	uint32 cfg_n5_rxe_sig_det_en	:1;
	uint32 cfg_n5_rxe_sig_lev	:5;
	uint32 cfg_n5_rxe_width	:2;
	uint32 cfg_n5_rxe_rate	:2;
	uint32 cfg_n5_rxe_data_loop	:1;
	uint32 cfg_n5_rxe_cdr_mode	:1;
	uint32 cfg_n5_rxe_bypass	:1;
	uint32 cfg_n5_rxe_pwr_dwn	:1;

	uint32 rsv_35	:2;
	uint32 cfg_n5_rxf_prbs_en	:1;
	uint32 cfg_n5_rxf_prbs_rst	:1;
	uint32 rsv_36	:2;
	uint32 cfg_n5_rxf_prbs_frc_err	:1;
	uint32 cfg_n5_rxf_prbs_wrap	:1;
	uint32 rsv_37	:1;
	uint32 cfg_n5_rxf_prbs	:3;
	uint32 rsv_38	:2;
	uint32 cfg_n5_rxf_eq	:2;
	uint32 rsv_39	:1;
	uint32 cfg_n5_rxf_data_sync	:1;
	uint32 cfg_n5_rxf_sig_det_en	:1;
	uint32 cfg_n5_rxf_sig_lev	:5;
	uint32 cfg_n5_rxf_width	:2;
	uint32 cfg_n5_rxf_rate	:2;
	uint32 cfg_n5_rxf_data_loop	:1;
	uint32 cfg_n5_rxf_cdr_mode	:1;
	uint32 cfg_n5_rxf_bypass	:1;
	uint32 cfg_n5_rxf_pwr_dwn	:1;

	uint32 rsv_40	:2;
	uint32 cfg_n5_rxg_prbs_en	:1;
	uint32 cfg_n5_rxg_prbs_rst	:1;
	uint32 rsv_41	:2;
	uint32 cfg_n5_rxg_prbs_frc_err	:1;
	uint32 cfg_n5_rxg_prbs_wrap	:1;
	uint32 rsv_42	:1;
	uint32 cfg_n5_rxg_prbs	:3;
	uint32 rsv_43	:2;
	uint32 cfg_n5_rxg_eq	:2;
	uint32 rsv_44	:1;
	uint32 cfg_n5_rxg_data_sync	:1;
	uint32 cfg_n5_rxg_sig_det_en	:1;
	uint32 cfg_n5_rxg_sig_lev	:5;
	uint32 cfg_n5_rxg_width	:2;
	uint32 cfg_n5_rxg_rate	:2;
	uint32 cfg_n5_rxg_data_loop	:1;
	uint32 cfg_n5_rxg_cdr_mode	:1;
	uint32 cfg_n5_rxg_bypass	:1;
	uint32 cfg_n5_rxg_pwr_dwn	:1;

	uint32 rsv_45	:2;
	uint32 cfg_n5_rxh_prbs_en	:1;
	uint32 cfg_n5_rxh_prbs_rst	:1;
	uint32 rsv_46	:2;
	uint32 cfg_n5_rxh_prbs_frc_err	:1;
	uint32 cfg_n5_rxh_prbs_wrap	:1;
	uint32 rsv_47	:1;
	uint32 cfg_n5_rxh_prbs	:3;
	uint32 rsv_48	:2;
	uint32 cfg_n5_rxh_eq	:2;
	uint32 rsv_49	:1;
	uint32 cfg_n5_rxh_data_sync	:1;
	uint32 cfg_n5_rxh_sig_det_en	:1;
	uint32 cfg_n5_rxh_sig_lev	:5;
	uint32 cfg_n5_rxh_width	:2;
	uint32 cfg_n5_rxh_rate	:2;
	uint32 cfg_n5_rxh_data_loop	:1;
	uint32 cfg_n5_rxh_cdr_mode	:1;
	uint32 cfg_n5_rxh_bypass	:1;
	uint32 cfg_n5_rxh_pwr_dwn	:1;

	uint32 rsv_50	:2;
	uint32 cfg_n5_txa_prbs_en	:1;
	uint32 cfg_n5_txa_prbs_rst	:1;
	uint32 rsv_51	:3;
	uint32 cfg_n5_txa_prbs_frc_err	:1;
	uint32 rsv_52	:1;
	uint32 cfg_n5_txa_prbs	:3;
	uint32 rsv_53	:1;
	uint32 cfg_n5_txa_drv_ampl	:3;
	uint32 cfg_n5_txa_drv_slew	:4;
	uint32 cfg_n5_txa_coef	:4;
	uint32 cfg_n5_txa_width	:2;
	uint32 cfg_n5_txa_rate	:2;
	uint32 cfg_n5_txa_byp_data	:1;
	uint32 cfg_n5_txa_ts	:1;
	uint32 cfg_n5_txa_bypass	:1;
	uint32 cfg_n5_txa_pwr_dwn	:1;

	uint32 rsv_54	:2;
	uint32 cfg_n5_txb_prbs_en	:1;
	uint32 cfg_n5_txb_prbs_rst	:1;
	uint32 rsv_55	:3;
	uint32 cfg_n5_txb_prbs_frc_err	:1;
	uint32 rsv_56	:1;
	uint32 cfg_n5_txb_prbs	:3;
	uint32 rsv_57	:1;
	uint32 cfg_n5_txb_drv_ampl	:3;
	uint32 cfg_n5_txb_drv_slew	:4;
	uint32 cfg_n5_txb_coef	:4;
	uint32 cfg_n5_txb_width	:2;
	uint32 cfg_n5_txb_rate	:2;
	uint32 cfg_n5_txb_byp_data	:1;
	uint32 cfg_n5_txb_ts	:1;
	uint32 cfg_n5_txb_bypass	:1;
	uint32 cfg_n5_txb_pwr_dwn	:1;

	uint32 rsv_58	:2;
	uint32 cfg_n5_txc_prbs_en	:1;
	uint32 cfg_n5_txc_prbs_rst	:1;
	uint32 rsv_59	:3;
	uint32 cfg_n5_txc_prbs_frc_err	:1;
	uint32 rsv_60	:1;
	uint32 cfg_n5_txc_prbs	:3;
	uint32 rsv_61	:1;
	uint32 cfg_n5_txc_drv_ampl	:3;
	uint32 cfg_n5_txc_drv_slew	:4;
	uint32 cfg_n5_txc_coef	:4;
	uint32 cfg_n5_txc_width	:2;
	uint32 cfg_n5_txc_rate	:2;
	uint32 cfg_n5_txc_byp_data	:1;
	uint32 cfg_n5_txc_ts	:1;
	uint32 cfg_n5_txc_bypass	:1;
	uint32 cfg_n5_txc_pwr_dwn	:1;

	uint32 rsv_62	:2;
	uint32 cfg_n5_txd_prbs_en	:1;
	uint32 cfg_n5_txd_prbs_rst	:1;
	uint32 rsv_63	:3;
	uint32 cfg_n5_txd_prbs_frc_err	:1;
	uint32 rsv_64	:1;
	uint32 cfg_n5_txd_prbs	:3;
	uint32 rsv_65	:1;
	uint32 cfg_n5_txd_drv_ampl	:3;
	uint32 cfg_n5_txd_drv_slew	:4;
	uint32 cfg_n5_txd_coef	:4;
	uint32 cfg_n5_txd_width	:2;
	uint32 cfg_n5_txd_rate	:2;
	uint32 cfg_n5_txd_byp_data	:1;
	uint32 cfg_n5_txd_ts	:1;
	uint32 cfg_n5_txd_bypass	:1;
	uint32 cfg_n5_txd_pwr_dwn	:1;

	uint32 rsv_66	:2;
	uint32 cfg_n5_txe_prbs_en	:1;
	uint32 cfg_n5_txe_prbs_rst	:1;
	uint32 rsv_67	:3;
	uint32 cfg_n5_txe_prbs_frc_err	:1;
	uint32 rsv_68	:1;
	uint32 cfg_n5_txe_prbs	:3;
	uint32 rsv_69	:1;
	uint32 cfg_n5_txe_drv_ampl	:3;
	uint32 cfg_n5_txe_drv_slew	:4;
	uint32 cfg_n5_txe_coef	:4;
	uint32 cfg_n5_txe_width	:2;
	uint32 cfg_n5_txe_rate	:2;
	uint32 cfg_n5_txe_byp_data	:1;
	uint32 cfg_n5_txe_ts	:1;
	uint32 cfg_n5_txe_bypass	:1;
	uint32 cfg_n5_txe_pwr_dwn	:1;

	uint32 rsv_70	:2;
	uint32 cfg_n5_txf_prbs_en	:1;
	uint32 cfg_n5_txf_prbs_rst	:1;
	uint32 rsv_71	:3;
	uint32 cfg_n5_txf_prbs_frc_err	:1;
	uint32 rsv_72	:1;
	uint32 cfg_n5_txf_prbs	:3;
	uint32 rsv_73	:1;
	uint32 cfg_n5_txf_drv_ampl	:3;
	uint32 cfg_n5_txf_drv_slew	:4;
	uint32 cfg_n5_txf_coef	:4;
	uint32 cfg_n5_txf_width	:2;
	uint32 cfg_n5_txf_rate	:2;
	uint32 cfg_n5_txf_byp_data	:1;
	uint32 cfg_n5_txf_ts	:1;
	uint32 cfg_n5_txf_bypass	:1;
	uint32 cfg_n5_txf_pwr_dwn	:1;

	uint32 rsv_74	:2;
	uint32 cfg_n5_txg_prbs_en	:1;
	uint32 cfg_n5_txg_prbs_rst	:1;
	uint32 rsv_75	:3;
	uint32 cfg_n5_txg_prbs_frc_err	:1;
	uint32 rsv_76	:1;
	uint32 cfg_n5_txg_prbs	:3;
	uint32 rsv_77	:1;
	uint32 cfg_n5_txg_drv_ampl	:3;
	uint32 cfg_n5_txg_drv_slew	:4;
	uint32 cfg_n5_txg_coef	:4;
	uint32 cfg_n5_txg_width	:2;
	uint32 cfg_n5_txg_rate	:2;
	uint32 cfg_n5_txg_byp_data	:1;
	uint32 cfg_n5_txg_ts	:1;
	uint32 cfg_n5_txg_bypass	:1;
	uint32 cfg_n5_txg_pwr_dwn	:1;

	uint32 rsv_78	:2;
	uint32 cfg_n5_txh_prbs_en	:1;
	uint32 cfg_n5_txh_prbs_rst	:1;
	uint32 rsv_79	:3;
	uint32 cfg_n5_txh_prbs_frc_err	:1;
	uint32 rsv_80	:1;
	uint32 cfg_n5_txh_prbs	:3;
	uint32 rsv_81	:1;
	uint32 cfg_n5_txh_drv_ampl	:3;
	uint32 cfg_n5_txh_drv_slew	:4;
	uint32 cfg_n5_txh_coef	:4;
	uint32 cfg_n5_txh_width	:2;
	uint32 cfg_n5_txh_rate	:2;
	uint32 cfg_n5_txh_byp_data	:1;
	uint32 cfg_n5_txh_ts	:1;
	uint32 cfg_n5_txh_bypass	:1;
	uint32 cfg_n5_txh_pwr_dwn	:1;
};
typedef struct hss_n5_ctl_s hss_n5_ctl_t;

struct qdr_mpmi_ctl_s	/* 368 */
{

	uint32 rsv_0	:31;
	uint32 reset_core_qdr_macro_rx	:1;

	uint32 rsv_1	:2;
	uint32 cfg_qdr_clock_macro_mode	:6;
	uint32 rsv_2	:2;
	uint32 cfg_qdr_valid_macro_mode	:6;
	uint32 rsv_3	:2;
	uint32 cfg_qdr_data_macro_mode	:6;
	uint32 cfg_qdr_mpmi_fifo_bypass	:1;
	uint32 cfg_qdr_mpmi_rd_ptr_init	:3;
	uint32 rsv_4	:3;
	uint32 reset_core_qdr_macro_tx	:1;
};
typedef struct qdr_mpmi_ctl_s qdr_mpmi_ctl_t;

struct ddr_mpmi_ctl_s	/* 369 */
{

	uint32 rsv_0	:31;
	uint32 reset_core_ext_ddr_macro_rx	:1;

	uint32 cfg_ddr_cmd_macro_mode	:1;
	uint32 rsv_1	:1;
	uint32 cfg_ddr_clock_macro_mode	:6;
	uint32 rsv_2	:2;
	uint32 cfg_ddr_valid_macro_mode	:6;
	uint32 rsv_3	:2;
	uint32 cfg_ddr_data_macro_mode	:6;
	uint32 cfg_ddr_mpmi_fifo_bypass	:1;
	uint32 cfg_ddr_mpmi_rd_ptr_init	:3;
	uint32 rsv_4	:3;
	uint32 reset_core_ext_ddr_macro_tx	:1;
};
typedef struct ddr_mpmi_ctl_s ddr_mpmi_ctl_t;

struct tcam_mpmi_ctl_s	/* 370 */
{

	uint32 rsv_0	:31;
	uint32 reset_core_ext_tcam_macro_rx	:1;

	uint32 rsv_1	:2;
	uint32 cfg_tcam_macro_ext_clock_mode	:6;
	uint32 rsv_2	:2;
	uint32 cfg_tcam_macro_ext_data_mode	:6;
	uint32 rsv_3	:7;
	uint32 cfg_tcam_macro_ext_edge_align	:1;
	uint32 cfg_tcam_mpmi_fifo_bypass	:1;
	uint32 cfg_tcam_mpmi_rd_ptr_init	:3;
	uint32 rsv_4	:3;
	uint32 reset_core_ext_tcam_macro_tx	:1;

	uint32 rsv_5	:26;
	uint32 cfg_tcam_macro_ext_sys_clock_mode	:6;
};
typedef struct tcam_mpmi_ctl_s tcam_mpmi_ctl_t;

struct mac_led_ctl_s	/* 371 */
{

	uint32 rsv_0	:15;
	uint32 led_timer_enable0	:1;
	uint32 led_timer0	:16;

	uint32 rsv_1	:15;
	uint32 led_timer_enable1	:1;
	uint32 led_timer1	:16;

	uint32 rsv_2	:15;
	uint32 led_timer_enable2	:1;
	uint32 led_timer2	:16;

	uint32 rsv_3	:15;
	uint32 led_timer_enable3	:1;
	uint32 led_timer3	:16;
};
typedef struct mac_led_ctl_s mac_led_ctl_t;

struct xgmac_select_ctl_s	/* 372 */
{

	uint32 rsv_0	:28;
	uint32 cfg_select_xgmac3	:1;
	uint32 cfg_select_xgmac2	:1;
	uint32 cfg_select_xgmac1	:1;
	uint32 cfg_select_xgmac0	:1;

	uint32 rsv_1	:28;
	uint32 reset_xgmac_phase3	:1;
	uint32 reset_xgmac_phase2	:1;
	uint32 reset_xgmac_phase1	:1;
	uint32 reset_xgmac_phase0	:1;
};
typedef struct xgmac_select_ctl_s xgmac_select_ctl_t;

struct fabric_select_ctl_s	/* 373 */
{

	uint32 rsv_0	:28;
	uint32 cfg_select_fabric3	:1;
	uint32 cfg_select_fabric2	:1;
	uint32 cfg_select_fabric1	:1;
	uint32 cfg_select_fabric0	:1;

	uint32 rsv_1	:28;
	uint32 reset_sgmac_phase3	:1;
	uint32 reset_sgmac_phase2	:1;
	uint32 reset_sgmac_phase1	:1;
	uint32 reset_sgmac_phase0	:1;
};
typedef struct fabric_select_ctl_s fabric_select_ctl_t;

struct misc_mac_clk_ctl_s	/* 374 */
{

	uint32 rsv_0	:11;
	uint32 cfg_reset_misc_mac_clocks	:1;
	uint32 rsv_1	:3;
	uint32 cfg_cpu_mac_clk_internal	:1;
	uint32 rsv_2	:2;
	uint32 cfg_cpu_mac_clock_divider	:2;
	uint32 rsv_3	:2;
	uint32 cfg_ge_ref_out_ctl	:2;
	uint32 cfg_mdio_clk_divider	:8;
};
typedef struct misc_mac_clk_ctl_s misc_mac_clk_ctl_t;

struct enable_ram1x_clk_ctl_s	/* 375 */
{

	uint32 rsv_0	:29;
	uint32 cfg_reset_tcam1x_clk	:1;
	uint32 cfg_reset_qdr1x_clk	:1;
	uint32 cfg_reset_ddr1x_clk	:1;
};
typedef struct enable_ram1x_clk_ctl_s enable_ram1x_clk_ctl_t;

struct hash_key_select_s	/* 376 */
{

	uint32 rsv_0	:31;
	uint32 cfg_hash_key_select98k	:1;
};
typedef struct hash_key_select_s hash_key_select_t;

struct mdio_in_select_s	/* 377 */
{

	uint32 rsv_0	:31;
	uint32 cfg_mdio_select_mdc	:1;
};
typedef struct mdio_in_select_s mdio_in_select_t;

struct ddr_dl_ctl_s	/* 378 */
{

	uint32 rsv_0	:3;
	uint32 cfg_ddr_macro_dl_tune	:5;
	uint32 rsv_1	:3;
	uint32 cfg_ddr_macro_ph290	:1;
	uint32 rsv_2	:3;
	uint32 cfg_ddr_macro_ph190	:1;
	uint32 rsv_3	:1;
	uint32 cfg_ddr_macro_ph2	:7;
	uint32 rsv_4	:1;
	uint32 cfg_ddr_macro_ph1	:7;
};
typedef struct ddr_dl_ctl_s ddr_dl_ctl_t;

struct qdr_dl_ctl_s	/* 379 */
{

	uint32 rsv_0	:3;
	uint32 cfg_qdr_macro_dl_tune	:5;
	uint32 rsv_1	:3;
	uint32 cfg_qdr_macro_ph290	:1;
	uint32 rsv_2	:3;
	uint32 cfg_qdr_macro_ph190	:1;
	uint32 rsv_3	:1;
	uint32 cfg_qdr_macro_ph2	:7;
	uint32 rsv_4	:1;
	uint32 cfg_qdr_macro_ph1	:7;
};
typedef struct qdr_dl_ctl_s qdr_dl_ctl_t;

struct dl_mon_s	/* 380 */
{

	uint32 rsv_0	:23;
	uint32 mon_qdr_macro_dl_lock	:1;
	uint32 rsv_1	:7;
	uint32 mon_ddr_macro_dl_lock	:1;
};
typedef struct dl_mon_s dl_mon_t;

struct sgmac_use4g_core_ctl_s	/* 381 */
{

	uint32 rsv_0	:15;
	uint32 cfg_sgmac_use4g_hss	:1;
	uint32 rsv_1	:4;
	uint32 reset_sgmac4g_phase_tx3	:1;
	uint32 reset_sgmac4g_phase_tx2	:1;
	uint32 reset_sgmac4g_phase_tx1	:1;
	uint32 reset_sgmac4g_phase_tx0	:1;
	uint32 rsv_2	:4;
	uint32 reset_sgmac4g_phase_rx3	:1;
	uint32 reset_sgmac4g_phase_rx2	:1;
	uint32 reset_sgmac4g_phase_rx1	:1;
	uint32 reset_sgmac4g_phase_rx0	:1;
};
typedef struct sgmac_use4g_core_ctl_s sgmac_use4g_core_ctl_t;

struct global_gated_clk_ctl_s	/* 382 */
{

	uint32 rsv_0	:23;
	uint32 xgmac0_reg_clk_en	:1;
	uint32 rsv_1	:7;
	uint32 global_en_clk	:1;
};
typedef struct global_gated_clk_ctl_s global_gated_clk_ctl_t;

struct module_gated_clk_ctl_s	/* 383 */
{

	uint32 en_clk_sup_gmac_wrapper31	:1;
	uint32 en_clk_sup_gmac_wrapper30	:1;
	uint32 en_clk_sup_gmac_wrapper29	:1;
	uint32 en_clk_sup_gmac_wrapper28	:1;
	uint32 en_clk_sup_gmac_wrapper27	:1;
	uint32 en_clk_sup_gmac_wrapper26	:1;
	uint32 en_clk_sup_gmac_wrapper25	:1;
	uint32 en_clk_sup_gmac_wrapper24	:1;
	uint32 en_clk_sup_gmac_wrapper23	:1;
	uint32 en_clk_sup_gmac_wrapper22	:1;
	uint32 en_clk_sup_gmac_wrapper21	:1;
	uint32 en_clk_sup_gmac_wrapper20	:1;
	uint32 en_clk_sup_gmac_wrapper19	:1;
	uint32 en_clk_sup_gmac_wrapper18	:1;
	uint32 en_clk_sup_gmac_wrapper17	:1;
	uint32 en_clk_sup_gmac_wrapper16	:1;
	uint32 en_clk_sup_gmac_wrapper15	:1;
	uint32 en_clk_sup_gmac_wrapper14	:1;
	uint32 en_clk_sup_gmac_wrapper13	:1;
	uint32 en_clk_sup_gmac_wrapper12	:1;
	uint32 en_clk_sup_gmac_wrapper11	:1;
	uint32 en_clk_sup_gmac_wrapper10	:1;
	uint32 en_clk_sup_gmac_wrapper9	:1;
	uint32 en_clk_sup_gmac_wrapper8	:1;
	uint32 en_clk_sup_gmac_wrapper7	:1;
	uint32 en_clk_sup_gmac_wrapper6	:1;
	uint32 en_clk_sup_gmac_wrapper5	:1;
	uint32 en_clk_sup_gmac_wrapper4	:1;
	uint32 en_clk_sup_gmac_wrapper3	:1;
	uint32 en_clk_sup_gmac_wrapper2	:1;
	uint32 en_clk_sup_gmac_wrapper1	:1;
	uint32 en_clk_sup_gmac_wrapper0	:1;

	uint32 en_clk_sup_sgmac3	:1;
	uint32 en_clk_sup_sgmac2	:1;
	uint32 en_clk_sup_sgmac1	:1;
	uint32 en_clk_sup_sgmac0	:1;
	uint32 en_clk_sup_quad_mac_app11	:1;
	uint32 en_clk_sup_quad_mac_app10	:1;
	uint32 en_clk_sup_quad_mac_app9	:1;
	uint32 en_clk_sup_quad_mac_app8	:1;
	uint32 en_clk_sup_quad_mac_app7	:1;
	uint32 en_clk_sup_quad_mac_app6	:1;
	uint32 en_clk_sup_quad_mac_app5	:1;
	uint32 en_clk_sup_quad_mac_app4	:1;
	uint32 en_clk_sup_quad_mac_app3	:1;
	uint32 en_clk_sup_quad_mac_app2	:1;
	uint32 en_clk_sup_quad_mac_app1	:1;
	uint32 en_clk_sup_quad_mac_app0	:1;
	uint32 en_clk_sup_gmac_wrapper47	:1;
	uint32 en_clk_sup_gmac_wrapper46	:1;
	uint32 en_clk_sup_gmac_wrapper45	:1;
	uint32 en_clk_sup_gmac_wrapper44	:1;
	uint32 en_clk_sup_gmac_wrapper43	:1;
	uint32 en_clk_sup_gmac_wrapper42	:1;
	uint32 en_clk_sup_gmac_wrapper41	:1;
	uint32 en_clk_sup_gmac_wrapper40	:1;
	uint32 en_clk_sup_gmac_wrapper39	:1;
	uint32 en_clk_sup_gmac_wrapper38	:1;
	uint32 en_clk_sup_gmac_wrapper37	:1;
	uint32 en_clk_sup_gmac_wrapper36	:1;
	uint32 en_clk_sup_gmac_wrapper35	:1;
	uint32 en_clk_sup_gmac_wrapper34	:1;
	uint32 en_clk_sup_gmac_wrapper33	:1;
	uint32 en_clk_sup_gmac_wrapper32	:1;

	uint32 rsv_0	:19;
	uint32 en_clk_sup_fabric_ser	:1;
	uint32 en_clk_sup_fabric_voq	:1;
	uint32 en_clk_sup_fabric_rxq	:1;
	uint32 en_clk_sup_fabric_rts	:1;
	uint32 en_clk_sup_fabric_gts	:1;
	uint32 en_clk_sup_fabric_dsf	:1;
	uint32 en_clk_sup_fabric_crb	:1;
	uint32 en_clk_sup_fabric_cas	:1;
	uint32 en_clk_sup_tcam_macro_int	:1;
	uint32 en_clk_sup_xgmac3	:1;
	uint32 en_clk_sup_xgmac2	:1;
	uint32 en_clk_sup_xgmac1	:1;
	uint32 en_clk_sup_xgmac0	:1;
};
typedef struct module_gated_clk_ctl_s module_gated_clk_ctl_t;

struct zcntl_ctl_s	/* 384 */
{

	uint32 rsv_0	:14;
	uint32 zcntl_reset_bar_out_cal	:1;
	uint32 zcntl_reset_bar_in_cal	:1;
	uint32 rsv_1	:14;
	uint32 zcntl_enable_out_cal	:1;
	uint32 zcntl_enable_in_cal	:1;
};
typedef struct zcntl_ctl_s zcntl_ctl_t;

struct sync_ethernet_cfg0_s	/* 385 */
{

	uint32 rsv_0	:10;
	uint32 cfg_ether_divider0	:6;
	uint32 rsv_1	:5;
	uint32 cfg_ether_test_func0	:3;
	uint32 rsv_2	:2;
	uint32 cfg_ether_user_off0	:1;
	uint32 cfg_ether_user_go0	:1;
	uint32 cfg_ether_invert_clock0	:1;
	uint32 cfg_ether_edram_mode0	:1;
	uint32 cfg_ether_clk_user_lbit0	:1;
	uint32 cfg_ether_bist_start_clk_off0	:1;

	uint32 rsv_3	:31;
	uint32 cfg_ether_reset0	:1;
};
typedef struct sync_ethernet_cfg0_s sync_ethernet_cfg0_t;

struct sync_ethernet_select0_s	/* 386 */
{

	uint32 rsv_0	:16;
	uint32 cfg_ether_clk_select0	:16;
};
typedef struct sync_ethernet_select0_s sync_ethernet_select0_t;

struct sync_ethernet_mon0_s	/* 387 */
{

	uint32 rsv_0	:31;
	uint32 ethernet_clk_off0	:1;
};
typedef struct sync_ethernet_mon0_s sync_ethernet_mon0_t;

struct sync_ethernet_cfg1_s	/* 388 */
{

	uint32 rsv_0	:10;
	uint32 cfg_ether_divider1	:6;
	uint32 rsv_1	:5;
	uint32 cfg_ether_test_func1	:3;
	uint32 rsv_2	:2;
	uint32 cfg_ether_user_off1	:1;
	uint32 cfg_ether_user_go1	:1;
	uint32 cfg_ether_invert_clock1	:1;
	uint32 cfg_ether_edram_mode1	:1;
	uint32 cfg_ether_clk_user_lbit1	:1;
	uint32 cfg_ether_bist_start_clk_off1	:1;

	uint32 rsv_3	:31;
	uint32 cfg_ether_reset1	:1;
};
typedef struct sync_ethernet_cfg1_s sync_ethernet_cfg1_t;

struct sync_ethernet_select1_s	/* 389 */
{

	uint32 rsv_0	:15;
	uint32 cfg_select_sync_ethernet1	:1;
	uint32 cfg_ether_clk_select1	:16;
};
typedef struct sync_ethernet_select1_s sync_ethernet_select1_t;

struct sync_ethernet_mon1_s	/* 390 */
{

	uint32 rsv_0	:31;
	uint32 ethernet_clk_off1	:1;
};
typedef struct sync_ethernet_mon1_s sync_ethernet_mon1_t;

struct time_out_info_s	/* 391 */
{

	uint32 time_out_addr	:32;

	uint32 rsv_0	:31;
	uint32 time_out_action	:1;
};
typedef struct time_out_info_s time_out_info_t;

struct time_out_happen_s	/* 392 */
{

	uint32 rsv_0	:31;
	uint32 time_out_happen	:1;
};
typedef struct time_out_happen_s time_out_happen_t;

struct ipe_aging_fifo_ram_s	/* 408 */
{

	uint32 rsv_0	:18;
	uint32 aging_fifo_ptr	:14;
};
typedef struct ipe_aging_fifo_ram_s ipe_aging_fifo_ram_t;

struct ipe_aging_ctl_s	/* 409 */
{

	uint32 rsv_0	:7;
	uint32 software_read_clear	:1;
	uint32 rsv_1	:3;
	uint32 fifo_depth_threshold	:5;
	uint32 rsv_2	:12;
	uint32 scan_pause_on_fifo_full	:1;
	uint32 stop_on_max_ptr	:1;
	uint32 entry_valid_check_en	:1;
	uint32 scan_en	:1;

	uint32 rsv_3	:18;
	uint32 aging_ptr	:14;

	uint32 aging_interval	:32;

	uint32 rsv_4	:2;
	uint32 max_ptr	:14;
	uint32 rsv_5	:2;
	uint32 min_ptr	:14;
};
typedef struct ipe_aging_ctl_s ipe_aging_ctl_t;

struct ipe_aging_status_s	/* 410 */
{

	uint32 rsv_0	:27;
	uint32 aging_fifo_overflow	:1;
	uint32 rsv_1	:3;
	uint32 update_fifo_overflow	:1;
};
typedef struct ipe_aging_status_s ipe_aging_status_t;

struct ipe_aging_status_mask_s	/* 411 */
{

	uint32 rsv_0	:27;
	uint32 aging_fifo_overflow	:1;
	uint32 rsv_1	:3;
	uint32 update_fifo_overflow	:1;
};
typedef struct ipe_aging_status_mask_s ipe_aging_status_mask_t;

struct ipe_aging_intr_value_set_s	/* 412 */
{

	uint32 rsv_0	:27;
	uint32 ipe_aging_intr_value_set	:5;
};
typedef struct ipe_aging_intr_value_set_s ipe_aging_intr_value_set_t;

struct ipe_aging_intr_value_reset_s	/* 413 */
{

	uint32 rsv_0	:27;
	uint32 ipe_aging_intr_value_reset	:5;
};
typedef struct ipe_aging_intr_value_reset_s ipe_aging_intr_value_reset_t;

struct ipe_aging_intr_mask_set_s	/* 414 */
{

	uint32 rsv_0	:27;
	uint32 ipe_aging_intr_mask_set	:5;
};
typedef struct ipe_aging_intr_mask_set_s ipe_aging_intr_mask_set_t;

struct ipe_aging_intr_mask_reset_s	/* 415 */
{

	uint32 rsv_0	:27;
	uint32 ipe_aging_intr_mask_reset	:5;
};
typedef struct ipe_aging_intr_mask_reset_s ipe_aging_intr_mask_reset_t;

struct ipe_aging_init_s	/* 416 */
{

	uint32 rsv_0	:31;
	uint32 init	:1;
};
typedef struct ipe_aging_init_s ipe_aging_init_t;

struct ipe_aging_init_done_s	/* 417 */
{

	uint32 rsv_0	:31;
	uint32 init	:1;
};
typedef struct ipe_aging_init_done_s ipe_aging_init_done_t;

struct ipeaging_normal_intr_value_set_s	/* 418 */
{

	uint32 rsv_0	:31;
	uint32 intr_value_set	:1;
};
typedef struct ipeaging_normal_intr_value_set_s ipeaging_normal_intr_value_set_t;

struct ipeaging_normal_intr_value_reset_s	/* 419 */
{

	uint32 rsv_0	:31;
	uint32 intr_value_reset	:1;
};
typedef struct ipeaging_normal_intr_value_reset_s ipeaging_normal_intr_value_reset_t;

struct ipeaging_normal_intr_mask_set_s	/* 420 */
{

	uint32 rsv_0	:31;
	uint32 intr_mask_set	:1;
};
typedef struct ipeaging_normal_intr_mask_set_s ipeaging_normal_intr_mask_set_t;

struct ipeaging_normal_intr_mask_reset_s	/* 421 */
{

	uint32 rsv_0	:31;
	uint32 intr_mask_reset	:1;
};
typedef struct ipeaging_normal_intr_mask_reset_s ipeaging_normal_intr_mask_reset_t;

struct ipe_aging_fifo_depth_s	/* 422 */
{

	uint32 rsv_0	:27;
	uint32 ipe_aging_fifo_depth	:5;
};
typedef struct ipe_aging_fifo_depth_s ipe_aging_fifo_depth_t;

struct ipe_forward_drain_enable_s	/* 423 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct ipe_forward_drain_enable_s ipe_forward_drain_enable_t;

struct ipe_forward_credit_value_cfg_s	/* 424 */
{

	uint32 rsv_0	:27;
	uint32 forward_buf_store_credit_value	:5;
};
typedef struct ipe_forward_credit_value_cfg_s ipe_forward_credit_value_cfg_t;

struct ipe_forward_thrd_cfg_s	/* 425 */
{

	uint32 rsv_0	:3;
	uint32 ipe_fwd_tb_info_fifoa_full_thrd	:5;
	uint32 ipe_fwd_sop_data_fifoa_full_thrd	:8;
	uint32 rsv_1	:3;
	uint32 ipe_fwd_pkt_info_fifoa_full_thrd	:5;
	uint32 rsv_2	:3;
	uint32 ipe_fwd_ext_data_fifoa_full_thrd	:5;

	uint32 rsv_3	:22;
	uint32 ipe_fwd_sop_header_fifoa_full_thrd	:2;
	uint32 ipe_fwd_time_stamp_fifoa_full_thrd	:8;
};
typedef struct ipe_forward_thrd_cfg_s ipe_forward_thrd_cfg_t;

struct ipe_forward_stats_s	/* 426 */
{

	uint32 rsv_0	:28;
	uint32 in_pkt_cnt	:4;

	uint32 rsv_1	:28;
	uint32 out_pkt_cnt	:4;
};
typedef struct ipe_forward_stats_s ipe_forward_stats_t;

struct ipe_forward_interrupt_s	/* 427 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;

	uint32 rsv_1	:15;
	uint32 mask_reset	:17;

	uint32 rsv_2	:15;
	uint32 value_set	:17;

	uint32 rsv_3	:15;
	uint32 value_reset	:17;
};
typedef struct ipe_forward_interrupt_s ipe_forward_interrupt_t;

struct ipe_forward_ctl_s	/* 428 */
{

	uint32 ds_fwd_stats_base	:12;
	uint32 ds_fwd_index_base_fatal	:16;
	uint32 multi_exception_en	:1;
	uint32 rsv_0	:1;
	uint32 rx_ether_oam_critical	:1;
	uint32 service_id_en_cfg	:1;

	uint32 discard_fatal	:16;
	uint32 ds_fwd_base0	:10;
	uint32 force_excep_local_phy_port	:1;
	uint32 humber_id	:5;

	uint32 header_hash_bits_num	:3;
	uint32 log_on_discard	:5;
	uint32 vpls_src_port_en	:1;
	uint32 vrf_id_en	:1;
	uint32 flow_id_en	:1;
	uint32 sgmac_header_en	:1;
	uint32 sequence_number_disable_check_en	:1;
	uint32 rsv_1	:1;
	uint32 oam_bypass_policing_discard	:1;
	uint32 log_port_discard	:1;
	uint32 exception2_priority_en	:1;
	uint32 gre_sequence_number_absence_check_en	:1;
	uint32 pw_sequence_number_zero_check_en	:1;
	uint32 sequence_number_exception_en	:1;
	uint32 ds_fwd_discard_stats_en	:1;
	uint32 pw_sequence_number_absence_check_en	:1;
	uint32 ds_fwd_base1	:2;
	uint32 oam_below_link_color	:2;
	uint32 oam_below_link_priority	:6;

	uint32 rsv_2	:16;
	uint32 critical_exception	:16;

	uint32 rsv_3	:2;
	uint32 exception2_priority3	:6;
	uint32 rsv_4	:2;
	uint32 exception2_priority2	:6;
	uint32 rsv_5	:2;
	uint32 exception2_priority1	:6;
	uint32 rsv_6	:2;
	uint32 exception2_priority0	:6;

	uint32 rsv_7	:2;
	uint32 exception2_priority7	:6;
	uint32 rsv_8	:2;
	uint32 exception2_priority6	:6;
	uint32 rsv_9	:2;
	uint32 exception2_priority5	:6;
	uint32 rsv_10	:2;
	uint32 exception2_priority4	:6;

	uint32 rsv_11	:2;
	uint32 exception2_priority11	:6;
	uint32 rsv_12	:2;
	uint32 exception2_priority10	:6;
	uint32 rsv_13	:2;
	uint32 exception2_priority9	:6;
	uint32 rsv_14	:2;
	uint32 exception2_priority8	:6;

	uint32 rsv_15	:2;
	uint32 exception2_priority15	:6;
	uint32 rsv_16	:2;
	uint32 exception2_priority14	:6;
	uint32 rsv_17	:2;
	uint32 exception2_priority13	:6;
	uint32 rsv_18	:2;
	uint32 exception2_priority12	:6;

	uint32 oam_discard_bitmap	:32;
};
typedef struct ipe_forward_ctl_s ipe_forward_ctl_t;

struct ipe_forward_sgmac_ctl_s	/* 429 */
{

	uint32 rsv_0	:8;
	uint32 ucast_non_l3_next_hop_ext	:1;
	uint32 ucast_l3_next_hop_ext	:1;
	uint32 dvp_next_hop_ext	:1;
	uint32 mirror_next_hop_ext	:1;
	uint32 sgmac_lbid_en	:1;
	uint32 sgmac_mcast_hash_en	:1;
	uint32 sgmac_version	:1;
	uint32 default_met_valid	:1;
	uint32 default_met_ptr	:16;

	uint32 l2_met_ptr_base	:16;
	uint32 l3_met_ptr_base	:16;

	uint32 rsv_1	:12;
	uint32 ucast_l3_next_hop_ptr	:20;

	uint32 rsv_2	:12;
	uint32 mirror_next_hop_ptr	:20;

	uint32 rsv_3	:12;
	uint32 ucast_non_l3_next_hop_ptr	:20;

	uint32 rsv_4	:12;
	uint32 dvp_next_hop_base	:20;

	uint32 rsv_5	:16;
	uint32 dvp_met_ptr_base	:16;
};
typedef struct ipe_forward_sgmac_ctl_s ipe_forward_sgmac_ctl_t;

struct ipe_forward_aps_bridge_table_parity_fail_record_s	/* 430 */
{

	uint32 rsv_0	:15;
	uint32 ds_aps_bridge_table_parity_fail	:1;
	uint32 rsv_1	:5;
	uint32 ds_aps_bridge_table_parity_fail_addr	:11;
};
typedef struct ipe_forward_aps_bridge_table_parity_fail_record_s ipe_forward_aps_bridge_table_parity_fail_record_t;

struct ipe_forward_aps_select_table_parity_fail_record_s	/* 431 */
{

	uint32 rsv_0	:15;
	uint32 ds_aps_select_table_parity_fail	:1;
	uint32 rsv_1	:10;
	uint32 ds_aps_select_table_parity_fail_addr	:6;
};
typedef struct ipe_forward_aps_select_table_parity_fail_record_s ipe_forward_aps_select_table_parity_fail_record_t;

struct ipe_forward_fwd_ext_table_parity_fail_record_s	/* 432 */
{

	uint32 rsv_0	:15;
	uint32 ds_fwd_ext_table_parity_fail	:1;
	uint32 rsv_1	:5;
	uint32 ds_fwd_ext_table_parity_fail_addr	:11;
};
typedef struct ipe_forward_fwd_ext_table_parity_fail_record_s ipe_forward_fwd_ext_table_parity_fail_record_t;

struct ipe_forward_sequence_number_table_parity_fail_record_s	/* 433 */
{

	uint32 rsv_0	:15;
	uint32 ds_sequence_number_table_parity_fail	:1;
	uint32 rsv_1	:8;
	uint32 ds_sequence_number_table_parity_fail_addr	:8;
};
typedef struct ipe_forward_sequence_number_table_parity_fail_record_s ipe_forward_sequence_number_table_parity_fail_record_t;

struct ipe_hdr_adj_drain_enable_s	/* 434 */
{

	uint32 rsv_0	:15;
	uint32 drain_enable1	:1;
	uint32 rsv_1	:15;
	uint32 drain_enable0	:1;
};
typedef struct ipe_hdr_adj_drain_enable_s ipe_hdr_adj_drain_enable_t;

struct ipe_hdr_adj_mode_ctl_s	/* 435 */
{

	uint32 rsv_0	:31;
	uint32 mode	:1;
};
typedef struct ipe_hdr_adj_mode_ctl_s ipe_hdr_adj_mode_ctl_t;

struct ipe_hdr_adj_sgmac_ctl_s	/* 436 */
{

	uint32 sgmac_lookup_dst_port	:8;
	uint32 sgmac_lookup_dst_mod	:7;
	uint32 sgmac_lookup_mode	:4;
	uint32 humber_id	:5;
	uint32 sgmac_lookup_en	:4;
	uint32 dual_mod_en	:1;
	uint32 humber_id_check_en	:1;
	uint32 version	:1;
	uint32 bypass_all_cfg	:1;
};
typedef struct ipe_hdr_adj_sgmac_ctl_s ipe_hdr_adj_sgmac_ctl_t;

struct ipe_hdr_adj_ctl_s	/* 437 */
{

	uint32 rsv_0	:25;
	uint32 vpls_src_port_en_cfg	:1;
	uint32 service_id_en_cfg	:1;
	uint32 vrf_id_en_cfg	:1;
	uint32 flow_id_en_cfg	:1;
	uint32 ptp_discard	:1;
	uint32 parser_length_error_mode	:2;
};
typedef struct ipe_hdr_adj_ctl_s ipe_hdr_adj_ctl_t;

struct ipe_hdr_adj_vlan_ptr_s	/* 438 */
{

	uint32 rsv_0	:15;
	uint32 vlan_ptr_valid	:1;
	uint32 rsv_1	:2;
	uint32 vlan_ptr	:14;
};
typedef struct ipe_hdr_adj_vlan_ptr_s ipe_hdr_adj_vlan_ptr_t;

struct ipe_hdr_adj_exp_map_table_s	/* 439 */
{

	uint32 priority7	:6;
	uint32 color7	:2;
	uint32 priority6	:6;
	uint32 color6	:2;
	uint32 priority5	:6;
	uint32 color5	:2;
	uint32 priority4	:6;
	uint32 color4	:2;

	uint32 priority3	:6;
	uint32 color3	:2;
	uint32 priority2	:6;
	uint32 color2	:2;
	uint32 priority1	:6;
	uint32 color1	:2;
	uint32 priority0	:6;
	uint32 color0	:2;
};
typedef struct ipe_hdr_adj_exp_map_table_s ipe_hdr_adj_exp_map_table_t;

struct ipe_hdr_adj_phy_port_mux_ctl_s	/* 440 */
{

	uint32 mux_en31to0	:32;

	uint32 rsv_0	:12;
	uint32 mux_en51to32	:20;
};
typedef struct ipe_hdr_adj_phy_port_mux_ctl_s ipe_hdr_adj_phy_port_mux_ctl_t;

struct ipe_hdr_adj_misc_ctl_s	/* 441 */
{

	uint32 rsv_0	:24;
	uint32 min_pkt_size_for_parser	:8;

	uint32 rsv_1	:13;
	uint32 ipe_intf_mapper_credit_value	:3;
	uint32 rsv_2	:12;
	uint32 buf_store_credit_value	:4;
};
typedef struct ipe_hdr_adj_misc_ctl_s ipe_hdr_adj_misc_ctl_t;

struct ipe_hdr_adj_fifo_thrd_s	/* 442 */
{

	uint32 rsv_0	:2;
	uint32 net_pkt_data_fifoa_full_thrd	:6;
	uint32 rsv_1	:3;
	uint32 net_pkt_info_fifoa_full_thrd	:5;
	uint32 rsv_2	:2;
	uint32 lpbk_pkt_data_fifoa_full_thrd	:6;
	uint32 rsv_3	:3;
	uint32 lpbk_pkt_info_fifoa_full_thrd	:5;

	uint32 rsv_4	:2;
	uint32 buf_track_fifoa_full_thrd	:6;
	uint32 rsv_5	:1;
	uint32 cmpcpd_in_fifoa_full_thrd	:7;
	uint32 rsv_6	:1;
	uint32 cmpcpi_in_fifoa_full_thrd	:7;
	uint32 cmpcpr_in_fifoa_full_thrd	:8;

	uint32 rsv_7	:8;
	uint32 cmpcpd_out_fifoa_full_thrd	:8;
	uint32 rsv_8	:16;

	uint32 rsv_9	:17;
	uint32 addr_track_fifoa_full_thrd	:7;
	uint32 rsv_10	:1;
	uint32 pkt_len_track_fifoa_full_thrd	:7;
};
typedef struct ipe_hdr_adj_fifo_thrd_s ipe_hdr_adj_fifo_thrd_t;

struct ipe_hdr_adj_interrupt_s	/* 443 */
{

	uint32 rsv_0	:7;
	uint32 mask_set	:25;

	uint32 rsv_1	:7;
	uint32 mask_reset	:25;

	uint32 rsv_2	:7;
	uint32 value_set	:25;

	uint32 rsv_3	:7;
	uint32 value_reset	:25;
};
typedef struct ipe_hdr_adj_interrupt_s ipe_hdr_adj_interrupt_t;

struct ipe_hdr_adj_random_seed_load_s	/* 444 */
{

	uint32 rsv_0	:17;
	uint32 rand_seed_value	:15;

	uint32 rsv_1	:31;
	uint32 rand_seed_load	:1;
};
typedef struct ipe_hdr_adj_random_seed_load_s ipe_hdr_adj_random_seed_load_t;

struct ipe_hdr_adj_stats_s	/* 445 */
{

	uint32 rsv_0	:28;
	uint32 fr_net_rx_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 fr_net_rx_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 fr_buf_retrv_sop_cnt	:4;

	uint32 rsv_3	:28;
	uint32 fr_buf_retrv_eop_cnt	:4;

	uint32 rsv_4	:28;
	uint32 to_buf_store_fake_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 to_buf_store_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 to_ipe_fwd_first_word_cnt	:4;

	uint32 rsv_7	:28;
	uint32 net_pkt_len_err_drop_cnt	:4;

	uint32 rsv_8	:28;
	uint32 lpbk_pkt_len_err_drop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 min_len_err_drop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 lpbk_pkt_crc_err_cnt	:4;
};
typedef struct ipe_hdr_adj_stats_s ipe_hdr_adj_stats_t;

struct ipe_hdr_adj_layer4_length_op_ctl_s	/* 446 */
{

	uint32 rsv_0	:18;
	uint32 layer4_op_length1	:14;

	uint32 rsv_1	:18;
	uint32 layer4_op_length2	:14;

	uint32 rsv_2	:18;
	uint32 layer4_op_length3	:14;

	uint32 rsv_3	:18;
	uint32 layer4_op_length4	:14;

	uint32 rsv_4	:18;
	uint32 layer4_op_length5	:14;

	uint32 rsv_5	:18;
	uint32 layer4_op_length6	:14;

	uint32 rsv_6	:18;
	uint32 layer4_op_length7	:14;

	uint32 rsv_7	:18;
	uint32 layer4_op_length8	:14;

	uint32 rsv_8	:18;
	uint32 layer4_op_length9	:14;

	uint32 rsv_9	:18;
	uint32 layer4_op_length10	:14;

	uint32 rsv_10	:18;
	uint32 layer4_op_length11	:14;

	uint32 rsv_11	:18;
	uint32 layer4_op_length12	:14;

	uint32 rsv_12	:18;
	uint32 layer4_op_length13	:14;

	uint32 rsv_13	:18;
	uint32 layer4_op_length14	:14;

	uint32 rsv_14	:18;
	uint32 layer4_op_length15	:14;
};
typedef struct ipe_hdr_adj_layer4_length_op_ctl_s ipe_hdr_adj_layer4_length_op_ctl_t;

struct ipe_hdr_adj_parity_fail_record_s	/* 447 */
{

	uint32 rsv_0	:15;
	uint32 ds_phy_port_parity_fail	:1;
	uint32 rsv_1	:8;
	uint32 ds_phy_port_parity_fail_addr	:8;

	uint32 rsv_2	:15;
	uint32 msg_ram_parity_fail	:1;
	uint32 rsv_3	:7;
	uint32 msg_ram_parity_fail_addr	:9;

	uint32 rsv_4	:15;
	uint32 cmpc_first_word_ram_parity_fail	:1;
	uint32 rsv_5	:10;
	uint32 cmpc_first_word_ram_parity_fail_addr	:6;

	uint32 rsv_6	:15;
	uint32 cmpc_res_word_ram_parity_fail	:1;
	uint32 rsv_7	:10;
	uint32 cmpc_res_word_ram_parity_fail_addr	:6;
};
typedef struct ipe_hdr_adj_parity_fail_record_s ipe_hdr_adj_parity_fail_record_t;

struct ipe_hdr_adj_disable_crc_upd_s	/* 448 */
{

	uint32 rsv_0	:30;
	uint32 disable_crc_chk	:1;
	uint32 disable_crc_upd	:1;
};
typedef struct ipe_hdr_adj_disable_crc_upd_s ipe_hdr_adj_disable_crc_upd_t;

struct ipe_hdr_adj_wrr_weight_s	/* 449 */
{

	uint32 rsv_0	:6;
	uint32 net_wrr_weight	:10;
	uint32 rsv_1	:6;
	uint32 lpbk_wrr_weight	:10;
};
typedef struct ipe_hdr_adj_wrr_weight_s ipe_hdr_adj_wrr_weight_t;

struct ipe_hdr_adj_addr_free_fifo_init_done_s	/* 450 */
{

	uint32 rsv_0	:31;
	uint32 addr_free_fifo_init_done	:1;
};
typedef struct ipe_hdr_adj_addr_free_fifo_init_done_s ipe_hdr_adj_addr_free_fifo_init_done_t;

struct ipe_hdr_adj_cmpc_res_word_ram_init_s	/* 451 */
{

	uint32 rsv_0	:31;
	uint32 cmpc_res_word_ram_init	:1;

	uint32 rsv_1	:31;
	uint32 cmpc_res_word_ram_init_done	:1;
};
typedef struct ipe_hdr_adj_cmpc_res_word_ram_init_s ipe_hdr_adj_cmpc_res_word_ram_init_t;

struct ipe_hdr_adj_credit_used_s	/* 452 */
{

	uint32 rsv_0	:29;
	uint32 ipe_intf_mapper_credit	:3;

	uint32 rsv_1	:28;
	uint32 buf_store_credit	:4;
};
typedef struct ipe_hdr_adj_credit_used_s ipe_hdr_adj_credit_used_t;

struct ipe_intf_mapper_interrupt0_s	/* 453 */
{

	uint32 value_set0	:32;

	uint32 value_reset0	:32;

	uint32 mask_set0	:32;

	uint32 mask_reset0	:32;
};
typedef struct ipe_intf_mapper_interrupt0_s ipe_intf_mapper_interrupt0_t;

struct ipe_intf_mapper_interrupt1_s	/* 454 */
{

	uint32 rsv_0	:19;
	uint32 value_set1	:13;

	uint32 rsv_1	:19;
	uint32 value_reset1	:13;

	uint32 rsv_2	:19;
	uint32 mask_set1	:13;

	uint32 rsv_3	:19;
	uint32 mask_reset1	:13;
};
typedef struct ipe_intf_mapper_interrupt1_s ipe_intf_mapper_interrupt1_t;

struct intf_mapper_config_s	/* 455 */
{

	uint32 rsv_0	:20;
	uint32 ds_link_aggr_credit_cfg	:4;
	uint32 info_credit_value	:4;
	uint32 rsv_1	:1;
	uint32 length_check_enable	:1;
	uint32 intf_mapper_drain_enable	:1;
	uint32 user_id_drain_enable	:1;

	uint32 rsv_2	:20;
	uint32 ds_link_aggr_running_credit	:4;
	uint32 rsv_3	:4;
	uint32 info_running_credit	:4;
};
typedef struct intf_mapper_config_s intf_mapper_config_t;

struct ipe_intf_mapper_init_s	/* 456 */
{

	uint32 rsv_0	:26;
	uint32 ds_vrf_init	:1;
	uint32 ds_src_interface_init	:1;
	uint32 ds_router_mac_init	:1;
	uint32 ds_protocol_vlan_init	:1;
	uint32 ds_src_port_init	:1;
	uint32 ds_phy_port_ext_init	:1;
};
typedef struct ipe_intf_mapper_init_s ipe_intf_mapper_init_t;

struct ipe_intf_mapper_init_done_s	/* 457 */
{

	uint32 rsv_0	:26;
	uint32 ds_vrf_init	:1;
	uint32 ds_src_interface_init	:1;
	uint32 ds_router_mac_init	:1;
	uint32 ds_protocol_vlan_init	:1;
	uint32 ds_src_port_init	:1;
	uint32 ds_phy_port_ext_init	:1;
};
typedef struct ipe_intf_mapper_init_done_s ipe_intf_mapper_init_done_t;

struct ipe_ds_vlan_ctl_s	/* 458 */
{

	uint32 ds_vlan_table_base_bit11_to8	:4;
	uint32 rsv_0	:1;
	uint32 protocol_based_stats_en	:1;
	uint32 mac_based_stats_en	:1;
	uint32 interface_stats_en	:1;
	uint32 rsv_1	:2;
	uint32 ds_stp_state_shift	:2;
	uint32 ds_vlan_status_table_base	:12;
	uint32 ds_vlan_table_base_bit19_to12	:8;

	uint32 rsv_2	:20;
	uint32 port_cross_connect_fwd_base	:12;

	uint32 rsv_3	:20;
	uint32 vlan_cross_connect_fwd_base	:12;
};
typedef struct ipe_ds_vlan_ctl_s ipe_ds_vlan_ctl_t;

struct ipe_intf_mapper_ctl_s	/* 459 */
{

	uint32 oam_bypass_vlan_rx_en	:1;
	uint32 oam_bypass_port_rx_en	:1;
	uint32 use_ip_hash	:1;
	uint32 ether_oam_obey_bypass_all	:1;
	uint32 pbb_tci_nca_check_en	:1;
	uint32 pbb_tci_res2_check_en	:1;
	uint32 arp_unicast_discard	:1;
	uint32 oam_bypass_ingress_filter	:1;
	uint32 pbb_oui_value	:24;

	uint32 pbb_bsi_oam_on_igs_cbp_en	:1;
	uint32 arp_unicast_exception_disable	:1;
	uint32 arp_check_exception_en	:1;
	uint32 pnp_use_cmac_hash	:1;
	uint32 cbp_use_cmac_hash	:1;
	uint32 rsv_0	:1;
	uint32 arp_address_check_en	:1;
	uint32 nca_value	:1;
	uint32 arp_exception_sub_index	:4;
	uint32 dhcp_exception_sub_index	:4;
	uint32 pip_mac_sa_bit47_to32	:16;

	uint32 pip_mac_sa_bit31_to8	:24;
	uint32 i_tag_valid_check	:1;
	uint32 pip_loop_mac_check	:1;
	uint32 pip_invalid_mac_da_check	:1;
	uint32 pbb_oam_en	:1;
	uint32 pbb_tci_nca_exception_en	:1;
	uint32 vlan_ptr_bits_num	:2;
	uint32 oam_bypass_routed_port	:1;

	uint32 svlan_ptr_base	:16;
	uint32 cvlan_ptr_base	:16;

	uint32 rsv_1	:26;
	uint32 discard_same_ip_addr	:1;
	uint32 discard_same_mac_addr	:1;
	uint32 arp_broadcast_routed_port_discard	:1;
	uint32 dhcp_broadcast_routed_port_discard	:1;
	uint32 dhcp_unicast_discard	:1;
	uint32 dhcp_unicast_exception_disable	:1;
};
typedef struct ipe_intf_mapper_ctl_s ipe_intf_mapper_ctl_t;

struct ipe_user_id_ctl_s	/* 460 */
{

	uint32 rsv_0	:16;
	uint32 use_hash_tcam0	:1;
	uint32 use_ext_tcam0	:1;
	uint32 key_size0	:2;
	uint32 table_id0	:4;
	uint32 lookup_inst0	:8;

	uint32 rsv_1	:16;
	uint32 use_hash_tcam1	:1;
	uint32 use_ext_tcam1	:1;
	uint32 key_size1	:2;
	uint32 table_id1	:4;
	uint32 lookup_inst1	:8;

	uint32 rsv_2	:16;
	uint32 use_hash_tcam2	:1;
	uint32 use_ext_tcam2	:1;
	uint32 key_size2	:2;
	uint32 table_id2	:4;
	uint32 lookup_inst2	:8;

	uint32 rsv_3	:16;
	uint32 use_hash_tcam3	:1;
	uint32 use_ext_tcam3	:1;
	uint32 key_size3	:2;
	uint32 table_id3	:4;
	uint32 lookup_inst3	:8;

	uint32 rsv_4	:6;
	uint32 index_shift0	:2;
	uint32 index_base0	:12;
	uint32 table_base0	:12;

	uint32 rsv_5	:6;
	uint32 index_shift1	:2;
	uint32 index_base1	:12;
	uint32 table_base1	:12;

	uint32 rsv_6	:6;
	uint32 index_shift2	:2;
	uint32 index_base2	:12;
	uint32 table_base2	:12;

	uint32 rsv_7	:6;
	uint32 index_shift3	:2;
	uint32 index_base3	:12;
	uint32 table_base3	:12;

	uint32 flow_policing_base	:8;
	uint32 service_policing_base	:8;
	uint32 user_id_mac_key_svlan_first	:1;
	uint32 sgmac_version	:1;
	uint32 force_mac_key	:1;
	uint32 arp_force_ipv4	:1;
	uint32 ds_fwd_base	:12;

	uint32 sgmac_ctl_ether_type	:16;
	uint32 rsv_8	:1;
	uint32 sgmac_ctl_msg_exception_en	:1;
	uint32 sgmac_ctl_msg_discard	:1;
	uint32 humber_id	:5;
	uint32 sgmac_ctl_msg_en	:1;
	uint32 sgmac_l3_mcast_hash_mod_port_en	:1;
	uint32 sgmac_mcast_hash_mod_port_en	:1;
	uint32 sgmac_mcast_hash_ip_sa_en	:1;
	uint32 sgmac_mcast_hash_ip_da_en	:1;
	uint32 sgmac_mcast_hash_mac_sa_en	:1;
	uint32 sgmac_mcast_hash_mac_da_en	:1;
	uint32 sgmac_lbid_en	:1;
};
typedef struct ipe_user_id_ctl_s ipe_user_id_ctl_t;

struct ipe_router_mac_ctl_s	/* 461 */
{

	uint32 rsv_0	:16;
	uint32 router_mac_type0_bit47_to32	:16;

	uint32 router_mac_type0_bit31_to8	:24;
	uint32 rsv_1	:8;

	uint32 rsv_2	:16;
	uint32 router_mac_type1_bit47_to32	:16;

	uint32 router_mac_type1_bit31_to8	:24;
	uint32 rsv_3	:8;

	uint32 rsv_4	:16;
	uint32 router_mac_type2_bit47_to32	:16;

	uint32 router_mac_type2_bit31_to8	:24;
	uint32 rsv_5	:8;
};
typedef struct ipe_router_mac_ctl_s ipe_router_mac_ctl_t;

struct ipe_intf_mapper_min_pkt_length_s	/* 462 */
{

	uint32 rsv_0	:18;
	uint32 min_pkt_length	:14;
};
typedef struct ipe_intf_mapper_min_pkt_length_s ipe_intf_mapper_min_pkt_length_t;

struct ipe_intf_mapper_max_pkt_length_s	/* 463 */
{

	uint32 rsv_0	:18;
	uint32 max_pkt_length	:14;
};
typedef struct ipe_intf_mapper_max_pkt_length_s ipe_intf_mapper_max_pkt_length_t;

struct ipe_intf_mapper_threshold_s	/* 464 */
{

	uint32 rsv_0	:4;
	uint32 vlan_track_fifoa_full_thrd	:4;
	uint32 rsv_1	:1;
	uint32 proc_info_fifoa_full_thrd	:7;
	uint32 rsv_2	:2;
	uint32 index_track_fifoa_full_thrd	:6;
	uint32 rsv_3	:1;
	uint32 key_track_fifoa_full_thrd	:7;

	uint32 rsv_4	:6;
	uint32 exppr_fifoa_full_thrd	:10;
	uint32 rsv_5	:3;
	uint32 src_port_info_fifoa_full_thrd	:5;
	uint32 exppi_fifoa_full_thrd	:8;
};
typedef struct ipe_intf_mapper_threshold_s ipe_intf_mapper_threshold_t;

struct ipe_bpdu_escape_ctl_s	/* 465 */
{

	uint32 protocol_escape	:16;
	uint32 rsv_0	:4;
	uint32 bpdu_exception_sub_index	:4;
	uint32 bpdu_exception	:1;
	uint32 rsv_1	:1;
	uint32 bpdu_bypass_all	:1;
	uint32 bpdu_escape_en	:5;

	uint32 protocol_bypass_all	:16;
	uint32 protocol_exception	:16;

	uint32 protocol_exception_sub_index0	:4;
	uint32 protocol_exception_sub_index1	:4;
	uint32 protocol_exception_sub_index2	:4;
	uint32 protocol_exception_sub_index3	:4;
	uint32 protocol_exception_sub_index4	:4;
	uint32 protocol_exception_sub_index5	:4;
	uint32 protocol_exception_sub_index6	:4;
	uint32 protocol_exception_sub_index7	:4;

	uint32 protocol_exception_sub_index8	:4;
	uint32 protocol_exception_sub_index9	:4;
	uint32 protocol_exception_sub_index10	:4;
	uint32 protocol_exception_sub_index11	:4;
	uint32 protocol_exception_sub_index12	:4;
	uint32 protocol_exception_sub_index13	:4;
	uint32 protocol_exception_sub_index14	:4;
	uint32 protocol_exception_sub_index15	:4;
};
typedef struct ipe_bpdu_escape_ctl_s ipe_bpdu_escape_ctl_t;

struct ipe_bpdu_protocol_escape_cam_s	/* 466 */
{

	uint32 rsv_0	:16;
	uint32 mac_da_mask0_bit47_to32	:16;

	uint32 mac_da_mask0_bit31_to0	:32;

	uint32 rsv_1	:16;
	uint32 mac_da_value0_bit47_to32	:16;

	uint32 mac_da_value0_bit31_to0	:32;

	uint32 rsv_2	:16;
	uint32 mac_da_mask1_bit47_to32	:16;

	uint32 mac_da_mask1_bit31_to0	:32;

	uint32 rsv_3	:16;
	uint32 mac_da_value1_bit47_to32	:16;

	uint32 mac_da_value1_bit31_to0	:32;

	uint32 rsv_4	:16;
	uint32 mac_da_mask2_bit47_to32	:16;

	uint32 mac_da_mask2_bit31_to0	:32;

	uint32 rsv_5	:16;
	uint32 mac_da_value2_bit47_to32	:16;

	uint32 mac_da_value2_bit31_to0	:32;

	uint32 rsv_6	:16;
	uint32 mac_da_mask3_bit47_to32	:16;

	uint32 mac_da_mask3_bit31_to0	:32;

	uint32 rsv_7	:16;
	uint32 mac_da_value3_bit47_to32	:16;

	uint32 mac_da_value3_bit31_to0	:32;
};
typedef struct ipe_bpdu_protocol_escape_cam_s ipe_bpdu_protocol_escape_cam_t;

struct ipe_bpdu_protocol_escape_cam2_s	/* 467 */
{

	uint32 rsv_0	:8;
	uint32 cam2_mac_da_mask0	:24;

	uint32 rsv_1	:8;
	uint32 cam2_mac_da_value0	:24;

	uint32 rsv_2	:8;
	uint32 cam2_mac_da_mask1	:24;

	uint32 rsv_3	:8;
	uint32 cam2_mac_da_value1	:24;

	uint32 rsv_4	:8;
	uint32 cam2_mac_da_mask2	:24;

	uint32 rsv_5	:8;
	uint32 cam2_mac_da_value2	:24;

	uint32 rsv_6	:8;
	uint32 cam2_mac_da_mask3	:24;

	uint32 rsv_7	:8;
	uint32 cam2_mac_da_value3	:24;

	uint32 rsv_8	:8;
	uint32 cam2_mac_da_mask4	:24;

	uint32 rsv_9	:8;
	uint32 cam2_mac_da_value4	:24;

	uint32 rsv_10	:8;
	uint32 cam2_mac_da_mask5	:24;

	uint32 rsv_11	:8;
	uint32 cam2_mac_da_value5	:24;

	uint32 rsv_12	:8;
	uint32 cam2_mac_da_mask6	:24;

	uint32 rsv_13	:8;
	uint32 cam2_mac_da_value6	:24;

	uint32 rsv_14	:8;
	uint32 cam2_mac_da_mask7	:24;

	uint32 rsv_15	:8;
	uint32 cam2_mac_da_value7	:24;
};
typedef struct ipe_bpdu_protocol_escape_cam2_s ipe_bpdu_protocol_escape_cam2_t;

struct ipe_bpdu_protocol_escape_cam3_s	/* 468 */
{

	uint32 rsv_0	:16;
	uint32 cam3_ether_type0	:16;

	uint32 rsv_1	:16;
	uint32 cam3_ether_type1	:16;

	uint32 rsv_2	:16;
	uint32 cam3_ether_type2	:16;

	uint32 rsv_3	:16;
	uint32 cam3_ether_type3	:16;

	uint32 rsv_4	:16;
	uint32 cam3_ether_type4	:16;

	uint32 rsv_5	:16;
	uint32 cam3_ether_type5	:16;

	uint32 rsv_6	:16;
	uint32 cam3_ether_type6	:16;

	uint32 rsv_7	:16;
	uint32 cam3_ether_type7	:16;

	uint32 rsv_8	:16;
	uint32 cam3_ether_type8	:16;

	uint32 rsv_9	:16;
	uint32 cam3_ether_type9	:16;

	uint32 rsv_10	:16;
	uint32 cam3_ether_type10	:16;

	uint32 rsv_11	:16;
	uint32 cam3_ether_type11	:16;

	uint32 rsv_12	:16;
	uint32 cam3_ether_type12	:16;

	uint32 rsv_13	:16;
	uint32 cam3_ether_type13	:16;

	uint32 rsv_14	:16;
	uint32 cam3_ether_type14	:16;

	uint32 rsv_15	:16;
	uint32 cam3_ether_type15	:16;
};
typedef struct ipe_bpdu_protocol_escape_cam3_s ipe_bpdu_protocol_escape_cam3_t;

struct ipe_bpdu_protocol_escape_cam_result_s	/* 469 */
{

	uint32 rsv_0	:26;
	uint32 cam1_escape_bypass_all0	:1;
	uint32 cam1_entry_valid0	:1;
	uint32 cam1_exception_sub_index0	:4;

	uint32 rsv_1	:26;
	uint32 cam1_escape_bypass_all1	:1;
	uint32 cam1_entry_valid1	:1;
	uint32 cam1_exception_sub_index1	:4;

	uint32 rsv_2	:26;
	uint32 cam1_escape_bypass_all2	:1;
	uint32 cam1_entry_valid2	:1;
	uint32 cam1_exception_sub_index2	:4;

	uint32 rsv_3	:26;
	uint32 cam1_escape_bypass_all3	:1;
	uint32 cam1_entry_valid3	:1;
	uint32 cam1_exception_sub_index3	:4;
};
typedef struct ipe_bpdu_protocol_escape_cam_result_s ipe_bpdu_protocol_escape_cam_result_t;

struct ipe_bpdu_protocol_escape_cam_result2_s	/* 470 */
{

	uint32 rsv_0	:26;
	uint32 cam2_escape_bypass_all0	:1;
	uint32 cam2_entry_valid0	:1;
	uint32 cam2_exception_sub_index0	:4;

	uint32 rsv_1	:26;
	uint32 cam2_escape_bypass_all1	:1;
	uint32 cam2_entry_valid1	:1;
	uint32 cam2_exception_sub_index1	:4;

	uint32 rsv_2	:26;
	uint32 cam2_escape_bypass_all2	:1;
	uint32 cam2_entry_valid2	:1;
	uint32 cam2_exception_sub_index2	:4;

	uint32 rsv_3	:26;
	uint32 cam2_escape_bypass_all3	:1;
	uint32 cam2_entry_valid3	:1;
	uint32 cam2_exception_sub_index3	:4;

	uint32 rsv_4	:26;
	uint32 cam2_escape_bypass_all4	:1;
	uint32 cam2_entry_valid4	:1;
	uint32 cam2_exception_sub_index4	:4;

	uint32 rsv_5	:26;
	uint32 cam2_escape_bypass_all5	:1;
	uint32 cam2_entry_valid5	:1;
	uint32 cam2_exception_sub_index5	:4;

	uint32 rsv_6	:26;
	uint32 cam2_escape_bypass_all6	:1;
	uint32 cam2_entry_valid6	:1;
	uint32 cam2_exception_sub_index6	:4;

	uint32 rsv_7	:26;
	uint32 cam2_escape_bypass_all7	:1;
	uint32 cam2_entry_valid7	:1;
	uint32 cam2_exception_sub_index7	:4;
};
typedef struct ipe_bpdu_protocol_escape_cam_result2_s ipe_bpdu_protocol_escape_cam_result2_t;

struct ipe_bpdu_protocol_escape_cam_result3_s	/* 471 */
{

	uint32 rsv_0	:26;
	uint32 cam3_escape_bypass_all0	:1;
	uint32 cam3_entry_valid0	:1;
	uint32 cam3_exception_sub_index0	:4;

	uint32 rsv_1	:26;
	uint32 cam3_escape_bypass_all1	:1;
	uint32 cam3_entry_valid1	:1;
	uint32 cam3_exception_sub_index1	:4;

	uint32 rsv_2	:26;
	uint32 cam3_escape_bypass_all2	:1;
	uint32 cam3_entry_valid2	:1;
	uint32 cam3_exception_sub_index2	:4;

	uint32 rsv_3	:26;
	uint32 cam3_escape_bypass_all3	:1;
	uint32 cam3_entry_valid3	:1;
	uint32 cam3_exception_sub_index3	:4;

	uint32 rsv_4	:26;
	uint32 cam3_escape_bypass_all4	:1;
	uint32 cam3_entry_valid4	:1;
	uint32 cam3_exception_sub_index4	:4;

	uint32 rsv_5	:26;
	uint32 cam3_escape_bypass_all5	:1;
	uint32 cam3_entry_valid5	:1;
	uint32 cam3_exception_sub_index5	:4;

	uint32 rsv_6	:26;
	uint32 cam3_escape_bypass_all6	:1;
	uint32 cam3_entry_valid6	:1;
	uint32 cam3_exception_sub_index6	:4;

	uint32 rsv_7	:26;
	uint32 cam3_escape_bypass_all7	:1;
	uint32 cam3_entry_valid7	:1;
	uint32 cam3_exception_sub_index7	:4;

	uint32 rsv_8	:26;
	uint32 cam3_escape_bypass_all8	:1;
	uint32 cam3_entry_valid8	:1;
	uint32 cam3_exception_sub_index8	:4;

	uint32 rsv_9	:26;
	uint32 cam3_escape_bypass_all9	:1;
	uint32 cam3_entry_valid9	:1;
	uint32 cam3_exception_sub_index9	:4;

	uint32 rsv_10	:26;
	uint32 cam3_escape_bypass_all10	:1;
	uint32 cam3_entry_valid10	:1;
	uint32 cam3_exception_sub_index10	:4;

	uint32 rsv_11	:26;
	uint32 cam3_escape_bypass_all11	:1;
	uint32 cam3_entry_valid11	:1;
	uint32 cam3_exception_sub_index11	:4;

	uint32 rsv_12	:26;
	uint32 cam3_escape_bypass_all12	:1;
	uint32 cam3_entry_valid12	:1;
	uint32 cam3_exception_sub_index12	:4;

	uint32 rsv_13	:26;
	uint32 cam3_escape_bypass_all13	:1;
	uint32 cam3_entry_valid13	:1;
	uint32 cam3_exception_sub_index13	:4;

	uint32 rsv_14	:26;
	uint32 cam3_escape_bypass_all14	:1;
	uint32 cam3_entry_valid14	:1;
	uint32 cam3_exception_sub_index14	:4;

	uint32 rsv_15	:26;
	uint32 cam3_escape_bypass_all15	:1;
	uint32 cam3_entry_valid15	:1;
	uint32 cam3_exception_sub_index15	:4;
};
typedef struct ipe_bpdu_protocol_escape_cam_result3_s ipe_bpdu_protocol_escape_cam_result3_t;

struct ipe_intf_mapper_rx_debug_s	/* 472 */
{

	uint32 rsv_0	:4;
	uint32 rx_len_error_drop_cnt	:4;
	uint32 rsv_1	:4;
	uint32 rx_drop_cnt	:4;
	uint32 rsv_2	:12;
	uint32 rx_pkt_cnt	:4;
};
typedef struct ipe_intf_mapper_rx_debug_s ipe_intf_mapper_rx_debug_t;

struct ipe_intf_mapper_tx_debug_s	/* 473 */
{

	uint32 rsv_0	:12;
	uint32 tx_drop_cnt	:4;
	uint32 rsv_1	:12;
	uint32 tx_pkt_cnt	:4;
};
typedef struct ipe_intf_mapper_tx_debug_s ipe_intf_mapper_tx_debug_t;

struct ipe_intf_mapper_tcam_arb_stats_s	/* 474 */
{

	uint32 rsv_0	:12;
	uint32 to_tcam_arb_eok_cnt	:4;
	uint32 rsv_1	:12;
	uint32 to_tcam_arb_sok_cnt	:4;

	uint32 rsv_2	:12;
	uint32 fr_tcam_arb_error_cnt	:4;
	uint32 rsv_3	:12;
	uint32 fr_tcam_arb_valid_cnt	:4;
};
typedef struct ipe_intf_mapper_tcam_arb_stats_s ipe_intf_mapper_tcam_arb_stats_t;

struct ipe_intf_mapper_tb_info_stats_s	/* 475 */
{

	uint32 rsv_0	:28;
	uint32 to_tb_info_valid_cnt	:4;

	uint32 rsv_1	:12;
	uint32 fr_tb_info_error_cnt	:4;
	uint32 rsv_2	:12;
	uint32 fr_tb_info_valid_cnt	:4;
};
typedef struct ipe_intf_mapper_tb_info_stats_s ipe_intf_mapper_tb_info_stats_t;

struct ipe_intf_mapper_shared_ds_stats_s	/* 476 */
{

	uint32 rsv_0	:28;
	uint32 to_ds_vlan_valid_cnt	:4;

	uint32 rsv_1	:4;
	uint32 fr_ds_vlan_seq_mismatch_cnt	:4;
	uint32 rsv_2	:4;
	uint32 fr_ds_vlan_error_cnt	:4;
	uint32 rsv_3	:12;
	uint32 fr_ds_vlan_valid_cnt	:4;

	uint32 rsv_4	:28;
	uint32 to_ds_vlan_status_valid_cnt	:4;

	uint32 rsv_5	:4;
	uint32 fr_ds_vlan_status_seq_mismatch_cnt	:4;
	uint32 rsv_6	:4;
	uint32 fr_ds_vlan_status_error_cnt	:4;
	uint32 rsv_7	:12;
	uint32 fr_ds_vlan_status_valid_cnt	:4;
};
typedef struct ipe_intf_mapper_shared_ds_stats_s ipe_intf_mapper_shared_ds_stats_t;

struct ipe_intf_mapper_ds_link_aggregate_group_stats_s	/* 477 */
{

	uint32 rsv_0	:28;
	uint32 to_ds_link_aggr_valid_cnt	:4;

	uint32 rsv_1	:4;
	uint32 fr_ds_link_aggr_seq_mismatch_cnt	:4;
	uint32 rsv_2	:4;
	uint32 fr_ds_link_aggr_error_cnt	:4;
	uint32 rsv_3	:12;
	uint32 fr_ds_link_aggr_valid_cnt	:4;
};
typedef struct ipe_intf_mapper_ds_link_aggregate_group_stats_s ipe_intf_mapper_ds_link_aggregate_group_stats_t;

struct ipe_intf_mapper_parity_fail_record_s	/* 478 */
{

	uint32 rsv_0	:15;
	uint32 ds_protocol_vlan_parity_fail	:1;
	uint32 rsv_1	:12;
	uint32 ds_protocol_vlan_parity_fail_addr	:4;

	uint32 rsv_2	:15;
	uint32 ds_router_mac_parity_fail	:1;
	uint32 rsv_3	:10;
	uint32 ds_router_mac_parity_fail_addr	:6;

	uint32 rsv_4	:15;
	uint32 ds_src_port_parity_fail	:1;
	uint32 rsv_5	:8;
	uint32 ds_src_port_parity_fail_addr	:8;

	uint32 rsv_6	:15;
	uint32 ds_phy_port_ext_parity_fail	:1;
	uint32 rsv_7	:8;
	uint32 ds_phy_port_ext_parity_fail_addr	:8;

	uint32 rsv_8	:15;
	uint32 ds_src_interface_parity_fail	:1;
	uint32 rsv_9	:6;
	uint32 ds_src_interface_parity_fail_addr	:10;

	uint32 rsv_10	:15;
	uint32 ds_vrf_parity_fail	:1;
	uint32 rsv_11	:8;
	uint32 ds_vrf_parity_fail_addr	:8;
};
typedef struct ipe_intf_mapper_parity_fail_record_s ipe_intf_mapper_parity_fail_record_t;

struct ipe_lookup_interrupt_s	/* 479 */
{

	uint32 rsv_0	:8;
	uint32 value_set	:24;

	uint32 rsv_1	:8;
	uint32 value_reset	:24;

	uint32 rsv_2	:8;
	uint32 mask_set	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset	:24;
};
typedef struct ipe_lookup_interrupt_s ipe_lookup_interrupt_t;

struct ipe_pkt_proc_credit_s	/* 480 */
{

	uint32 rsv_0	:26;
	uint32 pkt_proc_credit	:6;
};
typedef struct ipe_pkt_proc_credit_s ipe_pkt_proc_credit_t;

struct ipe_dual_index_order_s	/* 481 */
{

	uint32 rsv_0	:30;
	uint32 qos_hi_acl_lo	:1;
	uint32 rsv_1	:1;
};
typedef struct ipe_dual_index_order_s ipe_dual_index_order_t;

struct ipe_pkt_proc_credit_cfg_s	/* 482 */
{

	uint32 rsv_0	:26;
	uint32 pkt_proc_credit_cfg	:6;
};
typedef struct ipe_pkt_proc_credit_cfg_s ipe_pkt_proc_credit_cfg_t;

struct ipe_lookup_drain_enable_s	/* 483 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct ipe_lookup_drain_enable_s ipe_lookup_drain_enable_t;

struct ipe_lookup_cur_state_machine_s	/* 484 */
{

	uint32 rsv_0	:10;
	uint32 index_gen_cur_state_machine	:6;
	uint32 rsv_1	:3;
	uint32 key_gen_cur_state_machine	:5;
	uint32 rsv_2	:4;
	uint32 key_tx_cur_state_machine	:4;
};
typedef struct ipe_lookup_cur_state_machine_s ipe_lookup_cur_state_machine_t;

struct ipe_lookup_parity_fail_addr_s	/* 485 */
{

	uint32 ds_mpls_ctl_mem_parity_fail	:1;
	uint32 rsv_0	:23;
	uint32 ds_mpls_ctl_mem_parity_fail_addr	:8;
};
typedef struct ipe_lookup_parity_fail_addr_s ipe_lookup_parity_fail_addr_t;

struct ipe_lookup_ctl_s	/* 486 */
{

	uint32 rsv_0	:16;
	uint32 acl_qos_lookup_ctl0	:16;

	uint32 rsv_1	:16;
	uint32 acl_qos_lookup_ctl1	:16;

	uint32 rsv_2	:16;
	uint32 acl_qos_lookup_ctl2	:16;

	uint32 rsv_3	:16;
	uint32 ip_da_lookup_ctl0	:16;

	uint32 rsv_4	:16;
	uint32 ip_da_lookup_ctl1	:16;

	uint32 rsv_5	:16;
	uint32 ip_da_lookup_ctl2	:16;

	uint32 rsv_6	:16;
	uint32 ip_da_lookup_ctl3	:16;

	uint32 rsv_7	:16;
	uint32 ip_sa_lookup_ctl0	:16;

	uint32 rsv_8	:16;
	uint32 ip_sa_lookup_ctl1	:16;

	uint32 rsv_9	:16;
	uint32 ip_sa_lookup_ctl2	:16;

	uint32 rsv_10	:16;
	uint32 ip_sa_lookup_ctl3	:16;

	uint32 rsv_11	:16;
	uint32 ip_sa_lookup_ctl4	:16;

	uint32 rsv_12	:16;
	uint32 ip_sa_lookup_ctl5	:16;

	uint32 rsv_13	:16;
	uint32 mac_da_lookup_ctl	:16;

	uint32 rsv_14	:16;
	uint32 mac_sa_lookup_ctl	:16;

	uint32 rsv_15	:3;
	uint32 ipv6_mcast_force_bridge_en	:1;
	uint32 ipv4_mcast_force_bridge_en	:1;
	uint32 routed_port_disable_bcast_bridge	:1;
	uint32 ipv6_mcast_force_unicast_en	:1;
	uint32 ipv4_mcast_force_unicast_en	:1;
	uint32 rsv_16	:2;
	uint32 ipv6_ucast_route_key_sa_en	:1;
	uint32 ipv4_ucast_route_key_sa_en	:1;
	uint32 oam_obey_acl_qos	:1;
	uint32 rsv_17	:4;
	uint32 ether_oam_discard_lookup_en	:1;
	uint32 route_obey_stp	:1;
	uint32 stp_block_ip_mpls	:1;
	uint32 rsv_18	:2;
	uint32 dual_acl_lookup	:1;
	uint32 use_route_mac_in_acl_key	:1;
	uint32 no_ip_mcast_mac_lookup	:1;
	uint32 merge_mac_ip_acl_key	:1;
	uint32 rsv_19	:3;
	uint32 acl_use_packet_vlan	:1;
	uint32 stp_block_bridge_disable	:1;
	uint32 global_vrf_id_lookup_en	:1;

	uint32 rsv_20	:16;
	uint32 global_vrf_id	:16;
};
typedef struct ipe_lookup_ctl_s ipe_lookup_ctl_t;

struct ipe_lookup_result_ctl_s	/* 487 */
{

	uint32 rsv_0	:13;
	uint32 acl_lookup_result_ctl00	:19;

	uint32 acl_lookup_result_ctl01	:32;

	uint32 rsv_1	:13;
	uint32 acl_lookup_result_ctl10	:19;

	uint32 acl_lookup_result_ctl11	:32;

	uint32 rsv_2	:13;
	uint32 acl_lookup_result_ctl20	:19;

	uint32 acl_lookup_result_ctl21	:32;

	uint32 rsv_3	:13;
	uint32 ip_da_lookup_result_ctl00	:19;

	uint32 ip_da_lookup_result_ctl01	:32;

	uint32 rsv_4	:13;
	uint32 ip_da_lookup_result_ctl10	:19;

	uint32 ip_da_lookup_result_ctl11	:32;

	uint32 rsv_5	:13;
	uint32 ip_da_lookup_result_ctl20	:19;

	uint32 ip_da_lookup_result_ctl21	:32;

	uint32 rsv_6	:13;
	uint32 ip_da_lookup_result_ctl30	:19;

	uint32 ip_da_lookup_result_ctl31	:32;

	uint32 rsv_7	:13;
	uint32 ip_sa_lookup_result_ctl00	:19;

	uint32 ip_sa_lookup_result_ctl01	:32;

	uint32 rsv_8	:13;
	uint32 ip_sa_lookup_result_ctl10	:19;

	uint32 ip_sa_lookup_result_ctl11	:32;

	uint32 rsv_9	:13;
	uint32 ip_sa_lookup_result_ctl20	:19;

	uint32 ip_sa_lookup_result_ctl21	:32;

	uint32 rsv_10	:13;
	uint32 ip_sa_lookup_result_ctl30	:19;

	uint32 ip_sa_lookup_result_ctl31	:32;

	uint32 rsv_11	:13;
	uint32 ip_sa_lookup_result_ctl40	:19;

	uint32 ip_sa_lookup_result_ctl41	:32;

	uint32 rsv_12	:13;
	uint32 ip_sa_lookup_result_ctl50	:19;

	uint32 ip_sa_lookup_result_ctl51	:32;

	uint32 rsv_13	:13;
	uint32 mac_da_lookup_result_ctl0	:19;

	uint32 mac_da_lookup_result_ctl1	:32;

	uint32 rsv_14	:13;
	uint32 mac_sa_lookup_result_ctl0	:19;

	uint32 mac_sa_lookup_result_ctl1	:32;

	uint32 rsv_15	:13;
	uint32 qos_lookup_result_ctl00	:19;

	uint32 qos_lookup_result_ctl01	:32;

	uint32 rsv_16	:13;
	uint32 qos_lookup_result_ctl10	:19;

	uint32 qos_lookup_result_ctl11	:32;

	uint32 rsv_17	:13;
	uint32 qos_lookup_result_ctl20	:19;

	uint32 qos_lookup_result_ctl21	:32;

	uint32 rsv_18	:4;
	uint32 label_base_global	:12;
	uint32 rsv_19	:4;
	uint32 label_base_global_mcast	:12;

	uint32 ds_ipv6_mcast_rpf_table_base	:12;
	uint32 rsv_20	:20;

	uint32 ds_ipv4_mcast_rpf_table_base	:12;
	uint32 rsv_21	:20;

	uint32 rsv_22	:8;
	uint32 label_space_size_type_global	:4;
	uint32 min_interface_label	:12;
	uint32 ds_mpls_table_base1	:8;

	uint32 rsv_23	:8;
	uint32 label_space_size_type_global_mcast	:4;
	uint32 min_interface_label_mcast	:12;
	uint32 rsv_24	:4;
	uint32 ds_mpls_table_base0	:4;
};
typedef struct ipe_lookup_result_ctl_s ipe_lookup_result_ctl_t;

struct ipe_lookup_debug_stats_s	/* 488 */
{

	uint32 rsv_0	:24;
	uint32 input_pkt_info_cnt	:4;
	uint32 input_pkt_info_chop_cnt	:4;

	uint32 rsv_1	:24;
	uint32 input_par_result_cnt	:4;
	uint32 input_par_result_chop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 input_lookup_result_cnt	:4;

	uint32 rsv_3	:28;
	uint32 input_pkt_proc_done_cnt	:4;

	uint32 rsv_4	:28;
	uint32 output_lookup_mgr_done_cnt	:4;

	uint32 rsv_5	:24;
	uint32 output_lookup_key_cnt	:4;
	uint32 output_lookup_key_seg_cnt	:4;

	uint32 rsv_6	:28;
	uint32 output_info_rd_cnt	:4;

	uint32 rsv_7	:24;
	uint32 output_pkt_info_cnt	:4;
	uint32 output_pkt_info_chop_cnt	:4;

	uint32 rsv_8	:24;
	uint32 output_par_result_cnt	:4;
	uint32 output_par_result_chop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 output_pkt_proc_trig_cnt	:4;

	uint32 rsv_10	:28;
	uint32 drop4_lookup_err_or_not_hit_cnt	:4;
};
typedef struct ipe_lookup_debug_stats_s ipe_lookup_debug_stats_t;

struct ipe_hash_lookup_result_ctl_s	/* 489 */
{

	uint32 rsv_0	:4;
	uint32 mac_da_hash_result_value	:12;
	uint32 rsv_1	:4;
	uint32 mac_da_hash_result_mask	:12;

	uint32 rsv_2	:4;
	uint32 mac_sa_hash_result_value	:12;
	uint32 rsv_3	:4;
	uint32 mac_sa_hash_result_mask	:12;

	uint32 rsv_4	:4;
	uint32 ipv4_ucast_hash_result_value	:12;
	uint32 rsv_5	:4;
	uint32 ipv4_ucast_hash_result_mask	:12;

	uint32 rsv_6	:4;
	uint32 ipv4_mcast_hash_result_value	:12;
	uint32 rsv_7	:4;
	uint32 ipv4_mcast_hash_result_mask	:12;

	uint32 rsv_8	:4;
	uint32 ipv6_ucast_hash_result_value	:12;
	uint32 rsv_9	:4;
	uint32 ipv6_ucast_hash_result_mask	:12;

	uint32 rsv_10	:4;
	uint32 ipv6_mcast_hash_result_value	:12;
	uint32 rsv_11	:4;
	uint32 ipv6_mcast_hash_result_mask	:12;

	uint32 rsv_12	:19;
	uint32 mac_da_lookup_table_base_pos	:1;
	uint32 mac_da_lookup_table_base	:12;

	uint32 rsv_13	:19;
	uint32 mac_sa_lookup_table_base_pos	:1;
	uint32 mac_sa_lookup_table_base	:12;

	uint32 rsv_14	:19;
	uint32 ipv4_ucast_lookup_table_base_pos	:1;
	uint32 ipv4_ucast_lookup_table_base	:12;

	uint32 rsv_15	:19;
	uint32 ipv4_mcast_lookup_table_base_pos	:1;
	uint32 ipv4_mcast_lookup_table_base	:12;

	uint32 rsv_16	:19;
	uint32 ipv6_ucast_lookup_table_base_pos	:1;
	uint32 ipv6_ucast_lookup_table_base	:12;

	uint32 rsv_17	:19;
	uint32 ipv6_mcast_lookup_table_base_pos	:1;
	uint32 ipv6_mcast_lookup_table_base	:12;
};
typedef struct ipe_hash_lookup_result_ctl_s ipe_hash_lookup_result_ctl_t;

struct ipe_ipv4_mcast_force_route_s	/* 490 */
{

	uint32 addr0_value	:32;

	uint32 addr0_mask	:32;

	uint32 addr1_value	:32;

	uint32 addr1_mask	:32;
};
typedef struct ipe_ipv4_mcast_force_route_s ipe_ipv4_mcast_force_route_t;

struct ipe_ipv6_mcast_force_route_s	/* 491 */
{

	uint32 addr0_value127_to96	:32;

	uint32 addr0_value95_to64	:32;

	uint32 addr0_value63_to32	:32;

	uint32 addr0_value31_to0	:32;

	uint32 addr0_mask127_to96	:32;

	uint32 addr0_mask95_to64	:32;

	uint32 addr0_mask63_to32	:32;

	uint32 addr0_mask31_to0	:32;

	uint32 addr1_value127_to96	:32;

	uint32 addr1_value95_to64	:32;

	uint32 addr1_value63_to32	:32;

	uint32 addr1_value31_to0	:32;

	uint32 addr1_mask127_to96	:32;

	uint32 addr1_mask95_to64	:32;

	uint32 addr1_mask63_to32	:32;

	uint32 addr1_mask31_to0	:32;
};
typedef struct ipe_ipv6_mcast_force_route_s ipe_ipv6_mcast_force_route_t;

struct interrupt_s	/* 492 */
{

	uint32 rsv_0	:8;
	uint32 value_set	:24;

	uint32 rsv_1	:8;
	uint32 value_reset	:24;

	uint32 rsv_2	:8;
	uint32 mask_set	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset	:24;
};
typedef struct interrupt_s interrupt_t;

struct interrupt_normal_s	/* 493 */
{

	uint32 rsv_0	:8;
	uint32 value_set	:24;

	uint32 rsv_1	:8;
	uint32 value_reset	:24;

	uint32 rsv_2	:8;
	uint32 mask_set	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset	:24;
};
typedef struct interrupt_normal_s interrupt_normal_t;

struct ipe_pkt_proc_parity_enable_s	/* 494 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct ipe_pkt_proc_parity_enable_s ipe_pkt_proc_parity_enable_t;

struct ipe_pkt_proc_drain_enable_s	/* 495 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct ipe_pkt_proc_drain_enable_s ipe_pkt_proc_drain_enable_t;

struct ipe_pkt_proc_forward_credit_s	/* 496 */
{

	uint32 rsv_0	:18;
	uint32 running_credit	:6;
	uint32 rsv_1	:2;
	uint32 forward_credit	:6;
};
typedef struct ipe_pkt_proc_forward_credit_s ipe_pkt_proc_forward_credit_t;

struct ipe_pkt_proc_init_s	/* 497 */
{

	uint32 rsv_0	:22;
	uint32 ds_pbb_mac_tab_init_done	:1;
	uint32 ds_bidi_pim_group_init_done	:1;
	uint32 rsv_1	:6;
	uint32 ds_pbb_mac_tab_init	:1;
	uint32 ds_bidi_pim_group_init	:1;
};
typedef struct ipe_pkt_proc_init_s ipe_pkt_proc_init_t;

struct ipe_acl_qos_rand_seed_load_s	/* 498 */
{

	uint32 rand_seed_load	:1;
	uint32 rsv_0	:7;
	uint32 rand_seed_value	:24;
};
typedef struct ipe_acl_qos_rand_seed_load_s ipe_acl_qos_rand_seed_load_t;

struct ipe_acl_qos_ctl_s	/* 499 */
{

	uint32 rsv_0	:4;
	uint32 ds_fwd_ptr_base	:12;
	uint32 rsv_1	:15;
	uint32 qos_high_priority	:1;
};
typedef struct ipe_acl_qos_ctl_s ipe_acl_qos_ctl_t;

struct ipe_learning_cache_valid_s	/* 500 */
{

	uint32 always_cpu_learning	:1;
	uint32 exception_en	:1;
	uint32 rsv_0	:9;
	uint32 learning_cache_int_thrd	:5;
	uint32 learning_entry_valid	:16;
};
typedef struct ipe_learning_cache_valid_s ipe_learning_cache_valid_t;

struct ipe_route_ctl_s	/* 501 */
{

	uint32 rsv_0	:12;
	uint32 gre_option2_check_en	:1;
	uint32 gre_flex_payload_packet_type	:3;
	uint32 gre_flex_protocol	:16;

	uint32 rsv_1	:7;
	uint32 martian_check_en_bit10_to6	:5;
	uint32 mcast_escape_to_cpu	:1;
	uint32 martian_addr_check_dis	:1;
	uint32 mcast_addr_match_check_dis	:1;
	uint32 ip_options_escape_dis	:1;
	uint32 route_offset_byte_shift	:2;
	uint32 martian_check_en_bit5_to0	:6;
	uint32 ip_ttl_limit	:8;
};
typedef struct ipe_route_ctl_s ipe_route_ctl_t;

struct ipe_bridge_ctl_s	/* 502 */
{

	uint32 rsv_0	:25;
	uint32 ucast_storm_ctrl_mode	:1;
	uint32 mcast_storm_ctrl_mode	:1;
	uint32 discard_force_bridge	:1;
	uint32 pnp_use_cmac_hash	:1;
	uint32 cbp_use_cmac_hash	:1;
	uint32 oam_bypass_ingress_stp	:1;
	uint32 use_ip_hash	:1;

	uint32 rsv_1	:16;
	uint32 protocol_exception	:16;

	uint32 protocol_exception_sub_index0	:4;
	uint32 protocol_exception_sub_index1	:4;
	uint32 protocol_exception_sub_index2	:4;
	uint32 protocol_exception_sub_index3	:4;
	uint32 protocol_exception_sub_index4	:4;
	uint32 protocol_exception_sub_index5	:4;
	uint32 protocol_exception_sub_index6	:4;
	uint32 protocol_exception_sub_index7	:4;

	uint32 protocol_exception_sub_index8	:4;
	uint32 protocol_exception_sub_index9	:4;
	uint32 protocol_exception_sub_index10	:4;
	uint32 protocol_exception_sub_index11	:4;
	uint32 protocol_exception_sub_index12	:4;
	uint32 protocol_exception_sub_index13	:4;
	uint32 protocol_exception_sub_index14	:4;
	uint32 protocol_exception_sub_index15	:4;
};
typedef struct ipe_bridge_ctl_s ipe_bridge_ctl_t;

struct ipe_bridge_storm_ctl_s	/* 503 */
{

	uint32 update_threshold	:32;

	uint32 storm_ctl_upd_en	:1;
	uint32 oam_obey_storm_ctl	:1;
	uint32 max_update_port_num	:14;
	uint32 ipg_en	:1;
	uint32 rsv_0	:1;
	uint32 max_port_num	:14;
};
typedef struct ipe_bridge_storm_ctl_s ipe_bridge_storm_ctl_t;

struct ipe_classification_ctl_s	/* 504 */
{

	uint32 oam_obey_policer_color	:1;
	uint32 flow_policer_first	:1;
	uint32 vlan_flow_policer_shift	:2;
	uint32 rsv_0	:2;
	uint32 port_policer_shift	:2;
	uint32 rsv_1	:2;
	uint32 phb_stats_en	:1;
	uint32 phb_per_port_stats_en	:1;
	uint32 rsv_2	:2;
	uint32 phb_port_stats_shift	:2;
	uint32 phb_stats_base	:16;

	uint32 rsv_3	:12;
	uint32 local_phy_port_mask	:4;
	uint32 port_policer_base	:16;
};
typedef struct ipe_classification_ctl_s ipe_classification_ctl_t;

struct ipe_classification_phb_offset_table_s	/* 505 */
{

	uint32 phb_offset_entry3	:32;

	uint32 phb_offset_entry2	:32;

	uint32 phb_offset_entry1	:32;

	uint32 phb_offset_entry0	:32;
};
typedef struct ipe_classification_phb_offset_table_s ipe_classification_phb_offset_table_t;

struct ipe_route_martian_addr_s	/* 506 */
{

	uint32 flex0_value	:32;

	uint32 flex0_mask	:32;

	uint32 flex1_value	:32;

	uint32 flex1_mask	:32;
};
typedef struct ipe_route_martian_addr_s ipe_route_martian_addr_t;

struct ipe_classification_path_map_table_s	/* 507 */
{

	uint32 rsv_0	:1;
	uint32 pri_path_sel63	:3;
	uint32 rsv_1	:1;
	uint32 pri_path_sel62	:3;
	uint32 rsv_2	:1;
	uint32 pri_path_sel61	:3;
	uint32 rsv_3	:1;
	uint32 pri_path_sel60	:3;
	uint32 rsv_4	:1;
	uint32 pri_path_sel59	:3;
	uint32 rsv_5	:1;
	uint32 pri_path_sel58	:3;
	uint32 rsv_6	:1;
	uint32 pri_path_sel57	:3;
	uint32 rsv_7	:1;
	uint32 pri_path_sel56	:3;

	uint32 rsv_8	:1;
	uint32 pri_path_sel55	:3;
	uint32 rsv_9	:1;
	uint32 pri_path_sel54	:3;
	uint32 rsv_10	:1;
	uint32 pri_path_sel53	:3;
	uint32 rsv_11	:1;
	uint32 pri_path_sel52	:3;
	uint32 rsv_12	:1;
	uint32 pri_path_sel51	:3;
	uint32 rsv_13	:1;
	uint32 pri_path_sel50	:3;
	uint32 rsv_14	:1;
	uint32 pri_path_sel49	:3;
	uint32 rsv_15	:1;
	uint32 pri_path_sel48	:3;

	uint32 rsv_16	:1;
	uint32 pri_path_sel47	:3;
	uint32 rsv_17	:1;
	uint32 pri_path_sel46	:3;
	uint32 rsv_18	:1;
	uint32 pri_path_sel45	:3;
	uint32 rsv_19	:1;
	uint32 pri_path_sel44	:3;
	uint32 rsv_20	:1;
	uint32 pri_path_sel43	:3;
	uint32 rsv_21	:1;
	uint32 pri_path_sel42	:3;
	uint32 rsv_22	:1;
	uint32 pri_path_sel41	:3;
	uint32 rsv_23	:1;
	uint32 pri_path_sel40	:3;

	uint32 rsv_24	:1;
	uint32 pri_path_sel39	:3;
	uint32 rsv_25	:1;
	uint32 pri_path_sel38	:3;
	uint32 rsv_26	:1;
	uint32 pri_path_sel37	:3;
	uint32 rsv_27	:1;
	uint32 pri_path_sel36	:3;
	uint32 rsv_28	:1;
	uint32 pri_path_sel35	:3;
	uint32 rsv_29	:1;
	uint32 pri_path_sel34	:3;
	uint32 rsv_30	:1;
	uint32 pri_path_sel33	:3;
	uint32 rsv_31	:1;
	uint32 pri_path_sel32	:3;

	uint32 rsv_32	:1;
	uint32 pri_path_sel31	:3;
	uint32 rsv_33	:1;
	uint32 pri_path_sel30	:3;
	uint32 rsv_34	:1;
	uint32 pri_path_sel29	:3;
	uint32 rsv_35	:1;
	uint32 pri_path_sel28	:3;
	uint32 rsv_36	:1;
	uint32 pri_path_sel27	:3;
	uint32 rsv_37	:1;
	uint32 pri_path_sel26	:3;
	uint32 rsv_38	:1;
	uint32 pri_path_sel25	:3;
	uint32 rsv_39	:1;
	uint32 pri_path_sel24	:3;

	uint32 rsv_40	:1;
	uint32 pri_path_sel23	:3;
	uint32 rsv_41	:1;
	uint32 pri_path_sel22	:3;
	uint32 rsv_42	:1;
	uint32 pri_path_sel21	:3;
	uint32 rsv_43	:1;
	uint32 pri_path_sel20	:3;
	uint32 rsv_44	:1;
	uint32 pri_path_sel19	:3;
	uint32 rsv_45	:1;
	uint32 pri_path_sel18	:3;
	uint32 rsv_46	:1;
	uint32 pri_path_sel17	:3;
	uint32 rsv_47	:1;
	uint32 pri_path_sel16	:3;

	uint32 rsv_48	:1;
	uint32 pri_path_sel15	:3;
	uint32 rsv_49	:1;
	uint32 pri_path_sel14	:3;
	uint32 rsv_50	:1;
	uint32 pri_path_sel13	:3;
	uint32 rsv_51	:1;
	uint32 pri_path_sel12	:3;
	uint32 rsv_52	:1;
	uint32 pri_path_sel11	:3;
	uint32 rsv_53	:1;
	uint32 pri_path_sel10	:3;
	uint32 rsv_54	:1;
	uint32 pri_path_sel9	:3;
	uint32 rsv_55	:1;
	uint32 pri_path_sel8	:3;

	uint32 rsv_56	:1;
	uint32 pri_path_sel7	:3;
	uint32 rsv_57	:1;
	uint32 pri_path_sel6	:3;
	uint32 rsv_58	:1;
	uint32 pri_path_sel5	:3;
	uint32 rsv_59	:1;
	uint32 pri_path_sel4	:3;
	uint32 rsv_60	:1;
	uint32 pri_path_sel3	:3;
	uint32 rsv_61	:1;
	uint32 pri_path_sel2	:3;
	uint32 rsv_62	:1;
	uint32 pri_path_sel1	:3;
	uint32 rsv_63	:1;
	uint32 pri_path_sel0	:3;
};
typedef struct ipe_classification_path_map_table_s ipe_classification_path_map_table_t;

struct ipe_mpls_ctl_s	/* 508 */
{

	uint32 rsv_0	:1;
	uint32 exp_policer_offset7	:3;
	uint32 rsv_1	:1;
	uint32 exp_policer_offset6	:3;
	uint32 rsv_2	:1;
	uint32 exp_policer_offset5	:3;
	uint32 rsv_3	:1;
	uint32 exp_policer_offset4	:3;
	uint32 rsv_4	:1;
	uint32 exp_policer_offset3	:3;
	uint32 rsv_5	:1;
	uint32 exp_policer_offset2	:3;
	uint32 rsv_6	:1;
	uint32 exp_policer_offset1	:3;
	uint32 rsv_7	:1;
	uint32 exp_policer_offset0	:3;

	uint32 vccv_cw	:4;
	uint32 mpls_flow_stats_base	:8;
	uint32 rsv_8	:2;
	uint32 mpls_offset_bytes_shift	:2;
	uint32 rsv_9	:6;
	uint32 use_first_label_exp	:1;
	uint32 use_first_label_ttl	:1;
	uint32 mpls_flow_policer_base	:8;

	uint32 mpls_ttl_limit	:8;
	uint32 rsv_10	:4;
	uint32 oam_alert_label0	:20;

	uint32 mpls_ttl_decrement	:8;
	uint32 rsv_11	:4;
	uint32 oam_alert_label1	:20;

	uint32 rsv_12	:6;
	uint32 mpls_policing_high_priority	:1;
	uint32 mpls_stats_high_priority	:1;
	uint32 mpls_ecmp_full_label	:1;
	uint32 mpls_ecmp_use_reserve_label	:1;
	uint32 mpls_ecmp_use_label	:1;
	uint32 mpls_ecmp_use_ip	:1;
	uint32 max_reserve_label	:20;
};
typedef struct ipe_mpls_ctl_s ipe_mpls_ctl_t;

struct ipe_ipg_ctl_s	/* 509 */
{

	uint32 rsv_0	:24;
	uint32 ipg0	:8;

	uint32 rsv_1	:24;
	uint32 ipg1	:8;

	uint32 rsv_2	:24;
	uint32 ipg2	:8;

	uint32 rsv_3	:24;
	uint32 ipg3	:8;
};
typedef struct ipe_ipg_ctl_s ipe_ipg_ctl_t;

struct ipe_exception3_ctl_s	/* 510 */
{

	uint32 protocol_exception_en	:16;
	uint32 rsv_0	:14;
	uint32 exception_cam_en2	:1;
	uint32 exception_cam_en	:1;

	uint32 rsv_1	:32;

	uint32 protocol_exception3_sub_index0	:4;
	uint32 protocol_exception3_sub_index1	:4;
	uint32 protocol_exception3_sub_index2	:4;
	uint32 protocol_exception3_sub_index3	:4;
	uint32 protocol_exception3_sub_index4	:4;
	uint32 protocol_exception3_sub_index5	:4;
	uint32 protocol_exception3_sub_index6	:4;
	uint32 protocol_exception3_sub_index7	:4;

	uint32 protocol_exception3_sub_index8	:4;
	uint32 protocol_exception3_sub_index9	:4;
	uint32 protocol_exception3_sub_index10	:4;
	uint32 protocol_exception3_sub_index11	:4;
	uint32 protocol_exception3_sub_index12	:4;
	uint32 protocol_exception3_sub_index13	:4;
	uint32 protocol_exception3_sub_index14	:4;
	uint32 protocol_exception3_sub_index15	:4;
};
typedef struct ipe_exception3_ctl_s ipe_exception3_ctl_t;

struct ipe_exception3_cam_s	/* 511 */
{

	uint32 rsv_0	:24;
	uint32 l3_header_protocol0	:8;

	uint32 rsv_1	:24;
	uint32 l3_header_protocol1	:8;

	uint32 rsv_2	:24;
	uint32 l3_header_protocol2	:8;

	uint32 rsv_3	:24;
	uint32 l3_header_protocol3	:8;

	uint32 rsv_4	:24;
	uint32 l3_header_protocol4	:8;

	uint32 rsv_5	:24;
	uint32 l3_header_protocol5	:8;

	uint32 rsv_6	:24;
	uint32 l3_header_protocol6	:8;

	uint32 rsv_7	:24;
	uint32 l3_header_protocol7	:8;

	uint32 rsv_8	:24;
	uint32 l3_header_protocol8	:8;

	uint32 rsv_9	:24;
	uint32 l3_header_protocol9	:8;

	uint32 rsv_10	:24;
	uint32 l3_header_protocol10	:8;

	uint32 rsv_11	:24;
	uint32 l3_header_protocol11	:8;

	uint32 rsv_12	:24;
	uint32 l3_header_protocol12	:8;

	uint32 rsv_13	:24;
	uint32 l3_header_protocol13	:8;

	uint32 rsv_14	:24;
	uint32 l3_header_protocol14	:8;

	uint32 rsv_15	:24;
	uint32 l3_header_protocol15	:8;
};
typedef struct ipe_exception3_cam_s ipe_exception3_cam_t;

struct ipe_exception3_cam_result_s	/* 512 */
{

	uint32 rsv_0	:27;
	uint32 entry_valid0	:1;
	uint32 exception_sub_index0	:4;

	uint32 rsv_1	:27;
	uint32 entry_valid1	:1;
	uint32 exception_sub_index1	:4;

	uint32 rsv_2	:27;
	uint32 entry_valid2	:1;
	uint32 exception_sub_index2	:4;

	uint32 rsv_3	:27;
	uint32 entry_valid3	:1;
	uint32 exception_sub_index3	:4;

	uint32 rsv_4	:27;
	uint32 entry_valid4	:1;
	uint32 exception_sub_index4	:4;

	uint32 rsv_5	:27;
	uint32 entry_valid5	:1;
	uint32 exception_sub_index5	:4;

	uint32 rsv_6	:27;
	uint32 entry_valid6	:1;
	uint32 exception_sub_index6	:4;

	uint32 rsv_7	:27;
	uint32 entry_valid7	:1;
	uint32 exception_sub_index7	:4;

	uint32 rsv_8	:27;
	uint32 entry_valid8	:1;
	uint32 exception_sub_index8	:4;

	uint32 rsv_9	:27;
	uint32 entry_valid9	:1;
	uint32 exception_sub_index9	:4;

	uint32 rsv_10	:27;
	uint32 entry_valid10	:1;
	uint32 exception_sub_index10	:4;

	uint32 rsv_11	:27;
	uint32 entry_valid11	:1;
	uint32 exception_sub_index11	:4;

	uint32 rsv_12	:27;
	uint32 entry_valid12	:1;
	uint32 exception_sub_index12	:4;

	uint32 rsv_13	:27;
	uint32 entry_valid13	:1;
	uint32 exception_sub_index13	:4;

	uint32 rsv_14	:27;
	uint32 entry_valid14	:1;
	uint32 exception_sub_index14	:4;

	uint32 rsv_15	:27;
	uint32 entry_valid15	:1;
	uint32 exception_sub_index15	:4;
};
typedef struct ipe_exception3_cam_result_s ipe_exception3_cam_result_t;

struct ipe_exception3_cam2_s	/* 513 */
{

	uint32 rsv_0	:12;
	uint32 is_udp_mask0	:1;
	uint32 is_udp_value0	:1;
	uint32 is_tcp_mask0	:1;
	uint32 is_tcp_value0	:1;
	uint32 dest_port0	:16;

	uint32 rsv_1	:12;
	uint32 is_udp_mask1	:1;
	uint32 is_udp_value1	:1;
	uint32 is_tcp_mask1	:1;
	uint32 is_tcp_value1	:1;
	uint32 dest_port1	:16;

	uint32 rsv_2	:12;
	uint32 is_udp_mask2	:1;
	uint32 is_udp_value2	:1;
	uint32 is_tcp_mask2	:1;
	uint32 is_tcp_value2	:1;
	uint32 dest_port2	:16;

	uint32 rsv_3	:12;
	uint32 is_udp_mask3	:1;
	uint32 is_udp_value3	:1;
	uint32 is_tcp_mask3	:1;
	uint32 is_tcp_value3	:1;
	uint32 dest_port3	:16;

	uint32 rsv_4	:12;
	uint32 is_udp_mask4	:1;
	uint32 is_udp_value4	:1;
	uint32 is_tcp_mask4	:1;
	uint32 is_tcp_value4	:1;
	uint32 dest_port4	:16;

	uint32 rsv_5	:12;
	uint32 is_udp_mask5	:1;
	uint32 is_udp_value5	:1;
	uint32 is_tcp_mask5	:1;
	uint32 is_tcp_value5	:1;
	uint32 dest_port5	:16;

	uint32 rsv_6	:12;
	uint32 is_udp_mask6	:1;
	uint32 is_udp_value6	:1;
	uint32 is_tcp_mask6	:1;
	uint32 is_tcp_value6	:1;
	uint32 dest_port6	:16;

	uint32 rsv_7	:12;
	uint32 is_udp_mask7	:1;
	uint32 is_udp_value7	:1;
	uint32 is_tcp_mask7	:1;
	uint32 is_tcp_value7	:1;
	uint32 dest_port7	:16;

	uint32 rsv_8	:12;
	uint32 is_udp_mask8	:1;
	uint32 is_udp_value8	:1;
	uint32 is_tcp_mask8	:1;
	uint32 is_tcp_value8	:1;
	uint32 dest_port8	:16;

	uint32 rsv_9	:12;
	uint32 is_udp_mask9	:1;
	uint32 is_udp_value9	:1;
	uint32 is_tcp_mask9	:1;
	uint32 is_tcp_value9	:1;
	uint32 dest_port9	:16;

	uint32 rsv_10	:12;
	uint32 is_udp_mask10	:1;
	uint32 is_udp_value10	:1;
	uint32 is_tcp_mask10	:1;
	uint32 is_tcp_value10	:1;
	uint32 dest_port10	:16;

	uint32 rsv_11	:12;
	uint32 is_udp_mask11	:1;
	uint32 is_udp_value11	:1;
	uint32 is_tcp_mask11	:1;
	uint32 is_tcp_value11	:1;
	uint32 dest_port11	:16;

	uint32 rsv_12	:12;
	uint32 is_udp_mask12	:1;
	uint32 is_udp_value12	:1;
	uint32 is_tcp_mask12	:1;
	uint32 is_tcp_value12	:1;
	uint32 dest_port12	:16;

	uint32 rsv_13	:12;
	uint32 is_udp_mask13	:1;
	uint32 is_udp_value13	:1;
	uint32 is_tcp_mask13	:1;
	uint32 is_tcp_value13	:1;
	uint32 dest_port13	:16;

	uint32 rsv_14	:12;
	uint32 is_udp_mask14	:1;
	uint32 is_udp_value14	:1;
	uint32 is_tcp_mask14	:1;
	uint32 is_tcp_value14	:1;
	uint32 dest_port14	:16;

	uint32 rsv_15	:12;
	uint32 is_udp_mask15	:1;
	uint32 is_udp_value15	:1;
	uint32 is_tcp_mask15	:1;
	uint32 is_tcp_value15	:1;
	uint32 dest_port15	:16;
};
typedef struct ipe_exception3_cam2_s ipe_exception3_cam2_t;

struct ipe_exception3_cam2_result_s	/* 514 */
{

	uint32 rsv_0	:27;
	uint32 cam2_entry_valid0	:1;
	uint32 cam2_exception_sub_index0	:4;

	uint32 rsv_1	:27;
	uint32 cam2_entry_valid1	:1;
	uint32 cam2_exception_sub_index1	:4;

	uint32 rsv_2	:27;
	uint32 cam2_entry_valid2	:1;
	uint32 cam2_exception_sub_index2	:4;

	uint32 rsv_3	:27;
	uint32 cam2_entry_valid3	:1;
	uint32 cam2_exception_sub_index3	:4;

	uint32 rsv_4	:27;
	uint32 cam2_entry_valid4	:1;
	uint32 cam2_exception_sub_index4	:4;

	uint32 rsv_5	:27;
	uint32 cam2_entry_valid5	:1;
	uint32 cam2_exception_sub_index5	:4;

	uint32 rsv_6	:27;
	uint32 cam2_entry_valid6	:1;
	uint32 cam2_exception_sub_index6	:4;

	uint32 rsv_7	:27;
	uint32 cam2_entry_valid7	:1;
	uint32 cam2_exception_sub_index7	:4;

	uint32 rsv_8	:27;
	uint32 cam2_entry_valid8	:1;
	uint32 cam2_exception_sub_index8	:4;

	uint32 rsv_9	:27;
	uint32 cam2_entry_valid9	:1;
	uint32 cam2_exception_sub_index9	:4;

	uint32 rsv_10	:27;
	uint32 cam2_entry_valid10	:1;
	uint32 cam2_exception_sub_index10	:4;

	uint32 rsv_11	:27;
	uint32 cam2_entry_valid11	:1;
	uint32 cam2_exception_sub_index11	:4;

	uint32 rsv_12	:27;
	uint32 cam2_entry_valid12	:1;
	uint32 cam2_exception_sub_index12	:4;

	uint32 rsv_13	:27;
	uint32 cam2_entry_valid13	:1;
	uint32 cam2_exception_sub_index13	:4;

	uint32 rsv_14	:27;
	uint32 cam2_entry_valid14	:1;
	uint32 cam2_exception_sub_index14	:4;

	uint32 rsv_15	:27;
	uint32 cam2_entry_valid15	:1;
	uint32 cam2_exception_sub_index15	:4;
};
typedef struct ipe_exception3_cam2_result_s ipe_exception3_cam2_result_t;

struct ipe_pkt_proc_discard_src_s	/* 515 */
{

	uint32 rsv_0	:22;
	uint32 pkt_proc_discard_src	:10;
};
typedef struct ipe_pkt_proc_discard_src_s ipe_pkt_proc_discard_src_t;

struct ipe_storm_ctl_update_cnt_s	/* 516 */
{

	uint32 update_count	:32;
};
typedef struct ipe_storm_ctl_update_cnt_s ipe_storm_ctl_update_cnt_t;

struct ipe_pkt_proc_input_pkt_info_cnt_s	/* 517 */
{

	uint32 rsv_0	:12;
	uint32 input_pkt_info_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_pkt_info_chop_cnt	:4;
};
typedef struct ipe_pkt_proc_input_pkt_info_cnt_s ipe_pkt_proc_input_pkt_info_cnt_t;

struct ipe_pkt_proc_input_par_result_cnt_s	/* 518 */
{

	uint32 rsv_0	:12;
	uint32 input_par_result_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_par_result_chop_cnt	:4;
};
typedef struct ipe_pkt_proc_input_par_result_cnt_s ipe_pkt_proc_input_par_result_cnt_t;

struct ipe_pkt_proc_input_trig_cnt_s	/* 519 */
{

	uint32 rsv_0	:28;
	uint32 input_trig_cnt	:4;
};
typedef struct ipe_pkt_proc_input_trig_cnt_s ipe_pkt_proc_input_trig_cnt_t;

struct ipe_pkt_proc_input_tb_info_ds_cnt_s	/* 520 */
{

	uint32 rsv_0	:28;
	uint32 input_tb_info_ds_cnt	:4;
};
typedef struct ipe_pkt_proc_input_tb_info_ds_cnt_s ipe_pkt_proc_input_tb_info_ds_cnt_t;

struct ipe_pkt_proc_input_policing_result_cnt_s	/* 521 */
{

	uint32 rsv_0	:28;
	uint32 input_policing_result_cnt	:4;
};
typedef struct ipe_pkt_proc_input_policing_result_cnt_s ipe_pkt_proc_input_policing_result_cnt_t;

struct ipe_pkt_proc_input_forward_done_cnt_s	/* 522 */
{

	uint32 rsv_0	:28;
	uint32 input_forward_done_cnt	:4;
};
typedef struct ipe_pkt_proc_input_forward_done_cnt_s ipe_pkt_proc_input_forward_done_cnt_t;

struct ipe_pkt_proc_output_policing_req_cnt_s	/* 523 */
{

	uint32 rsv_0	:28;
	uint32 output_policing_req_cnt	:4;
};
typedef struct ipe_pkt_proc_output_policing_req_cnt_s ipe_pkt_proc_output_policing_req_cnt_t;

struct ipe_pkt_proc_output_cla_result_cnt_s	/* 524 */
{

	uint32 rsv_0	:28;
	uint32 output_cla_result_cnt	:4;
};
typedef struct ipe_pkt_proc_output_cla_result_cnt_s ipe_pkt_proc_output_cla_result_cnt_t;

struct ipe_pkt_proc_output_result_cnt_s	/* 525 */
{

	uint32 rsv_0	:12;
	uint32 output_pkt_info_cnt	:4;
	uint32 rsv_1	:12;
	uint32 output_pkt_info_chop_cnt	:4;
};
typedef struct ipe_pkt_proc_output_result_cnt_s ipe_pkt_proc_output_result_cnt_t;

struct ipe_pkt_proc_output_pkt_proc_done_cnt_s	/* 526 */
{

	uint32 rsv_0	:28;
	uint32 output_pkt_proc_done_cnt	:4;
};
typedef struct ipe_pkt_proc_output_pkt_proc_done_cnt_s ipe_pkt_proc_output_pkt_proc_done_cnt_t;

struct ipe_pkt_proc_drop_from_former_cnt_s	/* 527 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_former_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_from_former_cnt_s ipe_pkt_proc_drop_from_former_cnt_t;

struct ipe_pkt_proc_drop_in_lookup_cnt_s	/* 528 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_lookup_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_lookup_cnt_s ipe_pkt_proc_drop_in_lookup_cnt_t;

struct ipe_pkt_proc_drop_in_expand_cnt_s	/* 529 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_expand_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_expand_cnt_s ipe_pkt_proc_drop_in_expand_cnt_t;

struct ipe_pkt_proc_drop_in_acl_cnt_s	/* 530 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_acl_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_acl_cnt_s ipe_pkt_proc_drop_in_acl_cnt_t;

struct ipe_pkt_proc_drop_in_qos_cnt_s	/* 531 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_qos_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_qos_cnt_s ipe_pkt_proc_drop_in_qos_cnt_t;

struct ipe_pkt_proc_drop_in_routing_cnt_s	/* 532 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_routing_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_routing_cnt_s ipe_pkt_proc_drop_in_routing_cnt_t;

struct ipe_pkt_proc_drop_in_bridge_cnt_s	/* 533 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_bridge_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_bridge_cnt_s ipe_pkt_proc_drop_in_bridge_cnt_t;

struct ipe_pkt_proc_drop_in_learn_cnt_s	/* 534 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_learn_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_learn_cnt_s ipe_pkt_proc_drop_in_learn_cnt_t;

struct ipe_pkt_proc_drop_in_storm_ctl_cnt_s	/* 535 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_storm_ctl_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_storm_ctl_cnt_s ipe_pkt_proc_drop_in_storm_ctl_cnt_t;

struct ipe_pkt_proc_drop_in_mpls_cnt_s	/* 536 */
{

	uint32 rsv_0	:28;
	uint32 drop_in_mpls_cnt	:4;
};
typedef struct ipe_pkt_proc_drop_in_mpls_cnt_s ipe_pkt_proc_drop_in_mpls_cnt_t;

struct ipe_pkt_proc_parity_fail_record_s	/* 537 */
{

	uint32 rsv_0	:15;
	uint32 cla_cos_map_tab_parity_fail	:1;
	uint32 rsv_1	:11;
	uint32 cla_cos_map_tab_parity_fail_addr	:5;

	uint32 rsv_2	:15;
	uint32 cla_dscp_map_tab_parity_fail	:1;
	uint32 rsv_3	:9;
	uint32 cla_dscp_map_tab_parity_fail_addr	:7;

	uint32 rsv_4	:15;
	uint32 cla_pre_map_tab_parity_fail	:1;
	uint32 rsv_5	:12;
	uint32 cla_pre_map_tab_parity_fail_addr	:4;

	uint32 rsv_6	:15;
	uint32 ds_bidi_pim_group_parity_fail	:1;
	uint32 rsv_7	:6;
	uint32 ds_bidi_pim_group_parity_fail_addr	:10;

	uint32 rsv_8	:15;
	uint32 ds_pbb_mac_tab_parity_fail	:1;
	uint32 rsv_9	:6;
	uint32 ds_pbb_mac_tab_parity_fail_addr	:10;
};
typedef struct ipe_pkt_proc_parity_fail_record_s ipe_pkt_proc_parity_fail_record_t;

struct ipe_stats_ctl_ipe_phb_intf_s	/* 538 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_ipe_phb_intf	:1;
	uint32 stats_hold_ipe_phb_intf	:1;
	uint32 clear_on_read_ipe_phb_intf	:1;
	uint32 stats_base_ptr_ipe_phb_intf	:16;
};
typedef struct ipe_stats_ctl_ipe_phb_intf_s ipe_stats_ctl_ipe_phb_intf_t;

struct ipe_stats_init_ipe_phb_intf_s	/* 539 */
{

	uint32 rsv_0	:31;
	uint32 init_ipe_phb_intf	:1;
};
typedef struct ipe_stats_init_ipe_phb_intf_s ipe_stats_init_ipe_phb_intf_t;

struct ipe_stats_init_done_ipe_phb_intf_s	/* 540 */
{

	uint32 rsv_0	:31;
	uint32 init_done_ipe_phb_intf	:1;
};
typedef struct ipe_stats_init_done_ipe_phb_intf_s ipe_stats_init_done_ipe_phb_intf_t;

struct ipe_stats_ctl_ipe_overall_fwd_s	/* 541 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_ipe_overall_fwd	:1;
	uint32 stats_hold_ipe_overall_fwd	:1;
	uint32 clear_on_read_ipe_overall_fwd	:1;
	uint32 stats_base_ptr_ipe_overall_fwd	:16;
};
typedef struct ipe_stats_ctl_ipe_overall_fwd_s ipe_stats_ctl_ipe_overall_fwd_t;

struct ipe_stats_init_ipe_overall_fwd_s	/* 542 */
{

	uint32 rsv_0	:31;
	uint32 init_ipe_overall_fwd	:1;
};
typedef struct ipe_stats_init_ipe_overall_fwd_s ipe_stats_init_ipe_overall_fwd_t;

struct ipe_stats_init_done_ipe_overall_fwd_s	/* 543 */
{

	uint32 rsv_0	:31;
	uint32 init_done_ipe_overall_fwd	:1;
};
typedef struct ipe_stats_init_done_ipe_overall_fwd_s ipe_stats_init_done_ipe_overall_fwd_t;

struct ipe_stats_intr_value_set_s	/* 544 */
{

	uint32 rsv_0	:14;
	uint32 intr_value_set	:18;
};
typedef struct ipe_stats_intr_value_set_s ipe_stats_intr_value_set_t;

struct ipe_stats_intr_value_reset_s	/* 545 */
{

	uint32 rsv_0	:14;
	uint32 intr_value_reset	:18;
};
typedef struct ipe_stats_intr_value_reset_s ipe_stats_intr_value_reset_t;

struct ipe_stats_intr_mask_set_s	/* 546 */
{

	uint32 rsv_0	:14;
	uint32 intr_mask_set	:18;
};
typedef struct ipe_stats_intr_mask_set_s ipe_stats_intr_mask_set_t;

struct ipe_stats_intr_mask_reset_s	/* 547 */
{

	uint32 rsv_0	:14;
	uint32 intr_mask_reset	:18;
};
typedef struct ipe_stats_intr_mask_reset_s ipe_stats_intr_mask_reset_t;

struct ipe_stats_debug_stats_s	/* 548 */
{

	uint32 rsv_0	:4;
	uint32 port_log_drop_cnt	:4;
	uint32 rsv_1	:4;
	uint32 fwd_drop_cnt	:4;
	uint32 rsv_2	:4;
	uint32 intf_drop_cnt	:4;
	uint32 rsv_3	:4;
	uint32 phb_drop_cnt	:4;
};
typedef struct ipe_stats_debug_stats_s ipe_stats_debug_stats_t;

struct ipe_stats_ctl_ipe_port_log_s	/* 549 */
{

	uint32 rsv_0	:13;
	uint32 saturate_en_ipe_port_log	:1;
	uint32 stats_hold_ipe_port_log	:1;
	uint32 clear_on_read_ipe_port_log	:1;
	uint32 stats_base_ptr_ipe_port_log	:16;
};
typedef struct ipe_stats_ctl_ipe_port_log_s ipe_stats_ctl_ipe_port_log_t;

struct ipe_stats_init_ipe_port_log_s	/* 550 */
{

	uint32 rsv_0	:31;
	uint32 init_ipe_port_log	:1;
};
typedef struct ipe_stats_init_ipe_port_log_s ipe_stats_init_ipe_port_log_t;

struct ipe_stats_init_done_ipe_port_log_s	/* 551 */
{

	uint32 rsv_0	:31;
	uint32 init_done_ipe_port_log	:1;
};
typedef struct ipe_stats_init_done_ipe_port_log_s ipe_stats_init_done_ipe_port_log_t;

struct mac_mux_calendar_s	/* 552 */
{

	uint32 rsv_0	:1;
	uint32 cal_entry07	:3;
	uint32 rsv_1	:1;
	uint32 cal_entry06	:3;
	uint32 rsv_2	:1;
	uint32 cal_entry05	:3;
	uint32 rsv_3	:1;
	uint32 cal_entry04	:3;
	uint32 rsv_4	:1;
	uint32 cal_entry03	:3;
	uint32 rsv_5	:1;
	uint32 cal_entry02	:3;
	uint32 rsv_6	:1;
	uint32 cal_entry01	:3;
	uint32 rsv_7	:1;
	uint32 cal_entry00	:3;

	uint32 rsv_8	:1;
	uint32 cal_entry15	:3;
	uint32 rsv_9	:1;
	uint32 cal_entry14	:3;
	uint32 rsv_10	:1;
	uint32 cal_entry13	:3;
	uint32 rsv_11	:1;
	uint32 cal_entry12	:3;
	uint32 rsv_12	:1;
	uint32 cal_entry11	:3;
	uint32 rsv_13	:1;
	uint32 cal_entry10	:3;
	uint32 rsv_14	:1;
	uint32 cal_entry09	:3;
	uint32 rsv_15	:1;
	uint32 cal_entry08	:3;

	uint32 rsv_16	:1;
	uint32 cal_entry23	:3;
	uint32 rsv_17	:1;
	uint32 cal_entry22	:3;
	uint32 rsv_18	:1;
	uint32 cal_entry21	:3;
	uint32 rsv_19	:1;
	uint32 cal_entry20	:3;
	uint32 rsv_20	:1;
	uint32 cal_entry19	:3;
	uint32 rsv_21	:1;
	uint32 cal_entry18	:3;
	uint32 rsv_22	:1;
	uint32 cal_entry17	:3;
	uint32 rsv_23	:1;
	uint32 cal_entry16	:3;

	uint32 rsv_24	:1;
	uint32 cal_entry31	:3;
	uint32 rsv_25	:1;
	uint32 cal_entry30	:3;
	uint32 rsv_26	:1;
	uint32 cal_entry29	:3;
	uint32 rsv_27	:1;
	uint32 cal_entry28	:3;
	uint32 rsv_28	:1;
	uint32 cal_entry27	:3;
	uint32 rsv_29	:1;
	uint32 cal_entry26	:3;
	uint32 rsv_30	:1;
	uint32 cal_entry25	:3;
	uint32 rsv_31	:1;
	uint32 cal_entry24	:3;

	uint32 rsv_32	:1;
	uint32 cal_entry39	:3;
	uint32 rsv_33	:1;
	uint32 cal_entry38	:3;
	uint32 rsv_34	:1;
	uint32 cal_entry37	:3;
	uint32 rsv_35	:1;
	uint32 cal_entry36	:3;
	uint32 rsv_36	:1;
	uint32 cal_entry35	:3;
	uint32 rsv_37	:1;
	uint32 cal_entry34	:3;
	uint32 rsv_38	:1;
	uint32 cal_entry33	:3;
	uint32 rsv_39	:1;
	uint32 cal_entry32	:3;

	uint32 rsv_40	:1;
	uint32 cal_entry47	:3;
	uint32 rsv_41	:1;
	uint32 cal_entry46	:3;
	uint32 rsv_42	:1;
	uint32 cal_entry45	:3;
	uint32 rsv_43	:1;
	uint32 cal_entry44	:3;
	uint32 rsv_44	:1;
	uint32 cal_entry43	:3;
	uint32 rsv_45	:1;
	uint32 cal_entry42	:3;
	uint32 rsv_46	:1;
	uint32 cal_entry41	:3;
	uint32 rsv_47	:1;
	uint32 cal_entry40	:3;

	uint32 rsv_48	:17;
	uint32 cal_entry51	:3;
	uint32 rsv_49	:1;
	uint32 cal_entry50	:3;
	uint32 rsv_50	:1;
	uint32 cal_entry49	:3;
	uint32 rsv_51	:1;
	uint32 cal_entry48	:3;
};
typedef struct mac_mux_calendar_s mac_mux_calendar_t;

struct mac_mux_walker_s	/* 553 */
{

	uint32 walker_stop	:1;
	uint32 rsv_0	:17;
	uint32 walker_stop_idx	:6;
	uint32 rsv_1	:2;
	uint32 walker_end	:6;
};
typedef struct mac_mux_walker_s mac_mux_walker_t;

struct mac_mux_stat_sel_s	/* 554 */
{

	uint32 stat_sel0	:32;

	uint32 rsv_0	:12;
	uint32 stat_sel1	:20;
};
typedef struct mac_mux_stat_sel_s mac_mux_stat_sel_t;

struct mac_mux_debug_stats_s	/* 555 */
{

	uint32 rsv_0	:28;
	uint32 sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 data_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 pkt_error_cnt	:4;

	uint32 rsv_4	:24;
	uint32 data_bytes_cnt	:8;
};
typedef struct mac_mux_debug_stats_s mac_mux_debug_stats_t;

struct mux_agg0_interrupt_fatal_s	/* 556 */
{

	uint32 rsv_0	:8;
	uint32 value_set0_interrupt_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0_interrupt_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0_interrupt_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0_interrupt_fatal	:24;
};
typedef struct mux_agg0_interrupt_fatal_s mux_agg0_interrupt_fatal_t;

struct mux_agg1_interrupt_fatal_s	/* 557 */
{

	uint32 rsv_0	:8;
	uint32 value_set1_interrupt_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset1_interrupt_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set1_interrupt_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset1_interrupt_fatal	:24;
};
typedef struct mux_agg1_interrupt_fatal_s mux_agg1_interrupt_fatal_t;

struct mux_agg2_interrupt_fatal_s	/* 558 */
{

	uint32 rsv_0	:8;
	uint32 value_set2_interrupt_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset2_interrupt_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set2_interrupt_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset2_interrupt_fatal	:24;
};
typedef struct mux_agg2_interrupt_fatal_s mux_agg2_interrupt_fatal_t;

struct mux_agg3_interrupt_fatal_s	/* 559 */
{

	uint32 rsv_0	:8;
	uint32 value_set3_interrupt_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset3_interrupt_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set3_interrupt_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset3_interrupt_fatal	:24;
};
typedef struct mux_agg3_interrupt_fatal_s mux_agg3_interrupt_fatal_t;

struct mux_agg_parity_enable_s	/* 560 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct mux_agg_parity_enable_s mux_agg_parity_enable_t;

struct met_fifo_ctl_s	/* 561 */
{

	uint32 ds_met_entry_base	:12;
	uint32 rsv_0	:7;
	uint32 humber_id	:5;
	uint32 local_bay_queue_on_bay_id	:1;
	uint32 from_fabric_multicast_en	:1;
	uint32 rsv_1	:2;
	uint32 port_check_en	:1;
	uint32 discard_met_loop	:1;
	uint32 ether_oam_mutilcast_en	:1;
	uint32 rsv_2	:1;

	uint32 rsv_3	:16;
	uint32 local_met_base	:16;
};
typedef struct met_fifo_ctl_s met_fifo_ctl_t;

struct met_fifo_force_use_rcd_ram_s	/* 562 */
{

	uint32 rsv_0	:31;
	uint32 force_use_rcd_ram	:1;
};
typedef struct met_fifo_force_use_rcd_ram_s met_fifo_force_use_rcd_ram_t;

struct met_fifo_wrr_ctl_s	/* 563 */
{

	uint32 rsv_0	:3;
	uint32 max_pending_mcast	:5;
	uint32 ucast_hi_weight	:6;
	uint32 ucast_lo_weight	:6;
	uint32 mcast_hi_weight	:6;
	uint32 mcast_lo_weight	:6;
};
typedef struct met_fifo_wrr_ctl_s met_fifo_wrr_ctl_t;

struct met_fifo_input_fifo_threshold_s	/* 564 */
{

	uint32 ucast_hi_threshold	:8;
	uint32 ucast_lo_threshold	:8;
	uint32 mcast_hi_threshold	:8;
	uint32 mcast_lo_threshold	:8;

	uint32 ucast_hi_half_threshold	:8;
	uint32 ucast_lo_half_threshold	:8;
	uint32 mcast_hi_half_threshold	:8;
	uint32 mcast_lo_half_threshold	:8;
};
typedef struct met_fifo_input_fifo_threshold_s met_fifo_input_fifo_threshold_t;

struct met_fifo_start_ptr_s	/* 565 */
{

	uint32 rsv_0	:4;
	uint32 ucast_hi_start_ptr	:12;
	uint32 rsv_1	:4;
	uint32 ucast_lo_start_ptr	:12;

	uint32 rsv_2	:4;
	uint32 mcast_hi_start_ptr	:12;
	uint32 rsv_3	:4;
	uint32 mcast_lo_start_ptr	:12;
};
typedef struct met_fifo_start_ptr_s met_fifo_start_ptr_t;

struct met_fifo_end_ptr_s	/* 566 */
{

	uint32 rsv_0	:4;
	uint32 ucast_hi_end_ptr	:12;
	uint32 rsv_1	:4;
	uint32 ucast_lo_end_ptr	:12;

	uint32 rsv_2	:4;
	uint32 mcast_hi_end_ptr	:12;
	uint32 rsv_3	:4;
	uint32 mcast_lo_end_ptr	:12;
};
typedef struct met_fifo_end_ptr_s met_fifo_end_ptr_t;

struct met_fifo_input_fifo_depth_s	/* 567 */
{

	uint32 rsv_0	:3;
	uint32 ucast_hi_fifo_depth	:13;
	uint32 rsv_1	:3;
	uint32 ucast_lo_fifo_depth	:13;

	uint32 rsv_2	:3;
	uint32 mcast_hi_fifo_depth	:13;
	uint32 rsv_3	:3;
	uint32 mcast_lo_fifo_depth	:13;
};
typedef struct met_fifo_input_fifo_depth_s met_fifo_input_fifo_depth_t;

struct met_fifo_init_s	/* 568 */
{

	uint32 rsv_0	:31;
	uint32 met_fifo_init	:1;
};
typedef struct met_fifo_init_s met_fifo_init_t;

struct met_fifo_init_done_s	/* 569 */
{

	uint32 rsv_0	:31;
	uint32 met_fifo_init	:1;
};
typedef struct met_fifo_init_done_s met_fifo_init_done_t;

struct met_fifo_max_init_cnt_s	/* 570 */
{

	uint32 rsv_0	:17;
	uint32 max_init_cnt	:15;
};
typedef struct met_fifo_max_init_cnt_s met_fifo_max_init_cnt_t;

struct met_fifo_mcast_enable_s	/* 571 */
{

	uint32 rsv_0	:31;
	uint32 mcast_met_fifo_enable	:1;
};
typedef struct met_fifo_mcast_enable_s met_fifo_mcast_enable_t;

struct met_fifo_msg_cnt_s	/* 572 */
{

	uint32 rsv_0	:3;
	uint32 ucast_hi_msg_cnt	:13;
	uint32 rsv_1	:3;
	uint32 ucast_lo_msg_cnt	:13;

	uint32 rsv_2	:3;
	uint32 mcast_hi_msg_cnt	:13;
	uint32 rsv_3	:3;
	uint32 mcast_lo_msg_cnt	:13;
};
typedef struct met_fifo_msg_cnt_s met_fifo_msg_cnt_t;

struct met_fifo_wr_ptr_s	/* 573 */
{

	uint32 rsv_0	:4;
	uint32 ucast_hi_wr_ptr	:12;
	uint32 rsv_1	:4;
	uint32 ucast_lo_wr_ptr	:12;

	uint32 rsv_2	:4;
	uint32 mcast_hi_wr_ptr	:12;
	uint32 rsv_3	:4;
	uint32 mcast_lo_wr_ptr	:12;
};
typedef struct met_fifo_wr_ptr_s met_fifo_wr_ptr_t;

struct met_fifo_rd_ptr_s	/* 574 */
{

	uint32 rsv_0	:4;
	uint32 ucast_hi_rd_ptr	:12;
	uint32 rsv_1	:4;
	uint32 ucast_lo_rd_ptr	:12;

	uint32 rsv_2	:4;
	uint32 mcast_hi_rd_ptr	:12;
	uint32 rsv_3	:4;
	uint32 mcast_lo_rd_ptr	:12;
};
typedef struct met_fifo_rd_ptr_s met_fifo_rd_ptr_t;

struct met_fifo_wrr_weight_cnt_s	/* 575 */
{

	uint32 rsv_0	:2;
	uint32 ucast_hi_weight_cnt	:6;
	uint32 rsv_1	:2;
	uint32 ucast_lo_weight_cnt	:6;
	uint32 rsv_2	:2;
	uint32 mcast_hi_weight_cnt	:6;
	uint32 rsv_3	:2;
	uint32 mcast_lo_weight_cnt	:6;
};
typedef struct met_fifo_wrr_weight_cnt_s met_fifo_wrr_weight_cnt_t;

struct met_fifo_pending_mcast_cnt_s	/* 576 */
{

	uint32 rsv_0	:27;
	uint32 pending_mcast_cnt	:5;
};
typedef struct met_fifo_pending_mcast_cnt_s met_fifo_pending_mcast_cnt_t;

struct met_fifo_update_rcd_error_spot_s	/* 577 */
{

	uint32 es_mcast_rcd	:1;
	uint32 rsv_0	:1;
	uint32 es_buffer_count	:6;
	uint32 es_resource_group_id	:8;
	uint32 rsv_1	:1;
	uint32 es_head_buffer_ptr	:15;

	uint32 es_qmgr_flag	:1;
	uint32 rsv_2	:15;
	uint32 es_rcd_ram_rcd	:8;
	uint32 es_update_rcd	:8;
};
typedef struct met_fifo_update_rcd_error_spot_s met_fifo_update_rcd_error_spot_t;

struct met_fifo_en_que_credit_s	/* 578 */
{

	uint32 rsv_0	:27;
	uint32 en_que_credit	:5;
};
typedef struct met_fifo_en_que_credit_s met_fifo_en_que_credit_t;

struct met_fifo_tb_info_arb_credit_s	/* 579 */
{

	uint32 rsv_0	:27;
	uint32 tb_info_arb_credit	:5;
};
typedef struct met_fifo_tb_info_arb_credit_s met_fifo_tb_info_arb_credit_t;

struct met_fifoq_mgr_credit_s	/* 580 */
{

	uint32 rsv_0	:27;
	uint32 q_mgr_credit	:5;
};
typedef struct met_fifoq_mgr_credit_s met_fifoq_mgr_credit_t;

struct met_fifo_parity_enable_s	/* 581 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct met_fifo_parity_enable_s met_fifo_parity_enable_t;

struct met_fifoq_write_rcd_upd_fifo_thrd_s	/* 582 */
{

	uint32 rsv_0	:17;
	uint32 q_rcd_upd_fifo_hi_thrd	:7;
	uint32 rsv_1	:1;
	uint32 q_rcd_upd_fifo_low_thrd	:7;
};
typedef struct met_fifoq_write_rcd_upd_fifo_thrd_s met_fifoq_write_rcd_upd_fifo_thrd_t;

struct met_fifo_drain_enable_s	/* 583 */
{

	uint32 rsv_0	:31;
	uint32 met_fifo_drain_enable	:1;
};
typedef struct met_fifo_drain_enable_s met_fifo_drain_enable_t;

struct met_fifo_wrr_wt_cfg_s	/* 584 */
{

	uint32 rsv_0	:18;
	uint32 ucast_wt_cfg	:6;
	uint32 rsv_1	:2;
	uint32 mcast_wt_cfg	:6;
};
typedef struct met_fifo_wrr_wt_cfg_s met_fifo_wrr_wt_cfg_t;

struct met_fifo_wrr_wt_s	/* 585 */
{

	uint32 rsv_0	:18;
	uint32 ucast_wt	:6;
	uint32 rsv_1	:2;
	uint32 mcast_wt	:6;
};
typedef struct met_fifo_wrr_wt_s met_fifo_wrr_wt_t;

struct met_fifo_rd_cur_state_machine_s	/* 586 */
{

	uint32 rsv_0	:28;
	uint32 rd_cur_state_machine	:4;
};
typedef struct met_fifo_rd_cur_state_machine_s met_fifo_rd_cur_state_machine_t;

struct met_fifo_input_en_que_msg_stats_s	/* 587 */
{

	uint32 rsv_0	:28;
	uint32 input_enque_msg_cnt	:4;
};
typedef struct met_fifo_input_en_que_msg_stats_s met_fifo_input_en_que_msg_stats_t;

struct met_fifo_output_en_que_msg_stats_s	/* 588 */
{

	uint32 rsv_0	:28;
	uint32 output_enque_msg_cnt	:4;
};
typedef struct met_fifo_output_en_que_msg_stats_s met_fifo_output_en_que_msg_stats_t;

struct met_fifo_inputq_write_rcd_update_stats_s	/* 589 */
{

	uint32 rsv_0	:12;
	uint32 inputq_write_rcd_update_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_buf_retrv_rcd_update_cnt	:4;
};
typedef struct met_fifo_inputq_write_rcd_update_stats_s met_fifo_inputq_write_rcd_update_stats_t;

struct met_fifo_output_free_buf_msg_stats_s	/* 590 */
{

	uint32 rsv_0	:28;
	uint32 output_free_buf_msg_cnt	:4;
};
typedef struct met_fifo_output_free_buf_msg_stats_s met_fifo_output_free_buf_msg_stats_t;

struct met_fifo_output_tb_info_req_stats_s	/* 591 */
{

	uint32 rsv_0	:28;
	uint32 output_tb_info_req_cnt	:4;
};
typedef struct met_fifo_output_tb_info_req_stats_s met_fifo_output_tb_info_req_stats_t;

struct met_fifo_input_tb_info_arb_done_stats_s	/* 592 */
{

	uint32 rsv_0	:28;
	uint32 input_tb_info_arb_done_cnt	:4;
};
typedef struct met_fifo_input_tb_info_arb_done_stats_s met_fifo_input_tb_info_arb_done_stats_t;

struct met_fifo_input_tb_info_rd_valid_stats_s	/* 593 */
{

	uint32 rsv_0	:28;
	uint32 input_tb_info_rd_valid_cnt	:4;
};
typedef struct met_fifo_input_tb_info_rd_valid_stats_s met_fifo_input_tb_info_rd_valid_stats_t;

struct met_fifo_inputq_write_met_fifo_done_stats_s	/* 594 */
{

	uint32 rsv_0	:28;
	uint32 inputq_write_met_fifo_done_cnt	:4;
};
typedef struct met_fifo_inputq_write_met_fifo_done_stats_s met_fifo_inputq_write_met_fifo_done_stats_t;

struct met_fifo_output_rcd_done_stats_s	/* 595 */
{

	uint32 rsv_0	:12;
	uint32 output_rcdq_write_done_cnt	:4;
	uint32 rsv_1	:12;
	uint32 output_rcd_buf_retrv_done_cnt	:4;
};
typedef struct met_fifo_output_rcd_done_stats_s met_fifo_output_rcd_done_stats_t;

struct met_fifo_output_enque_discard_stats_s	/* 596 */
{

	uint32 rsv_0	:28;
	uint32 output_enque_discard_cnt	:4;
};
typedef struct met_fifo_output_enque_discard_stats_s met_fifo_output_enque_discard_stats_t;

struct met_fifo_output_free_buf_cnt_s	/* 597 */
{

	uint32 rsv_0	:28;
	uint32 output_free_buf_cnt	:4;
};
typedef struct met_fifo_output_free_buf_cnt_s met_fifo_output_free_buf_cnt_t;

struct met_fifo_output_met_fifo_buf_cnt_s	/* 598 */
{

	uint32 rsv_0	:28;
	uint32 output_met_fifo_buf_cnt	:4;
};
typedef struct met_fifo_output_met_fifo_buf_cnt_s met_fifo_output_met_fifo_buf_cnt_t;

struct met_fifo_input_upd_buf_cnt_s	/* 599 */
{

	uint32 rsv_0	:12;
	uint32 inputq_mgr_upd_buf_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_buf_retrv_upd_buf_cnt	:4;
};
typedef struct met_fifo_input_upd_buf_cnt_s met_fifo_input_upd_buf_cnt_t;

struct met_fifo_input_buf_store_buf_cnt_s	/* 600 */
{

	uint32 rsv_0	:28;
	uint32 input_buf_store_buf_cnt	:4;
};
typedef struct met_fifo_input_buf_store_buf_cnt_s met_fifo_input_buf_store_buf_cnt_t;

struct met_fifo_input_ucast_msg_cnt_s	/* 601 */
{

	uint32 rsv_0	:12;
	uint32 input_ucast_hi_msg_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_ucast_lo_msg_cnt	:4;
};
typedef struct met_fifo_input_ucast_msg_cnt_s met_fifo_input_ucast_msg_cnt_t;

struct met_fifo_input_mcast_msg_cnt_s	/* 602 */
{

	uint32 rsv_0	:12;
	uint32 input_mcast_hi_msg_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_mcast_lo_msg_cnt	:4;
};
typedef struct met_fifo_input_mcast_msg_cnt_s met_fifo_input_mcast_msg_cnt_t;

struct met_fifo_output_ucast_msg_cnt_s	/* 603 */
{

	uint32 rsv_0	:12;
	uint32 output_ucast_hi_msg_cnt	:4;
	uint32 rsv_1	:12;
	uint32 output_ucast_lo_msg_cnt	:4;
};
typedef struct met_fifo_output_ucast_msg_cnt_s met_fifo_output_ucast_msg_cnt_t;

struct met_fifo_output_mcast_msg_cnt_s	/* 604 */
{

	uint32 rsv_0	:12;
	uint32 output_mcast_hi_msg_cnt	:4;
	uint32 rsv_1	:12;
	uint32 output_mcast_lo_msg_cnt	:4;
};
typedef struct met_fifo_output_mcast_msg_cnt_s met_fifo_output_mcast_msg_cnt_t;

struct met_fifo_ecc_ctl_s	/* 605 */
{

	uint32 rsv_0	:23;
	uint32 cfg_report_single_bit_error	:1;
	uint32 rsv_1	:3;
	uint32 cfg_ecc_correct_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_ecc_check_en	:1;
};
typedef struct met_fifo_ecc_ctl_s met_fifo_ecc_ctl_t;

struct met_fifo_ecc_error_stats_s	/* 606 */
{

	uint32 rsv_0	:16;
	uint32 cfg_ecc_single_bit_count	:8;
	uint32 cfg_ecc_multiple_bit_count	:8;
};
typedef struct met_fifo_ecc_error_stats_s met_fifo_ecc_error_stats_t;

struct met_fifo_met_entry_ecc_discard_cnt_s	/* 607 */
{

	uint32 rsv_0	:28;
	uint32 met_entry_ecc_err_discard_cnt	:4;
};
typedef struct met_fifo_met_entry_ecc_discard_cnt_s met_fifo_met_entry_ecc_discard_cnt_t;

struct met_fifo_parity_fail_record_s	/* 608 */
{

	uint32 rsv_0	:15;
	uint32 ds_aps_bridge_mcast_parity_fail	:1;
	uint32 rsv_1	:5;
	uint32 ds_aps_bridge_mcast_parity_fail_addr	:11;

	uint32 rsv_2	:15;
	uint32 ds_link_agg_block_mask_parity_fail	:1;
	uint32 rsv_3	:10;
	uint32 ds_link_agg_block_mask_parity_fail_addr	:6;

	uint32 rsv_4	:15;
	uint32 ds_link_agg_bitmap_parity_fail	:1;
	uint32 rsv_5	:9;
	uint32 ds_link_agg_bitmap_parity_fail_addr	:7;
};
typedef struct met_fifo_parity_fail_record_s met_fifo_parity_fail_record_t;

struct met_fifo_interrupt_s	/* 609 */
{

	uint32 mask_set_fatal0	:32;

	uint32 mask_reset_fatal0	:32;

	uint32 value_set_fatal0	:32;

	uint32 value_reset_fatal0	:32;

	uint32 rsv_0	:28;
	uint32 mask_set_fatal1	:4;

	uint32 rsv_1	:28;
	uint32 mask_reset_fatal1	:4;

	uint32 rsv_2	:28;
	uint32 value_set_fatal1	:4;

	uint32 rsv_3	:28;
	uint32 value_reset_fatal1	:4;
};
typedef struct met_fifo_interrupt_s met_fifo_interrupt_t;

struct net_rx_drain_enable_s	/* 610 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct net_rx_drain_enable_s net_rx_drain_enable_t;

struct net_rx_ctl_s	/* 611 */
{

	uint32 rsv_0	:6;
	uint32 under_len_error_chk_enable	:1;
	uint32 over_len_error_chk_enable	:1;
	uint32 rsv_1	:4;
	uint32 fill_enable_hi	:20;

	uint32 fill_enable_lo	:32;
};
typedef struct net_rx_ctl_s net_rx_ctl_t;

struct net_rx_free_list_init_s	/* 612 */
{

	uint32 rsv_0	:31;
	uint32 free_list_init	:1;

	uint32 rsv_1	:31;
	uint32 free_list_init_done	:1;
};
typedef struct net_rx_free_list_init_s net_rx_free_list_init_t;

struct net_rx_free_list_ctl_s	/* 613 */
{

	uint32 rsv_0	:23;
	uint32 free_list_head_ptr	:9;

	uint32 rsv_1	:23;
	uint32 free_list_tail_ptr	:9;

	uint32 rsv_2	:23;
	uint32 free_list_buf_cnt	:9;
};
typedef struct net_rx_free_list_ctl_s net_rx_free_list_ctl_t;

struct net_rx_max_pkt_size_s	/* 614 */
{

	uint32 rsv_0	:2;
	uint32 max_pkt_size1	:14;
	uint32 rsv_1	:2;
	uint32 max_pkt_size0	:14;
};
typedef struct net_rx_max_pkt_size_s net_rx_max_pkt_size_t;

struct net_rx_min_pkt_size_s	/* 615 */
{

	uint32 rsv_0	:2;
	uint32 min_pkt_size1	:14;
	uint32 rsv_1	:2;
	uint32 min_pkt_size0	:14;
};
typedef struct net_rx_min_pkt_size_s net_rx_min_pkt_size_t;

struct net_rx_max_pkt_size_select_s	/* 616 */
{

    uint32 rsv_0	:12;
    uint32 max_pkt_sel_hi    :20;

    uint32 max_pkt_sel_lo   :32;
};
typedef struct net_rx_max_pkt_size_select_s net_rx_max_pkt_size_select_t;

struct net_rx_min_pkt_size_select_s	/* 617 */
{

	uint32 rsv_0	:12;
    uint32 min_pkt_sel_hi    :20;

    uint32 min_pkt_sel_lo   :32;
};
typedef struct net_rx_min_pkt_size_select_s net_rx_min_pkt_size_select_t;

struct net_rx_channel_drop_threshold_s	/* 618 */
{

	uint32 rsv_0	:7;
	uint32 channel_drop_threshold1	:9;
	uint32 rsv_1	:7;
	uint32 channel_drop_threshold0	:9;
};
typedef struct net_rx_channel_drop_threshold_s net_rx_channel_drop_threshold_t;

struct net_rx_channel_drop_threshold_select_s	/* 619 */
{

	uint32 rsv_0	:7;
	uint32 channel_drop_threshold1	:9;
	uint32 rsv_1	:7;
	uint32 channel_drop_threshold0	:9;

	uint32 rsv_2	:32;
};
typedef struct net_rx_channel_drop_threshold_select_s net_rx_channel_drop_threshold_select_t;

struct net_rx_channel_buffer_count_s	/* 620 */
{

	uint32 rsv_0	:23;
	uint32 channel_buffer_count0	:9;

	uint32 rsv_1	:23;
	uint32 channel_buffer_count1	:9;

	uint32 rsv_2	:23;
	uint32 channel_buffer_count2	:9;

	uint32 rsv_3	:23;
	uint32 channel_buffer_count3	:9;

	uint32 rsv_4	:23;
	uint32 channel_buffer_count4	:9;

	uint32 rsv_5	:23;
	uint32 channel_buffer_count5	:9;

	uint32 rsv_6	:23;
	uint32 channel_buffer_count6	:9;

	uint32 rsv_7	:23;
	uint32 channel_buffer_count7	:9;

	uint32 rsv_8	:23;
	uint32 channel_buffer_count8	:9;

	uint32 rsv_9	:23;
	uint32 channel_buffer_count9	:9;

	uint32 rsv_10	:23;
	uint32 channel_buffer_count10	:9;

	uint32 rsv_11	:23;
	uint32 channel_buffer_count11	:9;

	uint32 rsv_12	:23;
	uint32 channel_buffer_count12	:9;

	uint32 rsv_13	:23;
	uint32 channel_buffer_count13	:9;

	uint32 rsv_14	:23;
	uint32 channel_buffer_count14	:9;

	uint32 rsv_15	:23;
	uint32 channel_buffer_count15	:9;

	uint32 rsv_16	:23;
	uint32 channel_buffer_count16	:9;

	uint32 rsv_17	:23;
	uint32 channel_buffer_count17	:9;

	uint32 rsv_18	:23;
	uint32 channel_buffer_count18	:9;

	uint32 rsv_19	:23;
	uint32 channel_buffer_count19	:9;

	uint32 rsv_20	:23;
	uint32 channel_buffer_count20	:9;

	uint32 rsv_21	:23;
	uint32 channel_buffer_count21	:9;

	uint32 rsv_22	:23;
	uint32 channel_buffer_count22	:9;

	uint32 rsv_23	:23;
	uint32 channel_buffer_count23	:9;

	uint32 rsv_24	:23;
	uint32 channel_buffer_count24	:9;

	uint32 rsv_25	:23;
	uint32 channel_buffer_count25	:9;

	uint32 rsv_26	:23;
	uint32 channel_buffer_count26	:9;

	uint32 rsv_27	:23;
	uint32 channel_buffer_count27	:9;

	uint32 rsv_28	:23;
	uint32 channel_buffer_count28	:9;

	uint32 rsv_29	:23;
	uint32 channel_buffer_count29	:9;

	uint32 rsv_30	:23;
	uint32 channel_buffer_count30	:9;

	uint32 rsv_31	:23;
	uint32 channel_buffer_count31	:9;

	uint32 rsv_32	:23;
	uint32 channel_buffer_count32	:9;

	uint32 rsv_33	:23;
	uint32 channel_buffer_count33	:9;

	uint32 rsv_34	:23;
	uint32 channel_buffer_count34	:9;

	uint32 rsv_35	:23;
	uint32 channel_buffer_count35	:9;

	uint32 rsv_36	:23;
	uint32 channel_buffer_count36	:9;

	uint32 rsv_37	:23;
	uint32 channel_buffer_count37	:9;

	uint32 rsv_38	:23;
	uint32 channel_buffer_count38	:9;

	uint32 rsv_39	:23;
	uint32 channel_buffer_count39	:9;

	uint32 rsv_40	:23;
	uint32 channel_buffer_count40	:9;

	uint32 rsv_41	:23;
	uint32 channel_buffer_count41	:9;

	uint32 rsv_42	:23;
	uint32 channel_buffer_count42	:9;

	uint32 rsv_43	:23;
	uint32 channel_buffer_count43	:9;

	uint32 rsv_44	:23;
	uint32 channel_buffer_count44	:9;

	uint32 rsv_45	:23;
	uint32 channel_buffer_count45	:9;

	uint32 rsv_46	:23;
	uint32 channel_buffer_count46	:9;

	uint32 rsv_47	:23;
	uint32 channel_buffer_count47	:9;

	uint32 rsv_48	:23;
	uint32 channel_buffer_count48	:9;

	uint32 rsv_49	:23;
	uint32 channel_buffer_count49	:9;

	uint32 rsv_50	:23;
	uint32 channel_buffer_count50	:9;

	uint32 rsv_51	:23;
	uint32 channel_buffer_count51	:9;
};
typedef struct net_rx_channel_buffer_count_s net_rx_channel_buffer_count_t;

struct net_rx_debug_stats_s	/* 621 */
{

	uint32 rsv_0	:28;
	uint32 channel_stat_in_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 channel_stat_in_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 channel_stat_in_pkt_cnt	:4;

	uint32 rsv_3	:28;
	uint32 channel_stat_in_pkt_good_cnt	:4;

	uint32 rsv_4	:28;
	uint32 channel_stat_in_pkt_error_cnt	:4;

	uint32 rsv_5	:28;
	uint32 channel_stat_underlen_error_cnt	:4;

	uint32 rsv_6	:28;
	uint32 channel_stat_overlen_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 channel_stat_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 channel_stat_pkt_error_cnt	:4;

	uint32 rsv_9	:28;
	uint32 channel_stat_no_sop_error_cnt	:4;

	uint32 rsv_10	:28;
	uint32 channel_stat_no_eop_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 channel_stat_out_sop_cnt	:4;

	uint32 rsv_12	:28;
	uint32 channel_stat_out_eop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 channel_stat_out_pkt_cnt	:4;

	uint32 rsv_14	:28;
	uint32 channel_stat_out_pkt_error_cnt	:4;

	uint32 rsv_15	:24;
	uint32 stat_cur_pkt_cnt	:8;
};
typedef struct net_rx_debug_stats_s net_rx_debug_stats_t;

struct net_rx_channel_stat_select_s	/* 622 */
{

	uint32 rsv_0	:12;
	uint32 stats_sel_hi	:20;

	uint32 stats_sel_lo	:32;
};
typedef struct net_rx_channel_stat_select_s net_rx_channel_stat_select_t;

struct net_rx_channel_info_ctl_s	/* 623 */
{

	uint32 rsv_0	:31;
	uint32 channel_info_init	:1;

	uint32 rsv_1	:31;
	uint32 channel_info_init_done	:1;
};
typedef struct net_rx_channel_info_ctl_s net_rx_channel_info_ctl_t;

struct net_rx_buffer_size_s	/* 624 */
{

	uint32 rsv_0	:23;
	uint32 net_rx_buffer_size	:9;
};
typedef struct net_rx_buffer_size_s net_rx_buffer_size_t;

struct net_rx_buffer_count_s	/* 625 */
{

	uint32 rsv_0	:23;
	uint32 net_rx_buffer_count	:9;
};
typedef struct net_rx_buffer_count_s net_rx_buffer_count_t;

struct net_rx_buffer_almost_full_threshold_s	/* 626 */
{

	uint32 rsv_0	:7;
	uint32 net_rx_buffer_almost_full_threshold_high	:9;
	uint32 rsv_1	:7;
	uint32 net_rx_buffer_almost_full_threshold_low	:9;
};
typedef struct net_rx_buffer_almost_full_threshold_s net_rx_buffer_almost_full_threshold_t;

struct net_rx_buffer_full_threshold_s	/* 627 */
{

	uint32 rsv_0	:7;
	uint32 net_rx_buffer_full_threshold_high	:9;
	uint32 rsv_1	:7;
	uint32 net_rx_buffer_full_threshold_low	:9;
};
typedef struct net_rx_buffer_full_threshold_s net_rx_buffer_full_threshold_t;

struct net_rx_interrupt_s	/* 628 */
{

	uint32 rsv_0	:23;
	uint32 mask_set_fatal	:9;

	uint32 rsv_1	:23;
	uint32 mask_reset_fatal	:9;

	uint32 rsv_2	:23;
	uint32 value_set_fatal	:9;

	uint32 rsv_3	:23;
	uint32 value_reset_fatal	:9;
};
typedef struct net_rx_interrupt_s net_rx_interrupt_t;

struct net_rx_bpdu_ctl_s	/* 629 */
{

	uint32 bpdu_check_enable_lo	:32;

	uint32 rsv_0	:12;
	uint32 bpdu_check_enable_hi	:20;
};
typedef struct net_rx_bpdu_ctl_s net_rx_bpdu_ctl_t;

struct net_rx_fair_drop_ctl_s	/* 630 */
{

	uint32 rsv_0	:31;
	uint32 net_rx_fair_drop_enable	:1;
};
typedef struct net_rx_fair_drop_ctl_s net_rx_fair_drop_ctl_t;

struct net_rx_state_s	/* 631 */
{

	uint32 rsv_0	:27;
	uint32 rd_ctl_fsm_ps	:5;
};
typedef struct net_rx_state_s net_rx_state_t;

struct net_rx_reserved_mac_da_ctl_s	/* 632 */
{

	uint32 rsv_0	:16;
	uint32 reserved_mac_da_value0	:16;

	uint32 reserved_mac_da_value1	:32;

	uint32 rsv_1	:16;
	uint32 reserved_mac_da_mask0	:16;

	uint32 reserved_mac_da_mask1	:32;
};
typedef struct net_rx_reserved_mac_da_ctl_s net_rx_reserved_mac_da_ctl_t;

struct net_rx_bpdu_stats_s	/* 633 */
{

	uint32 rsv_0	:16;
	uint32 bpdu_pkt_in_cnt	:16;

	uint32 rsv_1	:16;
	uint32 bpdu_pkt_drop_cnt	:16;
};
typedef struct net_rx_bpdu_stats_s net_rx_bpdu_stats_t;

struct net_rx_pre_fetch_fifo_depth_s	/* 634 */
{

	uint32 rsv_0	:29;
	uint32 free_buf_fifo_depth	:3;
};
typedef struct net_rx_pre_fetch_fifo_depth_s net_rx_pre_fetch_fifo_depth_t;

struct net_rx_parity_fail_record_s	/* 635 */
{

	uint32 rsv_0	:15;
	uint32 pkt_buf_parity_fail	:1;
	uint32 rsv_1	:7;
	uint32 pkt_buf_parity_fail_addr	:9;
};
typedef struct net_rx_parity_fail_record_s net_rx_parity_fail_record_t;

struct net_tx_interrupt_s	/* 636 */
{

	uint32 rsv_0	:27;
	uint32 value_set	:5;

	uint32 rsv_1	:27;
	uint32 value_reset	:5;

	uint32 rsv_2	:27;
	uint32 mask_set	:5;

	uint32 rsv_3	:27;
	uint32 mask_reset	:5;
};
typedef struct net_tx_interrupt_s net_tx_interrupt_t;

struct net_tx_channel_en_s	/* 637 */
{

	uint32 rsv_0	:11;
	uint32 e_loop_ch_en	:1;
	uint32 ch_en_ch51_to32	:20;

	uint32 ch_en_ch31_to0	:32;
};
typedef struct net_tx_channel_en_s net_tx_channel_en_t;

struct net_tx_channel_tx_en_s	/* 638 */
{

	uint32 rsv_0	:11;
	uint32 e_loop_ch_tx_en	:1;
	uint32 ch_tx_en_ch51_to32	:20;

	uint32 ch_tx_en_ch31_to0	:32;
};
typedef struct net_tx_channel_tx_en_s net_tx_channel_tx_en_t;

struct net_tx_init_s	/* 639 */
{

	uint32 rsv_0	:31;
	uint32 net_tx_init	:1;
};
typedef struct net_tx_init_s net_tx_init_t;

struct net_tx_init_done_s	/* 640 */
{

	uint32 rsv_0	:31;
	uint32 net_tx_init	:1;
};
typedef struct net_tx_init_done_s net_tx_init_done_t;

struct net_tx_pkt_mem_segment_s	/* 641 */
{

	uint32 rsv_0	:29;
	uint32 pkt_mem_segment	:3;
};
typedef struct net_tx_pkt_mem_segment_s net_tx_pkt_mem_segment_t;

struct net_tx_parity_enable_s	/* 642 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct net_tx_parity_enable_s net_tx_parity_enable_t;

struct net_tx_stat_sel_s	/* 643 */
{

	uint32 rsv_0	:11;
	uint32 stat_sele_loop	:1;
	uint32 stat_sel_ch51_to32	:20;

	uint32 stat_sel_ch31_to0	:32;
};
typedef struct net_tx_stat_sel_s net_tx_stat_sel_t;

struct net_tx_cfg_max_chan_id_s	/* 644 */
{

	uint32 rsv_0	:24;
	uint32 max_chan_id_cfg	:8;
};
typedef struct net_tx_cfg_max_chan_id_s net_tx_cfg_max_chan_id_t;

struct net_tx_xgmac_alternative_en_s	/* 645 */
{

	uint32 rsv_0	:28;
	uint32 xgmac3_en	:1;
	uint32 xgmac2_en	:1;
	uint32 xgmac1_en	:1;
	uint32 xgmac0_en	:1;
};
typedef struct net_tx_xgmac_alternative_en_s net_tx_xgmac_alternative_en_t;

struct net_txe_loop_stall_record_s	/* 646 */
{

	uint32 rsv_0	:27;
	uint32 e_loop_stall_record_snap	:1;
	uint32 rsv_1	:3;
	uint32 e_loop_stall_record	:1;
};
typedef struct net_txe_loop_stall_record_s net_txe_loop_stall_record_t;

struct net_tx_epe_stall_en_s	/* 647 */
{

	uint32 rsv_0	:31;
	uint32 net_tx_epe_stall_en	:1;
};
typedef struct net_tx_epe_stall_en_s net_tx_epe_stall_en_t;

struct net_tx_stall_thrd_s	/* 648 */
{

	uint32 rsv_0	:28;
	uint32 net_tx_stall_thrd	:4;
};
typedef struct net_tx_stall_thrd_s net_tx_stall_thrd_t;

struct net_tx_stall_record_s	/* 649 */
{

	uint32 net_tx_stall_snap_en	:1;
	uint32 rsv_0	:30;
	uint32 net_tx_stall_record	:1;
};
typedef struct net_tx_stall_record_s net_tx_stall_record_t;

struct net_tx_parity_fail_record_s	/* 650 */
{

	uint32 rsv_0	:15;
	uint32 pkt_mem_parity_fail	:1;
	uint32 rsv_1	:5;
	uint32 pkt_mem_parity_fail_addr	:11;
};
typedef struct net_tx_parity_fail_record_s net_tx_parity_fail_record_t;

struct net_tx_pkt_auto_gen_config_s	/* 651 */
{

	uint32 pkt_auto_gen_en	:1;
	uint32 rsv_0	:25;
	uint32 pkt_auto_gen_ch_id	:6;
};
typedef struct net_tx_pkt_auto_gen_config_s net_tx_pkt_auto_gen_config_t;

struct net_tx_pkt_auto_gen_burst_cnt_s	/* 652 */
{

	uint32 pkt_auto_gen_burst_cnt	:32;
};
typedef struct net_tx_pkt_auto_gen_burst_cnt_s net_tx_pkt_auto_gen_burst_cnt_t;

struct net_tx_pkt_auto_gen_data_s	/* 653 */
{

	uint32 pkt_auto_gen_data0	:32;

	uint32 pkt_auto_gen_data1	:32;

	uint32 pkt_auto_gen_data2	:32;

	uint32 pkt_auto_gen_data3	:32;

	uint32 pkt_auto_gen_data4	:32;

	uint32 pkt_auto_gen_data5	:32;

	uint32 pkt_auto_gen_data6	:32;

	uint32 pkt_auto_gen_data7	:32;

	uint32 pkt_auto_gen_data8	:32;

	uint32 pkt_auto_gen_data9	:32;

	uint32 pkt_auto_gen_data10	:32;

	uint32 pkt_auto_gen_data11	:32;

	uint32 pkt_auto_gen_data12	:32;

	uint32 pkt_auto_gen_data13	:32;

	uint32 pkt_auto_gen_data14	:32;

	uint32 pkt_auto_gen_data15	:32;
};
typedef struct net_tx_pkt_auto_gen_data_s net_tx_pkt_auto_gen_data_t;

struct net_tx_cal_ctl_s	/* 654 */
{

	uint32 walker_stop	:1;
	uint32 rsv_0	:15;
	uint32 walker_end	:8;
	uint32 walker_stop_idx	:8;
};
typedef struct net_tx_cal_ctl_s net_tx_cal_ctl_t;

struct net_tx_fr_stats_s	/* 655 */
{

	uint32 rsv_0	:28;
	uint32 fr_epe_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 fr_epe_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 fr_epe_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 fr_epe_data_error_cnt	:4;
};
typedef struct net_tx_fr_stats_s net_tx_fr_stats_t;

struct net_tx_fr_epe_data_unit_stats_s	/* 656 */
{

	uint32 rsv_0	:28;
	uint32 fr_epe_data_unit_cnt	:4;
};
typedef struct net_tx_fr_epe_data_unit_stats_s net_tx_fr_epe_data_unit_stats_t;

struct net_tx_drop_stats_s	/* 657 */
{

	uint32 rsv_0	:28;
	uint32 no_buf_drop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 ch_id_error_drop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 no_sop_drop_cnt	:4;

	uint32 rsv_3	:28;
	uint32 no_eop_drop_cnt	:4;
};
typedef struct net_tx_drop_stats_s net_tx_drop_stats_t;

struct net_tx_pkt_mem_access_stats_s	/* 658 */
{

	uint32 rsv_0	:28;
	uint32 wr_pkt_mem_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rd_pkt_mem_cnt	:4;
};
typedef struct net_tx_pkt_mem_access_stats_s net_tx_pkt_mem_access_stats_t;

struct net_tx_to_net_stats_s	/* 659 */
{

	uint32 rsv_0	:28;
	uint32 to_net_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 to_net_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 to_net_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 to_net_data_error_cnt	:4;
};
typedef struct net_tx_to_net_stats_s net_tx_to_net_stats_t;

struct net_tx_to_net_data_unit_stats_s	/* 660 */
{

	uint32 rsv_0	:28;
	uint32 to_net_data_unit_cnt	:4;
};
typedef struct net_tx_to_net_data_unit_stats_s net_tx_to_net_data_unit_stats_t;

struct net_tx_toe_loop_stats_s	/* 661 */
{

	uint32 rsv_0	:28;
	uint32 toe_loop_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 toe_loop_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 toe_loop_data_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 toe_loop_pkt_error_cnt	:4;
};
typedef struct net_tx_toe_loop_stats_s net_tx_toe_loop_stats_t;

struct net_tx_silent_drop_stats_s	/* 662 */
{

	uint32 rsv_0	:28;
	uint32 silent_drop_cnt	:4;
};
typedef struct net_tx_silent_drop_stats_s net_tx_silent_drop_stats_t;

struct net_tx_log_pkt_stats_s	/* 663 */
{

	uint32 rsv_0	:28;
	uint32 log_pkt_cnt	:4;
};
typedef struct net_tx_log_pkt_stats_s net_tx_log_pkt_stats_t;

struct net_tx_credit_return_stats_s	/* 664 */
{

	uint32 rsv_0	:28;
	uint32 credit_return_cnt	:4;
};
typedef struct net_tx_credit_return_stats_s net_tx_credit_return_stats_t;

struct oam_hdr_edit_ctl_s	/* 665 */
{

	uint32 cpu_exception_en	:32;

	uint32 rsv_0	:4;
	uint32 ccm_with_lm_to_cpu	:1;
	uint32 local_chip_id	:5;
	uint32 lbr_sa_using_lbm_da	:1;
	uint32 lbr_sa_type	:1;
	uint32 bypass_next_hop_ptr	:20;

	uint32 rsv_1	:12;
	uint32 relay_all_to_cpu_next_hop_ptr	:20;
};
typedef struct oam_hdr_edit_ctl_s oam_hdr_edit_ctl_t;

struct oam_hdr_edit_drain_enable_s	/* 666 */
{

	uint32 rsv_0	:31;
	uint32 hdr_edit_drain_enable	:1;
};
typedef struct oam_hdr_edit_drain_enable_s oam_hdr_edit_drain_enable_t;

struct oam_hdr_edit_misc_cfig_s	/* 667 */
{

	uint32 rsv_0	:28;
	uint32 hdr_edit_fwd_credit_value	:4;
};
typedef struct oam_hdr_edit_misc_cfig_s oam_hdr_edit_misc_cfig_t;

struct oam_hdr_edit_debug_stats_s	/* 668 */
{

	uint32 hdr_edit_in_eop_cnt	:24;
	uint32 rsv_0	:4;
	uint32 hdr_edit_in_sop_cnt	:4;

	uint32 hdr_edit_out_to_cpu_eop_cnt	:24;
	uint32 rsv_1	:4;
	uint32 hdr_edit_out_to_cpu_sop_cnt	:4;

	uint32 hdr_edit_out_to_bsr_eop_cnt	:24;
	uint32 rsv_2	:4;
	uint32 hdr_edit_out_to_bsr_sop_cnt	:4;

	uint32 hdr_edit_discard_eop_cnt	:24;
	uint32 rsv_3	:4;
	uint32 hdr_edit_discard_sop_cnt	:4;
};
typedef struct oam_hdr_edit_debug_stats_s oam_hdr_edit_debug_stats_t;

struct oam_fwd_ctl_s	/* 669 */
{

	uint32 rsv_0	:16;
	uint32 cpu_channel_id	:8;
	uint32 oam_channel_id	:8;
};
typedef struct oam_fwd_ctl_s oam_fwd_ctl_t;

struct oam_fwd_misc_cfg_s	/* 670 */
{

	uint32 rsv_0	:28;
	uint32 oam_buf_store_credit_value	:4;
};
typedef struct oam_fwd_misc_cfg_s oam_fwd_misc_cfg_t;

struct oam_fwd_drain_enable_s	/* 671 */
{

	uint32 rsv_0	:31;
	uint32 fwd_drain_enable	:1;
};
typedef struct oam_fwd_drain_enable_s oam_fwd_drain_enable_t;

struct oam_fwd_bs_wrr_weight_s	/* 672 */
{

	uint32 rsv_0	:20;
	uint32 tx_pkt_weight	:4;
	uint32 rsv_1	:4;
	uint32 rx_pkt_weight	:4;
};
typedef struct oam_fwd_bs_wrr_weight_s oam_fwd_bs_wrr_weight_t;

struct oam_fwd_cpu_wrr_weight_s	/* 673 */
{

	uint32 rsv_0	:8;
	uint32 cpu_pkt_weight	:8;
	uint32 aps_pkt_weight	:8;
	uint32 oam_pkt_weight	:8;
};
typedef struct oam_fwd_cpu_wrr_weight_s oam_fwd_cpu_wrr_weight_t;

struct oam_fwd_debug_stats_s	/* 674 */
{

	uint32 rsv_0	:20;
	uint32 fr_cpu_eop_cnt	:4;
	uint32 rsv_1	:4;
	uint32 fr_cpu_sop_cnt	:4;

	uint32 rsv_2	:20;
	uint32 fr_tx_eop_cnt	:4;
	uint32 rsv_3	:4;
	uint32 fr_tx_sop_cnt	:4;

	uint32 rsv_4	:20;
	uint32 fr_upd_eop_cnt	:4;
	uint32 rsv_5	:4;
	uint32 fr_upd_sop_cnt	:4;

	uint32 rsv_6	:20;
	uint32 fr_rx_to_bs_eop_cnt	:4;
	uint32 rsv_7	:4;
	uint32 fr_rx_to_bs_sop_cnt	:4;

	uint32 rsv_8	:20;
	uint32 fr_rx_to_cpu_eop_cnt	:4;
	uint32 rsv_9	:4;
	uint32 fr_rx_to_cpu_sop_cnt	:4;

	uint32 rsv_10	:20;
	uint32 fr_br_to_cpu_eop_cnt	:4;
	uint32 rsv_11	:4;
	uint32 fr_br_to_cpu_sop_cnt	:4;

	uint32 rsv_12	:20;
	uint32 to_bs_eop_cnt	:4;
	uint32 rsv_13	:4;
	uint32 to_bs_sop_cnt	:4;

	uint32 rsv_14	:20;
	uint32 to_cpu_eop_cnt	:4;
	uint32 rsv_15	:4;
	uint32 to_cpu_sop_cnt	:4;

	uint32 rsv_16	:28;
	uint32 to_cpu_pkt_error_cnt	:4;
};
typedef struct oam_fwd_debug_stats_s oam_fwd_debug_stats_t;

struct oam_fwd_interrupt_fatal_s	/* 675 */
{

	uint32 rsv_0	:20;
	uint32 mask_set_fatal	:12;

	uint32 rsv_1	:20;
	uint32 mask_reset_fatal	:12;

	uint32 rsv_2	:20;
	uint32 value_set_fatal	:12;

	uint32 rsv_3	:20;
	uint32 value_reset_fatal	:12;
};
typedef struct oam_fwd_interrupt_fatal_s oam_fwd_interrupt_fatal_t;

struct oam_fwd_credit_used_s	/* 676 */
{

	uint32 rsv_0	:27;
	uint32 hdr_edit_fwd_credit_used	:5;

	uint32 rsv_1	:28;
	uint32 oam_buf_store_credit_used	:4;
};
typedef struct oam_fwd_credit_used_s oam_fwd_credit_used_t;

struct oam_lookup_interrupt_s	/* 677 */
{

	uint32 rsv_0	:16;
	uint32 value_set	:16;

	uint32 rsv_1	:16;
	uint32 value_reset	:16;

	uint32 rsv_2	:16;
	uint32 mask_set	:16;

	uint32 rsv_3	:16;
	uint32 mask_reset	:16;
};
typedef struct oam_lookup_interrupt_s oam_lookup_interrupt_t;

struct oam_lookup_credit_config_s	/* 678 */
{

	uint32 rsv_0	:28;
	uint32 rx_proc_credit	:4;
};
typedef struct oam_lookup_credit_config_s oam_lookup_credit_config_t;

struct oam_lookup_drain_enable_config_s	/* 679 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct oam_lookup_drain_enable_config_s oam_lookup_drain_enable_config_t;

struct oam_lookup_ctl_s	/* 680 */
{

	uint32 rsv_0	:8;
	uint32 lm_proc_by_cpu	:1;
	uint32 lbr_proc_by_cpu	:1;
	uint32 lbm_proc_by_cpu	:1;
	uint32 oam_tcam_lookup_en	:5;
	uint32 oam_lkup_hash_en	:1;
	uint32 oam_lkup_ext_tcam_en	:1;
	uint32 oam_lkup_key_size	:2;
	uint32 oam_lkup_table_id	:4;
	uint32 oam_lkup_inst	:8;

	uint32 rsv_1	:1;
	uint32 slow_oam_all_to_cpu	:1;
	uint32 below_link_level_use_vlan0	:1;
	uint32 eth_oam_p2p_mode	:1;
	uint32 mpls_use_ttsi	:1;
	uint32 min_interval_to_cpu	:3;
	uint32 oam_lookup_en_bmp	:8;
	uint32 link_oam_vlan_ptr	:16;
};
typedef struct oam_lookup_ctl_s oam_lookup_ctl_t;

struct oam_lookup_result_ctl_s	/* 681 */
{

	uint32 rsv_0	:2;
	uint32 index_shift	:2;
	uint32 rsv_1	:2;
	uint32 index_base	:12;
	uint32 rsv_2	:2;
	uint32 table_base	:12;
};
typedef struct oam_lookup_result_ctl_s oam_lookup_result_ctl_t;

struct oam_hash_lookup_ctl_s	/* 682 */
{

	uint32 rsv_0	:14;
	uint32 hash_bits_num0	:2;
	uint32 rsv_1	:11;
	uint32 oam_table_base0	:5;

	uint32 rsv_2	:14;
	uint32 hash_bits_num1	:2;
	uint32 rsv_3	:11;
	uint32 oam_table_base1	:5;

	uint32 rsv_4	:14;
	uint32 hash_bits_num2	:2;
	uint32 rsv_5	:11;
	uint32 oam_table_base2	:5;

	uint32 rsv_6	:14;
	uint32 hash_bits_num3	:2;
	uint32 rsv_7	:11;
	uint32 oam_table_base3	:5;

	uint32 rsv_8	:14;
	uint32 hash_bits_num4	:2;
	uint32 rsv_9	:11;
	uint32 oam_table_base4	:5;
};
typedef struct oam_hash_lookup_ctl_s oam_hash_lookup_ctl_t;

struct oam_hash_lookup_result_ctl_s	/* 683 */
{

	uint32 rsv_0	:4;
	uint32 mep_lookup_default_index0	:12;
	uint32 rsv_1	:11;
	uint32 oam_lookup_result_table_base0	:5;

	uint32 rsv_2	:4;
	uint32 mep_lookup_default_index1	:12;
	uint32 rsv_3	:11;
	uint32 oam_lookup_result_table_base1	:5;

	uint32 rsv_4	:4;
	uint32 mep_lookup_default_index2	:12;
	uint32 rsv_5	:11;
	uint32 oam_lookup_result_table_base2	:5;

	uint32 rsv_6	:4;
	uint32 mep_lookup_default_index3	:12;
	uint32 rsv_7	:11;
	uint32 oam_lookup_result_table_base3	:5;

	uint32 rsv_8	:4;
	uint32 mep_lookup_default_index4	:12;
	uint32 rsv_9	:11;
	uint32 oam_lookup_result_table_base4	:5;
};
typedef struct oam_hash_lookup_result_ctl_s oam_hash_lookup_result_ctl_t;

struct oam_lkup_parity_fail_record_s	/* 684 */
{

	uint32 rsv_0	:15;
	uint32 ds_mep_chan_parity_fail	:1;
	uint32 rsv_1	:4;
	uint32 ds_mep_chan_parity_fail_addr	:12;

	uint32 rsv_2	:15;
	uint32 ds_oam_hash_key_parity_fail	:1;
	uint32 rsv_3	:6;
	uint32 ds_oam_hash_key_parity_fail_addr	:10;
};
typedef struct oam_lkup_parity_fail_record_s oam_lkup_parity_fail_record_t;

struct oam_lkup_threshold_cfg_s	/* 685 */
{

	uint32 rsv_0	:11;
	uint32 mep_track_fifoa_full_thrd	:5;
	uint32 rsv_1	:4;
	uint32 rmep_ack_fifoa_full_thrd	:4;
	uint32 rsv_2	:3;
	uint32 rmep_req_fifoa_full_thrd	:5;
};
typedef struct oam_lkup_threshold_cfg_s oam_lkup_threshold_cfg_t;

struct oam_rx_stats_s	/* 686 */
{

	uint32 rsv_0	:4;
	uint32 seq_mismatch_cnt	:4;
	uint32 rsv_1	:4;
	uint32 fr_parserpr_cnt	:4;
	uint32 rsv_2	:4;
	uint32 fr_hdr_adj_discard_cnt	:4;
	uint32 rsv_3	:4;
	uint32 fr_hdr_adjpi_cnt	:4;
};
typedef struct oam_rx_stats_s oam_rx_stats_t;

struct oam_tx_stats_s	/* 687 */
{

	uint32 rsv_0	:20;
	uint32 to_proc_discard_cnt	:4;
	uint32 rsv_1	:4;
	uint32 to_procpi_cnt	:4;
};
typedef struct oam_tx_stats_s oam_tx_stats_t;

struct oam_lookup_stats_s	/* 688 */
{

	uint32 rsv_0	:4;
	uint32 fr_tcam_error_cnt	:4;
	uint32 rsv_1	:4;
	uint32 fr_tcam_valid_cnt	:4;
	uint32 rsv_2	:12;
	uint32 to_tcam_valid_cnt	:4;

	uint32 rsv_3	:20;
	uint32 lookup_default_cnt	:4;
	uint32 rsv_4	:4;
	uint32 lookup_hash_cnt	:4;
};
typedef struct oam_lookup_stats_s oam_lookup_stats_t;

struct oam_hdr_adjust_ctl_s	/* 689 */
{

	uint32 rsv_0	:31;
	uint32 relay_all_to_cpu	:1;
};
typedef struct oam_hdr_adjust_ctl_s oam_hdr_adjust_ctl_t;

struct oam_hdr_adjust_drain_enable_s	/* 690 */
{

	uint32 rsv_0	:30;
	uint32 hdr_adjust_cpu_drain_enable	:1;
	uint32 hdr_adjust_oam_drain_enable	:1;
};
typedef struct oam_hdr_adjust_drain_enable_s oam_hdr_adjust_drain_enable_t;

struct oam_hdr_adjust_misc_cfg_s	/* 691 */
{

	uint32 disable_crc_chk	:1;
	uint32 rsv_0	:8;
	uint32 hdr_adjust_hdr_edit_credit_value	:7;
	uint32 rsv_1	:4;
	uint32 hdr_adjust_lkup_credit_value	:4;
	uint32 rsv_2	:3;
	uint32 hdr_adjust_rx_proc_credit_value	:5;

	uint32 rsv_3	:19;
	uint32 cpu_pkt_fifoa_full_thrd	:5;
	uint32 rsv_4	:3;
	uint32 oam_pkt_fifoa_full_thrd	:5;
};
typedef struct oam_hdr_adjust_misc_cfg_s oam_hdr_adjust_misc_cfg_t;

struct oam_hdr_adjust_credit_used_s	/* 692 */
{

	uint32 rsv_0	:7;
	uint32 hdr_adjust_fwd_credit_used	:1;
	uint32 rsv_1	:1;
	uint32 hdr_adjust_hdr_edit_credit_used	:7;
	uint32 rsv_2	:4;
	uint32 hdr_adjust_lkup_credit_used	:4;
	uint32 rsv_3	:3;
	uint32 hdr_adjust_rx_proc_credit_used	:5;
};
typedef struct oam_hdr_adjust_credit_used_s oam_hdr_adjust_credit_used_t;

struct oam_parser_packet_type_table_s	/* 693 */
{

	uint32 rsv_0	:24;
	uint32 layer2_type0	:4;
	uint32 layer3_type0	:4;

	uint32 rsv_1	:24;
	uint32 layer2_type1	:4;
	uint32 layer3_type1	:4;

	uint32 rsv_2	:24;
	uint32 layer2_type2	:4;
	uint32 layer3_type2	:4;

	uint32 rsv_3	:24;
	uint32 layer2_type3	:4;
	uint32 layer3_type3	:4;

	uint32 rsv_4	:24;
	uint32 layer2_type4	:4;
	uint32 layer3_type4	:4;

	uint32 rsv_5	:24;
	uint32 layer2_type5	:4;
	uint32 layer3_type5	:4;

	uint32 rsv_6	:24;
	uint32 layer2_type6	:4;
	uint32 layer3_type6	:4;

	uint32 rsv_7	:24;
	uint32 layer2_type7	:4;
	uint32 layer3_type7	:4;
};
typedef struct oam_parser_packet_type_table_s oam_parser_packet_type_table_t;

struct oam_parser_ether_ctl_s	/* 694 */
{

	uint32 bvlan_tpid	:16;
	uint32 max_mep_id	:16;

	uint32 rsv_0	:2;
	uint32 cfm_pdu_mac_da_md_lvl_check_en	:1;
	uint32 tlv_length_check_en	:1;
	uint32 not_check_bip	:1;
	uint32 rsv_1	:1;
	uint32 max_label_num	:3;
	uint32 allow_non_zero_oui	:1;
	uint32 cfm_pdu_max_length	:14;
	uint32 first_tlv_offset_chk	:8;

	uint32 mpls_oam_min_length	:8;
	uint32 cfm_pdu_min_length	:8;
	uint32 md_name_length_chk	:8;
	uint32 ma_id_length_chk	:8;

	uint32 min_mep_id	:8;
	uint32 min_port_status_tlv_value	:2;
	uint32 max_port_status_tlv_value	:2;
	uint32 mpls_oam_alert_label0	:20;

	uint32 ignore_eth_oam_version	:1;
	uint32 rsv_2	:1;
	uint32 invalid_ccm_interval_check_en	:1;
	uint32 invalid_ccm_interval	:3;
	uint32 min_intf_status_tlv_value	:3;
	uint32 max_intf_status_tlv_value	:3;
	uint32 mpls_oam_alert_label1	:20;

	uint32 svlan_tpid0	:16;
	uint32 svlan_tpid1	:16;

	uint32 svlan_tpid2	:16;
	uint32 svlan_tpid3	:16;

	uint32 cvlan_tpid	:16;
	uint32 max_length_field	:16;
};
typedef struct oam_parser_ether_ctl_s oam_parser_ether_ctl_t;

struct oam_parser_layer2_protocol_cam_valid_s	/* 695 */
{

	uint32 rsv_0	:24;
	uint32 layer2_cam_entry_valid	:8;
};
typedef struct oam_parser_layer2_protocol_cam_valid_s oam_parser_layer2_protocol_cam_valid_t;

struct oam_parser_layer2_protocol_cam_s	/* 696 */
{

	uint32 layer2_cam_layer3_type0	:4;
	uint32 layer2_cam_additional_offset0	:4;
	uint32 rsv_0	:1;
	uint32 layer2_cam_value0	:23;

	uint32 layer2_cam_layer3_type1	:4;
	uint32 layer2_cam_additional_offset1	:4;
	uint32 rsv_1	:1;
	uint32 layer2_cam_value1	:23;

	uint32 layer2_cam_layer3_type2	:4;
	uint32 layer2_cam_additional_offset2	:4;
	uint32 rsv_2	:1;
	uint32 layer2_cam_value2	:23;

	uint32 layer2_cam_layer3_type3	:4;
	uint32 layer2_cam_additional_offset3	:4;
	uint32 rsv_3	:1;
	uint32 layer2_cam_value3	:23;

	uint32 layer2_cam_layer3_type4	:4;
	uint32 layer2_cam_additional_offset4	:4;
	uint32 rsv_4	:1;
	uint32 layer2_cam_value4	:23;

	uint32 layer2_cam_layer3_type5	:4;
	uint32 layer2_cam_additional_offset5	:4;
	uint32 rsv_5	:1;
	uint32 layer2_cam_value5	:23;

	uint32 layer2_cam_layer3_type6	:4;
	uint32 layer2_cam_additional_offset6	:4;
	uint32 rsv_6	:1;
	uint32 layer2_cam_value6	:23;

	uint32 layer2_cam_layer3_type7	:4;
	uint32 layer2_cam_additional_offset7	:4;
	uint32 rsv_7	:1;
	uint32 layer2_cam_value7	:23;

	uint32 rsv_8	:9;
	uint32 layer2_cam_mask0	:23;

	uint32 rsv_9	:9;
	uint32 layer2_cam_mask1	:23;

	uint32 rsv_10	:9;
	uint32 layer2_cam_mask2	:23;

	uint32 rsv_11	:9;
	uint32 layer2_cam_mask3	:23;

	uint32 rsv_12	:9;
	uint32 layer2_cam_mask4	:23;

	uint32 rsv_13	:9;
	uint32 layer2_cam_mask5	:23;

	uint32 rsv_14	:9;
	uint32 layer2_cam_mask6	:23;

	uint32 rsv_15	:9;
	uint32 layer2_cam_mask7	:23;
};
typedef struct oam_parser_layer2_protocol_cam_s oam_parser_layer2_protocol_cam_t;

struct oam_parser_max_tlv_num_s	/* 697 */
{

	uint32 rsv_0	:29;
	uint32 max_tlv_num	:3;
};
typedef struct oam_parser_max_tlv_num_s oam_parser_max_tlv_num_t;

struct oam_parser_debug_stats_s	/* 698 */
{

	uint32 cpu_pkt_eop_in_cnt	:24;
	uint32 rsv_0	:4;
	uint32 cpu_pkt_sop_in_cnt	:4;

	uint32 oam_pkt_eop_in_cnt	:24;
	uint32 rsv_1	:4;
	uint32 oam_pkt_sop_in_cnt	:4;

	uint32 rsv_2	:20;
	uint32 cpu_pkt_eop_out_cnt	:4;
	uint32 rsv_3	:4;
	uint32 cpu_pkt_sop_out_cnt	:4;

	uint32 rsv_4	:20;
	uint32 oam_pkt_eop_out_cnt	:4;
	uint32 rsv_5	:4;
	uint32 oam_pkt_sop_out_cnt	:4;

	uint32 rsv_6	:20;
	uint32 outpr_sot_cnt	:4;
	uint32 rsv_7	:4;
	uint32 outpr_eot_cnt	:4;

	uint32 rsv_8	:28;
	uint32 outpr_invalid_cnt	:4;

	uint32 rsv_9	:28;
	uint32 cpu_pkt_crc_chk_err_cnt	:4;

	uint32 rsv_10	:28;
	uint32 oam_pkt_crc_chk_err_cnt	:4;
};
typedef struct oam_parser_debug_stats_s oam_parser_debug_stats_t;

struct oam_parser_interrupt_fatal_s	/* 699 */
{

	uint32 rsv_0	:28;
	uint32 mask_set_fatal	:4;

	uint32 rsv_1	:28;
	uint32 mask_reset_fatal	:4;

	uint32 rsv_2	:28;
	uint32 value_set_fatal	:4;

	uint32 rsv_3	:28;
	uint32 value_reset_fatal	:4;
};
typedef struct oam_parser_interrupt_fatal_s oam_parser_interrupt_fatal_t;

struct oam_rx_proc_ether_ctl_s	/* 700 */
{

	uint32 rsv_0	:2;
	uint32 alarm_src_mac_mismatch	:1;
	uint32 ma_id_length_type	:2;
	uint32 tri_cycle_unexpcv	:3;
	uint32 rsv_1	:4;
	uint32 ten_cycle_unexpcv	:4;
	uint32 rsv_2	:2;
	uint32 rmep_while_cfg	:4;
	uint32 tri_cycle_bdi	:2;
	uint32 rsv_3	:4;
	uint32 ten_cycle_bdi	:4;

	uint32 lbm_mac_da_check_en	:1;
	uint32 seq_num_fail_report_thrd	:6;
	uint32 eth_oam_p2p_mode	:1;
	uint32 ether_defect_to_rdi	:8;
	uint32 d_unexp_period_timer_cfg	:4;
	uint32 d_unexp_mep_timer_cfg	:4;
	uint32 d_mismerge_timer_cfg	:4;
	uint32 d_meg_lvl_timer_cfg	:4;

	uint32 rsv_4	:16;
	uint32 bridge_mac47to32	:16;

	uint32 bridge_mac31to0	:32;

	uint32 rsv_5	:16;
	uint32 port_mac47to32	:16;

	uint32 port_mac31to8	:24;
	uint32 rsv_6	:8;
};
typedef struct oam_rx_proc_ether_ctl_s oam_rx_proc_ether_ctl_t;

struct oam_rx_proc_misc_ctl_s	/* 701 */
{

	uint32 rsv_0	:28;
	uint32 rx_proc_hdr_edit_credit_value	:4;
};
typedef struct oam_rx_proc_misc_ctl_s oam_rx_proc_misc_ctl_t;

struct oam_rx_proc_credit_used_s	/* 702 */
{

	uint32 rsv_0	:28;
	uint32 rx_proc_hdr_edit_credit_used	:4;
};
typedef struct oam_rx_proc_credit_used_s oam_rx_proc_credit_used_t;

struct oam_rx_proc_debug_stats_s	/* 703 */
{

	uint32 rsv_0	:28;
	uint32 rx_proc_hdr_edit_info_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_proc_defet_free_cnt	:4;

	uint32 rsv_2	:23;
	uint32 rx_procpr_in_fifo_pop_fsm_ps	:9;
};
typedef struct oam_rx_proc_debug_stats_s oam_rx_proc_debug_stats_t;

struct oamproc_oam_upd_ctl_s	/* 704 */
{

	uint32 rsv_0	:10;
	uint32 upd_en	:1;
	uint32 rsv_1	:1;
	uint32 upd_eth_oam_p2p_mode	:1;
	uint32 cci_while_cfg	:3;
	uint32 upd_interval	:16;

	uint32 min_ptr	:16;
	uint32 max_ptr	:16;

	uint32 rsv_2	:3;
	uint32 t2_pending_while_cfg	:4;
	uint32 bdi_tx_set_by_cpu	:1;
	uint32 mpls_cci_while_cfg	:3;
	uint32 t1_cycle	:4;
	uint32 t3_cycle	:4;
	uint32 itu_defect_clear_mode	:1;
	uint32 cnt_shift_while_cfg	:3;
	uint32 gen_rdi_by_dloc	:1;
	uint32 bdi_cycle_cfg	:4;
	uint32 upd_rmep_while_cfg	:4;
};
typedef struct oamproc_oam_upd_ctl_s oamproc_oam_upd_ctl_t;

struct oamproc_oam_upd_status_s	/* 705 */
{

	uint32 rsv_0	:2;
	uint32 quarter_flag_seen	:6;
	uint32 rsv_1	:2;
	uint32 quarter_flag	:6;
	uint32 upd_ptr	:16;
};
typedef struct oamproc_oam_upd_status_s oamproc_oam_upd_status_t;

struct oamproc_oam_upd_cci_ctl_s	/* 706 */
{

	uint32 rsv_0	:10;
	uint32 quarter_cci2_interval	:22;

	uint32 rsv_1	:12;
	uint32 quarter_cci3_interval	:4;
	uint32 quarter_cci4_interval	:4;
	uint32 quarter_cci5_interval	:4;
	uint32 quarter_cci6_interval	:4;
	uint32 quarter_cci7_interval	:4;
};
typedef struct oamproc_oam_upd_cci_ctl_s oamproc_oam_upd_cci_ctl_t;

struct oamproc_oam_upd_aps_ctl_s	/* 707 */
{

	uint32 rsv_0	:14;
	uint32 src_chip_id	:5;
	uint32 mpls_aps_sig_fail_mask	:4;
	uint32 eth_mep_aps_sig_fail_mask	:3;
	uint32 eth_rmep_aps_sig_fail_mask	:5;
	uint32 global_aps_en	:1;

	uint32 rsv_1	:12;
	uint32 sig_fail_next_hop_ptr	:20;

	uint32 rsv_2	:16;
	uint32 mac_da_hi	:16;

	uint32 mac_da_lo	:32;

	uint32 rsv_3	:16;
	uint32 mac_sa_hi	:16;

	uint32 mac_sa_lo	:32;

	uint32 tpid	:16;
	uint32 vlan	:16;
};
typedef struct oamproc_oam_upd_aps_ctl_s oamproc_oam_upd_aps_ctl_t;

struct oamproc_oam_upd_debug_stats_s	/* 708 */
{

	uint32 rsv_0	:24;
	uint32 upd_ptr_req_drop_cnt	:8;

	uint32 rsv_1	:24;
	uint32 upd_aps_drop_cnt	:8;

	uint32 rsv_2	:31;
	uint32 upd_ptr_req_fifo_pop_fsm_ps	:1;
};
typedef struct oamproc_oam_upd_debug_stats_s oamproc_oam_upd_debug_stats_t;

struct oam_tx_proc_ether_ctl_s	/* 709 */
{

	uint32 rsv_0	:8;
	uint32 cfm_mcast_addr23to3	:21;
	uint32 rsv_1	:3;

	uint32 rsv_2	:6;
	uint32 tx_ma_id_length_type	:2;
	uint32 rsv_3	:4;
	uint32 mpls_oam_alert_label	:20;

	uint32 rsv_4	:3;
	uint32 ccm_version	:5;
	uint32 ccm_opcode	:8;
	uint32 ccm_ether_type	:16;

	uint32 cfm_mcast_addr47to24	:24;
	uint32 ccm_first_tlv_offset	:8;

	uint32 tpid0	:16;
	uint32 tpid1	:16;

	uint32 tpid2	:16;
	uint32 tpid3	:16;

	uint32 rsv_5	:16;
	uint32 tx_bridge_mac47to32	:16;

	uint32 tx_bridge_mac31to0	:32;
};
typedef struct oam_tx_proc_ether_ctl_s oam_tx_proc_ether_ctl_t;

struct oam_tx_proc_ether_mac_s	/* 710 */
{

	uint32 rsv_0	:16;
	uint32 tx_port_mac47to32	:16;

	uint32 tx_port_mac31to8	:24;
	uint32 rsv_1	:8;
};
typedef struct oam_tx_proc_ether_mac_s oam_tx_proc_ether_mac_t;

struct oam_tx_proc_ether_send_id_s	/* 711 */
{

	uint32 rsv_0	:4;
	uint32 send_id_length	:4;
	uint32 send_id_byte0to2	:24;

	uint32 send_id_byte3to6	:32;

	uint32 send_id_byte7to10	:32;

	uint32 send_id_byte11to14	:32;
};
typedef struct oam_tx_proc_ether_send_id_s oam_tx_proc_ether_send_id_t;

struct oam_tx_proc_priority_map_s	/* 712 */
{

	uint32 rsv_0	:18;
	uint32 color0	:2;
	uint32 rsv_1	:1;
	uint32 cos0	:3;
	uint32 rsv_2	:2;
	uint32 priority0	:6;

	uint32 rsv_3	:18;
	uint32 color1	:2;
	uint32 rsv_4	:1;
	uint32 cos1	:3;
	uint32 rsv_5	:2;
	uint32 priority1	:6;

	uint32 rsv_6	:18;
	uint32 color2	:2;
	uint32 rsv_7	:1;
	uint32 cos2	:3;
	uint32 rsv_8	:2;
	uint32 priority2	:6;

	uint32 rsv_9	:18;
	uint32 color3	:2;
	uint32 rsv_10	:1;
	uint32 cos3	:3;
	uint32 rsv_11	:2;
	uint32 priority3	:6;

	uint32 rsv_12	:18;
	uint32 color4	:2;
	uint32 rsv_13	:1;
	uint32 cos4	:3;
	uint32 rsv_14	:2;
	uint32 priority4	:6;

	uint32 rsv_15	:18;
	uint32 color5	:2;
	uint32 rsv_16	:1;
	uint32 cos5	:3;
	uint32 rsv_17	:2;
	uint32 priority5	:6;

	uint32 rsv_18	:18;
	uint32 color6	:2;
	uint32 rsv_19	:1;
	uint32 cos6	:3;
	uint32 rsv_20	:2;
	uint32 priority6	:6;

	uint32 rsv_21	:18;
	uint32 color7	:2;
	uint32 rsv_22	:1;
	uint32 cos7	:3;
	uint32 rsv_23	:2;
	uint32 priority7	:6;
};
typedef struct oam_tx_proc_priority_map_s oam_tx_proc_priority_map_t;

struct oam_tx_proc_misc_ctl_s	/* 713 */
{

	uint32 rsv_0	:30;
	uint32 tx_proc_fwd_credit_value	:2;
};
typedef struct oam_tx_proc_misc_ctl_s oam_tx_proc_misc_ctl_t;

struct oam_tx_proc_credit_used_s	/* 714 */
{

	uint32 rsv_0	:30;
	uint32 tx_proc_fwd_credit_used	:2;
};
typedef struct oam_tx_proc_credit_used_s oam_tx_proc_credit_used_t;

struct oam_tx_proc_debug_stats_s	/* 715 */
{

	uint32 rsv_0	:24;
	uint32 tx_msg_push_cnt	:8;

	uint32 rsv_1	:24;
	uint32 tx_msg_drop_cnt	:8;

	uint32 rsv_2	:28;
	uint32 tx_non_pkt_cnt	:4;

	uint32 rsv_3	:28;
	uint32 tx_eth_pkt_cnt	:4;

	uint32 rsv_4	:28;
	uint32 tx_mpls_pkt_cnt	:4;
};
typedef struct oam_tx_proc_debug_stats_s oam_tx_proc_debug_stats_t;

struct oam_err_cache_valid_s	/* 716 */
{

	uint32 always_cpu_proc	:1;
	uint32 rsv_0	:10;
	uint32 err_cache_intr_thrd	:5;
	uint32 err_cache_entry_valid	:16;
};
typedef struct oam_err_cache_valid_s oam_err_cache_valid_t;

struct oam_err_cache_debug_stats_s	/* 717 */
{

	uint32 rsv_0	:2;
	uint32 is_mpls_err_cache0	:1;
	uint32 defect_type_err_cache0	:3;
	uint32 defect_sub_type_err_cache0	:3;
	uint32 port_status_valid_err_cache0	:1;
	uint32 port_status_value_err_cache0	:2;
	uint32 intf_status_valid_err_cache0	:1;
	uint32 intf_status_value_err_cache0	:3;
	uint32 rsv_1	:10;
	uint32 defect_priority_err_cache0	:6;

	uint32 rsv_2	:1;
	uint32 rmep_index_err_cache0	:15;
	uint32 rsv_3	:1;
	uint32 mep_index_err_cache0	:15;

	uint32 rsv_4	:2;
	uint32 is_mpls_err_cache1	:1;
	uint32 defect_type_err_cache1	:3;
	uint32 defect_sub_type_err_cache1	:3;
	uint32 port_status_valid_err_cache1	:1;
	uint32 port_status_value_err_cache1	:2;
	uint32 intf_status_valid_err_cache1	:1;
	uint32 intf_status_value_err_cache1	:3;
	uint32 rsv_5	:10;
	uint32 defect_priority_err_cache1	:6;

	uint32 rsv_6	:1;
	uint32 rmep_index_err_cache1	:15;
	uint32 rsv_7	:1;
	uint32 mep_index_err_cache1	:15;
};
typedef struct oam_err_cache_debug_stats_s oam_err_cache_debug_stats_t;

struct oam_cpu_access_ds_mp_ctl_s	/* 718 */
{

	uint32 lock	:1;
	uint32 rsv_0	:16;
	uint32 ds_mp_ptr	:15;
};
typedef struct oam_cpu_access_ds_mp_ctl_s oam_cpu_access_ds_mp_ctl_t;

struct oam_proc_locked_addr_entry_valid_s	/* 719 */
{

	uint32 rsv_0	:28;
	uint32 locked_addr_entry_valid	:4;

	uint32 rsv_1	:28;
	uint32 cpu_locked_addr_entry_valid	:4;
};
typedef struct oam_proc_locked_addr_entry_valid_s oam_proc_locked_addr_entry_valid_t;

struct oam_proc_interrupt_normal_s	/* 720 */
{

	uint32 rsv_0	:31;
	uint32 mask_set_normal	:1;

	uint32 rsv_1	:31;
	uint32 mask_reset_normal	:1;

	uint32 rsv_2	:31;
	uint32 value_set_normal	:1;

	uint32 rsv_3	:31;
	uint32 value_reset_normal	:1;
};
typedef struct oam_proc_interrupt_normal_s oam_proc_interrupt_normal_t;

struct oam_proc_interrupt_fatal_s	/* 721 */
{

	uint32 rsv_0	:16;
	uint32 mask_set_fatal	:16;

	uint32 rsv_1	:16;
	uint32 mask_reset_fatal	:16;

	uint32 rsv_2	:16;
	uint32 value_set_fatal	:16;

	uint32 rsv_3	:16;
	uint32 value_reset_fatal	:16;
};
typedef struct oam_proc_interrupt_fatal_s oam_proc_interrupt_fatal_t;

struct oam_proc_parity_fail_record_s	/* 722 */
{

	uint32 rsv_0	:15;
	uint32 ds_mp_parity_fail	:1;
	uint32 rsv_1	:5;
	uint32 ds_mp_parity_fail_addr	:11;

	uint32 rsv_2	:15;
	uint32 ds_ma_parity_fail	:1;
	uint32 rsv_3	:5;
	uint32 ds_ma_parity_fail_addr	:11;

	uint32 rsv_4	:15;
	uint32 ds_ma_name_parity_fail	:1;
	uint32 rsv_5	:5;
	uint32 ds_ma_name_parity_fail_addr	:11;

	uint32 rsv_6	:15;
	uint32 ds_icc_parity_fail	:1;
	uint32 rsv_7	:10;
	uint32 ds_icc_parity_fail_addr	:6;

	uint32 rsv_8	:15;
	uint32 ds_port_property_parity_fail	:1;
	uint32 rsv_9	:8;
	uint32 ds_port_property_parity_fail_addr	:8;
};
typedef struct oam_proc_parity_fail_record_s oam_proc_parity_fail_record_t;

struct oamproc_oam_proc_cfg_err_record_s	/* 723 */
{

	uint32 rsv_0	:15;
	uint32 ds_rmep_mep_index_cfg_err	:1;
	uint32 rsv_1	:1;
	uint32 ds_rmep_mep_index_cfg_err_addr	:15;
};
typedef struct oamproc_oam_proc_cfg_err_record_s oamproc_oam_proc_cfg_err_record_t;

struct oam_proc_ds_init_s	/* 724 */
{

	uint32 rsv_0	:31;
	uint32 ds_mp_init	:1;

	uint32 rsv_1	:31;
	uint32 ds_mp_init_done	:1;

	uint32 rsv_2	:31;
	uint32 ds_ma_init	:1;

	uint32 rsv_3	:31;
	uint32 ds_ma_init_done	:1;

	uint32 rsv_4	:31;
	uint32 ds_ma_name_init	:1;

	uint32 rsv_5	:31;
	uint32 ds_ma_name_init_done	:1;
};
typedef struct oam_proc_ds_init_s oam_proc_ds_init_t;

struct parser_pbb_ctl_s	/* 731 */
{

	uint32 rsv_0	:5;
	uint32 pbb_cos_ecmp_hash_en	:1;
	uint32 nca_value	:1;
	uint32 pbb_vlan_hash_en	:1;
	uint32 rsv_1	:3;
	uint32 c_mac_outer_vlan_is_cvlan	:1;
	uint32 rsv_2	:3;
	uint32 pbb_cos_hash_en	:1;
	uint32 rsv_3	:2;
	uint32 pbb_vlan_parsing_num	:2;
	uint32 rsv_4	:8;
	uint32 c_mac_hash_disable	:4;

	uint32 rsv_5	:10;
	uint32 pbb_oam_ether_type_offset	:6;
	uint32 pbb_oam_ether_type	:16;
};
typedef struct parser_pbb_ctl_s parser_pbb_ctl_t;

struct parser_packet_type_table_s	/* 732 */
{

	uint32 rsv_0	:24;
	uint32 packet_type_layer2_type0	:4;
	uint32 packet_type_layer3_type0	:4;

	uint32 rsv_1	:24;
	uint32 packet_type_layer2_type1	:4;
	uint32 packet_type_layer3_type1	:4;

	uint32 rsv_2	:24;
	uint32 packet_type_layer2_type2	:4;
	uint32 packet_type_layer3_type2	:4;

	uint32 rsv_3	:24;
	uint32 packet_type_layer2_type3	:4;
	uint32 packet_type_layer3_type3	:4;

	uint32 rsv_4	:24;
	uint32 packet_type_layer2_type4	:4;
	uint32 packet_type_layer3_type4	:4;

	uint32 rsv_5	:24;
	uint32 packet_type_layer2_type5	:4;
	uint32 packet_type_layer3_type5	:4;

	uint32 rsv_6	:24;
	uint32 packet_type_layer2_type6	:4;
	uint32 packet_type_layer3_type6	:4;

	uint32 rsv_7	:24;
	uint32 packet_type_layer2_type7	:4;
	uint32 packet_type_layer3_type7	:4;
};
typedef struct parser_packet_type_table_s parser_packet_type_table_t;

struct parser_ethernet_ctl_s	/* 733 */
{

	uint32 cvlan_tpid	:16;
	uint32 vlan_hash_en	:1;
	uint32 cos_hash_en	:1;
	uint32 layer2_header_protocol_hash_en	:1;
	uint32 allow_non_zero_oui	:1;
	uint32 rsv_0	:4;
	uint32 mod_hash_en	:1;
	uint32 port_hash_en	:1;
	uint32 vlan_hash_mode	:1;
	uint32 cos_ecmp_hash_en	:1;
	uint32 mac_hash_disable	:4;

	uint32 rsv_1	:16;
	uint32 max_length_field	:16;

	uint32 i_tag_tpid	:16;
	uint32 bvlan_tpid	:16;

	uint32 rsv_2	:26;
	uint32 parsing_quad_vlan	:1;
	uint32 isid_hash_en	:1;
	uint32 i_pcp_hash_en	:1;
	uint32 rsv_3	:1;
	uint32 vlan_parsing_num	:2;

	uint32 svlan_tpid0	:16;
	uint32 svlan_tpid1	:16;

	uint32 svlan_tpid2	:16;
	uint32 svlan_tpid3	:16;
};
typedef struct parser_ethernet_ctl_s parser_ethernet_ctl_t;

struct parser_layer2_protocol_cam_valid_s	/* 734 */
{

	uint32 rsv_0	:16;
	uint32 layer2_cam_entry_valid	:16;
};
typedef struct parser_layer2_protocol_cam_valid_s parser_layer2_protocol_cam_valid_t;

struct parser_layer2_protocol_cam_s	/* 735 */
{

	uint32 layer2_protocol_cam_layer3_type0	:4;
	uint32 layer2_protocol_cam_additional_offset0	:4;
	uint32 rsv_0	:1;
	uint32 layer2_protocol_cam_value0	:23;

	uint32 layer2_protocol_cam_layer3_type1	:4;
	uint32 layer2_protocol_cam_additional_offset1	:4;
	uint32 rsv_1	:1;
	uint32 layer2_protocol_cam_value1	:23;

	uint32 layer2_protocol_cam_layer3_type2	:4;
	uint32 layer2_protocol_cam_additional_offset2	:4;
	uint32 rsv_2	:1;
	uint32 layer2_protocol_cam_value2	:23;

	uint32 layer2_protocol_cam_layer3_type3	:4;
	uint32 layer2_protocol_cam_additional_offset3	:4;
	uint32 rsv_3	:1;
	uint32 layer2_protocol_cam_value3	:23;

	uint32 layer2_protocol_cam_layer3_type4	:4;
	uint32 layer2_protocol_cam_additional_offset4	:4;
	uint32 rsv_4	:1;
	uint32 layer2_protocol_cam_value4	:23;

	uint32 layer2_protocol_cam_layer3_type5	:4;
	uint32 layer2_protocol_cam_additional_offset5	:4;
	uint32 rsv_5	:1;
	uint32 layer2_protocol_cam_value5	:23;

	uint32 layer2_protocol_cam_layer3_type6	:4;
	uint32 layer2_protocol_cam_additional_offset6	:4;
	uint32 rsv_6	:1;
	uint32 layer2_protocol_cam_value6	:23;

	uint32 layer2_protocol_cam_layer3_type7	:4;
	uint32 layer2_protocol_cam_additional_offset7	:4;
	uint32 rsv_7	:1;
	uint32 layer2_protocol_cam_value7	:23;

	uint32 layer2_protocol_cam_layer3_type8	:4;
	uint32 layer2_protocol_cam_additional_offset8	:4;
	uint32 rsv_8	:1;
	uint32 layer2_protocol_cam_value8	:23;

	uint32 layer2_protocol_cam_layer3_type9	:4;
	uint32 layer2_protocol_cam_additional_offset9	:4;
	uint32 rsv_9	:1;
	uint32 layer2_protocol_cam_value9	:23;

	uint32 layer2_protocol_cam_layer3_type10	:4;
	uint32 layer2_protocol_cam_additional_offset10	:4;
	uint32 rsv_10	:1;
	uint32 layer2_protocol_cam_value10	:23;

	uint32 layer2_protocol_cam_layer3_type11	:4;
	uint32 layer2_protocol_cam_additional_offset11	:4;
	uint32 rsv_11	:1;
	uint32 layer2_protocol_cam_value11	:23;

	uint32 layer2_protocol_cam_layer3_type12	:4;
	uint32 layer2_protocol_cam_additional_offset12	:4;
	uint32 rsv_12	:1;
	uint32 layer2_protocol_cam_value12	:23;

	uint32 layer2_protocol_cam_layer3_type13	:4;
	uint32 layer2_protocol_cam_additional_offset13	:4;
	uint32 rsv_13	:1;
	uint32 layer2_protocol_cam_value13	:23;

	uint32 layer2_protocol_cam_layer3_type14	:4;
	uint32 layer2_protocol_cam_additional_offset14	:4;
	uint32 rsv_14	:1;
	uint32 layer2_protocol_cam_value14	:23;

	uint32 layer2_protocol_cam_layer3_type15	:4;
	uint32 layer2_protocol_cam_additional_offset15	:4;
	uint32 rsv_15	:1;
	uint32 layer2_protocol_cam_value15	:23;

	uint32 rsv_16	:9;
	uint32 layer2_protocol_cam_mask0	:23;

	uint32 rsv_17	:9;
	uint32 layer2_protocol_cam_mask1	:23;

	uint32 rsv_18	:9;
	uint32 layer2_protocol_cam_mask2	:23;

	uint32 rsv_19	:9;
	uint32 layer2_protocol_cam_mask3	:23;

	uint32 rsv_20	:9;
	uint32 layer2_protocol_cam_mask4	:23;

	uint32 rsv_21	:9;
	uint32 layer2_protocol_cam_mask5	:23;

	uint32 rsv_22	:9;
	uint32 layer2_protocol_cam_mask6	:23;

	uint32 rsv_23	:9;
	uint32 layer2_protocol_cam_mask7	:23;

	uint32 rsv_24	:9;
	uint32 layer2_protocol_cam_mask8	:23;

	uint32 rsv_25	:9;
	uint32 layer2_protocol_cam_mask9	:23;

	uint32 rsv_26	:9;
	uint32 layer2_protocol_cam_mask10	:23;

	uint32 rsv_27	:9;
	uint32 layer2_protocol_cam_mask11	:23;

	uint32 rsv_28	:9;
	uint32 layer2_protocol_cam_mask12	:23;

	uint32 rsv_29	:9;
	uint32 layer2_protocol_cam_mask13	:23;

	uint32 rsv_30	:9;
	uint32 layer2_protocol_cam_mask14	:23;

	uint32 rsv_31	:9;
	uint32 layer2_protocol_cam_mask15	:23;
};
typedef struct parser_layer2_protocol_cam_s parser_layer2_protocol_cam_t;

struct parser_layer2_flex_ctl_s	/* 736 */
{

	uint32 rsv_0	:3;
	uint32 layer2_byte_select0	:5;
	uint32 rsv_1	:3;
	uint32 layer2_byte_select1	:5;
	uint32 rsv_2	:3;
	uint32 layer2_byte_select2	:5;
	uint32 rsv_3	:3;
	uint32 layer2_byte_select3	:5;

	uint32 rsv_4	:3;
	uint32 layer2_byte_select4	:5;
	uint32 rsv_5	:3;
	uint32 layer2_byte_select5	:5;
	uint32 rsv_6	:3;
	uint32 layer2_protocol_byte_select0	:5;
	uint32 rsv_7	:3;
	uint32 layer2_protocol_byte_select1	:5;

	uint32 rsv_8	:16;
	uint32 layer2_min_length	:8;
	uint32 layer2_basic_offset	:8;
};
typedef struct parser_layer2_flex_ctl_s parser_layer2_flex_ctl_t;

struct parser_mpls_ctl_s	/* 737 */
{

	uint32 rsv_0	:18;
	uint32 use_mpls_hash	:1;
	uint32 mpls_protocol_ecmp_hash_en	:1;
	uint32 rsv_1	:1;
	uint32 mpls_dscp_ecmp_hash_en	:1;
	uint32 mpls_protocol_hash_en	:1;
	uint32 mpls_flow_label_ecmp_hash_en	:1;
	uint32 rsv_2	:4;
	uint32 mpls_ip_hash_disable	:4;
};
typedef struct parser_mpls_ctl_s parser_mpls_ctl_t;

struct parser_ip_hash_ctl_s	/* 738 */
{

	uint32 rsv_0	:14;
	uint32 small_fragment_offset	:2;
	uint32 rsv_1	:3;
	uint32 use_ip_hash	:1;
	uint32 protocol_ecmp_hash_en	:1;
	uint32 dscp_ecmp_hash_en	:1;
	uint32 protocol_hash_en	:1;
	uint32 flow_label_ecmp_hash_en	:1;
	uint32 rsv_2	:4;
	uint32 ip_hash_disable	:4;
};
typedef struct parser_ip_hash_ctl_s parser_ip_hash_ctl_t;

struct parser_ipv6_ctl_s	/* 739 */
{

	uint32 ipv6_ext_disable	:1;
	uint32 ipv6_ext_level_check_en0	:1;
	uint32 ipv6_set_ip_options0	:1;
	uint32 ipv6_ext_shift0	:1;
	uint32 ipv6_ext_level0	:4;
	uint32 rsv_0	:1;
	uint32 ipv6_ext_level_check_en1	:1;
	uint32 ipv6_set_ip_options1	:1;
	uint32 ipv6_ext_shift1	:1;
	uint32 ipv6_ext_level1	:4;
	uint32 rsv_1	:1;
	uint32 ipv6_ext_level_check_en2	:1;
	uint32 ipv6_set_ip_options2	:1;
	uint32 ipv6_ext_shift2	:1;
	uint32 ipv6_ext_level2	:4;
	uint32 rsv_2	:1;
	uint32 ipv6_ext_level_check_en3	:1;
	uint32 ipv6_set_ip_options3	:1;
	uint32 ipv6_ext_shift3	:1;
	uint32 ipv6_ext_level3	:4;

	uint32 rsv_3	:1;
	uint32 ipv6_ext_level_check_en4	:1;
	uint32 ipv6_set_ip_options4	:1;
	uint32 ipv6_ext_shift4	:1;
	uint32 ipv6_ext_level4	:4;
	uint32 rsv_4	:1;
	uint32 ipv6_ext_level_check_en5	:1;
	uint32 ipv6_set_ip_options5	:1;
	uint32 ipv6_ext_shift5	:1;
	uint32 ipv6_ext_level5	:4;
	uint32 rsv_5	:1;
	uint32 ipv6_ext_level_check_en6	:1;
	uint32 ipv6_set_ip_options6	:1;
	uint32 ipv6_ext_shift6	:1;
	uint32 ipv6_ext_level6	:4;
	uint32 rsv_6	:1;
	uint32 ipv6_ext_level_check_en7	:1;
	uint32 ipv6_set_ip_options7	:1;
	uint32 ipv6_ext_shift7	:1;
	uint32 ipv6_ext_level7	:4;

	uint32 rsv_7	:7;
	uint32 parser_ipv6_error_option_en	:1;
	uint32 reserved_l3_header_protocol	:8;
	uint32 ipv6_flex_ext_header1	:8;
	uint32 ipv6_flex_ext_header0	:8;
};
typedef struct parser_ipv6_ctl_s parser_ipv6_ctl_t;

struct parser_layer3_flex_ctl_s	/* 740 */
{

	uint32 rsv_0	:3;
	uint32 layer3_byte_select0	:5;
	uint32 rsv_1	:3;
	uint32 layer3_byte_select1	:5;
	uint32 rsv_2	:3;
	uint32 layer3_byte_select2	:5;
	uint32 rsv_3	:3;
	uint32 layer3_byte_select3	:5;

	uint32 rsv_4	:3;
	uint32 layer3_byte_select4	:5;
	uint32 rsv_5	:3;
	uint32 layer3_byte_select5	:5;
	uint32 rsv_6	:3;
	uint32 layer3_byte_select6	:5;
	uint32 rsv_7	:3;
	uint32 layer3_byte_select7	:5;

	uint32 rsv_8	:8;
	uint32 layer3_min_length	:8;
	uint32 rsv_9	:3;
	uint32 layer3_protocol_byte_select	:5;
	uint32 layer3_basic_offset	:8;
};
typedef struct parser_layer3_flex_ctl_s parser_layer3_flex_ctl_t;

struct parser_layer3_protocol_cam_valid_s	/* 741 */
{

	uint32 rsv_0	:16;
	uint32 layer3_cam_entry_valid	:16;
};
typedef struct parser_layer3_protocol_cam_valid_s parser_layer3_protocol_cam_valid_t;

struct parser_layer3_protocol_cam_s	/* 742 */
{

	uint32 layer3_protocol_cam_layer4_type0	:4;
	uint32 layer3_protocol_cam_additional_offset0	:4;
	uint32 layer3_protocol_cam_layer3_type_mask0	:4;
	uint32 layer3_protocol_cam_layer3_type0	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask0	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol0	:8;

	uint32 layer3_protocol_cam_layer4_type1	:4;
	uint32 layer3_protocol_cam_additional_offset1	:4;
	uint32 layer3_protocol_cam_layer3_type_mask1	:4;
	uint32 layer3_protocol_cam_layer3_type1	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask1	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol1	:8;

	uint32 layer3_protocol_cam_layer4_type2	:4;
	uint32 layer3_protocol_cam_additional_offset2	:4;
	uint32 layer3_protocol_cam_layer3_type_mask2	:4;
	uint32 layer3_protocol_cam_layer3_type2	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask2	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol2	:8;

	uint32 layer3_protocol_cam_layer4_type3	:4;
	uint32 layer3_protocol_cam_additional_offset3	:4;
	uint32 layer3_protocol_cam_layer3_type_mask3	:4;
	uint32 layer3_protocol_cam_layer3_type3	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask3	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol3	:8;

	uint32 layer3_protocol_cam_layer4_type4	:4;
	uint32 layer3_protocol_cam_additional_offset4	:4;
	uint32 layer3_protocol_cam_layer3_type_mask4	:4;
	uint32 layer3_protocol_cam_layer3_type4	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask4	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol4	:8;

	uint32 layer3_protocol_cam_layer4_type5	:4;
	uint32 layer3_protocol_cam_additional_offset5	:4;
	uint32 layer3_protocol_cam_layer3_type_mask5	:4;
	uint32 layer3_protocol_cam_layer3_type5	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask5	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol5	:8;

	uint32 layer3_protocol_cam_layer4_type6	:4;
	uint32 layer3_protocol_cam_additional_offset6	:4;
	uint32 layer3_protocol_cam_layer3_type_mask6	:4;
	uint32 layer3_protocol_cam_layer3_type6	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask6	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol6	:8;

	uint32 layer3_protocol_cam_layer4_type7	:4;
	uint32 layer3_protocol_cam_additional_offset7	:4;
	uint32 layer3_protocol_cam_layer3_type_mask7	:4;
	uint32 layer3_protocol_cam_layer3_type7	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask7	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol7	:8;

	uint32 layer3_protocol_cam_layer4_type8	:4;
	uint32 layer3_protocol_cam_additional_offset8	:4;
	uint32 layer3_protocol_cam_layer3_type_mask8	:4;
	uint32 layer3_protocol_cam_layer3_type8	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask8	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol8	:8;

	uint32 layer3_protocol_cam_layer4_type9	:4;
	uint32 layer3_protocol_cam_additional_offset9	:4;
	uint32 layer3_protocol_cam_layer3_type_mask9	:4;
	uint32 layer3_protocol_cam_layer3_type9	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask9	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol9	:8;

	uint32 layer3_protocol_cam_layer4_type10	:4;
	uint32 layer3_protocol_cam_additional_offset10	:4;
	uint32 layer3_protocol_cam_layer3_type_mask10	:4;
	uint32 layer3_protocol_cam_layer3_type10	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask10	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol10	:8;

	uint32 layer3_protocol_cam_layer4_type11	:4;
	uint32 layer3_protocol_cam_additional_offset11	:4;
	uint32 layer3_protocol_cam_layer3_type_mask11	:4;
	uint32 layer3_protocol_cam_layer3_type11	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask11	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol11	:8;

	uint32 layer3_protocol_cam_layer4_type12	:4;
	uint32 layer3_protocol_cam_additional_offset12	:4;
	uint32 layer3_protocol_cam_layer3_type_mask12	:4;
	uint32 layer3_protocol_cam_layer3_type12	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask12	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol12	:8;

	uint32 layer3_protocol_cam_layer4_type13	:4;
	uint32 layer3_protocol_cam_additional_offset13	:4;
	uint32 layer3_protocol_cam_layer3_type_mask13	:4;
	uint32 layer3_protocol_cam_layer3_type13	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask13	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol13	:8;

	uint32 layer3_protocol_cam_layer4_type14	:4;
	uint32 layer3_protocol_cam_additional_offset14	:4;
	uint32 layer3_protocol_cam_layer3_type_mask14	:4;
	uint32 layer3_protocol_cam_layer3_type14	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask14	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol14	:8;

	uint32 layer3_protocol_cam_layer4_type15	:4;
	uint32 layer3_protocol_cam_additional_offset15	:4;
	uint32 layer3_protocol_cam_layer3_type_mask15	:4;
	uint32 layer3_protocol_cam_layer3_type15	:4;
	uint32 layer3_protocol_cam_layer3_header_protocol_mask15	:8;
	uint32 layer3_protocol_cam_layer3_header_protocol15	:8;
};
typedef struct parser_layer3_protocol_cam_s parser_layer3_protocol_cam_t;

struct parser_application_cam_s	/* 743 */
{

	uint32 rsv_0	:2;
	uint32 application_cam_application_type0	:1;
	uint32 application_cam_byte0_select0	:5;
	uint32 application_cam_byte_select_dest0	:1;
	uint32 application_cam_entry_valid0	:1;
	uint32 rsv_1	:1;
	uint32 application_cam_byte1_select0	:5;
	uint32 application_cam_source_port_mask0	:1;
	uint32 application_cam_dest_port_mask0	:1;
	uint32 application_cam_tcp_flag_mask0	:6;
	uint32 application_cam_is_tcp_mask0	:1;
	uint32 application_cam_is_tcp_value0	:1;
	uint32 application_cam_tcp_flag_value0	:6;

	uint32 rsv_2	:2;
	uint32 application_cam_application_type1	:1;
	uint32 application_cam_byte0_select1	:5;
	uint32 application_cam_byte_select_dest1	:1;
	uint32 application_cam_entry_valid1	:1;
	uint32 rsv_3	:1;
	uint32 application_cam_byte1_select1	:5;
	uint32 application_cam_source_port_mask1	:1;
	uint32 application_cam_dest_port_mask1	:1;
	uint32 application_cam_tcp_flag_mask1	:6;
	uint32 application_cam_is_tcp_mask1	:1;
	uint32 application_cam_is_tcp_value1	:1;
	uint32 application_cam_tcp_flag_value1	:6;

	uint32 application_cam_source_port_value0	:16;
	uint32 application_cam_dest_port_value0	:16;

	uint32 application_cam_source_port_value1	:16;
	uint32 application_cam_dest_port_value1	:16;
};
typedef struct parser_application_cam_s parser_application_cam_t;

struct parser_l4_hash_ctl_s	/* 744 */
{

	uint32 rsv_0	:27;
	uint32 use_layer4_hash	:1;
	uint32 source_port_ecmp_hash_en	:1;
	uint32 dest_port_ecmp_hash_en	:1;
	uint32 source_port_hash_en	:1;
	uint32 dest_port_hash_en	:1;
};
typedef struct parser_l4_hash_ctl_s parser_l4_hash_ctl_t;

struct parser_layer4_flag_op_ctl_s	/* 745 */
{

	uint32 rsv_0	:23;
	uint32 layer4_op_and_or0	:1;
	uint32 rsv_1	:2;
	uint32 layer4_op_flags_mask0	:6;

	uint32 rsv_2	:23;
	uint32 layer4_op_and_or1	:1;
	uint32 rsv_3	:2;
	uint32 layer4_op_flags_mask1	:6;

	uint32 rsv_4	:23;
	uint32 layer4_op_and_or2	:1;
	uint32 rsv_5	:2;
	uint32 layer4_op_flags_mask2	:6;

	uint32 rsv_6	:23;
	uint32 layer4_op_and_or3	:1;
	uint32 rsv_7	:2;
	uint32 layer4_op_flags_mask3	:6;
};
typedef struct parser_layer4_flag_op_ctl_s parser_layer4_flag_op_ctl_t;

struct parser_layer4_port_op_sel_s	/* 746 */
{

	uint32 rsv_0	:24;
	uint32 layer4_op_dest_port	:8;
};
typedef struct parser_layer4_port_op_sel_s parser_layer4_port_op_sel_t;

struct parser_layer4_port_op_ctl_s	/* 747 */
{

	uint32 layer4_op_port_max0	:16;
	uint32 layer4_op_port_min0	:16;

	uint32 layer4_op_port_max1	:16;
	uint32 layer4_op_port_min1	:16;

	uint32 layer4_op_port_max2	:16;
	uint32 layer4_op_port_min2	:16;

	uint32 layer4_op_port_max3	:16;
	uint32 layer4_op_port_min3	:16;

	uint32 layer4_op_port_max4	:16;
	uint32 layer4_op_port_min4	:16;

	uint32 layer4_op_port_max5	:16;
	uint32 layer4_op_port_min5	:16;

	uint32 layer4_op_port_max6	:16;
	uint32 layer4_op_port_min6	:16;

	uint32 layer4_op_port_max7	:16;
	uint32 layer4_op_port_min7	:16;
};
typedef struct parser_layer4_port_op_ctl_s parser_layer4_port_op_ctl_t;

struct parser_udp_app_op_ctl_s	/* 748 */
{

	uint32 udp_app_mask0	:16;
	uint32 udp_app_value0	:16;

	uint32 udp_app_mask1	:16;
	uint32 udp_app_value1	:16;

	uint32 udp_app_mask2	:16;
	uint32 udp_app_value2	:16;

	uint32 udp_app_mask3	:16;
	uint32 udp_app_value3	:16;
};
typedef struct parser_udp_app_op_ctl_s parser_udp_app_op_ctl_t;

struct parser_layer4_length_op_ctl_s	/* 749 */
{

	uint32 rsv_0	:18;
	uint32 layer4_op_length1	:14;

	uint32 rsv_1	:18;
	uint32 layer4_op_length2	:14;

	uint32 rsv_2	:18;
	uint32 layer4_op_length3	:14;

	uint32 rsv_3	:18;
	uint32 layer4_op_length4	:14;

	uint32 rsv_4	:18;
	uint32 layer4_op_length5	:14;

	uint32 rsv_5	:18;
	uint32 layer4_op_length6	:14;

	uint32 rsv_6	:18;
	uint32 layer4_op_length7	:14;

	uint32 rsv_7	:18;
	uint32 layer4_op_length8	:14;

	uint32 rsv_8	:18;
	uint32 layer4_op_length9	:14;

	uint32 rsv_9	:18;
	uint32 layer4_op_length10	:14;

	uint32 rsv_10	:18;
	uint32 layer4_op_length11	:14;

	uint32 rsv_11	:18;
	uint32 layer4_op_length12	:14;

	uint32 rsv_12	:18;
	uint32 layer4_op_length13	:14;

	uint32 rsv_13	:18;
	uint32 layer4_op_length14	:14;

	uint32 rsv_14	:18;
	uint32 layer4_op_length15	:14;
};
typedef struct parser_layer4_length_op_ctl_s parser_layer4_length_op_ctl_t;

struct parser_layer4_flex_ctl_s	/* 750 */
{

	uint32 rsv_0	:3;
	uint32 layer4_app_min_length	:5;
	uint32 rsv_1	:3;
	uint32 layer4_min_length	:5;
	uint32 rsv_2	:3;
	uint32 layer4_byte_select0	:5;
	uint32 rsv_3	:3;
	uint32 layer4_byte_select1	:5;
};
typedef struct parser_layer4_flex_ctl_s parser_layer4_flex_ctl_t;

struct parser_layer4_ptp_ctl_s	/* 751 */
{

	uint32 rsv_0	:31;
	uint32 ptp_en	:1;

	uint32 ptp_port0	:16;
	uint32 ptp_port1	:16;
};
typedef struct parser_layer4_ptp_ctl_s parser_layer4_ptp_ctl_t;

struct parser_interrupt_s	/* 752 */
{

	uint32 rsv_0	:14;
	uint32 ipe_mask_set	:18;

	uint32 rsv_1	:14;
	uint32 ipe_mask_reset	:18;

	uint32 rsv_2	:14;
	uint32 ipe_value_set	:18;

	uint32 rsv_3	:14;
	uint32 ipe_value_reset	:18;

	uint32 rsv_4	:14;
	uint32 epe_mask_set	:18;

	uint32 rsv_5	:14;
	uint32 epe_mask_reset	:18;

	uint32 rsv_6	:14;
	uint32 epe_value_set	:18;

	uint32 rsv_7	:14;
	uint32 epe_value_reset	:18;
};
typedef struct parser_interrupt_s parser_interrupt_t;

struct parser_debug_s	/* 753 */
{

	uint32 rsv_0	:28;
	uint32 ipe_in_pkt_cnt	:4;

	uint32 rsv_1	:28;
	uint32 ipe_out_pkt_cnt	:4;

	uint32 rsv_2	:28;
	uint32 epe_in_pkt_cnt	:4;

	uint32 rsv_3	:28;
	uint32 epe_out_pkt_cnt	:4;
};
typedef struct parser_debug_s parser_debug_t;

struct parser_state_s	/* 754 */
{

	uint32 rsv_0	:25;
	uint32 ipe_parser_l2_fsm_ps	:7;

	uint32 rsv_1	:25;
	uint32 ipe_parser_l3_fsm_ps	:7;

	uint32 rsv_2	:25;
	uint32 ipe_parser_l4_fsm_ps	:7;

	uint32 rsv_3	:25;
	uint32 epe_parser_l2_fsm_ps	:7;

	uint32 rsv_4	:25;
	uint32 epe_parser_l3_fsm_ps	:7;

	uint32 rsv_5	:25;
	uint32 epe_parser_l4_fsm_ps	:7;
};
typedef struct parser_state_s parser_state_t;

struct pb_ctl_interrupt_s	/* 755 */
{

	uint32 rsv_0	:26;
	uint32 mask_set	:6;

	uint32 rsv_1	:26;
	uint32 mask_reset	:6;

	uint32 rsv_2	:26;
	uint32 value_set	:6;

	uint32 rsv_3	:26;
	uint32 value_reset	:6;
};
typedef struct pb_ctl_interrupt_s pb_ctl_interrupt_t;

struct pb_ctl_init_s	/* 756 */
{

	uint32 rsv_0	:31;
	uint32 tab_init	:1;
};
typedef struct pb_ctl_init_s pb_ctl_init_t;

struct pb_ctl_init_done_s	/* 757 */
{

	uint32 rsv_0	:31;
	uint32 tab_init	:1;
};
typedef struct pb_ctl_init_done_s pb_ctl_init_done_t;

struct pb_ctl_page_address_s	/* 758 */
{

	uint32 rsv_0	:23;
	uint32 page_address	:9;
};
typedef struct pb_ctl_page_address_s pb_ctl_page_address_t;

struct pb_ctl_refresh_interval_s	/* 759 */
{

	uint32 rsv_0	:24;
	uint32 ref_interval	:8;
};
typedef struct pb_ctl_refresh_interval_s pb_ctl_refresh_interval_t;

struct pb_ctl_refresh_enable_s	/* 760 */
{

	uint32 rsv_0	:27;
	uint32 ref_consecutive	:1;
	uint32 rsv_1	:3;
	uint32 ref_enable	:1;
};
typedef struct pb_ctl_refresh_enable_s pb_ctl_refresh_enable_t;

struct pb_ctl_weight_cfg_s	/* 761 */
{

	uint32 rsv_0	:24;
	uint32 write_weight_cfg	:8;

	uint32 rsv_1	:24;
	uint32 read_weight_cfg	:8;
};
typedef struct pb_ctl_weight_cfg_s pb_ctl_weight_cfg_t;

struct pb_ctl_bank_offset_sel_s	/* 762 */
{

	uint32 rsv_0	:31;
	uint32 rand_bank_offset_sel	:1;
};
typedef struct pb_ctl_bank_offset_sel_s pb_ctl_bank_offset_sel_t;

struct pb_ctl_input_stats_s	/* 763 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_req_cnt	:4;

	uint32 rsv_1	:28;
	uint32 buf_retrv_req_cnt	:4;

	uint32 rsv_2	:12;
	uint32 fr_buf_store_ipe_sop_cnt	:4;
	uint32 rsv_3	:12;
	uint32 fr_buf_store_eop_cnt	:4;

	uint32 rsv_4	:28;
	uint32 to_buf_store_eop_cnt	:4;
};
typedef struct pb_ctl_input_stats_s pb_ctl_input_stats_t;

struct pb_ctl_output_stats_s	/* 764 */
{

	uint32 rsv_0	:28;
	uint32 pb_ctl_wr_valid_cnt	:4;

	uint32 rsv_1	:28;
	uint32 pb_ctl_rd_valid_cnt	:4;
};
typedef struct pb_ctl_output_stats_s pb_ctl_output_stats_t;

struct pb_ctl_request_hold_stats_s	/* 765 */
{

	uint32 rsv_0	:16;
	uint32 pb_ctl_wr_hold_cnt	:16;

	uint32 rsv_1	:16;
	uint32 pb_ctl_rd_hold_cnt	:16;
};
typedef struct pb_ctl_request_hold_stats_s pb_ctl_request_hold_stats_t;

struct pci_vendor_id_s	/* 773 */
{

	uint32 device_id	:16;
	uint32 vendor_id	:16;
};
typedef struct pci_vendor_id_s pci_vendor_id_t;

struct pci_cmd_s	/* 774 */
{

	uint32 status	:16;
	uint32 cmd	:16;
};
typedef struct pci_cmd_s pci_cmd_t;

struct pci_rev_id_s	/* 775 */
{

	uint32 class_code	:24;
	uint32 rev_id	:8;
};
typedef struct pci_rev_id_s pci_rev_id_t;

struct pci_header_type_s	/* 776 */
{

	uint32 header_type	:32;
};
typedef struct pci_header_type_s pci_header_type_t;

struct pci_base_addr_s	/* 777 */
{

	uint32 base_addr	:32;
};
typedef struct pci_base_addr_s pci_base_addr_t;

struct pci_subsystem_vendor_id_s	/* 778 */
{

	uint32 sub_system_id	:16;
	uint32 sub_vendor_id	:16;
};
typedef struct pci_subsystem_vendor_id_s pci_subsystem_vendor_id_t;

struct pci_sm_s	/* 783 */
{

	uint32 rsv_0	:27;
	uint32 ps_turnar_state	:1;
	uint32 ps_idle_state	:1;
	uint32 ps_data_state	:1;
	uint32 ps_busy_state	:1;
	uint32 ps_backoff_state	:1;
};
typedef struct pci_sm_s pci_sm_t;

struct policing_ctrl0_s	/* 784 */
{

	uint32 rsv_0	:4;
	uint32 ts_tick_gen_interval	:12;
	uint32 rsv_1	:3;
	uint32 ipg_en	:1;
	uint32 update_interval0	:4;
	uint32 ts_tick_gen_en	:1;
	uint32 parity_check_en	:1;
	uint32 min_length_check_en	:1;
	uint32 sequential_policing	:1;
	uint32 stats_en_violate	:1;
	uint32 stats_en_confirm	:1;
	uint32 stats_en_not_confirm	:1;
	uint32 update_en	:1;
};
typedef struct policing_ctrl0_s policing_ctrl0_t;

struct policing_ctrl1_s	/* 785 */
{

	uint32 max_ptr0	:16;
	uint32 min_ptr0	:16;
};
typedef struct policing_ctrl1_s policing_ctrl1_t;

struct policing_ds_policer_access_s	/* 786 */
{

	uint32 rsv_0	:2;
	uint32 cpu_req	:1;
	uint32 cpu_read_data_valid	:1;
	uint32 rsv_1	:3;
	uint32 cpu_req_type	:1;
	uint32 rsv_2	:8;
	uint32 cpu_index	:16;
};
typedef struct policing_ds_policer_access_s policing_ds_policer_access_t;

struct policing_ds_policer_wr00_s	/* 787 */
{

	uint32 rsv_0	:28;
	uint32 policer_wr_data0	:4;
};
typedef struct policing_ds_policer_wr00_s policing_ds_policer_wr00_t;

struct policing_ds_policer_wr01_s	/* 788 */
{

	uint32 policer_wr_data1	:32;
};
typedef struct policing_ds_policer_wr01_s policing_ds_policer_wr01_t;

struct policing_ds_policer_wr02_s	/* 789 */
{

	uint32 rsv_0	:28;
	uint32 policer_wr_data2	:4;
};
typedef struct policing_ds_policer_wr02_s policing_ds_policer_wr02_t;

struct policing_ds_policer_wr03_s	/* 790 */
{

	uint32 policer_wr_data3	:32;
};
typedef struct policing_ds_policer_wr03_s policing_ds_policer_wr03_t;

struct policing_ds_policer_rd00_s	/* 791 */
{

	uint32 rsv_0	:28;
	uint32 policer_rd_data0	:4;
};
typedef struct policing_ds_policer_rd00_s policing_ds_policer_rd00_t;

struct policing_ds_policer_rd01_s	/* 792 */
{

	uint32 policer_rd_data1	:32;
};
typedef struct policing_ds_policer_rd01_s policing_ds_policer_rd01_t;

struct policing_ds_policer_rd02_s	/* 793 */
{

	uint32 rsv_0	:28;
	uint32 policer_rd_data2	:4;
};
typedef struct policing_ds_policer_rd02_s policing_ds_policer_rd02_t;

struct policing_ds_policer_rd03_s	/* 794 */
{

	uint32 policer_rd_data3	:32;
};
typedef struct policing_ds_policer_rd03_s policing_ds_policer_rd03_t;

struct policing_ipe_epe_fifo_thrd_s	/* 795 */
{

	uint32 rsv_0	:11;
	uint32 ipe_fifo_thrd	:5;
	uint32 rsv_1	:11;
	uint32 epe_fifo_thrd	:5;
};
typedef struct policing_ipe_epe_fifo_thrd_s policing_ipe_epe_fifo_thrd_t;

struct policing_ext_base_ptr_s	/* 796 */
{

	uint32 rsv_0	:12;
	uint32 ext_base_ptr	:20;
};
typedef struct policing_ext_base_ptr_s policing_ext_base_ptr_t;

struct policing_stats_confirm_base_ptr_s	/* 797 */
{

	uint32 rsv_0	:20;
	uint32 stats_confirm_base_ptr	:12;
};
typedef struct policing_stats_confirm_base_ptr_s policing_stats_confirm_base_ptr_t;

struct policing_stats_not_confirm_base_ptr_s	/* 798 */
{

	uint32 rsv_0	:4;
	uint32 stats_violate_base_ptr	:12;
	uint32 rsv_1	:4;
	uint32 stats_not_confirm_base_ptr	:12;
};
typedef struct policing_stats_not_confirm_base_ptr_s policing_stats_not_confirm_base_ptr_t;

struct policing_intr0_value_set_s	/* 799 */
{

	uint32 rsv_0	:12;
	uint32 intr0_value_set	:20;
};
typedef struct policing_intr0_value_set_s policing_intr0_value_set_t;

struct policing_intr0_value_reset_s	/* 800 */
{

	uint32 rsv_0	:12;
	uint32 intr0_value_reset	:20;
};
typedef struct policing_intr0_value_reset_s policing_intr0_value_reset_t;

struct policing_intr0_mask_set_s	/* 801 */
{

	uint32 rsv_0	:12;
	uint32 intr0_mask_set	:20;
};
typedef struct policing_intr0_mask_set_s policing_intr0_mask_set_t;

struct policing_intr0_mask_reset_s	/* 802 */
{

	uint32 rsv_0	:12;
	uint32 intr0_mask_reset	:20;
};
typedef struct policing_intr0_mask_reset_s policing_intr0_mask_reset_t;

struct policing_intr1_value_set_s	/* 803 */
{

	uint32 rsv_0	:3;
	uint32 intr1_value_set	:29;
};
typedef struct policing_intr1_value_set_s policing_intr1_value_set_t;

struct policing_intr1_value_reset_s	/* 804 */
{

	uint32 rsv_0	:3;
	uint32 intr1_value_reset	:29;
};
typedef struct policing_intr1_value_reset_s policing_intr1_value_reset_t;

struct policing_intr1_mask_set_s	/* 805 */
{

	uint32 rsv_0	:3;
	uint32 intr1_mask_set	:29;
};
typedef struct policing_intr1_mask_set_s policing_intr1_mask_set_t;

struct policing_intr1_mask_reset_s	/* 806 */
{

	uint32 rsv_0	:3;
	uint32 intr1_mask_reset	:29;
};
typedef struct policing_intr1_mask_reset_s policing_intr1_mask_reset_t;

struct policing_cfg_rd_credit_s	/* 807 */
{

	uint32 rsv_0	:20;
	uint32 cfg_int_rd_credit	:4;
	uint32 rsv_1	:4;
	uint32 cfg_ext_rd_credit	:4;
};
typedef struct policing_cfg_rd_credit_s policing_cfg_rd_credit_t;

struct policing_running_rd_credit_s	/* 808 */
{

	uint32 rsv_0	:20;
	uint32 running_int_rd_credit	:4;
	uint32 rsv_1	:4;
	uint32 running_ext_rd_credit	:4;
};
typedef struct policing_running_rd_credit_s policing_running_rd_credit_t;

struct policing_cfg_wr_credit_s	/* 809 */
{

	uint32 rsv_0	:20;
	uint32 cfg_int_wr_credit	:4;
	uint32 rsv_1	:4;
	uint32 cfg_ext_wr_credit	:4;
};
typedef struct policing_cfg_wr_credit_s policing_cfg_wr_credit_t;

struct policing_running_wr_credit_s	/* 810 */
{

	uint32 rsv_0	:20;
	uint32 running_int_wr_credit	:4;
	uint32 rsv_1	:4;
	uint32 running_ext_wr_credit	:4;
};
typedef struct policing_running_wr_credit_s policing_running_wr_credit_t;

struct policing_cache_clear_s	/* 811 */
{

	uint32 rsv_0	:31;
	uint32 cache_clear	:1;
};
typedef struct policing_cache_clear_s policing_cache_clear_t;

struct policing_hierarchy_en_s	/* 812 */
{

	uint32 rsv_0	:31;
	uint32 hierarchy_en	:1;
};
typedef struct policing_hierarchy_en_s policing_hierarchy_en_t;

struct policing_pending_credit_s	/* 813 */
{

	uint32 rsv_0	:11;
	uint32 ext_pending_credit	:5;
	uint32 rsv_1	:11;
	uint32 int_pending_credit	:5;
};
typedef struct policing_pending_credit_s policing_pending_credit_t;

struct ptp_frc_ctl_s	/* 814 */
{

	uint32 rsv_0	:27;
	uint32 ptp_ntp_v4_en	:1;
	uint32 rsv_1	:3;
	uint32 frc_en	:1;
};
typedef struct ptp_frc_ctl_s ptp_frc_ctl_t;

struct ptp_frc_s	/* 815 */
{

	uint32 frc_second	:32;

	uint32 frc_ns	:32;

	uint32 rsv_0	:2;
	uint32 frc_frac_ns	:30;
};
typedef struct ptp_frc_s ptp_frc_t;

struct ptp_quanta_s	/* 816 */
{

	uint32 rsv_0	:24;
	uint32 quanta	:8;
};
typedef struct ptp_quanta_s ptp_quanta_t;

struct ptp_drift_adjust_s	/* 817 */
{

	uint32 sign	:1;
	uint32 rsv_0	:1;
	uint32 drift_rate	:30;
};
typedef struct ptp_drift_adjust_s ptp_drift_adjust_t;

struct ptp_offset_adjust_s	/* 818 */
{

	uint32 offset_second	:32;

	uint32 offset_ns	:32;
};
typedef struct ptp_offset_adjust_s ptp_offset_adjust_t;

struct ptp_mac_tx_capture_ts_ctl_s	/* 819 */
{

	uint32 rsv_0	:31;
	uint32 ignore_tx_rdy	:1;
};
typedef struct ptp_mac_tx_capture_ts_ctl_s ptp_mac_tx_capture_ts_ctl_t;

struct ptp_mac_tx_capture_ts_status_s	/* 820 */
{

	uint32 rsv_0	:27;
	uint32 ptp_msg_tx_rdy	:1;
	uint32 rsv_1	:2;
	uint32 overflow	:2;
};
typedef struct ptp_mac_tx_capture_ts_status_s ptp_mac_tx_capture_ts_status_t;

struct ptp_mac_tx_capture_ts_s	/* 821 */
{

	uint32 tx_ts_second	:32;

	uint32 tx_ts_ns	:32;

	uint32 rsv_0	:26;
	uint32 tx_mac_num	:6;
};
typedef struct ptp_mac_tx_capture_ts_s ptp_mac_tx_capture_ts_t;

struct ptp_sync_intf_cfg_s	/* 822 */
{

	uint32 rsv_0	:3;
	uint32 sync_intf_clk_en	:1;
	uint32 rsv_1	:3;
	uint32 sync_intf_mode	:1;
	uint32 rsv_2	:3;
	uint32 time_code_enable	:1;
	uint32 rsv_3	:3;
	uint32 lock	:1;
	uint32 rsv_4	:2;
	uint32 epoch	:6;
	uint32 accuracy	:8;
};
typedef struct ptp_sync_intf_cfg_s ptp_sync_intf_cfg_t;

struct ptp_sync_intf_half_period_s	/* 823 */
{

	uint32 sync_ns	:32;

	uint32 rsv_0	:2;
	uint32 sync_frac_ns	:30;
};
typedef struct ptp_sync_intf_half_period_s ptp_sync_intf_half_period_t;

struct ptp_sync_intf_toggle_time_s	/* 824 */
{

	uint32 sync_intf_toggle_second	:32;

	uint32 sync_intf_toggle_ns	:32;

	uint32 rsv_0	:2;
	uint32 sync_intf_toggle_frac_ns	:30;
};
typedef struct ptp_sync_intf_toggle_time_s ptp_sync_intf_toggle_time_t;

struct ptp_sync_intf_heart_beat_cfg_s	/* 825 */
{

	uint32 rsv_0	:3;
	uint32 heart_beat_enable	:1;
	uint32 rsv_1	:3;
	uint32 heart_beat_threshold	:25;
};
typedef struct ptp_sync_intf_heart_beat_cfg_s ptp_sync_intf_heart_beat_cfg_t;

struct ptp_sync_intf_input_ts_s	/* 826 */
{

	uint32 data31_to0	:32;

	uint32 data63_to32	:32;

	uint32 rsv_0	:7;
	uint32 data88_to64	:25;

	uint32 rsv_1	:23;
	uint32 crc_err	:1;
	uint32 crc	:8;
};
typedef struct ptp_sync_intf_input_ts_s ptp_sync_intf_input_ts_t;

struct ptp_sync_intf_capture_ctl_s	/* 827 */
{

	uint32 rsv_0	:27;
	uint32 intr_en	:1;
	uint32 rsv_1	:2;
	uint32 ts_capture_mode	:2;
};
typedef struct ptp_sync_intf_capture_ctl_s ptp_sync_intf_capture_ctl_t;

struct ptp_sync_intf_capture_frc_ts_s	/* 828 */
{

	uint32 capture_frc_second	:32;

	uint32 capture_frc_ns	:32;
};
typedef struct ptp_sync_intf_capture_frc_ts_s ptp_sync_intf_capture_frc_ts_t;

struct ptp_sync_intf_capture_adj_frc_ts_s	/* 829 */
{

	uint32 capture_adj_frc_second	:32;

	uint32 capture_adj_frc_ns	:32;
};
typedef struct ptp_sync_intf_capture_adj_frc_ts_s ptp_sync_intf_capture_adj_frc_ts_t;

struct ptp_interrupt_s	/* 830 */
{

	uint32 rsv_0	:24;
	uint32 value_set	:8;

	uint32 rsv_1	:24;
	uint32 value_reset	:8;

	uint32 rsv_2	:24;
	uint32 mask_set	:8;

	uint32 rsv_3	:24;
	uint32 mask_reset	:8;
};
typedef struct ptp_interrupt_s ptp_interrupt_t;

struct ptp_mac_rx_capture_ts_s	/* 831 */
{

	uint32 rx_ts_ns31_roll_over_bit	:1;
	uint32 rx_ts_ns30_roll_over_bit	:1;
	uint32 rx_ts_ns29_roll_over_bit	:1;
	uint32 rx_ts_ns28_roll_over_bit	:1;
	uint32 rx_ts_ns27_roll_over_bit	:1;
	uint32 rx_ts_ns26_roll_over_bit	:1;
	uint32 rx_ts_ns25_roll_over_bit	:1;
	uint32 rx_ts_ns24_roll_over_bit	:1;
	uint32 rx_ts_ns23_roll_over_bit	:1;
	uint32 rx_ts_ns22_roll_over_bit	:1;
	uint32 rx_ts_ns21_roll_over_bit	:1;
	uint32 rx_ts_ns20_roll_over_bit	:1;
	uint32 rx_ts_ns19_roll_over_bit	:1;
	uint32 rx_ts_ns18_roll_over_bit	:1;
	uint32 rx_ts_ns17_roll_over_bit	:1;
	uint32 rx_ts_ns16_roll_over_bit	:1;
	uint32 rx_ts_ns15_roll_over_bit	:1;
	uint32 rx_ts_ns14_roll_over_bit	:1;
	uint32 rx_ts_ns13_roll_over_bit	:1;
	uint32 rx_ts_ns12_roll_over_bit	:1;
	uint32 rx_ts_ns11_roll_over_bit	:1;
	uint32 rx_ts_ns10_roll_over_bit	:1;
	uint32 rx_ts_ns9_roll_over_bit	:1;
	uint32 rx_ts_ns8_roll_over_bit	:1;
	uint32 rx_ts_ns7_roll_over_bit	:1;
	uint32 rx_ts_ns6_roll_over_bit	:1;
	uint32 rx_ts_ns5_roll_over_bit	:1;
	uint32 rx_ts_ns4_roll_over_bit	:1;
	uint32 rx_ts_ns3_roll_over_bit	:1;
	uint32 rx_ts_ns2_roll_over_bit	:1;
	uint32 rx_ts_ns1_roll_over_bit	:1;
	uint32 rx_ts_ns0_roll_over_bit	:1;

	uint32 rsv_0	:12;
	uint32 rx_ts_ns51_roll_over_bit	:1;
	uint32 rx_ts_ns50_roll_over_bit	:1;
	uint32 rx_ts_ns49_roll_over_bit	:1;
	uint32 rx_ts_ns48_roll_over_bit	:1;
	uint32 rx_ts_ns47_roll_over_bit	:1;
	uint32 rx_ts_ns46_roll_over_bit	:1;
	uint32 rx_ts_ns45_roll_over_bit	:1;
	uint32 rx_ts_ns44_roll_over_bit	:1;
	uint32 rx_ts_ns43_roll_over_bit	:1;
	uint32 rx_ts_ns42_roll_over_bit	:1;
	uint32 rx_ts_ns41_roll_over_bit	:1;
	uint32 rx_ts_ns40_roll_over_bit	:1;
	uint32 rx_ts_ns39_roll_over_bit	:1;
	uint32 rx_ts_ns38_roll_over_bit	:1;
	uint32 rx_ts_ns37_roll_over_bit	:1;
	uint32 rx_ts_ns36_roll_over_bit	:1;
	uint32 rx_ts_ns35_roll_over_bit	:1;
	uint32 rx_ts_ns34_roll_over_bit	:1;
	uint32 rx_ts_ns33_roll_over_bit	:1;
	uint32 rx_ts_ns32_roll_over_bit	:1;

	uint32 rx_ts_ns0	:32;

	uint32 rx_ts_ns1	:32;

	uint32 rx_ts_ns2	:32;

	uint32 rx_ts_ns3	:32;

	uint32 rx_ts_ns4	:32;

	uint32 rx_ts_ns5	:32;

	uint32 rx_ts_ns6	:32;

	uint32 rx_ts_ns7	:32;

	uint32 rx_ts_ns8	:32;

	uint32 rx_ts_ns9	:32;

	uint32 rx_ts_ns10	:32;

	uint32 rx_ts_ns11	:32;

	uint32 rx_ts_ns12	:32;

	uint32 rx_ts_ns13	:32;

	uint32 rx_ts_ns14	:32;

	uint32 rx_ts_ns15	:32;

	uint32 rx_ts_ns16	:32;

	uint32 rx_ts_ns17	:32;

	uint32 rx_ts_ns18	:32;

	uint32 rx_ts_ns19	:32;

	uint32 rx_ts_ns20	:32;

	uint32 rx_ts_ns21	:32;

	uint32 rx_ts_ns22	:32;

	uint32 rx_ts_ns23	:32;

	uint32 rx_ts_ns24	:32;

	uint32 rx_ts_ns25	:32;

	uint32 rx_ts_ns26	:32;

	uint32 rx_ts_ns27	:32;

	uint32 rx_ts_ns28	:32;

	uint32 rx_ts_ns29	:32;

	uint32 rx_ts_ns30	:32;

	uint32 rx_ts_ns31	:32;

	uint32 rx_ts_ns32	:32;

	uint32 rx_ts_ns33	:32;

	uint32 rx_ts_ns34	:32;

	uint32 rx_ts_ns35	:32;

	uint32 rx_ts_ns36	:32;

	uint32 rx_ts_ns37	:32;

	uint32 rx_ts_ns38	:32;

	uint32 rx_ts_ns39	:32;

	uint32 rx_ts_ns40	:32;

	uint32 rx_ts_ns41	:32;

	uint32 rx_ts_ns42	:32;

	uint32 rx_ts_ns43	:32;

	uint32 rx_ts_ns44	:32;

	uint32 rx_ts_ns45	:32;

	uint32 rx_ts_ns46	:32;

	uint32 rx_ts_ns47	:32;

	uint32 rx_ts_ns48	:32;

	uint32 rx_ts_ns49	:32;

	uint32 rx_ts_ns50	:32;

	uint32 rx_ts_ns51	:32;
};
typedef struct ptp_mac_rx_capture_ts_s ptp_mac_rx_capture_ts_t;

struct qdr_arb_interrupt_s	/* 832 */
{

	uint32 rsv_0	:24;
	uint32 value_set	:8;

	uint32 rsv_1	:24;
	uint32 value_reset	:8;

	uint32 rsv_2	:24;
	uint32 mask_set	:8;

	uint32 rsv_3	:24;
	uint32 mask_reset	:8;
};
typedef struct qdr_arb_interrupt_s qdr_arb_interrupt_t;

struct qdr_arb_debug_stats_s	/* 833 */
{

	uint32 rsv_0	:4;
	uint32 to_policing_rd_error_cnt	:4;
	uint32 rsv_1	:4;
	uint32 to_policing_rd_ack_cnt	:4;
	uint32 rsv_2	:12;
	uint32 fr_policing_rd_valid_cnt	:4;

	uint32 rsv_3	:12;
	uint32 to_policing_wr_comp_cnt	:4;
	uint32 rsv_4	:12;
	uint32 fr_policing_wr_valid_cnt	:4;

	uint32 rsv_5	:4;
	uint32 to_stats_rd_error_cnt	:4;
	uint32 rsv_6	:4;
	uint32 to_stats_rd_ack_cnt	:4;
	uint32 rsv_7	:12;
	uint32 fr_stats_rd_valid_cnt	:4;

	uint32 rsv_8	:12;
	uint32 to_stats_wr_comp_cnt	:4;
	uint32 rsv_9	:12;
	uint32 fr_stats_wr_valid_cnt	:4;
};
typedef struct qdr_arb_debug_stats_s qdr_arb_debug_stats_t;

struct qdr_ctl_cfg_s	/* 841 */
{

	uint32 rsv_0	:2;
	uint32 cfg_inv_data_valid_bar	:1;
	uint32 rsv_1	:17;
	uint32 cfg_data_pop_latency	:4;
	uint32 rsv_2	:2;
	uint32 cfg_read_valid_latency	:2;
	uint32 cfg_parity_check_en	:1;
	uint32 cfg_auto_parity	:1;
	uint32 cfg_clock	:1;
	uint32 cfg_clock_en	:1;
};
typedef struct qdr_ctl_cfg_s qdr_ctl_cfg_t;

struct qdr_parity_error_count_s	/* 842 */
{

	uint32 rsv_0	:24;
	uint32 cfg_parity_error_count	:8;
};
typedef struct qdr_parity_error_count_s qdr_parity_error_count_t;

struct qdr_bist_control_s	/* 843 */
{

	uint32 cfg_bist_mismatch_count	:16;
	uint32 cfg_capture_en	:1;
	uint32 cfg_capture_once	:1;
	uint32 cfg_bist_en	:1;
	uint32 cfg_bist_once	:1;
	uint32 cfg_bist_read_expect_latency	:4;
	uint32 cfg_stop_on_error	:1;
	uint32 rsv_0	:1;
	uint32 cfg_bist_entries	:6;
};
typedef struct qdr_bist_control_s qdr_bist_control_t;

struct qdr_bist_pointers_s	/* 844 */
{

	uint32 rsv_0	:10;
	uint32 cfg_bist_expect_read_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_bist_request_done_once	:1;
	uint32 cfg_bist_request_read_ptr	:6;
	uint32 rsv_2	:1;
	uint32 cfg_bist_result_done_once	:1;
	uint32 cfg_bist_result_write_ptr	:6;
};
typedef struct qdr_bist_pointers_s qdr_bist_pointers_t;

struct qdr_capture_result_s	/* 845 */
{

	uint32 rsv_0	:9;
	uint32 cfg_capture_request_done_once	:1;
	uint32 cfg_capture_request_write_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_capture_result_done_once	:1;
	uint32 cfg_capture_result_write_ptr	:6;
	uint32 rsv_2	:8;
};
typedef struct qdr_capture_result_s qdr_capture_result_t;

struct qdr_adr_match_mask_s	/* 846 */
{

	uint32 rsv_0	:12;
	uint32 cfg_address_match_mask	:20;
};
typedef struct qdr_adr_match_mask_s qdr_adr_match_mask_t;

struct qdr_adr_match_value_s	/* 847 */
{

	uint32 rsv_0	:12;
	uint32 cfg_address_match_value	:20;
};
typedef struct qdr_adr_match_value_s qdr_adr_match_value_t;

struct qdr_init_ctl_s	/* 848 */
{

	uint32 rsv_0	:12;
	uint32 init_start_index	:20;

	uint32 rsv_1	:3;
	uint32 init_done	:1;
	uint32 rsv_2	:3;
	uint32 init_en	:1;
	uint32 rsv_3	:4;
	uint32 init_end_index	:20;
};
typedef struct qdr_init_ctl_s qdr_init_ctl_t;

struct qdr_ctl_req_fifo_threshold_s	/* 849 */
{

	uint32 rsv_0	:19;
	uint32 read_fifo_threshold	:5;
	uint32 rsv_1	:3;
	uint32 write_fifo_threshold	:5;
};
typedef struct qdr_ctl_req_fifo_threshold_s qdr_ctl_req_fifo_threshold_t;

struct qdr_ctl_interrupt_s	/* 850 */
{

	uint32 rsv_0	:27;
	uint32 value_set	:5;

	uint32 rsv_1	:27;
	uint32 value_reset	:5;

	uint32 rsv_2	:27;
	uint32 mask_set	:5;

	uint32 rsv_3	:27;
	uint32 mask_reset	:5;
};
typedef struct qdr_ctl_interrupt_s qdr_ctl_interrupt_t;

struct qdr_ctl_parity_fail_record_s	/* 851 */
{

	uint32 rsv_0	:7;
	uint32 qdr_parity_fail	:1;
	uint32 rsv_1	:4;
	uint32 qdr_parity_fail_addr	:20;
};
typedef struct qdr_ctl_parity_fail_record_s qdr_ctl_parity_fail_record_t;

struct q_mgr_enq_interrupt_s	/* 852 */
{

	uint32 rsv_0	:6;
	uint32 value_set	:26;

	uint32 rsv_1	:6;
	uint32 value_reset	:26;

	uint32 rsv_2	:6;
	uint32 mask_set	:26;

	uint32 rsv_3	:6;
	uint32 mask_reset	:26;
};
typedef struct q_mgr_enq_interrupt_s q_mgr_enq_interrupt_t;

struct q_mgr_enq_ctl_s	/* 853 */
{

	uint32 rsv_0	:31;
	uint32 base_on_buf_cnt	:1;
};
typedef struct q_mgr_enq_ctl_s q_mgr_enq_ctl_t;

struct q_mgr_enq_init_s	/* 854 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_enq_init	:1;
};
typedef struct q_mgr_enq_init_s q_mgr_enq_init_t;

struct q_mgr_enq_init_done_s	/* 855 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_enq_init	:1;
};
typedef struct q_mgr_enq_init_done_s q_mgr_enq_init_done_t;

struct q_mgr_ethernet_ipg_s	/* 856 */
{

	uint32 rsv_0	:1;
	uint32 ipg0	:7;
	uint32 rsv_1	:1;
	uint32 ipg1	:7;
	uint32 rsv_2	:1;
	uint32 ipg2	:7;
	uint32 rsv_3	:1;
	uint32 ipg3	:7;
};
typedef struct q_mgr_ethernet_ipg_s q_mgr_ethernet_ipg_t;

struct q_mgr_rand_seed_load_s	/* 857 */
{

	uint32 rand_seed_load	:1;
	uint32 rsv_0	:1;
	uint32 rand_seed_value	:30;
};
typedef struct q_mgr_rand_seed_load_s q_mgr_rand_seed_load_t;

struct q_mgr_enq_queue_id_max_num_s	/* 858 */
{

	uint32 rsv_0	:21;
	uint32 queue_id_max_num	:11;
};
typedef struct q_mgr_enq_queue_id_max_num_s q_mgr_enq_queue_id_max_num_t;

struct q_mgr_enq_que_num_fifo_credit_s	/* 859 */
{

	uint32 rsv_0	:28;
	uint32 enq_que_num_fifo_credit	:4;
};
typedef struct q_mgr_enq_que_num_fifo_credit_s q_mgr_enq_que_num_fifo_credit_t;

struct qmgrenq_q_mgr_enq_rcd_upd_credit_s	/* 860 */
{

	uint32 rsv_0	:25;
	uint32 rcd_upd_credit	:7;
};
typedef struct qmgrenq_q_mgr_enq_rcd_upd_credit_s qmgrenq_q_mgr_enq_rcd_upd_credit_t;

struct q_mgr_enq_table_ram_credit_s	/* 861 */
{

	uint32 rsv_0	:27;
	uint32 table_ram_credit	:5;
};
typedef struct q_mgr_enq_table_ram_credit_s q_mgr_enq_table_ram_credit_t;

struct q_mgr_enq_parity_enable_s	/* 862 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct q_mgr_enq_parity_enable_s q_mgr_enq_parity_enable_t;

struct q_mgr_enq_drain_enable_s	/* 863 */
{

	uint32 rsv_0	:31;
	uint32 drain_enable	:1;
};
typedef struct q_mgr_enq_drain_enable_s q_mgr_enq_drain_enable_t;

struct q_mgr_que_drop_stats_base_s	/* 864 */
{

	uint32 rsv_0	:15;
	uint32 stats_en	:1;
	uint32 rsv_1	:4;
	uint32 stats_base	:12;
};
typedef struct q_mgr_que_drop_stats_base_s q_mgr_que_drop_stats_base_t;

struct q_mgr_enq_rand_seed_load_force_drop_s	/* 865 */
{

	uint32 rand_seed_load	:1;
	uint32 rsv_0	:1;
	uint32 rand_seed_value	:30;
};
typedef struct q_mgr_enq_rand_seed_load_force_drop_s q_mgr_enq_rand_seed_load_force_drop_t;

struct q_mgr_length_adjust_s	/* 866 */
{

	uint32 rsv_0	:2;
	uint32 adjust_length1	:14;
	uint32 rsv_1	:2;
	uint32 adjust_length0	:14;
};
typedef struct q_mgr_length_adjust_s q_mgr_length_adjust_t;

struct q_mgr_enq_critical_pkt_ctl_s	/* 867 */
{

	uint32 rsv_0	:30;
	uint32 critical_pkt_force_no_drop	:1;
	uint32 critical_pkt_sel_thrd	:1;
};
typedef struct q_mgr_enq_critical_pkt_ctl_s q_mgr_enq_critical_pkt_ctl_t;

struct q_mgr_egress_resrc_mgr_s	/* 868 */
{

	uint32 rsv_0	:7;
	uint32 egress_resrc_mgr_en	:1;
	uint32 rsv_1	:8;
	uint32 shared_que_entry_thrd	:16;
};
typedef struct q_mgr_egress_resrc_mgr_s q_mgr_egress_resrc_mgr_t;

struct q_mgr_shared_que_entry_cnt_s	/* 869 */
{

	uint32 rsv_0	:16;
	uint32 shared_que_entry_cnt	:16;
};
typedef struct q_mgr_shared_que_entry_cnt_s q_mgr_shared_que_entry_cnt_t;

struct q_mgr_reserved_channel_range_s	/* 870 */
{

	uint32 reserved_channel_valid0	:1;
	uint32 rsv_0	:15;
	uint32 reserved_channel_max0	:8;
	uint32 reserved_channel_min0	:8;

	uint32 reserved_channel_valid1	:1;
	uint32 rsv_1	:15;
	uint32 reserved_channel_max1	:8;
	uint32 reserved_channel_min1	:8;
};
typedef struct q_mgr_reserved_channel_range_s q_mgr_reserved_channel_range_t;

struct q_mgr_qwrite_ctl_s	/* 871 */
{

	uint32 random_drop_thrd	:16;
	uint32 rsv_0	:3;
	uint32 bay_id	:5;
	uint32 service_id_en	:1;
	uint32 gen_que_id_rx_ether_oam	:1;
	uint32 flow_id_en	:1;
	uint32 mask_src_que_sel_from_fabric	:1;
	uint32 rsv_1	:2;
	uint32 que_sel_type_bits	:2;

	uint32 rx_ether_oam_queue_base	:16;
	uint32 rsv_2	:3;
	uint32 header_hash_bits_num	:3;
	uint32 rx_ether_oam_queue_select_shift	:4;
	uint32 rx_ether_oam_queue_select_mask	:6;
};
typedef struct q_mgr_qwrite_ctl_s q_mgr_qwrite_ctl_t;

struct q_mgr_queue_id_mon_s	/* 872 */
{

	uint32 rsv_0	:21;
	uint32 mon_que_id0	:11;

	uint32 rsv_1	:21;
	uint32 mon_que_id1	:11;

	uint32 rsv_2	:21;
	uint32 mon_que_id2	:11;

	uint32 rsv_3	:21;
	uint32 mon_que_id3	:11;
};
typedef struct q_mgr_queue_id_mon_s q_mgr_queue_id_mon_t;

struct q_mgr_enq_debug_stats_s	/* 873 */
{

	uint32 rsv_0	:12;
	uint32 output_met_fifo_done_cnt	:4;
	uint32 rsv_1	:12;
	uint32 input_met_fifo_enq_cnt	:4;

	uint32 output_rcd_wr_buf_cnt	:8;
	uint32 rsv_2	:4;
	uint32 input_rcd_wr_done_cnt	:4;
	uint32 rsv_3	:12;
	uint32 output_rcd_wr_cnt	:4;

	uint32 output_enq_link_pkt_cnt	:16;
	uint32 rsv_4	:12;
	uint32 output_enq_link_queue_cnt	:4;

	uint32 rsv_5	:12;
	uint32 input_table_wr_done_cnt	:4;
	uint32 rsv_6	:12;
	uint32 output_table_wr_cnt	:4;

	uint32 input_sch_deq_buf_cnt	:8;
	uint32 rsv_7	:4;
	uint32 input_sch_deq_queue_cnt	:4;
	uint32 rsv_8	:12;
	uint32 output_enq_sch_queue_cnt	:4;

	uint32 rsv_9	:4;
	uint32 drop4_enq_discard_cnt	:4;
	uint32 rsv_10	:4;
	uint32 drop4_force_random_drop_cnt	:4;
	uint32 rsv_11	:4;
	uint32 drop4_free_used_up_cnt	:4;
	uint32 rsv_12	:4;
	uint32 drop4_wred_tail_cnt	:4;

	uint32 rsv_13	:4;
	uint32 drop4_que_map_discard_cnt	:4;
	uint32 rsv_14	:4;
	uint32 drop4_egress_resrc_mgr_cnt	:4;
	uint32 rsv_15	:4;
	uint32 drop4_disabled_channel_id_cnt	:4;
	uint32 rsv_16	:4;
	uint32 drop_critical_pkt_cnt	:4;
};
typedef struct q_mgr_enq_debug_stats_s q_mgr_enq_debug_stats_t;

struct q_mgr_enqueue_stats_s	/* 874 */
{

	uint32 rsv_0	:8;
	uint32 admit_enq_msg_cnt	:24;

	uint32 rsv_1	:8;
	uint32 drop_enq_msg_cnt	:24;
};
typedef struct q_mgr_enqueue_stats_s q_mgr_enqueue_stats_t;

struct q_mgr_enq_parity_fail_record_s	/* 875 */
{

	uint32 rsv_0	:15;
	uint32 ds_egress_resrc_thrd_parity_fail	:1;
	uint32 rsv_1	:9;
	uint32 ds_egress_resrc_thrd_parity_fail_addr	:7;

	uint32 rsv_2	:15;
	uint32 ds_head_hash_mod_parity_fail	:1;
	uint32 rsv_3	:8;
	uint32 ds_head_hash_mod_parity_fail_addr	:8;

	uint32 rsv_4	:15;
	uint32 ds_link_aggr_num_parity_fail	:1;
	uint32 rsv_5	:9;
	uint32 ds_link_aggr_num_parity_fail_addr	:7;

	uint32 rsv_6	:15;
	uint32 ds_link_aggr_parity_fail	:1;
	uint32 rsv_7	:5;
	uint32 ds_link_aggr_parity_fail_addr	:11;

	uint32 rsv_8	:15;
	uint32 ds_que_drop_prof_id_parity_fail	:1;
	uint32 rsv_9	:7;
	uint32 ds_que_drop_prof_id_parity_fail_addr	:9;

	uint32 rsv_10	:15;
	uint32 ds_que_drop_prof_parity_fail	:1;
	uint32 rsv_11	:8;
	uint32 ds_que_drop_prof_parity_fail_addr	:8;

	uint32 rsv_12	:15;
	uint32 ds_que_num_gen_ctl_parity_fail	:1;
	uint32 rsv_13	:8;
	uint32 ds_que_num_gen_ctl_parity_fail_addr	:8;

	uint32 rsv_14	:15;
	uint32 ds_queue_ipg_index_parity_fail	:1;
	uint32 rsv_15	:8;
	uint32 ds_queue_ipg_index_parity_fail_addr	:8;

	uint32 rsv_16	:15;
	uint32 ds_service_queue_hash_key_parity_fail	:1;
	uint32 rsv_17	:9;
	uint32 ds_service_queue_hash_key_parity_fail_addr	:7;

	uint32 rsv_18	:15;
	uint32 ds_service_queue_parity_fail	:1;
	uint32 rsv_19	:7;
	uint32 ds_service_queue_parity_fail_addr	:9;

	uint32 rsv_20	:15;
	uint32 ds_sgmac_map_parity_fail	:1;
	uint32 rsv_21	:8;
	uint32 ds_sgmac_map_parity_fail_addr	:8;

	uint32 rsv_22	:25;
	uint32 ds_egress_resrc_count_parity_fail_addr	:7;

	uint32 rsv_23	:21;
	uint32 ds_queue_depth_parity_fail_addr	:11;
};
typedef struct q_mgr_enq_parity_fail_record_s q_mgr_enq_parity_fail_record_t;

struct q_mgrq_hash_cam_ctl_s	/* 876 */
{

	uint32 rsv_0	:6;
	uint32 dest_id0	:10;
	uint32 service_id0	:16;

	uint32 rsv_1	:6;
	uint32 dest_id1	:10;
	uint32 service_id1	:16;

	uint32 rsv_2	:6;
	uint32 dest_id2	:10;
	uint32 service_id2	:16;

	uint32 rsv_3	:6;
	uint32 dest_id3	:10;
	uint32 service_id3	:16;

	uint32 rsv_4	:6;
	uint32 dest_id4	:10;
	uint32 service_id4	:16;

	uint32 rsv_5	:6;
	uint32 dest_id5	:10;
	uint32 service_id5	:16;

	uint32 rsv_6	:6;
	uint32 dest_id6	:10;
	uint32 service_id6	:16;

	uint32 rsv_7	:6;
	uint32 dest_id7	:10;
	uint32 service_id7	:16;

	uint32 rsv_8	:6;
	uint32 dest_id8	:10;
	uint32 service_id8	:16;

	uint32 rsv_9	:6;
	uint32 dest_id9	:10;
	uint32 service_id9	:16;

	uint32 rsv_10	:6;
	uint32 dest_id10	:10;
	uint32 service_id10	:16;

	uint32 rsv_11	:6;
	uint32 dest_id11	:10;
	uint32 service_id11	:16;

	uint32 rsv_12	:6;
	uint32 dest_id12	:10;
	uint32 service_id12	:16;

	uint32 rsv_13	:6;
	uint32 dest_id13	:10;
	uint32 service_id13	:16;

	uint32 rsv_14	:6;
	uint32 dest_id14	:10;
	uint32 service_id14	:16;

	uint32 rsv_15	:6;
	uint32 dest_id15	:10;
	uint32 service_id15	:16;
};
typedef struct q_mgrq_hash_cam_ctl_s q_mgrq_hash_cam_ctl_t;

struct q_mgrq_write_sgmac_ctl_s	/* 877 */
{

	uint32 rsv_0	:16;
	uint32 sgmac3	:2;
	uint32 sgmac2	:2;
	uint32 sgmac1	:2;
	uint32 sgmac0	:2;
	uint32 rsv_1	:3;
	uint32 sgmac_mcast_map_en	:1;
	uint32 sgmac_en	:1;
	uint32 sgmac_trunk_en	:1;
	uint32 sgmac_trunk_num	:2;
};
typedef struct q_mgrq_write_sgmac_ctl_s q_mgrq_write_sgmac_ctl_t;

struct q_hash_lookup_result_ctl_s	/* 878 */
{

	uint32 rsv_0	:20;
	uint32 service_que_base_default	:12;
};
typedef struct q_hash_lookup_result_ctl_s q_hash_lookup_result_ctl_t;

struct q_mgr_hash_lookup_stats_s	/* 879 */
{

	uint32 rsv_0	:24;
	uint32 service_lookup_fail_cnt	:8;
};
typedef struct q_mgr_hash_lookup_stats_s q_mgr_hash_lookup_stats_t;

struct q_mgr_link_list_interrupt_s	/* 880 */
{

	uint32 rsv_0	:24;
	uint32 value_set_fatal	:8;

	uint32 rsv_1	:24;
	uint32 value_reset_fatal	:8;

	uint32 rsv_2	:24;
	uint32 mask_set_fatal	:8;

	uint32 rsv_3	:24;
	uint32 mask_reset_fatal	:8;
};
typedef struct q_mgr_link_list_interrupt_s q_mgr_link_list_interrupt_t;

struct q_mgr_free_list_status_s	/* 881 */
{

	uint32 rsv_0	:1;
	uint32 free_head_ptr	:15;
	uint32 rsv_1	:1;
	uint32 free_tail_ptr	:15;

	uint32 rsv_2	:16;
	uint32 free_list_cnt	:16;
};
typedef struct q_mgr_free_list_status_s q_mgr_free_list_status_t;

struct q_mgr_free_list_max_num_s	/* 882 */
{

	uint32 rsv_0	:17;
	uint32 free_list_max_num	:15;
};
typedef struct q_mgr_free_list_max_num_s q_mgr_free_list_max_num_t;

struct qmgrlinklist_q_mgr_queue_id_max_num_s	/* 883 */
{

	uint32 rsv_0	:21;
	uint32 queue_id_max_num	:11;
};
typedef struct qmgrlinklist_q_mgr_queue_id_max_num_s qmgrlinklist_q_mgr_queue_id_max_num_t;

struct q_mgr_init_s	/* 884 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_link_list_init	:1;
};
typedef struct q_mgr_init_s q_mgr_init_t;

struct q_mgr_init_done_s	/* 885 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_link_list_init	:1;
};
typedef struct q_mgr_init_done_s q_mgr_init_done_t;

struct q_mgr_free_list_fifo_credit_s	/* 886 */
{

	uint32 rsv_0	:28;
	uint32 free_list_fifo_credit	:4;
};
typedef struct q_mgr_free_list_fifo_credit_s q_mgr_free_list_fifo_credit_t;

struct q_mgr_link_list_parity_enable_s	/* 887 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct q_mgr_link_list_parity_enable_s q_mgr_link_list_parity_enable_t;

struct q_mgr_debug_stats_s	/* 888 */
{

	uint32 rsv_0	:28;
	uint32 output_free_list_valid_cnt	:4;

	uint32 rsv_1	:28;
	uint32 output_get_free_list_cnt	:4;

	uint32 rsv_2	:28;
	uint32 input_enq_pkt_cnt	:4;

	uint32 rsv_3	:28;
	uint32 input_enq_rep_cnt	:4;

	uint32 rsv_4	:28;
	uint32 input_deq_req_cnt	:4;

	uint32 rsv_5	:28;
	uint32 output_deq_pkt_cnt	:4;

	uint32 rsv_6	:28;
	uint32 output_deq_rep_cnt	:4;

	uint32 rsv_7	:28;
	uint32 input_rel_list_cnt	:4;

	uint32 rsv_8	:28;
	uint32 input_enq_free_list_done_cnt	:4;
};
typedef struct q_mgr_debug_stats_s q_mgr_debug_stats_t;

struct q_mgr_link_list_ecc_ctrl_s	/* 889 */
{

	uint32 rsv_0	:23;
	uint32 cfg_report_single_bit_error	:1;
	uint32 rsv_1	:3;
	uint32 cfg_ecc_correct_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_ecc_check_en	:1;
};
typedef struct q_mgr_link_list_ecc_ctrl_s q_mgr_link_list_ecc_ctrl_t;

struct q_mgr_link_list_ecc_error_stats_s	/* 890 */
{

	uint32 rsv_0	:16;
	uint32 cfg_ecc_single_bit_count	:8;
	uint32 cfg_ecc_multiple_bit_count	:8;
};
typedef struct q_mgr_link_list_ecc_error_stats_s q_mgr_link_list_ecc_error_stats_t;

struct q_mgr_link_list_parity_fail_record_s	/* 891 */
{

	uint32 rsv_0	:17;
	uint32 dsq_link_list_ecc_fail_addr	:15;

	uint32 rsv_1	:21;
	uint32 dsq_link_state_parity_fail_addr	:11;
};
typedef struct q_mgr_link_list_parity_fail_record_s q_mgr_link_list_parity_fail_record_t;

struct q_mgr_sch_interrupt_s	/* 892 */
{

	uint32 rsv_0	:8;
	uint32 value_set_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset_fatal	:24;
};
typedef struct q_mgr_sch_interrupt_s q_mgr_sch_interrupt_t;

struct q_mgr_sch_init_s	/* 893 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_sch_init	:1;
};
typedef struct q_mgr_sch_init_s q_mgr_sch_init_t;

struct q_mgr_sch_init_done_s	/* 894 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_sch_init	:1;
};
typedef struct q_mgr_sch_init_done_s q_mgr_sch_init_done_t;

struct qmgrsch_q_mgr_queue_id_max_num_s	/* 895 */
{

	uint32 rsv_0	:21;
	uint32 que_id_max_num	:11;
};
typedef struct qmgrsch_q_mgr_queue_id_max_num_s qmgrsch_q_mgr_queue_id_max_num_t;

struct q_mgr_sch_parity_enable_s	/* 896 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct q_mgr_sch_parity_enable_s q_mgr_sch_parity_enable_t;

struct q_mgr_que_deq_stats_base_s	/* 897 */
{

	uint32 rsv_0	:15;
	uint32 stats_en	:1;
	uint32 rsv_1	:4;
	uint32 q_mgr_que_deq_stats_base	:12;
};
typedef struct q_mgr_que_deq_stats_base_s q_mgr_que_deq_stats_base_t;

struct q_mgr_sch_sp_patch_en_s	/* 898 */
{

	uint32 rsv_0	:31;
	uint32 sch_sp_patch_en	:1;
};
typedef struct q_mgr_sch_sp_patch_en_s q_mgr_sch_sp_patch_en_t;

struct q_mgr_queue_shape_ctl_s	/* 899 */
{

	uint32 que_shp_max_ptr	:16;
	uint32 que_shp_min_ptr	:16;

	uint32 que_shp_lo_bw_max_ptr	:16;
	uint32 que_shp_hi_bw_min_ptr	:16;

	uint32 rsv_0	:16;
	uint32 que_shp_lo_bw_weight	:8;
	uint32 que_shp_hi_bw_weight	:8;

	uint32 que_shp_max_phy_ptr	:16;
	uint32 que_shp_upd_max_cnt	:8;
	uint32 rsv_1	:3;
	uint32 que_shp_upd_en	:1;
	uint32 rsv_2	:3;
	uint32 que_shp_gbl_en	:1;
};
typedef struct q_mgr_queue_shape_ctl_s q_mgr_queue_shape_ctl_t;

struct q_mgr_group_shape_ctl_s	/* 900 */
{

	uint32 grp_shp_max_ptr	:16;
	uint32 grp_shp_min_ptr	:16;

	uint32 grp_shp_lo_bw_max_ptr	:16;
	uint32 grp_shp_hi_bw_min_ptr	:16;

	uint32 rsv_0	:16;
	uint32 grp_shp_lo_bw_weight	:8;
	uint32 grp_shp_hi_bw_weight	:8;

	uint32 grp_shp_max_phy_ptr	:16;
	uint32 grp_shp_upd_max_cnt	:8;
	uint32 rsv_1	:3;
	uint32 grp_shp_upd_en	:1;
	uint32 rsv_2	:3;
	uint32 grp_shp_gbl_en	:1;
};
typedef struct q_mgr_group_shape_ctl_s q_mgr_group_shape_ctl_t;

struct q_mgr_sch_debug_stats_s	/* 901 */
{

	uint32 rsv_0	:28;
	uint32 input_enq_valid_cnt	:4;

	uint32 rsv_1	:28;
	uint32 inputq_read_done_cnt	:4;

	uint32 rsv_2	:28;
	uint32 input_link_list_valid_cnt	:4;

	uint32 rsv_3	:28;
	uint32 input_link_list_queue_cnt	:4;

	uint32 rsv_4	:28;
	uint32 output_sch2_enq_valid_cnt	:4;

	uint32 rsv_5	:28;
	uint32 output_sch2_sub_ch_valid_cnt	:4;

	uint32 rsv_6	:28;
	uint32 outputq_read_valid_cnt	:4;

	uint32 rsv_7	:28;
	uint32 output_rel_list_valid_cnt	:4;

	uint32 rsv_8	:28;
	uint32 output_get_ll_valid_cnt	:4;
};
typedef struct q_mgr_sch_debug_stats_s q_mgr_sch_debug_stats_t;

struct q_mgr_sch_parity_fail_record_s	/* 902 */
{

	uint32 rsv_0	:21;
	uint32 ds_queue_shape_parity_fail_addr	:11;

	uint32 rsv_1	:23;
	uint32 ds_group_shape_parity_fail_addr	:9;

	uint32 rsv_2	:21;
	uint32 ds_queue_drr_deficit_parity_fail_addr	:11;

	uint32 rsv_3	:21;
	uint32 ds_in_profile_next_queue_ptr_parity_fail_addr	:11;

	uint32 rsv_4	:21;
	uint32 ds_out_profile_next_queue_ptr_parity_fail_addr	:11;

	uint32 rsv_5	:21;
	uint32 ds_queue_map_parity_fail_addr	:11;

	uint32 rsv_6	:23;
	uint32 ds_channel_link_state_parity_fail_addr	:9;

	uint32 rsv_7	:25;
	uint32 ds_queue_state_parity_fail_addr	:7;

	uint32 rsv_8	:25;
	uint32 ds_queue_shape_state_parity_fail_addr	:7;

	uint32 rsv_9	:23;
	uint32 ds_group_context_parity_fail_addr	:9;

	uint32 rsv_10	:23;
	uint32 ds_group_cache_parity_fail_addr	:9;
};
typedef struct q_mgr_sch_parity_fail_record_s q_mgr_sch_parity_fail_record_t;

struct q_mgr_sub_ch_interrupt_s	/* 907 */
{

	uint32 rsv_0	:8;
	uint32 value_set_fatal	:24;

	uint32 rsv_1	:8;
	uint32 value_reset_fatal	:24;

	uint32 rsv_2	:8;
	uint32 mask_set_fatal	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset_fatal	:24;
};
typedef struct q_mgr_sub_ch_interrupt_s q_mgr_sub_ch_interrupt_t;

struct q_mgr_sub_ch_init_s	/* 908 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_sub_ch_init	:1;
};
typedef struct q_mgr_sub_ch_init_s q_mgr_sub_ch_init_t;

struct q_mgr_sub_ch_init_done_s	/* 909 */
{

	uint32 rsv_0	:31;
	uint32 q_mgr_sub_ch_init	:1;
};
typedef struct q_mgr_sub_ch_init_done_s q_mgr_sub_ch_init_done_t;

struct q_mgr_sub_ch_shape_ctl_s	/* 910 */
{

	uint32 shape_max_phy_ptr	:16;
	uint32 shape_upd_max_cnt	:8;
	uint32 rsv_0	:3;
	uint32 shape_upd_en	:1;
	uint32 rsv_1	:3;
	uint32 shape_gbl_en	:1;

	uint32 shape_max_ptr	:16;
	uint32 shape_min_ptr	:16;
};
typedef struct q_mgr_sub_ch_shape_ctl_s q_mgr_sub_ch_shape_ctl_t;

struct q_mgr_network_drain_en_cfg_s	/* 911 */
{

	uint32 network_en_cfg_lo	:32;

	uint32 rsv_0	:12;
	uint32 network_en_cfg_hi	:20;
};
typedef struct q_mgr_network_drain_en_cfg_s q_mgr_network_drain_en_cfg_t;

struct q_mgr_fabric_drain_en_cfg_s	/* 912 */
{

	uint32 fb_ch_en_cfg	:32;
};
typedef struct q_mgr_fabric_drain_en_cfg_s q_mgr_fabric_drain_en_cfg_t;

struct q_mgr_misc_drain_en_cfg_s	/* 913 */
{

	uint32 rsv_0	:28;
	uint32 e_loop_en_cfg	:1;
	uint32 oam_en_cfg	:1;
	uint32 cpu_en_cfg	:1;
	uint32 i_loop_en_cfg	:1;
};
typedef struct q_mgr_misc_drain_en_cfg_s q_mgr_misc_drain_en_cfg_t;

struct q_mgr_qdr_arb_credit_s	/* 914 */
{

	uint32 rsv_0	:22;
	uint32 qdr_arb_credit	:10;
};
typedef struct q_mgr_qdr_arb_credit_s q_mgr_qdr_arb_credit_t;

struct q_mgr_track_fifo_credit_s	/* 915 */
{

	uint32 rsv_0	:22;
	uint32 track_fifo_credit	:10;
};
typedef struct q_mgr_track_fifo_credit_s q_mgr_track_fifo_credit_t;

struct q_mgr_sub_ch_parity_enable_s	/* 916 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct q_mgr_sub_ch_parity_enable_s q_mgr_sub_ch_parity_enable_t;

struct q_mgr_interface_wrr_weight_cfg_s	/* 917 */
{

	uint32 rsv_0	:24;
	uint32 network_wt_cfg	:8;

	uint32 rsv_1	:24;
	uint32 fabric_wt_cfg	:8;

	uint32 rsv_2	:24;
	uint32 misc_wt_cfg	:8;

	uint32 rsv_3	:24;
	uint32 e_loop_wt_cfg	:8;
};
typedef struct q_mgr_interface_wrr_weight_cfg_s q_mgr_interface_wrr_weight_cfg_t;

struct q_mgr_misc_interface_wrr_weight_cfg_s	/* 918 */
{

	uint32 rsv_0	:24;
	uint32 i_loop_wt_cfg	:8;

	uint32 rsv_1	:24;
	uint32 cpu_wt_cfg	:8;

	uint32 rsv_2	:24;
	uint32 oam_wt_cfg	:8;
};
typedef struct q_mgr_misc_interface_wrr_weight_cfg_s q_mgr_misc_interface_wrr_weight_cfg_t;

struct q_mgri_loop_out_profile_wrr_weight_cfg_s	/* 919 */
{

	uint32 i_loop_pri0_outp_wt_cfg	:8;
	uint32 i_loop_pri1_outp_wt_cfg	:8;
	uint32 i_loop_pri2_outp_wt_cfg	:8;
	uint32 i_loop_pri3_outp_wt_cfg	:8;
};
typedef struct q_mgri_loop_out_profile_wrr_weight_cfg_s q_mgri_loop_out_profile_wrr_weight_cfg_t;

struct q_mgr_cpu_out_profile_wrr_weight_cfg_s	/* 920 */
{

	uint32 cpu_pri0_outp_wt_cfg	:8;
	uint32 cpu_pri1_outp_wt_cfg	:8;
	uint32 cpu_pri2_outp_wt_cfg	:8;
	uint32 cpu_pri3_outp_wt_cfg	:8;
};
typedef struct q_mgr_cpu_out_profile_wrr_weight_cfg_s q_mgr_cpu_out_profile_wrr_weight_cfg_t;

struct q_mgr_oam_out_profile_wrr_weight_cfg_s	/* 921 */
{

	uint32 oam_pri0_outp_wt_cfg	:8;
	uint32 oam_pri1_outp_wt_cfg	:8;
	uint32 oam_pri2_outp_wt_cfg	:8;
	uint32 oam_pri3_outp_wt_cfg	:8;
};
typedef struct q_mgr_oam_out_profile_wrr_weight_cfg_s q_mgr_oam_out_profile_wrr_weight_cfg_t;

struct q_mgre_loop_outp_wrr_weight_cfg_s	/* 922 */
{

	uint32 e_loop_pri0_outp_wt_cfg	:8;
	uint32 e_loop_pri1_outp_wt_cfg	:8;
	uint32 e_loop_pri2_outp_wt_cfg	:8;
	uint32 e_loop_pri3_outp_wt_cfg	:8;
};
typedef struct q_mgre_loop_outp_wrr_weight_cfg_s q_mgre_loop_outp_wrr_weight_cfg_t;

struct q_mgr_sub_ch_bw_mon_s	/* 923 */
{

	uint32 rsv_0	:24;
	uint32 ch_id_mon	:8;

	uint32 rsv_1	:22;
	uint32 min_ch_credit	:10;
};
typedef struct q_mgr_sub_ch_bw_mon_s q_mgr_sub_ch_bw_mon_t;

struct q_mgr_ch_shape_state_s	/* 924 */
{

	uint32 shp_state95to64	:32;

	uint32 shp_state63to32	:32;

	uint32 shp_state31to0	:32;
};
typedef struct q_mgr_ch_shape_state_s q_mgr_ch_shape_state_t;

struct q_mgr_sub_ch_debug_stats_s	/* 925 */
{

	uint32 rsv_0	:28;
	uint32 input_track_msg_cnt	:4;

	uint32 rsv_1	:28;
	uint32 input_qdr_arb_credit_inc_cnt	:4;

	uint32 rsv_2	:28;
	uint32 inputq_mgr_sch_valid_cnt	:4;

	uint32 rsv_3	:28;
	uint32 input_table_qdr_data_valid_cnt	:4;

	uint32 rsv_4	:28;
	uint32 input_buf_retrv_done_cnt	:4;

	uint32 rsv_5	:28;
	uint32 output_sub_ch_valid_cnt	:4;

	uint32 rsv_6	:28;
	uint32 outputq_mgr_deq_valid_cnt	:4;

	uint32 rsv_7	:24;
	uint32 output_buf_retrv_buf_cnt	:8;
};
typedef struct q_mgr_sub_ch_debug_stats_s q_mgr_sub_ch_debug_stats_t;

struct q_mgr_table_que_entry_interrupt_s	/* 926 */
{

	uint32 rsv_0	:27;
	uint32 value_set	:5;

	uint32 rsv_1	:27;
	uint32 value_reset	:5;

	uint32 rsv_2	:27;
	uint32 mask_set	:5;

	uint32 rsv_3	:27;
	uint32 mask_reset	:5;
};
typedef struct q_mgr_table_que_entry_interrupt_s q_mgr_table_que_entry_interrupt_t;

struct q_mgr_table_ecc_ctrl_s	/* 927 */
{

	uint32 rsv_0	:23;
	uint32 cfg_report_single_bit_error	:1;
	uint32 rsv_1	:3;
	uint32 cfg_ecc_correct_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_ecc_check_en	:1;
};
typedef struct q_mgr_table_ecc_ctrl_s q_mgr_table_ecc_ctrl_t;

struct q_mgr_table_parity_ctrl_s	/* 928 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct q_mgr_table_parity_ctrl_s q_mgr_table_parity_ctrl_t;

struct que_entry_read_fail_record_s	/* 929 */
{

	uint32 rsv_0	:15;
	uint32 que_entry_read_fail	:1;
	uint32 rsv_1	:1;
	uint32 que_entry_read_fail_addr	:15;
};
typedef struct que_entry_read_fail_record_s que_entry_read_fail_record_t;

struct q_mgr_table_que_entry_debug_stats_s	/* 930 */
{

	uint32 rsv_0	:28;
	uint32 que_entry_write_cnt	:4;

	uint32 rsv_1	:12;
	uint32 que_entry_read_err_cnt	:4;
	uint32 rsv_2	:12;
	uint32 que_entry_read_cnt	:4;
};
typedef struct q_mgr_table_que_entry_debug_stats_s q_mgr_table_que_entry_debug_stats_t;

struct q_mgr_table_ecc_error_stats_s	/* 931 */
{

	uint32 rsv_0	:16;
	uint32 cfg_ecc_single_bit_count	:8;
	uint32 cfg_ecc_multiple_bit_count	:8;
};
typedef struct q_mgr_table_ecc_error_stats_s q_mgr_table_ecc_error_stats_t;

struct quadmacapp0_quad_mac_app_interrupt_fatal_s	/* 932 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp0_quad_mac_app_interrupt_fatal_s quadmacapp0_quad_mac_app_interrupt_fatal_t;

struct quadmacapp0_quad_mac_app_packet_len_mtu1_s	/* 933 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp0_quad_mac_app_packet_len_mtu1_s quadmacapp0_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp0_quad_mac_app_packet_len_mtu2_s	/* 934 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp0_quad_mac_app_packet_len_mtu2_s quadmacapp0_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp0_quad_mac_app_dot1q_delta_bytes_s	/* 935 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp0_quad_mac_app_dot1q_delta_bytes_s quadmacapp0_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp0_quad_mac_app_init_s	/* 936 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp0_quad_mac_app_init_s quadmacapp0_quad_mac_app_init_t;

struct quadmacapp0_quad_mac_app_init_done_s	/* 937 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp0_quad_mac_app_init_done_s quadmacapp0_quad_mac_app_init_done_t;

struct quadmacapp0_quad_mac_app_stats_update_ctrl_s	/* 938 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp0_quad_mac_app_stats_update_ctrl_s quadmacapp0_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp0_quad_mac_app_parity_enable_s	/* 939 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp0_quad_mac_app_parity_enable_s quadmacapp0_quad_mac_app_parity_enable_t;

struct quadmacapp0_quad_mac_app_status_over_write_s	/* 940 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp0_quad_mac_app_status_over_write_s quadmacapp0_quad_mac_app_status_over_write_t;

struct quadmacapp0_quad_mac_app_status_over_write_old_snap_s	/* 941 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp0_quad_mac_app_status_over_write_old_snap_s quadmacapp0_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp0_quad_mac_app_status_over_write_new_snap_s	/* 942 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp0_quad_mac_app_status_over_write_new_snap_s quadmacapp0_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp0_quad_mac_app_max_init_cnt_s	/* 943 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp0_quad_mac_app_max_init_cnt_s quadmacapp0_quad_mac_app_max_init_cnt_t;

struct quadmacapp0_quad_mac_app_pause_frame_ctl_s	/* 944 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp0_quad_mac_app_pause_frame_ctl_s quadmacapp0_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp0_quad_mac_app_pkt_err_mask_out_s	/* 945 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp0_quad_mac_app_pkt_err_mask_out_s quadmacapp0_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp0_quad_mac_app_pkt_err_inv_s	/* 946 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp0_quad_mac_app_pkt_err_inv_s quadmacapp0_quad_mac_app_pkt_err_inv_t;

struct quadmacapp0_quad_mac_app_crc_strip_s	/* 947 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp0_quad_mac_app_crc_strip_s quadmacapp0_quad_mac_app_crc_strip_t;

struct quadmacapp0_quad_mac_app_tp_id_s	/* 948 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp0_quad_mac_app_tp_id_s quadmacapp0_quad_mac_app_tp_id_t;

struct quadmacapp0_quad_mac_app_buf_store_stall_mask_s	/* 949 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp0_quad_mac_app_buf_store_stall_mask_s quadmacapp0_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp0_quad_mac_app_keep_bay_hdr_s	/* 950 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp0_quad_mac_app_keep_bay_hdr_s quadmacapp0_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp0_quad_mac_app_stall_record_s	/* 951 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp0_quad_mac_app_stall_record_s quadmacapp0_quad_mac_app_stall_record_t;

struct quadmacapp0_quad_mac_app_pause_timer_out_s	/* 952 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp0_quad_mac_app_pause_timer_out_s quadmacapp0_quad_mac_app_pause_timer_out_t;

struct quadmacapp0_quad_mac_app_vlan_ctrl_s	/* 953 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp0_quad_mac_app_vlan_ctrl_s quadmacapp0_quad_mac_app_vlan_ctrl_t;

struct quadmacapp0_quad_mac_app_cur_tx_state_machine_s	/* 954 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp0_quad_mac_app_cur_tx_state_machine_s quadmacapp0_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp0_quad_mac_app_stat_sel_s	/* 955 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp0_quad_mac_app_stat_sel_s quadmacapp0_quad_mac_app_stat_sel_t;

struct quadmacapp0_quad_mac_app_debug_stats_s	/* 956 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp0_quad_mac_app_debug_stats_s quadmacapp0_quad_mac_app_debug_stats_t;

struct quadmacapp10_quad_mac_app_interrupt_fatal_s	/* 957 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp10_quad_mac_app_interrupt_fatal_s quadmacapp10_quad_mac_app_interrupt_fatal_t;

struct quadmacapp10_quad_mac_app_packet_len_mtu1_s	/* 958 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp10_quad_mac_app_packet_len_mtu1_s quadmacapp10_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp10_quad_mac_app_packet_len_mtu2_s	/* 959 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp10_quad_mac_app_packet_len_mtu2_s quadmacapp10_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp10_quad_mac_app_dot1q_delta_bytes_s	/* 960 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp10_quad_mac_app_dot1q_delta_bytes_s quadmacapp10_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp10_quad_mac_app_init_s	/* 961 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp10_quad_mac_app_init_s quadmacapp10_quad_mac_app_init_t;

struct quadmacapp10_quad_mac_app_init_done_s	/* 962 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp10_quad_mac_app_init_done_s quadmacapp10_quad_mac_app_init_done_t;

struct quadmacapp10_quad_mac_app_stats_update_ctrl_s	/* 963 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp10_quad_mac_app_stats_update_ctrl_s quadmacapp10_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp10_quad_mac_app_parity_enable_s	/* 964 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp10_quad_mac_app_parity_enable_s quadmacapp10_quad_mac_app_parity_enable_t;

struct quadmacapp10_quad_mac_app_status_over_write_s	/* 965 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp10_quad_mac_app_status_over_write_s quadmacapp10_quad_mac_app_status_over_write_t;

struct quadmacapp10_quad_mac_app_status_over_write_old_snap_s	/* 966 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp10_quad_mac_app_status_over_write_old_snap_s quadmacapp10_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp10_quad_mac_app_status_over_write_new_snap_s	/* 967 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp10_quad_mac_app_status_over_write_new_snap_s quadmacapp10_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp10_quad_mac_app_max_init_cnt_s	/* 968 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp10_quad_mac_app_max_init_cnt_s quadmacapp10_quad_mac_app_max_init_cnt_t;

struct quadmacapp10_quad_mac_app_pause_frame_ctl_s	/* 969 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp10_quad_mac_app_pause_frame_ctl_s quadmacapp10_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp10_quad_mac_app_pkt_err_mask_out_s	/* 970 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp10_quad_mac_app_pkt_err_mask_out_s quadmacapp10_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp10_quad_mac_app_pkt_err_inv_s	/* 971 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp10_quad_mac_app_pkt_err_inv_s quadmacapp10_quad_mac_app_pkt_err_inv_t;

struct quadmacapp10_quad_mac_app_crc_strip_s	/* 972 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp10_quad_mac_app_crc_strip_s quadmacapp10_quad_mac_app_crc_strip_t;

struct quadmacapp10_quad_mac_app_tp_id_s	/* 973 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp10_quad_mac_app_tp_id_s quadmacapp10_quad_mac_app_tp_id_t;

struct quadmacapp10_quad_mac_app_buf_store_stall_mask_s	/* 974 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp10_quad_mac_app_buf_store_stall_mask_s quadmacapp10_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp10_quad_mac_app_keep_bay_hdr_s	/* 975 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp10_quad_mac_app_keep_bay_hdr_s quadmacapp10_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp10_quad_mac_app_stall_record_s	/* 976 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp10_quad_mac_app_stall_record_s quadmacapp10_quad_mac_app_stall_record_t;

struct quadmacapp10_quad_mac_app_pause_timer_out_s	/* 977 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp10_quad_mac_app_pause_timer_out_s quadmacapp10_quad_mac_app_pause_timer_out_t;

struct quadmacapp10_quad_mac_app_vlan_ctrl_s	/* 978 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp10_quad_mac_app_vlan_ctrl_s quadmacapp10_quad_mac_app_vlan_ctrl_t;

struct quadmacapp10_quad_mac_app_cur_tx_state_machine_s	/* 979 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp10_quad_mac_app_cur_tx_state_machine_s quadmacapp10_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp10_quad_mac_app_stat_sel_s	/* 980 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp10_quad_mac_app_stat_sel_s quadmacapp10_quad_mac_app_stat_sel_t;

struct quadmacapp10_quad_mac_app_debug_stats_s	/* 981 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp10_quad_mac_app_debug_stats_s quadmacapp10_quad_mac_app_debug_stats_t;

struct quadmacapp11_quad_mac_app_interrupt_fatal_s	/* 982 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp11_quad_mac_app_interrupt_fatal_s quadmacapp11_quad_mac_app_interrupt_fatal_t;

struct quadmacapp11_quad_mac_app_packet_len_mtu1_s	/* 983 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp11_quad_mac_app_packet_len_mtu1_s quadmacapp11_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp11_quad_mac_app_packet_len_mtu2_s	/* 984 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp11_quad_mac_app_packet_len_mtu2_s quadmacapp11_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp11_quad_mac_app_dot1q_delta_bytes_s	/* 985 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp11_quad_mac_app_dot1q_delta_bytes_s quadmacapp11_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp11_quad_mac_app_init_s	/* 986 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp11_quad_mac_app_init_s quadmacapp11_quad_mac_app_init_t;

struct quadmacapp11_quad_mac_app_init_done_s	/* 987 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp11_quad_mac_app_init_done_s quadmacapp11_quad_mac_app_init_done_t;

struct quadmacapp11_quad_mac_app_stats_update_ctrl_s	/* 988 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp11_quad_mac_app_stats_update_ctrl_s quadmacapp11_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp11_quad_mac_app_parity_enable_s	/* 989 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp11_quad_mac_app_parity_enable_s quadmacapp11_quad_mac_app_parity_enable_t;

struct quadmacapp11_quad_mac_app_status_over_write_s	/* 990 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp11_quad_mac_app_status_over_write_s quadmacapp11_quad_mac_app_status_over_write_t;

struct quadmacapp11_quad_mac_app_status_over_write_old_snap_s	/* 991 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp11_quad_mac_app_status_over_write_old_snap_s quadmacapp11_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp11_quad_mac_app_status_over_write_new_snap_s	/* 992 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp11_quad_mac_app_status_over_write_new_snap_s quadmacapp11_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp11_quad_mac_app_max_init_cnt_s	/* 993 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp11_quad_mac_app_max_init_cnt_s quadmacapp11_quad_mac_app_max_init_cnt_t;

struct quadmacapp11_quad_mac_app_pause_frame_ctl_s	/* 994 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp11_quad_mac_app_pause_frame_ctl_s quadmacapp11_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp11_quad_mac_app_pkt_err_mask_out_s	/* 995 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp11_quad_mac_app_pkt_err_mask_out_s quadmacapp11_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp11_quad_mac_app_pkt_err_inv_s	/* 996 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp11_quad_mac_app_pkt_err_inv_s quadmacapp11_quad_mac_app_pkt_err_inv_t;

struct quadmacapp11_quad_mac_app_crc_strip_s	/* 997 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp11_quad_mac_app_crc_strip_s quadmacapp11_quad_mac_app_crc_strip_t;

struct quadmacapp11_quad_mac_app_tp_id_s	/* 998 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp11_quad_mac_app_tp_id_s quadmacapp11_quad_mac_app_tp_id_t;

struct quadmacapp11_quad_mac_app_buf_store_stall_mask_s	/* 999 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp11_quad_mac_app_buf_store_stall_mask_s quadmacapp11_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp11_quad_mac_app_keep_bay_hdr_s	/* 1000 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp11_quad_mac_app_keep_bay_hdr_s quadmacapp11_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp11_quad_mac_app_stall_record_s	/* 1001 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp11_quad_mac_app_stall_record_s quadmacapp11_quad_mac_app_stall_record_t;

struct quadmacapp11_quad_mac_app_pause_timer_out_s	/* 1002 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp11_quad_mac_app_pause_timer_out_s quadmacapp11_quad_mac_app_pause_timer_out_t;

struct quadmacapp11_quad_mac_app_vlan_ctrl_s	/* 1003 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp11_quad_mac_app_vlan_ctrl_s quadmacapp11_quad_mac_app_vlan_ctrl_t;

struct quadmacapp11_quad_mac_app_cur_tx_state_machine_s	/* 1004 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp11_quad_mac_app_cur_tx_state_machine_s quadmacapp11_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp11_quad_mac_app_stat_sel_s	/* 1005 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp11_quad_mac_app_stat_sel_s quadmacapp11_quad_mac_app_stat_sel_t;

struct quadmacapp11_quad_mac_app_debug_stats_s	/* 1006 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp11_quad_mac_app_debug_stats_s quadmacapp11_quad_mac_app_debug_stats_t;

struct quadmacapp1_quad_mac_app_interrupt_fatal_s	/* 1007 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp1_quad_mac_app_interrupt_fatal_s quadmacapp1_quad_mac_app_interrupt_fatal_t;

struct quadmacapp1_quad_mac_app_packet_len_mtu1_s	/* 1008 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp1_quad_mac_app_packet_len_mtu1_s quadmacapp1_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp1_quad_mac_app_packet_len_mtu2_s	/* 1009 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp1_quad_mac_app_packet_len_mtu2_s quadmacapp1_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp1_quad_mac_app_dot1q_delta_bytes_s	/* 1010 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp1_quad_mac_app_dot1q_delta_bytes_s quadmacapp1_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp1_quad_mac_app_init_s	/* 1011 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp1_quad_mac_app_init_s quadmacapp1_quad_mac_app_init_t;

struct quadmacapp1_quad_mac_app_init_done_s	/* 1012 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp1_quad_mac_app_init_done_s quadmacapp1_quad_mac_app_init_done_t;

struct quadmacapp1_quad_mac_app_stats_update_ctrl_s	/* 1013 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp1_quad_mac_app_stats_update_ctrl_s quadmacapp1_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp1_quad_mac_app_parity_enable_s	/* 1014 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp1_quad_mac_app_parity_enable_s quadmacapp1_quad_mac_app_parity_enable_t;

struct quadmacapp1_quad_mac_app_status_over_write_s	/* 1015 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp1_quad_mac_app_status_over_write_s quadmacapp1_quad_mac_app_status_over_write_t;

struct quadmacapp1_quad_mac_app_status_over_write_old_snap_s	/* 1016 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp1_quad_mac_app_status_over_write_old_snap_s quadmacapp1_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp1_quad_mac_app_status_over_write_new_snap_s	/* 1017 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp1_quad_mac_app_status_over_write_new_snap_s quadmacapp1_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp1_quad_mac_app_max_init_cnt_s	/* 1018 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp1_quad_mac_app_max_init_cnt_s quadmacapp1_quad_mac_app_max_init_cnt_t;

struct quadmacapp1_quad_mac_app_pause_frame_ctl_s	/* 1019 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp1_quad_mac_app_pause_frame_ctl_s quadmacapp1_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp1_quad_mac_app_pkt_err_mask_out_s	/* 1020 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp1_quad_mac_app_pkt_err_mask_out_s quadmacapp1_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp1_quad_mac_app_pkt_err_inv_s	/* 1021 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp1_quad_mac_app_pkt_err_inv_s quadmacapp1_quad_mac_app_pkt_err_inv_t;

struct quadmacapp1_quad_mac_app_crc_strip_s	/* 1022 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp1_quad_mac_app_crc_strip_s quadmacapp1_quad_mac_app_crc_strip_t;

struct quadmacapp1_quad_mac_app_tp_id_s	/* 1023 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp1_quad_mac_app_tp_id_s quadmacapp1_quad_mac_app_tp_id_t;

struct quadmacapp1_quad_mac_app_buf_store_stall_mask_s	/* 1024 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp1_quad_mac_app_buf_store_stall_mask_s quadmacapp1_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp1_quad_mac_app_keep_bay_hdr_s	/* 1025 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp1_quad_mac_app_keep_bay_hdr_s quadmacapp1_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp1_quad_mac_app_stall_record_s	/* 1026 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp1_quad_mac_app_stall_record_s quadmacapp1_quad_mac_app_stall_record_t;

struct quadmacapp1_quad_mac_app_pause_timer_out_s	/* 1027 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp1_quad_mac_app_pause_timer_out_s quadmacapp1_quad_mac_app_pause_timer_out_t;

struct quadmacapp1_quad_mac_app_vlan_ctrl_s	/* 1028 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp1_quad_mac_app_vlan_ctrl_s quadmacapp1_quad_mac_app_vlan_ctrl_t;

struct quadmacapp1_quad_mac_app_cur_tx_state_machine_s	/* 1029 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp1_quad_mac_app_cur_tx_state_machine_s quadmacapp1_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp1_quad_mac_app_stat_sel_s	/* 1030 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp1_quad_mac_app_stat_sel_s quadmacapp1_quad_mac_app_stat_sel_t;

struct quadmacapp1_quad_mac_app_debug_stats_s	/* 1031 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp1_quad_mac_app_debug_stats_s quadmacapp1_quad_mac_app_debug_stats_t;

struct quadmacapp2_quad_mac_app_interrupt_fatal_s	/* 1032 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp2_quad_mac_app_interrupt_fatal_s quadmacapp2_quad_mac_app_interrupt_fatal_t;

struct quadmacapp2_quad_mac_app_packet_len_mtu1_s	/* 1033 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp2_quad_mac_app_packet_len_mtu1_s quadmacapp2_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp2_quad_mac_app_packet_len_mtu2_s	/* 1034 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp2_quad_mac_app_packet_len_mtu2_s quadmacapp2_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp2_quad_mac_app_dot1q_delta_bytes_s	/* 1035 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp2_quad_mac_app_dot1q_delta_bytes_s quadmacapp2_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp2_quad_mac_app_init_s	/* 1036 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp2_quad_mac_app_init_s quadmacapp2_quad_mac_app_init_t;

struct quadmacapp2_quad_mac_app_init_done_s	/* 1037 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp2_quad_mac_app_init_done_s quadmacapp2_quad_mac_app_init_done_t;

struct quadmacapp2_quad_mac_app_stats_update_ctrl_s	/* 1038 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp2_quad_mac_app_stats_update_ctrl_s quadmacapp2_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp2_quad_mac_app_parity_enable_s	/* 1039 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp2_quad_mac_app_parity_enable_s quadmacapp2_quad_mac_app_parity_enable_t;

struct quadmacapp2_quad_mac_app_status_over_write_s	/* 1040 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp2_quad_mac_app_status_over_write_s quadmacapp2_quad_mac_app_status_over_write_t;

struct quadmacapp2_quad_mac_app_status_over_write_old_snap_s	/* 1041 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp2_quad_mac_app_status_over_write_old_snap_s quadmacapp2_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp2_quad_mac_app_status_over_write_new_snap_s	/* 1042 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp2_quad_mac_app_status_over_write_new_snap_s quadmacapp2_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp2_quad_mac_app_max_init_cnt_s	/* 1043 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp2_quad_mac_app_max_init_cnt_s quadmacapp2_quad_mac_app_max_init_cnt_t;

struct quadmacapp2_quad_mac_app_pause_frame_ctl_s	/* 1044 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp2_quad_mac_app_pause_frame_ctl_s quadmacapp2_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp2_quad_mac_app_pkt_err_mask_out_s	/* 1045 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp2_quad_mac_app_pkt_err_mask_out_s quadmacapp2_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp2_quad_mac_app_pkt_err_inv_s	/* 1046 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp2_quad_mac_app_pkt_err_inv_s quadmacapp2_quad_mac_app_pkt_err_inv_t;

struct quadmacapp2_quad_mac_app_crc_strip_s	/* 1047 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp2_quad_mac_app_crc_strip_s quadmacapp2_quad_mac_app_crc_strip_t;

struct quadmacapp2_quad_mac_app_tp_id_s	/* 1048 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp2_quad_mac_app_tp_id_s quadmacapp2_quad_mac_app_tp_id_t;

struct quadmacapp2_quad_mac_app_buf_store_stall_mask_s	/* 1049 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp2_quad_mac_app_buf_store_stall_mask_s quadmacapp2_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp2_quad_mac_app_keep_bay_hdr_s	/* 1050 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp2_quad_mac_app_keep_bay_hdr_s quadmacapp2_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp2_quad_mac_app_stall_record_s	/* 1051 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp2_quad_mac_app_stall_record_s quadmacapp2_quad_mac_app_stall_record_t;

struct quadmacapp2_quad_mac_app_pause_timer_out_s	/* 1052 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp2_quad_mac_app_pause_timer_out_s quadmacapp2_quad_mac_app_pause_timer_out_t;

struct quadmacapp2_quad_mac_app_vlan_ctrl_s	/* 1053 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp2_quad_mac_app_vlan_ctrl_s quadmacapp2_quad_mac_app_vlan_ctrl_t;

struct quadmacapp2_quad_mac_app_cur_tx_state_machine_s	/* 1054 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp2_quad_mac_app_cur_tx_state_machine_s quadmacapp2_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp2_quad_mac_app_stat_sel_s	/* 1055 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp2_quad_mac_app_stat_sel_s quadmacapp2_quad_mac_app_stat_sel_t;

struct quadmacapp2_quad_mac_app_debug_stats_s	/* 1056 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp2_quad_mac_app_debug_stats_s quadmacapp2_quad_mac_app_debug_stats_t;

struct quadmacapp3_quad_mac_app_interrupt_fatal_s	/* 1057 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp3_quad_mac_app_interrupt_fatal_s quadmacapp3_quad_mac_app_interrupt_fatal_t;

struct quadmacapp3_quad_mac_app_packet_len_mtu1_s	/* 1058 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp3_quad_mac_app_packet_len_mtu1_s quadmacapp3_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp3_quad_mac_app_packet_len_mtu2_s	/* 1059 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp3_quad_mac_app_packet_len_mtu2_s quadmacapp3_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp3_quad_mac_app_dot1q_delta_bytes_s	/* 1060 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp3_quad_mac_app_dot1q_delta_bytes_s quadmacapp3_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp3_quad_mac_app_init_s	/* 1061 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp3_quad_mac_app_init_s quadmacapp3_quad_mac_app_init_t;

struct quadmacapp3_quad_mac_app_init_done_s	/* 1062 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp3_quad_mac_app_init_done_s quadmacapp3_quad_mac_app_init_done_t;

struct quadmacapp3_quad_mac_app_stats_update_ctrl_s	/* 1063 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp3_quad_mac_app_stats_update_ctrl_s quadmacapp3_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp3_quad_mac_app_parity_enable_s	/* 1064 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp3_quad_mac_app_parity_enable_s quadmacapp3_quad_mac_app_parity_enable_t;

struct quadmacapp3_quad_mac_app_status_over_write_s	/* 1065 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp3_quad_mac_app_status_over_write_s quadmacapp3_quad_mac_app_status_over_write_t;

struct quadmacapp3_quad_mac_app_status_over_write_old_snap_s	/* 1066 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp3_quad_mac_app_status_over_write_old_snap_s quadmacapp3_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp3_quad_mac_app_status_over_write_new_snap_s	/* 1067 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp3_quad_mac_app_status_over_write_new_snap_s quadmacapp3_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp3_quad_mac_app_max_init_cnt_s	/* 1068 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp3_quad_mac_app_max_init_cnt_s quadmacapp3_quad_mac_app_max_init_cnt_t;

struct quadmacapp3_quad_mac_app_pause_frame_ctl_s	/* 1069 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp3_quad_mac_app_pause_frame_ctl_s quadmacapp3_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp3_quad_mac_app_pkt_err_mask_out_s	/* 1070 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp3_quad_mac_app_pkt_err_mask_out_s quadmacapp3_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp3_quad_mac_app_pkt_err_inv_s	/* 1071 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp3_quad_mac_app_pkt_err_inv_s quadmacapp3_quad_mac_app_pkt_err_inv_t;

struct quadmacapp3_quad_mac_app_crc_strip_s	/* 1072 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp3_quad_mac_app_crc_strip_s quadmacapp3_quad_mac_app_crc_strip_t;

struct quadmacapp3_quad_mac_app_tp_id_s	/* 1073 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp3_quad_mac_app_tp_id_s quadmacapp3_quad_mac_app_tp_id_t;

struct quadmacapp3_quad_mac_app_buf_store_stall_mask_s	/* 1074 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp3_quad_mac_app_buf_store_stall_mask_s quadmacapp3_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp3_quad_mac_app_keep_bay_hdr_s	/* 1075 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp3_quad_mac_app_keep_bay_hdr_s quadmacapp3_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp3_quad_mac_app_stall_record_s	/* 1076 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp3_quad_mac_app_stall_record_s quadmacapp3_quad_mac_app_stall_record_t;

struct quadmacapp3_quad_mac_app_pause_timer_out_s	/* 1077 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp3_quad_mac_app_pause_timer_out_s quadmacapp3_quad_mac_app_pause_timer_out_t;

struct quadmacapp3_quad_mac_app_vlan_ctrl_s	/* 1078 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp3_quad_mac_app_vlan_ctrl_s quadmacapp3_quad_mac_app_vlan_ctrl_t;

struct quadmacapp3_quad_mac_app_cur_tx_state_machine_s	/* 1079 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp3_quad_mac_app_cur_tx_state_machine_s quadmacapp3_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp3_quad_mac_app_stat_sel_s	/* 1080 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp3_quad_mac_app_stat_sel_s quadmacapp3_quad_mac_app_stat_sel_t;

struct quadmacapp3_quad_mac_app_debug_stats_s	/* 1081 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp3_quad_mac_app_debug_stats_s quadmacapp3_quad_mac_app_debug_stats_t;

struct quadmacapp4_quad_mac_app_interrupt_fatal_s	/* 1082 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp4_quad_mac_app_interrupt_fatal_s quadmacapp4_quad_mac_app_interrupt_fatal_t;

struct quadmacapp4_quad_mac_app_packet_len_mtu1_s	/* 1083 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp4_quad_mac_app_packet_len_mtu1_s quadmacapp4_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp4_quad_mac_app_packet_len_mtu2_s	/* 1084 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp4_quad_mac_app_packet_len_mtu2_s quadmacapp4_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp4_quad_mac_app_dot1q_delta_bytes_s	/* 1085 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp4_quad_mac_app_dot1q_delta_bytes_s quadmacapp4_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp4_quad_mac_app_init_s	/* 1086 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp4_quad_mac_app_init_s quadmacapp4_quad_mac_app_init_t;

struct quadmacapp4_quad_mac_app_init_done_s	/* 1087 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp4_quad_mac_app_init_done_s quadmacapp4_quad_mac_app_init_done_t;

struct quadmacapp4_quad_mac_app_stats_update_ctrl_s	/* 1088 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp4_quad_mac_app_stats_update_ctrl_s quadmacapp4_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp4_quad_mac_app_parity_enable_s	/* 1089 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp4_quad_mac_app_parity_enable_s quadmacapp4_quad_mac_app_parity_enable_t;

struct quadmacapp4_quad_mac_app_status_over_write_s	/* 1090 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp4_quad_mac_app_status_over_write_s quadmacapp4_quad_mac_app_status_over_write_t;

struct quadmacapp4_quad_mac_app_status_over_write_old_snap_s	/* 1091 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp4_quad_mac_app_status_over_write_old_snap_s quadmacapp4_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp4_quad_mac_app_status_over_write_new_snap_s	/* 1092 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp4_quad_mac_app_status_over_write_new_snap_s quadmacapp4_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp4_quad_mac_app_max_init_cnt_s	/* 1093 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp4_quad_mac_app_max_init_cnt_s quadmacapp4_quad_mac_app_max_init_cnt_t;

struct quadmacapp4_quad_mac_app_pause_frame_ctl_s	/* 1094 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp4_quad_mac_app_pause_frame_ctl_s quadmacapp4_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp4_quad_mac_app_pkt_err_mask_out_s	/* 1095 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp4_quad_mac_app_pkt_err_mask_out_s quadmacapp4_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp4_quad_mac_app_pkt_err_inv_s	/* 1096 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp4_quad_mac_app_pkt_err_inv_s quadmacapp4_quad_mac_app_pkt_err_inv_t;

struct quadmacapp4_quad_mac_app_crc_strip_s	/* 1097 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp4_quad_mac_app_crc_strip_s quadmacapp4_quad_mac_app_crc_strip_t;

struct quadmacapp4_quad_mac_app_tp_id_s	/* 1098 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp4_quad_mac_app_tp_id_s quadmacapp4_quad_mac_app_tp_id_t;

struct quadmacapp4_quad_mac_app_buf_store_stall_mask_s	/* 1099 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp4_quad_mac_app_buf_store_stall_mask_s quadmacapp4_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp4_quad_mac_app_keep_bay_hdr_s	/* 1100 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp4_quad_mac_app_keep_bay_hdr_s quadmacapp4_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp4_quad_mac_app_stall_record_s	/* 1101 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp4_quad_mac_app_stall_record_s quadmacapp4_quad_mac_app_stall_record_t;

struct quadmacapp4_quad_mac_app_pause_timer_out_s	/* 1102 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp4_quad_mac_app_pause_timer_out_s quadmacapp4_quad_mac_app_pause_timer_out_t;

struct quadmacapp4_quad_mac_app_vlan_ctrl_s	/* 1103 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp4_quad_mac_app_vlan_ctrl_s quadmacapp4_quad_mac_app_vlan_ctrl_t;

struct quadmacapp4_quad_mac_app_cur_tx_state_machine_s	/* 1104 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp4_quad_mac_app_cur_tx_state_machine_s quadmacapp4_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp4_quad_mac_app_stat_sel_s	/* 1105 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp4_quad_mac_app_stat_sel_s quadmacapp4_quad_mac_app_stat_sel_t;

struct quadmacapp4_quad_mac_app_debug_stats_s	/* 1106 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp4_quad_mac_app_debug_stats_s quadmacapp4_quad_mac_app_debug_stats_t;

struct quadmacapp5_quad_mac_app_interrupt_fatal_s	/* 1107 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp5_quad_mac_app_interrupt_fatal_s quadmacapp5_quad_mac_app_interrupt_fatal_t;

struct quadmacapp5_quad_mac_app_packet_len_mtu1_s	/* 1108 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp5_quad_mac_app_packet_len_mtu1_s quadmacapp5_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp5_quad_mac_app_packet_len_mtu2_s	/* 1109 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp5_quad_mac_app_packet_len_mtu2_s quadmacapp5_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp5_quad_mac_app_dot1q_delta_bytes_s	/* 1110 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp5_quad_mac_app_dot1q_delta_bytes_s quadmacapp5_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp5_quad_mac_app_init_s	/* 1111 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp5_quad_mac_app_init_s quadmacapp5_quad_mac_app_init_t;

struct quadmacapp5_quad_mac_app_init_done_s	/* 1112 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp5_quad_mac_app_init_done_s quadmacapp5_quad_mac_app_init_done_t;

struct quadmacapp5_quad_mac_app_stats_update_ctrl_s	/* 1113 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp5_quad_mac_app_stats_update_ctrl_s quadmacapp5_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp5_quad_mac_app_parity_enable_s	/* 1114 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp5_quad_mac_app_parity_enable_s quadmacapp5_quad_mac_app_parity_enable_t;

struct quadmacapp5_quad_mac_app_status_over_write_s	/* 1115 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp5_quad_mac_app_status_over_write_s quadmacapp5_quad_mac_app_status_over_write_t;

struct quadmacapp5_quad_mac_app_status_over_write_old_snap_s	/* 1116 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp5_quad_mac_app_status_over_write_old_snap_s quadmacapp5_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp5_quad_mac_app_status_over_write_new_snap_s	/* 1117 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp5_quad_mac_app_status_over_write_new_snap_s quadmacapp5_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp5_quad_mac_app_max_init_cnt_s	/* 1118 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp5_quad_mac_app_max_init_cnt_s quadmacapp5_quad_mac_app_max_init_cnt_t;

struct quadmacapp5_quad_mac_app_pause_frame_ctl_s	/* 1119 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp5_quad_mac_app_pause_frame_ctl_s quadmacapp5_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp5_quad_mac_app_pkt_err_mask_out_s	/* 1120 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp5_quad_mac_app_pkt_err_mask_out_s quadmacapp5_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp5_quad_mac_app_pkt_err_inv_s	/* 1121 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp5_quad_mac_app_pkt_err_inv_s quadmacapp5_quad_mac_app_pkt_err_inv_t;

struct quadmacapp5_quad_mac_app_crc_strip_s	/* 1122 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp5_quad_mac_app_crc_strip_s quadmacapp5_quad_mac_app_crc_strip_t;

struct quadmacapp5_quad_mac_app_tp_id_s	/* 1123 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp5_quad_mac_app_tp_id_s quadmacapp5_quad_mac_app_tp_id_t;

struct quadmacapp5_quad_mac_app_buf_store_stall_mask_s	/* 1124 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp5_quad_mac_app_buf_store_stall_mask_s quadmacapp5_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp5_quad_mac_app_keep_bay_hdr_s	/* 1125 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp5_quad_mac_app_keep_bay_hdr_s quadmacapp5_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp5_quad_mac_app_stall_record_s	/* 1126 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp5_quad_mac_app_stall_record_s quadmacapp5_quad_mac_app_stall_record_t;

struct quadmacapp5_quad_mac_app_pause_timer_out_s	/* 1127 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp5_quad_mac_app_pause_timer_out_s quadmacapp5_quad_mac_app_pause_timer_out_t;

struct quadmacapp5_quad_mac_app_vlan_ctrl_s	/* 1128 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp5_quad_mac_app_vlan_ctrl_s quadmacapp5_quad_mac_app_vlan_ctrl_t;

struct quadmacapp5_quad_mac_app_cur_tx_state_machine_s	/* 1129 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp5_quad_mac_app_cur_tx_state_machine_s quadmacapp5_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp5_quad_mac_app_stat_sel_s	/* 1130 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp5_quad_mac_app_stat_sel_s quadmacapp5_quad_mac_app_stat_sel_t;

struct quadmacapp5_quad_mac_app_debug_stats_s	/* 1131 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp5_quad_mac_app_debug_stats_s quadmacapp5_quad_mac_app_debug_stats_t;

struct quadmacapp6_quad_mac_app_interrupt_fatal_s	/* 1132 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp6_quad_mac_app_interrupt_fatal_s quadmacapp6_quad_mac_app_interrupt_fatal_t;

struct quadmacapp6_quad_mac_app_packet_len_mtu1_s	/* 1133 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp6_quad_mac_app_packet_len_mtu1_s quadmacapp6_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp6_quad_mac_app_packet_len_mtu2_s	/* 1134 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp6_quad_mac_app_packet_len_mtu2_s quadmacapp6_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp6_quad_mac_app_dot1q_delta_bytes_s	/* 1135 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp6_quad_mac_app_dot1q_delta_bytes_s quadmacapp6_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp6_quad_mac_app_init_s	/* 1136 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp6_quad_mac_app_init_s quadmacapp6_quad_mac_app_init_t;

struct quadmacapp6_quad_mac_app_init_done_s	/* 1137 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp6_quad_mac_app_init_done_s quadmacapp6_quad_mac_app_init_done_t;

struct quadmacapp6_quad_mac_app_stats_update_ctrl_s	/* 1138 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp6_quad_mac_app_stats_update_ctrl_s quadmacapp6_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp6_quad_mac_app_parity_enable_s	/* 1139 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp6_quad_mac_app_parity_enable_s quadmacapp6_quad_mac_app_parity_enable_t;

struct quadmacapp6_quad_mac_app_status_over_write_s	/* 1140 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp6_quad_mac_app_status_over_write_s quadmacapp6_quad_mac_app_status_over_write_t;

struct quadmacapp6_quad_mac_app_status_over_write_old_snap_s	/* 1141 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp6_quad_mac_app_status_over_write_old_snap_s quadmacapp6_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp6_quad_mac_app_status_over_write_new_snap_s	/* 1142 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp6_quad_mac_app_status_over_write_new_snap_s quadmacapp6_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp6_quad_mac_app_max_init_cnt_s	/* 1143 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp6_quad_mac_app_max_init_cnt_s quadmacapp6_quad_mac_app_max_init_cnt_t;

struct quadmacapp6_quad_mac_app_pause_frame_ctl_s	/* 1144 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp6_quad_mac_app_pause_frame_ctl_s quadmacapp6_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp6_quad_mac_app_pkt_err_mask_out_s	/* 1145 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp6_quad_mac_app_pkt_err_mask_out_s quadmacapp6_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp6_quad_mac_app_pkt_err_inv_s	/* 1146 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp6_quad_mac_app_pkt_err_inv_s quadmacapp6_quad_mac_app_pkt_err_inv_t;

struct quadmacapp6_quad_mac_app_crc_strip_s	/* 1147 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp6_quad_mac_app_crc_strip_s quadmacapp6_quad_mac_app_crc_strip_t;

struct quadmacapp6_quad_mac_app_tp_id_s	/* 1148 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp6_quad_mac_app_tp_id_s quadmacapp6_quad_mac_app_tp_id_t;

struct quadmacapp6_quad_mac_app_buf_store_stall_mask_s	/* 1149 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp6_quad_mac_app_buf_store_stall_mask_s quadmacapp6_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp6_quad_mac_app_keep_bay_hdr_s	/* 1150 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp6_quad_mac_app_keep_bay_hdr_s quadmacapp6_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp6_quad_mac_app_stall_record_s	/* 1151 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp6_quad_mac_app_stall_record_s quadmacapp6_quad_mac_app_stall_record_t;

struct quadmacapp6_quad_mac_app_pause_timer_out_s	/* 1152 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp6_quad_mac_app_pause_timer_out_s quadmacapp6_quad_mac_app_pause_timer_out_t;

struct quadmacapp6_quad_mac_app_vlan_ctrl_s	/* 1153 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp6_quad_mac_app_vlan_ctrl_s quadmacapp6_quad_mac_app_vlan_ctrl_t;

struct quadmacapp6_quad_mac_app_cur_tx_state_machine_s	/* 1154 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp6_quad_mac_app_cur_tx_state_machine_s quadmacapp6_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp6_quad_mac_app_stat_sel_s	/* 1155 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp6_quad_mac_app_stat_sel_s quadmacapp6_quad_mac_app_stat_sel_t;

struct quadmacapp6_quad_mac_app_debug_stats_s	/* 1156 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp6_quad_mac_app_debug_stats_s quadmacapp6_quad_mac_app_debug_stats_t;

struct quadmacapp7_quad_mac_app_interrupt_fatal_s	/* 1157 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp7_quad_mac_app_interrupt_fatal_s quadmacapp7_quad_mac_app_interrupt_fatal_t;

struct quadmacapp7_quad_mac_app_packet_len_mtu1_s	/* 1158 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp7_quad_mac_app_packet_len_mtu1_s quadmacapp7_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp7_quad_mac_app_packet_len_mtu2_s	/* 1159 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp7_quad_mac_app_packet_len_mtu2_s quadmacapp7_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp7_quad_mac_app_dot1q_delta_bytes_s	/* 1160 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp7_quad_mac_app_dot1q_delta_bytes_s quadmacapp7_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp7_quad_mac_app_init_s	/* 1161 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp7_quad_mac_app_init_s quadmacapp7_quad_mac_app_init_t;

struct quadmacapp7_quad_mac_app_init_done_s	/* 1162 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp7_quad_mac_app_init_done_s quadmacapp7_quad_mac_app_init_done_t;

struct quadmacapp7_quad_mac_app_stats_update_ctrl_s	/* 1163 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp7_quad_mac_app_stats_update_ctrl_s quadmacapp7_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp7_quad_mac_app_parity_enable_s	/* 1164 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp7_quad_mac_app_parity_enable_s quadmacapp7_quad_mac_app_parity_enable_t;

struct quadmacapp7_quad_mac_app_status_over_write_s	/* 1165 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp7_quad_mac_app_status_over_write_s quadmacapp7_quad_mac_app_status_over_write_t;

struct quadmacapp7_quad_mac_app_status_over_write_old_snap_s	/* 1166 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp7_quad_mac_app_status_over_write_old_snap_s quadmacapp7_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp7_quad_mac_app_status_over_write_new_snap_s	/* 1167 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp7_quad_mac_app_status_over_write_new_snap_s quadmacapp7_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp7_quad_mac_app_max_init_cnt_s	/* 1168 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp7_quad_mac_app_max_init_cnt_s quadmacapp7_quad_mac_app_max_init_cnt_t;

struct quadmacapp7_quad_mac_app_pause_frame_ctl_s	/* 1169 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp7_quad_mac_app_pause_frame_ctl_s quadmacapp7_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp7_quad_mac_app_pkt_err_mask_out_s	/* 1170 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp7_quad_mac_app_pkt_err_mask_out_s quadmacapp7_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp7_quad_mac_app_pkt_err_inv_s	/* 1171 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp7_quad_mac_app_pkt_err_inv_s quadmacapp7_quad_mac_app_pkt_err_inv_t;

struct quadmacapp7_quad_mac_app_crc_strip_s	/* 1172 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp7_quad_mac_app_crc_strip_s quadmacapp7_quad_mac_app_crc_strip_t;

struct quadmacapp7_quad_mac_app_tp_id_s	/* 1173 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp7_quad_mac_app_tp_id_s quadmacapp7_quad_mac_app_tp_id_t;

struct quadmacapp7_quad_mac_app_buf_store_stall_mask_s	/* 1174 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp7_quad_mac_app_buf_store_stall_mask_s quadmacapp7_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp7_quad_mac_app_keep_bay_hdr_s	/* 1175 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp7_quad_mac_app_keep_bay_hdr_s quadmacapp7_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp7_quad_mac_app_stall_record_s	/* 1176 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp7_quad_mac_app_stall_record_s quadmacapp7_quad_mac_app_stall_record_t;

struct quadmacapp7_quad_mac_app_pause_timer_out_s	/* 1177 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp7_quad_mac_app_pause_timer_out_s quadmacapp7_quad_mac_app_pause_timer_out_t;

struct quadmacapp7_quad_mac_app_vlan_ctrl_s	/* 1178 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp7_quad_mac_app_vlan_ctrl_s quadmacapp7_quad_mac_app_vlan_ctrl_t;

struct quadmacapp7_quad_mac_app_cur_tx_state_machine_s	/* 1179 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp7_quad_mac_app_cur_tx_state_machine_s quadmacapp7_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp7_quad_mac_app_stat_sel_s	/* 1180 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp7_quad_mac_app_stat_sel_s quadmacapp7_quad_mac_app_stat_sel_t;

struct quadmacapp7_quad_mac_app_debug_stats_s	/* 1181 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp7_quad_mac_app_debug_stats_s quadmacapp7_quad_mac_app_debug_stats_t;

struct quadmacapp8_quad_mac_app_interrupt_fatal_s	/* 1182 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp8_quad_mac_app_interrupt_fatal_s quadmacapp8_quad_mac_app_interrupt_fatal_t;

struct quadmacapp8_quad_mac_app_packet_len_mtu1_s	/* 1183 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp8_quad_mac_app_packet_len_mtu1_s quadmacapp8_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp8_quad_mac_app_packet_len_mtu2_s	/* 1184 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp8_quad_mac_app_packet_len_mtu2_s quadmacapp8_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp8_quad_mac_app_dot1q_delta_bytes_s	/* 1185 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp8_quad_mac_app_dot1q_delta_bytes_s quadmacapp8_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp8_quad_mac_app_init_s	/* 1186 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp8_quad_mac_app_init_s quadmacapp8_quad_mac_app_init_t;

struct quadmacapp8_quad_mac_app_init_done_s	/* 1187 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp8_quad_mac_app_init_done_s quadmacapp8_quad_mac_app_init_done_t;

struct quadmacapp8_quad_mac_app_stats_update_ctrl_s	/* 1188 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp8_quad_mac_app_stats_update_ctrl_s quadmacapp8_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp8_quad_mac_app_parity_enable_s	/* 1189 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp8_quad_mac_app_parity_enable_s quadmacapp8_quad_mac_app_parity_enable_t;

struct quadmacapp8_quad_mac_app_status_over_write_s	/* 1190 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp8_quad_mac_app_status_over_write_s quadmacapp8_quad_mac_app_status_over_write_t;

struct quadmacapp8_quad_mac_app_status_over_write_old_snap_s	/* 1191 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp8_quad_mac_app_status_over_write_old_snap_s quadmacapp8_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp8_quad_mac_app_status_over_write_new_snap_s	/* 1192 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp8_quad_mac_app_status_over_write_new_snap_s quadmacapp8_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp8_quad_mac_app_max_init_cnt_s	/* 1193 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp8_quad_mac_app_max_init_cnt_s quadmacapp8_quad_mac_app_max_init_cnt_t;

struct quadmacapp8_quad_mac_app_pause_frame_ctl_s	/* 1194 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp8_quad_mac_app_pause_frame_ctl_s quadmacapp8_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp8_quad_mac_app_pkt_err_mask_out_s	/* 1195 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp8_quad_mac_app_pkt_err_mask_out_s quadmacapp8_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp8_quad_mac_app_pkt_err_inv_s	/* 1196 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp8_quad_mac_app_pkt_err_inv_s quadmacapp8_quad_mac_app_pkt_err_inv_t;

struct quadmacapp8_quad_mac_app_crc_strip_s	/* 1197 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp8_quad_mac_app_crc_strip_s quadmacapp8_quad_mac_app_crc_strip_t;

struct quadmacapp8_quad_mac_app_tp_id_s	/* 1198 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp8_quad_mac_app_tp_id_s quadmacapp8_quad_mac_app_tp_id_t;

struct quadmacapp8_quad_mac_app_buf_store_stall_mask_s	/* 1199 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp8_quad_mac_app_buf_store_stall_mask_s quadmacapp8_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp8_quad_mac_app_keep_bay_hdr_s	/* 1200 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp8_quad_mac_app_keep_bay_hdr_s quadmacapp8_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp8_quad_mac_app_stall_record_s	/* 1201 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp8_quad_mac_app_stall_record_s quadmacapp8_quad_mac_app_stall_record_t;

struct quadmacapp8_quad_mac_app_pause_timer_out_s	/* 1202 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp8_quad_mac_app_pause_timer_out_s quadmacapp8_quad_mac_app_pause_timer_out_t;

struct quadmacapp8_quad_mac_app_vlan_ctrl_s	/* 1203 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp8_quad_mac_app_vlan_ctrl_s quadmacapp8_quad_mac_app_vlan_ctrl_t;

struct quadmacapp8_quad_mac_app_cur_tx_state_machine_s	/* 1204 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp8_quad_mac_app_cur_tx_state_machine_s quadmacapp8_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp8_quad_mac_app_stat_sel_s	/* 1205 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp8_quad_mac_app_stat_sel_s quadmacapp8_quad_mac_app_stat_sel_t;

struct quadmacapp8_quad_mac_app_debug_stats_s	/* 1206 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp8_quad_mac_app_debug_stats_s quadmacapp8_quad_mac_app_debug_stats_t;

struct quadmacapp9_quad_mac_app_interrupt_fatal_s	/* 1207 */
{

	uint32 rsv_0	:8;
	uint32 value_set0	:24;

	uint32 rsv_1	:8;
	uint32 value_reset0	:24;

	uint32 rsv_2	:8;
	uint32 mask_set0	:24;

	uint32 rsv_3	:8;
	uint32 mask_reset0	:24;
};
typedef struct quadmacapp9_quad_mac_app_interrupt_fatal_s quadmacapp9_quad_mac_app_interrupt_fatal_t;

struct quadmacapp9_quad_mac_app_packet_len_mtu1_s	/* 1208 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct quadmacapp9_quad_mac_app_packet_len_mtu1_s quadmacapp9_quad_mac_app_packet_len_mtu1_t;

struct quadmacapp9_quad_mac_app_packet_len_mtu2_s	/* 1209 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct quadmacapp9_quad_mac_app_packet_len_mtu2_s quadmacapp9_quad_mac_app_packet_len_mtu2_t;

struct quadmacapp9_quad_mac_app_dot1q_delta_bytes_s	/* 1210 */
{

	uint32 rsv_0	:28;
	uint32 dot1q_delta_bytes	:4;
};
typedef struct quadmacapp9_quad_mac_app_dot1q_delta_bytes_s quadmacapp9_quad_mac_app_dot1q_delta_bytes_t;

struct quadmacapp9_quad_mac_app_init_s	/* 1211 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp9_quad_mac_app_init_s quadmacapp9_quad_mac_app_init_t;

struct quadmacapp9_quad_mac_app_init_done_s	/* 1212 */
{

	uint32 rsv_0	:31;
	uint32 quad_mac_init	:1;
};
typedef struct quadmacapp9_quad_mac_app_init_done_s quadmacapp9_quad_mac_app_init_done_t;

struct quadmacapp9_quad_mac_app_stats_update_ctrl_s	/* 1213 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct quadmacapp9_quad_mac_app_stats_update_ctrl_s quadmacapp9_quad_mac_app_stats_update_ctrl_t;

struct quadmacapp9_quad_mac_app_parity_enable_s	/* 1214 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct quadmacapp9_quad_mac_app_parity_enable_s quadmacapp9_quad_mac_app_parity_enable_t;

struct quadmacapp9_quad_mac_app_status_over_write_s	/* 1215 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp9_quad_mac_app_status_over_write_s quadmacapp9_quad_mac_app_status_over_write_t;

struct quadmacapp9_quad_mac_app_status_over_write_old_snap_s	/* 1216 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp9_quad_mac_app_status_over_write_old_snap_s quadmacapp9_quad_mac_app_status_over_write_old_snap_t;

struct quadmacapp9_quad_mac_app_status_over_write_new_snap_s	/* 1217 */
{

	uint32 rsv_0	:22;
	uint32 port_num	:2;
	uint32 rsv_1	:3;
	uint32 update_fifo_status	:1;
	uint32 rsv_2	:3;
	uint32 status_over_write	:1;
};
typedef struct quadmacapp9_quad_mac_app_status_over_write_new_snap_s quadmacapp9_quad_mac_app_status_over_write_new_snap_t;

struct quadmacapp9_quad_mac_app_max_init_cnt_s	/* 1218 */
{

	uint32 rsv_0	:24;
	uint32 max_init_cnt	:8;
};
typedef struct quadmacapp9_quad_mac_app_max_init_cnt_s quadmacapp9_quad_mac_app_max_init_cnt_t;

struct quadmacapp9_quad_mac_app_pause_frame_ctl_s	/* 1219 */
{

	uint32 rsv_0	:20;
	uint32 pause_off_enable	:4;
	uint32 rsv_1	:4;
	uint32 pause_frame_dis	:4;
};
typedef struct quadmacapp9_quad_mac_app_pause_frame_ctl_s quadmacapp9_quad_mac_app_pause_frame_ctl_t;

struct quadmacapp9_quad_mac_app_pkt_err_mask_out_s	/* 1220 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_mask_out	:4;
};
typedef struct quadmacapp9_quad_mac_app_pkt_err_mask_out_s quadmacapp9_quad_mac_app_pkt_err_mask_out_t;

struct quadmacapp9_quad_mac_app_pkt_err_inv_s	/* 1221 */
{

	uint32 rsv_0	:28;
	uint32 pkt_err_inv	:4;
};
typedef struct quadmacapp9_quad_mac_app_pkt_err_inv_s quadmacapp9_quad_mac_app_pkt_err_inv_t;

struct quadmacapp9_quad_mac_app_crc_strip_s	/* 1222 */
{

	uint32 rsv_0	:28;
	uint32 crc_strip	:4;
};
typedef struct quadmacapp9_quad_mac_app_crc_strip_s quadmacapp9_quad_mac_app_crc_strip_t;

struct quadmacapp9_quad_mac_app_tp_id_s	/* 1223 */
{

	uint32 rsv_0	:16;
	uint32 tp_id	:16;
};
typedef struct quadmacapp9_quad_mac_app_tp_id_s quadmacapp9_quad_mac_app_tp_id_t;

struct quadmacapp9_quad_mac_app_buf_store_stall_mask_s	/* 1224 */
{

	uint32 rsv_0	:28;
	uint32 buf_store_stall_mask	:4;
};
typedef struct quadmacapp9_quad_mac_app_buf_store_stall_mask_s quadmacapp9_quad_mac_app_buf_store_stall_mask_t;

struct quadmacapp9_quad_mac_app_keep_bay_hdr_s	/* 1225 */
{

	uint32 rsv_0	:28;
	uint32 keep_bay_hdr	:4;
};
typedef struct quadmacapp9_quad_mac_app_keep_bay_hdr_s quadmacapp9_quad_mac_app_keep_bay_hdr_t;

struct quadmacapp9_quad_mac_app_stall_record_s	/* 1226 */
{

	uint32 snap_shot_en	:1;
	uint32 rsv_0	:27;
	uint32 buf_store_stall_record	:4;
};
typedef struct quadmacapp9_quad_mac_app_stall_record_s quadmacapp9_quad_mac_app_stall_record_t;

struct quadmacapp9_quad_mac_app_pause_timer_out_s	/* 1227 */
{

	uint32 gmac0_pause_timer_out	:32;

	uint32 gmac1_pause_timer_out	:32;

	uint32 gmac2_pause_timer_out	:32;

	uint32 gmac3_pause_timer_out	:32;
};
typedef struct quadmacapp9_quad_mac_app_pause_timer_out_s quadmacapp9_quad_mac_app_pause_timer_out_t;

struct quadmacapp9_quad_mac_app_vlan_ctrl_s	/* 1228 */
{

	uint32 rsv_0	:28;
	uint32 use_hdr_priority	:4;

	uint32 rsv_1	:28;
	uint32 cos_cfi	:4;
};
typedef struct quadmacapp9_quad_mac_app_vlan_ctrl_s quadmacapp9_quad_mac_app_vlan_ctrl_t;

struct quadmacapp9_quad_mac_app_cur_tx_state_machine_s	/* 1229 */
{

	uint32 rsv_0	:16;
	uint32 cur_tx_state_machine	:16;
};
typedef struct quadmacapp9_quad_mac_app_cur_tx_state_machine_s quadmacapp9_quad_mac_app_cur_tx_state_machine_t;

struct quadmacapp9_quad_mac_app_stat_sel_s	/* 1230 */
{

	uint32 rsv_0	:28;
	uint32 stat_sel	:4;
};
typedef struct quadmacapp9_quad_mac_app_stat_sel_s quadmacapp9_quad_mac_app_stat_sel_t;

struct quadmacapp9_quad_mac_app_debug_stats_s	/* 1231 */
{

	uint32 rsv_0	:28;
	uint32 rx_input_sop_cnt	:4;

	uint32 rsv_1	:28;
	uint32 rx_input_eop_cnt	:4;

	uint32 rsv_2	:28;
	uint32 rx_input_pkt_error_cnt	:4;

	uint32 rsv_3	:28;
	uint32 rx_input_data_error_cnt	:4;

	uint32 rsv_4	:28;
	uint32 rx_output_sop_cnt	:4;

	uint32 rsv_5	:28;
	uint32 rx_output_eop_cnt	:4;

	uint32 rsv_6	:28;
	uint32 rx_output_pkt_error_cnt	:4;

	uint32 rsv_7	:28;
	uint32 rx_output_data_error_cnt	:4;

	uint32 rsv_8	:28;
	uint32 tx_input_sop_cnt	:4;

	uint32 rsv_9	:28;
	uint32 tx_input_eop_cnt	:4;

	uint32 rsv_10	:28;
	uint32 tx_input_pkt_error_cnt	:4;

	uint32 rsv_11	:28;
	uint32 tx_input_data_error_cnt	:4;

	uint32 rsv_12	:28;
	uint32 tx_output_sop_cnt	:4;

	uint32 rsv_13	:28;
	uint32 tx_output_eop_cnt	:4;

	uint32 rsv_14	:28;
	uint32 tx_output_pkt_error_cnt	:4;
};
typedef struct quadmacapp9_quad_mac_app_debug_stats_s quadmacapp9_quad_mac_app_debug_stats_t;

struct sgmac0_sgmac_mdio_cmd_s	/* 1232 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct sgmac0_sgmac_mdio_cmd_s sgmac0_sgmac_mdio_cmd_t;

struct sgmac0_sgmac_config1_s	/* 1233 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 sgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct sgmac0_sgmac_config1_s sgmac0_sgmac_config1_t;

struct sgmac0_sgmac_config2_s	/* 1234 */
{

	uint32 rsv_0	:11;
	uint32 buf_fifo_almost_empty_threshold	:5;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 rsv_1	:3;
	uint32 buf_fifo_almost_full_threshold	:5;
};
typedef struct sgmac0_sgmac_config2_s sgmac0_sgmac_config2_t;

struct sgmac0_sgmac_config3_s	/* 1235 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct sgmac0_sgmac_config3_s sgmac0_sgmac_config3_t;

struct sgmac0_sgmac_config4_s	/* 1236 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:1;
	uint32 e2e_crc_bit_swizzle	:1;
	uint32 e2e_msg_error_en	:1;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct sgmac0_sgmac_config4_s sgmac0_sgmac_config4_t;

struct sgmac0_sgmac_soft_rst_s	/* 1237 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct sgmac0_sgmac_soft_rst_s sgmac0_sgmac_soft_rst_t;

struct sgmac0_sgmac_dbg1_s	/* 1238 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct sgmac0_sgmac_dbg1_s sgmac0_sgmac_dbg1_t;

struct sgmac0_sgmac_mdio_rd_data_s	/* 1239 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct sgmac0_sgmac_mdio_rd_data_s sgmac0_sgmac_mdio_rd_data_t;

struct sgmac0_sgmac_stretch_mode_s	/* 1240 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct sgmac0_sgmac_stretch_mode_s sgmac0_sgmac_stretch_mode_t;

struct sgmac0_sgmac_stats_mtu1_s	/* 1241 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct sgmac0_sgmac_stats_mtu1_s sgmac0_sgmac_stats_mtu1_t;

struct sgmac0_sgmac_stats_mtu2_s	/* 1242 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct sgmac0_sgmac_stats_mtu2_s sgmac0_sgmac_stats_mtu2_t;

struct sgmac0_sgmac_stats_config_s	/* 1243 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct sgmac0_sgmac_stats_config_s sgmac0_sgmac_stats_config_t;

struct sgmac0_sgmac_stats_init_s	/* 1244 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac0_sgmac_stats_init_s sgmac0_sgmac_stats_init_t;

struct sgmac0_sgmac_stats_init_done_s	/* 1245 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac0_sgmac_stats_init_done_s sgmac0_sgmac_stats_init_done_t;

struct sgmac0_sgmac_deskew_fifo_config_s	/* 1246 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct sgmac0_sgmac_deskew_fifo_config_s sgmac0_sgmac_deskew_fifo_config_t;

struct sgmac0_sgmac_config5_s	/* 1247 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct sgmac0_sgmac_config5_s sgmac0_sgmac_config5_t;

struct sgmac0_sgmac_config6_s	/* 1248 */
{

	uint32 rsv_0	:16;
	uint32 mac_sa_cfg47_to32	:16;
};
typedef struct sgmac0_sgmac_config6_s sgmac0_sgmac_config6_t;

struct sgmac0_sgmac_interrupt_status_set_s	/* 1249 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct sgmac0_sgmac_interrupt_status_set_s sgmac0_sgmac_interrupt_status_set_t;

struct sgmac0_sgmac_interrupt_status_reset_s	/* 1250 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct sgmac0_sgmac_interrupt_status_reset_s sgmac0_sgmac_interrupt_status_reset_t;

struct sgmac0_sgmac_interrupt_mask_set_s	/* 1251 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct sgmac0_sgmac_interrupt_mask_set_s sgmac0_sgmac_interrupt_mask_set_t;

struct sgmac0_sgmac_interrupt_mask_reset_s	/* 1252 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct sgmac0_sgmac_interrupt_mask_reset_s sgmac0_sgmac_interrupt_mask_reset_t;

struct sgmac0_sgmac_prbs_cfg_s	/* 1253 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct sgmac0_sgmac_prbs_cfg_s sgmac0_sgmac_prbs_cfg_t;

struct sgmac0_sgmac_prbs_err_cnt0_s	/* 1254 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct sgmac0_sgmac_prbs_err_cnt0_s sgmac0_sgmac_prbs_err_cnt0_t;

struct sgmac0_sgmac_prbs_err_cnt1_s	/* 1255 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct sgmac0_sgmac_prbs_err_cnt1_s sgmac0_sgmac_prbs_err_cnt1_t;

struct sgmac0_sgmac_prbs_err_cnt2_s	/* 1256 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct sgmac0_sgmac_prbs_err_cnt2_s sgmac0_sgmac_prbs_err_cnt2_t;

struct sgmac0_sgmac_prbs_err_cnt3_s	/* 1257 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct sgmac0_sgmac_prbs_err_cnt3_s sgmac0_sgmac_prbs_err_cnt3_t;

struct sgmac0_sgmac8_b10b_err_cnt_s	/* 1258 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct sgmac0_sgmac8_b10b_err_cnt_s sgmac0_sgmac8_b10b_err_cnt_t;

struct sgmac0_sgmac_tp_id_s	/* 1259 */
{

	uint32 rsv_0	:16;
	uint32 sgmac_tp_id	:16;
};
typedef struct sgmac0_sgmac_tp_id_s sgmac0_sgmac_tp_id_t;

struct sgmac0_sgmac_use_orginal_cos_s	/* 1260 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct sgmac0_sgmac_use_orginal_cos_s sgmac0_sgmac_use_orginal_cos_t;

struct sgmac0_sgmac_priority_map_s	/* 1261 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct sgmac0_sgmac_priority_map_s sgmac0_sgmac_priority_map_t;

struct sgmac0_sgmac_gen_pkt_s	/* 1262 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct sgmac0_sgmac_gen_pkt_s sgmac0_sgmac_gen_pkt_t;

struct sgmac0_sgmac_payload_s	/* 1263 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct sgmac0_sgmac_payload_s sgmac0_sgmac_payload_t;

struct sgmac0_sgmac_drain_en_s	/* 1264 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_drain_en	:1;
};
typedef struct sgmac0_sgmac_drain_en_s sgmac0_sgmac_drain_en_t;

struct sgmac0_sgmac_mdio_cmd_status_s	/* 1265 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct sgmac0_sgmac_mdio_cmd_status_s sgmac0_sgmac_mdio_cmd_status_t;

struct sgmac0_sgmac_mdio_cfg_s	/* 1266 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_sgmac_pre	:5;
};
typedef struct sgmac0_sgmac_mdio_cfg_s sgmac0_sgmac_mdio_cfg_t;

struct sgmac0_sgmac_ptp_en_s	/* 1267 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct sgmac0_sgmac_ptp_en_s sgmac0_sgmac_ptp_en_t;

struct sgmac0_sgmac_mode_s	/* 1268 */
{

	uint32 rsv_0	:30;
	uint32 header_mode	:2;
};
typedef struct sgmac0_sgmac_mode_s sgmac0_sgmac_mode_t;

struct sgmac0_sgmac_ct_superg_tx_cfg_s	/* 1269 */
{

	uint32 rsv_0	:29;
	uint32 inter_msg_gap	:3;
};
typedef struct sgmac0_sgmac_ct_superg_tx_cfg_s sgmac0_sgmac_ct_superg_tx_cfg_t;

struct sgmac0_sgmac_ct_superg_rx_cfg_s	/* 1270 */
{

	uint32 fc_msg_type_physical_lvl	:8;
	uint32 rsv_0	:4;
	uint32 fc_msg_fc_obj_physical_lvl	:4;
	uint32 fc_msg_type_logical_lvl	:8;
	uint32 fc_msg_fc_obj_logical_lvl	:4;
	uint32 rsv_1	:3;
	uint32 fc_msg_rx_en	:1;
};
typedef struct sgmac0_sgmac_ct_superg_rx_cfg_s sgmac0_sgmac_ct_superg_rx_cfg_t;

struct sgmac0_sgmac_ct_superg_mon_stats_s	/* 1271 */
{

	uint32 fc_msg_physical_lvl_tx_cnt	:16;
	uint32 fc_msg_physical_lvl_rx_cnt	:16;

	uint32 fc_msg_error_rx_cnt	:16;
	uint32 fc_msg_logical_lvl_rx_cnt	:16;
};
typedef struct sgmac0_sgmac_ct_superg_mon_stats_s sgmac0_sgmac_ct_superg_mon_stats_t;

struct sgmac0_sgmac_ts_status_s	/* 1272 */
{

	uint32 rsv_0	:28;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
};
typedef struct sgmac0_sgmac_ts_status_s sgmac0_sgmac_ts_status_t;

struct sgmac0_sgmac_ctl_s	/* 1273 */
{

	uint32 rsv_0	:1;
	uint32 header_mode	:2;
	uint32 cpu_queue_select_type	:5;
	uint32 rsv_1	:1;
	uint32 sgmac2_dvp_clear_en	:1;
	uint32 queue_sel_type_bits	:2;
	uint32 queue_sel_type_en	:1;
	uint32 sgmac2_bcast_vlan_en	:1;
	uint32 sgmac_lbid_en	:1;
	uint32 dual_mod_en	:1;
	uint32 dvp_met_ptr_base	:16;

	uint32 l3_met_ptr_base	:16;
	uint32 l2_met_ptr_base	:16;
};
typedef struct sgmac0_sgmac_ctl_s sgmac0_sgmac_ctl_t;

struct sgmac1_sgmac_mdio_cmd_s	/* 1274 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct sgmac1_sgmac_mdio_cmd_s sgmac1_sgmac_mdio_cmd_t;

struct sgmac1_sgmac_config1_s	/* 1275 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 sgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct sgmac1_sgmac_config1_s sgmac1_sgmac_config1_t;

struct sgmac1_sgmac_config2_s	/* 1276 */
{

	uint32 rsv_0	:11;
	uint32 buf_fifo_almost_empty_threshold	:5;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 rsv_1	:3;
	uint32 buf_fifo_almost_full_threshold	:5;
};
typedef struct sgmac1_sgmac_config2_s sgmac1_sgmac_config2_t;

struct sgmac1_sgmac_config3_s	/* 1277 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct sgmac1_sgmac_config3_s sgmac1_sgmac_config3_t;

struct sgmac1_sgmac_config4_s	/* 1278 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:1;
	uint32 e2e_crc_bit_swizzle	:1;
	uint32 e2e_msg_error_en	:1;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct sgmac1_sgmac_config4_s sgmac1_sgmac_config4_t;

struct sgmac1_sgmac_soft_rst_s	/* 1279 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct sgmac1_sgmac_soft_rst_s sgmac1_sgmac_soft_rst_t;

struct sgmac1_sgmac_dbg1_s	/* 1280 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct sgmac1_sgmac_dbg1_s sgmac1_sgmac_dbg1_t;

struct sgmac1_sgmac_mdio_rd_data_s	/* 1281 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct sgmac1_sgmac_mdio_rd_data_s sgmac1_sgmac_mdio_rd_data_t;

struct sgmac1_sgmac_stretch_mode_s	/* 1282 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct sgmac1_sgmac_stretch_mode_s sgmac1_sgmac_stretch_mode_t;

struct sgmac1_sgmac_stats_mtu1_s	/* 1283 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct sgmac1_sgmac_stats_mtu1_s sgmac1_sgmac_stats_mtu1_t;

struct sgmac1_sgmac_stats_mtu2_s	/* 1284 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct sgmac1_sgmac_stats_mtu2_s sgmac1_sgmac_stats_mtu2_t;

struct sgmac1_sgmac_stats_config_s	/* 1285 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct sgmac1_sgmac_stats_config_s sgmac1_sgmac_stats_config_t;

struct sgmac1_sgmac_stats_init_s	/* 1286 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac1_sgmac_stats_init_s sgmac1_sgmac_stats_init_t;

struct sgmac1_sgmac_stats_init_done_s	/* 1287 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac1_sgmac_stats_init_done_s sgmac1_sgmac_stats_init_done_t;

struct sgmac1_sgmac_deskew_fifo_config_s	/* 1288 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct sgmac1_sgmac_deskew_fifo_config_s sgmac1_sgmac_deskew_fifo_config_t;

struct sgmac1_sgmac_config5_s	/* 1289 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct sgmac1_sgmac_config5_s sgmac1_sgmac_config5_t;

struct sgmac1_sgmac_config6_s	/* 1290 */
{

	uint32 rsv_0	:16;
	uint32 mac_sa_cfg47_to32	:16;
};
typedef struct sgmac1_sgmac_config6_s sgmac1_sgmac_config6_t;

struct sgmac1_sgmac_interrupt_status_set_s	/* 1291 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct sgmac1_sgmac_interrupt_status_set_s sgmac1_sgmac_interrupt_status_set_t;

struct sgmac1_sgmac_interrupt_status_reset_s	/* 1292 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct sgmac1_sgmac_interrupt_status_reset_s sgmac1_sgmac_interrupt_status_reset_t;

struct sgmac1_sgmac_interrupt_mask_set_s	/* 1293 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct sgmac1_sgmac_interrupt_mask_set_s sgmac1_sgmac_interrupt_mask_set_t;

struct sgmac1_sgmac_interrupt_mask_reset_s	/* 1294 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct sgmac1_sgmac_interrupt_mask_reset_s sgmac1_sgmac_interrupt_mask_reset_t;

struct sgmac1_sgmac_prbs_cfg_s	/* 1295 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct sgmac1_sgmac_prbs_cfg_s sgmac1_sgmac_prbs_cfg_t;

struct sgmac1_sgmac_prbs_err_cnt0_s	/* 1296 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct sgmac1_sgmac_prbs_err_cnt0_s sgmac1_sgmac_prbs_err_cnt0_t;

struct sgmac1_sgmac_prbs_err_cnt1_s	/* 1297 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct sgmac1_sgmac_prbs_err_cnt1_s sgmac1_sgmac_prbs_err_cnt1_t;

struct sgmac1_sgmac_prbs_err_cnt2_s	/* 1298 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct sgmac1_sgmac_prbs_err_cnt2_s sgmac1_sgmac_prbs_err_cnt2_t;

struct sgmac1_sgmac_prbs_err_cnt3_s	/* 1299 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct sgmac1_sgmac_prbs_err_cnt3_s sgmac1_sgmac_prbs_err_cnt3_t;

struct sgmac1_sgmac8_b10b_err_cnt_s	/* 1300 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct sgmac1_sgmac8_b10b_err_cnt_s sgmac1_sgmac8_b10b_err_cnt_t;

struct sgmac1_sgmac_tp_id_s	/* 1301 */
{

	uint32 rsv_0	:16;
	uint32 sgmac_tp_id	:16;
};
typedef struct sgmac1_sgmac_tp_id_s sgmac1_sgmac_tp_id_t;

struct sgmac1_sgmac_use_orginal_cos_s	/* 1302 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct sgmac1_sgmac_use_orginal_cos_s sgmac1_sgmac_use_orginal_cos_t;

struct sgmac1_sgmac_priority_map_s	/* 1303 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct sgmac1_sgmac_priority_map_s sgmac1_sgmac_priority_map_t;

struct sgmac1_sgmac_gen_pkt_s	/* 1304 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct sgmac1_sgmac_gen_pkt_s sgmac1_sgmac_gen_pkt_t;

struct sgmac1_sgmac_payload_s	/* 1305 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct sgmac1_sgmac_payload_s sgmac1_sgmac_payload_t;

struct sgmac1_sgmac_drain_en_s	/* 1306 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_drain_en	:1;
};
typedef struct sgmac1_sgmac_drain_en_s sgmac1_sgmac_drain_en_t;

struct sgmac1_sgmac_mdio_cmd_status_s	/* 1307 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct sgmac1_sgmac_mdio_cmd_status_s sgmac1_sgmac_mdio_cmd_status_t;

struct sgmac1_sgmac_mdio_cfg_s	/* 1308 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_sgmac_pre	:5;
};
typedef struct sgmac1_sgmac_mdio_cfg_s sgmac1_sgmac_mdio_cfg_t;

struct sgmac1_sgmac_ptp_en_s	/* 1309 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct sgmac1_sgmac_ptp_en_s sgmac1_sgmac_ptp_en_t;

struct sgmac1_sgmac_mode_s	/* 1310 */
{

	uint32 rsv_0	:30;
	uint32 header_mode	:2;
};
typedef struct sgmac1_sgmac_mode_s sgmac1_sgmac_mode_t;

struct sgmac1_sgmac_ct_superg_tx_cfg_s	/* 1311 */
{

	uint32 rsv_0	:29;
	uint32 inter_msg_gap	:3;
};
typedef struct sgmac1_sgmac_ct_superg_tx_cfg_s sgmac1_sgmac_ct_superg_tx_cfg_t;

struct sgmac1_sgmac_ct_superg_rx_cfg_s	/* 1312 */
{

	uint32 fc_msg_type_physical_lvl	:8;
	uint32 rsv_0	:4;
	uint32 fc_msg_fc_obj_physical_lvl	:4;
	uint32 fc_msg_type_logical_lvl	:8;
	uint32 fc_msg_fc_obj_logical_lvl	:4;
	uint32 rsv_1	:3;
	uint32 fc_msg_rx_en	:1;
};
typedef struct sgmac1_sgmac_ct_superg_rx_cfg_s sgmac1_sgmac_ct_superg_rx_cfg_t;

struct sgmac1_sgmac_ct_superg_mon_stats_s	/* 1313 */
{

	uint32 fc_msg_physical_lvl_tx_cnt	:16;
	uint32 fc_msg_physical_lvl_rx_cnt	:16;

	uint32 fc_msg_error_rx_cnt	:16;
	uint32 fc_msg_logical_lvl_rx_cnt	:16;
};
typedef struct sgmac1_sgmac_ct_superg_mon_stats_s sgmac1_sgmac_ct_superg_mon_stats_t;

struct sgmac1_sgmac_ts_status_s	/* 1314 */
{

	uint32 rsv_0	:28;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
};
typedef struct sgmac1_sgmac_ts_status_s sgmac1_sgmac_ts_status_t;

struct sgmac1_sgmac_ctl_s	/* 1315 */
{

	uint32 rsv_0	:1;
	uint32 header_mode	:2;
	uint32 cpu_queue_select_type	:5;
	uint32 rsv_1	:1;
	uint32 sgmac2_dvp_clear_en	:1;
	uint32 queue_sel_type_bits	:2;
	uint32 queue_sel_type_en	:1;
	uint32 sgmac2_bcast_vlan_en	:1;
	uint32 sgmac_lbid_en	:1;
	uint32 dual_mod_en	:1;
	uint32 dvp_met_ptr_base	:16;

	uint32 l3_met_ptr_base	:16;
	uint32 l2_met_ptr_base	:16;
};
typedef struct sgmac1_sgmac_ctl_s sgmac1_sgmac_ctl_t;

struct sgmac2_sgmac_mdio_cmd_s	/* 1316 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct sgmac2_sgmac_mdio_cmd_s sgmac2_sgmac_mdio_cmd_t;

struct sgmac2_sgmac_config1_s	/* 1317 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 sgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct sgmac2_sgmac_config1_s sgmac2_sgmac_config1_t;

struct sgmac2_sgmac_config2_s	/* 1318 */
{

	uint32 rsv_0	:11;
	uint32 buf_fifo_almost_empty_threshold	:5;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 rsv_1	:3;
	uint32 buf_fifo_almost_full_threshold	:5;
};
typedef struct sgmac2_sgmac_config2_s sgmac2_sgmac_config2_t;

struct sgmac2_sgmac_config3_s	/* 1319 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct sgmac2_sgmac_config3_s sgmac2_sgmac_config3_t;

struct sgmac2_sgmac_config4_s	/* 1320 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:1;
	uint32 e2e_crc_bit_swizzle	:1;
	uint32 e2e_msg_error_en	:1;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct sgmac2_sgmac_config4_s sgmac2_sgmac_config4_t;

struct sgmac2_sgmac_soft_rst_s	/* 1321 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct sgmac2_sgmac_soft_rst_s sgmac2_sgmac_soft_rst_t;

struct sgmac2_sgmac_dbg1_s	/* 1322 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct sgmac2_sgmac_dbg1_s sgmac2_sgmac_dbg1_t;

struct sgmac2_sgmac_mdio_rd_data_s	/* 1323 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct sgmac2_sgmac_mdio_rd_data_s sgmac2_sgmac_mdio_rd_data_t;

struct sgmac2_sgmac_stretch_mode_s	/* 1324 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct sgmac2_sgmac_stretch_mode_s sgmac2_sgmac_stretch_mode_t;

struct sgmac2_sgmac_stats_mtu1_s	/* 1325 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct sgmac2_sgmac_stats_mtu1_s sgmac2_sgmac_stats_mtu1_t;

struct sgmac2_sgmac_stats_mtu2_s	/* 1326 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct sgmac2_sgmac_stats_mtu2_s sgmac2_sgmac_stats_mtu2_t;

struct sgmac2_sgmac_stats_config_s	/* 1327 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct sgmac2_sgmac_stats_config_s sgmac2_sgmac_stats_config_t;

struct sgmac2_sgmac_stats_init_s	/* 1328 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac2_sgmac_stats_init_s sgmac2_sgmac_stats_init_t;

struct sgmac2_sgmac_stats_init_done_s	/* 1329 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac2_sgmac_stats_init_done_s sgmac2_sgmac_stats_init_done_t;

struct sgmac2_sgmac_deskew_fifo_config_s	/* 1330 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct sgmac2_sgmac_deskew_fifo_config_s sgmac2_sgmac_deskew_fifo_config_t;

struct sgmac2_sgmac_config5_s	/* 1331 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct sgmac2_sgmac_config5_s sgmac2_sgmac_config5_t;

struct sgmac2_sgmac_config6_s	/* 1332 */
{

	uint32 rsv_0	:16;
	uint32 mac_sa_cfg47_to32	:16;
};
typedef struct sgmac2_sgmac_config6_s sgmac2_sgmac_config6_t;

struct sgmac2_sgmac_interrupt_status_set_s	/* 1333 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct sgmac2_sgmac_interrupt_status_set_s sgmac2_sgmac_interrupt_status_set_t;

struct sgmac2_sgmac_interrupt_status_reset_s	/* 1334 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct sgmac2_sgmac_interrupt_status_reset_s sgmac2_sgmac_interrupt_status_reset_t;

struct sgmac2_sgmac_interrupt_mask_set_s	/* 1335 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct sgmac2_sgmac_interrupt_mask_set_s sgmac2_sgmac_interrupt_mask_set_t;

struct sgmac2_sgmac_interrupt_mask_reset_s	/* 1336 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct sgmac2_sgmac_interrupt_mask_reset_s sgmac2_sgmac_interrupt_mask_reset_t;

struct sgmac2_sgmac_prbs_cfg_s	/* 1337 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct sgmac2_sgmac_prbs_cfg_s sgmac2_sgmac_prbs_cfg_t;

struct sgmac2_sgmac_prbs_err_cnt0_s	/* 1338 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct sgmac2_sgmac_prbs_err_cnt0_s sgmac2_sgmac_prbs_err_cnt0_t;

struct sgmac2_sgmac_prbs_err_cnt1_s	/* 1339 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct sgmac2_sgmac_prbs_err_cnt1_s sgmac2_sgmac_prbs_err_cnt1_t;

struct sgmac2_sgmac_prbs_err_cnt2_s	/* 1340 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct sgmac2_sgmac_prbs_err_cnt2_s sgmac2_sgmac_prbs_err_cnt2_t;

struct sgmac2_sgmac_prbs_err_cnt3_s	/* 1341 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct sgmac2_sgmac_prbs_err_cnt3_s sgmac2_sgmac_prbs_err_cnt3_t;

struct sgmac2_sgmac8_b10b_err_cnt_s	/* 1342 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct sgmac2_sgmac8_b10b_err_cnt_s sgmac2_sgmac8_b10b_err_cnt_t;

struct sgmac2_sgmac_tp_id_s	/* 1343 */
{

	uint32 rsv_0	:16;
	uint32 sgmac_tp_id	:16;
};
typedef struct sgmac2_sgmac_tp_id_s sgmac2_sgmac_tp_id_t;

struct sgmac2_sgmac_use_orginal_cos_s	/* 1344 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct sgmac2_sgmac_use_orginal_cos_s sgmac2_sgmac_use_orginal_cos_t;

struct sgmac2_sgmac_priority_map_s	/* 1345 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct sgmac2_sgmac_priority_map_s sgmac2_sgmac_priority_map_t;

struct sgmac2_sgmac_gen_pkt_s	/* 1346 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct sgmac2_sgmac_gen_pkt_s sgmac2_sgmac_gen_pkt_t;

struct sgmac2_sgmac_payload_s	/* 1347 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct sgmac2_sgmac_payload_s sgmac2_sgmac_payload_t;

struct sgmac2_sgmac_drain_en_s	/* 1348 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_drain_en	:1;
};
typedef struct sgmac2_sgmac_drain_en_s sgmac2_sgmac_drain_en_t;

struct sgmac2_sgmac_mdio_cmd_status_s	/* 1349 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct sgmac2_sgmac_mdio_cmd_status_s sgmac2_sgmac_mdio_cmd_status_t;

struct sgmac2_sgmac_mdio_cfg_s	/* 1350 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_sgmac_pre	:5;
};
typedef struct sgmac2_sgmac_mdio_cfg_s sgmac2_sgmac_mdio_cfg_t;

struct sgmac2_sgmac_ptp_en_s	/* 1351 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct sgmac2_sgmac_ptp_en_s sgmac2_sgmac_ptp_en_t;

struct sgmac2_sgmac_mode_s	/* 1352 */
{

	uint32 rsv_0	:30;
	uint32 header_mode	:2;
};
typedef struct sgmac2_sgmac_mode_s sgmac2_sgmac_mode_t;

struct sgmac2_sgmac_ct_superg_tx_cfg_s	/* 1353 */
{

	uint32 rsv_0	:29;
	uint32 inter_msg_gap	:3;
};
typedef struct sgmac2_sgmac_ct_superg_tx_cfg_s sgmac2_sgmac_ct_superg_tx_cfg_t;

struct sgmac2_sgmac_ct_superg_rx_cfg_s	/* 1354 */
{

	uint32 fc_msg_type_physical_lvl	:8;
	uint32 rsv_0	:4;
	uint32 fc_msg_fc_obj_physical_lvl	:4;
	uint32 fc_msg_type_logical_lvl	:8;
	uint32 fc_msg_fc_obj_logical_lvl	:4;
	uint32 rsv_1	:3;
	uint32 fc_msg_rx_en	:1;
};
typedef struct sgmac2_sgmac_ct_superg_rx_cfg_s sgmac2_sgmac_ct_superg_rx_cfg_t;

struct sgmac2_sgmac_ct_superg_mon_stats_s	/* 1355 */
{

	uint32 fc_msg_physical_lvl_tx_cnt	:16;
	uint32 fc_msg_physical_lvl_rx_cnt	:16;

	uint32 fc_msg_error_rx_cnt	:16;
	uint32 fc_msg_logical_lvl_rx_cnt	:16;
};
typedef struct sgmac2_sgmac_ct_superg_mon_stats_s sgmac2_sgmac_ct_superg_mon_stats_t;

struct sgmac2_sgmac_ts_status_s	/* 1356 */
{

	uint32 rsv_0	:28;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
};
typedef struct sgmac2_sgmac_ts_status_s sgmac2_sgmac_ts_status_t;

struct sgmac2_sgmac_ctl_s	/* 1357 */
{

	uint32 rsv_0	:1;
	uint32 header_mode	:2;
	uint32 cpu_queue_select_type	:5;
	uint32 rsv_1	:1;
	uint32 sgmac2_dvp_clear_en	:1;
	uint32 queue_sel_type_bits	:2;
	uint32 queue_sel_type_en	:1;
	uint32 sgmac2_bcast_vlan_en	:1;
	uint32 sgmac_lbid_en	:1;
	uint32 dual_mod_en	:1;
	uint32 dvp_met_ptr_base	:16;

	uint32 l3_met_ptr_base	:16;
	uint32 l2_met_ptr_base	:16;
};
typedef struct sgmac2_sgmac_ctl_s sgmac2_sgmac_ctl_t;

struct sgmac3_sgmac_mdio_cmd_s	/* 1358 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct sgmac3_sgmac_mdio_cmd_s sgmac3_sgmac_mdio_cmd_t;

struct sgmac3_sgmac_config1_s	/* 1359 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 sgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct sgmac3_sgmac_config1_s sgmac3_sgmac_config1_t;

struct sgmac3_sgmac_config2_s	/* 1360 */
{

	uint32 rsv_0	:11;
	uint32 buf_fifo_almost_empty_threshold	:5;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 rsv_1	:3;
	uint32 buf_fifo_almost_full_threshold	:5;
};
typedef struct sgmac3_sgmac_config2_s sgmac3_sgmac_config2_t;

struct sgmac3_sgmac_config3_s	/* 1361 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct sgmac3_sgmac_config3_s sgmac3_sgmac_config3_t;

struct sgmac3_sgmac_config4_s	/* 1362 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:1;
	uint32 e2e_crc_bit_swizzle	:1;
	uint32 e2e_msg_error_en	:1;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct sgmac3_sgmac_config4_s sgmac3_sgmac_config4_t;

struct sgmac3_sgmac_soft_rst_s	/* 1363 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct sgmac3_sgmac_soft_rst_s sgmac3_sgmac_soft_rst_t;

struct sgmac3_sgmac_dbg1_s	/* 1364 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct sgmac3_sgmac_dbg1_s sgmac3_sgmac_dbg1_t;

struct sgmac3_sgmac_mdio_rd_data_s	/* 1365 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct sgmac3_sgmac_mdio_rd_data_s sgmac3_sgmac_mdio_rd_data_t;

struct sgmac3_sgmac_stretch_mode_s	/* 1366 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct sgmac3_sgmac_stretch_mode_s sgmac3_sgmac_stretch_mode_t;

struct sgmac3_sgmac_stats_mtu1_s	/* 1367 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct sgmac3_sgmac_stats_mtu1_s sgmac3_sgmac_stats_mtu1_t;

struct sgmac3_sgmac_stats_mtu2_s	/* 1368 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct sgmac3_sgmac_stats_mtu2_s sgmac3_sgmac_stats_mtu2_t;

struct sgmac3_sgmac_stats_config_s	/* 1369 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct sgmac3_sgmac_stats_config_s sgmac3_sgmac_stats_config_t;

struct sgmac3_sgmac_stats_init_s	/* 1370 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac3_sgmac_stats_init_s sgmac3_sgmac_stats_init_t;

struct sgmac3_sgmac_stats_init_done_s	/* 1371 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_init	:1;
};
typedef struct sgmac3_sgmac_stats_init_done_s sgmac3_sgmac_stats_init_done_t;

struct sgmac3_sgmac_deskew_fifo_config_s	/* 1372 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct sgmac3_sgmac_deskew_fifo_config_s sgmac3_sgmac_deskew_fifo_config_t;

struct sgmac3_sgmac_config5_s	/* 1373 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct sgmac3_sgmac_config5_s sgmac3_sgmac_config5_t;

struct sgmac3_sgmac_config6_s	/* 1374 */
{

	uint32 rsv_0	:16;
	uint32 mac_sa_cfg47_to32	:16;
};
typedef struct sgmac3_sgmac_config6_s sgmac3_sgmac_config6_t;

struct sgmac3_sgmac_interrupt_status_set_s	/* 1375 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct sgmac3_sgmac_interrupt_status_set_s sgmac3_sgmac_interrupt_status_set_t;

struct sgmac3_sgmac_interrupt_status_reset_s	/* 1376 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct sgmac3_sgmac_interrupt_status_reset_s sgmac3_sgmac_interrupt_status_reset_t;

struct sgmac3_sgmac_interrupt_mask_set_s	/* 1377 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct sgmac3_sgmac_interrupt_mask_set_s sgmac3_sgmac_interrupt_mask_set_t;

struct sgmac3_sgmac_interrupt_mask_reset_s	/* 1378 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct sgmac3_sgmac_interrupt_mask_reset_s sgmac3_sgmac_interrupt_mask_reset_t;

struct sgmac3_sgmac_prbs_cfg_s	/* 1379 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct sgmac3_sgmac_prbs_cfg_s sgmac3_sgmac_prbs_cfg_t;

struct sgmac3_sgmac_prbs_err_cnt0_s	/* 1380 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct sgmac3_sgmac_prbs_err_cnt0_s sgmac3_sgmac_prbs_err_cnt0_t;

struct sgmac3_sgmac_prbs_err_cnt1_s	/* 1381 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct sgmac3_sgmac_prbs_err_cnt1_s sgmac3_sgmac_prbs_err_cnt1_t;

struct sgmac3_sgmac_prbs_err_cnt2_s	/* 1382 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct sgmac3_sgmac_prbs_err_cnt2_s sgmac3_sgmac_prbs_err_cnt2_t;

struct sgmac3_sgmac_prbs_err_cnt3_s	/* 1383 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct sgmac3_sgmac_prbs_err_cnt3_s sgmac3_sgmac_prbs_err_cnt3_t;

struct sgmac3_sgmac8_b10b_err_cnt_s	/* 1384 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct sgmac3_sgmac8_b10b_err_cnt_s sgmac3_sgmac8_b10b_err_cnt_t;

struct sgmac3_sgmac_tp_id_s	/* 1385 */
{

	uint32 rsv_0	:16;
	uint32 sgmac_tp_id	:16;
};
typedef struct sgmac3_sgmac_tp_id_s sgmac3_sgmac_tp_id_t;

struct sgmac3_sgmac_use_orginal_cos_s	/* 1386 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct sgmac3_sgmac_use_orginal_cos_s sgmac3_sgmac_use_orginal_cos_t;

struct sgmac3_sgmac_priority_map_s	/* 1387 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct sgmac3_sgmac_priority_map_s sgmac3_sgmac_priority_map_t;

struct sgmac3_sgmac_gen_pkt_s	/* 1388 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct sgmac3_sgmac_gen_pkt_s sgmac3_sgmac_gen_pkt_t;

struct sgmac3_sgmac_payload_s	/* 1389 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct sgmac3_sgmac_payload_s sgmac3_sgmac_payload_t;

struct sgmac3_sgmac_drain_en_s	/* 1390 */
{

	uint32 rsv_0	:31;
	uint32 sgmac_drain_en	:1;
};
typedef struct sgmac3_sgmac_drain_en_s sgmac3_sgmac_drain_en_t;

struct sgmac3_sgmac_mdio_cmd_status_s	/* 1391 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct sgmac3_sgmac_mdio_cmd_status_s sgmac3_sgmac_mdio_cmd_status_t;

struct sgmac3_sgmac_mdio_cfg_s	/* 1392 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_sgmac_pre	:5;
};
typedef struct sgmac3_sgmac_mdio_cfg_s sgmac3_sgmac_mdio_cfg_t;

struct sgmac3_sgmac_ptp_en_s	/* 1393 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct sgmac3_sgmac_ptp_en_s sgmac3_sgmac_ptp_en_t;

struct sgmac3_sgmac_mode_s	/* 1394 */
{

	uint32 rsv_0	:30;
	uint32 header_mode	:2;
};
typedef struct sgmac3_sgmac_mode_s sgmac3_sgmac_mode_t;

struct sgmac3_sgmac_ct_superg_tx_cfg_s	/* 1395 */
{

	uint32 rsv_0	:29;
	uint32 inter_msg_gap	:3;
};
typedef struct sgmac3_sgmac_ct_superg_tx_cfg_s sgmac3_sgmac_ct_superg_tx_cfg_t;

struct sgmac3_sgmac_ct_superg_rx_cfg_s	/* 1396 */
{

	uint32 fc_msg_type_physical_lvl	:8;
	uint32 rsv_0	:4;
	uint32 fc_msg_fc_obj_physical_lvl	:4;
	uint32 fc_msg_type_logical_lvl	:8;
	uint32 fc_msg_fc_obj_logical_lvl	:4;
	uint32 rsv_1	:3;
	uint32 fc_msg_rx_en	:1;
};
typedef struct sgmac3_sgmac_ct_superg_rx_cfg_s sgmac3_sgmac_ct_superg_rx_cfg_t;

struct sgmac3_sgmac_ct_superg_mon_stats_s	/* 1397 */
{

	uint32 fc_msg_physical_lvl_tx_cnt	:16;
	uint32 fc_msg_physical_lvl_rx_cnt	:16;

	uint32 fc_msg_error_rx_cnt	:16;
	uint32 fc_msg_logical_lvl_rx_cnt	:16;
};
typedef struct sgmac3_sgmac_ct_superg_mon_stats_s sgmac3_sgmac_ct_superg_mon_stats_t;

struct sgmac3_sgmac_ts_status_s	/* 1398 */
{

	uint32 rsv_0	:28;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
};
typedef struct sgmac3_sgmac_ts_status_s sgmac3_sgmac_ts_status_t;

struct sgmac3_sgmac_ctl_s	/* 1399 */
{

	uint32 rsv_0	:1;
	uint32 header_mode	:2;
	uint32 cpu_queue_select_type	:5;
	uint32 rsv_1	:1;
	uint32 sgmac2_dvp_clear_en	:1;
	uint32 queue_sel_type_bits	:2;
	uint32 queue_sel_type_en	:1;
	uint32 sgmac2_bcast_vlan_en	:1;
	uint32 sgmac_lbid_en	:1;
	uint32 dual_mod_en	:1;
	uint32 dvp_met_ptr_base	:16;

	uint32 l3_met_ptr_base	:16;
	uint32 l2_met_ptr_base	:16;
};
typedef struct sgmac3_sgmac_ctl_s sgmac3_sgmac_ctl_t;

struct shared_ds_interrupt_s	/* 1400 */
{

	uint32 rsv_0	:24;
	uint32 value_set	:8;

	uint32 rsv_1	:24;
	uint32 value_reset	:8;

	uint32 rsv_2	:24;
	uint32 mask_set	:8;

	uint32 rsv_3	:24;
	uint32 mask_reset	:8;
};
typedef struct shared_ds_interrupt_s shared_ds_interrupt_t;

struct shared_ds_parity_enable_s	/* 1401 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct shared_ds_parity_enable_s shared_ds_parity_enable_t;

struct shared_ds_threshold_s	/* 1402 */
{

	uint32 rsv_0	:28;
	uint32 if0_req_fifoa_full_thrd	:4;

	uint32 rsv_1	:28;
	uint32 if1_req_fifoa_full_thrd	:4;

	uint32 rsv_2	:28;
	uint32 if2_req_fifoa_full_thrd	:4;

	uint32 rsv_3	:28;
	uint32 if3_req_fifoa_full_thrd	:4;
};
typedef struct shared_ds_threshold_s shared_ds_threshold_t;

struct shared_ds_parity_fail_record_s	/* 1403 */
{

	uint32 rsv_0	:15;
	uint32 ds_vlan_tab_parity_fail	:1;
	uint32 rsv_1	:3;
	uint32 ds_vlan_tab_parity_fail_addr	:13;

	uint32 rsv_2	:15;
	uint32 ds_link_aggr_group_parity_fail	:1;
	uint32 rsv_3	:5;
	uint32 ds_link_aggr_group_parity_fail_addr	:11;
};
typedef struct shared_ds_parity_fail_record_s shared_ds_parity_fail_record_t;

struct shared_ds_init_s	/* 1404 */
{

	uint32 rsv_0	:30;
	uint32 ds_link_aggr_group_init	:1;
	uint32 ds_vlan_tab_init	:1;

	uint32 rsv_1	:30;
	uint32 ds_link_aggr_group_init_done	:1;
	uint32 ds_vlan_tab_init_done	:1;
};
typedef struct shared_ds_init_s shared_ds_init_t;

struct shared_ds_debug_stats_s	/* 1405 */
{

	uint32 rsv_0	:4;
	uint32 to_ipe_ds_vlan_error_cnt	:4;
	uint32 rsv_1	:4;
	uint32 to_ipe_ds_vlan_ack_cnt	:4;
	uint32 rsv_2	:12;
	uint32 fr_ipe_ds_vlan_rd_valid_cnt	:4;

	uint32 rsv_3	:4;
	uint32 to_ipe_ds_vlan_status_error_cnt	:4;
	uint32 rsv_4	:4;
	uint32 to_ipe_ds_vlan_status_ack_cnt	:4;
	uint32 rsv_5	:12;
	uint32 fr_ipe_ds_vlan_status_rd_valid_cnt	:4;

	uint32 rsv_6	:4;
	uint32 to_epe_ds_vlan_error_cnt	:4;
	uint32 rsv_7	:4;
	uint32 to_epe_ds_vlan_ack_cnt	:4;
	uint32 rsv_8	:12;
	uint32 fr_epe_ds_vlan_rd_valid_cnt	:4;

	uint32 rsv_9	:4;
	uint32 to_epe_ds_vlan_status_error_cnt	:4;
	uint32 rsv_10	:4;
	uint32 to_epe_ds_vlan_status_ack_cnt	:4;
	uint32 rsv_11	:12;
	uint32 fr_epe_ds_vlan_status_rd_valid_cnt	:4;

	uint32 rsv_12	:4;
	uint32 to_ipe_ds_link_aggr_error_cnt	:4;
	uint32 rsv_13	:4;
	uint32 to_ipe_ds_link_aggr_valid_cnt	:4;
	uint32 rsv_14	:12;
	uint32 fr_ipe_ds_link_aggr_rd_valid_cnt	:4;

	uint32 rsv_15	:4;
	uint32 to_epe_ds_link_aggr_error_cnt	:4;
	uint32 rsv_16	:4;
	uint32 to_epe_ds_link_aggr_valid_cnt	:4;
	uint32 rsv_17	:12;
	uint32 fr_epe_ds_link_aggr_rd_valid_cnt	:4;
};
typedef struct shared_ds_debug_stats_s shared_ds_debug_stats_t;

struct statistics_satu_addr_s	/* 1406 */
{

	uint32 rsv_0	:12;
	uint32 satu_addr_rd_data	:20;
};
typedef struct statistics_satu_addr_s statistics_satu_addr_t;

struct statistics_ctl_s	/* 1407 */
{

	uint32 rsv_0	:4;
	uint32 cache_clear	:1;
	uint32 saturate_en	:1;
	uint32 stats_hold	:1;
	uint32 clear_on_read	:1;
	uint32 rsv_1	:4;
	uint32 stats_base_ptr	:20;
};
typedef struct statistics_ctl_s statistics_ctl_t;

struct statistics_init_s	/* 1408 */
{

	uint32 rsv_0	:31;
	uint32 cpu_init	:1;
};
typedef struct statistics_init_s statistics_init_t;

struct statistics_init_done_s	/* 1409 */
{

	uint32 rsv_0	:31;
	uint32 cpu_init	:1;
};
typedef struct statistics_init_done_s statistics_init_done_t;

struct statistics_epe_acl_credit_s	/* 1410 */
{

	uint32 rsv_0	:29;
	uint32 epe_acl_credit	:3;
};
typedef struct statistics_epe_acl_credit_s statistics_epe_acl_credit_t;

struct statistics_ipe_fwd_credit_s	/* 1411 */
{

	uint32 rsv_0	:29;
	uint32 ipe_fwd_credit	:3;
};
typedef struct statistics_ipe_fwd_credit_s statistics_ipe_fwd_credit_t;

struct statistics_policing_credit_s	/* 1412 */
{

	uint32 rsv_0	:29;
	uint32 policing_credit	:3;
};
typedef struct statistics_policing_credit_s statistics_policing_credit_t;

struct statisticsq_mgr_credit_s	/* 1413 */
{

	uint32 rsv_0	:29;
	uint32 q_mgr_credit	:3;
};
typedef struct statisticsq_mgr_credit_s statisticsq_mgr_credit_t;

struct statistics_table_qdr_access_s	/* 1414 */
{

	uint32 rsv_0	:2;
	uint32 cpu_req	:1;
	uint32 cpu_read_data_valid	:1;
	uint32 rsv_1	:3;
	uint32 cpu_req_type	:1;
	uint32 rsv_2	:4;
	uint32 cpu_index	:20;
};
typedef struct statistics_table_qdr_access_s statistics_table_qdr_access_t;

struct statistics_table_qdr_wr00_s	/* 1415 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_wr_data0	:4;
};
typedef struct statistics_table_qdr_wr00_s statistics_table_qdr_wr00_t;

struct statistics_table_qdr_wr01_s	/* 1416 */
{

	uint32 table_qdr_wr_data1	:32;
};
typedef struct statistics_table_qdr_wr01_s statistics_table_qdr_wr01_t;

struct statistics_table_qdr_wr02_s	/* 1417 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_wr_data2	:4;
};
typedef struct statistics_table_qdr_wr02_s statistics_table_qdr_wr02_t;

struct statistics_table_qdr_wr03_s	/* 1418 */
{

	uint32 table_qdr_wr_data3	:32;
};
typedef struct statistics_table_qdr_wr03_s statistics_table_qdr_wr03_t;

struct statistics_table_qdr_rd00_s	/* 1419 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_rd_data0	:4;
};
typedef struct statistics_table_qdr_rd00_s statistics_table_qdr_rd00_t;

struct statistics_table_qdr_rd01_s	/* 1420 */
{

	uint32 table_qdr_rd_data1	:32;
};
typedef struct statistics_table_qdr_rd01_s statistics_table_qdr_rd01_t;

struct statistics_table_qdr_rd02_s	/* 1421 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_rd_data2	:4;
};
typedef struct statistics_table_qdr_rd02_s statistics_table_qdr_rd02_t;

struct statistics_table_qdr_rd03_s	/* 1422 */
{

	uint32 table_qdr_rd_data3	:32;
};
typedef struct statistics_table_qdr_rd03_s statistics_table_qdr_rd03_t;

struct statistics_internal_base_ptr_s	/* 1423 */
{

	uint32 rsv_0	:16;
	uint32 internal_base_ptr	:16;
};
typedef struct statistics_internal_base_ptr_s statistics_internal_base_ptr_t;

struct statistics_intr_value_set_s	/* 1424 */
{

	uint32 rsv_0	:6;
	uint32 intr_value_set	:26;
};
typedef struct statistics_intr_value_set_s statistics_intr_value_set_t;

struct statistics_intr_value_reset_s	/* 1425 */
{

	uint32 rsv_0	:6;
	uint32 intr_value_reset	:26;
};
typedef struct statistics_intr_value_reset_s statistics_intr_value_reset_t;

struct statistics_intr_mask_set_s	/* 1426 */
{

	uint32 rsv_0	:6;
	uint32 intr_mask_set	:26;
};
typedef struct statistics_intr_mask_set_s statistics_intr_mask_set_t;

struct statistics_intr_mask_reset_s	/* 1427 */
{

	uint32 rsv_0	:6;
	uint32 intr_mask_reset	:26;
};
typedef struct statistics_intr_mask_reset_s statistics_intr_mask_reset_t;

struct statistics_table_qdr_arb_rd_credit_s	/* 1428 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_arb_rd_credit	:4;
};
typedef struct statistics_table_qdr_arb_rd_credit_s statistics_table_qdr_arb_rd_credit_t;

struct statistics_running_rd_credit_s	/* 1429 */
{

	uint32 rsv_0	:28;
	uint32 running_rd_credit	:4;
};
typedef struct statistics_running_rd_credit_s statistics_running_rd_credit_t;

struct statistics_table_qdr_arb_wr_credit_s	/* 1430 */
{

	uint32 rsv_0	:28;
	uint32 table_qdr_arb_wr_credit	:4;
};
typedef struct statistics_table_qdr_arb_wr_credit_s statistics_table_qdr_arb_wr_credit_t;

struct statistics_running_wr_credit_s	/* 1431 */
{

	uint32 rsv_0	:28;
	uint32 running_wr_credit	:4;
};
typedef struct statistics_running_wr_credit_s statistics_running_wr_credit_t;

struct statistics_epe_drop_count_s	/* 1432 */
{

	uint32 rsv_0	:16;
	uint32 epe_drop_count	:16;
};
typedef struct statistics_epe_drop_count_s statistics_epe_drop_count_t;

struct statistics_ipe_drop_count_s	/* 1433 */
{

	uint32 rsv_0	:16;
	uint32 ipe_drop_count	:16;
};
typedef struct statistics_ipe_drop_count_s statistics_ipe_drop_count_t;

struct statistics_policing_drop_count_s	/* 1434 */
{

	uint32 rsv_0	:16;
	uint32 policing_drop_count	:16;
};
typedef struct statistics_policing_drop_count_s statistics_policing_drop_count_t;

struct statisticsq_mgr_drop_count_s	/* 1435 */
{

	uint32 rsv_0	:16;
	uint32 q_mgr_drop_count	:16;
};
typedef struct statisticsq_mgr_drop_count_s statisticsq_mgr_drop_count_t;

struct statistics_byte_count_threshold_s	/* 1436 */
{

	uint32 rsv_0	:20;
	uint32 byte_count_threshold	:12;
};
typedef struct statistics_byte_count_threshold_s statistics_byte_count_threshold_t;

struct statistics_packet_count_threshold_s	/* 1437 */
{

	uint32 rsv_0	:20;
	uint32 packet_count_threshold	:12;
};
typedef struct statistics_packet_count_threshold_s statistics_packet_count_threshold_t;

struct statistics_threshold_fifo_depth_s	/* 1438 */
{

	uint32 rsv_0	:27;
	uint32 threshold_fifo_depth	:5;
};
typedef struct statistics_threshold_fifo_depth_s statistics_threshold_fifo_depth_t;

struct statistics_fifo_depth_threshold_s	/* 1439 */
{

	uint32 rsv_0	:27;
	uint32 fifo_depth_threshold	:5;
};
typedef struct statistics_fifo_depth_threshold_s statistics_fifo_depth_threshold_t;

struct statistics_intr_value_set_normal_s	/* 1440 */
{

	uint32 rsv_0	:6;
	uint32 intr_value_set	:26;
};
typedef struct statistics_intr_value_set_normal_s statistics_intr_value_set_normal_t;

struct statistics_intr_value_reset_normal_s	/* 1441 */
{

	uint32 rsv_0	:6;
	uint32 intr_value_reset	:26;
};
typedef struct statistics_intr_value_reset_normal_s statistics_intr_value_reset_normal_t;

struct statistics_intr_mask_set_normal_s	/* 1442 */
{

	uint32 rsv_0	:6;
	uint32 intr_mask_set	:26;
};
typedef struct statistics_intr_mask_set_normal_s statistics_intr_mask_set_normal_t;

struct statistics_intr_mask_reset_normal_s	/* 1443 */
{

	uint32 rsv_0	:6;
	uint32 intr_mask_reset	:26;
};
typedef struct statistics_intr_mask_reset_normal_s statistics_intr_mask_reset_normal_t;

struct statistics_stats_epe_base_ptr_s	/* 1444 */
{

	uint32 rsv_0	:12;
	uint32 stats_epe_base_ptr	:20;
};
typedef struct statistics_stats_epe_base_ptr_s statistics_stats_epe_base_ptr_t;

struct statistics_stats_ipe_base_ptr_s	/* 1445 */
{

	uint32 rsv_0	:12;
	uint32 stats_ipe_base_ptr	:20;
};
typedef struct statistics_stats_ipe_base_ptr_s statistics_stats_ipe_base_ptr_t;

struct stp_state_intr_value_set_s	/* 1446 */
{

	uint32 rsv_0	:31;
	uint32 value_set	:1;
};
typedef struct stp_state_intr_value_set_s stp_state_intr_value_set_t;

struct stp_state_intr_value_reset_s	/* 1447 */
{

	uint32 rsv_0	:31;
	uint32 value_reset	:1;
};
typedef struct stp_state_intr_value_reset_s stp_state_intr_value_reset_t;

struct stp_state_intr_mask_set_s	/* 1448 */
{

	uint32 rsv_0	:31;
	uint32 mask_set	:1;
};
typedef struct stp_state_intr_mask_set_s stp_state_intr_mask_set_t;

struct stp_state_intr_mask_reset_s	/* 1449 */
{

	uint32 rsv_0	:31;
	uint32 mask_reset	:1;
};
typedef struct stp_state_intr_mask_reset_s stp_state_intr_mask_reset_t;

struct tb_info_arb_interrupt0_s	/* 1450 */
{

	uint32 value_set0	:32;

	uint32 value_reset0	:32;

	uint32 mask_set0	:32;

	uint32 mask_reset0	:32;
};
typedef struct tb_info_arb_interrupt0_s tb_info_arb_interrupt0_t;

struct tb_info_arb_interrupt1_s	/* 1451 */
{

	uint32 rsv_0	:19;
	uint32 value_set1	:13;

	uint32 rsv_1	:19;
	uint32 value_reset1	:13;

	uint32 rsv_2	:19;
	uint32 mask_set1	:13;

	uint32 rsv_3	:19;
	uint32 mask_reset1	:13;
};
typedef struct tb_info_arb_interrupt1_s tb_info_arb_interrupt1_t;

struct tb_info_arb_int_sram_parity_enable_s	/* 1452 */
{

	uint32 rsv_0	:31;
	uint32 parity_enable	:1;
};
typedef struct tb_info_arb_int_sram_parity_enable_s tb_info_arb_int_sram_parity_enable_t;

struct tb_info_arb_parity_fail_record_s	/* 1453 */
{

	uint32 rsv_0	:15;
	uint32 sram_parity_fail	:1;
	uint32 rsv_1	:2;
	uint32 sram_parity_fail_addr	:14;
};
typedef struct tb_info_arb_parity_fail_record_s tb_info_arb_parity_fail_record_t;

struct tb_info_arb_int_sram_delay_cfg_s	/* 1454 */
{

	uint32 rsv_0	:30;
	uint32 int_sram_delay_cfg	:2;
};
typedef struct tb_info_arb_int_sram_delay_cfg_s tb_info_arb_int_sram_delay_cfg_t;

struct tb_info_arb_threshold_cfg_s	/* 1455 */
{

	uint32 rsv_0	:8;
	uint32 ext_track_threshold	:8;
	uint32 rsv_1	:3;
	uint32 hash_track_threshold	:5;
	uint32 rsv_2	:3;
	uint32 int_track_threshold	:5;
};
typedef struct tb_info_arb_threshold_cfg_s tb_info_arb_threshold_cfg_t;

struct tb_info_arb_wrr_cfg_s	/* 1456 */
{

	uint32 rsv_0	:29;
	uint32 ipe_user_id_credit_cfg	:3;

	uint32 rsv_1	:29;
	uint32 ipe_lookup_mgr_credit_cfg	:3;

	uint32 rsv_2	:29;
	uint32 ipe_forward_credit_cfg	:3;

	uint32 rsv_3	:29;
	uint32 epe_next_hop_credit_cfg	:3;

	uint32 rsv_4	:29;
	uint32 epe_edit_credit_cfg	:3;

	uint32 rsv_5	:29;
	uint32 epe_acl_credit_cfg	:3;

	uint32 rsv_6	:29;
	uint32 met_fifo_credit_cfg	:3;
};
typedef struct tb_info_arb_wrr_cfg_s tb_info_arb_wrr_cfg_t;

struct tb_info_arb_req_fifo_threshold_s	/* 1457 */
{

	uint32 rsv_0	:27;
	uint32 ipe_user_id_req_fifo_threshold	:5;

	uint32 rsv_1	:25;
	uint32 ipe_lookup_mgr_req_fifo_threshold	:7;

	uint32 rsv_2	:27;
	uint32 ipe_forward_req_fifo_threshold	:5;

	uint32 rsv_3	:26;
	uint32 epe_next_hop_req_fifo_threshold	:6;

	uint32 rsv_4	:25;
	uint32 epe_edit_req_fifo_threshold	:7;

	uint32 rsv_5	:26;
	uint32 epe_acl_req_fifo_threshold	:6;

	uint32 rsv_6	:27;
	uint32 met_fifo_req_fifo_threshold	:5;
};
typedef struct tb_info_arb_req_fifo_threshold_s tb_info_arb_req_fifo_threshold_t;

struct tb_info_arb_track_fifo_threshold_s	/* 1458 */
{

	uint32 rsv_0	:11;
	uint32 ipe_forward_track_fifo_threshold	:5;
	uint32 rsv_1	:1;
	uint32 ipe_lookup_mgr_track_fifo_threshold	:7;
	uint32 rsv_2	:3;
	uint32 ipe_user_id_track_fifo_threshold	:5;

	uint32 rsv_3	:3;
	uint32 met_fifo_track_fifo_threshold	:5;
	uint32 rsv_4	:2;
	uint32 epe_acl_track_fifo_threshold	:6;
	uint32 rsv_5	:1;
	uint32 epe_edit_track_fifo_threshold	:7;
	uint32 rsv_6	:2;
	uint32 epe_next_hop_track_fifo_threshold	:6;
};
typedef struct tb_info_arb_track_fifo_threshold_s tb_info_arb_track_fifo_threshold_t;

struct tb_info_arb_intif_debug_stats_s	/* 1459 */
{

	uint32 rsv_0	:4;
	uint32 to_ipe_user_id_error_cnt	:4;
	uint32 rsv_1	:4;
	uint32 to_ipe_user_id_ack_cnt	:4;
	uint32 rsv_2	:12;
	uint32 fr_ipe_user_id_req_cnt	:4;

	uint32 rsv_3	:4;
	uint32 to_ipe_pkt_proc_error_cnt	:4;
	uint32 rsv_4	:4;
	uint32 to_ipe_pkt_proc_ack_cnt	:4;
	uint32 rsv_5	:12;
	uint32 fr_ipe_lookup_mgr_req_cnt	:4;

	uint32 rsv_6	:4;
	uint32 to_ipe_forward_error_cnt	:4;
	uint32 rsv_7	:4;
	uint32 to_ipe_forward_ack_cnt	:4;
	uint32 rsv_8	:12;
	uint32 fr_ipe_forward_req_cnt	:4;

	uint32 rsv_9	:4;
	uint32 to_epe_next_hop_error_cnt	:4;
	uint32 rsv_10	:4;
	uint32 to_epe_next_hop_ack_cnt	:4;
	uint32 rsv_11	:12;
	uint32 fr_epe_next_hop_req_cnt	:4;

	uint32 rsv_12	:4;
	uint32 to_epe_edit_error_cnt	:4;
	uint32 rsv_13	:4;
	uint32 to_epe_edit_ack_cnt	:4;
	uint32 rsv_14	:12;
	uint32 fr_epe_edit_req_cnt	:4;

	uint32 rsv_15	:4;
	uint32 to_epe_acl_error_cnt	:4;
	uint32 rsv_16	:4;
	uint32 to_epe_acl_ack_cnt	:4;
	uint32 rsv_17	:12;
	uint32 fr_epe_acl_req_cnt	:4;

	uint32 rsv_18	:4;
	uint32 to_met_fifo_error_cnt	:4;
	uint32 rsv_19	:4;
	uint32 to_met_fifo_ack_cnt	:4;
	uint32 rsv_20	:12;
	uint32 fr_met_fifo_req_cnt	:4;

	uint32 rsv_21	:4;
	uint32 tocpu_error_cnt	:4;
	uint32 rsv_22	:4;
	uint32 tocpu_ack_cnt	:4;
	uint32 rsv_23	:12;
	uint32 frcpu_req_cnt	:4;
};
typedef struct tb_info_arb_intif_debug_stats_s tb_info_arb_intif_debug_stats_t;

struct tb_info_arb_sramif_debug_stats_s	/* 1460 */
{

	uint32 rsv_0	:4;
	uint32 int_sram_wr_valid_cnt	:4;
	uint32 rsv_1	:4;
	uint32 int_sram_rd_error_cnt	:4;
	uint32 rsv_2	:12;
	uint32 int_sram_rd_valid_cnt	:4;

	uint32 rsv_3	:4;
	uint32 hash_wr_valid_cnt	:4;
	uint32 rsv_4	:4;
	uint32 hash_rd_error_cnt	:4;
	uint32 rsv_5	:4;
	uint32 hash_rd_valid_cnt	:4;
	uint32 rsv_6	:4;
	uint32 hash_rd_request_cnt	:4;

	uint32 rsv_7	:4;
	uint32 ext_ddr_wr_valid_cnt	:4;
	uint32 rsv_8	:4;
	uint32 ext_ddr_rd_error_cnt	:4;
	uint32 rsv_9	:4;
	uint32 ext_ddr_rd_valid_cnt	:4;
	uint32 rsv_10	:4;
	uint32 ext_ddr_rd_request_cnt	:4;
};
typedef struct tb_info_arb_sramif_debug_stats_s tb_info_arb_sramif_debug_stats_t;

struct tb_info_ext_ddr_control_s	/* 1461 */
{

	uint32 rsv_0	:2;
	uint32 cfg_inv_data_valid_bar	:1;
	uint32 rsv_1	:13;
	uint32 cfgwr_turn_around	:2;
	uint32 cfgrw_turn_around	:2;
	uint32 cfg_data_pop_latency	:4;
	uint32 rsv_2	:2;
	uint32 cfg_read_valid_latency	:2;
	uint32 cfg_parity_check_en	:1;
	uint32 cfg_auto_parity	:1;
	uint32 cfg_clock	:1;
	uint32 cfg_clock_en	:1;
};
typedef struct tb_info_ext_ddr_control_s tb_info_ext_ddr_control_t;

struct tb_info_ext_ddr_parity_error_count_s	/* 1462 */
{

	uint32 rsv_0	:24;
	uint32 cfg_parity_error_count	:8;
};
typedef struct tb_info_ext_ddr_parity_error_count_s tb_info_ext_ddr_parity_error_count_t;

struct tb_info_ext_ddr_bist_control_s	/* 1463 */
{

	uint32 cfg_bist_mismatch_count	:16;
	uint32 cfg_capture_en	:1;
	uint32 cfg_capture_once	:1;
	uint32 cfg_bist_en	:1;
	uint32 cfg_bist_once	:1;
	uint32 cfg_bist_read_expect_latency	:4;
	uint32 cfg_stop_on_error	:1;
	uint32 rsv_0	:1;
	uint32 cfg_bist_entries	:6;
};
typedef struct tb_info_ext_ddr_bist_control_s tb_info_ext_ddr_bist_control_t;

struct tb_info_ext_ddr_bist_pointers_s	/* 1464 */
{

	uint32 rsv_0	:10;
	uint32 cfg_bist_expect_read_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_bist_request_done_once	:1;
	uint32 cfg_bist_request_read_ptr	:6;
	uint32 rsv_2	:1;
	uint32 cfg_bist_result_done_once	:1;
	uint32 cfg_bist_result_write_ptr	:6;
};
typedef struct tb_info_ext_ddr_bist_pointers_s tb_info_ext_ddr_bist_pointers_t;

struct tb_info_ext_ddr_capture_result_s	/* 1465 */
{

	uint32 rsv_0	:9;
	uint32 cfg_capture_request_done_once	:1;
	uint32 cfg_capture_request_write_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_capture_result_done_once	:1;
	uint32 cfg_capture_result_write_ptr	:6;
	uint32 rsv_2	:8;
};
typedef struct tb_info_ext_ddr_capture_result_s tb_info_ext_ddr_capture_result_t;

struct tb_info_ext_ddr_adr_match_mask_s	/* 1466 */
{

	uint32 rsv_0	:12;
	uint32 cfg_address_match_mask	:20;
};
typedef struct tb_info_ext_ddr_adr_match_mask_s tb_info_ext_ddr_adr_match_mask_t;

struct tb_info_ext_ddr_adr_match_value_s	/* 1467 */
{

	uint32 rsv_0	:12;
	uint32 cfg_address_match_value	:20;
};
typedef struct tb_info_ext_ddr_adr_match_value_s tb_info_ext_ddr_adr_match_value_t;

struct tb_info_ext_ddr_init_ctl_s	/* 1468 */
{

	uint32 rsv_0	:12;
	uint32 init_start_index	:20;

	uint32 rsv_1	:3;
	uint32 init_done	:1;
	uint32 rsv_2	:3;
	uint32 init_en	:1;
	uint32 rsv_3	:4;
	uint32 init_end_index	:20;
};
typedef struct tb_info_ext_ddr_init_ctl_s tb_info_ext_ddr_init_ctl_t;

struct tb_info_ext_ddr_req_fifo_threshold_s	/* 1469 */
{

	uint32 rsv_0	:27;
	uint32 req_fifo_threshold	:5;
};
typedef struct tb_info_ext_ddr_req_fifo_threshold_s tb_info_ext_ddr_req_fifo_threshold_t;

struct tb_info_ext_ddr_ctl_interrupt_s	/* 1470 */
{

	uint32 rsv_0	:29;
	uint32 value_set	:3;

	uint32 rsv_1	:29;
	uint32 value_reset	:3;

	uint32 rsv_2	:29;
	uint32 mask_set	:3;

	uint32 rsv_3	:29;
	uint32 mask_reset	:3;
};
typedef struct tb_info_ext_ddr_ctl_interrupt_s tb_info_ext_ddr_ctl_interrupt_t;

struct tb_info_ext_ddr_ctl_parity_fail_record_s	/* 1471 */
{

	uint32 rsv_0	:7;
	uint32 ddr_parity_fail	:1;
	uint32 rsv_1	:4;
	uint32 ddr_parity_fail_addr	:20;
};
typedef struct tb_info_ext_ddr_ctl_parity_fail_record_s tb_info_ext_ddr_ctl_parity_fail_record_t;

struct tcam_arb_config_s	/* 1472 */
{

	uint32 rsv_0	:2;
	uint32 key_loc_fifo_oama_full_thrd	:6;
	uint32 rsv_1	:2;
	uint32 key_loc_fifo_epea_full_thrd	:6;
	uint32 rsv_2	:2;
	uint32 key_loc_fifo_ipe_user_ida_full_thrd	:6;
	uint32 rsv_3	:1;
	uint32 key_loc_fifo_ipe_lookup_mgra_full_thrd	:7;

	uint32 rsv_4	:7;
	uint32 result_match_fifo_exta_full_thrd	:9;
	uint32 rsv_5	:9;
	uint32 result_match_fifo_inta_full_thrd	:7;

	uint32 rsv_6	:9;
	uint32 tcam_index_fifo_inta_full_thrd	:7;
	uint32 rsv_7	:9;
	uint32 hash_index_fifo_inta_full_thrd	:7;

	uint32 rsv_8	:11;
	uint32 cfg_tcam_ctl_credit	:5;
	uint32 rsv_9	:12;
	uint32 cfg_hash_intf_credit	:4;
};
typedef struct tcam_arb_config_s tcam_arb_config_t;

struct tcam_arb_ext_index_base_s	/* 1473 */
{

	uint32 rsv_0	:12;
	uint32 ext_index_base	:20;
};
typedef struct tcam_arb_ext_index_base_s tcam_arb_ext_index_base_t;

struct tcam_arb_interrupt_s	/* 1474 */
{

	uint32 value_set	:32;

	uint32 value_reset	:32;

	uint32 mask_set	:32;

	uint32 mask_reset	:32;
};
typedef struct tcam_arb_interrupt_s tcam_arb_interrupt_t;

struct tcam_arb_weight_s	/* 1475 */
{

	uint32 rsv_0	:5;
	uint32 ipe_lookup_mgr_weight	:3;
	uint32 rsv_1	:5;
	uint32 ipe_user_id_weight	:3;
	uint32 rsv_2	:5;
	uint32 epe_weight	:3;
	uint32 rsv_3	:5;
	uint32 oam_weight	:3;
};
typedef struct tcam_arb_weight_s tcam_arb_weight_t;

struct tcam_ctl_ext_setup_s	/* 1476 */
{

	uint32 rsv_0	:2;
	uint32 cfg_tcam_type	:2;
	uint32 rsv_1	:3;
	uint32 cfg_mpmiq_vld	:1;
	uint32 rsv_2	:10;
	uint32 cfg_idt_nsa_com_mod	:1;
	uint32 cfg_idt_fast_bus_mode	:1;
	uint32 cfg_mpmi_read_latency	:4;
	uint32 cfg_write_to_lookup_cycles	:4;
	uint32 cfg_lookup_to_cpu_cycles	:4;
};
typedef struct tcam_ctl_ext_setup_s tcam_ctl_ext_setup_t;

struct tcam_ctl_ext_access_s	/* 1477 */
{

	uint32 cpu_req	:1;
	uint32 cpu_read_data_valid	:1;
	uint32 cpu_dev_id	:2;
	uint32 cpu_req_type	:4;
	uint32 rsv_0	:3;
	uint32 cpu_aging_en	:1;
	uint32 cpu_index	:20;
};
typedef struct tcam_ctl_ext_access_s tcam_ctl_ext_access_t;

struct tcam_ctl_ext_cascade_ctl_s	/* 1478 */
{

	uint32 rsv_0	:24;
	uint32 cfg_lookup_dev_sel	:8;
};
typedef struct tcam_ctl_ext_cascade_ctl_s tcam_ctl_ext_cascade_ctl_t;

struct tcam_ctl_ext_write_data_s	/* 1479 */
{

	uint32 rsv_0	:16;
	uint32 tcam_write_data00	:16;

	uint32 tcam_write_data01	:32;

	uint32 tcam_write_data02	:32;
};
typedef struct tcam_ctl_ext_write_data_s tcam_ctl_ext_write_data_t;

struct tcam_ctl_ext_write_mask_s	/* 1480 */
{

	uint32 rsv_0	:16;
	uint32 tcam_write_data10	:16;

	uint32 tcam_write_data11	:32;

	uint32 tcam_write_data12	:32;
};
typedef struct tcam_ctl_ext_write_mask_s tcam_ctl_ext_write_mask_t;

struct tcam_ctl_ext_read_data_s	/* 1481 */
{

	uint32 rsv_0	:16;
	uint32 tcam_read_data0	:16;

	uint32 tcam_read_data1	:32;

	uint32 tcam_read_data2	:32;
};
typedef struct tcam_ctl_ext_read_data_s tcam_ctl_ext_read_data_t;

struct tcam_ctl_ext_bist_pointers_s	/* 1482 */
{

	uint32 rsv_0	:7;
	uint32 cfg_training_done	:1;
	uint32 rsv_1	:2;
	uint32 cfg_bist_expect_read_ptr	:6;
	uint32 rsv_2	:1;
	uint32 cfg_bist_request_done_once	:1;
	uint32 cfg_bist_request_read_ptr	:6;
	uint32 rsv_3	:1;
	uint32 cfg_bist_result_done_once	:1;
	uint32 cfg_bist_result_write_ptr	:6;
};
typedef struct tcam_ctl_ext_bist_pointers_s tcam_ctl_ext_bist_pointers_t;

struct tcam_ctl_ext_capture_result_s	/* 1483 */
{

	uint32 rsv_0	:9;
	uint32 cfg_capture_request_done_once	:1;
	uint32 cfg_capture_request_write_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_capture_result_done_once	:1;
	uint32 cfg_capture_result_write_ptr	:6;
	uint32 rsv_2	:8;
};
typedef struct tcam_ctl_ext_capture_result_s tcam_ctl_ext_capture_result_t;

struct tcam_ctl_ext_init_ctl_s	/* 1484 */
{

	uint32 rsv_0	:12;
	uint32 cfg_init_start_addr	:20;

	uint32 rsv_1	:3;
	uint32 cfg_init_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_init_done	:1;
	uint32 rsv_3	:4;
	uint32 cfg_init_end_addr	:20;
};
typedef struct tcam_ctl_ext_init_ctl_s tcam_ctl_ext_init_ctl_t;

struct tcam_ctl_ext_debug_s	/* 1485 */
{

	uint32 rsv_0	:28;
	uint32 fr_tcam_arb_lookup_key_cnt	:4;

	uint32 rsv_1	:28;
	uint32 to_tcam_arb_index_cnt	:4;
};
typedef struct tcam_ctl_ext_debug_s tcam_ctl_ext_debug_t;

struct tcam_ctl_ext_intr_s	/* 1486 */
{

	uint32 rsv_0	:28;
	uint32 mask_set	:4;

	uint32 rsv_1	:28;
	uint32 mask_reset	:4;

	uint32 rsv_2	:28;
	uint32 value_set	:4;

	uint32 rsv_3	:28;
	uint32 value_reset	:4;
};
typedef struct tcam_ctl_ext_intr_s tcam_ctl_ext_intr_t;

struct tcam_ctl_ext_bist_ctl_s	/* 1487 */
{

	uint32 cfg_bist_mismatch_count	:16;
	uint32 rsv_0	:2;
	uint32 cfg_training_en	:1;
	uint32 cfg_capture_en	:1;
	uint32 cfg_capture_once	:1;
	uint32 cfg_bist_en	:1;
	uint32 cfg_stop_on_error	:1;
	uint32 cfg_bist_once	:1;
	uint32 rsv_1	:2;
	uint32 cfg_bist_entries	:6;

	uint32 rsv_2	:16;
	uint32 cfg_nop_send_num	:8;
	uint32 rsv_3	:1;
	uint32 cfg_bist_expect_latency	:7;
};
typedef struct tcam_ctl_ext_bist_ctl_s tcam_ctl_ext_bist_ctl_t;

struct tcam_ctl_int_setup_s	/* 1488 */
{

	uint32 rsv_0	:22;
	uint32 cfg_tcam_wrapper_read_dly	:2;
	uint32 rsv_1	:6;
	uint32 cfg_tcam_wrapper_lookup_dly	:2;
};
typedef struct tcam_ctl_int_setup_s tcam_ctl_int_setup_t;

struct tcam_ctl_int_access_s	/* 1489 */
{

	uint32 cpu_req	:1;
	uint32 cpu_read_data_valid	:1;
	uint32 rsv_0	:2;
	uint32 cpu_req_type	:4;
	uint32 rsv_1	:10;
	uint32 cpu_index	:14;
};
typedef struct tcam_ctl_int_access_s tcam_ctl_int_access_t;

struct tcam_ctl_int_cpu_rd_data_s	/* 1490 */
{

	uint32 rsv_0	:16;
	uint32 tcam_read_data0	:16;

	uint32 tcam_read_data1	:32;

	uint32 tcam_read_data2	:32;
};
typedef struct tcam_ctl_int_cpu_rd_data_s tcam_ctl_int_cpu_rd_data_t;

struct tcam_ctl_int_bist_ctl_s	/* 1491 */
{

	uint32 cfg_bist_mismatch_count	:16;
	uint32 rsv_0	:3;
	uint32 cfg_capture_en	:1;
	uint32 cfg_capture_once	:1;
	uint32 cfg_bist_en	:1;
	uint32 cfg_stop_on_error	:1;
	uint32 cfg_bist_once	:1;
	uint32 rsv_1	:2;
	uint32 cfg_bist_entries	:6;
};
typedef struct tcam_ctl_int_bist_ctl_s tcam_ctl_int_bist_ctl_t;

struct tcam_ctl_int_bist_pointers_s	/* 1492 */
{

	uint32 rsv_0	:10;
	uint32 cfg_bist_expect_read_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_bist_request_done_once	:1;
	uint32 cfg_bist_request_read_ptr	:6;
	uint32 rsv_2	:1;
	uint32 cfg_bist_result_done_once	:1;
	uint32 cfg_bist_result_write_ptr	:6;
};
typedef struct tcam_ctl_int_bist_pointers_s tcam_ctl_int_bist_pointers_t;

struct tcam_ctl_int_capture_result_s	/* 1493 */
{

	uint32 rsv_0	:9;
	uint32 cfg_capture_request_done_once	:1;
	uint32 cfg_capture_request_write_ptr	:6;
	uint32 rsv_1	:1;
	uint32 cfg_capture_result_done_once	:1;
	uint32 cfg_capture_result_write_ptr	:6;
	uint32 rsv_2	:8;
};
typedef struct tcam_ctl_int_capture_result_s tcam_ctl_int_capture_result_t;

struct tcam_ctl_int_init_ctrl_s	/* 1494 */
{

	uint32 rsv_0	:18;
	uint32 cfg_init_start_addr	:14;

	uint32 rsv_1	:3;
	uint32 cfg_init_en	:1;
	uint32 rsv_2	:3;
	uint32 cfg_init_done	:1;
	uint32 rsv_3	:10;
	uint32 cfg_init_end_addr	:14;
};
typedef struct tcam_ctl_int_init_ctrl_s tcam_ctl_int_init_ctrl_t;

struct tcam_ctl_int_debug_s	/* 1495 */
{

	uint32 rsv_0	:28;
	uint32 fr_tcam_arb_lookup_key_cnt	:4;

	uint32 rsv_1	:28;
	uint32 to_tcam_arb_index_cnt	:4;
};
typedef struct tcam_ctl_int_debug_s tcam_ctl_int_debug_t;

struct tcam_ctl_int_intr_s	/* 1496 */
{

	uint32 rsv_0	:27;
	uint32 mask_set	:5;

	uint32 rsv_1	:27;
	uint32 mask_reset	:5;

	uint32 rsv_2	:27;
	uint32 value_set	:5;

	uint32 rsv_3	:27;
	uint32 value_reset	:5;
};
typedef struct tcam_ctl_int_intr_s tcam_ctl_int_intr_t;

struct tcam_ctl_int_state_s	/* 1497 */
{

	uint32 rsv_0	:29;
	uint32 intf_state	:3;
};
typedef struct tcam_ctl_int_state_s tcam_ctl_int_state_t;

struct tcam_ctl_int_cpu_wr_data_s	/* 1498 */
{

	uint32 rsv_0	:16;
	uint32 tcam_write_data00	:16;

	uint32 tcam_write_data01	:32;

	uint32 tcam_write_data02	:32;
};
typedef struct tcam_ctl_int_cpu_wr_data_s tcam_ctl_int_cpu_wr_data_t;

struct tcam_ctl_int_cpu_wr_mask_s	/* 1499 */
{

	uint32 rsv_0	:16;
	uint32 tcam_write_data10	:16;

	uint32 tcam_write_data11	:32;

	uint32 tcam_write_data12	:32;
};
typedef struct tcam_ctl_int_cpu_wr_mask_s tcam_ctl_int_cpu_wr_mask_t;

struct tcam_ctl_int_misc_ctrl_s	/* 1500 */
{

	uint32 rsv_0	:12;
	uint32 index_fifo_full_threshold	:4;
	uint32 rsv_1	:3;
	uint32 intf_req_fifo_full_threshold	:5;
	uint32 rsv_2	:8;

	uint32 rsv_3	:28;
	uint32 cfg_intf_credit	:4;
};
typedef struct tcam_ctl_int_misc_ctrl_s tcam_ctl_int_misc_ctrl_t;

struct tcam_ctl_int_key_size_cfg_s	/* 1501 */
{

	uint32 rsv_0	:16;
	uint32 key80_en	:16;

	uint32 rsv_1	:24;
	uint32 key160_en	:8;

	uint32 rsv_2	:28;
	uint32 key320_en	:4;
};
typedef struct tcam_ctl_int_key_size_cfg_s tcam_ctl_int_key_size_cfg_t;

struct tcam_ctl_int_key_type_cfg_s	/* 1502 */
{

	uint32 rsv_0	:24;
	uint32 qos_key_en	:8;
};
typedef struct tcam_ctl_int_key_type_cfg_s tcam_ctl_int_key_type_cfg_t;

struct xgmac0_xgmac_mdio_cmd_s	/* 1503 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct xgmac0_xgmac_mdio_cmd_s xgmac0_xgmac_mdio_cmd_t;

struct xgmac0_xgmac_config1_s	/* 1504 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 xgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct xgmac0_xgmac_config1_s xgmac0_xgmac_config1_t;

struct xgmac0_xgmac_config2_s	/* 1505 */
{

	uint32 mac_sa_cfg47_to32	:16;
	uint32 buf_fifo_almost_empty_threshold	:4;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 buf_fifo_almost_full_threshold	:4;
};
typedef struct xgmac0_xgmac_config2_s xgmac0_xgmac_config2_t;

struct xgmac0_xgmac_config3_s	/* 1506 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct xgmac0_xgmac_config3_s xgmac0_xgmac_config3_t;

struct xgmac0_xgmac_config4_s	/* 1507 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:3;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct xgmac0_xgmac_config4_s xgmac0_xgmac_config4_t;

struct xgmac0_xgmac_soft_rst_s	/* 1508 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct xgmac0_xgmac_soft_rst_s xgmac0_xgmac_soft_rst_t;

struct xgmac0_xgmac_dbg1_s	/* 1509 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct xgmac0_xgmac_dbg1_s xgmac0_xgmac_dbg1_t;

struct xgmac0_xgmac_mdio_rd_data_s	/* 1510 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct xgmac0_xgmac_mdio_rd_data_s xgmac0_xgmac_mdio_rd_data_t;

struct xgmac0_xgmac_stretch_mode_s	/* 1511 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct xgmac0_xgmac_stretch_mode_s xgmac0_xgmac_stretch_mode_t;

struct xgmac0_xgmac_stats_mtu1_s	/* 1512 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct xgmac0_xgmac_stats_mtu1_s xgmac0_xgmac_stats_mtu1_t;

struct xgmac0_xgmac_stats_mtu2_s	/* 1513 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct xgmac0_xgmac_stats_mtu2_s xgmac0_xgmac_stats_mtu2_t;

struct xgmac0_xgmac_stats_config_s	/* 1514 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct xgmac0_xgmac_stats_config_s xgmac0_xgmac_stats_config_t;

struct xgmac0_xgmac_stats_init_s	/* 1515 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac0_xgmac_stats_init_s xgmac0_xgmac_stats_init_t;

struct xgmac0_xgmac_stats_init_done_s	/* 1516 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac0_xgmac_stats_init_done_s xgmac0_xgmac_stats_init_done_t;

struct xgmac0_xgmac_deskew_fifo_config_s	/* 1517 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct xgmac0_xgmac_deskew_fifo_config_s xgmac0_xgmac_deskew_fifo_config_t;

struct xgmac0_xgmac_config5_s	/* 1518 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct xgmac0_xgmac_config5_s xgmac0_xgmac_config5_t;

struct xgmac0_xgmac_interrupt_status_set_s	/* 1519 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct xgmac0_xgmac_interrupt_status_set_s xgmac0_xgmac_interrupt_status_set_t;

struct xgmac0_xgmac_interrupt_status_reset_s	/* 1520 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct xgmac0_xgmac_interrupt_status_reset_s xgmac0_xgmac_interrupt_status_reset_t;

struct xgmac0_xgmac_interrupt_mask_set_s	/* 1521 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct xgmac0_xgmac_interrupt_mask_set_s xgmac0_xgmac_interrupt_mask_set_t;

struct xgmac0_xgmac_interrupt_mask_reset_s	/* 1522 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct xgmac0_xgmac_interrupt_mask_reset_s xgmac0_xgmac_interrupt_mask_reset_t;

struct xgmac0_xgmac_prbs_cfg_s	/* 1523 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct xgmac0_xgmac_prbs_cfg_s xgmac0_xgmac_prbs_cfg_t;

struct xgmac0_xgmac_prbs_err_cnt0_s	/* 1524 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct xgmac0_xgmac_prbs_err_cnt0_s xgmac0_xgmac_prbs_err_cnt0_t;

struct xgmac0_xgmac_prbs_err_cnt1_s	/* 1525 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct xgmac0_xgmac_prbs_err_cnt1_s xgmac0_xgmac_prbs_err_cnt1_t;

struct xgmac0_xgmac_prbs_err_cnt2_s	/* 1526 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct xgmac0_xgmac_prbs_err_cnt2_s xgmac0_xgmac_prbs_err_cnt2_t;

struct xgmac0_xgmac_prbs_err_cnt3_s	/* 1527 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct xgmac0_xgmac_prbs_err_cnt3_s xgmac0_xgmac_prbs_err_cnt3_t;

struct xgmac0_xgmac8_b10b_err_cnt_s	/* 1528 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct xgmac0_xgmac8_b10b_err_cnt_s xgmac0_xgmac8_b10b_err_cnt_t;

struct xgmac0_xgmac_tp_id_s	/* 1529 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_tp_id	:16;
};
typedef struct xgmac0_xgmac_tp_id_s xgmac0_xgmac_tp_id_t;

struct xgmac0_xgmac_use_orginal_cos_s	/* 1530 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct xgmac0_xgmac_use_orginal_cos_s xgmac0_xgmac_use_orginal_cos_t;

struct xgmac0_xgmac_priority_map_s	/* 1531 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct xgmac0_xgmac_priority_map_s xgmac0_xgmac_priority_map_t;

struct xgmac0_xgmac_gen_pkt_s	/* 1532 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct xgmac0_xgmac_gen_pkt_s xgmac0_xgmac_gen_pkt_t;

struct xgmac0_xgmac_payload_s	/* 1533 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct xgmac0_xgmac_payload_s xgmac0_xgmac_payload_t;

struct xgmac0_xgmac_drain_en_s	/* 1534 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_drain_en	:1;
};
typedef struct xgmac0_xgmac_drain_en_s xgmac0_xgmac_drain_en_t;

struct xgmac0_xgmac_mdio_cmd_status_s	/* 1535 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct xgmac0_xgmac_mdio_cmd_status_s xgmac0_xgmac_mdio_cmd_status_t;

struct xgmac0_xgmac_mdio_cfg_s	/* 1536 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_xgmac_pre	:5;
};
typedef struct xgmac0_xgmac_mdio_cfg_s xgmac0_xgmac_mdio_cfg_t;

struct xgmac0_xgmac_ptp_en_s	/* 1537 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct xgmac0_xgmac_ptp_en_s xgmac0_xgmac_ptp_en_t;

struct xgmac0_xgmac_ptp_status_s	/* 1538 */
{

	uint32 rsv_0	:28;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
};
typedef struct xgmac0_xgmac_ptp_status_s xgmac0_xgmac_ptp_status_t;

struct xgmac1_xgmac_mdio_cmd_s	/* 1539 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct xgmac1_xgmac_mdio_cmd_s xgmac1_xgmac_mdio_cmd_t;

struct xgmac1_xgmac_config1_s	/* 1540 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 xgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct xgmac1_xgmac_config1_s xgmac1_xgmac_config1_t;

struct xgmac1_xgmac_config2_s	/* 1541 */
{

	uint32 mac_sa_cfg47_to32	:16;
	uint32 buf_fifo_almost_empty_threshold	:4;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 buf_fifo_almost_full_threshold	:4;
};
typedef struct xgmac1_xgmac_config2_s xgmac1_xgmac_config2_t;

struct xgmac1_xgmac_config3_s	/* 1542 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct xgmac1_xgmac_config3_s xgmac1_xgmac_config3_t;

struct xgmac1_xgmac_config4_s	/* 1543 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:3;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct xgmac1_xgmac_config4_s xgmac1_xgmac_config4_t;

struct xgmac1_xgmac_soft_rst_s	/* 1544 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct xgmac1_xgmac_soft_rst_s xgmac1_xgmac_soft_rst_t;

struct xgmac1_xgmac_dbg1_s	/* 1545 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct xgmac1_xgmac_dbg1_s xgmac1_xgmac_dbg1_t;

struct xgmac1_xgmac_mdio_rd_data_s	/* 1546 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct xgmac1_xgmac_mdio_rd_data_s xgmac1_xgmac_mdio_rd_data_t;

struct xgmac1_xgmac_stretch_mode_s	/* 1547 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct xgmac1_xgmac_stretch_mode_s xgmac1_xgmac_stretch_mode_t;

struct xgmac1_xgmac_stats_mtu1_s	/* 1548 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct xgmac1_xgmac_stats_mtu1_s xgmac1_xgmac_stats_mtu1_t;

struct xgmac1_xgmac_stats_mtu2_s	/* 1549 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct xgmac1_xgmac_stats_mtu2_s xgmac1_xgmac_stats_mtu2_t;

struct xgmac1_xgmac_stats_config_s	/* 1550 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct xgmac1_xgmac_stats_config_s xgmac1_xgmac_stats_config_t;

struct xgmac1_xgmac_stats_init_s	/* 1551 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac1_xgmac_stats_init_s xgmac1_xgmac_stats_init_t;

struct xgmac1_xgmac_stats_init_done_s	/* 1552 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac1_xgmac_stats_init_done_s xgmac1_xgmac_stats_init_done_t;

struct xgmac1_xgmac_deskew_fifo_config_s	/* 1553 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct xgmac1_xgmac_deskew_fifo_config_s xgmac1_xgmac_deskew_fifo_config_t;

struct xgmac1_xgmac_config5_s	/* 1554 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct xgmac1_xgmac_config5_s xgmac1_xgmac_config5_t;

struct xgmac1_xgmac_interrupt_status_set_s	/* 1555 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct xgmac1_xgmac_interrupt_status_set_s xgmac1_xgmac_interrupt_status_set_t;

struct xgmac1_xgmac_interrupt_status_reset_s	/* 1556 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct xgmac1_xgmac_interrupt_status_reset_s xgmac1_xgmac_interrupt_status_reset_t;

struct xgmac1_xgmac_interrupt_mask_set_s	/* 1557 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct xgmac1_xgmac_interrupt_mask_set_s xgmac1_xgmac_interrupt_mask_set_t;

struct xgmac1_xgmac_interrupt_mask_reset_s	/* 1558 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct xgmac1_xgmac_interrupt_mask_reset_s xgmac1_xgmac_interrupt_mask_reset_t;

struct xgmac1_xgmac_prbs_cfg_s	/* 1559 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct xgmac1_xgmac_prbs_cfg_s xgmac1_xgmac_prbs_cfg_t;

struct xgmac1_xgmac_prbs_err_cnt0_s	/* 1560 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct xgmac1_xgmac_prbs_err_cnt0_s xgmac1_xgmac_prbs_err_cnt0_t;

struct xgmac1_xgmac_prbs_err_cnt1_s	/* 1561 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct xgmac1_xgmac_prbs_err_cnt1_s xgmac1_xgmac_prbs_err_cnt1_t;

struct xgmac1_xgmac_prbs_err_cnt2_s	/* 1562 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct xgmac1_xgmac_prbs_err_cnt2_s xgmac1_xgmac_prbs_err_cnt2_t;

struct xgmac1_xgmac_prbs_err_cnt3_s	/* 1563 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct xgmac1_xgmac_prbs_err_cnt3_s xgmac1_xgmac_prbs_err_cnt3_t;

struct xgmac1_xgmac8_b10b_err_cnt_s	/* 1564 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct xgmac1_xgmac8_b10b_err_cnt_s xgmac1_xgmac8_b10b_err_cnt_t;

struct xgmac1_xgmac_tp_id_s	/* 1565 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_tp_id	:16;
};
typedef struct xgmac1_xgmac_tp_id_s xgmac1_xgmac_tp_id_t;

struct xgmac1_xgmac_use_orginal_cos_s	/* 1566 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct xgmac1_xgmac_use_orginal_cos_s xgmac1_xgmac_use_orginal_cos_t;

struct xgmac1_xgmac_priority_map_s	/* 1567 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct xgmac1_xgmac_priority_map_s xgmac1_xgmac_priority_map_t;

struct xgmac1_xgmac_gen_pkt_s	/* 1568 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct xgmac1_xgmac_gen_pkt_s xgmac1_xgmac_gen_pkt_t;

struct xgmac1_xgmac_payload_s	/* 1569 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct xgmac1_xgmac_payload_s xgmac1_xgmac_payload_t;

struct xgmac1_xgmac_drain_en_s	/* 1570 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_drain_en	:1;
};
typedef struct xgmac1_xgmac_drain_en_s xgmac1_xgmac_drain_en_t;

struct xgmac1_xgmac_mdio_cmd_status_s	/* 1571 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct xgmac1_xgmac_mdio_cmd_status_s xgmac1_xgmac_mdio_cmd_status_t;

struct xgmac1_xgmac_mdio_cfg_s	/* 1572 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_xgmac_pre	:5;
};
typedef struct xgmac1_xgmac_mdio_cfg_s xgmac1_xgmac_mdio_cfg_t;

struct xgmac1_xgmac_ptp_en_s	/* 1573 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct xgmac1_xgmac_ptp_en_s xgmac1_xgmac_ptp_en_t;

struct xgmac1_xgmac_ptp_status_s	/* 1574 */
{

	uint32 rsv_0	:28;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
};
typedef struct xgmac1_xgmac_ptp_status_s xgmac1_xgmac_ptp_status_t;

struct xgmac2_xgmac_mdio_cmd_s	/* 1575 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct xgmac2_xgmac_mdio_cmd_s xgmac2_xgmac_mdio_cmd_t;

struct xgmac2_xgmac_config1_s	/* 1576 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 xgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct xgmac2_xgmac_config1_s xgmac2_xgmac_config1_t;

struct xgmac2_xgmac_config2_s	/* 1577 */
{

	uint32 mac_sa_cfg47_to32	:16;
	uint32 buf_fifo_almost_empty_threshold	:4;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 buf_fifo_almost_full_threshold	:4;
};
typedef struct xgmac2_xgmac_config2_s xgmac2_xgmac_config2_t;

struct xgmac2_xgmac_config3_s	/* 1578 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct xgmac2_xgmac_config3_s xgmac2_xgmac_config3_t;

struct xgmac2_xgmac_config4_s	/* 1579 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:3;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct xgmac2_xgmac_config4_s xgmac2_xgmac_config4_t;

struct xgmac2_xgmac_soft_rst_s	/* 1580 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct xgmac2_xgmac_soft_rst_s xgmac2_xgmac_soft_rst_t;

struct xgmac2_xgmac_dbg1_s	/* 1581 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct xgmac2_xgmac_dbg1_s xgmac2_xgmac_dbg1_t;

struct xgmac2_xgmac_mdio_rd_data_s	/* 1582 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct xgmac2_xgmac_mdio_rd_data_s xgmac2_xgmac_mdio_rd_data_t;

struct xgmac2_xgmac_stretch_mode_s	/* 1583 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct xgmac2_xgmac_stretch_mode_s xgmac2_xgmac_stretch_mode_t;

struct xgmac2_xgmac_stats_mtu1_s	/* 1584 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct xgmac2_xgmac_stats_mtu1_s xgmac2_xgmac_stats_mtu1_t;

struct xgmac2_xgmac_stats_mtu2_s	/* 1585 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct xgmac2_xgmac_stats_mtu2_s xgmac2_xgmac_stats_mtu2_t;

struct xgmac2_xgmac_stats_config_s	/* 1586 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct xgmac2_xgmac_stats_config_s xgmac2_xgmac_stats_config_t;

struct xgmac2_xgmac_stats_init_s	/* 1587 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac2_xgmac_stats_init_s xgmac2_xgmac_stats_init_t;

struct xgmac2_xgmac_stats_init_done_s	/* 1588 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac2_xgmac_stats_init_done_s xgmac2_xgmac_stats_init_done_t;

struct xgmac2_xgmac_deskew_fifo_config_s	/* 1589 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct xgmac2_xgmac_deskew_fifo_config_s xgmac2_xgmac_deskew_fifo_config_t;

struct xgmac2_xgmac_config5_s	/* 1590 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct xgmac2_xgmac_config5_s xgmac2_xgmac_config5_t;

struct xgmac2_xgmac_interrupt_status_set_s	/* 1591 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct xgmac2_xgmac_interrupt_status_set_s xgmac2_xgmac_interrupt_status_set_t;

struct xgmac2_xgmac_interrupt_status_reset_s	/* 1592 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct xgmac2_xgmac_interrupt_status_reset_s xgmac2_xgmac_interrupt_status_reset_t;

struct xgmac2_xgmac_interrupt_mask_set_s	/* 1593 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct xgmac2_xgmac_interrupt_mask_set_s xgmac2_xgmac_interrupt_mask_set_t;

struct xgmac2_xgmac_interrupt_mask_reset_s	/* 1594 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct xgmac2_xgmac_interrupt_mask_reset_s xgmac2_xgmac_interrupt_mask_reset_t;

struct xgmac2_xgmac_prbs_cfg_s	/* 1595 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct xgmac2_xgmac_prbs_cfg_s xgmac2_xgmac_prbs_cfg_t;

struct xgmac2_xgmac_prbs_err_cnt0_s	/* 1596 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct xgmac2_xgmac_prbs_err_cnt0_s xgmac2_xgmac_prbs_err_cnt0_t;

struct xgmac2_xgmac_prbs_err_cnt1_s	/* 1597 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct xgmac2_xgmac_prbs_err_cnt1_s xgmac2_xgmac_prbs_err_cnt1_t;

struct xgmac2_xgmac_prbs_err_cnt2_s	/* 1598 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct xgmac2_xgmac_prbs_err_cnt2_s xgmac2_xgmac_prbs_err_cnt2_t;

struct xgmac2_xgmac_prbs_err_cnt3_s	/* 1599 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct xgmac2_xgmac_prbs_err_cnt3_s xgmac2_xgmac_prbs_err_cnt3_t;

struct xgmac2_xgmac8_b10b_err_cnt_s	/* 1600 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct xgmac2_xgmac8_b10b_err_cnt_s xgmac2_xgmac8_b10b_err_cnt_t;

struct xgmac2_xgmac_tp_id_s	/* 1601 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_tp_id	:16;
};
typedef struct xgmac2_xgmac_tp_id_s xgmac2_xgmac_tp_id_t;

struct xgmac2_xgmac_use_orginal_cos_s	/* 1602 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct xgmac2_xgmac_use_orginal_cos_s xgmac2_xgmac_use_orginal_cos_t;

struct xgmac2_xgmac_priority_map_s	/* 1603 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct xgmac2_xgmac_priority_map_s xgmac2_xgmac_priority_map_t;

struct xgmac2_xgmac_gen_pkt_s	/* 1604 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct xgmac2_xgmac_gen_pkt_s xgmac2_xgmac_gen_pkt_t;

struct xgmac2_xgmac_payload_s	/* 1605 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct xgmac2_xgmac_payload_s xgmac2_xgmac_payload_t;

struct xgmac2_xgmac_drain_en_s	/* 1606 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_drain_en	:1;
};
typedef struct xgmac2_xgmac_drain_en_s xgmac2_xgmac_drain_en_t;

struct xgmac2_xgmac_mdio_cmd_status_s	/* 1607 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct xgmac2_xgmac_mdio_cmd_status_s xgmac2_xgmac_mdio_cmd_status_t;

struct xgmac2_xgmac_mdio_cfg_s	/* 1608 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_xgmac_pre	:5;
};
typedef struct xgmac2_xgmac_mdio_cfg_s xgmac2_xgmac_mdio_cfg_t;

struct xgmac2_xgmac_ptp_en_s	/* 1609 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct xgmac2_xgmac_ptp_en_s xgmac2_xgmac_ptp_en_t;

struct xgmac2_xgmac_ptp_status_s	/* 1610 */
{

	uint32 rsv_0	:28;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
};
typedef struct xgmac2_xgmac_ptp_status_s xgmac2_xgmac_ptp_status_t;

struct xgmac3_xgmac_mdio_cmd_s	/* 1611 */
{

	uint32 rsv_0	:2;
	uint32 st	:2;
	uint32 op	:2;
	uint32 prtad	:5;
	uint32 devad	:5;
	uint32 data	:16;
};
typedef struct xgmac3_xgmac_mdio_cmd_s xgmac3_xgmac_mdio_cmd_t;

struct xgmac3_xgmac_config1_s	/* 1612 */
{

	uint32 pause_quanta_cfg	:16;
	uint32 channel_cfg	:8;
	uint32 xgmac_loopback_en	:1;
	uint32 preamble4_bytes	:1;
	uint32 pause_frame_enable	:1;
	uint32 dic_cnt_enable	:1;
	uint32 bit_polarity_invert	:1;
	uint32 bit_order_invert	:1;
	uint32 tx_enable	:1;
	uint32 rx_enable	:1;
};
typedef struct xgmac3_xgmac_config1_s xgmac3_xgmac_config1_t;

struct xgmac3_xgmac_config2_s	/* 1613 */
{

	uint32 mac_sa_cfg47_to32	:16;
	uint32 buf_fifo_almost_empty_threshold	:4;
	uint32 insert_threshold	:4;
	uint32 delete_threshold	:4;
	uint32 buf_fifo_almost_full_threshold	:4;
};
typedef struct xgmac3_xgmac_config2_s xgmac3_xgmac_config2_t;

struct xgmac3_xgmac_config3_s	/* 1614 */
{

	uint32 mac_sa_cfg31_to0	:32;
};
typedef struct xgmac3_xgmac_config3_s xgmac3_xgmac_config3_t;

struct xgmac3_xgmac_config4_s	/* 1615 */
{

	uint32 mux_port_enable	:1;
	uint32 ignore_remote_fault	:1;
	uint32 ignore_local_fault	:1;
	uint32 pad_enable	:1;
	uint32 force_realign	:1;
	uint32 force_sync	:1;
	uint32 crc_enable	:1;
	uint32 force_signal_detect	:1;
	uint32 pause_off_enable	:1;
	uint32 sig_det_active_value	:1;
	uint32 serdes_rx_byte_swap	:1;
	uint32 serdes_tx_byte_swap	:1;
	uint32 rsv_0	:3;
	uint32 pause_error_mask_off	:1;
	uint32 rsv_1	:2;
	uint32 tx_threshold	:6;
	uint32 rsv_2	:1;
	uint32 crc_error_mask	:1;
	uint32 full_threshold	:6;
};
typedef struct xgmac3_xgmac_config4_s xgmac3_xgmac_config4_t;

struct xgmac3_xgmac_soft_rst_s	/* 1616 */
{

	uint32 rsv_0	:24;
	uint32 serdes_rx3_soft_rst	:1;
	uint32 serdes_rx2_soft_rst	:1;
	uint32 serdes_rx1_soft_rst	:1;
	uint32 serdes_rx0_soft_rst	:1;
	uint32 rsv_1	:1;
	uint32 mdio_soft_rst	:1;
	uint32 pcs_rx_soft_rst	:1;
	uint32 pcs_tx_soft_rst	:1;
};
typedef struct xgmac3_xgmac_soft_rst_s xgmac3_xgmac_soft_rst_t;

struct xgmac3_xgmac_dbg1_s	/* 1617 */
{

	uint32 stats_ram_parity_error_addr	:8;
	uint32 pkt_tx_state	:2;
	uint32 pause_state	:1;
	uint32 align_status	:1;
	uint32 sync_status	:4;
	uint32 rsv_0	:2;
	uint32 xgmii_tx_state	:2;
	uint32 xgmii_rx_state	:2;
	uint32 rsv_1	:1;
	uint32 pcs_tx_state	:3;
	uint32 link_fault_state	:2;
	uint32 pkt_with_no_sop	:1;
	uint32 rsv_2	:1;
	uint32 link_fault_type	:2;
};
typedef struct xgmac3_xgmac_dbg1_s xgmac3_xgmac_dbg1_t;

struct xgmac3_xgmac_mdio_rd_data_s	/* 1618 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_mdio_rd_data	:16;
};
typedef struct xgmac3_xgmac_mdio_rd_data_s xgmac3_xgmac_mdio_rd_data_t;

struct xgmac3_xgmac_stretch_mode_s	/* 1619 */
{

	uint32 rsv_0	:16;
	uint32 ifs_stretch_count_init	:4;
	uint32 ifs_stretch_size_init	:4;
	uint32 ifs_stretch_ratio	:4;
	uint32 rsv_1	:3;
	uint32 ifs_stretch_mode	:1;
};
typedef struct xgmac3_xgmac_stretch_mode_s xgmac3_xgmac_stretch_mode_t;

struct xgmac3_xgmac_stats_mtu1_s	/* 1620 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu1	:14;
};
typedef struct xgmac3_xgmac_stats_mtu1_s xgmac3_xgmac_stats_mtu1_t;

struct xgmac3_xgmac_stats_mtu2_s	/* 1621 */
{

	uint32 rsv_0	:18;
	uint32 packet_len_mtu2	:14;
};
typedef struct xgmac3_xgmac_stats_mtu2_s xgmac3_xgmac_stats_mtu2_t;

struct xgmac3_xgmac_stats_config_s	/* 1622 */
{

	uint32 rsv_0	:29;
	uint32 incr_hold	:1;
	uint32 incr_saturate	:1;
	uint32 clear_on_read	:1;
};
typedef struct xgmac3_xgmac_stats_config_s xgmac3_xgmac_stats_config_t;

struct xgmac3_xgmac_stats_init_s	/* 1623 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac3_xgmac_stats_init_s xgmac3_xgmac_stats_init_t;

struct xgmac3_xgmac_stats_init_done_s	/* 1624 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_init	:1;
};
typedef struct xgmac3_xgmac_stats_init_done_s xgmac3_xgmac_stats_init_done_t;

struct xgmac3_xgmac_deskew_fifo_config_s	/* 1625 */
{

	uint32 rsv_0	:12;
	uint32 deskew_fifo_depth_cfg	:4;
	uint32 deskew_mask	:16;
};
typedef struct xgmac3_xgmac_deskew_fifo_config_s xgmac3_xgmac_deskew_fifo_config_t;

struct xgmac3_xgmac_config5_s	/* 1626 */
{

	uint32 keep_bay_hdr	:1;
	uint32 mdio_in_dly	:2;
	uint32 buf_store_stall_mask	:1;
	uint32 rsv_0	:4;
	uint32 pause_timer_cfg	:24;
};
typedef struct xgmac3_xgmac_config5_s xgmac3_xgmac_config5_t;

struct xgmac3_xgmac_interrupt_status_set_s	/* 1627 */
{

	uint32 rsv_0	:15;
	uint32 value_set	:17;
};
typedef struct xgmac3_xgmac_interrupt_status_set_s xgmac3_xgmac_interrupt_status_set_t;

struct xgmac3_xgmac_interrupt_status_reset_s	/* 1628 */
{

	uint32 rsv_0	:15;
	uint32 value_reset	:17;
};
typedef struct xgmac3_xgmac_interrupt_status_reset_s xgmac3_xgmac_interrupt_status_reset_t;

struct xgmac3_xgmac_interrupt_mask_set_s	/* 1629 */
{

	uint32 rsv_0	:15;
	uint32 mask_set	:17;
};
typedef struct xgmac3_xgmac_interrupt_mask_set_s xgmac3_xgmac_interrupt_mask_set_t;

struct xgmac3_xgmac_interrupt_mask_reset_s	/* 1630 */
{

	uint32 rsv_0	:15;
	uint32 mask_reset	:17;
};
typedef struct xgmac3_xgmac_interrupt_mask_reset_s xgmac3_xgmac_interrupt_mask_reset_t;

struct xgmac3_xgmac_prbs_cfg_s	/* 1631 */
{

	uint32 rsv_0	:26;
	uint32 pcs_cfg_test_pattern	:2;
	uint32 rsv_1	:1;
	uint32 pcs_cfg_prbs_use_prbs7	:1;
	uint32 pcs_cfg_prbs_enable	:1;
	uint32 pcs_cfg_prbs_rst	:1;
};
typedef struct xgmac3_xgmac_prbs_cfg_s xgmac3_xgmac_prbs_cfg_t;

struct xgmac3_xgmac_prbs_err_cnt0_s	/* 1632 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt0	:4;
};
typedef struct xgmac3_xgmac_prbs_err_cnt0_s xgmac3_xgmac_prbs_err_cnt0_t;

struct xgmac3_xgmac_prbs_err_cnt1_s	/* 1633 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt1	:4;
};
typedef struct xgmac3_xgmac_prbs_err_cnt1_s xgmac3_xgmac_prbs_err_cnt1_t;

struct xgmac3_xgmac_prbs_err_cnt2_s	/* 1634 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt2	:4;
};
typedef struct xgmac3_xgmac_prbs_err_cnt2_s xgmac3_xgmac_prbs_err_cnt2_t;

struct xgmac3_xgmac_prbs_err_cnt3_s	/* 1635 */
{

	uint32 rsv_0	:28;
	uint32 prbs_err_cnt3	:4;
};
typedef struct xgmac3_xgmac_prbs_err_cnt3_s xgmac3_xgmac_prbs_err_cnt3_t;

struct xgmac3_xgmac8_b10b_err_cnt_s	/* 1636 */
{

	uint32 rsv_0	:4;
	uint32 code_err_cnt3	:4;
	uint32 rsv_1	:4;
	uint32 code_err_cnt2	:4;
	uint32 rsv_2	:4;
	uint32 code_err_cnt1	:4;
	uint32 rsv_3	:4;
	uint32 code_err_cnt0	:4;
};
typedef struct xgmac3_xgmac8_b10b_err_cnt_s xgmac3_xgmac8_b10b_err_cnt_t;

struct xgmac3_xgmac_tp_id_s	/* 1637 */
{

	uint32 rsv_0	:16;
	uint32 xgmac_tp_id	:16;
};
typedef struct xgmac3_xgmac_tp_id_s xgmac3_xgmac_tp_id_t;

struct xgmac3_xgmac_use_orginal_cos_s	/* 1638 */
{

	uint32 rsv_0	:31;
	uint32 use_original_cos	:1;
};
typedef struct xgmac3_xgmac_use_orginal_cos_s xgmac3_xgmac_use_orginal_cos_t;

struct xgmac3_xgmac_priority_map_s	/* 1639 */
{

	uint32 priority_map0	:32;

	uint32 priority_map1	:32;

	uint32 priority_map2	:32;

	uint32 priority_map3	:32;

	uint32 priority_map4	:32;

	uint32 priority_map5	:32;

	uint32 priority_map6	:32;

	uint32 priority_map7	:32;
};
typedef struct xgmac3_xgmac_priority_map_s xgmac3_xgmac_priority_map_t;

struct xgmac3_xgmac_gen_pkt_s	/* 1640 */
{

	uint32 rsv_0	:2;
	uint32 auto_pkt_len	:14;
	uint32 auto_burst_pkt_num	:12;
	uint32 auto_payload_inc	:1;
	uint32 auto_random_len	:1;
	uint32 auto_burst	:1;
	uint32 auto_pkt_gen	:1;
};
typedef struct xgmac3_xgmac_gen_pkt_s xgmac3_xgmac_gen_pkt_t;

struct xgmac3_xgmac_payload_s	/* 1641 */
{

	uint32 pktd_word8	:32;

	uint32 pktd_word9	:32;

	uint32 pktd_word10	:32;

	uint32 pktd_word11	:32;

	uint32 pktd_word12	:32;

	uint32 mac_da_cfg47_16	:32;

	uint32 rsv_0	:16;
	uint32 mac_da_cfg15_0	:16;
};
typedef struct xgmac3_xgmac_payload_s xgmac3_xgmac_payload_t;

struct xgmac3_xgmac_drain_en_s	/* 1642 */
{

	uint32 rsv_0	:31;
	uint32 xgmac_drain_en	:1;
};
typedef struct xgmac3_xgmac_drain_en_s xgmac3_xgmac_drain_en_t;

struct xgmac3_xgmac_mdio_cmd_status_s	/* 1643 */
{

	uint32 rsv_0	:31;
	uint32 mdio_cmd_done	:1;
};
typedef struct xgmac3_xgmac_mdio_cmd_status_s xgmac3_xgmac_mdio_cmd_status_t;

struct xgmac3_xgmac_mdio_cfg_s	/* 1644 */
{

	uint32 rsv_0	:15;
	uint32 mdio_status_mode	:1;
	uint32 rsv_1	:3;
	uint32 mdio_gmac_pre	:5;
	uint32 rsv_2	:3;
	uint32 mdio_xgmac_pre	:5;
};
typedef struct xgmac3_xgmac_mdio_cfg_s xgmac3_xgmac_mdio_cfg_t;

struct xgmac3_xgmac_ptp_en_s	/* 1645 */
{

	uint32 rsv_0	:30;
	uint32 tx_ptp_error_en	:1;
	uint32 ptp_en	:1;
};
typedef struct xgmac3_xgmac_ptp_en_s xgmac3_xgmac_ptp_en_t;

struct xgmac3_xgmac_ptp_status_s	/* 1646 */
{

	uint32 rsv_0	:28;
	uint32 tx_ts_collision	:1;
	uint32 tx_ts_not_rdy	:1;
	uint32 rx_ts_collision	:1;
	uint32 rx_ts_not_rdy	:1;
};
typedef struct xgmac3_xgmac_ptp_status_s xgmac3_xgmac_ptp_status_t;

struct buf_retrv_pkt_msg_mem_s	/* 0 */
{

	uint32 rsv_0	:25;
	uint32 data0	:7;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;
};
typedef struct buf_retrv_pkt_msg_mem_s buf_retrv_pkt_msg_mem_t;

struct buf_retrv_buf_ram_s	/* 1 */
{

	uint32 rsv_0	:7;
	uint32 data0	:25;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;
};
typedef struct buf_retrv_buf_ram_s buf_retrv_buf_ram_t;

struct buf_retrv_pkt_config_mem_s	/* 2 */
{

	uint32 rsv_0	:10;
	uint32 pkt_config_weight	:6;
	uint32 rsv_1	:5;
	uint32 pkt_config_end	:11;

	uint32 rsv_2	:21;
	uint32 pkt_config_start	:11;
};
typedef struct buf_retrv_pkt_config_mem_s buf_retrv_pkt_config_mem_t;

struct buf_retrv_pkt_status_mem_s	/* 3 */
{

	uint32 rsv_0	:10;
	uint32 pkt_status_weight	:6;
	uint32 rsv_1	:5;
	uint32 pkt_status_count	:11;

	uint32 rsv_2	:5;
	uint32 pkt_status_tail	:11;
	uint32 rsv_3	:5;
	uint32 pkt_status_head	:11;
};
typedef struct buf_retrv_pkt_status_mem_s buf_retrv_pkt_status_mem_t;

struct buf_retrv_pkt_park_mem_s	/* 4 */
{

	uint32 rsv_0	:3;
	uint32 data0	:29;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;
};
typedef struct buf_retrv_pkt_park_mem_s buf_retrv_pkt_park_mem_t;

struct buf_retrv_buf_config_mem_s	/* 5 */
{

	uint32 rsv_0	:14;
	uint32 buf_config_burst_cnt_sel	:2;
	uint32 rsv_1	:10;
	uint32 buf_config_weight	:6;

	uint32 rsv_2	:5;
	uint32 buf_config_end	:11;
	uint32 rsv_3	:5;
	uint32 buf_config_start	:11;
};
typedef struct buf_retrv_buf_config_mem_s buf_retrv_buf_config_mem_t;

struct buf_retrv_buf_status_mem_s	/* 6 */
{

	uint32 buf_status1st_data	:1;
	uint32 rsv_0	:4;
	uint32 buf_status_data_cnt	:3;
	uint32 rsv_1	:6;
	uint32 buf_status_weight	:6;
	uint32 rsv_2	:1;
	uint32 buf_status_count	:11;

	uint32 rsv_3	:5;
	uint32 buf_status_offset	:3;
	uint32 rsv_4	:1;
	uint32 buf_status_tail	:11;
	uint32 rsv_5	:1;
	uint32 buf_status_head	:11;
};
typedef struct buf_retrv_buf_status_mem_s buf_retrv_buf_status_mem_t;

struct buf_retrv_credit_mem_s	/* 7 */
{

	uint32 rsv_0	:20;
	uint32 credit	:12;
};
typedef struct buf_retrv_credit_mem_s buf_retrv_credit_mem_t;

struct buf_retrv_credit_config_mem_s	/* 8 */
{

	uint32 rsv_0	:29;
	uint32 credit_config	:3;
};
typedef struct buf_retrv_credit_config_mem_s buf_retrv_credit_config_mem_t;

struct buf_retrv_exception_mem_s	/* 9 */
{

	uint32 rsv_0	:7;
	uint32 exception_sub_index_en	:1;
	uint32 rsv_1	:4;
	uint32 exception_data	:20;
};
typedef struct buf_retrv_exception_mem_s buf_retrv_exception_mem_t;

struct buf_retrv_buf_credit_mem_s	/* 10 */
{

	uint32 rsv_0	:28;
	uint32 credit	:4;
};
typedef struct buf_retrv_buf_credit_mem_s buf_retrv_buf_credit_mem_t;

struct buf_retrv_buf_credit_config_mem_s	/* 11 */
{

	uint32 rsv_0	:30;
	uint32 credit_config	:2;
};
typedef struct buf_retrv_buf_credit_config_mem_s buf_retrv_buf_credit_config_mem_t;

struct ds_buf_retrv_color_map_s	/* 12 */
{

	uint32 rsv_0	:30;
	uint32 color_map	:2;
};
typedef struct ds_buf_retrv_color_map_s ds_buf_retrv_color_map_t;

struct met_fifo_priority_map_table_s	/* 13 */
{

	uint32 rsv_0	:19;
	uint32 met_fifo_priority	:1;
	uint32 resrc_drop_precedence	:2;
	uint32 drop_precedence	:2;
	uint32 rsv_1	:2;
	uint32 queue_select	:6;
};
typedef struct met_fifo_priority_map_table_s met_fifo_priority_map_table_t;

struct buffer_store_resrc_cnt_s	/* 14 */
{

	uint32 rsv_0	:16;
	uint32 resrc_cnt	:16;
};
typedef struct buffer_store_resrc_cnt_s buffer_store_resrc_cnt_t;

struct buffer_store_resrc_threshold_s	/* 15 */
{

	uint32 resrc_drop_threshold3	:16;
	uint32 resrc_drop_threshold2	:16;

	uint32 resrc_drop_threshold1	:16;
	uint32 resrc_drop_threshold0	:16;
};
typedef struct buffer_store_resrc_threshold_s buffer_store_resrc_threshold_t;

struct buf_store_channel_info_ram_s	/* 16 */
{

	uint32 rsv_0	:15;
	uint32 word0	:17;

	uint32 word1	:32;

	uint32 word2	:32;

	uint32 word3	:32;

	uint32 word4	:32;

	uint32 word5	:32;

	uint32 word6	:32;

	uint32 rsv_1	:32;
};
typedef struct buf_store_channel_info_ram_s buf_store_channel_info_ram_t;

struct buf_store_buf_ptr_s	/* 17 */
{

	uint32 rsv_0	:15;
	uint32 buf_ptr	:17;
};
typedef struct buf_store_buf_ptr_s buf_store_buf_ptr_t;

struct buffer_store_stall_threshold_s	/* 18 */
{

	uint32 stall_high	:16;
	uint32 stall_low	:16;
};
typedef struct buffer_store_stall_threshold_s buffer_store_stall_threshold_t;

struct cpumac_stats_ram_s	/* 19 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_high	:4;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_high	:4;

	uint32 byte_cnt_data_low	:32;
};
typedef struct cpumac_stats_ram_s cpumac_stats_ram_t;

struct tcam_int_key_ram_s	/* 22 */
{

	uint32 rsv_0	:31;
	uint32 key0	:1;

	uint32 rsv_1	:16;
	uint32 key1	:16;

	uint32 key2	:32;

	uint32 key3	:32;
};
typedef struct tcam_int_key_ram_s tcam_int_key_ram_t;

struct tcam_int_mask_ram_s	/* 23 */
{

	uint32 rsv_0	:31;
	uint32 mask0	:1;

	uint32 rsv_1	:16;
	uint32 mask1	:16;

	uint32 mask2	:32;

	uint32 mask3	:32;
};
typedef struct tcam_int_mask_ram_s tcam_int_mask_ram_t;

struct tcam_ext_key_ram_s	/* 24 */
{

	uint32 rsv_0	:31;
	uint32 key0	:1;

	uint32 rsv_1	:16;
	uint32 key1	:16;

	uint32 key2	:32;

	uint32 key3	:32;
};
typedef struct tcam_ext_key_ram_s tcam_ext_key_ram_t;

struct tcam_ext_mask_ram_s	/* 25 */
{

	uint32 rsv_0	:31;
	uint32 mask0	:1;

	uint32 rsv_1	:16;
	uint32 mask1	:16;

	uint32 mask2	:32;

	uint32 mask3	:32;
};
typedef struct tcam_ext_mask_ram_s tcam_ext_mask_ram_t;

struct int_sram_ram_s	/* 26 */
{

	uint32 rsv_0	:24;
	uint32 data0	:8;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 rsv_1	:32;
};
typedef struct int_sram_ram_s int_sram_ram_t;

struct hash_tab98k_ram_s	/* 27 */
{

	uint32 rsv_0	:24;
	uint32 data0	:8;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 rsv_1	:32;
};
typedef struct hash_tab98k_ram_s hash_tab98k_ram_t;

struct ext_ddr_ram_s	/* 28 */
{

	uint32 rsv_0	:24;
	uint32 data0	:8;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 rsv_1	:32;
};
typedef struct ext_ddr_ram_s ext_ddr_ram_t;

struct hash_tab50k_ram_s	/* 29 */
{

	uint32 rsv_0	:24;
	uint32 data0	:8;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 rsv_1	:32;
};
typedef struct hash_tab50k_ram_s hash_tab50k_ram_t;

struct tcam_ext_reg_ram_s	/* 30 */
{

	uint32 rsv_0	:31;
	uint32 data0	:1;

	uint32 rsv_1	:16;
	uint32 data1	:16;

	uint32 data2	:32;

	uint32 data3	:32;
};
typedef struct tcam_ext_reg_ram_s tcam_ext_reg_ram_t;

struct epe_classification_phb_offset_table_s	/* 31 */
{

	uint32 offset0	:2;
	uint32 offset1	:2;
	uint32 offset2	:2;
	uint32 offset3	:2;
	uint32 offset4	:2;
	uint32 offset5	:2;
	uint32 offset6	:2;
	uint32 offset7	:2;
	uint32 offset8	:2;
	uint32 offset9	:2;
	uint32 offset10	:2;
	uint32 offset11	:2;
	uint32 offset12	:2;
	uint32 offset13	:2;
	uint32 offset14	:2;
	uint32 offset15	:2;
};
typedef struct epe_classification_phb_offset_table_s epe_classification_phb_offset_table_t;

struct ds_dest_phy_port_s	/* 32 */
{

	uint32 rsv_0	:8;
	uint32 random_log_en	:1;
	uint32 dest_discard	:1;
	uint32 discard_non8023_oam	:1;
	uint32 mux_port_type	:2;
	uint32 l2_span_en	:1;
	uint32 l2_span_id	:2;
	uint32 rsv_1	:3;
	uint32 global_dest_port	:13;

	uint32 rsv_2	:17;
	uint32 random_threshold	:15;
};
typedef struct ds_dest_phy_port_s ds_dest_phy_port_t;

struct epe_hdr_edit_l2_edit_loopback_ram_s	/* 33 */
{

	uint32 rsv_0	:28;
	uint32 parity0	:1;
	uint32 rsv_1	:3;

	uint32 rsv_2	:9;
	uint32 lb_length_adjust_type	:1;
	uint32 lb_dest_map	:22;

	uint32 rsv_3	:28;
	uint32 parity1	:1;
	uint32 rsv_4	:3;

	uint32 rsv_5	:11;
	uint32 lb_next_hop_ext	:1;
	uint32 lb_next_hop_ptr	:20;
};
typedef struct epe_hdr_edit_l2_edit_loopback_ram_s epe_hdr_edit_l2_edit_loopback_ram_t;

struct epe_header_edit_sgmac_priority_map_mem_s	/* 34 */
{

	uint32 rsv_0	:22;
	uint32 dp	:2;
	uint32 rsv_1	:4;
	uint32 tc	:4;
};
typedef struct epe_header_edit_sgmac_priority_map_mem_s epe_header_edit_sgmac_priority_map_mem_t;

struct epe_hdr_edit_discard_type_stats_s	/* 35 */
{

	uint32 rsv_0	:24;
	uint32 discard_count	:8;
};
typedef struct epe_hdr_edit_discard_type_stats_s epe_hdr_edit_discard_type_stats_t;

struct ds_l3_edit_tunnel_v6_ip_s	/* 36 */
{

	uint32 ip_sa127_to96	:32;

	uint32 ip_sa95_to64	:32;

	uint32 ip_sa63_to32	:32;

	uint32 ip_sa31_to0	:32;
};
typedef struct ds_l3_edit_tunnel_v6_ip_s ds_l3_edit_tunnel_v6_ip_t;

struct ds_l3_edit_sequence_num_s	/* 37 */
{

	uint32 seq_num127_to96	:32;

	uint32 seq_num95_to64	:32;

	uint32 seq_num63_to32	:32;

	uint32 seq_num31_to0	:32;
};
typedef struct ds_l3_edit_sequence_num_s ds_l3_edit_sequence_num_t;

struct ds_l3_edit_tunnel_v4_ip_sa_s	/* 38 */
{

	uint32 ip_sa	:32;
};
typedef struct ds_l3_edit_tunnel_v4_ip_sa_s ds_l3_edit_tunnel_v4_ip_sa_t;

struct ds_dest_port_s	/* 39 */
{

	uint32 port_policer_valid	:1;
	uint32 transmit_en	:1;
	uint32 l2_acl_en	:1;
	uint32 l2_qos_lookup_en	:1;
	uint32 bridge_en	:1;
	uint32 qos_domain	:3;
	uint32 force_ipv6_to_mac_key	:1;
	uint32 vlan_flow_policer_valid	:1;
	uint32 force_ipv4_to_mac_key	:1;
	uint32 l2_qos_high_priority	:1;
	uint32 egress_filter_en	:1;
	uint32 routed_port	:1;
	uint32 rsv_0	:1;
	uint32 bridge_l2_match_disable	:1;
	uint32 stp_check_disable	:1;
	uint32 l2_acl_high_priority	:1;
	uint32 replace_cos	:1;
	uint32 replace_dscp	:1;
	uint32 default_vlan_id	:12;

	uint32 l2_acl_label	:8;
	uint32 l2_qos_lable	:8;
	uint32 vpls_port_type	:1;
	uint32 untag_default_svlan	:1;
	uint32 rsv_1	:7;
	uint32 svlan_tpid_index	:2;
	uint32 dot1q_en	:2;
	uint32 untag_default_vlan_id	:1;
	uint32 ipg_index	:2;

	uint32 rsv_2	:5;
	uint32 pbb_port_type	:3;
	uint32 pip_mac_sa	:8;
	uint32 rsv_3	:4;
	uint32 mcast_flooding_disable	:1;
	uint32 ucast_flooding_disable	:1;
	uint32 dest_port_isolation_id	:6;
	uint32 md_level	:3;
	uint32 ether_oam_valid	:1;

	uint32 rsv_4	:32;
};
typedef struct ds_dest_port_s ds_dest_port_t;

struct epe_edit_priority_map_table_s	/* 40 */
{

	uint32 rsv_0	:17;
	uint32 mapped_exp	:3;
	uint32 mapped_cfi	:1;
	uint32 mapped_cos	:3;
	uint32 rsv_1	:2;
	uint32 mapped_dscp	:6;
};
typedef struct epe_edit_priority_map_table_s epe_edit_priority_map_table_t;

struct ds_dest_interface_s	/* 41 */
{

	uint32 rsv_0	:1;
	uint32 l3_acl_routed_only	:1;
	uint32 l3_acl_en	:1;
	uint32 l3_qos_lookup_en	:1;
	uint32 mtu_exception_en	:1;
	uint32 rsv_1	:4;
	uint32 mtu_check_en	:1;
	uint32 rsv_2	:1;
	uint32 l3_span_en	:1;
	uint32 l3_span_id	:2;
	uint32 mcast_ttl_threshold	:8;
	uint32 mac_sa_type	:2;
	uint32 mac_sa	:8;

	uint32 rsv_3	:2;
	uint32 mtu_size	:14;
	uint32 l3_acl_label	:8;
	uint32 l3_qos_label	:8;
};
typedef struct ds_dest_interface_s ds_dest_interface_t;

struct ds_vpls_port_s	/* 42 */
{

	uint32 vpls_port_type	:32;
};
typedef struct ds_vpls_port_s ds_vpls_port_t;

struct epe_next_hop_internal4w_s	/* 43 */
{

	uint32 rsv_0	:29;
	uint32 l2_rewrite_type	:3;

	uint32 stag_cfi	:1;
	uint32 stag_cos	:3;
	uint32 copy_ctag_cos	:1;
	uint32 svlan_tagged	:1;
	uint32 cvlan_tagged	:1;
	uint32 mtu_check_en	:1;
	uint32 by_pass_all	:1;
	uint32 payload_operation	:3;
	uint32 output_svlan_id_valid	:1;
	uint32 output_cvlan_id_valid	:1;
	uint32 l3edit_ptr	:18;

	uint32 rsv_1	:29;
	uint32 l3_rewrite_type	:3;

	uint32 replace_dscp	:1;
	uint32 replace_ctag_cos	:1;
	uint32 dest_vlan_ptr	:14;
	uint32 derive_stag_cos	:1;
	uint32 service_acl_qos_en	:1;
	uint32 service_policer_vld	:1;
	uint32 tagged_mode	:1;
	uint32 l2edit_ptr	:12;
};
typedef struct epe_next_hop_internal4w_s epe_next_hop_internal4w_t;

struct epe_next_hop_internal8w_s	/* 44 */
{

	uint32 rsv_0	:29;
	uint32 l2_rewrite_type	:3;

	uint32 stag_cfi	:1;
	uint32 stag_cos	:3;
	uint32 copy_ctag_cos	:1;
	uint32 svlan_tagged	:1;
	uint32 cvlan_tagged	:1;
	uint32 mtu_check_en	:1;
	uint32 by_pass_all	:1;
	uint32 payload_operation	:3;
	uint32 output_svlan_id_valid	:1;
	uint32 output_cvlan_id_valid	:1;
	uint32 l3edit_ptr170	:18;

	uint32 rsv_1	:29;
	uint32 l3_rewrite_type	:3;

	uint32 replace_dscp	:1;
	uint32 replace_ctag_cos	:1;
	uint32 dest_vlan_ptr	:14;
	uint32 derive_stag_cos	:1;
	uint32 service_acl_qos_en	:1;
	uint32 service_policer_valid	:1;
	uint32 tagged_mode	:1;
	uint32 l2edit_ptr12to0	:12;

	uint32 rsv_2	:29;
	uint32 output_svlan_id_valid_ext	:1;
	uint32 output_cvlan_id_valid_ext	:1;
	uint32 l3edit_ptr19	:1;

	uint32 l3edit_ptr18	:1;
	uint32 l2edit_ptr19to13	:7;
	uint32 output_svlan_id_ext	:12;
	uint32 output_cvlan_id_ext	:12;

	uint32 rsv_3	:29;
	uint32 vpls_port_check	:1;
	uint32 tunnel_mtu_check	:1;
	uint32 service_id_en	:1;

	uint32 tunnel_update_disable	:1;
	uint32 community_port	:1;
	uint32 service_id	:14;
	uint32 svlan_tpid	:2;
	uint32 svlan_tpid_en	:1;
	uint32 vpls_dest_port	:13;
};
typedef struct epe_next_hop_internal8w_s epe_next_hop_internal8w_t;

struct epe_statsramepephbintf_s	/* 45 */
{

	uint32 rsv_0	:28;
	uint32 parity0_epe_phb_intf	:1;
	uint32 use_l3_length_epe_phb_intf	:1;
	uint32 byte_count36_to35_epe_phb_intf	:2;

	uint32 packet_count31_to0_epe_phb_intf	:32;

	uint32 rsv_1	:28;
	uint32 parity1_epe_phb_intf	:1;
	uint32 byte_count34_to32_epe_phb_intf	:3;

	uint32 byte_count31_to0_epe_phb_intf	:32;
};
typedef struct epe_statsramepephbintf_s epe_statsramepephbintf_t;

struct epe_statsramepeportlog_s	/* 46 */
{

	uint32 rsv_0	:28;
	uint32 parity0_epe_port_log	:1;
	uint32 use_l3_length_epe_port_log	:1;
	uint32 byte_count36_to35_epe_port_log	:2;

	uint32 packet_count31_to0_epe_port_log	:32;

	uint32 rsv_1	:28;
	uint32 parity1_epe_port_log	:1;
	uint32 byte_count34_to32_epe_port_log	:3;

	uint32 byte_count31_to0_epe_port_log	:32;
};
typedef struct epe_statsramepeportlog_s epe_statsramepeportlog_t;

struct epe_statsramepeoverallfwd_s	/* 47 */
{

	uint32 rsv_0	:28;
	uint32 parity0_epe_overall_fwd	:1;
	uint32 use_l3_length_epe_overall_fwd	:1;
	uint32 byte_count36_to35_epe_overall_fwd	:2;

	uint32 packet_count31_to0_epe_overall_fwd	:32;

	uint32 rsv_1	:28;
	uint32 parity1_epe_overall_fwd	:1;
	uint32 byte_count34_to32_epe_overall_fwd	:3;

	uint32 byte_count31_to0_epe_overall_fwd	:32;
};
typedef struct epe_statsramepeoverallfwd_s epe_statsramepeoverallfwd_t;

struct fabric_cas_info_ram_s	/* 48 */
{

	uint32 rsv_0	:5;
	uint32 info_data	:27;
};
typedef struct fabric_cas_info_ram_s fabric_cas_info_ram_t;

struct fabric_cas_data_ram_s	/* 49 */
{

	uint32 data0	:32;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;

	uint32 data4	:32;

	uint32 data5	:32;

	uint32 data6	:32;

	uint32 data7	:32;
};
typedef struct fabric_cas_data_ram_s fabric_cas_data_ram_t;

struct fabriccrbcellbu_f0_s	/* 50 */
{

	uint32 word0	:32;

	uint32 word1	:32;

	uint32 word2	:32;

	uint32 word3	:32;

	uint32 word4	:32;

	uint32 word5	:32;

	uint32 word6	:32;

	uint32 word7	:32;

	uint32 rsv_0	:24;
	uint32 parity	:8;

	uint32 rsv_1	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:32;
};
typedef struct fabriccrbcellbu_f0_s fabriccrbcellbu_f0_t;

struct fabriccrbcellbu_f1_s	/* 51 */
{

	uint32 word0	:32;

	uint32 word1	:32;

	uint32 word2	:32;

	uint32 word3	:32;

	uint32 word4	:32;

	uint32 word5	:32;

	uint32 word6	:32;

	uint32 word7	:32;

	uint32 rsv_0	:24;
	uint32 parity	:8;

	uint32 rsv_1	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:32;
};
typedef struct fabriccrbcellbu_f1_s fabriccrbcellbu_f1_t;

struct fabriccrbvalidtabl_e0_s	/* 52 */
{

	uint32 rsv_0	:31;
	uint32 valid	:1;
};
typedef struct fabriccrbvalidtabl_e0_s fabriccrbvalidtabl_e0_t;

struct fabriccrbvalidtabl_e1_s	/* 53 */
{

	uint32 rsv_0	:31;
	uint32 valid	:1;
};
typedef struct fabriccrbvalidtabl_e1_s fabriccrbvalidtabl_e1_t;

struct fabriccrbptrtabl_e0_s	/* 54 */
{

	uint32 rsv_0	:28;
	uint32 addr_ptr	:4;
};
typedef struct fabriccrbptrtabl_e0_s fabriccrbptrtabl_e0_t;

struct fabriccrbptrtabl_e1_s	/* 55 */
{

	uint32 rsv_0	:28;
	uint32 addr_ptr	:4;
};
typedef struct fabriccrbptrtabl_e1_s fabriccrbptrtabl_e1_t;

struct fabriccrbcellinfobu_f0_s	/* 56 */
{

	uint32 rsv_0	:8;
	uint32 idle_cell	:1;
	uint32 priority	:2;
	uint32 src_chip_id	:5;
	uint32 cell_infor	:14;
	uint32 seq_num	:2;
};
typedef struct fabriccrbcellinfobu_f0_s fabriccrbcellinfobu_f0_t;

struct fabriccrbcellinfobu_f1_s	/* 57 */
{

	uint32 rsv_0	:8;
	uint32 idle_cell	:1;
	uint32 priority	:2;
	uint32 src_chip_id	:5;
	uint32 cell_infor	:14;
	uint32 seq_num	:2;
};
typedef struct fabriccrbcellinfobu_f1_s fabriccrbcellinfobu_f1_t;

struct fabric_gts_rts_track_ram_s	/* 58 */
{

	uint32 rsv_0	:16;
	uint32 data	:16;
};
typedef struct fabric_gts_rts_track_ram_s fabric_gts_rts_track_ram_t;

struct fabricrtscounter_s	/* 59 */
{

	uint32 rsv_0	:21;
	uint32 count	:11;
};
typedef struct fabricrtscounter_s fabricrtscounter_t;

struct chaninfotable_s	/* 60 */
{

	uint32 rsv_0	:7;
	uint32 chan_cell_count	:9;
	uint32 rsv_1	:12;
	uint32 two_frag	:1;
	uint32 tail_cell_write_offset	:3;

	uint32 rsv_2	:7;
	uint32 chan_cell_tail	:9;
	uint32 rsv_3	:7;
	uint32 chan_cell_head	:9;
};
typedef struct chaninfotable_s chaninfotable_t;

struct cellinfotable_s	/* 61 */
{

	uint32 rsv_0	:6;
	uint32 second_frag_error	:1;
	uint32 second_frag_full	:1;
	uint32 rsv_1	:1;
	uint32 second_frag_length	:3;
	uint32 rsv_2	:2;
	uint32 second_frag_eop	:1;
	uint32 second_frag_sop	:1;
	uint32 rsv_3	:6;
	uint32 first_frag_error	:1;
	uint32 first_frag_full	:1;
	uint32 rsv_4	:1;
	uint32 first_frag_length	:3;
	uint32 rsv_5	:2;
	uint32 first_frag_eop	:1;
	uint32 first_frag_sop	:1;
};
typedef struct cellinfotable_s cellinfotable_t;

struct celltable_s	/* 62 */
{

	uint32 rsv_0	:23;
	uint32 cell_ptr	:9;
};
typedef struct celltable_s celltable_t;

struct hash_ds_ctl_hash_table_s	/* 63 */
{

	uint32 rsv_0	:28;
	uint32 parity0	:1;
	uint32 data69_to67	:3;

	uint32 data66_to35	:32;

	uint32 rsv_1	:28;
	uint32 parity1	:1;
	uint32 data34_to32	:3;

	uint32 data31_to0	:32;
};
typedef struct hash_ds_ctl_hash_table_s hash_ds_ctl_hash_table_t;

struct ipe_aging_ram_s	/* 65 */
{

	uint32 aging_status	:32;
};
typedef struct ipe_aging_ram_s ipe_aging_ram_t;

struct fwdexttable_s	/* 66 */
{

	uint32 aps_group_id	:16;
	uint32 stats_ptr	:16;
};
typedef struct fwdexttable_s fwdexttable_t;

struct apsbridgetable_s	/* 67 */
{

	uint32 rsv_0	:16;
	uint32 working_dest_map	:16;

	uint32 rsv_1	:7;
	uint32 protecting_en	:1;
	uint32 rsv_2	:2;
	uint32 protecting_dest_map	:22;
};
typedef struct apsbridgetable_s apsbridgetable_t;

struct sequencenumbertable_s	/* 68 */
{

	uint32 sequence_number	:32;
};
typedef struct sequencenumbertable_s sequencenumbertable_t;

struct apsselecttable_s	/* 69 */
{

	uint32 protecting_en31	:1;
	uint32 protecting_en30	:1;
	uint32 protecting_en29	:1;
	uint32 protecting_en28	:1;
	uint32 protecting_en27	:1;
	uint32 protecting_en26	:1;
	uint32 protecting_en25	:1;
	uint32 protecting_en24	:1;
	uint32 protecting_en23	:1;
	uint32 protecting_en22	:1;
	uint32 protecting_en21	:1;
	uint32 protecting_en20	:1;
	uint32 protecting_en19	:1;
	uint32 protecting_en18	:1;
	uint32 protecting_en17	:1;
	uint32 protecting_en16	:1;
	uint32 protecting_en15	:1;
	uint32 protecting_en14	:1;
	uint32 protecting_en13	:1;
	uint32 protecting_en12	:1;
	uint32 protecting_en11	:1;
	uint32 protecting_en10	:1;
	uint32 protecting_en9	:1;
	uint32 protecting_en8	:1;
	uint32 protecting_en7	:1;
	uint32 protecting_en6	:1;
	uint32 protecting_en5	:1;
	uint32 protecting_en4	:1;
	uint32 protecting_en3	:1;
	uint32 protecting_en2	:1;
	uint32 protecting_en1	:1;
	uint32 protecting_en0	:1;
};
typedef struct apsselecttable_s apsselecttable_t;

struct discardcount_s	/* 70 */
{

	uint32 rsv_0	:24;
	uint32 counter	:8;
};
typedef struct discardcount_s discardcount_t;

struct phy_port_map_table_s	/* 71 */
{

	uint32 rsv_0	:26;
	uint32 local_phy_port	:6;
};
typedef struct phy_port_map_table_s phy_port_map_table_t;

struct sgmac_tc_map_table_s	/* 72 */
{

	uint32 priority3	:6;
	uint32 color3	:2;
	uint32 priority2	:6;
	uint32 color2	:2;
	uint32 priority1	:6;
	uint32 color1	:2;
	uint32 priority0	:6;
	uint32 color0	:2;
};
typedef struct sgmac_tc_map_table_s sgmac_tc_map_table_t;

struct ds_phy_port_s	/* 73 */
{

	uint32 random_log_en	:1;
	uint32 random_threshold	:15;
	uint32 src_discard	:1;
	uint32 outer_vlan_is_cvlan	:1;
	uint32 l2_span_en	:1;
	uint32 l2_span_id	:2;
	uint32 ptp_en	:1;
	uint32 svlan_tpid_index	:2;
	uint32 keep_vlan_tag	:1;
	uint32 pbb_port_type	:3;
	uint32 packet_type_valid	:1;
	uint32 packet_type	:3;
};
typedef struct ds_phy_port_s ds_phy_port_t;

struct ds_protocol_vlan_s	/* 74 */
{

	uint32 rsv_0	:16;
	uint32 protocol_vlan_id_valid	:1;
	uint32 cpu_exception_en	:1;
	uint32 discard	:1;
	uint32 replace_tag_en	:1;
	uint32 protocol_vlan_id	:12;
};
typedef struct ds_protocol_vlan_s ds_protocol_vlan_t;

struct ds_router_mac_s	/* 75 */
{

	uint32 rsv_0	:24;
	uint32 router_mac3_type	:2;
	uint32 router_mac2_type	:2;
	uint32 router_mac1_type	:2;
	uint32 router_mac0_type	:2;

	uint32 router_mac3_byte	:8;
	uint32 router_mac2_byte	:8;
	uint32 router_mac1_byte	:8;
	uint32 router_mac0_byte	:8;
};
typedef struct ds_router_mac_s ds_router_mac_t;

struct ds_vrf_s	/* 76 */
{

	uint32 rsv_0	:24;
	uint32 pfm3	:2;
	uint32 pfm2	:2;
	uint32 pfm1	:2;
	uint32 pfm0	:2;
};
typedef struct ds_vrf_s ds_vrf_t;

struct ds_src_port_s	/* 77 */
{

	uint32 ingress_filtering_en	:1;
	uint32 mac_security_discard	:1;
	uint32 l2_acl_en	:1;
	uint32 l2_qos_lookup_en	:1;
	uint32 bridge_en	:1;
	uint32 rsv_0	:3;
	uint32 use_outer_ttl	:1;
	uint32 qos_policy	:3;
	uint32 l2_acl_high_priority	:1;
	uint32 rsv_1	:5;
	uint32 allow_mcast_mac_sa	:1;
	uint32 protocol_vlan_en	:1;
	uint32 route_disable	:1;
	uint32 routed_port	:1;
	uint32 receive_en	:1;
	uint32 port_check_en	:1;
	uint32 pip_mac_sa	:8;

	uint32 l2_acl_label	:8;
	uint32 l2_qos_label	:8;
	uint32 default_replace_tag_en	:1;
	uint32 learning_disable	:1;
	uint32 force_acl_qos_ipv4_to_mac_key	:1;
	uint32 l2_qos_high_priority	:1;
	uint32 qos_domain	:3;
	uint32 port_security_exception_en	:1;
	uint32 vpls_port_type	:1;
	uint32 vlan_flow_policer_valid	:1;
	uint32 port_policer_valid	:1;
	uint32 port_cross_connect	:1;
	uint32 vlan_tag_ctl	:4;

	uint32 oam_tunnel_en	:1;
	uint32 port_security_en	:1;
	uint32 source_port_isolated	:6;
	uint32 md_level	:3;
	uint32 ether_oam_valid	:1;
	uint32 force_acl_qos_ipv6_to_mac_key	:1;
	uint32 use_btag_cos	:1;
	uint32 rsv_2	:2;
	uint32 oam_link_max_md_level	:3;
	uint32 vpls_src_port	:13;

	uint32 rsv_3	:9;
	uint32 equal_cos_path_num	:3;
	uint32 rsv_4	:1;
	uint32 priority_path_en	:1;
	uint32 rsv_5	:1;
	uint32 use_stag_cos	:1;
	uint32 ipg_index	:2;
	uint32 routed_port_vlan_ptr	:14;
};
typedef struct ds_src_port_s ds_src_port_t;

struct ds_phy_port_ext_s	/* 78 */
{

	uint32 egress_user_id_type	:2;
	uint32 egress_user_id_en	:1;
	uint32 svlan_key_first	:1;
	uint32 oam_obey_user_id	:1;
	uint32 src_outer_vlan_is_svlan	:1;
	uint32 use_default_vlan_lookup	:1;
	uint32 user_id_en	:1;
	uint32 user_id_type	:2;
	uint32 user_id_label	:6;
	uint32 rsv_0	:3;
	uint32 global_src_port	:13;

	uint32 disable_user_id_ipv6	:1;
	uint32 disable_user_id_ipv4	:1;
	uint32 force_user_id_ipv6_to_mac_key	:1;
	uint32 force_user_id_ipv4_to_mac_key	:1;
	uint32 default_vlan_id	:12;
	uint32 exception2_en	:16;

	uint32 exception2_discard	:16;
	uint32 rsv_1	:12;
	uint32 default_dei	:1;
	uint32 default_pcp	:3;

	uint32 rsv_2	:32;
};
typedef struct ds_phy_port_ext_s ds_phy_port_ext_t;

struct ds_src_interface_s	/* 79 */
{

	uint32 rsv_0	:2;
	uint32 pbr_label	:6;
	uint32 lookup_mode	:1;
	uint32 v4_mcast_rpf_en	:1;
	uint32 v6_mcast_rpf_en	:1;
	uint32 mpls_en	:1;
	uint32 rsv_1	:4;
	uint32 l3_acl_en	:1;
	uint32 l3_qos_lookup_en	:1;
	uint32 l3_acl_routed_only	:1;
	uint32 route_all_packets	:1;
	uint32 l3_if_type	:2;
	uint32 router_mac_type	:2;
	uint32 rsv_2	:2;
	uint32 router_mac_label	:6;

	uint32 exception3_en	:16;
	uint32 l3_qos_label	:8;
	uint32 l3_acl_label	:8;

	uint32 rsv_3	:16;
	uint32 mpls_label_space	:8;
	uint32 rsv_4	:5;
	uint32 l3_span_en	:1;
	uint32 l3_span_id	:2;

	uint32 rsv_5	:32;
};
typedef struct ds_src_interface_s ds_src_interface_t;

struct ds_mpls_ctl_s	/* 80 */
{

	uint32 rsv_0	:13;
	uint32 interface_label_valid_mcast	:1;
	uint32 num_of_label_mcast	:2;
	uint32 label_space_sizetype_mcast	:4;
	uint32 label_base_mcast	:12;

	uint32 rsv_1	:13;
	uint32 interface_label_valid	:1;
	uint32 num_of_label	:2;
	uint32 label_space_sizetype	:4;
	uint32 label_base	:12;
};
typedef struct ds_mpls_ctl_s ds_mpls_ctl_t;

struct ds_bidi_pim_group_table_s	/* 81 */
{

	uint32 rsv_0	:24;
	uint32 bidi_pim_block	:8;
};
typedef struct ds_bidi_pim_group_table_s ds_bidi_pim_group_table_t;

struct ds_storm_ctl_table_s	/* 82 */
{

	uint32 threshold	:32;

	uint32 running_count	:32;

	uint32 rsv_0	:29;
	uint32 storm_en	:1;
	uint32 exception_en	:1;
	uint32 use_packet_count	:1;

	uint32 rsv_1	:32;
};
typedef struct ds_storm_ctl_table_s ds_storm_ctl_table_t;

struct ipe_ds_pbb_mac_table_s	/* 83 */
{

	uint32 rsv_0	:3;
	uint32 global_src_port	:13;
	uint32 bmac_sa_bit47_to32	:16;

	uint32 bmac_sa_bit31_to0	:32;
};
typedef struct ipe_ds_pbb_mac_table_s ipe_ds_pbb_mac_table_t;

struct ipe_learning_cache_s	/* 84 */
{

	uint32 rsv_0	:20;
	uint32 cvlan_id	:12;

	uint32 is_vpls_src_port	:1;
	uint32 is_ether_oam	:1;
	uint32 rsv_1	:2;
	uint32 svlan_id	:12;
	uint32 ether_oam_md_level	:3;
	uint32 global_src_port	:13;

	uint32 mapped_vlan_id	:16;
	uint32 mac_sa_msb	:16;

	uint32 mac_sa_lsb	:32;

	uint32 rsv_2	:16;
	uint32 cmac_sa_msb	:16;

	uint32 cmac_sa_lsb	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;
};
typedef struct ipe_learning_cache_s ipe_learning_cache_t;

struct ipe_classification_dscp_map_table_s	/* 85 */
{

	uint32 dscp_priority3	:6;
	uint32 dscp_color3	:2;
	uint32 dscp_priority2	:6;
	uint32 dscp_color2	:2;
	uint32 dscp_priority1	:6;
	uint32 dscp_color1	:2;
	uint32 dscp_priority0	:6;
	uint32 dscp_color0	:2;
};
typedef struct ipe_classification_dscp_map_table_s ipe_classification_dscp_map_table_t;

struct ipe_classification_cos_map_table_s	/* 86 */
{

	uint32 cos_priority3	:6;
	uint32 cos_color3	:2;
	uint32 cos_priority2	:6;
	uint32 cos_color2	:2;
	uint32 cos_priority1	:6;
	uint32 cos_color1	:2;
	uint32 cos_priority0	:6;
	uint32 cos_color0	:2;
};
typedef struct ipe_classification_cos_map_table_s ipe_classification_cos_map_table_t;

struct ipe_classification_precedence_map_table_s	/* 87 */
{

	uint32 pre_priority3	:6;
	uint32 pre_color3	:2;
	uint32 pre_priority2	:6;
	uint32 pre_color2	:2;
	uint32 pre_priority1	:6;
	uint32 pre_color1	:2;
	uint32 pre_priority0	:6;
	uint32 pre_color0	:2;
};
typedef struct ipe_classification_precedence_map_table_s ipe_classification_precedence_map_table_t;

struct ipe_mpls_exp_map_table_s	/* 88 */
{

	uint32 mpls_priority3	:6;
	uint32 mpls_color3	:2;
	uint32 mpls_priority2	:6;
	uint32 mpls_color2	:2;
	uint32 mpls_priority1	:6;
	uint32 mpls_color1	:2;
	uint32 mpls_priority0	:6;
	uint32 mpls_color0	:2;
};
typedef struct ipe_mpls_exp_map_table_s ipe_mpls_exp_map_table_t;

struct ipe_statsramipephbintf_s	/* 89 */
{

	uint32 rsv_0	:28;
	uint32 parity0_ipe_phb_intf	:1;
	uint32 use_l3_length_ipe_phb_intf	:1;
	uint32 byte_count36_to35_ipe_phb_intf	:2;

	uint32 packet_count31_to0_ipe_phb_intf	:32;

	uint32 rsv_1	:28;
	uint32 parity1_ipe_phb_intf	:1;
	uint32 byte_count34_to32_ipe_phb_intf	:3;

	uint32 byte_count31_to0_ipe_phb_intf	:32;
};
typedef struct ipe_statsramipephbintf_s ipe_statsramipephbintf_t;

struct ipe_statsramipeportlog_s	/* 90 */
{

	uint32 rsv_0	:28;
	uint32 parity0_ipe_port_log	:1;
	uint32 use_l3_length_ipe_port_log	:1;
	uint32 byte_count36_to35_ipe_port_log	:2;

	uint32 packet_count31_to0_ipe_port_log	:32;

	uint32 rsv_1	:28;
	uint32 parity1_ipe_port_log	:1;
	uint32 byte_count34_to32_ipe_port_log	:3;

	uint32 byte_count31_to0_ipe_port_log	:32;
};
typedef struct ipe_statsramipeportlog_s ipe_statsramipeportlog_t;

struct ipe_statsramipeoverallfwd_s	/* 91 */
{

	uint32 rsv_0	:28;
	uint32 parity0_ipe_overall_fwd	:1;
	uint32 use_l3_length_ipe_overall_fwd	:1;
	uint32 byte_count36_to35_ipe_overall_fwd	:2;

	uint32 packet_count31_to0_ipe_overall_fwd	:32;

	uint32 rsv_1	:28;
	uint32 parity1_ipe_overall_fwd	:1;
	uint32 byte_count34_to32_ipe_overall_fwd	:3;

	uint32 byte_count31_to0_ipe_overall_fwd	:32;
};
typedef struct ipe_statsramipeoverallfwd_s ipe_statsramipeoverallfwd_t;

struct met_fifo_rcd_ram_s	/* 92 */
{

	uint32 rsv_0	:24;
	uint32 rcd	:8;
};
typedef struct met_fifo_rcd_ram_s met_fifo_rcd_ram_t;

struct met_fifo_msg_ram_s	/* 93 */
{

	uint32 rsv_0	:1;
	uint32 data0	:31;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;

	uint32 data4	:32;

	uint32 data5	:32;

	uint32 rsv_1	:32;

	uint32 rsv_2	:32;
};
typedef struct met_fifo_msg_ram_s met_fifo_msg_ram_t;

struct ds_met_fifo_excp_s	/* 94 */
{

	uint32 rsv_0	:7;
	uint32 length_adjust_type	:1;
	uint32 next_hop_ext	:1;
	uint32 exception_sub_index_en	:1;
	uint32 dest_map	:22;
};
typedef struct ds_met_fifo_excp_s ds_met_fifo_excp_t;

struct ds_aps_bridge_mcast_s	/* 95 */
{

	uint32 protecting_en	:1;
	uint32 rsv_0	:3;
	uint32 working_ucast_id	:12;
	uint32 rsv_1	:4;
	uint32 protecting_ucast_id	:12;
};
typedef struct ds_aps_bridge_mcast_s ds_aps_bridge_mcast_t;

struct ds_link_agg_block_mask_s	/* 96 */
{

	uint32 mask_hi	:32;

	uint32 mask_lo	:32;
};
typedef struct ds_link_agg_block_mask_s ds_link_agg_block_mask_t;

struct ds_link_agg_bitmap_s	/* 97 */
{

	uint32 rsv_0	:31;
	uint32 block_mask_en	:1;

	uint32 member_hi	:32;

	uint32 member_lo	:32;

	uint32 rsv_1	:32;
};
typedef struct ds_link_agg_bitmap_s ds_link_agg_bitmap_t;

struct net_rx_channel_info_ram_s	/* 98 */
{

	uint32 rsv_0	:24;
	uint32 channel_info_data_state	:1;
	uint32 channel_info_data_sob_state	:1;
	uint32 channel_info_data_bpdu_state	:1;
	uint32 channel_info_data_data_error_seen	:1;
	uint32 channel_info_data_no_sop_error_seen	:1;
	uint32 channel_info_data_entry_offset	:1;
	uint32 channel_info_data_buf_cnt	:2;

	uint32 channel_info_data_pkt_len	:14;
	uint32 channel_info_data_head_ptr	:9;
	uint32 channel_info_data_curr_ptr	:9;
};
typedef struct net_rx_channel_info_ram_s net_rx_channel_info_ram_t;

struct net_rx_link_list_table_s	/* 99 */
{

	uint32 rsv_0	:23;
	uint32 link_list_table_word	:9;
};
typedef struct net_rx_link_list_table_s net_rx_link_list_table_t;

struct net_rx_pkt_buf_ram_s	/* 100 */
{

	uint32 pkt_buf_word0	:32;

	uint32 pkt_buf_word1	:32;

	uint32 pkt_buf_word2	:32;

	uint32 pkt_buf_word3	:32;

	uint32 pkt_buf_word4	:32;

	uint32 pkt_buf_word5	:32;

	uint32 pkt_buf_word6	:32;

	uint32 pkt_buf_word7	:32;

	uint32 pkt_buf_word8	:32;

	uint32 pkt_buf_word9	:32;

	uint32 pkt_buf_word10	:32;

	uint32 pkt_buf_word11	:32;

	uint32 pkt_buf_word12	:32;

	uint32 pkt_buf_word13	:32;

	uint32 pkt_buf_word14	:32;

	uint32 pkt_buf_word15	:32;
};
typedef struct net_rx_pkt_buf_ram_s net_rx_pkt_buf_ram_t;

struct pkt_mem_s	/* 101 */
{

	uint32 rsv_0	:19;
	uint32 data_info	:13;

	uint32 pkt_info	:32;

	uint32 data0	:32;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;

	uint32 data4	:32;

	uint32 data5	:32;

	uint32 data6	:32;

	uint32 data7	:32;

	uint32 data8	:32;

	uint32 data9	:32;

	uint32 data10	:32;

	uint32 data11	:32;

	uint32 data12	:32;

	uint32 data13	:32;

	uint32 data14	:32;

	uint32 data15	:32;

	uint32 rsv_1	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:32;

	uint32 rsv_8	:32;

	uint32 rsv_9	:32;

	uint32 rsv_10	:32;

	uint32 rsv_11	:32;

	uint32 rsv_12	:32;

	uint32 rsv_13	:32;

	uint32 rsv_14	:32;
};
typedef struct pkt_mem_s pkt_mem_t;

struct ch_static_info_regs_s	/* 102 */
{

	uint32 rsv_0	:5;
	uint32 start_ptr	:11;
	uint32 rsv_1	:5;
	uint32 end_ptr	:11;

	uint32 rsv_2	:8;
	uint32 threshold	:8;
	uint32 rsv_3	:8;
	uint32 fifo_depth	:8;
};
typedef struct ch_static_info_regs_s ch_static_info_regs_t;

struct ch_dynamic_info_regs_s	/* 103 */
{

	uint32 rsv_0	:24;
	uint32 data_unit_cnt	:8;

	uint32 rsv_1	:24;
	uint32 last_pkt_unit_cnt	:8;

	uint32 rd_state	:1;
	uint32 rsv_2	:20;
	uint32 rd_ptr	:11;

	uint32 wr_state	:1;
	uint32 rsv_3	:20;
	uint32 wr_ptr	:11;
};
typedef struct ch_dynamic_info_regs_s ch_dynamic_info_regs_t;

struct calendar_ctl_s	/* 104 */
{

	uint32 rsv_0	:26;
	uint32 cal_entry	:6;
};
typedef struct calendar_ctl_s calendar_ctl_t;

struct oam_ds_oam_excp_s	/* 105 */
{

	uint32 rsv_0	:12;
	uint32 next_hop_ptr	:20;
};
typedef struct oam_ds_oam_excp_s oam_ds_oam_excp_t;

struct ds_mep_chan_table_s	/* 106 */
{

	uint32 rsv_0	:28;
	uint32 data0	:4;

	uint32 data1	:32;

	uint32 rsv_1	:28;
	uint32 data2	:4;

	uint32 data3	:32;

	uint32 rsv_2	:28;
	uint32 data4	:4;

	uint32 data5	:32;

	uint32 rsv_3	:28;
	uint32 data6	:4;

	uint32 data7	:32;
};
typedef struct ds_mep_chan_table_s ds_mep_chan_table_t;

struct oam_ds_ma_s	/* 107 */
{

	uint32 rsv_0	:29;
	uint32 ma_id_type	:2;
	uint32 tx_untagged_oam	:1;

	uint32 oam_tunnel_disable	:1;
	uint32 ma_name_index	:15;
	uint32 aps_en	:1;
	uint32 priority_index	:3;
	uint32 defect_priority	:6;
	uint32 ma_id_len	:6;

	uint32 rsv_1	:29;
	uint32 ccm_interval	:3;

	uint32 tx_with_intf_status	:1;
	uint32 md_lvl	:3;
	uint32 port_status	:2;
	uint32 aps_signal_fail_local	:1;
	uint32 aps_signal_fail_remote	:1;
	uint32 intf_status	:3;
	uint32 tx_with_port_status	:1;
	uint32 next_hop_ptr	:20;
};
typedef struct oam_ds_ma_s oam_ds_ma_t;

struct oam_ds_ma_name_s	/* 108 */
{

	uint32 rsv_0	:29;
	uint32 ma_id_icc_index_hi	:3;

	uint32 ma_id_umc0	:32;

	uint32 rsv_1	:29;
	uint32 ma_id_icc_index_lo	:3;

	uint32 ma_id_umc1	:32;
};
typedef struct oam_ds_ma_name_s oam_ds_ma_name_t;

struct oam_ds_icc_s	/* 109 */
{

	uint32 rsv_0	:16;
	uint32 icc_hi	:16;

	uint32 icc_lo	:32;
};
typedef struct oam_ds_icc_s oam_ds_icc_t;

struct oam_ds_port_property_s	/* 110 */
{

	uint32 rsv_0	:2;
	uint32 global_src_port	:14;
	uint32 rsv_1	:8;
	uint32 mac_sa_byte	:8;
};
typedef struct oam_ds_port_property_s oam_ds_port_property_t;

struct oam_ds_defect_priority_s	/* 111 */
{

	uint32 rsv_0	:26;
	uint32 defect_priority	:6;
};
typedef struct oam_ds_defect_priority_s oam_ds_defect_priority_t;

struct oam_err_cache_s	/* 112 */
{

	uint32 rsv_0	:2;
	uint32 is_mpls_err_cache0	:1;
	uint32 defect_type_err_cache0	:3;
	uint32 defect_sub_type_err_cache0	:3;
	uint32 port_status_valid_err_cache0	:1;
	uint32 port_status_value_err_cache0	:2;
	uint32 intf_status_valid_err_cache0	:1;
	uint32 intf_status_value_err_cache0	:3;
	uint32 rsv_1	:10;
	uint32 defect_priority_err_cache0	:6;

	uint32 rsv_2	:1;
	uint32 rmep_index_err_cache0	:15;
	uint32 rsv_3	:1;
	uint32 mep_index_err_cache0	:15;
};
typedef struct oam_err_cache_s oam_err_cache_t;

struct pktbuf_s	/* 113 */
{

	uint32 data0	:32;

	uint32 data1	:32;

	uint32 data2	:32;

	uint32 data3	:32;

	uint32 data4	:32;

	uint32 data5	:32;

	uint32 data6	:32;

	uint32 data7	:32;

	uint32 data8	:32;

	uint32 data9	:32;

	uint32 data10	:32;

	uint32 data11	:32;

	uint32 data12	:32;

	uint32 data13	:32;

	uint32 data14	:32;

	uint32 data15	:32;
};
typedef struct pktbuf_s pktbuf_t;

struct intprofileram_s	/* 117 */
{

	uint32 rsv_0	:28;
	uint32 data71_to68	:4;

	uint32 data67_to36	:32;

	uint32 rsv_1	:28;
	uint32 data35_to32	:4;

	uint32 data31_to0	:32;
};
typedef struct intprofileram_s intprofileram_t;

struct extprofileram_s	/* 118 */
{

	uint32 rsv_0	:28;
	uint32 data71_to68	:4;

	uint32 data67_to36	:32;

	uint32 rsv_1	:28;
	uint32 data35_to32	:4;

	uint32 data31_to0	:32;
};
typedef struct extprofileram_s extprofileram_t;

struct ram_bist_qdr_s	/* 121 */
{

	uint32 rsv_0	:1;
	uint32 ram_access_valid	:1;
	uint32 ram_access_read	:1;
	uint32 ram_access_write	:1;
	uint32 ram_read_address	:20;
	uint32 rsv_1	:4;
	uint32 ram_data71_to68	:4;

	uint32 ram_data67_to36	:32;

	uint32 rsv_2	:4;
	uint32 ram_write_address	:20;
	uint32 rsv_3	:4;
	uint32 ram_data35_to32	:4;

	uint32 ram_data31_to0	:32;
};
typedef struct ram_bist_qdr_s ram_bist_qdr_t;

struct ram_qdr_s	/* 122 */
{

	uint32 rsv_0	:28;
	uint32 ram_data71_to68	:4;

	uint32 ram_data67_to36	:32;

	uint32 rsv_1	:28;
	uint32 ram_data35_to32	:4;

	uint32 ram_data31_to0	:32;
};
typedef struct ram_qdr_s ram_qdr_t;

struct ds_queue_ipg_index_s	/* 123 */
{

	uint32 rsv_0	:16;
	uint32 ipg_index7	:2;
	uint32 ipg_index6	:2;
	uint32 ipg_index5	:2;
	uint32 ipg_index4	:2;
	uint32 ipg_index3	:2;
	uint32 ipg_index2	:2;
	uint32 ipg_index1	:2;
	uint32 ipg_index0	:2;
};
typedef struct ds_queue_ipg_index_s ds_queue_ipg_index_t;

struct ds_queue_depth_s	/* 124 */
{

	uint32 queue_avg_depth	:16;
	uint32 queue_inst_depth	:16;
};
typedef struct ds_queue_depth_s ds_queue_depth_t;

struct ds_link_aggregation_s	/* 125 */
{

	uint32 rsv_0	:11;
	uint32 dest_chip_id	:5;
	uint32 rsv_1	:8;
	uint32 dest_queue	:8;
};
typedef struct ds_link_aggregation_s ds_link_aggregation_t;

struct ds_queue_drop_profile_id_s	/* 126 */
{

	uint32 rsv_0	:3;
	uint32 force_random_drop0	:1;
	uint32 que_depth_wt0	:4;
	uint32 prof_id0	:8;
	uint32 rsv_1	:3;
	uint32 force_random_drop1	:1;
	uint32 que_depth_wt1	:4;
	uint32 prof_id1	:8;

	uint32 rsv_2	:3;
	uint32 force_random_drop2	:1;
	uint32 que_depth_wt2	:4;
	uint32 prof_id2	:8;
	uint32 rsv_3	:3;
	uint32 force_random_drop3	:1;
	uint32 que_depth_wt3	:4;
	uint32 prof_id3	:8;
};
typedef struct ds_queue_drop_profile_id_s ds_queue_drop_profile_id_t;

struct ds_queue_drop_profile_s	/* 127 */
{

	uint32 wred_min_thrd0	:16;
	uint32 wred_max_thrd0	:16;

	uint32 wred_min_thrd1	:16;
	uint32 wred_max_thrd1	:16;

	uint32 wred_min_thrd2	:16;
	uint32 wred_max_thrd2	:16;

	uint32 wred_min_thrd3	:16;
	uint32 wred_max_thrd3	:16;

	uint32 rsv_0	:4;
	uint32 factor0	:4;
	uint32 rsv_1	:4;
	uint32 factor1	:4;
	uint32 rsv_2	:4;
	uint32 factor2	:4;
	uint32 rsv_3	:4;
	uint32 factor3	:4;

	uint32 rsv_4	:31;
	uint32 wred_drop_mode	:1;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;
};
typedef struct ds_queue_drop_profile_s ds_queue_drop_profile_t;

struct ds_queue_num_gen_ctl_s	/* 128 */
{

	uint32 que_sel_shift	:4;
	uint32 dest_chip_id_shift	:4;
	uint32 rsv_0	:3;
	uint32 service_que_en	:1;
	uint32 dest_que_shift	:4;
	uint32 rsv_1	:2;
	uint32 que_sel_mask	:6;
	uint32 rsv_2	:3;
	uint32 dest_chip_id_mask	:5;

	uint32 rsv_3	:16;
	uint32 dest_que_mask	:16;

	uint32 sgmac_shift	:4;
	uint32 sgmac_mask	:2;
	uint32 sgmac_base	:2;
	uint32 rsv_4	:3;
	uint32 dest_chip_id_base	:5;
	uint32 rsv_5	:3;
	uint32 que_num_base	:13;

	uint32 dest_que_base	:16;
	uint32 rsv_6	:16;

	uint32 flow_id_shift	:4;
	uint32 rsv_7	:4;
	uint32 flow_id_mask	:8;
	uint32 rsv_8	:8;
	uint32 flow_id_base	:8;

	uint32 rsv_9	:32;

	uint32 rsv_10	:32;

	uint32 rsv_11	:32;
};
typedef struct ds_queue_num_gen_ctl_s ds_queue_num_gen_ctl_t;

struct dsq_mgr_egress_resrc_threshold_s	/* 129 */
{

	uint32 rsv_0	:16;
	uint32 pri0_que_entry_thrd	:16;

	uint32 rsv_1	:16;
	uint32 pri1_que_entry_thrd	:16;

	uint32 rsv_2	:16;
	uint32 pri2_que_entry_thrd	:16;

	uint32 rsv_3	:16;
	uint32 pri3_que_entry_thrd	:16;
};
typedef struct dsq_mgr_egress_resrc_threshold_s dsq_mgr_egress_resrc_threshold_t;

struct dsq_mgr_egress_resrc_count_s	/* 130 */
{

	uint32 rsv_0	:16;
	uint32 egress_que_entry_count	:16;
};
typedef struct dsq_mgr_egress_resrc_count_s dsq_mgr_egress_resrc_count_t;

struct ds_link_agg_member_num_s	/* 131 */
{

	uint32 rsv_0	:23;
	uint32 hash_mode	:1;
	uint32 rsv_1	:3;
	uint32 link_agg_mem_num	:5;
};
typedef struct ds_link_agg_member_num_s ds_link_agg_member_num_t;

struct ds_sgmac_map_s	/* 132 */
{

	uint32 sgmac_trunk_grp_mode7	:1;
	uint32 sgmac_override7	:1;
	uint32 sgmac7	:2;
	uint32 sgmac_trunk_grp_mode6	:1;
	uint32 sgmac_override6	:1;
	uint32 sgmac6	:2;
	uint32 sgmac_trunk_grp_mode5	:1;
	uint32 sgmac_override5	:1;
	uint32 sgmac5	:2;
	uint32 sgmac_trunk_grp_mode4	:1;
	uint32 sgmac_override4	:1;
	uint32 sgmac4	:2;
	uint32 sgmac_trunk_grp_mode3	:1;
	uint32 sgmac_override3	:1;
	uint32 sgmac3	:2;
	uint32 sgmac_trunk_grp_mode2	:1;
	uint32 sgmac_override2	:1;
	uint32 sgmac2	:2;
	uint32 sgmac_trunk_grp_mode1	:1;
	uint32 sgmac_override1	:1;
	uint32 sgmac1	:2;
	uint32 sgmac_trunk_grp_mode0	:1;
	uint32 sgmac_override0	:1;
	uint32 sgmac0	:2;
};
typedef struct ds_sgmac_map_s ds_sgmac_map_t;

struct ds_head_hash_mod_s	/* 133 */
{

	uint32 rsv_0	:21;
	uint32 mod3	:2;
	uint32 mod5	:3;
	uint32 mod6	:3;
	uint32 mod7	:3;

	uint32 rsv_1	:4;
	uint32 mod9	:4;
	uint32 mod10	:4;
	uint32 mod11	:4;
	uint32 mod12	:4;
	uint32 mod13	:4;
	uint32 mod14	:4;
	uint32 mod15	:4;
};
typedef struct ds_head_hash_mod_s ds_head_hash_mod_t;

struct ds_service_queue_hash_key_s	/* 134 */
{

	uint32 rsv_0	:6;
	uint32 dest_id0	:10;
	uint32 service_id0	:16;

	uint32 rsv_1	:6;
	uint32 dest_id1	:10;
	uint32 service_id1	:16;

	uint32 rsv_2	:6;
	uint32 dest_id2	:10;
	uint32 service_id2	:16;

	uint32 rsv_3	:6;
	uint32 dest_id3	:10;
	uint32 service_id3	:16;
};
typedef struct ds_service_queue_hash_key_s ds_service_queue_hash_key_t;

struct ds_service_queue_s	/* 135 */
{

	uint32 rsv_0	:20;
	uint32 service_queue_base_offset	:12;
};
typedef struct ds_service_queue_s ds_service_queue_t;

struct q_mgrq_hash_cam_hash_ctl_s	/* 136 */
{

	uint32 rsv_0	:20;
	uint32 service_que_base_offset	:12;
};
typedef struct q_mgrq_hash_cam_hash_ctl_s q_mgrq_hash_cam_hash_ctl_t;

struct dsq_mgrq_link_list_s	/* 137 */
{

	uint32 rsv_0	:2;
	uint32 pkt_length	:14;
	uint32 rsv_1	:1;
	uint32 next_ptr	:15;

	uint32 rsv_2	:20;
	uint32 rep_count	:12;
};
typedef struct dsq_mgrq_link_list_s dsq_mgrq_link_list_t;

struct dsq_mgrq_link_state_s	/* 138 */
{

	uint32 rsv_0	:1;
	uint32 queue_empty	:1;
	uint32 pkt_length	:14;
	uint32 rsv_1	:1;
	uint32 head_ptr	:15;

	uint32 rsv_2	:4;
	uint32 rep_count	:12;
	uint32 rsv_3	:1;
	uint32 tail_ptr	:15;
};
typedef struct dsq_mgrq_link_state_s dsq_mgrq_link_state_t;

struct ds_queue_shape_s	/* 139 */
{

	uint32 rsv_0	:8;
	uint32 commit_token	:24;

	uint32 rsv_1	:8;
	uint32 peak_token	:24;
};
typedef struct ds_queue_shape_s ds_queue_shape_t;

struct ds_queue_drr_deficit_s	/* 140 */
{

	uint32 rsv_0	:5;
	uint32 deficit	:27;
};
typedef struct ds_queue_drr_deficit_s ds_queue_drr_deficit_t;

struct ds_in_profile_next_queue_ptr_s	/* 141 */
{

	uint32 in_prof_next_que_shp_en	:1;
	uint32 rsv_0	:6;
	uint32 in_prof_next_grp_id	:9;
	uint32 rsv_1	:5;
	uint32 in_prof_next_que_ptr	:11;
};
typedef struct ds_in_profile_next_queue_ptr_s ds_in_profile_next_queue_ptr_t;

struct ds_out_profile_next_queue_ptr_s	/* 142 */
{

	uint32 out_prof_next_que_shp_en	:1;
	uint32 rsv_0	:6;
	uint32 out_prof_next_grp_id	:9;
	uint32 rsv_1	:5;
	uint32 out_prof_next_que_ptr	:11;
};
typedef struct ds_out_profile_next_queue_ptr_s ds_out_profile_next_queue_ptr_t;

struct ds_queue_drr_weight_s	/* 143 */
{

	uint32 rsv_0	:8;
	uint32 weight	:24;
};
typedef struct ds_queue_drr_weight_s ds_queue_drr_weight_t;

struct ds_queue_map_s	/* 144 */
{

	uint32 rsv_0	:3;
	uint32 que_shp_en	:1;
	uint32 rsv_1	:2;
	uint32 priority_id	:2;
	uint32 channel_id	:8;
	uint32 rsv_2	:1;
	uint32 sgmac_valid	:1;
	uint32 sgmac_id	:2;
	uint32 rsv_3	:3;
	uint32 grp_id	:9;
};
typedef struct ds_queue_map_s ds_queue_map_t;

struct ds_channel_link_state_s	/* 145 */
{

	uint32 rsv_0	:7;
	uint32 in_prof_grp_id0	:9;
	uint32 rsv_1	:7;
	uint32 in_prof_grp_id1	:9;

	uint32 rsv_2	:5;
	uint32 in_prof_head_que0	:11;
	uint32 rsv_3	:5;
	uint32 in_prof_tail_que0	:11;

	uint32 rsv_4	:5;
	uint32 in_prof_head_que1	:11;
	uint32 rsv_5	:5;
	uint32 in_prof_tail_que1	:11;

	uint32 rsv_6	:23;
	uint32 in_prof_sub_ch_state1	:1;
	uint32 rsv_7	:3;
	uint32 in_prof_que_shp_en1	:1;
	uint32 rsv_8	:3;
	uint32 in_prof_que_shp_en0	:1;

	uint32 rsv_9	:7;
	uint32 out_prof_grp_id0	:9;
	uint32 rsv_10	:7;
	uint32 out_prof_grp_id1	:9;

	uint32 rsv_11	:5;
	uint32 out_prof_head_que0	:11;
	uint32 rsv_12	:5;
	uint32 out_prof_tail_que0	:11;

	uint32 rsv_13	:5;
	uint32 out_prof_head_que1	:11;
	uint32 rsv_14	:5;
	uint32 out_prof_tail_que1	:11;

	uint32 rsv_15	:23;
	uint32 out_prof_sub_ch_state1	:1;
	uint32 rsv_16	:3;
	uint32 out_prof_que_shp_en1	:1;
	uint32 rsv_17	:3;
	uint32 out_prof_que_shp_en0	:1;
};
typedef struct ds_channel_link_state_s ds_channel_link_state_t;

struct ds_queue_state_s	/* 146 */
{

	uint32 que_not_empty	:16;
	uint32 que_in_in_prof_list	:16;

	uint32 rsv_0	:16;
	uint32 que_in_out_prof_list	:16;
};
typedef struct ds_queue_state_s ds_queue_state_t;

struct ds_queue_shape_state_s	/* 147 */
{

	uint32 que_shp_ok	:32;
};
typedef struct ds_queue_shape_state_s ds_queue_shape_state_t;

struct ds_group_shape_profile_id_s	/* 148 */
{

	uint32 grp_shp_prof_id0	:8;
	uint32 grp_shp_prof_id1	:8;
	uint32 grp_shp_prof_id2	:8;
	uint32 grp_shp_prof_id3	:8;
};
typedef struct ds_group_shape_profile_id_s ds_group_shape_profile_id_t;

struct ds_group_shape_state_s	/* 149 */
{

	uint32 rsv_0	:19;
	uint32 grp_shp_ok3	:1;
	uint32 rsv_1	:3;
	uint32 grp_shp_ok2	:1;
	uint32 rsv_2	:3;
	uint32 grp_shp_ok1	:1;
	uint32 rsv_3	:3;
	uint32 grp_shp_ok0	:1;
};
typedef struct ds_group_shape_state_s ds_group_shape_state_t;

struct ds_group_shape_profile_s	/* 150 */
{

	uint32 rsv_0	:10;
	uint32 grp_token_rate	:22;

	uint32 rsv_1	:20;
	uint32 grp_token_thrd_shift	:4;
	uint32 grp_token_thrd	:8;
};
typedef struct ds_group_shape_profile_s ds_group_shape_profile_t;

struct ds_group_context_s	/* 151 */
{

	uint32 rsv_0	:27;
	uint32 valid	:1;
	uint32 rsv_1	:2;
	uint32 priority	:2;
};
typedef struct ds_group_context_s ds_group_context_t;

struct ds_queue_shape_profile_id_s	/* 152 */
{

	uint32 que_shp_prof_id0	:8;
	uint32 que_shp_prof_id1	:8;
	uint32 que_shp_prof_id2	:8;
	uint32 que_shp_prof_id3	:8;
};
typedef struct ds_queue_shape_profile_id_s ds_queue_shape_profile_id_t;

struct ds_queue_shape_profile_s	/* 153 */
{

	uint32 rsv_0	:10;
	uint32 que_commit_token_rate	:22;

	uint32 rsv_1	:20;
	uint32 que_commit_token_thrd_shift	:4;
	uint32 que_commit_token_thrd	:8;

	uint32 rsv_2	:10;
	uint32 que_peak_token_rate	:22;

	uint32 rsv_3	:20;
	uint32 que_peak_token_thrd_shift	:4;
	uint32 que_peak_token_thrd	:8;
};
typedef struct ds_queue_shape_profile_s ds_queue_shape_profile_t;

struct ds_group_shape_s	/* 154 */
{

	uint32 rsv_0	:8;
	uint32 grp_token	:24;
};
typedef struct ds_group_shape_s ds_group_shape_t;

struct ds_group_cache_s	/* 155 */
{

	uint32 rsv_0	:7;
	uint32 valid0	:1;
	uint32 rsv_1	:3;
	uint32 cir0	:1;
	uint32 rsv_2	:2;
	uint32 pri_id0	:2;
	uint32 rsv_3	:5;
	uint32 que_id0	:11;

	uint32 rsv_4	:7;
	uint32 valid1	:1;
	uint32 rsv_5	:3;
	uint32 cir1	:1;
	uint32 rsv_6	:2;
	uint32 pri_id1	:2;
	uint32 rsv_7	:5;
	uint32 que_id1	:11;

	uint32 rsv_8	:7;
	uint32 valid2	:1;
	uint32 rsv_9	:3;
	uint32 cir2	:1;
	uint32 rsv_10	:2;
	uint32 pri_id2	:2;
	uint32 rsv_11	:5;
	uint32 que_id2	:11;

	uint32 rsv_12	:7;
	uint32 valid3	:1;
	uint32 rsv_13	:3;
	uint32 cir3	:1;
	uint32 rsv_14	:2;
	uint32 pri_id3	:2;
	uint32 rsv_15	:5;
	uint32 que_id3	:11;
};
typedef struct ds_group_cache_s ds_group_cache_t;

struct ds_channel_credit_s	/* 160 */
{

	uint32 rsv_0	:22;
	uint32 ch_credit	:10;
};
typedef struct ds_channel_credit_s ds_channel_credit_t;

struct ds_channel_shape_profile_s	/* 161 */
{

	uint32 rsv_0	:10;
	uint32 token_rate	:22;

	uint32 rsv_1	:15;
	uint32 shape_en	:1;
	uint32 rsv_2	:4;
	uint32 token_thrd_shift	:4;
	uint32 token_thrd	:8;
};
typedef struct ds_channel_shape_profile_s ds_channel_shape_profile_t;

struct ds_channel_shape_s	/* 162 */
{

	uint32 rsv_0	:8;
	uint32 token	:24;
};
typedef struct ds_channel_shape_s ds_channel_shape_t;

struct ds_ch_credit_s	/* 163 */
{

	uint32 rsv_0	:22;
	uint32 ch_credit	:10;
};
typedef struct ds_ch_credit_s ds_ch_credit_t;

struct ds_fabric_wrr_weight_cfg_s	/* 164 */
{

	uint32 rsv_0	:24;
	uint32 wt_cfg	:8;
};
typedef struct ds_fabric_wrr_weight_cfg_s ds_fabric_wrr_weight_cfg_t;

struct ds_fabric_wrr_weight_s	/* 165 */
{

	uint32 rsv_0	:24;
	uint32 wt	:8;
};
typedef struct ds_fabric_wrr_weight_s ds_fabric_wrr_weight_t;

struct ds_network_wrr_weight_cfg_s	/* 166 */
{

	uint32 rsv_0	:24;
	uint32 wt_cfg	:8;
};
typedef struct ds_network_wrr_weight_cfg_s ds_network_wrr_weight_cfg_t;

struct q_mgr_network_out_profile_wrr_weight_cfg_s	/* 167 */
{

	uint32 pri0_wt_cfg	:8;
	uint32 pri1_wt_cfg	:8;
	uint32 pri2_wt_cfg	:8;
	uint32 pri3_wt_cfg	:8;
};
typedef struct q_mgr_network_out_profile_wrr_weight_cfg_s q_mgr_network_out_profile_wrr_weight_cfg_t;

struct q_mgr_network_out_profile_wrr_weight_s	/* 168 */
{

	uint32 pri0_wt	:8;
	uint32 pri1_wt	:8;
	uint32 pri2_wt	:8;
	uint32 pri3_wt	:8;
};
typedef struct q_mgr_network_out_profile_wrr_weight_s q_mgr_network_out_profile_wrr_weight_t;

struct q_mgr_fabric_out_profile_wrr_weight_cfg_s	/* 169 */
{

	uint32 pri0_wt_cfg	:8;
	uint32 pri1_wt_cfg	:8;
	uint32 pri2_wt_cfg	:8;
	uint32 pri3_wt_cfg	:8;
};
typedef struct q_mgr_fabric_out_profile_wrr_weight_cfg_s q_mgr_fabric_out_profile_wrr_weight_cfg_t;

struct q_mgr_fabric_out_profile_wrr_weight_s	/* 170 */
{

	uint32 pri0_wt	:8;
	uint32 pri1_wt	:8;
	uint32 pri2_wt	:8;
	uint32 pri3_wt	:8;
};
typedef struct q_mgr_fabric_out_profile_wrr_weight_s q_mgr_fabric_out_profile_wrr_weight_t;

struct ds_network_wrr_weight_s	/* 171 */
{

	uint32 rsv_0	:24;
	uint32 wt	:8;
};
typedef struct ds_network_wrr_weight_s ds_network_wrr_weight_t;

struct ds_queue_entry_s	/* 172 */
{

	uint32 rsv_0	:28;
	uint32 ecc_bit7_to4	:4;

	uint32 pt_enable	:1;
	uint32 head_buffer_ptr	:15;
	uint32 rsv_1	:16;

	uint32 rsv_2	:28;
	uint32 ecc_bit3_to0	:4;

	uint32 header_version	:1;
	uint32 rsv_3	:1;
	uint32 buffer_count	:6;
	uint32 resource_group_id	:8;
	uint32 head_ptr_bank_offset	:2;
	uint32 packet_length	:14;

	uint32 rsv_4	:30;
	uint32 parity	:2;

	uint32 next_hop_ext	:1;
	uint32 length_adjust_type	:1;
	uint32 mcast_rcd	:1;
	uint32 dest_select	:1;
	uint32 rsv_5	:8;
	uint32 replication_ctl	:20;

	uint32 rsv_6	:32;

	uint32 rcd	:8;
	uint32 rsv_7	:2;
	uint32 dest_map	:22;
};
typedef struct ds_queue_entry_s ds_queue_entry_t;

struct quadmacapp0_stats_ram_s	/* 173 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp0_stats_ram_s quadmacapp0_stats_ram_t;

struct quadmacapp10_stats_ram_s	/* 174 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp10_stats_ram_s quadmacapp10_stats_ram_t;

struct quadmacapp11_stats_ram_s	/* 175 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp11_stats_ram_s quadmacapp11_stats_ram_t;

struct quadmacapp1_stats_ram_s	/* 176 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp1_stats_ram_s quadmacapp1_stats_ram_t;

struct quadmacapp2_stats_ram_s	/* 177 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp2_stats_ram_s quadmacapp2_stats_ram_t;

struct quadmacapp3_stats_ram_s	/* 178 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp3_stats_ram_s quadmacapp3_stats_ram_t;

struct quadmacapp4_stats_ram_s	/* 179 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp4_stats_ram_s quadmacapp4_stats_ram_t;

struct quadmacapp5_stats_ram_s	/* 180 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp5_stats_ram_s quadmacapp5_stats_ram_t;

struct quadmacapp6_stats_ram_s	/* 181 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp6_stats_ram_s quadmacapp6_stats_ram_t;

struct quadmacapp7_stats_ram_s	/* 182 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp7_stats_ram_s quadmacapp7_stats_ram_t;

struct quadmacapp8_stats_ram_s	/* 183 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp8_stats_ram_s quadmacapp8_stats_ram_t;

struct quadmacapp9_stats_ram_s	/* 184 */
{

	uint32 rsv_0	:28;
	uint32 frame_cnt_data_hi	:4;

	uint32 frame_cnt_data_lo	:32;

	uint32 rsv_1	:28;
	uint32 byte_cnt_data_hi	:4;

	uint32 byte_cnt_data_lo	:32;
};
typedef struct quadmacapp9_stats_ram_s quadmacapp9_stats_ram_t;

struct sgmac0_sgmac_stats_ram_s	/* 185 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct sgmac0_sgmac_stats_ram_s sgmac0_sgmac_stats_ram_t;

struct sgmac1_sgmac_stats_ram_s	/* 186 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct sgmac1_sgmac_stats_ram_s sgmac1_sgmac_stats_ram_t;

struct sgmac2_sgmac_stats_ram_s	/* 187 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct sgmac2_sgmac_stats_ram_s sgmac2_sgmac_stats_ram_t;

struct sgmac3_sgmac_stats_ram_s	/* 188 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct sgmac3_sgmac_stats_ram_s sgmac3_sgmac_stats_ram_t;

struct shared_ds_vlan_mem_s	/* 189 */
{

	uint32 rsv_0	:28;
	uint32 data0	:4;

	uint32 data1	:32;

	uint32 rsv_1	:28;
	uint32 data2	:4;

	uint32 data3	:32;
};
typedef struct shared_ds_vlan_mem_s shared_ds_vlan_mem_t;

struct ds_link_aggreagation_group_s	/* 190 */
{

	uint32 rsv_0	:24;
	uint32 link_aggregate_group_en	:1;
	uint32 link_aggregate_group_id	:7;
};
typedef struct ds_link_aggreagation_group_s ds_link_aggreagation_group_t;

struct stp_state_ram_s	/* 191 */
{

	uint32 stp_state15	:2;
	uint32 stp_state14	:2;
	uint32 stp_state13	:2;
	uint32 stp_state12	:2;
	uint32 stp_state11	:2;
	uint32 stp_state10	:2;
	uint32 stp_state9	:2;
	uint32 stp_state8	:2;
	uint32 stp_state7	:2;
	uint32 stp_state6	:2;
	uint32 stp_state5	:2;
	uint32 stp_state4	:2;
	uint32 stp_state3	:2;
	uint32 stp_state2	:2;
	uint32 stp_state1	:2;
	uint32 stp_state0	:2;
};
typedef struct stp_state_ram_s stp_state_ram_t;

struct tb_info_int_mem_s	/* 192 */
{

	uint32 rsv_0	:28;
	uint32 data0	:4;

	uint32 data1	:32;

	uint32 rsv_1	:28;
	uint32 data2	:4;

	uint32 data3	:32;
};
typedef struct tb_info_int_mem_s tb_info_int_mem_t;

struct tb_info_hash_mem_s	/* 193 */
{

	uint32 rsv_0	:28;
	uint32 data0	:4;

	uint32 data1	:32;

	uint32 rsv_1	:28;
	uint32 data2	:4;

	uint32 data3	:32;
};
typedef struct tb_info_hash_mem_s tb_info_hash_mem_t;

struct tb_info_ext_mem_s	/* 194 */
{

	uint32 rsv_0	:28;
	uint32 data0	:4;

	uint32 data1	:32;

	uint32 rsv_1	:28;
	uint32 data2	:4;

	uint32 data3	:32;
};
typedef struct tb_info_ext_mem_s tb_info_ext_mem_t;

struct ram_bist_tb_info_ext_ddr_s	/* 195 */
{

	uint32 rsv_0	:1;
	uint32 ram_access_valid	:1;
	uint32 ram_access_read	:1;
	uint32 ram_access_load	:1;
	uint32 ram_address	:20;
	uint32 rsv_1	:4;
	uint32 ram_data71_to68	:4;

	uint32 ram_data67_to36	:32;

	uint32 rsv_2	:28;
	uint32 ram_data35_to32	:4;

	uint32 ram_data31_to0	:32;
};
typedef struct ram_bist_tb_info_ext_ddr_s ram_bist_tb_info_ext_ddr_t;

struct tcam_ctl_ext_bist_request_mem_s	/* 196 */
{

	uint32 key_valid	:1;
	uint32 rsv_0	:18;
	uint32 key_inst	:3;
	uint32 key_size	:2;
	uint32 key_cmd	:8;

	uint32 rsv_1	:16;
	uint32 key159_to144	:16;

	uint32 key143_to112	:32;

	uint32 key111_to80	:32;

	uint32 link_training_valid	:1;
	uint32 rsv_2	:18;
	uint32 link_training_cmd	:13;

	uint32 rsv_3	:16;
	uint32 key79_to64	:16;

	uint32 key63_to32	:32;

	uint32 key31_to0	:32;
};
typedef struct tcam_ctl_ext_bist_request_mem_s tcam_ctl_ext_bist_request_mem_t;

struct tcam_ctl_ext_bist_result_mem_s	/* 197 */
{

	uint32 result_compare_valid	:1;
	uint32 rsv_0	:6;
	uint32 index_valid	:1;
	uint32 rsv_1	:3;
	uint32 index0_compare_en	:1;
	uint32 index0	:20;

	uint32 rsv_2	:11;
	uint32 index1_compare_en	:1;
	uint32 index1	:20;
};
typedef struct tcam_ctl_ext_bist_result_mem_s tcam_ctl_ext_bist_result_mem_t;

struct tcam_ctl_int_cpu_request_mem_s	/* 198 */
{

	uint32 key_valid	:1;
	uint32 rsv_0	:21;
	uint32 key_size	:2;
	uint32 key_cmd	:8;

	uint32 rsv_1	:16;
	uint32 key159_to144	:16;

	uint32 key143_to112	:32;

	uint32 key111_to80	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 key79_to64	:16;

	uint32 key63_to32	:32;

	uint32 key31_to0	:32;
};
typedef struct tcam_ctl_int_cpu_request_mem_s tcam_ctl_int_cpu_request_mem_t;

struct tcam_ctl_int_cpu_result_mem_s	/* 199 */
{

	uint32 result_compare_valid	:1;
	uint32 rsv_0	:6;
	uint32 index_valid	:1;
	uint32 rsv_1	:3;
	uint32 index_acl_comp_en	:1;
	uint32 index_acl	:20;

	uint32 rsv_2	:11;
	uint32 index_qos_comp_en	:1;
	uint32 index_qos	:20;
};
typedef struct tcam_ctl_int_cpu_result_mem_s tcam_ctl_int_cpu_result_mem_t;

struct xgmac0_xgmac_stats_ram_s	/* 200 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct xgmac0_xgmac_stats_ram_s xgmac0_xgmac_stats_ram_t;

struct xgmac1_xgmac_stats_ram_s	/* 201 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct xgmac1_xgmac_stats_ram_s xgmac1_xgmac_stats_ram_t;

struct xgmac2_xgmac_stats_ram_s	/* 202 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct xgmac2_xgmac_stats_ram_s xgmac2_xgmac_stats_ram_t;

struct xgmac3_xgmac_stats_ram_s	/* 203 */
{

	uint32 rsv_0	:24;
	uint32 frame_cnt_data_high	:8;

	uint32 frame_cnt_data_low	:32;

	uint32 rsv_1	:24;
	uint32 byte_cnt_data_high	:8;

	uint32 byte_cnt_data_low	:32;
};
typedef struct xgmac3_xgmac_stats_ram_s xgmac3_xgmac_stats_ram_t;

struct ds_mac_acl_s	/* 204 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_mac_acl_s ds_mac_acl_t;

struct ds_ipv4_acl_s	/* 205 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_ipv4_acl_s ds_ipv4_acl_t;

struct ds_mpls_acl_s	/* 206 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_mpls_acl_s ds_mpls_acl_t;

struct ds_ipv6_acl_s	/* 207 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_ipv6_acl_s ds_ipv6_acl_t;

struct ds_mac_qos_s	/* 208 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_mac_qos_s ds_mac_qos_t;

struct ds_ipv4_qos_s	/* 209 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_ipv4_qos_s ds_ipv4_qos_t;

struct ds_mpls_qos_s	/* 210 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_mpls_qos_s ds_mpls_qos_t;

struct ds_ipv6_qos_s	/* 211 */
{

	uint32 rsv_0	:29;
	uint32 stats_mode	:1;
	uint32 deny_replace_cos	:1;
	uint32 discard_packet	:1;

	uint32 fwd_ptr	:12;
	uint32 random_log_en	:1;
	uint32 priority_valid	:1;
	uint32 acl_log_id	:2;
	uint32 priority	:6;
	uint32 color	:2;
	uint32 random_threshold_shift	:4;
	uint32 deny_replace_dscp	:1;
	uint32 qos_policy	:3;

	uint32 rsv_1	:29;
	uint32 deny_bridge	:1;
	uint32 deny_learning	:1;
	uint32 deny_route	:1;

	uint32 stats_ptr	:16;
	uint32 flow_policer_ptr	:16;
};
typedef struct ds_ipv6_qos_s ds_ipv6_qos_t;

struct ds_ipv4_ucast_da_s	/* 212 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv4_ucast_da_s ds_ipv4_ucast_da_t;

struct ds_ipv4_mcast_da_s	/* 213 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv4_mcast_da_s ds_ipv4_mcast_da_t;

struct ds_ipv6_ucast_da_s	/* 214 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv6_ucast_da_s ds_ipv6_ucast_da_t;

struct ds_ipv6_mcast_da_s	/* 215 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv6_mcast_da_s ds_ipv6_mcast_da_t;

struct ds_ipv4_ucast_pbr_dual_da_s	/* 216 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv4_ucast_pbr_dual_da_s ds_ipv4_ucast_pbr_dual_da_t;

struct ds_ipv6_ucast_pbr_dual_da_s	/* 217 */
{

	uint32 rsv_0	:29;
	uint32 isatap_check_en	:1;
	uint32 icmp_check_en	:1;
	uint32 mcast_rpf_fail_cpu_en	:1;

	uint32 excep_sub_index	:4;
	uint32 bidi_pim_group_valid	:1;
	uint32 bidi_pim_group	:3;
	uint32 priority_path_en	:1;
	uint32 ip_da_exception_en	:1;
	uint32 ttl_check_en	:1;
	uint32 equal_cost_path_num2	:1;
	uint32 deny_pbr	:1;
	uint32 vpls_en	:1;
	uint32 equal_cost_path_num	:2;
	uint32 exp3_ctl_en	:1;
	uint32 vrf_id	:15;

	uint32 rsv_1	:29;
	uint32 tunnel_packet_type	:3;

	uint32 tunnel_payload_offset_type	:1;
	uint32 tunnel_gre_options	:3;
	uint32 tunnel_payload_offset	:4;
	uint32 l3_if_type	:2;
	uint32 payload_select	:2;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_ipv6_ucast_pbr_dual_da_s ds_ipv6_ucast_pbr_dual_da_t;

struct ds_ipv4_ucast_sa_s	/* 218 */
{

	uint32 rsv_0	:29;
	uint32 check_en	:1;
	uint32 if_id_valid3	:1;
	uint32 if_id_valid2	:1;

	uint32 if_id3	:16;
	uint32 if_id2	:16;

	uint32 rsv_1	:29;
	uint32 ipsa_more_rpf_if	:1;
	uint32 if_id_valid1	:1;
	uint32 if_id_valid0	:1;

	uint32 if_id1	:16;
	uint32 if_id0	:16;
};
typedef struct ds_ipv4_ucast_sa_s ds_ipv4_ucast_sa_t;

struct ds_ipv6_ucast_sa_s	/* 219 */
{

	uint32 rsv_0	:29;
	uint32 check_en	:1;
	uint32 if_id_valid3	:1;
	uint32 if_id_valid2	:1;

	uint32 if_id3	:16;
	uint32 if_id2	:16;

	uint32 rsv_1	:29;
	uint32 ipsa_more_rpf_if	:1;
	uint32 if_id_valid1	:1;
	uint32 if_id_valid0	:1;

	uint32 if_id1	:16;
	uint32 if_id0	:16;
};
typedef struct ds_ipv6_ucast_sa_s ds_ipv6_ucast_sa_t;

struct ds_ipv4_mcast_rpf_s	/* 220 */
{

	uint32 rsv_0	:29;
	uint32 check_en	:1;
	uint32 if_id_valid3	:1;
	uint32 if_id_valid2	:1;

	uint32 if_id3	:16;
	uint32 if_id2	:16;

	uint32 rsv_1	:29;
	uint32 ipsa_more_rpf_if	:1;
	uint32 if_id_valid1	:1;
	uint32 if_id_valid0	:1;

	uint32 if_id1	:16;
	uint32 if_id0	:16;
};
typedef struct ds_ipv4_mcast_rpf_s ds_ipv4_mcast_rpf_t;

struct ds_ipv6_mcast_rpf_s	/* 221 */
{

	uint32 rsv_0	:29;
	uint32 check_en	:1;
	uint32 if_id_valid3	:1;
	uint32 if_id_valid2	:1;

	uint32 if_id3	:16;
	uint32 if_id2	:16;

	uint32 rsv_1	:29;
	uint32 ipsa_more_rpf_if	:1;
	uint32 if_id_valid1	:1;
	uint32 if_id_valid0	:1;

	uint32 if_id1	:16;
	uint32 if_id0	:16;
};
typedef struct ds_ipv6_mcast_rpf_s ds_ipv6_mcast_rpf_t;

struct ds_ipv4_sa_nat_s	/* 222 */
{

	uint32 rsv_0	:29;
	uint32 force_ip_sa_fwd	:1;
	uint32 ip_sa_fwd_ptr_valid	:1;
	uint32 replace_l4_source_port	:1;

	uint32 ip_sa_prefix	:8;
	uint32 rsv_1	:2;
	uint32 ip_sa_mode	:2;
	uint32 ipv4_embed_mode	:1;
	uint32 ip_sa_prefix_len	:3;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:31;
	uint32 replace_ip_sa	:1;

	uint32 ip_sa	:32;
};
typedef struct ds_ipv4_sa_nat_s ds_ipv4_sa_nat_t;

struct ds_ipv6_sa_nat_s	/* 223 */
{

	uint32 rsv_0	:29;
	uint32 force_ip_sa_fwd	:1;
	uint32 ip_sa_fwd_ptr_valid	:1;
	uint32 replace_l4_source_port	:1;

	uint32 ip_sa_prefix	:8;
	uint32 rsv_1	:2;
	uint32 ip_sa_mode	:2;
	uint32 ipv4_embed_mode	:1;
	uint32 ip_sa_prefix_len	:3;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:31;
	uint32 replace_ip_sa	:1;

	uint32 ip_sa	:32;
};
typedef struct ds_ipv6_sa_nat_s ds_ipv6_sa_nat_t;

struct ds_user_id_vlan_s	/* 224 */
{

	uint32 rsv_0	:29;
	uint32 exception_en	:1;
	uint32 by_pass_all	:1;
	uint32 aps_select_valid	:1;

	uint32 src_queue_select	:1;
	uint32 vpls_port_type	:1;
	uint32 src_communicate_port	:1;
	uint32 user_vlan_ptr	:13;
	uint32 binding_datah	:16;

	uint32 rsv_1	:29;
	uint32 ds_fwd_ptr_valid	:1;
	uint32 binding_en	:1;
	uint32 binding_mac_sa	:1;

	uint32 binding_datam	:16;
	uint32 binding_datal	:16;
};
typedef struct ds_user_id_vlan_s ds_user_id_vlan_t;

struct ds_user_id_mac_s	/* 225 */
{

	uint32 rsv_0	:29;
	uint32 exception_en	:1;
	uint32 by_pass_all	:1;
	uint32 aps_select_valid	:1;

	uint32 src_queue_select	:1;
	uint32 vpls_port_type	:1;
	uint32 src_communicate_port	:1;
	uint32 user_vlan_ptr	:13;
	uint32 binding_datah	:16;

	uint32 rsv_1	:29;
	uint32 ds_fwd_ptr_valid	:1;
	uint32 binding_en	:1;
	uint32 binding_mac_sa	:1;

	uint32 binding_datam	:16;
	uint32 binding_datal	:16;
};
typedef struct ds_user_id_mac_s ds_user_id_mac_t;

struct ds_user_id_ipv4_s	/* 226 */
{

	uint32 rsv_0	:29;
	uint32 exception_en	:1;
	uint32 by_pass_all	:1;
	uint32 aps_select_valid	:1;

	uint32 src_queue_select	:1;
	uint32 vpls_port_type	:1;
	uint32 src_communicate_port	:1;
	uint32 user_vlan_ptr	:13;
	uint32 binding_datah	:16;

	uint32 rsv_1	:29;
	uint32 ds_fwd_ptr_valid	:1;
	uint32 binding_en	:1;
	uint32 binding_mac_sa	:1;

	uint32 binding_datam	:16;
	uint32 binding_datal	:16;
};
typedef struct ds_user_id_ipv4_s ds_user_id_ipv4_t;

struct ds_user_id_ipv6_s	/* 227 */
{

	uint32 rsv_0	:29;
	uint32 exception_en	:1;
	uint32 by_pass_all	:1;
	uint32 aps_select_valid	:1;

	uint32 src_queue_select	:1;
	uint32 vpls_port_type	:1;
	uint32 src_communicate_port	:1;
	uint32 user_vlan_ptr	:13;
	uint32 binding_datah	:16;

	uint32 rsv_1	:29;
	uint32 ds_fwd_ptr_valid	:1;
	uint32 binding_en	:1;
	uint32 binding_mac_sa	:1;

	uint32 binding_datam	:16;
	uint32 binding_datal	:16;
};
typedef struct ds_user_id_ipv6_s ds_user_id_ipv6_t;

struct ds_l2_edit_eth4w_s	/* 228 */
{

	uint32 rsv_0	:29;
	uint32 derive_mcast_mac	:1;
	uint32 overwrite_ether_type	:1;
	uint32 type	:1;

	uint32 output_cvlanid_valid	:1;
	uint32 packet_type	:3;
	uint32 output_vlan_id	:12;
	uint32 mac_dah	:16;

	uint32 rsv_1	:29;
	uint32 output_vlanid_is_svlan	:1;
	uint32 output_vlan_id_valid	:1;
	uint32 mac_sa_valid	:1;

	uint32 mac_dal	:32;
};
typedef struct ds_l2_edit_eth4w_s ds_l2_edit_eth4w_t;

struct ds_l2_edit_eth8w_s	/* 229 */
{

	uint32 rsv_0	:29;
	uint32 derive_mcast_mac	:1;
	uint32 overwrite_ether_type	:1;
	uint32 type	:1;

	uint32 output_cvlan_id_valid	:1;
	uint32 packet_type	:3;
	uint32 output_vlan_id	:12;
	uint32 mac_dah	:16;

	uint32 rsv_1	:29;
	uint32 output_vlanid_is_svlan	:1;
	uint32 output_vlan_id_valid	:1;
	uint32 mac_sa_valid	:1;

	uint32 mac_dal	:32;

	uint32 rsv_2	:29;
	uint32 output_cvlan_idh	:3;

	uint32 output_cvlan_idm	:6;
	uint32 rsv_3	:10;
	uint32 mac_sah	:16;

	uint32 rsv_4	:29;
	uint32 output_cvlan_idl	:3;

	uint32 mac_sal	:32;
};
typedef struct ds_l2_edit_eth8w_s ds_l2_edit_eth8w_t;

struct ds_l2_edit_flex4w_s	/* 230 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:1;
	uint32 rewrite_byte_num	:3;
	uint32 rsv_2	:1;
	uint32 packet_type	:3;
	uint32 rewrite_stringh	:24;

	uint32 rsv_3	:32;

	uint32 rewrite_stringl	:32;
};
typedef struct ds_l2_edit_flex4w_s ds_l2_edit_flex4w_t;

struct ds_l2_edit_flex8w_s	/* 231 */
{

	uint32 rsv_0	:32;

	uint32 rewrite_byte_num	:4;
	uint32 rsv_1	:1;
	uint32 packet_type	:3;
	uint32 rewrite_string3	:24;

	uint32 rsv_2	:32;

	uint32 rewrite_string2	:32;

	uint32 rsv_3	:32;

	uint32 rewrite_string1	:32;

	uint32 rsv_4	:32;

	uint32 rewrite_string0	:32;
};
typedef struct ds_l2_edit_flex8w_s ds_l2_edit_flex8w_t;

struct ds_l2_edit_loopback_s	/* 232 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:9;
	uint32 lb_length_adjust_type	:1;
	uint32 lb_dest_map	:22;

	uint32 rsv_2	:32;

	uint32 rsv_3	:11;
	uint32 lb_next_hop_ext	:1;
	uint32 lb_next_hop_ptr	:20;
};
typedef struct ds_l2_edit_loopback_s ds_l2_edit_loopback_t;

struct ds_l2_edit_pbb8w_s	/* 233 */
{

	uint32 rsv_0	:29;
	uint32 mac_sa_valid	:1;
	uint32 derive_mac_da	:1;
	uint32 isid_valid	:1;

	uint32 rsv_1	:11;
	uint32 bvlan_tagged	:1;
	uint32 copy_nca_res	:1;
	uint32 itag_cos	:3;
	uint32 derive_itag_cos	:1;
	uint32 itag_cfi	:1;
	uint32 pbb_header_op_type	:1;
	uint32 bvlan_tag_disable	:1;
	uint32 bvlan_id	:12;

	uint32 rsv_2	:29;
	uint32 derive_btag_cos	:1;
	uint32 mac_da_valid	:1;
	uint32 bvlan_valid	:1;

	uint32 btag_cfi	:1;
	uint32 btag_cos	:3;
	uint32 nca	:1;
	uint32 res1	:1;
	uint32 res2	:2;
	uint32 i_sid	:24;

	uint32 rsv_3	:32;

	uint32 rsv_4	:16;
	uint32 mac_da4732	:16;

	uint32 rsv_5	:32;

	uint32 mac_da310	:32;
};
typedef struct ds_l2_edit_pbb8w_s ds_l2_edit_pbb8w_t;

struct ds_l2_edit_pbb4w_s	/* 234 */
{

	uint32 rsv_0	:29;
	uint32 mac_sa_valid	:1;
	uint32 derive_mac_da	:1;
	uint32 isid_valid	:1;

	uint32 rsv_1	:11;
	uint32 bvlan_tagged	:1;
	uint32 copy_nca_res	:1;
	uint32 itag_cos	:3;
	uint32 derive_itag_cos	:1;
	uint32 itag_cfi	:1;
	uint32 pbb_header_op_type	:1;
	uint32 bvlan_tag_disable	:1;
	uint32 bvlan_id	:12;

	uint32 rsv_2	:29;
	uint32 map_btag_cos	:1;
	uint32 macda_valid	:1;
	uint32 bvlan_valid	:1;

	uint32 btag_cfi	:1;
	uint32 btag_cos	:3;
	uint32 nca	:1;
	uint32 res1	:1;
	uint32 res2	:2;
	uint32 i_sid	:24;
};
typedef struct ds_l2_edit_pbb4w_s ds_l2_edit_pbb4w_t;

struct ds_l3edit_mpls4w_s	/* 235 */
{

	uint32 rsv_0	:29;
	uint32 martini_encap_valid	:1;
	uint32 mcast_label0	:1;
	uint32 map_ttl0	:1;

	uint32 derive_exp0	:1;
	uint32 exp0	:3;
	uint32 ttl0	:8;
	uint32 label0	:20;

	uint32 rsv_1	:29;
	uint32 label_valid1	:1;
	uint32 mcast_label1	:1;
	uint32 map_ttl1	:1;

	uint32 derive_exp1	:1;
	uint32 exp1	:3;
	uint32 ttl1	:8;
	uint32 label1	:20;
};
typedef struct ds_l3edit_mpls4w_s ds_l3edit_mpls4w_t;

struct ds_l3edit_mpls8w_s	/* 236 */
{

	uint32 rsv_0	:29;
	uint32 martini_encap_valid	:1;
	uint32 mcast_label0	:1;
	uint32 map_ttl0	:1;

	uint32 derive_exp0	:1;
	uint32 exp0	:3;
	uint32 ttl0	:8;
	uint32 label0	:20;

	uint32 rsv_1	:29;
	uint32 label_valid1	:1;
	uint32 mcast_label1	:1;
	uint32 map_ttl1	:1;

	uint32 derive_exp1	:1;
	uint32 exp1	:3;
	uint32 ttl1	:8;
	uint32 label1	:20;

	uint32 rsv_2	:29;
	uint32 label_valid2	:1;
	uint32 mcast_label2	:1;
	uint32 map_ttl2	:1;

	uint32 derive_exp2	:1;
	uint32 exp2	:3;
	uint32 ttl2	:8;
	uint32 label2	:20;

	uint32 rsv_3	:29;
	uint32 label_valid3	:1;
	uint32 mcast_label3	:1;
	uint32 map_ttl3	:1;

	uint32 derive_exp3	:1;
	uint32 exp3	:3;
	uint32 ttl3	:8;
	uint32 label3	:20;
};
typedef struct ds_l3edit_mpls8w_s ds_l3edit_mpls8w_t;

struct ds_l3edit_nat4w_s	/* 237 */
{

	uint32 rsv_0	:29;
	uint32 ipda104	:1;
	uint32 nat_mode	:1;
	uint32 replace_l4_dest_port	:1;

	uint32 ipda3932	:8;
	uint32 use_port	:1;
	uint32 ipda_prefix_len	:7;
	uint32 l4_dest_port	:16;

	uint32 rsv_1	:29;
	uint32 ipv4_embed_mode	:1;
	uint32 ipda105	:1;
	uint32 replace_ipda	:1;

	uint32 ipda	:32;
};
typedef struct ds_l3edit_nat4w_s ds_l3edit_nat4w_t;

struct ds_l3edit_nat8w_s	/* 238 */
{

	uint32 rsv_0	:29;
	uint32 ipda104	:1;
	uint32 nat_mode	:1;
	uint32 replace_l4_dest_port	:1;

	uint32 ipda3932	:8;
	uint32 ipda_use_port	:1;
	uint32 ipda_prefix_len	:7;
	uint32 l4_dest_port	:16;

	uint32 rsv_1	:29;
	uint32 ipv4_embed_mode	:1;
	uint32 ipda105	:1;
	uint32 replace_ipda	:1;

	uint32 ipda	:32;

	uint32 rsv_2	:29;
	uint32 ipda108106	:3;

	uint32 ipda7140	:32;

	uint32 rsv_3	:29;
	uint32 ipda111109	:3;

	uint32 ipda10372	:32;
};
typedef struct ds_l3edit_nat8w_s ds_l3edit_nat8w_t;

struct ds_l3edit_tunnel_v4_s	/* 239 */
{

	uint32 rsv_0	:29;
	uint32 is_atp_tunnel	:1;
	uint32 inner_header_type	:2;

	uint32 gre_flags	:4;
	uint32 gre_sequence_id108	:3;
	uint32 gre_version	:1;
	uint32 copy_dont_frag	:1;
	uint32 dont_frag	:1;
	uint32 dscp	:6;
	uint32 ttl	:8;
	uint32 ip_protocol_type	:8;

	uint32 rsv_1	:29;
	uint32 ip_identific_type	:1;
	uint32 derive_dscp	:1;
	uint32 map_ttl	:1;

	uint32 gre_protocol_udp_src_port	:16;
	uint32 gre_key_udp_dest_port	:16;

	uint32 rsv_2	:29;
	uint32 t6to4_tunnel_sa	:1;
	uint32 t6to4_tunnel	:1;
	uint32 inner_header_valid	:1;

	uint32 ip_sa	:32;

	uint32 rsv_3	:29;
	uint32 vpls_port_chk_en	:1;
	uint32 rsv_4	:1;
	uint32 mtu_check_en	:1;

	uint32 ip_da	:32;
};
typedef struct ds_l3edit_tunnel_v4_s ds_l3edit_tunnel_v4_t;

struct ds_l3edit_tunnel_v6_s	/* 240 */
{

	uint32 rsv_0	:29;
	uint32 vpls_port_chk_en	:1;
	uint32 rsv_1	:2;

	uint32 gre_flags	:4;
	uint32 gre_sequence_id108	:3;
	uint32 gre_version	:1;
	uint32 tos	:8;
	uint32 ttl	:8;
	uint32 ip_protocol_type	:8;

	uint32 rsv_2	:29;
	uint32 new_flow_label_valid	:1;
	uint32 derive_dscp	:1;
	uint32 map_ttl	:1;

	uint32 gre_protocol	:16;
	uint32 gre_key150	:16;

	uint32 rsv_3	:29;
	uint32 inner_header_valid	:1;
	uint32 inner_header_type	:2;

	uint32 vpls_dest_port11to0	:12;
	uint32 flow_label	:20;

	uint32 rsv_4	:29;
	uint32 vpls_dest_port12	:1;
	uint32 rsv_5	:1;
	uint32 mtu_check_en	:1;

	uint32 gre_key3116	:16;
	uint32 ipsa_index	:8;
	uint32 ipda_index	:8;
};
typedef struct ds_l3edit_tunnel_v6_s ds_l3edit_tunnel_v6_t;

struct ds_l3edit_flex_s	/* 241 */
{

	uint32 rsv_0	:32;

	uint32 rewrite_byte_num	:4;
	uint32 rsv_1	:1;
	uint32 packet_type	:3;
	uint32 rewrite_string3	:24;

	uint32 rsv_2	:32;

	uint32 rewrite_string2	:32;

	uint32 rsv_3	:32;

	uint32 rewrite_string1	:32;

	uint32 rsv_4	:32;

	uint32 rewrite_string0	:32;
};
typedef struct ds_l3edit_flex_s ds_l3edit_flex_t;

struct ds_l3edit_loop_back_s	/* 242 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:9;
	uint32 length_adjust_type	:1;
	uint32 dest_map	:22;

	uint32 rsv_2	:32;

	uint32 rsv_3	:11;
	uint32 next_hop_ext	:1;
	uint32 next_hop_ptr	:20;
};
typedef struct ds_l3edit_loop_back_s ds_l3edit_loop_back_t;

struct ds_met_entry_s	/* 243 */
{

	uint32 rsv_0	:29;
	uint32 aps_brg_mismatch_discard	:1;
	uint32 aps_brg_protect_path	:1;
	uint32 aps_brg_en	:1;

	uint32 ucast_id_lower	:12;
	uint32 is_link_aggregation	:1;
	uint32 nexthop_ext	:1;
	uint32 next_met_entry_ptr	:18;

	uint32 rsv_1	:29;
	uint32 end_local_rep	:1;
	uint32 remote_bay	:1;
	uint32 port_check_discard	:1;

	uint32 ucast_id_upper	:4;
	uint32 length_adjust_type	:1;
	uint32 replication_ctl	:27;
};
typedef struct ds_met_entry_s ds_met_entry_t;

struct ds_mpls_s	/* 244 */
{

	uint32 rsv_0	:29;
	uint32 flow_policer_ptr10to8	:3;

	uint32 flow_policer_ptr11	:1;
	uint32 stats_ptr_mode	:1;
	uint32 llsp_valid	:1;
	uint32 use_label_ttl	:1;
	uint32 sbit	:1;
	uint32 priority_path_en	:1;
	uint32 equal_cost_path_num10	:2;
	uint32 equal_cost_path_num2	:1;
	uint32 ttl_threshhold	:7;
	uint32 flow_policer_ptr7to0	:8;
	uint32 ttl_check_mode	:2;
	uint32 ttl_decrease_mode	:1;
	uint32 use_label_exp	:1;
	uint32 flow_policer_ptr15to12	:4;

	uint32 rsv_1	:29;
	uint32 llsp_priority	:3;

	uint32 over_write_priority	:1;
	uint32 packet_type	:3;
	uint32 s_bit_check_en	:1;
	uint32 scontinue	:1;
	uint32 oam_check	:1;
	uint32 is_vc_label	:1;
	uint32 aps_select_valid	:1;
	uint32 offset_bytes	:3;
	uint32 ds_fwd_ptr	:20;
};
typedef struct ds_mpls_s ds_mpls_t;

struct ds_nexthop_s	/* 245 */
{

	uint32 rsv_0	:29;
	uint32 l2_rewrite_type	:3;

	uint32 stag_cfi	:1;
	uint32 stag_cos	:3;
	uint32 copy_ctag_cos	:1;
	uint32 svlan_tagged	:1;
	uint32 cvlan_tagged	:1;
	uint32 mtu_check_en	:1;
	uint32 by_pass_all	:1;
	uint32 payload_operation	:3;
	uint32 output_svlan_id_valid	:1;
	uint32 output_cvlan_id_valid	:1;
	uint32 l3edit_ptr	:18;

	uint32 rsv_1	:29;
	uint32 l3_rewrite_type	:3;

	uint32 replace_dscp	:1;
	uint32 replace_ctag_cos	:1;
	uint32 dest_vlan_ptr	:14;
	uint32 derive_stag_cos	:1;
	uint32 service_acl_qos_en	:1;
	uint32 service_policer_vld	:1;
	uint32 tagged_mode	:1;
	uint32 l2edit_ptr	:12;
};
typedef struct ds_nexthop_s ds_nexthop_t;

struct ds_nexthop8w_s	/* 246 */
{

	uint32 rsv_0	:29;
	uint32 l2_rewrite_type	:3;

	uint32 stag_cfi	:1;
	uint32 stag_cos	:3;
	uint32 copy_ctag_cos	:1;
	uint32 svlan_tagged	:1;
	uint32 cvlan_tagged	:1;
	uint32 mtu_check_en	:1;
	uint32 by_pass_all	:1;
	uint32 payload_operation	:3;
	uint32 output_svlan_id_valid	:1;
	uint32 output_cvlan_id_valid	:1;
	uint32 l3edit_ptr170	:18;

	uint32 rsv_1	:29;
	uint32 l3_rewrite_type	:3;

	uint32 replace_dscp	:1;
	uint32 replace_ctag_cos	:1;
	uint32 dest_vlan_ptr	:14;
	uint32 derive_stag_cos	:1;
	uint32 service_acl_qos_en	:1;
	uint32 service_policer_valid	:1;
	uint32 tagged_mode	:1;
	uint32 l2edit_ptr11to0	:12;

	uint32 rsv_2	:29;
	uint32 output_svlan_id_valid_ext	:1;
	uint32 output_cvlan_id_valid_ext	:1;
	uint32 l3edit_ptr19	:1;

	uint32 l3edit_ptr18	:1;
	uint32 l2edit_ptr18to12	:7;
	uint32 output_svlan_id_ext	:12;
	uint32 output_cvlan_id_ext	:12;

	uint32 rsv_3	:29;
	uint32 vpls_port_check	:1;
	uint32 tunnel_mtu_check	:1;
	uint32 service_id_en	:1;

	uint32 tunnel_update_disable	:1;
	uint32 community_port	:1;
	uint32 service_id	:14;
	uint32 svlan_tpid	:2;
	uint32 svlan_tpid_en	:1;
	uint32 vpls_dest_port	:13;
};
typedef struct ds_nexthop8w_s ds_nexthop8w_t;

struct ds_policer_s	/* 247 */
{

	uint32 rsv_0	:29;
	uint32 use_layer3_length	:1;
	uint32 sr_tcm_mode	:1;
	uint32 color_blind_mode	:1;

	uint32 peak_count	:22;
	uint32 commit_count_upper	:10;

	uint32 rsv_1	:29;
	uint32 stats_en	:1;
	uint32 color_drop_code	:2;

	uint32 commit_count_lower	:12;
	uint32 old_ts	:12;
	uint32 profile	:8;
};
typedef struct ds_policer_s ds_policer_t;

struct ds_forwarding_stats_s	/* 248 */
{

	uint32 rsv_0	:29;
	uint32 use_layer3_length	:1;
	uint32 byte_count_upper0	:2;

	uint32 packet_count	:32;

	uint32 rsv_1	:29;
	uint32 byte_count_upper	:3;

	uint32 byte_count_lower	:32;
};
typedef struct ds_forwarding_stats_s ds_forwarding_stats_t;

struct ds_vlan_s	/* 249 */
{

	uint32 rsv_0	:29;
	uint32 vlan_cross_connect	:1;
	uint32 replace_dscp	:1;
	uint32 trans_en	:1;

	uint32 pfm	:2;
	uint32 route_disable	:1;
	uint32 v4_ucast_en	:1;
	uint32 v4_mcast_en	:1;
	uint32 v6_ucast_en	:1;
	uint32 v6_mcast_en	:1;
	uint32 e_ether_oamv	:1;
	uint32 e_md_level	:3;
	uint32 igmp_snoop_en	:1;
	uint32 rec_en	:1;
	uint32 stp_id6	:1;
	uint32 v4_ucast_sa_type	:2;
	uint32 ether_oamv	:1;
	uint32 learn_dis	:1;
	uint32 stp_id	:6;
	uint32 md_level	:3;
	uint32 src_que_select	:1;
	uint32 dhcp_exception_type	:2;
	uint32 arp_exception_type	:2;

	uint32 rsv_1	:29;
	uint32 mac_sec_vlan_dis	:1;
	uint32 brg_dis	:1;
	uint32 vlan_sec_exp_en	:1;

	uint32 vrf_id	:16;
	uint32 v6_ucast_sa_type	:2;
	uint32 rsv_2	:4;
	uint32 if_id	:10;
};
typedef struct ds_vlan_s ds_vlan_t;

struct ds_vlan_status_s	/* 250 */
{

	uint32 rsv_0	:32;

	uint32 vlanid_validh	:32;

	uint32 rsv_1	:32;

	uint32 vlanid_validl	:32;
};
typedef struct ds_vlan_status_s ds_vlan_status_t;

struct ds_mac_s	/* 251 */
{

	uint32 rsv_0	:29;
	uint32 priority_path_en	:1;
	uint32 mac_sa_exception_en	:1;
	uint32 mac_da_exception_en	:1;

	uint32 learn_source1	:1;
	uint32 mac_known	:1;
	uint32 proto_exception_en	:1;
	uint32 global_src_port	:13;
	uint32 ucast_discard	:1;
	uint32 aps_select_protect_path	:1;
	uint32 aps_select_valid	:1;
	uint32 esp_key_or_oam	:13;

	uint32 rsv_1	:29;
	uint32 mcast_discard	:1;
	uint32 equal_cost_path_num10	:2;

	uint32 src_mismatch_discard	:1;
	uint32 src_mismatch_learn_en	:1;
	uint32 src_discard	:1;
	uint32 learn_en	:1;
	uint32 learn_source0	:1;
	uint32 brg_aps_select_valid	:1;
	uint32 source_port_check_en	:1;
	uint32 storm_ctl_en	:1;
	uint32 exception_sub_index	:4;
	uint32 fwd_ptr	:20;
};
typedef struct ds_mac_s ds_mac_t;

struct ds_fwd_s	/* 252 */
{

	uint32 rsv_0	:29;
	uint32 aps_select_group_valid	:1;
	uint32 length_adjust_type	:1;
	uint32 critical_packet	:1;

	uint32 stats_valid	:1;
	uint32 rsv_1	:1;
	uint32 aps_type	:2;
	uint32 stats_ptr_upper	:4;
	uint32 rsv_2	:2;
	uint32 dest_map	:22;

	uint32 rsv_3	:29;
	uint32 next_hop_ext	:1;
	uint32 sequence_number_chk_en	:1;
	uint32 send_local_phy_port	:1;

	uint32 stats_ptr_lower	:12;
	uint32 next_hop_ptr	:20;
};
typedef struct ds_fwd_s ds_fwd_t;

struct ds_eth_oam_chan_s	/* 253 */
{

	uint32 rsv_0	:29;
	uint32 passive_max_md_lvl	:3;

	uint32 passive_valid	:1;
	uint32 mem_index0	:15;
	uint32 mem_on_remote_chip	:1;
	uint32 mem_index1	:15;

	uint32 rsv_1	:29;
	uint32 oam_dest_chip_id4_to2	:3;

	uint32 rsv_2	:1;
	uint32 mep_index2	:15;
	uint32 rsv_3	:1;
	uint32 mep_index3	:15;

	uint32 rsv_4	:29;
	uint32 active_max_mdlvl	:3;

	uint32 rsv_5	:1;
	uint32 mep_index4	:15;
	uint32 rsv_6	:1;
	uint32 mep_index5	:15;

	uint32 rsv_7	:30;
	uint32 oam_dest_chip_id1_to0	:2;

	uint32 rsv_8	:1;
	uint32 mep_index6	:15;
	uint32 rsv_9	:1;
	uint32 mep_index7	:15;
};
typedef struct ds_eth_oam_chan_s ds_eth_oam_chan_t;

struct ds_mpls_pbt_oam_chan_s	/* 254 */
{

	uint32 rsv_0	:29;
	uint32 md_lvl	:3;

	uint32 md_lvl_check_valid	:1;
	uint32 rsv_1	:1;
	uint32 mep_on_remote_chip	:1;
	uint32 rmep_id	:13;
	uint32 rmep_id_check_valid	:1;
	uint32 mep_idex	:15;

	uint32 rsv_2	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:32;
};
typedef struct ds_mpls_pbt_oam_chan_s ds_mpls_pbt_oam_chan_t;

struct ds_rmep_chan_s	/* 255 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:17;
	uint32 rmep_index0	:15;

	uint32 rsv_2	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:32;
};
typedef struct ds_rmep_chan_s ds_rmep_chan_t;

struct ds_eth_mep_s	/* 256 */
{

	uint32 rsv_0	:29;
	uint32 cci_while	:3;

	uint32 present_traffic_check_en	:1;
	uint32 tx_with_send_id	:1;
	uint32 enable_pm	:1;
	uint32 rsv_1	:2;
	uint32 rmep_last_rdi	:1;
	uint32 present_rdi	:1;
	uint32 tpid_type	:2;
	uint32 d_unexp_mep	:1;
	uint32 d_mismerge	:1;
	uint32 d_meg_lvl	:1;
	uint32 port_id	:8;
	uint32 d_unexp_mep_timer	:4;
	uint32 d_mismerge_timer	:4;
	uint32 d_meg_lvl_timer	:4;

	uint32 rsv_2	:29;
	uint32 mep_type	:3;

	uint32 ccm_sep_num	:32;

	uint32 rsv_3	:29;
	uint32 active	:1;
	uint32 is_remote	:1;
	uint32 is_mpls	:1;

	uint32 dest_id	:16;
	uint32 cci_en	:1;
	uint32 ma_index	:15;

	uint32 rsv_4	:29;
	uint32 share_mac_en	:1;
	uint32 present_traffic 	:1;
	uint32 seq_num_en	:1;

	uint32 is_up	:1;
	uint32 dest_chip	:5;
	uint32 ma_id_check_disable	:1;
	uint32 mep_primary_vid	:12;
	uint32 mep_id	:13;
};
typedef struct ds_eth_mep_s ds_eth_mep_t;

struct ds_eth_rmep_s	/* 257 */
{

	uint32 rsv_0	:32;

	uint32 first_pkt_rx	:1;
	uint32 rmep_last_port_status	:2;
	uint32 seq_num_fail_counter	:6;
	uint32 mac_addr_update_disable	:1;
	uint32 rmep_while	:4;
	uint32 rmep_last_rdi	:1;
	uint32 d_unexp_period	:1;
	uint32 d_unexp_period_timer	:4;
	uint32 d_loc	:1;
	uint32 exp_ccm_num	:8;
	uint32 cnt_shift_while	:3;

	uint32 rsv_1	:29;
	uint32 rmep_last_intf_status	:3;

	uint32 ccm_sep_num	:32;

	uint32 rsv_2	:29;
	uint32 active	:1;
	uint32 is_remote	:1;
	uint32 is_mpls	:1;

	uint32 rmep_mac_sa31_to0	:32;

	uint32 rsv_3	:31;
	uint32 seq_num_en	:1;

	uint32 rsv_4	:1;
	uint32 mep_index	:15;
	uint32 rmep_mac_sa47_to32	:16;
};
typedef struct ds_eth_rmep_s ds_eth_rmep_t;

struct ds_mpls_mep_s	/* 258 */
{

	uint32 rsv_0	:29;
	uint32 cci_while	:3;

	uint32 first_pkt_rx	:1;
	uint32 t1_valid	:1;
	uint32 t1_while	:4;
	uint32 rsv_1	:1;
	uint32 d_ttsi_mismerge	:1;
	uint32 d_locv	:1;
	uint32 d_excess	:1;
	uint32 d_ttsi_mismatch	:1;
	uint32 is_available	:1;
	uint32 rsv_2	:3;
	uint32 t2_pending_valid	:1;
	uint32 tri_cycle_unexpcv	:3;
	uint32 recv_fdi	:1;
	uint32 ten_unexpcv	:4;
	uint32 ten_cycle_expectedcv	:7;
	uint32 expectedcvh	:1;

	uint32 rsv_3	:29;
	uint32 mep_type	:3;

	uint32 expected_cv 	:32;

	uint32 rsv_4	:29;
	uint32 active	:1;
	uint32 is_remote	:1;
	uint32 is_mpls	:1;

	uint32 dest_id	:16;
	uint32 cci_en	:1;
	uint32 ma_index	:15;

	uint32 rsv_5	:31;
	uint32 near_end_fsm_up_disable	:1;

	uint32 is_up	:1;
	uint32 dest_chip	:5;
	uint32 rsv_6	:18;
	uint32 period	:8;
};
typedef struct ds_mpls_mep_s ds_mpls_mep_t;

struct ds_mpls_rmep_s	/* 259 */
{

	uint32 rsv_0	:29;
	uint32 cci_while	:3;

	uint32 t3_valid	:1;
	uint32 t4_valid	:1;
	uint32 bdi_while	:4;
	uint32 rsv_1	:12;
	uint32 t3_while	:4;
	uint32 t3_cycle	:4;
	uint32 is_available	:1;
	uint32 tri_bdi_loop	:1;
	uint32 ten_bdi	:4;

	uint32 rsv_2	:29;
	uint32 bdi_tx	:1;
	uint32 ten_bdiloop	:1;
	uint32 rx_bdi	:1;

	uint32 rsv_3	:4;
	uint32 rmep_while	:4;
	uint32 tri_cycle_bdi	:2;
	uint32 defect_type	:6;
	uint32 defect_location	:16;

	uint32 rsv_4	:29;
	uint32 active	:1;
	uint32 is_remote	:1;
	uint32 is_mpls	:1;

	uint32 dest_id	:16;
	uint32 rsv_5	:1;
	uint32 ma_index	:15;

	uint32 rsv_6	:31;
	uint32 far_end_fsm_up_disable	:1;

	uint32 rsv_7	:1;
	uint32 dest_chip	:5;
	uint32 rsv_8	:18;
	uint32 period	:8;
};
typedef struct ds_mpls_rmep_s ds_mpls_rmep_t;

struct ds_mac_key_s	/* 260 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 rsv_2	:2;
	uint32 vlan_ptrh	:10;

	uint32 vlan_id2	:12;
	uint32 layer2_type	:4;
	uint32 rsv_3	:3;
	uint32 gbl_src_port	:13;

	uint32 rsv_4	:4;
	uint32 vlan_ptrl	:4;
	uint32 cos	:3;
	uint32 cfi	:1;
	uint32 layer3_type	:4;
	uint32 layer2_header_protocol	:16;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 table_id0	:4;
	uint32 vlan_id1	:12;

	uint32 mapped_vlan_id	:16;
	uint32 mapped_mac_da_upper	:16;

	uint32 mapped_mac_da_lower	:32;
};
typedef struct ds_mac_key_s ds_mac_key_t;

struct ds_mac_hash_key0_s	/* 261 */
{

	uint32 rsv_0	:32;

	uint32 mapped_vlanid	:16;
	uint32 mapped_mach	:16;

	uint32 rsv_1	:32;

	uint32 mapped_macl	:32;
};
typedef struct ds_mac_hash_key0_s ds_mac_hash_key0_t;

struct ds_mac_hash_key1_s	/* 262 */
{

	uint32 rsv_0	:32;

	uint32 mapped_vlanid	:16;
	uint32 mapped_mach	:16;

	uint32 rsv_1	:32;

	uint32 mapped_macl	:32;
};
typedef struct ds_mac_hash_key1_s ds_mac_hash_key1_t;

struct ds_acl_mac_key_s	/* 263 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 rsv_2	:12;

	uint32 l2_qos_label	:8;
	uint32 rsv_3	:24;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid2	:4;
	uint32 rsv_7	:4;
	uint32 l3_qos_label	:8;

	uint32 rsv_8	:18;
	uint32 vlan_ptr	:14;

	uint32 ether_type	:16;
	uint32 rsv_9	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 tableid1	:4;
	uint32 cvlan_id11to5	:7;
	uint32 is_ip_key	:1;
	uint32 acl_labelh	:4;

	uint32 acl_labell	:4;
	uint32 cvlan_id4to0	:5;
	uint32 cos	:3;
	uint32 layer3_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_12	:32;

	uint32 rsv_13	:16;
	uint32 tableid0	:4;
	uint32 is_label	:1;
	uint32 rsv_14	:3;
	uint32 qos_label	:8;

	uint32 layer2_type	:4;
	uint32 svlan_id	:12;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;
};
typedef struct ds_acl_mac_key_s ds_acl_mac_key_t;

struct ds_qos_mac_key_s	/* 264 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 rsv_2	:12;

	uint32 l2_qos_label	:8;
	uint32 rsv_3	:24;

	uint32 rsv_4	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid2	:4;
	uint32 rsv_7	:4;
	uint32 l3_qos_label	:8;

	uint32 rsv_8	:18;
	uint32 vlan_ptr	:14;

	uint32 ether_type	:16;
	uint32 rsv_9	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 tableid1	:4;
	uint32 cvlan_id11to5	:7;
	uint32 is_ip_key	:1;
	uint32 acl_labelh	:4;

	uint32 acl_labell	:4;
	uint32 cvlan_id4to0	:5;
	uint32 cos	:3;
	uint32 layer3_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_12	:32;

	uint32 rsv_13	:16;
	uint32 tableid0	:4;
	uint32 is_label	:1;
	uint32 rsv_14	:3;
	uint32 qos_label	:8;

	uint32 layer2_type	:4;
	uint32 svlan_id	:12;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;
};
typedef struct ds_qos_mac_key_s ds_qos_mac_key_t;

struct ds_acl_ipv4_key_s	/* 265 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 cvlan_id	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 macda_upper	:16;

	uint32 macda_lower	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 tableid2	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_4	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid1	:4;
	uint32 layer3_type	:4;
	uint32 ip_header_error	:1;
	uint32 is_mpls_key	:1;
	uint32 routed_packet	:1;
	uint32 is_ip_key	:1;
	uint32 acl_label_upper	:4;

	uint32 acl_label_lower	:4;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_7	:32;

	uint32 rsv_8	:16;
	uint32 tableid0	:4;
	uint32 cos	:3;
	uint32 is_label	:1;
	uint32 qos_label	:8;

	uint32 ip_da	:32;

	uint32 ip_sa	:32;
};
typedef struct ds_acl_ipv4_key_s ds_acl_ipv4_key_t;

struct ds_qos_ipv4_key_s	/* 266 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 cvlan_id	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 macda_upper	:16;

	uint32 macda_lower	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 tableid2	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_4	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid1	:4;
	uint32 layer3_type	:4;
	uint32 ip_header_error	:1;
	uint32 is_mpls_key	:1;
	uint32 routed_packet	:1;
	uint32 is_ip_key	:1;
	uint32 acl_label_upper	:4;

	uint32 acl_label_lower	:4;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_7	:32;

	uint32 rsv_8	:16;
	uint32 tableid0	:4;
	uint32 cos	:3;
	uint32 is_label	:1;
	uint32 qos_label	:8;

	uint32 ip_da	:32;

	uint32 ip_sa	:32;
};
typedef struct ds_qos_ipv4_key_s ds_qos_ipv4_key_t;

struct ds_acl_mpls_key_s	/* 267 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 cvlan_id	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 macda_upper	:16;

	uint32 macda_lower	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 tableid2	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_4	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid1	:4;
	uint32 mpls_label331to28	:4;
	uint32 rsv_7	:1;
	uint32 is_mpls_key	:1;
	uint32 route_pkt	:1;
	uint32 is_ip_key	:1;
	uint32 acl_label_upper	:4;

	uint32 acl_label_lower	:4;
	uint32 mpls_label327to0	:28;

	uint32 mpls_label2	:32;

	uint32 rsv_8	:32;

	uint32 rsv_9	:16;
	uint32 tableid0	:4;
	uint32 cos	:3;
	uint32 is_label	:1;
	uint32 qos_label	:8;

	uint32 mpls_label1	:32;

	uint32 mpls_label0	:32;
};
typedef struct ds_acl_mpls_key_s ds_acl_mpls_key_t;

struct ds_qos_mpls_key_s	/* 268 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 tableid3	:4;
	uint32 cvlan_id	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 macda_upper	:16;

	uint32 macda_lower	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 tableid2	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_4	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 tableid1	:4;
	uint32 mpls_label331to28	:4;
	uint32 rsv_7	:1;
	uint32 is_mpls_key	:1;
	uint32 route_pkt	:1;
	uint32 is_ip_key	:1;
	uint32 acl_label_upper	:4;

	uint32 acl_label_lower	:4;
	uint32 mpls_label327to0	:28;

	uint32 mpls_label2	:32;

	uint32 rsv_8	:32;

	uint32 rsv_9	:16;
	uint32 tableid0	:4;
	uint32 cos	:3;
	uint32 is_label	:1;
	uint32 qos_label	:8;

	uint32 mpls_label1	:32;

	uint32 mpls_label0	:32;
};
typedef struct ds_qos_mpls_key_s ds_qos_mpls_key_t;

struct ds_acl_ipv6_key_s	/* 269 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 aclqos_ipv6key_tableid7	:4;
	uint32 rsv_2	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_da_upper	:16;

	uint32 mac_da_lower	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:16;
	uint32 tableid6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_5	:2;
	uint32 vlan_ptr	:14;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:16;
	uint32 tableid5	:4;
	uint32 cvlan_id	:12;

	uint32 rsv_8	:1;
	uint32 cos	:3;
	uint32 svlan_id	:12;
	uint32 ether_type	:16;

	uint32 rsv_9	:6;
	uint32 dscp	:6;
	uint32 ipv6_flow_label	:20;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 tableid4	:4;
	uint32 ipv6_extension_headers	:8;
	uint32 layer3_type	:4;

	uint32 rsv_12	:4;
	uint32 acl_label_upper	:2;
	uint32 qos_label_upper	:2;
	uint32 rsv_13	:3;
	uint32 is_application	:1;
	uint32 ip_head_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 l4info_mapped	:16;

	uint32 l4_destport	:16;
	uint32 rsv_14	:16;

	uint32 rsv_15	:32;

	uint32 rsv_16	:16;
	uint32 tableid3	:4;
	uint32 l4destport_or_l4info15to6	:10;
	uint32 acl_label_middle	:2;

	uint32 acl_label_lower	:4;
	uint32 routed_packet	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_label	:1;
	uint32 ip_da127to104	:24;

	uint32 ip_da103to72	:32;

	uint32 rsv_17	:32;

	uint32 rsv_18	:16;
	uint32 tableid2	:4;
	uint32 l4_source_port15to12	:4;
	uint32 ip_da71to64	:8;

	uint32 ip_da63to32	:32;

	uint32 ip_da31to0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 tableid1	:4;
	uint32 l4destport_or_l4info5to0	:6;
	uint32 qos_label_lower	:6;

	uint32 l4_source_port11to4	:8;
	uint32 ip_sa127to104	:24;

	uint32 ip_sa103to72	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 tableid0	:4;
	uint32 l4_source_port3to0	:4;
	uint32 ip_sa71to64	:8;

	uint32 ip_sa63to32	:32;

	uint32 ip_sa31to0	:32;
};
typedef struct ds_acl_ipv6_key_s ds_acl_ipv6_key_t;

struct ds_qos_ipv6_key_s	/* 270 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 aclqos_ipv6key_tableid7	:4;
	uint32 rsv_2	:12;

	uint32 l2_qos_label	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_da_upper	:16;

	uint32 mac_da_lower	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:16;
	uint32 tableid6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 l3_qos_label	:8;

	uint32 rsv_5	:2;
	uint32 vlan_ptr	:14;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_6	:32;

	uint32 rsv_7	:16;
	uint32 tableid5	:4;
	uint32 cvlan_id	:12;

	uint32 rsv_8	:1;
	uint32 cos	:3;
	uint32 svlan_id	:12;
	uint32 ether_type	:16;

	uint32 rsv_9	:6;
	uint32 dscp	:6;
	uint32 ipv6_flow_label	:20;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 tableid4	:4;
	uint32 ipv6_extension_headers	:8;
	uint32 layer3_type	:4;

	uint32 rsv_12	:4;
	uint32 acl_label_upper	:2;
	uint32 qos_label_upper	:2;
	uint32 rsv_13	:3;
	uint32 is_application	:1;
	uint32 ip_head_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 l4info_mapped	:16;

	uint32 l4_destport	:16;
	uint32 rsv_14	:16;

	uint32 rsv_15	:32;

	uint32 rsv_16	:16;
	uint32 tableid3	:4;
	uint32 l4destport_or_l4info15to6	:10;
	uint32 acl_label_middle	:2;

	uint32 acl_label_lower	:4;
	uint32 routed_packet	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_label	:1;
	uint32 ip_da127to104	:24;

	uint32 ip_da103to72	:32;

	uint32 rsv_17	:32;

	uint32 rsv_18	:16;
	uint32 tableid2	:4;
	uint32 l4_source_port15to12	:4;
	uint32 ip_da71to64	:8;

	uint32 ip_da63to32	:32;

	uint32 ip_da31to0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 tableid1	:4;
	uint32 l4destport_or_l4info5to0	:6;
	uint32 qos_label_lower	:6;

	uint32 l4_source_port11to4	:8;
	uint32 ip_sa127to104	:24;

	uint32 ip_sa103to72	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 tableid0	:4;
	uint32 l4_source_port3to0	:4;
	uint32 ip_sa71to64	:8;

	uint32 ip_sa63to32	:32;

	uint32 ip_sa31to0	:32;
};
typedef struct ds_qos_ipv6_key_s ds_qos_ipv6_key_t;

struct ds_ipv4_ucast_route_key_s	/* 271 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 vrf_idl	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id0	:4;
	uint32 rsv_4	:1;
	uint32 lkp_mode	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;

	uint32 ip_sa	:32;

	uint32 ip_da	:32;
};
typedef struct ds_ipv4_ucast_route_key_s ds_ipv4_ucast_route_key_t;

struct ds_ipv4_mcast_route_key_s	/* 272 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 vrf_idl	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id0	:4;
	uint32 rsv_4	:1;
	uint32 lkp_mode	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;

	uint32 ip_sa	:32;

	uint32 ip_da	:32;
};
typedef struct ds_ipv4_mcast_route_key_s ds_ipv4_mcast_route_key_t;

struct ds_ipv4_nat_key_s	/* 273 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 vrf_idl	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id0	:4;
	uint32 rsv_4	:1;
	uint32 lkp_mode	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;

	uint32 ip_sa	:32;

	uint32 ip_da	:32;
};
typedef struct ds_ipv4_nat_key_s ds_ipv4_nat_key_t;

struct ds_ipv4_pbr_dualda_key_s	/* 274 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 vrf_idl	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id0	:4;
	uint32 rsv_4	:1;
	uint32 lkp_mode	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;

	uint32 ip_sa	:32;

	uint32 ip_da	:32;
};
typedef struct ds_ipv4_pbr_dualda_key_s ds_ipv4_pbr_dualda_key_t;

struct ds_ipv4_ucast_hash_key0_s	/* 275 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 key_vrfid	:16;

	uint32 rsv_2	:32;

	uint32 key_mapped_ip	:32;
};
typedef struct ds_ipv4_ucast_hash_key0_s ds_ipv4_ucast_hash_key0_t;

struct ds_ipv4_ucast_hash_key1_s	/* 276 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 key_vrfid	:16;

	uint32 rsv_2	:32;

	uint32 key_mapped_ip	:32;
};
typedef struct ds_ipv4_ucast_hash_key1_s ds_ipv4_ucast_hash_key1_t;

struct ds_ipv4_mcast_hash_key0_s	/* 277 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 key_vrfid	:16;

	uint32 rsv_2	:32;

	uint32 key_mapped_ip	:32;
};
typedef struct ds_ipv4_mcast_hash_key0_s ds_ipv4_mcast_hash_key0_t;

struct ds_ipv4_mcast_hash_key1_s	/* 278 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 key_vrfid	:16;

	uint32 rsv_2	:32;

	uint32 key_mapped_ip	:32;
};
typedef struct ds_ipv4_mcast_hash_key1_s ds_ipv4_mcast_hash_key1_t;

struct ds_ipv6_ucast_route_key_s	/* 279 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 ipv6_rtk_table_id7	:4;
	uint32 cvlan_id	:12;

	uint32 ether_typeh	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 ether_typel	:8;

	uint32 rsv_4	:2;
	uint32 ds_vlan_ptr	:14;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 table_id5	:4;
	uint32 rsv_7	:8;
	uint32 layer3_type	:4;

	uint32 rsv_8	:32;

	uint32 rsv_9	:20;
	uint32 scvlan_id	:12;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 table_id4	:4;
	uint32 rsv_12	:4;
	uint32 ipv6_extension_headers	:8;

	uint32 rsv_13	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4_info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_14	:32;

	uint32 rsv_15	:16;
	uint32 table_id3	:4;
	uint32 ipv6_flow_labelh	:12;

	uint32 ipv6_flow_labell	:8;
	uint32 ip_sa4	:24;

	uint32 ip_sa3	:32;

	uint32 rsv_16	:32;

	uint32 rsv_17	:16;
	uint32 table_id2	:4;
	uint32 rsv_18	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_sa2	:8;

	uint32 ip_sa1	:32;

	uint32 ip_sa0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 ip_header_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;
	uint32 ip_da4	:24;

	uint32 ip_da3	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 table_id0	:4;
	uint32 vrf_idl	:4;
	uint32 ip_da2	:8;

	uint32 ip_da1	:32;

	uint32 ip_da0	:32;
};
typedef struct ds_ipv6_ucast_route_key_s ds_ipv6_ucast_route_key_t;

struct ds_ipv6_nat_key_s	/* 280 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 ipv6_rtk_table_id7	:4;
	uint32 cvlan_id	:12;

	uint32 ether_typeh	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 ether_typel	:8;

	uint32 rsv_4	:2;
	uint32 ds_vlan_ptr	:14;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 table_id5	:4;
	uint32 rsv_7	:8;
	uint32 layer3_type	:4;

	uint32 rsv_8	:32;

	uint32 rsv_9	:20;
	uint32 scvlan_id	:12;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 table_id4	:4;
	uint32 rsv_12	:4;
	uint32 ipv6_extension_headers	:8;

	uint32 rsv_13	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4_info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_14	:32;

	uint32 rsv_15	:16;
	uint32 table_id3	:4;
	uint32 ipv6_flow_labelh	:12;

	uint32 ipv6_flow_labell	:8;
	uint32 ip_sa4	:24;

	uint32 ip_sa3	:32;

	uint32 rsv_16	:32;

	uint32 rsv_17	:16;
	uint32 table_id2	:4;
	uint32 rsv_18	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_sa2	:8;

	uint32 ip_sa1	:32;

	uint32 ip_sa0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 ip_header_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;
	uint32 ip_da4	:24;

	uint32 ip_da3	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 table_id0	:4;
	uint32 vrf_idl	:4;
	uint32 ip_da2	:8;

	uint32 ip_da1	:32;

	uint32 ip_da0	:32;
};
typedef struct ds_ipv6_nat_key_s ds_ipv6_nat_key_t;

struct ds_ipv6_pbr_dualda_key_s	/* 281 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 ipv6_rtk_table_id7	:4;
	uint32 cvlan_id	:12;

	uint32 ether_typeh	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 ether_typel	:8;

	uint32 rsv_4	:2;
	uint32 ds_vlan_ptr	:14;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 table_id5	:4;
	uint32 rsv_7	:8;
	uint32 layer3_type	:4;

	uint32 rsv_8	:32;

	uint32 rsv_9	:20;
	uint32 scvlan_id	:12;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 table_id4	:4;
	uint32 rsv_12	:4;
	uint32 ipv6_extension_headers	:8;

	uint32 rsv_13	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4_info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_14	:32;

	uint32 rsv_15	:16;
	uint32 table_id3	:4;
	uint32 ipv6_flow_labelh	:12;

	uint32 ipv6_flow_labell	:8;
	uint32 ip_sa4	:24;

	uint32 ip_sa3	:32;

	uint32 rsv_16	:32;

	uint32 rsv_17	:16;
	uint32 table_id2	:4;
	uint32 rsv_18	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_sa2	:8;

	uint32 ip_sa1	:32;

	uint32 ip_sa0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 ip_header_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;
	uint32 ip_da4	:24;

	uint32 ip_da3	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 table_id0	:4;
	uint32 vrf_idl	:4;
	uint32 ip_da2	:8;

	uint32 ip_da1	:32;

	uint32 ip_da0	:32;
};
typedef struct ds_ipv6_pbr_dualda_key_s ds_ipv6_pbr_dualda_key_t;

struct ds_ipv6_mcast_route_key_s	/* 282 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 ipv6_rtk_table_id7	:4;
	uint32 cvlan_id	:12;

	uint32 ether_typeh	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_dah	:16;

	uint32 mac_dal	:32;

	uint32 rsv_2	:32;

	uint32 rsv_3	:16;
	uint32 table_id6	:4;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 ether_typel	:8;

	uint32 rsv_4	:2;
	uint32 ds_vlan_ptr	:14;
	uint32 mac_sah	:16;

	uint32 mac_sal	:32;

	uint32 rsv_5	:32;

	uint32 rsv_6	:16;
	uint32 table_id5	:4;
	uint32 rsv_7	:8;
	uint32 layer3_type	:4;

	uint32 rsv_8	:32;

	uint32 rsv_9	:20;
	uint32 scvlan_id	:12;

	uint32 rsv_10	:32;

	uint32 rsv_11	:16;
	uint32 table_id4	:4;
	uint32 rsv_12	:4;
	uint32 ipv6_extension_headers	:8;

	uint32 rsv_13	:4;
	uint32 pbr_label	:6;
	uint32 dscp	:6;
	uint32 l4_info_mapped	:16;

	uint32 l4_dest_port	:16;
	uint32 l4_source_port	:16;

	uint32 rsv_14	:32;

	uint32 rsv_15	:16;
	uint32 table_id3	:4;
	uint32 ipv6_flow_labelh	:12;

	uint32 ipv6_flow_labell	:8;
	uint32 ip_sa4	:24;

	uint32 ip_sa3	:32;

	uint32 rsv_16	:32;

	uint32 rsv_17	:16;
	uint32 table_id2	:4;
	uint32 rsv_18	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_sa2	:8;

	uint32 ip_sa1	:32;

	uint32 ip_sa0	:32;

	uint32 rsv_19	:32;

	uint32 rsv_20	:16;
	uint32 table_id1	:4;
	uint32 vrf_idh	:12;

	uint32 ip_header_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 layer4_type	:4;
	uint32 ip_da4	:24;

	uint32 ip_da3	:32;

	uint32 rsv_21	:32;

	uint32 rsv_22	:16;
	uint32 table_id0	:4;
	uint32 vrf_idl	:4;
	uint32 ip_da2	:8;

	uint32 ip_da1	:32;

	uint32 ip_da0	:32;
};
typedef struct ds_ipv6_mcast_route_key_s ds_ipv6_mcast_route_key_t;

struct ds_ipv6_ucast_hash_key0_s	/* 283 */
{

	uint32 rsv_0	:29;
	uint32 vrfid3	:3;

	uint32 key_ipda3	:32;

	uint32 rsv_1	:29;
	uint32 vrfid2	:3;

	uint32 key_ipda2	:32;

	uint32 rsv_2	:29;
	uint32 vrfid1	:3;

	uint32 key_ipda1	:32;

	uint32 rsv_3	:29;
	uint32 vrfid0	:3;

	uint32 key_ipda0	:32;
};
typedef struct ds_ipv6_ucast_hash_key0_s ds_ipv6_ucast_hash_key0_t;

struct ds_ipv6_mcast_hash_key0_s	/* 284 */
{

	uint32 rsv_0	:29;
	uint32 vrfid3	:3;

	uint32 key_ipda3	:32;

	uint32 rsv_1	:29;
	uint32 vrfid2	:3;

	uint32 key_ipda2	:32;

	uint32 rsv_2	:29;
	uint32 vrfid1	:3;

	uint32 key_ipda1	:32;

	uint32 rsv_3	:29;
	uint32 vrfid0	:3;

	uint32 key_ipda0	:32;
};
typedef struct ds_ipv6_mcast_hash_key0_s ds_ipv6_mcast_hash_key0_t;

struct ds_ipv6_ucast_hash_key1_s	/* 285 */
{

	uint32 rsv_0	:29;
	uint32 vrfid3	:3;

	uint32 key_ipda3	:32;

	uint32 rsv_1	:29;
	uint32 vrfid2	:3;

	uint32 key_ipda2	:32;

	uint32 rsv_2	:29;
	uint32 vrfid1	:3;

	uint32 key_ipda1	:32;

	uint32 rsv_3	:29;
	uint32 vrfid0	:3;

	uint32 key_ipda0	:32;
};
typedef struct ds_ipv6_ucast_hash_key1_s ds_ipv6_ucast_hash_key1_t;

struct ds_ipv6_mcast_hash_key1_s	/* 286 */
{

	uint32 rsv_0	:29;
	uint32 vrfid3	:3;

	uint32 key_ipda3	:32;

	uint32 rsv_1	:29;
	uint32 vrfid2	:3;

	uint32 key_ipda2	:32;

	uint32 rsv_2	:29;
	uint32 vrfid1	:3;

	uint32 key_ipda1	:32;

	uint32 rsv_3	:29;
	uint32 vrfid0	:3;

	uint32 key_ipda0	:32;
};
typedef struct ds_ipv6_mcast_hash_key1_s ds_ipv6_mcast_hash_key1_t;

struct ds_user_id_vlan_key_s	/* 287 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 cvlan_idh	:6;
	uint32 user_id_label	:6;

	uint32 cvlan_idl	:6;
	uint32 svlan_id	:12;
	uint32 from_sgmac	:1;
	uint32 global_src_port	:13;

	uint32 customer_id	:32;
};
typedef struct ds_user_id_vlan_key_s ds_user_id_vlan_key_t;

struct ds_user_id_mac_key_s	/* 288 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id1	:4;
	uint32 rsv_2	:3;
	uint32 exp2	:1;
	uint32 exp_sub_idx	:4;
	uint32 vlan_id211to8	:4;

	uint32 vlan_id27to0	:8;
	uint32 cos2	:3;
	uint32 cfi2	:1;
	uint32 layer2_type	:4;
	uint32 mac_da_upper	:16;

	uint32 mac_da_lower	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:16;
	uint32 table_id0	:4;
	uint32 cos1	:3;
	uint32 cfi1	:1;
	uint32 rsv_5	:1;
	uint32 from_sgmac	:1;
	uint32 user_id_label	:6;

	uint32 layer3_type	:4;
	uint32 vlan_id1	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;
};
typedef struct ds_user_id_mac_key_s ds_user_id_mac_key_t;

struct ds_user_id_ipv4_key_s	/* 289 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id3	:4;
	uint32 rsv_2	:4;
	uint32 ether_type_lower	:8;

	uint32 ether_type_upper	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_da_upper	:16;

	uint32 mac_da_lower	:32;

	uint32 rsv_3	:32;

	uint32 rsv_4	:16;
	uint32 table_id2	:4;
	uint32 rsv_5	:1;
	uint32 exp2	:1;
	uint32 exp_sub_idx	:4;
	uint32 routed_packet	:1;
	uint32 ip_header_error	:1;
	uint32 rsv_6	:4;

	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_application	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4dest_port	:16;
	uint32 l4source_port	:16;

	uint32 rsv_7	:32;

	uint32 rsv_8	:16;
	uint32 table_id1	:4;
	uint32 cvlan_id	:12;

	uint32 layer3_type	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_9	:32;

	uint32 rsv_10	:16;
	uint32 table_id0	:4;
	uint32 rsv_11	:5;
	uint32 from_sgmac	:1;
	uint32 user_id_label	:6;

	uint32 ip_da	:32;

	uint32 ip_sa	:32;
};
typedef struct ds_user_id_ipv4_key_s ds_user_id_ipv4_key_t;

struct ds_user_id_ipv6_key_s	/* 290 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id7	:4;
	uint32 rsv_2	:12;

	uint32 rsv_3	:8;
	uint32 ip_da127_to104	:24;

	uint32 ip_da103_to72	:32;

	uint32 rsv_4	:32;

	uint32 rsv_5	:16;
	uint32 table_id6	:4;
	uint32 rsv_6	:4;
	uint32 ip_da71_to64	:8;

	uint32 ip_da63_to32	:32;

	uint32 ip_da31_to0	:32;

	uint32 rsv_7	:32;

	uint32 rsv_8	:16;
	uint32 table_id5	:4;
	uint32 rsv_9	:12;

	uint32 rsv_10	:8;
	uint32 ctag_cos	:3;
	uint32 ctag_cfi	:1;
	uint32 rsv_11	:20;

	uint32 ipv6_flow_label	:20;
	uint32 rsv_12	:4;
	uint32 ether_type7to0	:8;

	uint32 rsv_13	:32;

	uint32 rsv_14	:16;
	uint32 table_id4	:4;
	uint32 rsv_15	:4;
	uint32 ipv6_externsion_headers	:8;

	uint32 rsv_16	:6;
	uint32 routed_packet	:1;
	uint32 is_tcp	:1;
	uint32 is_udp	:1;
	uint32 is_app	:1;
	uint32 dscp	:6;
	uint32 l4info_mapped	:16;

	uint32 l4dest_port	:16;
	uint32 l4source_port	:16;

	uint32 rsv_17	:32;

	uint32 rsv_18	:16;
	uint32 table_id3	:4;
	uint32 rsv_19	:12;

	uint32 ether_type15to8	:8;
	uint32 stag_cos	:3;
	uint32 stag_cfi	:1;
	uint32 layer2_type	:4;
	uint32 mac_da_upper	:16;

	uint32 mac_da_lower	:32;

	uint32 rsv_20	:32;

	uint32 rsv_21	:16;
	uint32 table_id2	:4;
	uint32 cvlan_id	:12;

	uint32 layer3_type	:4;
	uint32 svlan_id	:12;
	uint32 mac_sa_upper	:16;

	uint32 mac_sa_lower	:32;

	uint32 rsv_22	:32;

	uint32 rsv_23	:16;
	uint32 table_id1	:4;
	uint32 exp2	:1;
	uint32 exp_sub_idx	:4;
	uint32 from_sgmac	:1;
	uint32 user_id_label	:6;

	uint32 ip_header_error	:1;
	uint32 ip_options	:1;
	uint32 frag_info	:2;
	uint32 rsv_24	:4;
	uint32 ip_sa127_to104	:24;

	uint32 ip_sa103_to72	:32;

	uint32 rsv_25	:32;

	uint32 rsv_26	:16;
	uint32 table_id0	:4;
	uint32 rsv_27	:4;
	uint32 ip_sa71_to64	:8;

	uint32 ip_sa63_to32	:32;

	uint32 ip_sa31_to0	:32;
};
typedef struct ds_user_id_ipv6_key_s ds_user_id_ipv6_key_t;

struct ds_eth_oam_key_s	/* 291 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 rsv_2	:9;
	uint32 oam_lookup_type	:3;

	uint32 is_up	:1;
	uint32 rsv_3	:18;
	uint32 port	:13;

	uint32 rsv_4	:16;
	uint32 vlan_ptr	:16;
};
typedef struct ds_eth_oam_key_s ds_eth_oam_key_t;

struct ds_eth_oam_hash_key0_s	/* 292 */
{

	uint32 rsv_0	:29;
	uint32 oam_hash_type0	:3;

	uint32 invalid0	:1;
	uint32 is_up0	:1;
	uint32 rsv_1	:1;
	uint32 global_src_port0	:13;
	uint32 vlan_ptr0	:16;

	uint32 rsv_2	:29;
	uint32 oam_hash_type1	:3;

	uint32 invalid1	:1;
	uint32 is_up1	:1;
	uint32 rsv_3	:1;
	uint32 global_src_port1	:13;
	uint32 vlan_ptr1	:16;

	uint32 rsv_4	:29;
	uint32 oam_hash_type2	:3;

	uint32 invalid2	:1;
	uint32 is_up2	:1;
	uint32 rsv_5	:1;
	uint32 global_src_port2	:13;
	uint32 vlan_ptr2	:16;

	uint32 rsv_6	:29;
	uint32 oam_hash_type3	:3;

	uint32 invalid3	:1;
	uint32 is_up3	:1;
	uint32 rsv_7	:1;
	uint32 global_src_port3	:13;
	uint32 vlan_ptr3	:16;
};
typedef struct ds_eth_oam_hash_key0_s ds_eth_oam_hash_key0_t;

struct ds_eth_oam_hash_key1_s	/* 293 */
{

	uint32 rsv_0	:29;
	uint32 oam_hash_type0	:3;

	uint32 invalid0	:1;
	uint32 is_up0	:1;
	uint32 rsv_1	:1;
	uint32 global_src_port0	:13;
	uint32 vlan_ptr0	:16;

	uint32 rsv_2	:29;
	uint32 oam_hash_type1	:3;

	uint32 invalid1	:1;
	uint32 is_up1	:1;
	uint32 rsv_3	:1;
	uint32 global_src_port1	:13;
	uint32 vlan_ptr1	:16;

	uint32 rsv_4	:29;
	uint32 oam_hash_type2	:3;

	uint32 invalid2	:1;
	uint32 is_up2	:1;
	uint32 rsv_5	:1;
	uint32 global_src_port2	:13;
	uint32 vlan_ptr2	:16;

	uint32 rsv_6	:29;
	uint32 oam_hash_type3	:3;

	uint32 invalid3	:1;
	uint32 is_up3	:1;
	uint32 rsv_7	:1;
	uint32 global_src_port3	:13;
	uint32 vlan_ptr3	:16;
};
typedef struct ds_eth_oam_hash_key1_s ds_eth_oam_hash_key1_t;

struct ds_pbt_oam_key_s	/* 294 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 rsv_2	:9;
	uint32 oam_lookup_type	:3;

	uint32 rsv_3	:32;

	uint32 rsv_4	:4;
	uint32 pbt_oam_port	:12;
	uint32 rsv_5	:3;
	uint32 esp_id	:13;
};
typedef struct ds_pbt_oam_key_s ds_pbt_oam_key_t;

struct ds_pbt_oam_hash_key0_s	/* 295 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 invalid0	:1;
	uint32 pbt_oam_port0	:15;
	uint32 esp_id0	:16;

	uint32 rsv_1	:29;
	uint32 oam_lookup_type1	:3;

	uint32 invalid1	:1;
	uint32 pbt_oam_port1	:15;
	uint32 esp_id1	:16;

	uint32 rsv_2	:29;
	uint32 oam_lookup_type2	:3;

	uint32 invalid2	:1;
	uint32 pbt_oam_port2	:15;
	uint32 esp_id2	:16;

	uint32 rsv_3	:29;
	uint32 oam_lookup_type3	:3;

	uint32 invalid3	:1;
	uint32 pbt_oam_port3	:15;
	uint32 esp_id3	:16;
};
typedef struct ds_pbt_oam_hash_key0_s ds_pbt_oam_hash_key0_t;

struct ds_pbt_oam_hash_key1_s	/* 296 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 invalid0	:1;
	uint32 pbt_oam_port0	:15;
	uint32 esp_id0	:16;

	uint32 rsv_1	:29;
	uint32 oam_lookup_type1	:3;

	uint32 invalid1	:1;
	uint32 pbt_oam_port1	:15;
	uint32 esp_id1	:16;

	uint32 rsv_2	:29;
	uint32 oam_lookup_type2	:3;

	uint32 invalid2	:1;
	uint32 pbt_oam_port2	:15;
	uint32 esp_id2	:16;

	uint32 rsv_3	:29;
	uint32 oam_lookup_type3	:3;

	uint32 invalid3	:1;
	uint32 pbt_oam_port3	:15;
	uint32 esp_id3	:16;
};
typedef struct ds_pbt_oam_hash_key1_s ds_pbt_oam_hash_key1_t;

struct ds_mpls_oam_label_key_s	/* 297 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 rsv_2	:9;
	uint32 oam_lookup_type	:3;

	uint32 rsv_3	:32;

	uint32 rsv_4	:12;
	uint32 mpls_lable	:20;
};
typedef struct ds_mpls_oam_label_key_s ds_mpls_oam_label_key_t;

struct ds_mpls_oam_label_hash_key0_s	/* 298 */
{

	uint32 rsv_0	:29;
	uint32 oam_look_up_type0	:3;

	uint32 invalid0	:1;
	uint32 rsv_1	:11;
	uint32 mpls_label0	:20;

	uint32 rsv_2	:29;
	uint32 oam_look_up_type1	:3;

	uint32 invalid1	:1;
	uint32 rsv_3	:11;
	uint32 mpls_label1	:20;

	uint32 rsv_4	:29;
	uint32 oam_look_up_type2	:3;

	uint32 invalid2	:1;
	uint32 rsv_5	:11;
	uint32 mpls_label2	:20;

	uint32 rsv_6	:29;
	uint32 oam_look_up_type3	:3;

	uint32 invalid3	:1;
	uint32 rsv_7	:11;
	uint32 mpls_label3	:20;
};
typedef struct ds_mpls_oam_label_hash_key0_s ds_mpls_oam_label_hash_key0_t;

struct ds_mpls_oam_label_hash_key1_s	/* 299 */
{

	uint32 rsv_0	:29;
	uint32 oam_look_up_type0	:3;

	uint32 invalid0	:1;
	uint32 rsv_1	:11;
	uint32 mpls_label0	:20;

	uint32 rsv_2	:29;
	uint32 oam_look_up_type1	:3;

	uint32 invalid1	:1;
	uint32 rsv_3	:11;
	uint32 mpls_label1	:20;

	uint32 rsv_4	:29;
	uint32 oam_look_up_type2	:3;

	uint32 invalid2	:1;
	uint32 rsv_5	:11;
	uint32 mpls_label2	:20;

	uint32 rsv_6	:29;
	uint32 oam_look_up_type3	:3;

	uint32 invalid3	:1;
	uint32 rsv_7	:11;
	uint32 mpls_label3	:20;
};
typedef struct ds_mpls_oam_label_hash_key1_s ds_mpls_oam_label_hash_key1_t;

struct ds_mpls_oam_ipv4_ttsi_key_s	/* 300 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 rsv_2	:9;
	uint32 oam_lookup_type	:3;

	uint32 lsp_id	:32;

	uint32 lsr_id	:32;
};
typedef struct ds_mpls_oam_ipv4_ttsi_key_s ds_mpls_oam_ipv4_ttsi_key_t;

struct ds_mpls_oam_ipv4_ttsi_hash_key0_s	/* 301 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 lsp_id0	:32;

	uint32 rsv_1	:31;
	uint32 invalid0	:1;

	uint32 lsr_id0	:32;

	uint32 rsv_2	:29;
	uint32 oam_lookup_type1	:3;

	uint32 lsp_id1	:32;

	uint32 rsv_3	:31;
	uint32 invalid1	:1;

	uint32 lsr_id1	:32;
};
typedef struct ds_mpls_oam_ipv4_ttsi_hash_key0_s ds_mpls_oam_ipv4_ttsi_hash_key0_t;

struct ds_mpls_oam_ipv4_ttsi_hash_key1_s	/* 302 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 lsp_id0	:32;

	uint32 rsv_1	:31;
	uint32 invalid0	:1;

	uint32 lsr_id0	:32;

	uint32 rsv_2	:29;
	uint32 oam_lookup_type1	:3;

	uint32 lsp_id1	:32;

	uint32 rsv_3	:31;
	uint32 invalid1	:1;

	uint32 lsr_id1	:32;
};
typedef struct ds_mpls_oam_ipv4_ttsi_hash_key1_s ds_mpls_oam_ipv4_ttsi_hash_key1_t;

struct ds_eth_oam_rmep_key_s	/* 303 */
{

	uint32 rsv_0	:32;

	uint32 rsv_1	:16;
	uint32 table_id	:4;
	uint32 rsv_2	:9;
	uint32 oam_lookup_type	:3;

	uint32 rsv_3	:32;

	uint32 rsv_4	:3;
	uint32 rmep_id	:13;
	uint32 mep_index	:16;
};
typedef struct ds_eth_oam_rmep_key_s ds_eth_oam_rmep_key_t;

struct ds_eth_oam_rmep_hash_key0_s	/* 304 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 invalid0	:1;
	uint32 rsv_1	:2;
	uint32 rmep_id0	:13;
	uint32 rsv_2	:1;
	uint32 mep_index0	:15;

	uint32 rsv_3	:29;
	uint32 oam_lookup_type1	:3;

	uint32 invalid1	:1;
	uint32 rsv_4	:2;
	uint32 rmep_id1	:13;
	uint32 rsv_5	:1;
	uint32 mep_index1	:15;

	uint32 rsv_6	:29;
	uint32 oam_lookup_type2	:3;

	uint32 invalid2	:1;
	uint32 rsv_7	:2;
	uint32 rmep_id2	:13;
	uint32 rsv_8	:1;
	uint32 mep_index2	:15;

	uint32 rsv_9	:29;
	uint32 oam_lookup_type3	:3;

	uint32 invalid3	:1;
	uint32 rsv_10	:2;
	uint32 rmep_id3	:13;
	uint32 rsv_11	:1;
	uint32 mep_index3	:15;
};
typedef struct ds_eth_oam_rmep_hash_key0_s ds_eth_oam_rmep_hash_key0_t;

struct ds_eth_oam_rmep_hash_key1_s	/* 305 */
{

	uint32 rsv_0	:29;
	uint32 oam_lookup_type0	:3;

	uint32 invalid0	:1;
	uint32 rsv_1	:2;
	uint32 rmep_id0	:13;
	uint32 rsv_2	:1;
	uint32 mep_index0	:15;

	uint32 rsv_3	:29;
	uint32 oam_lookup_type1	:3;

	uint32 invalid1	:1;
	uint32 rsv_4	:2;
	uint32 rmep_id1	:13;
	uint32 rsv_5	:1;
	uint32 mep_index1	:15;

	uint32 rsv_6	:29;
	uint32 oam_lookup_type2	:3;

	uint32 invalid2	:1;
	uint32 rsv_7	:2;
	uint32 rmep_id2	:13;
	uint32 rsv_8	:1;
	uint32 mep_index2	:15;

	uint32 rsv_9	:29;
	uint32 oam_lookup_type3	:3;

	uint32 invalid3	:1;
	uint32 rsv_10	:2;
	uint32 rmep_id3	:13;
	uint32 rsv_11	:1;
	uint32 mep_index3	:15;
};
typedef struct ds_eth_oam_rmep_hash_key1_s ds_eth_oam_rmep_hash_key1_t;

struct humber_packet_header_s
{
    uint32 hdr_type          :2;
    uint32 dest_id_discard   :1;
    uint32 pkt_offset        :7;
    uint32 multi_cast        :1;
    uint32 dest_chip_id      :5;
    uint32 dest_id           :16;

    uint32 priority          :6;
    uint32 pkt_type          :3;
    uint32 src_cos           :3;
    uint32 src_queue_select  :1;
    uint32 hd_hash_2_to_0    :3;
    uint32 vpls_port_type    :1;
    uint32 untag_pkt         :1;
    uint32 src_port          :14;

    uint32 src_vid           :12;
    uint32 color             :2;
    uint32 nxt_hop_ptr       :18;

    uint32 len_adj_type      :1;
    uint32 critical_pkt      :1;
    uint32 pkt_len           :14;
    uint32 ttl_or_oam_defect :8;
    uint32 hdr_crc           :8;

    uint32 srcport_isolate_id :6;
    uint32 pbb_srcport_type_or_l4srcport_vld :3;
    uint32 communicate_port  :1;
    uint32 src_cfi           :1;
    uint32 next_hop_ext      :1;
    uint32 nexthop_ptr_19_18 :2;
    uint32 svlan_tpid_index  :2;
    uint32 deny_rplc_cos     :1;
    uint32 deny_rplc_dscp    :1;
    uint32 src_svid_vld      :1;
    uint32 src_cvid_vld      :1;
    uint32 src_cvid          :12;

    uint32 src_vlanptr_or_timestamp_79_64 :16;
    uint32 vrfid_timestamp_63_48 :16;

    uint32 l4srcport_or_vplssrcport_oamtype :16;
    uint32 rsv2              :2;
    uint32 src_tagged        :1;
    uint32 use_outer_vrfid   :1;
    uint32 pfm               :2;
    uint32 oam_tunnel_en     :1;
    uint32 mirror            :1;
    uint32 operation_type    :3;
    uint32 hd_hash_7_to_3    :5;

    uint32 flowid_servecid_or_oamportid :32;
};
typedef struct humber_packet_header_s humber_packet_header_t;

struct humber_exception_info_s
{
    uint32 loopback_en       :1;
    uint32 mac_valid         :1;

    uint32 exp_vect_15       :1;
    uint32 exp_vect_14       :1;
    uint32 exp_vect_13       :1;
    uint32 exp_vect_12       :1;
    uint32 exception_sub_idx :4;
    uint32 egress_exception  :1;
    uint32 exception_pkt_type :3;
    uint32 l2_span_id        :2;
    uint32 l3_span_id        :2;
    uint32 acl_log_id        :2;
    uint32 qos_log_id        :2;
    uint32 exceptions_vect_11_0 :12;
};
typedef struct humber_exception_info_s humber_exception_info_t;

struct bay_packet_header
{
    uint32 hdr_type            :2;
    uint32 dest_id_discard     :1;
    uint32 pkt_offset          :7;
    uint32 multi_cast          :1;
    uint32 dest_chip_id        :5;
    uint32 dest_id             :16;

    uint32 priority            :6;
    uint32 pkt_type            :3;
    uint32 src_cos             :3;
    uint32 src_queue_select    :1;
    uint32 header_hash         :3;
    uint32 vpls_port_type      :1;
    uint32 untagged_pkt        :1;
    uint32 src_port            :14;

    uint32 src_vlan_id         :12;
    uint32 color               :2;
    uint32 nxt_hop_ptr         :18;

    uint32 len_adj_type        :1;
    uint32 critical_pkt        :1;
    uint32 pkt_len             :14;
    uint32 ttl                 :8;
    uint32 hdr_crc             :8;
};
typedef struct bay_packet_header bpkt_hdr_t;

struct bay_exception_info
{
    uint32                      :4;
    uint32 vlan_insert          :2;
    uint32 loopback_valid       :1;
    uint32 mac_valid            :1;
    uint32 egress_exception     :1;
    uint32 exception_pkt_type   :3;
    uint32 l2_span_id           :2;
    uint32 l3_span_id           :2;
    uint32 acl_log_id           :2;
    uint32 qos_log_id           :2;
    uint32 exceptions_vect      :12;
};
typedef struct bay_exception_info bexpt_info_t;

struct bay2rich_cell_header_s
{
    uint32 dest_chip_id         :5;
    uint32 priority             :2;
    uint32 reserved             :25;
};
typedef struct bay2rich_cell_header_s bay2rich_cell_header_t;

struct humber_hcgig_plus_header_s
{
    uint32 hcgig_plus_start                                                                    :8;
    uint32 hcgig_plus_hgi                                                                       :2;
    uint32 hcgig_plus_rsv1                                                                      :1;
    uint32 hcgig_plus_hdr_ext_len                                                         :3;
    uint32 hcgig_plus_src_modid_6                                                        :1;
    uint32 hcgig_plus_dst_modid_6                                                        :1;
    uint32 hcgig_plus_vid_high                                                               :8;
    uint32 hcgig_plus_vid_low                                                                :8;

    uint32 hcgig_plus_src_modid                                                            :5;
    uint32 hcgig_plus_opcode                                                                 :3;
    uint32 hcgig_plus_pfm                                                                      :2;
    uint32 hcgig_plus_src_pid                                                                 :6;
    uint32 hcgig_plus_dst_port                                                               :5;
    uint32 hcgig_plus_cos                                                                       :3;
    uint32 hcgig_plus_header_type                                                        :2;
    uint32 hcgig_plus_cng                                                                       :1;
    uint32 hcgig_plus_dst_modid                                                            :5;
  /*  uint32 hcgig_plus_ppd_type                                                              :3;*/

    uint32 hcgig_plus_dst_t                                                                     :1;
    uint32 hcgig_plus_dst_tgid                                                                :3;
    uint32 hcgig_plus_ingress_tagged                                                    :1;
    uint32 hcgig_plus_mirror_only                                                          :1;
    uint32 hcgig_plus_mirror_done                                                         :1;
    uint32 hcgig_plus_mirror                                                                   :1;
    uint32 hcgig_plus_src_mod_5                                                            :1;
    uint32 hcgig_plus_dst_mod_5                                                            :1;
    uint32 hcgig_plus_l3                                                                           :1;
    uint32 hcgig_plus_label_present                                                       :1;
    uint32 hcgig_plus_vc_label_19_16                                                    :4;
    uint32 hcgig_plus_vc_label_15_8                                                      :8;
    uint32 hcgig_plus_vc_label_7_0                                                        :8;

};
typedef struct humber_hcgig_plus_header_s humber_hcgig_plus_header_t;

union humber_hcgig2_header_u
{
    struct {
        uint8 byte[16];
    } overlay0;

    struct {    /*ppd type 0*/
        uint32 hcgig2_sop               :8;
        uint32 hcgig2_rsv1              :3;
        uint32 hcgig2_mcst              :1;
        uint32 hcgig2_tc                :4;
        uint32 hcgig2_dst_modid_mgidh   :8;
        uint32 hcgig2_dst_port_mgidl    :8;

        uint32 hcgig2_src_modid         :8;
        uint32 hcgig2_src_pid           :8;
        uint32 hcgig2_lbid              :8;
        uint32 hcgig2_dp                :2;
        uint32 hcgig2_rsvd_6            :3;
        uint32 hcgig2_ppd_ty            :3;

        uint32 hcgig2_dst_t             :1;
        uint32 hcgig2_dst_tgid          :3;
        uint32 hcgig2_ingress_tagged    :1;
        uint32 hcgig2_mirror_only       :1;
        uint32 hcgig2_mirror_done       :1;
        uint32 hcgig2_mirror            :1;
        uint32 hcgig2_rsvd_55_54        :2;
        uint32 hcgig2_l3                :1;
        uint32 hcgig2_label_precent     :1;
        uint32 hcgig2_vc_label_19_16    :4;
        uint32 hcgig2_vc_label_15_8     :8;
        uint32 hcgig2_vc_label_7_0      :8;

        uint32 hcgig2_vid_high          :8;
        uint32 hcgig2_vid_low           :8;
        uint32 hcgig2_pfm               :2;
        uint32 hcgig2_src_t             :1;
        uint32 hcgig2_rsvd_12_11        :2;
        uint32 hcgig2_opcode            :3;
        uint32 hcgig2_hdr_ext_len       :3;
        uint32 hcgig2_rsvd_4_0          :5;

    } ppd_ty0;

    struct {    /*ppd type 2*/
        uint32 hcgig2_sop               :8;
        uint32 hcgig2_rsv1              :3;
        uint32 hcgig2_mcst              :1;
        uint32 hcgig2_tc                :4;
        uint32 hcgig2_dst_modid_mgidh   :8;
        uint32 hcgig2_dst_port_mgidl    :8;

        uint32 hcgig2_src_modid         :8;
        uint32 hcgig2_src_pid           :8;
        uint32 hcgig2_lbid              :8;
        uint32 hcgig2_dp                :2;
        uint32 hcgig2_rsvd_6            :3;
        uint32 hcgig2_ppd_ty            :3;

        uint32 hcgig2_multi_point       :1;
        uint32 hcgig2_fwd_type          :5;
        uint32 hcgig2_rsvd_25_16        :10;
        uint32 hcgig2_dest_vp_high      :8;
        uint32 hcgig2_dest_vp_low       :8;

        uint32 hcgig2_src_vp_high       :8;
        uint32 hcgig2_src_vp_low        :8;
        uint32 hcgig2_mirror            :1;
        uint32 hcgig2_donot_modify      :1;
        uint32 hcgig2_donot_learn       :1;
        uint32 hcgig2_lag_failover      :1;
        uint32 hcgig2_rsvd_11           :1;
        uint32 hcgig2_opcode            :3;
        uint32 hcgig2_rsvd_7_0          :8;

    } ppd_ty2;
};
typedef union humber_hcgig2_header_u humber_hcgig2_header_t;

#endif /*end of _DRV_HUMBER_BE_H_*/

