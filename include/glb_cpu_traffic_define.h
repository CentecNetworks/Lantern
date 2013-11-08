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

#ifndef __GLB_CPU_TRAFFIC_DEFINE_H__
#define __GLB_CPU_TRAFFIC_DEFINE_H__

#define GLB_CPU_TRAFFIC_MAX_RATE   1000000  /* kbps */
#define GLB_CPU_TRAFFIC_REASON_CLASS_MAX            4
#define GLB_MAX_CPU_REASON_MSG_NUM          32
#define GLB_PDU_L2HDR_PROTO_SMART_LINK  0x0127
#define GLB_PDU_L2HDR_PROTO_L2PING      0x9009
#define GLB_PDU_L2HDR_PROTO_UDLD        0x0111
#define GLB_PDU_L3HDR_PROTO_OSPF   89
#define GLB_PDU_L3HDR_PROTO_PIM    103
#define GLB_PDU_L3HDR_PROTO_VRRP   112
#define GLB_PDU_L3HDR_PROTO_RSVP   46
#define GLB_PDU_L3HDR_PROTO_ICMPv6 58
#define GLB_PDU_L4PORT_RIP              520
#define GLB_PDU_L4PORT_RIPng            521
#define GLB_PDU_L4PORT_BGP              179
#define GLB_PDU_L4PORT_LDP              646
#define GLB_PDU_L4PORT_MPLS_OAM         0x3503
#define GLB_PDU_L4PORT_BFD              3784
#define GLB_NEXTHOP_BYPASS_TO_CPU             0x3fffd

enum glb_packet_tocpu_reason_e
{
    GLB_PACKET_TOCPU_REASON_USERID = 0,
    GLB_PACKET_TOCPU_REASON_PROTO_VLAN,

    /* BRIDGE exception block */
    GLB_PACKET_TOCPU_REASON_BRIDGE_BPDU,
    GLB_PACKET_TOCPU_REASON_BRIDGE_CFM,
    GLB_PACKET_TOCPU_REASON_BRIDGE_SLOW_PROTO,
    GLB_PACKET_TOCPU_REASON_BRIDGE_EAPOL,
    GLB_PACKET_TOCPU_REASON_BRIDGE_ERPS,
    GLB_PACKET_TOCPU_REASON_BRIDGE_SMART_LINK,
    GLB_PACKET_TOCPU_REASON_BRIDGE_FLEX,
    GLB_PACKET_TOCPU_REASON_BRIDGE_UDLD,
    GLB_PACKET_TOCPU_REASON_BRIDGE_DEFAULT,
    GLB_PACKET_TOCPU_REASON_BRIDGE_MAC_COPY_CPU,

    /* ROUTE-IPDA exception block */
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_ARP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_DHCP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_RIP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_LDP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_OSPF,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_PIM,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_VRRP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_RSVP,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_ICMPV6,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_MPLS_OAM,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_BFD,
    GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_DEFAULT,

    GLB_PACKET_TOCPU_REASON_ICMP_REDIRECT,
    GLB_PACKET_TOCPU_REASON_LEARNING_FULL,
    GLB_PACKET_TOCPU_REASON_MCAST_RPF_FAIL,
    GLB_PACKET_TOCPU_REASON_MACSA_DISCARD,
    GLB_PACKET_TOCPU_REASON_MACSA_MISMATCH,
    GLB_PACKET_TOCPU_REASON_PORT_SECURITY_DISCARD,
    GLB_PACKET_TOCPU_REASON_VLAN_SECURITY_DISCARD,

    GLB_PACKET_TOCPU_REASON_MCAST_TTL,
    GLB_PACKET_TOCPU_REASON_MTU_DONTFRAG,
    GLB_PACKET_TOCPU_REASON_MTU_FRAG,
    GLB_PACKET_TOCPU_REASON_IP_TTL_FAIL,
    GLB_PACKET_TOCPU_REASON_TUNNEL_MTU_DONTFRAG,
    GLB_PACKET_TOCPU_REASON_TUNNEL_MTU_FRAG,
    GLB_PACKET_TOCPU_REASON_BFD_UDP,

    GLB_PACKET_TOCPU_REASON_PTP,

    GLB_PACKET_TOCPU_REASON_IP_OPTION,
    GLB_PACKET_TOCPU_REASON_GRE_UNKOWN,
    GLB_PACKET_TOCPU_REASON_UCAST_IP_TTL_FAIL,
    GLB_PACKET_TOCPU_REASON_MPLS_TTL_FAIL,
    GLB_PACKET_TOCPU_REASON_MORE_RPF,
    GLB_PACKET_TOCPU_REASON_LINK_ID_FAIL,
    GLB_PACKET_TOCPU_REASON_IGMP_SNOOPING,

    GLB_PACKET_TOCPU_REASON_SFLOW_INGRESS,
    GLB_PACKET_TOCPU_REASON_SFLOW_EGRESS,

    GLB_PACKET_TOCPU_REASON_NORMAL_FWD,
    GLB_PACKET_TOCPU_REASON_FOAM_BFD, /*nhptr 0x8800 бл 0x8898 */
    GLB_PACKET_TOCPU_REASON_FOAM_TPOAM, /*nhptr 0x9900 бл 0x9998 */

    GLB_PACKET_TOCPU_REASON_ELOOP_FWD,
    GLB_PACKET_TOCPU_REASON_MAX
};
typedef enum glb_packet_tocpu_reason_e glb_packet_tocpu_reason_t;

/* l2-pdu definition */
enum glb_pdu_l2pdu_e
{
    GLB_PDU_L2PDU_BPDU = 0,     /* MAC-DA: 0180:C200:0000 */
    GLB_PDU_L2PDU_CFM,          /* ETHER-TYPE: 0X8902 */
    GLB_PDU_L2PDU_SLOW_PROTO,   /* ETHER-TYPE: 0X8809 */
    GLB_PDU_L2PDU_EAPOL,        /* ETHER-TYPE: 0X888E */
    GLB_PDU_L2PDU_ERPS,         /* MAC-DA: 0180:6307:0000  MASK: FFFF:FFFF:FFFD */
                                /* MAC-DA: 0001:7A4F:4826  MASK: FFFF:FFFF:FFFF */
    GLB_PDU_L2PDU_SMART_LINK,   /* ETHER_TYPE: 0x0127 */
    GLB_PDU_L2PDU_L2PING,       /* ETHER_TYPE: 0x9009 */
    GLB_PDU_L2PDU_UDLD,         /* ETHER_TYPE: 0x0111 */
    /* The newly added L2-PDUs should be placed from here */

    GLB_PDU_L2PDU_DEFAULT,      /* This reason is used for all packets which are sent to cpu
                                   via default action index. It's action is set to redirect-to-cpu
                                   when port is created*/

    GLB_PDU_L2PDU_MAX
};
typedef enum glb_pdu_l2pdu_e glb_pdu_l2pdu_t;

/* flexible reserved l2-pdu definition */
enum glb_pdu_flex_l2pdu_e
{
    GLB_PDU_FLEX_L2PDU_NORMAL,
    GLB_PDU_FLEX_L2PDU_L2PROTO_MAC0,
    GLB_PDU_FLEX_L2PDU_L2PROTO_MAC1,
    GLB_PDU_FLEX_L2PDU_L2PROTO_MAC_DFT,
    GLB_PDU_FLEX_L2PDU_L2PROTO_TUNNEL_DMAC,
    /* The newly added flexible reserved L2-PDUs should be placed from here */
    /* GLB_PDU_FLEX_L2PDU_NORMAL means it's action-index is dynamically assigned*/

    GLB_PDU_FLEX_L2PDU_MAX
};
typedef enum glb_pdu_flex_l2pdu_e glb_pdu_flex_l2pdu_t;

/* l2-pdu classification key type */
enum glb_pdu_l2pdu_key_type_e
{
    GLB_PDU_L2PDU_KEY_TYPE_L2HDR_PROTO = 0,
    GLB_PDU_L2PDU_KEY_TYPE_MACDA,
    GLB_PDU_L2PDU_KEY_TYPE_MACDA_LOW24,

    GLB_PDU_L2PDU_KEY_TYPE_MAX
};
typedef enum glb_pdu_l2pdu_key_type_e glb_pdu_l2pdu_key_type_t;

/* l2-pdu port action */
enum glb_pdu_l2pdu_port_action_e
{
    GLB_PDU_L2PDU_PORT_ACTION_REDIRECT_TO_CPU = 0,
    GLB_PDU_L2PDU_PORT_ACTION_COPY_TO_CPU,
    GLB_PDU_L2PDU_PORT_ACTION_FWD,
    GLB_PDU_L2PDU_PORT_ACTION_DISCARD,

    GLB_PDU_L2PDU_PORT_ACTION_MAX
};
typedef enum glb_pdu_l2pdu_port_action_e glb_pdu_l2pdu_port_action_t;

/* flexible l2-pdu classification key definition */
struct glb_pdu_flex_l2pdu_key_s
{
    uint8_t  flex_l2pdu;   /* glb_pdu_flex_l2pdu_t */
    uint8_t  key_type;     /* glb_pdu_l2pdu_key_type_t */
    uint8_t  reserved[2];

    union
    {
        struct
        {
            uint8_t macda[6];
            uint8_t macda_mask[6];
        } l2pdu_by_mac;

        uint16_t ether_type;
    } key;
};
typedef struct glb_pdu_flex_l2pdu_key_s glb_pdu_flex_l2pdu_key_t;

/* l3-pdu definition */
enum glb_pdu_l3pdu_e
{
    GLB_PDU_L3PDU_ARP = 0,     /* ETHER-TYPE: 0X0806 */
    GLB_PDU_L3PDU_DHCP,        /* UDP-PORT: 67/68 */
    GLB_PDU_L3PDU_RIP,         /* UDP-PORT: 520 */
    GLB_PDU_L3PDU_RIPNG,       /* UDP-PORT: 521 */
    GLB_PDU_L3PDU_LDP,         /* TCP/UDP-PORT: 646 */
    GLB_PDU_L3PDU_OSPF,        /* IP-PROTOCOL: 89 */
    GLB_PDU_L3PDU_PIM,         /* IP-PROTOCOL: 103 */
    GLB_PDU_L3PDU_VRRP,        /* IP-PROTOCOL: 112 */
    GLB_PDU_L3PDU_RSVP,        /* IP-PROTOCOL: 46 */
    GLB_PDU_L3PDU_ICMPV6,      /* IP-PROTOCOL: 58 */
    GLB_PDU_L3PDU_MPLS_OAM,    /* UDP-PORT: 0x3503 */
    /* The newly added L3-PDUs should be placed from here */
    GLB_PDU_L3PDU_BFD,         /*UDP-PORT: 3784*/

    GLB_PDU_L3PDU_MAX
};
typedef enum glb_pdu_l3pdu_e glb_pdu_l3pdu_t;

/* l3-pdu classification key type */
enum glb_pdu_l3pdu_key_type_e
{
    GLB_PDU_L3PDU_KEY_TYPE_L3HDR_PROTO = 0,
    GLB_PDU_L3PDU_KEY_TYPE_LAYER4_PORT,

    GLB_PDU_L3PDU_KEY_TYPE_MAX
};
typedef enum glb_pdu_l3pdu_key_type_e glb_pdu_l3pdu_key_type_t;

/* l3-pdu l3-interface action */
enum glb_pdu_l3pdu_l3if_action_e
{
    GLB_PDU_L3PDU_L3IF_ACTION_FWD = 0,
    GLB_PDU_L3PDU_L3IF_ACTION_COPY_TO_CPU,

    GLB_PDU_L3PDU_L3IF_ACTION_MAX
};
typedef enum glb_pdu_l3pdu_l3if_action_e glb_pdu_l3pdu_l3if_action_t;

#define GLB_CPU_TRAFFIC_MAP_REASON_TO_STR(reason)                                       \
    (reason == GLB_PACKET_TOCPU_REASON_USERID               ?  "userid"              :  \
     reason == GLB_PACKET_TOCPU_REASON_PROTO_VLAN           ?  "proto-vlan"          :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_BPDU          ?  "bpdu"                :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_CFM           ?  "cfm"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_SLOW_PROTO    ?  "slow-proto"          :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_EAPOL         ?  "eapol"               :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_ERPS          ?  "erps"                :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_SMART_LINK    ?  "smart-link"          :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_FLEX          ?  "flex-l2-pdu"         :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_UDLD          ?  "udld"                :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_DEFAULT       ?  "bridge-default"      :  \
     reason == GLB_PACKET_TOCPU_REASON_BRIDGE_MAC_COPY_CPU  ?  "mac-copy-cpu"        :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_ARP       ?  "arp"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_DHCP      ?  "dhcp"                :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_RIP       ?  "rip"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_LDP       ?  "ldp"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_OSPF      ?  "ospf"                :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_PIM       ?  "pim"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_VRRP      ?  "vrrp"                :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_RSVP      ?  "rsvp"                :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_ICMPV6    ?  "icmpv6"              :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_MPLS_OAM  ?  "mpls-oam"            :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_BFD       ?  "bfd"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_ROUTE_IPDA_DEFAULT   ?  "ipda"                :  \
     reason == GLB_PACKET_TOCPU_REASON_ICMP_REDIRECT        ?  "icmp-redirect"       :  \
     reason == GLB_PACKET_TOCPU_REASON_LEARNING_FULL        ?  "learning-full"       :  \
     reason == GLB_PACKET_TOCPU_REASON_MCAST_RPF_FAIL       ?  "mcast-rpf-fail"      :  \
     reason == GLB_PACKET_TOCPU_REASON_MACSA_DISCARD        ?  "macsa-discard"       :  \
     reason == GLB_PACKET_TOCPU_REASON_MACSA_MISMATCH       ?  "macsa-mismatch"      :  \
     reason == GLB_PACKET_TOCPU_REASON_PORT_SECURITY_DISCARD? "port-security-discard":  \
     reason == GLB_PACKET_TOCPU_REASON_VLAN_SECURITY_DISCARD? "vlan-security-discard":  \
     reason == GLB_PACKET_TOCPU_REASON_MCAST_TTL            ?  "mcast-ttl"           :  \
     reason == GLB_PACKET_TOCPU_REASON_MTU_DONTFRAG         ?  "mtu-dontfrag"        :  \
     reason == GLB_PACKET_TOCPU_REASON_MTU_FRAG             ?  "mtu-frag"            :  \
     reason == GLB_PACKET_TOCPU_REASON_IP_TTL_FAIL          ?  "ip-ttl-fail"         :  \
     reason == GLB_PACKET_TOCPU_REASON_TUNNEL_MTU_DONTFRAG  ?  "tunnel-mtu-dontfrag" :  \
     reason == GLB_PACKET_TOCPU_REASON_TUNNEL_MTU_FRAG      ?  "tunnel-mtu-frag"     :  \
     reason == GLB_PACKET_TOCPU_REASON_BFD_UDP              ?  "bfd-udp"             :  \
     reason == GLB_PACKET_TOCPU_REASON_PTP                  ?  "ptp"                 :  \
     reason == GLB_PACKET_TOCPU_REASON_IP_OPTION            ?  "ip-option"           :  \
     reason == GLB_PACKET_TOCPU_REASON_GRE_UNKOWN           ?  "gre-unknown"         :  \
     reason == GLB_PACKET_TOCPU_REASON_UCAST_IP_TTL_FAIL    ?  "ucast-ip-ttl-fail"   :  \
     reason == GLB_PACKET_TOCPU_REASON_MORE_RPF             ?  "more-rpf"            :  \
     reason == GLB_PACKET_TOCPU_REASON_LINK_ID_FAIL         ?  "link-id-fail"        :  \
     reason == GLB_PACKET_TOCPU_REASON_MPLS_TTL_FAIL        ?  "mpls-ttl-fail"       :  \
     reason == GLB_PACKET_TOCPU_REASON_IGMP_SNOOPING        ?  "igmp-snooping"       :  \
     reason == GLB_PACKET_TOCPU_REASON_SFLOW_INGRESS        ?  "sflow-ingress"       :  \
     reason == GLB_PACKET_TOCPU_REASON_SFLOW_EGRESS         ?  "sflow-egress"        :  \
     reason == GLB_PACKET_TOCPU_REASON_NORMAL_FWD           ?  "fwd-to-cpu"          :  \
     reason == GLB_PACKET_TOCPU_REASON_FOAM_BFD             ?  "foam-bfd"            :  \
     reason == GLB_PACKET_TOCPU_REASON_FOAM_TPOAM           ?  "mpls-tp-oam"         :  \
     "unknown-reason")

#define GLB_PDU_L2PDU_MAP_KEY_TYPE_TO_STR(key_type)                                     \
    (key_type == GLB_PDU_L2PDU_KEY_TYPE_L2HDR_PROTO ? "L2HDR-PROTO" :                   \
     key_type == GLB_PDU_L2PDU_KEY_TYPE_MACDA       ? "MACDA-FULL"  :                   \
     key_type == GLB_PDU_L2PDU_KEY_TYPE_MACDA_LOW24 ? "MACDA-LOW24" :                   \
     "error-type")

#define GLB_CPU_PKTTOCPU_BUILDUP_NHPTR(modid,exp_idx,action_idx)  ((modid) << 15 | (exp_idx) << 9 |(action_idx))
#define GLB_CPU_PKTTOCPU_NHPTR_MODID(nh_ptr) ((nh_ptr)>>15 & 0xff)
#define GLB_CPU_PKTTOCPU_NHPTR_EXCEP(nh_ptr) ((nh_ptr)>>9 & 0x3f)
#define GLB_CPU_PKTTOCPU_NHPTR_ACTION_INDEX(nh_ptr) ((nh_ptr) & 0xf)

#define GLB_CPU_TRAFFIC_NH_PTR_IS_FOR_OAM(nh_ptr) \
        (GLB_CPU_PKTTOCPU_NHPTR_MODID(nh_ptr) == GLB_CPU_PKTTOCPU_MOD_OAM_EXCP && \
         GLB_CPU_PKTTOCPU_NHPTR_EXCEP(nh_ptr) == 4)

#define GLB_CPU_TRAFFIC_NH_PTR_IS_FOR_FOAM(nh_ptr) \
        (GLB_CPU_PKTTOCPU_NHPTR_MODID(nh_ptr) == GLB_CPU_PKTTOCPU_MOD_OAM_EXCP && \
         GLB_CPU_PKTTOCPU_NHPTR_EXCEP(nh_ptr) == 12)

enum glb_cpu_pkttocpu_mod_e
{
    GLB_CPU_PKTTOCPU_MOD_NORMAL_EXCP = 0,  /**< normal exception */
    GLB_CPU_PKTTOCPU_MOD_FATAL_EXCP,       /**< fatal exception */
    GLB_CPU_PKTTOCPU_MOD_OAM_EXCP,         /**< oam exception */
    GLB_CPU_PKTTOCPU_MOD_NORMAL_FWD_CPU,   /**< normal forward to cpu*/
    GLB_CPU_PKTTOCPU_MOD_ELOOP_FWD_CPU,    /**< do eloop and forward to cpu*/
    GLB_CPU_PKTTOCPU_MOD_RSV1,             /**< reserved for future*/
    GLB_CPU_PKTTOCPU_MOD_RSV2,             /**< reserved for future*/
    GLB_CPU_PKTTOCPU_MOD_RSV3,             /**< reserved for future*/
    MAX_GLB_CPU_PKTTOCPU_MOD_FLAG,
};

/* l2-pdu action index */
enum glb_l2pdu_action_index_e
{
    /* statically assigned */
    GLB_L2PDU_ACTION_INDEX_BPDU                 = 0,    /* BPDU action index */
    GLB_L2PDU_ACTION_INDEX_CFM                  = 1,    /* CFM action index */
    GLB_L2PDU_ACTION_INDEX_SLOW_PROTO           = 2,    /* slow protocol action index */
    GLB_L2PDU_ACTION_INDEX_EAPOL                = 3,    /* EAPOL action index */
    GLB_L2PDU_ACTION_INDEX_ERPS                 = 5,    /* ERPS action index */
    GLB_L2PDU_ACTION_INDEX_SMART_LINK           = 6,    /* SMART LINK action index */
    GLB_L2PDU_ACTION_INDEX_L2PROTO_MAC0         = 7,    /* L2Protocol MAC0 */
    GLB_L2PDU_ACTION_INDEX_L2PROTO_MAC1         = 8,    /* L2Protocol MAC1 */
    GLB_L2PDU_ACTION_INDEX_L2PROTO_MAC_DFT      = 9,    /* L2Protocol MAC Default */
    GLB_L2PDU_ACTION_INDEX_L2PROTO_TUNNEL_DMAC  = 10,   /* L2Protocol Tunnel DMAC */
    GLB_L2PDU_ACTION_INDEX_UDLD                 = 11,   /* UDLD action index */

    /* The default L2-PDU action index used to send PDU to CPU for potential process in Peth */
    GLB_L2PDU_ACTION_INDEX_DEFAULT              = 14,
    GLB_L2PDU_ACTION_INDEX_MAC_COPY_CPU         = 15,
};
typedef enum glb_l2pdu_action_index_e glb_l2pdu_action_index_t;

/* l3-pdu action index */
enum glb_l3pdu_action_index_e
{
    GLB_L3PDU_ACTION_INDEX_RIP         = 1,    /* RIP action index */
    GLB_L3PDU_ACTION_INDEX_RIPNG       = 1,    /* RIPng action index */
    GLB_L3PDU_ACTION_INDEX_LDP         = 2,    /* LDP action index */
    GLB_L3PDU_ACTION_INDEX_OSPF        = 3,    /* OSPF action index */
    GLB_L3PDU_ACTION_INDEX_PIM         = 5,    /* PIM action index */
    GLB_L3PDU_ACTION_INDEX_VRRP        = 6,    /* VRRP action index */
    GLB_L3PDU_ACTION_INDEX_RSVP        = 7,    /* RSVP action index */
    GLB_L3PDU_ACTION_INDEX_ICMPV6      = 8,    /* ICMPV6 action index */
    GLB_L3PDU_ACTION_INDEX_MPLS_OAM    = 9,    /* MPLS-OAM action index */
    GLB_L3PDU_ACTION_INDEX_BFD         = 10,   /* BFD action index */
    GLB_L3PDU_ACTION_INDEX_DEFAULT     = 11,   /* L3-PDU default action index */
    GLB_L3PDU_ACTION_INDEX_ARP         = 13,   /* ARP action index, SDK allocated */
    GLB_L3PDU_ACTION_INDEX_DHCP        = 14,   /* DHCP action index, SDK allocated */
    GLB_L3PDU_ACTION_INDEX_IPDA        = 15

};
typedef enum glb_l3pdu_action_index_e hagt_l3pdu_action_index_t;

#endif /* __GLB_CPU_TRAFFIC_DEFINE_H__*/
