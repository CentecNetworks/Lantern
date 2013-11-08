/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file is the header file for netdev-ctc.c
 */

#ifndef NETDEV_CTC_H
#define NETDEV_CTC_H 1

#include <config.h>

#include "packets.h"
#include "netdev-provider.h"
#include "netdev-vport.h"

struct netdev;
struct netdev_stats;

enum {
    VALID_IFINDEX           = 1 << 0,
    VALID_ETHERADDR         = 1 << 1,
    VALID_IN4               = 1 << 2,
    VALID_IN6               = 1 << 3,
    VALID_MTU               = 1 << 4,
    VALID_POLICING          = 1 << 5,
    VALID_VPORT_STAT_ERROR  = 1 << 6,
    VALID_DRVINFO           = 1 << 7,
    VALID_FEATURES          = 1 << 8,
};

struct if_rx_stats
{
    uint64_t good_ucast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of unicast packets received without error */
    uint64_t good_ucast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of unicast packets received without error */
    uint64_t good_mcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of multicast packets received without error */
    uint64_t good_mcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of multicast packets received without error */
    uint64_t good_bcast_pkts;              /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total number of broadcast packets received without error */
    uint64_t good_bcast_bytes;             /**< length equal to 64B to 1518B(no vlan) or 1522B(vlan),total bytes of broadcast packets received without error */
    uint64_t good_pause_pkts;              /**< total number of pause packets without error */
    uint64_t good_pause_bytes;             /**< total bytes of pause packets without error */
    uint64_t good_control_pkts;            /**< total number of control packets without error excluding the pause packet */
    uint64_t good_control_bytes;           /**< total bytes of control packets without error excluding the pause packet */
    uint64_t jabber_pkts;                  /**< no mac overrun,length greater than MTU,total number of packets with fcs error or alignment error */
    uint64_t jabber_bytes;                 /**< no mac overrun,length greater than MTU,total bytes of packets with fcs error or alignment error */
    uint64_t collision_pkts;               /**< no mac overrun,length less than 64B,total number of packets with fcs error or alignment error */
    uint64_t collision_bytes;              /**< no mac overrun,length less than 64B,total bytes of packets with fcs error or alignment error */
    uint64_t fcs_error_pkts;               /**< no mac overrun,length equal to 64B to MTU,total number of packets with fcs error */
    uint64_t fcs_error_bytes;              /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with fcs error */
    uint64_t alignment_error_pkts;         /**< no mac overrun,length equal to 64B to MTU,total number of packets with alignment error */
    uint64_t alignment_error_bytes;        /**< no mac overrun,length equal to 64B to MTU,total bytes of packets with alignment error */
    uint64_t mac_overrun_pkts;             /**< total number of packets received with mac overrun condition */
    uint64_t mac_overrun_bytes;            /**< total bytes of packets received with mac overrun condition */
    uint64_t good_oversize_pkts;           /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total number of packets received without fcs error and alignment error */
    uint64_t good_oversize_bytes;          /**< no mac overrun,length greater than 1518B(no vlan) or 1522B(vlan),total bytes of packets received without fcs error and alignment error */
    uint64_t good_undersize_pkts;          /**< no mac overrun,length less than 64B,total number of packets received without fcs error and alignment error */
    uint64_t good_undersize_bytes;         /**< no mac overrun,length less than 64B,total bytes of packets received without fcs error and alignment error */
    uint64_t gmac_good_oam_pkts;           /**< total number of packets without error */
    uint64_t gmac_good_oam_bytes;          /**< total bytes of packets without error */
    uint64_t good_63_pkts;                 /**< total number of good packets received with length less than 64B including fcs error and late collision */
    uint64_t good_63_bytes;                /**< total bytes of good packets received with length less than 64B including fcs error and late collision */
    uint64_t bad_63_pkts;                  /**< total number of errored packets received with length less than 64B including fcs error and late collision */
    uint64_t bad_63_bytes;                 /**< total bytes of errored packets received with length less than 64B including fcs error and late collision */
    uint64_t good_1519_pkts;               /**< total number of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64_t good_1519_bytes;              /**< total bytes of good packets received with length equal to 1519B to MTU including fcs error and late collision */
    uint64_t bad_1519_pkts;                /**< total number of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64_t bad_1519_bytes;               /**< total bytes of errored packets received with length equal to 1519B to MTU including fcs error */
    uint64_t good_jumbo_pkts;              /**< total number of good packets received with length greater than MTU including fcs error */
    uint64_t good_jumbo_bytes;             /**< total bytes of good packets received with length greater than MTU including fcs error */
    uint64_t bad_jumbo_pkts;               /**< total number of errored packets received with length greater than MTU including fcs error */
    uint64_t bad_jumbo_bytes;              /**< total bytes of errored packets received with length greater than MTU including fcs error */
    uint64_t pkts_64;                      /**< total number of packets received with length equal to 64B including fcs error and late collision */
    uint64_t bytes_64;                     /**< total bytes of packets received with length equal to 64B including fcs error and late collision */
    uint64_t pkts_65_to_127;               /**< total number of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64_t bytes_65_to_127;              /**< total bytes of packets received with length equal to 65B to 127B including fcs error and late collision */
    uint64_t pkts_128_to_255;              /**< total number of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64_t bytes_128_to_255;             /**< total bytes of packets received with length equal to 128B to 255B including fcs error and late collision */
    uint64_t pkts_256_to_511;              /**< total number of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64_t bytes_256_to_511;             /**< total bytes of packets received with length equal to 256B to 511B including fcs error and late collision */
    uint64_t pkts_512_to_1023;             /**< total number of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64_t bytes_512_to_1023;            /**< total bytes of packets received with length equal to 512B to 1023B including fcs error and late collision */
    uint64_t pkts_1024_to_1518;            /**< total number of packets received with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_t bytes_1024_to_1518;           /**< total bytes of packets received with length equal to 1024B to 1518B including fcs error and late collision */
};

struct if_tx_stats
{
    uint64_t good_ucast_pkts;              /**< total number of unicast packets transmitted without error */
    uint64_t good_ucast_bytes;             /**< total bytes of unicast packets transmitted without error */
    uint64_t good_mcast_pkts;              /**< total number of multicast packets transmitted without error */
    uint64_t good_mcast_bytes;             /**< total bytes of multicast packets transmitted without error */
    uint64_t good_bcast_pkts;              /**< total number of broadcast packets transmitted without error */
    uint64_t good_bcast_bytes;             /**< total bytes of broadcast packets transmitted without error */
    uint64_t good_pause_pkts;              /**< total number of pause packets transmitted without error */
    uint64_t good_pause_bytes;             /**< total bytes of pause packets transmitted without error */
    uint64_t good_control_pkts;            /**< total number of pause packets transmitted without error excluding the pause packet */
    uint64_t good_control_bytes;           /**< total bytes of pause packets transmitted without error excluding the pause packet */
    uint64_t good_oam_pkts;                /**< total number of oam packets transmitted without error */
    uint64_t good_oam_bytes;               /**< total bytes of oam packets transmitted without error */
    uint64_t pkts_63;                      /**< total number of packets transmitted with length less than 64B including fcs error and late collision */
    uint64_t bytes_63;                     /**< total bytes of packets transmitted with length less than 64B including fcs error and late collision */
    uint64_t pkts_64;                      /**< total number of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64_t bytes_64;                     /**< total bytes of packets transmitted with length equal to 64B including fcs error and late collision */
    uint64_t pkts_65_to_127;               /**< total number of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64_t bytes_65_to_127;              /**< total bytes of packets transmitted with length equal to 65B to 127B including fcs error and late collision */
    uint64_t pkts_128_to_255;              /**< total number of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64_t bytes_128_to_255;             /**< total bytes of packets transmitted with length equal to 128B to 255B including fcs error and late collision */
    uint64_t pkts_256_to_511;              /**< total number of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64_t bytes_256_to_511;             /**< total bytes of packets transmitted with length equal to 256B to 511B including fcs error and late collision */
    uint64_t pkts_512_to_1023;             /**< total number of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64_t bytes_512_to_1023;            /**< total bytes of packets transmitted with length equal to 512B to 1023B including fcs error and late collision */
    uint64_t pkts_1024_to_1518;            /**< total number of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_t bytes_1024_to_1518;           /**< total bytes of packets transmitted with length equal to 1024B to 1518B including fcs error and late collision */
    uint64_t pkts_1519;                    /**< total number of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64_t bytes_1519;                   /**< total bytes of packets transmitted with length equal to 1519B to MTU including fcs error and late collision */
    uint64_t jumbo_pkts;                   /**< total number of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64_t jumbo_bytes;                  /**< total bytes of packets transmitted with length greater than MTU including fcs error and late collision */
    uint64_t mac_underrun_pkts;            /**< total number of packets transmitted with mac underrun condition */
    uint64_t mac_underrun_bytes;           /**< total bytes of packets transmitted with mac underrun condition */
    uint64_t fcs_error_pkts;               /**< total number of packets transmitted with fcs error */
    uint64_t fcs_error_bytes;              /**< total bytes of packets transmitted with fcs error */
};

struct netdev_dev_ctc {
    struct netdev_dev netdev_dev;

    struct shash_node *shash_node;
    unsigned int cache_valid;
    unsigned int change_seq;
    
    /* The following are figured out "on demand" only.  They are only valid
     * when the corresponding VALID_* bit in 'cache_valid' is set. */
    int ifindex;
    uint8_t etheraddr[ETH_ADDR_LEN];
    int mtu;
    unsigned int ifi_flags;
    uint32_t kbits_rate;        /* Policing data. */
    uint32_t kbits_burst;
    int vport_stats_error;      /* Cached error code from vport_get_stats().
                                   0 or an errno value. */
    int netdev_mtu_error;       /* Cached error code from SIOCGIFMTU or SIOCSIFMTU. */
    int ether_addr_error;       /* Cached error code from set/get etheraddr. */
    int netdev_policing_error;  /* Cached error code from set policing. */
    int get_features_error;     /* Cached error code from ETHTOOL_GSET. */
    int get_ifindex_error;      /* Cached error code from SIOCGIFINDEX. */
    
    struct if_rx_stats *rx_stats;
    struct if_tx_stats *tx_stats;
    struct netdev_stats *netdev_stats;
};

struct netdev_ctc {
    struct netdev netdev;
    int fd;
};
struct ofputil_queue_prop;

struct netdev_dev_ctc * 
netdev_dev_ctc_cast(const struct netdev_dev *netdev_dev);
struct netdev_ctc *
netdev_ctc_cast(const struct netdev *netdev);
int
netdev_ctc_queue_check_phy_port(const struct netdev *netdev);
void
register_netdev_live_class(void);
void
netdev_ctc_register_unixctl_cmd(void);
#endif
