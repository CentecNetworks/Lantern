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
 * @brief This file is the header file of ofp_netdev_api.c
 */

#ifndef _OFP_NETDEV_API_H_
#define _OFP_NETDEV_API_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if_arp.h>
#include "flow.h"
#include "ofpbuf.h"
#include "dpif.h"
#include "netlink.h"

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
int
ctc_cli_out_ofp(const char *fmt, ...);

/**
 * Get netdev statistics
 * @param ifname                interface name
 * @param if_stats              interface statistics
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_stats(const char * ifname, ofp_if_stats_t *if_stats);

/**
 * Reset netdev statistics
 * @param ifname                interface name
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_reset_port_stats(const char * ifname);

/**
 * Set interface feature: advertised
 * @param ifname                interface name
 * @param advertised            advertised feature
 * @return  OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_set_port_advertised(const char * ifname, uint32_ofp advertised);

/**
 * Get interface features
 * @param ifname interface name
 * @param current current
 * @param advertised advertised
 * @param supported supported
 * @param peer peer
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_features(const char * ifname, uint32_ofp *current, uint32_ofp *advertised, uint32_ofp *supported, uint32_ofp *peer);

/**
 * Get interface features
 * @param ifname interface name
 * @param supported supported
 * @return OFP_SUCCESS, OFP_FAIL
 */
int32_ofp
ofp_netdev_get_port_supported(const char * ifname, uint32_ofp *supported);

/**
 * Get interface link status
 * @param ifname interface name
 * @param carrier carrier
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_port_carrier(const char * ifname, uint8_ofp * carrier);

/**
 * Get ethernet address
 * @param netdev_name           netdev name
 * @param ea                    ethernet address
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_get_etheraddr(const char *netdev_name, uint8_t ea[OFP_ETH_ADDR_LEN]);

/**
 * Set netdev mtu
 * @param ifname                netdev name
 * @param mtu                   MTU
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_set_mtu(const char * ifname, uint32_ofp mtu);

/**
 * Enable interface
 * @param ifname                netdev interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_enable_interface(const char * ifname);

/**
 * Disable interface
 * @param ifname                netdev interface name
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_disable_interface(const char * ifname);

/**
 * is a valid eth name, eg. "eth-0-1"
 * @param eth_name              eth name
 * @return true or false
 */
bool
ofp_netdev_name_is_phy_port(const char* eth_name);

/**
 * Check whether the name is a valid gre tunnel name
 *  valid name should be greX ~ greXXX, and X is between 1 and OFP_TUNNEL_PORT_NO_NUM
 * @param netdev_name           netdev name
 * @return true or false
 */
bool
ofp_netdev_name_is_gre_port(const char* netdev_name);

/**
 * Check if a port number is a tunnel port
 * @param port_no               port number
 * @param check_exist           whether check the port is created
 * @return true, false
 */
bool
ofp_netdev_ofport_is_gre_port(uint16_ofp port_no, bool check_exist);

/**
 * Check if a port number is a physical port
 * @param port_no               port number
 * @param check_exist           whether check the port is created
 * @return true, false
 */
bool
ofp_netdev_ofport_is_phy_port(uint16_ofp ofport, bool check_exist);

/**
 * Get gre ofport number by name
 *  valid name should be greX ~ greXXX, and X is between 1 and OFP_TUNNEL_PORT_NO_NUM
 * @param[in]  netdev_name              netdev name
 * @param[out] p_ofport                 pointer of ofport
 * @return port number
 */
uint16_ofp 
ofp_netdev_assign_gre_ofport(const char * netdev_name, uint16_ofp* p_ofport);

/**
 * Get physical ofport number by name
 *  valid name should be eth-x-xx
 * @param[in]  netdev_name              netdev name
 * @param[out] p_ofport                 pointer of ofport
 * @return port number
 */
uint16_ofp 
ofp_netdev_assign_phy_ofport(const char * ifname, uint16_ofp* p_ofport);

/**
 * Set port status modified
 * @param[in]  ifname                interface name
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_set_port_modified(const char * ifname);

/**
 * Get if port is modified
 * @param[in]  ifname                interface name
 * @param[out] p_is_modified         pointer to true/false
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_get_port_modified(const char * ifname, bool* p_is_modified);

/**
 * Clear the port modified status
 * @param[in]  ifname                interface name
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_clear_port_modified(const char * ifname);

/**
 * Get if any port is modified
 * @param[out] ifname                interface name that modified
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_netdev_get_any_port_modified(char** ifname);

int32_ofp
ofp_netdev_get_port_speed(const char * ifname, uint32_ofp *speed);

#endif /* _OFP_NETDEV_API_H_ */
