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
 * @brief This file contains the implementations of ofproto
 */

#include <config.h>
#include <errno.h>

#include "byte-order.h"
#include "meta-flow.h"
#include "netdev.h"
#include "odp-util.h"
#include "ofp-util.h"
#include "ofpbuf.h"
#include "ofp-actions.h"
#include "ofp-parse.h"
#include "ofp-print.h"
#include "poll-loop.h"
#include "vlog.h"
#include "odp-util.h"
#include "flow.h"
#include "connmgr.h"
#include "socket-util.h"
#include "nx-match.h"
#include "multipath.h"
#include "linux/openvswitch.h"

#include "ofproto-ctc.h"
#include "netdev-ctc-vport.h"
#include "ofp_api.h"

VLOG_DEFINE_THIS_MODULE(ofproto_ctc);

enum { CTC_NETDEV_HEADROOM = 2 + VLAN_HEADER_LEN };

extern int do_get_ifindex(const char *netdev_name);

static struct ofproto *alloc(void);

static void
action_xlate_ctx_init(struct action_xlate_ctx *ctx,
                      struct ofproto_ctc *ofproto, const struct flow *flow,
                      ovs_be16 initial_tci, struct rule_ctc *rule_,
                      uint8_t tcp_flags, const struct ofpbuf *packet);
static void
do_xlate_actions(const struct ofpact *ofpacts, size_t ofpacts_len,
                 struct action_xlate_ctx *ctx);

static int
execute(struct ofproto_ctc *ofproto,
             const struct nlattr *key, size_t key_len,
             const struct nlattr *actions, size_t actions_len,
             const struct ofpbuf *buf);

static void
execute_controller_action(struct action_xlate_ctx *ctx, int len,
                          enum ofp_packet_in_reason reason,
                          uint16_t controller_id);

static void
xlate_actions(struct action_xlate_ctx *ctx,
              const struct ofpact *ofpacts, size_t ofpacts_len,
              struct ofpbuf *odp_actions);

static bool
is_ofproto_ctc_class(const struct ofproto_class *class)
{
    return class->alloc == alloc;
}

struct rule_ctc *rule_ctc_cast(const struct rule *rule)
{
    return rule ? CONTAINER_OF(rule, struct rule_ctc, up) : NULL;
}

static struct ofport_ctc *
ofport_ctc_cast(const struct ofport *ofport)
{
    ovs_assert(is_ofproto_ctc_class(ofport->ofproto->ofproto_class));
    return ofport ? CONTAINER_OF(ofport, struct ofport_ctc, up) : NULL;
}

struct ofproto_ctc *
ofproto_ctc_cast(const struct ofproto *ofproto)
{
    ovs_assert(is_ofproto_ctc_class(ofproto->ofproto_class));
    return CONTAINER_OF(ofproto, struct ofproto_ctc, up);
}

/* Global variables. */

/* The unique ofproto_ctc instance. */
static struct ofproto_ctc *ofproto = NULL;

int g_cpuport_fd = 0;
uint8_t *g_miss_buf[FLOW_MISS_MAX_BATCH];

adapt_to_ofp_error_code_map_t translate_error_code[]={
    /*{adapt_error_code,   openflow_error_code,  }*/
    {OFP_ERR_SUCCESS,                       0},
    {OFP_ERR_FAIL,                          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_ALL_TABLES_FULL,               OFPERR_OFPFMFC_TABLE_FULL},
    {OFP_ERR_ETHER_TYPE_FULL,               OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_INVALID_PTR,                   OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_UNSUPPORTED_ACTIONS,           OFPERR_OFPBAC_BAD_TYPE},

    {OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE,  OFPERR_OFPBMC_BAD_VALUE},
    {OFP_ERR_INVALID_PARAM,             OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_ENTRY_EXIST,               OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_ENTRY_NOT_EXIST,           OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_NO_MEMORY,                 OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_TUNNEL_ID_FULL,            OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_NETDEV_EXIST,              OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_NETDEV_NOT_EXIST,          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_NOT_INIT,                  OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_TUNNEL_PORT_FULL,          OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_INVALID_IN_PORT_NUMBER,        OFPERR_OFPBMC_BAD_VALUE},
    {OFP_ERR_TUNNEL_INVALID_BIND_NAME,      OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_TUNNEL_REPEAT_LOCAL_REMOTE_IP, OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_TUNNEL_LOCAL_IP_FULL,          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_BAD_OUT_PORT,                  OFPERR_OFPBAC_BAD_OUT_PORT},

    {OFP_ERR_ACTION_TOO_MANY_STRIP_VLAN,    OFPERR_OFPBAC_TOO_MANY},
    {OFP_ERR_INVALID_ACTION_LIST,           OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_TOO_MANY_PUSH_VLAN,     OFPERR_OFPBAC_TOO_MANY},
    {OFP_ERR_ACTION_VLAN_PUSH_ONLY_TPID,    OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_VLAN_PUSH_AFT_STRIP,    OFPERR_OFPBAC_UNSUPPORTED_ORDER},

    {OFP_ERR_ACTION_VLAN_PUSH_AFT_MOD,      OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_VLAN_STRIP_AFT_PUSH,    OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_VLAN_STRIP_AFT_MOD,     OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP_TWO, OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTION_VLAN_PUSH_WO_VID,       OFPERR_OFPBAC_UNSUPPORTED_ORDER},

    {OFP_ERR_ACTION_VLAN_PUSH_W_MAC,        OFPERR_OFPBAC_TOO_MANY},
    {OFP_ERR_ACTION_METER_NOT_FIRST,        OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_METER_ID_INVALID,              OFPERR_OFPMMFC_INVALID_METER},
    {OFP_ERR_METER_NOT_EXIST,               OFPERR_OFPMMFC_UNKNOWN_METER},
    {OFP_ERR_METER_EXIST,                   OFPERR_OFPMMFC_METER_EXISTS},

    {OFP_ERR_METER_NOT_SUPPORT_PPS,         OFPERR_OFPMMFC_BAD_FLAGS},
    {OFP_ERR_METER_INVALID_FLAGS,           OFPERR_OFPMMFC_BAD_FLAGS},
    {OFP_ERR_METER_TOO_MANY_BANDS,          OFPERR_OFPMMFC_OUT_OF_BANDS},
    {OFP_ERR_METER_INVALID_RATE,            OFPERR_OFPMMFC_BAD_RATE},
    {OFP_ERR_METER_INVALID_BURST,           OFPERR_OFPMMFC_BAD_BURST},

    {OFP_ERR_METER_INVALID_BAND_TYPE,       OFPERR_OFPMMFC_BAD_BAND},
    {OFP_ERR_METER_OUT_OF_BAND_PROFILE,     OFPERR_OFPMMFC_UNKNOWN},
    {OFP_ERR_METER_TOO_MANY_STATS,          OFPERR_OFPMMFC_UNKNOWN},
    {OFP_ERR_METER_TOO_MANY_METER,          OFPERR_OFPMMFC_UNKNOWN},
    {OFP_ERR_METER_UPDATE_STATS,            OFPERR_OFPMMFC_UNKNOWN},

    {OFP_ERR_SET_TPID_FLOW_NOT_EMPTY,       OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP,     OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_TOO_MANY_OUTPUT_SINGLE_FLOW,   OFPERR_OFPBAC_TOO_MANY},
    {OFP_ERR_LCM_MESSAGE_FUNC_NOT_INIT,     OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_PORT_NOT_EXIST,                OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_TOO_MANY_OUTPUT,               OFPERR_OFPBAC_TOO_MANY},
    {OFP_ERR_INSTRUCTION_METER_NOT_EXIST,   OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_HYBRID_DISABLE,                OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_MODE,                   OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_FDB_INIT_FAIL,          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_INVALID_HASH_KEY,       OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_RESOURCE_FULL,          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_INVALID_PARAM,          OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_ENTRY_NOT_EXIT,         OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_ENTRY_SAME,             OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_HYBRID_PORT_VLAN_MISMATCH,     OFPERR_OFPFMFC_UNKNOWN},

    {OFP_ERR_OF13_BAD_SET_FIELD_TYPE,       OFPERR_OFPBAC_SET_TYPE},
    {OFP_ERR_OF13_BAD_SET_FIELD_ARGUMENT,   OFPERR_OFPBAC_ARGUMENT},
    {OFP_ERR_BAD_ACTION_ARGUMENT,           OFPERR_OFPBAC_BAD_ARGUMENT},
    {OFP_ERR_UNSUPPORTED_ACTIONS_ORDER,     OFPERR_OFPBAC_UNSUPPORTED_ORDER},
    {OFP_ERR_ACTIONS_TOO_MANY,              OFPERR_OFPBAC_TOO_MANY},

    {OFP_ERR_ACTION_AND_MATCH_INCONSISTENT, OFPERR_OFPBAC_MATCH_INCONSISTENT},
    {OFP_ERR_BAD_MATCH_VALUE,               OFPERR_OFPBMC_BAD_VALUE},
    {OFP_ERR_BAD_ACTION_IN_PORT,            OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_GROUP_EXISTS,                  OFPERR_OFPGMFC_GROUP_EXISTS},
    {OFP_ERR_GROUP_UNKNOWN,                 OFPERR_OFPGMFC_UNKNOWN_GROUP},

    {OFP_ERR_GROUP_INVALID,                 OFPERR_OFPGMFC_INVALID_GROUP},
    {OFP_ERR_GROUP_TOO_MANY_BUCKETS,        OFPERR_OFPGMFC_OUT_OF_BUCKETS},
    {OFP_ERR_GROUP_TOO_MANY_GROUPS,         OFPERR_OFPGMFC_OUT_OF_GROUPS},
    {OFP_ERR_GROUP_BAD_TYPE,                OFPERR_OFPGMFC_BAD_TYPE},
    {OFP_ERR_GROUP_BAD_BUCKETS,             OFPERR_OFPGMFC_BAD_BUCKET},

    {OFP_ERR_GROUP_BAD_WATCH,               OFPERR_OFPGMFC_BAD_WATCH},
    {OFP_ERR_WATCH_PORT_UNSUPPORTED,        OFPERR_OFPGMFC_WATCH_UNSUPPORTED},
    {OFP_ERR_WATCH_GROUP_UNSUPPORTED,       OFPERR_OFPGMFC_WATCH_UNSUPPORTED},
    {OFP_ERR_CHAINING_UNSUPPORTED,          OFPERR_OFPGMFC_CHAINING_UNSUPPORTED},
    {OFP_ERR_GROUP_CHAINED_GROUP,           OFPERR_OFPGMFC_CHAINED_GROUP},


    {OFP_ERR_WEIGHT_UNSUPPORTED,            OFPERR_OFPGMFC_WEIGHT_UNSUPPORTED},
    {OFP_ERR_TOO_MANY_GROUP_SINGLE_FLOW,    OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_TOO_MANY_METER_SINGLE_FLOW,    OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_NOT_GROUP_ONLY,                OFPERR_OFPFMFC_UNKNOWN},
    {OFP_ERR_ACTION_BAD_OUT_GROUP,          OFPERR_OFPBAC_BAD_OUT_GROUP},

    {OFP_ERR_ACTION_BAD_TYPE,               OFPERR_OFPBAC_BAD_TYPE},
    {OFP_ERR_UNSUPPORTED_FLOW_FIELD,        OFPERR_OFPBMC_BAD_FIELD},
    {OFP_ERR_INVALID_SET_TUNNEL_ID,         OFPERR_OFPBAC_ARGUMENT},

    {OFP_ERR_MAX,                           OFPERR_OFPFMFC_UNKNOWN},

};
OFP_ASSERT(sizeof(translate_error_code) / sizeof(adapt_to_ofp_error_code_map_t) == (1 + (-OFP_ERR_MAX)));

/* Translate ofp_error_e error to ofperr. */
static enum ofperr
translate_adpt_error_code(int error, ofp_error_type_t type)
{
    int index;

    if (error < OFP_ERR_MAX || error > OFP_ERR_SUCCESS) {
        VLOG_ERR("Fatal error, unknown adapt layer error code: %d", error);

        switch(type) {
            case OFP_TYPE_FLOW:
                return OFPERR_OFPFMFC_UNKNOWN;
            case OFP_TYPE_GROUP:
                return OFPERR_OFPGMFC_BAD_BUCKET;
            case OFP_TYPE_METER:
                return OFPERR_OFPMMFC_UNKNOWN;
        }
    }

    index = -error;

    return translate_error_code[index].ofp_error_code;

}

/* Factory functions. */

static int
recv_packet(struct ofpbuf *packet)
{
    ssize_t retval = 0;
    size_t size;

    if (g_cpuport_fd <= 0) {
        /* Device is not listening. */
        return -1;
    }

    size = ofpbuf_tailroom(packet);
    for (;;) {
        retval = recv(g_cpuport_fd, packet->data, size, MSG_TRUNC);
        if (retval >= 0) {
            packet->size += retval;
            if (packet->size < ETH_TOTAL_MIN) {
                ofpbuf_put_zeros(packet, ETH_TOTAL_MIN - packet->size);
            }
            return retval <= size ? retval : -EMSGSIZE;
        } else if (errno != EINTR) {
            if (errno != EAGAIN) {
                VLOG_WARN("error receiving Ethernet packet on %s",
                             strerror(errno));
            }

            return -errno;
        }
    }
    return 0;
}

static int
ofproto_netdev_port_output(struct ctc_upcall *upcall,
                 struct ofpbuf *buf)
{
    struct ctc_netdev_queue *q = &ofproto->queues;
    struct ctc_netdev_upcall *u;

    if (q->head == q->tail) {
        return EAGAIN;
    }

    u = &q->upcalls[q->tail++ & (MAX_QUEUE_LEN - 1)];
    *upcall = u->upcall;
    upcall->packet = buf;

    ofpbuf_uninit(buf);
    *buf = u->buf;

    return 0;
}

static void
ofproto_netdev_port_input(struct ofproto *ofproto_, struct ofpbuf *packet)
{
    struct flow key;
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);
    struct ctc_netdev_queue *q = &ofproto->queues;
    struct ctc_upcall *upcall;
    ofp_packet_to_cpu_info_t packet_to_cpu_info;
    struct ofpbuf *buf;
    size_t key_len;
    struct ctc_netdev_upcall *u;
    struct odputil_keybuf keybuf;
    struct ofpbuf buf_key;
    struct action_xlate_ctx ctx;
    struct ofpbuf odp_actions;
    uint64_t odp_actions_stub[1024 / 8];

    memset(&packet_to_cpu_info, 0, sizeof(ofp_packet_to_cpu_info_t));

    if (OFP_ERR_SUCCESS != ofp_netdev_decap_upcall(packet, &packet_to_cpu_info)) {
        return;
    }

    if (packet->size < ETH_HEADER_LEN) {
        return;
    }

    flow_extract(packet, 0, 0, NULL, packet_to_cpu_info.in_port, &key);

    /*we do not support MPLS slow path matching and forwarding, ignore this packet, the packet buffer
      is shared, no need free it here*/
    if (key.dl_type == htons(ETH_TYPE_MPLS) || key.dl_type == htons(ETH_TYPE_MPLS_MCAST)) {
        return;
    }

    switch (packet_to_cpu_info.packet_in_reason)
    {
        /* TODO software process of flow with group is not completed */
        case PACKET_TO_CPU_REASON_SW_PROCESS_GROUP:
            break;

        case PACKET_TO_CPU_REASON_SW_PROCESS:
            if (packet_to_cpu_info.p_rule) {
                ofpbuf_use_stack(&buf_key, &keybuf, sizeof keybuf);
                odp_flow_key_from_flow(&buf_key, &key, key.in_port);
                action_xlate_ctx_init(&ctx, ofproto, &key, key.vlan_tci, packet_to_cpu_info.p_rule,
                        packet_get_tcp_flags(packet, &key), packet);
                ctx.flow_process_type = OFP_FLOW_PROCESS_TYPE_MATCH_TABLE_AND_FORWARD;
                ctx.any_port_flow = OFP_FLOW_INPORT_BASED(packet_to_cpu_info.p_rule) == FLOW_TYPE_PORT_BASED_PER_PORT ? false : true;
                ofpbuf_use_stub(&odp_actions, odp_actions_stub, sizeof odp_actions_stub);
                xlate_actions(&ctx, packet_to_cpu_info.p_rule->up.ofpacts, packet_to_cpu_info.p_rule->up.ofpacts_len, &odp_actions);
                execute(ofproto, buf_key.data, buf_key.size, odp_actions.data, odp_actions.size, packet);
                ofpbuf_uninit(&odp_actions);
            }
            break;

        case PACKET_TO_CPU_REASON_MISS_MATCH:
            /* Miss match packet */
            if (q->head - q->tail >= MAX_QUEUE_LEN) {
                return;
            }

            u = &q->upcalls[q->head++ & (MAX_QUEUE_LEN - 1)];

            buf = &u->buf;
            ofpbuf_init(buf, ODPUTIL_FLOW_KEY_BYTES + 2 + packet->size);
            odp_flow_key_from_flow(buf, &key, packet_to_cpu_info.in_port);
            key_len = buf->size;
            ofpbuf_pull(buf, key_len);
            ofpbuf_reserve(buf, 2);
            ofpbuf_put(buf, packet->data, packet->size);

            upcall = &u->upcall;
            upcall->type = CTC_UC_MISS;
            upcall->packet = buf;
            upcall->key = buf->base;
            upcall->key_len = key_len;
            upcall->userdata = 0;
            ofproto->fast_expiration = TRUE;
            break;

        case PACKET_TO_CPU_REASON_MAX:
        default:
            break;
    }
}

static void
ofproto_netdev_port_recv(struct ofproto *ofproto)
{
    struct ofpbuf packet;
    ssize_t retval = 0;

    ofpbuf_init(&packet, CTC_NETDEV_HEADROOM + VLAN_ETH_HEADER_LEN + 9600);
    ofpbuf_clear(&packet);
    ofpbuf_reserve(&packet, CTC_NETDEV_HEADROOM);
    retval = recv_packet(&packet);
    if (retval > 0) {
        ofproto_netdev_port_input(ofproto, &packet);
    }
    ofpbuf_uninit(&packet);
}

static int
ofproto_netdev_port_init(void)
{
    struct sockaddr_ll sll;
    int ifindex;
    int error;
    int fd;

    if (g_cpuport_fd >0) {
        return 0;
    }

    /* Create file descriptor. */
    fd = socket(PF_PACKET, SOCK_RAW, 0);
    if (fd < 0) {
        error = errno;
        VLOG_ERR("failed to create raw socket (%s)", strerror(error));
        goto error;
    }

    /* Set non-blocking mode. */
    error = set_nonblocking(fd);
    if (error) {
        goto error;
    }

    /* Get ethernet device index. */
    ifindex = do_get_ifindex(NETDEV_CPU_PORT);
    if (ifindex <= 0) {
        goto error;
    }

    /* Bind to specific ethernet device. */
    memset(&sll, 0, sizeof sll);
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifindex;
    sll.sll_protocol = (OVS_FORCE unsigned short int) htons(ETH_P_ALL);
    if (bind(fd, (struct sockaddr *) &sll, sizeof sll) < 0) {
        error = errno;
        VLOG_ERR("failed to bind raw socket (%s)", strerror(error));
        goto error;
    }

    g_cpuport_fd = fd;
    return 0;

error:
    if (fd >= 0) {
        close(fd);
    }
    return error;
}

static void
init(const struct shash *iface_hints)
{
    int i;

    iface_hints = iface_hints; /* TODO unused parameter */
    for (i = 0; i < FLOW_MISS_MAX_BATCH; i++) {
        g_miss_buf[i] = xzalloc(FLOW_MISS_BUF_SIZE);
    }

    ofproto_netdev_port_init();

    /* XXX: iface_hints processing is needed ? */
}

static void
enumerate_types(struct sset *types)
{
    sset_clear(types);

    sset_add(types, OFP_DEFAULT_NETDEV_TYPE);
}

static int
enumerate_names(const char *type, struct sset *names)
{
    type = type; /* TODO unused parameter */
    names = names; /* TODO unused parameter */

    sset_clear(names);

    /* Only one datapath can be added. */
    sset_add(names, "v330");

    return 0;
}

static int
del(const char *type, const char *name)
{
    type = type; /* TODO unused parameter */
    name = name; /* TODO unused parameter */

    /* Hardware datapath can't be deleted. */
    return -1;
}

/* Sends an OFPT_PACKET_IN message for 'packet' of type OFPR_NO_MATCH to each
 * OpenFlow controller as necessary according to their individual
 * configurations. */
static void
send_packet_in_miss(struct ofproto_ctc *ofproto, const struct ofpbuf *packet,
                    const struct flow *flow)
{
    struct ofputil_packet_in pin;

    pin.packet = packet->data;
    pin.packet_len = packet->size;
    pin.reason = OFPR_NO_MATCH;
    pin.controller_id = 0;

    pin.table_id = 0;
    pin.cookie = 0;

    pin.send_len = 0;           /* not used for flow table misses */

    flow_get_metadata(flow, &pin.fmd);

    connmgr_send_packet_in(ofproto->up.connmgr, &pin);
}

static void
handle_flow_miss(struct ofproto_ctc *ofproto, struct flow_miss *miss,
                 struct flow_miss_op *ops, size_t *n_ops)
{
    const struct flow *flow = &miss->flow;
    struct ofpbuf *packet;

    ops = ops; /* TODO unused parameter */
    n_ops = n_ops; /* TODO unused parameter */
    LIST_FOR_EACH (packet, list_node, &miss->packets) {
        send_packet_in_miss(ofproto, packet, flow);
    }

    return;
}

static struct flow_miss *
flow_miss_find(struct hmap *todo, const struct flow *flow, uint32_t hash)
{
    struct flow_miss *miss;

    HMAP_FOR_EACH_WITH_HASH (miss, hmap_node, hash, todo) {
        if (flow_equal(&miss->flow, flow)) {
            return miss;
        }
    }

    return NULL;
}

static void
handle_miss_upcalls(struct ctc_upcall *upcalls,
                    int n_upcalls)
{
    struct ctc_upcall *upcall;
    struct flow_miss *miss;
    struct hmap todo;
    size_t n_ops;
    struct flow_miss misses[FLOW_MISS_MAX_BATCH];
    int n_misses;

    if (n_upcalls == 0) {
        return;
    }

    /* Construct the to-do list.
     *
     * This just amounts to extracting the flow from each packet and sticking
     * the packets that have the same flow in the same "flow_miss" structure so
     * that we can process them together. */
    hmap_init(&todo);
    n_misses = 0;
    for (upcall = upcalls; upcall < &upcalls[n_upcalls]; upcall++) {
        struct flow_miss *miss = &misses[n_misses];
        struct flow_miss *existing_miss;
        uint32_t odp_in_port = 0;
        struct flow flow;
        uint32_t hash;
        int error;

        error = odp_flow_key_to_flow(upcall->key, upcall->key_len, &flow);
        odp_in_port = flow.in_port;

        if (error == ENODEV) {
            /** TODO: do nothing */
            continue;
        }
        if (error) {
            continue;
        }

        flow_extract(upcall->packet, flow.skb_priority, flow.skb_mark,
                     &flow.tunnel, flow.in_port, &miss->flow);

        /* Add other packets to a to-do list. */
        hash = flow_hash(&miss->flow, 0);
        existing_miss = flow_miss_find(&todo, &miss->flow, hash);
        if (!existing_miss) {
            hmap_insert(&todo, &miss->hmap_node, hash);
            miss->ofproto = ofproto;
            miss->key = upcall->key;
            miss->key_len = upcall->key_len;
            miss->upcall_type = upcall->type;
            miss->odp_in_port = odp_in_port;
            list_init(&miss->packets);

            n_misses++;
        } else {
            miss = existing_miss;
        }
        list_push_back(&miss->packets, &upcall->packet->list_node);
    }

    /* Process each element in the to-do list, constructing the set of
     * operations to batch. */
    n_ops = 0;
    HMAP_FOR_EACH (miss, hmap_node, &todo) {
        handle_flow_miss(ofproto, miss, NULL, &n_ops);
    }

    hmap_destroy(&todo);
}

static int
handle_upcalls__(unsigned int max_batch)
{
    struct ctc_upcall misses[FLOW_MISS_MAX_BATCH];
    struct ofpbuf miss_bufs[FLOW_MISS_MAX_BATCH];
    int n_processed;
    int n_misses;
    int i;

    ovs_assert(max_batch <= FLOW_MISS_MAX_BATCH);

    n_misses = 0;
    for (n_processed = 0; n_processed < max_batch; n_processed++) {
        struct ctc_upcall *upcall = &misses[n_misses];
        struct ofpbuf *packet = &miss_bufs[n_misses];
        int error;

        ofpbuf_use_stub(packet, g_miss_buf[n_misses],
                        FLOW_MISS_BUF_SIZE);
        /* Reset packet contents. */
        ofpbuf_clear(packet);
        ofpbuf_reserve(packet, CTC_NETDEV_HEADROOM);
        error = ofproto_netdev_port_output(upcall, packet);
        if (error || packet->size < ETH_HEADER_LEN) {
            break;
        }

        switch (upcall->type) {
        case CTC_UC_MISS:
            /* Handle it later. */
            n_misses++;
            break;

        case CTC_UC_ACTION:
        case CTC_N_UC_TYPES:
        default:
            ofpbuf_uninit(packet);
            break;
        }
    }

    if (n_misses == 0) {
        return 0;
    }

    /* Handle deferred MISS_UPCALL processing. */
    handle_miss_upcalls(misses, n_misses);
    for (i = 0; i < n_misses; i++) {
        ofpbuf_uninit(&miss_bufs[i]);
    }
    return n_processed;
}

static int
handle_upcalls(void)
{
    unsigned int work;

    /* Handle one or more batches of upcalls, until there's nothing left to do
     * or until we do a fixed total amount of work.
     *
     * We do work in batches because it can be much cheaper to set up a number
     * of flows and fire off their patches all at once.  We do multiple batches
     * because in some cases handling a packet can cause another packet to be
     * queued almost immediately as part of the return flow.  Both
     * optimizations can make major improvements on some benchmarks and
     * presumably for real traffic as well. */
    work = 0;
    while (work < FLOW_MISS_MAX_BATCH) {
        int retval = handle_upcalls__(FLOW_MISS_MAX_BATCH - work);
        if (retval <= 0) {
            return -retval;
        }
        work += retval;
    }

    return 0;
}

/* Basic life-cycle. */

static struct ofproto *
alloc(void)
{
    /* Only one bridge can be created for hardware provider. */
    if (ofproto) {
        return NULL;
    }

    ofproto = xzalloc(sizeof *ofproto);
    return &ofproto->up;
}

static void
dealloc(struct ofproto *ofproto_)
{
    if (ofproto != ofproto_ctc_cast(ofproto_)) {
        return;
    }

    free(ofproto);
    ofproto = NULL;
}

static int
construct(struct ofproto *ofproto_)
{
#define CLI_BUF_256 256
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);
    int error = 0;
    int i;
    struct oftable *table;
    struct ofputil_table_feature_prop *otfp;
    char company[CLI_BUF_256] = {0};
    char hardware_type[CLI_BUF_256] = {0};

    /* XXX: 'ofproto_create' sets the following descriptions to NULL before
     * call this function, we overwrite these variables according to our
     * device. please note as commented in 'ofproto_class' definition, it's
     * may not be good to change the base data instead of just refer to it.*/
    oem_info_get_company_name(company);
    oem_info_get_hardware_type(hardware_type);
    ofproto_->mfr_desc = xstrdup(company);
    ofproto_->hw_desc = xstrdup(hardware_type);
    ofproto_->sw_desc = xstrdup(CTC_SW_DESC);
    ofproto_->serial_desc = xstrdup((char*)CTC_SERIAL_DESC);
    ofproto_->dp_desc = xstrdup(CTC_DP_DESC);

    ofproto_init_max_ports(ofproto_, MIN((int)MAX_PORTS, (int)OFPP_MAX));

    ofproto_init_tables(ofproto_, N_TABLES);

    /* Init table 0 feature properties. */
    table = &ofproto_->tables[0];
    if (table) {
        /* Max flow entries number is 2560, this will be overwritten by
           bridge_configure_tables, so we need call get_tables instead when
           handle table features.
           table->max_flows = ENTRY_ID_MAX_SIZE;
        */
        /* No meta data support currently. */
        table->metadata_match = 0;
        table->metadata_write = 0;

        /* 2 instructions. */
        otfp = ofproto_init_table_feature_property(table,
                                                OFPTFPT13_INSTRUCTIONS,
                                                3 * 4);
        if (otfp) {
            ofproto_table_property_add_instruction(otfp, OFPIT13_METER);
            ofproto_table_property_add_instruction(otfp, OFPIT11_APPLY_ACTIONS);
            ofproto_table_property_add_instruction(otfp, OFPIT11_WRITE_ACTIONS);
        }

        /* 9 OF actions + 2 nx actions. */
        otfp = ofproto_init_table_feature_property(table,
                                                OFPTFPT13_WRITE_ACTIONS,
                                                9 * 4 + 2 * 10);
        if (otfp) {
            ofproto_table_property_add_action(otfp, OFPAT11_OUTPUT);
            ofproto_table_property_add_action(otfp, OFPAT11_SET_MPLS_TTL);
            ofproto_table_property_add_action(otfp, OFPAT11_PUSH_VLAN);
            ofproto_table_property_add_action(otfp, OFPAT11_POP_VLAN);
            ofproto_table_property_add_action(otfp, OFPAT11_PUSH_MPLS);
            ofproto_table_property_add_action(otfp, OFPAT11_POP_MPLS);
            ofproto_table_property_add_action(otfp, OFPAT11_SET_QUEUE);
            ofproto_table_property_add_action(otfp, OFPAT11_GROUP);
            ofproto_table_property_add_action(otfp, OFPAT12_SET_FIELD);

            /* No need to add this null experimenter actions
             * ofproto_table_property_add_action(otfp, OFPAT11_EXPERIMENTER); */
            ofproto_table_property_add_nxaction(otfp, NXAST_PUSH_L2);
            ofproto_table_property_add_nxaction(otfp, NXAST_POP_L2);
        }

        /* 9 OF actions + 2 nx actions. */
        otfp = ofproto_init_table_feature_property(table,
                                                OFPTFPT13_APPLY_ACTIONS,
                                                9 * 4 + 2 * 10);
        if (otfp) {
            ofproto_table_property_add_action(otfp, OFPAT11_OUTPUT);
            ofproto_table_property_add_action(otfp, OFPAT11_SET_MPLS_TTL);
            ofproto_table_property_add_action(otfp, OFPAT11_PUSH_VLAN);
            ofproto_table_property_add_action(otfp, OFPAT11_POP_VLAN);
            ofproto_table_property_add_action(otfp, OFPAT11_PUSH_MPLS);
            ofproto_table_property_add_action(otfp, OFPAT11_POP_MPLS);
            ofproto_table_property_add_action(otfp, OFPAT11_SET_QUEUE);
            ofproto_table_property_add_action(otfp, OFPAT11_GROUP);
            ofproto_table_property_add_action(otfp, OFPAT12_SET_FIELD);
            /* No need to add this null experimenter actions
             * ofproto_table_property_add_action(otfp, OFPAT11_EXPERIMENTER); */
            ofproto_table_property_add_nxaction(otfp, NXAST_PUSH_L2);
            ofproto_table_property_add_nxaction(otfp, NXAST_POP_L2);
        }

        /* 18 32-bit oxm fields. */
        otfp = ofproto_init_table_feature_property(table, OFPTFPT13_MATCH,
                                                   18 * 4);
        if (otfp) {
            ofproto_table_property_add_oxm(otfp, OXM_OF_IN_PORT);
            /* No need to add OXM_OF_ETH_DST. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_DST_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_SRC_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_TYPE);
            /* Not support vlan vid mask now. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_VID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_PCP);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IP_DSCP);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IP_PROTO);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_SRC_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_DST_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ICMPV4_TYPE);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ICMPV4_CODE);
            /* V330 does not support any arp fields. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_LABEL);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TUNNEL_ID);

        }

        /* OF spec 1.3.1:
         * The OFPTFPT_WILDCARDS property indicates the fields for which that
         * particular table supports wildcarding (omiting).
         * We only list the fields that we support them, we will ignore the
         * unsupported fields like IPv6.
         *
         * 18 32-bit oxm fields. */
        otfp = ofproto_init_table_feature_property(table, OFPTFPT13_WILDCARDS,
                                                   18 * 4);
        if (otfp) {
            ofproto_table_property_add_oxm(otfp, OXM_OF_IN_PORT);
            /* No need to add OXM_OF_ETH_DST. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_DST_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_SRC_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_TYPE);
            /* Not support vlan vid mask now. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_VID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_PCP);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IP_DSCP);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IP_PROTO);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_SRC_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_DST_W);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ICMPV4_TYPE);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ICMPV4_CODE);
            /* V330 does not support any arp fields. */
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_LABEL);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TUNNEL_ID);
        }

        /* Note: the following field should be consistent with function
         * adpt_flow_translate_reg_load.
         *
         * 9 32-bit oxm fields. */
        /* set_field for write-actions */
        otfp = ofproto_init_table_feature_property(table,
                                                OFPTFPT13_WRITE_SETFIELD,
                                                9 * 4);
        if (otfp) {
            ofproto_table_property_add_oxm(otfp, OXM_OF_TUNNEL_ID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_VID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_LABEL);
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_TC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_DST);
        }

        otfp = ofproto_init_table_feature_property(table,
                                                OFPTFPT13_APPLY_SETFIELD,
                                                9 * 4);

        /* set_field for apply-actions */
        if (otfp) {
            ofproto_table_property_add_oxm(otfp, OXM_OF_TUNNEL_ID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_ETH_SRC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_VLAN_VID);
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_LABEL);
            ofproto_table_property_add_oxm(otfp, OXM_OF_MPLS_TC);
            ofproto_table_property_add_oxm(otfp, OXM_OF_IPV4_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_TCP_DST);
            ofproto_table_property_add_oxm(otfp, OXM_OF_UDP_DST);
        }



        /* OF spec 1.3.1:
         * When a property of the table-miss flow entry is the same as the
         * corresponding property for regular flow entries (i.e. both properties
         * have the same list of capabilities), this table-miss property can be
         * omited from the property list.
         * We have the same capability for table-miss flow entry and regular
         * flow entry, so we omit them here.*/
    }

    for (i = 0; i < MAX_PORTS; i++) {
        ofproto->ports[i] = NULL;
    }

    timer_set_duration(&ofproto->next_expiration, 10);

    ofproto_->ogf.types = (1u << OFPGT11_ALL)     |
                          (1u << OFPGT11_SELECT)  |
                          (1u << OFPGT11_INDIRECT)|
                          (1u << OFPGT11_FF);

    ofproto_->ogf.capabilities = OFPGFC12_SELECT_LIVENESS;
    ofproto_->ogf.max_groups[OFPGT11_ALL]      = OFP_GROUP_NUM_TYPE_ALL;
    ofproto_->ogf.max_groups[OFPGT11_SELECT]   = OFP_GROUP_NUM_TYPE_SELECT;
    ofproto_->ogf.max_groups[OFPGT11_INDIRECT] = OFP_GROUP_NUM_TYPE_INDIRECT;
    ofproto_->ogf.max_groups[OFPGT11_FF]       = OFP_GROUP_NUM_TYPE_FF;

    /* Must consider OF1.1+ (group started at OF1.1+), so the SET_FIELD_actions
     * must also be reported. */
    ofproto_->ogf.actions[OFPGT11_ALL]
        = (1u << OFPAT11_OUTPUT)       |
          (1u << OFPAT11_SET_VLAN_VID) |
          (1u << OFPAT11_SET_DL_SRC)   |
          (1u << OFPAT11_SET_DL_DST)   |
        /*(1u << OFPAT11_PUSH_MPLS)  |
          (1u << OFPAT11_POP_MPLS)     | */
          (1u << OFPAT11_PUSH_VLAN)    |
          (1u << OFPAT11_POP_VLAN)     |
          (1u << OFPAT11_SET_NW_DST)   |
          (1u << OFPAT11_SET_TP_DST)   |
          (1u << OFPAT12_SET_FIELD);

    ofproto_->ogf.actions[OFPGT11_FF]
        = (1u << OFPAT11_OUTPUT)       |
          (1u << OFPAT11_SET_VLAN_VID) |
          (1u << OFPAT11_SET_DL_SRC)   |
          (1u << OFPAT11_SET_DL_DST)   |
        /*(1u << OFPAT11_PUSH_MPLS)  |
          (1u << OFPAT11_POP_MPLS)     | */
          (1u << OFPAT11_PUSH_VLAN)    |
          (1u << OFPAT11_POP_VLAN)     |
          (1u << OFPAT11_SET_NW_DST)   |
          (1u << OFPAT11_SET_TP_DST)   |
          (1u << OFPAT12_SET_FIELD);

    ofproto_->ogf.actions[OFPGT11_SELECT]
        = (1u << OFPAT11_OUTPUT)       |
          (1u << OFPAT11_SET_VLAN_VID) |
          (1u << OFPAT11_SET_DL_SRC)   |
          (1u << OFPAT11_SET_DL_DST)   |
        /*(1u << OFPAT11_PUSH_MPLS)  |
          (1u << OFPAT11_POP_MPLS)     | */
          (1u << OFPAT11_PUSH_VLAN)    |
          (1u << OFPAT11_POP_VLAN)     |
          (1u << OFPAT11_SET_NW_DST)   |
          (1u << OFPAT11_SET_TP_DST)   |
          (1u << OFPAT12_SET_FIELD);

    ofproto_->ogf.actions[OFPGT11_INDIRECT]
        = (1u << OFPAT11_GROUP);

    ofp_ofproto_construct();

    return error;
}


static void
destruct(struct ofproto *ofproto_ OVS_UNUSED)
{
    ofp_ofproto_destruct();
}

static int
run_fast(struct ofproto *ofproto_ OVS_UNUSED)
{
    ofproto_netdev_port_recv(&ofproto->up);
    return 0;
}

/* If 'rule' is an OpenFlow rule, that has expired according to OpenFlow rules,
 * then delete it entirely. */
static void
rule_expire(struct rule_ctc *rule)
{
    long long int now;
    uint8_t reason;

    if (rule->up.pending) {
        /* We'll have to expire it later. */
        return;
    }

    /* Update rule->used for idle_timeout using ofp_api. */
    if (rule->up.idle_timeout) {
        ofp_get_flow_last_matched_time(rule, &rule->up.used);
    }

    /* Has 'rule' expired? */
    now = time_msec();
    if (rule->up.hard_timeout
        && now > rule->up.modified + rule->up.hard_timeout * 1000) {
        reason = OFPRR_HARD_TIMEOUT;
    } else if (rule->up.idle_timeout
               && now > rule->up.used + rule->up.idle_timeout * 1000) {
        reason = OFPRR_IDLE_TIMEOUT;
    } else {
        return;
    }

    /* Get rid of the rule. */
    ofproto_rule_expire(&rule->up, reason);
}

/* This function is called periodically by run().  Its job is to collect
 * updates for the flows that have been installed into the datapath, most
 * importantly when they last were used, and then use that information to
 * expire flows that have not been used recently.
 *
 * Returns the number of milliseconds after which it should be called again. */
static int
expire(struct ofproto_ctc *ofproto)
{
#define N_EXPRIE_STEP 3
#define N_EXPRIE_MIN 10
#define N_EXPRIE_MAX 100
    struct rule_ctc *rule, *next_rule;
    struct oftable *table;
    static int time = N_EXPRIE_MIN;

    /* Expire OpenFlow flows whose idle_timeout or hard_timeout has passed. */
    OFPROTO_FOR_EACH_TABLE (table, &ofproto->up) {
        struct cls_cursor cursor;

        cls_cursor_init(&cursor, &table->cls, NULL);
        CLS_CURSOR_FOR_EACH_SAFE (rule, next_rule, up.cr, &cursor) {
            rule_expire(rule);
        }
    }

    if (ofproto->fast_expiration == TRUE) {
        ofproto->fast_expiration = FALSE;
        time = N_EXPRIE_MIN;
    }
    else {
        if (time < N_EXPRIE_MAX) {
            time = time + N_EXPRIE_STEP;
        }
    }

    return time;
}

static int
run(struct ofproto *ofproto_)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);

    if (timer_expired(&ofproto->next_expiration)) {
        int delay = expire(ofproto);
        timer_set_duration(&ofproto->next_expiration, delay);
    }

    handle_upcalls();
    return 0;
}

static void
wait__(struct ofproto *ofproto_ OVS_UNUSED)
{
    timer_wait(&ofproto->next_expiration);
}

static void
get_features(struct ofproto *ofproto_ OVS_UNUSED,
             bool *arp_match_ip, enum ofputil_action_bitmap *actions)
{
    /* Humber does not support arp_match_ip. */
    *arp_match_ip = false;
    /* We support limited range of actions, leave original code
     * for reference, this is only useful for OF 1.0, in OF 1.3
     * match/instruction/actions capability are reported by using
     * Table features multipart request.
     */
    *actions = (OFPUTIL_A_OUTPUT |
                OFPUTIL_A_SET_VLAN_VID |
                OFPUTIL_A_STRIP_VLAN |
                OFPUTIL_A_SET_DL_SRC |
                OFPUTIL_A_SET_DL_DST |
                OFPUTIL_A_SET_NW_DST |
                OFPUTIL_A_DEC_NW_TTL |
                OFPUTIL_A_SET_TP_DST);
}

static void
get_capabilities(struct ofproto *ofproto_ OVS_UNUSED,
                 enum ofp_version version, enum ofputil_capabilities *capabilities)
{
    *capabilities =  (OFPUTIL_C_FLOW_STATS | OFPUTIL_C_TABLE_STATS |
                      OFPUTIL_C_PORT_STATS);

    switch (version) {
    case OFP10_VERSION:
        break;
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION:
        *capabilities |= OFPUTIL_C_GROUP_STATS;
        *capabilities |= OFPUTIL_C_QUEUE_STATS;
        break;
    default:
        NOT_REACHED();
    }
}

static void
get_datapath_id(const struct ofproto *ofproto, uint64_t *datapath_id)
{
    uint8_t ea[ETH_ADDR_LEN];
    int error;

    *datapath_id = 0;

    ofproto = ofproto; /* TODO unused parameter */

    /* Use mac addr of eth0(management interface), since the mac addr of eth0
     * is an unique constant value. */
    error = ofp_netdev_get_etheraddr("eth0", ea);
    if (error) {
        VLOG_WARN("Could not get MAC address for eth0.");
        return;
    }

    *datapath_id = eth_addr_to_uint64(ea);
}

static void
initial_cfg_done(const struct ofproto *ofproto OVS_UNUSED)
{
#define OFP_SYNC_DONE_FILE "/tmp/ofp_sync_done"

    FILE * fp = fopen(OFP_SYNC_DONE_FILE, "a");
    if (fp != NULL)
    {
        fclose(fp);
    }
}

static void
get_tables(struct ofproto *ofproto_, struct ofp12_table_stats *ots)
{
    struct oftable *table;
    struct classifier *cls;
    ofp_stats_t miss_stats = {0};
    struct cls_cursor cursor;
    struct rule *rule;
    ofp_stats_t old_match_stats = {0};
    uint64_t total_matched = 0, missed = 0;

    strcpy(ots->name, "classifier");

    /* XXX: update ots->wildcards when implementing of 1.3. */
    ots->max_entries = htonl(ENTRY_ID_MAX_SIZE);

    ofp_get_flow_missmatch_stats(&miss_stats);
    missed = miss_stats.packet_count;

    ofp_get_flow_old_stats(&old_match_stats);
    total_matched = old_match_stats.packet_count;

    for (table = &ofproto_->tables[0];
         table < &ofproto_->tables[ofproto_->n_tables];
         table++) {
        cls = &table->cls;
        cls_cursor_init(&cursor, cls, NULL);
        CLS_CURSOR_FOR_EACH (rule, cr, &cursor) {
            ofp_stats_t flow_matched = {0};
            struct rule_ctc *rule_ctc = rule_ctc_cast(rule);
            ofp_get_flow_stats(rule_ctc, &flow_matched);
            total_matched += flow_matched.packet_count;
        }
    }

    ots->lookup_count  = htonll(total_matched + missed);
    ots->matched_count = htonll(total_matched);
}

static struct ofport *
port_alloc(void)
{
    struct ofport_ctc *port = xmalloc(sizeof *port);
    return &port->up;
}

static void
port_dealloc(struct ofport *port_)
{
    struct ofport_ctc *port = ofport_ctc_cast(port_);
    free(port);
}

static int
port_construct(struct ofport *port_)
{
    port_ = port_; /* TODO unused parameter */

    /* Nothing to do currently. */
    return 0;
}

static void
port_destruct(struct ofport *port_)
{
    port_ = port_; /* TODO unused parameter */

    /* Nothing to do currently. */
}

static void
port_modified(struct ofport *port_)
{
    ofp_netdev_clear_port_modified(port_->netdev->netdev_dev->name);
}

static void
port_reconfigured(struct ofport *port_, enum ofputil_port_config old_config)
{
    struct ofport_ctc *port = ofport_ctc_cast(port_);

    old_config = old_config; /* TODO unused parameter */

    /* support  OFPPC_NO_RECV, OFPPC_NO_RECV_STP, OFPPC_NO_FWD */
    ofp_port_set_config(port->up.ofp_port, port->up.pp.config);
}

/* Ports. */

static int
port_query_by_name(const struct ofproto *ofproto_, const char *devname,
                   struct ofproto_port *ofproto_port)
{
    int error = 0;
    ofp_error_t ofp_api_error = OFP_ERR_SUCCESS;

    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);

    ofp_api_error = ofp_port_get_ofport_by_name(devname,
                                                &ofproto_port->ofp_port);
    if (ofp_api_error != OFP_ERR_SUCCESS) {
        return ENODEV;
    }
    ovs_assert(ofproto_port->ofp_port > 0 && ofproto_port->ofp_port < MAX_PORTS);
    if (!ofproto->ports[ofproto_port->ofp_port]) {
         return ENODEV;
    }

    ofproto_port->name = xstrdup(devname);
    ofproto_port->type = xstrdup(OFP_DEFAULT_NETDEV_TYPE);

    return error;
}

static int
port_add(struct ofproto *ofproto_, struct netdev *netdev_)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);
    const char *name = netdev_get_name(netdev_);
    const char *type = netdev_get_type(netdev_);
    struct netdev *netdev = NULL;
    ofp_error_t ofp_api_error = OFP_ERR_SUCCESS;
    int error = 0;
    ofp_port_info_t port_info;

    if (0 != strcmp(type, OFP_DEFAULT_NETDEV_TYPE) && 0 != strcmp(type, "gre")) {
        return ECANCELED;
    }

    /* Allocate a netdev in datapath for reference in ofproto->ports. */
    error = netdev_open(name, type, &netdev);
    if (error) {
        return error;
    }

    memset(&port_info, 0, sizeof(port_info));
    /* XXX: port_info.ifindex should be removed later. */
    strncpy(port_info.name, name, OFP_IFNAME_SIZE);
    if (0 == strcmp(type, OFP_DEFAULT_NETDEV_TYPE)) {
        if (ofp_netdev_name_is_phy_port(name)) {
            port_info.type = OFP_INTERFACE_TYPE_PHYSICAL;
            ofp_api_error = ofp_port_get_ofport_by_name(name,
                                                        &port_info.ofport);
            if (ofp_api_error != OFP_ERR_SUCCESS) {
                VLOG_WARN("Can't find port name %s when adding port",
                        name);
                netdev_close(netdev);
                return ECANCELED;
            }
        }
        else {
            netdev_close(netdev);
            return ECANCELED;
        }
    } else if (0 == strcmp(type, "gre")) {
        port_info.type = OFP_INTERFACE_TYPE_GRE;
        port_info.info = xmalloc(sizeof(ofp_tunnel_info_t));
        error = ofp_netdev_assign_gre_ofport(name, &port_info.ofport);
        if (error != OFP_ERR_SUCCESS)
        {
            VLOG_ERR("Fail to create tunnel due to invalid tunnel name '%s'.", name);
            free(port_info.info);
            port_info.info = NULL;
            netdev_close(netdev);
            return ECANCELED;
        }
        error = netdev_vport_get_tunnel_info(netdev->netdev_dev, (ofp_tunnel_info_t*) port_info.info);
        if (error != 0) {
            VLOG_ERR("Fail to get tunnel information '%s'.", name);
            free(port_info.info);
            port_info.info = NULL;
            netdev_close(netdev);
            return ECANCELED;
        }
        port_info.ifindex = 0;
    }

    ofp_api_error = ofp_port_add(&port_info);
    free(port_info.info);
    port_info.info = NULL;
    if (ofp_api_error != OFP_ERR_SUCCESS) {
        VLOG_ERR("Adding port failed with name %s", name);
        netdev_close(netdev);
        return ECANCELED;
    }

    ovs_assert(port_info.ofport > 0 && port_info.ofport < MAX_PORTS);
    ofproto->ports[port_info.ofport] = netdev;

    return error;
}

static enum ofperr
collect_rules_loose__(struct ofproto_ctc *ofproto,
                      const struct match *match,
                      uint16_t out_port, uint32_t out_group, struct list *rules)
{
    struct oftable *table;
    struct cls_rule cr;
    enum ofperr error = 0;
    ovs_be64 cookie = 0;
    ovs_be64 cookie_mask = 0;

    list_init(rules);
    cls_rule_init(&cr, match, 0);

    OFPROTO_FOR_EACH_TABLE (table, &ofproto->up) {
        struct cls_cursor cursor;
        struct rule *rule;

        cls_cursor_init(&cursor, &table->cls, &cr);
        CLS_CURSOR_FOR_EACH (rule, cr, &cursor) {
            if (rule->pending) {
                error = OFPROTO_POSTPONE;
                goto exit;
            }
            if (!ofproto_rule_is_hidden(rule)
                && ofproto_rule_has_out_port(rule, out_port)
                && ofproto_rule_has_out_group(rule, out_group)
                    && !((rule->flow_cookie ^ cookie) & cookie_mask)) {
                list_push_back(rules, &rule->ofproto_node);
            }
        }
    }

exit:
    cls_rule_destroy(&cr);
    return error;
}

static int
remove_rules_by_in_port__(struct ofproto_ctc *ofproto, uint16_t ofp_port)
{
    struct list rules;
    struct match match;
    struct rule *rule, *next;
    int error = 0;

    list_init(&rules);
    match_init_catchall(&match);
    match_set_in_port(&match, ofp_port);
    error = collect_rules_loose__(ofproto, &match, OFPP_NONE, OFPG11_ANY, &rules);
    if (error || list_is_empty(&rules))
    {
        return error;
    }

    LIST_FOR_EACH_SAFE (rule, next, ofproto_node, &rules) {
        ofproto_rule_expire(rule, OFPRR_DELETE);
    }

    return 0;
}

static int
remove_rules_by_output_port__(struct ofproto_ctc *ofproto, uint16_t ofp_port)
{
    struct list rules;
    struct match match;
    struct rule *rule = NULL;
    struct rule *next = NULL;
    int error = 0;

    list_init(&rules);
    match_init_catchall(&match);
    error = collect_rules_loose__(ofproto, &match, ofp_port, OFPG11_ANY, &rules);
    if (error || list_is_empty(&rules))
    {
        return error;
    }

    LIST_FOR_EACH_SAFE (rule, next, ofproto_node, &rules) {
        ofproto_rule_expire(rule, OFPRR_DELETE);
    }

    return 0;
}

static int
port_del(struct ofproto *ofproto_, uint16_t ofp_port)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);

    remove_rules_by_in_port__(ofproto, ofp_port);
    remove_rules_by_output_port__(ofproto, ofp_port);

    ofp_port_del(ofp_port);

    ovs_assert(ofp_port > 0 && ofp_port < MAX_PORTS);
    netdev_close(ofproto->ports[ofp_port]);
    ofproto->ports[ofp_port] = NULL;

    return 0;
}

static int
port_get_stats(const struct ofport *ofport_, struct netdev_stats *stats)
{
    struct ofport_ctc *ofport = ofport_ctc_cast(ofport_);

    return netdev_get_stats(ofport->up.netdev, stats);
}

struct port_dump_state {
    uint32_t port_no;
};

static int
port_dump_start(const struct ofproto *ofproto_ OVS_UNUSED, void **statep)
{
    struct port_dump_state *state;

    *statep = state = xmalloc(sizeof *state);
    state->port_no = 0;

    return 0;
}

static int
port_dump_next(const struct ofproto *ofproto_, void *state_,
               struct ofproto_port *port)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);
    struct port_dump_state *state = state_;
    uint32_t port_no;

    for (port_no = state->port_no; port_no < MAX_PORTS; port_no++) {
        struct netdev *netdev = ofproto->ports[port_no];
        if (netdev) {
            port->name = (char *)(netdev_get_name(netdev));
            port->type = (char *)(netdev_get_type(netdev));
            port->ofp_port = port_no;

            state->port_no = port_no + 1;
            return 0;
        }
    }

    return EOF;
}

static int
port_dump_done(const struct ofproto *ofproto_ OVS_UNUSED, void *state_)
{
    struct port_dump_state *state = state_;

    free(state);

    return 0;
}

static int
port_poll(const struct ofproto *ofproto_ OVS_UNUSED, char **devnamep OVS_UNUSED)
{
    /* uint8_ofp has_changed = 0; */
    struct ofproto_port ofproto_port;
    int ret = 0;

    ofp_netdev_get_any_port_modified(devnamep);
    if (*devnamep == NULL) {
        return EAGAIN;
    }

    ret = ofproto_port_query_by_name(ofproto_, *devnamep, &ofproto_port);
    ofproto_port_destroy(&ofproto_port);
    if (0 != ret)
    {
        free(*devnamep);
        *devnamep = NULL;
        return EAGAIN;
    }

    return 0; /* Just return 0 to indicate update specific netdev */
}

/* Rules. */

static struct rule *
rule_alloc(void)
{
    struct rule_ctc *rule = xmalloc(sizeof *rule);
    return &rule->up;
}

static void
rule_dealloc(struct rule *rule_)
{
    struct rule_ctc *rule = rule_ctc_cast(rule_);
    free(rule);
}

static enum ofperr
rule_construct(struct rule *rule_)
{
    struct rule_ctc *rule = rule_ctc_cast(rule_);
    struct rule_ctc *victim;
    enum ofperr ofp_error = 0;
    int error = 0;

    rule->packet_count = 0;
    rule->byte_count = 0;
    list_init(&rule->flow_actions);

    victim = rule_ctc_cast(ofoperation_get_victim(rule->up.pending));

    if (victim && victim->is_mpls_pop) {
        VLOG_ERR("Modifying flow failed, "
                 "can't modify flow which matches MPLS label.\n");
        ofp_error = OFPERR_OFPFMFC_UNSUPPORTED;
        goto err0;
    }

    error = ofp_translate_ofpact(rule_->ofpacts, rule_->ofpacts_len,
                                 &rule->flow_actions);
    if (error) {
        goto err1;
    }

    minimatch_expand(&(rule_->cr.match), &(rule->match));

    error = ofp_validate_action_data(rule);
    if (error) {
        goto err1;
    }

    /* XXX: translate error. */
    error = ofp_add_flow(rule);
    if (error) {
        goto err1;
    }

    /* Same-key-different action. */
    if (victim) {
        if (ofp_del_flow(victim)) {
            VLOG_ERR("Remove flow entry failed");
        }
    }

    /* The translated actions is not longer used, free it. */
    ofp_destroy_flow_actions(&rule->flow_actions);

    ofoperation_complete(rule_->pending, 0);

    return 0;

err1:
    /* Free any resources associated with the rule, ovs will call rule_dealloc
     * only. */

    ofp_destroy_flow_actions(&rule->flow_actions);

    ofp_error = translate_adpt_error_code(error, OFP_TYPE_FLOW);

err0:
    /* When we found some error during adding flow, we should not call
     * ofoperation_complete as ofproto core will do the rollback, this
     * behavior is documented in ofproto_class structure in ofproto-provider.h
     */
    return ofp_error;
}

static void
rule_destruct(struct rule *rule_)
{
    struct rule_ctc *rule = rule_ctc_cast(rule_);
    int error = 0;

    error = ofp_del_flow(rule);
    if (error) {
        if (OFP_ERR_INVALID_IN_PORT_NUMBER == error) {
            VLOG_ERR("Invalid in_port port number");
        }
        else {
            VLOG_ERR("Remove flow entry failed, error code: %d", error);
        }
    }

    ofoperation_complete(rule_->pending, 0);
}

static void
rule_get_stats(struct rule *rule_, uint64_t *packets, uint64_t *bytes)
{
    struct rule_ctc *rule = rule_ctc_cast(rule_);
    struct ofp_stats_s ofp_stats;

    memset(&ofp_stats, 0 , sizeof(ofp_stats));

    ofp_get_flow_stats(rule, &ofp_stats);
    *packets = ofp_stats.packet_count;
    *bytes = ofp_stats.byte_count;
}

static void
rule_clear_stats(struct rule *rule_)
{
    rule_ = rule_; /* TODO unused parameter */
}

static enum ofperr
rule_execute(struct rule *rule_, const struct flow *flow,
             struct ofpbuf *packet)
{
    rule_ = rule_; /* TODO unused parameter */
    flow = flow; /* TODO unused parameter */
    packet = packet; /* TODO unused parameter */
    return 0;
}

static void
rule_modify_actions(struct rule *rule_)
{
    struct rule_ctc *rule = rule_ctc_cast(rule_);
    enum ofperr ofp_error = 0;
    int error = 0;

    if (rule->is_mpls_pop) {
        VLOG_ERR("Modifying flow failed, can't modify flow "
                 "which matches MPLS label.\n");
        ofp_error = OFPERR_OFPFMFC_UNSUPPORTED;
        goto err0;
    }

    error = ofp_translate_ofpact(rule_->ofpacts, rule_->ofpacts_len,
                                 &rule->flow_actions);
    if (error) {
        goto err1;
    }

    error = ofp_validate_action_data(rule);
    if (error) {
        goto err1;
    }

    error = ofp_modify_flow_action(rule);
    if (error) {
        goto err1;
    }

    ofp_destroy_flow_actions(&rule->flow_actions);

    ofoperation_complete(rule->up.pending, 0);

    return;

err1:
    ofp_destroy_flow_actions(&rule->flow_actions);

    ofp_error = translate_adpt_error_code(error, OFP_TYPE_FLOW);

err0:
    ofoperation_complete(rule->up.pending, ofp_error);
}

static bool
set_frag_handling(struct ofproto *ofproto_ OVS_UNUSED,
                  enum ofp_config_flags frag_handling OVS_UNUSED)
{
    /* We only support OFPC_FRAG_NORMAL mode, and OVS default is
     * OFPC_FRAG_NORMAL, thus we do not support change it. */
    return false;
}

static void
action_xlate_ctx_init(struct action_xlate_ctx *ctx,
                      struct ofproto_ctc *ofproto, const struct flow *flow,
                      ovs_be16 initial_tci, struct rule_ctc *rule_,
                      uint8_t tcp_flags, const struct ofpbuf *packet)
{
    ctx->ofproto = ofproto;
    ctx->flow = *flow;
    memset(&ctx->flow.tunnel, 0, sizeof ctx->flow.tunnel);
    ctx->base_flow = ctx->flow;
    ctx->base_flow.vlan_tci = initial_tci;
    ctx->rule = rule_;
    ctx->packet = packet;
    ctx->tcp_flags = tcp_flags;
}

static struct ofport_ctc *
get_ofp_port(const struct ofproto_ctc *ofproto, uint16_t ofp_port)
{
    struct ofport *ofport = ofproto_get_port(&ofproto->up, ofp_port);
    return ofport ? ofport_ctc_cast(ofport) : NULL;
}

/* Returns the ODP port number of the Linux VLAN device that corresponds to
 * 'vlan_tci' on the network device with port number 'realdev_odp_port' in
 * 'ofproto'.  For example, given 'realdev_odp_port' of eth0 and 'vlan_tci' 9,
 * it would return the port number of eth0.9.
 *
 * Unless VLAN splinters are enabled for port 'realdev_odp_port', this
 * function just returns its 'realdev_odp_port' argument. */
static uint32_t
vsp_realdev_to_vlandev(const struct ofproto_ctc *ofproto,
                       uint32_t realdev_odp_port, ovs_be16 vlan_tci)
{
    ofproto = ofproto; /* TODO unused parameter */
    realdev_odp_port = realdev_odp_port; /* TODO unused parameter */
    vlan_tci = vlan_tci; /* TODO unused parameter */

    return realdev_odp_port;
}

static void
compose_output_action(struct action_xlate_ctx *ctx, uint16_t ofp_port)
{
    const struct ofport_ctc *ofport = get_ofp_port(ctx->ofproto, ofp_port);
    uint32_t odp_port = ofp_port;
    ovs_be16 flow_vlan_tci = ctx->flow.vlan_tci;
    uint8_t flow_nw_tos = ctx->flow.nw_tos;
    uint32_t out_port;

    if (OFPP_IN_PORT != ofp_port && OFPP_FLOOD != ofp_port && OFPP_ALL != ofp_port)
    {
        if (!ofport) {
            VLOG_WARN("Nonexistent output port");
            return;
        } else if (ofport->up.pp.config & OFPUTIL_PC_NO_FWD) {
            VLOG_WARN("OFPPC_NO_FWD set, skipping output");
            return;
        }
    }

    out_port = vsp_realdev_to_vlandev(ctx->ofproto, odp_port,
                                      ctx->flow.vlan_tci);
    if (out_port != odp_port) {
        ctx->flow.vlan_tci = htons(0);
    }
    commit_odp_actions(&ctx->flow, &ctx->base_flow, ctx->odp_actions);
    nl_msg_put_u32(ctx->odp_actions, OVS_ACTION_ATTR_OUTPUT, out_port);

    ctx->sflow_odp_port = odp_port;
    ctx->sflow_n_outputs++;
    ctx->nf_output_iface = ofp_port;
    ctx->flow.vlan_tci = flow_vlan_tci;
    ctx->flow.nw_tos = flow_nw_tos;
}

static void
xlate_group_action(struct action_xlate_ctx *ctx,
                    uint32_t group_id)
{
    nl_msg_put_u32(ctx->odp_actions, OVS_ACTION_ATTR_GROUP, group_id);
}

static void
xlate_output_action(struct action_xlate_ctx *ctx,
                    uint16_t port, uint16_t max_len, bool may_packet_in)
{
    uint16_t prev_nf_output_iface = ctx->nf_output_iface;

    may_packet_in = may_packet_in; /* TODO unused parameter */
    ctx->nf_output_iface = NF_OUT_DROP;

    switch (port) {
    case OFPP_IN_PORT:
        /* 1. Only packets from any port will be handled by software, packets from standard port will be handled by hardware */
        /* 2. For packet_out, it is illegal if in_port == IN_PORT  */
        if (true == ctx->any_port_flow &&
                OFP_FLOW_PROCESS_TYPE_MATCH_TABLE_AND_FORWARD == ctx->flow_process_type) {
            compose_output_action(ctx, ctx->flow.in_port);
        }
        break;
    case OFPP_TABLE:
        /* we don't support table */
        VLOG_WARN("skipping output to unsupported port: TABLE\n");
        break;
    case OFPP_NORMAL:
        /* we don't support normal */
        VLOG_WARN("skipping output to unsupported port: NORMAL\n");
        break;
    case OFPP_FLOOD:
        /* only valid when packet-out */
        if (OFP_FLOW_PROCESS_TYPE_PACKET_OUT == ctx->flow_process_type) {
            compose_output_action(ctx,  port);
        }
        break;
    case OFPP_ALL:
        /* only valid when packet-out */
        if (OFP_FLOW_PROCESS_TYPE_PACKET_OUT == ctx->flow_process_type) {
            compose_output_action(ctx, port);
        }
        break;
    case OFPP_CONTROLLER:
        /* only valid when packet-in */
        if (OFP_FLOW_PROCESS_TYPE_MATCH_TABLE_AND_FORWARD == ctx->flow_process_type) {
            execute_controller_action(ctx, max_len, OFPR_ACTION, 0);
        }
        break;
    case OFPP_NONE:
        VLOG_WARN("skipping output to unsupported port: NONE\n");
        break;
    case OFPP_LOCAL:
        /* not support local */
        VLOG_WARN("skipping output to unsupported port: LOCAL\n");
        break;
    case OFPP_MAX:
    case OFPP_PW_FWD:
        /* not support reserved ports */
        VLOG_WARN("skipping output to unsupported port: 0x%x\n", port);
        break;
    default:
        /* for multiple outputs, only packet-out will be handled by software */
        if (OFP_FLOW_PROCESS_TYPE_PACKET_OUT == ctx->flow_process_type) {
            compose_output_action(ctx, port);
        }
        break;
    }

    if (prev_nf_output_iface == NF_OUT_FLOOD) {
        ctx->nf_output_iface = NF_OUT_FLOOD;
    } else if (ctx->nf_output_iface == NF_OUT_DROP) {
        ctx->nf_output_iface = prev_nf_output_iface;
    } else if (prev_nf_output_iface != NF_OUT_DROP &&
               ctx->nf_output_iface != NF_OUT_FLOOD) {
        ctx->nf_output_iface = NF_OUT_MULTI;
    }
}

static void
do_xlate_actions(const struct ofpact *ofpacts, size_t ofpacts_len,
                 struct action_xlate_ctx *ctx)
{
    const struct ofpact *a;
    int i = 0;
    int push_vlan_tags_count = ctx->flow.org_vlan_tag_count - 1;
    int8_t queue_id = 0;

    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
        if (ctx->exit) {
            break;
        }

        switch (a->type) {
        case OFPACT_OUTPUT:
            xlate_output_action(ctx, ofpact_get_OUTPUT(a)->port,
                                ofpact_get_OUTPUT(a)->max_len, true);
            break;

        case OFPACT_SET_VLAN_VID:
            ctx->flow.vlan_tci &= ~htons(VLAN_VID_MASK);
            ctx->flow.vlan_tci |= (htons(ofpact_get_SET_VLAN_VID(a)->vlan_vid)
                                   | htons(VLAN_CFI));
            break;

        case OFPACT_SET_VLAN_PCP:
            ctx->flow.vlan_tci &= ~htons(VLAN_PCP_MASK);
            ctx->flow.vlan_tci |= htons((ofpact_get_SET_VLAN_PCP(a)->vlan_pcp
                                         << VLAN_PCP_SHIFT)
                                        | VLAN_CFI);
            break;

        case OFPACT_STRIP_VLAN:
            /* Support QINQ */
            if (0 == push_vlan_tags_count) {
                ctx->flow.vlan_tci = htons(0);
                ctx->flow.vlan_tpid = htons(0);
                push_vlan_tags_count = -1;
            } else if (push_vlan_tags_count > 0) {
                ctx->flow.vlan_tci = ctx->flow.vlan_qinq_tci[0];
                ctx->flow.vlan_tpid = ctx->flow.vlan_qinq_tpid[0];

                for (i = 0; i < MAX_VLAN_TAGS - 2; i++) {
                    if (ctx->flow.vlan_qinq_tci[i + 1] & htons(VLAN_CFI)) {
                        ctx->flow.vlan_qinq_tci[i] = ctx->flow.vlan_qinq_tci[i + 1];
                        ctx->flow.vlan_qinq_tpid[i] = ctx->flow.vlan_qinq_tpid[i + 1];
                    }
                }
                if (push_vlan_tags_count >= 1 && push_vlan_tags_count < MAX_VLAN_TAGS) {
                    ctx->flow.vlan_qinq_tci[push_vlan_tags_count - 1] = htons(0);
                    ctx->flow.vlan_qinq_tpid[push_vlan_tags_count - 1] = htons(0);
                }
                push_vlan_tags_count --;
            }
            break;

        case OFPACT_PUSH_VLAN:
            /* XXX 802.1AD(QinQ) */
            /* Support QINQ */
            if (push_vlan_tags_count < MAX_VLAN_TAGS - 1) {
                for (i = MAX_VLAN_TAGS - 2; i >=0 ; i--) {
                    if (0 == i) {
                        ctx->flow.vlan_qinq_tci[0] = ctx->flow.vlan_tci;
                        ctx->flow.vlan_qinq_tpid[0] = ctx->flow.vlan_tpid;
                    } else {
                        ctx->flow.vlan_qinq_tci[i] = ctx->flow.vlan_qinq_tci[i - 1];
                        ctx->flow.vlan_qinq_tpid[i] = ctx->flow.vlan_qinq_tpid[i - 1];
                    }
                }
            }
            ctx->flow.vlan_tci = htons(VLAN_CFI);
            ctx->flow.vlan_tpid =  ofpact_get_PUSH_VLAN(a)->ethertype;
            push_vlan_tags_count++;
            break;

        case OFPACT_SET_ETH_SRC:
            memcpy(ctx->flow.dl_src, ofpact_get_SET_ETH_SRC(a)->mac,
                   ETH_ADDR_LEN);
            break;

        case OFPACT_SET_ETH_DST:
            memcpy(ctx->flow.dl_dst, ofpact_get_SET_ETH_DST(a)->mac,
                   ETH_ADDR_LEN);
            break;

        case OFPACT_SET_IPV4_SRC:
            ctx->flow.nw_src = ofpact_get_SET_IPV4_SRC(a)->ipv4;
            break;

        case OFPACT_SET_IPV4_DST:
            ctx->flow.nw_dst = ofpact_get_SET_IPV4_DST(a)->ipv4;
            break;

        case OFPACT_SET_IPV4_DSCP:
            /* OpenFlow 1.0 only supports IPv4. */
            if (ctx->flow.dl_type == htons(ETH_TYPE_IP)) {
                ctx->flow.nw_tos &= ~IP_DSCP_MASK;
                ctx->flow.nw_tos |= ofpact_get_SET_IPV4_DSCP(a)->dscp;
            }
            break;

        case OFPACT_SET_L4_SRC_PORT:
            ctx->flow.tp_src = htons(ofpact_get_SET_L4_SRC_PORT(a)->port);
            break;

        case OFPACT_SET_L4_DST_PORT:
            ctx->flow.tp_dst = htons(ofpact_get_SET_L4_DST_PORT(a)->port);
            break;

        /* REG_LOAD may be converted from OF1.2+ set_field action, so we must
         * support it. */
        case OFPACT_REG_LOAD:
            nxm_execute_reg_load(ofpact_get_REG_LOAD(a), &ctx->flow);
            break;

        case OFPACT_GROUP:
            xlate_group_action(ctx, ofpact_get_GROUP(a)->group_id);
            break;

        case OFPACT_SET_QUEUE:
            queue_id = ofpact_get_SET_QUEUE(a)->queue_id;
            if (queue_id >= OFP_MAX_QUEUE_VALUE || queue_id < 0) {
                ctx->flow.skb_priority = 0;
            } else {
                ctx->flow.skb_priority = queue_id * 8;
            }
            break;

        /* The following actions are not supported by our datapath(except MPLS
         * ,but we still do not support MPLS actions in packet-out because OVS
         * code can't process L2VPN scenario), we comment them out to keep
         * consistent with our hardware. */
        case OFPACT_CONTROLLER:
        case OFPACT_ENQUEUE:
        case OFPACT_RESUBMIT:
        case OFPACT_POP_QUEUE:
        case OFPACT_REG_MOVE:
        case OFPACT_DEC_TTL:
        case OFPACT_SET_TUNNEL:
        case OFPACT_NOTE:
        case OFPACT_MULTIPATH:
        case OFPACT_BUNDLE:
        case OFPACT_OUTPUT_REG:
        case OFPACT_LEARN:
        case OFPACT_EXIT:
        case OFPACT_FIN_TIMEOUT:
        case OFPACT_CLEAR_ACTIONS:
        case OFPACT_WRITE_METADATA:
        case OFPACT_GOTO_TABLE:
            break;

        case OFPACT_METER:
            break;

        case OFPACT_PUSH_MPLS:
        case OFPACT_POP_MPLS:
        case OFPACT_PUSH_L2:
        case OFPACT_POP_L2:
        case OFPACT_SET_MPLS_TTL:
            break;
        }
    }
}

/* Translates the 'ofpacts_len' bytes of "struct ofpacts" starting at 'ofpacts'
 * into datapath actions in 'odp_actions', using 'ctx'. */
static void
xlate_actions(struct action_xlate_ctx *ctx,
              const struct ofpact *ofpacts, size_t ofpacts_len,
              struct ofpbuf *odp_actions)
{
    ofpbuf_clear(odp_actions);
    ofpbuf_reserve(odp_actions, NL_A_U32_SIZE);

    ctx->odp_actions = odp_actions;
    ctx->tags = 0;
    ctx->nf_output_iface = NF_OUT_DROP;
    ctx->orig_skb_priority = ctx->flow.skb_priority;
    ctx->table_id = 0;
    ctx->exit = false;

    if (ctx->flow.nw_frag & FLOW_NW_FRAG_ANY) {
        switch (ctx->ofproto->up.frag_handling) {
        case OFPC_FRAG_NORMAL:
            /* We must pretend that transport ports are unavailable. */
            ctx->flow.tp_src = ctx->base_flow.tp_src = htons(0);
            ctx->flow.tp_dst = ctx->base_flow.tp_dst = htons(0);
            break;

        case OFPC_FRAG_DROP:
            return;

        case OFPC_FRAG_REASM:
            NOT_REACHED();

        case OFPC_FRAG_NX_MATCH:
            /* Nothing to do. */
            break;

        case OFPC_INVALID_TTL_TO_CONTROLLER:
            NOT_REACHED();
        }
    }

    do_xlate_actions(ofpacts, ofpacts_len, ctx);
}

static int
flow_from_nlattrs(const struct nlattr *key, uint32_t key_len,
                              struct flow *flow)
{
    if (odp_flow_key_to_flow(key, key_len, flow) != ODP_FIT_PERFECT) {
        return EINVAL;
    }

    if (flow->in_port < OFPP_MAX
        ? flow->in_port >= MAX_PORTS
        : flow->in_port != OFPP_LOCAL && flow->in_port != OFPP_NONE && flow->in_port != OFPP_CONTROLLER) {
        return EINVAL;
    }

    return 0;
}

static void
send_packet_to_chip(struct ofproto_ctc *ofproto, struct ofpbuf *packet)
{
    int retval;

    if (g_cpuport_fd <= 0) {
        ofproto_netdev_port_init();
    }

    if (g_cpuport_fd <= 0) {
        return;
    }

    ofproto = ofproto; /* TODO unused parameter */

    retval = write(g_cpuport_fd, packet->data, packet->size);
    if (retval < 0) {
        /* The Linux AF_PACKET implementation never blocks waiting for room
         * for packets, instead returning ENOBUFS.  Translate this into
         * EAGAIN for the caller. */
        if (errno == ENOBUFS) {
            return;
        } else if (errno == EINTR) {
            return;
        } else if (errno != EAGAIN) {
            VLOG_WARN("error sending Ethernet packet: %s", strerror(errno));
        }
        return;
    } else if (retval != packet->size) {
        VLOG_WARN("sent partial Ethernet packet (%zd bytes of "
                     "%zu)", retval, packet->size);
        return;
    } else {
        return;
    }
}

static void
set_dl(struct ofpbuf *packet, const struct ovs_key_ethernet *eth_key)
{
    struct eth_header *eh = packet->l2;

    memcpy(eh->eth_src, eth_key->eth_src, sizeof eh->eth_src);
    memcpy(eh->eth_dst, eth_key->eth_dst, sizeof eh->eth_dst);
}

static void
execute_set_action(struct ofpbuf *packet, const struct nlattr *a)
{
    enum ovs_key_attr type = nl_attr_type(a);
    const struct ovs_key_ipv4 *ipv4_key;
    const struct ovs_key_ipv6 *ipv6_key;
    const struct ovs_key_tcp *tcp_key;
    const struct ovs_key_udp *udp_key;

    switch (type) {
    case OVS_KEY_ATTR_TUN_ID:
    case OVS_KEY_ATTR_PRIORITY:
    case OVS_KEY_ATTR_SKB_MARK:
#ifdef __KERNEL__
    case OVS_KEY_ATTR_IPV4_TUNNEL:
#endif
        /* not implemented */
        break;

    case OVS_KEY_ATTR_ETHERNET:
        set_dl(packet,
                   nl_attr_get_unspec(a, sizeof(struct ovs_key_ethernet)));
        break;

    case OVS_KEY_ATTR_IPV4:
        ipv4_key = nl_attr_get_unspec(a, sizeof(struct ovs_key_ipv4));
        packet_set_ipv4(packet, ipv4_key->ipv4_src, ipv4_key->ipv4_dst,
                        ipv4_key->ipv4_tos, ipv4_key->ipv4_ttl);
        break;

    case OVS_KEY_ATTR_IPV6:
        ipv6_key = nl_attr_get_unspec(a, sizeof(struct ovs_key_ipv6));
        packet_set_ipv6(packet, ipv6_key->ipv6_proto, ipv6_key->ipv6_src,
                        ipv6_key->ipv6_dst, ipv6_key->ipv6_tclass,
                        ipv6_key->ipv6_label, ipv6_key->ipv6_hlimit);
        break;

    case OVS_KEY_ATTR_TCP:
        tcp_key = nl_attr_get_unspec(a, sizeof(struct ovs_key_tcp));
        packet_set_tcp_port(packet, tcp_key->tcp_src, tcp_key->tcp_dst);
        break;

     case OVS_KEY_ATTR_UDP:
        udp_key = nl_attr_get_unspec(a, sizeof(struct ovs_key_udp));
        packet_set_udp_port(packet, udp_key->udp_src, udp_key->udp_dst);
        break;

     case OVS_KEY_ATTR_UNSPEC:
     case OVS_KEY_ATTR_ENCAP:
     case OVS_KEY_ATTR_ETHERTYPE:
     case OVS_KEY_ATTR_IN_PORT:
     case OVS_KEY_ATTR_VLAN:
     case OVS_KEY_ATTR_ICMP:
     case OVS_KEY_ATTR_ICMPV6:
     case OVS_KEY_ATTR_ARP:
     case OVS_KEY_ATTR_ND:
     case __OVS_KEY_ATTR_MAX:
     case OVS_KEY_ATTR_TUNNEL:
     case OVS_KEY_ATTR_MPLS:
         break;
     default:
        NOT_REACHED();
    }
}

static void
execute_set_packet_priority(uint32_ofp *priority, const struct nlattr *a)
{
    enum ovs_key_attr type = nl_attr_type(a);

    switch (type) {
    case OVS_KEY_ATTR_PRIORITY:
        *priority = nl_attr_get_u32(a);
        break;
    case OVS_KEY_ATTR_TUN_ID:
    case OVS_KEY_ATTR_SKB_MARK:
#ifdef __KERNEL__
    case OVS_KEY_ATTR_IPV4_TUNNEL:
#endif
    case OVS_KEY_ATTR_ETHERNET:
    case OVS_KEY_ATTR_IPV4:
    case OVS_KEY_ATTR_IPV6:
    case OVS_KEY_ATTR_TCP:
    case OVS_KEY_ATTR_UDP:
    case OVS_KEY_ATTR_UNSPEC:
    case OVS_KEY_ATTR_ENCAP:
    case OVS_KEY_ATTR_ETHERTYPE:
    case OVS_KEY_ATTR_IN_PORT:
    case OVS_KEY_ATTR_VLAN:
    case OVS_KEY_ATTR_ICMP:
    case OVS_KEY_ATTR_ICMPV6:
    case OVS_KEY_ATTR_ARP:
    case OVS_KEY_ATTR_ND:
    case __OVS_KEY_ATTR_MAX:
    case OVS_KEY_ATTR_TUNNEL:
    case OVS_KEY_ATTR_MPLS:
         break;
     default:
        NOT_REACHED();
    }
}
static void
execute_controller_action(struct action_xlate_ctx *ctx, int len,
                          enum ofp_packet_in_reason reason,
                          uint16_t controller_id)
{
    struct ofputil_packet_in pin;
    struct ofpbuf *packet;
    int vlan_tag_index = 0;

    /*ctx->slow |= SLOW_CONTROLLER;*/
    if (!ctx->packet) {
        return;
    }

    packet = ofpbuf_clone(ctx->packet);

    if (packet->l2 && packet->l3) {
        struct eth_header *eh;
        uint16_t mpls_depth;

        /* Support QINQ */
        for (vlan_tag_index = 0; vlan_tag_index < ctx->flow.org_vlan_tag_count; vlan_tag_index++) {
            eth_pop_vlan(packet);
        }

        eh = packet->l2;

        memcpy(eh->eth_src, ctx->flow.dl_src, sizeof eh->eth_src);
        memcpy(eh->eth_dst, ctx->flow.dl_dst, sizeof eh->eth_dst);

        /* Support QINQ */
        for (vlan_tag_index = MAX_VLAN_TAGS - 2; vlan_tag_index >= 0; vlan_tag_index--) {
            if (ctx->flow.vlan_qinq_tci[vlan_tag_index] & htons(VLAN_CFI)) {
                eth_push_vlan(packet, ctx->flow.vlan_qinq_tci[vlan_tag_index],
                        ctx->flow.vlan_qinq_tpid[vlan_tag_index]);
            }
        }
        if (ctx->flow.vlan_tci & htons(VLAN_CFI)) {
            eth_push_vlan(packet, ctx->flow.vlan_tci, ctx->flow.vlan_tpid);
        }

        mpls_depth = eth_mpls_depth(packet);

        if (mpls_depth < ctx->flow.mpls_depth) {
            push_mpls(packet, ctx->flow.dl_type, ctx->flow.mpls_lse);
        } else if (mpls_depth > ctx->flow.mpls_depth) {
            pop_mpls(packet, ctx->flow.dl_type);
        } else if (mpls_depth) {
            set_mpls_lse(packet, ctx->flow.mpls_lse);
        }

        if (packet->l4) {
            if (ctx->flow.dl_type == htons(ETH_TYPE_IP)) {
                packet_set_ipv4(packet, ctx->flow.nw_src, ctx->flow.nw_dst,
                                ctx->flow.nw_tos, ctx->flow.nw_ttl);
            }

            if (packet->l7) {
                if (ctx->flow.nw_proto == IPPROTO_TCP) {
                    packet_set_tcp_port(packet, ctx->flow.tp_src,
                                        ctx->flow.tp_dst);
                } else if (ctx->flow.nw_proto == IPPROTO_UDP) {
                    packet_set_udp_port(packet, ctx->flow.tp_src,
                                        ctx->flow.tp_dst);
                }
            }
        }
    }

    pin.packet = packet->data;
    pin.packet_len = packet->size;
    pin.reason = reason;
    pin.controller_id = controller_id;
    pin.table_id = ctx->table_id;
    pin.cookie = ctx->rule ? ctx->rule->up.flow_cookie : 0;

    pin.send_len = len;
    flow_get_metadata(&ctx->flow, &pin.fmd);

    connmgr_send_packet_in(ctx->ofproto->up.connmgr, &pin);
    ofpbuf_delete(packet);
}

static void
execute_actions(struct ofproto_ctc *ofproto,
                          struct ofpbuf *packet, struct flow *key,
                          const struct nlattr *actions,
                          size_t actions_len)
{
    const struct nlattr *a;
    unsigned int left;
    union ofp_action ofp_actions;
    struct ofpbuf hardware_process_packet;
    int32_ofp cpu_process_status = OFP_CPU_PROCESS_DISABLE;
    uint32_ofp priority = 0;

    NL_ATTR_FOR_EACH_UNSAFE (a, left, actions, actions_len) {
        const struct ovs_action_push_vlan *vlan;
        const struct ovs_action_group *group;
        int type = nl_attr_type(a);
        switch ((enum ovs_action_attr) type) {
        case OVS_ACTION_ATTR_OUTPUT:
            ofp_actions.type = htons(OFPAT10_OUTPUT);
            ofp_actions.output10.port = htons(nl_attr_get_u32(a));
            cpu_process_status = ofp_send_packet_out(packet, key,
                                                &ofp_actions, 1, &hardware_process_packet, priority);
            if (OFP_CPU_PROCESS_DISCARD == cpu_process_status)
            {
                /* Do nothing, ignore silently. */
            }
            else if (OFP_CPU_PROCESS_ENABLE == cpu_process_status)
            {
                /* XXX: Not supported yet. */
            }
            else if (OFP_CPU_PROCESS_DISABLE == cpu_process_status)
            {
                send_packet_to_chip(ofproto, &hardware_process_packet);
                ofpbuf_uninit(&hardware_process_packet);
            }
            break;

        case OVS_ACTION_ATTR_GROUP:
            group = nl_attr_get(a);
            if (OFP_ERR_SUCCESS == ofp_send_packet_group(packet, key, ntohl(group->group_id), &hardware_process_packet))
            {
                send_packet_to_chip(ofproto, &hardware_process_packet);
                ofpbuf_uninit(&hardware_process_packet);
            }
            break;

        case OVS_ACTION_ATTR_USERSPACE:
            break;

        case OVS_ACTION_ATTR_PUSH_VLAN:
            vlan = nl_attr_get(a);
            /* Support QINQ */
            eth_push_vlan(packet, vlan->vlan_tci, vlan->vlan_tpid);
            break;

        case OVS_ACTION_ATTR_POP_VLAN:
            eth_pop_vlan(packet);
            break;

        case OVS_ACTION_ATTR_SET:
            execute_set_action(packet, nl_attr_get(a));
            execute_set_packet_priority(&priority, nl_attr_get(a));
            break;

        case OVS_ACTION_ATTR_SAMPLE:
            break;

        case OVS_ACTION_ATTR_PUSH_MPLS:
        case OVS_ACTION_ATTR_POP_MPLS:
            /* XXX: MPLS soft forwarding is not supported. */
            break;

        case OVS_ACTION_ATTR_UNSPEC:
        case __OVS_ACTION_ATTR_MAX:
            NOT_REACHED();
        }
    }
}

static int
execute(struct ofproto_ctc *ofproto,
             const struct nlattr *key, size_t key_len,
             const struct nlattr *actions, size_t actions_len,
             const struct ofpbuf *buf)
{
    struct ofpbuf copy;
    struct flow flow_key;
    int error;

    if (buf->size < ETH_HEADER_LEN ||
        buf->size > UINT16_MAX) {
        return EINVAL;
    }

    /* Make a deep copy of 'packet', because we might modify its data. */
    ofpbuf_init(&copy, CTC_NETDEV_HEADROOM + buf->size);
    ofpbuf_reserve(&copy, CTC_NETDEV_HEADROOM);
    ofpbuf_put(&copy, buf->data, buf->size);

    flow_extract(&copy, 0, 0, NULL, -1, &flow_key);
    error = flow_from_nlattrs(key, key_len, &flow_key);
    if (!error) {
        execute_actions(ofproto, &copy, &flow_key, actions, actions_len);
    }

    ofpbuf_uninit(&copy);
    return error;
}

static enum ofperr
packet_out(struct ofproto *ofproto_, struct ofpbuf *packet,
           const struct flow *flow,
           const struct ofpact *ofpacts, size_t ofpacts_len)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofproto_);
    struct odputil_keybuf keybuf;

    struct ofpbuf key;

    struct action_xlate_ctx ctx;
    uint64_t odp_actions_stub[1024 / 8];
    struct ofpbuf odp_actions;

    ofpbuf_use_stack(&key, &keybuf, sizeof keybuf);
    odp_flow_key_from_flow(&key, flow, flow->in_port);
    action_xlate_ctx_init(&ctx, ofproto, flow, flow->vlan_tci, NULL,
                          packet_get_tcp_flags(packet, flow), packet);

    ofpbuf_use_stub(&odp_actions,
                    odp_actions_stub, sizeof odp_actions_stub);
    ctx.flow_process_type = OFP_FLOW_PROCESS_TYPE_PACKET_OUT;
    ctx.any_port_flow = false;
    xlate_actions(&ctx, ofpacts, ofpacts_len, &odp_actions);
    execute(ofproto, key.data, key.size, odp_actions.data, odp_actions.size, packet);
    ofpbuf_uninit(&odp_actions);

    return 0;
}

static struct group_ctc *group_ctc_cast(const struct ofgroup *ofgroup)
{
    return ofgroup ? CONTAINER_OF(ofgroup, struct group_ctc, up) : NULL;
}

static struct ofgroup *
group_alloc(void)
{
    struct group_ctc *group = xmalloc(sizeof *group);
    return &group->up;
}

static void
group_dealloc(struct ofgroup *ofgroup)
{
    struct group_ctc *group = group_ctc_cast(ofgroup);
    free(group);
}

static int
group_translate_ofpact(struct ofgroup *ofgroup)
{
    int error = 0;
    struct ofputil_bucket *bucket;

    LIST_FOR_EACH (bucket, list_node, &ofgroup->buckets) {
        bucket->actions = xmalloc(sizeof(struct list));
        if (!bucket->actions)
        {
            continue;
        }
        error = ofp_translate_ofpact(bucket->ofpacts, bucket->ofpacts_len, bucket->actions);
        if (error) {
            return error;
        }
    }

    return error;
}

static int
group_destory_ofpact(struct ofgroup *ofgroup)
{
    int error = 0;
    struct ofputil_bucket *bucket;

    LIST_FOR_EACH (bucket, list_node, &ofgroup->buckets) {
        if (!bucket->actions)
        {
            continue;
        }
        ofp_destroy_flow_actions(bucket->actions);
        free(bucket->actions);
        bucket->actions = NULL;
    }

    return error;
}

static int
group_construct(struct ofgroup *ofgroup)
{
    struct group_ctc *group = group_ctc_cast(ofgroup);
    enum ofperr ofp_error = 0;
    int error = 0;

    error = group_translate_ofpact(ofgroup);

    /** TODO not implemented*/
    /*error = error ? error : ofp_add_group(group);*/
    ofp_error = translate_adpt_error_code(error, OFP_TYPE_GROUP);

    group_destory_ofpact(ofgroup);

    return ofp_error;
}

static int
group_modify(struct ofgroup *ofgroup, struct ofgroup *victim_)
{
    struct group_ctc *group = group_ctc_cast(ofgroup);
    struct group_ctc *victim = group_ctc_cast(victim_);
    enum ofperr ofp_error = 0;
    int error = 0;

    error = group_translate_ofpact(ofgroup);
    group_translate_ofpact(victim_);

    /** TODO not implemented*/
    /* error = error ? error : ofp_modify_group(group, victim); */
    ofp_error = translate_adpt_error_code(error, OFP_TYPE_GROUP);

    group_destory_ofpact(ofgroup);
    group_destory_ofpact(victim_);

    return ofp_error;
}

static bool
rule_has_group_id(const struct rule *rule, uint32_t group_id)
{

    const struct ofpact *a;

    OFPACT_FOR_EACH (a, rule->ofpacts, rule->ofpacts_len) {
        if (OFPACT_GROUP == a->type
            && ofpact_get_GROUP(a)->group_id == group_id) {
            return true;
        }
    }

    return false;
}

static int
collect_rules_loose_group(struct ofproto_ctc *ofproto,
                      const struct match *match,
                      uint32_t group_id, struct list *rules)
{
    struct oftable *table;
    struct cls_rule cr;
    enum ofperr error = 0;
    ovs_be64 cookie = 0;
    ovs_be64 cookie_mask = 0;

    list_init(rules);
    cls_rule_init(&cr, match, 0);

    OFPROTO_FOR_EACH_TABLE (table, &ofproto->up) {
        struct cls_cursor cursor;
        struct rule *rule;

        cls_cursor_init(&cursor, &table->cls, NULL);
        CLS_CURSOR_FOR_EACH (rule, cr, &cursor) {
            if (rule->pending) {
                error = OFPROTO_POSTPONE;
                goto exit;
            }
            if (!ofproto_rule_is_hidden(rule)
                && rule_has_group_id(rule, group_id)
                    && !((rule->flow_cookie ^ cookie) & cookie_mask)) {
                list_push_back(rules, &rule->ofproto_node);
            }
        }
    }

exit:
    cls_rule_destroy(&cr);
    return error;
}

static int
remove_rules_by_group__(struct ofproto_ctc *ofproto, uint32_t group_id)
{
    /* Remove all rules which refer this group_id */
    struct list rules;
    struct match match;
    struct rule *rule = NULL;
    struct rule *next = NULL;
    int error;


    list_init(&rules);
    match_init_catchall(&match);
    error = collect_rules_loose_group(ofproto, &match,
                                group_id, &rules);
    if (error || list_is_empty(&rules))
    {
        return error;
    }

    LIST_FOR_EACH_SAFE (rule, next, ofproto_node, &rules) {
        ofproto_rule_expire(rule, OFPRR_GROUP_DELETE);
    }

    return 0;
}

static int
group_destruct(struct ofgroup *ofgroup)
{
    struct group_ctc *group = group_ctc_cast(ofgroup);
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(ofgroup->ofproto);
    enum ofperr ofp_error = 0;
    int error = 0;
    uint32_t ref_flow  = 0;
    uint32_t ref_group = 0;

    /** TODO not implemented*/
    /* ofp_group_get_ref_cnt(ofgroup->group_id, &ref_flow, &ref_group); */
    if (ref_group) {
        VLOG_ERR("Failed to delete group %u, it is referenced by other groups\n",
            ofgroup->group_id);
        return OFPERR_OFPGMFC_CHAINED_GROUP;
    }

    remove_rules_by_group__(ofproto, ofgroup->group_id);

    /** TODO not implemented*/
    /* error = ofp_del_group(group); */
    ofp_error = translate_adpt_error_code(error, OFP_TYPE_GROUP);

    return ofp_error;
}

/* Get group stats */
static int
group_get_stats(const struct ofgroup *ofgroup,
                          struct ofputil_group_stats *stats)
{
    struct group_ctc *group = group_ctc_cast(ofgroup);
    struct group_stats group_stats;
    size_t bucket_i;

    memset(&group_stats, 0, sizeof(group_stats));

    /** TODO not implemented*/
    /* ofp_get_group_stats(ofgroup->group_id, &group_stats); */

    stats->group_id = group->up.group_id;
    stats->packet_count = group_stats.packet_count;
    stats->byte_count = group_stats.byte_count;

    stats->n_buckets = group_stats.n_buckets;
    for (bucket_i = 0; bucket_i < group_stats.n_buckets; bucket_i++) {
        stats->bucket_stats[bucket_i].packet_count = group_stats.buckets_stats[bucket_i].packet_count;
        stats->bucket_stats[bucket_i].byte_count = group_stats.buckets_stats[bucket_i].byte_count;
    }

    return 0;
}

static int
group_get_ref_cnt(const struct ofgroup *ofgroup,
                      uint32_t *ref_cnt)
{
    uint32_t ref_flow  = 0;
    uint32_t ref_group = 0;
    if (!ref_cnt) {
        return 0;
    }

    *ref_cnt = 0;

    /** TODO not implemented*/
    /* ofp_group_get_ref_cnt(ofgroup->group_id, &ref_flow, &ref_group); */
    *ref_cnt = ref_flow + ref_group;

    return 0;
}

/* Meter */
static struct meter_ctc *meter_ctc_cast(const struct ofmeter *ofmeter)
{
    return ofmeter ? CONTAINER_OF(ofmeter, struct meter_ctc, up) : NULL;
}

static struct ofmeter *
meter_alloc(void)
{
    struct meter_ctc * meter = xmalloc(sizeof *meter);
    return &meter->up;
}

static void
meter_dealloc(struct ofmeter *meter_)
{
    struct meter_ctc *meter = meter_ctc_cast(meter_);
    free(meter);
}

static int
meter_construct(struct ofmeter *meter_)
{
    int error;
    int ofp_error;

    struct meter_ctc *meter = meter_ctc_cast(meter_);
    /** TODO not implemented*/
    /* error = ofp_add_meter(meter); */

    ofp_error = translate_adpt_error_code(error, OFP_TYPE_METER);

    return ofp_error;
}

static int
meter_destruct(struct ofmeter *meter_)
{
    int error;

    struct meter_ctc *meter = meter_ctc_cast(meter_);
    /** TODO not implemented*/
    /*error = ofp_del_meter(meter);*/

    if (error) {
        VLOG_ERR("Remove meter failed");
        return error;
    }

    return 0;
}

static int
meter_get_stats(struct ofmeter *meter_, struct ofputil_meter_stats* of_meter_stats)
{
    int error;
    struct meter_stats_ctc meter_stats;

    struct meter_ctc *meter = meter_ctc_cast(meter_);
    /** TODO not implemented*/
    /* error = ofp_get_meter_stats(meter, &meter_stats); */

    of_meter_stats->byte_in_count = meter_stats.byte_in_count;
    of_meter_stats->packet_in_count = meter_stats.packet_in_count;
    of_meter_stats->n_bands = meter_stats.n_bands;
    memcpy(of_meter_stats->bands_stats, meter_stats.bands_stats, sizeof(struct meter_band_stats_ctc) * meter_stats.n_bands);

    if (error) {
        VLOG_ERR("Get meter stats failed");
        return error;
    }

    return 0;
}

static int
meter_update(struct ofmeter *meter_)
{
    int error;
    int ofp_error;

    struct meter_ctc *meter = meter_ctc_cast(meter_);
    /** TODO not implemented*/
    /* error = ofp_update_meter(meter); */

    ofp_error = translate_adpt_error_code(error, OFP_TYPE_METER);

    return ofp_error;
}

static int
meter_get_refcnt(struct ofmeter *meter_, uint32_t* refcnt)
{
    int error;

    struct meter_ctc *meter = meter_ctc_cast(meter_);
    /** TODO not implemented*/
    /* error = ofp_get_meter_refcnt(meter, refcnt); */
    if (error) {
        VLOG_ERR("Get meter refcnt failed");
        return error;
    }

    return 0;
}

static void
meter_get_features(struct ofproto *ofproto, uint32_t* max_meter, uint32_t* min_meter, uint8_t* max_bands)
{
    ofproto = ofproto; /* TODO unused parameter */
    /** TODO not implemented*/
    /* ofp_meter_get_features(max_meter, min_meter, max_bands); */
    return;
}

static int
queue_get_config(uint16_t ofport, uint8_t queue_id, uint16_t *min_rate, uint16_t *max_rate)
{
    int error;

    /** TODO not implemented*/
    /* error = ofp_port_get_queue_info_by_ofport(ofport, queue_id, min_rate, max_rate); */
    if (error) {
        VLOG_ERR("Get queue config failed");
        return error;
    }

    return 0;
}

const static struct ofproto_class ofproto_ctc_class = {
    init,
    enumerate_types,
    enumerate_names,
    del,
    NULL,                       /* port_open_type */
    NULL,                       /* type_run */
    NULL,                       /* type_run_fast */
    NULL,                       /* type_wait */
    alloc,
    construct,
    destruct,
    dealloc,
    run,
    run_fast,
    wait__,
    NULL,                       /* get_memory_usage */
    NULL,                       /* flush */
    get_features,
    get_capabilities,
    get_datapath_id,
    initial_cfg_done,
    get_tables,
    port_alloc,
    port_construct,
    port_destruct,
    port_dealloc,
    port_modified,
    port_reconfigured,
    port_query_by_name,
    port_add,
    port_del,
    port_get_stats,
    port_dump_start,
    port_dump_next,
    port_dump_done,
    port_poll,
    NULL,                       /* port_poll_wait */
    NULL,                       /* port_is_lacp_current */
    NULL,                       /* rule_choose_table */
    rule_alloc,
    rule_construct,
    rule_destruct,
    rule_dealloc,
    rule_get_stats,
    rule_clear_stats,
    rule_execute,
    rule_modify_actions,
    set_frag_handling,
    packet_out,
    NULL,                       /* Legacy L2 features... */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,                       /* Legacy L2 features... */
    group_alloc,
    group_dealloc,
    group_construct,
    group_modify,
    group_destruct,
    group_get_stats,
    group_get_ref_cnt,
    meter_alloc,
    meter_construct,
    meter_destruct,
    meter_dealloc,
    meter_get_stats,
    meter_update,
    meter_get_refcnt,
    meter_get_features,
    queue_get_config,
};

extern struct ofproto_class ofproto_live_class;
void register_ofproto_live_class(void)
{
    memcpy(&ofproto_live_class, &ofproto_ctc_class,
           sizeof(struct ofproto_class));
}
