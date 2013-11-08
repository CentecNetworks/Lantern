/**
 @file sys_humber_qos_policer.h

 @date 2009-11-30

 @version v2.0

 The file defines macro, data structure, and function for qos policer component
*/

#ifndef _SYS_HUMBER_QOS_POLICER_H_
#define _SYS_HUMBER_QOS_POLICER_H_

#include "ctc_vector.h"

/*********************************************************************
  *
  * macro definition
  *
  *********************************************************************/

#define SYS_QOS_POLICER_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(qos, policer, QOS_PLC_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_QOS_POLICER_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(qos, policer, QOS_PLC_SYS); \
    }

#define SYS_MAX_POLICER_RATE      10000000*(1000/8)     /* convert 10Gbps into byte/s */
#define SYS_MAX_POLICER_TOKEN_SIZE      0x400000

/*********************************************************************
  *
  * data structure definition
  *
  *********************************************************************/

struct sys_qos_policing_ctl_s
{
    uint16 tick_gen_interval;
    uint16 port_policer_base[CTC_BOTH_DIRECTION];
    uint8  port_policer_shift[CTC_BOTH_DIRECTION];
    uint16 flow_policer_base;
    uint16 flow_stats_policer_base;
    uint16 flow_stats_policer_size;
    uint16 min_policer_ptr;
    uint16 max_policer_ptr;
    uint16 int_policer_profile_num;
    uint16 ext_policer_profile_num;
    uint8  policing_enable;
    uint8  stats_enable;
    uint16 service_policer_base;
    uint8  is_phb_support;
    uint8  stats_mode;
    uint32 service_policer_num;
    ctc_vector_t* sys_service_policer_vec;
};
typedef struct sys_qos_policing_ctl_s sys_qos_policing_ctl_t;


/**
 @brief  qos profile entry data structure
*/
struct sys_qos_policer_profile_s
{
    uint16 commit_rate;
    uint16 peak_rate;
    uint16 commit_threshold;
    uint16 peak_threshold;
    uint8  commit_shift;
    uint8  peak_shift;
    uint8  tick_shift;
    uint8  index;
    uint16 ref;
    uint8  rsv[2];
};
typedef struct sys_qos_policer_profile_s sys_qos_policer_profile_t;


/**
 @brief  qos policer data structure
*/
struct sys_qos_policer_s
{
    uint32 id;

    sys_qos_policer_profile_t *p_profile[CTC_MAX_LOCAL_CHIP_NUM];
    uint16 index[CTC_MAX_LOCAL_CHIP_NUM];
    uint16 ref[CTC_MAX_LOCAL_CHIP_NUM];

    uint32 cir;     /**< bytes per secnod */
    uint32 cbs;     /**< bytes */
    uint32 pir;     /**< bytes per secnod */
    uint32 pbs;     /**< bytes */

    uint8 is_srtcm;             /**< single rate = 1, double rate = 0 */
    uint8 is_color_blind;       /**< color blind = 1, color aware = 0 */
    uint8 use_l3_length;        /**< use l3 length = 1, full packet length = 0 */
    uint8 drop_color;

    uint8 is_stats_en;          /**< enable policer stats for three color */
    uint8 rsv0[3];
};
typedef struct sys_qos_policer_s sys_qos_policer_t;


/*********************************************************************
  * function declaration
  *********************************************************************/

extern int32
sys_humber_qos_flow_policer_first_enable(ctc_direction_t dir, bool enable);

/**
 @brief create qos flow policer
*/
extern int32
sys_humber_qos_flow_policer_new(ctc_qos_policer_t* p_ctc_policer, uint32 plc_id);


/**
 @brief delete qos flow policer
*/
extern int32
sys_humber_qos_flow_policer_delete(uint32 plc_id);


/**
 @brief get policer index for the given chip
*/
extern int32
sys_humber_qos_policer_index_get(uint8 lchip, uint32 plc_id, uint32* p_index);

/**
 @brief refresh old qos flow policer with new qos flow policer
*/
extern int32
sys_humber_qos_flow_policer_refresh(uint32 plc_id, ctc_qos_policer_t* p_ctc_policer);


/**
 @brief bind flow policer to the given chip
*/
extern int32
sys_humber_qos_flow_policer_bind(uint8 lchip, uint32 plc_id);


/**
 @brief unbind flow policer from attached chip
*/
extern int32
sys_humber_qos_flow_policer_unbind(uint8 lchip, uint32 plc_id);


/**
 @brief Get real flow policer data added to chip.
*/
extern int32
sys_humber_qos_get_flow_policer_data(uint32 plc_id, ctc_qos_policer_t* p_ctc_policer);

/**
 @brief bind port policer
*/
extern int32
sys_humber_qos_port_policer_bind(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_ctc_policer);

/**
 @brief Bind service policer.
*/
extern int32
sys_humber_qos_service_policer_bind(uint16 service_id, ctc_qos_policer_t* p_ctc_policer);

/**
 @brief Unbind service policer.
*/
extern int32
sys_humber_qos_service_policer_unbind(uint16 service_id);

/**
 @brief unbind port policer
*/
extern int32
sys_humber_qos_port_policer_unbind(uint32 gport, ctc_direction_t dir, uint8 phb_offset);



/**
 @brief Get real port policer data added to chip.
*/
extern int32
sys_humber_qos_get_port_policer_data(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_ctc_policer);


/**
 @brief To globally enable/disable QoS policer.
*/
extern int32
sys_humber_qos_policer_update_enable(bool enable);


/**
 @brief Get QoS policer global enable status.
*/
extern int32
sys_humber_qos_get_policer_update_enable(bool* p_enable);


/**
 @brief To globally enable/disable QoS policer statistics.
*/
extern int32
sys_humber_qos_policer_stats_enable(bool enable);


/**
 @brief Get QoS policer statistics global enable status.
*/
extern int32
sys_humber_qos_get_policer_stats_enable(bool* p_enable);


/**
 @brief Get flow policer statistics.
*/
extern int32
sys_humber_qos_get_flow_policer_stats_result(uint32 plc_id, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear flow policer statistics.
*/
extern int32
sys_humber_qos_clear_flow_policer_stats_result(uint32 plc_id);


/**
 @brief Get port policer statistics.
*/
extern int32
sys_humber_qos_get_port_policer_stats_result(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats);


/**
 @brief Clear port policer statistics.
*/
extern int32
sys_humber_qos_clear_port_policer_stats_result(uint16 gport, ctc_direction_t dir, uint8 phb_offset);

extern int32
sys_humber_qos_policer_sequential_enable(bool enable);

extern int32
sys_humber_qos_policer_ipg_enable(bool enable);

extern int32
sys_humber_qos_policer_get_service_policer_num(uint32* service_policer_num);

/**
 @brief qos policer component initialization
*/
extern int32
sys_humber_qos_policer_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg);


#endif

