/**
 @file ctc_pdu.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-30

 @version v2.0

 This file contains all pdu related data structure, enum, macro and proto.
*/

#ifndef _CTC_PDU_H_
#define _CTC_PDU_H_

/**
 @defgroup pdu PDU
 @{
*/

/**
 @defgroup l2pdu L2PDU
 @{
*/

/**
 @brief  l2 pdu action based flags
*/
enum ctc_pdu_l2pdu_type_e
{
    CTC_PDU_L2PDU_TYPE_L3TYPE,              /**<layer2 pdu action based on layer3 type*/
    CTC_PDU_L2PDU_TYPE_L2HDR_PROTO,         /**<layer2 pdu action based on ether type*/
    CTC_PDU_L2PDU_TYPE_BPDU,                /**<01:80:c2:00:00:00*/
    CTC_PDU_L2PDU_TYPE_MACDA,               /**<layer2 pdu action based on macda*/
    CTC_PDU_L2PDU_TYPE_MACDA_LOW24,         /**<layer2 pdu action based on macda low24 bit, the most 24bit is 01:80:C2*/
    MAX_CTC_PDU_L2PDU_TYPE
};
typedef enum ctc_pdu_l2pdu_type_e ctc_pdu_l2pdu_type_t;


/**
 @brief  layer2 pdu entry fileds
*/
union ctc_pdu_l2pdu_key_u
{
    /*CTC_PDU_L2PDU_TYPE_MACDA and CTC_PDU_L2PDU_TYPE_MACDA_LOW24*/
    struct
    {
       mac_addr_t mac;                    /**<mac da */
       mac_addr_t mac_mask;               /**<mac da mask*/
    } l2pdu_by_mac;
    uint16 l2hdr_proto;                   /**<layer2 header protocol :CTC_PDU_L2PDU_TYPE_L2HDR_PROTO*/
 };
typedef union ctc_pdu_l2pdu_key_u ctc_pdu_l2pdu_key_t;

/**
 @brief  layer2 pdu global action types
*/
struct ctc_pdu_global_l2pdu_action_s
{
    uint8 bypass_all;           /**<when set the bit, not do lkp operation and so on*/
    uint8 entry_valid;          /**<control the added protocol entry is valid or not*/

    uint8 action_index;         /**<entry for per-port action index, the index range is 0-14*/
    uint8 copy_to_cpu;          /**<bpdu exception operation*/
};
typedef struct ctc_pdu_global_l2pdu_action_s ctc_pdu_global_l2pdu_action_t;

/**
 @brief  layer2 pdu action types
*/
enum ctc_pdu_port_l2pdu_action_e
{
    CTC_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU,         /**<layer2 pdu action type: redirect to cpu*/
    CTC_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU,             /**<layer2 pdu action type: copy to cpu*/
    CTC_PDU_L2PDU_ACTION_TYPE_FWD,                     /**<layer2 pdu action type: normal forwarding*/
    CTC_PDU_L2PDU_ACTION_TYPE_DISCARD,                 /**<layer2 pdu action type: discard the pdu*/
    MAX_CTC_PDU_L2PDU_ACTION_TYPE
};
typedef enum ctc_pdu_port_l2pdu_action_e ctc_pdu_port_l2pdu_action_t;


/**@} end of @defgroup  l2pdu L2PDU */


/**
 @defgroup l3pdu L3PDU
 @{
*/

/**
 @brief  layer3 pdu action based flags
*/
enum ctc_pdu_l3pdu_type_e
{
    CTC_PDU_L3PDU_TYPE_L3HDR_PROTO,      /**<layer3 pdu action based on layer3 header protocol*/
    CTC_PDU_L3PDU_TYPE_LAYER4_PORT,      /**<layer3 pdu action based on layer4 destport*/
    CTC_PDU_L3PDU_TYPE_LAYER4_TYPE,      /**<layer3 pdu action based on layer4 type*/
    MAX_CTC_PDU_L3PDU_TYPE
};
typedef enum ctc_pdu_l3pdu_type_e ctc_pdu_l3pdu_type_t;

/**
 @brief  layer3 pdu entry fields
*/
union ctc_pdu_l3pdu_key_s
{

    uint8 l3hdr_proto;      /**<layer3 header protocol*/

    struct
    {
        uint16 dest_port;    /**< layer4 dest port*/
        uint8 is_tcp;        /**< dest_port field is tcp value*/
        uint8 is_udp;        /**< dest_port field is udp vlaue*/
    } l3pdu_by_port;
};
typedef union ctc_pdu_l3pdu_key_s ctc_pdu_l3pdu_key_t;

/**
 @brief  layer3 pdu entry fields
*/
struct ctc_pdu_global_l3pdu_action_s
{
    uint8 action_index;      /**<entry for per-l3if action index, the  index range is 0-12*/
    uint8 entry_valid;       /**<layer3 protocol action entry is valid or not*/

};
typedef struct ctc_pdu_global_l3pdu_action_s ctc_pdu_global_l3pdu_action_t;

/**
 @brief  layer2 pdu action types
*/
enum ctc_pdu_l3if_l3pdu_action_e
{
    CTC_PDU_L3PDU_ACTION_TYPE_FWD,                    /**<layer3 pdu action type: normal forwarding*/
    CTC_PDU_L3PDU_ACTION_TYPE_COPY_TO_CPU,            /**<layer3 pdu action type: copy to cpu*/
    MAX_CTC_PDU_L3PDU_ACTION_TYPE
};
typedef enum ctc_pdu_l3if_l3pdu_action_e ctc_pdu_l3if_l3pdu_action_t;


/**@} end of @defgroup  l3pdu L3PDU*/

/**@} end of @defgroup  pdu PDU */

#endif




