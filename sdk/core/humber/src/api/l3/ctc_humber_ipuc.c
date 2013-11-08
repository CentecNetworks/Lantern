/**
 @file ctc_humber_ipuc.c

 @date 2009-12-30

 @version v2.0


*/
    /****************************************************************************
     *
    * Header Files
    *
    ****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"

#include "ctc_ipuc.h"
#include "sys_humber_ipuc.h"
#include "sys_humber_nexthop_api.h"

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
 @brief SDK ipuc module initilize

 @param[]

 @return CTC_E_XXX

*/

int32
ctc_humber_ipuc_init(void* ipuc_global_cfg)
{
    return sys_humber_ipuc_init();
}


/**
 @brief

 @param[in] p_ipuc_info data of the ipuc entry

 @return CTC_E_XXX

*/
int32
ctc_humber_ipuc_add(ctc_ipuc_param_t* p_ipuc_info)
{
    return sys_humber_ipuc_add(p_ipuc_info);
}

/**
 @brief

 @param[in] p_ipuc_info data of the ipuc entry

 @return CTC_E_XXX

*/
int32
ctc_humber_ipuc_remove(ctc_ipuc_param_t* p_ipuc_info)
{
    return sys_humber_ipuc_remove(p_ipuc_info);
}

/* now hash next-hop need not to set > 512 */
int32
ctc_humber_ipuc_add_default_entry(ctc_ipuc_param_t* p_ipuc_info)
{
    return sys_humber_ipuc_add_default_entry(p_ipuc_info->ip_ver, p_ipuc_info->nh_id);
}

int32
ctc_humber_ipuc_ipv6_enable(bool enable)
{
    return CTC_E_NONE;
}

int32
ctc_humber_ipuc_cpu_rpf_check(bool enable)
{
    return sys_humber_ipuc_cpu_rpf_check(enable);
}

int32
ctc_humber_ipuc_set_route_ctl(ctc_ipuc_route_ctl_t* p_route_ctl_info)
{
    sys_humber_ipuc_set_route_ctl(p_route_ctl_info);
    return CTC_E_NONE;
}

int32
ctc_humber_ipuc_set_lookup_ctl(ctc_ipuc_lookup_ctl_t* p_lookup_ctl_info)
{
    sys_humber_ipuc_set_lookup_ctl(p_lookup_ctl_info);
    return CTC_E_NONE;
}
