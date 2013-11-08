/**
 @file sys_humber_internal_port.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-03-29

 @version v2.0

 The file defines macro, data structure, and function for internal port
*/

#ifndef _SYS_HUMBER_INTERNAL_PORT_H_
#define _SYS_HUMBER_INTERNAL_PORT_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_internal_port.h"

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/

#define SYS_RESERVED_INTERNAL_PORT_FOR_DROP 56  /**< It's value is same as SYS_STATIC_INT_PORT_START, but queue select type is different */

#define SYS_HUMBER_INTERNAL_PORT_START 64

#define SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_START 56
#define SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_NUM 8

enum ctc_internal_port_eloop_type_e
{
    CTC_INTERNAL_PORT_ELOOP_TYPE_ROUTE,    /**< E-I-Loopback port for route */
    CTC_INTERNAL_PORT_ELOOP_TYPE_BRIDGE,    /**< E-I-Loopback  port for bridge */
    CTC_INTERNAL_PORT_ELOOP_TYPE_MAX
};
typedef enum ctc_internal_port_eloop_type_e ctc_internal_port_eloop_type_t;

struct sys_inter_port_master_s
{
    uint32 is_used[2][MAX_PORT_NUM_PER_CHIP/BITS_NUM_OF_WORD];    /*bitmap for internal port*/
    uint8  rsv_port_bitmap[2]; /*for static reserved internal port(56 - 63)*/
    uint8  rsv1[2];
    uint16 eloop_port[2][CTC_INTERNAL_PORT_ELOOP_TYPE_MAX];
};
typedef struct sys_inter_port_master_s sys_inter_port_master_t;

/*********************************************************************
 *
 * Function
 *
 *********************************************************************/

/**
 @brief Set internal port for special usage, for example, I-Loop, E-Loop.
*/
extern int32
sys_humber_internal_port_set(ctc_internal_port_assign_para_t* port_assign);

/**
 @brief Allocate internal port for special usage, for example, I-Loop, E-Loop.
*/
extern int32
sys_humber_internal_port_allocate(ctc_internal_port_assign_para_t* port_assign);


/**
 @brief Release internal port.
*/
extern int32
sys_humber_internal_port_release(ctc_internal_port_assign_para_t* port_assign);


/**
 @brief Get eloop internal port.
*/
extern int32
sys_humber_internal_port_get_eloop_port(uint8 type, uint8 lchip, uint16 *p_gport);

/**
 @brief Get reserve static internal port
*/
extern int32
sys_humber_get_rsv_internal_port(uint8 lchip, uint8* p_inter_port);


/**
 @brief Release istatic internal port
*/
extern int32
sys_humber_free_rsv_internal_port(uint8 lchip, uint8 inter_port);


/**
 @brief Internal port initialization.
*/
extern int32
sys_humber_internal_port_init(void);

#endif

