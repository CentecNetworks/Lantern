/**
 @file ctc_mpls.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2010-03-11

 @version v2.0

   This file contains all mpls related data structure, enum, macro and proto.
*/


 #ifndef _CTC_MPLS_H
 #define _CTC_MPLS_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/**
 @defgroup mpls MPLS
 @{
*/

/**
 @brief  Define mpls label type
*/
enum ctc_mpls_label_type_e
{
    CTC_MPLS_LABEL_TYPE_NORMAL,      /**< This label is a normal label */
    CTC_MPLS_LABEL_TYPE_L3VPN,       /**< This label is a l3vpn VC label */
    CTC_MPLS_LABEL_TYPE_VPWS,        /**< This label is a vpws VC label */
    CTC_MPLS_LABEL_TYPE_VPLS,        /**< This label is a vpls VC label */
    CTC_MPLS_MAX_LABEL_TYPE
};

/**
 @brief  Define mpls tunnel mode
*/
enum ctc_mpls_tunnel_mode_e
{
    CTC_MPLS_TUNNEL_MODE_UNIFORM,      /**< The Tunnel mode of this label is uniform */
    CTC_MPLS_TUNNEL_MODE_SHORT_PIPE,   /**< The Tunnel mode of this label is short pipe */
    CTC_MPLS_TUNNEL_MODE_PIPE,         /**< The Tunnel mode of this label is pipe */
    CTC_MPLS_MAX_TUNNEL_MODE
};

/**
 @brief  Define mpls tunnel mode
*/
enum ctc_mpls_id_type_e
{
    CTC_MPLS_ID_NULL,        /**< NULL */
    CTC_MPLS_ID_FLOW,        /**< The ILM use flow policer */
    CTC_MPLS_ID_VRF,         /**< The ILM use vrfid, only for l3vpn vc label */
    CTC_MPLS_ID_SERVICE,     /**< The ILM use qos, only for l2vpn vc label */
    CTC_MPLS_ID_APS_SELECT,  /**< The aps select group id, only for l2vpn vc label */
    CTC_MPLS_ID_STATS,       /**< The ILM use stats */
    CTC_MPLS_MAX_ID
};

/**
 @brief  Define mpls pw mode
*/
enum ctc_mpls_vpws_pw_mode_e
{
    CTC_MPLS_VPWS_TAGGED,      /**< The PW mode is TAGGED */
    CTC_MPLS_VPWS_RAW,         /**< The PW mode is RAW */
    CTC_MPLS_MAX_VPWS_MODE
};

/**
 @brief  Define mpls ac bind type
*/
enum ctc_mpls_ac_bind_type_e
{
    CTC_MPLS_BIND_ETHERNET,      /**< The AC bind type is port */
    CTC_MPLS_BIND_SVLAN,         /**< The customer vlan is svlan */
    CTC_MPLS_BIND_CVLAN,         /**< The customer vlan is cvlan */
    CTC_MPLS_MAX_BIND_TYPE
};

/**
 @brief  Define l2vpn type
*/
enum ctc_mpls_l2vpn_type_e
{
    CTC_MPLS_L2VPN_VPWS,      /**< The L2VPN is VPWS */
    CTC_MPLS_L2VPN_VPLS,      /**< The L2VPN is VPLS */
    CTC_MPLS_MAX_L2VPN_TYPE
};

/**
 @brief  Define mpls stats index
*/
struct ctc_mpls_stats_index_s
{
    uint32 label;   /**< MPLS label */
    uint8 spaceid;  /**< Label space id */
};
typedef struct ctc_mpls_stats_index_s ctc_mpls_stats_index_t;

/**
 @brief  Define mpls parameter structure
*/
struct ctc_mpls_ilm_s
{
    uint32 label;                 /**< In label */
    uint32 nh_id;                 /**< Nexthop ID */
    uint16 pwid;                  /**< PW ID, VPLS use PW ID to bridge packets in instance, 0xffff means not used */
    union
    {
        uint16 flow_id;           /**< The ILM use flow policer */
        uint16 vrf_id;            /**< The ILM use vrfid, only for l3vpn vc label */
        uint16 service_id;        /**< The ILM use qos, only for l2vpn vc label */
        uint16 aps_select_grp_id; /**< The aps select group id, only only for l2vpn vc label */
    }flw_vrf_srv_aps;

    uint8 id_type;                /**< flw_vrf_srv_aps value with which type */
    uint8 spaceid;                /**< Label space ID, platform space ID is 0 */
    uint8 type;                   /**< Label type, should be one of the ctc_mpls_label_type_e value */
    uint8 model;                  /**< Tunnel mode, should be one of the ctc_mpls_tunnel_mode_e value */
    uint8 cwen;                   /**< Control word enable, if label is a l2vpn vc label, the PW control word function maybe enable */
    uint8 pop;                    /**< Whether the label is POP label or not, only used when label type is normal */
    uint8 aps_select_protect_path;/** If id_type is CTC_MPLS_ID_APS_SELECT and it is set, indicate the path is aps protect path,else the path is working path */
    uint8 vpls_port_type;         /**< The VPLS PW is a H-VPLS tunnel */
    uint8 oam_en;                 /**< Enable OAM */
};
typedef struct ctc_mpls_ilm_s ctc_mpls_ilm_t;

/**
 @brief  Define l2vpn pw parameter structure
*/
struct ctc_mpls_l2vpn_pw_s
{
    uint32 label;                   /**< In label */
    uint8 l2vpntype;                /**< ctc_mpls_l2vpn_type_e,L2VPN type, VPWS or VPLS */
    uint8 learn_disable;            /** < VPLS learning disable, 0:learning enable, 1:learning disable>*/
    uint8 maclimit_enable;          /** < VPLS mac limit enable*/
    uint8 service_aclqos_enable;    /** < enable service aclqos>*/
    union
    {
        uint32 pw_nh_id;            /**< Nexthop ID of PW, should be a vpws nexthop */
        struct
        {
            uint16 fid;             /**< VPLS ID */
            uint16 vpls_src_port;   /**< VPLS source port, 0xffff means no used */
            uint8  vpls_port_type;  /**< VPLS PW is a VPLS tunnel or H-VPLS tunnel */
        } vpls_info;
    } u;
};
typedef struct ctc_mpls_l2vpn_pw_s ctc_mpls_l2vpn_pw_t;

/**
 @brief  Define vpls pw parameter structure
*/
struct ctc_mpls_pw_s
{
    uint32 vc_label;    /**< VC label of vpls PW */
    uint32 fid;         /**< VPLS ID */
};
typedef struct ctc_mpls_pw_s ctc_mpls_pw_t;

/**
 @brief  Define mpls label space information structure
*/
struct ctc_mpls_space_info_s
{
    uint8 enable;       /**< Whether the label space is valid */
    uint8 sizetype;     /**< The label space size =  256 * (2 ^ sizetype) */
};
typedef struct ctc_mpls_space_info_s ctc_mpls_space_info_t;

/**
 @brief  Define mpls init information structure
*/
struct ctc_mpls_init_s
{
    ctc_mpls_space_info_t space_info[CTC_MPLS_SPACE_NUMBER];    /**< Label space information array */
};
typedef struct ctc_mpls_init_s ctc_mpls_init_t;

/**@} end of @defgroup mpls MPLS */

#endif  /*_CTC_MPLS_H*/

