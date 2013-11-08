/**
 @file ctc_humber_l3if.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-1-7

 @version v2.0

This file contains all L3 interface related Humber APIs.

*/
#ifndef _CTC_HUMBER_L3_IF
#define _CTC_HUMBER_L3_IF

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_l3if.h"


/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup l3if L3if
 @{
*/

/**
 @brief   Init L3 interface module
 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3if_init(void* l3if_global_cfg);

  /**
   @brief    Create  L3 interfaces

   @param[in] l3if_id   The id of L3 interface
   @param[in] p_l3_if   L3 interface structure

   @return CTC_E_XXX
  */

extern int32
ctc_humber_l3if_create(uint16 l3if_id, ctc_l3if_t* p_l3_if);

 /**
   @brief    Delete L3 interfaces

   @param[in] l3if_id   The id of L3 interface ,If l3if is phy if or sub-if,the l3if_id range is from 0~511,else 0~1022
   @param[in] p_l3_if   L3 interface structure

   @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_delete(uint16 l3if_id, ctc_l3if_t* p_l3_if);


 /**
   @brief    Config L3 interface's properties

   @param[in]  l3if_id          The id of L3 interface
   @param[in]  l3if_prop     An L3 interface properties
   @param[in]  value            The value of an L3 interface properties

   @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_set_property(uint16 l3if_id,ctc_l3if_property_t l3if_prop,uint32 value);

  /**
    @brief    Get l3 interface's properties according to interface id

    @param[in]  l3if_id             The id of L3 interface
    @param[in]  l3if_prop  An L3 interface properties
    @param[out]  p_value        The value of an L3 interface properties

    @return CTC_E_XXX
  */

extern int32
ctc_humber_l3if_get_property(uint16 l3if_id,ctc_l3if_property_t l3if_prop,uint32* p_value);

/**
       @brief    Config 40bits virtual router mac prefix

       @param[in]   prefix_type     The prefix type of L3 interface router mac
       @param[out]  mac_40bit    40bits  router mac prefix

       @return CTC_E_XXX
  */
extern int32
ctc_humber_l3if_set_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit);

 /**
     @brief    Get 40bits virtual router mac prefix

     @param[in]  prefix_type     The prefix type of L3 interface router mac
     @param[out]  mac_40bit    40bits router mac prefix

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit);


/**
     @brief    Config a low 8 bits virtual router-mac in the L3 interface, it can config up to 4 VRIDs for a index

     @param[in]  l3if_id                  The id of L3 interface
     @param[in]  p_l3if_vmac        The information of a low 8 bits virtual router-mac

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_add_vmac_low_8bit(uint16 l3if_id ,ctc_l3if_vmac_t* p_l3if_vmac);

/**
     @brief    Config a low 8 bits virtual router-mac in the L3 interface, it can config up to 4 VRIDs for a index

     @param[in]  l3if_id                   The id of L3 interface
     @param[in]  p_l3if_vmac         The information of a low 8 bits virtual router-mac

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_remove_vmac_low_8bit(uint16 l3if_id ,ctc_l3if_vmac_t* p_l3if_vmac);

/**
     @brief    Get a low 8 bits router-mac according to interface id

     @param[in]  l3if_id            The id of L3  interface
     @param[in,out]  p_l3if_vmac    The information of  a low 8 bits virtual router-mac

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_vmac_low_8bit(uint16 l3if_id ,ctc_l3if_vmac_t* p_l3if_vmac);

 /**
     @brief    Config router mac

     @param[in]  mac_addr     Router mac

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_set_router_mac(mac_addr_t mac_addr);

 /**
     @brief    Get router mac

     @param[out]  mac_addr     Router mac

     @return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_router_mac(mac_addr_t mac_addr);


/**@} end of @addtogroup l3if L3if*/

#endif

