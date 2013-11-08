/**
 @file sys_humber_port.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_debug.h"

#include "sys_humber_port.h"
#include "sys_humber_chip.h"
#include "sys_humber_l3if.h"
#include "sys_humber_usrid.h"
#include "sys_humber_vlan.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_internal_port.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_api.h"
#include "drv_io.h"
#include "drv_humber.h"
#include "drv_humber_data_path.h"
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
static sys_port_master_t *p_port_master = NULL;

/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief initialize the port module
*/
int32
sys_humber_port_init(void)
{
    int32 ret = CTC_E_NONE;
    uint32 index= 0;
    uint16 gport = 0;
    //uint8 lport = 0;
    uint32 lport = 0;
    uint8 chip = 0;
    uint8 gchip = 0;
    uint8 lchip_num = 0;
    ds_phy_port_t phy_port;
    ds_phy_port_ext_t phy_port_ext;
    ds_src_port_t src_port;
    ds_dest_phy_port_t dest_phy_port;
    phy_port_map_table_t ipe_phyport_map;
    ds_dest_port_t dest_port;
    uint32 cmd = 0;
    uint32 phy_cmd = 0;
    uint32 phy_ext_cmd= 0;
    uint32 src_cmd = 0;
    uint32 dest_phy_cmd = 0;
    uint32 dest_cmd = 0;
    uint32 oam_port_cmd = 0;

    if (p_port_master != NULL)
    {
        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();

    /*alloc&init DB and mutex*/
    p_port_master = (sys_port_master_t *)mem_malloc(MEM_PORT_MODULE, sizeof(sys_port_master_t));

    if (NULL == p_port_master)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_port_master, 0, sizeof(sys_port_master_t));
    ret = kal_mutex_create(&(p_port_master->p_port_mutex));

    if (ret || !(p_port_master->p_port_mutex))
    {
        mem_free(p_port_master);

        return CTC_E_FAIL_CREATE_MUTEX;
    }
    MALLOC_2D_POINTER(sys_igs_port_prop_t, p_port_master->igs_port_prop,
                        lchip_num, MAX_PORT_NUM_PER_CHIP);

    if (NULL == p_port_master->igs_port_prop)
    {
        kal_mutex_destroy(p_port_master->p_port_mutex);
        mem_free(p_port_master);

        return CTC_E_NO_MEMORY;
    }

    MALLOC_2D_POINTER(sys_egs_port_prop_t, p_port_master->egs_port_prop,
                        lchip_num, MAX_PORT_NUM_PER_CHIP);

    if (NULL == p_port_master->egs_port_prop)
    {
        mem_free(p_port_master->igs_port_prop);
        kal_mutex_destroy(p_port_master->p_port_mutex);
        mem_free(p_port_master);

        return CTC_E_NO_MEMORY;
    }

    /*init asic table*/
    kal_memset(&phy_port, 0, sizeof(ds_phy_port_t));
    kal_memset(&phy_port_ext, 0, sizeof(ds_phy_port_ext_t));
    kal_memset(&src_port, 0, sizeof(ds_src_port_t));
    kal_memset(&dest_phy_port, 0, sizeof(ds_dest_phy_port_t));
    kal_memset(&dest_port, 0, sizeof(ds_dest_port_t));
    kal_memset(&ipe_phyport_map, 0, sizeof(phy_port_map_table_t));

    phy_port.keep_vlan_tag = 1;
    phy_port.outer_vlan_is_cvlan = 1;
    phy_port_ext.default_vlan_id = 1;
    phy_port_ext.oam_obey_user_id =1;
    src_port.source_port_isolated = 0x3F;
    src_port.vpls_src_port = 0x1FFF;
    src_port.use_stag_cos  = 1;
    src_port.routed_port_vlan_ptr = 0x3FFF;
    src_port.port_security_en = 1;
    src_port.allow_mcast_mac_sa = 1;
    dest_port.default_vlan_id = 1;
    dest_port.bridge_l2_match_disable = 0;
    dest_port.dest_port_isolation_id = 0x3F;

    phy_cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DRV_ENTRY_FLAG);
    phy_ext_cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DRV_ENTRY_FLAG);
    src_cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DRV_ENTRY_FLAG);
    dest_phy_cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DRV_ENTRY_FLAG);
    dest_cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DRV_ENTRY_FLAG);
    oam_port_cmd = DRV_IOW(IOC_TABLE, OAM_DS_PORT_PROPERTY, OAM_DS_PORT_PROPERTY_MAC_SA_BYTE);



    for (chip = 0; chip < lchip_num; chip++)
    {
        sys_humber_get_gchip_id(chip, &gchip);

        for (index = 0; index < MAX_PORT_NUM_PER_CHIP; index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, phy_cmd, &phy_port));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, phy_ext_cmd, &phy_port_ext));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, src_cmd, &src_port));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, dest_phy_cmd, &dest_phy_port));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, dest_cmd, &dest_port));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, oam_port_cmd, &index));

            gport = CTC_MAP_LPORT_TO_GPORT(gchip, index);
            sys_humber_port_set_global_port(chip, index, gport);

            p_port_master->igs_port_prop[chip][index].keep_vlan_tag = 1;
            p_port_master->igs_port_prop[chip][index].outer_is_cvlan = 1;
            p_port_master->igs_port_prop[chip][index].default_vlan = 1;
            p_port_master->igs_port_prop[chip][index].vpls_src_port = 0x1FFF;
            p_port_master->igs_port_prop[chip][index].route_enable = 1;
            p_port_master->igs_port_prop[chip][index].learning_enable = 1;
            p_port_master->egs_port_prop[chip][index].default_vlan = 1;
            p_port_master->igs_port_prop[chip][index].l3if_id = 0x3FF;
        }

        cmd = DRV_IOW(IOC_TABLE, PHY_PORT_MAP_TABLE, DRV_ENTRY_FLAG);
        /* gmac use lport 0~47 */
        for(index=0; index<DRV_MAX_GMAC_NUM; index++)
        {
            if(drv_humber_gmac_is_enable(index))
            {
                ipe_phyport_map.local_phy_port = index;

                CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, cmd, &ipe_phyport_map));
                p_port_master->igs_port_prop[chip][index].port_mac_type = CTC_PORT_MAC_GMAC;
                p_port_master->egs_port_prop[chip][index].tx_threshold = 10;
                p_port_master->egs_port_prop[chip][index].pading_en = 1;
            }

        }
        /* xgmac use lport 0,12,24,36 */
        for(index=0; index<DRV_MAX_XGMAC_NUM; index++)
        {
            if(drv_humber_xgmac_is_enable(index))
            {
                lport = index*12;
                ipe_phyport_map.local_phy_port = lport;

                CTC_ERROR_RETURN(drv_tbl_ioctl(chip, lport, cmd, &ipe_phyport_map));
                p_port_master->igs_port_prop[chip][lport].port_mac_type = CTC_PORT_MAC_XGMAC;
                p_port_master->egs_port_prop[chip][lport].tx_threshold = 7;
                p_port_master->egs_port_prop[chip][lport].pading_en = 1;
            }
        }
        /* sgmac use lport 48,49,50,51 */
        for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
        {
            if(drv_humber_sgmac_is_enable(index))
            {
                lport = index+48;
                ipe_phyport_map.local_phy_port = lport;
                CTC_ERROR_RETURN(drv_tbl_ioctl(chip, lport, cmd, &ipe_phyport_map));
                p_port_master->igs_port_prop[chip][lport].port_mac_type = CTC_PORT_MAC_SGMAC;
                p_port_master->egs_port_prop[chip][lport].tx_threshold = 7;
                p_port_master->egs_port_prop[chip][lport].pading_en = 1;
            }
        }

        /* cpu mac use lport 53 */
        p_port_master->igs_port_prop[chip][CTC_HUMBER_LPORT_TO_CPU].port_mac_type = CTC_PORT_MAC_CPUMAC;
    }

    return CTC_E_NONE;

}

/**
 @brief set the port global_src_port and global_dest_port in system
*/
int32
sys_humber_port_set_global_port(uint8 lchip_id, uint8 lport, uint16 gport)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = gport;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_GLOBAL_PORT_CHECK(gport);
    SYS_PORT_DEBUG_INFO("Set Global port, chip_id=%d, lport=%d, gport=%d\n", lchip_id, lport, gport);

    /*do write table*/
    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip_id][lport].global_src_port, gport);
    PORT_DB_SET(p_port_master->egs_port_prop[lchip_id][lport].global_dest_port, gport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_GLOBAL_SRC_PORT);
    ret = drv_tbl_ioctl(lchip_id, lport, cmd, &field_value);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_GLOBAL_DEST_PORT);
    ret = ret? ret:drv_tbl_ioctl(lchip_id, lport, cmd, &field_value);

    cmd = DRV_IOW(IOC_TABLE, OAM_DS_PORT_PROPERTY, OAM_DS_PORT_PROPERTY_GLOBAL_SRC_PORT);
    ret = ret? ret:drv_tbl_ioctl(lchip_id, lport, cmd, &field_value);

    PORT_UNLOCK;
    return ret;
}

/**
 @brief set the port global_src_port and global_dest_port in system
*/
int32
sys_humber_port_get_global_port(uint8 lchip_id, uint8 lport, uint16 *gport)
{
    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(gport);

    /*do write table*/
    PORT_LOCK;
    PORT_DB_GET(gport, p_port_master->igs_port_prop[lchip_id][lport].global_src_port);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set port whether the tranmist is enable
*/
int32
sys_humber_port_set_transmit_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == enable) ? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d transmit enable:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    PORT_LOCK;
    PORT_DB_SET(p_port_master->egs_port_prop[lchip][lport].transmit_en, enable);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_TRANSMIT_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    PORT_UNLOCK;

    return ret;
}

/**
 @brief get port whether the tranmist is enable
*/
int32
sys_humber_port_get_transmit_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d transmit enable!\n", gport);

    /*do read*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    PORT_LOCK;
    PORT_DB_GET(enable, p_port_master->egs_port_prop[lchip][lport].transmit_en);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set port whether the receive is enable
*/
int32
sys_humber_port_set_receive_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d receive enable:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].receive_en, enable);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_RECEIVE_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief set port whether the receive is enable
*/
int32
sys_humber_port_get_receive_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d receive enable!\n", gport);

    /*do read*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    PORT_LOCK;
    PORT_DB_GET(enable, p_port_master->igs_port_prop[lchip][lport].receive_en);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set port whehter layer2 bridge function is enable
*/
int32
sys_humber_port_set_bridge_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d bridge enable:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].bridge_en, enable);
    PORT_DB_SET(p_port_master->egs_port_prop[lchip][lport].bridge_en, enable);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_BRIDGE_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_BRIDGE_EN);
    ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_set_bridge(uint16 gport, ctc_direction_t dir, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d bridge dir: %d enable:%d!\n", gport, dir, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if ((dir == CTC_INGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].bridge_en, enable);
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_BRIDGE_EN);
        ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    }
    
    if ((dir == CTC_EGRESS) || (dir == CTC_BOTH_DIRECTION))
    {
        PORT_DB_SET(p_port_master->egs_port_prop[lchip][lport].bridge_en, enable);
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_BRIDGE_EN);
        ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    }
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_set_allow_mcast_mac_sa(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d allow mcast mac sa enable:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ALLOW_MCAST_MAC_SA);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief set port whether the bridge is enable
*/
int32
sys_humber_port_get_bridge_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d bridge enable!\n", gport);

    /*do read*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    PORT_LOCK;
    PORT_DB_GET(enable, p_port_master->igs_port_prop[lchip][lport].bridge_en);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief decide the port whether is routed port
*/
int32
_sys_humber_port_set_routed_port(uint16 gport, bool is_routed)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == is_routed)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d routed port:%d!\n", gport, is_routed);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    p_port_master->igs_port_prop[lchip][lport].routed_port = is_routed;
    p_port_master->egs_port_prop[lchip][lport].routed_port = is_routed;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ROUTED_PORT);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_ROUTED_PORT);
    ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

/**
 @brief bind phy_if to port
*/
int32
sys_humber_port_set_phy_if_en(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    bool is_phy = FALSE;
    uint16 vlan_ptr = 0x3FFF;
    uint16 l3if_id = 0x3FF;
    uint32 field_value = 0;
    uint32 cmd = 0;
    sys_vlan_info_t vlan_info;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d phy_if ,l3if_id:%d enable:%d\n", gport, l3if_id, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
    vlan_info.gport         = p_port_master->igs_port_prop[lchip][lport].global_src_port;
    vlan_info.vid    = 0xFFFF;

    is_phy = sys_humber_l3if_is_port_phy_if(p_port_master->igs_port_prop[lchip][lport].global_src_port);

    if ((TRUE == enable) && (FALSE == is_phy))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L3IF_NOT_EXIST, p_port_master->p_port_mutex);
    }

    if ((FALSE == enable) && (TRUE == is_phy))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L3IF_EXIST, p_port_master->p_port_mutex);
    }

    if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            ret = sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
            ret = ret? ret:sys_humber_vlan_get_l3if_id(&vlan_info, &l3if_id);

            p_port_master->igs_port_prop[lchip][lport].l3if_id  = l3if_id;
            p_port_master->igs_port_prop[lchip][lport].flag     = SYS_PORT_PHY_IF;
            p_port_master->igs_port_prop[lchip][lport].default_vlan = 0;

            ret = ret? ret:_sys_humber_port_set_routed_port(gport, TRUE);

            field_value = vlan_ptr;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ROUTED_PORT_VLAN_PTR);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);

            field_value = 0;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_VLAN_ID);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);
        }
        else
        {
            ret = CTC_E_NONE;
        }
    }
    else if (SYS_PORT_PHY_IF == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            uint16 tmp_vlan_ptr = 0x3FFF;

            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ROUTED_PORT_VLAN_PTR);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
            if (CTC_E_NONE == ret)
            {
                tmp_vlan_ptr = field_value;
                ret = sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);
                if ((CTC_E_NONE == ret) && (vlan_ptr != tmp_vlan_ptr))
                /*Update vlan ptr*/
                {
                    field_value = vlan_ptr;
                    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ROUTED_PORT_VLAN_PTR);
                    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
                }
            }
        }
        else
        {
            p_port_master->igs_port_prop[lchip][lport].l3if_id  = l3if_id;
            p_port_master->igs_port_prop[lchip][lport].flag     = 0;
            p_port_master->igs_port_prop[lchip][lport].default_vlan = 1;

            ret = ret? ret:_sys_humber_port_set_routed_port(gport, FALSE);

            field_value = vlan_ptr;
            cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ROUTED_PORT_VLAN_PTR);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);

            field_value = 1;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_VLAN_ID);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);
        }
    }
    else
    {
        if (TRUE == enable)
        {
            ret = CTC_E_PORT_HAS_OTHER_FEATURE;
        }
        else
        {
            ret = CTC_E_PORT_FEATURE_MISMATCH;
        }
    }
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_phy_if_en(uint16 gport, uint16* l3if_id, bool*enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    CTC_PTR_VALID_CHECK(l3if_id);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d phyical interface!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    *enable = (SYS_PORT_PHY_IF == p_port_master->igs_port_prop[lchip][lport].flag)? TRUE:FALSE;
    PORT_DB_GET(l3if_id, p_port_master->igs_port_prop[lchip][lport].l3if_id);
    PORT_UNLOCK;

    return CTC_E_NONE;
}
/**
 @brief enable sub-if on tihs port
*/
int32
sys_humber_port_set_sub_if_en(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 gl_port = 0;
    bool is_sub = FALSE;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d sub_if, enable:%d\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    gl_port = p_port_master->igs_port_prop[lchip][lport].global_src_port;
    is_sub  = sys_humber_l3if_is_port_sub_if(gl_port);

    if ((TRUE == enable) && (FALSE == is_sub))
    {
        /*gport is not sub if*/
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L3IF_NOT_EXIST, p_port_master->p_port_mutex);
    }

    if ((FALSE == enable) && (TRUE == is_sub))
    {
        /*l3if exist, remove l3if first*/
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L3IF_EXIST, p_port_master->p_port_mutex);
    }

    if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_SUB_IF;

            field_val = 1;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            field_val = SYS_PORT_USRID_VLAN;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
        else
        {
            ret = CTC_E_NONE;
        }
    }
    else if (SYS_PORT_SUB_IF == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            ret = CTC_E_NONE;
        }
        else
        {
            p_port_master->igs_port_prop[lchip][lport].flag = 0;

            field_val = 0;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
    }
    else
    {
        if (TRUE == enable)
        {
            ret = CTC_E_PORT_HAS_OTHER_FEATURE;
        }
        else
        {
            ret = CTC_E_PORT_FEATURE_MISMATCH;
        }
    }
    PORT_UNLOCK;

    return ret;
}
int32
sys_humber_port_get_sub_if_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d default vid!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    *enable = (SYS_PORT_SUB_IF == p_port_master->igs_port_prop[lchip][lport].flag)? TRUE:FALSE;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set default vlan id of packet which receive from this port
*/
int32
sys_humber_port_set_default_vlan(uint16 gport, uint16 vid)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = vid;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_VLAN_RANGE_CHECK(vid);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d default vlan:%d!\n", gport, vid);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].default_vlan, vid);
    PORT_DB_SET(p_port_master->egs_port_prop[lchip][lport].default_vlan, vid);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_VLAN_ID);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_DEFAULT_VLAN_ID);
    ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;

}

/**
 @brief get default vlan id on the Port
*/
int32
sys_humber_port_get_default_vlan(uint16 gport, uint16 *vid)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(vid);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d default vid!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(vid, p_port_master->igs_port_prop[lchip][lport].default_vlan);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set default cos of vlantag of packet which receive from the port
*/
int32
sys_humber_port_set_default_pcp(uint16 gport, uint8 pcp)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = pcp;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d default pcp:%d!\n", gport, pcp);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_PCP);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

/**
 @brief get default cos on this port
*/
int32
sys_humber_port_get_default_pcp(uint16 gport, uint8 *pcp)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(pcp);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d default pcp!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_PCP);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *pcp = field_val;

    return ret;
}

/**
 @brief set default cfi of vlan tag of packet which receive from the port
*/
int32
sys_humber_port_set_default_dei(uint16 gport, uint8 dei)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = dei;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d default cfi:%d!\n", gport, dei);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_DEI);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

/**
 @brief get default cfi on this port
*/
int32
sys_humber_port_get_default_dei(uint16 gport, uint8 *dei)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(dei);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d default cfi!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_DEFAULT_DEI);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *dei = field_val;

    return ret;
}

/**
 @brief set protocol vlan enable on port
*/
int32
sys_humber_port_set_protocol_vlan_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d protocol:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PROTOCOL_VLAN_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

/**
 @brief set protocol vlan enable on port
*/
int32
sys_humber_port_get_protocol_vlan_en(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d protocol enable!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PROTOCOL_VLAN_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val) ? TRUE:FALSE;

    return ret;
}

/**
 @brief set vlan mapping enable on port
*/
int32
sys_humber_port_set_vlan_mapping_en(uint16 gport, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan mapping enable:%d\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (SYS_PORT_VLAN_MAPPING == p_port_master->igs_port_prop[lchip][lport].flag)
    /*orignal is enable*/
    {
        if (TRUE == enable)     /*current setting is enable*/
        {
            ret = CTC_E_NONE;
        }
        else                    /*current setting is disable*/
        {
            p_port_master->igs_port_prop[lchip][lport].flag = 0;

            field_val = 0;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
    }
    else
    /*orignal is disable*/
    {
        if (TRUE == enable)     /*current setting is enable*/
        {
            if (0 != p_port_master->igs_port_prop[lchip][lport].flag)
            {
                ret = CTC_E_PORT_HAS_OTHER_FEATURE;
            }
            else
            {
                p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_VLAN_MAPPING;

                field_val = SYS_PORT_USRID_VLAN;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 1;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
        }
        else                    /*current setting is disable*/
        {
            ret = CTC_E_NONE;
        }
    }
    PORT_UNLOCK;

    return ret;
}

/**
 @brief Get vlan mapping enable/disable on port
*/
int32
sys_humber_port_get_vlan_mapping_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d vlan mapping enable\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (SYS_PORT_VLAN_MAPPING == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        *enable = TRUE;
    }
    else
    {
        *enable = FALSE;
    }
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_vlan_switching_en(uint16 gport, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan switching enable:%d\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (SYS_PORT_VLAN_SWITCH == p_port_master->igs_port_prop[lchip][lport].flag)
    /*orignal is enable*/
    {
        if (TRUE == enable)     /*current setting is enable*/
        {
            ret = CTC_E_NONE;
        }
        else                    /*current setting is disable*/
        {
            p_port_master->igs_port_prop[lchip][lport].flag = 0;

            field_val = 0;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
    }
    else
    /*orignal is disable*/
    {
        if (TRUE == enable)     /*current setting is enable*/
        {
            if (0 != p_port_master->igs_port_prop[lchip][lport].flag)
            {
                ret = CTC_E_PORT_HAS_OTHER_FEATURE;
            }
            else
            {
                p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_VLAN_SWITCH;

                field_val = SYS_PORT_USRID_VLAN;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 1;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
        }
        else                    /*current setting is disable*/
        {
            ret = CTC_E_NONE;
        }
    }
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_vlan_switching_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d vlan mapping enable\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (SYS_PORT_VLAN_SWITCH == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        *enable = TRUE;
    }
    else
    {
        *enable = FALSE;
    }
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t type)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan classification enable, type:%d\n", gport, type);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    switch (type)
    {
        case CTC_VLAN_CLASS_MAC:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_VLAN_MAC;

                field_val = SYS_PORT_USRID_MAC;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 1;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else if (SYS_PORT_VLAN_MAC == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_HAS_OTHER_FEATURE;
            }

            break;

        case CTC_VLAN_CLASS_IPV4:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_VLAN_IPV4;

                field_val = SYS_PORT_USRID_IP;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 1;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else if (SYS_PORT_VLAN_IPV4 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_HAS_OTHER_FEATURE;
            }

            break;

        case CTC_VLAN_CLASS_IPV6:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_VLAN_IPV6;

                field_val = SYS_PORT_USRID_IP;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 1;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else if (SYS_PORT_VLAN_IPV6 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                field_val = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_HAS_OTHER_FEATURE;
            }

            break;

        case CTC_VLAN_CLASS_PROTOCOL:
            break;

        default:
            ret = CTC_E_VLAN_CLASS_INVALID_TYPE;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_set_vlan_classify_disable(uint16 gport, ctc_vlan_class_type_t type)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan classification disable, type:%d\n", gport, type);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    PORT_LOCK;
    switch (type)
    {
        case CTC_VLAN_CLASS_MAC:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                ret = CTC_E_NONE;
            }
            else if (SYS_PORT_VLAN_MAC == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = 0;
                field_val = FALSE;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_FEATURE_MISMATCH;
            }

            break;

        case CTC_VLAN_CLASS_IPV4:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                ret = CTC_E_NONE;
            }
            else if (SYS_PORT_VLAN_IPV4 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = 0;
                field_val = FALSE;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_FEATURE_MISMATCH;
            }

            break;

        case CTC_VLAN_CLASS_IPV6:

            if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                ret = CTC_E_NONE;
            }
            else if (SYS_PORT_VLAN_IPV6 == p_port_master->igs_port_prop[lchip][lport].flag)
            {
                p_port_master->igs_port_prop[lchip][lport].flag = 0;
                field_val = FALSE;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
                ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

                field_val = 0;
                cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
                ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
            }
            else
            {
                ret = CTC_E_PORT_FEATURE_MISMATCH;
            }
            break;

        case CTC_VLAN_CLASS_PROTOCOL:
            break;

        default:
            ret = CTC_E_VLAN_CLASS_INVALID_TYPE;
    }
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_vlan_classify_enable(uint16 gport, ctc_vlan_class_type_t* type)
{
    uint8 lchip = 0;
    uint8 lport = 0;


    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d vlan classification\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    switch (p_port_master->igs_port_prop[lchip][lport].flag)
    {
        case SYS_PORT_VLAN_MAC:
            *type = CTC_VLAN_CLASS_MAC;
            break;

        case SYS_PORT_VLAN_IPV4:
            *type = CTC_VLAN_CLASS_IPV4;
            break;

        case SYS_PORT_VLAN_IPV6:
            *type = CTC_VLAN_CLASS_IPV6;
            break;

        default:
            *type = CTC_VLAN_CLASS_MAX;
    }

    PORT_UNLOCK;

    return CTC_E_NONE;
}


/***********************************************************
*   OAM
***********************************************************/
int32
sys_humber_port_set_oam_tunnel_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (enable ==TRUE)?1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_OAM_TUNNEL_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    return ret;

}

int32
sys_humber_port_get_oam_tunnel_en(uint16 gport, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_MD_LEVEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *enable = (field_value == 1)?TRUE:FALSE;
    return ret;

}

int32
sys_humber_port_set_igs_oam_max_md_level(uint16 gport, uint8 md_level)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = md_level;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_MD_LEVEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    return ret;
}

int32
sys_humber_port_get_igs_oam_max_md_level(uint16 gport, uint8 * md_level)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_MD_LEVEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *md_level = field_value;
    return ret;
}

 int32
sys_humber_port_set_igs_oam_valid(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (enable == TRUE)?1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ETHER_OAM_VALID);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    return ret;
}

 int32
sys_humber_port_get_igs_oam_valid(uint16 gport, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_ETHER_OAM_VALID);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *enable = (field_value == 1)?TRUE:FALSE;
    return ret;
}

 extern int32
sys_humber_port_set_egs_oam_max_md_level(uint16 gport, uint8 md_level)
 {
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = md_level;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_MD_LEVEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}
extern int32
sys_humber_port_get_egs_oam_max_md_level(uint16 gport, uint8 * md_level)
 {
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_MD_LEVEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *md_level = field_value;
    return ret;
 }
extern int32
sys_humber_port_set_egs_oam_valid(uint16 gport, bool enable)
 {
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (enable ==TRUE)?1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

   SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
   cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_ETHER_OAM_VALID);
   ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
   return ret;
 }

extern int32
sys_humber_port_get_egs_oam_valid(uint16 gport, bool *enable)
 {
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_ETHER_OAM_VALID);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *enable = (field_value == 1)?TRUE:FALSE;
    return ret;

 }

/**
 @brief set port enable discard 8023 oam
*/
int32
sys_humber_port_set_discard_none_8023oam_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (enable ==TRUE)?1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_DISCARD_NON8023_OAM);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    return ret;
}

/**
 @brief set port enable replace tag with default vlan id
*/
int32
sys_humber_port_set_replace_tag_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (enable ==TRUE)?1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_DEFAULT_REPLACE_TAG_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    return ret;
}

/**
 @brief get port enable replace tag with default vlan id
*/
int32
sys_humber_port_get_replace_tag_en(uint16 gport, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_DEFAULT_REPLACE_TAG_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    *enable = (field_value == 1)?TRUE:FALSE;
    return ret;
}

/**
 @brief set port enable ptp
*/
int32
sys_humber_port_set_ptp_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 reg_step;
    uint8 field_step;
    uint16 reg_id;
    uint16 field_id;

    uint8 rx_reg_step;
    uint8 rx_field_step;
    uint16 rx_reg_id;
    uint16 rx_field_id;
    uint32 old_mac_rx_value;
    uint32 field_value;

    uint8 tx_reg_step;
    uint8 tx_field_step;
    uint16 tx_reg_id;
    uint16 tx_field_id;

#define MAX_BUFFER_COUNT_READ_TIME 1000
    uint32 rx_channel_buffer_count = 0;
    uint32 rx_channel_buffer_count_field_id = 0;
    uint32 time_out = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (p_port_master->igs_port_prop[lchip][lport].ptp_en == enable)
    {
        PORT_UNLOCK;
        return CTC_E_NONE;
    }

    switch(p_port_master->igs_port_prop[lchip][lport].port_mac_type)
    {
        case CTC_PORT_MAC_GMAC:
            reg_step = GMAC1_GMAC_PTP_EN - GMAC0_GMAC_PTP_EN;
            reg_id = GMAC0_GMAC_PTP_EN + lport*reg_step;
            field_step = GMAC1_GMAC_PTP_EN_PTP_EN - GMAC0_GMAC_PTP_EN_PTP_EN;
            field_id = GMAC0_GMAC_PTP_EN_PTP_EN + lport*field_step;

            rx_reg_step = GMAC1_GMACWRAPPER_GMAC_RX_CTRL - GMAC0_GMACWRAPPER_GMAC_RX_CTRL;
            rx_reg_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL + lport*reg_step;
            rx_field_step = GMAC1_GMACWRAPPER_GMAC_RX_CTRL_RX_ENABLE - GMAC0_GMACWRAPPER_GMAC_RX_CTRL_RX_ENABLE;
            rx_field_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL_RX_ENABLE + lport*field_step;

            tx_reg_step = GMAC1_GMACWRAPPER_GMAC_TX_CTRL - GMAC0_GMACWRAPPER_GMAC_TX_CTRL;
            tx_reg_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL + lport*reg_step;
            tx_field_step = GMAC1_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD - GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD;
            tx_field_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD + lport*field_step;

            break;
        case CTC_PORT_MAC_XGMAC:
            reg_step = XGMAC1_XGMAC_PTP_EN - XGMAC0_XGMAC_PTP_EN;
            reg_id = XGMAC0_XGMAC_PTP_EN + (lport/12)*reg_step;
            field_step = XGMAC1_XGMAC_PTP_EN_PTP_EN - XGMAC0_XGMAC_PTP_EN_PTP_EN;
            field_id = XGMAC0_XGMAC_PTP_EN_PTP_EN + (lport/12)*field_step;

            rx_reg_step = XGMAC1_XGMAC_CONFIG1 - XGMAC0_XGMAC_CONFIG1;
            rx_reg_id = XGMAC0_XGMAC_CONFIG1 + (lport/12)*reg_step;
            rx_field_step = XGMAC1_XGMAC_CONFIG1_RX_ENABLE - XGMAC0_XGMAC_CONFIG1_RX_ENABLE;
            rx_field_id = XGMAC0_XGMAC_CONFIG1_RX_ENABLE + (lport/12)*field_step;

            tx_reg_step = XGMAC1_XGMAC_CONFIG4 - XGMAC0_XGMAC_CONFIG4;
            tx_reg_id = XGMAC0_XGMAC_CONFIG4 + (lport/12)*reg_step;
            tx_field_step = XGMAC1_XGMAC_CONFIG4_TX_THRESHOLD - XGMAC0_XGMAC_CONFIG4_TX_THRESHOLD;
            tx_field_id = XGMAC0_XGMAC_CONFIG4_TX_THRESHOLD + (lport/12)*field_step;
            break;
        case CTC_PORT_MAC_SGMAC:
            reg_step = SGMAC1_SGMAC_PTP_EN - SGMAC0_SGMAC_PTP_EN;
            reg_id = SGMAC0_SGMAC_PTP_EN + (lport-SYS_MAX_GMAC_NUM)*reg_step;
            field_step = SGMAC1_SGMAC_PTP_EN_PTP_EN - SGMAC0_SGMAC_PTP_EN_PTP_EN;
            field_id = SGMAC0_SGMAC_PTP_EN_PTP_EN + (lport-SYS_MAX_GMAC_NUM)*field_step;

            rx_reg_step = SGMAC1_SGMAC_CONFIG1 - SGMAC0_SGMAC_CONFIG1;
            rx_reg_id = SGMAC0_SGMAC_CONFIG1 + (lport-SYS_MAX_GMAC_NUM)*reg_step;
            rx_field_step = SGMAC1_SGMAC_CONFIG1_RX_ENABLE - SGMAC0_SGMAC_CONFIG1_RX_ENABLE;
            rx_field_id = SGMAC0_SGMAC_CONFIG1_RX_ENABLE + (lport-SYS_MAX_GMAC_NUM)*field_step;

            tx_reg_step = SGMAC1_SGMAC_CONFIG4 - SGMAC0_SGMAC_CONFIG4;
            tx_reg_id = SGMAC0_SGMAC_CONFIG4 + (lport-SYS_MAX_GMAC_NUM)*reg_step;
            tx_field_step = SGMAC1_SGMAC_CONFIG4_TX_THRESHOLD - SGMAC0_SGMAC_CONFIG4_TX_THRESHOLD;
            tx_field_id = SGMAC0_SGMAC_CONFIG4_TX_THRESHOLD + (lport-SYS_MAX_GMAC_NUM)*field_step;
            break;
        default:
            PORT_UNLOCK;
            return CTC_E_INVALID_PORT_MAC_TYPE;
    }

    /* store the original mac rx enable value*/
    cmd = DRV_IOR(IOC_REG, rx_reg_id, rx_field_id);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &old_mac_rx_value), p_port_master->p_port_mutex);

    /* disable MAC rxEn */
    field_value = 0;
    cmd = DRV_IOW(IOC_REG, rx_reg_id, rx_field_id);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &field_value), p_port_master->p_port_mutex);

    /* enable/disable mac ptp en */
    field_value = enable?1:0;
    cmd = DRV_IOW(IOC_REG, reg_id, field_id);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &field_value), p_port_master->p_port_mutex);

    /* read channel buffer counter until it is cleared */
    rx_channel_buffer_count_field_id = NET_RX_CHANNEL_BUFFER_COUNT_CHANNEL_BUFFER_COUNT0 +lport;
    cmd = DRV_IOR(IOC_TABLE, NET_RX_CHANNEL_BUFFER_COUNT, rx_channel_buffer_count_field_id);

    do
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &rx_channel_buffer_count), p_port_master->p_port_mutex);
        time_out++;
    }while((rx_channel_buffer_count != 0) && (time_out<MAX_BUFFER_COUNT_READ_TIME));

    field_value = enable ? 0 : p_port_master->egs_port_prop[lchip][lport].tx_threshold;
    cmd = DRV_IOW(IOC_REG, tx_reg_id, tx_field_id);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &field_value), p_port_master->p_port_mutex);

    field_value = enable?1:0;
    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_PTP_EN);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_tbl_ioctl(lchip, lport, cmd, &field_value), p_port_master->p_port_mutex);

    /* restore MAC rxEn */
    cmd = DRV_IOW(IOC_REG, rx_reg_id, rx_field_id);
    CTC_ERROR_RETURN_WITH_UNLOCK(drv_reg_ioctl(lchip, 0, cmd, &old_mac_rx_value), p_port_master->p_port_mutex);
    p_port_master->igs_port_prop[lchip][lport].ptp_en = enable;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief get port enable ptp
*/
int32
sys_humber_port_get_ptp_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    PORT_LOCK;
    *enable = p_port_master->igs_port_prop[lchip][lport].ptp_en;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set port's vlan tag control mode
*/
int32
sys_humber_port_set_vlanctl(uint16 gport, ctc_vlantag_ctl_t mode)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = mode;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    if (mode >= MAX_CTC_VLANTAG_CTL)
    {
        return CTC_E_EXCEED_MAX_VLANCTL;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan tag control:%d!\n", gport, mode);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].vlanctl_mode, mode);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_VLAN_TAG_CTL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief get port's vlan tag control mode
*/
int32
sys_humber_port_get_vlanctl(uint16 gport, ctc_vlantag_ctl_t *mode)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(mode);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d vlan tag control!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(mode, p_port_master->igs_port_prop[lchip][lport].vlanctl_mode);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief set packet received from this port vlan tag outer vlan is cvlan
*/
int32
sys_humber_port_set_outer_is_cvlan(uint16 gport, bool is_cvlan)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (is_cvlan == TRUE)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d outer is cvlan:%d!\n", gport, is_cvlan);
    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].outer_is_cvlan, is_cvlan);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_OUTER_VLAN_IS_CVLAN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief Get packet received from this port vlan tag outer vlan is cvlan
*/
int32
sys_humber_port_get_outer_is_cvlan(uint16 gport, bool *is_cvlan)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(is_cvlan);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d outer is cvlan!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(is_cvlan, p_port_master->igs_port_prop[lchip][lport].outer_is_cvlan);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_src_outer_is_svlan(uint16 gport, bool is_svlan)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (is_svlan == TRUE)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d outer is svlan:%d!\n", gport, is_svlan);
    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].src_outer_is_svlan, is_svlan);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_SRC_OUTER_VLAN_IS_SVLAN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_src_outer_is_svlan(uint16 gport, bool *is_svlan)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(is_svlan);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d outer is svlan!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(is_svlan, p_port_master->igs_port_prop[lchip][lport].src_outer_is_svlan);
    PORT_UNLOCK;

    return CTC_E_NONE;
}


/**
 @brief For double tagged packet, this port is use ctag cos or not
*/
int32
sys_humber_port_set_use_inner_cos(uint16 gport, bool is_inner)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (is_inner == TRUE)? 0:1;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d use inner cos:%d!\n", gport, is_inner);
    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_USE_STAG_COS);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief For double tagged packet, Get this port is use ctag cos
*/
int32
sys_humber_port_get_use_inner_cos(uint16 gport, bool* is_inner)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(is_inner);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d use stag cos!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_USE_STAG_COS);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    *is_inner = (field_value == TRUE)? 0:1;

    return CTC_E_NONE;
}


static int32
_sys_humber_port_set_stag_tpid_index_igs(uint8 lchip, uint8 lport, uint8 index)
{
    int32 ret = 0;
    uint32 cmd = 0;
    uint32 field_val = index;

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_SVLAN_TPID_INDEX);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

static int32
_sys_humber_port_set_stag_tpid_index_egs(uint8 lchip, uint8 lport, uint8 index)
{
    int32 ret = 0;
    uint32 cmd = 0;
    uint32 field_val = index;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_SVLAN_TPID_INDEX);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_set_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 index)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_MAX_VALUE_CHECK(index, 3);

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%dsvlan tpid direction:%d, index:%d\n", gport, dir, index);


    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        CTC_ERROR_RETURN(_sys_humber_port_set_stag_tpid_index_igs(lchip, lport, index));
    }

    if((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        CTC_ERROR_RETURN(_sys_humber_port_set_stag_tpid_index_egs(lchip, lport, index));
    }

    return CTC_E_NONE;
}

int32
sys_humber_port_get_stag_tpid_index(uint16 gport, ctc_direction_t dir, uint8 *index)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(index);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d svlan tpid index direction:%d!\n", gport, dir);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_SVLAN_TPID_INDEX);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_SVLAN_TPID_INDEX);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *index = field_val;

    return ret;
}


static int32
_sys_humber_port_set_igs_vlan_filter(uint8 lchip, uint8 lport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_INGRESS_FILTERING_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_egs_vlan_filter(uint8 lchip, uint8 lport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_EGRESS_FILTER_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

/**
 @brief set vlan filtering enable/disable on the port
*/
int32
sys_humber_port_set_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d vlan filtering direction:%d enable:%d!\n", gport, dir, enable);
    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        CTC_ERROR_RETURN(_sys_humber_port_set_igs_vlan_filter(lchip, lport, enable));
    }

    if((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        CTC_ERROR_RETURN(_sys_humber_port_set_egs_vlan_filter(lchip, lport, enable));
    }

    return CTC_E_NONE;
}

/**
 @brief Get vlan filtering enable/disable on the port
*/
int32
sys_humber_port_get_vlan_filter_en(uint16 gport, ctc_direction_t dir, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d vlan filtering direction:%d enable!\n", gport, dir);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_INGRESS_FILTERING_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_EGRESS_FILTER_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

/**
 @brief Set port cross connect
*/
int32
sys_humber_port_set_cross_connect(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d cross connect enable:%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_CROSS_CONNECT);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;

}

/**
 @brief Get port cross connect
*/
int32
sys_humber_port_get_cross_connect(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d cross connect!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_CROSS_CONNECT);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val) ? TRUE:FALSE;

    return ret;
}

/**
 @brief Set learning enable/disable on port
*/
int32
sys_humber_port_set_learning_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 0:1;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d learning enable:%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->igs_port_prop[lchip][lport].learning_enable, enable);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_LEARNING_DISABLE);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief Get learning enable/disable on port
*/
int32
sys_humber_port_get_learning_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d learning enable!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(enable, p_port_master->igs_port_prop[lchip][lport].learning_enable);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_keep_vlan_tag(uint16 gport, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d keep vlan tag enable:%d\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_KEEP_VLAN_TAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    return CTC_E_NONE;
}

int32
sys_humber_port_get_keep_vlan_tag(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d keep vlan tag\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_KEEP_VLAN_TAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    *enable = (field_val == 1)? TRUE:FALSE;

    return CTC_E_NONE;
}

/**
 @brief Set port dot1q type
*/
int32
sys_humber_port_set_dot1q_type(uint16 gport, ctc_dot1q_type_t type)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_val = type;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_MAX_VALUE_CHECK(type, CTC_DOT1Q_TYPE_BOTH);

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d dot1q type(0-untag,1-ctag,2-stag,3-dtag):%d!\n", gport, type);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_SET(p_port_master->egs_port_prop[lchip][lport].dot1q_type, type);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_DOT1Q_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    PORT_UNLOCK;

    return ret;
}

/**
 @brief Get port dot1q type
*/
int32
sys_humber_port_get_dot1q_type(uint16 gport, ctc_dot1q_type_t *type)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(type);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d dot1q type(0-untag,1-ctag,2-stag,3-dtag)!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    PORT_DB_GET(type, p_port_master->egs_port_prop[lchip][lport].dot1q_type);
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_use_outer_ttl(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port%d use outer ttl enable%d\n", gport, enable);


    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_USE_OUTER_TTL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_use_outer_ttl(uint16 gport, bool *enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d use outer ttl.\n", gport);

    CTC_PTR_VALID_CHECK(enable);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_USE_OUTER_TTL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

/**
 @brief Set port untag default vlan id
*/
int32
sys_humber_port_set_untag_dft_vid(uint16 gport, bool enable, bool untag_svlan)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_val;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d untag default vlan id enable:%d, is svlan:%d!\n", gport, enable, untag_svlan);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    field_val = (TRUE == untag_svlan)? 1:0;
    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UNTAG_DEFAULT_SVLAN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    field_val = (TRUE == enable)? 1:0;
    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UNTAG_DEFAULT_VLAN_ID);
    ret = ret ? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_untag_dft_vid(uint16 gport, bool *enable, bool *untag_svlan)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    CTC_PTR_VALID_CHECK(untag_svlan);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d untag default vlan!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UNTAG_DEFAULT_VLAN_ID);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UNTAG_DEFAULT_SVLAN);
    ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *untag_svlan = (1 == field_val)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_exception_en(uint16 gport, uint16 bitmap)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = bitmap;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d exception enable:0x%2X!\n", gport, bitmap);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_EXCEPTION2_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_exception_en(uint16 gport, uint16 *bitmap)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d exception enable!\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_EXCEPTION2_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *bitmap = field_val;

    return ret;
}

int32
sys_humber_port_set_exception_discard(uint16 gport, uint16 bitmap)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = bitmap;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d exception discard:0x%2X!\n", gport, bitmap);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_EXCEPTION2_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_exception_discard(uint16 gport, uint16 *bitmap)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d exception discard!\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_EXCEPTION2_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *bitmap = field_val;

    return ret;
}

int32
sys_humber_port_set_security_excp_en(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d security exception enable:%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_SECURITY_EXCEPTION_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_security_excp_en(uint16 gport, bool *enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d security exception!\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_SECURITY_EXCEPTION_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (field_val == 1)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_security_en(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d security enable :%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_SECURITY_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}
int32
sys_humber_port_get_security_en(uint16 gport, bool *enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d security exception!\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_SECURITY_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (field_val == 1)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_mac_security_discard(uint16 gport, bool discard)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == discard)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d mac security discard:%d!\n", gport, discard);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_MAC_SECURITY_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

int32
sys_humber_port_get_mac_security_discard(uint16 gport, bool *discard)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port%d mac security discard!\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_MAC_SECURITY_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *discard = (field_val == 1)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_ipsg_en(uint16 gport, bool enable)
{
    int32 ret = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d ip source guard enable:%d", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (0 == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            p_port_master->igs_port_prop[lchip][lport].flag = SYS_PORT_IPSRC_GUARD;

            field_val = 1;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            field_val = SYS_PORT_USRID_IP;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            field_val = SYS_USRID_RESERVE_LABEL_FOR_IPSG;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
        else
        {
            ret = CTC_E_NONE;
        }
    }
    else if (SYS_PORT_IPSRC_GUARD == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        if (TRUE == enable)
        {
            ret = CTC_E_NONE;
        }
        else
        {
            p_port_master->igs_port_prop[lchip][lport].flag = 0;

            field_val = 0;
            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
            ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);

            cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
            ret = ret? ret:drv_tbl_ioctl(lchip, lport, cmd, &field_val);
        }
    }
    else
    {
        if (TRUE == enable)
        {
            ret = CTC_E_PORT_HAS_OTHER_FEATURE;
        }
        else
        {
            ret = CTC_E_PORT_FEATURE_MISMATCH;
        }
    }
    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_ipsg_en(uint16 gport, bool *enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d IP source guard enable\n", gport);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    if (SYS_PORT_IPSRC_GUARD == p_port_master->igs_port_prop[lchip][lport].flag)
    {
        *enable = TRUE;
    }
    else
    {
        *enable = FALSE;
    }
    PORT_UNLOCK;

    return CTC_E_NONE;
}


int32
sys_humber_port_set_isolation_id(uint16 gport, ctc_direction_t dir, uint8 isolation_id)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = isolation_id;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if((CTC_INGRESS == dir)||(CTC_BOTH_DIRECTION== dir))
    {
        cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_SOURCE_PORT_ISOLATED);
        ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    }

    if((CTC_EGRESS == dir)||(CTC_BOTH_DIRECTION== dir))
    {
        cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_DEST_PORT_ISOLATION_ID);
        ret = ret ? ret : drv_tbl_ioctl(lchip, lport, cmd, &field_value);
    }

    return ret;
}

int32
sys_humber_port_get_isolation_id(uint16 gport, ctc_direction_t dir, uint8 *isolation_id)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(isolation_id);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write table*/
    if((CTC_INGRESS == dir)||(CTC_BOTH_DIRECTION== dir))
    {
        cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_SOURCE_PORT_ISOLATED);
        ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    }

    if((CTC_EGRESS == dir)||(CTC_BOTH_DIRECTION== dir))
    {
        cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_DEST_PORT_ISOLATION_ID);
        ret = ret ? ret : drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    }

    *isolation_id = field_val;

    return ret;
}

int32
sys_humber_port_set_ucast_flooding_en(uint16 gport, bool enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val = (TRUE == enable)? 0:1;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UCAST_FLOODING_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    return CTC_E_NONE;
}

int32
sys_humber_port_get_ucast_flooding_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_PTR_VALID_CHECK(enable);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_UCAST_FLOODING_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    *enable = (0 == field_val)? TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_mcast_flooding_en(uint16 gport, bool enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val = (TRUE == enable)? 0:1;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_MCAST_FLOODING_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    return CTC_E_NONE;
}

int32
sys_humber_port_get_mcast_flooding_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_PTR_VALID_CHECK(enable);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_MCAST_FLOODING_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    *enable = (0 == field_val)? TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_reflective_bridge_en(uint16 gport, bool enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_BRIDGE_L2_MATCH_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    return CTC_E_NONE;
}

int32
sys_humber_port_get_reflective_bridge_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_PTR_VALID_CHECK(enable);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_BRIDGE_L2_MATCH_DISABLE);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &field_val));

    *enable = (1 == field_val)? TRUE:FALSE;

    return CTC_E_NONE;
}

static int32
_sys_humber_port_set_random_log_igs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_RANDOM_LOG_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_random_log_egs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_RANDOM_LOG_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_random_log_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_random_log_igs_en(lchip, lport, enable);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_random_log_egs_en(lchip, lport, enable);
    }

    return ret;
}

int32
sys_humber_port_get_random_log_en(uint16 gport, ctc_direction_t dir, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);

    /*do read, soft table high priority*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_RANDOM_LOG_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_RANDOM_LOG_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_random_threshold_igs(uint8 chip, uint8 port, uint16 threshold)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = threshold;

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_RANDOM_THRESHOLD);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_random_threshold_egs(uint8 chip, uint8 port, uint16 threshold)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = threshold;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_RANDOM_THRESHOLD);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_random_threshold(uint16 gport, ctc_direction_t dir, uint16 threshold)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    if (threshold > 0x7FFF)
    {
        return CTC_E_INVALID_PARAM;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_random_threshold_igs(lchip, lport, threshold);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_random_threshold_egs(lchip, lport, threshold);
    }

    return ret;
}

int32
sys_humber_port_get_random_threshold(uint16 gport, ctc_direction_t dir, uint16 *threshold)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(threshold);

    /*do read, soft table high priority*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_RANDOM_THRESHOLD);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_RANDOM_THRESHOLD);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *threshold = field_val & 0x7FFF;

    return ret;
}

/**
 @brief mirror APIs for sys calling
*/
static int32
_sys_humber_port_set_span_igs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_L2_SPAN_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_span_egs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_L2_SPAN_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_span_en(uint16 gport, ctc_direction_t dir, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_span_igs_en(lchip, lport, enable);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_span_egs_en(lchip, lport, enable);
    }

    return ret;
}

int32
sys_humber_port_get_span_en(uint16 gport, ctc_direction_t dir, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);

    /*do read, soft table high priority*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_L2_SPAN_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_L2_SPAN_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_span_igs_id(uint8 chip, uint8 port, uint8 span_id)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = span_id;

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_L2_SPAN_ID);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;

}

static int32
_sys_humber_port_set_span_egs_id(uint8 chip, uint8 port, uint8 span_id)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = span_id;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_L2_SPAN_ID);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_span_id(uint16 gport, ctc_direction_t dir, uint8 span_id)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_span_igs_id(lchip, lport, span_id);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_span_egs_id(lchip, lport, span_id);
    }

    return ret;
}

int32
sys_humber_port_get_span_id(uint16 gport, ctc_direction_t dir, uint8 *span_id)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(span_id);

    /*do read, soft table high priority*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_L2_SPAN_ID);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PHY_PORT, DS_DEST_PHY_PORT_L2_SPAN_ID);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *span_id = field_val;

    return ret;
}


/**
 @brief Userid APIs for sys calling
*/
int32
sys_humber_port_set_usrid_enable(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_usrid_enable(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);

    /*do read table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_usrid_type(uint16 gport, sys_port_usrid_type_t type)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_value = type;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_usrid_type(uint16 gport, sys_port_usrid_type_t *type)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(type);

    /*do read table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_TYPE);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *type = field_val;

    return ret;
}


int32
sys_humber_port_set_usrid_label(uint16 gport, uint8 label_id)
{
    uint32 cmd = 0;
    int32 ret= CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = label_id;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_usrid_label(uint16 gport, uint8 *label_id)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(label_id);

    /*do read table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DS_PHY_PORT_EXT_USER_ID_LABEL);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *label_id = field_val;

    return ret;
}

/**
 @brief Acl&Qos APIs for sys calling
*/

static int32
_sys_humber_port_set_l2acl_igs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2acl_egs_en(uint8 chip, uint8 port, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_l2acl_enable(uint16 gport, ctc_direction_t dir, bool enable)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    int32 ret = CTC_E_NONE;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2acl_igs_en(lchip, lport, enable);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2acl_egs_en(lchip, lport, enable);
    }

    return ret;
}

int32
sys_humber_port_get_l2acl_enable(uint16 gport, ctc_direction_t dir, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_l2acl_igs_label(uint8 chip, uint8 port, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = label;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_LABEL);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2acl_egs_label(uint8 chip, uint8 port, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = label;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_LABEL);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_l2acl_label(uint16 gport, ctc_direction_t dir, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2acl_igs_label(lchip, lport, label);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2acl_egs_label(lchip, lport, label);
    }

    return ret;
}

int32
sys_humber_port_get_l2acl_label(uint16 gport, ctc_direction_t dir, uint8 *label)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_LABEL);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_LABEL);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *label = field_val;

    return ret;
}

static int32
_sys_humber_port_set_l2acl_igs_prio(uint8 chip, uint8 port, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == l2_high_prio)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_HIGH_PRIORITY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2acl_egs_prio(uint8 chip, uint8 port, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == l2_high_prio)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_HIGH_PRIORITY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_l2acl_prio(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2acl_igs_prio(lchip, lport, l2_high_prio);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2acl_egs_prio(lchip, lport, l2_high_prio);
    }

    return ret;
}

int32
sys_humber_port_get_l2acl_prio(uint16 gport, ctc_direction_t dir, bool *l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_ACL_HIGH_PRIORITY);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_ACL_HIGH_PRIORITY);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *l2_high_prio = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_l2qos_igs_en(uint8 chip, uint8 port, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_LOOKUP_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2qos_egs_en(uint8 chip, uint8 port, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_LOOKUP_EN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}


int32
sys_humber_port_set_l2qos_enable(uint16 gport, ctc_direction_t dir, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2qos_igs_en(lchip, lport, enable);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2qos_egs_en(lchip, lport, enable);
    }

    return ret;
}

int32
sys_humber_port_get_l2qos_enable(uint16 gport, ctc_direction_t dir, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_LOOKUP_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_LOOKUP_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_l2qos_igs_label(uint8 chip, uint8 port, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = label;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_LABEL);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2qos_egs_label(uint8 chip, uint8 port, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = label;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_LABLE);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_l2qos_label(uint16 gport, ctc_direction_t dir, uint8 label)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2qos_igs_label(lchip, lport, label);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2qos_egs_label(lchip, lport, label);
    }

    return ret;
}

int32
sys_humber_port_get_l2qos_label(uint16 gport, ctc_direction_t dir, uint8 *label)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_LOOKUP_EN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_LOOKUP_EN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *label = field_val;

    return ret;
}

static int32
_sys_humber_port_set_l2qos_igs_prio(uint8 chip, uint8 port, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == l2_high_prio)? TRUE:FALSE;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_HIGH_PRIORITY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_l2qos_egs_prio(uint8 chip, uint8 port, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == l2_high_prio)? TRUE:FALSE;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_HIGH_PRIORITY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_l2qos_prio(uint16 gport, ctc_direction_t dir, bool l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_l2qos_igs_prio(lchip, lport, l2_high_prio);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_l2qos_egs_prio(lchip, lport, l2_high_prio);
    }

    return ret;
}

int32
sys_humber_port_get_l2qos_prio(uint16 gport, ctc_direction_t dir, bool *l2_high_prio)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_L2_QOS_HIGH_PRIORITY);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_L2_QOS_HIGH_PRIORITY);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *l2_high_prio = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_igs_qos_domain(uint8 chip, uint8 port, uint8 qos_domain)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = qos_domain;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_QOS_DOMAIN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_egs_qos_domain(uint8 chip, uint8 port, uint8 qos_domain)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = qos_domain;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_QOS_DOMAIN);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_qos_domain(uint16 gport, ctc_direction_t dir, uint8 domain)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_igs_qos_domain(lchip, lport, domain);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_egs_qos_domain(lchip, lport, domain);
    }

    return ret;
}

int32
sys_humber_port_get_qos_domain(uint16 gport, ctc_direction_t dir, uint8 *domain)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_QOS_DOMAIN);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_QOS_DOMAIN);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *domain = field_val;

    return ret;
}

int32
sys_humber_port_set_qos_policy(uint16 gport, uint8 policy)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = policy;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_QOS_POLICY);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_qos_policy(uint16 gport, uint8 *policy)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(policy);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_QOS_POLICY);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *policy = field_val;

    return ret;
}

static int32
_sys_humber_port_set_port_igs_policer_valid(uint8 chip, uint8 port, bool valid)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == valid)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_POLICER_VALID);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_port_egs_policer_valid(uint8 chip, uint8 port, bool valid)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == valid)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_PORT_POLICER_VALID);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_port_policer_valid(uint16 gport, ctc_direction_t dir, bool valid)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_port_igs_policer_valid(lchip, lport, valid);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_port_egs_policer_valid(lchip, lport, valid);
    }

    return ret;
}

int32
sys_humber_port_get_port_policer_valid(uint16 gport, ctc_direction_t dir,  bool *valid)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_POLICER_VALID);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_PORT_POLICER_VALID);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *valid = (field_val == 1)? TRUE:FALSE;

    return ret;
}


static int32
_sys_humber_port_set_qacl_igs_force_ipv4_mackey(uint8 chip, uint8 port, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == force_mac_key)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_FORCE_ACL_QOS_IPV4_TO_MAC_KEY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_qacl_egs_force_ipv4_mackey(uint8 chip, uint8 port, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == force_mac_key)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_FORCE_IPV4_TO_MAC_KEY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}


int32
sys_humber_port_set_qacl_force_ipv4_mackey(uint16 gport, ctc_direction_t dir, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_qacl_igs_force_ipv4_mackey(lchip, lport, force_mac_key);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_qacl_egs_force_ipv4_mackey(lchip, lport, force_mac_key);
    }

    return ret;

}

int32
sys_humber_port_get_qacl_force_ipv4_mackey(uint16 gport, ctc_direction_t dir, bool * force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_FORCE_ACL_QOS_IPV4_TO_MAC_KEY);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_FORCE_IPV4_TO_MAC_KEY);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *force_mac_key = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_qacl_igs_force_ipv6_mackey(uint8 chip, uint8 port, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == force_mac_key)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_FORCE_ACL_QOS_IPV6_TO_MAC_KEY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

static int32
_sys_humber_port_set_qacl_egs_force_ipv6_mackey(uint8 chip, uint8 port, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 field_value = (TRUE == force_mac_key)? 1:0;

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_FORCE_IPV6_TO_MAC_KEY);
    ret = drv_tbl_ioctl(chip, port, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_set_qacl_force_ipv6_mackey(uint16 gport, ctc_direction_t dir, bool force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do write*/
    if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = _sys_humber_port_set_qacl_igs_force_ipv6_mackey(lchip, lport, force_mac_key);
    }

    if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
    {
        ret = ret? ret:_sys_humber_port_set_qacl_egs_force_ipv6_mackey(lchip, lport, force_mac_key);
    }

    return ret;
}

int32
sys_humber_port_get_qacl_force_ipv6_mackey(uint16 gport, ctc_direction_t dir, bool *force_mac_key)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*do read*/
    switch (dir)
    {
        case CTC_INGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_FORCE_ACL_QOS_IPV6_TO_MAC_KEY);
            break;

        case CTC_EGRESS:
            cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_FORCE_IPV6_TO_MAC_KEY);
            break;

        default:
            return CTC_E_INVALID_DIR;
    }

    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);
    *force_mac_key = (1 == field_val)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_replace_cos_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d replace cos enable:%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_REPLACE_COS);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_replace_cos_en(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d replace cos!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_REPLACE_COS);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

int32
sys_humber_port_set_replace_dscp_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d replace dscp enable:%d!\n", gport, enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_REPLACE_DSCP);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

int32
sys_humber_port_get_replace_dscp_en(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d replace dscp!\n", gport);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_DEST_PORT, DS_DEST_PORT_REPLACE_DSCP);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val)? TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_hss4g_macro_enable(uint8 lchip, uint8 lserdes_id, ctc_serdes_mode_t serdes_mode)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;    
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 0;
    uint32 hss_div_sel, width, rate;
    uint8 lport;
    uint32 hss_ready;

    table_id = HSS_N0_CTL + lserdes_id;
    /* 1.  -------- Cfg Hss4G ---------*/
    /* 1.1 ---cfg Hss4G PLL power up ---*/    
    field_id = HSS_N0_CTL_CFG_N0_HSS_PDWN_PLL;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    /* 1.2 ---cfg all link of Rx/Tx power up in one macro ---*/    
    for(lport=0; lport<CTC_MAC_NUM_PER_MACRO; lport++)
    {
        if((lport%CTC_MAC_NUM_PER_MACRO) < 4)
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+((lport%CTC_MAC_NUM_PER_MACRO) * 16);
        }
        else
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+4*16+((lport%CTC_MAC_NUM_PER_MACRO)-4)*15;
        }
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
        field_id = HSS_N0_CTL_CFG_N0_TXA_PWR_DWN+(lport * 13);
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    }
    /* 2.  -------- Release Hss4G reset ---------*/    
    field_id = HSS_N0_CTL_CFG_N0_HSS_RESET;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    /*3. ------ set Hss4G reset  -------*/
    field_val = 1;
    field_id = HSS_N0_CTL_CFG_N0_HSS_RESET;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*4. -- cfg Hss4G control regs, modify DivSel, Rx/Tx width, Rx/Tx Rate --*/
    /*For 1G and 2.5G change, the width is always 0x1, only for 10G port need modify width to 0x3.*/
    width = 1;
    if(serdes_mode == CTC_SERDES_MODE_1G25)
    {
        hss_div_sel = 2;        
        rate = 1;
    }
    else
    {
        hss_div_sel = 3;        
        rate = 0;
    }
    /*4.1 --- modify DivSel ---*/
    field_id = HSS_N0_CTL_CFG_N0_HSS_DIV_SEL;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &hss_div_sel);

    for(lport=0; lport<CTC_MAC_NUM_PER_MACRO; lport++)
    {
        /*4.2 --- modify rx width --- */
        if((lport%CTC_MAC_NUM_PER_MACRO) < 4)
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_WIDTH+((lport%CTC_MAC_NUM_PER_MACRO) * 16);
        }
        else
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_WIDTH+3*16+((lport%CTC_MAC_NUM_PER_MACRO)-3)*15;
        }
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &width);
        /*4.3 --- modify rx rate --- */
        if((lport%CTC_MAC_NUM_PER_MACRO) < 4)
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_RATE+((lport%CTC_MAC_NUM_PER_MACRO) * 16);
        }
        else
        {
            field_id = HSS_N0_CTL_CFG_N0_RXA_RATE+3*16+((lport%CTC_MAC_NUM_PER_MACRO)-3)*15;
        }
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &rate);
        /*4.4 --- modify tx width --- */
        field_id = HSS_N0_CTL_CFG_N0_TXA_WIDTH+(lport * 13);
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &width);
        /*4.5 --- modify tx rate --- */
        field_id = HSS_N0_CTL_CFG_N0_TXA_RATE+(lport * 13);
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &rate);
    }

    /*5. ------ release Hss4G reset ------*/
    field_val = 0;
    field_id = HSS_N0_CTL_CFG_N0_HSS_RESET;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    /*6. -- wait HSS PLL lock and HssReady asserted --*/
    kal_task_sleep(10);
    table_id = HSS_N0_MON + lserdes_id;
    field_id = HSS_N0_MON_MON_N0_HSS_READY;
    cmd = DRV_IOR(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &hss_ready);
    if(hss_ready != 1)
    {
        return CTC_E_CHECK_HSS_READY_FAIL;
    }
    else
    {
        return ret;
    }    
}

static int32
_sys_humber_port_hss4g_macro_disable(uint8 lchip, uint8 lserdes_id)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;    
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 1;

    /*1. ------ cfg Hss4G PLL power down  -------*/
    table_id = HSS_N0_CTL + lserdes_id;
    field_id = HSS_N0_CTL_CFG_N0_HSS_PDWN_PLL;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    return ret;
}

static int32
_sys_humber_port_hss4g_link_power_up(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;    
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 0;

    /*Old humber version not support powerdown serdes*/
    if(drv_humber_old_version())
    {
        return 0;
    }
    /*---- Power up Hss4G -----*/
    table_id = HSS_N0_CTL + (lport/CTC_MAC_NUM_PER_MACRO);
    if((lport%CTC_MAC_NUM_PER_MACRO) < 4)
    {
        field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+((lport%CTC_MAC_NUM_PER_MACRO) * 16);
    }
    else
    {
        field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+4*16+((lport%CTC_MAC_NUM_PER_MACRO)-4)*15;
    }
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    table_id = HSS_N0_CTL + (lport/CTC_MAC_NUM_PER_MACRO);
    field_id = HSS_N0_CTL_CFG_N0_TXA_PWR_DWN+((lport%CTC_MAC_NUM_PER_MACRO) * 13);
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    return ret;
}

static int32
_sys_humber_port_hss4g_link_power_down(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;    
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 1;

    /*Old humber version not support powerdown serdes*/
    if(drv_humber_old_version())
    {
        return 0;
    }
    table_id = HSS_N0_CTL + (lport/CTC_MAC_NUM_PER_MACRO);
    if((lport%CTC_MAC_NUM_PER_MACRO) < 4)
    {
        field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+((lport%CTC_MAC_NUM_PER_MACRO) * 16);
    }
    else
    {
        field_id = HSS_N0_CTL_CFG_N0_RXA_PWR_DWN+4*16+((lport%CTC_MAC_NUM_PER_MACRO)-4)*15;
    }
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*If cpumac use internal ref clk and serdes is also for cpumac clk, shouldn't be power down.*/
    if(!drv_humber_cpumac_use_internal_refclk() || (lport!=DRV_CPUMAC_CLK_SERDES))
    {
        table_id = HSS_N0_CTL + (lport/CTC_MAC_NUM_PER_MACRO);
        field_id = HSS_N0_CTL_CFG_N0_TXA_PWR_DWN+((lport%CTC_MAC_NUM_PER_MACRO) * 13);
        cmd = DRV_IOW(IOC_REG, table_id, field_id);
        ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    }
    
    return ret;    
}

/* Release reset sequence depending on <HumberHss configuration guide.doc> Rev1.1*/
static int32
_sys_humber_port_release_gmac_reset(uint8 lchip, uint8 lport, ctc_port_speed_t speed_mode)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;    
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 0;    
    uint32 clk_divider;
    uint32 mode;
    gmac_pcs_config1_t gmac_pcs;

    /*!!!!!!!!!!!!!!!keep order!!!!!!!!!!!!!!!!!!!!!!*/
    switch(speed_mode)
    {
        case CTC_PORT_SPEED_10M:
            clk_divider = 2;
            gmac_pcs.sgmii100m_mode_cfg = 0;
            gmac_pcs.sgmii1g_mode_cfg = 0;
            mode = 0;
            
            break;

        case CTC_PORT_SPEED_100M:
            clk_divider = 1;
            gmac_pcs.sgmii100m_mode_cfg = 1;
            gmac_pcs.sgmii1g_mode_cfg = 0;
            mode = 1;
            break;

        case CTC_PORT_SPEED_1G:
            clk_divider = 0;
            gmac_pcs.sgmii100m_mode_cfg = 0;
            gmac_pcs.sgmii1g_mode_cfg = 1;
            mode = 2;
            break;
        case CTC_PORT_SPEED_2G5:
            clk_divider = 0;
            gmac_pcs.sgmii100m_mode_cfg = 0;
            gmac_pcs.sgmii1g_mode_cfg = 1;
            mode = 3;
            break;
        default:
            return CTC_E_INVALID_PORT_SPEED_MODE;
    }    
    /* 1. ----- cfg GMII refClk divide base on speed  -----*/
    table_id = GMAC0_GMAC_CLK_DIVIDER + (lport * 20);
    field_id = GMAC0_GMAC_CLK_DIVIDER_CLK_DIVIDER;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &clk_divider);
    
    /* 2. ----- Release GMII refClk divider reset  --------*/
    table_id = GMAC0_GMAC_CLK_DIVIDER + (lport * 20);
    field_id = GMAC0_GMAC_CLK_DIVIDER_RST_CLK_DIVIDER;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    /* 3. -----  enable gmac clock  --------*/
    field_val = 1;
    field_id = lport + MODULE_GATED_CLK_CTL_EN_CLK_SUP_GMAC_WRAPPER0;
    cmd = DRV_IOW(IOC_REG, MODULE_GATED_CLK_CTL, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    field_val = 0;

    /* 4. -----    cfg gmac mode      --------*/
    table_id = GMAC0_GMACWRAPPER_GMAC_MAC_MODE + (lport * 20);
    field_id = GMAC0_GMACWRAPPER_GMAC_MAC_MODE_SPEED_MODE;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &mode);

    /* 5. -----    cfg gmac SGMII mode   --------*/
    table_id = GMAC0_GMAC_PCS_CONFIG1 + (lport * 20);
    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &gmac_pcs);
    
    /* 6. ------- release sup logic reset --------------*/
    field_id = lport + RESET_INT_RELATED_RESET_SUP_GMAC0;
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*7. ----------- release PCS Tx reset -----------*/
    table_id = GMAC0_GMAC_PCS_SOFT_RST + (lport * 20);
    field_id = GMAC0_GMAC_PCS_SOFT_RST_PCS_TX_SOFT_RST;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val); 

    /*8. ---------- release GMII Tx reset -----------*/
    table_id = GMAC0_GMAC_PCS_SOFT_RST + (lport * 20);
    field_id = GMAC0_GMAC_PCS_SOFT_RST_GMII_TX_SOFT_RST;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);        

    /*9. ------------ release PCS Rx reset -----------*/
    table_id = GMAC0_GMAC_PCS_SOFT_RST + (lport * 20);
    field_id = GMAC0_GMAC_PCS_SOFT_RST_PCS_RX_SOFT_RST;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val); 

    /*10. ------------release GMII Rx reset  -----------*/
    table_id = GMAC0_GMAC_PCS_SOFT_RST + (lport * 20);
    field_id = GMAC0_GMAC_PCS_SOFT_RST_GMII_RX_SOFT_RST;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);     

    return ret;
}

/* Reset sequence depending on <HumberHss configuration guide.doc> Rev1.1*/
static int32
_sys_humber_port_reset_gmac_reset(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;
    uint32 field_id;
    uint32 table_id;
    uint32 field_val = 1;
    gmac_pcs_soft_rst_t gmac_pcs_rst;

    /*!!!!!!!!!!!!!!!keep order!!!!!!!!!!!!!!!!!!!!!!*/
    gmac_pcs_rst.gmii_rx_soft_rst = 1;
    gmac_pcs_rst.gmii_tx_soft_rst = 1;
    gmac_pcs_rst.pcs_rx_soft_rst = 1;
    gmac_pcs_rst.pcs_tx_soft_rst = 1;
    /*1. --------- set GMAC logic reset ----------*/
    field_id = lport + RESET_INT_RELATED_RESET_SUP_GMAC0;
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    /*2. -----reset PCS Rx/Tx, GMII Rx/Tx reset -----*/
    table_id = GMAC0_GMAC_PCS_SOFT_RST + (lport * 20);
    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &gmac_pcs_rst);
    
    /*3. --------disable GMAC clock ------------*/
    field_val = 0;
    field_id = lport + MODULE_GATED_CLK_CTL_EN_CLK_SUP_GMAC_WRAPPER0;
    cmd = DRV_IOW(IOC_REG, MODULE_GATED_CLK_CTL, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);
    field_val = 1;

    /*4. set divider soft reset*/
    table_id = GMAC0_GMAC_CLK_DIVIDER + (lport * 20);
    field_id = GMAC0_GMAC_CLK_DIVIDER_RST_CLK_DIVIDER;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    return ret;
}

static int32
_sys_humber_port_release_xgmac_reset(uint8 lchip, uint8 lport)
{
    int32 cmd;
     int32 ret = CTC_E_NONE;
    uint32 field_val;
    uint32 field_id ;
    uint32 table_id;
    xgmac0_xgmac_config4_t xgmac_cfg4;
    xgmac0_xgmac_ptp_en_t xgmac_ptp;
    xgmac0_xgmac_soft_rst_t xgmac_soft_rst;

    /*!!!!!!!!!!!!!!!keep order!!!!!!!!!!!!!!!!!!!!!!*/

    /*reset sup logic reset*/
    field_id = RESET_INT_RELATED_RESET_SUP_XGMAC0 + (lport/12);
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    field_val = 1;
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    kal_task_sleep(1);
    /*release sup logic reset*/
    field_id = RESET_INT_RELATED_RESET_SUP_XGMAC0 + (lport/12);
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    field_val = 0;
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);


    /*release MDIO soft reset*/
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) * 36);
    field_id = XGMAC0_XGMAC_SOFT_RST_MDIO_SOFT_RST /*+ ((lport/12) * 123 )*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*init xgmac*/
    kal_memset(&xgmac_cfg4, 0, sizeof(xgmac0_xgmac_config4_t));
    table_id = XGMAC0_XGMAC_CONFIG4 + ((lport/12) * 36);

    xgmac_cfg4.ignore_remote_fault  = 1;
    xgmac_cfg4.pad_enable           = p_port_master->egs_port_prop[lchip][lport].pading_en;
    xgmac_cfg4.crc_enable           = 1;
    xgmac_cfg4.pause_off_enable     = 1;
    xgmac_cfg4.sig_det_active_value = 1;
    xgmac_cfg4.tx_threshold         = p_port_master->igs_port_prop[lchip][lport].ptp_en ? 0
                                        : p_port_master->egs_port_prop[lchip][lport].tx_threshold;
    xgmac_cfg4.full_threshold       = 0x13;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_cfg4);

    /*when xg/sg up/down, the xgmac/sgmac should not disable PTP while Phyport do nothing*/
    /*re-configue ptpEn*/
    kal_memset(&xgmac_ptp, 0, sizeof(xgmac0_xgmac_ptp_en_t));
    xgmac_ptp.ptp_en = p_port_master->igs_port_prop[lchip][lport].ptp_en ? 1 : 0;
    table_id = XGMAC0_XGMAC_PTP_EN + ((lport/12) * 36);
    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_ptp);

    /*clear drain enable*/
    table_id = XGMAC0_XGMAC_DRAIN_EN + ((lport/12) * 36);
    field_id = XGMAC0_XGMAC_DRAIN_EN_XGMAC_DRAIN_EN /*+ ((lport/12) *123)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);


    /*release PCS Tx soft reset*/
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_SOFT_RST_PCS_TX_SOFT_RST /*+ ((lport/12) *123)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*release PCS Rx soft reset*/
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_SOFT_RST_PCS_RX_SOFT_RST  /*+ ((lport/12) *123)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*release serdes[0:3] soft reset */
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) *36);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_soft_rst);

    xgmac_soft_rst.serdes_rx0_soft_rst = 0;
    xgmac_soft_rst.serdes_rx1_soft_rst = 0;
    xgmac_soft_rst.serdes_rx2_soft_rst = 0;
    xgmac_soft_rst.serdes_rx3_soft_rst = 0;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_soft_rst);

    /*set drain enable*/
    table_id = XGMAC0_XGMAC_DRAIN_EN + ((lport/12) * 36);
    field_id = XGMAC0_XGMAC_DRAIN_EN_XGMAC_DRAIN_EN /*+ ((lport/12) *123)*/;
    field_val = 1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*open Tx enable*/
    table_id = XGMAC0_XGMAC_CONFIG1 + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_CONFIG1_TX_ENABLE /*+ ((lport/12) *123)*/;
    field_val = 1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    /*recover flow control configuration*/
    // RX
    table_id = XGMAC0_XGMAC_CONFIG1 + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_CONFIG1_PAUSE_FRAME_ENABLE /*+ ((lport/12) *123)*/;
    field_val = p_port_master->igs_port_prop[lchip][lport].pause_rx_en ? 1 : 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    // TX
    table_id = XGMAC0_XGMAC_CONFIG5 + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_CONFIG5_BUF_STORE_STALL_MASK /*+ ((lport/12) *123)*/;
    field_val = p_port_master->egs_port_prop[lchip][lport].pause_tx_en ? 0 : 1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    return ret;
}

static int32
_sys_humber_port_reset_xgmac_reset(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;
    xgmac0_xgmac_soft_rst_t xgmac_soft_rst;

    /*!!!!!!!!!!!!!!!keep order!!!!!!!!!!!!!!!!!!!!!!*/

    /*clear Tx enable*/
    table_id = XGMAC0_XGMAC_CONFIG1 + ((lport/12) *36);
    field_id = XGMAC0_XGMAC_CONFIG1_TX_ENABLE /* + ((lport/12) *123)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*reset PCS Tx,Rx soft reset*/
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) *36);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_soft_rst);

    xgmac_soft_rst.pcs_rx_soft_rst = 1;
    xgmac_soft_rst.pcs_tx_soft_rst = 1;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip,0, cmd, &xgmac_soft_rst);

    /*reset serdes[0:3] soft reset*/
    table_id = XGMAC0_XGMAC_SOFT_RST + ((lport/12) *36);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &xgmac_soft_rst);

    xgmac_soft_rst.serdes_rx0_soft_rst = 1;
    xgmac_soft_rst.serdes_rx1_soft_rst = 1;
    xgmac_soft_rst.serdes_rx2_soft_rst = 1;
    xgmac_soft_rst.serdes_rx3_soft_rst = 1;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip,0, cmd, &xgmac_soft_rst);

    /*clear drain enable*/
    table_id = XGMAC0_XGMAC_DRAIN_EN + ((lport/12) * 36);
    field_id = XGMAC0_XGMAC_DRAIN_EN_XGMAC_DRAIN_EN /*+ ((lport/12) *123)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    return ret;
}


static int32
_sys_humber_port_release_sgmac_reset(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;
    uint32 field_id;
    uint32 table_id;
    uint32 field_val;
    sgmac0_sgmac_config4_t sgmac_cfg4;
    sgmac0_sgmac_ptp_en_t sgmac_ptp;
    sgmac0_sgmac_soft_rst_t sgmac_soft_rst;

    /*reset sup logic reset*/
    field_id = RESET_INT_RELATED_RESET_SUP_SG_MAC0 + (lport - 48);
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    field_val = 1;
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    kal_task_sleep(1);

    /*release sup logic reset*/
    field_id = RESET_INT_RELATED_RESET_SUP_SG_MAC0 + (lport - 48);
    cmd = DRV_IOW(IOC_REG, RESET_INT_RELATED, field_id);
    field_val = 0;
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*init sgmac*/
    kal_memset(&sgmac_cfg4, 0, sizeof(sgmac0_sgmac_config4_t));
    table_id = SGMAC0_SGMAC_CONFIG4 + ((lport - 48) * 42);

    sgmac_cfg4.ignore_remote_fault  = 1;
    sgmac_cfg4.pad_enable           = p_port_master->egs_port_prop[lchip][lport].pading_en;
    sgmac_cfg4.crc_enable           = 1;
    sgmac_cfg4.pause_off_enable     = 1;
    sgmac_cfg4.sig_det_active_value = 1;
    sgmac_cfg4.e2e_crc_bit_swizzle  = 1;
    sgmac_cfg4.e2e_msg_error_en     = 1;
    sgmac_cfg4.tx_threshold         = p_port_master->igs_port_prop[lchip][lport].ptp_en ? 0
                                        : p_port_master->egs_port_prop[lchip][lport].tx_threshold;
    sgmac_cfg4.full_threshold       = 0x13;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_cfg4);

    /*when xg/sg up/down, the xgmac/sgmac should not disable PTP while Phyport do nothing*/
    /*re-configue ptpEn*/
    kal_memset(&sgmac_ptp, 0, sizeof(sgmac0_sgmac_ptp_en_t));
    sgmac_ptp.ptp_en = p_port_master->igs_port_prop[lchip][lport].ptp_en ? 1 : 0;
    table_id = SGMAC0_SGMAC_PTP_EN + ((lport - 48) * 42);
    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_ptp);

    /*clear drain enable*/
    table_id = SGMAC0_SGMAC_DRAIN_EN + ((lport - 48) * 42);
    field_id = SGMAC0_SGMAC_DRAIN_EN_SGMAC_DRAIN_EN /*+ ((lport - 48) *147)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);


    /*release PCS Tx soft reset*/
    table_id = SGMAC0_SGMAC_SOFT_RST + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_SOFT_RST_PCS_TX_SOFT_RST /*+ ((lport - 48) *147)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*release PCS Rx soft reset*/
    table_id = SGMAC0_SGMAC_SOFT_RST + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_SOFT_RST_PCS_RX_SOFT_RST /*+ ((lport - 48) *147)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*release serdes[0:3] soft reset */
    table_id = SGMAC0_SGMAC_SOFT_RST + ((lport - 48) *42);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_soft_rst);

    sgmac_soft_rst.serdes_rx0_soft_rst = 0;
    sgmac_soft_rst.serdes_rx1_soft_rst = 0;
    sgmac_soft_rst.serdes_rx2_soft_rst = 0;
    sgmac_soft_rst.serdes_rx3_soft_rst = 0;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_soft_rst);

    /*set drain enable*/
    table_id = SGMAC0_SGMAC_DRAIN_EN + ((lport - 48) * 42);
    field_id = SGMAC0_SGMAC_DRAIN_EN_SGMAC_DRAIN_EN /*+ ((lport - 48) *147)*/;
    field_val = 1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*open Tx enable*/
    table_id = SGMAC0_SGMAC_CONFIG1 + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_CONFIG1_TX_ENABLE /*+ ((lport - 48)*147)*/;
    field_val =1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*recover flow control configuration*/
    // RX
    table_id = SGMAC0_SGMAC_CONFIG1 + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_CONFIG1_PAUSE_FRAME_ENABLE /*+ ((lport - 48)*147)*/;
    field_val = p_port_master->igs_port_prop[lchip][lport].pause_rx_en ? 1 : 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    // TX
    table_id = SGMAC0_SGMAC_CONFIG5 + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_CONFIG5_BUF_STORE_STALL_MASK /*+ ((lport - 48)*147)*/;
    field_val = p_port_master->egs_port_prop[lchip][lport].pause_tx_en ? 0 : 1;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
    
    return ret;
}

static int32
_sys_humber_port_reset_sgmac_reset(uint8 lchip, uint8 lport)
{
    int32 ret = CTC_E_NONE;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;
    sgmac0_sgmac_soft_rst_t sgmac_soft_rst;

    /*!!!!!!!!!!!!!!!keep order!!!!!!!!!!!!!!!!!!!!!!*/

    /*clear Tx enable*/
    table_id = SGMAC0_SGMAC_CONFIG1 + ((lport - 48) *42);
    field_id = SGMAC0_SGMAC_CONFIG1_TX_ENABLE /*+ ((lport - 48)*147)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

    /*reset PCS Tx,Rx soft reset*/
    table_id = SGMAC0_SGMAC_SOFT_RST + ((lport - 48) *42);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_soft_rst);

    sgmac_soft_rst.pcs_rx_soft_rst = 1;
    sgmac_soft_rst.pcs_tx_soft_rst = 1;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip,0, cmd, &sgmac_soft_rst);

    /*reset serdes[0:3] soft reset*/
    table_id = SGMAC0_SGMAC_SOFT_RST + ((lport - 48) *42);
    cmd = DRV_IOR(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &sgmac_soft_rst);

    sgmac_soft_rst.serdes_rx0_soft_rst = 1;
    sgmac_soft_rst.serdes_rx1_soft_rst = 1;
    sgmac_soft_rst.serdes_rx2_soft_rst = 1;
    sgmac_soft_rst.serdes_rx3_soft_rst = 1;

    cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
    ret = ret? ret:drv_reg_ioctl(lchip,0, cmd, &sgmac_soft_rst);

    /*clear drain enable*/
    table_id = SGMAC0_SGMAC_DRAIN_EN + ((lport - 48) * 42);
    field_id = SGMAC0_SGMAC_DRAIN_EN_SGMAC_DRAIN_EN /*+ ((lport - 48) *147)*/;
    field_val = 0;
    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &field_val);

    return ret;
}

static int32
_sys_humber_port_set_gmac_en(uint8 lchip, uint8 lport, bool enable)
{
    int32 ret = CTC_E_NONE;
    
    if (TRUE == enable)
    {
        ret = _sys_humber_port_hss4g_link_power_up(lchip, lport);
        ret = ret? ret:_sys_humber_port_release_gmac_reset(lchip, lport, p_port_master->igs_port_prop[lchip][lport].speed_mode);
    }
    else
    {
        ret = _sys_humber_port_reset_gmac_reset(lchip, lport);     
        ret = ret? ret:_sys_humber_port_hss4g_link_power_down(lchip, lport);
    }

    return ret;
}

static int32
_sys_humber_port_set_xgmac_en(uint8 lchip, uint8 lport, bool enable)
{
    int32 ret = CTC_E_NONE;

    if (enable)
    {
        ret = _sys_humber_port_release_xgmac_reset(lchip, lport);
    }
    else
    {
        ret = _sys_humber_port_reset_xgmac_reset(lchip, lport);
    }

    return ret;
}

static int32
_sys_humber_port_set_sgmac_en(uint8 lchip, uint8 lport, bool enable)
{
    int32 ret;

    if (enable)
    {
        ret = _sys_humber_port_release_sgmac_reset(lchip, lport);
    }
    else
    {
        ret = _sys_humber_port_reset_sgmac_reset(lchip, lport);

    }

    return ret;
}

/* software\docs\humber_sdk\manual下有个Concepts & Resources.doc文档 */
int32
sys_humber_serdes_set_mode(uint16 gserdes_id, ctc_serdes_mode_t serdes_mode)
{
    uint8 lchip;
    uint8 lport, lserdes_id, lport_idx;
    int32 ret = CTC_E_NONE;
    ctc_port_mac_type_t mac_type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_MAP_GSERDESID_TO_LSERDESID(gserdes_id, lchip, lserdes_id);
    lport = lserdes_id*CTC_MAC_NUM_PER_MACRO;
    
    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }    
    
    PORT_LOCK;    
    mac_type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;
    
    if(CTC_PORT_MAC_GMAC == mac_type)
    {
        /*If cpumac use internal refclk, not allow to change Macro 5 serdes mode to 3.125G*/
        if(drv_humber_cpumac_use_internal_refclk() && (lport==DRV_CPUMAC_CLK_SERDES))
        {
            if(serdes_mode == CTC_SERDES_MODE_1G25)
                ret = CTC_E_NONE;
            else
                ret = CTC_E_CANT_CHANGE_SERDES_MODE;
        }
        else
        {
            /*1. set all gmac in one macro disable*/
            for(lport_idx=lport; lport_idx<lport+CTC_MAC_NUM_PER_MACRO; lport_idx++)
            {
                ret = ret? ret:_sys_humber_port_reset_gmac_reset(lchip, lport_idx);
            }
            /*2. set hss4g disable*/
            ret = ret? ret:_sys_humber_port_hss4g_macro_disable(lchip, lserdes_id);
            /*3. set hss4g enable*/
            ret = ret? ret:_sys_humber_port_hss4g_macro_enable(lchip, lserdes_id, serdes_mode);
            /*4. set gmac enable*/
            for(lport_idx=lport; lport_idx<lport+CTC_MAC_NUM_PER_MACRO; lport_idx++)
            {
                p_port_master->igs_port_prop[lchip][lport_idx].serdes_mode = serdes_mode;
                /*When cfg serdes mode to 3.125G, speed mode should cfg to 2.5GE. */
                if(CTC_SERDES_MODE_3G125 == serdes_mode)
                {
                    p_port_master->igs_port_prop[lchip][lport_idx].speed_mode = CTC_PORT_SPEED_2G5;
                }
                else
                {
                    p_port_master->igs_port_prop[lchip][lport_idx].speed_mode = CTC_PORT_SPEED_1G;
                }
                ret = ret? ret:_sys_humber_port_set_gmac_en(lchip, lport_idx, p_port_master->igs_port_prop[lchip][lport_idx].port_mac_en);            
            }
        }
    }
    else
    {
        ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }
    
    PORT_UNLOCK;
    
    return ret;
}

int32
sys_humber_serdes_get_mode(uint16 gserdes_id, ctc_serdes_mode_t *serdes_mode)
{
    uint8 lchip;
    uint8 lport, lserdes_id;
    int32 ret = CTC_E_NONE;
    ctc_port_mac_type_t mac_type;
    
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_MAP_GSERDESID_TO_LSERDESID(gserdes_id, lchip, lserdes_id);
    lport = lserdes_id*CTC_MAC_NUM_PER_MACRO;

    PORT_LOCK;
    mac_type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;
    
    if(CTC_PORT_MAC_GMAC == mac_type)
    {
        *serdes_mode = p_port_master->igs_port_prop[lchip][lport].serdes_mode;
    }
    else
    {
        ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }    
    PORT_UNLOCK;
    return ret;
}


int32
sys_humber_port_set_mac_en(uint16 gport, bool enable)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    ctc_port_mac_type_t type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC:
            ret = _sys_humber_port_set_gmac_en(lchip, lport, enable);
            break;

        case CTC_PORT_MAC_XGMAC:
            ret = _sys_humber_port_set_xgmac_en(lchip, lport, enable);
            break;

        case CTC_PORT_MAC_SGMAC:
            ret = _sys_humber_port_set_sgmac_en(lchip, lport, enable);
            break;

        case CTC_PORT_MAC_CPUMAC:
            ret = sys_humber_set_cpu_mac_en(enable);
            break;

        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }

    if (CTC_E_NONE == ret)
    {
        p_port_master->igs_port_prop[lchip][lport].port_mac_en  = enable;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_mac_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    *enable = p_port_master->igs_port_prop[lchip][lport].port_mac_en;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_speed(uint16 gport, ctc_port_speed_t speed_mode)
{
    uint8 lchip;
    uint8 lport;
    int32 ret;
    ctc_port_mac_type_t mac_type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    mac_type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(mac_type)
    {
        case CTC_PORT_MAC_GMAC:
            /*Only when port enable, cfg real speed; else just store speed mode, and will cfg when enable*/
            if(p_port_master->igs_port_prop[lchip][lport].port_mac_en)
            {
                ret = _sys_humber_port_reset_gmac_reset(lchip, lport);
                ret = ret? ret:_sys_humber_port_release_gmac_reset(lchip, lport, speed_mode);
            }
            else
            {
                ret = CTC_E_NONE;
            }
            break;
            
       default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }

    if (CTC_E_NONE == ret)
    {
        p_port_master->igs_port_prop[lchip][lport].speed_mode = speed_mode;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_speed(uint16 gport, ctc_port_speed_t *speed_mode)
{
    uint8 lchip;
    uint8 lport;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(speed_mode);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    *speed_mode = p_port_master->igs_port_prop[lchip][lport].speed_mode;
    PORT_UNLOCK;

    return CTC_E_NONE;
}


int32
sys_humber_set_max_frame_size(ctc_frame_size_t index, uint16 value)
{
    uint8 chip_id;
    uint8 lchip_num;
    uint32 cmd;
    uint32 field_val;

    if (value >= SYS_MAX_FRAME_VALUE)
    {
        return CTC_E_INVALID_PARAM;
    }

    switch(index)
    {
        case CTC_FRAME_SIZE_0:
            cmd = DRV_IOW(IOC_REG, NET_RX_MAX_PKT_SIZE, NET_RX_MAX_PKT_SIZE_MAX_PKT_SIZE0);
            break;

        case CTC_FRAME_SIZE_1:
            cmd = DRV_IOW(IOC_REG, NET_RX_MAX_PKT_SIZE, NET_RX_MAX_PKT_SIZE_MAX_PKT_SIZE1);
            break;

        default:
            return CTC_E_UNEXPECT;
    }

    lchip_num = sys_humber_get_local_chip_num();
    field_val = value;

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &field_val));
    }

    return CTC_E_NONE;
}

int32
sys_humber_get_max_frame_size(ctc_frame_size_t index,uint16 *max_size)
{
    uint32 cmd;
    uint32 field_val;

    switch(index)
    {
        case CTC_FRAME_SIZE_0:
            cmd = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE, NET_RX_MAX_PKT_SIZE_MAX_PKT_SIZE0);
            break;

        case CTC_FRAME_SIZE_1:
            cmd = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE, NET_RX_MAX_PKT_SIZE_MAX_PKT_SIZE1);
            break;

        default:
            return CTC_E_UNEXPECT;
    }

    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &field_val));

    *max_size = field_val & 0x3FFF;

    return CTC_E_NONE;
}

int32
sys_humber_set_cpu_mac_en(bool enable)
{
    uint8 chip_id;
    uint8 lchip_num;
    uint32 cmd, cmd2;
    uint32 field_val = (TRUE == enable)? 1:0;

    cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_TX_CTRL, CPUMAC_GMAC_TX_CTRL_TX_ENABLE);
    cmd2 = DRV_IOW(IOC_REG, CPUMAC_GMAC_RX_CTRL, CPUMAC_GMAC_RX_CTRL_RX_ENABLE);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &field_val));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd2, &field_val));
    }

    return CTC_E_NONE;
}

int32
sys_humber_get_cpu_mac_en(bool *enable)
{
    uint32 cmd;
    uint32 field_val;

    cmd = DRV_IOR(IOC_REG, CPUMAC_GMAC_TX_CTRL, CPUMAC_GMAC_TX_CTRL_TX_ENABLE);

    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &field_val));

    *enable = (1 == field_val)? TRUE:FALSE;

    return CTC_E_NONE;
}


int32
sys_humber_port_set_max_frame(uint16 gport, ctc_frame_size_t index)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd_r;
    uint32 cmd_w;
    uint32 field_value;

    if(index >= CTC_FRAME_SIZE_MAX)
    {
        return CTC_E_INVALID_PARAM;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    if (lport < BITS_NUM_OF_WORD)
    {
        cmd_r = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_LO);
        cmd_w = DRV_IOW(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_LO);
    }
    else
    {
        cmd_r = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_HI);
        cmd_w = DRV_IOW(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_HI);
    }

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd_r, &field_value));

    if (CTC_FRAME_SIZE_1 == index)
    {
        CTC_SET_FLAG(field_value, 1 << (lport%BITS_NUM_OF_WORD));
    }
    else
    {
        CTC_UNSET_FLAG(field_value, 1 << (lport%BITS_NUM_OF_WORD));
    }

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd_w, &field_value));

    return CTC_E_NONE;
}

int32
sys_humber_port_get_max_frame(uint16 gport, ctc_frame_size_t *index)
{
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 field_val;
    CTC_PTR_VALID_CHECK(index);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    if (lport < BITS_NUM_OF_WORD)
    {
        cmd = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_LO);
    }
    else
    {
        cmd = DRV_IOR(IOC_REG, NET_RX_MAX_PKT_SIZE_SELECT, NET_RX_MAX_PKT_SIZE_SELECT_MAX_PKT_SIZE_SELECT_HI);
    }

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field_val));

    if (CTC_FLAG_ISSET(field_val, 1 << (lport%BITS_NUM_OF_WORD)))
    {
        *index = CTC_FRAME_SIZE_1;
    }
    else
    {
        *index = CTC_FRAME_SIZE_0;
    }

    return CTC_E_NONE;
}

int32
sys_humber_port_set_flow_ctl_en(uint16 gport, ctc_direction_t dir,uint32 enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 reg_id = 0;
    uint32 field_id = 0;
    ctc_port_mac_type_t type;
    uint32 field_val = (TRUE == enable)? 1:0;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC: /* GMAC */
            if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*rx pause frame*/
                reg_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL + (lport *GMAC_FLOW_CTL_REG_ID_INTERVAL);
                field_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL_RX_FLOW_CTRL_ENABLE;
                cmd = DRV_IOW(IOC_REG, reg_id, field_id);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }

            if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*tx pause frame*/
                reg_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL + (lport *GMAC_FLOW_CTL_REG_ID_INTERVAL);
                field_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_FLOW_CTRL_ENABLE;
                cmd = DRV_IOW(IOC_REG, reg_id, field_id);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }
            break;

        case CTC_PORT_MAC_XGMAC:    /* XGMAC */
            reg_id = XGMAC0_XGMAC_CONFIG1 + (lport / 12) *XGMAC_FLOW_CTL_REG_ID_INTERVAL;
            field_id = XGMAC0_XGMAC_CONFIG1_PAUSE_FRAME_ENABLE;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            break;

        case CTC_PORT_MAC_SGMAC:    /* SGMAC */
            reg_id = SGMAC0_SGMAC_CONFIG1 + (lport - 48) *SGMAC_FLOW_CTL_REG_ID_INTERVAL;
            field_id = SGMAC0_SGMAC_CONFIG1_PAUSE_FRAME_ENABLE;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            break;

        case CTC_PORT_MAC_CPUMAC:    /* CPUMAC */
            if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*rx pause frame*/
                cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_RX_CTRL, CPUMAC_GMAC_RX_CTRL_RX_FLOW_CTRL_ENABLE);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }

            if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*tx pause frame*/
                cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_TX_CTRL, CPUMAC_GMAC_TX_CTRL_TX_FLOW_CTRL_ENABLE);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }

        default:
            return CTC_E_INVALID_PARAM;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_flow_ctl_en(uint16 gport, ctc_direction_t dir, uint32 *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 table_id = 0;
    uint32 field_id = 0;
    uint32 field_val = 0;
    ctc_port_mac_type_t type;

    CTC_PTR_VALID_CHECK(enable);
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC: /* GMAC */
            if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*rx pause frame*/
                table_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL + (lport *GMAC_FLOW_CTL_REG_ID_INTERVAL);
                field_id = GMAC0_GMACWRAPPER_GMAC_RX_CTRL_RX_FLOW_CTRL_ENABLE;
                cmd = DRV_IOR(IOC_REG, table_id, field_id);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }

            if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*tx pause frame*/
                table_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL + (lport *GMAC_FLOW_CTL_REG_ID_INTERVAL);
                field_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_FLOW_CTRL_ENABLE;
                cmd = DRV_IOR(IOC_REG, table_id, field_id);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }
            break;

        case CTC_PORT_MAC_XGMAC:    /* XGMAC */
            table_id = XGMAC0_XGMAC_CONFIG1 + (lport / 12) *XGMAC_FLOW_CTL_REG_ID_INTERVAL;
            field_id = XGMAC0_XGMAC_CONFIG1_PAUSE_FRAME_ENABLE;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            break;

        case CTC_PORT_MAC_SGMAC:    /* SGMAC */
            table_id = SGMAC0_SGMAC_CONFIG1 + (lport - 48) *SGMAC_FLOW_CTL_REG_ID_INTERVAL;
            field_id = SGMAC0_SGMAC_CONFIG1_PAUSE_FRAME_ENABLE;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            break;

        case CTC_PORT_MAC_CPUMAC:    /* CPUMAC */
            if ((CTC_INGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*rx pause frame*/
                cmd = DRV_IOR(IOC_REG, CPUMAC_GMAC_RX_CTRL, CPUMAC_GMAC_RX_CTRL_RX_FLOW_CTRL_ENABLE);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }

            if ((CTC_EGRESS == dir) || (CTC_BOTH_DIRECTION == dir))
            {
                /*tx pause frame*/
                cmd = DRV_IOR(IOC_REG, CPUMAC_GMAC_TX_CTRL, CPUMAC_GMAC_TX_CTRL_TX_FLOW_CTRL_ENABLE);
                ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            }
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    PORT_UNLOCK;

    *enable = (field_val == 1)?TRUE:FALSE;

    return ret;
}

static int32
_sys_humber_port_set_gmac_preamble(uint8 lchip, uint8 lport, uint8 pre_bytes)
{
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val = pre_bytes;

    CTC_VALUE_RANGE_CHECK(pre_bytes, SYS_MIN_PREAMBLE_FOR_GMAC, SYS_MAX_PREAMBLE_FOR_GMAC);

    table_id = GMAC0_GMACWRAPPER_GMAC_PRE_LENGTH + (lport * 20);
    field_id = GMAC0_GMACWRAPPER_GMAC_PRE_LENGTH_PRE_LENGTH /*+ (lport * 55)*/;

    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field_val));

    return CTC_E_NONE;
}

static int32
_sys_humber_port_set_xgmac_preamble(uint8 lchip, uint8 lport, uint8 pre_bytes)
{
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;
    /*only 4Bytes and 8Bytes can be configed to xgmac*/
    if (SYS_MIN_PREAMBLE_FOR_SGMAC_XGMAC == pre_bytes)
    {
        field_val = 1;
    }
    else if (SYS_MAX_PREAMBLE_FOR_SGMAC_XGMAC == pre_bytes)
    {
        field_val = 0;
    }
    else
    {
        return CTC_E_INVALID_PREAMBLE;
    }

    table_id = XGMAC0_XGMAC_CONFIG1 + (lport /16);
    field_id = XGMAC0_XGMAC_CONFIG1_PREAMBLE4_BYTES /*+ ((lport /16) *123)*/;

    cmd = DRV_IOW(IOC_REG, table_id, field_id);

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field_val));

    return CTC_E_NONE;
}

static int32
_sys_humber_port_set_sgmac_preamble(uint8 lchip, uint8 lport, uint8 pre_bytes)
{
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;

    /*only 4Bytes and 8Bytes can be configed to sgmac*/
    if (SYS_MIN_PREAMBLE_FOR_SGMAC_XGMAC == pre_bytes)
    {
        field_val = 1;
    }
    else if (SYS_MAX_PREAMBLE_FOR_SGMAC_XGMAC == pre_bytes)
    {
        field_val = 0;
    }
    else
    {
        return CTC_E_INVALID_PREAMBLE;
    }

    table_id = SGMAC0_SGMAC_CONFIG1 + ((lport - 48)*42);
    field_id = SGMAC0_SGMAC_CONFIG1_PREAMBLE4_BYTES /*+ ((lport - 48) *147)*/;

    cmd = DRV_IOW(IOC_REG, table_id, field_id);

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field_val));

    return CTC_E_NONE;
}


int32
sys_humber_port_set_preamble(uint16 gport, uint8 pre_bytes)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    ctc_port_mac_type_t type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC:
            ret = _sys_humber_port_set_gmac_preamble(lchip, lport, pre_bytes);
            break;

        case CTC_PORT_MAC_XGMAC:
            ret = _sys_humber_port_set_xgmac_preamble(lchip, lport, pre_bytes);
            break;

        case CTC_PORT_MAC_SGMAC:
            ret = _sys_humber_port_set_sgmac_preamble(lchip, lport, pre_bytes);
            break;

        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_preamble(uint16 gport, uint8 *pre_bytes)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;
    ctc_port_mac_type_t type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;

    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC:
            table_id = GMAC0_GMACWRAPPER_GMAC_PRE_LENGTH + (lport * 20);
            field_id = GMAC0_GMACWRAPPER_GMAC_PRE_LENGTH_PRE_LENGTH /*+ (lport * 55)*/;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            *pre_bytes = field_val;
            break;

        case CTC_PORT_MAC_XGMAC:
            table_id = XGMAC0_XGMAC_CONFIG1 + ((lport /12)*36);
            field_id = XGMAC0_XGMAC_CONFIG1_PREAMBLE4_BYTES /*+ ((lport /12) *123)*/;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            *pre_bytes = (1 == field_val)? 4:8;
            break;

        case CTC_PORT_MAC_SGMAC:
            table_id = SGMAC0_SGMAC_CONFIG1 + ((lport - 48)*42);
            field_id = SGMAC0_SGMAC_CONFIG1_PREAMBLE4_BYTES /* + ((lport - 48) *147)*/;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            *pre_bytes = (1 == field_val)? 4:8;
            break;

        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }
    PORT_UNLOCK;


    return ret;
}

static int32
_sys_humber_port_set_gmac_min_frame_size(uint8 lchip, uint8 lport, uint8 size)
{
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val = size;

    CTC_VALUE_RANGE_CHECK(size, SYS_MIN_LENGTH_FOR_GMAC, SYS_MAX_LENGTH_FOR_GMAC);

    table_id = GMAC0_GMACWRAPPER_GMAC_PKT_LENGTH + (lport * 20);
    field_id = GMAC0_GMACWRAPPER_GMAC_PKT_LENGTH_MIN_PKT_LEN;

    cmd = DRV_IOW(IOC_REG, table_id, field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field_val));

    return CTC_E_NONE;
}

int32
sys_humber_port_set_min_frame_size(uint16 gport, uint8 size)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    ctc_port_mac_type_t type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC:
            ret = _sys_humber_port_set_gmac_min_frame_size(lchip, lport, size);
            break;

        /*not support config sgmac and xgmac min pktlen, they are fixed*/
        case CTC_PORT_MAC_XGMAC:
        case CTC_PORT_MAC_SGMAC:
            ret = CTC_E_INVALID_GLOBAL_PORT;
            break;

        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_min_frame_size(uint16 gport, uint8* size)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint32 field_val;
    ctc_port_mac_type_t type;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;

    type = p_port_master->igs_port_prop[lchip][lport].port_mac_type;

    switch(type)
    {
        case CTC_PORT_MAC_GMAC:
            table_id = GMAC0_GMACWRAPPER_GMAC_PKT_LENGTH + (lport * 20);
            field_id = GMAC0_GMACWRAPPER_GMAC_PKT_LENGTH_MIN_PKT_LEN;
            cmd = DRV_IOR(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);
            *size = field_val;
            break;

        case CTC_PORT_MAC_XGMAC:
        case CTC_PORT_MAC_SGMAC:
            ret = CTC_E_INVALID_GLOBAL_PORT;
            break;

        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }
    PORT_UNLOCK;


    return ret;
}

int32
sys_humber_port_set_stretch_mode_en(uint16 gport, bool enable)
{
    int32 ret;
    uint8 lchip;
    uint8 lport;
    uint32 table_id;
    uint32 field_id;
    uint32 cmd;
    uint32 field_val;
    sgmac0_sgmac_stretch_mode_t mode;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if (TRUE == enable)
    {
        field_val = 0;
        mode.ifs_stretch_count_init = 0;
        mode.ifs_stretch_size_init  = 14;
        mode.ifs_stretch_ratio      = 13;
        mode.ifs_stretch_mode       = 1;
    }
    else
    {
        field_val = 1;
        kal_memset(&mode, 0, sizeof(sgmac0_sgmac_stretch_mode_t));
    }


    PORT_LOCK;
    switch(p_port_master->igs_port_prop[lchip][lport].port_mac_type)
    {
        case CTC_PORT_MAC_XGMAC:
            table_id = XGMAC0_XGMAC_CONFIG1 + (lport /12) *36;
            field_id = XGMAC0_XGMAC_CONFIG1_DIC_CNT_ENABLE /*+ (lport /12)*123*/;
            cmd = DRV_IOW(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

            table_id = XGMAC0_XGMAC_STRETCH_MODE + (lport /12) *36;
            cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
            ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &mode);
            break;
        case CTC_PORT_MAC_SGMAC:
            table_id = SGMAC0_SGMAC_CONFIG1 + (lport - 48) *42;
            field_id = SGMAC0_SGMAC_CONFIG1_DIC_CNT_ENABLE /* + (lport - 48)*147*/;
            cmd = DRV_IOW(IOC_REG, table_id, field_id);
            ret = drv_reg_ioctl(lchip, 0, cmd, &field_val);

            table_id = SGMAC0_SGMAC_STRETCH_MODE + (lport - 48) *42;
            cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
            ret = ret? ret:drv_reg_ioctl(lchip, 0, cmd, &mode);
            break;
        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
    }

    if (CTC_E_NONE == ret)
    {
        p_port_master->igs_port_prop[lchip][lport].stretch_en = enable?1:0;
    }

    PORT_UNLOCK;

    return ret;
}


int32
sys_humber_port_get_stretch_mode_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    PORT_LOCK;
    *enable = p_port_master->igs_port_prop[lchip][lport].stretch_en?TRUE:FALSE;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_pading_en(uint16 gport, bool enable)
{
    uint8 lchip;
    uint8 lport;
    uint8 reg_step;
    uint16 reg_id = 0;
    uint16 field_id = 0;    
    uint32 cmd;
    uint32 field_value;
    int32 ret;
    

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
        
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }
    

    PORT_LOCK;
    switch(p_port_master->igs_port_prop[lchip][lport].port_mac_type)
    {
        case CTC_PORT_MAC_XGMAC:
            reg_step = XGMAC1_XGMAC_CONFIG4 - XGMAC0_XGMAC_CONFIG4;
            reg_id = XGMAC0_XGMAC_CONFIG4 + (lport/12)*reg_step;
            field_id = XGMAC0_XGMAC_CONFIG4_PAD_ENABLE;
            break;
        case CTC_PORT_MAC_SGMAC:
            reg_step = SGMAC1_SGMAC_CONFIG4 - SGMAC0_SGMAC_CONFIG4;
            reg_id = SGMAC0_SGMAC_CONFIG4 + (lport-SYS_MAX_GMAC_NUM)*reg_step;
            field_id = SGMAC0_SGMAC_CONFIG4_PAD_ENABLE;
            break;
        case CTC_PORT_MAC_GMAC:
            reg_step = GMAC1_GMACWRAPPER_GMAC_TX_CTRL - GMAC0_GMACWRAPPER_GMAC_TX_CTRL;
            reg_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL + lport*reg_step;
            field_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL_PAD_ENABLE;
            break;
        default:
            ret = CTC_E_INVALID_PORT_MAC_TYPE;
            break;
    }

    field_value = enable?1:0;
    cmd = DRV_IOW(IOC_REG, reg_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &field_value);

    if (CTC_E_NONE == ret)
    {
        p_port_master->egs_port_prop[lchip][lport].pading_en= enable?1:0;
    }  
    PORT_UNLOCK;
    
    return CTC_E_NONE;    
}

int32
sys_humber_port_get_pading_en(uint16 gport, bool *enable)
{
    uint8 lchip;
    uint8 lport;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    PORT_LOCK;
    *enable = p_port_master->egs_port_prop[lchip][lport].pading_en?TRUE:FALSE;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_port_set_tx_threshold(uint16 gport, uint8 tx_threshold)
{
    uint8 lchip;
    uint8 lport;
    uint8 reg_step;
    uint8 field_step;
    uint16 reg_id;
    uint16 field_id;
    uint32 cmd;
    uint8 reg_value;
    int32 ret = CTC_E_NONE;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    if (CTC_PORT_MAC_GMAC == p_port_master->igs_port_prop[lchip][lport].port_mac_type)
    {
        /*tx_threshold of gmac has 7 bits*/
        if (tx_threshold > 0x7f)
        {
            return CTC_E_INVALID_TX_THRESHOLD;
        }
    }
    else
    {
        /*tx_threshold of xgmac/sgmac has 6 bits*/
        if (tx_threshold > 0x3f)
        {
            return CTC_E_INVALID_TX_THRESHOLD;
        }
    }

    PORT_LOCK;
    switch(p_port_master->igs_port_prop[lchip][lport].port_mac_type)
    {
        case CTC_PORT_MAC_GMAC:
            reg_step = GMAC1_GMACWRAPPER_GMAC_TX_CTRL - GMAC0_GMACWRAPPER_GMAC_TX_CTRL;
            reg_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL + lport*reg_step;
            field_step = GMAC1_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD - GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD;
            field_id = GMAC0_GMACWRAPPER_GMAC_TX_CTRL_TX_THRESHOLD /*+ lport*field_step*/;
            break;
        case CTC_PORT_MAC_XGMAC:
            reg_step = XGMAC1_XGMAC_CONFIG4 - XGMAC0_XGMAC_CONFIG4;
            reg_id = XGMAC0_XGMAC_CONFIG4 + (lport/12)*reg_step;
            field_step = XGMAC1_XGMAC_CONFIG4_TX_THRESHOLD - XGMAC0_XGMAC_CONFIG4_TX_THRESHOLD;
            field_id = XGMAC0_XGMAC_CONFIG4_TX_THRESHOLD /*+ (lport/12)*field_step*/;
            break;
        case CTC_PORT_MAC_SGMAC:
            reg_step = SGMAC1_SGMAC_CONFIG4 - SGMAC0_SGMAC_CONFIG4;
            reg_id = SGMAC0_SGMAC_CONFIG4 + (lport-SYS_MAX_GMAC_NUM)*reg_step;
            field_step = SGMAC1_SGMAC_CONFIG4_TX_THRESHOLD - SGMAC0_SGMAC_CONFIG4_TX_THRESHOLD;
            field_id = SGMAC0_SGMAC_CONFIG4_TX_THRESHOLD /*+ (lport-SYS_MAX_GMAC_NUM)*field_step*/;
            break;
        default:
            PORT_UNLOCK;
            return CTC_E_INVALID_PORT_MAC_TYPE;
    }

    reg_value = p_port_master->igs_port_prop[lchip][lport].ptp_en ? 0 : tx_threshold;
    cmd = DRV_IOW(IOC_REG, reg_id, field_id);
    ret = drv_reg_ioctl(lchip, 0, cmd, &reg_value);
    if (CTC_E_NONE == ret)
    {
        p_port_master->egs_port_prop[lchip][lport].tx_threshold = tx_threshold;
    }

    PORT_UNLOCK;

    return ret;
}

int32
sys_humber_port_get_tx_threshold(uint16 gport, uint8 *tx_threshold)
{
    uint8 lchip;
    uint8 lport;

    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(tx_threshold);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if (lport >= CTC_MAX_HUMBER_PHY_PORT)
    {
        return CTC_E_INVALID_LOCAL_PORT;
    }

    PORT_LOCK;
    *tx_threshold = p_port_master->egs_port_prop[lchip][lport].tx_threshold;
    PORT_UNLOCK;

    return CTC_E_NONE;
}

 /**
 @brief set port whether the src_discard is enable
*/
int32
sys_humber_port_set_srcdiscard_en(uint16 gport, bool enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = (TRUE == enable)? 1:0;


    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d src_discard enable:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_SRC_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    return ret;
}

/**
 @brief get port whether the srcdiscard is enable
*/
int32
sys_humber_port_get_srcdiscard_en(uint16 gport, bool *enable)
{
    uint32 cmd = 0;
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 field_value = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d srcdiscard enable!\n", gport);

    /*do read*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


    cmd =  DRV_IOR(IOC_TABLE, DS_PHY_PORT, DS_PHY_PORT_SRC_DISCARD);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_value);

    *enable = field_value ? TRUE: FALSE;

    return ret;
}

int32
_sys_humber_port_maping_to_local_phy_port(uint8 lchip_id, uint8 lport, uint8 local_phy_port)
{
    uint32 cmd = 0;
    uint32 field_value = local_phy_port;

    cmd = DRV_IOW(IOC_TABLE, PHY_PORT_MAP_TABLE, PHY_PORT_MAP_TABLE_LOCAL_PHY_PORT);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip_id, lport, cmd, &field_value));

    return CTC_E_NONE;
}

int32
sys_humber_port_set_lbk_port_property (ctc_port_lbk_param_t *p_port_lbk, uint8 inter_lport)
{
    uint8 lchip;
    uint16 src_gport   = 0;
    uint8  src_lport   = 0;
    uint32 cmd         = 0;
    ds_phy_port_ext_t inter_phy_port_ext;
    ds_src_port_t inter_src_port;
    uint8 enable = p_port_lbk->lbk_enable?1:0;

    src_gport = p_port_lbk->src_gport;
    SYS_MAP_GPORT_TO_LPORT(src_gport, lchip, src_lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, inter_lport, cmd, &inter_src_port));
    inter_src_port.port_cross_connect     = enable;
    inter_src_port.receive_en             = enable;
    inter_src_port.bridge_en              = 0;
    inter_src_port.default_replace_tag_en = enable;
    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, inter_lport, cmd, &inter_src_port));

    /*l2pdu same to src port*/
    cmd = DRV_IOR(IOC_TABLE, DS_PHY_PORT_EXT, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, inter_lport, cmd, &inter_phy_port_ext));
    if (enable)
    {
        if (p_port_lbk->efm_to_cpu_en)
        {
            if (p_port_lbk->efm_to_cpu_index >= 16)
            {
                return CTC_E_INVALID_INDEX;
            }
            inter_phy_port_ext.exception2_en = 1 << p_port_lbk->efm_to_cpu_index;
            inter_phy_port_ext.exception2_discard = 0;
        }
        else
        {
            inter_phy_port_ext.exception2_en      = 0;
            inter_phy_port_ext.exception2_discard = 0;
        }
    }
    else
    {
        inter_phy_port_ext.exception2_en      = 0;
        inter_phy_port_ext.exception2_discard = 0;
    }

    inter_phy_port_ext.default_vlan_id = 0;
    cmd = DRV_IOW(IOC_TABLE, DS_PHY_PORT_EXT, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, inter_lport, cmd, &inter_phy_port_ext));

    return CTC_E_NONE;

}


int32
sys_humber_port_set_loopback (ctc_port_lbk_param_t *p_port_lbk)
{
    uint16 inter_gport = 0;
    uint8  inter_lport = 0;
    uint16 src_gport   = 0;
    uint8  src_lport   = 0;
    uint16 dst_gport   = 0;
    uint8  gchip       = 0;
    uint8  lchip       = 0;
    int32  ret         = CTC_E_NONE;
    sys_nh_param_crscnt_t crscnt_param;

    #define RET_PROCESS_WITH_ERROR(func) ret = ret? ret: (func)

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_port_lbk);
    SYS_PORT_DEBUG_FUNC();
    src_gport = p_port_lbk->src_gport;
    dst_gport = p_port_lbk->dst_gport;
    /*not support linkagg*/
    if (CTC_IS_LINKAGG_PORT(src_gport))
    {
        return CTC_E_INVALID_PARAM;
    }

    SYS_MAP_GPORT_TO_LPORT(src_gport, lchip, src_lport);
    gchip = CTC_MAP_GPORT_TO_GCHIP(src_gport);
    CTC_GLOBAL_CHIPID_CHECK(gchip);
    kal_memset(&crscnt_param, 0, sizeof(sys_nh_param_crscnt_t));
    if (p_port_lbk->lbk_enable)
    {
        if (p_port_master->igs_port_prop[lchip][src_lport].lbk_en)
        {
            return CTC_E_ENTRY_EXIST;
        }

        /* allocate internal port */
        CTC_ERROR_RETURN(sys_humber_get_rsv_internal_port(lchip, &inter_lport));
        inter_gport =  CTC_MAP_LPORT_TO_GPORT(gchip, inter_lport);

        SYS_PORT_DEBUG_INFO("src_gport:0x%x, dst_gport:0x%x, inter_lport= %d!\n", src_gport, dst_gport, inter_lport);

        if (src_gport == dst_gport)
        {
            RET_PROCESS_WITH_ERROR(sys_humber_add_port_to_channel(inter_gport, src_lport));
            RET_PROCESS_WITH_ERROR(sys_humber_add_port_to_channel(src_gport, SYS_DROP_CHANNEL_ID_START));
            crscnt_param.destport = inter_gport;
        }
        else
        {
            crscnt_param.destport = dst_gport;
        }

        crscnt_param.srcport = inter_gport;

        switch(p_port_lbk->lbk_type)
        {
            case CTC_PORT_LBK_TYPE_SWAP_MAC:
                crscnt_param.swap_mac = TRUE;
                break;

            case CTC_PORT_LBK_TYPE_BYPASS:
                crscnt_param.swap_mac = FALSE;
                break;

            default:
                RET_PROCESS_WITH_ERROR(CTC_E_INVALID_PARAM);
        }
        RET_PROCESS_WITH_ERROR(sys_humber_nh_update_port_crscnt_nexthop(&crscnt_param));

        /*set lbk port property*/
        RET_PROCESS_WITH_ERROR(sys_humber_port_set_lbk_port_property (p_port_lbk, inter_lport));
        if (CTC_E_NONE != ret)
        {
            sys_humber_free_rsv_internal_port(lchip, inter_lport);
            return ret;
        }

        /* map to internal port*/
        RET_PROCESS_WITH_ERROR(_sys_humber_port_maping_to_local_phy_port(lchip, src_lport, inter_lport));

        /*save the port*/
    PORT_LOCK;
        p_port_master->igs_port_prop[lchip][src_lport].inter_lport = inter_lport;
        p_port_master->igs_port_prop[lchip][src_lport].lbk_en= TRUE;
    PORT_UNLOCK;

    }
    else
    {

        if (!p_port_master->igs_port_prop[lchip][src_lport].lbk_en)
        {
            return CTC_E_ENTRY_NOT_EXIST;
        }

    PORT_LOCK;
        p_port_master->igs_port_prop[lchip][src_lport].lbk_en = FALSE;
        inter_lport = p_port_master->igs_port_prop[lchip][src_lport].inter_lport;
    PORT_UNLOCK;

        CTC_ERROR_RETURN(_sys_humber_port_maping_to_local_phy_port(lchip, src_lport, src_lport));

        inter_gport =  CTC_MAP_LPORT_TO_GPORT(gchip, inter_lport);
        CTC_ERROR_RETURN(sys_humber_port_set_lbk_port_property (p_port_lbk, inter_lport));

        if (inter_lport)
        {
            inter_gport = CTC_MAP_LPORT_TO_GPORT(gchip, inter_lport);

            if (src_gport == dst_gport)
            {
                CTC_ERROR_RETURN(sys_humber_remove_port_from_channel(inter_gport, src_lport));
                CTC_ERROR_RETURN(sys_humber_remove_port_from_channel(src_gport, SYS_DROP_CHANNEL_ID_START));
            }
            /* Release internal port */
            CTC_ERROR_RETURN(sys_humber_free_rsv_internal_port(lchip, inter_lport));
        }


    }

    return CTC_E_NONE;
}

/**
 @brief set port whether the src port match check is enable
*/
int32
sys_humber_port_set_port_check_en(uint16 gport, bool enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = (TRUE == enable)? 1:0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Set port:%d match check:%d!\n", gport, enable);

    /*do write table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOW(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_CHECK_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    return ret;
}

/**
 @brief get port whether the src port match check is enable
*/
int32
sys_humber_port_get_port_check_en(uint16 gport, bool *enable)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint32 cmd = 0;
    uint32 field_val = 0;

    /*Sanity check*/
    if (NULL == p_port_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(enable);
    SYS_PORT_DEBUG_FUNC();
    SYS_PORT_DEBUG_INFO("Get port:%d src match check enable!\n", gport);

    /*do read table*/
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    cmd = DRV_IOR(IOC_TABLE, DS_SRC_PORT, DS_SRC_PORT_PORT_CHECK_EN);
    ret = drv_tbl_ioctl(lchip, lport, cmd, &field_val);

    *enable = (1 == field_val) ? TRUE:FALSE;

    return ret;
}

