/**
 @file sys_humber_queue_sch.c

 @date 2010-01-13

 @version v2.0

*/

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ctc_error.h"
#include "ctc_macro.h"
#include "ctc_queue.h"

#include "sys_humber_chip.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_sch.h"

#include "drv_io.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
uint32 queue_wdrr_weight_mtu = 2000;


/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 @brief Set class for the given queue in a chip.
*/
static int32
_sys_humber_queue_set_queue_class(uint8 lchip, uint16 queue_id, uint8 class)
{
    uint32 cmd;
    uint32 tmp;

    /* write to asic */
    tmp = class;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_PRIORITY_ID);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

    /* write to db */
    sys_queue_config[lchip][queue_id].class = class;

    SYS_QUEUE_DBG_INFO("lchip = %d, queue_id = %d, class = %d\n", lchip, queue_id, class);

    return CTC_E_NONE;
}

/**
 @brief Set class for the given queue in a chip.
*/
int32
sys_humber_queue_set_class(uint8 lchip, uint16 queue_id, uint8 class)
{
    uint32 cmd;
    uint32 field;
    uint32 tmp;
    uint8 is_empty = 0;
    int32 ret = CTC_E_NONE;

    CTC_MAX_VALUE_CHECK(queue_id, SYS_MAX_QUEUE_NUM - 1);
    CTC_MAX_VALUE_CHECK(class, CTC_MAX_QUEUE_CLASS_NUM - 1);

    SYS_QUEUE_DBG_FUNC();

    if (sys_queue_config[lchip][queue_id].p_channel_entry)
    {
        /* set queue to drop */
        switch (queue_id % 4)
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

        cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
        tmp = 0;
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &tmp));

        /* wait until queue is empty */
        CTC_ERROR_RETURN(sys_humber_queue_wait_queue_empty(lchip, queue_id, &is_empty));

        if(is_empty == TRUE)
        {
            CTC_ERROR_RETURN(_sys_humber_queue_set_queue_class(lchip, queue_id, class));
        }
        else    /*need disable channel*/
        {
            CTC_ERROR_RETURN(sys_humber_queue_disable_channel(lchip, sys_queue_config[lchip][queue_id].channel));
            /* wait until queue is empty */
            CTC_ERROR_RETURN(sys_humber_queue_wait_queue_empty(lchip, queue_id, &is_empty));
            if(is_empty == TRUE)
            {
                CTC_ERROR_RETURN(_sys_humber_queue_set_queue_class(lchip, queue_id, class));
            }
            else
            {
                ret = CTC_E_QUEUE_DEPTH_NOT_EMPTY;
            }
            CTC_ERROR_RETURN(sys_humber_queue_enable_channel(lchip, sys_queue_config[lchip][queue_id].channel));
        }

        /* restore queue from drop */
        if (sys_queue_config[lchip][queue_id].p_drop_profile)
        {
            switch (queue_id % 4)
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

            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DROP_PROFILE_ID, field);
            tmp = sys_queue_config[lchip][queue_id].p_drop_profile->index;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) queue_id / 4, cmd, &tmp));
        }
    }

    return ret;
}


/**
 @brief Get class for the given queue in a chip.
*/
int32
sys_humber_queue_get_class(uint8 lchip, uint16 queue_id, uint8* p_class)
{
    CTC_PTR_VALID_CHECK(p_class);

    *p_class = sys_queue_config[lchip][queue_id].class;

    return CTC_E_NONE;
}


/**
 @brief Set DRR weight for the given queue in a chip.
*/
int32
sys_humber_queue_set_queue_wdrr_weight(uint8 lchip, uint16 queue_id, uint16 weight)
{
    uint32 cmd;
    ds_queue_drr_weight_t que_drr_weight;
    uint32 tmp_weight;

    CTC_MAX_VALUE_CHECK(queue_id, SYS_MAX_QUEUE_NUM - 1);
    CTC_MIN_VALUE_CHECK(weight, 1);

    SYS_QUEUE_DBG_FUNC();

    kal_memset(&que_drr_weight, 0, sizeof(que_drr_weight));

    /* write to asic */
    tmp_weight = weight * queue_wdrr_weight_mtu;
    if (tmp_weight > SYS_QUEUE_MAX_DRR_WEIGHT)
    {
        /*if weith value is more than max weight, just use max weight, 2011-10-20*/
        tmp_weight = SYS_QUEUE_MAX_DRR_WEIGHT;
    }
    que_drr_weight.weight = tmp_weight;

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_DRR_WEIGHT, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &que_drr_weight));

    /* write to db */
    sys_queue_config[lchip][queue_id].wdrr_weight = weight;

    SYS_QUEUE_DBG_INFO("lchip = %d, queue_id = %d, weight = %d\n", lchip, queue_id, weight);

    return CTC_E_NONE;
}


/**
 @brief Get DRR weight for the given queue in a chip.
*/
int32
sys_humber_queue_get_queue_wdrr_weight(uint8 lchip, uint16 queue_id, uint16* p_weight)
{
    CTC_MAX_VALUE_CHECK(queue_id, SYS_MAX_QUEUE_NUM - 1);
    CTC_PTR_VALID_CHECK(p_weight);

    SYS_QUEUE_DBG_FUNC();

    *p_weight = sys_queue_config[lchip][queue_id].wdrr_weight;

    return CTC_E_NONE;
}


/**
 @brief Set RR weight for the given channel in a chip.
*/
int32
sys_humber_queue_set_channel_wrr_weight(uint8 lchip, uint8 channel, uint16 weight)
{
    return CTC_E_NONE;
}


/**
 @brief Queue scheduler initialization.
*/
int32
sys_humber_queue_sch_init(void)
{
    uint8  lchip, lchip_num;
    uint16 queue_id;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        for (queue_id = 0; queue_id < SYS_MAX_QUEUE_NUM; queue_id++)
        {
            CTC_ERROR_RETURN(sys_humber_queue_set_queue_wdrr_weight(lchip, queue_id, 1));
        }
    }

    return CTC_E_NONE;
}


