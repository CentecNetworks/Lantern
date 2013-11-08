/**
 @file ctc_humber_vlan.h

 @date 2009-10-17

 @version v2.0

 This file is to define Vlan types and ctc APIs

*/

 #ifndef _CTC_HUMBER_VLAN_H
 #define _CTC_HUMBER_VLAN_H

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_vlan.h"
#include "ctc_const.h"
#include "ctc_parser.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
/**
 @addtogroup vlan VLAN
 @{
*/

/**
 @brief init the vlan module
 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_init(ctc_vlan_global_cfg_t* vlan_global_cfg);

/**
 @brief The function is to create a vlan

 @param[in] vlan_id         802.1q vlan id

 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_create_vlan(uint16 vlan_id);

/**
 @brief The function is to remove the vlan

 @param[in] vlan_id         802.1q vlan id

 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_remove_vlan(uint16 vlan_id);

/**
 @brief The function is to add member port to a vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] gport           Global port of the local chip

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_add_port(uint16 vlan_id, uint16 gport);

/**
 @brief The function is to show vlan's member port

 @param[in] vlan_id         802.1q vlan id

 @param[out] port_bitmap    bits map of port in vlan

 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_get_ports(uint16 vlan_id, ctc_port_bitmap_t* port_bitmap);

/**
 @brief The function is to remove member port to a vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] gport           Global port of the local chip

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_port(uint16 vlan_id, uint16 gport);

/**
 @brief The function is to set receive enable on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote receive is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_set_receive_en(uint16 vlan_id, bool enable);

/**
 @brief The function is to get receive on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote receive is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_receive_en(uint16 vlan_id, bool* enable);

/**
 @brief The function is to set tranmit enable on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote tranmit is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_vlan_set_transmit_en(uint16 vlan_id, bool enable);

/**
 @brief The function is to get tranmit on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote tranmit is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_transmit_en(uint16 vlan_id, bool* enable);

/**
 @brief The function is to set bridge enable on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote tranmit is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_bridge_en(uint16 vlan_id, bool enable);

/**
 @brief The function is to get bridge on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean value denote tranmit is enable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_bridge_en(uint16 vlan_id, bool* enable);

/**
 @brief The fucntion is to set vrfid of vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] fid             Fid used in bridge

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_fid(uint16 vlan_id, uint16 fid);

/**
 @brief The fucntion is to get vrfid of vlan

 @param[in] vlan_id         802.1q vlan id

 @param[out] fid            Fid used in bridge

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_fid(uint16 vlan_id, uint16* fid);

/**
 @brief The function is set mac learning enable/disable on the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean val denote learning enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_learning_en(uint16 vlan_id, bool enable);

/**
 @brief The function is get mac learning enable/disable on the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[out] enable         A boolean val denote learning enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_learning_en(uint16 vlan_id, bool* enable);

/**
 @brief The function is to set igmp snooping enable on the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[out] enable         A boolean val denote learning enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_igmp_snoop_en(uint16 vlan_id, bool enable);

/**
 @brief The function is to get igmp snooping enable of the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[out] enable         A boolean val denote learning enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_igmp_snoop_en(uint16 vlan_id, bool* enable);

/**
 @brief The function is to set dhcp exception action of the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] type            exception type, refer to data structure of ctc_exception_type_s

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t type);

/**
 @brief The function is to get dhcp exception action of the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] type            exception type, refer to data structure of ctc_exception_type_s

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t* type);

/**
 @brief The function is to set arp exception action of the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] type            exception type, refer to data structure of ctc_exception_type_s

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_arp_excp_type(uint16 vlan_id, ctc_exception_type_t type);

/**
 @brief The function is to get arp exception action of the vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] type            exception type, refer to data structure of ctc_exception_type_s

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_arp_excp_type(uint16 vlan_id, ctc_exception_type_t* type);

/**
 @brief The function is to set replace packet's dscp by classify of vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] enable          A boolean val denote replace enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_set_replace_dscp_en(uint16 vlan_id, bool enable);

/**
 @brief The function is to set replace packet's dscp by classify of vlan

 @param[in] vlan_id         802.1q vlan id

 @param[out] enable         A boolean val denote replace enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_get_replace_dspc_en(uint16 vlan_id, bool* enable);

/**
 @addtogroup VlanClassification VlanClassification
 @{
*/

/**
 @brief The function is to add one vlan classification rule

 @param[in] p_vlan_class    Vlan classification info, include key and policy

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_add_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class);

/**
 @brief The fucntion is to remove on vlan classification rule

 @param[in] p_vlan_class    Vlan classification info, include key and policy

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class);


/**
 @brief The fucntion is to flush vlan classification by type

 @param[in] ctc_vlan_class_type_t    Vlan classification type, like mac-based or ip-based.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_flush_vlan_classification_entry(ctc_vlan_class_type_t type);



/**
 @brief Add vlan class default entry per label

 @param[in] type            Vlan class type, mac-based ipv4-based and ipv6-based

 @param[in] policy_id       Vlan class policy which default entry based

 @param[in] p_action        Default entry action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_add_vlan_classification_default_entry(ctc_vlan_class_type_t type, ctc_vlan_miss_t* p_action);
/**
 @brief Remove vlan class default entry per label

 @param[in] type            Vlan class type, mac-based ipv4-based and ipv6-based

 @param[in] policy_id       Vlan class policy which default entry based

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_vlan_classification_default_entry(ctc_vlan_class_type_t type);


/**@} end of @addtogroup VlanClassification*/

/**
 @addtogroup VlanMapping VlanMapping
 @{
*/

/**
 @brief The function is to add one vlan mapping entry on the port in IPE of Humber

 @param[in] gport           Global phyical port of system

 @param[in] p_vlan_mapping  Vlan mapping info, include key and mapped action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_add_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping);

/**
 @brief The function is to remove one vlan mapping entry on the port in IPE of Humber

 @param[in] gport           Global phyical port of system

 @param[in] p_vlan_mapping  Vlan mapping info, include key info to be deleted

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping);

/**
 @brief The function is to add vlan mapping default entry on the port

 @param[in] gport           Global phyical port of system

 @param[in] p_action        Default entry action

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_add_vlan_mapping_default_entry(uint16 gport, ctc_vlan_miss_t* p_action);

/**
 @brief The function is to remove vlan mapping miss match default entry of port

 @param[in] gport           Global phyical port of system

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_vlan_mapping_default_entry(uint16 gport);

/**
 @brief The function is to remove all vlan mapping entries on port

 @param[in] gport           Global phyical port of system

 @return CTC_E_XXX
*/
extern int32
ctc_humber_vlan_remove_vlan_mapping_all_by_port(uint16 gport);

/**@} end of @addtogroup VlanMapping*/


/**@} end of @addtogroup EgressVlanEdit EgressVlanEdit*/

/**@} end of @addtogroup vlan VLAN*/

#endif

