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
 * @brief This file calls the sdk vlan APIs.
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_macro.h"
#include "ofp_api.h"
#include "hal.h"
#include "hal_vlan.h"

#include "ctc_api.h"

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
 * init vlan by vid
 * @param[in]  vid                  Vlan id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_vlan_init_vlan(uint16_ofp vid)
{
    /* Create vlan for SDK */
    HAL_ERROR_RETURN(ctc_vlan_create_vlan(vid));
    HAL_ERROR_RETURN(ctc_vlan_set_transmit_en(vid, true));
    HAL_ERROR_RETURN(ctc_vlan_set_receive_en(vid, true));
    HAL_ERROR_RETURN(ctc_vlan_set_fid(vid, vid));

    /* Make default action of arp/dhcp packet to: Normal */
    HAL_ERROR_RETURN(ctc_vlan_set_arp_excp_type(vid, CTC_EXCP_NORMAL_FWD));
    HAL_ERROR_RETURN(ctc_vlan_set_dhcp_excp_type(vid, CTC_EXCP_NORMAL_FWD));

    return OFP_ERR_SUCCESS;
}

/**
 * Add vlan group
 * @param[in]  vid                  Vlan id
 * @param[in]  group_id             Group id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_vlan_add_vlan_group(uint16_ofp vid, uint32_ofp group_id)
{
    ctc_l2dflt_addr_t l2dflt_addr;
    
    sal_memset(&l2dflt_addr, 0, sizeof(ctc_l2dflt_addr_t));
    l2dflt_addr.fid = vid;
    l2dflt_addr.l2mc_grp_id = group_id;
    HAL_ERROR_RETURN(ctc_l2_add_default_entry(&l2dflt_addr));

    return OFP_ERR_SUCCESS;
}
