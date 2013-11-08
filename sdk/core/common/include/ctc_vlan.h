/**
 @file ctc_vlan.h

 @date 2009-12-11

 @version v2.0

 The file define structure used in Humber SDK API.
*/
#ifndef _CTC_VLAN_H
#define _CTC_VLAN_H

#include "ctc_const.h"
#include "ctc_parser.h"

#define CTC_VLAN_MISS_ACTION_DO_NOTHING     (1 << 0)    /**<default action do nothing, will be forwarding normal*/
#define CTC_VLAN_MISS_ACTION_DISCARD        (1 << 1)    /**<default action discard, will be discard*/
#define CTC_VLAN_MISS_ACTION_TO_CPU         (1 << 2)    /**<default action to cpu, will be fowarding to cpu*/
#define CTC_VLAN_MISS_ACTION_DEF_SVLAN      (1 << 3)    /**<default action output svlan id*/
#define CTC_VLAN_MISS_ACTION_DEF_CVLAN      (1 << 4)    /**<default action output cvlan id*/
#define CTC_VLAN_MISS_ACTION_SERVICE_QUEUE  (1 << 5)    /**<default action output service queue infomation*/

/*****
 *@defgroup vlan VLAN
 *@{
 *****/
enum ctc_exception_type_e
{
    CTC_EXCP_FWD_AND_TO_CPU,    /**< forwarding and send to cpu */
    CTC_EXCP_NORMAL_FWD,        /**< normal forwarding */
    CTC_EXCP_DISCARD_AND_TO_CPU,/**< discard and send to cpu */
    CTC_EXCP_DISCARD,           /**< discard */
};
typedef enum ctc_exception_type_e ctc_exception_type_t;


/**
 @defgroup VlanMapping VlanMapping
 @{
*/
/**
 @brief define key type for QinQ application
*/
enum ctc_vlan_mapping_key_e
{
    CTC_VLAN_MAPPING_KEY_NONE               = 0x00000000,         /**< invalid key*/
    CTC_VLAN_MAPPING_KEY_CVID               = 0x00000001,         /**< use cvid as key*/
    CTC_VLAN_MAPPING_KEY_SVID               = 0x00000002,         /**< use svid as key*/
    CTC_VLAN_MAPPING_KEY_CTAG_COS           = 0x00000004,         /**< use ctag cos as key*/
    CTC_VLAN_MAPPING_KEY_STAG_COS           = 0x00000008,         /**< use stag cos as key*/
    CTC_VLAN_MAPPING_KEY_ONLY_PORT          = 0x00000010,         /**< only use port as key*/
    CTC_VLAN_MAPPING_KEY_MAC_SA             = 0x00000020,         /**< use mac sa as key*/
    CTC_VLAN_MAPPING_KEY_MAC_DA             = 0x00000040,         /**< use mac da as key*/
    CTC_VLAN_MAPPING_KEY_IPV4_SA            = 0x00000080,         /**< use ipv4 sa as key*/
    CTC_VLAN_MAPPING_KEY_IPV4_DA            = 0x00000100,         /**< use ipv4 da as key*/
    CTC_VLAN_MAPPING_KEY_L2_TYPE            = 0x00000200,         /**< use l2 type as key*/
    CTC_VLAN_MAPPING_KEY_L3_TYPE            = 0x00000400,         /**< use l3 type as key*/
    CTC_VLAN_MAPPING_KEY_L4_SRCPORT         = 0x00000800,         /**< use l2 type as key*/
    CTC_VLAN_MAPPING_KEY_L4_DSTPORT         = 0x00001000,         /**< use l3 type as key*/


};
typedef enum ctc_vlan_mapping_key_e ctc_vlan_mapping_key_t;

/**
 @brief vlan mapping output used on ipe_usrid
*/
enum ctc_vlan_mapping_action_e
{
    CTC_VLAN_MAPPING_OUTPUT_SVID             = 0x00000001,   /**<output svid in mapping result*/
    CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT    = 0x00000002,   /**<output vpls source port in mapping result*/
    CTC_VLAN_MAPPING_OUTPUT_APS_SELECT       = 0x00000004,   /**<output aps select infomation in mapping result*/

    CTC_VLAN_MAPPING_OUTPUT_FID              = 0x00000008,   /**<output fid in mapping result*/
    CTC_VLAN_MAPPING_OUTPUT_NHID             = 0x00000010,   /**<output nexthop in mapping result*/

    CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID       = 0x00000020,   /**<output service ID of HQoS in mapping result*/
    CTC_VLAN_MAPPING_OUTPUT_CVID             = 0x00000040,   /**<output cvid in mapping result*/
    CTC_VLAN_MAPPING_OUTPUT_VLANPTR          = 0x00000080,   /**<output VlanPtr in mapping result*/




    CTC_VLAN_MAPPING_FLAG_VPWS               = 0x00001000,   /**<flag to indicate vpws application*/
    CTC_VLAN_MAPPING_FLAG_VPLS               = 0x00002000,   /**<flag to indicate vpls application*/
    CTC_VLAN_MAPPING_FLAG_VPLS_LRN_DIS       = 0x00004000,   /**<flag to indicate vpls learning will be disable*/
    CTC_VLAN_MAPPING_FLAG_MACLIMIT_EN        = 0x00008000,   /**<flag to indicate vpls learning will be disable*/

    CTC_VLAN_MAPPING_FLAG_SERVICE_ACL_EN     = 0x00010000,   /**<enable service acl of HQoS in mapping result*/
    CTC_VLAN_MAPPING_FLAG_SERVICE_POLICER_EN = 0x00020000,   /**<enable service policer of HQoS in mapping result*/



};
typedef enum ctc_vlan_mapping_action_e ctc_vlan_mapping_action_t;






/**
 @brief define data structure used in APIs
*/
struct ctc_vlan_mapping_s
{

/****************************************
 *key :
 */

    ctc_vlan_mapping_key_t    key;          /**<select key(s) to mapping*/

    uint16 old_cvid;                        /**<old cvid*/
    uint16 old_svid;                        /**<old svid*/
    uint16 cvlan_end;                       /**<cvlan end, for ingress only*/
    uint16 svlan_end;                       /**<svlan end, for ingress only*/

    uint8  old_ccos;                        /**<old ctag cos*/
    uint8  old_scos;                        /**<old stag cos*/



/****************************************
 *action :
 */

    ctc_vlan_mapping_action_t action;       /**<action of vlan mapping*/





    union
    {
        uint16 new_svid;                /**<new svid append on packet*/
        uint16 vpls_src_port;           /**<vpls source port*/
        uint16 aps_select_group_id;     /**<aps selector group id*/
    }u1;

    union
    {
        uint16 service_id;              /**<service id binding to the packet*/
        uint16 new_cvid;                /**<new cvid append on packet*/
    }u2;


    union
    {
        uint16 fid;                     /**<forwarding instance, maybe EVC concept on MEF10*/
        uint32 nh_id;                   /**<nexthop id, need create nexthop first*/
    }u3;





    bool is_igs_srvq;                   /**<service queue direction*/
    bool is_working_path;               /**<indicate the flow is working path or protecting path, used in APS*/
    uint16 protected_vlan;              /**<protected vlan, used in APS, general equal to vlan of working path*/
};
typedef struct ctc_vlan_mapping_s ctc_vlan_mapping_t;


/**
 @brief define data structure used in APIs
*/
struct ctc_flex_vlan_mapping_s
{

/****************************************
 *key :
 */

    ctc_vlan_mapping_key_t    key;          /**<select key(s) to mapping*/

    uint16 old_cvid;                        /**<old cvid*/
    uint16 old_svid;                        /**<old svid*/
    uint16 cvlan_end;                       /**<cvlan end, for ingress only*/
    uint16 svlan_end;                       /**<svlan end, for ingress only*/

    uint8  old_ccos;                        /**<old ctag cos*/
    uint8  old_scos;                        /**<old stag cos*/





    mac_addr_t macsa;                   /**< macsa as key   for flexiable vlan mapping*/
    mac_addr_t macda;                   /**< macda as key   for flexiable vlan mapping*/
    mac_addr_t macsa_mask;              /**< macsa mask     for flexiable vlan mapping*/
    mac_addr_t macda_mask;              /**< macda mask     for flexiable vlan mapping*/

    ip_addr_t ipv4_sa;                  /**< ipv4 sa as key        for flexiable vlan mapping*/
    ip_addr_t ipv4_smask;               /**< ipv4 sa mask as key   for flexiable vlan mapping*/
    ip_addr_t ipv4_da;                  /**< ipv4 da as key        for flexiable vlan mapping*/
    ip_addr_t ipv4_dmask;               /**< ipv4 da mask as key   for flexiable vlan mapping*/

    uint16 l4src_port;                  /**< l4src_port as key for flexiable vlan mapping*/
    uint16 l4dest_port;                 /**< l4dest_port as key for flexiable vlan mapping*/

    uint8 l2_type;                      /**< l2 type as key for flexiable vlan mapping*/
    uint8 l3_type;                      /**< l3 type as key for flexiable vlan mapping*/


/****************************************
 *action :
 */

    ctc_vlan_mapping_action_t action;       /**<action of vlan mapping*/


    uint16  vlanptr;                      /**< User defined Vlan pointer, the vlan_ptr will be used to set or get vlan property*/

    union
    {
        uint16 new_svid;                /**<new svid append on packet*/
        uint16 vpls_src_port;           /**<vpls source port*/
        uint16 aps_select_group_id;     /**<aps selector group id*/
    }u1;

    union
    {
        uint16 service_id;              /**<service id binding to the packet*/
        uint16 new_cvid;                /**<new cvid append on packet*/
    }u2;


    union
    {
        uint16 fid;                     /**<forwarding instance, maybe EVC concept on MEF10*/
        uint32 nh_id;                   /**<nexthop id, need create nexthop first*/
    }u3;




    bool is_igs_srvq;                   /**<service queue direction*/
    bool is_working_path;               /**<indicate the flow is working path or protecting path, used in APS*/
    uint16 protected_vlan;              /**<protected vlan, used in APS, general equal to vlan of working path*/
};
typedef struct ctc_flex_vlan_mapping_s ctc_flex_vlan_mapping_t;
/**
 @brief define action of default entry of vlan mapping and vlan class
*/
struct ctc_vlan_miss_s
{
    uint16 svlan_id;                /**<output the designated svlan id*/
    uint16 cvlan_id;                /**<output the designated cvlan id*/
    uint16 service_id;              /**<service id binding to the packet*/
    uint8 flag;                     /**<flag*/
    bool is_igs_srvq;               /*service queue direction*/
};
typedef struct ctc_vlan_miss_s ctc_vlan_miss_t;

/**@} end of @defgroup VlanMapping */

/**
 @defgroup VlanClassification VlanClassification
 @{
*/


/**
 @brief enum value used in vlan classification APIs
*/
enum ctc_vlan_class_type_e
{
    CTC_VLAN_CLASS_MAC,                 /**<mac based vlan*/
    CTC_VLAN_CLASS_IPV4,                /**<ipv4/flow based vlan*/
    CTC_VLAN_CLASS_IPV6,                /**<ipv6/flow based vlan*/
    CTC_VLAN_CLASS_PROTOCOL,            /**<protocol based vlan*/
    CTC_VLAN_CLASS_MAX
};
typedef enum ctc_vlan_class_type_e ctc_vlan_class_type_t;




/**
 @brief the structure of vlan classification
*/
struct ctc_vlan_class_s
{
    ctc_vlan_class_type_t type;                 /**<vlan classification type*/


/*=====remove policy_id?? can stay.. if remove it, sacl must add a label_valid to mask label.======*/
    uint8  port_grpid;                          /**<port_group id, port in this group share same props*/
/*==action==*/
    uint8  is_svlan;                            /**<if vlan is svlan is_svlan == 1, else is_svlan == 0*/
    uint16 vlan_id;                             /**<vlan id*/
    union
    {
        struct
        {
            mac_addr_t macsa;                   /**<mac source address*/
            mac_addr_t macda;                   /**<mac destination address*/
            mac_addr_t macsa_mask;              /**<mac sa mask*/
            mac_addr_t macda_mask;              /**<mac da mask*/
            ctc_parser_l3_type_t l3_type;       /**<layer3 parser type*/
        }vlan_mac;                              /**<mac based vlan*/

        struct
        {
            ip_addr_t ipv4_sa;                  /**<IPv4 source address*/
            ip_addr_t ipv4_smask;               /**<IPv4 sa mask*/
            ip_addr_t ipv4_da;                  /**<IPv4 destination address*/
            ip_addr_t ipv4_dmask;               /**<IPv4 da mask*/
            mac_addr_t macsa;                   /**<mac source address*/
            mac_addr_t macda;                   /**<mac destination address*/
            mac_addr_t macsa_mask;              /**<mac sa mask*/
            mac_addr_t macda_mask;              /**<mac da mask*/
            uint16 l4src_port;                  /**<layer4 source port*/
            uint16 l4dest_port;                 /**<layer4 destination port*/
            ctc_parser_l3_type_t l3_type;       /**<layer3 parser type*/
            ctc_parser_l4_type_t l4_type;       /**<layer4 parser type*/
        }vlan_ipv4;                             /**<ipv4 based vlan*/

        struct
        {
            ipv6_addr_t ipv6_sa;                /**<IPv6 source address*/
            ipv6_addr_t ipv6_smask;             /**<IPv6 sa mask*/
            ipv6_addr_t ipv6_da;                /**<IPv6 destination address*/
            ipv6_addr_t ipv6_dmask;             /**<IPv6 da mask*/
            mac_addr_t macsa;                   /**<mac source address*/
            mac_addr_t macda;                   /**<mac destination address*/
            mac_addr_t macsa_mask;              /**<mac sa mask*/
            mac_addr_t macda_mask;              /**<mac da mask*/
            uint16 l4src_port;                  /**<layer4 source port*/
            uint16 l4dest_port;                 /**<layer4 parser type*/
            ctc_parser_l4_type_t l4_type;       /**<layer4 parser type*/
        }vlan_ipv6;                             /**<ipv6 based vlan*/


        struct
        {
            ctc_parser_l3_type_t l3_type;       /**<layer3 parser type [humber only]*/
        }vlan_protocol;


    }vlan_class;            /**<vlan classification key infomation*/

};
typedef struct ctc_vlan_class_s ctc_vlan_class_t;
/**@} end of @defgroup VlanClassification*/


/**
 @brief Define bits map of port in vlan
*/
struct ctc_port_bitmap_s
{
    uint8 port_bitmap[CTC_PORT_BITMAP_IN_BYTE];       /**<bits map of port in vlan*/
};
typedef struct ctc_port_bitmap_s ctc_port_bitmap_t;





/**
 @brief  define the mode of vlanPtr
*/
enum ctc_vlanptr_mode_e
{
    CTC_VLANPTR_MODE_VLANID ,                   /*set or get vlan property by vlan id*/
    CTC_VLANPTR_MODE_USER_DEFINE    /*set or get vlan property by user defined vlanPtr*/
};
typedef enum ctc_vlanptr_mode_e  ctc_vlanptr_mode_t;

/**
 @brief l2 fdb global config infomation
*/
struct ctc_vlan_global_cfg_s
{
    uint8    vlanptr_mode;  /* ctc_vlanptr_mode_t  */
    uint8    rsv0;
    uint16   rsv1;

};
typedef struct ctc_vlan_global_cfg_s ctc_vlan_global_cfg_t;

/**@} end of @defgroup vlan*/
#endif


