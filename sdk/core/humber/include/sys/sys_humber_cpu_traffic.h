/**
 @file sys_humber_cpu_traffic.h

 @date 2010-01-13

 @version v2.0

 The file defines macro, data structure, and function for cpu traffic
*/

#ifndef _SYS_HUMBER_CPU_TRAFFIC_H_
#define _SYS_HUMBER_CPU_TRAFFIC_H_

#include "ctc_queue.h"
#include "ctc_debug.h"

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/
#define SYS_CPU_TRAFFIC_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(cpu, traffic, CPU_TRAFFIC_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_CPU_TRAFFIC_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(cpu, traffic, CPU_TRAFFIC_SYS); \
    }

#define SYS_BUILD_EXCP_DESTMAP_LOCAL(chip_id, index)  \
    (((chip_id) << 16) | (SYS_QSEL_TYPE_EXCP_CPU << 12) | ((index) & 0xFFF))

#define SYS_BUILD_EXCP_DESTMAP_LOCAL_PORT(chip_id, index)  \
    (((chip_id) << 16) | (SYS_QSEL_TYPE_REGULAR << 12) | ((index) & 0xFFF))

#define SYS_BUILD_EXCP_DESTMAP_REMOTE(chip_id, index)  \
    (((chip_id) << 16) | (SYS_QSEL_TYPE_SGMAC_CPU_TO_CPU << 12) | ((index) & 0xFFF))

#define SYS_CPU_TRAFFIC_DEFAULT_IND_RATE    75000   /* default individual reason rate, 600kbps */
#define SYS_CPU_TRAFFIC_DEFAULT_TOTAL_RATE  1250000 /* default total rate, 10Mbps */


#define SYS_SHOW_CPU_TRAFFIC_INFO_MODE_BRIEF    0
#define SYS_SHOW_CPU_TRAFFIC_INFO_MODE_DETAIL   1

#define IS_SUB_IDX_EN(excp) ((excp == CTC_EXCEPTION_INGRESS_2) || (excp == CTC_EXCEPTION_INGRESS_3) || \
                            (excp == CTC_EXCEPTION_INGRESS_7) || (excp == CTC_EXCEPTION_INGRESS_PARSER_PTP) || \
                            (excp == CTC_EXCEPTION_EGRESS_PARSER_PTP))


/*********************************************************************
 *
 * Data Structure
 *
 *********************************************************************/

extern uint8 sys_excp_max_sub_index[];
extern uint8 sys_excp_queue_num;

struct sys_cpu_traffic_excp_info_s
{
    uint8  class;
    uint32 rate;
    char   *name;
    char   *desp;
};
typedef struct sys_cpu_traffic_excp_info_s sys_cpu_traffic_excp_info_t;

struct sys_cpu_traffic_info_s
{
    sys_cpu_traffic_excp_info_t excp_info[MAX_CTC_EXCEPTION];

    uint32 sub_index_rate[5][16];
    uint8 sub_index_class[5][16];

    uint32 total_rate;
};
typedef struct sys_cpu_traffic_info_s sys_cpu_traffic_info_t;


/*********************************************************************
 *
 * Function Declaration
 *
 *********************************************************************/

/**
 @brief Mapping packet-to-CPU reason to a class level. Each packet-to-CPU reason corresponds to a queue,
        CPU queues among different classes get bandwidth in SP mode.
*/
extern int32
sys_humber_packet_tocpu_set_reason_class(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint8 class);


/**
 @brief Set the individual rate limit for the given packet-to-CPU reason.
*/
extern int32
sys_humber_packet_tocpu_limit_individual_rate(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint32 rate);


/**
 @brief Set the total rate limit for all packet-to-CPU reasons.
*/
extern int32
sys_humber_packet_tocpu_limit_total_rate(uint32 rate);


/**
 @brief Set shaping for the given queue in normal cpu forward.
*/
extern int32
sys_humber_cpu_traffic_set_fwd_queue_shape(uint8 qid, ctc_queue_shape_t* p_shape);


/**
 @brief Cancel shaping for the given queue in noraml cpu forward.
*/
extern int32
sys_humber_cpu_traffic_unset_fwd_queue_shape(uint8 qid);

/**
 @brief Set class for the given queue in normal cpu forward.
*/
int32
sys_humber_cpu_traffic_set_fwd_queue_class(uint8 qid, uint8 class);


/**
 @brief Get exception queue number.
*/
extern uint8
sys_humber_get_excp_queue_num(void);

/**
 @brief Get exception queue offset.
*/
extern int32
sys_humber_get_excp_queue_offset(ctc_packet_tocpu_reason_t reason, uint16* p_queue_id);

/**
 @brief Set exception to local cpu, remote port or drop.
*/
extern int32
sys_humber_cpu_traffic_set_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type);

/**
 @brief Set fatal exception to local cpu, remote port or drop.
*/
extern int32
sys_humber_cpu_traffic_set_fatal_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type);

/**
 @brief Show exception info.
*/
extern int32
sys_humber_cpu_traffic_show_exception(uint8 excp);

/**
 @brief Show fatal exception info .
*/
extern int32
sys_humber_cpu_traffic_show_fatal_exception(uint8 excp);

/**
 @brief Show cpu traffic info.
*/
extern int32
sys_humber_show_cpu_traffic_info(uint8 mode);


/**
 @brief CPU traffic initialization.
*/
extern int32
sys_humber_cpu_traffic_init(void);

#endif

