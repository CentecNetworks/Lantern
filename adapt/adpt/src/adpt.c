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
 * @brief This file is the main implementation of adapt layer 
 */

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ofp_api.h"

#include "adpt.h"
#include "adpt_opf.h"
#include "adpt_port.h"
#include "adpt_flow.h"
#include "adpt_message.h"
#include "adpt_nexthop.h"
#include "adpt_parser.h"
#include "glb_stm_define.h"
#include "hal.h"
#include "hal_vlan.h"
#include "hal_parser.h"
#include "hal_pdu.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt);

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
uint8_ofp g_current_profile;
/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
 static void
 adpt_get_current_profile(void)
 {
     struct stat buf;
     g_current_profile = GLB_STM_DEFAULT;

     return;
 }

/**
 * Initialize vlan for openflow
 */
int32_ofp
adpt_vlan_init(void)
{
    uint16_ofp vid;
    uint32_ofp group_id; 
    
    for (vid = 1; vid <= GLB_VLAN_MAX; vid++)
    {
        ADPT_ERROR_RETURN(hal_vlan_init_vlan(vid));
        
        if (GLB_STM_HYBRID == g_current_profile)
        {
            ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_MET_OFFSET, &group_id));
            ADPT_ERROR_RETURN(hal_vlan_add_vlan_group(vid, group_id));
        }
    }

    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_misc_init(void)
{
    ADPT_ERROR_RETURN(hal_misc_init());
    ADPT_ERROR_RETURN(hal_parser_init());
    ADPT_ERROR_RETURN(hal_pdu_init());
    
    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_module_init(void)
{
    adpt_get_current_profile();
    ADPT_ERROR_RETURN(adpt_message_init());
    ADPT_ERROR_RETURN(adpt_opf_init());
    ADPT_ERROR_RETURN(adpt_port_init());
    ADPT_ERROR_RETURN(adpt_vlan_init());
    ADPT_ERROR_RETURN(adpt_tunnel_init());
    ADPT_ERROR_RETURN(adpt_flow_init());
    ADPT_ERROR_RETURN(adpt_nexthop_init());
    ADPT_ERROR_RETURN(adpt_misc_init());
    ADPT_ERROR_RETURN(adpt_parser_init());
    
    return OFP_ERR_SUCCESS;
}
