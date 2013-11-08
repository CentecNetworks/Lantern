/**
 @file ctc_humber_nexthop.c

 @date 2009-11-18

 @version v2.0


*/
    /****************************************************************************
     *
    * Header Files
    *
    ****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_nexthop.h"
#include "ctc_humber_nexthop.h"

#include "sys_humber_nexthop_api.h"
#include "ctc_l3if.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_chip.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define CTC_HUMBER_NEXTHOP_MAX_MARTI_SEQ_INDEX    255
#define CTC_HUMBER_CHECK_MPLS_EXP_VALUE(exp)                    \
    {                                                        \
        if((exp) > (7)) return CTC_E_INVALID_EXP_VALUE;         \
    }

#define CTC_HUMBER_CHECK_MPLS_LABVEL_VALUE(label)               \
    {                                                        \
        if((label) > (0xFFFFF))                                 \
            return CTC_E_INVALID_MPLS_LABEL_VALUE;              \
    }

#define CTC_HUMBER_CHECK_MARTI_SEQ_INDEX(label)               \
    {                                                        \
        if((label) > (CTC_HUMBER_NEXTHOP_MAX_MARTI_SEQ_INDEX))  \
            return CTC_E_INVALID_PARAM;              \
    }

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
 @brief SDK nexthop module initilize

 @param[in] nh_cfg  nexthop module global config

 @return CTC_E_XXX

*/
int32
ctc_humber_nexthop_init(ctc_nh_global_cfg_t * nh_cfg)
{
    CTC_ERROR_RETURN(sys_humber_nh_api_init(nh_cfg));

    return CTC_E_NONE;
}


static INLINE int32
_ctc_humber_mpls_map_opcode(ctc_mpls_nh_op_t ctc_op, sys_nh_mpls_op_t* p_sys_op)
{
    switch(ctc_op)
    {
        case CTC_MPLS_NH_PUSH_OP_NONE:
            *p_sys_op = SYS_NH_MPLS_PUSH_OP_NONE;
            break;
        case CTC_MPLS_NH_PUSH_OP_ROUTE:
            *p_sys_op = SYS_NH_MPLS_PUSH_OP_ROUTE;
            break;
        case CTC_MPLS_NH_PUSH_OP_L2VPN:
            *p_sys_op = SYS_NH_MPLS_PUSH_OP_L2VPN;
            break;

        case CTC_MPLS_NH_POP_OP_NONE:
            *p_sys_op = SYS_NH_MPLS_POP_OP_NONE;
            break;
        case CTC_MPLS_NH_POP_OP_ROUTE:
            *p_sys_op = SYS_NH_MPLS_POP_OP_ROUTE;
            break;
        case CTC_MPLS_NH_POP_OP_MPLS:
            *p_sys_op = SYS_NH_MPLS_POP_OP_MPLS;
            break;
        case CTC_MPLS_NH_POP_OP_L2VPN_WITH_EDIT:
            *p_sys_op = SYS_NH_MPLS_POP_OP_L2VPN_WITH_EDIT;
            break;
        case CTC_MPLS_NH_POP_OP_L2VPN_NO_EDIT:
            *p_sys_op = SYS_NH_MPLS_POP_OP_L2VPN_NO_EDIT;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}
static INLINE int32
_ctc_humber_mpls_push_nh_map_param(ctc_mpls_nexthop_push_param_t* p_nh_param_in,bool is_unrov,
                                  sys_nh_mpls_param_t* p_nh_param_out)
{
    int32 i = 0, j = 0;
    if((is_unrov == FALSE)
        && (p_nh_param_in->nh_com.opcode < CTC_MPLS_NH_PUSH_OP_NONE || p_nh_param_in->nh_com.opcode > CTC_MPLS_NH_PUSH_OP_L2VPN))
      {
        return CTC_E_INVALID_PARAM;
      }

    kal_memset(p_nh_param_out, 0, sizeof(sys_nh_mpls_param_t));
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_INNER_TPID_OW_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_INNER_TPID_OW_EN);
        p_nh_param_out->inner_dot1q_type = p_nh_param_in->nh_com.inner_dot1q_type;/*TBD*/
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_STATS_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_STATS_EN);
        p_nh_param_out->stats_ptr = p_nh_param_in->nh_com.stats_ptr;
    }

    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_L3IF_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_L3IF_EN);
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_VPLS))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_VPLS);
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_APS_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_MPLS_NH_FLAG_APS_EN);
    }

    p_nh_param_out->opcode = p_nh_param_in->nh_com.opcode;
    CTC_ERROR_RETURN(_ctc_humber_mpls_map_opcode(p_nh_param_in->nh_com.opcode,
        &(p_nh_param_out->opcode)));
    kal_memcpy(&(p_nh_param_out->oif), &(p_nh_param_in->nh_com.oif), sizeof(ctc_nh_oif_info_t));
    kal_memcpy(p_nh_param_out->mac, p_nh_param_in->nh_com.mac, sizeof(mac_addr_t));

    if(TRUE == (p_nh_param_out->martini_encap_valid = p_nh_param_in->martini_encap_valid))
    {
        CTC_HUMBER_CHECK_MARTI_SEQ_INDEX(p_nh_param_in->seq_num_index);
        p_nh_param_out->seq_num_index = p_nh_param_in->seq_num_index;
        switch(p_nh_param_in->seq_num_type)
        {
            case CTC_MPLS_NH_MARTINI_SEQ_NONE:
                p_nh_param_out->seq_num_type = SYS_NH_MPLS_MARTINI_SEQ_NONE;
                break;

            case CTC_MPLS_NH_MARTINI_SEQ_PER_PW:
                p_nh_param_out->seq_num_type = SYS_NH_MPLS_MARTINI_SEQ_PER_PW;
                break;

            case CTC_MPLS_NH_MARTINI_SEQ_GLB_TYPE0:
                p_nh_param_out->seq_num_type = SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE0;
                break;

            case CTC_MPLS_NH_MARTINI_SEQ_GLB_TYPE1:
                p_nh_param_out->seq_num_type = SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE1;
                break;

            default:
                return CTC_E_INVALID_PARAM;
        }
        j++;
    }
    if(TRUE == (p_nh_param_out->vpls_port_valid = p_nh_param_in->vpls_port_valid))
    {
        p_nh_param_out->vpls_port = p_nh_param_in->vpls_port;
        CTC_VPLS_PORT_RANGE_CHECK(p_nh_param_out->vpls_port);
        j++;
    }
    for(i = 0; i < CTC_MPLS_NH_MAX_PUSH_LABEL_NUM; i++)
    {
        if (CTC_FLAG_ISSET(p_nh_param_in->push_label[i].lable_flag,
                           CTC_MPLS_NH_LABEL_IS_VALID))
        {
            CTC_SET_FLAG(p_nh_param_out->push_label[i].lable_flag,
                         SYS_NH_MPLS_LABEL_IS_VALID);
            p_nh_param_out->push_label[i].label = p_nh_param_in->push_label[i].label;
            CTC_HUMBER_CHECK_MPLS_LABVEL_VALUE(p_nh_param_in->push_label[i].label);

            if(CTC_FLAG_ISSET(p_nh_param_in->push_label[i].lable_flag,
                           CTC_MPLS_NH_LABEL_IS_MCAST))
            {
                CTC_SET_FLAG(p_nh_param_out->push_label[i].lable_flag,
                             SYS_NH_MPLS_LABEL_IS_MCAST);
            }
            if(CTC_FLAG_ISSET(p_nh_param_in->push_label[i].lable_flag,
                           CTC_MPLS_NH_LABEL_MAP_TTL))
            {
                CTC_SET_FLAG(p_nh_param_out->push_label[i].lable_flag,
                             SYS_NH_MPLS_LABEL_MAP_TTL);
            }
            CTC_HUMBER_CHECK_MPLS_EXP_VALUE(p_nh_param_in->push_label[i].exp);
            p_nh_param_out->push_label[i].exp = p_nh_param_in->push_label[i].exp;
            switch(p_nh_param_in->push_label[i].exp_type)
            {
                case CTC_MPLS_NH_EXP_USE_LABEL_EXP:
                    p_nh_param_out->push_label[i].exp_type = SYS_NH_MPLS_EXP_USE_LABEL_EXP;
                    break;

                case CTC_MPLS_NH_EXP_DERIVE_FROM_SRC_EXP:
                    p_nh_param_out->push_label[i].exp_type = SYS_NH_MPLS_EXP_DERIVE_FROM_SRC_EXP;
                    break;

                case CTC_MPLS_NH_EXP_DERIVE_FROM_PKT_PRIORITY:
                    p_nh_param_out->push_label[i].exp_type = SYS_NH_MPLS_EXP_DERIVE_FROM_PKT_PRIORITY;
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
            p_nh_param_out->push_label[i].ttl = p_nh_param_in->push_label[i].ttl;
            j++;
        }
        else
        {
            break;
        }
    }

    p_nh_param_out->total_label_num = j;
    kal_memcpy(&(p_nh_param_out->vlan_info), &(p_nh_param_in->nh_com.vlan_info),
               sizeof(ctc_vlan_egress_edit_info_t));

    return CTC_E_NONE;
}

static INLINE int32
_ctc_humber_mpls_pop_nh_map_param(ctc_mpls_nexthop_pop_param_t *p_nh_param_in,bool is_unrov,
                             sys_nh_mpls_param_t *p_nh_param_out)
{
    if((is_unrov == FALSE)
        && ( p_nh_param_in->nh_com.opcode < CTC_MPLS_NH_POP_OP_NONE ||  p_nh_param_in->nh_com.opcode > CTC_MPLS_NH_POP_OP_L2VPN_NO_EDIT))
     {
        return CTC_E_INVALID_PARAM;
     }

    kal_memset(p_nh_param_out, 0, sizeof(sys_nh_mpls_param_t));

    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_INNER_TPID_OW_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_INNER_TPID_OW_EN);
        p_nh_param_out->inner_dot1q_type = p_nh_param_in->nh_com.inner_dot1q_type;/*TBD*/
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_STATS_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_STATS_EN);
        p_nh_param_out->stats_ptr = p_nh_param_in->nh_com.stats_ptr;
    }

    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_L3IF_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_L3IF_EN);
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_SQN_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_MPLS_NH_FLAG_SQN_EN);
        p_nh_param_out->stats_ptr = p_nh_param_in->nh_com.stats_ptr;
    }
    if(CTC_FLAG_ISSET(p_nh_param_in->nh_com.mpls_nh_flag,
        CTC_MPLS_NH_FLAG_SERVICE_QUEUE_EN))
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_MPLS_NH_FLAG_SERVICE_QUEUE_EN);
    }

    CTC_ERROR_RETURN(_ctc_humber_mpls_map_opcode(p_nh_param_in->nh_com.opcode,
        &(p_nh_param_out->opcode)));
    kal_memcpy(&(p_nh_param_out->oif), &(p_nh_param_in->nh_com.oif), sizeof(ctc_nh_oif_info_t));
    kal_memcpy(p_nh_param_out->mac, p_nh_param_in->nh_com.mac, sizeof(mac_addr_t));
    kal_memcpy(&(p_nh_param_out->vlan_info), &(p_nh_param_in->nh_com.vlan_info),  sizeof(ctc_vlan_egress_edit_info_t));

    if(p_nh_param_in->use_ttl_from_packet)
    {
        CTC_SET_FLAG(p_nh_param_out->nh_mpls_flag, SYS_NH_MPLS_FLAG_USE_PKT_TTL);
    }

    if(TRUE == (p_nh_param_out->vpls_port_valid = p_nh_param_in->vpls_port_valid))
    {
        p_nh_param_out->vpls_port = p_nh_param_in->vpls_port;
        CTC_VPLS_PORT_RANGE_CHECK(p_nh_param_out->vpls_port);
    }
    return CTC_E_NONE;
}


/**
 @brief  Create normal ucast bridge nexthop entry

 @param[in] gport   Global port id

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_create_ucast_nh(uint16 gport ,  ctc_nh_param_brguc_sub_type_t nh_type)
{
    CTC_ERROR_RETURN(sys_humber_brguc_nh_create(gport,nh_type));
    return CTC_E_NONE;
}

/**
 @brief Delete normal ucast bridge nexthop entry

 @param[in] gport   Global port id

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_delete_ucast_nh(uint16 gport)
{
    CTC_ERROR_RETURN(sys_humber_brguc_nh_delete(gport));
    return CTC_E_NONE;
}

/**
 @brief Create IPUC nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] nexthop parameter used to create this ipuc nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_ipuc_nh_create(uint32 nhid, ctc_ip_nh_param_t* p_nh_param)
{
    CTC_PTR_VALID_CHECK(p_nh_param);

    if (CTC_IP_NH_OP_IP_ROUTE == p_nh_param->opcode)
    {
        CTC_ERROR_RETURN(sys_humber_ipuc_nh_create(nhid, p_nh_param->dsnh_offset, &(p_nh_param->oif), p_nh_param->mac,  CTC_FLAG_ISSET(p_nh_param->flag, CTC_IP_NH_FLAG_UNROV)));
    }
    else
    {
        CTC_ERROR_RETURN(sys_humber_ip_tunnel_nh_create( nhid, p_nh_param->dsnh_offset, p_nh_param));
    }
    return CTC_E_NONE;
}

/**
 @brief Remove IPUC nexthop

 @param[in] nhid, nexthop ID to be created

 @return CTC_E_XXX

*/
int32
ctc_humber_ipuc_nh_remove(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_ipuc_nh_delete(nhid));
    return CTC_E_NONE;
}


/**
 @brief Update IPUC nexthop

 @param[in] nhid nexthop ID to be updated

 @param[in] nexthop parameter used to update this ipuc nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_ipuc_nh_update(uint32 nhid, ctc_ip_nh_param_t* p_nh_param)
{

    mac_addr_t mac = {0};
    ctc_nh_oif_info_t oif;
    kal_memset(&oif, 0, sizeof(ctc_nh_oif_info_t));


    switch (p_nh_param->upd_type)
    {
        case CTC_NH_UPD_UNRSV_TO_FWD :
            if (CTC_IP_NH_OP_IP_ROUTE == p_nh_param->opcode)
            {
                CTC_ERROR_RETURN(sys_humber_ipuc_nh_update(nhid, &(p_nh_param->oif), p_nh_param->mac, SYS_NH_CHANGE_TYPE_UNROV_TO_FWD));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_ip_tunnel_nh_update( nhid, p_nh_param, SYS_NH_CHANGE_TYPE_UNROV_TO_FWD));
            }
            break;
        case CTC_NH_UPD_FWD_TO_UNRSV :
            if (CTC_IP_NH_OP_IP_ROUTE == p_nh_param->opcode)
            {
                CTC_ERROR_RETURN(sys_humber_ipuc_nh_update(nhid, &oif, mac, SYS_NH_CHANGE_TYPE_FWD_TO_UNROV));
            }
            else
            {
               CTC_ERROR_RETURN(sys_humber_ip_tunnel_nh_update(nhid, p_nh_param, SYS_NH_CHANGE_TYPE_FWD_TO_UNROV));
            }
            break;
        case CTC_NH_UPD_FWD_ATTR :
            if (CTC_IP_NH_OP_IP_ROUTE == p_nh_param->opcode)
            {
                CTC_ERROR_RETURN(sys_humber_ipuc_nh_update(nhid, &p_nh_param->oif, p_nh_param->mac, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));
            }
            else
            {
               CTC_ERROR_RETURN(sys_humber_ip_tunnel_nh_update(nhid, p_nh_param, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));
            }
            break;
        default :
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief Create a mpls nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param   nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_nh_create(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param)
{
    uint32 dsnh_offset = 0;
    bool use_dsnh_ex = FALSE;
    bool is_unrov = FALSE;

    ctc_mpls_nexthop_push_param_t* p_nh_push_param = NULL;
    ctc_mpls_nexthop_push_param_t* p_nh_push_p_param = NULL;
    ctc_mpls_nexthop_switch_param_s* p_nh_switch_param = NULL;
    ctc_mpls_nexthop_pop_param_t* p_nh_pop_param = NULL;
    ctc_mpls_nexthop_pop_param_t* p_nh_pop_p_param = NULL;

    sys_nh_mpls_param_t sys_nh_param;
    sys_nh_mpls_param_t nh_param_work;
    sys_nh_mpls_param_t nh_param_protect;

    CTC_PTR_VALID_CHECK(p_nh_param);

    kal_memset(&sys_nh_param, 0, sizeof(sys_nh_mpls_param_t));
    kal_memset(&nh_param_work, 0, sizeof(sys_nh_mpls_param_t));
    kal_memset(&nh_param_protect, 0, sizeof(sys_nh_mpls_param_t));


    switch (p_nh_param->nh_prop)
    {
     case CTC_MPLS_NH_PUSH_TYPE :
             p_nh_push_param = &(p_nh_param->nh_para.nh_param_push);
             p_nh_push_p_param = p_nh_param->aps_en? &(p_nh_param->nh_p_para.nh_p_param_push):NULL;
             is_unrov = CTC_FLAG_ISSET(p_nh_param->flag, CTC_MPLS_NH_IS_UNRSV);
             use_dsnh_ex = CTC_FLAG_ISSET(p_nh_param->flag, CTC_MPLS_NH_IS_NH_8W);
             dsnh_offset = p_nh_param->dsnh_offset;
             CTC_ERROR_RETURN(_ctc_humber_mpls_push_nh_map_param(p_nh_push_param, is_unrov, &nh_param_work));
             if (p_nh_param->aps_en)
             {
                 CTC_ERROR_RETURN(_ctc_humber_mpls_push_nh_map_param(p_nh_push_p_param, is_unrov, &nh_param_protect));
                 nh_param_work.oif.gport = p_nh_param->aps_bridge_group_id;
                 nh_param_protect.oif.gport = p_nh_param->aps_bridge_group_id;
                 CTC_ERROR_RETURN(sys_humber_mpls_nh_create(nhid, dsnh_offset, use_dsnh_ex,
                                                            is_unrov, &nh_param_work, &nh_param_protect));
             }
             else
             {
                 CTC_ERROR_RETURN(sys_humber_mpls_nh_create(nhid, dsnh_offset, use_dsnh_ex,
                                                            is_unrov, &nh_param_work, NULL));
             }
             break;
        case CTC_MPLS_NH_POP_TYPE :
            p_nh_pop_param = &(p_nh_param->nh_para.nh_param_pop);
            p_nh_pop_p_param =p_nh_param->aps_en? &(p_nh_param->nh_p_para.nh_p_param_pop):NULL;
            is_unrov = CTC_FLAG_ISSET(p_nh_param->flag,CTC_MPLS_NH_IS_UNRSV);
            use_dsnh_ex = CTC_FLAG_ISSET(p_nh_param->flag,CTC_MPLS_NH_IS_NH_8W);
            dsnh_offset = p_nh_param->dsnh_offset;

            CTC_ERROR_RETURN(_ctc_humber_mpls_pop_nh_map_param(p_nh_pop_param, is_unrov,&nh_param_work));
            if (p_nh_param->aps_en)
            {
                CTC_ERROR_RETURN(_ctc_humber_mpls_pop_nh_map_param(p_nh_pop_p_param, is_unrov, &nh_param_protect));
                nh_param_work.oif.gport = p_nh_param->aps_bridge_group_id;
                nh_param_protect.oif.gport = p_nh_param->aps_bridge_group_id;
                CTC_ERROR_RETURN(sys_humber_mpls_nh_create(nhid, dsnh_offset, use_dsnh_ex,
                                                           is_unrov, &nh_param_work, &nh_param_protect));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_mpls_nh_create(nhid, dsnh_offset, use_dsnh_ex,
                                                           is_unrov, &nh_param_work, NULL));
            }
             break;

        case CTC_MPLS_NH_SWITCH_TYPE :

            p_nh_switch_param = &(p_nh_param->nh_para.nh_param_switch);
            CTC_VPLS_PORT_RANGE_CHECK(p_nh_switch_param->vpls_port);

            sys_nh_param.opcode = SYS_NH_MPLS_OP_SWITCH;
            sys_nh_param.vpls_port_valid = p_nh_switch_param->vpls_port_valid;
            sys_nh_param.vpls_port = p_nh_switch_param->vpls_port;
            sys_nh_param.oif.oif_type = CTC_NH_OIF_TYPE_VLAN_PORT;
            sys_nh_param.oif.gport = p_nh_switch_param->gport;
            sys_nh_param.oif.vid =  CTC_MAX_VLAN_ID;
            sys_nh_param.egs_vlan_valid = p_nh_switch_param->egs_vlan_valid;
            kal_memcpy(&sys_nh_param.vlan_info,&p_nh_switch_param->vlan_info, sizeof(ctc_vlan_egress_edit_info_t));

            CTC_ERROR_RETURN(sys_humber_mpls_nh_create(nhid, dsnh_offset, TRUE,
                            FALSE, &sys_nh_param, NULL));

            break;

        default :
            return CTC_E_INVALID_PARAM;

    }
    return CTC_E_NONE;
}

/**
 @brief Remove mpls nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_nh_remove(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_mpls_nh_delete(nhid));
    return CTC_E_NONE;
}


/**
 @brief Update a mpls unresolved nexthop to forwarded mpls push nexthop

 @param[in] nhid nexthop ID to be updated

 @param[in] p_nh_param nexthop parameter used to update this nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_mpls_nh_update(uint32 nhid, ctc_mpls_nexthop_param_t* p_nh_param)
{
    ctc_mpls_nexthop_push_param_t* p_nh_push_param = NULL;
    ctc_mpls_nexthop_pop_param_t* p_nh_pop_param = NULL;
    sys_nh_mpls_param_t sys_nh_param;
    sys_nh_mpls_param_t sys_nh_p_param;
    kal_memset(&sys_nh_param, 0, sizeof(sys_nh_mpls_param_t));
    kal_memset(&sys_nh_p_param, 0, sizeof(sys_nh_mpls_param_t));

    CTC_PTR_VALID_CHECK(p_nh_param);

    switch (p_nh_param->nh_prop)
    {
        case CTC_MPLS_NH_PUSH_TYPE :

           p_nh_push_param = &(p_nh_param->nh_para.nh_param_push);


            CTC_ERROR_RETURN(_ctc_humber_mpls_push_nh_map_param(p_nh_push_param, FALSE,&sys_nh_param));
            if (p_nh_param->aps_en)
            {
                p_nh_push_param = &(p_nh_param->nh_p_para.nh_p_param_push);
                CTC_ERROR_RETURN(_ctc_humber_mpls_push_nh_map_param(p_nh_push_param, FALSE, &sys_nh_p_param));
            }
            if (p_nh_param->upd_type == CTC_NH_UPD_FWD_ATTR)
            {
                if (p_nh_param->aps_en)
                {
                    CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, &sys_nh_p_param, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));
                }
                else
                {
                    CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));

                }
            }
            else  if(p_nh_param->upd_type == CTC_NH_UPD_UNRSV_TO_FWD)
            {

                  if (p_nh_param->aps_en)
                {
                    CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, &sys_nh_p_param, SYS_NH_CHANGE_TYPE_UNROV_TO_FWD));
                }
                else
                {
                 CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_UNROV_TO_FWD));

                }
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_FWD_TO_UNROV));
            }
            break;


        case CTC_MPLS_NH_POP_TYPE :

            p_nh_pop_param = &(p_nh_param->nh_para.nh_param_pop);

            CTC_ERROR_RETURN(_ctc_humber_mpls_pop_nh_map_param(p_nh_pop_param, FALSE,&sys_nh_param));

            if (p_nh_param->aps_en)
            {
                p_nh_pop_param = &(p_nh_param->nh_p_para.nh_p_param_pop);
                CTC_ERROR_RETURN(_ctc_humber_mpls_pop_nh_map_param(p_nh_pop_param, FALSE, &sys_nh_p_param));
            }
            if (p_nh_param->upd_type == CTC_NH_UPD_FWD_ATTR)
            {

                if (p_nh_param->aps_en)
                {
                       CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, &sys_nh_p_param, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));
                }
                else
                {
                 CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR));

                }
            }
            else  if(p_nh_param->upd_type == CTC_NH_UPD_UNRSV_TO_FWD)
            {
                CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_UNROV_TO_FWD));

            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_FWD_TO_UNROV));
            }

            break;

        case  CTC_MPLS_NH_NONE:
           if (p_nh_param->upd_type == CTC_NH_UPD_FWD_TO_UNRSV)
           {
               CTC_ERROR_RETURN(sys_humber_mpls_nh_update(nhid, &sys_nh_param, NULL, SYS_NH_CHANGE_TYPE_FWD_TO_UNROV));
           }
            break;
        default :
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

/**
 @brief Create a ipe loopback nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param loopback nexthop parameters

 @return CTC_E_XXX

*/
int32
ctc_humber_iloop_nh_create(uint32 nhid, ctc_loopback_nexthop_param_t* p_nh_param)
{
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_ERROR_RETURN(sys_humber_iloop_nh_create(nhid, p_nh_param));

    return CTC_E_NONE;
}

/**
 @brief Remove ipe loopback nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
int32
ctc_humber_iloop_nh_remove(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_iloop_nh_delete(nhid));
    return CTC_E_NONE;
}


/**
 @brief Create a rspan(remote mirror) nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param remote mirror nexthop parameters

 @return CTC_E_XXX

*/
int32
ctc_humber_rspan_nh_create(uint32 nhid, uint32 dsnh_offset,ctc_rspan_nexthop_param_t* p_nh_param)
{

    CTC_ERROR_RETURN(sys_humber_rspan_nh_create(nhid, dsnh_offset,p_nh_param));
    return CTC_E_NONE;
}

/**
 @brief Remove rspan(remote mirror) nexthop

 @param[in] nhid nexthop ID to be removed

 @return CTC_E_XXX

*/
int32
ctc_humber_rspan_nh_remove(uint32 nhid)
{

    CTC_ERROR_RETURN(sys_humber_rspan_nh_delete(nhid));
    return CTC_E_NONE;
}

/**
 @brief Create a ECMP nexthop

 @param[in] pdata Create data

 @return CTC_E_XXX

*/
int32
ctc_humber_ecmp_nh_create(ctc_nh_ecmp_creat_data_t* pdata)
{
    CTC_ERROR_RETURN(sys_humber_ecmp_group_create(pdata));
    return CTC_E_NONE;
}

/**
 @brief Delete a ECMP nexthop

 @param[in] nhid nexthop ID of ECMP to be removed

 @return CTC_E_XXX

*/
int32
ctc_humber_ecmp_nh_delete(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_ecmp_group_delete(nhid));
    return CTC_E_NONE;
}

/**
 @brief Update a ECMP nexthop

 @param[in] nhid nexthop ID of ECMP to be updated

 @param[in] subnhid member nexthop ID of ECMP

 @param[in] op_type Add or Remove nexthop member nexthop into/from ECMP group

 @return CTC_E_XXX

*/
int32
ctc_humber_ecmp_nh_update(ctc_nh_ecmp_update_data_t* pdata)
{
    CTC_ERROR_RETURN(sys_humber_ecmp_group_update(pdata));
    return CTC_E_NONE;
}


/**
 @brief Get ucast nhid by type

 @param[in] gport global port of the system

 @param[in] nh_type the type of ucast nexthop

 @param[in] nhid nexthop ID to get

 @return CTC_E_XXX

*/
int32
ctc_humber_l2_get_ucast_nh(uint16 gport, ctc_nh_param_brguc_sub_type_t nh_type, uint32* nhid)
{
    CTC_ERROR_RETURN(sys_humber_l2_get_ucast_nh(gport, nh_type, nhid));
    return CTC_E_NONE;
}


/**
 @brief The function is to create Egress Vlan Editing nexthop or APS Egress Vlan Editing nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] nh_param        nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_egress_vlan_edit_create(uint32 nhid, ctc_vlan_edit_nh_param_t* p_nh_param)
{
    uint32 dsnh_offset = 0;
    uint16 gport_or_aps_bridge_id = 0;
    ctc_vlan_egress_edit_info_t* p_vlan_edit_info = NULL;
    ctc_vlan_egress_edit_info_t* p_vlan_edit_info_p = NULL;

    if (!p_nh_param->aps_en)
    {
        dsnh_offset = p_nh_param->dsnh_offset;
        gport_or_aps_bridge_id = p_nh_param->gport_or_aps_bridge_id;
        p_vlan_edit_info = &(p_nh_param->vlan_edit_info);
        CTC_ERROR_RETURN(sys_humber_egress_vlan_edit_nh_create(nhid, gport_or_aps_bridge_id,
                                                               p_vlan_edit_info, dsnh_offset));
    }
    else
    {
        dsnh_offset = p_nh_param->dsnh_offset;
        gport_or_aps_bridge_id = p_nh_param->gport_or_aps_bridge_id;
        p_vlan_edit_info = &(p_nh_param->vlan_edit_info);
        p_vlan_edit_info_p = &(p_nh_param->vlan_edit_info_p);
        CTC_ERROR_RETURN(sys_humber_aps_egress_vlan_edit_nh_create(nhid, dsnh_offset, gport_or_aps_bridge_id,
                                                                   p_vlan_edit_info, p_vlan_edit_info_p));
    }
    return CTC_E_NONE;
}

/**
 @brief The function is to remove Egress Vlan Editing nexthop or APS Egress Vlan Editing nexthop

 @param[in] nhid            Egress vlan Editing nexthop id or APS Egress vlan Editing nexthop id

 @return CTC_E_XXX

*/
int32
ctc_humber_egress_vlan_edit_remove(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_egress_vlan_edit_nh_delete(nhid));
    return CTC_E_NONE;
}

/**
 @brief The function is to create flexible nexthop

 @param[in] nhid nexthop ID to be created

 @param[in] p_nh_param  nexthop parameter used to create this nexthop

 @return CTC_E_XXX

*/
int32
ctc_humber_flex_nh_create(uint32 nhid, ctc_flex_nh_param_t* p_nh_param)
{
  CTC_ERROR_RETURN(sys_humber_flex_nh_create(nhid,p_nh_param));
    return CTC_E_NONE;
}

/**
 @brief The function is to remove Eflexible nexthop

 @param[in] nhid     nexthop ID to be created

 @return CTC_E_XXX

*/
int32
ctc_humber_flex_nh_remove(uint32 nhid)
{
     CTC_ERROR_RETURN(sys_humber_flex_nh_remove(nhid));
    return CTC_E_NONE;
}

/**
 @brief The function is to add stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
int32
ctc_humber_nh_add_stats(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_nh_add_stats(nhid));
    return CTC_E_NONE;
}

/**
 @brief The function is to delete stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
int32
ctc_humber_nh_del_stats(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_nh_del_stats(nhid));
    return CTC_E_NONE;
}

/**
 @brief The function is to get stats

 @param[in] nhid            stats nexthop id

 @param[out] p_stats        stats data to be get

 @return CTC_E_XXX

 */
int32
ctc_humber_nh_get_stats(uint32 nhid, ctc_stats_basic_t* p_stats)
{
    CTC_ERROR_RETURN(sys_humber_nh_get_stats(nhid, p_stats));
    return CTC_E_NONE;
}


/**
 @brief The function is to reset stats

 @param[in] nhid            stats nexthop id

 @return CTC_E_XXX

 */
int32
ctc_humber_nh_reset_stats(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_nh_reset_stats(nhid));
    return CTC_E_NONE;
}

/**
 @brief This function is to create mcast nexthop

 @param[in] nhid   nexthop ID to be created

 @param[in] p_nh_mcast_group   nexthop parameter used to create this mcast nexthop

 @return CTC_E_XXX
 */
int32
ctc_humber_mcast_nh_create(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group)
{
    sys_nh_param_mcast_group_t nh_mcast_group;

    kal_memset(&nh_mcast_group, 0, sizeof(nh_mcast_group));

    nh_mcast_group.nhid = nhid;
    nh_mcast_group.is_nhid_valid = 1;

    CTC_ERROR_RETURN(sys_humber_mcast_nh_create(p_nh_mcast_group->mc_grp_id, &nh_mcast_group));

    return CTC_E_NONE;
}

/**
 @brief This function is to delete mcast nexthop

 @param[in] nhid   nexthopid

 @return CTC_E_XXX
 */
int32
ctc_humber_mcast_nh_delete(uint32 nhid)
{
    CTC_ERROR_RETURN(sys_humber_mcast_nh_delete(nhid));

    return CTC_E_NONE;
}

/**
 @brief This function is to update mcast nexthop

 @param[in] nhid   nexthop ID

 @param[in] p_nh_mcast_group,  nexthop parameter used to add/remove  mcast member

 @return CTC_E_XXX
 */
int32
ctc_humber_mcast_nh_update(uint32 nhid, ctc_mcast_nh_param_group_t* p_nh_mcast_group)
{
    sys_nh_param_mcast_group_t nh_mcast_group;
    uint8 aps_brg_en = 0;
    uint16 dest_id = 0;
    uint8 lchip, lchip_num;
    uint8 lport = 0;

    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));

    nh_mcast_group.nhid = nhid;

    if (CTC_NH_PARAM_MCAST_ADD_MEMBER == p_nh_mcast_group->opcode)
    {
        nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_ADD_MEMBER;
    }
    else if (CTC_NH_PARAM_MCAST_DEL_MEMBER == p_nh_mcast_group->opcode)
    {
        nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_DEL_MEMBER;
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    lchip_num = sys_humber_get_local_chip_num();
    switch (p_nh_mcast_group->mem_info.member_type)
    {
        case CTC_NH_PARAM_MEM_BRGMC_LOCAL:
            nh_mcast_group.mem_info.member_type = SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS;
            nh_mcast_group.mem_info.destid = CTC_MAP_GPORT_TO_LPORT(p_nh_mcast_group->mem_info.destid);
            nh_mcast_group.mem_info.is_linkagg = CTC_IS_LINKAGG_PORT(p_nh_mcast_group->mem_info.destid);
            if (nh_mcast_group.mem_info.is_linkagg)
            {
                for (lchip = 0; lchip < lchip_num; lchip++)
                {
                    nh_mcast_group.mem_info.lchip = lchip;
                    CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
                }
            }
            else
            {
                SYS_MAP_GPORT_TO_LPORT(p_nh_mcast_group->mem_info.destid, lchip, lport);
                nh_mcast_group.mem_info.lchip = lchip;
                CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
            }
            break;

        case CTC_NH_PARAM_MEM_IPMC_LOCAL:
            nh_mcast_group.mem_info.member_type = SYS_NH_PARAM_IPMC_MEM_LOCAL;
            CTC_GLOBAL_PORT_CHECK(p_nh_mcast_group->mem_info.destid);
            nh_mcast_group.mem_info.destid = CTC_MAP_GPORT_TO_LPORT(p_nh_mcast_group->mem_info.destid);
            nh_mcast_group.mem_info.l3if_type = p_nh_mcast_group->mem_info.l3if_type;
            nh_mcast_group.mem_info.l3if_vlan_port =  \
                (p_nh_mcast_group->mem_info.l3if_type == CTC_L3IF_TYPE_VLAN_IF) && p_nh_mcast_group->mem_info.is_vlan_port;
            nh_mcast_group.mem_info.vid = p_nh_mcast_group->mem_info.vid;
            nh_mcast_group.mem_info.is_linkagg = CTC_IS_LINKAGG_PORT(p_nh_mcast_group->mem_info.destid);
            if (nh_mcast_group.mem_info.is_linkagg)
            {
                for (lchip = 0; lchip < lchip_num; lchip++)
                {
                    nh_mcast_group.mem_info.lchip = lchip;
                    CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
                }
            }
            else
            {
                SYS_MAP_GPORT_TO_LPORT(p_nh_mcast_group->mem_info.destid, lchip, lport);
                nh_mcast_group.mem_info.lchip = lchip;
                CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
            }
            break;

        case CTC_NH_PARAM_MEM_LOCAL_WITH_NH:
            nh_mcast_group.mem_info.ref_nhid = p_nh_mcast_group->mem_info.ref_nhid;
            CTC_ERROR_RETURN(sys_humber_nh_get_port(p_nh_mcast_group->mem_info.ref_nhid, &aps_brg_en, &dest_id));
            nh_mcast_group.mem_info.member_type = aps_brg_en ? SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE \
                                                               : SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH;
            nh_mcast_group.mem_info.port_check_discard = p_nh_mcast_group->mem_info.port_check_discard;
            
            if (aps_brg_en)
            {
                nh_mcast_group.mem_info.destid = dest_id;

            }
            else
            {
                CTC_GLOBAL_PORT_CHECK(dest_id);
                nh_mcast_group.mem_info.destid = CTC_MAP_GPORT_TO_LPORT(dest_id);
            }

            if (aps_brg_en || CTC_IS_LINKAGG_PORT(dest_id))
            {
                nh_mcast_group.mem_info.is_linkagg  = !aps_brg_en;
                for (lchip = 0; lchip < lchip_num; lchip++)
                {
                    nh_mcast_group.mem_info.lchip = lchip;
                    CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
                }
            }
            else
            {
                SYS_MAP_GPORT_TO_LPORT(dest_id, lchip, lport);
                nh_mcast_group.mem_info.lchip = lchip;
                nh_mcast_group.mem_info.is_linkagg = 0;
                CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
            }
            break;

        case CTC_NH_PARAM_MEM_REMOTE:
            nh_mcast_group.mem_info.member_type = SYS_NH_PARAM_MCAST_MEM_REMOTE;
            CTC_GLOBAL_PORT_CHECK(p_nh_mcast_group->mem_info.destid);
            nh_mcast_group.mem_info.destid = CTC_MAP_GPORT_TO_LPORT(p_nh_mcast_group->mem_info.destid);
            nh_mcast_group.mem_info.is_linkagg = 0;
            sys_humber_chip_is_local(CTC_MAP_GPORT_TO_GCHIP(p_nh_mcast_group->mem_info.destid), &lchip);
            nh_mcast_group.mem_info.lchip = lchip;
            CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

