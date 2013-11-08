/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file calls the sdk port APIs
 */

/******************************************************************************
* Header Files
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_const.h"
#include "ofp_macro.h"
#include "ofp_stats.h"
#include "ofp_port.h"
#include "ofp_netdev_api.h"

#include "glb_l2_define.h"
#include "hal.h"
#include "hal_port.h"
#include "hal_pdu.h"


#include "ctc_api.h"
#include "sys_humber_port.h"
#include "glb_qos_define.h"
#include "sys_humber_queue_drop.h"

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
 * Function
 *
 ****************************************************************************/

/**
* Set port config
* @param gport                          gport
* @param port_config                    port config
* @return OFP_ERR_XX
*/
int32
hal_port_set_config(uint16_ofp gport, uint32_ofp port_config)
{
    if (port_config & OFPPC_NO_FWD )
    {
        HAL_ERROR_RETURN(ctc_port_set_transmit_en(gport, false));
    }
    else
    {
        HAL_ERROR_RETURN(ctc_port_set_transmit_en(gport, true));
    }

    if (port_config & OFPPC_NO_RECV)
    {
        HAL_ERROR_RETURN(ctc_port_set_receive_en(gport, false));
        if (port_config & OFPPC10_NO_RECV_STP)
        {
            HAL_ERROR_RETURN(hal_l2pdu_set_stp_packet_action(gport, OFP_PDU_L2PDU_ACTION_TYPE_DISCARD));
        }
        else
        {
            HAL_ERROR_RETURN(hal_l2pdu_set_stp_packet_action(gport, OFP_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU));
        }
    }
    else
    {
        HAL_ERROR_RETURN(ctc_port_set_receive_en(gport, true));
        if (port_config & OFPPC10_NO_RECV_STP)
        {
            HAL_ERROR_RETURN(hal_l2pdu_set_stp_packet_action(gport, OFP_PDU_L2PDU_ACTION_TYPE_DISCARD));
        }
        else
        {
            HAL_ERROR_RETURN(hal_l2pdu_set_stp_packet_action(gport, OFP_PDU_L2PDU_ACTION_TYPE_FWD));
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
* Reset port statistics
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32
hal_port_reset_mac_stats(uint16_ofp gport)
{
    HAL_ERROR_RETURN(ctc_stats_reset_mac_stats(gport, CTC_STATS_MAC_STATS_TX));
    HAL_ERROR_RETURN(ctc_stats_reset_mac_stats(gport, CTC_STATS_MAC_STATS_RX));

    return OFP_ERR_SUCCESS;
}

/**
* Get port statistics
* @param[in]  gport                     gport
* @param[out] p_stats                   Pointer to stats data structure
* @return OFP_ERR_XX
*/
int32
hal_port_get_mac_stats(uint16_ofp gport, ofp_if_stats_t* p_stats)
{
    HAL_ERROR_RETURN(ctc_stats_get_mac_stats(gport, CTC_STATS_MAC_STATS_TX,
        (ctc_mac_stats_t *)&p_stats->tx_stats));
    HAL_ERROR_RETURN(ctc_stats_get_mac_stats(gport, CTC_STATS_MAC_STATS_RX,
        (ctc_mac_stats_t *)&p_stats->rx_stats));

    return OFP_ERR_SUCCESS;
}
/**
* Enable port qos
* @param gport                          gport
* @return OFP_ERR_XX
*/
static int32_ofp
hal_port_set_queue_default(uint16_ofp gport)
{
    uint16_ofp qid = 0;
    uint16_ofp prec = 0;
    ctc_queue_drop_t drop;

    memset(&drop, 0, sizeof (drop));

    drop.mode = CTC_QUEUE_DROP_WTD;
    drop.weight= 0;

    for (prec = 0; prec < GLB_QOS_DROP_PREC_NUM; prec ++)
    {
        drop.max_th[prec] = SYS_DROP_COUNT_GE + prec * SYS_DROP_DELTA_GE;
    }

    for (qid = 0; qid < OFP_MAX_QUEUE_VALUE; qid ++)
    {
        /* set queue class */
        HAL_ERROR_RETURN(ctc_set_port_queue_class(gport, qid, 0));

        /* set queue threshold */
        HAL_ERROR_RETURN(ctc_set_port_queue_drop(gport, qid, &drop));
    }

    /*queue 0 is high priority. other queue (1~7) is low priority*/
    HAL_ERROR_RETURN(ctc_set_port_queue_class(gport, 0, 3));

    return OFP_ERR_SUCCESS;
}

/**
* Enable port qos
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_enable_qos(uint16_ofp gport)
{
    ctc_direction_t dir = CTC_INGRESS;
    uint32 qos_label = 0;

    qos_label = BASE_PORT_LABEL_ID + gport;
    HAL_ERROR_RETURN(ctc_qos_port_label_create(qos_label));

    /* create qos port label */
    HAL_ERROR_RETURN(ctc_qos_port_label_set(gport, dir, qos_label));
    HAL_ERROR_RETURN(ctc_qos_port_enable(gport, dir, true));

    HAL_ERROR_RETURN(hal_port_set_queue_default(gport));

    return OFP_ERR_SUCCESS;
}

/**
* Disable port qos
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_disable_qos(uint16_ofp gport)
{
    ctc_direction_t dir = CTC_INGRESS;
    uint32 qos_label = 0;

    qos_label = BASE_PORT_LABEL_ID + gport;
    HAL_ERROR_RETURN(ctc_qos_port_enable(gport, dir, false));

    HAL_ERROR_RETURN(ctc_qos_set_port_replace_enable(gport, CTC_QOS_REPLACE_COS, false));
    HAL_ERROR_RETURN(ctc_qos_set_port_replace_enable(gport, CTC_QOS_REPLACE_DSCP, false));

    HAL_ERROR_RETURN(ctc_qos_entry_remove_all(qos_label, CTC_ACLQOS_MAC_KEY));
    HAL_ERROR_RETURN(ctc_qos_entry_remove_all(qos_label, CTC_ACLQOS_IPV4_KEY));

    HAL_ERROR_RETURN(ctc_qos_port_label_unset(gport, dir));
    HAL_ERROR_RETURN(ctc_qos_port_label_remove(qos_label));

    return OFP_ERR_SUCCESS;
}

/**
* Set port user id enable
* @param gport                          gport
* @param enable                         true/false
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_user_id_enable(uint16_ofp gport, bool enable)
{
    HAL_ERROR_RETURN(ctc_port_set_vlan_mapping_en(gport, enable));

    return OFP_ERR_SUCCESS;
}

/**
* Init l3if
* @param gport                          gport
* @param l3ifid                         l3ifid
* @param ethaddr                        port ethernet address
* @return OFP_ERR_XX
*/
static int32_ofp
hal_port_init_l3if(uint16_ofp gport, uint16_ofp l3ifid, uint8_ofp ethaddr[OFP_ETH_ADDR_LEN])
{
    ctc_l3if_t l3if_info;

    /* 1. create l3if for gre tunnel decapsulation process*/
    memset(&l3if_info, 0, sizeof(l3if_info));
    l3if_info.gport     = gport;
    l3if_info.l3if_type = CTC_L3IF_TYPE_PHY_IF;
    l3if_info.vlan_id   = OFP_INVALID_VLANID;
    HAL_ERROR_RETURN(ctc_l3if_create(l3ifid, &l3if_info));

    /* 2. set router mac in epe */
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid,
        CTC_L3IF_PROP_EGS_MAC_SA_PREFIX_TYPE, 0));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid,
        CTC_L3IF_PROP_EGS_MAC_SA_LOW_8BITS, ethaddr[CTC_ETH_ADDR_LEN - 1]));

    /* 3. set l3if attr */
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_ROUTE_EN,   FALSE));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_IPV4_UCAST, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_IPV4_MCAST, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_MTU_EN,     FALSE));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_RECEIVE_EN, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_TANSMIT_EN, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_ROUTE_ALL_PKT, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_ARP_EXCEPTION_TYPE, CTC_EXCP_NORMAL_FWD));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_DHCP_EXCEPTION_TYPE, CTC_EXCP_NORMAL_FWD));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_MPLS_EN, TRUE ));
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_MTU_SIZE, OFP_DEFAULT_MAX_JUMBO_FRAME_SIZE));
    HAL_ERROR_RETURN(ctc_l3pdu_set_l3if_action(l3ifid,
        OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU, CTC_PDU_L3PDU_ACTION_TYPE_FWD));
    HAL_ERROR_RETURN(ctc_l3pdu_set_l3if_action(l3ifid,
        OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_ARP_TO_CPU, CTC_PDU_L3PDU_ACTION_TYPE_FWD));
    HAL_ERROR_RETURN(ctc_l3pdu_set_l3if_action(l3ifid,
        OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_DHCP_TO_CPU, CTC_PDU_L3PDU_ACTION_TYPE_FWD));

    return OFP_ERR_SUCCESS;
}

/**
* Init port
* @param gport                          gport
* @param l3ifid                         l3ifid
* @param ethaddr                        port ethernet address
* @return OFP_ERR_XX
*/
int32
hal_port_enable_openflow(uint16_ofp gport, uint16_ofp l3ifid, uint8_ofp ethaddr[6])
{
    ctc_vlantag_ctl_t vlantag_ctl;

    HAL_ERROR_RETURN(ctc_port_set_transmit_en(gport, true));
    HAL_ERROR_RETURN(ctc_port_set_receive_en(gport, true));

    HAL_ERROR_RETURN(ctc_port_set_bridge_en(gport, true));
    HAL_ERROR_RETURN(sys_humber_port_set_bridge(gport, CTC_INGRESS, false));
    HAL_ERROR_RETURN(ctc_port_set_learning_en(gport, false));

    vlantag_ctl = CTC_VLANCTL_ALLOW_ALL_PACKETS;
    HAL_ERROR_RETURN(ctc_port_set_vlanctl(gport, vlantag_ctl));
    HAL_ERROR_RETURN(ctc_port_set_vlan_filter_en(gport, CTC_BOTH_DIRECTION, false));
    HAL_ERROR_RETURN(ctc_port_set_dot1q_type(gport, CTC_DOT1Q_TYPE_BOTH));
    HAL_ERROR_RETURN(ctc_port_set_keep_vlan_tag (gport, true));
    HAL_ERROR_RETURN(ctc_port_set_src_outer_is_svlan(gport, true));
    HAL_ERROR_RETURN(ctc_port_set_outer_is_cvlan(gport, false));
    HAL_ERROR_RETURN(ctc_port_set_stag_tpid_index(gport, CTC_BOTH_DIRECTION, OFP_SVLAN_DFT_TPID_IDX));
    HAL_ERROR_RETURN(ctc_port_set_flow_ctl_en(gport, CTC_INGRESS, false));
    HAL_ERROR_RETURN(ctc_port_set_flow_ctl_en(gport, CTC_EGRESS, false));
    HAL_ERROR_RETURN(ctc_qos_set_port_replace_enable(gport, CTC_QOS_REPLACE_COS, true));

    HAL_ERROR_RETURN(ctc_set_max_frame_size(CTC_FRAME_SIZE_1, OFP_DEFAULT_MAX_JUMBO_FRAME_SIZE));
    HAL_ERROR_RETURN(ctc_port_set_max_frame(gport, CTC_FRAME_SIZE_1));
    /* need to enable padding bits when packet < 64 bytes */
    HAL_ERROR_RETURN(ctc_port_set_pading_en(gport, true));

    HAL_ERROR_RETURN(hal_port_init_l3if(gport, l3ifid, ethaddr));
    HAL_ERROR_RETURN(ctc_port_set_phy_if_en(gport, true));

    /* Set port default vlan id to reserved vlan_id: 4095. */
    HAL_ERROR_RETURN(ctc_port_set_default_vlan(gport, OFP_DEFAULT_VLAN_ID));

    /* packet can't forward to ingress port if this property isn't enable*/
    HAL_ERROR_RETURN(ctc_port_set_reflective_bridge_en(gport, true));

    return OFP_ERR_SUCCESS;
}

int32
hal_port_disable_openflow(uint16_ofp gport, uint16_ofp l3ifid)
{
    ctc_l3if_t l3if_info;
    HAL_ERROR_RETURN(hal_port_disable_qos(gport));

    memset(&l3if_info, 0, sizeof(l3if_info));
    l3if_info.gport     = gport;
    l3if_info.l3if_type = CTC_L3IF_TYPE_PHY_IF;
    l3if_info.vlan_id   = OFP_INVALID_VLANID;
    HAL_ERROR_RETURN(ctc_l3if_delete(l3ifid, &l3if_info));
    /*set before ctc_port_set_phy_if_en(gport, false), this function will make
    p_port_master->igs_port_prop[lchip][lport].default_vlan
    p_port_master->egs_port_prop[lchip][lport].default_vlan
    and sys_humber_port_set_default_vlan will return*/
    HAL_ERROR_RETURN(ctc_port_set_default_vlan(gport, HYBRID_DEFAULT_VLAN_ID));
    HAL_ERROR_RETURN(ctc_port_set_phy_if_en(gport, false));

    return OFP_ERR_SUCCESS;
}

/**
* Set l3if route en
* @param l3ifid                         l3ifid
* @param enable                         true/false
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_l3if_route_en(uint16_ofp l3ifid, bool enable)
{
    HAL_ERROR_RETURN(ctc_l3if_set_property(l3ifid, CTC_L3IF_PROP_ROUTE_EN, enable));

    return OFP_ERR_SUCCESS;
}

/**
* Set l3if vmac prefix
* @param mac_40bit                      route mac high 40 bits
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_l3if_vmac(uint8_ofp mac_40bit[OFP_ETH_ADDR_LEN])
{
    HAL_ERROR_RETURN(ctc_l3if_set_vmac_prefix(CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE0, mac_40bit));

    return OFP_ERR_SUCCESS;
}

/**
* Set port phy enable
* @param gport                          Global port
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_phy_enable(uint16_ofp gport, bool enable)
{
#ifndef _OFP_UML_
    uint16_ofp dev_port_id;

    dev_port_id = lcm_get_port_from_port_unmapping(gport);
    HAL_ERROR_RETURN(lcm_set_interface_enable(dev_port_id, enable));
#endif
    return OFP_ERR_SUCCESS;
}

/**
* Get port phy speed
* @param gport      Global port
* @param speed     Port speed
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_phy_speed(uint16_ofp gport, uint32_ofp *speed)
{
#ifndef _OPEN_SOURCE_
    uint16_ofp dev_port_id;

    dev_port_id = lcm_get_port_from_port_unmapping(gport);
    HAL_ERROR_RETURN(lcm_get_interface_speed(dev_port_id, speed));
#endif

    return OFP_ERR_SUCCESS;
}

/**
* Get port auto negotiate speed
* @param gport      Global port
* @param speed      Port speed
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_autonego_speed(uint16_ofp gport, uint32_ofp *speed)
{
#ifndef _OPEN_SOURCE_
    uint16_ofp dev_port_id;

    dev_port_id = lcm_get_port_from_port_unmapping(gport);
    HAL_ERROR_RETURN(lcm_get_interface_autonego_speed(dev_port_id, speed));
#endif

    return OFP_ERR_SUCCESS;
}

/**
* Get port phy duplex
* @param gport      Global port
* @param duplex    Port duplex
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_phy_duplex(uint16_ofp gport, uint32_ofp *duplex)
{
#ifndef _OPEN_SOURCE_
    uint16_ofp dev_port_id;

    dev_port_id = lcm_get_port_from_port_unmapping(gport);
    HAL_ERROR_RETURN(lcm_get_interface_duplex(dev_port_id, duplex));
#endif

    return OFP_ERR_SUCCESS;
}

/**
* Get system mac
* @param[out]                           system mac
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_system_mac(uint8_ofp system_mac[OFP_ETH_ADDR_LEN])
{
#ifndef _OPEN_SOURCE_
   HAL_ERROR_RETURN(lcm_get_system_mac(system_mac));
#endif

    return OFP_ERR_SUCCESS;
}
