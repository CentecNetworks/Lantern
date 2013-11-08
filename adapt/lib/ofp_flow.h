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
 * @brief This file defines data structure for adapter layer flow
 */

#ifndef __OFP_FLOW_H__
#define __OFP_FLOW_H__

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

/****************************************************************************
 *
 * Structures and macros, enums
 *
 ****************************************************************************/
#define SPECIAL_STATS_PTR            0xFFFF

#ifndef MAX_OUTPUT_PORT
#define MAX_OUTPUT_PORT 64
#endif 
#define MAX_OUTPUT_STATS (15*1024)
#define MAX_OUTPUT_STATS_HYBRID (9*1024)
#define MAX_GRE_AND_MPLS_PUSH_OUTPUT_STATS (4*1024)
#define MAX_MPLS_OUTPUT_STATS (5*1024)
#define MAX_QUEUE_SHAPE_PROFILE  254

#define ENTRY_ID_MAX_SIZE 2560
#define ENTRY_ID_MAC_DEFAULT  (0xFFFFFFFF - 1)
#define ENTRY_ID_IPV4_DEFAULT (0xFFFFFFFF - 2)
#define ENTRY_ID_GRE_DEFAULT  (0xFFFFFFFF - 3)

#define MAC_START_ENTRY_ID  1
#define IPV4_START_ENTRY_ID 10001
#define MPLS_START_ENTRY_ID 20001

#define MAX_CPU_TRAFFIC_RATE 640000
#define ENTRY_ID_START_OFFSET 1
#define NEXTHOP_ID_START_OFFSET 10
#define NEXTHOP_ID_MAX_SIZE 32768
#define NEXTHOP_START_OFFSET 5
#define NEXTHOP_MAX_SIZE 18432
#define GLB_MET_START_OFFSET 3
#define GLB_MET_MAX_SIZE 6912

#define OFP_IDLE_TIMEOUT_TIMER 5000
#define OFP_ETHER_TYPE_MAX_NUM 10

#define OFP_BUCKET_NUM_PER_GROUP        16
#define OFP_BUCKET_NUM_SELECT_GROUP     1
#define OFP_GROUP_NUM_TYPE_ALL          63
#define OFP_GROUP_NUM_TYPE_SELECT       63
#define OFP_GROUP_NUM_TYPE_INDIRECT     63
#define OFP_GROUP_NUM_TYPE_FF           63

#define OFP_GROUP_INVALID_GROUP_ID 0xffffffff

/* Flow id should be smaller than size of nexthop_ptr, 2^20 */
enum ofp_flow_id_offset_e
{
    FLOW_ID_DEFAULT_FLOW = 1,       /**< default flow id */
    FLOW_ID_GROUP_FLOW = 2,          /**< group flow id */
    FLOW_ID_MNGT_DFT_FLOW = 3,   /**< flow id of flows for management */

    FLOW_ID_START_OFFSET = 10       /**< start offset id */
};

enum ofp_flow_type_e
{
    FLOW_TYPE_MAC, /**< MAC flow rule type */
    FLOW_TYPE_IPV4, /**< IPv4 flow rule type */
    FLOW_TYPE_IPV6, /**< IPv6 flow filter type */
    FLOW_TYPE_EXIP, /**< Extended IP flow rule type */
    FLOW_TYPE_EXIPV6, /**< Extended IPv6 flow rule type */
    FLOW_TYPE_MPLS, /**< MPLS flow rule type */
    FLOW_TYPE_ANY, /**< Not specified flow rule type */
    FLOW_TYPE_OTHER, /**< Other ether type */

    FLOW_TYPE_MAX /**< Type Max */
};
typedef enum ofp_flow_type_e ofp_flow_type_t;

enum ofp_flow_type_port_based_e
{
    FLOW_TYPE_PORT_BASED_PER_PORT, /**< Per-Port flow rule */
    FLOW_TYPE_PORT_BASED_GLOBAL, /**< Global flow rule */

    FLOW_TYPE_PORT_BASED_MAX /**< Flow type Max */
};
typedef enum ofp_flow_type_port_based_e ofp_flow_type_port_based_t;

/* XXX: UINT16_MAX is from match_set_in_port in match.c, although
 * the type of in_port is unit32. */
#define OFP_FLOW_INPORT_BASED(rule) \
    ( ((rule)->match.wc.masks.in_port == UINT16_MAX ) ? \
        FLOW_TYPE_PORT_BASED_PER_PORT : FLOW_TYPE_PORT_BASED_GLOBAL )

#ifndef MAX_OUTPUT_PORT
#define MAX_OUTPUT_PORT 64
#endif 

enum ofp_nh_info_type_e
{
    NH_INFO_TYPE_NH_ID, /**< next-hop id */
    NH_INFO_TYPE_GLB_MET_OFFSET, /**< global met offset */
    NH_INFO_TYPE_GLB_NH_OFFSET, /**< global next-hop offset */
    NH_INFO_TYPE_GLB_NH_OFFSET_8W,
    NH_INFO_TYPE_MAX /**< Type Max */
};
typedef enum ofp_nh_info_type_e ofp_nh_info_type_t;

enum ofp_nh_type_e
{
    OPF_NH_TYPE_NH_NONE,
    OPF_NH_TYPE_NH_IPUC, 
    OPF_NH_TYPE_NH_MPLS,
    OPF_NH_TYPE_NH_MPLS_VPWS,
    OPF_NH_TYPE_NH_MPLS_VPLS,
    OPF_NH_TYPE_NH_FLEX,
    OPF_NH_TYPE_NH_FLEX_QINQ,
    OPF_NH_TYPE_NH_FLEX_IPDA,
    OPF_NH_TYPE_NH_FLEX_USER_DEFINED_OFFSET,
    OPF_NH_TYPE_MAX /**< Type Max */
};
typedef enum ofp_nh_type_e ofp_nh_type_t;

#define OFP_NH_TYPE_STR(type)                      \
    (OPF_NH_TYPE_NH_NONE == (type)      ? "NONE" : \
     OPF_NH_TYPE_NH_IPUC == (type)      ? "IPUC" : \
     OPF_NH_TYPE_NH_MPLS == (type)      ? "MPLS" : \
     OPF_NH_TYPE_NH_MPLS_VPWS == (type) ? "VPWS" : \
     OPF_NH_TYPE_NH_MPLS_VPLS == (type) ? "VPLS" : \
     OPF_NH_TYPE_NH_FLEX == (type)      ? "FLEX" : \
     OPF_NH_TYPE_NH_FLEX_QINQ == (type) ? "QINQ" : \
     OPF_NH_TYPE_NH_FLEX_IPDA == (type) ? "IPDA" : \
     OPF_NH_TYPE_NH_FLEX_USER_DEFINED_OFFSET == (type)      ? "USER_DEFINED OFFSET" : \
     "UNKW")

enum ofp_action_type_e
{
    OPF_ACTION_TYPE_NORMAL, 
    OPF_ACTION_TYPE_MPLS_VPWS,
    OPF_ACTION_TYPE_MPLS_VPLS,
    OPF_ACTION_TYPE_MPLS_NORMAL,
    OPF_ACTION_TYPE_MPLS_POP,
    OPF_ACTION_TYPE_MPLS_SWAP,
    OPF_ACTION_TYPE_MPLS_L3VPN,
    OPF_ACTION_TYPE_MAX /**< Type Max */
};
typedef enum ofp_action_type_e ofp_action_type_t;

/**
  *  @brief flow process type
  */
enum ofp_flow_process_type_e
{
    OFP_FLOW_PROCESS_TYPE_MATCH_TABLE_AND_FORWARD,    /**< normal match table and forward */
    OFP_FLOW_PROCESS_TYPE_PACKET_OUT,                 /**< packet out */

    OFP_FLOW_PROCESS_TYPE_MAX                         /**< process type max */
};
typedef enum ofp_flow_process_type_e ofp_flow_process_type_t;


#define OFP_ACT_TYPE_STR(type)                          \
    (OPF_ACTION_TYPE_NORMAL == (type)      ? "NORMAL" : \
     OPF_ACTION_TYPE_MPLS_VPWS == (type)   ? "VPWS"   : \
     OPF_ACTION_TYPE_MPLS_VPLS == (type)   ? "VPLS"   : \
     OPF_ACTION_TYPE_MPLS_NORMAL == (type) ? "MPLS"   : \
     OPF_ACTION_TYPE_MPLS_POP == (type)    ? "POP"    : \
     OPF_ACTION_TYPE_MPLS_SWAP == (type)   ? "SWAP"   : \
     OPF_ACTION_TYPE_MPLS_L3VPN == (type)  ? "L3VPN"  : \
     "UNKW")

struct ofp_nh_offset_s
{
    uint32_t nhid;
    /*for reserved nh id, offset and nh_type will not be used*/
    uint32_t offset;
    ofp_nh_type_t nh_type;
    
    bool port_check_discard; /* If set 1, discard the packet if it's destination port is same with ingress port */
};
typedef struct ofp_nh_offset_s ofp_nh_offset_t;

struct ofp_nexthop_info_s
{
    bool use_mcast;
    ofp_nh_offset_t main_nh;
    ofp_nh_offset_t member_nh[MAX_OUTPUT_PORT];
    uint32_t output_count;
    uint32_t gre_and_mpls_push_output_count;    /* and write ipda */
    uint32_t mpls_output_count;
};
typedef struct ofp_nexthop_info_s ofp_nexthop_info_t;

struct ofp_group_info_s
{
    uint32_t group_nhid;
    uint32_t group_id;
    bool     is_group_bound;
};
typedef struct ofp_group_info_s ofp_group_info_t;

struct ofp_meter_info_s
{
    uint32_t meter_id;
    bool is_meter_bound;
};
typedef struct ofp_meter_info_s ofp_meter_info_t;

#endif /* !__OFP_FLOW_H__ */
