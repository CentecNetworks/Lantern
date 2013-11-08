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
 * @brief This file calls the sdk parser APIs.
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_macro.h"
#include "adpt_parser.h"
#include "ctc_api.h"

#include "hal.h"
#include "hal_parser.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
/**
 * Get layer 2 type by ether_type
 * @param[in]  ether_type               ether type
 * @param[in]  ether_type_mask          ether type mask
 * @param[out] p_l3type                 pointer to ctc_parser_l3_type_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_get_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, ctc_parser_l3_type_t* p_l3type)
{
    ctc_parser_l2_ptl_entry_t l2_ptl_entry;

    memset(&l2_ptl_entry, 0, sizeof(l2_ptl_entry));
    l2_ptl_entry.isEth     = 1;
    l2_ptl_entry.l2hdr_ptl = ether_type;
    l2_ptl_entry.mask      = ether_type_mask;
    
    HAL_ERROR_RETURN(ctc_parser_get_l2_type(&l2_ptl_entry, p_l3type));

    return OFP_ERR_SUCCESS;
}

/**
 * Add layer 3 type -  ether_type map
 * @param[in]  ether_type               ether type
 * @param[in]  ether_type_mask          ether type mask
 * @param[in]  index                    index to map table
 * @param[out] p_l3type                 pointer to ctc_parser_l3_type_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_add_l3_type(uint16_ofp ether_type, uint16_ofp ether_type_mask, uint8_ofp index, ctc_parser_l3_type_t l3type)
{
    ctc_parser_l2_ptl_entry_t l2_ptl_entry;

    memset(&l2_ptl_entry, 0, sizeof(l2_ptl_entry));
    l2_ptl_entry.isEth     = 1;
    l2_ptl_entry.l2hdr_ptl = ether_type;
    l2_ptl_entry.mask      = ether_type_mask;
    l2_ptl_entry.l3_type   = l3type;
    
    HAL_ERROR_RETURN(ctc_parser_add_l2_type(index, &l2_ptl_entry));

    return OFP_ERR_SUCCESS;
}

/**
 * Remove layer 3 type -  ether_type map
 * @param[in]  index                    index to map table
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_remove_l2_type(uint8_ofp index)
{
    HAL_ERROR_RETURN(ctc_parser_remove_l2_type(index));

    return OFP_ERR_SUCCESS;
}

/**
 * Set svlan tpid
 * @param[in] tpid                      stag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_svlan_tpid(uint16_ofp tpid)
{
    HAL_ERROR_RETURN(ctc_parser_set_tpid(CTC_PARSER_L2_TPID_SVLAN_TPID_0 + OFP_SVLAN_DFT_TPID_IDX, tpid));

    return OFP_ERR_SUCCESS;
}

/**
 * Set cvlan tpid
 * @param[in] tpid                      ctag tpid
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_cvlan_tpid(uint16_ofp tpid)
{
    HAL_ERROR_RETURN(ctc_parser_set_tpid(CTC_PARSER_L2_TPID_CVLAN_TPID, tpid));

    return OFP_ERR_SUCCESS;
}

/**
 * Set hash use ip or layer4
 * @param[in] hash_use                  Bitmap of adpt_parser_hash_use_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_hash_use(uint32_ofp hash_use)
{
    ctc_parser_ip_hash_ctl_t ip_hash_ctl;
    ctc_parser_layer4_parser_ctl_t l4_parser_ctl;
    ctc_parser_layer4_tbl_flags_t tbl_flags = 0;

    memset(&ip_hash_ctl, 0, sizeof(ip_hash_ctl));
    memset(&l4_parser_ctl, 0, sizeof(l4_parser_ctl));

    HAL_ERROR_RETURN(ctc_parser_get_ip_hash_ctl(&ip_hash_ctl));

    ip_hash_ctl.use_ip_hash = 0;
    SET_FLAG(ip_hash_ctl.flags, CTC_PARSER_IP_HASH_FLAGS_USE_IP_HASH);
    if (IS_FLAG_SET(hash_use, ADPT_PARSER_HASH_USE_IP))
    {
        ip_hash_ctl.use_ip_hash = 1;
    }
    HAL_ERROR_RETURN(ctc_parser_set_ip_hash_ctl(&ip_hash_ctl));

    tbl_flags |= CTC_PARSER_LAYER4_TBL_FLAGS_L4HASH;
    HAL_ERROR_RETURN(ctc_parser_get_layer4_parser_ctl(tbl_flags, &l4_parser_ctl));

    l4_parser_ctl.l4hash_ctl.use_l4hash = 0;
    SET_FLAG(l4_parser_ctl.l4hash_ctl.field_flags, CTC_PARSER_L4_FLAGS_USE_L4HASH);
    if (IS_FLAG_SET(hash_use, ADPT_PARSER_HASH_USE_L4))
    {
        l4_parser_ctl.l4hash_ctl.use_l4hash = 1;
    }
    HAL_ERROR_RETURN(ctc_parser_set_layer4_parser_ctl(tbl_flags, &l4_parser_ctl));
 
    return OFP_ERR_SUCCESS;
}

/**
 * Set hash keys
 * @param hash_keys                     Pointer to bitmap of adpt_parser_hash_key_t
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_set_hash_key(uint32_ofp hash_keys)
{
    ctc_parser_l2_ctl_fld_t l2ctl;
    ctc_parser_ip_hash_ctl_t ip_hash_ctl;
    ctc_parser_layer4_tbl_flags_t tbl_flags;
    ctc_parser_layer4_parser_ctl_t l4_parser_ctl;

    memset(&l2ctl, 0, sizeof(l2ctl));
    memset(&ip_hash_ctl, 0, sizeof(ip_hash_ctl));
    memset(&l4_parser_ctl, 0, sizeof(l4_parser_ctl));

    /* 1. set mac hash key */
    HAL_ERROR_RETURN(ctc_parser_get_l2_hash(&l2ctl));
    l2ctl.port_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_PORT_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_PORT))
    {
        l2ctl.port_hash = 1;
    }

    l2ctl.linkagg_macsa_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACSA_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_MACSA))
    {
        l2ctl.linkagg_macsa_hash = 1;
    }

    l2ctl.linkagg_macda_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACDA_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_MACDA))
    {
        l2ctl.linkagg_macda_hash = 1;
    }

    l2ctl.vlan_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_VLAN_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_VID))
    {
        l2ctl.vlan_hash = 1;
    }
    
    l2ctl.cos_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_COS_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_COS))
    {
        l2ctl.cos_hash = 1;
    }

    l2ctl.l2hdr_ptl_hash = 0;
    SET_FLAG(l2ctl.flags, CTC_PARSER_L2_HASH_FIELD_L2_HDR_PRL_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_L2PRO))
    {
        l2ctl.l2hdr_ptl_hash = 1;
    }
    HAL_ERROR_RETURN(ctc_parser_set_l2_hash(&l2ctl));

    /* 2. set ip hash key */
    HAL_ERROR_RETURN(ctc_parser_get_ip_hash_ctl(&ip_hash_ctl));
    ip_hash_ctl.linkagg_ipsa_hash = 0;
    SET_FLAG(ip_hash_ctl.flags, CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPSA_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_IPSA))
    {
        ip_hash_ctl.linkagg_ipsa_hash = 1;
    }

    ip_hash_ctl.linkagg_ipda_hash = 0;
    SET_FLAG(ip_hash_ctl.flags, CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPDA_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_IPDA))
    {
        ip_hash_ctl.linkagg_ipda_hash = 1;
    }

    ip_hash_ctl.ptl_hash = 0;
    SET_FLAG(ip_hash_ctl.flags, CTC_PARSER_IP_HASH_FLAGS_PROTOCOL_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_IPPRO))
    {
        ip_hash_ctl.ptl_hash = 1;
    }
    HAL_ERROR_RETURN(ctc_parser_set_ip_hash_ctl(&ip_hash_ctl));

    /* 2. set layer4 hash key */
    tbl_flags = CTC_PARSER_LAYER4_TBL_FLAGS_L4HASH;
    HAL_ERROR_RETURN(ctc_parser_get_layer4_parser_ctl(tbl_flags, &l4_parser_ctl));
    l4_parser_ctl.l4hash_ctl.src_port_hash = 0;
    SET_FLAG(l4_parser_ctl.l4hash_ctl.field_flags, CTC_PARSER_L4_FLAGS_SRC_PORT_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_SRC_PORT))
    {
        l4_parser_ctl.l4hash_ctl.src_port_hash = 1;
    }

    l4_parser_ctl.l4hash_ctl.dst_port_hash = 0;
    SET_FLAG(l4_parser_ctl.l4hash_ctl.field_flags, CTC_PARSER_L4_FLAGS_DST_PORT_HASH);
    if (IS_FLAG_SET(hash_keys, ADPT_PARSER_HASH_KEY_DST_PORT))
    {
        l4_parser_ctl.l4hash_ctl.dst_port_hash = 1;
    }
    HAL_ERROR_RETURN(ctc_parser_set_layer4_parser_ctl(tbl_flags, &l4_parser_ctl));
    
    return OFP_ERR_SUCCESS;
}

/**
 * init sdk parser module
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_parser_init(void)
{
    /* Disable IPv6/MPLS parser */
    HAL_ERROR_RETURN(ctc_parser_l2_enable_l3_type(CTC_PARSER_L3_TYPE_IPV6, FALSE));
    HAL_ERROR_RETURN(ctc_parser_l2_enable_l3_type(CTC_PARSER_L3_TYPE_MPLS, TRUE));
    HAL_ERROR_RETURN(ctc_parser_l2_enable_l3_type(CTC_PARSER_L3_TYPE_MPLS_MCAST, FALSE));
    HAL_ERROR_RETURN(ctc_parser_set_parser_quad_vlan_en(TRUE));

    return OFP_ERR_SUCCESS;
}
