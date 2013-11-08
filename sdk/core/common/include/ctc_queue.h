/**
 @file ctc_queue.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2012-01-06

 @version v2.0

   This file contains all queue related data structure, enum, macro and proto.
*/


#ifndef _CTC_QUEUE_H_
#define _CTC_QUEUE_H_

#include "ctc_const.h"
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/


/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define CTC_MAX_QUEUE_CLASS_NUM     4
#define CTC_RESC_DROP_PREC_NUM      4
#define CTC_DROP_PREC_NUM           4

/****************************************************************
*
* Data Structures
*
****************************************************************/



/**
 @defgroup queue QUEUE
 @{
*/


/**
 @brief Queue type definition.
*/
enum ctc_queue_type_e
{
    CTC_QUEUE_TYPE_SERVICE_INGRESS,     /**< Ingress service queue */
    CTC_QUEUE_TYPE_NETWORK_EGRESS,      /**< Egress network queue */
    CTC_QUEUE_TYPE_SERVICE_EGRESS,      /**< Egress service queue */
    CTC_QUEUE_TYPE_ILOOP,               /**< Ingress loopback queue */
    CTC_QUEUE_TYPE_NORMAL_CPU,          /**< Normal CPU queue */
    CTC_QUEUE_TYPE_OAM,                 /**< OAM queue */
    CTC_QUEUE_TYPE_ELOOP,               /**< Egress loopback queue */
    CTC_QUEUE_TYPE_STATIC_INT_PORT,     /**< Static internal port queue */
    CTC_QUEUE_TYPE_EXCP_CPU,            /**< Exception CPU queue */
    CTC_QUEUE_TYPE_FABRIC,              /**< Fabric queue */
    CTC_QUEUE_TYPE_INTERNAL_PORT,       /**< Internal port queue */
    CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU,   /**<Sgmac port to cpu queue*/
    CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU,    /**<Sgmac cpu to cpu queue*/

    MAX_CTC_QUEUE_TYPE
};
typedef enum ctc_queue_type_e ctc_queue_type_t;


/**
 @brief Queue drop mode.
*/
enum ctc_queue_drop_mode_s
{
    CTC_QUEUE_DROP_WTD,     /**< WTD drop mode */
    CTC_QUEUE_DROP_WRED,    /**< WRED drop mode */

    MAX_CTC_QUEUE_DROP
};
typedef enum ctc_queue_drop_mode_s ctc_queue_drop_mode_t;


/**
 @brief Queue szie mode.
*/
enum ctc_queue_size_mode_s
{
    CTC_QUEUE_SIZE_PACKET,      /**< packet mode */
    CTC_QUEUE_SIZE_BUF_CNT,     /**< buffer_cnt mode */

    MAX_CTC_QUEUE_SIZE_MODE
};
typedef enum ctc_queue_size_mode_s ctc_queue_size_mode_t;


/**
 @brief Group shape type.
*/
enum ctc_group_shape_type_e
{
    CTC_PORT_GROUP_SHAPE,           /**< network port:0-51, iloop:52, normal cpu:53, oam:54, eloop:55, static internal port:56-63, internal port 64-XX */
    CTC_SERVICE_GROUP_SHAPE,        /**< service group shape */
};
typedef enum ctc_group_shape_type_e ctc_group_shape_type_t;


/**
 @brief Queue statistics.
*/
struct ctc_queue_stats_s
{
    uint64 deq_packets;     /**< De-queue packet number */
    uint64 deq_bytes;       /**< De-queue packet bytes */
    uint64 drop_packets;    /**< Dropped packet number */
    uint64 drop_bytes;      /**< Dropped packet bytes */
};
typedef struct ctc_queue_stats_s ctc_queue_stats_t;


/**
 @brief Queue drop parameters.
*/
struct ctc_queue_drop_s
{
    ctc_queue_drop_mode_t mode;          /**< Queue Drop Mode, type of CTC_QUEUE_DROP_XXX */

    uint16 min_th[CTC_DROP_PREC_NUM];    /**< WRED min threshold */
    uint16 max_th[CTC_DROP_PREC_NUM];    /**< WRED max threshold, or WTD drop threshold */
    uint16 drop_prob[CTC_DROP_PREC_NUM]; /**< WRED max drop probability */
    uint8  weight;                       /**< WRED weight */
    uint8  rsv1;
    uint16 rsv2;
};
typedef struct ctc_queue_drop_s ctc_queue_drop_t;


/**
 @brief Queue shape parameters.
*/
struct ctc_two_rate_shape_s
{
    uint32 cir;     /**< Commit information rate */
    uint32 cbs;     /**< Commit burst size */
    uint32 pir;     /**< Peak information rate */
    uint32 pbs;     /**< Peak burst size */
};
typedef struct ctc_two_rate_shape_s ctc_queue_shape_t;


/**
 @brief Port shape and channel shape parameters.
*/
struct ctc_single_rate_shape_s
{
    uint32 pir;     /**< Peak information rate */
    uint32 pbs;     /**< Peak burst size */
};
typedef struct ctc_single_rate_shape_s ctc_port_shape_t;
typedef struct ctc_single_rate_shape_s ctc_group_shape_t;


/**
 @brief Group shape parameters.
*/
struct ctc_group_shape_cfg_s
{
    ctc_group_shape_type_t type; /**< shape type */
    union
    {
        uint16 gport;           /**< global port, which normal queue need do group shape */
        uint16 serviceId;       /**< a service, which queue need do group shape*/
    }para;
    uint8 rsv0[2];
    ctc_direction_t dir;        /**< service direction, not support both */

    ctc_group_shape_t shape;    /**< shape rate information */
};
typedef struct ctc_group_shape_cfg_s ctc_group_shape_cfg_t;


/**
 @brief Queue priority map.
*/
struct ctc_queue_pri_map_s
{
    uint8 priority;             /**< priority <0-63> */
    uint8 color;                /**< color 1-red 2-yellow 3-green */
    uint8 queue_select;         /**< mapped queue select <0-63> */
    uint8 drop_precedence;      /**< mapped drop precedence <0-3> */
};
typedef struct ctc_queue_pri_map_s ctc_queue_pri_map_t;


/**
 @brief Global queue config info.
*/
struct ctc_queue_global_cfg_s
{
    uint8 queue_num_per_network_port;       /**< queue number per network port */
    uint8 queue_num_per_static_int_port;    /**< queue number per static internal port */
    uint8 queue_num_per_fabric;             /**< queue number per fabric */
    uint8 max_internal_port_id;             /**< max internal port id */
    uint8 queue_num_per_internal_port;      /**< queue number per internal port */
    uint8 rsv1[3];
    uint16 service_num;                     /**< service number */
    uint8 queue_num_per_ingress_service;    /**< queue number per ingress service */
    uint8 queue_num_per_egress_service;     /**< queue number per egress service */
};
typedef struct ctc_queue_global_cfg_s ctc_queue_global_cfg_t;

/**@} end of @defgroup queue QUEUE  */

#endif

