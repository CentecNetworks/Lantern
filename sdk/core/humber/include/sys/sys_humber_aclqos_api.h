/**
 @file sys_humber_aclqos_api.h

 @date 2009-11-30

 @version v2.0

 The file defines acl/qos api
*/

#ifndef _SYS_HUMBER_ACLQOS_API_H_
#define _SYS_HUMBER_ACLQOS_API_H_


/****************************************************************************
  *
  * Defines and Macros
  *
  ****************************************************************************/


/****************************************************************************
  *
  * Global and Declaration
  *
  ****************************************************************************/


/****************************************************************************
  *
  * Function Declaration
  *
  ****************************************************************************/

/**
 @brief Acl/QoS look up enable/disable on the given port
*/
extern int32
sys_humber_aclqos_port_enable(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable);

/**
 @brief Acl/QoS look up enable/disable on the given vlan
*/
extern int32
sys_humber_aclqos_vlan_enable(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable);

/**
 @brief Acl pbr look up enable/disable on the given l3 interface
*/
extern int32
sys_humber_acl_pbr_enable(uint16 l3if_id, bool enable);

/**
 @brief create acl/qos port label
*/
extern int32
sys_humber_aclqos_port_label_create(ctc_aclqos_label_type_t label_type, uint32 label_id);


/**
 @brief create acl/qos vlan label
*/
extern int32
sys_humber_aclqos_vlan_label_create(ctc_aclqos_label_type_t label_type, uint32 label_id);

/**
 @brief create acl pbr label
*/
extern int32
sys_humber_acl_pbr_label_create(uint32 label_id);

/**
 @brief create acl/qos service label
*/
extern int32
sys_humber_aclqos_service_label_create(uint16 label_id);

/**
 @brief remove acl/qos port label
*/
extern int32
sys_humber_aclqos_port_label_remove(ctc_aclqos_label_type_t label_type, uint32 label_id);


/**
 @brief remove acl/qos vlan label
*/
extern int32
sys_humber_aclqos_vlan_label_remove(ctc_aclqos_label_type_t label_type, uint32 label_id);

/**
 @brief remove acl pbr label
*/
int32
sys_humber_acl_pbr_label_remove(uint32 label_id);

/**
 @brief remove acl/qos service label
*/
extern int32
sys_humber_aclqos_service_label_remove(uint16 label_id);

/**
 @brief set acl/qos label to the given port
*/
extern int32
sys_humber_aclqos_port_label_set(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, uint32 label_id);


/**
 @brief set acl/qos label to the given vlan
*/
extern int32
sys_humber_aclqos_vlan_label_set(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, uint32 label_id);

/**
 @brief set acl pbr label to the given l3 interface
*/
int32
sys_humber_acl_pbr_label_set(uint16 l3if_id, uint32 label_id);

/**
 @brief unset acl/qos label to the given port
*/
extern int32
sys_humber_aclqos_port_label_unset(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type);

/**
 @brief unset acl/qos label to the given vlan
*/
extern int32
sys_humber_aclqos_vlan_label_unset(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type);

/**
 @brief unset acl pbr label to the given l3 interface
*/
int32
sys_humber_acl_pbr_label_unset(uint16 l3if_id);

/**
 @brief set l2 acl high priority
*/
extern int32
sys_humber_acl_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio);


/**
 @brief set l2 qos high priority
*/
extern int32
sys_humber_qos_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio);


/**
 @brief To globally enable/disable QoS policer.
*/
extern int32
sys_humber_qos_policer_global_enable(bool enable);

extern int32
sys_humber_qos_set_sequential_policer(bool enable);

extern int32
sys_humber_qos_set_policer_ipg_global_enable(bool enable);

extern int32
sys_humber_qos_get_service_policer_num(uint32* service_policer_num);

extern int32
sys_humber_qos_set_flow_policer_first_en(ctc_direction_t dir, bool enable);

/**
 @brief Get QoS policer global enable status.
*/
extern int32
sys_humber_qos_get_policer_global_enable(bool* p_enable);


/**
 @brief create qos flow policer
*/
extern int32
sys_humber_qos_flow_policer_create(ctc_qos_policer_t* p_policer, uint32 plc_id);


/**
 @brief remove qos flow policer
*/
extern int32
sys_humber_qos_flow_policer_remove(uint32 plc_id);


/**
 @brief update qos flow policer, replace old policer with new policer
*/
extern int32
sys_humber_qos_flow_policer_update(uint32 plc_id, ctc_qos_policer_t* p_policer);


/**
 @brief Get real flow policer data added to chip.
*/
extern int32
sys_humber_qos_get_flow_policer(uint32 plc_id, ctc_qos_policer_t* p_policer);


/**
 @brief set port policer
*/
extern int32
sys_humber_qos_set_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer);

/**
 @brief set service policer
*/
extern int32
sys_humber_qos_set_service_policer(uint16 service_id, ctc_qos_policer_t* p_policer);

/**
 @brief unset service policer
*/
extern int32
sys_humber_qos_unset_service_policer(uint16 service_id);

/**
 @brief unset port policer
*/
extern int32
sys_humber_qos_unset_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset);


/**
 @brief Get real port policer data added to chip.
*/
extern int32
sys_humber_qos_get_port_policer(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer);

/**
 @brief To globally enable/disable QoS policer statistics globally.
*/
extern int32
sys_humber_qos_policer_stats_global_enable(bool enable);


/**
 @brief Get QoS policer statistics enable status.
*/
extern int32
sys_humber_qos_get_policer_stats_global_enable(bool* p_enable);


/**
 @brief Get flow policer statistics.
*/
extern int32
sys_humber_qos_get_flow_policer_stats(uint32 plc_id, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear flow policer statistics.
*/
extern int32
sys_humber_qos_reset_flow_policer_stats(uint32 plc_id);


/**
 @brief Get port policer statistics.
*/
extern int32
sys_humber_qos_get_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear port policer statistics.
*/
extern int32
sys_humber_qos_reset_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset);

/**
 @brief set ingress cos -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_cos_priority_color_map(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, uint8 color);


/**
 @brief set ingress dscp -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_dscp_priority_color_map(uint8 domain, uint8 dscp, uint8 priority, uint8 color);


/**
 @brief set ingress ip precedence -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_ip_prec_priority_color_map(uint8 domain, uint8 ip_prec, uint8 priority, uint8 color);


/**
 @brief set ingress mpls exp -> priority + color mapping table for the given domain
*/
extern int32
sys_humber_qos_set_exp_priority_color_map(uint8 domain, uint8 exp, uint8 priority, uint8 color);


/**
 @brief set egress priority + color -> cos mapping table for the given domain
*/
extern int32
sys_humber_qos_set_priority_color_cos_map(uint8 domain, uint8 priority, uint8 color, uint8 cos, uint8 cfi);


/**
 @brief set egress priority + color -> dscp mapping table for the given domain
*/
extern int32
sys_humber_qos_set_priority_color_dscp_map(uint8 domain, uint8 priority, uint8 color, uint8 dscp);


/**
 @brief set egress priority + color -> mpls exp mapping table for the given domain
*/
extern int32
sys_humber_qos_set_priority_color_exp_map(uint8 domain, uint8 priority, uint8 color, uint8 exp);

/**
 @brief set domain default value
*/
extern int32
sys_humber_qos_set_default_domain(void);

/**
 @brief set qos domain for the given port
*/
extern int32
sys_humber_qos_set_port_domain(uint16 gport, ctc_direction_t dir, uint8 domain);


/**
 @brief get qos domain for the given port
*/
extern int32
sys_humber_qos_get_port_domain(uint16 gport, ctc_direction_t dir, uint8* p_domain);


/**
 @brief set trust state for the given port
*/
extern int32
sys_humber_qos_set_port_trust(uint16 gport, ctc_qos_trust_t trust);


/**
 @brief get trust state for the given port
*/
extern int32
sys_humber_qos_get_port_trust(uint16 gport, ctc_qos_trust_t* p_trust);


/**
 @brief set default cos for the given port
*/
extern int32
sys_humber_qos_set_port_cos(uint16 gport, uint8 cos);

/**
 @brief set default cfi for the given port
*/
extern int32
sys_humber_qos_set_port_cfi(uint16 gport, uint8 cfi);

/**
 @brief get default cos for the given port
*/
extern int32
sys_humber_qos_get_port_cos(uint16 gport, uint8* p_cos);

/**
 @brief get default cfi for the given port
*/
extern int32
sys_humber_qos_get_port_cfi(uint16 gport, uint8* p_cfi);


/**
 @brief set/unset replace state for the given port
*/
extern int32
sys_humber_qos_set_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool enable);


/**
 @brief Get replace state for the given port
*/
extern int32
sys_humber_qos_get_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool* p_enable);


/**
 @brief add an acl/qos entry to the the given label
*/
extern int32
sys_humber_aclqos_entry_add(uint32 label_id, ctc_aclqos_label_type_t label_type, uint32 entry_id, ctc_aclqos_entry_t* p_entry);


/**
 @brief remove acl/qos entry from the given label
*/
extern int32
sys_humber_aclqos_entry_remove(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id);


/**
 @brief remove all acl/qos entries with particular entry type from the given label
*/
extern int32
sys_humber_aclqos_entry_remove_all(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type);


/**
 @brief To invalid an ACL entry, which indicates removing the entry from hardware table but still
        stay in software table.
*/
extern int32
sys_humber_aclqos_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid);


/**
 @brief get acl/qos entry stats result
*/
extern int32
sys_humber_aclqos_entry_stats_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats);


/**
 @brief reset acl/qos entry stats result
*/
extern int32
sys_humber_aclqos_entry_stats_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id);

/**
 @brief set acl/qos entry action
*/
extern int32
sys_humber_aclqos_entry_set_action(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);


/**
 @brief reset acl/qos entry action
*/
extern int32
sys_humber_aclqos_entry_unset_action(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

extern int32
sys_humber_acl_get_permit_entry_stats_all(uint32 label_id, ctc_stats_basic_t* entry_stats);

extern int32
sys_humber_acl_tiny_fragment_default_entry_add(bool deny);

extern int32
sys_humber_qos_tiny_fragment_default_entry_add(bool deny);

/**
 @brief acl/qos initiliazation
*/
extern int32
sys_humber_aclqos_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg);

#endif

