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
 * @brief This file is the header file of adpt.c
 */

#ifndef __ADPT_H__
#define __ADPT_H__

/******************************************************************************
* Header Files 
******************************************************************************/

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

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/
#define ADPT_LEARNING_CACHE_MAX_INDEX          16 

#ifdef _OPEN_SOURCE_
#define ctc_cli_out_ofp(fmt, args...)                                    \
{                                                                       \
    printf(fmt"\n", ##args);                                          \
}
#endif

#define ADPT_ERROR_RETURN(op)                                           \
do {                                                                    \
    int rv;                                                             \
    if ((rv = (op)) < 0)                                                \
    {                                                                   \
        ADPT_LOG_DEBUG("%s:%d, rv = %d\n", __FUNCTION__, __LINE__, rv); \
        return(rv);                                                     \
    }                                                                   \
}while(0)

#define ADPT_ERROR_GOTO(op, ERR)                                        \
do {                                                                    \
    int rv;                                                             \
    if ((rv = (op)) < 0)                                                \
    {                                                                   \
        ADPT_LOG_DEBUG("%s:%d, rv = %d\n", __FUNCTION__, __LINE__, rv); \
        goto ERR;                                                       \
    }                                                                   \
}while(0)

#define ADPT_PTR_CHECK(ptr)                                             \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        VLOG_ERR("Invalid pointer, %s()@%d", __FUNCTION__, __LINE__);   \
        return (OFP_ERR_INVALID_PTR);                                   \
    }                                                                   \
}while(0)

#define ADPT_MEM_PTR_CHECK(ptr)                                         \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        VLOG_ERR("No memory, %s()@%d", __FUNCTION__, __LINE__);         \
        return (OFP_ERR_NO_MEMORY);                                     \
    }                                                                   \
}while(0)

#define ADPT_MODULE_INIT_CHECK(ptr)                                     \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        VLOG_ERR("%s:%d, not init\n", __FUNCTION__, __LINE__);          \
        return (OFP_ERR_NOT_INIT);                                      \
    }                                                                   \
}while(0)

#if 1
#define ADPT_LOG_DEBUG_FUNC()                                           \
{                                                                       \
    VLOG_DBG("Enter into %s\n", __FUNCTION__);                          \
}
#else
#define ADPT_LOG_DEBUG_FUNC() 
#endif

#define ADPT_LOG_INFO(fmt, args...)                                     \
{                                                                       \
    VLOG_INFO(fmt"\n", ##args);                                         \
}

#if 1
#define ADPT_LOG_DEBUG(fmt, args...)                                    \
{                                                                       \
    VLOG_DBG(fmt"\n", ##args);                                          \
}
#else
#define ADPT_LOG_DEBUG(fmt, args...)
#endif

#define ADPT_LOG_WARN(fmt, args...)                                     \
{                                                                       \
    VLOG_WARN(fmt"\n", ##args);                                         \
}

#define ADPT_LOG_ERROR(fmt, args...)                                    \
{                                                                       \
    VLOG_ERR(fmt"\n", ##args);                                          \
}

#define ADPT_METER_INV_ID             0

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * Initialize vlan for openflow
 */
int32_ofp
adpt_vlan_init(void);

int32_ofp
adpt_misc_init(void);

int32_ofp
adpt_module_init(void);

#endif
