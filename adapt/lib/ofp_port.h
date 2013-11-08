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
 * @brief This file define data structure or macros used in adapter layer port
 */

#ifndef __OFP_PORT_H__
#define __OFP_PORT_H__

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define MAX_PHY_PORT_NUM 52
/* must be same with MAX_PORTS in dpif-ctc.c */
#define OFP_MAX_OVS_PORT_NUM MAX_PORTS

#define BASE_PORT_LABEL_ID 1000
#define BASE_TUNNEL_LABEL_ID 1000

#define OFP_PHYSICAL_PORT_NO_BASE 0
#define OFP_PHYSICAL_PORT_NO_MAX  (OFP_PHYSICAL_PORT_NO_BASE + MAX_PHY_PORT_NUM + 1)

#define OFP_INVALID_GPORT       0xffff
#define OFP_INVALID_OFP_PORT    0xffff

/* CPU_PORT_NUMBER must less than MAX_PORTS in dpif-ctc.c */
#define DEFAULT_CPU_PORT_NUMBER (OFP_MAX_OVS_PORT_NUM - 1)

#define OFP_DEFAULT_CPU_GPORT_ID 250

/* valid port number 201~263 */
#define OFP_TUNNEL_PORT_NO_BASE   200
#define OFP_TUNNEL_PORT_NO_NUM    63
#define OFP_TUNNEL_PORT_NO_MAX    (OFP_TUNNEL_PORT_NO_BASE + OFP_TUNNEL_PORT_NO_NUM + 1)
#define OFP_TUNNEL_MAX_LOCAL_IP_NUM 64

#define OFP_TUNNEL_RSV_SERVICE_ID_NUM OFP_TUNNEL_PORT_NO_NUM
#define OFP_TUNNEL_MAX_SERVICE_ID_NUM 150

#define OFP_TUNNEL_SERVICE_ID_MIN 1
#define OFP_TUNNEL_SERVICE_ID_NUM (OFP_TUNNEL_RSV_SERVICE_ID_NUM + OFP_TUNNEL_MAX_SERVICE_ID_NUM)
#define OFP_TUNNEL_SERVICE_ID_MAX (OFP_TUNNEL_SERVICE_ID_MIN + OFP_TUNNEL_SERVICE_ID_NUM)

/* We do not check tunnel mtu currently, so we can support 32bits tunnel id in humber */
#define OFP_MAX_TUNNEL_ID   OFP_UINT32_MAX


#define OFP_MAX_QUEUE_VALUE   8
#define OFP_QUEUE_DISABLE_RATE  1000    /* In 1/10 of a percent; >1000 -> disabled. [spec1.3]*/
#define OFP_LINKAGG_TID_MIN 1
#define OFP_LINKAGG_TID_NUM 128
#define OFP_LINKAGG_TID_MAX (OFP_LINKAGG_TID_MIN + OFP_LINKAGG_TID_NUM - 1)

#define GLB_LOCAL_PORT_LENGTH    8   /*this should be private*/
#define GLB_LOCAL_PORT_MASK      0xFF
#define GLB_TRANS_PORTID_TO_GPORT(chipid, lportid) ((((chipid) & 0x1F) << GLB_LOCAL_PORT_LENGTH) + (lportid))
#define GLB_GET_GLOBAL_CHIPID(gportid) (((gportid) >> GLB_LOCAL_PORT_LENGTH) & 0x1F)
#define GLB_GET_LOCAL_PORTID(gport) ((gport) & 0xFF)

#define OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU     15
#define OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_ARP_TO_CPU      13
#define OFP_L3PDU_PER_L3IF_ACTION_INDEX_RSV_DHCP_TO_CPU     14

#define OFP_SVLAN_DFT_TPID_IDX       2

#define OFP_DEFAULT_MAX_JUMBO_FRAME_SIZE 9600
#define OFP_MIN_MTU 68
#define OFP_MAX_MTU 1500

#define OFP_L3IFID_BASE 2
#define OFP_L3IFID_MAX  ( MAX_PHY_PORT_NUM + OFP_L3IFID_BASE - 1)
#define OFP_L3IFID_VID1 1

#define MAX_VLAN_NUM 4094
#define MAX_VLAN_NUM_WITH_RESERVED 4095
#define OFP_DEFAULT_VLAN_ID 4095

#define HYBRID_DEFAULT_VLAN_ID 1

#define OFP_INVALID_VLANID  0xFFFF

#define DEFAULT_BRIDGE_PORT_NUMBER 0
#define DEFAULT_CPU_PORT_INTERFACE_NAME "eth1"
#define DEFAULT_BRIDGE_INTERFACE_NAME "br0"

#define CPU_PORT_NUM 53

#define OFP_MAP_INTF_TYPE_STR(type)                     \
    (type == OFP_INTERFACE_TYPE_INTERNAL ? "Internal" : \
     type == OFP_INTERFACE_TYPE_TAP      ? "Tap"      : \
     type == OFP_INTERFACE_TYPE_PHYSICAL ? "Physical" : \
     type == OFP_INTERFACE_TYPE_GRE      ? "GRE"      : \
     type == OFP_INTERFACE_TYPE_NVGRE    ? "NVGRE"    : \
     type == OFP_INTERFACE_TYPE_SYSTEM   ? "SYSTEM"   : \
     "Unknown")

/**
 *  @brief interface type
 */
enum ofp_interface_type_e
{
    OFP_INTERFACE_TYPE_INTERNAL,    /**< internal interface */
    OFP_INTERFACE_TYPE_TAP,         /**< tap interface */
    OFP_INTERFACE_TYPE_PHYSICAL,    /**< physical interface */
    OFP_INTERFACE_TYPE_GRE,         /**< gre */
    OFP_INTERFACE_TYPE_NVGRE,       /**< nvgre */
    OFP_INTERFACE_TYPE_SYSTEM,      /**< system. e.g br0 */

    OFP_INTERFACE_TYPE_UNKNOWN,     /**< unknown */
    OFP_INTERFACE_TYPE_MAX          /**< interface max */
};
typedef enum ofp_interface_type_e ofp_interface_type_t;

/**
 *  @brief tunnel flag
 */
enum ofp_tunnel_flag_e
{
    OFP_TNL_F_CSUM          =  1 << 0,
    OFP_TNL_F_TOS_INHERIT   =  1 << 1,
    OFP_TNL_F_TTL_INHERIT   =  1 << 2,
    OFP_TNL_F_DF_INHERIT    =  1 << 3,
    OFP_TNL_F_DF_DEFAULT    =  1 << 4,
    OFP_TNL_F_PMTUD         =  1 << 5,
};
typedef enum ofp_tunnel_flag_e ofp_tunnel_flag_t;

/**
 *  @brief port direction
 */
enum ofp_port_dir_e
{
    OFP_PORT_DIR_TX,                /**< tx */
    OFP_PORT_DIR_RX,                /**< rx */

    OFP_PORT_DIR_MAX                /**< max*/
};
typedef enum ofp_port_dir_e ofp_port_dir_t;

/**
 *  @brief port information
 */
struct ofp_port_info_s
{
    ofp_interface_type_t type;          /**< interface type */

    char       name[OFP_IFNAME_SIZE];   /**< port name */
    uint16_ofp ofport;                  /**< OVS port number */
    int32_ofp  ifindex;                 /**< Interface index */

    void* info;                         /**< port attr info */
};
typedef struct ofp_port_info_s ofp_port_info_t;

/**
 *  @brief tunnel information
 */
struct ofp_tunnel_info_s
{
    uint32_ofp flag;            /* bitmap of ofp_tunnel_flag_t */

    uint8_ofp nexthop_mac[OFP_ETH_ADDR_LEN];
    uint32_ofp remote_ip;
    uint32_ofp local_ip;
    uint16_ofp vlan_id;
    char bind_port_name[OFP_IFNAME_SIZE];
};
typedef struct ofp_tunnel_info_s ofp_tunnel_info_t;

#endif /* !__OFP_PORT_H__ */
