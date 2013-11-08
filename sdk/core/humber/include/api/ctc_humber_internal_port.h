/**
 @file ctc_humber_internal_port.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-03-29

 @version v2.0

   The file apply APIs to initialize,allocation,release internal port.
*/


#ifndef _CTC_HUMBER_INTERNAL_PORT_H_
#define _CTC_HUMBER_INTERNAL_PORT_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_internal_port.h"

/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @addtogroup internal_port INTERNAL_PORT
 @{
*/


/**
 @brief     Set internal port for special usage

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
extern int32
ctc_humber_internal_port_set(ctc_internal_port_assign_para_t* port_assign);

/**
 @brief     Allocate internal port for special usage

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
extern int32
ctc_humber_internal_port_allocate(ctc_internal_port_assign_para_t* port_assign);

/**
 @brief     release internal port.

 @param[in] port_assign  Internal port parameters

 @return    CTC_E_XXX

*/
extern int32
ctc_humber_internal_port_release(ctc_internal_port_assign_para_t* port_assign);

/**
 @brief     Initialize internal port.

 @return    CTC_E_XXX

*/
extern int32
ctc_humber_internal_port_init(void);

/**@} end of @addtogroup internal_port INTERNAL_PORT  */


#endif

