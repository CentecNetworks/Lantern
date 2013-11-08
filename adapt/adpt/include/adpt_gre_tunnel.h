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
 * @brief This file is the public header file of adpt_gre_tunnel.c
 */

#ifndef __ADPT_GRE_TUNNEL_H__
#define __ADPT_GRE_TUNNEL_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofproto/ofproto-provider.h"
#include "ofp_stats.h"
#include "ofp_port.h"
#include "adpt_flow.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
typedef int32_ofp (*ADPT_TUNNEL_SERVICE_ID_CB_FUNC)(uint32_ofp service_id, void* pv_arg);

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
 * Create tunnel port
 * @param p_port_info                   tunnel port info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_create_port(ofp_port_info_t* p_port_info);

/**
 * Delete tunnel port
 * @param ofport                        tunnel port
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_delete_port(uint16_ofp ofport);

/**
 * Alloc nhid for encapsulation
 * @param[in] ofport                    tunnel port
 * @param[in] p_combo                   tunnel id, strip svlan, strip cvlan in adpt_flow_action_combo_t
 * @param[in] p_nh_offset               pointer to ofp_nh_offset_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_alloc_encap_nhid(uint32_ofp ofport, adpt_flow_action_combo_t* p_combo, ofp_nh_offset_t* p_nh_offset);

/**
 * Lookup service id which was bind with tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_lookup_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp* p_service_id);

/**
 * Bind service id  with tunnel port and tunnel id
 * @param[in]  tunnel_port              tunnel port
 * @param[in]  tunnel_id                tunnel id
 * @param[out] p_service_id             pointer to service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_bind_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp* p_service_id);

/**
 * Unbind service id  with tunnel port and tunnel id
 * @param[in] tunnel_port               tunnel port
 * @param[in] tunnel_id                 tunnel id
 * @param[in] service_id                service id
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_unbind_service_id(uint32_ofp tunnel_port, uint32_ofp tunnel_id, uint32_ofp service_id);

/**
 * Check tunnel id when adding flow
 * @param[in] p_rule                    tunnel port
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_check_tunnel_id(const struct rule_ctc *p_rule);

/**
 * Loop all service id which was bind with tunnel port and tunnel id
 * @param[in] func                      callback function
 * @param[in] pv_arg                    argument
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_loop_tunnel_port_service_id(ADPT_TUNNEL_SERVICE_ID_CB_FUNC func, void* pv_arg);

/**
 * Get GRE packets miss-match (packet's ip do not match any tunnel port's ip) statistics
 * @param p_ofp_stats                   openflow statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_get_gre_packets_miss_match_stats(ofp_stats_t* p_ofp_stats);

/**
 * Clear GRE packets miss-match (packet's ip do not match any tunnel port's ip) statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_clear_gre_packets_miss_match_stats(void);

/**
 * Adapter layer tunnel init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunnel_init(void);

/**
 * Show tunnel port db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_tunnel_port(void);

/**
 * Show service id db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_service_id(void);

/**
 * Show tunnel info db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_tunnel_info(void);

/**
 * Show local ip db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_local_ip(void);

/**
 * Show local ip and remote ip db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_local_remote_ip(void);

/**
 * Show bind port db
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_tunneldb_show_bind_port(void);

#endif
