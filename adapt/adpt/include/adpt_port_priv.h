
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
 * @brief This file is the private header file of adpt_port.c
 */

#ifndef _OFP_PORT_PRIV_H_
#define _OFP_PORT_PRIV_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_hash.h"
#include "ctclib_list.h"
#include "glb_hw_define.h"
#include "glb_l2_define.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define VLAN_CONFIG_MAX_LINE_LEN    80
#define VLAN_STR_BUFFER_LEN         10

/**
 @brief port bridge information
*/
struct adpt_bridge_port_s
{
    uint16_ofp native_vlan;
    adpt_bridge_port_mode_t mode;
    l2_vlan_bmp_t bmp;
};
typedef struct adpt_bridge_port_s adpt_bridge_port_t;

/**
 @brief port common flag
*/
enum adpt_port_com_flag_e
{
    ADPT_PORT_COM_FLAG_OPENFLOW_EN = 1 << 0,
    ADPT_PORT_COM_FLAG_HYBRID_EN = 1 << 1,
    ADPT_PORT_COM_FLAG_SHUTDOWN = 1 << 2,
    ADPT_PORT_COM_FLAG_MAX
};
typedef enum adpt_port_com_flag_e adpt_port_com_flag_t;

/**
 @brief port common information
*/
struct adpt_port_common_info_s
{    
    uint32_ofp flag;                    /**< adpt_port_com_flag_t */
    char  name[OFP_IFNAME_SIZE];        /**< netdev name */
    uint16_ofp ofport;                  /**< OVS port number */
    uint16_ofp gport;                   /**< Centec global port id */
    uint32_ofp ifindex;                 /**< Interface index */
};
typedef struct adpt_port_common_info_s adpt_port_common_info_t;

/**
 @brief port phy information
*/
struct adpt_port_phy_info_s
{
    glb_port_duplex_t  duplex;
    glb_port_speed_t   speed;
    glb_port_link_t    link;
    uint32_ofp         media;
    glb_port_cfg_t     port_cfg;
    uint8_ofp          mac[OFP_ETH_ADDR_LEN];
    adpt_bridge_port_t bridge_port;
};
typedef struct adpt_port_phy_info_s adpt_port_phy_info_t;

/**
 @brief adapter layer port information data structure
*/
struct adpt_port_l2_info_s
{
    uint32_ofp flag;
};
typedef struct adpt_port_l2_info_s adpt_port_l2_info_t;

/**
 @brief adapter layer port information data structure
*/
struct adpt_port_l3_info_s
{
    uint32_ofp flag;
    uint16_ofp l3ifid;
    uint32_ofp ipaddress;   /**< interface ip address, for internal management */
    uint32_ofp mask;          /**< interface ip mask, for internal management */
};
typedef struct adpt_port_l3_info_s adpt_port_l3_info_t;

/**
 @brief adapter layer port information data structure
*/
struct adpt_port_info_s
{
    ctclib_list_node_t node;                /**< node for ADPT_PORT_LIST */
    
    uint32_ofp type;                        /**< ofp_interface_type_t */

    adpt_port_common_info_t com_info;       /**< common information */

    void* pv_data[ADPT_PORT_DATA_TYPE_MAX]; /**< port data array */
};
typedef struct adpt_port_info_s adpt_port_info_t;

/**
 @brief adapter layer port master data structure
*/
struct adpt_port_master_s
{
    ctclib_list_t port_list[OFP_INTERFACE_TYPE_MAX];    /**< adpt_port_info_t */
    
    ctc_hash_t*  p_name_hash;       /**< adpt_port_info_t */
    ctc_hash_t*  p_ofport_hash;     /**< adpt_port_info_t */
    ctc_hash_t*  p_gport_hash;      /**< adpt_port_info_t */

    uint32_ofp port_num[OFP_INTERFACE_TYPE_MAX];
};
typedef struct adpt_port_master_s adpt_port_master_t;

/**
 * @brief adapter layer tunnel port modified data structure
 **/
struct adpt_tunnel_port_modified_s
{
    int32_ofp tunnel_type;          /**< tunnel type */
    char* bind_ifname;              /**< bind ifname */
    bool is_modified;               /**< is modified */
};
typedef struct adpt_tunnel_port_modified_s adpt_tunnel_port_modified_t;

/**
 @brief adapter layer queue master data structure
*/
struct adpt_queue_master_s
{
        bool queue_igp_enable;      /**< queue ipg enable */
};
typedef struct adpt_queue_master_s adpt_queue_master_t;

#define ADPT_PORT_LIST          &g_p_adpt_port_master->port_list
#define ADPT_PORT_NAME_HASH     g_p_adpt_port_master->p_name_hash
#define ADPT_PORT_OFPORT_HASH   g_p_adpt_port_master->p_ofport_hash
#define ADPT_PORT_GPORT_HASH    g_p_adpt_port_master->p_gport_hash
#define ADPT_PORT_TYPE_PORT_NUM g_p_adpt_port_master->port_num

#define ADPT_PORT_HASH_BLOCK_SIZE 64

extern adpt_port_master_t* g_p_adpt_port_master;

#define ADPT_PORT_INIT_CHECK()                                      \
do {                                                                \
    ADPT_MODULE_INIT_CHECK(g_p_adpt_port_master);                   \
    ADPT_MODULE_INIT_CHECK(ADPT_PORT_NAME_HASH);                    \
    ADPT_MODULE_INIT_CHECK(ADPT_PORT_OFPORT_HASH);                  \
    ADPT_MODULE_INIT_CHECK(ADPT_PORT_GPORT_HASH);                   \
}while(0)

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Get port info pointer by ofport
 * @param[in] ofport                    ofport
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_ofport(uint16_ofp ofport, adpt_port_info_t** pp_port_info);

/**
 * Get port info pointer by name
 * @param[in] ifname                    interface name
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_name(const char* ifname, adpt_port_info_t** pp_port_info);

/**
 * Get port info pointer by gport
 * @param[in] gport                     gport
 * @param[out] pp_port_info             Pointer to pointer of adpt_port_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_info_by_gport(uint16_ofp gport, adpt_port_info_t** p_port_info);

/**
 * Set port data by type
 * @param gport                         gport
 * @param data_type                     adpt_port_data_type_t
 * @param p_data                        pointer to port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_set_port_data(uint16_ofp gport, adpt_port_data_type_t data_type, void* p_data);

/**
 * Set port data by type
 * @param[in]  gport                    gport
 * @param[in]  data_type                adpt_port_data_type_t
 * @param[out] pp_data                  pointer to port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_data(uint16_ofp gport, adpt_port_data_type_t data_type, void** pp_data);

/**
 * Add port to db
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_add_port(adpt_port_info_t* p_port_info);

/**
 * Remove port from db
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_del_port(adpt_port_info_t* p_port_info);

/**
 * Get port number of each type
 * @param p_port_info                   port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_get_port_num(ofp_interface_type_t type, uint32_ofp* p_port_num);


/**
 * Set tunnel port modified status by bind port
 * @param tunnel_port_modified     Tunnel port modified information
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_set_tunnel_port_modified_by_bind_port(adpt_tunnel_port_modified_t *tunnel_port_modified);

/**
 * Adapter layer port db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_init(void);

int32_ofp
adpt_portdb_set_port_flag(uint16_ofp ofport, bool set, adpt_port_com_flag_t flag);

int32_ofp
adpt_portdb_check_port_flag(uint16_ofp ofport, adpt_port_com_flag_t flag);

#endif 
