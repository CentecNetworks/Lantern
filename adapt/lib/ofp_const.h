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
 * @brief This file defines constants
 */

#ifndef __OFP_CONST_H__
#define __OFP_CONST_H__

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


/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/*******************************************************************
*
*Structures and macros, enums
*
********************************************************************/

#define OFP_ETH_ADDR_LEN           6
#define OFP_IPUC_V4_MAX_MASK_LEN   32
#define OFP_IFNAME_SIZE            16
#define OFP_DEFAULT_NETDEV_TYPE    "switch"

/* match IP protocol type */
#define IP_PROTO_ICMP 1
#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17
#define IP_PROTO_GRE 47

#define OFP_UINT16_MAX      0xffff
#define OFP_UINT32_MAX      0xffffffff
#define OFP_UINT64_MAX      0xffffffffffffffffLLU

#endif  /* !__OFP_CONST_H__ */
