/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file define data structure for process packet from/to chip
 */

#ifndef __OFP_PACKET_H__
#define __OFP_PACKET_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/


/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

/*Should large than max frame size*/
#define OFP_CPU_PKT_BUF_LEN            9800
#define OFP_CPU_PKT_HDR_LEN            32
#define OFP_CPU_LOCAL_PHYPORT          250
#define OFP_CPU_BRG_NH_PTR             (0xFFFF << 2 | 0)
#define OFP_CPU_QOS_HIGHEST_PRIORITY   63
#define OFP_CPU_QOS_INVALID_FLOWID     0xFF
#define OFP_CPU_QOS_FLOWID_SHIFT       16
#define OFP_CPU_BRGHDR_TYPE_HUMBER     1
#define OFP_CPU_BRGHDR_CRC_POS         15
#define OFP_CPU_BRGHDR_RESV_BYTES      2
#define OFP_ETH_HDR_LEN                 14
#define OFP_SKB_RESERVE                 64
#define OFP_HUMBER_HDR_LEN              32
#define OFP_PORT_MINIMUM_FRAME_SIZE     33
#define OFP_BRGHDR_LEN                  (OFP_ETH_HDR_LEN + OFP_HUMBER_HDR_LEN + OFP_CPU_BRGHDR_RESV_BYTES)
#define OFP_FLOOD_STP_ID 1

#define OFP_CPU_ETH_P_ASIC             0x5A5A


#define BRIDGE_NEXTHOP_PTR              0x3FFFC
#define BYPASS_NEXTHOP_PTR              0x3FFFD
#define MIRROR_NEXTHOP_PTR              0x3FFFE
#define UNTAG_NEXTHOP_PTR               0x3FFFF

/**
 *@brief CPU process status
 */
enum ofp_cpu_process_e
{
    OFP_CPU_PROCESS_DISCARD,               /**< Discard packet, ie. disable CPU and chip process */
    OFP_CPU_PROCESS_ENABLE,                /**< Enable CPU process */
    OFP_CPU_PROCESS_DISABLE,               /**< Disable CPU process, chip process is enabled */

    OFP_CPU_PROCESS_MAX                    /**< cpu process max */
};
typedef enum ofp_cpu_process_e ofp_cpu_process_t;

/**
  *@brief packet in reason
  */
enum ofp_packet_to_cpu_reason_e
{
    PACKET_TO_CPU_REASON_MISS_MATCH,       /**< Miss match */
    PACKET_TO_CPU_REASON_SW_PROCESS,       /**< Need software help to process some actions */
    PACKET_TO_CPU_REASON_SW_PROCESS_GROUP, /**< Need special handling to group */
    PACKET_TO_CPU_REASON_MAX               /**< Packet_to_cpu reason Max */
};
typedef enum ofp_packet_to_cpu_reason_e ofp_packet_to_cpu_reason_t;

/**
 *@brief packet to cpu information
 */
struct ofp_packet_to_cpu_info_s
{
    uint16_ofp in_port;                           /**< packet to cpu inport */
    uint16_ofp rsv;                               /**< reserved */
    uint32_ofp flow_id;                           /**< to_cpu matched flow id */
    ofp_packet_to_cpu_reason_t packet_in_reason;  /**< packet to cpu reason */
    struct rule_ctc* p_rule;                      /**< rule */
};
typedef struct ofp_packet_to_cpu_info_s ofp_packet_to_cpu_info_t;

enum ctc_swap_direction
{
    HOST_TO_NETWORK,
    NETWORK_TO_HOST
};

enum ofp_nexthop_type_e
{
    OFP_NEXTHOP_TYPE_BYPASS = 0,
    OFP_NEXTHOP_TYPE_BRIDGE,
    OFP_NEXTHOP_TYPE_MIRROR,
    OFP_NEXTHOP_TYPE_UNTAG,
    OFP_NEXTHOP_TYPE_OFFSET
};
typedef enum ofp_nexthop_type_e ofp_nexthop_type_t;


struct ofp_sk_buff_s
{
        uint8_ofp * head;
        uint8_ofp * data;
        uint8_ofp * tail;
        uint8_ofp * end;
        uint32_ofp len;
};
typedef struct ofp_sk_buff_s ofp_sk_buff_t;

/**
 * @brief bridge header info
 */
#if HOST_IS_LE
struct ofp_brghdr_info_s
{
    uint32_ofp dest_id :16;
    uint32_ofp dest_chip_id :5;
    uint32_ofp multi_cast :1;
    uint32_ofp pkt_offset :7;
    uint32_ofp dest_id_discard :1;
    uint32_ofp hdr_type :2;

    uint32_ofp src_port :14;
    uint32_ofp untag_pkt :1;
    uint32_ofp vpls_port_type :1;
    uint32_ofp hd_hash_2_to_0 :3;
    uint32_ofp src_queue_select :1;
    uint32_ofp src_cos :3;
    uint32_ofp pkt_type :3;
    uint32_ofp priority :6;

    uint32_ofp nxt_hop_ptr :18;
    uint32_ofp color :2;
    uint32_ofp src_vid :12;

    uint32_ofp hdr_crc :8;
    uint32_ofp ttl_or_oam_defect :8;
    uint32_ofp pkt_len :14;
    uint32_ofp critical_pkt :1;
    uint32_ofp len_adj_type :1;

    uint32_ofp src_cvid :12;
    uint32_ofp src_cvid_vld :1;
    uint32_ofp src_svid_vld :1;
    uint32_ofp deny_rplc_dscp :1;
    uint32_ofp deny_rplc_cos :1;
    uint32_ofp svlan_tpid_index :2;
    uint32_ofp nexthop_ptr_19_18 :2;
    uint32_ofp next_hop_ext :1;
    uint32_ofp src_cfi :1;
    uint32_ofp communicate_port :1;
    uint32_ofp pbb_srcport_type_or_l4srcport_vld :3;
    uint32_ofp srcport_isolate_id :6;

    uint32_ofp vrfid_timestamp_63_48 :16;
    uint32_ofp src_vlanptr_or_timestamp_79_64 :16;

    uint32_ofp hd_hash_7_to_3 :5;
    uint32_ofp operation_type :3;
    uint32_ofp mirror :1;
    uint32_ofp oam_tunnel_en :1;
    uint32_ofp pfm :2;
    uint32_ofp use_outer_vrfid :1;
    uint32_ofp src_tagged :1;
    uint32_ofp rsv2 :2;
    uint32_ofp l4srcport_or_vplssrcport_oamtype :16;

    uint32_ofp flowid_servecid_or_oamportid :32;
};

#else
struct ofp_brghdr_info_s
{
    uint32_ofp hdr_type :2;
    uint32_ofp dest_id_discard :1;
    uint32_ofp pkt_offset :7;
    uint32_ofp multi_cast :1;
    uint32_ofp dest_chip_id :5;
    uint32_ofp dest_id :16;

    uint32_ofp priority :6;
    uint32_ofp pkt_type :3;
    uint32_ofp src_cos :3;
    uint32_ofp src_queue_select :1;
    uint32_ofp hd_hash_2_to_0 :3;
    uint32_ofp vpls_port_type :1;
    uint32_ofp untag_pkt :1;
    uint32_ofp src_port :14;

    uint32_ofp src_vid :12;
    uint32_ofp color :2;
    uint32_ofp nxt_hop_ptr :18;

    uint32_ofp len_adj_type :1;
    uint32_ofp critical_pkt :1;
    uint32_ofp pkt_len :14;
    uint32_ofp ttl_or_oam_defect :8;
    uint32_ofp hdr_crc :8;

    uint32_ofp srcport_isolate_id :6;
    uint32_ofp pbb_srcport_type_or_l4srcport_vld :3;
    uint32_ofp communicate_port :1;
    uint32_ofp src_cfi :1;
    uint32_ofp next_hop_ext :1;
    uint32_ofp nexthop_ptr_19_18 :2;
    uint32_ofp svlan_tpid_index :2;
    uint32_ofp deny_rplc_cos :1;
    uint32_ofp deny_rplc_dscp :1;
    uint32_ofp src_svid_vld :1;
    uint32_ofp src_cvid_vld :1;
    uint32_ofp src_cvid :12;

    uint32_ofp src_vlanptr_or_timestamp_79_64 :16;
    uint32_ofp vrfid_timestamp_63_48 :16;

    uint32_ofp l4srcport_or_vplssrcport_oamtype :16;
    uint32_ofp rsv2 :2;
    uint32_ofp src_tagged :1;
    uint32_ofp use_outer_vrfid :1;
    uint32_ofp pfm :2;
    uint32_ofp oam_tunnel_en :1;
    uint32_ofp mirror :1;
    uint32_ofp operation_type :3;
    uint32_ofp hd_hash_7_to_3 :5;

    uint32_ofp flowid_servecid_or_oamportid :32;
};
#endif
typedef struct ofp_brghdr_info_s ofp_brghdr_info_t;

struct ofp_cpu_encap_info_s
{
    ofp_nexthop_type_t nh_type;
    uint32_ofp nh_offset;
    uint32_ofp hash;
    uint16_ofp src_vid;
    uint16_ofp destid;      /* destination local portid or dest mcast group id */
    uint8_ofp  dest_chipid; /* destination chip id */
    uint8_ofp  is_mcast;
    uint8_ofp  ttl;
    uint8_ofp  rsv0;
    uint16_ofp src_port; /* source gport id */
    uint16_ofp  packet_priority;
};
typedef struct ofp_cpu_encap_info_s ofp_cpu_encap_info_t;

struct ofp_cpu_mac_header_s
{
    mac_addr_t cpu_mac_da;        /* destination eth addr */
    mac_addr_t cpu_mac_sa;        /* source ether addr */
    uint16_ofp cpu_hdr_type;          /* packet type ID field */
    uint16_ofp cpu_hdr_reserved;
};
typedef struct ofp_cpu_mac_header_s ofp_cpu_mac_header_t;

/* 16(cpu mac header) + 32(bridge header) + XX(packet) */
struct ofp_cpu_packet_s
{
    ofp_cpu_mac_header_t cpu_mac_header;
    ofp_brghdr_info_t bridge_header;
    uint8_ofp  data[OFP_CPU_PKT_BUF_LEN];
    uint32_ofp data_length;
};
typedef struct ofp_cpu_packet_s ofp_cpu_packet_t;

#endif /* ! */
