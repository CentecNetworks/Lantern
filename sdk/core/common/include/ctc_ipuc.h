/**
 @file ctc_ipuc.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2011-11-07

 @version v2.0

   This file contains all ipuc related data structure, enum, macro and proto.
*/

 #ifndef _CTC_IPUC_H
 #define _CTC_IPUC_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/**
 @defgroup ipuc IPUC
 @{
*/

/**
 @brief  Define ipuc route flags
*/
enum ctc_ipuc_flag_e
{
    CTC_IPUC_FLAG_RPF_CHECK          = 0x01,      /**< This route will do RPF check */
    CTC_IPUC_FLAG_TTL_CHECK          = 0x02,      /**< This route will do ttl check */
    CTC_IPUC_FLAG_ICMP_CHECK         = 0x04,      /**< This route will do icmp redirect check */
    CTC_IPUC_FLAG_CPU                = 0x08,      /**< Packets hitting this route will be copied to CPU */
    CTC_IPUC_FLAG_NEIGHBOR           = 0x10,      /**< This is a ARP entry, only set when mask length is 32 on IPv4 or 128 on IPv6 */
    CTC_IPUC_FLAG_CONNECT            = 0x20,      /**< This is a CONNECT entry */
    CTC_IPUC_FLAG_PROTOCOL_ENTRY     = 0x40,      /**< If this flag is set, excp3_ctl will be set for l3pdu to cpu */
    MAX_CTC_IPUC_FLAG                = 0x7f

};

/**
 @brief  Define ipuc route flags
*/
enum ctc_ipuc_route_ctl_flag_e
{
    CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_EN      = 0x01, /**< Change martian_check_en_bit of IPEIpucRouteCtl */
    CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_DISABLE = 0x02, /**< Change martian_addr_check_dis of IPEIpucRouteCtl */
    CTC_IPUC_ROUTE_CTL_MCAST_ADDRESS_CHECK   = 0x04, /**< Change mcast_addr_match_check_dis of IPEIpucRouteCtl */
    CTC_IPUC_ROUTE_CTL_TTL_LIMIT             = 0x08, /**< Change ip_ttl_limit of IPEIpucRouteCtl */
    MAX_CTC_IPUC_ROUTE_CTL                   = 0x0f
};

/**
 @brief  Define ipuc route flags
*/
enum ctc_ipuc_lookup_ctl_flag_e
{
    CTC_IPUC_lookup_CTL_IPSA_LOOKUP_EN       = 0x01, /**< Change IPSA lookup ctl of IPEIpuclookupCtl */
    CTC_IPUC_IPv6_lookup_CTL_IPSA_LOOKUP_EN  = 0x02, /**< Change IPSA lookup ctl of IPEIpuclookupCtl */
    MAX_CTC_IPUC_LOOKUP_CTL                  = 0x0f
};

/**
 @brief  the packet layer4 type
*/
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_NONE   0x00    /**<0: payload layer4 TYPE NONE */
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_TCP    0x01    /**<1: payload layer4 TYPE TCP*/
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_UDP    0x02    /**<2: payload layer4 TYPE UDP*/
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE    0x03    /**<3: payload layer4 TYPE GRE*/
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV4   0x04    /**<4: payload layer4 TYPE IPV4*/
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV6   0x05    /**<5: payload layer4 TYPE IPV6 */
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_MAX    0x06
#define    CTC_IPUC_TUNNEL_PAYLOAD_TYPE_FLX    0x0f


/**
 @brief  Define tunnel_info structure used in ipuc parameter
*/
struct ctc_tunnel_info_s
{
    uint32 gre_key;             /**< GRE key  */
    uint8 tunnel_packet_type;   /**< set pld type */
    uint8 is_set_gre_key;       /**< GRE encap information */
    uint16 rsv;
};
typedef struct ctc_tunnel_info_s ctc_tunnel_info_t;

/**
 @brief  Define ipuc parameter structure
*/
struct ctc_ipuc_param_s
{
    uint32 nh_id;          /**< Nexthop ID */
    uint16 vrf_id;         /**< Vrf ID of the route */
    uint16 l3_inf;         /**< l3 interface, only valid when CTC_IPUC_FLAG_CONNECT is set ; tunnel RPF ICMP check*/
    uint8 route_flag;      /**< Flags of the route, values is defined by ctc_ipuc_flag_e, one route can set one or more flags, or no flag */
    uint8 masklen;         /**< Mask length of destination */
    uint8 ip_ver;          /**< Destination ip address version, CTC_IP_VER_4 or CTC_IP_VER_6 */
    uint8 is_ecmp_nh;      /**< Nexthop is a ECMP group */
    union
    {
        ip_addr_t ipv4;             /**< IPv4 destination address */
        ipv6_addr_t ipv6;           /**< IPv6 destination address */
    }ip;
    ctc_tunnel_info_t tunnel_info;  /**< IP tunnel information of the route */
    uint8 is_tunnel;                /**< if set is tunnel packet */
    uint8 is_ipsa;                  /**< if enable set ip sa */
    uint8 is_tunnel_l3if;           /**< if enable tunnel can do rpf and ICMP check */
    uint8 rsv;                      /**< resvered */
    union
    {
        ip_addr_t ipv4;             /**< IPv4 sourec address */
        ipv6_addr_t ipv6;           /**< IPv6 sourec address */
    }ip_sa;
};
typedef struct ctc_ipuc_param_s ctc_ipuc_param_t;

struct ctc_ipuc_route_ctl_s
{
    uint32 valid_flag;                           /**< Flags indicate the valid members */

    uint16 martian_check_en;                     /**< New value of martian_check_en_bit, valid when CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_EN set */
    uint8 martian_check_disable;                 /**< New value of martian_addr_check_dis, valid when CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_DISABLE set */
    uint8 mcast_address_match_check_disable;     /**< New value of mcast_addr_match_check_dis, valid when CTC_IPUC_ROUTE_CTL_MCAST_ADDRESS_CHECK set */
    uint8 ip_ttl_limit;                          /**< New value of ip_ttl_limit, valid when CTC_IPUC_ROUTE_CTL_TTL_LIMIT set */
};
typedef struct ctc_ipuc_route_ctl_s ctc_ipuc_route_ctl_t;


struct ctc_ipuc_lookup_ctl_s
{
    uint16 valid_flag;                           /**< Flags indicate the valid members */

    uint8 ipv4_ucast_route_key_sa_en;            /**< set ucast route key sa look up status */
    uint8 ipv6_ucast_route_key_sa_en;            /**< set IPv6 ucast route key sa look up status */
};
typedef struct ctc_ipuc_lookup_ctl_s ctc_ipuc_lookup_ctl_t;
/**@} end of @defgroup  ipuc IPUC */

#endif  /*_CTC_IPUC_H*/
