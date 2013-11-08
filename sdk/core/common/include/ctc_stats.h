/**
 @file ctc_stats.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

   This file contains all stats related data structure, enum, macro and proto.
*/


 #ifndef _CTC_STATS_H
 #define _CTC_STATS_H
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
 /**
  @defgroup stats STATS
  @{
 */

/**
 @brief  Define stats type
*/
 enum ctc_stats_type_e
 {
     CTC_STATS_TYPE_IGS_PORT_PHB,
     CTC_STATS_TYPE_EGS_PORT_PHB,
     CTC_STATS_TYPE_IGS_GLOBAL_FWD,
     CTC_STATS_TYPE_EGS_GLOBAL_FWD,
     CTC_STATS_TYPE_FWD,
     CTC_STATS_TYPE_GMAC,
     CTC_STATS_TYPE_XGMAC,
     CTC_STATS_TYPE_SGMAC,
     CTC_STATS_TYPE_CPUMAC,

     CTC_STATS_TYPE_MAX
 };
 typedef enum ctc_stats_type_e ctc_stats_type_t;

/**
 @brief  Define port stats option type
*/
enum ctc_stats_port_stats_option_type_e
{
    CTC_STATS_PORT_BASED_NULL,                   /**< port do not do statistics */
    CTC_STATS_PORT_BASED_BASIC,                  /**< port excluding mac based or protocol based */
    CTC_STATS_PORT_BASED_MAC,                    /**< mac based port */
    CTC_STATS_PORT_BASED_PROTOCOL,	             /**< protocol based port */
    CTC_STATS_PORT_BASED_PHB,	                 /**< phb */
    CTC_STATS_PORT_BASED_PHB_PER_PORT            /**< phb per port */
};
typedef enum ctc_stats_port_stats_option_type_e ctc_stats_port_stats_option_type_t;

/**
 @brief  Define stats forward type
*/
enum ctc_stats_fwd_type_e
{
    CTC_STATS_QUEUE_DEQ,
    CTC_STATS_QUEUE_DROP,
    CTC_STATS_POLICING_GREEN,
    CTC_STATS_POLICING_YELLOW,
    CTC_STATS_POLICING_RED,
    CTC_STATS_FLOW,
    CTC_STATS_MPLS,

    CTC_STATS_FWD_TYPE_MAX
};
typedef enum ctc_stats_fwd_type_e ctc_stats_fwd_type_t;

/**
 @brief  Define MAC base stats property type
*/
enum  ctc_mac_stats_prop_type_e
{
    CTC_STATS_PACKET_LENGTH_MTU1,                  /**< deal with mtu1 packet length */
    CTC_STATS_PACKET_LENGTH_MTU2,                  /**< deal with mtu2 packet length */
    CTC_STATS_DOT1Q_SUBTRACT_BYTES,                /**< deal with bytes to subtract for dot1q packet */

    CTC_STATS_MAC_STATS_PROP_MAX
};
typedef enum ctc_mac_stats_prop_type_e ctc_mac_stats_prop_type_t;

/**
 @brief  Define MAC base stats direction type
*/
enum  ctc_mac_stats_dir_e
{
    CTC_STATS_MAC_STATS_RX,                  /**< Mac stats RX direction */
    CTC_STATS_MAC_STATS_TX,                  /**< Mac stats TX direction */

    CTC_STATS_MAC_STATS_MAX
};
typedef enum ctc_mac_stats_dir_e ctc_mac_stats_dir_t;


/**
 @brief  Define MAC base stats direction type
*/
enum  ctc_stats_property_type_e
{
    CTC_STATS_PROPERTY_SATURATE,                /**< deal with general options saturate enable */
    CTC_STATS_PROPERTY_HOLD,                    /**< deal with general options hold enable */
    CTC_STATS_PROPERTY_CLEAR_AFTER_READ,        /**< deal with general options clear after read */
    CTC_STATS_PROPERTY_PKT_CNT_THREASHOLD,      /**< deal with general options pkt cnt threshold */
    CTC_STATS_PROPERTY_BYTE_CNT_THREASHOLD,     /**< deal with general options byte cnt threshold */
    CTC_STATS_PROPERTY_FIFO_DEPTH_THREASHOLD,   /**< deal with general options fifo depth threshold */

    CTC_STATS_PROPERTY__MAX
};
typedef enum ctc_stats_property_type_e ctc_stats_property_type_t;

/**
 @brief  Define mac receive statistics storage structor
*/
struct ctc_stats_mac_rec_s
{
    uint64 good_ucast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of unicast packets received without error */
    uint64 good_ucast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of unicast packets received without error */
    uint64 good_mcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of multicast packets received without error */
    uint64 good_mcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of multicast packets received without error */
    uint64 good_bcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of broadcast packets received without error */
    uint64 good_bcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of broadcast packets received without error */
    uint64 good_pause_pkts;              /**< total number of pause packets without error */
    uint64 good_pause_bytes;             /**< total bytes of pause packets without error */
    uint64 good_control_pkts;            /**< total number of control packets without error excluding the pause packet */
    uint64 good_control_bytes;           /**< total bytes of control packets without error excluding the pause packet */
    uint64 jabber_pkts;                  /**< no mac overrun,length greater than MTU,total number of packets with fcs error or alignment error */
    uint64 jabber_bytes;                 /**< no mac overrun,length greater than MTU,total bytes of packets with fcs error or alignment error */
    uint64 collision_pkts;               /**< no mac overrun,length less than 64B,total number of packets with fcs error or alignment error */
    uint64 collision_bytes;              /**< no mac overrun,length less than 64B,total bytes of packets with fcs error or alignment error */
    uint64 fcs_error_pkts;               /**< no mac overrun,length equal to 64B to MTU,total number of packets with fcs error */
    uint64 fcs_error_bytes;              /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with fcs error */
    uint64 alignment_error_pkts;         /**< no mac overrun,length equal to 64B to MTU,total number of packets with alignment error */
    uint64 alignment_error_bytes;        /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with alignment error */
    uint64 mac_overrun_pkts;             /**< total number of packets received with mac overrun condition */
    uint64 mac_overrun_bytes;            /**< total bytes of packets received with mac overrun condition */
    uint64 good_oversize_pkts;           /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total number of packets received without fcs error and alignment error */
    uint64 good_oversize_bytes;          /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total bytes of packets received without fcs error and alignment error */
    uint64 good_undersize_pkts;          /**< no mac overrun,length less than 64B,total number of packets received without fcs error and alignment error */
    uint64 good_undersize_bytes;         /**< no mac overrun,length less than 64B,total bytes of packets received without fcs error and alignment error */
    uint64 gmac_good_oam_pkts;           /**< total number of packets without error */
    uint64 gmac_good_oam_bytes;          /**< total bytes of packets without error */
    uint64 good_63_pkts;                 /**< total number of good packets received with length less than 64B including fcs error and late collision */
    uint64 good_63_bytes;                /**< total bytes of good packets received with length less than 64B including fcs error and late collision */
    uint64 bad_63_pkts;                  /**< total number of errored packets received with length less than 64B including fcs error and late collision */
    uint64 bad_63_bytes;                 /**< total bytes of errored packets received with length less than 64B including fcs error and late collision */
    uint64 good_1519_pkts;               /**< total number of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64 good_1519_bytes;              /**< total bytes of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64 bad_1519_pkts;                /**< total number of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64 bad_1519_bytes;               /**< total bytes of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64 good_jumbo_pkts;              /**< total number of good packets received with length greater than MTU including fcs error */
    uint64 good_jumbo_bytes;             /**< total bytes of good packets received with length greater than MTU including fcs error */
    uint64 bad_jumbo_pkts;               /**< total number of errored packets received with length greater than MTU including fcs error */
    uint64 bad_jumbo_bytes;              /**< total bytes of errored packets received with length greater than MTU including fcs error */
    uint64 pkts_64;                      /**< total number of packets received with length equal to 64B including fcs error and late collision */
    uint64 bytes_64;                     /**< total bytes of packets received with length equal to 64B including fcs error and late collision */
    uint64 pkts_65_to_127;               /**< total number of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64 bytes_65_to_127;              /**< total bytes of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64 pkts_128_to_255;              /**< total number of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64 bytes_128_to_255;             /**< total bytes of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64 pkts_256_to_511;              /**< total number of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64 bytes_256_to_511;             /**< total bytes of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64 pkts_512_to_1023;             /**< total number of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64 bytes_512_to_1023;            /**< total bytes of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64 pkts_1024_to_1518;            /**< total number of packets received with length equal to 1024B to 1518B including fcs error and late collision */
    uint64 bytes_1024_to_1518;           /**< total bytes of packets received with length equal to 1024B to 1518B including fcs error and late collision */
};
typedef struct ctc_stats_mac_rec_s ctc_stats_mac_rec_t;

/**
 @brief  Define mac transmit statistics storage structor
*/
struct ctc_stats_mac_snd_s
{
    uint64 good_ucast_pkts;              /**< total number of unicast packets transmitted without error */
    uint64 good_ucast_bytes;             /**< total bytes of unicast packets transmitted without error */
    uint64 good_mcast_pkts;              /**< total number of multicast packets transmitted without error */
    uint64 good_mcast_bytes;             /**< total bytes of multicast packets transmitted without error */
    uint64 good_bcast_pkts;              /**< total number of broadcast packets transmitted without error */
    uint64 good_bcast_bytes;             /**< total bytes of broadcast packets transmitted without error */
    uint64 good_pause_pkts;              /**< total number of pause packets transmitted without error */
    uint64 good_pause_bytes;             /**< total bytes of pause packets transmitted without error */
    uint64 good_control_pkts;            /**< total number of pause packets transmitted without error excluding the pause packet */
    uint64 good_control_bytes;           /**< total bytes of pause packets transmitted without error excluding the pause packet */
    uint64 good_oam_pkts;                /**< total number of oam packets transmitted without error */
    uint64 good_oam_bytes;               /**< total bytes of oam packets transmitted without error */
    uint64 pkts_63;                      /**< total number of packets transmitted with length less than 64B including fcs error and late collision */
    uint64 bytes_63;                     /**< total bytes of packets transmitted with length less than 64B including fcs error and late collision */
    uint64 pkts_64;                      /**< total number of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64 bytes_64;                     /**< total bytes of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64 pkts_65_to_127;               /**< total number of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64 bytes_65_to_127;              /**< total bytes of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64 pkts_128_to_255;              /**< total number of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64 bytes_128_to_255;             /**< total bytes of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64 pkts_256_to_511;              /**< total number of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64 bytes_256_to_511;             /**< total bytes of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64 pkts_512_to_1023;             /**< total number of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64 bytes_512_to_1023;            /**< total bytes of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64 pkts_1024_to_1518;            /**< total number of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64 bytes_1024_to_1518;           /**< total bytes of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64 pkts_1519;                    /**< total number of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64 bytes_1519;                   /**< total bytes of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64 jumbo_pkts;                   /**< total number of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64 jumbo_bytes;                  /**< total bytes of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64 mac_underrun_pkts;            /**< total number of packets transmitted with mac underrun condition */
    uint64 mac_underrun_bytes;           /**< total bytes of packets transmitted with mac underrun condition */
    uint64 fcs_error_pkts;               /**< total number of packets transmitted with fcs error */
    uint64 fcs_error_bytes;              /**< total bytes of packets transmitted with fcs error */
};
typedef struct ctc_stats_mac_snd_s ctc_stats_mac_snd_t;

/**
 @brief  Define basic port statistics storage structor
*/
struct ctc_stats_port_based_basic_s
{
    uint32 port_pkts;                    /**< total number of packets based designated port */
    uint64 port_bytes;                   /**< total bytes of packets based designated port */
};
typedef struct ctc_stats_port_based_basic_s ctc_stats_port_based_basic_t;


/**
 @brief  Define mac based port statistics storage structor
*/
struct ctc_stats_port_based_mac_s
{
    uint32 port_ucast_pkts;              /**< total number of unicast packets based designated port */
    uint64 port_ucast_bytes;             /**< total bytes of unicast packets based designated port */
    uint32 port_rut_mac_pkts;            /**< total number of route mac packets based designated port */
    uint64 port_rut_mac_bytes;           /**< total bytes of route mac packets based designated port */
    uint32 port_mcast_pkts;              /**< total number of multicast packets based designated port */
    uint64 port_mcast_bytes;             /**< total bytes of multicast packets based designated port */
    uint32 port_bcast_pkts;              /**< total number of broadcast packets based designated port */
    uint64 port_bcast_bytes;             /**< total bytes of broadcast packets based designated port */
};
typedef struct ctc_stats_port_based_mac_s ctc_stats_port_based_mac_t;

/**
 @brief  Define protocol based port statistics storage structor
*/
struct ctc_stats_port_based_protocol_s
{
    uint32 port_ipv4_pkts;               /**< total number of Ipv4 packets based designated port */
    uint64 port_ipv4_bytes;              /**< total bytes of Ipv4 packets based designated port */
    uint32 port_ipv6_pkts;               /**< total number of Ipv6 packets based designated port */
    uint64 port_ipv6_bytes;              /**< total bytes of Ipv6 packets based designated port */
    uint32 port_mpls_pkts;               /**< total number of MPLS packets based designated port */
    uint64 port_mpls_bytes;              /**< total bytes of MPLS packets based designated port */
    uint32 port_prtol_dft_pkts;          /**< total number of protocol packets based designated port excluding IPv4 or IPv6 or MPLS*/
    uint64 port_prtol_dft_bytes;         /**< total bytes of protocol packets based designated port excluding IPv4 or IPv6 or MPLS*/
};
typedef struct ctc_stats_port_based_protocol_s ctc_stats_port_based_protocol_t;

/**
 @brief  Define port statistics storage structor
*/
struct ctc_stats_port_s
{
    ctc_stats_port_stats_option_type_t type;           /**< port stats option type */
    union
    {
        ctc_stats_port_based_basic_t basic_port;       /**< port statistics excluding mac based or protocol based */
        ctc_stats_port_based_mac_t mac_port;           /**< mac based port statistics */
        ctc_stats_port_based_protocol_t prtol_port;    /**< protocol based port statistics */
    }stats;
};
typedef struct ctc_stats_port_s ctc_stats_port_t;

/**
 @brief  Define ingress global forwarding statistics storage structor
*/
struct ctc_stats_igs_global_fwd_s
{
    uint32 ipv4_ucast_rut_pkts;           /**< total number of IPv4 unicast routed packets */
    uint64 ipv4_ucast_rut_bytes;          /**< total bytes of IPv4 unicast routed packets */
    uint32 ipv4_mcast_rut_pkts;           /**< total number of IPv4 multicast routed packets */
    uint64 ipv4_mcast_rut_bytes;          /**< total bytes of IPv4 multicast routed packets */
    uint32 ipv6_ucast_rut_pkts;           /**< total number of IPv6 unicast routed packets */
    uint64 ipv6_ucast_rut_bytes;          /**< total bytes of IPv6 unicast routed packets */
    uint32 ipv6_mcast_rut_pkts;           /**< total number of IPv6 multicast routed packets */
    uint64 ipv6_mcast_rut_bytes;          /**< total bytes of IPv6 multicast routed packets */
    uint32 ipv4_ucast_rut_esp_pkts;       /**< total number of IPv4 unicast route escaped packets */
    uint64 ipv4_ucast_rut_esp_bytes;      /**< total bytes of IPv4 unicast route escaped packets */
    uint32 ipv4_mcast_rut_esp_pkts;       /**< total number of IPv4 multicast route escaped packets */
    uint64 ipv4_mcast_rut_esp_bytes;      /**< total bytes of IPv4 multicast route escaped packets */
    uint32 ipv6_ucast_rut_esp_pkts;       /**< total number of IPv6 unicast route escaped packets */
    uint64 ipv6_ucast_rut_esp_bytes;      /**< total bytes of IPv6 unicast route escaped packets */
    uint32 ipv6_mcast_rut_esp_pkts;       /**< total number of IPv6 multicast route escaped packets */
    uint64 ipv6_mcast_rut_esp_bytes;      /**< total bytes of IPv6 multicast route escaped packets */
    uint32 mpls_ucast_swt_pkts;           /**< total number of MPLS unicast switched packets */
    uint64 mpls_ucast_swt_bytes;          /**< total bytes of MPLS unicast switched packets */
    uint32 mpls_mcast_swt_pkts;           /**< total number of MPLS multicast switched packets */
    uint64 mpls_mcast_swt_bytes;          /**< total bytes of MPLS multicast switched packets */
    uint32 mpls_ucast_swt_ftl_pkts;       /**< total number of MPLS unicast switched packets with fatal escaption */
    uint64 mpls_ucast_swt_ftl_bytes;      /**< total bytes of MPLS unicast switched packets with fatal escaption */
    uint32 mpls_mcast_swt_ftl_pkts;       /**< total number of MPLS multicast switched packets with fatal escaption */
    uint64 mpls_mcast_swt_ftl_bytes;      /**< total bytes of MPLS multicast switched packets with fatal escaption */
    uint32 bdg_bcast_pkts;                /**< total number of bridged packets with broadcast address */
    uint64 bdg_bcast_bytes;               /**< total bytes of bridged packets with broadcast address */
    uint32 bdg_mcast_pkts;                /**< total number of bridged packets with multicast address */
    uint64 bdg_mcast_bytes;               /**< total bytes of bridged packets with multicast address */
    uint32 bdg_ucast_pkts;                /**< total number of bridged packets with unicast address */
    uint64 bdg_ucast_bytes;               /**< total bytes of bridged packets with unicast address */
    uint32 bdg_escape_pkts;               /**< total number of bridged escaped packets */
    uint64 bdg_escape_bytes;              /**< total bytes of bridged escaped packets */
};
typedef struct ctc_stats_igs_global_fwd_s ctc_stats_igs_global_fwd_t;

/**
 @brief  Define egress global forwarding statistics storage structor
*/
struct ctc_stats_egs_global_fwd_s
{
    uint32 pld_ipv4_ucast_rut_pkts;           /**< total number of IPv4 unicast routed packets */
    uint64 pld_ipv4_ucast_rut_bytes;          /**< total bytes of IPv4 unicast routed packets */
    uint32 pld_ipv4_mcast_rut_pkts;           /**< total number of IPv4 multicast routed packets */
    uint64 pld_ipv4_mcast_rut_bytes;          /**< total bytes of IPv4 multicast routed packets */
    uint32 pld_ipv6_ucast_rut_pkts;           /**< total number of IPv6 unicast routed packets */
    uint64 pld_ipv6_ucast_rut_bytes;          /**< total bytes of IPv6 unicast routed packets */
    uint32 pld_ipv6_mcast_rut_pkts;           /**< total number of IPv6 multicast routed packets */
    uint64 pld_ipv6_mcast_rut_bytes;          /**< total bytes of IPv6 multicast routed packets */
    uint32 pld_bdg_ucast_pkts;                /**< total number of bridged packets with unicast address */
    uint64 pld_bdg_ucast_bytes;               /**< total bytes of bridged packets with unicast address */
    uint32 pld_bdg_mcast_pkts;                /**< total number of bridged packets with multicast address */
    uint64 pld_bdg_mcast_bytes;	              /**< total bytes of bridged packets with multicast address */
    uint32 pld_bdg_bcast_pkts;                /**< total number of bridged packets with broadcast address */
    uint64 pld_bdg_bcast_bytes;               /**< total bytes of bridged packets with broadcast address */
    uint32 payload_no_op_pkts;            /**< total number of packets with no payload operation */
    uint64 payload_no_op_bytes;           /**< total bytes of packets with no payload operation */
    uint32 l3_edit_no_op_pkts;            /**< total number of packets without L3 editing operation */
    uint64 l3_edit_no_op_bytes;           /**< total bytes of packets without L3 editing operation */
    uint32 l3_edit_mpls_pkts;             /**< total number of packets with MPLS header attached */
    uint64 l3_edit_mpls_bytes;            /**< total bytes of packets with MPLS header attached */
    uint32 l3_edit_nat_pkts;              /**< total number of packets goes through NAT operation */
    uint64 l3_edit_nat_bytes;             /**< total bytes of packets goes through NAT operation */
    uint32 l3_edit_tunnel_pkts;           /**< total number of packets with IP tunnel header attached */
    uint64 l3_edit_tunnel_bytes;          /**< total bytes of packets with IP tunnel header attached */
    uint32 l3_edit_flex_pkts;             /**< total number of packets with flexible L3 header attached */
    uint64 l3_edit_flex_bytes;            /**< total bytes of packets with flexible L3 header attached */
    uint32 l2_edit_no_op_pkts;            /**< total number of packets without L2 editing operation */
    uint64 l2_edit_no_op_bytes;           /**< total bytes of packets without L2 editing operation */
    uint32 l2_edit_eth_pkts;              /**< total number of packets with ethernet header attached */
    uint64 l2_edit_eth_bytes;             /**< total bytes of packets with ethernet header attached */
    uint32 l2_edit_flex_pkts;             /**< total number of packets with flexible L2 header attached */
    uint64 l2_edit_flex_bytes;            /**< total bytes of packets with flexible L2 header attached */
};
typedef struct ctc_stats_egs_global_fwd_s ctc_stats_egs_global_fwd_t;

/**
 @brief  Define basic statistics storage structor
*/
struct ctc_stats_basic_s
{
    uint64 packet_count;                  /**< total number of packets */
    uint64 byte_count;                    /**< total bytes of paclets */
};
typedef struct ctc_stats_basic_s ctc_stats_basic_t;

/**
 @brief  Define Mac stats param storage structor
*/
struct ctc_mac_stats_param_type_s
{
    ctc_mac_stats_dir_t packet_dir;                            /**< Mac stats direction type  */
    ctc_mac_stats_prop_type_t mac_stats_prop;                  /**< Mac stats property type  */
};
typedef struct ctc_mac_stats_param_type_s ctc_mac_stats_param_type_t;

/**
 @brief  Define Mac base stats property storage structor
*/
struct ctc_mac_stats_property_s
{
    union
    {
        uint16 length;       /**< length of mtu packet */
        uint8 byte;          /**< num of bytes */
    }data;
    uint16 rsv0;
};
typedef struct ctc_mac_stats_property_s ctc_mac_stats_property_t;


/**
 @brief  Define Mac base stats property storage structor
*/
struct ctc_mac_stats_s
{
    union
    {
        ctc_stats_mac_rec_t rx_stats;       /**< mac receive statistics storage */
        ctc_stats_mac_snd_t tx_stats;       /**< mac send statistics storage */
    }stats;
};
typedef struct ctc_mac_stats_s ctc_mac_stats_t;


/**
 @brief  Define Global Fwarding stats property storage structor
*/
struct ctc_stats_global_fwd_s
{
    union
    {
        ctc_stats_igs_global_fwd_t igs_stats;       /**< ingress global forwarding statistics storage */
        ctc_stats_egs_global_fwd_t egs_stats;       /**< egress global forwarding statistics storage */
    }stats;
};
typedef struct ctc_stats_global_fwd_s ctc_stats_global_fwd_t;


/**
 @brief  Define Global Fwarding stats property storage structor
*/
struct ctc_stats_property_s
{
    union
    {
        bool enable;                 /**< saturate enable , hold enable ,  clear after read value */
        uint16 threshold_16byte;     /**< packet count threshold or byte count threshold value */
        uint8 threshold_8byte;       /**< fifo depth threshold value */
    }data;
};
typedef struct ctc_stats_property_s ctc_stats_property_t;

/**
 @brief  Define Global Fwarding stats property storage structor
*/
struct ctc_stats_property_param_s
{
    ctc_stats_property_type_t prop_type;    /**< stats proterty type */
    ctc_stats_type_t          stats_type;   /**< stats type */

};
typedef struct ctc_stats_property_param_s ctc_stats_property_param_t;

/**@} end of @defgroup stats STATS  */

#endif
