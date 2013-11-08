/**
 @file sys_humber_queue_api.h

 @date 2010-01-13

 @version v2.0

 The file defines queue api
*/

#ifndef _SYS_HUMBER_QUEUE_API_H_
#define _SYS_HUMBER_QUEUE_API_H_

#include "ctc_queue.h"
/****************************************************************************
 *
 * Macro
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Function Declaration
 *
 ****************************************************************************/


/**
 @brief Configure queue drop scheme for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.
*/
extern int32
sys_humber_set_port_queue_drop(uint16 gport, uint8 offset, ctc_queue_drop_t* p_drop);


/**
 @brief Get queue drop config for the given queue in a port. WTD and WRED drop mechanisms are supported.
        For either drop mechanism, there are four drop precedences: red-, yellow-, green-, and none-colored
        are associated to each drop precedence, respectively.
*/
extern int32
sys_humber_get_port_queue_drop(uint16 gport, uint8 qid, ctc_queue_drop_t* p_drop);


/**
 @brief To globally enable/disable QoS flow ID.
*/
extern int32
sys_humber_qos_flow_id_global_enable(bool enable);


/**
 @brief Get QoS flow ID global enable status.
*/
extern int32
sys_humber_qos_get_flow_id_global_enable(bool* p_enable);


/**
 @brief Globally enable/disable queue shaping function.
*/
extern int32
sys_humber_queue_shape_global_enable(bool enable);


/**
 @brief Get queue shape global enable status.
*/
extern int32
sys_humber_get_queue_shape_global_enable(bool* p_enable);


/**
 @brief Globally enable/disable group shaping function.
*/
extern int32
sys_humber_group_shape_global_enable(bool enable);


/**
 @brief Get group shape global enable status.
*/
extern int32
sys_humber_get_group_shape_global_enable(bool* p_enable);


extern int32
sys_humber_queue_shape_ipg_global_enable(bool enable);

/**
 @brief Set shaping for the given queue in a port. Queue shaping supports two-rate two token bucket algorithm.
        Traffic rate below CIR is viewed as in-profile, between CIR and PIR is viewed as out-profile, and above
        PIR is backlogged in the queue and get dropped if the queue overflows.
*/
extern int32
sys_humber_set_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape);


/**
 @brief Cancel shaping for the given queue in a port.
*/
extern int32
sys_humber_unset_port_queue_shape(uint16 gport, uint8 qid);


/**
 @brief Get shaping parameters for the given queue in a port.
*/
extern int32
sys_humber_get_port_queue_shape(uint16 gport, uint8 qid, ctc_queue_shape_t* p_shape);


/**
 @brief Get total number of queue shape profile.
*/
extern int32
sys_humber_get_port_queue_profile_num(uint32* p_shape_num);


/**
 @brief Globally enable/disable channel shaping function.
*/
extern int32
sys_humber_channel_shape_global_enable(bool enable);


/**
 @brief Get channel shape global enable status.
*/
extern int32
sys_humber_get_channel_shape_global_enable(bool* p_enable);


/**
 @brief Set shaping for the given port. Port shaping supports single-rate single bucket algorithm. Whether
        a packet can be scheduled is determined by whether there are enough tokens in the bucket.
*/
extern int32
sys_humber_set_port_shape(uint16 gport, ctc_port_shape_t* p_shape);


/**
 @brief Cancel port shaping for the given port.
*/
extern int32
sys_humber_unset_port_shape(uint16 gport);


/**
 @brief Get shaping parameters for the given port.
*/
extern int32
sys_humber_get_port_shape(uint16 gport, ctc_port_shape_t* p_shape);

/**
 @brief Mapping the given port queue to a class. Queues in the different classes are serviced by SP scheduling
        mode, and queues in the same class are serviced by WDRR scheduling mode.
*/
extern int32
sys_humber_set_port_queue_class(uint16 gport, uint8 qid, uint8 class);


/**
 @brief Get class level for the given port queue.
*/
extern int32
sys_humber_get_port_queue_class(uint16 gport, uint8 qid, uint8* p_class);


/**
 @brief Set port queue DRR weight. Queues in the same class are scheduled by WDRR algorithm. The weight
        influences the proportion of the shared bandwidth among queues.
*/
extern int32
sys_humber_set_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16 weight);


/**
 @brief Get port queue DRR weight.
*/
extern int32
sys_humber_get_port_queue_wdrr_weight(uint16 gport, uint8 qid, uint16* p_weight);


/**
 @brief set queue wdrr weight mtu.
*/
extern int32
sys_humber_set_queue_wdrr_weight_mtu(uint32 mtu);


/**
 @brief To globally enable/disable queue statistics function.
*/
extern int32
sys_humber_queue_stats_global_enable(bool enable);


/**
 @brief To get queue statistics enable status.
*/
extern int32
sys_humber_get_queue_stats_enable(bool* p_enable);



/**
 @brief Get statistics for the given queue in the specific port.
*/
extern int32
sys_humber_get_port_queue_stats(uint16 gport, uint8 qid, ctc_queue_stats_t* p_stats);


/**
 @brief Clear statistics for the given queue in the specific port.
*/
extern int32
sys_humber_reset_port_queue_stats(uint16 gport, uint8 qid);


/*~~~~~~~~~~~~~~~~~~~~~~~service queue APIs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 @brief Get service queue enable status.
*/
extern int32
sys_humber_get_service_queue_enable(bool* p_enable);


/**
 @brief Get per-service queue number.
*/
extern int32
sys_humber_get_service_queue_num(uint8* p_que_num);


/**
 @brief Create a service.
*/
extern int32
sys_humber_create_service(uint16 logical_service_id, uint8 dest_port);


/**
 @brief remove a service.
*/
extern int32
sys_humber_remove_service(uint16 logical_service_id);


/**
 @brief Register a service.
*/
extern int32
sys_humber_register_service(uint16 logical_service_id);


/**
 @brief Register a service.
*/
extern int32
sys_humber_unregister_service(uint16 logical_service_id);


/**
 @brief Get physical service ID by logical ID.
*/
extern int32
sys_humber_get_physical_service_id(uint16 logical_service_id, uint16* p_physical_service_id);


/**
 @brief Set queue shape for the given service.
*/
extern int32
sys_humber_set_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_shape_t* p_shape);


/**
 @brief Cancel queue shape for the given service.
*/
extern int32
sys_humber_unset_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid);


/**
 @brief Get queue shape configuration for the given service.
*/
extern int32
sys_humber_get_service_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_shape_t* p_shape);


/**
 @brief Set group shape.
*/
extern int32
sys_humber_set_group_shape(ctc_group_shape_cfg_t* group_shape);


/**
 @brief Cancel group shape.
*/
extern int32
sys_humber_unset_group_shape(ctc_group_shape_cfg_t* group_shape);


/**
 @brief Get group shape.
*/
extern int32
sys_humber_get_group_shape(ctc_group_shape_cfg_t* group_shape);


/**
 @brief Set service queue class mapping.
*/
extern int32
sys_humber_set_service_queue_class(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint8 class);


/**
 @brief Get service queue class mapping.
*/
extern int32
sys_humber_get_service_queue_class(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint8* p_class);


/**
 @brief Set service queue drop scheme.
*/
extern int32
sys_humber_set_service_queue_drop(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_drop_t* p_drop);


/**
 @brief Get service queue drop configuration.
*/
extern int32
sys_humber_get_service_queue_drop(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_drop_t* p_drop);



/**
 @brief Set service queue WDRR weight.
*/
extern int32
sys_humber_set_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint16 weight);


/**
 @brief Get service queue WDRR weight.
*/
extern int32
sys_humber_get_service_queue_wdrr_weight(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, uint16* p_weight);



/**
 @brief Get service queue statistics.
*/
extern int32
sys_humber_get_service_queue_stats(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid, ctc_queue_stats_t* p_stats);



/**
 @brief Clear service queue statistics.
*/
extern int32
sys_humber_reset_service_queue_stats(ctc_queue_type_t type, uint16 logical_service_id, uint8 qid);

/**
 @brief Add port queue to channel
*/
extern int32
sys_humber_add_port_to_channel(uint16 gport, uint8 channel);

/**
 @brief Remove port queue from channel
*/
extern int32
sys_humber_remove_port_from_channel(uint16 gport, uint8 channel);

/**
 @brief Enable/disable queue resource manage.
*/
extern int32
sys_humber_queue_resrc_mgr_global_enable(bool enable);

/**
 @brief Get queue resource manage enable status.
*/
extern int32
sys_humber_queue_get_resrc_mgr_global_enable(bool* p_enable);

/**
 @brief set priority color map to queue select and drop_precedence.
*/
extern int32
sys_humber_set_queue_priority_map(ctc_queue_pri_map_t* p_queue_pri_map);

/**
 @brief Set queue size mode: 1 -- buffer_cnt, 0 -- packet.
*/
extern int32
sys_humber_set_queue_size_mode(uint8 szie_mode);

/**
 @brief Queue initialization.
*/
extern int32
sys_humber_queue_init(ctc_queue_global_cfg_t* queue_global_cfg);

#endif

