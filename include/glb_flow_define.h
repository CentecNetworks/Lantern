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

#ifndef __GLB_FLOW_DEFINE_H__
#define __GLB_FLOW_DEFINE_H__

#define GLB_FLOW_TCP_FLAG_OP_TO_STR(op)                                         \
    (op == GLB_FLOW_TCP_FLAG_OP_ANY ? "ANY" :                                   \
     op == GLB_FLOW_TCP_FLAG_OP_ALL ? "ALL" :                                   \
     "ERROR")

#define GLB_FLOW_L4PORT_OP_TO_STR(op)                                           \
    (op == GLB_FLOW_L4PORT_OPERATOR_EQ ? "EQ" :                                 \
     op == GLB_FLOW_L4PORT_OPERATOR_GT ? "GT" :                                 \
     op == GLB_FLOW_L4PORT_OPERATOR_LT ? "LT" :                                 \
     op == GLB_FLOW_L4PORT_OPERATOR_NEQ ? "NEQ" :                               \
     op == GLB_FLOW_L4PORT_OPERATOR_RANGE ? "RANGE" :                           \
     "ERROR")

#define GLB_FLOW_L2TYPE_TO_STR(type)                                            \
    (type == GLB_FLOW_L2TYPE_ETH_V2 ? "eth2" :                                  \
     type == GLB_FLOW_L2TYPE_ETH_SAP ? "sap" :                                  \
     type == GLB_FLOW_L2TYPE_ETH_SNAP ? "snap" :                                \
     type == GLB_FLOW_L2TYPE_PPP_2B ? "ppp-2b" :                                \
     type == GLB_FLOW_L2TYPE_PPP_1B ? "ppp-1b" :                                \
     type == GLB_FLOW_L2TYPE_RAW_SNAP ? "raw-snap" :                            \
     "error l2-type")

#define GLB_FLOW_L3TYPE_TO_STR(type)                                            \
    (type == GLB_FLOW_L3TYPE_IP_V4 ? "ipv4" :                                   \
     type == GLB_FLOW_L3TYPE_IP_V6 ? "ipv6" :                                   \
     type == GLB_FLOW_L3TYPE_MPLS ? "mpls" :                                    \
     type == GLB_FLOW_L3TYPE_MPLS_MCAST ? "mpls_mcast" :                        \
     type == GLB_FLOW_L3TYPE_ARP ? "arp" :                                      \
     type == GLB_FLOW_L3TYPE_RARP ? "rarp" :                                    \
     type == GLB_FLOW_L3TYPE_EAPOL ? "eapol" :                                  \
     type == GLB_FLOW_L3TYPE_SLOWPROTO ? "slow-proto" :                         \
     type == GLB_FLOW_L3TYPE_CMAC ? "cmac" :                                    \
     type == GLB_FLOW_L3TYPE_PTP ? "ptp" :                                      \
     "error l3-type")

#define GLB_FLOW_L4PROTO_TO_STR(l4protocal)                                     \
    (l4protocal == 1 ? "icmp" :                                                 \
     l4protocal == 58 ? "icmpv6" :                                              \
     l4protocal == 2 ? "igmp" :                                                 \
     l4protocal == 6 ? "tcp" :                                                  \
     l4protocal == 17 ? "udp" :                                                 \
     l4protocal == 0 ? "any" :                                                  \
     "error l4-proto")

enum glb_flow_l2type_e
{
    GLB_FLOW_L2TYPE_ETH_V2   = 1,    /**< L2 packet type: Ethernet II */
    GLB_FLOW_L2TYPE_ETH_SAP  = 2,    /**< L2 packet type: SAP */
    GLB_FLOW_L2TYPE_ETH_SNAP = 3,    /**< L2 packet type: SNAP */
    GLB_FLOW_L2TYPE_PPP_2B   = 4,    /**< L2 packet type: PPP-2B */
    GLB_FLOW_L2TYPE_PPP_1B   = 5,    /**< L2 packet type: PPP-1B */
    GLB_FLOW_L2TYPE_RAW_SNAP = 6     /**< L2 packet type: RAW */
};

enum glb_flow_l3type_e
{
    GLB_FLOW_L3TYPE_IP_V4      = 2,  /**< L3 packet type: IPv4 */
    GLB_FLOW_L3TYPE_IP_V6      = 3,  /**< L3 packet type: IPv6 */
    GLB_FLOW_L3TYPE_MPLS       = 4,  /**< L3 packet type: MPLS */
    GLB_FLOW_L3TYPE_MPLS_MCAST = 5,  /**< L3 packet type: MPLS Multicast */
    GLB_FLOW_L3TYPE_ARP        = 6,  /**< L3 packet type: ARP */
    GLB_FLOW_L3TYPE_RARP       = 7,  /**< L3 packet type: RARP */
    GLB_FLOW_L3TYPE_EAPOL      = 8,  /**< L3 packet type: EAPOL */
    GLB_FLOW_L3TYPE_ETHOAM     = 9,  /**< L3 packet type: Ethernet OAM */
    GLB_FLOW_L3TYPE_SLOWPROTO  = 10, /**< L3 packet type: Slow Protocol */
    GLB_FLOW_L3TYPE_CMAC       = 11, /**< L3 packet type: CMAC */
    GLB_FLOW_L3TYPE_PTP        = 12  /**< L3 packet type: PTP */
};

enum glb_flow_action_flag_e
{
    GLB_FLOW_ACTION_FLAG_DISCARD              = 1 << 0,
    GLB_FLOW_ACTION_FLAG_DENY_BRIDGE          = 1 << 1,
    GLB_FLOW_ACTION_FLAG_DENY_LEARN           = 1 << 2,
    GLB_FLOW_ACTION_FLAG_DENY_ROUTE           = 1 << 3,
    GLB_FLOW_ACTION_FLAG_DENY_REPLACE_COS     = 1 << 4,
    GLB_FLOW_ACTION_FLAG_DENY_REPLACE_DSCP    = 1 << 5,
    GLB_FLOW_ACTION_FLAG_STATS                = 1 << 6,
    GLB_FLOW_ACTION_FLAG_REDIRECT_IF          = 1 << 7,
    GLB_FLOW_ACTION_FLAG_REDIRECT_CPU         = 1 << 8,
    GLB_FLOW_ACTION_FLAG_LOG_IF               = 1 << 9,
    GLB_FLOW_ACTION_FLAG_LOG                  = 1 << 10,
    GLB_FLOW_ACTION_FLAG_TRUST                = 1 << 11,
    GLB_FLOW_ACTION_FLAG_PRIORITY             = 1 << 12,
    GLB_FLOW_ACTION_FLAG_FLOW_POLICER         = 1 << 13,
    GLB_FLOW_ACTION_FLAG_INVALID              = 1 << 14
};

enum glb_flow_rule_type_e
{
    GLB_FLOW_RULE_TYPE_MAC,          /* MAC flow rule type */
    GLB_FLOW_RULE_TYPE_IPV4,         /* IPv4 flow rule type */
    GLB_FLOW_RULE_TYPE_IPV6,         /* IPv6 flow filter type */
    GLB_FLOW_RULE_TYPE_EXIP,         /* Extended IP flow rule type */
    GLB_FLOW_RULE_TYPE_EXIPV6,       /* Extended IPv6 flow rule type */
    GLB_FLOW_RULE_TYPE_MPLS,         /* MPLS flow rule type */

    GLB_FLOW_RULE_TYPE_MAX
};
typedef enum glb_flow_rule_type_e glb_flow_rule_type_t;


enum glb_flow_l4port_operator_e
{
    GLB_FLOW_L4PORT_OPERATOR_EQ,         /* x = m */
    GLB_FLOW_L4PORT_OPERATOR_NEQ,        /* x != m */
    GLB_FLOW_L4PORT_OPERATOR_GT,         /* x > m */
    GLB_FLOW_L4PORT_OPERATOR_LT,         /* x < m */
    GLB_FLOW_L4PORT_OPERATOR_RANGE,      /* m < x < n */

    GLB_FLOW_L4PORT_OPERATOR_MAX
};
typedef enum glb_flow_l4port_operator_e glb_flow_l4port_operator_t;


/* TCP flag operator */
enum glb_flow_tcp_flag_operator_e
{
    GLB_FLOW_TCP_FLAG_OP_ANY,     /* any flags being set */
    GLB_FLOW_TCP_FLAG_OP_ALL,     /* all flags being set */

    GLB_FLOW_TCP_FLAG_OP_MAX
};
typedef enum glb_flow_tcp_flag_operator_e glb_acl_tcp_flag_operator_t;


enum glb_flow_tcp_flag_flag_e
{
    GLB_FLOW_TCP_FLAG_URG = 1 << 0,
    GLB_FLOW_TCP_FLAG_ACK = 1 << 1,
    GLB_FLOW_TCP_FLAG_PSH = 1 << 2,
    GLB_FLOW_TCP_FLAG_RST = 1 << 3,
    GLB_FLOW_TCP_FLAG_SYN = 1 << 4,
    GLB_FLOW_TCP_FLAG_FIN = 1 << 5
};


enum glb_flow_ip_frag_e
{
    GLB_FLOW_IP_FRAG_NONE,           /* non fragment or an initial fragment */
    GLB_FLOW_IP_FRAG_NON_INITIAL,    /* non initial fragment */
    GLB_FLOW_IP_FRAG_TINY,           /* small fragment */
    GLB_FLOW_IP_FRAG_LAST,           /* last fragment */

    GLB_FLOW_IP_FRAG_MAX
};
typedef enum glb_flow_ip_frag_e glb_flow_ip_frag_t;


enum glb_flow_igmp_type_e
{
    GLB_FLOW_IGMP_TYPE_PRECEDENCE       = 0,
    GLB_FLOW_IGMP_TYPE_HOST_QUERY       = 0x11,
    GLB_FLOW_IGMP_TYPE_HOST_REPORT      = 0x12,
    GLB_FLOW_IGMP_TYPE_HOST_DVMRP       = 0x13,
    GLB_FLOW_IGMP_TYPE_PIM              = 0x14,
    GLB_FLOW_IGMP_TYPE_TRACE            = 0x15,
    GLB_FLOW_IGMP_TYPE_V2_REPORT        = 0x16,
    GLB_FLOW_IGMP_TYPE_V2_LEAVE         = 0x17,
    GLB_FLOW_IGMP_TYPE_MTRACT_RESPONSE  = 0x1e,
    GLB_FLOW_IGMP_TYPE_MTRACT           = 0x1f,
    GLB_FLOW_IGMP_TYPE_V3_REPORT        = 0x22
};
typedef enum glb_flow_igmp_type_e glb_flow_igmp_type_t;


enum glb_flow_mac_rule_flag_e
{
    GLB_FLOW_MAC_RULE_FLAG_MACSA       = 1 << 0,
    GLB_FLOW_MAC_RULE_FLAG_MACDA       = 1 << 1,
    GLB_FLOW_MAC_RULE_FLAG_CVLAN_ID    = 1 << 2,
    GLB_FLOW_MAC_RULE_FLAG_SVLAN_ID    = 1 << 3,
    GLB_FLOW_MAC_RULE_FLAG_CTAG_COS    = 1 << 4,
    GLB_FLOW_MAC_RULE_FLAG_STAG_COS    = 1 << 5,
    GLB_FLOW_MAC_RULE_FLAG_L3TYPE      = 1 << 6,
    GLB_FLOW_MAC_RULE_FLAG_L2TYPE      = 1 << 7
};


enum glb_flow_ipv4_rule_flag_e
{
    GLB_FLOW_IPV4_RULE_FLAG_IPSA           = 1 << 0,
    GLB_FLOW_IPV4_RULE_FLAG_IPDA           = 1 << 1,
    GLB_FLOW_IPV4_RULE_FLAG_L4_PROTO       = 1 << 2,
    GLB_FLOW_IPV4_RULE_FLAG_L4_SRC_PORT    = 1 << 3,
    GLB_FLOW_IPV4_RULE_FLAG_L4_DST_PORT    = 1 << 4,
    GLB_FLOW_IPV4_RULE_FLAG_TCP_FLAG       = 1 << 5,
    GLB_FLOW_IPV4_RULE_FLAG_ICMP_TYPE      = 1 << 6,
    GLB_FLOW_IPV4_RULE_FLAG_IGMP_TYPE      = 1 << 7,
    GLB_FLOW_IPV4_RULE_FLAG_ICMP_CODE      = 1 << 8,
    GLB_FLOW_IPV4_RULE_FLAG_DSCP           = 1 << 9,
    GLB_FLOW_IPV4_RULE_FLAG_IP_FRAG        = 1 << 10,
    GLB_FLOW_IPV4_RULE_FLAG_IP_OPTIONS     = 1 << 11,
    GLB_FLOW_IPV4_RULE_FLAG_ROUTED_PACKET  = 1 << 12,

    /* extended mac rule flags */
    GLB_FLOW_IPV4_RULE_FLAG_MACSA          = 1 << 13,
    GLB_FLOW_IPV4_RULE_FLAG_MACDA          = 1 << 14,
    GLB_FLOW_IPV4_RULE_FLAG_CVLAN_ID       = 1 << 15,
    GLB_FLOW_IPV4_RULE_FLAG_SVLAN_ID       = 1 << 16,
    GLB_FLOW_IPV4_RULE_FLAG_CTAG_COS       = 1 << 17,
    GLB_FLOW_IPV4_RULE_FLAG_STAG_COS       = 1 << 18,
    GLB_FLOW_IPV4_RULE_FLAG_L3TYPE         = 1 << 19,
    GLB_FLOW_IPV4_RULE_FLAG_L2TYPE         = 1 << 20
};


enum glb_flow_ipv6_rule_flag_e
{
    GLB_FLOW_IPV6_RULE_FLAG_IPSA           = 1 << 0,
    GLB_FLOW_IPV6_RULE_FLAG_IPDA           = 1 << 1,
    GLB_FLOW_IPV6_RULE_FLAG_L4_PROTO       = 1 << 2,
    GLB_FLOW_IPV6_RULE_FLAG_L4_SRC_PORT    = 1 << 3,
    GLB_FLOW_IPV6_RULE_FLAG_L4_DST_PORT    = 1 << 4,
    GLB_FLOW_IPV6_RULE_FLAG_TCP_FLAG       = 1 << 5,
    GLB_FLOW_IPV6_RULE_FLAG_ICMP_TYPE      = 1 << 6,
    GLB_FLOW_IPV6_RULE_FLAG_ICMP_CODE      = 1 << 7,
    GLB_FLOW_IPV6_RULE_FLAG_DSCP           = 1 << 9,
    GLB_FLOW_IPV6_RULE_FLAG_IP_FRAG        = 1 << 10,
    GLB_FLOW_IPV6_RULE_FLAG_IP_OPTION      = 1 << 11,
    GLB_FLOW_IPV6_RULE_FLAG_ROUTED_PACKET  = 1 << 12,
    GLB_FLOW_IPV6_RULE_FLAG_ROUTING        = 1 << 13,
    GLB_FLOW_IPV6_RULE_FLAG_AUTHEN         = 1 << 14,
    GLB_FLOW_IPV6_RULE_FLAG_DESTOPTS       = 1 << 15,

    /* extended mac rule flags */
    GLB_FLOW_IPV6_RULE_FLAG_MACSA          = 1 << 16,
    GLB_FLOW_IPV6_RULE_FLAG_MACDA          = 1 << 17,
    GLB_FLOW_IPV6_RULE_FLAG_CVLAN_ID       = 1 << 18,
    GLB_FLOW_IPV6_RULE_FLAG_SVLAN_ID       = 1 << 19,
    GLB_FLOW_IPV6_RULE_FLAG_CTAG_COS       = 1 << 20,
    GLB_FLOW_IPV6_RULE_FLAG_STAG_COS       = 1 << 21,
    GLB_FLOW_IPV6_RULE_FLAG_L3TYPE         = 1 << 22,
    GLB_FLOW_IPV6_RULE_FLAG_L2TYPE         = 1 << 23
};


/*-------------------------- MAC filter --------------------------*/

struct glb_flow_mac_rule_flag_s
{
  uint32 mac_sa:1,
         cvlan:1,
         svlan:1,
         l3_type:1,
         mac_da:1,
         l2_type:1,
         ctag_cos:1,
         stag_cos:1,
         reserved:24;
};
typedef struct glb_flow_mac_rule_flag_s glb_flow_mac_rule_flag_t;

struct glb_flow_mac_rule_s
{
   glb_flow_mac_rule_flag_t flag;

    mac_addr_t mac_sa;
    mac_addr_t mac_sa_mask;
    mac_addr_t mac_da;
    mac_addr_t mac_da_mask;
    uint16 cvlan;
    uint16 svlan;
    uint8  ctag_cos;
    uint8  stag_cos;
    uint8  l3_type;
    uint8  l2_type;
};
typedef struct glb_flow_mac_rule_s glb_flow_mac_rule_t;


/*-------------------------- IPv4 filter --------------------------*/

struct glb_flow_ipv4_rule_flag_s
{
    uint32 ip_sa:1,
           ip_da:1,
           l4_protocol:1,
           l4_source_port:1,
           l4_dest_port:1,
           tcp_flag:1,
           icmp_type:1,
           igmp_type:1,
           icmp_code:1,
           dscp:1,
           ip_frag:1,
           ip_options:1,
           routed_packet:1,
           reserved:19;
};
typedef struct glb_flow_ipv4_rule_flag_s glb_flow_ipv4_rule_flag_t;


/* l4 port parameter */
struct glb_flow_l4_port_s
{
    glb_flow_l4port_operator_t operator;
    uint16 port;      /* first port */
    uint16 port2;     /* second port */
};
typedef struct glb_flow_l4_port_s glb_flow_l4_port_t;

/* TCP flag parameter */
struct glb_flow_tcp_flag_s
{
    glb_acl_tcp_flag_operator_t operator;
    uint8 tcp_flags;    /* GLB_FLOW_TCP_FLAG_XXX */
    uint8 reserved[3];
};
typedef struct glb_flow_tcp_flag_s glb_flow_tcp_flag_t;


struct glb_flow_ipv4_rule_s
{
    glb_flow_ipv4_rule_flag_t flag;

    uint32 ip_sa;
    uint32 ip_sa_mask;
    uint32 ip_da;
    uint32 ip_da_mask;
    glb_flow_l4_port_t l4_source_port;
    glb_flow_l4_port_t l4_dest_port;
    glb_flow_ip_frag_t ip_frag;
    glb_flow_tcp_flag_t tcp_flag;
    uint8  ip_options;
    uint8  routed_packet;
    uint8  dscp;
    uint8  l4_protocol;
    uint8  icmp_type;
    uint8  icmp_code;
    uint8  igmp_type;
    uint8  reserved;
};
typedef struct glb_flow_ipv4_rule_s glb_flow_ipv4_rule_t;

/*-------------------------- IPv6 filter --------------------------*/

struct glb_flow_ipv6_rule_flag_s
{
    uint32 ip_sa:1,
           ip_da:1,
           l4_protocol:1,
           l4_source_port:1,
           l4_dest_port:1,
           tcp_flag:1,
           icmp_type:1,
           icmp_code:1,
           igmp_type:1,
           dscp:1,
           ip_frag:1,
           ip_options:1,
           routed_packet:1,
           routing:1,
           authen:1,
           destopts:1,
           reserved:16;
};
typedef struct glb_flow_ipv6_rule_flag_s glb_flow_ipv6_rule_flag_t;

struct glb_flow_ipv6_rule_s
{
    glb_flow_ipv6_rule_flag_t flag;

    ipv6_addr_t sprefix;
    ipv6_addr_t dprefix;
    uint8  sprefix_len;
    uint8  dprefix_len;
    uint8  ip_frag;        /* GLB_ACL_IP_FRAG_XXX */
    uint8  ip_options;
    glb_flow_l4_port_t l4_source_port;
    glb_flow_l4_port_t l4_dest_port;
    glb_flow_tcp_flag_t tcp_flag;
    uint8  routed_packet;
    uint8  l4_protocol;
    uint8  icmp_type;
    uint8  icmp_code;
    uint8  ext_hdr;
    uint8  dscp;
    uint8  reserved[2];
};
typedef struct glb_flow_ipv6_rule_s glb_flow_ipv6_rule_t;

struct glb_flow_rule_s
{
    glb_flow_rule_type_t rule_type;
    
    uint8 discard;
    uint8 invalid;
    uint8 reserved[2];
    
    union
    {
        glb_flow_mac_rule_t  mac_rule;
        glb_flow_ipv4_rule_t ipv4_rule;
        glb_flow_ipv6_rule_t ipv6_rule;
    } rule;
};
typedef struct glb_flow_rule_s glb_flow_rule_t;


#endif /* __GLB_FLOW_DEFINE_H__*/
