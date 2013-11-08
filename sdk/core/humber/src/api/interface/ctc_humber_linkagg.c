/**
 @file ctc_humber_linkagg.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-10

 @version v2.0

 This file contains linkagg function interface.
*/

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_debug.h"

#include "ctc_humber_linkagg.h"
#include "sys_humber_linkagg.h"

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
 @brief The function is to init the linkagg module

 @param[]

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_init(void* linkagg_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_linkagg_init());

    return CTC_E_NONE;
}

/**
 @brief The function is to create one linkagg

 @param[in] tid the linkagg id wanted to create

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_create(uint8 tid)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC, "Create linkagg group id:%d\n", tid);

    CTC_ERROR_RETURN(sys_humber_linkagg_create(tid));

    return CTC_E_NONE;
}

/**
 @brief The function is to delete one linkagg

 @param[in] tid the linkagg id wanted to remove

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_remove(uint8 tid)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC, "Remove linkagg group id:%d\n", tid);

    CTC_ERROR_RETURN(sys_humber_linkagg_remove(tid));

    return CTC_E_NONE;
}

/**
 @brief The function is to add a port to linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[in] gport global port of the member port which wanted to add

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_add_port(uint8 tid, uint16 gport)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC,
    "Add member port to linkagg group id:%d, gport:%d\n", tid, gport);

    CTC_ERROR_RETURN(sys_humber_linkagg_add_port(tid, gport));

    return CTC_E_NONE;
}

/**
 @brief The function is to remove the port from linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[in] gport global port of the member port which wanted to add

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_remove_port(uint8 tid, uint16 gport)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC,
    "Remove member port from linkagg group id:%d, gport:%d\n", tid, gport);

    CTC_ERROR_RETURN(sys_humber_linkagg_remove_port(tid, gport));

    return CTC_E_NONE;
}

/**
 @brief The function is to get the first local member port of linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[out] p_gport the pointer point to the first local member port, will be NULL if none

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_get_1st_local_port(uint8 tid, uint16* p_gport,uint8 *local_cnt)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC,
    "Get 1st member local_port,linkagg group id:%d\n", tid);

    CTC_ERROR_RETURN(sys_humber_linkagg_get_1st_local_port(tid, p_gport,local_cnt));
    return CTC_E_NONE;
}

/**
 @brief The function is to remove the port from linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[out] p_gports a global member ports list of linkagg group

 @return CTC_E_XXX

*/
int32
ctc_humber_linkagg_show_ports(uint8 tid, uint16* p_gports, uint8* cnt)
{
    CTC_DEBUG_OUT_INFO(linkagg, linkagg, LINKAGG_CTC, "Show linkagg member ports, tid = %d\n", tid);

    CTC_ERROR_RETURN(sys_humber_linkagg_show_ports(tid, p_gports, cnt));

    return CTC_E_NONE;
}



