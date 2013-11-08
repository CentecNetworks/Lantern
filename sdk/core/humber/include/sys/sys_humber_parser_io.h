#ifndef _SYS_HUMBER_PARSER_IO_H_
#define _SYS_HUMBER_PARSER_IO_H_

#include "kal.h"
#include "ctc_parser.h"
#include "drv_io.h"
#include "drv_humber.h"

extern int32
sys_humber_parser_io_set_parser_pbb_ctl_entry(uint8 chip_id, parser_pbb_ctl_t* parser_pbb_ctl);

extern int32
sys_humber_parser_io_get_parser_pbb_ctl_entry(uint8 chip_id, parser_pbb_ctl_t* parser_pbb_ctl);

extern int32
sys_humber_parser_io_set_parser_packet_type_table_entry(uint8 chip_id, parser_packet_type_table_t* parser_packet_type_table);

extern int32
sys_humber_parser_io_set_parser_ethernet_ctl_entry(uint8 chip_id, parser_ethernet_ctl_t* parser_ethernet_ctl);

extern int32
sys_humber_parser_io_get_parser_ethernet_ctl_entry(uint8 chip_id, parser_ethernet_ctl_t* parser_ethernet_ctl);

extern int32
sys_humber_parser_io_set_cvlan_tpid(uint8 chip_id, uint32 cvlan_tpid);

extern int32
sys_humber_parser_io_get_cvlan_tpid(uint8 chip_id, uint32 *cvlan_tpid);

extern int32
sys_humber_parser_io_set_allow_non_zero_oui(uint8 chip_id, uint32 allow_non_zero_oui);

extern int32
sys_humber_parser_io_get_allow_non_zero_oui(uint8 chip_id, uint32 *allow_non_zero_oui);

extern int32
sys_humber_parser_io_set_max_length_field(uint8 chip_id, uint32 max_length_field);

extern int32
sys_humber_parser_io_get_max_length_field(uint8 chip_id, uint32 *max_length_field);

extern int32
sys_humber_parser_io_set_i_tag_tpid(uint8 chip_id, uint32 i_tag_tpid);

extern int32
sys_humber_parser_io_get_i_tag_tpid(uint8 chip_id, uint32 *i_tag_tpid);

extern int32
sys_humber_parser_io_set_bvlan_tpid(uint8 chip_id, uint32 bvlan_tpid);

extern int32
sys_humber_parser_io_get_bvlan_tpid(uint8 chip_id, uint32 *bvlan_tpid);

extern int32
sys_humber_parser_io_set_parsing_quad_vlan(uint8 chip_id, uint32 parsing_quad_vlan);

extern int32
sys_humber_parser_io_get_parsing_quad_vlan(uint8 chip_id, uint32 *parsing_quad_vlan);

extern int32
sys_humber_parser_io_set_vlan_parsing_num(uint8 chip_id, uint32 vlan_parsing_num);

extern int32
sys_humber_parser_io_get_vlan_parsing_num(uint8 chip_id, uint32 *vlan_parsing_num);

extern int32
sys_humber_parser_io_set_svlan_tpid0(uint8 chip_id, uint32 svlan_tpid0);

extern int32
sys_humber_parser_io_get_svlan_tpid0(uint8 chip_id, uint32 *svlan_tpid0);

extern int32
sys_humber_parser_io_set_svlan_tpid1(uint8 chip_id, uint32 svlan_tpid1);

extern int32
sys_humber_parser_io_get_svlan_tpid1(uint8 chip_id, uint32 *svlan_tpid1);

extern int32
sys_humber_parser_io_set_svlan_tpid2(uint8 chip_id, uint32 svlan_tpid2);

extern int32
sys_humber_parser_io_get_svlan_tpid2(uint8 chip_id, uint32 *svlan_tpid2);

extern int32
sys_humber_parser_io_set_svlan_tpid3(uint8 chip_id, uint32 svlan_tpid3);

extern int32
sys_humber_parser_io_get_svlan_tpid3(uint8 chip_id, uint32 *svlan_tpid3);

extern int32
sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(uint8 chip_id, parser_layer2_protocol_cam_valid_t* parser_layer2_protocol_cam_valid);

extern int32
sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(uint8 chip_id, parser_layer2_protocol_cam_valid_t* parser_layer2_protocol_cam_valid);

extern int32
sys_humber_parser_io_set_parser_layer2_protocol_cam_entry(uint8 chip_id, parser_layer2_protocol_cam_t* parser_layer2_protocol_cam);

extern int32
sys_humber_parser_io_get_parser_layer2_protocol_cam_entry(uint8 chip_id, parser_layer2_protocol_cam_t* parser_layer2_protocol_cam);

extern int32
sys_humber_parser_io_set_parser_layer2_flex_ctl_entry(uint8 chip_id, parser_layer2_flex_ctl_t* parser_layer2_flex_ctl);

extern int32
sys_humber_parser_io_get_parser_layer2_flex_ctl_entry(uint8 chip_id, parser_layer2_flex_ctl_t* parser_layer2_flex_ctl);

extern int32
sys_humber_parser_io_set_parser_mpls_ctl_entry(uint8 chip_id, parser_mpls_ctl_t* parser_mpls_ctl);

extern int32
sys_humber_parser_io_get_parser_mpls_ctl_entry(uint8 chip_id, parser_mpls_ctl_t* parser_mpls_ctl);

extern int32
sys_humber_parser_io_set_parser_ip_hash_ctl_entry(uint8 chip_id, parser_ip_hash_ctl_t* parser_ip_hash_ctl);

extern int32
sys_humber_parser_io_get_parser_ip_hash_ctl_entry(uint8 chip_id, parser_ip_hash_ctl_t* parser_ip_hash_ctl);

extern int32
sys_humber_parser_io_set_parser_ipv6_ctl_entry(uint8 chip_id, parser_ipv6_ctl_t* parser_ipv6_ctl);

extern int32
sys_humber_parser_io_get_parser_ipv6_ctl_entry(uint8 chip_id, parser_ipv6_ctl_t* parser_ipv6_ctl);

extern int32
sys_humber_parser_io_set_parser_layer3_flex_ctl_entry(uint8 chip_id, parser_layer3_flex_ctl_t* parser_layer3_flex_ctl);

extern int32
sys_humber_parser_io_get_parser_layer3_flex_ctl_entry(uint8 chip_id, parser_layer3_flex_ctl_t* parser_layer3_flex_ctl);

extern int32
sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(uint8 chip_id, parser_layer3_protocol_cam_valid_t* parser_layer3_protocol_cam_valid);

extern int32
sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(uint8 chip_id, parser_layer3_protocol_cam_valid_t* parser_layer3_protocol_cam_valid);

extern int32
sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(uint8 chip_id, parser_layer3_protocol_cam_t* parser_layer3_protocol_cam);

extern int32
sys_humber_parser_io_get_parser_layer3_protocol_cam_entry(uint8 chip_id, parser_layer3_protocol_cam_t* parser_layer3_protocol_cam);

extern int32
sys_humber_parser_io_set_parser_application_cam_entry(uint8 chip_id, parser_application_cam_t* parser_application_cam);

extern int32
sys_humber_parser_io_get_parser_application_cam_entry(uint8 chip_id, parser_application_cam_t* parser_application_cam);

extern int32
sys_humber_parser_io_set_parser_l4_hash_ctl_entry(uint8 chip_id, parser_l4_hash_ctl_t* parser_l4_hash_ctl);

extern int32
sys_humber_parser_io_get_parser_l4_hash_ctl_entry(uint8 chip_id, parser_l4_hash_ctl_t* parser_l4_hash_ctl);

extern int32
sys_humber_parser_io_set_parser_layer4_flag_op_ctl_entry(uint8 chip_id, parser_layer4_flag_op_ctl_t* parser_layer4_flag_op_ctl);

extern int32
sys_humber_parser_io_get_parser_layer4_flag_op_ctl_entry(uint8 chip_id, parser_layer4_flag_op_ctl_t* parser_layer4_flag_op_ctl);

extern int32
sys_humber_parser_io_set_parser_layer4_port_op_sel_entry(uint8 chip_id, parser_layer4_port_op_sel_t* parser_layer4_port_op_sel);

extern int32
sys_humber_parser_io_get_parser_layer4_port_op_sel_entry(uint8 chip_id, parser_layer4_port_op_sel_t* parser_layer4_port_op_sel);

extern int32
sys_humber_parser_io_set_parser_layer4_port_op_ctl_entry(uint8 chip_id, parser_layer4_port_op_ctl_t* parser_layer4_port_op_ctl);

extern int32
sys_humber_parser_io_get_parser_layer4_port_op_ctl_entry(uint8 chip_id, parser_layer4_port_op_ctl_t* parser_layer4_port_op_ctl);

extern int32
sys_humber_parser_io_set_parser_udp_app_op_ctl_entry(uint8 chip_id, parser_udp_app_op_ctl_t* parser_udp_app_op_ctl);

extern int32
sys_humber_parser_io_get_parser_udp_app_op_ctl_entry(uint8 chip_id, parser_udp_app_op_ctl_t* parser_udp_app_op_ctl);

extern int32
sys_humber_parser_io_set_parser_layer4_length_op_ctl_entry(uint8 chip_id, parser_layer4_length_op_ctl_t* parser_layer4_length_op_ctl);

extern int32
sys_humber_parser_io_get_parser_layer4_length_op_ctl_entry(uint8 chip_id, parser_layer4_length_op_ctl_t* parser_layer4_length_op_ctl);

extern int32
sys_humber_parser_io_set_parser_layer4_flex_ctl_entry(uint8 chip_id, parser_layer4_flex_ctl_t* parser_layer4_flex_ctl);

extern int32
sys_humber_parser_io_get_parser_layer4_flex_ctl_entry(uint8 chip_id, parser_layer4_flex_ctl_t* parser_layer4_flex_ctl);

extern int32
sys_humber_parser_io_set_parser_layer4_ptp_ctl_entry(uint8 chip_id, parser_layer4_ptp_ctl_t* parser_layer4_ptp_ctl);

extern int32
sys_humber_parser_io_get_parser_layer4_ptp_ctl_entry(uint8 chip_id, parser_layer4_ptp_ctl_t* parser_layer4_ptp_ctl);

#endif


