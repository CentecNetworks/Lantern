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
 * @brief This file defines macros used in adapter layer
 */

#ifndef __OFP_MACRO_H__
#define __OFP_MACRO_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

extern int ctc_cli_out_ofp(const char *fmt, ...);
extern uint16_ofp ofp_get_debug_mode(void);

/**
 @brief define bit operations  
*/
#define SET_FLAG(V,F)        (V) = (V) | (F)
#define UNSET_FLAG(V,F)      (V) = (V) & ~(F)
#define IS_FLAG_SET(V,F)        (((V) & (F)) != 0)

#define OFP_PTR_CHECK(ptr)                                              \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        VLOG_ERR("Invalid pointer, %s()@%d", __FUNCTION__, __LINE__);    \
        return (OFP_ERR_INVALID_PTR);                                   \
    }                                                                   \
}while(0)

#define OFP_MEM_PTR_CHECK(ptr)                                          \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        VLOG_ERR("No memory, %s()@%d", __FUNCTION__, __LINE__);         \
        return (OFP_ERR_NO_MEMORY);                                     \
    }                                                                   \
}while(0)

#define OFP_ERROR_RETURN(op) \
do { \
    int rv; \
    if ((rv = (op)) < 0) \
    {\
        OFP_LOG_DEBUG("%s:%d, rv = %d\n", __FUNCTION__, __LINE__, rv); \
        return(rv); \
    }\
}while(0)

#define OFP_DEBUG_PRINT_TO_FILE(fmt, args...)                      \
{ \
    FILE * fp = fopen("/mnt/flash/log1.txt", "a"); \
    if (fp != NULL) \
    { \
        fprintf(fp, fmt, ##args); \
        fclose(fp); \
    } \
}

#define OFP_DEBUG_PRINT(fmt, args...)                       \
    OFP_LOG_DEBUG(fmt, ##args);

#define OFP_CONSOLE_PRINT(fmt, args...)                     \
{                                                           \
    FILE * fp_console = NULL;                               \
    fp_console = fopen("/dev/console", "a+");               \
    fprintf(fp_console, fmt, ##args);                       \
    fclose(fp_console);                                     \
}

#if 1
#define OFP_LOG_DEBUG_FUNC()                                \
{                                                           \
    VLOG_DBG("Enter into %s\n", __FUNCTION__);              \
}
#else
#define OFP_LOG_DEBUG_FUNC()
#endif

#define OFP_LOG_INFO(fmt, args...)                          \
{                                                           \
    VLOG_INFO(fmt"\n", ##args);                             \
}

#if 1
#define OFP_LOG_DEBUG(fmt, args...)                         \
{                                                           \
    VLOG_DBG(fmt"\n", ##args);                              \
}
#else
#define OFP_LOG_DEBUG(fmt, args...)
#endif

#define OFP_LOG_ERROR(fmt, args...)                         \
{                                                           \
    VLOG_ERR(fmt"\n", ##args);                              \
}

#endif /* !__OFP_MACRO_H__ */
