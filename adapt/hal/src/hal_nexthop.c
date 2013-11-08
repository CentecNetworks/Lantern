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
 * @brief This file calls the sdk nexthop APIs
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_const.h"
#include "ofp_stats.h"
#include "ofp_macro.h"

#include "hal.h"
#include "hal_nexthop.h"

#include "ctc_api.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_stats.h"

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
hal_nexthop_create_mcast_group(uint32_ofp nhid, uint32_ofp offset)
{
    ctc_mcast_nh_param_group_t nh_param;
    
    memset(&nh_param, 0x0, sizeof(nh_param));
    nh_param.mc_grp_id = offset;
    
    HAL_ERROR_RETURN(ctc_mcast_nh_create(nhid, &nh_param));
    
    return OFP_ERR_SUCCESS;
}

/**
* Remove mcast group
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_mcast_group(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_mcast_nh_delete(nhid));
    
    return OFP_ERR_SUCCESS;
}

/**
* Add member to mcast group
* @param groupid                    mcast group id
* @param mem_nhid                   member nhid
* @param port_check_discard         Whether discard packet when destination port and ingress port are same
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_add_mcast_member(uint32_ofp groupid, uint32_ofp mem_nhid, bool port_check_discard)
{
    ctc_mcast_nh_param_group_t nh_param;

    memset(&nh_param, 0, sizeof(nh_param));

    nh_param.mem_info.ref_nhid    = mem_nhid;
    nh_param.mem_info.member_type = CTC_NH_PARAM_MEM_LOCAL_WITH_NH;
    nh_param.mem_info.port_check_discard = port_check_discard;
    nh_param.opcode = CTC_NH_PARAM_MCAST_ADD_MEMBER;

    HAL_ERROR_RETURN(ctc_mcast_nh_update(groupid, &nh_param));

    return OFP_ERR_SUCCESS;
}

/**
* Remove member from mcast group
* @param groupid                    mcast group id
* @param mem_nhid                   member nhid
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_del_mcast_member(uint32_ofp groupid, uint32_ofp mem_nhid)
{
    ctc_mcast_nh_param_group_t nh_param;

    memset(&nh_param, 0, sizeof(nh_param));

    nh_param.mem_info.ref_nhid    = mem_nhid;
    nh_param.mem_info.member_type = CTC_NH_PARAM_MEM_LOCAL_WITH_NH;
    nh_param.opcode = CTC_NH_PARAM_MCAST_DEL_MEMBER;

    HAL_ERROR_RETURN(ctc_mcast_nh_update(groupid, &nh_param));

    return OFP_ERR_SUCCESS;
}

/**
* Add nexthop stats
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_add_stats(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_nh_add_stats(nhid));

    return OFP_ERR_SUCCESS;    
}

/**
* Delete nexthop stats
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_del_stats(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_nh_del_stats(nhid));

    return OFP_ERR_SUCCESS;    
}

/**
* Get nexthop statistics
* @param nhid                       nexthop id
* @param p_stats                    Pointer to statistics data structure
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_get_nh_stats(uint32_ofp nhid, ofp_stats_t* p_stats)
{
    ctc_stats_basic_t result;
    sys_nh_u16_array_t stats_ptr;
    
    HAL_PTR_CHECK(p_stats);
    memset(&result, 0, sizeof(result));
    memset(stats_ptr, 0, sizeof(stats_ptr));
    
    HAL_ERROR_RETURN(sys_humber_nh_get_statsptr(nhid, stats_ptr));
    HAL_ERROR_RETURN(sys_humber_stats_get_flow_stats(0, stats_ptr[0], &result));
    
    p_stats->packet_count = result.packet_count;
    p_stats->byte_count   = result.byte_count;
    
    return OFP_ERR_SUCCESS;
}

/**
* Clear nexthop statistics
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_clear_nh_stats(uint32_ofp nhid)
{
    sys_nh_u16_array_t stats_ptr;

    memset(stats_ptr, 0, sizeof(stats_ptr));

    HAL_ERROR_RETURN(sys_humber_nh_get_statsptr(nhid, stats_ptr));
    HAL_ERROR_RETURN(sys_humber_stats_reset_flow_stats(0, stats_ptr[0]));
    
    return OFP_ERR_SUCCESS;
}

/**
* Create flex nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_flex_nh_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_flex_nh(uint32_ofp nhid, ctc_flex_nh_param_t* p_nh_param)
{
    HAL_ERROR_RETURN(ctc_flex_nh_create(nhid, p_nh_param));

    return OFP_ERR_SUCCESS;
}

/**
* Remove flex nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_flex_nh(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_flex_nh_remove(nhid));

    return OFP_ERR_SUCCESS;
}

/**
* Create mpls nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_mpls_nexthop_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_mpls_nh(uint32_ofp nhid, ctc_mpls_nexthop_param_t* p_mpls_nh)
{
    HAL_ERROR_RETURN(ctc_mpls_nh_create(nhid, p_mpls_nh));

    return OFP_ERR_SUCCESS;
}

/**
* Remove mpls nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_mpls_nh(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_mpls_nh_remove(nhid));

    return OFP_ERR_SUCCESS;
}

/**
* Create ipuc nexthop
* @param nhid                       nexthop id
* @param p_nh_param                 pointer to ctc_ip_nh_param_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_create_ipuc_nh(uint32_ofp nhid, ctc_ip_nh_param_t* p_nh_param)
{
    HAL_ERROR_RETURN(ctc_ipuc_nh_create(nhid, p_nh_param));  

    return OFP_ERR_SUCCESS;
}

/**
* Remove ipuc nexthop
* @param nhid                       nexthop id
* @return OFP_ERR_XX
*/
int32_ofp
hal_nexthop_remove_ipuc_nh(uint32_ofp nhid)
{
    HAL_ERROR_RETURN(ctc_ipuc_nh_remove(nhid));  

    return OFP_ERR_SUCCESS;
}
