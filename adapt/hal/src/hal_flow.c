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
 * @brief This file calls the sdk flow APIs for adapter layer
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_const.h"
#include "ofp_macro.h"
#include "ofp_stats.h"
#include "ofp_flow.h"
#include "ofp_port.h"

#include "adpt_nexthop.h"

#include "hal.h"
#include "hal_flow.h"

#include "ctc_api.h"
#include "sys_humber_aclqos_entry.h"
#include "sys_humber_aclqos_api.h"
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
hal_flow_unmatched_behavior_t unmatched_behavior;
static uint16_ofp mac_default_entry_stats_ptr = SPECIAL_STATS_PTR;
static uint16_ofp ipv4_default_entry_stats_ptr = SPECIAL_STATS_PTR;
    
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
hal_flow_entry_stats_get(uint32_ofp label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t key_type, uint32_ofp entry_id, ofp_stats_t* p_stats)
{

    ctc_stats_basic_t stats;

    memset(&stats, 0, sizeof(ctc_stats_basic_t));
    HAL_ERROR_RETURN(ctc_qos_entry_stats_get(label_id, label_type, key_type, entry_id, &stats));
    p_stats->byte_count   = stats.byte_count;
    p_stats->packet_count = stats.packet_count;

    return OFP_ERR_SUCCESS;
};

/**
* Add default flow entry
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_add_default_entry(void)
{
    ctc_aclqos_entry_t mac_entry;
    ctc_aclqos_entry_t ipv4_entry;
    ctc_flex_nh_param_t nh_param;

    /* 1. create flex nexthop, action to_cpu, dsnh_offset set to default 
     *    flow id, if the unmatched behavior is to controller. */
    if (unmatched_behavior == HAL_FLOW_UNMATCHED_TO_CONTROLLER)
    {
        memset(&nh_param, 0, sizeof(ctc_flex_nh_param_t));
        nh_param.gport = CPU_PORT_NUM;
        nh_param.dsnh_offset = FLOW_ID_DEFAULT_FLOW;
        HAL_ERROR_RETURN(ctc_flex_nh_create(ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY, &nh_param));
    }
    
    /* 2. insert mac default qos entry, action: to cpu */
    memset(&mac_entry, 0, sizeof(ctc_aclqos_entry_t));
    mac_entry.entry_id = ENTRY_ID_MAC_DEFAULT;
    mac_entry.key.type = CTC_ACLQOS_MAC_KEY;
    if (unmatched_behavior == HAL_FLOW_UNMATCHED_TO_CONTROLLER)
    {
        mac_entry.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY;
    }
    else
    {
        mac_entry.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DROP;
    }
    SET_FLAG(mac_entry.key.key_info.mac_key.flag, CTC_ACLQOS_MAC_KEY_MACSA_FLAG);
    SET_FLAG(mac_entry.key.key_info.mac_key.flag, CTC_ACLQOS_MAC_KEY_MACDA_FLAG);

    mac_default_entry_stats_ptr = SPECIAL_STATS_PTR;
    sys_humber_stats_create_statsptr(0, 1, &mac_default_entry_stats_ptr);
    SET_FLAG(mac_entry.action.flag, CTC_ACLQOS_ACTION_STATS_FLAG);
    mac_entry.action.stats_ptr = mac_default_entry_stats_ptr;

    SET_FLAG(mac_entry.action.flag, CTC_ACLQOS_ACTION_DENY_BRIDGE_FLAG);
    SET_FLAG(mac_entry.action.flag, CTC_ACLQOS_ACTION_DENY_LEARN_FLAG);
    SET_FLAG(mac_entry.action.flag, CTC_ACLQOS_ACTION_DENY_ROUTE_FLAG);
    SET_FLAG(mac_entry.action.flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);
    HAL_ERROR_RETURN(
        ctc_qos_entry_add(CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL, CTC_ACLQOS_ENTRY_ID_TAIL, &mac_entry));

    /* 3. insert ipv4 default qos entry, action: to cpu */
    kal_memset(&ipv4_entry, 0, sizeof(ctc_aclqos_entry_t));
    ipv4_entry.entry_id = ENTRY_ID_IPV4_DEFAULT;
    ipv4_entry.key.type = CTC_ACLQOS_IPV4_KEY;
    if (unmatched_behavior == HAL_FLOW_UNMATCHED_TO_CONTROLLER)
    {
        ipv4_entry.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY;
    }
    else
    {
        ipv4_entry.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DROP;
    }
    SET_FLAG(ipv4_entry.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPSA_FLAG);
    SET_FLAG(ipv4_entry.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPDA_FLAG);

    ipv4_default_entry_stats_ptr = SPECIAL_STATS_PTR;
    sys_humber_stats_create_statsptr(0, 1, &ipv4_default_entry_stats_ptr);
    SET_FLAG(ipv4_entry.action.flag, CTC_ACLQOS_ACTION_STATS_FLAG);
    ipv4_entry.action.stats_ptr = ipv4_default_entry_stats_ptr;

    SET_FLAG(ipv4_entry.action.flag, CTC_ACLQOS_ACTION_DENY_BRIDGE_FLAG);
    SET_FLAG(ipv4_entry.action.flag, CTC_ACLQOS_ACTION_DENY_LEARN_FLAG);
    SET_FLAG(ipv4_entry.action.flag, CTC_ACLQOS_ACTION_DENY_ROUTE_FLAG);
    SET_FLAG(ipv4_entry.action.flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);

    HAL_ERROR_RETURN(
        ctc_qos_entry_add(CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL, CTC_ACLQOS_ENTRY_ID_TAIL, &ipv4_entry));

    return OFP_ERR_SUCCESS;
}

/**
* Update default flow entry
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_update_default_entry(hal_flow_unmatched_behavior_t new_behavior)
{
    ctc_aclqos_action_t new_action;
    ctc_flex_nh_param_t nh_param;

    memset(&new_action, 0, sizeof(new_action));
    /* Only need update redirect. */
    SET_FLAG(new_action.flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);

    if (new_behavior == HAL_FLOW_UNMATCHED_DROP)
    {
        HAL_ERROR_RETURN(ctc_flex_nh_remove(ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY));

        new_action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DROP;
    }
    else
    {
        memset(&nh_param, 0, sizeof(ctc_flex_nh_param_t));
        nh_param.gport = CPU_PORT_NUM;
        nh_param.dsnh_offset = FLOW_ID_DEFAULT_FLOW;
        HAL_ERROR_RETURN(ctc_flex_nh_create(ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY, 
                                            &nh_param));

        new_action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_DEFAULT_ENTRY;
    }

    HAL_ERROR_RETURN(ctc_qos_entry_set_action(
                         CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL, 
                         CTC_ACLQOS_MAC_KEY, ENTRY_ID_MAC_DEFAULT,
                         &new_action));

    HAL_ERROR_RETURN(ctc_qos_entry_set_action(
                         CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL, 
                         CTC_ACLQOS_IPV4_KEY, ENTRY_ID_IPV4_DEFAULT,
                         &new_action));    

    return OFP_ERR_SUCCESS;
}

/**
* Clear flow stats
* @return OFP_ERR_XX
*/
int32_ofp
hal_flow_clear_flow_stats(uint32_ofp label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32_ofp entry_id)
{
    HAL_ERROR_RETURN(sys_humber_aclqos_entry_stats_reset(label_id, label_type, entry_type, entry_id));

    return OFP_ERR_SUCCESS;
}

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
                    ctc_aclqos_action_t* p_action)
{
    if (CTC_QOS_LABEL == label_type)
    {
        HAL_ERROR_RETURN(ctc_qos_entry_set_action(label_id, entry_type, entry_id, p_action));
    }
    else if (CTC_SERVICE_LABEL == label_type)
    {
        HAL_ERROR_RETURN(ctc_service_entry_set_action(label_id, entry_type, entry_id, p_action));
    }
    else
    {
        return OFP_ERR_INVALID_PARAM;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Unset flow action
 * @param[in]  label_id                 label_id
 * @param[in]  label_type               label type
 * @param[in]  entry_type               entry type
 * @param[in]  entry_id                 entry id
 * @param[in]  p_action                 Pointer to action
 * @return OFP_ERR_XX
 */
int32_ofp
hal_flow_unset_flow_action(uint32_ofp label_id, ctc_aclqos_label_type_t label_type,
                    ctc_aclqos_key_type_t entry_type, uint32_ofp entry_id,
                    ctc_aclqos_action_t* p_action)
{
    if (CTC_QOS_LABEL == label_type)
    {
        HAL_ERROR_RETURN(ctc_qos_entry_unset_action(label_id, entry_type, entry_id, p_action));
    }
    else if (CTC_SERVICE_LABEL == label_type)
    {
        HAL_ERROR_RETURN(sys_humber_aclqos_entry_unset_action(label_id, CTC_SERVICE_LABEL,
                    entry_type, entry_id, p_action));
    }
    else
    {
        return OFP_ERR_INVALID_PARAM;
    }

    return OFP_ERR_SUCCESS;
}

/**
* Add qos entry
* @param[in]  label_id                  label id
* @param[in]  entry_id                  entry id
* @param[in]  p_entry                   pointer to qos entry
* @return OFP_ERR_XX
*/
int32
hal_flow_add_qos_entry(uint32_ofp label_id, uint32_ofp entry_id, ctc_aclqos_entry_t* p_entry)
{
    HAL_ERROR_RETURN(ctc_qos_entry_add(label_id, entry_id, p_entry));

    return OFP_ERR_SUCCESS;
}

/**
* Remove qos entry
* @param[in]  label_id                  label id
* @param[in]  key_type                  key type
* @param[in]  entry_id                  entry id
* @return OFP_ERR_XX
*/
int32 
hal_flow_remove_qos_entry(uint32_ofp label_id, ctc_aclqos_key_type_t key_type, uint32_ofp entry_id)
{
    HAL_ERROR_RETURN(ctc_qos_entry_remove(label_id, key_type, entry_id));

    return OFP_ERR_SUCCESS;
}

/**
* Add service entry
* @param[in]  p_entry_oper              Pointer to ctc_aclqos_entry_oper_t
* @return OFP_ERR_XX
*/
int32 
hal_flow_add_service_entry(ctc_aclqos_entry_oper_t* p_entry_oper)
{
    HAL_ERROR_RETURN(ctc_service_entry_add(p_entry_oper));

    return OFP_ERR_SUCCESS;
}

/**
* Remove service entry
* @param[in]  p_entry_oper              Pointer to ctc_aclqos_entry_oper_t
* @return OFP_ERR_XX
*/
int32 
hal_flow_remove_service_entry(ctc_aclqos_entry_oper_t* p_entry_oper)
{
    HAL_ERROR_RETURN(ctc_service_entry_remove(p_entry_oper));

    return OFP_ERR_SUCCESS;
}
