/**
 @file sys_humber_nexthop_l2.c

 @date 2009-09-19

 @version v2.0

 The file contains all nexthop layer2 related callback function
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
/* XXX_OSS_TRIM: disabled. */
//#include "sys_humber_aps.h"
#include "sys_humber_queue_enq.h"
#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

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
static INLINE int32
_sys_humber_nh_brguc_write_dsnh_with_vlan_edit(uint8 curr_chip_num,
                                              uint32 dsnh_offset, uint8 gchip,
                                              uint16 dest_vlan_ptr,  uint8 dsnh_type,
                                              ctc_vlan_egress_edit_info_t *p_vlan_edit_info,
                                              sys_nh_info_brguc_t *p_nhdb_ucast_brg)
{
    uint8 lchip, lchip2;
    uint32 dsl2edit_offset = 0;
    sys_nh_param_dsnh_t dsnh_param;
    bool is_glb_alloc_nexthop = 0;
    sys_nh_entry_table_type_t entry_type = 0;
    void *p_l2edit_entry = NULL;
    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    dsnh_param.dsnh_offset = dsnh_offset;
    dsnh_param.dest_vlan_ptr = dest_vlan_ptr;
    dsnh_param.dsnh_type = dsnh_type;
    dsnh_param.p_vlan_info = p_vlan_edit_info;

    if ( p_vlan_edit_info->e_i_loop)
    {
         if (p_vlan_edit_info->e_i_loop)
        {
            ds_l2_edit_loopback_t ds_l2_lbk;
            kal_memset(&ds_l2_lbk, 0, sizeof(ds_l2_edit_loopback_t));
            ds_l2_lbk.lb_dest_map    = SYS_HBNH_ENCODE_DESTMAP(0, gchip, SYS_ILOOP_CHANNEL_ID) ;
            ds_l2_lbk.lb_next_hop_ext = 0;
            ds_l2_lbk.lb_next_hop_ptr = SYS_HUMBER_NH_ENCODE_ILOOP_DSNH(
            p_vlan_edit_info->e_iloop_param.lpbk_lport,
                                           (p_vlan_edit_info->e_iloop_param.vpls_src_port ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
                                           (p_vlan_edit_info->e_iloop_param.inner_packet_type_valid ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
                                           p_vlan_edit_info->e_iloop_param.inner_packet_type,
                                           (p_vlan_edit_info->e_iloop_param.map_vclabel_exp ? \
            SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE),
                                           p_vlan_edit_info->e_iloop_param.customerid_valid,
                                           p_vlan_edit_info->e_iloop_param.words_removed_from_hdr);

            dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_LOOPBACK;
            entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_LPBK;
            p_l2edit_entry =  &ds_l2_lbk;

            SYS_NH_DBG_INFO("lb_dest_map = 0x%x, lb_next_hop_ptr = 0x%x \n", ds_l2_lbk.lb_dest_map, ds_l2_lbk.lb_next_hop_ptr);
        }


        if (sys_humber_chip_is_local(gchip, &lchip))
        {
            p_nhdb_ucast_brg->p_dsl2edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_info_dsl2edit_t));
            if ( p_nhdb_ucast_brg->p_dsl2edit[lchip] == NULL)
            {
                return CTC_E_NO_MEMORY;
            }
            kal_memset(p_nhdb_ucast_brg->p_dsl2edit[lchip] , 0, sizeof(sys_nh_info_dsl2edit_t));
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  entry_type, 1,  &dsl2edit_offset));
            CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl2edit_offset, p_l2edit_entry));
            SYS_NH_DBG_INFO("Lchip = %d, DsL2Edit, offset = %d\n", lchip, dsl2edit_offset);
            p_nhdb_ucast_brg->p_dsl2edit[lchip]->dsl2edit_offset = dsl2edit_offset;
            p_nhdb_ucast_brg->p_dsl2edit[lchip]->entry_tbl_type = entry_type;

        }
        else  if (CTC_LINKAGG_CHIPID == gchip)
        {
            for (lchip = 0; lchip < curr_chip_num; lchip++)
            {
                p_nhdb_ucast_brg->p_dsl2edit[lchip] = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_info_dsl2edit_t));
                if ( p_nhdb_ucast_brg->p_dsl2edit[lchip] == NULL)
                {
                    for (lchip2 = 0; lchip2 < lchip; lchip2++)
                    {
                        mem_free( p_nhdb_ucast_brg->p_dsl2edit[lchip2]);
                    }
                    return CTC_E_NO_MEMORY;
                }
                kal_memset(p_nhdb_ucast_brg->p_dsl2edit[lchip] , 0, sizeof(sys_nh_info_dsl2edit_t));
                CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  entry_type, 1,  &dsl2edit_offset));
                CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip,  entry_type, dsl2edit_offset, p_l2edit_entry));
                SYS_NH_DBG_INFO("Lchip = %d, DsL2Edit, offset = %d\n", lchip, dsl2edit_offset);
                p_nhdb_ucast_brg->p_dsl2edit[lchip]->dsl2edit_offset = dsl2edit_offset;
                p_nhdb_ucast_brg->p_dsl2edit[lchip]->entry_tbl_type = entry_type;
            }
        }
    }

    is_glb_alloc_nexthop = sys_humber_is_glb_alloc_nexthop();
    if (sys_humber_chip_is_local(gchip, &lchip))
    {
        dsnh_param.lchip = lchip;
        if ( p_nhdb_ucast_brg->p_dsl2edit[lchip])
        {
            dsnh_param.l2edit_ptr = p_nhdb_ucast_brg->p_dsl2edit[lchip]->dsl2edit_offset;
        }
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
    }
    else  if (CTC_LINKAGG_CHIPID == gchip || is_glb_alloc_nexthop)
    {
        for (lchip = 0; lchip < curr_chip_num; lchip++)
        {
            dsnh_param.lchip = lchip;
            if ( p_nhdb_ucast_brg->p_dsl2edit[lchip])
            {
                dsnh_param.l2edit_ptr = p_nhdb_ucast_brg->p_dsl2edit[lchip]->dsl2edit_offset;
            }
            CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
        }
    }
    else
    {
        return CTC_E_INVALID_GLOBAL_CHIPID;
    }

    return CTC_E_NONE;
}

/**
 @brief Callback function of create unicast bridge nexthop

 @param[in,out] p_com_nh_para, parameters used to create bridge nexthop,
                writeback dsfwd offset array to this param

 @param[out] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_create_brguc_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_brguc_t * p_nh_para_bridge;
    sys_nh_info_brguc_t *p_nhdb_ucast_brg;
    uint32 dsfwd_offset[MAX_LOCAL_CHIP_NUM];
    uint8 lchip, curr_chip_num;
    sys_nh_param_dsfwd_t dsfwd_param;
    int32 ret;
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_BRGUC, p_com_nh_para->hdr.nh_param_type);
    p_nh_para_bridge = (sys_nh_param_brguc_t *)p_com_nh_para;
    p_nhdb_ucast_brg = (sys_nh_info_brguc_t *)p_com_db;
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    kal_memset(dsfwd_offset, SYS_HUMBER_NH_INVALID_OFFSET, sizeof(dsfwd_offset));
    kal_memset(p_nhdb_ucast_brg, 0, sizeof(sys_nh_info_brguc_t));

    curr_chip_num = sys_humber_get_local_chip_num();
    SYS_NH_DBG_INFO("nhType = %d, NHID = %d, isInternaNH = %d, \n\
        GlobalPort = %d, DsNHOffset = %d, curr_chip_num = %d\n",
        p_nh_para_bridge->hdr.nh_param_type,
        p_nh_para_bridge->hdr.nhid,
        p_nh_para_bridge->hdr.is_internal_nh,
        p_nh_para_bridge->gport,
        p_nh_para_bridge->dsnh_offset,
        curr_chip_num);

    p_nhdb_ucast_brg->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_BRGUC;
    p_nhdb_ucast_brg->hdr.nh_entry_flags = 0;
    p_nhdb_ucast_brg->dest.dest_gport = p_nh_para_bridge->gport;
    dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(p_nh_para_bridge->gport);
    dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(p_nh_para_bridge->gport);

    switch(p_nh_para_bridge->nh_sub_type)
    {
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC:
            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH,
                &p_nhdb_ucast_brg->dsnh_info.dsnh_offset));
            p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC;
            break;
	case SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED:
		  CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_UNTAGGED_BRIDGE_NH,
                &p_nhdb_ucast_brg->dsnh_info.dsnh_offset));
              p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
             break;
      case SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU:
            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_RAW_PACKET_ELOG_CPU,
                &p_nhdb_ucast_brg->dsnh_info.dsnh_offset));
            p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
             break;
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT:
            CTC_PTR_VALID_CHECK(p_nh_para_bridge->p_vlan_edit_info);
            p_nhdb_ucast_brg->dsnh_info.dsnh_offset = p_nh_para_bridge->dsnh_offset;

            CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nhdb_ucast_brg->dsnh_info.dsnh_offset, 1, TRUE));

            p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT;
             CTC_ERROR_RETURN(_sys_humber_nh_brguc_write_dsnh_with_vlan_edit(curr_chip_num,
                p_nh_para_bridge->dsnh_offset,
                ((p_nh_para_bridge->gport) >> CTC_LOCAL_PORT_LENGTH),
                SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL,
                SYS_NH_PARAM_DSNH_TYPE_BRGUC,
                p_nh_para_bridge->p_vlan_edit_info,
                p_nhdb_ucast_brg));
            CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb_ucast_brg->dsnh_info.dsnh_offset, 1, TRUE));
            if(p_nh_para_bridge->p_vlan_edit_info->flag & CTC_VLAN_NH_SERVICE_QUEUE_FLAG)
            {
                dsfwd_param.service_queue_en = TRUE;
                p_nhdb_ucast_brg->hdr.nh_entry_flags |= SYS_NH_INFO_BRGUC_FLAG_SERVICE_QUEUE;
            }
            break;

        case SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT:
            return CTC_E_NOT_SUPPORT;
            break;

        case SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS:
            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH,
                &p_nhdb_ucast_brg->dsnh_info.dsnh_offset));
            p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS;
            break;

        case SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE:
            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH,
                &p_nhdb_ucast_brg->dsnh_info.dsnh_offset));
            p_nhdb_ucast_brg->nh_sub_type = SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
            dsfwd_param.service_queue_en =  TRUE;
            p_nhdb_ucast_brg->hdr.nh_entry_flags |= SYS_NH_INFO_BRGUC_FLAG_SERVICE_QUEUE;
            break;

        default:
            return CTC_E_INVALID_NH_SUB_TYPE;
    }

    dsfwd_param.is_mcast = FALSE;
    dsfwd_param.dsnh_offset = p_nhdb_ucast_brg->dsnh_info.dsnh_offset;
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*Build DsFwd Table*/
        ret = sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                         &dsfwd_offset[lchip]);
        if(ret)
            goto err;

        p_nhdb_ucast_brg->hdr.dsfwd_info[lchip].dsfwd_offset = dsfwd_offset[lchip];
        /*Writeback dsfwd offset*/
        p_nh_para_bridge->hdr.dsfwd_offset[lchip] = dsfwd_offset[lchip];
        dsfwd_param.p_dsfwd_info = &(p_nhdb_ucast_brg->hdr.dsfwd_info[lchip]);
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

/**
 @brief Callback function of delete unicast bridge nexthop

 @param[in] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_delete_brguc_cb(sys_nh_info_com_t *p_data)
{
    sys_nh_info_brguc_t *nh_brg_ucast;
     uint32 dsl2edit_offset = 0;
    uint8 lchip, curr_chip_num, gchip;
    sys_nh_param_dsfwd_t dsfwd_param;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_BRGUC, p_data->hdr.nh_entry_type);
    nh_brg_ucast = (sys_nh_info_brguc_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();
    gchip = SYS_MAP_GPORT_TO_GCHIP(nh_brg_ucast->dest.dest_gport);

    switch(nh_brg_ucast->nh_sub_type)
    {
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT:
              CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(nh_brg_ucast->dsnh_info.dsnh_offset, 1, FALSE));
            break;
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT:
               CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(nh_brg_ucast->dsnh_info.dsnh_offset, 2, FALSE));
            break;

        case SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS:
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC:
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU:
        case SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE:
        default:
            break;
    }

    /*2. Free DsFwd offset*/
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
            SYS_NH_ENTRY_TYPE_FWD, 1, nh_brg_ucast->hdr.dsfwd_info[lchip].dsfwd_offset));

       /*should update to normal bridge (tagged) nexthop*/
        dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(nh_brg_ucast->dest.dest_gport);
        dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(nh_brg_ucast->dest.dest_gport);
        dsfwd_param.is_mcast = FALSE;
        CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH, &dsfwd_param.dsnh_offset));

        dsfwd_param.p_dsfwd_info = &nh_brg_ucast->hdr.dsfwd_info[lchip];
        dsfwd_param.lchip = lchip;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
    }


    /*3. Free DsNexthop offset use dsmet init, APS, Down MEP should free ?? TBD*/
    if(SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT == nh_brg_ucast->nh_sub_type)
    {
        if (CTC_IS_LINKAGG_PORT(nh_brg_ucast->dest.dest_gport))
        {
            for(lchip = 0; lchip < curr_chip_num; lchip++)
            {

                if(nh_brg_ucast->p_dsl2edit[lchip] != NULL)
                {
                   dsl2edit_offset = nh_brg_ucast->p_dsl2edit[lchip]->dsl2edit_offset;
                   mem_free( nh_brg_ucast->p_dsl2edit[lchip]);
                   CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,  SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W, 1, dsl2edit_offset));
                }
            }
        }
        else
        {
            if (sys_humber_chip_is_local(gchip, &lchip))
            {
                if(nh_brg_ucast->p_dsl2edit[lchip] != NULL)
                {
                  dsl2edit_offset = nh_brg_ucast->p_dsl2edit[lchip]->dsl2edit_offset;
                  mem_free( nh_brg_ucast->p_dsl2edit[lchip]);
                  CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,  SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W, 1, dsl2edit_offset));
                }
            }
        }
    }

    return CTC_E_NONE;
}

/**
 @brief Callback function of create unicast bridge nexthop

 @param[in,out] p_com_nh_para, parameters used to create bridge nexthop,
                writeback dsfwd offset array to this param

 @param[out] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
 int32
sys_humber_nh_create_downmep_cb(sys_nh_param_com_t* p_com_nh_para, sys_nh_info_com_t* p_com_db)
{

    sys_nh_param_downmep_t * p_nh_para_downmep;
    sys_nh_info_downmep_t *p_nhdb_downmep;
     sys_nh_param_dsnh_t dsnh_param;
    uint16  dsnh_offset = 0;
    uint8   lchip, curr_chip_num;
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_DOWNMEP, p_com_nh_para->hdr.nh_param_type);
    p_nh_para_downmep = (sys_nh_param_downmep_t *)p_com_nh_para;
    p_nhdb_downmep = (sys_nh_info_downmep_t *)p_com_db;

    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    dsnh_param.dsnh_offset = dsnh_offset;
    dsnh_param.dest_vlan_ptr = p_nh_para_downmep->dest_vlan_ptr;
    dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_DOWNMEP;
    dsnh_param.p_vlan_info = NULL;

    p_nhdb_downmep->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_DOWNMEP;
    p_nhdb_downmep->hdr.nh_entry_flags = 0;

    curr_chip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip,  SYS_NH_ENTRY_TYPE_NEXTHOP_4W, 1,  &dsnh_param.dsnh_offset ));
        dsnh_param.lchip = lchip;
        p_nhdb_downmep->p_dsnh_info[lchip].dsnh_offset = dsnh_param.dsnh_offset;
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
    }
    return CTC_E_NONE;

}

/**
 @brief Callback function of delete unicast bridge nexthop

 @param[in] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_delete_downmep_cb(sys_nh_info_com_t *p_data)
{
    sys_nh_info_downmep_t *p_nhdb_downmep;
    uint8   lchip, curr_chip_num;

    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_DOWNMEP, p_data->hdr.nh_entry_type);
    p_nhdb_downmep = (sys_nh_info_downmep_t *)(p_data);

    curr_chip_num = sys_humber_get_local_chip_num();

     for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,  SYS_NH_ENTRY_TYPE_NEXTHOP_4W, 1, p_nhdb_downmep->p_dsnh_info[lchip].dsnh_offset  ));

    }
    return CTC_E_NONE;
}


