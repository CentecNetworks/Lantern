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
 * @brief This file is the header file for ofproto-ctc.c
 */

#ifndef OFPROTO_CTC_H
#define OFPROTO_CTC_H 1

#include "config.h"

#include <limits.h>
#include "timer.h"

#include "lib/ofp-util.h"
#include "ofproto/ofproto-provider.h"
#include "netdev-provider.h"

#include "ofp_const.h"
#include "ofp_types.h"
#include "ofp_flow.h"
#include "ofp_port.h"
#include "ofp_version.h"
#include "ofpbuf.h"

struct timer;

/* Number of implemented OpenFlow tables. */
enum { N_TABLES = 1 };

/* Maximum number of ports. */
enum { MAX_PORTS = OFP_TUNNEL_PORT_NO_MAX }; /* assign max tunnel port number to max port number */

enum { MAX_QUEUE_LEN = 128 };   /* Maximum number of packets per queue. */

#define FLOW_MISS_MAX_BATCH 50
#define FLOW_MISS_BUF_SIZE 4096

#ifdef _OFP_UML_
#define NETDEV_CPU_PORT "eth104"
#else
#define NETDEV_CPU_PORT "eth1"
#endif

/* Hardware descriptions. */
#define CTC_MFR_DESC OPENFLOW_HW_MFR_DESC
#define CTC_HW_DESC OPENFLOW_HW_DESC
#define CTC_SW_DESC OPENFLOW_CTC_VERSION
#define CTC_SERIAL_DESC OPENFLOW_CTC_SERIAL_DESC
#define CTC_DP_DESC OPENFLOW_CTC_DP_DESC

struct ofport_ctc {
    struct ofport up;
};

enum ctc_upcall_type {
    CTC_UC_MISS,               /* Miss in flow table. */
    CTC_UC_ACTION,             /* OVS_ACTION_ATTR_USERSPACE action. */
    CTC_N_UC_TYPES
};

struct ctc_upcall {
    /* All types. */
    enum ctc_upcall_type type;
    struct ofpbuf *packet;      /* Packet data. */
    struct nlattr *key;         /* Flow key. */
    size_t key_len;             /* Length of 'key' in bytes. */

    /* DPIF_UC_ACTION only. */
    uint64_t userdata;          /* Argument to OVS_ACTION_ATTR_USERSPACE. */
};

struct ctc_netdev_upcall {
    struct ctc_upcall upcall;  /* Queued upcall information. */
    struct ofpbuf buf;          /* ofpbuf instance for upcall.packet. */
};

struct ctc_netdev_queue {
    struct ctc_netdev_upcall upcalls[MAX_QUEUE_LEN];
    unsigned int head, tail;
};

struct ofproto_ctc {
    struct ofproto up;

    /* Ports. */
    struct netdev *ports[MAX_PORTS];

    /* Expiration. */
    struct timer next_expiration;
    int fast_expiration;
    struct ctc_netdev_queue queues;
};

struct rule_ctc {
    struct rule up;

    struct match match; /* XXX: We save the expanded match in the rule, because
                         * we can accept the storage penalty for performance. 
                         */
    struct list flow_actions; /* XXX: these actions are translated from 
                               * ofpacts in the ovs rule before passed to
                               * adapter layer, and it's not persistent, please
                               * do not access it later. */

    /* Stats. */
    uint64_t packet_count;       /* Number of packets received. */
    uint64_t byte_count;         /* Number of bytes received. */

    uint32_t flow_id;
    uint32_t entry_id;
    uint32_t extra_entry_id;
    uint32_t stats_ptr;
    uint32_t mpls_label;
    bool is_mpls;
    bool is_mpls_pop;
    ofp_action_type_t action_type;
    ofp_nexthop_info_t nh_info;
    
    ofp_group_info_t group_info;
    ofp_meter_info_t meter_info;

    uint32_t queue_id;
};

struct group_ctc {
    struct ofgroup up;
};

struct group_bucket_stats {
    uint64_t packet_count; /* Number of packets in bucket. */
    uint64_t byte_count;   /* Number of bytes in bucket. */
};

struct group_stats {
    uint64_t packet_count;   /* Number of packets in group. */
    uint64_t byte_count;     /* Number of bytes in group. */

    struct group_bucket_stats buckets_stats[OFP_BUCKET_NUM_PER_GROUP];
    size_t n_buckets;
};

struct meter_ctc {
    struct ofmeter up;
};

struct meter_band_stats_ctc {
    uint64_t packet_band_count; /* Number of packets in band. */
    uint64_t byte_band_count;   /* Number of bytes in band. */
};

struct meter_stats_ctc {
    uint64_t packet_in_count;   /* Number of packets in input. */
    uint64_t byte_in_count;     /* Number of bytes in input. */

    struct meter_band_stats_ctc bands_stats[OFP_MAX_BAND_PER_METER];
    size_t n_bands;
};

struct flow_miss_op {
    void *garbage;              /* Pointer to pass to free(), NULL if none. */
    uint64_t stub[1024 / 8];    /* Temporary buffer. */
};

struct flow_miss {
    struct hmap_node hmap_node;
    struct ofproto_ctc *ofproto;
    struct flow flow;
    const struct nlattr *key;
    size_t key_len;
    ovs_be16 initial_tci;
    struct list packets;
    enum ctc_upcall_type upcall_type;
    uint32_t odp_in_port;
};

struct action_xlate_ctx {
    /* action_xlate_ctx_init() initializes these members. */

    /* The ofproto. */
    struct ofproto_ctc *ofproto;

    /* Flow to which the OpenFlow actions apply.  xlate_actions() will modify
     * this flow when actions change header fields. */
    struct flow flow;

    /* The packet corresponding to 'flow', or a null pointer if we are
     * revalidating without a packet to refer to. */
    const struct ofpbuf *packet;

    /* The rule that we are currently translating, or NULL. */
    struct rule_ctc *rule;

    /* flow process type, used to determine which actions need software handling */
    enum ofp_flow_process_type_e flow_process_type;

    /* indicate if the flow is a flow with in_port masked */
    bool any_port_flow;

    /* Union of the set of TCP flags seen so far in this flow.  (Used only by
     * NXAST_FIN_TIMEOUT.  Set to zero to avoid updating updating rules'
     * timeouts.) */
    uint8_t tcp_flags;

    /* xlate_actions() initializes and uses these members.  The client might want
     * to look at them after it returns. */

    struct ofpbuf *odp_actions; /* Datapath actions. */
    tag_type tags;              /* Tags associated with actions. */
    uint16_t nf_output_iface;   /* Output interface index for NetFlow. */

    /* xlate_actions() initializes and uses these members, but the client has no
     * reason to look at them. */

    struct flow base_flow;      /* Flow at the last commit. */
    uint32_t orig_skb_priority; /* Priority when packet arrived. */
    uint8_t table_id;           /* OpenFlow table ID where flow was found. */
    uint32_t sflow_n_outputs;   /* Number of output ports. */
    uint32_t sflow_odp_port;    /* Output port for composing sFlow action. */
    uint16_t user_cookie_offset;/* Used for user_action_cookie fixup. */
    bool exit;                  /* No further actions should be processed. */
    struct flow orig_flow;      /* Copy of original flow. */
};

struct rule_ctc *rule_ctc_cast(const struct rule *rule);
struct ofproto_ctc *ofproto_ctc_cast(const struct ofproto *ofproto);
void register_ofproto_live_class(void);

#endif
