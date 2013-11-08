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
 * @brief This file is the header file of hal_flow.c
 */

#ifndef __HAL_FLOW_H__
#define __HAL_FLOW_H__ 

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ctc_aclqos.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
/**
 @brief hal flow unmatched behavior 
*/
enum hal_flow_unmatched_behavior_e
{
    HAL_FLOW_UNMATCHED_TO_CONTROLLER, /**< To Controller via packet-in */
    HAL_FLOW_UNMATCHED_DROP,          /**< Drop packet silently */
    HAL_FLOW_UNMATCHED_MAX,           /**< Type Max */
};
typedef enum hal_flow_unmatched_behavior_e hal_flow_unmatched_behavior_t;

extern hal_flow_unmatched_behavior_t unmatched_behavior;
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
* Get flow stats
* @param[in]  label_id                  label id
* @param[in]  label_type                label type
* @param[in]  key_type                  key type
* @param[in]  entry_id                  entry id
* @param[out] p_stats                   Pointer to stats data structure
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_entry_stats_get(uint32_ofp label_id, ctc_aclqos_label_type_t label_type, 
                        ctc_aclqos_key_type_t key_type, uint32_ofp entry_id, 
                        ofp_stats_t* p_stats);

/**
* Add default flow entry
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_add_default_entry(void);

/**
* Update default flow entry
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_update_default_entry(hal_flow_unmatched_behavior_t new_behavior);

/**
* Clear flow stats
* @param[in]  label_id                  label id
* @param[in]  label_type                label type
* @param[in]  key_type                  key type
* @param[in]  entry_id                  entry id
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_clear_flow_stats(uint32_ofp label_id, ctc_aclqos_label_type_t label_type,
                          ctc_aclqos_key_type_t entry_type, uint32_ofp entry_id);

/**
* Set flow action
* @param[in]  label_id                  label id
* @param[in]  label_type                label type
* @param[in]  key_type                  key type
* @param[in]  entry_id                  entry id
* @param[in]  p_action                  pointer to ctc_aclqos_action_t
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_set_flow_action(uint32_ofp label_id, ctc_aclqos_label_type_t label_type, 
                    ctc_aclqos_key_type_t entry_type, uint32_ofp entry_id,
                    ctc_aclqos_action_t* p_action);

/**
 * Unset flow action
 * @param[in]  label_id                 label_id
 * @param[in]  label_type               label type
 * @param[in]  entry_type               entry type
 * @param[in]  entry_id                 entry id
 * @param[out] p_action                 Pointer to action
 * @return OFP_ERR_XX
 */
int32_ofp
hal_flow_unset_flow_action(uint32_ofp label_id, ctc_aclqos_label_type_t label_type,
                    ctc_aclqos_key_type_t entry_type, uint32_ofp entry_id,
                    ctc_aclqos_action_t* p_action);

/**
* Add qos entry
* @param[in]  label_id                  label id
* @param[in]  entry_id                  entry id
* @param[in]  p_entry                   pointer to qos entry
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_add_qos_entry(uint32_ofp label_id, uint32_ofp entry_id, ctc_aclqos_entry_t* p_entry);

/**
* Remove qos entry
* @param[in]  label_id                  label id
* @param[in]  key_type                  key type
* @param[in]  entry_id                  entry id
* @return OFP_ERR_XX
*/
int32_ofp 
hal_flow_remove_qos_entry(uint32_ofp label_id, ctc_aclqos_key_type_t key_type, uint32_ofp entry_id);

/**
* Add service entry
* @param[in]  p_entry_oper              Pointer to ctc_aclqos_entry_oper_t
* @return OFP_ERR_XX
*/
int32_ofp 
hal_flow_add_service_entry(ctc_aclqos_entry_oper_t* p_entry_oper);

/**
* Remove service entry
* @param[in]  p_entry_oper              Pointer to ctc_aclqos_entry_oper_t
* @return OFP_ERR_XX
*/
int32_ofp 
hal_flow_remove_service_entry(ctc_aclqos_entry_oper_t* p_entry_oper);

#endif
