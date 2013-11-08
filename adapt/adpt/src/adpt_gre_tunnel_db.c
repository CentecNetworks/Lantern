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
 * @brief This file is the database implementation of adapter layer gre tunnel
 */

/******************************************************************************
* Header Files
******************************************************************************/

#include "vlog.h"
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_port.h"
#include "adpt_gre_tunnel.h"
#include "adpt_gre_tunnel_priv.h"


/******************************************************************************
* Defines and Macros
******************************************************************************/

/******************************************************************************
* Global and Declaration
******************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_gre_tunnel);

/******************************************************************************
*
* Functions
*
******************************************************************************/

static inline uint32_ofp
adpt_tunneldb_service_id_hash_make(void *service_id_info_void)
{
    adpt_tunnel_service_id_info_t *service_id_info = NULL;
    uint32_ofp key = 0;

    service_id_info = service_id_info_void;
    key = service_id_info->tunnel_port | service_id_info->tunnel_id;

    return key % ADPT_TUNNEL_SERVICE_ID_BLOCK_SIZE;
}

static inline bool
adpt_tunneldb_service_id_hash_compare(void *p_info_in_bucket_void, void *p_info_to_lkp_void)
{
    adpt_tunnel_service_id_info_t *p_info_in_bucket = NULL;
    adpt_tunnel_service_id_info_t *p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->tunnel_port != p_info_to_lkp->tunnel_port)
    {
        return FALSE;
    }
    if (p_info_in_bucket->tunnel_id != p_info_to_lkp->tunnel_id)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Add service id - tunnel port tunnel id combination to db
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id)
{
    adpt_tunnel_service_id_info_t info_to_lkp;
    adpt_tunnel_service_id_info_t* p_service_id_info = NULL;

    OFP_PTR_CHECK(ADPT_SERVICE_ID_HASH);

    memset(&info_to_lkp, 0, sizeof(info_to_lkp));
    info_to_lkp.tunnel_port = tunnel_port;
    info_to_lkp.tunnel_id   = tunnel_id;

    p_service_id_info = ctc_hash_lookup(ADPT_SERVICE_ID_HASH, &info_to_lkp);
    if (p_service_id_info)
    {
        if (p_service_id_info->service_id != service_id)
        {
            OFP_LOG_ERROR("Tunnel id reference error!\n");
            p_service_id_info->service_id = service_id;
        }
        p_service_id_info->ref ++;
    }
    else
    {
        p_service_id_info = malloc(sizeof(adpt_tunnel_service_id_info_t));
        OFP_MEM_PTR_CHECK(p_service_id_info);

        p_service_id_info->tunnel_port = tunnel_port;
        p_service_id_info->tunnel_id   = tunnel_id;
        p_service_id_info->service_id  = service_id;
        p_service_id_info->ref         = 1;

        ctc_hash_insert(ADPT_SERVICE_ID_HASH, p_service_id_info);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Lookup service id from db by tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             Pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_lookup_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp* p_service_id)
{
    adpt_tunnel_service_id_info_t info_to_lkp;
    adpt_tunnel_service_id_info_t* p_service_id_info = NULL;

    OFP_PTR_CHECK(p_service_id);
    OFP_PTR_CHECK(ADPT_SERVICE_ID_HASH);

    memset(&info_to_lkp, 0, sizeof(info_to_lkp));
    info_to_lkp.tunnel_port = tunnel_port;
    info_to_lkp.tunnel_id   = tunnel_id;

    p_service_id_info = ctc_hash_lookup(ADPT_SERVICE_ID_HASH, &info_to_lkp);
    if (p_service_id_info)
    {
        *p_service_id = p_service_id_info->service_id;
        return OFP_ERR_SUCCESS;
    }

    return OFP_ERR_ENTRY_NOT_EXIST;
}

/**
 * Delete service id from db.
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id)
{
    adpt_tunnel_service_id_info_t info_to_lkp;
    adpt_tunnel_service_id_info_t* p_service_id_info = NULL;

    OFP_PTR_CHECK(ADPT_SERVICE_ID_HASH);

    memset(&info_to_lkp, 0, sizeof(info_to_lkp));
    info_to_lkp.tunnel_port = tunnel_port;
    info_to_lkp.tunnel_id   = tunnel_id;

    p_service_id_info = ctc_hash_lookup(ADPT_SERVICE_ID_HASH, &info_to_lkp);
    if (p_service_id_info)
    {
        if (p_service_id_info->service_id != service_id)
        {
            OFP_LOG_ERROR("Tunnel id reference error!\n");
        }
        if (!p_service_id_info->ref)
        {
            OFP_LOG_ERROR("Tunnel id reference error!\n");
            return OFP_ERR_SUCCESS;
        }
        p_service_id_info->ref --;
        if (!p_service_id_info->ref)
        {
            ctc_hash_remove(ADPT_SERVICE_ID_HASH, p_service_id_info);
            free(p_service_id_info);
        }
    }
    else
    {
        OFP_LOG_ERROR("Tunnel id reference error!\n");
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunneldb_tunnel_port_service_loop_fn(adpt_tunnel_service_id_info_t *service_id_info, adpt_tunnel_loop_data_t *loop_data)
{
    OFP_PTR_CHECK(loop_data);
    OFP_PTR_CHECK(loop_data->func);

    return loop_data->func(service_id_info->service_id, loop_data->pv_arg);
}

/**
 * Loop all created service id
 * @param func                          ADPT_TUNNEL_SERVICE_ID_CB_FUNC
 * @param pv_arg                        pointer argv
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_loop_tunnel_port_service_id(ADPT_TUNNEL_SERVICE_ID_CB_FUNC func, void* pv_arg)
{
    adpt_tunnel_loop_data_t loopdata;
    loopdata.func = func;
    loopdata.pv_arg = pv_arg;

    return ctc_hash_traverse(ADPT_SERVICE_ID_HASH, (hash_traversal_fn)adpt_tunneldb_tunnel_port_service_loop_fn, &loopdata);
}

static int32_ofp
adpt_tunneldb_show_service_id_info(adpt_tunnel_service_id_info_t *p_info, void *data)
{
    int32_ofp* p_tunnel_port = data;

    ADPT_PTR_CHECK(p_info);
    ADPT_PTR_CHECK(data);

    if (p_tunnel_port && (*p_tunnel_port == p_info->tunnel_port))
    {
        ctc_cli_out_ofp(" %11u %9u %10u %9u\n",
            p_info->tunnel_port, p_info->tunnel_id, p_info->service_id, p_info->ref);
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Show service id db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_service_id(void)
{
    int32_ofp tunnel_port;

    ADPT_MODULE_INIT_CHECK(g_p_tunnel_master);
    ADPT_MODULE_INIT_CHECK(ADPT_SERVICE_ID_HASH);

    ctc_cli_out_ofp("The Maximum combination of tunnel port and tunnel id is %u, currently is %u \n\n",
        OFP_TUNNEL_SERVICE_ID_NUM, ADPT_SERVICE_ID_HASH->count);

    ctc_cli_out_ofp(" Tunnel-port Tunnel-id Service-id Reference\n");
    ctc_cli_out_ofp(" ----------- --------- ---------- ----------\n");

    for (tunnel_port = OFP_TUNNEL_PORT_NO_BASE + 1; tunnel_port < OFP_TUNNEL_PORT_NO_MAX; tunnel_port ++)
    {
        ctc_hash_traverse(ADPT_SERVICE_ID_HASH, (hash_traversal_fn)adpt_tunneldb_show_service_id_info, &tunnel_port);
    }

    return OFP_ERR_SUCCESS;
}

static inline uint32_ofp
adpt_tunneldb_local_ip_hash_make(void *p_local_ip_info_void)
{
    adpt_tunnel_local_ip_info_t *p_local_ip_info = NULL;
    uint32_ofp key = 0;

    p_local_ip_info = p_local_ip_info_void;
    key = p_local_ip_info->local_ip;

    return key % ADPT_TUNNEL_LOCAL_IP_BLOCK_SIZE;
}

static inline bool
adpt_tunneldb_local_ip_hash_compare(void *p_info_in_bucket_void, void *p_info_to_lkp_void)
{
    adpt_tunnel_local_ip_info_t *p_info_in_bucket = NULL;
    adpt_tunnel_local_ip_info_t *p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->local_ip != p_info_to_lkp->local_ip)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Add tunnel port's local ip to db for repeat checking
 * @param local_ip                      local ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_local_ip(uint32_ofp local_ip)
{
    adpt_tunnel_local_ip_info_t info_to_lkp;
    adpt_tunnel_local_ip_info_t* p_local_ip_info = NULL;
    uint32_ofp local_ip_cnt = 0;

    info_to_lkp.local_ip = local_ip;

    p_local_ip_info = ctc_hash_lookup(ADPT_LOCAL_IP_HASH, &info_to_lkp);

    if (p_local_ip_info)
    {
        p_local_ip_info->ref ++;
    }
    else
    {
        ctc_hash_get_count(ADPT_LOCAL_IP_HASH, &local_ip_cnt);
        if (local_ip_cnt >= OFP_TUNNEL_MAX_LOCAL_IP_NUM)
        {
            return OFP_ERR_TUNNEL_LOCAL_IP_FULL;
        }
        p_local_ip_info = malloc(sizeof(adpt_tunnel_local_ip_info_t));
        OFP_MEM_PTR_CHECK(p_local_ip_info);

        p_local_ip_info->local_ip = local_ip;
        p_local_ip_info->ref = 1;

        ctc_hash_insert(ADPT_LOCAL_IP_HASH, p_local_ip_info);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Delete tunnel port's local ip from db
 * @param local_ip                      local ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_local_ip(uint32_ofp local_ip)
{
    adpt_tunnel_local_ip_info_t info_to_lkp;
    adpt_tunnel_local_ip_info_t* p_local_ip_info = NULL;

    info_to_lkp.local_ip = local_ip;
    p_local_ip_info = ctc_hash_lookup(ADPT_LOCAL_IP_HASH, &info_to_lkp);

    if (p_local_ip_info)
    {
        if (!p_local_ip_info->ref)
        {
            OFP_LOG_ERROR("Tunnel local ip reference error!\n");
            return OFP_ERR_SUCCESS;
        }
        p_local_ip_info->ref --;
        if (!p_local_ip_info->ref)
        {
            ctc_hash_remove(ADPT_LOCAL_IP_HASH, p_local_ip_info);
            free(p_local_ip_info);
        }
    }
    else
    {
        OFP_LOG_ERROR("Tunnel local ip reference error!\n");
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunneldb_show_local_ip_info(adpt_tunnel_local_ip_info_t *p_local_ip_info, int *p_index)
{
    char addr[16];
    uint32_ofp local_ip_network_byte_order = 0;

    local_ip_network_byte_order = htonl(p_local_ip_info->local_ip);
    inet_ntop(AF_INET, &local_ip_network_byte_order, addr, 16);

    ctc_cli_out_ofp("index= %2u, local_ip= %s, ref = %u\n",
        (*p_index) ++, addr, p_local_ip_info->ref);

    return OFP_ERR_SUCCESS;
}

/**
 * Show local ip db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_local_ip(void)
{
    int32_ofp i = 0;
    uint32_ofp local_ip_cnt;

    ADPT_MODULE_INIT_CHECK(g_p_tunnel_master);
    ADPT_MODULE_INIT_CHECK(ADPT_LOCAL_IP_HASH);

    ctc_hash_get_count(ADPT_LOCAL_IP_HASH, &local_ip_cnt);

    ctc_cli_out_ofp("The Maximum of local ip is %u, currently is %u \n",
        OFP_TUNNEL_MAX_LOCAL_IP_NUM, local_ip_cnt);
    ctc_cli_out_ofp("----------------------------------------------------\n");

    ctc_hash_traverse(ADPT_LOCAL_IP_HASH, (hash_traversal_fn)adpt_tunneldb_show_local_ip_info, &i);

    return OFP_ERR_SUCCESS;
}

static inline uint32_ofp
adpt_tunneldb_local_remote_ip_hash_make(void *p_tnl_ip_info_void)
{
    adpt_tunnel_local_remote_ip_info_t *p_tnl_ip_info = NULL;
    uint32_ofp key = 0;

    p_tnl_ip_info =  p_tnl_ip_info_void;
    key = p_tnl_ip_info->local_ip | p_tnl_ip_info->remote_ip;

    return key % ADPT_TUNNEL_LOCAL_REMOTE_IP_BLOCK_SIZE;
}

static inline bool
adpt_tunneldb_local_remote_ip_hash_compare(void *p_info_in_bucket_void,
                                           void *p_info_to_lkp_void)
{
    adpt_tunnel_local_remote_ip_info_t *p_info_in_bucket = NULL;
    adpt_tunnel_local_remote_ip_info_t *p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->local_ip != p_info_to_lkp->local_ip)
    {
        return FALSE;
    }
    if (p_info_in_bucket->remote_ip != p_info_to_lkp->remote_ip)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Add tunnel port's local ip and remote ip to db
 * @param local_ip                      local ip address
 * @param remote_ip                     remote ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_local_remote_ip(uint32_ofp local_ip, uint32_ofp remote_ip)
{
    adpt_tunnel_local_remote_ip_info_t info_to_lkp;
    adpt_tunnel_local_remote_ip_info_t* p_ip_info = NULL;

    OFP_PTR_CHECK(ADPT_LOCAL_REMOTE_IP_HASH);

    memset(&info_to_lkp, 0, sizeof(info_to_lkp));
    info_to_lkp.local_ip  = local_ip;
    info_to_lkp.remote_ip = remote_ip;
    
    p_ip_info = ctc_hash_lookup(ADPT_LOCAL_REMOTE_IP_HASH, &info_to_lkp);
    if (p_ip_info)
    {
        return OFP_ERR_TUNNEL_REPEAT_LOCAL_REMOTE_IP;
    }
    
    p_ip_info = malloc(sizeof(adpt_tunnel_local_remote_ip_info_t));
    OFP_MEM_PTR_CHECK(p_ip_info);

    p_ip_info->local_ip  = local_ip;
    p_ip_info->remote_ip = remote_ip;
    p_ip_info->ref       = 1;

    ctc_hash_insert(ADPT_LOCAL_REMOTE_IP_HASH, p_ip_info);

    return OFP_ERR_SUCCESS;
}

/**
 * Delete tunnel port's local ip and remote ip from db
 * @param local_ip                      local ip address
 * @param remote_ip                     remote ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_local_remote_ip(uint32_ofp local_ip, uint32_ofp remote_ip)
{
    adpt_tunnel_local_remote_ip_info_t info_to_lkp;
    adpt_tunnel_local_remote_ip_info_t* p_ip_info = NULL;

    OFP_PTR_CHECK(ADPT_LOCAL_REMOTE_IP_HASH);

    memset(&info_to_lkp, 0, sizeof(info_to_lkp));
    info_to_lkp.local_ip  = local_ip;
    info_to_lkp.remote_ip = remote_ip;

    p_ip_info = ctc_hash_lookup(ADPT_LOCAL_REMOTE_IP_HASH, &info_to_lkp);
    if (p_ip_info)
    {
        if (!p_ip_info->ref)
        {
            OFP_LOG_ERROR("Tunnel local remote ip reference error!\n");
            return OFP_ERR_SUCCESS;
        }
        p_ip_info->ref --;
        if (!p_ip_info->ref)
        {
            ctc_hash_remove(ADPT_LOCAL_REMOTE_IP_HASH, p_ip_info);
            free(p_ip_info);
        }
    }
    else
    {
        OFP_LOG_ERROR("Tunnel local remote ip reference error!\n");
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunneldb_show_local_remote_ip_info(adpt_tunnel_local_remote_ip_info_t *p_info, void *data)
{
    int32_ofp* p_index = data;
    char addr1[16];
    char addr2[16];
    uint32_ofp local_ip_network_byte_order = 0;
    uint32_ofp remote_ip_network_byte_order = 0;

    local_ip_network_byte_order = htonl(p_info->local_ip);
    remote_ip_network_byte_order = htonl(p_info->remote_ip);

    inet_ntop(AF_INET, &local_ip_network_byte_order, addr1, 16);
    inet_ntop(AF_INET, &remote_ip_network_byte_order, addr2, 16);

    ctc_cli_out_ofp("index= %2u, local_ip = %s, remote_ip = %s, ref = %u\n",
        (*p_index)++, addr1, addr2, p_info->ref);

    return OFP_ERR_SUCCESS;
}

/**
 * Show local ip and remote ip db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_local_remote_ip(void)
{
    int32_ofp index = 0;

    ADPT_MODULE_INIT_CHECK(g_p_tunnel_master);
    ADPT_MODULE_INIT_CHECK(ADPT_LOCAL_REMOTE_IP_HASH);

    ctc_cli_out_ofp("The Maximum of local remote ip is %u, currently is %u \n",
        OFP_TUNNEL_MAX_LOCAL_IP_NUM, ADPT_LOCAL_REMOTE_IP_HASH->count);

    ctc_cli_out_ofp("---------------------------------------------------------\n");
    ctc_hash_traverse(ADPT_LOCAL_REMOTE_IP_HASH, (hash_traversal_fn)adpt_tunneldb_show_local_remote_ip_info, &index);

    return OFP_ERR_SUCCESS;
}

static uint32_ofp
adpt_tunneldb_bind_port_hash_make(void *p_tnl_ip_info_void)
{
    adpt_tunnel_bind_port_info_t *p_tnl_ip_info = NULL;
    uint32_ofp key = 0;

    p_tnl_ip_info = p_tnl_ip_info_void;
    key = p_tnl_ip_info->bind_port;

    return key % ADPT_TUNNEL_BIND_PORT_BLOCK_SIZE;
}

static inline bool
adpt_tunneldb_bind_port_hash_compare(void *p_info_in_bucket_void,
                                     void *p_info_to_lkp_void)
{
    adpt_tunnel_bind_port_info_t *p_info_in_bucket = NULL;
    adpt_tunnel_bind_port_info_t *p_info_to_lkp = NULL;

    p_info_in_bucket = p_info_in_bucket_void;
    p_info_to_lkp = p_info_to_lkp_void;

    if (p_info_in_bucket->bind_port != p_info_to_lkp->bind_port)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * Add tunnel port's bind_port to db
 * @param bind_port                     bind port id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_bind_port(uint16_ofp bind_port)
{
    adpt_tunnel_bind_port_info_t* p_bind_port_info = NULL;
    adpt_tunnel_bind_port_info_t info_to_lkp;

    info_to_lkp.bind_port = bind_port;

    p_bind_port_info = ctc_hash_lookup(ADPT_BIND_PORT_HASH, &info_to_lkp);

    if (p_bind_port_info)
    {
        p_bind_port_info->ref ++;
    }
    else
    {
        p_bind_port_info = malloc(sizeof(adpt_tunnel_bind_port_info_t));
        OFP_MEM_PTR_CHECK(p_bind_port_info);

        p_bind_port_info->bind_port = bind_port;
        p_bind_port_info->ref = 1;

        ctc_hash_insert(ADPT_BIND_PORT_HASH, p_bind_port_info);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Check whether bind port is in db
 * @param bind_port                     bind port id
 * @return true, false
 */
bool
adpt_tunneldb_has_bind_port(uint16_ofp bind_port)
{
    adpt_tunnel_bind_port_info_t* p_bind_port_info = NULL;
    adpt_tunnel_bind_port_info_t info_to_lkp;

    info_to_lkp.bind_port = bind_port;

    p_bind_port_info = ctc_hash_lookup(ADPT_BIND_PORT_HASH, &info_to_lkp);

    return p_bind_port_info == NULL ? false : true;
}

/**
 * Delete tunnel port's bind port from db
 * @param bind_port                     bind port id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_bind_port(uint16_ofp bind_port)
{
    adpt_tunnel_bind_port_info_t* p_bind_port_info = NULL;
    adpt_tunnel_bind_port_info_t info_to_lkp;
    
    info_to_lkp.bind_port = bind_port;
    
    p_bind_port_info = ctc_hash_lookup(ADPT_BIND_PORT_HASH, &info_to_lkp);
    if (p_bind_port_info)
    {
        if (!p_bind_port_info->ref)
        {
            OFP_LOG_ERROR("Tunnel bind port reference error!\n");
            return OFP_ERR_SUCCESS;
        }
        p_bind_port_info->ref --;
        if (!p_bind_port_info->ref)
        {
            ctc_hash_remove(ADPT_BIND_PORT_HASH, p_bind_port_info);
            free(p_bind_port_info);
        }
    }
    else
    {
        OFP_LOG_ERROR("Tunnel bind port reference error!\n");
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_tunneldb_show_bind_port_info(adpt_tunnel_bind_port_info_t *p_bind_port_info, void *data)
{
    int32_ofp* p_index = data;
    char name[OFP_IFNAME_SIZE] = {0};

    ADPT_ERROR_RETURN(adpt_port_get_name_by_gport(p_bind_port_info->bind_port, name));
    
    ctc_cli_out_ofp("index: %2u, bind_port: %u, name = %s, ref = %u\n",
        (*p_index)++, p_bind_port_info->bind_port, name, p_bind_port_info->ref);

    return OFP_ERR_SUCCESS;
}

/**
 * Show bind port db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_bind_port(void)
{
    int32_ofp i = 0;

    ctc_cli_out_ofp("Print tunnel bind port DB\n");
    ctc_cli_out_ofp("--------------------------------------\n");
    ctc_hash_traverse(ADPT_BIND_PORT_HASH, (hash_traversal_fn)adpt_tunneldb_show_bind_port_info, &i);

    return OFP_ERR_SUCCESS;
}

/**
 * Show tunnel port db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_tunnel_port(void)
{
#define TEMP_STR_LEN 32
    adpt_tunnel_info_t* p_tunnel_info = NULL;
    int32_ofp i = 0;
    int32_ofp ret;
    char name[16] = {0};
    char vlan_id_str[5] = {0};
    int32_ofp tnl_ofport;
    char local_ip_str[TEMP_STR_LEN] = {0};
    char remot_ip_str[TEMP_STR_LEN] = {0};
    char mac_str[TEMP_STR_LEN] = {0};
    adpt_port_of_fea_info_t fea_info = {0};
    uint32_ofp port_max;
    uint32_ofp port_count;
    uint8_ofp link_status = 0;
    uint16_ofp vlan_id = 0;
    uint32_ofp local_ip_network_byte_order = 0;
    uint32_ofp remote_ip_network_byte_order = 0;

    adpt_port_get_tunnel_port_max(&port_max);
    adpt_port_get_tunnel_port_count(&port_count);

    ctc_cli_out_ofp("The Maximum of tunnel ports is %u, currently %u tunnel ports is valid.\n\n",
        port_max, port_count);
    if (port_count == 0)
    {
        return OFP_ERR_SUCCESS;
    }

    ctc_cli_out_ofp(" %5s %8s %6s %5s %9s %15s %15s %18s %7s %4s\n",
        "index", "type", "name", "port", "bind_port", "local_ip", "remote_ip", "remote_mac", "vlan_id", "link");
    ctc_cli_out_ofp(" ----- -------- ------ ----- --------- ---------------- --------------- ----------------- ------- ----\n");
    for (tnl_ofport = OFP_TUNNEL_PORT_NO_BASE + 1; tnl_ofport < OFP_TUNNEL_PORT_NO_MAX; tnl_ofport ++)
    {
        ret = adpt_port_get_port_data(tnl_ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, (void**)&p_tunnel_info);
        if (ret)
        {
            continue;
        }
        adpt_port_get_name_by_ofport(tnl_ofport, name);
        adpt_port_get_of_fea_info(p_tunnel_info->bind_port_name, &fea_info);
        link_status = fea_info.carrier ? 1 : 0;

        local_ip_network_byte_order = htonl(p_tunnel_info->local_ip);
        remote_ip_network_byte_order = htonl(p_tunnel_info->remote_ip);
        inet_ntop(AF_INET, &local_ip_network_byte_order, local_ip_str, 16);
        inet_ntop(AF_INET, &remote_ip_network_byte_order, remot_ip_str, 16);

        sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p_tunnel_info->nexthop_mac[0], p_tunnel_info->nexthop_mac[1],
            p_tunnel_info->nexthop_mac[2], p_tunnel_info->nexthop_mac[3],
            p_tunnel_info->nexthop_mac[4], p_tunnel_info->nexthop_mac[5]);

        vlan_id = p_tunnel_info->vlan_id;
        if (vlan_id == OFP_DEFAULT_VLAN_ID)
        {
            sprintf(vlan_id_str, "None");
        }
        else
        {
           sprintf(vlan_id_str, "%u", vlan_id);
        }

        ctc_cli_out_ofp(" %5d %8s %6s %5d %9s %15s %15s %18s %7s %4s\n",
            i,
            "GRE",
            name,
            p_tunnel_info->ofport,
            p_tunnel_info->bind_port_name,
            local_ip_str,
            remot_ip_str,
            mac_str,
            vlan_id_str,
            link_status ? "UP" : "DOWN");

        i++;
    }
#undef TEMP_STR_LEN

    return OFP_ERR_SUCCESS;
}

/**
 * Show tunnel info db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_tunnel_info(void)
{
    int32_ofp i = 0;
    int32_ofp tnl_ofport;
    adpt_tunnel_info_t* p_tunnel_info = NULL;
    char name[16];
    uint32_ofp port_max;
    uint32_ofp port_count;
    int32_ofp  ret;

    adpt_port_get_tunnel_port_max(&port_max);
    adpt_port_get_tunnel_port_count(&port_count);

    ctc_cli_out_ofp("The Maximum of tunnel ports is %u, currently %u tunnel ports is valid\n\n",
        port_max, port_count);

    ctc_cli_out_ofp(" %5s %8s %6s %5s %10s %10s %5s %10s\n",
        "index", "type", "name", "port", "decap_wk", "decap_wok", "iloop", "service_id");
    ctc_cli_out_ofp(" ----- -------- ------ ----- --------- ---------------- --------------- -------------------\n");
    for (tnl_ofport = OFP_TUNNEL_PORT_NO_BASE + 1; tnl_ofport < OFP_TUNNEL_PORT_NO_MAX; tnl_ofport ++)
    {
        p_tunnel_info = NULL;
        ret = adpt_port_get_port_data(tnl_ofport, ADPT_PORT_DATA_TYPE_TNL_INFO, (void**)&p_tunnel_info);
        if (ret || NULL == p_tunnel_info)
        {
            continue;
        }
        adpt_port_get_name_by_ofport(tnl_ofport, name);

        ctc_cli_out_ofp(" %5d %8s %6s %5u %10u %10u %5u %10u\n",
            i,
            "GRE",
            name,
            p_tunnel_info->ofport,
            p_tunnel_info->decap_nhid_wk,
            p_tunnel_info->decap_nhid_wok,
            p_tunnel_info->iloop_port,
            p_tunnel_info->service_id);

        i++;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Adapter layer gre tunnel db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_init(void)
{
    ADPT_MODULE_INIT_CHECK(g_p_tunnel_master);

    ADPT_SERVICE_ID_HASH = ctc_hash_create(1, ADPT_TUNNEL_SERVICE_ID_BLOCK_SIZE,
        adpt_tunneldb_service_id_hash_make,
        adpt_tunneldb_service_id_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_SERVICE_ID_HASH);

    ADPT_LOCAL_REMOTE_IP_HASH = ctc_hash_create(1, ADPT_TUNNEL_LOCAL_REMOTE_IP_BLOCK_SIZE,
        adpt_tunneldb_local_remote_ip_hash_make,
        adpt_tunneldb_local_remote_ip_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_LOCAL_REMOTE_IP_HASH);

    ADPT_LOCAL_IP_HASH = ctc_hash_create(1, ADPT_TUNNEL_LOCAL_IP_BLOCK_SIZE,
        adpt_tunneldb_local_ip_hash_make,
        adpt_tunneldb_local_ip_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_LOCAL_IP_HASH);

    ADPT_BIND_PORT_HASH = ctc_hash_create(1, ADPT_TUNNEL_BIND_PORT_BLOCK_SIZE,
        adpt_tunneldb_bind_port_hash_make,
        adpt_tunneldb_bind_port_hash_compare);
    ADPT_MEM_PTR_CHECK(ADPT_BIND_PORT_HASH);

    g_p_tunnel_master->tunnel_port_count = 0;
    g_p_tunnel_master->tunnel_port_max = OFP_TUNNEL_PORT_NO_NUM;

    ihash_init(&g_p_tunnel_master->bind_port_ihmap);

    return OFP_ERR_SUCCESS;
}
