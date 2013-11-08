/**
 @file ctc_l2.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-11-26

 @version v2.0

   This file contains all L2 related data structure, enum, macro and proto.
*/


 #ifndef _CTC_L2_H
 #define _CTC_L2_H
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
 @defgroup fdb FDB
 @{
*/

/**
 @brief  define l2 FDB entry flags
*/
enum ctc_l2_flag_e
{
    CTC_L2_FLAG_IS_STATIC         = 0x00000001, /**< unicast FDB,if set ,indicate the entry is static entry, using for MAC DA */
    CTC_L2_FLAG_DISCARD           = 0x00000002, /**< unicast static FDB, ds_fwdPtr == 0xFFFF,using for MAC DA */
    CTC_L2_FLAG_SRC_DISCARD       = 0x00000004, /**< unicast static  FDB, MacSaExceptionEn = 1, using for MAC SA */
    CTC_L2_FLAG_SRC_DISCARD_TOCPU = 0x00000008, /**< unicast static FDB, MacSaExceptionEn = 1, mac_sa_exception_en = 1, using for MAC SA */
    CTC_L2_FLAG_COPY_TO_CPU       = 0x00000010, /**< unicast static FDB, MacDaExceptionEn = 1, using for MAC DA */
    CTC_L2_FLAG_RSV0              = 0x00000020, /**< Rsv0 */
    CTC_L2_FLAG_PROTOCOL_ENTRY    = 0x00000040, /**< unicast/Mcast FDB,protocolExceptionEn = 1, using for MAC DA */
    CTC_L2_FLAG_BIND_PORT         = 0x00000080, /**< unicast static FDB, MAC binding port/MAC +vlan binding port,srcMisMatchDiscard = 1, using for MAC SA */
    CTC_L2_FLAG_RAW_PKT_ELOG_CPU  = 0x00000100, /**< unicast static FDB, send raw packet to cpu,using for terminating protocol pakcet(LBM/DMM/LMM) to cpu */
    CTC_L2_FLAG_REMOTE_DYNAMIC    = 0x00000200, /**< unicast dynamic FDB, if set ,indicate the entry is remote chip learning dynamic entry, using for MAC DA */
    CTC_L2_FLAG_SYSTEM_RSV        = 0x00000400, /**< unicast static FDB, if set ,indicate the entry is system mac, it can't be deleted by flush api, using for MAC DA */
    CTC_L2_FLAG_PORTUNTAGGED      = 0x00000800, /**< unicast static FDB, if set ,indicate the output packet will is untagged packet from output port */
    CTC_L2_FLAG_UCAST_DISCARD     = 0x00001000, /**< unicast static FDB, if set ,indicate the output packet will be discard for unicast */
    CTC_L2_FLAG_KEEP_EMPTY_ENTRY  = 0x00002000, /**< Mcast  FDB,  if set, When removing group,  only delete all member */
    CTC_L2_FLAG_SERVICE_QUEUE     = 0x00004000, /**< unicast/Mcast FDB, if set, indicate the packet will do service queue*/
    CTC_L2_FLAG_LOGIC_PORT_CHK_EN = 0x00008000, /**< unicast/Mcast FDB, if set, indicate the packet's check will use logic port*/

    MAX_CTC_L2_FLAG
};
typedef enum ctc_l2_flag_e ctc_l2_flag_t;

/**
 @brief define vlan property flags
*/
enum ctc_l2_dft_vlan_flag_e
{
    CTC_L2_DFT_VLAN_FLAG_UNKOWN_UCAST_DROP   = 0x00000001,  /**< Unknown unicast discard in specific VLAN */
    CTC_L2_DFT_VLAN_FLAG_UNKOWN_MCAST_DROP   = 0x00000002,  /**< Unknown multicast discard in specific VLAN */
    CTC_L2_DFT_VLAN_FLAG_PROTOCOL_EXCP_TOCPU = 0x00000004,  /**< Protocol exception to cpu in in specific VLAN */
    MAX_CTC_L2_DFT_VLAN_FLAG
};
typedef enum ctc_l2_dft_vlan_flag_e ctc_l2_dft_vlan_flag_t;

/**
 @brief  define l2 FDB entry type flags
*/
enum ctc_l2_fdb_entry_op_type_e
{
    CTC_L2_FDB_ENTRY_OP_BY_VID,       /**<operation BY FID(VLAN/VSI)*/
    CTC_L2_FDB_ENTRY_OP_BY_PORT,      /**<operation BY PORT(VLAN/VSI)*/
    CTC_L2_FDB_ENTRY_OP_BY_MAC,       /**<operation BY MA(VLAN/VSI)*/
    CTC_L2_FDB_ENTRY_OP_BY_PORT_VLAN, /**<operation BY PORT + FID(VLAN/VSI)*/
    CTC_L2_FDB_ENTRY_OP_BY_MAC_VLAN,  /**<operation BY MAC+FID(VLAN/VSI)*/
    CTC_L2_FDB_ENTRY_OP_ALL,          /**<operation BY ALL(VLAN/VSI)*/
    MAX_CTC_L2_FDB_ENTRY_OP
};
typedef enum ctc_l2_fdb_entry_op_type_e ctc_l2_fdb_flush_type_t;
typedef enum ctc_l2_fdb_entry_op_type_e ctc_l2_fdb_query_type_t;


/**
 @brief define l2 FDB flush flag
*/
enum ctc_l2_fdb_entry_flag_e
{
    CTC_L2_FDB_ENTRY_STATIC,        /**<  all static fdb record */
    CTC_L2_FDB_ENTRY_DYNAMIC,       /**<  all dynamic fdb record */
    CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC, /**<  all local chip's dynamic fdb record */
    CTC_L2_FDB_ENTRY_ALL,           /**<  all fdb record */
    MAX_CTC_L2_FDB_ENTRY_FLAG
};
typedef enum ctc_l2_fdb_entry_flag_e ctc_l2_fdb_flush_flag_t;
typedef enum ctc_l2_fdb_entry_flag_e ctc_l2_fdb_query_flag_t;


/**
 @brief define source port type
*/
enum ctc_l2_fdb_src_port_e
{
    CTC_L2_FDB_SRC_PORT_TYPE_NORMAL = 0, /**< from normal port */
    CTC_L2_FDB_SRC_PORT_TYPE_VPLS = 1,   /**< from vpls port/qinq service queue appication */
    CTC_L2_FDB_SRC_PORT_TYPE_PBB = 2,    /**< from pbb PIP */
    MAX_CTC_L2_FDB_SRC_PORT_TYPE
};
typedef enum ctc_l2_fdb_src_port_e ctc_l2_fdb_src_port_t;

/**
 @brief Device-independent L2 unicast address.
*/
struct ctc_l2_addr_s
{
    mac_addr_t mac;             /**< 802.3 MAC address */
    mac_addr_t mask;            /**< mac address mask */

    uint16  fid;                /**< vid or fid ,if fid == 0xFFFF,indicate vlan will be masked */
    uint16  gport;              /**< Port ID ,if gsrcport_type is normal port, the gport is destination  and soure port; else the gport is soure port; */
    uint32  flag;               /**< ctc_l2_flag_t  CTC_L2_xxx flags */

    /* the following info only use for vpls/qinq serverce queue appication/PBB */
    ctc_l2_fdb_src_port_t gsrcport_type;    /**< src port type.*/
    uint16  logic_port;      				/**< if CTC_L2_FLAG_LOGIC_PORT_CHK_EN set,indicate logic Port is valid */
    uint16  mask_valid;
};
typedef struct ctc_l2_addr_s ctc_l2_addr_t;

/**
 @brief define FDB query DS.
*/
struct ctc_l2_fdb_query_s
{
    mac_addr_t mac;            /**< 802.3 MAC address */
    uint16     fid;            /**< vid or fid */
    uint32     count;          /**< return fdb count */
    uint16     gport;          /**< FDB Port ID */
    uint8      query_type;     /**< ctc_l2_fdb_query_type_t */
    uint8      query_flag;     /**< ctc_l2_fdb_query_flag_t */

};
typedef struct ctc_l2_fdb_query_s ctc_l2_fdb_query_t;

/**
 @brief  Store FDB entries query results,the results contain ucast fdb entries
*/
struct ctc_l2_fdb_query_rst_s
{
    ctc_l2_addr_t* buffer;      /**< A buffer store query results */
    uint32  start_index;        /**< If it is the first query, it is equal to 0, else it is equal to the last next_query_index */
    uint32  next_query_index;   /**< return index of the next query */
    uint8   is_end;             /**< if  is_end == 1, indicate the end of query */
    uint8   rsv;                /**< reserved*/
    uint16  buffer_len;         /**< multiple of sizeof(ctc_l2_addr_t) */

};
typedef struct ctc_l2_fdb_query_rst_s ctc_l2_fdb_query_rst_t;

/**
 @brief  Flush FDB entries data structure
*/
struct ctc_l2_fdb_flush_s
{
    mac_addr_t mac;             /**< 802.3 MAC address */
    uint16 fid;                 /**< vid or fid */
    uint16 gport;               /**< FDB Port ID */
    uint8  flush_flag;          /**< ctc_l2_fdb_flush_flag_t */
    uint8  flush_type;          /**< ctc_l2_fdb_flush_type_t */
};
typedef  struct ctc_l2_fdb_flush_s ctc_l2_fdb_flush_t;


/**@} end of @defgroup  fdb FDB */


/**
 @defgroup l2mcast L2Mcast
 @{
*/
/**
 @brief Device-independent L2 multicast address.
*/
struct ctc_l2_mcast_addr_s
{
    mac_addr_t mac;             /**< 802.3 MAC address */
    uint16  fid;                /**< vid or fid */
    uint32  flag;               /**< ctc_l2_flag_t  CTC_L2_xxx flags */
    uint16  l2mc_grp_id;        /**< mcast group_id, it only valid when add mcast group,and unique over the switch system */
    uint8   member_invalid;     /**< if set, member will be invalid,maybe applied to add/remove mcast group */
    uint8   with_nh;            /**< if set, packet will be edited by nhid */
    union
    {
       uint16   mem_port;       /**< member port if member is local member  gport:gchip(8bit) +local phy port(8bit);
                                                    else if member is remote chip entry,gport: gchip(local) + remote gchip id(8bit)*/
       uint32   nh_id;          /**< packet will be edited by nhid*/
    }member;
    uint16  logic_port;         /**< if CTC_L2_FLAG_LOGIC_PORT_CHK_EN set,indicate logic Port is valid */
    bool remote_chip;           /**< if set,member is remote chip entry*/

};
typedef struct ctc_l2_mcast_addr_s ctc_l2_mcast_addr_t;
/**
 @brief Device-independent L2 default information address.
*/
struct ctc_l2dflt_addr_s
{
    uint32   flag;               /**< ctc_l2_dft_vlan_flag_t  CTC_L2_DFT_VLAN_FLAG_xxx flags */

    uint16   fid;                /**< vid or fid */
    uint16   gport;              /**< port as default entry key */

    uint16   l2mc_grp_id;        /**< mcast group_id, it only valid when add default entry,and unique over the switch system */
    uint8    port_valid;         /**< if set,indicate port as default entry key is valid*/
    uint8    with_nh;            /**< if set, packet will be edited by nhid */
    uint16   logic_port;      /**< if CTC_L2_FLAG_LOGIC_PORT_CHK_EN set,indicate logic Port is valid */
    uint16   rsv0;
    union
    {
       uint16   mem_port;        /**<  member port if member is local member  gport:gchip(8bit) +local phy port(8bit);
                                                    else if member is remote chip entry,gport: gchip(local) + remote gchip id(8bit) */
       uint32   nh_id;           /**<  packet will be edited by nhid */
    }member;

    uint8    port_unattged;      /**< if set, out packet will untagged packet from the member port */
    uint8    remote_chip;        /**< if set,member is remote chip entry*/
    uint8    port_srv_queue;     /**< if set, out packet will  enqueue by service id*/
    uint8    rsv;
};
typedef struct ctc_l2dflt_addr_s ctc_l2dflt_addr_t;
/**
 @brief Device-independent L2 default information address.
*/
struct ctc_l2dflt_member_s
{
    uint8    with_nh;           /**< if set, packet will be edited by nhid */
    uint8    port_unattged;     /**< if set, out packet will untagged packet from the member port */
    uint16   rsv;
    union
    {
       uint16   mem_port;       /**<  member port if member is local member  gport:gchip(8bit) +local phy port(8bit);
                                                    else if member is remote chip entry,gport: gchip(local) + remote gchip id(8bit) */
       uint32   nh_id;          /**<  packet will be edited by nhid */
    }member;

    bool remote_chip;           /**< if set,member is remote chip entry*/
};
typedef struct ctc_l2dflt_member_s ctc_l2dflt_member_t;

/**@} end of @defgroup  l2mcast L2Mcast */


/**
 @defgroup stp STP
 @{
 */
#define CTC_MAX_STP_ID 0x80
#define CTC_MIN_STP_ID 1
enum stp_state_e
{
    CTC_STP_FORWARDING  = 0x00,     /**< stp state, packet normal forwarding */
    CTC_STP_BLOCKING    = 0x01,     /**< stp state, packet blocked */
    CTC_STP_LEARNING    = 0x02,     /**< stp state, learning enable */
    CTC_STP_UNAVAIL  	= 0x03
};
/**@} end of @defgroup  stp STP */

/**
 @brief l2 fdb global config infomation
*/
struct ctc_l2_fdb_global_cfg_s
{
    uint32 flush_fdb_cnt_per_loop;  /**< flush flush_fdb_cnt_per_loop entries one time if flush_fdb_cnt_per_loop>0,
                                         flush all entries if flush_fdb_cnt_per_loop=0 */
    uint16 default_entry_rsv_num;   /**< Systems reserve the number of default entry,example:
                                       if systems support per FID default entry,and systems support FID ranges:1-4094,
                                       it's equal to 4094; if systems support FID ranges:100-200, it's equal to 100.In addition,
                                       if default_entry_rsv_num is equal to 0, systems won't limit the number of default entry. */
    uint16 rsv;
};
typedef struct ctc_l2_fdb_global_cfg_s ctc_l2_fdb_global_cfg_t;

#endif  /*_CTC_L2_H*/

