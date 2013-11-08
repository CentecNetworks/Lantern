/**
 @file ctc_mirror.h

 @date 2010-1-7

 @version v2.0

 This file define the types used in APIs

*/

#ifndef _MIRROR_H
#define _MIRROR_H
#ifdef __cplusplus
extern "C" {
#endif

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
 @defgroup mirror MIRROR
 @{
*/
/**
 @brief define max session id
*/
#define MAX_CTC_MIRROR_SESSION_ID  4
/**
 @brief Denote setting session
*/
enum ctc_mirror_session_type
{
    CTC_MIRROR_L2SPAN_SESSION = 0,    /**<mirror l2span session*/
    CTC_MIRROR_L3SPAN_SESSION = 1,    /**<mirror l3span session*/
    CTC_MIRROR_ACLLOG_SESSION = 2,    /**<mirror acllog session*/
    CTC_MIRROR_QOSLOG_SESSION = 3,    /**<mirror qoslog session*/
    MAX_CTC_MIRROR_SESSION_TYPE =4
};
typedef enum ctc_mirror_session_type ctc_mirror_session_type_e;


/**
 @brief Denote setting port/vlan mirror drop packet
*/
enum ctc_mirror_discard_e
{
    CTC_MIRROR_QOSLOG_DISCARD = 0x1,    /**<mirror vlan session drop packet*/
    CTC_MIRROR_ACLLOG_DISCARD = 0x2,    /**<mirror vlan session drop packet*/
    CTC_MIRROR_L3SPAN_DISCARD = 0x4,    /**<mirror vlan session drop packet*/
    CTC_MIRROR_L2SPAN_DISCARD = 0x8     /**<mirror port session drop packet*/
};
typedef enum ctc_mirror_discard_e ctc_mirror_discard_t;

/**
 @brief Define mirror source infomation used in Mirror API
*/
struct ctc_mirror_dest_s
{
    uint8   session_id;               /**<mirror session id, range<0-3>; for aclqos log, range<0-2>*/
    uint8   is_rspan;
    uint8   vlan_valid;
    uint16 dest_gport;              /**<mirror destination global port*/
    union
    {
        uint16 vlan_id;              /**< rspan over L2*/
        uint32 nh_id;                 /**< rspan edit info by nexthop*/
    }rspan;
    ctc_mirror_session_type_e type; /**<mirror session type*/
    ctc_direction_t dir;            /**<direction of mirror, ingress or egress*/
};
typedef struct ctc_mirror_dest_s ctc_mirror_dest_t;

/**
 @brief Defien rspan escape info, mirrored packet with this info will be discard
*/
struct ctc_mirror_rspan_escape_s
{
    mac_addr_t mac0;            /**<mac0 value,all should be 0 if no care*/
    mac_addr_t mac_mask0;       /**<mac0 mask, all should be 0xFF if no care*/
    mac_addr_t mac1;            /**<mac1 value,all should be 0 if no care*/
    mac_addr_t mac_mask1;       /**<mac1 mask, all should be 0xFF if no care*/
};
typedef struct ctc_mirror_rspan_escape_s ctc_mirror_rspan_escape_t;

/**@} end of @defgroup mirror  */


#ifdef __cplusplus
}
#endif

#endif
