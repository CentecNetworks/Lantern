/**
 @file ctc_aps.h

 @date 2010-3-10

 @version v2.0

 The file define all struct used in aps APIs.
*/

#ifndef _CTC_APS_H
#define _CTC_APS_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "kal.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @addtogroup aps APS
 @{
*/

/**
 @brief Define APS type
*/
enum ctc_aps_type_e
{
    CTC_APS_DISABLE0,   /**<APS disable */
    CTC_APS_DISABLE1,   /**<APS disable */
    CTC_APS_SELECT,     /**<APS selector*/
    CTC_APS_BRIDGE      /**<APS bridge  */
};
typedef enum ctc_aps_type_e ctc_aps_type_t;

/**
 @brief Define APS Bridge group
*/
struct ctc_aps_bridge_group_s
{
    bool protect_en;                    /**< For bridge switch */
    uint16 working_gport;               /**< APS bridge working path */
    uint16 protection_gport;            /**< APS bridge protecting path */

    bool raps_en;                       /**< Indicate raps whether enable */
    uint16 raps_group_id;               /**< Raps mcast group id */
};
typedef struct ctc_aps_bridge_group_s ctc_aps_bridge_group_t;

/**
 @brief Define Enumerator of APS Selector
*/
enum ctc_aps_select_type_e
{
    CTC_APS_SELECTOR_ETH,       /**<APS select for ETH, used in QinQ*/
    CTC_APS_SELECTOR_MPLS,      /**<APS select for MPLS, used in vpls/vpws*/
    CTC_APS_SELECTOR_PBT        /**<APS select for PBT, used in PBB/PBT*/
};
typedef enum ctc_aps_select_type_e ctc_aps_select_type_t;


/**
 @brief Define R-APS member
*/
struct ctc_raps_member_s
{
    uint16 group_id;        /**< mcast group id*/
    uint16 mem_port;        /**< member port if member is local member  gport:gchip(8bit) +local phy port(8bit);
                                            else if member is remote chip entry,gport: gchip(local) + remote gchip id(8bit)*/
    bool   remote_chip;     /**< if set,member is remote chip entry*/
    bool   remove_flag;     /**< if set,remove member, else add member*/

};
typedef struct ctc_raps_member_s ctc_raps_member_t;


/**@}*/ /*end of @addtogroup APS*/

#endif

