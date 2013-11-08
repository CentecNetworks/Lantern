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
 * @brief This file calls the sdk stats APIs.
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_stats.h"
#include "ofp_macro.h"

#include "hal.h"
#include "hal_stats.h"

#include "ctc_api.h"
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
 * Create stats ptr
 * @param[out]  p_stats_ptr             Pointer to stats ptr
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_create_stats_ptr(uint16_ofp* p_stats_ptr)
{
    HAL_ERROR_RETURN(sys_humber_stats_create_statsptr(0, 1, p_stats_ptr));

    return OFP_ERR_SUCCESS;
}

/**
 * Delete stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_delete_stats_ptr(uint16_ofp stats_ptr)
{
    HAL_ERROR_RETURN(sys_humber_stats_delete_statsptr(0, 1, stats_ptr));

    return OFP_ERR_SUCCESS;
}

/**
 * Get stats by stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @param[out] p_stats                  Pointer to statistics data structure
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_get_stats(uint16_ofp stats_ptr, ofp_stats_t * p_stats)
{
    ctc_stats_basic_t stats;
    
    memset(&stats, 0, sizeof(ctc_stats_basic_t));
    HAL_ERROR_RETURN(sys_humber_stats_get_flow_stats(0, stats_ptr, &stats));
    p_stats->byte_count   = stats.byte_count;
    p_stats->packet_count = stats.packet_count;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get stats by stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @param[out] p_stats                  Pointer to statistics data structure
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_clear_stats_ptr(uint16_ofp stats_ptr)
{
    HAL_ERROR_RETURN(sys_humber_stats_reset_flow_stats(0, stats_ptr));

    return OFP_ERR_SUCCESS;
}
