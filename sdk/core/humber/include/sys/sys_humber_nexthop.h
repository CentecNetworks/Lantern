/**************************************************************************
 * sys_humber_nexthop_internal.h       :  the define and structure used only by nexthop module
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History:
 * Revision     :       R0.01
 * Author       :       Xianghong Gu
 * Date         :       2009-10-28
 * Reason       :       First Create.
 **************************************************************************/
#ifndef _SYS_HUMBER_NEXTHOP_INTERNAL_H_
#define _SYS_HUMBER_NEXTHOP_INTERNAL_H_
#include "kal.h"
#include "ctc_const.h"
#include "ctc_avl_tree.h"
#include "ctc_vector.h"
#include "ctc_debug.h"
#include "ctc_vlan.h"
#include "ctc_nexthop.h"
#include "ctc_stats.h"

#define SYS_HUMBER_NH_DROP_DESTMAP          0xFFFF
#define SYS_HUMBER_NH_MET_END_REPLICATE_OFFSET  0x3FFFF
#define SYS_HUMBER_NH_MET_DROP_UCAST_ID_UPPER    0xF
#define SYS_HUMBER_NH_MET_DROP_UCAST_ID_LOWER    0xFFF

#define SYS_HUMBER_DSNH4WREG_INDEX_FOR_BRG          0
#define SYS_HUMBER_DSNH4WREG_INDEX_FOR_BYPASS       1
#define SYS_HUMBER_DSNH4WREG_INDEX_FOR_MIRROR       2
#define SYS_HUMBER_DSNH4WREG_INDEX_FOR_UNTAGGED_BRG 3

#define SYS_HUMBER_RESV_OFFSET_FOR_FATAL_EXCEP_NUM

#define SYS_HUMBER_RESV_OFFSET_CRS_CNT_SHIFT        8
#define SYS_HUMBER_RESV_OFFSET_FATAL_EXCEP_SHIFT    4
#define SYS_HUMBER_RESV_FATAL_EXCEPTION_INDEX_BASE  48

#define SYS_HUMBER_NH_EXTERNAL_NHID_MAX_BLK_NUM     16
#define SYS_HUMBER_NH_INTERNAL_NHID_MAX_BLK_NUM     16
#define SYS_HUMBER_NH_INTERNAL_NHID_BLK_SIZE        2048
#define SYS_HUMBER_NH_INTERNAL_NHID_MAX ( (p_nh_master->max_external_nhid) + \
        (SYS_HUMBER_NH_INTERNAL_NHID_MAX_BLK_NUM *\
        SYS_HUMBER_NH_INTERNAL_NHID_BLK_SIZE))
#define SYS_HBNH_ENCODE_DESTMAP(is_mcast, gchip, port) (((is_mcast)<<21)|((gchip)<<16)|(port))
#define SYS_NH_GET_VECTOR_INDEX_BY_NHID(nhid)  (nhid - p_nh_master->max_external_nhid)
#define SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL 0x3FFF
#define SYS_HUMBER_NH_ROUTED_PORT_VID    0xFFF
#define SYS_HUMBER_NH_BOOL_TRUE 1
#define SYS_HUMBER_NH_BOOL_FALSE 0

#define SYS_NH_INFO_FLAG_IS_UNROV    (1 << 0)

/*Now we just set l2edit ptr length to 15, but this value can be configured in asic*/
#define SYS_HUMBER_DEFAULT_EDIT_PTR_LENGTH_CHECK(offset)   \
        if (offset >= (1 << SYS_HUMBER_EDIT_PTR_DEFAULT_BIT_LENGTH))\
        {\
            return CTC_E_INVALID_DSEDIT_PTR;\
        }

enum sys_nh_entry_table_type_e
{
    SYS_NH_ENTRY_TYPE_NULL,

    SYS_NH_ENTRY_TYPE_FWD = SYS_NH_ENTRY_TYPE_NULL,
    SYS_NH_ENTRY_TYPE_MET,
    SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
    SYS_NH_ENTRY_TYPE_NEXTHOP_8W,

    /* +++ l2edit +++ */
    SYS_NH_ENTRY_TYPE_L2EDIT_FROM,
    SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W = SYS_NH_ENTRY_TYPE_L2EDIT_FROM,
    SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W,
    SYS_NH_ENTRY_TYPE_L2EDIT_FLEX_4W,
    SYS_NH_ENTRY_TYPE_L2EDIT_FLEX_8W,
    SYS_NH_ENTRY_TYPE_L2EDIT_PBB_4W,
    SYS_NH_ENTRY_TYPE_L2EDIT_PBB_8W,
    SYS_NH_ENTRY_TYPE_L2EDIT_LPBK,
    SYS_NH_ENTRY_TYPE_L2EDIT_TO = SYS_NH_ENTRY_TYPE_L2EDIT_LPBK,
    /* --- l2edit --- */

    /* +++ l3edit +++ */
    SYS_NH_ENTRY_TYPE_L3EDIT_FROM,
    SYS_NH_ENTRY_TYPE_L3EDIT_NAT_4W = SYS_NH_ENTRY_TYPE_L3EDIT_FROM,
    SYS_NH_ENTRY_TYPE_L3EDIT_NAT_8W,
    SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_4W,
    SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_8W,
    SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4,
    SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6,
    SYS_NH_ENTRY_TYPE_L3EDIT_FLEX,
    SYS_NH_ENTRY_TYPE_L3EDIT_LPBK,
    SYS_NH_ENTRY_TYPE_L3EDIT_TO = SYS_NH_ENTRY_TYPE_L3EDIT_LPBK,
    /* --- l3edit --- */


    SYS_NH_ENTRY_TYPE_MAX

};
typedef enum sys_nh_entry_table_type_e sys_nh_entry_table_type_t;

enum sys_nh_entry_pld_op_type_e
{
    SYS_NH_OP_NONE = 0,
    SYS_NH_OP_ROUTE,
    SYS_NH_OP_BRIDGE,
    SYS_NH_OP_BRIDGE_VPLS,             /* BRIDGE_VPLS_VC for SGMAC */
    SYS_NH_OP_BRIDGE_INNER,
    SYS_NH_OP_MIRROR,
    SYS_NH_OP_ROUTE_NOTTL,
    SYS_NH_OP_RESERVED,
};
typedef enum sys_nh_entry_pld_op_type_e sys_nh_entry_pld_op_type_t;

enum sys_nh_entry_l2edit_type_e
{
    SYS_NH_L2EDIT_TYPE_NONE =0,
    SYS_NH_L2EDIT_TYPE_LOOPBACK,
    SYS_NH_L2EDIT_TYPE_ETH_4W,
    SYS_NH_L2EDIT_TYPE_ETH_8W,
    SYS_NH_L2EDIT_TYPE_ETH_MAC,
    SYS_NH_L2EDIT_TYPE_FLEX_8W,
    SYS_NH_L2EDIT_TYPE_PBB_4W,
    SYS_NH_L2EDIT_TYPE_PBB_8W,
    SYS_NH_L2EDIT_TYPE_RESERVED,

    SYS_NH_L2EDIT_TYPE_MAX
};
typedef enum sys_nh_entry_l2edit_type_e sys_nh_entry_l2edit_type_t;

enum sys_nh_entry_l3edit_type_e
{
    SYS_NH_L3EDIT_TYPE_NONE =0,
    SYS_NH_L3EDIT_TYPE_MPLS_4W,
    SYS_NH_L3EDIT_TYPE_MPLS_8W,
    SYS_NH_L3EDIT_TYPE_NAT_4W,
    SYS_NH_L3EDIT_TYPE_TUNNEL_V4,
    SYS_NH_L3EDIT_TYPE_TUNNEL_V6,
    SYS_NH_L3EDIT_TYPE_FLEX,
    SYS_NH_L3EDIT_TYPE_NAT_8W,
    SYS_NH_L3EDIT_TYPE_LOOPBACK,
    SYS_NH_L3EDIT_TYPE_RESERVED,

    SYS_NH_L3EDIT_TYPE_MAX
};
typedef enum sys_nh_entry_l3edit_type_e sys_nh_entry_l3edit_type_t;


/*=== DB shared entrystruct Start ===*/
struct sys_nh_db_entry_com_hdr_s
{
    uint32 offset;
    int32 ref_cnt;
    uint8 entry_type;    /*sys_nh_entry_table_type_t*/
    uint8 lchip;
    uint16 res;
};
typedef struct sys_nh_db_entry_com_hdr_s sys_nh_db_entry_com_hdr_t;

struct sys_nh_db_com_entry_s
{
    sys_nh_db_entry_com_hdr_t hdr;
    uint32 data;
};
typedef struct sys_nh_db_com_entry_s sys_nh_db_com_entry_t;

struct sys_nh_db_dsl2editeth4w_s
{
    sys_nh_db_entry_com_hdr_t hdr;
    mac_addr_t mac_da;
    uint16 output_vid;
    uint8 flags;
    uint8 ouput_vlan_is_svlan;
#define SYS_NH_DSL2EDIT_DERIVE_MCAST_MAC      (1 << 0)
};
typedef struct sys_nh_db_dsl2editeth4w_s sys_nh_db_dsl2editeth4w_t;

/*=== DB shared entrystruct End ===*/

struct sys_nh_info_dsmet_s
{
    uint32 dsmet_offset;
    uint32 next_dsmet_offset;
    uint32 dsnh_offset;
    uint32 ref_nhid;
    uint16 ucastid;
    uint16 vid;/*For IPMC*/
    uint16 replicate_num;/*If mcast no logical replicate, this num should be 0*/
    uint8 is_linkagg;
    uint8 end_local;
    uint8 port_check_discard;
    uint8 member_type;
    uint8 aps_protection_path;
    uint8 use_dsnh8w;

};
typedef struct sys_nh_info_dsmet_s sys_nh_info_dsmet_t;


struct sys_nh_info_dsnh4w_s
{
    uint32 dsnh_offset;
};
typedef struct sys_nh_info_dsnh4w_s sys_nh_info_dsnh4w_t;

struct sys_nh_info_dsl2edit_s
{
    uint32 dsl2edit_offset;
	sys_nh_entry_table_type_t  entry_tbl_type;
};
typedef struct sys_nh_info_dsl2edit_s sys_nh_info_dsl2edit_t;

struct sys_nh_info_dsl3edit_s
{
    uint32 dsl3edit_offset;
    sys_nh_entry_table_type_t  entry_tbl_type;
};
typedef struct sys_nh_info_dsl3edit_s sys_nh_info_dsl3edit_t;

struct sys_nh_param_dsnh_s
{
    uint32 dsnh_offset;
    sys_nh_entry_l2edit_type_t  l2edit_type;
    uint32 l2edit_ptr;
    sys_nh_entry_l3edit_type_t l3dit_type;
    uint32 l3edit_ptr;
    ctc_vlan_egress_edit_info_t *p_vlan_info;
    uint16 dest_vlan_ptr;
    uint8 lchip;
    uint8 dsnh_type;
#define SYS_NH_PARAM_DSNH_TYPE_IPUC                  1
#define SYS_NH_PARAM_DSNH_TYPE_BRGUC                 2
#define SYS_NH_PARAM_DSNH_TYPE_BYPASS                3
#define SYS_NH_PARAM_DSNH_TYPE_IPMC                  4
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_NONE     5
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_ROUTE    6
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_L2VPN    7
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_NONE      8
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_ROUTE     9
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_MPLS      10
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN     11
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN_NO_OP  12
#define SYS_NH_PARAM_DSNH_TYPE_DOWNMEP              13
#define SYS_NH_PARAM_DSNH_TYPE_RSPAN                    14
#define SYS_NH_PARAM_DSNH_TYPE_MPLS_OP_NONE    15
#define SYS_NH_PARAM_DSNH_TYPE_UPMEP                  16
#define SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_ROUTE     17
#define SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_MIRROR    18

#define SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_GRE       19

    uint8 use_ttl_from_pkt;
    uint8 vpls_nexthop;
    uint8 vpls_port_check;
    uint8 dscp_en;
    uint16 vpls_port;
};
typedef struct sys_nh_param_dsnh_s sys_nh_param_dsnh_t;

struct sys_nh_info_dsfwd_s
{
    uint32 dsfwd_offset;
    uint32 stats_ptr;
};
typedef struct sys_nh_info_dsfwd_s sys_nh_info_dsfwd_t;

struct sys_nh_param_dsfwd_s
{
    sys_nh_info_dsfwd_t* p_dsfwd_info;
    uint32 dsnh_offset;
    bool is_mcast;
    bool drop_pkt;
    bool nexthop_ext;
    bool service_queue_en;
    bool sequence_chk_en;
    uint32 aps_type;
    uint16 dest_id;/*UcastId or McastId*/
    uint8 dest_chipid;/*Destination global chip id*/
    uint8 lchip;/*This entry's target local chipid*/
    uint8 stats_valid;/*This entry's target local chipid*/
    uint32 stats_ptr;/*This entry's target local chipid*/
};
typedef struct sys_nh_param_dsfwd_s sys_nh_param_dsfwd_t;

/*=== Humber Nexthop DB's data structure, per NH type data structure, Start ===*/
struct sys_nh_ref_list_node_s
{
    struct sys_nh_info_com_s *p_ref_nhinfo;
    struct sys_nh_ref_list_node_s *p_next;
};
typedef struct sys_nh_ref_list_node_s sys_nh_ref_list_node_t;

struct sys_nh_info_com_hdr_s
{
    uint16 nh_entry_type; /*sys_humber_nh_type_t*/
    uint16 nh_entry_flags;
    sys_nh_info_dsfwd_t dsfwd_info[CTC_MAX_LOCAL_CHIP_NUM];
    sys_nh_ref_list_node_t *p_ref_nh_list;
};
typedef struct sys_nh_info_com_hdr_s sys_nh_info_com_hdr_t;

struct sys_nh_info_com_s
{
    sys_nh_info_com_hdr_t hdr;
    uint32 data;
};
typedef struct sys_nh_info_com_s sys_nh_info_com_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_TOCPU or DROP, or UNROV Strat ===*/
struct sys_nh_info_special_s
{
    sys_nh_info_com_hdr_t hdr;
};
typedef struct sys_nh_info_special_s sys_nh_info_special_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_TOCPU or DROP, or UNROV End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_TOCPU or DROP, or UNROV Strat ===*/
struct sys_nh_info_rspan_s
{
    sys_nh_info_com_hdr_t hdr;
    sys_nh_info_dsnh4w_t  dsnh_info;
};
typedef struct sys_nh_info_rspan_s sys_nh_info_rspan_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_TOCPU or DROP, or UNROV End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_BRGUC Strat ===*/

struct sys_nh_info_brguc_s
{
    sys_nh_info_com_hdr_t hdr;
#define SYS_NH_INFO_BRGUC_FLAG_SERVICE_QUEUE    (1 << 5)
    sys_nh_info_dsnh4w_t  dsnh_info;
    sys_nh_info_dsl2edit_t  *p_dsl2edit[CTC_MAX_LOCAL_CHIP_NUM];
    union {
        uint16 dest_gport;
        uint16 aps_bridge_group_id;
    } dest;
    uint8 nh_sub_type;/*sys_nh_param_brguc_sub_type_t*/
    uint8 rsv0;
};
typedef struct sys_nh_info_brguc_s sys_nh_info_brguc_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_BRGUC End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_BRGUC Strat ===*/
struct sys_nh_info_downmep_s
{
    sys_nh_info_com_hdr_t hdr;
    sys_nh_info_dsnh4w_t  p_dsnh_info[CTC_MAX_LOCAL_CHIP_NUM];
};
typedef struct sys_nh_info_downmep_s sys_nh_info_downmep_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_BRGUC End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_IPUC Strat ===*/
struct sys_nh_info_ipuc_s
{
    sys_nh_info_com_hdr_t hdr;
    sys_nh_info_dsnh4w_t dsnh_info;
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info[CTC_MAX_LOCAL_CHIP_NUM];/*For LAG port, every chip have its own l2edit*/
    uint16 l3ifid;
    uint16 gport;
};
typedef struct sys_nh_info_ipuc_s sys_nh_info_ipuc_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_IPUC End ===*/


/*=== DB data structure for SYS_HUMBER_NH_TYPE_MCAST Strat ===*/
struct sys_nh_mcast_meminfo_s
{
    ctc_list_pointer_node_t list_head;
    sys_nh_info_dsmet_t dsmet;
};
typedef struct sys_nh_mcast_meminfo_s sys_nh_mcast_meminfo_t;

struct sys_nh_mcast_meminfo_norepli_s
{
    sys_nh_mcast_meminfo_t comm;
};
typedef struct sys_nh_mcast_meminfo_norepli_s sys_nh_mcast_meminfo_norepli_t;

struct sys_nh_mcast_repli_node_s
{
    ctc_list_pointer_node_t list_head;
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info;
};
typedef struct sys_nh_mcast_repli_node_s sys_nh_mcast_repli_node_t;

struct sys_nh_mcast_meminfo_withrepli_s
{
    sys_nh_mcast_meminfo_t comm;
    ctc_list_pointer_t repli_list; /*sys_nh_mcast_repli_node_t*/
    uint8 free_dsnh_offset_cnt;
};
typedef struct sys_nh_mcast_meminfo_withrepli_s sys_nh_mcast_meminfo_withrepli_t;

struct sys_nh_mcast_meminfo_list_s
{
    ctc_list_pointer_t mem_list;/*List node is sys_nh_mcast_meminfo_t*/
};
typedef struct sys_nh_mcast_meminfo_list_s sys_nh_mcast_meminfo_list_t;

struct sys_nh_info_mcast_s
{
    sys_nh_info_com_hdr_t hdr;
#define SYS_NH_INFO_MCAST_FLAG_GROUP_IS_SPECIAL    (1 << 0)
#define SYS_NH_INFO_MCAST_FLAG_GROUP_IS_UP_MEP     (1 << 1)
    uint32 basic_met_offset;
    sys_nh_u16_array_t  stats_ptr;
    uint32 dsnh_offset;
    sys_nh_mcast_meminfo_list_t meminfo[CTC_MAX_LOCAL_CHIP_NUM];
};
typedef struct sys_nh_info_mcast_s sys_nh_info_mcast_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_MCAST End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_ECMP Strat ===*/
struct sys_ecmp_item_s
{
    uint32 nh_id;
    uint32 oper_nh_id;
    uint8 is_oper_valid;
    uint8 rev[3];              /* reserve for future using */
};
typedef struct sys_ecmp_item_s sys_ecmp_item_t;

struct sys_nh_info_ecmp_s
{
    sys_nh_info_com_hdr_t hdr;
    uint8 valid_item_cnt;
    uint8 oif_cnt;
    uint8 oif_changed;
    uint8 oif_need;              /* need return oif */
    sys_ecmp_item_t item_array[CTC_MAX_ECPN];
    sys_rpf_info_t rpf_array[CTC_MAX_ECPN];
};
typedef struct sys_nh_info_ecmp_s sys_nh_info_ecmp_t;

struct sys_info_ecmp_s
{
    sys_nh_info_com_t* p_nhinfo;
    uint16 l3oif;
};
typedef struct sys_info_ecmp_s sys_info_ecmp_t;
/*=== DB data structure for SYS_HUMBER_NH_TYPE_ECMP End ===*/

/*=== DB data structure for SYS_HUMBER_NH_TYPE_MPLS Strat ===*/
struct sys_nh_info_mpls_edit_info_s
{
    uint32 dsl3edit_offset[CTC_MAX_LOCAL_CHIP_NUM];
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info[CTC_MAX_LOCAL_CHIP_NUM];/*For LAG port, every chip have its own l2edit*/
    uint16 l3ifid;
    uint16 gport;
    uint8 dsl3edit_type; /*sys_nh_entry_l3edit_type_t*/
    uint8 lchip_bmp;
    uint8 l3if_en;
};
typedef struct sys_nh_info_mpls_edit_info_s sys_nh_info_mpls_edit_info_t;

struct sys_nh_info_mpls_s
{
    sys_nh_info_com_hdr_t hdr;
#define SYS_NH_INFO_MPLS_FLAG_IS_UNROV      (SYS_NH_INFO_FLAG_IS_UNROV)
#define SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W    (1 << 1)
#define SYS_NH_INFO_MPLS_FLAG_APS_EN        (1 << 2)

    uint32  dsnh_offset;
    uint16  aps_group_id;
    sys_nh_info_mpls_edit_info_t working_path;
    sys_nh_info_mpls_edit_info_t protection_path;
};
typedef struct sys_nh_info_mpls_s sys_nh_info_mpls_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_MPLS End ===*/

/*added by MPLS-BFD*/
struct sys_nh_info_bfd_s
{
    sys_nh_info_com_hdr_t hdr;
    uint32 dsnh_offset;
    uint32 l2_edit_ptr;
    uint32 l3_edit_ptr;
};
typedef struct sys_nh_info_bfd_s sys_nh_info_bfd_t;


/*=== DB data structure for SYS_HUMBER_NH_TYPE_IP_TUNNEL BEGIN ===*/

struct sys_nh_info_ip_tunnel_ip_idx_s
{
    uint8 ipv4_sa_idx_valid;
    uint32 ipv4_sa_idx;
    uint32 ipv6_sa_idx;
    uint32 ipv6_da_idx;
};
typedef struct sys_nh_info_ip_tunnel_ip_idx_s sys_nh_info_ip_tunnel_ip_idx_t;

struct sys_nh_info_ip_tunnel_edit_info_s
{
    uint32 dsl3edit_offset[CTC_MAX_LOCAL_CHIP_NUM];
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info[CTC_MAX_LOCAL_CHIP_NUM];/*For LAG port, every chip have its own l2edit*/
    sys_nh_info_ip_tunnel_ip_idx_t ip_idx[CTC_MAX_LOCAL_CHIP_NUM];
    uint16 l3ifid;
    uint16 gport;
    uint8 lchip_bmp;
    uint8 dsl2edit_type; /*sys_nh_entry_l2edit_type_t*/
    uint8 dsl3edit_type; /*sys_nh_entry_l3edit_type_t*/
    uint8 l3if_en;
};
typedef struct sys_nh_info_ip_tunnel_edit_info_s sys_nh_info_ip_tunnel_edit_info_t;

struct sys_nh_info_ip_tunnel_s
{
    sys_nh_info_com_hdr_t hdr;
#define SYS_NH_INFO_IP_TUNNEL_FLAG_IS_UNROV      (SYS_NH_INFO_FLAG_IS_UNROV)
#define SYS_NH_INFO_IP_TUNNEL_FLAG_USE_DSNH8W    (1 << 1)
    uint32  dsnh_offset;
    sys_nh_info_ip_tunnel_edit_info_t ip_tunnel_edit_info;
};
typedef struct sys_nh_info_ip_tunnel_s sys_nh_info_ip_tunnel_t;

/*=== DB data structure for SYS_HUMBER_NH_TYPE_IP_TUNNEL END ===*/


struct sys_nh_info_ptr_s
{
    sys_nh_info_com_t *p_info;
};
typedef struct sys_nh_info_ptr_s sys_nh_info_ptr_t;


/*=== DB data structure for SYS_HUMBER_NH_TYPE_FLEX Strat ===*/
struct sys_nh_info_flex_s
{
    sys_nh_info_com_hdr_t hdr;

#define SYS_NH_INFO_FLEX_FLAG_IS_UNROV      (SYS_NH_INFO_FLAG_IS_UNROV)
#define SYS_NH_INFO_FLEX_FLAG_USE_DSNH8W    (1 << 1)

    sys_nh_info_dsnh4w_t dsnh_info;
    sys_nh_info_dsl2edit_t  *p_dsl2edit[CTC_MAX_LOCAL_CHIP_NUM];

    sys_nh_info_dsl3edit_t  *p_dsl3edit[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 flag;
    uint16 gport;
    uint8  rsv0[2];
};
typedef struct sys_nh_info_flex_s sys_nh_info_flex_t;

/*=== Humber Nexthop DB's data structure, per NH type data structure, End ===*/

enum sys_humber_nh_res_dsnh_offset_e
{
    SYS_HBNH_RES_DSNH_PTR_REMOTE_CPU           = 0, /*Entry size if 1*/
    SYS_HBNH_RES_DSNH_PTR_RAW_PACKET_ELOG_CPU = 1, /*Entry size if 1*/
    SYS_HBNH_RES_DSNH_PTR_SWAP_MAC = 2, /*Entry size if 1*/
    SYS_HBNH_RES_DSNH_PTR_MAX = 16
};
typedef enum sys_humber_nh_res_dsnh_offset_e sys_humber_nh_res_dsnh_offset_t;
enum sys_humber_nh_res_offset_type_e
{
    SYS_HBNH_RES_OFFSET_TYPE_NONE = 0,

    SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH = SYS_HBNH_RES_OFFSET_TYPE_NONE,  /*port tagged mode*/
    SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH,
    SYS_HBNH_RES_OFFSET_TYPE_MIRROR_NH,
    SYS_HBNH_RES_OFFSET_TYPE_RAW_PACKET_ELOG_CPU,
    SYS_HBNH_RES_OFFSET_TYPE_UNTAGGED_BRIDGE_NH, /*port untagged mode*/
    SYS_HBNH_RES_OFFSET_TYPE_REMOTECPU_NH,
    SYS_HBNH_RES_OFFSET_TYPE_SWAPMAC_NH,
    SYS_HBNH_RES_OFFSET_TYPE_MAX
};
typedef enum sys_humber_nh_res_offset_type_e sys_humber_nh_res_offset_type_t;

struct sys_nh_offset_attr_s
{
    uint32 offset_base;
    uint16 entry_size;
    uint16 entry_num;
};
typedef struct sys_nh_offset_attr_s sys_nh_offset_attr_t;

struct sys_nh_table_entry_info_s
{
    uint32 table_id;
    uint8  entry_size;
    uint8  opf_pool_type;
    uint8  alloc_dir; /*0:forward  alloc ,1,reverse alloc*/
};
typedef struct sys_nh_table_entry_info_s sys_nh_table_entry_info_t;


enum sys_humber_nh_type_e
{
    SYS_HUMBER_NH_TYPE_NULL,
    SYS_HUMBER_NH_TYPE_MCAST,
    SYS_HUMBER_NH_TYPE_BRGUC,
    SYS_HUMBER_NH_TYPE_IPUC,
    SYS_HUMBER_NH_TYPE_MPLS,
    SYS_HUMBER_NH_TYPE_ECMP,/*For IPUC, MPLS, etc*/
    SYS_HUMBER_NH_TYPE_DROP,
    SYS_HUMBER_NH_TYPE_TOCPU,
    SYS_HUMBER_NH_TYPE_UNROV,
    SYS_HUMBER_NH_TYPE_ILOOP,
    SYS_HUMBER_NH_TYPE_ELOOP,
    SYS_HUMBER_NH_TYPE_RSPAN,
    SYS_HUMBER_NH_TYPE_DOWNMEP,
    SYS_HUMBER_NH_TYPE_IP_TUNNEL, /*Added by IP Tunnel*/
    SYS_HUMBER_NH_TYPE_FLEX,
    SYS_HUMBER_NH_TYPE_MAX
};
typedef enum sys_humber_nh_type_e sys_humber_nh_type_t;

/*=== Humber Nexthop parameter data structure Start ===*/
struct sys_nh_param_hdr_s
{
    sys_humber_nh_type_t nh_param_type;
    uint32 nhid;
    bool is_internal_nh;
    bool dsfwd_valid;
    bool  stats_valid;
    sys_nh_offset_array_t dsfwd_offset;
    sys_nh_u16_array_t   stats_ptr;

};
typedef struct sys_nh_param_hdr_s sys_nh_param_hdr_t;

struct sys_nh_param_com_s
{
    sys_nh_param_hdr_t hdr;
};
typedef struct sys_nh_param_com_s sys_nh_param_com_t;


struct sys_nh_param_special_s
{
    sys_nh_param_hdr_t hdr;
};
typedef struct sys_nh_param_special_s sys_nh_param_special_t;

struct sys_nh_param_brguc_s
{
    sys_nh_param_hdr_t hdr;
    sys_nh_param_brguc_sub_type_t nh_sub_type;
    ctc_vlan_egress_edit_info_t *p_vlan_edit_info;
    ctc_vlan_egress_edit_info_t *p_vlan_edit_info_prot_path;
    uint32 dsnh_offset;/*Used for vlan editing*/
    uint16 gport;/*gport or aps bridge group id*/
    uint16 dest_vlan_ptr;
};
typedef struct sys_nh_param_brguc_s sys_nh_param_brguc_t;


struct sys_nh_param_downmep_s
{
    sys_nh_param_hdr_t hdr;
    uint16 dest_vlan_ptr;
};
typedef struct sys_nh_param_downmep_s sys_nh_param_downmep_t;


enum sys_nh_param_mcast_opcode_e
{
    SYS_HBNH_PARAM_MCAST_ADD_MEMBER            = 1,
    SYS_HBNH_PARAM_MCAST_DEL_MEMBER            = 2,
    SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_DSNH    = 3,
    SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_GRP       = 4,
    SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_MEMBER    = 5,
    SYS_HBNH_PARAM_MCAST_DEL_SPECIAL_MEMBER    = 6,
    SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_ADD     = 7,
    SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_DEL     = 8
};
typedef enum sys_nh_param_mcast_opcode_e sys_nh_param_mcast_opcode_t;

struct sys_nh_param_mcast_s {
    sys_nh_param_hdr_t hdr;
    uint32 groupid;
    sys_nh_param_mcast_member_t *p_member;
    uint32 opcode;
};
typedef struct sys_nh_param_mcast_s sys_nh_param_mcast_t;

struct sys_nh_param_ipuc_s
{
    sys_nh_param_hdr_t hdr;
    mac_addr_t mac;
    uint32 dsnh_offset;
    bool  is_unrov_nh;

    sys_nh_entry_change_type_t change_type;
    ctc_nh_oif_info_t oif;
};
typedef struct sys_nh_param_ipuc_s sys_nh_param_ipuc_t;



struct sys_nh_param_ecmp_s
{
    sys_nh_param_hdr_t hdr;
    sys_nh_param_ecmp_api_t *p_ecmp_param;
};
typedef struct sys_nh_param_ecmp_s sys_nh_param_ecmp_t;


struct sys_nh_param_mpls_s
{
    sys_nh_param_hdr_t hdr;
    uint32 dsnh_offset;
    bool use_dsnh8w;/*False stand for use dsnh4w*/
    bool is_unrov;
    sys_nh_entry_change_type_t change_type;
    sys_nh_mpls_param_t *p_mpls_nh_param;
    sys_nh_mpls_param_t *p_mpls_nh_param_protection;
};
typedef struct sys_nh_param_mpls_s sys_nh_param_mpls_t;



/* IP Tunnel nexthop info*/
struct sys_nh_param_ip_tunnel_s
{
    sys_nh_param_hdr_t hdr;
    uint32 dsnh_offset;
    sys_nh_entry_change_type_t change_type;

    ctc_ip_nh_param_t ip_nh_param;
};
typedef struct sys_nh_param_ip_tunnel_s sys_nh_param_ip_tunnel_t;
struct sys_nh_param_iloop_s
{
    sys_nh_param_hdr_t hdr;
    ctc_loopback_nexthop_param_t *p_iloop_param;
};
typedef struct sys_nh_param_iloop_s sys_nh_param_iloop_t;

struct sys_nh_param_rspan_s
{
    sys_nh_param_hdr_t hdr;
    ctc_rspan_nexthop_param_t *p_rspan_param;
    uint32 dsnh_offset;
};
typedef struct sys_nh_param_rspan_s sys_nh_param_rspan_t;


struct sys_nh_param_crscnt_s
{
    bool swap_mac;
    uint16 srcport;
    uint16 destport;
};
typedef struct sys_nh_param_crscnt_s sys_nh_param_crscnt_t;


struct sys_nh_param_flex_s
{
    sys_nh_param_hdr_t hdr;
    ctc_flex_nh_param_t nh_flex_param;
};
typedef struct sys_nh_param_flex_s sys_nh_param_flex_t;

/*=== Humber Nexthop parameter data structure End ===*/


typedef int32 (* p_sys_nh_create_cb_t)(sys_nh_param_com_t* p_com_nh_para, \
        sys_nh_info_com_t* p_com_db);
typedef int32 (* p_sys_nh_delete_cb_t)(sys_nh_info_com_t * p_data);
typedef int32 (* p_sys_nh_update_cb_t)(sys_nh_info_com_t * p_data, \
        sys_nh_param_com_t * p_com_nh_para);

struct sys_humber_nh_master_s
{
    kal_mutex_t*         p_mutex;
    p_sys_nh_create_cb_t callbacks_nh_create[SYS_HUMBER_NH_TYPE_MAX];
    p_sys_nh_delete_cb_t callbacks_nh_delete[SYS_HUMBER_NH_TYPE_MAX];
    p_sys_nh_update_cb_t callbacks_nh_update[SYS_HUMBER_NH_TYPE_MAX];
    uint32 max_external_nhid;
    ctc_vector_t* external_nhid_vec;
    sys_nh_offset_attr_t sys_hbnh_resolved_offset[CTC_MAX_LOCAL_CHIP_NUM][SYS_HBNH_RES_OFFSET_TYPE_MAX];
    uint32                    ipmc_phyif_resolved_l2edit[CTC_MAX_LOCAL_CHIP_NUM];
    ctc_avl_tree_t*      dsl2edit4w_tree;
    ctc_vector_t* internal_nhid_vec;
    sys_nh_offset_array_t port_crscnt_base;
    sys_nh_offset_array_t vlan_crscnt_base;
    sys_nh_offset_array_t fatal_excp_base;
    uint32    max_glb_met_sram_offset;
    uint32    max_glb_nh_sram_offset;
    uint32 *p_occupid_met_offset_bmp;       /*Occupid global DsMet sram offset bitmap */
    uint32 *p_occupid_nh_offset_bmp;        /*Occupid global DsNexthop sram offset bitmap */

    uint32 *p_dsnh_offset_by_vlan[CTC_MAX_LOCAL_CHIP_NUM];
    uint8     remote_met_use_nhp;
    uint8     glb_alloc_nexhop;
    uint8     use_epe_parser_ttl_in_ipmc;
    uint8     edit_ptr_bits_num;
    uint32    acl_redirect_fwd_ptr_num;     /**<The number of ds_fwd_ptr reserved for acl redirect function*/
    uint32 local_met_num;                   /**< value 0: indicate local met share sram with dsFwd */
    uint32 local_nh_4w_num;                 /**< value 0: indicate local nexthop 4w share sram with local nexthop 8w */
    uint32 l2_edit_4w_num;                  /**< value 0: indicate l2edit 4w share sram with l2edit 8w */
    uint32 l3_edit_4w_num;                  /**< value 0: indicate l3edit 4w share sram with l3edit 8w */
};
typedef struct sys_humber_nh_master_s sys_humber_nh_master_t;

#define SYS_NH_CREAT_LOCK(mutex_type)                   \
    {                                                   \
        kal_mutex_create(&mutex_type);                   \
        if (NULL == mutex_type)                         \
            CTC_ERROR_RETURN(CTC_E_FAIL_CREATE_MUTEX);  \
    }

#define SYS_NH_LOCK(mutex_type)\
            kal_mutex_lock(mutex_type)

#define SYS_NH_UNLOCK(mutex_type)\
            kal_mutex_unlock(mutex_type)

#define SYS_NH_DESTROY_LOCK(mutex_type)\
            kal_mutex_destroy(mutex_type)


#define SYS_NH_DBG_FUNC()                          \
                    {\
                    CTC_DEBUG_OUT_FUNC(nexthop, nexthop, NH_SYS);\
                    }

#define SYS_NH_DBG_INFO(FMT, ...)                          \
    {                                                      \
        CTC_DEBUG_OUT_INFO(nexthop, nexthop, NH_SYS, FMT, ##__VA_ARGS__);  \
    }

#define SYS_NH_DBG_DUMP(FMT, ...)                          \
    {                                                      \
        CTC_DEBUG_OUT_DUMP(FMT, ##__VA_ARGS__);  \
    }

#define SYS_NH_ECMP_OIF_SET(ecmpinfo)                            \
    {                                                        \
        switch((ecmpinfo).p_nhinfo->hdr.nh_entry_type) \
        { \
            case SYS_HUMBER_NH_TYPE_IPUC: \
            { \
                sys_nh_info_ipuc_t *p_ipuc_info; \
                p_ipuc_info = (sys_nh_info_ipuc_t *)((ecmpinfo).p_nhinfo); \
                (ecmpinfo).l3oif = p_ipuc_info->l3ifid; \
                break; \
            } \
            case SYS_HUMBER_NH_TYPE_MPLS: \
            { \
                sys_nh_info_mpls_t *p_mpls_info; \
                p_mpls_info = (sys_nh_info_mpls_t *)((ecmpinfo).p_nhinfo); \
                (ecmpinfo).l3oif = p_mpls_info->working_path.l3ifid; \
                break; \
            } \
            case SYS_HUMBER_NH_TYPE_IP_TUNNEL: \
            { \
                sys_nh_info_ip_tunnel_t *p_ip_tunnel_info; \
                p_ip_tunnel_info = (sys_nh_info_ip_tunnel_t *)((ecmpinfo).p_nhinfo); \
                (ecmpinfo).l3oif = p_ip_tunnel_info->ip_tunnel_edit_info.l3ifid; \
                break; \
            } \
            case SYS_HUMBER_NH_TYPE_DROP: \
            case SYS_HUMBER_NH_TYPE_TOCPU: \
            { \
                (ecmpinfo).l3oif = SYS_L3IF_INVALID_L3IF_ID; \
                break; \
            } \
            default: \
                return CTC_E_INVALID_NH_TYPE; \
        } \
    }

/*
    1. (DsFwd.nexthopPtr[19:18] << 3) | (DsFwd.nexthopPtr[9:8] << 1)|DsFwd.nexthopPtr[17]: use to calculate words(4byts) numbers to be removed from the loopback packet header;
    2. DsFwd.nexthopPtr[16]: CustomerId is valid, Stands for the loopback packet have customerId in it;
    3. DsFwd.nexthopPtr[15]: If set, Priority and color will be mapped from packet's exp field in CustomerId;
    4. DsFwd.nexthopPtr[14:12]: Used to store loopback packet type if DsFwd.nexthopPtr[11] is set;
    5. DsFwd.nexthopPtr[11]: If set, loopback packet type will be got from DsFwd.nexthopPtr[14:12];
    6. DsFwd.nexthopPtr[10]: vpls port type;
    7. DsFwd.nexthopPtr[7:0]: Used to store loopback localPhyPort;
*/
#define SYS_HUMBER_NH_ENCODE_ILOOP_DSNH(lport, vpls_port, \
    pkt_type_valid, pkt_type, map_exp, \
    cid_valid, remove_words) \
        (lport | (vpls_port << 10) | (pkt_type_valid << 11) | \
        ((pkt_type & 0x7)  << 12) | (map_exp << 15) | (cid_valid << 16) |\
        ((remove_words & 1) << 17) | (((remove_words >> 1) & 0x3) << 8) | \
        (((remove_words >> 3) & 0x3) << 18))


extern int32
sys_humber_nh_init(ctc_nh_global_cfg_t * nh_cfg);

extern int32
sys_humber_nh_add_remove_dsnh_offset_by_vlan(uint16 vlan_id, bool is_add);


extern int32
sys_humber_nh_get_dsnh_offset_by_vlan(uint16 vlan_id, uint8 lchip, uint32 *p_offset);

/*port cross connect*/
extern int32
sys_humber_nh_update_port_crscnt_nexthop(sys_nh_param_crscnt_t *p_crscnt_param);

extern int32
sys_humber_nh_write_asic_table(uint8 lchip,
    sys_nh_entry_table_type_t table_type, uint32 offset, void* value);
extern int32
sys_humber_nh_write_entry_dsnh4w(sys_nh_param_dsnh_t *p_dsnh_param);
extern int32
sys_humber_nh_write_entry_dsnh8w(sys_nh_param_dsnh_t *p_dsnh_param);

extern int32
sys_humber_nh_write_entry_dsfwd(sys_nh_param_dsfwd_t *p_dsfwd_param);

extern int32
sys_humber_nh_db_set_entry(sys_nh_db_com_entry_t** pp_com_entry);
extern int32
sys_humber_nh_db_remove_entry(sys_nh_db_com_entry_t* p_com_entry);
extern int32
sys_humber_nh_db_update_entry(sys_nh_db_com_entry_t* p_old_entry,
                              sys_nh_db_com_entry_t** pp_new_entry);

extern int32
sys_humber_nh_check_max_glb_nh_sram_offset(uint32 offset);
extern int32
sys_humber_nh_check_max_glb_met_sram_offset(uint32 offset);
extern int32
sys_humber_nh_get_resolved_offset(sys_humber_nh_res_offset_type_t type,
                                  uint32* p_offset);
extern int32
sys_humber_nh_get_ipmc_phyif_rsv_l2edit_offset(uint8 lchip, uint32 *p_l2edit_offset);
extern int32
sys_humber_nh_get_nhinfo_by_nhid(uint32 nhid, sys_nh_info_com_t **pp_nhinfo);

extern int32
sys_humber_nh_add_ref_item(uint32 nhid, sys_nh_info_com_t *p_ref_nhinfo, sys_info_ecmp_t *p_ecmpinfo);
extern int32
sys_humber_nh_remove_ref_item(uint32 nhid, sys_nh_info_com_t *p_ref_nhinfo, sys_info_ecmp_t *p_ecmpinfo);

extern int32
sys_humber_nh_get_fatal_excp_dsnh_offset(uint8 lchip, uint32 *p_offset);

extern int32
sys_humber_nh_global_dync_entry_set_default(uint8 lchip, uint32 min_offset, uint32 max_offset);
extern int32
sys_humber_nh_offset_alloc(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint32* p_offset);
extern int32
sys_humber_nh_offset_alloc_with_multiple(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint16 multi, uint32* p_offset);
extern int32
sys_humber_nh_reverse_offset_alloc(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint32* p_offset);
extern int32
sys_humber_nh_reverse_offset_alloc_with_multiple(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint16 multi, uint32* p_offset);

extern int32
sys_humber_nh_offset_free(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                          uint32 entry_num, uint32 offset);
extern int32
sys_humber_nh_create_brguc_cb(sys_nh_param_com_t* p_com_nh_para,
                              sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_brguc_cb(sys_nh_info_com_t* data);

extern int32
sys_humber_nh_create_downmep_cb(sys_nh_param_com_t* p_com_nh_para,
                              sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_downmep_cb(sys_nh_info_com_t* data);

extern int32
sys_humber_nh_create_mcast_cb(sys_nh_param_com_t* p_com_nh_para,
                              sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_mcast_cb(sys_nh_info_com_t* p_data);
extern int32
sys_humber_nh_update_mcast_cb(sys_nh_info_com_t* p_nh_info,
                              sys_nh_param_com_t* p_para/*Member info*/);
extern int32
sys_humber_nh_create_ipuc_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_ipuc_cb(sys_nh_info_com_t *p_data);
extern int32
sys_humber_nh_update_ipuc_cb(sys_nh_info_com_t* p_nh_db,
        sys_nh_param_com_t* p_para);

extern int32
sys_humber_nh_create_special_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_special_cb(sys_nh_info_com_t* p_data);

extern int32
sys_humber_nh_ecmp_update_item(sys_nh_info_ecmp_t* p_nhdb, uint32 nh_id);

extern int32
sys_humber_nh_create_ecmp_cb(sys_nh_param_com_t* p_com_nh_para, sys_nh_info_com_t* p_com_db);

extern int32
sys_humber_nh_delete_ecmp_cb(sys_nh_info_com_t* p_ecmp_param);

extern int32
sys_humber_nh_update_ecmp_cb(sys_nh_info_com_t* p_ecmp_info, sys_nh_param_com_t* p_ecmp_param);

extern int32
sys_humber_nh_create_mpls_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_mpls_cb(sys_nh_info_com_t *p_data);
extern int32
sys_humber_nh_update_mpls_cb(sys_nh_info_com_t* p_nh_db,
        sys_nh_param_com_t* p_para);
extern int32
sys_humber_nh_create_iloop_cb(sys_nh_param_com_t* p_com_nh_para,
        sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_iloop_cb(sys_nh_info_com_t* p_data);
extern int32
sys_humber_nh_update_iloop_cb(sys_nh_info_com_t* p_iloop_info, sys_nh_param_com_t* p_iloop_param);

extern int32
sys_humber_nh_create_rspan_cb(sys_nh_param_com_t* p_com_nh_para,
        sys_nh_info_com_t* p_com_db);
extern int32
sys_humber_nh_delete_rspan_cb(sys_nh_info_com_t* p_data);

extern int32
sys_humber_nh_api_create(sys_nh_param_com_t* p_nh_com_para);
extern int32
sys_humber_nh_api_delete(uint32 nhid, sys_humber_nh_type_t nhid_type);
extern int32
sys_humber_nh_api_update(uint32 nhid, sys_nh_param_com_t* p_nh_com_para);
extern int32
sys_humber_nh_api_init(ctc_nh_global_cfg_t * nh_cfg);

extern int32
sys_humber_nh_debug_cp_dsfwd(uint8 lchip, uint32 src_cc_lport, uint32 src_fwd_ptr);

extern int32
sys_humber_nh_get_flags_nolock(uint32 nhid, uint16* p_nh_flags);

extern int32
sys_humber_nh_get_entry_dsfwd(uint8 lchip, uint32 dsfwd_offset, void* p_dsfwd);

extern int32
sys_humber_nh_get_l3ifid_nolock(uint32 nhid, uint16 *p_l3ifid);

extern int32
sys_humber_nh_get_dsfwd_offset_nolock(uint32 nhid, sys_nh_offset_array_t offset_array);

extern int32
sys_humber_nh_set_glb_nh_sram_offset(uint32 start_offset, uint32 entry_num, bool is_set);
extern int32
sys_humber_nh_set_glb_met_sram_offset(uint32 start_offset, uint32 entry_num, bool is_set);

extern int32
sys_humber_nh_check_glb_nh_sram_offset(uint32 start_offset, uint32 entry_num,
                                        bool should_not_inuse);
extern int32
sys_humber_nh_check_glb_met_sram_offset(uint32 start_offset, uint32 entry_num,
                                        bool should_not_inuse);

extern bool
sys_humber_is_glb_alloc_nexthop(void);

extern bool
sys_humber_is_remote_met_use_nexthop(void);

extern int32
sys_humber_nh_add_stats_action(uint32 nhid);

extern int32
sys_humber_nh_del_stats_action(uint32 nhid);

extern int32
sys_humber_nh_get_stats_result(uint32 nhid, ctc_stats_basic_t* p_stats);

extern int32
sys_humber_nh_reset_stats_result(uint32 nhid);

extern int32
sys_humer_nh_get_max_external_nhid(uint32* nhid);
/*added by mpls-bfd*/
extern int32
sys_humber_nh_create_ip_bfd_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);

extern int32
sys_humber_nh_create_mpls_bfd_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);

extern int32
sys_humber_nh_delete_ip_bfd_cb(sys_nh_info_com_t* p_data);

extern int32
sys_humber_nh_delete_mpls_bfd_cb(sys_nh_info_com_t* p_data);


/*added by ip tunnel*/

extern int32
sys_humber_nh_ip_tunnel_init(void);

extern int32
sys_humber_nh_create_ip_tunnel_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db);

extern int32
sys_humber_nh_delete_ip_tunnel_cb(sys_nh_info_com_t *p_data);

extern int32
sys_humber_nh_update_ip_tunnel_cb(sys_nh_info_com_t* p_nh_db,
    sys_nh_param_com_t* p_para);

extern int32
sys_humber_nh_delete_flex_cb(sys_nh_info_com_t* p_flex_info);
extern int32
sys_humber_nh_create_flex_cb(sys_nh_param_com_t* p_com_nh_para,
                             sys_nh_info_com_t* p_com_db);
#endif /*_SYS_HUMBER_NEXTHOP_INTERNAL_H_*/

