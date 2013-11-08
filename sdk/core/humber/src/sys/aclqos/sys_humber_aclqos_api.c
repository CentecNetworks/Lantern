/**
 @file sys_humber_aclqos_api.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_stats.h"
#include "ctc_aclqos.h"

#include "sys_humber_aclqos_label.h"
#include "sys_humber_qos_policer.h"
#include "sys_humber_qos_class.h"
#include "sys_humber_aclqos_entry.h"
#include "sys_humber_port.h"

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
uint8 aclqos_init = 0;

#define SYS_ACLQOS_INIT_CHECK() \
    {\
        if(  aclqos_init == 0)\
          return CTC_E_NOT_INIT;\
    }


/****************************************************************************
  *
  * Function
  *
  ****************************************************************************/

/**
 @brief Acl/QoS look up enable/disable on the given port
*/
int32
sys_humber_aclqos_port_enable(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_port_lkp_enable(gport, dir, label_type, enable));

    return CTC_E_NONE;
}

/**
 @brief Acl/QoS look up enable/disable on the given vlan
*/
int32
sys_humber_aclqos_vlan_enable(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_vlan_lkp_enable(vid, dir, label_type, enable));

    return CTC_E_NONE;
}

/**
 @brief Acl pbr look up enable/disable on the given l3 interface
*/
int32
sys_humber_acl_pbr_enable(uint16 l3if_id, bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_pbr_lkp_enable(l3if_id, enable));

    return CTC_E_NONE;
}

/**
 @brief create acl/qos port label
*/
int32
sys_humber_aclqos_port_label_create(ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(label_type, label_id));

    return CTC_E_NONE;
}


/**
 @brief create acl/qos vlan label
*/
int32
sys_humber_aclqos_vlan_label_create(ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_vlan_label_new(label_type, label_id));

    return CTC_E_NONE;
}


/**
 @brief create acl/qos vlan label
*/
int32
sys_humber_acl_pbr_label_create(uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_new(label_id));

    return CTC_E_NONE;
}


/**
 @brief create acl/qos service label
*/
int32
sys_humber_aclqos_service_label_create(uint16 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_service_label_new(label_id));

    return CTC_E_NONE;
}


/**
 @brief remove acl/qos port label
*/
int32
sys_humber_aclqos_port_label_remove( ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_delete(label_type, label_id));

    return CTC_E_NONE;
}


/**
 @brief remove acl/qos vlan label
*/
int32
sys_humber_aclqos_vlan_label_remove(ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_vlan_label_delete(label_type, label_id));

    return CTC_E_NONE;
}


/**
 @brief remove acl pbr label
*/
int32
sys_humber_acl_pbr_label_remove(uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_delete(label_id));

    return CTC_E_NONE;
}


/**
 @brief remove acl/qos service label
*/
int32
sys_humber_aclqos_service_label_remove(uint16 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_service_label_delete(label_id));

    return CTC_E_NONE;
}


/**
 @brief set acl/qos label to the given port
*/
int32
sys_humber_aclqos_port_label_set(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_apply(gport, dir, label_type, label_id));

    return CTC_E_NONE;
}

/**
 @brief set acl/qos label to the given vlan
*/
int32
sys_humber_aclqos_vlan_label_set(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_vlan_label_apply(vid, dir, label_type, label_id));

    return CTC_E_NONE;
}

/**
 @brief set acl pbr label to the given l3 interface
*/
int32
sys_humber_acl_pbr_label_set(uint16 l3if_id, uint32 label_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_apply(l3if_id, label_id));

    return CTC_E_NONE;
}

/**
 @brief unset acl/qos label to the given port
*/
int32
sys_humber_aclqos_port_label_unset(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_unapply(gport, dir, label_type));

    return CTC_E_NONE;
}

/**
 @brief unset acl/qos label to the given vlan
*/
int32
sys_humber_aclqos_vlan_label_unset(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_vlan_label_unapply(vid, dir, label_type));

    return CTC_E_NONE;
}

/**
 @brief unset acl pbr label to the given l3 interface
*/
int32
sys_humber_acl_pbr_label_unset(uint16 l3if_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_unapply(l3if_id));

    return CTC_E_NONE;
}

/**
 @brief set l2 acl high priority
*/
int32
sys_humber_acl_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_set_l2acl_prio(gport, dir, l2_high_prio));

    return CTC_E_NONE;
}


/**
 @brief set l2 qos high priority
*/
int32
sys_humber_qos_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_set_l2qos_prio(gport, dir, l2_high_prio));

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS policer.
*/
int32
sys_humber_qos_policer_global_enable(bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_policer_update_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get QoS policer global enable status.
*/
int32
sys_humber_qos_get_policer_global_enable(bool* p_enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_policer_update_enable(p_enable));

    return CTC_E_NONE;
}

int32
sys_humber_qos_set_sequential_policer(bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_policer_sequential_enable(enable));

    return CTC_E_NONE;
}

int32
sys_humber_qos_set_policer_ipg_global_enable(bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_policer_ipg_enable(enable));

    return CTC_E_NONE;
}

int32
sys_humber_qos_get_service_policer_num(uint32* service_policer_num)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_PTR_VALID_CHECK(service_policer_num);
    CTC_ERROR_RETURN(sys_humber_qos_policer_get_service_policer_num(service_policer_num));

    return CTC_E_NONE;
}

int32
sys_humber_qos_set_flow_policer_first_en(ctc_direction_t dir, bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_flow_policer_first_enable(dir, enable));

    return CTC_E_NONE;
}


/**
 @brief create qos flow policer
*/
int32
sys_humber_qos_flow_policer_create(ctc_qos_policer_t* p_policer, uint32 plc_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_flow_policer_new(p_policer, plc_id));

    return CTC_E_NONE;
}


/**
 @brief remove qos flow policer
*/
int32
sys_humber_qos_flow_policer_remove(uint32 plc_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_flow_policer_delete(plc_id));

    return CTC_E_NONE;
}


/**
 @brief update qos flow policer, replace old policer with new policer
*/
int32
sys_humber_qos_flow_policer_update(uint32 plc_id, ctc_qos_policer_t* p_policer)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_flow_policer_refresh(plc_id, p_policer));

    return CTC_E_NONE;
}


/**
 @brief Get real flow policer data added to chip.
*/
int32
sys_humber_qos_get_flow_policer(uint32 plc_id, ctc_qos_policer_t* p_policer)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_flow_policer_data(plc_id, p_policer));

    return CTC_E_NONE;
}


/**
 @brief set port policer
*/
int32
sys_humber_qos_set_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_port_policer_bind(gport, dir, phb_offset, p_policer));

    CTC_ERROR_RETURN(sys_humber_port_set_port_policer_valid(gport, dir, TRUE));

    return CTC_E_NONE;
}

/**
 @brief unset port policer
*/
int32
sys_humber_qos_unset_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_set_port_policer_valid(gport, dir, FALSE));

    CTC_ERROR_RETURN(sys_humber_qos_port_policer_unbind(gport, dir, phb_offset));

    return CTC_E_NONE;
}

/**
 @brief set service policer
*/
int32
sys_humber_qos_set_service_policer(uint16 service_id, ctc_qos_policer_t* p_policer)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_service_policer_bind(service_id, p_policer));

    return CTC_E_NONE;
}

/**
 @brief unset service policer
*/
int32
sys_humber_qos_unset_service_policer(uint16 service_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_service_policer_unbind(service_id));

    return CTC_E_NONE;
}


/**
 @brief Get real port policer data added to chip.
*/
int32
sys_humber_qos_get_port_policer(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_port_policer_data(gport, dir, phb_offset, p_policer));

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS policer statistics globally.
*/
int32
sys_humber_qos_policer_stats_global_enable(bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_policer_stats_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get QoS policer statistics enable status.
*/
int32
sys_humber_qos_get_policer_stats_global_enable(bool* p_enable)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_policer_stats_enable(p_enable));

    return CTC_E_NONE;
}

/**
 @brief Get flow policer statistics.
*/
int32
sys_humber_qos_get_flow_policer_stats(uint32 plc_id, ctc_qos_policer_stats_t* p_stats)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_flow_policer_stats_result(plc_id, p_stats));

    return CTC_E_NONE;
}

/**
 @brief Clear flow policer statistics.
*/
int32
sys_humber_qos_reset_flow_policer_stats(uint32 plc_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_clear_flow_policer_stats_result(plc_id));

    return CTC_E_NONE;
}


/**
 @brief Get port policer statistics.
*/
int32
sys_humber_qos_get_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_get_port_policer_stats_result(gport, dir, phb_offset, p_stats));

    return CTC_E_NONE;
}

/**
 @brief Clear port policer statistics.
*/
int32
sys_humber_qos_reset_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_clear_port_policer_stats_result(gport, dir, phb_offset));

    return CTC_E_NONE;
}


/**
 @brief set ingress cos -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_cos_priority_color_map(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, uint8 color)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_igs_cos_map_table(domain, cos, cfi, priority, color));

    return CTC_E_NONE;
}


/**
 @brief set ingress dscp -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_dscp_priority_color_map(uint8 domain, uint8 dscp, uint8 priority, uint8 color)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_igs_dscp_map_table(domain, dscp, priority, color));

    return CTC_E_NONE;
}


/**
 @brief set ingress ip precedence -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_ip_prec_priority_color_map(uint8 domain, uint8 ip_prec, uint8 priority, uint8 color)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_igs_ip_prec_map_table(domain, ip_prec, priority, color));

    return CTC_E_NONE;
}


/**
 @brief set ingress mpls exp -> priority + color mapping table for the given domain
*/
int32
sys_humber_qos_set_exp_priority_color_map(uint8 domain, uint8 exp, uint8 priority, uint8 color)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_igs_exp_map_table(domain, exp, priority, color));

    return CTC_E_NONE;
}


/**
 @brief set egress priority + color -> cos mapping table for the given domain
*/
int32
sys_humber_qos_set_priority_color_cos_map(uint8 domain, uint8 priority, uint8 color, uint8 cos, uint8 cfi)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_egs_cos_map_table(domain, priority, color, cos, cfi));

    return CTC_E_NONE;
}


/**
 @brief set egress priority + color -> dscp mapping table for the given domain
*/
int32
sys_humber_qos_set_priority_color_dscp_map(uint8 domain, uint8 priority, uint8 color, uint8 dscp)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_egs_dscp_map_table(domain, priority, color, dscp));

    return CTC_E_NONE;
}


/**
 @brief set egress priority + color -> mpls exp mapping table for the given domain
*/
int32
sys_humber_qos_set_priority_color_exp_map(uint8 domain, uint8 priority, uint8 color, uint8 exp)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_set_egs_exp_map_table(domain, priority, color, exp));

    return CTC_E_NONE;
}

/**
 @brief set domain default value
*/
int32
sys_humber_qos_set_default_domain(void)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_class_init());

    return CTC_E_NONE;
}

/**
 @brief set qos domain for the given port
*/
int32
sys_humber_qos_set_port_domain(uint16 gport, ctc_direction_t dir, uint8 domain)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_set_qos_domain(gport, dir, domain));

    return CTC_E_NONE;
}


/**
 @brief get qos domain for the given port
*/
int32
sys_humber_qos_get_port_domain(uint16 gport, ctc_direction_t dir, uint8* p_domain)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_get_qos_domain(gport, dir, p_domain));

    return CTC_E_NONE;
}


/**
 @brief set trust state for the given port
*/
int32
sys_humber_qos_set_port_trust(uint16 gport, ctc_qos_trust_t trust)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_set_qos_policy(gport, trust));

    return CTC_E_NONE;
}


/**
 @brief get trust state for the given port
*/
int32
sys_humber_qos_get_port_trust(uint16 gport, ctc_qos_trust_t* p_trust)
{
    uint8 trust;

    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_get_qos_policy(gport, &trust));

    *p_trust = trust;

    return CTC_E_NONE;
}


/**
 @brief set default cos for the given port
*/
int32
sys_humber_qos_set_port_cos(uint16 gport, uint8 cos)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_MAX_VALUE_CHECK(cos, 7);
    CTC_ERROR_RETURN(sys_humber_port_set_default_pcp(gport, cos));

    return CTC_E_NONE;
}

/**
 @brief set default cfi for the given port
*/
int32
sys_humber_qos_set_port_cfi(uint16 gport, uint8 cfi)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_MAX_VALUE_CHECK(cfi, 1);
    CTC_ERROR_RETURN(sys_humber_port_set_default_dei(gport, cfi));

    return CTC_E_NONE;
}


/**
 @brief get default cos for the given port
*/
int32
sys_humber_qos_get_port_cos(uint16 gport, uint8* p_cos)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_get_default_pcp(gport, p_cos));

    return CTC_E_NONE;
}

/**
 @brief get default cfi for the given port
*/
int32
sys_humber_qos_get_port_cfi(uint16 gport, uint8* p_cfi)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_port_get_default_dei(gport, p_cfi));

    return CTC_E_NONE;
}


/**
 @brief set/unset replace state for the given port
*/
int32
sys_humber_qos_set_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool enable)
{
    SYS_ACLQOS_INIT_CHECK();

    if (CTC_QOS_REPLACE_COS == replace)
    {
        CTC_ERROR_RETURN(sys_humber_port_set_replace_cos_en(gport, enable));
    }
    else if (CTC_QOS_REPLACE_DSCP == replace)
    {
        CTC_ERROR_RETURN(sys_humber_port_set_replace_dscp_en(gport, enable));
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief Get replace state for the given port
*/
int32
sys_humber_qos_get_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool* p_enable)
{
    SYS_ACLQOS_INIT_CHECK();

    if (CTC_QOS_REPLACE_COS == replace)
    {
        CTC_ERROR_RETURN(sys_humber_port_get_replace_cos_en(gport, p_enable));
    }
    else if (CTC_QOS_REPLACE_DSCP == replace)
    {
        CTC_ERROR_RETURN(sys_humber_port_get_replace_dscp_en(gport, p_enable));
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief add an acl/qos entry to the the given label
*/
int32
sys_humber_aclqos_entry_add(uint32 label_id, ctc_aclqos_label_type_t label_type, uint32 entry_id, ctc_aclqos_entry_t* p_entry)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_insert(label_id, label_type, entry_id, p_entry));

    return CTC_E_NONE;
}


/**
 @brief remove acl/qos entry from the given label
*/
int32
sys_humber_aclqos_entry_remove(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_delete(label_id, label_type, entry_type, entry_id));

    return CTC_E_NONE;
}


/**
 @brief remove all acl/qos entries with particular entry type from the given label
*/
int32
sys_humber_aclqos_entry_remove_all(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_delete_all(label_id, label_type, entry_type));

    return CTC_E_NONE;
}


/**
 @brief To invalid an ACL entry, which indicates removing the entry from hardware table but still
        stay in software table.
*/
int32
sys_humber_aclqos_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_set_invalid(label_id, entry_type, entry_id, invalid));

    return CTC_E_NONE;
}


/**
 @brief get acl/qos entry stats result
*/
int32
sys_humber_aclqos_entry_stats_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_stats_result_get(label_id, label_type, entry_type, entry_id, p_stats));

    return CTC_E_NONE;
}


/**
 @brief reset acl/qos entry stats result
*/
int32
sys_humber_aclqos_entry_stats_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    SYS_ACLQOS_INIT_CHECK();

    if(entry_id == 0)   /*reset all entry stats of the entry type*/
    {
        CTC_ERROR_RETURN(sys_humber_aclqos_all_entry_stats_result_reset(label_id, label_type, entry_type));
    }
    else
    {
        CTC_ERROR_RETURN(sys_humber_aclqos_entry_stats_result_reset(label_id, label_type, entry_type, entry_id));
    }

    return CTC_E_NONE;
}

/**
 @brief set acl/qos entry action
*/
int32
sys_humber_aclqos_entry_set_action(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_action_add(label_id, label_type, entry_type, entry_id, p_action));

    return CTC_E_NONE;
}


/**
 @brief reset acl/qos entry action
*/
int32
sys_humber_aclqos_entry_unset_action(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_aclqos_entry_action_delete(label_id, label_type, entry_type, entry_id, p_action));

    return CTC_E_NONE;
}

int32
sys_humber_acl_get_permit_entry_stats_all(uint32 label_id, ctc_stats_basic_t* entry_stats)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_get_all_permit_entry_stats(label_id, entry_stats));

    return CTC_E_NONE;
}


int32
sys_humber_acl_tiny_fragment_default_entry_add(bool deny)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_acl_add_tiny_fragment_default_entry(deny));

    return CTC_E_NONE;
}

int32
sys_humber_qos_tiny_fragment_default_entry_add(bool deny)
{
    SYS_ACLQOS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_qos_add_tiny_fragment_default_entry(deny));

    return CTC_E_NONE;
}

/**
 @brief acl/qos initiliazation
*/
int32
sys_humber_aclqos_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_aclqos_label_init());
    CTC_ERROR_RETURN(sys_humber_qos_class_init());
    CTC_ERROR_RETURN(sys_humber_qos_policer_init(aclqos_global_cfg));
    CTC_ERROR_RETURN(sys_humber_aclqos_entry_init(aclqos_global_cfg));

    aclqos_init = 1;

    return CTC_E_NONE;
}


