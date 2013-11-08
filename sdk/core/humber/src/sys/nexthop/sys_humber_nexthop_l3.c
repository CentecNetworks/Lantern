/**
 @file sys_humber_nexthop_l3.c

 @date 2009-11-23

 @version v2.0

 The file contains all nexthop layer3 related callback function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_linklist.h"
#include "sys_humber_chip.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_vlan.h"

#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/**
 @brief Callback function of create unicast ip route nexthop

 @param[in,out] p_com_nh_para, parameters used to create nexthop,
                writeback dsfwd offset array to this param

 @param[out] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
 static INLINE int32
 _sys_humber_nh_ipuc_set_dsnh_and_dsl2edit(uint8 lchip, sys_nh_param_dsnh_t* p_dsnh_param,
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_param, sys_nh_info_ipuc_t* p_nhdb)
{
    /*1. Op dsL2Edit*/
    p_dsl2edit_param->hdr.lchip = lchip;
    CTC_ERROR_RETURN(sys_humber_nh_db_set_entry((sys_nh_db_com_entry_t**)(&p_dsl2edit_param)));
    p_nhdb->p_dsl2edit_info[lchip] = p_dsl2edit_param;

    /*2. Op dsNexthop4w*/
    p_dsnh_param->lchip = lchip;
    p_dsnh_param->l2edit_ptr = p_nhdb->p_dsl2edit_info[lchip]->hdr.offset;
    CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(p_dsnh_param));
    return CTC_E_NONE;
}

 static INLINE int32
 _sys_humber_nh_ipuc_unset_dsnh_and_dsl2edit(uint8 lchip, sys_nh_info_ipuc_t* p_nhinfo)
{
     sys_nh_db_dsl2editeth4w_t* p_l2edit;

    if(NULL == (p_l2edit = p_nhinfo->p_dsl2edit_info[lchip]))
    {
        return CTC_E_NONE;
    }

     /*1. Op dsL2Edit*/
     CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry((sys_nh_db_com_entry_t*)p_l2edit));

     p_nhinfo->p_dsl2edit_info[lchip] = NULL;

    return CTC_E_NONE;
}

int32
sys_humber_nh_add_ref_item(uint32 nhid, sys_nh_info_com_t *p_ref_nhinfo, sys_info_ecmp_t *p_ecmpinfo)
{
    sys_nh_ref_list_node_t *p_ref_nh_list;

    p_ref_nh_list = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_ref_list_node_t));
    if(NULL == p_ref_nh_list)
        return CTC_E_NO_MEMORY;

    /*Insert new node at first*/
    p_ref_nh_list->p_ref_nhinfo = p_ref_nhinfo;
    p_ref_nh_list->p_next = p_ecmpinfo->p_nhinfo->hdr.p_ref_nh_list;
    p_ecmpinfo->p_nhinfo->hdr.p_ref_nh_list = p_ref_nh_list;

    return CTC_E_NONE;
}

int32
sys_humber_nh_remove_ref_item(uint32 nhid, sys_nh_info_com_t *p_ref_nhinfo, sys_info_ecmp_t *p_ecmpinfo)
{
    sys_nh_ref_list_node_t *p_curr, *p_prev = NULL;

    p_curr = p_ecmpinfo->p_nhinfo->hdr.p_ref_nh_list;

    while(p_curr)
    {
        if(p_curr->p_ref_nhinfo == p_ref_nhinfo)
        {
            if(NULL == p_prev)
            /*Remove first node*/
            {
                p_ecmpinfo->p_nhinfo->hdr.p_ref_nh_list = p_curr->p_next;
            }
            else
            {
                p_prev->p_next = p_curr->p_next;
            }
            mem_free(p_curr);
            return CTC_E_NONE;
        }
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }

    return CTC_E_ENTRY_NOT_EXIST;
}

int32
sys_humber_nh_create_ipuc_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_ipuc_t* p_nh_para;
    sys_nh_info_ipuc_t* p_nhdb;
    sys_nh_info_dsnh4w_t *p_dsnh_info;
    uint8 lchip, gchip, curr_chip_num;
    sys_nh_param_dsnh_t dsnh_param;
    sys_nh_param_dsfwd_t dsfwd_param;
    sys_nh_db_dsl2editeth4w_t dsl2edit;
    sys_vlan_info_t vlan_info;
    bool is_glb_alloc_nexthop;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IPUC, p_com_nh_para->hdr.nh_param_type);
    p_nh_para = (sys_nh_param_ipuc_t*)(p_com_nh_para);
    p_nhdb = (sys_nh_info_ipuc_t*)(p_com_db);
    curr_chip_num = sys_humber_get_local_chip_num();
    p_nhdb->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_IPUC;
    p_dsnh_info = &(p_nhdb->dsnh_info);
    p_dsnh_info->dsnh_offset = p_nh_para->dsnh_offset;
    p_nhdb->gport = p_nh_para->oif.gport;

    CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_dsnh_info->dsnh_offset, 1, TRUE));

    /*Get dest l3ifid*/
    vlan_info.gport = p_nh_para->oif.gport;
    vlan_info.vid = p_nh_para->oif.vid;
    switch(p_nh_para->oif.oif_type)
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

    is_glb_alloc_nexthop = sys_humber_is_glb_alloc_nexthop();
    if (CTC_IS_LINKAGG_PORT(p_nh_para->oif.gport) || is_glb_alloc_nexthop)
    {
        CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_nhdb->l3ifid)));
    }
    else
    {
        gchip = SYS_MAP_GPORT_TO_GCHIP(p_nh_para->oif.gport);
        if (sys_humber_chip_is_local(gchip, &lchip))
        {
            CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_nhdb->l3ifid)));
        }
    }

    /*Create unresolved ipuc nh*/
    if(p_nh_para->is_unrov_nh)
    {
        sys_nh_param_special_t nh_para_spec;
        sys_nh_info_special_t nhdb_spec;
        kal_memset(&nhdb_spec, 0, sizeof(nhdb_spec));
        kal_memset(&nh_para_spec, 0, sizeof(nh_para_spec));
        nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
        nh_para_spec.hdr.is_internal_nh = TRUE;
        CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
            sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));
        for(lchip = 0; lchip < curr_chip_num; lchip++)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = nhdb_spec.hdr.dsfwd_info[lchip].dsfwd_offset;
        }
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV);
        CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_dsnh_info->dsnh_offset, 1, TRUE));
        return CTC_E_NONE;
    }

    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    kal_memset(&dsl2edit, 0, sizeof(dsl2edit));

    /*1. Op dsL2Edit*/
    kal_memcpy(dsl2edit.mac_da, p_nh_para->mac, sizeof(mac_addr_t));
    dsl2edit.output_vid = p_nh_para->oif.vid;
    dsl2edit.hdr.entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W;
    dsl2edit.ouput_vlan_is_svlan = p_nh_para->oif.outervlan_is_svlan;
    /*2. Op dsNexthop4w*/
    dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IPUC;
    dsnh_param.dsnh_offset = p_nh_para->dsnh_offset;
    /*Get dest vlan ptr*/

    if (CTC_IS_LINKAGG_PORT(p_nh_para->oif.gport) || is_glb_alloc_nexthop)
    {
        CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &dsnh_param.dest_vlan_ptr));
        for (lchip = 0; lchip < curr_chip_num; lchip++)
        {
            CTC_ERROR_RETURN(_sys_humber_nh_ipuc_set_dsnh_and_dsl2edit(lchip,
                                                                         &dsnh_param, &dsl2edit, p_nhdb));
        }
    }
    else
    {
        gchip = SYS_MAP_GPORT_TO_GCHIP(p_nh_para->oif.gport);
        if (sys_humber_chip_is_local(gchip, &lchip) )
        {
           CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &dsnh_param.dest_vlan_ptr));
           CTC_ERROR_RETURN(_sys_humber_nh_ipuc_set_dsnh_and_dsl2edit(lchip,
                                                                         &dsnh_param, &dsl2edit, p_nhdb));
        }
    }

    /*Op dsFwd*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        if (p_nh_para->hdr.dsfwd_valid)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = p_nh_para->hdr.dsfwd_offset[lchip];
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_reverse_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                         &(p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset)));
            p_nh_para->hdr.dsfwd_offset[lchip] = (p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset);
        }

        dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(p_nh_para->oif.gport);
        dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(p_nh_para->oif.gport);
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.is_mcast = FALSE;
        dsfwd_param.lchip = lchip;
        dsfwd_param.dsnh_offset = p_dsnh_info->dsnh_offset;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_dsnh_info->dsnh_offset, 1, TRUE));
    return CTC_E_NONE;
}

/**
 @brief Callback function of delete unicast ip nexthop

 @param[in] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_delete_ipuc_cb(sys_nh_info_com_t *p_data)
{
    sys_nh_info_ipuc_t *p_nhinfo;
    sys_nh_ref_list_node_t *p_ref_node, *p_tmp_node;
    sys_nh_param_dsfwd_t dsfwd_param;
    uint8 lchip, curr_chip_num;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IPUC, p_data->hdr.nh_entry_type);
    p_nhinfo = (sys_nh_info_ipuc_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();

    /*1. Notify all reference nh, ipuc will be deleted*/
    p_ref_node = p_nhinfo->hdr.p_ref_nh_list;
    while(p_ref_node)
    {
        /*1. Notify ref nh*/
        /*TBD*/

        /*2. Remove from db*/
        p_tmp_node = p_ref_node;
        p_ref_node = p_ref_node->p_next;
        mem_free(p_tmp_node);
    }

    /*2. Delete this ipuc nexthop*/
     kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*_sys_humber_nh_ipuc_unset_dsnh_and_dsl2edit(lchip, p_nhinfo);*/

        dsfwd_param.p_dsfwd_info = &p_nhinfo->hdr.dsfwd_info[lchip];
        dsfwd_param.lchip = lchip;
        dsfwd_param.drop_pkt = 1;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));

        /*Free DsFwd offset*/
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
            SYS_NH_ENTRY_TYPE_FWD, 1, p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset));

        _sys_humber_nh_ipuc_unset_dsnh_and_dsl2edit(lchip, p_nhinfo);
    }
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_info.dsnh_offset, 1, FALSE));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_ipuc_fwd_to_spec(sys_nh_param_ipuc_t *p_nhpara, sys_nh_info_ipuc_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num;
    sys_nh_param_special_t nh_para_spec;
    sys_nh_info_special_t nhdb_spec;
    sys_nh_ref_list_node_t *p_ref_nh_list;

    kal_memset(&nh_para_spec, 0, sizeof(sys_nh_param_special_t));
    kal_memset(&nhdb_spec, 0, sizeof(sys_nh_info_special_t));
    curr_chip_num = sys_humber_get_local_chip_num();
    nh_para_spec.hdr.dsfwd_valid = TRUE;
    nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
    nh_para_spec.hdr.is_internal_nh = TRUE;

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*1.1 update dsfwd to unrov nh's dsfwd, assign dsfwd ptr*/
        nh_para_spec.hdr.dsfwd_offset[lchip] = p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    /*1.2 update dsfwd to unrov nh's dsfwd, write dsfwd*/
    CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
        sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));

    /*1.3 Set SYS_NH_INFO_FLAG_IS_UNROV, should before ecmp notify, ecmp will use this flag*/
    CTC_SET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV);

    /*1.3 Notify ECMP, should update ecmp before the operation of freeing dsl2edit*dsnexthop*/
    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item((sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo, p_nhpara->hdr.nhid));
    }

    /*2 Remove DsL2Edit & DsNexthop, This operation should be last*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        _sys_humber_nh_ipuc_unset_dsnh_and_dsl2edit(lchip, p_nhinfo);
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_ipuc_fwd_attr(sys_nh_param_ipuc_t *p_nhpara, sys_nh_info_ipuc_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num;
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info[CTC_MAX_LOCAL_CHIP_NUM];
    sys_nh_ref_list_node_t *p_ref_nh_list;

    curr_chip_num = sys_humber_get_local_chip_num();
    /*Build nhpara*/
    p_nhpara->hdr.dsfwd_valid = TRUE;
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        p_dsl2edit_info[lchip] = p_nhinfo->p_dsl2edit_info[lchip];
        p_nhpara->hdr.dsfwd_offset[lchip] = p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }
    p_nhpara->dsnh_offset = p_nhinfo->dsnh_info.dsnh_offset;
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_info.dsnh_offset, 1, FALSE));

    CTC_ERROR_RETURN(sys_humber_nh_create_ipuc_cb((
        sys_nh_param_com_t*)p_nhpara, (sys_nh_info_com_t*)p_nhinfo));

    CTC_UNSET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV);

    /*Notify ECMP, should update ecmp before the operation of freeing dsl2edit*dsnexthop*/
    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item(
            (sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo, p_nhpara->hdr.nhid));
    }

    /*Free old dsl2edit and dsnh*/
    for (lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        if(p_dsl2edit_info[lchip])
        {
            CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry(
                (sys_nh_db_com_entry_t*)p_dsl2edit_info));
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
sys_humber_nh_update_ipuc_cb(sys_nh_info_com_t* p_nh_db,
        sys_nh_param_com_t* p_para)
{
    sys_nh_info_ipuc_t *p_nh_info;
    sys_nh_param_ipuc_t* p_nh_para;
    /*sys_nh_ref_list_node_t *p_ref_nh_list;*/

    CTC_PTR_VALID_CHECK(p_para);
    CTC_PTR_VALID_CHECK(p_nh_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IPUC, p_para->hdr.nh_param_type);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IPUC, p_nh_db->hdr.nh_entry_type);
    p_nh_info = (sys_nh_info_ipuc_t *)(p_nh_db);
    p_nh_para = (sys_nh_param_ipuc_t*)(p_para);

    switch(p_nh_para->change_type)
    {
        case SYS_NH_CHANGE_TYPE_FWD_TO_UNROV:
            CTC_ERROR_RETURN(_sys_humber_nh_update_ipuc_fwd_to_spec(p_nh_para, p_nh_info));
            break;

        case SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR:
            CTC_ERROR_RETURN(_sys_humber_nh_update_ipuc_fwd_attr(p_nh_para, p_nh_info));
            break;

        case SYS_NH_CHANGE_TYPE_UNROV_TO_FWD:
            if(!CTC_FLAG_ISSET(p_nh_info->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
                return CTC_E_NH_ISNT_UNROV;

            CTC_ERROR_RETURN(_sys_humber_nh_update_ipuc_fwd_attr(p_nh_para, p_nh_info));
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}



