/**
 @file

 @date 2009-10-20

 @version v2.0


*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "kal.h"
#include "ctc_humber_stp.h"
#include "sys_humber_stp.h"


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
 @brief the function is to clear all instances blonged to one port

 @param[in] gport  global port

 @return CTC_E_XXX

*/
int32
ctc_humber_stp_clear_all_inst_state( uint16 gport )
{
    CTC_ERROR_RETURN(sys_humber_stp_clear_all_inst_state( gport ));

    return CTC_E_NONE;
}

/**
 @brief the function is to set stp stp id (MSTI instance) for vlan

 @param[in] vlan_id, 802.1q vlan id

 @param[in] stpid, stp instance

 @return CTC_E_XXX

*/
int32
ctc_humber_stp_set_vlan_stpid(uint16 vlan_id, uint8 stpid)
{
    CTC_ERROR_RETURN(sys_humber_stp_set_vlan_stpid(vlan_id, stpid));

    return CTC_E_NONE;
}

/**
 @brief The function is to get stp id

 @param[in] vlan_id 802.1q vlan id

 @param[in] stpid stp instance

 @return CTC_E_XXX

*/
int32
ctc_humber_stp_get_vlan_stpid(uint16 vlan_id, uint8* stpid)
{
    CTC_ERROR_RETURN(sys_humber_stp_get_vlan_stpid(vlan_id, stpid));

    return CTC_E_NONE;
}

/**
 @brief the function is to update the state

 @param[in] gport  global port

 @param[in] stpid  a value denoted the index of the table

 @param[in] state  a value denoted the instance

 @return CTC_E_XXX

*/
int32
ctc_humber_stp_set_state( uint16 gport , uint8 stpid, uint8 state)
{
   CTC_ERROR_RETURN( sys_humber_stp_set_state(gport,   stpid,  state ));

    return CTC_E_NONE;
};

/**
 @brief the function is to get the instance of the port

 @param[in] gport  global port

 @param[in] stpid  a value denoted the index of the table

 @param[out] state  the pointer point to the getted state, will be NULL if none

 @return CTC_E_XXX

*/
int32
ctc_humber_stp_get_state( uint16 gport , uint8 stpid, uint8* state)
{
    CTC_ERROR_RETURN(sys_humber_stp_get_state(gport, stpid, state));

    return CTC_E_NONE;
};

