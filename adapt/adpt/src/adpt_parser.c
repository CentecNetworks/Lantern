/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file is the main file of adapter layer parser
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "afx.h"
#include "ofp_api.h"
#include "glb_stm_define.h"
#include "adpt.h"
#include "adpt_flow.h"
#include "adpt_parser.h"
#include "adpt_parser_priv.h"
#include "hal_parser.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_parser);

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
static adpt_parser_master_t* g_p_adpt_parser_master;

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Set svlan tpid
 * @param[in] tpid                      stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_svlan_tpid(uint16_ofp tpid)
{
    ADPT_LOG_DEBUG_FUNC();
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);

    if (g_p_adpt_parser_master->svlan_tpid == tpid)
    {
        return OFP_ERR_SUCCESS;
    }

    if (adpt_flowdb_get_flow_entry_cur_num() != 0)
    {
        return OFP_ERR_SET_TPID_FLOW_NOT_EMPTY;
    }

    ADPT_ERROR_RETURN(hal_parser_set_svlan_tpid(tpid));
    g_p_adpt_parser_master->svlan_tpid = tpid;

    return OFP_ERR_SUCCESS;
}

/**
 * Get svlan tpid
 * @param[out] p_tpid                   Pointer to stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_svlan_tpid(uint16_ofp* p_tpid)
{
    ADPT_LOG_DEBUG_FUNC();
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);
    ADPT_PTR_CHECK(p_tpid);
    *p_tpid = g_p_adpt_parser_master->svlan_tpid;

    return OFP_ERR_SUCCESS;
}

/**
 * Set cvlan tpid
 * @param[in] tpid                      ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_cvlan_tpid(uint16_ofp tpid)
{
    ADPT_LOG_DEBUG_FUNC();
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);

    if (g_p_adpt_parser_master->cvlan_tpid == tpid)
    {
        return OFP_ERR_SUCCESS;
    }

    if (adpt_flowdb_get_flow_entry_cur_num() != 0)
    {
        return OFP_ERR_SET_TPID_FLOW_NOT_EMPTY;
    }

    ADPT_ERROR_RETURN(hal_parser_set_cvlan_tpid(tpid));
    g_p_adpt_parser_master->cvlan_tpid = tpid;

    return OFP_ERR_SUCCESS;
}

/**
 * Get cvlan tpid
 * @param[out] p_tpid                   Pointer to ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_cvlan_tpid(uint16_ofp* p_tpid)
{
    ADPT_LOG_DEBUG_FUNC();
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);
    ADPT_PTR_CHECK(p_tpid);
    
    *p_tpid = g_p_adpt_parser_master->cvlan_tpid;

    return OFP_ERR_SUCCESS;
}

/**
 * Set hash use ip or layer4
 * @param[in] hash_use                  Bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_hash_use(uint32_ofp hash_use)
{
    uint32_ofp hash_keys;
    
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);

    hash_keys = g_p_adpt_parser_master->linkagg_hash_keys;
    if (IS_FLAG_SET(hash_use, ADPT_PARSER_HASH_USE_MAC))
    {
        ADPT_ERROR_RETURN(hal_parser_set_hash_key(hash_keys));
    }
    else
    {
        UNSET_FLAG(hash_keys, ADPT_PARSER_HASH_KEY_MAC_ALL);
        ADPT_ERROR_RETURN(hal_parser_set_hash_key(hash_keys));
    }
    ADPT_ERROR_RETURN(hal_parser_set_hash_use(hash_use));
    g_p_adpt_parser_master->linkagg_hash_use = hash_use;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get hash use
 * @param[out] p_hash_use               Pointer to bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_hash_use(uint32_ofp* p_hash_use)
{
    ADPT_PTR_CHECK(p_hash_use);
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);
    
    *p_hash_use = g_p_adpt_parser_master->linkagg_hash_use;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Set hash keys
 * @param[in] hash_keys                Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_set_hash_key(uint32_ofp hash_keys)
{
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);

    g_p_adpt_parser_master->linkagg_hash_keys = hash_keys;
    if (!IS_FLAG_SET(g_p_adpt_parser_master->linkagg_hash_use, ADPT_PARSER_HASH_USE_MAC))
    {
        UNSET_FLAG(hash_keys, ADPT_PARSER_HASH_KEY_MAC_ALL);
    }
    ADPT_ERROR_RETURN(hal_parser_set_hash_key(hash_keys));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get hash keys
 * @param[out] p_hash_keys              Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_get_hash_key(uint32_ofp* p_hash_keys)
{
    ADPT_PTR_CHECK(p_hash_keys);
    ADPT_MODULE_INIT_CHECK(g_p_adpt_parser_master);
    
    *p_hash_keys = g_p_adpt_parser_master->linkagg_hash_keys;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Adapter parser init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_parser_init(void)
{
    g_p_adpt_parser_master = malloc(sizeof(adpt_parser_master_t));
    ADPT_MEM_PTR_CHECK(g_p_adpt_parser_master);
    memset(g_p_adpt_parser_master, 0, sizeof(adpt_parser_master_t));

    SET_FLAG(g_p_adpt_parser_master->linkagg_hash_use, ADPT_PARSER_HASH_USE_MAC);
    SET_FLAG(g_p_adpt_parser_master->linkagg_hash_use, ADPT_PARSER_HASH_USE_IP);
    SET_FLAG(g_p_adpt_parser_master->linkagg_hash_keys, ADPT_PARSER_HASH_KEY_MAC_ALL);
    SET_FLAG(g_p_adpt_parser_master->linkagg_hash_keys, ADPT_PARSER_HASH_KEY_IP_ALL);
    SET_FLAG(g_p_adpt_parser_master->linkagg_hash_keys, ADPT_PARSER_HASH_KEY_L4_ALL);

    g_p_adpt_parser_master->cvlan_tpid = ADPT_PARSER_CVLAN_DFT_TPID;
    g_p_adpt_parser_master->svlan_tpid =  ADPT_PASER_SVLAN_DFT_TPID;

    hal_parser_set_hash_use(g_p_adpt_parser_master->linkagg_hash_use);
    hal_parser_set_hash_key(g_p_adpt_parser_master->linkagg_hash_keys);

    return 0;
}
