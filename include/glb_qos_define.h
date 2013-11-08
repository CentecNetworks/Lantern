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

#ifndef __GLB_QOS_DEFINE_H__
#define __GLB_QOS_DEFINE_H__

#include "glb_const.h"
#include "glb_acl_define.h"
#include "glb_phy_define.h"
#include "glb_flow_define.h"
#include "glb_stats_define.h"

#define GLB_QOS_DOMAIN_NUM              8
#define GLB_QOS_PRIORITY_NUM            64
#define GLB_QOS_HIGHEST_PRIORITY        12
#define GLB_QOS_HIGHEST_COS             7
#define GLB_QOS_COLOR_NUM               4
#define GLB_QOS_COS_NUM                 8
#define GLB_QOS_DSCP_NUM                64
#define GLB_QOS_IP_PREC_NUM             8
#define GLB_QOS_EXP_NUM                 8
#define GLB_QOS_PRI_COLOR_TABLE_SIZE    (GLB_QOS_PRIORITY_NUM * GLB_QOS_COLOR_NUM)
#define GLB_QOS_INVALID_FLOWID          0xFF

#define GLB_QOS_PORT_QUEUE_NUM          16
#define GLB_QOS_MAX_PORT_QUEUE_NUM      16
#define GLB_QOS_ALL_QUEUE_ID            0xFF

#define GLB_QOS_PMAP_NAME_LEN           40
#define GLB_QOS_CMAP_NAME_LEN           40
#define GLB_QOS_AGP_NAME_LEN            40

#define GLB_QOS_DROP_PREC_NUM           4

#define GLB_QOS_QUEUE_WDRR_WEIGHT_MTU_DEF 1600

#define GLB_QOS_MAX_HSRV2AGT_FLOW_RULE_NUM    10
#define GLB_QOS_MAX_HSRV2AGT_MAP_TABLE_SIZE   128
#define GLB_QOS_MAX_HAGT2SRV_PEND_FLOW_NUM    20

#define GLB_QOS_DEFAULT_DROP_THRESH_1G  0xE0
#define GLB_QOS_DEFAULT_WRED_MIN_THRESH_1G 0x10
#define GLB_QOS_DEFAULT_WRED_MAX_THRESH_1G 0x1E0
#define GLB_QOS_DEFAULT_DROP_DELTA_1G   0x10
#define GLB_QOS_DEFAULT_WRED_MIN_THRESH_DELTA_1G   8
#define GLB_QOS_DEFAULT_DROP_THRESH_10G 0x240
#define GLB_QOS_DEFAULT_DROP_DELTA_10G  0x10

#define GLB_QOS_DEFAULT_MONITOR_LOG_WEIGHT 15

#define GLB_QOS_MAX_DROP_THRESHOLD 24574
#define GLB_QOS_MAX_WRED_THRESHOLD 24574

#define GLB_QOS_DEFAULT_CLASSID            0
#define GLB_QOS_QID_BIT_NUMBER          5
#define GLB_QOS_QID_BIT_MASK            0x1F
#define GLB_QOS_THRESHOLD_BIT_NUMBER    3
#define GLB_QOS_THRESHOLD_BIT_MASK      0x7

#define GLB_QOS_DEFAULT_COS             0
#define GLB_QOS_DEFAULT_DSCP            0
#define GLB_QOS_DEFAULT_EXP             0
#define GLB_QOS_DEFAULT_IP_PREC         0

#define GLB_QOS_INIFINITY_DRR_WEIGHT     0xFF
#define GLB_QOS_UNSET_DRR_WEIGHT     0xFE

#define GLB_QOS_DRR_WEIGHT_MAX           100000

#define GLB_QOS_MAX_DROP_PROBABILITY_VALUE  65535
#define GLB_QOS_MAX_DROP_PROBABILITY_BIT    16
#define GLB_QOS_DEFAULT_WRED_DROP_PROB 1024

#define GLB_QOS_DEFAULT_WRED_MIN_THRESH(speed_mode, drop_prec)   \
    (GLB_QOS_DEFAULT_WRED_MIN_THRESH_1G + \
        (drop_prec) * GLB_QOS_DEFAULT_WRED_MIN_THRESH_DELTA_1G)
#define GLB_QOS_DEFAULT_WRED_MAX_THRESH(speed_mode, drop_prec)   \
    (GLB_QOS_DEFAULT_WRED_MAX_THRESH_1G + \
        (drop_prec) * GLB_QOS_DEFAULT_DROP_DELTA_1G)

#define GLB_QOS_DEFAULT_WTD_THRESH(speed_mode, drop_prec) \
    (GLB_SPEED_10G == (speed_mode)) ? \
        (GLB_QOS_DEFAULT_DROP_THRESH_10G + (drop_prec) * GLB_QOS_DEFAULT_DROP_DELTA_10G) : \
        (GLB_QOS_DEFAULT_DROP_THRESH_1G + \
                (drop_prec) * GLB_QOS_DEFAULT_DROP_DELTA_1G)

#define GLB_QOS_DEFAULT_TH3_WRED_MIN_THRESH(speed_mode, th2_thresh)   \
    (((th2_thresh) + GLB_QOS_DEFAULT_WRED_MIN_THRESH_DELTA_1G) >= GLB_QOS_MAX_WRED_THRESHOLD ? \
     GLB_QOS_MAX_WRED_THRESHOLD : ((th2_thresh) + GLB_QOS_DEFAULT_WRED_MIN_THRESH_DELTA_1G))
#define GLB_QOS_DEFAULT_TH3_WRED_MAX_THRESH(speed_mode, th2_thresh)   \
    (((th2_thresh) + GLB_QOS_DEFAULT_DROP_DELTA_1G) >= GLB_QOS_MAX_WRED_THRESHOLD ? \
     GLB_QOS_MAX_WRED_THRESHOLD : ((th2_thresh) + GLB_QOS_DEFAULT_DROP_DELTA_1G))
#define GLB_QOS_DEFAULT_TH3_WTD_THRESH(speed_mode, th2_thresh)   \
    (GLB_SPEED_10G == (speed_mode)) \
        ? ((((th2_thresh) + GLB_QOS_DEFAULT_DROP_THRESH_10G) >=  GLB_QOS_MAX_WRED_THRESHOLD) ? \
           GLB_QOS_MAX_WRED_THRESHOLD : ((th2_thresh) + GLB_QOS_DEFAULT_DROP_THRESH_10G))\
        : ((((th2_thresh) + GLB_QOS_DEFAULT_DROP_DELTA_1G) >=  GLB_QOS_MAX_WRED_THRESHOLD) ? \
           GLB_QOS_MAX_WRED_THRESHOLD : ((th2_thresh) + GLB_QOS_DEFAULT_DROP_DELTA_1G))
#define GLB_QOS_DEFAULT_TH3_WRED_DROP_PROB  0

// when qos is disable, queue 0 of each channel should be 352, 368, 384(DEFAULT), 400(RESERVED)
#define GLB_NON_QOS_DEFAULT_DROP_THRESH  0x160
#define GLB_NON_QOS_DEFAULT_DROP_DELTA   0x10
#define GLB_NON_QOS_DEFAULT_WTD_THRESH(drop_prec) \
    (GLB_NON_QOS_DEFAULT_DROP_THRESH + (drop_prec) * GLB_NON_QOS_DEFAULT_DROP_DELTA)

enum glb_qos_phb_type_e
{
    GBL_QOS_PHB_CS1 = 0,
    GBL_QOS_PHB_DF,
    GBL_QOS_PHB_AF11,
    GBL_QOS_PHB_AF12,
    GBL_QOS_PHB_AF13,
    GBL_QOS_PHB_AF21,
    GBL_QOS_PHB_AF22,
    GBL_QOS_PHB_AF23,
    GBL_QOS_PHB_AF31,
    GBL_QOS_PHB_AF32,
    GBL_QOS_PHB_AF33,
    GBL_QOS_PHB_AF41,
    GBL_QOS_PHB_AF42,
    GBL_QOS_PHB_AF43,
    GBL_QOS_PHB_CS2,
    GBL_QOS_PHB_CS3,
    GBL_QOS_PHB_CS6,
    GBL_QOS_PHB_CS7,
    GBL_QOS_PHB_CS4,
    GBL_QOS_PHB_CS5,
    GBL_QOS_PHB_EF,
    GBL_QOS_PHB_MAX
};
typedef enum glb_qos_phb_type_e glb_qos_phb_type_t;

enum glb_qos_global_enable_flag_e
{
    GLB_QOS_GLOBAL_FLAG_GLB_EN             = 0x0001,
    GLB_QOS_GLOBAL_FLAG_POLICER_EN         = 0x0002,
    GLB_QOS_GLOBAL_FLAG_FLOW_FIRST         = 0x0004,
    GLB_QOS_GLOBAL_FLAG_QUEUE_STATS_EN     = 0x0008,
    GLB_QOS_GLOBAL_FLAG_POLICER_STATS_EN   = 0x0010,
    GLB_QOS_GLOBAL_FLAG_POLICER_IPG_EN     = 0x0020,
    GLB_QOS_GLOBAL_FLAG_SHAPING_IPG_EN     = 0x0040,
};

enum glb_qos_flow_action_flag_e
{
    GLB_QOS_FLOW_ACTION_FLAG_TRUST         = 0x0001,
    GLB_QOS_FLOW_ACTION_FLAG_PRIORITY      = 0x0002,
    GLB_QOS_FLOW_ACTION_FLAG_POLICER       = 0x0004,
    GLB_QOS_FLOW_ACTION_FLAG_STATS         = 0x0008,
    GLB_QOS_FLOW_ACTION_FLAG_REDIRECT      = 0x0010,
    GLB_QOS_FLOW_ACTION_FLAG_LOG           = 0x0020,
};

/* Priority-Color mapping table type */
enum glb_qos_pri_color_map_type_e
{
    GLB_QOS_PRI_COLOR_MAP_TYPE_COS = 0,
    GLB_QOS_PRI_COLOR_MAP_TYPE_DSCP,
    GLB_QOS_PRI_COLOR_MAP_TYPE_IP_PREC,
    GLB_QOS_PRI_COLOR_MAP_TYPE_EXP,
    GLB_QOS_PRI_COLOR_MAP_TYPE_QID_TID,/*Mapped queueId and ThresholdId*/

    GLB_QOS_PRI_COLOR_MAP_TYPE_MAX
};
typedef enum glb_qos_pri_color_map_type_e glb_qos_pri_color_map_type_t;


/* QoS port attribute type */
enum glb_qos_port_attr_type_e
{
    GLB_QOS_PORT_ATTR_TYPE_TRUST_STATE = 0,
    GLB_QOS_PORT_ATTR_TYPE_DEFAULT_COS,
    GLB_QOS_PORT_ATTR_TYPE_REPLACE_COS,
    GLB_QOS_PORT_ATTR_TYPE_REPLACE_DSCP,
    GLB_QOS_PORT_ATTR_TYPE_DOMAIN,

    GLB_QOS_PORT_ATTR_TYPE_MAX
};


/* QoS trust state */
enum glb_qos_trust_state_e
{
    GLB_QOS_TRUST_PORT,         /* Trust port default CoS */
    GLB_QOS_TRUST_OUTER,        /* Trust outer priority and color, for tunnel or loopback */
    GLB_QOS_TRUST_COS,          /* Trust packet CoS field */
    GLB_QOS_TRUST_DSCP,         /* Trust IP DSCP field */
    GLB_QOS_TRUST_IP_PREC,      /* Trust IP precedence field */
    GLB_QOS_TRUST_COS_INNER,    /* Trust packet inner CoS field */

    GLB_QOS_TRUST_MAX
};
typedef enum glb_qos_trust_state_e glb_qos_trust_state_t;


/* QoS color */
enum glb_qos_color_e
{
    GLB_QOS_COLOR_NONE,
    GLB_QOS_COLOR_RED,      /* Red color: the lowest drop precedence */
    GLB_QOS_COLOR_YELLOW,   /* Yellow color: the mild drop precedence */
    GLB_QOS_COLOR_GREEN,    /* Green color: the highest drop precedence */

    GLB_QOS_COLOR_MAX
};
typedef enum glb_qos_color_e glb_qos_color_t;

/* QoS threshold */
enum glb_qos_threshold_e
{
    GLB_QOS_THRESHOLD_0,    /* The lowest drop precedence threshold */
    GLB_QOS_THRESHOLD_1,    /* The mild drop precedence threshold */
    GLB_QOS_THRESHOLD_2,    /* The high drop precedence threshold */
    GLB_QOS_THRESHOLD_3,    /* The highest drop precedence threshold reserved for critical packet */

    GLB_QOS_THRESHOLD_MAX
};
typedef enum glb_qos_threshold_e glb_qos_threshold_t;

#define  GLB_QOS_DEFULT_ENABLE_SHAPING_QID 7
#define GLB_QOS_DEFAULT_QUEUE_SHAPE_PER    30

#define GLB_QOS_PORT_DEFAULT_COS          0
#define GLB_QOS_PORT_DEFAULT_DOMAIN       0
#define GLB_QOS_PORT_DEFAULT_TRUST        GLB_QOS_TRUST_COS

/* priority color */
struct glb_qos_pri_color_s
{
    uint8  pri;
    uint8  color;
    uint8  reserved[2];
};
typedef struct glb_qos_pri_color_s glb_qos_pri_color_t;


/* priority color mapping */
struct glb_qos_pri_color_map_s
{
    uint8  domain;
    uint8  value;/*For qid-tid: qid have higher 5 bits, tid have lower 3bits*/
    uint8  pri;
    uint8  color;
};
typedef struct glb_qos_pri_color_map_s glb_qos_pri_color_map_t;


/* QoS policer */
struct glb_qos_policer_s
{
    uint8  is_srtcm;
    uint8  is_color_blind;
    uint8  drop_color;
    uint8  use_l3_length;

    uint8  is_stats_en;
    uint8  resved[3];

    uint32 cir;
    uint32 cbs;
    uint32 pir;
    uint32 pbs;
};
typedef struct glb_qos_policer_s glb_qos_policer_t;

struct glb_qos_flow_action_flag_s
{
    uint32 policer:1,
           agp:1,
           priority:1,
           trust:1,
           stats:1,
           redirect:1,
           log:1,
           reserved:25;
};
typedef struct glb_qos_flow_action_flag_s glb_qos_flow_action_flag_t;

#define GLB_QOS_INVALID_NH_ID 0xffffffff

struct glb_qos_flow_action_s
{
    glb_qos_flow_action_flag_t flag;

    char  sz_agp_name[GLB_QOS_AGP_NAME_LEN+4];

    glb_qos_policer_t policer;

    uint8  priority;
    uint8  color;
    uint8  trust;
    uint8  log_id;

    uint32 log_weight;
    uint32 redirect_ifindex;
};
typedef struct glb_qos_flow_action_s glb_qos_flow_action_t;


struct glb_qos_flow_action_update_param_s
{
    uint32 op_action;     /* GLB_QOS_FLOW_ACTION_OP_XXX */
    char  sz_agp_name[GLB_QOS_AGP_NAME_LEN+4];

    glb_qos_policer_t policer;

    uint8  priority;
    uint8  color;
    uint8  trust;
    uint8  log_id;
    uint32 log_weight;
    uint32 redirect_ifindex;
};
typedef struct glb_qos_flow_action_update_param_s glb_qos_flow_action_update_param_t;


struct glb_qos_flow_rule_s
{
    uint32 seq_num;
    glb_flow_rule_t rule;
};
typedef struct glb_qos_flow_rule_s glb_qos_flow_rule_t;


struct glb_qos_flow_rule_group_s
{
    char  sz_group_name[GLB_ACL_NAME_LEN+4];  /* ACL name */

    uint8  group_type;     /* acl type: GLB_FLOW_RULE_TYPE_XXX */
    uint8  rule_num;
    uint8  has_ext_mac_rule;
    uint8  reserved;

    glb_qos_flow_rule_t rule_group[GLB_QOS_MAX_HSRV2AGT_FLOW_RULE_NUM];
    glb_flow_mac_rule_t ext_mac_rule;     /* mac key part: used for match-all class-map when
                                             mac and ip key are required to merge together */
};
typedef struct glb_qos_flow_rule_group_s glb_qos_flow_rule_group_t;


enum glb_qos_flow_action_op_code_e
{
    GLB_QOS_FLOW_ACTION_OP_SET_PRI_COLOR = 0,
    GLB_QOS_FLOW_ACTION_OP_UNSET_PRI_COLOR,
    GLB_QOS_FLOW_ACTION_OP_SET_TRUST,
    GLB_QOS_FLOW_ACTION_OP_UNSET_TRUST,
    GLB_QOS_FLOW_ACTION_OP_ADD_IND_POLICER,
    GLB_QOS_FLOW_ACTION_OP_DEL_IND_POLICER,
    GLB_QOS_FLOW_ACTION_OP_MOD_IND_POLICER,
    GLB_QOS_FLOW_ACTION_OP_ADD_AGG_POLICER,
    GLB_QOS_FLOW_ACTION_OP_DEL_AGG_POLICER,
    GLB_QOS_FLOW_ACTION_OP_SET_LOG,
    GLB_QOS_FLOW_ACTION_OP_UNSET_LOG,
    GLB_QOS_FLOW_ACTION_OP_ENABLE_STATS,
    GLB_QOS_FLOW_ACTION_OP_DISABLE_STATS,
    GLB_QOS_FLOW_ACTION_OP_SET_REDIRECT,
    GLB_QOS_FLOW_ACTION_OP_UNSET_REDIRECT,

    GLB_QOS_FLOW_ACTION_OP_MAX
};

enum glb_qos_flow_op_code_e
{
    GLB_QOS_FLOW_OP_ADD_FLOW_RULE = 0,
    GLB_QOS_FLOW_OP_DEL_FLOW_RULE,
    GLB_QOS_FLOW_OP_VALID_FLOW_RULE,
    GLB_QOS_FLOW_OP_INVALID_FLOW_RULE,
    GLB_QOS_FLOW_OP_ADD_RULE_GROUP,
    GLB_QOS_FLOW_OP_DEL_RULE_GROUP,
    GLB_QOS_FLOW_OP_ADD_FLOW_ENTRY,
    GLB_QOS_FLOW_OP_DEL_FLOW_ENTRY,
    GLB_QOS_FLOW_OP_MOD_FLOW_ACTION,

    GLB_QOS_FLOW_OP_MAX
};


struct glb_qos_flow_update_param_s
{
    uint32 ifindex;

    uint8  dir;
    uint8  op_flow;     /* GLB_QOS_FLOW_TABLE_OP_XXX */
    uint8  op_action;   /* GLB_QOS_FLOW_ACTION_OP_XXX, used for
                           op_code = GLB_QOS_FLOW_OP_MOD_FLOW_ACTION */
    uint8  reserved;

    char  sz_flow_name[GLB_QOS_CMAP_NAME_LEN+4];
    char  sz_group_name[GLB_ACL_NAME_LEN+4];

    uint32 seq_num;
};
typedef struct glb_qos_flow_update_param_s glb_qos_flow_update_param_t;



/* queue shape parameters */
struct glb_qos_queue_shape_s
{
    uint32 cir;
    uint32 cbs;
    uint32 pir;
    uint32 pbs;
};
typedef struct glb_qos_queue_shape_s glb_qos_queue_shape_t;


/* port shape parameters */
struct glb_qos_port_shape_s
{
    uint32 pir;
    uint32 pbs;
};
typedef struct glb_qos_port_shape_s glb_qos_port_shape_t;
typedef struct glb_qos_port_shape_s glb_qos_group_shape_t;


enum glb_qos_queue_drop_mode_e
{
    GLB_QOS_QUEUE_DROP_MODE_WTD = 0,
    GLB_QOS_QUEUE_DROP_MODE_WRED,

    GLB_QOS_QUEUE_DROP_MODE_MAX
};


/* queue drop scheme & parameters */
struct glb_qos_queue_drop_s
{
    uint8  mode;                         /* GLB_QOS_QUEUE_DROP_MODE_XXX */
    uint8  weight;                       /* WRED weight */
    uint8  reserved[2];
    uint16 min_th[GLB_QOS_DROP_PREC_NUM];    /* WRED min threshold */
    uint16 max_th[GLB_QOS_DROP_PREC_NUM];    /* WRED max threshold, or WTD drop threshold */
    uint16 drop_prob[GLB_QOS_DROP_PREC_NUM]; /* WRED max drop probability */
};
typedef struct glb_qos_queue_drop_s glb_qos_queue_drop_t;

struct glb_qos_port_init_cfg_s
{
    uint32 ifindex;

    uint8  attr_value[GLB_QOS_PORT_ATTR_TYPE_MAX];
    uint8  reserved[2];

    uint32 wdrr_weight;
};
typedef struct glb_qos_port_init_cfg_s glb_qos_port_init_cfg_t;


enum glb_qos_port_cfg_flag_e
{
    GLB_QOS_PORT_CFG_FLAG_DOMAIN           = 1 << 0,
    GLB_QOS_PORT_CFG_FLAG_TRUST            = 1 << 1,
    GLB_QOS_PORT_CFG_FLAG_COS              = 1 << 2,
    GLB_QOS_PORT_CFG_FLAG_REPLACE_COS      = 1 << 3,
    GLB_QOS_PORT_CFG_FLAG_REPLACE_DSCP     = 1 << 4,
    GLB_QOS_PORT_CFG_FLAG_IGS_PORT_POLICER = 1 << 5,
    GLB_QOS_PORT_CFG_FLAG_EGS_PORT_POLICER = 1 << 6,
    GLB_QOS_PORT_CFG_FLAG_PORT_SHAPE       = 1 << 7,
    GLB_QOS_PORT_CFG_FLAG_QUEUE_DROP       = 1 << 8,
    GLB_QOS_PORT_CFG_FLAG_QUEUE_CLASS      = 1 << 9,
    GLB_QOS_PORT_CFG_FLAG_WDRR_WEIGHT      = 1 << 10,
    GLB_QOS_PORT_CFG_FLAG_QUEUE_SHAPE      = 1 << 11
};


struct glb_qos_port_deinit_cfg_s
{
    uint32 ifindex;

    uint32 flag;    /* GLB_QOS_PORT_CFG_XXX */

    uint8  domain;
    uint8  trust_state;
    uint8  default_cos;

    uint32 wdrr_weight;
    uint32 queue_wdrr_weight_bmp;

    uint32 queue_class_bmp;
    uint32 queue_classid;/*each queue have 2 bits classid*/
    glb_qos_queue_drop_t queue_drop;
    uint32 queue_drop_bmp;

    glb_qos_port_shape_t port_shape;
    uint32 queue_shape_bmp;
};
typedef struct glb_qos_port_deinit_cfg_s glb_qos_port_deinit_cfg_t;

enum glb_qos_pend_type_e
{
    GLB_QOS_PEND_TYPE_ADD_FAIL       = 1,
    GLB_QOS_PEND_TYPE_SET_STATS_FAIL = 2,
    GLB_QOS_PEND_TYPE_SET_PLC_FAIL   = 3
    /* other pend type */
};
typedef enum glb_qos_pend_type_e glb_qos_pend_type_t;

#endif /*!__GLB_QOS_DEFINE_H__*/
