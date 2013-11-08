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
 * @brief This file calls the sdk user-id APIs.
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_macro.h"

#include "hal.h"
#include "hal_user_id.h"

#include "ctc_api.h"
#include "sys_humber_usrid.h"

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
hal_user_id_add_vlan_entry(uint16_ofp global_port, uint32_ofp customer_id, uint32_ofp service_id)
{
    sys_usrid_vlan_entry_t usrid_entry;
    
    memset(&usrid_entry, 0, sizeof(usrid_entry));

    usrid_entry.valid.src_port_valid = 1;
    usrid_entry.valid.gre_valid      = 1;

    usrid_entry.usrid_key_entry.global_port = global_port;
    usrid_entry.usrid_key_entry.customer_id = customer_id;

    usrid_entry.ds_entry_usrid.binding_data_h.srv_aclqos_en = 1;
    usrid_entry.ds_entry_usrid.binding_data_m.svr_id_en     = 1;
    usrid_entry.ds_entry_usrid.binding_data_m.svr_id        = service_id;
    usrid_entry.ds_entry_usrid.usr_vlan_ptr                 = 0x1FFF;

    HAL_ERROR_RETURN(sys_humber_usrid_add_vlan_entry(&usrid_entry));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Remove user id vlan key 
 * @param global_port                   global port
 * @param customer_id                   customer id
 * @param service_id                    service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_del_vlan_entry(uint16_ofp global_port, uint32_ofp customer_id, uint32_ofp service_id)
{
    sys_usrid_vlan_entry_t usrid_entry;
    
    memset(&usrid_entry, 0, sizeof(usrid_entry));

    usrid_entry.valid.src_port_valid = 1;
    usrid_entry.valid.gre_valid      = 1;

    usrid_entry.usrid_key_entry.global_port = global_port;
    usrid_entry.usrid_key_entry.customer_id = customer_id;

    usrid_entry.ds_entry_usrid.binding_data_h.srv_aclqos_en = 1;
    usrid_entry.ds_entry_usrid.binding_data_m.svr_id_en     = 1;
    usrid_entry.ds_entry_usrid.binding_data_m.svr_id        = service_id;
    usrid_entry.ds_entry_usrid.usr_vlan_ptr                 = 0x1FFF;

    HAL_ERROR_RETURN(sys_humber_usrid_delete_vlan_entry(&usrid_entry));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Add default user id vlan entry per port
 * @param global_port                   global port
 * @param service_id                    service id
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_add_vlan_default_entry_per_port(uint16_ofp gport, uint32_ofp service_id)
{
    sys_usrid_ds_entry_t usrid_def_entry;
    
    memset(&usrid_def_entry, 0, sizeof(usrid_def_entry));
    
    usrid_def_entry.binding_data_h.srv_aclqos_en = 1;
    usrid_def_entry.binding_data_m.svr_id_en     = 1;
    usrid_def_entry.binding_data_m.svr_id        = service_id;
    usrid_def_entry.usr_vlan_ptr                 = 0x1FFF;

    HAL_ERROR_RETURN(sys_humber_usrid_add_vlan_default_entry_per_port(
        gport, &usrid_def_entry));

    return OFP_ERR_SUCCESS;
}

/**
 * Remove default user id vlan entry per port
 * @param global_port                   global port
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_user_id_del_default_enry_per_port(uint16_ofp gport)
{
    HAL_ERROR_RETURN(sys_humber_usrid_delete_vlan_default_entry_per_port(gport));

    return OFP_ERR_SUCCESS;
}
