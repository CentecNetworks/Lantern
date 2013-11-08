/**
 @file sys_humber_queue_api.c

 @date 2010-01-13

 @version v2.0

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
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_drop.h"
#include "sys_humber_queue_shape.h"
#include "sys_humber_queue_sch.h"
#include "sys_humber_stats.h"
#include "sys_humber_ftm.h"
#include "sys_humber_internal_port.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
uint8 queue_init = 0;

#define SYS_QUEUE_INIT_CHECK() \
    {\
        if(  queue_init == 0)\
            return CTC_E_NOT_INIT;\
    }

extern uint32 queue_wdrr_weight_mtu;
extern sys_queue_enq_ctl_t sys_enq_ctl;

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 @brief Configure queue drop scheme for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.
*/
int32
sys_humber_set_port_queue_drop(uint16 gport, uint8 offset, ctc_queue_drop_t* p_drop)
{
    uint16 queue_id;
    uint8 lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, offset, &queue_id));

    if (CTC_QUEUE_DROP_WRED == p_drop->mode)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_wred_weight(lchip, queue_id, p_drop->weight));
    }
    else
    {
        /*humber use oldEnqQueAvgDepth instead of newEnqQueInstDepth in tail drop, so we should configure factor
        to 0 to ensure oldEnqQueAvgDepth == newEnqQueInstDepth*/
        CTC_ERROR_RETURN(sys_humber_queue_set_wred_weight(lchip, queue_id, 0));
    }

    CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, p_drop));

    return CTC_E_NONE;
}


/**
 @brief Get queue drop config for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.
*/
int32
sys_humber_get_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));

    CTC_ERROR_RETURN(sys_humber_queue_get_drop(lchip, queue_id, p_drop));

    if (CTC_QUEUE_DROP_WRED == p_drop->mode)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_wred_weight(lchip, queue_id, &p_drop->weight));
    }

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS flow ID.
*/
int32
sys_humber_qos_flow_id_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_flow_id_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get QoS flow ID global enable status.
*/
int32
sys_humber_qos_get_flow_id_global_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_get_flow_id_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable queue shaping function.
*/
int32
sys_humber_queue_shape_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get queue shape global enable status.
*/
int32
sys_humber_get_queue_shape_global_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_get_queue_shape_global_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable group shaping function.
*/
int32
sys_humber_group_shape_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_set_group_shape_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get group shape global enable status.
*/
int32
sys_humber_get_group_shape_global_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_get_group_shape_global_enable(p_enable));

    return CTC_E_NONE;
}


int32
sys_humber_queue_shape_ipg_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_queue_set_shape_ipg_enable(enable));

    return CTC_E_NONE;
}



/**
 @brief Set shaping for the given queue in a port. Queue shaping supports two-rate two token bucket algorithm.
        Traffic rate below CIR is viewed as in-profile, between CIR and PIR is viewed as out-profile, and above
        PIR is backlogged in the queue and get dropped if the queue overflows.
*/

int32
sys_humber_set_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get queue type and queue number per port by port*/
    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape(lchip, queue_id, p_shape));

    return CTC_E_NONE;
}

/**
 @brief Cancel shaping for the given queue in a port.
*/
int32
sys_humber_unset_port_queue_shape(uint16 gport, uint8 qid)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


    /*get queue type and queue number per port by port*/
    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));

    return CTC_E_NONE;
}


/**
 @brief Get shaping parameters for the given queue in a port.
*/
int32
sys_humber_get_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get queue type and queue number per port by port*/
    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_get_queue_shape(lchip, queue_id, p_shape));

    return CTC_E_NONE;
}

/**
 @brief Get total number of queue shape profile.
*/
int32
sys_humber_get_port_queue_profile_num(uint32* p_shape_num)
{

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_profile_num(p_shape_num));

    return CTC_E_NONE;
}

/**
 @brief Globally enable/disable channel shaping function.
*/
int32
sys_humber_channel_shape_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_set_channel_shape_global_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get channel shape global enable status.
*/
int32
sys_humber_get_channel_shape_global_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_queue_get_channel_shape_global_enable(p_enable));

    return CTC_E_NONE;
}



/**
 @brief Set shaping for the given port. Port shaping supports single-rate single bucket algorithm. Whether
        a packet can be scheduled is determined by whether there are enough tokens in the bucket.
*/
int32
sys_humber_set_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    uint8 lchip, lport;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_NETWORK_PORT_NUM - 1);

    CTC_ERROR_RETURN(sys_humber_queue_set_channel_shape(lchip, lport, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel port shaping for the given port.
*/
int32
sys_humber_unset_port_shape(uint16 gport)
{
    uint8 lchip, lport;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_NETWORK_PORT_NUM - 1);

    CTC_ERROR_RETURN(sys_humber_queue_unset_channel_shape(lchip, lport));

    return CTC_E_NONE;
}

/**
 @brief Get shaping parameters for the given port.
*/
int32
sys_humber_get_port_shape(uint16 gport, ctc_port_shape_t* p_shape)
{
    uint8 lchip, lport;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_NETWORK_PORT_NUM - 1);

    CTC_ERROR_RETURN(sys_humber_queue_get_channel_shape(lchip, lport, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Mapping the given port queue to a class. Queues in the different classes are serviced by SP scheduling
        mode, and queues in the same class are serviced by WDRR scheduling mode.
*/
int32
sys_humber_set_port_queue_class(uint16 gport, uint8 qid, uint8 class)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, class));

    return CTC_E_NONE;
}


/**
 @brief Get class level for the given port queue.
*/
int32
sys_humber_get_port_queue_class(uint16 gport, uint8 qid, uint8* p_class)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_class);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));

    *p_class = sys_queue_config[lchip][queue_id].class;

    return CTC_E_NONE;
}


/**
 @brief Set port queue DRR weight. Queues in the same class are scheduled by WDRR algorithm. The weight
        influences the proportion of the shared bandwidth among queues.
*/
int32
sys_humber_set_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16 weight)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_set_queue_wdrr_weight(lchip, queue_id, weight));

    return CTC_E_NONE;
}


/**
 @brief Get port queue DRR weight.
*/
int32
sys_humber_get_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16* p_weight)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_get_queue_wdrr_weight(lchip, queue_id, p_weight));

    return CTC_E_NONE;
}

/**
 @brief set queue wdrr weight mtu.
*/
int32
sys_humber_set_queue_wdrr_weight_mtu(uint32 mtu)
{
    CTC_VALUE_RANGE_CHECK(mtu, 1, SYS_QUEUE_MAX_DRR_WEIGHT);

    queue_wdrr_weight_mtu = mtu;
    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable queue statistics function.
*/
int32
sys_humber_queue_stats_global_enable(bool enable)
{
    uint8 lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_stats_set_queue_en(lchip, enable));
    }

    return CTC_E_NONE;
}


/**
 @brief To get queue statistics enable status.
*/
int32
sys_humber_get_queue_stats_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    CTC_ERROR_RETURN(sys_humber_stats_get_queue_en(0, p_enable));

    return CTC_E_NONE;
}


/**
 @brief Get statistics for the given queue in the specific port.
*/
int32
sys_humber_get_port_queue_stats(uint16 gport, uint8 qid, ctc_queue_stats_t* p_stats)
{
    sys_stats_queue_t stats;
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;
    uint8 queue_num;

    SYS_QUEUE_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num));
    CTC_MAX_VALUE_CHECK(lport, SYS_NETWORK_PORT_NUM - 1);
    CTC_MAX_VALUE_CHECK(qid, queue_num - 1);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_stats_get_queue_stats(lchip, queue_id, &stats));

    p_stats->deq_packets  = stats.queue_deq_pkts;
    p_stats->deq_bytes    = stats.queue_deq_bytes;
    p_stats->drop_packets = stats.queue_drop_pkts;
    p_stats->drop_bytes   = stats.queue_drop_bytes;

    return CTC_E_NONE;
}

/**
 @brief Clear statistics for the given queue in the specific port.
*/
int32
sys_humber_reset_port_queue_stats(uint16 gport, uint8 qid)
{
    uint16 queue_id;
    uint8  lchip, lport;
    ctc_queue_type_t queue_type = 0;
    uint8 queue_num;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num));

    CTC_MAX_VALUE_CHECK(qid, queue_num - 1);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_stats_reset_queue_stats(lchip, queue_id));

    return CTC_E_NONE;
}

/*~~~~~~~~~~~~~~~~~~~~~~~service queue APIs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 @brief Get service queue enable status.
*/
int32
sys_humber_get_service_queue_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_ERROR_RETURN(sys_humber_get_service_id_enable(p_enable));

    return CTC_E_NONE;
}


/**
 @brief Get per-service queue number.
*/
int32
sys_humber_get_service_queue_num(uint8* p_que_num)
{
    SYS_QUEUE_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_que_num);

    *p_que_num = sys_humber_get_per_service_queue_num();

    return CTC_E_NONE;
}



/**
 @brief Create a service.
*/
int32
sys_humber_create_service(uint16 logical_service_id, uint8 dest_port)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_create_physical_service_id(logical_service_id, dest_port));

    CTC_ERROR_RETURN(sys_humber_service_queue_init(logical_service_id, dest_port));

    return CTC_E_NONE;
}



/**
 @brief Remove a service.
*/
int32
sys_humber_remove_service(uint16 logical_service_id)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_queue_deinit(logical_service_id));

    CTC_ERROR_RETURN(sys_humber_remove_physical_service_id(logical_service_id));

    return CTC_E_NONE;
}


/**
 @brief Register a service.
*/
int32
sys_humber_register_service(uint16 logical_service_id)
{
    sys_service_config_t *p_service;

    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_NOT_EQUAL_CHECK(p_service->register_num, CTC_MAX_UINT16_VALUE);
    p_service->register_num++;

    return CTC_E_NONE;
}


/**
 @brief Unregister a service.
*/
int32
sys_humber_unregister_service(uint16 logical_service_id)
{
    sys_service_config_t *p_service;

    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_NOT_EQUAL_CHECK(p_service->register_num, 0);
    p_service->register_num--;

    return CTC_E_NONE;
}


/**
 @brief Get physical service ID by logical ID.
*/
int32
sys_humber_get_physical_service_id(uint16 logical_service_id, uint16* p_physical_service_id)
{
    sys_service_config_t *p_service;

    SYS_QUEUE_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_physical_service_id);

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    *p_physical_service_id = p_service->physical_service_id;

    return CTC_E_NONE;
}


/**
 @brief Set queue shape for the given service.
*/
int32
sys_humber_set_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_set_queue_shape(type, logical_service_id, qid, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel queue shape for the given service.
*/
int32
sys_humber_unset_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_unset_queue_shape(type, logical_service_id, qid));

    return CTC_E_NONE;
}


/**
 @brief Get queue shape configuration for the given service.
*/
int32
sys_humber_get_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_shape_t* p_shape)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_service_get_queue_shape(type, logical_service_id, qid, p_shape));

    return CTC_E_NONE;
}


/**
 @brief Set group shape.
*/
int32
sys_humber_set_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_group_set_shape(group_shape));

    return CTC_E_NONE;
}


/**
 @brief Cancel group shape.
*/
int32
sys_humber_unset_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_group_unset_shape(group_shape));

    return CTC_E_NONE;
}


/**
 @brief Get group shape.
*/
int32
sys_humber_get_group_shape(ctc_group_shape_cfg_t* group_shape)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_group_get_shape(group_shape));

    return CTC_E_NONE;
}


/**
 @brief Set service queue class mapping.
*/
int32
sys_humber_set_service_queue_class(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint8 class)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_MAX_VALUE_CHECK(class, CTC_MAX_QUEUE_CLASS_NUM - 1);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, class));
    }

    return CTC_E_NONE;
}


/**
 @brief Get service queue class mapping.
*/
int32
sys_humber_get_service_queue_class(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint8* p_class)
{
    sys_service_config_t *p_service;
    uint16 queue_id;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_PTR_VALID_CHECK(p_class);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_get_class(0, queue_id, p_class));

    return CTC_E_NONE;
}


/**
 @brief Set service queue drop scheme.
*/
int32
sys_humber_set_service_queue_drop(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_PTR_VALID_CHECK(p_drop);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (CTC_QUEUE_DROP_WRED == p_drop->mode)
        {
            CTC_ERROR_RETURN(sys_humber_queue_set_wred_weight(lchip, queue_id, p_drop->weight));
        }
        else
        {
            /*humber use oldEnqQueAvgDepth instead of newEnqQueInstDepth in tail drop, so we should configure factor
            to 0 to ensure oldEnqQueAvgDepth == newEnqQueInstDepth*/
            CTC_ERROR_RETURN(sys_humber_queue_set_wred_weight(lchip, queue_id, 0));
        }

        CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, p_drop));
    }

    return CTC_E_NONE;
}


/**
 @brief Get service queue drop configuration.
*/
int32
sys_humber_get_service_queue_drop(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_drop_t* p_drop)
{
    sys_service_config_t *p_service;
    uint16 queue_id;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_PTR_VALID_CHECK(p_drop);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_get_drop(0, queue_id, p_drop));
    if (CTC_QUEUE_DROP_WRED == p_drop->mode)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_wred_weight(0, queue_id, &p_drop->weight));
    }

    return CTC_E_NONE;
}



/**
 @brief Set service queue WDRR weight.
*/
int32
sys_humber_set_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint16 weight)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_queue_wdrr_weight(lchip, queue_id, weight));
    }

    return CTC_E_NONE;
}


/**
 @brief Get service queue WDRR weight.
*/
int32
sys_humber_get_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint16* p_weight)
{
    sys_service_config_t *p_service;
    uint16 queue_id;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_PTR_VALID_CHECK(p_weight);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));
    CTC_ERROR_RETURN(sys_humber_queue_get_queue_wdrr_weight(0, queue_id, p_weight));

    return CTC_E_NONE;
}



/**
 @brief Get service queue statistics.
*/
int32
sys_humber_get_service_queue_stats(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_stats_t* p_stats)
{
    sys_service_config_t *p_service;
    sys_stats_queue_t stats;
    uint16 queue_id;
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_QUEUE_DBG_FUNC();

    kal_memset(p_stats, 0, sizeof(ctc_queue_stats_t));

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_stats_get_queue_stats(lchip, queue_id, &stats));
        p_stats->drop_packets += stats.queue_drop_pkts;
        p_stats->drop_bytes   += stats.queue_drop_bytes;
        p_stats->deq_packets  += stats.queue_deq_pkts;
        p_stats->deq_bytes    += stats.queue_deq_bytes;
    }

    return CTC_E_NONE;
}



/**
 @brief Clear service queue statistics.
*/
int32
sys_humber_reset_service_queue_stats(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint8  lchip, lchip_num;
    uint8 ext_qdr_en = 0;

    SYS_QUEUE_INIT_CHECK();

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_MAX_VALUE_CHECK(qid, sys_humber_get_per_service_queue_num() - 1);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    if(!ext_qdr_en) /*do service queue stats in ext qdr*/
    {
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, qid, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_stats_reset_queue_stats(lchip, queue_id));
    }

    return CTC_E_NONE;
}

/**
 @add port queue mapping to channel
*/
int32
sys_humber_add_port_to_channel(uint16 gport, uint8 channel)
{
    uint8 lchip, lport, offset;
    uint16 queue_id = 0;
    ctc_queue_type_t queue_type = 0;
    uint8 queue_num = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));
    CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num));

    for(offset = 0; offset < queue_num; offset++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, offset, &queue_id));
        CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, channel));
    }

    return CTC_E_NONE;
}

/**
 @remove port queue from channel
*/
int32
sys_humber_remove_port_from_channel(uint16 gport, uint8 channel)
{
    uint8  lchip, lport, offset;
    uint16 queue_id = 0;
    ctc_queue_type_t queue_type = 0;
    uint8 queue_num = 0;
    uint8 orginal_channel = 0;

    SYS_QUEUE_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));
    CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num));

    if(lport < SYS_STATIC_INT_PORT_START)
    {
        orginal_channel = lport;
    }
    else
    {
        orginal_channel = SYS_DROP_CHANNEL_ID_START;
    }

    for(offset = 0; offset < queue_num; offset++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, offset, &queue_id));
        CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, channel));
        CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, orginal_channel));
    }

    return CTC_E_NONE;
}


/**
 @brief Enable/disable queue resource manage.
*/
int32
sys_humber_queue_resrc_mgr_global_enable(bool enable)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_queue_set_buf_store_resrc_mgr_enable(enable));

    return CTC_E_NONE;
}


/**
 @brief Get queue resource manage enable status.
*/
int32
sys_humber_queue_get_resrc_mgr_global_enable(bool* p_enable)
{
    SYS_QUEUE_INIT_CHECK();

    CTC_ERROR_RETURN(sys_humber_queue_get_buf_store_resrc_mgr_enable(p_enable));

    return CTC_E_NONE;
}

/**
 @brief set priority color map to queue select and drop_precedence.
*/
int32
sys_humber_set_queue_priority_map(ctc_queue_pri_map_t* p_queue_pri_map)
{
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_set_priority_queue_map(lchip, p_queue_pri_map));
    }

    return CTC_E_NONE;
}

/**
 @brief Set queue size mode: 1 -- buffer_cnt, 0 -- packet.
*/
int32
sys_humber_set_queue_size_mode(uint8 size_mode)
{
    uint8  lchip, lchip_num;

    SYS_QUEUE_INIT_CHECK();

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_queue_size_mode(lchip, size_mode));
    }

    return CTC_E_NONE;
}

/**
 @brief Queue initialization.
*/
int32
sys_humber_queue_init(ctc_queue_global_cfg_t* queue_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_queue_enq_init(queue_global_cfg));
    CTC_ERROR_RETURN(sys_humber_queue_drop_init());
    CTC_ERROR_RETURN(sys_humber_queue_shape_init());
    CTC_ERROR_RETURN(sys_humber_queue_sch_init());

    queue_init = 1;

    return CTC_E_NONE;
}

