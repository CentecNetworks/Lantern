/**
 @file ctc_humber_stats.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

   This file define ctc functions of SDK
*/


#ifndef _CTC_HUMBER_STATS_H
#define _CTC_HUMBER_STATS_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_stats.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
 #define CTC_STATS_DBG_INFO(FMT, ...)                          \
                        {                                                      \
                            CTC_DEBUG_OUT_INFO(stats, stats, STATS_CTC,FMT,##__VA_ARGS__);\
                        }

#define CTC_STATS_DBG_FUNC()                          \
                        {\
                            CTC_DEBUG_OUT_FUNC(stats, stats, STATS_CTC);\
                        }

/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @addtogroup stats STATS
 @{
*/


/**
 @brief Init statistics table and register

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_init(void* stats_global_cfg);




/**********************************************************************************
                        Define MAC base stats functions
***********************************************************************************/

/**
 @brief Set Mac base stats property

 @param[in] gport  global port of the system

 @param[in] mac_stats_prop_type  the property type to set

 @param[in] prop_data  the property value to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_set_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t prop_data);

/**
 @brief Get Mac base stats property

 @param[in] gport  global port of the system

 @param[in] mac_stats_prop_type  the property type to get

 @param[out] p_prop_data  the property value to get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_mac_stats_cfg(uint16 gport, ctc_mac_stats_prop_type_t mac_stats_prop_type, ctc_mac_stats_property_t* p_prop_data);

/**
 @brief Get Mac base stats

 @param[in] gport  global port of the system

 @param[in] dir  MAC base stats direction

 @param[out] p_stats  MAC base stats value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir, ctc_mac_stats_t* p_stats);

/**
 @brief Reset Mac base stats

 @param[in] gport  global port of the system

 @param[in] dir  MAC base stats direction

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_reset_mac_stats(uint16 gport, ctc_mac_stats_dir_t dir);


/**********************************************************************************
                        Define port base stats functions
***********************************************************************************/

/**
 @brief Set Port base stats option

 @param[in] dir  direction

 @param[in] type  port stats option type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_set_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t type);

/**
 @brief Get Port base stats option

 @param[in] dir  direction

 @param[out] p_type  port stats option type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_port_stats_cfg(ctc_direction_t dir, ctc_stats_port_stats_option_type_t* p_type);


/**
 @brief Get Port base stats

 @param[in] gport  global port of the system

 @param[in] dir  direction

 @param[out] p_stats    port base stats

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_port_stats(uint16 gport, ctc_direction_t dir, ctc_stats_port_t* p_stats);

/**
 @brief Reset Port base stats

 @param[in] gport  global port of the system

 @param[in] dir  direction

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_reset_port_stats(uint16 gport, ctc_direction_t dir);



/**********************************************************************************
                        Define global frowarding stats functions
***********************************************************************************/

/**
 @brief Get Global Forwarding stats

 @param[in] dir  direction

 @param[out] p_stats  global forwarding stats

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_global_fwd_stats(ctc_direction_t dir, ctc_stats_global_fwd_t* p_stats);

/**
 @brief Reset Global Forwarding stats

 @param[in] dir  direction

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_reset_global_fwd_stats(ctc_direction_t dir);


/**********************************************************************************
                        Define stats general options functions
***********************************************************************************/

/**
 @brief Set port log discard stats enable

 @param[in] enable    a boolean value denote log port discard stats is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_set_drop_packet_stats_enable(bool enable);

/**
 @brief Get port log discard stats enable

 @param[out] p_enable    a boolean value denote log port discard stats is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_drop_packet_stats_enable(bool* p_enable);


/**
 @brief Get port log stats

 @param[in] gport  global port of the system

 @param[in] dir  direction

 @param[out] p_stats    port log stats

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_log_option(uint16 gport ,ctc_direction_t dir, ctc_stats_basic_t* p_stats);

/**
 @brief Reset port log stats

 @param[in] gport  global port of the system

 @param[in] dir  direction

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_reset_log_option(uint16 gport ,ctc_direction_t dir);


/**
 @brief Set stats general property

 @param[in] stats_param  global forwarding stats property type

 @param[in] stats_prop   the property value to set

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_set_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t stats_prop);


/**
 @brief Get stats general property

 @param[in] stats_param  global forwarding stats property type

 @param[out] p_stats_prop   the property value to get

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_global_cfg(ctc_stats_property_param_t stats_param, ctc_stats_property_t* p_stats_prop);



/**
 @brief Get port log discard stats enable

 @param[in] fwd_stats_type    forwarding stats sub type

 @param[out] p_enable    a boolean value denote forwarding stats is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_get_fwd_stats_enable(ctc_stats_fwd_type_t fwd_stats_type, bool* p_enable);

/**
 @brief forward stats interrupt callback function

 @param[in] gchip    Global chip of the system

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stats_intr_callback_func(uint8* gchip);


/**@} end of @addtogroup stats STATS  */




#endif
