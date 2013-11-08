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
 * @brief This file process the message from lcm
 */
 
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ofp_api.h"

#include "adpt.h"
#include "adpt_message.h"
#include "adpt_message_priv.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
VLOG_DEFINE_THIS_MODULE(adapt_message);

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
adpt_message_master_t* g_p_adpt_message_master = NULL;
  
/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Dispatch message from lcm
 * @param type          message type
 * @param arg           message arg
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_dispatch_msg_from_lcm(adpt_msg_type_t type, void* arg)
{
    int32_ofp ret = OFP_ERR_SUCCESS;
    
    if (ADPT_MSG_TYPE_MODULE_INIT == type && NULL == g_p_adpt_message_master)
    {
        ADPT_ERROR_RETURN(adpt_module_init());
        
        return OFP_ERR_SUCCESS;
    }
    
    ADPT_MODULE_INIT_CHECK(g_p_adpt_message_master);
    if (g_p_adpt_message_master->lcm2adpt_msg_cb[type])
    {
        ret = g_p_adpt_message_master->lcm2adpt_msg_cb[type](arg);
        if (ret)
        {
            ADPT_LOG_ERROR("Fail to process lcm message %d, ret = %d\n", type, ret)
        }
    }
    else
    {
        return OFP_ERR_LCM_MESSAGE_FUNC_NOT_INIT;
    }

    return OFP_ERR_SUCCESS;
}

/**
 * Register callback function for lcm message
 * @param type          message type
 * @param func          callback function
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_register_lcm_callback(adpt_msg_type_t type, ADPT_MESSAGE_CB_FUNC func)
{
    ADPT_MODULE_INIT_CHECK(g_p_adpt_message_master);
    g_p_adpt_message_master->lcm2adpt_msg_cb[type] = func;

    return OFP_ERR_SUCCESS;
}

/**
 * Adapt layer message init
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_message_init(void)
{
    if (g_p_adpt_message_master)
    {
        return OFP_ERR_SUCCESS;
    }
    g_p_adpt_message_master = malloc(sizeof(adpt_message_master_t));

    ADPT_MEM_PTR_CHECK(g_p_adpt_message_master);
    memset(g_p_adpt_message_master, 0, sizeof(adpt_message_master_t));

    return OFP_ERR_SUCCESS;
}
