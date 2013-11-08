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
 * @brief This file is the private header for adapter layer gre tunnel
 */

#ifndef __ADPT_GRE_TUNNEL_PRIV_H__
#define __ADPT_GRE_TUNNEL_PRIV_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ihash.h"
#include "hash.h"
#include "ctc_hash.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
#define ADPT_TUNNEL_SERVICE_ID_BLOCK_SIZE       256
#define ADPT_TUNNEL_LOCAL_REMOTE_IP_BLOCK_SIZE  64
#define ADPT_TUNNEL_LOCAL_IP_BLOCK_SIZE         64
#define ADPT_TUNNEL_BIND_PORT_BLOCK_SIZE        64

#define OFP_TUNNEL_DEFAULT_VID_FOR_ENCAP 4095
#define OFP_TUNNEL_DEFAULT_MTU_SIZE 1518
#define OFP_TUNNEL_DEFAULT_SET_TTL  64
#define OFP_TUNNEL_DEFAULT_SET_TOS  0

/****************************************************************************
 *
 * Structures and macros, enums
 *
 ****************************************************************************/

/**
* @brief Tunnel service id info, for maintaining tunnel-port + tunnel-id -> service-id map
*/
struct adpt_tunnel_service_id_info_s
{
    uint32_ofp tunnel_port;
    uint32_ofp tunnel_id;

    uint32_ofp service_id;
    uint32_ofp ref;
};
typedef struct adpt_tunnel_service_id_info_s adpt_tunnel_service_id_info_t;

/**
* @brief For traversing all tunnel ports
*/
struct adpt_tunnel_loop_data_s
{
    ADPT_TUNNEL_SERVICE_ID_CB_FUNC func;
    void* pv_arg;
};
typedef struct adpt_tunnel_loop_data_s adpt_tunnel_loop_data_t;

/**
* @brief For maintaining local-ip reference
*/
struct adpt_tunnel_local_ip_info_s
{
    uint32_ofp local_ip;

    uint32_ofp ref;
};
typedef struct adpt_tunnel_local_ip_info_s adpt_tunnel_local_ip_info_t;

/**
* @brief For maintaining reference of the combination of local-ip and remote ip
*/
struct adpt_tunnel_local_remote_ip_info_s
{
    uint32_ofp local_ip;
    uint32_ofp remote_ip;

    uint32_ofp ref;
};
typedef struct adpt_tunnel_local_remote_ip_info_s adpt_tunnel_local_remote_ip_info_t;

/**
* @brief tunnel bind port information
*/
struct adpt_tunnel_bind_port_info_s
{
    uint16_ofp bind_port;
    uint16_ofp resv;

    uint32_ofp ref;
};
typedef struct adpt_tunnel_bind_port_info_s adpt_tunnel_bind_port_info_t;

/**
* @brief Tunnel info data structure
*/
struct adpt_tunnel_info_s
{
    ofp_interface_type_t type;              /**< interface type */
    uint32_ofp flag;                        /**< bitmap of ofp_tunnel_flag_t */

    uint32_ofp remote_ip;                   /**< remote ip address */
    uint32_ofp local_ip;                    /**< local ip address */
    uint8_ofp nexthop_mac[OFP_ETH_ADDR_LEN];/**< nexthop mac address */
    uint16_ofp vlan_id;
    char bind_port_name[OFP_IFNAME_SIZE];   /**< bind port name */

    /* private data */
    uint16_ofp ofport;                      /**< OVS port number */
    uint16_ofp bind_port;                   /**< bind gport */

    uint32_ofp decap_nhid_wk;  /**< decapsulation with gre key nhid */
    uint32_ofp decap_nhid_wok; /**< decapsulation without gre key nhid */
    uint16_ofp iloop_port;                  /**< decapsulation iloop gport */
    uint32_ofp service_id;                  /**< encapsulation service id without gre key */
};
typedef struct adpt_tunnel_info_s adpt_tunnel_info_t;

/**
* @brief adapter layer tunnel master data structure
*/
struct adpt_tunnel_master_s
{
    ctc_hash_t* service_id_hash;
    struct ihash bind_port_ihmap;
    ctc_hash_t* local_ip_hash;
    ctc_hash_t* local_remote_ip_hash;
    ctc_hash_t* bind_port_hash;

    uint32_ofp tunnel_port_count;
    uint32_ofp tunnel_port_max;

    uint32_ofp miss_match_nhid;
};
typedef struct adpt_tunnel_master_s adpt_tunnel_master_t;

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
extern adpt_tunnel_master_t* g_p_tunnel_master;
#define ADPT_SERVICE_ID_HASH      g_p_tunnel_master->service_id_hash
#define ADPT_LOCAL_REMOTE_IP_HASH g_p_tunnel_master->local_remote_ip_hash
#define ADPT_LOCAL_IP_HASH        g_p_tunnel_master->local_ip_hash
#define ADPT_BIND_PORT_HASH       g_p_tunnel_master->bind_port_hash

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 * Add service id - tunnel port tunnel id combination to db
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id);

/**
 * Lookup service id from db by tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             Pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_lookup_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp* p_service_id);

/**
 * Delete service id from db.
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id);

/**
 * Loop all created service id
 * @param func                          ADPT_TUNNEL_SERVICE_ID_CB_FUNC
 * @param pv_arg                        pointer argv
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_loop_tunnel_port_service_id(ADPT_TUNNEL_SERVICE_ID_CB_FUNC func, void* pv_arg);

/**
 * Add tunnel port's local ip to db for repeat checking
 * @param local_ip                      local ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_local_ip(uint32_ofp local_ip);

/**
 * Delete tunnel port's local ip from db
 * @param local_ip                      local ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_local_ip(uint32_ofp local_ip);

/**
 * Add tunnel port's local ip and remote ip to db
 * @param local_ip                      local ip address
 * @param remote_ip                     remote ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_local_remote_ip(uint32_ofp local_ip, uint32_ofp remote_ip);

/**
 * Delete tunnel port's local ip and remote ip from db
 * @param local_ip                      local ip address
 * @param remote_ip                     remote ip address
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_local_remote_ip(uint32_ofp local_ip, uint32_ofp remote_ip);

/**
 * Add tunnel port's bind_port to db
 * @param bind_port                     bind port id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_add_bind_port(uint16_ofp bind_port);

/**
 * Check whether bind port is in db
 * @param bind_port                     bind port id
 * @return true, false
 */
bool
adpt_tunneldb_has_bind_port(uint16_ofp bind_port);

/**
 * Delete tunnel port's bind port from db
 * @param bind_port                     bind port id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_del_bind_port(uint16_ofp bind_port);

/**
 * Get tunnel info by ofport
 * @param ofport                        tunnel port
 * @return OFP_ERR_XXX
 */
adpt_tunnel_info_t *
adpt_tunnel_get_tunnel_info_by_ofport(uint16_ofp ofport);

/**
 * Adapter layer gre tunnel db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_init(void);

#endif
