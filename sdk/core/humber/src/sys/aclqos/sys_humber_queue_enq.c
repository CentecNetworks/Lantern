/**
 @file sys_humber_queue_enq.c

 @date 2010-01-13

 @version v2.0

*/

/****************************************************************************
 *  Default Queue Mode
 *  current queue allocation scheme (Alt + F12 for pretty):
 *
 *           type          port id    channel id      queue number    queue id range
 *     ----------------  ----------   ----------      ------------    --------------
 *      network egress     0 - 51      0 - 51           8 * 52           0 - 415
 *      i-loop               X           52             8              416 - 423
 *      normal cpu           X           53             8              424 - 431
 *      oam                  X           54             8              432 - 439
 *      e-loop               X           55             8              440 - 447
 *      drop                 X           56             8              448 - 455
 *      static int port   56 - 63        X              8 * 8          456 - 519
 *      fabric               X       128 - 255          4 * 16         520 - 583
 *      exception cpu        X           53             115            584 - 698
 *      sgmac             48 - 51     48 - 51           2 * 4          700 - 707
 *      internal port     64 - 191       X              4 * 128        708 - 1219
 *      service ingress      X           X              4 * 100        1220 - 1619
 *      service egress       X           X              4 * 100        1620 - 2019
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/

#include "ctc_debug.h"
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_macro.h"
#include "ctc_vector.h"
#include "ctc_linklist.h"
#include "ctc_cpu_traffic.h"

#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_crc8.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_sch.h"
#include "sys_humber_queue_shape.h"
#include "sys_humber_cpu_traffic.h"
#include "sys_humber_internal_port.h"

#include "drv_io.h"
#include "drv_humber.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

sys_queue_config_t  sys_queue_config[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_QUEUE_NUM];
ctc_list_pointer_t  sys_channel_list[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_CHANNEL_NUM];
sys_queue_enq_ctl_t sys_enq_ctl;
uint32 sys_service_id_bitmap[(SYS_SERVICE_ID_NUM+31)/32];
ctc_list_pointer_t sys_service_list;
sys_queue_resrc_alloc_t sys_resrc_alloc[MAX_SYS_RESRC_ALLOC_TYPE] = {
    /* single chip, no service queue */
    {35, 5, 5, 0, 5, 0, 50},

    /* single chip, has service queue */
    {20, 5, 25, 0, 5, 0, 45},

    /* multi chip, no service queue */
    {20, 5, 5, 0, 5, 30, 35},

    /* multi chip, has service queue */
    {20, 5, 20, 0, 5, 20, 30}
};

extern sys_queue_group_t *p_sys_group_info[CTC_MAX_LOCAL_CHIP_NUM][SYS_MAX_GROUP_NUM];
extern ctc_vector_t *p_sys_channel_shape_vec[CTC_MAX_LOCAL_CHIP_NUM];


#define SYS_SERVICE_QUEUE_HASH_INDEX(service_id, dest_id, index) \
    { \
        uint8 hash_key[4]; \
        hash_key[0] = (dest_id) >> 8; \
        hash_key[1] = (dest_id) & 0xFF; \
        hash_key[2] = (service_id) >> 8; \
        hash_key[3] = (service_id) & 0xFF; \
        index = sys_humber_calculate_crc8(hash_key, 4, 0); \
        index = (index) & 0x7F; \
    }

#define SYS_GET_SERVICE_QUEUE_HASH_KEY_FIELD(depth, field_service_id, field_dest_id) \
    { \
        switch (depth) \
        { \
            case 0: \
                field_dest_id = DS_SERVICE_QUEUE_HASH_KEY_DEST_ID0; \
                field_service_id = DS_SERVICE_QUEUE_HASH_KEY_SERVICE_ID0; \
                break; \
            case 1: \
                field_dest_id = DS_SERVICE_QUEUE_HASH_KEY_DEST_ID1; \
                field_service_id = DS_SERVICE_QUEUE_HASH_KEY_SERVICE_ID1; \
                break; \
            case 2: \
                field_dest_id = DS_SERVICE_QUEUE_HASH_KEY_DEST_ID2; \
                field_service_id = DS_SERVICE_QUEUE_HASH_KEY_SERVICE_ID2; \
                break; \
            case 3: \
                field_dest_id = DS_SERVICE_QUEUE_HASH_KEY_DEST_ID3; \
                field_service_id = DS_SERVICE_QUEUE_HASH_KEY_SERVICE_ID3; \
                break; \
            default: \
                return CTC_E_INVALID_PARAM; \
        } \
    }

#define SYS_GET_QMGR_HASH_CAM_CTL_FIELD(depth, field_service_id, field_dest_id) \
    { \
        switch (depth) \
        { \
            case 0: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID0; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID0; \
                break; \
            case 1: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID1; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID1; \
                break; \
            case 2: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID2; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID2; \
                break; \
            case 3: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID3; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID3; \
                break; \
            case 4: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID4; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID4; \
                break; \
            case 5: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID5; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID5; \
                break; \
            case 6: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID6; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID6; \
                break; \
            case 7: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID7; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID7; \
                break; \
            case 8: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID8; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID8; \
                break; \
            case 9: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID9; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID9; \
                break; \
            case 10: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID10; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID10; \
                break; \
            case 11: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID11; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID11; \
                break; \
            case 12: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID12; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID12; \
                break; \
            case 13: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID13; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID13; \
                break; \
            case 14: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID14; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID14; \
                break; \
            case 15: \
                field_dest_id = Q_MGRQ_HASH_CAM_CTL_DEST_ID15; \
                field_service_id = Q_MGRQ_HASH_CAM_CTL_SERVICE_ID15; \
                break; \
            default: \
                return CTC_E_INVALID_PARAM; \
        } \
    }


/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

int32
sys_humber_queue_set_buf_store_resrc_mgr_enable(bool enable)
{
    uint8 lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    tmp = enable ? 0 : 1;
    cmd = DRV_IOW(IOC_REG, BUF_STORE_MISC_CTRL, BUF_STORE_MISC_CTRL_RESRC_MGR_DISABLE);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_enq_ctl.flow_ctl_enable= tmp;

    return CTC_E_NONE;
}

int32
sys_humber_queue_get_buf_store_resrc_mgr_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = sys_enq_ctl.flow_ctl_enable;

    SYS_QUEUE_DBG_INFO("sys_enq_ctl.flow_ctl_enable = %d\n", sys_enq_ctl.flow_ctl_enable);

    return CTC_E_NONE;
}


static int32
_sys_humber_queue_set_shared_resource_threshold(uint16 threshold)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    cmd = DRV_IOW(IOC_REG, BUF_STORE_SHARE_RESRC_INFO, BUF_STORE_SHARE_RESRC_INFO_SHARED_RESRC_THRESHOLD);
    tmp = (uint32)threshold;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_set_private_resource_threshold(uint8 resrc_group, uint8 prec, uint16 threshold)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;
    uint32 field;

    switch (prec)
    {
        case 0:
            field = BUFFER_STORE_RESRC_THRESHOLD_RESRC_DROP_THRESHOLD0;
            break;

        case 1:
            field = BUFFER_STORE_RESRC_THRESHOLD_RESRC_DROP_THRESHOLD1;
            break;

        case 2:
            field = BUFFER_STORE_RESRC_THRESHOLD_RESRC_DROP_THRESHOLD2;
            break;

        default:
            field = BUFFER_STORE_RESRC_THRESHOLD_RESRC_DROP_THRESHOLD3;
    }

    cmd = DRV_IOW(IOC_TABLE, BUFFER_STORE_RESRC_THRESHOLD, field);
    tmp = (uint32)threshold;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, resrc_group, cmd, &tmp));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_queue_ingress_resource_alloc(sys_queue_resrc_alloc_type_t type)
{
    uint32 i;
    uint32 tmp;
    uint32 sum;
    uint32 resouce_count;

    /* shared resource */
    tmp = sys_resrc_alloc[type].shared_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_shared_resource_threshold(tmp));

    /* CPU resource */
    resouce_count = sys_resrc_alloc[type].cpu_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / 100;
    tmp = resouce_count;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_CPU, 3, tmp));
    tmp = 90 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_CPU, 2, tmp));
    tmp = 80 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_CPU, 1, tmp));
    tmp = 70 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_CPU, 0, tmp));

    /* ingress loopback resource */
    resouce_count = sys_resrc_alloc[type].iloop_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / 100;
    tmp = resouce_count;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ILOOP, 3, tmp));
    tmp = 90 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ILOOP, 2, tmp));
    tmp = 80 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ILOOP, 1, tmp));
    tmp = 70 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ILOOP, 0, tmp));

    /* egress loopback resource */
    resouce_count = sys_resrc_alloc[type].eloop_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / 100;
    tmp = resouce_count;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ELOOP, 3, tmp));
    tmp = 90 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ELOOP, 2, tmp));
    tmp = 80 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ELOOP, 1, tmp));
    tmp = 70 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_ELOOP, 0, tmp));

    /* oam resource */
    resouce_count = sys_resrc_alloc[type].oam_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / 100;
    tmp = resouce_count;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_OAM, 3, tmp));
    tmp = 90 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_OAM, 2, tmp));
    tmp = 80 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_OAM, 1, tmp));
    tmp = 70 * resouce_count / 100;
    CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_OAM, 0, tmp));

    /* fabric resource */
    resouce_count = sys_resrc_alloc[type].fabric_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / (100 * CTC_MAX_QUEUE_CLASS_NUM);
    for (i = 0; i < CTC_MAX_QUEUE_CLASS_NUM; i++)
    {
        tmp = resouce_count;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_FABRIC_BASE + i, 3, tmp));
        tmp = 90 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_FABRIC_BASE + i, 2, tmp));
        tmp = 80 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_FABRIC_BASE + i, 1, tmp));
        tmp = 70 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(SYS_QUEUE_RESOURCE_GROUP_FABRIC_BASE + i, 0, tmp));
    }

    /* network resource */
    sum = SYS_MAX_GMAC_CHANNEL_NUM + SYS_MAX_XGMAC_CHANNEL_NUM * 10;
    for (i = 0; i < SYS_NETWORK_PORT_NUM; i++)
    {
        if (i < SYS_MAX_GMAC_CHANNEL_NUM)
        {
            resouce_count = sys_resrc_alloc[type].network_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / (100 * sum);
        }
        else
        {
            resouce_count = 10 * sys_resrc_alloc[type].network_private_resrc_percent * SYS_QUEUE_MAX_RESOURCE_CNT / (100 * sum);
        }

        tmp = resouce_count;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(i, 3, tmp));
        tmp = 90 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(i, 2, tmp));
        tmp = 80 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(i, 1, tmp));
        tmp = 70 * resouce_count / 100;
        CTC_ERROR_RETURN(_sys_humber_queue_set_private_resource_threshold(i, 0, tmp));
    }

    return CTC_E_NONE;
}


/**
 @brief Write DsQueueNumGenCtl to the given chip
*/
static int32
_sys_humber_queue_num_gen_ctl_write(uint8 lchip, uint8 index, sys_queue_num_gen_ctl_t* p_qnum_gen_ctl)
{
    ds_queue_num_gen_ctl_t ds_qnum_gen_ctl;
    uint32 cmd;

    CTC_PTR_VALID_CHECK(p_qnum_gen_ctl);

    SYS_QUEUE_DBG_FUNC();

    kal_memset(&ds_qnum_gen_ctl, 0, sizeof(ds_queue_num_gen_ctl_t));

    ds_qnum_gen_ctl.que_sel_shift      = p_qnum_gen_ctl->queue_select_shift & 0xF;
    ds_qnum_gen_ctl.dest_chip_id_shift = p_qnum_gen_ctl->dest_chip_shift & 0xF;
    ds_qnum_gen_ctl.service_que_en     = p_qnum_gen_ctl->service_queue_enable & 0x1;
    ds_qnum_gen_ctl.dest_que_shift     = p_qnum_gen_ctl->dest_queue_shift & 0xF;
    ds_qnum_gen_ctl.que_sel_mask       = p_qnum_gen_ctl->queue_select_mask & 0x3F;
    ds_qnum_gen_ctl.dest_chip_id_mask  = p_qnum_gen_ctl->dest_chip_mask & 0x1F;
    ds_qnum_gen_ctl.dest_que_mask      = p_qnum_gen_ctl->dest_queue_mask & 0xFFFF;
    ds_qnum_gen_ctl.sgmac_shift        = p_qnum_gen_ctl->sgmac_shift & 0xF;
    ds_qnum_gen_ctl.sgmac_mask         = p_qnum_gen_ctl->sgmac_mask & 0x3;
    ds_qnum_gen_ctl.dest_chip_id_base  = p_qnum_gen_ctl->dest_chip_base & 0x1F;
    ds_qnum_gen_ctl.que_num_base       = p_qnum_gen_ctl->queue_num_base & 0x1FFF;
    ds_qnum_gen_ctl.dest_que_base      = p_qnum_gen_ctl->dest_queue_base & 0xFFFF;
    ds_qnum_gen_ctl.flow_id_shift      = p_qnum_gen_ctl->flow_id_shift & 0xF;
    ds_qnum_gen_ctl.flow_id_mask       = p_qnum_gen_ctl->flow_id_mask & 0xFF;
    ds_qnum_gen_ctl.flow_id_base       = p_qnum_gen_ctl->flow_id_base & 0xFF;

    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_NUM_GEN_CTL, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_qnum_gen_ctl));

    return CTC_E_NONE;
}


/**
 @brief Init DsQueueNumGenCtl for regular queues, including network egress queues, cpu queues, oam queues,
        i-loop queues, and e-loop queues.
*/
static int32
_sys_humber_regular_queue_num_gen_ctl_init(void)
{
    uint8 lchip, lchip_num;
    uint8 channel;
    uint8 offset;
    uint16 queue_id;
    uint8 index;

    /* Init DsQueueNumGenCtl for network egress queue, network channel from 0 - 51 */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_num_base = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_base = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_num = sys_enq_ctl.queue_num_per_network_port;
    switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_num)
    {
        case 1:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 0;
            break;

        case 2:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0x20;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 5;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0xF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0x80;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 7;
            break;

        case 4:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0x30;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 4;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0xE;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0xC0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 6;
            break;

        case 8:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0x38;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 3;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0xD;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0xE0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 5;
            break;

        case 16:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0x3C;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 2;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0xC;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0xF0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 4;
            break;

        case 32:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_mask  = 0x3E;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_select_shift = 1;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].dest_queue_shift   = 0xB;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_mask       = 0xF8;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].flow_id_shift      = 3;
            break;

        default:
            return CTC_E_QUEUE_INVALID_CONFIG;
    }

    /* Init DsQueueNumGenCtl for I-Loop queues, I-Loop channel is 52 */
    kal_memcpy(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_ILOOP],
               &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS],
               sizeof(sys_queue_num_gen_ctl_t));


    /* Init DsQueueNumGenCtl for normal CPU queues, CPU channel is 53 */
    kal_memcpy(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NORMAL_CPU],
               &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS],
               sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for OAM queues, OAM channel is 54 */
    kal_memcpy(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_OAM],
               &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS],
               sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for E-Loop queues, E-Loop channel is 55 */
    kal_memcpy(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_ELOOP],
               &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS],
               sizeof(sys_queue_num_gen_ctl_t));


    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (sys_enq_ctl.local_switch_disable)   /* local switch disable */
        {
            /* packet from fabric destined to network, cpu, i-loop, e-loop, oam channels */
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_REGULAR);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS]));
        }
        else    /* local switch enable */
        {
            /* unicast/multicast packet to network, cpu, i-loop, e-loop, oam channels on local chip  */
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_REGULAR);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS]));
        }

        /* init regular queue channel map */
        for (channel = 0; channel <= SYS_DROP_CHANNEL_ID_START; channel++)
        {
            for (offset = 0; offset < sys_enq_ctl.queue_num_per_network_port; offset++)
            {
                queue_id = channel * sys_enq_ctl.queue_num_per_network_port + offset;
                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, channel));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Init DsQueueNumGenCtl for packet-to-CPU exceptions.
*/
int32
sys_humber_excp_cpu_queue_num_gen_ctl_init(void)
{
    uint8 lchip, lchip_num;
    uint8 offset;
    uint16 queue_id;
    uint8 index;
    uint16 excp_queue_num = sys_humber_get_excp_queue_num();

    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for exception cpu queues */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_num_base =
        (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num * SYS_MAX_SUPPORTED_FABRIC_PORT_NUM + 3) / 4 * 4;

    /* Each exception or sub-index is allocated with a queue, <<can not change!!!>> */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_num = 1;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_select_mask  = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_select_shift = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].dest_queue_mask    = 0x7FF;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].dest_queue_shift   = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].flow_id_base       = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].flow_id_mask       = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].flow_id_shift      = 0;

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (sys_enq_ctl.local_switch_disable)   /* local switch disable */
        {
            /* exception packet from network port destined to cpu */
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_EXCP_CPU);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU]));

            /* exception packet from fabric destined to cpu */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 1, SYS_QSEL_TYPE_EXCP_CPU);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU]));

        }
        else    /* local switch enable */
        {
            /* exception packet destined to cpu */
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_EXCP_CPU);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU]));
        }

        /* init packet-to-cpu excp queue channel map */
        for (offset = 0; offset < excp_queue_num; offset++)
        {
            queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_num_base + offset;

            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_CPU_CHANNEL_ID));
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Init DsQueueNumGenCtl for sgmac queue.
*/
static int32
_sys_humber_sgmac_queue_num_gen_ctl_init(void)
{
    uint8 lchip, lchip_num;
    uint16 queue_id;
    uint8 index;
    uint8 offset;

    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for sgmac port to cpu queue */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num_base =
        (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_EXCP_CPU].queue_num * sys_excp_queue_num + 3) / 4 * 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num = 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_select_mask  = 0x30;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_select_shift = 4;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].flow_id_base       = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].flow_id_mask       = 0xC0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].flow_id_shift      = 6;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].sgmac_base         = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].sgmac_mask         = 0xF;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].sgmac_shift        = 0;

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 0, SYS_QSEL_TYPE_SGMAC_PORT_TO_CPU);
        CTC_ERROR_RETURN(_sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU]));

        /* init sgmac cpu to cpu queue channel map */
        for(offset=0; offset<4; offset++)
        {
            queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num_base + offset;
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, 48+offset));
        }

    }
    SYS_QUEUE_DBG_INFO("sgmac port to cpu queue, base = %d, queue_num = %d\n",
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num_base,
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num);

    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for sgmac cpu to cpu queues */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_num_base =
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_num = 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_select_mask  = 0x30;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_select_shift = 4;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].flow_id_base       = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].flow_id_mask       = 0xC0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].flow_id_shift      = 6;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].sgmac_base         = 0;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].sgmac_mask         = 0xF;
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].sgmac_shift        = 0;

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 0, SYS_QSEL_TYPE_SGMAC_CPU_TO_CPU);
        CTC_ERROR_RETURN(_sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU]));

        /* init sgmac cpu to cpu queue channel map */
        for(offset=0; offset<4; offset++)
        {
            queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_num_base + offset;
            CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, 48+offset));
        }
    }

    SYS_QUEUE_DBG_INFO("sgmac port to cpu queue, base = %d, queue_num = %d\n",
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num_base,
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_PORT_TO_CPU].queue_num);

    return CTC_E_NONE;
}

/**
 @brief Init DsQueueNumGenCtl for fabric queues.
*/
static int32
_sys_humber_fabric_queue_num_gen_ctl_init(void)
{
    uint8 lchip, lchip_num;
    uint8 channel;
    uint8 port, offset;
    uint16 queue_id;
    uint8 index;

    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* Init DsQueueNumGenCtl for fabric queues */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num_base =
        (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num * SYS_STATIC_INT_PORT_NUM + 3) / 4 * 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num = sys_enq_ctl.queue_num_per_fabric;

    switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num)
    {
        case 1:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_mask  = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_shift = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_mask     = 0x1F;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_shift    = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_mask       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_shift      = 0;
            break;

        case 2:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_mask  = 0x20;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_shift = 5;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_mask     = 0x1F;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_shift    = 0xF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_mask       = 0x80;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_shift      = 7;
            break;

        case 4:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_mask  = 0x30;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_shift = 4;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_mask     = 0x1F;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_shift    = 0xE;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_mask       = 0xC0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_shift      = 6;
            break;

        case 8:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_mask  = 0x38;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_select_shift = 3;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_mask     = 0x1F;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].dest_chip_shift    = 0xD;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_mask       = 0xE0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].flow_id_shift      = 5;
            break;

        default:
            return CTC_E_QUEUE_INVALID_CONFIG;
    }

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (sys_enq_ctl.local_switch_disable)   /* local switch disable */
        {
            /* unicast packet to remote chip, pass to fabric queue */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 0, SYS_QSEL_TYPE_REGULAR);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));

            /* unicast packet to local chip, force to fabric queue */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 1, SYS_QSEL_TYPE_REGULAR);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));


            /* multicast packet to remote chip, pass to fabric queue */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 0, SYS_QSEL_TYPE_MCAST);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));

            /* multicast packet to local chip, force to fabric queue */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 1, SYS_QSEL_TYPE_MCAST);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));
        }
        else    /* local switch enable */
        {
            /* unicast packet to remote chip  */
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 0, SYS_QSEL_TYPE_REGULAR);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));

            /* multicast packet to remote chip */
            index = SYS_QNUM_GEN_CTL_INDEX(1, 0, 0, SYS_QSEL_TYPE_MCAST);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC]));
        }

        /* init fabric queue config */
        for (port = 0; port < SYS_MAX_SUPPORTED_FABRIC_PORT_NUM; port++)
        {
            for (offset = 0; offset < sys_enq_ctl.queue_num_per_fabric; offset++)
            {
                queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_FABRIC].queue_num_base +
                           port * sys_enq_ctl.queue_num_per_fabric + offset;

                channel = SYS_FABRIC_CHANNEL_ID_BASE + (queue_id % 4) * SYS_MAX_FABRIC_PORT_NUM + port;

                /* class mapping */
                CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, queue_id % 4));

                /* channel mapping */
                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, channel));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Init DsQueueNumGenCtl for static internal port queues.
*/
static int32
_sys_humber_static_int_port_queue_num_gen_ctl_init()
{
    uint8 lchip, lchip_num;
    uint8 index = 0;
    uint8 port = 0;
    uint8 offset = 0;
    uint8 static_internal_port_num = 0;
    uint16 queue_id = 0;

    /* Init DsQueueNumGenCtl for fabric queues, round up to be the multiple of 4 */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num_base =
        (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_NETWORK_EGRESS].queue_num * SYS_REGULAR_PORT_NUM + 3) / 4 * 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num = sys_enq_ctl.queue_num_per_static_int_port;


    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_base   = SYS_STATIC_INT_PORT_START;
    static_internal_port_num = SYS_STATIC_INT_PORT_END - SYS_STATIC_INT_PORT_START + 1;

    switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num)
    {
        case 1:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_mask  = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_shift = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_shift   = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_mask       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_shift      = 0;
            break;

        case 2:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_mask  = 0x20;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_shift = 5;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_shift   = 0xF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_mask       = 0x80;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_shift      = 7;
            break;

        case 4:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_mask  = 0x30;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_shift = 4;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_shift   = 0xE;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_mask       = 0xC0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_shift      = 6;
            break;

        case 8:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_mask  = 0x38;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_select_shift = 3;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].dest_queue_shift   = 0xD;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_mask       = 0xE0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].flow_id_shift      = 5;
            break;

        default:
            return CTC_E_QUEUE_INVALID_CONFIG;
    }

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_STATIC_INT_PORT);
        CTC_ERROR_RETURN(
            _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT]));

        /*init static int port queue map to drop channel*/
        for (port = 0; port <= static_internal_port_num; port++)
        {
            for (offset = 0; offset < sys_enq_ctl.queue_num_per_static_int_port; offset++)
            {
                queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_STATIC_INT_PORT].queue_num_base +
                    port * sys_enq_ctl.queue_num_per_static_int_port + offset;
                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_DROP_CHANNEL_ID_START));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Init DsQueueNumGenCtl for internal port queues.
*/
static int32
_sys_humber_internal_port_queue_num_gen_ctl_init()
{
    uint8 lchip, lchip_num;
    uint8 index = 0;
    uint8 port = 0;
    uint8 offset = 0;
    uint16 queue_id = 0;

    /* Init DsQueueNumGenCtl for fabric queues, round up to be the multiple of 4 */
    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num_base =
        (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_num_base +
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SGMAC_CPU_TO_CPU].queue_num+ 3) / 4 * 4;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num = sys_enq_ctl.queue_num_per_internal_port;

    sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_base   = SYS_HUMBER_INTERNAL_PORT_START;
    sys_enq_ctl.internal_port_num = sys_enq_ctl.max_internal_port_id - SYS_HUMBER_INTERNAL_PORT_START + 1;


    switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num)
    {
        case 1:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_mask  = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_shift = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_shift   = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_mask       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_shift      = 0;
            break;

        case 2:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_mask  = 0x20;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_shift = 5;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_shift   = 0xF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_mask       = 0x80;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_shift      = 7;
            break;

        case 4:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_mask  = 0x30;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_shift = 4;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_shift   = 0xE;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_mask       = 0xC0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_shift      = 6;
            break;

        case 8:
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_mask  = 0x38;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_select_shift = 3;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_mask    = 0x7FF;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].dest_queue_shift   = 0xD;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_base       = 0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_mask       = 0xE0;
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].flow_id_shift      = 5;
            break;

        default:
            return CTC_E_QUEUE_INVALID_CONFIG;
    }

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_INTERNAL_PORT);
        CTC_ERROR_RETURN(
            _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT]));

        /* init internal queue map to drop channel*/
        for (port = 0; port < sys_enq_ctl.internal_port_num; port++)
        {
            for (offset = 0; offset < sys_enq_ctl.queue_num_per_internal_port; offset++)
            {
                queue_id = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num_base +
                    port * sys_enq_ctl.queue_num_per_internal_port + offset;
                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, SYS_DROP_CHANNEL_ID_START));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Init DsServiceQueueHashKey and QmgrHashCamCtl.
*/
static int32
_sys_humber_service_queue_hash_key_init(void)
{
    uint8  lchip, lchip_num;
    uint8  bucket_index;
    ds_service_queue_hash_key_t ds_service_queue_hash_key;
    q_mgrq_hash_cam_ctl_t qmgr_hash_cam_ctl;
    uint32 cmd;

    kal_memset(&ds_service_queue_hash_key, 0xFF, sizeof(ds_service_queue_hash_key));
    kal_memset(&qmgr_hash_cam_ctl, 0xFF, sizeof(q_mgrq_hash_cam_ctl_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* init DsServiceQeuueHashKey */
        cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY, DRV_ENTRY_FLAG);
        for (bucket_index = 0; bucket_index < SYS_SERVICE_QUEUE_HASH_BUCKET_NUM; bucket_index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, bucket_index, cmd, &ds_service_queue_hash_key));
        }

        /* init QmgrHashCamCtl */
        cmd = DRV_IOW(IOC_REG, Q_MGRQ_HASH_CAM_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qmgr_hash_cam_ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief Wait until queue is empty.
*/
int32
sys_humber_queue_wait_queue_empty(uint8 lchip, uint16 queue_id, uint8 *is_empty)
{
    uint32 cmd;
    uint8 timeout = 0;
    uint32 queue_depth = 0;

    *is_empty = FALSE;

    do
    {
        cmd = DRV_IOR(IOC_TABLE, DS_QUEUE_DEPTH, DS_QUEUE_DEPTH_QUEUE_INST_DEPTH);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32)queue_id, cmd, &queue_depth));

        if(0 == queue_depth)
        {
            break;
        }

        kal_task_sleep(1);

        timeout ++;
    }while(timeout < 10);

    if(timeout < 10)
    {
        *is_empty = TRUE;
    }

    return CTC_E_NONE;
}

/**
 @brief Enable/disable local switching. When local swith is disabled, all incoming packets
        will be forced to fabric no matter it is intended to the local chip or remote chip.
*/
int32
sys_humber_set_local_switch_enable(bool enable)
{
    uint8 lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    tmp = enable ? 0 : 1;   /* 0 = local switch enable, 1 = local switch disable */
    cmd = DRV_IOW(IOC_REG, BUF_STORE_CTRL, BUF_STORE_CTRL_LOCAL_SWITCHING_DISABLE);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_enq_ctl.local_switch_disable = tmp;

    return CTC_E_NONE;
}


/**
 @brief Get local switch enable status.
*/
int32
sys_humber_get_local_switch_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QUEUE_DBG_FUNC();

    *p_enable = sys_enq_ctl.local_switch_disable;

    SYS_QUEUE_DBG_INFO("sys_enq_ctl.local_switch_disable = %d\n", sys_enq_ctl.local_switch_disable);

    return CTC_E_NONE;
}


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
        service egress   service id          queue offset in the service
*/
int32
sys_humber_queue_get_queue_id(ctc_queue_type_t type, uint16 id, uint8 offset, uint16* p_queue_id)
{
    sys_service_config_t *p_service;

    CTC_PTR_VALID_CHECK(p_queue_id);

    SYS_QUEUE_DBG_FUNC();

    switch(type)
    {
        case CTC_QUEUE_TYPE_NETWORK_EGRESS:     /* network egress queue */
            CTC_MAX_VALUE_CHECK(id, SYS_STATIC_INT_PORT_END);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_network_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          id * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_network_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("network egress, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_ILOOP:              /* i-loopback queue */
            CTC_EQUAL_CHECK(id, SYS_ILOOP_CHANNEL_ID);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_network_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          SYS_ILOOP_CHANNEL_ID * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_network_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("i-loop, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_NORMAL_CPU:         /* normal cpu queue */
            CTC_EQUAL_CHECK(id, SYS_CPU_CHANNEL_ID);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_network_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          SYS_CPU_CHANNEL_ID * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_network_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("normal cpu, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_OAM:                /* oam queue */
            CTC_EQUAL_CHECK(id, SYS_OAM_CHANNEL_ID);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_network_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          SYS_OAM_CHANNEL_ID * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_network_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("oam, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_ELOOP:              /* e-loopback queue */
            CTC_EQUAL_CHECK(id, SYS_ELOOP_CHANNEL_ID);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_network_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          SYS_ELOOP_CHANNEL_ID * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_network_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("e-loop, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_STATIC_INT_PORT:
            CTC_VALUE_RANGE_CHECK(id, SYS_STATIC_INT_PORT_START, SYS_STATIC_INT_PORT_END);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_static_int_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          (id - SYS_STATIC_INT_PORT_START) * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_static_int_port - 1) - offset);
            SYS_QUEUE_DBG_INFO("static int port, id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_EXCP_CPU:           /* packet-to-CPU exception queue */
            CTC_MAX_VALUE_CHECK(id, MAX_CTC_EXCEPTION - 1);
            CTC_MAX_VALUE_CHECK(offset, sys_excp_max_sub_index[id]);
            CTC_ERROR_RETURN(sys_humber_get_excp_queue_offset(id, p_queue_id));
            *p_queue_id += sys_enq_ctl.qnum_gen_ctl[type].queue_num_base + offset;
            SYS_QUEUE_DBG_INFO("excp cpu, exception = %d, sub_index = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_FABRIC:             /* fabric queue */
            CTC_MAX_VALUE_CHECK(id, SYS_MAX_SUPPORTED_FABRIC_PORT_NUM - 1);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_fabric- 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          id * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_fabric - 1) - offset);
            SYS_QUEUE_DBG_INFO("fabric, target_chip = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_INTERNAL_PORT:      /* internal port queue */
            CTC_MIN_VALUE_CHECK(id, SYS_HUMBER_INTERNAL_PORT_START);
            CTC_MAX_VALUE_CHECK(id, sys_enq_ctl.max_internal_port_id);
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_internal_port - 1);
            *p_queue_id = sys_enq_ctl.qnum_gen_ctl[type].queue_num_base +
                          (id - SYS_HUMBER_INTERNAL_PORT_START) * sys_enq_ctl.qnum_gen_ctl[type].queue_num + ((sys_enq_ctl.queue_num_per_internal_port - 1) - offset);

            SYS_QUEUE_DBG_INFO("internal port, port_id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_SERVICE_INGRESS:    /* service ingress queue */
            if(!sys_enq_ctl.queue_num_per_ingress_service)
            {
                return CTC_E_SERVICE_QUEUE_NOT_INITIALIZED;
            }
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_ingress_service - 1);
            CTC_ERROR_RETURN(sys_humber_service_lookup(id, &p_service));
            if (!p_service)
            {
                return CTC_E_SERVICE_NOT_EXIST;
            }
            *p_queue_id = p_service->ingress_queue_base + ((sys_enq_ctl.queue_num_per_ingress_service - 1) - offset);
            SYS_QUEUE_DBG_INFO("service ingress, service_id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        case CTC_QUEUE_TYPE_SERVICE_EGRESS:     /* service egress queue */
            if(!sys_enq_ctl.queue_num_per_egress_service)
            {
                return CTC_E_SERVICE_QUEUE_NOT_INITIALIZED;
            }
            CTC_MAX_VALUE_CHECK(offset, sys_enq_ctl.queue_num_per_egress_service - 1);
            CTC_ERROR_RETURN(sys_humber_service_lookup(id, &p_service));
            if (!p_service)
            {
                return CTC_E_SERVICE_NOT_EXIST;
            }
            *p_queue_id = p_service->egress_queue_base + ((sys_enq_ctl.queue_num_per_egress_service - 1) - offset);
            SYS_QUEUE_DBG_INFO("service egress, service_id = %d, offset = %d, queue_id = %d\n", id, offset, *p_queue_id);
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

/**
 @brief Disable channel's all queue threshold, all queue,group,channel shapping.
*/
int32
sys_humber_queue_disable_channel(uint8 lchip, uint8 channel)
{
    sys_queue_channel_entry_t *p_entry = NULL;
    ctc_list_pointer_node_t *p_node = NULL;
    sys_queue_shape_profile_t *queue_shape_profile = NULL;
    ds_queue_shape_t queue_shape;
    ds_channel_shape_profile_t channel_profile;
    uint8  ref[SYS_MAX_GROUP_NUM];
    uint16 group = 0;
    uint32 field = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_QUEUE_DBG_FUNC();

    kal_memset(&queue_shape, 0, sizeof(ds_queue_shape_t));
    kal_memset(&channel_profile, 0, sizeof(ds_channel_shape_profile_t));
    kal_memset(ref, 0, SYS_MAX_GROUP_NUM);

    CTC_LIST_POINTER_LOOP(p_node, &sys_channel_list[lchip][channel])
    {
        p_entry = _ctc_container_of(p_node, sys_queue_channel_entry_t, head);

        /* set all queue to drop in the channel */
        switch (p_entry->queue_id % 4)
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
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id / 4, cmd, &tmp));

        /* disable all queue shapping in the channel */
        queue_shape_profile = sys_queue_config[lchip][p_entry->queue_id].p_queue_shape_profile;
        if (queue_shape_profile)
        {
            switch(p_entry->queue_id % 4)
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
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id / 4, cmd, &tmp));

            queue_shape.commit_token = SYS_FULL_SHAPE_TOKENS;
            queue_shape.peak_token = SYS_FULL_SHAPE_TOKENS;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id, cmd, &queue_shape));
        }

        /* disable all group shapping in the channel */
        group = sys_queue_config[lchip][p_entry->queue_id].group;
        if ((group < SYS_MAX_GROUP_NUM) && (p_sys_group_info[lchip][group] != NULL))
        {
            if (ref[group] == 0)
            {
                ref[group] = 1;

                /* init group tokens */
                cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE, DS_GROUP_SHAPE_GRP_TOKEN);
                tmp = SYS_FULL_SHAPE_TOKENS;
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

                tmp = SYS_RESERVED_GROUP_SHAPE_PROFILE_ID;
                cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE_ID, field);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, group / 4, cmd, &tmp));
            }
        }
    }

    /* disable channel shapping */
    channel_profile.shape_en = 1;
    channel_profile.token_rate = 0x3FFFF;
    channel_profile.token_thrd = 0xFF;
    channel_profile.token_thrd_shift = 0xF;

    cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE, DS_CHANNEL_SHAPE_TOKEN);
    tmp = 0xFFFFFF;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel, cmd, &tmp));

    cmd = DRV_IOW(IOC_TABLE, DS_CHANNEL_SHAPE_PROFILE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, channel, cmd, &channel_profile));

    return CTC_E_NONE;
}

/**
 @brief Restore channel's all queue threshold, all queue,group,channel shapping.
*/
int32
sys_humber_queue_enable_channel(uint8 lchip, uint8 channel)
{
    sys_queue_channel_entry_t *p_entry = NULL;
    ctc_list_pointer_node_t *p_node = NULL;
    sys_queue_shape_profile_t *queue_shape_profile = NULL;
    ds_queue_shape_t queue_shape;
    sys_channel_shape_profile_t *channel_profile;
    uint8  ref[SYS_MAX_GROUP_NUM];
    uint16 group = 0;
    uint32 field = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_QUEUE_DBG_FUNC();

    kal_memset(&queue_shape, 0, sizeof(ds_queue_shape_t));
    kal_memset(ref, 0, SYS_MAX_GROUP_NUM);

    /* restore channel shapping */
    channel_profile = ctc_vector_get(p_sys_channel_shape_vec[lchip], channel);
    if (channel_profile)
    {
        CTC_ERROR_RETURN(_sys_humber_channel_shape_profile_write(lchip, channel, channel_profile));
    }

    CTC_LIST_POINTER_LOOP(p_node, &sys_channel_list[lchip][channel])
    {
        p_entry = _ctc_container_of(p_node, sys_queue_channel_entry_t, head);

        /* restore all group shapping in the channel */
        group = sys_queue_config[lchip][p_entry->queue_id].group;
        if ((group < SYS_MAX_GROUP_NUM) && (p_sys_group_info[lchip][group] != NULL))
        {
            if (ref[group] == 0)
            {
                ref[group] = 1;

                /* init group tokens */
                cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE, DS_GROUP_SHAPE_GRP_TOKEN);
                tmp = p_sys_group_info[lchip][group]->p_profile->threshold
                    << p_sys_group_info[lchip][group]->p_profile->shift;
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

                tmp = p_sys_group_info[lchip][group]->p_profile->index;
                cmd = DRV_IOW(IOC_TABLE, DS_GROUP_SHAPE_PROFILE_ID, field);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, group / 4, cmd, &tmp));
            }
        }

        /* restore all queue shapping in the channel */
        queue_shape_profile = sys_queue_config[lchip][p_entry->queue_id].p_queue_shape_profile;
        if (queue_shape_profile)
        {
            switch(p_entry->queue_id % 4)
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

            tmp = queue_shape_profile->index;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE_PROFILE_ID, field);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id / 4, cmd, &tmp));

            queue_shape.commit_token = queue_shape_profile->commit_threshold << queue_shape_profile->commit_shift;
            queue_shape.peak_token = queue_shape_profile->peak_threshold << queue_shape_profile->peak_shift;
            cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_SHAPE, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id, cmd, &queue_shape));
        }

        /* restore all queue drop threshold in the channel */
        if (sys_queue_config[lchip][p_entry->queue_id].p_drop_profile)
        {
            switch (p_entry->queue_id % 4)
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
            tmp = sys_queue_config[lchip][p_entry->queue_id].p_drop_profile->index;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, (uint32) p_entry->queue_id / 4, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}

/**
 @brief Mapping a queue in the specific local chip to the given channel.
*/
int32
sys_humber_queue_add_to_channel(uint8 lchip, uint16 queue_id, uint8 channel)
{
    sys_queue_channel_entry_t *p_entry;
    uint32 cmd;
    uint32 tmp;

    CTC_MAX_VALUE_CHECK(queue_id, SYS_MAX_QUEUE_NUM - 1);

    SYS_QUEUE_DBG_FUNC();

    p_entry = (sys_queue_channel_entry_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_queue_channel_entry_t));
    if (!p_entry)
    {
        return CTC_E_NO_MEMORY;
    }
    p_entry->queue_id = queue_id;

    /* write to asic */
    tmp = channel;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_CHANNEL_ID);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

    /* write to db */
    sys_queue_config[lchip][queue_id].channel = channel;
    sys_queue_config[lchip][queue_id].p_channel_entry = p_entry;
    ctc_list_pointer_insert_tail(&sys_channel_list[lchip][channel], &p_entry->head);

    SYS_QUEUE_DBG_INFO("Add queue to channel, lchip = %d, queue_id = %d, channel = %d\n", lchip, queue_id, channel);

    return CTC_E_NONE;
}

static int32
_sys_humber_queue_remove_queue_from_channel(uint8 lchip, uint16 queue_id, uint8 channel)
{
    sys_queue_channel_entry_t *p_entry;
    uint32 cmd;
    uint32 tmp;

    SYS_QUEUE_DBG_FUNC();

    p_entry = sys_queue_config[lchip][queue_id].p_channel_entry;
    if (!p_entry)
    {
        return CTC_E_NONE;
    }

    if(sys_queue_config[lchip][queue_id].channel != channel)
    {
        return CTC_E_INVALID_PARAM;
    }

    /* write to asic */
    tmp = SYS_DROP_CHANNEL_ID_START;
    cmd = DRV_IOW(IOC_TABLE, DS_QUEUE_MAP, DS_QUEUE_MAP_CHANNEL_ID);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, queue_id, cmd, &tmp));

    /* write to db */
    ctc_list_pointer_delete(&sys_channel_list[lchip][channel], &p_entry->head);
    mem_free(p_entry);

    sys_queue_config[lchip][queue_id].p_channel_entry = NULL;

    SYS_QUEUE_DBG_INFO("Remove queue from channel, lchip = %d, queue_id = %d, channel = %d\n", lchip, queue_id, channel);

    return CTC_E_NONE;
}

/**
 @brief Remove a queue in the specific local chip from the given channel.
*/
int32
sys_humber_queue_remove_from_channel(uint8 lchip, uint16 queue_id, uint8 channel)
{
    uint32 cmd;
    uint32 field;
    uint32 tmp;
    uint8 is_empty = 0;
    int32 ret = CTC_E_NONE;

    CTC_MAX_VALUE_CHECK(queue_id, SYS_MAX_QUEUE_NUM - 1);

    SYS_QUEUE_DBG_FUNC();

    if(sys_queue_config[lchip][queue_id].channel != channel)
    {
        return CTC_E_INVALID_PARAM;
    }

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
            CTC_ERROR_RETURN(_sys_humber_queue_remove_queue_from_channel(lchip, queue_id, channel));
        }
        else    /*need disable channel*/
        {
            CTC_ERROR_RETURN(sys_humber_queue_disable_channel(lchip, channel));
            /* wait until queue is empty */
            CTC_ERROR_RETURN(sys_humber_queue_wait_queue_empty(lchip, queue_id, &is_empty));
            if(is_empty == TRUE)
            {
                CTC_ERROR_RETURN(_sys_humber_queue_remove_queue_from_channel(lchip, queue_id, channel));
            }
            else
            {
                ret = CTC_E_QUEUE_DEPTH_NOT_EMPTY;
            }
            CTC_ERROR_RETURN(sys_humber_queue_enable_channel(lchip, channel));
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


static int32
_sys_humber_queue_write_ctl_init()
{
    q_mgr_qwrite_ctl_t q_mgr_qwrite_ctl;
    uint8 lchip = 0, gchip = 0;
    uint8 lchip_num = 0;
    uint32 cmd = 0;

    kal_memset(&q_mgr_qwrite_ctl, 0, sizeof(q_mgr_qwrite_ctl));
    cmd = DRV_IOW(IOC_REG, Q_MGR_QWRITE_CTL, DRV_ENTRY_FLAG);
    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* Q_MGR_QWRITE_CTL */
        CTC_ERROR_RETURN(sys_humber_get_gchip_id(lchip, &gchip));
        q_mgr_qwrite_ctl.bay_id = gchip;
        q_mgr_qwrite_ctl.que_sel_type_bits = 2;
        q_mgr_qwrite_ctl.gen_que_id_rx_ether_oam = 1;

        q_mgr_qwrite_ctl.rx_ether_oam_queue_base = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_OAM].queue_num_base +
        SYS_OAM_CHANNEL_ID * sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_OAM].queue_num;
        q_mgr_qwrite_ctl.rx_ether_oam_queue_select_mask = 0x38;
        q_mgr_qwrite_ctl.rx_ether_oam_queue_select_shift = 3;
        q_mgr_qwrite_ctl.flow_id_en = 1;

        if(sys_enq_ctl.service_queue_enable)
        {
            q_mgr_qwrite_ctl.service_id_en = 1;
        }
        else
        {
            q_mgr_qwrite_ctl.service_id_en = 0;
        }

        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &q_mgr_qwrite_ctl));
    }

    return CTC_E_NONE;

}


/*~~~~~~~~~~~~~~~~~~~~~~service queue functions~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 @brief Init DsQueueNumGenCtl for service ingress/egress queues.
*/
static int32
_sys_humber_service_queue_num_gen_ctl_init()
{
    uint8 lchip, lchip_num;
    uint8 index;

    SYS_QUEUE_DBG_FUNC();

    /* Init DsQueueNumGenCtl for service ingress queues */
    if(sys_enq_ctl.queue_num_per_ingress_service)
    {
        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num_base =
            (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num_base +
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num * sys_enq_ctl.internal_port_num + 3) / 4 * 4;

        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num = sys_enq_ctl.queue_num_per_ingress_service;

        switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num)
        {
            case 1:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_mask  = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_shift = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_mask       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_shift      = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].service_queue_enable = 1;
                break;

            case 2:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_mask  = 0x20;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_shift = 5;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_mask       = 0x80;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_shift      = 7;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].service_queue_enable = 1;
                break;

            case 4:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_mask  = 0x30;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_shift = 4;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_mask       = 0xC0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_shift      = 6;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].service_queue_enable = 1;
                break;

            case 8:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_mask  = 0x38;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_select_shift = 3;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_mask       = 0xE0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_shift      = 5;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].service_queue_enable = 1;
                break;

            default:
                return CTC_E_QUEUE_INVALID_CONFIG;
        }

        /* write above DsQueueNumGenCtl to local chips */
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            if(sys_enq_ctl.queue_num_per_egress_service)
            {
                /* do ingress loopback, chip id match  */
                index = SYS_QNUM_GEN_CTL_INDEX(0, 1, 1, SYS_QSEL_TYPE_SERVICE);
                CTC_ERROR_RETURN(
                    _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));

                /* do ingress loopback, chip id mis-match */
                index = SYS_QNUM_GEN_CTL_INDEX(0, 1, 0, SYS_QSEL_TYPE_SERVICE);
                CTC_ERROR_RETURN(
                    _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));
            }
            else    /*when the queue is one way direction, there is no need for SRC_QSEL to point out the ingress queue and the egress queue, so it will be set 0*/
            {
                /* do ingress loopback, chip id match  */
                index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_SERVICE);
                CTC_ERROR_RETURN(
                    _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));

                /* do ingress loopback, chip id mis-match */
                index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 0, SYS_QSEL_TYPE_SERVICE);
                CTC_ERROR_RETURN(
                    _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));
            }
        }

        SYS_QUEUE_DBG_INFO("service ingress queue, base = %d, queue_num = %d\n",
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num_base,
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num);

        if(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num_base +
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num *
            sys_enq_ctl.supported_service_num >= SYS_MAX_QUEUE_NUM)
        {
            return CTC_E_QUEUE_INVALID_CONFIG;
        }
    }

    /* Init DsQueueNumGenCtl for service egress queues */
    if(sys_enq_ctl.queue_num_per_egress_service)
    {
        if(!sys_enq_ctl.queue_num_per_ingress_service)
        {
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num_base =
                (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num_base +
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_INTERNAL_PORT].queue_num * sys_enq_ctl.internal_port_num + 3) / 4 * 4;
        }
        else
        {
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num_base =
                (sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num_base +
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num * sys_enq_ctl.supported_service_num + 3) / 4 * 4;
        }

        sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num = sys_enq_ctl.queue_num_per_egress_service;

        switch(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num)
        {
            case 1:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_mask  = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_shift = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_mask       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_shift      = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].service_queue_enable = 1;
                break;

            case 2:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_mask  = 0x20;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_shift = 5;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_mask       = 0x80;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_shift      = 7;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].service_queue_enable = 1;
                break;

            case 4:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_mask  = 0x30;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_shift = 4;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_mask       = 0xC0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_shift      = 6;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].service_queue_enable = 1;
                break;

            case 8:
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_mask  = 0x38;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_select_shift = 3;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_mask       = 0xE0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_shift      = 5;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].flow_id_base       = 0;
                sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].service_queue_enable = 1;
                break;

            default:
                return CTC_E_QUEUE_INVALID_CONFIG;
        }

        /* write above DsQueueNumGenCtl to local chips */
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_SERVICE);
            CTC_ERROR_RETURN(
                _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS]));
        }

        SYS_QUEUE_DBG_INFO("service egress queue, base = %d, queue_num = %d\n",
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num_base,
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num);

        if(sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num_base +
            sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num *
            sys_enq_ctl.supported_service_num >= SYS_MAX_QUEUE_NUM)
        {
            return CTC_E_QUEUE_INVALID_CONFIG;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief De-init DsQueueNumGenCtl for service ingress/egress queues.
*/
static int32
_sys_humber_service_queue_num_gen_ctl_deinit(void)
{
    uint8 lchip, lchip_num;
    uint8 index;

    if (sys_enq_ctl.current_service_num > 0)
    {
        return CTC_E_SERVICE_EXIST;
    }

    /* release service ingress queues */
    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 1, 1, SYS_QSEL_TYPE_SERVICE);
        CTC_ERROR_RETURN(
            _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));

        index = SYS_QNUM_GEN_CTL_INDEX(0, 1, 0, SYS_QSEL_TYPE_SERVICE);
        CTC_ERROR_RETURN(
            _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS]));
    }

    /* release service egress queues */
    kal_memset(&sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS], 0, sizeof(sys_queue_num_gen_ctl_t));

    /* write above DsQueueNumGenCtl to local chips */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index = SYS_QNUM_GEN_CTL_INDEX(0, 0, 1, SYS_QSEL_TYPE_SERVICE);
        CTC_ERROR_RETURN(
            _sys_humber_queue_num_gen_ctl_write(lchip, index, &sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS]));
    }

    return CTC_E_NONE;
}



/**
 @brief Lookup a service by the given service ID.
*/
int32
sys_humber_service_lookup(uint16 logical_service_id, sys_service_config_t** pp_service)
{
    ctc_list_pointer_node_t *p_node;
    sys_service_config_t *p_service;
    ctc_list_pointer_t *p_list;

    CTC_PTR_VALID_CHECK(pp_service);

    SYS_QUEUE_DBG_FUNC();

    *pp_service = NULL;
    /*COMPILE ERROR: sys_humber_queue_enq.c:1563: error: the address of 'sys_service_list' will always evaluate as 'true'*/
    p_list = &sys_service_list;
    CTC_LIST_POINTER_LOOP(p_node, p_list)
    {
        p_service = _ctc_container_of(p_node, sys_service_config_t, head);
        if (p_service->logical_service_id == logical_service_id)
        {
            *pp_service = p_service;
            return CTC_E_NONE;
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_get_service_queue_hash_index(sys_service_queue_hash_index_t* p_index)
{
    uint8  bucket_index;
    uint8  depth;
    uint32 cmd, field_service_id, field_dest_id;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_index);

    SYS_QUEUE_DBG_FUNC();

    p_index->bucket_index = 0xFF;
    p_index->depth = 0xFF;

    SYS_SERVICE_QUEUE_HASH_INDEX(p_index->service_id, p_index->dest_id, bucket_index);

    /* first lookup in DsServiceQueueHashKey */
    for (depth = 0; depth < SYS_SERVICE_QUEUE_HASH_BUCKET_DEPTH; depth++)
    {
        SYS_GET_SERVICE_QUEUE_HASH_KEY_FIELD(depth, field_service_id, field_dest_id);

        cmd = DRV_IOR(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY,field_service_id);
        CTC_ERROR_RETURN(drv_tbl_ioctl(0, bucket_index, cmd, &tmp));
        if (tmp == 0xFFFF)
        {
            p_index->bucket_index = bucket_index;
            p_index->depth = depth;
            return CTC_E_NONE;
        }
    }

    /* not found in DsServiceQueueHashKey, lookup QmgrHashCamCtl */
    for (depth = 0; depth < SYS_QMGR_HASH_CAM_CTL_SIZE; depth++)
    {
        SYS_GET_QMGR_HASH_CAM_CTL_FIELD (depth, field_service_id, field_dest_id);

        cmd = DRV_IOR(IOC_REG, Q_MGRQ_HASH_CAM_CTL, field_service_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &tmp));
        if (tmp == 0xFFFF)
        {
            p_index->bucket_index = 0xFF;
            p_index->depth = depth;
            break;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief Write service queue base offset to DsServiceQueue.
*/
static int32
_sys_humber_service_queue_base_offset_write(uint8 lchip, sys_service_queue_hash_index_t* p_index, uint16 service_queue_base)
{
    uint32 index;
    uint32 cmd;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_index);

    SYS_QUEUE_DBG_FUNC();

    if (p_index->bucket_index < SYS_SERVICE_QUEUE_HASH_BUCKET_NUM)
    {
        index = p_index->bucket_index * 4 + p_index->depth;
        tmp = service_queue_base;
        cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE, DS_SERVICE_QUEUE_SERVICE_QUEUE_BASE_OFFSET);

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &tmp));

        SYS_QUEUE_DBG_INFO("write service queue base to DsServiceQueue, index = %d, service_queue_base = %d\n", index, service_queue_base);
    }
    else if (p_index->bucket_index == 0xFF && p_index->depth < SYS_QMGR_HASH_CAM_CTL_SIZE)
    {
        index = p_index->depth;
        cmd = DRV_IOW(IOC_TABLE, Q_MGRQ_HASH_CAM_HASH_CTL, Q_MGRQ_HASH_CAM_HASH_CTL_SERVICE_QUE_BASE_OFFSET);
        tmp = service_queue_base;
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, index, cmd, &tmp));

        SYS_QUEUE_DBG_INFO("write service queue base to QmgrHashCamResultCtl, index = %d, service_queue_base = %d\n", index, service_queue_base);
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}



/**
 @brief Write service queue hash key to DsServiceHashKey.
*/
int32
sys_humber_service_queue_hash_write(sys_service_queue_hash_index_t* p_index)
{
    uint8  lchip, lchip_num;
    uint8  depth = 0;
    uint32 cmd, field_dest_id, field_service_id;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_index);

    SYS_QUEUE_DBG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();

    /* write to DsServiceQueueHashKey */
    if (p_index->bucket_index < SYS_SERVICE_QUEUE_HASH_BUCKET_NUM)
    {
        SYS_GET_SERVICE_QUEUE_HASH_KEY_FIELD(p_index->depth, field_service_id, field_dest_id);

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY, field_service_id);
            tmp = p_index->service_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_index->bucket_index, cmd, &tmp));

            cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY, field_dest_id);
            tmp = p_index->dest_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_index->bucket_index, cmd, &tmp));
        }
        SYS_QUEUE_DBG_INFO("write DsServiceQueueHashKey, bucket = %d, depth = %d, "
                           "service_id = %d, dest_id = %d\n", p_index->bucket_index , depth, p_index->service_id, p_index->dest_id);


        return CTC_E_NONE;
    }
    else
    {
        SYS_GET_QMGR_HASH_CAM_CTL_FIELD (p_index->depth, field_service_id, field_dest_id);

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, Q_MGRQ_HASH_CAM_CTL, field_service_id);
            tmp = p_index->service_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, depth, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, Q_MGRQ_HASH_CAM_CTL, field_dest_id);
            tmp = p_index->dest_id;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, depth, cmd, &tmp));
        }

        SYS_QUEUE_DBG_INFO("write QmgrHashCamCtl, depth = %d, "
                           "service_id = %d, dest_id = %d\n", depth, p_index->service_id, p_index->dest_id);
    }

    return CTC_E_NONE;
}


/**
 @brief Remove hash key from DsServiceHashKey.
*/
int32
sys_humber_service_queue_hash_remove(sys_service_queue_hash_index_t* p_index)
{
    uint8  lchip, lchip_num;

    uint8  depth = 0;
    uint32 cmd, field_dest_id, field_service_id;
    uint32 tmp;

    CTC_PTR_VALID_CHECK(p_index);

    SYS_QUEUE_DBG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();

    /* write to DsServiceQueueHashKey */
    if (p_index->bucket_index < SYS_SERVICE_QUEUE_HASH_BUCKET_NUM)
    {
        SYS_GET_SERVICE_QUEUE_HASH_KEY_FIELD(p_index->depth, field_service_id, field_dest_id);

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY, field_service_id);
            tmp = 0xFFFF;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_index->bucket_index, cmd, &tmp));

            cmd = DRV_IOW(IOC_TABLE, DS_SERVICE_QUEUE_HASH_KEY, field_dest_id);
            tmp = 0x3FF;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_index->bucket_index, cmd, &tmp));
        }

        SYS_QUEUE_DBG_INFO("remove DsServiceQueueHashKey, bucket = %d, depth = %d\n",
                           p_index->bucket_index , depth);

        return CTC_E_NONE;
    }
    else
    {
        SYS_GET_QMGR_HASH_CAM_CTL_FIELD(p_index->depth, field_service_id, field_dest_id);

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, Q_MGRQ_HASH_CAM_CTL, field_service_id);
            tmp = 0xFFFF;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, depth, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, Q_MGRQ_HASH_CAM_CTL, field_dest_id);
            tmp = 0x3FF;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, depth, cmd, &tmp));
        }

        SYS_QUEUE_DBG_INFO("remove QmgrHashCamCtl, depth = %d\n", depth);
    }

    return CTC_E_NONE;
}


/**
 @brief To globally enable/disable QoS flow ID.
*/
int32
sys_humber_queue_flow_id_enable(bool enable)
{
    uint32 cmd;
    uint32 tmp;
    uint8  lchip, lchip_num;

    SYS_QUEUE_DBG_FUNC();

    if (sys_enq_ctl.flow_id_enable != enable)
    {
        cmd = DRV_IOW(IOC_REG, Q_MGR_QWRITE_CTL, Q_MGR_QWRITE_CTL_FLOW_ID_EN);
        tmp = enable ? 1 : 0;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        sys_enq_ctl.flow_id_enable = enable;

        SYS_QUEUE_DBG_INFO("sys_enq_ctl.flow_id_enable = %d\n", enable);
    }

    return CTC_E_NONE;
}


/**
 @brief Get QoS flow ID global enable status.
*/
int32
sys_humber_queue_get_flow_id_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QUEUE_DBG_FUNC();

    *p_enable = sys_enq_ctl.flow_id_enable;

    SYS_QUEUE_DBG_INFO("sys_enq_ctl.flow_id_enable = %d\n", *p_enable);

    return CTC_E_NONE;
}


/**
 @brief Enableservice ID.
*/
int32
sys_humber_service_id_enable()
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_service_queue_num_gen_ctl_init());

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        tmp = 1;
        cmd = DRV_IOW(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_SERVICE_ID_EN_CFG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, Q_MGR_QWRITE_CTL, Q_MGR_QWRITE_CTL_SERVICE_ID_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, Q_HASH_LOOKUP_RESULT_CTL, Q_HASH_LOOKUP_RESULT_CTL_SERVICE_QUE_BASE_DEFAULT);
        tmp = 0xFFF;
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_enq_ctl.service_queue_enable = 1;

    SYS_QUEUE_DBG_INFO("Service queue is globally enabled\n");

    return CTC_E_NONE;
}


/**
 @brief Disable service ID.
*/
int32
sys_humber_service_id_disable(void)
{
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 tmp;

    SYS_QUEUE_DBG_FUNC();

    if (!sys_enq_ctl.service_queue_enable)
    {
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(_sys_humber_service_queue_num_gen_ctl_deinit());

    tmp = 1;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_SERVICE_ID_EN_CFG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, Q_MGR_QWRITE_CTL, Q_MGR_QWRITE_CTL_SERVICE_ID_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    sys_enq_ctl.service_queue_enable = 0;

    SYS_QUEUE_DBG_INFO("Service queue is globally disabled\n");

    return CTC_E_NONE;
}

/**
 @brief Enable/Disable service ID.
*/
int32
sys_humber_get_service_id_enable(bool* p_enable)
{
    CTC_PTR_VALID_CHECK(p_enable);

    SYS_QUEUE_DBG_FUNC();

    *p_enable = sys_enq_ctl.service_queue_enable;

    SYS_QUEUE_DBG_INFO("sys_enq_ctl.service_queue_enable = %d\n", sys_enq_ctl.service_queue_enable);

    return CTC_E_NONE;
}


/**
 @brief Create a service ID.
*/
int32
sys_humber_create_physical_service_id(uint16 logical_service_id, uint8 dest_port)
{
    uint16 physical_service_id = 0;
    uint16 idx, bit;
    sys_service_queue_hash_index_t *p_hash_index[2];
    sys_service_config_t *p_service = NULL;
    ctc_list_pointer_node_t *p_node, *p_next_node;
    int32  ret;

    SYS_QUEUE_DBG_FUNC();

    /* sanity check */
    /*~~ logcial service id must not be 0 ~~*/
    CTC_NOT_ZERO_CHECK(logical_service_id);

    if (sys_enq_ctl.current_service_num >= sys_enq_ctl.supported_service_num)
    {
        return CTC_E_SERVICE_EXCEED_MAX_COUNT_LIMIT;
    }

    /* lookup service to find whether a service already exist with the same service id */
    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (p_service)
    {
        return CTC_E_SERVICE_EXIST;
    }

    kal_memset(p_hash_index, 0, sizeof(sys_service_queue_hash_index_t*) * 2);

    /* create service queue hash index */
    for (idx = 0; idx < 2; idx++)
    {
        p_hash_index[idx] = (sys_service_queue_hash_index_t *)mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_service_queue_hash_index_t));
        if (!p_hash_index[idx])
        {
            ret = CTC_E_NO_MEMORY;
            goto error;
        }
        kal_memset(p_hash_index[idx], 0xFF, sizeof(sys_service_queue_hash_index_t));
    }

    /* In order to avoid hash collision, attempt to find an available service id */
    for (physical_service_id = 0; physical_service_id < SYS_SERVICE_ID_NUM; physical_service_id++)
    {
        idx = physical_service_id / 32;
        bit = physical_service_id % 32;
        if (IS_BIT_SET(sys_service_id_bitmap[idx], bit))
        {
            continue;
        }

        /* service ingress queue, unicast */
        p_hash_index[0]->service_id = physical_service_id;
        p_hash_index[0]->dest_id = (0 << 9) | (0 << 8) | (dest_port & 0xFF);
        CTC_ERROR_RETURN(_sys_humber_get_service_queue_hash_index(p_hash_index[0]));
        if (0xFF == p_hash_index[0]->bucket_index && 0xFF == p_hash_index[0]->depth)
        {
            continue;
        }

        /* service ingress queue, multicast */
        p_hash_index[1]->service_id = physical_service_id;
        p_hash_index[1]->dest_id = (1 << 9) | (0 << 8) | (dest_port & 0xFF);
        CTC_ERROR_RETURN(_sys_humber_get_service_queue_hash_index(p_hash_index[1]));
        if (0xFF == p_hash_index[1]->bucket_index && 0xFF == p_hash_index[1]->depth)
        {
            continue;
        }

        SET_BIT(sys_service_id_bitmap[idx], bit);
        break;
    }

    if (physical_service_id >= SYS_SERVICE_ID_NUM)
    {
        ret = CTC_E_SERVICE_QUEUE_NO_HASH_ENTRY;
        goto error;
    }

    /* surely, there are available hash entries, go on to create service */
    p_service = (sys_service_config_t *) mem_malloc(MEM_QUEUE_MODULE, sizeof(sys_service_config_t));
    if (!p_service)
    {
        ret = CTC_E_NO_MEMORY;
        goto error;
    }
    kal_memset(p_service, 0, sizeof(sys_service_config_t));
    ctc_list_pointer_init(&p_service->hash_key_idx_list);
    p_service->logical_service_id = logical_service_id;
    p_service->physical_service_id = physical_service_id;

    SYS_QUEUE_DBG_INFO("create service, logical_service_id = %d, physical_service_id = %d\n",
                       logical_service_id, physical_service_id);

    ctc_list_pointer_insert_tail(&sys_service_list, &(p_service->head));

    for (idx = 0; idx < 2; idx++)
    {
        ret  = sys_humber_service_queue_hash_write(p_hash_index[idx]);
        if (ret)
        {
            goto error;
        }

        ctc_list_pointer_insert_tail(&p_service->hash_key_idx_list, &(p_hash_index[idx]->head));
    }

    sys_enq_ctl.current_service_num++;

    return CTC_E_NONE;

error:
    if (p_service)
    {
        idx = 0;
        CTC_LIST_POINTER_LOOP_DEL(p_node, p_next_node, &p_service->hash_key_idx_list)
        {
            p_hash_index[idx] = _ctc_container_of(p_node, sys_service_queue_hash_index_t, head);
            ctc_list_pointer_delete(&p_service->hash_key_idx_list, p_node);
            sys_humber_service_queue_hash_remove(p_hash_index[idx]);
            mem_free(p_hash_index[idx]);
            idx++;
        }

        ctc_list_pointer_delete(&sys_service_list, &p_service->head);
        mem_free(p_service);
    }

    for (idx = 0; idx < 2; idx++)
    {
        if (p_hash_index[idx])
        {
            mem_free(p_hash_index[idx]);
        }
    }

    idx = physical_service_id / 32;
    bit = physical_service_id % 32;
    CLEAR_BIT(sys_service_id_bitmap[idx], bit);

    return ret;
}


/**
 @brief Remove a service ID.
*/
int32
sys_humber_remove_physical_service_id(uint16 logical_service_id)
{
    uint16 idx, bit;
    sys_service_config_t *p_service = NULL;
    ctc_list_pointer_node_t *p_node, *p_next_node;
    sys_service_queue_hash_index_t *p_hash_index;

    SYS_QUEUE_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    idx = p_service->physical_service_id / 32;
    bit = p_service->physical_service_id % 32;

    CTC_LIST_POINTER_LOOP_DEL(p_node, p_next_node, &p_service->hash_key_idx_list)
    {
        p_hash_index = _ctc_container_of(p_node, sys_service_queue_hash_index_t, head);
        ctc_list_pointer_delete(&p_service->hash_key_idx_list, p_node);
        sys_humber_service_queue_hash_remove(p_hash_index);
        mem_free(p_hash_index);
    }

    CLEAR_BIT(sys_service_id_bitmap[idx], bit);
    sys_enq_ctl.current_service_num--;

    ctc_list_pointer_delete(&sys_service_list, &p_service->head);
    mem_free(p_service);

    return CTC_E_NONE;
}



/**
 @brief Init service ingress/egress queue for the given service.
*/
int32
sys_humber_service_queue_init(uint16 logical_service_id, uint8 dest_port)
{
    sys_service_config_t *p_service;
    uint32 offset;
    uint16 queue_id;
    ctc_queue_drop_t queue_drop;
    uint8  drop_prec;
    sys_humber_opf_t opf;
    uint16 service_queue_base;
    ctc_list_pointer_node_t *p_node;
    sys_service_queue_hash_index_t *p_index;
    uint8  lchip, lchip_num;

    SYS_QUEUE_DBG_FUNC();

    if((!sys_enq_ctl.queue_num_per_ingress_service) && (!sys_enq_ctl.queue_num_per_egress_service))
    {
        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    opf.pool_type = OPF_SERVICE_QUEUE;
    opf.pool_index = 0;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

    if(sys_enq_ctl.queue_num_per_ingress_service)
    {
        service_queue_base = offset * sys_enq_ctl.queue_num_per_ingress_service;
    }
    else
    {
        service_queue_base = offset * sys_enq_ctl.queue_num_per_egress_service;
    }

    p_service->opf_offset = offset;
    p_service->ingress_queue_base = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_INGRESS].queue_num_base +
                                    service_queue_base;
    p_service->egress_queue_base  = sys_enq_ctl.qnum_gen_ctl[CTC_QUEUE_TYPE_SERVICE_EGRESS].queue_num_base +
                                    service_queue_base;

    /* write service queue base offset */
    CTC_LIST_POINTER_LOOP(p_node, &p_service->hash_key_idx_list)
    {
        p_index = _ctc_container_of(p_node, sys_service_queue_hash_index_t, head);
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            CTC_ERROR_RETURN(_sys_humber_service_queue_base_offset_write(lchip, p_index, service_queue_base));
        }
    }

    kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));
    queue_drop.mode = CTC_QUEUE_DROP_WTD;
    for (drop_prec = 0; drop_prec < CTC_DROP_PREC_NUM; drop_prec++)
    {
        queue_drop.max_th[drop_prec] = SYS_DROP_COUNT_GE + drop_prec * SYS_DROP_DELTA_GE;
    }

    if(sys_enq_ctl.queue_num_per_ingress_service)
    {
        for (offset = 0; offset < sys_enq_ctl.queue_num_per_ingress_service; offset++)
        {
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                /* service ingress queue initialization */
                queue_id = p_service->ingress_queue_base + offset;

                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, dest_port));
                CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, 0));
                CTC_ERROR_RETURN(sys_humber_queue_set_queue_wdrr_weight(lchip, queue_id, 1));
                CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
            }
        }
    }

    if(sys_enq_ctl.queue_num_per_egress_service)
    {
        for (offset = 0; offset < sys_enq_ctl.queue_num_per_egress_service; offset++)
        {
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                /* service egress queue initialization */
                queue_id = p_service->egress_queue_base + offset;

                CTC_ERROR_RETURN(sys_humber_queue_add_to_channel(lchip, queue_id, dest_port));
                CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, 0));
                CTC_ERROR_RETURN(sys_humber_queue_set_queue_wdrr_weight(lchip, queue_id, 1));
                CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
            }
        }
    }

    return CTC_E_NONE;
}


/**
 @brief De-init service ingress/egress queue for the given service.
*/
int32
sys_humber_service_queue_deinit(uint16 logical_service_id)
{
    sys_service_config_t *p_service;
    uint32 offset;
    uint16 queue_id;
    ctc_queue_drop_t queue_drop;
    uint8  drop_prec;
    sys_humber_opf_t opf;
    uint8  lchip, lchip_num;
    ctc_group_shape_cfg_t group_shape;

    SYS_QUEUE_DBG_FUNC();

    kal_memset(&group_shape, 0, sizeof(ctc_group_shape_cfg_t));

    lchip_num = sys_humber_get_local_chip_num();

    CTC_ERROR_RETURN(sys_humber_service_lookup(logical_service_id, &p_service));
    if (!p_service)
    {
        return CTC_E_SERVICE_NOT_EXIST;
    }

    if (p_service->register_num)
    {
        return CTC_E_SERVICE_REGISTERED;
    }

    group_shape.type = CTC_SERVICE_GROUP_SHAPE;
    group_shape.para.serviceId = logical_service_id;
    if (p_service->p_igs_group[0])
    {
        group_shape.dir = CTC_INGRESS;
        CTC_ERROR_RETURN(sys_humber_group_unset_shape(&group_shape));
    }

    if (p_service->p_egs_group[0])
    {
        group_shape.dir = CTC_EGRESS;
        CTC_ERROR_RETURN(sys_humber_group_unset_shape(&group_shape));
    }

    if(sys_enq_ctl.queue_num_per_ingress_service)
    {
        for (offset = 0; offset < sys_enq_ctl.queue_num_per_ingress_service; offset++)
        {
            /* service ingress queue de-initialization */
            queue_id = p_service->ingress_queue_base + offset;

            /* default queue drop scheme */
            kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));
            queue_drop.mode = CTC_QUEUE_DROP_WTD;
            for (drop_prec = 0; drop_prec < CTC_DROP_PREC_NUM; drop_prec++)
            {
                queue_drop.max_th[drop_prec] = SYS_DROP_COUNT_GE + drop_prec * SYS_DROP_DELTA_GE;
            }

            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                if (sys_queue_config[lchip][queue_id].p_queue_shape_profile)
                {
                    CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));
                }

                CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
                CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, sys_queue_config[lchip][queue_id].channel));
            }
        }
    }

    if(sys_enq_ctl.queue_num_per_egress_service)
    {
        for (offset = 0; offset < sys_enq_ctl.queue_num_per_egress_service; offset++)
        {
            /* service egress queue initialization */
            queue_id = p_service->egress_queue_base + offset;

            /* reset service egress queue drop to default drop scheme */
            kal_memset(&queue_drop, 0, sizeof(ctc_queue_drop_t));
            queue_drop.mode = CTC_QUEUE_DROP_WTD;
            for (drop_prec = 0; drop_prec < CTC_DROP_PREC_NUM; drop_prec++)
            {
                queue_drop.max_th[drop_prec] = SYS_DROP_COUNT_GE + drop_prec * SYS_DROP_DELTA_GE;
            }

            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                if (sys_queue_config[lchip][queue_id].p_queue_shape_profile)
                {
                    CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));
                }

                CTC_ERROR_RETURN(sys_humber_queue_set_drop(lchip, queue_id, &queue_drop));
                CTC_ERROR_RETURN(sys_humber_queue_remove_from_channel(lchip, queue_id, sys_queue_config[lchip][queue_id].channel));
            }
        }
    }

    opf.pool_type = OPF_SERVICE_QUEUE;
    opf.pool_index = 0;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_service->opf_offset));

    p_service->ingress_queue_base = 0;
    p_service->egress_queue_base = 0;
    p_service->opf_offset = 0;

    return CTC_E_NONE;
}


/**
 @brief Get per-service queue number.
*/
uint8
sys_humber_get_per_service_queue_num(void)
{
    if(sys_enq_ctl.queue_num_per_egress_service)
    {
        return sys_enq_ctl.queue_num_per_egress_service;
    }
    else
    {
        return sys_enq_ctl.queue_num_per_ingress_service;
    }
}

/**
 @brief Get service ingress queue enable.
*/
int32
sys_humber_get_service_ingress_queue_enable(bool* enable)
{
    *enable = sys_enq_ctl.queue_num_per_ingress_service ? 1 : 0;
    return CTC_E_NONE;
}

/**
 @brief Get service egress queue enable.
*/
int32
sys_humber_get_service_egress_queue_enable(bool* enable)
{
    *enable = sys_enq_ctl.queue_num_per_egress_service ? 1 : 0;
    return CTC_E_NONE;
}



/**
 @brief set priority color map to queue select and drop_precedence.
*/
int32
sys_humber_set_priority_queue_map(uint8 lchip, ctc_queue_pri_map_t* p_queue_pri_map)
{
    uint32 index = 0;
    uint32 cmd = 0;
    met_fifo_priority_map_table_t met_fifo_priority_map_table;

    kal_memset(&met_fifo_priority_map_table, 0, sizeof(met_fifo_priority_map_table));

    index = (p_queue_pri_map->priority << 2) | p_queue_pri_map->color;

    cmd = DRV_IOR(IOC_TABLE, MET_FIFO_PRIORITY_MAP_TABLE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &met_fifo_priority_map_table));

    met_fifo_priority_map_table.drop_precedence = p_queue_pri_map->drop_precedence;
    /* the queue select and pri should be revert */
    met_fifo_priority_map_table.queue_select = 64 - (p_queue_pri_map->queue_select+1);

    cmd = DRV_IOW(IOC_TABLE, MET_FIFO_PRIORITY_MAP_TABLE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &met_fifo_priority_map_table));

    return CTC_E_NONE;
}


/**
 @brief reserved channel drop init
*/
int32 _sys_humber_rsv_channel_drop_init()
{
    uint32 cmd = 0;
    uint8  lchip, lchip_num;
    q_mgr_reserved_channel_range_t  qmgt_rsv_chan_rag;

    kal_memset(&qmgt_rsv_chan_rag, 0, sizeof(q_mgr_reserved_channel_range_t));

    /*rsv channel valid0 set*/
    qmgt_rsv_chan_rag.reserved_channel_valid0 = 1;
    qmgt_rsv_chan_rag.reserved_channel_min0 = SYS_DROP_CHANNEL_ID_START ;
    qmgt_rsv_chan_rag.reserved_channel_max0 = SYS_DROP_CHANNEL_ID_END  ;
    qmgt_rsv_chan_rag.reserved_channel_valid1 = 1;
    qmgt_rsv_chan_rag.reserved_channel_min1 = SYS_DROP_CHANNEL_ID_START ;
    qmgt_rsv_chan_rag.reserved_channel_max1 = SYS_DROP_CHANNEL_ID_END  ;


    cmd =  DRV_IOW(IOC_REG, Q_MGR_RESERVED_CHANNEL_RANGE, DRV_ENTRY_FLAG);
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
         CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qmgt_rsv_chan_rag));
    }

    return CTC_E_NONE;

}


/**
 @brief En-queue component initialization.
*/
int32
sys_humber_queue_enq_init(ctc_queue_global_cfg_t* queue_global_cfg)
{
    uint8  lchip, lchip_num;
    uint16 channel;
    sys_humber_opf_t opf;

    kal_memset(&sys_enq_ctl, 0, sizeof(sys_enq_ctl));
    kal_memset(sys_queue_config, 0, sizeof(sys_queue_config));
    kal_memset(sys_service_id_bitmap, 0, sizeof(sys_service_id_bitmap));

    sys_enq_ctl.queue_num_per_network_port = queue_global_cfg->queue_num_per_network_port;
    sys_enq_ctl.queue_num_per_static_int_port = queue_global_cfg->queue_num_per_static_int_port;
    sys_enq_ctl.queue_num_per_fabric = queue_global_cfg->queue_num_per_fabric;
    sys_enq_ctl.max_internal_port_id = queue_global_cfg->max_internal_port_id;
    sys_enq_ctl.queue_num_per_internal_port = queue_global_cfg->queue_num_per_internal_port;
    sys_enq_ctl.supported_service_num = queue_global_cfg->service_num;
    sys_enq_ctl.queue_num_per_ingress_service = queue_global_cfg->queue_num_per_ingress_service;
    sys_enq_ctl.queue_num_per_egress_service = queue_global_cfg->queue_num_per_egress_service;

    CTC_MIN_VALUE_CHECK(sys_enq_ctl.max_internal_port_id, SYS_HUMBER_INTERNAL_PORT_START);

    if((sys_enq_ctl.queue_num_per_ingress_service && sys_enq_ctl.queue_num_per_egress_service)
        && (sys_enq_ctl.queue_num_per_ingress_service != sys_enq_ctl.queue_num_per_egress_service))
    {
        return CTC_E_QUEUE_INVALID_CONFIG;
    }

    ctc_list_pointer_init(&sys_service_list);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        for (channel = 0; channel < SYS_MAX_CHANNEL_NUM; channel++)
        {
            ctc_list_pointer_init(&sys_channel_list[lchip][channel]);
        }
    }

    CTC_ERROR_RETURN(sys_humber_set_local_switch_enable(TRUE));

    CTC_ERROR_RETURN(_sys_humber_regular_queue_num_gen_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_static_int_port_queue_num_gen_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_fabric_queue_num_gen_ctl_init());
    CTC_ERROR_RETURN(sys_humber_excp_cpu_queue_num_gen_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_sgmac_queue_num_gen_ctl_init());
    /* init internal port queue */
    CTC_ERROR_RETURN(_sys_humber_internal_port_queue_num_gen_ctl_init());

    /* init service queue */
    CTC_ERROR_RETURN(_sys_humber_service_queue_hash_key_init());
    if((sys_enq_ctl.queue_num_per_egress_service || sys_enq_ctl.queue_num_per_ingress_service)
        && sys_enq_ctl.supported_service_num)
    {
        CTC_ERROR_RETURN(_sys_humber_queue_ingress_resource_alloc(SYS_RESRC_SINGLE_CHIP_WITH_SERVICE_QUEUE));

        /* init service Id */
        CTC_ERROR_RETURN(sys_humber_opf_init(OPF_SERVICE_QUEUE, lchip_num));

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            opf.pool_type = OPF_SERVICE_QUEUE;
            opf.pool_index = lchip;

            CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, sys_enq_ctl.supported_service_num));
        }

        CTC_ERROR_RETURN(sys_humber_service_id_enable());
    }
    else
    {
        CTC_ERROR_RETURN(_sys_humber_queue_ingress_resource_alloc(SYS_RESRC_SINGLE_CHIP_WITHOUT_SERVICE_QUEUE));
    }

    CTC_ERROR_RETURN(_sys_humber_queue_write_ctl_init());
    CTC_ERROR_RETURN(_sys_humber_rsv_channel_drop_init());

    return CTC_E_NONE;
}



