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

#ifndef __GLB_L2_DEFINE_H__
#define __GLB_L2_DEFINE_H__

#include "glb_const.h"

#define GLB_BR_PORT_STATE_DISABLED         0
#define GLB_BR_PORT_STATE_LISTENING        1
#define GLB_BR_PORT_STATE_LEARNING         2
#define GLB_BR_PORT_STATE_FORWARDING       3
#define GLB_BR_PORT_STATE_BLOCKING         4
#define GLB_BR_PORT_STATE_MAX              5
#define GLB_BR_PORT_STATE_EDGE             7

#define GLB_FDB_FLAG_STATIC               (1 << 0)
#define GLB_FDB_FLAG_SRC_DISC_EXCEPT      (1 << 1)
#define GLB_FDB_FLAG_COPY_TOCPU           (1 << 2)
#define GLB_FDB_FLAG_DISC                 (1 << 3)
#define GLB_FDB_FLAG_KP_EMPTY_MCAST_ENT   (1 << 4)  /* when a L2 Mcast entry has no any members,
                                                       still keep the entry in ASIC so that all
                                                          the matched traffic will be dropped */
#define GLB_FDB_FLAG_FWD_TOCPU            (1 << 5)
#define GLB_FDB_FLAG_EGS_VLAN_TRANS       (1 << 6)
#define GLB_FDB_FLAG_MAC_SECURITY_EN      (1 << 7)
#define GLB_FDB_FLAG_INVALID              (1 << 8) /* only use for static arp on vlan interface, no out port */
#define GLB_FDB_FLAG_SET_SECUR_DISCARD    (1 << 9)/*When add security entry, set this flag means enable macSecurityDiscard,
                                                                                               If delete security entry, set this flag means disable macSecurity Discard*/

#define GLB_VLAN_MAP_TYPE_NONE             (1 << 0)
#define GLB_VLAN_MAP_TYPE_BASIC_QINQ       (1 << 1)
#define GLB_VLAN_MAP_TYPE_SELECTIVE_QINQ   (1 << 2)
#define GLB_VLAN_MAP_TYPE_VLAN_REPLACE     (1 << 3)

#define GLB_VLAN_CLASS_MAC_BASED            0x1
#define GLB_VLAN_CLASS_IP_BASED             0x2
#define GLB_VLAN_CLASS_PROTOCOL_BASED       0x4

/* Filter type */
#define GLB_VLAN_CLASSIFIER_MAC       1  /* filter on source MAC */
#define GLB_VLAN_CLASSIFIER_PROTOCOL  2  /* filter on protocol */
#define GLB_VLAN_CLASSIFIER_IPV4      4  /* filter on src IPv4 subnet */
#define GLB_VLAN_CLASSIFIER_IPV6      8  /* filter on src IPv6 subnet */

#define GLB_IP_BINDING_IP        (1 << 0)
#define GLB_IP_BINDING_IPV6      (1 << 1)
#define GLB_IP_BINDING_PORT      (1 << 2)
#define GLB_IP_BINDING_MAC       (1 << 3)
#define GLB_IP_BINDING_VLAN      (1 << 4)


#define GLB_VLAN_MAX                 4094              /* Max VLANs.   */
#define GLB_VLAN_ALL                 (GLB_VLAN_MAX + 1)/* All VLANs.   */
#define GLB_VLAN_NONE                0                 /* No VLANs.    */
#define GLB_VLAN_DEFAULT_VID         1                 /* Default VID. */

/* Bit defines only for HalMsgPortState_t.state */
#define GLB_VLAN_FLAG_DEF_ENTRY                   (1 << 0)

/* Bit defines only for hsrv2AgtMsgL2VlanSync.flag */
#define GLB_VLAN_FLAG_STATE_ENABLE                   (1 << 0)
#define GLB_VLAN_FLAG_BRIDGE_ENABLE                   (1 << 1)
#define GLB_VLAN_FLAG_REPLACE_DSCP                   (1 << 2)
#define GLB_VLAN_FLAG_LEARNING_ENABLE                   (1 << 3)

#define GLB_STP_INSTANCE_MAX             16
#define GLB_STP_INSTANCE_ALL          0xFF

#define GLB_MIRROR_MAX_SESSION_ID  3
#define GLB_MIRROR_MIN_SESSION_ID  1
#define GLB_MIRROR_DIR_IGS          (1 << 0)
#define GLB_MIRROR_DIR_EGS          (1 << 1)
#define GLB_MIRROR_DIR_BOTH         ((1 << 0) | (1 << 1))
#define GLB_MIRROR_MAC_ESCAPE_MAX  2

#define GLB_DOT1X_AUTHED    1
#define GLB_DOT1X_UNAUTHED  2
#define GLB_DOT1X_UNCTRL    3
#define GLB_DOT1X_DIR_IN    1
#define GLB_DOT1X_DIR_BOTH  2

#define GLB_VLAN_MAPPING_TAB_NAME_MAX 16

#define GLB_CFM_MAID_LENGTH       16
#define GLB_CFM_UP_MEP             1
#define GLB_CFM_DOWN_MEP           0

#define GLB_CFM_ETHERTYPE        0x8902
#define GLB_CFM_OPERATION_TYPE      3
#define GLB_CFM_MAX_ERROR_CACHE_NUM     16

#define GLB_CFM_INVALID_LEVEL       8

#define GLB_CFM_MODE_DOT1AG             0
#define GLB_CFM_MODE_Y1731              1

#define GLB_CFM_MAX_LMEP_ERROR_NUM      10
#define GLB_CFM_MAX_RMEP_ERROR_NUM      10

#define GLB_FDB_FLOOD_ALL           (1 << 1)
#define GLB_FDB_FLOOD_UNKOWN_UCAST  (1 << 2)
#define GLB_FDB_FLOOD_MCAST         (1 << 3)
#define GLB_FDB_FLOOD_BROADCAST     (1 << 4)

#define GLB_APS_TYPE_PERNANENT     1
#define GLB_APS_TYPE_NON_PERNANENT 2
#define GLB_APS_STATE_BLOCKING     (1 << 0)
//#define GLB_APS_STATE_SELECT       (1 << 1)

#define GLB_VOICE_VLAN_OUI_DEF_NUM            5
#define GLB_VOICE_VLAN_OUI_NUM                16
#define GLB_VOICE_VLAN_OUI_DESC               128

/* OAM exceptions in packet info */
enum glb_oam_exception_e
{
    GLB_OAM_EXCEPTION_OAM_PDU_INVALID                        = 0,
    GLB_OAM_EXCEPTION_SOME_RDI_DEFECT                        = 1,
    GLB_OAM_EXCEPTION_SOME_MAC_STATUS_DEFECT                 = 2,
    GLB_OAM_EXCEPTION_HIGH_VERSION_ETH_OAM_TO_CPU            = 3,
    GLB_OAM_EXCEPTION_ERROR_CCM_DEFECT_RMEP_NOT_FOUND        = 4,
    GLB_OAM_EXCEPTION_XCON_CCM_DEFECT                        = 5,
    GLB_OAM_EXCEPTION_CCM_SEQ_NUM_ERROR                      = 6,
    GLB_OAM_EXCEPTION_DS_OR_CTRL_REG_CONFIG_ERROR            = 7,
    GLB_OAM_EXCEPTION_ETH_CCM_TLV_OPTION                     = 8,
    GLB_OAM_EXCEPTION_SLOW_OAM_PDU_TO_CPU                    = 9,
    GLB_OAM_EXCEPTION_RESERVED0                              = 10,
    GLB_OAM_EXCEPTION_HIGHER_CCM_ON_CBP_PIP                  = 11,
    GLB_OAM_EXCEPTION_OAM_LOOKUP_NOT_EN_TO_CPU               = 12,
    GLB_OAM_EXCEPTION_SOURCE_MAC_MISMATCH                    = 13,
    GLB_OAM_EXCEPTION_FDI_TO_CPU                             = 14,
    GLB_OAM_EXCEPTION_BDI_TO_CPU                             = 15,
    GLB_OAM_EXCEPTION_APS_PDU_TO_CPU                         = 16,
    GLB_OAM_EXCEPTION_ERROR_CCM_DEFECT_CCM_INTERVAL_MISMATCH = 17,
    GLB_OAM_EXCEPTION_DM_TO_CPU                              = 18,
    GLB_OAM_EXCEPTION_MD_LEVEL_LOWER_THAN_PASSIVE_MEP        = 19,
    GLB_OAM_EXCEPTION_MPLS_OR_PBT_MEP_CONFIG_ERROR           = 20,
    GLB_OAM_EXCEPTION_MPLS_OR_PBT_CHECK_ERROR                = 21,
    GLB_OAM_EXCEPTION_ALL_PBB_BSI_PDU_TO_CPU                 = 22,
    GLB_OAM_EXCEPTION_PBT_MM_DEFECT_PDU_TO_CPU               = 23,
    GLB_OAM_EXCEPTION_MPLS_CV0_TO_CPU                        = 24,
    GLB_OAM_EXCEPTION_EQUAL_LBR_TO_CPU                       = 25,
    GLB_OAM_EXCEPTION_LBM_MAC_DA_CHECK_FAIL                  = 26,
    GLB_OAM_EXCEPTION_LM_TO_CPU                              = 27,
    GLB_OAM_EXCEPTION_LEARNING_BFD_TO_CPU                    = 28,
    GLB_OAM_EXCEPTION_ERROR_BFD_TO_CPU                       = 29,
    GLB_OAM_EXCEPTION_RESERVED1                              = 30,
    GLB_OAM_EXCEPTION_ERROR_CACHE_FULL                       = 31,
};

enum glb_cfm_error_reason_e
{
    GLB_CFM_RMEP_SRC_MAC_MISMATCH                        = 0,
    GLB_CFM_SOME_RDI_DEFECT                              = 1,
    GLB_CFM_SOME_RDI_DEFECT_CLEAR                        = 2,
    GLB_CFM_SOME_MAC_STATUS_DEFECT                       = 3,
    GLB_CFM_SOME_RMEP_CCM_DEFECT_DLOC                    = 4,
    GLB_CFM_SOME_RMEP_CCM_DEFECT_DLOC_CLEAR              = 5,
    GLB_CFM_ERROR_CCM_DEFECT_RMEP_NOT_FOUND              = 6,
    GLB_CFM_ERROR_CCM_DEFECT_RMEP_NOT_FOUND_CLEAR        = 7,
    GLB_CFM_ERROR_CCM_DEFECT_CCM_INTERVAL_MISMATCH       = 8,
    GLB_CFM_ERROR_CCM_DEFECT_CCM_INTERVAL_MISMATCH_CLEAR = 9,
    GLB_CFM_XCON_CCM_DEFECT_CCM_MAID_MISMATCH            = 10,
    GLB_CFM_XCON_CCM_DEFECT_CCM_MAID_MISMATCH_CLEAR      = 11,
    GLB_CFM_XCON_CCM_DEFECT_CCM_LOW_CCM                  = 12,
    GLB_CFM_XCON_CCM_DEFECT_CCM_LOW_CCM_CLEAR            = 13,
    GLB_CFM_AIS_DEFECT_AIS_RECEIVE                       = 14,
    GLB_CFM_AIS_DEFECT_AIS_RECEIVE_CLEAR                 = 15,
    GLB_CFM_UNKNOWN_REASON,
};
typedef enum glb_cfm_error_reason_e glb_cfm_error_reason_t;

struct glb_fdb_info_s
{
    uint8_t mac[6];
    uint16_t fid;
    uint16_t gl_port;
    uint16_t flag;
    uint16_t l3_type;
    uint16_t is_valid;
    uint32_t nhid;

    uint8_t   gsrcport_type;
    uint16_t  virtual_port;       /**< virtual Port Number */

    uint8_t is_ether_oam;        /* If set, indicate packet is Ethernet OAM */
    uint8_t ether_oam_md_level;  /* Ethernet OAM MD Level */
    uint16_t  reserved1;
    uint8_t   reserved2;
    uint32_t vpls_peer_ip; /*for show mac addr vpls only*/
};
typedef struct glb_fdb_info_s glb_fdb_info_t;

enum glb_port_security_mode_e
{
    GLB_PORT_SECURITY_MODE_NONE = 0,
    GLB_PORT_SECURITY_MODE_PORT,
    GLB_PORT_SECURITY_MODE_VLAN,
    GLB_PORT_SECURITY_MODE_VLAN_EX
};
typedef enum glb_port_security_mode_e glb_port_security_mode_t;

enum glb_port_security_violation_mode_e
{
    GLB_PORT_SECURITY_VIOLATION_MODE_PROTECT = 0,
    GLB_PORT_SECURITY_VIOLATION_MODE_RESTRICT,
    GLB_PORT_SECURITY_VIOLATION_MODE_SHUTDOWN
};
typedef enum glb_port_security_violation_mode_e glb_port_security_violation_mode_t;

enum glb_vlan_security_action_e
{
    GLB_VLAN_SECURITY_ACTION_NONE,
    GLB_VLAN_SECURITY_ACTION_DISCARD,
    GLB_VLAN_SECURITY_ACTION_WARN,
    GLB_VLAN_SECURITY_ACTION_FORWARD
};
typedef enum glb_vlan_security_action_e glb_vlan_security_action_t;

#define GLB_VLAN_SECURITY_MAX_MSG_VLAN  16
#define GLB_VLAN_SEC_ACT_TO_STR(action)                         \
    (action == GLB_VLAN_SECURITY_ACTION_NONE ? "None" :         \
    action == GLB_VLAN_SECURITY_ACTION_DISCARD ? "Discard" :    \
     action == GLB_VLAN_SECURITY_ACTION_WARN ? "Warn" :         \
     action == GLB_VLAN_SECURITY_ACTION_FORWARD ? "Forward" :   \
     "error action")

#define GLB_PORT_SECURITY_MAC_DEFAULT_MAXIMUM  1

#define GLB_L2_PROTOCOL_MAC_MAX 2
#define GLB_L2_PROTOCOL_DEFAULT_COS 0

enum glb_l2_proto_e
{
    GLB_PROTO_MAC00 = 0,
    GLB_PROTO_MAC01,
    GLB_PROTO_BPDU,
    GLB_PROTO_LACP_SLOW_PROTO,
    GLB_PROTO_EAPOL,
    GLB_PROTO_MAX
};
typedef enum glb_l2_proto_e glb_l2_proto_t;

enum glb_l2_proto_process_e
{
    GLB_L2_PROTO_PEER = 0,
    GLB_L2_PROTO_TUNNEL,
    GLB_L2_PROTO_DISCARD,
    GLB_L2_PROTO_PROCESS_MAX,
};
typedef enum glb_l2_proto_process_e glb_l2_proto_process_t;

enum glb_dhcp_pkt_excp_type_e
{
    GLB_DHCPPKT_FORWORD_CPU = 0,
    GLB_DHCPPKT_FORWORD,
    GLB_DHCPPKT_CPU,
    GLB_DHCPPKT_DISGARD
};
typedef enum glb_dhcp_pkt_excp_type_e glb_dhcp_pkt_excp_type_t;


enum glb_arp_excp_type_e
{
    GLB_ARPPKT_FORWORD_CPU = 0,
    GLB_ARPPKT_FORWORD,
    GLB_ARPPKT_CPU,
    GLB_ARPPKT_DISGARD
};
typedef enum glb_arp_excp_type_e glb_arp_excp_type_t;


struct glb_mef_l2_proto_prop_s
{
    glb_l2_proto_process_t l2_proto_action;
    uint16_t tunnel_vid;
};
typedef struct glb_mef_l2_proto_prop_s glb_mef_l2_proto_prop_t;

enum glb_efm_mux_action_s
{
    GLB_MUX_ACTION_FWD,
    GLB_MUX_ACTION_DISCARD,
    GLB_MUX_ACTION_INVALID,
};
typedef enum glb_efm_mux_action_s glb_efm_mux_action_t;

enum glb_efm_par_action_s
{
    GLB_PAR_ACTION_FWD,
    GLB_PAR_ACTION_LB,
    GLB_PAR_ACTION_DISCARD,
    GLB_PAR_ACTION_INVALID,
};
typedef enum glb_efm_par_action_s glb_efm_par_action_t;

struct glb_efm_par_mux_action_s
{
    u_int8_t  par_action;
    u_int8_t  mux_action;
    u_int16_t reserved2;
};
typedef struct glb_efm_par_mux_action_s glb_efm_par_mux_action_t;

enum glb_l2_fdb_port_type_e
{
    GLB_L2_FDB_NORMAL = 0, /**< from normal port */
    GLB_L2_FDB_TYPE_VPLS = 1,   /**< from vpls port/qinq service queue application */
    GLB_L2_FDB_TYPE_PBB = 2,    /**< from pbb PIP */
    MAX_L2_FDB_TYPE,
};
typedef enum glb_l2_fdb_port_type_e glb_l2_fdb_port_type_t;

enum glb_port_security_enable_e
{
    GLB_PORT_SECURITY_DISABLE = 0,
    GLB_PORT_SECURITY_ENABLE
};
typedef enum glb_port_security_enable_e glb_port_security_enable_t;

struct glb_l2_delete_static_fdb_s
{
    uint16_t    vid;
    uint8_t     mac[6];
    uint32_t    ifindex;
    uint32_t    flag;
};
typedef struct glb_l2_delete_static_fdb_s glb_l2_delete_static_fdb_t;

#define GLB_MAX_OAM_FDB_ENTRIES 32

struct glb_l2_oam_fdb_s
{
    uint16_t    vid;
    uint8_t     mac[6];
    uint8_t     level;
    uint32_t    ifindex;
    uint8_t     bstatic;
    uint8_t     reserved[2];
};
typedef struct glb_l2_oam_fdb_s glb_l2_oam_fdb_t;

struct glb_l2_oam_fdb_entries_s
{
    uint32_t            count;
    glb_l2_oam_fdb_t    oam_fdb_entry[GLB_MAX_OAM_FDB_ENTRIES];
};
typedef struct glb_l2_oam_fdb_entries_s glb_l2_oam_fdb_entries_t;

struct glb_cfm_error_cache_entry_s
{
    uint8_t error_reason;         /**<Error reason */
    uint8_t port_status_valid;   /**<If port status is valid */
    uint8_t port_status_value;   /**<Port status value */
    uint8_t intf_status_valid;   /**<If interface status is valid */
    uint8_t intf_status_value;   /**<Interface status value */
    uint8_t defect_priority;     /**<Defect priority */
    uint8_t  level;
    uint16_t vlan_id;
    uint16_t rmep_id;         /**<Rmep id */
    uint16_t mep_id;          /**<Mep id */
    uint8_t is_error_cache_valid;/**<If this entry is valid */
    uint8_t present_rdi;
    uint8_t rmep_last_rdi;
};
typedef struct glb_cfm_error_cache_entry_s glb_cfm_error_cache_entry_t;

struct glb_cfm_error_cache_s
{
    uint8_t error_num;
    glb_cfm_error_cache_entry_t error_entry[GLB_CFM_MAX_ERROR_CACHE_NUM];
};
typedef struct glb_cfm_error_cache_s glb_cfm_error_cache_t;

struct glb_cfm_lmep_error_entry_s
{
    uint8_t  level;
    uint16_t vlan_id;
    uint16_t mep_id;

    uint8_t   present_rdi;
    uint8_t   d_unexp_mep;
    uint8_t   d_mismerge;
    uint8_t   d_meg_lvl;
    uint8_t   reserved[3];
};
typedef struct glb_cfm_lmep_error_entry_s glb_cfm_lmep_error_entry_t;

struct glb_cfm_lmep_error_cache_s
{
    uint8_t num;
    glb_cfm_lmep_error_entry_t entry[GLB_CFM_MAX_LMEP_ERROR_NUM];
};
typedef struct glb_cfm_lmep_error_cache_s glb_cfm_lmep_error_cache_t;

struct glb_cfm_rmep_error_entry_s
{
    uint8_t  level;
    uint16_t vlan_id;
    uint16_t mep_id;
    uint16_t rmep_id;

    uint8_t   last_rdi;
    uint8_t   d_loc;
    uint8_t   d_unexp_period;
    uint16_t   reserved;
};
typedef struct glb_cfm_rmep_error_entry_s glb_cfm_rmep_error_entry_t;

struct glb_cfm_rmep_error_cache_s
{
    uint8_t num;
    glb_cfm_rmep_error_entry_t entry[GLB_CFM_MAX_RMEP_ERROR_NUM];
};
typedef struct glb_cfm_rmep_error_cache_s glb_cfm_rmep_error_cache_t;

struct glb_cfm_size_s
{
    uint16_t cfm_local_and_remote_meps;
};
typedef struct glb_cfm_size_s glb_cfm_size_t;

typedef struct glb_voice_vlan_oui_s
{
    uint8_t mac[GLB_ETH_ADDR_LEN];
    uint8_t mask[GLB_ETH_ADDR_LEN];
    char desc[GLB_VOICE_VLAN_OUI_DESC];
    uint8_t enable;
} glb_voice_vlan_oui_t;

typedef struct glb_voice_vlan_s
{
    uint16_t vid;
    glb_voice_vlan_oui_t oui[GLB_VOICE_VLAN_OUI_NUM];
} glb_voice_vlan_t;


enum glb_l2_fdb_flush_type_e
{
    GLB_L2_FDB_FLUSH_ALL_FDB = 0,
    GLB_L2_FDB_FLUSH_BY_VID = 1,
    GLB_L2_FDB_FLUSH_BY_PORT = 2,
    GLB_L2_FDB_FLUSH_BY_PORT_AND_VID = 3,
    GLB_L2_FDB_FLUSH_ALL_FDB_TOTAL = 4,    /* flush all fdb one time*/
};
typedef enum glb_l2_fdb_flush_type_e glb_l2_fdb_flush_type_t;

struct glb_l2_get_index_by_mac_vid_req_s
{
    uint16_t    vid;
    uint8_t     mac_addr[GLB_ETH_ADDR_LEN];
};
typedef struct glb_l2_get_index_by_mac_vid_req_s glb_l2_get_index_by_mac_vid_req_t;

struct glb_l2_get_index_by_mac_vid_resp_s
{
    uint32_t    ifindex;
};
typedef struct glb_l2_get_index_by_mac_vid_resp_s glb_l2_get_index_by_mac_vid_resp_t;

#if 1
#define L2_VLAN_BMP_WORD_MAX  ((GLB_VLAN_MAX + GLB_BITS_NUM_OF_WORD) / GLB_BITS_NUM_OF_WORD)

typedef struct l2_vlan_bmp_s
{
  u_int32_t bitmap[L2_VLAN_BMP_WORD_MAX];
} l2_vlan_bmp_t;

#define L2_VLAN_BMP_INIT(bmp)                                             \
   do {                                                                    \
       memset ((bmp).bitmap, 0, sizeof ((bmp).bitmap));               \
   } while (0)

#define L2_VLAN_BMP_SET(bmp, vid)                                         \
   do {                                                                    \
        int _word = (vid) / GLB_BITS_NUM_OF_WORD;                       \
        (bmp).bitmap[_word] |= (1U << ((vid) % GLB_BITS_NUM_OF_WORD));  \
   } while (0)

#define L2_VLAN_BMP_UNSET(bmp, vid)                                       \
   do {                                                                    \
        int _word = (vid) / GLB_BITS_NUM_OF_WORD;                       \
        (bmp).bitmap[_word] &= ~(1U <<((vid) % GLB_BITS_NUM_OF_WORD));  \
   } while (0)

#define L2_VLAN_BMP_IS_MEMBER(bmp, vid)                                   \
  ((bmp).bitmap[(vid) / GLB_BITS_NUM_OF_WORD] & (1U << ((vid) % GLB_BITS_NUM_OF_WORD)))

#define L2_VLAN_SET_BMP_ITER_BEGIN(bmp, vid)                              \
    do {                                                                   \
        int _w, _i;                                                        \
        (vid) = 0;                                                         \
        for (_w = 0; _w < L2_VLAN_BMP_WORD_MAX; _w++)                     \
          for (_i = 0; _i < GLB_BITS_NUM_OF_WORD; _i++, (vid)++)        \
            if ((bmp).bitmap[_w] & (1U << _i))

#define L2_VLAN_SET_BMP_ITER_END(bmp, vid)                                \
    } while (0)
#endif

#endif /*__GLB_L2_DEFINE_H__*/
