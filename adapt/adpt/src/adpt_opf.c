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
 * @brief This file is the main file for adapter opf
 */

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_opf.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_opf);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Allocate opf offset
 * @param[in]  type                     Type
 * @param[in]  num                      opf number
 * @param[out] p_offset                 Pointer to offset base
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_alloc_offset(uint8_ofp type, uint8_ofp num, uint32_ofp* p_offset)
{
    sys_humber_opf_t opf;
    kal_memset(&opf, 0, sizeof(opf));

    opf.pool_index = 0;
    opf.pool_type  = type;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, num, p_offset));

    return OFP_ERR_SUCCESS;
}

/**
 * Free opf offset
 * @param[in]  type                     Type
 * @param[in]  num                      opf number
 * @param[in]  offset                   offset base
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_free_offset(uint8_ofp type, uint8_ofp num, uint32_ofp offset)
{
    sys_humber_opf_t opf;
    kal_memset(&opf, 0, sizeof(opf));

    opf.pool_index = 0;
    opf.pool_type  = type;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_free_offset(&opf, num, offset));

    return OFP_ERR_SUCCESS;
}

/**
 * Init opf
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_init(void)
{
    sys_humber_opf_t opf;
    
    kal_memset(&opf, 0x0, sizeof(opf));    
    
    /* 1. init nexthop id opf*/
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_NH_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_NH_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, NEXTHOP_ID_START_OFFSET, NEXTHOP_ID_MAX_SIZE));
    
    /* 2. init global met offset opf*/
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_GLB_MET;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_GLB_MET, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, GLB_MET_START_OFFSET, GLB_MET_MAX_SIZE));
    
    /* 3. init global nexthop offset opf*/
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_GLB_NH;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_GLB_NH, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, NEXTHOP_START_OFFSET, NEXTHOP_MAX_SIZE));
    
    /* 4. init opf for qos mac entry_id */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_QOS_MAC_ENTRY_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_QOS_MAC_ENTRY_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MAC_START_ENTRY_ID, ENTRY_ID_MAX_SIZE));
    
    /* 5. init opf for qos ipv4 entry_id */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_QOS_IPV4_ENTRY_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_QOS_IPV4_ENTRY_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, IPV4_START_ENTRY_ID, ENTRY_ID_MAX_SIZE));
    
    /* 6. init opf for qos mpls entry_id */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_QOS_MPLS_ENTRY_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_QOS_MPLS_ENTRY_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MPLS_START_ENTRY_ID, ENTRY_ID_MAX_SIZE));

    /* 7. init opf for flow id */
    /* Flow id opf's deletion (when flow timeout) is later than flow's deletion, so we need lager flow id */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_FLOW_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_FLOW_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, FLOW_ID_START_OFFSET, OFP_UINT16_MAX));

    /* 8. init opf for service id */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_TNL_SERVICE_ID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_TNL_SERVICE_ID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 
        OFP_TUNNEL_SERVICE_ID_MIN, OFP_TUNNEL_SERVICE_ID_NUM));

    /* 9. init opf for linkagg tid */
    opf.pool_index = 0;
    opf.pool_type  = OPF_OFP_LINKAGG_TID;
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init(OPF_OFP_LINKAGG_TID, 1));
    ADPT_OPF_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 
        OFP_LINKAGG_TID_MIN, OFP_LINKAGG_TID_NUM));

    return OFP_ERR_SUCCESS;
}
