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
 * @brief This file contains the implementations of netdev
 */

#include <config.h>

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <linux/if_ether.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <linux/version.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_packet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "coverage.h"
#include "dpif-linux.h"
#include "dynamic-string.h"
#include "fatal-signal.h"
#include "hash.h"
#include "hmap.h"
#include "ofpbuf.h"
#include "openflow/openflow.h"
#include "poll-loop.h"
#include "rtnetlink-link.h"
#include "socket-util.h"
#include "shash.h"
#include "sset.h"
#include "timer.h"
#include "vlog.h"
#include "unixctl.h"

#include "netdev-ctc.h"
#include "ofp_api.h"

VLOG_DEFINE_THIS_MODULE(netdev_ctc);

/* Sockets used for ioctl operations. */
static int af_inet_sock = -1;   /* AF_INET, SOCK_DGRAM. */
static struct shash ctc_netdev_devs = SHASH_INITIALIZER(&ctc_netdev_devs);

/* This is set pretty low because we probably won't learn anything from the
 * additional log messages. */
static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(5, 20);

static int netdev_ctc_create(const struct netdev_class *, const char *,
                               struct netdev_dev **);

int do_get_ifindex(const char *netdev_name);

static bool
is_netdev_ctc_class(const struct netdev_class *class)
{
    return class->create == netdev_ctc_create;
}

struct netdev_dev_ctc * 
netdev_dev_ctc_cast(const struct netdev_dev *netdev_dev)
{
    assert(is_netdev_ctc_class(netdev_dev_get_class(netdev_dev)));
    return CONTAINER_OF(netdev_dev, struct netdev_dev_ctc, netdev_dev);
}

struct netdev_ctc *
netdev_ctc_cast(const struct netdev *netdev)
{
    struct netdev_dev *netdev_dev = netdev_get_dev(netdev);
    assert(is_netdev_ctc_class(netdev_dev_get_class(netdev_dev)));
    return CONTAINER_OF(netdev, struct netdev_ctc, netdev);
}

static int
netdev_ctc_do_ioctl(const char *name, struct ifreq *ifr, int cmd,
                      const char *cmd_name)
{
    ovs_strzcpy(ifr->ifr_name, name, sizeof ifr->ifr_name);
    if (ioctl(af_inet_sock, cmd, ifr) == -1) {
        VLOG_WARN("%s: ioctl(%s) failed: %s", name, cmd_name,
                     strerror(errno));
    }
    return 0;
}

static int
get_flags(const struct netdev_dev *dev, unsigned int *flags)
{
    struct ifreq ifr;
    int error;

    *flags = 0;
    error = netdev_ctc_do_ioctl(dev->name, &ifr, SIOCGIFFLAGS,
                                  "SIOCGIFFLAGS");
    if (!error) {
        *flags = ifr.ifr_flags;
    }
    return error;
}

static int
set_flags(struct netdev *netdev, unsigned int flags)
{
    struct ifreq ifr;

    ifr.ifr_flags = flags;
    return netdev_ctc_do_ioctl(netdev_get_name(netdev), &ifr, SIOCSIFFLAGS,
                                 "SIOCSIFFLAGS");
}

static int
netdev_ctc_init(void)
{
    if (af_inet_sock < 0) {
        af_inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    }

    return 0;
}

static int
netdev_ctc_create(const struct netdev_class *class, const char *name,
                    struct netdev_dev **netdev_devp)
{
    struct netdev_dev_ctc *netdev_dev;

    netdev_dev = xzalloc(sizeof *netdev_dev);
    netdev_dev->change_seq = 1;
    netdev_dev_init(&netdev_dev->netdev_dev, name, class);
    get_flags(&netdev_dev->netdev_dev, &netdev_dev->ifi_flags);
    netdev_dev->rx_stats = xzalloc(sizeof *netdev_dev->rx_stats);
    netdev_dev->tx_stats = xzalloc(sizeof *netdev_dev->tx_stats);
    *netdev_devp = &netdev_dev->netdev_dev;
    return 0;
}

static void
netdev_ctc_destroy(struct netdev_dev *netdev_dev_)
{
    struct netdev_dev_ctc *netdev_dev = netdev_dev_ctc_cast(netdev_dev_);

    free(netdev_dev->rx_stats);
    free(netdev_dev->tx_stats);
    free(netdev_dev);
}

static int
netdev_ctc_open(struct netdev_dev *netdev_dev_, struct netdev **netdevp)
{
    struct netdev_ctc *netdev;
    enum netdev_flags flags;
    int error;

    /* Allocate network device. */
    netdev = xzalloc(sizeof *netdev);
    netdev_init(&netdev->netdev, netdev_dev_);
    netdev->fd = -1;
    
    error = netdev_get_flags(&netdev->netdev, &flags);
    if (error == ENODEV) {
        goto error;
    }

    *netdevp = &netdev->netdev;
    return 0;

error:
    netdev_uninit(&netdev->netdev, true);
    return error;
}

static void
netdev_ctc_close(struct netdev *netdev_)
{
    struct netdev_ctc *netdev = netdev_ctc_cast(netdev_);
    free(netdev);
}

int
do_get_ifindex(const char *netdev_name)
{
    struct ifreq ifr;

    if (af_inet_sock < 0) {
        af_inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    }
    
    ovs_strzcpy(ifr.ifr_name, netdev_name, sizeof ifr.ifr_name);
    if (ioctl(af_inet_sock, SIOCGIFINDEX, &ifr) < 0) {
        VLOG_WARN_RL(&rl, "ioctl(SIOCGIFINDEX) on %s device failed: %s",
                     netdev_name, strerror(errno));
        return -errno;
    }
    return ifr.ifr_ifindex;
}

static int
get_ifindex(const struct netdev *netdev_, int *ifindexp)
{
    struct netdev_dev_ctc *netdev_dev =
                                netdev_dev_ctc_cast(netdev_get_dev(netdev_));

    if (!(netdev_dev->cache_valid & VALID_IFINDEX)) {
        int ifindex = do_get_ifindex(netdev_get_name(netdev_));

        if (ifindex < 0) {
            netdev_dev->get_ifindex_error = -ifindex;
            netdev_dev->ifindex = 0;
        } else {
            netdev_dev->get_ifindex_error = 0;
            netdev_dev->ifindex = ifindex;
        }
        netdev_dev->cache_valid |= VALID_IFINDEX;
    }

    *ifindexp = netdev_dev->ifindex;
    return netdev_dev->get_ifindex_error;
}

static int
get_etheraddr(const char *netdev_name, uint8_t ea[ETH_ADDR_LEN])
{
    struct ifreq ifr;
    int hwaddr_family;

    memset(&ifr, 0, sizeof ifr);
    ovs_strzcpy(ifr.ifr_name, netdev_name, sizeof ifr.ifr_name);
    if (ioctl(af_inet_sock, SIOCGIFHWADDR, &ifr) < 0) {
        /* ENODEV probably means that a vif disappeared asynchronously and
         * hasn't been removed from the database yet, so reduce the log level
         * to INFO for that case. */
        VLOG(errno == ENODEV ? VLL_INFO : VLL_ERR,
             "ioctl(SIOCGIFHWADDR) on %s device failed: %s",
             netdev_name, strerror(errno));
        return errno;
    }
    hwaddr_family = ifr.ifr_hwaddr.sa_family;
    if (hwaddr_family != AF_UNSPEC && hwaddr_family != ARPHRD_ETHER) {
        VLOG_WARN("%s device has unknown hardware address family %d",
                  netdev_name, hwaddr_family);
    }
    memcpy(ea, ifr.ifr_hwaddr.sa_data, ETH_ADDR_LEN);
    return 0;
}

/* Returns an AF_PACKET raw socket or a negative errno value. */
static int
af_packet_sock(void)
{
    static int sock = INT_MIN;

    if (sock == INT_MIN) {
        sock = socket(AF_PACKET, SOCK_RAW, 0);
        if (sock >= 0) {
            set_nonblocking(sock);
        } else {
            sock = -errno;
            VLOG_ERR("failed to create packet socket: %s", strerror(errno));
        }
    }

    return sock;
}

/* Sends 'buffer' on 'netdev'.  Returns 0 if successful, otherwise a positive
 * errno value.  Returns EAGAIN without blocking if the packet cannot be queued
 * immediately.  Returns EMSGSIZE if a partial packet was transmitted or if
 * the packet is too big or too small to transmit on the device.
 *
 * The caller retains ownership of 'buffer' in all cases.
 *
 * The kernel maintains a packet transmission queue, so the caller is not
 * expected to do additional queuing of packets. */
static int
netdev_ctc_send(struct netdev *netdev_, const void *data, size_t size)
{
    struct netdev_ctc *netdev = netdev_ctc_cast(netdev_);
    for (;;) {
        ssize_t retval;

        if (netdev->fd < 0) {
            /* Use our AF_PACKET socket to send to this device. */
            struct sockaddr_ll sll;
            struct msghdr msg;
            struct iovec iov;
            int ifindex;
            int error;
            int sock;

            sock = af_packet_sock();
            if (sock < 0) {
                return sock;
            }

            error = get_ifindex(netdev_, &ifindex);
            if (error) {
                return error;
            }

            /* We don't bother setting most fields in sockaddr_ll because the
             * kernel ignores them for SOCK_RAW. */
            memset(&sll, 0, sizeof sll);
            sll.sll_family = AF_PACKET;
            sll.sll_ifindex = ifindex;

            iov.iov_base = CONST_CAST(void *, data);
            iov.iov_len = size;

            msg.msg_name = &sll;
            msg.msg_namelen = sizeof sll;
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = NULL;
            msg.msg_controllen = 0;
            msg.msg_flags = 0;

            retval = sendmsg(sock, &msg, 0);
        } else {
            /* Use the netdev's own fd to send to this device.  This is
             * essential for tap devices, because packets sent to a tap device
             * with an AF_PACKET socket will loop back to be *received* again
             * on the tap device. */
            retval = write(netdev->fd, data, size);
        }

        if (retval < 0) {
            /* The Linux AF_PACKET implementation never blocks waiting for room
             * for packets, instead returning ENOBUFS.  Translate this into
             * EAGAIN for the caller. */
            if (errno == ENOBUFS) {
                return EAGAIN;
            } else if (errno == EINTR) {
                continue;
            } else if (errno != EAGAIN) {
                VLOG_WARN_RL(&rl, "error sending Ethernet packet on %s: %s",
                             netdev_get_name(netdev_), strerror(errno));
            }
            return errno;
        } else if (retval != size) {
            VLOG_WARN_RL(&rl, "sent partial Ethernet packet (%zd bytes of "
                         "%zu) on %s", retval, size, netdev_get_name(netdev_));
            return EMSGSIZE;
        } else {
            return 0;
        }
    }
    return 0;
}

/* Copies 'netdev''s MAC address to 'mac' which is passed as param. */
static int
netdev_ctc_get_etheraddr(const struct netdev *netdev_,
                           uint8_t mac[ETH_ADDR_LEN])
{
    struct netdev_dev_ctc *netdev_dev =
                                netdev_dev_ctc_cast(netdev_get_dev(netdev_));

    if (!(netdev_dev->cache_valid & VALID_ETHERADDR)) {
        int error = get_etheraddr(netdev_get_name(netdev_),
                                  netdev_dev->etheraddr);

        netdev_dev->ether_addr_error = error;
        netdev_dev->cache_valid |= VALID_ETHERADDR;
    }

    if (!netdev_dev->ether_addr_error) {
        memcpy(mac, netdev_dev->etheraddr, ETH_ADDR_LEN);
    }

    return netdev_dev->ether_addr_error;
}

static int
netdev_ctc_get_carrier(const struct netdev *netdev_, bool *carrier)
{
    int ret = 0;
    uint8_t ofp_carrier = false;
    const char * ifname  = NULL;

    ifname = netdev_get_name(netdev_);
    ret = ofp_netdev_get_port_carrier(ifname, &ofp_carrier);
    if (ret < 0) { 
        VLOG_ERR("ifname %s: failed to get carrier", ifname);
        return ENODEV; 
    }
    
    *carrier = ofp_carrier;

    return 0;
}

static int
netdev_ctc_get_stats(const struct netdev *netdev, struct netdev_stats *stats)
{
    int32_t ret = 0;
    uint32_t supported = 0;
    struct ofp_if_stats_s if_stats;
    char * ifname  = NULL;
    uint64_t max_mac_stats;
    struct netdev_dev_ctc *netdev_ctc = netdev_dev_ctc_cast(netdev_get_dev(netdev));

    ifname = (char *)netdev_get_name(netdev);
    memset(&if_stats, 0, sizeof if_stats);
    ret = ofp_netdev_get_port_stats(ifname, &if_stats);
    if (ret < 0) {
        return ENODEV;
    }

    ofp_netdev_get_port_supported(ifname, &supported);
    if (supported & OFPPF_10GB_FD) {
        max_mac_stats = 0xffffffffffLLU;
    }
    else {
        max_mac_stats = 0x0fffffffffLLU;
    }

#define PLUS_MAC_STATS(sum, a)                                          \
do{                                                                     \
    sum = ((sum&max_mac_stats ) <= (max_mac_stats&a)) ?                 \
          ((sum&(~max_mac_stats)) +  (max_mac_stats&a)) :              \
          ((sum&(~max_mac_stats)) +  ((max_mac_stats+1)|a));           \
}while(0)

    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_ucast_pkts       , if_stats.rx_stats.good_ucast_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_ucast_bytes      , if_stats.rx_stats.good_ucast_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_mcast_pkts       , if_stats.rx_stats.good_mcast_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_mcast_bytes      , if_stats.rx_stats.good_mcast_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_bcast_pkts       , if_stats.rx_stats.good_bcast_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_bcast_bytes      , if_stats.rx_stats.good_bcast_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_pause_pkts       , if_stats.rx_stats.good_pause_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_pause_bytes      , if_stats.rx_stats.good_pause_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_control_pkts     , if_stats.rx_stats.good_control_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_control_bytes    , if_stats.rx_stats.good_control_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->jabber_pkts           , if_stats.rx_stats.jabber_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->jabber_bytes          , if_stats.rx_stats.jabber_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->collision_pkts        , if_stats.rx_stats.collision_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->collision_bytes       , if_stats.rx_stats.collision_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->fcs_error_pkts        , if_stats.rx_stats.fcs_error_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->fcs_error_bytes       , if_stats.rx_stats.fcs_error_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->alignment_error_pkts  , if_stats.rx_stats.alignment_error_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->alignment_error_bytes , if_stats.rx_stats.alignment_error_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->mac_overrun_pkts      , if_stats.rx_stats.mac_overrun_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->mac_overrun_bytes     , if_stats.rx_stats.mac_overrun_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_oversize_pkts    , if_stats.rx_stats.good_oversize_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_oversize_bytes   , if_stats.rx_stats.good_oversize_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_undersize_pkts   , if_stats.rx_stats.good_undersize_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_undersize_bytes  , if_stats.rx_stats.good_undersize_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->gmac_good_oam_pkts    , if_stats.rx_stats.gmac_good_oam_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->gmac_good_oam_bytes   , if_stats.rx_stats.gmac_good_oam_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_63_pkts          , if_stats.rx_stats.good_63_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_63_bytes         , if_stats.rx_stats.good_63_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_63_pkts           , if_stats.rx_stats.bad_63_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_63_bytes          , if_stats.rx_stats.bad_63_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_1519_pkts        , if_stats.rx_stats.good_1519_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_1519_bytes       , if_stats.rx_stats.good_1519_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_1519_pkts         , if_stats.rx_stats.bad_1519_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_1519_bytes        , if_stats.rx_stats.bad_1519_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_jumbo_pkts       , if_stats.rx_stats.good_jumbo_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->good_jumbo_bytes      , if_stats.rx_stats.good_jumbo_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_jumbo_pkts        , if_stats.rx_stats.bad_jumbo_pkts);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bad_jumbo_bytes       , if_stats.rx_stats.bad_jumbo_bytes);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_64               , if_stats.rx_stats.pkts_64);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_64              , if_stats.rx_stats.bytes_64);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_65_to_127        , if_stats.rx_stats.pkts_65_to_127);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_65_to_127       , if_stats.rx_stats.bytes_65_to_127);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_128_to_255       , if_stats.rx_stats.pkts_128_to_255);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_128_to_255      , if_stats.rx_stats.bytes_128_to_255);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_256_to_511       , if_stats.rx_stats.pkts_256_to_511);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_256_to_511      , if_stats.rx_stats.bytes_256_to_511);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_512_to_1023      , if_stats.rx_stats.pkts_512_to_1023);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_512_to_1023     , if_stats.rx_stats.bytes_512_to_1023);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->pkts_1024_to_1518     , if_stats.rx_stats.pkts_1024_to_1518);
    PLUS_MAC_STATS(netdev_ctc->rx_stats->bytes_1024_to_1518    , if_stats.rx_stats.bytes_1024_to_1518);

    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_ucast_pkts       , if_stats.tx_stats.good_ucast_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_ucast_bytes      , if_stats.tx_stats.good_ucast_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_mcast_pkts       , if_stats.tx_stats.good_mcast_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_mcast_bytes      , if_stats.tx_stats.good_mcast_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_bcast_pkts       , if_stats.tx_stats.good_bcast_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_bcast_bytes      , if_stats.tx_stats.good_bcast_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_pause_pkts       , if_stats.tx_stats.good_pause_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_pause_bytes      , if_stats.tx_stats.good_pause_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_control_pkts     , if_stats.tx_stats.good_control_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_control_bytes    , if_stats.tx_stats.good_control_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_oam_pkts         , if_stats.tx_stats.good_oam_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->good_oam_bytes        , if_stats.tx_stats.good_oam_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_63               , if_stats.tx_stats.pkts_63);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_63              , if_stats.tx_stats.bytes_63);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_64               , if_stats.tx_stats.pkts_64);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_64              , if_stats.tx_stats.bytes_64);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_65_to_127        , if_stats.tx_stats.pkts_65_to_127);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_65_to_127       , if_stats.tx_stats.bytes_65_to_127);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_128_to_255       , if_stats.tx_stats.pkts_128_to_255);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_128_to_255      , if_stats.tx_stats.bytes_128_to_255);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_256_to_511       , if_stats.tx_stats.pkts_256_to_511);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_256_to_511      , if_stats.tx_stats.bytes_256_to_511);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_512_to_1023      , if_stats.tx_stats.pkts_512_to_1023);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_512_to_1023     , if_stats.tx_stats.bytes_512_to_1023);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_1024_to_1518     , if_stats.tx_stats.pkts_1024_to_1518);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_1024_to_1518    , if_stats.tx_stats.bytes_1024_to_1518);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->pkts_1519             , if_stats.tx_stats.pkts_1519);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->bytes_1519            , if_stats.tx_stats.bytes_1519);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->jumbo_pkts            , if_stats.tx_stats.jumbo_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->jumbo_bytes           , if_stats.tx_stats.jumbo_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->mac_underrun_pkts     , if_stats.tx_stats.mac_underrun_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->mac_underrun_bytes    , if_stats.tx_stats.mac_underrun_bytes);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->fcs_error_pkts        , if_stats.tx_stats.fcs_error_pkts);
    PLUS_MAC_STATS(netdev_ctc->tx_stats->fcs_error_bytes       , if_stats.tx_stats.fcs_error_bytes);

#undef PLUS_MAC_STATS

    memset(stats, 0, sizeof *stats);

    stats->rx_dropped = UINT64_MAX; /* TODO: Not Support */
    stats->tx_dropped = UINT64_MAX; /* TODO: Not Support */

    /* Detailed receive errors. */
    stats->rx_fifo_errors = UINT64_MAX; /* TODO: Not Support */
    stats->rx_length_errors = UINT64_MAX; /* TODO: Not Support */
    stats->rx_missed_errors = UINT64_MAX; /* TODO: Not Support */

    /* Detailed receive errors. */
    stats->tx_aborted_errors = UINT64_MAX; /* TODO: Not Support */
    stats->tx_carrier_errors = UINT64_MAX; /* TODO: Not Support */
    stats->tx_fifo_errors = UINT64_MAX; /* TODO: Not Support */
    stats->tx_heartbeat_errors = UINT64_MAX; /* TODO: Not Support */
    stats->tx_window_errors = UINT64_MAX; /* TODO: Not Support */

    stats->rx_packets += netdev_ctc->rx_stats->good_63_pkts;
    stats->rx_packets += netdev_ctc->rx_stats->bad_63_pkts;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_64;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_65_to_127;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_128_to_255;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_256_to_511;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_512_to_1023;
    stats->rx_packets += netdev_ctc->rx_stats->pkts_1024_to_1518;
    stats->rx_packets += netdev_ctc->rx_stats->good_1519_pkts;
    stats->rx_packets += netdev_ctc->rx_stats->bad_1519_pkts;
    stats->rx_packets += netdev_ctc->rx_stats->good_jumbo_pkts;
    stats->rx_packets += netdev_ctc->rx_stats->bad_jumbo_pkts;

    stats->rx_bytes += netdev_ctc->rx_stats->good_63_bytes;
    stats->rx_bytes += netdev_ctc->rx_stats->bad_63_bytes;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_64;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_65_to_127;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_128_to_255;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_256_to_511;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_512_to_1023;
    stats->rx_bytes += netdev_ctc->rx_stats->bytes_1024_to_1518;
    stats->rx_bytes += netdev_ctc->rx_stats->good_1519_bytes;
    stats->rx_bytes += netdev_ctc->rx_stats->bad_1519_bytes;
    stats->rx_bytes += netdev_ctc->rx_stats->good_jumbo_bytes;
    stats->rx_bytes += netdev_ctc->rx_stats->bad_jumbo_bytes;

    stats->rx_errors += netdev_ctc->rx_stats->good_undersize_pkts;
    stats->rx_errors += netdev_ctc->rx_stats->jabber_pkts;
    stats->rx_errors += netdev_ctc->rx_stats->collision_pkts;
    stats->rx_errors += netdev_ctc->rx_stats->fcs_error_pkts;
    stats->rx_errors += netdev_ctc->rx_stats->alignment_error_pkts;

    stats->rx_over_errors = netdev_ctc->rx_stats->mac_overrun_pkts;
    stats->collisions = netdev_ctc->rx_stats->collision_pkts;
    stats->multicast = netdev_ctc->rx_stats->good_mcast_pkts;
    
    /* Detailed receive errors. */
    stats->rx_crc_errors = netdev_ctc->rx_stats->fcs_error_pkts;
    stats->rx_frame_errors = netdev_ctc->rx_stats->fcs_error_pkts;

    stats->tx_packets += netdev_ctc->tx_stats->pkts_63;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_64;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_65_to_127;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_128_to_255;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_256_to_511;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_512_to_1023;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_1024_to_1518;
    stats->tx_packets += netdev_ctc->tx_stats->pkts_1519;
    stats->tx_packets += netdev_ctc->tx_stats->jumbo_pkts;

    stats->tx_bytes += netdev_ctc->tx_stats->bytes_63;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_64;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_65_to_127;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_128_to_255;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_256_to_511;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_512_to_1023;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_1024_to_1518;
    stats->tx_bytes += netdev_ctc->tx_stats->bytes_1519;
    stats->tx_bytes += netdev_ctc->tx_stats->jumbo_bytes;

    stats->tx_errors = netdev_ctc->tx_stats->fcs_error_pkts;
    return 0;
}

static int
netdev_ctc_set_stats(struct netdev *netdev, const struct netdev_stats *stats)
{
    netdev = netdev;
    stats = stats;
    return 0;
}

static int 
netdev_ctc_get_features(const struct netdev *netdev,
                         enum netdev_features *current,
                         enum netdev_features *advertised,
                         enum netdev_features *supported,
                         enum netdev_features *peer)
{
    int32_ofp ret = 0;
    uint32_ofp ofp_current = 0;
    uint32_ofp ofp_advertised = 0;
    uint32_ofp ofp_supported = 0;
    uint32_ofp ofp_peer = 0;
    char * ifname  = NULL;

    ifname = (char *)netdev_get_name(netdev);
    ret = ofp_netdev_get_port_features(ifname, 
                                       &ofp_current, 
                                       &ofp_advertised, 
                                       &ofp_supported, 
                                       &ofp_peer);
    if (ret) {
        VLOG_ERR("ifname %s: failed to get port features", ifname);
        return ENODEV;
    }
    
    *current = ofp_current;
    *advertised = ofp_advertised;
    *supported = ofp_supported;
    *peer = ofp_peer;

    return 0;
}

static int 
netdev_ctc_set_advertisements(struct netdev *netdev,
                              enum netdev_features advertise)
{
    const char * ifname  = NULL;
    int32_ofp ret = 0;

    ifname = netdev_get_name(netdev);

    ret = ofp_netdev_set_port_advertised(ifname, advertise);
    if (ret) {
        VLOG_ERR("ifname %s: failed to get port features", ifname);
        return ENODEV;
    }

    return 0;
}

int
netdev_ctc_queue_check_phy_port(const struct netdev *netdev)
{
    struct netdev_dev *net_dev;
    bool phy_port = false;
    
    net_dev = netdev_get_dev(netdev);
    phy_port = is_netdev_ctc_class(netdev_dev_get_class(net_dev));

    return phy_port;
}

static int
nd_to_iff_flags(enum netdev_flags nd)
{
    int iff = 0;
    if (nd & NETDEV_UP) {
        iff |= IFF_UP;
    }
    if (nd & NETDEV_PROMISC) {
        iff |= IFF_PROMISC;
    }
    return iff;
}

static int
iff_to_nd_flags(int iff)
{
    enum netdev_flags nd = 0;
    if (iff & IFF_UP) {
        nd |= NETDEV_UP;
    }
    if (iff & IFF_PROMISC) {
        nd |= NETDEV_PROMISC;
    }
    return nd;
}

static int
netdev_ctc_update_flags(struct netdev *netdev,
                          enum netdev_flags off, enum netdev_flags on,
                          enum netdev_flags *old_flagsp)
{
    struct netdev_dev_ctc *netdev_dev;
    int old_flags, new_flags;
    int error = 0;
    bool port_changed = false;

    ofp_netdev_get_port_modified(netdev->netdev_dev->name, &port_changed);
    netdev_dev = netdev_dev_ctc_cast(netdev_get_dev(netdev));
    old_flags = netdev_dev->ifi_flags;
    *old_flagsp = iff_to_nd_flags(old_flags);
    new_flags = (old_flags & ~nd_to_iff_flags(off)) | nd_to_iff_flags(on);
    if (new_flags != old_flags) {
        if (off == NETDEV_UP) {
            ofp_netdev_disable_interface(netdev_dev->netdev_dev.name);
        }

        if (on == NETDEV_UP) {
            ofp_netdev_enable_interface(netdev_dev->netdev_dev.name);
        }

        error = set_flags(netdev, new_flags);
        get_flags(&netdev_dev->netdev_dev, &netdev_dev->ifi_flags);
    }
    else if (true == port_changed) {
        get_flags(&netdev_dev->netdev_dev, &netdev_dev->ifi_flags);
    }

    return error;
}

static unsigned int
netdev_ctc_change_seq(const struct netdev *netdev)
{
    /* XXX: change_seq is not set currently. */
    return netdev_dev_ctc_cast(netdev_get_dev(netdev))->change_seq;
}

const struct netdev_class netdev_ctc_class = {
    OFP_DEFAULT_NETDEV_TYPE,
    netdev_ctc_init,                                
    NULL,                                       /* run */                          
    NULL,                                       /* wait */
    netdev_ctc_create,
    netdev_ctc_destroy,
    NULL,                                       /* get_config */
    NULL,                                       /* set_config */
    NULL,                                       /* get_tunnel_config */
    netdev_ctc_open,
    netdev_ctc_close,
    NULL,                                       /* listen */
    NULL,                                       /* recv */
    NULL,                                       /* recv_wait */
    NULL,                                       /* drain */
    netdev_ctc_send,                            /* send */                             
    NULL,                                       /* send_wait */                         
    NULL,                                       /* set_etheraddr */
    netdev_ctc_get_etheraddr,                       
    NULL,                                       /* get_mtu */
    NULL,                                       /* set_mtu */
    NULL,                                       /* get_ifindex */
    netdev_ctc_get_carrier,                     /* carrier is used to determine
                                                 * port state and this is requi-
                                                 * red in OpenFlow spec, we must
                                                 * support it. */                         
    NULL,                                       /* carrier stats is used as
                                                 * link_resets count and would
                                                 * be stored in ovsdb, this is
                                                 * not required in OpenFlow
                                                 * spec, wo do not support it.*/
    NULL,                                       /* get_miimon */
    netdev_ctc_get_stats,
    netdev_ctc_set_stats,
    netdev_ctc_get_features,         
    netdev_ctc_set_advertisements,    
    NULL,                                       /* set_policing */
    NULL,                                       /* get_qos_types */
    NULL,                                       /* get_qos_capabilities */
    NULL,                                       /* get_qos */
    NULL,                                       /* set_qos */
    NULL,                                       /* get_queue */
    NULL,                                       /* set_queue */
    NULL,                                       /* delete_queue */
    NULL,                                       /* get_queue_stats */
    NULL,                                       /* dump_queues */
    NULL,                                       /* dump_queue_stats */
    NULL,                                       /* get_in4 */
    NULL,                                       /* set_in4 */
    NULL,                                       /* get_in6 */
    NULL,                                       /* add_router */
    NULL,                                       /* get_next_hop */
    NULL,                                       /* get_status */
    NULL,                                       /* arp_lookup */
    netdev_ctc_update_flags,
    netdev_ctc_change_seq
};

extern struct netdev_class netdev_live_class;
void register_netdev_live_class(void)
{
    memcpy(&netdev_live_class, &netdev_ctc_class, 
           sizeof(struct netdev_class));    
}
