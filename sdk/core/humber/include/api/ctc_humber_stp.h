/**
 @file ctc_humber_stp.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-16

 @version v2.0

 This file contains all the STP APIs
 */



#ifndef _CTC_HUMBER_STP_H
#define _CTC_HUMBER_STP_H


/**********************************************************************************
              Define API function interfaces
***********************************************************************************/

/**
 @addtogroup stp STP
 @{
*/

/**
 @brief The function is to clear all instances blonged to one port

 @param[in]   gport     global port

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stp_clear_all_inst_state(uint16 gport);

/**
 @brief The function is to set stp id

 @param[in] vlan_id 802.1q vlan id

 @param[in] stpid stp instance

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stp_set_vlan_stpid(uint16 vlan_id, uint8 stpid);

/**
 @brief The function is to get stp id

 @param[in] vlan_id 802.1q vlan id

 @param[in] stpid stp instance

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stp_get_vlan_stpid(uint16 vlan_id, uint8* stpid);


/**
 @brief The function is to update the state

 @param[in]   gport         global port

 @param[in]   stpid          a value denoted the index of the table

 @param[in]   state         a value denoted the instance

 @return CTC_E_XXX

*/

extern int32
ctc_humber_stp_set_state(uint16 gport, uint8 stpid, uint8 state);

/**
 @brief The function is to get the instance of the port

  @param[in]   gport        global port

 @param[in]     stpid        a value denoted the index of the table

 @param[out]  state        the pointer point to the getted state, will be NULL if none

 @return CTC_E_XXX

*/
extern int32
ctc_humber_stp_get_state(uint16 gport,uint8 stpid, uint8* state);

/**@} end of @addtogroup stp STP*/

#endif
