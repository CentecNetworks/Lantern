/**
 @file ctc_ftm.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-11

 @version v2.0

   This file contains all memory allocation related data structure, enum, macro and proto.
*/

#ifndef _CTC_FTM_H
#define _CTC_FTM_H

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define MAX_CTC_FTM_KEY_TYPE_SIZE (2 * CTC_FTM_KEY_TYPE_MAX)

/**
 @defgroup allocation ALLOCATION
 @{
*/

/**
 @brief Define allocation key location
*/
enum ctc_ftm_key_location_e
{
    CTC_FTM_INT_TCAM,         /**<Location in internal TCAM*/
    CTC_FTM_EXT_TCAM,         /**<Location in external TCAM*/
    CTC_FTM_HASH,             /**<Location in SRAM*/
    CTC_FTM_LOCATION_MAX
};
typedef enum ctc_ftm_key_location_e ctc_ftm_key_location_t;

/**
 @brief Define key type
*/
enum ctc_ftm_key_type_e
{
    CTC_FTM_KEY_TYPE_IPV6_UCAST,         /**<Ipv6 unicast route key, must be first enum */
    CTC_FTM_KEY_TYPE_IPV4_UCAST,         /**<Ipv4 unicast route key*/
    CTC_FTM_KEY_TYPE_IPV4_MCAST,         /**<Ipv4 multicast route key*/
    CTC_FTM_KEY_TYPE_IPV4_NAT,           /**<Ipv4 nat key*/
    CTC_FTM_KEY_TYPE_IPV4_PBR,           /**<Ipv4 pbr key*/
    CTC_FTM_KEY_TYPE_USERID_MAC,         /**<User id mac key*/
    CTC_FTM_KEY_TYPE_USERID_VLAN,        /**<User id vlan key*/
    CTC_FTM_KEY_TYPE_IPV6_MCAST,         /**<Ipv6 multicast route key*/
    CTC_FTM_KEY_TYPE_IPV6_NAT,           /**<Ipv6 nat key*/
    CTC_FTM_KEY_TYPE_FIB,                /**<Mac key*/
    CTC_FTM_KEY_TYPE_IPV6_PBR,           /**<Ipv6 pbr key*/
    CTC_FTM_KEY_TYPE_USERID_IPV4,        /**<User id ipv4 key*/
    CTC_FTM_KEY_TYPE_USERID_IPV6,        /**<User id ipv6 key*/
    CTC_FTM_KEY_TYPE_OAM,                /**<Oam key*/

    /*humber*/
    CTC_FTM_KEY_TYPE_ACL_IPV6,           /**<Acl ipv6 key*/
    CTC_FTM_KEY_TYPE_ACL_MAC_IPV4,       /**<Acl mac ipv4 mpls key*/
    CTC_FTM_KEY_TYPE_QOS_MAC_IPV4,       /**<Qos mac ipv4 mpls key*/
    CTC_FTM_KEY_TYPE_QOS_IPV6,           /**<Qos ipv6 key*/


    CTC_FTM_KEY_TYPE_RSV,
    CTC_FTM_KEY_TYPE_MAX
};
typedef enum ctc_ftm_key_type_e ctc_ftm_key_type_t;

/**
 @brief Define key size
*/
enum ctc_ftm_key_size_e
{
    CTC_FTM_KEY_SIZE_INVALID = 0,   /**<Invalid key size*/
    CTC_FTM_KEY_SIZE_80_BIT  = 1,    /**<80 bits key size*/
    CTC_FTM_KEY_SIZE_160_BIT = 2,   /**<160 bits key size*/
    CTC_FTM_KEY_SIZE_320_BIT = 4,   /**<320 bits key size*/
    CTC_FTM_KEY_SIZE_640_BIT = 8,   /**<640 bits key size*/
    CTC_FTM_KEY_SIZE_MAX
};
typedef enum ctc_ftm_key_size_e ctc_ftm_key_size_t;

/**
 @brief Define TCAM type
*/
enum ctc_ftm_tcam_type_e
{
    CTC_FTM_TCAM_TYPE_NL9K = 2, /**< Netlogic 9000 TCAM type*/
    CTC_FTM_TCAM_TYPE_MAX,
};
typedef enum ctc_ftm_tcam_type_e ctc_ftm_tcam_type_t;

/**
 @brief Profile key information
*/
struct ctc_ftm_key_info_s
{
    uint8  key_size;                    /**< Value = {1,2,4,8}, indicates {80b,160b,320b,640b}. */
    uint32 max_key_index;               /**< Key total number. key_max_index * key_size = consumed 80b tcam entry. */
    uint8  key_media;                   /**< CTC_ALLOC_XXX*/
    uint8  key_id;                      /**< Key type*/
};
typedef struct ctc_ftm_key_info_s ctc_ftm_key_info_t;


/**
 @brief Define tble type
*/
enum ctc_ftm_tbl_type_e
{
    CTC_FTM_TBL_TYPE_MPLS,

    /*humber*/
    CTC_FTM_TBL_TYPE_GLB_MET,       /**< store global MET table's sram size */
    CTC_FTM_TBL_TYPE_GLB_NH,        /**< store global Nexthop table's sram size */
    CTC_FTM_TBL_TYPE_LOCAL_NH,      /**< store local Nexthop table's sram size */
    CTC_FTM_TBL_TYPE_L2EDIT,        /**< store l2edit table's sram size */
    CTC_FTM_TBL_TYPE_L3EDIT,        /**< store l3edit table's sram size */
    CTC_FTM_TBL_TYPE_IP_TUNNEL,     /**< store IP-tunnel table's sram size*/
    CTC_FTM_TBL_TYPE_HASH,          /**< Optional hash size, 48K or 96K */
    CTC_FTM_TBL_TYPE_ASS_DYN_SRAM,  /**< Internal Sram for associate and dynamic table */
    CTC_FTM_TBL_TYPE_EXT_TCAM_TYPE, /**< External TCAM hardware type, default NL9K */
    CTC_FTM_TBL_TYPE_EXT_TCAM_SIZE, /**< External external tcam size, 256K */
    CTC_FTM_TBL_TYPE_EXT_SRAM_SIZE, /**< External sram size */
    CTC_FTM_TBL_TYPE_FLOW_PLOCIER,  /**< store policer table's sram size*/
    CTC_FTM_TBL_TYPE_FWD_STATS,     /**< store forward statistics table's sram size*/


    CTC_FTM_TBL_TYPE_MAX
};
typedef enum ctc_ftm_tbl_type_e ctc_ftm_tbl_type_t;

/**
 @brief Define tble type
*/
enum ctc_ftm_flag_e
{
    CTC_FTM_FLAG_IPUC_SA_EN             = (1<<0), /**< Enable rpf */
    CTC_FTM_FLAG_ACL_QOS_DUAL_LKUP      = (1<<1), /**< Enable lookup acl/qos key at the same time */
    CTC_FTM_FLAG_ACL_QOS_MERGE_MAC_IP   = (1<<2), /**< Enable merge acl/qos mac/ip Key */
    CTC_FTM_FLAG_MPLS_IN_EXT_SRAM       = (1<<3), /**< Mpls table store in external sram or not */
    CTC_FTM_FLAG_MET_IN_EXT_SRAM        = (1<<4), /**< MET table store in external sram or not */
    CTC_FTM_FLAG_NH_IN_EXT_SRAM         = (1<<5), /**< Nexthop table store in external sram or not*/
    CTC_FTM_FLAG_L2EDIT_IN_EXT_SRAM     = (1<<6), /**< L2edit table store in external sram or not */
    CTC_FTM_FLAG_L3EDIT_IN_EXT_SRAM     = (1<<7), /**< L3edit table store in external sram or not*/
    CTC_FTM_FLAG_EXT_QDR_EN             = (1<<8), /**< Enable external QDR*/
    CTC_FTM_FLAG_DISABLE_MERGE_MAC_IP_KEY_PHYSICAL = (1<<9), /**< If disable merge mac ip key physical, 
                  mac key size is half of ipv4 key size, and not support ext tcam, not support acl key*/
    CTC_FTM_FLAG_MAX
};
typedef enum ctc_ftm_flag_e ctc_ftm_flag_t;

/**
 @brief Define profile type
*/
enum ctc_ftm_profile_type_e
{
    CTC_FTM_PROFILE_DEFAULT,
    CTC_FTM_PROFILE_BRIDGE,
    CTC_FTM_PROFILE_ROUTE,
    CTC_FTM_PROFILE_BRIDGE_ROUTE,
    CTC_FTM_PROFILE_METRO_BRIDGE,
    CTC_FTM_PROFILE_METRO_MPLS,
    CTC_FTM_PROFILE_ROUTE_TST,      /* 128K route for test*/
    CTC_FTM_PROFILE_MAX
};
typedef enum ctc_ftm_profile_type_e ctc_ftm_profile_type_t;


/**
 @brief Profile information
*/
struct ctc_ftm_profile_info_s
{
    uint32 flag;                    /**<CTC_FTM_FLAG_XXX */
    ctc_ftm_key_info_t *key_info;   /**<Profile key information*/
    uint16 key_info_size;           /**< Size of key_info, multiple of sizeof(ctc_ftm_key_info_t) */
    uint8 profile_type;
    uint8  rsv0;

    uint32 table_size[CTC_FTM_TBL_TYPE_MAX];
};
typedef struct ctc_ftm_profile_info_s ctc_ftm_profile_info_t;

/**@} end of @defgroup allocation ALLOCATION  */


#endif
