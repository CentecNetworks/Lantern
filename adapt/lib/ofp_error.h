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
 * @brief This file defines error codes used in adapter layer
 */

#ifndef __OFP_ERROR_H__
#define __OFP_ERROR_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

enum ofp_error_e
{
    OFP_ERR_SUCCESS                     =  0,      /**< success */

    OFP_ERR_FAIL                        = -1,      /**< fail */
    OFP_ERR_ALL_TABLES_FULL             = -2,      /**< table full */
    OFP_ERR_ETHER_TYPE_FULL             = -3,      /**< map ether-type to l3type fail */
    OFP_ERR_INVALID_PTR                 = -4,      /**< invalid pointer */
    OFP_ERR_UNSUPPORTED_ACTIONS         = -5,      /**< unsupported actions */

    OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE= -6,      /**< unsupported flow field value */
    OFP_ERR_INVALID_PARAM               = -7,      /**< invalid param */
    OFP_ERR_ENTRY_EXIST                 = -8,      /**< entry exist */
    OFP_ERR_ENTRY_NOT_EXIST             = -9,     /**< entry not exist */
    OFP_ERR_NO_MEMORY                   = -10,     /**< no memory */

    OFP_ERR_TUNNEL_ID_FULL              = -11,     /**< no label for tunnel id */
    OFP_ERR_NETDEV_EXIST                = -12,     /**< net device exist */
    OFP_ERR_NETDEV_NOT_EXIST            = -13,     /**< net device not exist */
    OFP_ERR_NOT_INIT                    = -14,     /**< not init */
    OFP_ERR_TUNNEL_PORT_FULL            = -15,

    OFP_ERR_INVALID_IN_PORT_NUMBER      = -16,     /**< invalid in_port port number */
    OFP_ERR_TUNNEL_INVALID_BIND_NAME    = -17,     /**< invalid bind port name for tunnel */
    OFP_ERR_TUNNEL_REPEAT_LOCAL_REMOTE_IP   = -18, /**< reduplicate local remote ip combination */
    OFP_ERR_TUNNEL_LOCAL_IP_FULL        = -19,     /**< local ip used up for tunnel */
    OFP_ERR_BAD_OUT_PORT                = -20,     /**< invalid output port */
    
    OFP_ERR_ACTION_TOO_MANY_STRIP_VLAN  = -21,    /**< too many strip vlan */
    OFP_ERR_INVALID_ACTION_LIST         = -22,    /**< invalid mpls action list*/
    OFP_ERR_ACTION_TOO_MANY_PUSH_VLAN   = -23,    /**< too many push vlan */
    OFP_ERR_ACTION_VLAN_PUSH_ONLY_TPID  = -24,    /**< can only push vlan with specified tpid */
    OFP_ERR_ACTION_VLAN_PUSH_AFT_STRIP  = -25,    /**< can not push vlan after strip vlan */

    OFP_ERR_ACTION_VLAN_PUSH_AFT_MOD    = -26,    /**< can not push vlan after modify vlan */
    OFP_ERR_ACTION_VLAN_STRIP_AFT_PUSH  = -27,    /**< can not strip vlan after push vlan */
    OFP_ERR_ACTION_VLAN_STRIP_AFT_MOD   = -28,    /**< no use */
    OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP_TWO   = -29,/**< can not modify vlan after strip vlan tag twice */
    OFP_ERR_ACTION_VLAN_PUSH_WO_VID     = -30,    /**< should mod_vlan_vid after push_vlan */

    OFP_ERR_ACTION_VLAN_PUSH_W_MAC      = -31,    /**< too many output with pushing two vlan tag and modify mac*/
    OFP_ERR_ACTION_METER_NOT_FIRST      = -32,    /**< meter should be placed at beginning of action list */
    OFP_ERR_METER_ID_INVALID            = -33,    /**< invalid meter id */
    OFP_ERR_METER_NOT_EXIST             = -34,    /**< the meter is not existent. */
    OFP_ERR_METER_EXIST                 = -35,    /**< the meter is existent. */

    OFP_ERR_METER_NOT_SUPPORT_PPS       = -36,    /**< cannot support pps mode band. */
    OFP_ERR_METER_INVALID_FLAGS         = -37,    /**< invalid meter flags. */
    OFP_ERR_METER_TOO_MANY_BANDS        = -38,    /**< too many bands in one meter. */
    OFP_ERR_METER_INVALID_RATE          = -39,    /**< invalid meter band rate. */
    OFP_ERR_METER_INVALID_BURST         = -40,    /**< invalid meter band burst. */

    OFP_ERR_METER_INVALID_BAND_TYPE     = -41,    /**< invalid meter band type. */
    OFP_ERR_METER_OUT_OF_BAND_PROFILE   = -42,    /**< out of 255 meter band profiles. */
    OFP_ERR_METER_TOO_MANY_STATS        = -43,    /**< out of 256 meters with stats. */
    OFP_ERR_METER_TOO_MANY_METER        = -44,    /**< out of 2560 meters without stats. */
    OFP_ERR_METER_UPDATE_STATS          = -45,    /**< should not update stats. */

    OFP_ERR_SET_TPID_FLOW_NOT_EMPTY     = -46,    /**< must clear all flows before setting stag/ctag tpid */
    OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP   = -47,    /**< cannot modify vlan after strip_vlan */
    OFP_ERR_TOO_MANY_OUTPUT_SINGLE_FLOW = -48,    /**< too many output for one flow */
    OFP_ERR_LCM_MESSAGE_FUNC_NOT_INIT   = -49,    /**< not set callback function for lcm message */
    OFP_ERR_PORT_NOT_EXIST              = -50,    /**< port have not been created */

    OFP_ERR_TOO_MANY_OUTPUT             = -51,    /**< too many output port */
    OFP_ERR_INSTRUCTION_METER_NOT_EXIST = -52,    /**< the meter is not existent in instruction meter. */

    OFP_ERR_HYBRID_DISABLE              = -53,    /**< hybrid is disable on the port */
    OFP_ERR_HYBRID_MODE                 = -54,    /**< hybrid is mode mismatch */
    OFP_ERR_HYBRID_FDB_INIT_FAIL        = -55, 
    OFP_ERR_HYBRID_INVALID_HASH_KEY     = -56,    /**< hybrid is mode mismatch */
    OFP_ERR_HYBRID_RESOURCE_FULL        = -57,
    OFP_ERR_HYBRID_INVALID_PARAM        = -58,
    OFP_ERR_HYBRID_ENTRY_NOT_EXIT       = -59,
    OFP_ERR_HYBRID_ENTRY_SAME           = -60,
    OFP_ERR_HYBRID_PORT_VLAN_MISMATCH   = -61,

    OFP_ERR_OF13_BAD_SET_FIELD_TYPE     = -62,
    OFP_ERR_OF13_BAD_SET_FIELD_ARGUMENT = -63,
    OFP_ERR_BAD_ACTION_ARGUMENT         = -64,
    OFP_ERR_UNSUPPORTED_ACTIONS_ORDER   = -65,
    OFP_ERR_ACTIONS_TOO_MANY            = -66,

    OFP_ERR_ACTION_AND_MATCH_INCONSISTENT   = -67,
    OFP_ERR_BAD_MATCH_VALUE             = -68,
    OFP_ERR_BAD_ACTION_IN_PORT          = -69,      /**< no use */
    OFP_ERR_GROUP_EXISTS                = -70,
    OFP_ERR_GROUP_UNKNOWN               = -71,
    
    OFP_ERR_GROUP_INVALID               = -72,
    OFP_ERR_GROUP_TOO_MANY_BUCKETS      = -73,
    OFP_ERR_GROUP_TOO_MANY_GROUPS       = -74,
    OFP_ERR_GROUP_BAD_TYPE              = -75,
    OFP_ERR_GROUP_BAD_BUCKETS           = -76,

    OFP_ERR_GROUP_BAD_WATCH             = -77,
    OFP_ERR_WATCH_PORT_UNSUPPORTED      = -78,
    OFP_ERR_WATCH_GROUP_UNSUPPORTED     = -79,
    OFP_ERR_CHAINING_UNSUPPORTED        = -80,
    OFP_ERR_GROUP_CHAINED_GROUP         = -81,

    OFP_ERR_WEIGHT_UNSUPPORTED          = -82,
    OFP_ERR_TOO_MANY_GROUP_SINGLE_FLOW  = -83,      /**< too many group for one flow */
    OFP_ERR_TOO_MANY_METER_SINGLE_FLOW  = -84,      /**< too many group for one flow */
    OFP_ERR_NOT_GROUP_ONLY              = -85,      /**< group can not coexist with other actions */

    OFP_ERR_ACTION_BAD_OUT_GROUP        = -86,
    OFP_ERR_ACTION_BAD_TYPE             = -87,
    OFP_ERR_UNSUPPORTED_FLOW_FIELD      = -88,      /**< unsupported flow field */
    OFP_ERR_INVALID_SET_TUNNEL_ID       = -89,      /**< invalid set tunnel id */

    OFP_ERR_MAX                         = -90       /**< error max */
};
typedef enum ofp_error_e ofp_error_t;

enum ofp_error_type_s
{
    OFP_TYPE_FLOW = 1,
    OFP_TYPE_GROUP,
    OFP_TYPE_METER,
};
typedef enum ofp_error_type_s ofp_error_type_t;

struct adapt_to_ofp_error_code_map_s{
    int adapt_error_code;
    int ofp_error_code;
};
typedef struct adapt_to_ofp_error_code_map_s adapt_to_ofp_error_code_map_t;

#endif /* !__OFP_ERROR_H__ */
