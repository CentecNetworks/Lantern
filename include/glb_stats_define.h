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

#ifndef __GLB_STATS_DEFINE_H__
#define __GLB_STATS_DEFINE_H__

#define GLB_STATS_RSLT_LEN(type)                                               \
    (type == GLB_STATS_TYPE_INTF ? sizeof(glb_if_stats_t) :                     \
    type == GLB_STATS_TYPE_PORT_QUEUE ? sizeof(glb_qos_queue_stats_t) :         \
    type == GLB_STATS_TYPE_PORT_POLICER ? sizeof(glb_qos_policer_stats_t) :     \
    type == GLB_STATS_TYPE_FLOW_POLICER ? sizeof(glb_qos_policer_stats_t) :     \
    sizeof(glb_stats_t))
    /* The newly added Stats's result length should be placed from here */

#define GLB_QOS_POLICER_STATS_SUPPORT_CONFIRM  (1<<0)
#define GLB_QOS_POLICER_STATS_SUPPORT_EXCEED   (1<<1)
#define GLB_QOS_POLICER_STATS_SUPPORT_VIOLATE  (1<<2)

#define GLB_QOS_QUEUE_STATS_SUPPORT_DEQUEUE  (1<<0)
#define GLB_QOS_QUEUE_STATS_SUPPORT_DROP     (1<<1)

#define GLB_STATS_CLEAR_KEY_MAX_NUM 20

#endif
