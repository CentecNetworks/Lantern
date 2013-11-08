/**
 @file ctc_humber_alloc.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-11

 @version v2.0

   This file define ctc functions of SDK
*/

#ifndef _CTC_HUMBER_FTM_H
#define _CTC_HUMBER_FTM_H


/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @addtogroup allocation FTM
 @{
*/

/**
 @brief Profile information
 @return CTC_E_XXX
*/
extern int32
ctc_humber_ftm_show_alloc_info(void);

/**
 @brief Profile information

 @param[in] ctc_profile_info  allocation profile information

 @return CTC_E_XXX
*/
extern int32
ctc_humber_ftm_mem_alloc(ctc_ftm_profile_info_t* ctc_profile_info);

/**
 @brief Profile information

 @param[in] ctc_profile_info  allocation profile information

 @return CTC_E_XXX
*/
extern int32
ctc_humber_ftm_set_default_profile(ctc_ftm_profile_info_t* ctc_profile_info);

/**@} end of @addtogroup allocation FTM  */

#endif
