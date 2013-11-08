/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file is the main file of adapter layer port
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ofp_api.h"
#include "glb_stm_define.h"
#include "glb_phy_define.h"

#include "adpt.h"
#include "adpt_port.h"
#include "adpt_port_priv.h"
#include "adpt_gre_tunnel.h"
#include "adpt_message.h"
#include "adpt_nexthop.h"
#include "hal_port.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_port);

extern uint8_ofp g_current_profile;

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
adpt_port_master_t* g_p_adpt_port_master;
adpt_queue_master_t* g_p_adpt_queue_master;
  
/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Get gport by ofport
 * @param[in]  ofport                   ofport
 * @param[out] p_gport                  pointer of gport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_gport_by_ofport(uint16_ofp ofport, uint16_ofp* p_gport)
{
    adpt_port_info_t* p_info = NULL;

    ADPT_PTR_CHECK(p_gport);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_info));
    *p_gport = p_info->com_info.gport;

    return OFP_ERR_SUCCESS;
}

/**
 * Get gport by port name
 * @param[in]  ifname                   port name
 * @param[out] p_gport                  pointer of gport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_gport_by_name(const char* ifname, uint16_ofp* p_gport)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(ifname);
    ADPT_PTR_CHECK(p_gport);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_name(ifname, &p_info));
    *p_gport = p_info->com_info.gport;

    return OFP_ERR_SUCCESS;
}

/**
 * Get ofport by gport
 * @param[in]  gport                    gport
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_ofport_by_gport(uint16_ofp gport, uint16_ofp* p_ofport)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(p_ofport);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_gport(gport, &p_info));
    *p_ofport = p_info->com_info.ofport;

    return OFP_ERR_SUCCESS;
}

/**
 * Get ofport by ifname
 * @param[in]  ifname                   port name
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_ofport_by_name(const char* ifname, uint16_ofp* p_ofport)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(ifname);
    ADPT_PTR_CHECK(p_ofport);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_name(ifname, &p_info));
    *p_ofport = p_info->com_info.ofport;

    return OFP_ERR_SUCCESS;
}

/**
 * Get port type by ofport
 * @param[in]  ofport                   ofport
 * @param[out] p_type                   port type
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_type_by_ofport(uint16_ofp ofport, ofp_interface_type_t* p_type)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(p_type);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_info));
    *p_type = p_info->type;

    return OFP_ERR_SUCCESS;
}

/**
 * Get port type by ofport
 * @param[in]  ifname                   port name
 * @param[out] p_type                   port type
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_type_by_name(const char* ifname, ofp_interface_type_t* p_type)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(ifname);
    ADPT_PTR_CHECK(p_type);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_name(ifname, &p_info));
    *p_type = p_info->type;

    return OFP_ERR_SUCCESS;
}

/**
 * Get port name by gport
 * @param[in]  gport                    gport
 * @param[out] ifname                   port name
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_name_by_gport(uint16_ofp gport, char* ifname)
{
    adpt_port_info_t* p_info = NULL;

    ADPT_PTR_CHECK(ifname);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_gport(gport, &p_info));
    strcpy(ifname, p_info->com_info.name);

    return OFP_ERR_SUCCESS;
}

/**
 * Get port name by gport
 * @param[in]  ofport                   ofport
 * @param[out] ifname                   port name
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_name_by_ofport(uint16_ofp ofport, char* ifname)
{
    adpt_port_info_t* p_info;

    ADPT_PTR_CHECK(ifname);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_info));
    strncpy(ifname, p_info->com_info.name, OFP_IFNAME_SIZE);

    return OFP_ERR_SUCCESS;
}

/**
 * Add port to openflow instance
 * @param[in] p_port_info               Pointer of ofport_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_enable_openflow(uint16_ofp ofport)
{
    uint16_ofp gport;
    adpt_port_phy_info_t* p_phy_info = NULL;
    adpt_port_l3_info_t* p_l3_info = NULL;
    adpt_port_info_t* p_port_info = NULL;
    

    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ofport = %d\n", ofport);

    if (adpt_port_is_openflow_port(ofport))
    {
        return OFP_ERR_SUCCESS;
    }

    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_port_info));
    ADPT_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    ADPT_LOG_DEBUG("gport = %d\n", gport);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport,  ADPT_PORT_DATA_TYPE_L3_INFO, (void **)&p_l3_info));
    ADPT_PTR_CHECK(p_l3_info);
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport,  ADPT_PORT_DATA_TYPE_PHY_INFO, (void **)&p_phy_info));
    ADPT_PTR_CHECK(p_phy_info);
    ADPT_ERROR_RETURN(hal_port_enable_openflow(gport, p_l3_info->l3ifid, p_phy_info->mac));    
    ADPT_ERROR_RETURN(hal_port_enable_qos(gport));
    
    ofp_netdev_enable_interface(p_port_info->com_info.name);

    adpt_portdb_set_port_flag(ofport, TRUE, ADPT_PORT_COM_FLAG_OPENFLOW_EN);

    return OFP_ERR_SUCCESS;
}

/**
 * delete port from openflow instance
 * @param[in] ofport                    ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_disable_openflow(uint16_ofp ofport)
{
    uint16_ofp gport;
    adpt_port_l3_info_t* p_l3_info = NULL;
    char ifname[OFP_IFNAME_SIZE] = {0};
    
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ofport = %d\n", ofport);
    if (!adpt_port_is_openflow_port(ofport))
    {
        return OFP_ERR_SUCCESS;
    }
    
    ADPT_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    ADPT_ERROR_RETURN(adpt_port_get_name_by_ofport(ofport, ifname));
    
    ofp_netdev_disable_interface(ifname);
    
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport,  ADPT_PORT_DATA_TYPE_L3_INFO, (void **)&p_l3_info));
    ADPT_PTR_CHECK(p_l3_info);
    ADPT_ERROR_RETURN(hal_port_disable_openflow(gport, p_l3_info->l3ifid));

    adpt_portdb_set_port_flag(ofport, FALSE, ADPT_PORT_COM_FLAG_OPENFLOW_EN);
    
    return OFP_ERR_SUCCESS;
}

/**
 * Check ofport is physical port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_physical_port(uint16_ofp ofport)
{
    ofp_interface_type_t type = OFP_INTERFACE_TYPE_UNKNOWN;
    int32_ofp ret;
    
    ret = adpt_port_get_port_type_by_ofport(ofport, &type);
    if (ret || type != OFP_INTERFACE_TYPE_PHYSICAL)
    {
        return FALSE;
    }
    
    return TRUE;
}

/**
 * Check ofport is tunnel port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_tunnel_port(uint16_ofp ofport)
{
    ofp_interface_type_t type = OFP_INTERFACE_TYPE_UNKNOWN;
    int32_ofp ret;
    
    ret = adpt_port_get_port_type_by_ofport(ofport, &type);
    if (ret || type != OFP_INTERFACE_TYPE_GRE)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Check ofport is hybrid port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_hybrid_port(uint16_ofp ofport)
{
    if (adpt_portdb_check_port_flag(ofport, ADPT_PORT_COM_FLAG_HYBRID_EN))
    {
        return TRUE;
    }
    return FALSE;
}

bool 
adpt_port_is_openflow_port(uint16_ofp ofport)
{
    if (adpt_portdb_check_port_flag(ofport, ADPT_PORT_COM_FLAG_OPENFLOW_EN))
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * Create port
 * @param[in] type                      interface type
 * @param[in] ifname                    interface name
 * @param[in] ofport                    ofport
 * @param[in] gport                     global port
 * @param[in] ifindex                   ifindex
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_create_com_port(ofp_interface_type_t type, char* ifname, uint16_ofp ofport, uint16_ofp gport, uint32_ofp ifindex)
{
    adpt_port_info_t* p_port_info = NULL;

    ADPT_PTR_CHECK(ifname);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ifname = %s, ofport = %d, gport = %d, ifindex = %d\n", ifname, ofport, gport, ifindex);

    adpt_portdb_get_port_info_by_ofport(ofport, &p_port_info);
    if (p_port_info)
    {
        return OFP_ERR_NETDEV_EXIST;
    }

    p_port_info = malloc(sizeof(adpt_port_info_t));
    ADPT_MEM_PTR_CHECK(p_port_info);
    memset(p_port_info, 0, sizeof(adpt_port_info_t));

    p_port_info->type             = type;
    p_port_info->com_info.ofport  = ofport;
    p_port_info->com_info.gport   = gport;
    p_port_info->com_info.ifindex = ifindex;
    strcpy(p_port_info->com_info.name, ifname);
    
    ADPT_ERROR_RETURN(adpt_portdb_add_port(p_port_info));

    return OFP_ERR_SUCCESS;
}

/**
 * Destroy port
 * @param[in] ofport                    ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_destroy_com_port(uint16_ofp ofport)
{
    adpt_port_info_t* p_port_info = NULL;
    
    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_port_info));
    ADPT_ERROR_RETURN(adpt_portdb_del_port(p_port_info));
    
    free(p_port_info);
    
    return OFP_ERR_SUCCESS;
}

/**
 * Create physical port
 * @param[in] p_req                     Pointer of adpt_create_phy_port_req_t
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_port_create_phy_port(adpt_create_phy_port_req_t* p_req)
{
    int32_ofp ret;
    char ifname[OFP_IFNAME_SIZE] = {0};
    ofp_interface_type_t type;
    adpt_port_phy_info_t* p_phy_info = NULL;
    adpt_port_l3_info_t* p_l3_info = NULL;
    adpt_port_info_t* p_port_info = NULL;
    adpt_queue_status_info_t* p_queue_info = NULL;
    adpt_port_of_fea_info_t* p_op_fea_info = NULL;
    adpt_port_status_info_t* p_port_status_info = NULL;
    uint16_ofp ofport;
    uint16_ofp l3ifid;
    
    ADPT_PTR_CHECK(p_req);
    
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("slot_no = %d, port_no = %d, gport = %d, mac = %02x:%02x:%02x:%02x:%02x:%02x, ifindex = %d",
        p_req->slot_no, p_req->port_no, p_req->gport, 
        p_req->mac[0], p_req->mac[1], p_req->mac[2],
        p_req->mac[3], p_req->mac[4], p_req->mac[5],
        p_req->ifindex);

    type     = OFP_INTERFACE_TYPE_PHYSICAL;
    snprintf(ifname, OFP_IFNAME_SIZE, "eth-%u-%u", p_req->slot_no, p_req->port_no);

    /* 1. Check whether the port exists */
    ret = adpt_portdb_get_port_info_by_name(ifname, &p_port_info);
    if (ret == OFP_ERR_SUCCESS)
    {
        return OFP_ERR_SUCCESS;
    }
    
    p_phy_info = malloc(sizeof(adpt_port_phy_info_t));
    ADPT_MEM_PTR_CHECK(p_phy_info);
    
    p_op_fea_info = malloc(sizeof(adpt_port_of_fea_info_t));
    ADPT_MEM_PTR_CHECK(p_op_fea_info);

    p_port_status_info = malloc(sizeof(adpt_port_status_info_t));
    ADPT_MEM_PTR_CHECK(p_port_status_info);

    /* 2. get ofport */
    ADPT_ERROR_RETURN(ofp_netdev_assign_phy_ofport(ifname, &ofport));
    ADPT_LOG_DEBUG("ofport = %d", ofport);

    /* 3. create port */
    ADPT_ERROR_RETURN(adpt_port_create_com_port(type, ifname, ofport, p_req->gport, p_req->ifindex));

    /* 4. set phy info */
    memset(p_phy_info, 0, sizeof(adpt_port_phy_info_t));
    memcpy(p_phy_info->mac, p_req->mac,OFP_ETH_ADDR_LEN);
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(p_req->gport, ADPT_PORT_DATA_TYPE_PHY_INFO, p_phy_info));

    /* 5. set openflow info */
    memset(p_op_fea_info, 0, sizeof(adpt_port_of_fea_info_t));
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(p_req->gport, ADPT_PORT_DATA_TYPE_OP_INFO, p_op_fea_info));

    /* 6. set port status info */
    memset(p_port_status_info, 0, sizeof(adpt_port_status_info_t));
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(p_req->gport, ADPT_PORT_DATA_TYPE_STATUS_INFO, p_port_status_info));

    /* 7. set nh info */
    ADPT_ERROR_RETURN(adpt_nexthop_phy_port_create(p_req->gport));

    /* 8. init port in sdk */
    /* set l3 info */
    ADPT_ERROR_RETURN(adpt_port_get_l3ifid(p_req->gport, &l3ifid));
    p_l3_info = malloc(sizeof(adpt_port_l3_info_t));
    memset(p_l3_info, 0, sizeof(adpt_port_l3_info_t));
    p_l3_info->l3ifid = l3ifid;
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(p_req->gport,  ADPT_PORT_DATA_TYPE_L3_INFO, p_l3_info));

    /* 9. set mtu of netdev */
    ADPT_ERROR_RETURN(ofp_netdev_set_mtu(ifname, OFP_DEFAULT_MAX_JUMBO_FRAME_SIZE));

    /* 10. set queue */
    p_queue_info = malloc(sizeof(adpt_queue_status_info_t));
    memset(p_queue_info, 0xff, sizeof(adpt_queue_status_info_t));    
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(p_req->gport,  ADPT_PORT_DATA_TYPE_QUEUE_INFO, p_queue_info));

    /* set default to openflow */
    ADPT_ERROR_RETURN(adpt_port_enable_openflow(ofport));

    ADPT_LOG_DEBUG("Create port %s successed", ifname);

    return OFP_ERR_SUCCESS;
}

/**
 * Destroy physical port
 * @param[in] p_req                     Not used
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_port_destroy_phy_port(void* req)
{
    req = req; /* TODO unused parameter */
    /*Do not destroy physical port currently */
    return OFP_ERR_SUCCESS;
}

/**
 * Map phy info to openflow feature 
 * @param[in]  p_phy_info               Pointer of adpt_port_phy_info_t
 * @param[out] p_fea_info               Pointer of adpt_port_of_fea_info_t
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_port_map_phy_info_op_fea(adpt_port_phy_info_t* p_phy_info, adpt_port_of_fea_info_t* p_fea_info)
{
    uint32 current = 0, advertised = 0, supported = 0, peer = 0, carrier = 0;
    glb_port_duplex_t cfg_duplex = p_phy_info->port_cfg.duplex;
    glb_port_speed_t cfg_speed = p_phy_info->port_cfg.speed;
    
    /* speed mode */
    if (GLB_DUPLEX_FULL == p_phy_info->duplex)
    {
        switch (p_phy_info->speed)
        {
        case GLB_SPEED_10M:
            current |= NETDEV_F_10MB_HD;
            break;
        case GLB_SPEED_100M:
            current |= NETDEV_F_100MB_FD;
            break;
        case GLB_SPEED_1G:
            current |= NETDEV_F_1GB_FD;
            break;
        case GLB_SPEED_10G:
            current |= NETDEV_F_10GB_FD;
            break;
        case GLB_SPEED_MAX:
            break;
        case GLB_SPEED_AUTO:
        default:
            current |= NETDEV_F_AUTONEG;
            break;
        }
    }
    else if (GLB_DUPLEX_HALF == p_phy_info->duplex)
    {
        switch (p_phy_info->speed)
        {
        case GLB_SPEED_10M:
            current |= NETDEV_F_10MB_HD;
            break;
        case GLB_SPEED_100M:
            current |= NETDEV_F_100MB_HD;
            break;
        case GLB_SPEED_1G:
            current |= NETDEV_F_1GB_HD;
            break;
        case GLB_SPEED_MAX:
            break;
        case GLB_SPEED_10G:
        case GLB_SPEED_AUTO:
        default:
            current |= NETDEV_F_AUTONEG;
            break;
        }
    }
    else
    {
        current |= 0;
    }

    /* autonego mode */
    if((cfg_speed == GLB_SPEED_AUTO) || (cfg_duplex == GLB_DUPLEX_AUTO))
    {
        current |= NETDEV_F_AUTONEG;
    }

    /* link media */
    switch (p_phy_info->media)
    {
        case GLB_PORT_TYPE_RJ45:
            current |= NETDEV_F_COPPER;
            supported = NETDEV_F_10MB_HD | NETDEV_F_10MB_FD | NETDEV_F_100MB_HD | NETDEV_F_100MB_FD |
                    NETDEV_F_1GB_HD | NETDEV_F_1GB_FD | NETDEV_F_COPPER | NETDEV_F_AUTONEG;
            advertised = NETDEV_F_COPPER;
            if (GLB_DUPLEX_FULL == cfg_duplex)
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_10M:
                        advertised |= NETDEV_F_10MB_FD;
                        break;
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_FD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_FD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_10MB_FD | NETDEV_F_100MB_FD | NETDEV_F_1GB_FD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            else if (GLB_DUPLEX_HALF == cfg_duplex)
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_10M:
                        advertised |= NETDEV_F_10MB_HD;
                        break;
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_HD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_HD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_10MB_HD | NETDEV_F_100MB_HD | NETDEV_F_1GB_HD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            else
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_10M:
                        advertised |= NETDEV_F_10MB_HD | NETDEV_F_10MB_FD;
                        break;
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_HD | NETDEV_F_100MB_FD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_HD | NETDEV_F_1GB_FD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_10MB_HD | NETDEV_F_100MB_HD | NETDEV_F_1GB_HD |
                            NETDEV_F_10MB_FD | NETDEV_F_100MB_FD | NETDEV_F_1GB_FD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            break;
        case GLB_PORT_TYPE_SFP:
            current |= NETDEV_F_FIBER;
            supported = NETDEV_F_100MB_HD | NETDEV_F_100MB_FD | NETDEV_F_1GB_HD | NETDEV_F_1GB_FD |
                    NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            advertised = NETDEV_F_FIBER;
            if (GLB_DUPLEX_FULL == cfg_duplex)
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_FD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_FD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_100MB_FD | NETDEV_F_1GB_FD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10M:
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            else if (GLB_DUPLEX_HALF == cfg_duplex)
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_HD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_HD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_100MB_HD | NETDEV_F_1GB_HD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10M:
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            else
            {
                switch (cfg_speed)
                {
                    case GLB_SPEED_100M:
                        advertised |= NETDEV_F_100MB_HD | NETDEV_F_100MB_FD;
                        break;
                    case GLB_SPEED_1G:
                        advertised |= NETDEV_F_1GB_HD | NETDEV_F_1GB_FD;
                        break;
                    case GLB_SPEED_AUTO:
                        advertised |= NETDEV_F_100MB_HD | NETDEV_F_1GB_HD |
                            NETDEV_F_100MB_FD | NETDEV_F_1GB_FD | NETDEV_F_AUTONEG;
                        break;
                    case GLB_SPEED_10M:
                    case GLB_SPEED_10G:
                    case GLB_SPEED_MAX:
                        break;
                    default:
                        advertised |= NETDEV_F_AUTONEG;
                        break;
                }
            }
            break;
        case GLB_PORT_TYPE_XFP:
            current |= NETDEV_F_FIBER;
            supported =  NETDEV_F_10GB_FD | NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            advertised = NETDEV_F_10GB_FD | NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            break;
        case GLB_PORT_TYPE_SFP_PLUS:
            current |= NETDEV_F_FIBER;
            supported =  NETDEV_F_10GB_FD | NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            advertised = NETDEV_F_10GB_FD | NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            break;
        case GLB_PORT_TYPE_AUTO:
            current |= NETDEV_F_FIBER;
            supported =  NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            advertised = NETDEV_F_FIBER | NETDEV_F_AUTONEG;
            break;
        default:
            current |= 0;
            supported = 0;
            advertised = 0;
            break;
    }

    if (GLB_LINK_UP == p_phy_info->link)
    {
        carrier = 1;
    }
    else if (GLB_LINK_DOWN == p_phy_info->link)
    {
        carrier    = 0;
        supported  = 0;
        current    = 0;
        advertised = 0;
        peer       = 0;
    }

    p_fea_info->carrier    = carrier;
    p_fea_info->current    = current;
    p_fea_info->supported  = supported;
    p_fea_info->advertised = advertised;
    p_fea_info->peer       = peer;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Receive link notification from lcm
 * @param[in] p_req                     Pointer of adpt_notify_link_status_req_t
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_port_rx_link_notify(adpt_notify_link_status_req_t* p_req)
{
    char ifname[OFP_IFNAME_SIZE] = {0};
    uint16_ofp gport;
    adpt_port_phy_info_t* p_phy_info;
    adpt_port_of_fea_info_t* p_fea_info;
    char duplex[4][8] = {"AUTO", "HALF", "FULL", "MAX"}; /* glb_port_duplex_t */
    char speed[6][8]  = {"AUTO", "10M", "100M", "1G", "10G", "MAX"}; /* glb_port_speed_t */
    char link[3][8]   = {"DOWN", "UP", "MAX"}; /* glb_port_link_t */
    bool link_changed = false;

    ADPT_PTR_CHECK(p_req);
    ADPT_MODULE_INIT_CHECK(g_p_adpt_port_master);

    snprintf(ifname, OFP_IFNAME_SIZE, "eth-%d-%d", p_req->slot_no, p_req->port_no);
    ADPT_LOG_DEBUG_FUNC();

    if (p_req->duplex >= GLB_DUPLEX_MAX || p_req->duplex < GLB_DUPLEX_AUTO)
    {
        ADPT_LOG_INFO("Invalid duplex %d", p_req->duplex);
        p_req->duplex = GLB_DUPLEX_MAX;
    }
    if (p_req->speed >= GLB_SPEED_MAX || p_req->speed < GLB_SPEED_AUTO)
    {
        ADPT_LOG_INFO("Invalid speed %d", p_req->speed);
        p_req->speed = GLB_SPEED_MAX;
    }
    if (p_req->enable >= GLB_LINK_MAX)
    {
        ADPT_LOG_INFO("Invalid link %d", p_req->enable);
        p_req->enable = GLB_LINK_MAX;
    }

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    ADPT_LOG_INFO("Port link status updated: ifname = %s, duplex = %s, speed = %s, link = %s\n", 
        ifname, duplex[p_req->duplex], speed[p_req->speed], link[p_req->enable]);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_PHY_INFO, (void**)&p_phy_info));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_OP_INFO, (void**)&p_fea_info));

    link_changed = (p_phy_info->link != p_req->enable) ? true : false;
    p_phy_info->duplex = p_req->duplex;
    p_phy_info->media  = p_req->linkup_media;
    p_phy_info->speed  = p_req->speed;
    p_phy_info->link   = p_req->enable;
    memcpy(&p_phy_info->port_cfg, &p_req->port_cfg, sizeof(glb_port_cfg_t));

    ADPT_ERROR_RETURN(adpt_port_map_phy_info_op_fea(p_phy_info, p_fea_info));
    adpt_port_set_port_modified(ifname, true);
    adpt_port_set_tunnel_port_modified_by_bind_port(ifname, true);

    return OFP_ERR_SUCCESS;
}


/**
 * Get openflow feature by name
 * @param[in]  ifname                   Interface name
 * @param[out] p_fea_info               Pointer of adpt_port_of_fea_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_of_fea_info(const char* ifname, adpt_port_of_fea_info_t* p_fea_info)
{
    uint16_ofp gport;
    adpt_port_of_fea_info_t* p_db_info = NULL;

    ADPT_PTR_CHECK(ifname);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ifname = %s\n", ifname);

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_OP_INFO, (void**)&p_db_info));
    memcpy(p_fea_info, p_db_info, sizeof(adpt_port_of_fea_info_t));
    ADPT_LOG_DEBUG("carrier = 0x%x, current = 0x%x, advertised = 0x%x, advertised = 0x%x, peer = 0x%x",
        p_fea_info->carrier, p_fea_info->current, p_fea_info->advertised, p_fea_info->supported, p_fea_info->peer);

    return OFP_ERR_SUCCESS;
}

/**
 * Set openflow feature by name
 * @param[in]  ifname                   Interface name
 * @param[in]  advertised               Pointer of advertised
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_of_fea_advertised(const char* ifname, uint32_ofp advertised)
{
    uint16_ofp gport;
    adpt_port_of_fea_info_t* p_fea_info;

    ADPT_PTR_CHECK(ifname);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ifname = %s\n", ifname);

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_OP_INFO, (void**)&p_fea_info));
    p_fea_info->advertised = advertised;

    ADPT_LOG_DEBUG("advertised = 0x%x", p_fea_info->advertised);

    return OFP_ERR_SUCCESS;
}

/**
 * Get port status by name
 * @param[in]  ifname                   Interface name
 * @param[out] p_status_info            Pointer of adpt_port_status_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_status(const char* ifname, adpt_port_status_info_t* p_status_info)
{
    uint16_ofp gport;
    adpt_port_status_info_t* p_db_info = NULL;

    ADPT_PTR_CHECK(ifname);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ifname = %s\n", ifname);

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_STATUS_INFO, (void**)&p_db_info));
    memcpy(p_status_info, p_db_info, sizeof(adpt_port_status_info_t));
    ADPT_LOG_DEBUG("port_modified = 0x%x", p_status_info->modified);

    return OFP_ERR_SUCCESS;
}

/**
 * Set port modified status by name
 * @param[in]  ifname                   Interface name
 * @param[in]  is_modified              Port is modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_port_modified(const char* ifname, bool is_modified)
{
    uint16_ofp gport;
    adpt_port_status_info_t* p_port_status_info;

    ADPT_PTR_CHECK(ifname);
    ADPT_LOG_DEBUG_FUNC();
    ADPT_LOG_DEBUG("ifname = %s\n", ifname);

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_name(ifname, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_STATUS_INFO, (void**)&p_port_status_info));

    ADPT_LOG_DEBUG("port_modified = 0x%x", p_port_status_info->modified);
    p_port_status_info->modified = is_modified;

    return OFP_ERR_SUCCESS;
}

/**
 * Set tunnel port modified status by bind port
 * @param bind_ifname                   Bind port interface name
 * @param is_modified                   Port is modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_tunnel_port_modified_by_bind_port(const char *bind_ifname, bool is_modified)
{
    adpt_tunnel_port_modified_t tunnel_port_modified;

    memset(&tunnel_port_modified, 0, sizeof(tunnel_port_modified));
    tunnel_port_modified.tunnel_type = OFP_INTERFACE_TYPE_GRE;
    tunnel_port_modified.bind_ifname = strdup(bind_ifname);
    tunnel_port_modified.is_modified = is_modified;
    adpt_portdb_set_tunnel_port_modified_by_bind_port(&tunnel_port_modified);
    free(tunnel_port_modified.bind_ifname);

    return OFP_ERR_SUCCESS;
}

/**
 * Get any port is modified
 * @param[out]  ifname                  Interface name that modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_any_port_modified(char** ifname)
{
    ctclib_list_node_t* p_node = NULL;
    adpt_port_info_t* p_port_info = NULL;
    adpt_port_status_info_t status_info;

    *ifname = NULL;

    ADPT_LOG_DEBUG_FUNC();

    memset(&status_info, 0, sizeof(adpt_port_status_info_t));

    ctclib_list_for_each(p_node, ADPT_PORT_LIST[OFP_INTERFACE_TYPE_PHYSICAL])
    {
        p_port_info = ctclib_container_of(p_node, adpt_port_info_t, node);
        adpt_port_get_port_status(p_port_info->com_info.name, &status_info);
        if (true == status_info.modified)
        {
            *ifname = malloc(sizeof(char) * OFP_IFNAME_SIZE);
            memset(*ifname, 0, sizeof(char) * OFP_IFNAME_SIZE);
            strncpy(*ifname, p_port_info->com_info.name, OFP_IFNAME_SIZE);

            return OFP_ERR_SUCCESS;
        }
    }

    ctclib_list_for_each(p_node, ADPT_PORT_LIST[OFP_INTERFACE_TYPE_GRE])
    {
        p_port_info = ctclib_container_of(p_node, adpt_port_info_t, node);
        adpt_port_get_port_status(p_port_info->com_info.name, &status_info);
        if (true == status_info.modified)
        {
            *ifname = malloc(sizeof(char) * OFP_IFNAME_SIZE);
            memset(*ifname, 0, sizeof(char) * OFP_IFNAME_SIZE);
            strncpy(*ifname, p_port_info->com_info.name, OFP_IFNAME_SIZE);

            return OFP_ERR_SUCCESS;
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Get physical port number
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_phy_port_num(uint32_ofp* p_port_num)
{
    ADPT_PTR_CHECK(p_port_num);
    ADPT_ERROR_RETURN(adpt_portdb_get_port_num(OFP_INTERFACE_TYPE_PHYSICAL, p_port_num));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get tunnel port number
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_tunnel_port_count(uint32_ofp* p_port_num)
{
    ADPT_PTR_CHECK(p_port_num);
    ADPT_ERROR_RETURN(adpt_portdb_get_port_num(OFP_INTERFACE_TYPE_GRE, p_port_num));

    return OFP_ERR_SUCCESS;
}

/**
 * Get tunnel port max
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_tunnel_port_max(uint32_ofp* p_port_max)
{
    ADPT_PTR_CHECK(p_port_max);
    *p_port_max = OFP_TUNNEL_PORT_NO_NUM;

    return OFP_ERR_SUCCESS;
}

/**
 * Get l3if id
 * @param[in]  gport                    global port
 * @param[out] p_l3ifid                 pointer of l3ifid
 * @return OFP_ERR_XXX
 */
uint16_ofp
adpt_port_get_l3ifid(uint16_ofp gport, uint16_ofp* p_l3ifid)
{
    ADPT_PTR_CHECK(p_l3ifid);
    *p_l3ifid = gport + OFP_L3IFID_BASE;

    return OFP_ERR_SUCCESS;
}

/**
 * Set port data by type
 * @param[in]  ofport                   ofport
 * @param[in]  data_type                port data type
 * @param[in]  p_data                   pointer of port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_port_data(uint16_ofp ofport, adpt_port_data_type_t data_type, void* p_data)
{
    uint16_ofp gport;

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_set_port_data(gport, data_type, p_data));

    return OFP_ERR_SUCCESS;
}

/**
 * Get port data by type
 * @param[in]  ofport                   ofport
 * @param[in]  data_type                port data type
 * @param[in]  p_data                   Pointer to get the pointer of port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_data(uint16_ofp ofport, adpt_port_data_type_t data_type, void** pp_data)
{
    uint16_ofp gport;

    ADPT_PTR_CHECK(pp_data);

    ADPT_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, data_type, pp_data));

    return OFP_ERR_SUCCESS;
}

/**
 * Clear the port change status
 * @param[in]  type                 Interface type
 * @param[in]  func                 Callback function
 * @param[in]  argv                 argv of Callback function
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_loop_port(ofp_interface_type_t type, ADPT_PORT_LOOP_CB_FUNC func, void* argv)
{
    ctclib_list_node_t* p_node = NULL;
    adpt_port_info_t* p_port_info = NULL;

    ADPT_PTR_CHECK(func);
    
    ctclib_list_for_each(p_node, ADPT_PORT_LIST[type])
    {
        p_port_info = ctclib_container_of(p_node, adpt_port_info_t, node);
        
        func(p_port_info->com_info.ofport, argv);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get non edit nhid by gport
 * @param[in]  gport                Gport
 * @param[out] p_nhid               pointer to nhid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_non_edit_nh_offset(uint16_ofp gport, uint32_ofp* p_nh_offset)
{
    adpt_port_nexthop_info_t* p_nh_info = NULL;

    ADPT_PTR_CHECK(p_nh_offset);

    ADPT_ERROR_RETURN(adpt_portdb_get_port_data(gport, ADPT_PORT_DATA_TYPE_NH_INFO, (void**)&p_nh_info));

    ADPT_PTR_CHECK(p_nh_info);
    *p_nh_offset = p_nh_info->non_edit_nh.offset;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Register callback function for lcm
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_port_register_lcm_callback(void)
{
    adpt_message_register_lcm_callback(ADPT_MSG_TYPE_CREATE_PORT,  (ADPT_MESSAGE_CB_FUNC)adpt_port_create_phy_port);
    adpt_message_register_lcm_callback(ADPT_MSG_TYPE_DESTORY_PORT, (ADPT_MESSAGE_CB_FUNC)adpt_port_destroy_phy_port);
    adpt_message_register_lcm_callback(ADPT_MSG_TYPE_NOTIFY_LINK,  (ADPT_MESSAGE_CB_FUNC)adpt_port_rx_link_notify);
    
    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_port_shutdown(uint16_ofp ofport)
{
    int32_ofp ret = 0;
    uint16_ofp gport = 0;
    char if_name[OFP_IFNAME_SIZE] = {0};
    
    ret = adpt_port_get_name_by_ofport(ofport, if_name);
    if (ret)
    {
        return OFP_ERR_FAIL;
    }
    
    ret = ofp_netdev_disable_interface(if_name);
    if (ret)
    {
        return OFP_ERR_FAIL;
    }

    adpt_portdb_set_port_flag(ofport, TRUE, ADPT_PORT_COM_FLAG_SHUTDOWN);
    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_port_no_shutdown(uint16_ofp ofport)
{
    int32_ofp ret = 0;
    char if_name[OFP_IFNAME_SIZE] = {0};
    
    ret = adpt_port_get_name_by_ofport(ofport, if_name);
    if (ret)
    {
        return OFP_ERR_FAIL;
    }
    
    ret = ofp_netdev_enable_interface(if_name);
    if (ret)
    {
        return OFP_ERR_FAIL;
    }

    adpt_portdb_set_port_flag(ofport, FALSE, ADPT_PORT_COM_FLAG_SHUTDOWN);
    return OFP_ERR_SUCCESS;
}

/**
 * Get route mac
 * @param[in] mac                       route mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_route_mac(uint8_ofp mac[OFP_ETH_ADDR_LEN])
{
    ADPT_ERROR_RETURN(hal_port_get_system_mac(mac));
    return OFP_ERR_SUCCESS;
}

/**
 * Adapter port init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_init(void)
{
    uint8_ofp route_mac[OFP_ETH_ADDR_LEN] = {0};
    
    g_p_adpt_port_master = malloc(sizeof(adpt_port_master_t));
    ADPT_MEM_PTR_CHECK(g_p_adpt_port_master);
    memset(g_p_adpt_port_master, 0, sizeof(adpt_port_master_t));

    g_p_adpt_queue_master = malloc(sizeof(adpt_queue_master_t));
    ADPT_MEM_PTR_CHECK(g_p_adpt_queue_master);
    memset(g_p_adpt_queue_master, 0, sizeof(adpt_queue_master_t));
    
    ADPT_ERROR_RETURN(adpt_portdb_init());
    ADPT_ERROR_RETURN(adpt_port_register_lcm_callback());
    
    ADPT_ERROR_RETURN(hal_port_get_system_mac(route_mac));
    ADPT_ERROR_RETURN(hal_port_set_l3if_vmac(route_mac));
    
    return OFP_ERR_SUCCESS;
}
