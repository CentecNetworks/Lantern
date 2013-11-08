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
 * @brief This file is the header file of hal_stats.c
 */

#ifndef __HAL_STATS_H__
#define __HAL_STATS_H__ 

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
 * Create stats ptr
 * @param[out]  p_stats_ptr             Pointer to stats ptr
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_create_stats_ptr(uint16_ofp* p_stats_ptr);

/**
 * Delete stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_delete_stats_ptr(uint16_ofp stats_ptr);

/**
 * Get stats by stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @param[out] p_stats                  Pointer to statistics data structure
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_get_stats(uint16_ofp stats_ptr, ofp_stats_t* p_stats);

/**
 * Clear stats by stats ptr
 * @param[in]  stats_ptr                Stats ptr
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_stats_clear_stats_ptr(uint16_ofp stats_ptr);

#endif
