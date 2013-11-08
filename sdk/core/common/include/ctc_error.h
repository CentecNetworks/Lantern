 /****************************************************************************
 *file ctc_error.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

 The file define  CENTEC SDK  error code
 ****************************************************************************/

#ifndef _CTC_ERROR_H
#define _CTC_ERROR_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/**
 @defgroup error ERROR
 @{
*/

/**
 @brief  define error type
*/
enum ctc_err
{
    CTC_E_NONE = 0,                /**< NO error */

    /**< generic */
    CTC_E_ENTRY_EXIST = -30000,    /**< entry exist */
    CTC_E_ENTRY_NOT_EXIST,         /**< entry not exist */
    CTC_E_EXCEED_MAX_SIZE,         /**< exceed max size */
    CTC_E_EXCEED_MIN_SIZE,         /**< exceed min size */
    CTC_E_INVALID_PARAM,           /**< invalid parameter */
    CTC_E_INVALID_PTR,             /**< invalid pointer */
    CTC_E_INVALID_DIR,             /**< invalid direction */
    CTC_E_NO_MEMORY,               /**< no memory */
    CTC_E_NOT_INIT,                /**< not init */
    CTC_E_CANT_CREATE_AVL,         /**< cant create avl tree */
    CTC_E_UNEXPECT,                /**< unexpect value/result */
    CTC_E_LESS_ZERO,               /**< less zero */
    CTC_E_NO_OFFSET_LEFT,          /**< no offset left */
    CTC_E_MEMBER_EXIST,            /**< member exist */
    CTC_E_MEMBER_NOT_EXIST,        /**< member not exist */
    CTC_E_INVALID_EXP_VALUE,       /**< invalid exp value */
    CTC_E_INVALID_MPLS_LABEL_VALUE,/**< invalid mpls label value */
    CTC_E_NO_ALLOC_OFFSET,         /**< no alloc offset */
    CTC_E_INVALID_REG,             /**< invalid reg id */
    CTC_E_INVALID_TBL,             /**< invalid tbl id */
    CTC_E_GLOBAL_CONFIG_CONFLICT,  /**< conflict global config */
    CTC_E_NOT_SUPPORT,             /**< Not supported API */
    CTC_E_INVALID_TTL,

    /**< MUTEX */
    CTC_E_FAIL_CREATE_MUTEX = -29900,   /**< fail create mutex */
    CTC_E_MUTEX_BUSY,                   /**< mutex busy */

    /**< Chip,port */
    CTC_E_INVALID_CHIP_NUM = -29800,        /**< invalid chip num */
    CTC_E_INVALID_GLOBAL_CHIPID,   /**< invalid global chip id */
    CTC_E_INVALID_LOCAL_CHIPID,    /**< invalid local chip id */
    CTC_E_CHIP_IS_LOCAL,           /**< chip is local */
    CTC_E_CHIP_IS_REMOTE,          /**< chip is remote */
    CTC_E_INVALID_GLOBAL_PORT,     /**< invalid global port */
    CTC_E_MEMBER_PORT_EXIST,       /**< member port exist */
    CTC_E_MEMBER_PORT_NOT_EXIST,   /**< member port not exist */
    CTC_E_LOCAL_PORT_NOT_EXIST,    /**< local port not exist */
    CTC_E_ALREADY_PHY_IF_EXIST,    /**< already physical interface exist */
    CTC_E_ALREADY_SUB_IF_EXIST,    /**< already sub interface exist */
    CTC_E_PORT_HAS_OTHER_FEATURE,  /**< port has other feature */
    CTC_E_PORT_FEATURE_MISMATCH,   /**< port feature mismatch */
    CTC_E_INVALID_LOCAL_PORT,      /**< invalid local port */
    CTC_E_INVALID_PORT_MAC_TYPE,   /**< invalid port mac type */
    CTC_E_INVALID_PORT_SPEED_MODE, /**< invalid port speed mode */
    CTC_E_INVALID_PREAMBLE,        /**< invalid preamble */
    CTC_E_INVALID_TX_THRESHOLD,    /**< invalid transmit threshold */

	/**< Vlan */
    CTC_E_EXCEED_MAX_VLANCTL = -29700,    /**< exceed max vlanctl */
    CTC_E_EXCEED_MAX_PHY_PORT,
    CTC_E_INVALID_VLAN_ID,                /**< invalid vlan id */
    CTC_E_INVALID_FID_ID,                 /**< invalid fid id */
    CTC_E_INVALID_VRFID,                  /**< invalid vrfid */
    CTC_E_INVALID_PROFILE_ID,
    CTC_E_VLAN_EXIST,                     /**< vlan exist */
    CTC_E_VLAN_NOT_CREATE,                /**< vlan not create */
    CTC_E_VLAN_FILTER_PORT,               /**< port in vlan filter is invalid*/
    CTC_E_VLAN_CLASS_INVALID_TYPE,        /**< vlan class invalid type */
    CTC_E_VLAN_CLASS_NOT_ENABLE,          /**< vlan class not enable */
    CTC_E_VLAN_MAPPING_NOT_ENABLE,        /**< vlan mapping not enable */
    CTC_E_VLAN_MAPPING_INVALID_SERVICE_ID,/**< vlan mapping invalid service id */
    CTC_E_VLAN_MAPPING_RANGE_TYPE_NOT_MATCH,/**< vlan mapping range type not match */
    CTC_E_VLAN_MAPPING_GET_VLAN_RANGE_FAILED,/**< vlan mapping get range failed*/
    CTC_E_VLAN_MAPPING_VRANGE_FULL,           /**< vlan mapping vlan range full*/
    CTC_E_VLAN_MAPPING_VRANGE_OVERLAPPED,     /**< vlan mapping vlan range overlapped*/
    CTC_E_VLAN_CLASS_PROTOCOL_VLAN_FULL,     /**< vlan class protocol vlan is full */

    /**< Linkagg */
    CTC_E_INVALID_TID = -29600,        /**< invalid tid */
    CTC_E_LINKAGG_NOT_EXIST,           /**< linkagg not exist */
    CTC_E_LINKAGG_HAS_EXIST,           /**< linkagg has exist */
    CTC_E_LINKAGG_DYNAMIC_BALANCE_NOT_SUPPORT,
    CTC_E_LINKAGG_INVALID_LINKAGG_MODE,

    /**< mem manager */
    CTC_E_CREATE_MEM_CACHE_FAIL = -29500,   /**< create mem cache fail */
    CTC_E_NO_RESOURCE,                      /**< no resource */

    /**< usrid */
    CTC_E_INVALID_USRID_KEY = -29400,  /**< invalid usrid key */
    CTC_E_INVALID_USRID_LABEL,         /**< invalid usrid label */
    CTC_E_INVALID_USRID_TYPE,          /**< invalid usrid type */
    CTC_E_USRID_DISABLE,               /**< usrid disable */
    CTC_E_USRID_ALREADY_ENABLE,        /**< usrid already enable */
    CTC_E_USRID_NO_KEY,                /**< usrid no key */

    /*sacl*/
    CTC_E_INVALID_SACL_KEY = -29380,
    CTC_E_INVALID_SACL_LABEL,
    CTC_E_INVALID_SACL_TYPE,
    CTC_E_SACL_DISABLE,
    CTC_E_SACL_ALREADY_ENABLE,
    CTC_E_SACL_NO_KEY,
    CTC_E_INVALID_SACL_DEFAULT_PORT,
    CTC_E_SACL_HASH_CONFLICT,
    CTC_E_SACL_HASH_INSERT_FAILED,
    CTC_E_SACL_HASH_CONVERT_FAILED,
    CTC_E_SACL_BUILD_AD_INDEX_FAILED,
    CTC_E_SACL_NEED_DEBUG,
    CTC_E_SACL_GET_KEY_FAILED,
    CTC_E_SACL_VLAN_ACTION_NO_MEMORY,
    CTC_E_SACL_KEY_AD_EXIST,
    CTC_E_SACL_AD_NO_MEMORY,
    CTC_E_SACL_BUILD_VLAN_ACTION_INDEX_FAILED,
    CTC_E_SACL_UNSUPPORT,
    CTC_E_SACL_CANNOT_OVERWRITE,
    CTC_E_SACL_LKP_FAILED,
    CTC_E_SACL_VLAN_ACTION_INVALID,
    CTC_E_SACL_STATIC_ENTRY,


    /**< fdb */
    CTC_E_WRONG_MAC_ADDR = -29300,     /**< wrong mac address */
    CTC_E_MAC_FILTERING_ENTRY_EXIST,   /**< mac filtering entry exist */
    CTC_E_FDB_ENTRY_FULL,              /**< fdb entry full */
    CTC_E_L2MCAST_MEMBER_INVALID,      /**< l2mcast member invalid */
    CTC_E_L2MCAST_ADD_MEMBER_FAILED,   /**< l2mcast add_member failed */
    CTC_E_MCAST_ENTRY_EXIST,           /**< mcast entry exist */
    CTC_E_DEFAULT_ENTRY_NOT_EXIST,     /**< default entry not exist */
    CTC_E_OPERATION_PAUSE,             /**< operation pause */
    CTC_E_HASH_INSERT_FAILED,
    CTC_E_HASH_REMOVE_FAILED,
    CTC_E_AD_INDEX_NOT_EXIST,
    CTC_E_DEFAULT_ENTRY_NOT_ALLOWED,
    CTC_E_INVALID_FDB_TYPE,
    CTC_E_NOT_LOCAL_MEMBER,
    CTC_E_KEY_ALREADY_EXIST,
    CTC_E_HASH_CONFLICT,

    /**< Nexthop module */
    CTC_E_NH_NOT_INIT = -29200,                 /**< hexthop not init */
    CTC_E_INVALID_NHID,                         /**< invalid nexthop id */
    CTC_E_NH_NOT_EXIST,                         /**< nexthop not exist */
    CTC_E_NH_EXIST,                             /**< nexthop exist */
    CTC_E_INVALID_DSEDIT_PTR,                   /**< invalid l2edit pointer */
    CTC_E_INVALID_NH_TYPE,                      /**< invalid nexthop type */
    CTC_E_INVALID_NH_SUB_TYPE,                  /**< invalid nexthop sub type */
    CTC_E_INVALID_VLAN_EDIT_TYPE,               /**< invalid vlan edit type */
    CTC_E_INVALID_DSNH_TYPE,                    /**< invalid dsnh type */
    CTC_E_VLAN_EDIT_CONFLICT,                   /**< vlan edit conflict */
    CTC_E_NH_STATS_NOT_EXIST,                   /**< nexthop stats not exist */
    CTC_E_NH_STATS_EXIST,                       /**< nexthop stats exist */
    CTC_E_INVALID_MARTINI_SEQ_TYPE = -29150,    /**< invalid martini seq type */
    CTC_E_SHOULD_USE_DSNH8W,                    /**< should use dsnh8w */
    CTC_E_SHOULD_USE_DSNH4W,                    /**< should use dsnh4w */
    CTC_E_NHID_NOT_MATCH_NHTYPE,                /**< nexthop Id not match nexthop type */
    CTC_E_GLB_SRAM_IS_INUSE,                    /**< global sram is inuse */
    CTC_E_GLB_SRAM_ISNOT_INUSE,                 /**< global sram isnot inuse */
    CTC_E_GLB_SRAM_INDEX_EXCEED,                /**< global sram index exceed */
    CTC_E_NO_TUNNEL_LABEL,                      /**< no tunnel label */
    CTC_E_NH_ISNT_UNROV,                        /**< nexthop isn't unrov */
    CTC_E_NH_IS_UNROV,                          /**< nexthop is unresolved */
    CTC_E_EXCEED_MAX_DSNH_OFFSET,               /**< exceed max dsnh offset */
    CTC_E_EXCEED_MAX_LOGICAL_REPLI_CNT,         /**< exceed max logical replicate number */
    CTC_E_MEMBER_IS_UPMEP,                      /**< member is upmep */
    CTC_E_EGS_EDIT_CONFLICT_VLAN_AND_MACDA_EDIT,/**< Vlan edit and change MAC DA edit exist conflict */
    CTC_E_NH_EXIST_VC_LABEL,           /**< VC label are using already*/
    CTC_E_NH_NOT_EXIST_TUNNEL_LABEL,  /**<Tunnel label not exist*/
    CTC_E_NH_NHOFFSET_EXHAUST,       /**< Nexthop offset is exhausted*/

    /**< l3 interface */
    CTC_E_L3IF_VMAC_NOT_EXIST = -29100,    /**< l3 interface vmac not exist */
    CTC_E_L3IF_VMAC_INVALID_PREFIX_TYPE,   /**< l3 interface vmac invalid prefix type */
    CTC_E_L3IF_VMAC_ENTRY_EXCEED_MAX_SIZE, /**< l3 interface vmac entry exceed max size */
    CTC_E_L3IF_ROUTED_EXCEED_IF_SIZE,      /**< l3 interface routed exceed interface size */
    CTC_E_L3IF_INVALID_IF_TYPE,            /**< l3 interface invalid interface type */
    CTC_E_L3IF_INVALID_IF_ID,              /**< l3 interface invalid interface id */
    CTC_E_L3IF_NOT_EXIST,                  /**< l3 interface not exist */
    CTC_E_L3IF_EXIST,                      /**< l3 interface exist */
    CTC_E_L3IF_MISMATCH,                   /**< l3 interface mismatch */
    CTC_E_L3IF_NO_ALLOCED_IPUCSA,          /**< l3 interface no alloced ipucsa */

    /**< acl/qos */
    CTC_E_ACLQOS_ENTRY_NOT_EXIST = -29000,       /**< aclqos entry not exist */
    CTC_E_ACLQOS_ENTRY_EXIST,                    /**< aclqos entry exist */
    CTC_E_ACLQOS_INVALID_ACTION,                 /**< aclqos invalid action */
    CTC_E_ACLQOS_INVALID_KEY_TYPE,               /**< aclqos invalid key type */
    CTC_E_ACLQOS_TCP_FLAG_EXHAUSTED,             /**< aclqos tcp flags entries is exhausted */
    CTC_E_ACLQOS_INVALID_LABEL_TYPE,             /**< aclqos invalid label type */
    CTC_E_ACLQOS_LABEL_NOT_EXIST,                /**< aclqos label not exist */
    CTC_E_ACLQOS_HAVE_ENABLED,                   /**< aclqos have enabled */
    CTC_E_ACLQOS_NOT_ENABLED,                    /**< aclqos not enabled */
    CTC_E_ACLQOS_LABEL_IN_USE,                   /**< aclqos label in use */
    CTC_E_QOS_POLICER_CREATED,                   /**< qos policer created */
    CTC_E_QOS_POLICER_NOT_EXIST,                 /**< qos policer not exist */
    CTC_E_QOS_POLICER_NOT_BIND,                  /**< qos policer not_bind */
    CTC_E_QOS_POLICER_IN_USE,                    /**< qos policer in_use */
    CTC_E_QOS_POLICER_CIR_GREATER_THAN_PIR,      /**< qos policer cir greater than pir */
    CTC_E_QOS_POLICER_CBS_GREATER_THAN_PBS,      /**< qos policer cbs greater than pbs */
    CTC_E_QOS_POLICER_SERVICE_POLICER_NOT_ENABLE,/**< qos policer service policer not enable */
    CTC_E_QOS_NO_INT_POLICER_ENTRY,              /**< qos no available internal policer entries */
    CTC_E_QOS_NO_EXT_POLICER_ENTRY,              /**< qos no available external policer entries */
    CTC_E_QOS_NO_INT_POLICER_PROFILE_ENTRY,      /**< qos no available internal policer profile entries */
    CTC_E_ACLQOS_DIFFERENT_DIR,                  /**< aclqos different direction */
    CTC_E_ACLQOS_DIFFERENT_TYPE,                 /**< aclqos different type */
    CTC_E_ACLQOS_DIFFERENT_CHIP,                 /**< aclqos different chip */
    CTC_E_ACLQOS_COLLISION_FIELD,                /**< aclqos can not add confliction acl/qos fields */
    CTC_E_ACL_PBR_ECMP_CMP_FAILED,               /**< ACL PBR ECMP entry compare failed, can not do ecmp */
    CTC_E_ACL_PBR_DO_ECMP_FAILED,                /**< acl pbr do ecmp failed */
    CTC_E_ACL_PBR_ENTRY_NO_NXTTHOP,              /**< for pbr permit entry, must add fwd-to-nhid action */
    CTC_E_QOS_THIS_PORT_NOT_SUPPORT_POLICER,     /**< this port not support policer */

    CTC_E_ACL_GET_BLOCK_INDEX_FAILED,

    /**< stats */
    CTC_E_STATS_MTU2_LESS_MTU1 = -28900,/**< stats mtu2 packet length less than mtu1 packet length */
    CTC_E_STATS_MTU1_GREATER_MTU2,      /**< stats mtu1 packet length greater than mtu2 packet length */
    CTC_E_STATS_PORT_STATS_NO_TYPE,     /**< Port stats option type is none */
    CTC_E_STATS_PHB_STATS_INVALID,      /**< Do phb/per-port-phb stats in qos module */
    CTC_E_STATS_PORT_NOT_MAP_TO_MAC,    /**< port not map to mac */
    CTC_E_STATS_PORT_NOT_ENABLE,        /**< port not enable */

    /**< ipuc */
    CTC_E_VERSION_DISABLE = -28800,    /**< SDK is not assigned resource for this version */
    CTC_E_INVALID_VRF,                 /**< invalid vrfid */
    CTC_E_INVALID_L3IF,                /**< l3if is not needed */
    CTC_E_NEED_L3IF,                   /**< need l3if */
    CTC_E_TUNNEL_INVALID,              /**< SDK is not assigned resource for tunnel of this IP version */
    CTC_E_TOO_MANY_FRAGMENT,           /**< too many fragments */

    /**< ipmc */
    CTC_E_IPMC_GROUP_NOT_EXIST = -28700,/**< ipmc group not exist */
    CTC_E_IPMC_GROUP_HAS_EXISTED,       /**< ipmc group has existed */
    CTC_E_IPMC_INVALID_MASK_LEN,        /**< Ipmc mask length error */
    CTC_E_BAD_L3IF_TYPE,                /**< Unknown l3if type */
    CTC_E_IPMC_NOT_MCAST_ADDRESS,       /**< Group address is NOT multicast IP address */
    CTC_E_IPMC_EXCEED_GLOBAL_CHIP,      /**< Target chip exceeds global chip id */
    CTC_E_IPMC_ADD_MEMBER_FAILED,       /**< ipmc add member failed */

    /**< pdu */
    CTC_E_INVALID_ACTION_INDEX = -28600,/**< invalid action index */
    CTC_E_INVALID_INDEX,                /**< invalid index */
    CTC_E_INVALID_MACDA,                /**< invalid macda */
    CTC_E_INVALID_MACDA_MASK,           /**< invalid macda mask */

    /**< queue */
    CTC_E_QUEUE_DROP_PROF_NOT_EXIST = -28500,   /**< queue drop profile not exist */
    CTC_E_QUEUE_SHAPE_PROF_NOT_EXIST,           /**< queue shape profile not exist */
    CTC_E_CHANNEL_SHAPE_PROF_EXIST,             /**< channel shape profile exist */
    CTC_E_CHANNEL_SHAPE_PROF_NOT_EXIST,         /**< channel shape profile not exist */
    CTC_E_QUEUE_NOT_ENOUGH,                     /**< queue not enough */
    CTC_E_SERVICE_QUEUE_NOT_INITIALIZED,        /**< service queue not initialized */
    CTC_E_SERVICE_QUEUE_INITIALIZED,            /**< service queue initialized */
    CTC_E_SERVICE_EXCEED_MAX_COUNT_LIMIT,       /**< the number of services reaches the maximum limit */
    CTC_E_SERVICE_NOT_EXIST,                    /**< service not exist */
    CTC_E_SERVICE_REGISTERED,                   /**< service registered */
    CTC_E_SERVICE_EXIST,                        /**< service exist */
    CTC_E_SERVICE_QUEUE_NO_HASH_ENTRY,          /**< service queue no hash entry */
    CTC_E_SERVICE_GROUP_NOT_EXIST,              /**< service group not exist */
    CTC_E_INTERNAL_PORT_IN_USE,                 /**< internal port in use */
    CTC_E_NO_FREE_INTERNAL_PORT,                /**< no free internal port */
    CTC_E_QUEUE_DEPTH_NOT_EMPTY,                /**< queue depth not empty */
    CTC_E_QUEUE_WDRR_WEIGHT_TOO_BIG,            /**< queue wdrr weight too big */
    CTC_E_QUEUE_INVALID_CONFIG,                 /**< queue invalid config */

    /**< aps */
    CTC_E_APS_GROUP_EXIST = -28400, /**< aps group exist */
    CTC_E_INVALID_APS_GROUP_ID,     /**< invalid aps group id */
    CTC_E_INVALID_APS_TYPE,         /**< invalid aps type */
    CTC_E_APS_GROUP_NOT_EXIST,      /**< aps group not exist */

    /**< vpls */
    CTC_E_VSI_NOT_CREATE = -28300,  /**< VPLS instance is not created */
    CTC_E_INVALID_VPLSPORT_ID,      /**< invalid vplsport id */

    /**< asic */
    CTC_E_HW_OP_FAIL = -28000,          /**< hardware operation failed */
    CTC_E_DRV_FAIL = -27999,            /**< driver init failed */
    CTC_E_CHECK_HSS_READY_FAIL = -27998, /*< Hss4G Macro change failed >*/

    /**< OAM */
    CTC_E_OAM_NOT_INIT = -27900,                   /**< Oam module not initialized */
    CTC_E_OAM_MAID_LENGTH_INVALID,                 /**< oam ma id length invalid */
    CTC_E_OAM_MAID_ENTRY_EXIST,                    /**< oam ma id entry exist */
    CTC_E_OAM_MAID_ENTRY_NOT_FOUND,                /**< oam ma id entry not found */
    CTC_E_OAM_CHAN_ENTRY_EXIST,                    /**< OAM lookup channel exists */
    CTC_E_OAM_CHAN_ENTRY_NOT_FOUND,                /**< oam lookup channel not found */
    CTC_E_OAM_CHAN_ENTRY_NUM_EXCEED,               /**< oam lookup channel entry full */
    CTC_E_OAM_CHAN_LMEP_EXIST,                     /**< Local mep exists in lookup channel */
    CTC_E_OMA_CHAN_LMEP_NOT_FOUND,                 /**< Local mep not found in lookup channel */
    CTC_E_OAM_CHAN_NOT_UP_DIRECTION,               /**< oam lookup channel isn't up mep channel */
    CTC_E_OAM_CHAN_NOT_DOWN_DIRECTION,             /**< oam lookup channel isn't down mep channel */
    CTC_E_OAM_TX_GPORT_AND_MASTER_GCHIP_NOT_MATCH, /**< Linkagg's master gchip does not match with tx_ccm_gport's chip */
    CTC_E_OAM_TX_GPORT_AND_CHAN_GPORT_NOT_MATCH,   /**< ccm_gport_id does not match with channel gport */
    CTC_E_OAM_RMEP_EXIST,                          /**< Remote mep exists */
    CTC_E_OAM_RMEP_NOT_FOUND,                      /**< Remote mep not found */
    CTC_E_OAM_INVALID_MD_LEVEL,                    /**< oam invalid md level */
    CTC_E_OAM_INVALID_MEP_ID,                      /**< oam invalid mep id */
    CTC_E_OAM_INVALID_MEP_CCM_INTERVAL,            /**< oam invalid mep ccm interval */
    CTC_E_OAM_INVALID_MEP_TPID_INDEX,              /**< oam invalid mep tpid index */
    CTC_E_OAM_INVALID_MEP_COS,                     /**< oam invalid mep cos */
    CTC_E_OAM_NH_OFFSET_EXIST,                     /**< Next hop offset for down mep exists */
    CTC_E_OAM_NH_OFFSET_NOT_FOUND,                 /**< Next hop offset for down mep not found */
    CTC_E_OAM_NH_OFFSET_ADD_VECTOR_FAIL,           /**< Add next hop offset for down mep fail */
    CTC_E_OAM_NH_OFFSET_IN_USE,                    /**< Next hop offset for down mep in use */
    CTC_E_OAM_RMEP_D_LOC_PRESENT,                  /**< Some rmep still has d_loc defect */
    CTC_E_OAM_ITU_DEFECT_CLEAR_MODE,               /**< clear the defect in itu defect clear mode */
    CTC_E_OAM_MEP_INDEX_VECTOR_ADD_FAIL,           /**< Add mep to index vector fail */
    CTC_E_OAM_INVALID_OPERATION_ON_CPU_MEP,        /**< Invalid operation on mep configured for cpu */
    CTC_E_OAM_DRIVER_FAIL,                         /**< Driver fail for OAM */
    CTC_E_OAM_EXCEED_MAX_LOOP_BACK_PORT_NUMBER,    /**< oam exceed max loop back port number */
    CTC_E_OAM_INVALID_GLOBAL_PARAM_TYPE,           /**< oam invalid global param type */
    CTC_E_OAM_INDEX_EXIST,                         /**< oam index exist */
    CTC_E_OAM_INVALID_MA_INDEX,                    /**< invalid oam ma index */
    CTC_E_OAM_INVALID_MEP_INDEX,                   /**< invalid oam mep index */
    CTC_E_OAM_INVALID_OAM_TYPE,                    /**< invalid oam type */
    CTC_E_OAM_EXCEED_MAX_LEVEL_NUM_PER_CHAN,

    /**< PTP */
    CTC_E_PTP_NOT_INIT = -27800,                   /**< ptp module not initialized */
    CTC_E_PTP_EXCEED_MAX_FRACTIONAL_VALUE,         /**< Exceed max ns or fractional ns value, must less than 10^9 */
    CTC_E_PTP_EXCEED_MAX_FFO_VALUE,                /**< ptp exceed max ffo value */
    CTC_E_PTP_INVALID_SYNC_INTF_PARAM_VALUE,       /**< ptp invalid sync interface param value */
    CTC_E_PTP_TX_TS_NOT_READY,                     /**< ptp tx ts not ready */
    CTC_E_PTP_TX_TS_ROLL_OVER_FAILURE,             /**< ptp tx ts roll over failure */

    /**< learning and aging */
    CTC_E_LEARNING_AND_AGING_INVALID_AGING_THRESHOLD = -27700,  /**< Invalid aging threshold */
    CTC_E_LEARNING_AND_AGING_INVALID_LEARNING_THRESHOLD,        /**< invalid learning threshold */
    CTC_E_AGING_INVALID_INTERVAL,                               /**< invalid aging interval */
    CTC_E_INVALID_MAC_FILTER_SIZE,                              /**< invalid mac filter size */


    /**< memory allcation */
    CTC_E_ALLOCATION_INVALID_ENTRY_SIZE= -27600,    /**< Invalid entry size,must be multiple of 256 (80bit) */
    CTC_E_ALLOCATION_INVALID_KEY_SIZE,              /**< invalid allocation key size */
    CTC_E_ALLOCATION_EXCEED_INT_TCAM_PHYSIZE,       /**< exceed internal tcam physize */
    CTC_E_ALLOCATION_EXCEED_EXT_TCAM_PHYSIZE,       /**< exceed external tcam physize */
    CTC_E_ALLOCATION_EXCEED_HASH_PHYSIZE,           /**< exceed hash physize */
    CTC_E_ALLOCATION_INVALID_HASH_PHYSIZE,          /**< invalid hash physize */
    CTC_E_ALLOCATION_EXCEEED_SRAM_PHYSIZE,          /**< exceed sram physize */
    CTC_E_ALLOCATION_EXCEED_MAX_OAM,                /**< exceed max oam num */
    CTC_E_HASH_MEM_INIT_TIMEOUT,                    /**< hash ram init timeout */

    /**< mirror */
    CTC_E_EXCEED_MIRROR_SESSION,             /**< exceed mirror session num */

    /**< mpls */
    CTC_E_MPLS_ENTRY_NOT_SUPPORT_STATS = -27500,    /**< mpls entry not support statistics */
    CTC_E_MPLS_ENTRY_STATS_EXIST,                   /**< mpls statistics entry exist */
    CTC_E_MPLS_ENTRY_STATS_NOT_EXIST,               /**< mpls statistics entry not exist */

    /**< serdes */
    CTC_E_CANT_CHANGE_SERDES_MODE = -27400, /**< can't change serdes mode  */
    CTC_E_SERDES_STATUS_NOT_READY = -27399, /**< serdes status is not ready  */
    CTC_E_SERDES_EYE_TEST_NOT_DONE = -27398, /**< serdes eye test not done  */
    CTC_E_ERROE_CODE_END = -10000
};

#define CTC_ERROR_RETURN(op) \
    { \
        int32 rv = (op); \
        if (rv  < 0) \
        { \
                return(rv); \
        } \
    }
#define CTC_ERROR_GOTO(op, ret, label) \
    { \
        ret = (op); \
        if (ret  < 0) \
        { \
                goto label; \
        } \
    }
#define CTC_ERROR_RETURN_WITH_UNLOCK(op, lock)\
    { \
        int32 rv; \
        if((rv = (op)) < 0) \
        { \
            kal_mutex_unlock(lock);\
            return (rv);\
        };\
    }

/**@} end of @defgroup  error ERROR */

#endif

