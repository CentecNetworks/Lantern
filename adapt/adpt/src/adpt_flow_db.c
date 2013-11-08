
/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file is the database implementation of adapter layer flow
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "afx.h"

#include "vlog.h"
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_flow.h"
#include "adpt_flow_priv.h"
#include "adpt_parser.h"
#include "glb_stm_define.h"
#include "adpt_port.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_flow);

extern uint8_ofp g_current_profile;

#define ADPT_FLOWDB_SELECT_DB(ret)                          \
do {                                                        \
    if (flow_type == FLOW_TYPE_IPV4)                        \
    {                                                       \
        p_priority_list = ADPT_FLOW_MAC_PRIORITY_LIST;      \
        p_priority_hash = ADPT_FLOW_MAC_PRIORITY_HASH;      \
    }                                                       \
    else if (flow_type == FLOW_TYPE_MAC ||                  \
             flow_type == FLOW_TYPE_ANY ||                  \
             flow_type == FLOW_TYPE_OTHER)                  \
    {                                                       \
        p_priority_list = ADPT_FLOW_IPV4_PRIORITY_LIST;     \
        p_priority_hash = ADPT_FLOW_IPV4_PRIORITY_HASH;     \
    }                                                       \
    else                                                    \
    {                                                       \
        return ret;                                         \
    }                                                       \
}                                                           \
while (0)

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Get max num of ether type - l3 type map 
 * @return max num
 */
int32_ofp
adpt_flowdb_get_ether_type_map_l3type_max_num(void)
{
    return g_p_adpt_flow_master->ether_type_l3type_map_max_num;
}

/**
 * Get l3type base of ether type - l3 type map 
 * @return l3type base
 */
int32_ofp
adpt_flowdb_get_ether_type_map_l3type_base(void)
{
    return g_p_adpt_flow_master->ether_type_l3type_map_base_l3type;
}

static int32_ofp
adpt_flowdb_set_ether_type_map_l3type_max_num(uint8_ofp max_num)
{
    g_p_adpt_flow_master->ether_type_l3type_map_max_num = max_num;
    
    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_flowdb_set_ether_type_map_l3type_base(uint8_ofp base_l3type)
{
    g_p_adpt_flow_master->ether_type_l3type_map_base_l3type = base_l3type;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Add ether type - l3 type map 
 * @param[in] ether_type                ether_type
 * @param[in] ether_type_mask           ether_type_mask
 * @param[out] p_index                  pointer to index
 * @param[out] p_l3type                 pointer to l3type
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_insert_map_ether_type_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp* p_index, ctc_parser_l3_type_t* p_l3type)
{
    int idx1, idx2;

    idx2 = OFP_ETHER_TYPE_L3TYPE_MAP_IDX_MAX;
    for (idx1 = 0; idx1 < adpt_flowdb_get_ether_type_map_l3type_max_num(); idx1++)
    {
        if (g_p_adpt_flow_master->ether_type_l3type_map[idx1].ref)
        {
            if (g_p_adpt_flow_master->ether_type_l3type_map[idx1].ether_type == ether_type &&
                g_p_adpt_flow_master->ether_type_l3type_map[idx1].ether_type_mask == ether_type_mask)
            {
                g_p_adpt_flow_master->ether_type_l3type_map[idx1].ref ++;
                /* output index 0xff to avoid add l3type to sdk unnecessarily */
                *p_index = 0xff;
                *p_l3type = g_p_adpt_flow_master->ether_type_l3type_map[idx1].l3type;
                return OFP_ERR_SUCCESS;
            }
        }
        else
        {
            idx2 = ( OFP_ETHER_TYPE_L3TYPE_MAP_IDX_MAX == idx2 ) ? idx1 : idx2;
        }
    }
    
    if (idx2 < OFP_ETHER_TYPE_L3TYPE_MAP_IDX_MAX)
    {
        g_p_adpt_flow_master->ether_type_l3type_map[idx2].ether_type = ether_type ;
        g_p_adpt_flow_master->ether_type_l3type_map[idx2].ether_type_mask = ether_type_mask;
        g_p_adpt_flow_master->ether_type_l3type_map[idx2].ref = 1;
        *p_index = idx2;
        *p_l3type = g_p_adpt_flow_master->ether_type_l3type_map[idx2].l3type;
    }
    else
    {
        return OFP_ERR_ETHER_TYPE_FULL;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Remove ether type - l3 type map 
 * @param[in] ether_type                ether_type
 * @param[in] ether_type_mask           ether_type_mask
 * @param[out] p_index                  pointer to index
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_remove_map_ether_type_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp* p_index)
{
    int idx;
    *p_index = 0xff;

    for (idx = 0; idx < adpt_flowdb_get_ether_type_map_l3type_max_num(); idx++)
    {
        if (g_p_adpt_flow_master->ether_type_l3type_map[idx].ref &&
            g_p_adpt_flow_master->ether_type_l3type_map[idx].ether_type == ether_type &&
            g_p_adpt_flow_master->ether_type_l3type_map[idx].ether_type_mask == ether_type_mask)
        {
            if (g_p_adpt_flow_master->ether_type_l3type_map[idx].ref)
            {
                g_p_adpt_flow_master->ether_type_l3type_map[idx].ref --;
            }
            if (0 == g_p_adpt_flow_master->ether_type_l3type_map[idx].ref)
            {
                *p_index = idx;
            }
            return OFP_ERR_SUCCESS;
        }
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Show ether type - l3 type map 
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_show_ether_type_l3type_map(void)
{
    int idx;
    int cnt = 1;

    ctc_cli_out_ofp ("The maximum of ether-type l3type map is %d\n", adpt_flowdb_get_ether_type_map_l3type_max_num());
    ctc_cli_out_ofp ("------------------------------------------\n");
    for (idx = 0; idx < adpt_flowdb_get_ether_type_map_l3type_max_num(); idx++)
    {
        if (!g_p_adpt_flow_master->ether_type_l3type_map[idx].ref)
        {
            continue;
        }

        ctc_cli_out_ofp ("%2d, index:%2d, ether-type:0x%04x, mask:0x%04x, l3type:%2d, ref:%d\n",
            cnt++,
            idx + adpt_flowdb_get_ether_type_map_l3type_base(),
            g_p_adpt_flow_master->ether_type_l3type_map[idx].ether_type,
            g_p_adpt_flow_master->ether_type_l3type_map[idx].ether_type_mask,
            g_p_adpt_flow_master->ether_type_l3type_map[idx].l3type,
            g_p_adpt_flow_master->ether_type_l3type_map[idx].ref);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Add flow information
 * @param flow_id                       flow id
 * @param flow_info                     flow information
 */
void
adpt_flowdb_add_flow_info(uint32_ofp flow_id, adpt_flow_info_t * flow_info)
{
    struct ihash_node *node = NULL;

    node = ihash_find(&g_p_adpt_flow_master->flow_info_ihmap, flow_id);
    if (node == NULL)
    {
        ihash_add(&g_p_adpt_flow_master->flow_info_ihmap, flow_id, flow_info);
    }
}

/**
 * Delete flow information
 * @param flow_id                       flow id
 */
void
adpt_flowdb_del_flow_info(uint32_ofp flow_id)
{
    struct ihash_node *node = NULL;
    adpt_flow_info_t* p_flow_info = NULL;

    node = ihash_find(&g_p_adpt_flow_master->flow_info_ihmap, flow_id);
    if (node == NULL)
    {
        return;
    }
    p_flow_info = (adpt_flow_info_t*) node->data;

    ihash_delete(&g_p_adpt_flow_master->flow_info_ihmap, node);
    free(p_flow_info);
}

/**
 * Get flow information
 * @param flow_id                       flow id
 * @return flow                         information
 */
adpt_flow_info_t *
adpt_flowdb_get_flow_info(uint32_ofp flow_id)
{
    struct ihash_node *node = NULL;

    node = ihash_find(&g_p_adpt_flow_master->flow_info_ihmap, flow_id);
    if (node != NULL)
    {
        return (adpt_flow_info_t *) node->data;
    }
    return NULL;
}

/**
 * Get flow info ihmap
 * @return flow info ihmap
 */
struct ihash *
adpt_flowdb_get_flow_info_ihmap(void)
{
    return &g_p_adpt_flow_master->flow_info_ihmap;
}

void
adpt_flowdb_show_flow_info(void)
{
    uint32_ofp key = 0;
    adpt_flow_info_t * data;
    int i = 0;
    struct ihash_node *node, *next;

    ctc_cli_out_ofp("--------------------------Flow INFO DB ----------------------------------------------------------\n");

    ctc_cli_out_ofp("%5s %7s %8s %6s %10s %10s %10s %20s %10s\n",
        "index", "flow_id", "entry_id", "extra_id", 
        "packet_cnt", "last_match", "timeout", "is_idle_timer", "need_delete");
    ctc_cli_out_ofp("-------------------------------------------------------------------------------------------------\n");
    IHASH_FOR_EACH_SAFE(node, next, &g_p_adpt_flow_master->flow_info_ihmap)
    {
        key = node->key;
        data = (adpt_flow_info_t *) (node->data);
        ctc_cli_out_ofp("%5d %5u %8u %6u %10llu %10u %10u %20u %10u\n",
                i, key, data->p_rule->entry_id, data->p_rule->extra_entry_id,
                data->packet_count, data->last_matched,
                data->idle_timeout, data->is_idle_timer, data->need_delete);
        i++;
    }
}

/**
 * Set flow entry max number
 * @param[in] max                       flow entry max number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_set_flow_entry_max_num(int32_ofp max)
{
    g_p_adpt_flow_master->flow_entry_num_max = max;

    return OFP_ERR_SUCCESS;
}

/**
 * Get flow entry max number
 * @return max flow entry number
 */
int32_ofp
adpt_flowdb_get_flow_entry_max_num(void)
{
    return g_p_adpt_flow_master->flow_entry_num_max;
}

/**
 * Get flow entry current number
 * @return flow entry current number
 */
int32_ofp
adpt_flowdb_get_flow_entry_cur_num(void)
{
    return g_p_adpt_flow_master->flow_entry_num_count;
}

/**
 * Increase flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_incr_flow_entry_num(void)
{
    g_p_adpt_flow_master->flow_entry_num_count ++;

    return OFP_ERR_SUCCESS;
}

/**
 * Decrease flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_decr_flow_entry_num(void)
{
    if (0 == g_p_adpt_flow_master->flow_entry_num_count)
    {
        OFP_DEBUG_PRINT("flow entry count error\n");
        return OFP_ERR_SUCCESS;
    }
    g_p_adpt_flow_master->flow_entry_num_count --;

    return OFP_ERR_SUCCESS;
}

/**
 * show flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_show_flow_entry_num(void)
{
    ctc_cli_out_ofp (" Flow entry num : %4d / %4d\n", 
        adpt_flowdb_get_flow_entry_cur_num(), adpt_flowdb_get_flow_entry_max_num());

    return OFP_ERR_SUCCESS;
}

static uint32_ofp
adpt_flowdb_get_output_port_count(void)
{
    return g_p_adpt_flow_master->output_port_count;
}

static uint32_ofp
adpt_flowdb_get_output_port_max(void)
{
    return g_p_adpt_flow_master->output_port_max;
}

/**
 * Get gre and mpls push output count
 * @return output count
 */
uint32_ofp
adpt_flowdb_get_gre_and_mpls_push_output_count(void)
{
    return g_p_adpt_flow_master->gre_mpls_output_count;
}

/**
 * Get gre and mpls push output max
 * @return output max
 */
uint32_ofp
adpt_flowdb_get_gre_and_mpls_push_output_max(void)
{
    return g_p_adpt_flow_master->gre_mpls_output_max;
}

/**
 * Operate output port resource in nexthop info
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_op_nexthop_res(ofp_nexthop_info_t *nh_info, adpt_res_op_type_t type)
{
    switch(type)
    {
    case ADPT_RES_OP_TYPE_CHECK:
        if (g_p_adpt_flow_master->output_port_count + nh_info->output_count > 
            g_p_adpt_flow_master->output_port_max)
        {
            OFP_LOG_ERROR("Exceed maximum output capability [%d].", 
                            g_p_adpt_flow_master->output_port_max);
            return OFP_ERR_TOO_MANY_OUTPUT;
        }
        if (g_p_adpt_flow_master->gre_mpls_output_count + nh_info->gre_and_mpls_push_output_count > 
            g_p_adpt_flow_master->gre_mpls_output_max)
        {
            OFP_LOG_ERROR("Exceed maximum gre, modify nw_dst/tp_dst and mpls push output capability [%d].",
                            g_p_adpt_flow_master->gre_mpls_output_max);
            return OFP_ERR_TOO_MANY_OUTPUT;
        }

        if (g_p_adpt_flow_master->mpls_output_count + nh_info->mpls_output_count > 
            g_p_adpt_flow_master->mpls_output_max)
        {
            OFP_LOG_ERROR("Exceed maximum mpls output(including mpls push or mpls pop action) capability [%d].", 
                g_p_adpt_flow_master->mpls_output_max);
            return OFP_ERR_TOO_MANY_OUTPUT;
        }
        break;
        
    case ADPT_RES_OP_TYPE_ADD:
        g_p_adpt_flow_master->output_port_count += nh_info->output_count;
        g_p_adpt_flow_master->gre_mpls_output_count += nh_info->gre_and_mpls_push_output_count;
        g_p_adpt_flow_master->mpls_output_count += nh_info->mpls_output_count;
        break;
        
    case ADPT_RES_OP_TYPE_DEL:
        if (g_p_adpt_flow_master->output_port_count < nh_info->output_count)
        {
            g_p_adpt_flow_master->output_port_count = 0;
            OFP_LOG_ERROR("Output counting error.");
        }
        else
        {
            g_p_adpt_flow_master->output_port_count -= nh_info->output_count;
        }

        if (g_p_adpt_flow_master->gre_mpls_output_count < nh_info->gre_and_mpls_push_output_count)
        {
            g_p_adpt_flow_master->gre_mpls_output_count = 0;
            OFP_LOG_ERROR("gre and mpls push output counting error.");
        }
        else
        {
            g_p_adpt_flow_master->gre_mpls_output_count -= nh_info->gre_and_mpls_push_output_count;
        }

        if (g_p_adpt_flow_master->mpls_output_count < nh_info->mpls_output_count)
        {
            g_p_adpt_flow_master->mpls_output_count = 0;
            OFP_LOG_ERROR("mpls(including mpls push or mpls pop action) output counting error.");
        }
        else
        {
            g_p_adpt_flow_master->mpls_output_count -= nh_info->mpls_output_count;
        }
        break;
        
    case ADPT_RES_OP_TYPE_MAX:
    default:
        return OFP_ERR_INVALID_PARAM;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get max output number of pushing two vlan tags and modifying mac
 * @return max number
 */
int32_ofp
adpt_flowdb_get_qinq_with_mac_max_num(void)
{
    return g_p_adpt_flow_master->qinq_with_mac_max;
}

/**
 * Set max output number of pushing two vlan tags and modifying mac
 * @param max       Max entry number
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_set_qinq_with_mac_max_num(int32_ofp max)
{
    g_p_adpt_flow_master->qinq_with_mac_max = max;

    return 0;
}

/**
 * Get current output number of pushing two vlan tags and modifying mac
 * @return current number
 */
int32_ofp
adpt_flowdb_get_qinq_with_mac_cur_num(void)
{
    return g_p_adpt_flow_master->qinq_with_mac_cur;
}

/**
 * Increase current output number of pushing two vlan tags and modifying mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_incr_qinq_with_mac_cur_num(void)
{
    g_p_adpt_flow_master->qinq_with_mac_cur++;

    return 0;
}

/**
 * Decrease current output number of pushing two vlan tags and modifying mac
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flowdb_decr_qinq_with_mac_cur_num(void)
{
    if (g_p_adpt_flow_master->qinq_with_mac_cur == 0)
    {
        OFP_LOG_ERROR("QinQ with mac entry count error.\n");
        return 0;
    }
    g_p_adpt_flow_master->qinq_with_mac_cur--;

    return 0;
}

int32_ofp
adpt_flowdb_show_output_port_num(void)
{
    ctc_cli_out_ofp (" Output entry num                    : %4d / %4d\n", 
        adpt_flowdb_get_output_port_count(), adpt_flowdb_get_output_port_max());
    ctc_cli_out_ofp (" GRE and MPLS(push) output entry num : %4d / %4d\n", 
        adpt_flowdb_get_gre_and_mpls_push_output_count(), adpt_flowdb_get_gre_and_mpls_push_output_max());
    ctc_cli_out_ofp (" QinQ with modified mac output entry num : %4d / %4d\n",
        adpt_flowdb_get_qinq_with_mac_cur_num(), adpt_flowdb_get_qinq_with_mac_max_num());

    return OFP_ERR_SUCCESS;
}

static inline uint32_ofp
adpt_flowdb_priority_hash_make(void* pst_key_void)
{
    adpt_flow_priority_hash_t* pst_key = NULL;
    pst_key = pst_key_void;

    return pst_key->priority;
}

static inline bool
adpt_flowdb_priority_hash_cmp (void* pv_node_a_void, void* pv_node_b_void)
{
    adpt_flow_priority_hash_t* pv_node_a = NULL;
    adpt_flow_priority_hash_t* pv_node_b = NULL;

    pv_node_a = pv_node_a_void;
    pv_node_b = pv_node_b_void;

    if (pv_node_a->priority != pv_node_b->priority)
    {
        return FALSE;
    }

    return TRUE;
}

static inline adpt_flow_priority_hash_t*
adpt_flowdb_priority_hash_lookup(ctc_hash_t *p_hash, uint32_ofp priority)
{
    adpt_flow_priority_hash_t lookup_key;
    
    lookup_key.priority = priority;
    
    return ctc_hash_lookup(p_hash, &lookup_key);
}

static inline int32_ofp
adpt_flowdb_priority_hash_add(ctc_hash_t *p_hash, adpt_flow_priority_hash_t* p_priority)
{
    ctc_hash_insert(p_hash, p_priority);

    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_priority_hash_del(ctc_hash_t *p_hash, adpt_flow_priority_hash_t* p_priority)
{
    ctc_hash_remove(p_hash, p_priority);

    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_priority_list_add(ctc_list_pointer_t *p_list, adpt_flow_priority_list_t* p_priority)
{
    ctc_list_pointer_node_t* p_node = NULL;
    ctc_list_pointer_node_t* p_head_node = NULL;
    ctc_list_pointer_node_t* p_tail_node = NULL;
    adpt_flow_priority_list_t* p_head_priority = NULL;
    adpt_flow_priority_list_t* p_tail_priority = NULL;

    /* 1. check whether the priority is the first one */
    if (ctc_list_pointer_empty(p_list))
    {
        ctc_list_pointer_insert_head(p_list, &p_priority->node);
        return OFP_ERR_SUCCESS;
    }

    /* 2. check whether the priority is larger than all other priorities */
    p_head_node = ctc_list_pointer_head(p_list);
    p_head_priority = _ctc_container_of(p_head_node, adpt_flow_priority_list_t, node);
    if (p_priority->priority > p_head_priority->priority)
    {
        ctc_list_pointer_insert_head(p_list, &p_priority->node);
        return OFP_ERR_SUCCESS;
    }

    /* 3. check whether the priority is lower than all other priorities */
    p_tail_node = ctc_list_pointer_node_tail(p_list);
    p_tail_priority = _ctc_container_of(p_tail_node, adpt_flow_priority_list_t, node);
    if (p_priority->priority < p_tail_priority->priority)
    {
        ctc_list_pointer_insert_tail(p_list, &p_priority->node);
        return OFP_ERR_SUCCESS;
    }

    /* 4. find the position */
    if ((p_head_priority->priority - p_priority->priority) <
        (p_priority->priority - p_tail_priority->priority))
    {
        /* loop from head */
        CTC_LIST_POINTER_LOOP(p_node, p_list)
        {
            p_head_priority = _ctc_container_of(p_node, adpt_flow_priority_list_t, node);
            if (p_head_priority->priority < p_priority->priority)
            {
                ctc_list_pointer_insert_before(p_list, &p_head_priority->node, &p_priority->node);
                return OFP_ERR_SUCCESS;
            }
        }
    }
    else
    {
        /* loop from tail */
        CTC_LIST_POINTER_LOOP_R(p_node, p_list)
        {
            p_tail_priority = _ctc_container_of(p_node, adpt_flow_priority_list_t, node);
            if (p_tail_priority->priority > p_priority->priority)
            {
                ctc_list_pointer_insert_after(p_list, &p_tail_priority->node, &p_priority->node);
                return OFP_ERR_SUCCESS;
            }
        }
    }
    
    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_priority_list_del(ctc_list_pointer_t *p_list, adpt_flow_priority_list_t* p_priority)
{
    ctc_list_pointer_delete(p_list, &p_priority->node);

    return OFP_ERR_SUCCESS;
}

static inline uint32_ofp
adpt_flowdb_entry_id_hash_make(void* pst_key_void)
{
    adpt_flow_entry_id_hash_t* pst_key = NULL;

    pst_key = pst_key_void;
    return pst_key->entry_id;
}

static inline bool
adpt_flowdb_entry_id_hash_cmp (void* pv_node_a_void, void* pv_node_b_void)
{
    adpt_flow_entry_id_hash_t* pv_node_a = NULL;
    adpt_flow_entry_id_hash_t* pv_node_b = NULL;

    pv_node_a = pv_node_a_void;
    pv_node_b = pv_node_b_void;

    if (pv_node_a->entry_id != pv_node_b->entry_id)
    {
        return FALSE;
    }

    return TRUE;
}

static inline adpt_flow_entry_id_hash_t*
adpt_flowdb_entry_id_hash_lookup(ctc_hash_t *p_hash, uint32_ofp entry_id)
{
    adpt_flow_entry_id_hash_t lookup_key;
    
    lookup_key.entry_id = entry_id;
    
    return ctc_hash_lookup(p_hash, &lookup_key);
}

static inline int32_ofp
adpt_flowdb_entry_id_hash_add(ctc_hash_t *p_hash, adpt_flow_entry_id_hash_t* p_entry_id)
{
    ctc_hash_insert(p_hash, p_entry_id);

    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_entry_id_hash_del(ctc_hash_t *p_hash, adpt_flow_entry_id_hash_t* p_entry_id)
{
    ctc_hash_remove(p_hash, p_entry_id);

    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_entry_id_list_add(ctc_list_pointer_t *p_list, adpt_flow_entry_id_list_t* p_entry_id)
{
    /* no need insert the entry in order of entry id*/
    ctc_list_pointer_insert_tail(p_list, &p_entry_id->node);

    return OFP_ERR_SUCCESS;
}

static inline int32_ofp
adpt_flowdb_entry_id_list_del(ctc_list_pointer_t *p_list, adpt_flow_entry_id_list_t* p_entry_id)
{
    ctc_list_pointer_delete(p_list, &p_entry_id->node);

    return OFP_ERR_SUCCESS;
}

/**
 * Add flow priority and entry id into the database
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_add_flow_priority_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id)
{
    ctc_list_pointer_t* p_priority_list = NULL;
    ctc_hash_t*         p_priority_hash = NULL;
    adpt_flow_priority_list_t* p_priority = NULL;
    adpt_flow_entry_id_list_t* p_entry_id = NULL;

    ADPT_FLOWDB_SELECT_DB(OFP_ERR_INVALID_PARAM);

    /* 1. check if priority exists in p_priority_hash, if not create */
    p_priority = adpt_flowdb_priority_hash_lookup(p_priority_hash, priority);
    if (p_priority == NULL)
    {
        p_priority = malloc(sizeof(adpt_flow_priority_list_t));
        p_priority->priority = priority;
        
        ctc_list_pointer_init(&p_priority->entry_id_list);
        
        p_priority->entry_id_hash = ctc_hash_create(
            ADPT_FLOW_ENTRY_ID_HASH_BLOCK_NUM,
            ADPT_FLOW_ENTRY_ID_HASH_BLOCK_SIZE,
            adpt_flowdb_entry_id_hash_make,
            adpt_flowdb_entry_id_hash_cmp);

        adpt_flowdb_priority_hash_add(p_priority_hash, p_priority);
        adpt_flowdb_priority_list_add(p_priority_list, p_priority);
    }
    
    p_entry_id = malloc(sizeof(adpt_flow_entry_id_hash_t));
    p_entry_id->entry_id = entry_id;
    adpt_flowdb_entry_id_hash_add(p_priority->entry_id_hash, p_entry_id);
    adpt_flowdb_entry_id_list_add(&p_priority->entry_id_list, p_entry_id);

    return OFP_ERR_SUCCESS;
}

/**
 * Delete entry id from the database
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_del_flow_priority_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id)
{
    ctc_list_pointer_t* p_priority_list = NULL;
    ctc_hash_t*         p_priority_hash = NULL;
    adpt_flow_priority_list_t* p_priority = NULL;
    adpt_flow_entry_id_list_t* p_entry_id = NULL;

    ADPT_FLOWDB_SELECT_DB(OFP_ERR_INVALID_PARAM);

    /* 1. check if priority exists, if not return */
    p_priority = adpt_flowdb_priority_hash_lookup(p_priority_hash, priority);
    if (p_priority == NULL)
    {
        return OFP_ERR_SUCCESS;
    }
    
    p_entry_id = adpt_flowdb_entry_id_hash_lookup(p_priority->entry_id_hash, entry_id);
    if (p_entry_id)
    {
        adpt_flowdb_entry_id_hash_del(p_priority->entry_id_hash, p_entry_id);
        adpt_flowdb_entry_id_list_del(&p_priority->entry_id_list, p_entry_id);
        free(p_entry_id);
    }
    
    if (ctc_list_pointer_empty(&p_priority->entry_id_list))
    {
        adpt_flowdb_priority_hash_del(p_priority_hash, p_priority);
        adpt_flowdb_priority_list_del(p_priority_list, p_priority);
        ctc_hash_free(p_priority->entry_id_hash);
        free(p_priority);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get "after" entry_id by given priority and entry_id
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id
 * @param p_after_entry_id              pointer of prev entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_get_prev_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id, uint32_ofp* p_after_entry_id)
{
    ctc_list_pointer_t* p_priority_list = NULL;
    ctc_hash_t*         p_priority_hash = NULL;
    adpt_flow_priority_list_t* p_priority = NULL;
    adpt_flow_entry_id_list_t* p_entry_id = NULL;
    ctc_list_pointer_node_t*   p_priority_node = NULL;
    ctc_list_pointer_node_t*   p_entry_id_node = NULL;
    
    ADPT_FLOWDB_SELECT_DB(OFP_ERR_INVALID_PARAM);

    *p_after_entry_id = 0;
    
    p_priority = adpt_flowdb_priority_hash_lookup(p_priority_hash, priority);
    ADPT_PTR_CHECK(p_priority);
    
    p_entry_id = adpt_flowdb_entry_id_hash_lookup(p_priority->entry_id_hash, entry_id);
    ADPT_PTR_CHECK(p_entry_id);
    
    p_entry_id_node = ctc_list_pointer_prev(&p_entry_id->node);
    p_priority_node = &p_priority->node;
    while(p_priority_node)
    {
        if (p_entry_id_node)
        {
            p_entry_id = _ctc_container_of(p_entry_id_node, adpt_flow_entry_id_list_t, node);
            *p_after_entry_id = p_entry_id->entry_id;
            break;
        }

        p_priority_node = ctc_list_pointer_prev(p_priority_node);
        if (p_priority_node)
        {
            p_priority = _ctc_container_of(p_priority_node, adpt_flow_priority_list_t, node);
            p_entry_id_node = ctc_list_pointer_node_tail(&p_priority->entry_id_list);
        }
    }
    
    return OFP_ERR_SUCCESS;
}

void
adpt_flowdb_show_priority_db(ofp_flow_type_t flow_type)
{
    ctc_list_pointer_t* p_priority_list = NULL;
    ctc_hash_t*         p_priority_hash = NULL;

    ctc_list_pointer_node_t* p_priority_node;
    adpt_flow_priority_list_t* p_priority;

    ctc_list_pointer_node_t* p_entry_id_node;
    adpt_flow_entry_id_list_t* p_entry_id;

    if (flow_type == FLOW_TYPE_ANY)
    {
        ctc_cli_out_ofp("Print Priority DB of MAC entry\n");
    }
    else
    {
        ctc_cli_out_ofp("Print Priority DB of IPv4 entry\n");
    }
    
    ADPT_FLOWDB_SELECT_DB();

    ctc_cli_out_ofp("Priority   |  Value (QoS Entry ID)\n");
    ctc_cli_out_ofp(" ---------------------------------\n");

    CTC_LIST_POINTER_LOOP(p_priority_node, p_priority_list)
    {
        uint32_ofp entry_cnt = 0;
        
        p_priority = _ctc_container_of(p_priority_node, adpt_flow_priority_list_t, node);
        
        ctc_cli_out_ofp("%-10u | ", p_priority->priority);
        
        CTC_LIST_POINTER_LOOP(p_entry_id_node, &p_priority->entry_id_list)
        {
            p_entry_id = _ctc_container_of(p_entry_id_node, adpt_flow_entry_id_list_t, node);
            
            ctc_cli_out_ofp("%u ", p_entry_id->entry_id);
            if ((entry_cnt + 1) % 10 == 0)
            {
                ctc_cli_out_ofp("\n%13s", "");
            }
            entry_cnt ++;
        }
        ctc_cli_out_ofp("\n");
    }

    ctc_cli_out_ofp(" ---------------------------------\n");
}

/**
 * Adpt flow db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_db_init(void)
{
    int32_ofp idx;
    
    ADPT_MODULE_INIT_CHECK(g_p_adpt_flow_master);

    ADPT_ERROR_RETURN(adpt_flowdb_set_flow_entry_max_num(ENTRY_ID_MAX_SIZE));

    ihash_init(&g_p_adpt_flow_master->flow_info_ihmap);

    ctc_list_pointer_init(ADPT_FLOW_MAC_PRIORITY_LIST);
    ctc_list_pointer_init(ADPT_FLOW_IPV4_PRIORITY_LIST);
    ADPT_FLOW_MAC_PRIORITY_HASH = ctc_hash_create(
        ADPT_FLOW_PRIORITY_HASH_BLOCK_NUM,
        ADPT_FLOW_PRIORITY_HASH_BLOCK_SIZE,
        adpt_flowdb_priority_hash_make,
        adpt_flowdb_priority_hash_cmp);
    ADPT_FLOW_IPV4_PRIORITY_HASH = ctc_hash_create(
        ADPT_FLOW_PRIORITY_HASH_BLOCK_NUM,
        ADPT_FLOW_PRIORITY_HASH_BLOCK_SIZE,
        adpt_flowdb_priority_hash_make,
        adpt_flowdb_priority_hash_cmp);

    if (GLB_STM_DEFAULT == g_current_profile)
    {
        g_p_adpt_flow_master->output_port_max = MAX_OUTPUT_STATS;
    }
    else
    {
        g_p_adpt_flow_master->output_port_max = MAX_OUTPUT_STATS_HYBRID;
    }
    g_p_adpt_flow_master->gre_mpls_output_max = MAX_GRE_AND_MPLS_PUSH_OUTPUT_STATS;
    g_p_adpt_flow_master->mpls_output_max     = MAX_MPLS_OUTPUT_STATS;
    g_p_adpt_flow_master->queue_profile_max   = MAX_QUEUE_SHAPE_PROFILE;
    
    /* should be equal to OFP_ETHER_TYPE_MAX_NUM */
    ADPT_ERROR_RETURN(adpt_flowdb_set_ether_type_map_l3type_max_num(ADPT_FLOW_L3_TYPE_RSV_USER_DEFINE_NUM));
    ADPT_ERROR_RETURN(adpt_flowdb_set_ether_type_map_l3type_base(CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_BASE));
    for (idx = 0; idx < adpt_flowdb_get_ether_type_map_l3type_max_num(); idx++)
    {
        g_p_adpt_flow_master->ether_type_l3type_map[idx].l3type = adpt_flowdb_get_ether_type_map_l3type_base() + idx;
    }

    /* init QinQ db*/
    ADPT_ERROR_RETURN(adpt_flowdb_set_qinq_with_mac_max_num(ADPT_QINQ_WITH_MAC_MAX_NUM));

    return OFP_ERR_SUCCESS;
}

/**
 * Adpt flow db deinit
 * @return output max
 */
int32_ofp
adpt_flow_db_deinit(void)
{
    ctc_list_pointer_node_t* p_priority_node;
    ctc_list_pointer_node_t* p_priority_next_node;
    adpt_flow_priority_list_t* p_priority;

    ctc_list_pointer_node_t* p_entry_id_node;
    ctc_list_pointer_node_t* p_entry_id_next_node;
    adpt_flow_entry_id_list_t* p_entry_id;

    ADPT_MODULE_INIT_CHECK(g_p_adpt_flow_master);
    
    ihash_destroy_free_data(&g_p_adpt_flow_master->flow_info_ihmap);

    CTC_LIST_POINTER_LOOP_DEL(p_priority_node, p_priority_next_node, ADPT_FLOW_MAC_PRIORITY_LIST)
    {
        p_priority = _ctc_container_of(p_priority_node, adpt_flow_priority_list_t, node);
        CTC_LIST_POINTER_LOOP_DEL(p_entry_id_node, p_entry_id_next_node, &p_priority->entry_id_list)
        {
            p_entry_id = _ctc_container_of(p_entry_id_node, adpt_flow_entry_id_list_t, node);

            adpt_flowdb_entry_id_hash_del(p_priority->entry_id_hash, p_entry_id);
            adpt_flowdb_entry_id_list_del(&p_priority->entry_id_list, p_entry_id);
        }
        ctc_hash_free(p_priority->entry_id_hash);

        adpt_flowdb_priority_hash_del(ADPT_FLOW_MAC_PRIORITY_HASH, p_priority);
        adpt_flowdb_priority_list_del(ADPT_FLOW_MAC_PRIORITY_LIST, p_priority);
    }
    
    CTC_LIST_POINTER_LOOP_DEL(p_priority_node, p_priority_next_node, ADPT_FLOW_IPV4_PRIORITY_LIST)
    {
        p_priority = _ctc_container_of(p_priority_node, adpt_flow_priority_list_t, node);
        CTC_LIST_POINTER_LOOP_DEL(p_entry_id_node, p_entry_id_next_node , &p_priority->entry_id_list)
        {
            p_entry_id = _ctc_container_of(p_entry_id_node, adpt_flow_entry_id_list_t, node);

            adpt_flowdb_entry_id_hash_del(p_priority->entry_id_hash, p_entry_id);
            adpt_flowdb_entry_id_list_del(&p_priority->entry_id_list, p_entry_id);
        }
        ctc_hash_free(p_priority->entry_id_hash);

        adpt_flowdb_priority_hash_del(ADPT_FLOW_IPV4_PRIORITY_HASH, p_priority);
        adpt_flowdb_priority_list_del(ADPT_FLOW_IPV4_PRIORITY_LIST, p_priority);
    }
    ctc_hash_free(ADPT_FLOW_MAC_PRIORITY_HASH);
    ctc_hash_free(ADPT_FLOW_IPV4_PRIORITY_HASH);

    return OFP_ERR_SUCCESS;
}
