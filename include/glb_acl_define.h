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
 */

#ifndef __GLB_ACL_DEFINE_H__
#define __GLB_ACL_DEFINE_H__

#include "glb_flow_define.h"
#include "glb_cpu_traffic_define.h"

#define GLB_ACL_NAME_LEN            20

#define GLB_ACL_LOG_BIT_LEN         15

#define GLB_MAX_MSG_ACE_NUM         10

#define GLB_ACL_MAX_MSG_INTF_NUM    20

struct glb_acl_action_flag_s
{
    uint32 discard:1,
           deny_bridge:1,
           deny_learning:1,
           deny_route:1,
           deny_replace_cos:1,
           deny_replace_dscp:1,
           stats:1,
           redirect_if:1,
           redirect_cpu:1,
           log_cpu:1,
           invalid:1,
           priority:1,
           reserved:19;
};
typedef struct glb_acl_action_flag_s glb_acl_action_flag_t;
 
struct glb_acl_action_s
{
    glb_acl_action_flag_t flag;

    uint32 redirect_ifindex;

    uint8 log_id;
    uint8 log_weight;   /* 0 ~ 15, log_prob = 1/2^(15-log_weight) */
    uint8 priority;
    uint8 color;
};
typedef struct glb_acl_action_s glb_acl_action_t;


struct glb_ace_s
{
    glb_acl_action_t action;
    glb_flow_rule_t rule;
    uint32 seq_num;
    uint8  reserved[3];
};
typedef struct glb_ace_s glb_ace_t;


struct glb_acl_access_grp_s
{
    char  sz_name[GLB_ACL_NAME_LEN+4];

    uint8  acl_type;     /* GLB_FLOW_FILTER_XXX */
    uint8  ace_num;    
    uint8  reserved[2];
    
    glb_ace_t ace[GLB_MAX_MSG_ACE_NUM];
};
typedef struct glb_acl_access_grp_s glb_acl_access_grp_t;

struct glb_acl_clear_stats_intf_s
{
    uint32 ifindex;
    uint8  dir;
    uint8  reserved[3];
};
typedef struct glb_acl_clear_stats_intf_s glb_acl_clear_stats_intf_t;

struct glb_acl_clear_stats_all_req_s
{
    uint8  acl_type;
    uint8  intf_num;
    uint8  reserved[2];
    glb_acl_clear_stats_intf_t if_list[GLB_ACL_MAX_MSG_INTF_NUM];
};
typedef struct glb_acl_clear_stats_all_req_s glb_acl_clear_stats_all_req_t;

#endif /* __GLB_ACL_DEFINE_H__ */
