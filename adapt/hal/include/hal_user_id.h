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
 * @brief This file is the header file of hal_user_id.c
 */

#ifndef __HAL_USER_ID_H__
#define __HAL_USER_ID_H__ 

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
 * Add user id vlan key 
 * @param global_port                   global port
 * @param customer_id                   customer id
 * @param service_id                    service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_add_vlan_entry(uint16_ofp global_port, uint32_ofp customer_id, uint32_ofp service_id);

/**
 * Remove user id vlan key 
 * @param global_port                   global port
 * @param customer_id                   customer id
 * @param service_id                    service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_del_vlan_entry(uint16_ofp global_port, uint32_ofp customer_id, uint32_ofp service_id);

/**
 * Add default user id vlan entry per port
 * @param global_port                   global port
 * @param service_id                    service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_add_vlan_default_entry_per_port(uint16_ofp gport, uint32_ofp service_id);

/**
 * Remove default user id vlan entry per port
 * @param global_port                   global port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_del_default_enry_per_port(uint16_ofp gport);

#endif
