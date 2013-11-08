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
 * @brief This file defines stats data structure used in adapter layer
 */

#ifndef __OFP_STATS_H__
#define __OFP_STATS_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

/**
 * @brief interface rx statistics data structure
 */
struct ofp_if_rx_stats
{
    uint64_ofp good_ucast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of unicast packets received without error */
    uint64_ofp good_ucast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of unicast packets received without error */
    uint64_ofp good_mcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of multicast packets received without error */
    uint64_ofp good_mcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of multicast packets received without error */
    uint64_ofp good_bcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of broadcast packets received without error */
    uint64_ofp good_bcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of broadcast packets received without error */
    uint64_ofp good_pause_pkts;              /**< total number of pause packets without error */
    uint64_ofp good_pause_bytes;             /**< total bytes of pause packets without error */
    uint64_ofp good_control_pkts;            /**< total number of control packets without error excluding the pause packet */
    uint64_ofp good_control_bytes;           /**< total bytes of control packets without error excluding the pause packet */
    uint64_ofp jabber_pkts;                  /**< no mac overrun,length greater than MTU,total number of packets with fcs error or alignment error */
    uint64_ofp jabber_bytes;                 /**< no mac overrun,length greater than MTU,total bytes of packets with fcs error or alignment error */
    uint64_ofp collision_pkts;               /**< no mac overrun,length less than 64B,total number of packets with fcs error or alignment error */
    uint64_ofp collision_bytes;              /**< no mac overrun,length less than 64B,total bytes of packets with fcs error or alignment error */
    uint64_ofp fcs_error_pkts;               /**< no mac overrun,length equal to 64B to MTU,total number of packets with fcs error */
    uint64_ofp fcs_error_bytes;              /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with fcs error */
    uint64_ofp alignment_error_pkts;         /**< no mac overrun,length equal to 64B to MTU,total number of packets with alignment error */
    uint64_ofp alignment_error_bytes;        /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with alignment error */
    uint64_ofp mac_overrun_pkts;             /**< total number of packets received with mac overrun condition */
    uint64_ofp mac_overrun_bytes;            /**< total bytes of packets received with mac overrun condition */
    uint64_ofp good_oversize_pkts;           /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total number of packets received without fcs error and alignment error */
    uint64_ofp good_oversize_bytes;          /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total bytes of packets received without fcs error and alignment error */
    uint64_ofp good_undersize_pkts;          /**< no mac overrun,length less than 64B,total number of packets received without fcs error and alignment error */
    uint64_ofp good_undersize_bytes;         /**< no mac overrun,length less than 64B,total bytes of packets received without fcs error and alignment error */
    uint64_ofp gmac_good_oam_pkts;           /**< total number of packets without error */
    uint64_ofp gmac_good_oam_bytes;          /**< total bytes of packets without error */
    uint64_ofp good_63_pkts;                 /**< total number of good packets received with length less than 64B including fcs error and late collision */
    uint64_ofp good_63_bytes;                /**< total bytes of good packets received with length less than 64B including fcs error and late collision */
    uint64_ofp bad_63_pkts;                  /**< total number of errored packets received with length less than 64B including fcs error and late collision */
    uint64_ofp bad_63_bytes;                 /**< total bytes of errored packets received with length less than 64B including fcs error and late collision */
    uint64_ofp good_1519_pkts;               /**< total number of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64_ofp good_1519_bytes;              /**< total bytes of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64_ofp bad_1519_pkts;                /**< total number of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64_ofp bad_1519_bytes;               /**< total bytes of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64_ofp good_jumbo_pkts;              /**< total number of good packets received with length greater than MTU including fcs error */
    uint64_ofp good_jumbo_bytes;             /**< total bytes of good packets received with length greater than MTU including fcs error */
    uint64_ofp bad_jumbo_pkts;               /**< total number of errored packets received with length greater than MTU including fcs error */
    uint64_ofp bad_jumbo_bytes;              /**< total bytes of errored packets received with length greater than MTU including fcs error */
    uint64_ofp pkts_64;                      /**< total number of packets received with length equal to 64B including fcs error and late collision */
    uint64_ofp bytes_64;                     /**< total bytes of packets received with length equal to 64B including fcs error and late collision */
    uint64_ofp pkts_65_to_127;               /**< total number of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64_ofp bytes_65_to_127;              /**< total bytes of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64_ofp pkts_128_to_255;              /**< total number of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64_ofp bytes_128_to_255;             /**< total bytes of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64_ofp pkts_256_to_511;              /**< total number of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64_ofp bytes_256_to_511;             /**< total bytes of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64_ofp pkts_512_to_1023;             /**< total number of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64_ofp bytes_512_to_1023;            /**< total bytes of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64_ofp pkts_1024_to_1518;            /**< total number of packets received with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_ofp bytes_1024_to_1518;           /**< total bytes of packets received with length equal to 1024B to 1518B including fcs error and late collision */
};
typedef struct ofp_if_rx_stats ofp_if_rx_stats_t;

/**
 * @brief interface tx statistics data structure
 */
struct ofp_if_tx_stats
{
    uint64_ofp good_ucast_pkts;              /**< total number of unicast packets transmitted without error */
    uint64_ofp good_ucast_bytes;             /**< total bytes of unicast packets transmitted without error */
    uint64_ofp good_mcast_pkts;              /**< total number of multicast packets transmitted without error */
    uint64_ofp good_mcast_bytes;             /**< total bytes of multicast packets transmitted without error */
    uint64_ofp good_bcast_pkts;              /**< total number of broadcast packets transmitted without error */
    uint64_ofp good_bcast_bytes;             /**< total bytes of broadcast packets transmitted without error */
    uint64_ofp good_pause_pkts;              /**< total number of pause packets transmitted without error */
    uint64_ofp good_pause_bytes;             /**< total bytes of pause packets transmitted without error */
    uint64_ofp good_control_pkts;            /**< total number of pause packets transmitted without error excluding the pause packet */
    uint64_ofp good_control_bytes;           /**< total bytes of pause packets transmitted without error excluding the pause packet */
    uint64_ofp good_oam_pkts;                /**< total number of oam packets transmitted without error */
    uint64_ofp good_oam_bytes;               /**< total bytes of oam packets transmitted without error */
    uint64_ofp pkts_63;                      /**< total number of packets transmitted with length less than 64B including fcs error and late collision */
    uint64_ofp bytes_63;                     /**< total bytes of packets transmitted with length less than 64B including fcs error and late collision */
    uint64_ofp pkts_64;                      /**< total number of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64_ofp bytes_64;                     /**< total bytes of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64_ofp pkts_65_to_127;               /**< total number of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64_ofp bytes_65_to_127;              /**< total bytes of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64_ofp pkts_128_to_255;              /**< total number of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64_ofp bytes_128_to_255;             /**< total bytes of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64_ofp pkts_256_to_511;              /**< total number of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64_ofp bytes_256_to_511;             /**< total bytes of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64_ofp pkts_512_to_1023;             /**< total number of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64_ofp bytes_512_to_1023;            /**< total bytes of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64_ofp pkts_1024_to_1518;            /**< total number of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_ofp bytes_1024_to_1518;           /**< total bytes of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_ofp pkts_1519;                    /**< total number of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64_ofp bytes_1519;                   /**< total bytes of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64_ofp jumbo_pkts;                   /**< total number of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64_ofp jumbo_bytes;                  /**< total bytes of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64_ofp mac_underrun_pkts;            /**< total number of packets transmitted with mac underrun condition */
    uint64_ofp mac_underrun_bytes;           /**< total bytes of packets transmitted with mac underrun condition */
    uint64_ofp fcs_error_pkts;               /**< total number of packets transmitted with fcs error */
    uint64_ofp fcs_error_bytes;              /**< total bytes of packets transmitted with fcs error */
};
typedef struct ofp_if_tx_stats ofp_if_tx_stats_t;

/**
 * @brief interface statistics data structure
 */
struct ofp_if_stats_s
{
    ofp_if_rx_stats_t rx_stats;             /**< rx stats */
    ofp_if_tx_stats_t tx_stats;             /**< tx stats */
};
typedef struct ofp_if_stats_s ofp_if_stats_t;

/**
 * @brief flow statistics data structure
 */
struct ofp_stats_s
{
    uint64_ofp packet_count;                  /**< total number of packets */
    uint64_ofp byte_count;                    /**< total bytes of packets */
};
typedef struct ofp_stats_s ofp_stats_t;

struct ofp_queue_stats_s {
    /* Values of unsupported statistics are set to all-1-bits (UINT64_MAX). */
    uint64_t tx_bytes;
    uint64_t tx_packets;
    uint64_t tx_errors;
};
typedef struct ofp_queue_stats_s ofp_queue_stats_t;

#endif /* !__OFP_STATS_H__ */
