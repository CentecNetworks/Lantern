/**
 @file ctc_humber_pdu.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-31

 @version v2.0

 This file contains pdu API function interfaces.
*/

#ifndef _CTC_HUMBER_PDU_H
#define _CTC_HUMBER_PDU_H

#include "kal.h"
#include "ctc_pdu.h"
#include "ctc_parser.h"

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup  pdu PDU
 @{
*/

/**
 @addtogroup l2pdu L2PDU
 @{
*/

/**
 @brief  Classify layer2 pdu based on macda,macda-low24 bit, layer2 header protocol

 @param[in] l2pdu_type  CTC_PDU_L2PDU_TYPE_XXX

 @param[in] index  classify layer2 pdu index,
                               based on macda,max index num is 4,
                               based on macda low24 bit, max index num is 8,
                               based on layer2 header protocol, max index num is 16,
                               based on layer3 type, max index num is 16,
                               based on bpdu,index is 1

 @param[in] key  layer2 pdu action key

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_classify_l2pdu(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key);

/**
 @brief  Get layer2 pdu key

 @param[in] l2pdu_type  CTC_PDU_L2PDU_TYPE_XXX

 @param[in] index  classify layer2 pdu index,
                               based on macda,max index num is 4,
                               based on macda low24 bit, max index num is 8,
                               based on layer2 header protocol, max index num is 16,
                               based on layer3 type, the index  is equal to l3type (CTC_PARSER_L3_TYPE_XXX),
                               based on bpdu,index num is 1

 @param[out] key  layer2 pdu action key

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_get_classified_key(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key);

/**
 @brief  Set layer2 pdu global property

 @param[in] l2pdu_type  CTC_PDU_L2PDU_TYPE_XXX

 @param[in] index  classify layer2 pdu index,
                               based on macda,max index num is 4,
                               based on macda low24 bit, max index num is 8,
                               based on layer2 header protocol, max index num is 16,
                               based on layer3 type, max index num is 16,
                               based on bpdu,index is 1
 @param[in] action  layer2 pdu global property filed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_set_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action);

/**
 @brief  Get layer2 pdu global property

 @param[in] l2pdu_type  CTC_PDU_L2PDU_TYPE_XXX

 @param[in] index  classify layer2 pdu index,
                               based on macda,max index num is 4,
                               based on macda low24 bit, max index num is 8,
                               based on layer2 header protocol, max index num is 16,
                               based on layer3 type, the index  is equal to l3type (CTC_PARSER_L3_TYPE_XXX),
                               based on bpdu,index num is 1
 @param[out] action  layer2 pdu global property filed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_get_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action);

/**
 @brief  Per port control layer2 pdu action

 @param[in] gport  global port

 @param[in] action_index  per port control action index,it's from action_index from ctc_pdu_global_l2pdu_action_t ds

 @param[in] action  layer2 pdu action type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_set_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t action);

/**
 @brief  Per port control layer2 pdu action

 @param[in] gport  global port

 @param[in] action_index  per port control action index,it's from action_index from ctc_pdu_global_l2pdu_action_t ds

 @param[out] action  layer2 pdu action type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l2pdu_get_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t* action);

/**@} end of @addtogroup  l2pdu L2PDU */


/**
 @addtogroup  l3pdu L3PDU
 @{
*/

/**
 @brief  Classify layer3 pdu based on layer3 header protocol, layer4 dest port

 @param[in] l3pdu_type  CTC_PDU_L3PDU_TYPE_XXX

 @param[in] index  classify layer3 pdu index,
                               based on layer3 header protocol, max index num is 16
                               based on layer4 dest port, max index num is 16
                               based on layer4 type, max index num is 16
 @param[in] key  layer3 pdu action key

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_classify_l3pdu(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index,
                                                                           ctc_pdu_l3pdu_key_t* key);
/**
 @brief  Get layer3 pdu classified key

 @param[in] l3pdu_type  CTC_PDU_L3PDU_TYPE_XXX

 @param[in] index  classify layer3 pdu index,
                               based on layer3 header protocol, max index num is 16
                               based on layer4 dest port, max index num is 16
                               based on layer4 type, the index  is equal to l4type (CTC_PARSER_L4_TYPE_XXX),
 @param[out] key  layer3 pdu action key

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_get_classified_key(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index,
                                                                           ctc_pdu_l3pdu_key_t* key);

/**
 @brief  Set layer3 pdu global property

 @param[in] l3pdu_type  CTC_PDU_L3PDU_TYPE_XXX

 @param[in] index  classify layer3 pdu index,
                               based on layer3 header protocol, max index num is 16
                               based on layer4 dest port, max index num is 16
                               based on layer4 type, max index num is 16

 @param[in] action  layer3 pdu global property filed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_set_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l3pdu_action_t* action);
/**
 @brief  Get layer3 pdu global property

 @param[in] l3pdu_type  CTC_PDU_L3PDU_TYPE_XXX

 @param[in] index  classify layer3 pdu index,
                               based on layer3 header protocol, max index num is 16
                               based on layer4 dest port, max index num is 16
                               based on layer4 type, the index  is equal to l4type (CTC_PARSER_L4_TYPE_XXX),

 @param[out] action  layer3 pdu global property filed

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_get_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l3pdu_action_t* action);

/**
 @brief  Per layer3 interface control layer3 pdu action

 @param[in] l3ifid  layer3 interface id

 @param[in] action_index  per layer3 interface control action index,it's from action_index from ctc_pdu_global_l3pdu_action_t ds

 @param[in] action  layer3 pdu action type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_set_l3if_action(uint16 l3ifid, uint8 action_index,
                                                                           ctc_pdu_l3if_l3pdu_action_t action);

/**
 @brief  Per layer3 interface control layer3 pdu action

 @param[in] l3ifid  layer3 interface id

 @param[in] action_index  per layer3 interface control action index,it's from action_index from ctc_pdu_global_l3pdu_action_t ds

 @param[out] action  layer3 pdu action type

 @return CTC_E_XXX

*/
extern int32
ctc_humber_l3pdu_get_l3if_action(uint16 l3ifid, uint8 action_index,
                                                                           ctc_pdu_l3if_l3pdu_action_t* action);

/**@} end of @addtogroup  l3pdu L3PDU */

/**
 @brief Init pdu module

 @return CTC_E_XXX

*/
extern int32
ctc_humber_pdu_init(void* pdu_global_cfg);

/**@} end of @addtogroup  pdu PDU*/


#endif


