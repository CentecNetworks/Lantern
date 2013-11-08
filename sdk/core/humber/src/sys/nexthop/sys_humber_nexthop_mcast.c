/**
 @file sys_humber_nexthop_l3.c

 @date 2009-11-23

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
#include "sys_humber_vlan.h"
#include "sys_humber_stats.h"
#include "sys_humber_port.h"
#include "sys_humber_linkagg.h"
#include "drv_io.h"
#include "sys_humber_queue_enq.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP   4
#define SYS_HUMBER_NH_LOGICAL_REPLI_MAX_NUM     4096
#define SYS_HUMBER_NH_LOGICAL_REPLI_MAGIC_NUM     64
#define SYS_HUMBER_NH_LOGICAL_RELI_CTL_BIT        26

struct sys_nh_mcast_dsmet_io_s
{
    sys_nh_param_mcast_member_t *p_mem_param;
    uint32 met_offset;
    uint32 next_met_offset;
    uint32 dsnh_offset;
    sys_nh_mcast_meminfo_t *p_next_mem;
};
typedef struct sys_nh_mcast_dsmet_io_s sys_nh_mcast_dsmet_io_t;

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
static int32
_sys_humber_nh_mcast_analyze_member(ctc_list_pointer_t *p_db_member_list,
                                   sys_nh_param_mcast_member_t *p_member,
                                   sys_nh_mcast_meminfo_t **pp_mem_node,
                                   sys_nh_mcast_repli_node_t **pp_repli_node,
                                   bool *p_entry_exit,
                                   uint32 *p_repli_pos)
{
    sys_nh_mcast_meminfo_t *p_meminfo;
    sys_nh_mcast_meminfo_withrepli_t *p_mem_withrepli;
     sys_nh_mcast_repli_node_t *p_repli;
     ctc_list_pointer_node_t *p_pos_mem, *p_pos_repli;


    *pp_mem_node = NULL;
    *pp_repli_node = NULL;
    *p_entry_exit = FALSE;
    *p_repli_pos = 0;
    CTC_LIST_POINTER_LOOP(p_pos_mem, p_db_member_list)
    {
        p_meminfo = _ctc_container_of(p_pos_mem, sys_nh_mcast_meminfo_t, list_head);

        switch(p_member->member_type)
        {
        case  SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
            if((p_meminfo->dsmet.member_type == p_member->member_type) &&
               (p_meminfo->dsmet.ucastid == p_member->destid) &&
               (p_meminfo->dsmet.aps_protection_path == p_member->is_protection_path) &&
               (p_meminfo->dsmet.ref_nhid == p_member->ref_nhid))
            {
                *pp_mem_node = p_meminfo;
                *p_entry_exit = TRUE;
                return CTC_E_NONE;
            }
             break;
        case   SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
        case SYS_NH_PARAM_MCAST_MEM_REMOTE:
        case SYS_NH_PARAM_BRGMC_MEM_RAPS:
             if( (p_meminfo->dsmet.member_type == p_member->member_type) &&
                 (p_meminfo->dsmet.is_linkagg == p_member->is_linkagg) &&
                  (p_meminfo->dsmet.ucastid == p_member->destid))
                {
                   *pp_mem_node = p_meminfo;
                   *p_entry_exit = TRUE;
                    return CTC_E_NONE;
                }
         break;
        case  SYS_NH_PARAM_IPMC_MEM_LOCAL:
        case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
             /* physical replication  */
             if( (p_meminfo->dsmet.member_type == p_member->member_type) &&
              (p_meminfo->dsmet.is_linkagg == p_member->is_linkagg) &&
              (p_meminfo->dsmet.ucastid == p_member->destid)
               &&(p_meminfo->dsmet.vid == p_member->vid))
              {
                    *pp_mem_node = p_meminfo;
                    p_mem_withrepli = (sys_nh_mcast_meminfo_withrepli_t*)p_meminfo;

                    CTC_LIST_POINTER_LOOP(p_pos_repli, &(p_mem_withrepli->repli_list))
                    {  /*physical replication only have one member*/
                        p_repli = _ctc_container_of(p_pos_repli, sys_nh_mcast_repli_node_t, list_head);
                        *pp_repli_node = p_repli;
                        *p_entry_exit = TRUE;
                         return CTC_E_NONE;
                    }
              }
              break;
        case    SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
            if( (p_meminfo->dsmet.member_type == p_member->member_type)
                &&  (p_meminfo->dsmet.ref_nhid == p_member->ref_nhid))
            {
                *pp_mem_node = p_meminfo;
                *p_entry_exit = TRUE;
                return CTC_E_NONE;
            }
            break;
        default:
        break;

        }
    }
    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_process_l2edit(sys_nh_param_mcast_member_t *p_mem_param,
                                   sys_nh_db_dsl2editeth4w_t **pp_dsl2edit_param)
{
    sys_nh_db_dsl2editeth4w_t *p_dsl2edit_param, dsl2edit_param;

    kal_memset(&dsl2edit_param, 0, sizeof(sys_nh_db_dsl2editeth4w_t));
    p_dsl2edit_param = &dsl2edit_param;
    dsl2edit_param.hdr.entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W;
    dsl2edit_param.hdr.lchip = p_mem_param->lchip;
    dsl2edit_param.output_vid = p_mem_param->vid;
    dsl2edit_param.ouput_vlan_is_svlan = TRUE;
    CTC_SET_FLAG(dsl2edit_param.flags, SYS_NH_DSL2EDIT_DERIVE_MCAST_MAC);

    if (p_mem_param->l3if_type == SYS_NH_PARAM_L3IF_ROUTED_PORT)
    {
         *pp_dsl2edit_param = NULL;
     }
    else
    {

        CTC_ERROR_RETURN(sys_humber_nh_db_set_entry((sys_nh_db_com_entry_t**)(&p_dsl2edit_param)));
        *pp_dsl2edit_param = p_dsl2edit_param;
    }


    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_repli_update_dsnh(uint8 lchip, uint32 old_dsnh_offset,
                                       uint32 new_dsnh_offset, uint32 update_cnt)
{
    ds_nexthop_t dsnh;
    uint32 cmd_r, cmd_w, offset;
    int32 i;

    cmd_w = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
    cmd_r = DRV_IOR(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
    for(i = 0; i < update_cnt; i++)
    {
        offset = old_dsnh_offset + i;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd_r, &dsnh));
        offset = new_dsnh_offset + i;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd_w, &dsnh));
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_process_dsnh(sys_nh_param_mcast_member_t *p_mem_param,
                                  sys_nh_mcast_meminfo_withrepli_t* p_member_info,
                                  sys_nh_db_dsl2editeth4w_t *p_dsl2edit_param,
                                  uint32 *p_dsnh_offset)
{
    sys_nh_param_dsnh_t dsnh_param;
    sys_vlan_info_t vlan_info;
    uint32 l2edit_offset = 0;

    kal_memset(&dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));

    if(p_mem_param->is_linkagg)
    {
        vlan_info.gport = CTC_MAP_TID_TO_GPORT(p_mem_param->destid);
    }
    else
    {
        uint8 gchip;
        CTC_ERROR_RETURN(sys_humber_get_gchip_id(p_mem_param->lchip, &gchip));
        vlan_info.gport = CTC_MAP_LPORT_TO_GPORT(gchip, p_mem_param->destid);
    }
    switch(p_mem_param->l3if_type)
    {
        case SYS_NH_PARAM_L3IF_ROUTED_PORT:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
            vlan_info.vid = 0;
            sys_humber_nh_get_ipmc_phyif_rsv_l2edit_offset( p_mem_param->lchip, &l2edit_offset);
            dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IPMC;
            break;
        case SYS_NH_PARAM_L3IF_VLAN_IF:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
            vlan_info.vid = p_mem_param->vid & 0xFFF;
            l2edit_offset  =  p_dsl2edit_param->hdr.offset;
            dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IPMC;
            break;
        case SYS_NH_PARAM_L3IF_SUB_IF:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
             vlan_info.vid = p_mem_param->vid & 0xFFF;
             l2edit_offset  =  p_dsl2edit_param->hdr.offset;
             dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IPMC;
             break;
        case SYS_NH_PARAM_L3IF_VLAN_PORT:
           {
               if (p_mem_param->member_type != SYS_NH_PARAM_BRGMC_MEM_UPMEP )
               {
                   ctc_vlan_egress_edit_info_t    vlan_egs_edit_info;
                   kal_memset(&vlan_egs_edit_info, 0, sizeof(ctc_vlan_egress_edit_info_t));

                   l2edit_offset  =  0;

                   vlan_egs_edit_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN;
                   vlan_egs_edit_info.output_svid =  p_mem_param->vid & 0xFFF;
                   CTC_SET_FLAG(vlan_egs_edit_info.edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID);
                   dsnh_param.p_vlan_info = &vlan_egs_edit_info;
                   dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_BRGUC;
                }
               else
                {
                     dsnh_param.dsnh_type = SYS_NH_PARAM_DSNH_TYPE_UPMEP;
                }

                dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_NONE;
                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
                vlan_info.vid = p_mem_param->vid & 0xFFF;
            }
             break;
        default:
            return CTC_E_L3IF_INVALID_IF_TYPE;
    }


    CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &dsnh_param.dest_vlan_ptr));
    dsnh_param.lchip = p_mem_param->lchip;
    dsnh_param.l2edit_ptr = l2edit_offset;

    if (p_member_info && p_member_info->free_dsnh_offset_cnt)
        /*Logical replicate*/
    {
        *p_dsnh_offset = p_member_info->comm.dsmet.dsnh_offset + p_member_info->comm.dsmet.replicate_num + 1;
        p_member_info->free_dsnh_offset_cnt--;
    }
    else if(p_member_info)
        /*Update dsnh offset*/
    {
        if ((p_member_info->comm.dsmet.replicate_num + \
            SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP) >= \
        SYS_HUMBER_NH_LOGICAL_REPLI_MAX_NUM)
        {
            return CTC_E_EXCEED_MAX_LOGICAL_REPLI_CNT;
        }
        else if ((p_member_info->comm.dsmet.replicate_num + \
            SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP) >= \
        SYS_HUMBER_NH_LOGICAL_REPLI_MAGIC_NUM)
        {
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc_with_multiple(p_mem_param->lchip,
              SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
              (p_member_info->comm.dsmet.replicate_num + 1 +\
              SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP),
              SYS_HUMBER_NH_LOGICAL_REPLI_MAGIC_NUM, p_dsnh_offset));
        }
        else
        {
            /*Allocate new dsnh*/
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(p_mem_param->lchip,
                SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
                (p_member_info->comm.dsmet.replicate_num + 1 +\
                SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP), p_dsnh_offset));
        }
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_repli_update_dsnh(p_mem_param->lchip,
            p_member_info->comm.dsmet.dsnh_offset, (*p_dsnh_offset),
            (p_member_info->comm.dsmet.replicate_num + 1)));
        /*Free old dsnh offset*/
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(p_mem_param->lchip,
            SYS_NH_ENTRY_TYPE_NEXTHOP_4W, (p_member_info->comm.dsmet.replicate_num + 1),
            (p_member_info->comm.dsmet.dsnh_offset)));
        p_member_info->comm.dsmet.dsnh_offset = *p_dsnh_offset;
        *p_dsnh_offset = (*p_dsnh_offset) + p_member_info->comm.dsmet.replicate_num + 1;/*Current used dsnh offset*/
        p_member_info->free_dsnh_offset_cnt = SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP -1;
    }
    else
    /*Physical replicate member*/
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(p_mem_param->lchip,
                                                    SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
                                                    1, p_dsnh_offset));
    }
    dsnh_param.dsnh_offset = *p_dsnh_offset;

    CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_write_dsmet(uint8 lchip, sys_nh_info_dsmet_t *p_met_info)
{
    ds_met_entry_t dsmet;
    sys_nh_info_com_t* p_nhinfo = NULL;
    int32 ret = CTC_E_NONE;

    SYS_NH_DBG_FUNC();

    SYS_NH_DBG_INFO("\n\
        lchip = %d\n\
        dsmet_offset = 0x%x,\n\
        dsnh_offset = 0x%x,\n\
        is_linkagg = %d,\n\
        member_type = %d, \n\
        replicate_num = %d, \n\
        ucastid = %d, \n\
        next_met_offset = 0x%x, \n\
        port_check_discard = %d\n",
        lchip,
        p_met_info->dsmet_offset,
        p_met_info->dsnh_offset,
        p_met_info->is_linkagg,
        p_met_info->member_type,
        p_met_info->replicate_num,
        p_met_info->ucastid,
        p_met_info->next_dsmet_offset,
        p_met_info->port_check_discard);

    if( (p_met_info->ucastid & 0xFF) > SYS_STATIC_INT_PORT_END && !(p_met_info->is_linkagg))
    {
        p_met_info->ucastid = (SYS_QSEL_TYPE_INTERNAL_PORT << 12) | p_met_info->ucastid;
    }

    ret = sys_humber_nh_get_nhinfo_by_nhid(p_met_info->ref_nhid, &p_nhinfo);
    if(ret == CTC_E_NONE)
    {
        if(p_nhinfo->hdr.nh_entry_flags & SYS_NH_INFO_BRGUC_FLAG_SERVICE_QUEUE)
        {
            p_met_info->ucastid = (SYS_QSEL_TYPE_SERVICE << 12) | p_met_info->ucastid;
        }
    }

    kal_memset(&dsmet, 0, sizeof(ds_met_entry_t));
    dsmet.next_met_entry_ptr = p_met_info->next_dsmet_offset;
    dsmet.nexthop_ext = p_met_info->use_dsnh8w;
    dsmet.is_link_aggregation = p_met_info->is_linkagg;
    dsmet.ucast_id_lower = p_met_info->ucastid & 0xFFF;
    dsmet.ucast_id_upper = (p_met_info->ucastid >> 12) & 0xF;
    dsmet.end_local_rep = p_met_info->end_local;
    dsmet.remote_bay = (SYS_NH_PARAM_MCAST_MEM_REMOTE == p_met_info->member_type) ? \
        SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE;

    dsmet.port_check_discard = p_met_info->port_check_discard;
    dsmet.aps_brg_en = (SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE == p_met_info->member_type) ?\
        SYS_HUMBER_NH_BOOL_TRUE : SYS_HUMBER_NH_BOOL_FALSE;
    dsmet.aps_brg_protect_path = p_met_info->aps_protection_path;

    if (p_met_info->replicate_num >= SYS_HUMBER_NH_LOGICAL_REPLI_MAGIC_NUM)
    {
        dsmet.replication_ctl = (1 << SYS_HUMBER_NH_LOGICAL_RELI_CTL_BIT) | \
            ( (p_met_info->dsnh_offset >> 6) << 12) | (p_met_info->replicate_num);
    }
    else
    {
        dsmet.replication_ctl = (p_met_info->dsnh_offset << 6) | (p_met_info->replicate_num);
    }

    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(lchip, SYS_NH_ENTRY_TYPE_MET,
                                                     p_met_info->dsmet_offset, &dsmet));


    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_process_dsmet_repli(uint8 lchip,
                                         sys_nh_mcast_meminfo_t *p_mem_info,
                                         sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info,
                                         bool add_mem)
{
    sys_nh_mcast_meminfo_withrepli_t *p_mem_repli = NULL;
    sys_nh_mcast_repli_node_t *p_repli;

    p_mem_repli = (sys_nh_mcast_meminfo_withrepli_t *)p_mem_info;
    if(add_mem)
    {
        /*Create a new replicate node*/
        p_repli = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_mcast_repli_node_t));
        if (NULL == p_repli)
        {
            return CTC_E_NO_MEMORY;
        }
        p_repli->p_dsl2edit_info = p_dsl2edit_info;
        ctc_list_pointer_insert_tail(&(p_mem_repli->repli_list), &(p_repli->list_head));
        p_mem_repli->comm.dsmet.replicate_num++;
    }
    else
    {
        p_repli = _ctc_container_of(ctc_list_pointer_node_tail(&(p_mem_repli->repli_list)),
            sys_nh_mcast_repli_node_t, list_head);
        ctc_list_pointer_delete_tail((&p_mem_repli->repli_list));
        p_mem_repli->comm.dsmet.replicate_num--;
        mem_free(p_repli);
    }
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(lchip, &(p_mem_repli->comm.dsmet)));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_build_dsmet_info(sys_nh_mcast_dsmet_io_t *p_dsmet_io,
                                      sys_nh_info_dsmet_t *p_dsmet)
{
    p_dsmet->ucastid = p_dsmet_io->p_mem_param->destid;
    p_dsmet->replicate_num = 0;
    p_dsmet->dsnh_offset = p_dsmet_io->dsnh_offset,
    p_dsmet->dsmet_offset = p_dsmet_io->met_offset;
    p_dsmet->member_type = p_dsmet_io->p_mem_param->member_type;
    p_dsmet->is_linkagg = p_dsmet_io->p_mem_param->is_linkagg;
    p_dsmet->aps_protection_path = p_dsmet_io->p_mem_param->is_protection_path;
    p_dsmet->ref_nhid = p_dsmet_io->p_mem_param->ref_nhid;
    p_dsmet->vid      = p_dsmet_io->p_mem_param->vid;

    switch(p_dsmet_io->p_mem_param->member_type)
    {
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
        case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
        case SYS_NH_PARAM_BRGMC_MEM_RAPS:
        case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
            p_dsmet->port_check_discard = TRUE;
            /*No need break here*/
        case SYS_NH_PARAM_IPMC_MEM_LOCAL:
            if(p_dsmet_io->p_next_mem)
            {
                p_dsmet->next_dsmet_offset = p_dsmet_io->p_next_mem->dsmet.dsmet_offset;
                if(SYS_NH_PARAM_MCAST_MEM_REMOTE != p_dsmet_io->p_next_mem->dsmet.member_type)
                {
                    p_dsmet->end_local = SYS_HUMBER_NH_BOOL_FALSE;
                }
                else
                {
                    p_dsmet->end_local = SYS_HUMBER_NH_BOOL_TRUE;
                }
            }
            else
            {
                p_dsmet->end_local = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsmet->next_dsmet_offset = SYS_HUMBER_NH_INVALID_OFFSET;
            }
            break;
        case SYS_NH_PARAM_MCAST_MEM_REMOTE:
            p_dsmet->end_local = SYS_HUMBER_NH_BOOL_FALSE;
            p_dsmet->port_check_discard = FALSE;
            if(p_dsmet_io->p_next_mem)
            {
                p_dsmet->next_dsmet_offset = p_dsmet_io->p_next_mem->dsmet.dsmet_offset;
            }
            else
            {
                p_dsmet->next_dsmet_offset = SYS_HUMBER_NH_INVALID_OFFSET;
            }
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    
    if (p_dsmet_io->p_mem_param->member_type == SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS
        || p_dsmet_io->p_mem_param->member_type == SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH)
    {
        p_dsmet->port_check_discard = p_dsmet_io->p_mem_param->port_check_discard;
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_allocate_member_info(sys_nh_param_mcast_member_type_t type,
                                          sys_nh_db_dsl2editeth4w_t* p_dsl2edit_info,
                                          sys_nh_mcast_meminfo_t **pp_mem_com)
{
    sys_nh_mcast_meminfo_norepli_t *p_mem_norepli;
    sys_nh_mcast_meminfo_withrepli_t *p_mem_withrepli;
    sys_nh_mcast_repli_node_t *p_repli_node;

    switch(type)
    {
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
        case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
        case SYS_NH_PARAM_BRGMC_MEM_RAPS:
        case SYS_NH_PARAM_MCAST_MEM_REMOTE:
            p_mem_norepli = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_mcast_meminfo_norepli_t));
            if(NULL == p_mem_norepli)
            {
                return CTC_E_NO_MEMORY;
            }
            else
            {
              kal_memset(p_mem_norepli, 0, sizeof(sys_nh_mcast_meminfo_norepli_t));
            }
            p_mem_norepli->comm.dsmet.member_type = type;
            *pp_mem_com = &(p_mem_norepli->comm);
            break;
        case SYS_NH_PARAM_IPMC_MEM_LOCAL:
         case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
            p_mem_withrepli = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_mcast_meminfo_withrepli_t));
            if(NULL == p_mem_withrepli)
            {
                return CTC_E_NO_MEMORY;
            }
            else
            {
                kal_memset(p_mem_withrepli, 0, sizeof(sys_nh_mcast_meminfo_withrepli_t));
            }
            p_repli_node = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_nh_mcast_repli_node_t));
            if(NULL == p_repli_node)
            {
                mem_free(p_mem_withrepli);
                return CTC_E_NO_MEMORY;
            }
            else
            {
                kal_memset(p_repli_node, 0, sizeof(sys_nh_mcast_repli_node_t));
            }
            p_mem_withrepli->comm.dsmet.member_type = type;
            ctc_list_pointer_init(&(p_mem_withrepli->repli_list));
            p_repli_node->p_dsl2edit_info = p_dsl2edit_info;
            ctc_list_pointer_insert_tail(&(p_mem_withrepli->repli_list), (&p_repli_node->list_head));
            *pp_mem_com = &(p_mem_withrepli->comm);
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_free_member_info(sys_nh_mcast_meminfo_t *p_mem_com,
                                          sys_nh_mcast_repli_node_t *p_repli_node)
{
    sys_nh_mcast_meminfo_norepli_t *p_mem_norepli;
    sys_nh_mcast_meminfo_withrepli_t *p_mem_withrepli;

    switch(p_mem_com->dsmet.member_type)
    {
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
        case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
        case SYS_NH_PARAM_BRGMC_MEM_RAPS:
        case SYS_NH_PARAM_MCAST_MEM_REMOTE:
            if (NULL != (p_mem_norepli = (sys_nh_mcast_meminfo_norepli_t*)p_mem_com))
            {
                mem_free(p_mem_norepli);
            }
            break;
        case SYS_NH_PARAM_IPMC_MEM_LOCAL:
          case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
            if (p_repli_node)
            {
                mem_free(p_repli_node);
            }
            if (NULL != (p_mem_withrepli = (sys_nh_mcast_meminfo_withrepli_t*)p_mem_com))
            {
                mem_free(p_mem_withrepli);
            }
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


static INLINE int32
_sys_humber_nh_mcast_add_dsmet_norepli(sys_nh_param_mcast_member_t *p_mem_param,
                                          uint32 basic_met_offset, uint32 dsnh_offset,
                                          sys_nh_mcast_meminfo_t *p_mem_info,
                                          ctc_list_pointer_t *p_db_member_list)
{
    sys_nh_mcast_meminfo_t *p_mem_flex;
    uint32 new_met_offset;
    sys_nh_mcast_dsmet_io_t dsmet_io;

    /*1. Init param*/
    kal_memset(&dsmet_io, 0, sizeof(sys_nh_mcast_dsmet_io_t));
    dsmet_io.p_mem_param = p_mem_param;

    /*2. Build new allocate member, and update flex member(prev member or next member)*/
    /*2.1 New member is the list's first member*/
    if(ctc_list_pointer_empty(p_db_member_list))
    {
        dsmet_io.met_offset = basic_met_offset;
        dsmet_io.dsnh_offset = dsnh_offset;
        dsmet_io.p_next_mem = NULL;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_build_dsmet_info(&dsmet_io, &(p_mem_info->dsmet)));
        /*Add this new member to db member list*/
        ctc_list_pointer_insert_head(p_db_member_list, &(p_mem_info->list_head));
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(p_mem_param->lchip,
            &(p_mem_info->dsmet)));
        return CTC_E_NONE;
    }

    /*2.2 New member is not the list's first member*/
    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(p_mem_param->lchip,
                                                SYS_NH_ENTRY_TYPE_MET, 1, &new_met_offset));

    if(SYS_NH_PARAM_MCAST_MEM_REMOTE  == p_mem_param->member_type)
    /*Remote Entry*/
    {
        /*(1) insert new member into tail*/
        dsmet_io.met_offset = new_met_offset;
        dsmet_io.dsnh_offset = dsnh_offset;/*Remote entry's dsnh offset should be 0*/
        dsmet_io.p_next_mem = NULL;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_build_dsmet_info(&dsmet_io, &(p_mem_info->dsmet)));
        /*The list have one member at least, so the tail node should not be NULL*/
        p_mem_flex = _ctc_container_of(ctc_list_pointer_node_tail(p_db_member_list),
            sys_nh_mcast_meminfo_t, list_head);
        /*Add this new member to db member list*/
        ctc_list_pointer_insert_tail(p_db_member_list, &(p_mem_info->list_head));
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(p_mem_param->lchip,
            &(p_mem_info->dsmet)));

        /*(2) update previous member's next met offset*/
        p_mem_flex->dsmet.next_dsmet_offset = p_mem_info->dsmet.dsmet_offset;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(p_mem_param->lchip,
            &(p_mem_flex->dsmet)));
    }
    else
    /*Local Entry, insert to head*/
    {
        /*(1) Move original head to secondary member position*/
        p_mem_flex = _ctc_container_of(ctc_list_pointer_head(p_db_member_list),
            sys_nh_mcast_meminfo_t, list_head);
        p_mem_flex->dsmet.dsmet_offset = new_met_offset;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(p_mem_param->lchip,
            &(p_mem_flex->dsmet)));

        /*(2)Insert new member into list head*/
        dsmet_io.met_offset = basic_met_offset;
        dsmet_io.dsnh_offset = dsnh_offset;
        dsmet_io.p_next_mem = p_mem_flex;
        _sys_humber_nh_mcast_build_dsmet_info(&dsmet_io, &(p_mem_info->dsmet));
        ctc_list_pointer_insert_head(p_db_member_list, &(p_mem_info->list_head));
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(p_mem_param->lchip,
            &(p_mem_info->dsmet)));
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_del_dsmet_norepli(uint8 lchip, sys_nh_mcast_meminfo_t *p_mem_info,
                                          ctc_list_pointer_t *p_db_member_list)
{
    sys_nh_mcast_meminfo_t *p_mem_target, *p_mem_flex;

    /*1. Init param*/
    p_mem_target = _ctc_container_of(ctc_list_pointer_head(p_db_member_list),
        sys_nh_mcast_meminfo_t, list_head);

    /*2. Remove member*/
    if(ctc_list_pointer_head(p_db_member_list) == (&(p_mem_info->list_head)))
    /*Target member is first member*/
    {
        if(p_mem_target->list_head.p_next)
        {
            /*Get next member*/
            p_mem_flex = _ctc_container_of(p_mem_target->list_head.p_next,
                sys_nh_mcast_meminfo_t, list_head);
            CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_MET,
                1, p_mem_flex->dsmet.dsmet_offset));

            p_mem_flex->dsmet.dsmet_offset = p_mem_target->dsmet.dsmet_offset;
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(lchip, &(p_mem_flex->dsmet)));
        }
        else
        /*This is the last member of this group in this chip, deinit basic met entry*/
        {
            /*Free basic met offset*/
            CTC_ERROR_RETURN(sys_humber_nh_global_dync_entry_set_default(lchip,
                                                                         p_mem_target->dsmet.dsmet_offset,
                                                                         p_mem_target->dsmet.dsmet_offset));

        }
    }
    else
    {
        p_mem_flex = _ctc_container_of(p_mem_info->list_head.p_prev,
            sys_nh_mcast_meminfo_t, list_head);
        if(SYS_NH_PARAM_MCAST_MEM_REMOTE != p_mem_info->dsmet.member_type)
        {
            p_mem_flex->dsmet.end_local = p_mem_info->dsmet.end_local;
        }
        p_mem_flex->dsmet.next_dsmet_offset = p_mem_info->dsmet.next_dsmet_offset;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_write_dsmet(lchip, &(p_mem_flex->dsmet)));
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_MET,
            1, p_mem_info->dsmet.dsmet_offset));
    }
    ctc_list_pointer_delete(p_db_member_list, &(p_mem_info->list_head));

    return CTC_E_NONE;
}


static INLINE int32
_sys_humber_nh_mcast_get_nh_and_edit_data(sys_nh_param_mcast_member_t *p_mem_param,
                                                sys_nh_mcast_meminfo_t *p_member_info,
                                                uint32 *p_dsnh_offset,uint8 *p_use_dsnh8w,
                                                sys_nh_db_dsl2editeth4w_t **pp_dsl2edit_param)
{

     bool is_remote_met_use_nexthop ;
     *p_use_dsnh8w = 0;

    switch(p_mem_param->member_type)
    {
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
            SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH, p_dsnh_offset));
            *pp_dsl2edit_param = NULL;
            SYS_NH_DBG_INFO("Get Reserved bridge dsNexthop offset = 0x%x\n", *p_dsnh_offset);
            break;
    case SYS_NH_PARAM_BRGMC_MEM_UPMEP:

            /*2. Op DsNh*/
            p_mem_param->l3if_type = SYS_NH_PARAM_L3IF_VLAN_PORT;
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_process_dsnh(p_mem_param,
                                                               (sys_nh_mcast_meminfo_withrepli_t*)p_member_info,
                                                               *pp_dsl2edit_param, p_dsnh_offset));
            break;
        case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:

            CTC_ERROR_RETURN(sys_humber_nh_get_dsnh_offset_by_nhid(
            p_mem_param->ref_nhid, p_dsnh_offset,p_use_dsnh8w));
            *pp_dsl2edit_param = NULL;
            SYS_NH_DBG_INFO("Get egress vlan translation NHID's dsNexthop offset = 0x%x\n", *p_dsnh_offset);
            break;

        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
            CTC_ERROR_RETURN(sys_humber_nh_get_dsnh_offset_by_nhid(
            p_mem_param->ref_nhid, p_dsnh_offset,p_use_dsnh8w));
            if(p_mem_param->is_protection_path)
            {
                (*p_dsnh_offset) += 1;
            }
            *pp_dsl2edit_param = NULL;
            SYS_NH_DBG_INFO("Get APS NHID's dsNexthop offset = 0x%x, %s\n",
                *p_dsnh_offset, (p_mem_param->is_protection_path) ? "Is protection path" : " Is working path");
            break;
        case SYS_NH_PARAM_IPMC_MEM_LOCAL:
            if(p_mem_param->l3if_type != SYS_NH_PARAM_L3IF_VLAN_PORT )
             {
            /*1. Process L2Edit*/
                CTC_ERROR_RETURN(_sys_humber_nh_mcast_process_l2edit(p_mem_param, pp_dsl2edit_param));
            }
            /*2. Op DsNh*/
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_process_dsnh(p_mem_param,
                                                               (sys_nh_mcast_meminfo_withrepli_t*)p_member_info,
                                                               *pp_dsl2edit_param, p_dsnh_offset));
           break;
        case SYS_NH_PARAM_MCAST_MEM_REMOTE:
            is_remote_met_use_nexthop = sys_humber_is_remote_met_use_nexthop();
            if (is_remote_met_use_nexthop)
            {
                if (p_mem_param->ref_nhid != 0)
                {
                    CTC_ERROR_RETURN(sys_humber_nh_get_dsnh_offset_by_nhid(
                    p_mem_param->ref_nhid, p_dsnh_offset, p_use_dsnh8w));

                }
                else
                {
                    CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                    SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH, p_dsnh_offset));
                }
            }
            else
            {
                *p_dsnh_offset = 0;
            }
            *pp_dsl2edit_param = NULL;
            SYS_NH_DBG_INFO("Get Reserved bridge dsNexthop offset = 0x%x\n", *p_dsnh_offset);
            break;
        case SYS_NH_PARAM_BRGMC_MEM_RAPS:
            *p_dsnh_offset = 0;
            *pp_dsl2edit_param = NULL;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_mcast_free_nh_and_edit_data(uint8 lchip,
                                           sys_nh_mcast_meminfo_t *p_member_info,
                                           sys_nh_mcast_repli_node_t *p_repli,
                                           uint32 del_pos)
{
    sys_nh_mcast_meminfo_withrepli_t *p_mem_withrepli;
    uint32 dsnh_offset_del, dsnh_offset_last;
    sys_nh_mcast_repli_node_t *p_repli_last;
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_del;

    if(NULL == p_repli || NULL == p_member_info)
    {
        return CTC_E_INVALID_PARAM;
    }
    p_mem_withrepli = (sys_nh_mcast_meminfo_withrepli_t *)p_member_info;
    p_dsl2edit_del = p_repli->p_dsl2edit_info;

    if(p_mem_withrepli->comm.dsmet.replicate_num > 0)
    /*Remove none last member*/
    {
        p_repli_last = _ctc_container_of(ctc_list_pointer_node_tail(&(p_mem_withrepli->repli_list)),
                                         sys_nh_mcast_repli_node_t, list_head);
        dsnh_offset_last = (p_mem_withrepli->comm.dsmet.dsnh_offset + \
            p_mem_withrepli->comm.dsmet.replicate_num);
        dsnh_offset_del = p_mem_withrepli->comm.dsmet.dsnh_offset + del_pos;
        if(ctc_list_pointer_node_tail(&(p_mem_withrepli->repli_list)) != &(p_repli->list_head))
        /*Repli node to be removed isn't at last, should update repli_node by the last repli_node*/
        {
            p_repli->p_dsl2edit_info = p_repli_last->p_dsl2edit_info;
            /*Update dsnh table*/
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_repli_update_dsnh(lchip, dsnh_offset_last, dsnh_offset_del, 1));
        }

        p_mem_withrepli->free_dsnh_offset_cnt++;
        if(SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP == p_mem_withrepli->free_dsnh_offset_cnt)
        {
            p_mem_withrepli->free_dsnh_offset_cnt = 0;
            CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,
                                                       SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
                                                       SYS_HUMBER_NH_MCAST_REPLI_INCR_STEP,
                                                       dsnh_offset_last));
        }
    }
    else if(0 == p_mem_withrepli->comm.dsmet.replicate_num )
    /*Remove last member*/
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,
                                                   SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
                                                   1,
                                                   p_member_info->dsmet.dsnh_offset));
    }
    else
        return CTC_E_UNEXPECT;

    if(p_dsl2edit_del)
    {
        CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry((sys_nh_db_com_entry_t*)p_dsl2edit_del));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_mcast_add_member(uint32 basic_met_offset,
                               sys_nh_param_mcast_member_t *p_mem_param,
                               ctc_list_pointer_t *p_db_member_list,
                               sys_nh_info_mcast_t *p_mcast_db)
{
    sys_nh_mcast_meminfo_t *p_member_info= NULL;
    sys_nh_mcast_repli_node_t   *p_repli_info = NULL;
    bool entry_exist = FALSE;
    uint8 use_dsnh8w = 0;
    uint32 dsnh_offset, repli_pos;
    sys_nh_db_dsl2editeth4w_t *p_dsl2edit_param = NULL;
    int32 ret;

    /*1. Check if mcast member have been existed*/
    CTC_PTR_VALID_CHECK(p_mem_param);
    if(p_mem_param->is_linkagg &&
        (p_mem_param->destid >= CTC_MAX_LINKAGG_GROUP_NUM))
    {
        return CTC_E_INVALID_TID;
    }
    else if(SYS_NH_PARAM_MCAST_MEM_REMOTE ==
        p_mem_param->member_type)
    {
        CTC_GLOBAL_CHIPID_CHECK(p_mem_param->destid);
    }
    else if(  SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE ==
    p_mem_param->member_type)
    {
         CTC_APS_GROUP_ID_CHECK(p_mem_param->destid);
    }
    else if(p_mem_param->destid >= MAX_PORT_NUM_PER_CHIP)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_analyze_member(p_db_member_list,
                                                        p_mem_param, &p_member_info,
                                                        &p_repli_info, &entry_exist, &repli_pos));
    if (entry_exist)
    {
        return CTC_E_NONE;
    }

    /*3. Op DsL2Edit & DsNH*/
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_get_nh_and_edit_data(
        p_mem_param, p_member_info, &dsnh_offset,&use_dsnh8w,&p_dsl2edit_param));

    if (CTC_FLAG_ISSET(p_mcast_db->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_UP_MEP))
    {
        dsnh_offset = p_mcast_db->dsnh_offset;
    }
    /*3. Op DsMet*/
    if (p_member_info)
    {
        p_member_info->dsmet.use_dsnh8w = use_dsnh8w;
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_process_dsmet_repli(p_mem_param->lchip,
                                                                  p_member_info,
                                                                  p_dsl2edit_param, TRUE));
    }
    else
    {
        /*Create new member node*/
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_allocate_member_info
                        (p_mem_param->member_type, p_dsl2edit_param, &p_member_info));


        p_member_info->dsmet.use_dsnh8w = use_dsnh8w;
        ret = _sys_humber_nh_mcast_add_dsmet_norepli(p_mem_param,
                         basic_met_offset, dsnh_offset, p_member_info,p_db_member_list);
        if(ret < 0)
        {
            p_repli_info = NULL;
            if(SYS_NH_PARAM_IPMC_MEM_LOCAL == p_member_info->dsmet.member_type
                || SYS_NH_PARAM_BRGMC_MEM_UPMEP == p_member_info->dsmet.member_type)
            {
                p_repli_info = (sys_nh_mcast_repli_node_t *)ctc_list_pointer_node_tail
                    (&(((sys_nh_mcast_meminfo_withrepli_t *)p_member_info)->repli_list));
            }
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_free_member_info(p_member_info, p_repli_info));
            return ret;
        }
    }
    return CTC_E_NONE;
}


int32
sys_humber_nh_mcast_remove_member(sys_nh_param_mcast_member_t *p_mem_param,
                                  ctc_list_pointer_t *p_db_member_list)
{
    sys_nh_mcast_meminfo_t *p_member_info= NULL;
    sys_nh_mcast_repli_node_t   *p_repli_info = NULL;
    bool entry_exist = FALSE;
    uint32 repli_pos;

    /*1. Check if mcast member have been existed*/
    if(p_mem_param->is_linkagg &&
        (p_mem_param->destid > CTC_MAX_LINKAGG_GROUP_NUM))
    {
        return CTC_E_INVALID_TID;
    }
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_analyze_member(p_db_member_list,
                                        p_mem_param, &p_member_info, &p_repli_info, &entry_exist, &repli_pos));
    if (!entry_exist || NULL == p_member_info)
    {
        return CTC_E_MEMBER_NOT_EXIST;
    }

    /*3. Op DsMet*/
     if ((SYS_NH_PARAM_IPMC_MEM_LOCAL == p_mem_param->member_type ||
            SYS_NH_PARAM_BRGMC_MEM_UPMEP == p_mem_param->member_type) &&
            (NULL == p_repli_info))
    {
        ctc_list_pointer_node_t *p_node;
        p_node = ctc_list_pointer_head(&(((sys_nh_mcast_meminfo_withrepli_t*)p_member_info)->repli_list));
        p_repli_info = _ctc_container_of(p_node, sys_nh_mcast_repli_node_t, list_head);
    }

    if (p_repli_info)
    /*Only IPMC member has replicate info;
    brgmc not have repli_info;
    only IPMC's member need free dsnh and dsl2edit;
    brgmc(noraml brgmc member, reference nh member) no need to free dsnh and dsl2edit*/
    {
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_free_nh_and_edit_data(p_mem_param->lchip,
        p_member_info, p_repli_info, repli_pos));
    }

    if (p_member_info->dsmet.replicate_num > 0)
    /*Logical replication case*/
    {
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_process_dsmet_repli(p_mem_param->lchip,
                                                                  p_member_info, NULL, FALSE));
    }
    else
    {
        /*Remove member node*/
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_del_dsmet_norepli(p_mem_param->lchip,
                                                                p_member_info,
                                                                p_db_member_list));
        CTC_ERROR_RETURN(_sys_humber_nh_mcast_free_member_info(p_member_info,
                                                               p_repli_info));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_nh_update_mcast_member_by_vlan(uint16 vlan_id,
                                                              sys_nh_info_mcast_t *p_mcast_db,
                                                              bool is_add)
{
    sys_nh_mcast_meminfo_t *p_meminfo = NULL;
    ctc_list_pointer_node_t *p_pos_mem = NULL;
    uint32 cur_met_offset   = 0;
    uint32 cmd = 0;
    ds_met_entry_t dsmet;
    uint8 lchip, curr_chip_num;
    uint32 dsnh_offset = 0;
    ctc_list_pointer_t *p_mem_list = NULL;

    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        p_mem_list = &(p_mcast_db->meminfo[lchip].mem_list);

        if (ctc_list_pointer_empty(p_mem_list))
        {
            continue;
        }

        if (is_add)
        {
            CTC_ERROR_RETURN(sys_humber_nh_get_dsnh_offset_by_vlan(vlan_id, lchip, &dsnh_offset));
        }
        else
        {

            CTC_ERROR_RETURN(sys_humber_nh_get_resolved_offset(
                                               SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH, &dsnh_offset));
        }

        CTC_LIST_POINTER_LOOP(p_pos_mem, p_mem_list)
        {
            p_meminfo = _ctc_container_of(p_pos_mem, sys_nh_mcast_meminfo_t, list_head);

            cur_met_offset = p_meminfo->dsmet.dsmet_offset;

            cmd = DRV_IOR(IOC_TABLE, DS_MET_ENTRY, DRV_ENTRY_FLAG);
            kal_memset(&dsmet, lchip, sizeof(ds_met_entry_t));
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, cur_met_offset, cmd, &dsmet));

            dsmet.replication_ctl = (dsnh_offset << 6) ;
            p_meminfo->dsmet.dsnh_offset = dsnh_offset;

            cmd = DRV_IOW(IOC_TABLE, DS_MET_ENTRY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, cur_met_offset, cmd, &dsmet));

            if (p_meminfo->dsmet.end_local)
            {
                break;
            }
        }
    }

    return CTC_E_NONE;
}

/**
 @brief This function is used to update member in multicast bridge nexthop

 @param[in] p_db_brgmc, pointer of multicast nexthop

 @param[in] ref_nhid, reference nexthop id

 @return CTC_E_XXX
 */
static int32
_sys_humber_nh_update_mcast_member_by_nhid(sys_nh_param_mcast_member_t *p_mem_param,
                                  ctc_list_pointer_t *p_db_member_list)
{
    sys_nh_mcast_meminfo_t *p_member_info= NULL;
    sys_nh_mcast_repli_node_t   *p_repli_info = NULL;
    bool entry_exist = FALSE;
    uint8 use_dsnh8w = 0;
    uint32 dsnh_offset, repli_pos, cmd;
    sys_nh_db_dsl2editeth4w_t *p_dsl2edit_param = NULL;

    switch(p_mem_param->member_type)
    {
        case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
        case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
            break;
        default:
            return CTC_E_INVALID_NH_SUB_TYPE;
    }

    /*1. Check if mcast member have been existed*/
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_analyze_member(p_db_member_list,
                                                        p_mem_param, &p_member_info,
                                                        &p_repli_info, &entry_exist, &repli_pos));
    if (!entry_exist)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }
    CTC_ERROR_RETURN(_sys_humber_nh_mcast_get_nh_and_edit_data(p_mem_param,
        p_member_info, &dsnh_offset, &use_dsnh8w,&p_dsl2edit_param));

    cmd = DRV_IOR(IOC_TABLE, DS_MET_ENTRY, DS_MET_ENTRY_REPLICATION_CTL);
    CTC_ERROR_RETURN(drv_tbl_ioctl(p_mem_param->lchip, p_member_info->dsmet.dsmet_offset, cmd, &dsnh_offset));
    p_member_info->dsmet.member_type = p_mem_param->member_type;

    return CTC_E_NONE;
};


/**
 @brief Callback function to create multicast bridge nexthop

 @param[in] p_com_nh_para, parameters used to create bridge nexthop

 @param[out] p_com_db, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_create_mcast_cb(sys_nh_param_com_t* p_com_nh_para,
    sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_mcast_t * p_nh_para_mcast;
    sys_nh_info_mcast_t *p_mcast_db;
    uint32 dsfwd_offset;
    uint8 lchip, curr_chip_num;
    sys_nh_param_dsfwd_t dsfwd_param;
    SYS_NH_DBG_FUNC();

    /* 1. sanity check & init */
    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MCAST, p_com_nh_para->hdr.nh_param_type);
    p_nh_para_mcast = (sys_nh_param_mcast_t *)p_com_nh_para;
    p_mcast_db = (sys_nh_info_mcast_t *)p_com_db;
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    SYS_NH_DBG_INFO("nhType = %d, NHID = %d, isInternaNH = %d, \n\
        GroupId = %d, Opcode = %d\n",
        p_nh_para_mcast->hdr.nh_param_type,
        p_nh_para_mcast->hdr.nhid,
        p_nh_para_mcast->hdr.is_internal_nh,
        p_nh_para_mcast->groupid,
        p_nh_para_mcast->opcode);

    /*2. Malloc and init DB structure*/
    curr_chip_num = sys_humber_get_local_chip_num();
    p_mcast_db->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_MCAST;
    p_mcast_db->basic_met_offset = p_nh_para_mcast->groupid;
    CTC_ERROR_RETURN(sys_humber_nh_check_glb_met_sram_offset(p_mcast_db->basic_met_offset, 1, TRUE));

    if(SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_GRP == p_nh_para_mcast->opcode)
    {
        CTC_SET_FLAG(p_mcast_db->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_SPECIAL);
    }

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        ctc_list_pointer_init(&(p_mcast_db->meminfo[lchip].mem_list));
        {
            dsfwd_param.drop_pkt = FALSE;
            dsfwd_param.dest_id = 0xFFFF & (p_mcast_db->basic_met_offset);
            dsfwd_param.is_mcast = TRUE;

            /*Init basic dsmet entry*/
            CTC_ERROR_RETURN(sys_humber_nh_global_dync_entry_set_default(lchip,
                                                                         p_mcast_db->basic_met_offset,
                                                                         p_mcast_db->basic_met_offset));
        }

        if (SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_GRP != p_nh_para_mcast->opcode)
        {
            /*Build DsFwd Table and write table*/
            CTC_ERROR_RETURN(sys_humber_nh_reverse_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1, &dsfwd_offset));
            p_mcast_db->hdr.dsfwd_info[lchip].dsfwd_offset = dsfwd_offset;
            /*Writeback dsfwd offset*/
            p_nh_para_mcast->hdr.dsfwd_offset[lchip] = dsfwd_offset;
            dsfwd_param.p_dsfwd_info = &(p_mcast_db->hdr.dsfwd_info[lchip]);
            dsfwd_param.lchip = lchip;
            dsfwd_param.dsnh_offset = 0;/*This value should be 0 for multicast,
                                      this nexthopPtr will be stored in humberHeader,
                                      For cross - chip case, this nexthopPtr will send from
                                      local chip to remote chip*/


             /*get stats ptr form stats module*/
              if (p_com_nh_para->hdr.stats_valid &&
                  CTC_E_NONE == sys_humber_stats_create_statsptr( lchip, 1, &p_nh_para_mcast->hdr.stats_ptr[lchip]))
              {
                  p_mcast_db->stats_ptr[lchip] = p_nh_para_mcast->hdr.stats_ptr[lchip];
                  dsfwd_param.stats_ptr = p_nh_para_mcast->hdr.stats_ptr[lchip];
                  dsfwd_param.stats_valid = p_com_nh_para->hdr.stats_valid;
              }
            /*Write table*/
            CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
        }
    }
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_met_sram_offset(p_mcast_db->basic_met_offset, 1, TRUE));
    return CTC_E_NONE;
}

/**
 @brief Callback function to delete multicast bridge nexthop

 @param[in] p_data, pointer used to store nexthop data

 @return CTC_E_XXX
 */
int32
sys_humber_nh_delete_mcast_cb(sys_nh_info_com_t* p_data)
{
    sys_nh_info_mcast_t *p_mcast_db;
    uint8 lchip, curr_chip_num;
    ctc_list_pointer_node_t *p_pos, *p_pos_next, *p_pos_repli, *p_pos_next_repli;
    sys_nh_mcast_meminfo_t *p_member;
    sys_nh_param_dsfwd_t dsfwd_param;
    sys_nh_mcast_meminfo_withrepli_t *p_member_withrepli;
    sys_nh_mcast_repli_node_t *p_repli;
    bool first_node = TRUE;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MCAST, p_data->hdr.nh_entry_type);
    p_mcast_db = (sys_nh_info_mcast_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();
    /*Free dsfwd offset*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        first_node = TRUE;
        CTC_LIST_POINTER_LOOP_DEL(p_pos, p_pos_next, &(p_mcast_db->meminfo[lchip].mem_list))
        {
            p_member = _ctc_container_of(p_pos, sys_nh_mcast_meminfo_t, list_head);
            switch(p_member->dsmet.member_type)
            {
             case SYS_NH_PARAM_IPMC_MEM_LOCAL:
             case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
                    /*1. Free Dsl2Edit*/
                    p_member_withrepli = (sys_nh_mcast_meminfo_withrepli_t*)(p_member);
                    CTC_LIST_POINTER_LOOP_DEL(p_pos_repli, p_pos_next_repli, &(p_member_withrepli->repli_list))
                    {
                        p_repli = _ctc_container_of(p_pos_repli, sys_nh_mcast_repli_node_t, list_head);
                        CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry(
                            (sys_nh_db_com_entry_t*)(p_repli->p_dsl2edit_info)));
                        mem_free(p_repli);
                    }

                    /*2. Free DsNH*/
                    CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip,
                                                               SYS_NH_ENTRY_TYPE_NEXTHOP_4W,
                                                               (p_member_withrepli->comm.dsmet.replicate_num + 1 +
                                                               p_member_withrepli->free_dsnh_offset_cnt),
                                                               p_member_withrepli->comm.dsmet.dsnh_offset));
                    break;
                default:
                    break;
            }
            if(first_node)
            {
                first_node = FALSE;
                /*Free basic met offset*/
                CTC_ERROR_RETURN(sys_humber_nh_global_dync_entry_set_default(lchip,
                                                                             p_member->dsmet.dsmet_offset,
                                                                             p_member->dsmet.dsmet_offset));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
                    SYS_NH_ENTRY_TYPE_MET, 1, p_member->dsmet.dsmet_offset));
            }
            CTC_ERROR_RETURN(_sys_humber_nh_mcast_free_member_info(p_member, NULL));
        }
        if(!CTC_FLAG_ISSET(p_mcast_db->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_SPECIAL))
        {
            if ( p_mcast_db->stats_ptr[lchip]  != 0 )
            {
                sys_humber_stats_delete_statsptr( lchip, 1, p_mcast_db->stats_ptr[lchip]);
            }
            CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
                        SYS_NH_ENTRY_TYPE_FWD, 1, \
                        p_mcast_db->hdr.dsfwd_info[lchip].dsfwd_offset));
          dsfwd_param.p_dsfwd_info = &p_mcast_db->hdr.dsfwd_info[lchip];
          dsfwd_param.lchip = lchip;
          dsfwd_param.drop_pkt = 1;
          dsfwd_param.dsnh_offset = 0;
          /*Write table*/
          CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
        }
    }
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_met_sram_offset(p_mcast_db->basic_met_offset, 1, FALSE));

    return CTC_E_NONE;
}

/**
 @brief Callback function used to update bridge multicast nexthop

 @param[in] p_nh_ptr, pointer of multicast nexthop DB

 @param[in] p_para, member information

 @return CTC_E_XXX
 */
int32
sys_humber_nh_update_mcast_cb(sys_nh_info_com_t* p_nh_info,
        sys_nh_param_com_t* p_para/*Member info*/)
{
    sys_nh_param_mcast_t * p_nh_para_mcast;
    sys_nh_info_mcast_t *p_mcast_db;
    uint8 lchip, curr_chip_num;
    sys_nh_param_dsfwd_t dsfwd_param;
    bool update_dsfwd = FALSE;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_para);
    CTC_PTR_VALID_CHECK(p_nh_info);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MCAST, p_para->hdr.nh_param_type);
    if( SYS_HUMBER_NH_TYPE_IP_TUNNEL != p_nh_info->hdr.nh_entry_type )
    {
        CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_MCAST, p_nh_info->hdr.nh_entry_type);
    }
    p_nh_para_mcast = (sys_nh_param_mcast_t *)(p_para);
    if(NULL == p_nh_para_mcast->p_member)
        return CTC_E_NONE;
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    p_mcast_db = (sys_nh_info_mcast_t*)(p_nh_info);
    curr_chip_num = sys_humber_get_local_chip_num();
    lchip = p_nh_para_mcast->p_member->lchip;

    switch(p_nh_para_mcast->opcode)
    {
     case SYS_HBNH_PARAM_MCAST_ADD_MEMBER:
            if(ctc_list_pointer_empty(&(p_mcast_db->meminfo[lchip].mem_list)))
            {
                update_dsfwd = TRUE;
            }

            CTC_ERROR_RETURN(sys_humber_nh_mcast_add_member(p_mcast_db->basic_met_offset,
                                                            p_nh_para_mcast->p_member,
                                                            &(p_mcast_db->meminfo[lchip].mem_list),p_mcast_db));
            break;

      case SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_MEMBER:
            CTC_ERROR_RETURN(sys_humber_nh_mcast_add_member(p_mcast_db->basic_met_offset,
                            p_nh_para_mcast->p_member,
                            &(p_mcast_db->meminfo[lchip].mem_list),
                            p_mcast_db));
            break;
      case SYS_HBNH_PARAM_MCAST_DEL_MEMBER:
             CTC_ERROR_RETURN(sys_humber_nh_mcast_remove_member(p_nh_para_mcast->p_member,
                                                            &(p_mcast_db->meminfo[lchip].mem_list)));
            if(ctc_list_pointer_empty(&(p_mcast_db->meminfo[lchip].mem_list)))
            {
            	/*Init basic dsmet entry*/
            	CTC_ERROR_RETURN(sys_humber_nh_global_dync_entry_set_default(lchip,
                                                                         p_mcast_db->basic_met_offset,
                                                                         p_mcast_db->basic_met_offset));
            }
            break;

        case SYS_HBNH_PARAM_MCAST_DEL_SPECIAL_MEMBER:
            CTC_ERROR_RETURN(sys_humber_nh_mcast_remove_member(p_nh_para_mcast->p_member,
                            &(p_mcast_db->meminfo[lchip].mem_list)));
            break;

        case SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_DSNH:
            CTC_ERROR_RETURN(_sys_humber_nh_update_mcast_member_by_nhid(p_nh_para_mcast->p_member,
                &(p_mcast_db->meminfo[lchip].mem_list)));
            break;

        case SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_ADD:
            CTC_SET_FLAG(p_mcast_db->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_UP_MEP);
            CTC_ERROR_RETURN(_sys_humber_nh_update_mcast_member_by_vlan(p_nh_para_mcast->p_member->vid,
                                                                        p_mcast_db, TRUE));

        case SYS_HBNH_PARAM_MCAST_UPDATE_MEMBER_DEL:
            CTC_UNSET_FLAG(p_mcast_db->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_UP_MEP);
            CTC_ERROR_RETURN(_sys_humber_nh_update_mcast_member_by_vlan(p_nh_para_mcast->p_member->vid,
                                                                        p_mcast_db, FALSE));
            break;


        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

