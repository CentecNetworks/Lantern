/**
 @file sys_humber_queue_drop.c

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
#include "ctc_linklist.h"
#include "ctc_hash.h"
#include "ctc_queue.h"

#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_drop.h"
#include "sys_humber_internal_port.h"

#include "drv_humber.h"
#include "drv_io.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

#define SYS_MAX_DROP_PROFILE_NUM    256
#define SYS_MAX_DROP_PROFILE 15

extern sys_queue_enq_ctl_t sys_enq_ctl;

static ctc_list_pointer_t sys_drop_profile_list[CTC_MAX_LOCAL_CHIP_NUM];



/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 @brief create a new drop profile entry and add it into profile list.
*/
static int32
_sys_humber_queue_drop_profile_create(uint8 lchip, ctc_queue_drop_t* p_drop, sys_queue_drop_profile_t** pp_profile)
{
    sys_queue_drop_profile_t *p_profile;
    sys_humber_opf_t opf;
    uint32 offset;


    CTC_PTR_VALID_CHECK(p_drop);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    *pp_profile = NULL;

    /* get available profile index */
    opf.pool_type = OPF_QUEUE_DROP_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

    p_profile = (sys_queue_drop_profile_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_queue_drop_profile_t));
    if (!p_profile)
    {
        sys_humber_opf_free_offset(&opf, 1, offset);
        return CTC_E_NO_MEMORY;
    }

    p_profile->mode = p_drop->mode;
    kal_memcpy(p_profile->min_th, p_drop->min_th, sizeof(p_drop->min_th));
    kal_memcpy(p_profile->max_th, p_drop->max_th, sizeof(p_drop->max_th));
    kal_memcpy(p_profile->drop_prob, p_drop->drop_prob, sizeof(p_drop->drop_prob));
    p_profile->index = offset;
    p_profile->ref = 1;

    /* add it into profile list */
    ctc_list_pointer_insert_tail(&sys_drop_profile_list[lchip], &p_profile->head);

    *pp_profile = p_profile;

    SYS_QUEUE_DBG_INFO("Create a new drop profile, lchip = %d\n", lchip);
    SYS_QUEUE_DBG_INFO("-------------------------------------\n");
    if (CTC_QUEUE_DROP_WTD == p_drop->mode)
    {
        SYS_QUEUE_DBG_INFO("    drop mode: WTD\n");
    }
    else
    {
        SYS_QUEUE_DBG_INFO("    drop mode: WRED\n");
    }
    SYS_QUEUE_DBG_INFO("    min_th = %d  %d  %d  %d\n",
        p_drop->min_th[0], p_drop->min_th[1], p_drop->min_th[2], p_drop->min_th[3]);
    SYS_QUEUE_DBG_INFO("    max_th = %d  %d  %d  %d\n",
        p_drop->max_th[0], p_drop->max_th[1], p_drop->max_th[2], p_drop->max_th[3]);
    SYS_QUEUE_DBG_INFO("    drop_prob = %d  %d  %d  %d\n",
        p_drop->drop_prob[0], p_drop->drop_prob[1], p_drop->drop_prob[2], p_drop->drop_prob[3]);


    return CTC_E_NONE;
}


/**
 @brief create a new drop profile entry and add it into profile list.
*/
static int32
_sys_humber_queue_drop_profile_remove(uint8 lchip, sys_queue_drop_profile_t* p_profile)
{
    ds_queue_drop_profile_t ds_profile;
    sys_humber_opf_t opf;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    CTC_NOT_ZERO_CHECK(p_profile->ref);
    if (--p_profile->ref)
    {
        return CTC_E_NONE;
    }

    /* write to asic */
    kal_memset(&ds_profile, 0, sizeof(ds_profile));
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_profile->index, cmd, &ds_profile));

    opf.pool_type = OPF_QUEUE_DROP_PROFILE;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_profile->index));

    /* write to db */
    ctc_list_pointer_delete(&sys_drop_profile_list[lchip], &p_profile->head);

    mem_free(p_profile);

    return CTC_E_NONE;
}


/**
 @brief Lookup drop profile in existing profile list.
*/
static int32
_sys_humber_queue_drop_profile_lookup(uint8 lchip, ctc_queue_drop_t* p_drop, sys_queue_drop_profile_t** pp_profile)
{
    sys_queue_drop_profile_t *p_profile;
    ctc_list_pointer_node_t  *p_node;

    CTC_PTR_VALID_CHECK(p_drop);
    CTC_PTR_VALID_CHECK(pp_profile);

    SYS_QUEUE_DBG_FUNC();

    *pp_profile = NULL;

    CTC_LIST_POINTER_LOOP(p_node, &sys_drop_profile_list[lchip])
    {
        p_profile = _ctc_container_of(p_node, sys_queue_drop_profile_t, head);

        if (p_profile->mode != p_drop->mode)
            continue;

        /* WTD drop profile */
        if (CTC_QUEUE_DROP_WTD == p_drop->mode)
        {
            if (0 == kal_memcmp(p_profile->max_th, p_drop->max_th, sizeof(p_drop->max_th)))
            {
                *pp_profile = p_profile;
                SYS_QUEUE_DBG_INFO("Find a WTD drop profile: profile_index = %d\n", p_profile->index);
                SYS_QUEUE_DBG_INFO("-------------------------------------------\n");
                SYS_QUEUE_DBG_INFO("    max_th = %d  %d  %d  %d\n",
                                   p_profile->max_th[0], p_profile->max_th[1], p_profile->max_th[2], p_profile->max_th[3]);

                return CTC_E_NONE;
            }
        }
        else    /* WRED drop profile */
        {
            if (0 == kal_memcmp(p_profile->min_th, p_drop->min_th, sizeof(p_drop->min_th)) &&
                0 == kal_memcmp(p_profile->max_th, p_drop->max_th, sizeof(p_drop->max_th)) &&
                0 == kal_memcmp(p_profile->drop_prob, p_drop->drop_prob, sizeof(p_drop->drop_prob)))
            {
                *pp_profile = p_profile;
                SYS_QUEUE_DBG_INFO("Find a WRED drop profile: profile_index = %d\n", p_profile->index);
                SYS_QUEUE_DBG_INFO("-------------------------------------------\n");
                SYS_QUEUE_DBG_INFO("    min_th = %d  %d  %d  %d\n",
                                   p_profile->min_th[0], p_profile->min_th[1], p_profile->min_th[2], p_profile->min_th[3]);
                SYS_QUEUE_DBG_INFO("    max_th = %d  %d  %d  %d\n",
                                   p_profile->max_th[0], p_profile->max_th[1], p_profile->max_th[2], p_profile->max_th[3]);
                SYS_QUEUE_DBG_INFO("    drop_prob = %d  %d  %d  %d\n",
                                   p_profile->drop_prob[0], p_profile->drop_prob[1], p_profile->drop_prob[2], p_profile->drop_prob[3]);

                return CTC_E_NONE;
            }
        }
    }

    return CTC_E_NONE;
}

/**
 @brief Write drop profile to asic.
*/
static int32
_sys_humber_queue_drop_profile_write(uint8 lchip, sys_queue_drop_profile_t* p_profile)
{
    ds_queue_drop_profile_t ds_profile;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_profile);

    SYS_QUEUE_DBG_FUNC();

    /* write profile to asic */
    kal_memset(&ds_profile, 0, sizeof(ds_profile));
    ds_profile.wred_drop_mode = (p_profile->mode == CTC_QUEUE_DROP_WTD) ? 0 : 1;
    ds_profile.wred_min_thrd0 = p_profile->min_th[0];
    ds_profile.wred_min_thrd1 = p_profile->min_th[1];
    ds_profile.wred_min_thrd2 = p_profile->min_th[2];
    ds_profile.wred_min_thrd3 = p_profile->min_th[3];
    ds_profile.wred_max_thrd0 = p_profile->max_th[0];
    ds_profile.wred_max_thrd1 = p_profile->max_th[1];
    ds_profile.wred_max_thrd2 = p_profile->max_th[2];
    ds_profile.wred_max_thrd3 = p_profile->max_th[3];
    ds_profile.factor0        = p_profile->drop_prob[0];
    ds_profile.factor1        = p_profile->drop_prob[1];
    ds_profile.factor2        = p_profile->drop_prob[2];
    ds_profile.factor3        = p_profile->drop_prob[3];

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_profile->index, cmd, &ds_profile));

    return CTC_E_NONE;
}


/**
 @brief Initialize default queue drop.
*/
static int32
_sys_humber_default_queue_drop_init(void)
{
    ctc_queue_drop_t queue_drop;
    sys_queue_drop_profile_t *p_profile[CTC_MAX_LOCAL_CHIP_NUM][MAX_SYS_DEFAULT_DROP];
    uint8  drop_prec;
    uint16 queue_id;
    uint8  lchip, lchip_num;
    uint32 prof_id;
    uint32 field;
    uint32 cmd;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* create discard drop profile */
        kal_memset(&queue_drop, 0, sizeof(queue_drop));
        queue_drop.mode = CTC_QUEUE_DROP_WTD;
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_create(lchip, &queue_drop, &p_profile[lchip][SYS_DEFAULT_DROP_DISCARD]));
        p_profile[lchip][SYS_DEFAULT_DROP_DISCARD]->ref = 0x8FFF;    /* won't be freed */
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_write(lchip, p_profile[lchip][SYS_DEFAULT_DROP_DISCARD]));

        /* create GE drop profile */
        queue_drop.mode = CTC_QUEUE_DROP_WTD;
        for (drop_prec = 0; drop_prec < CTC_DROP_PREC_NUM; drop_prec++)
        {
            queue_drop.max_th[drop_prec] = SYS_DROP_COUNT_GE + drop_prec * SYS_DROP_DELTA_GE;
        }
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_create(lchip, &queue_drop, &p_profile[lchip][SYS_DEFAULT_DROP_GE]));
        p_profile[lchip][SYS_DEFAULT_DROP_GE]->ref = 0x8FFF;    /* won't be freed */
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_write(lchip, p_profile[lchip][SYS_DEFAULT_DROP_GE]));

        /* create 10GE drop profile */
        queue_drop.mode = CTC_QUEUE_DROP_WTD;
        for (drop_prec = 0; drop_prec < CTC_DROP_PREC_NUM; drop_prec++)
        {
            queue_drop.max_th[drop_prec] = SYS_DROP_COUNT_XGE + drop_prec * SYS_DROP_DELTA_XGE;
        }
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_create(lchip, &queue_drop, &p_profile[lchip][SYS_DEFAULT_DROP_XGE]));
        p_profile[lchip][SYS_DEFAULT_DROP_XGE]->ref = 0x8FFF;    /* won't be freed */
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_write(lchip, p_profile[lchip][SYS_DEFAULT_DROP_XGE]));

        /* init regular queue drop */
        for (queue_id = 0; queue_id < SYS_MAX_QUEUE_NUM; queue_id++)
        {
            /* write profile id to asic */
            switch(queue_id % 4)
            {
                case 0:
                    field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID0;
                    break;

                case 1:
                    field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID1;
                    break;

                case 2:
                    field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID2;
                    break;

                default:
                    field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID3;
                    break;
            }

             /* XGE channel */
            if (queue_id / sys_enq_ctl.queue_num_per_network_port >= SYS_XGMAC_CHANNEL_ID_BASE &&
                queue_id / sys_enq_ctl.queue_num_per_network_port < (SYS_XGMAC_CHANNEL_ID_BASE + SYS_MAX_XGMAC_CHANNEL_NUM))
            {
                prof_id = p_profile[lchip][SYS_DEFAULT_DROP_XGE]->index;
                sys_queue_config[lchip][queue_id].p_drop_profile = p_profile[lchip][SYS_DEFAULT_DROP_XGE];
            }
            else if( queue_id/sys_enq_ctl.queue_num_per_network_port == SYS_RESERVED_INTERNAL_PORT_FOR_DROP ) /*drop port*/
            {
                prof_id = p_profile[lchip][SYS_DEFAULT_DROP_DISCARD]->index;
                sys_queue_config[lchip][queue_id].p_drop_profile = p_profile[lchip][SYS_DEFAULT_DROP_DISCARD];
            }
            else    /* GE channel */
            {
                prof_id = p_profile[lchip][SYS_DEFAULT_DROP_GE]->index;
                sys_queue_config[lchip][queue_id].p_drop_profile = p_profile[lchip][SYS_DEFAULT_DROP_GE];
            }

            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &prof_id));
        }

        /*init queue size mode, default is buffer count*/
        cmd = DRV_IOW(IOC_REG, Q_MGR_ENQ_CTL, Q_MGR_ENQ_CTL_BASE_ON_BUF_CNT);
        field = 1;
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field));
    }

    return CTC_E_NONE;
}


/**
 @brief Get default queue drop.
*/
int32
sys_humber_queue_get_default_drop(sys_default_queue_drop_type_t type, ctc_queue_drop_t* p_drop)
{
    sys_queue_drop_profile_t *p_drop_profile = NULL;
    ctc_list_pointer_node_t *p_node = NULL;
    uint8  profile_index;
    ctc_list_pointer_t *p_list;

    CTC_MAX_VALUE_CHECK(type, MAX_SYS_DEFAULT_DROP - 1);
    CTC_PTR_VALID_CHECK(p_drop);

    SYS_QUEUE_DBG_FUNC();

    profile_index = type;
    /*COMPILE ERROR: sys_humber_queue_drop.c:353:error: the address of 'sys_drop_profile_list' will always evaluate as 'true'*/
    p_list = &sys_drop_profile_list[0];
    CTC_LIST_POINTER_LOOP(p_node, p_list)
    {
        p_drop_profile = _ctc_container_of(p_node, sys_queue_drop_profile_t, head);
        if (p_drop_profile->index == profile_index)
        {
            break;
        }
    }

    if (!p_node)
    {
        return CTC_E_QUEUE_DROP_PROF_NOT_EXIST;
    }

    p_drop->mode = p_drop_profile->mode;
    kal_memcpy(p_drop->min_th, p_drop_profile->min_th, sizeof(p_drop->min_th));
    kal_memcpy(p_drop->max_th, p_drop_profile->max_th, sizeof(p_drop->max_th));
    kal_memcpy(p_drop->drop_prob, p_drop_profile->drop_prob, sizeof(p_drop->drop_prob));

    SYS_QUEUE_DBG_INFO("default drop profile: index = %d\n", profile_index);
    SYS_QUEUE_DBG_INFO("---------------------------------------\n");
    SYS_QUEUE_DBG_INFO("    min_th = %d  %d  %d  %d\n",
        p_drop->min_th[0], p_drop->min_th[1], p_drop->min_th[2], p_drop->min_th[3]);
    SYS_QUEUE_DBG_INFO("    max_th = %d  %d  %d  %d\n",
        p_drop->max_th[0], p_drop->max_th[1], p_drop->max_th[2], p_drop->max_th[3]);
    SYS_QUEUE_DBG_INFO("    drop_prob = %d  %d  %d  %d\n",
        p_drop->drop_prob[0], p_drop->drop_prob[1], p_drop->drop_prob[2], p_drop->drop_prob[3]);

    return CTC_E_NONE;
}


/**
 @brief Set a drop scheme for the given queue in a chip. WTD and WRED drop schemes are supported.
        For WTD, only threshold for each drop precedence need to be configured. For WRED, parameters
        include min-threshold, max-threshold, drop-probability, and
        exponential-weighted-moving-average (EWMA) constant.
*/
int32
sys_humber_queue_set_drop(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop)
{
    sys_queue_drop_profile_t *p_profile;
    uint32 prof_id, field;
    uint32 cmd;
    int32 ret;

    CTC_PTR_VALID_CHECK(p_drop);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[0], SYS_MAX_DROP_PROFILE);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[1], SYS_MAX_DROP_PROFILE);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[2], SYS_MAX_DROP_PROFILE);

    SYS_QUEUE_DBG_FUNC();

    if (sys_queue_config[lchip][queue_id].p_drop_profile)
    {
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_remove(lchip, sys_queue_config[lchip][queue_id].p_drop_profile));
        sys_queue_config[lchip][queue_id].p_drop_profile = NULL;
    }

    CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_lookup(lchip, p_drop, &p_profile));
    if (!p_profile)
    {
        /* create and write a drop profile */
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_create(lchip, p_drop, &p_profile));

        ret = _sys_humber_queue_drop_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_queue_drop_profile_remove(lchip, p_profile);
            return ret;
        }
    }
    else
    {
        p_profile->ref++;   /* the same profile exists, increament the reference counter */
    }

    /* write profile id to asic */
    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID0;
            break;

        case 1:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID1;
            break;

        case 2:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID2;
            break;

        default:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID3;
            break;
    }

    prof_id = p_profile->index;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
    ret = drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &prof_id);
    if (ret)
    {
        _sys_humber_queue_drop_profile_remove(lchip, p_profile);
        return ret;
    }

    sys_queue_config[lchip][queue_id].p_drop_profile = p_profile;

    return CTC_E_NONE;
}

/**
 @set drop for oam loopback
*/
int32
sys_humber_queue_set_drop_oam_lpk(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop)
{
    sys_queue_drop_profile_t *p_profile;
    uint32 prof_id, field;
    uint32 cmd;
    int32 ret;

    CTC_PTR_VALID_CHECK(p_drop);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[0], SYS_MAX_DROP_PROFILE);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[1], SYS_MAX_DROP_PROFILE);
    CTC_MAX_VALUE_CHECK(p_drop->drop_prob[2], SYS_MAX_DROP_PROFILE);

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_lookup(lchip, p_drop, &p_profile));
    if (!p_profile)
    {
        /* create and write a drop profile */
        CTC_ERROR_RETURN(_sys_humber_queue_drop_profile_create(lchip, p_drop, &p_profile));

        ret = _sys_humber_queue_drop_profile_write(lchip, p_profile);
        if (ret)
        {
            _sys_humber_queue_drop_profile_remove(lchip, p_profile);
            return ret;
        }
    }
    else
    {
        p_profile->ref++;   /* the same profile exists, increament the reference counter */
    }

    /* write profile id to asic */
    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID0;
            break;

        case 1:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID1;
            break;

        case 2:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID2;
            break;

        default:
            field = DS_QUEUE_DROP_PROFILE_ID_PROF_ID3;
            break;
    }

    prof_id = p_profile->index;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
    ret = drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &prof_id);
    if (ret)
    {
        _sys_humber_queue_drop_profile_remove(lchip, p_profile);
        return ret;
    }

    return CTC_E_NONE;
}

/**
 @brief Get the drop paramters for the given queue in a chip.
*/
int32
sys_humber_queue_get_drop(uint8 lchip, uint16 queue_id, ctc_queue_drop_t* p_drop)
{
    CTC_PTR_VALID_CHECK(p_drop);

    SYS_QUEUE_DBG_FUNC();

    if (!sys_queue_config[lchip][queue_id].p_drop_profile)
    {
        return CTC_E_QUEUE_DROP_PROF_NOT_EXIST;
    }

    p_drop->mode = sys_queue_config[lchip][queue_id].p_drop_profile->mode;
    kal_memcpy(p_drop->min_th, sys_queue_config[lchip][queue_id].p_drop_profile->min_th, sizeof(p_drop->min_th));
    kal_memcpy(p_drop->max_th, sys_queue_config[lchip][queue_id].p_drop_profile->max_th, sizeof(p_drop->max_th));
    kal_memcpy(p_drop->drop_prob, sys_queue_config[lchip][queue_id].p_drop_profile->drop_prob, sizeof(p_drop->drop_prob));

    SYS_QUEUE_DBG_INFO("queue drop profile: lchip = %d, queue_id = %d\n", lchip, queue_id);
    SYS_QUEUE_DBG_INFO("---------------------------------------\n");
    SYS_QUEUE_DBG_INFO("    min_th = %d  %d  %d  %d\n",
        p_drop->min_th[0], p_drop->min_th[1], p_drop->min_th[2], p_drop->min_th[3]);
    SYS_QUEUE_DBG_INFO("    max_th = %d  %d  %d  %d\n",
        p_drop->max_th[0], p_drop->max_th[1], p_drop->max_th[2], p_drop->max_th[3]);
    SYS_QUEUE_DBG_INFO("    drop_prob = %d  %d  %d  %d\n",
        p_drop->drop_prob[0], p_drop->drop_prob[1], p_drop->drop_prob[2], p_drop->drop_prob[3]);


    return CTC_E_NONE;
}


/**
 @brief Set WRED Exponential-Weighted-Moving-Average (EWMA) factor.
*/
int32
sys_humber_queue_set_wred_weight(uint8 lchip, uint16 queue_id, uint8 weight)
{
    uint32 cmd;
    uint32 tmp;
    uint32 field;

    SYS_QUEUE_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(weight, 0xF);

    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_DROP_PROFILE_ID_QUE_DEPTH_WT0;
            break;

        case 1:
            field = DS_QUEUE_DROP_PROFILE_ID_QUE_DEPTH_WT1;
            break;

        case 2:
            field = DS_QUEUE_DROP_PROFILE_ID_QUE_DEPTH_WT2;
            break;

        default:
            field = DS_QUEUE_DROP_PROFILE_ID_QUE_DEPTH_WT3;
            break;
    }

    /* write to asic */
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
    tmp = weight;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id / 4, cmd, &tmp));

    /* write to db */
    sys_queue_config[lchip][queue_id].wred_weight = weight;

    SYS_QUEUE_DBG_INFO("lchip = %d, queue_id = %d, wred_weight = %d\n", lchip, queue_id, weight);

    return CTC_E_NONE;
}


/**
 @brief Get WRED Exponential-Weighted-Moving-Average (EWMA) factor.
*/
int32
sys_humber_queue_get_wred_weight(uint8 lchip, uint16 queue_id, uint8* p_weight)
{
    CTC_PTR_VALID_CHECK(p_weight);

    SYS_QUEUE_DBG_FUNC();

    *p_weight = sys_queue_config[lchip][queue_id].wred_weight;

    SYS_QUEUE_DBG_INFO("lchip = %d, queue_id = %d, wred_weight = %d\n", lchip, queue_id, *p_weight);

    return CTC_E_NONE;
}


/**
 @brief Force random queue dropping.
*/
int32
sys_humber_queue_force_random_drop(uint8 lchip, uint16 queue_id, bool random_drop)
{
    uint32 cmd;
    uint32 tmp;
    uint32 field;

    SYS_QUEUE_DBG_FUNC();

    switch(queue_id % 4)
    {
        case 0:
            field = DS_QUEUE_DROP_PROFILE_ID_FORCE_RANDOM_DROP0;
            break;

        case 1:
            field = DS_QUEUE_DROP_PROFILE_ID_FORCE_RANDOM_DROP1;
            break;

        case 2:
            field = DS_QUEUE_DROP_PROFILE_ID_FORCE_RANDOM_DROP2;
            break;

        default:
            field = DS_QUEUE_DROP_PROFILE_ID_FORCE_RANDOM_DROP3;
            break;
    }

    /* write to asic */
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
    tmp = random_drop ? 1 : 0;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id / 4, cmd, &tmp));

    /* write to db */
    sys_queue_config[lchip][queue_id].force_random_drop = random_drop ? 1 : 0;

    SYS_QUEUE_DBG_INFO("lchip = %d, queue_id = %d, force_random_drop = %d\n", lchip, queue_id, random_drop);

    return CTC_E_NONE;
}


/**
 @brief Set queue size mode: 1 -- buffer_cnt, 0 -- packet.
*/
int32
sys_humber_queue_set_queue_size_mode(uint8 lchip, uint8 szie_mode)
{
    uint32 cmd;
    uint32 field;

    SYS_QUEUE_DBG_FUNC();

    CTC_MAX_VALUE_CHECK(szie_mode, 1);

    /* write to asic */
    cmd = DRV_IOW(IOC_REG, Q_MGR_ENQ_CTL, Q_MGR_ENQ_CTL_BASE_ON_BUF_CNT);
    field = szie_mode;
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &field));

    SYS_QUEUE_DBG_INFO("lchip = %d, drop mode = %d\n", lchip, szie_mode);

    return CTC_E_NONE;
}


/**
 @brief Queue dropping initialization.
*/
int32
sys_humber_queue_drop_init(void)
{
    sys_humber_opf_t opf;
    uint8 lchip, lchip_num;

    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QUEUE_DROP_PROFILE, CTC_MAX_LOCAL_CHIP_NUM));

    opf.pool_type = OPF_QUEUE_DROP_PROFILE;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        ctc_list_pointer_init(&sys_drop_profile_list[lchip]);

        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, SYS_MAX_DROP_PROFILE_NUM));
    }

    CTC_ERROR_RETURN(_sys_humber_default_queue_drop_init());

    return CTC_E_NONE;
}



