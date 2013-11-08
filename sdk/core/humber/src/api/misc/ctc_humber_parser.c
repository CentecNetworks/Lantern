/**
 @file ctc_humber_parser.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

This file contains parser module function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_humber_parser.h"
#include "sys_humber_parser.h"

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
 @brief set l2 hash field

 @param[in] l2ctl  the field set of parser ethernet ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_l2_hash(l2ctl));
    return CTC_E_NONE;
}

/**
 @brief get hash computing field

 @param[in] l2ctl  the field set of parser ethernet ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_l2_hash(l2ctl));
    return CTC_E_NONE;
}

/**
 @brief set tpid

 @param[in] type tpid_type CTC_PARSER_L2_TPID_XXX

 @param[in] tpid tpid value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_tpid(ctc_parser_l2_tpid_t type, uint16 tpid)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_tpid(type, tpid));
    return CTC_E_NONE;
}

/**
 @brief get tpid with some type

 @param[in] type tpid_type CTC_PARSER_L2_TPID_XXX

 @param[out] tpid tpid value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_tpid(ctc_parser_l2_tpid_t type, uint16* tpid)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_tpid(type, tpid));
    return CTC_E_NONE;
}

/**
 @brief set snap allow none zero oui

 @param[in] enable  enable to set none zero oui

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_snap_allow_none_zero_oui(uint8 enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_snap_allow_none_zero_oui(enable));
    return CTC_E_NONE;
}

/**
 @brief get snap allow none zero oui

 @param[out] enable returned value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_snap_allow_none_zero_oui(uint8* enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_snap_allow_none_zero_oui( enable));
    return CTC_E_NONE;
}

/**
 @brief set max_length,based on the value differentiate type or length

 @param[in] max_length max_length value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_max_length_filed(uint16 max_length)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_max_length_filed(max_length));

    return CTC_E_NONE;
}

/**
 @brief get max_length value

 @param[out] max_length max_length value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_max_length_filed(uint16* max_length)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_max_length_filed(max_length));

    return CTC_E_NONE;
}

/**
 @brief set parser quad vlan enable

 @param[in] enable enable set parser quad vlan or not

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_parser_quad_vlan_en(uint8 enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_parser_quad_vlan_en(enable));

    return CTC_E_NONE;
}

/**
 @brief get parser quad vlan

 @param[out] enable  returned value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_parser_quad_vlan_en(uint8* enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_parser_quad_vlan_en(enable));

    return CTC_E_NONE;
}

/**
 @brief set vlan parser num

 @param[in] vlan_num vlan parser num

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_vlan_parser_num(uint8 vlan_num)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_vlan_parser_num(vlan_num));

    return CTC_E_NONE;
}

/**
 @brief get vlan parser num

 @param[out] vlan_num returned value

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_vlan_parser_num(uint8* vlan_num)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_vlan_parser_num(vlan_num));

    return CTC_E_NONE;
}

/**
 @brief set pbb parser ctl info

 @param[in] pbb_parser_ctl  pbb parser ctl info

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_pbb_parser_ctl(pbb_parser_ctl));

    return CTC_E_NONE;
}

/**
 @brief get pbb parser ctl info

 @param[in] pbb_parser_ctl  pbb parser ctl info

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_pbb_parser_ctl(pbb_parser_ctl));

    return CTC_E_NONE;
}

/**
 @brief set l2 flex ctl info

 @param[in] l2flex_ctl  l2 flex ctl info

 @param[in] index  entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_l2flex_ctl(index, l2flex_ctl));

    return CTC_E_NONE;
}

/**
 @brief get l2 flex ctl info

 @param[in] l2flex_ctl  l2 flex ctl info

 @param[in] index  entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_l2flex_ctl(index, l2flex_ctl));

    return CTC_E_NONE;
}

/**
 @brief add l2type

 @param[in] index  the entry index

 @param[in] entry  the entry

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_add_l2_type(uint8 index, ctc_parser_l2_ptl_entry_t* entry)
{
    CTC_ERROR_RETURN(sys_humber_parser_add_l2_type(index, entry));

    return CTC_E_NONE;
}

/**
 @brief set the entry invalid based on the index

 @param[in] index  the entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_remove_l2_type(uint8 index)
{
    CTC_ERROR_RETURN(sys_humber_parser_remove_l2_type(index));

    return CTC_E_NONE;
}

/**
 @brief Get l3type by l2 protocol header

 @param[in] p_entry  the pionter to structure of ctc_parser_l2_ptl_entry_t

 @param[out] p_l3_type return value of l3 type;

 @return CTC_E_XXX
*/
extern int32
ctc_humber_parser_get_l2_type(ctc_parser_l2_ptl_entry_t* p_entry, ctc_parser_l3_type_t* p_l3_type)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_l2_type(p_entry, p_l3_type));

    return CTC_E_NONE;
}


int32
ctc_humber_parser_l2_enable_l3_type(ctc_parser_l3_type_t l3_type, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_l2_enable_l3_type(l3_type, enable));

    return CTC_E_NONE;
}


/**
 @brief set ip hash ctl reg

 @param[in] hash_ctl ip hash ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_ip_hash_ctl(hash_ctl));

    return CTC_E_NONE;
}

/**
 @brief get ip hash ctl reg info

 @param[in] hash_ctl ip hash ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_ip_hash_ctl(hash_ctl));

    return CTC_E_NONE;
}

/**
 @brief set parser ipv6 ctl reg

 @param[in] index  ipv6 ext header index

 @param[in] ipv6_ctl  parser ipv6 ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_ipv6_ctl(index, ipv6_ctl));

    return CTC_E_NONE;
}

/**
 @brief get parser ipv6 ctl reg

 @param[in] index  ipv6 ext header index

 @param[in] ipv6_ctl  parser ipv6 ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_ipv6_ctl(index, ipv6_ctl));

    return CTC_E_NONE;
}

/**
 @brief set parser mpls ctl reg

 @param[in] mpls_ctl  parser mpls ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_mpls_ctl(mpls_ctl));

    return CTC_E_NONE;
}

/**
 @brief get parser mpls ctl reg

 @param[in] mpls_ctl  parser mpls ctl reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_mpls_ctl(mpls_ctl));

    return CTC_E_NONE;
}

/**
 @brief set parser l3flex ctl reg

 @param[in] l3flex_ctl  parser l3flex ctl reg

 @param[in] index  entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_l3flex_ctl(index, l3flex_ctl));

    return CTC_E_NONE;
}

/**
 @brief get parser l3flex ctl reg

 @param[in] l3flex_ctl  parser l3flex ctl reg

 @param[in] index  entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_l3flex_ctl(index, l3flex_ctl));

    return CTC_E_NONE;
}

/**
 @brief add l3type,can add a new l3type,addition offset for the type,can get the layer4 type

 @param[in] index  the entry index

 @param[in] entry  the entry

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_add_l3_type(uint8 index, ctc_parser_l3_ptl_entry_t* entry)
{
    CTC_ERROR_RETURN(sys_humber_parser_add_l3_type(index, entry));

    return CTC_E_NONE;
}

/**
 @brief set the entry invalid based on the index

 @param[in] index  the entry index

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_remove_l3_type(uint8 index)
{
    CTC_ERROR_RETURN(sys_humber_parser_remove_l3_type(index));

    return CTC_E_NONE;
}

int32
ctc_humber_parser_l3_enable_l4_type(ctc_parser_l4_type_t l4_type, bool enable)
{
    CTC_ERROR_RETURN(sys_humber_parser_l3_enable_l4_type(l4_type, enable));

    return CTC_E_NONE;
}


/**
 @brief set layer4 parser reg

 @param[in] tbl_flags  CTC_PARSER_LAYER4_TBL_FLAGS_XXX

 @param[in] l4_parser_ctl  l4 parser reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_layer4_parser_ctl(tbl_flags, l4_parser_ctl));

    return CTC_E_NONE;
}

/**
 @brief get layer4 parser info

 @param[in] tbl_flags  CTC_PARSER_LAYER4_TBL_FLAGS_XXX

 @param[in] l4_parser_ctl  l4 parser reg

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_get_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    CTC_ERROR_RETURN(sys_humber_parser_get_layer4_parser_ctl(tbl_flags, l4_parser_ctl));

    return CTC_E_NONE;
}

/**
 @brief add l4type

 @param[in] index application entry index

 @param[in] entry  the entry

 @return SDK_E_XXX

*/
int32
ctc_humber_parser_set_layer4_type(uint8 index, ctc_parser_l4_ptl_entry_t* entry)
{
    CTC_ERROR_RETURN(sys_humber_parser_set_layer4_type(index, entry));

    return CTC_E_NONE;
}


/**
 @brief init parser module

 @param[]

 @return CTC_E_XXX

*/
int32
ctc_humber_parser_init(void* parser_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_parser_init());
    return CTC_E_NONE;
}

