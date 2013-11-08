/**************************************************************************
 * sys_humber_nexthop.h       :  the global define and structure of nexthop module
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History:
 * Revision     :       R0.01
 * Author       :       Xianghong Gu
 * Date         :       2009-10-27
 * Reason       :       First Create.
 **************************************************************************/
#ifndef _SYS_HUMBER_NEXTHOP_H_
#define _SYS_HUMBER_NEXTHOP_H_
#include "kal.h"
#include "ctc_linklist.h"
#include "ctc_vlan.h"
#include "ctc_nexthop.h"
#include "ctc_stats.h"

typedef uint32 sys_nh_offset_array_t[CTC_MAX_LOCAL_CHIP_NUM];

struct sys_humber_nh_init_s
{
    uint32 max_external_nhid_num;
};
typedef struct sys_humber_nh_init_s sys_humber_nh_init_t;

/*global reserved nexthop */
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_NONE             0
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_DROP              1
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_TOCPU            2

#define SYS_HUMBER_NH_RESOLVED_NHID_MAX              3

#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_BRIDGE_NH    3
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_BYPASS_NH    4
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_MIRROR_NH    5
#define SYS_HUMBER_NH_RESOLVED_NHID_FOR_RAW_PACKET_ELOG_CPU    6


#define SYS_HUMBER_NHID_EXTERNAL_MAX                  8192
#define SYS_HUMBER_NHID_INTERNAL_START  (SYS_HUMBER_NHID_EXTERNAL_MAX)
#define SYS_HUMBER_NH_INVALID_OFFSET        0xFFFFFFFF
#define SYS_HUMBER_NH_INVALID_NHID          0xFFFFFFFF

//#define SYS_HUMBER_NH_DEBUG_ON

#define SYS_NHID_EXTERNAL_VALID_CHECK(nhid)           \
    {                                                    \
        if(nhid < SYS_HUMBER_NH_RESOLVED_NHID_MAX ||      \
            nhid >= (p_nh_api_master->max_external_nhid)){        \
            return CTC_E_INVALID_NHID;                    \
        };                                                 \
    }

#define SYS_NH_BRGU_USED_FLAG_CHECK(TYPE,FLAG,RESULT) \
    {\
        if( (SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC == (TYPE)) || ( SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE == (TYPE)))\
        {\
            if( ((FLAG) & SYS_BRGUC_BASIC_NH_USED ) != 0)\
                (RESULT) |= SYS_BRGUC_BASIC_NH_USED;\
        }\
        if(( SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED == (TYPE)) || ( SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE == (TYPE)))\
        {\
            if( ((FLAG) & SYS_BRGUC_UNTAGGED_NH_USED ) != 0)\
                (RESULT) |= SYS_BRGUC_UNTAGGED_NH_USED;\
        }\
        if( SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS == (TYPE) || ( SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE == (TYPE)))\
        {\
            if( ((FLAG) & SYS_BRGUC_BYPASS_NH_USED ) != 0)\
                (RESULT) |= SYS_BRGUC_BYPASS_NH_USED;\
        }\
        if( SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU == (TYPE) || ( SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE == (TYPE)))\
        {\
            if( ((FLAG) & SYS_BRGUC_RAW_PACKET_ELOG_CPU_NH_USED ) != 0)\
                (RESULT) |= SYS_BRGUC_RAW_PACKET_ELOG_CPU_NH_USED;\
        }\
        if( SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE == (TYPE) || ( SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE == (TYPE)))\
        {\
            if( ((FLAG) & SYS_BRGUC_SERVICE_QUEUE_NH_USED ) != 0)\
                (RESULT) |= SYS_BRGUC_SERVICE_QUEUE_NH_USED;\
        }\
    }

typedef uint16 sys_nh_u16_array_t[CTC_MAX_LOCAL_CHIP_NUM];

enum sys_nh_param_mcast_member_type_s
{
    SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS = 0,
    SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE,
    SYS_NH_PARAM_IPMC_MEM_LOCAL,
    SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH,
    SYS_NH_PARAM_MCAST_MEM_REMOTE,
    SYS_NH_PARAM_BRGMC_MEM_UPMEP,
    SYS_NH_PARAM_BRGMC_MEM_RAPS,
    SYS_NH_PARAM_MCAST_MEM_INVALID
};
typedef enum sys_nh_param_mcast_member_type_s sys_nh_param_mcast_member_type_t;


struct sys_nh_param_mcast_member_s
{
    sys_nh_param_mcast_member_type_t member_type;
    uint32 ref_nhid;/*Reference other nhid, eg egress vlan translation nhid*/
    uint16 destid;/*local portid, LAGID without 0x1F(eg.0x1F01), global dest chipid, aps bridge groupid*/
    uint16 vid;/*For IPMC*/
    uint8 is_linkagg;
#define SYS_NH_PARAM_IS_LINKAGG_TRUE     1
#define SYS_NH_PARAM_IS_LINKAGG_FALSE    0
    uint8 lchip;/*Local chip this member will be added to */
    uint8 l3if_type;/*For IPMC*/
#define SYS_NH_PARAM_L3IF_ROUTED_PORT    1
#define SYS_NH_PARAM_L3IF_VLAN_IF        2
#define SYS_NH_PARAM_L3IF_SUB_IF         3
#define SYS_NH_PARAM_L3IF_VLAN_PORT   4
#define SYS_NH_PARAM_L3IF_NONE           5
    uint8 is_protection_path;/*Used for aps bridge*/
    uint8 l3if_vlan_port;

    uint8 port_check_discard;
    uint8 rsv[2];
};
typedef struct sys_nh_param_mcast_member_s sys_nh_param_mcast_member_t;


enum sys_nh_param_brguc_sub_type_e
{
    SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE =  0,
    SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC = (1 << 0),
    SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED = (1 << 1) , /*apply for port untagged mode*/
    SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS = (1 << 2),
    SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU = (1 << 3),
    SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE = (1 << 4),
    /*Output vlan edit*/
    SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT = (1 << 5),
    SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT = (1 << 6),
    SYS_NH_PARAM_BRGUC_SUB_TYPE_DOWN_MEP = (1 << 7),

    SYS_NH_PARAM_BRGUC_SUB_TYPE_MAX
};
typedef enum sys_nh_param_brguc_sub_type_e sys_nh_param_brguc_sub_type_t;

struct sys_nh_param_mcast_group_s
{
    uint32                       nhid;
    uint32                       opcode;  /*sys_nh_param_mcast_opcode_t */
    sys_nh_param_mcast_member_t  mem_info;
    sys_nh_offset_array_t        fwd_offset;
    sys_nh_u16_array_t           stats_ptr;
    bool                         is_stats_enable;
    uint8                        is_nhid_valid;
    uint8                        rsv[3];
};
typedef struct sys_nh_param_mcast_group_s sys_nh_param_mcast_group_t;

enum sys_nh_entry_change_type_e {
    SYS_NH_CHANGE_TYPE_NULL,

    SYS_NH_CHANGE_TYPE_FWD_TO_UNROV,
    SYS_NH_CHANGE_TYPE_UNROV_TO_FWD,
    SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR,

    SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM,
    SYS_NH_CHANGE_TYPE_ECMP_REMOVE_ITEM,

    SYS_NH_CHANGE_TYPE_INVALID
};
typedef enum sys_nh_entry_change_type_e sys_nh_entry_change_type_t;

/* mpls nexthop operation */
enum sys_nh_mpls_op_e
{
    SYS_NH_MPLS_OP_SWITCH,                   /**<Only do vpls port check,and packet will keep unchanged.can be used to do switch */
    /*Push operation*/
    SYS_NH_MPLS_PUSH_OP_NONE,           /**< Will append new layer2 header, mpls payload could be ip , mpls or ethernet, but will keep mpls payload unchanged*/
    SYS_NH_MPLS_PUSH_OP_ROUTE,          /**< Will append new layer2 header, mpls payload is IP, and ip TTL will be updated*/
    SYS_NH_MPLS_PUSH_OP_L2VPN,          /**< Will append new layer2 header, mpls payload is ethernet, vlan tag coulde be edited*/

    /*Mpls nexthop pop operation*/
    SYS_NH_MPLS_POP_OP_NONE,            /**< Will append new layer2 header, will not edit layer2 payload, but layer2 header vlan may be untagged according to output port configuration*/
    SYS_NH_MPLS_POP_OP_ROUTE,           /**< Will append new layer2 header, layer2 payload is ip, will decreace ip TTL*/
    SYS_NH_MPLS_POP_OP_MPLS,            /**< Will append new layer2 header, layer2 payload is mpls, will update mpls TTL and exp*/
    SYS_NH_MPLS_POP_OP_L2VPN_WITH_EDIT, /**< Will NOT append new layer2 header, Will modify inner layer header's vlan tag*/
    SYS_NH_MPLS_POP_OP_L2VPN_NO_EDIT,   /**< Will NOT append new layer2 header, Will Not modify inner layer header*/

    SYS_MPLS_NH_POP_OP_MAX
};
typedef enum sys_nh_mpls_op_e sys_nh_mpls_op_t;


struct sys_rpf_info_s
{
    uint16 oif_id;
    uint8 ref;
    uint8 rev;
};
typedef struct sys_rpf_info_s sys_rpf_info_t;

struct sys_nh_param_ecmp_api_s
{
    uint32 ref_nh_id;
    sys_nh_offset_array_t fwd_offset;
    sys_rpf_info_t* p_rpf_array;
    uint8 change_type;
    uint8 oif_cnt;
    uint8 oif_changed;
    uint8 oif_need;              /* oif need return */
};
typedef struct sys_nh_param_ecmp_api_s sys_nh_param_ecmp_api_t;

struct sys_nh_mpls_label_param_s
{
    uint8 lable_flag;
#define SYS_NH_MPLS_LABEL_IS_VALID   (1 << 0)
#define SYS_NH_MPLS_LABEL_IS_MCAST   (1 << 1)
#define SYS_NH_MPLS_LABEL_MAP_TTL      (1 << 3)/**< If set, means new TTL mapped from (pkt_ttl - ttl)*/
    uint8 ttl;
    uint8 exp_type;
#define SYS_NH_MPLS_EXP_USE_LABEL_EXP               0
#define SYS_NH_MPLS_EXP_DERIVE_FROM_SRC_EXP         1
#define SYS_NH_MPLS_EXP_DERIVE_FROM_PKT_PRIORITY    2
    uint8 exp;
    uint32 label;
};
typedef struct sys_nh_mpls_label_param_s sys_nh_mpls_label_param_t;

enum sys_nh_mpls_martini_seq_type_e
{
    SYS_NH_MPLS_MARTINI_SEQ_NONE = 0,
    SYS_NH_MPLS_MARTINI_SEQ_PER_PW,
    SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE0,
    SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE1,

    CTC_NH_MPLS_MARTINI_SEQ_MAX
};
typedef enum sys_nh_mpls_martini_seq_type_e sys_nh_mpls_martini_seq_type_t;

struct sys_nh_brguc_nhid_info_s
{
    uint32 nh_type_flag;
    uint32 brguc_untagged_nhid;
    uint32 brguc_nhid;
    uint32 bypass_nhid;
    uint32 ram_pkt_nhid;
    uint32 srv_queue_nhid;
};
typedef struct sys_nh_brguc_nhid_info_s sys_nh_brguc_nhid_info_t;

struct sys_nh_mpls_param_s
{
    uint16 nh_mpls_flag;
#define SYS_NH_MPLS_FLAG_INNER_TPID_OW_EN    (1 << 0)
#define SYS_NH_MPLS_FLAG_STATS_EN            (1 << 1)
#define SYS_NH_MPLS_FLAG_USE_PKT_TTL         (1 << 2)
#define SYS_NH_MPLS_FLAG_L3IF_EN             (1 << 3)
#define SYS_NH_MPLS_FLAG_VPLS                (1 << 4)
#define SYS_MPLS_NH_FLAG_SQN_EN              (1 << 5)
#define SYS_MPLS_NH_FLAG_SERVICE_QUEUE_EN    (1 << 6)
#define SYS_MPLS_NH_FLAG_APS_EN              (1 << 7)

    uint16 total_label_num;
    sys_nh_mpls_op_t opcode;
    ctc_nh_oif_info_t oif;
    mac_addr_t mac;/*Destination MAC address*/

    sys_nh_mpls_label_param_t push_label[CTC_MPLS_NH_MAX_PUSH_LABEL_NUM];
    bool martini_encap_valid;
    sys_nh_mpls_martini_seq_type_t seq_num_type;
    uint32 seq_num_index;

    ctc_dot1q_type_t inner_dot1q_type;/*TBD*/
    bool   egs_vlan_valid;     /*Optional, used for l2vpn*/
    ctc_vlan_egress_edit_info_t vlan_info;/*Optional, used for l2vpn*/

    uint32 stats_ptr;

    bool vpls_port_valid;                         /* VPLS port is valid*/
    uint16 vpls_port;
};
typedef struct sys_nh_mpls_param_s sys_nh_mpls_param_t;

/*Common Functions*/
extern int32
sys_humber_nh_api_init(ctc_nh_global_cfg_t * nh_cfg);

extern int32
sys_humber_nh_get_dsfwd_offset(uint32 nhid,
                                       sys_nh_offset_array_t offset_array);
extern int32
sys_humber_nh_get_bypass_dsnh_offset(uint32 *p_offset);

extern int32
sys_humber_nh_get_mirror_dsnh_offset(uint32 *p_offset);

/*Ucast bridge functions*/
extern int32
sys_humber_brguc_nh_create(uint16 gport ,ctc_nh_param_brguc_sub_type_t nh_type );
extern int32
sys_humber_brguc_nh_delete(uint16 gport);

extern int32
sys_humber_brguc_get_nhid(uint16 gport, sys_nh_brguc_nhid_info_t* p_brguc_nhid_info);

extern int32
sys_humber_l2_get_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type, uint32* nhid);

extern int32
sys_humber_brguc_get_dsfwd_offset(uint16 gport, uint8 untagged,
                                  sys_nh_offset_array_t offset_array);
extern int32
sys_humber_bypass_get_dsfwd_offset(uint16 gport,
                                   sys_nh_offset_array_t offset_array);
extern int32
sys_humber_rawpkt_elog_cpu_get_dsfwd_offset(uint16 gport,
                                   sys_nh_offset_array_t offset_array);

extern int32
sys_humber_srv_queue_get_dsfwd_offset(uint16 gport,
                                   sys_nh_offset_array_t offset_array);

extern int32
sys_humber_nh_get_l3ifid(uint32 nhid, uint16 *p_l3ifid);
extern int32
sys_humber_nh_get_port(uint32 nhid, uint8 *aps_brg_en,uint16 *gport);

extern int32
sys_humber_nh_get_statsptr(uint32 nhid,  sys_nh_u16_array_t stats_ptr);

/*Egress Vlan translation*/
extern int32
sys_humber_egress_vlan_edit_nh_create(uint32 nhid, uint16 gport,
            ctc_vlan_egress_edit_info_t *p_vlan_info, uint32 dsnh_offset);
extern int32
sys_humber_egress_vlan_edit_nh_delete(uint32 nhid);


/*APS Egress Vlan translation*/
extern int32
sys_humber_aps_egress_vlan_edit_nh_create(uint32 nhid,
            uint32 dsnh_offset, uint16 aps_bridge_id,
            ctc_vlan_egress_edit_info_t *p_vlan_info_working_path,
            ctc_vlan_egress_edit_info_t *p_vlan_info_protection_path);
extern int32
sys_humber_aps_egress_vlan_edit_nh_delete(uint32 nhid);

/*Mcast bridge*/
extern int32
sys_humber_mcast_nh_create(uint32 groupid,  sys_nh_param_mcast_group_t *p_nh_mcast_group);

extern int32
sys_humber_mcast_nh_delete(uint32 nhid);

extern int32
sys_humber_mcast_nh_update(sys_nh_param_mcast_group_t *p_nh_mcast_group);


/*IPUC*/
extern int32
sys_humber_ipuc_nh_create(uint32 nhid, uint32 dsnh_offset, ctc_nh_oif_info_t *p_oif,
                          mac_addr_t mac, bool is_unrov);
extern int32
sys_humber_ipuc_nh_delete(uint32 nhid);
extern int32
sys_humber_ipuc_nh_update(uint32 nhid, ctc_nh_oif_info_t * p_oif,
                          mac_addr_t mac, sys_nh_entry_change_type_t update_type);
extern int32
sys_humber_ecmp_nh_create(sys_nh_param_ecmp_api_t* p_nh_param, uint32 *p_nhid);
extern int32
sys_humber_ecmp_nh_delete(uint32 nhid);
extern int32
sys_humber_ecmp_nh_update(uint32 nhid, sys_nh_param_ecmp_api_t* p_nh_param);

extern int32
sys_humber_mpls_nh_create(uint32 nhid, uint32 dsnh_offset,
                          bool use_nh8w, bool is_unrov,
                          sys_nh_mpls_param_t *p_nh_param,
                          sys_nh_mpls_param_t *p_nh_param_protection);
extern int32
sys_humber_mpls_nh_delete(uint32 nhid);
extern int32
sys_humber_mpls_nh_update(uint32 nhid, sys_nh_mpls_param_t *p_nh_param,
                                    sys_nh_mpls_param_t *p_nh_param_protection,
                                    sys_nh_entry_change_type_t change_type);

extern int32
sys_humber_iloop_nh_create(uint32 nhid, ctc_loopback_nexthop_param_t* p_nh_param);
extern int32
sys_humber_iloop_nh_delete(uint32 nhid);

extern int32
sys_humber_rspan_nh_create(uint32 nhid, uint32 dsnh_offset,
                                       ctc_rspan_nexthop_param_t* p_nh_param);
extern int32
sys_humber_rspan_nh_delete(uint32 nhid);

extern int32
sys_humber_downmep_nh_create( uint16 vlan_id, uint32* p_nhid);
extern int32
sys_humber_downmep_nh_delete(uint32 nhid);

extern int32
sys_humber_flex_nh_create(uint32 nhid, ctc_flex_nh_param_t* p_nh_param);
extern int32
sys_humber_flex_nh_remove(uint32 nhid);

extern int32
sys_humber_nh_get_dsnh_offset_by_nhid(uint32 nhid, uint32 *p_dsnh_offset,uint8 *p_use_dsnh8w);

extern int32
sys_humber_nh_get_dsnh_offset_by_nhid_downmep(uint32 nhid, uint8 lchip,uint32 *p_dsnh_offset);

/*#ifdef SYS_HUMBER_NH_DEBUG_ON*//*For test*/
extern int32
sys_humber_nh_test_crscnt_ref_brguc(uint16 cc_gport, uint32 ref_gport);
extern int32
sys_humber_nh_test_crscnt_ref_nhid(uint16 cc_gport, uint32 ref_nhid);
extern int32
sys_humber_nh_dump_mcast(uint32 nhid, bool detail);

extern int32
sys_humber_ecmp_group_create(ctc_nh_ecmp_creat_data_t* pdata);
extern int32
sys_humber_ecmp_group_delete(uint32 nhid);
extern int32
sys_humber_ecmp_group_update(ctc_nh_ecmp_update_data_t* pdata);

extern int32
sys_humber_nh_add_stats(uint32 nhid);

extern int32
sys_humber_nh_del_stats(uint32 nhid);

extern int32
sys_humber_nh_get_stats(uint32 nhid, ctc_stats_basic_t* p_stats);

extern int32
sys_humber_nh_reset_stats(uint32 nhid);

extern int32
sys_humber_ip_tunnel_nh_create(uint32 nhid, uint32 dsnh_offset, ctc_ip_nh_param_t* p_nh_param);

extern int32
sys_humber_ip_tunnel_nh_delete(uint32 nhid);

extern int32
sys_humber_ip_tunnel_nh_update(uint32 nhid, ctc_ip_nh_param_t* p_nh_param,
                          sys_nh_entry_change_type_t change_type);

/*#endif*/
#endif /*_SYS_HUMBER_NEXTHOP_H_*/
