/**
 @file ctc_humber_register.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-1-7

 @version v2.0

Initialize driver ,and  write default value to common table/register

*/

#ifndef _CTC_HUMBER_REGISTER
#define _CTC_HUMBER_REGISTER

#include "kal.h"
#include "ctc_register.h"


/**
 @brief  Set global control info

 @param[in] type    a type of global control

 @param[in] value   the value to be set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_global_ctl_set(ctc_global_control_type_t type, void* value);

/**
 @brief  Get global control info

 @param[in] type    a type of global control

 @param[out] value   the value to be get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_global_ctl_get(ctc_global_control_type_t type, void* value);

/**
 @brief    Initialize driver ,and  write default value to common table/register

 @return CTC_E_XXX

*/
extern int32
ctc_humber_register_init(void);

#endif

