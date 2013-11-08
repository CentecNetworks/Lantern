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
 * @brief This file is the header file of hal_gre_tunnel.c
 */

#ifndef __HAL_GRE_TUNNEL_H__
#define __HAL_GRE_TUNNEL_H__

/******************************************************************************
* Header Files 
******************************************************************************/

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
 * Create decap nhid
 * @param[in]  iloop_port               Internal port for iloop
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_create_decap_nhid(uint16_ofp iloop_port, uint32_ofp nh_wo_key, uint32_ofp nh_w_key);

/**
 * Remove decap nhid
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_remove_decap_nhid(uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key);

/**
 * Add route entry for gre decapsulation
 * @param[in]  local_ip                 Local  ip(ipda) in the route entry
 * @param[in]  remote_ip                Remote ip(ipsa) in the route entry
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_decap_route(uint32_ofp local_ip, uint32_ofp remote_ip, uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key);

/**
 * Remove route entry for gre decapsulation
 * @param[in]  local_ip                 Local  ip(ipda) in the route entry
 * @param[in]  remote_ip                Remote ip(ipsa) in the route entry
 * @param[in]  nh_wo_key                Nhid for decapsulate gre packet without gre key
 * @param[in]  nh_w_key                 Nhid for decapsulate gre packet with    gre key
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_del_decap_route(uint32_ofp local_ip, uint32_ofp remote_ip, uint32_ofp nhid_wo_key, uint32_ofp nhid_w_key);

/**
 * Alloc internal port
 * @param[out]  iloop_port              Pointer to internal port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_alloc_internal_port(uint16_ofp* p_internal_port);

/**
 * Release internal port
 * @param[in]  iloop_port               Internal port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_release_internal_port(uint16_ofp internal_port);

/**
 * Add default route entry to send packets to cpu via miss-match reason
 * @param[out]  p_nhid                  Pointer to nexthop id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_default_route_entry(uint32_ofp* p_nhid);

/**
 * Add default route entry to Matching GRE packets for routing processing to decapsulation
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_add_default_qos_entry(void);

/**
 * Enable iloop internal port for packet process after iloop
 * @param[in]  iloop_port               Internal port
 * @param[in]  enable                   true/false
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_enable_iloop_port(uint16_ofp iloop_port, bool enable);

/**
 * Create service id for acl process after decapsulation
 * @param[in]  service_id               Service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_create_service_id(uint32_ofp service_id);

/**
 * Remove service id
 * @param[in]  service_id               Service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_tunnel_remove_service_id(uint32_ofp service_id);

#endif /* !__HAL_GRE_TUNNEL_H__ */
