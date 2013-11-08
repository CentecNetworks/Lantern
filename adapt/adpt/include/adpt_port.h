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
 * @brief This file is the public header file of adapt_port.c
 */

#ifndef __ADPT_PORT_H__
#define __ADPT_PORT_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#ifndef _OPEN_SOURCE_
#include "ctc_cli.h"
#endif

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
typedef int32_ofp (*ADPT_PORT_LOOP_CB_FUNC)(uint16_ofp ofport, void* pv_arg);

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
/**
 @brief port bridge flag
*/
enum adpt_bridge_port_mode_e
  {
    ADPT_BRIDGE_PORT_MODE_INVALID,
    ADPT_BRIDGE_PORT_MODE_ACCESS,
    ADPT_BRIDGE_PORT_MODE_TRUNK,
  };
typedef enum adpt_bridge_port_mode_e adpt_bridge_port_mode_t;

/**
 * @brief Adapter layer port openflow feature information
 */
struct adpt_port_of_fea_info_s
{
    uint32_ofp carrier;
    uint32_ofp current;
    uint32_ofp advertised;
    uint32_ofp supported;
    uint32_ofp peer;    
};
typedef struct adpt_port_of_fea_info_s adpt_port_of_fea_info_t;

/**
 * @brief Adapter layer port status information
 */
struct adpt_port_status_info_s
{
    bool modified;
};
typedef struct adpt_port_status_info_s adpt_port_status_info_t;

/**
 * @brief Adapter layer queue status information
 */
struct adpt_queue_status_info_s
{
    uint16_ofp min_rate[OFP_MAX_QUEUE_VALUE];  /* In 1/10 of a percent; >1000 -> disabled. [spec1.3]*/
    uint16_ofp max_rate[OFP_MAX_QUEUE_VALUE]; 
    
    /*other property*/
};
typedef struct adpt_queue_status_info_s adpt_queue_status_info_t;

/**
 @brief adapter layer port information data structure
*/
enum adpt_port_data_type_e
{
    ADPT_PORT_DATA_TYPE_PHY_INFO,   /**< adpt_port_phy_info_t */
    ADPT_PORT_DATA_TYPE_TNL_INFO,   /**< adpt_tunnel_info_t */
    ADPT_PORT_DATA_TYPE_NH_INFO,    /**< adpt_port_nexthop_info_t */
    ADPT_PORT_DATA_TYPE_L2_INFO,    /**< adpt_port_l2_info_t */
    ADPT_PORT_DATA_TYPE_L3_INFO,    /**< adpt_port_l3_info_t */
    ADPT_PORT_DATA_TYPE_OP_INFO,    /**< adpt_port_of_fea_info_t */
    ADPT_PORT_DATA_TYPE_GP_INFO,    /**< adpt_group_port_info_t */
    ADPT_PORT_DATA_TYPE_STATUS_INFO,   /**< adpt_port_status_t */
    ADPT_PORT_DATA_TYPE_QUEUE_INFO,   /**< adpt_queue_status_t */

    ADPT_PORT_DATA_TYPE_MAX
};
typedef enum adpt_port_data_type_e adpt_port_data_type_t;

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
adpt_port_get_gport_by_ofport(uint16_ofp ofport, uint16_ofp* p_gport);

/**
 * Get gport by port name
 * @param[in]  ifname                   port name
 * @param[out] p_gport                  pointer of gport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_gport_by_name(const char* ifname, uint16_ofp* p_gport);

/**
 * Get ofport by gport
 * @param[in]  gport                    gport
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_ofport_by_gport(uint16_ofp gport, uint16_ofp* p_ofport);

/**
 * Get ofport by ifname
 * @param[in]  ifname                   port name
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_ofport_by_name(const char* ifname, uint16_ofp* p_ofport);

/**
 * Get port type by ofport
 * @param[in]  ofport                   ofport
 * @param[out] p_type                   port type
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_type_by_ofport(uint16_ofp ofport, ofp_interface_type_t* p_type);

/**
 * Get port type by ofport
 * @param[in]  ifname                   port name
 * @param[out] p_type                   port type
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_type_by_name(const char* ifname, ofp_interface_type_t* p_type);

/**
 * Get port name by gport
 * @param[in]  gport                    gport
 * @param[out] ifname                   port name
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_name_by_gport(uint16_ofp gport, char* ifname);

/**
 * Get port name by gport
 * @param[in]  ofport                   ofport
 * @param[out] ifname                   port name
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_name_by_ofport(uint16_ofp ofport, char* ifname);

/**
 * Add port to openflow instance
 * @param[in] p_port_info               Pointer of ofport_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_enable_openflow(uint16_ofp ofport);

/**
 * delete port from openflow instance
 * @param[in] ofport                    ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_disable_openflow(uint16_ofp ofport);

/**
 * Check ofport is physical port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_physical_port(uint16_ofp ofport);

/**
 * Check ofport is tunnel port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_tunnel_port(uint16_ofp ofport);

/**
 * Check ofport is hybrid port
 * @param[in] ofport                    ofport
 * @return true/false
 */
bool 
adpt_port_is_hybrid_port(uint16_ofp ofport);

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
adpt_port_create_com_port(ofp_interface_type_t type, char* ifname, uint16_ofp ofport, uint16_ofp gport, uint32_ofp ifindex);

/**
 * Destroy port
 * @param[in] ofport                    ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_destroy_com_port(uint16_ofp ofport);

/**
 * Get openflow feature by name
 * @param[in]  ifname                   Interface name
 * @param[out] p_fea_info               Pointer of adpt_port_of_fea_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_of_fea_info(const char* ifname, adpt_port_of_fea_info_t* p_fea_info);

/**
 * Get openflow feature by name
 * @param[in]  ifname                   Interface name
 * @param[out] p_fea_info               Pointer of adpt_port_of_fea_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_of_fea_advertised(const char* ifname, uint32_ofp advertised);

/**
 * Get port status by name
 * @param[in]  ifname                   Interface name
 * @param[out] p_status_info            Pointer of adpt_port_status_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_status(const char* ifname, adpt_port_status_info_t* p_status_info);

/**
 * Set port modified status by name
 * @param[in]  ifname                   Interface name
 * @param[in]  is_modified              Port is modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_port_modified(const char* ifname, bool is_modified);

/**
 * Set tunnel port modified status by bind port
 * @param bind_ifname                   Bind port interface name
 * @param is_modified                   Port is modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_tunnel_port_modified_by_bind_port(const char *bind_ifname, bool is_modified);

/**
 * Get any port is modified
 * @param[out]  ifname                  Interface name that modified
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_any_port_modified(char** ifname);

/**
 * Get physical port number
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_phy_port_num(uint32_ofp* p_port_num);

/**
 * Get tunnel port number
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_tunnel_port_count(uint32_ofp* p_port_num);

/**
 * Get tunnel port max
 * @param[out] p_port_num               port number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_tunnel_port_max(uint32_ofp* p_port_max);

/**
 * Get l3if id
 * @param[in]  gport                    global port
 * @param[out] p_l3ifid                 pointer of l3ifid
 * @return OFP_ERR_XXX
 */
uint16_ofp
adpt_port_get_l3ifid(uint16_ofp gport, uint16_ofp* p_l3ifid);

/**
 * Set port data by type
 * @param[in]  ofport                   ofport
 * @param[in]  data_type                port data type
 * @param[in]  p_data                   pointer of port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_set_port_data(uint16_ofp ofport, adpt_port_data_type_t data_type, void* p_data);

/**
 * Get port data by type
 * @param[in]  ofport                   ofport
 * @param[in]  data_type                port data type
 * @param[in]  p_data                   Pointer to get the pointer of port data
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_port_data(uint16_ofp ofport, adpt_port_data_type_t data_type, void** pp_data);

/**
 * Clear the port change status
 * @param[in]  type                     Interface type
 * @param[in]  func                     Callback function
 * @param[in]  argv                     argv of Callback function
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_loop_port(ofp_interface_type_t type, ADPT_PORT_LOOP_CB_FUNC func, void* argv);

/**
 * Get non edit nhid by gport
 * @param[in]  gport                    Gport
 * @param[out] p_nhid                   pointer to nhid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_non_edit_nh_offset(uint16_ofp gport, uint32_ofp* p_nh_offset);

/**
 * Get route mac
 * @param[in] mac                       route mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_get_route_mac(uint8_ofp mac[OFP_ETH_ADDR_LEN]);

/**
 * Adapter port init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_port_init(void);

bool 
adpt_port_is_openflow_port(uint16_ofp ofport);

/**
 * Loop all port, show port info
 * @return OFP_ERR_XXX
 */
void
adpt_portdb_traversal_show_port_info(void);

/**
 * Loop all port, show phy info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_phy_info(void);

/**
 * Loop all port, show openflow feature info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_op_info(void);

/**
 * Loop all port, show status info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_status_info(void);

/**
 * Loop all port, show nexthop info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_portdb_traversal_show_nh_info(void);

int32_ofp
adpt_port_shutdown(uint16_ofp ofport);

int32_ofp
adpt_port_no_shutdown(uint16_ofp ofport);

#endif
