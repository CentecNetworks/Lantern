/**
 @file ctc_parser.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-15

 @version v2.0

This file contains all parser related data structure, enum, macro and proto.
*/

#ifndef CTC_PARSER_H_
#define CTC_PARSER_H_

/**
 @defgroup parser PARSER
 @{
*/

/**
 @defgroup parser_l2 PARSER_L2
 @{
*/

/**
 @brief  the packet layer2 type
*/
enum ctc_parser_pkt_type_e
{
    CTC_PARSER_PKT_TYPE_ETHERNET,    /**<0: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_IP_OR_IPV6,  /**<1: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_MPLS,        /**<2: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_IPV6,        /**<3: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_MCAST_MPLS,  /**<4: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_CESOETH,     /**<5: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_FLEXIBLE,    /**<6: parser packet type,it's config in system intialization*/
    CTC_PARSER_PKT_TYPE_RESERVED,    /**<7: parser packet type,it's config in system intialization*/

    MAX_CTC_PARSER_PKT_TYPE
};
typedef enum ctc_parser_pkt_type_e ctc_parser_pkt_type_t;

/**
 @brief  the packet layer2 type
*/
enum ctc_parser_l2_type_e
{
    CTC_PARSER_L2_TYPE_NONE,                    /**<0: parser layer2 type,it's config in system intialization*/
    CTC_PARSER_L2_TYPE_ETH_V2,                  /**<1: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_ETH_SAP,                 /**<2: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_ETH_SNAP,                /**<3: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_PPP_2B,                  /**<4: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_PPP_1B,                  /**<5: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_RAW_SNAP,                /**<6: parser layer2 type,it's config in system initialization*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE0,        /**<7: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE1,        /**<8: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE2,        /**<9: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE3,        /**<10: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE4,        /**<11: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE5,        /**<12: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE6,        /**<13: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE7,        /**<14: parser layer2 type, it can be configed by user*/
    CTC_PARSER_L2_TYPE_RSV_USER_DEFINE_FLEXL2,  /**<15: parser layer2 type,it's config in system initialization*/
    MAX_CTC_PARSER_L2_TYPE
};
typedef enum ctc_parser_l2_type_e ctc_parser_l2_type_t;

/**
 @brief  the packet layer3 type
*/
enum ctc_parser_l3_type_e
{
    CTC_PARSER_L3_TYPE_NONE,             /**<0: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_IP,               /**<1: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_IPV4,             /**<2: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_IPV6,             /**<3: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_MPLS,             /**<4: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_MPLS_MCAST,       /**<5: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_ARP,              /**<6: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_RARP,             /**<7: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_EAPOL,            /**<8: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_ETHER_OAM,        /**<9: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_SLOW_PROTO,       /**<10: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_CMAC,             /**<11: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_PTP,              /**<12: parser layer3 type,it's config in system intialization*/
    CTC_PARSER_L3_TYPE_RSV_USER_DEFINE0, /**<13: parser layer3 type, it can be configed by user*/
    CTC_PARSER_L3_TYPE_RSV_USER_DEFINE1, /**<14: parser layer3 type, it can be configed by user*/
    CTC_PARSER_L3_TYPE_RSV_USER_FLEXL3,  /**<15: parser layer3 type,it's config in system intialization*/
    MAX_CTC_PARSER_L3_TYPE,
    CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_BASE = CTC_PARSER_L3_TYPE_ARP,
    CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_MAX  = CTC_PARSER_L3_TYPE_RSV_USER_FLEXL3
};
typedef enum ctc_parser_l3_type_e ctc_parser_l3_type_t;


/**
 @brief  the packet layer4 type
*/
enum ctc_parser_l4_type_e
{
    CTC_PARSER_L4_TYPE_NONE,             /**<0: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_TCP,              /**<1: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_UDP,              /**<2: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_GRE,              /**<3: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_IPINIP,           /**<4: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_V6INIP,           /**<5: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_ICMP,             /**<6: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_IGMP,             /**<7: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE0, /**<8: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE1, /**<9: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE2, /**<10: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE3, /**<11: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE4, /**<12: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE5, /**<13: parser layer4 type, it can be configed by user*/
    CTC_PARSER_L4_TYPE_RSV_USER_DEFINE6, /**<14: parser layer4 type,it's config in system intialization*/
    CTC_PARSER_L4_TYPE_ANY_PROTO,        /**<15: parser layer4 type,it's config in system intialization*/
    MAX_CTC_PARSER_L4_TYPE
};
typedef enum ctc_parser_l4_type_e  ctc_parser_l4_type_t;

/**
 @brief  packet type
*/
enum packet_type_e
{
    PKT_TYPE_ETH,           /**<pkt type map type0*/
    PKT_TYPE_IPV4,          /**<pkt type map type1*/
    PKT_TYPE_MPLS,          /**<pkt type map type2*/
    PKT_TYPE_IPV6,          /**<pkt type map type3*/
    PKT_TYPE_RESERVED = 7   /**<pkt type maptype7*/
};

/**
 @brief  ethernet hash field enum
*/
enum ctc_parser_l2_hash_field_e
{
    CTC_PARSER_L2_HASH_FIELD_VLAN_HASH          = 0x1,      /**<used to compute hash value with vlan*/
    CTC_PARSER_L2_HASH_FIELD_COS_HASH           = 0x2,      /**<used to compute hash value with cos*/
    CTC_PARSER_L2_HASH_FIELD_L2_HDR_PRL_HASH    = 0x4,      /**<used to compute hash value with layer2 header protocol*/
    CTC_PARSER_L2_HASH_FIELD_MOD_HASH           = 0x8,      /**<used to compute hash value with mod*/
    CTC_PARSER_L2_HASH_FIELD_PORT_HASH          = 0x10,     /**<used to compute hash value with port*/
    CTC_PARSER_L2_HASH_FIELD_VLAN_HASH_MODE     = 0x100,    /**<used to set vlan hash mode*/
    CTC_PARSER_L2_HASH_FIELD_COS_ECMP_HASH      = 0x400,    /**<used cos value to compute ecmp hash */
    CTC_PARSER_L2_HASH_FIELD_ECMP_MACSA_HASH    = 0x800,    /**<used macsa to compute ecmp hash*/
    CTC_PARSER_L2_HASH_FIELD_ECMP_MACDA_HASH    = 0x1000,   /**<used macda to compute ecmp hash*/
    CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACSA_HASH = 0x2000,   /**<used macsa to compute linkagg hash*/
    CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACDA_HASH = 0x4000,   /**<used macda to compute linkagg hash*/
    CTC_PARSER_L2_HASH_FIELD_ISID_HASH          = 0x8000,   /**<used isid to compute hash*/
    CTC_PARSER_L2_HASH_FIELD_IPCP_HASH          = 0x10000   /**<used ipcp to compute hash*/
};
typedef enum ctc_parser_l2_hash_field_e ctc_parser_l2_hash_field_t;

/**
 @brief  l2 parser ctl filed
*/
struct ctc_parser_l2_ctl_fld_s
{
    uint32 flags;                       /**<layer2 ctl flags,which field care about*/

    uint8 vlan_hash;                    /**<vlan use to compute hash*/
    uint8 cos_hash;                     /**<cos use to compute hash*/
    uint8 l2hdr_ptl_hash;               /**<layer2 header protocol use to compute hash*/
    uint8 mod_hash;                     /**<mod use to compute hash*/

    uint8 port_hash;                    /**<port use to compute hash*/
    uint8 vlan_hash_mode;               /**<outer-vlan or outer-vlan+inner-vlan use to compute hash*/
    uint8 cos_ecmp_hash;                /**<cos use to compute ecmp hash*/
    uint8 ecmp_macsa_hash;              /**<macsa use to compute ecmp hash*/

    uint8 ecmp_macda_hash;              /**<macda use to compute ecmp hash*/
    uint8 linkagg_macsa_hash;           /**<macsa use to compute linkagg hash*/
    uint8 linkagg_macda_hash;           /**<macda use to compute linkagg hash*/
    uint8 isid_hash;                    /**<isid use to compute hash*/

    uint8 ipcp_hash;                    /**<ipcp in itag use to compute hash*/
};
typedef struct ctc_parser_l2_ctl_fld_s ctc_parser_l2_ctl_fld_t;

/**
 @brief  tpid enum
*/
enum ctc_parser_l2_tpid_e
{
    CTC_PARSER_L2_TPID_CVLAN_TPID,      /**<cvlan tpid*/
    CTC_PARSER_L2_TPID_ITAG_TPID,       /**<itag tpid*/
    CTC_PARSER_L2_TPID_BLVAN_TPID,      /**<bvlan tpid*/
    CTC_PARSER_L2_TPID_SVLAN_TPID_0,    /**<svlan tpid0*/
    CTC_PARSER_L2_TPID_SVLAN_TPID_1,    /**<svlan tpid1*/
    CTC_PARSER_L2_TPID_SVLAN_TPID_2,    /**<svlan tpid2*/
    CTC_PARSER_L2_TPID_SVLAN_TPID_3     /**<svlan tpid3*/
};
typedef enum ctc_parser_l2_tpid_e ctc_parser_l2_tpid_t;

/**
 @brief  pbb ctl flags
*/
enum ctc_parser_pbb_flags_e
{
    CTC_PARSER_PBB_FLAGS_COS_ECMP_HASH             = 0x1,   /**<cos use to compute ecmp hash flags*/
    CTC_PARSER_PBB_FLAGS_VLAN_HASH                 = 0x2,   /**<vlan use to compute hash flags*/
    CTC_PARSER_PBB_FLAGS_COS_HASH                  = 0x4,   /**<cos use to compute hash flags*/
    CTC_PARSER_PBB_FLAGS_CMAC_HASH_ECMP_MACSA      = 0x8,   /**<macsa use to compute cmac ecmp hash flags*/
    CTC_PARSER_PBB_FLAGS_CMAC_HASH_ECMP_MACDA      = 0x10,  /**<macda use to compute cmac ecmp hash flags*/
    CTC_PARSER_PBB_FLAGS_CMAC_HASH_LINKAGG_MACSA   = 0x20,  /**<macsa use to compute cmac linkagg hash flags*/
    CTC_PARSER_PBB_FLAGS_CMAC_HASH_LINKAGG_MACDA   = 0x40,  /**<macda use to compute cmac linkagg hash flags*/
    CTC_PARSER_PBB_FLAGS_NCA_VAL                   = 0x80,  /**<nca set flags*/
    CTC_PARSER_PBB_FLAGS_OUTER_VLAN_IS_CVLAN       = 0x100, /**<outer vlan is cvlan set flags*/
    CTC_PARSER_PBB_FLAGS_VLAN_PAS_NUM              = 0x200, /**<vlan parsing num set flags*/
    CTC_PARSER_PBB_FLAGS_PBB_OAM_ETHER_TYPE_OFFSET = 0x400, /**<pbb oam ethernet type offset set flags*/
    CTC_PARSER_PBB_FLAGS_PBB_OAM_ETHER_TYPE        = 0x800  /**<pbb oam ethernet type set flags*/
};
typedef enum ctc_parser_pbb_flags_e ctc_parser_pbb_flags_t;

/**
 @brief  pbb ctl field
*/
struct ctc_parser_pbb_ctl_s
{
    uint32 flags;                   /**<CTC_PARSER_PBB_FLAGS_XXX*/

    uint8 cos_ecmp_hash;            /**<cos use to compute ecmp hash */
    uint8 vlan_hash;                /**<vlan use to compute hash*/
    uint8 cos_hash;                 /**<cos use to compute hash*/
    uint8 ecmp_macsa_hash;          /**<macsa use to compute ecmp hash*/

    uint8 ecmp_macda_hash;          /**<macda use to compute ecmp hash*/
    uint8 linkagg_macsa_hash;       /**<macsa use to compute linkagg hash*/
    uint8 linkagg_macda_hash;       /**<macda use to compte linkagg hash*/

    uint8 nca_value_en;             /**<nca value enable set*/
    uint8 outer_vlan_is_cvlan;      /**<outer vlan is cvlan */
    uint8 vlan_parsing_num;         /**<vlan parsing num */
    uint8 pbb_oam_ether_type_offset;/**<pbb oam ethernet type offset*/

    uint16 pbb_oam_ether_type;      /**<pbb oam ethertype */
};
typedef struct ctc_parser_pbb_ctl_s ctc_parser_pbb_ctl_t;

/**
 @brief  layer2 flex ctl flags
*/
enum ctc_parser_l2flex_flags_e
{
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL0       = 0x1,      /**<byte select0 flags*/
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL1       = 0x2,      /**<byte select1 flags*/
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL2       = 0x4,      /**<byte select2 flags*/
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL3       = 0x8,      /**<byte select3 flags*/
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL4       = 0x10,     /**<byte select4 flags*/
    CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL5       = 0x20,     /**<byte select5 flags*/
    CTC_PARSER_L2FLEX_FLAGS_PTL_BYTE_SEL0   = 0x40,     /**<protocol byte select0 flags*/
    CTC_PARSER_L2FLEX_FLAGS_PTL_BYTE_SEL1   = 0x80,     /**<protocol byte select1 flags*/
    CTC_PARSER_L2FLEX_FLAGS_MIN_LEN         = 0x100,    /**<min length flags*/
    CTC_PARSER_L2FLEX_FLAGS_L2_BASIC_OFFSET = 0x200     /**<layer2 basic offset flags*/
};
typedef enum ctc_parser_l2flex_flags_e ctc_parser_l2flex_flags_t;

/**
 @brief  layer2 flex ctl filed
*/
struct ctc_parser_l2flex_ctl_s
{
    uint32 flags;           /**<CTC_PARSER_L2FLEX_FLAGS_XXX*/

    uint8 byte_sel;         /**<byte selection*/
    uint8 ptl_byte_sel;     /**<protocol byte selection*/

    uint8 min_length;       /**<min length*/
    uint8 l2_basic_offset;  /**<layer2 basic offset*/
};
typedef struct ctc_parser_l2flex_ctl_s ctc_parser_l2flex_ctl_t;

/**
 @brief  layer2 protocol entry fields
*/
struct ctc_parser_l2_ptl_entry_s
{
    uint32 mask;            /**<mask for layer2 header protocol,layer2 type,isEth,isPPP,isSAP*/

    uint8 isEth;            /**<is ethernet packet*/
    uint8 isPPP;            /**<is ppp packet*/
    uint8 isSAP;            /**<is sap packet*/
    uint8 l2_type;          /**<layer2 type,CTC_PARSER_L2_TYPE_XXX*/

    uint16 l2hdr_ptl;       /**<layer2 header protocol*/
    uint8 l3_type;          /**<layer3 type, CTC_PARSER_L3_TYPE_XXX*/
    uint8 addition_offset;  /**<addition offset for layer3 info parser*/
};
typedef struct ctc_parser_l2_ptl_entry_s ctc_parser_l2_ptl_entry_t;
/**@} end of @defgroup  parser_l2 PARSER_L2 */

/**
 @defgroup parser_l3 PARSER_L3
 @{
*/

/**
 @brief  ip hash flags ctl enum
*/
enum ctc_parser_ip_hash_flags_e
{
    CTC_PARSER_IP_HASH_FLAGS_USE_IP_HASH          = 0x1,    /**<use ip hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_PROTOCOL_ECMP_HASH   = 0x2,    /**<use protocol num to compute ecmp hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_DSCP_ECMP_HASH       = 0x4,    /**<use dscp to compute ecmp hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_PROTOCOL_HASH        = 0x8,    /**<use protocol to compute hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_FLOW_LABEL_ECMP_HASH = 0x10,   /**<use flow label to compute ecmp hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_ECMP_IPSA_HASH       = 0x20,   /**<use ipsa to compute ecmp hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_ECMP_IPDA_HASH       = 0x40,   /**<use ipda to compute ecmp hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPSA_HASH    = 0x80,   /**<use ipsa to compute linkagg hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPDA_HASH    = 0x100,  /**<use ipda to compute linkagg hash flags*/
    CTC_PARSER_IP_HASH_FLAGS_SMALL_FRAG_OFFSET    = 0x200   /**<fragment offset flags*/
};
typedef enum ctc_parser_ip_hash_flags_e ctc_parser_ip_hash_flags_t;

/**
 @brief  ip hash ctl fileds
*/
struct ctc_parser_ip_hash_ctl_s
{
    uint32 flags;                 /**< CTC_PARSER_IP_HASH_FLAGS_XXX*/

    uint8 use_ip_hash;            /**<use ip hash*/
    uint8 ptl_ecmp_hash;          /**<use protocol to compute ecmp hash*/
    uint8 dscp_ecmp_hash;         /**<use dscp to compute ecmp hash*/
    uint8 ptl_hash;               /**<use protocol to compute hash*/

    uint8 flow_label_ecmp_hash;   /**<use flow label to compute ecmp hash*/
    uint8 ecmp_ipsa_hash;         /**<use ipsa to compute ecmp hash*/
    uint8 ecmp_ipda_hash;         /**<use ipda to compute ecmp hash*/
    uint8 linkagg_ipsa_hash;      /**<use ipsa to compute linkagg hash*/

    uint8 linkagg_ipda_hash;      /**<use ipda to compute linkagg hash*/
    uint8 small_frag_offset;      /**<fragment offset*/
};
typedef struct ctc_parser_ip_hash_ctl_s ctc_parser_ip_hash_ctl_t;

/**
 @brief  ipv6 ctl flags
*/
enum ctc_parser_ipv6_ctl_flags_e
{
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL0 = 0x1,     /**<ipv6 extend header level0 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL1 = 0x2,     /**<ipv6 extend header level1 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL2 = 0x4,     /**<ipv6 extend header level2 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL3 = 0x8,     /**<ipv6 extend header level3 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL4 = 0x10,    /**<ipv6 extend header level4 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL5 = 0x20,    /**<ipv6 extend header level5 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL6 = 0x40,    /**<ipv6 extend header level6 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL7 = 0x80,    /**<ipv6 extend header level7 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_EXT_DIS        = 0x100,   /**<ipv6 extend header disable flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_ERR_OP_EN      = 0x200,   /**<error operation enable*/
    CTC_PARSER_IPV6_CTL_FLAGS_RSV_L3_HDR_PTL = 0x400,   /**<reserved layer3 header protocol*/
    CTC_PARSER_IPV6_CTL_FLAGS_FLEX_EXT_HDR0  = 0x800,   /**<flexiable extend header0 flags*/
    CTC_PARSER_IPV6_CTL_FLAGS_FLEX_EXT_HDR1  = 0x1000   /**<flexiable extend header1 flags*/
};
typedef enum ctc_parser_ipv6_ctl_flags_e ctc_parser_ipv6_ctl_flags_t;


/**
 @brief  ipv6 ctl fields
*/
struct ctc_parser_ipv6_ctl_s
{
    struct
    {
        uint8 ext_level_chk_en; /**<extend header level check enable*/
        uint8 set_ip_options;   /**<set ip option*/
        uint8 ext_shift;        /**<extend header shift*/
        uint8 ext_level;        /**<extend level*/
    } ext_hdr_level;

    uint32 flags;               /**<CTC_PARSER_IPV6_CTL_FLAGS_XXX*/

    uint8 ipv6_ext_dis;         /**<ipv6 extend header disable*/
    uint8 ipv6_err_option_en;   /**<ipv6 error option enable*/
    uint8 rsv_l3_hdr_ptl;       /**<reserved layer3 header protocol*/
    uint8 flex_ext_hdr1;        /**<flex extend header1*/

    uint8 flex_ext_hdr0;        /**<flex extend header0*/
};
typedef struct ctc_parser_ipv6_ctl_s ctc_parser_ipv6_ctl_t;

/**
 @brief  mpls ctl flags
*/
enum ctc_parser_mpls_ctl_e
{
    CTC_PARSER_MPLS_CTL_USE_MPLS_HASH        = 0x1,     /**<use mpls hash flags*/
    CTC_PARSER_MPLS_CTL_PTL_ECMP_HASH        = 0x2,     /**<use protocol to compute ecmp hash flags*/
    CTC_PARSER_MPLS_CTL_DSCP_ECMP_HASH       = 0x4,     /**<use dscp to compute ecmp hash flags*/
    CTC_PARSER_MPLS_CTL_PTL_HASH             = 0x8,     /**<use protocol to compute hash flags*/
    CTC_PARSER_MPLS_CTL_FLOW_LABEL_ECMP_HASH = 0x10,    /**<use flow label to compute ecmp hash*/
    CTC_PARSER_MPLS_CTL_ECMP_IPSA_HASH       = 0x20,    /**<use ipsa to compute ecmp hash flags*/
    CTC_PARSER_MPLS_CTL_ECMP_IPDA_HASH       = 0x40,    /**<use ipda to compute ecmp hash flags*/
    CTC_PARSER_MPLS_CTL_LINKAGG_IPSA_HASH    = 0x80,    /**<use ipsa to compte linkagg hash flags*/
    CTC_PARSER_MPLS_CTL_LINKAGG_IPDA_HASH    = 0x100    /**<use ipda to compute linkagg hash flags*/
};
typedef enum ctc_parser_mpls_ctl_e  ctc_parser_mpls_ctl_t;

/**
 @brief  mpls ctl field
*/
struct ctc_parser_mpls_ctl_fld_s
{
    uint32 flags;               /**<CTC_PARSER_MPLS_CTL_XXX*/

    uint8 use_mpls_hash;        /**<use mpls hash*/
    uint8 ptl_ecmp_hash;        /**<use protocol to compute ecmp hash*/
    uint8 dscp_ecmp_hash;       /**<use dscp to compute ecmp hash*/
    uint8 ptl_hash;             /**<use protocol to compute hash*/

    uint8 flow_label_ecmp_hash; /**<use flow label to compute ecmp hash*/
    uint8 ecmp_ipsa_hash;       /**<use ipsa to compute ecmp hash*/
    uint8 ecmp_ipda_hash;       /**<use ipda to compute ecmp hash*/
    uint8 linkagg_ipsa_hash;    /**<use ipsa to compute linkagg hash*/

    uint8 linkagg_ipda_hash;    /**<use ipda to compute linkagg hash*/
};
typedef struct ctc_parser_mpls_ctl_fld_s ctc_parser_mpls_ctl_fld_t;

/**
 @brief  layer3 flex ctl flags
*/
enum ctc_parser_l3flex_flags_e
{
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL0       = 0x1,      /**<byte select0 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL1       = 0x2,      /**<byte select1 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL2       = 0x4,      /**<byte select2 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL3       = 0x8,      /**<byte select3 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL4       = 0x10,     /**<byte select4 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL5       = 0x20,     /**<byte select5 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL6       = 0x40,     /**<byte select6 flags*/
    CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL7       = 0x80,     /**<byte select7 flags*/
    CTC_PARSER_L3FLEX_FLAGS_L3MIN_LEN       = 0x100,    /**<layer3 min length flags*/
    CTC_PARSER_L3FLEX_FLAGS_PTL_BYTE_SEL    = 0x200,    /**<protocol byte select flags*/
    CTC_PARSER_L3FLEX_FLAGS_L3_BASIC_OFFSET = 0x400     /**<layer3 basic offset flags*/
};
typedef enum ctc_parser_l3flex_flags_e ctc_parser_l3flex_flags_t;

/**
 @brief  layer3 flex ctl fields
*/
struct ctc_parser_l3flex_ctl_s
{
    uint32 flags;           /**<CTC_PARSER_L3FLEX_FLAGS_XXX*/
    uint8 byte_sel;         /**<byte select*/

    uint8 l3min_length;     /**<layer3 min length*/
    uint8 ptl_byte_sel;     /**<protocol byte selection*/
    uint8 l3_basic_offset;  /**<layer3 basic offset*/
};
typedef struct ctc_parser_l3flex_ctl_s ctc_parser_l3flex_ctl_t;

/**
 @brief  layer3 protocol entry fields
*/
struct ctc_parser_l3_ptl_entry_s
{
    uint8 l4_type;          /**<layer4 type,CTC_PARSER_L4_TYPE_XXX*/
    uint8 addition_offset;  /**<additional offset for layer4 basic offset*/
    uint8 l3_type_mask;     /**<layer3 type mask*/
    uint8 l3_type;          /**<layer3 type, CTC_PARSER_L3_TYPE_XXX*/

    uint8 l3_hdr_ptl;       /**<layer3 header protocol*/
    uint8 l3_hdr_ptl_mask;  /**<layer3 header protocol mask*/
};
typedef struct ctc_parser_l3_ptl_entry_s ctc_parser_l3_ptl_entry_t;

/**@} end of @defgroup  parser_l3 PARSER_L3*/


/**
 @defgroup parser_l4 PARSER_L4
 @{
*/

/**
 @brief  layer4 hash ctl fields
*/
union ctc_parser_layer4_parser_ctl_u
{

    struct
    {
        uint32 field_flags;         /**<CTC_PARSER_L4_FLAGS_XXX*/
        uint8 use_l4hash;           /**<use layer4 hash*/
        uint8 src_port_hash;        /**<use source port to compute hash*/
        uint8 dst_port_hash;        /**<use dest port to compute hash*/
        uint8 src_port_ecmp_hash;   /**<use source port to compute ecmp hash*/

        uint8 dst_port_ecmp_hash;   /**<use dest port to compute ecmp hash*/
    }l4hash_ctl;

    struct
    {
        uint32 field_flags;         /**<CTC_PARSER_L4_FLAGS_XXX*/
        uint8 index;                /**<udp application entry index*/
        uint16 udp_app_value;       /**<udp application value*/
        uint16 udp_app_mask;        /**<udp application mask*/
    }udp_app_op_ctl;

    struct
    {
        uint32 l4len_op_flags;      /**<CTC_PARSER_L4_FLAGS_XXX*/
        uint16 length;              /**<length value*/
        uint8 index;                /**<layer4 length option index*/
    }l4len_op_ctl;

    struct
    {
        uint32 field_flags;         /**<CTC_PARSER_L4_FLAGS_XXX*/
        uint8 byte_select0;         /**<byte selection0 */
        uint8 byte_select1;         /**<byte selection1*/
        uint8 l4_app_min_len;       /**<layer4 application min length*/
        uint8 l4_min_len;           /**<layer4 min length*/
    }l4flex_ctl;


    struct
    {
        uint32 field_flags;         /**<CTC_PARSER_L4_FLAGS_XXX*/
        uint16 ptp_port0;           /**<ptp port0*/
        uint16 ptp_port1;           /**<ptp port1*/

        uint8 ptp_en;               /**<ptp enable*/
    }l4ptp_ctl;

};

typedef union ctc_parser_layer4_parser_ctl_u ctc_parser_layer4_parser_ctl_t;

/**
 @brief  layer4 flag option ctl fields
*/
struct ctc_parser_l4flag_op_ctl_s
{
    uint32 field_flags; /**<CTC_PARSER_L4_FLAGS_XXX*/
    uint8 op_and_or;    /**<operation and or*/
    uint8 flags_mask;   /**<flags mask*/
};
typedef struct ctc_parser_l4flag_op_ctl_s ctc_parser_l4flag_op_ctl_t;

/**
 @brief  layer4 port option select ctl fields
*/
struct ctc_parser_l4_port_op_sel_s
{
    uint32 field_flags; /**<CTC_PARSER_L4_FLAGS_XXX*/
    uint8 op_dest_port; /**<operation dest port*/
};
typedef struct ctc_parser_l4_port_op_sel_s ctc_parser_l4_port_op_sel_t;

/**
 @brief  layer4 port option ctl fields
*/
struct ctc_parser_l4_port_op_ctl_s
{
    uint32 field_flags;     /**<CTC_PARSER_L4_FLAGS_XXX*/
    uint16 layer4_port_max; /**<layer4 max port*/
    uint16 layer4_port_min; /**<layer4 min port*/
};
typedef struct ctc_parser_l4_port_op_ctl_s ctc_parser_l4_port_op_ctl_t;

/**
 @brief  select to config layer4 table
*/
enum ctc_parser_layer4_tbl_flags_e
{
    CTC_PARSER_LAYER4_TBL_FLAGS_L4HASH         = 0x1,   /**<select layer4 hash table flags*/
    CTC_PARSER_LAYER4_TBL_FLAGS_UDP_APP_OP_CTL = 0x2,   /**<select udp application option ctl table flags*/
    CTC_PARSER_LAYER4_TBL_FLAGS_L4LEN_OP_CTL   = 0x4,   /**<select layer4 length option ctl table flags*/
    CTC_PARSER_LAYER4_TBL_FLAGS_L4FLEX_CTL     = 0x8,   /**<select layer4 flex ctl table flags*/
    CTC_PARSER_LAYER4_TBL_FLAGS_L4PTP_CTL      = 0x10   /**<select layer4 ptp ctl table flags*/
};
typedef enum ctc_parser_layer4_tbl_flags_e ctc_parser_layer4_tbl_flags_t;


/**
 @brief  layer4 ctl flags
*/
enum ctc_parser_layer4_flags_e
{
    CTC_PARSER_L4_FLAGS_USE_L4HASH         = 0x1,           /**<use layer4 hash flags*/
    CTC_PARSER_L4_FLAGS_SRC_PORT_HASH      = 0x2,           /**<use source port hash flags*/
    CTC_PARSER_L4_FLAGS_DST_PORT_HASH      = 0x4,           /**<use dest port to compute hash flags*/
    CTC_PARSER_L4_FLAGS_SRC_PORT_ECMP_HASH = 0x10,          /**<use source port to compute ecmp hash flags*/
    CTC_PARSER_L4_FLAGS_DST_PORT_ECMP_HASH = 0x20,          /**<use dest port to compute ecmp hash flags*/

    CTC_PARSER_L4_FLAGS_FLAG_OP_CTL0 = 0x40,                /**<layer4 flag option ctl0 flags*/
    CTC_PARSER_L4_FLAGS_FLAG_OP_CTL1 = 0x80,                /**<layer4 flag option ctl1 flags*/
    CTC_PARSER_L4_FLAGS_FLAG_OP_CTL2 = 0x100,               /**<layer4 flag option ctl2 flags*/
    CTC_PARSER_L4_FLAGS_FLAG_OP_CTL3 = 0x200,               /**<layer4 flag option ctl3 flags*/

    CTC_PARSER_L4_FLAGS_PORT_OP_CTL0 = 0x400,               /**<layer4 port option ctl0 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL1 = 0x800,               /**<layer4 port option ctl1 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL2 = 0x1000,              /**<layer4 port option ctl2 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL3 = 0x2000,              /**<layer4 port option ctl3 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL4 = 0x4000,              /**<layer4 port option ctl4 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL5 = 0x8000,              /**<layer4 port option ctl5 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL6 = 0x10000,             /**<layer4 port option ctl6 flags*/
    CTC_PARSER_L4_FLAGS_PORT_OP_CTL7 = 0x20000,             /**<layer4 port option ctl7 flags*/

    CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL0 = 0x40000,          /**<layer4 udp application option ctl0 flags*/
    CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL1 = 0x80000,          /**<layer4 udp application option ctl1 flags*/
    CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL2 = 0x100000,         /**<layer4 udp application option ctl2 flags*/
    CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL3 = 0x200000,         /**<layer4 udp application option ctl3 flags*/

    CTC_PARSER_L4_FLAGS_PORT_OP_SEL_DEST_PORT = 0x400000,   /**<layer4 port option selection dest port flags*/

    CTC_PARSER_L4_FLAGS_L4FLEX_CTL_BYTE_SEL0   = 0x800000,  /**<layer4 flex ctl byte select0 flags*/
    CTC_PARSER_L4_FLAGS_L4FLEX_CTL_BYTE_SEL1   = 0x1000000, /**<layer4 flex ctl byte select1 flags*/
    CTC_PARSER_L4_FLAGS_L4FLEX_CTL_APP_MIN_LEN = 0x4000000, /**<layer4 flex application min length flags*/
    CTC_PARSER_L4_FLAGS_L4FLEX_CTL_MIN_LEN     = 0x8000000, /**<layer4 flex min length flags*/

    CTC_PARSER_L4_FLAGS_L4PTP_CTL_PTP_EN = 0x10000000,      /**<layer4 ptp enable flags*/
    CTC_PARSER_L4_FLAGS_L4PTP_CTL_PORT0  = 0x20000000,      /**<layer4 ptp port0 flags*/
    CTC_PARSER_L4_FLAGS_L4PTP_CTL_PORT1  = 0x40000000       /**<layer4 ptp port1 flags*/
};
typedef  enum ctc_parser_layer4_flags_e ctc_parser_layer4_flags_t;

/**
 @brief  layer4 length option ctl flags
*/
enum ctc_parser_l4len_op_flags_e
{
    CTC_PARSER_L4LEN_OP_FLAGS_CTL0  = 0x1,      /**<layer4 length option flags ctl0*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL1  = 0x2,      /**<layer4 length option flags ctl1*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL2  = 0x4,      /**<layer4 length option flags ctl2*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL3  = 0x8,      /**<layer4 length option flags ctl3*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL4  = 0x10,     /**<layer4 length option flags ctl4*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL5  = 0x20,     /**<layer4 length option flags ctl5*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL6  = 0x40,     /**<layer4 length option flags ctl6*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL7  = 0x80,     /**<layer4 length option flags ctl7*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL8  = 0x100,    /**<layer4 length option flags ctl8*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL9  = 0x200,    /**<layer4 length option flags ctl9*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL10 = 0x400,    /**<layer4 length option flags ctl10*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL11 = 0x800,    /**<layer4 length option flags ctl11*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL12 = 0x1000,   /**<layer4 length option flags ctl12*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL13 = 0x2000,   /**<layer4 length option flags ctl13*/
    CTC_PARSER_L4LEN_OP_FLAGS_CTL14 = 0x4000    /**<layer4 length option flags ctl14*/

};
typedef enum ctc_parser_l4len_op_flags_e ctc_parser_l4len_op_flags_t;

/**
 @brief  layer4 protocol ctl flags
*/
enum ctc_parser_l4_ptl_flags_e
{
    CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_VALUE = 0x1,       /**<layer4 protocol source port flags*/
    CTC_PARSER_L4_PTL_FLAGS_DST_PORT_VALUE = 0x2,       /**<layer4 protocol dest port flags*/
    CTC_PARSER_L4_PTL_FLAGS_APP_TYPE       = 0x4,       /**<layer4 protocol application type flags*/
    CTC_PARSER_L4_PTL_FLAGS_BYTE0_SEL      = 0x8,       /**<layer4 protocol byte0 selection flags*/
    CTC_PARSER_L4_PTL_FLAGS_BYTE_SEL_DEST  = 0x10,      /**<layer4 protocol byte select dest flags*/
    CTC_PARSER_L4_PTL_FLAGS_ENTRY_VLD      = 0x20,      /**<layer4 protocol entry valid flags*/
    CTC_PARSER_L4_PTL_FLAGS_BYTE1_SEL      = 0x40,      /**<layer4 protocol byte1 select flags*/
    CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_MASK  = 0x80,      /**<layer4 protocol source port mask flags*/
    CTC_PARSER_L4_PTL_FLAGS_DEST_PORT_MASK = 0x100,     /**<layer4 protocol dest port mask flags*/
    CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_MASK  = 0x200,     /**<layer4 protocol tcp flags mask*/
    CTC_PARSER_L4_PTL_FLAGS_IS_TCP_MASK    = 0x400,     /**<lyaer4 protocol is tcp mask*/
    CTC_PARSER_L4_PTL_FLAGS_IS_TCP_VALUE   = 0x800,     /**<layef4 protocol is tcp value*/
    CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_VALUE = 0x1000     /**<layer4 protocol tcp value*/
};
typedef enum ctc_parser_l4_ptl_flags_e  ctc_parser_l4_ptl_flags_t;

/**
 @brief  layer4 protocol entry fields
*/
struct ctc_parser_l4_ptl_entry_s
{
    uint32 flags;           /**<CTC_PARSER_L4_PTL_FLAGS_XXX*/

    uint16 src_port_value;  /**<source port value*/
    uint16 dst_port_value;  /**<dest port value*/

    uint8 application_type; /**<application type*/
    uint8 byte0_select;     /**<byte0 select*/
    uint8 byte_select_dest; /**<byte selection dest */
    uint8 entry_vld;        /**<entry valid*/

    uint8 byte1_select;     /**<byte1 selection*/
    uint8 source_port_mask; /**<source port mask*/
    uint8 dest_port_mask;   /**<dest port mask*/
    uint8 tcp_flag_mask;    /**<tcp flag mask*/

    uint8 is_tcp_mask;      /**<is tcp mask*/
    uint8 is_tcp_value;     /**<is tcp value*/
    uint8 tcp_flag_value;   /**<tcp flag value*/

};
typedef struct ctc_parser_l4_ptl_entry_s ctc_parser_l4_ptl_entry_t;

/**
 @brief  ipv6 extend header type
*/
enum ctc_parser_ipv6_extend_header_type_e
{
    CTC_PARSER_IPV6_EHT_ROUTING     = 1,   /**<routing  header*/
    CTC_PARSER_IPV6_EHT_FRAGMENT    = 2,   /**<flagment  header*/
    CTC_PARSER_IPV6_EHT_DEST_OPT    = 3,   /**<destination option header */
    CTC_PARSER_IPV6_EHT_AUTHEN      = 4,   /**<authentiation  header */
    CTC_PARSER_IPV6_EHT_SCTY_ENCAP  = 5    /**<Encapsulating  security payload header */
};
typedef enum ctc_parser_ipv6_extend_header_type_e  ctc_parser_ipv6_extend_header_type_t;

/**@} end of @defgroup  parser_l4 PARSER_L4*/

/**@} end of @defgroup  parser PARSER */

#endif



