/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file is the database implementation of ctc_api
 */

#ifndef __HAL_PARSER_H__
#define __HAL_PARSER_H__ 

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ctc_parser.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
#define OFP_SVLAN_DFT_TPID_IDX       2

/****************************************************************************
 *  
 * Function
 *
 ***************************************************************************/

/**
 * Get layer 2 type by ether_type
 * @param[in]  ether_type               ether type
 * @param[in]  ether_type_mask          ether type mask
 * @param[out] p_l3type                 pointer to ctc_parser_l3_type_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_get_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, ctc_parser_l3_type_t* p_l3type);

/**
 * Add layer 3 type -  ether_type map
 * @param[in]  ether_type               ether type
 * @param[in]  ether_type_mask          ether type mask
 * @param[in]  index                    index to map table
 * @param[out] p_l3type                 pointer to ctc_parser_l3_type_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_add_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp index, ctc_parser_l3_type_t l3type);

/**
 * Remove layer 3 type -  ether_type map
 * @param[in]  index                    index to map table
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_remove_l2_type(uint8_ofp index);

/**
 * Set svlan tpid
 * @param[in] tpid                      stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_svlan_tpid(uint16_ofp tpid);

/**
 * Set cvlan tpid
 * @param[in] tpid                      ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_cvlan_tpid(uint16_ofp tpid);

/**
 * Set hash use ip or layer4
 * @param[in] hash_use                  Bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_hash_use(uint32_ofp hash_use);

/**
 * Set hash keys
 * @param hash_keys                     Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_hash_key(uint32_ofp hash_keys);

/**
 * init sdk parser module
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_init(void);

#endif /* !__HAL_PARSER_H__ */
