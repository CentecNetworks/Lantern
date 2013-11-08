/**
 @file sys_humber_usrid.h

 @date 2009-9-25

 @version v2.0

 This file is to define the API and the data structure used in usrid
*/

#ifndef _SYS_HUMBER_USRID_H
#define _SYS_HUMBER_USRID_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_debug.h"
#include "sys_humber_usrid_db.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define SYS_USRID_LABEL_CHECK(label_id)\
    if (label_id >= SYS_USRID_MAX_LABEL) \
    {\
        return CTC_E_INVALID_USRID_LABEL;\
    }

#define USRID_VLAN_LOCK \
    if (p_mutex->usrid_vlan_mutex) kal_mutex_lock(p_mutex->usrid_vlan_mutex)

#define USRID_VLAN_UNLOCK \
    if (p_mutex->usrid_vlan_mutex) kal_mutex_unlock(p_mutex->usrid_vlan_mutex)

#define USRID_MAC_LOCK \
    if (p_mutex->usrid_mac_mutex) kal_mutex_lock(p_mutex->usrid_mac_mutex)

#define USRID_MAC_UNLOCK \
    if (p_mutex->usrid_mac_mutex) kal_mutex_unlock(p_mutex->usrid_mac_mutex)

#define USRID_IPV4_LOCK \
    if (p_mutex->usrid_ipv4_mutex) kal_mutex_lock(p_mutex->usrid_ipv4_mutex)

#define USRID_IPV4_UNLOCK \
    if (p_mutex->usrid_ipv4_mutex) kal_mutex_unlock(p_mutex->usrid_ipv4_mutex)

#define USRID_IPV6_LOCK \
    if (p_mutex->usrid_ipv6_mutex) kal_mutex_lock(p_mutex->usrid_ipv6_mutex)

#define USRID_IPV6_UNLOCK \
    if (p_mutex->usrid_ipv6_mutex) kal_mutex_unlock(p_mutex->usrid_ipv6_mutex)

struct binding_data_h_s
{
    uint32 binding_data                 :16;
    uint32 srv_aclqos_en                :1;
    uint32 vpls_src_port_valid          :1;
    uint32 vpls_src_port                :13;
    uint32 aps_select_protecting_path   :1;
    uint32 aps_select_group_valid       :1;
    uint32 usr_svlan_valid              :1;

    uint32 aps_select_group_id          :12;
    uint32 usr_svlan_id                 :12;
    uint32                              :8;
};
typedef struct binding_data_h_s binding_data_h_t;

struct binding_data_m_s
{
    uint32 binding_data         :16;
    uint32 svr_id_en            :1;
    uint32 svr_policer_valid    :1;
    uint32 svr_id               :14;

    uint32 usr_cvlan_valid      :1;
    uint32 usr_cvlan_id         :12;
    uint32 flow_policer_valid   :1;
    uint32 flow_policer_ptr     :14;
    uint32                      :4;
};
typedef struct binding_data_m_s binding_data_m_t;

union binding_data_l_u
{
    uint32 binding_data :16;
    uint32 vrf_id       :16;
    uint32 stats_ptr    :16;
    uint32 fwd_ptr      :16;
};
typedef union binding_data_l_u binding_data_l_t;


struct sys_usrid_ds_entry_s
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
    uint32 vrfid_valid          :1;
    uint32 binding_macsa        :1;
    uint32 usr_vlan_ptr         :13;
    uint32                      :8;

    binding_data_h_t binding_data_h;
    binding_data_m_t binding_data_m;
    binding_data_l_t binding_data_l;
};
typedef struct sys_usrid_ds_entry_s sys_usrid_ds_entry_t;

/**
 @brief  definition vlan entry that sys layer 's API received
*/
struct sys_usrid_vlankey_entry_s
{
    uint32 ingress_cvid     :12;
    uint32 ctag_cos         :3;
    uint32 ctag_cfi         :1;
    uint32 ingress_svid     :12;
    uint32 stag_cos         :3;
    uint32 stag_cfi         :1;

    uint32 igs_cvid_mask    :12;
    uint32 igs_svid_mask    :12;
    uint32 label_id         :6;
    uint32 from_sgmac       :1;
    uint32 exception2       :1;

    uint32 customer_id      :32;

    uint32 global_port      :13;
    uint32 exp_subindex     :4;
    uint32 lchip            :5;
    uint32                  :2;

};
typedef struct sys_usrid_vlankey_entry_s sys_usrid_vlankey_entry_t;

struct sys_usrid_vlan_entry_s
{
    /*key valid info*/
    sys_usrid_valid_t valid;

    /*key*/
    sys_usrid_vlankey_entry_t usrid_key_entry;

    /*ds*/
    sys_usrid_ds_entry_t ds_entry_usrid;

};
typedef struct sys_usrid_vlan_entry_s sys_usrid_vlan_entry_t;

typedef struct sys_humber_vlan_link_s
{
    uint16                   usVlanStart;
    uint16                   usVlanEnd;
    uint16                   usMaskBits;
    uint16                   usValue;
    uint16                   usMask;
    struct  sys_humber_vlan_link_s* pstNodeNext;
}sys_humber_vlan_link_t;

/**
 @brief definition mac entry that sys layer 's API received
*/
struct sys_usrid_mackey_entry_s
{
    uint32 chip_id          :5;
    uint32 exception2       :1;
    uint32 exp_subindex     :4;
    uint32 l2_type          :4;
    uint32 l3_type          :4;
    uint32 usrid_label      :6;
    uint32 from_sgmac       :1;
    uint32                  :7;

    uint32 igs_svid         :12;
    uint32 stag_cos         :3;
    uint32 stag_cfi         :1;
    uint32 igs_cvid         :12;
    uint32 ctag_cos         :3;
    uint32 ctag_cfi         :1;

    uint32 macsa_h          :16;
    uint32 macda_h          :16;
    uint32 macsa_l          :32;
    uint32 macda_l          :32;

    uint32 macsa_mask_h     :16;
    uint32 macda_mask_h     :16;
    uint32 macsa_mask_l     :32;
    uint32 macda_mask_l     :32;

};
typedef struct sys_usrid_mackey_entry_s sys_usrid_mackey_entry_t;

struct sys_usrid_mac_entry_s
{
    /*key valid info*/
    sys_usrid_valid_t valid;

    /*key*/
    sys_usrid_mackey_entry_t usrid_key_entry;

    /*ds*/
    sys_usrid_ds_entry_t ds_entry_usrid;

};
typedef struct sys_usrid_mac_entry_s sys_usrid_mac_entry_t;

/**
 @brief definition vipv6 entry that sys layer 's API received
*/

struct sys_usrid_ipv4key_entry_s
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
    uint32                  :18;

    uint32 dscp             :6;
    uint32 l2_type          :4;
    uint32 l3_type          :4;
    uint32 chip_id          :5;
    uint32 usrid_label      :6;
    uint32                  :7;

    uint32 ether_type       :16;
    uint32 l4_info_mapped   :16;
    uint32 l4_dest_port     :16;
    uint32 l4_src_port      :16;
    uint32 macsa_h          :16;
    uint32 macda_h          :16;
    uint32 macsa_l          :32;
    uint32 macda_l          :32;
    uint32 macsa_mask_h     :16;
    uint32 macda_mask_h     :16;
    uint32 macsa_mask_l     :32;
    uint32 macda_mask_l     :32;
    uint32 ipv4_sa          :32;
    uint32 ipv4_sa_mask     :32;
    uint32 ipv4_da          :32;
    uint32 ipv4_da_mask     :32;

};
typedef struct sys_usrid_ipv4key_entry_s sys_usrid_ipv4key_entry_t;

struct sys_usrid_ipv4_entry_s
{
    /*key valid info*/
    sys_usrid_valid_t valid;

    /*key*/
    sys_usrid_ipv4key_entry_t usrid_key_entry;

    /*ds*/
    sys_usrid_ds_entry_t ds_entry_usrid;
};
typedef struct sys_usrid_ipv4_entry_s sys_usrid_ipv4_entry_t;

/**
 @brief definition ipv6 entry that sys layer 's API received
*/
struct sys_usrid_ipv6key_entry_s
{
    uint32 chip_id          :5;
    uint32                  :27;

    uint32 ipv6_sa[4];              /**< ipv6_sa[3] is the lowest ipv6 address*/
    uint32 ipv6_da[4];              /**< ipv6_da[3] is the lowest ipv6 address*/
    uint32 ipv6_smask[4];
    uint32 ipv6_dmask[4];
    uint32 cvid             :12;
    uint32 ctag_cos         :3;
    uint32 ctag_cfi         :1;
    uint32 svid             :12;
    uint32 stag_cos         :3;
    uint32 stag_cfi         :1;

    uint32 macda_h          :16;
    uint32 macsa_h          :16;
    uint32 macda_l          :32;
    uint32 macsa_l          :32;
    uint32 macsa_mask_h     :16;
    uint32 macda_mask_h     :16;
    uint32 macsa_mask_l     :32;
    uint32 macda_mask_l     :32;
    uint32 l4_src_port      :16;
    uint32 l4_dest_port     :16;
    uint32 ether_type       :16;
    uint32 l4_info_map      :16;

    uint32 ipv6_flow_label  :20;
    uint32 ipv6_ext_hdr     :8;
    uint32 l2_type          :4;

    uint32 l3_type          :4;
    uint32 usrid_label      :6;
    uint32 dscp             :6;
    uint32 routed_packet    :1;
    uint32 is_tcp           :1;
    uint32 is_udp           :1;
    uint32 is_app           :1;
    uint32 from_sgmac       :1;
    uint32 ip_hdr_err       :1;
    uint32 frag_info        :2;
    uint32 exception2       :1;
    uint32 exp_subindex     :4;
    uint32 ip_option        :1;
    uint32                  :2;
};
typedef struct sys_usrid_ipv6key_entry_s sys_usrid_ipv6key_entry_t;

struct sys_usrid_ipv6_entry_s
{
    /*key valid info*/
    sys_usrid_valid_t valid;

    /*key*/
    sys_usrid_ipv6key_entry_t usrid_key_entry;

    /*ds*/
    sys_usrid_ds_entry_t ds_entry_usrid;
};
typedef struct sys_usrid_ipv6_entry_s sys_usrid_ipv6_entry_t;

enum sys_usrid_key_type_e
{
    SYS_USRID_VLAN_KEY = 0,
    SYS_USRID_MAC_KEY = 1,
    SYS_USRID_IPV4_KEY = 2,
    SYS_USRID_IPV6_KEY = 3
};
typedef enum sys_usrid_key_type_e sys_usrid_key_type_t;

struct sys_usrid_mutex_s
{
    kal_mutex_t *usrid_vlan_mutex;
    kal_mutex_t *usrid_mac_mutex;
    kal_mutex_t *usrid_ipv4_mutex;
    kal_mutex_t *usrid_ipv6_mutex;
};
typedef struct sys_usrid_mutex_s sys_usrid_mutex_t;

/****************************************************************************
 *
* Function
*
*****************************************************************************/
extern int32
sys_humber_usrid_init(void);

extern int32
sys_humber_usrid_add_mac_entry(sys_usrid_mac_entry_t *p_entry);

extern int32
sys_humber_usrid_delete_mac_entry(sys_usrid_mac_entry_t *p_entry);

extern int32
sys_humber_usrid_add_vlan_entry(sys_usrid_vlan_entry_t *p_entry);

extern int32
sys_humber_usrid_delete_vlan_entry(sys_usrid_vlan_entry_t *p_entry);

extern int32
sys_humber_usrid_add_ipv4_entry(sys_usrid_ipv4_entry_t *p_entry);

extern int32
sys_humber_usrid_delete_ipv4_entry(sys_usrid_ipv4_entry_t *p_entry);

extern int32
sys_humber_usrid_add_ipv6_entry(sys_usrid_ipv6_entry_t *p_entry);

extern int32
sys_humber_usrid_delete_ipv6_entry(sys_usrid_ipv6_entry_t *p_entry);

extern int32
sys_humber_usrid_add_vlan_default_entry_per_port(uint16 gport, sys_usrid_ds_entry_t *usrid_def_entry);

extern int32
sys_humber_usrid_delete_vlan_default_entry_per_port(uint16 gport);

extern int32
sys_humber_usrid_add_mac_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry);

extern int32
sys_humber_usrid_delete_mac_default_entry_per_label(uint8 lchip, uint8 label_id);

extern int32
sys_humber_usrid_add_ipv4_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry);

extern int32
sys_humber_usrid_delete_ipv4_default_entry_per_label(uint8 lchip, uint8 label_id);

extern int32
sys_humber_usrid_add_ipv6_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry);

extern int32
sys_humber_usrid_delete_ipv6_default_entry_per_label(uint8 lchip, uint8 label_id);

extern int32
sys_humber_usrid_remove_vlan_all_by_port(uint16 gport);

extern int32
sys_humber_usrid_remove_mac_all_by_label(uint8 label_id);

extern int32
sys_humber_usrid_remove_ipv4_all_by_label(uint8 label_id);

extern int32
sys_humber_usrid_remove_ipv6_all_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_vlan_by_port(uint16 gport);

extern int32
sys_humber_usrid_show_mac_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_ipv4_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_ipv6_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_vlan_count_by_port(uint16 gport);

extern int32
sys_humber_usrid_show_mac_count_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_ipv4_count_by_label(uint8 label_id);

extern int32
sys_humber_usrid_show_ipv6_count_by_label(uint8 label_id);

extern int32
sys_humber_vlan_range_split_vlan(uint16 vlan_start, uint16 vlan_end, sys_humber_vlan_link_t** head, uint32 *sum);

extern void
sys_humber_vlan_range_free_node(sys_humber_vlan_link_t *p_head);

#endif

