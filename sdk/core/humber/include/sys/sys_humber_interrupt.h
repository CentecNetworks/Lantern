/**
 @file sys_humber_interrupt.h

 @date 2010-1-20

 @version v2.0

*/
#ifndef _SYS_HUMBER_INTERRUPT_H
#define _SYS_HUMBER_INTERRUPT_H

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
 #include "ctc_debug.h"
#include "ctc_const.h"


/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define SYS_INTERRUPT_DBG_INFO(FMT, ...)                          \
                    {                                                      \
                        CTC_DEBUG_OUT_INFO(interrupt, interrupt, INTERRUPT_SYS,FMT,##__VA_ARGS__);\
                    }

#define SYS_INTERRUPT_DBG_FUNC()                          \
                    {\
                        CTC_DEBUG_OUT_FUNC(interrupt, interrupt, INTERRUPT_SYS);\
                    }

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/


extern int32
sys_humber_interrupt_reg_init_start(void);

extern int32
sys_humber_interrupt_reg_init_end(void);

extern int32
sys_humber_interrupt_get_all_normal_intr_status(uint8 lchip,  uint32* p_bitmap);

extern int32
sys_humber_interrupt_get_normal_intr_status(uint8 lchip, uint8 type, bool* p_enable);

extern int32
sys_humber_interrupt_enable_normal_intr(uint8 lchip, uint8 type);

extern int32
sys_humber_interrupt_disable_normal_intr(uint8 lchip, uint8 type);

extern int32
sys_humber_interrupt_clear_normal_intr(uint8 lchip, uint8 type);

extern int32
sys_humber_interrupt_enable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

extern int32
sys_humber_interrupt_disable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

extern int32
sys_humber_interrupt_clear_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset);

extern int32
sys_humber_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t* status);

extern int32
sys_humber_interrupt_enable_fatal_intr(uint8 lchip, uint8 type);

extern int32
sys_humber_interrupt_disable_fatal_intr(uint8 lchip, uint8 type);

extern int32
sys_humber_interrupt_clear_fatal_intr(uint8 lchip, uint8 type);

#endif

