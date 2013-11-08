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
 * @brief This file is the gre tunnel implementation of Centec open vswitch adapt layer
 */

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_flow.h"
#include "adpt_gre_tunnel.h"
#include "adpt_gre_tunnel_priv.h"
#include "adpt_port.h"
#include "adpt_nexthop.h"
#include "adpt_opf.h"

#include "hal.h"
#include "hal_gre_tunnel.h"
#include "hal_user_id.h"
#include "hal_nexthop.h"
#include "hal_port.h"

/****************************************************************************
 *
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_gre_tunnel);

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
adpt_tunnel_master_t* g_p_tunnel_master;

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

static int32_ofp
adpt_tunnel_init_tunnel_port_service_id(void)
{
    int tunnel_port;
    uint32_ofp service_id;

    for (tunnel_port = OFP_TUNNEL_PORT_NO_BASE + 1; tunnel_port < OFP_TUNNEL_PORT_NO_MAX; tunnel_port ++)
    {
        /* Create default service id to db */
        ADPT_ERROR_RETURN(adpt_tunnel_bind_service_id(tunnel_port, 0, &service_id));
    }

    return OFP_ERR_SUCCESS;
}

/**
 *  Add default userid vlan entry to process decapsulated packets
 *  by global openflow flow.
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_tunnel_add_default_userid_entry(void)
{
    /** refer to _sys_humber_usrid_vlan_init() */
    return OFP_ERR_SUCCESS;
}

/**
 * Alloc nhid for encapsulation
 * @param[in] ofport                    tunnel port
 * @param[in] p_combo                   tunnel id, strip svlan, strip cvlan in adpt_flow_action_combo_t
 * @param[in] p_nh_offset               pointer to ofp_nh_offset_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_alloc_encap_nhid(uint32_ofp ofport, adpt_flow_action_combo_t* p_combo, ofp_nh_offset_t* p_nh_offset)
{
    ctc_ip_nh_param_t nh_param;
    adpt_tunnel_info_t* p_tnl_info;
    uint32_ofp tunnel_id;
    int32 ret = 0;

    ADPT_PTR_CHECK(p_nh_offset);
    ADPT_PTR_CHECK(p_combo);
    ADPT_LOG_DEBUG_FUNC();
    tunnel_id = p_combo->tunnel_id;
    ADPT_LOG_DEBUG("ofport = %d, tunnel_id = %d\n", ofport, tunnel_id);

    memset(&nh_param, 0, sizeof(nh_param));

    p_tnl_info = adpt_tunnel_get_tunnel_info_by_ofport(ofport);
    ADPT_PTR_CHECK(p_tnl_info);

    /* 1. alloc nhid*/
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_nh_offset->nhid));
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, &p_nh_offset->offset));
    ADPT_LOG_DEBUG("nhid = %d, offset = %d\n", p_nh_offset->nhid, p_nh_offset->offset);

    /* 2. call sdk api to create nhid*/
    nh_param.dsnh_offset = p_nh_offset->offset;
    nh_param.opcode = CTC_IP_NH_OP_ETH_IN_GRE;
    nh_param.oif.gport = p_tnl_info->bind_port;
    nh_param.oif.vid   = p_tnl_info->vlan_id;
    nh_param.oif.oif_type = CTC_NH_OIF_TYPE_ROUTED_PORT;
    memcpy(nh_param.mac, p_tnl_info->nexthop_mac, sizeof(mac_addr_t));

    /* 2.1 set tunnel info */
    nh_param.tunnel_info.ttl = OFP_TUNNEL_DEFAULT_SET_TTL;
    nh_param.tunnel_info.encap_type  = CTC_IP_TUNNEL_NH_ENCAP_TYPE_GRE;
    nh_param.tunnel_info.dscp_type   = CTC_IP_NH_TUNNEL_DSCP_USE_USER_DEFINE;
    nh_param.tunnel_info.dscp_or_tos = OFP_TUNNEL_DEFAULT_SET_TOS;
    nh_param.tunnel_info.ip_da.ipv4  = p_tnl_info->remote_ip;
    nh_param.tunnel_info.ip_sa.ipv4  = p_tnl_info->local_ip;

    if (tunnel_id)
    {
        CTC_BIT_SET(nh_param.tunnel_info.encap_info.gre_info.gre_flag, CTC_IP_NH_TUNNEL_GRE_FLAG_KEY_VALID);
        nh_param.tunnel_info.encap_info.gre_info.gre_key  = tunnel_id;
    }

    if (IS_FLAG_SET(p_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN))
    {
        nh_param.tunnel_info.strip_svlan = TRUE;
    }
    if (IS_FLAG_SET(p_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN))
    {
        nh_param.tunnel_info.strip_cvlan = TRUE;
    }

    ret = hal_nexthop_create_ipuc_nh(p_nh_offset->nhid, &nh_param);
    if (ret)
    {
        adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, p_nh_offset->nhid);
        adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, p_nh_offset->offset);
        ADPT_ERROR_RETURN(ret);
    }

    p_nh_offset->nh_type = OPF_NH_TYPE_NH_IPUC;

    return 0;
}

static int32_ofp
adpt_tunnel_bind_ip(uint32_ofp local_ip, uint32_ofp remote_ip)
{
    int32_ofp ret = OFP_ERR_SUCCESS;

    ADPT_ERROR_RETURN(adpt_tunneldb_add_local_ip(local_ip));

    ret = adpt_tunneldb_add_local_remote_ip(local_ip, remote_ip);
    if (ret)
    {
        adpt_tunneldb_del_local_ip(local_ip);
        ADPT_ERROR_RETURN(ret);
    }
    
    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunnel_unbind_ip(uint32_ofp local_ip, uint32_ofp remote_ip)
{
    adpt_tunneldb_del_local_ip(local_ip);
    adpt_tunneldb_del_local_remote_ip(local_ip, remote_ip);

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunnel_set_bind_port_route_en(uint16_ofp bind_port, bool enable)
{
    uint16_ofp l3ifid;

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("bind_port = %d, enable = %s\n", bind_port, enable ? "true" : "false");

    ADPT_ERROR_RETURN(adpt_port_get_l3ifid(bind_port, &l3ifid));

    if (enable)
    {
        if (!adpt_tunneldb_has_bind_port(bind_port))
        {
            ADPT_ERROR_RETURN(hal_port_set_l3if_route_en(l3ifid, TRUE));
        }
        ADPT_ERROR_RETURN(adpt_tunneldb_add_bind_port(bind_port));
    }
    else
    {
        ADPT_ERROR_RETURN(adpt_tunneldb_del_bind_port(bind_port));
        if (!adpt_tunneldb_has_bind_port(bind_port))
        {
            ADPT_ERROR_RETURN(hal_port_set_l3if_route_en(l3ifid, FALSE));
        }
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunnel_alloc_service_id(uint32_ofp* p_service_id)
{
    ADPT_PTR_CHECK(p_service_id);

    ADPT_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_TNL_SERVICE_ID, 1, p_service_id));

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunnel_release_service_id(uint32_ofp service_id)
{
    ADPT_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_TNL_SERVICE_ID, 1, service_id));

    return OFP_ERR_SUCCESS;
}

/**
 * Lookup service id which was binded with tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_lookup_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp* p_service_id)
{
    /*
    * Note :
    * One service id for one combination of tunnel port and tunnel id.
    * The combination can be referenced by multiple flows.
    */
    ADPT_PTR_CHECK(p_service_id);

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("tunnel_port = %d, tunnel_id = %d\n", tunnel_port, tunnel_id);

    ADPT_ERROR_RETURN(adpt_tunneldb_lookup_service_id(tunnel_port, tunnel_id, p_service_id));

    return OFP_ERR_SUCCESS;
}

/**
 * Bind service id  with tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_bind_service_id(uint32_ofp tunnel_ofport, uint32_ofp tunnel_id, uint32_ofp* p_service_id)
{
    /*
    * Note :
    * One service id for one combination of tunnel port and tunnel id.
    * The combination can be referenced by multiple flows.
    * When adding flow should this function should be called once.
    */
    uint32_ofp new_service_id = 0;
    int32 ret;

    ADPT_PTR_CHECK(p_service_id);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("tunnel_ofport = %d, tunnel_id = %d\n", tunnel_ofport, tunnel_id);

    if (OFP_ERR_SUCCESS != adpt_tunneldb_lookup_service_id(tunnel_ofport, tunnel_id, &new_service_id))
    {
        adpt_tunnel_info_t* tunnel_info = NULL;

        ret = adpt_tunnel_alloc_service_id(&new_service_id);
        if (ret)
        {
            VLOG_ERR("Adding flow entry with tunnel port %d and tunnel id %d failed, only %d different combinations are supported",
                tunnel_ofport, tunnel_id, OFP_TUNNEL_MAX_SERVICE_ID_NUM);
            ADPT_ERROR_RETURN(OFP_ERR_TUNNEL_ID_FULL);
        }

        /* For first reference, we need create the service in sdk */
        ADPT_ERROR_RETURN(hal_tunnel_create_service_id(new_service_id));

        if (tunnel_id)
        {
            /* Add user id entry to generate service id for decapsulated packets */
            tunnel_info = adpt_tunnel_get_tunnel_info_by_ofport(tunnel_ofport);
            ADPT_PTR_CHECK(tunnel_info);

            ADPT_ERROR_RETURN(hal_user_id_add_vlan_entry(tunnel_info->iloop_port,
                tunnel_id, new_service_id));
        }
    }
    
    /* If the service id is created, just increase the reference counter */
    ADPT_ERROR_RETURN(adpt_tunneldb_add_service_id(tunnel_ofport, tunnel_id, new_service_id));

    *p_service_id = new_service_id;

    return OFP_ERR_SUCCESS;
}

/**
 * Unbind service id  with tunnel port and tunnel id
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_unbind_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id)
{
    /*
    * Note :
    * One service id for one combination of tunnel port and tunnel id.
    * The combination can be referenced by multiple flows.
    * When deleting flow should this function should be called once.
    */
    int32_ofp ret;
    uint32_ofp old_service_id;

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("tunnel_port = %d, tunnel_id = %d\n", tunnel_port, tunnel_id);

    adpt_tunneldb_del_service_id(tunnel_port, tunnel_id, service_id);

    ret = adpt_tunneldb_lookup_service_id(tunnel_port, tunnel_id, &old_service_id);
    /* If this service id is lasted referenced, remove service id */
    if (OFP_ERR_ENTRY_NOT_EXIST == ret)
    {
        adpt_tunnel_info_t* tunnel_info = NULL;

        adpt_tunnel_release_service_id(service_id);

        hal_tunnel_remove_service_id(service_id);
        if (tunnel_id)
        {
            tunnel_info = adpt_tunnel_get_tunnel_info_by_ofport(tunnel_port);
            ADPT_PTR_CHECK(tunnel_info);

            hal_user_id_del_vlan_entry(tunnel_info->iloop_port, tunnel_id, service_id);
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Check tunnel id when adding flow
 * @param[in] p_rule                    tunnel port
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_check_tunnel_id(const struct rule_ctc *p_rule)
{
    const struct flow *flow;
    const struct flow *masks;

    flow = &p_rule->match.flow;
    masks = &p_rule->match.wc.masks;

    if (masks->tunnel.tun_id == htonll(0))
    {
        return OFP_ERR_SUCCESS;
    }

    if (masks->tunnel.tun_id !=  htonll(OFP_UINT64_MAX))
    {
        OFP_LOG_ERROR("Unsupported flow fields: tun_id mask");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }

    if (ntohll(flow->tunnel.tun_id) > OFP_MAX_TUNNEL_ID || ntohll(flow->tunnel.tun_id) == 0)
    {
        OFP_LOG_ERROR("Unsupported tun_id value: %"PRIu64" tun_id should be in range <1-%u>",
            ntohll(flow->tunnel.tun_id), OFP_MAX_TUNNEL_ID);

        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }

    if (FLOW_TYPE_PORT_BASED_GLOBAL == OFP_FLOW_INPORT_BASED(p_rule) ||
        !adpt_port_is_tunnel_port(flow->in_port))
    {
        OFP_LOG_ERROR("Unsupported tun_id without tunnel-port as in_port ");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Get tunnel info by ofport
 * @param ofport                        tunnel port
 * @return OFP_ERR_XXX
 */
adpt_tunnel_info_t*
adpt_tunnel_get_tunnel_info_by_ofport(uint16_ofp ofport)
{
    int32_ofp ret;
    adpt_tunnel_info_t* p_tunnel_info = NULL;

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ofport = %d\n", ofport);

    ret = adpt_port_get_port_data(ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, (void**)&p_tunnel_info);
    if (ret)
    {
        return NULL;
    }

    return p_tunnel_info;
}

/**
 * Loop all service id which was binded with tunnel port and tunnel id
 * @param[in] p_rule                    tunnel port
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_loop_tunnel_port_service_id(ADPT_TUNNEL_SERVICE_ID_CB_FUNC func, void* pv_arg)
{
    ADPT_ERROR_RETURN(adpt_tunneldb_loop_tunnel_port_service_id(func, pv_arg));

    return OFP_ERR_SUCCESS;
}

/**
 * Get GRE packets miss-match (packet's ip do not match any tunnel port's ip) statistics
 * @param p_ofp_stats                   openflow statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_get_gre_packets_miss_match_stats(ofp_stats_t* p_ofp_stats)
{
    ADPT_PTR_CHECK(p_ofp_stats);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("miss_match_nhid = %d\n", g_p_tunnel_master->miss_match_nhid);

    ADPT_ERROR_RETURN(hal_nexthop_get_nh_stats(g_p_tunnel_master->miss_match_nhid, p_ofp_stats));

    return OFP_ERR_SUCCESS;
}

/**
 * Clear GRE packets miss-match (packet's ip do not match any tunnel port's ip) statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_clear_gre_packets_miss_match_stats(void)
{
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("miss_match_nhid = %d\n", g_p_tunnel_master->miss_match_nhid);

    ADPT_ERROR_RETURN(hal_nexthop_clear_nh_stats(g_p_tunnel_master->miss_match_nhid));

    return OFP_ERR_SUCCESS;
}

/**
 * Create tunnel port
 * @param tunnel_info                   tunnel port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_create_port(ofp_port_info_t* p_port_info)
{
#define TEMP_STR_LEN 32
    ofp_tunnel_info_t* p_ofp_info = NULL;
    adpt_tunnel_info_t* p_tunnel_info = NULL;
    adpt_port_of_fea_info_t* p_op_fea_info = NULL;
    adpt_port_status_info_t* p_port_status_info = NULL;
    int32_ofp ret;

    ofp_interface_type_t type;
    uint32_ofp nhid_wo_key;
    uint32_ofp nhid_w_key;

    OFP_PTR_CHECK(p_port_info);
    p_ofp_info = p_port_info->info;
    ADPT_PTR_CHECK(p_ofp_info);

    /* Begin dump tunnel information */
    ADPT_LOG_DEBUG_FUNC();
    {
        char local_ip_str[TEMP_STR_LEN] = {0};
        char remot_ip_str[TEMP_STR_LEN] = {0};
        char mac_str[TEMP_STR_LEN] = {0};
        uint32_ofp local_ip_network_byte_order = 0;
        uint32_ofp remote_ip_network_byte_order = 0;

        local_ip_network_byte_order = htonl(p_ofp_info->local_ip);
        remote_ip_network_byte_order = htonl(p_ofp_info->remote_ip);

        inet_ntop(AF_INET, &local_ip_network_byte_order, local_ip_str, 16);
        inet_ntop(AF_INET, &remote_ip_network_byte_order, remot_ip_str, 16);
        sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p_ofp_info->nexthop_mac[0], p_ofp_info->nexthop_mac[1],
            p_ofp_info->nexthop_mac[2], p_ofp_info->nexthop_mac[3],
            p_ofp_info->nexthop_mac[4], p_ofp_info->nexthop_mac[5]);
        ADPT_LOG_DEBUG("local_ip = %15s, remote_ip = %15s, nexthop_mac = %18s, bind_port = %8s\n",
            local_ip_str, remot_ip_str, mac_str, p_ofp_info->bind_port_name);
    }
    /* End dump tunnel information */

    p_tunnel_info = malloc(sizeof(adpt_tunnel_info_t));
    p_tunnel_info->ofport    = p_port_info->ofport;
    p_tunnel_info->flag      = p_ofp_info->flag;
    p_tunnel_info->local_ip  = p_ofp_info->local_ip;
    p_tunnel_info->remote_ip = p_ofp_info->remote_ip;
    memcpy(p_tunnel_info->nexthop_mac, p_ofp_info->nexthop_mac, OFP_ETH_ADDR_LEN);
    memcpy(p_tunnel_info->bind_port_name, p_ofp_info->bind_port_name, OFP_IFNAME_SIZE);
    p_tunnel_info->vlan_id   = p_ofp_info->vlan_id;

    p_tunnel_info->bind_port  = OFP_INVALID_GPORT;
    p_tunnel_info->iloop_port = OFP_INVALID_GPORT;

    p_op_fea_info = malloc(sizeof(adpt_port_of_fea_info_t));
    ADPT_MEM_PTR_CHECK(p_op_fea_info);
    p_port_status_info = malloc(sizeof(adpt_port_status_info_t));
    ADPT_MEM_PTR_CHECK(p_port_status_info);

    /* 1. Check whether the tunnel port is created */
    ret = adpt_port_get_port_type_by_ofport(p_port_info->ofport, &type);
    if (OFP_ERR_SUCCESS == ret)
    {
        ret = OFP_ERR_NETDEV_EXIST;
        goto Err0;
    }

    /* 2. check bind port valid */
    ret = adpt_port_get_port_type_by_name((const char*)p_tunnel_info->bind_port_name, &type);
    if (ret || type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        ret = OFP_ERR_TUNNEL_INVALID_BIND_NAME;
        goto Err0;
    }

    /* 3. get bind port gport */
    ret = adpt_port_get_gport_by_name((const char*)p_tunnel_info->bind_port_name, &p_tunnel_info->bind_port);
    if (ret)
    {
        ret = OFP_ERR_TUNNEL_INVALID_BIND_NAME;
        goto Err0;
    }

    /* 4. check whether the combination of local ip and remote ip is exist */
    ret = adpt_tunnel_bind_ip(p_tunnel_info->local_ip, p_tunnel_info->remote_ip);
    if (ret)
    {
        goto Err0;
    }
    
    /* 5. get internal port for openflow process after decapsulated. */
    ret = hal_tunnel_alloc_internal_port(&p_tunnel_info->iloop_port);
    if (ret)
    {
        goto Err1;
    }
    /* 5.1 enable the internal port. */
    ret = hal_tunnel_enable_iloop_port(p_tunnel_info->iloop_port, TRUE);
    if (ret)
    {
        goto Err2;
    }

    /* 6. enable user id to generate service id */
    ret = adpt_tunnel_lookup_service_id(p_tunnel_info->ofport, 0, &p_tunnel_info->service_id);
    if (ret)
    {
        goto Err3;
    }
    /* 6.1 add default user id entry  */
    ret = hal_user_id_add_vlan_default_entry_per_port(p_tunnel_info->iloop_port, p_tunnel_info->service_id);
    if (ret)
    {
        goto Err3;
    }
    /* 6.2 enable user id lookup */
    ret = hal_port_set_user_id_enable(p_tunnel_info->iloop_port, TRUE);
    if (ret)
    {
        goto Err4;
    }

    /* 7. add qos entry & ipuc route for decapsulation */
    ret = adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &nhid_wo_key);
    if (ret)
    {
        goto Err5;
    }

    ret = adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &nhid_w_key);
    if (ret)
    {
        goto Err6;
    }

    /* 8. create nhid for decapsulation */
    ret = hal_tunnel_create_decap_nhid(p_tunnel_info->iloop_port, nhid_wo_key, nhid_w_key);
    if (ret)
    {
        goto Err7;
    }

    /* 9. Add route entry for decapsulation */
    ret = hal_tunnel_add_decap_route(p_tunnel_info->local_ip, p_tunnel_info->remote_ip, nhid_wo_key, nhid_w_key);
    if (ret)
    {
        goto Err8;
    }
    
    /* 6. enable routing lookup */
    ret = adpt_tunnel_set_bind_port_route_en(p_tunnel_info->bind_port, TRUE);
    if (ret)
    {
        goto Err9;
    }

    p_tunnel_info->decap_nhid_wok = nhid_wo_key;
    p_tunnel_info->decap_nhid_wk  = nhid_w_key;
    
    /* 7. Create port in port db */
    ret = adpt_port_create_com_port(OFP_INTERFACE_TYPE_GRE, p_port_info->name, p_port_info->ofport,
        p_tunnel_info->iloop_port, p_port_info->ifindex);
    if (ret)
    {
        goto Err10;
    }

    /* 8. Save tunnel info in port db */
    ret = adpt_port_set_port_data(p_port_info->ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, p_tunnel_info);
    if (ret)
    {
        goto Err11;
    }

    /* 9. set openflow info */
    memset(p_op_fea_info, 0, sizeof(adpt_port_of_fea_info_t));
    ret = adpt_port_set_port_data(p_port_info->ofport, ADPT_PORT_DATA_TYPE_OP_INFO, p_op_fea_info);
    if (ret)
    {
        goto Err11;
    }

    /* 10. set port status info */
    memset(p_port_status_info, 0, sizeof(adpt_port_status_info_t));
    p_port_status_info->modified = false;
    ret = adpt_port_set_port_data(p_port_info->ofport, ADPT_PORT_DATA_TYPE_STATUS_INFO, p_port_status_info);
    if (ret)
    {
        goto Err11;
    }

#undef TEMP_STR_LEN
    return OFP_ERR_SUCCESS;

Err11:
    adpt_port_destroy_com_port(p_tunnel_info->ofport);
Err10:
    adpt_tunnel_set_bind_port_route_en(p_tunnel_info->bind_port, FALSE);
Err9:
    hal_tunnel_del_decap_route(p_tunnel_info->local_ip, p_tunnel_info->remote_ip, nhid_wo_key, nhid_w_key);
Err8:
    hal_tunnel_remove_decap_nhid(nhid_wo_key, nhid_w_key);
Err7:
    adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, nhid_w_key);
Err6:
    adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, nhid_wo_key);
Err5:
    hal_port_set_user_id_enable(p_tunnel_info->iloop_port, false);
Err4:
    hal_user_id_del_default_enry_per_port(p_tunnel_info->iloop_port);
Err3:
    hal_tunnel_enable_iloop_port(p_tunnel_info->iloop_port, FALSE);
Err2:
    hal_tunnel_release_internal_port(p_tunnel_info->iloop_port);
Err1:
    adpt_tunnel_unbind_ip(p_tunnel_info->local_ip, p_tunnel_info->remote_ip);
Err0:
    free(p_tunnel_info);
    free(p_op_fea_info);
    free(p_port_status_info);
    p_tunnel_info = NULL;
    p_op_fea_info = NULL;
    p_port_status_info = NULL;
    ADPT_ERROR_RETURN(ret);

    return OFP_ERR_SUCCESS;
}

/**
 * Delete tunnel port
 * @param ofport                        tunnel port
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_delete_port(uint16_ofp ofport)
{
    adpt_tunnel_info_t* p_tunnel_info = NULL;
    adpt_port_of_fea_info_t* p_op_fea_info = NULL;
    adpt_port_status_info_t* p_port_status_info = NULL;
    int32_ofp ret;

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ofport = %d\n", ofport);

    /* 1. Check whether the tunnel port is created */
    ret = adpt_port_get_port_data(ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, (void**)&p_tunnel_info);
    if (ret || !p_tunnel_info)
    {
        return OFP_ERR_SUCCESS;
    }
    ret = adpt_port_get_port_data(ofport, ADPT_PORT_DATA_TYPE_OP_INFO, (void**)&p_op_fea_info);
    if (ret || !p_op_fea_info)
    {
        return OFP_ERR_SUCCESS;
    }
    ret = adpt_port_get_port_data(ofport, ADPT_PORT_DATA_TYPE_STATUS_INFO, (void**)&p_port_status_info);
    if (ret || !p_port_status_info)
    {
        return OFP_ERR_SUCCESS;
    }

    /* 2. unbind local ip and remote ip */
    adpt_tunnel_unbind_ip(p_tunnel_info->local_ip, p_tunnel_info->remote_ip);

    /* 3. disable routing lookup */
    adpt_tunnel_set_bind_port_route_en(p_tunnel_info->bind_port, FALSE);

    /* 4. remove route entry*/
    hal_tunnel_del_decap_route(p_tunnel_info->local_ip, p_tunnel_info->remote_ip,
        p_tunnel_info->decap_nhid_wok, p_tunnel_info->decap_nhid_wk);

    /* 4. remove nhid */
    hal_tunnel_remove_decap_nhid(p_tunnel_info->decap_nhid_wok, p_tunnel_info->decap_nhid_wk);

    adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, p_tunnel_info->decap_nhid_wok);
    adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, p_tunnel_info->decap_nhid_wk);

    /* 5. disable user id to generate service id */
    hal_port_set_user_id_enable(p_tunnel_info->iloop_port, false);
    hal_user_id_del_default_enry_per_port(p_tunnel_info->iloop_port);

    /* 6. release internal port */
    hal_tunnel_enable_iloop_port(p_tunnel_info->iloop_port, FALSE);
    hal_tunnel_release_internal_port(p_tunnel_info->iloop_port);

    /* 7. clear db */
    adpt_port_set_port_data(p_tunnel_info->ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, NULL);
    adpt_port_set_port_data(p_tunnel_info->ofport, ADPT_PORT_DATA_TYPE_OP_INFO, NULL);
    adpt_port_set_port_data(p_tunnel_info->ofport, ADPT_PORT_DATA_TYPE_STATUS_INFO, NULL);

    /* 8. create port */
    adpt_port_destroy_com_port(p_tunnel_info->ofport);

    free(p_tunnel_info);
    free(p_op_fea_info);
    free(p_port_status_info);
    p_tunnel_info = NULL;
    p_op_fea_info = NULL;
    p_port_status_info = NULL;

    return OFP_ERR_SUCCESS;
}

/**
 * Adapter layer tunnel init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_init(void)
{
    g_p_tunnel_master = malloc(sizeof(adpt_tunnel_master_t));
    ADPT_MEM_PTR_CHECK(g_p_tunnel_master);

    memset(g_p_tunnel_master, 0, sizeof(adpt_tunnel_master_t));
    ADPT_ERROR_RETURN(adpt_tunneldb_init());
    ADPT_ERROR_RETURN(adpt_tunnel_init_tunnel_port_service_id());
    ADPT_ERROR_RETURN(adpt_tunnel_add_default_userid_entry());
    ADPT_ERROR_RETURN(hal_tunnel_add_default_route_entry(&g_p_tunnel_master->miss_match_nhid));
    ADPT_ERROR_RETURN(hal_tunnel_add_default_qos_entry());

    return OFP_ERR_SUCCESS;
}
