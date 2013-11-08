/**
 @file sys_humber_queue_shape.c

 @date 2010-01-13

 @version v2.0

*/

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_hash.h"
#include "ctc_vector.h"
#include "ctc_queue.h"

#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_queue_enq.h"

#include "drv_humber.h"
#include "drv_io.h"
#include "drv_humber_data_path.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

extern sys_queue_enq_ctl_t sys_enq_ctl;

ctc_hash_t *p_sys_queue_shape_hash[CTC_MAX_LOCAL_CHIP_NUM];
ctc_hash_t *p_sys_group_shape_hash[CTC_MAX_LOCAL_CHIP_NUM];

ctc_vector_t *p_sys_channel_shape_vec[CTC_MAX_LOCAL_CHIP_NUM];

sys_queue_shape_ctl_t sys_shape_ctl;

sys_queue_group_t *port_queue_group[CTC_MAX_LOCAL_CHIP_NUM][MAX_PORT_NUM_PER_CHIP];
sys_queue_group_t *p_sys_group_info[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_GROUP_NUM];

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

static int32
_sys_humber_queue_shape_ctl_init(void)
{
    q_mgr_queue_shape_ctl_t  qmgr_queue_shape_ctl;
    q_mgr_group_shape_ctl_t  qmgr_group_shape_ctl;
    q_mgr_sub_ch_shape_ctl_t qmgr_channel_shape_ctl;
    uint32 cmd;
    uint8  lchip, lchip_num;

    uint32 core_frequency;
    const uint16 core_frequency_625mhz = 625;
    core_frequency = drv_humber_get_core_freq();

    /* QmgrQueueShapeCtl: queue shape ctrl */
    if((core_frequency < core_frequency_625mhz) &&
        (3907*core_frequency/core_frequency_625mhz > 2048))
    {
        sys_shape_ctl.queue_shape_max_ptr = 3907*core_frequency/core_frequency_625mhz - 1;
        sys_shape_ctl.queue_shape_update_max_cnt = 5 - 1;
    }
    else
    {
        sys_shape_ctl.queue_shape_max_ptr = 3907 - 1;
        sys_shape_ctl.queue_shape_update_max_cnt = 10*core_frequency/core_frequency_625mhz - 1;
    }

    sys_shape_ctl.queue_shape_min_ptr = 0;
    sys_shape_ctl.queue_shape_max_phy_ptr = 2048 - 1;
    sys_shape_ctl.queue_shape_low_bw_max_ptr = sys_shape_ctl.queue_shape_max_ptr/2;
    sys_shape_ctl.queue_shape_high_bw_min_ptr = sys_shape_ctl.queue_shape_max_ptr/2 + 1;
    sys_shape_ctl.queue_shape_low_bw_weight = 1;
    sys_shape_ctl.queue_shape_high_bw_weight = 1;

    kal_memset(&qmgr_queue_shape_ctl, 0, sizeof(q_mgr_queue_shape_ctl_t));
    qmgr_queue_shape_ctl.que_shp_max_ptr       = sys_shape_ctl.queue_shape_max_ptr;
    qmgr_queue_shape_ctl.que_shp_min_ptr       = sys_shape_ctl.queue_shape_min_ptr;
    qmgr_queue_shape_ctl.que_shp_max_phy_ptr   = sys_shape_ctl.queue_shape_max_phy_ptr;
    qmgr_queue_shape_ctl.que_shp_lo_bw_max_ptr = sys_shape_ctl.queue_shape_low_bw_max_ptr;
    qmgr_queue_shape_ctl.que_shp_hi_bw_min_ptr = sys_shape_ctl.queue_shape_high_bw_min_ptr;
    qmgr_queue_shape_ctl.que_shp_lo_bw_weight  = sys_shape_ctl.queue_shape_low_bw_weight;
    qmgr_queue_shape_ctl.que_shp_hi_bw_weight  = sys_shape_ctl.queue_shape_high_bw_weight;
    qmgr_queue_shape_ctl.que_shp_upd_max_cnt   = sys_shape_ctl.queue_shape_update_max_cnt;

    /* QmgrGroupShapeCtl: group shape ctrl */
    if((core_frequency < core_frequency_625mhz) &&
        (256*core_frequency/core_frequency_625mhz > 256))
    {
        sys_shape_ctl.group_shape_max_ptr = 256*core_frequency/core_frequency_625mhz - 1;
        sys_shape_ctl.group_shape_update_max_cnt = 40 - 1;
    }
    else
    {
        sys_shape_ctl.group_shape_max_ptr = 256 - 1;
        sys_shape_ctl.group_shape_update_max_cnt = 40*core_frequency/core_frequency_625mhz - 1;
    }

    sys_shape_ctl.group_shape_min_ptr         = 0;
    sys_shape_ctl.group_shape_max_phy_ptr     = SYS_MAX_GROUP_SHAPE_PROFILE_NUM - 1;
    sys_shape_ctl.group_shape_high_bw_min_ptr = SYS_MAX_GROUP_SHAPE_PROFILE_NUM / 2;
    sys_shape_ctl.group_shape_low_bw_max_ptr  = SYS_MAX_GROUP_SHAPE_PROFILE_NUM / 2 - 1;
    sys_shape_ctl.group_shape_high_bw_weight  = 1;
    sys_shape_ctl.group_shape_low_bw_weight   = 1;

    kal_memset(&qmgr_group_shape_ctl, 0, sizeof(q_mgr_group_shape_ctl_t));
    qmgr_group_shape_ctl.grp_shp_max_ptr       = sys_shape_ctl.group_shape_max_ptr;
    qmgr_group_shape_ctl.grp_shp_min_ptr       = sys_shape_ctl.group_shape_min_ptr;
    qmgr_group_shape_ctl.grp_shp_max_phy_ptr   = sys_shape_ctl.group_shape_max_phy_ptr;
    qmgr_group_shape_ctl.grp_shp_hi_bw_min_ptr = sys_shape_ctl.group_shape_high_bw_min_ptr;
    qmgr_group_shape_ctl.grp_shp_lo_bw_max_ptr = sys_shape_ctl.group_shape_low_bw_max_ptr;
    qmgr_group_shape_ctl.grp_shp_hi_bw_weight  = sys_shape_ctl.group_shape_high_bw_weight;
    qmgr_group_shape_ctl.grp_shp_lo_bw_weight  = sys_shape_ctl.group_shape_low_bw_weight;
    qmgr_group_shape_ctl.grp_shp_upd_max_cnt   = sys_shape_ctl.group_shape_update_max_cnt;

    /* QmgrChShapeCtl: channel shape ctrl */
    if((core_frequency < core_frequency_625mhz) &&
        (196*core_frequency/core_frequency_625mhz > 96))
    {
        sys_shape_ctl.channel_shape_max_ptr = 196*core_frequency/core_frequency_625mhz - 1;
        sys_shape_ctl.channel_shape_update_max_cnt = 100 - 1;
    }
    else
    {
        sys_shape_ctl.channel_shape_max_ptr = 196 - 1;
        sys_shape_ctl.channel_shape_update_max_cnt = 100*core_frequency/core_frequency_625mhz - 1;
    }

    sys_shape_ctl.channel_shape_min_ptr = 0;
    sys_shape_ctl.channel_shape_max_phy_ptr = 96 - 1;

    kal_memset(&qmgr_channel_shape_ctl, 0, sizeof(q_mgr_sub_ch_shape_ctl_t));
    qmgr_channel_shape_ctl.shape_max_ptr     = sys_shape_ctl.channel_shape_max_ptr;
    qmgr_channel_shape_ctl.shape_min_ptr     = sys_shape_ctl.channel_shape_min_ptr;
    qmgr_channel_shape_ctl.shape_max_phy_ptr = sys_shape_ctl.channel_shape_max_phy_ptr;
    qmgr_channel_shape_ctl.shape_upd_max_cnt = sys_shape_ctl.channel_shape_update_max_cnt;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* init queue shape ctrl */
        cmd = DRV_IOW(IOC_REG, Q_MGR_QUEUE_SHAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qmgr_queue_shape_ctl));

        /* init group shape ctrl */
        cmd = DRV_IOW(IOC_REG, Q_MGR_GROUP_SHAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qmgr_group_shape_ctl));

        /* init channel shape ctrl */
        cmd = DRV_IOW(IOC_REG, Q_MGR_SUB_CH_SHAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qmgr_channel_shape_ctl));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_queue_init_queue_shape(void)
{
    ds_queue_shape_profile_t ds_profile;
    ds_queue_shape_profile_id_t ds_profile_id;
    uint16 queue_id;
    uint32 cmd;
    uint32 tmp;
    uint8  lchip, lchip_num;

    /* reserved queue shape profile */
    ds_profile.que_commit_token_rate       = 0x3FFFF;
    ds_profile.que_commit_token_thrd       = 0xFF;
    ds_profile.que_commit_token_thrd_shift = 0xF;
    ds_profile.que_peak_token_rate         = 0x3FFFF;
    ds_profile.que_peak_token_thrd         = 0xFF;
    ds_profile.que_peak_token_thrd_shift   = 0xF;

    /* link all queue to the reserved shape profile */
    kal_memset(&ds_profile_id, 0, sizeof(ds_queue_shape_profile_id_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* reserved shape profile (index = 0) */
        cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, 0, cmd, &ds_profile));

        for (queue_id = 0; queue_id < SYS_MAX_QUEUE_NUM; queue_id++)
        {
            /* shape profile id */
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE_ID, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id / 4, cmd, &ds_profile_id));

            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DS_QUEUE_SHAPE_COMMIT_TOKEN);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DS_QUEUE_SHAPE_PEAK_TOKEN);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

            /* enable queue shape */
            tmp = 1;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_QUE_SHP_EN);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_init_group_shape(void)
{
    ds_group_shape_profile_t ds_profile;
    uint32 cmd;
    uint8  lchip, lchip_num;

    /* reserved group shape profile */
    ds_profile.grp_token_rate       = 0x3FFFFF;
    ds_profile.grp_token_thrd       = 0xFF;
    ds_profile.grp_token_thrd_shift = 0xF;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* reserved group shape profile (index = 0) */
        cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, 0, cmd, &ds_profile));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_init_channel_shape()
{
    uint8 lchip, lchip_num;
    uint16 channel_id;
    uint32 cmd;
    uint32 tmp;
    ds_channel_shape_profile_t ds_profile;

    ds_profile.shape_en = 1;
    ds_profile.token_rate = 0x3FFFF;
    ds_profile.token_thrd = 0xFF;
    ds_profile.token_thrd_shift = 0xF;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        for(channel_id=0; channel_id<96; channel_id++)
        {
            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE, DS_CHANNEL_SHAPE_TOKEN);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel_id, cmd, &tmp));

            cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE_PROFILE, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel_id, cmd, &ds_profile));
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Queue shape hash key hook.
*/
static INLINE uint32
_sys_humber_queue_shape_hash_key(void* data)
{
    sys_queue_shape_profile_t *p_profile = (sys_queue_shape_profile_t *)data;

    return (p_profile->commit_rate + (p_profile->commit_threshold << p_profile->commit_shift) +
            p_profile->peak_rate   + (p_profile->peak_threshold   << p_profile->peak_shift));
}

/**
 @brief Queue shape hash comparison hook.
*/
static INLINE bool
_sys_humber_queue_shape_hash_cmp(void* data1, void* data2)
{
    sys_queue_shape_profile_t *p_profile1 = (sys_queue_shape_profile_t *)data1;
    sys_queue_shape_profile_t *p_profile2 = (sys_queue_shape_profile_t *)data2;

    if ((p_profile1->commit_rate      == p_profile2->commit_rate) &&
        (p_profile1->commit_threshold == p_profile2->commit_threshold) &&
        (p_profile1->commit_shift     == p_profile2->commit_shift) &&
        (p_profile1->peak_rate        == p_profile2->peak_rate) &&
        (p_profile1->peak_threshold   == p_profile2->peak_threshold) &&
        (p_profile1->peak_shift       == p_profile2->peak_shift))
    {
        return TRUE;
    }

    return FALSE;
}


/**
 @brief Group shape hash key hook.
*/
static INLINE uint32
_sys_humber_group_shape_hash_key(void* data)
{
    sys_group_shape_profile_t *p_profile = (sys_group_shape_profile_t *)data;

    return (p_profile->rate + (p_profile->threshold << p_profile->shift));
}


/**
 @brief Group shape hash comparison hook.
*/
static INLINE bool
_sys_humber_group_shape_hash_cmp(void* data1, void* data2)
{
    sys_group_shape_profile_t *p_profile1 = (sys_group_shape_profile_t *)data1;
    sys_group_shape_profile_t *p_profile2 = (sys_group_shape_profile_t *)data2;

    if ((p_profile1->rate      == p_profile2->rate) &&
        (p_profile1->threshold == p_profile2->threshold) &&
        (p_profile1->shift     == p_profile2->shift))
    {
        return TRUE;
    }

    return FALSE;
}


/**
 @brief Compute queue shape token rate.
*/
static int32
_sys_humber_queue_shape_token_rate_compute(uint16 queue_id, uint32 rate, uint32* p_token_rate)
{
    uint64 dividend;
    uint64 divisor;
    uint64 tmp64;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_token_rate);

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    if ((queue_id <= sys_shape_ctl.queue_shape_max_ptr) &&
        (queue_id >= sys_shape_ctl.queue_shape_high_bw_min_ptr))
    {
        dividend = (uint64)sys_shape_ctl.queue_shape_high_bw_weight * core_frequency;
        divisor = (uint64)(sys_shape_ctl.queue_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.queue_shape_high_bw_weight + sys_shape_ctl.queue_shape_low_bw_weight) *
            (sys_shape_ctl.queue_shape_max_ptr - sys_shape_ctl.queue_shape_high_bw_min_ptr + 1);
    }
    else
    {
        dividend = (uint64)sys_shape_ctl.queue_shape_low_bw_weight * core_frequency;
        divisor = (uint64)(sys_shape_ctl.queue_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.queue_shape_high_bw_weight + sys_shape_ctl.queue_shape_low_bw_weight) *
            (sys_shape_ctl.queue_shape_low_bw_max_ptr - sys_shape_ctl.queue_shape_min_ptr + 1);
    }

    tmp64 = rate * divisor / dividend;

    if (tmp64 >= (1 << 22))
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }
    else
    {
        *p_token_rate = (uint32)tmp64;
    }

    return CTC_E_NONE;
}


/**
 @brief Compute queue shape token threshold.
*/
static int32
_sys_humber_queue_shape_threshold_compute(uint32 burst, uint8 *p_threshold, uint8 *p_shift, bool round_up)
{
#define MAX_QUEUE_THRESHOLD 256
    int32 i;
    int32 value = burst / MAX_QUEUE_THRESHOLD;

    static uint16 exponent[][2] = {
        {0,                      0},
        {(1 << 0),    (1 << 1) - 1},
        {(1 << 1),    (1 << 2) - 1},
        {(1 << 2),    (1 << 3) - 1},
        {(1 << 3),    (1 << 4) - 1},
        {(1 << 4),    (1 << 5) - 1},
        {(1 << 5),    (1 << 6) - 1},
        {(1 << 6),    (1 << 7) - 1},
        {(1 << 7),    (1 << 8) - 1},
        {(1 << 8),    (1 << 9) - 1},
        {(1 << 9),   (1 << 10) - 1},
        {(1 << 10),  (1 << 11) - 1},
        {(1 << 11),  (1 << 12) - 1},
        {(1 << 12),  (1 << 13) - 1},
        {(1 << 13),  (1 << 14) - 1},
        {(1 << 14),  (1 << 15) - 1}
    };

    CTC_PTR_VALID_CHECK(p_threshold);
    CTC_PTR_VALID_CHECK(p_shift);

    SYS_QUEUE_DBG_FUNC();

    for (i = 0; i < sizeof(exponent) / (2 * sizeof(uint16)); i++)
    {
        if ((value >= exponent[i][0]) && (value <= exponent[i][1]))
        {
             /* round up value */
            if (((burst + ((1 << i) - 1)) / (1 << i) >= MAX_QUEUE_THRESHOLD) || round_up)
            {
                i++;
            }

            *p_shift = i;
            *p_threshold = (burst + ((1 << i) - 1)) / (1 << i);

            return CTC_E_NONE;
        }
    }

    return CTC_E_EXCEED_MAX_SIZE;
}



/**
 @brief Mapping queue shape profile.
*/
static int32
_sys_humber_queue_shape_profile_map(uint8 lchip, uint16 queue_id, ctc_queue_shape_t* p_shape, sys_queue_shape_profile_t* p_profile)
{
    uint32 burst;

    CTC_PTR_VALID_CHECK(p_shape);
    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* compute commit token rate and peak token rate */
    CTC_ERROR_RETURN(_sys_humber_queue_shape_token_rate_compute(queue_id, p_shape->cir, &p_profile->commit_rate));
    CTC_ERROR_RETURN(_sys_humber_queue_shape_token_rate_compute(queue_id, p_shape->pir, &p_profile->peak_rate));

    /* compute commit token threshold */
    if (p_shape->cbs != SYS_SPECIAL_SHAPE_BURST)
    {
        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(p_shape->cbs, &p_profile->commit_threshold, &p_profile->commit_shift, FALSE));
    }
    else
    {
        burst = (p_profile->commit_rate * 100) >> 1;

        if (burst < SYS_MIN_SHAPE_BURST)
        {
            burst = SYS_MIN_SHAPE_BURST;
        }

        if (burst > SYS_MAX_SHAPE_BURST)
        {
            burst = SYS_MAX_SHAPE_BURST;
        }

        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(burst, &p_profile->commit_threshold, &p_profile->commit_shift, TRUE));
    }

    /* compute peak token threshold */
    if (p_shape->pbs != SYS_SPECIAL_SHAPE_BURST)
    {
        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(p_shape->pbs, &p_profile->peak_threshold, &p_profile->peak_shift, FALSE));
    }
    else
    {
        burst = (p_profile->peak_rate * 100) >> 1;

        if (burst < SYS_MIN_SHAPE_BURST)
        {
            burst = SYS_MIN_SHAPE_BURST;
        }

        if (burst > SYS_MAX_SHAPE_BURST)
        {
            burst = SYS_MAX_SHAPE_BURST;
        }

        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(burst, &p_profile->peak_threshold, &p_profile->peak_shift, TRUE));
    }

    SYS_QUEUE_DBG_INFO("queue shape profile mapping:\n");
    SYS_QUEUE_DBG_INFO("----------------------------\n");
    SYS_QUEUE_DBG_INFO("    cir = %u --> commit_rate = %d\n", p_shape->cir, p_profile->commit_rate);
    SYS_QUEUE_DBG_INFO("    cbs = %u --> commit_threshold = %d, commit_shift = %d\n",
        p_shape->cbs, p_profile->commit_threshold, p_profile->commit_shift);
    SYS_QUEUE_DBG_INFO("    pir = %u --> peak_rate = %d\n", p_shape->pir, p_profile->peak_rate);
    SYS_QUEUE_DBG_INFO("    pbs = %u --> peak_threshold = %d, peak_shift = %d\n",
        p_shape->pbs, p_profile->peak_threshold, p_profile->peak_shift);

    return CTC_E_NONE;
}


/**
 @brief Create queue shape profile.
*/
static int32
_sys_humber_queue_shape_profile_create(uint8 lchip, sys_queue_shape_profile_t* p_profile, sys_queue_shape_profile_t** pp_profile)
{
    sys_queue_shape_profile_t *p_new_profile;

    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    p_new_profile = (sys_queue_shape_profile_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_queue_shape_profile_t));
    if (!p_new_profile)
    {
        return CTC_E_NO_MEMORY;
    }

    p_new_profile->commit_rate      = p_profile->commit_rate;
    p_new_profile->commit_threshold = p_profile->commit_threshold;
    p_new_profile->commit_shift     = p_profile->commit_shift;
    p_new_profile->peak_rate        = p_profile->peak_rate;
    p_new_profile->peak_threshold   = p_profile->peak_threshold;
    p_new_profile->peak_shift       = p_profile->peak_shift;

    p_new_profile->ref   = 1;
    p_new_profile->index = 0;

    ctc_hash_insert(p_sys_queue_shape_hash[lchip], p_new_profile);

    *pp_profile = p_new_profile;

    SYS_QUEUE_DBG_INFO("p_profile->commit_rate = %d\n", p_profile->commit_rate);
    SYS_QUEUE_DBG_INFO("p_profile->commit_threshold = %d\n", p_profile->commit_threshold);
    SYS_QUEUE_DBG_INFO("p_profile->commit_shift = %d\n", p_profile->commit_shift);
    SYS_QUEUE_DBG_INFO("p_profile->peak_rate = %d\n", p_profile->peak_rate);
    SYS_QUEUE_DBG_INFO("p_profile->peak_threshold = %d\n", p_profile->peak_threshold);
    SYS_QUEUE_DBG_INFO("p_profile->peak_shift = %d\n", p_profile->peak_shift);

    return CTC_E_NONE;
}


/**
 @brief Remove queue shape profile.
*/
static int32
_sys_humber_queue_shape_profile_remove(uint8 lchip, sys_queue_shape_profile_t* p_profile)
{
    sys_humber_opf_t opf;

    SYS_QUEUE_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_profile);

    CTC_NOT_ZERO_CHECK(p_profile->ref);
    if (--p_profile->ref)
    {
        return CTC_E_NONE;
    }

    /* remove from asic */

    /* get available shape profile index */
    opf.pool_type = OPF_QUEUE_SHAPE_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_profile->index));

    SYS_QUEUE_DBG_INFO("remove queue shape profile, lchip = %d, index = %d\n", lchip, p_profile->index);

    /* remove from db */
    ctc_hash_remove(p_sys_queue_shape_hash[lchip], p_profile);

    mem_free(p_profile);

    return CTC_E_NONE;
}


/**
 @brief Lookup queue shape profile in hash table.
*/
static int32
_sys_humber_queue_shape_profile_lookup(uint8 lchip, sys_queue_shape_profile_t* p_profile, sys_queue_shape_profile_t** pp_profile)
{
    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    *pp_profile = ctc_hash_lookup(p_sys_queue_shape_hash[lchip], p_profile);

    return CTC_E_NONE;
}


/**
 @brief Write queue shape profile to ASIC.
*/
static int32
_sys_humber_queue_shape_profile_write(uint8 lchip, sys_queue_shape_profile_t* p_profile)
{
    ds_queue_shape_profile_t ds_profile;
    sys_humber_opf_t opf;
    uint32 offset;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* get available shape profile index */
    opf.pool_type = OPF_QUEUE_SHAPE_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

    p_profile->index = offset;

    /* write profile to asic */
    ds_profile.que_commit_token_rate       = p_profile->commit_rate;
    ds_profile.que_commit_token_thrd       = p_profile->commit_threshold;
    ds_profile.que_commit_token_thrd_shift = p_profile->commit_shift;
    ds_profile.que_peak_token_rate         = p_profile->peak_rate;
    ds_profile.que_peak_token_thrd         = p_profile->peak_threshold;
    ds_profile.que_peak_token_thrd_shift   = p_profile->peak_shift;

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_profile->index, cmd, &ds_profile));

    SYS_QUEUE_DBG_INFO("write queue shape profile, lchip = %d, index = %d\n", lchip, p_profile->index);
    SYS_QUEUE_DBG_INFO("commit_token_rate = %u, commit_token_thrd = %u, commit_token_thrd_shift = %u\n",
                       p_profile->commit_rate, p_profile->commit_threshold, p_profile->commit_shift);
    SYS_QUEUE_DBG_INFO("peak_token_rate = %u, peak_token_thrd = %u, peak_token_thrd_shift = %u\n",
                       p_profile->peak_rate, p_profile->peak_threshold, p_profile->peak_shift);

    return CTC_E_NONE;
}


/**
 @brief Create channel shape profile.
*/
static int32
_sys_humber_channel_shape_profile_create(uint8 lchip, uint8 channel, sys_channel_shape_profile_t* p_profile, sys_channel_shape_profile_t** pp_profile)
{
    sys_channel_shape_profile_t *p_new_profile;

    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    if (ctc_vector_get(p_sys_channel_shape_vec[lchip], channel))
    {
        return CTC_E_CHANNEL_SHAPE_PROF_EXIST;
    }

    p_new_profile = (sys_channel_shape_profile_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_channel_shape_profile_t));
    if (!p_new_profile)
    {
        return CTC_E_NO_MEMORY;
    }

    p_new_profile->rate      = p_profile->rate;
    p_new_profile->threshold = p_profile->threshold;
    p_new_profile->shift     = p_profile->shift;

    ctc_vector_add(p_sys_channel_shape_vec[lchip], channel, p_new_profile);

    *pp_profile = p_new_profile;

    SYS_QUEUE_DBG_INFO("create channel shape profile, lchip = %d, channel = %d\n", lchip, channel);
    SYS_QUEUE_DBG_INFO("p_profile->rate = %d\n", p_profile->rate);
    SYS_QUEUE_DBG_INFO("p_profile->threshold = %d\n", p_profile->threshold);
    SYS_QUEUE_DBG_INFO("p_profile->shift = %d\n", p_profile->shift);

    return CTC_E_NONE;
}


/**
 @brief Remove channel shape profile.
*/
static int32
_sys_humber_channel_shape_profile_remove(uint8 lchip, uint8 channel)
{
    sys_channel_shape_profile_t *p_profile;
    ds_channel_shape_profile_t ds_profile;
    uint32 cmd;

    SYS_QUEUE_DBG_FUNC();

    /* remove channel shape profile from asic */
    ds_profile.shape_en = 1;
    ds_profile.token_rate = 0x3FFFF;
    ds_profile.token_thrd = 0xFF;
    ds_profile.token_thrd_shift = 0xF;

    cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel, cmd, &ds_profile));

    SYS_QUEUE_DBG_INFO("remove channel shape profile, lchip = %d, channel = %d\n", lchip, channel);

    /* remove channel shape profile from db */
    p_profile = ctc_vector_del(p_sys_channel_shape_vec[lchip], channel);
    if (p_profile)
    {
        mem_free(p_profile);
    }

    return CTC_E_NONE;
}


/**
 @brief Write channel shape profile.
*/
int32
_sys_humber_channel_shape_profile_write(uint8 lchip, uint8 channel, sys_channel_shape_profile_t* p_profile)
{
    ds_channel_shape_profile_t ds_profile;
    uint32 cmd;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    ds_profile.shape_en = 1;
    ds_profile.token_rate = p_profile->rate;
    ds_profile.token_thrd = p_profile->threshold;
    ds_profile.token_thrd_shift = p_profile->shift;

    /* write ds channel shape */
    cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE, DS_CHANNEL_SHAPE_TOKEN);
    tmp = ds_profile.token_thrd << ds_profile.token_thrd_shift;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel, cmd, &tmp));

    cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel, cmd, &ds_profile));

    SYS_QUEUE_DBG_INFO("write channel shape profile, lchip = %d, channel = %d\n", lchip, channel);
    SYS_QUEUE_DBG_INFO("token_rate = %u, token_threshold = %u, token_shift = %u\n",
                       p_profile->rate, p_profile->threshold, p_profile->shift);

    return CTC_E_NONE;
}



/**
 @brief Compute channel shape token rate.
*/
static int32
_sys_humber_channel_shape_token_rate_compute(uint8 lchip, uint32 rate, uint32 *p_token_rate)
{
    uint32 cycle;
    uint64 tmp64;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_token_rate);

    SYS_QUEUE_DBG_FUNC();

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    cycle = (sys_shape_ctl.channel_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.channel_shape_max_ptr - sys_shape_ctl.channel_shape_min_ptr + 1);

    tmp64 = (uint64)rate * cycle / core_frequency;

    if (tmp64 >= (1 << 22))
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }
    else
    {
        *p_token_rate = (uint32)tmp64;
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_group_shape_token_rate_compute(uint8 lchip, uint32 rate, uint32 *p_token_rate)
{
    uint64 dividend;
    uint64 divisor;
    uint64 tmp64;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_token_rate);

    SYS_QUEUE_DBG_FUNC();

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    dividend = (uint64)sys_shape_ctl.group_shape_low_bw_weight * core_frequency;
    divisor = (uint64)(sys_shape_ctl.group_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
              (sys_shape_ctl.group_shape_high_bw_weight + sys_shape_ctl.group_shape_low_bw_weight) *
              (sys_shape_ctl.group_shape_low_bw_max_ptr - sys_shape_ctl.group_shape_min_ptr + 1);

    tmp64 = rate * divisor / dividend;

    if (tmp64 >= (1 << 22))
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }
    else
    {
        *p_token_rate = (uint32)tmp64;
    }

    return CTC_E_NONE;
}



/**
 @brief Set group for the given queue in a chip.
*/
static int32
_sys_humber_queue_set_group(uint8 lchip, uint16 queue_id, uint16 group)
{
    uint32 cmd;
    uint32 tmp;

    SYS_QUEUE_DBG_FUNC();

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_GRP_ID);
    tmp = group;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

    sys_queue_config[lchip][queue_id].group = group;

    SYS_QUEUE_DBG_INFO("set queue to group, lchip = %d, queue_id = %d, group = %d\n", lchip, queue_id, group);

    return CTC_E_NONE;
}



static int32
_sys_humber_group_shape_profile_map(uint8 lchip, ctc_group_shape_t* p_shape, sys_group_shape_profile_t* p_profile)
{
    uint32 burst;

    CTC_PTR_VALID_CHECK(p_shape);
    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* compute token rate */
    CTC_ERROR_RETURN(_sys_humber_group_shape_token_rate_compute(lchip, p_shape->pir, &p_profile->rate));

    /* compute token threshold */
    if (p_shape->pbs != SYS_SPECIAL_SHAPE_BURST)
    {
        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(p_shape->pbs, &p_profile->threshold, &p_profile->shift, FALSE));
    }
    else
    {
        burst = (p_profile->rate * 100) >> 1;

        if (burst < SYS_MIN_SHAPE_BURST)
        {
            burst = SYS_MIN_SHAPE_BURST;
        }

        if (burst > SYS_MAX_SHAPE_BURST)
        {
            burst = SYS_MAX_SHAPE_BURST;
        }

        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(burst, &p_profile->threshold, &p_profile->shift, TRUE));
    }

    SYS_QUEUE_DBG_INFO("group shape profile mapping:\n");
    SYS_QUEUE_DBG_INFO("----------------------------\n");
    SYS_QUEUE_DBG_INFO("    pir = %u --> token_rate = %u\n", p_shape->pir, p_profile->rate);
    SYS_QUEUE_DBG_INFO("    pbs = %u --> token_threshold = %u, token_shift = %u\n",
        p_shape->pbs, p_profile->threshold, p_profile->shift);

    return CTC_E_NONE;
}


/**
 @brief Create group shape profile.
*/
static int32
_sys_humber_group_shape_profile_create(uint8 lchip, sys_group_shape_profile_t* p_profile, sys_group_shape_profile_t** pp_profile)
{
    sys_group_shape_profile_t *p_new_profile;
    sys_humber_opf_t opf;
    uint32 offset;

    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    opf.pool_type = OPF_GROUP_SHAPE_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

    p_new_profile = (sys_group_shape_profile_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_group_shape_profile_t));
    if (!p_new_profile)
    {
        return CTC_E_NO_MEMORY;
    }

    p_new_profile->rate      = p_profile->rate;
    p_new_profile->threshold = p_profile->threshold;
    p_new_profile->shift     = p_profile->shift;

    p_new_profile->ref   = 1;
    p_new_profile->index = offset;

    ctc_hash_insert(p_sys_group_shape_hash[lchip], p_new_profile);

    *pp_profile = p_new_profile;

    SYS_QUEUE_DBG_INFO("create group shape profile, lchip = %d, index = %d\n", lchip, offset);
    SYS_QUEUE_DBG_INFO("p_profile->rate = %d\n", p_profile->rate);
    SYS_QUEUE_DBG_INFO("p_profile->threshold = %d\n", p_profile->threshold);
    SYS_QUEUE_DBG_INFO("p_profile->shift = %d\n", p_profile->shift);

    return CTC_E_NONE;
}


/**
 @brief Remove queue shape profile.
*/
static int32
_sys_humber_group_shape_profile_remove(uint8 lchip, sys_group_shape_profile_t* p_profile)
{
    ds_group_shape_profile_t ds_profile;
    sys_humber_opf_t opf;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    CTC_NOT_ZERO_CHECK(p_profile->ref);
    if (--p_profile->ref)
    {
        return CTC_E_NONE;
    }

    /* remove from asic */
    kal_memset(&ds_profile, 0, sizeof(ds_profile));
    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_profile->index, cmd, &ds_profile));

    SYS_QUEUE_DBG_INFO("remove group shape profile, lchip = %d, index = %d\n", lchip, p_profile->index);

    opf.pool_type = OPF_GROUP_SHAPE_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_profile->index));

    /* remove from db */
    ctc_hash_remove(p_sys_group_shape_hash[lchip], p_profile);

    mem_free(p_profile);

    return CTC_E_NONE;
}



/**
 @brief Write group shape profile to ASIC.
*/
static int32
_sys_humber_group_shape_profile_write(uint8 lchip, sys_group_shape_profile_t* p_profile)
{
    ds_group_shape_profile_t ds_profile;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* write profile to asic */
    ds_profile.grp_token_rate       = p_profile->rate;
    ds_profile.grp_token_thrd       = p_profile->threshold;
    ds_profile.grp_token_thrd_shift = p_profile->shift;

    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_profile->index, cmd, &ds_profile));


    SYS_QUEUE_DBG_INFO("write group shape profile, lchip = %d, index = %d\n", lchip, p_profile->index);
    SYS_QUEUE_DBG_INFO("grp_token_rate = %u\n", p_profile->rate);
    SYS_QUEUE_DBG_INFO("grp_token_thrd = %u\n", p_profile->threshold);
    SYS_QUEUE_DBG_INFO("grp_token_thrd_shift = %u\n", p_profile->shift);

    return CTC_E_NONE;
}


/**
 @brief Lookup group shape profile.
*/
static int32
_sys_humber_group_shape_profile_lookup(uint8 lchip, sys_group_shape_profile_t* p_profile, sys_group_shape_profile_t** pp_profile)
{
    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    *pp_profile = ctc_hash_lookup(p_sys_group_shape_hash[lchip], p_profile);

    return CTC_E_NONE;
}

int32
_sys_humber_get_port_queue_type_by_lport(uint8 lport, ctc_queue_type_t* queue_type)
{
    if(lport < SYS_ILOOP_CHANNEL_ID)
    {
        *queue_type = CTC_QUEUE_TYPE_NETWORK_EGRESS;
    }
    else if(lport == SYS_ILOOP_CHANNEL_ID)
    {
        *queue_type = CTC_QUEUE_TYPE_ILOOP;
    }
    else if(lport == SYS_CPU_CHANNEL_ID)
    {
        *queue_type = CTC_QUEUE_TYPE_NORMAL_CPU;
    }
    else if(lport == SYS_OAM_CHANNEL_ID)
    {
        *queue_type = CTC_QUEUE_TYPE_OAM;
    }
    else if(lport == SYS_ELOOP_CHANNEL_ID)
    {
        *queue_type = CTC_QUEUE_TYPE_ELOOP;
    }
    else if(lport <= SYS_STATIC_INT_PORT_END)
    {
        *queue_type = CTC_QUEUE_TYPE_STATIC_INT_PORT;
    }
    else
    {
        *queue_type = CTC_QUEUE_TYPE_INTERNAL_PORT;
    }

    return CTC_E_NONE;
}

int32
_sys_humber_get_per_port_queue_num_by_lport(uint8 lport, uint8* queue_num_per_port)
{
    if(lport < SYS_ILOOP_CHANNEL_ID)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_network_port;
    }
    else if(lport == SYS_ILOOP_CHANNEL_ID)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_network_port;
    }
    else if(lport == SYS_CPU_CHANNEL_ID)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_network_port;
    }
    else if(lport == SYS_OAM_CHANNEL_ID)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_network_port;
    }
    else if(lport == SYS_ELOOP_CHANNEL_ID)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_network_port;
    }
    else if(lport <= SYS_STATIC_INT_PORT_END)
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_static_int_port;
    }
    else
    {
        *queue_num_per_port = sys_enq_ctl.queue_num_per_internal_port;
    }

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable queue shaping function.
*/
int32
sys_humber_queue_set_queue_shape_global_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 tmp;
    uint32 cmd;

    SYS_QUEUE_DBG_FUNC();

    if (sys_shape_ctl.queue_shape_enable != enable)
    {
        tmp = enable ? 1 : 0;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_TABLE, Q_MGR_QUEUE_SHAPE_CTL, Q_MGR_QUEUE_SHAPE_CTL_QUE_SHP_GBL_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_TABLE, Q_MGR_QUEUE_SHAPE_CTL, Q_MGR_QUEUE_SHAPE_CTL_QUE_SHP_UPD_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        sys_shape_ctl.queue_shape_enable = enable;

        SYS_QUEUE_DBG_INFO("sys_shape_ctl.queue_shape_enable = %d\n", enable);
    }

    return CTC_E_NONE;
}


/**
 @brief Get queue shape global enable status.
*/
int32
sys_humber_queue_get_queue_shape_global_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QUEUE_DBG_FUNC();

    *p_enable = sys_shape_ctl.queue_shape_enable;

    SYS_QUEUE_DBG_INFO("sys_shape_ctl.queue_shape_enable = %d\n", *p_enable);

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable group shaping function.
*/
int32
sys_humber_queue_set_group_shape_global_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 tmp;
    uint32 cmd;

    SYS_QUEUE_DBG_FUNC();

    if (sys_shape_ctl.group_shape_enable != enable)
    {
        tmp = enable ? 1 : 0;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, Q_MGR_GROUP_SHAPE_CTL, Q_MGR_GROUP_SHAPE_CTL_GRP_SHP_GBL_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, Q_MGR_GROUP_SHAPE_CTL, Q_MGR_GROUP_SHAPE_CTL_GRP_SHP_UPD_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        sys_shape_ctl.group_shape_enable = enable;

        SYS_QUEUE_DBG_INFO("sys_shape_ctl.group_shape_enable = %d\n", enable);
    }

    return CTC_E_NONE;
}


/**
 @brief Get group shape global enable status.
*/
int32
sys_humber_queue_get_group_shape_global_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QUEUE_DBG_FUNC();

    *p_enable = sys_shape_ctl.group_shape_enable;

    SYS_QUEUE_DBG_INFO("sys_shape_ctl.group_shape_enable = %d\n", *p_enable);
    return CTC_E_NONE;
}


int32
sys_humber_queue_set_shape_ipg_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    q_mgr_ethernet_ipg_t ipg_ctl;

    SYS_QUEUE_DBG_FUNC();

    ipg_ctl.ipg0 = enable ? CTC_DEFAULT_IPG : 0;
    ipg_ctl.ipg1 = enable ? CTC_DEFAULT_IPG : 0;
    ipg_ctl.ipg2 = enable ? CTC_DEFAULT_IPG : 0;
    ipg_ctl.ipg3 = enable ? CTC_DEFAULT_IPG : 0;

    cmd = DRV_IOW(IOC_REG, Q_MGR_ETHERNET_IPG, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &ipg_ctl));
    }

    return CTC_E_NONE;
}


/**
 @brief Globally enable/disable channel shaping function.
*/
int32
sys_humber_queue_set_channel_shape_global_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 tmp;
    uint32 cmd;

    SYS_QUEUE_DBG_FUNC();

    if (sys_shape_ctl.channel_shape_enable != enable)
    {
        tmp = enable ? 1 : 0;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_TABLE, Q_MGR_SUB_CH_SHAPE_CTL, Q_MGR_SUB_CH_SHAPE_CTL_SHAPE_GBL_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_TABLE, Q_MGR_SUB_CH_SHAPE_CTL, Q_MGR_SUB_CH_SHAPE_CTL_SHAPE_UPD_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        sys_shape_ctl.channel_shape_enable = enable;

        SYS_QUEUE_DBG_INFO("sys_shape_ctl.channel_shape_enable = %d\n", enable);
    }

    return CTC_E_NONE;
}


/**
 @brief Get channel shape global enable stauts.
*/
int32
sys_humber_queue_get_channel_shape_global_enable(bool* p_enable)
{
    SYS_QUEUE_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = sys_shape_ctl.channel_shape_enable;

    SYS_QUEUE_DBG_INFO("sys_shape_ctl.channel_shape_enable = %d\n", *p_enable);

    return CTC_E_NONE;
}


/**
 @brief Set shaping for the given queue in a chip. Queue shaping support dual-rate dual-bucket algorithm.
        In different priority queues, the committed traffic is scheduled in SP mode and the exceeded traffic
        is serviced in WDRR mode.
*/
int32
sys_humber_queue_set_queue_shape(uint8 lchip, uint16 queue_id, ctc_queue_shape_t* p_shape)
{
    sys_queue_shape_profile_t profile;
    sys_queue_shape_profile_t *p_profile;
    uint32 field;
    uint32 cmd, tmp;
    int32 ret;

    ds_queue_shape_t queue_shape;
    CTC_PTR_VALID_CHECK(p_shape);
    CTC_MAX_VALUE_CHECK(p_shape->cir, SYS_MAX_SHAPE_RATE);
    CTC_MAX_VALUE_CHECK(p_shape->pir, SYS_MAX_SHAPE_RATE);

    SYS_QUEUE_DBG_FUNC();

    /* remove old shape profile */
    if (sys_queue_config[lchip][queue_id].p_queue_shape_profile)
    {
        CTC_ERROR_RETURN(_sys_humber_queue_shape_profile_remove(lchip, sys_queue_config[lchip][queue_id].p_queue_shape_profile));
        sys_queue_config[lchip][queue_id].p_queue_shape_profile = NULL;
    }

    CTC_ERROR_RETURN(_sys_humber_queue_shape_profile_map(lchip, queue_id, p_shape, &profile));

    p_profile = NULL;
    CTC_ERROR_RETURN(_sys_humber_queue_shape_profile_lookup(lchip, &profile, &p_profile));
    if (p_profile)  /* the same queue shape profile is existent */
    {
        p_profile->ref++;
    }
    else    /* not found existing profile */
    {
        /* create a new profile */
        CTC_ERROR_RETURN(_sys_humber_queue_shape_profile_create(lchip, &profile, &p_profile));

        /* write profile to asic */
        ret = _sys_humber_queue_shape_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_queue_shape_profile_remove(lchip, p_profile);
            return ret;
        }
    }

    /* write profile id to asic */
    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID0;
            break;

        case 1:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID1;
            break;

        case 2:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID2;
            break;

        default:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID3;
            break;
    }

    tmp = p_profile->index;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE_ID, field);
    ret = drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &tmp);
    if (ret)
    {
        goto err1;
    }

    queue_shape.commit_token = p_profile->commit_threshold << p_profile->commit_shift;
    queue_shape.peak_token = p_profile->peak_threshold << p_profile->peak_shift;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, (uint32) queue_id, cmd, &queue_shape);
    if (ret)
    {
        goto err2;
    }

    sys_queue_config[lchip][queue_id].p_queue_shape_profile = p_profile;

    return CTC_E_NONE;

err2:
    /* reset queue profile id */
    tmp = SYS_RESERVED_SHAPE_PROFILE_ID;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE_ID, field);
    drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &tmp);

err1:
    _sys_humber_queue_shape_profile_remove(lchip, p_profile);
    return ret;
}


/**
 @brief Unset shaping for the given queue in a chip.
*/
int32
sys_humber_queue_unset_queue_shape(uint8 lchip, uint16 queue_id)
{
    uint32 field;
    uint32 tmp, cmd;
    ds_queue_shape_t queue_shape;

    SYS_QUEUE_DBG_FUNC();

    if (!sys_queue_config[lchip][queue_id].p_queue_shape_profile)
    {
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(_sys_humber_queue_shape_profile_remove(lchip, sys_queue_config[lchip][queue_id].p_queue_shape_profile));
    sys_queue_config[lchip][queue_id].p_queue_shape_profile = NULL;

    /* write profile id to asic */
    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID0;
            break;

        case 1:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID1;
            break;

        case 2:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID2;
            break;

        default:
            field = DS_QUEUE_SHAPE_PROFILE_ID_QUE_SHP_PROF_ID3;
            break;
    }

    tmp = SYS_RESERVED_SHAPE_PROFILE_ID;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE_ID, field);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id / 4, cmd, &tmp));

    queue_shape.commit_token = SYS_FULL_SHAPE_TOKENS;
    queue_shape.peak_token = SYS_FULL_SHAPE_TOKENS;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) queue_id, cmd, &queue_shape));

    return CTC_E_NONE;
}


/**
 @brief Get shaping for the given queue in a chip.
*/
int32
sys_humber_queue_get_queue_shape(uint8 lchip, uint16 queue_id, ctc_queue_shape_t* p_shape)
{
    sys_queue_shape_profile_t *p_profile;
    uint64 dividend;
    uint64 divisor;
    uint64 tmp64;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_shape);

    SYS_QUEUE_DBG_FUNC();

    p_profile = sys_queue_config[lchip][queue_id].p_queue_shape_profile;
    if (!p_profile)
    {
        return CTC_E_QUEUE_SHAPE_PROF_NOT_EXIST;
    }

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    /* compute cir and pir by token rate */
    if ((queue_id <= sys_shape_ctl.queue_shape_max_ptr) &&
        (queue_id >= sys_shape_ctl.queue_shape_high_bw_min_ptr))
    {
        divisor = (uint64)sys_shape_ctl.queue_shape_high_bw_weight * core_frequency;
        dividend = (uint64)(sys_shape_ctl.queue_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.queue_shape_high_bw_weight + sys_shape_ctl.queue_shape_low_bw_weight) *
            (sys_shape_ctl.queue_shape_max_ptr - sys_shape_ctl.queue_shape_high_bw_min_ptr + 1);
    }
    else
    {
        divisor = (uint64)sys_shape_ctl.queue_shape_low_bw_weight * core_frequency;
        dividend = (uint64)(sys_shape_ctl.queue_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.queue_shape_high_bw_weight + sys_shape_ctl.queue_shape_low_bw_weight) *
            (sys_shape_ctl.queue_shape_low_bw_max_ptr - sys_shape_ctl.queue_shape_min_ptr + 1);
    }
    tmp64 = p_profile->commit_rate * divisor / dividend;
    p_shape->cir = (uint32)tmp64;

    tmp64 = p_profile->peak_rate * divisor / dividend;
    p_shape->pir = (uint32)tmp64;

    /* compute cbs and pbs by token threshold and shift */
    p_shape->cbs = p_profile->commit_threshold << p_profile->commit_shift;
    p_shape->pbs = p_profile->peak_threshold << p_profile->peak_shift;

    SYS_QUEUE_DBG_INFO("get queue shape, lchip = %d, queue_id = %d\n", lchip, queue_id);
    SYS_QUEUE_DBG_INFO("p_shape->cir = %d\n", p_shape->cir);
    SYS_QUEUE_DBG_INFO("p_shape->cbs = %d\n", p_shape->cbs);
    SYS_QUEUE_DBG_INFO("p_shape->pir = %d\n", p_shape->pir);
    SYS_QUEUE_DBG_INFO("p_shape->pbs = %d\n", p_shape->pbs);

    return CTC_E_NONE;
}



/**
 @brief Set shaping for the given channel in a chip. Channel shaping supports single-rate single-bucket algorithm.
*/
int32
sys_humber_queue_set_channel_shape(uint8 lchip, uint8 channel, ctc_port_shape_t* p_shape)
{
    sys_channel_shape_profile_t profile;
    sys_channel_shape_profile_t *p_profile;
    uint32 burst;
    int32  ret;

    CTC_PTR_VALID_CHECK(p_shape);
    CTC_MAX_VALUE_CHECK(p_shape->pir, SYS_MAX_SHAPE_RATE);

    SYS_QUEUE_DBG_FUNC();

    /* compute token rate */
    CTC_ERROR_RETURN(_sys_humber_channel_shape_token_rate_compute(lchip, p_shape->pir, &profile.rate));

    /* compute token threshold & shift */
    if (p_shape->pbs != SYS_SPECIAL_SHAPE_BURST)
    {
        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(p_shape->pbs, &profile.threshold, &profile.shift, FALSE));
    }
    else
    {
        burst = (profile.rate * 100) >> 1;

        if (burst < SYS_MIN_SHAPE_BURST)
        {
            burst = SYS_MIN_SHAPE_BURST;
        }

        if (burst > SYS_MAX_SHAPE_BURST)
        {
            burst = SYS_MAX_SHAPE_BURST;
        }

        CTC_ERROR_RETURN(
            _sys_humber_queue_shape_threshold_compute(burst, &profile.threshold, &profile.shift, TRUE));
    }

    p_profile = ctc_vector_get(p_sys_channel_shape_vec[lchip], channel);
    if (p_profile)
    {
        /* update old profile with the new one */
        CTC_ERROR_RETURN(
            _sys_humber_channel_shape_profile_write(lchip, channel, &profile));

        kal_memcpy(p_profile, &profile, sizeof(profile));
    }
    else
    {
        /* create a channel shape profile */
        CTC_ERROR_RETURN(_sys_humber_channel_shape_profile_create(lchip, channel, &profile, &p_profile));

        /* write the profile to asic */
        ret = _sys_humber_channel_shape_profile_write(lchip, channel, p_profile);
        if (ret)
        {
            _sys_humber_channel_shape_profile_remove(lchip, channel);
            return ret;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Unset shaping for the given channel in a chip.
*/
int32
sys_humber_queue_unset_channel_shape(uint8 lchip, uint8 channel)
{
    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_channel_shape_profile_remove(lchip, channel));

    return CTC_E_NONE;
}

/**
 @brief Get shaping for the given channel in a chip.
*/
int32
sys_humber_queue_get_channel_shape(uint8 lchip, uint8 channel, ctc_port_shape_t* p_shape)
{
    sys_channel_shape_profile_t *p_profile;
    uint32 cycle;
    uint64 tmp64;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_shape);

    SYS_QUEUE_DBG_FUNC();

    p_profile = ctc_vector_get(p_sys_channel_shape_vec[lchip], channel);
    if (!p_profile)
    {
        return CTC_E_CHANNEL_SHAPE_PROF_NOT_EXIST;
    }

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    /* compute rate according to the token rate */
    cycle = (sys_shape_ctl.channel_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
            (sys_shape_ctl.channel_shape_max_ptr - sys_shape_ctl.channel_shape_min_ptr + 1);

    tmp64 = (uint64)p_profile->rate * core_frequency / cycle;

    p_shape->pir = (uint32)tmp64;

    /* compute burst according to the token threshold and shift */
    p_shape->pbs = p_profile->threshold << p_profile->shift;

    SYS_QUEUE_DBG_INFO("get channel shape, lchip = %d, channel = %d\n", lchip, channel);
    SYS_QUEUE_DBG_INFO("p_shape->pir = %d\n", p_shape->pir);
    SYS_QUEUE_DBG_INFO("p_shape->pbs = %d\n", p_shape->pbs);

    return CTC_E_NONE;
}


/**
 @brief Set queue shaping for the given service ID.
*/
int32
sys_humber_service_set_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset, ctc_queue_shape_t* p_shape)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint8  lchip, lchip_num;

    CTC_PTR_VALID_CHECK(p_shape);
    CTC_MAX_VALUE_CHECK(p_shape->cir, SYS_MAX_SHAPE_RATE);
    CTC_MAX_VALUE_CHECK(p_shape->pir, SYS_MAX_SHAPE_RATE);

    if (type != CTC_QUEUE_TYPE_SERVICE_INGRESS && type != CTC_QUEUE_TYPE_SERVICE_EGRESS)
    {
        return CTC_E_INVALID_PARAM;
    }

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, offset, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape(lchip, queue_id, p_shape));
    }

    return CTC_E_NONE;
}


/**
 @brief Unset queue shaping for the given service ID.
*/
int32
sys_humber_service_unset_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset)
{
    sys_service_config_t *p_service;
    uint16 queue_id;
    uint32 cmd;
    uint32 tmp;
    uint8  lchip, lchip_num;

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, offset, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));

        /* if group shape is configured for the queue,
           re-enable queue shape with default queue shape profile */
        cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_QUE_SHP_EN);
        tmp = 1;

        if (CTC_QUEUE_TYPE_SERVICE_INGRESS == type && p_service->p_igs_group[lchip])
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));
        }
        else if (CTC_QUEUE_TYPE_SERVICE_EGRESS == type && p_service->p_egs_group[lchip])
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Get queue shaping for the given service ID.
*/
int32
sys_humber_service_get_queue_shape(ctc_queue_type_t type, uint16 logical_service_id, uint8 offset, ctc_queue_shape_t* p_shape)
{
    sys_service_config_t *p_service;
    uint16 queue_id;

    CTC_PTR_VALID_CHECK(p_shape);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(type, logical_service_id, offset, &queue_id));

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_shape(0, queue_id, p_shape));

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_create_group(uint8 lchip, ctc_group_shape_t* p_shape, sys_queue_group_t** pp_group)
{
    sys_queue_group_t *p_group;
    sys_group_shape_profile_t profile;
    sys_group_shape_profile_t *p_profile;
    sys_humber_opf_t opf;
    uint32 group;
    uint32 field;
    uint32 cmd;
    uint32 tmp;
    int32  ret;

    CTC_PTR_VALID_CHECK(pp_group);

    SYS_QUEUE_DBG_FUNC();

    *pp_group = NULL;

    /* create group */
    p_group = (sys_queue_group_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_queue_group_t));
    if (!p_group)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_group, 0, sizeof(sys_queue_group_t));

    /* get available group id */
    opf.pool_type = OPF_QUEUE_GROUP;
    opf.pool_index = lchip;
    ret = sys_humber_opf_alloc_offset(&opf, 1, &group);
    if (ret)
    {
        mem_free(p_group);
        return ret;
    }

    p_group->group = group;

    /* group profile mapping according to group shape paramters */
    CTC_ERROR_RETURN(_sys_humber_group_shape_profile_map(lchip, p_shape, &profile));

    p_profile = NULL;
    CTC_ERROR_RETURN(_sys_humber_group_shape_profile_lookup(lchip, &profile, &p_profile));

    if (p_profile)  /* the same group shape profile is existent */
    {
        p_profile->ref++;
    }
    else
    {
        /* create a new profile */
        CTC_ERROR_RETURN(_sys_humber_group_shape_profile_create(lchip, &profile, &p_profile));

        /* write profile to asic */
        ret = _sys_humber_group_shape_profile_write(lchip, p_profile);
        if (ret)
        {
            goto error;
        }
    }

    p_group->p_profile = p_profile;

    /* init group tokens */
    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE, DS_GROUP_SHAPE_GRP_TOKEN);
    tmp = p_profile->threshold << p_profile->shift;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, group, cmd, &tmp));

    /* write group shape profile id */
    switch(group % 4)
    {
        case 0:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID0;
            break;

        case 1:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID1;
            break;

        case 2:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID2;
            break;

        default:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID3;
            break;
    }

    tmp = p_profile->index;
    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE_ID, field);
    ret = drv_tbl_ioctl(lchip, group / 4, cmd, &tmp);
    if (ret)
    {
        goto error;
    }

    *pp_group = p_group;
    p_sys_group_info[lchip][group] = p_group;

    SYS_QUEUE_DBG_INFO("Create group, lchip = %d, group_id = %d\n", lchip, group);

    return CTC_E_NONE;

error:
    _sys_humber_group_shape_profile_remove(lchip, p_profile);
    sys_humber_opf_free_offset(&opf, 1, group);
    mem_free(p_group);

    return ret;
}


static int32
_sys_humber_queue_remove_group(uint8 lchip, sys_queue_group_t* p_group)
{
    sys_humber_opf_t opf;
    uint32 field;
    uint32 cmd;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_group);

    SYS_QUEUE_DBG_FUNC();

    /* reset group shape profile id */
    switch(p_group->group % 4)
    {
        case 0:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID0;
            break;

        case 1:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID1;
            break;

        case 2:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID2;
            break;

        default:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID3;
            break;
    }

    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE_ID, field);
    tmp = 0;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_group->group / 4, cmd, &tmp));

    /* remove group shape profile */
    CTC_ERROR_RETURN(_sys_humber_group_shape_profile_remove(lchip, p_group->p_profile));

    opf.pool_type = OPF_QUEUE_GROUP;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_group->group));

    SYS_QUEUE_DBG_INFO("remove group, lchip = %d, group_id = %d\n", lchip , p_group->group);

    p_sys_group_info[lchip][p_group->group] = NULL;
    mem_free(p_group);

    return CTC_E_NONE;
}



static int32
_sys_humber_queue_update_group(uint8 lchip, sys_queue_group_t* p_group, ctc_group_shape_t* p_shape)
{
    sys_group_shape_profile_t profile;
    sys_group_shape_profile_t *p_profile;
    uint32 field;
    uint32 cmd;
    uint32 tmp;
    int32  ret;

    CTC_PTR_VALID_CHECK(p_shape);
    CTC_PTR_VALID_CHECK(p_group);
    CTC_PTR_VALID_CHECK(p_group->p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* group profile mapping according to group shape paramters */
    CTC_ERROR_RETURN(_sys_humber_group_shape_profile_map(lchip, p_shape, &profile));

    p_profile = NULL;
    CTC_ERROR_RETURN(_sys_humber_group_shape_profile_lookup(lchip, &profile, &p_profile));

    if (p_profile)  /* the same group shape profile is existent */
    {
        if (p_profile == p_group->p_profile)
        {
            return CTC_E_NONE;
        }
        else
        {
            p_profile->ref++;
        }
    }
    else
    {
        /* create a new profile */
        CTC_ERROR_RETURN(_sys_humber_group_shape_profile_create(lchip, &profile, &p_profile));

        /* write profile to asic */
        ret = _sys_humber_group_shape_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_group_shape_profile_remove(lchip, p_profile);
            return ret;
        }
    }

    /* init group tokens */
    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE, DS_GROUP_SHAPE_GRP_TOKEN);
    tmp = p_profile->threshold << p_profile->shift;
    ret = drv_tbl_ioctl(lchip, p_group->group, cmd, &tmp);
    if (ret)
    {
        _sys_humber_group_shape_profile_remove(lchip, p_profile);
        return ret;
    }

    /* update group shape profile id for the group */
    switch(p_group->group % 4)
    {
        case 0:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID0;
            break;

        case 1:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID1;
            break;

        case 2:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID2;
            break;

        default:
            field = DS_GROUP_SHAPE_PROFILE_ID_GRP_SHP_PROF_ID3;
            break;
    }

    tmp = p_profile->index;
    cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE_ID, field);
    ret = drv_tbl_ioctl(lchip, p_group->group / 4, cmd, &tmp);
    if (ret)
    {
        _sys_humber_group_shape_profile_remove(lchip, p_profile);
        return ret;
    }

    /* remove old profile */
    _sys_humber_group_shape_profile_remove(lchip, p_group->p_profile);

    p_group->p_profile = p_profile;

    return CTC_E_NONE;
}


/**
 @brief Set group shaping.
*/
int32
sys_humber_group_set_shape(ctc_group_shape_cfg_t* group_shape)
{
    sys_service_config_t *p_service = NULL;
    sys_queue_group_t *p_service_group = NULL;
    sys_queue_group_t **pp_service_group = NULL;
    ctc_queue_type_t queue_type=0;
    uint8  queue_num_per_port=0;
    uint16 queue_id=0;
    uint8  offset=0;
    uint8  qnum_per_service=0;
    uint8  lchip=0, lchip_num=0;
    uint8  lport=0;

    CTC_PTR_VALID_CHECK(group_shape);
    CTC_MAX_VALUE_CHECK(group_shape->shape.pir, SYS_MAX_SHAPE_RATE);
    SYS_QUEUE_DBG_FUNC();

    if(group_shape->type == CTC_PORT_GROUP_SHAPE)
    {
        SYS_MAP_GPORT_TO_LPORT(group_shape->para.gport, lchip, lport);
        if (NULL == port_queue_group[lchip][lport])
        {
            CTC_ERROR_RETURN(_sys_humber_queue_create_group(lchip, &(group_shape->shape), &(port_queue_group[lchip][lport])));

            /*get queue type and queue number per port by port*/
            CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));
            CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num_per_port));

            for (offset = 0; offset < queue_num_per_port; offset++)
            {
                CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, offset, &queue_id));
                CTC_ERROR_RETURN(_sys_humber_queue_set_group(lchip, queue_id, port_queue_group[lchip][lport]->group));
            }
        }
        else
        {
            CTC_ERROR_RETURN(_sys_humber_queue_update_group(lchip, port_queue_group[lchip][lport], &(group_shape->shape)));
        }
    }
    else if(group_shape->type == CTC_SERVICE_GROUP_SHAPE)
    {
        CTC_ERROR_RETURN(sys_humber_service_lookup(group_shape->para.serviceId, &p_service));
        if (!p_service)
        {
            return CTC_E_SERVICE_NOT_EXIST;
        }

        qnum_per_service = sys_humber_get_per_service_queue_num();

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            pp_service_group = (group_shape->dir == CTC_INGRESS) ?
                                &(p_service->p_igs_group[lchip]) : &(p_service->p_egs_group[lchip]);

            if (NULL == *pp_service_group)
            {
                /* create new group */
                CTC_ERROR_RETURN(_sys_humber_queue_create_group(lchip, &(group_shape->shape), &p_service_group));

                queue_type = (group_shape->dir == CTC_INGRESS) ?
                                CTC_QUEUE_TYPE_SERVICE_INGRESS : CTC_QUEUE_TYPE_SERVICE_EGRESS;

                /* set queue group id */
                for (offset = 0; offset < qnum_per_service; offset++)
                {
                    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, group_shape->para.serviceId, offset, &queue_id));
                    CTC_ERROR_RETURN(_sys_humber_queue_set_group(lchip, queue_id, p_service_group->group));
                }

                *pp_service_group = p_service_group;
            }
            else
            {
                /* update group */
                CTC_ERROR_RETURN(_sys_humber_queue_update_group(lchip, *pp_service_group, &(group_shape->shape)));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Unset group shaping.
*/
int32
sys_humber_group_unset_shape(ctc_group_shape_cfg_t* group_shape)
{
    sys_service_config_t *p_service = NULL;
    sys_queue_group_t **pp_service_group = NULL;
    ctc_queue_type_t queue_type=0;
    uint8  queue_num_per_port=0;
    uint8  offset=0;
    uint8  qnum_per_service=0;
    uint8  lchip=0, lchip_num=0;
    uint16 queue_id=0;
    uint8  lport=0;

    CTC_PTR_VALID_CHECK(group_shape);
    SYS_QUEUE_DBG_FUNC();

    if(group_shape->type == CTC_PORT_GROUP_SHAPE)
    {
        SYS_MAP_GPORT_TO_LPORT(group_shape->para.gport, lchip, lport);
        if (NULL == port_queue_group[lchip][lport])
        {
            return CTC_E_NONE;
        }

        /*get queue type and queue number per port by port*/
        CTC_ERROR_RETURN(_sys_humber_get_port_queue_type_by_lport(lport, &queue_type));
        CTC_ERROR_RETURN(_sys_humber_get_per_port_queue_num_by_lport(lport, &queue_num_per_port));

        for (offset = 0; offset < queue_num_per_port; offset++)
        {
            CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, lport, offset, &queue_id));
            CTC_ERROR_RETURN(_sys_humber_queue_set_group(lchip, queue_id, 0x1ff));
        }

        CTC_ERROR_RETURN(_sys_humber_queue_remove_group(lchip, port_queue_group[lchip][lport]));

        port_queue_group[lchip][lport] = NULL;
    }
    else if(group_shape->type == CTC_SERVICE_GROUP_SHAPE)
    {
        CTC_ERROR_RETURN(sys_humber_service_lookup(group_shape->para.serviceId, &p_service));
        if (!p_service)
        {
            return CTC_E_SERVICE_NOT_EXIST;
        }

        lchip_num = sys_humber_get_local_chip_num();
        qnum_per_service = sys_humber_get_per_service_queue_num();

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            pp_service_group = (group_shape->dir == CTC_INGRESS) ?
                                &(p_service->p_igs_group[lchip]) : &(p_service->p_egs_group[lchip]);

            if (NULL == *pp_service_group)
            {
                return CTC_E_NONE;
            }

            queue_type = (group_shape->dir == CTC_INGRESS) ?
                                CTC_QUEUE_TYPE_SERVICE_INGRESS : CTC_QUEUE_TYPE_SERVICE_EGRESS;

            /* map queue to an invalid group */
            for (offset = 0; offset < qnum_per_service; offset++)
            {
                CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(queue_type, group_shape->para.serviceId, offset, &queue_id));
                CTC_ERROR_RETURN(_sys_humber_queue_set_group(lchip, queue_id, SYS_QUEUE_INVALID_GROUP));
            }

            /* remove group */
            CTC_ERROR_RETURN(_sys_humber_queue_remove_group(lchip, *pp_service_group));
            *pp_service_group = NULL;
        }
    }

    return CTC_E_NONE;
}

/**
 @brief Get total number of queue shape profile.
*/
int32
sys_humber_queue_get_queue_profile_num(uint32* p_shape_num)
{
    ctc_hash_get_count(p_sys_queue_shape_hash[0], p_shape_num);

    return CTC_E_NONE;
}

/**
 @brief Get group shaping.
*/
int32
sys_humber_group_get_shape(ctc_group_shape_cfg_t* group_shape)
{
    sys_service_config_t *p_service = NULL;
    sys_queue_group_t *p_group = NULL;
    sys_group_shape_profile_t *p_profile = NULL;
    uint64 dividend;
    uint64 divisor;
    uint64 tmp64;
    uint32 core_frequency;
    uint8  lchip=0;
    uint8  lport=0;

    SYS_QUEUE_DBG_FUNC();
    CTC_PTR_VALID_CHECK(group_shape);

    if(group_shape->type == CTC_PORT_GROUP_SHAPE)
    {
        SYS_MAP_GPORT_TO_LPORT(group_shape->para.gport, lchip, lport);
        if (NULL == port_queue_group[lchip][lport])
        {
            return CTC_E_ENTRY_NOT_EXIST;
        }
        p_group = port_queue_group[lchip][lport];
    }
    else if(group_shape->type == CTC_SERVICE_GROUP_SHAPE)
    {
        CTC_ERROR_RETURN(sys_humber_service_lookup(group_shape->para.serviceId, &p_service));
        if (!p_service)
        {
            return CTC_E_SERVICE_NOT_EXIST;
        }

        p_group = (group_shape->dir == CTC_INGRESS) ?
            p_service->p_igs_group[0] : p_service->p_egs_group[0];
        if (!p_group)
        {
            return CTC_E_SERVICE_GROUP_NOT_EXIST;
        }
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    p_profile = p_group->p_profile;
    CTC_PTR_VALID_CHECK(p_profile);

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    /* compute pir by token rate */
    divisor = (uint64)sys_shape_ctl.group_shape_low_bw_weight * core_frequency;
    dividend = (uint64)(sys_shape_ctl.group_shape_update_max_cnt + 1) * SHAPE_UPDATE_UNIT *
              (sys_shape_ctl.group_shape_high_bw_weight + sys_shape_ctl.group_shape_low_bw_weight) *
              (sys_shape_ctl.group_shape_low_bw_max_ptr - sys_shape_ctl.group_shape_min_ptr + 1);

    tmp64 = p_profile->rate * divisor / dividend;
    group_shape->shape.pir = (uint32)tmp64;

    /* compute pbs by token threshold and shift */
    group_shape->shape.pbs = p_profile->threshold << p_profile->shift;

    if(group_shape->type == CTC_PORT_GROUP_SHAPE)
    {
        SYS_QUEUE_DBG_INFO("gport = %d, p_shape->pir = %d, p_shape->pbs = %d\n",
                           group_shape->para.gport, group_shape->shape.pir, group_shape->shape.pbs);
    }
    else if(group_shape->type == CTC_SERVICE_GROUP_SHAPE)
    {
        SYS_QUEUE_DBG_INFO("service_id = %d, p_shape->pir = %d, p_shape->pbs = %d\n",
                           group_shape->para.serviceId, group_shape->shape.pir, group_shape->shape.pbs);
    }

    return CTC_E_NONE;
}

/**
 @brief Queue shaper initialization.
*/
int32
sys_humber_queue_shape_init(void)
{
    sys_humber_opf_t opf;
    uint8  lchip, lchip_num;

    kal_memset(port_queue_group, 0, sizeof(port_queue_group));
    kal_memset(p_sys_group_info, 0, sizeof(p_sys_group_info));

    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QUEUE_SHAPE_PROFILE, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_GROUP_SHAPE_PROFILE, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QUEUE_GROUP, CTC_MAX_LOCAL_CHIP_NUM));

    CTC_ERROR_RETURN(_sys_humber_queue_shape_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_queue_init_queue_shape());
    CTC_ERROR_RETURN(_sys_humber_queue_init_group_shape());
    CTC_ERROR_RETURN(_sys_humber_queue_init_channel_shape());
    CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape_global_enable(TRUE));
    CTC_ERROR_RETURN(sys_humber_queue_set_group_shape_global_enable(TRUE));
    CTC_ERROR_RETURN(sys_humber_queue_set_channel_shape_global_enable(TRUE));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* init queue shape hash table */
        p_sys_queue_shape_hash[lchip] =
            ctc_hash_create(1,SYS_QUEUE_SHAPE_HASH_BUCKET_SIZE,
            _sys_humber_queue_shape_hash_key,
            _sys_humber_queue_shape_hash_cmp);

        /* init group shape hash table */
        p_sys_group_shape_hash[lchip] =
            ctc_hash_create(1,SYS_GROUP_SHAPE_HASH_BUCKET_SIZE,
            _sys_humber_group_shape_hash_key,
            _sys_humber_group_shape_hash_cmp);


        /* init channel shape vector */
        p_sys_channel_shape_vec[lchip] = ctc_vector_init(8, SYS_MAX_CHANNEL_NUM / 8);

        /* init queue shape offset pool */
        opf.pool_type = OPF_QUEUE_SHAPE_PROFILE;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 1, SYS_MAX_QUEUE_SHAPE_PROFILE_NUM - 1));

        /* init group shape offset pool */
        opf.pool_type = OPF_GROUP_SHAPE_PROFILE;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 1, SYS_MAX_GROUP_SHAPE_PROFILE_NUM - 1));

        /* init queue group offset pool */
        opf.pool_type = OPF_QUEUE_GROUP;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, SYS_MAX_GROUP_NUM - 1));
    }

    return CTC_E_NONE;
}

