/**
 @file ctc_humber_ipuc.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-07

 @version v2.0

 This file define ctc functions of SDK
*/


#ifndef _CTC_HUMBER_IPUC_H
#define _CTC_HUMBER_IPUC_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_ipuc.h"
#include "ctc_nexthop.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/****************************************************************
 *
* Function
*
****************************************************************/
/**
 @addtogroup ipuc IPUC
 @{
*/

/**
 @brief Initialize the IPUC module

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_init(void* ipuc_global_cfg);

/**
 @brief Add a route entry

 @param[in] p_ipuc_info Data of the ipuc entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_add(ctc_ipuc_param_t* p_ipuc_info);

/**
 @brief Remove a route entry

 @param[in] p_ipuc_info Data of the ipuc entry, route_flag no need to specified

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_remove(ctc_ipuc_param_t* p_ipuc_info);

/**
 @brief Add the default route entrys for both ipv4 and ipv6, default routes should be installed before using ipuc function

 @param[in] p_ipuc_info Data of the ipuc entry, route_flag no need to specified

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_add_default_entry(ctc_ipuc_param_t* p_ipuc_info);

/**
 @brief Enable or disable the ipv6 function

 @param[in] enable TRUE or FALSE

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_ipv6_enable(bool enable);

/**
 @brief Enable or disable the cpu rpf check function

 @param[in] enable TRUE or FALSE

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_cpu_rpf_check(bool enable);

/**
 @brief Config route behavior, such as ttl limit, mcast address check, etc.

 @param[in] p_route_ctl_info Structure of input information

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_set_route_ctl(ctc_ipuc_route_ctl_t* p_route_ctl_info);

/**
 @brief Config lookup behavior, such as ipsa lookup etc.

 @param[in] p_lookup_ctl_info Structure of input information

 @return CTC_E_XXX

*/
extern int32
ctc_humber_ipuc_set_lookup_ctl(ctc_ipuc_lookup_ctl_t* p_lookup_ctl_info);


/**@} end of @addgroup   */

#endif

