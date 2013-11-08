/**
 @file sys_humber_queue_enq.h

 @date 2010-01-13

 @version v2.0

 The file defines macro, data structure, and function for en-queue
*/

#ifndef _SYS_HUMBER_QUEUE_ENQ_H_
#define _SYS_HUMBER_QUEUE_ENQ_H_

#include "ctc_debug.h"
#include "ctc_const.h"
#include "ctc_stats.h"

#include "sys_humber_queue_drop.h"
#include "sys_humber_queue_shape.h"

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/

#define SYS_QUEUE_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(qos, queue, QOS_QUE_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_QUEUE_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(qos, queue, QOS_QUE_SYS); \
    }


#define SYS_MAX_QUEUE_NUM           2048

#define SYS_MAX_CHANNEL_NUM         256
#define SYS_MAX_GMAC_CHANNEL_NUM    48  /**< channel id = 0..47 */
#define SYS_MAX_XGMAC_CHANNEL_NUM   4   /**< channel id = 48..51 */
#define SYS_XGMAC_CHANNEL_ID_BASE   48
#define SYS_ILOOP_CHANNEL_ID        52
#define SYS_CPU_CHANNEL_ID          53
#define SYS_OAM_CHANNEL_ID          54
#define SYS_ELOOP_CHANNEL_ID        55
#define SYS_DROP_CHANNEL_ID_START   56
#define SYS_DROP_CHANNEL_ID_END     127
#define SYS_FABRIC_CHANNEL_ID_BASE  128
#define SYS_MAX_FABRIC_CHANNEL_NUM  128 /**< channel id = 128..255 */

#define SYS_STATIC_INT_PORT_START   56
#define SYS_STATIC_INT_PORT_END     63

#define SYS_NETWORK_PORT_NUM    (SYS_MAX_GMAC_CHANNEL_NUM + SYS_MAX_XGMAC_CHANNEL_NUM)
#define SYS_REGULAR_PORT_NUM    (SYS_DROP_CHANNEL_ID_START + 1)
#define SYS_STATIC_INT_PORT_NUM (SYS_STATIC_INT_PORT_END - SYS_STATIC_INT_PORT_START + 1)


#define SYS_QNUM_GEN_CTL_INDEX(QUE_ON_BAY,SRC_QSEL,DEST_CHIP_MATCH,QSEL_TYPE) \
    ((((QUE_ON_BAY) & 0x1)<<7) | (((SRC_QSEL) & 0x1)<< 6) | (((DEST_CHIP_MATCH) & 0x1) << 5) | ((QSEL_TYPE) & 0x1F))


#define SYS_FABRIC_CHANNEL_NUM_PER_PORT       4
#define SYS_MAX_SUPPORTED_FABRIC_PORT_NUM     16
#define SYS_MAX_FABRIC_PORT_NUM               32

#define SYS_OAM_INTERNAL_PORT_QUEUE_BASE      1939

#define SYS_QUEUE_MAX_RESOURCE_CNT            24 * 1024   /* 6MB (256 Byte per Cell) */
#define SYS_QUEUE_RESOURCE_GROUP_OAM          248
#define SYS_QUEUE_RESOURCE_GROUP_ELOOP        249
#define SYS_QUEUE_RESOURCE_GROUP_ILOOP        250
#define SYS_QUEUE_RESOURCE_GROUP_CPU          251
#define SYS_QUEUE_RESOURCE_GROUP_FABRIC_BASE  252

#define SYS_SERVICE_QUEUE_HASH_BUCKET_NUM     128
#define SYS_SERVICE_QUEUE_HASH_BUCKET_DEPTH   4
#define SYS_SERVICE_ID_NUM                    0x4000
#define SYS_QMGR_HASH_CAM_CTL_SIZE            16

/*********************************************************************
 *
 * Data Structure
 *
 *********************************************************************/

enum sys_queue_resrc_alloc_type_e
{
    SYS_RESRC_SINGLE_CHIP_WITHOUT_SERVICE_QUEUE = 0,
    SYS_RESRC_SINGLE_CHIP_WITH_SERVICE_QUEUE,
    SYS_RESRC_MULTI_CHIP_WITHOUT_SERVICE_QUEUE,
    SYS_RESRC_MULTI_CHIP_WITH_SERVICE_QUEUE,

    MAX_SYS_RESRC_ALLOC_TYPE
};
typedef enum sys_queue_resrc_alloc_type_e sys_queue_resrc_alloc_type_t;

/**
 @brief Allocation of queue select type.
*/
enum sys_queue_select_type_e
{
    SYS_QSEL_TYPE_REGULAR           = 0,
    SYS_QSEL_TYPE_EXCP_CPU          = 0x01,
    SYS_QSEL_TYPE_INTERNAL_PORT     = 0x02,
    SYS_QSEL_TYPE_SERVICE           = 0x03,
    SYS_QSEL_TYPE_OAM_INTERNAL_PORT = 0x04,
    SYS_QSEL_TYPE_SGMAC_PORT_TO_CPU = 0x05,
    SYS_QSEL_TYPE_SGMAC_CPU_TO_CPU  = 0x06,
    SYS_QSEL_TYPE_STATIC_INT_PORT   = 0x07,
    SYS_QSEL_TYPE_MCAST             = 0x1F
};

struct sys_queue_resrc_alloc_e
{
    uint8 shared_resrc_percent;
    uint8 cpu_private_resrc_percent;
    uint8 iloop_private_resrc_percent;
    uint8 eloop_private_resrc_percent;
    uint8 oam_private_resrc_percent;
    uint8 fabric_private_resrc_percent;
    uint8 network_private_resrc_percent;
};
typedef struct sys_queue_resrc_alloc_e sys_queue_resrc_alloc_t;


/**
 @brief Service queue hash bucket information.
*/
struct sys_service_queue_hash_key_s
{
    uint16 dest_id;
    uint16 service_id;
};
typedef struct sys_service_queue_hash_key_s sys_service_queue_hash_key_t;


struct sys_service_queue_hash_index_s
{
    ctc_list_pointer_node_t head;

    uint16 service_id;      /**< physical service id */
    uint16 dest_id;

    uint8 bucket_index;     /**< 0xFF indicates in QmgrHashCamCtl reg */
    uint8 depth;
};
typedef struct sys_service_queue_hash_index_s sys_service_queue_hash_index_t;


struct sys_queue_channel_entry_s
{
    ctc_list_pointer_node_t head;

    uint16 queue_id;
};
typedef struct sys_queue_channel_entry_s sys_queue_channel_entry_t;


/**
 @brief Queue configurations.
*/
struct sys_queue_config_s
{
    uint8  type;
    uint8  class;
    uint8  channel;
    uint8  wred_weight;    /**< WRED exponential weighted moving average (EWMA) constant for
                                calculating average queue size. EWMA = 1/2^weight,which results:
                                avgq(k) = 1/2^weight * q(k) + (1-1/2^weight) * q(k-1) */

    sys_queue_channel_entry_t *p_channel_entry;

    uint8  force_random_drop;
    uint8  rsv1;
    uint16 group;
    uint32 wdrr_weight;


    sys_queue_drop_profile_t  *p_drop_profile;
    sys_queue_shape_profile_t *p_queue_shape_profile;

    ctc_stats_basic_t drop_stats;
    ctc_stats_basic_t deq_stats;
};
typedef struct sys_queue_config_s sys_queue_config_t;


struct sys_service_config_s
{
    ctc_list_pointer_node_t head;

    uint16 logical_service_id;  /**< logical service id, unique per linecard */
    uint16 physical_service_id; /**< physical service id, unique per local chip */
    uint16 register_num;
    uint8  opf_offset;
    uint16 ingress_queue_base;
    uint16 egress_queue_base;
    sys_queue_group_t *p_igs_group[CTC_MAX_LOCAL_CHIP_NUM];
    sys_queue_group_t *p_egs_group[CTC_MAX_LOCAL_CHIP_NUM];

    ctc_list_pointer_t hash_key_idx_list;
};
typedef struct sys_service_config_s sys_service_config_t;


/**
 @brief DsQueueNumGenCtl data structure.
*/
struct sys_queue_num_gen_ctl_s
{
    uint8  queue_select_shift;
    uint8  queue_select_mask;

    uint8  dest_chip_shift;
    uint8  dest_chip_mask;
    uint8  dest_chip_base;

    uint8  dest_queue_shift;
    uint16 dest_queue_mask;
    uint16 dest_queue_base;

    uint8  sgmac_shift;
    uint8  sgmac_mask;
    uint8  sgmac_base;

    uint8  flow_id_shift;
    uint8  flow_id_mask;
    uint8  flow_id_base;

    uint16 queue_num_base;
    uint8  queue_num;

    uint8  service_queue_enable;
};
typedef struct sys_queue_num_gen_ctl_s sys_queue_num_gen_ctl_t;

/**
 @brief En-queue configurations.
*/
struct sys_queue_enq_ctl_s
{
    uint8  local_switch_disable;
    uint8  flow_id_enable;
    uint8  service_queue_enable;
    uint8  flow_ctl_enable;
    uint8  queue_num_per_network_port;       /**< queue number per network port */
    uint8  queue_num_per_static_int_port;    /**< queue number per static internal port */
    uint8  queue_num_per_fabric;             /**< queue number per fabric */
    uint8  max_internal_port_id;             /**< max internal port id */
    uint8  queue_num_per_internal_port;      /**< queue number per internal port */
    uint8  queue_num_per_ingress_service;    /**< queue number per ingress service */
    uint8  queue_num_per_egress_service;     /**< queue number per egress service */
    uint8  current_service_num;
    uint16 supported_service_num;
    uint8  internal_port_num;
    uint8  rsv1;

    sys_queue_num_gen_ctl_t qnum_gen_ctl[MAX_CTC_QUEUE_TYPE];
    sys_service_queue_hash_key_t *p_qmgr_hash_cam_ctl[CTC_MAX_LOCAL_CHIP_NUM][SYS_QMGR_HASH_CAM_CTL_SIZE];
};
typedef struct sys_queue_enq_ctl_s sys_queue_enq_ctl_t;

extern sys_queue_config_t sys_queue_config[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_QUEUE_NUM];
extern ctc_list_pointer_t sys_channel_list[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_CHANNEL_NUM];
extern sys_queue_enq_ctl_t sys_enq_ctl;


/*********************************************************************
 *
 * Function Declaration
 *
 *********************************************************************/

/**
 @brief Init DsQueueNumGenCtl for packet-to-CPU exceptions.
*/
extern int32
sys_humber_excp_cpu_queue_num_gen_ctl_init(void);


/**
 @brief Enable/disable local switching. When local swith is disabled, all incoming packets
        will be forced to fabric no matter it is intended to the local chip or remote chip.
*/
extern int32
sys_humber_set_local_switch_enable(bool enable);


/**
 @brief Wait until queue is empty.
*/
extern int32
sys_humber_queue_wait_queue_empty(uint8 lchip, uint16 queue_id, uint8 *is_empty);

/**
 @brief Get local switch enable status.
*/
extern int32
sys_humber_get_local_switch_enable(bool* p_enable);


/**
 @brief enable/disable buf store resource manage.
*/
extern int32
sys_humber_queue_set_buf_store_resrc_mgr_enable(bool enable);

/**
 @brief Get buf store resource manage enable status.
*/
extern int32
sys_humber_queue_get_buf_store_resrc_mgr_enable(bool* p_enable);


/**
 @brief Get queue ID by the given queue type, id, and offset. NOTE that, for each queue type,
        the meaning of parameters "id" and "offset" is derived from the context.

        <ALT + F12 to pretty the following table in source-insight>

        type             meaning of "id"     meaning of "offset"
        --------------   ------------------  ---------------------------------
        network egress   global port id      queue offfset in the egress port
        i-loop           i-loop channel id   queue offset
        normal cpu       cpu channel id      queue offset
        oam              oam channel id      queue offset
        e-loop           e-loop channel id   queue offset

        exception cpu    exception           sub index
        fabric           target chip id      queue offset in the target chip

        service ingress  service id          queue offset in the service
        network ingress  global port id      queue offfset in the ingress port
        service egress   service id          queue offset in the service
*/
extern int32
sys_humber_queue_get_queue_id(ctc_queue_type_t type, uint16 id, uint8 offset, uint16* p_queue_id);

/**
 @brief Disable channel's all queue threshold, all queue,group,channel shapping.
*/
extern int32
sys_humber_queue_disable_channel(uint8 lchip, uint8 channel);

/**
 @brief Restore channel's all queue threshold, all queue,group,channel shapping.
*/
extern int32
sys_humber_queue_enable_channel(uint8 lchip, uint8 channel);

/**
 @brief Add a queue in the specific local chip to the given channel.
*/
extern int32
sys_humber_queue_add_to_channel(uint8 lchip, uint16 queue_id, uint8 channel);


/**
 @brief Remove a queue in the specific local chip from the given channel.
*/
extern int32
sys_humber_queue_remove_from_channel(uint8 lchip, uint16 queue_id, uint8 channel);


/*~~~~~~~~~~~~~~~~~~~~~~service queue function declaration~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 @brief To globally enable/disable QoS flow ID.
*/
extern int32
sys_humber_queue_flow_id_enable(bool enable);


/**
 @brief Get QoS flow ID global enable status.
*/
extern int32
sys_humber_queue_get_flow_id_enable(bool* p_enable);


/**
 @brief Enable service ID.
*/
extern int32
sys_humber_service_id_enable();


/**
 @brief Disable service ID.
*/
extern int32
sys_humber_service_id_disable();


/**
 @brief Enable/Disable service ID.
*/
extern int32
sys_humber_get_service_id_enable(bool* p_enable);


/**
 @brief Create a service ID.
*/
extern int32
sys_humber_create_physical_service_id(uint16 logical_service_id, uint8 dest_port);


/**
 @brief Remove a service ID.
*/
extern int32
sys_humber_remove_physical_service_id(uint16 logical_service_id);


/**
 @brief Init service ingress/egress queue for the given service.
*/
extern int32
sys_humber_service_queue_init(uint16 logical_service_id, uint8 dest_port);


/**
 @brief De-init service ingress/egress queue for the given service.
*/
extern int32
sys_humber_service_queue_deinit(uint16 logical_service_id);


/**
 @brief Lookup a service by the given service ID.
*/
extern int32
sys_humber_service_lookup(uint16 service_id, sys_service_config_t** pp_service);


/**
 @brief Write service queue hash key to DsServiceHashKey.
*/
extern int32
sys_humber_service_queue_hash_write(sys_service_queue_hash_index_t* p_index);


/**
 @brief Remove hash key from DsServiceHashKey.
*/
extern int32
sys_humber_service_queue_hash_remove(sys_service_queue_hash_index_t* p_index);


/**
 @brief Get per-service queue number.
*/
extern uint8
sys_humber_get_per_service_queue_num(void);

/**
 @brief Get service ingress queue enable.
*/
extern int32
sys_humber_get_service_ingress_queue_enable(bool* enable);

/**
 @brief Get service egress queue enable.
*/
extern int32
sys_humber_get_service_egress_queue_enable(bool* enable);

/**
 @brief set priority color map to queue select and drop_precedence.
*/
extern int32
sys_humber_set_priority_queue_map(uint8 lchip, ctc_queue_pri_map_t* p_queue_pri_map);


/**
 @brief En-queue component initialization.
*/
extern int32
sys_humber_queue_enq_init(ctc_queue_global_cfg_t* queue_global_cfg);

#endif

