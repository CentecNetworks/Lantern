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
 * @brief This file is the header file of hal_nexthop.c
 */

#ifndef __HAL_NEXTHOP_H__
#define __HAL_NEXTHOP_H__ 

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ctc_const.h"
#include "ctc_nexthop.h"

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
* Create mcast group
* @param nhid                       nexthop id
* @param offset                     nexthop offset
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_mcast_group(uint32_ofp nhid, uint32_ofp offset);

/**
* Remove mcast group
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_mcast_group(uint32_ofp nhid);

/**
* Add member to mcast group
* @param groupid                    mcast group id
* @param mem_nhid                   member nhid
* @param port_check_discard         Whether discard packet when destination port and ingress port are same
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_add_mcast_member(uint32_ofp groupid, uint32_ofp mem_nhid, bool port_check_discard);

/**
* Remove member from mcast group
* @param groupid                    mcast group id
* @param mem_nhid                   member nhid
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_del_mcast_member(uint32_ofp groupid, uint32_ofp mem_nhid);

/**
* Add nexthop stats
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_add_stats(uint32_ofp nhid);

/**
* Delete nexthop stats
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_del_stats(uint32_ofp nhid);

/**
* Get nexthop statistics
* @param nhid                       nexthop id
* @param p_stats                    Pointer to statistics data structure
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_get_nh_stats(uint32_ofp nhid, ofp_stats_t* p_stats);

/**
* Clear nexthop statistics
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_clear_nh_stats(uint32_ofp nhid);

/**
* Create flex nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_flex_nh_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_flex_nh(uint32_ofp nhid, ctc_flex_nh_param_t* p_nh_param);

/**
* Remove flex nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_flex_nh(uint32_ofp nhid);

/**
* Create mpls nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_mpls_nexthop_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_mpls_nh(uint32_ofp nhid, ctc_mpls_nexthop_param_t* p_mpls_nh);

/**
* Remove mpls nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_mpls_nh(uint32_ofp nhid);

/**
* Create ipuc nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_ip_nh_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_ipuc_nh(uint32_ofp nhid, ctc_ip_nh_param_t* p_nh_param);

/**
* Remove ipuc nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_ipuc_nh(uint32_ofp nhid);

#endif /* !__HAL_NEXTHOP_H__ */
