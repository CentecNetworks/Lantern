/**
 @file ctc_humber_mpls.c

 @date 2010-03-16

 @version v2.0


*/
/****************************************************************************
*
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_mpls.h"
#include "ctc_vector.h"
#include "sys_humber_usrid.h"
#include "sys_humber_mpls.h"
#include "sys_humber_nexthop_api.h"
#include "ctc_stats.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief SDK mpls module initilize

 @param[]

 @return CTC_E_XXX

*/

int32
ctc_humber_mpls_init(ctc_mpls_init_t* p_mpls_info)
{
    return sys_humber_mpls_init(p_mpls_info);
}


/**
 @brief

 @param[in] p_mpls_ilm, parameters used to add mpls ilm entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_add_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    return sys_humber_mpls_add_ilm(p_mpls_ilm);
}

/**
 @brief

 @param[in] p_mpls_ilm, parameters used to update mpls ilm entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_update_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    return sys_humber_mpls_update_ilm(p_mpls_ilm);
}
/**
 @brief

 @param[in] p_ipuc_info data of the ipuc entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_del_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    return sys_humber_mpls_del_ilm(p_mpls_ilm);
}

/**
 @brief

 @param[in] p_mpls_ilm index of the mpls ilm entry

 @param[out] p_mpls_ilm data of the mpls ilm entry

 @param[out] nh_id nexthop id array of the mpls ilm entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_get_ilm(uint32* nh_id, ctc_mpls_ilm_t* p_mpls_ilm)
{
    return sys_humber_mpls_get_ilm(nh_id, p_mpls_ilm);
}

/**
 @brief add mpls stats

 @param[in] stats_index index of the mpls labe stats to be add

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_add_stats(ctc_mpls_stats_index_t* stats_index)
{
    return sys_humber_mpls_add_stats(stats_index);
}

/**
 @brief delete mpls stats

 @param[in] stats_index index of the mpls labe stats to be delete

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_del_stats(ctc_mpls_stats_index_t* stats_index)
{
    return sys_humber_mpls_del_stats(stats_index);
}

/**
 @brief get mpls stats

 @param[in] stats_index index of the mpls labe stats

 @param[out] p_stats point to stats to be get

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_get_stats(ctc_mpls_stats_index_t* stats_index, ctc_stats_basic_t* p_stats)
{
    return sys_humber_mpls_get_stats(stats_index, p_stats);
}

/**
 @brief reset mpls stats

 @param[in] stats_index index of the mpls labe stats

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_reset_stats(ctc_mpls_stats_index_t* stats_index)
{
    return sys_humber_mpls_reset_stats(stats_index);
}


/**
 @brief Add the l2vpn pw entry

 @param[in] p_mpls_pw Information of l2vpn pw entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_add_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    return sys_humber_mpls_add_l2vpn_pw(p_mpls_pw);
}

/**
 @brief Remove the l2vpn pw entry

 @param[in] label VC label of the l2vpn pw entry

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_del_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    return sys_humber_mpls_del_l2vpn_pw(p_mpls_pw);
}

/**
 @brief Reset the ingress sequence counter

 @param[in] index index of ingress sequence counter

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_reset_in_sqn(uint8 index)
{
    return sys_humber_mpls_reset_in_sqn(index);
}

/**
 @brief Reset the engress sequence counter

 @param[in] index index of engress sequence counter

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_reset_out_sqn(uint8 index)
{
    return sys_humber_mpls_reset_out_sqn(index);
}

/**
 @brief Check mpls label is used or not

 @param[in] spaceid spaceid label space id
 @param[in] label label value
 @param[out] used whether the label is used or not

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_check_label_used(uint16 spaceid, uint32 label, bool *used)
{
    return sys_humber_mpls_check_label_used(spaceid, label, used);
}


