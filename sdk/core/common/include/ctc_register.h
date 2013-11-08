/**
 @file ctc_register.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2011-7-26

 @version v2.0

   This file contains all register related data structure, enum, macro and proto.
*/


 #ifndef _CTC_REGISTER_H
 #define _CTC_REGISTER_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @brief  global control stats mode
*/
enum ctc_global_control_stats_mode_e
{
    CTC_GLOBAL_STATS_CONFLICT_MODE = 0,     /**< support more stats feature, but may be conflict */
    CTC_GLOBAL_STATS_NO_CONFLICT_MODE,      /**< no phb, no service policer stats, only support 64 port policer stats and few flow policer stats*/
    CTC_GLOBAL_STATS_OPENFLOW_MODE,         /**< no port policer, no service policer, only flow policer */
    CTC_GLOBAL_STATS_MODE_MAX
};
typedef enum ctc_global_control_stats_mode_e ctc_global_control_stats_mode_t;

/**
 @brief  global control type
*/
enum ctc_global_control_type_e
{
    CTC_GLOBAL_DISCARD_SAME_MACDASA_PKT = 0,   /**< value: bool*, TRUE or FALSE */
    CTC_GLOBAL_DISCARD_SAME_IPDASA_PKT,        /**< value: bool*, TRUE or FALSE */
    CTC_GLOBAL_SERVICE_POLICER_NUM,            /**< Global service policer num */
    CTC_GLOBAL_SUPPORT_PHB,                    /**< value: bool*, TRUE or FALSE */
    CTC_GLOBAL_STATS_MODE,                     /**< CTC_GLOBAL_STATS_XXX */

    CTC_GLOBAL_CONTROL_MAX
};
typedef enum ctc_global_control_type_e ctc_global_control_type_t;



#endif

