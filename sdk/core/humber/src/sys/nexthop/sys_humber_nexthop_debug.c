/**
 @file sys_humber_nexthop_debug.c

 @date 2009-12-28

 @version v2.0

 The file contains all nexthop module core logic
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
#include "sys_humber_opf.h"

#include "sys_humber_ftm.h"
#include "sys_humber_register.h"

#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"

#include "drv_io.h"
#include "drv_humber.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

#define SYS_NH_DEBUG_DUMP_HW_TABLE(tbl_str, FMT, ...)              \
{                                                        \
    SYS_NH_DBG_DUMP("    "tbl_str"::");                                  \
    SYS_NH_DBG_DUMP(FMT, ##__VA_ARGS__);                      \
}

#define SYS_NH_DEBUG_TYPE_HEAD(type) " ~~~~~ DUMP "type" Nexthop ~~~~~~\n"

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/

extern int32 sys_humber_nh_debug_get_nh_master(sys_humber_nh_master_t** p_nh_master);
extern int32 sys_humber_nh_debug_get_nhinfo_by_nhid(uint32 nhid, sys_humber_nh_master_t * p_nh_master, sys_nh_info_com_t * * pp_nhinfo);
/****************************************************************************
 *
* Function
*
*****************************************************************************/
int32
sys_humber_nh_test2(void)
{
    uint8 lport = 0;
    uint8 untagged = 0;
    uint32 crscnt_dsfwd_base;
    ds_fwd_t dsfwd;
    uint32 cmd, cmd2;
    sys_nh_offset_array_t offset_array;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));
    crscnt_dsfwd_base = p_nh_master->port_crscnt_base[0];

    cmd = DRV_IOR(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    cmd2 = DRV_IOW(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    for(lport = 0; lport < 16; lport++)
    {
        sys_humber_brguc_nh_create(lport,CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE);
        sys_humber_brguc_get_dsfwd_offset(lport, untagged, offset_array);
        CTC_ERROR_RETURN(drv_tbl_ioctl(0, offset_array[0], cmd, &dsfwd));
        CTC_ERROR_RETURN(drv_tbl_ioctl(0, (crscnt_dsfwd_base + lport - 1), cmd2, &dsfwd));
        CTC_ERROR_RETURN(drv_tbl_ioctl(0, (crscnt_dsfwd_base + lport - 1), cmd, &dsfwd));

    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_debug_cp_dsfwd(uint8 lchip, uint32 src_cc_lport, uint32 src_fwd_ptr)
{
    ds_fwd_t dsfwd;
    uint32 cmd;
    uint32 crscnt_dsfwd_ptr;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));
    crscnt_dsfwd_ptr = (p_nh_master->port_crscnt_base[lchip] + src_cc_lport);

    cmd = DRV_IOR(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, src_fwd_ptr, cmd, &dsfwd));

    cmd = DRV_IOW(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, crscnt_dsfwd_ptr, cmd, &dsfwd));


    return CTC_E_NONE;
}

int32
sys_humber_nh_display_current_global_sram_info()
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 *p_bmp;
    int32 index, pos;
    bool have_used = FALSE;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));
    if(NULL == p_nh_master)
    {
        SYS_NH_DBG_DUMP("%% Nexthop Module have not been inited \n");
    }
    else
    {
        SYS_NH_DBG_DUMP("%% System max global met sram offset is %d\n",
            p_nh_master->max_glb_met_sram_offset);
        p_bmp = p_nh_master->p_occupid_met_offset_bmp;
        SYS_NH_DBG_DUMP("%% Occupied Offsets are: ");
        for(index = 0; index <= (p_nh_master->max_glb_met_sram_offset >> BITS_SHIFT_OF_WORD); index++)
        {
            for(pos = 0; pos < BITS_NUM_OF_WORD; pos++)
            {
                if(CTC_FLAG_ISSET(p_bmp[index], 1 << pos))
                {
                    SYS_NH_DBG_DUMP("%d, ", (index << BITS_SHIFT_OF_WORD) | (pos));
                    have_used = TRUE;
                }
            }
        }
        if(!have_used)
        {
            SYS_NH_DBG_DUMP("No offset in use");
        }
        SYS_NH_DBG_DUMP("\n");

        have_used = 0;
        SYS_NH_DBG_DUMP("%% System max global nh sram offset is %d\n",
            p_nh_master->max_glb_nh_sram_offset);
        p_bmp = p_nh_master->p_occupid_nh_offset_bmp;
        SYS_NH_DBG_DUMP("%% Occupied Offsets are: ");
        for(index = 0; index <= (p_nh_master->max_glb_nh_sram_offset >> BITS_SHIFT_OF_WORD); index++)
        {
            for(pos = 0; pos < BITS_NUM_OF_WORD; pos++)
            {
                if(CTC_FLAG_ISSET(p_bmp[index], 1 << pos))
                {
                    SYS_NH_DBG_DUMP("%d, ", (index << BITS_SHIFT_OF_WORD) | (pos));
                    have_used = TRUE;
                }
            }
        }
        if(!have_used)
        {
            SYS_NH_DBG_DUMP("No offset in use");
        }
        SYS_NH_DBG_DUMP("\n");
    }

    return CTC_E_NONE;
}
int32
sys_humber_nh_dump_dsfwd(uint8 lchip, uint32 dsfwd_offset, bool detail)
 {
    ds_fwd_t *p_dsfwd, dsfwd;
    uint32 cmd;
    p_dsfwd = &dsfwd;

    cmd = DRV_IOR(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsfwd_offset, cmd, &dsfwd));
    SYS_NH_DEBUG_DUMP_HW_TABLE("DsFwd", "DestMap = 0x%x, NextHopExt:%d,NextHopPtr = %d\n", p_dsfwd->dest_map, p_dsfwd->next_hop_ext,
        p_dsfwd->next_hop_ptr);
    if(detail)
    {
        SYS_NH_DEBUG_DUMP_HW_TABLE("DsFwd", "SequenceNumberChkEn = %d, CriticalPkt = %d, NexthopExt = %d\n",
                        p_dsfwd->sequence_number_chk_en, p_dsfwd->critical_packet,
                        p_dsfwd->next_hop_ext);
        SYS_NH_DEBUG_DUMP_HW_TABLE("DsFwd", "SendLocalPhyPort = %d, LengthAdjType = %d\n",
            p_dsfwd->send_local_phy_port, p_dsfwd->length_adjust_type);
        SYS_NH_DEBUG_DUMP_HW_TABLE("DsFwd", "ApsType = %d, ApsSelectGroupValid = %d\n",
            p_dsfwd->aps_type, p_dsfwd->aps_select_group_valid);
        SYS_NH_DEBUG_DUMP_HW_TABLE("DsFwd", "StatsValid = %d, StatsPtrLow = 0x%x, StatsPtrHigh = 0x%x\n",
            p_dsfwd->stats_valid, p_dsfwd->stats_ptr_lower, p_dsfwd->stats_ptr_upper);

    }

     return CTC_E_NONE;
 }

 int32
 sys_humber_nh_dump_dsnh(uint8 lchip, uint32 dsnh_offset, bool detail)
{
     uint32 cmd;
     ds_nexthop_t *p_dsnh, dsnh;
     epe_next_hop_ctl_t  nxthop_ctl;
     p_dsnh = &dsnh;
#define SYS_HUMBER_NH_DSNH_HD "DsNH"
    memset(&nxthop_ctl, 0, sizeof(nxthop_ctl));
    cmd = DRV_IOR(IOC_REG, EPE_NEXT_HOP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &nxthop_ctl));

    if (((dsnh_offset >> 2) & 0x3FFFF) != nxthop_ctl.ds_next_hop_internal_base)
    {
        cmd = DRV_IOR(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsnh_offset, cmd, &dsnh));
    }
    else
    {
        epe_next_hop_internal4w_t nhp_inter_4w;
        memset(&nhp_inter_4w, 0, sizeof(nhp_inter_4w));
        dsnh_offset = dsnh_offset & 0x3;
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "Use NexthopInternal4W Table, index is %d\n", dsnh_offset);
        cmd = DRV_IOR(IOC_TABLE, EPE_NEXT_HOP_INTERNAL4W, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_tbl_ioctl(lchip, dsnh_offset,
                                         cmd, &nhp_inter_4w));
        memcpy(&dsnh, &nhp_inter_4w, sizeof(dsnh));

    }

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                               "BypassAll = %d, PayloadOP = %d, DestVlanPtr = %d\n",
                               p_dsnh->by_pass_all, p_dsnh->payload_operation, p_dsnh->dest_vlan_ptr);
    if (detail)
    {
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "L2ReWriteType = %d, L2EditPtr = %d\n",
                                   p_dsnh->l2_rewrite_type, p_dsnh->l2edit_ptr);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "L3ReWriteType = %d, L3EditPtr = %d, ServicPoliceValid =%d\n",
                                   p_dsnh->l3_rewrite_type, p_dsnh->l3edit_ptr, p_dsnh->service_policer_vld);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "TaggedMode = %d, MtuCheckEn = %d, StagCfi = %d, StagCos = %d\n",
                                   p_dsnh->tagged_mode, p_dsnh->mtu_check_en, p_dsnh->stag_cfi, p_dsnh->stag_cos);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "OutputSvlanValid = %d, OutputCvlanValid = %d, ServicAclQosEn = %d\n",
                                   p_dsnh->output_svlan_id_valid, p_dsnh->output_cvlan_id_valid, p_dsnh->service_acl_qos_en);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "SvlanTagged = %d, CvlanTagged = %d,  ReplaceDscp = %d\n",
                                   p_dsnh->svlan_tagged, p_dsnh->cvlan_tagged, p_dsnh->replace_dscp);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH_HD,
                                   "ReplaceCtagCos = %d, DeriveStagCos = %d, CopyCtagCos = %d\n",
                                   p_dsnh->replace_ctag_cos, p_dsnh->derive_stag_cos, p_dsnh->copy_ctag_cos);
    }
    return CTC_E_NONE;
}

 int32
 sys_humber_nh_dump_dsnh8w(uint8 lchip, uint32 dsnh_offset, bool detail)
{
     uint32 cmd;
     ds_nexthop8w_t *p_dsnh, dsnh;
     p_dsnh = &dsnh;
#define SYS_HUMBER_NH_DSNH8W_HD "DsNH8W"
    cmd = DRV_IOR(IOC_TABLE, DS_NEXTHOP8W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsnh_offset, cmd, &dsnh));

    SYS_NH_DEBUG_DUMP_HW_TABLE("DsNH8W", "BypassAll = %d, PayloadOP = %d, DestVlanPtr = %d\n",
        p_dsnh->by_pass_all, p_dsnh->payload_operation, p_dsnh->dest_vlan_ptr);

    if (detail)
    {
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "L2ReWriteType = %d, L2EditPtr11to0 = %d, L2EditPtr18to12 = %d\n",
                                   p_dsnh->l2_rewrite_type, p_dsnh->l2edit_ptr11to0, p_dsnh->l2edit_ptr18to12);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "L3ReWriteType = %d, L3EditPtr17to0 = %d, L3EditPtr18 = %d, L3EditPtr19 = %d\n",
                                   p_dsnh->l3_rewrite_type, p_dsnh->l3edit_ptr170, p_dsnh->l3edit_ptr18, p_dsnh->l3edit_ptr19);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "TaggedMode = %d, MtuCheckEn = %d, StagCfi = %d, StagCos = %d\n",
                                   p_dsnh->tagged_mode, p_dsnh->mtu_check_en, p_dsnh->stag_cfi, p_dsnh->stag_cos);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "OutputSvlanValid = %d, OutputCvlanValid = %d, ServicAclQosEn = %d\n",
                                   p_dsnh->output_svlan_id_valid, p_dsnh->output_cvlan_id_valid, p_dsnh->service_acl_qos_en);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "OutputSvlanIdExt = %d, OutputCvlanIdExt = %d, OutputSvlanValidExt = %d\n",
                                   p_dsnh->output_svlan_id_ext, p_dsnh->output_cvlan_id_ext, p_dsnh->output_svlan_id_valid_ext);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "OutputCvlanValidExt = %d, SvlanTpidEn = %d, SvlanTpid = %d\n",
                                   p_dsnh->output_cvlan_id_valid_ext, p_dsnh->svlan_tpid_en, p_dsnh->svlan_tpid);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "SvlanTagged = %d, CvlanTagged = %d,  ReplaceDscp = %d\n",
                                   p_dsnh->svlan_tagged, p_dsnh->cvlan_tagged, p_dsnh->replace_dscp);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "ReplaceCtagCos = %d, DeriveStagCos = %d, CopyCtagCos = %d\n",
                                   p_dsnh->replace_ctag_cos, p_dsnh->derive_stag_cos, p_dsnh->copy_ctag_cos);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "VplsDestPort =%d, VplsPortCheck = %d, TunnelMtuCheck = %d\n",
                                   p_dsnh->vpls_dest_port, p_dsnh->vpls_port_check, p_dsnh->tunnel_mtu_check);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "TunnelUpdateDisable =%d, CommuityPort = %d\n",
                                   p_dsnh->tunnel_update_disable, p_dsnh->community_port);

        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSNH8W_HD,
                                   "ServicIdEn = %d, ServicePolicerValid =%d, ServiceId = %d\n",
                                   p_dsnh->service_id_en, p_dsnh->service_policer_valid, p_dsnh->service_id);

    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_dsl2edit(uint8 lchip, uint32 dsl2edit_offset, bool detail)
{
    uint32 cmd;
    ds_l2_edit_eth4w_t  dsl2edit;
#define SYS_HUMBER_NH_DSL2EDIT_ETH4W_HD "DsL2Edit"

    cmd = DRV_IOR(IOC_TABLE, DS_L2_EDIT_ETH4W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsl2edit_offset, cmd, &dsl2edit));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL2EDIT_ETH4W_HD,
                               "OutputVidValid = %d, OutPutVid = %d, OutPutVidIsSvlan = %d\n",
                               dsl2edit.output_vlan_id_valid, dsl2edit.output_vlan_id, dsl2edit.output_vlanid_is_svlan);
    if (detail)
    {
       SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL2EDIT_ETH4W_HD,
                                  "OutputCVidValid = %d, OutputMacDaHigh = 0x%x, OutputMacDaLow = 0x%x\n",
                                  dsl2edit.output_cvlanid_valid, dsl2edit.mac_dah, dsl2edit.mac_dal);

       SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL2EDIT_ETH4W_HD,
                                  "DerivMcastMac = %d, PktType = %d, MacSaVld = %d, OverwritEtherType = %d\n",
                                  dsl2edit.derive_mcast_mac, dsl2edit.packet_type, dsl2edit.mac_sa_valid, dsl2edit.overwrite_ether_type);
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_dsl3edit_mpls4w(uint8 lchip, uint32 offset, bool detail)
{
    uint32 cmd;
    ds_l3edit_mpls4w_t  dsl3edit;
#define SYS_HUMBER_NH_DSL3EDIT_MPLS4W_HD "DsL3EditMpls4W"

    cmd = DRV_IOR(IOC_TABLE, DS_L3EDIT_MPLS4W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &dsl3edit));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS4W_HD,
                               "MartiniEncapValid = %d, Label0 = 0x%x, Ttl0 = %d, exp0 = %d\n",
                               dsl3edit.martini_encap_valid, dsl3edit.label0,
                               dsl3edit.ttl0, dsl3edit.exp0);
   SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS4W_HD,
                              "DeriveExp0 = %d, MapTtl0 = %d, mcastLabel0 = %d\n",
                              dsl3edit.derive_exp0, dsl3edit.map_ttl0, dsl3edit.mcast_label0);
    if (detail)
    {
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS4W_HD,
                                    "Label1Valid = %d, Label1 = 0x%x, Ttl1 = %d, exp1 = %d\n",
                                    dsl3edit.label_valid1, dsl3edit.label1,
                                    dsl3edit.ttl1, dsl3edit.exp1);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS4W_HD,
                                   "DeriveExp1 = %d, MapTtl1 = %d, mcastLabel1 = %d\n",
                                   dsl3edit.derive_exp1, dsl3edit.map_ttl1, dsl3edit.mcast_label1);
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_dsl3edit_mpls8w(uint8 lchip, uint32 offset, bool detail)
{
    uint32 cmd;
    ds_l3edit_mpls8w_t  dsl3edit;
#define SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD "DsL3EditMpls8W"

    cmd = DRV_IOR(IOC_TABLE, DS_L3EDIT_MPLS8W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &dsl3edit));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                               "MartiniEncapValid = %d, Label0 = 0x%x, Ttl0 = %d, exp0 = %d\n",
                               dsl3edit.martini_encap_valid, dsl3edit.label0,
                               dsl3edit.ttl0, dsl3edit.exp0);
   SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                              "DeriveExp0 = %d, MapTtl0 = %d, mcastLabel0 = %d\n",
                              dsl3edit.derive_exp0, dsl3edit.map_ttl0, dsl3edit.mcast_label0);
    if (detail)
    {
        /*Label1*/
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                    "Label1Valid = %d, Label1 = 0x%x, Ttl1 = %d, exp1 = %d\n",
                                    dsl3edit.label_valid1, dsl3edit.label1,
                                    dsl3edit.ttl1, dsl3edit.exp1);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                   "DeriveExp1 = %d, MapTtl1 = %d, mcastLabel1 = %d\n",
                                   dsl3edit.derive_exp1, dsl3edit.map_ttl1, dsl3edit.mcast_label1);

        /*Label2*/
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                    "Label2Valid = %d, Label2 = 0x%x, Ttl2 = %d, exp2 = %d\n",
                                    dsl3edit.label_valid2, dsl3edit.label2,
                                    dsl3edit.ttl2, dsl3edit.exp2);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                   "DeriveExp2 = %d, MapTtl2 = %d, mcastLabel2 = %d\n",
                                   dsl3edit.derive_exp2, dsl3edit.map_ttl2, dsl3edit.mcast_label2);

        /*Label3*/
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                    "Label3Valid = %d, Label3 = 0x%x, Ttl3 = %d, exp3 = %d\n",
                                    dsl3edit.label_valid3, dsl3edit.label3,
                                    dsl3edit.ttl3, dsl3edit.exp3);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_MPLS8W_HD,
                                   "DeriveExp3 = %d, MapTtl3 = %d, mcastLabel3 = %d\n",
                                   dsl3edit.derive_exp3, dsl3edit.map_ttl3, dsl3edit.mcast_label3);
    }
    return CTC_E_NONE;
}
int32
sys_humber_nh_dump_dsl3edit_tunnel_v4(uint8 lchip, uint32 offset, bool detail)
{
    uint32 cmd;
    ds_l3edit_tunnel_v4_t  dsl3edit;

#define SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V4_HD "DsL3EditTunnelV4"

    cmd = DRV_IOR(IOC_TABLE, DS_L3EDIT_TUNNEL_V4, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &dsl3edit));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V4_HD,
                               "ttl = %d, dscp = %d, map_ttl = %d, derive_dscp = %d\n",
                               dsl3edit.ttl, dsl3edit.dscp,
                               dsl3edit.map_ttl, dsl3edit.derive_dscp);

   SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V4_HD,
                              "ip_sa_gre_key_31_16 = 0x%x, ip_da = 0x%x, inner_header_type = %d\n",
                              dsl3edit.ip_sa, dsl3edit.ip_da, dsl3edit.inner_header_type);
    if (detail)
    {
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V4_HD,
                                    "is_atap_tunnel = %d, t6to4_tunnel_sa =  %d, t6to4_tunnel = %d, inner_header_valid = %d\n",
                                    dsl3edit.is_atp_tunnel, dsl3edit.t6to4_tunnel_sa,
                                    dsl3edit.t6to4_tunnel, dsl3edit.inner_header_valid);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V4_HD,
                                   "mtu_check_en = %d, gre_key_15_0_udp_dest_port = %d, gre_flags = %d, gre_version = %d\n",
                                   dsl3edit.mtu_check_en, dsl3edit.gre_key_udp_dest_port, dsl3edit.gre_flags, dsl3edit.gre_version);
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_dsl3edit_tunnel_v6(uint8 lchip, uint32 offset, bool detail)
{
    uint32 cmd;
    ds_l3edit_tunnel_v4_t  dsl3edit;
#define SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V6_HD "DsL3EditTunnelV6"

    cmd = DRV_IOR(IOC_TABLE, DS_L3EDIT_TUNNEL_V4, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &dsl3edit));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V6_HD,
                               "ttl = %d, dscp = %d, map_ttl = %d, derive_dscp = %d\n",
                               dsl3edit.ttl, dsl3edit.dscp,
                               dsl3edit.map_ttl, dsl3edit.derive_dscp);

   SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V6_HD,
                              "ip_sa = 0x%x, ip_da = 0x%x, inner_header_type = %d\n",
                              dsl3edit.ip_sa, dsl3edit.ip_da, dsl3edit.inner_header_type);
    if (detail)
    {
         SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V6_HD,
                                    "is_atap_tunnel = %d, t6to4_tunnel_sa =  %d, t6to4_tunnel = %d, inner_header_valid = %d\n",
                                    dsl3edit.is_atp_tunnel, dsl3edit.t6to4_tunnel_sa,
                                    dsl3edit.t6to4_tunnel, dsl3edit.inner_header_valid);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSL3EDIT_TUNNEL_V6_HD,
                                   "mtu_check_en = %d, gre_key_udp_dest_port = %d, gre_flags = %d, gre_version = %d\n",
                                   dsl3edit.mtu_check_en, dsl3edit.gre_key_udp_dest_port, dsl3edit.gre_flags, dsl3edit.gre_version);
    }
    return CTC_E_NONE;
}




int32
sys_humber_nh_dump_dsmet(uint8 lchip, uint32 dsmet_offset, bool detail)
{
    uint32 cmd;
    ds_met_entry_t dsmet;
#define SYS_HUMBER_NH_DSMET_HD "DsMet"

    cmd = DRV_IOR(IOC_TABLE, DS_MET_ENTRY, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsmet_offset, cmd, &dsmet));

    SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSMET_HD,
                               "UcastIdLow = %d, UcastIdhigh = %d, isLinkAgg = %d, RemoteBay = %d\n",
                               dsmet.ucast_id_lower, dsmet.ucast_id_upper, dsmet.is_link_aggregation, dsmet.remote_bay);

    if (detail)
    {
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSMET_HD,
                                   "DsMetOffset = 0x%x, NextMetOffset = 0x%x, EndLocalRepli = %d\n",
                                   dsmet_offset, dsmet.next_met_entry_ptr, dsmet.end_local_rep);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSMET_HD,
                                   "ReplicationCtl = 0x%x, NexthopExt = %d, portCheckDiscard = %d\n",
                                   dsmet.replication_ctl, dsmet.nexthop_ext, dsmet.port_check_discard);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSMET_HD,
                                    "ApsBrgEn = %d, ApsProtectPath = %d, ApsMismatchDiscard = %d\n",
                                    dsmet.aps_brg_en, dsmet.aps_brg_protect_path, dsmet.aps_brg_mismatch_discard);
        SYS_NH_DEBUG_DUMP_HW_TABLE(SYS_HUMBER_NH_DSMET_HD,
                                    "LengthAdjustType = %d\n", dsmet.length_adjust_type);
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_iloop(uint32 nhid, bool detail)
{
    sys_nh_info_brguc_t *p_brguc_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  BridgeUC nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_ILOOP!= p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't BridgeUC nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("IPE Loopback"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_brguc_info = (sys_nh_info_brguc_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
                        p_brguc_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
        CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_brguc_info->hdr.dsfwd_info[lchip].dsfwd_offset, detail));
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_rspan(uint32 nhid, bool detail)
{
    sys_nh_info_rspan_t *p_rspan_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  RSPAN nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_RSPAN!= p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't RSPAN nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("RSPAN"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_rspan_info = (sys_nh_info_rspan_t*)(p_nhinfo);

         SYS_NH_DBG_DUMP("   <<< Dump DsNexthop, Offset = %d >>>\n", p_rspan_info->dsnh_info.dsnh_offset);

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_downmep(uint32 nhid, bool detail)
{
    sys_nh_info_downmep_t *p_downmep_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    int32 ret;
    uint8 curr_chip_num = 0;
    uint8 lchip = 0;

    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  Downmep  nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_DOWNMEP!= p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't downMep nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("downMep"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_downmep_info = (sys_nh_info_downmep_t*)(p_nhinfo);

     curr_chip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {

         SYS_NH_DBG_DUMP("   <<< Dump DsNexthop, lchip= %d Offset = %d >>>\n", lchip,p_downmep_info->p_dsnh_info[lchip].dsnh_offset);
    }
    return CTC_E_NONE;
}


int32
sys_humber_nh_dump_brguc(uint32 nhid, bool detail)
{
    sys_nh_info_brguc_t *p_brguc_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num, gchip, tmp_lchip;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  BridgeUC nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_BRGUC != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't BridgeUC nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("BridgeUC"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_brguc_info = (sys_nh_info_brguc_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();
    gchip = SYS_MAP_GPORT_TO_GCHIP(p_brguc_info->dest.dest_gport);
    sys_humber_chip_is_local(gchip, &tmp_lchip);

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
                        p_brguc_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_brguc_info->hdr.dsfwd_info[lchip].dsfwd_offset, detail));


        /*Dump Dsnh*/
        if(CTC_IS_LINKAGG_PORT(p_brguc_info->dest.dest_gport) || (tmp_lchip == lchip))
        {
            switch(p_brguc_info->nh_sub_type)
            {
            case SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC:
                SYS_NH_DBG_DUMP("    BridgeUC nexthop sub type is: Normal bridge unicast nexthop\n");
                break;
            case SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT:
                SYS_NH_DBG_DUMP("    BridgeUC nexthop sub type is: Egress vlan translation nexthop\n");
                break;
            case SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS:
                SYS_NH_DBG_DUMP("    BridgeUC nexthop sub type is: Bypass all nexthop\n");
                break;
            case SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT:
                SYS_NH_DBG_DUMP("    BridgeUC nexthop sub type is: APS egress vlan edit nexthop\n");
                break;
            case SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU:
                SYS_NH_DBG_DUMP("    BridgeUC nexthop sub type is: raw packet elog to cpu(oamengine)\n");
                break;
            default:
                SYS_NH_DBG_DUMP("    Dump bridgeUC nexthop ERROR, sub type is %d\n", p_brguc_info->nh_sub_type);
                return CTC_E_INVALID_NH_SUB_TYPE;
            }
            if (detail)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip, p_brguc_info->dsnh_info.dsnh_offset, detail));
                if(SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT == p_brguc_info->nh_sub_type)
                {
                    SYS_NH_DBG_DUMP("    APS protection path's dsnexthop information:\n");
                    CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip,
                        (p_brguc_info->dsnh_info.dsnh_offset + 1), detail));
                }
            }

        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_ipuc(uint32 nhid, bool detail)
{
    sys_nh_info_ipuc_t *p_ipuc_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  IPUC nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_IPUC != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't IPUC nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("IPUC"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_ipuc_info = (sys_nh_info_ipuc_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
            p_ipuc_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
        CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_ipuc_info->hdr.dsfwd_info[lchip].dsfwd_offset, detail));
        SYS_NH_DBG_DUMP("    L3 interface Id is %d\n", p_ipuc_info->l3ifid);
        /*Dump Dsnh & DsL2Edit*/
        if(p_ipuc_info->p_dsl2edit_info[lchip])
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip, p_ipuc_info->dsnh_info.dsnh_offset, detail));
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsl2edit(lchip, p_ipuc_info->p_dsl2edit_info[lchip]->hdr.offset, detail));
            SYS_NH_DBG_DUMP("    L2EditRefCnt is %d\n", p_ipuc_info->p_dsl2edit_info[lchip]->hdr.ref_cnt);
        }
        else
        {
            SYS_NH_DBG_DUMP("    No DsNexthop and DsL2Edit\n");
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_ecmp(uint32 nhid, bool detail)
{
    sys_nh_info_ecmp_t *p_ecmp_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num;
    int32 ret;
    uint8 i;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  ECMP nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_ECMP != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't ECMP nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("ECMP"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_ecmp_info = (sys_nh_info_ecmp_t*)(p_nhinfo);

    SYS_NH_DBG_DUMP(" Item Number = %d\n", p_ecmp_info->valid_item_cnt);
    for(i = 0; i < p_ecmp_info->valid_item_cnt; i++)
    {
        SYS_NH_DBG_DUMP(" NH = %d, Oper NH = %d    Oper %s\n", p_ecmp_info->item_array[i].nh_id, p_ecmp_info->item_array[i].oper_nh_id,
                        p_ecmp_info->item_array[i].is_oper_valid ? "Enable" : "Disable");
    }

    SYS_NH_DBG_DUMP(" Outgoing interface Number = %d\n", p_ecmp_info->oif_cnt);
    for(i = 0; i < p_ecmp_info->oif_cnt; i++)
    {
        SYS_NH_DBG_DUMP(" Out if = %d, count = %d\n", p_ecmp_info->rpf_array[i].oif_id, p_ecmp_info->rpf_array[i].ref);
    }

    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
            p_ecmp_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
        for(i = 0; i < p_ecmp_info->valid_item_cnt; i++)
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_ecmp_info->hdr.dsfwd_info[lchip].dsfwd_offset + i, detail));
        }
    }

    return CTC_E_NONE;
}


int32
sys_humber_nh_dump_mcast(uint32 nhid, bool detail)
{
    sys_nh_info_mcast_t *p_mcast_info;
    uint8 lchip, curr_chip_num;
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_info_com_t* p_nhinfo = NULL;
    ctc_list_pointer_node_t *p_pos, *p_pos_repli;
    sys_nh_mcast_meminfo_t *p_meminfo_com;
    sys_nh_mcast_meminfo_withrepli_t *p_meminfo_withrepli;
    sys_nh_mcast_repli_node_t *p_repli_node;
    int32 memcnt, replicnt, ret;

    SYS_NH_DBG_FUNC();
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("%% Mcast nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);
    if(SYS_HUMBER_NH_TYPE_MCAST != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't BridgeMC nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("Mcast"));

    p_mcast_info = (sys_nh_info_mcast_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();

    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_mcast_info->hdr.nh_entry_type, p_mcast_info->hdr.nh_entry_flags);
    SYS_NH_DBG_DUMP(" Baisc Met offset = %d\n", p_mcast_info->basic_met_offset);
    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        if (!CTC_FLAG_ISSET(p_mcast_info->hdr.nh_entry_flags, SYS_NH_INFO_MCAST_FLAG_GROUP_IS_SPECIAL))
        {
            SYS_NH_DBG_DUMP("\n @@@ Dump member on Local Chip %d, DsFwdPtr = %d @@@\n", lchip,
                            p_mcast_info->hdr.dsfwd_info[lchip].dsfwd_offset);
        }
        else
        {
            SYS_NH_DBG_DUMP("\n @@@ Dump member on Local Chip %d, OAM Up MEP, no DsFwd @@@\n", lchip);
        }
        memcnt = 0;
        CTC_LIST_POINTER_LOOP(p_pos, &(p_mcast_info->meminfo[lchip].mem_list))
        {
            memcnt++;
            p_meminfo_com = _ctc_container_of(p_pos, sys_nh_mcast_meminfo_t, list_head);
            SYS_NH_DBG_DUMP("    <<< Multicast Member: %d, met offset = 0x%x  >>>\n",
                memcnt, p_meminfo_com->dsmet.dsmet_offset);
            if (detail)
            {
                sys_humber_nh_dump_dsmet(lchip, p_meminfo_com->dsmet.dsmet_offset, detail);
            }
            else
            {
                SYS_NH_DBG_DUMP("    NextMetOffset = 0x%x\n    DsNexthopOffset = 0x%x\n    UcastId = %d\n"
                    "    APS protection path = %d\n",
                    p_meminfo_com->dsmet.next_dsmet_offset,
                    p_meminfo_com->dsmet.dsnh_offset,
                    p_meminfo_com->dsmet.ucastid,
                    p_meminfo_com->dsmet.aps_protection_path);
                SYS_NH_DBG_DUMP("    IsLinkagg = %d\n    EndLocal = %d\n    ReplicateNum = %d\n    PortCheckDiscard = %d\n",
                    p_meminfo_com->dsmet.is_linkagg,
                    p_meminfo_com->dsmet.end_local,
                    p_meminfo_com->dsmet.replicate_num,
                    p_meminfo_com->dsmet.port_check_discard);

            }
            switch(p_meminfo_com->dsmet.member_type)
            {
                case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
                    SYS_NH_DBG_DUMP("    McastMemberType = Bridge Multicast Local Member(No VlanTranslation)\n");
                    break;

                case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
                    SYS_NH_DBG_DUMP("    McastMemberType = Bridge Multicast Local Member(With VlanTranslation)\n");
                    break;

                case SYS_NH_PARAM_IPMC_MEM_LOCAL:
                 case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
                    SYS_NH_DBG_DUMP("    McastMemberType = IP Multicast Local Member\n");
                    p_meminfo_withrepli = (sys_nh_mcast_meminfo_withrepli_t*)p_meminfo_com;
                    SYS_NH_DBG_DUMP("    FreeDsNexthopOffsetCnt(Allocate but not use): %d\n", p_meminfo_withrepli->free_dsnh_offset_cnt);
                    replicnt = 0;
                    CTC_LIST_POINTER_LOOP(p_pos_repli, &(p_meminfo_withrepli->repli_list))
                    {
                        p_repli_node = _ctc_container_of(p_pos_repli, sys_nh_mcast_repli_node_t, list_head);
                        replicnt ++;
                        SYS_NH_DBG_DUMP("    *** Logical Replicate Node %d ***\n", replicnt);
                        CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip,
                                                                 (p_meminfo_withrepli->comm.dsmet.dsnh_offset + replicnt - 1),
                                                                 detail));
                        if( !p_repli_node->p_dsl2edit_info)
                            continue;

                        if(SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W != p_repli_node->p_dsl2edit_info->hdr.entry_type)
                        {
                            SYS_NH_DBG_DUMP("    Invalid DsL2Edit4W\n");
                        }
                        else
                        {
                            CTC_ERROR_RETURN(sys_humber_nh_dump_dsl2edit(
                                lchip, p_repli_node->p_dsl2edit_info->hdr.offset, detail));
                            SYS_NH_DBG_DUMP("    L2EditRefCnt = %d\n", p_repli_node->p_dsl2edit_info->hdr.ref_cnt);
                        }
                    }
                    break;

                case SYS_NH_PARAM_MCAST_MEM_REMOTE:
                    SYS_NH_DBG_DUMP("    McastMemberType = Remote Entry\n");
                    break;
                default:
                    SYS_NH_DBG_DUMP("    McastMemberType = Invalid\n");
                    break;
            }
        };
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_mpls(uint32 nhid, bool detail)
{
    sys_nh_info_mpls_t *p_mpls_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  MPLS nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_MPLS != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't MPLS nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("MPLS"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_mpls_info = (sys_nh_info_mpls_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
            p_mpls_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
        CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_mpls_info->hdr.dsfwd_info[lchip].dsfwd_offset, detail));
        /*Dump Dsnh & DsL2Edit*/
        if(CTC_FLAG_ISSET(p_mpls_info->working_path.lchip_bmp, (1 << lchip)))
        {
            SYS_NH_DBG_DUMP("    L3 interface Id is %d  gport = 0x%x\n", p_mpls_info->working_path.l3ifid,p_mpls_info->working_path.gport);
            if(CTC_FLAG_ISSET(p_mpls_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W))
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh8w(lchip, p_mpls_info->dsnh_offset, detail));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip, p_mpls_info->dsnh_offset, detail));
            }

            if(p_mpls_info->working_path.p_dsl2edit_info[lchip])
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl2edit(lchip,
                    p_mpls_info->working_path.p_dsl2edit_info[lchip]->hdr.offset, detail));
                SYS_NH_DBG_DUMP("    L2EditRefCnt is %d\n",
                    p_mpls_info->working_path.p_dsl2edit_info[lchip]->hdr.ref_cnt);
            }
            else
            {
                SYS_NH_DBG_DUMP("    No DsL2Edit on local chip %d\n", lchip);
            }

            if(SYS_NH_L3EDIT_TYPE_MPLS_8W == p_mpls_info->working_path.dsl3edit_type)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_mpls8w(lchip,
                    p_mpls_info->working_path.dsl3edit_offset[lchip], detail));
            }
            else if(SYS_NH_L3EDIT_TYPE_MPLS_4W == p_mpls_info->working_path.dsl3edit_type)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_mpls4w(lchip,
                    p_mpls_info->working_path.dsl3edit_offset[lchip], detail));
            }
            else
            {
                SYS_NH_DBG_DUMP("    No DsL3EditMpls Table on local chip %d\n", lchip);
            }
        }

        if(CTC_FLAG_ISSET(p_mpls_info->protection_path.lchip_bmp, (1 << lchip)))
        {
            SYS_NH_DBG_DUMP("    APS MPLS nexthop, protection path information\n");
            SYS_NH_DBG_DUMP("    L3 interface Id is %d,  gport = 0x%x\n", p_mpls_info->protection_path.l3ifid,p_mpls_info->protection_path.gport);
            if(CTC_FLAG_ISSET(p_mpls_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W))
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh8w(lchip,
                    (p_mpls_info->dsnh_offset + 2), detail));
            }
            else
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip,
                    (p_mpls_info->dsnh_offset + 1), detail));
            }

            if(p_mpls_info->protection_path.p_dsl2edit_info[lchip])
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl2edit(lchip,
                    p_mpls_info->protection_path.p_dsl2edit_info[lchip]->hdr.offset, detail));
                SYS_NH_DBG_DUMP("    L2EditRefCnt is %d\n",
                    p_mpls_info->protection_path.p_dsl2edit_info[lchip]->hdr.ref_cnt);
            }
            else
            {
                SYS_NH_DBG_DUMP("    No DsL2Edit on local chip %d\n", lchip);
            }

            if (SYS_NH_L3EDIT_TYPE_MPLS_8W == p_mpls_info->protection_path.dsl3edit_type)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_mpls8w(lchip,
                        p_mpls_info->protection_path.dsl3edit_offset[lchip], detail));
            }
            else if(SYS_NH_L3EDIT_TYPE_MPLS_4W == p_mpls_info->protection_path.dsl3edit_type)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_mpls4w(lchip,
                        p_mpls_info->protection_path.dsl3edit_offset[lchip], detail));
            }
            else
            {
                SYS_NH_DBG_DUMP("    No DsL3EditMpls Table on local chip %d\n", lchip);
            }
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_dump_ip_tunnel(uint32 nhid, bool detail)
{
    sys_nh_info_ip_tunnel_t *p_ip_tunnel_info;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, curr_chip_num;
    int32 ret;
    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
    if(CTC_E_NH_NOT_EXIST == ret)
    {
        SYS_NH_DBG_DUMP("  IP tunnel nexthop %d not exist\n", nhid);
        return CTC_E_NONE;
    }
    else if(CTC_E_INVALID_NHID == ret)
    {
        SYS_NH_DBG_DUMP("%% Invalid nexthopId %d\n", nhid);
        return CTC_E_NONE;
    }
    else
        CTC_ERROR_RETURN(ret);

    if(SYS_HUMBER_NH_TYPE_IP_TUNNEL != p_nhinfo->hdr.nh_entry_type)
    {
        SYS_NH_DBG_DUMP("%% Isn't IP tunnel nexthop for nexthopId %d\n", nhid);
        return CTC_E_INVALID_PARAM;
    }
    SYS_NH_DBG_DUMP(SYS_NH_DEBUG_TYPE_HEAD("IP tunnel"));
    SYS_NH_DBG_DUMP(" EntryType = %d, EntryFlags = 0x%x\n",
        p_nhinfo->hdr.nh_entry_type, p_nhinfo->hdr.nh_entry_flags);
    p_ip_tunnel_info = (sys_nh_info_ip_tunnel_t*)(p_nhinfo);
    curr_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*Dump dsFwd*/
        SYS_NH_DBG_DUMP("   <<< Dump DsFwd, Offset = %d, Lchip = %d>>>\n",
            p_ip_tunnel_info->hdr.dsfwd_info[lchip].dsfwd_offset, lchip);
        CTC_ERROR_RETURN(sys_humber_nh_dump_dsfwd(lchip, p_ip_tunnel_info->hdr.dsfwd_info[lchip].dsfwd_offset, detail));
        /*Dump Dsnh & DsL2Edit*/

        SYS_NH_DBG_DUMP("    L3 interface Id is %d  gport = 0x%x\n", p_ip_tunnel_info->ip_tunnel_edit_info.l3ifid,p_ip_tunnel_info->ip_tunnel_edit_info.gport);
        if(CTC_FLAG_ISSET(p_ip_tunnel_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W))
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh8w(lchip, p_ip_tunnel_info->dsnh_offset, detail));
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsnh(lchip, p_ip_tunnel_info->dsnh_offset, detail));
        }

        if(p_ip_tunnel_info->ip_tunnel_edit_info.p_dsl2edit_info[lchip])
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsl2edit(lchip,
                p_ip_tunnel_info->ip_tunnel_edit_info.p_dsl2edit_info[lchip]->hdr.offset, detail));
            SYS_NH_DBG_DUMP("    L2EditRefCnt is %d\n",
                p_ip_tunnel_info->ip_tunnel_edit_info.p_dsl2edit_info[lchip]->hdr.ref_cnt);
        }
        else
        {
            SYS_NH_DBG_DUMP("    No DsL2Edit on local chip %d\n", lchip);
        }

        if(SYS_NH_L3EDIT_TYPE_TUNNEL_V4 == p_ip_tunnel_info->ip_tunnel_edit_info.dsl3edit_type)
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_tunnel_v4(lchip,
                p_ip_tunnel_info->ip_tunnel_edit_info.dsl3edit_offset[lchip], detail));
        }
        else if(SYS_NH_L3EDIT_TYPE_TUNNEL_V6 == p_ip_tunnel_info->ip_tunnel_edit_info.dsl3edit_type)
        {
            CTC_ERROR_RETURN(sys_humber_nh_dump_dsl3edit_tunnel_v6(lchip,
                p_ip_tunnel_info->ip_tunnel_edit_info.dsl3edit_offset[lchip], detail));
        }
        else
        {
            SYS_NH_DBG_DUMP("    No DsL3EditMpls Table on local chip %d\n", lchip);
        }


    }
    return CTC_E_NONE;
}


void
sys_humber_nh_dump_by_type(uint32 nhid, sys_humber_nh_type_t nh_type, bool detail)
{
    SYS_NH_DBG_DUMP("\n\n");
    switch(nh_type)
    {
        case SYS_HUMBER_NH_TYPE_NULL:
            SYS_NH_DBG_DUMP("NHID is %d, Invalid Nexthop\n", nhid);
            break;

        case SYS_HUMBER_NH_TYPE_MCAST:
            SYS_NH_DBG_DUMP("NHID is %d, Mcast Nexthop\n", nhid);
            sys_humber_nh_dump_mcast(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_BRGUC:
            SYS_NH_DBG_DUMP("NHID is %d, Bridge Unicast Nexthop\n", nhid);
            sys_humber_nh_dump_brguc(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_IPUC:
            SYS_NH_DBG_DUMP("NHID is %d, IP Unicast Nexthop\n", nhid);
            sys_humber_nh_dump_ipuc(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_MPLS:
            SYS_NH_DBG_DUMP("NHID is %d, MPLS Nexthop\n", nhid);
            sys_humber_nh_dump_mpls(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_ECMP:
            SYS_NH_DBG_DUMP("NHID is %d, ECMP Nexthop\n", nhid);
            sys_humber_nh_dump_ecmp(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_DROP:
            SYS_NH_DBG_DUMP("NHID is %d, Drop Nexthop\n", nhid);
            break;

        case SYS_HUMBER_NH_TYPE_TOCPU:
            SYS_NH_DBG_DUMP("NHID is %d, ToCPU Nexthop\n", nhid);
            break;

        case SYS_HUMBER_NH_TYPE_UNROV:
            SYS_NH_DBG_DUMP("NHID is %d, Unresolved Nexthop\n", nhid);
            break;

        case SYS_HUMBER_NH_TYPE_ILOOP:
            SYS_NH_DBG_DUMP("NHID is %d, I-Loop Nexthop\n", nhid);
            sys_humber_nh_dump_iloop(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_IP_TUNNEL:
            SYS_NH_DBG_DUMP("NHID is %d, IP Tunnel Nexthop\n", nhid);
            sys_humber_nh_dump_ip_tunnel(nhid, detail);
            break;

        case SYS_HUMBER_NH_TYPE_ELOOP:
            SYS_NH_DBG_DUMP("NHID is %d, E-Loop Nexthop\n", nhid);
            break;
       case SYS_HUMBER_NH_TYPE_RSPAN:
            SYS_NH_DBG_DUMP("NHID is %d, RSPAN Nexthop\n", nhid);
            break;
      case SYS_HUMBER_NH_TYPE_DOWNMEP:
          SYS_NH_DBG_DUMP("NHID is %d, DownMep Nexthop\n", nhid);
            break;
        default:
            SYS_NH_DBG_DUMP("Invalid Nexthop Type\n");
            return;
    }

    return;
}

int32
sys_humber_nh_dump_all(sys_humber_nh_type_t nh_type, bool detail)
{
    uint32 nhid;
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_info_com_t* p_nhinfo = NULL;
    int32 ret;


    CTC_ERROR_RETURN(sys_humber_nh_debug_get_nh_master(&p_nh_master));

    SYS_NH_DBG_DUMP("remote_met_use_nhp:%d\n",p_nh_master->remote_met_use_nhp);
    SYS_NH_DBG_DUMP("glb_alloc_nexhop:%d\n",p_nh_master->glb_alloc_nexhop);
    SYS_NH_DBG_DUMP("use_epe_parser_ttl_in_ipmc:%d\n",p_nh_master->use_epe_parser_ttl_in_ipmc);

    for(nhid = 0; nhid < SYS_HUMBER_NH_INTERNAL_NHID_MAX; nhid++)
    {
        ret = (sys_humber_nh_debug_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));
        if(CTC_E_NH_NOT_EXIST == ret)
            continue;
        if(p_nhinfo && (p_nhinfo->hdr.nh_entry_type == nh_type || SYS_HUMBER_NH_TYPE_MAX == nh_type))
        {
            sys_humber_nh_dump_by_type(nhid, p_nhinfo->hdr.nh_entry_type, detail);
        }
    }
    return CTC_E_NONE;
}
