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
 * @brief This file is the header of adpt_message.c
 */

#ifndef __ADPT_MESSAGE_H__
#define __ADPT_MESSAGE_H__
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ofp_types.h"
#include "ofp_const.h"
#include "adpt.h"
#include "glb_phy_define.h"
#include "glb_l2_define.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

typedef int32_ofp (*ADPT_MESSAGE_CB_FUNC)(void *); 

#define LCM2ADPT_MSG_SEND(msg_type, arg)\
    adpt_message_dispatch_msg_from_lcm(msg_type, arg)


/**
 @brief lcm notify adapter layer to init all modules
*/
struct adpt_notify_module_init_req_s
{
    uint32 enable;
};
typedef struct adpt_notify_module_init_req_s adpt_notify_module_init_req_t;

/**
 @brief lcm notify adapter layer to start all modules
*/
struct adpt_notify_module_start_req_s
{
    uint32 enable;
};
typedef struct adpt_notify_module_start_req_s adpt_notify_module_start_req_t;

/**
 @brief Create port request data structure
*/
struct adpt_create_port_if_req_s
{
    uint8_ofp  slot_no;
    uint8_ofp  port_no;
    uint16_ofp gport;
    uint8_ofp  mac[OFP_ETH_ADDR_LEN];
    uint32_ofp ifindex;
};
typedef struct adpt_create_port_if_req_s adpt_create_phy_port_req_t;

/**
 @brief Notify port link status request data structure
*/
struct adpt_notify_port_status_req_s
{
    uint8_ofp port_no;
    uint8_ofp slot_no;
    
    uint8_ofp         enable;
    glb_port_duplex_t duplex;
    glb_port_speed_t  speed;
    uint32_ofp        phy_type;
    glb_port_type_t   linkup_media;
    glb_port_cfg_t    port_cfg;
};
typedef struct adpt_notify_port_status_req_s adpt_notify_link_status_req_t;

/**
 @brief Create port request data structure
*/
struct adpt_fdb_req_s
{
    uint8_ofp valid_num;
    glb_fdb_info_t fdb_entry[ADPT_LEARNING_CACHE_MAX_INDEX];
};
typedef struct adpt_fdb_req_s adpt_fdb_req_t;

/**
 @brief  LCM to Adapter layer message type
*/
enum adpt_msg_type_e
{
    ADPT_MSG_TYPE_MIN = 0,
    ADPT_MSG_TYPE_MODULE_INIT,
    ADPT_MSG_TYPE_MODULE_START,
    ADPT_MSG_TYPE_CREATE_PORT,
    ADPT_MSG_TYPE_DESTORY_PORT,
    ADPT_MSG_TYPE_NOTIFY_LINK,
    ADPT_MSG_TYPE_INTERRUPT_LEARNING,
    ADPT_MSG_TYPE_INTERRUPT_AGING,
    ADPT_MSG_TYPE_MAX
};
typedef enum adpt_msg_type_e adpt_msg_type_t;

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
/**
 * Dispatch message from lcm
 * @param type          message type
 * @param arg           message arg
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_dispatch_msg_from_lcm(adpt_msg_type_t type, void* arg);

/**
 * Register callback function for lcm message
 * @param type          message type
 * @param func          callback function
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_register_lcm_callback(adpt_msg_type_t type, ADPT_MESSAGE_CB_FUNC func);

/**
 * Adapt layer message init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_init(void);

#endif /* !__ADPT_MESSAGE_H__ */
