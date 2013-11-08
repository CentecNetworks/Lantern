/**
 @file sys_humber_aclqos_entry.h

 @date 2009-11-30

 @version v2.0

 The file defines macro, data structure, and function for acl/qos entry component
*/

#ifndef _SYS_HUMBER_ACLQOS_ENTRY_H_
#define _SYS_HUMBER_ACLQOS_ENTRY_H_

#include "ctc_debug.h"
#include "ctc_stats.h"
#include "ctc_linklist.h"
#include "ctc_hash.h"

/*********************************************************************
  *
  * macro definition
  *
  *********************************************************************/
#define SYS_ACLQOS_ENTRY_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(aclqos, entry, ACLQOS_ENTRY_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_ACLQOS_ENTRY_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(aclqos, entry, ACLQOS_ENTRY_SYS); \
    }

#define SYS_ACL_DBG_FUNC()  \
    SYS_ACLQOS_ENTRY_DBG_FUNC()
#define SYS_ACL_DBG_INFO(FMT, ...)  \
    SYS_ACLQOS_ENTRY_DBG_INFO(FMT, ##__VA_ARGS__)

#define MAX_TCP_FLAG_ENTRY_NUM    4
#define MAX_L4_PORT_ENTRY_NUM      8

#define SYS_PAS_MAX_LAYER4_LEN_OP_INDEX  14

#define SYS_ACL_INGRESS_LOG_INDEX_BASE    8
#define SYS_QOS_INGRESS_LOG_INDEX_BASE    12
#define SYS_ACL_EGRESS_LOG_INDEX_BASE     24
#define SYS_QOS_EGRESS_LOG_INDEX_BASE     28

#define SYS_ACLQOS_MAX_SESSION_NUM 4

/*********************************************************************
  *
  * data structure definition
  *
  *********************************************************************/

/**
 @brief acl/qos action flag
*/
struct sys_aclqos_action_flag_s
{
    uint32 discard:1,
           deny_replace_cos:1,
           deny_replace_dscp:1,
           deny_bridge:1,
           deny_learning:1,
           deny_route:1,
           stats:1,
           flow_policer:1,
           trust:1,
           priority:1,
           random_log:1,
           fwd:1,
           fwd_to_cpu:1,
           flow_id:1,
           stats_mode:1,
           invalid:1,
           pbr_fwd:1,
           pbr_ttl_check:1,
           pbr_icmp_check:1,
           pbr_ecmp:1,          /*indicates if user use ecmp nexthop directly*/
           pbr_copy_to_cpu:1,
           pbr_deny:1,
           rsv:10;
 };
typedef struct sys_aclqos_action_flag_s sys_aclqos_action_flag_t;


/**
 @brief acl/qos action
*/
struct sys_aclqos_action_s
{
    sys_aclqos_action_flag_t flag;

    union
    {
        uint32 fwd_nh_id;
        uint32 fwd_reason;
    } fwd;

    union
    {
        uint16 stats_ptr;
        uint16 flow_id;
    } stats_or_flowid;
    uint16 rsv1;

    uint32 policer_id;

    uint32 priority:6,
           color:2,
           trust:3,
           acl_log_id:2,
           random_threshold_shift:4,
           ds_fwd_ptr:12,
           rsv2:15;

    uint32 pbr_fwd_ptr	:20,    /*for pbr, should be 20 bit*/
           rsv3:12;
    uint16 pbr_vrfid;
    uint8  pbr_ecpn;
    uint8  rsv4;
};
typedef struct sys_aclqos_action_s sys_aclqos_action_t;

/**
 @brief acl/qos mac key flag
*/
struct sys_aclqos_mac_key_flag_s
{
    uint32 macda:1,
           macsa:1,
           vlan_ptr:1,
           cos:1,
           cvlan:1,
           ctag_cos:1,
           ctag_cfi:1,
           svlan:1,
           stag_cos:1,
           stag_cfi:1,
           eth_type:1,
           l2_type:1,
           l3_type:1,
           acl_label:1,
           qos_label:1,
           l2_qos_label:1,
           l3_qos_label:1,
           /* If the entry is global entry, it should can also match service label */
           is_glb_entry:1,
           rsv:14;
};
typedef struct sys_aclqos_mac_key_flag_s sys_aclqos_mac_key_flag_t;


/**
 @brief acl/qos mac key
*/
struct sys_aclqos_mac_key_s
{
    sys_aclqos_mac_key_flag_t flag;

    mac_addr_t mac_da;
    mac_addr_t mac_da_mask;
    mac_addr_t mac_sa;
    mac_addr_t mac_sa_mask;
    uint32 vlan_ptr:14,
           eth_type:16,
           rsv1:2;
    uint32 cvlan:12,
           ctag_cos_cfi:4,
           svlan:12,
           stag_cos_cfi:4;
	uint32 cvlan_mask:12,
           svlan_mask:12,
           rsv3:8;
    uint32 l2_type:4,
           l3_type:4,
           cos:3,
           rsv2:21;
    uint32 acl_label:8,
           qos_label:8,
           l2_qos_label:8,
           l3_qos_label:8;

    uint32 acl_label_mask:8,
           qos_label_mask:8,
           l2_qos_label_mask:8,
           l3_qos_label_mask:8;

    uint32 eth_type_mask:16,
		   table_id0:4,
           table_id1:4,
           table_id2:4,
           table_id3:4;
};
typedef struct sys_aclqos_mac_key_s  sys_aclqos_mac_key_t;


/**
 @brief acl/qos mpls key flag
*/
struct sys_aclqos_mpls_key_flag_s
{
    uint32 macda:1,
           macsa:1,
           cos:1,
           cvlan:1,
           ctag_cos:1,
           ctag_cfi:1,
           svlan:1,
           stag_cos:1,
           stag_cfi:1,
           l2_type:1,
           label0:1,
           label1:1,
           label2:1,
           label3:1,
           routed_packet:1,
           acl_label:1,
           qos_label:1,
           l2_qos_label:1,
           l3_qos_label:1,
           rsv:13;
};
typedef struct sys_aclqos_mpls_key_flag_s sys_aclqos_mpls_key_flag_t;

/**
 @brief acl/qos mpls key
*/
struct sys_aclqos_mpls_key_s
{
    sys_aclqos_mpls_key_flag_t flag;

    mac_addr_t mac_da;
    mac_addr_t mac_da_mask;
    mac_addr_t mac_sa;
    mac_addr_t mac_sa_mask;
    uint32 cvlan:12,
           ctag_cos_cfi:4,
           svlan:12,
           stag_cos_cfi:4;
	uint32 cvlan_mask:12,
           svlan_mask:12,
           rsv3:8;
    uint32 l2_type:4,
           cos:3,
           routed_packet:1,
           rsv1:24;
    uint32 mpls_label0;
    uint32 mpls_label0_mask;
    uint32 mpls_label1;
    uint32 mpls_label1_mask;
    uint32 mpls_label2;
    uint32 mpls_label2_mask;
    uint32 mpls_label3;
    uint32 mpls_label3_mask;

    uint32 acl_label:8,
           qos_label:8,
           l2_qos_label:8,
           l3_qos_label:8;

    uint32 acl_label_mask:8,
           qos_label_mask:8,
           l2_qos_label_mask:8,
           l3_qos_label_mask:8;

    uint32 table_id0:4,
           table_id1:4,
           table_id2:4,
           table_id3:4,
           rsv2:16;
};
typedef struct sys_aclqos_mpls_key_s sys_aclqos_mpls_key_t;


/**
 @brief acl/qos ipv4 key flag
*/
struct sys_aclqos_ipv4_key_flag_s
{
    uint32 ipda:1,
           ipsa:1,
           l4info_mapped:1,
           is_application:1,
           is_tcp:1,
           is_udp:1,
           l4_src_port:1,
           l4_dst_port:1,
           tcp_flag:1,
           dscp:1,
           frag_info:1,
           ip_option:1,
           ip_hdr_error:1,
           routed_packet:1,
           macda:1,
           macsa:1,
           cos:1,
           cvlan:1,
           ctag_cos:1,
           ctag_cfi:1,
           svlan:1,
           stag_cos:1,
           stag_cfi:1,
           l2_type:1,
           l3_type:1,
           acl_label:1,
           qos_label:1,
           l2_qos_label:1,
           l3_qos_label:1,
           /* If the entry is global entry, it should can also match service label */
           is_glb_entry:1,
           rsv:2;
};
typedef struct sys_aclqos_ipv4_key_flag_s sys_aclqos_ipv4_key_flag_t;


/**
 @brief acl/qos ipv4 key
*/
struct sys_aclqos_ipv4_key_s
{
    sys_aclqos_ipv4_key_flag_t flag;

    uint32 ip_sa;
    uint32 ip_sa_mask;
    uint32 ip_da;
    uint32 ip_da_mask;
    uint16 l4_src_port;
    uint16 l4_src_port_mask;
    uint16 l4_dst_port;
    uint16 l4_dst_port_mask;
    uint16 l4info_mapped;
    uint16 l4info_mapped_mask;
    uint32 frag_info:2,
           frag_info_mask:2,
           dscp:6,
           dscp_mask:6,
           is_application:1,
           is_tcp:1,
           is_udp:1,
           routed_packet:1,
           ip_hdr_error:1,
           rsv1:11;

    mac_addr_t mac_sa;
    mac_addr_t mac_sa_mask;
    mac_addr_t mac_da;
    mac_addr_t mac_da_mask;
    uint32 cvlan:12,
           ctag_cos_cfi:4,
           svlan:12,
           stag_cos_cfi:4;
	uint32 cvlan_mask:12,
           svlan_mask:12,
           rsv4:8;
    uint32 cos:3,
           l2_type:4,
           l3_type:4,
           rsv2:21;

    uint32 acl_label:8,
           qos_label:8,
           l2_qos_label:8,
           l3_qos_label:8;

    uint32 acl_label_mask:8,
           qos_label_mask:8,
           l2_qos_label_mask:8,
           l3_qos_label_mask:8;

    uint32 table_id0:4,
           table_id1:4,
           table_id2:4,
           table_id3:4,
           rsv3:16;
};
typedef struct sys_aclqos_ipv4_key_s  sys_aclqos_ipv4_key_t;

/**
 @brief acl/qos ipv6 key flag
*/
struct sys_aclqos_ipv6_key_flag_s
{
    uint32 ipda:1,
           ipsa:1,
           l4info_mapped:1,
           is_application:1,
           is_tcp:1,
           is_udp:1,
           l4_src_port:1,
           l4_dst_port:1,
           tcp_flag:1,
           dscp:1,
           frag_info:1,
           ip_option:1,
           ip_hdr_error:1,
           routed_packet:1,
           ext_hdr:1,
           flow_label:1,
           macda:1,
           macsa:1,
           vlan_ptr:1,
           cos:1,
           cvlan:1,
           ctag_cos:1,
           svlan:1,
           stag_cos:1,
           eth_type:1,
           l2_type:1,
           l3_type:1,
           acl_label:1,
           qos_label:1,
           l2_qos_label:1,
           l3_qos_label:1,
           ctag_cfi:1;

    uint32 stag_cfi:1,
           rsv:31;
};
typedef struct sys_aclqos_ipv6_key_flag_s sys_aclqos_ipv6_key_flag_t;

/**
 @brief acl/qos ipv6 key
*/
struct sys_aclqos_ipv6_key_s
{
    sys_aclqos_ipv6_key_flag_t flag;

    ipv6_addr_t ip_sa;
    ipv6_addr_t ip_sa_mask;
    ipv6_addr_t ip_da;
    ipv6_addr_t ip_da_mask;
    uint16 l4_src_port;
    uint16 l4_src_port_mask;
    uint16 l4_dst_port;
    uint16 l4_dst_port_mask;
    uint16 l4info_mapped;
    uint16 l4info_mapped_mask;
    uint32 flow_label:20,
           ext_hdr:8,
           frag_info:2,
           is_tcp:1,
           is_udp:1;
    uint32 flow_label_mask:20,
           ext_hdr_mask:8,
           frag_info_mask:2,
           is_application:1,
           ip_option:1;
    uint32 dscp:6,
           dscp_mask:6,
           routed_packet:1,
           ip_hdr_error:1,
           eth_type:16,
           rsv1:8;

    mac_addr_t mac_sa;
    mac_addr_t mac_sa_mask;
    mac_addr_t mac_da;
    mac_addr_t mac_da_mask;
    uint32 cvlan:12,
           ctag_cos_cfi:4,
           svlan:12,
           stag_cos_cfi:4;
	uint32 cvlan_mask:12,
           svlan_mask:12,
           rsv3:8;
    uint32 cos:3,
           l2_type:4,
           l3_type:4,
           rsv2:21;

    uint32 acl_label:8,
           qos_label:8,
           l2_qos_label:8,
           l3_qos_label:8;

    uint32 acl_label_mask:8,
           qos_label_mask:8,
           l2_qos_label_mask:8,
           l3_qos_label_mask:8;

    uint32 table_id0:4,
           table_id1:4,
           table_id2:4,
           table_id3:4,
           table_id4:4,
           table_id5:4,
           table_id6:4,
           table_id7:4;
};
typedef struct sys_aclqos_ipv6_key_s sys_aclqos_ipv6_key_t;


/**
 @brief acl pbr ipv4 key flag
*/
struct sys_acl_pbr_ipv4_key_flag_s
{
    uint32 ipda:1,
           ipsa:1,
           l4info_mapped:1,
           is_tcp:1,
           is_udp:1,
           l4_src_port:1,
           l4_dst_port:1,
           tcp_flag:1,
           dscp:1,
           frag_info:1,
           ip_option:1,
           vrf_id:1,
           rsv:20;
};
typedef struct sys_acl_pbr_ipv4_key_flag_s sys_acl_pbr_ipv4_key_flag_t;


/**
 @brief acl pbr ipv4 key
*/
struct sys_acl_pbr_ipv4_key_s
{
    sys_acl_pbr_ipv4_key_flag_t flag;

    uint32 ip_sa;
    uint32 ip_sa_mask;
    uint32 ip_da;
    uint32 ip_da_mask;
    uint16 l4_src_port;
    uint16 l4_src_port_mask;
    uint16 l4_dst_port;
    uint16 l4_dst_port_mask;
    uint16 l4info_mapped;
    uint16 l4info_mapped_mask;
    uint16 vrf_id;
    uint16 vrf_id_mask;
    uint32 frag_info:2,
           frag_info_mask:2,
           dscp:6,
           dscp_mask:6,
           is_tcp:1,
           is_udp:1,
           rsv1:14;

    uint32 pbr_label:6,
           pbr_label_mask:6,
           table_id0:4,
           table_id1:4,
           rsv2:12;
};
typedef struct sys_acl_pbr_ipv4_key_s  sys_acl_pbr_ipv4_key_t;

/**
 @brief acl pbr ipv6 key flag
*/
struct sys_acl_pbr_ipv6_key_flag_s
{
    uint32 ipda:1,
           ipsa:1,
           l4info_mapped:1,
           is_tcp:1,
           is_udp:1,
           l4_src_port:1,
           l4_dst_port:1,
           tcp_flag:1,
           dscp:1,
           frag_info:1,
           ip_option:1,
           ext_hdr:1,
           vrf_id:1,
           flow_label:1,
           rsv:18;
};
typedef struct sys_acl_pbr_ipv6_key_flag_s sys_acl_pbr_ipv6_key_flag_t;

/**
 @brief acl pbr ipv6 key
*/
struct sys_acl_pbr_ipv6_key_s
{
    sys_acl_pbr_ipv6_key_flag_t flag;

    ipv6_addr_t ip_sa;
    ipv6_addr_t ip_sa_mask;
    ipv6_addr_t ip_da;
    ipv6_addr_t ip_da_mask;
    uint16 l4_src_port;
    uint16 l4_src_port_mask;
    uint16 l4_dst_port;
    uint16 l4_dst_port_mask;
    uint16 l4info_mapped;
    uint16 l4info_mapped_mask;
    uint16 vrf_id;
    uint16 vrf_id_mask;
    uint32 flow_label:20,
           ext_hdr:8,
           frag_info:2,
           is_tcp:1,
           is_udp:1;
    uint32 flow_label_mask:20,
           ext_hdr_mask:8,
           frag_info_mask:2,
           rsv1:2;
    uint32 dscp:6,
           dscp_mask:6,
           pbr_label:6,
           pbr_label_mask:6,
           rsv2:8;

    uint32 table_id0:4,
           table_id1:4,
           table_id2:4,
           table_id3:4,
           table_id4:4,
           table_id5:4,
           table_id6:4,
           table_id7:4;
};
typedef struct sys_acl_pbr_ipv6_key_s sys_acl_pbr_ipv6_key_t;

/**
 @brief  acl/qos offset info flag
*/
struct sys_aclqos_sub_entry_info_flag_s
{
    uint32 l4_src_port:1,
           l4_dst_port:1,
           frag_info:1,
           ext_hdr:1,
           reset_l4info:1,
           rsv:27;
};
typedef struct sys_aclqos_sub_entry_info_flag_s sys_aclqos_sub_entry_info_flag_t;


/**
 @brief  acl/qos sub entry information
*/
struct sys_aclqos_sub_entry_info_s
{
    sys_aclqos_sub_entry_info_flag_t flag;

    uint16 l4_src_port;
    uint16 l4_src_port_mask;
    uint16 l4_dst_port;
    uint16 l4_dst_port_mask;
    uint8  frag_info;
    uint8  frag_info_mask;
    uint8  ext_hdr;
    uint8  ext_hdr_mask;

    uint32 offset;
};
typedef struct sys_aclqos_sub_entry_info_s sys_aclqos_sub_entry_info_t;


/**
 @brief  acl/qos key
*/
struct sys_aclqos_key_s
{
    ctc_aclqos_key_type_t type;

    union
    {
        sys_aclqos_mac_key_t mac_key;
        sys_aclqos_mpls_key_t mpls_key;
        sys_aclqos_ipv4_key_t ipv4_key;
        sys_aclqos_ipv6_key_t ipv6_key;
        sys_acl_pbr_ipv4_key_t pbr_ipv4_key;
        sys_acl_pbr_ipv6_key_t pbr_ipv6_key;
    } key_info;
};
typedef struct sys_aclqos_key_s sys_aclqos_key_t;


/**
 @brief  acl/qos entry
*/
struct sys_aclqos_entry_s
{
    ctc_list_pointer_node_t head;

    uint32 entry_id;

    sys_aclqos_action_t action;
    sys_aclqos_key_t key;

    uint16 block_index;
    void *p_label; /* sys_aclqos_label_t */
};
typedef struct sys_aclqos_entry_s sys_aclqos_entry_t;
typedef struct sys_aclqos_entry_s sys_acl_entry_t;

struct sys_acl_block_s
{
    uint8                   block_number; /* physical block 0~4*/
    uint8                   block_type;   /* mac / ipv4 / ipv6 */
    uint16                  entry_count;  /* entry count on each block, 512. uint16 is enough.*/
    uint16                  entry_dft_cnt;
    uint16                  entry_dft_max;
    uint16                  free_count;   /* entry count left on each block*/
    sys_aclqos_entry_t**    entries;      /* pointer to entry*/

    uint8                   lchip;
    uint16                  after_0_cnt;  /* entry_count < SYS_ACL_REMEMBER_BASE */
    uint16                  after_1_cnt;  /* entry_count < SYS_ACL_REMEMBER_BASE */
};
typedef struct sys_acl_block_s sys_acl_block_t;

#define SYS_ACL_HASH_BLOCK_NUM  10
#define SYS_ACL_HASH_BLOCK_SIZE 256

#define SYS_ACL_ASIC_TYPE_MAC   0
#define SYS_ACL_ASIC_TYPE_IPV4  1
#define SYS_ACL_ASIC_TYPE_MAX   2

#define SYS_ACL_REMEMBER_BASE 10

/**
 @brief  acl/qos entry control structure
*/
struct sys_aclqos_entry_ctl_s
{
    uint8 entry_sort_mode;                         /**< CTC_ACLQOS_ENTRY_SORT_MODE_XXX */
    uint8 is_merge_mac_ip_key;
    uint8 is_dual_aclqos_lookup;
    uint8 disable_merge_mac_ip_key_physical;
    uint16 mac_ipv4_acl_entry_num;
    uint16 ipv6_acl_entry_num;
    uint16 mac_ipv4_qos_entry_num;
    uint16 ipv6_qos_entry_num;
    uint16 pbr_ipv4_acl_entry_num;
    uint16 pbr_ipv6_acl_entry_num;
    uint32 acl_fwd_base;
    uint32 global_aclqos_entry_head_num;
    uint32 global_aclqos_entry_tail_num;
    uint32 global_pbr_entry_head_num;
    uint32 global_pbr_entry_tail_num;


    ctc_hash_t* entry;  /* sys_acl_entry_t */

    sys_acl_block_t block[SYS_ACL_ASIC_TYPE_MAX]; /* type:0-IPV4,1-MAC*/
    uint8           asic_type[MAX_CTC_ACLQOS_KEY];  /* SYS_ACL_ASIC_TYPE_MAX */
};
typedef struct sys_aclqos_entry_ctl_s sys_aclqos_entry_ctl_t;


/*********************************************************************
  *
  * function declaration
  *
  *********************************************************************/

/**
 @brief insert acl/qos entry into entry list in the given label,
               entry id = 0 indicate inserting at front, otherwise after the entry id
*/
extern int32
sys_humber_aclqos_entry_insert(uint32 label_id, ctc_aclqos_label_type_t label_type, uint32 entry_id, ctc_aclqos_entry_t* p_ctc_entry);


/**
 @brief delete acl/qos entry from entry list in the given label
*/
extern int32
sys_humber_aclqos_entry_delete(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id);


/**
 @brief delete all acl/qos entry from entry list in the given label
*/
extern int32
sys_humber_aclqos_entry_delete_all(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type);


/**
 @brief To invalid an ACL entry, which indicates removing the entry from hardware table but still
        stay in software table.
*/
extern int32
sys_humber_aclqos_entry_set_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid);

/**
 @brief get acl/qos entry stats result
*/
extern int32
sys_humber_aclqos_entry_stats_result_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats);


/**
 @brief reset acl/qos entry stats result
*/
extern int32
sys_humber_aclqos_entry_stats_result_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id);

extern int32
sys_humber_aclqos_all_entry_stats_result_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type);

/**
 @brief set acl/qos entry action
*/
extern int32
sys_humber_aclqos_entry_action_add(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

/**
 @brief reset acl/qos entry action
*/
extern int32
sys_humber_aclqos_entry_action_delete(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);


/**
 @brief show the given entry information
*/
extern int32
sys_humber_show_aclqos_entry(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t ctc_type, uint32 entry_id);


/**
 @brief show the given entry information
*/
extern int32
sys_humber_show_all_aclqos_entry(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t ctc_type);

extern int32
sys_humber_acl_get_all_permit_entry_stats(uint32 label_id, ctc_stats_basic_t* entry_stats);

extern int32
sys_humber_acl_add_tiny_fragment_default_entry(bool deny);

extern int32
sys_humber_qos_add_tiny_fragment_default_entry(bool deny);

/**
 @brief acl/qos entry component initialization
*/
extern int32
sys_humber_aclqos_entry_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg);

extern int32
sys_humber_show_aclqos_entry_id(uint32 entry_id);

extern int32
sys_humber_show_aclqos_block(int32 asic_type);

#endif


