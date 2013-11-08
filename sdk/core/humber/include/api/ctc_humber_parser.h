/**
 @file ctc_humber_parser.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

 This file contains parser packet layer2 ,layer3,layer4 information
*/

#ifndef CTC_HUMBER_PARSER_H_
#define CTC_HUMBER_PARSER_H_

#include "kal.h"
#include "ctc_parser.h"

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
@addtogroup parser PARSER
@{
*/

/**
 @addtogroup parser_l2 PARSER_L2
 @{
*/

/**
 @brief Set layer2 hash field

 @param[in] l2ctl  the field set of parser ethernet ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl);

/**
 @brief Get hash field

 @param[out] l2ctl  the filed of hash computing

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl);

/**
 @brief Set tpid value

 @param[in] type tpid_type CTC_PARSER_L2_TPID_XXX

 @param[in] tpid tpid value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_tpid(ctc_parser_l2_tpid_t type, uint16 tpid);

/**
 @brief Get tpid with specified type

 @param[in] type tpid_type CTC_PARSER_L2_TPID_XXX

 @param[out] tpid tpid value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_tpid(ctc_parser_l2_tpid_t type, uint16* tpid);

/**
 @brief Set snap allow none zero oui

 @param[in] enable  enable to set none zero oui

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_snap_allow_none_zero_oui(uint8 enable);

/**
 @brief Get snap allow none zero oui

 @param[out] enable returned value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_snap_allow_none_zero_oui(uint8* enable);

/**
 @brief Set max_length,based on the value differentiate to denote type or length

 @param[in] max_length max_length value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_max_length_filed(uint16 max_length);

/**
 @brief Get max_length value

 @param[out] max_length max_length value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_max_length_filed(uint16* max_length);

/**
 @brief Set parser quad vlan enable

 @param[in] enable enable set parser quad vlan or not

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_parser_quad_vlan_en(uint8 enable);

/**
 @brief Get parser quad vlan

 @param[out] enable  returned value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_parser_quad_vlan_en(uint8* enable);

/**
 @brief Set vlan parser num

 @param[in] vlan_num vlan parser num

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_vlan_parser_num(uint8 vlan_num);

/**
 @brief Get vlan parser num

 @param[out] vlan_num returned value

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_vlan_parser_num(uint8* vlan_num);

/**
 @brief Set pbb parser ctl info

 @param[in] pbb_parser_ctl  pbb parser ctl info

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl);

/**
 @brief Get pbb parser ctl info

 @param[out] pbb_parser_ctl  pbb parser ctl info

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl);

/**
 @brief Set layer2 flex control

 @param[in] index  entry index

 @param[in] l2flex_ctl  l2 flex ctl info

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl);

/**
 @brief Get layer2 flex control

 @param[in] index  entry index

 @param[out] l2flex_ctl  l2 flex ctl info

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl);

/**
 @brief Add l2type,can add a new l2type,addition offset for the type,can get the layer3 type

 @param[in] index  the entry index (The range :0~2)

 @param[in] entry  the entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_add_l2_type(uint8 index, ctc_parser_l2_ptl_entry_t* entry);

/**
 @brief Remove entry based on the index

 @param[in] index  the entry index(The range :0~2)

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_remove_l2_type(uint8 index);

/**
 @brief Enable entry based on the l3 type

 @param[in] p_entry  layer2 protocol entry fields

 @param[in] p_l3_type  layer3 type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_l2_type(ctc_parser_l2_ptl_entry_t* p_entry, ctc_parser_l3_type_t* p_l3_type);

/**
 @brief Enable entry based on the l3 type

 @param[in] l3_type  layer3 type

 @param[in] enable  a boolean value denote l3 type is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_l2_enable_l3_type(ctc_parser_l3_type_t l3_type, bool enable);


/**@} end of @addtogroup   parser_l2 PARSER_L2*/

/**
 @addtogroup parser_l3 PARSER_L3
 @{
*/

/**
 @brief Set ip hash control

 @param[in] hash_ctl ip hash ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl);

/**
 @brief Get ip hash control

 @param[out] hash_ctl ip hash ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl);

/**
 @brief Set parser ipv6 control

 @param[in] index  ipv6 ext header index

 @param[in] ipv6_ctl  parser ipv6 ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl);

/**
 @brief Get parser ipv6 control

 @param[in] index  ipv6 ext header index

 @param[out] ipv6_ctl  parser ipv6 ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl);

/**
 @brief Set parser mpls control

 @param[in] mpls_ctl  parser mpls ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl);

/**
 @brief Get parser mpls control

 @param[out] mpls_ctl  parser mpls ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl);

/**
 @brief Set parser l3flex control

 @param[in] index  entry index

 @param[in] l3flex_ctl  parser l3flex ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl);

/**
 @brief Get parser l3flex control

 @param[in] index  entry index

 @param[out] l3flex_ctl  parser l3flex ctl reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl);

/**
 @brief Add l3type,can add a new l3type,addition offset for the type,can get the layer4 type

 @param[in] index  the entry index(The range :0~6)

 @param[in] entry  the entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_add_l3_type(uint8 index, ctc_parser_l3_ptl_entry_t* entry);

/**
 @brief Remove entry based on the index

 @param[in] index  the entry index(The range :0~6)

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_remove_l3_type(uint8 index);

/**
 @brief Enable entry based on the l4 type

 @param[in] l4_type  layer4 type

 @param[in] enable  a boolean value denote l4 type is enable

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_l3_enable_l4_type(ctc_parser_l4_type_t l4_type, bool enable);


/**@} end of @addtogroup   parser_l3 PARSER_L3*/


/**
 @addtogroup parser_l4 PARSER_L4
 @{
*/

/**
 @brief Set layer4 parser

 @param[in] tbl_flags  CTC_PARSER_LAYER4_TBL_FLAGS_XXX

 @param[in] l4_parser_ctl  l4 parser reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl);

/**
 @brief Get layer4 parser

 @param[in] tbl_flags  CTC_PARSER_LAYER4_TBL_FLAGS_XXX

 @param[in] l4_parser_ctl  l4 parser reg

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_get_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl);

/**
 @brief Add l4type,for parser application

 @param[in] index application entry index

 @param[in] entry  the entry

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_set_layer4_type(uint8 index, ctc_parser_l4_ptl_entry_t* entry);


/**@} end of @addtogroup   parser_l4 PARSER_L4*/

/**
 @brief Init parser module
 @return CTC_E_XXX

*/
extern int32
ctc_humber_parser_init(void* parser_global_cfg);

/**@} end of @addtogroup   parser PARSER*/

#endif

