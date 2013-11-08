/**
 @file sys_humber_nexthop_mpls.c

 @date 2010-01-10

 @version v2.0

 The file contains all nexthop mpls related callback function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_aps.h"
#include "ctc_linklist.h"
#include "sys_humber_chip.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_vlan.h"
/* XXX_OSS_TRIM: disabled. */
//#include "sys_humber_aps.h"
#include "sys_humber_usrid.h"

#include "drv_io.h"
#define SYS_HUMBER_NH_MPLS_VPLS_SRC_PORT_NUM        0xFFF
#define SYS_HUMBER_NH_DSL3EDIT_MPLS4W_LABEL_NUMBER  2
#define SYS_HUMBER_NH_DSL3EDIT_MPLS8W_LABEL_NUMBER  4
#define SYS_HUMBER_NH_MARTINI_SEQ_TYPE_DISABLE      0
#define SYS_HUMBER_NH_MARTINI_SEQ_TYPE_PER_PW       1
#define SYS_HUMBER_NH_MARTINI_SEQ_TYPE_GLB0         2
#define SYS_HUMBER_NH_MARTINI_SEQ_TYPE_GLB1         3
#define SYS_HUMBER_NH_MPLS_LABEL_LOW2BITS_MASK      0xFFFFC
#define SYS_HUMBER_NH_MARTINI_SEQ_SHIFT             4
#define SYS_HUMBER_NH_MPLS_LABEL_MASK               0xFFFFF
#define SYS_HUMBER_NH_MPLS_LABEL_EXP_FROM_PKT_PRI   1
#define SYS_HUMBER_NH_MPLS_LABEL_EXP_FROM_SRC_EXP   0
#define SYS_HUMBER_NH_MAX_EXP_VALUE                 7
#define SYS_HUMBER_NH_SET_LABEL(__label_param, __label_pos, __push_param, __param_pos)                  \
{                                                                                                    \
    __label_param->label##__label_pos = __push_param->push_label[__param_pos].label;                    \
    __label_param->mcast_label##__label_pos =                                                           \
        CTC_FLAG_ISSET(__push_param->push_label[__param_pos].lable_flag, SYS_NH_MPLS_LABEL_IS_MCAST) ?  \
                       SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE;                              \
    __label_param->map_ttl##__label_pos = CTC_FLAG_ISSET(__push_param->push_label[__param_pos].lable_flag, SYS_NH_MPLS_LABEL_MAP_TTL);\
    __label_param->ttl##__label_pos = __push_param->push_label[__param_pos].ttl;                        \
                                                                                                        \
    switch(__push_param->push_label[__param_pos].exp_type)                                              \
    {                                                                                                   \
        case SYS_NH_MPLS_EXP_USE_LABEL_EXP:                                                             \
            CTC_MAX_VALUE_CHECK(__push_param->push_label[__param_pos].exp, SYS_HUMBER_NH_MAX_EXP_VALUE);\
            __label_param->derive_exp##__label_pos = SYS_HUMBER_NH_BOOL_FALSE;                          \
            __label_param->exp##__label_pos = __push_param->push_label[__param_pos].exp;                \
            break;                                                                                      \
                                                                                                        \
        case SYS_NH_MPLS_EXP_DERIVE_FROM_SRC_EXP:                                                       \
            __label_param->derive_exp##__label_pos = SYS_HUMBER_NH_BOOL_TRUE;                           \
            __label_param->exp##__label_pos = SYS_HUMBER_NH_MPLS_LABEL_EXP_FROM_SRC_EXP;                \
            break;                                                                                      \
                                                                                                        \
        case SYS_NH_MPLS_EXP_DERIVE_FROM_PKT_PRIORITY:                                                  \
            __label_param->derive_exp##__label_pos = SYS_HUMBER_NH_BOOL_TRUE;                           \
            __label_param->exp##__label_pos = SYS_HUMBER_NH_MPLS_LABEL_EXP_FROM_PKT_PRI;                \
            break;                                                                                      \
                                                                                                        \
        default:                                                                                        \
            return CTC_E_INVALID_PARAM;                                                                 \
    }                                                                                                   \
}

#define SYS_HUMBER_NH_SET_VPLS_PORT(__label_param, __label_pos, __vpls_port)    \
{                                                                            \
    __label_param->label##__label_pos = __vpls_port & 0x1FFF;                    \
    __label_param->mcast_label##__label_pos = SYS_HUMBER_NH_BOOL_TRUE;          \
}

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
static INLINE int32
_sys_humber_nh_mpls_build_martini_label(sys_nh_mpls_martini_seq_type_t seq_num_type,
                                        uint32 per_pw_seq_index, uint32 *p_label0)
{
    *p_label0 = 0;
    SYS_NH_DBG_FUNC();

    switch(seq_num_type)
    {
        case SYS_NH_MPLS_MARTINI_SEQ_NONE:
            SYS_NH_DBG_INFO("Sequence Number type NONE\n");
            *p_label0 |= SYS_HUMBER_NH_MARTINI_SEQ_TYPE_DISABLE;
            break;

        case SYS_NH_MPLS_MARTINI_SEQ_PER_PW:
            SYS_NH_DBG_INFO("Sequence Number type Per-PW, seq_index is %d\n", per_pw_seq_index);
            *p_label0 |= SYS_HUMBER_NH_MARTINI_SEQ_TYPE_PER_PW;
            *p_label0 |= (per_pw_seq_index << SYS_HUMBER_NH_MARTINI_SEQ_SHIFT);
            break;

        case SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE0:
            SYS_NH_DBG_INFO("Sequence Number type Global-Type0\n");
            *p_label0 |= SYS_HUMBER_NH_MARTINI_SEQ_TYPE_GLB0;
            break;

        case SYS_NH_MPLS_MARTINI_SEQ_GLB_TYPE1:
            SYS_NH_DBG_INFO("Sequence Number type Global-Type1\n");
            *p_label0 |= SYS_HUMBER_NH_MARTINI_SEQ_TYPE_GLB1;
            break;

        default:
            return CTC_E_INVALID_MARTINI_SEQ_TYPE;
    }

    SYS_NH_DBG_INFO("Martini Label is 0x%x\n", *p_label0);

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mpls_build_dsl3edit(sys_nh_mpls_param_t *p_nh_param,
                                 ds_l3edit_mpls4w_t *p_dsl3edit4w,
                                 ds_l3edit_mpls8w_t *p_dsl3edit8w,
                                 sys_nh_info_mpls_edit_info_t* p_nhinfo,
                                 bool use_dsnh8w)
{
    uint32 tmp_label;
    int32 i, label_num = 0;

    SYS_NH_DBG_FUNC();
    /*Set default valule*/
    p_nhinfo->dsl3edit_type = SYS_NH_L3EDIT_TYPE_NONE;

    switch(p_nh_param->opcode)
    {
        case SYS_NH_MPLS_PUSH_OP_NONE:
        case SYS_NH_MPLS_PUSH_OP_ROUTE:
        case SYS_NH_MPLS_PUSH_OP_L2VPN:
            SYS_NH_DBG_INFO("Push Mpls Nexthop\n");
            break;

        case SYS_NH_MPLS_POP_OP_NONE:
        case SYS_NH_MPLS_POP_OP_ROUTE:
        case SYS_NH_MPLS_POP_OP_MPLS:
        case SYS_NH_MPLS_POP_OP_L2VPN_NO_EDIT:
        case SYS_NH_MPLS_POP_OP_L2VPN_WITH_EDIT:
            SYS_NH_DBG_INFO("Pop Mpls Nexthop\n");
            return CTC_E_NONE;
        case SYS_NH_MPLS_OP_SWITCH:
              SYS_NH_DBG_INFO("No Build MPLS NH DsL3Edit\n");
            return CTC_E_NONE;
        default:
            SYS_NH_DBG_INFO("Invalid Mpls Nexthop\n");
            return CTC_E_INVALID_PARAM;
    }

    if(p_nh_param->vpls_port_valid && p_nh_param->vpls_port > SYS_HUMBER_NH_MPLS_VPLS_SRC_PORT_NUM)
    {
        return CTC_E_INVALID_PARAM;
    }

    for(i = 0; i < SYS_HUMBER_NH_DSL3EDIT_MPLS8W_LABEL_NUMBER; i++)
    {
        if(!CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
        {
            break;
        }
        label_num++;
    }
    if(p_nh_param->martini_encap_valid)
    {
        label_num++;
    }

    /*Build mpls label*/
    i = 0;
    SYS_NH_DBG_INFO("Push label total number is %d, Have martini labe = %d, VPLS port enable = %d\n",
            p_nh_param->total_label_num,
            p_nh_param->martini_encap_valid,
            p_nh_param->vpls_port_valid);
    if(label_num > SYS_HUMBER_NH_DSL3EDIT_MPLS4W_LABEL_NUMBER ||
       (p_nh_param->total_label_num > SYS_HUMBER_NH_DSL3EDIT_MPLS4W_LABEL_NUMBER && !use_dsnh8w))
    {
        p_nhinfo->dsl3edit_type = SYS_NH_L3EDIT_TYPE_MPLS_8W;
        kal_memset(p_dsl3edit8w, 0, sizeof(ds_l3edit_mpls8w_t));

        /*1. Build label 0*/
        if(p_nh_param->martini_encap_valid)
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_build_martini_label(p_nh_param->seq_num_type,
                                                                     p_nh_param->seq_num_index,
                                                                     &tmp_label));
            p_dsl3edit8w->label0 = tmp_label & SYS_HUMBER_NH_MPLS_LABEL_MASK;
            p_dsl3edit8w->martini_encap_valid = SYS_HUMBER_NH_BOOL_TRUE;
        }
        else
        {
            if(!CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
                CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
            SYS_HUMBER_NH_SET_LABEL(p_dsl3edit8w, 0, p_nh_param, i);
            i++;
        }

        /*2. Build label 1*/
        if(!CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
            CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
        SYS_HUMBER_NH_SET_LABEL(p_dsl3edit8w, 1, p_nh_param, i);
        p_dsl3edit8w->label_valid1 = SYS_HUMBER_NH_BOOL_TRUE;
        i++;

        /*3. Build label 2*/
        if(CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
        {
            SYS_HUMBER_NH_SET_LABEL(p_dsl3edit8w, 2, p_nh_param, i);
            p_dsl3edit8w->label_valid2 = SYS_HUMBER_NH_BOOL_TRUE;
            i++;
        }

        /*4. Build label 3*/
        if(CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
        {
            SYS_HUMBER_NH_SET_LABEL(p_dsl3edit8w, 3, p_nh_param, i);
            p_dsl3edit8w->label_valid3 = SYS_HUMBER_NH_BOOL_TRUE;
            i++;
        }

        if(p_nh_param->vpls_port_valid && !use_dsnh8w)
        {
            switch(label_num)
            {
                case 1 :
                    SYS_HUMBER_NH_SET_VPLS_PORT(p_dsl3edit8w, 1, p_nh_param->vpls_port);
                    p_dsl3edit8w->label_valid1 = SYS_HUMBER_NH_BOOL_FALSE;
                    break;
                case 2 :
                    SYS_HUMBER_NH_SET_VPLS_PORT(p_dsl3edit8w, 2, p_nh_param->vpls_port);
                    p_dsl3edit8w->label_valid2 = SYS_HUMBER_NH_BOOL_FALSE;
                    break;
                case 3 :
                    SYS_HUMBER_NH_SET_VPLS_PORT(p_dsl3edit8w, 3, p_nh_param->vpls_port);
                    p_dsl3edit8w->label_valid3 = SYS_HUMBER_NH_BOOL_FALSE;
                    break;
                default :
                    return CTC_E_UNEXPECT;
            }
        }
    }
    else
    {
        p_nhinfo->dsl3edit_type = SYS_NH_L3EDIT_TYPE_MPLS_4W;
        if(!CTC_FLAG_ISSET(p_nh_param->push_label[0].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
            CTC_ERROR_RETURN(CTC_E_NO_TUNNEL_LABEL);

        kal_memset(p_dsl3edit4w, 0, sizeof(ds_l3edit_mpls4w_t));

        /*1. Build label 0*/
        if(p_nh_param->martini_encap_valid)
        {
            CTC_ERROR_RETURN(
            _sys_humber_nh_mpls_build_martini_label(p_nh_param->seq_num_type,
                                                    p_nh_param->seq_num_index,
                                                    &tmp_label));
            p_dsl3edit4w->label0 = tmp_label & SYS_HUMBER_NH_MPLS_LABEL_MASK;
            p_dsl3edit4w->martini_encap_valid = SYS_HUMBER_NH_BOOL_TRUE;
        }
        else
        {
            if (!CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
                CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
            /*Build label 0*/
              SYS_HUMBER_NH_SET_LABEL(p_dsl3edit4w, 0, p_nh_param, i);
               i++;
        }

        /*Build label 1*/
        if(CTC_FLAG_ISSET(p_nh_param->push_label[i].lable_flag, SYS_NH_MPLS_LABEL_IS_VALID))
        {
            SYS_HUMBER_NH_SET_LABEL(p_dsl3edit4w, 1, p_nh_param, i);
            p_dsl3edit4w->label_valid1 = SYS_HUMBER_NH_BOOL_TRUE;
        }
        else if(p_nh_param->vpls_port_valid && !use_dsnh8w)
        {
            SYS_HUMBER_NH_SET_VPLS_PORT(p_dsl3edit4w, 1, p_nh_param->vpls_port);
            p_dsl3edit4w->label_valid1 = SYS_HUMBER_NH_BOOL_FALSE;
        }
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mpls_build_dsl2edit(sys_nh_mpls_param_t *p_nh_param,
                                 sys_nh_db_dsl2editeth4w_t* p_dsl2edit)
{
    kal_memset(p_dsl2edit, 0, sizeof(sys_nh_db_dsl2editeth4w_t));
    p_dsl2edit->hdr.entry_type = SYS_NH_ENTRY_TYPE_NULL;
    SYS_NH_DBG_FUNC();

    switch(p_nh_param->opcode)
    {
        case SYS_NH_MPLS_PUSH_OP_NONE:
        case SYS_NH_MPLS_PUSH_OP_ROUTE:
        case SYS_NH_MPLS_PUSH_OP_L2VPN:
        case SYS_NH_MPLS_POP_OP_NONE:
        case SYS_NH_MPLS_POP_OP_ROUTE:
        case SYS_NH_MPLS_POP_OP_MPLS:
            SYS_NH_DBG_INFO("Build MPLS NH DsL2Edit\n");
            break;
        case SYS_NH_MPLS_OP_SWITCH:
              p_dsl2edit->hdr.entry_type = SYS_NH_ENTRY_TYPE_NULL;
              SYS_NH_DBG_INFO("No Build MPLS NH DsL2Edit\n");
            return CTC_E_NONE;
        default:
            SYS_NH_DBG_INFO("No Build MPLS NH DsL2Edit\n");
            return CTC_E_NONE;
    }

    kal_memcpy(p_dsl2edit->mac_da, p_nh_param->mac, sizeof(mac_addr_t));
    p_dsl2edit->output_vid = p_nh_param->oif.vid;
    p_dsl2edit->ouput_vlan_is_svlan = p_nh_param->oif.outervlan_is_svlan;
    p_dsl2edit->hdr.entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W;

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mpls_build_dsnh(sys_nh_mpls_param_t *p_nh_mpls_param,
                               sys_nh_param_dsnh_t *p_dsnh_param,
                               sys_nh_info_mpls_edit_info_t* p_nhinfo,
                               uint32 dsnh_offset, bool use_dsnh8w)
{
    sys_vlan_info_t vlan_info;
    uint8 gchip,lchip;
    SYS_NH_DBG_FUNC();

    kal_memset(p_dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    p_dsnh_param->dsnh_offset = dsnh_offset;
    SYS_NH_DBG_INFO("DsNexthop Offset %d, useDsNexthop8W = %d, mplsOpCode = %d\n",
        dsnh_offset, use_dsnh8w, p_nh_mpls_param->opcode);

    switch(p_nh_mpls_param->opcode)
    {
        case SYS_NH_MPLS_PUSH_OP_NONE:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_NONE;
            break;
        case SYS_NH_MPLS_PUSH_OP_ROUTE:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_ROUTE;
            break;
        case SYS_NH_MPLS_PUSH_OP_L2VPN:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_L2VPN;
            break;
        case SYS_NH_MPLS_POP_OP_NONE:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_NONE;
            break;
        case SYS_NH_MPLS_POP_OP_ROUTE:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_ROUTE;
            break;
        case SYS_NH_MPLS_POP_OP_MPLS:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_ROUTE;
            break;
        case SYS_NH_MPLS_POP_OP_L2VPN_WITH_EDIT:

            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN;
            break;
        case SYS_NH_MPLS_POP_OP_L2VPN_NO_EDIT:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN_NO_OP;
            break;
         case SYS_NH_MPLS_OP_SWITCH:
            p_nhinfo->gport = p_nh_mpls_param->oif.gport;
            p_dsnh_param->dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_MPLS_OP_NONE;
            if(p_nh_mpls_param->egs_vlan_valid)
            {
              p_dsnh_param->p_vlan_info = &(p_nh_mpls_param->vlan_info);
            }
             return CTC_E_NONE;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }


    vlan_info.vid = p_nh_mpls_param->oif.vid;
    vlan_info.gport = p_nh_mpls_param->oif.gport;
    switch(p_nh_mpls_param->oif.oif_type)
    {
        case CTC_NH_OIF_TYPE_VLAN_PORT:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
            break;
        case CTC_NH_OIF_TYPE_ROUTED_PORT:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
            break;
        case CTC_NH_OIF_TYPE_SUB_IF:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
            break;
        default:
            return CTC_E_L3IF_INVALID_IF_TYPE;
    }
    p_nhinfo->gport = p_nh_mpls_param->oif.gport;

    if (p_nh_mpls_param->oif.oif_type == CTC_NH_OIF_TYPE_VLAN_PORT
        && p_nh_mpls_param->oif.vid == CTC_MAX_VLAN_ID)
    {
            p_dsnh_param->dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
    }
    else
    {
        if (CTC_IS_LINKAGG_PORT( p_nhinfo->gport ))
        {
            /*store l3ifid into ipuc nexthop db*/
            CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &p_dsnh_param->dest_vlan_ptr));
            if (CTC_FLAG_ISSET(p_nh_mpls_param->nh_mpls_flag, SYS_NH_MPLS_FLAG_L3IF_EN))
            {
                CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_nhinfo->l3ifid)));
                SYS_NH_DBG_INFO("Layer3IfId = %d\n", p_nhinfo->l3ifid);
            }
        }
        else
        {
            gchip = SYS_MAP_GPORT_TO_GCHIP( p_nhinfo->gport );
            if (sys_humber_chip_is_local(gchip, &lchip))
            {
                /*store l3ifid into ipuc nexthop db*/
                CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &p_dsnh_param->dest_vlan_ptr));
                 if (CTC_FLAG_ISSET(p_nh_mpls_param->nh_mpls_flag, SYS_NH_MPLS_FLAG_L3IF_EN))
                {
                    CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_nhinfo->l3ifid)));
                    SYS_NH_DBG_INFO("Layer3IfId = %d\n", p_nhinfo->l3ifid);
                }
            }
            else
            {
                p_dsnh_param->dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
            }
        }
    }


    SYS_NH_DBG_INFO("DestVlanPtr = %d\n", p_dsnh_param->dest_vlan_ptr);
    if(CTC_FLAG_ISSET(p_nh_mpls_param->nh_mpls_flag, SYS_NH_MPLS_FLAG_USE_PKT_TTL))
    {
        p_dsnh_param->use_ttl_from_pkt = SYS_HUMBER_NH_BOOL_TRUE;
        SYS_NH_DBG_INFO("Use TTL from packet\n");
    }
    p_dsnh_param->p_vlan_info = &(p_nh_mpls_param->vlan_info);
    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mpls_op_edit_table(sys_nh_mpls_param_t *p_mpls_nh_param,
                                  sys_nh_info_mpls_edit_info_t* p_edit_db, uint8 curr_chip_num,
                                  bool use_dsnh8w, uint32 dsnh_offset)
{
    uint8 gchip, tmp_lchip, lchip;
    ds_l3edit_mpls4w_t dsl3edit4w;
    ds_l3edit_mpls8w_t dsl3edit8w;
    sys_nh_db_dsl2editeth4w_t dsl2edit;
    sys_nh_param_dsnh_t dsnh_param;
    bool is_glb_alloc_nexthop;

    /*1. Build dsL3EditMpls*/
    CTC_ERROR_RETURN(_sys_humber_nh_mpls_build_dsl3edit(p_mpls_nh_param, &dsl3edit4w, &dsl3edit8w, p_edit_db, use_dsnh8w));

    /*2. Build dsL2EditMpls*/
    CTC_ERROR_RETURN(_sys_humber_nh_mpls_build_dsl2edit(p_mpls_nh_param, &dsl2edit));

    /*3. Build dsNexthop*/
    CTC_ERROR_RETURN(_sys_humber_nh_mpls_build_dsnh(p_mpls_nh_param, &dsnh_param,
                                                    p_edit_db, dsnh_offset, use_dsnh8w));

    gchip = SYS_MAP_GPORT_TO_GCHIP(p_mpls_nh_param->oif.gport);
    sys_humber_chip_is_local(gchip, &tmp_lchip);
     is_glb_alloc_nexthop = sys_humber_is_glb_alloc_nexthop();

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        if(CTC_IS_LINKAGG_PORT(p_mpls_nh_param->oif.gport) || (lchip == tmp_lchip)
            || is_glb_alloc_nexthop)
        {
            /*Op Write DsL3Edit to Asic*/
            switch(p_edit_db->dsl3edit_type)
            {
                case SYS_NH_L3EDIT_TYPE_MPLS_8W:
                    /*1. Allocate new dsl3edit mpls entry*/
                    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,
                                                                SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_8W, 1,
                                                                &p_edit_db->dsl3edit_offset[lchip]));
                    /*2. Write HW table*/
                    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,
                                                                    SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_8W,
                                                                    p_edit_db->dsl3edit_offset[lchip], &dsl3edit8w));
                    SYS_NH_DBG_INFO("Lchip = %d, DsL3EditMPLS8W, offset = %d\n",
                        lchip, p_edit_db->dsl3edit_offset[lchip]);
                    break;

                case SYS_NH_L3EDIT_TYPE_MPLS_4W:
                    /*1. Allocate new dsl3edit mpls entry*/
                    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,
                                                                SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_4W, 1,
                                                                &p_edit_db->dsl3edit_offset[lchip]));
                    /*2. Write HW table*/
                    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,
                                                                    SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_4W,
                                                                    p_edit_db->dsl3edit_offset[lchip], &dsl3edit4w));
                    SYS_NH_DBG_INFO("Lchip = %d, DsL3EditMPLS4W, offset = %d\n",
                        lchip, p_edit_db->dsl3edit_offset[lchip]);
                    break;

                case SYS_NH_L3EDIT_TYPE_NONE:
                    SYS_NH_DBG_INFO("Lchip = %d, No DsL3Edit Table\n", lchip);
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }

            /*Write DsL2Edit to Asic*/
            if(SYS_NH_ENTRY_TYPE_NULL != dsl2edit.hdr.entry_type)
            {
                sys_nh_db_dsl2editeth4w_t *p_dsl2edit;
                p_dsl2edit = &dsl2edit;
                p_dsl2edit->hdr.lchip = lchip;
                CTC_ERROR_RETURN(sys_humber_nh_db_set_entry((sys_nh_db_com_entry_t**)(&p_dsl2edit)));
                p_edit_db->p_dsl2edit_info[lchip] = p_dsl2edit;
                dsnh_param.l2edit_ptr = p_dsl2edit->hdr.offset;
                SYS_NH_DBG_INFO("Lchip = %d, DsL2EditEth4w, offset = %d\n", lchip, p_dsl2edit->hdr.offset);
            }
            else
            {
                dsnh_param.l2edit_ptr = SYS_HUMBER_NH_INVALID_OFFSET;
                dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_NONE;
                SYS_NH_DBG_INFO("Lchip = %d, No DsL2EditEth4W table\n", lchip);
            }

            /*Write DsNexthop to Asic*/
            dsnh_param.l3edit_ptr = p_edit_db->dsl3edit_offset[lchip];
            dsnh_param.l3dit_type= p_edit_db->dsl3edit_type;
            dsnh_param.lchip = lchip;
            dsnh_param.vpls_nexthop = CTC_FLAG_ISSET(p_mpls_nh_param->nh_mpls_flag, SYS_NH_MPLS_FLAG_VPLS);
            dsnh_param.vpls_port = p_mpls_nh_param->vpls_port;
            dsnh_param.vpls_port_check = p_mpls_nh_param->vpls_port_valid;

            if (use_dsnh8w)
            {
                SYS_NH_DBG_INFO("Lchip = %d, Write DsNexthop8w Table\n", lchip);
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh8w(&dsnh_param));
            }
            else
            {
                SYS_NH_DBG_INFO("Lchip = %d, Write DsNexthop4w Table\n", lchip);
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
            }
            CTC_SET_FLAG(p_edit_db->lchip_bmp, (1 << lchip));
        }
    }

    return CTC_E_NONE;
}

 static INLINE int32
 _sys_humber_nh_mpls_get_dsnh_entry_num(uint16 entry_flag, uint8 *p_entry_num)
{
     SYS_NH_DBG_FUNC();

    switch(entry_flag & (SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W |SYS_NH_INFO_MPLS_FLAG_APS_EN))
    {
        case SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W:
        case SYS_NH_INFO_MPLS_FLAG_APS_EN:
            *p_entry_num = 2;
            break;

        case (SYS_NH_INFO_MPLS_FLAG_APS_EN|SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W):
            *p_entry_num = 4;
            break;

        default:
            *p_entry_num = 1;
            break;
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_create_mpls_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_mpls_t* p_nh_param;
    sys_nh_info_mpls_t* p_nhdb;
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_param_dsfwd_t dsfwd_param;


    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MPLS, p_com_nh_para->hdr.nh_param_type);
    p_nh_param = (sys_nh_param_mpls_t*)(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_nh_param->p_mpls_nh_param);

    if (NULL == p_nh_param->p_mpls_nh_param && NULL == p_nh_param->p_mpls_nh_param_protection)
    {
        return CTC_E_INVALID_PARAM;
    }

    CTC_VPLS_PORT_RANGE_CHECK(p_nh_param->p_mpls_nh_param->vpls_port);

    if(!p_nh_param->use_dsnh8w && p_nh_param->p_mpls_nh_param->total_label_num > CTC_MPLS_NH_MAX_PUSH_LABEL_NUM)
    {
        return CTC_E_SHOULD_USE_DSNH8W;
    }

    p_nhdb = (sys_nh_info_mpls_t*)(p_com_db);
    curr_chip_num = sys_humber_get_local_chip_num();
    p_nhdb->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_MPLS;
    p_nhdb->dsnh_offset = p_nh_param->dsnh_offset;

    if(p_nh_param->use_dsnh8w)
    {
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W);
    }

    _sys_humber_nh_mpls_get_dsnh_entry_num(p_nhdb->hdr.nh_entry_flags, &dsnh_entry_num);
    CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nhdb->dsnh_offset,
                                                             dsnh_entry_num, TRUE));

    /*Create unresolved ipuc nh*/
    if(p_nh_param->is_unrov)
    {
        sys_nh_param_special_t nh_para_spec;
        sys_nh_info_special_t nhdb_spec;
        kal_memset(&nhdb_spec, 0, sizeof(nhdb_spec));
        kal_memset(&nh_para_spec, 0, sizeof(nh_para_spec));
        nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
        nh_para_spec.hdr.is_internal_nh = TRUE;
        SYS_NH_DBG_INFO("Create unresolved mpls nexthop\n");

        CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
            sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));
        for(lchip = 0; lchip < curr_chip_num; lchip++)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = nhdb_spec.hdr.dsfwd_info[lchip].dsfwd_offset;
            SYS_NH_DBG_INFO("Unresolved mpls nexthop's dsfwd offset is 0x%x\n",
                p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset);
        }
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV);
        CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_offset,
                                                               dsnh_entry_num, TRUE));
        return CTC_E_NONE;
    }

    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));

    CTC_ERROR_RETURN(_sys_humber_nh_mpls_op_edit_table(p_nh_param->p_mpls_nh_param,
                   &(p_nhdb->working_path), curr_chip_num,
                   p_nh_param->use_dsnh8w, p_nh_param->dsnh_offset));

    dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(p_nh_param->p_mpls_nh_param->oif.gport);
    dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(p_nh_param->p_mpls_nh_param->oif.gport);
    dsfwd_param.aps_type = CTC_APS_DISABLE0;

    if(CTC_FLAG_ISSET(p_nh_param->p_mpls_nh_param->nh_mpls_flag, SYS_MPLS_NH_FLAG_SQN_EN))
    {
        dsfwd_param.sequence_chk_en = TRUE;
        dsfwd_param.stats_ptr = p_nh_param->p_mpls_nh_param->stats_ptr & 0xff;
    }

    if(CTC_FLAG_ISSET(p_nh_param->p_mpls_nh_param->nh_mpls_flag, SYS_MPLS_NH_FLAG_SERVICE_QUEUE_EN))
    {
        dsfwd_param.service_queue_en = TRUE;
    }

    dsfwd_param.is_mcast = FALSE;
    dsfwd_param.dsnh_offset = p_nh_param->dsnh_offset;
    dsfwd_param.nexthop_ext = p_nh_param->use_dsnh8w;

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        if (p_nh_param->hdr.dsfwd_valid)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = p_nh_param->hdr.dsfwd_offset[lchip];
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                         &(p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset)));
            SYS_NH_DBG_INFO("Lchip = %d, DsFwdOffset = %d\n", lchip,
                (p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset));
            p_nh_param->hdr.dsfwd_offset[lchip] = (p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset);
        }
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.lchip = lchip;
        SYS_NH_DBG_INFO("DsFwd(Lchip : %d) :: DestChipId = %d, DestId = %d"
            "DsNexthop Offset = %d, DsNexthopExt = %d\n", lchip,
            dsfwd_param.dest_chipid, dsfwd_param.dest_id,
            dsfwd_param.dsnh_offset, dsfwd_param.nexthop_ext);
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_offset,
        dsnh_entry_num, TRUE));
    return CTC_E_NONE;
}

 static INLINE int32
 _sys_humber_nh_mpls_unset_dsedit(uint8 lchip, sys_nh_info_mpls_edit_info_t* p_editinfo)
{
     SYS_NH_DBG_FUNC();

     /*1. Free DsL3Edit offset*/
     switch(p_editinfo->dsl3edit_type)
     {
         case SYS_NH_L3EDIT_TYPE_MPLS_8W:
             CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
             SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_8W, 1, p_editinfo->dsl3edit_offset[lchip]));
             SYS_NH_DBG_INFO("Free DsL3EditMpls8W Offset = %d", p_editinfo->dsl3edit_offset[lchip]);
             break;

         case SYS_NH_L3EDIT_TYPE_MPLS_4W:
             CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
             SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_4W, 1, p_editinfo->dsl3edit_offset[lchip]));
             SYS_NH_DBG_INFO("Free DsL3EditMpls4W Offset = %d", p_editinfo->dsl3edit_offset[lchip]);
             break;

         case SYS_NH_L3EDIT_TYPE_NONE:
             break;
         default:
             return CTC_E_INVALID_PARAM;
     }

     /*2. Free DsL2Edit offset*/
     if (p_editinfo->p_dsl2edit_info[lchip])
     {
         /*1. Op dsL2Edit*/
         CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry(
         (sys_nh_db_com_entry_t*)(p_editinfo->p_dsl2edit_info[lchip])));
         p_editinfo->p_dsl2edit_info[lchip] = NULL;
     }

    return CTC_E_NONE;
}

/**
 @brief Callback function of delete unicast ip nexthop

 @param[in] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_delete_mpls_cb(sys_nh_info_com_t *p_data)
{
    sys_nh_info_mpls_t *p_nhinfo;
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_param_dsfwd_t dsfwd_param;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MPLS, p_data->hdr.nh_entry_type);
    p_nhinfo = (sys_nh_info_mpls_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();

    _sys_humber_nh_mpls_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags, &dsnh_entry_num);
    /*2. Delete this ipuc nexthop*/
     kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        dsfwd_param.p_dsfwd_info = &p_nhinfo->hdr.dsfwd_info[lchip];
        dsfwd_param.lchip = lchip;
        dsfwd_param.drop_pkt = 1;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
        /*Free DsFwd offset*/
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
            SYS_NH_ENTRY_TYPE_FWD, 1, p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset));
        if(CTC_FLAG_ISSET(p_nhinfo->working_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(p_nhinfo->working_path)));
            CTC_UNSET_FLAG(p_nhinfo->working_path.lchip_bmp, (1 << lchip));
        }
        if(CTC_FLAG_ISSET(p_nhinfo->protection_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(p_nhinfo->protection_path)));
            CTC_UNSET_FLAG(p_nhinfo->protection_path.lchip_bmp, (1 << lchip));
        }

    }

    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_offset,
                                                           dsnh_entry_num, FALSE));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_mpls_fwd_to_spec(sys_nh_param_mpls_t *p_nhpara, sys_nh_info_mpls_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_param_special_t nh_para_spec;
    sys_nh_info_special_t nhdb_spec;
    sys_nh_ref_list_node_t *p_ref_nh_list;
    SYS_NH_DBG_FUNC();

    kal_memset(&nh_para_spec, 0, sizeof(sys_nh_param_special_t));
    kal_memset(&nhdb_spec, 0, sizeof(sys_nh_info_special_t));
    curr_chip_num = sys_humber_get_local_chip_num();
    nh_para_spec.hdr.dsfwd_valid = TRUE;
    nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
    nh_para_spec.hdr.is_internal_nh = TRUE;
    _sys_humber_nh_mpls_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags, &dsnh_entry_num);

   /*1.1 update dsfwd to unrov nh's dsfwd, assign dsfwd ptr*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        nh_para_spec.hdr.dsfwd_offset[lchip] = p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    /*1.2 update dsfwd to unrov nh's dsfwd, write dsfwd*/
    CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
        sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));
    CTC_SET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_IS_UNROV);

    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item((sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo, p_nhpara->hdr.nhid));
    }

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*1.2 Remove DsL2Edit & DsNexthop*/
        if(CTC_FLAG_ISSET(p_nhinfo->working_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(p_nhinfo->working_path)));
            CTC_UNSET_FLAG(p_nhinfo->working_path.lchip_bmp, (1 << lchip));
        }
        if(CTC_FLAG_ISSET(p_nhinfo->protection_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(p_nhinfo->protection_path)));
            CTC_UNSET_FLAG(p_nhinfo->protection_path.lchip_bmp, (1 << lchip));
        }
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_mpls_fwd_attr(sys_nh_param_mpls_t *p_nhpara, sys_nh_info_mpls_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_info_mpls_t nhinfo_tmp;
    sys_nh_ref_list_node_t *p_ref_nh_list;
    SYS_NH_DBG_FUNC();

    curr_chip_num = sys_humber_get_local_chip_num();
    /*Build nhpara*/
    p_nhpara->hdr.dsfwd_valid = TRUE;
    _sys_humber_nh_mpls_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags, &dsnh_entry_num);
    memcpy(&nhinfo_tmp, p_nhinfo, sizeof(sys_nh_info_mpls_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        p_nhinfo->working_path.lchip_bmp = 0;
        p_nhinfo->protection_path.lchip_bmp = 0;
        p_nhpara->hdr.dsfwd_offset[lchip] = nhinfo_tmp.hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    p_nhpara->dsnh_offset = p_nhinfo->dsnh_offset;
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_offset, dsnh_entry_num, FALSE));
    if(CTC_FLAG_ISSET(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W))
    {
        p_nhpara->use_dsnh8w = TRUE;
    }
    CTC_ERROR_RETURN(sys_humber_nh_create_mpls_cb((
        sys_nh_param_com_t*)p_nhpara, (sys_nh_info_com_t*)p_nhinfo));
    CTC_UNSET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_IS_UNROV);

    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item((sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo, p_nhpara->hdr.nhid));
    }

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /* Remove dsl2edit, dsnh*/
        if(CTC_FLAG_ISSET(nhinfo_tmp.working_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(nhinfo_tmp.working_path)));
        }
        if(CTC_FLAG_ISSET(nhinfo_tmp.protection_path.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_mpls_unset_dsedit(lchip, &(nhinfo_tmp.protection_path)));
        }
    }

    return CTC_E_NONE;

}

/**
 @brief Callback function used to update ipuc nexthop

 @param[in] p_nh_ptr, pointer of ipuc nexthop DB

 @param[in] p_para, member information

 @return CTC_E_XXX
 */
int32
sys_humber_nh_update_mpls_cb(sys_nh_info_com_t* p_nh_db,
        sys_nh_param_com_t* p_para)
{
    sys_nh_info_mpls_t *p_nh_info;
    sys_nh_param_mpls_t* p_nh_para;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_para);
    CTC_PTR_VALID_CHECK(p_nh_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MPLS, p_para->hdr.nh_param_type);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MPLS, p_nh_db->hdr.nh_entry_type);
    p_nh_info = (sys_nh_info_mpls_t *)(p_nh_db);
    p_nh_para = (sys_nh_param_mpls_t*)(p_para);

    switch(p_nh_para->change_type)
    {
    case SYS_NH_CHANGE_TYPE_FWD_TO_UNROV:
            CTC_ERROR_RETURN(_sys_humber_nh_update_mpls_fwd_to_spec(p_nh_para, p_nh_info));
            break;

     case SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR:
            CTC_ERROR_RETURN(_sys_humber_nh_update_mpls_fwd_attr(p_nh_para, p_nh_info));
            break;
     case SYS_NH_CHANGE_TYPE_UNROV_TO_FWD:
          if(!CTC_FLAG_ISSET(p_nh_info->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
                return CTC_E_NH_ISNT_UNROV;
            CTC_ERROR_RETURN(_sys_humber_nh_update_mpls_fwd_attr(p_nh_para, p_nh_info));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}
