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
 * @brief This file is the header file of hal_pdu.c
 */

#ifndef __HAL_PDU_H__
#define __HAL_PDU_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "ctc_pdu.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
/*
*  @brief pdu l2pdu action
*/
enum ofp_pdu_l2pdu_action_e
{
    OFP_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU,         /**<layer2 pdu action type: redirect to cpu*/
    OFP_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU,             /**<layer2 pdu action type: copy to cpu*/
    OFP_PDU_L2PDU_ACTION_TYPE_FWD,                     /**<layer2 pdu action type: normal forwarding*/
    OFP_PDU_L2PDU_ACTION_TYPE_DISCARD,                 /**<layer2 pdu action type: discard the pdu*/

    OFP_PDU_L2PDU_ACTION_TYPE_MAX
};
typedef enum ofp_pdu_l2pdu_action_e ofp_pdu_l2pdu_action_t;

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
hal_l2pdu_set_stp_packet_action(uint16_ofp gport_id, ofp_pdu_l2pdu_action_t l2pdu_action);

/**
 * init pdu module
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_pdu_init(void);

#endif
