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
 * @brief This file is the database implementation of adapter layer port
 */
 
/******************************************************************************
* Header Files 
******************************************************************************/
#include <string.h>

#include "vlog.h"
#include "ofp_api.h"
#include "adpt.h"
#include "adpt_port.h"
#include "adpt_port_priv.h"
#include "adpt_nexthop.h"
#include "adpt_gre_tunnel.h"
#include "adpt_gre_tunnel_priv.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_port);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Port db make hash key by name
 * @param p_info                        Pointer to adpt_port_info_t
 * @return hash key
 */
static inline uint32_ofp
adpt_portdb_name_hash_make(void* p_info_void)
{
    uint32_ofp hash = 0;
    uint32_ofp len = 0;
    adpt_port_info_t* p_info = NULL;

    p_info = p_info_void;
    len = strlen(p_info->com_info.name);

    while(len > 0)
    {
        hash += *(p_info->com_info.name + len - 1);
        len --;
    }
    
    return hash;
}

/**
 * Port db compare two port info
 * @param p_info_in_bucket_void              Port in db
 * @param p_info_to_lkp_void                 Port to be compared
 * @return true/false
 */
static inline bool
adpt_portdb_name_hash_compare(void* p_info_in_bucket_void, void* p_info_to_lkp_void)
{
    adpt_port_info_t* p_info_in_bucket = NULL;
    adpt_port_info_t* p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if ( 0 != strcmp((const char*)p_info_in_bucket->com_info.name, (const char*)p_info_to_lkp->com_info.name))
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Port db make hash key by ofport
 * @param p_info_void                        Pointer to adpt_port_info_t
 * @return hash key
 */
static inline uint32_ofp
adpt_portdb_ofport_hash_make(void* p_info_void)
{
    adpt_port_info_t* p_info = NULL;

    p_info = p_info_void;
    return (uint32_ofp)p_info->com_info.ofport;
}

/**
 * Port db compare two port info
 * @param p_info_in_bucket_void              Port in db
 * @param p_info_to_lkp_void                 Port to be compared
 * @return true/false
 */
static inline bool
adpt_portdb_ofport_hash_compare(void* p_info_in_bucket_void, void* p_info_to_lkp_void)
{
    adpt_port_info_t* p_info_in_bucket = NULL;
    adpt_port_info_t* p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->com_info.ofport != p_info_to_lkp->com_info.ofport)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Port db make hash key by gport
 * @param p_info_void                        Pointer to adpt_port_info_t
 * @return hash key
 */
static inline uint32_ofp
adpt_portdb_gport_hash_make(void* p_info_void)
{
    adpt_port_info_t* p_info = NULL;

    p_info = p_info_void;
    return (uint32_ofp)p_info->com_info.gport;
}

/**
 * Port db compare two port info
 * @param p_info_in_bucket_void              Port in db
 * @param p_info_to_lkp_void                 Port to be compared
 * @return true/false
 */
static inline bool
adpt_portdb_gport_hash_compare(void* p_info_in_bucket_void, void* p_info_to_lkp_void)
{
    adpt_port_info_t* p_info_in_bucket = NULL;
    adpt_port_info_t* p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->com_info.gport != p_info_to_lkp->com_info.gport)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Add port info to hash
 * @param  hash                         Pointer to ctc_hash_t
 * @param  p_port_info                  Pointer to adpt_port_info_t
 * @return OFP_ERR_XXX
 */
static inline int32_ofp 
adpt_portdb_hash_add(ctc_hash_t* hash, adpt_port_info_t* p_port_info)
{
    void* p_data = NULL;

    p_data = ctc_hash_insert(hash, p_port_info);
    ADPT_MEM_PTR_CHECK(p_data);

    return OFP_ERR_SUCCESS;
}

/**
 * Remove port info from hash
 * @param  hash                         Pointer to ctc_hash_t
 * @param  p_port_info                  Pointer to adpt_port_info_t
 * @return OFP_ERR_XXX
 */
static inline int32_ofp 
adpt_portdb_hash_del(ctc_hash_t* hash, adpt_port_info_t* p_port_info)
{
    ctc_hash_remove(hash, p_port_info);

    return OFP_ERR_SUCCESS;
}

/**
 * Add port info to list
 * @param  p_list                       Pointer to ctclib_list_t
 * @param  p_port_info                  Pointer to adpt_port_info_t
 * @return OFP_ERR_XXX
 */
static inline int32_ofp 
adpt_portdb_list_add(ctclib_list_t* p_list, adpt_port_info_t* p_port_info)
{
    ctclib_list_insert_tail(p_list, &p_port_info->node);

    return OFP_ERR_SUCCESS;
}

/**
 * Remove port info from list
 * @param  p_list                       Pointer to ctclib_list_t
 * @param  p_port_info                  Pointer to adpt_port_info_t
 * @return OFP_ERR_XXX
 */
static inline int32_ofp 
adpt_portdb_list_del(ctclib_list_t* p_list, adpt_port_info_t* p_port_info)
{
    ctclib_list_delete(p_list, &p_port_info->node);
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get port info pointer by ofport
 * @param[in] ofport                    ofport
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_ofport(uint16_ofp ofport, adpt_port_info_t** pp_port_info)
{
    adpt_port_info_t info_to_lkp;
    adpt_port_info_t* p_lkp_result = NULL;

    memset(&info_to_lkp, 0, sizeof(adpt_port_info_t));
    info_to_lkp.com_info.ofport = ofport;

    ADPT_PORT_INIT_CHECK();
    p_lkp_result = ctc_hash_lookup(ADPT_PORT_OFPORT_HASH, &info_to_lkp);
    if (NULL == p_lkp_result)
    {
        return OFP_ERR_PORT_NOT_EXIST;
    }
    *pp_port_info = p_lkp_result;

    return OFP_ERR_SUCCESS;
}

/**
 * Get port info pointer by name
 * @param[in] ifname                    interface name
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_name(const char* ifname, adpt_port_info_t** pp_port_info)
{
    adpt_port_info_t info_to_lkp;
    adpt_port_info_t* p_lkp_result = NULL;

    ADPT_PORT_INIT_CHECK();
    memset(&info_to_lkp, 0, sizeof(adpt_port_info_t));
    strncpy(info_to_lkp.com_info.name, ifname, OFP_IFNAME_SIZE);
    p_lkp_result = ctc_hash_lookup(ADPT_PORT_NAME_HASH, &info_to_lkp);
    if (NULL == p_lkp_result)
    {
        return OFP_ERR_PORT_NOT_EXIST;
    }
    *pp_port_info = p_lkp_result;

    return OFP_ERR_SUCCESS;
}

/**
 * Get port info pointer by gport
 * @param[in] gport                     gport
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_gport(uint16_ofp gport, adpt_port_info_t** pp_port_info)
{
    adpt_port_info_t info_to_lkp;
    adpt_port_info_t* p_lkp_result = NULL;
    
    ADPT_PORT_INIT_CHECK();
    memset(&info_to_lkp, 0, sizeof(adpt_port_info_t));
    info_to_lkp.com_info.gport = gport;
    
    p_lkp_result = ctc_hash_lookup(ADPT_PORT_GPORT_HASH, &info_to_lkp);
    if (NULL == p_lkp_result)
    {
        return OFP_ERR_PORT_NOT_EXIST;
    }
    *pp_port_info = p_lkp_result;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set port data by type
 * @param gport                         gport
 * @param data_type                     adpt_port_data_type_t
 * @param p_data                        pointer to port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_set_port_data(uint16_ofp gport, adpt_port_data_type_t data_type, void* p_data)
{
    adpt_port_info_t info_to_lkp;
    adpt_port_info_t* p_lkp_result = NULL;

    ADPT_PORT_INIT_CHECK();
    memset(&info_to_lkp, 0, sizeof(adpt_port_info_t));
    info_to_lkp.com_info.gport = gport;
    
    p_lkp_result = ctc_hash_lookup(ADPT_PORT_GPORT_HASH, &info_to_lkp);
    if (NULL == p_lkp_result)
    {
        return OFP_ERR_PORT_NOT_EXIST;
    }
    p_lkp_result->pv_data[data_type] = p_data;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set port data by type
 * @param[in]  gport                    gport
 * @param[in]  data_type                adpt_port_data_type_t
 * @param[out] pp_data                  pointer to port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_data(uint16_ofp gport, adpt_port_data_type_t data_type, void** pp_data)
{
    adpt_port_info_t info_to_lkp;
    adpt_port_info_t* p_lkp_result = NULL;

    ADPT_PORT_INIT_CHECK();
    memset(&info_to_lkp, 0, sizeof(adpt_port_info_t));
    info_to_lkp.com_info.gport = gport;
    
    p_lkp_result = ctc_hash_lookup(ADPT_PORT_GPORT_HASH, &info_to_lkp);
    if (NULL == p_lkp_result)
    {
        return OFP_ERR_PORT_NOT_EXIST;
    }
    *pp_data = p_lkp_result->pv_data[data_type];
    
    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_portdb_set_port_flag(uint16_ofp ofport, bool set, adpt_port_com_flag_t flag)
{
    adpt_port_info_t *p_info = NULL;
    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_info));
    if (set)
    {
        SET_FLAG(p_info->com_info.flag, flag);
        return OFP_ERR_SUCCESS;
    }
    UNSET_FLAG(p_info->com_info.flag, flag);
    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_portdb_check_port_flag(uint16_ofp ofport, adpt_port_com_flag_t flag)
{
    adpt_port_info_t *p_info = NULL;
    ADPT_ERROR_RETURN(adpt_portdb_get_port_info_by_ofport(ofport, &p_info));
    if (IS_FLAG_SET(p_info->com_info.flag, flag))
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * Add port to db
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_add_port(adpt_port_info_t* p_port_info)
{
    ADPT_PTR_CHECK(p_port_info);
    ADPT_PORT_INIT_CHECK();
    
    ADPT_ERROR_RETURN(adpt_portdb_list_add(ADPT_PORT_LIST[p_port_info->type], p_port_info));
    ADPT_ERROR_RETURN(adpt_portdb_hash_add(ADPT_PORT_NAME_HASH, p_port_info));
    ADPT_ERROR_RETURN(adpt_portdb_hash_add(ADPT_PORT_OFPORT_HASH, p_port_info));

    if (p_port_info->com_info.gport != OFP_INVALID_GPORT)
    {
        ADPT_ERROR_RETURN(adpt_portdb_hash_add(ADPT_PORT_GPORT_HASH, p_port_info));
    }

    ADPT_PORT_TYPE_PORT_NUM[p_port_info->type]++;

    return OFP_ERR_SUCCESS;
}

/**
 * Remove port from db
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_del_port(adpt_port_info_t* p_port_info)
{
    ADPT_PORT_INIT_CHECK();

    ADPT_ERROR_RETURN(adpt_portdb_list_del(ADPT_PORT_LIST[p_port_info->type], p_port_info));
    ADPT_ERROR_RETURN(adpt_portdb_hash_del(ADPT_PORT_NAME_HASH,   p_port_info));
    ADPT_ERROR_RETURN(adpt_portdb_hash_del(ADPT_PORT_OFPORT_HASH, p_port_info));

    if (OFP_INVALID_GPORT != p_port_info->com_info.gport)
    {
        ADPT_ERROR_RETURN(adpt_portdb_hash_del(ADPT_PORT_GPORT_HASH, p_port_info));
    }
    ADPT_PORT_TYPE_PORT_NUM[p_port_info->type]--;

    return OFP_ERR_SUCCESS;
}

/**
 *  show port info
 * @return OFP_ERR_XXX
 */
static int32_ofp 
adpt_portdb_show_port_info(adpt_port_info_t *p_info, void *user_data)
{
    /* " %6s %8s %11s %5s %7s \n", "ofport", "type", "name", "gport", "ifindex" */
    ctc_cli_out_ofp(" %6d %8s %11s %5u %7u\n", 
        p_info->com_info.ofport, OFP_MAP_INTF_TYPE_STR(p_info->type),
        p_info->com_info.name,
        p_info->com_info.gport,  
        p_info->com_info.ifindex);

    return OFP_ERR_SUCCESS;
}

/**
 * Loop all port, show port info
 * @return OFP_ERR_XXX
 */
void
adpt_portdb_traversal_show_port_info(void)
{
    ofp_interface_type_t type;
    ctclib_list_node_t *p_node;
    adpt_port_info_t* p_port_info = NULL;

    if (NULL == g_p_adpt_port_master)
    {
        ctc_cli_out_ofp("Port module is not initialized\n");
        return;
    }
    
    ctc_cli_out_ofp(" ------------------ Port LIST DB --------------------------------------\n");
    ctc_cli_out_ofp(" %6s %8s %11s %5s %7s \n", "ofport", "type", "name", "gport", "ifindex");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    
    for (type = 0; type < OFP_INTERFACE_TYPE_MAX; type ++)
    {
        ctclib_list_for_each(p_node, ADPT_PORT_LIST[type])
        {
            p_port_info = ctclib_container_of(p_node, adpt_port_info_t, node);
            
            adpt_portdb_show_port_info(p_port_info, NULL);
        }
    }

    ctc_cli_out_ofp(" \n------------------ Port NAME HASH DB ---------------------------------\n");
    ctc_cli_out_ofp(" %6s %8s %11s %5s %7s \n", "ofport", "type", "name", "gport", "ifindex");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_NAME_HASH, (hash_traversal_fn)adpt_portdb_show_port_info, NULL);

    ctc_cli_out_ofp("\n------------------ Port OFPORT HASH DB -------------------------------\n");
    ctc_cli_out_ofp(" %6s %8s %11s %5s %7s \n", "ofport", "type", "name", "gport", "ifindex");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_OFPORT_HASH, (hash_traversal_fn)adpt_portdb_show_port_info, NULL);

    ctc_cli_out_ofp("\n------------------ Port GPORT HASH DB ---------------------------------\n");
    ctc_cli_out_ofp(" %6s %8s %11s %5s %7s \n", "ofport", "type", "name", "gport", "ifindex");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn)adpt_portdb_show_port_info, NULL);

    ctc_cli_out_ofp("\nPort number of each types :\n");
    for (type = 0; type < OFP_INTERFACE_TYPE_MAX; type++)
    {
        if (ADPT_PORT_TYPE_PORT_NUM[type])
        {
            ctc_cli_out_ofp("%8s : %3d\n", OFP_MAP_INTF_TYPE_STR(type), ADPT_PORT_TYPE_PORT_NUM[type]);
        }
    }
}

static int32_ofp 
adpt_portdb_show_phy_info(adpt_port_info_t *p_info, void *user_data)
{
    int32_ofp type;
    adpt_port_phy_info_t* p_phy_info;
    char duplex[4][8] = {"AUTO", "HALF", "FULL", "MAX"}; /* glb_port_duplex_t */
    char speed[6][8]  = {"AUTO", "10M", "100M", "1G", "10G", "MAX"}; /* glb_port_speed_t */
    char link[3][8]   = {"DOWN", "UP", "MAX"}; /* glb_port_link_t */
    
    type = *(int32_ofp*)user_data;
    if (OFP_INTERFACE_TYPE_PHYSICAL != type)
    {
        return OFP_ERR_SUCCESS;
    }
    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_PHY_INFO])
    {
        return OFP_ERR_SUCCESS;
    }
    p_phy_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_PHY_INFO];
    
    /* refer to adpt_portdb_traversal_show_phy_info */
    ctc_cli_out_ofp(" %11s %5u %6s %6s %6s 0x%08x [%2u %6s %6s 0x%08x]\n", 
        p_info->com_info.name,
        p_info->com_info.gport,  
        duplex[p_phy_info->duplex],
        speed[p_phy_info->speed],
        link[p_phy_info->link],
        p_phy_info->media, 
        p_phy_info->port_cfg.enable,
        duplex[p_phy_info->port_cfg.duplex],
        speed[p_phy_info->port_cfg.speed],
        p_phy_info->port_cfg.media);

    return OFP_ERR_SUCCESS;
}

/**
 * Loop all port, show phy info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_phy_info(void)
{
    int32_ofp type = OFP_INTERFACE_TYPE_PHYSICAL;

    /* adpt_port_phy_info_t */
    ctc_cli_out_ofp(" ------------------ Port PHY INFO DB ---------------------------------\n");
    ctc_cli_out_ofp(" %11s %5s %6s %6s %6s %10s [%2s %6s %6s %10s]port-config\n", 
        "name", "gport", "duplex", "speed", "link", "media",
        "En", "duplex", "speed", "media");
    ctc_cli_out_ofp(" ---------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn)adpt_portdb_show_phy_info, &type);

    return OFP_ERR_SUCCESS;
}

static int32_ofp 
adpt_portdb_show_op_info(adpt_port_info_t *p_info, void *user_data)
{
    int32_ofp type;
    adpt_port_of_fea_info_t* p_op_info;
    
    type = *(int32_ofp*)user_data;
    if (OFP_INTERFACE_TYPE_PHYSICAL != type)
    {
        return OFP_ERR_SUCCESS;
    }
    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_OP_INFO])
    {
        return OFP_ERR_SUCCESS;
    }
    p_op_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_OP_INFO];
    
    /* refer to adpt_portdb_traversal_show_op_info */
    ctc_cli_out_ofp(" %11s %5u 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", 
        p_info->com_info.name,
        p_info->com_info.gport,  
        p_op_info->carrier,
        p_op_info->current,
        p_op_info->advertised,
        p_op_info->supported,
        p_op_info->peer);

    return OFP_ERR_SUCCESS;
}

/**
 * Loop all port, show openflow feature info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_op_info(void)
{
    int32_ofp type = OFP_INTERFACE_TYPE_PHYSICAL;

    /* adpt_port_of_fea_info_t */
    ctc_cli_out_ofp(" -------------------- Port OP INFO DB ---------------------------------\n");
    ctc_cli_out_ofp(" %11s %5s %10s %10s %10s %10s %10s\n", "name", "gport", "carrier", "current", "advertised", "supported", "peer");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn)adpt_portdb_show_op_info, &type);

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_portdb_show_status_info(adpt_port_info_t *p_info, void *user_data)
{
    adpt_port_status_info_t* p_staus_info;

    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_STATUS_INFO])
    {
        return OFP_ERR_SUCCESS;
    }

    p_staus_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_STATUS_INFO];

    /* refer to adpt_portdb_traversal_show_status_info */
    ctc_cli_out_ofp(" %11s %5s\n", p_info->com_info.name, p_staus_info->modified ? "Yes" : "No");

    return OFP_ERR_SUCCESS;
}

/**
 * Loop all port, show port status info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_status_info(void)
{
    /* adpt_port_status_info_t */
    ctc_cli_out_ofp(" -------------------- Port STATUS INFO DB -------\n");
    ctc_cli_out_ofp(" %11s %5s\n", "name", "modified");
    ctc_cli_out_ofp(" ------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn)adpt_portdb_show_status_info, NULL);

    return OFP_ERR_SUCCESS;
}

static int32_ofp 
adpt_portdb_show_nh_info(adpt_port_info_t *p_info, void *user_data)
{
    int32_ofp type;
    adpt_port_nexthop_info_t* p_nh_info;
    
    type = *(int32_ofp*)user_data;
    if (OFP_INTERFACE_TYPE_PHYSICAL != type)
    {
        return OFP_ERR_SUCCESS;
    }
    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_NH_INFO])
    {
        return OFP_ERR_SUCCESS;
    }
    p_nh_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_NH_INFO];
    
    /* refer to adpt_portdb_traversal_show_nh_info */
    ctc_cli_out_ofp(" %11s %5d %15d %15d\n",
        p_info->com_info.name,
        p_info->com_info.gport,
        p_nh_info->non_edit_nh.nhid,
        p_nh_info->non_edit_nh.offset);

    return OFP_ERR_SUCCESS;
}

/**
 * Loop all port, show nexthop info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_nh_info(void)
{
    int32_ofp type = OFP_INTERFACE_TYPE_PHYSICAL;

    /* ADPT_PORT_DATA_TYPE_NH_INFO */
    ctc_cli_out_ofp(" -------------------- Port NH INFO DB ---------------------------------\n");
    ctc_cli_out_ofp(" %11s %5s %15s %15s\n", "name", "gport", "non-edit-nhid", "dsnh-offset");
    ctc_cli_out_ofp(" ----------------------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn)adpt_portdb_show_nh_info, &type);

    return OFP_ERR_SUCCESS;
}

/**
 * Get port number of each type
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_num(ofp_interface_type_t type, uint32_ofp* p_port_num)
{
    ADPT_PTR_CHECK(p_port_num);
    ADPT_PORT_INIT_CHECK();
    
    *p_port_num = ADPT_PORT_TYPE_PORT_NUM[type];
    
    return OFP_ERR_SUCCESS;
}

static int32_ofp
_adpt_portdb_set_tunnel_port_modified(adpt_port_info_t *p_info, void *user_data)
{
    adpt_tunnel_port_modified_t* p_tunnel_port_modified;
    adpt_port_status_info_t* p_port_status_info;
    adpt_tunnel_info_t* p_tunnel_info;

    p_tunnel_port_modified = (adpt_tunnel_port_modified_t *)user_data;
    if (p_info->type != p_tunnel_port_modified->tunnel_type)
    {
        return OFP_ERR_SUCCESS;
    }

    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_STATUS_INFO])
    {
        return OFP_ERR_SUCCESS;
    }
    if (NULL == p_info->pv_data[ADPT_PORT_DATA_TYPE_TNL_INFO])
    {
        return OFP_ERR_SUCCESS;
    }
    p_port_status_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_STATUS_INFO];
    p_tunnel_info = p_info->pv_data[ADPT_PORT_DATA_TYPE_TNL_INFO];
    if (!strcmp(p_tunnel_info->bind_port_name, p_tunnel_port_modified->bind_ifname))
    {
        p_port_status_info->modified = p_tunnel_port_modified->is_modified;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Set tunnel port modified status by bind port
 * @param tunnel_port_modified     Tunnel port modified information
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_set_tunnel_port_modified_by_bind_port(adpt_tunnel_port_modified_t *tunnel_port_modified)
{
    ctc_hash_traverse(ADPT_PORT_GPORT_HASH, (hash_traversal_fn) _adpt_portdb_set_tunnel_port_modified,
            tunnel_port_modified);
    return OFP_ERR_SUCCESS;
}

/**
 * Adapter layer port db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_init(void)
{
    ofp_interface_type_t type;
    
    for (type = 0; type < OFP_INTERFACE_TYPE_MAX; type ++)
    {
        ctclib_list_init(ADPT_PORT_LIST[type]);
    }
    
    ADPT_PORT_NAME_HASH   = ctc_hash_create(1, ADPT_PORT_HASH_BLOCK_SIZE, 
        adpt_portdb_name_hash_make,
        adpt_portdb_name_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_PORT_NAME_HASH);
    
    ADPT_PORT_OFPORT_HASH = ctc_hash_create(1, ADPT_PORT_HASH_BLOCK_SIZE, 
        adpt_portdb_ofport_hash_make,
        adpt_portdb_ofport_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_PORT_OFPORT_HASH);
    
    ADPT_PORT_GPORT_HASH  = ctc_hash_create(1, ADPT_PORT_HASH_BLOCK_SIZE, 
        adpt_portdb_gport_hash_make,
        adpt_portdb_gport_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_PORT_GPORT_HASH);
    
    return OFP_ERR_SUCCESS;
}
