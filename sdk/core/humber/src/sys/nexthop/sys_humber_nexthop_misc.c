/**
 @file sys_humber_nexthop_misc.c

 @date 2009-11-25

 @version v2.0

 The file contains all non-layer2 and non-layer3 nexthop related callback function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_common.h"
#include "ctc_linklist.h"
#include "ctc_cpu_traffic.h"
#include "sys_humber_chip.h"
#include "sys_humber_vlan.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_queue_enq.h"

#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define SYS_HUMBER_NH_ILOOP_MAX_REMOVE_WORDS    31

#define SYS_NH_FLEX_FLAG_CONFLICT_CHECK(flag) \
    { \
        if(flag & CTC_FLEX_NH_SWAP_MAC) \
        { \
            if((flag & ~CTC_FLEX_NH_SWAP_MAC) > 0) \
            { \
                return CTC_E_INVALID_PARAM; \
            } \
        } \
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
 @brief Callback function of create unicast bridge nexthop

 @param[in,out] p_com_nh_para, parameters used to create bridge nexthop,
                writeback dsfwd offset array to this param

 @param[out] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_create_special_cb(sys_nh_param_com_t* p_com_nh_para, sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_special_t* p_nh_para;
    sys_nh_info_special_t* p_nhdb;
    uint32 dsfwd_offset[MAX_LOCAL_CHIP_NUM];
    uint8 lchip, curr_chip_num, gchip;
    sys_nh_param_dsfwd_t dsfwd_param;
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    p_nh_para = (sys_nh_param_special_t *)p_com_nh_para;
    p_nhdb = (sys_nh_info_special_t *)p_com_db;
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    curr_chip_num = sys_humber_get_local_chip_num();


    switch(p_nh_para->hdr.nh_param_type)
    {
        case SYS_HUMBER_NH_TYPE_UNROV:
        case SYS_HUMBER_NH_TYPE_DROP:
            dsfwd_param.drop_pkt = TRUE;
            break;

        case SYS_HUMBER_NH_TYPE_TOCPU:
            dsfwd_param.dest_id = SYS_CPU_CHANNEL_ID;
            break;
        default:
            CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
    }
    p_nhdb->hdr.nh_entry_type = (p_nh_para->hdr.nh_param_type);

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        if (p_nh_para->hdr.dsfwd_valid)
        {
            dsfwd_offset[lchip] = p_nh_para->hdr.dsfwd_offset[lchip];
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                             &dsfwd_offset[lchip]));
        }

        p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = dsfwd_offset[lchip];
        p_nh_para->hdr.dsfwd_offset[lchip] = dsfwd_offset[lchip];
        sys_humber_get_gchip_id(lchip, &gchip);
        dsfwd_param.dest_chipid = gchip;
        /*Writeback dsfwd offset*/
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.is_mcast = FALSE;
        dsfwd_param.lchip = lchip;
        dsfwd_param.dsnh_offset = CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_FWD_CPU,0,0);

        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_delete_special_cb(sys_nh_info_com_t* p_data)
{
    sys_nh_info_special_t *nh_info;
    sys_nh_param_dsfwd_t dsfwd_param;
    uint8 lchip, curr_chip_num;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    nh_info = (sys_nh_info_special_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();

    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    /*Free dsfwd offset*/
    for (lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
        SYS_NH_ENTRY_TYPE_FWD, 1, nh_info->hdr.dsfwd_info[lchip].dsfwd_offset));

       dsfwd_param.p_dsfwd_info = &nh_info->hdr.dsfwd_info[lchip];
       dsfwd_param.lchip = lchip;
       dsfwd_param.drop_pkt = 1;
       /*Write table*/
       CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_create_iloop_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_iloop_t* p_nh_para;
    sys_nh_info_special_t* p_nhdb;
    uint32 dsfwd_offset[MAX_LOCAL_CHIP_NUM];
    uint8 lchip, curr_chip_num, gchip;
    sys_nh_param_dsfwd_t dsfwd_param;
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    p_nh_para = (sys_nh_param_iloop_t *)p_com_nh_para;
    CTC_PTR_VALID_CHECK(p_nh_para->p_iloop_param);
    if(p_nh_para->p_iloop_param->words_removed_from_hdr > SYS_HUMBER_NH_ILOOP_MAX_REMOVE_WORDS)
        return CTC_E_INVALID_PARAM;

    p_nhdb = (sys_nh_info_special_t *)p_com_db;
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    curr_chip_num = sys_humber_get_local_chip_num();
    p_nhdb->hdr.nh_entry_type = (p_nh_para->hdr.nh_param_type);
    dsfwd_param.dest_id = SYS_ILOOP_CHANNEL_ID;
    dsfwd_param.dsnh_offset = SYS_HUMBER_NH_ENCODE_ILOOP_DSNH(
            p_nh_para->p_iloop_param->lpbk_lport,
            (p_nh_para->p_iloop_param->vpls_src_port ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
            (p_nh_para->p_iloop_param->inner_packet_type_valid ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
            p_nh_para->p_iloop_param->inner_packet_type,
            (p_nh_para->p_iloop_param->map_vclabel_exp ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
            p_nh_para->p_iloop_param->customerid_valid,
            p_nh_para->p_iloop_param->words_removed_from_hdr);
    if(p_nh_para->p_iloop_param->sequence_en)
    {
        dsfwd_param.sequence_chk_en = TRUE;
        dsfwd_param.stats_ptr = p_nh_para->p_iloop_param->sequence_counter_index;
    }

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                         &dsfwd_offset[lchip]));

        p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = dsfwd_offset[lchip];
        p_nh_para->hdr.dsfwd_offset[lchip] = dsfwd_offset[lchip];
        sys_humber_get_gchip_id(lchip, &gchip);
        dsfwd_param.dest_chipid = gchip;
        /*Writeback dsfwd offset*/
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.is_mcast = FALSE;
        dsfwd_param.lchip = lchip;
	    dsfwd_param.service_queue_en = p_nh_para->p_iloop_param->service_queue_en;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_delete_iloop_cb(sys_nh_info_com_t* p_data)
{
    sys_nh_info_special_t *nh_info;
    sys_nh_param_dsfwd_t dsfwd_param;
    uint8 lchip, curr_chip_num;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    nh_info = (sys_nh_info_special_t *)(p_data);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_ILOOP, p_data->hdr.nh_entry_type);

    curr_chip_num = sys_humber_get_local_chip_num();
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    /*Free dsfwd offset*/
    for (lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
        SYS_NH_ENTRY_TYPE_FWD, 1, nh_info->hdr.dsfwd_info[lchip].dsfwd_offset));

       dsfwd_param.p_dsfwd_info = &nh_info->hdr.dsfwd_info[lchip];
       dsfwd_param.lchip = lchip;
       dsfwd_param.drop_pkt = 1;
       /*Write table*/
       CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_update_iloop_cb(sys_nh_info_com_t* p_iloop_info, sys_nh_param_com_t* p_iloop_param)
{

    return CTC_E_NONE;
}

int32
sys_humber_nh_create_rspan_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_rspan_t* p_nh_para;
    uint8 lchip, curr_chip_num;
    sys_nh_param_dsnh_t dsnh_param;
    ctc_vlan_egress_edit_info_t  vlan_egress_edit_info;
    sys_nh_info_rspan_t  *p_nhdb = NULL;
    sys_vlan_info_t vlan_info;

    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    p_nh_para = (sys_nh_param_rspan_t *)p_com_nh_para;
    p_nhdb = (sys_nh_info_rspan_t *)p_com_db;

    CTC_PTR_VALID_CHECK(p_nh_para->p_rspan_param);
    CTC_VLAN_RANGE_CHECK(p_nh_para->p_rspan_param->rspan_vid);

    CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nh_para->dsnh_offset, 1, TRUE));
    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    curr_chip_num = sys_humber_get_local_chip_num();


    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = p_nh_para->p_rspan_param->rspan_vid;

    vlan_egress_edit_info.output_svid = p_nh_para->p_rspan_param->rspan_vid;

    CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, & dsnh_param.dest_vlan_ptr));
    dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_RSPAN;
    dsnh_param.p_vlan_info = &vlan_egress_edit_info;

    dsnh_param.dsnh_offset = p_nh_para->dsnh_offset;

    p_nhdb->dsnh_info.dsnh_offset = p_nh_para->dsnh_offset;
    p_nhdb->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_RSPAN;
    p_nhdb->hdr.nh_entry_flags = 0;

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        dsnh_param.lchip = lchip;

        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
    }


   CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nh_para->dsnh_offset, 1, TRUE));
    return CTC_E_NONE;
}

int32
sys_humber_nh_delete_rspan_cb(sys_nh_info_com_t* p_com_db)
{

    sys_nh_info_rspan_t  *p_nhdb = NULL;

     /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_db);
    p_nhdb = (sys_nh_info_rspan_t *)p_com_db;


    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_info.dsnh_offset, 1, FALSE));
    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_flex_write_dsnh(uint8 curr_chip_num,
                              uint32 dsnh_offset, uint8 gchip,
                              uint16 dest_vlan_ptr, uint8 dsnh_type,
                              ctc_flex_nh_param_t *p_flex_info,
                              sys_nh_info_flex_t *p_nhdb)
{
    uint8 lchip,lchip2;
    uint32 dsl2edit_offset = 0;
    sys_nh_param_dsnh_t dsnh_param;
    bool is_glb_alloc_nexthop = 0;
    sys_nh_entry_table_type_t entry_type = 0;
    void *p_l2edit_entry = NULL;
    ds_l2_edit_eth8w_t l2edit_8w;
    ctc_vlan_egress_edit_info_t vlan_info;
    uint8 use_dsnh8w = false;

      
    void *p_l3edit_entry = NULL;
    ds_l3edit_nat4w_t l3edit_4w;
    uint32 dsl3edit_offset = 0;

    kal_memset(&l2edit_8w, 0, sizeof(ds_l2_edit_eth8w_t));
    kal_memset(&l3edit_4w, 0, sizeof(ds_l3edit_nat4w_t));
    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    kal_memset(&vlan_info, 0, sizeof(ctc_vlan_egress_edit_info_t));

    dsnh_param.dsnh_offset = dsnh_offset;
    dsnh_param.dest_vlan_ptr = dest_vlan_ptr;
    dsnh_param.dsnh_type = dsnh_type;
    dsnh_param.p_vlan_info = &vlan_info;

    if(p_flex_info->flag & CTC_FLEX_NH_REPLACE_MAC_DA)
    {
        l2edit_8w.type = 1;
        l2edit_8w.derive_mcast_mac = 1;
        l2edit_8w.mac_dah = (p_flex_info->mac_da[0]<<8)|p_flex_info->mac_da[1];
        l2edit_8w.mac_dal= p_flex_info->mac_da[5]|(p_flex_info->mac_da[4]<<8)|(p_flex_info->mac_da[3]<<16)|(p_flex_info->mac_da[2]<<24);
    }

    if(p_flex_info->flag & CTC_FLEX_NH_REPLACE_MAC_SA)
    {
        l2edit_8w.mac_sah = (p_flex_info->mac_sa[0]<<8)|p_flex_info->mac_sa[1];
        l2edit_8w.mac_sal= p_flex_info->mac_sa[5]|(p_flex_info->mac_sa[4]<<8)|(p_flex_info->mac_sa[3]<<16)|(p_flex_info->mac_sa[2]<<24);
        l2edit_8w.mac_sa_valid = 1;
        l2edit_8w.overwrite_ether_type = 1;
    }

    vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE;
    vlan_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE;
    if ((p_flex_info->flag & CTC_FLEX_NH_REPLACE_SVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_STRIP_SVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if ((p_flex_info->flag & CTC_FLEX_NH_REPLACE_SVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_INSERT_SVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if ((p_flex_info->flag & CTC_FLEX_NH_INSERT_SVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_STRIP_SVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if (p_flex_info->flag & CTC_FLEX_NH_REPLACE_SVLAN_TAG)
    {
        vlan_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN;
        vlan_info.output_svid = p_flex_info->vlan_id;
        vlan_info.edit_flag = CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID;
    }
    else if (p_flex_info->flag & CTC_FLEX_NH_STRIP_SVLAN_TAG)
    {
        vlan_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_STRIP_VLAN;
    }
    else if (p_flex_info->flag & CTC_FLEX_NH_INSERT_SVLAN_TAG)
    {
        vlan_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_INSERT_VLAN;
        vlan_info.output_svid = p_flex_info->vlan_id;
        vlan_info.edit_flag = CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID;
    }

    if ((p_flex_info->flag & CTC_FLEX_NH_REPLACE_CVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_STRIP_CVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if ((p_flex_info->flag & CTC_FLEX_NH_REPLACE_CVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_INSERT_CVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if ((p_flex_info->flag & CTC_FLEX_NH_INSERT_CVLAN_TAG) && 
        (p_flex_info->flag & CTC_FLEX_NH_STRIP_CVLAN_TAG))
    {
        return CTC_E_VLAN_EDIT_CONFLICT;
    }
    if (p_flex_info->flag & CTC_FLEX_NH_REPLACE_CVLAN_TAG)
    {
        vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN;
        vlan_info.output_cvid = p_flex_info->vlan_id;
        vlan_info.edit_flag |= CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID;
    }
    else if (p_flex_info->flag & CTC_FLEX_NH_STRIP_CVLAN_TAG)
    {
        vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_STRIP_VLAN;
    }
    else if (p_flex_info->flag & CTC_FLEX_NH_INSERT_CVLAN_TAG)
    {
        vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_INSERT_VLAN;
        vlan_info.output_cvid = p_flex_info->cvlan_id;
        vlan_info.edit_flag |= CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID;
    }

    if (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_INSERT_SVLAN_TAG) &&
        CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_INSERT_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
    }
    if (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_SVLAN_TAG) &&
        CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
    }

      
    if (p_flex_info->flag & CTC_FLEX_NH_REPLACE_STAG_COS)
    {
        vlan_info.replace_stag_cos_valid = TRUE;
        vlan_info.stag_cos = p_flex_info->cos;
    }
    else if (p_flex_info->flag & CTC_FLEX_NH_MAP_STAG_COS)
    {
        vlan_info.map_stag_cos_valid = TRUE;
    }
    
      
    if (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_IPDA) ||
        CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        use_dsnh8w = TRUE;
    }

    if (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_IPDA))
    {
        l3edit_4w.replace_ipda = 1;
        l3edit_4w.ipda = p_flex_info->ipda;
        l3edit_4w.nat_mode = 1;
    }
    if (CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        l3edit_4w.replace_l4_dest_port = 1;
        l3edit_4w.l4_dest_port = p_flex_info->dst_port;
        l3edit_4w.nat_mode = 1;
    }


    if((p_flex_info->flag & CTC_FLEX_NH_REPLACE_MAC_DA) || (p_flex_info->flag & CTC_FLEX_NH_REPLACE_MAC_SA))
    {
        dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_ETH_MAC;
    	entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W;
    	p_l2edit_entry =  &l2edit_8w;

        if (sys_humber_chip_is_local(gchip, &lchip))
        {
            p_nhdb->p_dsl2edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE,sizeof(sys_nh_info_dsl2edit_t));
            if(p_nhdb->p_dsl2edit[lchip] == NULL)
            {
               return CTC_E_NO_MEMORY;
            }
            kal_memset(p_nhdb->p_dsl2edit[lchip], 0, sizeof(sys_nh_info_dsl2edit_t));

            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  entry_type, 1,  &dsl2edit_offset));
            CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl2edit_offset, p_l2edit_entry));
            SYS_NH_DBG_INFO("Lchip = %d, DsL2Edit, offset = %d\n", lchip, dsl2edit_offset);
            p_nhdb->p_dsl2edit[lchip]->dsl2edit_offset = dsl2edit_offset;
            p_nhdb->p_dsl2edit[lchip]->entry_tbl_type = entry_type;

        }
        else  if (CTC_LINKAGG_CHIPID == gchip)
        {
            for(lchip = 0; lchip < curr_chip_num; lchip++)
            {
                p_nhdb->p_dsl2edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE,sizeof(sys_nh_info_dsl2edit_t));
                if( p_nhdb->p_dsl2edit[lchip] == NULL)
                {
                   for(lchip2 = 0;lchip2 <lchip; lchip2++)
                   {
                       mem_free(p_nhdb->p_dsl2edit[lchip2]);
                   }
                   return CTC_E_NO_MEMORY;
                }
                kal_memset(p_nhdb->p_dsl2edit[lchip] ,0,sizeof(sys_nh_info_dsl2edit_t));

                CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  entry_type, 1,  &dsl2edit_offset));
                CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl2edit_offset, p_l2edit_entry));
                SYS_NH_DBG_INFO("Lchip = %d, DsL2Edit, offset = %d\n", lchip, dsl2edit_offset);
                p_nhdb->p_dsl2edit[lchip]->dsl2edit_offset = dsl2edit_offset;
                p_nhdb->p_dsl2edit[lchip]->entry_tbl_type = entry_type;
            }
        }
    }

    if(CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_IPDA) ||
       CTC_FLAG_ISSET(p_flex_info->flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        dsnh_param.l3dit_type = SYS_NH_L3EDIT_TYPE_NAT_4W;
        entry_type = SYS_NH_ENTRY_TYPE_L3EDIT_NAT_4W;
        p_l3edit_entry =  &l3edit_4w;

        if (sys_humber_chip_is_local(gchip, &lchip))
        {
            p_nhdb->p_dsl3edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_info_dsl3edit_t));
            if(p_nhdb->p_dsl3edit[lchip] == NULL)
            {
               return CTC_E_NO_MEMORY;
            }
            kal_memset(p_nhdb->p_dsl3edit[lchip], 0, sizeof(sys_nh_info_dsl3edit_t));

            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, entry_type, 1, &dsl3edit_offset));
            CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl3edit_offset, p_l3edit_entry));
            SYS_NH_DBG_INFO("Lchip = %d, DsL3Edit, offset = %d\n", lchip, dsl3edit_offset);
            p_nhdb->p_dsl3edit[lchip]->dsl3edit_offset = dsl3edit_offset;
            p_nhdb->p_dsl3edit[lchip]->entry_tbl_type  = entry_type;
        }
        else if (CTC_LINKAGG_CHIPID == gchip)
        {
            for(lchip = 0; lchip < curr_chip_num; lchip++)
            {
                p_nhdb->p_dsl3edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_info_dsl3edit_t));
                if (p_nhdb->p_dsl3edit[lchip] == NULL)
                {
                   for(lchip2 = 0;lchip2 <lchip; lchip2++)
                   {
                       mem_free(p_nhdb->p_dsl3edit[lchip2]);
                   }
                   return CTC_E_NO_MEMORY;
                }
                kal_memset(p_nhdb->p_dsl3edit[lchip] ,0,sizeof(sys_nh_info_dsl3edit_t));

                CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  entry_type, 1,  &dsl3edit_offset));
                CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl3edit_offset, p_l3edit_entry));
                SYS_NH_DBG_INFO("Lchip = %d, DsL3Edit, offset = %d\n", lchip, dsl3edit_offset);
                p_nhdb->p_dsl3edit[lchip]->dsl3edit_offset = dsl3edit_offset;
                p_nhdb->p_dsl3edit[lchip]->entry_tbl_type  = entry_type;
            }
        }
    }

    is_glb_alloc_nexthop = sys_humber_is_glb_alloc_nexthop();
    if (sys_humber_chip_is_local(gchip, &lchip))
    {
        dsnh_param.lchip = lchip;
        if(p_nhdb->p_dsl2edit[lchip])
        {
            dsnh_param.l2edit_ptr = p_nhdb->p_dsl2edit[lchip]->dsl2edit_offset;
        }
        if( p_nhdb->p_dsl3edit[lchip])
        {
            dsnh_param.l3edit_ptr = p_nhdb->p_dsl3edit[lchip]->dsl3edit_offset;
        }

        if (use_dsnh8w)
        {
            CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh8w(&dsnh_param));
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
        }
    }
    else  if (CTC_LINKAGG_CHIPID == gchip || is_glb_alloc_nexthop)
    {
        for(lchip = 0; lchip < curr_chip_num; lchip++)
        {
            dsnh_param.lchip = lchip;
            if( p_nhdb->p_dsl2edit[lchip])
            {
                dsnh_param.l2edit_ptr = p_nhdb->p_dsl2edit[lchip]->dsl2edit_offset;
            }
            if( p_nhdb->p_dsl3edit[lchip])
            {
                dsnh_param.l3edit_ptr = p_nhdb->p_dsl3edit[lchip]->dsl3edit_offset;
            }
            if (use_dsnh8w)
            {
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh8w(&dsnh_param));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
            }
        }
    }
    else
    {
       return CTC_E_INVALID_GLOBAL_CHIPID;
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_create_flex_cb(sys_nh_param_com_t* p_com_nh_para,
                             sys_nh_info_com_t* p_com_db)
{


    sys_nh_param_flex_t* p_nh_flex;
    uint8 lchip, curr_chip_num;
    uint32 dsfwd_offset[MAX_LOCAL_CHIP_NUM];
    //sys_nh_param_dsnh_t dsnh_param;
    sys_nh_param_dsfwd_t dsfwd_param;
    sys_nh_info_flex_t  *p_nhdb = NULL;
    int32 ret;
    bool use_dsnh8w = FALSE;
    
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_FLEX, p_com_nh_para->hdr.nh_param_type);

    p_nh_flex = (sys_nh_param_flex_t *)p_com_nh_para;
    p_nhdb    = (sys_nh_info_flex_t *)p_com_db;

    SYS_NH_FLEX_FLAG_CONFLICT_CHECK(p_nh_flex->nh_flex_param.flag);
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    kal_memset(dsfwd_offset, SYS_HUMBER_NH_INVALID_OFFSET, sizeof(dsfwd_offset));
    kal_memset(p_nhdb, 0, sizeof(sys_nh_info_flex_t));

    curr_chip_num = sys_humber_get_local_chip_num();

    SYS_NH_DBG_INFO("nhType = %d, NHID = %d, isInternaNH = %d, \n\
        GlobalPort = %d, DsNHOffset = %d, curr_chip_num = %d\n",
        p_nh_flex->hdr.nh_param_type,
        p_nh_flex->hdr.nhid,
        p_nh_flex->hdr.is_internal_nh,
        p_nh_flex->nh_flex_param.gport,
        p_nh_flex->nh_flex_param.dsnh_offset,
        curr_chip_num);

    p_nhdb->hdr.nh_entry_type = p_nh_flex->hdr.nh_param_type;
    p_nhdb->hdr.nh_entry_flags = 0;
    p_nhdb->gport = p_nh_flex->nh_flex_param.gport;
    p_nhdb->flag = p_nh_flex->nh_flex_param.flag;
    dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(p_nhdb->gport);
    dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(p_nhdb->gport);

    if (CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_INSERT_SVLAN_TAG) &&
        CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_INSERT_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLEX_FLAG_USE_DSNH8W);
    }
    if (CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_SVLAN_TAG) &&
        CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLEX_FLAG_USE_DSNH8W);
    }

    if (CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_IPDA) ||
        CTC_FLAG_ISSET(p_nh_flex->nh_flex_param.flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLEX_FLAG_USE_DSNH8W);
    }

    if(p_nh_flex->nh_flex_param.flag & CTC_FLEX_NH_SWAP_MAC)
    {
        CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_SWAPMAC_NH,
            &p_nhdb->dsnh_info.dsnh_offset));
    }
    else
    {
        p_nhdb->dsnh_info.dsnh_offset = p_nh_flex->nh_flex_param.dsnh_offset;
        
        if (CTC_HUMBER_LPORT_TO_CPU != CTC_MAP_GPORT_TO_LPORT(p_nhdb->gport))
        {
            if (use_dsnh8w)
            {
                CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nhdb->dsnh_info.dsnh_offset, 2, TRUE));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nhdb->dsnh_info.dsnh_offset, 1, TRUE));
            }

            /*write l2edit and nexthop*/
            CTC_ERROR_RETURN(_sys_humber_nh_flex_write_dsnh(curr_chip_num,
                                p_nh_flex->nh_flex_param.dsnh_offset,
                                ((p_nhdb->gport) >> CTC_LOCAL_PORT_LENGTH),
                                SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL,
                                SYS_NH_PARAM_DSNH_TYPE_BRGUC,
                                &p_nh_flex->nh_flex_param,
                                p_nhdb));

            if (use_dsnh8w)
            {
                CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_info.dsnh_offset, 2, TRUE));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_info.dsnh_offset, 1, TRUE));
            }
        }
    }

    dsfwd_param.is_mcast = FALSE;
    dsfwd_param.dsnh_offset = p_nhdb->dsnh_info.dsnh_offset;
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        ret = sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                         &dsfwd_offset[lchip]);
        if(ret)
            goto err;

        p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = dsfwd_offset[lchip];
        /*Writeback dsfwd offset*/
        p_nh_flex->hdr.dsfwd_offset[lchip] = dsfwd_offset[lchip];
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.lchip = lchip;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }

    return CTC_E_NONE;
err:
    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        if(SYS_HUMBER_NH_INVALID_OFFSET != dsfwd_offset[lchip])
        {
            sys_humber_nh_offset_free(lchip, \
                SYS_NH_ENTRY_TYPE_FWD, 1, dsfwd_offset[lchip]);
        }
    }
    CTC_ERROR_RETURN(ret);

    return CTC_E_NONE;
}

int32
sys_humber_nh_delete_flex_cb(sys_nh_info_com_t* p_flex_info)
{
    sys_nh_info_flex_t *nh_info_flex;
    uint32 dsl2edit_offset = 0;
    sys_nh_entry_table_type_t  entry_tbl_type = 0;
    uint8 lchip, curr_chip_num, gchip;
    sys_nh_param_dsfwd_t dsfwd_param;
    bool use_dsnh8w = FALSE;
    uint32 dsl3edit_offset = 0;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_flex_info);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_FLEX, p_flex_info->hdr.nh_entry_type);

    nh_info_flex = (sys_nh_info_flex_t *)(p_flex_info);
    curr_chip_num = sys_humber_get_local_chip_num();
    gchip = SYS_MAP_GPORT_TO_GCHIP(nh_info_flex->gport);

    if (CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_INSERT_SVLAN_TAG) &&
        CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_INSERT_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
    }
    if (CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_SVLAN_TAG) &&
        CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_CVLAN_TAG) &&
        (CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_MAC_DA) ||
         CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_MAC_SA)))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
    }
      
    if (CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_IPDA) ||
        CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        use_dsnh8w = TRUE;
        dsfwd_param.nexthop_ext = TRUE;
    }

    /*1. Free DsNexthop offset*/
    if (!(nh_info_flex->flag & CTC_FLEX_NH_SWAP_MAC))
    {
        if (CTC_HUMBER_LPORT_TO_CPU != CTC_MAP_GPORT_TO_LPORT(nh_info_flex->gport))
        {
            if (use_dsnh8w)
            {
                CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(nh_info_flex->dsnh_info.dsnh_offset, 2, FALSE));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(nh_info_flex->dsnh_info.dsnh_offset, 1, FALSE));
            }
        }
    }

    /*2. Free DsFwd offset*/
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
            SYS_NH_ENTRY_TYPE_FWD, 1, nh_info_flex->hdr.dsfwd_info[lchip].dsfwd_offset));

       /*should update to normal bridge (tagged) nexthop*/
        dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(nh_info_flex->gport);
        dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(nh_info_flex->gport);
        dsfwd_param.is_mcast = FALSE;
        CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH, &dsfwd_param.dsnh_offset));

        dsfwd_param.p_dsfwd_info = &nh_info_flex->hdr.dsfwd_info[lchip];
        dsfwd_param.lchip = lchip;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }


    /*3. Free Dsl2edit*/
    if((nh_info_flex->flag & CTC_FLEX_NH_REPLACE_MAC_DA)
        || (nh_info_flex->flag & CTC_FLEX_NH_REPLACE_MAC_SA))
    {
        if (CTC_IS_LINKAGG_PORT(nh_info_flex->gport))
        {
            for(lchip = 0; lchip < curr_chip_num; lchip++)
            {
                if(nh_info_flex->p_dsl2edit[lchip] != NULL)
                {
                   dsl2edit_offset = nh_info_flex->p_dsl2edit[lchip]->dsl2edit_offset;
                   entry_tbl_type = nh_info_flex->p_dsl2edit[lchip]->entry_tbl_type;
                   mem_free( nh_info_flex->p_dsl2edit[lchip]);
                   CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,  entry_tbl_type, 1, dsl2edit_offset));
                }
            }
        }
        else
        {
            if (sys_humber_chip_is_local(gchip, &lchip))
            {
                if(nh_info_flex->p_dsl2edit[lchip] != NULL)
                {
                  dsl2edit_offset = nh_info_flex->p_dsl2edit[lchip]->dsl2edit_offset;
                  entry_tbl_type = nh_info_flex->p_dsl2edit[lchip]->entry_tbl_type;
                  mem_free( nh_info_flex->p_dsl2edit[lchip]);
                  CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,  entry_tbl_type, 1, dsl2edit_offset));
                }
            }
        }
    }
    
    if(CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_IPDA) ||
       CTC_FLAG_ISSET(nh_info_flex->flag, CTC_FLEX_NH_REPLACE_DST_PORT))
    {
        if (CTC_IS_LINKAGG_PORT(nh_info_flex->gport))
        {
            for(lchip = 0; lchip < curr_chip_num; lchip++)
            {
                if (nh_info_flex->p_dsl3edit[lchip] != NULL)
                {
                   dsl3edit_offset = nh_info_flex->p_dsl3edit[lchip]->dsl3edit_offset;
                   entry_tbl_type = nh_info_flex->p_dsl3edit[lchip]->entry_tbl_type;
                   mem_free(nh_info_flex->p_dsl3edit[lchip]);
                   CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, entry_tbl_type, 1, dsl3edit_offset));
                }
            }
        }
        else
        {
            if (sys_humber_chip_is_local(gchip, &lchip))
            {
                if(nh_info_flex->p_dsl3edit[lchip] != NULL)
                {
                  dsl3edit_offset = nh_info_flex->p_dsl3edit[lchip]->dsl3edit_offset;
                  entry_tbl_type = nh_info_flex->p_dsl3edit[lchip]->entry_tbl_type;
                  mem_free(nh_info_flex->p_dsl3edit[lchip]);
                  CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, entry_tbl_type, 1, dsl3edit_offset));
                }
            }
        }
    }

    return CTC_E_NONE;
}
