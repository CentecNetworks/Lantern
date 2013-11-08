/**
 @file ctc_humber_interrupt.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-1-20

 @version v2.0

   This file define ctc functions.
*/


#ifndef _CTC_HUMBER_INTERRUPT_H
#define _CTC_HUMBER_INTERRUPT_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_interrupt.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @addtogroup isr ISR
 @{
*/

/**
 @brief Init interrupt register start, should before sdk moudle init

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_reg_init_start(void);

/**
 @brief Init interrupt register end, should after sdk moudle init

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_reg_init_end(void);

/**
 @brief Get all normal interrupt status

 @param[in] lchip  local chip id
 @param[out] p_bitmap  all normal interrupt status

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_get_normal_all_intr_status(uint8 lchip, uint32* p_bitmap);


/**
 @brief Get special normal  interrupt status

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[out] p_enable  special normal  interrupt status

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_get_normal_intr_status(uint8 lchip, uint8 type, bool* p_enable);

/**
 @brief Enable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_enable_normal_intr(uint8 lchip, uint8 type);

/**
 @brief Disable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_disable_normal_intr(uint8 lchip, uint8 type);

/**
 @brief Clear normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_clear_normal_intr(uint8 lchip, uint8 type);

/**
 @brief Enable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[in] bit_offset  bit offset

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_enable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

/**
 @brief Disable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[in] bit_offset  bit offset

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_disable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

/**
 @brief Clear normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[in] bit_offset  bit offset

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_clear_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

/**
 @brief Get special fatal  interrupt status

 @param[in] lchip  local chip id
 @param[in] type  fatal interrupt type
 @param[out] status  special fatal  interrupt status and action

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t* status);

/**
 @brief Enable fatal interrupt

 @param[in] lchip  local chip id
 @param[in] type  fatal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_enable_fatal_intr(uint8 lchip, uint8 type);

/**
 @brief Disable fatal interrupt

 @param[in] lchip  local chip id
 @param[in] type  fatal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_disable_fatal_intr(uint8 lchip, uint8 type);

/**
 @brief Clear fatal interrupt

 @param[in] lchip  local chip id
 @param[in] type  fatal interrupt type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_clear_fatal_intr(uint8 lchip, uint8 type);


/**@} end of @addtogroup isr ISR  */

#endif

