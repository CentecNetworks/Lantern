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
 * @brief This file is the main file of adapter layer flow
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "afx.h"
#include "sal.h"
#include "sal_mutex.h"

#include "ofp-print.h"
#include "ofp-actions.h"
#include "dynamic-string.h"

#include "ofp_api.h"
#include "ofp_lib.h"

#include "adpt.h"
#include "adpt_port.h"
#include "adpt_flow.h"
#include "adpt_flow_priv.h"
#include "adpt_nexthop.h"
#include "adpt_opf.h"

#include "hal.h"
#include "hal_flow.h"
#include "hal_parser.h"
#include "hal_stats.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
extern sal_mutex_t *pg_mutex;

adpt_flow_master_t* g_p_adpt_flow_master;
VLOG_DEFINE_THIS_MODULE(adapt_flow);

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

void
adpt_flow_destroy_flow_actions(struct list *flow_actions)
{
    adpt_flow_action_t *flow_action, *next;

    LIST_FOR_EACH_SAFE(flow_action, next, list_node, flow_actions) 
    {
        list_remove(&flow_action->list_node);
        free(flow_action);    
    }

    return;
}

static int32_ofp
adpt_flow_translate_reg_load(const struct ofpact *ofpact,
                             adpt_flow_action_t *flow_action)
{
    const struct ofpact_reg_load *load = ofpact_get_REG_LOAD(ofpact);
    const struct mf_field *field = load->dst.field;
    union mf_value value;
    uint64_ofp tun_id = 0;

    bitwise_copy(&(load->subvalue), sizeof load->subvalue, 0, 
                 &value, field->n_bytes, load->dst.ofs, 
                 load->dst.n_bits);
    
    switch (field->id) 
    {
        /* OXM_OF_TUNNEL_ID */
        case MFF_TUN_ID:
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_TUNNEL;
            /* XXX: our chip only support 32-bit gre key, 
             * this will lose precision. */
            tun_id = ntohll(value.be64);
            if (tun_id > OFP_MAX_TUNNEL_ID)
            {
                ADPT_LOG_ERROR("Invalid tun_id: %"PRIu64" in action set_tunnel, tun_id should be in range <1-%u>",
                        tun_id, OFP_MAX_TUNNEL_ID);
                return OFP_ERR_INVALID_SET_TUNNEL_ID;
            }
            flow_action->value.u32 = tun_id;
            break;
            
        /* OXM_OF_ETH_SRC */
        case MFF_ETH_SRC:
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_DL_SRC;
            memcpy(flow_action->value.mac, value.mac, sizeof(mac_addr_t));
            break;
            
        /* OXM_OF_ETH_DST */
        case MFF_ETH_DST:
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_DL_DST;
            memcpy(flow_action->value.mac, value.mac, sizeof(mac_addr_t));
            break;

        /* OXM_OF_VLAN_VID */
        case MFF_VLAN_VID:            
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID;
            /* XXX: this will lost precision. */
            flow_action->value.u16 = 
                ntohs(value.be16) & VLAN_VID_MASK;
            if (flow_action->value.u16 < 1 || flow_action->value.u16 > MAX_VLAN_NUM)
            {
                ADPT_LOG_ERROR("Unsupported vlan_id: %"PRIu16" should be in range <1-4094>", flow_action->value.u16);
                return OFP_ERR_OF13_BAD_SET_FIELD_ARGUMENT;
            }
            break;

        /* OXM_OF_MPLS_LABEL */
        case MFF_MPLS_LABEL:            
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_MPLS_LABEL;
            flow_action->value.u32 = ntohl(value.be32) & ADPT_MPLS_LABEL_MASK;
            break;

        /* OXM_OF_MPLS_TC */
        case MFF_MPLS_TC:            
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_MPLS_TC;
            flow_action->value.u8 = value.u8 & ADPT_MPLS_TC_MASK;
            break;
        
        case MFF_IPV4_DST:
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_NW_DST;
            flow_action->value.u32 = ntohl(value.be32);
            break;

        case MFF_TCP_DST:
        case MFF_UDP_DST:
            flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_TP_DST;
            flow_action->value.u16 = ntohs(value.be16);
            break;

        case MFF_TUN_SRC:
        case MFF_TUN_DST:
        case MFF_TUN_FLAGS:
        case MFF_TUN_TTL:
        case MFF_TUN_TOS:
        case MFF_METADATA:
        case MFF_IN_PORT:
        case MFF_SKB_PRIORITY:
        case MFF_SKB_MARK:
        case MFF_REG0:
        case MFF_REG1:
        case MFF_REG2:
        case MFF_REG3:
        case MFF_REG4:
        case MFF_REG5:
        case MFF_REG6:
        case MFF_REG7:
        case MFF_ETH_TYPE:
        case MFF_VLAN_TCI:
        case MFF_DL_VLAN:
        case MFF_DL_VLAN_PCP:
        case MFF_VLAN_PCP:
        case MFF_MPLS_BOS:
        case MFF_IPV4_SRC:
        case MFF_IPV6_SRC:
        case MFF_IPV6_DST:
        case MFF_IPV6_LABEL:
        case MFF_IP_PROTO:
        case MFF_IP_DSCP:
        case MFF_IP_ECN:
        case MFF_IP_TTL:
        case MFF_IP_FRAG:
        case MFF_ARP_OP:
        case MFF_ARP_SPA:
        case MFF_ARP_TPA:
        case MFF_ARP_SHA:
        case MFF_ARP_THA:
        case MFF_TCP_SRC:
        case MFF_UDP_SRC:
        case MFF_ICMPV4_TYPE:
        case MFF_ICMPV4_CODE:
        case MFF_ICMPV6_TYPE:
        case MFF_ICMPV6_CODE:
        case MFF_ND_TARGET:
        case MFF_ND_SLL:
        case MFF_ND_TLL:
        case MFF_N_IDS:
        default:
            {
                char *action_str = NULL;
                action_str = adpt_flow_get_action_str(ofpact);
                ADPT_LOG_ERROR("Unsupported %s\n", action_str);
                free(action_str);
            }            
            return OFP_ERR_OF13_BAD_SET_FIELD_TYPE;     
    }

    return OFP_ERR_SUCCESS;
}

static int32_ofp
adpt_flow_translate_ofpact__(struct list *flow_actions, 
                             const struct ofpact *ofpacts,
                             int ofpacts_len)                            
{
    const struct ofpact *a;
    adpt_flow_action_t *flow_action;
    int32_ofp err_code = OFP_ERR_SUCCESS;
    
#define MALLOC_FLOW_ACTION(flow_action) \
do \
{ \
    flow_action = (adpt_flow_action_t *)malloc(sizeof(adpt_flow_action_t)); \
    if (NULL == flow_action) \
    { \
        ADPT_LOG_ERROR("Failed to allocate memory " \
                       "when translating ofpactions.\n"); \
        err_code = OFP_ERR_NO_MEMORY; \
        goto err; \
    } \
} while(0)

    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) 
    {
        switch (a->type)
        {
            case OFPACT_OUTPUT:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_OUTPUT;
                    flow_action->value.u16 = ofpact_get_OUTPUT(a)->port;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_PUSH_VLAN:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_PUSH_VLAN;
                    flow_action->value.u16 = 
                        ntohs(ofpact_get_PUSH_VLAN(a)->ethertype);

                    if (flow_action->value.u16 != ETH_TYPE_VLAN_8021Q      &&
                        flow_action->value.u16 != ETH_TYPE_VLAN_8021AD     &&
                        flow_action->value.u16 != ETH_TYPE_VLAN_8021Q_9100 &&
                        flow_action->value.u16 != ETH_TYPE_VLAN_8021Q_9200)
                    {
                        ADPT_LOG_ERROR("Unsupported vlan ethertype: 0x%"PRIx16", should be 0x%"PRIx16", 0x%"PRIx16", 0x%"PRIx16", or 0x%"PRIx16" ",
                            flow_action->value.u16,
                            ETH_TYPE_VLAN_8021Q,
                            ETH_TYPE_VLAN_8021AD,
                            ETH_TYPE_VLAN_8021Q_9100,
                            ETH_TYPE_VLAN_8021Q_9200);

                        err_code = OFP_ERR_BAD_ACTION_ARGUMENT;
                        free(flow_action);
                        goto err;
                    }

                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
                
            case OFPACT_STRIP_VLAN:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_STRIP_VLAN;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_PUSH_MPLS:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_PUSH_MPLS;
                    if (ntohs(ofpact_get_PUSH_MPLS(a)->ethertype) != ETH_TYPE_MPLS)
                    {
                        ADPT_LOG_ERROR("Only MPLS unicast is supported for push_mpls action");
                        err_code = OFP_ERR_BAD_ACTION_ARGUMENT;
                        free(flow_action);
                        goto err;
                    }
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
                
            case OFPACT_POP_MPLS:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_POP_MPLS;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_PUSH_L2:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_PUSH_L2;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
                
            case OFPACT_POP_L2:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_POP_L2;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_SET_TUNNEL:
                {
                    uint64_ofp tun_id = 0;

                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_TUNNEL;
                    tun_id = ofpact_get_SET_TUNNEL(a)->tun_id;
                    if (tun_id > OFP_MAX_TUNNEL_ID)
                    {
                        ADPT_LOG_ERROR("Invalid tun_id: %"PRIu64" in action set_tunnel, tun_id should be in range <1-%u>",
                                tun_id, OFP_MAX_TUNNEL_ID);
                        err_code = OFP_ERR_INVALID_SET_TUNNEL_ID;
                        free(flow_action);
                        goto err;
                    }
                    flow_action->value.u32 = tun_id;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_SET_ETH_SRC:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_DL_SRC;
                    memcpy(flow_action->value.mac, 
                           ofpact_get_SET_ETH_SRC(a)->mac, 
                           sizeof(mac_addr_t));
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
                
            case OFPACT_SET_ETH_DST:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_DL_DST;
                    memcpy(flow_action->value.mac, 
                           ofpact_get_SET_ETH_DST(a)->mac, 
                           sizeof(mac_addr_t));
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
                
            case OFPACT_SET_VLAN_VID:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID;
                    flow_action->value.u16 = 
                        ntohs(ofpact_get_SET_VLAN_VID(a)->vlan_vid) &
                        VLAN_VID_MASK;
                    if (flow_action->value.u16 < 1 || flow_action->value.u16 > MAX_VLAN_NUM)
                    {
                        ADPT_LOG_ERROR("Unsupported vlan_id: %"PRIu16" should be in range <1-4094>", flow_action->value.u16);
                        err_code = OFP_ERR_OF13_BAD_SET_FIELD_ARGUMENT;
                        free(flow_action);
                        goto err;
                    }
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_GROUP:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_GROUP;
                    flow_action->value.u32 = ofpact_get_GROUP(a)->group_id;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_SET_MPLS_TTL:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_MPLS_TTL;
                    if (ofpact_get_SET_MPLS_TTL(a)->mpls_ttl < 2)
                    {
                        ADPT_LOG_ERROR("MPLS ttl to be set must be >= 2");
                        err_code = OFP_ERR_BAD_ACTION_ARGUMENT;
                        free(flow_action);
                        goto err;
                    }
                    flow_action->value.u8 = ofpact_get_SET_MPLS_TTL(a)->mpls_ttl; 
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            /* reg_load is ovs representation of of1.3 set_field. */
            case OFPACT_REG_LOAD:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    err_code = adpt_flow_translate_reg_load(a, flow_action);
                    if (err_code)
                    {
                        free(flow_action);
                        goto err;
                    }
                    list_push_back(flow_actions, &flow_action->list_node);                    
                }
                break;

            /* translate instruction meter to action */
            case OFPACT_METER:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_INSTRUCTION_METER;
                    flow_action->value.u32 = ofpact_get_METER(a)->meter_id;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;

            case OFPACT_SET_QUEUE:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_QUEUE;
                    if (ofpact_get_SET_QUEUE(a)->queue_id >= OFP_MAX_QUEUE_VALUE)
                    {
                        ADPT_LOG_ERROR("QUEUE id must be <= 7");
                        err_code = OFP_ERR_BAD_ACTION_ARGUMENT;
                        free(flow_action);
                        goto err;
                    }
                    flow_action->value.u32 = ofpact_get_SET_QUEUE(a)->queue_id;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
            case OFPACT_SET_IPV4_DST:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_NW_DST;
                    flow_action->value.u32 = ntohl(ofpact_get_SET_IPV4_DST(a)->ipv4);
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
            case OFPACT_SET_L4_DST_PORT:
                {
                    MALLOC_FLOW_ACTION(flow_action);
                    flow_action->type = ADPT_FLOW_ACTION_TYPE_SET_TP_DST;
                    flow_action->value.u16 = ofpact_get_SET_L4_DST_PORT(a)->port;
                    list_push_back(flow_actions, &flow_action->list_node);
                }
                break;
            case OFPACT_DEC_TTL:
            case OFPACT_CONTROLLER:
            case OFPACT_ENQUEUE:
            case OFPACT_OUTPUT_REG:
            case OFPACT_BUNDLE:
            case OFPACT_SET_VLAN_PCP:
            case OFPACT_SET_IPV4_SRC:
            case OFPACT_SET_IPV4_DSCP:
            case OFPACT_SET_L4_SRC_PORT:
            case OFPACT_REG_MOVE:
            case OFPACT_POP_QUEUE:
            case OFPACT_FIN_TIMEOUT:
            case OFPACT_RESUBMIT:
            case OFPACT_LEARN:
            case OFPACT_MULTIPATH:
            case OFPACT_NOTE:
            case OFPACT_EXIT:
            case OFPACT_WRITE_METADATA:
            case OFPACT_CLEAR_ACTIONS:
            case OFPACT_GOTO_TABLE:
            default:
                {
                    char *action_str = NULL;
                    action_str = adpt_flow_get_action_str(a);
                    ADPT_LOG_ERROR("Unsupported %s", action_str);
                    free(action_str);
                }
                err_code = OFP_ERR_UNSUPPORTED_ACTIONS;
                goto err;
        }
    }

    return OFP_ERR_SUCCESS;
    
err:
    adpt_flow_destroy_flow_actions(flow_actions);
    return err_code;
}

/**
 * Translate OVS action to adapter representation.
 * @param[in] ofpacts                   action list
 * @param[in] ofpacts_len               action list length
 * @param[out] p_flow_actions           pointer to the translated flow actions
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_translate_ofpact(struct ofpact *ofpacts, unsigned int ofpacts_len, 
                           struct list *p_flow_actions)
{
    list_init(p_flow_actions);

    ADPT_FLOW_ERROR_RETURN(adpt_flow_translate_ofpact__(
        p_flow_actions, ofpacts, ofpacts_len));
    
    return OFP_ERR_SUCCESS;
}

static bool
adpt_flow_validate_output(struct ofproto_ctc *ofproto, uint16_t ofp_port)
{
    switch (ofp_port) {
    case OFPP_TABLE:
    case OFPP_NORMAL:
    case OFPP_FLOOD:
    case OFPP_LOCAL:
    case OFPP_NONE:
        return false;

    case OFPP_ALL:
    case OFPP_IN_PORT:
    case OFPP_CONTROLLER:
    case OFPP_PW_FWD:
        return true;

    default:
        if (ofp_port >= MAX_PORTS) {
            return false;
        }
        
        if (!ofproto->ports[ofp_port]) {
            return false;
        }

        return true;
    }
}

/**
 * Validate action data.
 * @param[in] p_rule                    pointer to ovs rule
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_validate_action_data(struct rule_ctc *p_rule)
{
    struct ofproto_ctc *ofproto = ofproto_ctc_cast(p_rule->up.ofproto);
    adpt_flow_action_t *flow_action;

    LIST_FOR_EACH(flow_action, list_node, &p_rule->flow_actions) 
    {
        switch (flow_action->type) 
        {
            case ADPT_FLOW_ACTION_TYPE_OUTPUT:
                if (!adpt_flow_validate_output(ofproto, 
                                               flow_action->value.u16)) 
                {
                    ADPT_LOG_ERROR("Validating action data failed, "
                                   "output port %d is not valid", 
                                   flow_action->value.u16);
                    return OFP_ERR_BAD_OUT_PORT;
                }
                break;
         
            case ADPT_FLOW_ACTION_TYPE_SET_NW_DST:
                if (ETH_TYPE_IP != ntohs(p_rule->match.flow.dl_type))
                {
                    ADPT_LOG_ERROR("Invalid flow, the flow match field for mod_nw_dst action does not include DL_TYPE=IPv4");
                    return OFP_ERR_ACTION_AND_MATCH_INCONSISTENT;
                }
                break;

            case ADPT_FLOW_ACTION_TYPE_SET_TP_DST:
                if (IP_PROTO_TCP != p_rule->match.flow.nw_proto &&
                    IP_PROTO_UDP != p_rule->match.flow.nw_proto)
                {
                    ADPT_LOG_ERROR("Invalid flow, the flow match field for mod_tp_dst action does not include NW_PROTO=TCP/UDP");
                    return OFP_ERR_ACTION_AND_MATCH_INCONSISTENT;
                }
                break;

            case ADPT_FLOW_ACTION_TYPE_DEC_NW_TTL:
                if (ETH_TYPE_IP != ntohs(p_rule->match.flow.dl_type))
                {
                    ADPT_LOG_ERROR("Invalid flow, the flow match field for dec_nw_ttl action does not include DL_TYPE=IPv4");
                    return OFP_ERR_ACTION_AND_MATCH_INCONSISTENT;
                }
                break;

            case ADPT_FLOW_ACTION_TYPE_PUSH_VLAN:
            case ADPT_FLOW_ACTION_TYPE_STRIP_VLAN:
            case ADPT_FLOW_ACTION_TYPE_PUSH_L2:
            case ADPT_FLOW_ACTION_TYPE_POP_L2:
            case ADPT_FLOW_ACTION_TYPE_PUSH_MPLS:
            case ADPT_FLOW_ACTION_TYPE_POP_MPLS:
            case ADPT_FLOW_ACTION_TYPE_SET_TUNNEL:
            case ADPT_FLOW_ACTION_TYPE_SET_DL_SRC:
            case ADPT_FLOW_ACTION_TYPE_SET_DL_DST:
            case ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID:
            case ADPT_FLOW_ACTION_TYPE_SET_MPLS_LABEL:
            case ADPT_FLOW_ACTION_TYPE_SET_MPLS_TC:
            case ADPT_FLOW_ACTION_TYPE_SET_MPLS_TTL:
            case ADPT_FLOW_ACTION_TYPE_GROUP:
            case ADPT_FLOW_ACTION_TYPE_INSTRUCTION_METER:
            case ADPT_FLOW_ACTION_TYPE_SET_QUEUE:
            case ADPT_FLOW_ACTION_TYPE_UNSUPPORTED:
            default:
                break;
        }
    }

    return OFP_ERR_SUCCESS;
}

char *
adpt_flow_get_action_str(const struct ofpact * action)
{
    struct ds s;
    char *out;
    
    ds_init(&s);
    ofpacts_format(action, action->len, &s);
    out = xstrdup(ds_cstr(&s));
    ds_destroy(&s);
    
    return out;        
}

/**
 * Map tcp/udp port to sdk data structure
 * @param[in]  l4_port          Layer 4 port
 * @param[out] p_port_range     Pointer of sdk port data structure
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_l4_port(uint16_ofp l4_port, ctc_aclqos_l4_port_t* p_port_range)
{
    ADPT_PTR_CHECK(p_port_range);
    
    p_port_range->operator = CTC_ACLQOS_L4PORT_OPERATOR_EQ;
    p_port_range->l4_port_min = l4_port;
    p_port_range->l4_port_max = l4_port;
    
    return 0;
}

/**
 * Map ether type to layer 3 type
 * @param[in]  ether_type           Ethernet type
 * @param[in]  ether_type_mask      Ethernet type mask
 * @param[out] p_l3type             Pointer of layer 3 type
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_ether_type_l3type(uint16_ofp ether_type, uint16_ofp ether_type_mask, ctc_parser_l3_type_t* p_l3type)
{
    uint8_ofp index = 0xff;

    if (ether_type == 0x0800 && ether_type_mask == 0xffff)
    {
        *p_l3type = CTC_PARSER_L3_TYPE_IPV4;
        return OFP_ERR_SUCCESS;
    }
    if (ether_type == 0x8847 && ether_type_mask == 0xffff)
    {
        *p_l3type = CTC_PARSER_L3_TYPE_MPLS;
        return OFP_ERR_SUCCESS;
    }

    ADPT_FLOW_ERROR_RETURN(adpt_flowdb_insert_map_ether_type_l3_type(ether_type, ether_type_mask, &index, p_l3type));
    if (index != 0xff)
    {
        ADPT_FLOW_ERROR_RETURN(hal_parser_add_l3_type(ether_type, ether_type_mask, index, *p_l3type));
    }
    
    return OFP_ERR_SUCCESS;
}


/**
 * Remove the map of ether type to layer 3 type
 * @param[in]  ether_type           Ethernet type
 * @param[in]  ether_type_mask      Ethernet type mask
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_rem_ether_type_l2type(uint16_ofp ether_type, uint16_ofp ether_type_mask)
{
    uint8_ofp index = 0xff;

    if (ether_type == 0x0800 && ether_type_mask == 0xffff)
    {
        return OFP_ERR_SUCCESS;
    }
    if (ether_type == 0x8847 && ether_type_mask == 0xffff)
    {
        return OFP_ERR_SUCCESS;
    }

    ADPT_FLOW_ERROR_RETURN(adpt_flowdb_remove_map_ether_type_l3_type(ether_type, ether_type_mask, &index));
    if (index != 0xff)
    {
        hal_parser_remove_l2_type(index);
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Map flow to qos mac key
 * @param[in]  p_flow               Pointer of OVS flow data structure
 * @param[in]  p_wc                 Pointer of OVS flow mask structure
 * @param[out] p_qos_mac_key        Pointer of sdk qos mac key
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_mac_key(const struct flow *p_flow, const struct flow *p_wc, ctc_aclqos_mac_key_t* p_qos_mac_key)
{
    ctc_parser_l3_type_t l3type = MAX_CTC_PARSER_L3_TYPE;
    int32_ofp ret = OFP_ERR_SUCCESS;
    
    /* mac source address & mask */
    memcpy(p_qos_mac_key->mac_sa, p_flow->dl_src, sizeof(mac_addr_t));
    memcpy(p_qos_mac_key->mac_sa_mask, p_wc->dl_src, sizeof(mac_addr_t));
    SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_MACSA_FLAG);
    
    /* mac destination address & mask */
    memcpy(p_qos_mac_key->mac_da, p_flow->dl_dst, sizeof(mac_addr_t));
    /* XXX: seems OVS 'match_set_dl_dst_masked' is not corresponding to it's
     * comment and not verify the mask(it said it will verify the mask by 
     * calling 'flow_wildcards_is_dl_dst_mask_valid'), so we copy the mask 
     * directly.
     * Besides, from OFP 1.1, Ethernet Dst/Src can have a mask and OFP does
     * not specify the canonical mask format. */
#if 0
    memcpy(p_qos_mac_key->mac_da_mask, flow_wildcards_to_dl_dst_mask(p_wc->wildcards), sizeof(mac_addr_t));
#endif
    memcpy(p_qos_mac_key->mac_da_mask, p_wc->dl_dst, sizeof(mac_addr_t));
    SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_MACDA_FLAG);
    
    /* ethernet type check */
    if (ntohs(p_wc->dl_type))
    {
        /** If a flow need to match ar_spa, ar_tpa, an unsupported error will be returned */
        if ((ntohs(p_flow->dl_type) == ETH_TYPE_ARP) && 
            ((ntohl(p_wc->nw_src) != 0) ||  (ntohl(p_wc->nw_dst) != 0 ) || p_wc->nw_proto))
        {
            ADPT_LOG_ERROR("Unsupported flow fields: arp_opcode, arp_spa, arp_tpa");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
        }

        /** if dl_type<0x600, an unsupported error will be returned */
        if (ntohs(p_flow->dl_type) < ETH_TYPE_MIN)
        {
            ADPT_LOG_ERROR("Unsupported flow fields: dl_type < 0x0600");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
        }
    }

    OFP_DEBUG_PRINT("mac_key p_wc->vlan_tci_mask: %d eth_type: %u\n", 
                    ntohs(p_wc->vlan_tci), p_qos_mac_key->eth_type);

    /* vlan tci */
    if (ntohs(p_wc->vlan_tci))
    {
        OFP_DEBUG_PRINT("mac_key vlan_tci: %u tci_mask: %d p_wc->vlan_tci_mask & 0x0FFF: %u\n",
                p_flow->vlan_tci, ntohs(p_wc->vlan_tci), ntohs(p_wc->vlan_tci) & VLAN_VID_MASK);

        if (ntohs(p_flow->vlan_tci) == 0) /* Match packets that are not tagged with vlan */
        {
            p_qos_mac_key->svlan = OFP_DEFAULT_VLAN_ID;
            SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_SVLAN_FLAG);
        }
        else
        {
            /* svlan id */
            if (VLAN_VID_MASK == (ntohs(p_wc->vlan_tci) & VLAN_VID_MASK))
            {
                p_qos_mac_key->svlan = vlan_tci_to_vid(p_flow->vlan_tci);
                if (p_qos_mac_key->svlan < 1 || p_qos_mac_key->svlan > MAX_VLAN_NUM)
                {
                    ADPT_LOG_ERROR("Unsupported vlan_id: dl_vlan_id should be in range <1-4094>");
                    return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
                }

                SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_SVLAN_FLAG);
                OFP_DEBUG_PRINT("p_qos_mac_key->svlan: %u\n", p_qos_mac_key->svlan);
            }
            /* stag cos */
            if (VLAN_PCP_MASK == (ntohs(p_wc->vlan_tci) & VLAN_PCP_MASK))
            {
                p_qos_mac_key->cos = (ntohs(p_flow->vlan_tci) & VLAN_PCP_MASK) >> VLAN_PCP_SHIFT;
                SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_COS_FLAG);
                OFP_DEBUG_PRINT("p_qos_mac_key->cos: %u\n", p_qos_mac_key->cos);
            }
        }
    }

    /* ethernet type, only for mac entry */
    if (ntohs(p_wc->dl_type) == OFP_UINT16_MAX)
    {
        ret = adpt_flow_map_ether_type_l3type(ntohs(p_flow->dl_type), 0xffff, &l3type);
        if (ret || (MAX_CTC_PARSER_L3_TYPE == l3type))
        {
            ADPT_LOG_ERROR("Adding flow entry ether_type 0x%x failed, only at most %u different ether-types (not include 0x0800) are supported",
                ntohs(p_flow->dl_type),
                OFP_ETHER_TYPE_MAX_NUM);
            return OFP_ERR_ETHER_TYPE_FULL;
        }
        p_qos_mac_key->l3_type = l3type;
        SET_FLAG(p_qos_mac_key->flag, CTC_ACLQOS_MAC_KEY_L3TYPE_FLAG);
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Map flow to qos ipv4 key
 * @param[in]  p_flow               Pointer of OVS flow data structure
 * @param[in]  p_wc                 Pointer of OVS flow wildcards data structure
 * @param[out] p_qos_ipv4_key       Pointer of sdk qos ipv4 key
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_ipv4_key(const struct flow *p_flow, const struct flow *p_wc, ctc_aclqos_ipv4_key_t *p_qos_ipv4_key)
{
    /* ip source address & mask */
    if (ntohl(p_wc->nw_src))
    {
        p_qos_ipv4_key->ip_sa = ntohl(p_flow->nw_src);
        p_qos_ipv4_key->ip_sa_mask = ntohl(p_wc->nw_src);
        SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_IPSA_FLAG);
    }
    
    /* ip destination address & mask */
    if (ntohl(p_wc->nw_dst))
    {
        p_qos_ipv4_key->ip_da = ntohl(p_flow->nw_dst);
        p_qos_ipv4_key->ip_da_mask = ntohl(p_wc->nw_dst);
        SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_IPDA_FLAG);
    }

    /* layer 3 protocol */
    if (0 != p_wc->nw_proto)
    {
        p_qos_ipv4_key->l4_protocol = p_flow->nw_proto;
        SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_L4PROTO_FLAG);
        OFP_DEBUG_PRINT("nw_proto: %u\n", p_qos_ipv4_key->l4_protocol);

        /* icmp */
        if (p_flow->nw_proto == IP_PROTO_ICMP)
        {
            /* icmp type */
            if (0 != p_wc->tp_src)
            {
                p_qos_ipv4_key->icmp_type = (uint8_ofp) ntohs(p_flow->tp_src);
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_ICMPTYPE_FLAG);
            }

            /* icmp code */
            if (0 != p_wc->tp_dst)
            {
                p_qos_ipv4_key->icmp_code = (uint8_ofp) ntohs(p_flow->tp_dst);
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_ICMPCODE_FLAG);
            }
        }
        /* tcp or udp */
        else if (p_flow->nw_proto == IP_PROTO_TCP || p_flow->nw_proto == IP_PROTO_UDP)
        {
            /* src port */
            if (0 != p_wc->tp_src)
            {
                adpt_flow_map_l4_port(ntohs(p_flow->tp_src), &p_qos_ipv4_key->l4_src_port);
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_L4SRCPORT_FLAG);
            }

            /* dst port */
            if (0 != p_wc->tp_dst)
            {
                adpt_flow_map_l4_port(ntohs(p_flow->tp_dst), &p_qos_ipv4_key->l4_dst_port);
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_L4DSTPORT_FLAG);
            }
        }
    }

    /* ip options */
    if (0 != p_wc->nw_tos)
    {
        p_qos_ipv4_key->dscp = p_flow->nw_tos >> 2;
        SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_DSCP_FLAG);
    }

    /* mac source address & mask */
     memcpy(p_qos_ipv4_key->mac_sa, p_flow->dl_src, sizeof(mac_addr_t));
     memcpy(p_qos_ipv4_key->mac_sa_mask, p_wc->dl_src, sizeof(mac_addr_t));
     SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_MAC_KEY_MACSA_FLAG);


    /* mac destination address & mask */
    memcpy(p_qos_ipv4_key->mac_da, p_flow->dl_dst, sizeof(mac_addr_t));
    memcpy(p_qos_ipv4_key->mac_da_mask, p_wc->dl_dst, sizeof(mac_addr_t));
    SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_MACDA_FLAG);

    OFP_DEBUG_PRINT("ipv4 p_wc->vlan_tci_mask: %d\n", ntohs(p_wc->vlan_tci));

    /* vlan tci */
    if (ntohs(p_wc->vlan_tci))
    {
        OFP_DEBUG_PRINT("ipv4_key vlan_tci: %u tci_mask: %d p_wc->vlan_tci_mask & 0x0FFF: %u\n",
                p_flow->vlan_tci, ntohs(p_wc->vlan_tci), ntohs(p_wc->vlan_tci) & VLAN_VID_MASK);

        if (ntohs(p_flow->vlan_tci) == 0) /* Match packets that are not tagged with vlan */
        {
            p_qos_ipv4_key->svlan = 4095;
            SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_SVLAN_FLAG);
        }
        else
        {
            /* svlan id */
            if (VLAN_VID_MASK == (ntohs(p_wc->vlan_tci) & VLAN_VID_MASK))
            {
                p_qos_ipv4_key->svlan = vlan_tci_to_vid(p_flow->vlan_tci);
                /** VLAN ID should be in ranged 1-4094 */
                if (p_qos_ipv4_key->svlan < 1 || p_qos_ipv4_key->svlan > MAX_VLAN_NUM)
                {
                    ADPT_LOG_ERROR("Unsupported vlan_id: dl_vlan_id should be in range <1-4094>");
                    return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
                }
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_SVLAN_FLAG);
                OFP_DEBUG_PRINT("p_qos_ipv4_key->svlan: %u\n", p_qos_ipv4_key->svlan);
            }
            /* stag cos */
            if (VLAN_PCP_MASK == (ntohs(p_wc->vlan_tci) & VLAN_PCP_MASK))
            {
                p_qos_ipv4_key->cos = (ntohs(p_flow->vlan_tci) & VLAN_PCP_MASK) >> VLAN_PCP_SHIFT;
                SET_FLAG(p_qos_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_COS_FLAG);
                OFP_DEBUG_PRINT("p_qos_ipv4_key->cos: %u\n", p_qos_ipv4_key->cos);
            }
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Map flow to qos mpls key
 * @param[in]  p_rule               Pointer of struct rule_ctc
 * @param[in]  p_wc                 Pointer of OVS flow wildcards data structure
 * @param[out] p_qos_mpls_key       Pointer of sdk qos mpls key
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_mpls_key(struct rule_ctc *p_rule, ctc_aclqos_mpls_key_t *p_qos_mpls_key)
{
    const struct flow *flow = &p_rule->match.flow;
    const struct flow *wc = &p_rule->match.wc.masks;
    const uint32_ofp mpls_label = (ntohl(flow->mpls_lse) & MPLS_LABEL_MASK) >> MPLS_LABEL_SHIFT;

    /* XXX: OVS master branch does not use FWW_* bits anymore, so it's hard
     * and silly to check if match field other than MPLS_LABEL and DL_TYPE 
     * present exist, so we only check label and dl_type field. */

    if ((!mpls_label) || (!wc->dl_type))
    {
        ADPT_LOG_ERROR("Matching field of the MPLS egress flow is invalid, "
                       "MPLS label and DL_TYPE=0x8847 must be both present on "
                       "the flow\n");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }

    if (ntohs(flow->dl_type) == 0x8848)
    {
        ADPT_LOG_ERROR("Matching field of the MPLS egress flow is invalid, "
                       "only MPLS unicast(0x8847) is supported.\n");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }
    
    p_qos_mpls_key = p_qos_mpls_key;
    /*record label in p_rule*/
    p_rule->mpls_label = mpls_label;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Map rule to sdk qos key
 * @param[in]  p_rule               Pointer of struct rule_ctc
 * @param[out] p_qos_key            Pointer of sdk qos key
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_flow_key(struct rule_ctc *p_rule, ctc_aclqos_key_t *p_qos_key)
{
    ctc_aclqos_mac_key_t*  p_mac_key = NULL;
    ctc_aclqos_ipv4_key_t* p_ipv4_key = NULL;
    ctc_aclqos_mpls_key_t* p_mpls_key = NULL;
    const struct flow *flow = &p_rule->match.flow;
    const struct flow *masks = &p_rule->match.wc.masks;

    if (ntohs(masks->dl_type) &&
        ntohs(masks->dl_type) != OFP_UINT16_MAX)
    {
        ADPT_LOG_ERROR("Unsupported dl_type mask 0x%x, should be 0x%x or 0x0",
            ntohs(masks->dl_type),
            OFP_UINT16_MAX);
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD_VALUE;
    }

    if ((ntohs(flow->dl_type) == 0x8847) || (ntohs(flow->dl_type) == 0x8848)) 
    {
        /*The ctc_aclqos_mpls_key_t is not used currently, leave it for future extension,
          this is mpls egress flow, we will not use acl at all*/
        p_qos_key->type = CTC_ACLQOS_MPLS_KEY;
        p_mpls_key = &p_qos_key->key_info.mpls_key;
        ADPT_FLOW_ERROR_RETURN(adpt_flow_map_mpls_key(p_rule, p_mpls_key));
    }
    else if (ntohs(flow->dl_type) == 0x0800)
    {
        p_qos_key->type = CTC_ACLQOS_IPV4_KEY;
        p_ipv4_key = &p_qos_key->key_info.ipv4_key;
        ADPT_FLOW_ERROR_RETURN(adpt_flow_map_ipv4_key(flow, masks, p_ipv4_key));
    }
    else
    {
        p_qos_key->type = CTC_ACLQOS_MAC_KEY;
        p_mac_key = &p_qos_key->key_info.mac_key;
        ADPT_FLOW_ERROR_RETURN(adpt_flow_map_mac_key(flow, masks, p_mac_key));
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Remove the resource during mapping rule to sdk qos key
 * @param  p_rule               Pointer of struct rule_ctc
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_remove_flow_key(const struct rule_ctc *p_rule)
{
    const struct flow *flow = &p_rule->match.flow;
    const struct flow *masks = &p_rule->match.wc.masks;

    if (ntohs(masks->dl_type) == OFP_UINT16_MAX)
    {
        adpt_flow_rem_ether_type_l2type(ntohs(flow->dl_type), 0xffff);
    }
    
    return 0;
}

/**
 * Function will be called by adpt_flow_map_mac_ipv4_entry
 * @param[in]  p_mac_key        Pointer to qos mac entry
 * @param[out] p_ipv4_key       Pointer to qos ipv4 entry
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_mac_ipv4_key(ctc_aclqos_mac_key_t* p_mac_key, ctc_aclqos_ipv4_key_t* p_ipv4_key)
{
    /* mac source address & mask */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_MACSA_FLAG))
    {
        memcpy(p_ipv4_key->mac_sa, p_mac_key->mac_sa, sizeof(mac_addr_t));
        memcpy(p_ipv4_key->mac_sa_mask, p_mac_key->mac_sa_mask, sizeof(mac_addr_t));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_MACSA_FLAG);
    }

    /* mac destination address & mask */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_MACDA_FLAG))
    {
        memcpy(p_ipv4_key->mac_da, p_mac_key->mac_da, sizeof(mac_addr_t));
        memcpy(p_ipv4_key->mac_da_mask, p_mac_key->mac_da_mask, sizeof(mac_addr_t));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_MACDA_FLAG);
    }

    /* svlan id */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_SVLAN_FLAG))
    {
        memcpy(&p_ipv4_key->svlan, &p_mac_key->svlan, sizeof(p_mac_key->svlan));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_SVLAN_FLAG);
    }

    /* stag cos */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_STAG_COS_FLAG))
    {
        memcpy(&p_ipv4_key->stag_cos, &p_mac_key->stag_cos, sizeof(p_mac_key->stag_cos));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_STAG_COS_FLAG);
    }

    /* stag cfi */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_STAG_CFI_FLAG))
    {
        memcpy(&p_ipv4_key->stag_cfi, &p_mac_key->stag_cfi, sizeof(p_mac_key->stag_cfi));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_STAG_CFI_FLAG);
    }

    /* cos */
    if (IS_FLAG_SET(p_mac_key->flag, CTC_ACLQOS_MAC_KEY_COS_FLAG))
    {
        memcpy(&p_ipv4_key->cos, &p_mac_key->cos, sizeof(p_mac_key->cos));
        SET_FLAG(p_ipv4_key->flag, CTC_ACLQOS_IPV4_KEY_COS_FLAG);
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Map qos mac entry to qos ipv4 entry, including action and key
 * @param[in]  p_mac_key        Pointer to qos mac entry
 * @param[out] p_ipv4_key       Pointer to qos ipv4 entry
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_mac_ipv4_entry(ctc_aclqos_entry_t* from_mac_entry, ctc_aclqos_entry_t* to_qos_entry)
{
    to_qos_entry->key.type = CTC_ACLQOS_IPV4_KEY;
    
    ADPT_FLOW_ERROR_RETURN(adpt_flow_map_mac_ipv4_key(&from_mac_entry->key.key_info.mac_key, &to_qos_entry->key.key_info.ipv4_key));
    memcpy(&to_qos_entry->action, &from_mac_entry->action, sizeof(ctc_aclqos_action_t));

    return OFP_ERR_SUCCESS;
}

/**
 * Print QoS entry
 * @param label_type           label type
 * @param label_id             QoS label id
 * @param p_entry              Pointer to Qos entry
 * @return OFP_ERR_XXX
 */
static void
adpt_flow_print_entry(ctc_aclqos_label_type_t label_type, uint32_ofp label_id, ctc_aclqos_entry_t *p_entry)
{
    if (p_entry->key.type == CTC_ACLQOS_MAC_KEY)
    {
        OFP_DEBUG_PRINT("MAC Key\n");
        OFP_DEBUG_PRINT("label_id: %u action_flag: %d entry_id:%u key_type:%u\n", label_id, p_entry->action.flag, p_entry->entry_id, p_entry->key.type);
        OFP_DEBUG_PRINT("svlan id: %u cvlan id: %u s_cfi: %u s_cos: %u mac_key_flag: %u\n",
                p_entry->key.key_info.mac_key.svlan, p_entry->key.key_info.mac_key.cvlan, p_entry->key.key_info.mac_key.stag_cfi, p_entry->key.key_info.mac_key.stag_cos, p_entry->key.key_info.mac_key.flag);
        OFP_DEBUG_PRINT("policer_id: %u\n", p_entry->action.policer_id);
        OFP_DEBUG_PRINT("MAC-DA: "ETH_ADDR_FMT" MAC-DA-MASK: "ETH_ADDR_FMT"\n", ETH_ADDR_ARGS(p_entry->key.key_info.mac_key.mac_da), ETH_ADDR_ARGS(p_entry->key.key_info.mac_key.mac_da_mask));
        OFP_DEBUG_PRINT("MAC-SA: "ETH_ADDR_FMT" MAC-SA-MASK: "ETH_ADDR_FMT"\n", ETH_ADDR_ARGS(p_entry->key.key_info.mac_key.mac_sa), ETH_ADDR_ARGS(p_entry->key.key_info.mac_key.mac_sa_mask));
    }
    else if (p_entry->key.type  == CTC_ACLQOS_IPV4_KEY)
    {
        OFP_DEBUG_PRINT("IPv4 Key\n");
        OFP_DEBUG_PRINT("label_id: %u action_flag: %d entry_id:%u key_type:%u\n", label_id, p_entry->action.flag, p_entry->entry_id, p_entry->key.type);
        OFP_DEBUG_PRINT(
                "svlan id: %u cvlan id: %u s_cfi: %u s_cos: %u ipv4_key_flag: %u\n",
                p_entry->key.key_info.ipv4_key.svlan, p_entry->key.key_info.ipv4_key.cvlan, p_entry->key.key_info.ipv4_key.stag_cfi, p_entry->key.key_info.ipv4_key.stag_cos, p_entry->key.key_info.ipv4_key.flag);
        OFP_DEBUG_PRINT("l3_proto: %u, l4_proto: %u dscp: %u\n", p_entry->key.key_info.ipv4_key.l3_type, p_entry->key.key_info.ipv4_key.l4_protocol, p_entry->key.key_info.ipv4_key.dscp);
        OFP_DEBUG_PRINT("policer_id: %u\n", p_entry->action.policer_id);
        OFP_DEBUG_PRINT("TCP/UDP tp_src: %u tp_dst: %u ICMP-CODE: %u ICMP-TYPE: %u\n", p_entry->key.key_info.ipv4_key.l4_src_port.l4_port_min, p_entry->key.key_info.ipv4_key.l4_dst_port.l4_port_min,  p_entry->key.key_info.ipv4_key.icmp_code,  p_entry->key.key_info.ipv4_key.icmp_type);
        OFP_DEBUG_PRINT("IPV4-MAC-DA: "ETH_ADDR_FMT" IPV4-MAC-DA-MASK: "ETH_ADDR_FMT"\n", ETH_ADDR_ARGS(p_entry->key.key_info.ipv4_key.mac_da), ETH_ADDR_ARGS(p_entry->key.key_info.ipv4_key.mac_da_mask));
        OFP_DEBUG_PRINT("IPV4-MAC-SA: "ETH_ADDR_FMT" IPV4-MAC-SA-MASK: "ETH_ADDR_FMT"\n", ETH_ADDR_ARGS(p_entry->key.key_info.ipv4_key.mac_sa), ETH_ADDR_ARGS(p_entry->key.key_info.ipv4_key.mac_sa_mask));
        OFP_DEBUG_PRINT("IPV4-DA:"IP_FMT" IPV4-DA-MASK:"IP_FMT"\n", 
            IP_ARGS(htonl(p_entry->key.key_info.ipv4_key.ip_da)), 
            IP_ARGS(htonl(p_entry->key.key_info.ipv4_key.ip_da_mask)));
        OFP_DEBUG_PRINT("IPV4-SA:"IP_FMT" IPV4-SA-MASK:"IP_FMT"\n", 
            IP_ARGS(htonl(p_entry->key.key_info.ipv4_key.ip_sa)), 
            IP_ARGS(htonl(p_entry->key.key_info.ipv4_key.ip_sa_mask)));
    }

}

/** 
 * check output for the specified rule
 * @param[in] flow_actions       List of flow actions
 * @param[out] p_nexthop_info    Nexthop information
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_get_output_count(struct list *flow_actions, ofp_nexthop_info_t* p_nexthop_info)
{
    adpt_flow_action_t *flow_action;
    
    uint32_ofp port;
    uint32_ofp all_port_num;
    uint32_ofp count = 0;
    uint32_ofp tunnel_count = 0;
    
    ADPT_FLOW_ERROR_RETURN(adpt_port_get_phy_port_num(&all_port_num));

    LIST_FOR_EACH(flow_action, list_node, flow_actions) 
    {
        if (ADPT_FLOW_ACTION_TYPE_OUTPUT == flow_action->type) 
        {
            port = flow_action->value.u16;
            if (OFPP_ALL == port)
                count += all_port_num;
            else
                count++;

            if (adpt_port_is_tunnel_port(port))
                tunnel_count ++;
        }
    }

    /* Plus to nexthop_info, used for group buckets */
    p_nexthop_info->output_count += count;
    p_nexthop_info->gre_and_mpls_push_output_count += tunnel_count;
     
    return true;      
}

/** 
 * check output action for the specified rule
 * @param[in] flow_actions       List of flow actions
 * @param[out] p_nh_info         pointer to ofp_nexthop_info_t
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_check_output_action(struct list *flow_actions, ofp_nexthop_info_t* p_nh_info)
{
    ADPT_FLOW_ERROR_RETURN(adpt_flow_get_output_count(flow_actions, p_nh_info));
    
    if (p_nh_info->output_count > MAX_OUTPUT_PORT)
    {
        ADPT_LOG_ERROR("Trying to add more than [%d] output for a single flow/group, current specified output count is [%d].\n",
                      MAX_OUTPUT_PORT, p_nh_info->output_count);
        return OFP_ERR_TOO_MANY_OUTPUT_SINGLE_FLOW;
    }

    /* Check nexthop resource */
    if (OFP_ERR_SUCCESS != adpt_flow_op_nexthop_res(p_nh_info, ADPT_RES_OP_TYPE_CHECK))
    {
        return OFP_ERR_ALL_TABLES_FULL;
    }
     
    return OFP_ERR_SUCCESS;      
}

/** 
 * add a combo action that its about to output to all
 * @param[in]  p_action             OVS rule actions(layer 2 field modification and output)
 * @param[out] p_member_nh_array    Pointer to next-hop members
 * @param[out] p_member_cnt         Pointer to next-hop member count
 * @return OFP_ERR_SUCCESS, OFP_ERR_FAIL
 */
 static int32_ofp
 adpt_flow_map_all_to_mcast_members(adpt_flow_action_combo_t* p_action, 
                                     ofp_nh_offset_t* p_member_nh_array,
                                     uint32_ofp* p_member_cnt)
{
    ofp_nh_offset_t member_nh;
    uint16_ofp gport;
    uint32_ofp port_num;

    memset(&member_nh, 0, sizeof(member_nh));

    ADPT_FLOW_ERROR_RETURN(adpt_port_get_phy_port_num(&port_num));

    for (gport = 0; gport < port_num; gport++)
    {
        p_action->output_gport = gport;
        SET_FLAG(p_action->flag, OFP_FLOW_ACTION_FIELD_OUTPUT);
        ADPT_FLOW_ERROR_RETURN(adpt_nexthop_alloc_flex_nh(p_action, &member_nh));

        p_member_nh_array[*p_member_cnt].nhid    = member_nh.nhid;
        p_member_nh_array[*p_member_cnt].offset  = member_nh.offset;
        p_member_nh_array[*p_member_cnt].nh_type = member_nh.nh_type;
        p_member_nh_array[*p_member_cnt].port_check_discard = TRUE;
        *p_member_cnt = *p_member_cnt + 1;
    }
    
    return OFP_ERR_SUCCESS;    
}

/**
 * allocate the multicast group id
 * @param[in]  p_action          adpt_flow_action_t
 * @param[out] p_action_combo    adpt_flow_action_combo_t
 * @return OFP_ERR_XX
 */
int32_ofp
adpt_flow_map_combo_action(const adpt_flow_action_t *p_action, adpt_flow_action_combo_t *p_action_combo)
{
    uint32_ofp tun_id = 0;
    uint32_ofp queue_id = 0;
    uint32_ofp meter_id = 0;

    switch (p_action->type)
    {
        case ADPT_FLOW_ACTION_TYPE_OUTPUT:
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_OUTPUT);
            break;

        case ADPT_FLOW_ACTION_TYPE_SET_VLAN_VID:
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, should not modify vlan tag after strip vlan tags.");
                return OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP;
            }
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, should not modify vlan tag after strip two vlan tags.");
                return OFP_ERR_ACTION_VLAN_MOD_AFT_STRIP_TWO;
            }
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN))
            {
                p_action_combo->vlan_id = p_action->value.u16;
            }
            else
            {
                p_action_combo->vlan_id = p_action->value.u16;
                SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID);
            }

            break;

        case ADPT_FLOW_ACTION_TYPE_STRIP_VLAN:
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, more than two strip_vlan actions are appeared.");
                return OFP_ERR_ACTION_TOO_MANY_STRIP_VLAN;
            }
            /* Can not strip vlan after insert vlan */
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN)||
                IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_CVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, should not strip vlan after push vlan tag.");
                return OFP_ERR_ACTION_VLAN_STRIP_AFT_PUSH;
            }
            /* Take "mod_vlan_vid,strip_vlan" as "strip_vlan" */
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID) ||
                IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_COS))
            {
                UNSET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID);
                UNSET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_COS);
            }
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN))
            {
                SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN);
            }
            else
            {
                SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN);
            }
            break;
            
        case ADPT_FLOW_ACTION_TYPE_SET_DL_SRC:
            memcpy(p_action_combo->mac_sa, p_action->value.mac, sizeof(mac_addr_t));
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_MACSA);
            break;
            
        case ADPT_FLOW_ACTION_TYPE_SET_DL_DST:
            memcpy(p_action_combo->mac_da, p_action->value.mac, sizeof(mac_addr_t));
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_MACDA);
            break;
            
        case ADPT_FLOW_ACTION_TYPE_SET_TUNNEL:
            tun_id = p_action->value.u32;
            if (0 == tun_id || tun_id > OFP_MAX_TUNNEL_ID)
            {
                ADPT_LOG_ERROR("Invalid tun_id: %"PRIu32" in action set_tunnel, tun_id should be in range <1-%u>",
                                        tun_id, OFP_MAX_TUNNEL_ID);
                return OFP_ERR_INVALID_SET_TUNNEL_ID;
            }
            p_action_combo->tunnel_id = tun_id;
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_TUNNEL_ID);
            break;

        case ADPT_FLOW_ACTION_TYPE_PUSH_VLAN:
            /* Should not push vlan twice more */
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_CVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, too many push_vlan actions are appeared.");
                return OFP_ERR_ACTION_TOO_MANY_PUSH_VLAN;
            }
            /* Should not push vlan after strip vlan */
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_SVLAN)||
                IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_STRIP_CVLAN))
            {
                OFP_LOG_ERROR("Adding flow/group failed, should not push vlan after strip vlan tag.");
                return OFP_ERR_ACTION_VLAN_PUSH_AFT_STRIP;
            }
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_SVLAN_VID))
            {
                OFP_LOG_ERROR("Adding flow/group failed, should not push vlan after modify vlan tag.");
                return OFP_ERR_ACTION_VLAN_PUSH_AFT_MOD;
            }
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN))
            {
                /* Take the first pushed vlan as customer vlan tag */
                SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_CVLAN);
                p_action_combo->cvlan_id  = p_action_combo->vlan_id;
                p_action_combo->ctag_tpid = p_action_combo->stag_tpid;
                p_action_combo->stag_tpid = p_action->value.u16;
            }
            else
            {
                SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_PUSH_SVLAN);
                p_action_combo->stag_tpid = p_action->value.u16;
            }
            break;

        case ADPT_FLOW_ACTION_TYPE_INSTRUCTION_METER:
            meter_id = p_action->value.u32;
            p_action_combo->meter_id = meter_id;
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_INSTRUCTION_METER);
            break;

        case ADPT_FLOW_ACTION_TYPE_SET_QUEUE:
            queue_id = p_action->value.u32;
            if (queue_id >= OFP_MAX_QUEUE_VALUE)
            {
                ADPT_LOG_ERROR("Unsupported queue_id: %"PRIu32" in action set_queue: queue_id should be in range <0-%u>", 
                    queue_id, (OFP_MAX_QUEUE_VALUE - 1));
                return OFP_ERR_BAD_ACTION_ARGUMENT;
            }
            p_action_combo->queue_id = queue_id;
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_SET_QUEUE_ID);
            break;

        case ADPT_FLOW_ACTION_TYPE_SET_NW_DST:
            p_action_combo->ipda = p_action->value.u32;
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_IPDA);
            break;

        case ADPT_FLOW_ACTION_TYPE_SET_TP_DST:
            p_action_combo->dst_port = p_action->value.u16;
            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_REPLACE_DST_PORT);
            break;

        case ADPT_FLOW_ACTION_TYPE_DEC_NW_TTL:
            if (IS_FLAG_SET(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_DEC_IP_TTL))
            {
                ADPT_LOG_ERROR("Adding flow failed, too many DEC_NW_TTL");
                return OFP_ERR_INVALID_ACTION_LIST;
            }

            SET_FLAG(p_action_combo->flag, OFP_FLOW_ACTION_FIELD_DEC_IP_TTL);
            break;

        case ADPT_FLOW_ACTION_TYPE_PUSH_L2:
        case ADPT_FLOW_ACTION_TYPE_POP_L2:
        case ADPT_FLOW_ACTION_TYPE_PUSH_MPLS:
        case ADPT_FLOW_ACTION_TYPE_POP_MPLS:
        case ADPT_FLOW_ACTION_TYPE_SET_MPLS_LABEL:
        case ADPT_FLOW_ACTION_TYPE_SET_MPLS_TC:
        case ADPT_FLOW_ACTION_TYPE_SET_MPLS_TTL:
        case ADPT_FLOW_ACTION_TYPE_GROUP:
        case ADPT_FLOW_ACTION_TYPE_UNSUPPORTED:
        default:
            ADPT_LOG_ERROR("Fatal error, unsupported adpt_flow_action type %d.\n", 
                           p_action->type);
            return OFP_ERR_INVALID_PARAM;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Map output action to mcast members
 * @param[in]  in_port             in_port
 * @param[in]  flow_actions        List of flow actions
 * @param[in]  flow_id             Flow id
 * @param[out] p_member_nh_array   Pointer of member nexthop array
 * @param[out] p_member_cnt        Pointer of member count
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_map_output_action_to_mcast_members(uint16_ofp in_port, 
                                    const struct list *flow_actions, uint32_ofp flow_id,
                                    ofp_nh_offset_t* p_member_nh_array, 
                                    uint32_ofp* p_member_cnt)
{
    adpt_flow_action_t *flow_action;
    int ofp_port = 0;
    adpt_flow_action_combo_t action_combo;
    ofp_nh_offset_t member_nh;
    bool output_to_controller = false;

    memset(&action_combo, 0, sizeof(action_combo));

    LIST_FOR_EACH(flow_action, list_node, flow_actions) 
    {        
        ADPT_FLOW_ERROR_RETURN(adpt_flow_map_combo_action(flow_action, &action_combo));

        if (ADPT_FLOW_ACTION_TYPE_OUTPUT != flow_action->type)
        {
            continue;
        }

        ofp_port = flow_action->value.u16;
        
        if (adpt_port_is_physical_port(ofp_port))
        {
            ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofp_port, &action_combo.output_gport));
            ADPT_FLOW_ERROR_RETURN(adpt_nexthop_alloc_flex_nh(&action_combo, &member_nh));
        }
        else if (adpt_port_is_tunnel_port(ofp_port))
        {
            if (adpt_tunnel_alloc_encap_nhid(ofp_port, &action_combo, &member_nh))
            {
                ADPT_LOG_ERROR("Fail to add flow entry, only at most %d gre and mpls push output are supported", 
                    adpt_flowdb_get_gre_and_mpls_push_output_max());

                return OFP_ERR_ALL_TABLES_FULL;
            }
        }
        else if (OFPP_CONTROLLER == ofp_port)
        {
            /* Create the nhid later */
            output_to_controller = true;
            continue;
        }
        else if(OFPP_IN_PORT == ofp_port)
        {
            /* if the p_rule is per-port p_rule, we can support in fast-path, if the p_rule is global-p_rule, we can only support in slow-path */
            if (0 == in_port)
            {
                /* Create the nhid later */
                output_to_controller = true;
                continue;
            }
            else if (OFP_INVALID_OFP_PORT == in_port)
            {
                ADPT_LOG_ERROR("Unable to find output port: %d\n", ofp_port);
                return OFP_ERR_BAD_OUT_PORT;
            }
            else if (adpt_port_is_physical_port(in_port))
            {
                ADPT_FLOW_ERROR_RETURN(
                    adpt_port_get_gport_by_ofport(in_port, &action_combo.output_gport));
                ADPT_FLOW_ERROR_RETURN(adpt_nexthop_alloc_flex_nh(&action_combo, &member_nh));
            }
            else if (adpt_port_is_tunnel_port(in_port))
            {
                ADPT_LOG_ERROR("Invalid action:in_port for gre in_port %d\n", in_port);
                return OFP_ERR_BAD_OUT_PORT;
            }
        }
        else if(OFPP_ALL == ofp_port)
        {
            ADPT_FLOW_ERROR_RETURN(adpt_flow_map_all_to_mcast_members(
                                            &action_combo, 
                                            p_member_nh_array,
                                            p_member_cnt));
            continue;
        }
        else
        {
            ADPT_LOG_ERROR("Unable to find output port: %d\n", ofp_port);
            return OFP_ERR_BAD_OUT_PORT;
        }
        
        p_member_nh_array[*p_member_cnt].nhid    = member_nh.nhid;
        p_member_nh_array[*p_member_cnt].offset  = member_nh.offset;
        p_member_nh_array[*p_member_cnt].nh_type = member_nh.nh_type;
        p_member_nh_array[*p_member_cnt].port_check_discard = FALSE;

        (*p_member_cnt) ++;
    }
        
    if (output_to_controller)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_nexthop_alloc_to_cpu_flex_nh(flow_id, &member_nh));

        p_member_nh_array[*p_member_cnt].nhid    = member_nh.nhid;
        p_member_nh_array[*p_member_cnt].offset  = member_nh.offset;
        p_member_nh_array[*p_member_cnt].nh_type = member_nh.nh_type;
        p_member_nh_array[*p_member_cnt].port_check_discard = FALSE;

        (*p_member_cnt) ++;
    }

    return OFP_ERR_SUCCESS;
}

/** 
 * Map a flow action to a nexthop and also create the nexthop
 * @param[in] p_rule            struct rule_ctc
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_add_output_action(struct rule_ctc* p_rule)
{
    uint32_ofp member_cnt = 0;
    uint32_ofp member_idx;
    ofp_nh_offset_t group_nh;
    uint16_ofp in_port;
    bool group_valid = false;
    
    ADPT_FLOW_ERROR_RETURN(adpt_flow_check_output_action(&p_rule->flow_actions, &p_rule->nh_info));

    /* If there is no output action exist, we need add a drop entry*/
    if (0 == p_rule->nh_info.output_count )
    {
        p_rule->nh_info.main_nh.nhid = ADPT_NH_RSV_NHID_FOR_DROP;
        p_rule->nh_info.main_nh.offset = 0;
        p_rule->nh_info.main_nh.nh_type = OPF_NH_TYPE_MAX;
        p_rule->nh_info.use_mcast = FALSE;
        
        return OFP_ERR_SUCCESS;
    }

    memset(&group_nh, 0, sizeof(group_nh));
    ADPT_FLOW_ERROR_RETURN(adpt_nexthop_alloc_mcast_group(&group_nh));
    p_rule->nh_info.main_nh.nhid   = group_nh.nhid;
    p_rule->nh_info.main_nh.offset = group_nh.offset;
    p_rule->nh_info.use_mcast      = TRUE;

    in_port = OFP_FLOW_INPORT_BASED(p_rule) == FLOW_TYPE_PORT_BASED_PER_PORT ? p_rule->match.flow.in_port : 0;
    ADPT_FLOW_ERROR_RETURN(adpt_flow_map_output_action_to_mcast_members(
        in_port, &p_rule->flow_actions, p_rule->flow_id,
        p_rule->nh_info.member_nh,
        &member_cnt));

    for (member_idx = 0; member_idx < member_cnt; member_idx ++)
    {
        ADPT_FLOW_ERROR_RETURN(hal_nexthop_add_mcast_member(
            group_nh.nhid, 
            p_rule->nh_info.member_nh[member_idx].nhid,
            p_rule->nh_info.member_nh[member_idx].port_check_discard));
    }

    return OFP_ERR_SUCCESS;
}

/** 
 * Map a flow action to Qos action
 * @param[in]  p_rule            Pointer to struct rule_ctc
 * @param[out] p_qos_action      Pointer to Qos action
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_flow_action(struct rule_ctc *p_rule, ctc_aclqos_action_t *p_qos_action)
{
    int32_ofp ret = 0;
    const struct list *flow_actions = &p_rule->flow_actions;
    uint16_ofp stats_ptr = SPECIAL_STATS_PTR;

    if (p_rule->stats_ptr == SPECIAL_STATS_PTR)
    {
        ADPT_FLOW_ERROR_RETURN(hal_stats_create_stats_ptr(&stats_ptr));
        SET_FLAG(p_qos_action->flag, CTC_ACLQOS_ACTION_STATS_FLAG);
        p_rule->stats_ptr       = stats_ptr;
        p_qos_action->stats_ptr = stats_ptr;
    }

    /* DROP  */
    if (list_is_empty(flow_actions))
    {
        p_rule->nh_info.main_nh.nhid   = ADPT_NH_RSV_NHID_FOR_DROP;
        p_rule->nh_info.main_nh.offset = 0;

        p_qos_action->fwd.fwd_nh_id = p_rule->nh_info.main_nh.nhid;
        SET_FLAG(p_qos_action->flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);
    }
    else
    {
        ret = adpt_flow_add_output_action(p_rule);
        if (ret)
        {
            adpt_nexthop_release_nh_info_res(&(p_rule->nh_info));
            return ret;
        }
        if (p_rule->nh_info.main_nh.nhid)
        {
            p_qos_action->fwd.fwd_nh_id = p_rule->nh_info.main_nh.nhid;
        }
        else if (p_rule->group_info.group_nhid)
        {
            p_qos_action->fwd.fwd_nh_id = p_rule->group_info.group_nhid;
        }
        SET_FLAG(p_qos_action->flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);
    }

    return OFP_ERR_SUCCESS;
}

/** 
 * Remove the resource allocated during mapping flow action
 * @param[in]  p_rule            Pointer to struct rule_ctc
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_map_remove_flow_action(struct rule_ctc* p_rule)
{
    if (p_rule->stats_ptr != SPECIAL_STATS_PTR)
    {
        hal_stats_delete_stats_ptr(p_rule->stats_ptr);
        hal_stats_clear_stats_ptr(p_rule->stats_ptr);
        p_rule->stats_ptr = SPECIAL_STATS_PTR;
    }

    adpt_nexthop_release_nh_info_res(&(p_rule->nh_info));

    return 0;
}

/** 
 * Add rule to sdk
 * @param[in]  p_rule            Pointer to struct rule_ctc
 * @param[in]  label_id          Qos label id
 * @param[in]  label_type        Qos label type
 * @param[in]  pre_entry_id      previous entry id
 * @param[in]  p_entry           Pointer to qos entry
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_add_flow_to_sdk(struct rule_ctc* p_rule, uint32_ofp label_id, ctc_aclqos_label_type_t label_type, uint32_ofp pre_entry_id, ctc_aclqos_entry_t* p_entry)
{
    adpt_flow_print_entry(label_type, label_id, p_entry);
    
    if (CTC_QOS_LABEL == label_type)
    {
        ADPT_FLOW_ERROR_RETURN(hal_flow_add_qos_entry(label_id, pre_entry_id, p_entry));
    }
    else if (CTC_SERVICE_LABEL == label_type)
    {
        ctc_aclqos_entry_oper_t entry_op;

        entry_op.label_id     = label_id;
        entry_op.label_type   = label_type;
        entry_op.pre_entry_id = pre_entry_id;
        memcpy(&entry_op.entry, p_entry, sizeof(ctc_aclqos_entry_t));
        
        ADPT_FLOW_ERROR_RETURN(hal_flow_add_service_entry(&entry_op));
    }
    else
    {
        return OFP_ERR_INVALID_PARAM;
    }
    
    return OFP_ERR_SUCCESS;
}

/** 
 * Remove rule from sdk
 * @param[in]  p_rule            Pointer to struct rule_ctc
 * @param[in]  label_id          Qos label id
 * @param[in]  label_type        Qos label type
 * @param[in]  entry_id          Qos entry id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_del_flow_from_sdk(const struct rule_ctc* p_rule, uint32_ofp label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t key_type, uint32_ofp entry_id)
{
    if (CTC_QOS_LABEL == label_type)
    {
        hal_flow_remove_qos_entry(label_id, key_type, entry_id);
    }
    else if (CTC_SERVICE_LABEL == label_type)
    {
        ctc_aclqos_entry_oper_t entry_op;

        entry_op.label_id = label_id;
        entry_op.label_type = CTC_SERVICE_LABEL;
        entry_op.entry.entry_id = entry_id;
        entry_op.entry.key.type = key_type;
        
        hal_flow_remove_service_entry(&entry_op);
    }
    else
    {
        return OFP_ERR_INVALID_PARAM;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Check ipv4 related flow fields
 * @param p_rule                 Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
_adpt_flow_check_ipv4_fields(const struct rule_ctc* p_rule)
{
    const struct flow *flow;
    const struct flow *masks;

    flow = &p_rule->match.flow;
    masks = &p_rule->match.wc.masks;

    if (ETH_TYPE_IP == ntohs(flow->dl_type))
    {
        if (0 != (masks->nw_ttl) || 0 != (masks->nw_tos & IP_ECN_MASK) || 0 != (masks->nw_frag))
        {
            ADPT_LOG_ERROR("Unsupported ipv4 flow fields: nw_ttl, nw_ecn and ip_frag");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
        }
    }

    if (ETH_TYPE_ARP == ntohs(flow->dl_type))
    {
        if (false == eth_addr_is_zero(masks->arp_sha) || false == eth_addr_is_zero(masks->arp_tha))
        {
            ADPT_LOG_ERROR("Unsupported arp flow fields: arp_sha and arp_tha");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Check ipv6 related flow fields
 * @param p_rule                 Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
_adpt_flow_check_ipv6_fields(const struct rule_ctc* p_rule)
{
    const struct flow *flow;
    const struct flow *masks;

    flow = &p_rule->match.flow;
    masks = &p_rule->match.wc.masks;

    if (ETH_TYPE_IPV6 == ntohs(flow->dl_type))
    {
        if (false == ipv6_mask_is_any(&masks->ipv6_src) ||
                false == ipv6_mask_is_any(&masks->ipv6_dst) ||
                0 != (masks->nw_tos & IP_ECN_MASK) ||
                0 != (masks->nw_tos & IP_DSCP_MASK) ||
                0 != (masks->nw_ttl) ||
                0 != (masks->nw_frag) ||
                htonl(0) != (masks->ipv6_label & ~htonl(IPV6_LABEL_MASK)))
        {
            ADPT_LOG_ERROR("Unsupported ipv6 flow fields: ipv6_src, ipv6_dst, ipv6_label, ip_frag, nw_tos, nw_ecn and nw_ttl");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
        }
    }

    if (ETH_TYPE_IPV6 == ntohs(flow->dl_type) && IPPROTO_ICMPV6 == flow->nw_proto)
    {
        if (false == eth_addr_is_zero(masks->arp_sha) || false == eth_addr_is_zero(masks->arp_tha) ||
                false == ipv6_mask_is_any(&masks->nd_target))
        {
            ADPT_LOG_ERROR("Unsupported ipv6 flow fields: nd_sll, nd_tll and nd_target");
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
        }
    }

    if (ETH_TYPE_IPV6 == ntohs(flow->dl_type) && 0 != masks->nw_proto)
    {
        ADPT_LOG_ERROR("Unsupported ipv6 flow field: nw_proto");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Check mpls flow fields
 * @param p_rule                 Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
_adpt_flow_check_mpls_fields(const struct rule_ctc* p_rule)
{
    const struct flow *masks;

    masks = &p_rule->match.wc.masks;

    if (0 != (ntohl(masks->mpls_lse) & MPLS_BOS_MASK) >> MPLS_BOS_SHIFT)
    {
        ADPT_LOG_ERROR("Unsupported flow field: mpls_bos");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
    }

    if (0 != (ntohl(masks->mpls_lse) & MPLS_TC_MASK) >> MPLS_TC_SHIFT)
    {
        ADPT_LOG_ERROR("Unsupported flow field: mpls_tc");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
    }

    if (0 != (ntohl(masks->mpls_lse) & MPLS_TTL_MASK) >> MPLS_TTL_SHIFT)
    {
        ADPT_LOG_ERROR("Unsupported flow field: mpls_ttl");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Check misc flow fields
 * @param p_rule                 Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
_adpt_flow_check_misc_fields(const struct rule_ctc* p_rule)
{
    int i = 0;
    const struct flow *masks;

    masks = &p_rule->match.wc.masks;

    if (0 != ntohll(masks->metadata))
    {
        ADPT_LOG_ERROR("Unsupported flow field: metadata");
        return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
    }

    for (i = 0; i < FLOW_N_REGS; i++)
    {
        if (0 != masks->regs[i])
        {
            ADPT_LOG_ERROR("Unsupported flow field: reg%d", i);
            return OFP_ERR_UNSUPPORTED_FLOW_FIELD;
        }
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Check flow fields
 * @param p_rule                 Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_check_flow_fields(const struct rule_ctc* p_rule)
{
    /* 1. check tunnel id */
    ADPT_FLOW_ERROR_RETURN(adpt_tunnel_check_tunnel_id(p_rule));

    /* 2. check ipv4 fields */
    ADPT_FLOW_ERROR_RETURN(_adpt_flow_check_ipv4_fields(p_rule));

    /* 3. check ipv6 fields */
    ADPT_FLOW_ERROR_RETURN(_adpt_flow_check_ipv6_fields(p_rule));

    /* 4. check mpls fields */
    ADPT_FLOW_ERROR_RETURN(_adpt_flow_check_mpls_fields(p_rule));

    /* 5. check misc fields */
    ADPT_FLOW_ERROR_RETURN(_adpt_flow_check_misc_fields(p_rule));

    return OFP_ERR_SUCCESS;
}

/** 
 * Check resource for a specified rule
 * @param[in]  p_rule            Pointer to ctc-provider rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_check_flow_entry(const struct rule_ctc* p_rule)
{
    /* 1. check flow field */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_check_flow_fields(p_rule));

    /* 2. check flow number */
    if (adpt_flowdb_get_flow_entry_cur_num() >= adpt_flowdb_get_flow_entry_max_num())
    {
        return OFP_ERR_ALL_TABLES_FULL;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Allocate flow id
 * @param[out] p_flow_id           Pointer to flow id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_alloc_flow_id(uint32_ofp* p_flow_id)
{
    ADPT_FLOW_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_FLOW_ID, 1, p_flow_id));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Release flow id
 * @param[out] p_flow_id           Flow id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_release_flow_id(uint32_ofp flow_id)
{
    ADPT_FLOW_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_FLOW_ID, 1, flow_id));
    
    return OFP_ERR_SUCCESS;
}

/** 
 * Add a flow timer for a specified rule
 * @param[in]  p_rule            Pointer to struct rule_ctc
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_add_flow_timer(struct rule_ctc* p_rule)
{
    uint32_ofp flow_id = 0;
    adpt_flow_info_t * flow_info_p = NULL;

    /* Allocate flow id for identify a flow, even if no idle timeout is configured */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_alloc_flow_id(&flow_id));
    p_rule->flow_id = flow_id;

    /* Allocate flow info for rule, and set the last used timestamp */
    flow_info_p = kal_malloc(sizeof(adpt_flow_info_t));
    kal_memset(flow_info_p, 0x0, sizeof(adpt_flow_info_t));

    flow_info_p->p_rule = p_rule;

    if (0 == p_rule->up.idle_timeout)
    {
        flow_info_p->is_idle_timer = false;
        adpt_flowdb_add_flow_info(flow_id, flow_info_p);
        return OFP_ERR_SUCCESS;
    }

    flow_info_p->is_idle_timer = true;
    flow_info_p->idle_timeout  = p_rule->up.idle_timeout;
    flow_info_p->last_matched  = p_rule->up.created;
    
    adpt_flowdb_add_flow_info(flow_id, flow_info_p);
    
    return OFP_ERR_SUCCESS;
}

/** 
 * Delete a flow timer for a specified rule
 * @param[in]  p_rule            Pointer to OVS rule
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_del_flow_timer(struct rule_ctc* p_rule)
{
    adpt_flow_info_t * flow_info_p = NULL;

    flow_info_p = adpt_flowdb_get_flow_info(p_rule->flow_id);
    if (flow_info_p)
    {
        flow_info_p->need_delete = true;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Allocate QoS entry id
 * @param flow_type             OpenFlow flow type(FLOW_TYPE_***)
 * @param entry_id              QoS entry id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_alloc_qos_entry_id(ofp_flow_type_t flow_type, uint32_ofp* p_entry_id)
{
    if (flow_type == FLOW_TYPE_MAC || flow_type == FLOW_TYPE_ANY || flow_type == FLOW_TYPE_OTHER)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_QOS_MAC_ENTRY_ID, 1, p_entry_id));
    }
    else if (flow_type == FLOW_TYPE_IPV4)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_QOS_IPV4_ENTRY_ID, 1, p_entry_id));
    }
    else if (flow_type == FLOW_TYPE_MPLS)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_alloc_offset(OPF_OFP_QOS_MPLS_ENTRY_ID, 1, p_entry_id));
    }
    else
    {
        return OFP_ERR_FAIL;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Release QoS entry id
 * @param flow_type             OpenFlow flow type(FLOW_TYPE_***)
 * @param entry_id              QoS entry id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_release_qos_entry_id(ofp_flow_type_t flow_type, uint32_ofp entry_id)
{
    if (flow_type == FLOW_TYPE_MAC || flow_type == FLOW_TYPE_ANY || flow_type == FLOW_TYPE_OTHER)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_QOS_MAC_ENTRY_ID, 1, entry_id));
    }
    else if (flow_type == FLOW_TYPE_IPV4)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_QOS_IPV4_ENTRY_ID, 1, entry_id));
    }
    else if (flow_type == FLOW_TYPE_MPLS)
    {
        ADPT_FLOW_ERROR_RETURN(adpt_opf_free_offset(OPF_OFP_QOS_MPLS_ENTRY_ID, 1, entry_id));
    }
    else
    {
        return OFP_ERR_FAIL;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Allocate QoS entry id
 * @param[in]  label_id              OpenFlow flow type(FLOW_TYPE_***)
 * @param[in]  flow_type             OpenFlow flow type(FLOW_TYPE_***)
 * @param[out] p_entry_id            Pointer to QoS entry id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_alloc_entry_id(uint32_ofp label_id, ofp_flow_type_t flow_type, uint32_ofp *p_entry_id)
{
    ADPT_FLOW_ERROR_RETURN(adpt_flow_alloc_qos_entry_id(flow_type, p_entry_id));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Release QoS entry id
 * @param[in]  label_id              OpenFlow flow type(FLOW_TYPE_***)
 * @param[in]  flow_type             OpenFlow flow type(FLOW_TYPE_***)
 * @param[out] entry_id              QoS entry id
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_release_entry_id(uint32_ofp label_id, ofp_flow_type_t flow_type, uint32_ofp entry_id)
{
    ADPT_FLOW_ERROR_RETURN(adpt_flow_release_qos_entry_id(flow_type, entry_id));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Alloc QoS label id
 * @param[in]  p_rule               Pointer to struct rule_ctc
 * @param[out] p_label_type         Pointer to label type
 * @param[out] p_label_id           Pointer to QoS label id
 * @return OFP_ERR_XXX
 */
static uint32_ofp
adpt_flow_alloc_label_id(struct rule_ctc *p_rule, ctc_aclqos_label_type_t* p_label_type, uint32_ofp* p_label_id)
{    
    if (FLOW_TYPE_PORT_BASED_GLOBAL == OFP_FLOW_INPORT_BASED(p_rule))
    {
        *p_label_type = CTC_QOS_LABEL;
        *p_label_id   = CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL;
    }
    else if (adpt_port_is_physical_port(p_rule->match.flow.in_port))
    {
        uint16_ofp gport;
        
        ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(
            p_rule->match.flow.in_port, &gport));
        *p_label_type = CTC_QOS_LABEL;
        *p_label_id   = BASE_PORT_LABEL_ID + gport;
    }
    else if (adpt_port_is_tunnel_port(p_rule->match.flow.in_port))
    {
        *p_label_type = CTC_SERVICE_LABEL;
        ADPT_FLOW_ERROR_RETURN(adpt_tunnel_bind_service_id(p_rule->match.flow.in_port,
            ntohll(p_rule->match.flow.tunnel.tun_id), p_label_id));
    }
    else
    {
        ADPT_LOG_ERROR("Invalid in_port port number is found\n");
        return OFP_ERR_INVALID_IN_PORT_NUMBER;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Lookup QoS label id
 * @param[in]  p_rule               Pointer to struct rule_ctc
 * @param[out] p_label_type         Pointer to label type
 * @param[out] p_label_id           Pointer to QoS label id
 * @return OFP_ERR_XXX
 */
static uint32_ofp
adpt_flow_lookup_label_id(struct rule_ctc* p_rule, ctc_aclqos_label_type_t* p_label_type, uint32_ofp* p_label_id)
{
    if (FLOW_TYPE_PORT_BASED_GLOBAL == OFP_FLOW_INPORT_BASED(p_rule))
    {
        *p_label_type = CTC_QOS_LABEL;
        *p_label_id   = CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL;
    }
    else if (adpt_port_is_physical_port(p_rule->match.flow.in_port))
    {
        uint16_ofp gport;
        
        ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(
            p_rule->match.flow.in_port, &gport));
        *p_label_type = CTC_QOS_LABEL;
        *p_label_id = BASE_PORT_LABEL_ID + gport;
    }
    else if (adpt_port_is_tunnel_port(p_rule->match.flow.in_port))
    {
        /** TODO to be implemented*/
    }
    else
    {
        return OFP_ERR_INVALID_IN_PORT_NUMBER;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Release QoS label id
 * @param[in]  p_rule               Pointer to struct rule_ctc
 * @param[out] label_type           label type
 * @param[out] label_id             QoS label id
 * @return OFP_ERR_XXX
 */
static uint32_ofp
adpt_flow_release_label_id(struct rule_ctc* p_rule, ctc_aclqos_label_type_t label_type, uint32_ofp label_id)
{
    if (FLOW_TYPE_PORT_BASED_GLOBAL == OFP_FLOW_INPORT_BASED(p_rule))
    {
        /* DO NOT release global label*/
    }
    else if (adpt_port_is_physical_port(p_rule->match.flow.in_port))
    {
        /* DO NOT release port label*/
    }
    else if (adpt_port_is_tunnel_port(p_rule->match.flow.in_port))
    {
        adpt_tunnel_unbind_service_id(p_rule->match.flow.in_port,
            p_rule->match.flow.tunnel.tun_id, label_id);
    }
    else
    {
        return OFP_ERR_INVALID_PARAM;       
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Reset tunnel flow stats
 * @param[in] service_id           Service id
 * @param[in] pv_arg               Not used
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_reset_tunnel_flow_stats(uint32_ofp service_id, void* pv_arg)
{
    uint32_ofp entry_id = 0;
    
    ADPT_FLOW_ERROR_RETURN(hal_flow_clear_flow_stats(service_id, 
        CTC_SERVICE_LABEL, CTC_ACLQOS_MAC_KEY, entry_id));
    
    ADPT_FLOW_ERROR_RETURN(hal_flow_clear_flow_stats(service_id, 
        CTC_SERVICE_LABEL, CTC_ACLQOS_IPV4_KEY, entry_id));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Reset flow stats
 * @param[in] ofport                ofport
 * @param[in] pv_arg                Not used
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_reset_flow_stats(uint16_ofp ofport, void* pv_arg)
{
    uint16_ofp gport;
    uint32_ofp label_id;
    uint32_ofp entry_id = 0;
    
    ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    label_id = BASE_PORT_LABEL_ID + gport;
    
    ADPT_FLOW_ERROR_RETURN(hal_flow_clear_flow_stats(label_id, CTC_QOS_LABEL, CTC_ACLQOS_MAC_KEY,  entry_id));
    ADPT_FLOW_ERROR_RETURN(hal_flow_clear_flow_stats(label_id, CTC_QOS_LABEL, CTC_ACLQOS_IPV4_KEY, entry_id));
    
    return OFP_ERR_SUCCESS;
}

/**
 * Reset all flow stats
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_clear_all_flow_stats(void)
{
    uint32_ofp label_id;
    uint32_ofp entry_id = 0;
    
    /* clear global stats */
    label_id = CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL;
    hal_flow_clear_flow_stats(label_id, CTC_QOS_LABEL, CTC_ACLQOS_MAC_KEY,  entry_id);
    hal_flow_clear_flow_stats(label_id, CTC_QOS_LABEL, CTC_ACLQOS_IPV4_KEY, entry_id);
    
    /* loop port to reset flow stats*/
    adpt_port_loop_port(OFP_INTERFACE_TYPE_PHYSICAL, adpt_flow_reset_flow_stats, NULL);
    
    /* clear service flow stats per service id */
    adpt_tunnel_loop_tunnel_port_service_id(adpt_flow_reset_tunnel_flow_stats, NULL);
    
    g_p_adpt_flow_master->removed_flow_stats_pkt   = 0;
    g_p_adpt_flow_master->removed_flow_stats_bytes = 0;

    return OFP_ERR_SUCCESS;
}

/**
 * Handle idle timeout timer
 * @param[in] p_arg             Not used
 * @return OFP_ERR_XXX
 */
static int32_ofp
adpt_flow_idle_timeout_timer(void* p_arg)
{
    uint32_ofp flow_id = 0;
    struct ihash *p_flow_info_ihmap;
    adpt_flow_info_t * p_flow_info;
    struct ihash_node *node, *next;
    struct ofp_stats_s new_stats;
    
    sal_mutex_lock(pg_mutex);
    
    p_flow_info_ihmap = adpt_flowdb_get_flow_info_ihmap();
    
    IHASH_FOR_EACH_SAFE(node, next, p_flow_info_ihmap)
    {
        flow_id = node->key;
        p_flow_info = (adpt_flow_info_t *) (node->data);
        
        /* If a flow need to delete in function del_flow, need to free the database */
        if (true == p_flow_info->need_delete)
        {
            p_flow_info->is_idle_timer = false;
            adpt_flow_release_flow_id(flow_id);
            adpt_flowdb_del_flow_info(flow_id);
            continue;
        }

        /* if a flow is not idle_timer, do not read stats */
        if (false == p_flow_info->is_idle_timer)
        {
            continue;
        }

        new_stats.packet_count = 0;
        new_stats.byte_count   = 0;

        ofp_get_flow_stats(p_flow_info->p_rule, &new_stats);

        if (new_stats.packet_count != p_flow_info->packet_count)
        {
            p_flow_info->last_matched = time_msec();
            p_flow_info->packet_count = new_stats.packet_count;
        }
    }
    
    sal_mutex_unlock(pg_mutex);
    
    return OFP_ERR_SUCCESS;
}

/**
 * Retrieve flow stats and save to DB
 * @param[in]  p_rule               Pointer to struct rule_ctc
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_retrieve_flow_stats(struct rule_ctc* p_rule)
{
    ofp_stats_t glb_stats;

    memset(&glb_stats, 0, sizeof(glb_stats));
    
    ADPT_FLOW_ERROR_RETURN(ofp_get_flow_stats(p_rule, &glb_stats));
    g_p_adpt_flow_master->removed_flow_stats_pkt   += glb_stats.packet_count;
    g_p_adpt_flow_master->removed_flow_stats_bytes += glb_stats.byte_count;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get statistics of removed flows
 * @param[out] p_stats             Pointer to stats
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_removed_flow_stats(ofp_stats_t* p_stats)
{
    p_stats->packet_count = g_p_adpt_flow_master->removed_flow_stats_pkt;
    p_stats->byte_count   = g_p_adpt_flow_master->removed_flow_stats_bytes;

    return OFP_ERR_SUCCESS;
}

/**
 * Get the last time when a packet match the flow.
 * @param[in] flow_id           Flow id
 * @param[in] p_last_matched    Last matched time
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_last_matched(struct rule_ctc* p_rule, int64_ofp* p_last_matched)
{
    adpt_flow_info_t* flow_info_p = NULL;
    
    flow_info_p = adpt_flowdb_get_flow_info(p_rule->flow_id);
    if (flow_info_p)
    {
        *p_last_matched = flow_info_p->last_matched;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * @brief Add flow entry
 */
int32_ofp
adpt_flow_add_flow(struct rule_ctc* p_rule)
{
    ctc_aclqos_entry_t entry;
    ctc_aclqos_entry_t extra_entry;
    ofp_flow_type_t flow_type;
    uint32_ofp label_id = 0;
    ctc_aclqos_label_type_t label_type;
    uint32_ofp prev_entry_id = 0;
    int ret = 0;

    p_rule->flow_id         = 0;
    p_rule->entry_id       = 0;
    p_rule->extra_entry_id = 0;
    p_rule->stats_ptr      = SPECIAL_STATS_PTR;
    p_rule->mpls_label     = 0;
    p_rule->is_mpls        = FALSE;
    p_rule->is_mpls_pop    = FALSE;
    p_rule->action_type    = OPF_ACTION_TYPE_NORMAL;
    memset(&(p_rule->nh_info), 0x0, sizeof(ofp_nexthop_info_t));
    p_rule->nh_info.use_mcast = FALSE;
    memset(&(p_rule->meter_info), 0, sizeof(ofp_meter_info_t));
    p_rule->meter_info.meter_id = ADPT_METER_INV_ID;
    p_rule->meter_info.is_meter_bound = FALSE;
    p_rule->queue_id = OFP_MAX_QUEUE_VALUE;
    p_rule->group_info.group_id = OFP_GROUP_INVALID_GROUP_ID;
    p_rule->group_info.is_group_bound = FALSE;
    p_rule->group_info.group_nhid     = 0;
    
    memset(&entry, 0, sizeof(ctc_aclqos_entry_t));

    OFP_LOG_DEBUG_FUNC();

    /* 1. check flow_entry */
    ret = adpt_flow_check_flow_entry(p_rule);
    if (ret)
    {
        goto Err0;
    }

    /* 3. get label id & label type */
    ret = adpt_flow_alloc_label_id(p_rule, &label_type, &label_id);
    if (ret)
    {
        goto Err0;
    }

    /* 4. get entry id */
    flow_type = OFP_MAP_FLOW_TYPE(ntohs(p_rule->match.flow.dl_type));
    ret = adpt_flow_alloc_entry_id(label_id, flow_type, &entry.entry_id);
    if (ret)
    {
        goto Err1;
    }

    /* 5. map flow key */
    ret = adpt_flow_map_flow_key(p_rule, &entry.key);
    if (ret)
    {
        goto Err2;
    }

    /* 6. add flow timer to handle idle_timeout */
    adpt_flow_add_flow_timer(p_rule);

    /* 7. map flow action */
    ret = adpt_flow_map_flow_action(p_rule, &entry.action);
    if (ret)
    {
        goto Err3;
    }

    /* 8. get prev entry id */
    adpt_flowdb_add_flow_priority_entry_id(flow_type, p_rule->up.cr.priority, entry.entry_id);
    adpt_flowdb_get_prev_entry_id(flow_type, p_rule->up.cr.priority, entry.entry_id, &prev_entry_id);
    OFP_DEBUG_PRINT("priority: %u, flow_type: %d, label_id: %u, prev_entry_id: %u entry_id: %u \n",
            p_rule->up.cr.priority, flow_type, label_id, prev_entry_id, entry.entry_id);

    /* 9. add flow entry to sdk*/
    ret = adpt_flow_add_flow_to_sdk(p_rule, label_id, label_type, prev_entry_id, &entry);
    if (ret)
    {
        goto Err4;
    }
    
    /* 10. Copy to ipv4 key to match ipv4 packets if necessary */
    if (FLOW_TYPE_ANY == flow_type)
    {
        /* 10.1 copy flow action, flow key */
        memset(&extra_entry, 0, sizeof(ctc_aclqos_entry_t));
        adpt_flow_map_mac_ipv4_entry(&entry, &extra_entry);

        /* 10.2 alloc flow entry id, after entry id*/
        ret = adpt_flow_alloc_entry_id(label_id, FLOW_TYPE_IPV4, &extra_entry.entry_id);
        if (ret)
        {
            goto Err5;
        }

        /* 10.3 get prev entry id */
        adpt_flowdb_add_flow_priority_entry_id(FLOW_TYPE_IPV4, p_rule->up.cr.priority, extra_entry.entry_id);
        adpt_flowdb_get_prev_entry_id(FLOW_TYPE_IPV4, p_rule->up.cr.priority, extra_entry.entry_id, &prev_entry_id);
        OFP_DEBUG_PRINT("priority: %u, flow_type: %d, label_id: %u, prev_entry_id: %u entry_id: %u \n",
                p_rule->up.cr.priority, FLOW_TYPE_IPV4, label_id, prev_entry_id, entry.entry_id);

        /* 10.4 add flow entry to sdk */
        ret = adpt_flow_add_flow_to_sdk(p_rule, label_id, label_type, prev_entry_id, &extra_entry);
        if (ret)
        {
            OFP_DEBUG_PRINT("Rollback the previous mac entry label id: %u, entry id: %u\n", label_id, entry.entry_id);
            
            goto Err6;
        }

        /* 10.5. add entry id to db*/
        p_rule->extra_entry_id = extra_entry.entry_id;
    }

    /* 11. add entry id to db*/
    p_rule->entry_id = entry.entry_id;

    /* 12. increase current flow number */
    adpt_flowdb_incr_flow_entry_num();

    adpt_flow_op_nexthop_res(&p_rule->nh_info, ADPT_RES_OP_TYPE_ADD);

    return OFP_ERR_SUCCESS;
    
Err6:
    adpt_flowdb_del_flow_priority_entry_id(FLOW_TYPE_IPV4, p_rule->up.cr.priority, extra_entry.entry_id);
    adpt_flow_release_entry_id(label_id, FLOW_TYPE_IPV4, extra_entry.entry_id);
Err5:
    adpt_flow_del_flow_from_sdk(p_rule, label_id, label_type, entry.key.type, entry.entry_id);
Err4:
    adpt_flowdb_del_flow_priority_entry_id(flow_type, p_rule->up.cr.priority, entry.entry_id);
Err3:
    adpt_flow_map_remove_flow_action(p_rule);
    adpt_flow_map_remove_flow_key(p_rule);
    adpt_flow_del_flow_timer(p_rule);
Err2:
    adpt_flow_release_entry_id(label_id, flow_type, entry.entry_id);
Err1:
    adpt_flow_release_label_id(p_rule, label_type, label_id);
Err0:
    ADPT_ERROR_RETURN(ret);
    
    return OFP_ERR_FAIL;
}

/**
 * @brief Modify flow entry action
 */
int32_ofp
adpt_flow_modify_flow_action(struct rule_ctc *p_rule)
{
    ctc_aclqos_action_t action;
    ofp_flow_type_t flow_type;
    ctc_aclqos_key_type_t entry_type;
    uint32_ofp label_id = 0;
    ctc_aclqos_label_type_t label_type;
    int ret = 0;
    ofp_nexthop_info_t old_nh_info;
    ofp_meter_info_t old_meter_info;
    ofp_group_info_t old_group_info;

    memcpy(&old_nh_info, &(p_rule->nh_info), sizeof(ofp_nexthop_info_t));
    memcpy(&old_meter_info, &(p_rule->meter_info), sizeof(ofp_meter_info_t));
    memcpy(&old_group_info, &(p_rule->group_info), sizeof(ofp_group_info_t));
    memset(&(p_rule->nh_info), 0x0, sizeof(ofp_nexthop_info_t));
    p_rule->nh_info.use_mcast = FALSE;
    p_rule->group_info.group_id       = OFP_GROUP_INVALID_GROUP_ID;
    p_rule->group_info.is_group_bound = FALSE;
    p_rule->group_info.group_nhid     = 0;
    memset(&action, 0, sizeof(ctc_aclqos_action_t));
    p_rule->meter_info.meter_id       = ADPT_METER_INV_ID;
    p_rule->meter_info.is_meter_bound = FALSE;
    p_rule->queue_id= OFP_MAX_QUEUE_VALUE;

    /* 1. check flow_entry */
    ret = adpt_flow_check_flow_entry(p_rule);
    if (ret)
    {
        goto Err0;
    }

    /* 2. lookup label id & label type */
    ret = adpt_flow_lookup_label_id(p_rule, &label_type, &label_id);
    if (ret)
    {
        goto Err0;
    }

    /* 3. map p_rule action */
    ret = adpt_flow_map_flow_action(p_rule, &action);
    if (ret)
    {
        goto Err0;
    }

    flow_type = OFP_MAP_FLOW_TYPE(ntohs(p_rule->match.flow.dl_type));
    entry_type = OFP_MAP_KEY_TYPE(flow_type);
    
    /* 4. set action to sdk */
    ret = hal_flow_set_flow_action(label_id, label_type, entry_type, p_rule->entry_id, &action);
    if (ret)
    {
        goto Err1;
    }

    /* 5. Copy action to ipv4 key if necessary */
    if (FLOW_TYPE_ANY == flow_type)
    {
        hal_flow_set_flow_action(label_id, label_type, CTC_ACLQOS_IPV4_KEY, p_rule->extra_entry_id, &action);
    }

    adpt_nexthop_release_nh_info_res(&old_nh_info);

    adpt_flow_op_nexthop_res(&p_rule->nh_info, ADPT_RES_OP_TYPE_ADD);
    adpt_flow_op_nexthop_res(&old_nh_info, ADPT_RES_OP_TYPE_DEL);

    return OFP_ERR_SUCCESS;

Err1:
    adpt_flow_map_remove_flow_action(p_rule);
Err0:    
    memcpy(&p_rule->nh_info, &old_nh_info, sizeof(ofp_nexthop_info_t));
    memcpy(&p_rule->meter_info, &old_meter_info, sizeof(ofp_meter_info_t));
    memcpy(&p_rule->group_info, &old_group_info, sizeof(ofp_group_info_t));

    ADPT_ERROR_RETURN(ret);
    
    return OFP_ERR_FAIL;
}

/**
 * @brief Delete flow entry
 */
int32_ofp
adpt_flow_del_flow(struct rule_ctc *p_rule)
{
    uint32_ofp label_id;
    ctc_aclqos_key_type_t key_type;
    ctc_aclqos_label_type_t label_type;
    ofp_flow_type_t flow_type;

    /* 1. get label id & label type */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_lookup_label_id(p_rule, &label_type, &label_id));

    /* 2. get entry type */
    flow_type = OFP_MAP_FLOW_TYPE(ntohs(p_rule->match.flow.dl_type));
    key_type = OFP_MAP_KEY_TYPE(flow_type);

    /* 3. get tables statistics*/
    ADPT_FLOW_ERROR_RETURN(adpt_flow_retrieve_flow_stats(p_rule));

    /* 4. remove entry from sdk */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_del_flow_from_sdk(p_rule, label_id, label_type, key_type, p_rule->entry_id));

    /* 5. remove entry from db */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_release_entry_id(label_id, flow_type, p_rule->entry_id));
    adpt_flowdb_del_flow_priority_entry_id(flow_type, p_rule->up.cr.priority, p_rule->entry_id);

    OFP_DEBUG_PRINT("flow is removed, label_id=%u, label_type=%d, key_type=%u, flow_type=%d, entry_id=%u\n",
        label_id, label_type, key_type, flow_type, p_rule->entry_id);

    /* 6. remove ipv4 entry from sdk & db */
    if (FLOW_TYPE_ANY == OFP_MAP_FLOW_TYPE(ntohs(p_rule->match.flow.dl_type)))
    {
        key_type = CTC_ACLQOS_IPV4_KEY;

        OFP_DEBUG_PRINT("extra(IP) flow is removed, label_id=%u, label_type=%d, key_type=%u, entry_id=%u\n",
                        label_id, label_type, key_type, p_rule->extra_entry_id);

        ADPT_FLOW_ERROR_RETURN(adpt_flow_del_flow_from_sdk(p_rule, label_id, label_type, key_type, p_rule->extra_entry_id));
        ADPT_FLOW_ERROR_RETURN(adpt_flow_release_entry_id(label_id, FLOW_TYPE_IPV4, p_rule->extra_entry_id));
        ADPT_FLOW_ERROR_RETURN(adpt_flowdb_del_flow_priority_entry_id(FLOW_TYPE_IPV4, p_rule->up.cr.priority, p_rule->extra_entry_id));
    }
    
    /* 7. remove some resource when mapping key & action */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_map_remove_flow_key(p_rule));
    ADPT_FLOW_ERROR_RETURN(adpt_flow_map_remove_flow_action(p_rule));
    ADPT_FLOW_ERROR_RETURN(adpt_flow_release_label_id(p_rule, label_type, label_id));

    /* 8. decrease flow entry counter */
    ADPT_FLOW_ERROR_RETURN(adpt_flowdb_decr_flow_entry_num());

    ADPT_FLOW_ERROR_RETURN(adpt_flow_op_nexthop_res(&p_rule->nh_info, ADPT_RES_OP_TYPE_DEL));

    /* 9. free flow in flow db */
    ADPT_FLOW_ERROR_RETURN(adpt_flow_del_flow_timer(p_rule));

    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_flow_add_idle_timeout_timer(void)
{
    ADPT_MODULE_INIT_CHECK(g_p_adpt_flow_master);
    
    ADPT_FLOW_ERROR_RETURN(afx_timer_create(&g_p_adpt_flow_master->idle_timeout_timer, 
        (afx_timer_cb_t) adpt_flow_idle_timeout_timer, NULL));
    ADPT_FLOW_ERROR_RETURN(afx_timer_start(g_p_adpt_flow_master->idle_timeout_timer, 
        OFP_IDLE_TIMEOUT_TIMER));

    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_flow_remove_idle_timeout_timer(void)
{
    ADPT_MODULE_INIT_CHECK(g_p_adpt_flow_master);
    
    ADPT_FLOW_ERROR_RETURN(afx_timer_stop(g_p_adpt_flow_master->idle_timeout_timer));
    ADPT_FLOW_ERROR_RETURN(afx_timer_destroy(g_p_adpt_flow_master->idle_timeout_timer));

    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_flow_add_interface_manage_entry(uint16_ofp ofport)
{
    ctc_aclqos_entry_t entry;
    ctc_aclqos_entry_t entry_mac;
    uint16_ofp gport;
    uint32_ofp label_id;
    uint32_ofp after_entry_id;

    memset(&entry, 0, sizeof(ctc_aclqos_entry_t));
    memset(&entry_mac, 0, sizeof(ctc_aclqos_entry_t));
    ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    label_id = BASE_PORT_LABEL_ID + gport;

    entry.entry_id = (0xFFFFFFFF - 2 - ofport*2);
    entry.key.type = CTC_ACLQOS_IPV4_KEY;
    SET_FLAG(entry.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPSA_FLAG);
    SET_FLAG(entry.key.key_info.ipv4_key.flag, CTC_ACLQOS_IPV4_KEY_IPDA_FLAG);
    SET_FLAG(entry.action.flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);
    entry.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_MNGT_DFT_ENTRY;
    adpt_flowdb_add_flow_priority_entry_id(FLOW_TYPE_IPV4, 
                ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry.entry_id);
    adpt_flowdb_get_prev_entry_id(FLOW_TYPE_IPV4, 
            ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry.entry_id, &after_entry_id);
    ADPT_FLOW_ERROR_RETURN(hal_flow_add_qos_entry(label_id, 
                after_entry_id, &entry));

    entry_mac.entry_id = (0xFFFFFFFF - 2 - ofport*2 - 1);
    entry_mac.key.type = CTC_ACLQOS_MAC_KEY;
    SET_FLAG(entry_mac.key.key_info.mac_key.flag, CTC_ACLQOS_MAC_KEY_MACDA_FLAG);
    SET_FLAG(entry_mac.key.key_info.mac_key.flag, CTC_ACLQOS_MAC_KEY_MACSA_FLAG);
    SET_FLAG(entry_mac.action.flag, CTC_ACLQOS_ACTION_REDIRECT_FLAG);
    entry_mac.action.fwd.fwd_nh_id = ADPT_NH_RSV_NHID_FOR_MNGT_DFT_ENTRY;
    adpt_flowdb_add_flow_priority_entry_id(FLOW_TYPE_MAC, 
                ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry_mac.entry_id);
    adpt_flowdb_get_prev_entry_id(FLOW_TYPE_MAC, 
            ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry_mac.entry_id, &after_entry_id);
    ADPT_FLOW_ERROR_RETURN(hal_flow_add_qos_entry(label_id, 
                after_entry_id, &entry_mac));

    return OFP_ERR_SUCCESS;
}

int32_ofp
adpt_flow_del_interface_manage_entry(uint16_ofp ofport)
{
    ctc_aclqos_entry_t entry;
    ctc_aclqos_entry_t entry_mac;
    uint16_ofp gport;
    uint32_ofp label_id;
    uint32 entry_id; 
    
    memset(&entry, 0, sizeof(ctc_aclqos_entry_t));
    memset(&entry_mac, 0, sizeof(ctc_aclqos_entry_t));
    ADPT_FLOW_ERROR_RETURN(adpt_port_get_gport_by_ofport(ofport, &gport));
    label_id = BASE_PORT_LABEL_ID + gport;
    entry_id = (0xFFFFFFFF - 2 - ofport*2);
    ADPT_FLOW_ERROR_RETURN(hal_flow_remove_qos_entry(label_id, 
                CTC_ACLQOS_IPV4_KEY, entry_id));
    adpt_flowdb_del_flow_priority_entry_id(FLOW_TYPE_IPV4, 
        ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry_id);

    entry_id = (0xFFFFFFFF - 2 - ofport*2 - 1);
    ADPT_FLOW_ERROR_RETURN(hal_flow_remove_qos_entry(label_id, 
                CTC_ACLQOS_MAC_KEY, entry_id));
    adpt_flowdb_del_flow_priority_entry_id(FLOW_TYPE_MAC, 
        ADPT_FLOW_MANAGE_ENTRY_PRIORITY, entry_id);

    return OFP_ERR_SUCCESS;
}

/**
 * Set unmatched behavior for flow table. 
 * @param[in]  new_behavior      The new unmatched behavior.
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_set_unmatched_behavior(hal_flow_unmatched_behavior_t new_behavior)
{
    if (unmatched_behavior != new_behavior)
    {
        ADPT_FLOW_ERROR_RETURN(hal_flow_update_default_entry(new_behavior));    
        unmatched_behavior = new_behavior;
    }
    
    return OFP_ERR_SUCCESS;
}

/**
 * Get unmatched behavior for flow table. 
 * @param[in]  new_behavior      The new unmatched behavior.
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_get_unmatched_behavior(hal_flow_unmatched_behavior_t *cur_behavior)
{
    *cur_behavior = unmatched_behavior;
    
    return OFP_ERR_SUCCESS;
}

/**
 * Adapter flow init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_flow_init(void)
{
    ctc_flex_nh_param_t nh_param;

    g_p_adpt_flow_master = malloc(sizeof(adpt_flow_master_t));
    ADPT_MEM_PTR_CHECK(g_p_adpt_flow_master);
    memset(g_p_adpt_flow_master, 0, sizeof(adpt_flow_master_t));
    
    ADPT_FLOW_ERROR_RETURN(adpt_flow_db_init());

    /* OF1.3: default behavior for unmatched packet is controller to keep
     * it consistent with old version.  */
    unmatched_behavior = HAL_FLOW_UNMATCHED_TO_CONTROLLER;
    ADPT_FLOW_ERROR_RETURN(hal_flow_add_default_entry());

    /* 1. create flex nexthop, action to_cpu, dsnh_offset set to management default flow id */
    kal_memset(&nh_param, 0, sizeof(ctc_flex_nh_param_t));
    nh_param.gport = CPU_PORT_NUM;
    nh_param.dsnh_offset = FLOW_ID_MNGT_DFT_FLOW;
    HAL_ERROR_RETURN(hal_nexthop_create_flex_nh(ADPT_NH_RSV_NHID_FOR_MNGT_DFT_ENTRY, 
                        &nh_param));

    return OFP_ERR_SUCCESS;
}
