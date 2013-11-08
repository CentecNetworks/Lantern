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
 * @brief This file is the main file of hal layer
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_flow.h"
#include "ofp_macro.h"

#include "hal.h"
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
 * init some feature in sdk
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_misc_init(void)
{
    /* enable cpu mac */
    HAL_ERROR_RETURN(ctc_set_cpu_mac_en(true));

    /* limit cpu traffic */
    HAL_ERROR_RETURN(ctc_packet_tocpu_limit_total_rate(MAX_CPU_TRAFFIC_RATE));

    /* enable QoS manger */
    HAL_ERROR_RETURN(ctc_queue_resrc_mgr_global_enable(TRUE));

    /* enable QoS policer */
    HAL_ERROR_RETURN(ctc_qos_policer_global_enable(TRUE));

    /* enable QoS policer statistics */
    HAL_ERROR_RETURN(ctc_qos_policer_stats_global_enable(TRUE));

    /* enable Queue shape */
    HAL_ERROR_RETURN(ctc_queue_shape_global_enable(TRUE));

    /* enable Queue stats */
    HAL_ERROR_RETURN(ctc_queue_stats_global_enable(TRUE));

    return OFP_ERR_SUCCESS;
}
