/**
 @file sys_humber_queue_shape.h

 @date 2010-01-13

 @version v2.0

 The file defines macro, data structure, and function for queue shaping.
*/

#ifndef _SYS_HUMBER_QUEUE_SHAPE_H_
#define _SYS_HUMBER_QUEUE_SHAPE_H_

#include "ctc_queue.h"

/*********************************************************************
 *
 * Macro
 *
 *********************************************************************/

#define SYS_SPECIAL_SHAPE_BURST             0xFFFFFFFF
#define SHAPE_UPDATE_UNIT                   4
#define SYS_RESERVED_SHAPE_PROFILE_ID       0
#define SYS_RESERVED_GROUP_SHAPE_PROFILE_ID 0
#define SYS_FULL_SHAPE_TOKENS               0xFFFFFF

#define SYS_MIN_SHAPE_BURST                 16000

/*If the burst is greater than this number, the shift figured out
    by _sys_humber_queue_shape_threshold_compute will be error 16 */
#define SYS_MAX_SHAPE_BURST                 4194300
#define SYS_MAX_QUEUE_SHAPE_PROFILE_NUM      256
#define SYS_QUEUE_SHAPE_HASH_BUCKET_SIZE     64

#define SYS_QUEUE_INVALID_GROUP             0x1FF
#define SYS_MAX_GROUP_SHAPE_PROFILE_NUM      256
#define SYS_MAX_GROUP_NUM                   256
#define SYS_GROUP_SHAPE_HASH_BUCKET_SIZE     64

#define SYS_MAX_SHAPE_RATE      10000000*(1000/8)     /* convert 10Gbps into byte/s */

/*********************************************************************
 *
 * Data Structure
 *
 *********************************************************************/

/**
 @brief Queue shape profile.
*/
struct sys_queue_shape_profile_s
{
    uint32 commit_rate;          /**< commit token rate */
    uint32 peak_rate;            /**< peak token rate */
    uint8  commit_shift;         /**< commit token threshold shift */
    uint8  commit_threshold;     /**< commit token threshold */
    uint8  peak_shift;           /**< peak token threshold shift */
    uint8  peak_threshold;       /**< peak token threshold */

    uint16 ref;      /**< reference counter */
    uint8  index;    /**< queue shape profile index */
    uint8  rsv;
};
typedef struct sys_queue_shape_profile_s sys_queue_shape_profile_t;


/**
 @brief Group shape profile.
*/
struct sys_group_shape_profile_s
{
    uint32 rate;
    uint8  shift;
    uint8  threshold;

    uint16 ref;     /**< reference counter */

    uint16 rsv1;
    uint8  index;   /**< group shape profile index */
    uint8  rsv2;

};
typedef struct sys_group_shape_profile_s sys_group_shape_profile_t;


struct sys_queue_group_s
{
    sys_group_shape_profile_t *p_profile;

    uint16 group;
};
typedef struct sys_queue_group_s sys_queue_group_t;


/**
 @brief Channel shape profile.
*/
struct sys_channel_shape_profile_s
{
    uint32 rate;
    uint8  shift;
    uint8  threshold;
};
typedef struct sys_channel_shape_profile_s sys_channel_shape_profile_t;


/**
 @brief Shape control.
*/
struct sys_queue_shape_ctl_s
{
    uint16 queue_shape_max_ptr;
    uint16 queue_shape_min_ptr;
    uint16 queue_shape_low_bw_max_ptr;
    uint16 queue_shape_high_bw_min_ptr;
    uint8  queue_shape_low_bw_weight;
    uint8  queue_shape_high_bw_weight;
    uint16 queue_shape_max_phy_ptr;
    uint8  queue_shape_update_max_cnt;
    uint8  queue_shape_enable;


    uint16 group_shape_max_ptr;
    uint16 group_shape_min_ptr;
    uint16 group_shape_low_bw_max_ptr;
    uint16 group_shape_high_bw_min_ptr;
    uint8  group_shape_low_bw_weight;
    uint8  group_shape_high_bw_weight;
    uint16 group_shape_max_phy_ptr;
    uint8  group_shape_update_max_cnt;
    uint8  group_shape_enable;

    uint16 channel_shape_max_ptr;
    uint16 channel_shape_min_ptr;
    uint16 channel_shape_max_phy_ptr;
    uint8  channel_shape_update_max_cnt;
    uint8  channel_shape_enable;
};
typedef struct sys_queue_shape_ctl_s sys_queue_shape_ctl_t;



/*********************************************************************
 *
 * Function Declaration
 *
 *********************************************************************/

extern int32
_sys_humber_get_port_queue_type_by_lport(uint8 lport, ctc_queue_type_t* queue_type);

extern int32
_sys_humber_get_per_port_queue_num_by_lport(uint8 lport, uint8* queue_num_per_port);

extern int32
_sys_humber_channel_shape_profile_write(uint8 lchip, uint8 channel, sys_channel_shape_profile_t* p_profile);

/**
 @brief Globally enable/disable queue shaping function.
*/
extern int32
sys_humber_queue_set_queue_shape_global_enable(bool enable);


/**
 @brief Get queue shape global enable status.
*/
extern int32
sys_humber_queue_get_queue_shape_global_enable(bool* p_enable);


/**
 @brief Globally enable/disable group shaping function.
*/
extern int32
sys_humber_queue_set_group_shape_global_enable(bool enable);


/**
 @brief Get group shape global enable status.
*/
extern int32
sys_humber_queue_get_group_shape_global_enable(bool* p_enable);


extern int32
sys_humber_queue_set_shape_ipg_enable(bool enable);

/**
 @brief Globally enable/disable channel shaping function.
*/
extern int32
sys_humber_queue_set_channel_shape_global_enable(bool enable);


/**
 @brief Get channel shape global enable stauts.
*/
extern int32
sys_humber_queue_get_channel_shape_global_enable(bool* p_enable);


/**
 @brief Set shaping for the given queue in a chip. Queue shaping supports dual-rate dual-bucket algorithm.
        In different priority queues, the committed traffic is scheduled in SP mode and the exceeded traffic
        is serviced in WDRR mode.
*/
extern int32
sys_humber_queue_set_queue_shape(uint8 lchip, uint16 queue_id, ctc_queue_shape_t* p_shape);


/**
 @brief Unset shaping for the given queue in a chip.
*/
extern int32
sys_humber_queue_unset_queue_shape(uint8 lchip, uint16 queue_id);


/**
 @brief Get shaping for the given queue in a chip.
*/
extern int32
sys_humber_queue_get_queue_shape(uint8 lchip, uint16 queue_id, ctc_queue_shape_t* p_shape);

/**
 @brief Set shaping for the given channel in a chip. Channel shaping supports single-rate single-bucket algorithm.
*/
extern int32
sys_humber_queue_set_channel_shape(uint8 lchip, uint8 channel, ctc_port_shape_t* p_shape);


/**
 @brief Unset shaping for the given channel in a chip.
*/
extern int32
sys_humber_queue_unset_channel_shape(uint8 lchip, uint8 channel);


/**
 @brief Get shaping for the given channel in a chip.
*/
extern int32
sys_humber_queue_get_channel_shape(uint8 lchip, uint8 channel, ctc_port_shape_t* p_shape);


/**
 @brief Set queue shaping for the given service ID.
*/
extern int32
sys_humber_service_set_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset, ctc_queue_shape_t* p_shape);


/**
 @brief Unset queue shaping for the given service ID.
*/
extern int32
sys_humber_service_unset_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset);


/**
 @brief Get queue shaping for the given service ID.
*/
extern int32
sys_humber_service_get_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset, ctc_queue_shape_t* p_shape);

/**
 @brief Set group shaping.
*/
extern int32
sys_humber_group_set_shape(ctc_group_shape_cfg_t* group_shape);

/**
 @brief Unset group shaping.
*/
extern int32
sys_humber_group_unset_shape(ctc_group_shape_cfg_t* group_shape);

/**
 @brief Get group shaping.
*/
extern int32
sys_humber_group_get_shape(ctc_group_shape_cfg_t* group_shape);

/**
 @brief Queue shaper initialization.
*/
extern int32
sys_humber_queue_shape_init(void);

/**
 @brief Get total number of queue shape profile.
*/
extern int32
sys_humber_queue_get_queue_profile_num(uint32* p_shape_num);

#endif

