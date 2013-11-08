/**
 @file ctc_humber_l3if.c

 @date 2009-12-10

 @version v2.0

This file contains all L3 interface related Humber APIs implemention.

*/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_humber_l3if.h"
#include "sys_humber_l3if.h"


/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/

/**
@brief    init l3 interface module

@param[]

@return CTC_E_XXX

*/
extern int32
ctc_humber_l3if_init(void* l3if_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_l3if_init());
    return CTC_E_NONE;
}


/**
@brief    Create  l3 interfaces

@param[in] l3_if   L3  interface structure

@return CTC_E_XXX
*/

extern int32
ctc_humber_l3if_create(uint16 l3if_id, ctc_l3if_t* l3_if)
{
    CTC_ERROR_RETURN(sys_humber_l3if_create(l3if_id, l3_if));
    return CTC_E_NONE;
}

/**
@brief    Delete  l3 interfaces

@param[in] l3_if   L3  interface structure

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_delete(uint16 l3if_id, ctc_l3if_t* l3_if)
{
    CTC_ERROR_RETURN(sys_humber_l3if_delete(l3if_id, l3_if));
    return CTC_E_NONE;
}

/**
@brief    Config  l3 interface's  properties

@param[in]  l3if_id        the id of L3 interface
@param[in]  igs_property   an L3 interface properties

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_set_property(uint16 l3if_id, ctc_l3if_property_t l3if_prop, uint32 value)
{
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id, l3if_prop, value));
    return CTC_E_NONE;
}

/**
@brief    Get  l3 interface's properties  according to interface id

@param[in]  l3if_id         the id of L3 interface
@param[out]  igs_property   an L3 interface properties

@return CTC_E_XXX
*/

extern int32
ctc_humber_l3if_get_property(uint16 l3if_id, ctc_l3if_property_t l3if_prop, uint32* value)
{
    CTC_ERROR_RETURN(sys_humber_l3if_get_property(l3if_id, l3if_prop, value));
    return CTC_E_NONE;
}

/**
@brief    Config  40bits  virtual router mac prefix

@param[in]  prefix_type   the prefix type of l3 interface router mac
@param[out]  mac_40bit    40bits router mac prefix

@return CTC_E_XXX
*/

extern int32
ctc_humber_l3if_set_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{
    CTC_ERROR_RETURN(sys_humber_l3if_set_vmac_prefix(prefix_type, mac_40bit));
    return CTC_E_NONE;
}

/**
@brief    Get  40bits   virtual router mac prefix

@param[in]  prefix_type   the prefix type of l3 interface router mac
@param[out]  mac_40bit    40bits router mac prefix

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{
    CTC_ERROR_RETURN(sys_humber_l3if_get_vmac_prefix(prefix_type, mac_40bit));
    return CTC_E_NONE;
}


/**
@brief    Config a low 8 bits virtual router - mac  in the L3 interface, it can config up to 4 VRIDs  for a index

@param[in]  l3if_id           the id of L3  interface
@param[in]  prefix_type       the prefix type of l3 interface  router mac
@param[in]  low_8bits_mac     the  least singificabt bytes of router mac

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_add_vmac_low_8bit(uint16 l3if_id , ctc_l3if_vmac_t *p_l3if_vmac)
{
    CTC_ERROR_RETURN(sys_humber_l3if_add_vmac_low_8bit(l3if_id, p_l3if_vmac));
    return CTC_E_NONE;
}
/**
@brief    Config a low 8 bits virtual router - mac  in the L3 interface, it can config up to 4 VRIDs  for a index

@param[in]  l3if_id                   the id of L3  interface
@param[in]  prefix_type          the prefix type of l3 interface  router mac
@param[in]  low_8bits_mac   the  least singificabt bytes of router mac

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_remove_vmac_low_8bit(uint16 l3if_id , ctc_l3if_vmac_t *p_l3if_vmac)
{
    CTC_ERROR_RETURN(sys_humber_l3if_remove_vmac_low_8bit(l3if_id, p_l3if_vmac));
    return CTC_E_NONE;
}

/**
@brief    Get a low 8 bits router - mac  according to interface id

@param[in]  l3if_id            the id of L3  interface
@param[out]  low_8bits_mac     the  least singificabt bytes of router mac

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_vmac_low_8bit(uint16 l3if_id , ctc_l3if_vmac_t *p_l3if_vmac)
{
    CTC_ERROR_RETURN(sys_humber_l3if_get_vmac_low_8bit(l3if_id, p_l3if_vmac));
    return CTC_E_NONE;
}

/**
@brief    Config  router mac

@param[in]  mac_addr     router mac

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_set_router_mac( mac_addr_t mac_addr)
{
    CTC_ERROR_RETURN(sys_humber_l3if_set_router_mac(mac_addr));
    return CTC_E_NONE;
}

/**
@brief    Get  router mac

@param[out]  mac_addr     router mac

@return CTC_E_XXX
*/
extern int32
ctc_humber_l3if_get_router_mac( mac_addr_t mac_addr)
{
    CTC_ERROR_RETURN(sys_humber_l3if_get_router_mac(mac_addr));
    return CTC_E_NONE;
}




