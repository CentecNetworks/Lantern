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
 * @brief This file is the header file of ofp_flow_api.c
 */

#ifndef __OFP_FLOW_API_H__
#define __OFP_FLOW_API_H__

/**
 * Translate OVS action to adapter representation.
 * @param[in] ofpacts                   action list
 * @param[in] ofpacts_len               action list length
 * @param[out] p_flow_actions           pointer to the translated flow actions
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_translate_ofpact(struct ofpact *ofpacts, unsigned int ofpacts_len, 
                     struct list *flow_actions);

/**
 * Validate action data.
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_validate_action_data(struct rule_ctc *p_rule);


/**
 * Destroy the translated flow actions list.
 * @param[in] flow_actions list of translated flow actions.
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_destroy_flow_actions(struct list *flow_actions);


/**
 * Add openflow flow to adapter layer
 * @param  rule         openflow rule structure
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_add_flow(struct rule_ctc* p_rule);

/**
 * Modify openflow flow action
 * @param  rule         openflow rule structure
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_modify_flow_action(struct rule_ctc* p_rule);

/**
 * Remove openflow flow
 * @param  p_rule         Pointer of struct rule_ctc
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_del_flow(struct rule_ctc *rule);

/**
 * Remove openflow flow
 * @param[in]  p_rule         Pointer of struct rule_ctc
 * @param[out] p_stats        Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_stats(struct rule_ctc* p_rule, ofp_stats_t* p_stats);


/**
 * Get flow miss matched statistics
 * @param[out]  p_stats         Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_missmatch_stats(ofp_stats_t* p_stats);

/**
 * Get removed flow statistics
 * @param[out]  p_stats         Pointer of statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_old_stats(ofp_stats_t* p_stats);

/**
 * Clear all flow statistics
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_clear_all_flows_stats(void);

/**
 * Get flow lasted matched time
 * @param[in]  p_rule          Pointer of struct rule_ctc
 * @param[out] p_last_match    Last used time
 * @return OFP_ERR_XXX
 */
int32_ofp
ofp_get_flow_last_matched_time(struct rule_ctc* p_rule, int64_ofp* p_last_match);

#endif /* !__OFP_FLOW_API_H__ */
