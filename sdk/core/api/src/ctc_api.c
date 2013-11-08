/**********************************************************
 * ctc_api.c
 * Date:
 * Author: auto generate from include file
 **********************************************************/
/**********************************************************
 * 
 * Header file
 * 
 **********************************************************/
#include "ctc_api.h"
/**********************************************************
 * 
 * Defines and macros
 * 
 **********************************************************/
/**********************************************************
 * 
 * Global and Declaration
 * 
 **********************************************************/
ctc_api_t *ctc_api = NULL;
#ifdef HUMBER
extern ctc_api_t ctc_humber_api;
#endif
/**********************************************************
 * 
 * Functions
 * 
 **********************************************************/
int32 ctc_install_api()
{
    #ifdef HUMBER
    ctc_api = &ctc_humber_api;
    #endif
    return CTC_E_NONE;
} 
int32 ctc_uninstall_api()
{
    ctc_api = NULL;
    return CTC_E_NONE;
}
/*##aclqos##*/
int32 ctc_acl_entry_add(uint32 label_id, uint32 entry_id, ctc_aclqos_entry_t* p_entry)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_add ? ctc_api->ctc_acl_entry_add(label_id, entry_id, p_entry) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_invalid ? ctc_api->ctc_acl_entry_invalid(label_id, entry_type, entry_id, invalid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_remove_all(uint32 label_id, ctc_aclqos_key_type_t entry_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_remove_all ? ctc_api->ctc_acl_entry_remove_all(label_id, entry_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_remove(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_remove ? ctc_api->ctc_acl_entry_remove(label_id, entry_type, entry_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_set_action ? ctc_api->ctc_acl_entry_set_action(label_id, entry_type, entry_id, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_stats_get(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_stats_get ? ctc_api->ctc_acl_entry_stats_get(label_id, entry_type, entry_id, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_stats_reset(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_stats_reset ? ctc_api->ctc_acl_entry_stats_reset(label_id, entry_type, entry_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_entry_unset_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_entry_unset_action ? ctc_api->ctc_acl_entry_unset_action(label_id, entry_type, entry_id, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_get_permit_entry_stats_all(uint32 label_id, ctc_stats_basic_t* entry_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_get_permit_entry_stats_all ? ctc_api->ctc_acl_get_permit_entry_stats_all(label_id, entry_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_pbr_enable(uint16 l3if_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_pbr_enable ? ctc_api->ctc_acl_pbr_enable(l3if_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_pbr_label_create(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_pbr_label_create ? ctc_api->ctc_acl_pbr_label_create(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_pbr_label_remove(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_pbr_label_remove ? ctc_api->ctc_acl_pbr_label_remove(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_pbr_label_set(uint16 l3if_id, uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_pbr_label_set ? ctc_api->ctc_acl_pbr_label_set(l3if_id, label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_pbr_label_unset(uint16 l3if_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_pbr_label_unset ? ctc_api->ctc_acl_pbr_label_unset(l3if_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_port_enable(uint16 gport, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_port_enable ? ctc_api->ctc_acl_port_enable(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_port_label_create(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_port_label_create ? ctc_api->ctc_acl_port_label_create(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_port_label_remove(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_port_label_remove ? ctc_api->ctc_acl_port_label_remove(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_port_label_set(uint16 gport, ctc_direction_t dir, uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_port_label_set ? ctc_api->ctc_acl_port_label_set(gport, dir, label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_port_label_unset(uint16 gport, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_port_label_unset ? ctc_api->ctc_acl_port_label_unset(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_aclqos_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_aclqos_init ? ctc_api->ctc_aclqos_init(aclqos_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_set_l2_high_priority ? ctc_api->ctc_acl_set_l2_high_priority(gport, dir, l2_high_prio) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_tiny_fragment_default_entry_add(bool deny)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_tiny_fragment_default_entry_add ? ctc_api->ctc_acl_tiny_fragment_default_entry_add(deny) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_vlan_enable(uint16 vid, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_vlan_enable ? ctc_api->ctc_acl_vlan_enable(vid, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_vlan_label_create(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_vlan_label_create ? ctc_api->ctc_acl_vlan_label_create(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_vlan_label_remove(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_vlan_label_remove ? ctc_api->ctc_acl_vlan_label_remove(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_vlan_label_set(uint16 vid, ctc_direction_t dir, uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_vlan_label_set ? ctc_api->ctc_acl_vlan_label_set(vid, dir, label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_acl_vlan_label_unset(uint16 vid, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_acl_vlan_label_unset ? ctc_api->ctc_acl_vlan_label_unset(vid, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_add(uint32 label_id, uint32 entry_id, ctc_aclqos_entry_t* p_entry)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_add ? ctc_api->ctc_qos_entry_add(label_id, entry_id, p_entry) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_invalid ? ctc_api->ctc_qos_entry_invalid(label_id, entry_type, entry_id, invalid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_remove_all(uint32 label_id, ctc_aclqos_key_type_t entry_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_remove_all ? ctc_api->ctc_qos_entry_remove_all(label_id, entry_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_remove(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_remove ? ctc_api->ctc_qos_entry_remove(label_id, entry_type, entry_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_set_action ? ctc_api->ctc_qos_entry_set_action(label_id, entry_type, entry_id, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_stats_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_stats_get ? ctc_api->ctc_qos_entry_stats_get(label_id, label_type, entry_type, entry_id, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_stats_reset(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_stats_reset ? ctc_api->ctc_qos_entry_stats_reset(label_id, entry_type, entry_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_entry_unset_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_entry_unset_action ? ctc_api->ctc_qos_entry_unset_action(label_id, entry_type, entry_id, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_flow_policer_create(ctc_qos_policer_t* p_policer, uint32 plc_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_flow_policer_create ? ctc_api->ctc_qos_flow_policer_create(p_policer, plc_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_flow_policer_remove(uint32 plc_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_flow_policer_remove ? ctc_api->ctc_qos_flow_policer_remove(plc_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_flow_policer_update(uint32 plc_id, ctc_qos_policer_t* p_policer)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_flow_policer_update ? ctc_api->ctc_qos_flow_policer_update(plc_id, p_policer) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_flow_policer_stats(uint32 plc_id, ctc_qos_policer_stats_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_flow_policer_stats ? ctc_api->ctc_qos_get_flow_policer_stats(plc_id, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_flow_policer(uint32 plc_id, ctc_qos_policer_t* p_policer)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_flow_policer ? ctc_api->ctc_qos_get_flow_policer(plc_id, p_policer) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_policer_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_policer_global_enable ? ctc_api->ctc_qos_get_policer_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_policer_stats_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_policer_stats_global_enable ? ctc_api->ctc_qos_get_policer_stats_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_cfi(uint16 gport, uint8* p_cfi)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_cfi ? ctc_api->ctc_qos_get_port_cfi(gport, p_cfi) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_cos(uint16 gport, uint8* p_cos)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_cos ? ctc_api->ctc_qos_get_port_cos(gport, p_cos) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_domain(uint16 gport, ctc_direction_t dir, uint8* p_domain)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_domain ? ctc_api->ctc_qos_get_port_domain(gport, dir, p_domain) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_policer_stats ? ctc_api->ctc_qos_get_port_policer_stats(gport, dir, phb_offset, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_policer(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_policer ? ctc_api->ctc_qos_get_port_policer(gport, dir, phb_offset, p_policer) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_replace_enable ? ctc_api->ctc_qos_get_port_replace_enable(gport, replace, p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_port_trust(uint16 gport, ctc_qos_trust_t* p_trust)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_port_trust ? ctc_api->ctc_qos_get_port_trust(gport, p_trust) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_policer_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_policer_global_enable ? ctc_api->ctc_qos_policer_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_policer_stats_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_policer_stats_global_enable ? ctc_api->ctc_qos_policer_stats_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_port_enable(uint16 gport, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_port_enable ? ctc_api->ctc_qos_port_enable(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_port_label_create(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_port_label_create ? ctc_api->ctc_qos_port_label_create(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_port_label_remove(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_port_label_remove ? ctc_api->ctc_qos_port_label_remove(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_port_label_set(uint16 gport, ctc_direction_t dir, uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_port_label_set ? ctc_api->ctc_qos_port_label_set(gport, dir, label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_port_label_unset(uint16 gport, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_port_label_unset ? ctc_api->ctc_qos_port_label_unset(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_reset_flow_policer_stats(uint32 plc_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_reset_flow_policer_stats ? ctc_api->ctc_qos_reset_flow_policer_stats(plc_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_reset_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_reset_port_policer_stats ? ctc_api->ctc_qos_reset_port_policer_stats(gport, dir, phb_offset) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_cos_priority_color_map(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, ctc_qos_color_t color)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_cos_priority_color_map ? ctc_api->ctc_qos_set_cos_priority_color_map(domain, cos, cfi, priority, color) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_default_domain(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_default_domain ? ctc_api->ctc_qos_set_default_domain() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_dscp_priority_color_map(uint8 domain, uint8 dscp, uint8 priority, ctc_qos_color_t color)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_dscp_priority_color_map ? ctc_api->ctc_qos_set_dscp_priority_color_map(domain, dscp, priority, color) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_exp_priority_color_map(uint8 domain, uint8 exp, uint8 priority, ctc_qos_color_t color)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_exp_priority_color_map ? ctc_api->ctc_qos_set_exp_priority_color_map(domain, exp, priority, color) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_flow_policer_first_en(ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_flow_policer_first_en ? ctc_api->ctc_qos_set_flow_policer_first_en(dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_ip_prec_priority_color_map(uint8 domain, uint8 ip_prec, uint8 priority, ctc_qos_color_t color)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_ip_prec_priority_color_map ? ctc_api->ctc_qos_set_ip_prec_priority_color_map(domain, ip_prec, priority, color) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_l2_high_priority ? ctc_api->ctc_qos_set_l2_high_priority(gport, dir, l2_high_prio) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_policer_ipg_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_policer_ipg_global_enable ? ctc_api->ctc_qos_set_policer_ipg_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_cfi(uint16 gport, uint8 cfi)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_cfi ? ctc_api->ctc_qos_set_port_cfi(gport, cfi) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_cos(uint16 gport, uint8 cos)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_cos ? ctc_api->ctc_qos_set_port_cos(gport, cos) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_domain(uint16 gport, ctc_direction_t dir, uint8 domain)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_domain ? ctc_api->ctc_qos_set_port_domain(gport, dir, domain) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_policer ? ctc_api->ctc_qos_set_port_policer(gport, dir, phb_offset, p_policer) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_replace_enable ? ctc_api->ctc_qos_set_port_replace_enable(gport, replace, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_port_trust(uint16 gport, ctc_qos_trust_t trust)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_port_trust ? ctc_api->ctc_qos_set_port_trust(gport, trust) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_priority_color_cos_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 cos, uint8 cfi)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_priority_color_cos_map ? ctc_api->ctc_qos_set_priority_color_cos_map(domain, priority, color, cos, cfi) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_priority_color_dscp_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 dscp)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_priority_color_dscp_map ? ctc_api->ctc_qos_set_priority_color_dscp_map(domain, priority, color, dscp) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_priority_color_exp_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 exp)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_priority_color_exp_map ? ctc_api->ctc_qos_set_priority_color_exp_map(domain, priority, color, exp) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_sequential_policer(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_sequential_policer ? ctc_api->ctc_qos_set_sequential_policer(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_set_service_policer(uint16 service_id, ctc_qos_policer_t* p_policer)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_set_service_policer ? ctc_api->ctc_qos_set_service_policer(service_id, p_policer) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_tiny_fragment_default_entry_add(bool deny)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_tiny_fragment_default_entry_add ? ctc_api->ctc_qos_tiny_fragment_default_entry_add(deny) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_unset_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_unset_port_policer ? ctc_api->ctc_qos_unset_port_policer(gport, dir, phb_offset) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_unset_service_policer(uint16 service_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_unset_service_policer ? ctc_api->ctc_qos_unset_service_policer(service_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_vlan_enable(uint16 vid, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_vlan_enable ? ctc_api->ctc_qos_vlan_enable(vid, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_vlan_label_create(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_vlan_label_create ? ctc_api->ctc_qos_vlan_label_create(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_vlan_label_remove(uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_vlan_label_remove ? ctc_api->ctc_qos_vlan_label_remove(label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_vlan_label_set(uint16 vid, ctc_direction_t dir, uint32 label_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_vlan_label_set ? ctc_api->ctc_qos_vlan_label_set(vid, dir, label_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_vlan_label_unset(uint16 vid, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_vlan_label_unset ? ctc_api->ctc_qos_vlan_label_unset(vid, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_entry_add(ctc_aclqos_entry_oper_t* p_entry_oper)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_entry_add ? ctc_api->ctc_service_entry_add(p_entry_oper) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_entry_remove_all(uint16 service_id, ctc_aclqos_key_type_t entry_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_entry_remove_all ? ctc_api->ctc_service_entry_remove_all(service_id, entry_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_entry_remove(ctc_aclqos_entry_oper_t* p_entry_oper)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_entry_remove ? ctc_api->ctc_service_entry_remove(p_entry_oper) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_entry_set_action ? ctc_api->ctc_service_entry_set_action(label_id, entry_type, entry_id, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_label_create(uint16 service_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_label_create ? ctc_api->ctc_service_label_create(service_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_service_label_remove(uint16 service_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_service_label_remove ? ctc_api->ctc_service_label_remove(service_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##chip##*/
int32 ctc_chip_init(uint8 lchip_num)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_chip_init ? ctc_api->ctc_chip_init(lchip_num) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_data_path_init(ctc_chip_reset_cb reset_cb, ctc_chip_datapath_t* chip_datapath, char* datapath_config_file)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_data_path_init ? ctc_api->ctc_data_path_init(reset_cb, chip_datapath, datapath_config_file) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_chip_clock(uint16* freq)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_chip_clock ? ctc_api->ctc_get_chip_clock(freq) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_gchip_id(uint8 lchip_id, uint8* gchip_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_gchip_id ? ctc_api->ctc_get_gchip_id(lchip_id, gchip_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_local_chip_num(uint8* lchip_num)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_local_chip_num ? ctc_api->ctc_get_local_chip_num(lchip_num) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parity_error_init(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parity_error_init ? ctc_api->ctc_parity_error_init() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_chip_global_cfg(ctc_chip_global_cfg_t* chip_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_chip_global_cfg ? ctc_api->ctc_set_chip_global_cfg(chip_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_gchip_id(uint8 lchip_id, uint8 gchip_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_gchip_id ? ctc_api->ctc_set_gchip_id(lchip_id, gchip_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##cpu_traffic##*/
int32 ctc_cpu_traffic_init(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_init ? ctc_api->ctc_cpu_traffic_init() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_cpu_traffic_set_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_set_exception ? ctc_api->ctc_cpu_traffic_set_exception(excp, dest_id, dest_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_cpu_traffic_set_fatal_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_set_fatal_exception ? ctc_api->ctc_cpu_traffic_set_fatal_exception(excp, dest_id, dest_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_cpu_traffic_set_fwd_queue_class(uint8 qid, uint8 class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_set_fwd_queue_class ? ctc_api->ctc_cpu_traffic_set_fwd_queue_class(qid, class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_cpu_traffic_set_fwd_queue_shape(uint8 qid, ctc_queue_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_set_fwd_queue_shape ? ctc_api->ctc_cpu_traffic_set_fwd_queue_shape(qid, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_cpu_traffic_unset_fwd_queue_shape(uint8 qid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_cpu_traffic_unset_fwd_queue_shape ? ctc_api->ctc_cpu_traffic_unset_fwd_queue_shape(qid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_packet_tocpu_limit_individual_rate(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint32 rate)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_packet_tocpu_limit_individual_rate ? ctc_api->ctc_packet_tocpu_limit_individual_rate(reason, sub_idx, rate) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_packet_tocpu_limit_total_rate(uint32 rate)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_packet_tocpu_limit_total_rate ? ctc_api->ctc_packet_tocpu_limit_total_rate(rate) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_packet_tocpu_set_reason_class(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint8 class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_packet_tocpu_set_reason_class ? ctc_api->ctc_packet_tocpu_set_reason_class(reason, sub_idx, class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##ftm##*/
int32 ctc_ftm_mem_alloc(ctc_ftm_profile_info_t* ctc_profile_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ftm_mem_alloc ? ctc_api->ctc_ftm_mem_alloc(ctc_profile_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ftm_set_default_profile(ctc_ftm_profile_info_t* ctc_profile_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ftm_set_default_profile ? ctc_api->ctc_ftm_set_default_profile(ctc_profile_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ftm_show_alloc_info(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ftm_show_alloc_info ? ctc_api->ctc_ftm_show_alloc_info() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##internal_port##*/
int32 ctc_internal_port_allocate(ctc_internal_port_assign_para_t* port_assign)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_internal_port_allocate ? ctc_api->ctc_internal_port_allocate(port_assign) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_internal_port_init(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_internal_port_init ? ctc_api->ctc_internal_port_init() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_internal_port_release(ctc_internal_port_assign_para_t* port_assign)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_internal_port_release ? ctc_api->ctc_internal_port_release(port_assign) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_internal_port_set(ctc_internal_port_assign_para_t* port_assign)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_internal_port_set ? ctc_api->ctc_internal_port_set(port_assign) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##interrupt##*/
int32 ctc_interrupt_clear_fatal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_clear_fatal_intr ? ctc_api->ctc_interrupt_clear_fatal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_clear_normal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_clear_normal_intr ? ctc_api->ctc_interrupt_clear_normal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_clear_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_clear_sub_normal_intr ? ctc_api->ctc_interrupt_clear_sub_normal_intr(lchip, type, bit_offset) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_disable_fatal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_disable_fatal_intr ? ctc_api->ctc_interrupt_disable_fatal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_disable_normal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_disable_normal_intr ? ctc_api->ctc_interrupt_disable_normal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_disable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_disable_sub_normal_intr ? ctc_api->ctc_interrupt_disable_sub_normal_intr(lchip, type, bit_offset) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_enable_fatal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_enable_fatal_intr ? ctc_api->ctc_interrupt_enable_fatal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_enable_normal_intr(uint8 lchip, uint8 type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_enable_normal_intr ? ctc_api->ctc_interrupt_enable_normal_intr(lchip, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_enable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_enable_sub_normal_intr ? ctc_api->ctc_interrupt_enable_sub_normal_intr(lchip, type, bit_offset) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t* status)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_get_fatal_intr_status ? ctc_api->ctc_interrupt_get_fatal_intr_status(lchip, type, status) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_get_normal_all_intr_status(uint8 lchip, uint32* p_bitmap)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_get_normal_all_intr_status ? ctc_api->ctc_interrupt_get_normal_all_intr_status(lchip, p_bitmap) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_get_normal_intr_status(uint8 lchip, uint8 type, bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_get_normal_intr_status ? ctc_api->ctc_interrupt_get_normal_intr_status(lchip, type, p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_reg_init_end(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_reg_init_end ? ctc_api->ctc_interrupt_reg_init_end() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_interrupt_reg_init_start(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_interrupt_reg_init_start ? ctc_api->ctc_interrupt_reg_init_start() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##ipuc##*/
int32 ctc_ipuc_add(ctc_ipuc_param_t* p_ipuc_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_add ? ctc_api->ctc_ipuc_add(p_ipuc_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_add_default_entry(ctc_ipuc_param_t* p_ipuc_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_add_default_entry ? ctc_api->ctc_ipuc_add_default_entry(p_ipuc_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_cpu_rpf_check(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_cpu_rpf_check ? ctc_api->ctc_ipuc_cpu_rpf_check(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_init(void* ipuc_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_init ? ctc_api->ctc_ipuc_init(ipuc_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_ipv6_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_ipv6_enable ? ctc_api->ctc_ipuc_ipv6_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_remove(ctc_ipuc_param_t* p_ipuc_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_remove ? ctc_api->ctc_ipuc_remove(p_ipuc_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_set_lookup_ctl(ctc_ipuc_lookup_ctl_t* p_lookup_ctl_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_set_lookup_ctl ? ctc_api->ctc_ipuc_set_lookup_ctl(p_lookup_ctl_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_set_route_ctl(ctc_ipuc_route_ctl_t* p_route_ctl_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_set_route_ctl ? ctc_api->ctc_ipuc_set_route_ctl(p_route_ctl_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##l2##*/
int32 ctc_l2_add_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_add_default_entry ? ctc_api->ctc_l2_add_default_entry(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_add_fdb(ctc_l2_addr_t* l2_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_add_fdb ? ctc_api->ctc_l2_add_fdb(l2_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_add_fdb_with_nexthop(ctc_l2_addr_t* l2_addr, uint32 nhp_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_add_fdb_with_nexthop ? ctc_api->ctc_l2_add_fdb_with_nexthop(l2_addr, nhp_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_add_port_to_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_add_port_to_default_entry ? ctc_api->ctc_l2_add_port_to_default_entry(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_fdb_flush(ctc_l2_fdb_flush_t* pFlush)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_fdb_flush ? ctc_api->ctc_l2_fdb_flush(pFlush) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_fdb_init(void* l2_fdb_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_fdb_init ? ctc_api->ctc_l2_fdb_init(l2_fdb_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_get_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_get_default_entry_features ? ctc_api->ctc_l2_get_default_entry_features(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_get_fdb_by_index(uint32 index,ctc_l2_addr_t* l2_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_get_fdb_by_index ? ctc_api->ctc_l2_get_fdb_by_index(index, l2_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_get_fdb_count(ctc_l2_fdb_query_t* pQuery)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_get_fdb_count ? ctc_api->ctc_l2_get_fdb_count(pQuery) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_get_fdb_entry(ctc_l2_fdb_query_t* pQuery, ctc_l2_fdb_query_rst_t* query_rst)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_get_fdb_entry ? ctc_api->ctc_l2_get_fdb_entry(pQuery, query_rst) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2mcast_add_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2mcast_add_addr ? ctc_api->ctc_l2mcast_add_addr(l2mc_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2mcast_add_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2mcast_add_member ? ctc_api->ctc_l2mcast_add_member(l2mc_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2mcast_remove_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2mcast_remove_addr ? ctc_api->ctc_l2mcast_remove_addr(l2mc_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2mcast_remove_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2mcast_remove_member ? ctc_api->ctc_l2mcast_remove_member(l2mc_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_remove_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_remove_default_entry ? ctc_api->ctc_l2_remove_default_entry(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_remove_fdb_by_index(uint32 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_remove_fdb_by_index ? ctc_api->ctc_l2_remove_fdb_by_index(index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_remove_fdb(ctc_l2_addr_t* l2_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_remove_fdb ? ctc_api->ctc_l2_remove_fdb(l2_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_remove_port_from_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_remove_port_from_default_entry ? ctc_api->ctc_l2_remove_port_from_default_entry(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_set_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_set_default_entry_features ? ctc_api->ctc_l2_set_default_entry_features(l2dflt_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##l3if##*/
int32 ctc_l3if_add_vmac_low_8bit(uint16 l3if_id,ctc_l3if_vmac_t* p_l3if_vmac)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_add_vmac_low_8bit ? ctc_api->ctc_l3if_add_vmac_low_8bit(l3if_id, p_l3if_vmac) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_create(uint16 l3if_id, ctc_l3if_t* p_l3_if)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_create ? ctc_api->ctc_l3if_create(l3if_id, p_l3_if) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_delete(uint16 l3if_id, ctc_l3if_t* p_l3_if)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_delete ? ctc_api->ctc_l3if_delete(l3if_id, p_l3_if) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_get_property(uint16 l3if_id,ctc_l3if_property_t l3if_prop,uint32* p_value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_get_property ? ctc_api->ctc_l3if_get_property(l3if_id, l3if_prop, p_value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_get_router_mac(mac_addr_t mac_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_get_router_mac ? ctc_api->ctc_l3if_get_router_mac(mac_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_get_vmac_low_8bit(uint16 l3if_id,ctc_l3if_vmac_t* p_l3if_vmac)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_get_vmac_low_8bit ? ctc_api->ctc_l3if_get_vmac_low_8bit(l3if_id, p_l3if_vmac) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_get_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_get_vmac_prefix ? ctc_api->ctc_l3if_get_vmac_prefix(prefix_type, mac_40bit) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_init(void* l3if_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_init ? ctc_api->ctc_l3if_init(l3if_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_remove_vmac_low_8bit(uint16 l3if_id,ctc_l3if_vmac_t* p_l3if_vmac)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_remove_vmac_low_8bit ? ctc_api->ctc_l3if_remove_vmac_low_8bit(l3if_id, p_l3if_vmac) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_set_property(uint16 l3if_id,ctc_l3if_property_t l3if_prop,uint32 value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_set_property ? ctc_api->ctc_l3if_set_property(l3if_id, l3if_prop, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_set_router_mac(mac_addr_t mac_addr)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_set_router_mac ? ctc_api->ctc_l3if_set_router_mac(mac_addr) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3if_set_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3if_set_vmac_prefix ? ctc_api->ctc_l3if_set_vmac_prefix(prefix_type, mac_40bit) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##learning_aging##*/
int32 ctc_aging_get_aging_status(uint8 lchip, uint32 aging_index, ctc_aging_status_t* age_status)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_aging_get_aging_status ? ctc_api->ctc_aging_get_aging_status(lchip, aging_index, age_status) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_aging_get_property(ctc_aging_prop_t aging_prop, uint32* value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_aging_get_property ? ctc_api->ctc_aging_get_property(aging_prop, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_aging_read_aging_fifo(uint8 lchip, ctc_aging_fifo_info_t* fifo_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_aging_read_aging_fifo ? ctc_api->ctc_aging_read_aging_fifo(lchip, fifo_info) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_learning_set_learning_en(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_learning_set_learning_en ? ctc_api->ctc_learning_set_learning_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_aging_set_property(ctc_aging_prop_t aging_prop, uint32 value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_aging_set_property ? ctc_api->ctc_aging_set_property(aging_prop, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_learning_action ? ctc_api->ctc_get_learning_action(p_learning_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_learning_aging_init(void* global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_learning_aging_init ? ctc_api->ctc_learning_aging_init(global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_learning_clear_learning_cache(uint8 lchip, uint16 entry_vld_bitmap)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_learning_clear_learning_cache ? ctc_api->ctc_learning_clear_learning_cache(lchip, entry_vld_bitmap) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_learning_get_cache_entry_valid_bitmap(uint8 lchip, uint16* entry_vld_bitmap)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_learning_get_cache_entry_valid_bitmap ? ctc_api->ctc_learning_get_cache_entry_valid_bitmap(lchip, entry_vld_bitmap) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_learning_read_learning_cache(uint8 lchip, uint16 entry_vld_bitmap, ctc_learning_cache_t* l2_lc)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_learning_read_learning_cache ? ctc_api->ctc_learning_read_learning_cache(lchip, entry_vld_bitmap, l2_lc) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_learning_action ? ctc_api->ctc_set_learning_action(p_learning_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##linkagg##*/
int32 ctc_linkagg_add_port(uint8 tid, uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_add_port ? ctc_api->ctc_linkagg_add_port(tid, gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_create(uint8 tid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_create ? ctc_api->ctc_linkagg_create(tid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_get_1st_local_port(uint8 tid, uint16* p_gport,uint8* local_cnt)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_get_1st_local_port ? ctc_api->ctc_linkagg_get_1st_local_port(tid, p_gport, local_cnt) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_init(void* linkagg_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_init ? ctc_api->ctc_linkagg_init(linkagg_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_remove_port(uint8 tid, uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_remove_port ? ctc_api->ctc_linkagg_remove_port(tid, gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_remove(uint8 tid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_remove ? ctc_api->ctc_linkagg_remove(tid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_linkagg_show_ports(uint8 tid, uint16* p_gports, uint8* cnt)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_linkagg_show_ports ? ctc_api->ctc_linkagg_show_ports(tid, p_gports, cnt) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##mirror##*/
int32 ctc_mirror_get_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_get_mirror_discard ? ctc_api->ctc_mirror_get_mirror_discard(dir, discard_flag, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_get_port_info(uint16 gport, ctc_direction_t dir, bool* enable, uint8* session_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_get_port_info ? ctc_api->ctc_mirror_get_port_info(gport, dir, enable, session_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_get_vlan_info(uint16 vlan_id, ctc_direction_t dir, bool* enable, uint8* session_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_get_vlan_info ? ctc_api->ctc_mirror_get_vlan_info(vlan_id, dir, enable, session_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_init(void* mirror_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_init ? ctc_api->ctc_mirror_init(mirror_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_rspan_escape_en(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_rspan_escape_en ? ctc_api->ctc_mirror_rspan_escape_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_rspan_escape_mac(ctc_mirror_rspan_escape_t escape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_rspan_escape_mac ? ctc_api->ctc_mirror_rspan_escape_mac(escape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_rspan_set_dest(ctc_mirror_dest_t* mirror_info, uint32 nh_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_rspan_set_dest ? ctc_api->ctc_mirror_rspan_set_dest(mirror_info, nh_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_dest(ctc_mirror_dest_t* mirror)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_dest ? ctc_api->ctc_mirror_set_dest(mirror) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_mirror_discard ? ctc_api->ctc_mirror_set_mirror_discard(dir, discard_flag, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_port_disable(uint16 gport, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_port_disable ? ctc_api->ctc_mirror_set_port_disable(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_port_enable(uint16 gport, ctc_direction_t dir, uint8 session_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_port_enable ? ctc_api->ctc_mirror_set_port_enable(gport, dir, session_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_vlan_disable(uint16 vlan_id, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_vlan_disable ? ctc_api->ctc_mirror_set_vlan_disable(vlan_id, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_set_vlan_enable(uint16 vlan_id, ctc_direction_t dir, uint8 session_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_set_vlan_enable ? ctc_api->ctc_mirror_set_vlan_enable(vlan_id, dir, session_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mirror_unset_dest(ctc_mirror_dest_t* mirror)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mirror_unset_dest ? ctc_api->ctc_mirror_unset_dest(mirror) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##mpls##*/
int32 ctc_mpls_add_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_add_ilm ? ctc_api->ctc_mpls_add_ilm(p_mpls_ilm) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_add_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_add_l2vpn_pw ? ctc_api->ctc_mpls_add_l2vpn_pw(p_mpls_pw) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_add_stats(ctc_mpls_stats_index_t* stats_index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_add_stats ? ctc_api->ctc_mpls_add_stats(stats_index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_del_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_del_ilm ? ctc_api->ctc_mpls_del_ilm(p_mpls_ilm) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_del_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_del_l2vpn_pw ? ctc_api->ctc_mpls_del_l2vpn_pw(p_mpls_pw) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_del_stats(ctc_mpls_stats_index_t* stats_index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_del_stats ? ctc_api->ctc_mpls_del_stats(stats_index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_get_ilm(uint32* nh_id, ctc_mpls_ilm_t* p_mpls_ilm)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_get_ilm ? ctc_api->ctc_mpls_get_ilm(nh_id, p_mpls_ilm) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_get_stats(ctc_mpls_stats_index_t* stats_index, ctc_stats_basic_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_get_stats ? ctc_api->ctc_mpls_get_stats(stats_index, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_init(ctc_mpls_init_t* p_mpls_info)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_init ? ctc_api->ctc_mpls_init(p_mpls_info) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_reset_in_sqn(uint8 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_reset_in_sqn ? ctc_api->ctc_mpls_reset_in_sqn(index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_reset_out_sqn(uint8 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_reset_out_sqn ? ctc_api->ctc_mpls_reset_out_sqn(index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_reset_stats(ctc_mpls_stats_index_t* stats_index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_reset_stats ? ctc_api->ctc_mpls_reset_stats(stats_index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_update_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_update_ilm ? ctc_api->ctc_mpls_update_ilm(p_mpls_ilm) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_mpls_check_label_used(uint16 spaceid, uint32 label, bool *used)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_check_label_used ? \
            ctc_api->ctc_mpls_check_label_used(spaceid,label, used) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_port_set_allow_mcast_mac_sa(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_allow_mcast_mac_sa ? \
            ctc_api->ctc_port_set_allow_mcast_mac_sa(gport,enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##nexthop##*/
int32 ctc_ecmp_nh_create(ctc_nh_ecmp_creat_data_t* pdata)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ecmp_nh_create ? ctc_api->ctc_ecmp_nh_create(pdata) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ecmp_nh_delete(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ecmp_nh_delete ? ctc_api->ctc_ecmp_nh_delete(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ecmp_nh_update(ctc_nh_ecmp_update_data_t* pdata)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ecmp_nh_update ? ctc_api->ctc_ecmp_nh_update(pdata) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_egress_vlan_edit_create(uint32 nhid, ctc_vlan_edit_nh_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_egress_vlan_edit_create ? ctc_api->ctc_egress_vlan_edit_create(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_egress_vlan_edit_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_egress_vlan_edit_remove ? ctc_api->ctc_egress_vlan_edit_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_flex_nh_create(uint32 nhid, ctc_flex_nh_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_flex_nh_create ? ctc_api->ctc_flex_nh_create(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_flex_nh_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_flex_nh_remove ? ctc_api->ctc_flex_nh_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_iloop_nh_create(uint32 nhid, ctc_loopback_nexthop_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_iloop_nh_create ? ctc_api->ctc_iloop_nh_create(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_iloop_nh_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_iloop_nh_remove ? ctc_api->ctc_iloop_nh_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_nh_create(uint32 nhid, ctc_ip_nh_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_nh_create ? ctc_api->ctc_ipuc_nh_create(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_nh_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_nh_remove ? ctc_api->ctc_ipuc_nh_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_ipuc_nh_update(uint32 nhid, ctc_ip_nh_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_ipuc_nh_update ? ctc_api->ctc_ipuc_nh_update(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_create_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_create_ucast_nh ? ctc_api->ctc_l2_create_ucast_nh(gport, nh_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_delete_ucast_nh(uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_delete_ucast_nh ? ctc_api->ctc_l2_delete_ucast_nh(gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2_get_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type, uint32* nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2_get_ucast_nh ? ctc_api->ctc_l2_get_ucast_nh(gport, nh_type, nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mcast_nh_create(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mcast_nh_create ? ctc_api->ctc_mcast_nh_create(nhid, p_nh_mcast_group) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mcast_nh_delete(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mcast_nh_delete ? ctc_api->ctc_mcast_nh_delete(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mcast_nh_update(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mcast_nh_update ? ctc_api->ctc_mcast_nh_update(nhid, p_nh_mcast_group) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_nh_create(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_nh_create ? ctc_api->ctc_mpls_nh_create(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_nh_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_nh_remove ? ctc_api->ctc_mpls_nh_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_mpls_nh_update(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_mpls_nh_update ? ctc_api->ctc_mpls_nh_update(nhid, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_nexthop_init(ctc_nh_global_cfg_t* nh_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_nexthop_init ? ctc_api->ctc_nexthop_init(nh_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_nh_add_stats(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_nh_add_stats ? ctc_api->ctc_nh_add_stats(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_nh_del_stats(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_nh_del_stats ? ctc_api->ctc_nh_del_stats(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_nh_get_stats(uint32 nhid, ctc_stats_basic_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_nh_get_stats ? ctc_api->ctc_nh_get_stats(nhid, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_nh_reset_stats(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_nh_reset_stats ? ctc_api->ctc_nh_reset_stats(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_rspan_nh_create(uint32 nhid, uint32 dsnh_offset,ctc_rspan_nexthop_param_t* p_nh_param)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_rspan_nh_create ? ctc_api->ctc_rspan_nh_create(nhid, dsnh_offset, p_nh_param) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_rspan_nh_remove(uint32 nhid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_rspan_nh_remove ? ctc_api->ctc_rspan_nh_remove(nhid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##parser##*/
int32 ctc_parser_add_l2_type(uint8 index, ctc_parser_l2_ptl_entry_t* entry)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_add_l2_type ? ctc_api->ctc_parser_add_l2_type(index, entry) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_add_l3_type(uint8 index, ctc_parser_l3_ptl_entry_t* entry)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_add_l3_type ? ctc_api->ctc_parser_add_l3_type(index, entry) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_ip_hash_ctl ? ctc_api->ctc_parser_get_ip_hash_ctl(hash_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_ipv6_ctl ? ctc_api->ctc_parser_get_ipv6_ctl(index, ipv6_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_l2flex_ctl ? ctc_api->ctc_parser_get_l2flex_ctl(index, l2flex_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_l2_hash ? ctc_api->ctc_parser_get_l2_hash(l2ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_l2_type(ctc_parser_l2_ptl_entry_t* p_entry, ctc_parser_l3_type_t* p_l3_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_l2_type ? ctc_api->ctc_parser_get_l2_type(p_entry, p_l3_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_l3flex_ctl ? ctc_api->ctc_parser_get_l3flex_ctl(index, l3flex_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_layer4_parser_ctl ? ctc_api->ctc_parser_get_layer4_parser_ctl(tbl_flags, l4_parser_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_max_length_filed(uint16* max_length)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_max_length_filed ? ctc_api->ctc_parser_get_max_length_filed(max_length) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_mpls_ctl ? ctc_api->ctc_parser_get_mpls_ctl(mpls_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_parser_quad_vlan_en(uint8* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_parser_quad_vlan_en ? ctc_api->ctc_parser_get_parser_quad_vlan_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_pbb_parser_ctl ? ctc_api->ctc_parser_get_pbb_parser_ctl(pbb_parser_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_snap_allow_none_zero_oui(uint8* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_snap_allow_none_zero_oui ? ctc_api->ctc_parser_get_snap_allow_none_zero_oui(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_tpid(ctc_parser_l2_tpid_t type, uint16* tpid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_tpid ? ctc_api->ctc_parser_get_tpid(type, tpid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_get_vlan_parser_num(uint8* vlan_num)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_get_vlan_parser_num ? ctc_api->ctc_parser_get_vlan_parser_num(vlan_num) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_init(void* parser_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_init ? ctc_api->ctc_parser_init(parser_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_l2_enable_l3_type(ctc_parser_l3_type_t l3_type, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_l2_enable_l3_type ? ctc_api->ctc_parser_l2_enable_l3_type(l3_type, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_l3_enable_l4_type(ctc_parser_l4_type_t l4_type, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_l3_enable_l4_type ? ctc_api->ctc_parser_l3_enable_l4_type(l4_type, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_remove_l2_type(uint8 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_remove_l2_type ? ctc_api->ctc_parser_remove_l2_type(index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_remove_l3_type(uint8 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_remove_l3_type ? ctc_api->ctc_parser_remove_l3_type(index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_ip_hash_ctl ? ctc_api->ctc_parser_set_ip_hash_ctl(hash_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_ipv6_ctl ? ctc_api->ctc_parser_set_ipv6_ctl(index, ipv6_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_l2flex_ctl ? ctc_api->ctc_parser_set_l2flex_ctl(index, l2flex_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_l2_hash ? ctc_api->ctc_parser_set_l2_hash(l2ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_l3flex_ctl ? ctc_api->ctc_parser_set_l3flex_ctl(index, l3flex_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_layer4_parser_ctl ? ctc_api->ctc_parser_set_layer4_parser_ctl(tbl_flags, l4_parser_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_layer4_type(uint8 index, ctc_parser_l4_ptl_entry_t* entry)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_layer4_type ? ctc_api->ctc_parser_set_layer4_type(index, entry) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_max_length_filed(uint16 max_length)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_max_length_filed ? ctc_api->ctc_parser_set_max_length_filed(max_length) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_mpls_ctl ? ctc_api->ctc_parser_set_mpls_ctl(mpls_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_parser_quad_vlan_en(uint8 enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_parser_quad_vlan_en ? ctc_api->ctc_parser_set_parser_quad_vlan_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_pbb_parser_ctl ? ctc_api->ctc_parser_set_pbb_parser_ctl(pbb_parser_ctl) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_snap_allow_none_zero_oui(uint8 enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_snap_allow_none_zero_oui ? ctc_api->ctc_parser_set_snap_allow_none_zero_oui(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_tpid(ctc_parser_l2_tpid_t type, uint16 tpid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_tpid ? ctc_api->ctc_parser_set_tpid(type, tpid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_parser_set_vlan_parser_num(uint8 vlan_num)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_parser_set_vlan_parser_num ? ctc_api->ctc_parser_set_vlan_parser_num(vlan_num) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##pdu##*/
int32 ctc_l2pdu_classify_l2pdu(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index, ctc_pdu_l2pdu_key_t* key)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_classify_l2pdu ? ctc_api->ctc_l2pdu_classify_l2pdu(l2pdu_type, index, key) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2pdu_get_classified_key(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index, ctc_pdu_l2pdu_key_t* key)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_get_classified_key ? ctc_api->ctc_l2pdu_get_classified_key(l2pdu_type, index, key) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2pdu_get_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index, ctc_pdu_global_l2pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_get_global_action ? ctc_api->ctc_l2pdu_get_global_action(l2pdu_type, index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2pdu_get_port_action(uint16 gport, uint8 action_index, ctc_pdu_port_l2pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_get_port_action ? ctc_api->ctc_l2pdu_get_port_action(gport, action_index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2pdu_set_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index, ctc_pdu_global_l2pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_set_global_action ? ctc_api->ctc_l2pdu_set_global_action(l2pdu_type, index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l2pdu_set_port_action(uint16 gport, uint8 action_index, ctc_pdu_port_l2pdu_action_t action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l2pdu_set_port_action ? ctc_api->ctc_l2pdu_set_port_action(gport, action_index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_classify_l3pdu(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index, ctc_pdu_l3pdu_key_t* key)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_classify_l3pdu ? ctc_api->ctc_l3pdu_classify_l3pdu(l3pdu_type, index, key) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_get_classified_key(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index, ctc_pdu_l3pdu_key_t* key)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_get_classified_key ? ctc_api->ctc_l3pdu_get_classified_key(l3pdu_type, index, key) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_get_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index, ctc_pdu_global_l3pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_get_global_action ? ctc_api->ctc_l3pdu_get_global_action(l3pdu_type, index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_get_l3if_action(uint16 l3ifid, uint8 action_index, ctc_pdu_l3if_l3pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_get_l3if_action ? ctc_api->ctc_l3pdu_get_l3if_action(l3ifid, action_index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_set_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index, ctc_pdu_global_l3pdu_action_t* action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_set_global_action ? ctc_api->ctc_l3pdu_set_global_action(l3pdu_type, index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_l3pdu_set_l3if_action(uint16 l3ifid, uint8 action_index, ctc_pdu_l3if_l3pdu_action_t action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_l3pdu_set_l3if_action ? ctc_api->ctc_l3pdu_set_l3if_action(l3ifid, action_index, action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_pdu_init(void* pdu_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_pdu_init ? ctc_api->ctc_pdu_init(pdu_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##port##*/
int32 ctc_get_cpu_mac_en(bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_cpu_mac_en ? ctc_api->ctc_get_cpu_mac_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_max_frame_size(ctc_frame_size_t index, uint16* value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_max_frame_size ? ctc_api->ctc_get_max_frame_size(index, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_bridge_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_bridge_en ? ctc_api->ctc_port_get_bridge_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_cross_connet(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_cross_connet ? ctc_api->ctc_port_get_cross_connet(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_default_vlan(uint16 gport, uint16* vid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_default_vlan ? ctc_api->ctc_port_get_default_vlan(gport, vid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_dot1q_type(uint16 gport, ctc_dot1q_type_t* type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_dot1q_type ? ctc_api->ctc_port_get_dot1q_type(gport, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_flow_ctl_en ? ctc_api->ctc_port_get_flow_ctl_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_ipsg_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_ipsg_en ? ctc_api->ctc_port_get_ipsg_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_keep_vlan_tag(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_keep_vlan_tag ? ctc_api->ctc_port_get_keep_vlan_tag(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_learning_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_learning_en ? ctc_api->ctc_port_get_learning_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_mac_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_mac_en ? ctc_api->ctc_port_get_mac_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_max_frame(uint16 gport, ctc_frame_size_t* index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_max_frame ? ctc_api->ctc_port_get_max_frame(gport, index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_mcast_flooding_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_mcast_flooding_en ? ctc_api->ctc_port_get_mcast_flooding_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_min_frame_size(uint16 gport, uint8* size)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_min_frame_size ? ctc_api->ctc_port_get_min_frame_size(gport, size) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_outer_is_cvlan(uint16 gport, bool* is_cvlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_outer_is_cvlan ? ctc_api->ctc_port_get_outer_is_cvlan(gport, is_cvlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_pading_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_pading_en ? ctc_api->ctc_port_get_pading_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_phy_if_en(uint16 gport,uint16* l3if_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_phy_if_en ? ctc_api->ctc_port_get_phy_if_en(gport, l3if_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_port_check_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_port_check_en ? ctc_api->ctc_port_get_port_check_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_preamble(uint16 gport, uint8* pre_bytes)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_preamble ? ctc_api->ctc_port_get_preamble(gport, pre_bytes) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_pro_vlan_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_pro_vlan_en ? ctc_api->ctc_port_get_pro_vlan_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_random_log_en(uint16 gport, ctc_direction_t dir, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_random_log_en ? ctc_api->ctc_port_get_random_log_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_random_threshold(uint16 gport, ctc_direction_t dir, uint16* threshold)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_random_threshold ? ctc_api->ctc_port_get_random_threshold(gport, dir, threshold) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_receive_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_receive_en ? ctc_api->ctc_port_get_receive_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_reflective_bridge_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_reflective_bridge_en ? ctc_api->ctc_port_get_reflective_bridge_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_speed(uint16 gport, ctc_port_speed_t* speed_mode)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_speed ? ctc_api->ctc_port_get_speed(gport, speed_mode) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_srcdiscard_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_srcdiscard_en ? ctc_api->ctc_port_get_srcdiscard_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_src_outer_is_svlan(uint16 gport, bool* is_svlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_src_outer_is_svlan ? ctc_api->ctc_port_get_src_outer_is_svlan(gport, is_svlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8* index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_stag_tpid_index ? ctc_api->ctc_port_get_stag_tpid_index(gport, dir, index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_stretch_mode_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_stretch_mode_en ? ctc_api->ctc_port_get_stretch_mode_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_sub_if_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_sub_if_en ? ctc_api->ctc_port_get_sub_if_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_transmit_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_transmit_en ? ctc_api->ctc_port_get_transmit_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_ucast_flooding_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_ucast_flooding_en ? ctc_api->ctc_port_get_ucast_flooding_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_untag_dft_vid(uint16 gport, bool* enable, bool* untag_svlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_untag_dft_vid ? ctc_api->ctc_port_get_untag_dft_vid(gport, enable, untag_svlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_use_inner_cos(uint16 gport, bool* is_inner)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_use_inner_cos ? ctc_api->ctc_port_get_use_inner_cos(gport, is_inner) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_use_outer_ttl(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_use_outer_ttl ? ctc_api->ctc_port_get_use_outer_ttl(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t* type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_vlan_classify_enable ? ctc_api->ctc_port_get_vlan_classify_enable(gport, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_vlanctl(uint16 gport, ctc_vlantag_ctl_t* mode)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_vlanctl ? ctc_api->ctc_port_get_vlanctl(gport, mode) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_vlan_filter_en ? ctc_api->ctc_port_get_vlan_filter_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_vlan_mapping_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_vlan_mapping_en ? ctc_api->ctc_port_get_vlan_mapping_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_get_vlan_switching_en(uint16 gport, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_get_vlan_switching_en ? ctc_api->ctc_port_get_vlan_switching_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_init(void* port_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_init ? ctc_api->ctc_port_init(port_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_bridge_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_bridge_en ? ctc_api->ctc_port_set_bridge_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_cross_connet(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_cross_connet ? ctc_api->ctc_port_set_cross_connet(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_default_vlan(uint16 gport, uint16 vid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_default_vlan ? ctc_api->ctc_port_set_default_vlan(gport, vid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_dot1q_type(uint16 gport, ctc_dot1q_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_dot1q_type ? ctc_api->ctc_port_set_dot1q_type(gport, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_flow_ctl_en ? ctc_api->ctc_port_set_flow_ctl_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_ipsg_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_ipsg_en ? ctc_api->ctc_port_set_ipsg_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_keep_vlan_tag(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_keep_vlan_tag ? ctc_api->ctc_port_set_keep_vlan_tag(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_learning_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_learning_en ? ctc_api->ctc_port_set_learning_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_loopback(ctc_port_lbk_param_t* p_port_lbk)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_loopback ? ctc_api->ctc_port_set_loopback(p_port_lbk) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_mac_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_mac_en ? ctc_api->ctc_port_set_mac_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_max_frame(uint16 gport, ctc_frame_size_t index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_max_frame ? ctc_api->ctc_port_set_max_frame(gport, index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_mcast_flooding_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_mcast_flooding_en ? ctc_api->ctc_port_set_mcast_flooding_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_min_frame_size(uint16 gport, uint8 size)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_min_frame_size ? ctc_api->ctc_port_set_min_frame_size(gport, size) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_outer_is_cvlan(uint16 gport, bool is_cvlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_outer_is_cvlan ? ctc_api->ctc_port_set_outer_is_cvlan(gport, is_cvlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_pading_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_pading_en ? ctc_api->ctc_port_set_pading_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_phy_if_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_phy_if_en ? ctc_api->ctc_port_set_phy_if_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_port_check_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_port_check_en ? ctc_api->ctc_port_set_port_check_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_preamble(uint16 gport, uint8 pre_bytes)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_preamble ? ctc_api->ctc_port_set_preamble(gport, pre_bytes) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_pro_vlan_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_pro_vlan_en ? ctc_api->ctc_port_set_pro_vlan_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_random_log_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_random_log_en ? ctc_api->ctc_port_set_random_log_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_random_threshold(uint16 gport, ctc_direction_t dir, uint16 threshold)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_random_threshold ? ctc_api->ctc_port_set_random_threshold(gport, dir, threshold) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_receive_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_receive_en ? ctc_api->ctc_port_set_receive_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_reflective_bridge_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_reflective_bridge_en ? ctc_api->ctc_port_set_reflective_bridge_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_speed(uint16 gport, ctc_port_speed_t speed_mode)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_speed ? ctc_api->ctc_port_set_speed(gport, speed_mode) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_srcdiscard_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_srcdiscard_en ? ctc_api->ctc_port_set_srcdiscard_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_src_outer_is_svlan(uint16 gport, bool is_svlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_src_outer_is_svlan ? ctc_api->ctc_port_set_src_outer_is_svlan(gport, is_svlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 index)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_stag_tpid_index ? ctc_api->ctc_port_set_stag_tpid_index(gport, dir, index) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_stretch_mode_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_stretch_mode_en ? ctc_api->ctc_port_set_stretch_mode_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_sub_if_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_sub_if_en ? ctc_api->ctc_port_set_sub_if_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_transmit_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_transmit_en ? ctc_api->ctc_port_set_transmit_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_ucast_flooding_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_ucast_flooding_en ? ctc_api->ctc_port_set_ucast_flooding_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_untag_dft_vid(uint16 gport, bool enable, bool untag_svlan)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_untag_dft_vid ? ctc_api->ctc_port_set_untag_dft_vid(gport, enable, untag_svlan) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_use_inner_cos(uint16 gport, bool is_inner)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_use_inner_cos ? ctc_api->ctc_port_set_use_inner_cos(gport, is_inner) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_use_outer_ttl(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_use_outer_ttl ? ctc_api->ctc_port_set_use_outer_ttl(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlan_classify_disable(uint16 gport, ctc_vlan_class_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlan_classify_disable ? ctc_api->ctc_port_set_vlan_classify_disable(gport, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlan_classify_enable ? ctc_api->ctc_port_set_vlan_classify_enable(gport, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlanctl(uint16 gport, ctc_vlantag_ctl_t mode)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlanctl ? ctc_api->ctc_port_set_vlanctl(gport, mode) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlan_filter_en ? ctc_api->ctc_port_set_vlan_filter_en(gport, dir, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlan_mapping_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlan_mapping_en ? ctc_api->ctc_port_set_vlan_mapping_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_port_set_vlan_switching_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_port_set_vlan_switching_en ? ctc_api->ctc_port_set_vlan_switching_en(gport, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_cpu_mac_en(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_cpu_mac_en ? ctc_api->ctc_set_cpu_mac_en(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_max_frame_size(ctc_frame_size_t index, uint16 value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_max_frame_size ? ctc_api->ctc_set_max_frame_size(index, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##queue##*/
int32 ctc_channel_shape_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_channel_shape_global_enable ? ctc_api->ctc_channel_shape_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_create_service(uint16 service_id, uint8 dest_port)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_create_service ? ctc_api->ctc_create_service(service_id, dest_port) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_channel_shape_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_channel_shape_global_enable ? ctc_api->ctc_get_channel_shape_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_group_shape ? ctc_api->ctc_get_group_shape(group_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_group_shape_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_group_shape_global_enable ? ctc_api->ctc_get_group_shape_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_per_service_queue_num(uint8* p_que_num)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_per_service_queue_num ? ctc_api->ctc_get_per_service_queue_num(p_que_num) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_port_queue_class(uint16 gport, uint8 qid, uint8* p_class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_class ? ctc_api->ctc_get_port_queue_class(gport, qid, p_class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_drop ? ctc_api->ctc_get_port_queue_drop(gport, qid, p_drop) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_shape ? ctc_api->ctc_get_port_queue_shape(gport, qid, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_get_port_queue_shape_profile_num(uint32* p_shape_num)
 {
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_shape_profile_num ? ctc_api->ctc_get_port_queue_shape_profile_num(p_shape_num) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_get_port_queue_stats(uint16 gport, uint8 qid, ctc_queue_stats_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_stats ? ctc_api->ctc_get_port_queue_stats(gport, qid, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16* p_weight)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_queue_wdrr_weight ? ctc_api->ctc_get_port_queue_wdrr_weight(gport, qid, p_weight) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_port_shape ? ctc_api->ctc_get_port_shape(gport, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_queue_shape_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_queue_shape_global_enable ? ctc_api->ctc_get_queue_shape_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_queue_stats_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_queue_stats_enable ? ctc_api->ctc_get_queue_stats_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_class(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint8* p_class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_class ? ctc_api->ctc_get_service_queue_class(type, service_id, qid, p_class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_drop(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_drop ? ctc_api->ctc_get_service_queue_drop(type, service_id, qid, p_drop) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_enable ? ctc_api->ctc_get_service_queue_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_shape ? ctc_api->ctc_get_service_queue_shape(type, service_id, qid, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_stats(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_stats_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_stats ? ctc_api->ctc_get_service_queue_stats(type, service_id, qid, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_get_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint16* p_weight)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_get_service_queue_wdrr_weight ? ctc_api->ctc_get_service_queue_wdrr_weight(type, service_id, qid, p_weight) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_group_shape_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_group_shape_global_enable ? ctc_api->ctc_group_shape_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_flow_id_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_flow_id_global_enable ? ctc_api->ctc_qos_flow_id_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_qos_get_flow_id_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_qos_get_flow_id_global_enable ? ctc_api->ctc_qos_get_flow_id_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_get_resrc_mgr_global_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_get_resrc_mgr_global_enable ? ctc_api->ctc_queue_get_resrc_mgr_global_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_init(ctc_queue_global_cfg_t* queue_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_init ? ctc_api->ctc_queue_init(queue_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_resrc_mgr_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_resrc_mgr_global_enable ? ctc_api->ctc_queue_resrc_mgr_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_shape_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_shape_global_enable ? ctc_api->ctc_queue_shape_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_shape_ipg_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_shape_ipg_global_enable ? ctc_api->ctc_queue_shape_ipg_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_queue_stats_global_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_queue_stats_global_enable ? ctc_api->ctc_queue_stats_global_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_remove_service(uint16 service_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_remove_service ? ctc_api->ctc_remove_service(service_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_reset_port_queue_stats(uint16 gport, uint8 qid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_reset_port_queue_stats ? ctc_api->ctc_reset_port_queue_stats(gport, qid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_reset_service_queue_stats(ctc_queue_type_t type, uint16 service_id, uint8 qid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_reset_service_queue_stats ? ctc_api->ctc_reset_service_queue_stats(type, service_id, qid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_group_shape ? ctc_api->ctc_set_group_shape(group_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_port_queue_class(uint16 gport, uint8 qid, uint8 class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_port_queue_class ? ctc_api->ctc_set_port_queue_class(gport, qid, class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_port_queue_drop ? ctc_api->ctc_set_port_queue_drop(gport, qid, p_drop) : CTC_E_NOT_SUPPORT;
    return ret;
}

int32 ctc_set_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_port_queue_shape ? ctc_api->ctc_set_port_queue_shape(gport, qid, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16 weight)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_port_queue_wdrr_weight ? ctc_api->ctc_set_port_queue_wdrr_weight(gport, qid, weight) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_port_shape ? ctc_api->ctc_set_port_shape(gport, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_queue_priority_map(ctc_queue_pri_map_t* p_queue_pri_map)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_queue_priority_map ? ctc_api->ctc_set_queue_priority_map(p_queue_pri_map) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_queue_size_mode(uint8 size_mode)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_queue_size_mode ? ctc_api->ctc_set_queue_size_mode(size_mode) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_queue_wdrr_weight_mtu(uint32 mtu)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_queue_wdrr_weight_mtu ? ctc_api->ctc_set_queue_wdrr_weight_mtu(mtu) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_service_queue_class(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint8 class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_service_queue_class ? ctc_api->ctc_set_service_queue_class(type, service_id, qid, class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_service_queue_drop(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_service_queue_drop ? ctc_api->ctc_set_service_queue_drop(type, service_id, qid, p_drop) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_service_queue_shape ? ctc_api->ctc_set_service_queue_shape(type, service_id, qid, p_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_set_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint16 weight)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_set_service_queue_wdrr_weight ? ctc_api->ctc_set_service_queue_wdrr_weight(type, service_id, qid, weight) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_unset_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_unset_group_shape ? ctc_api->ctc_unset_group_shape(group_shape) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_unset_port_queue_shape(uint16 gport, uint8 qid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_unset_port_queue_shape ? ctc_api->ctc_unset_port_queue_shape(gport, qid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_unset_port_shape(uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_unset_port_shape ? ctc_api->ctc_unset_port_shape(gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_unset_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_unset_service_queue_shape ? ctc_api->ctc_unset_service_queue_shape(type, service_id, qid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##register##*/
int32 ctc_global_ctl_get(ctc_global_control_type_t type, void* value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_global_ctl_get ? ctc_api->ctc_global_ctl_get(type, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_global_ctl_set(ctc_global_control_type_t type, void* value)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_global_ctl_set ? ctc_api->ctc_global_ctl_set(type, value) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_register_init(void)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_register_init ? ctc_api->ctc_register_init() : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##stats##*/
int32 ctc_stats_get_drop_packet_stats_enable(bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_drop_packet_stats_enable ? ctc_api->ctc_stats_get_drop_packet_stats_enable(p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_fwd_stats_enable(ctc_stats_fwd_type_t fwd_stats_type, bool* p_enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_fwd_stats_enable ? ctc_api->ctc_stats_get_fwd_stats_enable(fwd_stats_type, p_enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t* p_stats_prop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_global_cfg ? ctc_api->ctc_stats_get_global_cfg(stats_param, p_stats_prop) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_global_fwd_stats(ctc_direction_t dir, ctc_stats_global_fwd_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_global_fwd_stats ? ctc_api->ctc_stats_get_global_fwd_stats(dir, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_log_option(uint16 gport,ctc_direction_t dir, ctc_stats_basic_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_log_option ? ctc_api->ctc_stats_get_log_option(gport, dir, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t* p_prop_data)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_mac_stats_cfg ? ctc_api->ctc_stats_get_mac_stats_cfg(gport, mac_stats_prop_type, p_prop_data) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir, ctc_mac_stats_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_mac_stats ? ctc_api->ctc_stats_get_mac_stats(gport, dir, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t* p_type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_port_stats_cfg ? ctc_api->ctc_stats_get_port_stats_cfg(dir, p_type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_get_port_stats(uint16 gport, ctc_direction_t dir, ctc_stats_port_t* p_stats)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_get_port_stats ? ctc_api->ctc_stats_get_port_stats(gport, dir, p_stats) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_init(void* stats_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_init ? ctc_api->ctc_stats_init(stats_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_intr_callback_func(uint8* gchip)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_intr_callback_func ? ctc_api->ctc_stats_intr_callback_func(gchip) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_reset_global_fwd_stats(ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_reset_global_fwd_stats ? ctc_api->ctc_stats_reset_global_fwd_stats(dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_reset_log_option(uint16 gport,ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_reset_log_option ? ctc_api->ctc_stats_reset_log_option(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_reset_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_reset_mac_stats ? ctc_api->ctc_stats_reset_mac_stats(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_reset_port_stats(uint16 gport, ctc_direction_t dir)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_reset_port_stats ? ctc_api->ctc_stats_reset_port_stats(gport, dir) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_set_drop_packet_stats_enable(bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_set_drop_packet_stats_enable ? ctc_api->ctc_stats_set_drop_packet_stats_enable(enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_set_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t stats_prop)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_set_global_cfg ? ctc_api->ctc_stats_set_global_cfg(stats_param, stats_prop) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_set_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t prop_data)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_set_mac_stats_cfg ? ctc_api->ctc_stats_set_mac_stats_cfg(gport, mac_stats_prop_type, prop_data) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stats_set_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stats_set_port_stats_cfg ? ctc_api->ctc_stats_set_port_stats_cfg(dir, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##stp##*/
int32 ctc_stp_clear_all_inst_state(uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stp_clear_all_inst_state ? ctc_api->ctc_stp_clear_all_inst_state(gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stp_get_state(uint16 gport,uint8 stpid, uint8* state)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stp_get_state ? ctc_api->ctc_stp_get_state(gport, stpid, state) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stp_get_vlan_stpid(uint16 vlan_id, uint8* stpid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stp_get_vlan_stpid ? ctc_api->ctc_stp_get_vlan_stpid(vlan_id, stpid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stp_set_state(uint16 gport, uint8 stpid, uint8 state)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stp_set_state ? ctc_api->ctc_stp_set_state(gport, stpid, state) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_stp_set_vlan_stpid(uint16 vlan_id, uint8 stpid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_stp_set_vlan_stpid ? ctc_api->ctc_stp_set_vlan_stpid(vlan_id, stpid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##usrid##*/
int32 ctc_usrid_init(void* usrid_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_usrid_init ? ctc_api->ctc_usrid_init(usrid_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
/*##vlan##*/
int32 ctc_vlan_add_port(uint16 vlan_id, uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_add_port ? ctc_api->ctc_vlan_add_port(vlan_id, gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_add_vlan_classification_default_entry(ctc_vlan_class_type_t type, ctc_vlan_miss_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_add_vlan_classification_default_entry ? ctc_api->ctc_vlan_add_vlan_classification_default_entry(type, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_add_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_add_vlan_classification_entry ? ctc_api->ctc_vlan_add_vlan_classification_entry(p_vlan_class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_add_vlan_mapping_default_entry(uint16 gport, ctc_vlan_miss_t* p_action)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_add_vlan_mapping_default_entry ? ctc_api->ctc_vlan_add_vlan_mapping_default_entry(gport, p_action) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_add_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_add_vlan_mapping_entry ? ctc_api->ctc_vlan_add_vlan_mapping_entry(gport, p_vlan_mapping) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_create_vlan(uint16 vlan_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_create_vlan ? ctc_api->ctc_vlan_create_vlan(vlan_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_flush_vlan_classification_entry(ctc_vlan_class_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_flush_vlan_classification_entry ? ctc_api->ctc_vlan_flush_vlan_classification_entry(type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_arp_excp_type(uint16 vlan_id, ctc_exception_type_t* type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_arp_excp_type ? ctc_api->ctc_vlan_get_arp_excp_type(vlan_id, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_bridge_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_bridge_en ? ctc_api->ctc_vlan_get_bridge_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t* type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_dhcp_excp_type ? ctc_api->ctc_vlan_get_dhcp_excp_type(vlan_id, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_fid(uint16 vlan_id, uint16* fid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_fid ? ctc_api->ctc_vlan_get_fid(vlan_id, fid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_igmp_snoop_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_igmp_snoop_en ? ctc_api->ctc_vlan_get_igmp_snoop_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_learning_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_learning_en ? ctc_api->ctc_vlan_get_learning_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_ports(uint16 vlan_id, ctc_port_bitmap_t* port_bitmap)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_ports ? ctc_api->ctc_vlan_get_ports(vlan_id, port_bitmap) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_receive_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_receive_en ? ctc_api->ctc_vlan_get_receive_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_replace_dspc_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_replace_dspc_en ? ctc_api->ctc_vlan_get_replace_dspc_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_get_transmit_en(uint16 vlan_id, bool* enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_get_transmit_en ? ctc_api->ctc_vlan_get_transmit_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_init(ctc_vlan_global_cfg_t* vlan_global_cfg)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_init ? ctc_api->ctc_vlan_init(vlan_global_cfg) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_port(uint16 vlan_id, uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_port ? ctc_api->ctc_vlan_remove_port(vlan_id, gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan_classification_default_entry(ctc_vlan_class_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan_classification_default_entry ? ctc_api->ctc_vlan_remove_vlan_classification_default_entry(type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan_classification_entry ? ctc_api->ctc_vlan_remove_vlan_classification_entry(p_vlan_class) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan_mapping_all_by_port(uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan_mapping_all_by_port ? ctc_api->ctc_vlan_remove_vlan_mapping_all_by_port(gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan_mapping_default_entry(uint16 gport)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan_mapping_default_entry ? ctc_api->ctc_vlan_remove_vlan_mapping_default_entry(gport) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan_mapping_entry ? ctc_api->ctc_vlan_remove_vlan_mapping_entry(gport, p_vlan_mapping) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_remove_vlan(uint16 vlan_id)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_remove_vlan ? ctc_api->ctc_vlan_remove_vlan(vlan_id) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_arp_excp_type(uint16 vlan_id, ctc_exception_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_arp_excp_type ? ctc_api->ctc_vlan_set_arp_excp_type(vlan_id, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_bridge_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_bridge_en ? ctc_api->ctc_vlan_set_bridge_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t type)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_dhcp_excp_type ? ctc_api->ctc_vlan_set_dhcp_excp_type(vlan_id, type) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_fid(uint16 vlan_id, uint16 fid)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_fid ? ctc_api->ctc_vlan_set_fid(vlan_id, fid) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_igmp_snoop_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_igmp_snoop_en ? ctc_api->ctc_vlan_set_igmp_snoop_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_learning_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_learning_en ? ctc_api->ctc_vlan_set_learning_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_receive_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_receive_en ? ctc_api->ctc_vlan_set_receive_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_replace_dscp_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_replace_dscp_en ? ctc_api->ctc_vlan_set_replace_dscp_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
int32 ctc_vlan_set_transmit_en(uint16 vlan_id, bool enable)
{
    int32 ret = CTC_E_NOT_SUPPORT;
    ret = ctc_api->ctc_vlan_set_transmit_en ? ctc_api->ctc_vlan_set_transmit_en(vlan_id, enable) : CTC_E_NOT_SUPPORT;
    return ret;
}
 
 
 
