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
 * @brief This file defines types
 */

#ifndef __OFP_TYPES_H__
#define __OFP_TYPES_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include <sys/types.h>
#include <netinet/in.h>
#include "sal.h"
#include "kal.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

#ifndef int8_ofp
typedef int8_t int8_ofp; /**< 8-bit  signed integer */
#endif

#ifndef int16_ofp
typedef int16_t int16_ofp; /**< 16-bit signed integer */
#endif

#ifndef int32_ofp
typedef int32_t int32_ofp; /**< 32-bit signed integer */
#endif

#ifndef int64_ofp
typedef long long int64_ofp; /**< 64-bit signed integer */
#endif

#ifndef uint8_ofp
typedef uint8_t uint8_ofp; /**< 8-bit  unsigned integer */
#endif

#ifndef uint16_ofp
typedef uint16_t uint16_ofp; /**< 16-bit unsigned integer */
#endif

#ifndef uint32_ofp
typedef uint32_t uint32_ofp; /**< 32-bit unsigned integer */
#endif

#ifndef uint64_ofp
typedef unsigned long long uint64_ofp; /**< 64-bit unsigned integer */
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* !__OFP_TYPES_H__ */
