/**
 @file ctc_humber_linkagg.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-19

 @version v2.0

 This file contains linkagg API interface.
*/

#ifndef _CTC_HUMBER_LINKAGG_H
#define _CTC_HUMBER_LINKAGG_H


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
 @addtogroup linkagg LINKAGG
 @{
*/

/**
 @brief The function is to init the linkagg module
 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_init(void* linkagg_global_cfg);

/**
 @brief The function is to create one linkagg

 @param[in] tid the linkagg id wanted to create

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_create(uint8 tid);

/**
 @brief The function is to delete one linkagg

 @param[in] tid the linkagg id wanted to remove

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_remove(uint8 tid);


/**
 @brief The function is to add a port to linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[in] gport global port of the member port which wanted to add

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_add_port(uint8 tid, uint16 gport);

/**
 @brief The function is to remove the port from linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[in] gport global port of the member port which wanted to add

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_remove_port(uint8 tid, uint16 gport);

/**
 @brief The function is to get the first local member port of linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[out] p_gport the pointer point to the first local member port, will be NULL if none

 @param[out] local_cnt number of local port

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_get_1st_local_port(uint8 tid, uint16* p_gport,uint8* local_cnt);

/**
 @brief The function is to remove the port from linkagg

 @param[in] tid the linkagg id wanted to operate

 @param[out] p_gports a global member ports list of linkagg group

 @param[out] cnt       the number of linkagg member

 @return CTC_E_XXX

*/
extern int32
ctc_humber_linkagg_show_ports(uint8 tid, uint16* p_gports, uint8* cnt);

/**@} end of @addgroup linkagg */

#endif


