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
 * @brief This file is the flow related APIs of adapter
 */

/******************************************************************************
* Header Files 
******************************************************************************/

#include "afx.h"
#include "ofp_api.h"
#include "ofp_lib.h"

#include "adpt_flow.h"
#include "adpt_flow_priv.h"
#include "adpt_port.h"
#include "adpt_gre_tunnel.h"
#include "adpt_nexthop.h"

#include "hal_stats.h"
#include "hal_flow.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

VLOG_DEFINE_THIS_MODULE(ofp_flow_api);


/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Translate OVS action to adapter representation.
 * @param[in] ofpacts                   action list
 * @param[in] ofpacts_len               action list length
 * @param[out] p_flow_actions           pointer to the translated flow actions
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_translate_ofpact(struct ofpact *ofpacts, unsigned int ofpacts_len, 
                     struct list *flow_actions)
{
    OFP_PTR_CHECK(ofpacts);
    OFP_PTR_CHECK(flow_actions);
    OFP_LOG_DEBUG_FUNC();
    
    OFP_ERROR_RETURN(adpt_flow_translate_ofpact(ofpacts, ofpacts_len, flow_actions));

    return OFP_ERR_SUCCESS;
}

/**
 * Validate action data.
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_validate_action_data(struct rule_ctc *p_rule)
{
    OFP_PTR_CHECK(p_rule);
    OFP_LOG_DEBUG_FUNC();
    
    OFP_ERROR_RETURN(adpt_flow_validate_action_data(p_rule));

    return OFP_ERR_SUCCESS;
}

/**
 * Destroy the translated flow actions list.
 * @param[in] flow_actions list of translated flow actions.
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_destroy_flow_actions(struct list *flow_actions)
{
    OFP_PTR_CHECK(flow_actions);
    OFP_LOG_DEBUG_FUNC();
    
    adpt_flow_destroy_flow_actions(flow_actions);

    return OFP_ERR_SUCCESS;
}

/**
 * Add openflow flow to adapter layer
 * @param  p_rule               Pointer of struct rule_ctc
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_add_flow(struct rule_ctc* p_rule)
{
    OFP_PTR_CHECK(p_rule);
    OFP_LOG_DEBUG_FUNC();
    
    OFP_ERROR_RETURN(adpt_flow_add_flow(p_rule));

    return OFP_ERR_SUCCESS;
}

/**
 * Modify openflow flow action
 * @param  p_rule               openflow rule structure
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_modify_flow_action(struct rule_ctc* p_rule)
{
    OFP_PTR_CHECK(p_rule);
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_modify_flow_action(p_rule));

    return OFP_ERR_SUCCESS;
}

/**
 * Remove openflow flow
 * @param  p_rule             Pointer of struct rule_ctc
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_del_flow(struct rule_ctc *p_rule)
{
    OFP_PTR_CHECK(p_rule);
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_del_flow(p_rule));

    return OFP_ERR_SUCCESS;
}

/**
 * Remove openflow flow
 * @param[in]  p_rule           Pointer of struct rule_ctc
 * @param[out] p_stats          Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_stats(struct rule_ctc* p_rule, ofp_stats_t* p_stats)
{
    OFP_PTR_CHECK(p_rule);
    OFP_PTR_CHECK(p_stats);
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(hal_stats_get_stats(p_rule->stats_ptr, p_stats));

    return OFP_ERR_SUCCESS;
}

/**
 * Get flow miss matched statistics
 * @param[out]  p_stats         Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_missmatch_stats(ofp_stats_t* p_stats)
{
    ofp_stats_t stats;

    OFP_PTR_CHECK(p_stats);
    OFP_LOG_DEBUG_FUNC();

    /* 1. mac default entry */
    memset(&stats, 0 , sizeof(ofp_stats_t));
    OFP_ERROR_RETURN(hal_flow_entry_stats_get(CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL,
        CTC_QOS_LABEL, CTC_ACLQOS_MAC_KEY, ENTRY_ID_MAC_DEFAULT, &stats));
    p_stats->packet_count = stats.packet_count;
    p_stats->byte_count   = stats.byte_count;
    
    /* 2. ipv4 default entry */
    memset(&stats, 0 , sizeof(ofp_stats_t));
    OFP_ERROR_RETURN(hal_flow_entry_stats_get(CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL,
        CTC_QOS_LABEL, CTC_ACLQOS_IPV4_KEY, ENTRY_ID_IPV4_DEFAULT, &stats));
    p_stats->packet_count += stats.packet_count;
    p_stats->byte_count   += stats.byte_count;
    
    /* 3. route default entry */
    memset(&stats, 0 , sizeof(ofp_stats_t));
    OFP_ERROR_RETURN(adpt_tunnel_get_gre_packets_miss_match_stats(&stats));
    p_stats->packet_count += stats.packet_count;
    p_stats->byte_count   += stats.byte_count;
    return OFP_ERR_SUCCESS;
}

/**
 * Get removed flow statistics
 * @param[out]  p_stats         Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_old_stats(ofp_stats_t* p_stats)
{
    OFP_PTR_CHECK(p_stats);
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_get_removed_flow_stats(p_stats));

    return OFP_ERR_SUCCESS;
}

/**
 * Clear all flow statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_clear_all_flows_stats(void)
{
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_clear_all_flow_stats());
    OFP_ERROR_RETURN(adpt_tunnel_clear_gre_packets_miss_match_stats());
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get the last time when a packet match the flow.
 * @param[in]  p_rule                   Pointer to struct rule_ctc
 * @param[out] p_last_matched           Lasted matched time
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_last_matched_time(struct rule_ctc* p_rule, int64_ofp* p_last_match)
{
    int64_ofp last_match = 0;

    OFP_PTR_CHECK(p_rule);
    OFP_PTR_CHECK(p_last_match);
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_get_last_matched(p_rule, &last_match));
    *p_last_match = last_match;
    
    return OFP_ERR_SUCCESS;
}
