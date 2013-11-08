/**
 @file ctc_humber_mirror.h

 @date 2009-10-21

 @version v2.0

 This file define ctc functions of SDK
*/

#ifndef _CTC_HUMBER_MIRROR_H
#define _CTC_HUMBER_MIRROR_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "ctc_mirror.h"

/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @addtogroup mirror MIRROR
 @{
*/

/**
 @brief This function is to initialize the mirror module

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_init(void* mirror_global_cfg);

/**
 @brief This function is to set mirror enable on port

 @param[in] gport           global phyical port of system

 @param[in] dir             mirror direction, ingress or egress flow

 @param[in] session_id      mirror session id

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_port_enable(uint16 gport, ctc_direction_t dir, uint8 session_id);

/**
 @brief This function is to set mirror disable on port

 @param[in] gport           global phyical port of system

 @param[in] dir             mirror direction, ingress or egress flow

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_port_disable(uint16 gport, ctc_direction_t dir);

/**
 @brief This function is to get the information of port mirror

 @param[in] gport           global phyical port of system

 @param[in] dir             mirror direction, ingress or egress flow

 @param[in] enable          a return value denote whether mirror enable on port

 @param[in] session_id      a return value denote mirror session id if mirror enable on this port

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_get_port_info(uint16 gport, ctc_direction_t dir, bool* enable, uint8* session_id);

/**
 @brief This function is to set enable mirror on vlan

 @param[in] vlan_id         802.1q vlan id

 @param[in] dir             mirror direction, ingress or egress flow

 @param[in] session_id      mirror session id

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_vlan_enable(uint16 vlan_id, ctc_direction_t dir, uint8 session_id);

/**
 @brief This function is to set vlan disable to mirror

 @param[in] vlan_id         802.1q vlan id

 @param[in] dir             mirror direction, ingress or egress flow

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_vlan_disable(uint16 vlan_id, ctc_direction_t dir);

/**
 @brief This function is to get the information of vlan mirror

 @param[in] vlan_id         802.1q vlan id

 @param[in] dir             mirror direction, ingress or egress flow

 @param[in] enable          a return value denote whether mirror enable on port

 @param[in] session_id      a return value denote mirror session id if mirror enable on this port

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_get_vlan_info(uint16 vlan_id, ctc_direction_t dir, bool* enable, uint8* session_id);

/**
 @brief This function is to set local mirror destination port

 @param[in] mirror          mirror destination infomation

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_dest(ctc_mirror_dest_t* mirror);

/**
 @brief This function is to set remote mirror destination port

 @param[in] mirror_info          mirror destination infomation

 @param[in] nh_id           nexthop id created for rspan

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_rspan_set_dest(ctc_mirror_dest_t* mirror_info, uint32 nh_id);

/**
 @brief Discard some special mirrored packet if enable

 @param[in] enable  boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_rspan_escape_en(bool enable);

/**
 @brief Config mac info indicat the mirrored packet is special

 @param[in] escape  please refer to ctc_mirror_rspan_escape_t

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_rspan_escape_mac(ctc_mirror_rspan_escape_t escape);

/**
 @brief This function is to remove mirror destination port

 @param[in] mirror          mirror destination infomation

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_unset_dest(ctc_mirror_dest_t* mirror);

/**
 @brief This function is to set that whether packet is mirrored when it has been droped

 @param[in] dir             direciton ingress or egress

 @param[in] discard_flag    indicate port mirror or vlan mirror

 @param[in] enable          enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_set_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool enable);

/**
 @brief This function is to get that whether packet is mirrored when it has been droped

 @param[in] dir             direciton ingress or egress

 @param[in] discard_flag    indicate port mirror or vlan mirror

 @param[out] enable          a return value denote enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_mirror_get_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool* enable);

/**@} end of @addtogroup mirror MIRROR */

#endif

