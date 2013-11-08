/**
 @file ctc_humber_register.c

 @date 2009-11-6

 @version v2.0

 The file apply APIs to initialize common register of humber
*/

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_error.h"
#include "ctc_humber_register.h"
#include "sys_humber_register.h"

/****************************************************************************
 *
 * Function
 *
 *****************************************************************************/

/**
 @brief  Set global control info

 @param[in] type    a type of global control

 @param[in] value   the value to be set

 @return CTC_E_XXX

*/
int32 ctc_humber_global_ctl_set(ctc_global_control_type_t type, void* value)
{

   CTC_ERROR_RETURN(sys_humber_global_ctl_set(type, value));

   return CTC_E_NONE;
}

/**
 @brief  Get global control info

 @param[in] type    a type of global control

 @param[out] value   the value to be get

 @return CTC_E_XXX

*/
int32 ctc_humber_global_ctl_get(ctc_global_control_type_t type, void* value)
{

   CTC_ERROR_RETURN(sys_humber_global_ctl_get(type, value));

   return CTC_E_NONE;
}

int32 ctc_humber_register_init(void)
{

   CTC_ERROR_RETURN(sys_humber_register_init());

   return CTC_E_NONE;
}


