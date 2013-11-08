/**
 @file sys_humber_queue_drop.h

 @date 2010-01-13

 @version v2.0

 The file defines macro, data structure, and function for queue dropping
*/

#ifndef _SYS_HUMBER_QUEUE_DROP_H_
#define _SYS_HUMBER_QUEUE_DROP_H_

#include "ctc_queue.h"

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/

#define SYS_DROP_COUNT_GE       0x38
#define SYS_DROP_DELTA_GE       0x4
#define SYS_DROP_COUNT_XGE      0x40
#define SYS_DROP_DELTA_XGE      0x10

/*********************************************************************
 *
 * Data Structure
 *
 *********************************************************************/

/**
 @brief Default queue drop profiles, these default profiles will persistently exist and won't be remove.
*/
enum sys_default_queue_drop_type_e
{
    SYS_DEFAULT_DROP_DISCARD = 0,
    SYS_DEFAULT_DROP_GE,
    SYS_DEFAULT_DROP_XGE,

    MAX_SYS_DEFAULT_DROP
};
typedef enum sys_default_queue_drop_type_e sys_default_queue_drop_type_t;


/**
 @brief Queue drop profile
*/
struct sys_queue_drop_profile_s
{
    ctc_list_pointer_node_t head;

    uint16 min_th[CTC_DROP_PREC_NUM];    /**< WRED min threshold */
    uint16 max_th[CTC_DROP_PREC_NUM];    /**< WRED max threshold, or WTD drop threshold */
    uint16 drop_prob[CTC_DROP_PREC_NUM]; /**< WRED max drop probability */

    uint8  mode;     /**< Queue Drop Mode, type of CTC_QUEUE_DROP_XXX */
    uint16 ref;      /**< reference counter */
    uint8  index;    /**< drop profile index */
};
typedef struct sys_queue_drop_profile_s sys_queue_drop_profile_t;


/*********************************************************************
 *
 * Function Declaration
 *
 *********************************************************************/

/**
 @brief Get default queue drop.
*/
extern int32
sys_humber_queue_get_default_drop(sys_default_queue_drop_type_t type, ctc_queue_drop_t* p_drop);


/**
 @brief Set a drop scheme for the given queue in a chip. WTD and WRED drop schemes are supported.
        For WTD, only threshold for each drop precedence need to be configured. For WRED,
        parameters include min-threshold, max-threshold, drop-probability, and
        exponential-weighted-moving-average (EWMA) constant.
*/
extern int32
sys_humber_queue_set_drop(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop);

/**
 @set drop for oam loopback
*/
extern int32
sys_humber_queue_set_drop_oam_lpk(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop);


/**
 @brief Get the drop paramters for the given queue in a chip.
*/
extern int32
sys_humber_queue_get_drop(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop);


/**
 @brief Set WRED Exponential-Weighted-Moving-Average (EWMA) factor.
*/
extern int32
sys_humber_queue_set_wred_weight(uint8 lchip, uint16 queue_id, uint8 weight);

/**
 @brief Get WRED Exponential-Weighted-Moving-Average (EWMA) factor.
*/
int32
sys_humber_queue_get_wred_weight(uint8 lchip, uint16 queue_id, uint8* p_weight);

/**
 @brief Set queue size mode: 1 -- buffer_cnt, 0 -- packet.
*/
extern int32
sys_humber_queue_set_queue_size_mode(uint8 lchip, uint8 size_mode);

/**
 @brief QoS queue drop component initialization.
*/
extern int32
sys_humber_queue_drop_init(void);

#endif

