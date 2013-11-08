/**
 @file ctc_port.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2011-11-9

 @version v2.0

 This file contains all port related data structure, enum, macro and proto.

*/

#ifndef _CTC_PORT_H
#define _CTC_PORT_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "kal.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @defgroup port PORT
 @{
*/

#define CTC_HUMBER_LPORT_TO_CPU 	53 		/**<local port to CPU*/
#define CTC_GREATBELT_LPORT_TO_CPU 	58 		/**<local port to CPU*/


/**
 @brief define vlan tag ctl
*/
enum ctc_vlantag_ctl_e
{
    CTC_VLANCTL_ALLOW_ALL_PACKETS,                  /**<allow all packet*/
    CTC_VLANCTL_DROP_ALL_UNTAGGED,                  /**<drop all untagged packet*/
    CTC_VLANCTL_DROP_ALL_TAGGED,                    /**<drop all tagged packet*/
    CTC_VLANCTL_DROP_ALL,                           /**<drop all packet*/
    CTC_VLANCTL_DROP_PACKET_WITHOUT_TWO_TAG,        /**<drop packet without double tagged*/
    CTC_VLANCTL_DROP_ALL_PACKET_WITH_TWO_TAG,       /**<drop packet with double tagged*/
    CTC_VLANCTL_DROP_ALL_SVLAN_TAG,                 /**<drop packet with stag*/
    CTC_VLANCTL_DROP_ALL_NON_SVLAN_TAG,             /**<drop packet without stag*/
    CTC_VLANCTL_DROP_ONLY_SVLAN_TAG,                /**<drop packet with only stag*/
    CTC_VLANCTL_PERMIT_ONLY_SVLAN_TAG,              /**<permit packet with only stag*/
    CTC_VLANCTL_DROP_ALL_CVLAN_TAG,                 /**<drop packet with ctag*/
    CTC_VLANCTL_DROP_ALL_NON_CVLAN_TAG,             /**<drop packet without ctag*/
    CTC_VLANCTL_DROP_ONLY_CVLAN_TAG,                /**<drop packet with only ctag*/
    CTC_VLANCTL_PERMIT_ONLY_CVLAN_TAG,              /**<permit packet with only ctag*/

    MAX_CTC_VLANTAG_CTL
};
typedef enum ctc_vlantag_ctl_e ctc_vlantag_ctl_t;

/**
 @brief define dot1q type of port
*/
enum ctc_dot1q_type_e
{
    CTC_DOT1Q_TYPE_NONE,        /**<packet transmit out with untag*/
    CTC_DOT1Q_TYPE_CVLAN,       /**<packet transmit out with ctag*/
    CTC_DOT1Q_TYPE_SVLAN,       /**<packet transmit out with stag*/
    CTC_DOT1Q_TYPE_BOTH         /**<packet transmit out with double tag*/
};
typedef enum ctc_dot1q_type_e ctc_dot1q_type_t;

/**
 @brief define serdes mode of port
*/
enum ctc_serdes_mode_e
{
    CTC_SERDES_MODE_1G25,          /**<serdes 1.25Gbps mode*/
    CTC_SERDES_MODE_3G125,        /**<serdes 3.125Gbps mode*/
    CTC_SERDES_MODE_MAX
};
typedef enum ctc_serdes_mode_e ctc_serdes_mode_t;

/**
 @brief define speed type of port
*/
enum ctc_port_speed_e
{
    CTC_PORT_SPEED_1G,          /**<port speed 1G mode*/
    CTC_PORT_SPEED_100M,        /**<port speed 100M mode*/
    CTC_PORT_SPEED_10M,         /**<port speed 10M mode*/
    CTC_PORT_SPEED_2G5,         /**<port speed 2.5G mode*/
    CTC_PORT_SPEED_MAX
};
typedef enum ctc_port_speed_e ctc_port_speed_t;

/**
 @brief define mac type of port
*/
enum ctc_port_mac_type_e
{
    CTC_PORT_MAC_GMAC,      /**<mac type gmac, speed can be 1G 100M and 10M*/
    CTC_PORT_MAC_XGMAC,     /**<mac type Xgmac, speed at 10G*/
    CTC_PORT_MAC_SGMAC,     /**<mac type Sgmac, speed can be 13G*/
    CTC_PORT_MAC_CPUMAC,    /**<mac type Cpumac, uplink to cpu*/
    CTC_PORT_MAC_MAX
};
typedef enum ctc_port_mac_type_e ctc_port_mac_type_t;

/**
 @brief define max frame size,
*/
enum ctc_frame_size_e
{
    CTC_FRAME_SIZE_0,       /**< max/min frame size0*/
    CTC_FRAME_SIZE_1,       /**< max/min frame size1*/
    CTC_FRAME_SIZE_MAX
};
typedef enum ctc_frame_size_e ctc_frame_size_t;

/**
 @brief define loopback action,
*/
enum ctc_port_lbk_type_e
{
    CTC_PORT_LBK_TYPE_SWAP_MAC,  /**< after port loopback, pakcet mac is swap */
    CTC_PORT_LBK_TYPE_BYPASS,    /**< after port loopback, pakcet is nochange */
    CTC_PORT_LBK_TYPE_MAX
};
typedef enum ctc_port_lbk_type_e ctc_port_lbk_type_t;


/**
 @brief define loopback parmamete,
*/
struct ctc_port_lbk_param_s
{
    ctc_port_lbk_type_t lbk_type;     /**< port loopback type -> ctc_port_lbk_type_t */
    uint16 src_gport;                 /**< souce port for loopback*/
    uint16 dst_gport;                 /**< destination port for loopback, if equal src_gport, indicat loopback to self, other port to this port is discard*/
    uint8  lbk_enable;                /**< if set, enable/disable loopback*/
    uint8  efm_to_cpu_en;             /**< if set, efm pdu will redirect to cpu*/
    uint8  efm_to_cpu_index;          /**< if set, efm pdu will redirect to cpu*/
};
typedef struct ctc_port_lbk_param_s ctc_port_lbk_param_t;


/**@} end of @defgroup port  */

#endif

