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
 * @brief This file is  the header file of hal_port.c
 */

#ifndef __HAL_PORT_H__
#define __HAL_PORT_H__ 

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ctc_common.h"
#include "ctc_cpu_traffic.h"

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
* Set port config
* @param gport                          gport
* @param port_config                    port config
* @return OFP_ERR_XX
*/
int32
hal_port_set_config(uint16_ofp gport, uint32_ofp port_config);

/**
* Reset port statistics
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_reset_mac_stats(uint16_ofp gport);

/**
* Get port statistics
* @param[in]  gport                     gport
* @param[out] p_stats                   Pointer to stats data structure
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_mac_stats(uint16_ofp gport, ofp_if_stats_t* p_stats);

/**
* Enable port qos
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_enable_qos(uint16_ofp gport);

/**
* Disable port qos
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_disable_qos(uint16_ofp gport);

/**
* Set port user id enable
* @param gport                          gport
* @param enable                         true/false
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_user_id_enable(uint16_ofp gport, bool enable);

/**
* Init port
* @param gport                          gport
* @param l3ifid                         l3ifid
* @param ethaddr                        port ethernet address
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_enable_openflow(uint16_ofp gport, uint16_ofp l3ifid, uint8_ofp ethaddr[6]);

int32_ofp
hal_port_disable_openflow(uint16_ofp gport, uint16_ofp l3ifid);

/**
* enable hybrid on interface
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_enable_hybrid(uint16_ofp gport);

/**
* disable hybrid on interface
* @param gport                          gport
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_disable_hybrid(uint16_ofp gport);

/**
* Set l3if route en
* @param l3ifid                         l3ifid
* @param enable                         true/false
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_l3if_route_en(uint16_ofp l3ifid, bool enable);

/**
* Set l3if vmac prefix
* @param mac_40bit                      route mac high 40 bits
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_l3if_vmac(uint8_ofp mac_40bit[OFP_ETH_ADDR_LEN]);

/**
* Set port phy enable
* @param gport                          Global port
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_set_phy_enable(uint16_ofp gport, bool enable);

/**
* Get port phy speed
* @param gport      Global port
* @param speed     Port speed
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_phy_speed(uint16_ofp gport, uint32_ofp *speed);

/**
* Get port auto negotiate speed
* @param gport      Global port
* @param speed     Port speed
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_autonego_speed(uint16_ofp gport, uint32_ofp *speed);

/**
* Get port phy duplex
* @param gport      Global port
* @param duplex    Port duplex
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_phy_duplex(uint16_ofp gport, uint32_ofp *duplex);

/**
* Get system mac
* @param[out]                           system mac
* @return OFP_ERR_XX
*/
int32_ofp
hal_port_get_system_mac(uint8_ofp system_mac[OFP_ETH_ADDR_LEN]);

#endif
