/**
 @file ctc_humber_interrupt.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-1-20

 @version v2.0

   This file define ctc functions.
*/


/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"
#include "ctc_humber_interrupt.h"
#include "sys_humber_interrupt.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief Init interrupt register start, should before sdk moudle init

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_reg_init_start(void)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_reg_init_start());

    return CTC_E_NONE;
}

/**
 @brief Init interrupt register end, should after sdk moudle init

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_reg_init_end(void)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_reg_init_end());

    return CTC_E_NONE;
}

/**
 @brief Get normal interrupt type enable

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[out] p_enable  a boolean value denote normal type interrupt is enable

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_get_normal_intr_status(uint8 lchip, uint8 type, bool* p_enable)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_get_normal_intr_status(lchip, type, p_enable));

    return CTC_E_NONE;
}

/**
 @brief Get all normal interrupt status

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type
 @param[out] p_bitmap  all normal interrupt status

 @return CTC_E_XXX

*/
extern int32
ctc_humber_interrupt_get_normal_all_intr_status(uint8 lchip,  uint32* p_bitmap)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_get_all_normal_intr_status(lchip,  p_bitmap));

    return CTC_E_NONE;
}

/**
 @brief Enable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_enable_normal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_enable_normal_intr(lchip, type));

    return CTC_E_NONE;
}

/**
 @brief Disable normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_disable_normal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_disable_normal_intr(lchip, type));

    return CTC_E_NONE;
}

/**
 @brief Clear normal interrupt

 @param[in] lchip  local chip id
 @param[in] type  normal interrupt type

 @return CTC_E_XXX

*/
int32
ctc_humber_interrupt_clear_normal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_clear_normal_intr(lchip, type));

    return CTC_E_NONE;
}

int32
ctc_humber_interrupt_enable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_enable_sub_normal_intr(lchip, type, bit_offset));

    return CTC_E_NONE;
}

int32
ctc_humber_interrupt_disable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_disable_sub_normal_intr(lchip, type, bit_offset));

    return CTC_E_NONE;
}

int32
ctc_humber_interrupt_clear_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_clear_sub_normal_intr(lchip, type, bit_offset));

    return CTC_E_NONE;
}



/**
 @brief Get special fatal  interrupt status
*/
extern int32
ctc_humber_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t* status)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_get_fatal_intr_status(lchip, type, status));

    return CTC_E_NONE;
}

/**
 @brief Enable fatal interrupt
*/
int32
ctc_humber_interrupt_enable_fatal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_enable_fatal_intr(lchip, type));

    return CTC_E_NONE;
}

/**
 @brief Disable fatal interrupt
*/
int32
ctc_humber_interrupt_disable_fatal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_disable_fatal_intr(lchip, type));

    return CTC_E_NONE;
}

/**
 @brief Clear fatal interrupt
*/
int32
ctc_humber_interrupt_clear_fatal_intr(uint8 lchip, uint8 type)
{
    CTC_ERROR_RETURN(sys_humber_interrupt_clear_fatal_intr(lchip, type));

    return CTC_E_NONE;
}


