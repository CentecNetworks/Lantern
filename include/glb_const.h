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
 */

#ifndef __GLB_CONST_H__
#define __GLB_CONST_H__

#define GLB_ETH_ADDR_LEN                6
#define GLB_IPV6_ADDR_LEN               4
#define GLB_MAX_VLAN_ID                 4095
#define GLB_MIN_VLAN_ID                 0
#define GLB_BITS_NUM_OF_BYTE            8
#define GLB_BITS_NUM_OF_WORD            32
#define GLB_BITS_SHIFT_OF_WORD          5
#define GLB_BITS_MASK_OF_WORD           0x1F
#define GLB_MAX_UINT16_VALUE            0xFFFF 
#define GLB_MAX_UINT32_VALUE            0xFFFFFFFF 
#define GLB_MAX_UINT8_VALUE             0xFF
#define GLB_IPV6_ADDR_LEN_IN_BYTE       16
#define GLB_IPV4_ADDR_LEN_IN_BYTE       4
#define GLB_IPV6_ADDR_LEN_IN_BIT        128
#define GLB_IPV4_ADDR_LEN_IN_BIT        32
#define GLB_MAX_CHIP_NUM                32
#define GLB_LEN_VLANBMP_BY_WORD         ((GLB_MAX_VLAN_ID+1)/GLB_BITS_NUM_OF_WORD)
#define GLB_VLAN_FID_NUM_VPLS           2048

/* supervisor and linecard connection msg */
#define LCM_MSG_VRF                     65
#define LCM_MSG_IP_SERIES_E800          INADDR_ANY
#define LCM_MSG_PORT                    6000
#define LCM_BUF_SZ                      0x1000
#define LCM_SRV_CB_TBL_SZ               0x100

#define KEEPALIVE_TIME_OUT_COUNT        6
#define KEEPALIVE_INTERVAL              3000
#define PHY_STATUS_MONITOR_INTERVAL     1000
#define FIBER_MODULE_UPDATE_COUNT       5

#define MAX_DESC_STR_LEN                16

#define MAX_DIAG_ENTRY_NUM              128
#define MAX_DUMP_CHIP_MOD_DATA_NUM      132

/*Socket buf define*/
#define GLB_HAL2HSRV_MSG_BUF_LEN        8192
#define GLB_HAGT2SRV_MSG_BUF_LEN        4096
#define GLB_HSRV2AGT_MSG_BUF_LEN        4096
#define GLB_HSRVAGT_MSG_VRF                LCM_MSG_VRF
#define GLB_HSRVAGT_MSG_IP_SERIES_E800     INADDR_ANY
#define GLB_HSRVAGT_MSG_PORT               7000

#define GLB_PETH_PKT_BUF_LEN            9800

#define GLB_OUTER_VLAN_IS_SVLAN         1

#define GLB_ERROR_BASE                  -6000
#define GLB_HSRV_ERROR_BASE             -1000
#define GLB_HAGT_ERROR_BASE             -10000
#define GLB_CTCLIB_ERROR_BASE           -20000
#define GLB_LCSH_ERROR_BASE             -3000

#define GLB_VLAN_HDR_LEN                4

#define GLB_INVALID_FD                  (-1)
#define GLB_INVALID_CHIPID              (-1)
#define GLB_INVALID_SLOTID              (-1)
#define GLB_INVALID_PORTID              (-1)

#define GLB_MAX_INTERNAL_PORT_ID 191
#define GLB_RESERVED_PORT_FOR_ILOOP_POP (GLB_MAX_INTERNAL_PORT_ID-1)
#define GLB_RESERVED_PORT_FOR_ILOOP_L3VPN (GLB_MAX_INTERNAL_PORT_ID-2)
#define GLB_RESERVED_PORT_FOR_ILOOP_VPLS_RAW (GLB_MAX_INTERNAL_PORT_ID-3)
#define GLB_RESERVED_PORT_FOR_ILOOP_VPLS_TAGGED (GLB_MAX_INTERNAL_PORT_ID-4)
#define GLB_RESERVED_PORT_FOR_ILOOP_VPLS_RAW_CWEN (GLB_MAX_INTERNAL_PORT_ID-5)
#define GLB_RESERVED_PORT_FOR_ILOOP_VPLS_TAGGED_CWEN (GLB_MAX_INTERNAL_PORT_ID-6)

#define GLB_ILOOP_TYPE_POP 1
#define GLB_ILOOP_TYPE_L3VPN 2
#define GLB_ILOOP_TYPE_VPLS_RAW 3
#define GLB_ILOOP_TYPE_VPLS_TAGGED 4

#define GLB_TPID_INDEX_0   0
#define GLB_TPID_INDEX_1   1
#define GLB_TPID_INDEX_2   2
#define GLB_TPID_INDEX_3   3
#define GLB_ETH_P_8021Q     0x8100

typedef enum
{
    GLB_TPID_TYPE_1,
    GLB_TPID_TYPE_2,        

    GLB_TPID_TYPE_INVALID,    
} glb_tpid_type_t;

enum glb_direction_e
{
    GLB_INGRESS,
    GLB_EGRESS,
    GLB_BOTH_DIRECTION,
};
typedef enum glb_direction_e glb_direction_t;

/*Notes: 
when add error code in this enum,
please add error string in ctclib_debug.c:
glb_err_mapper_t g_ast_err_mapper[] =
*/
enum glb_errno_e
{
    GLB_E_NONE = 0,
        
    GLB_E_GENERAL = GLB_ERROR_BASE,/*-6000*/
    GLB_E_NO_MEMORY,
    GLB_E_RESOURCE_FULL,    
    GLB_E_INVALID_PARAM,
    GLB_E_INVALID_PTR,
    
    GLB_E_SOCK_CREATE_ERR,/*-5995*/
    GLB_E_SOCK_BIND_ERR,
    GLB_E_SOCK_ACCEPT_ERR,
    GLB_E_SOCK_READ_ERR,
    GLB_E_SOCK_WRITE_ERR,
    
    GLB_E_MSG_LEN_TOO_SHORT,/*-5990*/
    GLB_E_MSG_LEN_TOO_LONG,
    GLB_E_MSG_TYPE_EER,    
    GLB_E_NOT_INIT,    
    GLB_E_ENTRY_NOT_EXIST,    
    
    GLB_E_ENTRY_EXISTED,/*-5985*/
    GLB_E_ENTRY_CONFLICT, 
    GLB_E_ENTRY_EXCEED_MAX, 
    GLB_ERR_FDB_CANT_OW_SECURITY_ENTRY,
    GLB_ERR_SET_SECURITY_MAX,
    
    GLB_E_PORT_SECURITY_RESOURCE_FULL,
    GLB_E_VLAN_SECURITY_RESOURCE_FULL,
    GLB_E_VPLS_SECURITY_RESOURCE_FULL,
    
    GLB_E_MAX
};
typedef enum glb_errno_e glb_errno_e_t;

#endif /* !__GLB_CONST_H__*/
