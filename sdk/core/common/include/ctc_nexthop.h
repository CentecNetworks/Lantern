/**
 @file ctc_nexthop.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-12-11

 @version v2.0

 This file contains all Nexthop related data structure, enum, macro and proto.
*/

#ifndef _CTC_NEXTHOP_H
#define _CTC_NEXTHOP_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_port.h"
#include "ctc_vlan.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define CTC_MPLS_NH_MAX_PUSH_LABEL_NUM     4 /**< The maximum of MPLS lable can be pushed */
#define CTC_MPLS_NH_MAX_TUNNEL_LABEL_NUM   2 /**< The maximum of MPLS lable can be pushed */
#define CTC_NH_ECMP_MEMBER_NUM_MAX         8 /**< The maximum of ECMP member number */

/****************************************************************
*
* Data Structures
*
****************************************************************/

/**
 @defgroup nexthop NEXTHOP
 @{
*/

/**
 @brief  Define Nexthop outgoing interface type flag
*/
enum ctc_nh_reserved_nhid_e
{
    CTC_NH_RESERVED_NHID_FOR_NONE   = 0,    /**< Global reserved nexthop Id for none */
    CTC_NH_RESERVED_NHID_FOR_DROP   = 1,    /**< Global reserved nexthop Id for drop */
    CTC_NH_RESERVED_NHID_FOR_TOCPU  = 2,    /**< Global reserved nexthop Id for cpu */
    CTC_NH_RESERVED_NHID_MAX        = 3
};
typedef enum ctc_nh_reserved_nhid_e ctc_nh_reserved_nhid_t;
enum ctc_nh_oif_type_e
{
    CTC_NH_OIF_TYPE_VLAN_PORT,      /**< Outgoing interface is vlan port */
    CTC_NH_OIF_TYPE_ROUTED_PORT,    /**< Outgoing interface is routed port*/
    CTC_NH_OIF_TYPE_SUB_IF,         /**< Outgoing interface is sub-interface*/
    CTC_NH_OIF_TYPE_MAX
};
typedef enum ctc_nh_oif_type_e ctc_nh_oif_type_t;

/**
 @brief  Define ECMP update type
*/
enum ctc_nh_ecmp_update_type_e
{
    CTC_NH_ECMP_ADD_MEMBER,        /**< Add a member into ECMP group */
    CTC_NH_ECMP_REMOVE_MEMBER,     /**< Remove a member from ECMP group */

    CTC_NH_ECMP_UPDATE_OP_MAX
};
typedef enum ctc_nh_ecmp_update_type_e ctc_nh_ecmp_update_type_t;



/**
 @brief  Define Nexthop outgoing interface type flag
*/
struct ctc_nh_global_cfg_s
{
    uint32 max_external_nhid;               /**< max nhid used for external nexthop should be a multiple of 16 */
    uint32 acl_redirect_fwd_ptr_num;        /**< The number of ds_fwd_ptr reserved for acl redirect function */
    uint32 local_met_num;                   /**< value 0: indicate local met share sram with dsFwd */
    uint32 local_nh_4w_num;                 /**< value 0: indicate local nexthop 4w share sram with local nexthop 8w */
    uint32 l2_edit_4w_num;                  /**< value 0: indicate l2edit 4w share sram with l2edit 8w */
    uint32 l3_edit_4w_num;                  /**< value 0: indicate l3edit 4w share sram with l3edit 8w */
};
typedef struct ctc_nh_global_cfg_s ctc_nh_global_cfg_t;

/**
 @brief  Define Nexthop update type
*/
enum ctc_nh_upd_type_s
{
    CTC_NH_UPD_UNRSV_TO_FWD,    /**< update unresoved to forwarding nexthop  */
    CTC_NH_UPD_FWD_TO_UNRSV,    /**< update forwarding to unresoved nexthop */
    CTC_NH_UPD_FWD_ATTR,        /**<  updated forwarding nexthop attribute */
    MAX_CTC_NH_UPDTYPE
};
typedef enum ctc_nh_upd_type_s ctc_nh_upd_type_t;

 /**
 @brief  Define ECMP create data structure
*/
struct ctc_nh_ecmp_creat_data_s
{
    uint32 ecmp_nhid;                       /**< ECMP nh ID, it is a return value */
    uint32 nhid[CTC_NH_ECMP_MEMBER_NUM_MAX];/**< ECMP member nh ID */
    uint8 nh_num;                           /**< Number of members  */
};
typedef struct ctc_nh_ecmp_creat_data_s ctc_nh_ecmp_creat_data_t;

/**
 @brief  Define ECMP update data structure
*/
struct ctc_nh_ecmp_update_data_s
{
    uint32 ecmp_nhid;     /**< ECMP nh ID */
    uint32 nhid;          /**< ECMP member nh ID */
    uint8 op_type;        /**< CTC_NH_ECMP_ADD_MEMBER or CTC_NH_ECMP_REMOVE_MEMBER */
};
typedef struct ctc_nh_ecmp_update_data_s ctc_nh_ecmp_update_data_t;


/**
 @brief  Define Nexthop outgoing interface information structure
*/
struct ctc_nh_oif_info_s
{
    uint16 gport;           /**< Outgoing global port*/
    uint16 vid;             /**< Outgoing vlan id ,when vid = 4095,the vid is invalid,
                                  if vlan port is normal L2 interface and vlan is valid,
                                   the vlan will be used to get egress vlan property*/

    uint8 outervlan_is_svlan;      /**< If set,outer vlan will be svlan*/
    ctc_nh_oif_type_t oif_type;    /**< Outgoing interface type */
};
typedef struct ctc_nh_oif_info_s ctc_nh_oif_info_t;

/**
 @defgroup ip IP
 @{
*/

/**
 @brief  Define ip nexthop flag enum
*/
enum ctc_ip_nh_flag_e
{
    CTC_IP_NH_FLAG_USE_DSNH8W     = 0x00000001,     /**< IP next hop use 8 world length dsnexthop */
    CTC_IP_NH_FLAG_UNROV          = 0x00000002,     /**< unresolved IP next hop */
    CTC_IP_NH_FLAG_USE_PACKET_TTL = 0x00000004,     /**< IP next hop use packet ttl */
    CTC_IP_NH_FLAG_SEQUENCE_CHECK = 0x00000008,     /**< IP next hop with sequence check */
    CTC_IP_NH_FLAG_MAX
};
typedef enum ctc_ip_nh_flag_e ctc_ip_nh_flag_t;

/**
 @brief  Define ip nexthop operation type enum
*/
enum ctc_ip_nh_op_e
{
    /*ip  operation*/
    CTC_IP_NH_OP_IP_ROUTE,               /**< IP nexthop is normal ipuc */
    CTC_IP_NH_OP_IPV6_IN_IPV4,           /**< IP nexthop is Ipv4 tunnel, payload is ipv6 */
    CTC_IP_NH_OP_IPV4_IN_IPV6,           /**< IP nexthop is Ipv6 tunnel, payload is ipv4 */
    CTC_IP_NH_OP_ETH_IN_IPV4_FOR_MIRROR, /**< IP nexthop is Ipv4 tunnel, payload is ethernet */
    CTC_IP_NH_OP_ETH_IN_GRE,    		 /**< IP nexthop is gre tunnel, payload is ethernet*/

    CTC_P_NH_OP_MAX
};
typedef enum ctc_ip_nh_op_e ctc_ip_nh_op_t;

/**
 @brief  Define gre type enum
*/
enum ctc_ip_nh_tunnel_gre_flag_e
{
    CTC_IP_NH_TUNNEL_GRE_FLAG_SEQ_VALID,
    CTC_IP_NH_TUNNEL_GRE_FLAG_KEY_VALID,
    CTC_IP_NH_TUNNEL_GRE_FLAG_RSV_VALID,
    CTC_IP_NH_TUNNEL_GRE_FLAG_CHK_VALID,

    CTC_IP_NH_TUNNEL_GRE_FLAG_MAX

};
typedef enum ctc_ip_nh_tunnel_gre_flag_e ctc_ip_nh_tunnel_gre_flag_t;

/**
 @brief  Define ip nexthop tunnel ecap info structure
*/
struct ctc_ip_nh_tunnel_encap_gre_info_s
{
    uint8  gre_ver;  /**< GRE verion */
    uint8  gre_flag; /**< GRE flag(CRKS) */
    uint16 seq_id;   /**< Sequence id */
    uint32 gre_key;  /**< GRE Key */
};
typedef struct ctc_ip_nh_tunnel_encap_gre_info_s ctc_ip_nh_tunnel_encap_gre_info_t;

/**
 @brief  Define ip nexthop tunnel ttl flag enum
*/
enum ctc_ip_nh_tunnel_ttl_flag_e
{
    CTC_IP_NH_TUNNEL_FLAG_MAP_TTL        =    (1 << 0),  /**< If set this flag, means new TTL mapped from (pkt_ttl - ttl)*/
    CTC_IP_NH_TUNNEL_FLAG_COPY_DONT_FRAG =    (1 << 1),  /**< If set this flag, means new dont frag will copy payload ip dont frag*/
    CTC_IP_NH_TUNNEL_FLAG_DONT_FRAG      =    (1 << 2),  /**< If set this flag, means new dont frag bit in the ip header*/
    CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK      =    (1 << 3),  /**< If set this flag, means mtu check enable*/
    CTC_IP_NH_TUNNEL_FLAG_LOOPBACK       =    (1 << 4)   /**< If set this flag, means loopback after tunnel*/
};
typedef enum ctc_ip_nh_tunnel_ttl_flag_e ctc_ip_nh_tunnel_ttl_flag_t;

/**
 @brief  Define ip nexthop tunnel dscp flag enum
*/
enum ctc_ip_nh_tunnel_dscp_flag_e
{
    CTC_IP_NH_TUNNEL_DSCP_USE_USER_DEFINE,      /**< New exp equal the dscp or tos value in this structure*/
    CTC_IP_NH_TUNNEL_DSCP_FROM_SRC_DSCP,        /**< New exp mapped from ingress dscp value*/
    CTC_IP_NH_TUNNEL_DSCP_FROM_PKT_PRIORITY     /**< New exp mapped from packet's priority*/
};
typedef enum ctc_ip_nh_tunnel_dscp_flag_e ctc_ip_nh_tunnel_dscp_flag_t;


/**
 @brief  Define ip nexthop tunnel type flag enum
*/
enum ctc_ip_nh_tunnel_type_flag_e
{
    CTC_IP_NH_TUNEEL_TYPE_MANUAL,     /**< IPv6-IN-IPv4 tunnel,ipv4 sa and da is configable */
    CTC_IP_NH_TUNEEL_TYPE_AUTO,       /**< IPv6-IN-IPv4 tunnel,ipv4 da copy ipv6 da(last 32bit) */
    CTC_IP_NH_TUNEEL_TYPE_6TO4,       /**< IPv6-IN-IPv4 tunnel,ipv4 da and sa copy ipv6 da and (middle 32bit) */
    CTC_IP_NH_TUNEEL_TYPE_ISATAP,     /**< IPv6-IN-IPv4 tunnel,ipv4 da copy ipv6 da(last 32bit) */
    CTC_IP_NH_TUNNEL_TYPE_6TO4_MANUAL /**< IPv6-IN-IPv4 tunnel,ipv4 da copy ipv6 da and sa is configable */
};
typedef enum ctc_ip_nh_tunnel_type_flag_e ctc_ip_nh_tunnel_type_flag_t;


/**
 @brief  Define ip nexthop tunnel encap type flag enum
*/
enum ctc_ip_nh_tunnel_encap_type_flag_e
{
    CTC_IP_TUNNEL_NH_ENCAP_TYPE_NONE,   /**< Have no ecapsulate info */
    CTC_IP_TUNNEL_NH_ENCAP_TYPE_GRE,    /**< Have GRE ecapsulate info */
    CTC_IP_TUNNEL_NH_ENCAP_TYPE_UDP     /**< Have UDP ecapsulate info */
};
typedef enum ctc_ip_nh_tunnel_encap_type_flag_e ctc_ip_nh_tunnel_encap_type_flag_t;


/**
 @brief  Define brg unicast nexthop sub type enum
*/
enum ctc_nh_param_brguc_sub_type_e
{
    CTC_NH_PARAM_BRGUC_SUB_TYPE_BASIC               = (1 << 0), /**< basic type nexthop*/
    CTC_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED            = (1 << 1), /**< apply for port untagged mode */
    CTC_NH_PARAM_BRGUC_SUB_TYPE_BYPASS              = (1 << 2), /**< bypass type nexthop */
    CTC_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU = (1 << 3), /**< raw pacekt type nexthop */
    CTC_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE       = (1 << 4), /**< service queue type nexthop*/
    CTC_NH_PARAM_BRGUC_SUB_TYPE_NONE                =  0        /**< default no subtype info */

};
typedef enum ctc_nh_param_brguc_sub_type_e ctc_nh_param_brguc_sub_type_t;


/**
 @brief  Define ip nexthop tunnel info structure
*/
struct ctc_ip_nh_tunne_info_s
{
    uint8 tunnel_flag;  /**< Ip tunnel flag */
    uint8 ttl;          /**< TTL value*/
    uint16 mtu_size;    /**< MTU size*/

    uint8 dscp_type;    /**< New ip dscp in ip header*/
    uint8 dscp_or_tos;  /**< Dscp value for ipv4, tos for ipv6*/
    uint8 tunnel_type;  /**< IP tunnel type*/
    uint8 encap_type;   /**< IP tunnel ecapsulate type(GRE,UDP or NONE*/

    uint8 strip_svlan;   /**< Strip svlan tag before encapsulation */
    uint8 strip_cvlan;   /**< Strip cvlan tag before encapsulation */
    uint8 resv[2];

    union
    {
        ip_addr_t   ipv4; /**< Ip version 4 address */
        ipv6_addr_t ipv6; /**< Ip version 6 address */
    }ip_sa;               /**< New ip sa address in ip header*/

    union
    {
        ip_addr_t   ipv4; /**< Ip version 4 address */
        ipv6_addr_t ipv6; /**< Ip version 6 address */
    }ip_da;               /**< New ip da address in ip header*/

    union
    {
        ctc_ip_nh_tunnel_encap_gre_info_t gre_info; /**< ip nexthop tunnel ecap info */
    } encap_info;   /**< Mean ctc_ip_nh_tunnel_encap_gre_info_t or ctc_ip_nh_tunnel_encap_udp_info_t*/


};
typedef struct ctc_ip_nh_tunne_info_s ctc_ip_nh_tunne_info_t;



/**
 @brief  Define ip nexthop parametes structure for cliv2
*/
struct ctc_ip_nh_param_s
{
    uint32                  flag;       /**< IP nexthop flag --> ctc_ip_nh_flag_t,CTC_IP_NH_FLAG_xxx flags*/
    ctc_ip_nh_op_t          opcode;     /**< Operation type*/
    ctc_nh_upd_type_t       upd_type;   /*nexthop update type ,used for update Operation*/
    ctc_nh_oif_info_t       oif;        /**< Outgoing interface if not need loopback for tunnel*/
    mac_addr_t              mac;        /**< Outgoing mac if not need loopback for tunnel*/
    ctc_ip_nh_tunne_info_t  tunnel_info;/**< If nexthop is tunnel, this struct assign tunnel info*/


    uint32                  dsnh_offset;/**< DsNh_offset */
};
typedef struct ctc_ip_nh_param_s ctc_ip_nh_param_t;

/**
 @brief  Define ip tunnel infor parameter structure for IP-BFD
*/
struct ctc_ip_tunnel_param_s
{
    uint32 ip_da;             /**<IP da in ip header*/
    uint32 ip_sa;             /**<IP sa in ip header*/
    uint8  ip_ttl;            /**<IP ttl in ip header*/
    uint8  ip_dscp;           /**<IP dscp in ip header*/
};
typedef struct ctc_ip_tunnel_param_s ctc_ip_tunnel_param_t;

/**@} end of @defgroup  ip IP */

/**
 @defgroup EgressVlanEdit EgressVlanEdit
 @{
*/
/**
 @brief Define the type of vlan edit type in EPE of Humber
*/
enum ctc_vlan_egress_edit_type_e
{
    /*output vlan is same as ingress mapped vlan(eg. vlan translation vid, epe do not edit the vlan*/
    CTC_VLAN_EGRESS_EDIT_NONE,                  /**< Will keep ingress mapped vlan*/
    /*Keep output vlan same as input vlan(raw vlan) in the packet*/
    CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE,    /**< Will keep original packet's vlan*/
    CTC_VLAN_EGRESS_EDIT_INSERT_VLAN,           /**< Will insert a new vlan, according to ingress mapped vlan or egress specified vlan, egress vlan have high priority*/
    CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN,          /**< Will replace original vlan, according to ingress mapped vlan or egress specified vlan, egress vlan have high priority; if original packet is untagged, will append a new tag*/
    CTC_VLAN_EGRESS_EDIT_STRIP_VLAN,            /**< Will strip original vlan*/
    CTC_VLAN_EGRESS_EDIT_MAX
};
typedef enum ctc_vlan_egress_edit_type_e ctc_vlan_egress_edit_type_t;


/**
 @brief  Define Egress Vlan Editing nexthop flag
*/
enum ctc_vlan_egress_edit_nh_flag
{
    CTC_VLAN_NH_SERVICE_QUEUE_FLAG = (1 << 0),    /**< create a egress valn nexthop with service queue select type*/
};
typedef enum ctc_vlan_egress_edit_nh_flag ctc_vlan_egress_edit_nh_flag_e;


/**
 @brief  Define eloop and iloop loopback nexthop parameter structure
*/
struct ctc_e_i_loop_param_s
{
    bool customerid_valid;	                /**< If set, indicates the inner payload has an MPLS encapsulation or GRE key, which contains the customer ID as the label */
    bool map_vclabel_exp;	                /**< If set, indicates that the priority and color are mapped from the EXP field in VC label. this flag is only effective in MPLS VPN loopback */
    bool inner_packet_type_valid;           /**< This field indicates if the inner_packet_type is valid*/
    bool vpls_src_port;                     /**< If set, indicates that the packet is from a VPLS tunnel port*/
    bool service_queue_en;                  /**< If set, indicates that the packets will do service queue processing*/
    bool sequence_en;                       /**< If set, indicates that the packets will do sequence order */
    ctc_parser_pkt_type_t inner_packet_type;/**< This field indicates the encapsulated packet type */
    uint8 lpbk_lport;                       /**< Loopback source local physical port, should not be linkagg port*/
    uint8 words_removed_from_hdr;           /**< 1 word stands for 4 bytes, not include customerId 4 bytes;*/
    uint8 sequence_counter_index;           /**< Sequence number counter index */
};
typedef struct ctc_e_i_loop_param_s ctc_e_i_loop_param_t;

/**
 @brief  Define egress vlan edit information structure
*/
struct ctc_vlan_egress_edit_info_s
{
    uint32 flag;                                    /**<nexthop flag, CTC_VLAN_NH_XXX_FLAG*/
    /*If cvlan edit type is insert cvlan, then svlan edit should not be replace;
    And if svlan edit type is insert svlan, cvlan edit should not be replace*/
    ctc_vlan_egress_edit_type_t cvlan_edit_type;    /**<Edit type on cvlan*/
    ctc_vlan_egress_edit_type_t svlan_edit_type;    /**<Edit type on svlan*/
    uint16 output_cvid;                             /**<Specified output cvlan id*/
    uint16 output_svid;                             /**<Specified output svlan id*/
    uint8 edit_flag;                                /**<Edit flag*/
#define CTC_VLAN_EGRESS_EDIT_VLAN_SWAP_EN           (1 << 0)    /**<Edit flag indicate vlan swap*/
#define CTC_VLAN_EGRESS_EDIT_TPID_SWAP_EN           (1 << 1)    /**<Edit flag indicate tpid swap*/
#define CTC_VLAN_EGRESS_EDIT_COS_SWAP_EN            (1 << 2)    /**<Edit flag indicate cos swap*/
#define CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID       (1 << 3)    /**<Edit flag indicate output cvlan id*/
#define CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID       (1 << 4)    /**<Edit flag indicate output svlan id*/

    uint8 replace_stag_cos_valid;
    uint8 map_stag_cos_valid;
    uint8 stag_cos;
    
    uint8 resv[3];

    uint8 e_i_loop;                                 /**<1.if set, output packet will do eloop and iloop */
    ctc_e_i_loop_param_t e_iloop_param;             /**< Edit eloop and iloop */
};
typedef struct ctc_vlan_egress_edit_info_s ctc_vlan_egress_edit_info_t;

/**
 @brief Define the data structure about vlan nexthop parameter structure
*/
struct ctc_vlan_edit_nh_param_s
{
    uint32 dsnh_offset;                           /**< DsNh_offset */

    uint16 gport_or_aps_bridge_id;                /**< while egress valn nexthop : Destination global port ; while ASP Egress Vlan Editing nexthop : APS bridge group id */
    uint8 aps_en;                                 /**< if set,indicate aps enable and vlan_edit_info_p will be used as protection path nexthop param */
    uint8 rsv0;                                   /**< Reserved0 */

    ctc_vlan_egress_edit_info_t vlan_edit_info;   /**< while egress valn nexthop : Egress vlan edit information ; while ASP Egress Vlan Editing nexthop : Egress vlan edit information of working path*/
    ctc_vlan_egress_edit_info_t vlan_edit_info_p; /**< only used in ASP Egress Vlan Editing nexthop : Egress vlan edit information of protection path*/
};
typedef struct ctc_vlan_edit_nh_param_s ctc_vlan_edit_nh_param_t;
/**@} end of @defgroup EgressVlanEdit  */



/**
 @defgroup mpls MPLS
 @{
*/

/**
 @brief  Define MPLS nexthop operation type
*/
enum ctc_mpls_nh_op_e
{
    /*Push operation*/
    CTC_MPLS_NH_PUSH_OP_NONE,             /**< Will append new layer2 header, mpls payload is Ethernet, and will keep mpls payload unchanged*/
    CTC_MPLS_NH_PUSH_OP_ROUTE,            /**< Will append new layer2 header, mpls payload is IP, and ip TTL will be updated*/
    CTC_MPLS_NH_PUSH_OP_L2VPN,            /**< Will append new layer2 header, mpls payload is Ethernet, ethernet packet's tag could be edited*/

    /* pop operation*/
    CTC_MPLS_NH_POP_OP_NONE,              /**< Will append new layer2 header, layer2 payload could be ip or mpls, layer2 payload will not be edited*/
    CTC_MPLS_NH_POP_OP_ROUTE,             /**< Will append new layer2 header, layer2 payload is ip, will decreace ip TTL*/
    CTC_MPLS_NH_POP_OP_MPLS,              /**< Will append new layer2 header, layer2 payload is mpls, will update mpls TTL and exp*/
    CTC_MPLS_NH_POP_OP_L2VPN_WITH_EDIT,   /**< Will append new layer2 header, layer2 payload is ethernet, ethernet packet's tag could be edited*/
    CTC_MPLS_NH_POP_OP_L2VPN_NO_EDIT,     /**< Will keep original layer2 header, doesn't make any change*/

    CTC_MPLS_NH_POP_OP_MAX
};
typedef enum ctc_mpls_nh_op_e ctc_mpls_nh_op_t;

/**
 @brief  Define ip tunnel nexthop label type
*/
enum ctc_mpls_nh_label_type_flag_e
{
    CTC_MPLS_NH_LABEL_IS_VALID  = (1 << 0) ,  /**< If set this flag, means this MPLS label is valid*/
    CTC_MPLS_NH_LABEL_IS_MCAST  = (1 << 1) ,  /**< If set this flag, means this MPLS label is mcast label*/
    CTC_MPLS_NH_LABEL_MAP_TTL   = (1 << 2)    /**< If set this flag, means new TTL mapped from (pkt_ttl - ttl)*/
};
typedef enum ctc_mpls_nh_label_type_flag_e ctc_mpls_nh_label_type_flag_t;

/**
 @brief  Define MPLS nexthop New exp type
*/
enum ctc_mpls_nh_exp_type_flag_e
{
    CTC_MPLS_NH_EXP_USE_LABEL_EXP,           /**< New exp equal the EXP value in this structure*/
    CTC_MPLS_NH_EXP_DERIVE_FROM_SRC_EXP,     /**< New exp mapped from ingress exp value*/
    CTC_MPLS_NH_EXP_DERIVE_FROM_PKT_PRIORITY /**< New exp mapped from packet's priority*/
};
typedef enum ctc_mpls_nh_exp_type_flag_e ctc_mpls_nh_exp_type_flag_t;

/**
 @brief  Define MPLS label parameter structure
*/
struct ctc_mpls_nh_label_param_s
{
    uint8  lable_flag; /**< MPLS label flag,ctc_ip_nh_tunnel_label_type_flag_t */
    uint8  ttl;        /**< TTL value, if FTN 's pipe mode ,ttl should be euqal to 0,else 1*/
    uint8  exp_type;   /**< New MPLS exp type in this label*/
    uint8  exp;        /**< EXP value*/
    uint32 label;      /**< MPLS label value*/
};
typedef struct ctc_mpls_nh_label_param_s ctc_mpls_nh_label_param_t;

/**
 @brief  Define MPLS martini label sequence type
*/
enum ctc_mpls_nh_martini_seq_type_e
{
    CTC_MPLS_NH_MARTINI_SEQ_NONE = 0,   /**< Martini label sequence number type none, means doesn't support martini label sequence*/
    CTC_MPLS_NH_MARTINI_SEQ_PER_PW,     /**< Martini label sequence number type is per-PW*/
    CTC_MPLS_NH_MARTINI_SEQ_GLB_TYPE0,  /**< Martini label sequence number type is global type 0*/
    CTC_MPLS_NH_MARTINI_SEQ_GLB_TYPE1,  /**< Martini label sequence number type is global type 1*/

    CTC_MPLS_NH_MARTINI_SEQ_MAX
};
typedef enum ctc_mpls_nh_martini_seq_type_e ctc_mpls_nh_martini_seq_type_t;

/**
 @brief  Define MPLS nexthop flag type
*/
enum ctc_mpls_nh_flag_type_e
{
    CTC_MPLS_NH_FLAG_INNER_TPID_OW_EN  =  (1 << 0),     /**< Enable inner tpid ow */
    CTC_MPLS_NH_FLAG_STATS_EN          =  (1 << 1),     /**< Enable stats */
    CTC_MPLS_NH_FLAG_L3IF_EN           =  (1 << 2),     /**< Enable layer3 interface */
    CTC_MPLS_NH_FLAG_VPLS              =  (1 << 3),     /**< VPLS */
    CTC_MPLS_NH_FLAG_SQN_EN            =  (1 << 4),     /**< Enable SQN */
    CTC_MPLS_NH_FLAG_SERVICE_QUEUE_EN  =  (1 << 5),     /**< Enable service queue */
    CTC_MPLS_NH_FLAG_APS_EN            =  (1 << 6)      /**< Enable APS */
};
typedef enum ctc_mpls_nh_flag_type_e ctc_mpls_nh_flag_type_t;


/**
 @brief  Define MPLS nexthop common parameter structure
*/
struct ctc_mpls_nexthop_com_param_s
{
    uint32  mpls_nh_flag;                  /**< MPLS nexthop flag*/

    ctc_mpls_nh_op_t   opcode;             /**< MPLS nexthop operation code*/
    ctc_nh_oif_info_t  oif;                /**< MPLS nexthop outgoing interface*/
    mac_addr_t mac;                        /**< MPLS nexthop outgoing MAC address*/
    ctc_vlan_egress_edit_info_t vlan_info; /**< VLAN edit information, used for layer2 VPN*/
    ctc_dot1q_type_t  inner_dot1q_type;    /**< Inner dot1q type, used for layer2 VPN*/
    uint32 stats_ptr;                      /**< Stats pointer*/
};
typedef struct ctc_mpls_nexthop_com_param_s ctc_mpls_nexthop_com_param_t;


/**
 @brief  Define MPLS tunnel label nexthop parameter structure
*/
struct ctc_mpls_nexthop_tunnel_info_s
{
    ctc_mpls_nh_label_param_t  tunnel_label[CTC_MPLS_NH_MAX_TUNNEL_LABEL_NUM]; /**< MPLS lable  information.Order:VC label-->Tunnel labell*/
    mac_addr_t mac;         /**< MPLS nexthop outgoing MAC address */
    uint16     rsv1;        /**< researve for future */
    bool       stats_valid; /**< stats valid*/
};
typedef struct ctc_mpls_nexthop_tunnel_info_s ctc_mpls_nexthop_tunnel_info_t;

/**
 @brief  Define MPLS tunnel label nexthop parameter structure
*/
struct ctc_mpls_nexthop_tunnel_param_s
{
    ctc_mpls_nexthop_tunnel_info_t    nh_param;
    ctc_mpls_nexthop_tunnel_info_t    nh_p_param;
    bool    aps_en;      /**< stats valid*/
    uint8   label_num;   /**< the number of tunnel label */
    uint8   rsv0;        /**< researve for future */
    uint16  rsv1;        /**< researve for future */
};
typedef struct ctc_mpls_nexthop_tunnel_param_s ctc_mpls_nexthop_tunnel_param_t;
/**
 @brief  Define MPLS push nexthop parameter structure
*/
struct ctc_mpls_nexthop_push_param_s
{
    ctc_mpls_nexthop_com_param_t nh_com;/**< MPLS nexthop common information*/
    ctc_mpls_nh_label_param_t    push_label[CTC_MPLS_NH_MAX_PUSH_LABEL_NUM]; /**< MPLS lable  information,Order :VC label-->Tunnel label*/
    bool vpls_port_valid;               /**< VPLS port is valid*/
    bool martini_encap_valid;           /**< Martini label is valid*/
    bool stats_valid;                   /**< stats is valid*/
    ctc_mpls_nh_martini_seq_type_t seq_num_type;  /**< Martini label sequence number type*/

    uint32 seq_num_index;               /**< Martini label sequence number index*/
    uint16 vpls_port;                   /**< VPLS port value */
    uint8  label_num;                   /**< the number of  label,include tunnel label */
    uint8  tunnel_label_num;            /**< the number of  tunnel label */

};
typedef struct ctc_mpls_nexthop_push_param_s ctc_mpls_nexthop_push_param_t;

/**
 @brief  Define MPLS pop nexthop parameter structure
*/
struct ctc_mpls_nexthop_pop_param_s
{
    ctc_mpls_nexthop_com_param_t nh_com; /**< MPLS nexthop common information*/
    bool use_ttl_from_packet;            /**< Use TTL from packet*/
    bool vpls_port_valid;                /**< VPLS port is valid*/
    uint16 vpls_port;                    /**< VPLS port value */
};
typedef struct ctc_mpls_nexthop_pop_param_s ctc_mpls_nexthop_pop_param_t;

/**
 @brief  Define MPLS siwtch nexthop parameter structure
*/
struct ctc_mpls_nexthop_switch_param_s
{
    uint16  gport;                         /**< outgoing interface */
    uint16  vpls_port;                     /**< VPLS port value */
    bool    vpls_port_valid;               /**< VPLS port is valid*/
    bool    egs_vlan_valid;                /*Optional, used for l2vpn*/
    ctc_vlan_egress_edit_info_t vlan_info; /**< VLAN edit information, used for layer2 VPN*/
};
typedef struct ctc_mpls_nexthop_switch_param_s ctc_mpls_nexthop_switch_param_s;

/**
 @brief  Define MPLS nexthop property associated type emun
*/
enum ctc_mpls_nh_prop_e
{
    CTC_MPLS_NH_NONE ,               /**<  any mpls nexthop */
    CTC_MPLS_NH_PUSH_TYPE ,          /**<  a mpls push nexthop */
    CTC_MPLS_NH_POP_TYPE,            /**<  a mpls pop nexthop */
    CTC_MPLS_NH_SWITCH_TYPE ,        /**<  a mpls  switch nexthop */
    CTC_MPLS_NH_PROP_TYPE_MAX
};
typedef enum ctc_mpls_nh_prop_e ctc_mpls_nh_prop_t;

/**
 @brief  Define MPLS nexthop operation type emun
*/
enum ctc_mpls_nh_operation_type_flag_e
{
    CTC_MPLS_NH_IS_NH_8W             = 0x00000001,   /**< if set use extend DsNexthop(DsNexthop8w) ,used in create nh */
    CTC_MPLS_NH_IS_UNRSV             = 0x00000002,   /**< if set nexthop is unresolved nexthop ,used in create nh */
    CTC_MPLS_NH_IS_NEXT_APS_EN       = 0x00000004,   /**< if set ,next level aps is  enable */
    CTC_MPLS_NH_IS_APS_USE_SAME_DSNH = 0x00000008    /**<if aps_en is enable ,when protected object isn't user service
                                                          (working and protection path's use same VC label ),it  is set to1*/
};
typedef enum ctc_mpls_nh_operation_type_flag_e ctc_mpls_nh_operation_type_flag_t;

/**
 @brief  Define MPLS nexthop parameter structure
*/
struct ctc_mpls_nexthop_param_s
{
    ctc_mpls_nh_prop_t nh_prop;               /**< MPLS nexthop property associated type --> ctc_mpls_nh_prop_t */
    ctc_nh_upd_type_t upd_type;               /*nexthop update type ,used for update Operation*/
    uint32  dsnh_offset;                      /**< dsNexthop offset,if aps_en is enable ,when protected object is user service
                                                    (working and protection path's use different VC label ),it will occupy 2 entries dsNexthop
                                                     and the offset are dsnh_offset and dsnh_offset+1*/
    uint32   flag;                            /**< flag used in  MPLS nexthop --> ctc_mpls_nh_operation_type_flag_t */
    uint16   aps_bridge_group_id;             /**< the groupid of aps bridge,only used in create a mpls aps nexthop */
    uint8     aps_en;                         /* if set,protection path's Nexthop parameter will be used*/
    uint8     rsv0;                           /**< researve for future */

    union{
        ctc_mpls_nexthop_pop_param_t   nh_param_pop;      /**< mpls (asp) pop nexthop */
        ctc_mpls_nexthop_push_param_t  nh_param_push;     /**< mpls push (asp) nexthop */
        ctc_mpls_nexthop_switch_param_s nh_param_switch;  /**< mpls switch nexthop */
    }  nh_para;     /**< nexthop parameter used to create this nexthop */

    union{
        ctc_mpls_nexthop_pop_param_t   nh_p_param_pop;      /**< mpls asp pop nexthop */
        ctc_mpls_nexthop_push_param_t  nh_p_param_push;     /**< mpls push asp nexthop */
        ctc_mpls_nexthop_switch_param_s nh_p_param_switch;  /**< mpls switch nexthop */
        uint16                      aps_bridge_group_id;    /**<CTC_MPLS_NH_IS_NEXT_APS_EN is set to 1 */
    }  nh_p_para;       /**< protection path's Nexthop parameter used to create this nexthop ,only used in create a mpls aps nexthop */
};
typedef struct ctc_mpls_nexthop_param_s ctc_mpls_nexthop_param_t;


/**
 @brief  Define loopback nexthop parameter structure
*/
struct ctc_loopback_nexthop_param_s
{
    bool customerid_valid;	                /**< If set, indicates the inner payload has an MPLS encapsulation or GRE key, which contains the customer ID as the label */
    bool map_vclabel_exp;	                /**< If set, indicates that the priority and color are mapped from the EXP field in VC label. this flag is only effective in MPLS VPN loopback */
    bool inner_packet_type_valid;           /**< This field indicates if the inner_packet_type is valid*/
    bool vpls_src_port;                     /**< If set, indicates that the packet is from a VPLS tunnel port*/
    bool service_queue_en;                  /**< If set, indicates that the packets will do service queue processing*/
    bool sequence_en;                       /**< If set, indicates that the packets will do sequence order */
    ctc_parser_pkt_type_t inner_packet_type;/**< This field indicates the encapsulated packet type */
    uint8 lpbk_lport;                       /**< Loopback source local physical port, should not be linkagg port*/
    uint8 words_removed_from_hdr;           /**< 1 word stands for 4 bytes, not include customerId 4 bytes;*/
    uint8 sequence_counter_index;           /**< Sequence number counter index */
};
typedef struct ctc_loopback_nexthop_param_s ctc_loopback_nexthop_param_t;


/**
 @brief  Define remote mirror nexthop parameter structure
*/
struct ctc_rspan_nexthop_param_s
{
    uint16 rspan_vid;   /**< RSPAN VLAN ID  */
};
typedef struct ctc_rspan_nexthop_param_s ctc_rspan_nexthop_param_t;


/**@} end of @defgroup  mpls MPLS */

enum ctc_flex_nh_flag_e
{
    CTC_FLEX_NH_REPLACE_MAC_DA      = 0x00000001,   /**< Will replace layer2 header mac da */
    CTC_FLEX_NH_REPLACE_MAC_SA      = 0x00000002,   /**< Will replace layer2 header mac sa */
    CTC_FLEX_NH_REPLACE_SVLAN_TAG   = 0x00000004,   /**< Will replace layer2 header mac da */
    CTC_FLEX_NH_SWAP_MAC            = 0x00000008,   /**< Will swap layer2 header mac da & sa, cannot mix with other types */
    CTC_FLEX_NH_STRIP_SVLAN_TAG     = 0x00000010,   /**< Will strip svlan tag */
    CTC_FLEX_NH_REPLACE_STAG_COS    = 0x00000020,   /**< Will replace stag cos to specified value */
    CTC_FLEX_NH_MAP_STAG_COS        = 0x00000040,   /**< Will replace stag cos, according to priority map */
    CTC_FLEX_NH_INSERT_SVLAN_TAG    = 0x00000080,   /**< Will insert svlan tag */

    CTC_FLEX_NH_REPLACE_CVLAN_TAG   = 0x00000100,   /**< Will replace ctag vid to specified value */
    CTC_FLEX_NH_STRIP_CVLAN_TAG     = 0x00000200,   /**< Will strip cvlan tag */
    CTC_FLEX_NH_COPY_CTAG_COS       = 0x00000400,   /**< Will copy cos from old packet */
    CTC_FLEX_NH_MAP_CTAG_COS        = 0x00000800,   /**< Will replace ctag cos, according to priority map */
    CTC_FLEX_NH_INSERT_CVLAN_TAG    = 0x00001000,   /**< Will insert cvlan tag */

    CTC_FLEX_NH_REPLACE_IPDA        = 0x00002000,   /**< Will replace ipda */
    CTC_FLEX_NH_REPLACE_DST_PORT    = 0x00004000,   /**< Will replace dst port */
};

struct ctc_flex_nh_param_s
{
    uint32       flag;                          /**< CTC_FLEX_NH_XXX */
    uint32       dsnh_offset;                   /**< DsNextHop offset */
    mac_addr_t   mac_da;                        /**< Mac destination address */
    mac_addr_t   mac_sa;                        /**< Mac source address */
    uint16       vlan_id;                       /**< Vlan id */
    uint8        cos;                           /**< Cos */
    uint8        rsv;                           /**< reserved */
    uint16       gport;                         /**< Dest global port id */

    uint16       cvlan_id;                      /**< Vlan id */
    uint8        ctag_cos;                      /**< Cos */
    uint8        rsv1;                          /**< reserved */

    uint16       dst_port;                      /**< dst-port */
    uint32       ipda;                          /**< ipda */
};
typedef struct ctc_flex_nh_param_s ctc_flex_nh_param_t;

/**
 @defgroup Mcast Mcast
 @{
*/

/**
 @brief  Define mcast nexthop operation code
*/
enum ctc_mcast_nh_param_opcode_e
{
    CTC_NH_PARAM_MCAST_NONE                  = 0, /**< None */
    CTC_NH_PARAM_MCAST_ADD_MEMBER            = 1, /**< Add mcast member */
    CTC_NH_PARAM_MCAST_DEL_MEMBER            = 2  /**< Del mcast member */
};
typedef enum ctc_mcast_nh_param_opcode_e ctc_mcast_nh_param_opcode_t;

/**
 @brief  Define mcast nexthop member type
*/
enum ctc_mcast_nh_param_member_type_s
{
    CTC_NH_PARAM_MEM_BRGMC_LOCAL = 0,       /**< Bridge mcast member  */
    CTC_NH_PARAM_MEM_IPMC_LOCAL,            /**< IP mcast member  */
    CTC_NH_PARAM_MEM_LOCAL_WITH_NH,         /**< Mcast member with nexthop */
    CTC_NH_PARAM_MEM_REMOTE,                /**< Remote member  */
    CTC_NH_PARAM_MEM_INVALID                /**< Invalid mcast member  */
};
typedef enum ctc_mcast_nh_param_member_type_s ctc_mcast_nh_param_member_type_t;

/**
 @brief  Define mcast nexthop parameter structure
*/
struct ctc_mcast_nh_param_member_s
{
    ctc_mcast_nh_param_member_type_t member_type;   /**< Mcast member type */
    uint32  ref_nhid;               /**< Reference other nhid, eg egress vlan translation nhid */
    uint16  destid;                 /**< Local member:local portid(gchip(8bit) +local phy port(8bit)), LAGID(eg.0x1F01)/remote chip member: gchip(local) + remote gchip id(8bit) */
    uint16  vid;                    /**< For IPMC */
    uint8   lchip;                  /**< Local chip this member will be added to */
    uint8   l3if_type;              /**< ctc_l3if_type_t */
    uint16  logic_port;             /**< Humber don't support */
    uint8   is_logic_port_chk_en;   /**< Humber don't support. if set,Met will do logic port check,apply to VPLS/CAPWAP/PBB/TRILL,etc. */
    uint8   is_vlan_port;           /**< If set and  l3if is vlan interface , output packet only do L2 bridging . */

    uint8   port_check_discard;     /**< whether discard packets when dest port and src port are same */
    uint8   rsv0;
};
typedef struct ctc_mcast_nh_param_member_s ctc_mcast_nh_param_member_t;

/**
 @brief  Define mcast nexthop parametes structure
*/
struct ctc_mcast_nh_param_group_s
{
    uint16                        mc_grp_id;    /**< Identify a group */
    uint8                         opcode;       /**< ctc_mcast_nh_param_opcode_t */
    uint8                         rsv;
    ctc_mcast_nh_param_member_t   mem_info;     /**< Mcast member info */
};
typedef struct ctc_mcast_nh_param_group_s ctc_mcast_nh_param_group_t;
/**@} end of @defgroup Mcast  */

/**@} end of @defgroup  nexthop */

#endif /*!_CTC_NEXTHOP_H*/

