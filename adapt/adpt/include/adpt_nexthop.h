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
 * @brief This file is the public header file for adpt_nexthop.c
 */

#ifndef __ADPT_NEXTHOP_H__
#define __ADPT_NEXTHOP_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofproto/ofproto-provider.h"
#include "ofproto-ctc.h"
#include "adpt_flow.h"
#include "hal_nexthop.h"

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

/**
 @brief port nexthop information
*/
struct adpt_port_nexthop_info_s
{
    ofp_nh_offset_t non_edit_nh;
};
typedef struct adpt_port_nexthop_info_s adpt_port_nexthop_info_t;

/**
 @brief reserved nhid
*/
enum adpt_nh_reserved_nhid_e
{
    ADPT_NH_RSV_NHID_FOR_NONE   = 0,    /**< Global reserved nexthop Id for none */
    ADPT_NH_RSV_NHID_FOR_DROP   = 1,    /**< Global reserved nexthop Id for drop */
    ADPT_NH_RSV_NHID_FOR_TOCPU  = 2,    /**< Global reserved nexthop Id for cpu */
    ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY = 3, /**< Global reserved nexthop Id for default entry */
    ADPT_NH_RSV_NHID_FOR_MNGT_DFT_ENTRY = 4, /**< Global reserved nexthop Id for default entry */
    ADPT_NH_RSV_NHID_MAX        = 5
};
typedef enum adpt_nh_reserved_nhid_e adpt_nh_reserved_nhid_t;

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
 * Allocate the next-hop info opf
 * @param[in]  type                     ofp_nh_info_type_t
 * @param[out] offset                   pointer to ofp_nh_info_type_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_nh_info_opf(ofp_nh_info_type_t type, uint32_ofp* p_offset);

/**
 * Release the next-hop info opf
 * @param[in]  type                     ofp_nh_info_type_t
 * @param[in]  offset                   offset
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info_opf(ofp_nh_info_type_t type, uint32_ofp offset);

/**
 * Is reserved nhid
 * @param[in] nhid                      nhid
 * @return true/false
 */
bool
adpt_nexthop_is_reserved_nhid(uint32_ofp nhid);

/**
 * release the next-hop info opf for the specified type
 * @param[in] nh_offset                 ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info(ofp_nh_offset_t *nh_offset);

/**
 * release all the next-hop info opf for the specified type
 * @param[in] p_nh_info                 ofp_nexthop_info_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info_res(ofp_nexthop_info_t *p_nh_info);

/**
 * allocate the multicast group id
 * @param[in] p_action                  adpt_flow_action_combo_t
 * @param[out] p_nh_param               ctc_flex_nh_param_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_map_flex_nh_param(adpt_flow_action_combo_t* p_action, ctc_flex_nh_param_t *p_nh_param);

/**
 * Allocate to cpu flex next hop with flow_id
 * @param[in]  flow_id                  flow id
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_to_cpu_flex_nh(uint32_ofp flow_id, ofp_nh_offset_t* p_nh_offset);

/**
 * allocate the multicast flex next-hop
 * @param[in] p_action_combo            adpt_flow_action_combo_t
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_flex_nh(adpt_flow_action_combo_t* p_action_combo, ofp_nh_offset_t* p_nh_offset);

/**
 * allocate the multicast group id
 * @param[out] p_group_nh               pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_mcast_group(ofp_nh_offset_t* p_group_nh);

/**
 * release multicast group
 * @param[in] p_group_nh                pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_mcast_group(ofp_nh_offset_t* p_group_nh);

/**
 * allocate nexthop offset by type
 * @param[in]  type                     ofp_nh_type_t
 * @param[out] p_nh                     pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_nexthop_offset(ofp_nh_type_t type, ofp_nh_offset_t* p_nh);

/**
 * release the multicast group id
 * @param[in] p_nh                      pointer to ofp_nh_offset_t
 * @param[in] type                      ofp_nh_type_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_id(ofp_nh_offset_t *p_nh, ofp_nh_type_t type);

/**
 * Get nh offset for output:all
 * @return OFP_ERR_XX
 */
uint16_ofp
adpt_nexthop_get_output_all_group_offset(void);

/**
 * Create nexthop info when port created
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_phy_port_create(uint32_ofp gport);

/**
 * Destroy nexthop info when port deleted
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_phy_port_destory(uint32_ofp gport);

/**
 * Show nexthop db
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_show_db(void);

/**
 * allocate ipuc next-hop to support decrease ip ttl
 * @param[in] p_action_combo            adpt_flow_action_combo_t
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_ipuc_nh(adpt_flow_action_combo_t* p_action_combo, ofp_nh_offset_t* p_nh_offset);

/**
 * Adapter layer nexthop module init
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_init(void);

#endif
