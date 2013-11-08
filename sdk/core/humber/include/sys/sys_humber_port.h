/**
 @file sys_humber_port.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-16

 @version v2.0

*/

#ifndef _SYS_HUMBER_PORT_H
#define _SYS_HUMBER_PORT_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

#include "ctc_const.h"
#include "ctc_vlan.h"
#include "ctc_port.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define SYS_MAX_FRAME_VALUE (16 * 1024)
#define SYS_MAX_PREAMBLE_FOR_GMAC 31
#define SYS_MIN_PREAMBLE_FOR_GMAC 2
#define SYS_MAX_PREAMBLE_FOR_SGMAC_XGMAC 8
#define SYS_MIN_PREAMBLE_FOR_SGMAC_XGMAC 4
#define SYS_MIN_LENGTH_FOR_GMAC 33
#define SYS_MAX_LENGTH_FOR_GMAC 64
#define SYS_MAX_GMAC_NUM 48

#define GMAC_FLOW_CTL_REG_ID_INTERVAL 20
#define SGMAC_FLOW_CTL_REG_ID_INTERVAL 42
#define XGMAC_FLOW_CTL_REG_ID_INTERVAL 36


#define PORT_LOCK \
    if (p_port_master->p_port_mutex) kal_mutex_lock(p_port_master->p_port_mutex)
#define PORT_UNLOCK \
    if (p_port_master->p_port_mutex) kal_mutex_unlock(p_port_master->p_port_mutex)

#define PORT_DB_SET(old, cur)\
    {\
        if (cur == old)\
        {\
            PORT_UNLOCK;\
            return CTC_E_NONE;\
        }\
        else\
        {\
            old = cur;\
        }\
    }

#define PORT_DB_GET(cur, old) \
    { \
        *cur = old;\
    }

#define SYS_PORT_DEBUG_INFO(FMT, ...) \
            { \
                CTC_DEBUG_OUT_INFO(port, port, PORT_SYS, FMT, ##__VA_ARGS__);\
            }

#define SYS_PORT_DEBUG_FUNC() \
            {\
                CTC_DEBUG_OUT_FUNC(port, port, PORT_SYS);\
            }
enum sys_port_usrid_type_e
{
    SYS_PORT_USRID_MAC,
    SYS_PORT_USRID_IP,
    SYS_PORT_USRID_VLAN,
    SYS_PORT_USRID_MAX
};
typedef enum sys_port_usrid_type_e sys_port_usrid_type_t;

enum sys_port_feature_e
{
    SYS_PORT_VLAN_MAC       = (1 << 0),
    SYS_PORT_VLAN_IPV4      = (1 << 1),
    SYS_PORT_VLAN_IPV6      = (1 << 2),
    SYS_PORT_VLAN_MAPPING   = (1 << 3),
    SYS_PORT_VLAN_SWITCH    = (1 << 4),
    SYS_PORT_SUB_IF         = (1 << 5),
    SYS_PORT_PHY_IF         = (1 << 6),
    SYS_PORT_IPSRC_GUARD    = (1 << 7)
};
typedef enum sys_port_feature_e sys_port_feature_t;

struct sys_igs_port_prop_s
{
    uint16 global_src_port;
    uint16 default_vlan;

    uint16 vpls_src_port;
    uint16 vlanctl_mode;

    uint8 learning_enable;
    uint8 vpls_port_type;
    uint8 receive_en;
    uint8 bridge_en;

    uint8 route_enable;
    uint8 routed_port;
    uint8 outer_is_cvlan;
    uint8 src_outer_is_svlan;

    uint8 stag_tpid_index;
    uint8 keep_vlan_tag;
    uint8 protocol_vlan_en;
    uint8 srcdiscard_en;

    uint16 l3if_id;
    uint16 flag;

    uint8 port_mac_type;
    uint8 speed_mode;
    uint8 serdes_mode;
    uint8 port_mac_en;
    uint8 stretch_en;

    uint8 ptp_en;
    uint8 lbk_en;
    uint8 pause_rx_en;
    uint8 inter_lport;
};
typedef struct sys_igs_port_prop_s sys_igs_port_prop_t;

struct sys_egs_port_prop_s
{
    uint16 global_dest_port;
    uint16 default_vlan;

    uint8 transmit_en;
    uint8 bridge_en;
    uint8 routed_port;
    uint8 dot1q_type;

    uint8 vpls_port_type;
    uint8 pading_en;
    uint8 tx_threshold;
    uint8 pause_tx_en;
};
typedef struct sys_egs_port_prop_s sys_egs_port_prop_t;

struct sys_port_master_s
{
    sys_igs_port_prop_t **igs_port_prop;
    sys_egs_port_prop_t **egs_port_prop;
    kal_mutex_t *p_port_mutex;
};
typedef struct sys_port_master_s sys_port_master_t;

/****************************************************************************
 *
* Function
*
*****************************************************************************/
extern int32
sys_humber_port_init(void);

extern int32
sys_humber_port_set_global_port(uint8 chip_id, uint8 lport, uint16 gl_port);
extern int32
sys_humber_port_get_global_port(uint8 chip_id, uint8 lport, uint16 *gl_port);

extern int32
sys_humber_port_set_transmit_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_transmit_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_receive_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_receive_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_bridge_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_bridge_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_bridge(uint16 gport, ctc_direction_t dir, bool enable);

extern int32
sys_humber_port_set_allow_mcast_mac_sa(uint16 gport, bool enable);

extern int32
_sys_humber_port_set_routed_port(uint16 gport, bool is_routed);
extern int32
sys_humber_port_set_phy_if_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_phy_if_en(uint16 gport, uint16 *l3if_id, bool *enable);

extern int32
sys_humber_port_set_sub_if_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_sub_if_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_default_vlan(uint16 gport, uint16 vid);
extern int32
sys_humber_port_get_default_vlan(uint16 gport, uint16 *vid);

extern int32
sys_humber_port_set_default_pcp(uint16 gport, uint8 pcp);
extern int32
sys_humber_port_get_default_pcp(uint16 gport, uint8 *pcp);

extern int32
sys_humber_port_set_default_dei(uint16 gport, uint8 dei);
extern int32
sys_humber_port_get_default_dei(uint16 gport, uint8 *dei);

extern int32
sys_humber_port_set_vlanctl(uint16 gport, ctc_vlantag_ctl_t mode);
extern int32
sys_humber_port_get_vlanctl(uint16 gport, ctc_vlantag_ctl_t *mode);

extern int32
sys_humber_port_set_outer_is_cvlan(uint16 gport, bool is_cvlan);
extern int32
sys_humber_port_get_outer_is_cvlan(uint16 gport, bool *is_cvlan);

extern int32
sys_humber_port_set_src_outer_is_svlan(uint16 gport, bool is_svlan);
extern int32
sys_humber_port_get_src_outer_is_svlan(uint16 gport, bool *is_svlan);

extern int32
sys_humber_port_set_use_inner_cos(uint16 gport, bool is_inner);
extern int32
sys_humber_port_get_use_inner_cos(uint16 gport, bool* is_inner);

extern int32
sys_humber_port_set_stag_tpid_index(uint16 gport, ctc_direction_t dir,uint8 index);
extern int32
sys_humber_port_get_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 *index);

extern int32
sys_humber_port_set_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool enable);
extern int32
sys_humber_port_get_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool *enable);

extern int32
sys_humber_port_set_cross_connect(uint16 gport, bool enable);
extern int32
sys_humber_port_get_cross_connect(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_learning_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_learning_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_keep_vlan_tag(uint16 gport, bool enable);
extern int32
sys_humber_port_get_keep_vlan_tag(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_dot1q_type(uint16 gport, ctc_dot1q_type_t type);
extern int32
sys_humber_port_get_dot1q_type(uint16 gport, ctc_dot1q_type_t *type);

extern int32
sys_humber_port_set_use_outer_ttl(uint16 gport, bool enable);
extern int32
sys_humber_port_get_use_outer_ttl(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_untag_dft_vid(uint16 gport, bool enable, bool untag_svlan);
extern int32
sys_humber_port_get_untag_dft_vid(uint16 gport, bool *enable, bool *untag_svlan);

extern int32
sys_humber_port_set_exception_en(uint16 gport, uint16 bitmap);
extern int32
sys_humber_port_get_exception_en(uint16 gport, uint16 *bitmap);

extern int32
sys_humber_port_set_exception_discard(uint16 gport, uint16 bitmap);
extern int32
sys_humber_port_get_exception_discard(uint16 gport, uint16 *bitmap);

extern int32
sys_humber_port_set_security_excp_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_security_excp_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_security_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_security_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_mac_security_discard(uint16 gport, bool discard);
extern int32
sys_humber_port_get_mac_security_discard(uint16 gport, bool *discard);

extern int32
sys_humber_port_set_ipsg_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_ipsg_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_isolation_id(uint16 gport, ctc_direction_t dir, uint8 isolation_id);
extern int32
sys_humber_port_get_isolation_id(uint16 gport, ctc_direction_t dir, uint8 *isolation_id);

extern int32
sys_humber_port_set_ucast_flooding_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_ucast_flooding_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_mcast_flooding_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_mcast_flooding_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_reflective_bridge_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_reflective_bridge_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_random_log_en(uint16 gport, ctc_direction_t dir, bool enable);
extern int32
sys_humber_port_get_random_log_en(uint16 gport, ctc_direction_t dir, bool *enable);

extern int32
sys_humber_port_set_random_threshold(uint16 gport, ctc_direction_t dir, uint16 threshold);
extern int32
sys_humber_port_get_random_threshold(uint16 gport, ctc_direction_t dir, uint16 *threshold);


/***********************************************************
*   Advanced vlan
***********************************************************/
extern int32
sys_humber_port_set_protocol_vlan_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_protocol_vlan_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_vlan_mapping_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_vlan_mapping_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_vlan_switching_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_vlan_switching_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t type);
extern int32
sys_humber_port_set_vlan_classify_disable(uint16 gport, ctc_vlan_class_type_t type);
extern int32
sys_humber_port_get_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t* type);


/***********************************************************
*   OAM
***********************************************************/
extern int32
sys_humber_port_set_oam_tunnel_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_oam_tunnel_en(uint16 gport, bool *enable);
extern int32
sys_humber_port_set_igs_oam_max_md_level(uint16 gport, uint8 md_level);
extern int32
sys_humber_port_get_igs_oam_max_md_level(uint16 gport, uint8 * md_level);
extern int32
sys_humber_port_set_igs_oam_valid(uint16 gport, bool enable);
extern int32
sys_humber_port_get_igs_oam_valid(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_egs_oam_max_md_level(uint16 gport, uint8 md_level);
extern int32
sys_humber_port_get_egs_oam_max_md_level(uint16 gport, uint8 * md_level);
extern int32
sys_humber_port_set_egs_oam_valid(uint16 gport, bool enable);
extern int32
sys_humber_port_get_egs_oam_valid(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_discard_none_8023oam_en(uint16 gport, bool enable);

extern int32
sys_humber_port_set_replace_tag_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_replace_tag_en(uint16 gport, bool *enable);

/***********************************************************
*   PTP
***********************************************************/
extern int32
sys_humber_port_set_ptp_en(uint16 gport, bool enable);
extern int32
sys_humber_port_get_ptp_en(uint16 gport, bool *enable);

/***************************************************************************************
 *Mirror, apply only for sys calling
 **************************************************************************************/
extern int32
sys_humber_port_set_span_en(uint16 gport, ctc_direction_t dir, bool enable);

extern int32
sys_humber_port_get_span_en(uint16 gport, ctc_direction_t dir, bool *enable);

extern int32
sys_humber_port_set_span_id(uint16 gport, ctc_direction_t dir, uint8 span_id);

extern int32
sys_humber_port_get_span_id(uint16 gport, ctc_direction_t dir, uint8 *span_id);

/*************************************************************************************
 *Usrid, apply only for sys calling
 *************************************************************************************/
extern int32
sys_humber_port_set_usrid_enable(uint16 gport, bool enable);

extern int32
sys_humber_port_get_usrid_enable(uint16 gport, bool* enable);

extern int32
sys_humber_port_set_usrid_type(uint16 gport, sys_port_usrid_type_t type);

extern int32
sys_humber_port_get_usrid_type(uint16 gport, sys_port_usrid_type_t* type);

extern int32
sys_humber_port_set_usrid_label(uint16 gport, uint8 label_id);

extern int32
sys_humber_port_get_usrid_label(uint16 gport, uint8* label_id);

/***********************************************************************************
 *ACL QoS, apply only for sys calling
 ***********************************************************************************/
extern int32
sys_humber_port_set_l2acl_enable(uint16 gport, ctc_direction_t dir, bool enable);

extern int32
sys_humber_port_get_l2acl_enable(uint16 gport, ctc_direction_t dir, bool *enable);

extern int32
sys_humber_port_set_l2acl_label(uint16 gport, ctc_direction_t dir, uint8 label);

extern int32
sys_humber_port_get_l2acl_label(uint16 gport, ctc_direction_t dir, uint8 *label);

extern int32
sys_humber_port_set_l2acl_prio(uint16 gport, ctc_direction_t dir, bool l2_high_prio);

extern int32
sys_humber_port_get_l2acl_prio(uint16 gport, ctc_direction_t dir, bool *l2_high_prio);

extern int32
sys_humber_port_set_l2qos_enable(uint16 gport, ctc_direction_t dir, bool enable);

extern int32
sys_humber_port_get_l2qos_enable(uint16 gport, ctc_direction_t dir, bool *enable);

extern int32
sys_humber_port_set_l2qos_label(uint16 gport, ctc_direction_t dir, uint8 label);

extern int32
sys_humber_port_get_l2qos_label(uint16 gport, ctc_direction_t dir, uint8 *label);

extern int32
sys_humber_port_set_l2qos_prio(uint16 gport, ctc_direction_t dir, bool l2_high_prio);

extern int32
sys_humber_port_get_l2qos_prio(uint16 gport, ctc_direction_t dir, bool *l2_high_prio);

extern int32
sys_humber_port_set_qos_domain(uint16 gport, ctc_direction_t dir, uint8 domain);

extern int32
sys_humber_port_get_qos_domain(uint16 gport, ctc_direction_t dir, uint8 *domain);

extern int32
sys_humber_port_set_qos_policy(uint16 gport, uint8 policy);

extern int32
sys_humber_port_get_qos_policy(uint16 gport, uint8 *policy);

extern int32
sys_humber_port_set_port_policer_valid(uint16 gport, ctc_direction_t dir, bool valid);

extern int32
sys_humber_port_get_port_policer_valid(uint16 gport, ctc_direction_t dir, bool *valid);

extern int32
sys_humber_port_set_qacl_force_ipv4_mackey(uint16 gport, ctc_direction_t dir, bool force_mac_key);

extern int32
sys_humber_port_get_qacl_force_ipv4_mackey(uint16 gport, ctc_direction_t dir, bool *force_mac_key);

extern int32
sys_humber_port_set_qacl_force_ipv6_mackey(uint16 gport, ctc_direction_t dir, bool force_mac_key);

extern int32
sys_humber_port_get_qacl_force_ipv6_mackey(uint16 gport, ctc_direction_t dir, bool *force_mac_key);

extern int32
sys_humber_port_set_replace_cos_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_replace_cos_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_replace_dscp_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_replace_dscp_en(uint16 gport, bool *enable);


/*port Mac related*/
extern int32
sys_humber_port_set_mac_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_mac_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_speed(uint16 gport, ctc_port_speed_t speed_mode);

extern int32
sys_humber_port_get_speed(uint16 gport, ctc_port_speed_t *speed_mode);

extern int32
sys_humber_set_max_frame_size(ctc_frame_size_t index, uint16 value);

extern int32
sys_humber_get_max_frame_size(ctc_frame_size_t index, uint16 *max_size);

extern int32
sys_humber_set_cpu_mac_en(bool enable);

extern int32
sys_humber_get_cpu_mac_en(bool *enable);

extern int32
sys_humber_port_set_max_frame(uint16 gport, ctc_frame_size_t index);

extern int32
sys_humber_port_get_max_frame(uint16 gport, ctc_frame_size_t *index);

extern int32
sys_humber_port_set_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 enable);

extern int32
sys_humber_port_get_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 *enable);

extern int32
sys_humber_port_set_preamble(uint16 gport, uint8 pre_bytes);

extern int32
sys_humber_port_get_preamble(uint16 gport, uint8 *pre_bytes);

extern int32
sys_humber_port_set_min_frame_size(uint16 gport, uint8 size);

extern int32
sys_humber_port_get_min_frame_size(uint16 gport, uint8* size);

extern int32
sys_humber_port_set_stretch_mode_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_stretch_mode_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_pading_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_pading_en(uint16 gport, bool *enable);


extern int32
sys_humber_port_set_srcdiscard_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_srcdiscard_en(uint16 gport, bool *enable);

extern int32
sys_humber_port_set_tx_threshold(uint16 gport, uint8 tx_threshold);

extern int32
sys_humber_port_get_tx_threshold(uint16 gport, uint8 *tx_threshold);

extern int32
sys_humber_port_set_loopback (ctc_port_lbk_param_t *p_port_lbk);

extern int32
sys_humber_port_set_port_check_en(uint16 gport, bool enable);

extern int32
sys_humber_port_get_port_check_en(uint16 gport, bool *enable);

#endif

