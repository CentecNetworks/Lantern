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
 * @brief This file is the main implementation of Centec open vswitchd adapt layer API
 */

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/

#include "ofp_api.h"
#include "ofp_port_api.h"

#include "adpt_port.h"
#include "adpt_gre_tunnel.h"
#include "hal_port.h"

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
VLOG_DEFINE_THIS_MODULE(ofp_port_api);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Set port config
 * @param ofp_port              ofp port
 * @param port_config           port config
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_set_config(uint16_ofp ofport, uint32_ofp port_config)
{
    uint16 gport = 0;

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ofport= %d, port_config = %d", ofport, port_config);

    OFP_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    OFP_ERROR_RETURN(hal_port_set_config(gport, port_config));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Check port name and ofport
 * @param p_port_info           Pointer of port information
 * @return OFP_ERR_XXX
 */
static int32_ofp
ofp_port_check(ofp_port_info_t* p_port_info)
{
    if (p_port_info->type == OFP_INTERFACE_TYPE_PHYSICAL)
    {
        if (!ofp_netdev_name_is_phy_port(p_port_info->name))
        {
            return OFP_ERR_INVALID_PARAM;
        }
        if (!ofp_netdev_ofport_is_phy_port(p_port_info->ofport, false))
        {
            return OFP_ERR_INVALID_PARAM;
        }
    }
    else if (p_port_info->type == OFP_INTERFACE_TYPE_GRE)
    {
        if (!ofp_netdev_name_is_gre_port(p_port_info->name))
        {
            return OFP_ERR_INVALID_PARAM;
        }
        if (!ofp_netdev_ofport_is_gre_port(p_port_info->ofport, false))
        {
            return OFP_ERR_INVALID_PARAM;
        }
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set port feature
 * @param ret                   Error code
 * @param p_port_info           Pointer of port information
 * @param p_tunnel_info         Pointer of tunnel information
 * @return OFP_ERR_XXX
 */
static int32_ofp
ofp_port_tunnel_log_error(int32_ofp error, ofp_port_info_t* p_port_info, ofp_tunnel_info_t* p_tunnel_info)
{
    switch(error)
    {
    case OFP_ERR_SUCCESS:
        break;
    case OFP_ERR_TUNNEL_INVALID_BIND_NAME:
        VLOG_ERR("Fail to create tunnel %s due to invalid bind name %s.",
                 p_port_info->name, p_tunnel_info->bind_port_name);
        break;
        
    case OFP_ERR_TUNNEL_REPEAT_LOCAL_REMOTE_IP:
        VLOG_ERR("Fail to create tunnel %s due to repeated local_ip "IP_FMT" and remote_ip "IP_FMT".",
                 p_port_info->name, IP_ARGS(htonl(p_tunnel_info->local_ip)),
                 IP_ARGS(htonl(p_tunnel_info->remote_ip)));
        break;

    case OFP_ERR_TUNNEL_LOCAL_IP_FULL:
        VLOG_ERR("Fail to create tunnel %s due to exceeding maximum %d local_ip.",
                 p_port_info->name, OFP_TUNNEL_MAX_LOCAL_IP_NUM);
        break;
        
    default:
        VLOG_ERR("Fail to create tunnel %s due to error %d.",
                 p_port_info->name, error);
        break;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Add port to openflow instance
 * @param p_port_info           Pointer of port information
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_add(ofp_port_info_t* p_port_info)
{
    uint16_ofp gport = OFP_INVALID_GPORT;
    
    OFP_PTR_CHECK(p_port_info);
    OFP_PTR_CHECK(p_port_info->name);

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("type = %s, name = %s, ofport = %d, ifindex = %d, info = 0x%x\n", 
        OFP_MAP_INTF_TYPE_STR(p_port_info->type), p_port_info->name, p_port_info->ofport, p_port_info->ifindex, 
        (uint32_ofp)p_port_info->info);
    
    OFP_ERROR_RETURN(ofp_port_check(p_port_info));
    
    if (OFP_INTERFACE_TYPE_PHYSICAL == p_port_info->type)
    {
        OFP_ERROR_RETURN(adpt_port_enable_openflow(p_port_info->ofport));
    }
    else if (OFP_INTERFACE_TYPE_GRE == p_port_info->type)
    {
        int32_ofp ret = OFP_ERR_SUCCESS;

        ret = adpt_tunnel_create_port(p_port_info);
        if (ret)
        {
            ofp_port_tunnel_log_error(ret, p_port_info, p_port_info->info);
            return ret;
        }
    }
    else
    {
        if  (OFP_INTERFACE_TYPE_SYSTEM == p_port_info->type)
        {
            OFP_ERROR_RETURN(ofp_netdev_set_mtu(p_port_info->name, OFP_CPU_PKT_BUF_LEN));
        }
        OFP_ERROR_RETURN(adpt_port_create_com_port(p_port_info->type, p_port_info->name, p_port_info->ofport, gport, p_port_info->ifindex));
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Delete port from openflow instance
 * @param ofport                ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_del(uint16_ofp ofport)
{
    ofp_interface_type_t if_type;
    int32_ofp ret;

    OFP_LOG_DEBUG_FUNC();
    OFP_LOG_DEBUG("ofport = %d\n", ofport);

    /* 1. check whether port is existent, and get if type */
    ret = adpt_port_get_port_type_by_ofport(ofport, &if_type);
    if (ret)
    {
        return OFP_ERR_SUCCESS;
    }

    if (OFP_INTERFACE_TYPE_PHYSICAL == if_type)
    {
        OFP_ERROR_RETURN(adpt_port_disable_openflow(ofport));
    }
    else if (OFP_INTERFACE_TYPE_GRE == if_type)
    {
        OFP_ERROR_RETURN(adpt_tunnel_delete_port(ofport));
    }
    else
    {
        OFP_ERROR_RETURN(adpt_port_destroy_com_port(ofport));
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get ofport by ifname
 * @param[in]  ifname                   port name
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_get_ofport_by_name(const char* ifname, uint16_ofp* p_ofport)
{
    OFP_ERROR_RETURN(adpt_port_get_ofport_by_name(ifname, p_ofport));

    return OFP_ERR_SUCCESS;
}

/**
 * Get route mac
 * @param[in] mac                       route mac
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_get_route_mac(uint8_ofp mac[OFP_ETH_ADDR_LEN])
{
    OFP_ERROR_RETURN(adpt_port_get_route_mac(mac));
    return OFP_ERR_SUCCESS;
}
