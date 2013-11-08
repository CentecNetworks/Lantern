#include "ctc_error.h"
#include "sys_humber_parser.h"
#include "drv_io.h"
#include "drv_humber.h"


/**
@brief The function is to set PARSER_PBB_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_pbb_ctl_entry(uint8 chip_id, parser_pbb_ctl_t* parser_pbb_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_PBB_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_pbb_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_PBB_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_pbb_ctl_entry(uint8 chip_id, parser_pbb_ctl_t* parser_pbb_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_PBB_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_pbb_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_PACKET_TYPE_TABLE whole entry
*/
int32
sys_humber_parser_io_set_parser_packet_type_table_entry(uint8 chip_id, parser_packet_type_table_t* parser_packet_type_table)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_PACKET_TYPE_TABLE, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_packet_type_table));
	return CTC_E_NONE;
}

 /**
@brief The function is to set PARSER_ETHERNET_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_ethernet_ctl_entry(uint8 chip_id, parser_ethernet_ctl_t* parser_ethernet_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ethernet_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_ETHERNET_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_ethernet_ctl_entry(uint8 chip_id, parser_ethernet_ctl_t* parser_ethernet_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ethernet_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set cvlan_tpid
*/
int32
sys_humber_parser_io_set_cvlan_tpid(uint8 chip_id, uint32 cvlan_tpid)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_CVLAN_TPID);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &cvlan_tpid));


     cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_CVLAN_TPID);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &cvlan_tpid));

	return CTC_E_NONE;
}

/**
@brief The function is to get cvlan_tpid
*/
int32
sys_humber_parser_io_get_cvlan_tpid(uint8 chip_id, uint32 *cvlan_tpid)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_CVLAN_TPID);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, cvlan_tpid));
	return CTC_E_NONE;
}

/**
@brief The function is to set allow_non_zero_oui
*/
int32
sys_humber_parser_io_set_allow_non_zero_oui(uint8 chip_id, uint32 allow_non_zero_oui)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_ALLOW_NON_ZERO_OUI);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &allow_non_zero_oui));
	return CTC_E_NONE;
}


/**
@brief The function is to get allow_non_zero_oui
*/
int32
sys_humber_parser_io_get_allow_non_zero_oui(uint8 chip_id, uint32 *allow_non_zero_oui)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_ALLOW_NON_ZERO_OUI);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, allow_non_zero_oui));
	return CTC_E_NONE;
}

/**
@brief The function is to set max_length_field
*/
int32
sys_humber_parser_io_set_max_length_field(uint8 chip_id, uint32 max_length_field)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_MAX_LENGTH_FIELD);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &max_length_field));
	return CTC_E_NONE;
}

/**
@brief The function is to get max_length_field
*/
int32
sys_humber_parser_io_get_max_length_field(uint8 chip_id, uint32 *max_length_field)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_MAX_LENGTH_FIELD);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, max_length_field));
	return CTC_E_NONE;
}

/**
@brief The function is to set i_tag_tpid
*/
int32
sys_humber_parser_io_set_i_tag_tpid(uint8 chip_id, uint32 i_tag_tpid)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_I_TAG_TPID);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &i_tag_tpid));

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_I_TAG_TPID);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &i_tag_tpid));
    return CTC_E_NONE;
}

/**
@brief The function is to get i_tag_tpid
*/
int32
sys_humber_parser_io_get_i_tag_tpid(uint8 chip_id, uint32 *i_tag_tpid)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_I_TAG_TPID);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, i_tag_tpid));
	return CTC_E_NONE;
}

/**
@brief The function is to set bvlan_tpid
*/
int32
sys_humber_parser_io_set_bvlan_tpid(uint8 chip_id, uint32 bvlan_tpid)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_BVLAN_TPID);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bvlan_tpid));

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_BVLAN_TPID);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bvlan_tpid));

    return CTC_E_NONE;
}

/**
@brief The function is to get bvlan_tpid
*/
int32
sys_humber_parser_io_get_bvlan_tpid(uint8 chip_id, uint32 *bvlan_tpid)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_BVLAN_TPID);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, bvlan_tpid));
	return CTC_E_NONE;
}

/**
@brief The function is to set parsing_quad_vlan
*/
int32
sys_humber_parser_io_set_parsing_quad_vlan(uint8 chip_id, uint32 parsing_quad_vlan)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_PARSING_QUAD_VLAN);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &parsing_quad_vlan));
	return CTC_E_NONE;
}

/**
@brief The function is to get parsing_quad_vlan
*/
int32
sys_humber_parser_io_get_parsing_quad_vlan(uint8 chip_id, uint32 *parsing_quad_vlan)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_PARSING_QUAD_VLAN);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parsing_quad_vlan));
	return CTC_E_NONE;
}

/**
@brief The function is to set vlan_parsing_num
*/
int32
sys_humber_parser_io_set_vlan_parsing_num(uint8 chip_id, uint32 vlan_parsing_num)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_VLAN_PARSING_NUM);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &vlan_parsing_num));
	return CTC_E_NONE;
}

/**
@brief The function is to get vlan_parsing_num
*/
int32
sys_humber_parser_io_get_vlan_parsing_num(uint8 chip_id, uint32 *vlan_parsing_num)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_VLAN_PARSING_NUM);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, vlan_parsing_num));
	return CTC_E_NONE;
}

/**
@brief The function is to set svlan_tpid0
*/
int32
sys_humber_parser_io_set_svlan_tpid0(uint8 chip_id, uint32 svlan_tpid0)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID0);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid0));

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_SVLAN_TPID0);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid0));
    return CTC_E_NONE;
}

/**
@brief The function is to get svlan_tpid0
*/
int32
sys_humber_parser_io_get_svlan_tpid0(uint8 chip_id, uint32 *svlan_tpid0)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID0);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, svlan_tpid0));
	return CTC_E_NONE;
}

/**
@brief The function is to set svlan_tpid1
*/
int32
sys_humber_parser_io_set_svlan_tpid1(uint8 chip_id, uint32 svlan_tpid1)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID1);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid1));

   cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_SVLAN_TPID1);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid1));


    return CTC_E_NONE;
}

/**
@brief The function is to get svlan_tpid1
*/
int32
sys_humber_parser_io_get_svlan_tpid1(uint8 chip_id, uint32 *svlan_tpid1)
{
    uint32 cmd = 0;

    cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID1);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, svlan_tpid1));

    return CTC_E_NONE;
}

/**
@brief The function is to set svlan_tpid2
*/
int32
sys_humber_parser_io_set_svlan_tpid2(uint8 chip_id, uint32 svlan_tpid2)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID2);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid2));

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_SVLAN_TPID2);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid2));

    return CTC_E_NONE;
}

/**
@brief The function is to get svlan_tpid2
*/
int32
sys_humber_parser_io_get_svlan_tpid2(uint8 chip_id, uint32 *svlan_tpid2)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID2);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, svlan_tpid2));
	return CTC_E_NONE;
}

/**
@brief The function is to set svlan_tpid3
*/
int32
sys_humber_parser_io_set_svlan_tpid3(uint8 chip_id, uint32 svlan_tpid3)
{
    uint32 cmd = 0;

    cmd = DRV_IOW(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID3);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid3));

    cmd = DRV_IOW(IOC_REG, EPE_L2_TPID_CTL, EPE_L2_TPID_CTL_SVLAN_TPID3);
    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &svlan_tpid3));
    return CTC_E_NONE;
}

/**
@brief The function is to get svlan_tpid3
*/
int32
sys_humber_parser_io_get_svlan_tpid3(uint8 chip_id, uint32 *svlan_tpid3)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_ETHERNET_CTL, PARSER_ETHERNET_CTL_SVLAN_TPID3);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, svlan_tpid3));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER2_PROTOCOL_CAM_VALID whole entry
*/
int32
sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(uint8 chip_id, parser_layer2_protocol_cam_valid_t* parser_layer2_protocol_cam_valid)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM_VALID, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_protocol_cam_valid));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER2_PROTOCOL_CAM_VALID whole entry
*/
int32
sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(uint8 chip_id, parser_layer2_protocol_cam_valid_t* parser_layer2_protocol_cam_valid)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM_VALID, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_protocol_cam_valid));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER2_PROTOCOL_CAM whole entry
*/
int32
sys_humber_parser_io_set_parser_layer2_protocol_cam_entry(uint8 chip_id, parser_layer2_protocol_cam_t* parser_layer2_protocol_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_protocol_cam));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER2_PROTOCOL_CAM whole entry
*/
int32
sys_humber_parser_io_get_parser_layer2_protocol_cam_entry(uint8 chip_id, parser_layer2_protocol_cam_t* parser_layer2_protocol_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_protocol_cam));
	return CTC_E_NONE;
}
/**
@brief The function is to set PARSER_LAYER2_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer2_flex_ctl_entry(uint8 chip_id, parser_layer2_flex_ctl_t* parser_layer2_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER2_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER2_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer2_flex_ctl_entry(uint8 chip_id, parser_layer2_flex_ctl_t* parser_layer2_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer2_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_MPLS_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_mpls_ctl_entry(uint8 chip_id, parser_mpls_ctl_t* parser_mpls_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_MPLS_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_mpls_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_MPLS_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_mpls_ctl_entry(uint8 chip_id, parser_mpls_ctl_t* parser_mpls_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_MPLS_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_mpls_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_IP_HASH_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_ip_hash_ctl_entry(uint8 chip_id, parser_ip_hash_ctl_t* parser_ip_hash_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_IP_HASH_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ip_hash_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_IP_HASH_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_ip_hash_ctl_entry(uint8 chip_id, parser_ip_hash_ctl_t* parser_ip_hash_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_IP_HASH_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ip_hash_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_IPV6_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_ipv6_ctl_entry(uint8 chip_id, parser_ipv6_ctl_t* parser_ipv6_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_IPV6_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ipv6_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_IPV6_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_ipv6_ctl_entry(uint8 chip_id, parser_ipv6_ctl_t* parser_ipv6_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_IPV6_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_ipv6_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER3_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer3_flex_ctl_entry(uint8 chip_id, parser_layer3_flex_ctl_t* parser_layer3_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER3_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER3_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer3_flex_ctl_entry(uint8 chip_id, parser_layer3_flex_ctl_t* parser_layer3_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER3_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER3_PROTOCOL_CAM_VALID whole entry
*/
int32
sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(uint8 chip_id, parser_layer3_protocol_cam_valid_t* parser_layer3_protocol_cam_valid)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER3_PROTOCOL_CAM_VALID, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_protocol_cam_valid));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER3_PROTOCOL_CAM_VALID whole entry
*/
int32
sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(uint8 chip_id, parser_layer3_protocol_cam_valid_t* parser_layer3_protocol_cam_valid)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER3_PROTOCOL_CAM_VALID, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_protocol_cam_valid));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER3_PROTOCOL_CAM whole entry
*/
int32
sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(uint8 chip_id, parser_layer3_protocol_cam_t* parser_layer3_protocol_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER3_PROTOCOL_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_protocol_cam));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER3_PROTOCOL_CAM whole entry
*/
int32
sys_humber_parser_io_get_parser_layer3_protocol_cam_entry(uint8 chip_id, parser_layer3_protocol_cam_t* parser_layer3_protocol_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER3_PROTOCOL_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer3_protocol_cam));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_APPLICATION_CAM whole entry
*/
int32
sys_humber_parser_io_set_parser_application_cam_entry(uint8 chip_id, parser_application_cam_t* parser_application_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_APPLICATION_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_application_cam));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_APPLICATION_CAM whole entry
*/
int32
sys_humber_parser_io_get_parser_application_cam_entry(uint8 chip_id, parser_application_cam_t* parser_application_cam)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_APPLICATION_CAM, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_application_cam));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_L4_HASH_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_l4_hash_ctl_entry(uint8 chip_id, parser_l4_hash_ctl_t* parser_l4_hash_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_L4_HASH_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_l4_hash_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_L4_HASH_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_l4_hash_ctl_entry(uint8 chip_id, parser_l4_hash_ctl_t* parser_l4_hash_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_L4_HASH_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_l4_hash_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_FLAG_OP_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer4_flag_op_ctl_entry(uint8 chip_id, parser_layer4_flag_op_ctl_t* parser_layer4_flag_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_FLAG_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_flag_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_FLAG_OP_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_flag_op_ctl_entry(uint8 chip_id, parser_layer4_flag_op_ctl_t* parser_layer4_flag_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_FLAG_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_flag_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_PORT_OP_SEL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer4_port_op_sel_entry(uint8 chip_id, parser_layer4_port_op_sel_t* parser_layer4_port_op_sel)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_PORT_OP_SEL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_port_op_sel));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_PORT_OP_SEL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_port_op_sel_entry(uint8 chip_id, parser_layer4_port_op_sel_t* parser_layer4_port_op_sel)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_PORT_OP_SEL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_port_op_sel));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_PORT_OP_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer4_port_op_ctl_entry(uint8 chip_id, parser_layer4_port_op_ctl_t* parser_layer4_port_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_PORT_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_port_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_PORT_OP_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_port_op_ctl_entry(uint8 chip_id, parser_layer4_port_op_ctl_t* parser_layer4_port_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_PORT_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_port_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_UDP_APP_OP_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_udp_app_op_ctl_entry(uint8 chip_id, parser_udp_app_op_ctl_t* parser_udp_app_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_UDP_APP_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_udp_app_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_UDP_APP_OP_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_udp_app_op_ctl_entry(uint8 chip_id, parser_udp_app_op_ctl_t* parser_udp_app_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_UDP_APP_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_udp_app_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_LENGTH_OP_CTL and IPE_HDR_ADJ_LAYER4_LENGTH_OP_CTL
*/
int32
sys_humber_parser_io_set_parser_layer4_length_op_ctl_entry(uint8 chip_id, parser_layer4_length_op_ctl_t* parser_layer4_length_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_LENGTH_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_length_op_ctl));

    /* here config IPE_HDR_ADJ_LAYER4_LENGTH_OP_CTL as same as PARSER_LAYER4_LENGTH_OP_CTL */
    cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_LAYER4_LENGTH_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_length_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_LENGTH_OP_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_length_op_ctl_entry(uint8 chip_id, parser_layer4_length_op_ctl_t* parser_layer4_length_op_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_LENGTH_OP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_length_op_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer4_flex_ctl_entry(uint8 chip_id, parser_layer4_flex_ctl_t* parser_layer4_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_FLEX_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_flex_ctl_entry(uint8 chip_id, parser_layer4_flex_ctl_t* parser_layer4_flex_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_FLEX_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_flex_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to set PARSER_LAYER4_PTP_CTL whole entry
*/
int32
sys_humber_parser_io_set_parser_layer4_ptp_ctl_entry(uint8 chip_id, parser_layer4_ptp_ctl_t* parser_layer4_ptp_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOW(IOC_REG, PARSER_LAYER4_PTP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_ptp_ctl));
	return CTC_E_NONE;
}

/**
@brief The function is to get PARSER_LAYER4_PTP_CTL whole entry
*/
int32
sys_humber_parser_io_get_parser_layer4_ptp_ctl_entry(uint8 chip_id, parser_layer4_ptp_ctl_t* parser_layer4_ptp_ctl)
{
	uint32 cmd = 0;

	cmd = DRV_IOR(IOC_REG, PARSER_LAYER4_PTP_CTL, DRV_ENTRY_FLAG);
	CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, parser_layer4_ptp_ctl));
	return CTC_E_NONE;
}
