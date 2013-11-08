/**
 @file sys_humber_internal_port.c

 @date 2010-03-29

 @version v2.0

*/

/****************************************************************************
 *  current queue allocation scheme (Alt + F12 for pretty):
 *
 *           type          port id    channel id      queue number    queue id range
 *     ----------------  ----------   ----------      ------------    --------------
 *      internal port     64 - 191       X              4 * 128        708 - 1219

 ****************************************************************************/


/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ctc_debug.h"
#include "ctc_macro.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "ctc_const.h"

#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_l3if.h"
#include "sys_humber_port.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_sch.h"
#include "sys_humber_internal_port.h"
#include "drv_humber.h"
#include "drv_io.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
extern sys_queue_enq_ctl_t sys_enq_ctl;
sys_inter_port_master_t *inter_port_master = NULL;

#define SYS_INTERNAL_PORT_INIT_CHECK() \
    {\
        if(  inter_port_master == NULL)\
            return CTC_E_NOT_INIT;\
    }

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 @brief Set internal port for special usage, for example, I-Loop, E-Loop.
*/
int32
sys_humber_internal_port_set(ctc_internal_port_assign_para_t* port_assign)
{
    uint8 lchip = 0, lchip1 = 0, lport = 0;
    uint16 queue_id = 0;
    uint32 offset = 0;
    ctc_queue_drop_t queue_drop;

    SYS_QUEUE_DBG_FUNC();
    SYS_INTERNAL_PORT_INIT_CHECK();

    /*param judge*/
    CTC_PTR_VALID_CHECK(port_assign);
    CTC_MAX_VALUE_CHECK(port_assign->type, CTC_INTERNAL_PORT_TYPE_FWD);

    kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));

    if(TRUE != sys_humber_chip_is_local(port_assign->gchip, &lchip))
    {
        return CTC_E_INVALID_PARAM;
    }

    if(port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
    {
        SYS_MAP_GPORT_TO_LPORT(port_assign->fwd_gport, lchip1, lport);

        if(lchip1 != lchip)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    CTC_VALUE_RANGE_CHECK(port_assign->inter_port, SYS_HUMBER_INTERNAL_PORT_START, sys_enq_ctl.max_internal_port_id);

    /*judge if internal port is used*/
    if(CTC_IS_BIT_SET(inter_port_master->is_used[lchip][port_assign->inter_port/BITS_NUM_OF_WORD], port_assign->inter_port%BITS_NUM_OF_WORD))
    {
        /*is used*/
        return CTC_E_INTERNAL_PORT_IN_USE;
    }
    else
    {
        inter_port_master->is_used[lchip][port_assign->inter_port/BITS_NUM_OF_WORD] |= (1 << (port_assign->inter_port%BITS_NUM_OF_WORD));
    }

    for (offset = 0; offset < sys_enq_ctl.queue_num_per_internal_port; offset++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_INTERNAL_PORT, port_assign->inter_port, offset, &queue_id));

        if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ILOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_ILOOP_CHANNEL_ID));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ELOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_ELOOP_CHANNEL_ID));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, lport));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else /*discard*/
        {
            kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));
            queue_drop.mode = CTC_QUEUE_DROP_WTD;

            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_DROP_CHANNEL_ID_START));
        }

        CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, offset % sys_enq_ctl.queue_num_per_internal_port));

    }

    return CTC_E_NONE;
}


/**
 @brief Allocate internal port for special usage, for example, I-Loop, E-Loop.
*/
int32
sys_humber_internal_port_allocate(ctc_internal_port_assign_para_t* port_assign)
{
    uint8 lchip = 0, lchip1 = 0, lport = 0;
    uint32 offset = 0;
    uint16 queue_id = 0;
    uint8 i = 0;
    ctc_queue_drop_t queue_drop;

    SYS_QUEUE_DBG_FUNC();
    SYS_INTERNAL_PORT_INIT_CHECK();
    CTC_PTR_VALID_CHECK(port_assign);
    CTC_MAX_VALUE_CHECK(port_assign->type, CTC_INTERNAL_PORT_TYPE_FWD);

    kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));

    if(TRUE != sys_humber_chip_is_local(port_assign->gchip, &lchip))
    {
        return CTC_E_INVALID_PARAM;
    }

    if(port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
    {
        SYS_MAP_GPORT_TO_LPORT(port_assign->fwd_gport, lchip1, lport);

        if(lchip1 != lchip)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    /*alloc a free internal port*/
    for(i=SYS_HUMBER_INTERNAL_PORT_START; i<(SYS_HUMBER_INTERNAL_PORT_START+sys_enq_ctl.internal_port_num); i++)
    {
        if(CTC_IS_BIT_SET(inter_port_master->is_used[lchip][i/BITS_NUM_OF_WORD], i%BITS_NUM_OF_WORD))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if(i == SYS_HUMBER_INTERNAL_PORT_START+sys_enq_ctl.internal_port_num)
    {
        return CTC_E_NO_FREE_INTERNAL_PORT;
    }

    port_assign->inter_port = i;

    for (offset = 0; offset < sys_enq_ctl.queue_num_per_internal_port; offset++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_INTERNAL_PORT, i, offset, &queue_id));

        if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ILOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_ILOOP_CHANNEL_ID));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ELOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_ELOOP_CHANNEL_ID));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_default_drop(SYS_DEFAULT_DROP_GE, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, lport));
            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_DISCARD)
        {
            kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));
            queue_drop.mode = CTC_QUEUE_DROP_WTD;

            CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_DROP_CHANNEL_ID_START));
        }
        else
        {
            return CTC_E_INVALID_PARAM;
        }

        CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, offset % sys_enq_ctl.queue_num_per_internal_port));

    }

    inter_port_master->is_used[lchip][i/BITS_NUM_OF_WORD] |= (1 << (i%BITS_NUM_OF_WORD));

    return CTC_E_NONE;
}


/**
 @brief Release internal port.
*/
int32
sys_humber_internal_port_release(ctc_internal_port_assign_para_t* port_assign)
{
    uint8 lchip = 0, lchip1 = 0, lport = 0;
    uint16 queue_id = 0;
    uint32 offset = 0;

    SYS_QUEUE_DBG_FUNC();
    SYS_INTERNAL_PORT_INIT_CHECK();
    CTC_PTR_VALID_CHECK(port_assign);
    CTC_MAX_VALUE_CHECK(port_assign->type, CTC_INTERNAL_PORT_TYPE_FWD);

    if(TRUE != sys_humber_chip_is_local(port_assign->gchip, &lchip))
    {
        return CTC_E_INVALID_PARAM;
    }

    if(port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
    {
        SYS_MAP_GPORT_TO_LPORT(port_assign->fwd_gport, lchip1, lport);

        if(lchip1 != lchip)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    CTC_VALUE_RANGE_CHECK(port_assign->inter_port, SYS_HUMBER_INTERNAL_PORT_START, sys_enq_ctl.max_internal_port_id);

    if(!(CTC_IS_BIT_SET(inter_port_master->is_used[lchip][port_assign->inter_port/BITS_NUM_OF_WORD], port_assign->inter_port%BITS_NUM_OF_WORD)))
    {
        return CTC_E_INVALID_PARAM;
    }

    for (offset = 0; offset < sys_enq_ctl.queue_num_per_internal_port; offset++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_INTERNAL_PORT, port_assign->inter_port, offset, &queue_id));

        if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ILOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, SYS_ILOOP_CHANNEL_ID));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_ELOOP)
        {
            CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, SYS_ELOOP_CHANNEL_ID));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_FWD)
        {
            CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, lport));
        }
        else if (port_assign->type == CTC_INTERNAL_PORT_TYPE_DISCARD)
        {
            CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, SYS_DROP_CHANNEL_ID_START));
        }
        else
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    inter_port_master->is_used[lchip][port_assign->inter_port/BITS_NUM_OF_WORD] &= ~(1 << (port_assign->inter_port%BITS_NUM_OF_WORD));

    return CTC_E_NONE;
}


int32
sys_humber_get_rsv_internal_port(uint8 lchip, uint8* p_inter_port)
{
    uint8 index = 0;

    for (index = 0; index < SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_NUM; index++)
    {
        if (CTC_IS_BIT_SET(inter_port_master->rsv_port_bitmap[lchip], index))
        {
           continue;
        }
        *p_inter_port = SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_START + index;
        inter_port_master->rsv_port_bitmap[lchip] |= 1 << index;

        return CTC_E_NONE;
    }

    return CTC_E_NO_OFFSET_LEFT;
}


int32
sys_humber_free_rsv_internal_port(uint8 lchip, uint8 inter_port)
{
   uint8 index = 0;
   uint8 rsv_port_start = SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_START;
   uint8 rsv_port_end   = SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_START + \
                               SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_NUM ;

   if (inter_port < rsv_port_start && inter_port > rsv_port_end)
   {
         return CTC_E_INVALID_PARAM;
   }

   index = inter_port - SYS_HUMBER_STATIC_RESERVED_INTERNAL_PORT_START;

   if (!CTC_IS_BIT_SET(inter_port_master->rsv_port_bitmap[lchip], index))
   {
       return CTC_E_NO_ALLOC_OFFSET;
   }

   inter_port_master->rsv_port_bitmap[lchip] &= ~(1 << index);

   return CTC_E_NONE;
}
static INLINE int32
_sys_humber_internal_port_eloop_port_init(uint8 lchip)
{
    uint8 gchip = 0;
    uint8 eloop_lport = 0;
    uint16 eloop_gport = 0;
    uint32 cmd =  0;
    ctc_l3if_t l3if;
    uint16 l3if_id = 0;
    uint8 iloop_lport = 0;
    uint16 iloop_gport = 0;
    uint32 field_val = 0;
    epe_hdr_edit_l2_edit_loopback_ram_t edit_lbk;
    ctc_internal_port_assign_para_t port_assign;

    kal_memset(&edit_lbk, 0, sizeof(epe_hdr_edit_l2_edit_loopback_ram_t));
    kal_memset(&l3if, 0, sizeof(ctc_l3if_t));
    kal_memset(&port_assign, 0, sizeof(ctc_internal_port_assign_para_t));


    sys_humber_get_gchip_id(lchip, &gchip);
    port_assign.gchip = gchip;
    /*for eloop-back for route*/
    port_assign.type = CTC_INTERNAL_PORT_TYPE_ELOOP;
    CTC_ERROR_RETURN(sys_humber_internal_port_allocate(&port_assign));
    eloop_lport = port_assign.inter_port;
    eloop_gport = CTC_MAP_LPORT_TO_GPORT(gchip, eloop_lport);
    inter_port_master->eloop_port[lchip][CTC_INTERNAL_PORT_ELOOP_TYPE_ROUTE] = eloop_gport;

    /*for iloop-back for route*/
    port_assign.type = CTC_INTERNAL_PORT_TYPE_ILOOP;
    CTC_ERROR_RETURN(sys_humber_internal_port_allocate(&port_assign));
    iloop_lport = port_assign.inter_port;
    iloop_gport = CTC_MAP_LPORT_TO_GPORT(gchip, iloop_lport);

    edit_lbk.lb_dest_map = gchip<<16 |SYS_ILOOP_CHANNEL_ID;
    edit_lbk.lb_next_hop_ext = 0;;
    edit_lbk.lb_next_hop_ptr = iloop_lport;

    cmd = DRV_IOW(IOC_TABLE, EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, eloop_lport, cmd, &edit_lbk));

    /*set eloop port property*/

    l3if_id = (SYS_L3IF_RSV_L3IF_ID_FOR_ELOOP_PORT + lchip);
    l3if.gport = eloop_gport;
    l3if.l3if_type = CTC_L3IF_TYPE_PHY_IF;
    CTC_ERROR_RETURN(sys_humber_l3if_create(l3if_id, &l3if));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV4_UCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV6_UCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV4_MCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV6_MCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_TANSMIT_EN,1));
    CTC_ERROR_RETURN(sys_humber_port_set_transmit_en(eloop_gport, TRUE));
    CTC_ERROR_RETURN(sys_humber_port_set_phy_if_en(eloop_gport, TRUE));
    CTC_ERROR_RETURN(sys_humber_port_set_replace_dscp_en(eloop_gport, TRUE));


    /*enable eloop for port*/
    field_val = 3;
    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_MUX_PORT_TYPE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, eloop_lport, cmd, &field_val));

    /*set iloop port property*/

    l3if_id = (SYS_L3IF_RSV_L3IF_ID_FOR_ILOOP_PORT + lchip);
    l3if.gport = iloop_gport;
    l3if.l3if_type = CTC_L3IF_TYPE_PHY_IF;
    CTC_ERROR_RETURN(sys_humber_l3if_create(l3if_id, &l3if));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV4_UCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV6_UCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV4_MCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_IPV6_MCAST,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_ROUTE_ALL_PKT,1));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id,CTC_L3IF_PROP_RECEIVE_EN,1));
    CTC_ERROR_RETURN(sys_humber_port_set_receive_en(iloop_gport, TRUE));
    CTC_ERROR_RETURN(sys_humber_port_set_phy_if_en(iloop_gport, TRUE));

    /*trust outer priority*/
    CTC_ERROR_RETURN(sys_humber_port_set_qos_policy(iloop_gport, 1));

    return CTC_E_NONE;
}

int32
sys_humber_internal_port_get_eloop_port(uint8 type, uint8 lchip, uint16 *p_gport)
{

    if (type >= CTC_INTERNAL_PORT_ELOOP_TYPE_MAX)
    {
        return CTC_E_INVALID_PARAM;
    }

    *p_gport = inter_port_master->eloop_port[lchip][type];

    return CTC_E_NONE;
}


/**
 @brief Internal port initialization.
*/
int32
sys_humber_internal_port_init(void)
{
    uint8 chip_id, lchip_num;

    if (NULL != inter_port_master)
    {
        return CTC_E_NONE;
    }

    MALLOC_POINTER(sys_inter_port_master_t, inter_port_master);
    if (NULL == inter_port_master)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(inter_port_master, 0, sizeof(sys_inter_port_master_t));

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(_sys_humber_internal_port_eloop_port_init(chip_id));
    }

    return CTC_E_NONE;
}


