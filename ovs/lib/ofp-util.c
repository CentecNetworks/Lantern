/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013 Nicira, Inc.
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
 */

#include <config.h>
#include "ofp-print.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <stdlib.h>
#include "bundle.h"
#include "byte-order.h"
#include "classifier.h"
#include "dynamic-string.h"
#include "learn.h"
#include "meta-flow.h"
#include "multipath.h"
#include "netdev.h"
#include "nx-match.h"
#include "ofp-actions.h"
#include "ofp-errors.h"
#include "ofp-msgs.h"
#include "ofp-util.h"
#include "ofpbuf.h"
#include "packets.h"
#include "random.h"
#include "unaligned.h"
#include "type-props.h"
#include "vlog.h"
#include "timeval.h"

VLOG_DEFINE_THIS_MODULE(ofp_util);

/* Rate limit for OpenFlow message parse errors.  These always indicate a bug
 * in the peer and so there's not much point in showing a lot of them. */
static struct vlog_rate_limit bad_ofmsg_rl = VLOG_RATE_LIMIT_INIT(1, 5);

/* Given the wildcard bit count in the least-significant 6 of 'wcbits', returns
 * an IP netmask with a 1 in each bit that must match and a 0 in each bit that
 * is wildcarded.
 *
 * The bits in 'wcbits' are in the format used in enum ofp_flow_wildcards: 0
 * is exact match, 1 ignores the LSB, 2 ignores the 2 least-significant bits,
 * ..., 32 and higher wildcard the entire field.  This is the *opposite* of the
 * usual convention where e.g. /24 indicates that 8 bits (not 24 bits) are
 * wildcarded. */
ovs_be32
ofputil_wcbits_to_netmask(int wcbits)
{
    wcbits &= 0x3f;
    return wcbits < 32 ? htonl(~((1u << wcbits) - 1)) : 0;
}

/* Given the IP netmask 'netmask', returns the number of bits of the IP address
 * that it wildcards, that is, the number of 0-bits in 'netmask', a number
 * between 0 and 32 inclusive.
 *
 * If 'netmask' is not a CIDR netmask (see ip_is_cidr()), the return value will
 * still be in the valid range but isn't otherwise meaningful. */
int
ofputil_netmask_to_wcbits(ovs_be32 netmask)
{
    return 32 - ip_count_cidr_bits(netmask);
}

/* Converts the OpenFlow 1.0 wildcards in 'ofpfw' (OFPFW10_*) into a
 * flow_wildcards in 'wc' for use in struct match.  It is the caller's
 * responsibility to handle the special case where the flow match's dl_vlan is
 * set to OFP_VLAN_NONE. */
void
ofputil_wildcard_from_ofpfw10(uint32_t ofpfw, struct flow_wildcards *wc)
{
    BUILD_ASSERT_DECL(FLOW_WC_SEQ == 19);

    /* Initialize most of wc. */
    flow_wildcards_init_catchall(wc);

    if (!(ofpfw & OFPFW10_IN_PORT)) {
        wc->masks.in_port = UINT16_MAX;
    }

    if (!(ofpfw & OFPFW10_NW_TOS)) {
        wc->masks.nw_tos |= IP_DSCP_MASK;
    }

    if (!(ofpfw & OFPFW10_NW_PROTO)) {
        wc->masks.nw_proto = UINT8_MAX;
    }
    wc->masks.nw_src = ofputil_wcbits_to_netmask(ofpfw
                                                 >> OFPFW10_NW_SRC_SHIFT);
    wc->masks.nw_dst = ofputil_wcbits_to_netmask(ofpfw
                                                 >> OFPFW10_NW_DST_SHIFT);

    if (!(ofpfw & OFPFW10_TP_SRC)) {
        wc->masks.tp_src = htons(UINT16_MAX);
    }
    if (!(ofpfw & OFPFW10_TP_DST)) {
        wc->masks.tp_dst = htons(UINT16_MAX);
    }

    if (!(ofpfw & OFPFW10_DL_SRC)) {
        memset(wc->masks.dl_src, 0xff, ETH_ADDR_LEN);
    }
    if (!(ofpfw & OFPFW10_DL_DST)) {
        memset(wc->masks.dl_dst, 0xff, ETH_ADDR_LEN);
    }
    if (!(ofpfw & OFPFW10_DL_TYPE)) {
        wc->masks.dl_type = htons(UINT16_MAX);
    }

    /* VLAN TCI mask. */
    if (!(ofpfw & OFPFW10_DL_VLAN_PCP)) {
        wc->masks.vlan_tci |= htons(VLAN_PCP_MASK | VLAN_CFI);
    }
    if (!(ofpfw & OFPFW10_DL_VLAN)) {
        wc->masks.vlan_tci |= htons(VLAN_VID_MASK | VLAN_CFI);
    }
}

/* Converts the ofp10_match in 'ofmatch' into a struct match in 'match'. */
void
ofputil_match_from_ofp10_match(const struct ofp10_match *ofmatch,
                               struct match *match)
{
    uint32_t ofpfw = ntohl(ofmatch->wildcards) & OFPFW10_ALL;

    /* Initialize match->wc. */
    memset(&match->flow, 0, sizeof match->flow);
    ofputil_wildcard_from_ofpfw10(ofpfw, &match->wc);

    /* Initialize most of match->flow. */
    match->flow.nw_src = ofmatch->nw_src;
    match->flow.nw_dst = ofmatch->nw_dst;
    match->flow.in_port = ntohs(ofmatch->in_port);
    match->flow.dl_type = ofputil_dl_type_from_openflow(ofmatch->dl_type);
    match->flow.tp_src = ofmatch->tp_src;
    match->flow.tp_dst = ofmatch->tp_dst;
    memcpy(match->flow.dl_src, ofmatch->dl_src, ETH_ADDR_LEN);
    memcpy(match->flow.dl_dst, ofmatch->dl_dst, ETH_ADDR_LEN);
    match->flow.nw_tos = ofmatch->nw_tos & IP_DSCP_MASK;
    match->flow.nw_proto = ofmatch->nw_proto;

    /* Translate VLANs. */
    if (!(ofpfw & OFPFW10_DL_VLAN) &&
        ofmatch->dl_vlan == htons(OFP10_VLAN_NONE)) {
        /* Match only packets without 802.1Q header.
         *
         * When OFPFW10_DL_VLAN_PCP is wildcarded, this is obviously correct.
         *
         * If OFPFW10_DL_VLAN_PCP is matched, the flow match is contradictory,
         * because we can't have a specific PCP without an 802.1Q header.
         * However, older versions of OVS treated this as matching packets
         * withut an 802.1Q header, so we do here too. */
        match->flow.vlan_tci = htons(0);
        match->wc.masks.vlan_tci = htons(0xffff);
    } else {
        ovs_be16 vid, pcp, tci;

        vid = ofmatch->dl_vlan & htons(VLAN_VID_MASK);
        pcp = htons((ofmatch->dl_vlan_pcp << VLAN_PCP_SHIFT) & VLAN_PCP_MASK);
        tci = vid | pcp | htons(VLAN_CFI);
        match->flow.vlan_tci = tci & match->wc.masks.vlan_tci;
    }

    /* Clean up. */
    match_zero_wildcarded_fields(match);
}

/* Convert 'match' into the OpenFlow 1.0 match structure 'ofmatch'. */
void
ofputil_match_to_ofp10_match(const struct match *match,
                             struct ofp10_match *ofmatch)
{
    const struct flow_wildcards *wc = &match->wc;
    uint32_t ofpfw;

    /* Figure out most OpenFlow wildcards. */
    ofpfw = 0;
    if (!wc->masks.in_port) {
        ofpfw |= OFPFW10_IN_PORT;
    }
    if (!wc->masks.dl_type) {
        ofpfw |= OFPFW10_DL_TYPE;
    }
    if (!wc->masks.nw_proto) {
        ofpfw |= OFPFW10_NW_PROTO;
    }
    ofpfw |= (ofputil_netmask_to_wcbits(wc->masks.nw_src)
              << OFPFW10_NW_SRC_SHIFT);
    ofpfw |= (ofputil_netmask_to_wcbits(wc->masks.nw_dst)
              << OFPFW10_NW_DST_SHIFT);
    if (!(wc->masks.nw_tos & IP_DSCP_MASK)) {
        ofpfw |= OFPFW10_NW_TOS;
    }
    if (!wc->masks.tp_src) {
        ofpfw |= OFPFW10_TP_SRC;
    }
    if (!wc->masks.tp_dst) {
        ofpfw |= OFPFW10_TP_DST;
    }
    if (eth_addr_is_zero(wc->masks.dl_src)) {
        ofpfw |= OFPFW10_DL_SRC;
    }
    if (eth_addr_is_zero(wc->masks.dl_dst)) {
        ofpfw |= OFPFW10_DL_DST;
    }

    /* Translate VLANs. */
    ofmatch->dl_vlan = htons(0);
    ofmatch->dl_vlan_pcp = 0;
    if (match->wc.masks.vlan_tci == htons(0)) {
        ofpfw |= OFPFW10_DL_VLAN | OFPFW10_DL_VLAN_PCP;
    } else if (match->wc.masks.vlan_tci & htons(VLAN_CFI)
               && !(match->flow.vlan_tci & htons(VLAN_CFI))) {
        ofmatch->dl_vlan = htons(OFP10_VLAN_NONE);
#ifndef _OFP_CENTEC_
        /* OVS bug, should not set this bit, see old version. */
        ofpfw |= OFPFW10_DL_VLAN_PCP;
#endif
    } else {
        if (!(match->wc.masks.vlan_tci & htons(VLAN_VID_MASK))) {
            ofpfw |= OFPFW10_DL_VLAN;
        } else {
            ofmatch->dl_vlan = htons(vlan_tci_to_vid(match->flow.vlan_tci));
        }

        if (!(match->wc.masks.vlan_tci & htons(VLAN_PCP_MASK))) {
            ofpfw |= OFPFW10_DL_VLAN_PCP;
        } else {
            ofmatch->dl_vlan_pcp = vlan_tci_to_pcp(match->flow.vlan_tci);
        }
    }

    /* Compose most of the match structure. */
    ofmatch->wildcards = htonl(ofpfw);
    ofmatch->in_port = htons(match->flow.in_port);
    memcpy(ofmatch->dl_src, match->flow.dl_src, ETH_ADDR_LEN);
    memcpy(ofmatch->dl_dst, match->flow.dl_dst, ETH_ADDR_LEN);
    ofmatch->dl_type = ofputil_dl_type_to_openflow(match->flow.dl_type);
    ofmatch->nw_src = match->flow.nw_src;
    ofmatch->nw_dst = match->flow.nw_dst;
    ofmatch->nw_tos = match->flow.nw_tos & IP_DSCP_MASK;
    ofmatch->nw_proto = match->flow.nw_proto;
    ofmatch->tp_src = match->flow.tp_src;
    ofmatch->tp_dst = match->flow.tp_dst;
    memset(ofmatch->pad1, '\0', sizeof ofmatch->pad1);
    memset(ofmatch->pad2, '\0', sizeof ofmatch->pad2);
}

enum ofperr
ofputil_pull_ofp11_match(struct ofpbuf *buf, struct match *match,
                         uint16_t *padded_match_len)
{
    struct ofp11_match_header *omh = buf->data;
    uint16_t match_len;

    if (buf->size < sizeof *omh) {
        return OFPERR_OFPBMC_BAD_LEN;
    }

    match_len = ntohs(omh->length);

    switch (ntohs(omh->type)) {
    case OFPMT_STANDARD: {
        struct ofp11_match *om;

        if (match_len != sizeof *om || buf->size < sizeof *om) {
            return OFPERR_OFPBMC_BAD_LEN;
        }
        om = ofpbuf_pull(buf, sizeof *om);
        if (padded_match_len) {
            *padded_match_len = match_len;
        }
        return ofputil_match_from_ofp11_match(om, match);
    }

    case OFPMT_OXM:
        if (padded_match_len) {
            *padded_match_len = ROUND_UP(match_len, 8);
        }
        return oxm_pull_match(buf, match);

    default:
        return OFPERR_OFPBMC_BAD_TYPE;
    }
}

/* Converts the ofp11_match in 'match' into a struct match in 'match.  Returns
 * 0 if successful, otherwise an OFPERR_* value. */
enum ofperr
ofputil_match_from_ofp11_match(const struct ofp11_match *ofmatch,
                               struct match *match)
{
    uint16_t wc = ntohl(ofmatch->wildcards);
    uint8_t dl_src_mask[ETH_ADDR_LEN];
    uint8_t dl_dst_mask[ETH_ADDR_LEN];
    bool ipv4, arp, rarp;
    int i;

    match_init_catchall(match);

    if (!(wc & OFPFW11_IN_PORT)) {
        uint16_t ofp_port;
        enum ofperr error;

        error = ofputil_port_from_ofp11(ofmatch->in_port, &ofp_port);
        if (error) {
            return OFPERR_OFPBMC_BAD_VALUE;
        }
        match_set_in_port(match, ofp_port);
    }

    for (i = 0; i < ETH_ADDR_LEN; i++) {
        dl_src_mask[i] = ~ofmatch->dl_src_mask[i];
    }
    match_set_dl_src_masked(match, ofmatch->dl_src, dl_src_mask);

    for (i = 0; i < ETH_ADDR_LEN; i++) {
        dl_dst_mask[i] = ~ofmatch->dl_dst_mask[i];
    }
    match_set_dl_dst_masked(match, ofmatch->dl_dst, dl_dst_mask);

    if (!(wc & OFPFW11_DL_VLAN)) {
        if (ofmatch->dl_vlan == htons(OFPVID11_NONE)) {
            /* Match only packets without a VLAN tag. */
            match->flow.vlan_tci = htons(0);
            match->wc.masks.vlan_tci = htons(UINT16_MAX);
        } else {
            if (ofmatch->dl_vlan == htons(OFPVID11_ANY)) {
                /* Match any packet with a VLAN tag regardless of VID. */
                match->flow.vlan_tci = htons(VLAN_CFI);
                match->wc.masks.vlan_tci = htons(VLAN_CFI);
            } else if (ntohs(ofmatch->dl_vlan) < 4096) {
                /* Match only packets with the specified VLAN VID. */
                match->flow.vlan_tci = htons(VLAN_CFI) | ofmatch->dl_vlan;
                match->wc.masks.vlan_tci = htons(VLAN_CFI | VLAN_VID_MASK);
            } else {
                /* Invalid VID. */
                return OFPERR_OFPBMC_BAD_VALUE;
            }

            if (!(wc & OFPFW11_DL_VLAN_PCP)) {
                if (ofmatch->dl_vlan_pcp <= 7) {
                    match->flow.vlan_tci |= htons(ofmatch->dl_vlan_pcp
                                                  << VLAN_PCP_SHIFT);
                    match->wc.masks.vlan_tci |= htons(VLAN_PCP_MASK);
                } else {
                    /* Invalid PCP. */
                    return OFPERR_OFPBMC_BAD_VALUE;
                }
            }
        }
    }

    if (!(wc & OFPFW11_DL_TYPE)) {
        match_set_dl_type(match,
                          ofputil_dl_type_from_openflow(ofmatch->dl_type));
    }

    ipv4 = match->flow.dl_type == htons(ETH_TYPE_IP);
    arp = match->flow.dl_type == htons(ETH_TYPE_ARP);
    rarp = match->flow.dl_type == htons(ETH_TYPE_RARP);

    if (ipv4 && !(wc & OFPFW11_NW_TOS)) {
        if (ofmatch->nw_tos & ~IP_DSCP_MASK) {
            /* Invalid TOS. */
            return OFPERR_OFPBMC_BAD_VALUE;
        }

        match_set_nw_dscp(match, ofmatch->nw_tos);
    }

    if (ipv4 || arp || rarp) {
        if (!(wc & OFPFW11_NW_PROTO)) {
            match_set_nw_proto(match, ofmatch->nw_proto);
        }
        match_set_nw_src_masked(match, ofmatch->nw_src, ~ofmatch->nw_src_mask);
        match_set_nw_dst_masked(match, ofmatch->nw_dst, ~ofmatch->nw_dst_mask);
    }

#define OFPFW11_TP_ALL (OFPFW11_TP_SRC | OFPFW11_TP_DST)
    if (ipv4 && (wc & OFPFW11_TP_ALL) != OFPFW11_TP_ALL) {
        switch (match->flow.nw_proto) {
        case IPPROTO_ICMP:
            /* "A.2.3 Flow Match Structures" in OF1.1 says:
             *
             *    The tp_src and tp_dst fields will be ignored unless the
             *    network protocol specified is as TCP, UDP or SCTP.
             *
             * but I'm pretty sure we should support ICMP too, otherwise
             * that's a regression from OF1.0. */
            if (!(wc & OFPFW11_TP_SRC)) {
                uint16_t icmp_type = ntohs(ofmatch->tp_src);
                if (icmp_type < 0x100) {
                    match_set_icmp_type(match, icmp_type);
                } else {
                    return OFPERR_OFPBMC_BAD_FIELD;
                }
            }
            if (!(wc & OFPFW11_TP_DST)) {
                uint16_t icmp_code = ntohs(ofmatch->tp_dst);
                if (icmp_code < 0x100) {
                    match_set_icmp_code(match, icmp_code);
                } else {
                    return OFPERR_OFPBMC_BAD_FIELD;
                }
            }
            break;

        case IPPROTO_TCP:
        case IPPROTO_UDP:
            if (!(wc & (OFPFW11_TP_SRC))) {
                match_set_tp_src(match, ofmatch->tp_src);
            }
            if (!(wc & (OFPFW11_TP_DST))) {
                match_set_tp_dst(match, ofmatch->tp_dst);
            }
            break;

        case IPPROTO_SCTP:
            /* We don't support SCTP and it seems that we should tell the
             * controller, since OF1.1 implementations are supposed to. */
            return OFPERR_OFPBMC_BAD_FIELD;

        default:
            /* OF1.1 says explicitly to ignore this. */
            break;
        }
    }

    if (eth_type_mpls(match->flow.dl_type)) {
        enum { OFPFW11_MPLS_ALL = OFPFW11_MPLS_LABEL | OFPFW11_MPLS_TC };

        if ((wc & OFPFW11_MPLS_ALL) != OFPFW11_MPLS_ALL) {
            /* MPLS not supported. */
            return OFPERR_OFPBMC_BAD_TAG;
        }
    }

    match_set_metadata_masked(match, ofmatch->metadata,
                              ~ofmatch->metadata_mask);

    return 0;
}

/* Convert 'match' into the OpenFlow 1.1 match structure 'ofmatch'. */
void
ofputil_match_to_ofp11_match(const struct match *match,
                             struct ofp11_match *ofmatch)
{
    uint32_t wc = 0;
    int i;

    memset(ofmatch, 0, sizeof *ofmatch);
    ofmatch->omh.type = htons(OFPMT_STANDARD);
    ofmatch->omh.length = htons(OFPMT11_STANDARD_LENGTH);

    if (!match->wc.masks.in_port) {
        wc |= OFPFW11_IN_PORT;
    } else {
        ofmatch->in_port = ofputil_port_to_ofp11(match->flow.in_port);
    }

    memcpy(ofmatch->dl_src, match->flow.dl_src, ETH_ADDR_LEN);
    for (i = 0; i < ETH_ADDR_LEN; i++) {
        ofmatch->dl_src_mask[i] = ~match->wc.masks.dl_src[i];
    }

    memcpy(ofmatch->dl_dst, match->flow.dl_dst, ETH_ADDR_LEN);
    for (i = 0; i < ETH_ADDR_LEN; i++) {
        ofmatch->dl_dst_mask[i] = ~match->wc.masks.dl_dst[i];
    }

    if (match->wc.masks.vlan_tci == htons(0)) {
        wc |= OFPFW11_DL_VLAN | OFPFW11_DL_VLAN_PCP;
    } else if (match->wc.masks.vlan_tci & htons(VLAN_CFI)
               && !(match->flow.vlan_tci & htons(VLAN_CFI))) {
        ofmatch->dl_vlan = htons(OFPVID11_NONE);
        wc |= OFPFW11_DL_VLAN_PCP;
    } else {
        if (!(match->wc.masks.vlan_tci & htons(VLAN_VID_MASK))) {
            ofmatch->dl_vlan = htons(OFPVID11_ANY);
        } else {
            ofmatch->dl_vlan = htons(vlan_tci_to_vid(match->flow.vlan_tci));
        }

        if (!(match->wc.masks.vlan_tci & htons(VLAN_PCP_MASK))) {
            wc |= OFPFW11_DL_VLAN_PCP;
        } else {
            ofmatch->dl_vlan_pcp = vlan_tci_to_pcp(match->flow.vlan_tci);
        }
    }

    if (!match->wc.masks.dl_type) {
        wc |= OFPFW11_DL_TYPE;
    } else {
        ofmatch->dl_type = ofputil_dl_type_to_openflow(match->flow.dl_type);
    }

    if (!(match->wc.masks.nw_tos & IP_DSCP_MASK)) {
        wc |= OFPFW11_NW_TOS;
    } else {
        ofmatch->nw_tos = match->flow.nw_tos & IP_DSCP_MASK;
    }

    if (!match->wc.masks.nw_proto) {
        wc |= OFPFW11_NW_PROTO;
    } else {
        ofmatch->nw_proto = match->flow.nw_proto;
    }

    ofmatch->nw_src = match->flow.nw_src;
    ofmatch->nw_src_mask = ~match->wc.masks.nw_src;
    ofmatch->nw_dst = match->flow.nw_dst;
    ofmatch->nw_dst_mask = ~match->wc.masks.nw_dst;

    if (!match->wc.masks.tp_src) {
        wc |= OFPFW11_TP_SRC;
    } else {
        ofmatch->tp_src = match->flow.tp_src;
    }

    if (!match->wc.masks.tp_dst) {
        wc |= OFPFW11_TP_DST;
    } else {
        ofmatch->tp_dst = match->flow.tp_dst;
    }

    /* MPLS not supported. */
    wc |= OFPFW11_MPLS_LABEL;
    wc |= OFPFW11_MPLS_TC;

    ofmatch->metadata = match->flow.metadata;
    ofmatch->metadata_mask = ~match->wc.masks.metadata;

    ofmatch->wildcards = htonl(wc);
}

/* Given a 'dl_type' value in the format used in struct flow, returns the
 * corresponding 'dl_type' value for use in an ofp10_match or ofp11_match
 * structure. */
ovs_be16
ofputil_dl_type_to_openflow(ovs_be16 flow_dl_type)
{
    return (flow_dl_type == htons(FLOW_DL_TYPE_NONE)
            ? htons(OFP_DL_TYPE_NOT_ETH_TYPE)
            : flow_dl_type);
}

/* Given a 'dl_type' value in the format used in an ofp10_match or ofp11_match
 * structure, returns the corresponding 'dl_type' value for use in struct
 * flow. */
ovs_be16
ofputil_dl_type_from_openflow(ovs_be16 ofp_dl_type)
{
    return (ofp_dl_type == htons(OFP_DL_TYPE_NOT_ETH_TYPE)
            ? htons(FLOW_DL_TYPE_NONE)
            : ofp_dl_type);
}

/* Protocols. */

struct proto_abbrev {
    enum ofputil_protocol protocol;
    const char *name;
};

/* Most users really don't care about some of the differences between
 * protocols.  These abbreviations help with that. */
static const struct proto_abbrev proto_abbrevs[] = {
    { OFPUTIL_P_ANY,          "any" },
    { OFPUTIL_P_OF10_STD_ANY, "OpenFlow10" },
    { OFPUTIL_P_OF10_NXM_ANY, "NXM" },
    { OFPUTIL_P_ANY_OXM,      "OXM" },
};
#define N_PROTO_ABBREVS ARRAY_SIZE(proto_abbrevs)

enum ofputil_protocol ofputil_flow_dump_protocols[] = {
    OFPUTIL_P_OF13_OXM,
    OFPUTIL_P_OF12_OXM,
    OFPUTIL_P_OF10_NXM,
    OFPUTIL_P_OF10_STD,
};
size_t ofputil_n_flow_dump_protocols = ARRAY_SIZE(ofputil_flow_dump_protocols);

/* Returns the set of ofputil_protocols that are supported with the given
 * OpenFlow 'version'.  'version' should normally be an 8-bit OpenFlow version
 * identifier (e.g. 0x01 for OpenFlow 1.0, 0x02 for OpenFlow 1.1).  Returns 0
 * if 'version' is not supported or outside the valid range.  */
enum ofputil_protocol
ofputil_protocols_from_ofp_version(enum ofp_version version)
{
    switch (version) {
    case OFP10_VERSION:
        return OFPUTIL_P_OF10_STD_ANY | OFPUTIL_P_OF10_NXM_ANY;
    case OFP12_VERSION:
        return OFPUTIL_P_OF12_OXM;
    case OFP13_VERSION:
        return OFPUTIL_P_OF13_OXM;
    case OFP11_VERSION:
    default:
        return 0;
    }
}

/* Returns the ofputil_protocol that is initially in effect on an OpenFlow
 * connection that has negotiated the given 'version'.  'version' should
 * normally be an 8-bit OpenFlow version identifier (e.g. 0x01 for OpenFlow
 * 1.0, 0x02 for OpenFlow 1.1).  Returns 0 if 'version' is not supported or
 * outside the valid range.  */
enum ofputil_protocol
ofputil_protocol_from_ofp_version(enum ofp_version version)
{
    return rightmost_1bit(ofputil_protocols_from_ofp_version(version));
}

/* Returns the OpenFlow protocol version number (e.g. OFP10_VERSION,
 * etc.) that corresponds to 'protocol'. */
enum ofp_version
ofputil_protocol_to_ofp_version(enum ofputil_protocol protocol)
{
    switch (protocol) {
    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID:
    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID:
        return OFP10_VERSION;
    case OFPUTIL_P_OF12_OXM:
        return OFP12_VERSION;
    case OFPUTIL_P_OF13_OXM:
        return OFP13_VERSION;
    }

    NOT_REACHED();
}

/* Returns a bitmap of OpenFlow versions that are supported by at
 * least one of the 'protocols'. */
uint32_t
ofputil_protocols_to_version_bitmap(enum ofputil_protocol protocols)
{
    uint32_t bitmap = 0;

    for (; protocols; protocols = zero_rightmost_1bit(protocols)) {
        enum ofputil_protocol protocol = rightmost_1bit(protocols);

        bitmap |= 1u << ofputil_protocol_to_ofp_version(protocol);
    }

    return bitmap;
}

/* Returns the set of protocols that are supported on top of the
 * OpenFlow versions included in 'bitmap'. */
enum ofputil_protocol
ofputil_protocols_from_version_bitmap(uint32_t bitmap)
{
    enum ofputil_protocol protocols = 0;

    for (; bitmap; bitmap = zero_rightmost_1bit(bitmap)) {
        enum ofp_version version = rightmost_1bit_idx(bitmap);

        protocols |= ofputil_protocols_from_ofp_version(version);
    }

    return protocols;
}

/* Returns true if 'protocol' is a single OFPUTIL_P_* value, false
 * otherwise. */
bool
ofputil_protocol_is_valid(enum ofputil_protocol protocol)
{
    return protocol & OFPUTIL_P_ANY && is_pow2(protocol);
}

/* Returns the equivalent of 'protocol' with the Nicira flow_mod_table_id
 * extension turned on or off if 'enable' is true or false, respectively.
 *
 * This extension is only useful for protocols whose "standard" version does
 * not allow specific tables to be modified.  In particular, this is true of
 * OpenFlow 1.0.  In later versions of OpenFlow, a flow_mod request always
 * specifies a table ID and so there is no need for such an extension.  When
 * 'protocol' is such a protocol that doesn't need a flow_mod_table_id
 * extension, this function just returns its 'protocol' argument unchanged
 * regardless of the value of 'enable'.  */
enum ofputil_protocol
ofputil_protocol_set_tid(enum ofputil_protocol protocol, bool enable)
{
    switch (protocol) {
    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID:
        return enable ? OFPUTIL_P_OF10_STD_TID : OFPUTIL_P_OF10_STD;

    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID:
        return enable ? OFPUTIL_P_OF10_NXM_TID : OFPUTIL_P_OF10_NXM;

    case OFPUTIL_P_OF12_OXM:
        return OFPUTIL_P_OF12_OXM;

    case OFPUTIL_P_OF13_OXM:
        return OFPUTIL_P_OF13_OXM;

    default:
        NOT_REACHED();
    }
}

/* Returns the "base" version of 'protocol'.  That is, if 'protocol' includes
 * some extension to a standard protocol version, the return value is the
 * standard version of that protocol without any extension.  If 'protocol' is a
 * standard protocol version, returns 'protocol' unchanged. */
enum ofputil_protocol
ofputil_protocol_to_base(enum ofputil_protocol protocol)
{
    return ofputil_protocol_set_tid(protocol, false);
}

/* Returns 'new_base' with any extensions taken from 'cur'. */
enum ofputil_protocol
ofputil_protocol_set_base(enum ofputil_protocol cur,
                          enum ofputil_protocol new_base)
{
    bool tid = (cur & OFPUTIL_P_TID) != 0;

    switch (new_base) {
    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID:
        return ofputil_protocol_set_tid(OFPUTIL_P_OF10_STD, tid);

    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID:
        return ofputil_protocol_set_tid(OFPUTIL_P_OF10_NXM, tid);

    case OFPUTIL_P_OF12_OXM:
        return ofputil_protocol_set_tid(OFPUTIL_P_OF12_OXM, tid);

    case OFPUTIL_P_OF13_OXM:
        return ofputil_protocol_set_tid(OFPUTIL_P_OF13_OXM, tid);

    default:
        NOT_REACHED();
    }
}

/* Returns a string form of 'protocol', if a simple form exists (that is, if
 * 'protocol' is either a single protocol or it is a combination of protocols
 * that have a single abbreviation).  Otherwise, returns NULL. */
const char *
ofputil_protocol_to_string(enum ofputil_protocol protocol)
{
    const struct proto_abbrev *p;

    /* Use a "switch" statement for single-bit names so that we get a compiler
     * warning if we forget any. */
    switch (protocol) {
    case OFPUTIL_P_OF10_NXM:
        return "NXM-table_id";

    case OFPUTIL_P_OF10_NXM_TID:
        return "NXM+table_id";

    case OFPUTIL_P_OF10_STD:
        return "OpenFlow10-table_id";

    case OFPUTIL_P_OF10_STD_TID:
        return "OpenFlow10+table_id";

    case OFPUTIL_P_OF12_OXM:
        return "OXM-OpenFlow12";

    case OFPUTIL_P_OF13_OXM:
        return "OXM-OpenFlow13";
    }

    /* Check abbreviations. */
    for (p = proto_abbrevs; p < &proto_abbrevs[N_PROTO_ABBREVS]; p++) {
        if (protocol == p->protocol) {
            return p->name;
        }
    }

    return NULL;
}

/* Returns a string that represents 'protocols'.  The return value might be a
 * comma-separated list if 'protocols' doesn't have a simple name.  The return
 * value is "none" if 'protocols' is 0.
 *
 * The caller must free the returned string (with free()). */
char *
ofputil_protocols_to_string(enum ofputil_protocol protocols)
{
    struct ds s;

    ovs_assert(!(protocols & ~OFPUTIL_P_ANY));
    if (protocols == 0) {
        return xstrdup("none");
    }

    ds_init(&s);
    while (protocols) {
        const struct proto_abbrev *p;
        int i;

        if (s.length) {
            ds_put_char(&s, ',');
        }

        for (p = proto_abbrevs; p < &proto_abbrevs[N_PROTO_ABBREVS]; p++) {
            if ((protocols & p->protocol) == p->protocol) {
                ds_put_cstr(&s, p->name);
                protocols &= ~p->protocol;
                goto match;
            }
        }

        for (i = 0; i < CHAR_BIT * sizeof(enum ofputil_protocol); i++) {
            enum ofputil_protocol bit = 1u << i;

            if (protocols & bit) {
                ds_put_cstr(&s, ofputil_protocol_to_string(bit));
                protocols &= ~bit;
                goto match;
            }
        }
        NOT_REACHED();

    match: ;
    }
    return ds_steal_cstr(&s);
}

static enum ofputil_protocol
ofputil_protocol_from_string__(const char *s, size_t n)
{
    const struct proto_abbrev *p;
    int i;

    for (i = 0; i < CHAR_BIT * sizeof(enum ofputil_protocol); i++) {
        enum ofputil_protocol bit = 1u << i;
        const char *name = ofputil_protocol_to_string(bit);

        if (name && n == strlen(name) && !strncasecmp(s, name, n)) {
            return bit;
        }
    }

    for (p = proto_abbrevs; p < &proto_abbrevs[N_PROTO_ABBREVS]; p++) {
        if (n == strlen(p->name) && !strncasecmp(s, p->name, n)) {
            return p->protocol;
        }
    }

    return 0;
}

/* Returns the nonempty set of protocols represented by 's', which can be a
 * single protocol name or abbreviation or a comma-separated list of them.
 *
 * Aborts the program with an error message if 's' is invalid. */
enum ofputil_protocol
ofputil_protocols_from_string(const char *s)
{
    const char *orig_s = s;
    enum ofputil_protocol protocols;

    protocols = 0;
    while (*s) {
        enum ofputil_protocol p;
        size_t n;

        n = strcspn(s, ",");
        if (n == 0) {
            s++;
            continue;
        }

        p = ofputil_protocol_from_string__(s, n);
        if (!p) {
            ovs_fatal(0, "%.*s: unknown flow protocol", (int) n, s);
        }
        protocols |= p;

        s += n;
    }

    if (!protocols) {
        ovs_fatal(0, "%s: no flow protocol specified", orig_s);
    }
    return protocols;
}

static int
ofputil_version_from_string(const char *s)
{
    if (!strcasecmp(s, "OpenFlow10")) {
        return OFP10_VERSION;
    }
    if (!strcasecmp(s, "OpenFlow11")) {
        return OFP11_VERSION;
    }
    if (!strcasecmp(s, "OpenFlow12")) {
        return OFP12_VERSION;
    }
    if (!strcasecmp(s, "OpenFlow13")) {
        return OFP13_VERSION;
    }
    return 0;
}

static bool
is_delimiter(char c)
{
    return isspace(c) || c == ',';
}

uint32_t
ofputil_versions_from_string(const char *s)
{
    size_t i = 0;
    uint32_t bitmap = 0;

    while (s[i]) {
        size_t j;
        int version;
        char *key;

        if (is_delimiter(s[i])) {
            i++;
            continue;
        }
        j = 0;
        while (s[i + j] && !is_delimiter(s[i + j])) {
            j++;
        }
        key = xmemdup0(s + i, j);
        version = ofputil_version_from_string(key);
        if (!version) {
            VLOG_FATAL("Unknown OpenFlow version: \"%s\"", key);
        }
        free(key);
        bitmap |= 1u << version;
        i += j;
    }

    return bitmap;
}

uint32_t
ofputil_versions_from_strings(char ** const s, size_t count)
{
    uint32_t bitmap = 0;

    while (count--) {
        int version = ofputil_version_from_string(s[count]);
        if (!version) {
            VLOG_WARN("Unknown OpenFlow version: \"%s\"", s[count]);
        } else {
            bitmap |= 1u << version;
        }
    }

    return bitmap;
}

const char *
ofputil_version_to_string(enum ofp_version ofp_version)
{
    switch (ofp_version) {
    case OFP10_VERSION:
        return "OpenFlow10";
    case OFP11_VERSION:
        return "OpenFlow11";
    case OFP12_VERSION:
        return "OpenFlow12";
    case OFP13_VERSION:
        return "OpenFlow13";
    default:
        NOT_REACHED();
    }
}

bool
ofputil_packet_in_format_is_valid(enum nx_packet_in_format packet_in_format)
{
    switch (packet_in_format) {
    case NXPIF_OPENFLOW10:
    case NXPIF_NXM:
        return true;
    }

    return false;
}

const char *
ofputil_packet_in_format_to_string(enum nx_packet_in_format packet_in_format)
{
    switch (packet_in_format) {
    case NXPIF_OPENFLOW10:
        return "openflow10";
    case NXPIF_NXM:
        return "nxm";
    default:
        NOT_REACHED();
    }
}

int
ofputil_packet_in_format_from_string(const char *s)
{
    return (!strcmp(s, "openflow10") ? NXPIF_OPENFLOW10
            : !strcmp(s, "nxm") ? NXPIF_NXM
            : -1);
}

static bool
regs_fully_wildcarded(const struct flow_wildcards *wc)
{
    int i;

    for (i = 0; i < FLOW_N_REGS; i++) {
        if (wc->masks.regs[i] != 0) {
            return false;
        }
    }
    return true;
}

static bool
tun_parms_fully_wildcarded(const struct flow_wildcards *wc)
{
    return (!wc->masks.tunnel.ip_src &&
            !wc->masks.tunnel.ip_dst &&
            !wc->masks.tunnel.ip_ttl &&
            !wc->masks.tunnel.ip_tos &&
            !wc->masks.tunnel.flags);
}

/* Returns a bit-mask of ofputil_protocols that can be used for sending 'match'
 * to a switch (e.g. to add or remove a flow).  Only NXM can handle tunnel IDs,
 * registers, or fixing the Ethernet multicast bit.  Otherwise, it's better to
 * use OpenFlow 1.0 protocol for backward compatibility. */
enum ofputil_protocol
ofputil_usable_protocols(const struct match *match)
{
    const struct flow_wildcards *wc = &match->wc;

    BUILD_ASSERT_DECL(FLOW_WC_SEQ == 19);

    /* tunnel params other than tun_id can't be sent in a flow_mod */
    if (!tun_parms_fully_wildcarded(wc)) {
        return OFPUTIL_P_NONE;
    }

    /* skb_mark and skb_priority can't be sent in a flow_mod */
    if (wc->masks.skb_mark || wc->masks.skb_priority) {
        return OFPUTIL_P_NONE;
    }

    /* NXM, OXM, and OF1.1 support bitwise matching on ethernet addresses. */
    if (!eth_mask_is_exact(wc->masks.dl_src)
        && !eth_addr_is_zero(wc->masks.dl_src)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }
    if (!eth_mask_is_exact(wc->masks.dl_dst)
        && !eth_addr_is_zero(wc->masks.dl_dst)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM, OXM, and OF1.1+ support matching metadata. */
    if (wc->masks.metadata != htonll(0)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching ARP hardware addresses. */
    if (!eth_addr_is_zero(wc->masks.arp_sha) ||
        !eth_addr_is_zero(wc->masks.arp_tha)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching IPv6 traffic. */
    if (match->flow.dl_type == htons(ETH_TYPE_IPV6)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching registers. */
    if (!regs_fully_wildcarded(wc)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching tun_id. */
    if (wc->masks.tunnel.tun_id != htonll(0)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching fragments. */
    if (wc->masks.nw_frag) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching IPv6 flow label. */
    if (wc->masks.ipv6_label) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching IP ECN bits. */
    if (wc->masks.nw_tos & IP_ECN_MASK) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support matching IP TTL/hop limit. */
    if (wc->masks.nw_ttl) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support non-CIDR IPv4 address masks. */
    if (!ip_is_cidr(wc->masks.nw_src) || !ip_is_cidr(wc->masks.nw_dst)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OXM support bitwise matching on transport port. */
    if ((wc->masks.tp_src && wc->masks.tp_src != htons(UINT16_MAX)) ||
        (wc->masks.tp_dst && wc->masks.tp_dst != htons(UINT16_MAX))) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OF1.1+ support matching MPLS label */
    if (wc->masks.mpls_lse & htonl(MPLS_LABEL_MASK)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OF1.1+ support matching MPLS TC */
    if (wc->masks.mpls_lse & htonl(MPLS_TC_MASK)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* NXM and OF1.3+ support matching MPLS stack flag */
    /* Allow for OF1.2 as there doesn't seem to be a
     * particularly good reason not to */
    if (wc->masks.mpls_lse & htonl(MPLS_BOS_MASK)) {
        return OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }

    /* Other formats can express this rule. */
    return OFPUTIL_P_ANY;
}

void
ofputil_format_version(struct ds *msg, enum ofp_version version)
{
    ds_put_format(msg, "0x%02x", version);
}

void
ofputil_format_version_name(struct ds *msg, enum ofp_version version)
{
    ds_put_cstr(msg, ofputil_version_to_string(version));
}

static void
ofputil_format_version_bitmap__(struct ds *msg, uint32_t bitmap,
                                void (*format_version)(struct ds *msg,
                                                       enum ofp_version))
{
    while (bitmap) {
        format_version(msg, raw_ctz(bitmap));
        bitmap = zero_rightmost_1bit(bitmap);
        if (bitmap) {
            ds_put_cstr(msg, ", ");
        }
    }
}

void
ofputil_format_version_bitmap(struct ds *msg, uint32_t bitmap)
{
    ofputil_format_version_bitmap__(msg, bitmap, ofputil_format_version);
}

void
ofputil_format_version_bitmap_names(struct ds *msg, uint32_t bitmap)
{
    ofputil_format_version_bitmap__(msg, bitmap, ofputil_format_version_name);
}

static bool
ofputil_decode_hello_bitmap(const struct ofp_hello_elem_header *oheh,
                            uint32_t *allowed_versionsp)
{
    uint16_t bitmap_len = ntohs(oheh->length) - sizeof *oheh;
    const ovs_be32 *bitmap = (const ovs_be32 *) (oheh + 1);
    uint32_t allowed_versions;

    if (!bitmap_len || bitmap_len % sizeof *bitmap) {
        return false;
    }

    /* Only use the first 32-bit element of the bitmap as that is all the
     * current implementation supports.  Subsequent elements are ignored which
     * should have no effect on session negotiation until Open vSwtich supports
     * wire-protocol versions greater than 31.
     */
    allowed_versions = ntohl(bitmap[0]);

    if (allowed_versions & 1) {
        /* There's no OpenFlow version 0. */
        VLOG_WARN_RL(&bad_ofmsg_rl, "peer claims to support invalid OpenFlow "
                     "version 0x00");
        allowed_versions &= ~1u;
    }

    if (!allowed_versions) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "peer does not support any OpenFlow "
                     "version (between 0x01 and 0x1f)");
        return false;
    }

    *allowed_versionsp = allowed_versions;
    return true;
}

static uint32_t
version_bitmap_from_version(uint8_t ofp_version)
{
    return ((ofp_version < 32 ? 1u << ofp_version : 0) - 1) << 1;
}

/* Decodes OpenFlow OFPT_HELLO message 'oh', storing into '*allowed_versions'
 * the set of OpenFlow versions for which 'oh' announces support.
 *
 * Because of how OpenFlow defines OFPT_HELLO messages, this function is always
 * successful, and thus '*allowed_versions' is always initialized.  However, it
 * returns false if 'oh' contains some data that could not be fully understood,
 * true if 'oh' was completely parsed. */
bool
ofputil_decode_hello(const struct ofp_header *oh, uint32_t *allowed_versions)
{
    struct ofpbuf msg;
    bool ok = true;

    ofpbuf_use_const(&msg, oh, ntohs(oh->length));
    ofpbuf_pull(&msg, sizeof *oh);

    *allowed_versions = version_bitmap_from_version(oh->version);
    while (msg.size) {
        const struct ofp_hello_elem_header *oheh;
        unsigned int len;

        if (msg.size < sizeof *oheh) {
            return false;
        }

        oheh = msg.data;
        len = ntohs(oheh->length);
        if (len < sizeof *oheh || !ofpbuf_try_pull(&msg, ROUND_UP(len, 8))) {
            return false;
        }

        if (oheh->type != htons(OFPHET_VERSIONBITMAP)
            || !ofputil_decode_hello_bitmap(oheh, allowed_versions)) {
            ok = false;
        }
    }

    return ok;
}

/* Returns true if 'allowed_versions' needs to be accompanied by a version
 * bitmap to be correctly expressed in an OFPT_HELLO message. */
static inline bool
should_send_version_bitmap(uint32_t allowed_versions)
{
    return !is_pow2((allowed_versions >> 1) + 1);
}

/* Create an OFPT_HELLO message that expresses support for the OpenFlow
 * versions in the 'allowed_versions' bitmaps and returns the message. */
struct ofpbuf *
ofputil_encode_hello(uint32_t allowed_versions)
{
    enum ofp_version ofp_version;
    struct ofpbuf *msg;

    ofp_version = leftmost_1bit_idx(allowed_versions);
    msg = ofpraw_alloc(OFPRAW_OFPT_HELLO, ofp_version, 0);

    if (should_send_version_bitmap(allowed_versions)) {
        struct ofp_hello_elem_header *oheh;
        uint16_t map_len;

        map_len = sizeof allowed_versions;
        oheh = ofpbuf_put_zeros(msg, ROUND_UP(map_len + sizeof *oheh, 8));
        oheh->type = htons(OFPHET_VERSIONBITMAP);
        oheh->length = htons(map_len + sizeof *oheh);
        *(ovs_be32 *)(oheh + 1) = htonl(allowed_versions);

        ofpmsg_update_length(msg);
    }

    return msg;
}

/* Returns an OpenFlow message that, sent on an OpenFlow connection whose
 * protocol is 'current', at least partly transitions the protocol to 'want'.
 * Stores in '*next' the protocol that will be in effect on the OpenFlow
 * connection if the switch processes the returned message correctly.  (If
 * '*next != want' then the caller will have to iterate.)
 *
 * If 'current == want', or if it is not possible to transition from 'current'
 * to 'want' (because, for example, 'current' and 'want' use different OpenFlow
 * protocol versions), returns NULL and stores 'current' in '*next'. */
struct ofpbuf *
ofputil_encode_set_protocol(enum ofputil_protocol current,
                            enum ofputil_protocol want,
                            enum ofputil_protocol *next)
{
    enum ofp_version cur_version, want_version;
    enum ofputil_protocol cur_base, want_base;
    bool cur_tid, want_tid;

    cur_version = ofputil_protocol_to_ofp_version(current);
    want_version = ofputil_protocol_to_ofp_version(want);
    if (cur_version != want_version) {
        *next = current;
        return NULL;
    }

    cur_base = ofputil_protocol_to_base(current);
    want_base = ofputil_protocol_to_base(want);
    if (cur_base != want_base) {
        *next = ofputil_protocol_set_base(current, want_base);

        switch (want_base) {
        case OFPUTIL_P_OF10_NXM:
            return ofputil_encode_nx_set_flow_format(NXFF_NXM);

        case OFPUTIL_P_OF10_STD:
            return ofputil_encode_nx_set_flow_format(NXFF_OPENFLOW10);

        case OFPUTIL_P_OF12_OXM:
        case OFPUTIL_P_OF13_OXM:
            /* There are only one of each OpenFlow 1.2+ protocols and we already
             * verified above that we're not trying to change versions. */
            NOT_REACHED();

        case OFPUTIL_P_OF10_STD_TID:
        case OFPUTIL_P_OF10_NXM_TID:
            NOT_REACHED();
        }
    }

    cur_tid = (current & OFPUTIL_P_TID) != 0;
    want_tid = (want & OFPUTIL_P_TID) != 0;
    if (cur_tid != want_tid) {
        *next = ofputil_protocol_set_tid(current, want_tid);
        return ofputil_make_flow_mod_table_id(want_tid);
    }

    ovs_assert(current == want);

    *next = current;
    return NULL;
}

/* Returns an NXT_SET_FLOW_FORMAT message that can be used to set the flow
 * format to 'nxff'.  */
struct ofpbuf *
ofputil_encode_nx_set_flow_format(enum nx_flow_format nxff)
{
    struct nx_set_flow_format *sff;
    struct ofpbuf *msg;

    ovs_assert(ofputil_nx_flow_format_is_valid(nxff));

    msg = ofpraw_alloc(OFPRAW_NXT_SET_FLOW_FORMAT, OFP10_VERSION, 0);
    sff = ofpbuf_put_zeros(msg, sizeof *sff);
    sff->format = htonl(nxff);

    return msg;
}

/* Returns the base protocol if 'flow_format' is a valid NXFF_* value, false
 * otherwise. */
enum ofputil_protocol
ofputil_nx_flow_format_to_protocol(enum nx_flow_format flow_format)
{
    switch (flow_format) {
    case NXFF_OPENFLOW10:
        return OFPUTIL_P_OF10_STD;

    case NXFF_NXM:
        return OFPUTIL_P_OF10_NXM;

    default:
        return 0;
    }
}

/* Returns true if 'flow_format' is a valid NXFF_* value, false otherwise. */
bool
ofputil_nx_flow_format_is_valid(enum nx_flow_format flow_format)
{
    return ofputil_nx_flow_format_to_protocol(flow_format) != 0;
}

/* Returns a string version of 'flow_format', which must be a valid NXFF_*
 * value. */
const char *
ofputil_nx_flow_format_to_string(enum nx_flow_format flow_format)
{
    switch (flow_format) {
    case NXFF_OPENFLOW10:
        return "openflow10";
    case NXFF_NXM:
        return "nxm";
    default:
        NOT_REACHED();
    }
}

struct ofpbuf *
ofputil_make_set_packet_in_format(enum ofp_version ofp_version,
                                  enum nx_packet_in_format packet_in_format)
{
    struct nx_set_packet_in_format *spif;
    struct ofpbuf *msg;

    msg = ofpraw_alloc(OFPRAW_NXT_SET_PACKET_IN_FORMAT, ofp_version, 0);
    spif = ofpbuf_put_zeros(msg, sizeof *spif);
    spif->format = htonl(packet_in_format);

    return msg;
}

/* Returns an OpenFlow message that can be used to turn the flow_mod_table_id
 * extension on or off (according to 'flow_mod_table_id'). */
struct ofpbuf *
ofputil_make_flow_mod_table_id(bool flow_mod_table_id)
{
    struct nx_flow_mod_table_id *nfmti;
    struct ofpbuf *msg;

    msg = ofpraw_alloc(OFPRAW_NXT_FLOW_MOD_TABLE_ID, OFP10_VERSION, 0);
    nfmti = ofpbuf_put_zeros(msg, sizeof *nfmti);
    nfmti->set = flow_mod_table_id;
    return msg;
}

/* Converts an OFPT_FLOW_MOD or NXT_FLOW_MOD message 'oh' into an abstract
 * flow_mod in 'fm'.  Returns 0 if successful, otherwise an OpenFlow error
 * code.
 *
 * Uses 'ofpacts' to store the abstract OFPACT_* version of 'oh''s actions.
 * The caller must initialize 'ofpacts' and retains ownership of it.
 * 'fm->ofpacts' will point into the 'ofpacts' buffer.
 *
 * Does not validate the flow_mod actions.  The caller should do that, with
 * ofpacts_check(). */
enum ofperr
ofputil_decode_flow_mod(struct ofputil_flow_mod *fm,
                        const struct ofp_header *oh,
                        enum ofputil_protocol protocol,
                        struct ofpbuf *ofpacts)
{
    uint16_t command;
    struct ofpbuf b;
    enum ofpraw raw;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    if (raw == OFPRAW_OFPT11_FLOW_MOD) {
        /* Standard OpenFlow 1.1 flow_mod. */
        const struct ofp11_flow_mod *ofm;
        enum ofperr error;

        ofm = ofpbuf_pull(&b, sizeof *ofm);

        error = ofputil_pull_ofp11_match(&b, &fm->match, NULL);
        if (error) {
            return error;
        }

        error = ofpacts_pull_openflow11_instructions(&b, b.size, ofpacts);
        if (error) {
            return error;
        }

        /* Translate the message. */
        fm->priority = ntohs(ofm->priority);
        if (ofm->command == OFPFC_ADD) {
            fm->cookie = htonll(0);
            fm->cookie_mask = htonll(0);
            fm->new_cookie = ofm->cookie;
        } else {
            fm->cookie = ofm->cookie;
            fm->cookie_mask = ofm->cookie_mask;
            fm->new_cookie = htonll(UINT64_MAX);
        }
        fm->command = ofm->command;
        fm->table_id = ofm->table_id;
        fm->idle_timeout = ntohs(ofm->idle_timeout);
        fm->hard_timeout = ntohs(ofm->hard_timeout);
        fm->buffer_id = ntohl(ofm->buffer_id);
        error = ofputil_port_from_ofp11(ofm->out_port, &fm->out_port);
        if (error) {
            return error;
        }

#ifdef _OFP_CENTEC_
        fm->out_group = ntohl(ofm->out_group);
#endif

#ifndef _OFP_CENTEC_
        if ((ofm->command == OFPFC_DELETE
             || ofm->command == OFPFC_DELETE_STRICT)
            && ofm->out_group != htonl(OFPG_ANY)) {
            return OFPERR_OFPFMFC_UNKNOWN;
        }
#endif
        fm->flags = ntohs(ofm->flags);

#ifdef _OFP_CENTEC_
        /* note: OFPFMFC_BAD_FLAGS is defined in openflow spec 1.2+ */
        if (protocol == OFPUTIL_P_OF12_OXM) {
            uint16_t unsupported_ofp12_flags = ~(OFPFF_SEND_FLOW_REM | OFPFF_CHECK_OVERLAP);
            if (fm->flags & unsupported_ofp12_flags) {
                return OFPERR_OFPFMFC_BAD_FLAGS;
            }
        } else if (protocol == OFPUTIL_P_OF13_OXM) {
            uint16_t unsupported_ofp13_flags = ~(OFPFF_SEND_FLOW_REM | OFPFF_CHECK_OVERLAP);
            if (fm->flags & unsupported_ofp13_flags) {
                return OFPERR_OFPFMFC_BAD_FLAGS;
            }
        }
#endif
    } else {
        if (raw == OFPRAW_OFPT10_FLOW_MOD) {
            /* Standard OpenFlow 1.0 flow_mod. */
            const struct ofp10_flow_mod *ofm;
            enum ofperr error;

            /* Get the ofp10_flow_mod. */
            ofm = ofpbuf_pull(&b, sizeof *ofm);

            /* Translate the rule. */
            ofputil_match_from_ofp10_match(&ofm->match, &fm->match);
            ofputil_normalize_match(&fm->match);

            /* Now get the actions. */
            error = ofpacts_pull_openflow10(&b, b.size, ofpacts);
            if (error) {
                return error;
            }

            /* OpenFlow 1.0 says that exact-match rules have to have the
             * highest possible priority. */
            fm->priority = (ofm->match.wildcards & htonl(OFPFW10_ALL)
                            ? ntohs(ofm->priority)
                            : UINT16_MAX);

            /* Translate the message. */
            command = ntohs(ofm->command);
            fm->cookie = htonll(0);
            fm->cookie_mask = htonll(0);
            fm->new_cookie = ofm->cookie;
            fm->idle_timeout = ntohs(ofm->idle_timeout);
            fm->hard_timeout = ntohs(ofm->hard_timeout);
            fm->buffer_id = ntohl(ofm->buffer_id);
            fm->out_port = ntohs(ofm->out_port);
            fm->flags = ntohs(ofm->flags);

#ifdef _OFP_CENTEC_
            if (command == OFPFC_DELETE
                         || command == OFPFC_DELETE_STRICT) {
                fm->out_group = OFPG11_ANY;
            }
#endif
        } else if (raw == OFPRAW_NXT_FLOW_MOD) {
            /* Nicira extended flow_mod. */
            const struct nx_flow_mod *nfm;
            enum ofperr error;

            /* Dissect the message. */
            nfm = ofpbuf_pull(&b, sizeof *nfm);
            error = nx_pull_match(&b, ntohs(nfm->match_len),
                                  &fm->match, &fm->cookie, &fm->cookie_mask);
            if (error) {
                return error;
            }
            error = ofpacts_pull_openflow10(&b, b.size, ofpacts);
            if (error) {
                return error;
            }

            /* Translate the message. */
            command = ntohs(nfm->command);
            if ((command & 0xff) == OFPFC_ADD && fm->cookie_mask) {
                /* Flow additions may only set a new cookie, not match an
                 * existing cookie. */
                return OFPERR_NXBRC_NXM_INVALID;
            }
            fm->priority = ntohs(nfm->priority);
            fm->new_cookie = nfm->cookie;
            fm->idle_timeout = ntohs(nfm->idle_timeout);
            fm->hard_timeout = ntohs(nfm->hard_timeout);
            fm->buffer_id = ntohl(nfm->buffer_id);
            fm->out_port = ntohs(nfm->out_port);
            fm->flags = ntohs(nfm->flags);
#ifdef _OFP_CENTEC_
            if (command == OFPFC_DELETE
                         || command == OFPFC_DELETE_STRICT) {
                fm->out_group = OFPG11_ANY;
            }
#endif
        } else {
            NOT_REACHED();
        }

        if (fm->flags & OFPFF10_EMERG) {
            /* We do not support the OpenFlow 1.0 emergency flow cache, which
             * is not required in OpenFlow 1.0.1 and removed from OpenFlow 1.1.
             *
             * OpenFlow 1.0 specifies the error code to use when idle_timeout
             * or hard_timeout is nonzero.  Otherwise, there is no good error
             * code, so just state that the flow table is full. */
            return (fm->hard_timeout || fm->idle_timeout
                    ? OFPERR_OFPFMFC_BAD_EMERG_TIMEOUT
                    : OFPERR_OFPFMFC_TABLE_FULL);
        }

        if (protocol & OFPUTIL_P_TID) {
            fm->command = command & 0xff;
            fm->table_id = command >> 8;
        } else {
            fm->command = command;
            fm->table_id = 0xff;
        }
    }

    fm->ofpacts = ofpacts->data;
    fm->ofpacts_len = ofpacts->size;

    return 0;
}

static ovs_be16
ofputil_tid_command(const struct ofputil_flow_mod *fm,
                    enum ofputil_protocol protocol)
{
    return htons(protocol & OFPUTIL_P_TID
                 ? (fm->command & 0xff) | (fm->table_id << 8)
                 : fm->command);
}

/* Converts 'fm' into an OFPT_FLOW_MOD or NXT_FLOW_MOD message according to
 * 'protocol' and returns the message. */
struct ofpbuf *
ofputil_encode_flow_mod(const struct ofputil_flow_mod *fm,
                        enum ofputil_protocol protocol)
{
    struct ofpbuf *msg;

    switch (protocol) {
    case OFPUTIL_P_OF12_OXM:
    case OFPUTIL_P_OF13_OXM: {
        struct ofp11_flow_mod *ofm;

        msg = ofpraw_alloc(OFPRAW_OFPT11_FLOW_MOD, 
                           ofputil_protocol_to_ofp_version(protocol),
                           NXM_TYPICAL_LEN + fm->ofpacts_len);
        ofm = ofpbuf_put_zeros(msg, sizeof *ofm);
        if (fm->command == OFPFC_ADD) {
            ofm->cookie = fm->new_cookie;
        } else {
            ofm->cookie = fm->cookie;
        }
        ofm->cookie_mask = fm->cookie_mask;
        ofm->table_id = fm->table_id;
        ofm->command = fm->command;
        ofm->idle_timeout = htons(fm->idle_timeout);
        ofm->hard_timeout = htons(fm->hard_timeout);
        ofm->priority = htons(fm->priority);
        ofm->buffer_id = htonl(fm->buffer_id);
        ofm->out_port = ofputil_port_to_ofp11(fm->out_port);
#ifdef _OFP_CENTEC_
        ofm->out_group = htonl(fm->out_group);
#else
        ofm->out_group = htonl(OFPG11_ANY);
#endif
        ofm->flags = htons(fm->flags);
        oxm_put_match(msg, &fm->match);
        ofpacts_put_openflow11_instructions(fm->ofpacts, fm->ofpacts_len, msg);
        break;
    }

    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID: {
        struct ofp10_flow_mod *ofm;

        msg = ofpraw_alloc(OFPRAW_OFPT10_FLOW_MOD, OFP10_VERSION,
                           fm->ofpacts_len);
        ofm = ofpbuf_put_zeros(msg, sizeof *ofm);
        ofputil_match_to_ofp10_match(&fm->match, &ofm->match);
        ofm->cookie = fm->new_cookie;
        ofm->command = ofputil_tid_command(fm, protocol);
        ofm->idle_timeout = htons(fm->idle_timeout);
        ofm->hard_timeout = htons(fm->hard_timeout);
        ofm->priority = htons(fm->priority);
        ofm->buffer_id = htonl(fm->buffer_id);
        ofm->out_port = htons(fm->out_port);
        ofm->flags = htons(fm->flags);
        ofpacts_put_openflow10(fm->ofpacts, fm->ofpacts_len, msg);
        break;
    }

    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID: {
        struct nx_flow_mod *nfm;
        int match_len;

        msg = ofpraw_alloc(OFPRAW_NXT_FLOW_MOD, OFP10_VERSION,
                           NXM_TYPICAL_LEN + fm->ofpacts_len);
        nfm = ofpbuf_put_zeros(msg, sizeof *nfm);
        nfm->command = ofputil_tid_command(fm, protocol);
        nfm->cookie = fm->new_cookie;
        match_len = nx_put_match(msg, &fm->match, fm->cookie, fm->cookie_mask);
        nfm = msg->l3;
        nfm->idle_timeout = htons(fm->idle_timeout);
        nfm->hard_timeout = htons(fm->hard_timeout);
        nfm->priority = htons(fm->priority);
        nfm->buffer_id = htonl(fm->buffer_id);
        nfm->out_port = htons(fm->out_port);
        nfm->flags = htons(fm->flags);
        nfm->match_len = htons(match_len);
        ofpacts_put_openflow10(fm->ofpacts, fm->ofpacts_len, msg);
        break;
    }

    default:
        NOT_REACHED();
    }

    ofpmsg_update_length(msg);
    return msg;
}

/* Returns a bitmask with a 1-bit for each protocol that could be used to
 * send all of the 'n_fm's flow table modification requests in 'fms', and a
 * 0-bit for each protocol that is inadequate.
 *
 * (The return value will have at least one 1-bit.) */
enum ofputil_protocol
ofputil_flow_mod_usable_protocols(const struct ofputil_flow_mod *fms,
                                  size_t n_fms)
{
    enum ofputil_protocol usable_protocols;
    size_t i;

    usable_protocols = OFPUTIL_P_ANY;
    for (i = 0; i < n_fms; i++) {
        const struct ofputil_flow_mod *fm = &fms[i];

        usable_protocols &= ofputil_usable_protocols(&fm->match);
        if (fm->table_id != 0xff) {
            usable_protocols &= OFPUTIL_P_TID;
        }

        /* Matching of the cookie is only supported through NXM or OF1.1+. */
        if (fm->cookie_mask != htonll(0)) {
            usable_protocols &= OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
                | OFPUTIL_P_OF13_OXM;
        }
    }

    return usable_protocols;
}

static enum ofperr
ofputil_decode_ofpst10_flow_request(struct ofputil_flow_stats_request *fsr,
                                    const struct ofp10_flow_stats_request *ofsr,
                                    bool aggregate)
{
    fsr->aggregate = aggregate;
    ofputil_match_from_ofp10_match(&ofsr->match, &fsr->match);
    fsr->out_port = ntohs(ofsr->out_port);
    fsr->table_id = ofsr->table_id;
    fsr->cookie = fsr->cookie_mask = htonll(0);
#ifdef _OFP_CENTEC_
    fsr->out_group = htonl(OFPG11_ANY);
#endif

    return 0;
}

static enum ofperr
ofputil_decode_ofpst11_flow_request(struct ofputil_flow_stats_request *fsr,
                                    struct ofpbuf *b, bool aggregate)
{
    const struct ofp11_flow_stats_request *ofsr;
    enum ofperr error;

    ofsr = ofpbuf_pull(b, sizeof *ofsr);
    fsr->aggregate = aggregate;
    fsr->table_id = ofsr->table_id;
    error = ofputil_port_from_ofp11(ofsr->out_port, &fsr->out_port);
    if (error) {
        return error;
    }
#ifndef _OFP_CENTEC_
    if (ofsr->out_group != htonl(OFPG11_ANY)) {
        return OFPERR_OFPFMFC_UNKNOWN;
    }
#else
	fsr->out_group = ntohl(ofsr->out_group);
#endif
    fsr->cookie = ofsr->cookie;
    fsr->cookie_mask = ofsr->cookie_mask;
    error = ofputil_pull_ofp11_match(b, &fsr->match, NULL);
    if (error) {
        return error;
    }

    return 0;
}

static enum ofperr
ofputil_decode_nxst_flow_request(struct ofputil_flow_stats_request *fsr,
                                 struct ofpbuf *b, bool aggregate)
{
    const struct nx_flow_stats_request *nfsr;
    enum ofperr error;

    nfsr = ofpbuf_pull(b, sizeof *nfsr);
    error = nx_pull_match(b, ntohs(nfsr->match_len), &fsr->match,
                          &fsr->cookie, &fsr->cookie_mask);
    if (error) {
        return error;
    }
    if (b->size) {
        return OFPERR_OFPBRC_BAD_LEN;
    }

    fsr->aggregate = aggregate;
    fsr->out_port = ntohs(nfsr->out_port);
    fsr->table_id = nfsr->table_id;
#ifdef _OFP_CENTEC_
    fsr->out_group = htonl(OFPG11_ANY);
#endif

    return 0;
}

/* Converts an OFPST_FLOW, OFPST_AGGREGATE, NXST_FLOW, or NXST_AGGREGATE
 * request 'oh', into an abstract flow_stats_request in 'fsr'.  Returns 0 if
 * successful, otherwise an OpenFlow error code. */
enum ofperr
ofputil_decode_flow_stats_request(struct ofputil_flow_stats_request *fsr,
                                  const struct ofp_header *oh)
{
    enum ofpraw raw;
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    switch ((int) raw) {
    case OFPRAW_OFPST10_FLOW_REQUEST:
        return ofputil_decode_ofpst10_flow_request(fsr, b.data, false);

    case OFPRAW_OFPST10_AGGREGATE_REQUEST:
        return ofputil_decode_ofpst10_flow_request(fsr, b.data, true);

    case OFPRAW_OFPST11_FLOW_REQUEST:
        return ofputil_decode_ofpst11_flow_request(fsr, &b, false);

    case OFPRAW_OFPST11_AGGREGATE_REQUEST:
        return ofputil_decode_ofpst11_flow_request(fsr, &b, true);

    case OFPRAW_NXST_FLOW_REQUEST:
        return ofputil_decode_nxst_flow_request(fsr, &b, false);

    case OFPRAW_NXST_AGGREGATE_REQUEST:
        return ofputil_decode_nxst_flow_request(fsr, &b, true);

    default:
        /* Hey, the caller lied. */
        NOT_REACHED();
    }
}

/* Converts abstract flow_stats_request 'fsr' into an OFPST_FLOW,
 * OFPST_AGGREGATE, NXST_FLOW, or NXST_AGGREGATE request 'oh' according to
 * 'protocol', and returns the message. */
struct ofpbuf *
ofputil_encode_flow_stats_request(const struct ofputil_flow_stats_request *fsr,
                                  enum ofputil_protocol protocol)
{
    struct ofpbuf *msg;
    enum ofpraw raw;

    switch (protocol) {
    case OFPUTIL_P_OF12_OXM:
    case OFPUTIL_P_OF13_OXM: {
        struct ofp11_flow_stats_request *ofsr;

        raw = (fsr->aggregate
               ? OFPRAW_OFPST11_AGGREGATE_REQUEST
               : OFPRAW_OFPST11_FLOW_REQUEST);
        msg = ofpraw_alloc(raw, ofputil_protocol_to_ofp_version(protocol),
			   NXM_TYPICAL_LEN);
        ofsr = ofpbuf_put_zeros(msg, sizeof *ofsr);
        ofsr->table_id = fsr->table_id;
        ofsr->out_port = ofputil_port_to_ofp11(fsr->out_port);
#ifndef _OFP_CENTEC_
        ofsr->out_group = htonl(OFPG11_ANY);
#else
        ofsr->out_group = htonl(fsr->out_group);
#endif
        ofsr->cookie = fsr->cookie;
        ofsr->cookie_mask = fsr->cookie_mask;
        oxm_put_match(msg, &fsr->match);
        break;
    }

    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID: {
        struct ofp10_flow_stats_request *ofsr;

        raw = (fsr->aggregate
               ? OFPRAW_OFPST10_AGGREGATE_REQUEST
               : OFPRAW_OFPST10_FLOW_REQUEST);
        msg = ofpraw_alloc(raw, OFP10_VERSION, 0);
        ofsr = ofpbuf_put_zeros(msg, sizeof *ofsr);
        ofputil_match_to_ofp10_match(&fsr->match, &ofsr->match);
        ofsr->table_id = fsr->table_id;
        ofsr->out_port = htons(fsr->out_port);
        break;
    }

    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID: {
        struct nx_flow_stats_request *nfsr;
        int match_len;

        raw = (fsr->aggregate
               ? OFPRAW_NXST_AGGREGATE_REQUEST
               : OFPRAW_NXST_FLOW_REQUEST);
        msg = ofpraw_alloc(raw, OFP10_VERSION, NXM_TYPICAL_LEN);
        ofpbuf_put_zeros(msg, sizeof *nfsr);
        match_len = nx_put_match(msg, &fsr->match,
                                 fsr->cookie, fsr->cookie_mask);

        nfsr = msg->l3;
        nfsr->out_port = htons(fsr->out_port);
        nfsr->match_len = htons(match_len);
        nfsr->table_id = fsr->table_id;
        break;
    }

    default:
        NOT_REACHED();
    }

    return msg;
}

/* Returns a bitmask with a 1-bit for each protocol that could be used to
 * accurately encode 'fsr', and a 0-bit for each protocol that is inadequate.
 *
 * (The return value will have at least one 1-bit.) */
enum ofputil_protocol
ofputil_flow_stats_request_usable_protocols(
    const struct ofputil_flow_stats_request *fsr)
{
    enum ofputil_protocol usable_protocols;

    usable_protocols = ofputil_usable_protocols(&fsr->match);
    if (fsr->cookie_mask != htonll(0)) {
        usable_protocols &= OFPUTIL_P_OF10_NXM_ANY | OFPUTIL_P_OF12_OXM
            | OFPUTIL_P_OF13_OXM;
    }
    return usable_protocols;
}

/* Converts an OFPST_FLOW or NXST_FLOW reply in 'msg' into an abstract
 * ofputil_flow_stats in 'fs'.
 *
 * Multiple OFPST_FLOW or NXST_FLOW replies can be packed into a single
 * OpenFlow message.  Calling this function multiple times for a single 'msg'
 * iterates through the replies.  The caller must initially leave 'msg''s layer
 * pointers null and not modify them between calls.
 *
 * Most switches don't send the values needed to populate fs->idle_age and
 * fs->hard_age, so those members will usually be set to 0.  If the switch from
 * which 'msg' originated is known to implement NXT_FLOW_AGE, then pass
 * 'flow_age_extension' as true so that the contents of 'msg' determine the
 * 'idle_age' and 'hard_age' members in 'fs'.
 *
 * Uses 'ofpacts' to store the abstract OFPACT_* version of the flow stats
 * reply's actions.  The caller must initialize 'ofpacts' and retains ownership
 * of it.  'fs->ofpacts' will point into the 'ofpacts' buffer.
 *
 * Returns 0 if successful, EOF if no replies were left in this 'msg',
 * otherwise a positive errno value. */
int
ofputil_decode_flow_stats_reply(struct ofputil_flow_stats *fs,
                                struct ofpbuf *msg,
                                bool flow_age_extension,
                                struct ofpbuf *ofpacts)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST11_FLOW_REPLY
               || raw == OFPRAW_OFPST13_FLOW_REPLY) {
        const struct ofp11_flow_stats *ofs;
        size_t length;
        uint16_t padded_match_len;

        ofs = ofpbuf_try_pull(msg, sizeof *ofs);
        if (!ofs) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply has %zu leftover "
                         "bytes at end", msg->size);
            return EINVAL;
        }

        length = ntohs(ofs->length);
        if (length < sizeof *ofs) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply claims invalid "
                         "length %zu", length);
            return EINVAL;
        }

        if (ofputil_pull_ofp11_match(msg, &fs->match, &padded_match_len)) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply bad match");
            return EINVAL;
        }

        if (ofpacts_pull_openflow11_instructions(msg, length - sizeof *ofs -
                                                 padded_match_len, ofpacts)) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply bad instructions");
            return EINVAL;
        }

        fs->priority = ntohs(ofs->priority);
        fs->table_id = ofs->table_id;
        fs->duration_sec = ntohl(ofs->duration_sec);
        fs->duration_nsec = ntohl(ofs->duration_nsec);
        fs->idle_timeout = ntohs(ofs->idle_timeout);
        fs->hard_timeout = ntohs(ofs->hard_timeout);
        fs->flags = (raw == OFPRAW_OFPST13_FLOW_REPLY) ? ntohs(ofs->flags) : 0;
        fs->idle_age = -1;
        fs->hard_age = -1;
        fs->cookie = ofs->cookie;
        fs->packet_count = ntohll(ofs->packet_count);
        fs->byte_count = ntohll(ofs->byte_count);
    } else if (raw == OFPRAW_OFPST10_FLOW_REPLY) {
        const struct ofp10_flow_stats *ofs;
        size_t length;

        ofs = ofpbuf_try_pull(msg, sizeof *ofs);
        if (!ofs) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply has %zu leftover "
                         "bytes at end", msg->size);
            return EINVAL;
        }

        length = ntohs(ofs->length);
        if (length < sizeof *ofs) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_FLOW reply claims invalid "
                         "length %zu", length);
            return EINVAL;
        }

        if (ofpacts_pull_openflow10(msg, length - sizeof *ofs, ofpacts)) {
            return EINVAL;
        }

        fs->cookie = get_32aligned_be64(&ofs->cookie);
        ofputil_match_from_ofp10_match(&ofs->match, &fs->match);
        fs->priority = ntohs(ofs->priority);
        fs->table_id = ofs->table_id;
        fs->duration_sec = ntohl(ofs->duration_sec);
        fs->duration_nsec = ntohl(ofs->duration_nsec);
        fs->idle_timeout = ntohs(ofs->idle_timeout);
        fs->hard_timeout = ntohs(ofs->hard_timeout);
        fs->idle_age = -1;
        fs->hard_age = -1;
        fs->packet_count = ntohll(get_32aligned_be64(&ofs->packet_count));
        fs->byte_count = ntohll(get_32aligned_be64(&ofs->byte_count));
        fs->flags = 0;
    } else if (raw == OFPRAW_NXST_FLOW_REPLY) {
        const struct nx_flow_stats *nfs;
        size_t match_len, actions_len, length;

        nfs = ofpbuf_try_pull(msg, sizeof *nfs);
        if (!nfs) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "NXST_FLOW reply has %zu leftover "
                         "bytes at end", msg->size);
            return EINVAL;
        }

        length = ntohs(nfs->length);
        match_len = ntohs(nfs->match_len);
        if (length < sizeof *nfs + ROUND_UP(match_len, 8)) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "NXST_FLOW reply with match_len=%zu "
                         "claims invalid length %zu", match_len, length);
            return EINVAL;
        }
        if (nx_pull_match(msg, match_len, &fs->match, NULL, NULL)) {
            return EINVAL;
        }

        actions_len = length - sizeof *nfs - ROUND_UP(match_len, 8);
        if (ofpacts_pull_openflow10(msg, actions_len, ofpacts)) {
            return EINVAL;
        }

        fs->cookie = nfs->cookie;
        fs->table_id = nfs->table_id;
        fs->duration_sec = ntohl(nfs->duration_sec);
        fs->duration_nsec = ntohl(nfs->duration_nsec);
        fs->priority = ntohs(nfs->priority);
        fs->idle_timeout = ntohs(nfs->idle_timeout);
        fs->hard_timeout = ntohs(nfs->hard_timeout);
        fs->idle_age = -1;
        fs->hard_age = -1;
        if (flow_age_extension) {
            if (nfs->idle_age) {
                fs->idle_age = ntohs(nfs->idle_age) - 1;
            }
            if (nfs->hard_age) {
                fs->hard_age = ntohs(nfs->hard_age) - 1;
            }
        }
        fs->packet_count = ntohll(nfs->packet_count);
        fs->byte_count = ntohll(nfs->byte_count);
        fs->flags = 0;
    } else {
        NOT_REACHED();
    }

    fs->ofpacts = ofpacts->data;
    fs->ofpacts_len = ofpacts->size;

    return 0;
}

/* Returns 'count' unchanged except that UINT64_MAX becomes 0.
 *
 * We use this in situations where OVS internally uses UINT64_MAX to mean
 * "value unknown" but OpenFlow 1.0 does not define any unknown value. */
static uint64_t
unknown_to_zero(uint64_t count)
{
    return count != UINT64_MAX ? count : 0;
}

/* Appends an OFPST_FLOW or NXST_FLOW reply that contains the data in 'fs' to
 * those already present in the list of ofpbufs in 'replies'.  'replies' should
 * have been initialized with ofputil_start_stats_reply(). */
void
ofputil_append_flow_stats_reply(const struct ofputil_flow_stats *fs,
                                struct list *replies)
{
    struct ofpbuf *reply = ofpbuf_from_list(list_back(replies));
    size_t start_ofs = reply->size;
    enum ofpraw raw;

    ofpraw_decode_partial(&raw, reply->data, reply->size);
    if (raw == OFPRAW_OFPST11_FLOW_REPLY || raw == OFPRAW_OFPST13_FLOW_REPLY) {
        struct ofp11_flow_stats *ofs;

        ofpbuf_put_uninit(reply, sizeof *ofs);
        oxm_put_match(reply, &fs->match);
        ofpacts_put_openflow11_instructions(fs->ofpacts, fs->ofpacts_len,
                                            reply);

        ofs = ofpbuf_at_assert(reply, start_ofs, sizeof *ofs);
        ofs->length = htons(reply->size - start_ofs);
        ofs->table_id = fs->table_id;
        ofs->pad = 0;
        ofs->duration_sec = htonl(fs->duration_sec);
        ofs->duration_nsec = htonl(fs->duration_nsec);
        ofs->priority = htons(fs->priority);
        ofs->idle_timeout = htons(fs->idle_timeout);
        ofs->hard_timeout = htons(fs->hard_timeout);
        ofs->flags = (raw == OFPRAW_OFPST13_FLOW_REPLY) ? htons(fs->flags) : 0;
        memset(ofs->pad2, 0, sizeof ofs->pad2);
        ofs->cookie = fs->cookie;
        ofs->packet_count = htonll(unknown_to_zero(fs->packet_count));
        ofs->byte_count = htonll(unknown_to_zero(fs->byte_count));
    } else if (raw == OFPRAW_OFPST10_FLOW_REPLY) {
        struct ofp10_flow_stats *ofs;

        ofpbuf_put_uninit(reply, sizeof *ofs);
        ofpacts_put_openflow10(fs->ofpacts, fs->ofpacts_len, reply);

        ofs = ofpbuf_at_assert(reply, start_ofs, sizeof *ofs);
        ofs->length = htons(reply->size - start_ofs);
        ofs->table_id = fs->table_id;
        ofs->pad = 0;
        ofputil_match_to_ofp10_match(&fs->match, &ofs->match);
        ofs->duration_sec = htonl(fs->duration_sec);
        ofs->duration_nsec = htonl(fs->duration_nsec);
        ofs->priority = htons(fs->priority);
        ofs->idle_timeout = htons(fs->idle_timeout);
        ofs->hard_timeout = htons(fs->hard_timeout);
        memset(ofs->pad2, 0, sizeof ofs->pad2);
        put_32aligned_be64(&ofs->cookie, fs->cookie);
        put_32aligned_be64(&ofs->packet_count,
                           htonll(unknown_to_zero(fs->packet_count)));
        put_32aligned_be64(&ofs->byte_count,
                           htonll(unknown_to_zero(fs->byte_count)));
    } else if (raw == OFPRAW_NXST_FLOW_REPLY) {
        struct nx_flow_stats *nfs;
        int match_len;

        ofpbuf_put_uninit(reply, sizeof *nfs);
        match_len = nx_put_match(reply, &fs->match, 0, 0);
        ofpacts_put_openflow10(fs->ofpacts, fs->ofpacts_len, reply);

        nfs = ofpbuf_at_assert(reply, start_ofs, sizeof *nfs);
        nfs->length = htons(reply->size - start_ofs);
        nfs->table_id = fs->table_id;
        nfs->pad = 0;
        nfs->duration_sec = htonl(fs->duration_sec);
        nfs->duration_nsec = htonl(fs->duration_nsec);
        nfs->priority = htons(fs->priority);
        nfs->idle_timeout = htons(fs->idle_timeout);
        nfs->hard_timeout = htons(fs->hard_timeout);
        nfs->idle_age = htons(fs->idle_age < 0 ? 0
                              : fs->idle_age < UINT16_MAX ? fs->idle_age + 1
                              : UINT16_MAX);
        nfs->hard_age = htons(fs->hard_age < 0 ? 0
                              : fs->hard_age < UINT16_MAX ? fs->hard_age + 1
                              : UINT16_MAX);
        nfs->match_len = htons(match_len);
        nfs->cookie = fs->cookie;
        nfs->packet_count = htonll(fs->packet_count);
        nfs->byte_count = htonll(fs->byte_count);
    } else {
        NOT_REACHED();
    }

    ofpmp_postappend(replies, start_ofs);
}

/* Converts abstract ofputil_aggregate_stats 'stats' into an OFPST_AGGREGATE or
 * NXST_AGGREGATE reply matching 'request', and returns the message. */
struct ofpbuf *
ofputil_encode_aggregate_stats_reply(
    const struct ofputil_aggregate_stats *stats,
    const struct ofp_header *request)
{
    struct ofp_aggregate_stats_reply *asr;
    uint64_t packet_count;
    uint64_t byte_count;
    struct ofpbuf *msg;
    enum ofpraw raw;

    ofpraw_decode(&raw, request);
    if (raw == OFPRAW_OFPST10_AGGREGATE_REQUEST) {
        packet_count = unknown_to_zero(stats->packet_count);
        byte_count = unknown_to_zero(stats->byte_count);
    } else {
        packet_count = stats->packet_count;
        byte_count = stats->byte_count;
    }

    msg = ofpraw_alloc_stats_reply(request, 0);
    asr = ofpbuf_put_zeros(msg, sizeof *asr);
    put_32aligned_be64(&asr->packet_count, htonll(packet_count));
    put_32aligned_be64(&asr->byte_count, htonll(byte_count));
    asr->flow_count = htonl(stats->flow_count);

    return msg;
}

enum ofperr
ofputil_decode_aggregate_stats_reply(struct ofputil_aggregate_stats *stats,
                                     const struct ofp_header *reply)
{
    struct ofp_aggregate_stats_reply *asr;
    struct ofpbuf msg;

    ofpbuf_use_const(&msg, reply, ntohs(reply->length));
    ofpraw_pull_assert(&msg);

    asr = msg.l3;
    stats->packet_count = ntohll(get_32aligned_be64(&asr->packet_count));
    stats->byte_count = ntohll(get_32aligned_be64(&asr->byte_count));
    stats->flow_count = ntohl(asr->flow_count);

    return 0;
}

/* Converts an OFPT_FLOW_REMOVED or NXT_FLOW_REMOVED message 'oh' into an
 * abstract ofputil_flow_removed in 'fr'.  Returns 0 if successful, otherwise
 * an OpenFlow error code. */
enum ofperr
ofputil_decode_flow_removed(struct ofputil_flow_removed *fr,
                            const struct ofp_header *oh)
{
    enum ofpraw raw;
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    if (raw == OFPRAW_OFPT11_FLOW_REMOVED) {
        const struct ofp12_flow_removed *ofr;
        enum ofperr error;

        ofr = ofpbuf_pull(&b, sizeof *ofr);

        error = ofputil_pull_ofp11_match(&b, &fr->match, NULL);
        if (error) {
            return error;
        }

        fr->priority = ntohs(ofr->priority);
        fr->cookie = ofr->cookie;
        fr->reason = ofr->reason;
        fr->table_id = ofr->table_id;
        fr->duration_sec = ntohl(ofr->duration_sec);
        fr->duration_nsec = ntohl(ofr->duration_nsec);
        fr->idle_timeout = ntohs(ofr->idle_timeout);
        fr->hard_timeout = ntohs(ofr->hard_timeout);
        fr->packet_count = ntohll(ofr->packet_count);
        fr->byte_count = ntohll(ofr->byte_count);
    } else if (raw == OFPRAW_OFPT10_FLOW_REMOVED) {
        const struct ofp10_flow_removed *ofr;

        ofr = ofpbuf_pull(&b, sizeof *ofr);

        ofputil_match_from_ofp10_match(&ofr->match, &fr->match);
        fr->priority = ntohs(ofr->priority);
        fr->cookie = ofr->cookie;
        fr->reason = ofr->reason;
        fr->table_id = 255;
        fr->duration_sec = ntohl(ofr->duration_sec);
        fr->duration_nsec = ntohl(ofr->duration_nsec);
        fr->idle_timeout = ntohs(ofr->idle_timeout);
        fr->hard_timeout = 0;
        fr->packet_count = ntohll(ofr->packet_count);
        fr->byte_count = ntohll(ofr->byte_count);
    } else if (raw == OFPRAW_NXT_FLOW_REMOVED) {
        struct nx_flow_removed *nfr;
        enum ofperr error;

        nfr = ofpbuf_pull(&b, sizeof *nfr);
        error = nx_pull_match(&b, ntohs(nfr->match_len), &fr->match,
                              NULL, NULL);
        if (error) {
            return error;
        }
        if (b.size) {
            return OFPERR_OFPBRC_BAD_LEN;
        }

        fr->priority = ntohs(nfr->priority);
        fr->cookie = nfr->cookie;
        fr->reason = nfr->reason;
        fr->table_id = 255;
        fr->duration_sec = ntohl(nfr->duration_sec);
        fr->duration_nsec = ntohl(nfr->duration_nsec);
        fr->idle_timeout = ntohs(nfr->idle_timeout);
        fr->hard_timeout = 0;
        fr->packet_count = ntohll(nfr->packet_count);
        fr->byte_count = ntohll(nfr->byte_count);
    } else {
        NOT_REACHED();
    }

    return 0;
}

/* Converts abstract ofputil_flow_removed 'fr' into an OFPT_FLOW_REMOVED or
 * NXT_FLOW_REMOVED message 'oh' according to 'protocol', and returns the
 * message. */
struct ofpbuf *
ofputil_encode_flow_removed(const struct ofputil_flow_removed *fr,
                            enum ofputil_protocol protocol)
{
    struct ofpbuf *msg;

    switch (protocol) {
    case OFPUTIL_P_OF12_OXM:
    case OFPUTIL_P_OF13_OXM: {
        struct ofp12_flow_removed *ofr;

        msg = ofpraw_alloc_xid(OFPRAW_OFPT11_FLOW_REMOVED,
                               ofputil_protocol_to_ofp_version(protocol),
                               htonl(0), NXM_TYPICAL_LEN);
        ofr = ofpbuf_put_zeros(msg, sizeof *ofr);
        ofr->cookie = fr->cookie;
        ofr->priority = htons(fr->priority);
        ofr->reason = fr->reason;
        ofr->table_id = fr->table_id;
        ofr->duration_sec = htonl(fr->duration_sec);
        ofr->duration_nsec = htonl(fr->duration_nsec);
        ofr->idle_timeout = htons(fr->idle_timeout);
        ofr->hard_timeout = htons(fr->hard_timeout);
        ofr->packet_count = htonll(fr->packet_count);
        ofr->byte_count = htonll(fr->byte_count);
        oxm_put_match(msg, &fr->match);
        break;
    }

    case OFPUTIL_P_OF10_STD:
    case OFPUTIL_P_OF10_STD_TID: {
        struct ofp10_flow_removed *ofr;

        msg = ofpraw_alloc_xid(OFPRAW_OFPT10_FLOW_REMOVED, OFP10_VERSION,
                               htonl(0), 0);
        ofr = ofpbuf_put_zeros(msg, sizeof *ofr);
        ofputil_match_to_ofp10_match(&fr->match, &ofr->match);
        ofr->cookie = fr->cookie;
        ofr->priority = htons(fr->priority);
        ofr->reason = fr->reason;
        ofr->duration_sec = htonl(fr->duration_sec);
        ofr->duration_nsec = htonl(fr->duration_nsec);
        ofr->idle_timeout = htons(fr->idle_timeout);
        ofr->packet_count = htonll(unknown_to_zero(fr->packet_count));
        ofr->byte_count = htonll(unknown_to_zero(fr->byte_count));
        break;
    }

    case OFPUTIL_P_OF10_NXM:
    case OFPUTIL_P_OF10_NXM_TID: {
        struct nx_flow_removed *nfr;
        int match_len;

        msg = ofpraw_alloc_xid(OFPRAW_NXT_FLOW_REMOVED, OFP10_VERSION,
                               htonl(0), NXM_TYPICAL_LEN);
        nfr = ofpbuf_put_zeros(msg, sizeof *nfr);
        match_len = nx_put_match(msg, &fr->match, 0, 0);

        nfr = msg->l3;
        nfr->cookie = fr->cookie;
        nfr->priority = htons(fr->priority);
        nfr->reason = fr->reason;
        nfr->duration_sec = htonl(fr->duration_sec);
        nfr->duration_nsec = htonl(fr->duration_nsec);
        nfr->idle_timeout = htons(fr->idle_timeout);
        nfr->match_len = htons(match_len);
        nfr->packet_count = htonll(fr->packet_count);
        nfr->byte_count = htonll(fr->byte_count);
        break;
    }

    default:
        NOT_REACHED();
    }

    return msg;
}

static void
ofputil_decode_packet_in_finish(struct ofputil_packet_in *pin,
                                struct match *match, struct ofpbuf *b)
{
    pin->packet = b->data;
    pin->packet_len = b->size;

    pin->fmd.in_port = match->flow.in_port;
    pin->fmd.tun_id = match->flow.tunnel.tun_id;
    pin->fmd.metadata = match->flow.metadata;
    memcpy(pin->fmd.regs, match->flow.regs, sizeof pin->fmd.regs);
}

enum ofperr
ofputil_decode_packet_in(struct ofputil_packet_in *pin,
                         const struct ofp_header *oh)
{
    enum ofpraw raw;
    struct ofpbuf b;

    memset(pin, 0, sizeof *pin);

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    if (raw == OFPRAW_OFPT13_PACKET_IN || raw == OFPRAW_OFPT12_PACKET_IN) {
        const struct ofp13_packet_in *opi;
        struct match match;
        int error;
        size_t packet_in_size;

        if (raw == OFPRAW_OFPT12_PACKET_IN) {
            packet_in_size = sizeof (struct ofp12_packet_in);
        } else {
            packet_in_size = sizeof (struct ofp13_packet_in);
        }

        opi = ofpbuf_pull(&b, packet_in_size);
        error = oxm_pull_match_loose(&b, &match);
        if (error) {
            return error;
        }

        if (!ofpbuf_try_pull(&b, 2)) {
            return OFPERR_OFPBRC_BAD_LEN;
        }

        pin->reason = opi->pi.reason;
        pin->table_id = opi->pi.table_id;
        pin->buffer_id = ntohl(opi->pi.buffer_id);
        pin->total_len = ntohs(opi->pi.total_len);

        if (raw == OFPRAW_OFPT13_PACKET_IN) {
            pin->cookie = opi->cookie;
        }

        ofputil_decode_packet_in_finish(pin, &match, &b);
    } else if (raw == OFPRAW_OFPT10_PACKET_IN) {
        const struct ofp10_packet_in *opi;

        opi = ofpbuf_pull(&b, offsetof(struct ofp10_packet_in, data));

        pin->packet = opi->data;
        pin->packet_len = b.size;

        pin->fmd.in_port = ntohs(opi->in_port);
        pin->reason = opi->reason;
        pin->buffer_id = ntohl(opi->buffer_id);
        pin->total_len = ntohs(opi->total_len);
    } else if (raw == OFPRAW_NXT_PACKET_IN) {
        const struct nx_packet_in *npi;
        struct match match;
        int error;

        npi = ofpbuf_pull(&b, sizeof *npi);
        error = nx_pull_match_loose(&b, ntohs(npi->match_len), &match, NULL,
                                    NULL);
        if (error) {
            return error;
        }

        if (!ofpbuf_try_pull(&b, 2)) {
            return OFPERR_OFPBRC_BAD_LEN;
        }

        pin->reason = npi->reason;
        pin->table_id = npi->table_id;
        pin->cookie = npi->cookie;

        pin->buffer_id = ntohl(npi->buffer_id);
        pin->total_len = ntohs(npi->total_len);

        ofputil_decode_packet_in_finish(pin, &match, &b);
    } else {
        NOT_REACHED();
    }

    return 0;
}

static void
ofputil_packet_in_to_match(const struct ofputil_packet_in *pin,
                           struct match *match)
{
    int i;

    match_init_catchall(match);
    if (pin->fmd.tun_id != htonll(0)) {
        match_set_tun_id(match, pin->fmd.tun_id);
    }
    if (pin->fmd.metadata != htonll(0)) {
        match_set_metadata(match, pin->fmd.metadata);
    }

    for (i = 0; i < FLOW_N_REGS; i++) {
        if (pin->fmd.regs[i]) {
            match_set_reg(match, i, pin->fmd.regs[i]);
        }
    }

    match_set_in_port(match, pin->fmd.in_port);
}

/* Converts abstract ofputil_packet_in 'pin' into a PACKET_IN message
 * in the format specified by 'packet_in_format'.  */
struct ofpbuf *
ofputil_encode_packet_in(const struct ofputil_packet_in *pin,
                         enum ofputil_protocol protocol,
                         enum nx_packet_in_format packet_in_format)
{
    size_t send_len = MIN(pin->send_len, pin->packet_len);
    struct ofpbuf *packet;

    /* Add OFPT_PACKET_IN. */
    if (protocol == OFPUTIL_P_OF13_OXM || protocol == OFPUTIL_P_OF12_OXM) {
        struct ofp13_packet_in *opi;
        struct match match;
        enum ofpraw packet_in_raw;
        enum ofp_version packet_in_version;
        size_t packet_in_size;

        if (protocol == OFPUTIL_P_OF12_OXM) {
            packet_in_raw = OFPRAW_OFPT12_PACKET_IN;
            packet_in_version = OFP12_VERSION;
            packet_in_size = sizeof (struct ofp12_packet_in);
        } else {
            packet_in_raw = OFPRAW_OFPT13_PACKET_IN;
            packet_in_version = OFP13_VERSION;
            packet_in_size = sizeof (struct ofp13_packet_in);
        }

        ofputil_packet_in_to_match(pin, &match);

        /* The final argument is just an estimate of the space required. */
        packet = ofpraw_alloc_xid(packet_in_raw, packet_in_version,
                                  htonl(0), (sizeof(struct flow_metadata) * 2
                                             + 2 + send_len));
        ofpbuf_put_zeros(packet, packet_in_size);
        oxm_put_match(packet, &match);
        ofpbuf_put_zeros(packet, 2);
        ofpbuf_put(packet, pin->packet, send_len);

        opi = packet->l3;
        opi->pi.buffer_id = htonl(pin->buffer_id);
        opi->pi.total_len = htons(pin->total_len);
        opi->pi.reason = pin->reason;
        opi->pi.table_id = pin->table_id;
        if (protocol == OFPUTIL_P_OF13_OXM) {
            opi->cookie = pin->cookie;
        }
    } else if (packet_in_format == NXPIF_OPENFLOW10) {
        struct ofp10_packet_in *opi;

        packet = ofpraw_alloc_xid(OFPRAW_OFPT10_PACKET_IN, OFP10_VERSION,
                                  htonl(0), send_len);
        opi = ofpbuf_put_zeros(packet, offsetof(struct ofp10_packet_in, data));
        opi->total_len = htons(pin->total_len);
        opi->in_port = htons(pin->fmd.in_port);
        opi->reason = pin->reason;
        opi->buffer_id = htonl(pin->buffer_id);

        ofpbuf_put(packet, pin->packet, send_len);
    } else if (packet_in_format == NXPIF_NXM) {
        struct nx_packet_in *npi;
        struct match match;
        size_t match_len;

        ofputil_packet_in_to_match(pin, &match);

        /* The final argument is just an estimate of the space required. */
        packet = ofpraw_alloc_xid(OFPRAW_NXT_PACKET_IN, OFP10_VERSION,
                                  htonl(0), (sizeof(struct flow_metadata) * 2
                                             + 2 + send_len));
        ofpbuf_put_zeros(packet, sizeof *npi);
        match_len = nx_put_match(packet, &match, 0, 0);
        ofpbuf_put_zeros(packet, 2);
        ofpbuf_put(packet, pin->packet, send_len);

        npi = packet->l3;
        npi->buffer_id = htonl(pin->buffer_id);
        npi->total_len = htons(pin->total_len);
        npi->reason = pin->reason;
        npi->table_id = pin->table_id;
        npi->cookie = pin->cookie;
        npi->match_len = htons(match_len);
    } else {
        NOT_REACHED();
    }
    ofpmsg_update_length(packet);

    return packet;
}

const char *
ofputil_packet_in_reason_to_string(enum ofp_packet_in_reason reason)
{
    static char s[INT_STRLEN(int) + 1];

    switch (reason) {
    case OFPR_NO_MATCH:
        return "no_match";
    case OFPR_ACTION:
        return "action";
    case OFPR_INVALID_TTL:
        return "invalid_ttl";

    case OFPR_N_REASONS:
    default:
        sprintf(s, "%d", (int) reason);
        return s;
    }
}

bool
ofputil_packet_in_reason_from_string(const char *s,
                                     enum ofp_packet_in_reason *reason)
{
    int i;

    for (i = 0; i < OFPR_N_REASONS; i++) {
        if (!strcasecmp(s, ofputil_packet_in_reason_to_string(i))) {
            *reason = i;
            return true;
        }
    }
    return false;
}

/* Converts an OFPT_PACKET_OUT in 'opo' into an abstract ofputil_packet_out in
 * 'po'.
 *
 * Uses 'ofpacts' to store the abstract OFPACT_* version of the packet out
 * message's actions.  The caller must initialize 'ofpacts' and retains
 * ownership of it.  'po->ofpacts' will point into the 'ofpacts' buffer.
 *
 * Returns 0 if successful, otherwise an OFPERR_* value. */
enum ofperr
ofputil_decode_packet_out(struct ofputil_packet_out *po,
                          const struct ofp_header *oh,
                          struct ofpbuf *ofpacts)
{
    enum ofpraw raw;
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);

    if (raw == OFPRAW_OFPT11_PACKET_OUT) {
        enum ofperr error;
        const struct ofp11_packet_out *opo = ofpbuf_pull(&b, sizeof *opo);

        po->buffer_id = ntohl(opo->buffer_id);
        error = ofputil_port_from_ofp11(opo->in_port, &po->in_port);
        if (error) {
            return error;
        }

        error = ofpacts_pull_openflow11_actions(&b, ntohs(opo->actions_len),
                                                ofpacts);
        if (error) {
            return error;
        }
    } else if (raw == OFPRAW_OFPT10_PACKET_OUT) {
        enum ofperr error;
        const struct ofp10_packet_out *opo = ofpbuf_pull(&b, sizeof *opo);

        po->buffer_id = ntohl(opo->buffer_id);
        po->in_port = ntohs(opo->in_port);

        error = ofpacts_pull_openflow10(&b, ntohs(opo->actions_len), ofpacts);
        if (error) {
            return error;
        }
    } else {
        NOT_REACHED();
    }

    if (po->in_port >= OFPP_MAX && po->in_port != OFPP_LOCAL
        && po->in_port != OFPP_NONE && po->in_port != OFPP_CONTROLLER) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "packet-out has bad input port %#"PRIx16,
                     po->in_port);
        return OFPERR_OFPBRC_BAD_PORT;
    }

    po->ofpacts = ofpacts->data;
    po->ofpacts_len = ofpacts->size;

    if (po->buffer_id == UINT32_MAX) {
        po->packet = b.data;
        po->packet_len = b.size;
    } else {
        po->packet = NULL;
        po->packet_len = 0;
    }

    return 0;
}

/* ofputil_phy_port */

/* NETDEV_F_* to and from OFPPF_* and OFPPF10_*. */
BUILD_ASSERT_DECL((int) NETDEV_F_10MB_HD    == OFPPF_10MB_HD);  /* bit 0 */
BUILD_ASSERT_DECL((int) NETDEV_F_10MB_FD    == OFPPF_10MB_FD);  /* bit 1 */
BUILD_ASSERT_DECL((int) NETDEV_F_100MB_HD   == OFPPF_100MB_HD); /* bit 2 */
BUILD_ASSERT_DECL((int) NETDEV_F_100MB_FD   == OFPPF_100MB_FD); /* bit 3 */
BUILD_ASSERT_DECL((int) NETDEV_F_1GB_HD     == OFPPF_1GB_HD);   /* bit 4 */
BUILD_ASSERT_DECL((int) NETDEV_F_1GB_FD     == OFPPF_1GB_FD);   /* bit 5 */
BUILD_ASSERT_DECL((int) NETDEV_F_10GB_FD    == OFPPF_10GB_FD);  /* bit 6 */

/* NETDEV_F_ bits 11...15 are OFPPF10_ bits 7...11: */
BUILD_ASSERT_DECL((int) NETDEV_F_COPPER == (OFPPF10_COPPER << 4));
BUILD_ASSERT_DECL((int) NETDEV_F_FIBER == (OFPPF10_FIBER << 4));
BUILD_ASSERT_DECL((int) NETDEV_F_AUTONEG == (OFPPF10_AUTONEG << 4));
BUILD_ASSERT_DECL((int) NETDEV_F_PAUSE == (OFPPF10_PAUSE << 4));
BUILD_ASSERT_DECL((int) NETDEV_F_PAUSE_ASYM == (OFPPF10_PAUSE_ASYM << 4));

static enum netdev_features
netdev_port_features_from_ofp10(ovs_be32 ofp10_)
{
    uint32_t ofp10 = ntohl(ofp10_);
    return (ofp10 & 0x7f) | ((ofp10 & 0xf80) << 4);
}

static ovs_be32
netdev_port_features_to_ofp10(enum netdev_features features)
{
    return htonl((features & 0x7f) | ((features & 0xf800) >> 4));
}

BUILD_ASSERT_DECL((int) NETDEV_F_10MB_HD    == OFPPF_10MB_HD);     /* bit 0 */
BUILD_ASSERT_DECL((int) NETDEV_F_10MB_FD    == OFPPF_10MB_FD);     /* bit 1 */
BUILD_ASSERT_DECL((int) NETDEV_F_100MB_HD   == OFPPF_100MB_HD);    /* bit 2 */
BUILD_ASSERT_DECL((int) NETDEV_F_100MB_FD   == OFPPF_100MB_FD);    /* bit 3 */
BUILD_ASSERT_DECL((int) NETDEV_F_1GB_HD     == OFPPF_1GB_HD);      /* bit 4 */
BUILD_ASSERT_DECL((int) NETDEV_F_1GB_FD     == OFPPF_1GB_FD);      /* bit 5 */
BUILD_ASSERT_DECL((int) NETDEV_F_10GB_FD    == OFPPF_10GB_FD);     /* bit 6 */
BUILD_ASSERT_DECL((int) NETDEV_F_40GB_FD    == OFPPF11_40GB_FD);   /* bit 7 */
BUILD_ASSERT_DECL((int) NETDEV_F_100GB_FD   == OFPPF11_100GB_FD);  /* bit 8 */
BUILD_ASSERT_DECL((int) NETDEV_F_1TB_FD     == OFPPF11_1TB_FD);    /* bit 9 */
BUILD_ASSERT_DECL((int) NETDEV_F_OTHER      == OFPPF11_OTHER);     /* bit 10 */
BUILD_ASSERT_DECL((int) NETDEV_F_COPPER     == OFPPF11_COPPER);    /* bit 11 */
BUILD_ASSERT_DECL((int) NETDEV_F_FIBER      == OFPPF11_FIBER);     /* bit 12 */
BUILD_ASSERT_DECL((int) NETDEV_F_AUTONEG    == OFPPF11_AUTONEG);   /* bit 13 */
BUILD_ASSERT_DECL((int) NETDEV_F_PAUSE      == OFPPF11_PAUSE);     /* bit 14 */
BUILD_ASSERT_DECL((int) NETDEV_F_PAUSE_ASYM == OFPPF11_PAUSE_ASYM);/* bit 15 */

static enum netdev_features
netdev_port_features_from_ofp11(ovs_be32 ofp11)
{
    return ntohl(ofp11) & 0xffff;
}

static ovs_be32
netdev_port_features_to_ofp11(enum netdev_features features)
{
    return htonl(features & 0xffff);
}

static enum ofperr
ofputil_decode_ofp10_phy_port(struct ofputil_phy_port *pp,
                              const struct ofp10_phy_port *opp)
{
    memset(pp, 0, sizeof *pp);

    pp->port_no = ntohs(opp->port_no);
    memcpy(pp->hw_addr, opp->hw_addr, OFP_ETH_ALEN);
    ovs_strlcpy(pp->name, opp->name, OFP_MAX_PORT_NAME_LEN);

    pp->config = ntohl(opp->config) & OFPPC10_ALL;
    pp->state = ntohl(opp->state) & OFPPS10_ALL;

    pp->curr = netdev_port_features_from_ofp10(opp->curr);
    pp->advertised = netdev_port_features_from_ofp10(opp->advertised);
    pp->supported = netdev_port_features_from_ofp10(opp->supported);
    pp->peer = netdev_port_features_from_ofp10(opp->peer);

    pp->curr_speed = netdev_features_to_bps(pp->curr, 0) / 1000;
    pp->max_speed = netdev_features_to_bps(pp->supported, 0) / 1000;

    return 0;
}

static enum ofperr
ofputil_decode_ofp11_port(struct ofputil_phy_port *pp,
                          const struct ofp11_port *op)
{
    enum ofperr error;

    memset(pp, 0, sizeof *pp);

    error = ofputil_port_from_ofp11(op->port_no, &pp->port_no);
    if (error) {
        return error;
    }
    memcpy(pp->hw_addr, op->hw_addr, OFP_ETH_ALEN);
    ovs_strlcpy(pp->name, op->name, OFP_MAX_PORT_NAME_LEN);

    pp->config = ntohl(op->config) & OFPPC11_ALL;
    pp->state = ntohl(op->state) & OFPPC11_ALL;

    pp->curr = netdev_port_features_from_ofp11(op->curr);
    pp->advertised = netdev_port_features_from_ofp11(op->advertised);
    pp->supported = netdev_port_features_from_ofp11(op->supported);
    pp->peer = netdev_port_features_from_ofp11(op->peer);

    pp->curr_speed = ntohl(op->curr_speed);
    pp->max_speed = ntohl(op->max_speed);

    return 0;
}

static size_t
ofputil_get_phy_port_size(enum ofp_version ofp_version)
{
    switch (ofp_version) {
    case OFP10_VERSION:
        return sizeof(struct ofp10_phy_port);
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION:
        return sizeof(struct ofp11_port);
    default:
        NOT_REACHED();
    }
}

static void
ofputil_encode_ofp10_phy_port(const struct ofputil_phy_port *pp,
                              struct ofp10_phy_port *opp)
{
    memset(opp, 0, sizeof *opp);

    opp->port_no = htons(pp->port_no);
    memcpy(opp->hw_addr, pp->hw_addr, ETH_ADDR_LEN);
    ovs_strlcpy(opp->name, pp->name, OFP_MAX_PORT_NAME_LEN);

    opp->config = htonl(pp->config & OFPPC10_ALL);
    opp->state = htonl(pp->state & OFPPS10_ALL);

    opp->curr = netdev_port_features_to_ofp10(pp->curr);
    opp->advertised = netdev_port_features_to_ofp10(pp->advertised);
    opp->supported = netdev_port_features_to_ofp10(pp->supported);
    opp->peer = netdev_port_features_to_ofp10(pp->peer);
}

static void
ofputil_encode_ofp11_port(const struct ofputil_phy_port *pp,
                          struct ofp11_port *op)
{
    memset(op, 0, sizeof *op);

    op->port_no = ofputil_port_to_ofp11(pp->port_no);
    memcpy(op->hw_addr, pp->hw_addr, ETH_ADDR_LEN);
    ovs_strlcpy(op->name, pp->name, OFP_MAX_PORT_NAME_LEN);

    op->config = htonl(pp->config & OFPPC11_ALL);
    op->state = htonl(pp->state & OFPPS11_ALL);

    op->curr = netdev_port_features_to_ofp11(pp->curr);
    op->advertised = netdev_port_features_to_ofp11(pp->advertised);
    op->supported = netdev_port_features_to_ofp11(pp->supported);
    op->peer = netdev_port_features_to_ofp11(pp->peer);

    op->curr_speed = htonl(pp->curr_speed);
    op->max_speed = htonl(pp->max_speed);
}

static void
ofputil_put_phy_port(enum ofp_version ofp_version,
                     const struct ofputil_phy_port *pp, struct ofpbuf *b)
{
    switch (ofp_version) {
    case OFP10_VERSION: {
        struct ofp10_phy_port *opp;
        if (b->size + sizeof *opp <= UINT16_MAX) {
            opp = ofpbuf_put_uninit(b, sizeof *opp);
            ofputil_encode_ofp10_phy_port(pp, opp);
        }
        break;
    }

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_port *op;
        if (b->size + sizeof *op <= UINT16_MAX) {
            op = ofpbuf_put_uninit(b, sizeof *op);
            ofputil_encode_ofp11_port(pp, op);
        }
        break;
    }

    default:
        NOT_REACHED();
    }
}

void
ofputil_append_port_desc_stats_reply(enum ofp_version ofp_version,
                                     const struct ofputil_phy_port *pp,
                                     struct list *replies)
{
    switch (ofp_version) {
    case OFP10_VERSION: {
        struct ofp10_phy_port *opp;

        opp = ofpmp_append(replies, sizeof *opp);
        ofputil_encode_ofp10_phy_port(pp, opp);
        break;
    }

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_port *op;

        op = ofpmp_append(replies, sizeof *op);
        ofputil_encode_ofp11_port(pp, op);
        break;
    }

    default:
      NOT_REACHED();
    }
}

/* ofputil_switch_features */

#define OFPC_COMMON (OFPC_FLOW_STATS | OFPC_TABLE_STATS | OFPC_PORT_STATS | \
                     OFPC_IP_REASM | OFPC_QUEUE_STATS)
BUILD_ASSERT_DECL((int) OFPUTIL_C_FLOW_STATS == OFPC_FLOW_STATS);
BUILD_ASSERT_DECL((int) OFPUTIL_C_TABLE_STATS == OFPC_TABLE_STATS);
BUILD_ASSERT_DECL((int) OFPUTIL_C_PORT_STATS == OFPC_PORT_STATS);
BUILD_ASSERT_DECL((int) OFPUTIL_C_IP_REASM == OFPC_IP_REASM);
BUILD_ASSERT_DECL((int) OFPUTIL_C_QUEUE_STATS == OFPC_QUEUE_STATS);
BUILD_ASSERT_DECL((int) OFPUTIL_C_ARP_MATCH_IP == OFPC_ARP_MATCH_IP);

struct ofputil_action_bit_translation {
    enum ofputil_action_bitmap ofputil_bit;
    int of_bit;
};

static const struct ofputil_action_bit_translation of10_action_bits[] = {
    { OFPUTIL_A_OUTPUT,       OFPAT10_OUTPUT },
    { OFPUTIL_A_SET_VLAN_VID, OFPAT10_SET_VLAN_VID },
    { OFPUTIL_A_SET_VLAN_PCP, OFPAT10_SET_VLAN_PCP },
    { OFPUTIL_A_STRIP_VLAN,   OFPAT10_STRIP_VLAN },
    { OFPUTIL_A_SET_DL_SRC,   OFPAT10_SET_DL_SRC },
    { OFPUTIL_A_SET_DL_DST,   OFPAT10_SET_DL_DST },
    { OFPUTIL_A_SET_NW_SRC,   OFPAT10_SET_NW_SRC },
    { OFPUTIL_A_SET_NW_DST,   OFPAT10_SET_NW_DST },
    { OFPUTIL_A_SET_NW_TOS,   OFPAT10_SET_NW_TOS },
    { OFPUTIL_A_SET_TP_SRC,   OFPAT10_SET_TP_SRC },
    { OFPUTIL_A_SET_TP_DST,   OFPAT10_SET_TP_DST },
    { OFPUTIL_A_ENQUEUE,      OFPAT10_ENQUEUE },
    { 0, 0 },
};

static enum ofputil_action_bitmap
decode_action_bits(ovs_be32 of_actions,
                   const struct ofputil_action_bit_translation *x)
{
    enum ofputil_action_bitmap ofputil_actions;

    ofputil_actions = 0;
    for (; x->ofputil_bit; x++) {
        if (of_actions & htonl(1u << x->of_bit)) {
            ofputil_actions |= x->ofputil_bit;
        }
    }
    return ofputil_actions;
}

static uint32_t
ofputil_capabilities_mask(enum ofp_version ofp_version)
{
    /* Handle capabilities whose bit is unique for all Open Flow versions */
    switch (ofp_version) {
    case OFP10_VERSION:
    case OFP11_VERSION:
        return OFPC_COMMON | OFPC_ARP_MATCH_IP;
    case OFP12_VERSION:
    case OFP13_VERSION:
        return OFPC_COMMON | OFPC12_PORT_BLOCKED;
    default:
        /* Caller needs to check osf->header.version itself */
        return 0;
    }
}

/* Decodes an OpenFlow 1.0 or 1.1 "switch_features" structure 'osf' into an
 * abstract representation in '*features'.  Initializes '*b' to iterate over
 * the OpenFlow port structures following 'osf' with later calls to
 * ofputil_pull_phy_port().  Returns 0 if successful, otherwise an
 * OFPERR_* value.  */
enum ofperr
ofputil_decode_switch_features(const struct ofp_header *oh,
                               struct ofputil_switch_features *features,
                               struct ofpbuf *b)
{
    const struct ofp_switch_features *osf;
    enum ofpraw raw;

    ofpbuf_use_const(b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(b);

    osf = ofpbuf_pull(b, sizeof *osf);
    features->datapath_id = ntohll(osf->datapath_id);
    features->n_buffers = ntohl(osf->n_buffers);
    features->n_tables = osf->n_tables;
    features->auxiliary_id = 0;

    features->capabilities = ntohl(osf->capabilities) &
        ofputil_capabilities_mask(oh->version);

    if (b->size % ofputil_get_phy_port_size(oh->version)) {
        return OFPERR_OFPBRC_BAD_LEN;
    }

    if (raw == OFPRAW_OFPT10_FEATURES_REPLY) {
        if (osf->capabilities & htonl(OFPC10_STP)) {
            features->capabilities |= OFPUTIL_C_STP;
        }
        features->actions = decode_action_bits(osf->actions, of10_action_bits);
    } else if (raw == OFPRAW_OFPT11_FEATURES_REPLY
               || raw == OFPRAW_OFPT13_FEATURES_REPLY) {
        if (osf->capabilities & htonl(OFPC11_GROUP_STATS)) {
            features->capabilities |= OFPUTIL_C_GROUP_STATS;
        }
        features->actions = 0;
        if (raw == OFPRAW_OFPT13_FEATURES_REPLY) {
            features->auxiliary_id = osf->auxiliary_id;
        }
    } else {
        return OFPERR_OFPBRC_BAD_VERSION;
    }

    return 0;
}

/* Returns true if the maximum number of ports are in 'oh'. */
static bool
max_ports_in_features(const struct ofp_header *oh)
{
    size_t pp_size = ofputil_get_phy_port_size(oh->version);
    return ntohs(oh->length) + pp_size > UINT16_MAX;
}

/* Given a buffer 'b' that contains a Features Reply message, checks if
 * it contains the maximum number of ports that will fit.  If so, it
 * returns true and removes the ports from the message.  The caller
 * should then send an OFPST_PORT_DESC stats request to get the ports,
 * since the switch may have more ports than could be represented in the
 * Features Reply.  Otherwise, returns false.
 */
bool
ofputil_switch_features_ports_trunc(struct ofpbuf *b)
{
    struct ofp_header *oh = b->data;

    if (max_ports_in_features(oh)) {
        /* Remove all the ports. */
        b->size = (sizeof(struct ofp_header)
                   + sizeof(struct ofp_switch_features));
        ofpmsg_update_length(b);

        return true;
    }

    return false;
}

static ovs_be32
encode_action_bits(enum ofputil_action_bitmap ofputil_actions,
                   const struct ofputil_action_bit_translation *x)
{
    uint32_t of_actions;

    of_actions = 0;
    for (; x->ofputil_bit; x++) {
        if (ofputil_actions & x->ofputil_bit) {
            of_actions |= 1 << x->of_bit;
        }
    }
    return htonl(of_actions);
}

/* Returns a buffer owned by the caller that encodes 'features' in the format
 * required by 'protocol' with the given 'xid'.  The caller should append port
 * information to the buffer with subsequent calls to
 * ofputil_put_switch_features_port(). */
struct ofpbuf *
ofputil_encode_switch_features(const struct ofputil_switch_features *features,
                               enum ofputil_protocol protocol, ovs_be32 xid)
{
    struct ofp_switch_features *osf;
    struct ofpbuf *b;
    enum ofp_version version;
    enum ofpraw raw;

    version = ofputil_protocol_to_ofp_version(protocol);
    switch (version) {
    case OFP10_VERSION:
        raw = OFPRAW_OFPT10_FEATURES_REPLY;
        break;
    case OFP11_VERSION:
    case OFP12_VERSION:
        raw = OFPRAW_OFPT11_FEATURES_REPLY;
        break;
    case OFP13_VERSION:
        raw = OFPRAW_OFPT13_FEATURES_REPLY;
        break;
    default:
        NOT_REACHED();
    }
    b = ofpraw_alloc_xid(raw, version, xid, 0);
    osf = ofpbuf_put_zeros(b, sizeof *osf);
    osf->datapath_id = htonll(features->datapath_id);
    osf->n_buffers = htonl(features->n_buffers);
    osf->n_tables = features->n_tables;

    osf->capabilities = htonl(features->capabilities & OFPC_COMMON);
    osf->capabilities = htonl(features->capabilities &
                              ofputil_capabilities_mask(version));
    switch (version) {
    case OFP10_VERSION:
        if (features->capabilities & OFPUTIL_C_STP) {
            osf->capabilities |= htonl(OFPC10_STP);
        }
        osf->actions = encode_action_bits(features->actions, of10_action_bits);
        break;
    case OFP13_VERSION:
        osf->auxiliary_id = features->auxiliary_id;
        /* fall through */
    case OFP11_VERSION:
    case OFP12_VERSION:
        if (features->capabilities & OFPUTIL_C_GROUP_STATS) {
            osf->capabilities |= htonl(OFPC11_GROUP_STATS);
        }
        break;
    default:
        NOT_REACHED();
    }

    return b;
}

/* Encodes 'pp' into the format required by the switch_features message already
 * in 'b', which should have been returned by ofputil_encode_switch_features(),
 * and appends the encoded version to 'b'. */
void
ofputil_put_switch_features_port(const struct ofputil_phy_port *pp,
                                 struct ofpbuf *b)
{
    const struct ofp_header *oh = b->data;

    if (oh->version < OFP13_VERSION) {
        ofputil_put_phy_port(oh->version, pp, b);
    }
}

/* ofputil_port_status */

/* Decodes the OpenFlow "port status" message in '*ops' into an abstract form
 * in '*ps'.  Returns 0 if successful, otherwise an OFPERR_* value. */
enum ofperr
ofputil_decode_port_status(const struct ofp_header *oh,
                           struct ofputil_port_status *ps)
{
    const struct ofp_port_status *ops;
    struct ofpbuf b;
    int retval;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    ofpraw_pull_assert(&b);
    ops = ofpbuf_pull(&b, sizeof *ops);

    if (ops->reason != OFPPR_ADD &&
        ops->reason != OFPPR_DELETE &&
        ops->reason != OFPPR_MODIFY) {
        return OFPERR_NXBRC_BAD_REASON;
    }
    ps->reason = ops->reason;

    retval = ofputil_pull_phy_port(oh->version, &b, &ps->desc);
    ovs_assert(retval != EOF);
    return retval;
}

/* Converts the abstract form of a "port status" message in '*ps' into an
 * OpenFlow message suitable for 'protocol', and returns that encoded form in
 * a buffer owned by the caller. */
struct ofpbuf *
ofputil_encode_port_status(const struct ofputil_port_status *ps,
                           enum ofputil_protocol protocol)
{
    struct ofp_port_status *ops;
    struct ofpbuf *b;
    enum ofp_version version;
    enum ofpraw raw;

    version = ofputil_protocol_to_ofp_version(protocol);
    switch (version) {
    case OFP10_VERSION:
        raw = OFPRAW_OFPT10_PORT_STATUS;
        break;

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION:
        raw = OFPRAW_OFPT11_PORT_STATUS;
        break;

    default:
        NOT_REACHED();
    }

    b = ofpraw_alloc_xid(raw, version, htonl(0), 0);
    ops = ofpbuf_put_zeros(b, sizeof *ops);
    ops->reason = ps->reason;
    ofputil_put_phy_port(version, &ps->desc, b);
    ofpmsg_update_length(b);
    return b;
}

/* ofputil_port_mod */

/* Decodes the OpenFlow "port mod" message in '*oh' into an abstract form in
 * '*pm'.  Returns 0 if successful, otherwise an OFPERR_* value. */
enum ofperr
ofputil_decode_port_mod(const struct ofp_header *oh,
                        struct ofputil_port_mod *pm)
{
    enum ofpraw raw;
    struct ofpbuf b;
    uint32_t all_check_bit;
    
    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);

    if (raw == OFPRAW_OFPT10_PORT_MOD) {
        const struct ofp10_port_mod *opm = b.data;

        pm->port_no = ntohs(opm->port_no);
        memcpy(pm->hw_addr, opm->hw_addr, ETH_ADDR_LEN);
#ifdef _OFP_CENTEC_
        OFPPC_PORT_CONFIG_CHECK(ntohl(opm->config), ntohl(opm->mask), OFPPC10_ALL);
#endif /*_OFP_CENTEC_*/
        pm->config = ntohl(opm->config) & OFPPC10_ALL;
        pm->mask = ntohl(opm->mask) & OFPPC10_ALL;
        pm->advertise = netdev_port_features_from_ofp10(opm->advertise);
    } else if (raw == OFPRAW_OFPT11_PORT_MOD) {
        const struct ofp11_port_mod *opm = b.data;
        enum ofperr error;

        error = ofputil_port_from_ofp11(opm->port_no, &pm->port_no);
        if (error) {
            return error;
        }

        memcpy(pm->hw_addr, opm->hw_addr, ETH_ADDR_LEN);
#ifdef _OFP_CENTEC_
        OFPPC_PORT_CONFIG_CHECK(ntohl(opm->config), ntohl(opm->mask), OFPPC11_ALL);
#endif /*_OFP_CENTEC_*/
        pm->config = ntohl(opm->config) & OFPPC11_ALL;
        pm->mask = ntohl(opm->mask) & OFPPC11_ALL;
        pm->advertise = netdev_port_features_from_ofp11(opm->advertise);
    } else {
        return OFPERR_OFPBRC_BAD_TYPE;
    }

    pm->config &= pm->mask;
    return 0;
}

/* Converts the abstract form of a "port mod" message in '*pm' into an OpenFlow
 * message suitable for 'protocol', and returns that encoded form in a buffer
 * owned by the caller. */
struct ofpbuf *
ofputil_encode_port_mod(const struct ofputil_port_mod *pm,
                        enum ofputil_protocol protocol)
{
    enum ofp_version ofp_version = ofputil_protocol_to_ofp_version(protocol);
    struct ofpbuf *b;

    switch (ofp_version) {
    case OFP10_VERSION: {
        struct ofp10_port_mod *opm;

        b = ofpraw_alloc(OFPRAW_OFPT10_PORT_MOD, ofp_version, 0);
        opm = ofpbuf_put_zeros(b, sizeof *opm);
        opm->port_no = htons(pm->port_no);
        memcpy(opm->hw_addr, pm->hw_addr, ETH_ADDR_LEN);
        opm->config = htonl(pm->config & OFPPC10_ALL);
        opm->mask = htonl(pm->mask & OFPPC10_ALL);
        opm->advertise = netdev_port_features_to_ofp10(pm->advertise);
        break;
    }

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_port_mod *opm;

        b = ofpraw_alloc(OFPRAW_OFPT11_PORT_MOD, ofp_version, 0);
        opm = ofpbuf_put_zeros(b, sizeof *opm);
        opm->port_no = ofputil_port_to_ofp11(pm->port_no);
        memcpy(opm->hw_addr, pm->hw_addr, ETH_ADDR_LEN);
        opm->config = htonl(pm->config & OFPPC11_ALL);
        opm->mask = htonl(pm->mask & OFPPC11_ALL);
        opm->advertise = netdev_port_features_to_ofp11(pm->advertise);
        break;
    }

    default:
        NOT_REACHED();
    }

    return b;
}

/* ofputil_role_request */

/* Decodes the OpenFlow "role request" or "role reply" message in '*oh' into
 * an abstract form in '*rr'.  Returns 0 if successful, otherwise an
 * OFPERR_* value. */
enum ofperr
ofputil_decode_role_message(const struct ofp_header *oh,
                            struct ofputil_role_request *rr)
{
    struct ofpbuf b;
    enum ofpraw raw;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);

    if (raw == OFPRAW_OFPT12_ROLE_REQUEST ||
        raw == OFPRAW_OFPT12_ROLE_REPLY) {
        const struct ofp12_role_request *orr = b.l3;

        if (orr->role != htonl(OFPCR12_ROLE_NOCHANGE) &&
            orr->role != htonl(OFPCR12_ROLE_EQUAL) &&
            orr->role != htonl(OFPCR12_ROLE_MASTER) &&
            orr->role != htonl(OFPCR12_ROLE_SLAVE)) {
            return OFPERR_OFPRRFC_BAD_ROLE;
        }

        rr->role = ntohl(orr->role);
        if (raw == OFPRAW_OFPT12_ROLE_REQUEST
            ? orr->role == htonl(OFPCR12_ROLE_NOCHANGE)
            : orr->generation_id == htonll(UINT64_MAX)) {
            rr->have_generation_id = false;
            rr->generation_id = 0;
        } else {
            rr->have_generation_id = true;
            rr->generation_id = ntohll(orr->generation_id);
        }
    } else if (raw == OFPRAW_NXT_ROLE_REQUEST ||
               raw == OFPRAW_NXT_ROLE_REPLY) {
        const struct nx_role_request *nrr = b.l3;

        BUILD_ASSERT(NX_ROLE_OTHER + 1 == OFPCR12_ROLE_EQUAL);
        BUILD_ASSERT(NX_ROLE_MASTER + 1 == OFPCR12_ROLE_MASTER);
        BUILD_ASSERT(NX_ROLE_SLAVE + 1 == OFPCR12_ROLE_SLAVE);

        if (nrr->role != htonl(NX_ROLE_OTHER) &&
            nrr->role != htonl(NX_ROLE_MASTER) &&
            nrr->role != htonl(NX_ROLE_SLAVE)) {
            return OFPERR_OFPRRFC_BAD_ROLE;
        }

        rr->role = ntohl(nrr->role) + 1;
        rr->have_generation_id = false;
        rr->generation_id = 0;
    } else {
        NOT_REACHED();
    }

    return 0;
}

/* Returns an encoded form of a role reply suitable for the "request" in a
 * buffer owned by the caller. */
struct ofpbuf *
ofputil_encode_role_reply(const struct ofp_header *request,
                          const struct ofputil_role_request *rr)
{
    struct ofpbuf *buf;
    enum ofpraw raw;

    raw = ofpraw_decode_assert(request);
    if (raw == OFPRAW_OFPT12_ROLE_REQUEST) {
        struct ofp12_role_request *orr;

        buf = ofpraw_alloc_reply(OFPRAW_OFPT12_ROLE_REPLY, request, 0);
        orr = ofpbuf_put_zeros(buf, sizeof *orr);

        orr->role = htonl(rr->role);
        orr->generation_id = htonll(rr->have_generation_id
                                    ? rr->generation_id
                                    : UINT64_MAX);
    } else if (raw == OFPRAW_NXT_ROLE_REQUEST) {
        struct nx_role_request *nrr;

        BUILD_ASSERT(NX_ROLE_OTHER == OFPCR12_ROLE_EQUAL - 1);
        BUILD_ASSERT(NX_ROLE_MASTER == OFPCR12_ROLE_MASTER - 1);
        BUILD_ASSERT(NX_ROLE_SLAVE == OFPCR12_ROLE_SLAVE - 1);

        buf = ofpraw_alloc_reply(OFPRAW_NXT_ROLE_REPLY, request, 0);
        nrr = ofpbuf_put_zeros(buf, sizeof *nrr);
        nrr->role = htonl(rr->role - 1);
    } else {
        NOT_REACHED();
    }

    return buf;
}

/* Table stats. */

static void
ofputil_put_ofp10_table_stats(const struct ofp12_table_stats *in,
                              struct ofpbuf *buf)
{
    struct wc_map {
        enum ofp10_flow_wildcards wc10;
        enum oxm12_ofb_match_fields mf12;
    };

    static const struct wc_map wc_map[] = {
        { OFPFW10_IN_PORT,     OFPXMT12_OFB_IN_PORT },
        { OFPFW10_DL_VLAN,     OFPXMT12_OFB_VLAN_VID },
        { OFPFW10_DL_SRC,      OFPXMT12_OFB_ETH_SRC },
        { OFPFW10_DL_DST,      OFPXMT12_OFB_ETH_DST},
        { OFPFW10_DL_TYPE,     OFPXMT12_OFB_ETH_TYPE },
        { OFPFW10_NW_PROTO,    OFPXMT12_OFB_IP_PROTO },
        { OFPFW10_TP_SRC,      OFPXMT12_OFB_TCP_SRC },
        { OFPFW10_TP_DST,      OFPXMT12_OFB_TCP_DST },
        { OFPFW10_NW_SRC_MASK, OFPXMT12_OFB_IPV4_SRC },
        { OFPFW10_NW_DST_MASK, OFPXMT12_OFB_IPV4_DST },
        { OFPFW10_DL_VLAN_PCP, OFPXMT12_OFB_VLAN_PCP },
        { OFPFW10_NW_TOS,      OFPXMT12_OFB_IP_DSCP },
    };

    struct ofp10_table_stats *out;
    const struct wc_map *p;

    out = ofpbuf_put_zeros(buf, sizeof *out);
    out->table_id = in->table_id;
    ovs_strlcpy(out->name, in->name, sizeof out->name);
    out->wildcards = 0;
    for (p = wc_map; p < &wc_map[ARRAY_SIZE(wc_map)]; p++) {
        if (in->wildcards & htonll(1ULL << p->mf12)) {
            out->wildcards |= htonl(p->wc10);
        }
    }
    out->max_entries = in->max_entries;
    out->active_count = in->active_count;
    put_32aligned_be64(&out->lookup_count, in->lookup_count);
    put_32aligned_be64(&out->matched_count, in->matched_count);
}

static ovs_be32
oxm12_to_ofp11_flow_match_fields(ovs_be64 oxm12)
{
    struct map {
        enum ofp11_flow_match_fields fmf11;
        enum oxm12_ofb_match_fields mf12;
    };

    static const struct map map[] = {
        { OFPFMF11_IN_PORT,     OFPXMT12_OFB_IN_PORT },
        { OFPFMF11_DL_VLAN,     OFPXMT12_OFB_VLAN_VID },
        { OFPFMF11_DL_VLAN_PCP, OFPXMT12_OFB_VLAN_PCP },
        { OFPFMF11_DL_TYPE,     OFPXMT12_OFB_ETH_TYPE },
        { OFPFMF11_NW_TOS,      OFPXMT12_OFB_IP_DSCP },
        { OFPFMF11_NW_PROTO,    OFPXMT12_OFB_IP_PROTO },
        { OFPFMF11_TP_SRC,      OFPXMT12_OFB_TCP_SRC },
        { OFPFMF11_TP_DST,      OFPXMT12_OFB_TCP_DST },
        { OFPFMF11_MPLS_LABEL,  OFPXMT12_OFB_MPLS_LABEL },
        { OFPFMF11_MPLS_TC,     OFPXMT12_OFB_MPLS_TC },
        /* I don't know what OFPFMF11_TYPE means. */
        { OFPFMF11_DL_SRC,      OFPXMT12_OFB_ETH_SRC },
        { OFPFMF11_DL_DST,      OFPXMT12_OFB_ETH_DST },
        { OFPFMF11_NW_SRC,      OFPXMT12_OFB_IPV4_SRC },
        { OFPFMF11_NW_DST,      OFPXMT12_OFB_IPV4_DST },
        { OFPFMF11_METADATA,    OFPXMT12_OFB_METADATA },
    };

    const struct map *p;
    uint32_t fmf11;

    fmf11 = 0;
    for (p = map; p < &map[ARRAY_SIZE(map)]; p++) {
        if (oxm12 & htonll(1ULL << p->mf12)) {
            fmf11 |= p->fmf11;
        }
    }
    return htonl(fmf11);
}

static void
ofputil_put_ofp11_table_stats(const struct ofp12_table_stats *in,
                              struct ofpbuf *buf)
{
    struct ofp11_table_stats *out;

    out = ofpbuf_put_zeros(buf, sizeof *out);
    out->table_id = in->table_id;
    ovs_strlcpy(out->name, in->name, sizeof out->name);
    out->wildcards = oxm12_to_ofp11_flow_match_fields(in->wildcards);
    out->match = oxm12_to_ofp11_flow_match_fields(in->match);
    out->instructions = in->instructions;
    out->write_actions = in->write_actions;
    out->apply_actions = in->apply_actions;
    out->config = in->config;
    out->max_entries = in->max_entries;
    out->active_count = in->active_count;
    out->lookup_count = in->lookup_count;
    out->matched_count = in->matched_count;
}

static void
ofputil_put_ofp13_table_stats(const struct ofp12_table_stats *in,
                              struct ofpbuf *buf)
{
    struct ofp13_table_stats *out;

    /* OF 1.3 splits table features off the ofp_table_stats,
     * so there is not much here. */

    out = ofpbuf_put_uninit(buf, sizeof *out);
    out->table_id = in->table_id;
    out->active_count = in->active_count;
    out->lookup_count = in->lookup_count;
    out->matched_count = in->matched_count;
}

struct ofpbuf *
ofputil_encode_table_stats_reply(const struct ofp12_table_stats stats[], int n,
                                 const struct ofp_header *request)
{
    struct ofpbuf *reply;
    int i;

    reply = ofpraw_alloc_stats_reply(request, n * sizeof *stats);

    switch ((enum ofp_version) request->version) {
    case OFP10_VERSION:
        for (i = 0; i < n; i++) {
            ofputil_put_ofp10_table_stats(&stats[i], reply);
        }
        break;

    case OFP11_VERSION:
        for (i = 0; i < n; i++) {
            ofputil_put_ofp11_table_stats(&stats[i], reply);
        }
        break;

    case OFP12_VERSION:
        ofpbuf_put(reply, stats, n * sizeof *stats);
        break;

    case OFP13_VERSION:
        for (i = 0; i < n; i++) {
            ofputil_put_ofp13_table_stats(&stats[i], reply);
        }
        break;

    default:
        NOT_REACHED();
    }

    return reply;
}

/* ofputil_flow_monitor_request */

/* Converts an NXST_FLOW_MONITOR request in 'msg' into an abstract
 * ofputil_flow_monitor_request in 'rq'.
 *
 * Multiple NXST_FLOW_MONITOR requests can be packed into a single OpenFlow
 * message.  Calling this function multiple times for a single 'msg' iterates
 * through the requests.  The caller must initially leave 'msg''s layer
 * pointers null and not modify them between calls.
 *
 * Returns 0 if successful, EOF if no requests were left in this 'msg',
 * otherwise an OFPERR_* value. */
int
ofputil_decode_flow_monitor_request(struct ofputil_flow_monitor_request *rq,
                                    struct ofpbuf *msg)
{
    struct nx_flow_monitor_request *nfmr;
    uint16_t flags;

    if (!msg->l2) {
        msg->l2 = msg->data;
        ofpraw_pull_assert(msg);
    }

    if (!msg->size) {
        return EOF;
    }

    nfmr = ofpbuf_try_pull(msg, sizeof *nfmr);
    if (!nfmr) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "NXST_FLOW_MONITOR request has %zu "
                     "leftover bytes at end", msg->size);
        return OFPERR_OFPBRC_BAD_LEN;
    }

    flags = ntohs(nfmr->flags);
    if (!(flags & (NXFMF_ADD | NXFMF_DELETE | NXFMF_MODIFY))
        || flags & ~(NXFMF_INITIAL | NXFMF_ADD | NXFMF_DELETE
                     | NXFMF_MODIFY | NXFMF_ACTIONS | NXFMF_OWN)) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "NXST_FLOW_MONITOR has bad flags %#"PRIx16,
                     flags);
        return OFPERR_NXBRC_FM_BAD_FLAGS;
    }

    if (!is_all_zeros(nfmr->zeros, sizeof nfmr->zeros)) {
        return OFPERR_NXBRC_MUST_BE_ZERO;
    }

    rq->id = ntohl(nfmr->id);
    rq->flags = flags;
    rq->out_port = ntohs(nfmr->out_port);
    rq->table_id = nfmr->table_id;

    return nx_pull_match(msg, ntohs(nfmr->match_len), &rq->match, NULL, NULL);
}

void
ofputil_append_flow_monitor_request(
    const struct ofputil_flow_monitor_request *rq, struct ofpbuf *msg)
{
    struct nx_flow_monitor_request *nfmr;
    size_t start_ofs;
    int match_len;

    if (!msg->size) {
        ofpraw_put(OFPRAW_NXST_FLOW_MONITOR_REQUEST, OFP10_VERSION, msg);
    }

    start_ofs = msg->size;
    ofpbuf_put_zeros(msg, sizeof *nfmr);
    match_len = nx_put_match(msg, &rq->match, htonll(0), htonll(0));

    nfmr = ofpbuf_at_assert(msg, start_ofs, sizeof *nfmr);
    nfmr->id = htonl(rq->id);
    nfmr->flags = htons(rq->flags);
    nfmr->out_port = htons(rq->out_port);
    nfmr->match_len = htons(match_len);
    nfmr->table_id = rq->table_id;
}

/* Converts an NXST_FLOW_MONITOR reply (also known as a flow update) in 'msg'
 * into an abstract ofputil_flow_update in 'update'.  The caller must have
 * initialized update->match to point to space allocated for a match.
 *
 * Uses 'ofpacts' to store the abstract OFPACT_* version of the update's
 * actions (except for NXFME_ABBREV, which never includes actions).  The caller
 * must initialize 'ofpacts' and retains ownership of it.  'update->ofpacts'
 * will point into the 'ofpacts' buffer.
 *
 * Multiple flow updates can be packed into a single OpenFlow message.  Calling
 * this function multiple times for a single 'msg' iterates through the
 * updates.  The caller must initially leave 'msg''s layer pointers null and
 * not modify them between calls.
 *
 * Returns 0 if successful, EOF if no updates were left in this 'msg',
 * otherwise an OFPERR_* value. */
int
ofputil_decode_flow_update(struct ofputil_flow_update *update,
                           struct ofpbuf *msg, struct ofpbuf *ofpacts)
{
    struct nx_flow_update_header *nfuh;
    unsigned int length;

    if (!msg->l2) {
        msg->l2 = msg->data;
        ofpraw_pull_assert(msg);
    }

    if (!msg->size) {
        return EOF;
    }

    if (msg->size < sizeof(struct nx_flow_update_header)) {
        goto bad_len;
    }

    nfuh = msg->data;
    update->event = ntohs(nfuh->event);
    length = ntohs(nfuh->length);
    if (length > msg->size || length % 8) {
        goto bad_len;
    }

    if (update->event == NXFME_ABBREV) {
        struct nx_flow_update_abbrev *nfua;

        if (length != sizeof *nfua) {
            goto bad_len;
        }

        nfua = ofpbuf_pull(msg, sizeof *nfua);
        update->xid = nfua->xid;
        return 0;
    } else if (update->event == NXFME_ADDED
               || update->event == NXFME_DELETED
               || update->event == NXFME_MODIFIED) {
        struct nx_flow_update_full *nfuf;
        unsigned int actions_len;
        unsigned int match_len;
        enum ofperr error;

        if (length < sizeof *nfuf) {
            goto bad_len;
        }

        nfuf = ofpbuf_pull(msg, sizeof *nfuf);
        match_len = ntohs(nfuf->match_len);
        if (sizeof *nfuf + match_len > length) {
            goto bad_len;
        }

        update->reason = ntohs(nfuf->reason);
        update->idle_timeout = ntohs(nfuf->idle_timeout);
        update->hard_timeout = ntohs(nfuf->hard_timeout);
        update->table_id = nfuf->table_id;
        update->cookie = nfuf->cookie;
        update->priority = ntohs(nfuf->priority);

        error = nx_pull_match(msg, match_len, update->match, NULL, NULL);
        if (error) {
            return error;
        }

        actions_len = length - sizeof *nfuf - ROUND_UP(match_len, 8);
        error = ofpacts_pull_openflow10(msg, actions_len, ofpacts);
        if (error) {
            return error;
        }

        update->ofpacts = ofpacts->data;
        update->ofpacts_len = ofpacts->size;
        return 0;
    } else {
        VLOG_WARN_RL(&bad_ofmsg_rl,
                     "NXST_FLOW_MONITOR reply has bad event %"PRIu16,
                     ntohs(nfuh->event));
        return OFPERR_NXBRC_FM_BAD_EVENT;
    }

bad_len:
    VLOG_WARN_RL(&bad_ofmsg_rl, "NXST_FLOW_MONITOR reply has %zu "
                 "leftover bytes at end", msg->size);
    return OFPERR_OFPBRC_BAD_LEN;
}

uint32_t
ofputil_decode_flow_monitor_cancel(const struct ofp_header *oh)
{
    const struct nx_flow_monitor_cancel *cancel = ofpmsg_body(oh);

    return ntohl(cancel->id);
}

struct ofpbuf *
ofputil_encode_flow_monitor_cancel(uint32_t id)
{
    struct nx_flow_monitor_cancel *nfmc;
    struct ofpbuf *msg;

    msg = ofpraw_alloc(OFPRAW_NXT_FLOW_MONITOR_CANCEL, OFP10_VERSION, 0);
    nfmc = ofpbuf_put_uninit(msg, sizeof *nfmc);
    nfmc->id = htonl(id);
    return msg;
}

void
ofputil_start_flow_update(struct list *replies)
{
    struct ofpbuf *msg;

    msg = ofpraw_alloc_xid(OFPRAW_NXST_FLOW_MONITOR_REPLY, OFP10_VERSION,
                           htonl(0), 1024);

    list_init(replies);
    list_push_back(replies, &msg->list_node);
}

void
ofputil_append_flow_update(const struct ofputil_flow_update *update,
                           struct list *replies)
{
    struct nx_flow_update_header *nfuh;
    struct ofpbuf *msg;
    size_t start_ofs;

    msg = ofpbuf_from_list(list_back(replies));
    start_ofs = msg->size;

    if (update->event == NXFME_ABBREV) {
        struct nx_flow_update_abbrev *nfua;

        nfua = ofpbuf_put_zeros(msg, sizeof *nfua);
        nfua->xid = update->xid;
    } else {
        struct nx_flow_update_full *nfuf;
        int match_len;

        ofpbuf_put_zeros(msg, sizeof *nfuf);
        match_len = nx_put_match(msg, update->match, htonll(0), htonll(0));
        ofpacts_put_openflow10(update->ofpacts, update->ofpacts_len, msg);

        nfuf = ofpbuf_at_assert(msg, start_ofs, sizeof *nfuf);
        nfuf->reason = htons(update->reason);
        nfuf->priority = htons(update->priority);
        nfuf->idle_timeout = htons(update->idle_timeout);
        nfuf->hard_timeout = htons(update->hard_timeout);
        nfuf->match_len = htons(match_len);
        nfuf->table_id = update->table_id;
        nfuf->cookie = update->cookie;
    }

    nfuh = ofpbuf_at_assert(msg, start_ofs, sizeof *nfuh);
    nfuh->length = htons(msg->size - start_ofs);
    nfuh->event = htons(update->event);

    ofpmp_postappend(replies, start_ofs);
}

struct ofpbuf *
ofputil_encode_packet_out(const struct ofputil_packet_out *po,
                          enum ofputil_protocol protocol)
{
    enum ofp_version ofp_version = ofputil_protocol_to_ofp_version(protocol);
    struct ofpbuf *msg;
    size_t size;

    size = po->ofpacts_len;
    if (po->buffer_id == UINT32_MAX) {
        size += po->packet_len;
    }

    switch (ofp_version) {
    case OFP10_VERSION: {
        struct ofp10_packet_out *opo;
        size_t actions_ofs;

        msg = ofpraw_alloc(OFPRAW_OFPT10_PACKET_OUT, OFP10_VERSION, size);
        ofpbuf_put_zeros(msg, sizeof *opo);
        actions_ofs = msg->size;
        ofpacts_put_openflow10(po->ofpacts, po->ofpacts_len, msg);

        opo = msg->l3;
        opo->buffer_id = htonl(po->buffer_id);
        opo->in_port = htons(po->in_port);
        opo->actions_len = htons(msg->size - actions_ofs);
        break;
    }

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_packet_out *opo;
        size_t len;

        msg = ofpraw_alloc(OFPRAW_OFPT11_PACKET_OUT, ofp_version, size);
        ofpbuf_put_zeros(msg, sizeof *opo);
        len = ofpacts_put_openflow11_actions(po->ofpacts, po->ofpacts_len, msg);

        opo = msg->l3;
        opo->buffer_id = htonl(po->buffer_id);
        opo->in_port = ofputil_port_to_ofp11(po->in_port);
        opo->actions_len = htons(len);
        break;
    }

    default:
        NOT_REACHED();
    }

    if (po->buffer_id == UINT32_MAX) {
        ofpbuf_put(msg, po->packet, po->packet_len);
    }

    ofpmsg_update_length(msg);

    return msg;
}

/* Creates and returns an OFPT_ECHO_REQUEST message with an empty payload. */
struct ofpbuf *
make_echo_request(enum ofp_version ofp_version)
{
    return ofpraw_alloc_xid(OFPRAW_OFPT_ECHO_REQUEST, ofp_version,
                            htonl(0), 0);
}

/* Creates and returns an OFPT_ECHO_REPLY message matching the
 * OFPT_ECHO_REQUEST message in 'rq'. */
struct ofpbuf *
make_echo_reply(const struct ofp_header *rq)
{
    struct ofpbuf rq_buf;
    struct ofpbuf *reply;

    ofpbuf_use_const(&rq_buf, rq, ntohs(rq->length));
    ofpraw_pull_assert(&rq_buf);

    reply = ofpraw_alloc_reply(OFPRAW_OFPT_ECHO_REPLY, rq, rq_buf.size);
    ofpbuf_put(reply, rq_buf.data, rq_buf.size);
    return reply;
}

struct ofpbuf *
ofputil_encode_barrier_request(enum ofp_version ofp_version)
{
    enum ofpraw type;

    switch (ofp_version) {
    case OFP13_VERSION:
    case OFP12_VERSION:
    case OFP11_VERSION:
        type = OFPRAW_OFPT11_BARRIER_REQUEST;
        break;

    case OFP10_VERSION:
        type = OFPRAW_OFPT10_BARRIER_REQUEST;
        break;

    default:
        NOT_REACHED();
    }

    return ofpraw_alloc(type, ofp_version, 0);
}

const char *
ofputil_frag_handling_to_string(enum ofp_config_flags flags)
{
    switch (flags & OFPC_FRAG_MASK) {
    case OFPC_FRAG_NORMAL:   return "normal";
    case OFPC_FRAG_DROP:     return "drop";
    case OFPC_FRAG_REASM:    return "reassemble";
    case OFPC_FRAG_NX_MATCH: return "nx-match";
    }

    NOT_REACHED();
}

bool
ofputil_frag_handling_from_string(const char *s, enum ofp_config_flags *flags)
{
    if (!strcasecmp(s, "normal")) {
        *flags = OFPC_FRAG_NORMAL;
    } else if (!strcasecmp(s, "drop")) {
        *flags = OFPC_FRAG_DROP;
    } else if (!strcasecmp(s, "reassemble")) {
        *flags = OFPC_FRAG_REASM;
    } else if (!strcasecmp(s, "nx-match")) {
        *flags = OFPC_FRAG_NX_MATCH;
    } else {
        return false;
    }
    return true;
}

/* Converts the OpenFlow 1.1+ port number 'ofp11_port' into an OpenFlow 1.0
 * port number and stores the latter in '*ofp10_port', for the purpose of
 * decoding OpenFlow 1.1+ protocol messages.  Returns 0 if successful,
 * otherwise an OFPERR_* number.
 *
 * See the definition of OFP11_MAX for an explanation of the mapping. */
enum ofperr
ofputil_port_from_ofp11(ovs_be32 ofp11_port, uint16_t *ofp10_port)
{
    uint32_t ofp11_port_h = ntohl(ofp11_port);

    if (ofp11_port_h < OFPP_MAX) {
        *ofp10_port = ofp11_port_h;
        return 0;
    } else if (ofp11_port_h >= OFPP11_MAX) {
        *ofp10_port = ofp11_port_h - OFPP11_OFFSET;
        return 0;
    } else {
#ifndef _OFP_CENTEC_
        VLOG_WARN_RL(&bad_ofmsg_rl, "port %"PRIu32" is outside the supported "
                     "range 0 through %d or 0x%"PRIx32" through 0x%"PRIx32,
                     ofp11_port_h, OFPP_MAX - 1,
                     (uint32_t) OFPP11_MAX, UINT32_MAX);
#else
        VLOG_WARN_RL(&bad_ofmsg_rl, "port 0x%"PRIx32" is outside the supported "
                     "range 0 through 0x%"PRIx32" or 0x%"PRIx32" through 0x%"PRIx32,
                     ofp11_port_h, OFPP_MAX - 1,
                     (uint32_t) OFPP11_MAX, UINT32_MAX);
#endif
        return OFPERR_OFPBAC_BAD_OUT_PORT;
    }
}

/* Returns the OpenFlow 1.1+ port number equivalent to the OpenFlow 1.0 port
 * number 'ofp10_port', for encoding OpenFlow 1.1+ protocol messages.
 *
 * See the definition of OFP11_MAX for an explanation of the mapping. */
ovs_be32
ofputil_port_to_ofp11(uint16_t ofp10_port)
{
    return htonl(ofp10_port < OFPP_MAX
                 ? ofp10_port
                 : ofp10_port + OFPP11_OFFSET);
}

/* Checks that 'port' is a valid output port for the OFPAT10_OUTPUT action, given
 * that the switch will never have more than 'max_ports' ports.  Returns 0 if
 * 'port' is valid, otherwise an OpenFlow return code. */
enum ofperr
ofputil_check_output_port(uint16_t port, int max_ports)
{
    switch (port) {
    case OFPP_IN_PORT:
    case OFPP_TABLE:
    case OFPP_NORMAL:
    case OFPP_FLOOD:
    case OFPP_ALL:
    case OFPP_CONTROLLER:
    case OFPP_NONE:
    case OFPP_LOCAL:
#ifdef _OFP_CENTEC_
    case OFPP_PW_FWD:
#endif
        return 0;

    default:
        if (port < max_ports) {
            return 0;
        }
        return OFPERR_OFPBAC_BAD_OUT_PORT;
    }
}

#ifdef _OFP_CENTEC_
#define OFPUTIL_NAMED_PORTS                     \
        OFPUTIL_NAMED_PORT(IN_PORT)             \
        OFPUTIL_NAMED_PORT(TABLE)               \
        OFPUTIL_NAMED_PORT(NORMAL)              \
        OFPUTIL_NAMED_PORT(FLOOD)               \
        OFPUTIL_NAMED_PORT(ALL)                 \
        OFPUTIL_NAMED_PORT(CONTROLLER)          \
        OFPUTIL_NAMED_PORT(LOCAL)               \
        OFPUTIL_NAMED_PORT(ANY)                 \
        OFPUTIL_NAMED_PORT(PW_FWD)
#else
#define OFPUTIL_NAMED_PORTS                     \
        OFPUTIL_NAMED_PORT(IN_PORT)             \
        OFPUTIL_NAMED_PORT(TABLE)               \
        OFPUTIL_NAMED_PORT(NORMAL)              \
        OFPUTIL_NAMED_PORT(FLOOD)               \
        OFPUTIL_NAMED_PORT(ALL)                 \
        OFPUTIL_NAMED_PORT(CONTROLLER)          \
        OFPUTIL_NAMED_PORT(LOCAL)               \
        OFPUTIL_NAMED_PORT(ANY)                
#endif

/* For backwards compatibility, so that "none" is recognized as OFPP_ANY */
#define OFPUTIL_NAMED_PORTS_WITH_NONE           \
        OFPUTIL_NAMED_PORTS                     \
        OFPUTIL_NAMED_PORT(NONE)

/* Stores the port number represented by 's' into '*portp'.  's' may be an
 * integer or, for reserved ports, the standard OpenFlow name for the port
 * (e.g. "LOCAL").
 *
 * Returns true if successful, false if 's' is not a valid OpenFlow port number
 * or name.  The caller should issue an error message in this case, because
 * this function usually does not.  (This gives the caller an opportunity to
 * look up the port name another way, e.g. by contacting the switch and listing
 * the names of all its ports).
 *
 * This function accepts OpenFlow 1.0 port numbers.  It also accepts a subset
 * of OpenFlow 1.1+ port numbers, mapping those port numbers into the 16-bit
 * range as described in include/openflow/openflow-1.1.h. */
bool
ofputil_port_from_string(const char *s, uint16_t *portp)
{
    unsigned int port32;

    *portp = 0;
    if (str_to_uint(s, 10, &port32)) {
        if (port32 < OFPP_MAX) {
            *portp = port32;
            return true;
        } else if (port32 < OFPP_FIRST_RESV) {
            VLOG_WARN("port %u is a reserved OF1.0 port number that will "
                      "be translated to %u when talking to an OF1.1 or "
                      "later controller", port32, port32 + OFPP11_OFFSET);
            *portp = port32;
            return true;
        } else if (port32 <= OFPP_LAST_RESV) {
            struct ds msg;

            ds_init(&msg);
            ofputil_format_port(port32, &msg);
            VLOG_WARN_ONCE("referring to port %s as %u is deprecated for "
                           "compatibility with future versions of OpenFlow",
                           ds_cstr(&msg), port32);
            ds_destroy(&msg);

            *portp = port32;
            return true;
        } else if (port32 < OFPP11_MAX) {
            VLOG_WARN("port %u is outside the supported range 0 through "
                      "%"PRIx16" or 0x%x through 0x%"PRIx32, port32,
                      UINT16_MAX, (unsigned int) OFPP11_MAX, UINT32_MAX);
            return false;
        } else {
            *portp = port32 - OFPP11_OFFSET;
            return true;
        }
    } else {
        struct pair {
            const char *name;
            uint16_t value;
        };
        static const struct pair pairs[] = {
#define OFPUTIL_NAMED_PORT(NAME) {#NAME, OFPP_##NAME},
            OFPUTIL_NAMED_PORTS_WITH_NONE
#undef OFPUTIL_NAMED_PORT
        };
        const struct pair *p;

        for (p = pairs; p < &pairs[ARRAY_SIZE(pairs)]; p++) {
            if (!strcasecmp(s, p->name)) {
                *portp = p->value;
                return true;
            }
        }
        return false;
    }
}

/* Appends to 's' a string representation of the OpenFlow port number 'port'.
 * Most ports' string representation is just the port number, but for special
 * ports, e.g. OFPP_LOCAL, it is the name, e.g. "LOCAL". */
void
ofputil_format_port(uint16_t port, struct ds *s)
{
    const char *name;

    switch (port) {
#define OFPUTIL_NAMED_PORT(NAME) case OFPP_##NAME: name = #NAME; break;
        OFPUTIL_NAMED_PORTS
#undef OFPUTIL_NAMED_PORT

    default:
        ds_put_format(s, "%"PRIu16, port);
        return;
    }
    ds_put_cstr(s, name);
}

/* Given a buffer 'b' that contains an array of OpenFlow ports of type
 * 'ofp_version', tries to pull the first element from the array.  If
 * successful, initializes '*pp' with an abstract representation of the
 * port and returns 0.  If no ports remain to be decoded, returns EOF.
 * On an error, returns a positive OFPERR_* value. */
int
ofputil_pull_phy_port(enum ofp_version ofp_version, struct ofpbuf *b,
                      struct ofputil_phy_port *pp)
{
    switch (ofp_version) {
    case OFP10_VERSION: {
        const struct ofp10_phy_port *opp = ofpbuf_try_pull(b, sizeof *opp);
        return opp ? ofputil_decode_ofp10_phy_port(pp, opp) : EOF;
    }
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        const struct ofp11_port *op = ofpbuf_try_pull(b, sizeof *op);
        return op ? ofputil_decode_ofp11_port(pp, op) : EOF;
    }
    default:
        NOT_REACHED();
    }
}

/* Given a buffer 'b' that contains an array of OpenFlow ports of type
 * 'ofp_version', returns the number of elements. */
size_t ofputil_count_phy_ports(uint8_t ofp_version, struct ofpbuf *b)
{
    return b->size / ofputil_get_phy_port_size(ofp_version);
}

/* Returns the 'enum ofputil_action_code' corresponding to 'name' (e.g. if
 * 'name' is "output" then the return value is OFPUTIL_OFPAT10_OUTPUT), or -1 if
 * 'name' is not the name of any action.
 *
 * ofp-util.def lists the mapping from names to action. */
int
ofputil_action_code_from_name(const char *name)
{
    static const char *names[OFPUTIL_N_ACTIONS] = {
        NULL,
#define OFPAT10_ACTION(ENUM, STRUCT, NAME)             NAME,
#define OFPAT11_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME) NAME,
#define NXAST_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME)   NAME,
#include "ofp-util.def"
    };

    const char **p;

    for (p = names; p < &names[ARRAY_SIZE(names)]; p++) {
        if (*p && !strcasecmp(name, *p)) {
            return p - names;
        }
    }
    return -1;
}

/* Appends an action of the type specified by 'code' to 'buf' and returns the
 * action.  Initializes the parts of 'action' that identify it as having type
 * <ENUM> and length 'sizeof *action' and zeros the rest.  For actions that
 * have variable length, the length used and cleared is that of struct
 * <STRUCT>.  */
void *
ofputil_put_action(enum ofputil_action_code code, struct ofpbuf *buf)
{
    switch (code) {
    case OFPUTIL_ACTION_INVALID:
        NOT_REACHED();

#define OFPAT10_ACTION(ENUM, STRUCT, NAME)                  \
    case OFPUTIL_##ENUM: return ofputil_put_##ENUM(buf);
#define OFPAT11_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME)      \
    case OFPUTIL_##ENUM: return ofputil_put_##ENUM(buf);
#define NXAST_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME)        \
    case OFPUTIL_##ENUM: return ofputil_put_##ENUM(buf);
#include "ofp-util.def"
    }
    NOT_REACHED();
}

#define OFPAT10_ACTION(ENUM, STRUCT, NAME)                        \
    void                                                        \
    ofputil_init_##ENUM(struct STRUCT *s)                       \
    {                                                           \
        memset(s, 0, sizeof *s);                                \
        s->type = htons(ENUM);                                  \
        s->len = htons(sizeof *s);                              \
    }                                                           \
                                                                \
    struct STRUCT *                                             \
    ofputil_put_##ENUM(struct ofpbuf *buf)                      \
    {                                                           \
        struct STRUCT *s = ofpbuf_put_uninit(buf, sizeof *s);   \
        ofputil_init_##ENUM(s);                                 \
        return s;                                               \
    }
#define OFPAT11_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME) \
    OFPAT10_ACTION(ENUM, STRUCT, NAME)
#define NXAST_ACTION(ENUM, STRUCT, EXTENSIBLE, NAME)            \
    void                                                        \
    ofputil_init_##ENUM(struct STRUCT *s)                       \
    {                                                           \
        memset(s, 0, sizeof *s);                                \
        s->type = htons(OFPAT10_VENDOR);                        \
        s->len = htons(sizeof *s);                              \
        s->vendor = htonl(NX_VENDOR_ID);                        \
        s->subtype = htons(ENUM);                               \
    }                                                           \
                                                                \
    struct STRUCT *                                             \
    ofputil_put_##ENUM(struct ofpbuf *buf)                      \
    {                                                           \
        struct STRUCT *s = ofpbuf_put_uninit(buf, sizeof *s);   \
        ofputil_init_##ENUM(s);                                 \
        return s;                                               \
    }
#include "ofp-util.def"

static void
ofputil_normalize_match__(struct match *match, bool may_log)
{
    enum {
        MAY_NW_ADDR     = 1 << 0, /* nw_src, nw_dst */
        MAY_TP_ADDR     = 1 << 1, /* tp_src, tp_dst */
        MAY_NW_PROTO    = 1 << 2, /* nw_proto */
        MAY_IPVx        = 1 << 3, /* tos, frag, ttl */
        MAY_ARP_SHA     = 1 << 4, /* arp_sha */
        MAY_ARP_THA     = 1 << 5, /* arp_tha */
        MAY_IPV6        = 1 << 6, /* ipv6_src, ipv6_dst, ipv6_label */
        MAY_ND_TARGET   = 1 << 7, /* nd_target */
        MAY_MPLS        = 1 << 8, /* mpls label and tc */
    } may_match;

    struct flow_wildcards wc;

    /* Figure out what fields may be matched. */
    if (match->flow.dl_type == htons(ETH_TYPE_IP)) {
        may_match = MAY_NW_PROTO | MAY_IPVx | MAY_NW_ADDR;
        if (match->flow.nw_proto == IPPROTO_TCP ||
            match->flow.nw_proto == IPPROTO_UDP ||
            match->flow.nw_proto == IPPROTO_ICMP) {
            may_match |= MAY_TP_ADDR;
        }
    } else if (match->flow.dl_type == htons(ETH_TYPE_IPV6)) {
        may_match = MAY_NW_PROTO | MAY_IPVx | MAY_IPV6;
        if (match->flow.nw_proto == IPPROTO_TCP ||
            match->flow.nw_proto == IPPROTO_UDP) {
            may_match |= MAY_TP_ADDR;
        } else if (match->flow.nw_proto == IPPROTO_ICMPV6) {
            may_match |= MAY_TP_ADDR;
            if (match->flow.tp_src == htons(ND_NEIGHBOR_SOLICIT)) {
                may_match |= MAY_ND_TARGET | MAY_ARP_SHA;
            } else if (match->flow.tp_src == htons(ND_NEIGHBOR_ADVERT)) {
                may_match |= MAY_ND_TARGET | MAY_ARP_THA;
            }
        }
    } else if (match->flow.dl_type == htons(ETH_TYPE_ARP) ||
               match->flow.dl_type == htons(ETH_TYPE_RARP)) {
        may_match = MAY_NW_PROTO | MAY_NW_ADDR | MAY_ARP_SHA | MAY_ARP_THA;
    } else if (eth_type_mpls(match->flow.dl_type)) {
        may_match = MAY_MPLS;
    } else {
        may_match = 0;
    }

    /* Clear the fields that may not be matched. */
    wc = match->wc;
    if (!(may_match & MAY_NW_ADDR)) {
        wc.masks.nw_src = wc.masks.nw_dst = htonl(0);
    }
    if (!(may_match & MAY_TP_ADDR)) {
        wc.masks.tp_src = wc.masks.tp_dst = htons(0);
    }
    if (!(may_match & MAY_NW_PROTO)) {
        wc.masks.nw_proto = 0;
    }
    if (!(may_match & MAY_IPVx)) {
        wc.masks.nw_tos = 0;
        wc.masks.nw_ttl = 0;
    }
    if (!(may_match & MAY_ARP_SHA)) {
        memset(wc.masks.arp_sha, 0, ETH_ADDR_LEN);
    }
    if (!(may_match & MAY_ARP_THA)) {
        memset(wc.masks.arp_tha, 0, ETH_ADDR_LEN);
    }
    if (!(may_match & MAY_IPV6)) {
        wc.masks.ipv6_src = wc.masks.ipv6_dst = in6addr_any;
        wc.masks.ipv6_label = htonl(0);
    }
    if (!(may_match & MAY_ND_TARGET)) {
        wc.masks.nd_target = in6addr_any;
    }
    if (!(may_match & MAY_MPLS)) {
        wc.masks.mpls_lse = htonl(0);
        wc.masks.mpls_depth = 0;
    }

    /* Log any changes. */
    if (!flow_wildcards_equal(&wc, &match->wc)) {
        bool log = may_log && !VLOG_DROP_INFO(&bad_ofmsg_rl);
        char *pre = log ? match_to_string(match, OFP_DEFAULT_PRIORITY) : NULL;

        match->wc = wc;
        match_zero_wildcarded_fields(match);

        if (log) {
            char *post = match_to_string(match, OFP_DEFAULT_PRIORITY);
            VLOG_INFO("normalization changed ofp_match, details:");
            VLOG_INFO(" pre: %s", pre);
            VLOG_INFO("post: %s", post);
            free(pre);
            free(post);
        }
    }
}

/* "Normalizes" the wildcards in 'match'.  That means:
 *
 *    1. If the type of level N is known, then only the valid fields for that
 *       level may be specified.  For example, ARP does not have a TOS field,
 *       so nw_tos must be wildcarded if 'match' specifies an ARP flow.
 *       Similarly, IPv4 does not have any IPv6 addresses, so ipv6_src and
 *       ipv6_dst (and other fields) must be wildcarded if 'match' specifies an
 *       IPv4 flow.
 *
 *    2. If the type of level N is not known (or not understood by Open
 *       vSwitch), then no fields at all for that level may be specified.  For
 *       example, Open vSwitch does not understand SCTP, an L4 protocol, so the
 *       L4 fields tp_src and tp_dst must be wildcarded if 'match' specifies an
 *       SCTP flow.
 *
 * If this function changes 'match', it logs a rate-limited informational
 * message. */
void
ofputil_normalize_match(struct match *match)
{
    ofputil_normalize_match__(match, true);
}

/* Same as ofputil_normalize_match() without the logging.  Thus, this function
 * is suitable for a program's internal use, whereas ofputil_normalize_match()
 * sense for use on flows received from elsewhere (so that a bug in the program
 * that sent them can be reported and corrected). */
void
ofputil_normalize_match_quiet(struct match *match)
{
    ofputil_normalize_match__(match, false);
}

/* Parses a key or a key-value pair from '*stringp'.
 *
 * On success: Stores the key into '*keyp'.  Stores the value, if present, into
 * '*valuep', otherwise an empty string.  Advances '*stringp' past the end of
 * the key-value pair, preparing it for another call.  '*keyp' and '*valuep'
 * are substrings of '*stringp' created by replacing some of its bytes by null
 * terminators.  Returns true.
 *
 * If '*stringp' is just white space or commas, sets '*keyp' and '*valuep' to
 * NULL and returns false. */
bool
ofputil_parse_key_value(char **stringp, char **keyp, char **valuep)
{
    char *pos, *key, *value;
    size_t key_len;

    pos = *stringp;
    pos += strspn(pos, ", \t\r\n");
    if (*pos == '\0') {
        *keyp = *valuep = NULL;
        return false;
    }

    key = pos;
    key_len = strcspn(pos, ":=(, \t\r\n");
    if (key[key_len] == ':' || key[key_len] == '=') {
        /* The value can be separated by a colon. */
        size_t value_len;

        value = key + key_len + 1;
        value_len = strcspn(value, ", \t\r\n");
        pos = value + value_len + (value[value_len] != '\0');
        value[value_len] = '\0';
    } else if (key[key_len] == '(') {
        /* The value can be surrounded by balanced parentheses.  The outermost
         * set of parentheses is removed. */
        int level = 1;
        size_t value_len;

        value = key + key_len + 1;
        for (value_len = 0; level > 0; value_len++) {
            switch (value[value_len]) {
            case '\0':
                level = 0;
                break;

            case '(':
                level++;
                break;

            case ')':
                level--;
                break;
            }
        }
        value[value_len - 1] = '\0';
        pos = value + value_len;
    } else {
        /* There might be no value at all. */
        value = key + key_len;  /* Will become the empty string below. */
        pos = key + key_len + (key[key_len] != '\0');
    }
    key[key_len] = '\0';

    *stringp = pos;
    *keyp = key;
    *valuep = value;
    return true;
}

/* Encode a dump ports request for 'port', the encoded message
 * will be for Open Flow version 'ofp_version'. Returns message
 * as a struct ofpbuf. Returns encoded message on success, NULL on error */
struct ofpbuf *
ofputil_encode_dump_ports_request(enum ofp_version ofp_version, int16_t port)
{
    struct ofpbuf *request;

    switch (ofp_version) {
    case OFP10_VERSION: {
        struct ofp10_port_stats_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST10_PORT_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->port_no = htons(port);
        break;
    }
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_port_stats_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST11_PORT_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->port_no = ofputil_port_to_ofp11(port);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

static void
ofputil_port_stats_to_ofp10(const struct ofputil_port_stats *ops,
                            struct ofp10_port_stats *ps10)
{
    ps10->port_no = htons(ops->port_no);
    memset(ps10->pad, 0, sizeof ps10->pad);
    put_32aligned_be64(&ps10->rx_packets, htonll(ops->stats.rx_packets));
    put_32aligned_be64(&ps10->tx_packets, htonll(ops->stats.tx_packets));
    put_32aligned_be64(&ps10->rx_bytes, htonll(ops->stats.rx_bytes));
    put_32aligned_be64(&ps10->tx_bytes, htonll(ops->stats.tx_bytes));
    put_32aligned_be64(&ps10->rx_dropped, htonll(ops->stats.rx_dropped));
    put_32aligned_be64(&ps10->tx_dropped, htonll(ops->stats.tx_dropped));
    put_32aligned_be64(&ps10->rx_errors, htonll(ops->stats.rx_errors));
    put_32aligned_be64(&ps10->tx_errors, htonll(ops->stats.tx_errors));
    put_32aligned_be64(&ps10->rx_frame_err, htonll(ops->stats.rx_frame_errors));
    put_32aligned_be64(&ps10->rx_over_err, htonll(ops->stats.rx_over_errors));
    put_32aligned_be64(&ps10->rx_crc_err, htonll(ops->stats.rx_crc_errors));
    put_32aligned_be64(&ps10->collisions, htonll(ops->stats.collisions));
}

static void
ofputil_port_stats_to_ofp11(const struct ofputil_port_stats *ops,
                            struct ofp11_port_stats *ps11)
{
    ps11->port_no = ofputil_port_to_ofp11(ops->port_no);
    memset(ps11->pad, 0, sizeof ps11->pad);
    ps11->rx_packets = htonll(ops->stats.rx_packets);
    ps11->tx_packets = htonll(ops->stats.tx_packets);
    ps11->rx_bytes = htonll(ops->stats.rx_bytes);
    ps11->tx_bytes = htonll(ops->stats.tx_bytes);
    ps11->rx_dropped = htonll(ops->stats.rx_dropped);
    ps11->tx_dropped = htonll(ops->stats.tx_dropped);
    ps11->rx_errors = htonll(ops->stats.rx_errors);
    ps11->tx_errors = htonll(ops->stats.tx_errors);
    ps11->rx_frame_err = htonll(ops->stats.rx_frame_errors);
    ps11->rx_over_err = htonll(ops->stats.rx_over_errors);
    ps11->rx_crc_err = htonll(ops->stats.rx_crc_errors);
    ps11->collisions = htonll(ops->stats.collisions);
}

static void
ofputil_port_stats_to_ofp13(const struct ofputil_port_stats *ops,
                            struct ofp13_port_stats *ps13)
{
    ofputil_port_stats_to_ofp11(ops, &ps13->ps);

    /* OF 1.3 adds duration fields */
    /* FIXME: Need to implement port alive duration (sec + nsec) */
    ps13->duration_sec = htonl(~0);
    ps13->duration_nsec = htonl(~0);
}


/* Encode a ports stat for 'ops' and append it to 'replies'. */
void
ofputil_append_port_stat(struct list *replies,
                         const struct ofputil_port_stats *ops)
{
    struct ofpbuf *msg = ofpbuf_from_list(list_back(replies));
    struct ofp_header *oh = msg->data;

    switch ((enum ofp_version)oh->version) {
    case OFP13_VERSION: {
        struct ofp13_port_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_port_stats_to_ofp13(ops, reply);
        break;
    }
    case OFP12_VERSION:
    case OFP11_VERSION: {
        struct ofp11_port_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_port_stats_to_ofp11(ops, reply);
        break;
    }

    case OFP10_VERSION: {
        struct ofp10_port_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_port_stats_to_ofp10(ops, reply);
        break;
    }

    default:
        NOT_REACHED();
    }
}

static enum ofperr
ofputil_port_stats_from_ofp10(struct ofputil_port_stats *ops,
                              const struct ofp10_port_stats *ps10)
{
    memset(ops, 0, sizeof *ops);

    ops->port_no = ntohs(ps10->port_no);
    ops->stats.rx_packets = ntohll(get_32aligned_be64(&ps10->rx_packets));
    ops->stats.tx_packets = ntohll(get_32aligned_be64(&ps10->tx_packets));
    ops->stats.rx_bytes = ntohll(get_32aligned_be64(&ps10->rx_bytes));
    ops->stats.tx_bytes = ntohll(get_32aligned_be64(&ps10->tx_bytes));
    ops->stats.rx_dropped = ntohll(get_32aligned_be64(&ps10->rx_dropped));
    ops->stats.tx_dropped = ntohll(get_32aligned_be64(&ps10->tx_dropped));
    ops->stats.rx_errors = ntohll(get_32aligned_be64(&ps10->rx_errors));
    ops->stats.tx_errors = ntohll(get_32aligned_be64(&ps10->tx_errors));
    ops->stats.rx_frame_errors =
        ntohll(get_32aligned_be64(&ps10->rx_frame_err));
    ops->stats.rx_over_errors = ntohll(get_32aligned_be64(&ps10->rx_over_err));
    ops->stats.rx_crc_errors = ntohll(get_32aligned_be64(&ps10->rx_crc_err));
    ops->stats.collisions = ntohll(get_32aligned_be64(&ps10->collisions));

    return 0;
}

static enum ofperr
ofputil_port_stats_from_ofp11(struct ofputil_port_stats *ops,
                              const struct ofp11_port_stats *ps11)
{
    enum ofperr error;

    memset(ops, 0, sizeof *ops);
    error = ofputil_port_from_ofp11(ps11->port_no, &ops->port_no);
    if (error) {
        return error;
    }

    ops->stats.rx_packets = ntohll(ps11->rx_packets);
    ops->stats.tx_packets = ntohll(ps11->tx_packets);
    ops->stats.rx_bytes = ntohll(ps11->rx_bytes);
    ops->stats.tx_bytes = ntohll(ps11->tx_bytes);
    ops->stats.rx_dropped = ntohll(ps11->rx_dropped);
    ops->stats.tx_dropped = ntohll(ps11->tx_dropped);
    ops->stats.rx_errors = ntohll(ps11->rx_errors);
    ops->stats.tx_errors = ntohll(ps11->tx_errors);
    ops->stats.rx_frame_errors = ntohll(ps11->rx_frame_err);
    ops->stats.rx_over_errors = ntohll(ps11->rx_over_err);
    ops->stats.rx_crc_errors = ntohll(ps11->rx_crc_err);
    ops->stats.collisions = ntohll(ps11->collisions);

    return 0;
}

static enum ofperr
ofputil_port_stats_from_ofp13(struct ofputil_port_stats *ops,
                              const struct ofp13_port_stats *ps13)
{
    enum ofperr error =
        ofputil_port_stats_from_ofp11(ops, &ps13->ps);
    if (!error) {
        /* FIXME: Get ps13->duration_sec and ps13->duration_nsec,
         * Add to netdev_stats? */
    }

    return error;
}


/* Returns the number of port stats elements in OFPTYPE_PORT_STATS_REPLY
 * message 'oh'. */
size_t
ofputil_count_port_stats(const struct ofp_header *oh)
{
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    ofpraw_pull_assert(&b);

    BUILD_ASSERT(sizeof(struct ofp10_port_stats) ==
                 sizeof(struct ofp11_port_stats));
    return b.size / sizeof(struct ofp10_port_stats);
}

/* Converts an OFPST_PORT_STATS reply in 'msg' into an abstract
 * ofputil_port_stats in 'ps'.
 *
 * Multiple OFPST_PORT_STATS replies can be packed into a single OpenFlow
 * message.  Calling this function multiple times for a single 'msg' iterates
 * through the replies.  The caller must initially leave 'msg''s layer pointers
 * null and not modify them between calls.
 *
 * Returns 0 if successful, EOF if no replies were left in this 'msg',
 * otherwise a positive errno value. */
int
ofputil_decode_port_stats(struct ofputil_port_stats *ps, struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST13_PORT_REPLY) {
        const struct ofp13_port_stats *ps13;

        ps13 = ofpbuf_try_pull(msg, sizeof *ps13);
        if (!ps13) {
            goto bad_len;
        }
        return ofputil_port_stats_from_ofp13(ps, ps13);
    } else if (raw == OFPRAW_OFPST11_PORT_REPLY) {
        const struct ofp11_port_stats *ps11;

        ps11 = ofpbuf_try_pull(msg, sizeof *ps11);
        if (!ps11) {
            goto bad_len;
        }
        return ofputil_port_stats_from_ofp11(ps, ps11);
    } else if (raw == OFPRAW_OFPST10_PORT_REPLY) {
        const struct ofp10_port_stats *ps10;

        ps10 = ofpbuf_try_pull(msg, sizeof *ps10);
        if (!ps10) {
            goto bad_len;
        }
        return ofputil_port_stats_from_ofp10(ps, ps10);
    } else {
        NOT_REACHED();
    }

 bad_len:
    VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_PORT reply has %zu leftover "
                 "bytes at end", msg->size);
    return OFPERR_OFPBRC_BAD_LEN;
}

/* Parse a port status request message into a 16 bit OpenFlow 1.0
 * port number and stores the latter in '*ofp10_port'.
 * Returns 0 if successful, otherwise an OFPERR_* number. */
enum ofperr
ofputil_decode_port_stats_request(const struct ofp_header *request,
                                  uint16_t *ofp10_port)
{
    switch ((enum ofp_version)request->version) {
    case OFP13_VERSION:
    case OFP12_VERSION:
    case OFP11_VERSION: {
        const struct ofp11_port_stats_request *psr11 = ofpmsg_body(request);
        return ofputil_port_from_ofp11(psr11->port_no, ofp10_port);
    }

    case OFP10_VERSION: {
        const struct ofp10_port_stats_request *psr10 = ofpmsg_body(request);
        *ofp10_port = ntohs(psr10->port_no);
        return 0;
    }

    default:
        NOT_REACHED();
    }
}

/* Parse a queue status request message into 'oqsr'.
 * Returns 0 if successful, otherwise an OFPERR_* number. */
enum ofperr
ofputil_decode_queue_stats_request(const struct ofp_header *request,
                                   struct ofputil_queue_stats_request *oqsr)
{
    switch ((enum ofp_version)request->version) {
    case OFP13_VERSION:
    case OFP12_VERSION:
    case OFP11_VERSION: {
        const struct ofp11_queue_stats_request *qsr11 = ofpmsg_body(request);
        oqsr->queue_id = ntohl(qsr11->queue_id);
        return ofputil_port_from_ofp11(qsr11->port_no, &oqsr->port_no);
    }

    case OFP10_VERSION: {
        const struct ofp10_queue_stats_request *qsr10 = ofpmsg_body(request);
        oqsr->queue_id = ntohl(qsr10->queue_id);
        oqsr->port_no = ntohs(qsr10->port_no);
        /* OF 1.0 uses OFPP_ALL for OFPP_ANY */
        if (oqsr->port_no == OFPP_ALL) {
            oqsr->port_no = OFPP_ANY;
        }
        return 0;
    }

    default:
        NOT_REACHED();
    }
}

/* Encode a queue statsrequest for 'oqsr', the encoded message
 * will be fore Open Flow version 'ofp_version'. Returns message
 * as a struct ofpbuf. Returns encoded message on success, NULL on error */
struct ofpbuf *
ofputil_encode_queue_stats_request(enum ofp_version ofp_version,
                                   const struct ofputil_queue_stats_request *oqsr)
{
    struct ofpbuf *request;

    switch (ofp_version) {
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_queue_stats_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST11_QUEUE_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->port_no = ofputil_port_to_ofp11(oqsr->port_no);
        req->queue_id = htonl(oqsr->queue_id);
        break;
    }
    case OFP10_VERSION: {
        struct ofp10_queue_stats_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST10_QUEUE_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        /* OpenFlow 1.0 needs OFPP_ALL instead of OFPP_ANY */
        req->port_no = htons(oqsr->port_no == OFPP_ANY
                             ? OFPP_ALL : oqsr->port_no);
        req->queue_id = htonl(oqsr->queue_id);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

/* Returns the number of queue stats elements in OFPTYPE_QUEUE_STATS_REPLY
 * message 'oh'. */
size_t
ofputil_count_queue_stats(const struct ofp_header *oh)
{
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    ofpraw_pull_assert(&b);

    BUILD_ASSERT(sizeof(struct ofp10_queue_stats) ==
                 sizeof(struct ofp11_queue_stats));
#ifndef _OFP_CENTEC_
    return b.size / sizeof(struct ofp10_queue_stats);
#else
    switch ((enum ofp_version)oh->version) {
    case OFP13_VERSION:
        return b.size / sizeof(struct ofp13_queue_stats);

    case OFP11_VERSION:
        return b.size / sizeof(struct ofp11_queue_stats);

    case OFP10_VERSION:
        return b.size / sizeof(struct ofp10_queue_stats);

    case OFP12_VERSION:
    default:
        NOT_REACHED();
    }
#endif /*_OFP_CENTEC_*/
}

static enum ofperr
ofputil_queue_stats_from_ofp10(struct ofputil_queue_stats *oqs,
                               const struct ofp10_queue_stats *qs10)
{
    oqs->port_no = ntohs(qs10->port_no);
    oqs->queue_id = ntohl(qs10->queue_id);
    oqs->stats.tx_bytes = ntohll(get_32aligned_be64(&qs10->tx_bytes));
    oqs->stats.tx_packets = ntohll(get_32aligned_be64(&qs10->tx_packets));
    oqs->stats.tx_errors = ntohll(get_32aligned_be64(&qs10->tx_errors));

    return 0;
}

static enum ofperr
ofputil_queue_stats_from_ofp11(struct ofputil_queue_stats *oqs,
                               const struct ofp11_queue_stats *qs11)
{
    enum ofperr error;

    error = ofputil_port_from_ofp11(qs11->port_no, &oqs->port_no);
    if (error) {
        return error;
    }

    oqs->queue_id = ntohl(qs11->queue_id);
    oqs->stats.tx_bytes = ntohll(qs11->tx_bytes);
    oqs->stats.tx_packets = ntohll(qs11->tx_packets);
    oqs->stats.tx_errors = ntohll(qs11->tx_errors);

    return 0;
}

static enum ofperr
ofputil_queue_stats_from_ofp13(struct ofputil_queue_stats *oqs,
                               const struct ofp13_queue_stats *qs13)
{
    enum ofperr error
        = ofputil_queue_stats_from_ofp11(oqs, &qs13->qs);
    if (!error) {
        /* FIXME: Get qs13->duration_sec and qs13->duration_nsec,
         * Add to netdev_queue_stats? */
#ifdef _OFP_CENTEC_
        oqs->stats.duration_sec = ntohl(qs13->duration_sec);
        oqs->stats.duration_nsec = ntohl(qs13->duration_nsec);
#endif /* _OFP_CENTEC_ */
    }

    return error;
}

/* Converts an OFPST_QUEUE_STATS reply in 'msg' into an abstract
 * ofputil_queue_stats in 'qs'.
 *
 * Multiple OFPST_QUEUE_STATS replies can be packed into a single OpenFlow
 * message.  Calling this function multiple times for a single 'msg' iterates
 * through the replies.  The caller must initially leave 'msg''s layer pointers
 * null and not modify them between calls.
 *
 * Returns 0 if successful, EOF if no replies were left in this 'msg',
 * otherwise a positive errno value. */
int
ofputil_decode_queue_stats(struct ofputil_queue_stats *qs, struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST13_QUEUE_REPLY) {
        const struct ofp13_queue_stats *qs13;

        qs13 = ofpbuf_try_pull(msg, sizeof *qs13);
        if (!qs13) {
            goto bad_len;
        }
        return ofputil_queue_stats_from_ofp13(qs, qs13);
    } else if (raw == OFPRAW_OFPST11_QUEUE_REPLY) {
        const struct ofp11_queue_stats *qs11;

        qs11 = ofpbuf_try_pull(msg, sizeof *qs11);
        if (!qs11) {
            goto bad_len;
        }
        return ofputil_queue_stats_from_ofp11(qs, qs11);
    } else if (raw == OFPRAW_OFPST10_QUEUE_REPLY) {
        const struct ofp10_queue_stats *qs10;

        qs10 = ofpbuf_try_pull(msg, sizeof *qs10);
        if (!qs10) {
            goto bad_len;
        }
        return ofputil_queue_stats_from_ofp10(qs, qs10);
    } else {
        NOT_REACHED();
    }

 bad_len:
    VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_QUEUE reply has %zu leftover "
                 "bytes at end", msg->size);
    return OFPERR_OFPBRC_BAD_LEN;
}

static void
ofputil_queue_stats_to_ofp10(const struct ofputil_queue_stats *oqs,
                             struct ofp10_queue_stats *qs10)
{
    qs10->port_no = htons(oqs->port_no);
    memset(qs10->pad, 0, sizeof qs10->pad);
    qs10->queue_id = htonl(oqs->queue_id);
    put_32aligned_be64(&qs10->tx_bytes, htonll(oqs->stats.tx_bytes));
    put_32aligned_be64(&qs10->tx_packets, htonll(oqs->stats.tx_packets));
    put_32aligned_be64(&qs10->tx_errors, htonll(oqs->stats.tx_errors));
}

static void
ofputil_queue_stats_to_ofp11(const struct ofputil_queue_stats *oqs,
                             struct ofp11_queue_stats *qs11)
{
    qs11->port_no = ofputil_port_to_ofp11(oqs->port_no);
    qs11->queue_id = htonl(oqs->queue_id);
    qs11->tx_bytes = htonll(oqs->stats.tx_bytes);
    qs11->tx_packets = htonll(oqs->stats.tx_packets);
    qs11->tx_errors = htonll(oqs->stats.tx_errors);
}

static void
ofputil_queue_stats_to_ofp13(const struct ofputil_queue_stats *oqs,
                             struct ofp13_queue_stats *qs13)
{
    long long int duration_sec;
    uint32_t sec, nsec;
    
    ofputil_queue_stats_to_ofp11(oqs, &qs13->qs);

#ifdef _OFP_CENTEC_
    duration_sec = time_msec() - time_boot_msec();
    sec = duration_sec  / 1000;
    nsec =  (duration_sec % 1000) * (1000 * 1000);
#endif /* _OFP_CENTEC_ */    
    qs13->duration_sec = htonl(sec);
    qs13->duration_nsec = htonl(nsec);
}

/* Encode a queue stat for 'oqs' and append it to 'replies'. */
void
ofputil_append_queue_stat(struct list *replies,
                          const struct ofputil_queue_stats *oqs)
{
    struct ofpbuf *msg = ofpbuf_from_list(list_back(replies));
    struct ofp_header *oh = msg->data;

    switch ((enum ofp_version)oh->version) {
    case OFP13_VERSION: {
        struct ofp13_queue_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_queue_stats_to_ofp13(oqs, reply);
        break;
    }

    case OFP12_VERSION:
    case OFP11_VERSION: {
        struct ofp11_queue_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_queue_stats_to_ofp11(oqs, reply);
        break;
    }

    case OFP10_VERSION: {
        struct ofp10_queue_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_queue_stats_to_ofp10(oqs, reply);
        break;
    }

    default:
        NOT_REACHED();
    }
}

#ifdef _OFP_CENTEC_
static void
ofputil_put_table_property(struct ofputil_table_feature_prop *prop, 
                        struct ofpbuf *reply)
{
    size_t start_prop = reply->size;
    uint16_t real_length;
    uint16_t padded_length;
    
    switch (prop->type) {
    case OFPTFPT13_INSTRUCTIONS: 
    case OFPTFPT13_INSTRUCTIONS_MISS: {
        struct ofp13_table_feature_prop_instructions *otfp;
        uint8_t *header;

        /* Padding bits should not affect data length, it's the controller's 
         * duty to skip the padding bits according to the spec. */
        real_length = sizeof(struct ofp13_table_feature_prop_instructions) + 
                      prop->elements_len;
        padded_length = ROUND_UP(real_length, 8);
        ofpbuf_put_uninit(reply, padded_length);
        otfp = ofpbuf_at_assert(reply, start_prop, sizeof *otfp);
        memset (otfp, 0, padded_length);
        otfp->type = htons(prop->type);
        otfp->length = htons(real_length);
        header = ofpbuf_at_assert(reply, 
            start_prop + sizeof(struct ofp13_table_feature_prop_instructions), 
            prop->elements_len);
        memcpy(header, prop->elements, prop->elements_len);
        break;
    }
    
    case OFPTFPT13_WRITE_ACTIONS: 
    case OFPTFPT13_WRITE_ACTIONS_MISS: 
    case OFPTFPT13_APPLY_ACTIONS: 
    case OFPTFPT13_APPLY_ACTIONS_MISS: {
        struct ofp13_table_feature_prop_actions *otfp;
        uint8_t *header;
        
        /* Padding bits should not affect data length, it's the controller's 
         * duty to skip the padding bits according to the spec. */
        real_length = sizeof(struct ofp13_table_feature_prop_actions) + 
                      prop->elements_len;
        padded_length = ROUND_UP(real_length, 8);
        ofpbuf_put_uninit(reply, padded_length);
        otfp = ofpbuf_at_assert(reply, start_prop, sizeof *otfp);
        memset (otfp, 0, padded_length);
        otfp->type = htons(prop->type);
        otfp->length = htons(real_length);
        header = ofpbuf_at_assert(reply, 
                start_prop + sizeof(struct ofp13_table_feature_prop_actions), 
                prop->elements_len);
        memcpy(header, prop->elements, prop->elements_len);
        break;
    }

    case OFPTFPT13_MATCH: 
    case OFPTFPT13_WILDCARDS: 
    case OFPTFPT13_WRITE_SETFIELD: 
    case OFPTFPT13_WRITE_SETFIELD_MISS: 
    case OFPTFPT13_APPLY_SETFIELD: 
    case OFPTFPT13_APPLY_SETFIELD_MISS: {
        struct ofp13_table_feature_prop_oxm *otfp;
        uint8_t *header;
        
        /* Padding bits should not affect data length, it's the controller's 
         * duty to skip the padding bits according to the spec. */
        real_length = sizeof(struct ofp13_table_feature_prop_oxm) + 
                      prop->elements_len;
        padded_length = ROUND_UP(real_length, 8);

        ofpbuf_put_uninit(reply, padded_length);
        otfp = ofpbuf_at_assert(reply, start_prop, sizeof *otfp);
        memset (otfp, 0, padded_length);
        otfp->type = htons(prop->type);
        otfp->length = htons(real_length);
        header = ofpbuf_at_assert(reply, 
                start_prop + sizeof(struct ofp13_table_feature_prop_oxm), 
                prop->elements_len);
        memcpy(header, prop->elements, prop->elements_len);
        break;
    }

    default:
        NOT_REACHED();
    }
}

struct ofpbuf *
ofputil_encode_table_feature_request(enum ofp_version ofp_version)
{
    struct ofpbuf *request = NULL;

    switch (ofp_version) {
    case OFP10_VERSION: 
    case OFP11_VERSION:
    case OFP12_VERSION: {
        return NULL;
    }
    case OFP13_VERSION: {
        request = ofpraw_alloc(OFPRAW_OFPST13_TABLE_FEATURES_REQUEST, 
                                        ofp_version, 0);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

void
ofputil_append_table_feature_reply(const struct ofputil_table_features *features, 
                                struct list *replies)
{
    struct ofpbuf *reply = ofpbuf_from_list(list_back(replies));
    enum ofpraw raw;
    size_t start_otf = reply->size;
    struct list *list_node;
    struct ofputil_table_feature_prop *otfp;
    
    ofpraw_decode_partial(&raw, reply->data, reply->size);
    if (raw == OFPRAW_OFPST13_TABLE_FEATURES_REPLY) {
        struct ofp13_table_features *otf;
        ofpbuf_put_uninit(reply, sizeof *otf);

        if (!list_is_empty(features->properties)) {
            list_node = list_front(features->properties);
            while (list_node != features->properties) {
                otfp = CONTAINER_OF(list_node,
                                struct ofputil_table_feature_prop, list_node);
                ofputil_put_table_property(otfp, reply);
                list_node = list_node->next;
            }
        }

        otf = ofpbuf_at_assert(reply, start_otf, sizeof *otf);
        otf->length = htons(reply->size - start_otf);
        otf->table_id = features->table_id;
        ovs_strlcpy(otf->name, features->name, sizeof otf->name);
        otf->metadata_match = htonll(features->metadata_match);
        otf->metadata_write = htonll(features->metadata_write);
        otf->config = htonl(features->config);
        otf->max_entries = htonl(features->max_entries);
    } else {
        NOT_REACHED();
    }    
    ofpmp_postappend(replies, start_otf);
}

int
ofputil_decode_table_feature_reply(struct ofputil_table_features *otf,
                struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST13_TABLE_FEATURES_REPLY) {
        const struct ofp13_table_features *otf13;
        struct ofp13_table_features *header;

        header = (struct ofp13_table_features *)msg->data;
        otf13 = ofpbuf_try_pull(msg, ntohs(header->length));
        if (!otf13) {
            goto bad_len;
        }

        otf->table_id = otf13->table_id;
        ovs_strlcpy(otf->name, otf13->name, sizeof otf->name);
        otf->metadata_match = ntohll(otf13->metadata_match);
        otf->metadata_write = ntohll(otf13->metadata_write);
        otf->config = ntohl(otf13->config);
        otf->max_entries = ntohl(otf13->max_entries);
        /*XXX: To do, decode properties*/
        
        return 0;
    } else {
        NOT_REACHED();
    }

 bad_len:
    VLOG_WARN_RL(&bad_ofmsg_rl, "TABLE_FEATURES reply has %zu leftover "
                 "bytes at end", msg->size);
    return OFPERR_OFPBRC_BAD_LEN;
}

struct ofpbuf *
ofputil_encode_group_stats_request(enum ofp_version ofp_version, uint32_t group_id)
{
    struct ofpbuf *request;

    switch (ofp_version) {
    case OFP10_VERSION: {
        ovs_fatal(0, "dump-gstats need argument \'-O openflow11(+)\'");
    }
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_group_stats_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST11_GROUP_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->group_id = htonl(group_id);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

struct ofpbuf *
ofputil_encode_group_desc_request(enum ofp_version ofp_version)
{
    struct ofpbuf *request;

    switch (ofp_version) {
    case OFP10_VERSION: {
        ovs_fatal(0, "dump-groups need argument \'-O openflow11(+)\'");
    }
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        request = ofpraw_alloc(OFPRAW_OFPST11_GROUP_DESC_REQUEST, ofp_version, 0);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

static void
ofputil_group_stats_to_ofp11(const struct ofputil_group_stats *ogs,
                            struct ofp11_group_stats *gs11)
{
    gs11->length = sizeof(*gs11) + sizeof(struct ofp11_bucket_counter) * ogs->n_buckets;
    memset(gs11->pad, 0, sizeof gs11->pad);
    gs11->group_id = htonl(ogs->group_id);
    gs11->ref_count = htonl(ogs->ref_count);
    memset(gs11->pad2, 0, sizeof gs11->pad2);
    gs11->packet_count = htonll(ogs->packet_count);
    gs11->byte_count = htonll(ogs->byte_count);
}

static void
ofputil_group_stats_to_ofp13(const struct ofputil_group_stats *ogs,
                            struct ofp13_group_stats *gs13)
{
    gs13->gs.length = sizeof(*gs13) + sizeof(struct ofp11_bucket_counter) * ogs->n_buckets;
    memset(gs13->gs.pad, 0, sizeof gs13->gs.pad);
    gs13->gs.group_id = htonl(ogs->group_id);
    gs13->gs.ref_count = htonl(ogs->ref_count);
    memset(gs13->gs.pad2, 0, sizeof gs13->gs.pad2);
    gs13->gs.packet_count = htonll(ogs->packet_count);
    gs13->gs.byte_count = htonll(ogs->byte_count);
    gs13->duration_sec = htonl(ogs->duration_sec);
    gs13->duration_nsec = htonl(ogs->duration_nsec);
}

/* Encode a group stat for 'ogs' and append it to 'replies'. */
void
ofputil_append_group_stats(struct list *replies,
                         const struct ofputil_group_stats *ogs)
{
    struct ofpbuf *msg = ofpbuf_from_list(list_back(replies));
    struct ofp_header *oh = msg->data;
    int32_t bucket_i;

    switch ((enum ofp_version)oh->version) {
    case OFP13_VERSION: {
        struct ofp13_group_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_group_stats_to_ofp13(ogs, reply);
        for (bucket_i = 0; bucket_i < ogs->n_buckets; bucket_i++) {
            struct ofp11_bucket_counter* bucket = ofpmp_append(replies, sizeof *bucket);
            bucket->packet_count = htonll(ogs->bucket_stats[bucket_i].packet_count);
            bucket->byte_count = htonll(ogs->bucket_stats[bucket_i].byte_count);
        }
        
        break;
    }
        
    case OFP12_VERSION:
    case OFP11_VERSION: {
        struct ofp11_group_stats *reply = ofpmp_append(replies, sizeof *reply);
        ofputil_group_stats_to_ofp11(ogs, reply);
        for (bucket_i = 0; bucket_i < ogs->n_buckets; bucket_i++) {
            struct ofp11_bucket_counter* bucket = ofpmp_append(replies, sizeof *bucket);
            bucket->packet_count = htonll(ogs->bucket_stats[bucket_i].packet_count);
            bucket->byte_count = htonll(ogs->bucket_stats[bucket_i].byte_count);
        }
        break;
    }

    case OFP10_VERSION: 
        break;
 
    default:
        NOT_REACHED();
    }
}

struct ofpbuf *
ofputil_encode_group_features_request(enum ofp_version ofp_version)
{
    struct ofpbuf *request = NULL;

    switch (ofp_version) {
    case OFP10_VERSION: 
    case OFP11_VERSION:{
        ovs_fatal(0, "dump-group-fea need argument \'-O openflow12(+)\'");
    }
    case OFP12_VERSION:
    case OFP13_VERSION: {
        request = ofpraw_alloc(OFPRAW_OFPST12_GROUP_FEATURES_REQUEST, 
                                        ofp_version, 0);
        break;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

struct ofpbuf *
ofputil_encode_group_features_reply(const struct ofputil_group_features *features,
                                 const struct ofp_header *request)
{
    struct ofpbuf *reply;
    enum ofpraw raw;
    struct ofp12_group_features_stats *ogf;
    
    switch ((enum ofp_version) request->version) {
    case OFP10_VERSION:
    case OFP11_VERSION:
        ovs_fatal(0, "Group need argument \'-O Openflow12\'");
        
    case OFP12_VERSION:
    case OFP13_VERSION:
        raw = OFPRAW_OFPST12_GROUP_FEATURES_REPLY;
        break;

    default:
        NOT_REACHED();
    }

    reply = ofpraw_alloc_xid(raw, (enum ofp_version) request->version, 
                        request->xid, 0);
    ogf = ofpbuf_put_zeros(reply, sizeof *ogf);
    ogf->types = htonl(features->types);
    ogf->capabilities = htonl(features->capabilities);
    ogf->max_groups[0] = htonl(features->max_groups[0]);
    ogf->max_groups[1] = htonl(features->max_groups[1]);
    ogf->max_groups[2] = htonl(features->max_groups[2]);
    ogf->max_groups[3] = htonl(features->max_groups[3]);
    ogf->actions[0] = htonl(features->actions[0]);
    ogf->actions[1] = htonl(features->actions[1]);
    ogf->actions[2] = htonl(features->actions[2]);
    ogf->actions[3] = htonl(features->actions[3]);

    return reply;
}

enum ofperr
ofputil_decode_group_features_reply(const struct ofp_header *oh,
                               struct ofputil_group_features *features,
                               struct ofpbuf *b)
{
    const struct ofp12_group_features_stats *ogf;
    enum ofpraw raw;

    ofpbuf_use_const(b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(b);

    ogf = ofpbuf_pull(b, sizeof *ogf);

    if (raw == OFPRAW_OFPST12_GROUP_FEATURES_REPLY) {
        features->types = htonl(ogf->types);
        features->capabilities = htonl(ogf->capabilities);
        features->max_groups[0] = htonl(ogf->max_groups[0]);
        features->max_groups[1] = htonl(ogf->max_groups[1]);
        features->max_groups[2] = htonl(ogf->max_groups[2]);
        features->max_groups[3] = htonl(ogf->max_groups[3]);
        features->actions[0] = htonl(ogf->actions[0]);
        features->actions[1] = htonl(ogf->actions[1]);
        features->actions[2] = htonl(ogf->actions[2]);
        features->actions[3] = htonl(ogf->actions[3]);
    } else {
        return OFPERR_OFPBRC_BAD_VERSION;
    }

    return 0;
}

/* Parse a group status request message into a 32 bit OpenFlow 1.1
 * group ID and stores the latter in '*group_id'.
 * Returns 0 if successful, otherwise an OFPERR_* number. */
enum ofperr
ofputil_decode_group_stats_request(const struct ofp_header *request,
                                  uint32_t *group_id)
{
    switch ((enum ofp_version)request->version) {
    case OFP13_VERSION:
    case OFP12_VERSION:
    case OFP11_VERSION: {
        const struct ofp11_group_stats_request *gsr11 = ofpmsg_body(request);
        *group_id = ntohl(gsr11->group_id);
        return 0;
    }

    case OFP10_VERSION: 
        return OFPERR_OFPGMFC_INVALID_GROUP;
            
    default:
        NOT_REACHED();
    }
}


int
ofputil_decode_group_stats_reply(uint32_t raw,
                               struct ofputil_group_stats *ogs,
                               struct ofpbuf *b)
{
    struct ofp11_group_stats *ogs11 = NULL;
    struct ofp13_group_stats *ogs13 = NULL;
    struct ofp11_bucket_counter* obc = NULL;
    size_t bucket_i;

    if (!b->size) {
        return EOF;
    }
    
    if (raw == OFPRAW_OFPST11_GROUP_REPLY) {
        ogs11 = ofpbuf_try_pull(b, sizeof *ogs11);
        if(!ogs11) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP reply has %zu leftover "
                         "bytes at end", b->size);
            return EINVAL;
        }
        ogs->group_id = ntohl(ogs11->group_id);
        ogs->ref_count = ntohl(ogs11->ref_count);
        ogs->packet_count = ntohll(ogs11->packet_count);
        ogs->byte_count = ntohll(ogs11->byte_count);

        if (ntohs(ogs11->length) < sizeof *ogs11) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP reply claims invalid "
                         "length %zu", ogs13->gs.length);
            return EINVAL;
        }
        ogs->n_buckets = (ogs11->length - sizeof *ogs11) / sizeof(struct ofp11_bucket_counter);

        obc = ofpbuf_try_pull(b, ogs->n_buckets * sizeof(struct ofp11_bucket_counter));

        if (!obc) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP reply has %zu leftover "
                         "bytes at end", b->size);
            return EINVAL;
        }
        for (bucket_i = 0; bucket_i < ogs->n_buckets; bucket_i++) {
            ogs->bucket_stats[bucket_i].packet_count = ntohll(obc[bucket_i].packet_count);
            ogs->bucket_stats[bucket_i].byte_count = ntohll(obc[bucket_i].byte_count);
        }
        
    } else if (raw == OFPRAW_OFPST13_GROUP_REPLY) {
        ogs13 = ofpbuf_try_pull(b, sizeof *ogs13);
        if(!ogs13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST13_GROUP reply has %zu leftover "
                         "bytes at end", b->size);
            return EINVAL;
        }
        ogs->group_id = ntohl(ogs13->gs.group_id);
        ogs->ref_count = ntohl(ogs13->gs.ref_count);
        ogs->packet_count = ntohll(ogs13->gs.packet_count);
        ogs->byte_count = ntohll(ogs13->gs.byte_count);
        ogs->duration_sec = ntohl(ogs13->duration_sec);
        ogs->duration_nsec = ntohl(ogs13->duration_nsec);

        if (ntohs(ogs13->gs.length) < sizeof *ogs13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST13_GROUP reply claims invalid "
                         "length %zu", ogs13->gs.length);
            return EINVAL;
        }
        ogs->n_buckets = (ogs13->gs.length - sizeof *ogs13) / sizeof(struct ofp11_bucket_counter);

        obc = ofpbuf_try_pull(b, ogs->n_buckets * sizeof(struct ofp11_bucket_counter));

        if (!obc) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST13_GROUP reply has %zu leftover "
                         "bytes at end", b->size);
            return EINVAL;
        }
        for (bucket_i = 0; bucket_i < ogs->n_buckets; bucket_i++) {
            ogs->bucket_stats[bucket_i].packet_count = ntohll(obc[bucket_i].packet_count);
            ogs->bucket_stats[bucket_i].byte_count = ntohll(obc[bucket_i].byte_count);
        }
    }
    
    return 0;
}


/* Appends an OFPST_FLOW reply that contains the data in 'fs' to
 * those already present in the list of ofpbufs in 'replies'.  'replies' should
 * have been initialized with ofputil_start_stats_reply(). */
void
ofputil_append_group_desc_reply(const struct ofputil_group_desc *gds,
                                struct list* buckets,
                                struct list *replies)
{
    struct ofpbuf *reply = ofpbuf_from_list(list_back(replies));
    size_t start_ogds = reply->size;
    size_t start_bucket;
    enum ofpraw raw;
    struct ofputil_bucket *bucket;
    
    ofpraw_decode_partial(&raw, reply->data, reply->size);
    if (raw == OFPRAW_OFPST11_GROUP_DESC_REPLY) {
        struct ofp11_group_desc_stats *ogds;
        struct ofp11_bucket *ob;
        ofpbuf_put_uninit(reply, sizeof *ogds);

        LIST_FOR_EACH(bucket, list_node, buckets) {
            start_bucket = reply->size;
            ofpbuf_put_uninit(reply, sizeof *ob);
            ofpacts_put_openflow11_actions(bucket->ofpacts, 
                            bucket->ofpacts_len, reply);
            ob = ofpbuf_at_assert(reply, start_bucket, sizeof *ob);
            ob->len = htons(reply->size - start_bucket);
            ob->weight = htons(bucket->weight);
            ob->watch_port = ofputil_port_to_ofp11(bucket->watch_port);
            ob->watch_group = htonl(bucket->watch_group);
        }
        ogds = ofpbuf_at_assert(reply, start_ogds, sizeof *ogds);
        ogds->length = htons(reply->size - start_ogds);
        ogds->type = gds->type;
        ogds->pad = 0;
        ogds->group_id = htonl(gds->group_id);
    } else {
        NOT_REACHED();
    }

    ofpmp_postappend(replies, start_ogds);
    return;
}


static enum ofperr
ofputil_pull_bucket11(struct ofpbuf *b, uint16_t group_length, struct list* buckets)
{
    enum ofperr error;
    struct ofp11_bucket *ob;
    struct ofpbuf ofpacts;
    int action_len;
    struct ofputil_bucket *bucket;
    size_t start_group = b->size;
    uint16_t ofp10_port;
    
    while((b->size >= sizeof(struct ofp11_bucket)) && 
          (start_group - b->size) < group_length)
    {
        ob = ofpbuf_pull(b, sizeof *ob);
        if (!ob || (ob->len < sizeof(struct ofp11_bucket) )) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OpenFlow message bucket length %u is not a valid length ",
                ob->len);
            return OFPERR_OFPGMFC_BAD_BUCKET;
        }
        action_len = ntohs(ob->len) - sizeof (struct ofp11_bucket);
        ofpbuf_init(&ofpacts, 0);
        error = ofpacts_pull_openflow11_actions(b, action_len, &ofpacts);
        if (error) {
            ofpbuf_uninit(&ofpacts);
            return error;
        }

        bucket = xzalloc(sizeof *bucket);
        bucket->weight = ntohs(ob->weight);

        error = ofputil_port_from_ofp11(ob->watch_port, &ofp10_port);
        if (error) {
            ofpbuf_uninit(&ofpacts);
            return OFPERR_OFPGMFC_BAD_WATCH;
        }
        bucket->watch_port = ofp10_port;
        bucket->watch_group = ntohl(ob->watch_group);
        bucket->ofpacts = xmemdup(ofpacts.data, ofpacts.size);
        bucket->ofpacts_len = ofpacts.size;
        list_push_back(buckets, &bucket->list_node);
        ofpbuf_uninit(&ofpacts);
    }

    return 0;
}

int
ofputil_decode_group_desc_reply(struct ofputil_group_desc *ogd,
                               struct ofpbuf *b)
{
    struct ofp11_group_desc_stats *ogds = NULL;

    if (!b->size) {
        return EOF;
    }
    
    ogds = ofpbuf_try_pull(b, sizeof *ogds);
    if(!ogds) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP_DESC reply has %zu leftover "
                     "bytes at end", b->size);
        return EINVAL;
    }
    ogd->length = ntohs(ogds->length);
    ogd->type = ogds->type;
    ogd->group_id = ntohl(ogds->group_id);
    list_init(&ogd->buckets);

    if (ntohs(ogds->length) < sizeof *ogds) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP_DESC reply claims invalid "
                     "length %zu", ogds->length);
        return EINVAL;
    }
    if (ntohs(ogds->length) - sizeof *ogds > b->size) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST11_GROUP_DESC reply claims invalid "
                     "length %zu", ogds->length);
        return EINVAL;
    }

    ofputil_pull_bucket11(b, ogd->length - sizeof(*ogds), &ogd->buckets);
    
    return 0;
}

struct ofpbuf *
ofputil_encode_group_mod(enum ofp_version ofp_version, const struct ofputil_group_mod *gm)
{
    struct ofpbuf *b;
    struct ofp11_group_mod *ogm;
    size_t start_ogm;
    size_t start_bucket;
    struct ofputil_bucket *bucket;
    struct ofp11_bucket *ob;

    switch (ofp_version) {
    case OFP10_VERSION: {
        if (OFPGC11_ADD == gm->command) {
            ovs_fatal(0, "add-group need argument \'-O openflow11(+)\'");
        } else if (OFPGC11_MODIFY ==  gm->command) {
            ovs_fatal(0, "mod-group need argument \'-O openflow11(+)\'");
        } else {
            ovs_fatal(0, "del-groups need argument \'-O openflow11(+)\'");
        }
    }

    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        b = ofpraw_alloc(OFPRAW_OFPT11_GROUP_MOD, ofp_version, 0);
        start_ogm = b->size;
        ofpbuf_put_uninit(b, sizeof *ogm);

        LIST_FOR_EACH(bucket, list_node, gm->buckets) {
            start_bucket = b->size;
            ofpbuf_put_uninit(b, sizeof *ob);
            if (bucket->ofpacts && bucket->ofpacts_len) {
                ofpacts_put_openflow11_actions(bucket->ofpacts, 
                                bucket->ofpacts_len, b);
            }
            ob = ofpbuf_at_assert(b, start_bucket, sizeof *ob);
            ob->len = htons(b->size - start_bucket);;
            ob->weight = htons(bucket->weight);
            ob->watch_port = htonl(bucket->watch_port);
            ob->watch_group = htonl(bucket->watch_group);
        }
        ogm = ofpbuf_at_assert(b, start_ogm, sizeof *ogm);
        ogm->command = htons(gm->command);
        ogm->type = gm->type;
        ogm->pad = 0;
        ogm->group_id = htonl(gm->group_id);

        break;
    }

    default:
        NOT_REACHED();
    }

    return b;
}

enum ofperr
ofputil_decode_group_mod(const struct ofp_header *oh,
                        struct ofputil_group_mod *gm)
{
    enum ofpraw raw;
    struct ofpbuf b;
    enum ofperr error;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);

    if (raw == OFPRAW_OFPT11_GROUP_MOD) {
        const struct ofp11_group_mod *ogm;
        uint32_t group_length;

        ogm = ofpbuf_pull(&b, sizeof *ogm);
        gm->command = ntohs(ogm->command);
        gm->type = ogm->type;
        gm->group_id = ntohl(ogm->group_id);
        
        /* Now get the buckets. */
        group_length = ntohs(oh->length) - sizeof *oh - sizeof *ogm;
        error = ofputil_pull_bucket11(&b, group_length, gm->buckets);
        if (error) {
            return error;
        }
    } else {
        return OFPERR_OFPBRC_BAD_TYPE;
    }

    return 0;
}

enum ofperr
ofputil_decode_table_mod(const struct ofp_header *oh,
                        struct ofputil_table_mod *tm)
{
    enum ofpraw raw;
    struct ofpbuf b;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);

    if (raw == OFPRAW_OFPT11_TABLE_MOD) {
        const struct ofp11_table_mod *otm;

        otm = ofpbuf_pull(&b, sizeof *otm);
        tm->table_id = otm->table_id;
        tm->config = ntohl(otm->config);
    } else {
        return OFPERR_OFPBRC_BAD_TYPE;
    }

    return 0;
}

/* Meter */
/* Find out usable protocols for meter */
enum ofputil_protocol
ofputil_meter_usable_protocols(void)
{
    enum ofputil_protocol usable_protocols;

    usable_protocols = OFPUTIL_P_ANY;
    usable_protocols &= OFPUTIL_P_OF13_OXM;

    return usable_protocols;
}

/* Pull bands */
static enum ofperr
ofputil_pull_bands(struct ofpbuf *b, unsigned int bands_len,
                     struct ofp13_meter_band_drop **bandsp, size_t *n_bandsp)
{
    if (bands_len % OFP_METER_BAND_ALIGN != 0) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "OpenFlow message bands length %u "
                     "is not a multiple of %d", bands_len, OFP_METER_BAND_ALIGN);
        goto error;
    }

    *bandsp = ofpbuf_try_pull(b, bands_len);
    if (*bandsp == NULL) {
        VLOG_WARN_RL(&bad_ofmsg_rl, "OpenFlow message bands length %u "
                     "exceeds remaining message length (%zu)",
                     bands_len, b->size);
        goto error;
    }

    *n_bandsp = bands_len / OFP_METER_BAND_ALIGN;
    return 0;

error:
    *bandsp = NULL;
    *n_bandsp = 0;

    return OFPERR_OFPBRC_BAD_LEN;
}

/* Converts 'mm' into an OFPT_METER_MOD message returns the message. */
struct ofpbuf *
ofputil_encode_meter_mod(const struct ofputil_meter_mod *mm,
                    enum ofp_version ofp_version)
{
    struct ofpbuf *msg;
    size_t band;

    switch (ofp_version) {
        case OFP13_VERSION: {
            struct ofp13_meter_mod *omm;

            msg = ofpraw_alloc(OFPRAW_OFPT13_METER_MOD, ofp_version, 0);
            omm = ofpbuf_put_zeros(msg, sizeof *omm);
            omm->command  = htons(mm->command);
            omm->flags    = htons(mm->flags);
            omm->meter_id = htonl(mm->meter_id);

            for(band = 0; band < mm->n_bands; band ++)
            {
                ofpbuf_put(msg, (const void *)&(mm->bands[band]), sizeof(struct ofp13_meter_band_drop));
            }
            break;
        }
        case OFP10_VERSION:
        case OFP11_VERSION:
        case OFP12_VERSION:
            return NULL;
        default:
            NOT_REACHED();
    }

    ofpmsg_update_length(msg);

    return msg;
}

/* Decode meter mod message */
enum ofperr
ofputil_decode_meter_mod(struct ofputil_meter_mod *mm,
                        const struct ofp_header *oh)
{
    struct ofpbuf b;
    enum ofpraw raw;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    if (raw == OFPRAW_OFPT13_METER_MOD) {
        const struct ofp13_meter_mod *omm;
        enum ofperr error;

        omm = ofpbuf_pull(&b, sizeof *omm);

        if (ntohs(omm->command) != OFPMC13_DELETE) {
            if (!b.size) {
                VLOG_WARN_RL(&bad_ofmsg_rl, "Invalid meter id: %"PRIu32", need specify a band",
                    ntohl(omm->meter_id));
                return OFPERR_OFPMMFC_INVALID_METER;
            }

            error  = ofputil_pull_bands(&b, b.size, &mm->bands, &mm->n_bands);
            if (error) {
                return error;
            }

            if (mm->n_bands > OFP_MAX_BAND_PER_METER) {
                VLOG_WARN_RL(&bad_ofmsg_rl, "Invalid meter, id : %"PRIu32". Only %"PRIu32" bands per meter are supported.",
                    ntohl(omm->meter_id),
                    OFP_MAX_BAND_PER_METER);
                return OFPERR_OFPMMFC_OUT_OF_BANDS;
            }
        }
        if (b.size) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "METER_MOD request claims invalid "
                         "length %zu", b.size);
            return OFPERR_OFPMMFC_INVALID_METER;
        }

        mm->meter_id = ntohl(omm->meter_id);
        mm->flags    = ntohs(omm->flags);
        mm->command  = ntohs(omm->command);

    } else {
        NOT_REACHED();
    }

    return 0;
}

/* Decode meter reply message */
enum ofperr
ofputil_decode_meter_reply(struct ofputil_meter_stats_reply *omsr,
                                struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST13_METER_REPLY) {
        const struct ofp13_meter_stats *oms13;
        size_t length;

        oms13 = ofpbuf_try_pull(msg, sizeof *oms13);
        if (!oms13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_METER reply has %zu leftover "
                         "bytes at end", msg->size);
            return EINVAL;
        }

        length = ntohs(oms13->len);
        if (length < sizeof *oms13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_METER reply claims invalid "
                         "length %zu", length);
            return EINVAL;
        }

        omsr->meter_id        = ntohl(oms13->meter_id);
        omsr->len             = ntohs(oms13->len);
        omsr->flow_count      = ntohl(oms13->flow_count);
        omsr->packet_in_count = ntohll(oms13->packet_in_count);
        omsr->byte_in_count   = ntohll(oms13->byte_in_count);
        omsr->duration_sec    = ntohl(oms13->duration_sec);
        omsr->duration_nsec   = ntohl(oms13->duration_nsec);

        omsr->n_bands = (omsr->len - sizeof *oms13) / sizeof(struct ofp13_meter_band_stats);

        omsr->bands_stats = ofpbuf_try_pull(msg, omsr->n_bands * sizeof(struct ofp13_meter_band_stats));

        if (!omsr->bands_stats) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_METER reply has %zu leftover "
                     "bytes at end", msg->size);
            return EINVAL;
        }
    }
    else {
        NOT_REACHED();
    }

    return 0;
}

/* Decode meter config reply message */
enum ofperr
ofputil_decode_meter_config_reply(struct ofputil_meter_config_reply *omcr,
                                struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;

    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPST13_METER_CONFIG_REPLY) {
        const struct ofp13_meter_config *omc13;
        size_t length;

        omc13 = ofpbuf_try_pull(msg, sizeof *omc13);
        if (!omc13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_METER_CONFIG reply has %zu leftover "
                         "bytes at end", msg->size);
            return EINVAL;
        }

        length = ntohs(omc13->length);
        if (length < sizeof *omc13) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFPST_METER_CONFIG reply claims invalid "
                         "length %zu", length);
            return EINVAL;
        }

        omcr->flags = ntohs(omc13->flags);
        omcr->length = ntohs(omc13->length);
        omcr->meter_id = ntohl(omc13->meter_id);
        omcr->n_bands = (omcr->length - sizeof *omc13) / sizeof(struct ofp13_meter_band_drop);
        omcr->bands = ofpbuf_try_pull(msg, omcr->n_bands * sizeof(struct ofp13_meter_band_drop));

        if (!omcr->bands) {
            VLOG_WARN_RL(&bad_ofmsg_rl, "OFST_METER_CONFIG reply has %zu leftover "
                    "bytes at end", msg->size);
            return EINVAL;
        }
    }
    else {
        NOT_REACHED();
    }

    return 0;
}

/* Encode meter config request message */
struct ofpbuf *
ofputil_encode_meter_config_request(const struct ofputil_meter_request *msr, enum ofp_version ofp_version)
{
        struct ofpbuf *request = NULL;

        switch (ofp_version) {
        case OFP10_VERSION:
        case OFP11_VERSION:
        case OFP12_VERSION: {
            return NULL;
        }
        case OFP13_VERSION: {
            struct ofp13_meter_multipart_request *ommr;
            request = ofpraw_alloc(OFPRAW_OFPST13_METER_CONFIG_REQUEST, ofp_version, 0);
            ommr = ofpbuf_put_zeros(request, sizeof *ommr);
            ommr->meter_id = htonl(msr->meter_id);
            break;
        }
        default:
            NOT_REACHED();
        }

        ofpmsg_update_length(request);

        return request;
}

/* Encode meter features request message */
struct ofpbuf *
ofputil_encode_meter_features_request(enum ofp_version ofp_version)
{
        struct ofpbuf *request = NULL;

        switch (ofp_version) {
        case OFP10_VERSION:
        case OFP11_VERSION:
        case OFP12_VERSION: {
            return NULL;
        }
        case OFP13_VERSION: {
            request = ofpraw_alloc(OFPRAW_OFPST13_METER_FEATURES_REQUEST, ofp_version, 0);
            break;
        }
        default:
            NOT_REACHED();
        }

        return request;
}

/* Encode meter stats request message */
struct ofpbuf *
ofputil_encode_meter_stats_request(const struct ofputil_meter_request *msr, enum ofp_version ofp_version)
{
    struct ofpbuf *request = NULL;

    switch (ofp_version) {
    case OFP10_VERSION:
    case OFP11_VERSION:
    case OFP12_VERSION: {
        return NULL;
    }
    case OFP13_VERSION: {
        struct ofp13_meter_multipart_request *req;
        request = ofpraw_alloc(OFPRAW_OFPST13_METER_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->meter_id = htonl(msr->meter_id);
        break;
    }
    default:
        NOT_REACHED();
    }
    ofpmsg_update_length(request);

    return request;
}

/* Decode meter request message */
enum ofperr
ofputil_decode_meter_request(struct ofputil_meter_request *msr,
                                 const struct ofp_header *oh)
{
    struct ofpbuf b;
    enum ofpraw raw;

    ofpbuf_use_const(&b, oh, ntohs(oh->length));
    raw = ofpraw_pull_assert(&b);
    if (raw == OFPRAW_OFPST13_METER_REQUEST ||
        raw == OFPRAW_OFPST13_METER_CONFIG_REQUEST) {
        const struct ofp13_meter_multipart_request *ommr;

        ommr = ofpbuf_pull(&b, sizeof *ommr);
        msr->meter_id = ntohl(ommr->meter_id);
    } else {
        return OFPERR_OFPBRC_BAD_TYPE;
    }

    return 0;
}

/* Append meter stats reply message */
void
ofputil_append_meter_stats_reply(const struct ofputil_meter_stats_reply *msr,
                                struct list *replies)
{
    struct ofp13_meter_stats *oms;

    oms = ofpmp_append(replies, sizeof *oms);
    oms->meter_id = htonl(msr->meter_id);
    memset(oms->pad, 0, sizeof(oms->pad));
    oms->flow_count      = htonl(msr->flow_count);
    oms->duration_sec    = htonl(msr->duration_sec);
    oms->duration_nsec   = htonl(msr->duration_nsec);
    oms->packet_in_count = htonll(msr->packet_in_count);
    oms->byte_in_count   = htonll(msr->byte_in_count);
    oms->len             = htons(msr->len);
}

/* Append meter band stats reply message */
void
ofputil_append_meter_band_stats_reply(const struct ofputil_meter_band_stats_reply *mbsr,
                                struct list *replies)
{
    struct ofp13_meter_band_stats *nmbs;

    nmbs = ofpmp_append(replies, sizeof *nmbs);
    nmbs->packet_band_count = htonll(mbsr->packet_band_count);
    nmbs->byte_band_count   = htonll(mbsr->byte_band_count);
}

/* Append meter config reply message */
void
ofputil_append_meter_config_reply(const struct ofputil_meter_config_reply *mcr,
                                struct list *replies)
{
    struct ofp13_meter_config *mc;

    mc = ofpmp_append(replies, sizeof *mc);
    mc->length   = htons(mcr->length);
    mc->flags    = htons(mcr->flags);
    mc->meter_id = htonl(mcr->meter_id);
}

/* Append meter band config reply message */
void
ofputil_append_meter_band_config_reply(const struct ofputil_meter_band_config_reply *mbc,
                                struct list *replies)
{
    struct ofp13_meter_band_drop *nmb;

    nmb = ofpmp_append(replies, sizeof *nmb);
    nmb->type       = htons(mbc->type);
    nmb->len        = htons(mbc->len);
    nmb->rate       = htonl(mbc->rate);
    nmb->burst_size = htonl(mbc->burst_size);
}

enum ofperr
ofputil_decode_queue_get_config_request(const struct ofp_header *request,
                                   struct ofputil_queue_config_request *oqsr)
{
    switch ((enum ofp_version)request->version) {
    case OFP13_VERSION:
    case OFP12_VERSION:
    case OFP11_VERSION: {
        const struct ofp11_queue_get_config_request *qgcr11 = ofpmsg_body(request);
        return ofputil_port_from_ofp11(qgcr11->port, &oqsr->port);
    }

    case OFP10_VERSION: {
        const struct ofp10_queue_get_config_request *qsr10 = ofpmsg_body(request);
        oqsr->port = ntohs(qsr10->port);
        /* OF 1.0 uses OFPP_ALL for OFPP_ANY */
        if (oqsr->port == OFPP_ALL) {
            oqsr->port = OFPP_ANY;
        }
        return 0;
    }

    default:
        NOT_REACHED();
    }
}

struct ofpbuf *
ofputil_encode_queue_config_request(enum ofp_version ofp_version,
                                   const struct ofputil_queue_config_request *oqsr)
{
    struct ofpbuf *request;

    switch (ofp_version) {
    case OFP11_VERSION:
    case OFP12_VERSION:
    case OFP13_VERSION: {
        struct ofp11_queue_get_config_request *req;
        request = ofpraw_alloc(OFPRAW_OFPT11_QUEUE_GET_CONFIG_REQUEST, ofp_version, 0);
        req = ofpbuf_put_zeros(request, sizeof *req);
        req->port= ofputil_port_to_ofp11(oqsr->port);
        break;
    }
    case OFP10_VERSION: {
        ovs_fatal(0, "dump-queue-config need argument \'-O openflow11(+)\'");
        return NULL;
    }
    default:
        NOT_REACHED();
    }

    return request;
}

enum ofperr
ofputil_decode_queue_config(struct ofputil_queue_config_s *oqc, struct ofpbuf *msg)
{
    enum ofperr error;
    enum ofpraw raw;
    int8_t queue_id = 0;
    
    error = (msg->l2
             ? ofpraw_decode(&raw, msg->l2)
             : ofpraw_pull(&raw, msg));
    if (error) {
        return error;
    }

    if (!msg->size) {
        return EOF;
    } else if (raw == OFPRAW_OFPT11_QUEUE_GET_CONFIG_REPLY) {
        struct ofputil_queue_get_config_reply *qc11;  
        struct ofp_packet_queue *opq;
        struct ofp_queue_prop_min_rate *oqp_min;
        struct ofp_queue_prop_min_rate *oqp_max;

        qc11 = ofpbuf_try_pull(msg, sizeof *qc11);
        if (!qc11) {
            goto bad_len;
        }

        oqc->port_id = ntohl(qc11->port);
        
        for (queue_id = 0; queue_id < OFUTIL_MAX_QUEUE_VALUE; queue_id++) {
            opq = ofpbuf_try_pull(msg, (sizeof *opq));
            if (!opq) {
                goto bad_len;
            }

            oqp_min = ofpbuf_try_pull(msg, (sizeof *oqp_min));
            if (!oqp_min) {
                goto bad_len;
            }

            oqp_max = ofpbuf_try_pull(msg, (sizeof *oqp_max));
            if (!oqp_max) {
                goto bad_len;
            }

            oqc->min_rate[queue_id] = ntohs(oqp_min->rate);

            oqc->max_rate[queue_id] = ntohs(oqp_max->rate);
            
        }
        return 0;
    } else {
        NOT_REACHED();
    }

 bad_len:
    VLOG_WARN_RL(&bad_ofmsg_rl, "QUEUE_GET_CONFIG reply has %zu leftover "
                 "bytes at end", msg->size);
    
    return OFPERR_OFPBRC_BAD_LEN;

}

#endif
