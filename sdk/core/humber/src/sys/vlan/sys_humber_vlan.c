/**
 @file sys_humber_vlan.c

 @date 2009-10-17

 @version v2.0

 This file contains all sys APIs of vlan

 vlan_ptr assignment:

 0          ------- vlan switching
 1-4094     ------- normal vlan
 4095       ------- service queue
 4096-4351  ------- phy-if & sub-if
 4352-4859  ------- TBD
 4860       ------- vpls mac limit enable
 4861       ------- vpls learing disable
 4862       ------- vpls learing enable
 4863       ------- vpws
*/

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_debug.h"
#include "ctc_error.h"
#include "ctc_vlan.h"

#include "sys_humber_vlan.h"
#include "sys_humber_vlan_db.h"
#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"
#include "sys_humber_opf.h"

#include "drv_io.h"
#include "drv_humber.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
static sys_vlan_master_t *p_vlan_master = NULL;
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
/**
 @brief init the vlan module

*/
int32
sys_humber_vlan_init(void)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    int32 ret = CTC_E_NONE;
    uint32 vlan_cmd = 0;
    uint32 vlan_status_cmd = 0;
    uint32 index = 0;
    uint32 vlan_entry_num = 0;
    uint32 vlan_status_entry_num = 0;
    ds_vlan_t ds_vlan;
    ds_vlan_status_t ds_vlan_status;

    if (NULL != p_vlan_master)
    {
        return CTC_E_NONE;
    }

    p_vlan_master = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_vlan_master_t));
    if (NULL == p_vlan_master)
    {
        ret = CTC_E_NO_MEMORY;
        goto error;
    }

    /*mutex*/
    ret = kal_mutex_create(&(p_vlan_master->p_vlan_mutex));
    if (ret || (!p_vlan_master->p_vlan_mutex))
    {
        ret = CTC_E_FAIL_CREATE_MUTEX;
        goto error;
    }

    /*vlan vector*/
    sys_alloc_get_table_entry_num(DS_VLAN, &vlan_entry_num);
    sys_alloc_get_table_entry_num(DS_VLAN_STATUS, &vlan_status_entry_num);

    if (0 == vlan_entry_num)
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }

    p_vlan_master->p_vlan_vector
    = ctc_vector_init(SYS_MAX_VLAN_BLOCK_NUM, (vlan_entry_num /SYS_MAX_VLAN_BLOCK_NUM));

    if (NULL == p_vlan_master->p_vlan_vector)
    {
        ret = CTC_E_NO_MEMORY;
        goto error;
    }

    ctc_vector_reserve(p_vlan_master->p_vlan_vector, 1);

    /*alloc opf for routed port vlan_ptr*/
    p_vlan_master->routed_port_vlan
    = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_vlan_info_t) * SYS_VLAN_ROUTED_PORT);

    if (NULL == p_vlan_master->routed_port_vlan)
    {
        ret = CTC_E_NO_MEMORY;
        goto error;
    }
    kal_memset(p_vlan_master->routed_port_vlan, 0, sizeof(sys_vlan_info_t) * SYS_VLAN_ROUTED_PORT);

    /*vlan status db*/
    if((ret = sys_humber_vlan_db_init()) < 0)
    {
        goto error;
    }

    /*init the asic table*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    kal_memset(&ds_vlan_status, 0, sizeof(ds_vlan_status_t));

    ds_vlan.if_id = 0x3FF;
    ds_vlan.arp_exception_type = CTC_EXCP_DISCARD;
    ds_vlan.dhcp_exception_type = CTC_EXCP_DISCARD;

    vlan_cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DRV_ENTRY_FLAG);
    vlan_status_cmd = DRV_IOW(IOC_TABLE, DS_VLAN_STATUS, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip = 0; chip < lchip_num; chip++)
    {
        for (index = 0; index < vlan_entry_num; index++)
        {
            if ((ret = drv_tbl_ioctl(chip, index, vlan_cmd, &ds_vlan)) < 0)
            {
                ret = CTC_E_HW_OP_FAIL;
                goto error;
            }
        }

        for(index = 0; index < vlan_status_entry_num; index++)
        {
            if ((ret = drv_tbl_ioctl(chip, index, vlan_status_cmd, &ds_vlan_status)) < 0)
            {
                ret = CTC_E_HW_OP_FAIL;
                goto error;
            }
        }
    }

    /*prepare vlan_ptr == 0 for vlan switching, no stp check, bridge/learning disable*/
    ds_vlan.brg_dis     = 1;
    ds_vlan.learn_dis   = 1;
    ds_vlan.rec_en      = 1;
    ds_vlan.trans_en    = 1;
    ds_vlan.if_id       = 0x3FF;
    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_SWITCH_PTR, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    /*prepare vlan_ptr == 4095 for service queue loopback*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.brg_dis     = 1;
    ds_vlan.learn_dis   = 1;
    ds_vlan.rec_en      = 1;
    ds_vlan.trans_en    = 1;
    ds_vlan.route_disable = 1;
    ds_vlan.if_id       = 0x3FF;
    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_SVQ_PTR, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    /*prepare vlan_ptr == 4860 for vpls (mac limit enable)*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.rec_en       = 1;
    ds_vlan.trans_en     = 1;
    ds_vlan.learn_dis    = 0;
    ds_vlan.mac_sec_vlan_dis = 1;
    ds_vlan.if_id        = 0x3FF;
    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_VPLS_PTR_MACLIMIT_EN, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    /*prepare vlan_ptr == 4861 for vpls (learning diable)*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.rec_en       = 1;
    ds_vlan.trans_en     = 1;
    ds_vlan.learn_dis    = 1;
    ds_vlan.if_id        = 0x3FF;
    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_VPLS_PTR_LRN_DIS, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    /*prepare vlan_ptr == 4862 for vpls (learning enable)*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.rec_en      = 1;
    ds_vlan.trans_en    = 1;
    ds_vlan.if_id       = 0x3FF;
    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_VPLS_PTR_LRN_EN, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    /*perpare vlan_ptr == 4863 for vpws*/
    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.brg_dis     = 1;
    ds_vlan.learn_dis   = 1;
    ds_vlan.rec_en      = 1;
    ds_vlan.trans_en    = 1;
    ds_vlan.if_id       = 0x3FF;

    for (chip = 0; chip < lchip_num; chip++)
    {
        if ((ret = drv_tbl_ioctl(chip, SYS_VLAN_VPWS_PTR, vlan_cmd, &ds_vlan)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto error;
        }
    }

    return CTC_E_NONE;

    error:

        if (p_vlan_master->routed_port_vlan)
        {
            mem_free(p_vlan_master->routed_port_vlan);
            p_vlan_master->routed_port_vlan = NULL;
        }

        if (p_vlan_master->p_vlan_vector)
        {
            ctc_vector_release(p_vlan_master->p_vlan_vector);
            p_vlan_master->p_vlan_vector = NULL;
        }

        if (p_vlan_master->p_vlan_mutex)
        {
            kal_mutex_destroy(p_vlan_master->p_vlan_mutex);
            p_vlan_master->p_vlan_mutex = NULL;
        }

        return ret;
}

/*alloc vlan ptr for phyical interface and sub-interface*/
static uint16
_sys_humber_vlan_get_routed_port_index(sys_vlan_info_t *l3_if)
{
    uint16 idx = 0;
    uint16 vid = 0;

    if (SYS_VLAN_PTR_TYPE_PHY_IF == l3_if->vlan_ptr_type)
    {
        vid = 0xFFFF;
    }
    else
    {
        vid = l3_if->vid;
    }

    while (idx < SYS_VLAN_ROUTED_PORT)
    {
        if ((p_vlan_master->routed_port_vlan[idx].gport == l3_if->gport)
            && (p_vlan_master->routed_port_vlan[idx].vid == vid))
        {
            break;
        }

        idx ++;
    }

    return idx;
}

static int32
_sys_humber_vlan_update_member_port(uint16 vlan_ptr, uint8 lchip, uint8 lport)
{
    int32 ret = 0;
    uint32 cmd = 0;
    uint32 index = 0;
    uint32 field_id = 0;
    uint32 vlan_status= 0;

    field_id = ((vlan_ptr >> 5)& 0x1 )? DS_VLAN_STATUS_VLANID_VALIDH : DS_VLAN_STATUS_VLANID_VALIDL;
    index = (lport << 6) +(vlan_ptr >> 6);

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN_STATUS, field_id);
    ret = drv_tbl_ioctl(lchip, index, cmd, &vlan_status);

    vlan_status &= ~(1 << (vlan_ptr % BITS_NUM_OF_WORD));

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN_STATUS, field_id);
    ret = ret? ret:drv_tbl_ioctl(lchip, index, cmd, &vlan_status);

    return CTC_E_NONE;
}

static int32
_sys_humber_vlan_get_vlan_ptr(sys_vlan_info_t *p_vlan_info, uint16 *vlan_ptr)
{
    int32 ret = CTC_E_NONE;
    uint16 if_idx = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(vlan_ptr);

    switch (p_vlan_info->vlan_ptr_type)
    {
        case SYS_VLAN_PTR_TYPE_VID:
        case SYS_VLAN_PTR_TYPE_VLAN_IF:

            CTC_VLAN_RANGE_CHECK(p_vlan_info->vid);
            *vlan_ptr = p_vlan_info->vid;
            p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, *vlan_ptr);

            if (NULL == p_vlan)
            {
                *vlan_ptr = SYS_VLAN_INVALID_PTR;
                ret = CTC_E_VLAN_NOT_CREATE;
            }

            break;

        case SYS_VLAN_PTR_TYPE_PHY_IF:
        case SYS_VLAN_PTR_TYPE_SUB_IF:

            if_idx = _sys_humber_vlan_get_routed_port_index(p_vlan_info);

            if (SYS_VLAN_ROUTED_PORT == if_idx)
            {
                *vlan_ptr = SYS_VLAN_INVALID_PTR;
                ret = CTC_E_L3IF_NOT_EXIST;
            }
            else
            {
                *vlan_ptr = if_idx + SYS_VLAN_ROUTED_PORT_BASE;
            }

            break;

        case SYS_VLAN_PTR_TYPE_VPLS_LRN_DIS:
             *vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_DIS;
            break;

        case SYS_VLAN_PTR_TYPE_VPLS_LRN_EN:
             *vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_EN;
            break;
         case SYS_VLAN_PTR_TYPE_VPLS_MACLIMIT_EN:
             *vlan_ptr = SYS_VLAN_VPLS_PTR_MACLIMIT_EN;
            break;
        case SYS_VLAN_PTR_TYPE_VPWS:
            *vlan_ptr  = SYS_VLAN_VPWS_PTR;
            break;

        default:

             *vlan_ptr  = SYS_VLAN_INVALID_PTR;
             return CTC_E_UNEXPECT;
    }

    return ret;
}

int32
sys_humber_vlan_get_vlan_ptr(sys_vlan_info_t *p_vlan_info, uint16 *vlan_ptr)
{
    int32 ret = 0;

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(vlan_ptr);

    VLAN_LOCK;
    ret =  _sys_humber_vlan_get_vlan_ptr(p_vlan_info, vlan_ptr);
    VLAN_UNLOCK;

    return ret ;
}

/**
 @brief The function is to create a vlan

*/
int32
sys_humber_vlan_create_vlan(sys_vlan_info_t *p_vlan_info)
{
    uint16 vlan_ptr = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    if ((SYS_VLAN_PTR_TYPE_VID == p_vlan_info->vlan_ptr_type)
        || (SYS_VLAN_PTR_TYPE_VLAN_IF == p_vlan_info->vlan_ptr_type))
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_info->vid);
        vlan_ptr = p_vlan_info->vid;
    }
    else if ((SYS_VLAN_PTR_TYPE_SUB_IF == p_vlan_info->vlan_ptr_type)
        || (SYS_VLAN_PTR_TYPE_PHY_IF == p_vlan_info->vlan_ptr_type))
    {

        kal_memcpy(&(p_vlan_master->routed_port_vlan[p_vlan_info->if_id]),
                    p_vlan_info,
                    sizeof(sys_vlan_info_t));

        vlan_ptr = SYS_VLAN_ROUTED_PORT_BASE + p_vlan_info->if_id;
    }
    else
    {
    }

    VLAN_LOCK;
    if (NULL != ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr))
    {
       CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_EXIST);
    }
    SYS_VLAN_DEBUG_INFO("Create vlan, Vlan ptr is == %d!\n", vlan_ptr);
    p_vlan = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_vlan_prop_t));

    if (NULL == p_vlan)
    {
       CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_NO_MEMORY);
    }

    kal_memset(p_vlan, 0, sizeof(sys_vlan_prop_t));
    /*sync up with asic*/
    p_vlan->bridge_en = 1;
    p_vlan->learn_en = 1;
    p_vlan->if_id = 0x3FF;
    p_vlan->route_en = 1;
    /*only vlan ptr for vlanid has member port*/
    if ((SYS_VLAN_PTR_TYPE_VID == p_vlan_info->vlan_ptr_type)
        || (SYS_VLAN_PTR_TYPE_VLAN_IF == p_vlan_info->vlan_ptr_type))
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(sys_humber_vlan_db_add_vlan(vlan_ptr));
    }

    if (FALSE == ctc_vector_add(p_vlan_master->p_vlan_vector, vlan_ptr, p_vlan))
    {
        SYS_VLAN_DEBUG_INFO("Add vlan to vector error, Create vlan fail!\n");
        mem_free(p_vlan);
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(sys_humber_vlan_db_remove_vlan(vlan_ptr));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief The function is to remove the vlan

*/
int32
sys_humber_vlan_remove_vlan(sys_vlan_info_t *p_vlan_info)
{
    int32 ret = CTC_E_NONE;
    uint8 lport = 0;
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    ds_vlan_t ds_vlan;
    sys_vlan_prop_t *p_node = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    kal_memset(&ds_vlan, 0, sizeof(ds_vlan_t));
    ds_vlan.if_id = 0x3FF;
    ds_vlan.arp_exception_type = CTC_EXCP_DISCARD;
    ds_vlan.dhcp_exception_type = CTC_EXCP_DISCARD;

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DRV_ENTRY_FLAG);
    lchip_num = sys_humber_get_local_chip_num();

    VLAN_LOCK;
    ret = _sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr);
    if (ret < 0)
    {
        goto error;
    }

    SYS_VLAN_DEBUG_INFO("Remove vlan, Vlan ptr is == %d!\n", vlan_ptr);

    if ((SYS_VLAN_PTR_TYPE_VID == p_vlan_info->vlan_ptr_type)
        || (SYS_VLAN_PTR_TYPE_VLAN_IF == p_vlan_info->vlan_ptr_type))
    {
        /*only vlan ptr for vlanid has member port*/
        for (chip_id = 0; chip_id < lchip_num; chip_id++)
        {
            /*clear ds_vlan*/
            if ((ret = drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &ds_vlan)) < 0)
            {
                ret = CTC_E_HW_OP_FAIL;
                goto error;
            }

            /*clear ds_vlan_status*/
            for (lport = 0; lport < CTC_MAX_HUMBER_PHY_PORT; lport++)
            {
                if (TRUE == sys_humber_vlan_db_has_port(chip_id, vlan_ptr, lport))
                {
                    if ((ret = _sys_humber_vlan_update_member_port(vlan_ptr, chip_id, lport)) < 0)
                    {
                        goto error;
                    }
                }
            }
        }

        if ((ret = sys_humber_vlan_db_remove_vlan(vlan_ptr)) < 0)
        {
            goto error;
        }
    }
    else if ((SYS_VLAN_PTR_TYPE_PHY_IF == p_vlan_info->vlan_ptr_type)
            || (SYS_VLAN_PTR_TYPE_SUB_IF == p_vlan_info->vlan_ptr_type))
    {
        /*vlan_ptr for routed & sub-if*/
        if (vlan_ptr < SYS_VLAN_ROUTED_PORT_BASE)
        {
            ret = CTC_E_L3IF_NOT_EXIST;
            goto error;
        }

        kal_memset(&(p_vlan_master->routed_port_vlan[p_vlan_info->if_id]), 0, sizeof(sys_vlan_info_t));

        /*clear ds_vlan*/
        for (chip_id = 0; chip_id < lchip_num; chip_id++)
        {
            if ((ret = drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &ds_vlan)) < 0)
            {
                ret = CTC_E_HW_OP_FAIL;
                goto error;
            }
        }

    }
    else
    {
    }

    p_node = ctc_vector_del(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_node)
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }

    mem_free(p_node);

    error:
        VLAN_UNLOCK;
        return ret;
}

/**
  @brief the function add port to vlan
*/
int32
sys_humber_vlan_add_port(sys_vlan_info_t *p_vlan_info, uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport= 0;
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 vlan_status= 0;
    uint32 index = 0;
    uint32 field_id = 0;
    uint16 vlan_ptr = 0;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    VLAN_LOCK;
    ret = _sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr);
    if (ret < 0)
    {
        goto OUT;
    }

    SYS_VLAN_DEBUG_INFO("Add global port:%d, to vlan:%d!\n", gport, vlan_ptr);

    if (FALSE == sys_humber_vlan_db_lookup(vlan_ptr))
    {
        ret = CTC_E_VLAN_NOT_CREATE;

        goto OUT;
    }

    if (TRUE == sys_humber_vlan_db_has_port(lchip, vlan_ptr, lport))
    {
        /*port already exsit in vlan*/
        SYS_VLAN_DEBUG_INFO(
        "Add lchip = %d, lport = %d to vlan = %d, port already exsit!\n", lchip, lport, vlan_ptr);

        ret = CTC_E_MEMBER_PORT_EXIST;
        goto OUT;
    }

    /*do add port to vlan*/
    if ((ret = sys_humber_vlan_db_add_port(lchip, vlan_ptr, lport)) < 0)
    {
        goto OUT;
    }

    /*read & write asic ds_vlan_status table*/
    if (lport <= CTC_MAX_HUMBER_PHY_PORT)
    {
        field_id = ((vlan_ptr >> 5) & 0x1)? DS_VLAN_STATUS_VLANID_VALIDH : DS_VLAN_STATUS_VLANID_VALIDL;
        index = (lport << 6) + (vlan_ptr >> 6);

        cmd = DRV_IOR(IOC_TABLE, DS_VLAN_STATUS, field_id);
        if ((ret = drv_tbl_ioctl(lchip, index, cmd, &vlan_status)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto OUT;
        }

        vlan_status |= 1 << (vlan_ptr % BITS_NUM_OF_WORD);

        cmd = DRV_IOW(IOC_TABLE, DS_VLAN_STATUS, field_id);
        if ((ret =  drv_tbl_ioctl(lchip, index, cmd, &vlan_status)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto OUT;
        }
    }

    OUT:
        VLAN_UNLOCK;
        return ret;
}

/**
 @brief show vlan's member ports
*/
int32
sys_humber_vlan_get_ports(sys_vlan_info_t *p_vlan_info, ctc_port_bitmap_t *port_bitmap)
{
    uint16 vlan_ptr = 0;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(port_bitmap);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));

    SYS_VLAN_DEBUG_INFO("Get member ports of vlan:%d!\n", vlan_ptr);

    if (FALSE == sys_humber_vlan_db_lookup(vlan_ptr))
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(sys_humber_vlan_db_get_ports(vlan_ptr, port_bitmap));
    VLAN_UNLOCK;

    return CTC_E_NONE;
}



/**
 @brief The function is to remove member port to a vlan
*/
int32
sys_humber_vlan_remove_port(sys_vlan_info_t *p_vlan_info, uint16 gport)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 vlan_ptr = 0;


    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Remove global port:%d, from vlan:%d!\n", gport, vlan_ptr);

    if (FALSE == sys_humber_vlan_db_lookup(vlan_ptr))
    {
        ret = CTC_E_VLAN_NOT_CREATE;
        goto OUT;
    }

    if (FALSE == sys_humber_vlan_db_has_port(lchip, vlan_ptr, lport))
    {
        SYS_VLAN_DEBUG_INFO(
        "Remove chip = %d, port = %d from vlan = %d, port not exsit!\n", lchip, lport, vlan_ptr);

        ret = CTC_E_MEMBER_PORT_NOT_EXIST;
        goto OUT;
    }

    /*do remove port from vlan*/
    if ((ret = sys_humber_vlan_db_remove_port(lchip, vlan_ptr, lport)) < 0)
    {
        goto OUT;
    }

    /*do remove port from vlan status*/
    if (lport < CTC_MAX_HUMBER_PHY_PORT)
    {
        ret = _sys_humber_vlan_update_member_port(vlan_ptr, lchip, lport);
    }

    OUT:
        VLAN_UNLOCK;
        return ret;

}

/**
 @brief The function is to set receive enable on vlan
*/
int32
sys_humber_vlan_set_receive_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_REC_EN);
    lchip_num = sys_humber_get_local_chip_num();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan:%d receive enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

    }
    VLAN_DB_SET(p_vlan->rece_en, enable);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}
int32
sys_humber_vlan_get_receive_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
     uint16 vlan_ptr = 0;
     sys_vlan_prop_t *p_vlan = NULL;

     CTC_PTR_VALID_CHECK(p_vlan_info);
     CTC_PTR_VALID_CHECK(enable);

     if (NULL == p_vlan_master)
     {
         return CTC_E_NOT_INIT;
     }

     SYS_VLAN_DEBUG_FUNC();

     VLAN_LOCK;
     CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
     SYS_VLAN_DEBUG_INFO("Get Vlan:%d receive enable!\n", vlan_ptr);

     p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

     if (NULL == p_vlan)
     {
         CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

     }
     VLAN_DB_GET(enable, p_vlan->rece_en);
     VLAN_UNLOCK;

     return CTC_E_NONE;
}


/**
 @brief The function is to set bridge enable on vlan
*/
int32
sys_humber_vlan_set_bridge_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 0:1;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_BRG_DIS);
    lchip_num = sys_humber_get_local_chip_num();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan:%d bridge enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

    }
    VLAN_DB_SET(p_vlan->bridge_en, enable);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_bridge_en(sys_vlan_info_t * p_vlan_info, bool *enable)
{
     uint16 vlan_ptr = 0;
     sys_vlan_prop_t *p_vlan = NULL;

     CTC_PTR_VALID_CHECK(p_vlan_info);
     CTC_PTR_VALID_CHECK(enable);

     if (NULL == p_vlan_master)
     {
         return CTC_E_NOT_INIT;
     }

     SYS_VLAN_DEBUG_FUNC();

     VLAN_LOCK;
     CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
     SYS_VLAN_DEBUG_INFO("Get Vlan:%d bridge enable!\n", vlan_ptr);

     p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

     if (NULL == p_vlan)
     {
         CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

     }
     VLAN_DB_GET(enable, p_vlan->bridge_en);
     VLAN_UNLOCK;

     return CTC_E_NONE;
}


/**
 @brief The function is to set tranmit enable on vlan
*/
int32
sys_humber_vlan_set_transmit_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = enable;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_TRANS_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan:%d transmit enable:%d\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

    }
    VLAN_DB_SET(p_vlan->trans_en, enable);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}
int32
sys_humber_vlan_get_transmit_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
   uint16 vlan_ptr = 0;
   sys_vlan_prop_t *p_vlan = NULL;

   CTC_PTR_VALID_CHECK(p_vlan_info);
   CTC_PTR_VALID_CHECK(enable);

   if (NULL == p_vlan_master)
   {
       return CTC_E_NOT_INIT;
   }

   SYS_VLAN_DEBUG_FUNC();

   VLAN_LOCK;
   CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
   SYS_VLAN_DEBUG_INFO("Get Vlan:%d transmit enable!\n", vlan_ptr);

   p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

   if (NULL == p_vlan)
   {
       CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

   }
   VLAN_DB_GET(enable,p_vlan->trans_en);
   VLAN_UNLOCK;

   return CTC_E_NONE;

}

int32
sys_humber_vlan_set_fid(sys_vlan_info_t *p_vlan_info, uint16 fid)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = fid;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_FID_RANGE_CHECK(fid);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_VRF_ID);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan:%d fid :%d!\n", vlan_ptr, fid);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    VLAN_DB_SET(p_vlan->vrf_id, fid);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_fid(sys_vlan_info_t *p_vlan_info, uint16 *fid)
{
    uint16 vlan_ptr = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(fid);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan:%d fid!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_GET(fid, p_vlan->vrf_id);
    VLAN_UNLOCK;

    return CTC_E_NONE;
}


/**
 @brief The fuiion is to set vrfid of l3if
*/
int32
sys_humber_vlan_set_vrfid(sys_vlan_info_t *p_vlan_info, uint16 vrfid)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = vrfid;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_VRF_ID);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan:%d vrfid :%d!\n", vlan_ptr, vrfid);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    VLAN_DB_SET(p_vlan->vrf_id, vrfid);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_vrfid(sys_vlan_info_t *p_vlan_info, uint16 *vrfid)
{
    uint16 vlan_ptr = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(vrfid);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan:%d vrfid!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_GET(vrfid, p_vlan->vrf_id);
    VLAN_UNLOCK;

    return CTC_E_NONE;
}


int32
sys_humber_vlan_set_learning_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 0:1;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_LEARN_DIS);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    SYS_VLAN_DEBUG_INFO("Set Vlan:%d learning enable:%d!\n", vlan_ptr, enable);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_SET(p_vlan->learn_en, enable);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;

}

int32
sys_humber_vlan_get_learning_en(sys_vlan_info_t * p_vlan_info, bool *enable)
{
     uint16 vlan_ptr = 0;
     sys_vlan_prop_t *p_vlan = NULL;

     CTC_PTR_VALID_CHECK(p_vlan_info);
     CTC_PTR_VALID_CHECK(enable);

     if (NULL == p_vlan_master)
     {
         return CTC_E_NOT_INIT;
     }

     SYS_VLAN_DEBUG_FUNC();

     VLAN_LOCK;
     CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
     SYS_VLAN_DEBUG_INFO("Get Vlan:%d learning enable!\n", vlan_ptr);

     p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

     if (NULL == p_vlan)
     {
         CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

     }
     VLAN_DB_GET(enable, p_vlan->learn_en);
     VLAN_UNLOCK;

     return CTC_E_NONE;
}



/**
 @brief The function is to set stp id
*/
int32
sys_humber_vlan_set_stpid(sys_vlan_info_t *p_vlan_info, uint8 stpid)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd1 = 0;
    uint32 cmd2 = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd1 = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_STP_ID);
    cmd2 = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_STP_ID6);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    SYS_VLAN_DEBUG_INFO("Set Vlan:%d stpid:%d!\n", vlan_ptr, stpid);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_SET(p_vlan->stp_id, stpid);

    for (chip = 0; chip < lchip_num; chip++)
    {
        field_value = (stpid & 0x3F);
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd1, &field_value));

        field_value = ((stpid >> 6) & 0x1);
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd2, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_stpid(sys_vlan_info_t *p_vlan_info, uint8 *stpid)
{
     uint16 vlan_ptr = 0;
     sys_vlan_prop_t *p_vlan = NULL;

     CTC_PTR_VALID_CHECK(p_vlan_info);
     CTC_PTR_VALID_CHECK(stpid);

     if (NULL == p_vlan_master)
     {
         return CTC_E_NOT_INIT;
     }

     SYS_VLAN_DEBUG_FUNC();

     VLAN_LOCK;
     CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
     SYS_VLAN_DEBUG_INFO("Get Vlan:%d stpid!\n", vlan_ptr);

     p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

     if (NULL == p_vlan)
     {
         CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);

     }
     VLAN_DB_GET(stpid, p_vlan->stp_id);
     VLAN_UNLOCK;

     return CTC_E_NONE;
}



int32
sys_humber_vlan_set_l3if_id(sys_vlan_info_t *p_vlan_info, uint16 if_id)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = if_id;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_IF_ID);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan interface id,vlan:%d if_id :%d!\n", vlan_ptr, if_id);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_SET(p_vlan->if_id, if_id);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_l3if_id(sys_vlan_info_t *p_vlan_info, uint16 *if_id)
{
    uint16 vlan_ptr = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(if_id);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan interface id: Vlan :%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_GET(if_id, p_vlan->if_id);
    VLAN_UNLOCK;

    if(*if_id == 0x3FF)
    {
        return CTC_E_L3IF_INVALID_IF_ID;
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_route_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = (enable == TRUE)?0:1;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_ROUTE_DISABLE);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan route enable, vlan:%d enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_SET(p_vlan->route_en, enable);

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;

}
int32
sys_humber_vlan_get_route_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();


    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan route enable, vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    VLAN_DB_GET(enable, p_vlan->route_en);
    VLAN_UNLOCK;

    return CTC_E_NONE;

}

int32
sys_humber_vlan_set_igmp_snoop_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_IGMP_SNOOP_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan IGMP snoop enable, vlan:%d\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(lchip, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_igmp_snoop_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_IGMP_SNOOP_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan IGMP snoop, vlan:%d\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *enable = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}


int32
sys_humber_vlan_set_v4ucast_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8  chip = 0;
    uint8  lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = (enable == TRUE)?1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V4_UCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v4ucast, vlan:%d enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}


int32
sys_humber_vlan_get_v4ucast_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V4_UCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v4ucast, vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *enable = field_value ? TRUE :FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_v4mcast_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = (enable == TRUE)?1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V4_MCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v4mcast, vlan:%d enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_v4mcast_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V4_MCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v4mcast, vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *enable = field_value ? TRUE :FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_v6ucast_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8  chip = 0;
    uint8  lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = (enable == TRUE)?1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V6_UCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v6ucast, vlan:%d enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_v6ucast_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V6_UCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v6ucast:vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *enable = field_value ? TRUE :FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_v6mcast_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = (enable == TRUE)?1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V6_MCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v6mcast, vlan:%d enable :%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_v6mcast_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint32 cmd = 0;
    uint16 vlan_ptr = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V6_MCAST_EN);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v6mcast, vlan:%d !\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *enable = field_value ? TRUE :FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_v4ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8 ipsa_lkup_type)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = ipsa_lkup_type;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);


    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V4_UCAST_SA_TYPE);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v4ucastSa_type, vlan:%d ipsa_lkup_type :%d!\n", vlan_ptr, ipsa_lkup_type);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_v4ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8 *ipsa_lkup_type)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(ipsa_lkup_type);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V4_UCAST_SA_TYPE);
    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v4ucastSa_type, vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *ipsa_lkup_type = field_value;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_v6ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8 ipsa_lkup_type)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = ipsa_lkup_type;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);


    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_V6_UCAST_SA_TYPE);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set Vlan v6ucastSa_type, vlan:%d ipsa_lkup_type :%d!\n", vlan_ptr, ipsa_lkup_type);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    for (chip = 0; chip < lchip_num; chip++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip, vlan_ptr, cmd, &field_value));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_v6ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8 *ipsa_lkup_type)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_value = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(ipsa_lkup_type);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_V6_UCAST_SA_TYPE);

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get Vlan v6ucastSa_type, vlan:%d!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_value));
    *ipsa_lkup_type = field_value;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_ether_oam_valid(sys_vlan_info_t *p_vlan_info, bool valid)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (valid == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d ehternet oam valid:%d!\n", vlan_ptr, valid);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_ETHER_OAMV);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_ether_oam_valid(sys_vlan_info_t *p_vlan_info, bool *valid)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(valid);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d ehternet oam valid!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_ETHER_OAMV);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *valid = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_md_level(sys_vlan_info_t *p_vlan_info, uint8 md_level)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = md_level;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d oam md level:%d!\n", vlan_ptr, md_level);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_MD_LEVEL);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_md_level(sys_vlan_info_t *p_vlan_info, uint8 *md_level)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(md_level);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d oam md level!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_MD_LEVEL);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *md_level = field_val;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_egress_ether_oam_valid(sys_vlan_info_t *p_vlan_info, uint8 valid)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (valid == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d ehternet oam egress valid:%d!\n", vlan_ptr, valid);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_E_ETHER_OAMV);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_egress_ether_oam_valid(sys_vlan_info_t *p_vlan_info, uint8 *valid)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(valid);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d ehternet oam egress valid!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_E_ETHER_OAMV);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *valid = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_egress_md_level(sys_vlan_info_t *p_vlan_info, uint8 md_level)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = md_level;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d oam egress md level:%d!\n", vlan_ptr, md_level);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_E_MD_LEVEL);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_egress_md_level(sys_vlan_info_t *p_vlan_info, uint8 *md_level)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(md_level);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d oam egress md level!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_E_MD_LEVEL);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *md_level = field_val;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_replace_dscp_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d relace dscp enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_REPLACE_DSCP);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_replace_dscp_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d replace dscp!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_REPLACE_DSCP);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *enable = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_src_queue_select(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d src queue select enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_SRC_QUE_SELECT);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_src_queue_select(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d src queue select!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_SRC_QUE_SELECT);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *enable = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_dhcp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t type)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = type;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_MAX_VALUE_CHECK(type, CTC_EXCP_DISCARD);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d dhcp exception type:%d!\n", vlan_ptr, type);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_DHCP_EXCEPTION_TYPE);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

extern int32
sys_humber_vlan_get_dhcp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t *type)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(type);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d dhcp exception type!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_DHCP_EXCEPTION_TYPE);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *type = field_val;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_arp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t type)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = type;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_MAX_VALUE_CHECK(type, CTC_EXCP_DISCARD);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d arp exception type:%d!\n", vlan_ptr, type);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_ARP_EXCEPTION_TYPE);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_get_arp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t *type)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(type);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d arp exception type!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_ARP_EXCEPTION_TYPE);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *type = field_val;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_mac_security_vlan_discard(sys_vlan_info_t *p_vlan_info, bool discard)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (discard == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d mac security vlan discard:%d!\n", vlan_ptr, discard);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_MAC_SEC_VLAN_DIS);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}
int32
sys_humber_vlan_get_mac_security_vlan_discard(sys_vlan_info_t *p_vlan_info, bool *discard)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(discard);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d mac security vlan discard!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_MAC_SEC_VLAN_DIS);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *discard = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_set_vlan_security_excp_en(sys_vlan_info_t *p_vlan_info, bool enable)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = (enable == TRUE)? 1:0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Set vlan:%d vlan security exception enable:%d!\n", vlan_ptr, enable);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);
    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }

    cmd = DRV_IOW(IOC_TABLE, DS_VLAN, DS_VLAN_VLAN_SEC_EXP_EN);
    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(chip_id, vlan_ptr, cmd, &field_val));
    }
    VLAN_UNLOCK;

    return CTC_E_NONE;
}
int32
sys_humber_vlan_get_vlan_security_excp_en(sys_vlan_info_t *p_vlan_info, bool *enable)
{
    uint16 vlan_ptr = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;
    sys_vlan_prop_t *p_vlan = NULL;

    CTC_PTR_VALID_CHECK(p_vlan_info);
    CTC_PTR_VALID_CHECK(enable);

    if (NULL == p_vlan_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_DEBUG_FUNC();

    VLAN_LOCK;
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(_sys_humber_vlan_get_vlan_ptr(p_vlan_info, &vlan_ptr));
    SYS_VLAN_DEBUG_INFO("Get vlan:%d vlan security exception enable!\n", vlan_ptr);

    p_vlan = ctc_vector_get(p_vlan_master->p_vlan_vector, vlan_ptr);

    if (NULL == p_vlan)
    {
        CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(CTC_E_VLAN_NOT_CREATE);
    }
    cmd = DRV_IOR(IOC_TABLE, DS_VLAN, DS_VLAN_VLAN_SEC_EXP_EN);
    CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(drv_tbl_ioctl(0, vlan_ptr, cmd, &field_val));

    *enable = (field_val == 1)? TRUE:FALSE;
    VLAN_UNLOCK;

    return CTC_E_NONE;
}


