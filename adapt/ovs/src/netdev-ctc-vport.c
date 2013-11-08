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
 * @brief This file contains the implementations of netdev vport
 */

#include <config.h>

#include "netdev-vport.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "byte-order.h"
#include "daemon.h"
#include "dirs.h"
#include "dpif.h"
#include "hash.h"
#include "hmap.h"
#include "list.h"
#include "netdev-provider.h"
#include "ofpbuf.h"
#include "packets.h"
#include "shash.h"
#include "socket-util.h"
#include "vlog.h"

#include "netdev-ctc-vport.h"
#include "ofp_api.h"

VLOG_DEFINE_THIS_MODULE(netdev_ctc_vport);

/* Sockets used for ioctl operations. */
static int af_inet_sock = -1;   /* AF_INET, SOCK_DGRAM. */

struct netdev_dev_vport {
    struct netdev_dev netdev_dev;
    unsigned int change_seq;
    uint8_t etheraddr[ETH_ADDR_LEN];
    struct netdev_stats stats;

    /* Tunnels. */
    struct netdev_tunnel_config tnl_cfg;

    /* Patch Ports. */
    char *peer;
};

static int netdev_vport_create(const struct netdev_class *, const char *,
                               struct netdev_dev **);
static int get_tunnel_config(struct netdev_dev *, struct smap *args);
static void netdev_vport_poll_notify(struct netdev_dev_vport *);

static bool
is_vport_class(const struct netdev_class *class)
{
    return class->create == netdev_vport_create;
}

static struct netdev_dev_vport *
netdev_dev_vport_cast(const struct netdev_dev *netdev_dev)
{
    ovs_assert(is_vport_class(netdev_dev_get_class(netdev_dev)));
    return CONTAINER_OF(netdev_dev, struct netdev_dev_vport, netdev_dev);
}

static struct netdev_dev_vport *
netdev_vport_get_dev(const struct netdev *netdev)
{
    return netdev_dev_vport_cast(netdev_get_dev(netdev));
}

static const struct netdev_tunnel_config *
get_netdev_tunnel_config(const struct netdev_dev *netdev_dev)
{
    return &netdev_dev_vport_cast(netdev_dev)->tnl_cfg;
}

static int
netdev_vport_init(void)
{
    if (af_inet_sock < 0) {
        af_inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    }

    return 0;
}

static int
netdev_vport_create(const struct netdev_class *netdev_class, const char *name,
                    struct netdev_dev **netdev_devp)
{
    struct netdev_dev_vport *dev;

    dev = xzalloc(sizeof *dev);
    netdev_dev_init(&dev->netdev_dev, name, netdev_class);
    dev->change_seq = 1;
    eth_addr_random(dev->etheraddr);

    *netdev_devp = &dev->netdev_dev;

    return 0;
}

static void
netdev_vport_destroy(struct netdev_dev *netdev_dev_)
{
    struct netdev_dev_vport *netdev_dev = netdev_dev_vport_cast(netdev_dev_);

    free(netdev_dev->peer);
    free(netdev_dev);
}

static int
netdev_vport_open(struct netdev_dev *netdev_dev, struct netdev **netdevp)
{
    *netdevp = xmalloc(sizeof **netdevp);
    netdev_init(*netdevp, netdev_dev);
    VLOG_DBG("netdev_vport_open, name=%s, class-type=%s\n", netdev_dev->name, netdev_dev->netdev_class->type);

    return 0;
}

static int
netdev_vport_listen(struct netdev *netdev)
{
    return 0;
}

static void
netdev_vport_close(struct netdev *netdev)
{
    free(netdev);
}

static int
netdev_vport_set_etheraddr(struct netdev *netdev,
                           const uint8_t mac[ETH_ADDR_LEN])
{
    /*
    struct netdev_dev_vport *dev = netdev_vport_get_dev(netdev);
    memcpy(dev->etheraddr, mac, ETH_ADDR_LEN);
    netdev_vport_poll_notify(dev);
    */
    return 0;
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

static int
netdev_vport_get_etheraddr(const struct netdev *netdev,
                           uint8_t mac[ETH_ADDR_LEN])
{
    const struct netdev_tunnel_config *tnl_cfg =
        &netdev_dev_vport_cast(netdev->netdev_dev)->tnl_cfg;
    int error = 0;
    uint8_t etheraddr[ETH_ADDR_LEN];

    /* Use route mac as default gre port's etheraddr */
    ofp_port_get_route_mac(mac);
    /* Get bind_port's etheraddr as tunnel's etheraddr */
    error = get_etheraddr(tnl_cfg->bind_port, etheraddr);
    if (!error) {
        memcpy(mac, etheraddr, ETH_ADDR_LEN);
    }

    return 0;
}

static int
tunnel_get_status(const struct netdev *netdev, struct smap *smap)
{
    const char *iface = netdev_vport_get_dev(netdev)->tnl_cfg.bind_port;

    if (iface) {
        struct netdev *egress_netdev;

        smap_add(smap, "tunnel_egress_iface", iface);

        if (!netdev_open(iface, "system", &egress_netdev)) {
            smap_add(smap, "tunnel_egress_iface_carrier",
                     netdev_get_carrier(egress_netdev) ? "up" : "down");
            netdev_close(egress_netdev);
        }
    }

    return 0;
}

static int
netdev_vport_update_flags(struct netdev *netdev OVS_UNUSED,
                        enum netdev_flags off, enum netdev_flags on OVS_UNUSED,
                        enum netdev_flags *old_flagsp)
{
    if (off & (NETDEV_UP | NETDEV_PROMISC)) {
        return EOPNOTSUPP;
    }

    *old_flagsp = NETDEV_UP | NETDEV_PROMISC;
    return 0;
}

static unsigned int
netdev_vport_change_seq(const struct netdev *netdev)
{
    return netdev_vport_get_dev(netdev)->change_seq;
}

static void
netdev_vport_run(void)
{
    return;
}

static void
netdev_vport_wait(void)
{
    return;
}

/* Helper functions. */
static int
get_mac_addr(const char* mac_str, uint8_t* mac)
{
    int mac_str_index = 0;
    int dot_count = 0;

    if (NULL == mac_str) {
        return EINVAL;
    }

    if (strspn(mac_str, "0123456789abcdefABCDEF.") != strlen(mac_str)) {
        return EINVAL;
    }

    for (mac_str_index = 0; mac_str_index < strlen(mac_str); mac_str_index++) {
        if (mac_str[mac_str_index] == '.') {
            dot_count++;
        }
    }
    if (dot_count != 2 || strlen(mac_str) > 14) {
        return EINVAL;
    }

    if (sscanf(mac_str, "%4hx.%4hx.%4hx",
                      (unsigned short *)&mac[0],
                      (unsigned short *)&mac[2],
                      (unsigned short *)&mac[4]) != 3) {
      return EINVAL;
    }

  /* Convert to network order */
  *(unsigned short *)&mac[0] =
    htons(*(unsigned short *)&mac[0]);
  *(unsigned short *)&mac[2] =
    htons(*(unsigned short *)&mac[2]);
  *(unsigned short *)&mac[4] =
    htons(*(unsigned short *)&mac[4]);

    return 0;
}

static bool
check_valid_physical_port_name(const char* port_name)
{
    return ofp_netdev_name_is_phy_port(port_name);
}

static void
netdev_vport_poll_notify(struct netdev_dev_vport *ndv)
{
    ndv->change_seq++;
    if (!ndv->change_seq) {
        ndv->change_seq++;
    }
}

static int
set_tunnel_config(struct netdev_dev *dev_, const struct smap *args)
{
    struct netdev_dev_vport *dev = netdev_dev_vport_cast(dev_);
    const char *name = netdev_dev_get_name(dev_);
    const char *type = netdev_dev_get_type(dev_);
    struct netdev_tunnel_config tnl_cfg;
    struct smap_node *node;
    uint8_t mac[6] = {0};

    VLOG_DBG("set_tunnel_config, name=%s, type=%s\n", name, dev_->netdev_class->type);

    if (strcmp(type, "gre")) {
        return EINVAL;
    }

    memset(&tnl_cfg, 0x0, sizeof(tnl_cfg));
    tnl_cfg.vlan_id = htons(OFP_DEFAULT_VLAN_ID);
    SMAP_FOR_EACH (node, args) {
        if (!strcmp(node->key, "remote_ip")) {
            struct in_addr in_addr;
            if (1 != inet_pton(AF_INET, node->value, &in_addr) || lookup_ip(node->value, &in_addr)) {
                VLOG_WARN("%s: bad %s 'remote_ip'", name, type);
            } else {
                tnl_cfg.ip_dst = in_addr.s_addr;
            }
        } else if (!strcmp(node->key, "local_ip")) {
            struct in_addr in_addr;
            if (1 != inet_pton(AF_INET, node->value, &in_addr) || lookup_ip(node->value, &in_addr)) {
                VLOG_WARN("%s: bad %s 'local_ip'", name, type);
            } else {
                tnl_cfg.ip_src = in_addr.s_addr;
            }
        } else if (!strcmp(node->key, "nexthop_mac")) {
            if (get_mac_addr(node->value, mac)) {
                VLOG_WARN("%s: bad %s 'nexthop_mac'", name, type);
            } else {
                strncpy(tnl_cfg.nexthop_mac, (const char*)node->value, 15);
            }
        } else if (!strcmp(node->key, "bind_port")) {
            if (!check_valid_physical_port_name(node->value)) {
                VLOG_WARN("%s: bad %s 'bind_port'", name, type);
            } else {
                strncpy(tnl_cfg.bind_port, (const char*)node->value, IFNAMSIZ);
            }
        } else if (!strcmp(node->key, "vlan_id")) {
            uint16_t vlan_id = atoi(node->value);
            if (vlan_id < 1 || vlan_id > MAX_VLAN_NUM) {
                VLOG_WARN("%s: bad %s 'vlan_id', should be in range <1-4094>", name, type);
            } else {
                tnl_cfg.vlan_id = htons(vlan_id);
            }

            /* Handled separately below. */
        } else {
            VLOG_WARN("%s: unknown %s argument '%s'", name, type, node->key);
        }
    }

    if (!tnl_cfg.ip_dst) {
        VLOG_ERR("%s type requires valid 'remote_ip' argument", type);
        return EINVAL;
    }
    if (!tnl_cfg.ip_src) {
         VLOG_ERR("%s type requires valid 'local_ip' argument", type);
        return EINVAL;
    }
    if (!tnl_cfg.nexthop_mac[0]) {
        VLOG_ERR("%s type requires valid 'nexthop_mac' argument", type);
        return EINVAL;
    }
    if (!tnl_cfg.bind_port[0]) {
        VLOG_ERR("%s type requires valid 'bind_port' argument", type);
        return EINVAL;
    }

    dev->tnl_cfg = tnl_cfg;
    netdev_vport_poll_notify(dev);

    return 0;
}

static int
get_tunnel_config(struct netdev_dev *dev, struct smap *args)
{
    const struct netdev_tunnel_config *tnl_cfg =
        &netdev_dev_vport_cast(dev)->tnl_cfg;

    if (tnl_cfg->ip_dst) {
        smap_add_format(args, "remote_ip", IP_FMT, IP_ARGS(tnl_cfg->ip_dst));
    }

    if (tnl_cfg->ip_src) {
        smap_add_format(args, "local_ip", IP_FMT, IP_ARGS(tnl_cfg->ip_src));
    }

    smap_add_format(args, "bind_port", "%s", tnl_cfg->bind_port);
    smap_add_format(args, "nexthop_mac", "%s", tnl_cfg->nexthop_mac);

    if (tnl_cfg->vlan_id == htons(OFP_DEFAULT_VLAN_ID)) {
        smap_add_format(args, "vlan_id", "%u", ntohs(tnl_cfg->vlan_id));
    }

    return 0;
}

int
netdev_vport_get_tunnel_info(struct netdev_dev *dev, ofp_tunnel_info_t *tunnel_info)
{
    const struct netdev_tunnel_config *tnl_cfg =
        &netdev_dev_vport_cast(dev)->tnl_cfg;

    if (tnl_cfg->ip_dst) {
        tunnel_info->remote_ip = ntohl(tnl_cfg->ip_dst);
    }
    if (tnl_cfg->ip_src) {
        tunnel_info->local_ip = ntohl(tnl_cfg->ip_src);
    }
    if (get_mac_addr(tnl_cfg->nexthop_mac, tunnel_info->nexthop_mac)) {
        VLOG_WARN("%s: bad %s 'nexthop_mac'", dev->name, dev->netdev_class->type);
        return EINVAL;
    }
    strcpy(tunnel_info->bind_port_name, tnl_cfg->bind_port);
    tunnel_info->vlan_id = ntohs(tnl_cfg->vlan_id);

    return 0;
}

static int
get_stats(const struct netdev *netdev, struct netdev_stats *stats)
{
    stats->rx_packets          = UINT64_MAX; /* TODO: Not Support */
    stats->tx_packets          = UINT64_MAX; /* TODO: Not Support */
    stats->rx_bytes            = UINT64_MAX; /* TODO: Not Support */
    stats->tx_bytes            = UINT64_MAX; /* TODO: Not Support */
    stats->rx_errors           = UINT64_MAX; /* TODO: Not Support */
    stats->tx_errors           = UINT64_MAX; /* TODO: Not Support */
    stats->rx_dropped          = UINT64_MAX; /* TODO: Not Support */
    stats->tx_dropped          = UINT64_MAX; /* TODO: Not Support */
    stats->multicast           = UINT64_MAX; /* TODO: Not Support */
    stats->collisions          = UINT64_MAX; /* TODO: Not Support */

    stats->rx_length_errors    = UINT64_MAX; /* TODO: Not Support */
    stats->rx_over_errors      = UINT64_MAX; /* TODO: Not Support */
    stats->rx_crc_errors       = UINT64_MAX; /* TODO: Not Support */
    stats->rx_frame_errors     = UINT64_MAX; /* TODO: Not Support */
    stats->rx_fifo_errors      = UINT64_MAX; /* TODO: Not Support */
    stats->rx_missed_errors    = UINT64_MAX; /* TODO: Not Support */

    stats->tx_aborted_errors   = UINT64_MAX; /* TODO: Not Support */
    stats->tx_carrier_errors   = UINT64_MAX; /* TODO: Not Support */
    stats->tx_fifo_errors      = UINT64_MAX; /* TODO: Not Support */
    stats->tx_heartbeat_errors = UINT64_MAX; /* TODO: Not Support */
    stats->tx_window_errors    = UINT64_MAX; /* TODO: Not Support */

    return 0;
}

static int
netdev_vport_get_carrier(const struct netdev *netdev_, bool *carrier)
{
    int ret = 0;
    uint8_t ofp_carrier = false;
    const struct netdev_tunnel_config *tnl_cfg =
            &netdev_dev_vport_cast(netdev_->netdev_dev)->tnl_cfg;

    ret = ofp_netdev_get_port_carrier(tnl_cfg->bind_port, &ofp_carrier);

    if (ret < 0)
    {
        return ENODEV;
    }
    *carrier = ofp_carrier;

    return 0;
}

/* Stores the features supported by 'netdev' into each of '*current',
 * '*advertised', '*supported', and '*peer' that are non-null. Returns 0 if
 * successful, otherwise a positive errno value. */
static int
netdev_vport_get_features(const struct netdev *netdev,
                          uint32_t *current, uint32_t *advertised,
                          uint32_t *supported, uint32_t *peer)
{
    int32_t ret = 0;
    uint32_t ofp_current = 0;
    uint32_t ofp_advertised = 0;
    uint32_t ofp_supported = 0;
    uint32_t ofp_peer = 0;
    const struct netdev_tunnel_config *tnl_cfg =
            &netdev_dev_vport_cast(netdev->netdev_dev)->tnl_cfg;

    ret = ofp_netdev_get_port_features(tnl_cfg->bind_port, &ofp_current, &ofp_advertised, &ofp_supported, &ofp_peer);
    if (ret < 0)
    {
        return ENODEV;
    }
    *current = ofp_current;
    *advertised = ofp_advertised;
    *supported = ofp_supported;
    *peer = ofp_peer;

    return 0;
}

/* Set the features advertised by 'netdev' to 'advertise'. */
static int
netdev_vport_set_advertisements(struct netdev *netdev, uint32_t advertise)
{
    char * ifname  = NULL;

    ifname = (char *)netdev_get_name(netdev);

    return ofp_netdev_set_port_advertised(ifname, advertise);
}

#define VPORT_FUNCTIONS(GET_CONFIG, SET_CONFIG,             \
                        GET_TUNNEL_CONFIG, GET_STATUS)      \
    netdev_vport_init,                                      \
    netdev_vport_run,                                       \
    netdev_vport_wait,                                      \
                                                            \
    netdev_vport_create,                                    \
    netdev_vport_destroy,                                   \
    GET_CONFIG,                                             \
    SET_CONFIG,                                             \
    GET_TUNNEL_CONFIG,                                      \
                                                            \
    netdev_vport_open,                                      \
    netdev_vport_close,                                     \
                                                            \
    netdev_vport_listen,        /* listen */                \
    NULL,                       /* recv */                  \
    NULL,                       /* recv_wait */             \
    NULL,                       /* drain */                 \
                                                            \
    NULL,                       /* send */                  \
    NULL,                       /* send_wait */             \
                                                            \
    netdev_vport_set_etheraddr,                             \
    netdev_vport_get_etheraddr,                             \
    NULL,                       /* get_mtu */               \
    NULL,                       /* set_mtu */               \
    NULL,                       /* get_ifindex */           \
    netdev_vport_get_carrier,   /* get_carrier */           \
    NULL,                       /* get_carrier_resets */    \
    NULL,                       /* get_miimon */            \
    get_stats,                                              \
    NULL,                       /* set_stats */             \
                                                            \
    netdev_vport_get_features,  /* get_features */          \
    netdev_vport_set_advertisements,/* set_advertisements */\
                                                            \
    NULL,                       /* set_policing */          \
    NULL,                       /* get_qos_types */         \
    NULL,                       /* get_qos_capabilities */  \
    NULL,                       /* get_qos */               \
    NULL,                       /* set_qos */               \
    NULL,                       /* get_queue */             \
    NULL,                       /* set_queue */             \
    NULL,                       /* delete_queue */          \
    NULL,                       /* get_queue_stats */       \
    NULL,                       /* dump_queues */           \
    NULL,                       /* dump_queue_stats */      \
                                                            \
    NULL,                       /* get_in4 */               \
    NULL,                       /* set_in4 */               \
    NULL,                       /* get_in6 */               \
    NULL,                       /* add_router */            \
    NULL,                       /* get_next_hop */          \
    GET_STATUS,                                             \
    NULL,                       /* arp_lookup */            \
                                                            \
    netdev_vport_update_flags,                              \
                                                            \
    netdev_vport_change_seq

#define TUNNEL_CLASS(NAME, DPIF_PORT)                       \
    { DPIF_PORT,                                            \
        { NAME, VPORT_FUNCTIONS(get_tunnel_config,          \
                                set_tunnel_config,          \
                                get_netdev_tunnel_config,   \
                                tunnel_get_status) }}

const struct vport_class vport_ctc_class =
    TUNNEL_CLASS("gre", "gre_system");

extern struct vport_class netdev_live_vport_class;
void
register_netdev_live_vport_class(void)
{
    memcpy(&netdev_live_vport_class, &vport_ctc_class,
           sizeof(struct vport_class));
}
