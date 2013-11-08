/**
 @file sys_humber_queue_sch.h

 @date 2010-01-13

 @version v2.0

 The file defines macro, data structure, and function for queue scheduling
*/

#ifndef _SYS_HUMBER_QUEUE_SCH_H_
#define _SYS_HUMBER_QUEUE_SCH_H_

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/
#define SYS_QUEUE_MAX_DRR_WEIGHT        0xFFFFFF

/*********************************************************************
 *
 * Data Structure
 *
 *********************************************************************/


/*********************************************************************
 *
 * Function Declaration
 *
 *********************************************************************/


/**
 @brief Set class for the given queue in a chip.
*/
extern int32
sys_humber_queue_set_class(uint8 lchip, uint16 queue_id, uint8 class);


/**
 @brief Set class for the given queue in a chip.
*/
extern int32
sys_humber_queue_get_class(uint8 lchip, uint16 queue_id, uint8* p_class);


/**
 @brief Set DRR weight for the given queue in a chip.
*/
extern int32
sys_humber_queue_set_queue_wdrr_weight(uint8 lchip, uint16 queue_id, uint16 weight);


/**
 @brief Get DRR weight for the given queue in a chip.
*/
extern int32
sys_humber_queue_get_queue_wdrr_weight(uint8 lchip, uint16 queue_id, uint16* p_weight);

/**
 @brief Set RR weight for the given channel in a chip.
*/
extern int32
sys_humber_queue_set_channel_wrr_weight(uint8 lchip, uint8 channel, uint16 weight);


/**
 @brief Queue scheduler initialization.
*/
extern int32
sys_humber_queue_sch_init(void);


#endif

