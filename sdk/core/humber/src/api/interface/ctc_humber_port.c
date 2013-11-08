/**
 @file ctc_humber_port.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-16

 @version v2.0

 The file provide all port related APIs of Humber SDK.
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"
#include "ctc_debug.h"

#include "ctc_humber_port.h"
#include "sys_humber_port.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief initialize the port module

 @param[]

 @return CTC_E_XXX

*/
int32 ctc_humber_port_init(void* port_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_port_init());
    return CTC_E_NONE;
}

/**
 @brief set port whether the tranmist is enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_transmit_en(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, transmit_enable:%d!\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_transmit_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port whether the tranmist is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_transmit_en(uint16 gport, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_transmit_en(gport, enable));

    return CTC_E_NONE;
}


/**
 @brief set port whether the receive is enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_receive_en(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, receive_enable:%d!\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_receive_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port whether the receive is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_receive_en(uint16 gport, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_receive_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set port whehter layer2 bridge function is enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_bridge_en(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, bridge_enable:%d!\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_bridge_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set port whehter layer2 bridge function is enable

 @param[in] gport global port of the system

 @param[out] enable a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_bridge_en(uint16 gport, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_bridge_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set the port as physical interface

 @param[in] gport global port of the system

 @param[in] enable   enable/disable the port as physical interface

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_set_phy_if_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_phy_if_en(gport, enable));
    return CTC_E_NONE;
}

/**
 @brief Get whether route function on port is enable

 @param[in] gport global port of the system

 @param[out] l3if_id bind physical interface(l3if)'s l3if_id on this port

 @param[out] enable enable/disable the port as physical interface

 @return CTC_E_XXX
*/
extern int32
ctc_humber_port_get_phy_if_en(uint16 gport,uint16* l3if_id, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_phy_if_en(gport,l3if_id, enable));
    return CTC_E_NONE;

}

/**
 @brief   set the port as sub interface

 @param[in] gport global port of the system

 @param[in] is_routed enable/disable the port as physical interface

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_set_sub_if_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_sub_if_en(gport, enable));
    return CTC_E_NONE;
}

/**
 @brief get whether the port is routed port

 @param[in] gport global port of the system

 @param[out] enable enable/disable the port as physical interface

 @return CTC_E_XXX

*/
extern int32
ctc_humber_port_get_sub_if_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_sub_if_en(gport, enable));
    return CTC_E_NONE;

}


/**
 @brief set default vlan id of packet which receive from this port

 @param[in] gport, global port of the system

 @param[in] vid, default vlan id of the packet

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_default_vlan(uint16 gport, uint16 vid)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, default_vid:%d\n", gport, vid);

    CTC_ERROR_RETURN(sys_humber_port_set_default_vlan(gport, vid));

    return CTC_E_NONE;
}

/**
 @brief get default vlan id of packet which receive from this port

 @param[in] gport global port of the system

 @param[out] vid default vlan id of the packet

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_default_vlan(uint16 gport, uint16* vid)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_default_vlan(gport, vid));

    return CTC_E_NONE;
}

/**
 @brief set port's vlan tag control mode

 @param[in] gport, global port of the system

 @param[in] mode, a enum value defined in port_h

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_vlanctl(uint16 gport, ctc_vlantag_ctl_t mode)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, aft_value:%d\n", gport, mode);

    CTC_ERROR_RETURN(sys_humber_port_set_vlanctl(gport, mode));

    return CTC_E_NONE;
}

/**
 @brief get port's vlan tag control mode

 @param[in] gport global port of the system

 @param[out] mode a enum value defined in ctc_port_h

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_vlanctl(uint16 gport, ctc_vlantag_ctl_t* mode)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_vlanctl(gport, mode));

    return CTC_E_NONE;
}

/**
 @brief set packet received from this port vlan tag outer vlan is cvlan

 @param[in] gport, global port of the system

 @param[in] is_cvlan, a boolean value denote the property

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_outer_is_cvlan(uint16 gport, bool is_cvlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, wheter outer vlan is cvlan:%d\n", gport, is_cvlan);

    CTC_ERROR_RETURN(sys_humber_port_set_outer_is_cvlan(gport, is_cvlan));

    return CTC_E_NONE;
}

/**
 @brief get packet received from this port vlan tag outer vlan is cvlan

 @param[in] gport global port of the system

 @param[out] is_cvlan, a boolean value denote the property

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_outer_is_cvlan(uint16 gport, bool* is_cvlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_outer_is_cvlan(gport, is_cvlan));

    return CTC_E_NONE;
}

/**
 @brief If set, denote that this port is can parsering svlan tag

 @param[in] gport global port of the system

 @param[in] is_svlan  a boolean value denote the property

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_src_outer_is_svlan(uint16 gport, bool is_svlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, wheter outer vlan is svlan:%d\n", gport, is_svlan);

    CTC_ERROR_RETURN(sys_humber_port_set_src_outer_is_svlan(gport, is_svlan));

    return CTC_E_NONE;
}

/**
 @brief Get src outer is svlan

 @param[in] gport global port of the system

 @param[out] is_svlan  a boolean value denote the property

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_src_outer_is_svlan(uint16 gport, bool* is_svlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_src_outer_is_svlan(gport, is_svlan));

    return CTC_E_NONE;
}

/**
 @brief If set, denote that for double tagged packet, this port is use ctag cos or not

 @param[in] gport global port of the system

 @param[in] is_inner  a boolean value denote the property

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_use_inner_cos(uint16 gport, bool is_inner)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, wheter use stag cos or not:%d\n", gport, is_inner);

    CTC_ERROR_RETURN(sys_humber_port_set_use_inner_cos(gport, is_inner));

    return CTC_E_NONE;
}

/**
 @brief For double tagged packet, Get this port is use ctag cos

 @param[in] gport global port of the system

 @param[out] is_inner  a boolean value denote the property

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_use_inner_cos(uint16 gport, bool* is_inner)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_use_inner_cos(gport, is_inner));

    return CTC_E_NONE;
}

/**
 @brief The function is to set stag tpid index

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress

 @param[in] index stag tpid index, can be configed in ethernet ctl register

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 index)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, direction:%d, stag TPID index:%d\n", gport, dir, index);

    CTC_ERROR_RETURN(sys_humber_port_set_stag_tpid_index(gport, dir, index));

    return CTC_E_NONE;
}

/**
 @brief The function is to get stag tpid index

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress

 @param[out] index stag tpid index, the index point to stag tpid value

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8* index)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_stag_tpid_index(gport, dir, index));

    return CTC_E_NONE;
}


/**
 @brief set vlan filtering enable/disable on the port

 @param[in] gport, global port of the system

 @param[in] dir can be ingress, egress or both direction

 @param[in] enable, a boolean value denote whether the function is enable

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, 0-Ingress 1-Egress 2-Both:%d, vlan filtering enable:%d\n",
    gport, dir, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_vlan_filter_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief get vlan filtering enable/disable on the port

 @param[in] gport global port of the system

 @param[in] dir can be ingress, egress or both direction

 @param[out] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, 0-Ingress 1-Egress 2-Both:%d!\n", gport, dir);

    CTC_ERROR_RETURN(sys_humber_port_get_vlan_filter_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief set port cross connect is enable/disable

 @param[in] gport global port of the system

 @param[in] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_cross_connet(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d, enable:%d\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_cross_connect(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief get port cross connect

 @param[in] gport global port of the system

 @param[out] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_cross_connet(uint16 gport, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_cross_connect(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set port cross connect is enable/disable

 @param[in] gport global port of the system

 @param[in] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_learning_en(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d, enable:%d\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_learning_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief get learning enable/disable on the port

 @param[in] gport global port of the system

 @param[out] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_learning_en(uint16 gport, bool* enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_learning_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief get enable/disable keep vlan tag on the port

 @param[in] gport global port of the system

 @param[in] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/int32
ctc_humber_port_set_keep_vlan_tag(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_keep_vlan_tag(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief get Get keep vlan tag state on the port

 @param[in] gport global port of the system

 @param[out] enable a boolean value denote whether the function is enable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_keep_vlan_tag(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_keep_vlan_tag(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set port dot1q type, untagged/ctagged/stagged/double-tagged

 @param[in] gport global port of the system

 @param[in] type of dot1q type,untagged/ctagged/stagged/double-tagged

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_dot1q_type(uint16 gport, ctc_dot1q_type_t type)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info : gport:%d, type :%d\n", gport, type);

    CTC_ERROR_RETURN(sys_humber_port_set_dot1q_type(gport, type));

    return CTC_E_NONE;
}

/**
 @brief  get what tag the packet with dot1q type from the port

 @param[in] gport global port of the system

 @param[out] type of dot1q type,untagged/ctagged/stagged/double-tagged

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_dot1q_type(uint16 gport, ctc_dot1q_type_t* type)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info : gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_dot1q_type(gport, type));

    return CTC_E_NONE;
}

/**
 @brief Set use outer ttl in case of tunnel

 @param[in] gport global port of the system

 @param[in] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_use_outer_ttl(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_use_outer_ttl(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get use outer ttl in case of tunnel

 @param[in] gport global port of the system

 @param[out] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_use_outer_ttl(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_use_outer_ttl(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief untag transmit packet's vid if packet's vid equal to default vid of port

 @param[in] gport global port of the system

 @param[in] enable a boolen vaule denote the function is enable/disable

 @param[in] untag svlan or cvlan of transmit packet

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_untag_dft_vid(uint16 gport, bool enable, bool untag_svlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info : gport:%d, untag default vid:%d, untag svlan:%d\n", gport, enable, untag_svlan);

    CTC_ERROR_RETURN(sys_humber_port_set_untag_dft_vid(gport, enable, untag_svlan));

    return CTC_E_NONE;
}

/**
 @brief get untag default vid enable/disable on svlan or cvlan

 @param[in] gport global port of the system

 @param[out] enable a boolen vaule denote the function is enable/disable

 @param[out] untag svlan or cvlan of transmit packet

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_untag_dft_vid(uint16 gport, bool* enable, bool* untag_svlan)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info : gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_untag_dft_vid(gport, enable, untag_svlan));

    return CTC_E_NONE;
}

/**
 @brief Set ucast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_ucast_flooding_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_ucast_flooding_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get ucast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[out] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_ucast_flooding_en(uint16 gport, bool *enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_ucast_flooding_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set mcast unkown flooding to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[in] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_mcast_flooding_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_mcast_flooding_en(gport, enable));

    return CTC_E_NONE;
}

/**
@brief Get mcast unkown flooding to this port enable or disable

@param[in] gport   global phyical port of system

@param[out] enable  a boolen vaule denote the function is enable / disable

@return CTC_E_XXX
*/
int32
ctc_humber_port_get_mcast_flooding_en(uint16 gport, bool *enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_mcast_flooding_en(gport, enable));

    return CTC_E_NONE;
}

/**
@brief Set reflective bridge enable or disable
For mcast, it should be called before mcast member added. So if you
want to do mcast reflective bridge, you'd better call it at initical code.
@param[in] gport   global phyical port of system

@param[in] enable  a boolen vaule denote the function is enable / disable

@return CTC_E_XXX
*/
int32
ctc_humber_port_set_reflective_bridge_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_reflective_bridge_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get reflective bridge to this port enable or disable

 @param[in] gport   global phyical port of system

 @param[out] enable  a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_reflective_bridge_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_reflective_bridge_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief set port whether the allow mcast mac sa is enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_allow_mcast_mac_sa(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, allow_mcast_mac_sa:%d!\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_allow_mcast_mac_sa(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set protocol vlan enable/disable on the port

 @param[in] gport global phyical port of system

 @param[in] enable a boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_pro_vlan_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_protocol_vlan_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get protocol vlan enable/disable of the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_pro_vlan_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_protocol_vlan_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set random log function of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[in] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_random_log_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_random_log_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief Get random log function of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[out] enable A boolen vaule denote the function is enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_random_log_en(uint16 gport, ctc_direction_t dir, bool *enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_random_log_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief Set random threshold of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[in] threshold Max value is 0x7FFF, log rate is threshold/0x7FFF

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_random_threshold(uint16 gport, ctc_direction_t dir, uint16 threshold)
{
    CTC_ERROR_RETURN(sys_humber_port_set_random_threshold(gport, dir, threshold));

    return CTC_E_NONE;
}

/**
 @brief Get random threshold of port

 @param[in] gport Global phyical port of system

 @param[in] dir   Flow direction

 @param[out] threshold Max value is 0x7FFF, log rate is threshold/0x7FFF

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_random_threshold(uint16 gport, ctc_direction_t dir, uint16* threshold)
{
    CTC_ERROR_RETURN(sys_humber_port_get_random_threshold(gport, dir, threshold));

    return CTC_E_NONE;
}

/**
 @brief Set vlan mapping enable on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_vlan_mapping_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_vlan_mapping_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get vlan mapping enable on the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_vlan_mapping_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_vlan_mapping_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set enable/disable vlan switching on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_vlan_switching_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_vlan_switching_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get enable/disable vlan switching on the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolean value denote the enable/disable

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_vlan_switching_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_vlan_switching_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set vlan classification enable on the port

 @param[in] gport Global phyical port of system

 @param[in] type Vlan classifcation type, maybe mac-based or ip-based vlan

 @param[in] policy_id The policy can be setting same on ports to share the same rule

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t type)
{
    CTC_ERROR_RETURN(sys_humber_port_set_vlan_classify_enable(gport, type));

    return CTC_E_NONE;
}

/**
 @brief The function is to disable vlan classification on the port

 @param[in] gport Global phyical port of system

 @param[in] type Vlan classifictiaon type, maybe mac-based or ip-based vlan

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_vlan_classify_disable(uint16 gport, ctc_vlan_class_type_t type)
{
    CTC_ERROR_RETURN(sys_humber_port_set_vlan_classify_disable(gport, type));

    return CTC_E_NONE;
}

/**
 @brief The fucntion is to get vlan classification type and policy on the port

 @param[in] gport Global phyical port of system

 @param[out] type Vlan classifictiaon type, maybe mac-based or ip-based vlan

 @param[out] policy_id The policy settled by user.

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t* type)
{
    CTC_ERROR_RETURN(sys_humber_port_get_vlan_classify_enable(gport, type));

    return CTC_E_NONE;
}

/**
 @brief Enable/disable ip source guard on the port

 @param[in] gport Global phyical port of system

 @param[in] enable A boolean value denote the enable/disable

 @return SDK_E_XXX
*/
int32
ctc_humber_port_set_ipsg_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_ipsg_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get ip source guard state of the port

 @param[in] gport Global phyical port of system

 @param[out] enable A boolean value denote the enable/disable

 @return SDK_E_XXX
*/
int32
ctc_humber_port_get_ipsg_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_ipsg_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set port mac enable

 @param[in] gport   Global phyical port of system

 @param[in] enable  A boolean value denote port mac enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_mac_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_mac_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port mac enable

 @param[in] gport   Global phyical port of system

 @param[out] enable  A boolean value denote port mac enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_mac_en(uint16 gport, bool * enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_mac_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set port speed mode

 @param[in] gport       Global phyical port of system

 @param[in] speed_mode  Speed at 10M 100M 1G of Gmac

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_speed(uint16 gport, ctc_port_speed_t speed_mode)
{
    CTC_ERROR_RETURN(sys_humber_port_set_speed(gport, speed_mode));

    return CTC_E_NONE;
}

/**
 @brief Set port speed mode

 @param[in] gport       Global phyical port of system

 @param[out] speed_mode Speed at 10M 100M 1G of Gmac

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_speed(uint16 gport, ctc_port_speed_t* speed_mode)
{
    CTC_ERROR_RETURN(sys_humber_port_get_speed(gport, speed_mode));

    return CTC_E_NONE;
}

/**
 @brief Set max frame size of system, two value can be configure

 @param[in] index   indicate index of max frame to be set

 @param[in] value   max frame size

 @return CTC_E_XXX
*/
int32
ctc_humber_set_max_frame_size(ctc_frame_size_t index, uint16 value)
{
    CTC_ERROR_RETURN(sys_humber_set_max_frame_size(index, value));

    return CTC_E_NONE;
}

/**
 @brief Get max frame size of system, may be one of index value

 @param[out] max_size  max frame size

 @return CTC_E_XXX
*/
int32
ctc_humber_get_max_frame_size(ctc_frame_size_t index, uint16* max_size)
{
    CTC_ERROR_RETURN(sys_humber_get_max_frame_size(index, max_size));

    return CTC_E_NONE;
}

/**
 @brief Set cpu mac enable

 @param[in] enable   a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_set_cpu_mac_en(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_set_cpu_mac_en(enable));

    return CTC_E_NONE;
}

/**
 @brief Get cpu mac enable

 @param[out] enable    a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_get_cpu_mac_en(bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_get_cpu_mac_en(enable));

    return CTC_E_NONE;
}

/**
 @brief Set max frame size per port

 @param[in] gport   global phyical port

 @param[in] index   size index

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_max_frame(uint16 gport, ctc_frame_size_t index)
{
    CTC_ERROR_RETURN(sys_humber_port_set_max_frame(gport, index));

    return CTC_E_NONE;
}

/**
 @brief Set max frame size per port

 @param[in] gport   global phyical port

 @param[out] index  size index

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_max_frame(uint16 gport, ctc_frame_size_t* index)
{
    CTC_ERROR_RETURN(sys_humber_port_get_max_frame(gport, index));

    return CTC_E_NONE;
}

/**
 @brief Set flow control of port

 @param[in] gport   global phyical port

 @param[in] dir     denote receive or transmit pause frame

 @param[in] enable  a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_flow_ctl_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief Get flow control of port

 @param[in] gport   global phyical port

 @param[in] dir     denote receive or transmit pause frame

 @param[out] enable a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_flow_ctl_en(gport, dir, enable));

    return CTC_E_NONE;
}

/**
 @brief Set port preamble

 @param[in] gport       global phyical port

 @param[in] pre_bytes   preamble value, for xgma/sgmac can only be 4bytes and 8bytes

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_preamble(uint16 gport, uint8 pre_bytes)
{
    CTC_ERROR_RETURN(sys_humber_port_set_preamble(gport, pre_bytes));

    return CTC_E_NONE;
}

/**
 @brief Get port preamble

 @param[in] gport       global phyical port

 @param[out] pre_bytes   preamble value, for xgma/sgmac can only be 4bytes and 8bytes

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_preamble(uint16 gport, uint8* pre_bytes)
{
    CTC_ERROR_RETURN(sys_humber_port_get_preamble(gport, pre_bytes));

    return CTC_E_NONE;
}

/**
 @brief Set port min frame size

 @param[in] gport       global phyical port

 @param[in] size   min frame size, not support xgma/sgmac

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_min_frame_size(uint16 gport, uint8 size)
{
    CTC_ERROR_RETURN(sys_humber_port_set_min_frame_size(gport, size));

    return CTC_E_NONE;
}

/**
 @brief Get port min frame size

 @param[in] gport       global phyical port

 @param[out] size   min frame size, not support xgma/sgmac

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_min_frame_size(uint16 gport, uint8* size)
{
    CTC_ERROR_RETURN(sys_humber_port_get_min_frame_size(gport, size));

    return CTC_E_NONE;
}

/**
 @brief Set port stretch mode

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_stretch_mode_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_stretch_mode_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port stretch mode

 @param[in] gport       global phyical port

 @param[out] enable     a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_stretch_mode_en(uint16 gport, bool* enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_stretch_mode_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set port pading (if packet length less than 63bytes, packet length will be paded to be 64bytes);

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_set_pading_en(uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_pading_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port pading (if packet length less than 63bytes, packet length will be paded to be 64bytes);

 @param[in] gport       global phyical port

 @param[in] enable      a boolean value denote the function enable or not

 @return CTC_E_XXX
*/
int32
ctc_humber_port_get_pading_en(uint16 gport, bool *enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_pading_en(gport, enable));

    return CTC_E_NONE;
}


/**
 @brief set port whether the srcdiscard is enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_srcdiscard_en(uint16 gport, bool enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC,
    "Arument info: gport:%d, srcdiscard_enable:%d!\n", gport, enable);

    CTC_ERROR_RETURN(sys_humber_port_set_srcdiscard_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port whether the srcdiscard is enable

 @param[in] gport global port of the system

 @param[out] a boolean value denote the function enable or not

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_srcdiscard_en(uint16 gport, bool *enable)
{
    CTC_DEBUG_OUT_INFO(port, port, PORT_CTC, "Arument info: gport:%d!\n", gport);

    CTC_ERROR_RETURN(sys_humber_port_get_srcdiscard_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Set port loopback

 @param[in] p_port_lbk point to  ctc_port_lbk_param_t

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_loopback (ctc_port_lbk_param_t *p_port_lbk)
{
    CTC_ERROR_RETURN(sys_humber_port_set_loopback(p_port_lbk));

    return CTC_E_NONE;
}

/**
 @brief Set port port check enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to set

 @return CTC_E_XXX

*/
int32
ctc_humber_port_set_port_check_en (uint16 gport, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_port_set_port_check_en(gport, enable));

    return CTC_E_NONE;
}

/**
 @brief Get port port check enable

 @param[in] gport, global port of the system

 @param[in] enable, a boolean value wanted to get

 @return CTC_E_XXX

*/
int32
ctc_humber_port_get_port_check_en(uint16 gport, bool *enable)
{
    CTC_ERROR_RETURN(sys_humber_port_get_port_check_en(gport, enable));

    return CTC_E_NONE;
}
