/**
 @file ctc_humber_aclqos.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-01-13

 @version v2.0

   This file provide the acl/qos interface for customer.
*/

#ifndef _CTC_HUMBER_ACLQOS_H_
#define _CTC_HUMBER_ACLQOS_H_


/*********************************************************************
  *
  * ACL/QoS API
  *
  *********************************************************************/

/**
 @addtogroup aclqos ACLQoS
 @{
*/


/**
 @addtogroup acl ACL
 @{
*/

/**
 @brief Acl look up enable/disable on the given port

 @param[in] gport           Global port ID

 @param[in] dir           Direction

 @param[in] enable      a boolean value denote acl look up is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_port_enable(uint16 gport, ctc_direction_t dir, bool enable);

/**
 @brief Acl look up enable/disable on the given port

 @param[in] vid           VLAN ID

 @param[in] dir           Direction

 @param[in] enable      a boolean value denote acl look up is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_vlan_enable(uint16 vid, ctc_direction_t dir, bool enable);


/**
 @brief Acl look up enable/disable on the given l3 interface

 @param[in] l3if_id       l3 interface ID

 @param[in] enable      a boolean value denote acl look up is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_pbr_enable(uint16 l3if_id, bool enable);


/**
 @brief Create ACL port label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_port_label_create(uint32 label_id);


/**
 @brief Create ACL vlan label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_vlan_label_create(uint32 label_id);


/**
 @brief Create ACL pbr label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_pbr_label_create(uint32 label_id);


/**
 @brief Remove ACL port label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_port_label_remove(uint32 label_id);


/**
 @brief Remove ACL vlan label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_vlan_label_remove(uint32 label_id);


/**
 @brief Remove ACL pbr label

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_pbr_label_remove(uint32 label_id);


/**
 @brief Set ACL label to the specific direction of the given port

 @param[in] gport        Global port ID

 @param[in] dir          Direction

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_port_label_set(uint16 gport, ctc_direction_t dir, uint32 label_id);

/**
 @brief Unset ACL label to the specific direction of the given port

 @param[in] gport        Global port ID

 @param[in] dir          Direction

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_port_label_unset(uint16 gport, ctc_direction_t dir);

/**

 @brief Set ACL label to the specific direction of the given VLAN

 @param[in] vid          VLAN ID

 @param[in] dir          Direction

 @param[in] label_id     ACL label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_vlan_label_set(uint16 vid, ctc_direction_t dir, uint32 label_id);

/**

 @brief Unset ACL label to the specific direction of the given VLAN

 @param[in] vid          VLAN ID

 @param[in] dir          Direction

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_vlan_label_unset(uint16 vid, ctc_direction_t dir);



/**
 @brief Set existing ACL PBR label to the given l3 interface

 @param[in] l3if_id      l3 interface ID

 @param[in] label_id     ACL PBR label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_pbr_label_set(uint16 l3if_id, uint32 label_id);


/**
 @brief Unset existing ACL PBR label to the given l3 interface

 @param[in] l3if_id      l3 interface ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_pbr_label_unset(uint16 l3if_id);


/**
 @brief Set l2 ACL higher priority than l3 ACL

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] l2_high_prio  If TRUE, l2 ACL label has higher priority than l3 ACL label; else otherwise

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio);


/**
 @brief Add ACL entry after the entry ID in the given ACL label

 @param[in] label_id      ACL label ID

 @param[in] entry_id      ACL entry ID

 @param[in] p_entry       ACL entry

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_add(uint32 label_id, uint32 entry_id, ctc_aclqos_entry_t* p_entry);


/**
 @brief Remove ACL entry from the given ACL label

 @param[in] label_id      ACL label ID

 @param[in] entry_type    ACL entry type

 @param[in] entry_id      ACL entry ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_remove(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id);


/**
 @brief Remove all ACL entries with particular entry type from the given ACL label

 @param[in] label_id      ACL label ID

 @param[in] entry_type    ACL entry type

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_remove_all(uint32 label_id, ctc_aclqos_key_type_t entry_type);


/**
 @brief To invalid an ACL entry, which indicates removing the entry from hardware table but still
        stay in software table.

 @param[in] label_id      ACL label ID

 @param[in] entry_type    ACL entry type

 @param[in] entry_id      ACL entry ID

 @param[in] invalid       True to invalid an entry, FALSE to valid an entry.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid);


/**
 @brief Get ACL entry stats result

 @param[in] label_id      ACL label ID

 @param[in] entry_type    ACL entry type

 @param[in] entry_id      ACL entry ID

 @param[out] p_stats      Stats result

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_stats_get(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats);


/**
 @brief Reset ACL entry stats result

 @param[in] label_id      ACL label ID

 @param[in] entry_type    ACL entry type

 @param[in] entry_id      ACL entry ID, 0 means reset all entry stats

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_stats_reset(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id);

 /**
  @brief Acl entry action set

  @param[in] label_id      Acl label ID

  @param[in] entry_type    Acl entry type

  @param[in] entry_id      Acl entry ID

  @param[in] p_action      Acl action

  @return CTC_E_XXX
 */
extern int32
ctc_humber_acl_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

 /**
 @brief Acl entry action set

 @param[in] label_id      Acl label ID

 @param[in] entry_type    Acl entry type

 @param[in] entry_id      Acl entry ID

 @param[in] p_action      Acl action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_entry_unset_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

 /**
 @brief Add acl tiny fragment global default entry

 @param[in] deny      action deny or permit

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_tiny_fragment_default_entry_add(bool deny);

 /**
 @brief Get all acl permit entry stats in one label

 @param[in] label_id      Acl label ID

 @param[out] entry_stats      All acl permit entry stats

 @return CTC_E_XXX
*/
extern int32
ctc_humber_acl_get_permit_entry_stats_all(uint32 label_id, ctc_stats_basic_t* entry_stats);

 /**@} end of @addtogroup acl ACL*/


/**
 @addtogroup qos QoS
 @{
*/

 /**
  @brief QoS look up enable/disable on the given port

  @param[in] gport           Global port ID

  @param[in] dir           Direction

  @param[in] enable      a boolean value denote qos look up is enable

  @return CTC_E_XXX
 */
extern int32
ctc_humber_qos_port_enable(uint16 gport, ctc_direction_t dir, bool enable);

 /**
  @brief QoS look up enable/disable on the given VLAN

  @param[in] vid           VLAN ID

  @param[in] dir           Direction

  @param[in] enable      a boolean value denote qos look up is enable

  @return CTC_E_XXX
 */
extern int32
ctc_humber_qos_vlan_enable(uint16 vid, ctc_direction_t dir, bool enable);


/**
 @brief Create QoS port label

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_port_label_create(uint32 label_id);


/**
 @brief Create QoS vlan label

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_vlan_label_create(uint32 label_id);


/**
 @brief Remove QoS port label

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_port_label_remove(uint32 label_id);


/**
 @brief Remove QoS vlan label

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_vlan_label_remove(uint32 label_id);


/**
 @brief Set QoS label to the specific direction on the given port

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_port_label_set(uint16 gport, ctc_direction_t dir, uint32 label_id);

/**
 @brief Unset QoS label to the specific direction on the given port

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_port_label_unset(uint16 gport, ctc_direction_t dir);

/**
 @brief Set QoS label to the specific direction on the given VLAN

 @param[in] vid           VLAN ID

 @param[in] dir           Direction

 @param[in] label_id      Qos label ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_vlan_label_set(uint16 vid, ctc_direction_t dir, uint32 label_id);

/**
 @brief Unset QoS label to the specific direction on the given VLAN

 @param[in] vid           VLAN ID

 @param[in] dir           Direction

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_vlan_label_unset(uint16 vid, ctc_direction_t dir);

/**
 @brief Set l2 QoS higher priority than l3 QoS.

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] l2_high_prio  If TRUE, l2 QoS label gains higher priority than l3 QoS label; else otherwise

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_l2_high_priority(uint16 gport, ctc_direction_t dir, bool l2_high_prio);



/**
 @brief To globally enable/disable QoS policer.

 @param[in] enable        Enable/Disable (TRUE/FALSE) QoS policer globally

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_policer_global_enable(bool enable);


/**
 @brief Get QoS policer global enable status.

 @param[out] p_enable       Pointer to QoS policer global enable status.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_policer_global_enable(bool* p_enable);

/**
 @brief Two policing operations are performed in a sequential manner.

 @param[in] enable  Enable/Disable sequential manner.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_sequential_policer(bool enable);

/**
 @brief Set policer IPG enable.

 @param[in] enable  Enable/Disable policer IPG.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_policer_ipg_global_enable(bool enable);

/**
 @brief Set to do flow policer first.

 @param[in] dir           Direction

 @param[in] enable  Enable/Disable flow policer first.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_flow_policer_first_en(ctc_direction_t dir, bool enable);


/**
 @brief Create qos flow policer

 @param[in] p_policer     Pointer to policer data

 @param[in] plc_id        Policer ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_flow_policer_create(ctc_qos_policer_t* p_policer, uint32 plc_id);


/**
 @brief Remove qos flow policer

 @param[in] plc_id        Policer ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_flow_policer_remove(uint32 plc_id);


/**
 @brief Update QoS flow policer, replace old policer with a new one

 @param[in] plc_id        Policer ID

 @param[in] p_policer     Pointer to new policer data

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_flow_policer_update(uint32 plc_id, ctc_qos_policer_t* p_policer);


/**
 @brief Get real flow policer data added to chip.

 @param[in] plc_id        Policer ID

 @param[out] p_policer    Pointer to the policer data
*/
extern int32
ctc_humber_qos_get_flow_policer(uint32 plc_id, ctc_qos_policer_t* p_policer);


/**
 @brief Add QoS entry after the entry ID in the given label

 @param[in] label_id      QoS label ID

 @param[in] entry_id      Qos entry ID

 @param[in] p_entry       Pointer to QoS entry

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_add(uint32 label_id, uint32 entry_id, ctc_aclqos_entry_t* p_entry);


/**
 @brief Remove QoS entry from the given QoS label

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_remove(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id);


/**
 @brief Remove all entries with particular entry type from the given QoS label

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_remove_all(uint32 label_id, ctc_aclqos_key_type_t entry_type);


/**
 @brief To invalid a QoS entry, which indicates removing the entry from hardware table but still
        stay in software table.

 @param[in] label_id      QoS label ID

 @param[in] entry_type    QoS entry type

 @param[in] entry_id      QoS entry ID

 @param[in] invalid       True to invalid an entry, FALSE to valid an entry.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid);


/**
 @brief Get QoS entry stats result

 @param[in] label_id      QoS label ID

 @param[in] label_type    Qos label type

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID

 @param[out] p_stats      Stats result

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_stats_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats);


/**
 @brief Reset QoS entry stats result

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID, 0 means reset all entry stats

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_stats_reset(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id);

/**
 @brief QoS entry action set

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID

 @param[in] p_action      Qos action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

/**
 @brief QoS entry action reset

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID

 @param[in] p_action      Qos action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_entry_unset_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);


/**
 @brief Set port policer

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] phb_offset    PHB offset

 @param[in] p_policer     Pointer to port policer

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer);


/**
 @brief Unset port policer

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] phb_offset    PHB offset

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_unset_port_policer(uint16 gport, ctc_direction_t dir, uint8 phb_offset);


/**
 @brief Set service policer

 @param[in] service_id      Service Id

 @param[in] p_policer     Pointer to port policer

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_service_policer(uint16 service_id, ctc_qos_policer_t* p_policer);


/**
 @brief Unset service policer

 @param[in] service_id      Service Id

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_unset_service_policer(uint16 service_id);


/**
 @brief Get real port policer data added to chip.

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] phb_offset    PHB offset

 @param[out] p_policer    Pointer to policer data attached to the port

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_policer(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_policer);


/**
 @brief To globally enable/disable QoS policer statistics globally.

 @param[in] enable        Enable/Disable QoS policer statistics globally

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_policer_stats_global_enable(bool enable);


/**
 @brief Get QoS policer statistics enable status.

 @param[out] p_enable     Pointer to QoS policer globally enable status

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_policer_stats_global_enable(bool* p_enable);


/**
 @brief Get flow policer statistics.

 @param[in] plc_id        Flow policer ID

 @param[out] p_stats      Pointer to flow policer statistic result

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_flow_policer_stats(uint32 plc_id, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear flow policer statistics.

 @param[in] plc_id        Flow policer ID

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_reset_flow_policer_stats(uint32 plc_id);

/**
 @brief Get port policer statistics.

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] phb_offset    PHB offset

 @param[out] p_stats      Pointer to port policer statistic result

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear port policer statistics.

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] phb_offset    PHB offset

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_reset_port_policer_stats(uint16 gport, ctc_direction_t dir, uint8 phb_offset);

/**
 @brief Set ingress CoS -> priority + color mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] cos           CoS (Class of Service)

 @param[in] cfi           CFI (Canonical Format Indicator)

 @param[in] priority      Priority

 @param[in] color         Color

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_cos_priority_color_map(uint8 domain, uint8 cos, uint8 cfi, uint8 priority, ctc_qos_color_t color);


/**
 @brief Set ingress DSCP -> priority + color mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] dscp          IP DSCP

 @param[in] priority      Priority

 @param[in] color         Color

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_dscp_priority_color_map(uint8 domain, uint8 dscp, uint8 priority, ctc_qos_color_t color);


/**
 @brief Set ingress IP-precedence -> priority + color mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] ip_prec       IP Precedence

 @param[in] priority      Priority

 @param[in] color         Color

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_ip_prec_priority_color_map(uint8 domain, uint8 ip_prec, uint8 priority, ctc_qos_color_t color);


/**
 @brief Set ingress MPLS-EXCP -> priority + color mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] exp           MPLS EXP

 @param[in] priority      Priority

 @param[in] color         Color

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_exp_priority_color_map(uint8 domain, uint8 exp, uint8 priority, ctc_qos_color_t color);


/**
 @brief Set egress priority + color -> CoS mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] priority      Priority

 @param[in] color         Color

 @param[in] cos           CoS (Class of Service)

 @param[in] cfi           CFI (Canonical Format Indicator)

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_priority_color_cos_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 cos, uint8 cfi);


/**
 @brief Set egress priority + color -> DSCP mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] priority      Priority

 @param[in] color         Color

 @param[in] dscp          IP DSCP

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_priority_color_dscp_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 dscp);


/**
 @brief Set egress priority + color -> MPLS-EXP mapping table for the given domain

 @param[in] domain        QoS domain

 @param[in] priority      Priority

 @param[in] color         Color

 @param[in] exp           MPLS EXP

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_priority_color_exp_map(uint8 domain, uint8 priority, ctc_qos_color_t color, uint8 exp);


/**
 @brief Set QoS default domain value

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_default_domain(void);


/**
 @brief Set QoS domain for the given port

 @param[in] gport         Global port ID

 @param[in] dir           Direction

 @param[in] domain        QoS domain

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_domain(uint16 gport, ctc_direction_t dir, uint8 domain);


/**
 @brief Get QoS domain for the given port

 @param[in]  gport         Global port ID

 @param[in]  dir           Direction

 @param[out] p_domain      Pointer to QoS domain

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_domain(uint16 gport, ctc_direction_t dir, uint8* p_domain);


/**
 @brief Set trust state for the given port

 @param[in] gport         Global port ID

 @param[in] trust         Trust state

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_trust(uint16 gport, ctc_qos_trust_t trust);


/**
 @brief Get trust state for the given port

 @param[in]  gport         Global port ID

 @param[out] p_trust       Pointer to trust state

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_trust(uint16 gport, ctc_qos_trust_t* p_trust);


/**
 @brief Set trust state for the given port

 @param[in] gport         Global port ID

 @param[in] cos           CoS (Class of Service)

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_cos(uint16 gport, uint8 cos);

/**
 @brief Set trust state for the given port

 @param[in] gport         Global port ID

 @param[in] cfi           CFI (Canonical Format Indicator)

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_cfi(uint16 gport, uint8 cfi);


/**
 @brief Get trust state for the given port

 @param[in]  gport         Global port ID

 @param[out] p_cos         Pointer to CoS (Class of Service)

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_cos(uint16 gport, uint8* p_cos);

/**
 @brief Get trust state for the given port

 @param[in]  gport         Global port ID

 @param[out] p_cfi         Pointer to CFI (Canonical Format Indicator)

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_cfi(uint16 gport, uint8* p_cfi);


/**
 @brief Set/Unset replace state for the given port

 @param[in] gport         Global port ID

 @param[in] replace       Replace state

 @param[in] enable        Set/Unset (TRUE/FALSE) port replace

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_set_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool enable);


/**
 @brief Get replace state for the given port

 @param[in]  gport         Global port ID

 @param[in]  replace       Replace state

 @param[out] p_enable      Pointer to port replace enable status

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_get_port_replace_enable(uint16 gport, ctc_qos_replace_t replace, bool* p_enable);


/**
 @brief Create service label

 @param[in] service_id      Service ID identifying a unique service instance

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_label_create(uint16 service_id);


/**
 @brief Remove service label

 @param[in] service_id      Service ID identifying a unique service instance

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_label_remove(uint16 service_id);


/**
 @brief Add service entry after the entry ID in the service ID

 @param[in] p_entry_oper    Data to store entry operation information

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_entry_add(ctc_aclqos_entry_oper_t* p_entry_oper);

/**
 @brief Service entry action set

 @param[in] label_id      QoS label ID

 @param[in] entry_type    Qos entry type

 @param[in] entry_id      Qos entry ID

 @param[in] p_action      Qos action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_entry_set_action(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action);

/**
 @brief Remove service entry from the given service ID

 @param[in] p_entry_oper      Data to store entry operation information

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_entry_remove(ctc_aclqos_entry_oper_t* p_entry_oper);


/**
 @brief Remove all entries with particular entry type from the given service ID

 @param[in] service_id      Service ID identifying a unique service instance

 @param[in] entry_type    Service entry type

 @return CTC_E_XXX
*/
extern int32
ctc_humber_service_entry_remove_all(uint16 service_id, ctc_aclqos_key_type_t entry_type);

 /**
 @brief Add qos tiny fragment global default entry

 @param[in] deny      action deny or permit

 @return CTC_E_XXX
*/
extern int32
ctc_humber_qos_tiny_fragment_default_entry_add(bool deny);


/**@} end of @addtogroup qos QoS*/


/**
 @brief ACL/Qos initialization

 @return CTC_E_XXX
*/
extern int32
ctc_humber_aclqos_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg);


/**@} end of @addtogroup aclqos ACLQoS*/
#endif

