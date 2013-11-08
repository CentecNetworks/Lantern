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
 * @brief This file is the public header file of adpt_flow.c
 */

#ifndef __ADPT_FLOW_H__
#define __ADPT_FLOW_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_flow.h"
#include "hal_flow.h"

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
 * Structures and macros, enums
 *
 ****************************************************************************/
#define OFP_MAP_FLOW_TYPE(dl_type)                                     \
    ((dl_type) == 0x0800 ?  FLOW_TYPE_IPV4 :                           \
     (dl_type) == 0x8847 ?  FLOW_TYPE_MPLS :                           \
     (dl_type) == 0x8848 ?  FLOW_TYPE_MPLS :                           \
     (dl_type) == 0x0000 ?  FLOW_TYPE_ANY  :                           \
     FLOW_TYPE_OTHER)
        
#define ADPT_FLOW_IS_IP_FLOW(p_rule_ctc) (ntohs((p_rule_ctc)->match.flow.dl_type) == 0x0800)

    
#define OFP_MAP_KEY_TYPE(flow_type)                                    \
    ((flow_type) == FLOW_TYPE_IPV4 ?  CTC_ACLQOS_IPV4_KEY :            \
     CTC_ACLQOS_MAC_KEY)

#define ADPT_FLOW_ERROR_RETURN(op) \
    ADPT_ERROR_RETURN(op)

#define ADPT_FLOW_MANAGE_ENTRY_PRIORITY 0xfffffff1

/**
 @brief adapter layer action combo
*/
enum adpt_res_op_type_e
{
    ADPT_RES_OP_TYPE_CHECK,      /**< Check resource */
    ADPT_RES_OP_TYPE_ADD,        /**< Add resource */
    ADPT_RES_OP_TYPE_DEL,        /**< Delete resource */
    ADPT_RES_OP_TYPE_MAX,        /**< Type Max */
};
typedef enum adpt_res_op_type_e adpt_res_op_type_t;

/**
 @brief QoS color
*/
enum adpt_qos_color_e
{
    ADPT_QOS_COLOR_NONE,     /**< None color */
    ADPT_QOS_COLOR_RED,      /**< Red color: the lowest drop precedence */
    ADPT_QOS_COLOR_YELLOW,   /**< Yellow color: the mild drop precedence */
    ADPT_QOS_COLOR_GREEN,    /**< Green color: the highest drop precedence */

    MAX_ADPT_QOS_COLOR
};
typedef enum adpt_qos_color_e adpt_qos_color_t;

enum ofp_flow_action_field_e
{
    OFP_FLOW_ACTION_FIELD_OUTPUT            = (1 << 0),
    OFP_FLOW_ACTION_FIELD_SET_MACSA         = (1 << 1),
    OFP_FLOW_ACTION_FIELD_SET_MACDA         = (1 << 2),
    OFP_FLOW_ACTION_FIELD_SET_TUNNEL_ID     = (1 << 3),
    OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID = (1 << 5),
    OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_COS = (1 << 6),
    OFP_FLOW_ACTION_FIELD_REPLACE_CVLAN_VID = (1 << 7),
    OFP_FLOW_ACTION_FIELD_STRIP_SVLAN       = (1 << 9),
    OFP_FLOW_ACTION_FIELD_STRIP_CVLAN       = (1 << 10),
    OFP_FLOW_ACTION_FIELD_PUSH_SVLAN        = (1 << 11),
    OFP_FLOW_ACTION_FIELD_PUSH_CVLAN        = (1 << 12),
    OFP_FLOW_ACTION_FIELD_INSTRUCTION_METER = (1 << 13),
    OFP_FLOW_ACTION_FIELD_SET_QUEUE_ID      = (1 << 14),
    OFP_FLOW_ACTION_FIELD_REPLACE_IPDA      = (1 << 15),
    OFP_FLOW_ACTION_FIELD_REPLACE_DST_PORT  = (1 << 16),
    OFP_FLOW_ACTION_FIELD_DEC_IP_TTL        = (1 << 17), 
};
typedef enum ofp_flow_action_field_e ofp_flow_action_field_t;

/**
 @brief adapter layer action combo
*/
struct adpt_flow_action_combo_s
{
    uint32_ofp flag;                    /**< bitmap of ofp_flow_action_field_t */

    uint16_ofp output_gport;             /**< output gport */
    
    uint8_ofp mac_da[OFP_ETH_ADDR_LEN]; /**< mac da */
    uint8_ofp mac_sa[OFP_ETH_ADDR_LEN]; /**< mac sa */
    uint16_ofp vlan_id;                 /**< vid */
    uint16_ofp cvlan_id;
    uint8_ofp  vlan_pcp;                /**< vlan pcp */
    uint8_ofp  cvlan_pcp;

    uint16_ofp stag_tpid;
    uint16_ofp ctag_tpid;

    uint32_ofp tunnel_id;

    uint32_ofp meter_id;

    uint16_ofp queue_id;
    uint16_ofp rsv1;        /**< reserved 1 */

    uint32_ofp ipda;        /**< ip da */

    uint16_ofp dst_port;    /**< dst port */
    uint16_ofp rsv2;        /**< reserved 2 */
};
typedef struct adpt_flow_action_combo_s adpt_flow_action_combo_t;

/**
 @brief adapter layer action type
*/
enum adpt_flow_action_type_e
{
    /* value in u16. */
    ADPT_FLOW_ACTION_TYPE_OUTPUT,
    /* value in u16. */
    ADPT_FLOW_ACTION_TYPE_PUSH_VLAN,
    /* no value. */
    ADPT_FLOW_ACTION_TYPE_STRIP_VLAN,
    /* no value. */
    ADPT_FLOW_ACTION_TYPE_PUSH_L2,
    /* no value. */
    ADPT_FLOW_ACTION_TYPE_POP_L2,
    /* value in u16. */
    ADPT_FLOW_ACTION_TYPE_PUSH_MPLS,
    /* no value. */
    ADPT_FLOW_ACTION_TYPE_POP_MPLS,
    /* value in u32. */
    ADPT_FLOW_ACTION_TYPE_SET_TUNNEL,
    /* value in mac. */
    ADPT_FLOW_ACTION_TYPE_SET_DL_SRC,
    /* value in mac. */
    ADPT_FLOW_ACTION_TYPE_SET_DL_DST,
    /* value in u16. */
    ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID,
    /* value in u32. */
    ADPT_FLOW_ACTION_TYPE_SET_MPLS_LABEL,
    /* value in u8. */
    ADPT_FLOW_ACTION_TYPE_SET_MPLS_TC,
    /* Note: In OF 1.3 Spec, unlike MPLS label and TC, MPLS TTL can't be set in
     * set_field action, there is a dedicated action OFPAT_SET_MPLS_TTL,
     * value in u8. */
    ADPT_FLOW_ACTION_TYPE_SET_MPLS_TTL,
    /* value in u32. */
    ADPT_FLOW_ACTION_TYPE_GROUP,
    /* Note: This is a mapped action from instruction meter
     * value in u32. */
    ADPT_FLOW_ACTION_TYPE_INSTRUCTION_METER,
    ADPT_FLOW_ACTION_TYPE_SET_QUEUE,
    ADPT_FLOW_ACTION_TYPE_SET_NW_DST,
    ADPT_FLOW_ACTION_TYPE_SET_TP_DST,
    ADPT_FLOW_ACTION_TYPE_DEC_NW_TTL,
    ADPT_FLOW_ACTION_TYPE_UNSUPPORTED
};
typedef enum adpt_flow_action_type_e adpt_flow_action_type_t;

#define ADPT_FLOW_ACTION_IS_IP_PKT_REWRITE(flow_action) \
    (((flow_action)->type == ADPT_FLOW_ACTION_TYPE_SET_NW_DST) || \
     ((flow_action)->type == ADPT_FLOW_ACTION_TYPE_SET_TP_DST))

#define ADPT_FLOW_ACTION_TYPE_TO_STR(type)                            \
    ((type == ADPT_FLOW_ACTION_TYPE_OUTPUT )       ? "OUTPUT"       : \
     (type == ADPT_FLOW_ACTION_TYPE_PUSH_VLAN )    ? "PUSH_VLAN"    : \
     (type == ADPT_FLOW_ACTION_TYPE_STRIP_VLAN )   ? "STRIP_VLAN"   : \
     (type == ADPT_FLOW_ACTION_TYPE_PUSH_L2 )      ? "PUSH_L2"      : \
     (type == ADPT_FLOW_ACTION_TYPE_POP_L2 )       ? "POP_L2"       : \
     (type == ADPT_FLOW_ACTION_TYPE_PUSH_MPLS )    ? "PUSH_MPLS"    : \
     (type == ADPT_FLOW_ACTION_TYPE_POP_MPLS )     ? "POP_MPLS"     : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_TUNNEL )   ? "SET_TUNNEL"   : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_DL_SRC )   ? "SET_DL_SRC"   : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_DL_DST )   ? "SET_DL_DST"   : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID ) ? "SET_VLAN_VID" : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_MPLS_LABEL)? "SET_MPLS_LBL" : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_MPLS_TC )  ? "SET_MPLS_TC"  : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_MPLS_TTL ) ? "SET_MPLS_TTL" : \
     (type == ADPT_FLOW_ACTION_TYPE_GROUP )        ? "GROUP"        : \
     (type == ADPT_FLOW_ACTION_TYPE_INSTRUCTION_METER)? "METER"     : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_QUEUE)     ? "SET_QUEUE"    : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_NW_DST)    ? "SET_NW_DST"   : \
     (type == ADPT_FLOW_ACTION_TYPE_SET_TP_DST)    ? "SET_TP_DST"   : \
     (type == ADPT_FLOW_ACTION_TYPE_DEC_NW_TTL)    ? "DEC_NW_TTL"   : \
     "UNKNOWN")
    

/**
 @brief adapter layer action representation
*/
struct adpt_flow_action_s
{
    struct list list_node;
    adpt_flow_action_type_t type;
    union 
    {
        uint8_ofp u8;
        uint16_ofp u16;
        uint32_ofp u32;
        uint8_ofp mac[OFP_ETH_ADDR_LEN];
    } value;
};
typedef struct adpt_flow_action_s adpt_flow_action_t;

#define ADPT_MPLS_LABEL_MASK 0xfffff
#define ADPT_MPLS_TC_MASK 0x7

#define ADPT_FLOW_ACTION_GET_FIRST(p_flow_action_list, p_first_flow_action) \
do \
{ \
    p_first_flow_action = CONTAINER_OF(list_front(p_flow_action_list), \
                                       adpt_flow_action_t, list_node); \
} while(0)

#define ADPT_FLOW_ACTION_GET_NEXT(p_cur_flow_action, p_next_flow_action) \
do \
{ \
    adpt_flow_action_t *tmp_flow_action = NULL; \
    tmp_flow_action = CONTAINER_OF((p_cur_flow_action)->list_node.next, \
                                   adpt_flow_action_t, list_node); \
    p_next_flow_action = tmp_flow_action; \
} while(0)

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Destroy the translated flow actions list.
 * @param[in] flow_actions list of translated flow actions.
 * @return OFP_ERR_XXX
 */
void 
adpt_flow_destroy_flow_actions(struct list *flow_actions);

/**
 * Translate OVS action to adapter representation.
 * @param[in] ofpacts                   action list
 * @param[in] ofpacts_len               action list length
 * @param[out] p_flow_actions           pointer to the translated flow actions
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_translate_ofpact(struct ofpact *ofpacts, unsigned int ofpacts_len, 
                           struct list *p_flow_actions);

/**
 * Validate action data.
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_validate_action_data(struct rule_ctc *p_rule);

/**
 * Operate output port resource in nexthop info
 * @param[in]  nh_info                  Pointer to ofp_nexthop_info_t
 * @param[in]  type                     adpt_res_op_type_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_op_nexthop_res(ofp_nexthop_info_t *nh_info, adpt_res_op_type_t type);

/**
 * Reset all flow stats
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_clear_all_flow_stats(void);

/**
 * Retrieve flow stats and save to DB
 * @param[in]  p_rule                   Pointer to OVS rule
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_retrieve_flow_stats(struct rule_ctc* p_rule);

/**
 * Get statistics of removed flows
 * @param[out] p_stats                  Pointer to stats
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_removed_flow_stats(ofp_stats_t* p_stats);

/**
 * Get the lasted time when a packet match the flow.
 * @param[in]  p_rule                 Pointer to struct rule_ctc
 * @param[out] p_last_matched         Last matched time
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_last_matched(struct rule_ctc* p_rule, int64_ofp* p_last_matched);

/**
 * Add flow entry
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_add_flow(struct rule_ctc* p_rule);

/**
 * Modify flow entry action
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_modify_flow_action(struct rule_ctc *p_rule);

/**
 * Delete flow entry
 * @param[in] p_rule                    pointer to struct rule_ctc
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_del_flow(struct rule_ctc *p_rule);

/**
 * Get action description string. Do not forget to free the return string
 * @param actions actions
 * @return action description string
 */
char *
adpt_flow_get_action_str(const struct ofpact * actions);

/**
 * Add timer to process idle time out
 * @return action description string
 */
int32_ofp
adpt_flow_add_idle_timeout_timer(void);

/**
 * Remove timer to process idle time out
 * @return action description string
 */
int32_ofp
adpt_flow_remove_idle_timeout_timer(void);

/**
 * Add interface manage entry
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_add_interface_manage_entry(uint16_ofp ofport);

/**
 * Delete interface manage entry
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_del_interface_manage_entry(uint16_ofp ofport);

/**
 * Adapter flow init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_init(void);

/****************************************************************************
 *  
 * FLOW DB Functions to show information
 *
 ****************************************************************************/
/**
 * @brief show ether_type - l3type map
 */
int32_ofp
adpt_flowdb_show_ether_type_l3type_map(void);

/**
 * @brief show flow entry number
 */
int32_ofp
adpt_flowdb_show_flow_entry_num(void);

/**
 * @brief show output port number
 */
int32_ofp
adpt_flowdb_show_output_port_num(void);

/**
 * @brief show priority-entry_id mapping database
 */
void
adpt_flowdb_show_priority_db(ofp_flow_type_t flow_type);

/**
 * Print flow information
 */
void
adpt_flowdb_show_flow_info(void);

/** 
 * check output action for the specified rule
 * @param[in] flow_actions       List of flow actions
 * @param[out] p_nh_info         pointer to ofp_nexthop_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_check_output_action(struct list *flow_actions, ofp_nexthop_info_t* p_nh_info);

/**
 * Get max output number of pushing two vlan tags and modifying mac
 * @return max number
 */
int32_ofp
adpt_flowdb_get_qinq_with_mac_max_num(void);

/**
 * Set max output number of pushing two vlan tags and modifying mac
 * @param max       Max entry number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_set_qinq_with_mac_max_num(int32_ofp max);

/**
 * Get current output number of pushing two vlan tags and modifying mac
 * @return current number
 */
int32_ofp
adpt_flowdb_get_qinq_with_mac_cur_num(void);

/**
 * Increase current output number of pushing two vlan tags and modifying mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_incr_qinq_with_mac_cur_num(void);

/**
 * Decrease current output number of pushing two vlan tags and modifying mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_decr_qinq_with_mac_cur_num(void);

/**
 * Map output action to mcast members
 * @param[in]  in_port             in_port
 * @param[in]  flow_actions        List of flow actions
 * @param[in]  flow_id             Flow id
 * @param[out] p_member_nh_array   Pointer of member nexthop array
 * @param[out] p_member_cnt        Pointer of member count
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_map_output_action_to_mcast_members(uint16_ofp in_port, 
                                    const struct list *flow_actions, uint32_ofp flow_id,
                                    ofp_nh_offset_t* p_member_nh_array, 
                                    uint32_ofp* p_member_cnt);

/**
 * allocate the multicast group id
 * @param[in]  p_action          adpt_flow_action_t
 * @param[out] p_action_combo    adpt_flow_action_combo_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flow_map_combo_action(const adpt_flow_action_t *p_action, adpt_flow_action_combo_t *p_action_combo);

/**
 * Get flow entry current number
 * @return flow entry current number
 */
int32_ofp
adpt_flowdb_get_flow_entry_cur_num(void);

/**
 * Get max queue shape profile num
 * @return max number
 */
int32_ofp
adpt_flowdb_get_queue_max_num(void);

/**
 * Get current queue shape profile number
 * @return current queue profile number
 */
uint32_ofp
adpt_flowdb_get_queue_cur_num(void);

/**
 * Set unmatched behavior for flow table. 
 * @param[in]  new_behavior      The new unmatched behavior.
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_set_unmatched_behavior(hal_flow_unmatched_behavior_t new_behavior);

/**
 * Get unmatched behavior for flow table. 
 * @param[in]  new_behavior      The new unmatched behavior.
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_unmatched_behavior(hal_flow_unmatched_behavior_t *cur_behavior);

#endif
