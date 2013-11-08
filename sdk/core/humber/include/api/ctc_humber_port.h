/**
 @file ctc_humber_port.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-15

 @version v2.0

 This file define ctc functions of SDK
*/


#ifndef _CTC_HUMBER_PORT_H
#define _CTC_HUMBER_PORT_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_vlan.h"
#include "ctc_port.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @addtogroup port PORT
 @{
*/

/**
 @brief Initialize the port module
 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_init(void* port_global_cfg);

/**
 @brief Set port whether the tranmist is enable

 @param[in] gport global port of the system

 @param[in] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_transmit_en(uint16 gport, bool enable);

/**
 @brief Get port whether the tranmist is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_transmit_en(uint16 gport, bool* enable);

/**
 @brief Set port whether the receive is enable

 @param[in] gport global port of the system

 @param[in] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_receive_en(uint16 gport, bool enable);

/**
 @brief Get port whether the receive is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_receive_en(uint16 gport, bool* enable);

/**
 @brief Set port whehter layer2 bridge function is enable

 @param[in] gport global port of the system

 @param[in] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_bridge_en(uint16 gport, bool enable);

/**
 @brief Set port whehter layer2 bridge function is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_bridge_en(uint16 gport, bool* enable);

/**
 @brief Set the port as physical interface

 @param[in] gport global port of the system
 @param[in] enable   enable/disable the port as physical interface

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_phy_if_en(uint16 gport, bool enable);

/**
 @brief Get whether route function on port is enable

 @param[in]  gport   global port of the system
 @param[out] l3if_id the id of associated l3 interface
 @param[out] enable  enable/disable the port as physical interface

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_phy_if_en(uint16 gport,uint16* l3if_id, bool* enable);

/**
 @brief   Set the port as sub interface

 @param[in] gport global port of the system
 @param[in] enable enable/disable the port as physical interface

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_sub_if_en(uint16 gport, bool enable);

/**
 @brief Get whether the port is routed port

 @param[in] gport global port of the system
 @param[out] enable enable/disable the port as physical interface

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_sub_if_en(uint16 gport, bool* enable);

/**
 @brief Set default vlan id of packet which receive from this port

 @param[in] gport global port of the system

 @param[in] vid default vlan id of the packet

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_default_vlan(uint16 gport, uint16 vid);

/**
 @brief Get default vlan id of packet which receive from this port

 @param[in] gport global port of the system

 @param[out] vid default vlan id of the packet

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_default_vlan(uint16 gport, uint16* vid);

/**
 @brief Set port's vlan tag control mode

 @param[in] gport global port of the system

 @param[in] mode a enum value defined in ctc_port_h

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_vlanctl(uint16 gport, ctc_vlantag_ctl_t mode);

/**
 @brief Get port's vlan tag control mode

 @param[in] gport global port of the system

 @param[out] mode a enum value defined in ctc_port_h

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_vlanctl(uint16 gport, ctc_vlantag_ctl_t* mode);

/**
 @brief If set, We consider it's ctag when packet singgle tagged with tpid 0x8100

 @param[in] gport global port of the system

 @param[in] is_cvlan  a boolean value denote the property

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_outer_is_cvlan(uint16 gport, bool is_cvlan);

/**
 @brief Get outer is cvlan

 @param[in] gport global port of the system

 @param[out] is_cvlan a boolean value denote the property

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_outer_is_cvlan(uint16 gport, bool* is_cvlan);

/**
 @brief If set, denote that this port is can parsering svlan tag

 @param[in] gport global port of the system

 @param[in] is_svlan  a boolean value denote the property

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_src_outer_is_svlan(uint16 gport, bool is_svlan);

/**
 @brief Get src outer is svlan

 @param[in] gport global port of the system

 @param[out] is_svlan  a boolean value denote the property

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_src_outer_is_svlan(uint16 gport, bool* is_svlan);

/**
 @brief If set, denote that for double tagged packet, this port is use ctag cos or not

 @param[in] gport global port of the system

 @param[in] is_inner  a boolean value denote the property

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_use_inner_cos(uint16 gport, bool is_inner);

/**
 @brief For double tagged packet, Get this port is use ctag cos

 @param[in] gport global port of the system

 @param[out] is_inner  a boolean value denote the property

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_use_inner_cos(uint16 gport, bool* is_inner);

/**
 @brief The function is to set stag tpid index

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress and both direction

 @param[in] index stag tpid index, can be configed in ethernet ctl register

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 index);

/**
 @brief The function is to get stag tpid index

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress

 @param[out] index stag tpid index, the index point to stag tpid value

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8* index);

/**
 @brief Set vlan filtering enable/disable on the port

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress or both direction

 @param[in] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool enable);

/**
 @brief Get vlan filtering enable/disable on the port

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress

 @param[out] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool* enable);

/**
 @brief Set port cross econnet forwarding by port if enable

 @param[in] gport global port of the system

 @param[in] enable cross connect is enable on the port if enable set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_cross_connet(uint16 gport, bool enable);

/**
 @brief Get port cross connect

 @param[in] gport global port of the system

 @param[out] enable cross connect is enable on the port if enable set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_cross_connet(uint16 gport, bool* enable);

/**
 @brief Set learning enable/disable on this port

 @param[in] gport global port of the system

 @param[in] enable mac learning is enable on the port if enable set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_learning_en(uint16 gport, bool enable);

/**
 @brief Get learning enable/disable on the port

 @param[in] gport global port of the system

 @param[out] enable mac learning is enable on the port if enable set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_learning_en(uint16 gport, bool* enable);

/**
 @brief Get enable/disable keep vlan tag on the port

 @param[in] gport global port of the system

 @param[out] enable mac learning is enable on the port if enable set

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_keep_vlan_tag(uint16 gport, bool enable);

/**
 @brief Get Get keep vlan tag state on the port

 @param[in] gport global port of the system

 @param[out] enable mac learning is enable on the port if enable set

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_keep_vlan_tag(uint16 gport, bool* enable);

/**
 @brief Set what tag the packet with transmit from the port

 @param[in] gport global port of the system

 @param[in] type dot1q type of port untagged/ctagged/stagged/double-tagged

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_dot1q_type(uint16 gport, ctc_dot1q_type_t type);

/**
 @brief  Get what tag the packet with transmit from the port

 @param[in] gport global port of the system

 @param[out] type dot1q type of port untagged/ctagged/stagged/double-tagged

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_dot1q_type(uint16 gport, ctc_dot1q_type_t* type);

/**
 @brief Set use outer ttl in case of tunnel

 @param[in] gport global port of the system

 @param[in] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_use_outer_ttl(uint16 gport, bool enable);

/**
 @brief Get use outer ttl in case of tunnel

 @param[in] gport global port of the system

 @param[out] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_use_outer_ttl(uint16 gport, bool* enable);

/**
 @brief Set untag default vid enable/disable on svlan or cvlan

 @param[in] gport global port of the system

 @param[in] enable untag packet's default vid if enable is set

 @param[in] untag_svlan untag svlan id if enable while untag cvlan vid if disable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_untag_dft_vid(uint16 gport, bool enable, bool untag_svlan);

/**
 @brief Get untag default vid enable/disable on svlan or cvlan

 @param[in] gport global port of the system

 @param[out] enable untag packet's default vid if enable is set

 @param[out] untag_svlan untag svlan id if enable while untag cvlan vid if disable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_untag_dft_vid(uint16 gport, bool* enable, bool* untag_svlan);

/**
 @brief Set ucast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_ucast_flooding_en(uint16 gport, bool enable);

/**
 @brief Get ucast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[out] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_ucast_flooding_en(uint16 gport, bool* enable);

/**
 @brief Set mcast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_mcast_flooding_en(uint16 gport, bool enable);

/**
 @brief Get mcast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[out] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_mcast_flooding_en(uint16 gport, bool* enable);

 /**
 @brief Set reflective bridge enable or disable
        For mcast, it should be called before mcast member added. So if you
        want to do mcast reflective bridge, you'd better call it at initical code.
 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_reflective_bridge_en(uint16 gport, bool enable);

/**
 @brief Get reflective bridge to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[out] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_reflective_bridge_en(uint16 gport, bool* enable);


/**
 @brief Set allow mcast mac sa to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_allow_mcast_mac_sa(uint16 gport, bool enable);

/**
 @brief Set protocol vlan enable/disable on the port

 @param[in] gport global phyical port of system

 @param[in] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_pro_vlan_en(uint16 gport, bool enable);

/**
 @brief Get protocol vlan enable/disable of the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_pro_vlan_en(uint16 gport, bool* enable);

/**
 @brief Set random log function of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[in] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_random_log_en(uint16 gport, ctc_direction_t dir, bool enable);

/**
 @brief Get random log function of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[out] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_random_log_en(uint16 gport, ctc_direction_t dir, bool* enable);

/**
 @brief Set random threshold of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[in] threshold Max value is 0x7FFF, log rate is threshold/0x7FFF

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_random_threshold(uint16 gport, ctc_direction_t dir, uint16 threshold);

/**
 @brief Get random threshold of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[out] threshold Max value is 0x7FFF, log rate is threshold/0x7FFF

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_random_threshold(uint16 gport, ctc_direction_t dir, uint16* threshold);

/**
 @brief The function is to set vlan mapping enable on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_vlan_mapping_en(uint16 gport, bool enable);

/**
 @brief The function is to get vlan mapping enable on the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_vlan_mapping_en(uint16 gport, bool* enable);

/**
 @brief Set enable/disable vlan switching on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_vlan_switching_en(uint16 gport, bool enable);

/**
 @brief Get enable/disable vlan switching on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_vlan_switching_en(uint16 gport, bool* enable);

/**
 @brief Set vlan classification enable on the port

 @param[in] gport Global phyical port of system

 @param[in] type Vlan classifcation type, maybe mac-based or ip-based vlan

 @param[in] policy_id The policy can be setting same on ports to share the same rule

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t type);

/**
 @brief The function is to disable vlan classification on the port

 @param[in] gport Global phyical port of system

 @param[in] type Vlan classifcation type, maybe mac-based or ip-based vlan

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_vlan_classify_disable(uint16 gport, ctc_vlan_class_type_t type);

/**
 @brief The fucntion is to get vlan classification type and policy on the port

 @param[in] gport Global phyical port of system

 @param[out] type Vlan classifictiaon type, maybe mac-based or ip-based vlan

 @param[out] policy_id The policy settled by user.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t* type);

/**
 @brief Enable/disable ip source guard on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return SDK_E_XXX
*/
extern int32
ctc_humber_port_set_ipsg_en(uint16 gport, bool enable);

/**
 @brief Get ip source guard state of the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolean value denote the enable/disable

 @return SDK_E_XXX
*/
extern int32
ctc_humber_port_get_ipsg_en(uint16 gport, bool* enable);

/**
 @brief Set port mac enable

 @param[in] gport   Global phyical port of system

 @param[in] enable  A boolean value denote port mac enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_mac_en(uint16 gport, bool enable);

/**
 @brief Get port mac enable

 @param[in] gport   Global phyical port of system

 @param[out] enable  A boolean value denote port mac enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_mac_en(uint16 gport, bool* enable);

/**
 @brief Set port speed mode

 @param[in] gport       Global phyical port of system

 @param[in] speed_mode  Speed at 10M 100M 1G of Gmac

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_speed(uint16 gport, ctc_port_speed_t speed_mode);

/**
 @brief Set port speed mode

 @param[in] gport       Global phyical port of system

 @param[out] speed_mode Speed at 10M 100M 1G of Gmac

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_speed(uint16 gport, ctc_port_speed_t* speed_mode);

/**
 @brief Set max frame size of system, two value can be configure

 @param[in] index   indicate index of max frame to be set

 @param[in] value   max frame size

 @return CTC_E_XXX
*/
extern int32
ctc_humber_set_max_frame_size(ctc_frame_size_t index, uint16 value);

/**
 @brief Get max frame size of system, may be one of index value

 @param[in] index   indicate index of max frame to be set

 @param[out] value  max frame size

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_max_frame_size(ctc_frame_size_t index, uint16* value);

/**
 @brief Set cpu mac enable

 @param[in] enable   a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_set_cpu_mac_en(bool enable);

/**
 @brief Get cpu mac enable

 @param[out] enable    a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_cpu_mac_en(bool* enable);

/**
 @brief Set max frame size per port

 @param[in] gport   global phyical port

 @param[in] index   size index

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_max_frame(uint16 gport, ctc_frame_size_t index);

/**
 @brief Set max frame size per port

 @param[in] gport   global phyical port

 @param[out] index  size index

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_max_frame(uint16 gport, ctc_frame_size_t* index);

/**
 @brief Set flow control of port

 @param[in] gport   global phyical port

 @param[in] dir     denote receive or transmit pause frame

 @param[in] enable  a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 enable);

/**
 @brief Get flow control of port

 @param[in] gport   global phyical port

 @param[in] dir     denote receive or transmit pause frame

 @param[out] enable a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32* enable);

/**
 @brief Set port preamble

 @param[in] gport       global phyical port

 @param[in] pre_bytes   preamble value, for xgma/sgmac can only be 4bytes and 8bytes

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_preamble(uint16 gport, uint8 pre_bytes);

/**
 @brief Get port preamble

 @param[in] gport       global phyical port

 @param[out] pre_bytes   preamble value, for xgma/sgmac can only be 4bytes and 8bytes

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_preamble(uint16 gport, uint8* pre_bytes);

/**
 @brief Set port min frame size

 @param[in] gport       global phyical port

 @param[in] size   min frame size, not support xgma/sgmac

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_min_frame_size(uint16 gport, uint8 size);

/**
 @brief Get port min frame size

 @param[in] gport       global phyical port

 @param[out] size   min frame size, not support xgma/sgmac

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_min_frame_size(uint16 gport, uint8* size);

/**
 @brief Set port stretch mode

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_stretch_mode_en(uint16 gport, bool enable);

/**
 @brief Get port stretch mode

 @param[in] gport       global phyical port

 @param[out] enable     a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_stretch_mode_en(uint16 gport, bool* enable);

/**
 @brief Set port pading (if packet length less than 63bytes, packet length will be paded to be 64bytes);

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_pading_en(uint16 gport, bool enable);

/**
 @brief Get port pading (if packet length less than 63bytes, packet length will be paded to be 64bytes);

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_pading_en(uint16 gport, bool* enable);

/**
 @brief Set port whether the srcdiscard is enable

 @param[in] gport global port of the system

 @param[in] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_srcdiscard_en(uint16 gport, bool enable);

/**
 @brief Get port whether the srcdiscard is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value denote the function enable or not

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_srcdiscard_en(uint16 gport, bool* enable);


/**
 @brief Set port loopback

 @param[in] p_port_lbk point to  ctc_port_lbk_param_t

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_loopback (ctc_port_lbk_param_t* p_port_lbk);

/**
 @brief Set port port check enable

 @param[in] gport  global port of the system

 @param[in] enable  a boolean value wanted to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_port_check_en (uint16 gport, bool enable);

/**
 @brief Get port port check enable

 @param[in] gport  global port of the system

 @param[in] enable  a boolean value wanted to get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_port_check_en(uint16 gport, bool* enable);

/**@} end of @addgroup   */

#endif

