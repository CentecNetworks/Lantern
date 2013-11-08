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
 * @brief This file calls the sdk pdu APIs for adapter layer
 */

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ofp_types.h"
#include "ofp_error.h"
#include "ofp_const.h"
#include "ofp_macro.h"
#include "packets.h"

#include "hal.h"
#include "hal_pdu.h"
#include "ctc_api.h"

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

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
/**
 * Set stp packet action
 * @param[in]  gport                     gport
 * @param[in]  l2pdu_action              l2pdu action
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_l2pdu_set_stp_packet_action(uint16_ofp gport_id, ofp_pdu_l2pdu_action_t l2pdu_action)
{
    switch (l2pdu_action)
    {
        case OFP_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU:
            HAL_ERROR_RETURN(ctc_l2pdu_set_port_action(gport_id, 0, CTC_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU));
            break;

        case OFP_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU:
            HAL_ERROR_RETURN(ctc_l2pdu_set_port_action(gport_id, 0, CTC_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU));
            break;

        case OFP_PDU_L2PDU_ACTION_TYPE_FWD:
            HAL_ERROR_RETURN(ctc_l2pdu_set_port_action(gport_id, 0, CTC_PDU_L2PDU_ACTION_TYPE_FWD));
            break;
        case OFP_PDU_L2PDU_ACTION_TYPE_DISCARD:
            HAL_ERROR_RETURN(ctc_l2pdu_set_port_action(gport_id, 0, CTC_PDU_L2PDU_ACTION_TYPE_DISCARD));
            break;

        case OFP_PDU_L2PDU_ACTION_TYPE_MAX:
        default:
            break;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * init pdu module
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_pdu_init(void)
{
    /* use l2pdu to implement OFPPC_NO_RECV, Drop all packets except 802.1D spanning tree packets. */
    ctc_pdu_l2pdu_key_t entry_key;
    ctc_pdu_global_l2pdu_action_t entry_action;
    ctc_pdu_l2pdu_type_t l2pdu_type;

    l2pdu_type = CTC_PDU_L2PDU_TYPE_MACDA;
    /* 1. classify stp packet using l2pdu classifier */
    memset(&entry_key, 0, sizeof(ctc_pdu_l2pdu_key_t));
    memcpy(&entry_key.l2pdu_by_mac.mac, eth_addr_stp, sizeof(mac_addr_t));
    memcpy(&entry_key.l2pdu_by_mac.mac_mask, eth_addr_broadcast, sizeof(mac_addr_t));
    HAL_ERROR_RETURN(ctc_l2pdu_classify_l2pdu(l2pdu_type, 0, &entry_key));

    /* 2. enable l2pdu global action */
    memset(&entry_action, 0, sizeof(ctc_pdu_global_l2pdu_action_t));
    entry_action.entry_valid = 1;
    entry_action.bypass_all = 0;
    entry_action.action_index = 0;
    HAL_ERROR_RETURN(ctc_l2pdu_set_global_action(l2pdu_type, 0, &entry_action));

    return OFP_ERR_SUCCESS;
}
