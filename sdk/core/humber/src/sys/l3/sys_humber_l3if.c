/**
 @file sys_humber_l3if.c

 @date 2009-12-7

 @version v2.0

---file comments----
*/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_debug.h"
#include "ctc_const.h"
#include "sys_humber_l3if.h"
#include "sys_humber_ftm.h"
#include "sys_humber_chip.h"
#include "sys_humber_linkagg.h"
#include "sys_humber_vlan.h"
#include "sys_humber_usrid.h"
#include "sys_humber_pdu.h"
#include "drv_humber.h"
#include "drv_io.h"


/**********************************************************************************
                        Defines and Macros
***********************************************************************************/
#define SYS_ROUTER_MAC_LABEL_INDEX       63
#define SYS_MIN_VMAC_MAC_LABEL_INDEX    0
#define SYS_MAX_VMAC_MAC_LABEL_INDEX   62
#define SYS_MAX_VMAC_MAC_LABEL_NUM     64

#define SYS_L3IF_VAILD_VALUE      1
#define SYS_L3IF_INVAILD_VALUE   (!SYS_L3IF_VAILD_VALUE)

#define SYS_L3IF_LOCK \
    if (p_l3if_master->p_mutex) kal_mutex_lock(p_l3if_master->p_mutex)
#define  SYS_L3IF_UNLOCK \
    if (p_l3if_master->p_mutex) kal_mutex_unlock(p_l3if_master->p_mutex)



 #define SYS_L3IF_INIT_CHECK() \
    {\
        if(  p_l3if_master == NULL)\
            return CTC_E_NOT_INIT;\
    }

#define SYS_L3IF_CREATE_CHECK(l3if_id)\
    if ( SYS_L3IF_INVAILD_VALUE == p_l3if_master->l3if_prop[l3if_id].vaild)\
    {\
      return CTC_E_L3IF_NOT_EXIST;\
    }

#define SYS_L3IFID_VAILD_CHECK(l3if_id)\
      if (l3if_id > MAX_CTC_L3IF_ID )\
      {\
          return CTC_E_L3IF_INVALID_IF_ID;\
      }

#define SYS_L3IFID_FORPHYSUB_VAILD_CHECK(l3if_id)\
      if (l3if_id > MAX_CTC_L3IF_ID_FOR_PHY_SUB_IF )\
      {\
          return CTC_E_L3IF_INVALID_IF_ID;\
      }

struct sys_l3if_prop_s
{
  uint16  vlan_id;
  uint16  gport;
  uint8   vaild;
  uint8   router_mac_label;
  uint8   l3if_type;   /**< the type of l3interface , CTC_L3IF_TYPE_XXX */
  uint8   reserved1;   /**< reserved */
  uint16  vlan_ptr;   /**< Vlanptr */
  uint16  reserved2;   /**< reserved */

};
typedef struct sys_l3if_prop_s sys_l3if_prop_t;

struct sys_l3if_vmac_entry_s
{
    uint8 vaild;
    uint8 rsv0;
    uint8 router_mac1_type;
    uint8 router_mac0_type;
    uint8 router_mac2_type;
    uint8 router_mac3_type;
}
;
typedef struct sys_l3if_vmac_entry_s sys_l3if_vmac_entry_t;

struct sys_l3if_master_s
{
    sys_l3if_prop_t  l3if_prop[MAX_CTC_L3IF_ID+1];
    sys_l3if_vmac_entry_t  vmac_entry[SYS_MAX_VMAC_MAC_LABEL_INDEX + 1];
    kal_mutex_t    *p_mutex;

};
typedef struct sys_l3if_master_s sys_l3if_master_t;


/****************************************************************************
               Global and Declaration
*****************************************************************************/

sys_l3if_master_t  *p_l3if_master = NULL;


/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/

/**
 @brief    init l3 interface module
*/
int32
sys_humber_l3if_init(void)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint32 index = 0;
    uint32 write_cmd = 0;
    uint32 src_if_cmd = 0;
    uint32 dest_if_cmd = 0;
    ds_src_interface_t src_interface;
    ipe_router_mac_ctl_t rt_mac_ctl;
    ds_router_mac_t rt_mac;
    ds_dest_interface_t dest_interface;
    epe_l2_router_mac_sa_t l2rt_macsa;

    kal_memset(&src_interface, 0, sizeof(src_interface));
    kal_memset(&dest_interface, 0, sizeof(dest_interface));
    kal_memset(&rt_mac, 0, sizeof(rt_mac));

    if (p_l3if_master != NULL)
    {
        return CTC_E_NONE;
    }
    p_l3if_master = (sys_l3if_master_t *)mem_malloc(MEM_L3IF_MODULE, sizeof(sys_l3if_master_t));

    if (NULL == p_l3if_master)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_l3if_master, 0, sizeof(sys_l3if_master_t));

    ret = kal_mutex_create(&(p_l3if_master->p_mutex));
    if (ret || !(p_l3if_master->p_mutex))
    {
        mem_free(p_l3if_master);
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
    rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
    rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
    rt_mac.router_mac3_type  = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
    src_interface.router_mac_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID; /*multiple router mac*/
    src_interface.router_mac_label = SYS_ROUTER_MAC_LABEL_INDEX;
    src_interface.lookup_mode = FALSE;
    /*#define  SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU      15 */
   /*#define  SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_ARPDHCP_TO_CPU 14 */


    src_interface.exception3_en =   1<<SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_DHCP_TO_CPU ;   /*bit 14 always set to 1, 14  reserved for arp&dhcp  to cpu */
    src_interface.exception3_en |= 1<<SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU;  /*bit 15 always set to 1, 15 reserved for normal IPDA copy_to_cpu */
    dest_interface.mac_sa_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC;

    write_cmd = DRV_IOW(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
    src_if_cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
    dest_if_cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        for (index = 0; index <= SYS_MAX_VMAC_MAC_LABEL_INDEX; index++ )
        {
             rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC;
             rt_mac.router_mac0_byte = 0;
             rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
             rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
             rt_mac.router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
             CTC_ERROR_RETURN( drv_tbl_ioctl(lchip, index, write_cmd, &rt_mac));
        }
       rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC;
       rt_mac.router_mac0_byte = 0;
       rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
       rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
       rt_mac.router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
       CTC_ERROR_RETURN( drv_tbl_ioctl(lchip, SYS_ROUTER_MAC_LABEL_INDEX, write_cmd, &rt_mac));
        for (index = 0; index <= MAX_CTC_L3IF_ID; index++ )
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, src_if_cmd , &src_interface));
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, dest_if_cmd , &dest_interface));
        }
    }

    kal_memset(&l2rt_macsa, 0, sizeof(epe_l2_router_mac_sa_t));
    write_cmd = DRV_IOW(IOC_REG, EPE_L2_ROUTER_MAC_SA, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
       CTC_ERROR_RETURN( drv_reg_ioctl(lchip, 0, write_cmd, &l2rt_macsa));

    }

    kal_memset(&rt_mac_ctl, 0, sizeof(ipe_router_mac_ctl_t));
    write_cmd = DRV_IOW(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, write_cmd, &rt_mac_ctl));
    }

    for (index = 0; index <= MAX_CTC_L3IF_ID; index++ )
    {
        p_l3if_master->l3if_prop[index].router_mac_label = SYS_ROUTER_MAC_LABEL_INDEX;
        p_l3if_master->l3if_prop[index].vaild = SYS_L3IF_INVAILD_VALUE;
        p_l3if_master->l3if_prop[index].vlan_ptr = SYS_VLAN_INVALID_PTR;
    }

    return CTC_E_NONE;

}


/**
   @brief  Create l3 interfaces,gport should be global logical port
 */
int32
sys_humber_l3if_create(uint16 l3if_id, ctc_l3if_t* l3_if)
{
    sys_vlan_info_t vlan_info;
    sys_usrid_vlan_entry_t  userid_vlan;
    uint16  vlan_ptr = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 lchip_num = sys_humber_get_local_chip_num();
    int32 ret = 0;

    SYS_L3IF_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l3_if);
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    if (SYS_L3IF_VAILD_VALUE == p_l3if_master->l3if_prop[l3if_id].vaild)
    {
        return CTC_E_L3IF_EXIST;
    }

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_type:%d ,l3if_id :%d port:0x%x, vlan_id:%d \n",
    l3_if->l3if_type,l3if_id,l3_if->gport,l3_if->vlan_id);

    switch(l3_if->l3if_type)
    {
        case CTC_L3IF_TYPE_PHY_IF:
            {
                SYS_L3IFID_FORPHYSUB_VAILD_CHECK(l3if_id);
                CTC_GLOBAL_PORT_CHECK(l3_if->gport);
                if (!CTC_IS_LINKAGG_PORT(l3_if->gport))
                {
                    SYS_MAP_GPORT_TO_LPORT(l3_if->gport, lchip, lport);
                }

                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
                vlan_info.vid = 0xFFFF;
                vlan_info.gport = l3_if->gport;
                vlan_info.if_id = l3if_id;

                ret = sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);

                if (SYS_VLAN_INVALID_PTR == vlan_ptr)
                {
                    ret = sys_humber_vlan_create_vlan(&vlan_info);
                }
                else
                {
                    /*exists ifid on this routed port*/
                    return CTC_E_L3IF_MISMATCH;
                }

                ret = ret?ret:sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
                if (vlan_ptr == SYS_VLAN_INVALID_PTR)
                {
                    return CTC_E_L3IF_ROUTED_EXCEED_IF_SIZE;
                }

                ret = ret?ret:sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
                ret = ret?ret:sys_humber_vlan_set_l3if_id(&vlan_info, l3if_id);
                ret = ret?ret:sys_humber_vlan_set_ether_oam_valid(&vlan_info, 1);
                ret = ret?ret:sys_humber_vlan_set_learning_en(&vlan_info, FALSE);
                ret = ret?ret:sys_humber_vlan_set_egress_ether_oam_valid(&vlan_info, 1);
                ret = ret?ret:sys_humber_vlan_set_md_level(&vlan_info, 0);

                break;
            }

        case CTC_L3IF_TYPE_VLAN_IF:
            {
                uint16 tmp_l3if_id = 0;
                CTC_VLAN_RANGE_CHECK(l3_if->vlan_id);
                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
                vlan_info.vid = l3_if->vlan_id;

                CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr));
                ret = ret? ret:sys_humber_vlan_get_l3if_id(&vlan_info, &tmp_l3if_id);

                if(ret == CTC_E_NONE
                && (tmp_l3if_id != CTC_L3IF_INVALID_L3IF_ID)
                && (tmp_l3if_id != l3if_id))
                {
                    ret = CTC_E_L3IF_EXIST;
                }
                else
                {
                    ret = sys_humber_vlan_set_l3if_id(&vlan_info, l3if_id);
                }

                break;
            }

        case CTC_L3IF_TYPE_SUB_IF:
            {
                SYS_L3IFID_FORPHYSUB_VAILD_CHECK(l3if_id);
                CTC_GLOBAL_PORT_CHECK(l3_if->gport);
                CTC_VLAN_RANGE_CHECK(l3_if->vlan_id);

                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
                vlan_info.vid = l3_if->vlan_id;
                vlan_info.gport = l3_if->gport;
                vlan_info.if_id = l3if_id;

                ret = sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
                if(SYS_VLAN_INVALID_PTR == vlan_ptr)
                {
                    ret = sys_humber_vlan_create_vlan(&vlan_info);
                }
                else
                {
                    /*exist ifid on sub_if*/
                    return CTC_E_L3IF_MISMATCH;
                }

                ret = ret? ret:sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
                if (vlan_ptr == SYS_VLAN_INVALID_PTR)
                {
                    return CTC_E_L3IF_ROUTED_EXCEED_IF_SIZE;
                }
                ret = ret?ret:sys_humber_vlan_set_l3if_id(&vlan_info, l3if_id);
                ret = ret?ret:sys_humber_vlan_set_ether_oam_valid(&vlan_info, 1);
                ret = ret?ret:sys_humber_vlan_set_egress_ether_oam_valid(&vlan_info, 1);
                ret = ret?ret:sys_humber_vlan_set_learning_en(&vlan_info, FALSE);
                ret = ret?ret:sys_humber_vlan_set_md_level(&vlan_info, 0);

                kal_memset(&userid_vlan, 0, sizeof(sys_usrid_vlan_entry_t));
                userid_vlan.valid.igs_cvid_valid = 1;
                userid_vlan.valid.src_port_valid = 1;
                userid_vlan.usrid_key_entry.global_port = l3_if->gport;
                userid_vlan.usrid_key_entry.ingress_cvid = l3_if->vlan_id;
                userid_vlan.usrid_key_entry.igs_cvid_mask = 0xFFF;
                userid_vlan.ds_entry_usrid.usr_vlan_ptr = vlan_ptr;

                if (CTC_IS_LINKAGG_PORT(l3_if->gport))
                {
                    for(lchip = 0; lchip < lchip_num; lchip++)
                    {
                        userid_vlan.usrid_key_entry.lchip = lchip;
                        ret = ret?ret:sys_humber_usrid_add_vlan_entry(&userid_vlan);
                    }
                }
                else
                {
                    SYS_MAP_GPORT_TO_LPORT(l3_if->gport, lchip, lport);
                    userid_vlan.usrid_key_entry.lchip = lchip;
                    ret = ret?ret:sys_humber_usrid_add_vlan_entry(&userid_vlan);
                }

                break;
            }

        default:
            ret = CTC_E_INVALID_PARAM;
    }

    if (ret != 0)
    {
        CTC_ERROR_RETURN(ret);
    }

    /*only save the last port property*/
    p_l3if_master->l3if_prop[l3if_id].gport = l3_if->gport;
    p_l3if_master->l3if_prop[l3if_id].vlan_id = l3_if->vlan_id;
    p_l3if_master->l3if_prop[l3if_id].l3if_type = l3_if->l3if_type;
    p_l3if_master->l3if_prop[l3if_id].vaild = SYS_L3IF_VAILD_VALUE;
    p_l3if_master->l3if_prop[l3if_id].vlan_ptr = vlan_ptr;

    return CTC_E_NONE;
}
 /**
   @brief    Delete  l3 interfaces
*/
int32
sys_humber_l3if_delete(uint16 l3if_id, ctc_l3if_t* l3_if)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 lchip_num = 0;
    uint32 cmd = 0;
    ds_src_interface_t src_interface;
    ds_dest_interface_t dest_interface;
     ds_dest_interface_t old_dest_interface;
    sys_vlan_info_t vlan_info;
    sys_usrid_vlan_entry_t userid_vlan;
    int32 ret = 0;

    SYS_L3IF_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l3_if);
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_type:%d ,l3if_id :%d port:0x%x, vlan_id:%d \n",
                      l3_if->l3if_type, l3if_id, l3_if->gport, l3_if->vlan_id);

    lchip_num = sys_humber_get_local_chip_num();
    switch(l3_if->l3if_type)
    {
        case CTC_L3IF_TYPE_PHY_IF:
            {
                CTC_GLOBAL_PORT_CHECK(l3_if->gport);

                if (p_l3if_master->l3if_prop[l3if_id].gport != l3_if->gport)
                {
                    return CTC_E_L3IF_MISMATCH;
                }

                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
                vlan_info.vid = 0xFFFF;
                vlan_info.gport = l3_if->gport;
                vlan_info.if_id = l3if_id;

                CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan(&vlan_info));

                break;
            }

        case CTC_L3IF_TYPE_SUB_IF:
            {
                CTC_GLOBAL_PORT_CHECK(l3_if->gport);

                if ((p_l3if_master->l3if_prop[l3if_id].vlan_id != l3_if->vlan_id)
                    || (p_l3if_master->l3if_prop[l3if_id].gport != l3_if->gport))
                {
                    return CTC_E_L3IF_MISMATCH;
                }

                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
                vlan_info.vid = l3_if->vlan_id;
                vlan_info.gport = l3_if->gport;
                vlan_info.if_id = l3if_id;
                CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan(&vlan_info));

                kal_memset(&userid_vlan, 0, sizeof(sys_usrid_vlan_entry_t));
                userid_vlan.valid.igs_cvid_valid  = 1;
                userid_vlan.valid.src_port_valid = 1;
                userid_vlan.usrid_key_entry.global_port = l3_if->gport;
                userid_vlan.usrid_key_entry.ingress_cvid = l3_if->vlan_id;
                userid_vlan.usrid_key_entry.igs_cvid_mask = 0xFFF;

                if (CTC_IS_LINKAGG_PORT(l3_if->gport))
                {
                    for(lchip = 0; lchip < lchip_num; lchip++)
                    {
                        userid_vlan.usrid_key_entry.lchip = lchip;
                        ret = ret?ret:sys_humber_usrid_delete_vlan_entry(&userid_vlan);
                    }
                }
                else
                {
                    SYS_MAP_GPORT_TO_LPORT(l3_if->gport, lchip, lport);
                    userid_vlan.usrid_key_entry.lchip = lchip;
                    ret = ret?ret:sys_humber_usrid_delete_vlan_entry(&userid_vlan);
                }
                break;
            }


        case CTC_L3IF_TYPE_VLAN_IF:
            {
                if(p_l3if_master->l3if_prop[l3if_id].vlan_id != l3_if->vlan_id)
                {
                    return CTC_E_L3IF_MISMATCH;
                }

                vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
                vlan_info.vid = l3_if->vlan_id;
                vlan_info.gport = l3_if->gport;

                CTC_ERROR_RETURN(sys_humber_vlan_set_l3if_id(&vlan_info, CTC_L3IF_INVALID_L3IF_ID));

                break;
            }

        default:
            return CTC_E_INVALID_PARAM;
    }


    kal_memset(&src_interface, 0, sizeof(src_interface));
    kal_memset(&dest_interface, 0, sizeof(dest_interface));
    src_interface.router_mac_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
    src_interface.router_mac_label = SYS_ROUTER_MAC_LABEL_INDEX;
    src_interface.lookup_mode = FALSE;
   /*#define  SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU      15 */
    src_interface.exception3_en =   1<<SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU ;   /*bit 15 always set to 1,reserved for normal IPDA copy_to_cpu */

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, l3if_id, cmd , &old_dest_interface));
    if(old_dest_interface.mac_sa_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC)
    {
       dest_interface.mac_sa = old_dest_interface.mac_sa;
    }
     dest_interface.mac_sa_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
        ret = ret?ret: drv_tbl_ioctl(lchip, l3if_id, cmd , &src_interface);

        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DRV_ENTRY_FLAG);
        ret = ret?ret: drv_tbl_ioctl(lchip, l3if_id, cmd , &dest_interface);
    }
    if (ret != 0)
    {
        CTC_ERROR_RETURN(ret);
    }

    p_l3if_master->l3if_prop[l3if_id].router_mac_label = SYS_ROUTER_MAC_LABEL_INDEX;
    p_l3if_master->l3if_prop[l3if_id].vaild = SYS_L3IF_INVAILD_VALUE;
    p_l3if_master->l3if_prop[l3if_id].vlan_ptr = SYS_VLAN_INVALID_PTR;

   return CTC_E_NONE;
}

static int32
 _sys_humber_l3if_get_if_info(uint16 l3if_id,sys_vlan_info_t *p_vlan_info)
{
    SYS_L3IF_LOCK;
    switch(p_l3if_master->l3if_prop[l3if_id].l3if_type)
    {
    case CTC_L3IF_TYPE_PHY_IF  :
        p_vlan_info->vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
        p_vlan_info->gport = p_l3if_master->l3if_prop[l3if_id].gport;
        p_vlan_info->vid = p_l3if_master->l3if_prop[l3if_id].vlan_id;

        break;
    case  CTC_L3IF_TYPE_SUB_IF :
        p_vlan_info->gport = p_l3if_master->l3if_prop[l3if_id].gport;
        p_vlan_info->vid = p_l3if_master->l3if_prop[l3if_id].vlan_id;
        p_vlan_info->vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
        break;
    case  CTC_L3IF_TYPE_VLAN_IF:
        p_vlan_info->gport = p_l3if_master->l3if_prop[l3if_id].gport;
        p_vlan_info->vid = p_l3if_master->l3if_prop[l3if_id].vlan_id;
        p_vlan_info->vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
        break;
    }
    SYS_L3IF_UNLOCK;
    return CTC_E_NONE;

}

bool
sys_humber_l3if_is_port_sub_if(uint16 gport)
{
    uint16 index = 0;
    bool  ret = FALSE;

    SYS_L3IF_INIT_CHECK();

    SYS_L3IF_LOCK;
      for(index = 0; index <MAX_CTC_L3IF_ID; index++)
      {
           if (p_l3if_master->l3if_prop[index].vaild == SYS_L3IF_VAILD_VALUE
              && p_l3if_master->l3if_prop[index].l3if_type == CTC_L3IF_TYPE_SUB_IF
              && p_l3if_master->l3if_prop[index].gport == gport)
           {
               ret = TRUE;
               break;
           }
      }
    SYS_L3IF_UNLOCK;

    return ret;
}

bool
sys_humber_l3if_is_port_phy_if(uint16 gport)
{
    uint16 index = 0;
    bool  ret = FALSE;

    SYS_L3IF_INIT_CHECK();

    SYS_L3IF_LOCK;
      for(index = 0; index <MAX_CTC_L3IF_ID; index++)
      {
           if (p_l3if_master->l3if_prop[index].vaild == SYS_L3IF_VAILD_VALUE
              && p_l3if_master->l3if_prop[index].l3if_type == CTC_L3IF_TYPE_PHY_IF
              && p_l3if_master->l3if_prop[index].gport == gport)
           {
               ret = TRUE;
               break;
           }
      }
    SYS_L3IF_UNLOCK;

    return ret;
}

 /**
   @brief    Config  l3 interface's  properties
*/
int32
sys_humber_l3if_set_property(uint16 l3if_id,ctc_l3if_property_t l3if_prop,uint32 value)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint16 value16 = 0;
    uint8 value8 = 0;
    uint32 cmd = 0;
    int32  ret =  0;
    bool   enable = FALSE;
    sys_vlan_info_t vlan_info;

    kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));


    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id :%d, l3if_prop:%d, value:%d \n", l3if_id, l3if_prop, value);
   _sys_humber_l3if_get_if_info(l3if_id, &vlan_info);

     switch(l3if_prop)
     {
        /*dsVlan */
        case CTC_L3IF_PROP_IPV4_UCAST :
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_v4ucast_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_IPV4_MCAST:
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_v4mcast_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_IPV4_SA_TYPE:
            if (value >= MAX_CTC_L3IF_IPSA_LKUP_TYPE)
            {
                return CTC_E_INVALID_PARAM;
            }
            sys_alloc_get_ipucsa_enable(&value8);
            if (value8 == 0)
            {
                return CTC_E_L3IF_NO_ALLOCED_IPUCSA;
            }
            value8 = value;
            ret = sys_humber_vlan_set_v4ucastSa_type(&vlan_info, value8);
            break;
        case CTC_L3IF_PROP_IPV6_UCAST:
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_v6ucast_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_IPV6_MCAST :
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_v6mcast_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_IPV6_SA_TYPE :
            if (value >= MAX_CTC_L3IF_IPSA_LKUP_TYPE)
            {
                return CTC_E_INVALID_PARAM;
            }
            sys_alloc_get_ipucsa_enable(&value8);
            if (value8 == 0)
            {
                return CTC_E_L3IF_NO_ALLOCED_IPUCSA;
            }
            value8 = value;
            ret = sys_humber_vlan_set_v6ucastSa_type(&vlan_info, value8);
            break;
        case CTC_L3IF_PROP_VRF_ID :
            value16 = value;
            ret = sys_humber_vlan_set_vrfid(&vlan_info, value16);
            break;
        case CTC_L3IF_PROP_TANSMIT_EN :
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_transmit_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_RECEIVE_EN:
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_receive_en(&vlan_info, enable);
            break;
        case CTC_L3IF_PROP_ROUTE_EN:
           enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_route_en(&vlan_info, enable);
            break;
        /*src interface */
        case CTC_L3IF_PROP_NAT_IFTYPE :
            value = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_IF_TYPE);
            break;
        case CTC_L3IF_PROP_ROUTE_ALL_PKT:
            value = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_ROUTE_ALL_PACKETS);
            break;
        case CTC_L3IF_PROP_IPV4_MCAST_RPF :
            value = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_V4_MCAST_RPF_EN);
            break;
        case CTC_L3IF_PROP_PBR_LABEL:
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_PBR_LABEL);
            break;
        case CTC_L3IF_PROP_IPV6_MCAST_RPF:
            value = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_V6_MCAST_RPF_EN);
            break;
      case CTC_L3IF_PROP_VRF_EN:
            value = value ?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_LOOKUP_MODE);
            break;
           /*dest  interface */
        case CTC_L3IF_PROP_MTU_EN:
            value = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_CHECK_EN);
            break;
        case CTC_L3IF_PROP_MTU_EXCEPTION_EN:
            (value) = (value)?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_EXCEPTION_EN);
            break;

        case CTC_L3IF_PROP_EGS_MCAST_TTL_THRESHOLD:
            if (value >= MAX_CTC_L3IF_MCAST_TTL_THRESHOLD)
            {
                return CTC_E_INVALID_PARAM;
            }
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MCAST_TTL_THRESHOLD);
            break;
        case CTC_L3IF_PROP_MTU_SIZE:
            if (value >= MAX_CTC_L3IF_MTU_SIZE)
            {
                return CTC_E_INVALID_PARAM;
            }
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_SIZE);
            break;
        case CTC_L3IF_PROP_EGS_MAC_SA_PREFIX_TYPE:
            if (value >= CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID)
            {
                return CTC_E_INVALID_PARAM;
            }
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MAC_SA_TYPE);
            break;
        case CTC_L3IF_PROP_EGS_MAC_SA_LOW_8BITS:
             if (value >  CTC_MAX_UINT8_VALUE)
            {
                return CTC_E_INVALID_PARAM;
            }
            cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MAC_SA);
            break;

        case CTC_L3IF_PROP_MPLS_EN:
            value = value ?1:0;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_MPLS_EN);
            break;
        case CTC_L3IF_PROP_MPLS_LABEL_SPACE:
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_MPLS_LABEL_SPACE);
            if (value >  CTC_MAX_UINT8_VALUE)
            {
                return CTC_E_INVALID_PARAM;
            }
            break;
        case CTC_L3IF_PROP_ARP_EXCEPTION_TYPE:
            ret = sys_humber_vlan_set_arp_excp_type(&vlan_info, value);
            break;

        case CTC_L3IF_PROP_DHCP_EXCEPTION_TYPE:
            ret = sys_humber_vlan_set_dhcp_excp_type(&vlan_info, value);
            break;

        case CTC_L3IF_PROP_IGMP_SNOOPING_EN:
            enable = (value)?TRUE:FALSE;
            ret = sys_humber_vlan_set_igmp_snoop_en(&vlan_info, enable);
            break;

        default:
            return CTC_E_INVALID_PARAM;
      }


      if (ret != CTC_E_NONE)
      {
          return ret ;
      }

      if(cmd == 0)
      {
          /*when cmd == 0, get value from  software memory*/
           return CTC_E_NONE;
      }

      lchip_num = sys_humber_get_local_chip_num();
      for (lchip = 0; lchip < lchip_num; lchip++ )
      {
          CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l3if_id, cmd, &value));
      }
      return CTC_E_NONE;
}

  /**
    @brief    Get  l3 interface's properties  according to interface id
  */

int32
sys_humber_l3if_get_property(uint16 l3if_id, ctc_l3if_property_t l3if_prop, uint32* value)
{
    uint32  cmd = 0;
    uint16 value16= 0;
    uint8  value8= 0;
    int32  ret = 0;
    bool   enable = FALSE;


    sys_vlan_info_t vlan_info;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id :%d l3if_prop:%d \n", l3if_id, l3if_prop);

    ret = _sys_humber_l3if_get_if_info(l3if_id, &vlan_info);

    switch(l3if_prop)
    {
       /*dsVlan */
        case CTC_L3IF_PROP_IPV4_UCAST :
           ret = sys_humber_vlan_get_v4ucast_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_IPV4_MCAST:
            ret = sys_humber_vlan_get_v4mcast_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_IPV4_SA_TYPE:
            ret = sys_humber_vlan_get_v4ucastSa_type(&vlan_info, &value8);
            *value = value8;
            break;
        case CTC_L3IF_PROP_IPV6_UCAST:
            ret = sys_humber_vlan_get_v6ucast_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_IPV6_MCAST :
            ret = sys_humber_vlan_get_v6mcast_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_IPV6_SA_TYPE :
            ret = sys_humber_vlan_get_v6ucastSa_type(&vlan_info, &value8);
            *value = value8;
            break;
        case CTC_L3IF_PROP_VRF_ID :
            ret = sys_humber_vlan_get_vrfid(&vlan_info, &value16);
            *value = value16;
            break;
        case CTC_L3IF_PROP_TANSMIT_EN :
            ret = sys_humber_vlan_get_transmit_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_RECEIVE_EN:
            ret = sys_humber_vlan_get_receive_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        case CTC_L3IF_PROP_ROUTE_EN:
            ret = sys_humber_vlan_get_route_en(&vlan_info, &enable);
            *value = (enable == TRUE)?1:0;
            break;
        /*src interface */
        case CTC_L3IF_PROP_NAT_IFTYPE :
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_IF_TYPE);
            break;
        case CTC_L3IF_PROP_ROUTE_ALL_PKT:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_ROUTE_ALL_PACKETS);
            break;
        case CTC_L3IF_PROP_IPV4_MCAST_RPF :
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_V4_MCAST_RPF_EN);
            break;
        case CTC_L3IF_PROP_PBR_LABEL:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_PBR_LABEL);
            break;
        case CTC_L3IF_PROP_IPV6_MCAST_RPF:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_V6_MCAST_RPF_EN);
            break;
        case CTC_L3IF_PROP_VRF_EN:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_LOOKUP_MODE);
            break;
           /*dest  interface */
        case CTC_L3IF_PROP_MTU_EN:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_CHECK_EN);
            break;
        case CTC_L3IF_PROP_MTU_EXCEPTION_EN:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_EXCEPTION_EN);
            break;
        case CTC_L3IF_PROP_EGS_MCAST_TTL_THRESHOLD:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MCAST_TTL_THRESHOLD);
            break;
        case CTC_L3IF_PROP_MTU_SIZE:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MTU_SIZE);
            break;
        case CTC_L3IF_PROP_EGS_MAC_SA_PREFIX_TYPE:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MAC_SA_TYPE);
            break;
        case CTC_L3IF_PROP_EGS_MAC_SA_LOW_8BITS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_MAC_SA);
            break;
        case   CTC_L3IF_PROP_MPLS_EN:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_MPLS_EN);
            break;
        case CTC_L3IF_PROP_MPLS_LABEL_SPACE:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_MPLS_LABEL_SPACE);
            break;
        case CTC_L3IF_PROP_ARP_EXCEPTION_TYPE:
            ret = sys_humber_vlan_get_arp_excp_type(&vlan_info, (ctc_exception_type_t *)value);
            break;
        case CTC_L3IF_PROP_DHCP_EXCEPTION_TYPE:
            ret = sys_humber_vlan_get_dhcp_excp_type(&vlan_info, (ctc_exception_type_t *)value);
            break;

        default:
          return CTC_E_INVALID_PARAM;
      }

    if (ret != CTC_E_NONE)
    {
        return ret ;
    }

    if(cmd == 0)
    {
        /*when cmd == 0, get value from  software memory*/
        return CTC_E_NONE;
    }
     CTC_ERROR_RETURN(drv_tbl_ioctl(0, l3if_id, cmd, value));

    return CTC_E_NONE;
}

int32
sys_humber_l3if_set_exception3_en(uint16 l3if_id, uint8 index, bool enbale)
{
    uint8   lchip = 0;
    uint8   lchip_num = 0;
    uint32  cmd = 0;
    uint32  value = 0;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id = %d, index = %d, enbale[0]= %d\n", l3if_id, index, enbale);


    cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_EXCEPTION3_EN);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, l3if_id, cmd, &value));
    if ( TRUE == enbale)
    {
        value |= (1 << index);

    }
    else
    {
        value &= (~(1 << index));
    }

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_EXCEPTION3_EN);
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num && lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip++ )
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l3if_id, cmd, &value));
    }
    return CTC_E_NONE;
}

int32
sys_humber_l3if_get_exception3_en(uint16 l3if_id, uint8 index, bool *enbale)
{

    uint32  cmd = 0;
    uint32  value = 0;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);
    CTC_PTR_VALID_CHECK(enbale);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id = %d, index = %d\n", l3if_id, index);


    cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_EXCEPTION3_EN);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, l3if_id, cmd, &value));
    *enbale = (value & (1 << index)) ? TRUE: FALSE;
    return CTC_E_NONE;
}


int32
sys_humber_l3if_set_aclqos_property(uint16 l3if_id, sys_l3if_aclaos_property_t l3if_prop, uint32 value[CTC_MAX_LOCAL_CHIP_NUM])
{
    uint8   lchip = 0;
    uint8   lchip_num = 0;
    uint32 cmd = 0;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id = %d, property = %d, value[0]= %d, value[1] = %d\n", l3if_id, l3if_prop, value[0], value[1]);

    switch(l3if_prop)
    {
        /*src  interface */
    case SYS_L3IF_PROP_L3ACL_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_ACL_EN);
        break;
    case SYS_L3IF_PROP_L3QOS_LKUP_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_QOS_LOOKUP_EN);
        break;
    case SYS_L3IF_PROP_L3ACL_ROUTEDONLY:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_ACL_ROUTED_ONLY);
        break;
    case SYS_L3IF_PROP_L3QOS_LABEL:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_QOS_LABEL);
        break;
    case SYS_L3IF_PROP_L3ACL_LABEL:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_ACL_LABEL);
        break;
        /*dest  interface */
    case SYS_L3IF_EGS_PROP_L3ACL_ROUTED_ONLY:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_ROUTED_ONLY);
        break;
    case SYS_L3IF_EGS_PROP_L3ACL_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_EN);
        break;
    case SYS_L3IF_EGS_PROP_L3QOS_LOOKUP_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_QOS_LOOKUP_EN);
        break;
    case SYS_L3IF_EGS_PROP_L3ACL_LABEL:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_LABEL);
        break;
    case SYS_L3IF_EGS_PROP_L3QOS_LABEL:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_QOS_LABEL);
        break;
    default:
        return CTC_E_INVALID_PARAM;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num && lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip++ )
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l3if_id, cmd, &value[lchip]));
    }

    return CTC_E_NONE;
}

  /**
    @brief    Get  l3 interface's properties  according to interface id
  */

int32
sys_humber_l3if_get_aclqos_property(uint16 l3if_id, sys_l3if_aclaos_property_t l3if_prop, uint32 value[CTC_MAX_LOCAL_CHIP_NUM])
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint32  cmd = 0;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");

    switch(l3if_prop)
    {
        /*src  interface */
    case SYS_L3IF_PROP_L3ACL_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_ACL_EN);
        break;
    case SYS_L3IF_PROP_L3QOS_LKUP_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_V4_MCAST_RPF_EN);
        break;
    case SYS_L3IF_PROP_L3ACL_ROUTEDONLY:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_QOS_LOOKUP_EN);
        break;
    case SYS_L3IF_PROP_L3QOS_LABEL:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_QOS_LABEL);
        break;
    case SYS_L3IF_PROP_L3ACL_LABEL:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_ACL_LABEL);
        break;
        /*dest  interface */
    case SYS_L3IF_EGS_PROP_L3ACL_ROUTED_ONLY:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_ROUTED_ONLY);
        break;
    case SYS_L3IF_EGS_PROP_L3ACL_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_EN);
        break;
    case SYS_L3IF_EGS_PROP_L3QOS_LOOKUP_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_QOS_LOOKUP_EN);
        break;
    case SYS_L3IF_EGS_PROP_L3ACL_LABEL:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_ACL_LABEL);
        break;
    case SYS_L3IF_EGS_PROP_L3QOS_LABEL:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_QOS_LABEL);
        break;
    default:
        return CTC_E_INVALID_PARAM;
    }


    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num && lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip++ )
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l3if_id, cmd, &value[lchip]));
    }

    SYS_L3IF_DBG_INFO("l3if_id = %d, property = %d, value[0]= %d, value[1] = %d\n", l3if_id, l3if_prop, value[0], value[1]);

    return CTC_E_NONE;
}

     /**
   @brief    Config  l3 interface's  properties
*/
int32
sys_humber_l3if_set_span_property(uint16 l3if_id, sys_l3if_span_property_t l3if_prop, uint32 value)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint32  cmd = 0;


    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);;

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id :%d l3if_prop:%d, value:%d \n", l3if_id, l3if_prop, value);

    switch(l3if_prop)
    {
    case SYS_L3IF_PROP_L3SPAN_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_SPAN_EN);
        break;
    case SYS_L3IF_PROP_L3SPAN_ID:
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_SPAN_ID);
        break;

    case SYS_L3IF_EGS_PROP_SPAN_EN:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_SPAN_EN);
        break;
    case SYS_L3IF_EGS_PROP_SPAN_ID:
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_SPAN_ID);
        break;
    default:
        return CTC_E_INVALID_PARAM;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l3if_id, cmd, &value));
    }
    return CTC_E_NONE;
}

  /**
    @brief    Get  l3 interface's properties  according to interface id
  */

int32
sys_humber_l3if_get_span_property(uint16 l3if_id,sys_l3if_span_property_t l3if_prop,uint32 *value)
{
    uint32  cmd = 0;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    SYS_L3IF_CREATE_CHECK(l3if_id);;

    SYS_L3IF_DBG_FUNC("");
    switch(l3if_prop)
    {
    case SYS_L3IF_PROP_L3SPAN_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_SPAN_EN);
        break;
    case SYS_L3IF_PROP_L3SPAN_ID:
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DS_SRC_INTERFACE_L3_SPAN_ID);
        break;

    case SYS_L3IF_EGS_PROP_SPAN_EN:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_SPAN_EN);
        break;
    case SYS_L3IF_EGS_PROP_SPAN_ID:
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DS_DEST_INTERFACE_L3_SPAN_ID);
        break;
    default:
        return CTC_E_INVALID_PARAM;
    }

    CTC_ERROR_RETURN(drv_tbl_ioctl(0, l3if_id, cmd, value));
    SYS_L3IF_DBG_INFO("l3if_id :%d l3if_prop:%d, value:%d \n", l3if_id, l3if_prop, *value);

    return CTC_E_NONE;
}


 /**
       @brief    Config  40bits  virtual router mac prefix
  */

int32
sys_humber_l3if_set_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{

    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint32  cmd = 0;
    ipe_router_mac_ctl_t rt_mac_ctl;
    epe_l2_router_mac_sa_t l2rt_macsa;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("prefix_type :%d mac_prefix:%.2x%.2x.%.2x%.2x.%.2x \n", prefix_type,
     mac_40bit[0],mac_40bit[1],mac_40bit[2],mac_40bit[3],mac_40bit[4]);

    CTC_MAX_VALUE_CHECK(prefix_type, CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID);

    cmd = DRV_IOR(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN( drv_reg_ioctl(lchip, 0, cmd, &rt_mac_ctl));

    cmd = DRV_IOR(IOC_REG, EPE_L2_ROUTER_MAC_SA, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &l2rt_macsa));


    switch(prefix_type)
    {
    case CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE0:
        rt_mac_ctl.router_mac_type0_bit47_to32 = (mac_40bit[0] << 8) + mac_40bit[1];
        rt_mac_ctl.router_mac_type0_bit31_to8 = (mac_40bit[2] << 16 ) + (mac_40bit[3] << 8) + mac_40bit[4];
        l2rt_macsa.epe_l2_router_mac_sa0_upper = (mac_40bit[0] << 8 ) + mac_40bit[1];
        l2rt_macsa.epe_l2_router_mac_sa0_lower = (mac_40bit[2] << 16) + (mac_40bit[3] << 8) +  mac_40bit[4];
        break;
    case CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE1:
        rt_mac_ctl.router_mac_type1_bit47_to32 = (mac_40bit[0] << 8 ) + mac_40bit[1];
        rt_mac_ctl.router_mac_type1_bit31_to8 = (mac_40bit[2] << 16) + (mac_40bit[3] << 8) + mac_40bit[4];
        l2rt_macsa.epe_l2_router_mac_sa1_upper = (mac_40bit[0] << 8 ) + mac_40bit[1];
        l2rt_macsa.epe_l2_router_mac_sa1_lower = (mac_40bit[2] << 16) + (mac_40bit[3] << 8) +  mac_40bit[4];
        break;
    default:
        return CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE;
    }
    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        CTC_ERROR_RETURN( drv_reg_ioctl(lchip, 0, cmd , &rt_mac_ctl));
    }
    cmd = DRV_IOW(IOC_REG, EPE_L2_ROUTER_MAC_SA, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < lchip_num; lchip++ )
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &l2rt_macsa));
    }
    return CTC_E_NONE;

}

 /**
 @brief    Get  40bits   virtual router mac prefix
*/
int32
sys_humber_l3if_get_vmac_prefix(uint8 prefix_type, mac_addr_t mac_40bit)
{
    uint8 lchip = 0;
    uint32  cmd = 0;
    ipe_router_mac_ctl_t rt_mac_ctl;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IF_DBG_FUNC("");
    CTC_MAX_VALUE_CHECK(prefix_type, CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID);

    cmd = DRV_IOR(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN( drv_reg_ioctl(lchip, 0, cmd, &rt_mac_ctl));

    switch(prefix_type)
    {
    case CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE0:
        mac_40bit[0] = (rt_mac_ctl.router_mac_type0_bit47_to32 >> 8) & 0xFF;
        mac_40bit[1] = (rt_mac_ctl.router_mac_type0_bit47_to32 ) & 0xFF;
        mac_40bit[2] = (rt_mac_ctl.router_mac_type0_bit31_to8 >> 16)& 0xFF;
        mac_40bit[3] = (rt_mac_ctl.router_mac_type0_bit31_to8 >> 8)& 0xFF;
        mac_40bit[4] = (rt_mac_ctl.router_mac_type0_bit31_to8 )& 0xFF;
        break;
    case CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE1:
        mac_40bit[0] = (rt_mac_ctl.router_mac_type1_bit47_to32 >> 8) & 0xFF;
        mac_40bit[1] = (rt_mac_ctl.router_mac_type1_bit47_to32 ) & 0xFF;
        mac_40bit[2] = (rt_mac_ctl.router_mac_type1_bit31_to8 >> 16)& 0xFF;
        mac_40bit[3] = (rt_mac_ctl.router_mac_type1_bit31_to8 >> 8)& 0xFF;
        mac_40bit[4] = (rt_mac_ctl.router_mac_type1_bit31_to8 )& 0xFF;
        break;
    default:
        return CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE;
    }
     SYS_L3IF_DBG_INFO("prefix_type :%d mac_prefix:%.2x%.2x.%.2x%.2x.%.2x \n", prefix_type,
     mac_40bit[0],mac_40bit[1],mac_40bit[2],mac_40bit[3],mac_40bit[4]);

    return CTC_E_NONE;
}

/**
     @brief    Config a low 8 bits virtual router-mac  in the L3 interface, it can config up to 4 VRIDs  for a index

*/
int32
sys_humber_l3if_add_vmac_low_8bit(uint16 l3if_id, ctc_l3if_vmac_t *p_l3if_vmac)
{
    uint8 lchip = 0;
    uint8 find_flag = SYS_L3IF_INVAILD_VALUE;
    uint8 first_find_flag = SYS_L3IF_INVAILD_VALUE;
    uint32 cmd = 0;
    uint8  index = 0;
    uint8  route_mac_label = 0;
    ds_router_mac_t rt_mac;
    ds_src_interface_t src_interface;
    sys_l3if_vmac_entry_t  *vmac_entry = NULL;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IF_CREATE_CHECK(l3if_id);
    SYS_L3IFID_VAILD_CHECK(l3if_id);
    CTC_PTR_VALID_CHECK(p_l3if_vmac);
    CTC_MAX_VALUE_CHECK(p_l3if_vmac->low_8bits_mac_index, MAX_CTC_L3IF_VMAC_MAC_INDEX);

    if (p_l3if_vmac->prefix_type > CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC )
    {
        return CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE;
    }

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id:%d,prefix_type :%d low_8bits_mac:%.2x\n",
                   l3if_id, p_l3if_vmac->prefix_type, p_l3if_vmac->low_8bits_mac);

    SYS_L3IF_LOCK;
    route_mac_label = p_l3if_master->l3if_prop[l3if_id].router_mac_label;
    if (route_mac_label  == SYS_ROUTER_MAC_LABEL_INDEX )
    {
        for (index = 0; index <= SYS_MAX_VMAC_MAC_LABEL_INDEX; index++ )
        {
            if (p_l3if_master->vmac_entry[index].vaild == SYS_L3IF_INVAILD_VALUE)
            {
                route_mac_label = index;
                find_flag = SYS_L3IF_VAILD_VALUE;
                first_find_flag = SYS_L3IF_VAILD_VALUE;
                break;
            }
        }
    }
    else
    {
        find_flag = SYS_L3IF_VAILD_VALUE;
    }

    if (SYS_L3IF_INVAILD_VALUE == find_flag)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK( CTC_E_L3IF_VMAC_ENTRY_EXCEED_MAX_SIZE, p_l3if_master->p_mutex);
    }

   cmd = DRV_IOR(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
   CTC_ERROR_RETURN_WITH_UNLOCK( drv_tbl_ioctl(lchip, route_mac_label, cmd, &rt_mac), p_l3if_master->p_mutex);
   vmac_entry = &p_l3if_master->vmac_entry[route_mac_label];

    if (first_find_flag )
    {
        rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        rt_mac.router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac0_type  = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac1_type  = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac2_type  = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac3_type  = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->vaild  = SYS_L3IF_INVAILD_VALUE;
    }

    switch(p_l3if_vmac->low_8bits_mac_index)
    {
    case 0:
        rt_mac.router_mac0_byte = p_l3if_vmac->low_8bits_mac;
        rt_mac.router_mac0_type = p_l3if_vmac->prefix_type;
        vmac_entry->router_mac0_type = p_l3if_vmac->prefix_type;
        break;
    case 1:
        rt_mac.router_mac1_byte = p_l3if_vmac->low_8bits_mac;
        rt_mac.router_mac1_type = p_l3if_vmac->prefix_type;
        vmac_entry->router_mac1_type = p_l3if_vmac->prefix_type;
        break ;
    case 2:
        rt_mac.router_mac2_byte = p_l3if_vmac->low_8bits_mac;
        rt_mac.router_mac2_type = p_l3if_vmac->prefix_type;
        vmac_entry->router_mac2_type = p_l3if_vmac->prefix_type;
        break;
    case 3:
        rt_mac.router_mac3_byte = p_l3if_vmac->low_8bits_mac;
        rt_mac.router_mac3_type = p_l3if_vmac->prefix_type;
        vmac_entry->router_mac3_type = p_l3if_vmac->prefix_type;
        break ;
    default:
        CTC_ERROR_RETURN_WITH_UNLOCK( CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE, p_l3if_master->p_mutex);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN_WITH_UNLOCK( drv_tbl_ioctl(lchip, route_mac_label, cmd, &rt_mac), p_l3if_master->p_mutex);
    p_l3if_master->l3if_prop[l3if_id].router_mac_label = route_mac_label;
    p_l3if_master->vmac_entry[route_mac_label].vaild = SYS_L3IF_VAILD_VALUE;

    cmd =  DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_tbl_ioctl(lchip, l3if_id, cmd , &src_interface), p_l3if_master->p_mutex);
    src_interface.router_mac_label = route_mac_label;
    cmd =  DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_tbl_ioctl(lchip, l3if_id, cmd , &src_interface),  p_l3if_master->p_mutex);

SYS_L3IF_UNLOCK;
    return CTC_E_NONE;
}

/**
     @brief    Config a low 8 bits virtual router-mac  in the L3 interface, it can config up to 4 VRIDs  for a index

*/
int32
sys_humber_l3if_remove_vmac_low_8bit(uint16 l3if_id,ctc_l3if_vmac_t *p_l3if_vmac)
{
    uint8 lchip = 0;
    uint32  cmd = 0;
    uint8  route_mac_label = 0;
    ds_router_mac_t rt_mac;
    sys_l3if_vmac_entry_t  *vmac_entry = NULL;
    ds_src_interface_t src_interface;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IF_CREATE_CHECK(l3if_id);
    SYS_L3IFID_VAILD_CHECK(l3if_id);

    CTC_PTR_VALID_CHECK(p_l3if_vmac);
    CTC_MAX_VALUE_CHECK(p_l3if_vmac->low_8bits_mac_index, MAX_CTC_L3IF_VMAC_MAC_INDEX);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id :%d low_8bits_mac_index:%d\n", l3if_id,p_l3if_vmac->low_8bits_mac_index);

SYS_L3IF_LOCK;

    route_mac_label = p_l3if_master->l3if_prop[l3if_id].router_mac_label;
    if (route_mac_label  == SYS_ROUTER_MAC_LABEL_INDEX
        ||  !p_l3if_master->vmac_entry[route_mac_label].vaild)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK( CTC_E_L3IF_VMAC_NOT_EXIST, p_l3if_master->p_mutex);
    }
    vmac_entry = &p_l3if_master->vmac_entry[route_mac_label];

   cmd = DRV_IOR(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
   CTC_ERROR_RETURN_WITH_UNLOCK( drv_tbl_ioctl(lchip, route_mac_label, cmd, &rt_mac),p_l3if_master->p_mutex);

    switch(p_l3if_vmac->low_8bits_mac_index)
    {
    case 0:
        rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        break;
    case 1:
        rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        break ;
    case 2:
        rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        vmac_entry->router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        break;
    case 3:
        rt_mac.router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
         vmac_entry->router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
        break ;
    default:
        return CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE;
    }
    cmd = DRV_IOW(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN_WITH_UNLOCK( drv_tbl_ioctl(lchip, route_mac_label, cmd, &rt_mac),p_l3if_master->p_mutex);

    if (vmac_entry->router_mac0_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID
        && vmac_entry->router_mac1_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID
        && vmac_entry->router_mac2_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID
        && vmac_entry->router_mac3_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID)
    {
        p_l3if_master->l3if_prop[l3if_id].router_mac_label =  SYS_ROUTER_MAC_LABEL_INDEX;
        p_l3if_master->vmac_entry[route_mac_label].vaild = SYS_L3IF_INVAILD_VALUE;

        cmd =  DRV_IOR(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN_WITH_UNLOCK(drv_tbl_ioctl(lchip, l3if_id, cmd , &src_interface), p_l3if_master->p_mutex);
        src_interface.router_mac_label = SYS_ROUTER_MAC_LABEL_INDEX;
        cmd =  DRV_IOW(IOC_TABLE, DS_SRC_INTERFACE, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN_WITH_UNLOCK(drv_tbl_ioctl(lchip, l3if_id, cmd , &src_interface), p_l3if_master->p_mutex);

    }
 SYS_L3IF_UNLOCK;

    return CTC_E_NONE;
}

/**
     @brief    Get a low 8 bits router-mac  according to interface id
*/
int32
sys_humber_l3if_get_vmac_low_8bit(uint16 l3if_id, ctc_l3if_vmac_t *p_l3if_vmac)
{
    uint8 lchip = 0;
    uint32  cmd = 0;
    uint8  route_mac_label = 0;
    ds_router_mac_t rt_mac;

     SYS_L3IF_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_l3if_vmac);
    CTC_MAX_VALUE_CHECK(p_l3if_vmac->low_8bits_mac_index, MAX_CTC_L3IF_VMAC_MAC_INDEX);

    SYS_L3IF_DBG_FUNC("");
    SYS_L3IF_DBG_INFO("l3if_id :%d low_8bits_mac_index:%d\n", l3if_id,p_l3if_vmac->low_8bits_mac_index);
SYS_L3IF_LOCK;
    route_mac_label = p_l3if_master->l3if_prop[l3if_id].router_mac_label;

    if (route_mac_label  == SYS_ROUTER_MAC_LABEL_INDEX
        ||  !p_l3if_master->vmac_entry[route_mac_label].vaild)
    {
         SYS_L3IF_UNLOCK;
        return CTC_E_L3IF_VMAC_NOT_EXIST;
    }
SYS_L3IF_UNLOCK;
    cmd = DRV_IOR(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN( drv_tbl_ioctl(lchip, route_mac_label, cmd, &rt_mac));

    switch(p_l3if_vmac->low_8bits_mac_index)
    {
    case 0:
        p_l3if_vmac->low_8bits_mac =  rt_mac.router_mac0_byte;
         p_l3if_vmac->prefix_type  = rt_mac.router_mac0_type;
        break;
    case 1:
       p_l3if_vmac->low_8bits_mac =  rt_mac.router_mac1_byte;
        p_l3if_vmac->prefix_type  = rt_mac.router_mac1_type;
        break  ;
    case 2:
       p_l3if_vmac->low_8bits_mac =  rt_mac.router_mac2_byte;
       p_l3if_vmac->prefix_type   = rt_mac.router_mac2_type;
        break;
    case 3:
        p_l3if_vmac->low_8bits_mac =  rt_mac.router_mac3_byte;
        p_l3if_vmac->prefix_type   = rt_mac.router_mac3_type;
        break  ;
    default:
        return CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE;
    }
    return CTC_E_NONE;
}

 /**
     @brief    Config  router mac

*/
 int32
 sys_humber_l3if_set_router_mac( mac_addr_t mac_addr)
 {
     uint8   lchip = 0;
     uint8   lchip_num = 0;
     uint32  read_cmd = 0;
     uint32  write_cmd = 0;
     uint32  read_dest_if_cmd = 0;
     uint32  write_dest_if_cmd = 0;
     uint16  index = 0;
     ds_router_mac_t rt_mac;
     ipe_router_mac_ctl_t rt_mac_ctl;
     epe_l2_router_mac_sa_t l2rt_macsa;
     ds_dest_interface_t dest_interface;


     kal_memset(&dest_interface, 0, sizeof(dest_interface));

     SYS_L3IF_INIT_CHECK();
     SYS_L3IF_DBG_FUNC("");
     SYS_L3IF_DBG_INFO("router_mac:%.2x%.2x.%.2x%.2x.%.2x%.2x \n",
                       mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

     read_cmd = DRV_IOR(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
     write_cmd = DRV_IOW(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
     lchip_num = sys_humber_get_local_chip_num();

     for (lchip = 0; lchip < lchip_num; lchip++ )
     {
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, read_cmd, &rt_mac_ctl));
         rt_mac_ctl.router_mac_type2_bit47_to32 = (mac_addr[0] << 8 )+ mac_addr[1];
         rt_mac_ctl.router_mac_type2_bit31_to8 = (mac_addr[2] << 16) + (mac_addr[3] << 8) +  mac_addr[4];
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, write_cmd, &rt_mac_ctl));
     }
     read_cmd = DRV_IOR(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
     write_cmd = DRV_IOW(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);
     for (lchip = 0; lchip < lchip_num; lchip++ )
     {
         CTC_ERROR_RETURN(
         drv_tbl_ioctl(lchip, SYS_ROUTER_MAC_LABEL_INDEX, read_cmd, &rt_mac));
         rt_mac.router_mac0_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC;
         rt_mac.router_mac0_byte = mac_addr[5];
         rt_mac.router_mac1_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
         rt_mac.router_mac2_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
         rt_mac.router_mac3_type = CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_INVALID;
         CTC_ERROR_RETURN(
              drv_tbl_ioctl(lchip, SYS_ROUTER_MAC_LABEL_INDEX, write_cmd, &rt_mac));
     }

     read_cmd = DRV_IOR(IOC_REG, EPE_L2_ROUTER_MAC_SA, DRV_ENTRY_FLAG);
     write_cmd = DRV_IOW(IOC_REG, EPE_L2_ROUTER_MAC_SA, DRV_ENTRY_FLAG);

     read_dest_if_cmd =  DRV_IOR(IOC_TABLE, DS_DEST_INTERFACE, DRV_ENTRY_FLAG);
     write_dest_if_cmd =  DRV_IOW(IOC_TABLE, DS_DEST_INTERFACE, DRV_ENTRY_FLAG);

     for (lchip = 0; lchip < lchip_num; lchip++ )
     {
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, read_cmd, &l2rt_macsa));
         l2rt_macsa.epe_l2_router_mac_sa2_upper = (mac_addr[0] << 8 ) + mac_addr[1];
         l2rt_macsa.epe_l2_router_mac_sa2_lower = (mac_addr[2] << 16) + (mac_addr[3] << 8) +  mac_addr[4];
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, write_cmd, &l2rt_macsa));

        for (index = 0; index <= MAX_CTC_L3IF_ID; index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, read_dest_if_cmd , &dest_interface));
            if (dest_interface.mac_sa_type == CTC_L3IF_ROUTE_MAC_PFEFIX_TYPE_RSV_ROUTER_MAC)
            {
                dest_interface.mac_sa = mac_addr[5];
            }
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, write_dest_if_cmd , &dest_interface));
        }
     }
      return CTC_E_NONE;
 }

 /**
     @brief    Get  router mac
*/
 int32
sys_humber_l3if_get_router_mac( mac_addr_t mac_addr)
{

    uint8   lchip = 0;
    uint32  read_cmd = 0;
    ds_router_mac_t rt_mac;
    ipe_router_mac_ctl_t rt_mac_ctl;

    SYS_L3IF_INIT_CHECK();
    SYS_L3IF_DBG_FUNC("");
    read_cmd = DRV_IOR(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, read_cmd, &rt_mac_ctl));
    mac_addr[0] = (rt_mac_ctl.router_mac_type2_bit47_to32 >> 8) & 0xFF;
    mac_addr[1] = (rt_mac_ctl.router_mac_type2_bit47_to32 ) & 0xFF;
    mac_addr[2] = (rt_mac_ctl.router_mac_type2_bit31_to8 >> 16)& 0xFF;
    mac_addr[3] = (rt_mac_ctl.router_mac_type2_bit31_to8 >> 8)& 0xFF;
    mac_addr[4] = (rt_mac_ctl.router_mac_type2_bit31_to8 )& 0xFF;
    read_cmd = DRV_IOR(IOC_TABLE, DS_ROUTER_MAC, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN( drv_tbl_ioctl(lchip, SYS_ROUTER_MAC_LABEL_INDEX, read_cmd, &rt_mac));

    mac_addr[5] = rt_mac.router_mac0_byte;

    SYS_L3IF_DBG_INFO("mac_prefix:%.2x%.2x.%.2x%.2x.%.2x%.2x \n",
                      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

     return CTC_E_NONE;
}

 /**
     @brief    Get  vlan_ptr by l3if_id
*/
 int32
sys_humber_l3if_get_vlan_ptr(uint16 l3if_id, uint16* vlan_ptr)
{
    SYS_L3IF_INIT_CHECK();
    SYS_L3IFID_VAILD_CHECK(l3if_id);

    SYS_L3IF_DBG_FUNC("");

    if (!p_l3if_master->l3if_prop[l3if_id].vaild)
    {
        return CTC_E_L3IF_NOT_EXIST;
    }

    *vlan_ptr = p_l3if_master->l3if_prop[l3if_id].vlan_ptr;
    return CTC_E_NONE;
}

