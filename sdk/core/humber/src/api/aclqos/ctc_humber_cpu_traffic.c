/**
 @file ctc_humber_cpu_traffic.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-01-13

 @version v2.0

   The file provide all CPU traffic related APIs of Humber SDK.
*/

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ctc_error.h"
#include "ctc_cpu_traffic.h"
#include "ctc_queue.h"

#include "sys_humber_cpu_traffic.h"


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
 @brief Mapping packet-to-CPU reason to a class level. Each packet-to-CPU reason corresponds to a queue,
        CPU queues among different classes get bandwidth in SP mode.

 @param[in] reason   The reason of a packet being sent to CPU

 @param[in] sub_idx  Sub index of the reason

 @param[in] class    The class of a CPU reason

 @return CTC_E_XXX
*/
int32
ctc_humber_packet_tocpu_set_reason_class(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint8 class)
{
    CTC_ERROR_RETURN(sys_humber_packet_tocpu_set_reason_class(reason, sub_idx, class));

    return CTC_E_NONE;
}


/**
 @brief Set the individual rate limit for the given packet-to-CPU reason.

 @param[in] reason   The reason of a packet being sent to CPU

 @param[in] sub_idx  Sub index of the reason

 @param[in] rate     The peak rate for the given reason and sub-index

 @return CTC_E_XXX
*/
int32
ctc_humber_packet_tocpu_limit_individual_rate(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint32 rate)
{
    CTC_ERROR_RETURN(sys_humber_packet_tocpu_limit_individual_rate(reason, sub_idx, rate));

    return CTC_E_NONE;
}



/**
 @brief Set the total rate limit for all packet-to-CPU reasons.

 @param[in] rate     The total rate for all packet-to-CPU reasons.

 @return CTC_E_XXX
*/
int32
ctc_humber_packet_tocpu_limit_total_rate(uint32 rate)
{
    CTC_ERROR_RETURN(sys_humber_packet_tocpu_limit_total_rate(rate));

    return CTC_E_NONE;
}

/**
 @brief Set shaping for the given queue in normal cpu forward.

 @param[in]  qid      Queue ID in the cpu port

 @param[in]  p_shape  Queue shape configuration

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_set_fwd_queue_shape(uint8 qid, ctc_queue_shape_t* p_shape)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_fwd_queue_shape(qid, p_shape));

    return CTC_E_NONE;
}

/**
 @brief Cancel shaping for the given queue in noraml cpu forward.

 @param[in]  qid      Queue ID in the cpu port

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_unset_fwd_queue_shape(uint8 qid)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_unset_fwd_queue_shape(qid));

    return CTC_E_NONE;
}

/**
 @brief Set class for the given queue in normal cpu forward.

 @param[in]  qid      Queue ID in the cpu port

 @param[in]  class    Class value

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_set_fwd_queue_class(uint8 qid, uint8 class)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_fwd_queue_class(qid, class));

    return CTC_E_NONE;
}

/**
 @brief Set exception to local cpu, remote port or drop.

 @param[in] excp  the exception of ctc_packet_tocpu_reason_t except fatal exception

 @param[in] dest_id  dest_type ==CTC_EXCP_DEST_TO_LOCAL_PORT or CTC_EXCP_DEST_TO_REMOTE_PORT indicate gloal port id,
                     else dest_type == CTC_EXCP_DEST_TO_LOCAL_CPU indicate cpu mac index

 @param[in] dest_type  ctc_excp_dest_type_t(dest type to local cpu, remote port or drop).

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_set_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_exception(excp, dest_id, dest_type));

    return CTC_E_NONE;
}

/**
 @brief Set fatal exception to local cpu, remote port or drop.

 @param[in] excp  fatal exception index.

 @param[in] dest_id  dest_type ==CTC_EXCP_DEST_TO_LOCAL_PORT or CTC_EXCP_DEST_TO_REMOTE_PORT indicate gloal port id,
                     else dest_type == CTC_EXCP_DEST_TO_LOCAL_CPU indicate cpu mac index

 @param[in] dest_type ctc_excp_dest_type_t(dest type to local cpu, remote port or drop).

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_set_fatal_exception( uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_fatal_exception(excp, dest_id, dest_type));

    return CTC_E_NONE;
}

/**
 @brief CPU traffic initialization.

 @return CTC_E_XXX
*/
int32
ctc_humber_cpu_traffic_init(void)
{
    CTC_ERROR_RETURN(sys_humber_cpu_traffic_init());

    return CTC_E_NONE;
}


