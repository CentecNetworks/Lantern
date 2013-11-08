/**
 @file ctc_packet_driver.h

 @date 2012-3-14

 @version v2.0

 This file define the packet driver APIs
*/

#ifndef _CTC_PKT_DRV_H
#define _CTC_PKT_DRV_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_common.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

enum ctc_pkt_drv_pkt_type_e
{
    CTC_PKT_DRV_ETH_PKT,
    CTC_PKT_DRV_IPV4_PKT,
    CTC_PKT_DRV_MPLS_PKT,
    CTC_PKT_DRV_IPV6_PKT,
    CTC_PKT_DRV_MPLS_MCAST_PKT,
    CTC_PKT_DRV_FLEX_PKT1,
    CTC_PKT_DRV_FLEX_PKT2,
    CTC_PKT_DRV_RESERVED
};
typedef enum ctc_pkt_drv_pkt_type_e ctc_pkt_drv_pkt_type_t;

enum ctc_pkt_drv_nexthop_type_e
{
    CTC_PKT_DRV_NEXTHOP_TYPE_BYPASS = 0,
    CTC_PKT_DRV_NEXTHOP_TYPE_BRIDGE,
    CTC_PKT_DRV_NEXTHOP_TYPE_UNTAG,
    CTC_PKT_DRV_NEXTHOP_TYPE_USER
};
typedef enum ctc_pkt_drv_nexthop_type_e ctc_pkt_drv_nexthop_type_t;

enum ctc_pkt_drv_operation_type_e
{
    CTC_PKT_DRV_OPERATION_NORMAL,
    CTC_PKT_DRV_OPERATION_PTP,
    CTC_PKT_DRV_OPERATION_NAT,
    CTC_PKT_DRV_OPERATION_OAM,
    CTC_PKT_DRV_OPERATION_UCAST,
    CTC_PKT_DRV_OPERATION_BCAST,
    CTC_PKT_DRV_OPERATION_L2MCAST,
    CTC_PKT_DRV_OPERATION_L3MCAST
};
typedef enum ctc_pkt_drv_operation_type_e ctc_pkt_drv_operation_type_t;

enum ctc_pkt_drv_hdr_tx_flag_e
{
    CTC_PKT_DRV_ENCAP_NEXTHOP_FLAG     = 1U << 1,    /**< Flag to indicate nexthop is set */
    CTC_PKT_DRV_ENCAP_OPT_TYPE_FLAG    = 1U << 2,    /**< Flag to indicate operation type is set */
    CTC_PKT_DRV_ENCAP_HASH_FLAG        = 1U << 3,    /**< Flag to indicate hash is set */
    CTC_PKT_DRV_ENCAP_SRC_SVID_FLAG    = 1U << 4,    /**< Flag to indicate source svlan id is set */
    CTC_PKT_DRV_ENCAP_SRC_CVID_FLAG    = 1U << 5,    /**< Flag to indicate source cvlan id is set */
    CTC_PKT_DRV_ENCAP_SRC_COS_FLAG     = 1U << 6,    /**< Flag to indicate source cos is set */
    CTC_PKT_DRV_ENCAP_TTL_FLAG         = 1U << 7,    /**< Flag to indicate ttl is set */
    CTC_PKT_DRV_ENCAP_PKT_TYPE_FLAG    = 1U << 8,    /**< Flag to indicate packet type is set */
    CTC_PKT_DRV_ENCAP_PRIORITY_FLAG    = 1U << 9,    /**< Flag to indicate priority is set */
    CTC_PKT_DRV_ENCAP_COLOR_FLAG       = 1U << 10,   /**< Flag to indicate color is set */
    CTC_PKT_DRV_ENCAP_TIMESTAMP_FLAG         = 1U << 11,   /**< Flag to indicate ptp is set */
    CTC_PKT_DRV_ENCAP_MCAST_FLAG       = 1U << 12,   /**< Flag to indicate is multicast packet or not */
    CTC_PKT_DRV_ENCAP_CRITICAL_FLAG    = 1U << 13,   /**< Flag to indicate packet is critical or not */
    CTC_PKT_DRV_ENCAP_NEXTHOP8W_FLAG   = 1U << 14    /**< Flag to indicate whether use 8W nexthop */
};
typedef enum ctc_pkt_drv_hdr_tx_flag_e ctc_pkt_hdr_tx_flag_t;

struct ctc_pkt_drv_to_cpu_reason_s
{
    ctc_cpu_pkttocpu_mod_t mod; /**< module */
    uint8 exp_index;            /**< exception index */
    union
    {
        uint8 exp_sub_index;    /**< exception sub index, if cpu destination is single cpu */
        uint8 cpu_mac;          /**< cpu mac, if cpu destination is multiple cpu */
    }sub;
    uint8 rsv0[2];
};
typedef struct ctc_pkt_drv_to_cpu_reason_s ctc_pkt_drv_to_cpu_reason_t;

struct ctc_pkt_drv_time_stamp_s
{
    uint64 seconds;     /**< time stamp second value, 1588v1 timestamp low 32bit valid, 1588v2 timestamp low 48bit valid*/
    uint32 nanoseconds; /**< time stamp nanosecond value */
};
typedef struct ctc_pkt_drv_time_stamp_s ctc_pkt_drv_time_stamp_t;

struct ctc_pkt_drv_hdr_rx_info_s
{
    ctc_pkt_drv_pkt_type_t pkt_type;    /**< packet type */
    ctc_pkt_drv_to_cpu_reason_t reason; /**< the reason of packet to cpu */
    ctc_pkt_drv_time_stamp_t timestamp; /**< PTP time stamp */
    uint16 src_gport;                   /**< source port */
    uint16 vrfid;                       /**< vrfid or fid */
    uint16 svlan_id;                    /**< source svlan ID */
    uint16 cvlan_id;                    /**< source cvlan ID */
    uint8  svlanid_valid;               /**< source svlanId valid */
    uint8  cvlanid_valid;               /**< source cvlanId valid */
    uint8  src_cos;                     /**< source cos */
    uint8  priority;                    /**< priority <0-63> */
    uint8  color;                       /**< color <1-3> */
    uint8  rsv[3];
    void*  pkt;                         /**< the packet which received */
    uint16 pkt_len;                     /**< packet length */    
};
typedef struct ctc_pkt_drv_hdr_rx_info_s ctc_pkt_drv_hdr_rx_info_t;

struct ctc_pkt_drv_hdr_tx_info_s
{
    uint32 flag;                                /**< CTC_PKT_DRV_ENCAP_XXX */
    ctc_pkt_drv_nexthop_type_t nh_type;         /**< nexthop type */
    ctc_pkt_drv_operation_type_t operation_type;/**< operation type */
    ctc_pkt_drv_pkt_type_t  pkt_type;           /**< packet type */
    uint32 nh_offset;                           /**< Ds nexthop offset in global dynamic sram */
    uint32 hash;                                /**< hash used for linkagg */
    uint16 src_gport;                           /**< global source port*/
    uint16 destid;                              /**< destination local portid(queue id(8bit) + local port id(8bit)) or dest mcast group id */
    uint16 svlan_id;                            /**< source svlan ID */
    uint16 cvlan_id;                            /**< source cvlan ID */
    uint8  dest_chipid;                         /**< destination chip id */
    uint8  src_cos;                             /**< source cos */
    uint8  ttl;                                 /**< TTL */
    uint8  priority;                            /**< priority <0-63> */
    uint8  color;                               /**< color <1-3> */
    uint8  ts_offset;                           /**< time stamp offset */
    uint8  ts_version;                          /**< time stamp version */
    uint8  rsv0;
    void*  pkt;                                 /**< the packet which need to be send */
    uint16 pkt_len;                             /**< packet length */    

};
typedef struct ctc_pkt_drv_hdr_tx_info_s ctc_pkt_drv_hdr_tx_info_t;

/****************************************************************************
*
* Function
*
*****************************************************************************/
/*******************************************************************************
 * Name         : ctc_pkt_drv_rx_process
 * Purpose      : decapsulate packets received from cpu port
 * Input        : pkt: the packet which have cpu header, bridge header and data
                  len: packet length
 * Output       : decap_info: the info decap from bridge header
 * Return       : CTC_E_NONE
 * Note         : 
*******************************************************************************/
extern int32
ctc_pkt_drv_decap_packet(void* pkt, uint16 len, ctc_pkt_drv_hdr_rx_info_t* rx_info);

/*******************************************************************************
 * Name         : ctc_pkt_drv_bridge_header_encap
 * Purpose      : encapsulate packets from up protocol layer
 * Input        : encap_info: the packet encapsulation info
                  len: buffer length
 * Output       : buffer: the buffer which used to store packet include 
                      cpu header, bridge header and data
                  len: output packet length
 * Return       : CTC_E_NONE
 * Note         : 
*******************************************************************************/
extern int32
ctc_pkt_drv_encap_packet(ctc_pkt_drv_hdr_tx_info_t* tx_info, void* pkt, uint16* len);

#endif

