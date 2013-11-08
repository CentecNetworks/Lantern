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

#ifndef __GLB_STM_DEFINE_H__
#define __GLB_STM_DEFINE_H__

/***************************************************************************************
*system reserved number define, GLB_SYS_RESV_xxxx
****************************************************************************************/
/*reserved number define*/
#define GLB_SYS_RESV_UCAST_FDB               53
#define GLB_SYS_RESV_HOST_ROUTE              0
#define GLB_SYS_RESV_REMOTE_ROUTE            40 /* 33 block + 3(martian addr)+1(default entry)+. */
#define GLB_SYS_RESV_MAC_BASED_VLAN_CLASS    1
#define GLB_SYS_RESV_IPV4_BASED_VLAN_CLASS   1
#define GLB_SYS_RESV_IPV4_SOURCE_GUARD       1
#define GLB_SYS_RESV_TCAM_GLB_PBR_TAIL        1
/*For humber system, 1 entry for global default entry, 64 for per port default entry*/
#define GLB_SYS_RESV_VLAN_MAPPING            65
#define GLB_SYS_RESV_DEFAULT_QOS_RULE        3 /*MAC permit any; ip permit any; ip deny tiny fragment*/
#define GLB_SYS_RESV_L2MC                    0
/* in ASIC, we reserve 2 entries,
one is default entry; the other is (*,G) because even if there is any  (*,G) entry,
we still need to reserve at least one entry for the algorithm purpose. except them, there may be
a few other applications which will use IPMC entry. e.g. VRRP. Considering those entries,
so we have to reserve more entries for them. Now it's 4.*/
#define GLB_SYS_RESV_L3MC                    4
#define GLB_SYS_RESV_HOST_ROUTE_V6           0
#define GLB_SYS_RESV_REMOTE_ROUTE_V6         135 /* 129 block + 2(martian addr) + 1(default entry)+. */
/*=========================FOR SRAM reserved===========================*/
#define GLB_SYS_RESV_SRAM_DSFWD              1536 /*1322=64 (ACL redirect)+544+714(port nh reserve)*/
#define GLB_SYS_RESV_SRAM_NH                 32 /*23=16(sdk reserve)+6 (loopback nh)+1(mpls tp nh)*/
#define GLB_SYS_RESV_SRAM_MET                64 /*48=1(VLAN 1 default entry)+47(local met for vlan 1 member port)*/
#define GLB_SYS_RESV_SRAM_L2EDIT             0
#define GLB_SYS_RESV_SRAM_L3EDIT             0
#define GLB_SYS_RESV_SRAM_MPLS               16 /* label 0 - label 15 are reserved label  */
#define GLB_NH_ID_BLOCK_SIZE 1024

/*==================for software database========================================*/
#define GLB_ARP_CACHE_SIZE   6144
#define GLB_NEIGH_CACHE_SIZE 1024

#define GLB_CFM_MAX_MEP_RMEP_SIZE  1024
/***************************************************************************************
*system soft profile maximum define, GLB_SPEC_PM_xxxx
****************************************************************************************/

/***************************************************************************************
*REAL HARDWARE INFORMATION DEFINE
****************************************************************************************/
#define MEM_CONFIG_HYBRID_MODE  "/mnt/flash/.hybrid_mode"

enum glb_stm_type_e
{
    GLB_STM_DEFAULT,
    GLB_STM_HYBRID
};
typedef enum glb_stm_type_e glb_stm_type_t;

enum glb_stm_key_type_e
{
    GLB_STM_ACL_IPV6_KEY          =0, /**<Acl ipv6 key*/
    GLB_STM_ACL_MAC_IPV4_KEY      =1, /**<Acl mac ipv4 mpls key*/
    GLB_STM_IPV4_UCAST_ROUTE_KEY  =2, /**<Ipv4 unicast route key*/
    GLB_STM_IPV4_MCAST_ROUTE_KEY  =3, /**<Ipv4 multicast route key*/
    GLB_STM_IPV4_NAT_KEY          =4, /**<Ipv4 nat key*/
    GLB_STM_IPV4_PBR_DUALDA_KEY   =5, /**<Ipv4 pbr key*/
    GLB_STM_IPV6_UCAST_ROUTE_KEY  =6,    /**<Ipv6 unicast route key*/
    GLB_STM_USER_ID_MAC_KEY       =7, /**<User id mac key*/
    GLB_STM_USER_ID_VLAN_KEY      =8, /**<User id vlan key*/
    GLB_STM_IPV6_MCAST_ROUTE_KEY  =9, /**<Ipv6 multicast route key*/
    GLB_STM_IPV6_NAT_KEY          =10, /**<Ipv6 nat key*/
    GLB_STM_MAC_KEY               =11, /**<Mac key*/
    GLB_STM_IPV6_PBR_DUALDA_KEY   =12, /**<Ipv6 pbr key*/
    GLB_STM_USER_ID_IPV4_KEY      =13, /**<User id ipv4 key*/
    GLB_STM_USER_ID_IPV6_KEY      =14, /**<User id ipv6 key*/
    GLB_STM_QOS_MAC_IPV4_KEY      =15, /**<Qos mac ipv4 mpls key*/
    GLB_STM_QOS_IPV6_KEY          =16, /**<Qos ipv6 key*/
    GLB_STM_OAM_KEY               =17, /**<Oam key*/
    GLB_STM_MAC_HASH_KEY          =18, /*MAC HASH KEY*/
    GLB_STM_IPV4_HASH_KEY         =19, /*IPv4 ucast HASH KEY*/
    GLB_STM_IPV6_HASH_KEY         =20, /*IPv6 ucast HASH KEY*/
    GLB_STM_TCAM_TABLE_MAX
};
typedef enum glb_stm_key_type_e glb_stm_key_type_t;

enum glb_stm_key_location_e
{
    GLB_STM_INT_TCAM, /**<Location in internal TCAM*/
    GLB_STM_EXT_TCAM, /**<Location in external TCAM*/
    GLB_STM_HASH, /**<Location in SRAM*/
    MAX_GLB_STM_KEY_LOCATION
};
typedef enum glb_stm_key_location_e glb_stm_key_location_t;

enum glb_stm_key_size_e
{
    GLB_STM_INVALID_KEY_SIZE = 0, /**<Invalid key size*/
    GLB_STM_KEY_SIZE_80_BIT = 1, /**<80 bits key size*/
    GLB_STM_KEY_SIZE_160_BIT = 2, /**<160 bits key size*/
    GLB_STM_KEY_SIZE_320_BIT = 4, /**<320 bits key size*/
    GLB_STM_KEY_SIZE_640_BIT = 8, /**<640 bits key size*/
    MAX_GLB_STM_KEY_SIZE
};
typedef enum glb_stm_key_size_e glb_stm_key_size_t;

enum glb_stm_flag_ext_sram_e
{
    GLB_EXT_SRAM_MPLS          = (1 << 0), /**< Mpls table store in external sram or not */
    GLB_EXT_SRAM_DSFWD_MET     = (1 << 1), /**dsfwd, glbmet, local met store*/
    GLB_EXT_SRAM_NH            = (1 << 2), /**glb_dsnh4w, glb_dsnh8w, local_nh4w, local_nh8w*/
    GLB_EXT_SRAM_DSL2EDIT      = (1 << 3), /**dsl2edit4w, dsl2edit8w*/
    GLB_EXT_SRAM_DSL3EDIT      = (1 << 4), /**dsl3edit4w, dsl3edit8w*/
    GLB_EXT_QDR_EN             = (1 << 5), /**<Enable external QDR*/
};
typedef enum glb_stm_flag_ext_sram_e glb_stm_flag_ext_sram_t;

enum glb_stm_flag_fea_ctrl_e
{
    GLB_CTRL_DUAL_ACL              = (1 << 0), /**< Enable lookup acl/qos key at the same time */
    GLB_CTRL_MERGE_ACLQOS_MAC_IP   = (1 << 1), /**< Enable merge acl/qos mac/ip Key */
    GLB_CTRL_URPF_EN               = (1 << 2), /**<Enable ipuc rpf*/
    GLB_CTRL_IPV4_EN               = (1 << 3),
    GLB_CTRL_IPV6_EN               = (1 << 4),
    GLB_CTRL_IPMCV4_EN             = (1 << 5),
    GLB_CTRL_IPMCV6_EN             = (1 << 6),
    GLB_CTRL_ETHOAM_EN             = (1 << 7),
    GLB_CTRL_MPLS_EN               = (1 << 8),
    GLB_CTRL_ACL_EN                = (1 << 9),
    GLB_CTRL_QOS_EN                = (1 << 10),
    GLB_CTRL_ACLV6_EN              = (1 << 11),
    GLB_CTRL_QOSV6_EN              = (1 << 12),
    GLB_CTRL_PBR_EN                = (1 << 13),
    GLB_CTRL_PBRV6_EN              = (1 << 14),
    GLB_CTRL_MAX,
};
typedef enum glb_stm_flag_fea_ctrl_e glb_stm_flag_fea_ctrl_t;

struct glb_stm_resv_fea_info_s
{
    uint32 mac_resv_for_vlan;
    uint32 mac_resv_for_vpls;
    uint32 ipuc_resv_for_tunnel;
};
typedef struct glb_stm_resv_fea_info_s glb_stm_resv_fea_info_t;

struct glb_stm_profile_key_info_s
{
    uint32 key_size;       /**< Value = {1,2,4,8}, indicates {80b,160b,320b,640b}. */
    uint32 max_key_index;  /**< Key total number. key_max_index * key_size = consumed 80b tcam entry. */
    glb_stm_key_location_t key_media; /**<CTC_ALLOC_XXX*/
    glb_stm_key_type_t           key_id; /**<Key type*/
};
typedef struct glb_stm_profile_key_info_s glb_stm_profile_key_info_t;

/*tcam profile info, the real hardware info should be stored in glb_humber_dev_t*/
struct glb_stm_profile_info_s
{
    glb_stm_profile_key_info_t* key_info; /**<Profile key information*/
    uint16 key_info_size;                /**< Size of key_info, multiple of sizeof(ctc_alloc_profile_key_info_t) */

    /* tcam/sram configuration */
    uint16 ext_flag;  /*glb_stm_flag_ext_sram_t*/
    uint32 ctrl_flag;  /*feature control flag, glb_stm_flag_fea_ctrl_t*/

    uint8  external_tcam_type;          /**< External TCAM hardware type, default NL9K */
    uint8  pad[3];

    uint32  dsfwd;           /*store DSFWD table's sram size*/
    uint32  gbl_nh4w;        /**< store global Nexthop_4W table's sram size */
    uint32  gbl_nh8w;        /**< store global Nexthop_8W table's sram size */
    uint32  local_nh4w;        /**< store local Nexthop_4W table's sram size */
    uint32  local_nh8w;        /**< store local Nexthop_8W table's sram size */
    uint32  gbl_met;        /**< store global MET table's sram size */
    uint32  local_met;        /**< store local MET table's sram size */
    uint32  dsl2edit_4w;     /**< store DSL2EDIT_4W table's sram size */
    uint32  dsl2edit_8w;     /**< store DSL2EDIT_8W table's sram size */
    uint32  dsl3edit_4w;     /**< store DSL3EDIT_4W table's sram size */
    uint32  dsl3edit_8w;     /**< store DSL3EDIT_8W table's sram size */

    uint32  mpls_entry_num;              /**< Record Mpls table entries defined in profile */

    /* hardware information is listed below */
    uint32 hash_physical_size;          /**< Optional hash size, 48K or 96K */
    uint32 ass_dyn_sram_size;           /**< Internal Sram for associate and dynamic table */

    uint32 external_tcam_size;          /**< External external tcam size, 256K */
    uint32 external_sram_size;         /**< External sram size */

    uint32 policer_tbl_size;            /**<store policer table's sram size*/
    uint32 fwd_stats_tbl_size;          /**<store forward statistics table's sram size*/

    glb_stm_resv_fea_info_t resv_info;
};
typedef struct glb_stm_profile_info_s glb_stm_profile_info_t;

/***************************************************************************************
*SYSTEM FEATUARE SPECIFICATION DEFINE
****************************************************************************************/
enum glb_stm_fea_list_e
{
    GLB_UCAST_FDB              = 0,
    GLB_VLAN_INSTANCE          = 1,
    GLB_MAC_FILTER             = 2,
    GLB_MAC_BASED_VLAN_CLASS   = 3,
    GLB_IPV4_BASED_VLAN_CLASS  = 4,
    GLB_IPV4_SOURCE_GUARD      = 5,
    GLB_VLAN_MAPPING           = 6,
    GLB_REMOTE_ROUTEV4         = 7,
    GLB_HOST_ROUTEV4           = 8,
    GLB_ECMP_ROUTEV4           = 9,
    GLB_PBRV4                  = 10,
    GLB_L2MC                   = 11,
    GLB_L3MC                   = 12,
    GLB_VOICE_VLAN             = 13,
    GLB_FLOW_ENTRY             = 14,
    GLB_CFM_CHANNEL            = 15,
    GLB_CFM_LCK                = 16,
    GLB_G8031_GROUPS           = 17,
    GLB_G8032_RINGS            = 18,
    GLB_FTN                    = 19,
    GLB_ILM                    = 20,
    GLB_VPWS                   = 21,
    GLB_LSP_PE                 = 22,
    GLB_LSP_P                  = 23,
    GLB_VPLS_PEER              = 24,
    GLB_VPLS_AC                = 25,
    GLB_VSI                    = 26,
    GLB_LSP_OAM                = 27,
    GLB_PW_OAM                 = 28,
    GLB_MPLS_APS_TUNNEL        = 29,
    GLB_IPV6_BASED_VLAN_CLASS  = 30,
    GLB_IPV6_SOURCE_GUARD      = 31,
    GLB_REMOTE_ROUTEV6         = 32,
    GLB_HOST_ROUTEV6           = 33,
    GLB_ECMP_ROUTEV6           = 34,
    GLB_PBRV6                  = 35,
    GLB_L3MC_V6                = 36,
    GLB_FLOW_ENTRY_V6          = 37,
    GLB_IP_TUNNEL              = 38,
    GLB_FEA_LIST_MAX,
};
typedef enum glb_stm_fea_list_e glb_stm_fea_list_t;

enum glb_stm_assit_list_e
{
    GLB_MEM_PORT_VLAN    = 0,
    GLB_MEM_PORT_L2MC    = 1,
    GLB_MEM_PORT_L3MC    = 2,
    GLB_MEM_PORT_L3MC_v6 = 3,
    GLB_MEM_PORT_G8032   = 4,
    GLB_STM_ASSIT_LIST_MAX,
};
typedef enum glb_stm_assit_list_e glb_stm_assit_list_t;

typedef enum glb_stm_asic_spec_e
{
    GLB_STM_ASIC_SPEC_DS_MPLS = 0,
    GLB_STM_ASIC_SPEC_MAX,
}glb_stm_asic_spec_t;

struct glb_system_fea_spec_s {
    /*layer2 features*/
    uint32 ucast_fdb;
    uint16 vlan_instance;
    uint16 mac_filter;
    uint16 mac_based_vlan_class;
    uint16 ipv4_based_vlan_class;
    uint16 ipv4_source_guard;
    uint16 vlan_mapping_entry_applied_port;

    /*ipv4 ucast*/
    uint32 remote_routes;
    uint16 host_routes;
    uint16 ecmp_routes;

    /*ipv4 mcast*/
    uint16 pbr_entries;
    uint16 l2mc_entries;
    uint16 l3mc_entries;
    uint8  l2mc_member_ports;
    uint8  l3mc_member_ports;

    /*layer2 features*/
    uint8 vlan_member_ports;
    uint8 voice_vlan;

    /*acl and qos*/
    uint16 flow_entries;

    /*cfm and g8031, g8032*/
    uint16 cfm_local_and_remote_meps;
    uint16 g8031_groups;
    uint16 g8032_rings;
    uint16 g8032_member_ports_per_ring;

    /*mpls*/
    uint16 ftn_entries;
    uint16 ilm_entries;

    /*l2vpn*/
    uint16 vpws;
    uint16 lsp_pe;
    uint16 lsp_p;
    uint16 vpls_peer; /*vpls_pw*/
    uint16 vpls_ac;
    uint16 vsi;
    uint16 lsp_oam;
    uint16 pw_oam; /*lsp_pw*/
    uint16 mpls_aps_tunnel; /*map-route&SPME*/
    uint16 cfm_lck;

    /*ipv6 & nat*/
    uint16 ip_tunnel; /*v6 in v4, ip tunnel shares the key space with ipuc remote key*/
    uint16 host_v6_routes;
    uint16 ecmp_v6_routes;
    uint16 pbr_v6_entries;
    uint16 l3mc_v6_entries;
    uint16 flow_v6_entries;
    uint16 ipv6_based_vlan_class;
    uint16 ipv6_source_guard;
    uint16 ipv6_nat;
    uint16 ipv4_nat;
    uint32 remote_v6_routes;
    uint8  l3mc_v6_member_ports;
    uint8  vlan_mapping_default_entry;
    uint8  pad[2];
};
typedef struct glb_system_fea_spec_s glb_system_fea_spec_t;

/*used for chsm and hsrv*/
struct glb_stm_resouce_info_s
{
    uint32 ctrl_flag;  /*feature control flag, glb_stm_flag_fea_ctrl_t*/

    uint32 glb_nh4w;
    uint32 glb_nh8w;
    uint32 glb_met;

    uint32 ucast_mac; /*ucast fdb*/
    uint32 mcast_mac; /*mcast fdb*/
    uint32 mpls; /*ilm and vpls , vpws*/
    uint32 vlan; /*vlan instance*/
    uint16 max_v4ecmp_groups;
    uint16 max_v6ecmp_groups;
};
typedef struct glb_stm_resouce_info_s glb_stm_resouce_info_t;

/* SDK init need stm profile info and datapath info. */
struct glb_sdk_init_info_s
{
    void* stm_profile_info; /*stm profile info, glb_stm_profile_info_t*/
    void* datapath_config_file;  /*datapath config file name*/
    uint8 ptp_quanta;
#ifdef FOAM_SUPPORT
    uint8 is_foam_supported;
#endif /*FOAM_SUPPORT*/
};
typedef struct glb_sdk_init_info_s glb_sdk_init_info_t;

#endif /*!__GLB_STM_DEFINE_H__*/
