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
 * @brief This file is the public header file of adapt_parser.c
 */

#ifndef __ADPT_PARSER_H__
#define __ADPT_PARSER_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
#define ADPT_PASER_SVLAN_DFT_TPID      0x8100
#define ADPT_PARSER_CVLAN_DFT_TPID      0x8100

enum adpt_parser_hash_use_e
{
    ADPT_PARSER_HASH_USE_MAC = 1 << 0,
    ADPT_PARSER_HASH_USE_IP  = 1 << 1,
    ADPT_PARSER_HASH_USE_L4  = 1 << 2
};
typedef enum adpt_parser_hash_use_e adpt_parser_hash_use_t;

enum adpt_parser_hash_key_e
{
    ADPT_PARSER_HASH_KEY_PORT     = 1 << 0,
    ADPT_PARSER_HASH_KEY_MACSA    = 1 << 1,
    ADPT_PARSER_HASH_KEY_MACDA    = 1 << 2,
    ADPT_PARSER_HASH_KEY_VID      = 1 << 3,
    ADPT_PARSER_HASH_KEY_COS      = 1 << 4,
    ADPT_PARSER_HASH_KEY_L2PRO    = 1 << 5,

    ADPT_PARSER_HASH_KEY_MAC_ALL  = ADPT_PARSER_HASH_KEY_PORT  |
                                    ADPT_PARSER_HASH_KEY_MACSA |
                                    ADPT_PARSER_HASH_KEY_MACDA |
                                    ADPT_PARSER_HASH_KEY_VID   |
                                    ADPT_PARSER_HASH_KEY_COS   |
                                    ADPT_PARSER_HASH_KEY_L2PRO,
    
    ADPT_PARSER_HASH_KEY_IPSA     = 1 << 6,
    ADPT_PARSER_HASH_KEY_IPDA     = 1 << 7,
    ADPT_PARSER_HASH_KEY_IPPRO    = 1 << 8,

    ADPT_PARSER_HASH_KEY_IP_ALL   = ADPT_PARSER_HASH_KEY_IPSA  |
                                    ADPT_PARSER_HASH_KEY_IPDA  |
                                    ADPT_PARSER_HASH_KEY_IPPRO,
    
    ADPT_PARSER_HASH_KEY_SRC_PORT = 1 << 9,
    ADPT_PARSER_HASH_KEY_DST_PORT = 1 << 10,

    ADPT_PARSER_HASH_KEY_L4_ALL   = ADPT_PARSER_HASH_KEY_SRC_PORT |
                                    ADPT_PARSER_HASH_KEY_DST_PORT
};
typedef enum adpt_parser_hash_key_e adpt_parser_hash_key_t;

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

/**
 * Set svlan tpid
 * @param[in] tpid                      stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_svlan_tpid(uint16_ofp tpid);

/**
 * Get svlan tpid
 * @param[out] p_tpid                   Pointer to stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_svlan_tpid(uint16_ofp* p_tpid);

/**
 * Set cvlan tpid
 * @param[in] tpid                      ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_cvlan_tpid(uint16_ofp tpid);

/**
 * Get cvlan tpid
 * @param[out] p_tpid                   Pointer to ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_cvlan_tpid(uint16_ofp* p_tpid);

/**
 * Set hash use ip or layer4
 * @param[in] hash_use                  Bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_hash_use(uint32_ofp hash_use);

/**
 * Get hash use
 * @param[out] p_hash_use               Pointer to bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_hash_use(uint32_ofp* p_hash_use);

/**
 * Set hash keys
 * @param[in] hash_keys                 Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_hash_key(uint32_ofp hash_keys);

/**
 * Get hash keys
 * @param[out] p_hash_keys              Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_hash_key(uint32_ofp* p_hash_keys);

/**
 * Adapter parser init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_init(void);

#endif
