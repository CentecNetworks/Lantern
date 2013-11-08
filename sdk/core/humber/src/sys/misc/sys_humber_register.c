/**
 @file sys_humber_register.c

 @date 2009-11-6

 @version v2.0


*/

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_error.h"
#include "drv_humber.h"
#include "drv_io.h"
#include "drv_tbl_reg.h"
#include "drv_humber_data_path.h"

#include "sys_humber_pdu.h"

#include "sys_humber_chip.h"
#include "sys_humber_pdu.h"
#include "sys_humber_register.h"
#include "sys_humber_ftm.h"
#include "sys_humber_stp.h"
#include "sys_humber_parser.h"
#include "drv_cfg.h"
/****************************************************************************
*
* Global and Declaration
*
*****************************************************************************/
static sys_global_control_db_t *p_global_control_db = NULL;
/****************************************************************************
 *
 * Function
 *
 *****************************************************************************/

static int32
 _sys_humber_ipe_bridge_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    ipe_bridge_ctl_t brg_ctl;

    kal_memset(&brg_ctl, 0, sizeof(ipe_bridge_ctl_t));

    brg_ctl.use_ip_hash = 1;
    brg_ctl.protocol_exception = 0x1700;/*for ipv4 and ipv6 pdu packet, like ospf, rip, no arp and rarp*/;

    cmd = DRV_IOW(IOC_REG, IPE_BRIDGE_CTL, DRV_ENTRY_FLAG);

        local_chip_num = sys_humber_get_local_chip_num();

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &brg_ctl));
    }

    return CTC_E_NONE;
}



static int32
 _sys_humber_ipe_router_mac_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    ipe_router_mac_ctl_t router_mac;

    kal_memset(&router_mac, 0, sizeof(ipe_router_mac_ctl_t));

    router_mac.router_mac_type0_bit47_to32 = 0;
    router_mac.router_mac_type0_bit31_to8 = 0x0AFFFF;
    router_mac.router_mac_type1_bit47_to32 = 0;
    router_mac.router_mac_type1_bit31_to8 = 0x0AAAAA;
    router_mac.router_mac_type2_bit47_to32 = 0;
    router_mac.router_mac_type2_bit31_to8 = 0x0A5555;

        local_chip_num = sys_humber_get_local_chip_num();

    for (i = 0; i < local_chip_num; i++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_ROUTER_MAC_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &router_mac));
    }

    return CTC_E_NONE;
}

static int32
 _sys_humber_ds_vlan_ctl_init()
 {
     int32 cmd;
     uint8 i, local_chip_num;
     uint32 stp_shift = SYS_HUMBER_STP_STATE_SHIFT;
     uint32 vlan_status_base = 0x13;
     uint32 table_base_bit11_to8 = 0;
     uint32 table_base_bit19_to12 = 0;

     local_chip_num = sys_humber_get_local_chip_num();

     for (i = 0; i < local_chip_num; i++)
     {
        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_DS_STP_STATE_SHIFT);
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &stp_shift));
        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_DS_VLAN_STATUS_TABLE_BASE);
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &vlan_status_base));

        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_DS_VLAN_TABLE_BASE_BIT11_TO8);
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &table_base_bit11_to8));
        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_DS_VLAN_TABLE_BASE_BIT19_TO12);
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &table_base_bit19_to12));
     }

     return CTC_E_NONE;
 }

static int32
 _sys_humber_phy_port_mux_ctl_init()
{
    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_hdr_adj_exp_map_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    ipe_hdr_adj_exp_map_table_t exp_map;

    exp_map.priority0 = 32;
    exp_map.color0 = 3;
    exp_map.priority1 = 40;
    exp_map.color1 = 3;
    exp_map.priority2 = 48;
    exp_map.color2 = 3;
    exp_map.priority3 = 56;
    exp_map.color3 = 3;
    exp_map.priority4 = 0;
    exp_map.color4 = 1;
    exp_map.priority5 = 8;
    exp_map.color5 = 3;
    exp_map.priority6 = 16;
    exp_map.color6 = 3;
    exp_map.priority7 = 24;
    exp_map.color7 = 3;

        local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_EXP_MAP_TABLE, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &exp_map));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_hdr_adj_ctl()
{
    uint8 i, local_chip_num;
    int32 cmd;
    ipe_hdr_adj_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(ipe_hdr_adj_ctl_t));

    ctl.ptp_discard = 1;
    ctl.parser_length_error_mode = 1;
    ctl.vrf_id_en_cfg = 1;
    ctl.vpls_src_port_en_cfg = 1;
    ctl.service_id_en_cfg = 1;

    cmd= DRV_IOW(IOC_REG, IPE_HDR_ADJ_CTL, DRV_ENTRY_FLAG);

        local_chip_num = sys_humber_get_local_chip_num();

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ctl));
    }

    return CTC_E_NONE;
}

/*default value of master is 0*/
static int32
_sys_humber_ipe_hdr_adj_vlan_ptr_init()
{
    return CTC_E_NONE;
}

/*for sgmac?*/
static int32
_sys_humber_ipe_hdr_adj_mode_ctl_init()
{
    uint8 i, local_chip_num;
    uint32 mode = 0;
    int32 cmd;

    cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_MODE_CTL, IPE_HDR_ADJ_MODE_CTL_MODE);

        local_chip_num = sys_humber_get_local_chip_num();

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &mode));
    }

    return CTC_E_NONE;
}

static int32
 _sys_humber_ipe_intf_map_ctl_init()
{
    uint8 i, local_chip_num;
    int32 cmd;
    ipe_intf_mapper_ctl_t intf_mapper_ctl;

    kal_memset(&intf_mapper_ctl, 0, sizeof(ipe_intf_mapper_ctl_t));

    intf_mapper_ctl.ether_oam_obey_bypass_all = 0;
    intf_mapper_ctl.arp_exception_sub_index   = SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_ARP_TO_CPU;
    intf_mapper_ctl.dhcp_exception_sub_index = SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_DHCP_TO_CPU;
    intf_mapper_ctl.arp_unicast_discard = 0;
    intf_mapper_ctl.arp_unicast_exception_disable = 1;
    intf_mapper_ctl.arp_broadcast_routed_port_discard = 1;
    intf_mapper_ctl.dhcp_unicast_discard = 0;
    intf_mapper_ctl.dhcp_unicast_exception_disable = 1;

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_CTL, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &intf_mapper_ctl));
    }

    return CTC_E_NONE;

}

static int32
_sys_humber_ipe_intfmap_max_pkt_length()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 field_val = 0x3FFF;

    cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_MAX_PKT_LENGTH, IPE_INTF_MAPPER_MAX_PKT_LENGTH_MAX_PKT_LENGTH);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &field_val));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_ipg_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    ipe_ipg_ctl_t ipg_ctl;

    ipg_ctl.ipg0 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg1 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg2 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg3 = CTC_DEFAULT_IPG;

    cmd = DRV_IOW(IOC_REG, IPE_IPG_CTL, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipg_ctl));
    }

    return CTC_E_NONE;
}
static int32
 _sys_humber_epe_hdr_edit_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    epe_hdr_edit_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(epe_hdr_edit_ctl_t));

    ctl.interface_stats_en = 1;
    ctl.flow_id_en = 1;
    ctl.vpls_src_port_en = 1;
    ctl.vrf_id_en = 1;
    ctl.service_id_en = 1;
    ctl.error_layer4_offset = 192;
    ctl.rx_ether_oam_critical = 1;

        local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ctl));
    }

    return CTC_E_NONE;

}

static int32
 _sys_humber_epe_pkt_proc_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    epe_pkt_proc_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(epe_pkt_proc_ctl_t));

    ctl.cbp_tci_res2_check_en = 1;
    ctl.discard_tunnel_ttl0 = 1;
    ctl.discard_route_ttl0 = 1;
    ctl.discard_mpls_tag_ttl0 = 1;
    ctl.discard_mpls_ttl0 = 1;
    ctl.route_obey_stp = 1;  /*route_obey_stp on vlan interface*/
    ctl.parser_length_error_mode = 1;
    /*cpu port ?*/
    ctl.bfd_en = 0;           /*added by BFD OAM, not support bfd to cpu!!*/
    ctl.cpu_port = 53;        /*added by BFD OAM*/
    ctl.bfd_udp_port0 = 3784; /*added by BFD OAM*/
    ctl.bfd_udp_port1 = 4784; /*added by BFD OAM*/

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_PKT_PROC_CTL, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ctl));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_epe_l2_ether_type_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    epe_l2_ether_type_t type;

    kal_memset(&type, 0, sizeof(epe_l2_ether_type_t));

    type.epe_l2_ether_type0 = 0;
    type.epe_l2_ether_type1 = 0x0800;
    type.epe_l2_ether_type2 = 0x8847;
    type.epe_l2_ether_type3 = 0x86DD;
    type.epe_l2_ether_type4 = 0x8848;
    type.epe_l2_ether_type7 = 0;

        local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_L2_ETHER_TYPE, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &type));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_epe_hdr_adj_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;
    epe_hdr_adjust_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(epe_hdr_adjust_ctl_t));

    ctl.vrf_id_en = 1;
    ctl.flow_id_en = 1;
    ctl.vpls_src_port_en = 1;
    ctl.cfg_service_id_en = 1;

     local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_HDR_ADJUST_CTL, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ctl));
    }

    return CTC_E_NONE;
}

static int32
 _sys_humber_epe_nexthop_ctl_init()
{
    int32 cmd;
    uint8 i, local_chip_num;

    epe_next_hop_ctl_t ctl;
    sys_alloc_allocated_sram_info_t alloc_sram;

    kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
    kal_memset(&ctl, 0, sizeof(epe_next_hop_ctl_t));

    /* Modified for packet with action: output to the ingress port */
    ctl.discard_bridge_l2_match = 0;
    ctl.force_bridge_l3_match = 1;
    ctl.discard_vpls_tunnel_match = 1;
    ctl.ds_stp_state_shift = SYS_HUMBER_STP_STATE_SHIFT;
    ctl.deny_duplicate_mirror = 1;
    ctl.route_obey_isolate = 1;
    ctl.ds_next_hop_internal_base = SYS_HUMBER_DSNH_INTERNAL_BASE;
    ctl.edit_ptr_bits_num = SYS_HUMBER_EDIT_PTR_TYPE_DEFAULT;

    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_NEXTHOP, &alloc_sram));
    ctl.ds_next_hop_table_base  = alloc_sram.table_base;

    kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_L2_EDIT_ETH4W, &alloc_sram));
    ctl.ds_l2_edit_table_base   = alloc_sram.table_base;
    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_L3EDIT_MPLS4W, &alloc_sram));
    ctl.ds_l3_edit_table_base   = alloc_sram.table_base;
    ctl.ds_vlan_status_table_base = 0x13;
    ctl.ds_vlan_table_base = 0;

    /* epe oam ctl */
    ctl.oam_bypass_port_tx = 1;
    ctl.oam_bypass_vlan_tx = 1;
    ctl.oam_obey_l2_match = 1;

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_NEXT_HOP_CTL, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ctl));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_epe_aclqos_ctl_init()
{
    uint8 chip_id;
    uint8 local_chip_num;
    uint8 index_shift = 0;  /* 0 80, 1 160, 2 320 3 640 */
    uint32 cmd = 0;
    uint32 index_base = 0;
    uint32 table_base = 0;
    epe_acl_qos_ctl_t epe_aclqos_ctl;
    sys_alloc_allocated_tcam_info_t alloc_tcam;
    sys_alloc_allocated_sram_info_t alloc_sram;

    kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
    kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
    kal_memset(&epe_aclqos_ctl, 0, sizeof(epe_acl_qos_ctl_t));

    /* DsAclMacKey */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_MAC_KEY, &alloc_tcam));
    epe_aclqos_ctl.acl_qos_lookup_ctl2 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                    |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MAC_ACL, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.acl_lookup_result_ctl238_to32 = index_shift << 4;
    epe_aclqos_ctl.acl_lookup_result_ctl231_to0 = index_base << 12 | table_base;


    /* DsAclIpv4Key */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV4_KEY, &alloc_tcam));
    epe_aclqos_ctl.acl_qos_lookup_ctl1 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                    |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_ACL, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.acl_lookup_result_ctl138_to32 = index_shift << 4;
    epe_aclqos_ctl.acl_lookup_result_ctl131_to0 = index_base << 12 | table_base;

    /* DsAclIpv6Key */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV6_KEY, &alloc_tcam));
    epe_aclqos_ctl.acl_qos_lookup_ctl0 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                    |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_ACL, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.acl_lookup_result_ctl038_to32 = index_shift << 4;
    epe_aclqos_ctl.acl_lookup_result_ctl031_to0 = index_base << 12 | table_base;

    /* DsQoSMacKey */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_MAC_KEY, &alloc_tcam));
    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MAC_QOS, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.qos_lookup_result_ctl238_to32 = index_shift << 4;
    epe_aclqos_ctl.qos_lookup_result_ctl231_to0 = index_base << 12 | table_base;

    /* DsQoSIpv4Key */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV4_KEY, &alloc_tcam));
    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_QOS, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.qos_lookup_result_ctl138_to32 = index_shift << 4;
    epe_aclqos_ctl.qos_lookup_result_ctl131_to0 = index_base << 12 | table_base;

    /* DsQoSIpv6Key */
    CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV6_KEY, &alloc_tcam));
    CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_QOS, &alloc_sram));
    index_base = alloc_tcam.index_base;
    table_base = alloc_sram.table_base;
    index_shift = alloc_tcam.index_shift;

    epe_aclqos_ctl.qos_lookup_result_ctl038_to32 = index_shift << 4;
    epe_aclqos_ctl.qos_lookup_result_ctl031_to0 = index_base << 12 | table_base;


    epe_aclqos_ctl.dual_acl_lookup = 0;
    epe_aclqos_ctl.merge_mac_ip_acl_key = 1;

    cmd = DRV_IOW(IOC_REG, EPE_ACL_QOS_CTL, DRV_ENTRY_FLAG);

    local_chip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &epe_aclqos_ctl));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_epe_ipg_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    epe_ipg_ctl_t ipg_ctl;

    ipg_ctl.ipg0 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg1 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg2 = CTC_DEFAULT_IPG;
    ipg_ctl.ipg3 = CTC_DEFAULT_IPG;

    cmd = DRV_IOW(IOC_REG, EPE_IPG_CTL, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipg_ctl));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_ctl_config_init()
{
    int32 cmd = 0;
    uint8 chip_id = 0;
    uint8 gchip_id = 0;
    uint8 local_chip_num = 0;
    uint32 table_base = 0;
    buf_store_ctrl_t buf_store_ctrl;
    buf_store_force_local_ctrl_t buf_store_force_local_ctrl;
    met_fifo_ctl_t met_fifo_ctl;
    met_fifo_wrr_ctl_t met_fifo_wrr_ctl;
    buffer_retrieve_header_version_t buffer_retrieve_header_version;

    qmgrlinklist_q_mgr_queue_id_max_num_t qmgrlinklist_q_mgr_queue_id_max_num;

    sys_alloc_allocated_sram_info_t alloc_sram;

    kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
    kal_memset(&buf_store_ctrl, 0, sizeof(buf_store_ctrl));
    kal_memset(&buf_store_force_local_ctrl, 0, sizeof(buf_store_force_local_ctrl));
    kal_memset(&met_fifo_ctl, 0, sizeof(met_fifo_ctl));
    kal_memset(&met_fifo_wrr_ctl, 0, sizeof(met_fifo_wrr_ctl));
    kal_memset(&buffer_retrieve_header_version, 0, sizeof(buffer_retrieve_header_version));

    kal_memset(&qmgrlinklist_q_mgr_queue_id_max_num, 0, sizeof(qmgrlinklist_q_mgr_queue_id_max_num));

    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id ++)
    {
        buf_store_ctrl.bay_id                   = chip_id;
        buf_store_ctrl.bay_id_chk_disable       = 1;
        buf_store_ctrl.mergee_loop_resrc_id     = 1;
        buf_store_ctrl.mcast_met_fifo_enable    = 1;
        buf_store_ctrl.local_switching_disable  = 0;
        cmd = DRV_IOW(IOC_REG, BUF_STORE_CTRL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &buf_store_ctrl));

        buf_store_force_local_ctrl.dest_map_mask0    = 0xF0;
        buf_store_force_local_ctrl.dest_map_value0   = 0x10;
        buf_store_force_local_ctrl.dest_map_mask1    = 0x3FFFFF;
        buf_store_force_local_ctrl.dest_map_value1   = 0x3FFFFF;
        buf_store_force_local_ctrl.dest_map_mask2    = 0x3FFFFF;
        buf_store_force_local_ctrl.dest_map_value2   = 0x3FFFFF;
        buf_store_force_local_ctrl.dest_map_mask3    = 0x3FFFFF;
        buf_store_force_local_ctrl.dest_map_value3   = 0x3FFFFF;
        cmd = DRV_IOW(IOC_REG, BUF_STORE_FORCE_LOCAL_CTRL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &buf_store_force_local_ctrl));

        sys_humber_get_gchip_id(chip_id, &gchip_id);
        met_fifo_ctl.humber_id  = gchip_id;
        met_fifo_ctl.port_check_en =  1;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MET_ENTRY, &alloc_sram));
        table_base = alloc_sram.table_base;

        met_fifo_ctl.ds_met_entry_base = table_base & 0xFFF;
        met_fifo_ctl.discard_met_loop = 1;

        cmd = DRV_IOW(IOC_REG, MET_FIFO_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &met_fifo_ctl));

        met_fifo_wrr_ctl.max_pending_mcast   = 8;
        met_fifo_wrr_ctl.ucast_hi_weight     = 1;
        met_fifo_wrr_ctl.ucast_lo_weight     = 1;
        met_fifo_wrr_ctl.mcast_hi_weight     = 1;
        met_fifo_wrr_ctl.mcast_lo_weight     = 1;
        cmd = DRV_IOW(IOC_REG, MET_FIFO_WRR_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &met_fifo_wrr_ctl));

        buffer_retrieve_header_version.dest_chip15_to0_header_version  = 0x55555555;
        buffer_retrieve_header_version.dest_chip31_to16_header_version = 0x55555555;
        cmd = DRV_IOW(IOC_REG, BUFFER_RETRIEVE_HEADER_VERSION, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &buffer_retrieve_header_version));

        qmgrlinklist_q_mgr_queue_id_max_num.queue_id_max_num = 0x7FF;
        cmd = DRV_IOW(IOC_REG, QMGRLINKLIST_Q_MGR_QUEUE_ID_MAX_NUM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &qmgrlinklist_q_mgr_queue_id_max_num));

    }

  return CTC_E_NONE;
}

static int32
_sys_humber_queue_tbl_config_init(void)
{
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 cmd = 0;
    uint32 index = 0;
    uint8 priority = 0;
    uint8 color = 0;
    ds_queue_map_t queue_map;

    /* 2K queue in total */
    ds_head_hash_mod_t ds_head_hash_mod;
    met_fifo_priority_map_table_t met_fifo_priority_map_table;

    kal_memset(&ds_head_hash_mod, 0, sizeof(ds_head_hash_mod));
    kal_memset(&met_fifo_priority_map_table, 0, sizeof(met_fifo_priority_map_table));
    kal_memset(&queue_map, 0, sizeof(queue_map));

    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        /* DS_QUEUE_MAP */
        queue_map.grp_id = 0x1ff;

        cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DRV_ENTRY_FLAG);
        for (index = 0; index < 2048; index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &queue_map));
        }

        /* DS_HEAD_HASH_MOD */
        cmd = DRV_IOW(IOC_TABLE, DS_HEAD_HASH_MOD, DRV_ENTRY_FLAG);
        for (index = 0; index < 256; index ++)
        {
            ds_head_hash_mod.mod3 = index % 3;
            ds_head_hash_mod.mod5 = index % 5;
            ds_head_hash_mod.mod6 = index % 6;
            ds_head_hash_mod.mod7 = index % 7;
            ds_head_hash_mod.mod9 = index % 9;
            ds_head_hash_mod.mod10 = index % 10;
            ds_head_hash_mod.mod11 = index % 11;
            ds_head_hash_mod.mod12 = index % 12;
            ds_head_hash_mod.mod13 = index % 13;
            ds_head_hash_mod.mod14 = index % 14;
            ds_head_hash_mod.mod15 = index % 15;

            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &ds_head_hash_mod));
        }

        /* MET_FIFO_PRIORITY_MAP_TABLE */
        cmd = DRV_IOW(IOC_TABLE, MET_FIFO_PRIORITY_MAP_TABLE, DRV_ENTRY_FLAG);
        for (priority = 0; priority < 64; priority ++)
        {
            index = priority * 4;
            for (color = 0; color < 4; color ++)
            {
                met_fifo_priority_map_table.met_fifo_priority   = priority / 32;
                met_fifo_priority_map_table.drop_precedence     = (color + 3) % 4;
                /* the queue select and pri should be revert */
                met_fifo_priority_map_table.queue_select        = 64 - (priority+1);

                CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &met_fifo_priority_map_table));

                index ++;
            }
        }

     }

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_ipg_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    q_mgr_ethernet_ipg_t ipg_ctl;

    ipg_ctl.ipg0 = 0;
    ipg_ctl.ipg1 = 0;
    ipg_ctl.ipg2 = 0;
    ipg_ctl.ipg3 = 0;

    cmd = DRV_IOW(IOC_REG, Q_MGR_ETHERNET_IPG, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipg_ctl));
    }

    return CTC_E_NONE;
}

static uint8
_sys_humber_get_hashlkpctl_bitnum(uint32 tbl_id, uint32 maxindex, uint32 key_size)
{
    #define HASH_ENTRY_SIZE  16            /* each hash entry have 72bits (sw use 8 Bytes to sim)*/
    #define EACH_BUCKET_HAVE_ENTRY_NUM  4  /* each bucket have 72bits entry num */

    uint32 bucket_num = 0;
    uint8 i = 0;
    uint32 valid_bucket_num[] = {256, 512, 1024, 2*1024, 4*1024, 8*1024, 16*1024, 32*1024};

    bucket_num = (maxindex * (key_size / HASH_ENTRY_SIZE)) / EACH_BUCKET_HAVE_ENTRY_NUM;

   /* return (uint8)(log(bucket_num)/log(2) - 8);*/
    for (i = 0; i < sizeof(valid_bucket_num)/sizeof(uint32); i++)
    {
        if (bucket_num == valid_bucket_num[i])
        {
            return i;
        }
    }

    return 0;
}

static int32
_sys_humber_lkp_ctl_init(void)
{
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint8 key_size = 0;     /* 0 80, 1 160, 2 320 3 640 */
    uint8 index_shift = 0;  /* 0 80, 1 160, 2 320 3 640 */
    uint32 cmd = 0;
    uint32 index_base = 0;
    uint32 table_base = 0;
    uint32 max_index = 0;

    ipe_lookup_ctl_t ipe_lookup_ctl;
    ipe_lookup_result_ctl_t ipe_lookup_result_ctl;
    ipe_user_id_ctl_t ipe_user_id_ctl;
    hash_ds_ctl_lookup_ctl_t hash_lkp_ctl;
    ipe_hash_lookup_result_ctl_t hash_lkp_rslt_ctl;
    sys_alloc_allocated_tcam_info_t alloc_tcam;
    sys_alloc_allocated_sram_info_t alloc_sram;

    kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
    kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
    kal_memset(&ipe_lookup_ctl, 0, sizeof(ipe_lookup_ctl));
    kal_memset(&ipe_lookup_result_ctl, 0, sizeof(ipe_lookup_result_ctl));
    kal_memset(&ipe_user_id_ctl, 0, sizeof(ipe_user_id_ctl));
    kal_memset(&hash_lkp_ctl, 0, sizeof(hash_lkp_ctl));
    kal_memset(&hash_lkp_rslt_ctl, 0, sizeof(hash_lkp_rslt_ctl));

    local_chip_num = sys_humber_get_local_chip_num();

   /*If vlan interface's VRF is disable ,the vrfid should be equal to 0,but L2 forwaring 's FID should use VRFID from DS_VLAN*/
    ipe_lookup_ctl.global_vrf_id = 0;
    ipe_lookup_ctl.global_vrf_id_lookup_en = 1;

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        /* Mac DA & SA */
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_MAC_KEY, &alloc_tcam));
        ipe_lookup_ctl.mac_da_lookup_ctl = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;
        ipe_lookup_ctl.mac_sa_lookup_ctl = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MAC, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.mac_da_lookup_result_ctl0 = index_shift<<16;
        ipe_lookup_result_ctl.mac_da_lookup_result_ctl1 = index_base<<16 | table_base;
        ipe_lookup_result_ctl.mac_sa_lookup_result_ctl0 = 0x1<<18|index_shift<<16;
        ipe_lookup_result_ctl.mac_sa_lookup_result_ctl1 = index_base<<16 | table_base;

        /* IPv4 Ucast */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV4_UCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_da_lookup_ctl0 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_UCAST_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_da_lookup_result_ctl00 = index_shift<<16;
        ipe_lookup_result_ctl.ip_da_lookup_result_ctl01 = index_base<<16 | table_base;

        /* IPv4 Mcast */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV4_MCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_da_lookup_ctl1 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_MCAST_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_da_lookup_result_ctl10 = index_shift<<16;
        ipe_lookup_result_ctl.ip_da_lookup_result_ctl11 = index_base<<16 | table_base;

        /* IPv6 Ucast */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV6_UCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_da_lookup_ctl2 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_UCAST_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_da_lookup_result_ctl20 = index_shift<<16;
        ipe_lookup_result_ctl.ip_da_lookup_result_ctl21 = index_base<<16 | table_base;

        /* IPv6 Mcast */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV6_MCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_da_lookup_ctl3 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_MCAST_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_da_lookup_result_ctl30 = index_shift<<16;
        ipe_lookup_result_ctl.ip_da_lookup_result_ctl31 = index_base<<16 | table_base;

        /* IPv4 Ucast RPF */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV4_UCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl0 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_UCAST_SA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl00 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl01 = index_base<<16 | table_base;

        /* IPv6 Ucast RPF */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV6_UCAST_ROUTE_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl1 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_UCAST_SA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl10 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl11 = index_base<<16 | table_base;

        /* IPv4 SA NAT */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV4_NAT_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl2 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_SA_NAT, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl20 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl21 = index_base<<16 | table_base;

        /* IPv6 SA NAT */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV6_NAT_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl3 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_SA_NAT, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl30 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl31 = index_base<<16 | table_base;

        /* DS_MPLS */
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MPLS, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.ds_mpls_table_base0 = table_base & 0xF;
        ipe_lookup_result_ctl.ds_mpls_table_base1 = (table_base>>4)&0xFF;
        ipe_lookup_result_ctl.min_interface_label = 1;

        /* IPv4 PBR, is NOT allocated */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV4_PBR_DUALDA_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl4 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_UCAST_PBR_DUAL_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl40 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl41 = index_base<<16 | table_base;

        /* IPv6 PBR, is NOT allocated */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_IPV6_PBR_DUALDA_KEY, &alloc_tcam));
        ipe_lookup_ctl.ip_sa_lookup_ctl5 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_UCAST_PBR_DUAL_DA, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl50 = index_shift<<16;
        ipe_lookup_result_ctl.ip_sa_lookup_result_ctl51 = index_base<<16 | table_base;

        /* IPv4 Mcast RPF, using ip da index */
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_MCAST_RPF, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.ds_ipv4_mcast_rpf_table_base = table_base;

        /* IPv6 Mcast RPF, using ip da index */
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_MCAST_RPF, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.ds_ipv6_mcast_rpf_table_base = table_base;

        /* DsAclMacKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_MAC_KEY, &alloc_tcam));
        if(0 == alloc_tcam.max_index)
        {
            CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_MAC_KEY, &alloc_tcam));
        }
        ipe_lookup_ctl.acl_qos_lookup_ctl2 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_MAC_KEY, &alloc_tcam));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MAC_ACL, &alloc_sram));

        if (0 == alloc_tcam.max_index)
        {
            uint32 index_qos_dft_entry = 0;
            uint32 index_base_qos_dft_entry = 0;

            /* map mac acl AD of index 0x3fff to qos ipv4 default entry's AD */
            CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV4_KEY, &alloc_tcam));
            CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_QOS, &alloc_sram));
            table_base = alloc_sram.table_base;
            index_shift = alloc_tcam.index_shift;
            index_base = alloc_tcam.index_base;
            
            index_qos_dft_entry      = alloc_tcam.max_index - (1 << alloc_tcam.index_shift);
            index_base_qos_dft_entry = (index_qos_dft_entry << alloc_tcam.index_shift) >> 8;

            if (0x3f >= index_base_qos_dft_entry)
            {
                index_base = 0x3f - index_base_qos_dft_entry;
            }
        }


        ipe_lookup_result_ctl.acl_lookup_result_ctl20 = index_shift << 16;
        ipe_lookup_result_ctl.acl_lookup_result_ctl21 = index_base << 16 | table_base;

        /* DsAclIpv4Key */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV4_KEY, &alloc_tcam));
        if(0 == alloc_tcam.max_index)
        {
            CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV4_KEY, &alloc_tcam));
        }
        ipe_lookup_ctl.acl_qos_lookup_ctl1 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV4_KEY, &alloc_tcam));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_ACL, &alloc_sram));

        if (0 == alloc_tcam.max_index)
        {
            uint32 index_qos_dft_entry = 0;
            uint32 index_base_qos_dft_entry = 0;
            
            /* map ipv4 acl AD of index 0x3fff to qos ipv4 default entry's AD */
            CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV4_KEY, &alloc_tcam));
            CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_QOS, &alloc_sram));
            table_base = alloc_sram.table_base;
            index_shift = alloc_tcam.index_shift;
            index_base = alloc_tcam.index_base;
            
            /* map acl AD of index 0x3fff to qos default entry's */
            index_qos_dft_entry      = alloc_tcam.max_index - (1 << alloc_tcam.index_shift);
            index_base_qos_dft_entry = (index_qos_dft_entry << alloc_tcam.index_shift) >> 8;

            if (0x3f >= index_base_qos_dft_entry)
            {
                index_base = 0x3f - index_base_qos_dft_entry;
            }
        }

        ipe_lookup_result_ctl.acl_lookup_result_ctl10 = index_shift << 16;
        ipe_lookup_result_ctl.acl_lookup_result_ctl11 = index_base << 16 | table_base;

        /* DsAclIpv6Key */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV6_KEY, &alloc_tcam));
        if(0 == alloc_tcam.max_index)
        {
            CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV6_KEY, &alloc_tcam));
        }
        ipe_lookup_ctl.acl_qos_lookup_ctl0 = alloc_tcam.is_hash_used<<15 | alloc_tcam.is_ext_tcam<<14 | alloc_tcam.index_shift<<12
                                        |alloc_tcam.table_id<<8 |alloc_tcam.ltr_id;

        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_ACL_IPV6_KEY, &alloc_tcam));
        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_ACL, &alloc_sram));
        index_base = alloc_tcam.index_base;
        table_base = alloc_sram.table_base;
        index_shift = alloc_tcam.index_shift;

        ipe_lookup_result_ctl.acl_lookup_result_ctl00 = index_shift << 16;
        ipe_lookup_result_ctl.acl_lookup_result_ctl01 = index_base << 16 | table_base;

        /* DsQoSMacKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_MAC_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;
        index_shift = alloc_tcam.index_shift;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_MAC_QOS, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.qos_lookup_result_ctl20 = index_shift << 16;
        ipe_lookup_result_ctl.qos_lookup_result_ctl21 = index_base << 16 | table_base;

        /* DsQoSIpv4Key */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV4_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;
        index_shift = alloc_tcam.index_shift;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV4_QOS, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.qos_lookup_result_ctl10 = index_shift << 16;
        ipe_lookup_result_ctl.qos_lookup_result_ctl11 = index_base << 16 | table_base;

        /* DsQoSIpv6Key */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_QOS_IPV6_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;
        index_shift = alloc_tcam.index_shift;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_IPV6_QOS, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_lookup_result_ctl.qos_lookup_result_ctl00 = index_shift << 16;
        ipe_lookup_result_ctl.qos_lookup_result_ctl01 = index_base << 16 | table_base;

        /*Generic Configuration */
        ipe_lookup_ctl.dual_acl_lookup = 0;
        ipe_lookup_ctl.merge_mac_ip_acl_key = 1;
        ipe_lookup_ctl.ether_oam_discard_lookup_en = 0; /* set by oam */
        ipe_lookup_ctl.oam_obey_acl_qos = 0;            /* set by oam */
        ipe_lookup_ctl.route_obey_stp = 0;              /*route_obey_stp on vlan interface*/
        ipe_lookup_ctl.routed_port_disable_bcast_bridge = 1;
        ipe_lookup_ctl.ipv4_ucast_route_key_sa_en = 1;

        /* IPE_LOOKUP_CTL */
        cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_lookup_ctl));

        /* IPE_LOOKUP_RESULT_CTL */
        cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_lookup_result_ctl));

        /* IPE_USER_ID_CTL*/
        /* DsUserIdMacKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_USER_ID_MAC_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_USER_ID_MAC, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_user_id_ctl.arp_force_ipv4 = 1;
        ipe_user_id_ctl.lookup_inst0    = alloc_tcam.ltr_id;
        ipe_user_id_ctl.index_shift0    = alloc_tcam.index_shift;
        ipe_user_id_ctl.key_size0       = alloc_tcam.index_shift;
        ipe_user_id_ctl.table_id0       = alloc_tcam.table_id;
        ipe_user_id_ctl.index_base0     = index_base;
        ipe_user_id_ctl.table_base0     = table_base;
        ipe_user_id_ctl.use_ext_tcam0   = alloc_tcam.is_ext_tcam;
        ipe_user_id_ctl.use_hash_tcam0  = alloc_tcam.is_hash_used;

        /* DsUserIdIpv6Key*/
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_USER_ID_IPV6_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_USER_ID_IPV6, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_user_id_ctl.lookup_inst1    = alloc_tcam.ltr_id;
        ipe_user_id_ctl.index_shift1    = alloc_tcam.index_shift;
        ipe_user_id_ctl.key_size1       = alloc_tcam.index_shift;
        ipe_user_id_ctl.table_id1       = alloc_tcam.table_id;
        ipe_user_id_ctl.index_base1     = index_base;
        ipe_user_id_ctl.table_base1     = table_base;
        ipe_user_id_ctl.use_ext_tcam1   = alloc_tcam.is_ext_tcam;
        ipe_user_id_ctl.use_hash_tcam1  = alloc_tcam.is_hash_used;

        /* DsUserIdIpv4Key*/
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_USER_ID_IPV4_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_USER_ID_IPV4, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_user_id_ctl.force_mac_key = 1;
        ipe_user_id_ctl.lookup_inst2    = alloc_tcam.ltr_id;
        ipe_user_id_ctl.index_shift2    = alloc_tcam.index_shift;
        ipe_user_id_ctl.key_size2       = alloc_tcam.index_shift;
        ipe_user_id_ctl.table_id2       = alloc_tcam.table_id;
        ipe_user_id_ctl.index_base2     = index_base;
        ipe_user_id_ctl.table_base2     = table_base;
        ipe_user_id_ctl.use_ext_tcam2   = alloc_tcam.is_ext_tcam;
        ipe_user_id_ctl.use_hash_tcam2  = alloc_tcam.is_hash_used;

        /* DsUserIdVlanKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        kal_memset(&alloc_sram, 0, sizeof(sys_alloc_allocated_sram_info_t));
        CTC_ERROR_RETURN(sys_alloc_get_tcam_key_alloc_info(DS_USER_ID_VLAN_KEY, &alloc_tcam));
        index_base = alloc_tcam.index_base;

        CTC_ERROR_RETURN(sys_alloc_get_sram_table_base(DS_USER_ID_VLAN, &alloc_sram));
        table_base = alloc_sram.table_base;

        ipe_user_id_ctl.lookup_inst3    = alloc_tcam.ltr_id;
        ipe_user_id_ctl.index_shift3    = alloc_tcam.index_shift;
        ipe_user_id_ctl.key_size3       = alloc_tcam.index_shift;
        ipe_user_id_ctl.table_id3       = alloc_tcam.table_id;
        ipe_user_id_ctl.index_base3     = index_base;
        ipe_user_id_ctl.table_base3     = table_base;
        ipe_user_id_ctl.use_ext_tcam3   = alloc_tcam.is_ext_tcam;
        ipe_user_id_ctl.use_hash_tcam3  = alloc_tcam.is_hash_used;

        cmd = DRV_IOW(IOC_REG, IPE_USER_ID_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_user_id_ctl));

        /* Init hash lookup control register */
        /* Tcam associate table precedes Hash, and disable fallInRange feature */
        /* DsMACHashKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_MAC_HASH_KEY0, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        key_size = alloc_tcam.entry_size;

        if (0 != max_index)
        {
            hash_lkp_ctl.mac_da_bits_num = _sys_humber_get_hashlkpctl_bitnum(DS_MAC_HASH_KEY0, max_index, key_size);
            hash_lkp_ctl.mac_sa_bits_num = hash_lkp_ctl.mac_da_bits_num;
            index_base = alloc_tcam.index_base;
            hash_lkp_ctl.mac_da_table_base = ((index_base - TB_INFO_HASH_MEM_OFFSET) / 16) >> 10;
            hash_lkp_ctl.mac_sa_table_base = ((index_base - TB_INFO_HASH_MEM_OFFSET) / 16) >> 10;
        }
        else
        {
            hash_lkp_ctl.mac_da_bits_num = 0;
            hash_lkp_ctl.mac_sa_bits_num = 0;
            hash_lkp_ctl.mac_da_table_base = 0;
            hash_lkp_ctl.mac_sa_table_base = 0;
        }

        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_MAC_KEY, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        hash_lkp_rslt_ctl.mac_da_lookup_table_base_pos = 1;
        hash_lkp_rslt_ctl.mac_da_lookup_table_base = max_index >> 8;
        hash_lkp_rslt_ctl.mac_da_hash_result_value = 0xFFF;
        hash_lkp_rslt_ctl.mac_da_hash_result_mask = 0xFFF;
        hash_lkp_rslt_ctl.mac_sa_lookup_table_base_pos = 1;
        hash_lkp_rslt_ctl.mac_sa_lookup_table_base = max_index >> 8;
        hash_lkp_rslt_ctl.mac_sa_hash_result_value = 0xFFF;
        hash_lkp_rslt_ctl.mac_sa_hash_result_mask = 0xFFF;


        /* DsIpv4UcastHashKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_IPV4_UCAST_HASH_KEY0, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        key_size = alloc_tcam.entry_size;

        if (0 != max_index)
        {
            hash_lkp_ctl.ipv4_ucast_bits_num = _sys_humber_get_hashlkpctl_bitnum(DS_IPV4_UCAST_HASH_KEY0, max_index, key_size);
            index_base = alloc_tcam.index_base;
            hash_lkp_ctl.ipv4_ucast_table_base = ((index_base - TB_INFO_HASH_MEM_OFFSET) / 16) >> 10;
        }
        else
        {
            hash_lkp_ctl.ipv4_ucast_bits_num= 0;
            hash_lkp_ctl.ipv4_ucast_table_base = 0;
        }

        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_IPV4_UCAST_ROUTE_KEY, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        hash_lkp_rslt_ctl.ipv4_ucast_lookup_table_base_pos  = 1;
        hash_lkp_rslt_ctl.ipv4_ucast_lookup_table_base      = max_index >> 8;
        hash_lkp_rslt_ctl.ipv4_ucast_hash_result_value      = 0xFFF;
        hash_lkp_rslt_ctl.ipv4_ucast_hash_result_mask       = 0xFFF;

        /* DsIpv6UcastHashKey */
        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_IPV6_UCAST_HASH_KEY0, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        key_size = alloc_tcam.entry_size;

        if (0 != max_index)
        {
            hash_lkp_ctl.ipv6_ucast_bits_num = _sys_humber_get_hashlkpctl_bitnum(DS_IPV6_UCAST_HASH_KEY0, max_index, key_size);
            index_base = alloc_tcam.index_base;
            hash_lkp_ctl.ipv6_ucast_table_base = ((index_base - TB_INFO_HASH_MEM_OFFSET) / 16) >> 10;
        }
        else
        {
            hash_lkp_ctl.ipv6_ucast_bits_num= 0;
            hash_lkp_ctl.ipv6_ucast_table_base = 0;
        }

        kal_memset(&alloc_tcam, 0, sizeof(sys_alloc_allocated_tcam_info_t));
        sys_alloc_get_tcam_key_alloc_info(DS_IPV6_UCAST_ROUTE_KEY, &alloc_tcam);
        max_index = alloc_tcam.max_index;
        hash_lkp_rslt_ctl.ipv6_ucast_lookup_table_base_pos  = 1;
        hash_lkp_rslt_ctl.ipv6_ucast_lookup_table_base      = max_index >> 8;
        hash_lkp_rslt_ctl.ipv6_ucast_hash_result_value      = 0xFFF;
        hash_lkp_rslt_ctl.ipv6_ucast_hash_result_mask       = 0xFFF;

        cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_lkp_ctl));
        cmd = DRV_IOW(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_lkp_rslt_ctl));

    }

    return CTC_E_NONE;
}

static int32 _sys_humber_mpls_ctl_init(void)
{
    uint8 chip_num = 0;
    uint32 i;
    uint32 cmd;
    ipe_mpls_ctl_t ipe_mpls_ctl;

    chip_num = sys_humber_get_local_chip_num();

    /* configure defautl values for ipe mpls control regs */
    kal_memset(&ipe_mpls_ctl, 0, sizeof(ipe_mpls_ctl_t));
    ipe_mpls_ctl.mpls_ttl_limit = 2;
    ipe_mpls_ctl.mpls_ttl_decrement = 0;
    ipe_mpls_ctl.use_first_label_ttl = 0;
    ipe_mpls_ctl.use_first_label_exp = 1;
    ipe_mpls_ctl.mpls_offset_bytes_shift = 2;
    ipe_mpls_ctl.mpls_ecmp_use_label = 1;
    ipe_mpls_ctl.mpls_ecmp_use_reserve_label = 1;
    ipe_mpls_ctl.mpls_ecmp_full_label = 1;
    ipe_mpls_ctl.mpls_ecmp_use_ip = 1;
    ipe_mpls_ctl.vccv_cw = 1;
    ipe_mpls_ctl.oam_alert_label0 = 13;
    ipe_mpls_ctl.oam_alert_label1 = 13;

    /*for stats*/
    ipe_mpls_ctl.mpls_stats_high_priority = 1;

    cmd = DRV_IOW(IOC_REG, IPE_MPLS_CTL, DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmd, &ipe_mpls_ctl));
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_check_ip_tunnel_filtering_sram_size(uint16 sram_size)
{
    /* ipuc filtering tcam index: 0 - (sram_size-1),
       and (sram_size-1) must be 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF...
       , so can not exist bit-0 below any bit-1 position */
    uint16 max_tcam_index = (sram_size-1);
    uint32 index = 0;
    bool zero_bit_flag = FALSE;

    for (index = 0; index < 12; index++) /* scan from lower bit position */
    {
        if (IS_BIT_SET(max_tcam_index, index)) /* it's bit-1 */
        {
            if (!zero_bit_flag)  /* no bit-0 below the current position */
            {
                continue;
            }
            else                 /* bit-0 locates below the current bit-1 position */
            {
                return CTC_E_VERSION_DISABLE;
            }
        }
        else
        {
            zero_bit_flag = TRUE; /* find bit-0 */
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_setting_ip_tunnel_filtering_idx_range(uint16 sram_size)
{
    /* ipuc filtering tcam index: 0 - (sram_size-1),
       and the priority is higer than hash hit */
    uint32 cmd = 0;
    uint8 chip_num = 0, lchip = 0;
    uint16 max_tcam_index = (sram_size-1);
    uint16 max_tcam_index2 = (sram_size*2-1);
    ipe_lookup_result_ctl_t rslt_ctl;
    ipe_hash_lookup_result_ctl_t hash_lkp_rst_ctl;
    uint32 index_shift = 0;
    uint32 index_base= 0;
    uint32 index_shift2 = 0;
    uint32 index_base2 = 0;


    kal_memset(&rslt_ctl, 0, sizeof(ipe_lookup_result_ctl_t))  ;
    kal_memset(&hash_lkp_rst_ctl, 0, sizeof(ipe_hash_lookup_result_ctl_t))  ;

    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &rslt_ctl));

    cmd = DRV_IOR(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &hash_lkp_rst_ctl));

    index_shift = (rslt_ctl.ip_da_lookup_result_ctl00>>16)&0x3;
    index_base = (rslt_ctl.ip_da_lookup_result_ctl01>>16)&0xFFF;
    index_shift2 = (rslt_ctl.ip_da_lookup_result_ctl20>>16)&0x3;
    index_base2 = (rslt_ctl.ip_da_lookup_result_ctl21>>16)&0xFFF;

    hash_lkp_rst_ctl.ipv4_ucast_hash_result_mask = (0xFFFFF & (~(max_tcam_index<<index_shift)) )>> 8;
    hash_lkp_rst_ctl.ipv4_ucast_hash_result_value = ((max_tcam_index << index_shift)+ (index_base << 8)) >> 8;

    hash_lkp_rst_ctl.ipv6_ucast_hash_result_mask = (0xFFFFF & (~(max_tcam_index2<<index_shift2)) )>> 8;
    hash_lkp_rst_ctl.ipv6_ucast_hash_result_value = ((max_tcam_index2<< index_shift2 ) + (index_base2 << 8) )  >> 8;


    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < chip_num && lchip < MAX_LOCAL_CHIP_NUM; lchip++)
    {
       CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &hash_lkp_rst_ctl));
    }

    return CTC_E_NONE;
}

static int32 _sys_humber_ipuc_ctl_init(void)
{
    uint8 chip_num = 0;
    uint32 i;
    uint32 cmdr,cmdw;
    ipe_route_ctl_t ipe_route_ctl;
    uint32 ip_tunnel_size = 0;

    chip_num = sys_humber_get_local_chip_num();

    /* configure defautl values for ipe mpls control regs */
    kal_memset(&ipe_route_ctl, 0, sizeof(ipe_route_ctl_t));


    cmdr = DRV_IOR(IOC_REG, IPE_ROUTE_CTL, DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_REG, IPE_ROUTE_CTL, DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmdr, &ipe_route_ctl));
        ipe_route_ctl.ip_ttl_limit = 2;
        CTC_ERROR_RETURN(drv_reg_ioctl(i, 0, cmdw, &ipe_route_ctl));
    }

    /* configure hash_lkp_rst_ctl for ip-tunnel TCMA priority */
    CTC_ERROR_RETURN(sys_alloc_get_ip_tunnel_size(&ip_tunnel_size));

    CTC_ERROR_RETURN(_sys_humber_check_ip_tunnel_filtering_sram_size(ip_tunnel_size));
    CTC_ERROR_RETURN(_sys_humber_setting_ip_tunnel_filtering_idx_range(ip_tunnel_size));

    return CTC_E_NONE;
}

static int32
_sys_humber_parser_ethernet_ctl_init( void)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_ethernet_ctl_t eth_ctl;
    epe_l2_tpid_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(epe_l2_tpid_ctl_t));
    kal_memset(&eth_ctl, 0, sizeof(parser_ethernet_ctl_t));

    /*please refer to sys_humber_oam_ctl_init()*/
    eth_ctl.cvlan_tpid = SYS_PAS_ETH_CVLAN_TPID;
    eth_ctl.bvlan_tpid = SYS_PAS_ETH_BVLAN_TPID;
    eth_ctl.svlan_tpid0 = SYS_PAS_ETH_SVLAN_TPID0;
    eth_ctl.svlan_tpid1 = SYS_PAS_ETH_SVLAN_TPID1;
    eth_ctl.svlan_tpid2 = SYS_PAS_ETH_SVLAN_TPID2;
    eth_ctl.svlan_tpid3 = SYS_PAS_ETH_SVLAN_TPID3;
    eth_ctl.i_tag_tpid = SYS_PAS_ETH_ITAG_TPID;
    eth_ctl.max_length_field = SYS_PAS_ETH_MAX_LENGTH;
    eth_ctl.vlan_parsing_num = SYS_PAS_ETH_VLAN_PAS_NUM;

     local_chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &eth_ctl));
    }

    ctl.bvlan_tpid = SYS_PAS_ETH_BVLAN_TPID;
    ctl.i_tag_tpid = SYS_PAS_ETH_ITAG_TPID;
    ctl.cvlan_tpid = SYS_PAS_ETH_CVLAN_TPID;
    ctl.svlan_tpid0 = SYS_PAS_ETH_SVLAN_TPID0;
    ctl.svlan_tpid1 = SYS_PAS_ETH_SVLAN_TPID1;
    ctl.svlan_tpid2 = SYS_PAS_ETH_SVLAN_TPID2;
    ctl.svlan_tpid3 = SYS_PAS_ETH_SVLAN_TPID3;
    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &ctl));
    }



    return CTC_E_NONE;
}

static int32
 _sys_humber_oam_parser_ctl_init(void)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    oam_parser_ether_ctl_t                  oam_parser_ether_ctl;

     kal_memset(&oam_parser_ether_ctl, 0, sizeof(oam_parser_ether_ctl));

       /* oam_parser_ether_ctl */
        oam_parser_ether_ctl.max_mep_id = 8191;
        oam_parser_ether_ctl.cfm_pdu_mac_da_md_lvl_check_en = 1;
        oam_parser_ether_ctl.tlv_length_check_en            = 1;
        oam_parser_ether_ctl.not_check_bip                  = 0;
        oam_parser_ether_ctl.max_label_num                  = 3;
        oam_parser_ether_ctl.allow_non_zero_oui             = 1;
        oam_parser_ether_ctl.cfm_pdu_max_length             = 132;
        oam_parser_ether_ctl.first_tlv_offset_chk           = 70;
        oam_parser_ether_ctl.mpls_oam_min_length            = 0;
        oam_parser_ether_ctl.cfm_pdu_min_length             = 0;
        oam_parser_ether_ctl.md_name_length_chk             = 43;
        oam_parser_ether_ctl.ma_id_length_chk               = 48;
        oam_parser_ether_ctl.min_mep_id                     = 1;
        oam_parser_ether_ctl.min_port_status_tlv_value      = 1;
        oam_parser_ether_ctl.max_port_status_tlv_value      = 2;
        oam_parser_ether_ctl.mpls_oam_alert_label0          = 13;
        oam_parser_ether_ctl.ignore_eth_oam_version         = 0;
        oam_parser_ether_ctl.invalid_ccm_interval_check_en  = 1;
        oam_parser_ether_ctl.invalid_ccm_interval           = 0;
        oam_parser_ether_ctl.min_intf_status_tlv_value      = 1;
        oam_parser_ether_ctl.max_intf_status_tlv_value      = 7;
        oam_parser_ether_ctl.mpls_oam_alert_label1          = 13;
        oam_parser_ether_ctl.not_check_bip                  = 1;

        /*please refer to _sys_humber_parser_ethernet_ctl_init()*/
        oam_parser_ether_ctl.svlan_tpid0                    = SYS_PAS_ETH_SVLAN_TPID0;
        oam_parser_ether_ctl.svlan_tpid1                    = SYS_PAS_ETH_SVLAN_TPID1;
        oam_parser_ether_ctl.svlan_tpid2                    = SYS_PAS_ETH_SVLAN_TPID2;
        oam_parser_ether_ctl.svlan_tpid3                    = SYS_PAS_ETH_SVLAN_TPID3;
        oam_parser_ether_ctl.cvlan_tpid                     = SYS_PAS_ETH_CVLAN_TPID;
        oam_parser_ether_ctl.bvlan_tpid                     = SYS_PAS_ETH_BVLAN_TPID;

        oam_parser_ether_ctl.max_length_field               = 1536;
        local_chip_num = sys_humber_get_local_chip_num();
        cmd = DRV_IOW(IOC_REG, OAM_PARSER_ETHER_CTL, DRV_ENTRY_FLAG);
        for (lchip = 0; lchip < local_chip_num; lchip++)
       {
           CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &oam_parser_ether_ctl));
       }
        return CTC_E_NONE;
}

static int32
_sys_humber_quadmac_app_pause_frame_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 table_id;
    uint8 qmac;
    quadmacapp0_quad_mac_app_pause_frame_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(quadmacapp0_quad_mac_app_pause_frame_ctl_t));

    ctl.pause_off_enable = 0xF;
    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(qmac=0; qmac<DRV_MAX_QMAC_NUM; qmac++)
        {
            if(drv_humber_qmac_is_enable(qmac))
            {
                if(qmac == 0)
                {
                    table_id = QUADMACAPP0_QUAD_MAC_APP_PAUSE_FRAME_CTL;
                }
                else if((qmac == 10)||(qmac == 11))
                {
                    table_id = QUADMACAPP0_QUAD_MAC_APP_PAUSE_FRAME_CTL+(qmac-9)*25;
                }
                else
                {
                    table_id = QUADMACAPP1_QUAD_MAC_APP_PAUSE_FRAME_CTL+(qmac-1)*25;
                }
                cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ctl));
            }
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_quadmac_app_buffer_store_stall_mask_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 table_id;
    uint8 qmac;
    quadmacapp0_quad_mac_app_pause_frame_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(quadmacapp0_quad_mac_app_pause_frame_ctl_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(qmac=0; qmac<DRV_MAX_QMAC_NUM; qmac++)
        {
            if(drv_humber_qmac_is_enable(qmac))
            {
                if(qmac == 0)
                {
                    table_id = QUADMACAPP0_QUAD_MAC_APP_BUF_STORE_STALL_MASK;
                }
                else if((qmac == 10)||(qmac == 11))
                {
                    table_id = QUADMACAPP0_QUAD_MAC_APP_BUF_STORE_STALL_MASK+(qmac-9)*25;
                }
                else
                {
                    table_id = QUADMACAPP1_QUAD_MAC_APP_BUF_STORE_STALL_MASK+(qmac-1)*25;
                }
                cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ctl));
            }
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_gmac_pause_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 table_id;
    uint8 gmac;
    gmacwrapper_gmac_pause_ctrl_t ctrl;

    ctrl.pause_quanta = 0xFFFF;

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(gmac=0; gmac<DRV_MAX_GMAC_NUM; gmac++)
        {
            if(drv_humber_gmac_is_enable(gmac))
            {
                table_id = GMAC0_GMACWRAPPER_GMAC_PAUSE_CTRL+20*gmac;
                cmd = DRV_IOW(IOC_REG, table_id, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ctrl));
            }
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_xgmac_pause_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint8 xgmac;
    uint32 pause_quanta = 0xFFFF;

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(xgmac=0; xgmac<DRV_MAX_XGMAC_NUM; xgmac++)
        {
            if(drv_humber_xgmac_is_enable(xgmac))
            {
                table_id = XGMAC0_XGMAC_CONFIG1+36*xgmac;
                field_id = XGMAC0_XGMAC_CONFIG1_PAUSE_QUANTA_CFG /*+ 123*xgmac*/;
                cmd = DRV_IOW(IOC_REG, table_id, field_id);
                CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &pause_quanta));
            }
        }
    }

    return CTC_E_NONE;
}



static int32
_sys_humber_sgmac_pause_ctl_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint32 cmd;
    uint32 table_id;
    uint32 field_id;
    uint8 sgmac;
    uint32 pause_quanta = 0xFFFF;

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(sgmac=0; sgmac<DRV_MAX_SGMAC_NUM; sgmac++)
        {
            if(drv_humber_sgmac_is_enable(sgmac))
            {
                table_id = SGMAC0_SGMAC_CONFIG1+42*sgmac;
                field_id = SGMAC0_SGMAC_CONFIG1_PAUSE_QUANTA_CFG /* + 147*sgmac*/;
                cmd = DRV_IOW(IOC_REG, table_id, field_id);
                CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &pause_quanta));
            }
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_buffer_store_resource_threshold_init()
{
    uint8 lchip_num;
    uint8 chip_id;
    uint16 index;
    uint32 cmd;
    buffer_store_resrc_threshold_t resrc_threshold;

    resrc_threshold.resrc_drop_threshold0 = 0x0400;
    resrc_threshold.resrc_drop_threshold1 = 0x0400;
    resrc_threshold.resrc_drop_threshold2 = 0x0400;
    resrc_threshold.resrc_drop_threshold3 = 0x0400;

    lchip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, BUFFER_STORE_RESRC_THRESHOLD, DRV_ENTRY_FLAG);

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for(index = 0; index < 256; index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &resrc_threshold));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_global_ctl_set(ctc_global_control_type_t type, void* value)
{
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint8 ext_qdr_en = 0;

    CTC_PTR_VALID_CHECK(value);

    lchip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < lchip_num; lchip ++)
    {
        switch(type)
        {
            case CTC_GLOBAL_DISCARD_SAME_MACDASA_PKT:
                tmp = *((uint32 *)value);
                CTC_MAX_VALUE_CHECK(tmp, 1);
                cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_CTL, IPE_INTF_MAPPER_CTL_DISCARD_SAME_MAC_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                break;

            case CTC_GLOBAL_DISCARD_SAME_IPDASA_PKT:
                tmp = *((uint32 *)value);
                CTC_MAX_VALUE_CHECK(tmp, 1);
                cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_CTL, IPE_INTF_MAPPER_CTL_DISCARD_SAME_IP_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                break;

            case CTC_GLOBAL_SERVICE_POLICER_NUM:
                tmp = *((uint32 *)value);
                p_global_control_db->service_policer_num = tmp;
                break;

            case CTC_GLOBAL_SUPPORT_PHB:
                tmp = *((uint32 *)value);
                p_global_control_db->is_phb_support = tmp;

                if(p_global_control_db->is_phb_support == TRUE)
                {
                    if(p_global_control_db->stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
                    {
                        return CTC_E_GLOBAL_CONFIG_CONFLICT;
                    }
                }
                break;

            case CTC_GLOBAL_STATS_MODE:
                tmp = *((uint32 *)value);
                CTC_MAX_VALUE_CHECK(tmp, CTC_GLOBAL_STATS_MODE_MAX-1);
                if(tmp == CTC_GLOBAL_STATS_NO_CONFLICT_MODE ||
                   tmp == CTC_GLOBAL_STATS_OPENFLOW_MODE)
                {
                    if(p_global_control_db->is_phb_support == TRUE)
                    {
                        return CTC_E_GLOBAL_CONFIG_CONFLICT;
                    }

                    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
                    if(ext_qdr_en > 0)
                    {
                        tmp = CTC_GLOBAL_STATS_CONFLICT_MODE;
                    }
                }
                p_global_control_db->stats_mode = tmp;
                break;

            default:
                return CTC_E_INVALID_PARAM;
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_global_ctl_get(ctc_global_control_type_t type, void* value)
{
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 lchip = 0;

    CTC_PTR_VALID_CHECK(value);

    switch(type)
    {
        case CTC_GLOBAL_DISCARD_SAME_MACDASA_PKT:
            cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_CTL, IPE_INTF_MAPPER_CTL_DISCARD_SAME_MAC_ADDR);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            *((uint32 *)value) = tmp;
            break;

        case CTC_GLOBAL_DISCARD_SAME_IPDASA_PKT:
            cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_CTL, IPE_INTF_MAPPER_CTL_DISCARD_SAME_IP_ADDR);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            *((uint32 *)value) = tmp;
            break;

        case CTC_GLOBAL_SERVICE_POLICER_NUM:
            *((uint32 *)value) = p_global_control_db->service_policer_num;
            break;

        case CTC_GLOBAL_SUPPORT_PHB:
            *((uint32 *)value) = p_global_control_db->is_phb_support;
            break;

        case CTC_GLOBAL_STATS_MODE:
            *((uint32 *)value) = p_global_control_db->stats_mode;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_class_dscp_map_tbl_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ipe_classification_dscp_map_table_t dscp_map_tbl;

    kal_memset(&dscp_map_tbl, 0, sizeof(dscp_map_tbl));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_DSCP_MAP_TABLE, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < IPE_CLASSIFICATION_DSCP_MAP_TABLE_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &dscp_map_tbl));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_class_prec_map_tbl_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ipe_classification_precedence_map_table_t prec_map_tbl;

    kal_memset(&prec_map_tbl, 0, sizeof(prec_map_tbl));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &prec_map_tbl));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ds_link_agg_block_mask_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ds_link_agg_block_mask_t trunk_mask;

    kal_memset(&trunk_mask, 0, sizeof(ds_link_agg_block_mask_t));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, DS_LINK_AGG_BLOCK_MASK, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < DS_LINK_AGG_BLOCK_MASK_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &trunk_mask));
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_ds_link_agg_bitmap_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ds_link_agg_bitmap_t bitmap;

    kal_memset(&bitmap, 0, sizeof(bitmap));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, DS_LINK_AGG_BITMAP, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < DS_LINK_AGG_BITMAP_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &bitmap));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ds_queue_drop_profile_id_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ds_queue_drop_profile_id_t drop_profile_id;

    kal_memset(&drop_profile_id, 0, sizeof(drop_profile_id));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < DS_QUEUE_DROP_PROFILE_ID_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &drop_profile_id));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ds_eth_mep_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    ds_eth_mep_t eth_mep;

    kal_memset(&eth_mep, 0, sizeof(eth_mep));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, DS_ETH_MEP, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < DS_ETH_MEP_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &eth_mep));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_oam_ds_ma_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    oam_ds_ma_t ds_ma;

    kal_memset(&ds_ma, 0, sizeof(ds_ma));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, OAM_DS_MA, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < OAM_DS_MA_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_ma));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_oam_ds_ma_name_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    oam_ds_ma_name_t ds_ma_name;

    kal_memset(&ds_ma_name, 0, sizeof(ds_ma_name));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, OAM_DS_MA_NAME, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < OAM_DS_MA_NAME_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_ma_name));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_dsq_mgr_egress_resrc_threshold_init()
{
    int32 cmd;
    uint8 lchip = 0, local_chip_num = 0;
    uint32 index = 0;
    dsq_mgr_egress_resrc_threshold_t ds_mgr_thld;

    kal_memset(&ds_mgr_thld, 0, sizeof(ds_mgr_thld));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_TABLE, DSQ_MGR_EGRESS_RESRC_THRESHOLD, DRV_ENTRY_FLAG);

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for(index = 0; index < DSQ_MGR_EGRESS_RESRC_THRESHOLD_MAX_INDEX; index ++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_mgr_thld));
        }
    }

    return CTC_E_NONE;
}

int32 sys_humber_register_init(void)
{
    /* Global control init */
    p_global_control_db = mem_malloc(MEM_SYSTEM_MODULE, sizeof(sys_global_control_db_t));
    if (NULL == p_global_control_db)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_global_control_db, 0, sizeof(sys_global_control_db_t));

    p_global_control_db->is_phb_support = FALSE;
    p_global_control_db->stats_mode = CTC_GLOBAL_STATS_CONFLICT_MODE;

    /*parser & ipe*/

    /*IPE/EPE parser*/
    CTC_ERROR_RETURN(_sys_humber_parser_ethernet_ctl_init());

    /*OAM parser*/
    CTC_ERROR_RETURN(_sys_humber_oam_parser_ctl_init());

    CTC_ERROR_RETURN(_sys_humber_ipe_bridge_ctl_init());

    CTC_ERROR_RETURN(_sys_humber_ipe_router_mac_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_ds_vlan_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_phy_port_mux_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_hdr_adj_ctl());
    CTC_ERROR_RETURN(_sys_humber_ipe_hdr_adj_vlan_ptr_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_hdr_adj_mode_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_intf_map_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_intfmap_max_pkt_length());
    CTC_ERROR_RETURN(_sys_humber_ipe_hdr_adj_exp_map_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_ipg_ctl_init());
    /*epe*/
    CTC_ERROR_RETURN(_sys_humber_epe_hdr_edit_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_epe_pkt_proc_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_epe_l2_ether_type_init());
    CTC_ERROR_RETURN(_sys_humber_epe_hdr_adj_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_epe_nexthop_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_epe_aclqos_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_epe_ipg_ctl_init());

    /* QoS related */
    CTC_ERROR_RETURN(_sys_humber_queue_ctl_config_init());
    CTC_ERROR_RETURN(_sys_humber_queue_tbl_config_init());
    CTC_ERROR_RETURN(_sys_humber_queue_ipg_ctl_init());

    /* TCAM Hash Lookup */
    CTC_ERROR_RETURN(_sys_humber_lkp_ctl_init());

    /* mpls */
    CTC_ERROR_RETURN(_sys_humber_mpls_ctl_init());

    /* ipuc */
    CTC_ERROR_RETURN(_sys_humber_ipuc_ctl_init());

    /*flow control*/
    CTC_ERROR_RETURN(_sys_humber_quadmac_app_pause_frame_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_quadmac_app_buffer_store_stall_mask_init());
    CTC_ERROR_RETURN(_sys_humber_buffer_store_resource_threshold_init());
    CTC_ERROR_RETURN(_sys_humber_gmac_pause_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_xgmac_pause_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_sgmac_pause_ctl_init());

    /*table init*/
    CTC_ERROR_RETURN(_sys_humber_ipe_class_dscp_map_tbl_init());
    CTC_ERROR_RETURN(_sys_humber_ipe_class_prec_map_tbl_init());
    CTC_ERROR_RETURN(_sys_humber_ds_link_agg_block_mask_init());
    CTC_ERROR_RETURN(_sys_humber_ds_link_agg_bitmap_init());
    CTC_ERROR_RETURN(_sys_humber_ds_queue_drop_profile_id_init());
    CTC_ERROR_RETURN(_sys_humber_ds_eth_mep_init());
    CTC_ERROR_RETURN(_sys_humber_oam_ds_ma_init());
    CTC_ERROR_RETURN(_sys_humber_oam_ds_ma_name_init());
    CTC_ERROR_RETURN(_sys_humber_dsq_mgr_egress_resrc_threshold_init());

    return CTC_E_NONE;
}


