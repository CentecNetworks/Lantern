
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
 * @brief This file 
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ofp_api.h"

#include "adpt.h"
#include "adpt_flow.h"
#include "adpt_port.h"
#include "adpt_nexthop.h"
#include "adpt_nexthop_priv.h"
#include "adpt_opf.h"
#include "adpt_parser.h"

#include "hal_nexthop.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
adpt_nexthop_master_t* g_p_adpt_nexthop_master;

VLOG_DEFINE_THIS_MODULE(adapt_nexthop);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Allocate the next-hop info opf
 * @param[in]  type                     ofp_nh_info_type_t
 * @param[out] offset                   pointer to ofp_nh_info_type_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_nh_info_opf(ofp_nh_info_type_t type, uint32_ofp* p_offset)
{
    ADPT_PTR_CHECK(p_offset);
    
    switch(type)
    {
    case NH_INFO_TYPE_NH_ID:
        ADPT_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_NH_ID, 1, p_offset));
        break;
        
    case NH_INFO_TYPE_GLB_MET_OFFSET:
        ADPT_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_GLB_MET, 1, p_offset));
        break;
        
    case  NH_INFO_TYPE_GLB_NH_OFFSET:
        ADPT_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_GLB_NH, 1, p_offset));
        break;
        
    case NH_INFO_TYPE_GLB_NH_OFFSET_8W:
        ADPT_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_GLB_NH, 2, p_offset));
        break;
        
    case NH_INFO_TYPE_MAX:
    default:
        return OFP_ERR_FAIL;
    }
    return OFP_ERR_SUCCESS;
}

/**
 * Release the next-hop info opf
 * @param[in]  type                     ofp_nh_info_type_t
 * @param[in]  offset                   offset
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info_opf(ofp_nh_info_type_t type, uint32_ofp offset)
{
    switch(type)
    {
    case NH_INFO_TYPE_NH_ID:
        if (!adpt_nexthop_is_reserved_nhid(offset))
        {
            ADPT_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_NH_ID, 1, offset)); 
        }
        break;
        
    case NH_INFO_TYPE_GLB_MET_OFFSET:
        ADPT_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_GLB_MET, 1, offset)); 
        break;
        
    case  NH_INFO_TYPE_GLB_NH_OFFSET:
        ADPT_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_GLB_NH, 1, offset)); 
        break;
        
    case NH_INFO_TYPE_GLB_NH_OFFSET_8W:
        ADPT_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_GLB_NH, 2, offset));
        break;

    case NH_INFO_TYPE_MAX:
    default:
        return OFP_ERR_FAIL;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Is reserved nhid
 * @param[in] nhid                      nhid
 * @return true/false
 */
bool
adpt_nexthop_is_reserved_nhid(uint32_ofp nhid)
{
    if (nhid < ADPT_NH_RSV_NHID_MAX)
    {
        return true;
    }

    return false;
}

/**
 * release the next-hop info opf for the specified type
 * @param[in] nh_offset                 ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info(ofp_nh_offset_t *nh_offset)
{
    ADPT_PTR_CHECK(nh_offset);

    if (!adpt_nexthop_is_reserved_nhid(nh_offset->nhid))
    {
        if (OPF_NH_TYPE_NH_FLEX_QINQ == nh_offset->nh_type)
        {
            adpt_flowdb_decr_qinq_with_mac_cur_num();
        }

        if (OPF_NH_TYPE_NH_FLEX_IPDA == nh_offset->nh_type)
        {
            ofp_nexthop_info_t nexthop_info;

            memset(&nexthop_info, 0, sizeof(nexthop_info));

            nexthop_info.gre_and_mpls_push_output_count = 1;
            adpt_flow_op_nexthop_res(&nexthop_info, ADPT_RES_OP_TYPE_DEL);
        }

        if (OPF_NH_TYPE_NH_FLEX == nh_offset->nh_type ||
            OPF_NH_TYPE_NH_FLEX_QINQ == nh_offset->nh_type ||
            OPF_NH_TYPE_NH_FLEX_IPDA == nh_offset->nh_type ||
            OPF_NH_TYPE_NH_FLEX_USER_DEFINED_OFFSET == nh_offset->nh_type)
        {
            ADPT_ERROR_RETURN(hal_nexthop_remove_flex_nh(nh_offset->nhid));
        }

        if ((OPF_NH_TYPE_NH_MPLS == nh_offset->nh_type) ||
            (OPF_NH_TYPE_NH_MPLS_VPWS == nh_offset->nh_type) ||
            (OPF_NH_TYPE_NH_MPLS_VPLS == nh_offset->nh_type))
        {
            ADPT_ERROR_RETURN(hal_nexthop_remove_mpls_nh(nh_offset->nhid));
        }
        
        if (OPF_NH_TYPE_NH_IPUC == nh_offset->nh_type)
        {
            ADPT_ERROR_RETURN(hal_nexthop_remove_ipuc_nh(nh_offset->nhid));
        }

        ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, nh_offset->nhid));
    }

    if (0 != nh_offset->offset)
    {
        if ((OPF_NH_TYPE_NH_MPLS_VPWS == nh_offset->nh_type) ||
            (OPF_NH_TYPE_NH_MPLS_VPLS == nh_offset->nh_type) ||
            (OPF_NH_TYPE_NH_FLEX_QINQ == nh_offset->nh_type) ||
            (OPF_NH_TYPE_NH_FLEX_IPDA == nh_offset->nh_type))
        {
            ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET_8W, nh_offset->offset));
        }
        else 
        {
            if (OPF_NH_TYPE_NH_FLEX_USER_DEFINED_OFFSET != nh_offset->nh_type)
            {
                ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, nh_offset->offset));
            }
        }
    }

    nh_offset->nhid = 0;
    nh_offset->offset = 0;
    nh_offset->nh_type = OPF_NH_TYPE_MAX;

    return OFP_ERR_SUCCESS;
}

/**
 * release all the next-hop info opf for the specified type
 * @param[in] p_nh_info                 ofp_nexthop_info_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_info_res(ofp_nexthop_info_t *p_nh_info)
{
    uint32_ofp key = 0;

    ADPT_PTR_CHECK(p_nh_info);

    /*for single action(MUST be output action), we will not allocate mcast group, just
      free the resources and return*/
    if (!p_nh_info->use_mcast)
    {
        adpt_nexthop_release_nh_info(&(p_nh_info->main_nh));
        
        return OFP_ERR_SUCCESS;
    }

    /*multiple action(including MPLS rule) will use mcast group to send packets out, so we should
      free the resources including the mcast group*/
    for (key = 0; key < p_nh_info->output_count; key++)
    {
        adpt_nexthop_release_nh_info(&(p_nh_info->member_nh[key]));
    }
    adpt_nexthop_release_mcast_group(&p_nh_info->main_nh);
    
    p_nh_info->use_mcast = FALSE;
    
    return OFP_ERR_SUCCESS;
}

/**
 * allocate the multi-cast group id
 * @param[in] p_action                  adpt_flow_action_combo_t
 * @param[out] p_nh_param               ctc_flex_nh_param_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_map_flex_nh_param(adpt_flow_action_combo_t* p_action, ctc_flex_nh_param_t *p_nh_param)
{
    ADPT_PTR_CHECK(p_action);
    ADPT_PTR_CHECK(p_nh_param);

    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_OUTPUT))    
    {
        p_nh_param->gport = p_action->output_gport;
    }

    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_SET_MACSA))
    {
        memcpy(p_nh_param->mac_sa, p_action->mac_sa, sizeof(mac_addr_t));
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_MAC_SA);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_SET_MACDA))
    {
        memcpy(p_nh_param->mac_da, p_action->mac_da, sizeof(mac_addr_t));
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_MAC_DA);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID))
    {
        p_nh_param->vlan_id = p_action->vlan_id;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_SVLAN_TAG);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_REPLACE_CVLAN_VID))
    {
        p_nh_param->cvlan_id = p_action->cvlan_id;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_CVLAN_TAG);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN))
    {
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_STRIP_SVLAN_TAG);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN))
    {
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_STRIP_CVLAN_TAG);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_COS))
    {
        p_nh_param->cos = p_action->vlan_pcp;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_STAG_COS);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN))
    {
        uint16_ofp stag_tpid;

        if (!p_action->vlan_id)
        {
            OFP_LOG_ERROR("Adding flow/group failed, should set vlan vid after push vlan .");
            return OFP_ERR_ACTION_VLAN_PUSH_WO_VID;
        }

        adpt_parser_get_svlan_tpid(&stag_tpid);
        if (stag_tpid != p_action->stag_tpid)
        {
            OFP_LOG_ERROR("Adding flow/group failed, cannot push_vlan:0x%x, the configured stag tpid is 0x%x.",
                    p_action->stag_tpid, stag_tpid);
            return OFP_ERR_ACTION_VLAN_PUSH_ONLY_TPID;
        }

        p_nh_param->vlan_id = p_action->vlan_id;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_INSERT_SVLAN_TAG);
    }
    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_PUSH_CVLAN))
    {
        uint16_ofp ctag_tpid;

        if (!p_action->cvlan_id)
        {
            OFP_LOG_ERROR("Adding flow/group failed, should set vlan vid after push vlan .");
            return OFP_ERR_ACTION_VLAN_PUSH_WO_VID;
        }

        adpt_parser_get_cvlan_tpid(&ctag_tpid);
        if (ctag_tpid != p_action->ctag_tpid)
        {
            OFP_LOG_ERROR("Adding flow/group failed, cannot push_vlan:0x%x, the configured ctag tpid is 0x%x.",
                    p_action->ctag_tpid, ctag_tpid);
            return OFP_ERR_ACTION_VLAN_PUSH_ONLY_TPID;
        }

        p_nh_param->cvlan_id = p_action->cvlan_id;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_INSERT_CVLAN_TAG);
    }

    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_REPLACE_IPDA))
    {
        p_nh_param->ipda = p_action->ipda;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_IPDA);
    }

    if (IS_FLAG_SET(p_action->flag, OFP_FLOW_ACTION_FIELD_REPLACE_DST_PORT))
    {
        p_nh_param->dst_port = p_action->dst_port;
        SET_FLAG(p_nh_param->flag, CTC_FLEX_NH_REPLACE_DST_PORT);
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Allocate to cpu flex next hop with flow_id
 * @param[in]  flow_id                  flow id
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_to_cpu_flex_nh(uint32_ofp flow_id, ofp_nh_offset_t* p_nh_offset)
{
    ctc_flex_nh_param_t nh_param;

    ADPT_PTR_CHECK(p_nh_offset);

    memset(&nh_param, 0x0, sizeof(ctc_flex_nh_param_t));
    nh_param.dsnh_offset = flow_id;
    nh_param.gport = CPU_PORT_NUM;
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_nh_offset->nhid));
    ADPT_ERROR_RETURN(hal_nexthop_create_flex_nh(p_nh_offset->nhid, &nh_param));

    p_nh_offset->offset = nh_param.dsnh_offset;
    p_nh_offset->nh_type = OPF_NH_TYPE_NH_FLEX_USER_DEFINED_OFFSET;

    return OFP_ERR_SUCCESS;
}

/**
 * allocate the multi-cast flex next-hop
 * @param[in] p_action_combo            adpt_flow_action_combo_t
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_flex_nh(adpt_flow_action_combo_t* p_action_combo, ofp_nh_offset_t* p_nh_offset)
{
    int ret = OFP_ERR_SUCCESS;
    ctc_flex_nh_param_t nh_param;
    bool use_dsnh8w = false;
    bool is_qinq = false;
    bool is_write_ipda = false;

    ADPT_PTR_CHECK(p_action_combo);
    ADPT_PTR_CHECK(p_nh_offset);

    if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_DEC_IP_TTL))
    {
        ADPT_ERROR_RETURN(adpt_nexthop_alloc_ipuc_nh(p_action_combo, p_nh_offset));
        return OFP_ERR_SUCCESS;
    }

    if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN) &&
        IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_CVLAN) &&
        (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_MACDA) ||
         IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_MACSA)) )
    {
        is_qinq    = true;
        use_dsnh8w = true;
    }

    if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_IPDA) ||
        IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_DST_PORT))
    {
        is_write_ipda = true;
        use_dsnh8w    = true;
    }

    if (is_write_ipda)
    {
        ofp_nexthop_info_t nexthop_info;

        memset(&nexthop_info, 0, sizeof(nexthop_info));

        nexthop_info.gre_and_mpls_push_output_count = 1;
        if (OFP_ERR_SUCCESS != adpt_flow_op_nexthop_res(&nexthop_info, ADPT_RES_OP_TYPE_CHECK))
        {
            OFP_LOG_ERROR("Add flow/group failed, resource is full");
            return OFP_ERR_ALL_TABLES_FULL;
        }

        adpt_flow_op_nexthop_res(&nexthop_info, ADPT_RES_OP_TYPE_ADD);
    }
    /* write ipda use dsnh8w also, so we use 'else' here */
    else if (is_qinq)
    {
        if (adpt_flowdb_get_qinq_with_mac_cur_num() >= adpt_flowdb_get_qinq_with_mac_max_num())
        {
            OFP_LOG_ERROR("Only support %d output with pushing two vlan tag and modifying mac\n",
                adpt_flowdb_get_qinq_with_mac_max_num());
            return OFP_ERR_ACTION_VLAN_PUSH_W_MAC;
        }
        adpt_flowdb_incr_qinq_with_mac_cur_num();
    }

    if (use_dsnh8w)
    {
        ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET_8W, &p_nh_offset->offset));
    }
    else
    {
        ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, &p_nh_offset->offset));
    }

    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_nh_offset->nhid));

    memset(&nh_param, 0x0, sizeof(ctc_flex_nh_param_t));
    nh_param.dsnh_offset = p_nh_offset->offset;

    ADPT_ERROR_RETURN(adpt_nexthop_map_flex_nh_param(p_action_combo, &nh_param));
    ADPT_ERROR_RETURN(hal_nexthop_create_flex_nh(p_nh_offset->nhid, &nh_param));

    if (use_dsnh8w)
    {
        if (is_write_ipda)
        {
            p_nh_offset->nh_type = OPF_NH_TYPE_NH_FLEX_IPDA;
        }
        else
        {
            p_nh_offset->nh_type = OPF_NH_TYPE_NH_FLEX_QINQ;
        }
    }
    else
    {
        p_nh_offset->nh_type = OPF_NH_TYPE_NH_FLEX;
    }

    return ret;
}

/**
 * allocate ipuc next-hop to support decrease ip ttl
 * @param[in] p_action_combo            adpt_flow_action_combo_t
 * @param[out] p_nh_offset              pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_ipuc_nh(adpt_flow_action_combo_t* p_action_combo, ofp_nh_offset_t* p_nh_offset)
{
    ctc_ip_nh_param_t nh_param;
    
#define NEED_ACTION_FOR_DEC_TTL(check_flag)                                     \
do {                                                                            \
    if (!IS_FLAG_SET(p_action_combo->flag, check_flag))                         \
    {                                                                           \
        ADPT_LOG_ERROR("Adding flow failed, need %s together with DEC_NW_TTL",  \
                       ADPT_FLOW_ACTION_TYPE_TO_STR(check_flag));               \
        return OFP_ERR_INVALID_ACTION_LIST;                                     \
    }                                                                           \
}while(0)

#define NO_ACTION_FOR_DEC_TTL(check_flag)                                       \
do {                                                                            \
    if (IS_FLAG_SET(p_action_combo->flag, check_flag))                          \
    {                                                                           \
        ADPT_LOG_ERROR("Adding flow failed, cannot %s together with DEC_NW_TTL",\
                       ADPT_FLOW_ACTION_TYPE_TO_STR(check_flag));               \
        return OFP_ERR_INVALID_ACTION_LIST;                                     \
    }                                                                           \
}while(0)

    ADPT_PTR_CHECK(p_nh_offset);
    ADPT_PTR_CHECK(p_action_combo);

    NEED_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_SET_MACDA);
    NEED_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_OUTPUT);

    /* not support modify macsa and vlan currently */
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_SET_MACSA);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_COS);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_REPLACE_CVLAN_VID);
    /* OFP_FLOW_ACTION_FIELD_STRIP_SVLAN is default action */
    /* OFP_FLOW_ACTION_FIELD_STRIP_CVLAN is default action */
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_PUSH_SVLAN);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_PUSH_CVLAN);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_REPLACE_IPDA);
    NO_ACTION_FOR_DEC_TTL(OFP_FLOW_ACTION_FIELD_REPLACE_DST_PORT);

    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, &p_nh_offset->offset));
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_nh_offset->nhid));

    memset(&nh_param, 0x0, sizeof(ctc_ip_nh_param_t));
    nh_param.dsnh_offset  = p_nh_offset->offset;

    nh_param.opcode       = CTC_IP_NH_OP_IP_ROUTE;
    SET_FLAG(nh_param.flag, CTC_IP_NH_FLAG_USE_PACKET_TTL);
    nh_param.oif.gport    = p_action_combo->output_gport;
    nh_param.oif.vid      = CTC_MAX_VLAN_ID;
    nh_param.oif.oif_type = CTC_NH_OIF_TYPE_ROUTED_PORT;
    memcpy(nh_param.mac, &p_action_combo->mac_da, sizeof(mac_addr_t));

    ADPT_ERROR_RETURN(hal_nexthop_create_ipuc_nh(p_nh_offset->nhid, &nh_param));

    p_nh_offset->nh_type = OPF_NH_TYPE_NH_IPUC;

    return OFP_ERR_SUCCESS;
}

/**
 * allocate the multi-cast group id
 * @param[out] p_group_nh               pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_mcast_group(ofp_nh_offset_t* p_group_nh)
{
    ADPT_PTR_CHECK(p_group_nh);
    
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_group_nh->nhid));
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_MET_OFFSET, &p_group_nh->offset));

    ADPT_ERROR_RETURN(hal_nexthop_create_mcast_group(p_group_nh->nhid, p_group_nh->offset));

    return OFP_ERR_SUCCESS;
}

/**
 * release multi-cast group
 * @param[in] p_group_nh                pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_mcast_group(ofp_nh_offset_t* p_group_nh)
{
    ADPT_PTR_CHECK(p_group_nh);

    if (!adpt_nexthop_is_reserved_nhid(p_group_nh->nhid))
    {
        ADPT_ERROR_RETURN(hal_nexthop_remove_mcast_group(p_group_nh->nhid));
        ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, p_group_nh->nhid));
    }
    
    if (0 != p_group_nh->offset)
    {
        ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_MET_OFFSET, p_group_nh->offset));
    }
    memset(p_group_nh, 0x0, sizeof(ofp_nh_offset_t));

    return OFP_ERR_SUCCESS;
}

/**
 * allocate nexthop offset by type
 * @param[in]  type                     ofp_nh_type_t
 * @param[out] p_nh                     pointer to ofp_nh_offset_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_alloc_nexthop_offset(ofp_nh_type_t type, ofp_nh_offset_t* p_nh)
{
    ADPT_PTR_CHECK(p_nh);

    ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_NH_ID, &p_nh->nhid));
    
    if(type == OPF_NH_TYPE_NH_MPLS_VPWS ||
            type == OPF_NH_TYPE_NH_MPLS_VPLS ||
            type == OPF_NH_TYPE_NH_FLEX_QINQ ||
            type == OPF_NH_TYPE_NH_FLEX_IPDA)
    {
        ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET_8W, &p_nh->offset));
    }
    else
    {
        ADPT_ERROR_RETURN(adpt_nexthop_alloc_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, &p_nh->offset));
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * release the multi-cast group id
 * @param[in] p_nh                      pointer to ofp_nh_offset_t
 * @param[in] type                      ofp_nh_type_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_release_nh_id(ofp_nh_offset_t *p_nh, ofp_nh_type_t type)
{
    bool is_glb_nh_8w = FALSE;
    
    ADPT_PTR_CHECK(p_nh);

    if (p_nh->nhid)
    {
        ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_NH_ID, p_nh->nhid));
    }
    
    if(type == OPF_NH_TYPE_NH_MPLS_VPWS ||
            type == OPF_NH_TYPE_NH_MPLS_VPWS ||
            type == OPF_NH_TYPE_NH_FLEX_QINQ ||
            type == OPF_NH_TYPE_NH_FLEX_IPDA)
    {
        is_glb_nh_8w = TRUE;
    }
    
    if (p_nh->offset)
    {
        if (is_glb_nh_8w)
        {
            ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET_8W, p_nh->offset));
        }
        else
        {
            ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info_opf(NH_INFO_TYPE_GLB_NH_OFFSET, p_nh->offset));
        }
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Create nhid for output:all
 * @return OFP_ERR_XX
 */
static int32_ofp
adpt_nexthop_create_output_all_nh(void)
{
    ofp_nh_offset_t group_nh;

    ADPT_ERROR_RETURN(adpt_nexthop_alloc_mcast_group(&group_nh));

    memcpy(&g_p_adpt_nexthop_master->group_nh, &group_nh, sizeof(ofp_nh_offset_t));

    return OFP_ERR_SUCCESS;
}

/**
 * Get nh offset for output:all
 * @return OFP_ERR_XX
 */
uint16_ofp
adpt_nexthop_get_output_all_group_offset(void)
{
    return g_p_adpt_nexthop_master->group_nh.offset;
}

/**
 * Create nexthop info when port created
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_phy_port_create(uint32_ofp gport)
{
    uint16_ofp ofport;
    uint32_ofp group_nhid;
    ofp_nh_offset_t member_nh;
    ofp_nh_offset_t non_edit_nh;
    adpt_port_nexthop_info_t* p_nh_info = NULL;
    adpt_flow_action_combo_t combo_action;

    p_nh_info = malloc(sizeof(adpt_port_nexthop_info_t));
    ADPT_MEM_PTR_CHECK(p_nh_info);

    /* 1. add port to output:all group */
    memset(&member_nh, 0, sizeof(member_nh));

    memset(&combo_action, 0, sizeof(combo_action));
    combo_action.output_gport = gport;
    SET_FLAG(combo_action.flag, OFP_FLOW_ACTION_FIELD_OUTPUT);
    
    group_nhid = g_p_adpt_nexthop_master->group_nh.nhid;
    ADPT_ERROR_RETURN(adpt_nexthop_alloc_flex_nh(&combo_action, &non_edit_nh));

    /* Discard the packet if it's destination port is same with ingress port */    
    ADPT_ERROR_RETURN(hal_nexthop_add_mcast_member(group_nhid, non_edit_nh.nhid, TRUE));
    
    /* 3. save to db */
    memcpy(&p_nh_info->non_edit_nh, &non_edit_nh, sizeof(non_edit_nh));
    ADPT_ERROR_RETURN(adpt_port_get_ofport_by_gport(gport, &ofport));
    ADPT_ERROR_RETURN(adpt_port_set_port_data(ofport, 
        ADPT_PORT_DATA_TYPE_NH_INFO, p_nh_info));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Destroy nexthop info when port deleted
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_phy_port_destory(uint32_ofp gport)
{
    uint16_ofp ofport;
    uint32_ofp group_nhid;
    adpt_port_nexthop_info_t* p_nh_info = NULL;

    /* 1. get nexthop info*/
    ADPT_ERROR_RETURN(adpt_port_get_ofport_by_gport(gport, &ofport));
    ADPT_ERROR_RETURN(adpt_port_get_port_data(ofport, 
        ADPT_PORT_DATA_TYPE_NH_INFO, (void**)&p_nh_info));
    ADPT_PTR_CHECK(p_nh_info);

    /* 2. remove port from output:all group */
    group_nhid = g_p_adpt_nexthop_master->group_nh.nhid;
    ADPT_ERROR_RETURN(hal_nexthop_del_mcast_member(group_nhid, p_nh_info->non_edit_nh.nhid));

    /* 3. release non edit nexthop info */
    ADPT_ERROR_RETURN(adpt_nexthop_release_nh_info(&p_nh_info->non_edit_nh));

    /* 4. reset db and free */
    ADPT_ERROR_RETURN(adpt_port_set_port_data(ofport, 
        ADPT_PORT_DATA_TYPE_NH_INFO, NULL));

    free(p_nh_info);

    return OFP_ERR_SUCCESS;
}

/**
 * Show nexthop db
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_show_db(void)
{
    ctc_cli_out_ofp(" -------------------- Nexthop DB -------------------------------------\n");
    ctc_cli_out_ofp(" all_group_nhid = %d, offset = %d\n", 
        g_p_adpt_nexthop_master->group_nh.nhid,
        g_p_adpt_nexthop_master->group_nh.offset);

    return OFP_ERR_SUCCESS;
}

/**
 * Adapter layer nexthop module init
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_nexthop_init(void)
{
    g_p_adpt_nexthop_master = malloc(sizeof(adpt_nexthop_master_t));
    ADPT_MEM_PTR_CHECK(g_p_adpt_nexthop_master);
    memset(g_p_adpt_nexthop_master, 0, sizeof(adpt_nexthop_master_t));
    
    ADPT_ERROR_RETURN(adpt_nexthop_create_output_all_nh());
    
    return OFP_ERR_SUCCESS;
}
