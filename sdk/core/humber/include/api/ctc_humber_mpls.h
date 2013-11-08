/**
 @file ctc_humber_mpls.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-03-12

 @version v2.0

 This file define ctc functions of SDK
*/


#ifndef _CTC_HUMBER_MPLS_H
#define _CTC_HUMBER_MPLS_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_mpls.h"
#include "ctc_nexthop.h"
#include "ctc_stats.h"
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
 @addtogroup mpls MPLS
 @{
*/

/**
 @brief Initialize the MPLS module

 @param[in] p_mpls_info Data of the mpls initialization

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_init(ctc_mpls_init_t* p_mpls_info);

/**
 @brief Add a mpls entry

 @param[in] p_mpls_ilm Data of the mpls entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_add_ilm(ctc_mpls_ilm_t* p_mpls_ilm);


/**
 @brief update a mpls entry

 @param[in] p_mpls_ilm Data of the mpls entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_update_ilm(ctc_mpls_ilm_t* p_mpls_ilm);

/**
 @brief Remove a mpls entry

 @param[in] p_mpls_ilm Data of the mpls entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_del_ilm(ctc_mpls_ilm_t* p_mpls_ilm);

/**
 @brief Get information of a mpls ilm entry, inlucde nexthop IDs

 @param[in] p_mpls_ilm Index of the mpls ilm entry

 @param[out] p_mpls_ilm Data of the mpls ilm entry

 @param[out] nh_id Nexthop id array of the mpls ilm entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_get_ilm(uint32* nh_id, ctc_mpls_ilm_t* p_mpls_ilm);

/**
 @brief add mpls stats

 @param[in] stats_index index of the mpls labe stats to be add

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_add_stats(ctc_mpls_stats_index_t* stats_index);

/**
 @brief delete mpls stats

 @param[in] stats_index index of the mpls labe stats to be delete

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_del_stats(ctc_mpls_stats_index_t* stats_index);

/**
 @brief get mpls stats

 @param[in] stats_index index of the mpls labe stats

 @param[out] p_stats point to stats to be get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_get_stats(ctc_mpls_stats_index_t* stats_index, ctc_stats_basic_t* p_stats);


/**
 @brief reset mpls stats

 @param[in] stats_index index of the mpls labe stats

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_reset_stats(ctc_mpls_stats_index_t* stats_index);


/**
 @brief Add the l2vpn pw entry

 @param[in] p_mpls_pw Information of l2vpn pw entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_add_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw);

/**
 @brief Remove the l2vpn pw entry

 @param[in] p_mpls_pw Information of l2vpn pw entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_del_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw);

/**
 @brief Reset the ingress sequence counter

 @param[in] index Index of ingress sequence counter

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_reset_in_sqn(uint8 index);

/**
 @brief Reset the engress sequence counter

 @param[in] index Index of engress sequence counter

 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_reset_out_sqn(uint8 index);

/**
 @brief Check mpls label used or not

 @param[in] spaceid spaceid label space id
 @param[in] label label value
 @param[out] used whether the label is used or not
 @return CTC_E_XXX

*/
extern int32
ctc_humber_mpls_check_label_used(uint16 spaceid, uint32 label, bool *used);

/**@} end of @addgroup   */

#endif
