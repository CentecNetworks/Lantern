/**
 @file

 @date 2009-10-20

 @version v2.0

*/

#ifndef _SYS_HUMBER_STP_H
#define _SYS_HUMBER_STP_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_const.h"

 /****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define SYS_STP_REG_SIZE (CTC_MAX_STP_ID * 2/ 32)   /* register size of one port */
#define SYS_MAX_STP_STATE_RAM_INDEX 1024
#define SYS_HUMBER_STP_STATE_SHIFT 3

#define SYS_STP_ID_VALID_CHECK(stp_id)                    \
    {                                                  \
        if(stp_id & ~(0x7F)){                               \
            return CTC_E_INVALID_PARAM;                    \
        };                                                \
    }

 #define SYS_STP_DBG_INFO(FMT, ...)                          \
                      {                                                      \
                         CTC_DEBUG_OUT_INFO(l2, stp, L2_STP_SYS,FMT,##__VA_ARGS__);\
                      }

#define SYS_STP_DBG_FUNC()                          \
                      {\
                        CTC_DEBUG_OUT_FUNC(l2, stp, L2_STP_SYS);\
                      }


/****************************************************************************
*
* Function
*
*****************************************************************************/


/**
 @brief the function is to clear all instances blonged to one port

*/
extern int32
sys_humber_stp_clear_all_inst_state (uint16 gport);

/**
 @brief the function is to stp id (MSTI instance) for vlan
*/
extern int32
sys_humber_stp_set_vlan_stpid(uint16 vlan_id, uint8 stpid);

/**
 @brief the function is to stp id (MSTI instance) for vlan
*/
extern int32
sys_humber_stp_get_vlan_stpid(uint16 vlan_id, uint8 *stpid);

/**
 @brief the function is to update the state
*/
extern int32
sys_humber_stp_set_state ( uint16 gport, uint8 stpid, uint8 state);

/**
 @brief the function is to get the instance of the port
*/
extern int32
sys_humber_stp_get_state (uint16 gport,uint8 stpid, uint8* state);


#endif

