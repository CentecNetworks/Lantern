/**
 @file sys_humber_aclqos_policer.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_macro.h"
#include "ctc_aclqos.h"
#include "ctc_debug.h"
#include "ctc_hash.h"
#include "ctc_vector.h"

#include "sys_humber_register.h"
#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_ftm.h"
#include "sys_humber_port.h"
#include "sys_humber_stats.h"
#include "sys_humber_queue_api.h"
#include "sys_humber_qos_policer.h"
#include "sys_humber_register.h"

#include "drv_io.h"
#include "drv_enum.h"
#include "drv_humber.h"
#include "drv_humber_data_path.h"
/****************************************************************************
  *
  * Defines and Macros
  *
  ****************************************************************************/

#define INVALID_POLICER_PROFILE_INDEX  0xFF
#define INVALID_POLICER_INDEX          0xFFFF

#define MAX_PROFILE_REF_NUM            0xFFFF
#define MAX_POLICER_REF_NUM            0xFFFF
#define MAX_POLICER_PROFILE_NUM        256

#define MAX_PHB_OFFSET_NUM             4

#define MAX_INTERNAL_POLICER_NUM       4096
#define POLICER_BUCKET_SIZE            256
#define POLICER_PROFILE_BUCKET_SIZE    32

#define POLICER_BASE_WITH_EXT_QDR 110592

#define MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT     64

/* get port policer offset */
#define SYS_QOS_PORT_POLICER_INDEX(lport, dir, phb_offset) \
    (((lport) << sys_qos_policing_ctl.port_policer_shift[dir]) + (phb_offset) + sys_qos_policing_ctl.port_policer_base[dir])

/* max port policer num in both direction */
#define MAX_PORT_POLICER_NUM   (MAX_PORT_NUM_PER_CHIP * MAX_PHB_OFFSET_NUM * 2)
#define MAX_PORT_POLICER_NUM_UNDER_CONFLICT   (MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT * 2)

/****************************************************************************
  *
  * Global and Declaration
  *
  ****************************************************************************/

static sys_qos_policing_ctl_t sys_qos_policing_ctl;
static ctc_hash_t *p_sys_policer_hash;
static ctc_hash_t *p_sys_policer_profile_hash[CTC_MAX_LOCAL_CHIP_NUM];
static ctc_vector_t *p_sys_port_policer_vec[CTC_MAX_LOCAL_CHIP_NUM][MAX_PHB_OFFSET_NUM][CTC_BOTH_DIRECTION];

/****************************************************************************
  *
  * Function
  *
  ****************************************************************************/

/**
 @brief Policer hash key hook.
*/
static INLINE uint32
_sys_humber_policer_hash_key(void* data)
{
    sys_qos_policer_t* p_policer = (sys_qos_policer_t *)data;

    return p_policer->id;
}

/**
 @brief Policer hash comparison hook.
*/
static INLINE bool
_sys_humber_policer_hash_cmp(void* data1, void* data2)
{
    sys_qos_policer_t *p_policer1 = (sys_qos_policer_t *)data1;
    sys_qos_policer_t *p_policer2 = (sys_qos_policer_t *)data2;

    if (p_policer1->id == p_policer2->id)
    {
        return TRUE;
    }

    return FALSE;
}


/**
 @brief Policer profile hash key hook.
*/
static INLINE uint32
_sys_humber_policer_profile_hash_key(void* data)
{
    sys_qos_policer_profile_t *p_profile = (sys_qos_policer_profile_t *)data;

    return (p_profile->commit_rate + (p_profile->commit_threshold << p_profile->commit_shift) +
            p_profile->peak_rate   + (p_profile->peak_threshold   << p_profile->peak_shift));
}

/**
 @brief Policer profile hash comparison hook.
*/
static INLINE bool
_sys_humber_policer_profile_hash_cmp(void* data1, void* data2)
{
    sys_qos_policer_profile_t *p_profile1 = (sys_qos_policer_profile_t *)data1;
    sys_qos_policer_profile_t *p_profile2 = (sys_qos_policer_profile_t *)data2;

    if ((p_profile1->tick_shift       == p_profile2->tick_shift) &&
        (p_profile1->commit_rate      == p_profile2->commit_rate) &&
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
 @brief Init policing control registers and tables.
*/
static int32
_sys_humber_qos_init_policing_ctl(void)
{
    uint8 lchip, lchip_num;
    uint32 cmd;
    uint32 value;
    uint32 core_frequency;
    const uint32 tick_interval_base_on_625mhz = 20;
    const uint16 core_frequency_625mhz = 625;
    core_frequency = drv_humber_get_core_freq();

    sys_qos_policing_ctl.tick_gen_interval
        = (tick_interval_base_on_625mhz * core_frequency / core_frequency_625mhz) - 1;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* tick gen interval */
        value = sys_qos_policing_ctl.tick_gen_interval;
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_TS_TICK_GEN_INTERVAL);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /* min policer ptr */
        value = sys_qos_policing_ctl.min_policer_ptr;
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL1, POLICING_CTRL1_MIN_PTR0);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, & value));

        /* max policing ptr */
        value = sys_qos_policing_ctl.max_policer_ptr;
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL1, POLICING_CTRL1_MAX_PTR0);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /* port policer always use external sram */
        /* ingress port policer base */
        value = sys_qos_policing_ctl.port_policer_base[CTC_INGRESS];
        cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PORT_POLICER_BASE);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /* ingress port policer shift */
        value = sys_qos_policing_ctl.port_policer_shift[CTC_INGRESS];
        cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PORT_POLICER_SHIFT);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /* egress port policer base */
        value = sys_qos_policing_ctl.port_policer_base[CTC_EGRESS] >> 8;
        cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_CTL, EPE_CLASSIFICATION_CTL_PORT_POLICER_BASE);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /* egress port policer shift */
        value = sys_qos_policing_ctl.port_policer_shift[CTC_EGRESS];
        cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_CTL, EPE_CLASSIFICATION_CTL_PORT_POLICER_SHIFT);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /*ext qdr policer base*/
        value = POLICER_BASE_WITH_EXT_QDR >> 8;
        cmd = DRV_IOW(IOC_REG, POLICING_EXT_BASE_PTR, POLICING_EXT_BASE_PTR_EXT_BASE_PTR);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));
    }

    return CTC_E_NONE;
}


/**
 @brief Get threshold shift according to the burst.
*/
static int32
_sys_humber_qos_policer_get_threshold_shift(uint32 burst, uint16* p_threshold, uint8* p_shift)
{
    int32 i;
    int32 value = 0;
    static uint16 policer_exp[16][2] =
    {
        {0,               0},
        {(1 << 0),    (1 << 1) - 1},
        {(1 << 1),    (1 << 2) - 1},
        {(1 << 2),    (1 << 3) - 1},
        {(1 << 3),    (1 << 4) - 1},
        {(1 << 4),    (1 << 5) - 1},
        {(1 << 5),    (1 << 6) - 1},
        {(1 << 6),    (1 << 7) - 1},
        {(1 << 7),    (1 << 8) - 1},
        {(1 << 8),    (1 << 9) - 1},
        {(1 << 9),    (1 << 10) - 1},
        {(1 << 10),  (1 << 11) - 1},
        {(1 << 11),  (1 << 12) - 1},
        {(1 << 12),  (1 << 13) - 1},
        {(1 << 13),  (1 << 14) - 1},
        {(1 << 14),  (1 << 15) - 1}
    };

    value = burst/4096;

    for (i = 0; i < 16; i++)
    {
        if ((value >= policer_exp[i][0]) && (value <= policer_exp[i][1]))
        {
             /* round up value */
            if ((burst + ((1 << i) - 1)) / (1 << i) >= 4096)
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
 @brief Get profile according to the given policer data.
*/
static int32
_sys_humber_qos_policer_map_profile(sys_qos_policer_t *p_policer, sys_qos_policer_profile_t *p_profile)
{
    uint64 tmp;
    uint32 rate;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_policer);
    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    /* SrTCM or TrTCM */
    rate = p_policer->is_srtcm ? p_policer->cir : p_policer->pir;
    if (rate <= 250000)     /* CIR/PIR < 2Mbps */ /*policer granularity 16kbps*/
    {
        p_profile->tick_shift = 14;
    }
    else if (rate < 125000000)  /* 2Mbps <= CIR/PIR < 1Gbps */ /*policer granularity 64kbps*/
    {
        p_profile->tick_shift = 12;
    }
    else    /* CIR/PIR >= 1Gbps */ /*policer granularity 512kbps*/
    {
        p_profile->tick_shift = 8;
    }

    /* compute commit rate */
    tmp = ((uint64)p_policer->cir) * ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1)) * ((uint64)(1 << p_profile->tick_shift));
    tmp /= core_frequency;

    if (tmp >= 65536)
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }
    else
    {
        p_profile->commit_rate = (uint16)tmp;
    }

    /* compute peak rate */
    tmp = ((uint64)p_policer->pir) * ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1)) * ((uint64)(1 << p_profile->tick_shift));
    tmp /= core_frequency;

    if (tmp >= 65536)
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }
    else
    {
        p_profile->peak_rate = (uint16)tmp;
    }

    /* compute peak threshold and peak threshold shift */
    CTC_ERROR_RETURN(
        _sys_humber_qos_policer_get_threshold_shift(p_policer->pbs, &p_profile->peak_threshold, &p_profile->peak_shift));

    /* compute commit threshold and commit threshold shift */
    CTC_ERROR_RETURN(
        _sys_humber_qos_policer_get_threshold_shift(p_policer->cbs, &p_profile->commit_threshold, &p_profile->commit_shift));

    SYS_QOS_POLICER_DBG_INFO("p_profile->tick_shift       = %d\n", p_profile->tick_shift);
    SYS_QOS_POLICER_DBG_INFO("p_profile->commit_rate      = %d\n", p_profile->commit_rate);
    SYS_QOS_POLICER_DBG_INFO("p_profile->commit_threshold = %d\n", p_profile->commit_threshold);
    SYS_QOS_POLICER_DBG_INFO("p_profile->commit_shift     = %d\n", p_profile->commit_shift);
    SYS_QOS_POLICER_DBG_INFO("p_profile->peak_rate        = %d\n", p_profile->peak_rate);
    SYS_QOS_POLICER_DBG_INFO("p_profile->peak_threshold   = %d\n", p_profile->peak_threshold);
    SYS_QOS_POLICER_DBG_INFO("p_profile->peak_shift       = %d\n", p_profile->peak_shift);

    return (DRV_E_NONE);
}


/**
 @brief Lookup policer profile in hash table.
*/
static int32
_sys_humber_qos_policer_profile_lookup(uint8 lchip, sys_qos_policer_profile_t* p_profile,
                                       sys_qos_policer_profile_t** pp_profile)
{
    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    *pp_profile = ctc_hash_lookup(p_sys_policer_profile_hash[lchip], p_profile);

    return CTC_E_NONE;
}

/**
 @brief Create profile entry in db and add it into hash table.
*/
static uint32
_sys_humber_qos_policer_profile_new(uint8 lchip, sys_qos_policer_profile_t *p_profile,
                                    sys_qos_policer_profile_t** pp_profile)
{
    sys_qos_policer_profile_t *p_new_profile;

    CTC_PTR_VALID_CHECK(p_profile);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    /* create associated profile */
    p_new_profile = (sys_qos_policer_profile_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_qos_policer_profile_t));
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
    p_new_profile->tick_shift       = p_profile->tick_shift;

    p_new_profile->ref   = 1;
    p_new_profile->index = INVALID_POLICER_PROFILE_INDEX;

    ctc_hash_insert(p_sys_policer_profile_hash[lchip], p_new_profile);

    *pp_profile = p_new_profile;

    return CTC_E_NONE;
}


/**
 @brief Write profile to asic.
*/
static int32
_sys_humber_qos_policer_profile_write(uint8 lchip, sys_qos_policer_profile_t *p_profile)
{
    sys_humber_opf_t opf;
    intprofileram_t ds_profile;
    uint32 index;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    opf.pool_type = OPF_QOS_POLICER_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &index));

    p_profile->index = index;

    kal_memset(&ds_profile, 0, sizeof(intprofileram_t));
    ds_profile.data71_to68 = p_profile->tick_shift >> 2;

    ds_profile.data67_to36 = p_profile->peak_threshold << 20;
    ds_profile.data67_to36 |= p_profile->peak_shift << 16;
    ds_profile.data67_to36 |= p_profile->peak_rate;

    ds_profile.data35_to32 = p_profile->tick_shift & 0x3;
    ds_profile.data31_to0 = p_profile->commit_threshold << 20;
    ds_profile.data31_to0 |= p_profile->commit_shift  << 16;
    ds_profile.data31_to0 |= p_profile->commit_rate;

    cmd = DRV_IOW(IOC_TABLE, INTPROFILERAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_profile));

    cmd = DRV_IOW(IOC_TABLE, EXTPROFILERAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_profile));

    SYS_QOS_POLICER_DBG_INFO("write policer profile, lchip = %d, tbl_id1 = %d, tbl_id2 = %d, index = %d\n",
                             lchip, INTPROFILERAM, EXTPROFILERAM, index);
    SYS_QOS_POLICER_DBG_INFO("=====================================================\n");
    SYS_QOS_POLICER_DBG_INFO("ds_profile.data71_to68 = 0x%x\n", ds_profile.data71_to68);
    SYS_QOS_POLICER_DBG_INFO("ds_profile.data67_to36 = 0x%x\n", ds_profile.data67_to36);
    SYS_QOS_POLICER_DBG_INFO("ds_profile.data35_to32 = 0x%x\n", ds_profile.data35_to32);
    SYS_QOS_POLICER_DBG_INFO("ds_profile.data31_to0  = 0x%x\n", ds_profile.data31_to0);

    return CTC_E_NONE;
}


/**
 @brief Delete profile from asic and db.
*/
static int32
_sys_humber_qos_policer_profile_delete(uint8 lchip, sys_qos_policer_profile_t *p_profile)
{
    sys_humber_opf_t opf;
    intprofileram_t ds_profile;
    uint32 cmd;
    uint32 index;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_NOT_ZERO_CHECK(p_profile->ref);
    p_profile->ref--;
    if (p_profile->ref > 0)
    {
        SYS_QOS_POLICER_DBG_INFO("policer profile, index = %d, ref = %d\n", p_profile->index, p_profile->ref);
        return CTC_E_NONE;
    }

    index = p_profile->index;

    /* remove from asic */
    if (INVALID_POLICER_PROFILE_INDEX != p_profile->index)
    {
        kal_memset(&ds_profile, 0, sizeof(intprofileram_t));
        cmd = DRV_IOW(IOC_TABLE, INTPROFILERAM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_profile));

        cmd = DRV_IOW(IOC_TABLE, EXTPROFILERAM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_profile));

        SYS_QOS_POLICER_DBG_INFO("delete policer profile, lchip = %d, tbl_id1 = %d, tbl_id2 = %d, index = %d\n",
                                 lchip, INTPROFILERAM, EXTPROFILERAM, index);

        opf.pool_type = OPF_QOS_POLICER_PROFILE;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_profile->index));
    }

    /* remove from profile hash table */
    ctc_hash_remove(p_sys_policer_profile_hash[lchip], p_profile);

    mem_free(p_profile);

    return CTC_E_NONE;
}


/**
 @brief Write policer to asic.
*/
static int32
_sys_humber_qos_policer_write(uint8 lchip, sys_qos_policer_t* p_policer, sys_qos_policer_profile_t* p_profile)
{
    sys_humber_opf_t opf;
    ds_policer_t ds_policer;
    uint32 index;
    uint32 cmd;
    int32  ret;

    CTC_PTR_VALID_CHECK(p_policer);
    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QOS_POLICER_DBG_FUNC();

    index = p_policer->index[lchip];

    /* if policer is already written to asic, indicates it is updated for a new profile this time */
    if (INVALID_POLICER_INDEX == index)
    {
        if((p_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE))
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
        }
        else if((p_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE))
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
        }
        else
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER;
        }
        opf.pool_index = lchip;

        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &index));
    }

    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));
    ds_policer.use_layer3_length  = p_policer->use_l3_length ? 1 : 0;
    ds_policer.sr_tcm_mode        = p_policer->is_srtcm ? 1 : 0;
    ds_policer.color_blind_mode   = p_policer->is_color_blind ? 1 : 0;
    ds_policer.color_drop_code    = p_policer->drop_color + 1;
    ds_policer.peak_count         = p_profile->peak_threshold << p_profile->peak_shift;
    ds_policer.commit_count_upper = ((p_profile->commit_threshold << p_profile->commit_shift) >> 12) & 0x3FF;
    ds_policer.commit_count_lower = (p_profile->commit_threshold << p_profile->commit_shift) & 0xFFF;
    ds_policer.old_ts             = 0;
    ds_policer.profile            = p_profile->index;

    if(p_policer->is_stats_en)
    {
        ds_policer.stats_en = 1;
    }

    cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, index, cmd, &ds_policer);
    if (ret)
    {
        if (INVALID_POLICER_INDEX == p_policer->index[lchip])
        {
            sys_humber_opf_free_offset(&opf, 1, index);
        }

        return ret;
    }

    p_policer->index[lchip] = index;
    p_policer->p_profile[lchip] = p_profile;

    SYS_QOS_POLICER_DBG_INFO("write policer, lchip = %d, index = %d\n", lchip, index);
    SYS_QOS_POLICER_DBG_INFO("========================================\n");
    SYS_QOS_POLICER_DBG_INFO("ds_policer.use_layer3_length  = %d\n", ds_policer.use_layer3_length);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.sr_tcm_mode        = %d\n", ds_policer.sr_tcm_mode);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.color_blind_mode   = %d\n", ds_policer.color_blind_mode);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.color_drop_code    = %d\n", ds_policer.color_drop_code);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.peak_count         = %d\n", ds_policer.peak_count);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.commit_count_upper = %d\n", ds_policer.commit_count_upper);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.commit_count_lower = %d\n", ds_policer.commit_count_lower);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.stats_en           = %d\n", ds_policer.stats_en);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.old_ts             = %d\n", ds_policer.old_ts);
    SYS_QOS_POLICER_DBG_INFO("ds_policer.profile            = %d\n", ds_policer.profile);

    return CTC_E_NONE;
}


/**
 @brief Lookup policer in hash table.
*/
static int32
_sys_humber_qos_policer_lookup(uint32 plc_id, sys_qos_policer_t** pp_policer)
{
    sys_qos_policer_t policer;

    CTC_PTR_VALID_CHECK(pp_policer);

    SYS_QOS_POLICER_DBG_FUNC();

    policer.id = plc_id;
    *pp_policer = ctc_hash_lookup(p_sys_policer_hash, &policer);

    return CTC_E_NONE;
}


/**
 @brief Mapping ctc layer policer to sys layer policer.
*/
static int32
_sys_humber_qos_policer_map(ctc_qos_policer_t* p_ctc_policer, sys_qos_policer_t* p_sys_policer)
{
    CTC_PTR_VALID_CHECK(p_ctc_policer);
    CTC_PTR_VALID_CHECK(p_sys_policer);

    SYS_QOS_POLICER_DBG_FUNC();

    p_sys_policer->use_l3_length  = p_ctc_policer->use_l3_length ? 1 : 0;
    p_sys_policer->is_srtcm       = p_ctc_policer->is_srtcm ? 1 : 0;
    p_sys_policer->is_color_blind = p_ctc_policer->is_color_blind ? 1 : 0;
    p_sys_policer->drop_color     = p_ctc_policer->drop_color;
    p_sys_policer->cir            = p_ctc_policer->cir;
    p_sys_policer->cbs            = p_ctc_policer->cbs;
    p_sys_policer->pir            = p_ctc_policer->pir;
    p_sys_policer->pbs            = p_ctc_policer->pbs;
    p_sys_policer->is_stats_en    = p_ctc_policer->is_stats_en;

    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->use_l3_length  = %d\n", p_sys_policer->use_l3_length);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->is_srtcm       = %d\n", p_sys_policer->is_srtcm);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->is_color_blind = %d\n", p_sys_policer->is_color_blind);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->drop_color     = %d\n", p_sys_policer->drop_color);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->cir            = %d\n", p_sys_policer->cir);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->cbs            = %d\n", p_sys_policer->cbs);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->pir            = %d\n", p_sys_policer->pir);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->pbs            = %d\n", p_sys_policer->pbs);
    SYS_QOS_POLICER_DBG_INFO("p_sys_policer->is_stats_en    = %d\n", p_sys_policer->is_stats_en);

    return CTC_E_NONE;
}

int32
sys_humber_qos_flow_policer_first_enable(ctc_direction_t dir, bool enable)
{
    uint8 lchip, lchip_num;
    uint32 cmd, tmp;

    tmp = (FALSE == enable)?0:1;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if((CTC_INGRESS==dir)||(CTC_BOTH_DIRECTION==dir))
        {
            cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_FLOW_POLICER_FIRST);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        if((CTC_EGRESS==dir)||(CTC_BOTH_DIRECTION==dir))
        {
            cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_CTL, EPE_CLASSIFICATION_CTL_FLOW_POLICER_FIRST);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}

/**
 @brief Create qos flow policer.
*/
int32
sys_humber_qos_flow_policer_new(ctc_qos_policer_t* p_ctc_policer, uint32 plc_id)
{
    sys_qos_policer_t *p_sys_policer;
    uint8  lchip, lchip_num;

    /* sanity check */
    CTC_PTR_VALID_CHECK(p_ctc_policer);
    CTC_NOT_ZERO_CHECK(plc_id);

    CTC_MAX_VALUE_CHECK(p_ctc_policer->cir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->cbs, SYS_MAX_POLICER_TOKEN_SIZE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pbs, SYS_MAX_POLICER_TOKEN_SIZE);

    /*if srTcm,cir equal to pir*/
    if((p_ctc_policer->cir) > (p_ctc_policer->pir))
    {
        return CTC_E_QOS_POLICER_CIR_GREATER_THAN_PIR;
    }

    /*if trtcm, pbs should not less than cbs*/
    if(!(p_ctc_policer->is_srtcm))
    {
        if((p_ctc_policer->cbs) > (p_ctc_policer->pbs))
        {
            return CTC_E_QOS_POLICER_CBS_GREATER_THAN_PBS;
        }
    }

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_sys_policer));
    if (p_sys_policer)
    {
        return CTC_E_QOS_POLICER_CREATED;
    }

    /* new policer */
    p_sys_policer = (sys_qos_policer_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_qos_policer_t));
    if (!p_sys_policer)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_sys_policer, 0, sizeof(sys_qos_policer_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_sys_policer->index[lchip] = INVALID_POLICER_INDEX;
    }

    /* map policer from ctc to sys layer */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_map(p_ctc_policer, p_sys_policer));
    
    p_sys_policer->id = plc_id;
    
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        sys_humber_opf_t opf;
        uint32 index = INVALID_POLICER_INDEX;
        if((p_sys_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE))
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
        }
        else if((p_sys_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE))
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
        }
        else
        {
            opf.pool_type = OPF_QOS_FLOW_POLICER;
        }
        opf.pool_index = lchip;

        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &index));
        p_sys_policer->index[lchip] = index;
    }



    /* insert into hash table */
    if (!ctc_hash_insert(p_sys_policer_hash, p_sys_policer))
    {
        mem_free(p_sys_policer);
        return CTC_E_NO_MEMORY;
    }

    return CTC_E_NONE;
}


/**
 @brief Delete qos flow policer.
*/
int32
sys_humber_qos_flow_policer_delete(uint32 plc_id)
{
    sys_qos_policer_t *p_policer;
    uint8 lchip, lchip_num;

    CTC_NOT_ZERO_CHECK(plc_id);

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    /* check if the policer is in using */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_policer->index[lchip] != INVALID_POLICER_INDEX)
        {
            sys_humber_opf_t opf;
            if((p_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE))
            {
                opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
            }
            else if((p_policer->is_stats_en) && (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE))
            {
                opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
            }
            else
            {
                opf.pool_type = OPF_QOS_FLOW_POLICER;
            }
            opf.pool_index = lchip;

            CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_policer->index[lchip]));
        }
    }

    ctc_hash_remove(p_sys_policer_hash, p_policer);
    mem_free(p_policer);

    return CTC_E_NONE;
}


/**
 @brief Get policer index for the given chip.
*/
int32
sys_humber_qos_policer_index_get(uint8 lchip, uint32 plc_id, uint32* p_index)
{
    sys_qos_policer_t *p_policer;

    CTC_NOT_ZERO_CHECK(plc_id);
    CTC_PTR_VALID_CHECK(p_index);

    /* lookup policer */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    *p_index = p_policer->index[lchip];

    return CTC_E_NONE;
}


/**
 @brief Refresh old flow policer with a new flow policer.
*/
int32
sys_humber_qos_flow_policer_refresh (uint32 plc_id, ctc_qos_policer_t* p_ctc_policer)
{
    sys_qos_policer_t *p_policer;
    sys_qos_policer_t old_policer;
    sys_qos_policer_profile_t *p_profile, *p_old_profile[CTC_MAX_LOCAL_CHIP_NUM];
    sys_qos_policer_profile_t new_profile;
    uint8  lchip, lchip_num;
    int32  ret;

    CTC_NOT_ZERO_CHECK(plc_id);
    CTC_PTR_VALID_CHECK(p_ctc_policer);

    CTC_MAX_VALUE_CHECK(p_ctc_policer->cir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->cbs, SYS_MAX_POLICER_TOKEN_SIZE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pbs, SYS_MAX_POLICER_TOKEN_SIZE);

    /*if srTcm,cir equal to pir*/
    if((p_ctc_policer->cir) > (p_ctc_policer->pir))
    {
        return CTC_E_QOS_POLICER_CIR_GREATER_THAN_PIR;
    }

    /*if trtcm, pbs should not less than cbs*/
    if(!(p_ctc_policer->is_srtcm))
    {
        if((p_ctc_policer->cbs) > (p_ctc_policer->pbs))
        {
            return CTC_E_QOS_POLICER_CBS_GREATER_THAN_PBS;
        }
    }

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    kal_memset(p_old_profile, 0, CTC_MAX_LOCAL_CHIP_NUM * sizeof(sys_qos_policer_profile_t *));

    /* save old policer data */
    kal_memcpy(&old_policer, p_policer, sizeof(sys_qos_policer_t));

    /* copy new policer data */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_map(p_ctc_policer, p_policer));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_policer->index[lchip] != INVALID_POLICER_INDEX)
            break;
    }

    if (lchip == lchip_num)  /* the policer is not added to any chip */
    {
        return CTC_E_NONE;
    }

    for ( ; lchip < lchip_num; lchip++)
    {
        p_old_profile[lchip] = p_policer->p_profile[lchip];
        if (!p_old_profile[lchip])
        {
            continue;
        }

        kal_memset(&new_profile, 0, sizeof(sys_qos_policer_profile_t));
        ret = _sys_humber_qos_policer_map_profile(p_policer, &new_profile);
        if (ret)
        {
            goto err;
        }

        /* lookup if exist the same profile */
        _sys_humber_qos_policer_profile_lookup(lchip, &new_profile, &p_profile);

        if (p_profile)
        {
            if (p_old_profile[lchip] == p_profile)
            {
                p_old_profile[lchip] = NULL;
                goto plc_up;
            }

            if (MAX_PROFILE_REF_NUM == p_profile->ref)
            {
                ret = CTC_E_INVALID_PARAM;
                goto err;
            }

            p_profile->ref++;
        }
        else
        {
            /* create a new profile and add it into db */
            ret = _sys_humber_qos_policer_profile_new(lchip, &new_profile, &p_profile);
            if (ret)
            {
                goto err;
            }

            /* write the new profile to asic */
            ret = _sys_humber_qos_policer_profile_write(lchip, p_profile);
            if (ret)
            {
                _sys_humber_qos_policer_profile_delete(lchip, p_profile);
                goto err;
            }
        }

plc_up:
        /* write new policer to the chip */
        ret = _sys_humber_qos_policer_write(lchip, p_policer, p_profile);
        if (ret)
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_profile);
            goto err;
        }
    }

    /* delete old profile from asic and db */
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_old_profile[lchip])
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_old_profile[lchip]);
        }
    }

    return CTC_E_NONE;

err:
    p_policer->cir            = old_policer.cir;
    p_policer->cbs            = old_policer.cbs;
    p_policer->pir            = old_policer.pir;
    p_policer->pbs            = old_policer.pbs;
    p_policer->is_srtcm       = old_policer.is_srtcm;
    p_policer->is_color_blind = old_policer.is_color_blind;
    p_policer->use_l3_length  = old_policer.use_l3_length;
    p_policer->drop_color     = old_policer.drop_color;

    lchip_num = lchip + 1;
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_old_profile[lchip])
        {
            continue;
        }

        if (p_old_profile[lchip] == p_policer->p_profile[lchip])
        {
            continue;
        }

        _sys_humber_qos_policer_profile_delete(lchip, p_policer->p_profile[lchip]);

        _sys_humber_qos_policer_profile_write(lchip, p_old_profile[lchip]);
    }

    return ret;
}

/**
 @brief Bind flow policer to the given chip.
*/
int32
sys_humber_qos_flow_policer_bind(uint8 lchip, uint32 plc_id)
{
    sys_qos_policer_t* p_policer;
    sys_qos_policer_profile_t profile;
    sys_qos_policer_profile_t *p_profile;
    uint32 ret;

    CTC_NOT_ZERO_CHECK(plc_id);

    SYS_QOS_POLICER_DBG_FUNC();

    /* lookup policer */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    if (p_policer->p_profile[lchip])   /* policer has been bound to the given chip, just increament the reference counter */
    {
        CTC_NOT_EQUAL_CHECK(p_policer->ref[lchip], MAX_POLICER_REF_NUM);
        p_policer->ref[lchip]++;
        return CTC_E_NONE;
    }

    /* lookup if there exist the same profile on the given chip for the policer */
    kal_memset(&profile, 0, sizeof(sys_qos_policer_profile_t));
    CTC_ERROR_RETURN(_sys_humber_qos_policer_map_profile(p_policer, &profile));

    CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_lookup(lchip, &profile, &p_profile));
    if (!p_profile)
    {
        /* create new policer profile */
        CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_new(lchip, &profile, &p_profile));

        /* write policer profile to asic */
        ret = _sys_humber_qos_policer_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_profile);
            return ret;
        }
    }
    else
    {
        /* increament profile reference counter */
        CTC_NOT_EQUAL_CHECK(p_profile->ref, MAX_PROFILE_REF_NUM);
        p_profile->ref++;
    }

    /* write policer to asic */
    ret = _sys_humber_qos_policer_write(lchip, p_policer, p_profile);
    if (ret)
    {
        _sys_humber_qos_policer_profile_delete(lchip, p_profile);
        return ret;
    }

    p_policer->ref[lchip]++;

    return CTC_E_NONE;
}

/**
 @brief Unbind flow policer from attached chip.
*/
int32
sys_humber_qos_flow_policer_unbind(uint8 lchip, uint32 plc_id)
{
    sys_qos_policer_t *p_policer;
    ds_policer_t ds_policer;
    uint32 cmd;

    CTC_NOT_ZERO_CHECK(plc_id);

    SYS_QOS_POLICER_DBG_FUNC();

    /* lookup policer */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    if (!p_policer->p_profile[lchip])
    {
        return CTC_E_QOS_POLICER_NOT_BIND;
    }

    CTC_NOT_EQUAL_CHECK(p_policer->ref[lchip], 0);
    p_policer->ref[lchip]--;
    if (p_policer->ref[lchip] > 0)
    {
        return CTC_E_NONE;
    }

    /* remove associated profile */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_delete(lchip, p_policer->p_profile[lchip]));
    p_policer->p_profile[lchip] = NULL;

    /* delete policer from asic */
    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));
    cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_policer->index[lchip], cmd, &ds_policer));

    return CTC_E_NONE;
}

/**
 @brief Get real flow policer data added to chip.
*/
int32
sys_humber_qos_get_flow_policer_data(uint32 plc_id, ctc_qos_policer_t* p_ctc_policer)
{
    sys_qos_policer_t *p_sys_policer;
    sys_qos_policer_profile_t *p_profile = NULL;
    uint64 tmp;
    uint8 lchip, lchip_num;
    uint32 core_frequency;

    CTC_NOT_ZERO_CHECK(plc_id);
    CTC_PTR_VALID_CHECK(p_ctc_policer);

    SYS_QOS_POLICER_DBG_FUNC();

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_sys_policer));
    if (!p_sys_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_profile = p_sys_policer->p_profile[lchip];
        if (p_profile)
        {
            break;
        }
    }

    if (p_profile)
    {
        tmp = ((uint64)p_profile->commit_rate) * core_frequency;
        tmp = tmp / ((uint64)(1 << p_profile->tick_shift)) / ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1));
        p_ctc_policer->cir = (uint32)tmp;
        p_ctc_policer->cbs = p_profile->commit_threshold << p_profile->commit_shift;

        tmp = ((uint64)p_profile->peak_rate) * core_frequency;
        tmp = tmp / ((uint64)(1 << p_profile->tick_shift)) / ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1));
        p_ctc_policer->pir = (uint32)tmp;
        p_ctc_policer->pbs = p_profile->peak_threshold << p_profile->peak_shift;
    }
    else
    {
        p_ctc_policer->cir = p_sys_policer->cir;
        p_ctc_policer->cbs = p_sys_policer->cbs;
        p_ctc_policer->pir = p_sys_policer->pir;
        p_ctc_policer->pbs = p_sys_policer->pbs;
    }
    p_ctc_policer->is_srtcm       = p_sys_policer->is_srtcm;
    p_ctc_policer->is_color_blind = p_sys_policer->is_color_blind;
    p_ctc_policer->drop_color     = p_sys_policer->drop_color;
    p_ctc_policer->use_l3_length  = p_sys_policer->use_l3_length;

    return CTC_E_NONE;
}



/**
 @brief Bind port policer.
*/
int32
sys_humber_qos_port_policer_bind(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_ctc_policer)
{
    sys_qos_policer_t* p_sys_policer;
    sys_qos_policer_profile_t profile;
    sys_qos_policer_profile_t *p_profile;
    uint8 lport, lchip;
    uint32 index;
    uint32 ret;

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(p_ctc_policer->cir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->cbs, SYS_MAX_POLICER_TOKEN_SIZE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pbs, SYS_MAX_POLICER_TOKEN_SIZE);

    /*if srTcm,cir equal to pir*/
    if((p_ctc_policer->cir) > (p_ctc_policer->pir))
    {
        return CTC_E_QOS_POLICER_CIR_GREATER_THAN_PIR;
    }

    /*if trtcm, pbs should not less than cbs*/
    if(!(p_ctc_policer->is_srtcm))
    {
        if((p_ctc_policer->cbs) > (p_ctc_policer->pbs))
        {
            return CTC_E_QOS_POLICER_CBS_GREATER_THAN_PBS;
        }
    }

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if((sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        && (lport >= MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT))
    {
        return CTC_E_QOS_THIS_PORT_NOT_SUPPORT_POLICER;
    }
    index = SYS_QOS_PORT_POLICER_INDEX(lport, dir, phb_offset);

    p_sys_policer = ctc_vector_get(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    if (p_sys_policer)
    {
        CTC_ERROR_RETURN(sys_humber_qos_port_policer_unbind(gport, dir, phb_offset));
        ctc_vector_del(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    }

    p_sys_policer = (sys_qos_policer_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_qos_policer_t));
    if (!p_sys_policer)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_sys_policer, 0, sizeof(sys_qos_policer_t));
    p_sys_policer->index[lchip] = index;

    CTC_ERROR_RETURN(_sys_humber_qos_policer_map(p_ctc_policer, p_sys_policer));

    kal_memset(&profile, 0, sizeof(sys_qos_policer_profile_t));
    CTC_ERROR_RETURN(_sys_humber_qos_policer_map_profile(p_sys_policer, &profile));
    CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_lookup(lchip, &profile, &p_profile));
    if (!p_profile) /* not find the same profile, create a new one */
    {
        /* write profile to db */
        CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_new(lchip, &profile, &p_profile));

        /* write profile to asic */
        ret = _sys_humber_qos_policer_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_profile);
            return ret;
        }
    }
    else
    {
        /* increament profile reference counter */
        CTC_NOT_EQUAL_CHECK(p_profile->ref, MAX_PROFILE_REF_NUM);
        p_profile->ref++;
        SYS_QOS_POLICER_DBG_INFO("find the same profile, lchip = %d, index = %d\n", lchip, p_profile->index);
    }

    /* write policer to chip */
    ret = _sys_humber_qos_policer_write(lchip, p_sys_policer, p_profile);
    if (ret)
    {
        _sys_humber_qos_policer_profile_delete(lchip, p_profile);
        return ret;
    }

    ctc_vector_add(p_sys_port_policer_vec[lchip][phb_offset][dir], lport, p_sys_policer);

    return CTC_E_NONE;
}


/**
 @brief Unbind port policer.
*/
int32
sys_humber_qos_port_policer_unbind(uint32 gport, ctc_direction_t dir, uint8 phb_offset)
{
    sys_qos_policer_t *p_sys_policer;
    ds_policer_t ds_policer;
    uint8 lchip, lport;
    uint32 cmd;

    SYS_QOS_POLICER_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    p_sys_policer = ctc_vector_get(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    if (!p_sys_policer)
    {
        return CTC_E_NONE;
    }

    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));
    cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_sys_policer->index[lchip], cmd, &ds_policer));

    SYS_QOS_POLICER_DBG_INFO("unbind port policer, gport = %d, phb_offset = %d, dir = %d, index = %d\n",
                             gport, phb_offset, dir, p_sys_policer->index[lchip]);

    /* remove profile from asic and db */
    CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_delete(lchip, p_sys_policer->p_profile[lchip]));

    ctc_vector_del(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);

    mem_free(p_sys_policer);

    return CTC_E_NONE;
}


/**
 @brief Bind service policer.
*/
int32
sys_humber_qos_service_policer_bind(uint16 service_id, ctc_qos_policer_t* p_ctc_policer)
{
    sys_qos_policer_t* p_sys_policer;
    sys_qos_policer_profile_t profile;
    sys_qos_policer_profile_t *p_profile;
    uint16 physical_service_id = 0;
    uint8  lchip = 0, lchip_num = 0, tmp_chip = 0;
    uint32 index = 0;
    uint32 ret = 0;
    ds_policer_t ds_policer;
    uint32 cmd = 0;

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_get_physical_service_id(service_id, &physical_service_id));

    if(physical_service_id >= sys_qos_policing_ctl.service_policer_num)
    {
        return CTC_E_QOS_POLICER_SERVICE_POLICER_NOT_ENABLE;
    }

    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));

    CTC_MAX_VALUE_CHECK(p_ctc_policer->cir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pir, SYS_MAX_POLICER_RATE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->cbs, SYS_MAX_POLICER_TOKEN_SIZE);
    CTC_MAX_VALUE_CHECK(p_ctc_policer->pbs, SYS_MAX_POLICER_TOKEN_SIZE);

    /*if srTcm,cir equal to pir*/
    if((p_ctc_policer->cir) > (p_ctc_policer->pir))
    {
        return CTC_E_QOS_POLICER_CIR_GREATER_THAN_PIR;
    }

    /*if trtcm, pbs should not less than cbs*/
    if(!(p_ctc_policer->is_srtcm))
    {
        if((p_ctc_policer->cbs) > (p_ctc_policer->pbs))
        {
            return CTC_E_QOS_POLICER_CBS_GREATER_THAN_PBS;
        }
    }

    index = physical_service_id + sys_qos_policing_ctl.service_policer_base;

    p_sys_policer = ctc_vector_get(sys_qos_policing_ctl.sys_service_policer_vec, physical_service_id);
    if (p_sys_policer)
    {
        CTC_ERROR_RETURN(sys_humber_qos_service_policer_unbind(service_id));
    }

    p_sys_policer = (sys_qos_policer_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_qos_policer_t));
    if (!p_sys_policer)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_sys_policer, 0, sizeof(sys_qos_policer_t));

    CTC_ERROR_RETURN(_sys_humber_qos_policer_map(p_ctc_policer, p_sys_policer));
    kal_memset(&profile, 0, sizeof(sys_qos_policer_profile_t));
    CTC_ERROR_RETURN(_sys_humber_qos_policer_map_profile(p_sys_policer, &profile));

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip=0; lchip<lchip_num; lchip++)
    {
        p_sys_policer->index[lchip] = index;

        CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_lookup(lchip, &profile, &p_profile));
        if (!p_profile) /* not find the same profile, create a new one */
        {
            /* write profile to db */
            ret = _sys_humber_qos_policer_profile_new(lchip, &profile, &p_profile);
            if(ret)
            {
                goto ERR;
            }

            /* write profile to asic */
            ret = _sys_humber_qos_policer_profile_write(lchip, p_profile);
            if (ret)
            {
                _sys_humber_qos_policer_profile_delete(lchip, p_profile);
                goto ERR;
            }
        }
        else
        {
            /* increament profile reference counter */
            if(MAX_PROFILE_REF_NUM == p_profile->ref)
            {
                ret = CTC_E_INVALID_PARAM;
                goto ERR;
            }
            p_profile->ref++;
            SYS_QOS_POLICER_DBG_INFO("find the same profile, lchip = %d, index = %d\n", lchip, p_profile->index);
        }

        if(sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            p_sys_policer->is_stats_en = FALSE;  /*for service policer, always disable*/
        }
        /* write policer to chip */
        ret = _sys_humber_qos_policer_write(lchip, p_sys_policer, p_profile);
        if (ret)
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_profile);
            goto ERR;
        }
    }

    ctc_vector_add(sys_qos_policing_ctl.sys_service_policer_vec, physical_service_id, p_sys_policer);

    return CTC_E_NONE;

ERR:
    tmp_chip = lchip;
    if(tmp_chip != 0) /*if lchip eq 0, no need to do rollback*/
    {
        for(lchip=0; lchip<tmp_chip; lchip++)
        {
            _sys_humber_qos_policer_profile_delete(lchip, p_profile);
            cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_sys_policer->index[lchip], cmd, &ds_policer));
        }
    }
    mem_free(p_sys_policer);

    return ret;
}


/**
 @brief Unbind service policer.
*/
int32
sys_humber_qos_service_policer_unbind(uint16 service_id)
{
    sys_qos_policer_t *p_sys_policer;
    ds_policer_t ds_policer;
    uint16 physical_service_id = 0;
    uint8  lchip = 0, lchip_num = 0;
    uint32 cmd;

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_get_physical_service_id(service_id, &physical_service_id));

    p_sys_policer = ctc_vector_get(sys_qos_policing_ctl.sys_service_policer_vec, physical_service_id);
    if (!p_sys_policer)
    {
        return CTC_E_NONE;
    }

    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip=0; lchip<lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_sys_policer->index[lchip], cmd, &ds_policer));

        SYS_QOS_POLICER_DBG_INFO("unbind service policer, lchip = %d, serviceId = %d, index = %d\n",
                                 lchip, service_id, p_sys_policer->index[lchip]);

        /* remove profile from asic and db */
        CTC_ERROR_RETURN(_sys_humber_qos_policer_profile_delete(lchip, p_sys_policer->p_profile[lchip]));
    }

    ctc_vector_del(sys_qos_policing_ctl.sys_service_policer_vec, physical_service_id);

    mem_free(p_sys_policer);

    return CTC_E_NONE;
}

/**
 @brief Get real port policer data added to chip.
*/
int32
sys_humber_qos_get_port_policer_data(uint32 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_t* p_ctc_policer)
{
    sys_qos_policer_t *p_sys_policer;
    sys_qos_policer_profile_t *p_profile;
    uint64 tmp;
    uint8 lchip, lport;
    uint32 core_frequency;

    CTC_PTR_VALID_CHECK(p_ctc_policer);

    SYS_QOS_POLICER_DBG_FUNC();

    core_frequency = drv_humber_get_core_freq();
    core_frequency *= 1000000;

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    p_sys_policer = ctc_vector_get(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    if (!p_sys_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    p_profile = p_sys_policer->p_profile[lchip];
    CTC_PTR_VALID_CHECK(p_profile);

    tmp = (uint64)p_profile->commit_rate * core_frequency;
    tmp = tmp / ((uint64)(1 << p_profile->tick_shift)) / ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1));
    p_ctc_policer->cir = (uint32)tmp;
    p_ctc_policer->cbs = p_profile->commit_threshold << p_profile->commit_shift;

    tmp = (uint64)p_profile->peak_rate * core_frequency;
    tmp = tmp / ((uint64)(1 << p_profile->tick_shift)) / ((uint64)(sys_qos_policing_ctl.tick_gen_interval + 1));
    p_ctc_policer->pir = (uint32)tmp;
    p_ctc_policer->pbs = p_profile->peak_threshold << p_profile->peak_shift;
    p_ctc_policer->is_srtcm       = p_sys_policer->is_srtcm;
    p_ctc_policer->is_color_blind = p_sys_policer->is_color_blind;
    p_ctc_policer->drop_color     = p_sys_policer->drop_color;
    p_ctc_policer->use_l3_length  = p_sys_policer->use_l3_length;

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS policer.
*/
int32
sys_humber_qos_policer_update_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    SYS_QOS_POLICER_DBG_FUNC();

    if (sys_qos_policing_ctl.policing_enable == enable)
    {
        return CTC_E_NONE;
    }

    tmp = enable ? 1 : 0;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_UPDATE_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_TS_TICK_GEN_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_qos_policing_ctl.policing_enable = enable;

    SYS_QOS_POLICER_DBG_INFO("sys_qos_policing_ctl.policing_enable = %d\n", enable);

    return CTC_E_NONE;
}


/**
 @brief Get QoS policer global enable status.
*/
int32
sys_humber_qos_get_policer_update_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QOS_POLICER_DBG_FUNC();

    *p_enable = sys_qos_policing_ctl.policing_enable;

    SYS_QOS_POLICER_DBG_INFO("sys_qos_policing_ctl.policing_enable = %d\n", *p_enable);

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS policer statistics.
*/
int32
sys_humber_qos_policer_stats_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    SYS_QOS_POLICER_DBG_FUNC();

    if (sys_qos_policing_ctl.stats_enable == enable)
    {
        return CTC_E_NONE;
    }

    tmp = enable ? 1 : 0;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_CONFIRM);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_VIOLATE);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_NOT_CONFIRM);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_qos_policing_ctl.stats_enable = enable;

    SYS_QOS_POLICER_DBG_INFO("sys_qos_policing_ctl.stats_enable = %d\n", enable);

    return CTC_E_NONE;
}

/**
 @brief Get QoS policer statistics global enable status.
*/
int32
sys_humber_qos_get_policer_stats_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QOS_POLICER_DBG_FUNC();

    *p_enable = sys_qos_policing_ctl.stats_enable;

    SYS_QOS_POLICER_DBG_INFO("sys_qos_policing_ctl.stats_enable = %d\n", *p_enable);

    return CTC_E_NONE;
}


/**
 @brief Get flow policer statistics.
*/
int32
sys_humber_qos_get_flow_policer_stats_result(uint32 plc_id, ctc_qos_policer_stats_t* p_stats)
{
    sys_qos_policer_t *p_policer;
    sys_stats_policing_t stats_result;
    uint8  lchip, lchip_num;

    CTC_NOT_ZERO_CHECK(plc_id);
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    kal_memset(p_stats, 0, sizeof(ctc_qos_policer_stats_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (INVALID_POLICER_INDEX != p_policer->index[lchip])
        {
            CTC_ERROR_RETURN(sys_humber_stats_get_policing_stats(lchip, p_policer->index[lchip], &stats_result));
            p_stats->confirm_pkts  += stats_result.policing_confirm_pkts;
            p_stats->confirm_bytes += stats_result.policing_confirm_bytes;
            p_stats->exceed_pkts   += stats_result.policing_exceed_pkts;
            p_stats->exceed_bytes  += stats_result.policing_exceed_bytes;
            p_stats->violate_pkts  += stats_result.policing_violate_pkts;
            p_stats->violate_bytes += stats_result.policing_violate_bytes;
        }
    }

    SYS_QOS_POLICER_DBG_INFO("get flow policer stats for policer_id = %d\n", plc_id);
    SYS_QOS_POLICER_DBG_INFO("============================================\n");
    SYS_QOS_POLICER_DBG_INFO("confirm_packet = %llu, confirm_bytes = %llu\n", p_stats->confirm_pkts, p_stats->confirm_bytes);
    SYS_QOS_POLICER_DBG_INFO("exceed_packet  = %llu, exceed_bytes  = %llu\n", p_stats->exceed_pkts, p_stats->exceed_bytes);
    SYS_QOS_POLICER_DBG_INFO("violate_packet = %llu, violate_bytes = %llu\n", p_stats->violate_pkts, p_stats->violate_bytes);

    return CTC_E_NONE;
}


/**
 @brief Clear flow policer statistics.
*/
int32
sys_humber_qos_clear_flow_policer_stats_result(uint32 plc_id)
{
    sys_qos_policer_t *p_policer;
    uint8  lchip, lchip_num;

    CTC_NOT_ZERO_CHECK(plc_id);

    SYS_QOS_POLICER_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_qos_policer_lookup(plc_id, &p_policer));
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (INVALID_POLICER_INDEX != p_policer->index[lchip])
        {
            CTC_ERROR_RETURN(sys_humber_stats_reset_policing_stats(lchip, p_policer->index[lchip]));
        }
    }

    return CTC_E_NONE;
}



/**
 @brief Get port policer statistics.
*/
int32
sys_humber_qos_get_port_policer_stats_result(uint16 gport, ctc_direction_t dir, uint8 phb_offset, ctc_qos_policer_stats_t* p_stats)
{
    sys_qos_policer_t *p_policer;
    sys_stats_policing_t stats_result;
    uint8  lport, lchip;

    CTC_PTR_VALID_CHECK(p_stats);

    SYS_QOS_POLICER_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    p_policer = ctc_vector_get(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    kal_memset(p_stats, 0, sizeof(ctc_qos_policer_stats_t));
    CTC_ERROR_RETURN(sys_humber_stats_get_policing_stats(lchip, p_policer->index[lchip], &stats_result));
    p_stats->confirm_pkts  += stats_result.policing_confirm_pkts;
    p_stats->confirm_bytes += stats_result.policing_confirm_bytes;
    p_stats->exceed_pkts   += stats_result.policing_exceed_pkts;
    p_stats->exceed_bytes  += stats_result.policing_exceed_bytes;
    p_stats->violate_pkts  += stats_result.policing_violate_pkts;
    p_stats->violate_bytes += stats_result.policing_violate_bytes;

    SYS_QOS_POLICER_DBG_INFO("get port policer stats for gport = %d, dir = %d, phb_offset = %d\n", gport, dir, phb_offset);
    SYS_QOS_POLICER_DBG_INFO("============================================\n");
    SYS_QOS_POLICER_DBG_INFO("confirm_packet = %llu, confirm_bytes = %llu\n", p_stats->confirm_pkts, p_stats->confirm_bytes);
    SYS_QOS_POLICER_DBG_INFO("exceed_packet = %llu, exceed_bytes = %llu\n", p_stats->exceed_pkts, p_stats->exceed_bytes);
    SYS_QOS_POLICER_DBG_INFO("violate_packet = %llu, violate_bytes = %llu\n", p_stats->violate_pkts, p_stats->violate_bytes);

    return CTC_E_NONE;
}


/**
 @brief Clear port policer statistics.
*/
int32
sys_humber_qos_clear_port_policer_stats_result(uint16 gport, ctc_direction_t dir, uint8 phb_offset)
{
    sys_qos_policer_t *p_policer;
    uint8  lport, lchip;

    SYS_QOS_POLICER_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    p_policer = ctc_vector_get(p_sys_port_policer_vec[lchip][phb_offset][dir], lport);
    if (!p_policer)
    {
        return CTC_E_QOS_POLICER_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_stats_reset_policing_stats(lchip, p_policer->index[lchip]));

    return CTC_E_NONE;
}

int32
sys_humber_qos_policer_sequential_enable(bool enable)
{
    uint8 lchip, lchip_num;
    uint32 cmd, tmp;

    tmp = (FALSE == enable)?0:1;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_SEQUENTIAL_POLICING);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

int32
sys_humber_qos_policer_ipg_enable(bool enable)
{
    uint8  lchip, lchip_num;
    uint32 cmd, tmp;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        tmp = (TRUE == enable) ? 1 : 0;

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_IPG_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

int32
sys_humber_qos_policer_get_service_policer_num(uint32* service_policer_num)
{
    *service_policer_num = sys_qos_policing_ctl.service_policer_num;

    return CTC_E_NONE;
}

/**
 @brief QoS policer component initialization.
*/
int32
sys_humber_qos_policer_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg)
{
    uint8  lchip, lchip_num;
    uint32 policer_num, int_profile_num, ext_profile_num = 0;
    uint8  phb_offset, ext_qdr_en;
    ctc_direction_t dir;
    sys_humber_opf_t opf;
    ds_policer_t ds_policer;
    uint32 service_policer_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint32 i = 0;
    uint32 is_phb_support = 0;
    uint32 stats_mode = 0;

    SYS_QOS_POLICER_DBG_FUNC();

    kal_memset(&sys_qos_policing_ctl, 0, sizeof(sys_qos_policing_ctl_t));

    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_SERVICE_POLICER_NUM, &service_policer_num));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_SUPPORT_PHB, &is_phb_support));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));
    sys_qos_policing_ctl.service_policer_num = service_policer_num;
    sys_qos_policing_ctl.is_phb_support = is_phb_support;
    sys_qos_policing_ctl.stats_mode = stats_mode;

    p_sys_policer_hash =
        ctc_hash_create(1,POLICER_BUCKET_SIZE,
                        _sys_humber_policer_hash_key,
                        _sys_humber_policer_hash_cmp);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* init policer profile hash table */
        p_sys_policer_profile_hash[lchip] =
            ctc_hash_create(1,POLICER_PROFILE_BUCKET_SIZE,
                            _sys_humber_policer_profile_hash_key,
                            _sys_humber_policer_profile_hash_cmp);

        /* init port policer vector */
        for (phb_offset = 0; phb_offset < MAX_PHB_OFFSET_NUM; phb_offset++)
        {
            for (dir = 0; dir < CTC_BOTH_DIRECTION; dir++)
            {
                p_sys_port_policer_vec[lchip][phb_offset][dir] =
                    ctc_vector_init(8, MAX_PORT_NUM_PER_CHIP / 8);
            }
        }
    }

    sys_qos_policing_ctl.sys_service_policer_vec = ctc_vector_init(8, (sys_qos_policing_ctl.service_policer_num+7) / 8);



    if(sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
    {
        /*
         * policer index allocation:
         *     -- ingress port policer:  0 - 63, 64 port with per port 1 entries
         *     -- internal flow policer with stats: 64 - 255, 192 flow policer with stats
         *     -- egress port policer:   256 - 319, 64 port with per port 1 entries
         *     -- waste 512-320 = 192 policer.
         *     -- internal service policer: 512 - 1023, 512 servicer policer
         *     -- internal flow policer: 1024 - 4095,  3072 flow policer without stats
         */
        /* ingress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_INGRESS] = 0;

        /*flow policer with stats*/
        sys_qos_policing_ctl.flow_stats_policer_base =
            sys_qos_policing_ctl.port_policer_base[CTC_INGRESS] + MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT;
        sys_qos_policing_ctl.flow_stats_policer_size = 256 - MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT;

        /* egress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_EGRESS] =
            (sys_qos_policing_ctl.flow_stats_policer_base + sys_qos_policing_ctl.flow_stats_policer_size+255)/256*256;

        /* service policer */
        sys_qos_policing_ctl.service_policer_base =
            (sys_qos_policing_ctl.port_policer_base[CTC_EGRESS]+MAX_SUPPORT_PORT_NUM_PER_CHIP_UNDER_CONFLICT+255)/256*256;

        /* flow policer without stats*/
        sys_qos_policing_ctl.flow_policer_base =
            sys_qos_policing_ctl.service_policer_base + sys_qos_policing_ctl.service_policer_num;
    }
    else if (sys_qos_policing_ctl.stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
    {
        /*
         * policer index allocation:
         *     -- ingress port policer:  0
         *     -- internal flow policer with stats: 0 - 255
         *     -- egress port policer:   0
         *     -- internal service policer: 0
         *     -- internal flow policer: 256 - 4095,  3839 flow policer without stats
         */
        /* ingress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_INGRESS] = 0;

        /*flow policer with stats*/
        sys_qos_policing_ctl.flow_stats_policer_base = 0;
        sys_qos_policing_ctl.flow_stats_policer_size = 256;

        /* egress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_EGRESS] = 0;

        /* service policer */
        sys_qos_policing_ctl.service_policer_base = 0;

        /* flow policer without stats*/
        sys_qos_policing_ctl.flow_policer_base = 256;
    }
    else
    {
        /*
         * policer index allocation:
         *     -- ingress port policer:   0 - 1k, 256 port with per port 4 entries
         *     -- egress port policer:   1k - 2k, 256 port with per port 4 entries
         *     -- internal service policer: 2k - 2815
         *     -- internal flow policer: 2816k - 4k
         *     -- external flow policer: 4k - (if have)
         */
        /* ingress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_INGRESS] = 0;

        /* egress port policer base */
        sys_qos_policing_ctl.port_policer_base[CTC_EGRESS] =
            sys_qos_policing_ctl.port_policer_base[CTC_INGRESS] + MAX_PORT_POLICER_NUM/2;

        sys_qos_policing_ctl.service_policer_base = (MAX_PORT_POLICER_NUM + 255) / 256 * 256;
        sys_qos_policing_ctl.flow_policer_base = sys_qos_policing_ctl.service_policer_base
            + sys_qos_policing_ctl.service_policer_num;
    }

    /* flow policer opf init */
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QOS_FLOW_POLICER, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QOS_FLOW_POLICER_WITH_STATS, CTC_MAX_LOCAL_CHIP_NUM));

    /* policer profile opf init */
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QOS_POLICER_PROFILE, CTC_MAX_LOCAL_CHIP_NUM));

    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_POLICER, &policer_num));
    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(INTPROFILERAM, &int_profile_num));
    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    if(ext_qdr_en)
    {
        CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(EXTPROFILERAM, &ext_profile_num));
    }

    if(MAX_POLICER_PROFILE_NUM == int_profile_num)
    {
        int_profile_num -= 1;
    }

    if(MAX_POLICER_PROFILE_NUM == ext_profile_num)
    {
        ext_profile_num -= 1;
    }

    sys_qos_policing_ctl.int_policer_profile_num = (uint16)int_profile_num;
    sys_qos_policing_ctl.ext_policer_profile_num = (uint16)ext_profile_num;

    sys_qos_policing_ctl.min_policer_ptr = 0;
    sys_qos_policing_ctl.max_policer_ptr = (uint16)(policer_num - 1);

    if(sys_qos_policing_ctl.flow_policer_base >= policer_num)
    {
        return CTC_E_INVALID_PARAM;
    }

    /* init qos policing ctl */
    CTC_ERROR_RETURN(_sys_humber_qos_init_policing_ctl());

    tmp = sys_qos_policing_ctl.service_policer_base >> 8;
    kal_memset(&ds_policer, 0, sizeof(ds_policer_t));
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        opf.pool_index = lchip;

        /* init service policer*/
        if(sys_qos_policing_ctl.service_policer_num > 0)
        {
            cmd = DRV_IOW(IOC_REG, IPE_USER_ID_CTL, IPE_USER_ID_CTL_SERVICE_POLICING_BASE);
            DRV_IF_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /*init DS_POLICER table*/
            for(i=0; i<sys_qos_policing_ctl.service_policer_num; i++)
            {
                cmd = DRV_IOW(IOC_TABLE, DS_POLICER, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, sys_qos_policing_ctl.service_policer_base+i, cmd, &ds_policer));
            }
        }

        /* init flow policer free index pool */
        opf.pool_type = OPF_QOS_FLOW_POLICER_WITH_STATS;
        CTC_ERROR_RETURN(
            sys_humber_opf_init_offset(&opf, sys_qos_policing_ctl.flow_stats_policer_base, sys_qos_policing_ctl.flow_stats_policer_size));

        opf.pool_type = OPF_QOS_FLOW_POLICER;
        CTC_ERROR_RETURN(
            sys_humber_opf_init_offset(&opf, sys_qos_policing_ctl.flow_policer_base, policer_num - sys_qos_policing_ctl.flow_policer_base));

        /* init policer profile free index pool, only support 0-254(total 255) profile */
        opf.pool_type = OPF_QOS_POLICER_PROFILE;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, int_profile_num));
    }

    return CTC_E_NONE;
}


