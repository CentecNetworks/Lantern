/**
 @file ctc_humber_usrid.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-9-25

 @version v2.0

 The file provide APIs of UserId
*/
#ifndef _CTC_HUMBER_USRID_H
#define _CTC_HUMBER_USRID_H


/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
/**
 @addtogroup usrid USRID
 @{
*/

/**
 @brief Initialize the usrid module

 @return CTC_E_XXX
*/
extern int32
ctc_humber_usrid_init(void* usrid_global_cfg);

/**@} end of @addgroup usrid*/

#endif

