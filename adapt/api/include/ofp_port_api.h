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
 * @brief This file is the header file of ofp_port_api.c
 */

#ifndef _OFP_PORT_API_H_
#define _OFP_PORT_API_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/****************************************************************
*
* Functions
*
****************************************************************/

/**
 * Set port config
 * @param ofp_port              ofp port
 * @param port_config           port config
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_set_config(uint16_ofp ofport, uint32_ofp port_config);

/**
 * Add port to openflow instance
 * @param p_port_info           Pointer of port information
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_add(ofp_port_info_t* p_port_info);

/**
 * Delete port from openflow instance
 * @param ofport                ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_del(uint16_ofp ofp_port);

/**
 * Get route mac
 * @param[in] mac                       route mac
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_get_route_mac(uint8_ofp mac[OFP_ETH_ADDR_LEN]);

/**
 * Get ofport by ifname
 * @param[in]  ifname                   port name
 * @param[out] p_ofport                 pointer of ofport
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_port_get_ofport_by_name(const char* ifname, uint16_ofp* p_ofport);

#endif /* !_OFP_PORT_API_H_ */
