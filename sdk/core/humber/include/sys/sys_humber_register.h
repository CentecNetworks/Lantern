/**
 @file sys_humber_register.h

 @date 2009-11-3

 @version v2.0

Macro, data structure for system common operations

*/

#ifndef _SYS_HUMBER_REGISTER
#define _SYS_HUMBER_REGISTER

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_register.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define SYS_HUMBER_DSNH_INTERNAL_BASE 0xFFFF
#define SYS_HUMBER_DSNH_INTERNAL_SHIFT      2

#define SYS_HUMBER_EDIT_PTR_TYPE_DEFAULT        (SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_16BIT)
#define SYS_HUMBER_EDIT_PTR_DEFAULT_BIT_LENGTH  15
#define SYS_HUMBER_EDIT_PTR_DEFAULT_BIT_SHIFT   12
#define SYS_HUMBER_EDIT_PTR_DEFAULT_BIT_MASK    0xFFF

enum sys_humber_edit_ptr_type_e
{
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_12BIT = 0,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_10BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_11BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_13BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_14BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_15BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_16BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_17BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_18BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_19BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_20BIT,
    SYS_HUMBER_EDIT_PTR_TYPE_L2EDIT_MAX,
};
typedef enum sys_humber_edit_ptr_type_e sys_humber_edit_ptr_type_t;

struct sys_global_control_db_s
{
    uint32 service_policer_num; /**< global service policer num */
    uint8  is_phb_support;      /**< 1: support, 0: not support */
    uint8  stats_mode;          /**< CTC_GLOBAL_STATS_XXX */
};
typedef struct sys_global_control_db_s sys_global_control_db_t;

/****************************************************************************
*
* Function
*
*****************************************************************************/
extern int32
sys_humber_global_ctl_set(ctc_global_control_type_t type, void* value);

extern int32
sys_humber_global_ctl_get(ctc_global_control_type_t type, void* value);

extern int32
sys_humber_register_init(void);

#endif

