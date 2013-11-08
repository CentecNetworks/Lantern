/**
 @file ctc_humber_queue.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-01-13

 @version v2.0

   The file provide all queue related APIs of Humber SDK.
*/


/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_queue.h"

#include "sys_humber_chip.h"
#include "sys_humber_queue_api.h"


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



/**
 @brief To globally enable/disable resource manage.

 @param[in] enable        Enable/Disable (TRUE/FALSE) resource manage globally

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_resrc_mgr_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_queue_resrc_mgr_global_enable(enable));

    return CTC_E_NONE;
}

/**
 @brief Get resource manage global enable status.

 @param[out] p_enable       Pointer to resource manage global enable status.

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_get_resrc_mgr_global_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_queue_get_resrc_mgr_global_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Configure queue drop scheme for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.

 @param[in] gport    Global port ID

 @param[in] qid      Queue ID in the given port, valid value range from 0 to CTC_MAX_PORT_QUEUE_NUM - 1

 @param[in] p_drop   Queue dropping configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop)
{
    CTC_ERROR_RETURN(sys_humber_set_port_queue_drop(gport, qid, p_drop));

    return CTC_E_NONE;
}


/**
 @brief Get queue drop config for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[out] p_drop   Queue dropping configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop)
{
    CTC_ERROR_RETURN(sys_humber_get_port_queue_drop(gport, qid, p_drop));

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS flow ID.

 @param[in] enable        Enable/Disable (TRUE/FALSE) QoS flow ID globally

 @return CTC_E_XXX
*/
int32
ctc_humber_qos_flow_id_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_qos_flow_id_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get QoS flow ID global enable status.

 @param[out] p_enable       Pointer to QoS flow ID global enable status.

 @return CTC_E_XXX
*/
int32
ctc_humber_qos_get_flow_id_global_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_qos_get_flow_id_global_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable queue shaping function.

 @param[in]  enable   Enable/disable (TRUE/FALSE) queue shaping globally.

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_shape_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_queue_shape_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get queue shape global enable status.

 @param[out] p_enable Pointer to queue shape global enable status.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_queue_shape_global_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_get_queue_shape_global_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable group shaping function.

 @param[in]  enable   Enable/disable (TRUE/FALSE) group shaping globally.

 @return CTC_E_XXX
*/
int32
ctc_humber_group_shape_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_group_shape_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get group shape global enable status.

 @param[out] p_enable Pointer to group shape global enable status.

 @return CTC_E_XXX
*/
int32
ctc_humber_get_group_shape_global_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_get_group_shape_global_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Set group shape IPG enable.

 @param[in]  enable   Enable/disable (TRUE/FALSE) group shaping IPG globally.

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_shape_ipg_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_queue_shape_ipg_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Set shaping for the given queue in a port. Queue shaping supports two-rate two token bucket algorithm.
        Traffic rate below CIR is viewed as in-profile, between CIR and PIR is viewed as out-profile, and above
        PIR is backlogged in the queue and get dropped if the queue overflows.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[in]  p_shape  Queue shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_set_port_queue_shape(gport, qid, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel shaping for the given queue in a port.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @return CTC_E_XXX
*/
int32
ctc_humber_unset_port_queue_shape(uint16 gport, uint8 qid)
{
    CTC_ERROR_RETURN(sys_humber_unset_port_queue_shape(gport, qid));

    return CTC_E_NONE;
}

/**
 @brief Get real shaping parameters for the given queue in a port.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[out] p_shape  Queue shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_get_port_queue_shape(gport, qid, p_shape));

    return CTC_E_NONE;
}

/**
 @brief Get total number of queue shape profile.

 @param[out]  p_shape_num    queue profile number

 @return CTC_E_XXX
*/
 int32 
 ctc_humber_get_port_queue_shape_profile_num(uint32* p_shape_num)
 {
    CTC_ERROR_RETURN(sys_humber_get_port_queue_profile_num(p_shape_num));

    return CTC_E_NONE;
}

/**
 @brief Globally enable/disable channel shaping function.

 @param[in]  enable   Enable/disable (TRUE/FALSE) channel shaping globally.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_channel_shape_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_channel_shape_global_enable(enable));

    return CTC_E_NONE;
}

/**
 @brief Get channel shape global enable status.

 @param[out] p_enable Pointer to channel shape global enable status.

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_channel_shape_global_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_get_channel_shape_global_enable(p_enable));

    return CTC_E_NONE;
}



/**
 @brief Set shaping for the given port. Port shaping supports single-rate single bucket algorithm. Whether
        a packet can be scheduled is determined by whether there are enough tokens in the bucket.

 @param[in]  gport    Global port ID

 @param[in]  p_shape  Port shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_set_port_shape(gport, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel port shaping for the given port.

 @param[in]  gport    Global port ID

 @return CTC_E_XXX
*/
int32
ctc_humber_unset_port_shape(uint16 gport)
{
    CTC_ERROR_RETURN(sys_humber_unset_port_shape(gport));

    return CTC_E_NONE;
}


/**
 @brief Get real shaping parameters for the given port.

 @param[in]  gport    Global port ID

 @param[out] p_shape  Port shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_get_port_shape(gport, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Mapping the given port queue to a class. Queues in the different classes are serviced by SP scheduling
        mode, and queues in the same class are serviced by WDRR scheduling mode.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[in]  class    Class level

 @return CTC_E_XXX
*/
int32
ctc_humber_set_port_queue_class(uint16 gport, uint8 qid, uint8 class)
{
    CTC_ERROR_RETURN(sys_humber_set_port_queue_class(gport, qid, class));

    return CTC_E_NONE;
}


/**
 @brief Get queue class for the given port queue.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[out] p_class  Pointer to the class level

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_queue_class(uint16 gport, uint8 qid, uint8* p_class)
{
    CTC_ERROR_RETURN(sys_humber_get_port_queue_class(gport, qid, p_class));

    return CTC_E_NONE;
}


/**
 @brief Set port queue DRR weight. Queues in the same class are scheduled by WDRR algorithm. The weight
        influences the proportion of the shared bandwidth among queues.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[in]  weight   Queue DRR weight, <1-(0xFFFFFF/MTU)>, MAX 65535

 @return CTC_E_XXX
*/
int32
ctc_humber_set_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16 weight)
{
    CTC_ERROR_RETURN(sys_humber_set_port_queue_wdrr_weight(gport, qid, weight));

    return CTC_E_NONE;
}


/**
 @brief Get port queue DRR weight.

 @param[in]  gport    Global port ID

 @param[in]  qid      Queue ID in the given port

 @param[out] weight   Queue DRR weight

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16* p_weight)
{
    CTC_ERROR_RETURN(sys_humber_get_port_queue_wdrr_weight(gport, qid, p_weight));

    return CTC_E_NONE;
}


/**
 @brief set queue wdrr weight mtu.

 @param[in]  mtu    wdrr weight mtu

 @return CTC_E_XXX
*/
int32
ctc_humber_set_queue_wdrr_weight_mtu(uint32 mtu)
{
    CTC_ERROR_RETURN(sys_humber_set_queue_wdrr_weight_mtu(mtu));

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable queue statistics function.

 @param[in]  enable    TRUE for enable and FALSE for disable queue statistics globally.

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_stats_global_enable(bool enable)
{
    CTC_ERROR_RETURN(sys_humber_queue_stats_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief To get queue statistics enable status.

 @param[out]  p_enable Pointer to the bool value.

 @return CTC_E_XXX
*/
int32
ctc_humber_get_queue_stats_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_get_queue_stats_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Get statistics for the given queue in the specific port.

 @param[in]  gport     Global port ID

 @param[in]  qid       Queue ID in the given port

 @param[out] p_stats   Pointer to the queue statistic result

 @return CTC_E_XXX
*/
int32
ctc_humber_get_port_queue_stats(uint16 gport, uint8 qid, ctc_queue_stats_t* p_stats)
{
    CTC_ERROR_RETURN(sys_humber_get_port_queue_stats(gport, qid, p_stats));

    return CTC_E_NONE;
}


/**
 @brief Clear statistics for the given queue in the specific port.

 @param[in]  gport     Global port ID

 @param[in]  qid       Queue ID in the given port

 @return CTC_E_XXX
*/
int32
ctc_humber_reset_port_queue_stats(uint16 gport, uint8 qid)
{
    CTC_ERROR_RETURN(sys_humber_reset_port_queue_stats(gport, qid));

    return CTC_E_NONE;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~service queue APIs~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 @brief Get service queue enable status.

 @param[in]  p_enable       Pointer to service queue enable status

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_enable(bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Get service queue enable status.

 @param[in]  p_que_num      Pointer to per-service queue number

 @return CTC_E_XXX
*/
int32
ctc_humber_get_per_service_queue_num(uint8* p_que_num)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_num(p_que_num));

    return CTC_E_NONE;
}



/**
 @brief Create a service.

 @param[in]  service_id         Service ID to uniquely identify a service.

 @param[in]  dest_port          The destination port.

 @return CTC_E_XXX
*/
int32
ctc_humber_create_service(uint16 service_id, uint8 dest_port)
{
    CTC_ERROR_RETURN(sys_humber_create_service(service_id, dest_port));

    return CTC_E_NONE;
}


/**
 @brief Remove a service.

 @param[in]  service_id         Service ID to uniquely identify a service.

 @return CTC_E_XXX
*/
int32
ctc_humber_remove_service(uint16 service_id)
{
    CTC_ERROR_RETURN(sys_humber_remove_service(service_id));

    return CTC_E_NONE;
}


/**
 @brief Set queue shape for the given service at specific direction.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[in]  p_shape    Queue shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_set_service_queue_shape(type, service_id, qid, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel queue shape for the given service at specific direction.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @return CTC_E_XXX
*/
int32
ctc_humber_unset_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid)
{
    CTC_ERROR_RETURN(sys_humber_unset_service_queue_shape(type, service_id, qid));

    return CTC_E_NONE;
}


/**
 @brief Get queue shape configuration for the given service at specific direction.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[out] p_shape    Pointer to the queue shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_shape(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_shape(type, service_id, qid, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Set service queue class mapping.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[in]  class      Class

 @return CTC_E_XXX
*/
int32
ctc_humber_set_service_queue_class(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint8 class)
{
    CTC_ERROR_RETURN(sys_humber_set_service_queue_class(type, service_id, qid, class));

    return CTC_E_NONE;
}


/**
 @brief Get service queue class mapping.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[out] p_class    Pointer to class value

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_class(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint8* p_class)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_class(type, service_id, qid, p_class));

    return CTC_E_NONE;
}


/**
 @brief Set service queue drop scheme.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[in]  p_drop     Drop configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_service_queue_drop(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    CTC_ERROR_RETURN(sys_humber_set_service_queue_drop(type, service_id, qid, p_drop));

    return CTC_E_NONE;
}


/**
 @brief Get service queue drop configuration.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[out] p_drop     Pointer to service queue drop configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_drop(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_drop(type, service_id, qid, p_drop));

    return CTC_E_NONE;
}



/**
 @brief Set service queue WDRR weight.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[in]  weight     WDRR weight

 @return CTC_E_XXX
*/
int32
ctc_humber_set_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint16 weight)
{
    CTC_ERROR_RETURN(sys_humber_set_service_queue_wdrr_weight(type, service_id, qid, weight));

    return CTC_E_NONE;
}


/**
 @brief Get service queue WDRR weight.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[out] p_weight   Point to WDRR weight

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 service_id, uint8 qid, uint16* p_weight)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_wdrr_weight(type, service_id, qid, p_weight));

    return CTC_E_NONE;
}



/**
 @brief Get service queue statistics.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  qid        Queue ID

 @param[out] p_stats    Pointer to the queue statistics

 @return CTC_E_XXX
*/
int32
ctc_humber_get_service_queue_stats(ctc_queue_type_t type, uint16 service_id, uint8 qid, ctc_queue_stats_t* p_stats)
{
    CTC_ERROR_RETURN(sys_humber_get_service_queue_stats(type, service_id, qid, p_stats));

    return CTC_E_NONE;
}

/**
 @brief Clear service queue statistics.

 @param[in]  type       CTC_QUEUE_TYPE_SERVICE_INGRESS or CTC_QUEUE_TYPE_SERVICE_EGRESS

 @param[in]  service_id Service ID identifying a unique service instance

 @param[in]  dir        Direction

 @param[in]  qid        Queue ID

 @return CTC_E_XXX
*/
int32
ctc_humber_reset_service_queue_stats(ctc_queue_type_t type, uint16 service_id, uint8 qid)
{
    CTC_ERROR_RETURN(sys_humber_reset_service_queue_stats(type, service_id, qid));

    return CTC_E_NONE;
}


/**
 @brief Set group shape.

 @param[in]  group_shape    Group shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_set_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    CTC_ERROR_RETURN(sys_humber_set_group_shape(group_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel group shape.

 @param[in]  group_shape    Group shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_unset_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    CTC_ERROR_RETURN(sys_humber_unset_group_shape(group_shape));

    return CTC_E_NONE;
}


/**
 @brief Get group shape for the given service at specific direction.

 @param[out]  group_shape   Pointer to the group shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_get_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    CTC_ERROR_RETURN(sys_humber_get_group_shape(group_shape));

    return CTC_E_NONE;
}

/**
 @brief set priority map with queue.

 @param[in]  p_queue_pri_map   map info of prioriyt and queue

 @return CTC_E_XXX
*/
int32
ctc_humber_set_queue_priority_map(ctc_queue_pri_map_t* p_queue_pri_map)
{
    CTC_ERROR_RETURN(sys_humber_set_queue_priority_map(p_queue_pri_map));

    return CTC_E_NONE;
}

/**
 @brief Set queue size mode

 @param[in]  size_mode   queue size mode, 1 -- buffer_cnt, 0 -- packet

 @return CTC_E_XXX
*/
int32
ctc_humber_set_queue_size_mode(uint8 size_mode)
{
    CTC_ERROR_RETURN(sys_humber_set_queue_size_mode(size_mode));

    return CTC_E_NONE;
}

/**
 @brief Queue initialization.

 @param[in] queue_cfg    queue global config info

 @return CTC_E_XXX
*/
int32
ctc_humber_queue_init(ctc_queue_global_cfg_t* queue_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_queue_init(queue_global_cfg));

    return CTC_E_NONE;
}


/**@}*/ /*end of @addtogroup Queue Queue*/


