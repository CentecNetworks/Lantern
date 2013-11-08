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
 * @brief This file is the header file of hal_vlan.c
 */

#ifndef __HAL_VLAN_H__
#define __HAL_VLAN_H__ 

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
 * init vlan by vid
 * @param[in]  vid                  Vlan id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_vlan_init_vlan(uint16_ofp vid);

/**
 * Add vlan group
 * @param[in]  vid                  Vlan id
 * @param[in]  group_id             Group id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_vlan_add_vlan_group(uint16_ofp vid, uint32_ofp group_id);

#endif
