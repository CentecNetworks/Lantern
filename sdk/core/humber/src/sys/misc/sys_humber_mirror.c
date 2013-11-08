/**
 @file sys_humber_mirror.c

 @date 2009-10-21

 @version v2.0
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_common.h"
#include "ctc_mirror.h"
#include "ctc_const.h"
#include "ctc_cpu_traffic.h"

#include "sys_humber_chip.h"
#include "sys_humber_mirror.h"
#include "sys_humber_port.h"
#include "sys_humber_vlan.h"
#include "sys_humber_l3if.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_internal_port.h"

#include "drv_io.h"
#include "drv_humber.h"

#include "sys_humber_queue_enq.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

static bool sys_mirror_initialized = FALSE;

#define SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX     3

#define SYS_MIRROR_INIT_CHECK() \
    {\
        if(sys_mirror_initialized == FALSE)\
            return CTC_E_NOT_INIT;\
    }
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/*
 @brief This function is to write chip to set the destination port
*/
static int32
_sys_humber_mirror_set_dest_write_chip(uint32 excp_index, uint16 dest_gport, uint32 nh_ptr)
{
    uint8 lchip_id;
    uint8 gchip_id;
    uint8 lchip_num;
    uint8 lport;
    uint32 destmap;
    uint32 cmd;
    ds_met_fifo_excp_t met_excp;
    buf_retrv_exception_mem_t bufrev_exp;

    lchip_num = sys_humber_get_local_chip_num();
    lport = CTC_MAP_GPORT_TO_LPORT(dest_gport);

    for (lchip_id = 0; lchip_id < lchip_num; lchip_id++)
    {

        if (SYS_RESERVED_INTERNAL_PORT_FOR_DROP == lport)
        {
            sys_humber_get_gchip_id(lchip_id, &gchip_id);
            destmap = (gchip_id << 16) | SYS_RESERVED_INTERNAL_PORT_FOR_DROP;
        }
        else
        {
            gchip_id = SYS_MAP_GPORT_TO_GCHIP(dest_gport);

            if((lport & 0xFF) > SYS_STATIC_INT_PORT_END)
            {
               destmap =  (gchip_id << 16)  | (SYS_QSEL_TYPE_INTERNAL_PORT << 12) | lport ; /*queueseltype:destmap[15:12]*/
            }
            else
            {
               destmap = (gchip_id << 16) | lport;
            }
         }


        /*read DS_MET_FIFO_EXCP table*/
        cmd = DRV_IOR(IOC_TABLE, DS_MET_FIFO_EXCP, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip_id, excp_index, cmd, &met_excp));

        met_excp.dest_map = destmap;

        /*write DS_MET_FIFO_EXCP table*/
        cmd = DRV_IOW(IOC_TABLE, DS_MET_FIFO_EXCP, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip_id, excp_index, cmd, &met_excp));

        /*read BUF_RETRV_EXCEPTION_MEM table*/
        cmd = DRV_IOR(IOC_TABLE, BUF_RETRV_EXCEPTION_MEM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip_id, excp_index, cmd, &bufrev_exp));

        bufrev_exp.exception_data = nh_ptr;

        /*write BUF_RETRV_EXCEPTION_MEM table*/
        cmd = DRV_IOW(IOC_TABLE, BUF_RETRV_EXCEPTION_MEM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip_id, excp_index, cmd, &bufrev_exp));
    }

    return CTC_E_NONE;
}

/*
 @brief This function is to initialize the mirror module
*/
int32
sys_humber_mirror_init(void)
{
    uint8 chip_id;
    uint8 lchip_num;
    uint8 session_id;
    uint8 gchip_id;
    uint32 cmd1, cmd2;
    uint32 excp_index;
    uint32 nh_ptr;
    ds_met_fifo_excp_t met_excp;
    buf_retrv_exception_mem_t bufrev_exp;


    if (sys_mirror_initialized)
    {
        return CTC_E_NONE;
    }

    kal_memset(&met_excp, 0, sizeof(ds_met_fifo_excp_t));
    kal_memset(&bufrev_exp, 0, sizeof(buf_retrv_exception_mem_t));
    cmd1 = DRV_IOW(IOC_TABLE, DS_MET_FIFO_EXCP, DRV_ENTRY_FLAG);
    cmd2 = DRV_IOW(IOC_TABLE, BUF_RETRV_EXCEPTION_MEM, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(sys_humber_nh_get_mirror_dsnh_offset(&nh_ptr));

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_get_gchip_id(chip_id, &gchip_id);
        met_excp.dest_map = (gchip_id << 16) |SYS_RESERVED_INTERNAL_PORT_FOR_DROP;
        bufrev_exp.exception_data = nh_ptr;

        for (session_id = 0; session_id < SYS_MIRROR_SESSION_NUM; session_id++)
        {
            /*port session ingress*/
            excp_index = SYS_MIRROR_INGRESS_L2_SPAN_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*port session egress*/
            excp_index = SYS_MIRROR_EGRESS_L2_SPAN_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*vlan session ingress*/
            excp_index = SYS_MIRROR_INGRESS_L3_SPAN_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*vlan session egress*/
            excp_index = SYS_MIRROR_EGRESS_L3_SPAN_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*acl session ingress*/
            excp_index = SYS_MIRROR_INGRESS_ACL_LOG_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*acl session egress*/
            excp_index = SYS_MIRROR_EGRESS_ACL_LOG_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*qos session ingress*/
            excp_index = SYS_MIRROR_INGRESS_QOS_LOG_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));

            /*qos session egress*/
            excp_index = SYS_MIRROR_EGRESS_QOS_LOG_INDEX_BASE + session_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd1, &met_excp));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, excp_index, cmd2, &bufrev_exp));
        }

        /* init copy to cpu for reserve session */
        excp_index = SYS_MIRROR_INGRESS_ACL_LOG_INDEX_BASE + SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX;
        nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                    CTC_EXCEPTION_INGRESS_ACL_LOG0+SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX,0);
        CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, CTC_HUMBER_LPORT_TO_CPU, nh_ptr));

        excp_index = SYS_MIRROR_EGRESS_ACL_LOG_INDEX_BASE + SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX;
        nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                    CTC_EXCEPTION_EGRESS_ACL_LOG0+SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX,0);
        CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, CTC_HUMBER_LPORT_TO_CPU, nh_ptr));

        excp_index = SYS_MIRROR_INGRESS_QOS_LOG_INDEX_BASE + SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX;
        nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                    CTC_EXCEPTION_INGRESS_QOS_LOG0+SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX,0);
        CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, CTC_HUMBER_LPORT_TO_CPU, nh_ptr));

        excp_index = SYS_MIRROR_EGRESS_QOS_LOG_INDEX_BASE + SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX;
        nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                    CTC_EXCEPTION_EGRESS_QOS_LOG0+SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX,0);
        CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, CTC_HUMBER_LPORT_TO_CPU, nh_ptr));



    }

    sys_mirror_initialized = TRUE;

    return CTC_E_NONE;
}

/*
 @brief This function is to set port able to mirror
*/
int32
sys_humber_mirror_set_port_enable(uint16 gport, ctc_direction_t dir, uint8 session_id)
{
    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("gport = %d, direction = %d, span_id = %d\n", gport, dir, session_id);

    if (session_id >= SYS_MIRROR_SESSION_NUM)
    {
        return CTC_E_EXCEED_MIRROR_SESSION;
    }

    CTC_ERROR_RETURN(sys_humber_port_set_span_id(gport, dir, session_id));
    CTC_ERROR_RETURN(sys_humber_port_set_span_en(gport, dir, TRUE));

    return CTC_E_NONE;
}

/*
 @brief This function is to set port disable to mirror
*/
int32
sys_humber_mirror_set_port_disable(uint16 gport, ctc_direction_t dir)
{
    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("gport = %d, direction = %d\n", gport, dir);

    CTC_ERROR_RETURN(sys_humber_port_set_span_en(gport, dir, FALSE));

    return CTC_E_NONE;
}

/*
 @brief This function is to get the information of port mirror
*/
int32
sys_humber_mirror_get_port_info(uint16 gport, ctc_direction_t dir, bool *enable, uint8 *session_id)
{
    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("gport = %d, direction = %d\n", gport, dir);

    CTC_PTR_VALID_CHECK(enable);
    CTC_PTR_VALID_CHECK(session_id);

    CTC_ERROR_RETURN(sys_humber_port_get_span_en(gport, dir, enable));
    CTC_ERROR_RETURN(sys_humber_port_get_span_id(gport, dir, session_id));

    SYS_MIRROR_DBG_INFO("gport = %d, direction = %d, enable = %d, span id is %d\n",
                        gport, dir, *enable, *session_id);

    return CTC_E_NONE;
}

/*
 @brief This function is to set vlan able to mirror
*/
int32
sys_humber_mirror_set_vlan_enable(uint16 vlan_id, ctc_direction_t dir, uint8 session_id)
{
    int32           ret;
    uint16          vlan_if_id;
    sys_vlan_info_t vlan_info;

    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("vlan_id = %d, direction = %d, span_id = %d\n", vlan_id, dir, session_id);

    kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));

    if (session_id >= SYS_MIRROR_SESSION_NUM)
    {
        return CTC_E_EXCEED_MIRROR_SESSION;
    }
    CTC_VLAN_RANGE_CHECK(vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;
    ret = sys_humber_vlan_get_l3if_id(&vlan_info,&vlan_if_id);

    if ((dir == CTC_INGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_PROP_L3SPAN_ID, session_id);
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_PROP_L3SPAN_EN, 1);
    }

    if ((dir == CTC_EGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_EGS_PROP_SPAN_ID, session_id);
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_EGS_PROP_SPAN_EN, 1);
    }

    return ret;

}

/*
 @brief This function is to set vlan disable to mirror
*/
int32
sys_humber_mirror_set_vlan_disable(uint16 vlan_id, ctc_direction_t dir)
{
    int32           ret = 0;
    uint16          vlan_if_id;
    sys_vlan_info_t vlan_info;

    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("vlan_id = %d, direction = %d\n", vlan_id, dir);

    CTC_VLAN_RANGE_CHECK(vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    ret = sys_humber_vlan_get_l3if_id(&vlan_info, &vlan_if_id);

    if ((dir == CTC_INGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_PROP_L3SPAN_EN, 0);
    }

    if ((dir == CTC_EGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        ret = ret? ret:sys_humber_l3if_set_span_property(vlan_if_id, SYS_L3IF_EGS_PROP_SPAN_EN, 0);
    }
    return ret;
}

/*
 @brief This function is to get the information of vlan mirror
*/
int32
sys_humber_mirror_get_vlan_info(uint16 vlan_id, ctc_direction_t dir, bool *enable, uint8 *session_id)
{
    int32           ret;
    uint16          vlan_if_id;
    uint32          value = 0;
    sys_vlan_info_t vlan_info;

    SYS_MIRROR_INIT_CHECK();

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("vlan_id = %d, direction = %d\n", vlan_id, dir);

    CTC_VLAN_RANGE_CHECK(vlan_id);
    CTC_PTR_VALID_CHECK(enable);
    CTC_PTR_VALID_CHECK(session_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;
    ret = sys_humber_vlan_get_l3if_id(&vlan_info, &vlan_if_id);

    if(dir == CTC_INGRESS)
    {
        ret = ret? ret:sys_humber_l3if_get_span_property(vlan_if_id, SYS_L3IF_PROP_L3SPAN_ID, &value);
        *session_id = value;
        ret = ret? ret:sys_humber_l3if_get_span_property(vlan_if_id, SYS_L3IF_PROP_L3SPAN_EN, &value);
        *enable = value;
    }
    else if(dir == CTC_EGRESS)
    {
        ret = ret? ret:sys_humber_l3if_get_span_property(vlan_if_id, SYS_L3IF_EGS_PROP_SPAN_ID,  &value);
        *session_id = value;
        ret = ret? ret:sys_humber_l3if_get_span_property(vlan_if_id, SYS_L3IF_EGS_PROP_SPAN_EN, &value);
        *enable = value;
    }
    else
    {
        return CTC_E_INVALID_DIR;
    }

    return ret;

}

/*
 @brief This function is to set local mirror destination port
*/
int32
sys_humber_mirror_set_dest(ctc_mirror_dest_t *mirror)
{
    uint16 dest_lport = 0;
    uint32 nh_ptr = 0;
    uint32 excp_index = 0;

    SYS_MIRROR_INIT_CHECK();
    CTC_PTR_VALID_CHECK(mirror);

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("dest_gport = %d, direction = %d, session_id = %d\n",
                        mirror->dest_gport, mirror->dir, mirror->session_id);

    if (mirror->session_id >= SYS_MIRROR_SESSION_NUM)
    {
        return CTC_E_EXCEED_MIRROR_SESSION;
    }
    CTC_GLOBAL_PORT_CHECK(mirror->dest_gport);

    dest_lport = CTC_MAP_GPORT_TO_LPORT(mirror->dest_gport);
    CTC_ERROR_RETURN(sys_humber_nh_get_mirror_dsnh_offset(&nh_ptr));

    /*port mirror set dest*/
    if (mirror->type == CTC_MIRROR_L2SPAN_SESSION)
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_L2_SPAN_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {

                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_INGRESS_L2_SPAN0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("ingress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }

        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_L2_SPAN_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {
                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_EGRESS_L2_SPAN0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("egress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }

    }
    else if (mirror->type == CTC_MIRROR_L3SPAN_SESSION)  /*vlan mirror set dest*/
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_L3_SPAN_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {
               nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_INGRESS_L3_SPAN0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("ingress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }
        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_L3_SPAN_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {
               nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_EGRESS_L3_SPAN0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("egress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);

            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }
    }
    else if (mirror->type == CTC_MIRROR_ACLLOG_SESSION)  /*acllog mirror set dest*/
    {
        CTC_MAX_VALUE_CHECK(mirror->session_id, SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX-1);
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_ACL_LOG_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {

                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_INGRESS_ACL_LOG0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("ingress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }

        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_ACL_LOG_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {
                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_EGRESS_ACL_LOG0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("egress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }
    }
    else if (mirror->type == CTC_MIRROR_QOSLOG_SESSION)  /*qoslog mirror set dest*/
    {
        CTC_MAX_VALUE_CHECK(mirror->session_id, SYS_MIRROR_RESERVE_ACLQOS_LOG_INDEX-1);
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_QOS_LOG_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {

                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_INGRESS_QOS_LOG0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("ingress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }

        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_QOS_LOG_INDEX_BASE + mirror->session_id;

            if (CTC_HUMBER_LPORT_TO_CPU == dest_lport)
            {
                nh_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,
                                      CTC_EXCEPTION_EGRESS_QOS_LOG0 + mirror->session_id,0);
            }

            SYS_MIRROR_DBG_INFO("egress : excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_ptr);
            CTC_ERROR_RETURN(_sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_ptr));
        }
    }

    return CTC_E_NONE;
}

/*
 @brief This function is to set remote mirror destination port
*/
int32
sys_humber_mirror_rspan_set_dest(ctc_mirror_dest_t *mirror, uint32 nh_id)
{
    uint32 nh_offset = 0;
    uint32 excp_index = 0;
    uint8 use_dsnh8w = 0;
    uint8 aps_en = 0;
    uint16 temp_gport = 0;
    uint32 ret = CTC_E_NONE;

    SYS_MIRROR_INIT_CHECK();
    CTC_PTR_VALID_CHECK(mirror);

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("dest_gport = %d, direction = %d, session_id = %d, nh_id = %d",
                        mirror->dest_gport, mirror->dir, mirror->session_id, nh_id);

    if (mirror->session_id >= SYS_MIRROR_SESSION_NUM)
    {
        return CTC_E_EXCEED_MIRROR_SESSION;
    }
    CTC_ERROR_RETURN(sys_humber_nh_get_dsnh_offset_by_nhid(nh_id, &nh_offset,&use_dsnh8w));
    ret = sys_humber_nh_get_port(nh_id, &aps_en,&temp_gport);
    if( (!ret) && (temp_gport != CTC_MAX_UINT16_VALUE) )
    {
        mirror->dest_gport = temp_gport;
    }
    CTC_GLOBAL_PORT_CHECK(mirror->dest_gport);

    /*port mirror*/
    if (mirror->type == CTC_MIRROR_L2SPAN_SESSION)
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_L2_SPAN_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_L2_SPAN_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
    }
    else if (mirror->type == CTC_MIRROR_L3SPAN_SESSION)   /*vlan mirror*/
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_L3_SPAN_INDEX_BASE +  mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_L3_SPAN_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_ptr = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
    }
    else if (mirror->type == CTC_MIRROR_ACLLOG_SESSION)  /*acllog mirror set dest*/
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_ACL_LOG_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_ACL_LOG_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
    }
    else if (mirror->type == CTC_MIRROR_QOSLOG_SESSION)  /*qoslog mirror set dest*/
    {
        if ((CTC_INGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_INGRESS_QOS_LOG_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
        if ((CTC_EGRESS == mirror->dir) || (CTC_BOTH_DIRECTION == mirror->dir))
        {
            excp_index = SYS_MIRROR_EGRESS_QOS_LOG_INDEX_BASE + mirror->session_id;

            SYS_MIRROR_DBG_INFO("excp_index = %d, nh_offset = 0x%x\n", excp_index, nh_offset);

            CTC_ERROR_RETURN(
            _sys_humber_mirror_set_dest_write_chip(excp_index, mirror->dest_gport, nh_offset));

        }
    }

    return CTC_E_NONE;

}

int32
sys_humber_mirror_rspan_escape_en(bool enable)
{
    uint8 chip_id;
    uint8 lchip_num;
    uint32 cmd;
    uint32 field_val = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_REG, EPE_PKT_PROC_CTL, EPE_PKT_PROC_CTL_MIRROR_ESCAPE_CAM_EN);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &field_val));
    }

    return CTC_E_NONE;
}

int32
sys_humber_mirror_rspan_escape_mac(ctc_mirror_rspan_escape_t escape)
{
    uint8 chip_id;
    uint8 lchip_num;
    uint32 cmd;
    epe_mirror_escape_cam_t mirror_escape;

    mirror_escape.mac_da_value00    = (escape.mac0[2] << 24) | (escape.mac0[3] << 16)
                                    | (escape.mac0[4] << 8) | escape.mac0[5];
    mirror_escape.mac_da_value01    = (escape.mac0[0] << 8) | escape.mac0[1];

    mirror_escape.mac_da_mask00     = (escape.mac_mask0[2] << 24) | (escape.mac_mask0[3] << 16)
                                    | (escape.mac_mask0[4] << 8) | escape.mac_mask0[5];
    mirror_escape.mac_da_mask01     = (escape.mac_mask0[0] << 8) | (escape.mac_mask0[1]);


    mirror_escape.mac_da_value10    = (escape.mac1[2] << 24) | (escape.mac1[3] << 16)
                                    | (escape.mac1[4] << 8) | escape.mac1[5];
    mirror_escape.mac_da_value11    = (escape.mac1[0] << 8) | escape.mac1[1];

    mirror_escape.mac_da_mask10     = (escape.mac_mask1[2] << 24) | (escape.mac_mask1[3] << 16)
                                    | (escape.mac_mask1[4] << 8) | escape.mac_mask1[5];
    mirror_escape.mac_da_mask11     = (escape.mac_mask1[0] << 8) | (escape.mac_mask1[1]);

    cmd = DRV_IOW(IOC_REG, EPE_MIRROR_ESCAPE_CAM, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mirror_escape));
    }

    return CTC_E_NONE;
}

/*
 @brief This function is to remove mirror destination port
*/
int32
sys_humber_mirror_unset_dest(ctc_mirror_dest_t *mirror)
{
    SYS_MIRROR_INIT_CHECK();
    CTC_PTR_VALID_CHECK(mirror);

    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("session_id = %d, direction = %d, session type = %d\n",
                        mirror->session_id, mirror->dir, mirror->type);

    mirror->dest_gport = SYS_RESERVED_INTERNAL_PORT_FOR_DROP;
    CTC_ERROR_RETURN(sys_humber_mirror_set_dest(mirror));

    return CTC_E_NONE;
}

/*
  @brief This functions is used to set packet enable or not to log if the packet is discarded on EPE process.
*/
int32
sys_humber_mirror_set_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool enable)
{
    uint32 cmd;
    uint32 logon_discard;
    uint8 chip_id;
    uint8 local_chip_num;

    SYS_MIRROR_INIT_CHECK();
    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("discard_flag = %d, direction = %d, enable = %d\n", discard_flag, dir, enable);
    SYS_MIRROR_DISCARD_CHECK(discard_flag);

    local_chip_num = sys_humber_get_local_chip_num();

    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        for(chip_id = 0; chip_id < local_chip_num; chip_id++)
        {
            cmd = DRV_IOR(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_LOG_ON_DISCARD);
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &logon_discard));

            if (TRUE == enable)
            {
                CTC_SET_FLAG(logon_discard, discard_flag);
            }
            else
            {
                CTC_UNSET_FLAG(logon_discard, discard_flag);
            }

            cmd = DRV_IOW(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_LOG_ON_DISCARD);
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &logon_discard));
        }
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        for(chip_id = 0; chip_id < local_chip_num; chip_id++)
        {
            cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_LOG_ON_DISCARD);
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &logon_discard));

            if (TRUE == enable)
            {
                CTC_SET_FLAG(logon_discard, discard_flag);
            }
            else
            {
                CTC_UNSET_FLAG(logon_discard, discard_flag);
            }

            cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_LOG_ON_DISCARD);
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &logon_discard));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_mirror_get_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool *enable)
{
    uint32 cmd;
    uint32 field_val;

    SYS_MIRROR_INIT_CHECK();
    SYS_MIRROR_DBG_FUNC();
    SYS_MIRROR_DBG_INFO("discard_flag = %d, direction = %d\n", discard_flag, dir);

    CTC_PTR_VALID_CHECK(enable);
    SYS_MIRROR_DISCARD_CHECK(discard_flag);

    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_LOG_ON_DISCARD);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_LOG_ON_DISCARD);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &field_val));

    *enable = CTC_FLAG_ISSET(field_val, discard_flag)? TRUE:FALSE;

    return CTC_E_NONE;
}



