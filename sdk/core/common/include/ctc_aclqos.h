/**
 @file ctc_aclqos.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-30

 @version v2.0

   This file contains all acl/qos related data structure, enum, macro and proto.
*/


#ifndef _CTC_ACLQOS_H_
#define _CTC_ACLQOS_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "ctc_const.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define CTC_ACLQOS_ENTRY_ID_HEAD 0
#define CTC_ACLQOS_ENTRY_ID_TAIL 0xFFFFFFFF

/****************************************************************
*
* Data Structures
*
****************************************************************/

/**
 @defgroup aclqos ACLQoS
 @{
*/


/**
 @defgroup acl ACL
 @{
*/


/**
 @brief ACL layer 2 type
*/
enum ctc_acl_l2type_e
{
    ACL_L2TYPE_ETH_V2   = 1,    /**< L2 packet type: Ethernet II */
    ACL_L2TYPE_ETH_SAP  = 2,    /**< L2 packet type: SAP */
    ACL_L2TYPE_ETH_SNAP = 3,    /**< L2 packet type: SNAP */
    ACL_L2TYPE_PPP_2B   = 4,    /**< L2 packet type: PPP-2B */
    ACL_L2TYPE_PPP_1B   = 5,    /**< L2 packet type: PPP-1B */
    ACL_L2TYPE_RAW_SNAP = 6     /**< L2 packet type: RAW */
};

/**
 @brief ACL layer 3 type
*/
enum ctc_acl_l3type_e
{
    ACL_L3TYPE_IP_V4      = 2,  /**< L3 packet type: IPv4 */
    ACL_L3TYPE_IP_V6      = 3,  /**< L3 packet type: IPv6 */
    ACL_L3TYPE_MPLS       = 4,  /**< L3 packet type: MPLS */
    ACL_L3TYPE_MPLS_MCAST = 5,  /**< L3 packet type: MPLS Multicast */
    ACL_L3TYPE_ARP        = 6,  /**< L3 packet type: ARP */
    ACL_L3TYPE_RARP       = 7,  /**< L3 packet type: RARP */
    ACL_L3TYPE_EAPOL      = 8,  /**< L3 packet type: EAPOL */
    ACL_L3TYPE_ETHOAM     = 9,  /**< L3 packet type: Ethernet OAM */
    ACL_L3TYPE_SLOWPROTO  = 10, /**< L3 packet type: Slow Protocol */
    ACL_L3TYPE_CMAC       = 11, /**< L3 packet type: CMAC */
    ACL_L3TYPE_PTP        = 12  /**< L3 packet type: PTP */
};


/**@} end of @defgroup acl ACL  */

/**
 @defgroup qos QoS
 @{
*/


/**
 @brief Qos trust
*/
enum ctc_qos_trust_e
{
    CTC_QOS_TRUST_PORT,         /**< Trust port default CoS */
    CTC_QOS_TRUST_OUTER,        /**< Trust outer priority and color, for tunnel or loopback */
    CTC_QOS_TRUST_COS,          /**< Trust packet CoS field */
    CTC_QOS_TRUST_DSCP,         /**< Trust IP DSCP field */
    CTC_QOS_TRUST_IPPREC,       /**< Trust IP precedence field */

    MAX_CTC_QOS_TRUST
};
typedef enum ctc_qos_trust_e ctc_qos_trust_t;


/**
 @brief Qos replace
*/
enum ctc_qos_replace_e
{
    CTC_QOS_REPLACE_COS,    /**< Replace packet CoS field */
    CTC_QOS_REPLACE_DSCP,   /**< Replace packet DSCP field */

    MAX_CTC_QOS_REPLACE
};
typedef enum ctc_qos_replace_e ctc_qos_replace_t;


/**
 @brief QoS color
*/
enum ctc_qos_color_e
{
    CTC_QOS_COLOR_NONE,     /**< None color */
    CTC_QOS_COLOR_RED,      /**< Red color: the lowest drop precedence */
    CTC_QOS_COLOR_YELLOW,   /**< Yellow color: the mild drop precedence */
    CTC_QOS_COLOR_GREEN,    /**< Green color: the highest drop precedence */

    MAX_CTC_QOS_COLOR
};
typedef enum ctc_qos_color_e ctc_qos_color_t;


/**
 @brief Qos policer
*/
struct ctc_qos_policer_s
{
    uint32 cir;             /**< Committed Information Rate */
    uint32 cbs;             /**< Committed Burst Size, equivalent to the C-bucket size */
    uint32 pir;             /**< Peak Information Rate */
    uint32 pbs;             /**< (1) Peak Burst Size, equivalent to the P-bucket size (if TrTCM, is_srtcm = 0), or
                                 (2) Excessive Burst Size, equivalent to the E-bucket size (if SrTCM, is_srtcm = 1) */

    uint8 use_l3_length;    /**< Use packet length from layer 3 header for metering */
    uint8 is_srtcm;         /**< Is_srtcm = 1 for SrTCM (Single-rate Three Color Marking),
                                 Is_srtcm = 0 for TrTCM (Two-rate Three Color Marking) */
    uint8 is_color_blind;   /**< Is_color_blind = 1 for Color-blind mode, is_color_blind = 0 for Color-awre mode */
    uint8 is_stats_en;      /**< enable policer stats for three color */

    ctc_qos_color_t drop_color; /**< Drop packet whose color is lower than the drop color after policing */
};
typedef struct ctc_qos_policer_s ctc_qos_policer_t;


/**
 @brief Qos policer statistics
*/
struct ctc_qos_policer_stats_s
{
    uint64 confirm_pkts;        /**< Total number of packets with color green*/
    uint64 confirm_bytes;       /**< Total bytes of packets with color green*/
    uint64 exceed_pkts;         /**< Total number of packets with color yellow*/
    uint64 exceed_bytes;        /**< Total bytes of packets with color yellow*/
    uint64 violate_pkts;        /**< Total number of packets with color red*/
    uint64 violate_bytes;	    /**< Total bytes of packets with color red*/
};
typedef struct ctc_qos_policer_stats_s ctc_qos_policer_stats_t;


/**@} end of @defgroup qos QoS */


/**
 @brief ACL/Qos label type
*/
enum ctc_aclqos_label_type_e
{
    CTC_ACL_LABEL = 0,    /**< ACL label type */
    CTC_QOS_LABEL,        /**< QoS label type */
    CTC_SERVICE_LABEL,    /**< Service label type*/

    MAX_CTC_ACLQOS_LABEL_TYPE
};
typedef enum ctc_aclqos_label_type_e ctc_aclqos_label_type_t;


/**
 @brief ACL/Qos global label
*/
enum ctc_aclqos_global_label_e
{
    CTC_GLOBAL_ACL_INGRESS_LABEL_ID_HEAD = 0xFFFFFFF1,  /**< Global acl ingress label id */
    CTC_GLOBAL_ACL_INGRESS_LABEL_ID_TAIL,               /**< Global acl ingress label id */
    CTC_GLOBAL_ACL_EGRESS_LABEL_ID_HEAD,                /**< Global acl egress label id */
    CTC_GLOBAL_ACL_EGRESS_LABEL_ID_TAIL,                /**< Global acl egress label id */
    CTC_GLOBAL_QOS_INGRESS_LABEL_ID_HEAD,               /**< Global qos ingress label id */
    CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL,               /**< Global qos ingress label id */
    CTC_GLOBAL_QOS_EGRESS_LABEL_ID_HEAD,                /**< Global qos egress label id */
    CTC_GLOBAL_QOS_EGRESS_LABEL_ID_TAIL,                /**< Global qos egress label id */
    CTC_GLOBAL_ACL_PBR_LABEL_ID_HEAD,                   /**< Global acl pbr label id */
    CTC_GLOBAL_ACL_PBR_LABEL_ID_TAIL                    /**< Global acl pbr label id */
};
typedef enum ctc_aclqos_global_label_e ctc_aclqos_global_label_t;


/**
 @brief ACL/Qos key type
*/
enum ctc_aclqos_key_type_e
{
    CTC_ACLQOS_MAC_KEY = 0,     /**< ACL/QoS MAC key type */
    CTC_ACLQOS_IPV4_KEY,        /**< ACL/QoS IPv4 key type */
    CTC_ACLQOS_MPLS_KEY,        /**< ACL/QoS MPLS key type */
    CTC_ACLQOS_IPV6_KEY,        /**< ACL/QoS IPv6 key type */
    CTC_ACL_PBR_IPV4_KEY,       /**< ACL PBR IPV4 key type */
    CTC_ACL_PBR_IPV6_KEY,       /**< ACL PBR IPv6 key type */

    MAX_CTC_ACLQOS_KEY
};
typedef enum ctc_aclqos_key_type_e ctc_aclqos_key_type_t;


/**
 @brief ACL/Qos layer 4 port operator
*/
enum ctc_aclqos_l4port_operator_e
{
    CTC_ACLQOS_L4PORT_OPERATOR_EQ,            /**< Equal, x = m */
    CTC_ACLQOS_L4PORT_OPERATOR_NEQ,           /**< Not equal, x != m */
    CTC_ACLQOS_L4PORT_OPERATOR_GT,            /**< Greater than, x > m */
    CTC_ACLQOS_L4PORT_OPERATOR_LT,            /**< Less than, x < m */
    CTC_ACLQOS_L4PORT_OPERATOR_RANGE,         /**< Range, m <= x <= n */

    MAX_CTC_ACLQOS_L4PORT_OPERATOR
};
typedef enum ctc_aclqos_l4port_operator_e ctc_aclqos_l4port_operator_t;


/**
 @brief ACL/Qos ip fragment
*/
enum ctc_aclqos_ip_frag_e
{
    CTC_ACLQOS_IP_FRAG_NONE,              /**< Non fragment or initial fragment */
    CTC_ACLQOS_IP_FRAG_NON_INITIAL,       /**< Non initial fragment */
    CTC_ACLQOS_IP_FRAG_TINY,              /**< Small fragment */
    CTC_ACLQOS_IP_FRAG_LAST,              /**< Last fragment */

    MAX_CTC_ACLQOS_IP_FRAG
};
typedef enum ctc_aclqos_ip_frag_e ctc_aclqos_ip_frag_t;


/**
 @brief ACL/Qos tcp flag operator
*/
enum ctc_aclqos_tcp_flag_operator_e
{
    CTC_ACLQOS_TCP_FLAG_ALL,         /**< All TCP flags being set */
    CTC_ACLQOS_TCP_FLAG_ANY,         /**< Any TCP flags being set */

    MAX_CTC_ACLQOS_TCP_FLAG
};
typedef enum ctc_aclqos_tcp_flag_operator_e ctc_aclqos_tcp_flag_operator_t;


/**
 @brief ACL/Qos tcp flag
*/
enum ctc_aclqos_tcp_flag_flag_e
{
    CTC_ACLQOS_TCP_FIN_FLAG = 1U << 0,  /**< TCP fin flag */
    CTC_ACLQOS_TCP_SYN_FLAG = 1U << 1,  /**< TCP syn flag */
    CTC_ACLQOS_TCP_RST_FLAG = 1U << 2,  /**< TCP rst flag */
    CTC_ACLQOS_TCP_PSH_FLAG = 1U << 3,  /**< TCP psh flag */
    CTC_ACLQOS_TCP_ACK_FLAG = 1U << 4,  /**< TCP ack flag */
    CTC_ACLQOS_TCP_URG_FLAG = 1U << 5   /**< TCP urg flag */
};


/**
 @brief ACL/Qos tcp flag operator and flag bits
*/
struct ctc_aclqos_tcp_flag_s
{
    ctc_aclqos_tcp_flag_operator_t operator;    /**< CTC_ACLQOS_TCP_FLAG_ANY (_ALL) */
    uint32 flag;                                /**< Bitmap of CTC_ACLQOS_TCP_XXX_FLAG */
};
typedef struct ctc_aclqos_tcp_flag_s ctc_aclqos_tcp_flag_t;

/**
 @brief ACL/Qos layer 4 port
*/
struct ctc_aclqos_l4_port_s
{
    ctc_aclqos_l4port_operator_t operator;  /**< CTC_ACLQOS_L4PORT_OPERATOR_XXX */
    uint16 l4_port_min;                     /**< First port */
    uint16 l4_port_max;                     /**< Second port */
};
typedef struct ctc_aclqos_l4_port_s ctc_aclqos_l4_port_t;

/**
 @brief ACL/Qos log session
*/
enum ctc_aclqos_log_session_e
{
    CTC_ACLQOS_LOG_SESSION_0,           /**< Log session Id 0 */
    CTC_ACLQOS_LOG_SESSION_1,           /**< Log session Id 1 */
    CTC_ACLQOS_LOG_SESSION_2,           /**< Log session Id 2 */
    CTC_ACLQOS_LOG_SESSION_3,           /**< Log session Id 3, reserved for copy to cpu */

    MAX_CTC_ACLQOS_SESSION
};
typedef enum ctc_aclqos_log_session_e ctc_aclqos_log_session_t;

/**
 @brief ACL/Qos mac key flag
*/
enum ctc_aclqos_mac_key_flag_e
{
    CTC_ACLQOS_MAC_KEY_MACDA_FLAG    = 1U << 0,     /**< Flag to indicate MAC-DA is set in MAC key */
    CTC_ACLQOS_MAC_KEY_MACSA_FLAG    = 1U << 1,     /**< Flag to indicate MAC-SA is set in MAC key */
    CTC_ACLQOS_MAC_KEY_COS_FLAG      = 1U << 2,     /**< Flag to indicate CoS is set in MAC key */
    CTC_ACLQOS_MAC_KEY_CVLAN_FLAG    = 1U << 3,     /**< Flag to indicate C-VLAN is set in MAC key */
    CTC_ACLQOS_MAC_KEY_CTAG_COS_FLAG = 1U << 4,     /**< Flag to indicate C-tag CoS is set in MAC key */
    CTC_ACLQOS_MAC_KEY_SVLAN_FLAG    = 1U << 5,     /**< Flag to indicate S-VLAN is set in MAC key */
    CTC_ACLQOS_MAC_KEY_STAG_COS_FLAG = 1U << 6,     /**< Flag to indicate S-tag CoS is set in MAC key */
    CTC_ACLQOS_MAC_KEY_ETHTYPE_FLAG  = 1U << 7,     /**< Flag to indicate eth-type is set in MAC key */
    CTC_ACLQOS_MAC_KEY_L2TYPE_FLAG   = 1U << 8,     /**< Flag to indicate l2-type is set in MAC key */
    CTC_ACLQOS_MAC_KEY_L3TYPE_FLAG   = 1U << 9,     /**< Flag to indicate l3-type is set in MAC key */
    CTC_ACLQOS_MAC_KEY_CTAG_CFI_FLAG = 1U << 10,    /**< Flag to indicate l2-type is set in MAC key */
    CTC_ACLQOS_MAC_KEY_STAG_CFI_FLAG = 1U << 11     /**< Flag to indicate l3-type is set in MAC key */
};

/**
 @brief ACL/Qos mpls key flag
*/
enum ctc_aclqos_mpls_key_flag_e
{
    CTC_ACLQOS_MPLS_KEY_MACDA_FLAG        = 1U << 0,    /**< Flag to indicate MAC-DA is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_MACSA_FLAG        = 1U << 1,    /**< Flag to indicate MAC-SA is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_COS_FLAG          = 1U << 2,    /**< Flag to indicate CoS is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_CVLAN_FLAG        = 1U << 3,    /**< Flag to indicate C-VLAN is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_CTAG_COS_FLAG     = 1U << 4,    /**< Flag to indicate C-tag CoS is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_SVLAN_FLAG        = 1U << 5,    /**< Flag to indicate S-VLAN is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_STAG_COS_FLAG     = 1U << 6,    /**< Flag to indicate S-tag CoS is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_L2TYPE_FLAG       = 1U << 7,    /**< Flag to indicate l2-type is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_LABEL0_FLAG       = 1U << 8,    /**< Flag to indicate MPLS label 0 is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_LABEL1_FLAG       = 1U << 9,    /**< Flag to indicate MPLS label 1 is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_LABEL2_FLAG       = 1U << 10,   /**< Flag to indicate MPLS label 2 is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_LABEL3_FLAG       = 1U << 11,   /**< Flag to indicate MPLS label 3 is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_ROUTEDPKT_FLAG    = 1U << 12,   /**< Flag to indicate Routed-packet is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_CTAG_CFI_FLAG     = 1U << 13,   /**< Flag to indicate C-tag CoS is set in MPLS key */
    CTC_ACLQOS_MPLS_KEY_STAG_CFI_FLAG     = 1U << 14    /**< Flag to indicate S-tag CoS is set in MPLS key */
};

/**
 @brief ACL/Qos ipv4 key flag
*/
enum ctc_aclqos_ipv4_key_flag_e
{
    CTC_ACLQOS_IPV4_KEY_MACDA_FLAG     = 1U << 0,   /**< Flag to indicate MAC-DA is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_MACSA_FLAG     = 1U << 1,   /**< Flag to indicate MAC-SA is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_COS_FLAG       = 1U << 2,   /**< Flag to indicate CoS is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_CVLAN_FLAG     = 1U << 3,   /**< Flag to indicate C-VLAN is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_CTAG_COS_FLAG  = 1U << 4,   /**< Flag to indicate C-tag CoS is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_SVLAN_FLAG     = 1U << 5,   /**< Flag to indicate S-VLAN is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_STAG_COS_FLAG  = 1U << 6,   /**< Flag to indicate S-tag CoS is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_L2TYPE_FLAG    = 1U << 7,   /**< Flag to indicate l2-type is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_L3TYPE_FLAG    = 1U << 8,   /**< Flag to indicate l3-type is set in IPv4 key */

    CTC_ACLQOS_IPV4_KEY_IPSA_FLAG      = 1U << 9,   /**< Flag to indicate IP-SA is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_IPDA_FLAG      = 1U << 10,  /**< Flag to indicate IP-DA is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_L4PROTO_FLAG   = 1U << 11,  /**< Flag to indicate L4-Proto is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_L4SRCPORT_FLAG = 1U << 12,  /**< Flag to indicate L4 source port is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_L4DSTPORT_FLAG = 1U << 13,  /**< Flag to indicate L4 destination port is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_TCPFLAG_FLAG   = 1U << 14,  /**< Flag to indicate TCP flag is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_ICMPTYPE_FLAG  = 1U << 15,  /**< Flag to indicate ICMP type is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_ICMPCODE_FLAG  = 1U << 16,  /**< Flag to indicate ICMP code is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_IGMPTYPE_FLAG  = 1U << 17,  /**< Flag to indicate IGMP type is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_DSCP_FLAG      = 1U << 18,  /**< Flag to indicate DSCP is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_PREC_FLAG      = 1U << 19,  /**< Flag to indicate IP Precedence is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_FRAG_FLAG      = 1U << 20,  /**< Flag to indicate fragment is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_OPTION_FLAG    = 1U << 21,  /**< Flag to indicate IP option is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_ROUTEDPKT_FLAG = 1U << 22,  /**< Flag to indicate routed packet is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_CTAG_CFI_FLAG  = 1U << 23,  /**< Flag to indicate C-tag CoS is set in IPv4 key */
    CTC_ACLQOS_IPV4_KEY_STAG_CFI_FLAG  = 1U << 24   /**< Flag to indicate S-tag CoS is set in IPv4 key */
};


/**
 @brief ACL/Qos ipv6 key flag
*/
enum ctc_aclqos_ipv6_key_flag_e
{
    CTC_ACLQOS_IPV6_KEY_MACDA_FLAG     = 1U << 0,   /**< Flag to indicate MAC-DA is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_MACSA_FLAG     = 1U << 1,   /**< Flag to indicate MAC-SA is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_COS_FLAG       = 1U << 2,   /**< Flag to indicate CoS is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_CVLAN_FLAG     = 1U << 3,   /**< Flag to indicate C-VLAN is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_CTAG_COS_FLAG  = 1U << 4,   /**< Flag to indicate C-tag CoS is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_SVLAN_FLAG     = 1U << 5,   /**< Flag to indicate S-VLAN is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_STAG_COS_FLAG  = 1U << 6,   /**< Flag to indicate S-tag CoS is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_ETHTYPE_FLAG   = 1U << 7,   /**< Flag to indicate eth-type is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_L2TYPE_FLAG    = 1U << 8,   /**< Flag to indicate l2-type CoS is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_L3TYPE_FLAG    = 1U << 9,   /**< Flag to indicate l3-type is set in IPv6 key */

    CTC_ACLQOS_IPV6_KEY_IPSA_FLAG      = 1U << 10,  /**< Flag to indicate IPv6-SA is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_IPDA_FLAG      = 1U << 11,  /**< Flag to indicate IPv6-DA is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_L4PROTO_FLAG   = 1U << 12,  /**< Flag to indicate L4-Proto is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_L4SRCPORT_FLAG = 1U << 13,  /**< Flag to indicate L4 source port is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_L4DSTPORT_FLAG = 1U << 14,  /**< Flag to indicate L4 destination port is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_TCPFLAG_FLAG   = 1U << 15,  /**< Flag to indicate TCP flag is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_ICMPTYPE_FLAG  = 1U << 16,  /**< Flag to indicate ICMP type is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_ICMPCODE_FLAG  = 1U << 17,  /**< Flag to indicate ICMP code is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_DSCP_FLAG      = 1U << 18,  /**< Flag to indicate DSCP is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_PREC_FLAG      = 1U << 19,  /**< Flag to indicate IP Precedence is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_FRAG_FLAG      = 1U << 20,  /**< Flag to indicate fragment is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_OPTION_FLAG    = 1U << 21,  /**< Flag to indicate IP option is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_ROUTEDPKT_FLAG = 1U << 22,  /**< Flag to indicate routed-packet is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_EXTHDR_FLAG    = 1U << 23,  /**< Flag to indicate IPv6 extension header is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_FLOWLABEL_FLAG = 1U << 24,  /**< Flag to indicate IPv6 flow label is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_CTAG_CFI_FLAG  = 1U << 25,  /**< Flag to indicate C-tag CoS is set in IPv6 key */
    CTC_ACLQOS_IPV6_KEY_STAG_CFI_FLAG  = 1U << 26   /**< Flag to indicate S-tag CoS is set in IPv6 key */
};


/**
 @brief ACL pbr ipv4 key flag
*/
enum ctc_acl_pbr_ipv4_key_flag_e
{
    CTC_ACL_PBR_IPV4_KEY_IPDA_FLAG      = 1U << 0,  /**< Flag to indicate IP-DA is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_IPSA_FLAG      = 1U << 1,  /**< Flag to indicate IP-SA is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_VRFID_FLAG     = 1U << 2,  /**< Flag to indicate vrfid is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_DSCP_FLAG      = 1U << 3,  /**< Flag to indicate dscp is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_PREC_FLAG      = 1U << 4,  /**< Flag to indicate IP Precedence is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_L4SRCPORT_FLAG = 1U << 5,  /**< Flag to indicate L4 source port is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_L4DSTPORT_FLAG = 1U << 6,  /**< Flag to indicate L4 destination port is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_L4PROTO_FLAG   = 1U << 7,  /**< Flag to indicate L4-Proto is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_FRAG_FLAG      = 1U << 8,  /**< Flag to indicate fragment is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_TCPFLAG_FLAG   = 1U << 9,  /**< Flag to indicate TCP flag is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_ICMPTYPE_FLAG  = 1U << 10, /**< Flag to indicate ICMP type is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_ICMPCODE_FLAG  = 1U << 11, /**< Flag to indicate ICMP code is set in IPv4 key */
    CTC_ACL_PBR_IPV4_KEY_LENGTH_FLAG    = 1U << 12  /**< Flag to indicate length index is set in IPv4 key */
};


/**
 @brief ACL pbr ipv6 key flag
*/
enum ctc_acl_pbr_ipv6_key_flag_e
{
    CTC_ACL_PBR_IPV6_KEY_IPDA_FLAG      = 1U << 0,  /**< Flag to indicate IP-DA is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_IPSA_FLAG      = 1U << 1,  /**< Flag to indicate IP-SA is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_VRFID_FLAG     = 1U << 2,  /**< Flag to indicate vrfid is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_DSCP_FLAG      = 1U << 3,  /**< Flag to indicate dscp is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_PREC_FLAG      = 1U << 4,  /**< Flag to indicate IP Precedence is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_L4SRCPORT_FLAG = 1U << 5,  /**< Flag to indicate L4 source port is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_L4DSTPORT_FLAG = 1U << 6,  /**< Flag to indicate L4 destination port is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_L4PROTO_FLAG   = 1U << 7,  /**< Flag to indicate L4-Proto is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_FRAG_FLAG      = 1U << 8,  /**< Flag to indicate fragment is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_TCPFLAG_FLAG   = 1U << 9,  /**< Flag to indicate TCP flag is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_ICMPTYPE_FLAG  = 1U << 10, /**< Flag to indicate ICMP type is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_ICMPCODE_FLAG  = 1U << 11, /**< Flag to indicate ICMP code is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_EXTHDR_FLAG    = 1U << 12, /**< Flag to indicate IPv6 extension header is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_FLOWLABEL_FLAG = 1U << 13, /**< Flag to indicate IPv6 flow label is set in IPv6 key */
    CTC_ACL_PBR_IPV6_KEY_LENGTH_FLAG    = 1U << 14  /**< Flag to indicate length index is set in IPv6 key */
};


/**
 @brief ACL/Qos action flag
*/
enum ctc_aclqos_action_flag_e
{
    CTC_ACLQOS_ACTION_DISCARD_FLAG           = 1U << 0,     /**< Discard the packet */
    CTC_ACLQOS_ACTION_DENY_BRIDGE_FLAG       = 1U << 1,     /**< Don't do bridging process for the packet */
    CTC_ACLQOS_ACTION_DENY_LEARN_FLAG        = 1U << 2,     /**< Don't do learning process for the packet */
    CTC_ACLQOS_ACTION_DENY_ROUTE_FLAG        = 1U << 3,     /**< Don't do routing process for the packet */
    CTC_ACLQOS_ACTION_DENY_REPLACE_COS_FLAG  = 1U << 4,     /**< Don't replace CoS in the packet header */
    CTC_ACLQOS_ACTION_DENY_REPLACE_DSCP_FLAG = 1U << 5,     /**< Don't replace DSCP in the packet header */
    CTC_ACLQOS_ACTION_STATS_FLAG             = 1U << 6,     /**< Accounting */
    CTC_ACLQOS_ACTION_FLOW_ID_FLAG           = 1U << 7,     /**< Set flow id for the packet */
    CTC_ACLQOS_ACTION_PRIORITY_FLAG          = 1U << 8,     /**< Set priority color for the packet */
    CTC_ACLQOS_ACTION_TRUST_FLAG             = 1U << 9,     /**< Set QoS trust state for the packet */
    CTC_ACLQOS_ACTION_FLOW_POLICER_FLAG      = 1U << 10,    /**< Set flow policer for the packet */
    CTC_ACLQOS_ACTION_RAND_LOG_FLAG          = 1U << 11,    /**< Log to any network port for the packet */
    CTC_ACLQOS_ACTION_COPY_TO_CPU_FLAG       = 1U << 12,    /**< Log to any network port for the packet */
    CTC_ACLQOS_ACTION_REDIRECT_FLAG          = 1U << 13,    /**< Set forward action for the packet */
    CTC_ACLQOS_ACTION_INVALID_FLAG           = 1U << 14,    /**< Invalidate an entry */
    CTC_ACLQOS_ACTION_PBR_TTL_CHECK_FLAG     = 1U << 15,    /**< Set PBR ttl-check flag */
    CTC_ACLQOS_ACTION_PBR_ICMP_CHECK_FLAG    = 1U << 16,    /**< Set PBR icmp-check flag */
    CTC_ACLQOS_ACTION_PBR_COPY_TO_CPU_FLAG   = 1U << 17,    /**< Set PBR copy-to-cpu flag */
    CTC_ACLQOS_ACTION_PBR_ECMP_FLAG          = 1U << 18,    /**< Set PBR ecmp flag that user use ecmp nexthop directly */
    CTC_ACLQOS_ACTION_PBR_FWD_FLAG           = 1U << 19,    /**< Set PBR fwd flag */
    CTC_ACLQOS_ACTION_PBR_DENY_FLAG          = 1U << 20     /**< Set PBR deny flag */
};


/**
 @brief ACL/Qos mac key
*/
struct ctc_aclqos_mac_key_s
{
    uint32 flag;                /**< Bitmap of CTC_ACLQOS_MAC_KEY_XXX_FLAG */

    mac_addr_t mac_da;          /**< MAC-DA */
    mac_addr_t mac_da_mask;     /**< MAC-DA mask */
    mac_addr_t mac_sa;          /**< MAC-SA */
    mac_addr_t mac_sa_mask;     /**< MAC-SA mask */
    uint16 cvlan;               /**< C-VLAN */
	uint16 cvlan_mask;          /**< C-VLAN mask*/
    uint16 svlan;               /**< S-VLAN */
	uint16 svlan_mask;          /**< C-VLAN mask*/
    uint8  cvlan_mask_valid;    /**< C-VLAN mask valid */
    uint8  svlan_mask_valid;    /**< S-VLAN mask valid */
    uint8  ctag_cos;            /**< C-tag CoS */
    uint8  ctag_cfi;            /**< C-tag CFI */
    uint8  stag_cos;            /**< S-tag CoS */
    uint8  stag_cfi;            /**< S-tag CFI */
    uint8  l2_type;             /**< Layer 2 type */
    uint8  l3_type;             /**< Layer 3 type */
    uint16 eth_type;            /**< Ethernet type */
	uint16 eth_type_mask;       /**< Ethernet type mask*/
    uint8  eth_type_mask_valid; /**< Ethernet type mask valid*/
    uint8  cos;                 /**< Packet_info.src_cos in IPE, packet_info.new_cos in EPE */
    uint8  rsv[2];
};
typedef struct ctc_aclqos_mac_key_s  ctc_aclqos_mac_key_t;


/**
 @brief ACL/Qos mpls key
*/
struct ctc_aclqos_mpls_key_s
{
    uint32 flag;                /**< Bitmap of CTC_ACLQOS_MPLS_KEY_XXX_FLAG */

    mac_addr_t mac_da;          /**< MAC-DA */
    mac_addr_t mac_da_mask;     /**< MAC-DA mask */
    mac_addr_t mac_sa;          /**< MAC-SA */
    mac_addr_t mac_sa_mask;     /**< MAC-SA mask */
    uint16 cvlan;               /**< C-VLAN tag */
	uint16 cvlan_mask;          /**< C-VLAN mask*/
    uint16 svlan;               /**< S-VLAN tag */
	uint16 svlan_mask;          /**< S-VLAN mask*/
    uint8  cvlan_mask_valid;    /**< C-VLAN mask valid */
    uint8  svlan_mask_valid;    /**< S-VLAN mask valid */
    uint8  ctag_cos;            /**< C-tag CoS */
    uint8  stag_cos;            /**< S-tag Cos */
    uint8  ctag_cfi;            /**< C-tag Cfi */
    uint8  stag_cfi;            /**< S-tag Cfi */
    uint8  l2_type;             /**< Layer 2 type */
    uint8  cos;                 /**< Packet_info.src_cos in IPE, packet_info.new_cos in EPE */
    uint8  routed_packet;       /**< Routed packet */
    uint8  rsv1[3];

    uint32 mpls_label0;         /**< MPLS label 0 */
    uint32 mpls_label0_mask;    /**< MPLS label 0 mask*/
    uint32 mpls_label1;         /**< MPLS label 1 */
    uint32 mpls_label1_mask;    /**< MPLS label 1 mask*/
    uint32 mpls_label2;         /**< MPLS label 2 */
    uint32 mpls_label2_mask;    /**< MPLS label 2 mask*/
    uint32 mpls_label3;         /**< MPLS label 3 */
    uint32 mpls_label3_mask;    /**< MPLS label 3 mask*/
};
typedef struct ctc_aclqos_mpls_key_s  ctc_aclqos_mpls_key_t;


/**
 @brief ACL/Qos ipv4 key
*/
struct ctc_aclqos_ipv4_key_s
{
    uint32 flag;                        /**< Bitmap of CTC_ACLQOS_IPV4_KEY_XXX_FLAG */

    uint32 ip_sa;                       /**< IP-SA */
    uint32 ip_sa_mask;                  /**< IP-SA mask */
    uint32 ip_da;                       /**< IP-DA */
    uint32 ip_da_mask;                  /**< IP-DA mask */
    ctc_aclqos_l4_port_t l4_src_port;   /**< Layer 4 source port */
    ctc_aclqos_l4_port_t l4_dst_port;   /**< Layer 4 destination port */
    ctc_aclqos_ip_frag_t ip_frag;       /**< Ip fragment */
    ctc_aclqos_tcp_flag_t tcp_flag;     /**< Tcp flag */
    uint8  dscp;                        /**< DSCP */
    uint8  l4_protocol;                 /**< Layer 4 protocol */
    uint8  icmp_type;                   /**< ICMP type */
    uint8  icmp_code;                   /**< ICMP code */
    uint8  igmp_type;                   /**< IGMP type */
    uint8  routed_packet;               /**< Routed packet */
    uint16 rsv1;

    mac_addr_t mac_sa;                  /**< MAC-SA */
    mac_addr_t mac_sa_mask;             /**< MAC-SA mask */
    mac_addr_t mac_da;                  /**< MAC-DA */
    mac_addr_t mac_da_mask;             /**< MAC-DA mask */
    uint16 cvlan;                       /**< C-VLAN */
	uint16 cvlan_mask;                  /**< C-VLAN mask*/
    uint16 svlan;                       /**< S-VLAN */
	uint16 svlan_mask;                  /**< S-VLAN mask*/
    uint8  cvlan_mask_valid;            /**< C-VLAN mask valid */
    uint8  svlan_mask_valid;            /**< S-VLAN mask valid */
    uint8  ctag_cos;                    /**< C-tag CoS */
    uint8  stag_cos;                    /**< S-tag CoS */
    uint8  ctag_cfi;                    /**< C-tag CFI */
    uint8  stag_cfi;                    /**< S-tag CFI */
    uint8  cos;                         /**< Packet_info.src_cos in IPE, packet_info.new_cos in EPE */
    uint8  l2_type;                     /**< Layer 2 type */
    uint8  l3_type;                     /**< Layer 3 type */
    uint8  rsv2[3];
};
typedef struct ctc_aclqos_ipv4_key_s  ctc_aclqos_ipv4_key_t;

/**
 @brief ACL/Qos ipv6 key
*/
struct ctc_aclqos_ipv6_key_s
{
    uint32 flag;                        /**< Bitmap of CTC_ACLQOS_IPV6_KEY_XXX_FLAG */

    ipv6_addr_t ip_sa;                  /**< IPv6-SA */
    ipv6_addr_t ip_sa_mask;             /**< IPv6-SA mask */
    ipv6_addr_t ip_da;                  /**< IPv6-DA */
    ipv6_addr_t ip_da_mask;             /**< IPv6-DA mask */
    ctc_aclqos_l4_port_t l4_src_port;   /**< Layer 4 source port */
    ctc_aclqos_l4_port_t l4_dst_port;   /**< Layer 4 destination port */
    ctc_aclqos_ip_frag_t ip_frag;       /**< IP fragment */
    ctc_aclqos_tcp_flag_t tcp_flag;     /**< TCP flag */
    uint32 flow_label;                  /**< Flow label */
    uint8  l4_protocol;                 /**< Layer 4 protocol */
    uint8  icmp_type;                   /**< ICMP type */
    uint8  icmp_code;                   /**< ICMP code */
    uint8  dscp;                        /**< DSCP */
    uint8  ext_hdr;                     /**< Extension header ,Bitmap of ctc_parser_ipv6_extend_header_type_t*/
    uint8  ip_option;                   /**< IP option */
    uint8  routed_packet;               /**< Routed packet */
    uint8  rsv1;

    mac_addr_t mac_sa;                  /**< MAC-SA */
    mac_addr_t mac_sa_mask;             /**< MAC-SA mask */
    mac_addr_t mac_da;                  /**< MAC-DA */
    mac_addr_t mac_da_mask;             /**< MAC-DA mask */
    uint16 eth_type;                    /**< Ethernet type */
    uint16 cvlan;                       /**< C-VLAN */
	uint16 cvlan_mask;                  /**< C-VLAN mask*/
    uint16 svlan;                       /**< S-VLAN */
	uint16 svlan_mask;                  /**< S-VLAN mask*/
    uint8  cvlan_mask_valid;            /**< C-VLAN mask valid */
    uint8  svlan_mask_valid;            /**< S-VLAN mask valid */
    uint8  ctag_cos;                    /**< C-tag CoS */
    uint8  stag_cos;                    /**< S-tag CoS */
    uint8  ctag_cfi;                    /**< C-tag CFI */
    uint8  stag_cfi;                    /**< S-tag CFI */
    uint8  l2_type;                     /**< Layer 2 type */
    uint8  l3_type;                     /**< Layer 3 type */
    uint8  cos;                         /**< Packet_info.src_cos in IPE, packet_info.new_cos in EPE */
    uint8  rsv2;
};
typedef struct ctc_aclqos_ipv6_key_s ctc_aclqos_ipv6_key_t;

/**
 @brief ACL pbr ipv4 key
*/
struct ctc_acl_pbr_ipv4_key_s
{
    uint32 flag;                        /**< Bitmap of CTC_ACL_PBR_IPV4_KEY_XXX_FLAG */

    uint32 ip_sa;                       /**< IP-SA */
    uint32 ip_sa_mask;                  /**< IP-SA mask */
    uint32 ip_da;                       /**< IP-DA */
    uint32 ip_da_mask;                  /**< IP-DA mask */
    ctc_aclqos_l4_port_t l4_src_port;   /**< Layer 4 source port */
    ctc_aclqos_l4_port_t l4_dst_port;   /**< Layer 4 destination port */
    ctc_aclqos_ip_frag_t ip_frag;       /**< Ip fragment */
    ctc_aclqos_tcp_flag_t tcp_flag;     /**< Tcp flag */
    uint16 vrf_id;                      /**< VRFID */
    uint8  dscp;                        /**< DSCP */
    uint8  l4_protocol;                 /**< Layer 4 protocol */
    uint8  icmp_type;                   /**< ICMP type */
    uint8  icmp_code;                   /**< ICMP code */
    uint8  len_index;                   /**< length index */
    uint8  rsv1;
};
typedef struct ctc_acl_pbr_ipv4_key_s  ctc_acl_pbr_ipv4_key_t;

/**
 @brief ACL pbr ipv6 key
*/
struct ctc_acl_pbr_ipv6_key_s
{
    uint32 flag;                        /**< Bitmap of CTC_ACL_PBR_IPV6_KEY_XXX_FLAG */

    ipv6_addr_t ip_sa;                  /**< IPv6-SA */
    ipv6_addr_t ip_sa_mask;             /**< IPv6-SA mask */
    ipv6_addr_t ip_da;                  /**< IPv6-DA */
    ipv6_addr_t ip_da_mask;             /**< IPv6-DA mask */
    ctc_aclqos_l4_port_t l4_src_port;   /**< Layer 4 source port */
    ctc_aclqos_l4_port_t l4_dst_port;   /**< Layer 4 destination port */
    ctc_aclqos_ip_frag_t ip_frag;       /**< IP fragment */
    ctc_aclqos_tcp_flag_t tcp_flag;     /**< TCP flag */
    uint32 flow_label;                  /**< Flow label */
    uint16 vrf_id;          /**< VRFID */
    uint8  dscp;            /**< DSCP */
    uint8  l4_protocol;     /**< Layer 4 protocol */
    uint8  icmp_type;       /**< ICMP type */
    uint8  icmp_code;       /**< ICMP code */
    uint8  ext_hdr;         /**< Extension header ,Bitmap of ctc_parser_ipv6_extend_header_type_t*/
    uint8  ip_option;       /**< IP option */
    uint8  len_index;       /**< length index */
    uint8  rsv1[3];
};
typedef struct ctc_acl_pbr_ipv6_key_s  ctc_acl_pbr_ipv6_key_t;

/**
 @brief ACL/Qos key
*/
struct ctc_aclqos_key_s
{
    ctc_aclqos_key_type_t type;                 /**< CTC_ACLQOS_XXX_KEY */

    union
    {
        ctc_aclqos_mac_key_t  mac_key;          /**< ACL/QoS MAC key content */
        ctc_aclqos_mpls_key_t mpls_key;         /**< ACL/QoS MPLS key content */
        ctc_aclqos_ipv4_key_t ipv4_key;         /**< ACL/QoS IPv4 key content */
        ctc_aclqos_ipv6_key_t ipv6_key;         /**< ACL/QoS IPv6 key content */
        ctc_acl_pbr_ipv4_key_t pbr_ipv4_key;    /**< ACL PBR IPv4 key content */
        ctc_acl_pbr_ipv6_key_t pbr_ipv6_key;    /**< ACL PBR IPv6 key content */
    } key_info;
};
typedef struct ctc_aclqos_key_s ctc_aclqos_key_t;


/**
 @brief ACL/Qos action
*/
struct ctc_aclqos_action_s
{
    uint32 flag;            /**< Bitmap of CTC_ACLQOS_ACTION_XXX_FLAG */

    union
    {
        uint32 fwd_nh_id;   /**< Forward nexthop ID, shared by acl, qos and pbr*/
        uint32 fwd_reason;  /**< Forward reason */
    } fwd;

    uint32 policer_id;      /**< Flow policer ID */
    ctc_qos_trust_t trust;  /**< QoS trust state */
    ctc_qos_color_t color;  /**< Color: green, yellow, or red */
    uint8  flow_id;         /**< Use for en-queue, TBD */
    uint8  priority;        /**< Priority: 0 - 63 */
    uint8  log_weight;      /**< Logging weight */
    uint8  log_session_id;  /**< CTC_ACLQOS_LOG_SESSION_XXX */

    uint16 stats_ptr;
    uint8  rsv[2];
};
typedef struct ctc_aclqos_action_s ctc_aclqos_action_t;


/**
 @brief ACL/Qos entry
*/
struct ctc_aclqos_entry_s
{
    uint32 entry_id;            /**< Uniquely identifies an entry in the same ACL/QoS label */
    ctc_aclqos_action_t action; /**< ACL/QoS action */
    ctc_aclqos_key_t key;       /**< ACL/QoS key content */
};
typedef struct ctc_aclqos_entry_s ctc_aclqos_entry_t;

/**
 @brief ACL/Qos entry operation
*/
struct ctc_aclqos_entry_oper_s
{
    uint32 label_id;                    /**<ACL/QOS label identify*/
    ctc_aclqos_label_type_t label_type; /**< CTC_XXX_LABEL */
    uint32 pre_entry_id;                /**<Previous entry identify*/
    ctc_aclqos_entry_t entry;           /**<Data to store entry information*/
};
typedef struct ctc_aclqos_entry_oper_s ctc_aclqos_entry_oper_t;


/**
 @brief ACL/QOS entry sort mode
*/
enum ctc_aclqos_entry_sort_mode_e
{
    CTC_ACLQOS_ENTRY_SORT_MODE_PER_LABEL,           /**< acl/qos entry sort by entryid in a label */
    CTC_ACLQOS_ENTRY_SORT_MODE_PER_SYSTEM,           /**< acl/qos entry sort by entryid in whole system */

    MAX_CTC_ACLQOS_ENTRY_SORT_MODE
};
typedef enum ctc_aclqos_entry_sort_mode_e ctc_aclqos_entry_sort_mode_t;


/**
 @brief ACL/Qos init
*/
struct ctc_aclqos_global_cfg_s
{
    uint32 acl_redirect_fwd_ptr_num;        /**< The number of ds_fwd_ptr reserved for acl redirect function */
    uint32 global_aclqos_entry_head_num;    /**< The number of global aclqos entry, which is located on head */
    uint32 global_aclqos_entry_tail_num;    /**< The number of global aclqos entry, which is located on tail */
    uint32 global_pbr_entry_head_num;       /**< The number of global pbr entry, which is located on head */
    uint32 global_pbr_entry_tail_num;       /**< The number of global pbr entry, which is located on tail */
    uint8  entry_sort_mode;                 /**< CTC_ACLQOS_ENTRY_SORT_MODE_XXX */
};
typedef struct ctc_aclqos_global_cfg_s ctc_aclqos_global_cfg_t;


/**@} end of @defgroup aclqos ACLQoS  */

#endif

