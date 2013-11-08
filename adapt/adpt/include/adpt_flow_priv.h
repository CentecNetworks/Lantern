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
 * @brief This file is the private file for adpt_flow.c
 */

#ifndef __ADPT_FLOW_PRIV_H__
#define __ADPT_FLOW_PRIV_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ihash.h"
#include "ivec.h"
#include "ctc_parser.h"
#include "ctc_hash.h"
#include "ctc_linklist.h"
#include "afx.h"

/*******************************************************************
 *
 * Structures and macros, enums
 *
 ********************************************************************/

#define ADPT_FLOW_L3_TYPE_RSV_USER_DEFINE_NUM OFP_ETHER_TYPE_MAX_NUM

#define OFP_ETHER_TYPE_L3TYPE_MAP_IDX_MAX 16

#define ADPT_QINQ_WITH_MAC_MAX_NUM 2560

#define ADPT_FLOW_MASTER g_p_adpt_flow_master

/**
 @brief ether_type - l3type map
*/
struct adpt_ether_type_map_l3type_s
{
    uint16_ofp ether_type;              /**< ether type */
    uint16_ofp ether_type_mask;         /**< ether type mask */

    uint16_ofp ref;                     /**< reference count */
    uint8_ofp  l3type;                  /**< layer 3 type */
    uint8_ofp  resv;                    /**< not used */
};
typedef struct adpt_ether_type_map_l3type_s adpt_ether_type_map_l3type_t;

/**
 @brief adapter layer action combo
*/
struct adpt_flow_info_s
{
    int64_ofp last_matched;             /**< Last matched timestamp */
    uint64_ofp packet_count;            /**< packet count */
    uint16_ofp idle_timeout;            /**< idle timeout */
    uint16_ofp rsv;                     /**< reserved */
    struct rule_ctc* p_rule;            /**< rule struct */
    bool is_idle_timer;                 /**< if this entry records idle_timeout */
    bool need_delete;                   /**< flag to delete */
};
typedef struct adpt_flow_info_s adpt_flow_info_t;

struct adpt_flow_entry_id_list_s
{
    ctc_list_pointer_node_t node;

    uint32_ofp entry_id;
};
typedef struct adpt_flow_entry_id_list_s adpt_flow_entry_id_list_t;
typedef struct adpt_flow_entry_id_list_s adpt_flow_entry_id_hash_t;

struct adpt_flow_priority_list_s
{
    ctc_list_pointer_node_t node;

    uint32_ofp priority;

    ctc_list_pointer_t entry_id_list;   /**< adpt_flow_entry_id_list_t */
    ctc_hash_t*        entry_id_hash;   /**< adpt_flow_entry_id_hash_t */
};
typedef struct adpt_flow_priority_list_s adpt_flow_priority_list_t;
typedef struct adpt_flow_priority_list_s adpt_flow_priority_hash_t;

/**
 @brief adapter layer flow master data structure
*/
struct adpt_flow_master_s
{
    afx_timer_t *idle_timeout_timer;

    /* ether_type l3type map */
    uint8_ofp ether_type_l3type_map_max_num;
    uint8_ofp ether_type_l3type_map_base_l3type;
    adpt_ether_type_map_l3type_t ether_type_l3type_map[OFP_ETHER_TYPE_L3TYPE_MAP_IDX_MAX];

    /* flow entry count */
    uint32_ofp flow_entry_num_count;
    uint32_ofp flow_entry_num_max;

    uint32_ofp output_port_count;
    uint32_ofp output_port_max;
    
    uint32_ofp gre_mpls_output_count;
    uint32_ofp gre_mpls_output_max;
    
    uint32_ofp mpls_output_count;
    uint32_ofp mpls_output_max;

    uint32_ofp qinq_with_mac_cur;
    uint32_ofp qinq_with_mac_max;

    uint32_ofp queue_profile_max;

    ctc_list_pointer_t mac_flow_priority_list;      /**< adpt_flow_priority_list_t */
    ctc_hash_t*        mac_flow_priority_hash;      /**< adpt_flow_priority_hash_t */
    ctc_list_pointer_t ipv4_flow_priority_list;     /**< adpt_flow_priority_list_t */
    ctc_hash_t*        ipv4_flow_priority_hash;     /**< adpt_flow_priority_hash_t */

    struct ihash flow_info_ihmap;

    uint64_ofp removed_flow_stats_pkt;
    uint64_ofp removed_flow_stats_bytes;
};
typedef struct adpt_flow_master_s adpt_flow_master_t;

#define ADPT_FLOW_MAC_PRIORITY_HASH  (g_p_adpt_flow_master->mac_flow_priority_hash)
#define ADPT_FLOW_MAC_PRIORITY_LIST  (&g_p_adpt_flow_master->mac_flow_priority_list)
#define ADPT_FLOW_IPV4_PRIORITY_HASH (g_p_adpt_flow_master->ipv4_flow_priority_hash)
#define ADPT_FLOW_IPV4_PRIORITY_LIST (&g_p_adpt_flow_master->ipv4_flow_priority_list)

#define ADPT_FLOW_PRIORITY_HASH_BLOCK_NUM 10
#define ADPT_FLOW_PRIORITY_HASH_BLOCK_SIZE 256

#define ADPT_FLOW_ENTRY_ID_HASH_BLOCK_NUM 10
#define ADPT_FLOW_ENTRY_ID_HASH_BLOCK_SIZE 256

extern adpt_flow_master_t* g_p_adpt_flow_master;

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
/**
 * Add flow priority and entry id into the database
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_add_flow_priority_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id);

/**
 * Delete entry id from the database
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_del_flow_priority_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id);

/**
 * Get "after" entry_id by given priority and entry_id
 * @param flow_type                     flow type, FLOW_TYPE_*
 * @param priority                      priority
 * @param entry_id                      entry id
 * @param p_after_entry_id              pointer of prev entry id in ctc_sdk acl/qos
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_get_prev_entry_id(ofp_flow_type_t flow_type, uint32_ofp priority, uint32_ofp entry_id, uint32_ofp * p_after_entry_id);

/**
 * Add flow information
 * @param flow_id                       flow id
 * @param flow_info                     flow information
 */
void
adpt_flowdb_add_flow_info(uint32_ofp flow_id, adpt_flow_info_t * flow_info);

/**
 * Delete flow information
 * @param flow_id                       flow id
 */
void
adpt_flowdb_del_flow_info(uint32_ofp flow_id);

/**
 * Get flow information
 * @param flow_id                       flow id
 * @return flow                         information
 */
adpt_flow_info_t *
adpt_flowdb_get_flow_info(uint32_ofp flow_id);

/**
 * Get flow info ihmap
 * @return flow info ihmap
 */
struct ihash *
adpt_flowdb_get_flow_info_ihmap(void);

/**
 * Get max num of ether type - l3 type map 
 * @return max num
 */
int32_ofp
adpt_flowdb_get_ether_type_map_l3type_max_num(void);

/**
 * Get l3type base of ether type - l3 type map 
 * @return l3type base
 */
int32_ofp
adpt_flowdb_get_ether_type_map_l3type_base(void);

/**
 * Add ether type - l3 type map 
 * @param[in] ether_type                ether_type
 * @param[in] ether_type_mask           ether_type_mask
 * @param[out] p_index                  pointer to index
 * @param[out] p_l3type                 pointer to l3type
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_insert_map_ether_type_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp* p_index, ctc_parser_l3_type_t* p_l3type);

/**
 * Remove ether type - l3 type map 
 * @param[in] ether_type                ether_type
 * @param[in] ether_type_mask           ether_type_mask
 * @param[out] p_index                  pointer to index
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_remove_map_ether_type_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp* p_index);

/**
 * Show ether type - l3 type map 
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_show_ether_type_l3type_map(void);

/**
 * Set flow entry max number
 * @param[in] max                       flow entry max number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_set_flow_entry_max_num(int32_ofp max);

/**
 * Get flow entry max number
 * @return max flow entry number
 */
int32_ofp
adpt_flowdb_get_flow_entry_max_num(void);

/**
 * Increase flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_incr_flow_entry_num(void);

/**
 * Decrease flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_decr_flow_entry_num(void);

/**
 * show flow entry number
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flowdb_show_flow_entry_num(void);

/**
 * Get gre and mpls push output count
 * @return output count
 */
uint32_ofp
adpt_flowdb_get_gre_and_mpls_push_output_count(void);

/**
 * Get gre and mpls push output max
 * @return output max
 */
uint32_ofp
adpt_flowdb_get_gre_and_mpls_push_output_max(void);

/**
 * Adpt flow db init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_db_init(void);

/**
 * Adpt flow db deinit
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_db_deinit(void);

#endif
