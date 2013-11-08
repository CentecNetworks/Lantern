/**
 @file ctc_cpu_traffic.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

   This file contains all stats related data structure, enum, macro and proto.
*/


#ifndef _CTC_CPU_TRAFFIC_H_
#define _CTC_CPU_TRAFFIC_H_

/****************************************************************
 *
 * Header Files
 *
 ****************************************************************/

#include "ctc_const.h"

/****************************************************************
 *
 * Defines and Macros
 *
 ****************************************************************/

#define CTC_SPECIAL_CPU_TRAFFIC_RATE        0xFFFFFFFF      /**< cpu traffic special rate*/

#define CTC_EXCEPTION_INGRESS_2_SUB_IDX_NUM             16  /**< ingress exception 2 sub num*/
#define CTC_EXCEPTION_INGRESS_3_SUB_IDX_NUM             16  /**< ingress exception 3 sub num*/
#define CTC_EXCEPTION_INGRESS_7_SUB_IDX_NUM             5   /**< ingress exception 7 sub num*/
#define CTC_EXCEPTION_INGRESS_PARSER_PTP_SUB_IDEX_NUM   7   /**< ingress exception parser ptp sub num*/
#define CTC_EXCEPTION_EGRESS_PARSER_PTP_SUB_IDEX_NUM    4   /**< egress exception parser ptp sub num*/


/****************************************************************
 *
 * Data Structures
 *
 ****************************************************************/

/**
 @defgroup cpu_traffic CPU_TRAFFIC
 @{
*/


/**
 @brief Definition of exception dest type
*/
enum ctc_excp_dest_type_e
{
    CTC_EXCP_DEST_TO_LOCAL_CPU = 0, /**< exception to local cpu */
    CTC_EXCP_DEST_TO_LOCAL_PORT,    /**< exception to local port */
    CTC_EXCP_DEST_TO_REMOTE_PORT,   /**< exception to remote port */
    CTC_EXCP_DEST_TO_DROP           /**< exception to drop **/
};
typedef enum ctc_excp_dest_type_e ctc_excp_dest_type_t;


/**
 @brief Definition of packet-to-CPU reasons
*/
enum ctc_packet_tocpu_reason_e
{
    /* 0- 3 */
    CTC_EXCEPTION_INGRESS_L2_SPAN0 = 0, /**< ingress exception l2 spanId 0 index */
    CTC_EXCEPTION_INGRESS_L2_SPAN1,     /**< ingress exception l2 spanId 1 index */
    CTC_EXCEPTION_INGRESS_L2_SPAN2,     /**< ingress exception l2 spanId 2 index */
    CTC_EXCEPTION_INGRESS_L2_SPAN3,     /**< ingress exception l2 spanId 3 index */

    /* 4 - 7 */
    CTC_EXCEPTION_INGRESS_L3_SPAN0, /**< ingress exception l3 spanId 0 index */
    CTC_EXCEPTION_INGRESS_L3_SPAN1, /**< ingress exception l3 spanId 1 index */
    CTC_EXCEPTION_INGRESS_L3_SPAN2, /**< ingress exception l3 spanId 2 index */
    CTC_EXCEPTION_INGRESS_L3_SPAN3, /**< ingress exception l3 spanId 3 index */

    /* 8 - 11 */
    CTC_EXCEPTION_INGRESS_ACL_LOG0, /**< ingress exception acl logId 0 index */
    CTC_EXCEPTION_INGRESS_ACL_LOG1, /**< ingress exception acl logId 1 index */
    CTC_EXCEPTION_INGRESS_ACL_LOG2, /**< ingress exception acl logId 2 index */
    CTC_EXCEPTION_INGRESS_ACL_LOG3, /**< ingress exception acl logId 3 index */

    /* 12 - 15 */
    CTC_EXCEPTION_INGRESS_QOS_LOG0, /**< ingress exception qos logId 0 index */
    CTC_EXCEPTION_INGRESS_QOS_LOG1, /**< ingress exception qos logId 1 index */
    CTC_EXCEPTION_INGRESS_QOS_LOG2, /**< ingress exception qos logId 2 index */
    CTC_EXCEPTION_INGRESS_QOS_LOG3, /**< ingress exception qos logId 3 index */

    /* 16 - 19 */
    CTC_EXCEPTION_EGRESS_L2_SPAN0, /**< egress exception l2 spanId 0 index */
    CTC_EXCEPTION_EGRESS_L2_SPAN1, /**< egress exception l2 spanId 1 index */
    CTC_EXCEPTION_EGRESS_L2_SPAN2, /**< egress exception l2 spanId 2 index */
    CTC_EXCEPTION_EGRESS_L2_SPAN3, /**< egress exception l2 spanId 3 index */

    /* 20 - 23 */
    CTC_EXCEPTION_EGRESS_L3_SPAN0, /**< egress exception l3 spanId 0 index */
    CTC_EXCEPTION_EGRESS_L3_SPAN1, /**< egress exception l3 spanId 1 index */
    CTC_EXCEPTION_EGRESS_L3_SPAN2, /**< egress exception l3 spanId 2 index */
    CTC_EXCEPTION_EGRESS_L3_SPAN3, /**< egress exception l3 spanId 3 index */

    /* 24 - 27 */
    CTC_EXCEPTION_EGRESS_ACL_LOG0, /**< egress exception acl logId 0 index */
    CTC_EXCEPTION_EGRESS_ACL_LOG1, /**< egress exception acl logId 1 index */
    CTC_EXCEPTION_EGRESS_ACL_LOG2, /**< egress exception acl logId 2 index */
    CTC_EXCEPTION_EGRESS_ACL_LOG3, /**< egress exception acl logId 3 index */

    /* 28 - 31 */
    CTC_EXCEPTION_EGRESS_QOS_LOG0, /**< egress exception qos logId 0 index */
    CTC_EXCEPTION_EGRESS_QOS_LOG1, /**< egress exception qos logId 1 index */
    CTC_EXCEPTION_EGRESS_QOS_LOG2, /**< egress exception qos logId 2 index */
    CTC_EXCEPTION_EGRESS_QOS_LOG3, /**< egress exception qos logId 3 index */

    /* 32 - 39 */
    CTC_EXCEPTION_INGRESS_0, /**< ingress exception 0 index */
    CTC_EXCEPTION_INGRESS_1, /**< ingress exception 1 index */
    CTC_EXCEPTION_INGRESS_2, /**< ingress exception 2 index */
    CTC_EXCEPTION_INGRESS_3, /**< ingress exception 3 index */
    CTC_EXCEPTION_INGRESS_4, /**< ingress exception 4 index */
    CTC_EXCEPTION_INGRESS_5, /**< ingress exception 5 index */
    CTC_EXCEPTION_INGRESS_6, /**< ingress exception 6 index */
    CTC_EXCEPTION_INGRESS_7, /**< ingress exception 7 index */

    /* 40 - 47 */
    CTC_EXCEPTION_EGRESS_0, /**< egress exception 0 index */
    CTC_EXCEPTION_EGRESS_1, /**< egress exception 1 index */
    CTC_EXCEPTION_EGRESS_2, /**< egress exception 2 index */
    CTC_EXCEPTION_EGRESS_3, /**< egress exception 3 index */
    CTC_EXCEPTION_EGRESS_4, /**< egress exception 4 index */
    CTC_EXCEPTION_EGRESS_5, /**< egress exception 5 index */
    CTC_EXCEPTION_EGRESS_6, /**< egress exception 6 index */
    CTC_EXCEPTION_EGRESS_7, /**< egress exception 7 index */

    /* 48 */
    CTC_EXCEPTION_INGRESS_PORT_LOG,     /**< ingress exception port log index*/

    /* 49 */
    CTC_EXCEPTION_INGRESS_SEQNUM_CHECK, /**< ingress exception sequence num check index*/

    /* 50 */
    CTC_EXCEPTION_INGRESS_PARSER_PTP,   /**< ingress exception parser or ptp index*/

    /* 51 */
    CTC_EXCEPTION_INGRESS_OAM,          /**< ingress exception oam index*/

    /* 52 */
    CTC_EXCEPTION_EGRESS_PORT_LOG,      /**< egress exception port log index*/

    /* 53 */
    CTC_EXCEPTION_EGRESS_SEQNUM_CHECK,  /**< egress exception sequence num check index*/

    /* 54 */
    CTC_EXCEPTION_EGRESS_PARSER_PTP,    /**< egress exception parser or ptp index*/

    /* 55 */
    CTC_EXCEPTION_EGRESS_OAM,           /**< egress exception oam index*/

    /* 56 - 71 */
    CTC_FATAL_EXCEPTION_0,  /**< fatal exception 0 index*/
    CTC_FATAL_EXCEPTION_1,  /**< fatal exception 1 index*/
    CTC_FATAL_EXCEPTION_2,  /**< fatal exception 2 index*/
    CTC_FATAL_EXCEPTION_3,  /**< fatal exception 3 index*/
    CTC_FATAL_EXCEPTION_4,  /**< fatal exception 4 index*/
    CTC_FATAL_EXCEPTION_5,  /**< fatal exception 5 index*/
    CTC_FATAL_EXCEPTION_6,  /**< fatal exception 6 index*/
    CTC_FATAL_EXCEPTION_7,  /**< fatal exception 7 index*/
    CTC_FATAL_EXCEPTION_8,  /**< fatal exception 8 index*/
    CTC_FATAL_EXCEPTION_9,  /**< fatal exception 9 index*/
    CTC_FATAL_EXCEPTION_10, /**< fatal exception 10 index*/
    CTC_FATAL_EXCEPTION_11, /**< fatal exception 11 index*/
    CTC_FATAL_EXCEPTION_12, /**< fatal exception 12 index*/
    CTC_FATAL_EXCEPTION_13, /**< fatal exception 13 index*/
    CTC_FATAL_EXCEPTION_14, /**< fatal exception 14 index*/
    CTC_FATAL_EXCEPTION_15, /**< fatal exception 15 index*/

    MAX_CTC_EXCEPTION, /**<max exception index*/

    /* Exceptions in IPE */
    CTC_EXCEP_INGRESS_USID       = CTC_EXCEPTION_INGRESS_0,          /**< ingress userid exception */
    CTC_EXCEP_INGRESS_PROTO_VLAN = CTC_EXCEPTION_INGRESS_1,          /**< ingress protocol vlan exception */
    CTC_EXCEP_INGRESS_BRIDGE     = CTC_EXCEPTION_INGRESS_2,          /**< ingress bridge exception */
    CTC_EXCEP_INGRESS_ROUTE_IPDA = CTC_EXCEPTION_INGRESS_3,          /**< ingress route exception for IP-DA */
    CTC_EXCEP_INGRESS_ROUTE_ICMP = CTC_EXCEPTION_INGRESS_4,          /**< ingress route exception for ICMP redirect */
    CTC_EXCEP_INGRESS_LEARNING   = CTC_EXCEPTION_INGRESS_5,          /**< ingress learning exception for cache full */
    CTC_EXCEP_INGRESS_MCAST_RPF  = CTC_EXCEPTION_INGRESS_6,          /**< ingress route multicast RPF fail */
    CTC_EXCEP_INGRESS_SECURITY   = CTC_EXCEPTION_INGRESS_7,          /**< ingress security exception */

    /* Exceptions in EPE, NOTE that exception 4 is not used */
    CTC_EXCEP_EGRESS_MTU_DONTFRAG        = CTC_EXCEPTION_EGRESS_0,   /**< egress mtu check fail and original packet don't frag */
    CTC_EXCEP_EGRESS_MTU_FRAG            = CTC_EXCEPTION_EGRESS_1,   /**< egress mtu check fail and original packet can frag */
    CTC_EXCEP_EGRESS_TTL                 = CTC_EXCEPTION_EGRESS_2,   /**< egress TTL = 0 */
    CTC_EXCEP_EGRESS_MCAST_TTL           = CTC_EXCEPTION_EGRESS_3,   /**< egress multicast TTL threshold */
    CTC_EXCEP_EGRESS_TUNNEL_MTU_DONTFRAG = CTC_EXCEPTION_EGRESS_5,   /**< egress tunnel mtu check fail and original packet don't frag */
    CTC_EXCEP_EGRESS_TUNNEL_MTU_FRAG     = CTC_EXCEPTION_EGRESS_6,   /**< egress tunnel mtu check fail and original packet can frag */
    CTC_EXCEP_EGRESS_BFD_UDP             = CTC_EXCEPTION_EGRESS_7,   /**< egress BFD UDP */

    /* Fatal exceptions */
    CTC_FATAL_EXCEP_UCAST_IP_HDR_ERROR      = CTC_FATAL_EXCEPTION_0, /**< ucast IP header error or IP martion address - discard */
    CTC_FATAL_EXCEP_UCAST_IP_OPTION         = CTC_FATAL_EXCEPTION_1, /**< ucast IP options - CPU */
    CTC_FATAL_EXCEP_UCAST_GRE_UNKNOWN       = CTC_FATAL_EXCEPTION_2, /**< ucast GRE unknown option or protocol - CPU */
    CTC_FATAL_EXCEP_ISATAP_SA_FAIL          = CTC_FATAL_EXCEPTION_3, /**< ucast ISATAP source address check failure - discard */
    CTC_FATAL_EXCEP_UCAST_IP_TTL_CHECK_FAIL = CTC_FATAL_EXCEPTION_4, /**< ucast IP TTL check failure - discard */
    CTC_FATAL_EXCEP_UCAST_RPF_FAIL          = CTC_FATAL_EXCEPTION_5, /**< ucast RPF failure - discard */
    CTC_FATAL_EXCEP_MORE_RPF                = CTC_FATAL_EXCEPTION_6, /**< ucast or Mcast more RPF - CPU */
    CTC_FATAL_EXCEP_UCAST_LINK_ID_FAIL      = CTC_FATAL_EXCEPTION_7, /**< ucast Link ID check failure - CPU */
    CTC_FATAL_EXCEP_MPLS_LABEL_OUT_RANGE    = CTC_FATAL_EXCEPTION_8, /**< mpls label out of range - discard */
    CTC_FATAL_EXCEP_MPLS_SBIT_ERROR         = CTC_FATAL_EXCEPTION_9, /**< mpls Sbit error - discard */
    CTC_FATAL_EXCEP_MPLS_TTL_FAIL           = CTC_FATAL_EXCEPTION_10,/**< mpls TTL check failure - diacard */
    CTC_FATAL_EXCEP_IGMP_SNOOPING_PACKET    = CTC_FATAL_EXCEPTION_12 /**< igmp Snooping packet - CPU */
};
typedef enum ctc_packet_tocpu_reason_e ctc_packet_tocpu_reason_t;




/**@} end of @defgroup cpu_traffic CPU_TRAFFIC  */

#endif


