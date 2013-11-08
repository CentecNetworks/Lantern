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
 * @brief This file calls the sdk APIs for adapter layer
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_const.h"
#include "ofp_port.h"
#include "ofp_flow.h"
#include "ofp_macro.h"

#include "hal.h"
#include "hal_gre_tunnel.h"

#include "ctc_api.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_port.h"
#include "sys_humber_stats.h"

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
 * Create decap nhid
 * @param[in]  iloop_port               Internal port for iloop
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_create_decap_nhid(uint16_ofp iloop_port, uint32_ofp nh_wo_key, uint32_ofp nh_w_key)
{
    ctc_loopback_nexthop_param_t nh_param;
    int32_ofp ret = 0;

    /* 1. alloc nhid*/

    memset(&nh_param, 0, sizeof(nh_param));
    nh_param.customerid_valid        = FALSE;
    nh_param.inner_packet_type_valid = TRUE;
    nh_param.inner_packet_type       = CTC_PARSER_PKT_TYPE_ETHERNET;
    nh_param.lpbk_lport              = iloop_port;
    nh_param.words_removed_from_hdr  = 0;

    /* 2. create nexthop for decap tunnel packet without gre key*/
    HAL_ERROR_RETURN(ctc_iloop_nh_create(nh_wo_key, &nh_param));

    memset(&nh_param, 0, sizeof(nh_param));
    nh_param.customerid_valid        = TRUE;
    nh_param.inner_packet_type_valid = TRUE;
    nh_param.inner_packet_type       = CTC_PARSER_PKT_TYPE_ETHERNET;
    nh_param.lpbk_lport              = iloop_port;
    nh_param.words_removed_from_hdr  = 0;

    /* 3. create nexthop for decap tunnel packet with gre key*/
    ret = ctc_iloop_nh_create(nh_w_key, &nh_param);
    if (ret)
    {
        ctc_iloop_nh_remove(nh_wo_key);
        HAL_ERROR_RETURN(ret);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Remove decap nhid
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_remove_decap_nhid(uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key)
{
    if (nhid_wo_key)
    {
        HAL_ERROR_RETURN(ctc_iloop_nh_remove(nhid_wo_key));
    }
    if (nhid_w_key)
    {
        HAL_ERROR_RETURN(ctc_iloop_nh_remove(nhid_w_key));
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Add route entry for gre decapsulation
 * @param[in]  local_ip                 Local  ip(ipda) in the route entry
 * @param[in]  remote_ip                Remote ip(ipsa) in the route entry
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_decap_route(uint32_ofp local_ip, uint32_ofp remote_ip, uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key)
{
    ctc_ipuc_param_t ipuc_info_wk;
    ctc_ipuc_param_t ipuc_info_wok;
    int32_ofp ret = 0;

    /* 1. add route to decap tunnel packet without gre key*/
    memset(&ipuc_info_wok, 0, sizeof(ctc_ipuc_param_t));
    ipuc_info_wok.is_tunnel  = TRUE;
    ipuc_info_wok.ip_ver     = CTC_IP_VER_4;
    ipuc_info_wok.tunnel_info.tunnel_packet_type = CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE;
    ipuc_info_wok.tunnel_info.is_set_gre_key     = 0;
    ipuc_info_wok.nh_id      = nhid_wo_key;
    ipuc_info_wok.ip.ipv4    = local_ip;
    ipuc_info_wok.is_ipsa    = 1;
    ipuc_info_wok.ip_sa.ipv4 = remote_ip;
    ipuc_info_wok.masklen    = OFP_IPUC_V4_MAX_MASK_LEN;
    HAL_ERROR_RETURN(ctc_ipuc_add(&ipuc_info_wok));

    /* 2. add route to decap tunnel packet with gre key*/
    memset(&ipuc_info_wk, 0, sizeof(ctc_ipuc_param_t));
    ipuc_info_wk.is_tunnel  = TRUE;
    ipuc_info_wk.ip_ver     = CTC_IP_VER_4;
    ipuc_info_wk.tunnel_info.tunnel_packet_type = CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE;
    ipuc_info_wk.tunnel_info.is_set_gre_key     = 1;
    ipuc_info_wk.nh_id      = nhid_w_key;
    ipuc_info_wk.ip.ipv4    = local_ip;
    ipuc_info_wk.is_ipsa    = 1;
    ipuc_info_wk.ip_sa.ipv4 = remote_ip;
    ipuc_info_wk.masklen    = OFP_IPUC_V4_MAX_MASK_LEN;
    ret = ctc_ipuc_add(&ipuc_info_wk);
    if (ret)
    {
        ctc_ipuc_remove(&ipuc_info_wok);
        HAL_ERROR_RETURN(ret);
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Remove route entry for gre decapsulation
 * @param[in]  local_ip                 Local  ip(ipda) in the route entry
 * @param[in]  remote_ip                Remote ip(ipsa) in the route entry
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_del_decap_route(uint32_ofp local_ip, uint32_ofp remote_ip, uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key)
{
    ctc_ipuc_param_t ipuc_info;

    /* 1. Remove route entry that was added to decap tunnel packet without gre key */
    memset(&ipuc_info, 0, sizeof(ctc_ipuc_param_t));
    ipuc_info.is_tunnel  = TRUE;
    ipuc_info.ip_ver     = CTC_IP_VER_4;
    ipuc_info.tunnel_info.tunnel_packet_type = CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE;
    ipuc_info.tunnel_info.is_set_gre_key     = 0;
    ipuc_info.nh_id      = nhid_wo_key;
    ipuc_info.ip.ipv4    = local_ip;
    ipuc_info.is_ipsa    = 1;
    ipuc_info.ip_sa.ipv4 = remote_ip;
    ipuc_info.masklen    = OFP_IPUC_V4_MAX_MASK_LEN;
    HAL_ERROR_RETURN(ctc_ipuc_remove(&ipuc_info));

    /* 2. Remove route entry that was added to decap tunnel packet with gre key */
    memset(&ipuc_info, 0, sizeof(ctc_ipuc_param_t));
    ipuc_info.is_tunnel  = TRUE;
    ipuc_info.ip_ver     = CTC_IP_VER_4;
    ipuc_info.tunnel_info.tunnel_packet_type = CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE;
    ipuc_info.tunnel_info.is_set_gre_key     = 1;
    ipuc_info.nh_id      = nhid_w_key;
    ipuc_info.ip.ipv4    = local_ip;
    ipuc_info.is_ipsa    = 1;
    ipuc_info.ip_sa.ipv4 = remote_ip;
    ipuc_info.masklen    = OFP_IPUC_V4_MAX_MASK_LEN;
    HAL_ERROR_RETURN(ctc_ipuc_remove(&ipuc_info));

    return OFP_ERR_SUCCESS;
}

/**
 * Alloc internal port
 * @param[out]  iloop_port              Pointer to internal port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_alloc_internal_port(uint16_ofp* p_internal_port)
{
    ctc_internal_port_assign_para_t port_assign;

    HAL_PTR_CHECK(p_internal_port);

    memset(&port_assign, 0, sizeof(port_assign));
    port_assign.gchip = 0;
    port_assign.type  = CTC_INTERNAL_PORT_TYPE_ILOOP;

    HAL_ERROR_RETURN(ctc_internal_port_allocate(&port_assign));
    *p_internal_port = port_assign.inter_port;

    return OFP_ERR_SUCCESS;
}

/**
 * Release internal port
 * @param[in]  iloop_port               Internal port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_release_internal_port(uint16_ofp internal_port)
{
    ctc_internal_port_assign_para_t port_assign;

    memset(&port_assign, 0, sizeof(port_assign));
    port_assign.gchip = 0;
    port_assign.type  = CTC_INTERNAL_PORT_TYPE_ILOOP;
    port_assign.inter_port = internal_port;

    HAL_ERROR_RETURN(ctc_internal_port_release(&port_assign));

    return OFP_ERR_SUCCESS;
}

/**
 * Add default route entry to send packets to cpu via miss-match reason
 * @param[out]  p_nhid                  Pointer to nexthop id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_default_route_entry(uint32_ofp* p_nhid)
{
    ctc_ipuc_param_t ipuc_info;
    uint32_ofp nhid;

    HAL_ERROR_RETURN(ctc_l2_create_ucast_nh(CPU_PORT_NUM, CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS));
    HAL_ERROR_RETURN(ctc_l2_get_ucast_nh(CPU_PORT_NUM, CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS, &nhid));
    HAL_ERROR_RETURN(sys_humber_nh_add_stats_action(nhid));

    memset(&ipuc_info, 0, sizeof(ctc_ipuc_param_t));

    ipuc_info.nh_id  = nhid;
    ipuc_info.ip_ver = CTC_IP_VER_4;
    ipuc_info.is_tunnel = TRUE;
    ipuc_info.tunnel_info.tunnel_packet_type = CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE;
    ipuc_info.tunnel_info.is_set_gre_key = 1;

    HAL_ERROR_RETURN(ctc_ipuc_add_default_entry(&ipuc_info));

    *p_nhid = nhid;

    return OFP_ERR_SUCCESS;
}

/**
 * Add default route entry to Matching GRE packets for routing processing to decapsulation
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_default_qos_entry(void)
{
    ctc_aclqos_entry_t ipv4_entry_gre;
    uint16_ofp stats_ptr = SPECIAL_STATS_PTR;

    /* insert ipv4 default qos entry matching gre packets, action: routing */
    kal_memset(&ipv4_entry_gre, 0, sizeof(ctc_aclqos_entry_t));
    ipv4_entry_gre.entry_id = ENTRY_ID_GRE_DEFAULT;
    ipv4_entry_gre.key.type = CTC_ACLQOS_IPV4_KEY;
    SET_FLAG(ipv4_entry_gre.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPSA_FLAG);
    SET_FLAG(ipv4_entry_gre.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPDA_FLAG);
    SET_FLAG(ipv4_entry_gre.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_ROUTEDPKT_FLAG);
    SET_FLAG(ipv4_entry_gre.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_L4PROTO_FLAG);
    ipv4_entry_gre.key.key_info.ipv4_key.l4_protocol = IP_PROTO_GRE;
    ipv4_entry_gre.key.key_info.ipv4_key.routed_packet = 1;

    HAL_ERROR_RETURN(sys_humber_stats_create_statsptr(0, 1, &stats_ptr));
    SET_FLAG(ipv4_entry_gre.action.flag, CTC_ACLQOS_ACTION_STATS_FLAG);
    ipv4_entry_gre.action.stats_ptr = stats_ptr;

    SET_FLAG(ipv4_entry_gre.action.flag,   CTC_ACLQOS_ACTION_DENY_BRIDGE_FLAG);
    SET_FLAG(ipv4_entry_gre.action.flag,   CTC_ACLQOS_ACTION_DENY_LEARN_FLAG);
    UNSET_FLAG(ipv4_entry_gre.action.flag, CTC_ACLQOS_ACTION_DENY_ROUTE_FLAG);

    HAL_ERROR_RETURN(
        ctc_qos_entry_add(CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL, CTC_ACLQOS_ENTRY_ID_TAIL, &ipv4_entry_gre));

    return OFP_ERR_SUCCESS;
}

/**
 * Enable iloop internal port for packet process after iloop
 * @param[in]  iloop_port               Internal port
 * @param[in]  enable                   true/false
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_enable_iloop_port(uint16_ofp iloop_port, bool enable)
{
    if (enable)
    {
        HAL_ERROR_RETURN(ctc_port_set_transmit_en(iloop_port, true));
        HAL_ERROR_RETURN(ctc_port_set_receive_en(iloop_port, true));

        HAL_ERROR_RETURN(ctc_port_set_bridge_en(iloop_port, true));
        HAL_ERROR_RETURN(sys_humber_port_set_bridge(iloop_port, CTC_INGRESS, false));
        HAL_ERROR_RETURN(ctc_port_set_learning_en(iloop_port, false));

        /* These features are not need to unset when port disabled */
        HAL_ERROR_RETURN(ctc_port_set_vlanctl(iloop_port, CTC_VLANCTL_ALLOW_ALL_PACKETS));
        HAL_ERROR_RETURN(ctc_port_set_dot1q_type(iloop_port, CTC_DOT1Q_TYPE_BOTH));
        HAL_ERROR_RETURN(ctc_port_set_keep_vlan_tag (iloop_port, true));
        HAL_ERROR_RETURN(ctc_port_set_src_outer_is_svlan(iloop_port, true));
        HAL_ERROR_RETURN(ctc_port_set_outer_is_cvlan(iloop_port, false));
        HAL_ERROR_RETURN(ctc_port_set_stag_tpid_index(iloop_port, CTC_BOTH_DIRECTION, 2));

        /* Set port default vlan id to reserved vlan_id: 4095. */
        HAL_ERROR_RETURN(ctc_port_set_default_vlan(iloop_port, OFP_DEFAULT_VLAN_ID));

    }
    else
    {
        HAL_ERROR_RETURN(ctc_port_set_transmit_en(iloop_port, false));
        HAL_ERROR_RETURN(ctc_port_set_receive_en(iloop_port, false));

        HAL_ERROR_RETURN(ctc_port_set_bridge_en(iloop_port, false));
        HAL_ERROR_RETURN(ctc_port_set_learning_en(iloop_port, false));
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Create service id for acl process after decapsulation
 * @param[in]  service_id               Service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_create_service_id(uint32_ofp service_id)
{
    HAL_ERROR_RETURN(ctc_service_label_create(service_id));

    return OFP_ERR_SUCCESS;
}

/**
 * Remove service id
 * @param[in]  service_id               Service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_remove_service_id(uint32_ofp service_id)
{
    HAL_ERROR_RETURN(ctc_service_label_remove(service_id));

    return OFP_ERR_SUCCESS;
}
