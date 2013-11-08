/**
 @file ctc_humber_internal_port.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-03-29

 @version v2.0

   The file apply APIs to initialize,allocation,release internal port.
*/


/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_error.h"

#include "ctc_humber_internal_port.h"
#include "sys_humber_internal_port.h"

/****************************************************************************
 *
 * Function
 *
 *****************************************************************************/

/**
 @brief     Set internal port for special usage

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
int32
ctc_humber_internal_port_set(ctc_internal_port_assign_para_t* port_assign)
{
    CTC_ERROR_RETURN(sys_humber_internal_port_set(port_assign));

    return CTC_E_NONE;
}


/**
 @brief     Allocate internal port for special usage

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
int32
ctc_humber_internal_port_allocate(ctc_internal_port_assign_para_t* port_assign)
{
    CTC_ERROR_RETURN(sys_humber_internal_port_allocate(port_assign));

    return CTC_E_NONE;
}


/**
 @brief     release internal port.

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
int32
ctc_humber_internal_port_release(ctc_internal_port_assign_para_t* port_assign)
{
    CTC_ERROR_RETURN(sys_humber_internal_port_release(port_assign));

    return CTC_E_NONE;
}


/**
 @brief     Initialize internal port.

 @return    CTC_E_XXX

*/
int32
ctc_humber_internal_port_init(void)
{
    CTC_ERROR_RETURN(sys_humber_internal_port_init());

    return CTC_E_NONE;
}

