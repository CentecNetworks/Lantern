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
 * @brief This file is the header file of ofp_api.c
 */

#ifndef _OFP_API_H_
#define _OFP_API_H_

/******************************************************************************
* Header Files 
******************************************************************************/
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vlog.h"
#include "byte-order.h"
#include "svec.h"
#include "netdev.h"
#include "netdev-provider.h"

#include "ofp-util.h"
#include "packets.h"
#include "classifier.h"
#include "flow.h"
#include "ofproto/ofproto-provider.h"
#include "ofproto-ctc.h"

#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_stats.h"
#include "ofp_const.h"
#include "ofp_macro.h"
#include "ofp_packet.h"
#include "ofp_flow.h"
#include "ofp_port.h"
#include "ofp_tempfile.h"
#include "ofp_lib.h"

#include "ofp_flow_api.h"
#include "ofp_port_api.h"
#include "ofp_netdev_api.h"

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Decapsulate packet with humber bridge header, and return the packet_to_cpu information
 * @param packet                received packet
 * @param packet_to_cpu_info    packet to cpu information, include in_port, reason
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
int32_ofp
ofp_netdev_decap_upcall(struct ofpbuf *packet, ofp_packet_to_cpu_info_t *packet_to_cpu_info);

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
                    struct ofpbuf *hardware_process_packet, uint32_ofp priority);

/**
 * Handling of packet-out messages for group, hardware forwarding of packets in packet-out messages if we support.
 * mismatched group will be ignored silently.
 * @param packet input packet buffer
 * @param key flow key
 * @param group_id group id
 * @param hardware_process_packet packet that will be forwarded by hardware, a bridge header will be encapsulated to the original packet
 * @return OFP_ERR_SUCCESS
 */
int32_ofp
ofp_send_packet_group(struct ofpbuf *packet, struct flow *key, uint32_ofp group_id, struct ofpbuf *hardware_process_packet);

int32_ofp
ofp_ofproto_construct(void);

int32_ofp
ofp_ofproto_destruct(void);

#endif /* _OFP_API_H_ */
