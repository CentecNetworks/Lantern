/**
 @file sys_humber_usrid_db.h

 @date 2009-9-29

 @version v2.0

 This file is database of the usrid.
*/

#ifndef _SYS_HUMBER_USRID_DB_H
#define _SYS_HUMBER_USRID_DB_H

/****************************************************************
*
* Header Files
*
***************************************************************/
#include "kal.h"

#include "ctc_avl_tree.h"
#include "ctc_const.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define SYS_USRID_MAX_LABEL 64
#define SYS_USRID_RESERVE_LABEL_FOR_IPSG 63
#define SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS 62


struct sys_usrid_db_ds_node_s
{
    uint32 usrid_exception_en   :1;
    uint32 by_pass_all          :1;
    uint32 aps_select_valid     :1;
    uint32 src_queue_select     :1;
    uint32 vpls_port_type       :1;
    uint32 communicate_port     :1;
    uint32 fwd_ptr_valid        :1;
    uint32 binding_en           :1;
    uint32 stats_ptr_valid      :1;
    uint32 binding_macsa        :1;
    uint32 usr_vlan_ptr         :13;
    uint32                      :9;

    uint32 binding_data_h       :16;
    uint32 binding_data_m       :16;
    uint32 binding_data_l       :16;
};
typedef struct sys_usrid_db_ds_node_s sys_usrid_db_ds_node_t;

struct sys_usrid_valid_s
{
    uint32 src_port_valid       :1;
    uint32 igs_svid_valid       :1;
    uint32 igs_cvid_valid       :1;
    uint32 stag_cos_valid       :1;
    uint32 stag_cfi_valid       :1;
    uint32 ctag_cos_valid       :1;
    uint32 ctag_cfi_valid       :1;
    uint32 exception2_valid     :1;

    uint32 exp_subindex_valid   :1;
    uint32 vc_valid             :1;
    uint32 isid_valid           :1;
    uint32 gre_valid            :1;
    uint32 l2_type_valid        :1;
    uint32 l3_type_valid        :1;
    uint32 ether_type_valid     :1;
    uint32 routed_packet_valid  :1;

    uint32 ip_hdr_error_valid   :1;
    uint32 is_tcp_valid         :1;
    uint32 is_udp_valid         :1;
    uint32 is_app_valid         :1;
    uint32 ip_option_valid      :1;
    uint32 frag_info_valid      :1;
    uint32 dscp_valid           :1;
    uint32 l4info_mapped_valid  :1;

    uint32 l4_dest_port_valid   :1;
    uint32 l4_src_port_valid    :1;
    uint32 sgmac_valid          :1;
    uint32 ipv6_flow_label_valid    :1;
    uint32 ipv6_ext_hdr_valid       :1;
    uint32                          :3;
};
typedef struct sys_usrid_valid_s sys_usrid_valid_t;

/**
 @brief usrid vlan node saved in AVL Vlan DataBase
*/
struct sys_usrid_db_vlankey_node_s
{
    uint32 igs_cvid             :12;
    uint32 igs_svid             :12;
    uint32 ctag_cos             :3;
    uint32 stag_cos             :3;
    uint32 ctag_cfi             :1;
    uint32 stag_cfi             :1;

    uint32 igs_cvid_mask        :12;
    uint32 igs_svid_mask        :12;
    uint32 from_sgmac           :1;
    uint32 exception2           :1;
    uint32 label_id             :6;

    uint32 customer_id          :32;    /**<maybe I-SID, VC label, GRE key, UDP port*/

    uint32 global_src_port      :13;
    uint32 exp_subindex         :4;
    uint32                      :7;

};
typedef struct sys_usrid_db_vlankey_node_s sys_usrid_db_vlankey_node_t;

struct sys_usrid_db_vlan_node_s
{
    /*valid info*/
    sys_usrid_valid_t valid;

    /*node info*/
    uint32 key_offset           :32;
    /*key*/
    sys_usrid_db_vlankey_node_t usrid_key_node;

    /*ds*/
    sys_usrid_db_ds_node_t ds_node_usrid;
};
typedef struct sys_usrid_db_vlan_node_s sys_usrid_db_vlan_node_t;

/**
 @brief usrid mac node saved in AVL Mac DataBase
*/
struct sys_usrid_db_mackey_node_s
{
    /*key*/
    uint32 svid             :12;
    uint32 stag_cos         :3;
    uint32 stag_cfi         :1;
    uint32 cvid             :12;
    uint32 ctag_cos         :3;
    uint32 ctag_cfi         :1;

    uint32 l2_type          :4;
    uint32 l3_type          :4;
    uint32 from_sgmac       :1;
    uint32 exception2       :1;
    uint32 exp_sub_index    :4;
    uint32 chip_id          :5;
    uint32 label_id         :6;
    uint32                  :7;

    uint32 macda_h          :16;
    uint32 macsa_h          :16;
    uint32 macda_l          :32;
    uint32 macsa_l          :32;

    uint32 macda_mask_h     :16;
    uint32 macsa_mask_h     :16;
    uint32 macda_mask_l     :32;
    uint32 macsa_mask_l     :32;
};
typedef struct sys_usrid_db_mackey_node_s sys_usrid_db_mackey_node_t;

struct sys_usrid_db_mac_node_s
{
    /*valid info*/
    sys_usrid_valid_t valid;

    /*node info*/
    uint32 key_offset       :32;

    /*key*/
    sys_usrid_db_mackey_node_t usrid_key_node;

    /*ds*/
    sys_usrid_db_ds_node_t ds_node_usrid;

};
typedef struct sys_usrid_db_mac_node_s sys_usrid_db_mac_node_t;

/**
 @brief usrid ipv4 node saved in AVL IPv4 DataBase
*/
struct sys_usrid_db_ipv4key_node_s
{
    uint32 svid             :12;
    uint32 stag_cos         :3;
    uint32 stag_cfi         :1;
    uint32 cvid             :12;
    uint32 ctag_cos         :3;
    uint32 ctag_cfi         :1;

	uint32 igs_cvid_mask    :12;
    uint32 igs_svid_mask    :12;
	uint32                  :8;

    uint32 exception2_en    :1;
    uint32 exp_sub_index    :4;
    uint32 routed_packet    :1;
    uint32 ip_hdr_error     :1;
    uint32 is_tcp           :1;
    uint32 is_udp           :1;
    uint32 is_app           :1;
    uint32 ip_option        :1;
    uint32 frag_info        :2;
    uint32 from_sgmac       :1;
    uint32 chip_id          :5;
    uint32 label_id         :6;
    uint32                  :7;

    uint32 dscp             :6;
    uint32 l2_type          :4;
    uint32 l3_type          :4;
    uint32                  :18;

    uint32 ether_type       :16;
    uint32 l4_info_mapped   :16;
    uint32 l4_dest_port     :16;
    uint32 l4_src_port      :16;
    uint32 macsa_h          :16;
    uint32 macda_h          :16;
    uint32 macsa_l          :32;
    uint32 macda_l          :32;
    uint32 macda_mask_h     :16;
    uint32 macsa_mask_h     :16;
    uint32 macda_mask_l     :32;
    uint32 macsa_mask_l     :32;
    uint32 ipv4_sa          :32;
    uint32 ipv4_sa_mask     :32;
    uint32 ipv4_da          :32;
    uint32 ipv4_da_mask     :32;
};
typedef struct sys_usrid_db_ipv4key_node_s sys_usrid_db_ipv4key_node_t;

struct sys_usrid_db_ipv4_node_s
{
    /*valid info*/
    sys_usrid_valid_t valid;

    /*node info*/
    uint32 key_offset       :32;

    /*key*/
    sys_usrid_db_ipv4key_node_t usrid_key_node;

    /*ds*/
    sys_usrid_db_ds_node_t ds_node_usrid;

};
typedef struct sys_usrid_db_ipv4_node_s sys_usrid_db_ipv4_node_t;

/**
 @brief usrid ipv6 node saved in AVL IPv6 DataBase
*/
struct sys_usrid_db_ipv6key_node_s
{
    uint32 ipv6_sa[4];          /**< ipv6_sa[3] is the lowest ipv6 address*/
    uint32 ipv6_smask[4];
    uint32 ipv6_da[4];
    uint32 ipv6_dmask[4];

    uint32 cvid         :12;
    uint32 ctag_cos     :3;
    uint32 ctag_cfi     :1;
    uint32 svid         :12;
    uint32 stag_cos     :3;
    uint32 stag_cfi     :1;

    uint32 macda_h      :16;
    uint32 macsa_h      :16;
    uint32 macda_l      :32;
    uint32 macsa_l      :32;
    uint32 macda_mask_h     :16;
    uint32 macsa_mask_h     :16;
    uint32 macda_mask_l     :32;
    uint32 macsa_mask_l     :32;
    uint32 l4_src_port  :16;
    uint32 l4_dest_port :16;
    uint32 ether_type   :16;
    uint32 l4_info_map  :16;

    uint32 ipv6_flow_label  :20;
    uint32 ipv6_ext_hdr :8;
    uint32 l2_type      :4;

    uint32 l3_type      :4;
    uint32 dscp         :6;
    uint32 routed_packet    :1;
    uint32 ip_option    :1;
    uint32 is_tcp       :1;
    uint32 is_udp       :1;
    uint32 is_app       :1;
    uint32 from_sgmac   :1;
    uint32 ip_hdr_err   :1;
    uint32 frag_info    :2;
    uint32 exception2   :1;
    uint32 exp_subindex :4;
    uint32 label_id     :6;
    uint32              :2;

    uint32 chip_id      :5;
    uint32              :27;

};
typedef struct sys_usrid_db_ipv6key_node_s sys_usrid_db_ipv6key_node_t;


struct sys_usrid_db_ipv6_node_s
{
    sys_usrid_valid_t valid;

    /*node info*/
    uint32 key_offset   :32;

    /*key*/
    sys_usrid_db_ipv6key_node_t usrid_key_node;
    /*ds*/
    sys_usrid_db_ds_node_t ds_node_usrid;

};
typedef struct sys_usrid_db_ipv6_node_s sys_usrid_db_ipv6_node_t;


struct sys_usrid_db_s
{
    ctc_avl_tree_t **vlan_tree;
    ctc_avl_tree_t **mac_tree;
    ctc_avl_tree_t **ipv4_tree;
    ctc_avl_tree_t **ipv6_tree;
    uint16 vlankey_default_offset[MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM + 1];
    uint16 mackey_default_offset[SYS_USRID_MAX_LABEL];
    uint16 ipv4key_default_offset[SYS_USRID_MAX_LABEL];
    uint16 ipv6key_default_offset[SYS_USRID_MAX_LABEL];
};
typedef struct sys_usrid_db_s sys_usrid_db_t;

#define SYS_USRID_DBG_INFO(FMT, ...)                          \
                {                                                      \
                   CTC_DEBUG_OUT_INFO(sacl, sacl, SACL_SYS, FMT,##__VA_ARGS__);\
                }


#define SYS_USRID_DBG_FUNC()                          \
                    {\
                    CTC_DEBUG_OUT_FUNC(sacl, sacl, SACL_SYS);\
                    }


/****************************************************************************
*
* Function
*
*****************************************************************************/

/*init*/
extern int32
sys_humber_usrid_db_init(void);

extern int32
sys_humber_usrid_db_vlan_init(void);

extern int32
sys_humber_usrid_db_mac_init(void);

extern int32
sys_humber_usrid_db_ipv4_init(void);

extern int32
sys_humber_usrid_db_ipv6_init(void);

/*offset in tcam*/
extern int32
sys_humber_usrid_db_alloc_offset(uint8 chip_id, uint32 key_type, uint32 *p_key_offset);

extern int32
sys_humber_usrid_db_free_offset(uint8 chip_id, uint32 key_type, uint32 key_offset);

extern int32
sys_humber_usrid_db_alloc_resv_offset(uint8 chip_id, uint16 label_or_gport, uint32 key_type, uint32 *p_key_offset);

extern int32
sys_humber_usrid_db_free_resv_offset(uint8 chip_id, uint16 label_or_gport, uint32 key_type, uint32 *p_key_offset);

extern int32
sys_humber_usrid_db_get_resv_offset(uint8 chip_id, uint16 gport_or_label, uint32 key_type, uint32 *p_key_offset);

extern int32
sys_humber_usrid_db_insert_node(uint8 chip_id, uint16 label_or_gport, uint32 key_type, void *p_node);

extern void *
sys_humber_usrid_db_delete_node(uint8 chip_id, uint16 label_or_gport, uint32 key_type, void *p_node);

extern void *
sys_humber_usrid_db_find_node(uint8 lchip, uint16 label_or_gport, uint32 key_type, void *p_node);

extern int32
sys_humber_usrid_db_remove_node_by_label_port(uint8 lchip, uint16 index, uint32 key_type, uint32 *key_offset);

extern void
sys_humber_usrid_db_show_vlan_by_port(uint8 chip_id, uint16 port);

extern void
sys_humber_usrid_db_show_mac_by_label(uint8 chip_id, uint8 label_id);

extern void
sys_humber_usrid_db_show_ipv4_by_label(uint8 chip_id, uint8 label_id);

extern void
sys_humber_usrid_db_show_ipv6_by_label(uint8 chip_id, uint8 label_id);

extern void
sys_humber_usrid_db_show_vlan_count_by_port(uint8 chip_id, uint16 gport);

extern void
sys_humber_usrid_db_show_mac_count_by_label(uint8 chip_id, uint8 label_id);

extern void
sys_humber_usrid_db_show_ipv4_count_by_label(uint8 chip_id, uint8 label_id);

extern void
sys_humber_usrid_db_show_ipv6_count_by_label(uint8 chip_id, uint8 label_id);

#endif
