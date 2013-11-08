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
 */

#ifndef __GLB_IF_DEFINE_H__
#define __GLB_IF_DEFINE_H__

#define GLB_INVALID_GPORTID 0xFFFF
#define GLB_INVALID_VLANID  0xFFFF
#define GLB_INVALID_IFINDEX 0xFFFF

#define GLB_PORT_NUM_PER_CHIP     64

#define GLB_CPU_LOCAL_PHYPORT    250
#define GLB_VLANIF_MAX_NUM       256
#define GLB_LINKAGG_MAX_GROUP    128  /*128 for humber sdk, and 256 for bay sdk*/
#define GLB_LINKAGG_MAX_MEMBER   16   /*get this from sdk?? humber supports 16, but bay only 8 */
#define GLB_LINKAGG_CHIPID       0x1F /*this should be private*/
#define GLB_LOCAL_PORT_LENGTH    8   /*this should be private*/
#define GLB_LOCAL_PORT_MASK      0xFF
#define GLB_TRANS_PORTID_TO_GPORT(chipid, lportid) ((((chipid) & 0x1F) << GLB_LOCAL_PORT_LENGTH) + (lportid))
#define GLB_GET_GLOBAL_CHIPID(gportid) (((gportid) >> GLB_LOCAL_PORT_LENGTH) & 0x1F)
#define GLB_GET_LOCAL_PORTID(gport) ((gport) & 0xFF)

#define GLB_GB_LOCAL_PORT_LENGTH    9
#define GLB_GB_IS_LINKAGG_PORT(gport)   ((((gport) >> GLB_GB_LOCAL_PORT_LENGTH)& GLB_LINKAGG_CHIPID) == GLB_LINKAGG_CHIPID)
#define GLB_GB_GPORT_TO_HUMBER_GPORT(gport) (((gport&0x3E00)>>1) | (gport&0xFF))

#define GLB_IS_LINKAGG_PORT(gport)   ((((gport) >> GLB_LOCAL_PORT_LENGTH)& GLB_LINKAGG_CHIPID) == GLB_LINKAGG_CHIPID)
#define GLB_TRANS_TID_TO_AGGPORT(tid)   ((tid) | (GLB_LINKAGG_CHIPID << 8))
#define GLB_TRANS_AGGPORT_TO_TID(gport) ((gport) & 0xFF)

#define GLB_INTERNAL_PORT_START      56
#define GLB_INTERNAL_PORT_END      64
#define GLB_PORT_ENABLE_INTERNAL_PORT_MAX      4

/*vid=1~4094, vpls id=4095~6143, the rest 6144~8191 is for vrfid in sdk*/
#define GLB_L3IF_VRFID_BASE  (GLB_MAX_VLAN_ID+1+GLB_VLAN_FID_NUM_VPLS)

typedef uint16_t glb_agg_mem_ports_t[GLB_LINKAGG_MAX_MEMBER];

enum glb_if_route_mac_type_e
{
    GLB_IF_ROUTE_MAC_TYPE0 ,    /**<  type 0*/
    GLB_IF_ROUTE_MAC_TYPE_VRRP ,     /**<  type 1*/
    GLB_IF_ROUTE_MAC_TYPE_RSV, /**< type 2,will be reserved for per-system router mac*/
    GLB_IF_ROUTE_MAC_TYPE_INVALID ,  /**< non-support type*/
};
typedef enum glb_if_route_mac_type_e  glb_if_route_mac_type_t;

/* Port type. */
enum glb_vlan_port_type_e
{
    GLB_VLAN_PORT_TYPE_ACCESS,
    GLB_VLAN_PORT_TYPE_TRUNK,
    GLB_VLAN_PORT_TYPE_QINQ,
    GLB_VLAN_PORT_TYPE_MAX
};
typedef enum glb_vlan_port_type_e glb_vlan_port_type_t;

/*define interface type*/
enum glb_if_type_e
{
    GLB_IF_TYPE_NULL,
    GLB_IF_TYPE_PORT_IF,
    GLB_IF_TYPE_LINKAGG_IF,
    GLB_IF_TYPE_VLAN_IF,
    GLB_IF_TYPE_SUB_IF,
    GLB_IF_TYPE_LOOPBACK_IF, /*only for ifindex judge*/
    GLB_IF_TYPE_INVALID
};
typedef enum glb_if_type_e glb_if_type_t;

enum glb_agg_type_e
{
    GLB_AGG_FLAG_DISABLE_DISTRIBUTOR,
    GLB_AGG_FLAG_DISABLE_COLLECTOR,
    GLB_AGG_FLAG_DISABLE_INVALID
};
typedef enum glb_agg_type_e glb_agg_type_t;

enum glb_if_ip_type_e
{
    GLB_IF_L3_IPV4_UC_EN = (1 << 0),
    GLB_IF_L3_IPV6_UC_EN = (1 << 1),
    GLB_IF_L3_IPV4_MC_EN = (1 << 2),
    GLB_IF_L3_IPV6_MC_EN = (1 << 3),
    GLB_IF_L3_IPV4_UCRPF_EN = (1 << 4),
    GLB_IF_L3_IPV6_UCRPF_EN = (1 << 5),
    GLB_IF_L3_IPV4_MCRPF_EN = (1 << 6),
    GLB_IF_L3_IPV6_MCRPF_EN = (1 << 7),
    GLB_IF_L3_RT_ALL_PKTS   = (1 << 8),
    GLB_IF_L3_DHCP_RELAY_EN = (1 << 9),
    GLB_IF_L3_TYPE_MAX
};
typedef enum glb_if_ip_type_e glb_if_ip_type_t;

/* Port type. */
enum glb_agg_load_balance_e
{
    GLB_AGG_LOAD_BALANCE_DST_MAC,
    GLB_AGG_LOAD_BALANCE_SRC_MAC,
    GLB_AGG_LOAD_BALANCE_SRC_DST_MAC,
    GLB_AGG_LOAD_BALANCE_SRC_IP,
    GLB_AGG_LOAD_BALANCE_DST_IP,
    GLB_AGG_LOAD_BALANCE_SRC_DST_IP,
    GLB_AGG_LOAD_BALANCE_TYPE_MAX
};
typedef enum glb_agg_load_balance_e glb_agg_load_balance_t;

/* support port-isolate*/
#define GLB_INTERFACE_MAX_PORT_ISOLATE_GROUP (0x3F)

enum glb_if_port_isolate_type_e
{
    GLB_IF_P_ISLT_L2,
    GLB_IF_P_ISLT_ALL,
    GLB_IF_P_ISLT_MAX
};
typedef enum glb_if_port_isolate_type_e glb_if_port_isolate_type_t;

#ifdef _GLB_DISTRIBUTE_SYSTEM_
/*for port*/
#define GLB_IF_SYNC_RUNNING                 (1 << 0)
#define GLB_IF_SYNC_AGGREGATED              (1 << 1)
#define GLB_IF_SYNC_ROUTE_EN                (1 << 2)
#define GLB_IF_SYNC_UNTAG_DEF_VID           (1 << 3)
#define GLB_IF_SYNC_UNTAG_SVLAN             (1 << 4)
#define GLB_IF_SYNC_JUMBO_FRAME_EN          (1 << 5)
#define GLB_IF_SYNC_EFM_LINK_MONITOR_ON     (1 << 7)
#endif /*_GLB_DISTRIBUTE_SYSTEM_*/

#define GLB_IF_STATS_LOAD_DEF_INTVAL        300         /* interface stats default load interval is 5min */
enum glb_if_stats_req_type_e {
    GLB_IF_STATS_REQ_IMISH,
    GLB_IF_STATS_REQ_RMON,
    GLB_IF_STATS_REQ_SFLOW,
    GLB_IF_STATS_REQ_EFM,
    GLB_IF_STATS_REQ_SNMP,
    GLB_IF_STATS_REQ_MAX
};
typedef enum glb_if_stats_req_type_e glb_if_stats_req_type_t;

#endif
