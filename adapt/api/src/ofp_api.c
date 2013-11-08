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
 * @brief This file is the main implementation of Centec open vswitchd adapt layer API
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_opf.h"
#include "adpt_port.h"
#include "adpt_flow_priv.h"
#include "adpt_flow.h"
#include "adpt_message.h"
#include "adpt_nexthop.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

VLOG_DEFINE_THIS_MODULE(ofp_api);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Send packet from CPU. 16(cpu mac header) + 32(bridge header) + payload
 * @param encap_info encapsulate information
 * @param buf packet buffer
 * @param buf_len packet buffer length
 * @param hardware_process_packet packet that will be forwarded by hardware, a bridge header will be encapsulated to the original packet
 * @return
 */
static int32_ofp
ofp_encap_packet(ofp_cpu_encap_info_t* encap_info, uint8_ofp* buf, uint16_ofp buf_len, struct ofpbuf *hardware_process_packet)
{
    ofp_cpu_packet_t packet;
    uint32_ofp hash = 0;
    uint8_ofp *p;
    uint32_ofp hardware_process_packet_length = 0;

    OFP_PTR_CHECK(encap_info);
    OFP_PTR_CHECK(buf);
    OFP_PTR_CHECK(hardware_process_packet);
    OFP_LOG_DEBUG_FUNC();

    memset(&packet, 0, sizeof(ofp_cpu_packet_t));

    /* encapsulate cpu mac header, bridge header and payload */
    /* 1 encapsulate cpu header, 16byte */
    packet.cpu_mac_header.cpu_mac_sa[0] = 0xFF;
    packet.cpu_mac_header.cpu_mac_sa[1] = 0xFF;
    packet.cpu_mac_header.cpu_mac_sa[2] = 0xFF;
    packet.cpu_mac_header.cpu_mac_sa[3] = 0xFF;
    packet.cpu_mac_header.cpu_mac_sa[4] = 0xFF;
    packet.cpu_mac_header.cpu_mac_sa[5] = 0x00;
    packet.cpu_mac_header.cpu_mac_da[0] = 0xFE;
    packet.cpu_mac_header.cpu_mac_da[1] = 0xFD;
    packet.cpu_mac_header.cpu_mac_da[2] = 0x0;
    packet.cpu_mac_header.cpu_mac_da[3] = 0x0;
    packet.cpu_mac_header.cpu_mac_da[4] = 0x0;
    packet.cpu_mac_header.cpu_mac_da[5] = 0x1;
    packet.cpu_mac_header.cpu_hdr_type = 0x5A5A;

    /* 2 encapsulate bridge header, 32byte */
    if (encap_info->is_mcast)
    {
        packet.bridge_header.nxt_hop_ptr = 0;
    }
    else if (encap_info->nh_type == OFP_NEXTHOP_TYPE_BRIDGE)
    {
        packet.bridge_header.nxt_hop_ptr = BRIDGE_NEXTHOP_PTR;     /* can do vlan tag edit */
    }
    else if (encap_info->nh_type == OFP_NEXTHOP_TYPE_BYPASS)
    {
        packet.bridge_header.nxt_hop_ptr = BYPASS_NEXTHOP_PTR;     /* no edit, should input final packet */
    }
    else if (encap_info->nh_type == OFP_NEXTHOP_TYPE_UNTAG)
    {
        packet.bridge_header.nxt_hop_ptr = UNTAG_NEXTHOP_PTR;      /* can remove vlan tags */
    }
    else if (encap_info->nh_type == OFP_NEXTHOP_TYPE_OFFSET)
    {
        packet.bridge_header.nxt_hop_ptr = encap_info->nh_offset;  /* can do any edit which support */
    }

    packet.bridge_header.dest_id = encap_info->destid;             /* dest local port */
    packet.bridge_header.dest_chip_id = encap_info->dest_chipid;   /* dest global chipId */
    packet.bridge_header.src_port = encap_info->src_port;

    /* head hash is used for linkagg */
    hash = encap_info->hash;
    packet.bridge_header.hd_hash_2_to_0 = hash & 0x7;
    packet.bridge_header.hd_hash_7_to_3 = (hash >> 3) & 0x1F;

    packet.bridge_header.hdr_type = 1;                             /* for humber, it should be 1 */
    packet.bridge_header.multi_cast = encap_info->is_mcast;        /* 0:unicast, 1:multicast */
    if(encap_info->src_vid)
    {
        packet.bridge_header.src_vid = encap_info->src_vid;        /* source svlanId*/
        packet.bridge_header.src_svid_vld = 1;
        packet.bridge_header.src_vlanptr_or_timestamp_79_64 = encap_info->src_vid;  /* vlan ptr */
    }
    packet.bridge_header.color = CTC_QOS_COLOR_GREEN;              /* highest priority */
    packet.bridge_header.priority = encap_info->packet_priority;                         
    packet.bridge_header.srcport_isolate_id = 0x3F;                /* disable port isolate */
    packet.bridge_header.flowid_servecid_or_oamportid |= 0xFF << 16;  /* disable flow id */
    packet.bridge_header.ttl_or_oam_defect = 20;                   /* TTL of packet */

    packet.bridge_header.pkt_len = buf_len + 4;                    /* add CRC 4 bytes */

    ctc_swap32((uint32_ofp*)&packet.bridge_header, sizeof(ofp_brghdr_info_t)/4, HOST_TO_NETWORK);

    /* calc bridge header CRC */
    p = (uint8_ofp*)(&packet.bridge_header);
    p[OFP_CPU_BRGHDR_CRC_POS] = 0;
    p[OFP_CPU_BRGHDR_CRC_POS] = _ctclib_crc8((uint8_ofp *)&packet.bridge_header, OFP_CPU_PKT_HDR_LEN, 0);

    /*3 encapsulate payload */
    kal_memcpy(packet.data, buf, buf_len);
    packet.data_length = buf_len;

    hardware_process_packet_length = packet.data_length + sizeof(ofp_cpu_mac_header_t) + sizeof(ofp_brghdr_info_t);

    ofpbuf_init(hardware_process_packet, hardware_process_packet_length);
    ofpbuf_put(hardware_process_packet, &packet, hardware_process_packet_length);

    return OFP_ERR_SUCCESS;
}


/**
 * Decapsulate packet with humber bridge header, and return the packet_to_cpu information
 * @param packet                received packet
 * @param packet_to_cpu_info    packet to cpu information, include in_port, reason
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_decap_upcall(struct ofpbuf *packet, ofp_packet_to_cpu_info_t *packet_to_cpu_info)
{
    uint16_ofp ofport = 0;
    ofp_brghdr_info_t* p_brghdr_ptr = NULL;
    uint32_ofp nexthop_ptr = 0;
    adpt_flow_info_t* flow_info_p = NULL;

    OFP_PTR_CHECK(packet);
    OFP_PTR_CHECK(packet_to_cpu_info);
    OFP_LOG_DEBUG_FUNC();

    /* 1. sanity check*/
    if (packet->size < OFP_BRGHDR_LEN)
    {
        OFP_LOG_DEBUG("Discard upcall packet due to invalid packet length %d\n", packet->size);
        return OFP_ERR_FAIL;
    }

    /* 2. pull reserved bytes*/
    ofpbuf_pull(packet, OFP_ETH_HDR_LEN);
    ofpbuf_pull(packet, OFP_CPU_BRGHDR_RESV_BYTES);

    /* 3. Retrieve humber header*/
    p_brghdr_ptr = (ofp_brghdr_info_t *)(packet->data);

    ctc_swap32((uint32_ofp*)(p_brghdr_ptr), OFP_HUMBER_HDR_LEN / 4, HOST_TO_NETWORK);

    nexthop_ptr = p_brghdr_ptr->nexthop_ptr_19_18 << 18 | p_brghdr_ptr->nxt_hop_ptr;

    OFP_DEBUG_PRINT("decap humber hdr, len=%d, src_port=%d, nhptr=0x%x, src_vid=%d\n",
                    p_brghdr_ptr->pkt_len,
                    p_brghdr_ptr->src_port,
                    p_brghdr_ptr->nxt_hop_ptr,
                    p_brghdr_ptr->src_vid);

    /* 4. Get port number from database */
    if (adpt_port_get_ofport_by_gport(p_brghdr_ptr->src_port, &ofport))
    {
        OFP_LOG_DEBUG("Discard upcall packet due to invalid src-port %d\n", p_brghdr_ptr->src_port);
        return OFP_ERR_FAIL;
    }

    /* 5. parse packet_to_cpu info */
    packet_to_cpu_info->in_port = ofport;
    packet_to_cpu_info->flow_id = nexthop_ptr;
    packet_to_cpu_info->packet_in_reason = PACKET_TO_CPU_REASON_MAX;;

    if (FLOW_ID_DEFAULT_FLOW == nexthop_ptr)
    {
        packet_to_cpu_info->packet_in_reason = PACKET_TO_CPU_REASON_MISS_MATCH;
    }
    else if (FLOW_ID_GROUP_FLOW == nexthop_ptr)
    {
        /* TODO not support, need to get group id */
        packet_to_cpu_info->packet_in_reason = PACKET_TO_CPU_REASON_SW_PROCESS_GROUP;
    }
    else if (FLOW_ID_MNGT_DFT_FLOW == nexthop_ptr)
    {
        packet_to_cpu_info->packet_in_reason = PACKET_TO_CPU_REASON_MAX;
    }
    else
    {
        packet_to_cpu_info->packet_in_reason = PACKET_TO_CPU_REASON_SW_PROCESS;
        flow_info_p = adpt_flowdb_get_flow_info(packet_to_cpu_info->flow_id);
        if ((flow_info_p) && (flow_info_p->need_delete == false))
        {
            packet_to_cpu_info->p_rule = flow_info_p->p_rule;
        }
        else
        {
            return OFP_ERR_FAIL;
        }
    }

    /* 6. strip humber header */
    ofpbuf_pull(packet, OFP_HUMBER_HDR_LEN);

    return OFP_ERR_SUCCESS;
}

/**
 *  Handling of packet-out messages for output, hardware forwarding of packets in packet-out messages if we support
 * @param packet input packet buffer
 * @param key flow key
 * @param ofp_actions actions in packet_out messages
 * @param n_actions number of actions in packet_out messages
 * @param hardware_process_packet packet that will be forwarded by hardware, a bridge header will be encapsulated to the original packet
 * @param priority packet priority for qos
 * @return OFP_CPU_PROCESS_DISCARD, OFP_CPU_PROCESS_ENABLE, OFP_CPU_PROCESS_DISABLE
 */
int32_ofp
ofp_send_packet_out(struct ofpbuf *packet, struct flow *key, const union ofp_action *ofp_actions, int32_ofp n_actions, 
                                struct ofpbuf *hardware_process_packet, uint32_ofp priority)
{
    uint8_ofp lport=0, gchip=0;
    uint16_ofp gport = 0, src_gport = 0;
    ofp_cpu_encap_info_t cpu_encap_info;
    uint16_ofp output_ofport = 0;
    ofp_interface_type_t type;
    uint32_ofp nh_offset;
    int32_ofp ret;
    uint16_ofp in_port = 0;

    OFP_PTR_CHECK(packet);
    OFP_PTR_CHECK(key);
    OFP_PTR_CHECK(ofp_actions);
    OFP_PTR_CHECK(hardware_process_packet);
    OFP_LOG_DEBUG_FUNC();

    in_port = key->in_port;

    /* Check if we support the actions, if support hardware forwarding will be performed, otherwise software forwarding will be performed */
    if (ofp_actions->type != htons(OFPAT10_OUTPUT))
    {
        return OFP_CPU_PROCESS_DISCARD;
    }
    output_ofport = ntohs(ofp_actions->output10.port);
    switch (output_ofport)
    {
    case OFPP_MAX:
    case OFPP_PW_FWD:
    case OFPP_IN_PORT:
    case OFPP_NORMAL:
    case OFPP_CONTROLLER:
    case OFPP_LOCAL:
    case OFPP_NONE:
        VLOG_WARN("skipping output to unsupported port: 0x%x\n", output_ofport);
        return OFP_CPU_PROCESS_DISCARD;
        break;

    case OFPP_FLOOD:
    case OFPP_ALL:
        kal_memset(&cpu_encap_info, 0, sizeof(cpu_encap_info));
        src_gport = OFP_DEFAULT_CPU_GPORT_ID;
        ret = adpt_port_get_gport_by_ofport(in_port, &gport);
        if (!ret)
        {
            src_gport = gport;
        }
        cpu_encap_info.destid = adpt_nexthop_get_output_all_group_offset();
        cpu_encap_info.dest_chipid = 0;
        cpu_encap_info.is_mcast    = 1;
        cpu_encap_info.src_vid     = OFP_DEFAULT_VLAN_ID;
        cpu_encap_info.src_port    = src_gport;
        cpu_encap_info.packet_priority = priority;
        ofp_encap_packet(&cpu_encap_info, packet->data, packet->size, hardware_process_packet);
        return OFP_CPU_PROCESS_DISABLE;
        break;

    case OFPP_TABLE:
        return OFP_CPU_PROCESS_ENABLE;
        break;

    default:
        kal_memset(&cpu_encap_info, 0, sizeof(cpu_encap_info));
        ret = adpt_port_get_port_type_by_ofport(output_ofport, &type);

        if (ret || OFP_INTERFACE_TYPE_PHYSICAL != type)
        {
            VLOG_WARN("skipping output to unknown standard port");
            return OFP_CPU_PROCESS_DISCARD;
        }
        ret = adpt_port_get_gport_by_ofport(output_ofport, &gport);
        if (ret)
        {
            return OFP_CPU_PROCESS_DISCARD;
        }
        ret = adpt_port_get_non_edit_nh_offset(gport, &nh_offset);
        if (ret)
        {
            return OFP_CPU_PROCESS_DISCARD;
        }

        gchip = gport >> 8;
        lport = gport & 0x00FF;
        cpu_encap_info.destid = lport;
        cpu_encap_info.dest_chipid = gchip;
        cpu_encap_info.nh_type   = OFP_NEXTHOP_TYPE_OFFSET;
        cpu_encap_info.nh_offset = nh_offset;
        cpu_encap_info.src_vid   = OFP_DEFAULT_VLAN_ID;
        cpu_encap_info.src_port  = gchip<<8 | OFP_DEFAULT_CPU_GPORT_ID;
        cpu_encap_info.is_mcast  = 0;
        cpu_encap_info.hash = 5; /* used to calculate header hash in linkagg */
        cpu_encap_info.packet_priority = priority;
        ofp_encap_packet(&cpu_encap_info, packet->data, packet->size, hardware_process_packet);
        return OFP_CPU_PROCESS_DISABLE;
        break;
    }

    return OFP_CPU_PROCESS_ENABLE;
}

/**
 * Handling of packet-out messages for group, hardware forwarding of packets in packet-out messages if we support.
 * mismatched group will be ignored silently.
 * @param packet input packet buffer
 * @param key flow key
 * @param group_id group id
 * @param hardware_process_packet packet that will be forwarded by hardware, a bridge header will be encapsulated to the original packet
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_send_packet_group(struct ofpbuf *packet, struct flow *key, uint32_ofp group_id, struct ofpbuf *hardware_process_packet)
{
    /**TODO to be implemented */
    return OFP_ERR_SUCCESS;
}

int32_ofp
ofp_ofproto_construct(void)
{
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_add_idle_timeout_timer());

    return OFP_ERR_SUCCESS;
}

int32_ofp
ofp_ofproto_destruct(void)
{
    OFP_LOG_DEBUG_FUNC();

    OFP_ERROR_RETURN(adpt_flow_remove_idle_timeout_timer());

    return OFP_ERR_SUCCESS;
}
