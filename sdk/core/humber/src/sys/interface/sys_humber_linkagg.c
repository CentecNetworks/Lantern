/**
 @file sys_humber_linkagg.c

 @date 2009-10-19

 @version v2.0

 The file contains all Linkagg APIs of sys layer
*/

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_debug.h"
#include "ctc_error.h"

#include "sys_humber_linkagg.h"
#include "sys_humber_chip.h"
#include "sys_humber_port.h"
#include "sys_humber_internal_port.h"

#include "drv_io.h"
#include "drv_humber.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
static sys_linkagg_master_t *p_linkagg_master = NULL;
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/

/**
 @brief The function is to get index of unused member port position
*/
static int32
_sys_humber_linkagg_get_fisrt_unused_pos(sys_linkagg_t *p_linkagg, uint8 *index)
{
    uint8 idx = 0;

    CTC_PTR_VALID_CHECK(p_linkagg);
    CTC_PTR_VALID_CHECK(index);

    for(idx = 0; idx < CTC_MAX_LINKAGG_MEMBER_PORT; idx++)
    {
        if(!(p_linkagg->port[idx].valid))
        {
            *index = idx;
            return CTC_E_NONE;
        }
    }

    return CTC_E_EXCEED_MAX_SIZE;
}

/**
 @brief The function is update asic table, add/remove member port from linkagg.
*/
static int32
_sys_humber_linkagg_update_table(sys_linkagg_t *p_linkagg, bool is_add_port, uint8 port_index)
{
    uint8 gchip = 0;
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint16 agg_base = 0;
    uint32 cmd_mem = 0;
    uint32 cmd_linkagg_w = 0;
    uint32 cmd_linkagg_r = 0;
    uint32 port_cnt = 0;
    ds_link_aggregation_t ds_linkagg;

    CTC_PTR_VALID_CHECK(p_linkagg);

    kal_memset(&ds_linkagg, 0, sizeof(ds_link_aggregation_t));
    cmd_mem = DRV_IOW(IOC_TABLE, DS_LINK_AGG_MEMBER_NUM, DS_LINK_AGG_MEMBER_NUM_LINK_AGG_MEM_NUM);
    cmd_linkagg_w = DRV_IOW(IOC_TABLE, DS_LINK_AGGREGATION, DRV_ENTRY_FLAG);
    cmd_linkagg_r = DRV_IOR(IOC_TABLE, DS_LINK_AGGREGATION, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    agg_base = (p_linkagg->tid) * CTC_MAX_LINKAGG_MEMBER_PORT;

    if(is_add_port)
    {
        /*add port, operate port first, then increase num*/
        for(chip = 0; chip < lchip_num; chip++)
        {
            port_cnt = p_linkagg->port_cnt;
            /*update only the last one*/
            ds_linkagg.dest_chip_id = SYS_MAP_GPORT_TO_GCHIP(p_linkagg->port[port_index].gport);
            ds_linkagg.dest_queue = CTC_MAP_GPORT_TO_LPORT(p_linkagg->port[port_index].gport);
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, (agg_base + port_index), cmd_linkagg_w, &ds_linkagg));

            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, p_linkagg->tid, cmd_mem, &port_cnt));
        }
    }
    else
    {
        /*remove port, decrease num first, then operate port*/
        for(chip = 0; chip < lchip_num; chip++)
        {
            /*before this function calling, the port cnt has been decreased.*/
            port_cnt = p_linkagg->port_cnt;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, p_linkagg->tid, cmd_mem, &port_cnt));

            /*copy the last one to the removed port position,and remove member port from linkagg at tail*/
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, (agg_base + port_cnt), cmd_linkagg_r, &ds_linkagg));
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, (agg_base + port_index), cmd_linkagg_w, &ds_linkagg));

            sys_humber_get_gchip_id(chip, &gchip);
            ds_linkagg.dest_chip_id = gchip;
            ds_linkagg.dest_queue = SYS_RESERVED_INTERNAL_PORT_FOR_DROP;
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, (agg_base + port_cnt), cmd_linkagg_w, &ds_linkagg));
        }
    }

    return CTC_E_NONE;
}

/**
 @brief The function is to check whether the port is member of the linkagg.
*/
static bool
_sys_humber_linkagg_port_is_member(sys_linkagg_t *p_linkagg, uint16 gport, uint8 *index)
{
    uint8 idx = 0;

    CTC_PTR_VALID_CHECK(p_linkagg);
    CTC_PTR_VALID_CHECK(index);

    for(idx = 0; idx < p_linkagg->port_cnt; idx++)
    {
        if((p_linkagg->port[idx].gport == gport) && (p_linkagg->port[idx].valid))
        {
            *index = idx;
            return TRUE;
        }
    }

    return FALSE;
}

/**
 @brief The function is to init the linkagg module
*/
int32
sys_humber_linkagg_init(void)
{
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint8 gchip = 0;
    uint16 agg_idx = 0;
    uint16 mem_idx = 0;
    uint16 index = 0;
    uint32 cmd_mem = 0;
    uint32 cmd_linkagg = 0;
    int32 ret = CTC_E_NONE;
    ds_link_agg_member_num_t linkagg_member;
    ds_link_aggregation_t ds_linkagg;

    if (p_linkagg_master != NULL)
    {
        return CTC_E_NONE;
    }

    SYS_LINKAGG_DEBUG_FUNC();

    /*init soft table*/
    p_linkagg_master =
    (sys_linkagg_master_t *)mem_malloc(MEM_LINKAGG_MODULE, sizeof(sys_linkagg_master_t));

    if (NULL == p_linkagg_master)
    {
        return CTC_E_NO_MEMORY;
    }

    ret = kal_mutex_create(&(p_linkagg_master->p_linkagg_mutex));
    if (ret || !(p_linkagg_master->p_linkagg_mutex))
    {
        SYS_LINKAGG_DEBUG_INFO("Create linkagg mutex fail!\n");
        mem_free(p_linkagg_master);
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    p_linkagg_master->p_linkagg_vector =
    ctc_vector_init(SYS_LINKAGG_BLOCK_NUM, CTC_MAX_LINKAGG_GROUP_NUM / SYS_LINKAGG_BLOCK_NUM);
    if (NULL == p_linkagg_master->p_linkagg_vector)
    {
        SYS_LINKAGG_DEBUG_INFO("Allocate vector for linkagg fail!\n");
        mem_free(p_linkagg_master);
        kal_mutex_destroy(p_linkagg_master->p_linkagg_mutex);
        return CTC_E_NO_MEMORY;
    }

    ctc_vector_reserve(p_linkagg_master->p_linkagg_vector, 1);

    /*init asic table*/
    kal_memset(&linkagg_member, 0, sizeof(ds_link_agg_member_num_t));
    linkagg_member.hash_mode = 1;
    kal_memset(&ds_linkagg, 0, sizeof(ds_link_aggregation_t));
    ds_linkagg.dest_queue = SYS_RESERVED_INTERNAL_PORT_FOR_DROP;

    cmd_mem= DRV_IOW(IOC_TABLE, DS_LINK_AGG_MEMBER_NUM, DRV_ENTRY_FLAG);
    cmd_linkagg = DRV_IOW(IOC_TABLE, DS_LINK_AGGREGATION, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();

    mem_idx = CTC_MAX_LINKAGG_GROUP_NUM * CTC_MAX_LINKAGG_MEMBER_PORT;
    for (chip = 0; chip < lchip_num; chip++)
    {
        sys_humber_get_gchip_id(chip, &gchip);
        ds_linkagg.dest_chip_id = gchip;

        for (agg_idx = 0; agg_idx < CTC_MAX_LINKAGG_GROUP_NUM; agg_idx++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, agg_idx, cmd_mem, &linkagg_member));
        }

        for (index = 0; index < mem_idx; index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(chip, index, cmd_linkagg, &ds_linkagg));
        }
    }

    return CTC_E_NONE;
}

/**
 @brief The function is to create one linkagg
*/
int32
sys_humber_linkagg_create(uint8 tid)
{
    uint8 mem_idx;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();

    /*sanity check*/
    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_TID_VALID_CHECK(tid);

    /*do create*/
    LINKAGG_LOCK;
    p_linkagg = ctc_vector_get(p_linkagg_master->p_linkagg_vector, tid);

    if (NULL != p_linkagg)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg has already exist!\n");
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_LINKAGG_HAS_EXIST, p_linkagg_master->p_linkagg_mutex);
    }

    p_linkagg = (sys_linkagg_t *)mem_malloc(MEM_LINKAGG_MODULE, sizeof(sys_linkagg_t));

    if (NULL == p_linkagg)
    {
        SYS_LINKAGG_DEBUG_INFO("No memory to create new linkagg group!\n");
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, p_linkagg_master->p_linkagg_mutex);
    }

    p_linkagg->tid = tid;
    p_linkagg->port_cnt = 0;

    for (mem_idx = 0; mem_idx < CTC_MAX_LINKAGG_MEMBER_PORT; mem_idx++)
    {
        p_linkagg->port[mem_idx].valid = FALSE;
        p_linkagg->port[mem_idx].gport
        = CTC_MAP_LPORT_TO_GPORT(CTC_INVALID_CHIPID, SYS_RESERVED_INTERNAL_PORT_FOR_DROP);
    }

    if (FALSE == ctc_vector_add(p_linkagg_master->p_linkagg_vector, tid, (void *)p_linkagg))
    {
        SYS_LINKAGG_DEBUG_INFO("Add linkagg to vector error, create linkagg fail!\n");
        mem_free(p_linkagg);
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_UNEXPECT, p_linkagg_master->p_linkagg_mutex);
    }
    LINKAGG_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief The function is to delete one linkagg
*/
int32
sys_humber_linkagg_remove(uint8 tid)
{
    int32 ret = CTC_E_NONE;
    uint8 chip = 0;
    uint8 lchip_num = 0;
    uint8 gchip = 0;
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 agg_idx = 0;
    uint32 agg_base = 0;
    uint32 member_num = 0;
    uint32 cmd_mem = 0;
    uint32 cmd_agg = 0;
    ds_link_aggregation_t ds_linkagg;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();
    /*sanity check*/
    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_TID_VALID_CHECK(tid);

    /*do remove*/
    LINKAGG_LOCK;
    p_linkagg = ctc_vector_del(p_linkagg_master->p_linkagg_vector, tid);

    if (NULL == p_linkagg)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg Not Exist!\n");
        ret = CTC_E_LINKAGG_NOT_EXIST;
        goto OUT;
    }

    for (member_num = 0; member_num < p_linkagg->port_cnt; member_num++)
    {
        gchip = SYS_MAP_GPORT_TO_GCHIP(p_linkagg->port[member_num].gport);
        lport = CTC_MAP_GPORT_TO_LPORT(p_linkagg->port[member_num].gport);

        if (FALSE == sys_humber_chip_is_local(gchip, &lchip))
        {
            continue;
        }

        ret = sys_humber_port_set_global_port(lchip, lport, CTC_MAP_LPORT_TO_GPORT(gchip, lport));
        if (ret < 0)
        {
            ret = CTC_E_UNEXPECT;
            goto OUT;
        }
    }

    if (NULL != p_linkagg)
    {
        mem_free(p_linkagg);
    }

    /*clear asic member port num to zero*/
    member_num = 0;
    cmd_mem = DRV_IOW(IOC_TABLE, DS_LINK_AGG_MEMBER_NUM, DS_LINK_AGG_MEMBER_NUM_LINK_AGG_MEM_NUM);
    cmd_agg = DRV_IOW(IOC_TABLE, DS_LINK_AGGREGATION, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    agg_base = tid * CTC_MAX_LINKAGG_MEMBER_PORT;

    for (chip = 0; chip < lchip_num; chip++)
    {
        sys_humber_get_gchip_id(chip, &gchip);

        ds_linkagg.dest_chip_id = gchip;
        ds_linkagg.dest_queue = SYS_RESERVED_INTERNAL_PORT_FOR_DROP;

        if ((ret = drv_tbl_ioctl(chip, tid, cmd_mem, &member_num)) < 0)
        {
            ret = CTC_E_HW_OP_FAIL;
            goto OUT;
        }

        for (agg_idx = 0; agg_idx < CTC_MAX_LINKAGG_MEMBER_PORT; agg_idx++)
        {
            if ((ret = drv_tbl_ioctl(chip, (agg_base+agg_idx), cmd_agg, &ds_linkagg)) < 0)
            {
                ret = CTC_E_HW_OP_FAIL;
                goto OUT;
            }
        }
    }

    OUT:
        LINKAGG_UNLOCK;
        return ret;
}

/**
 @brief The function is to add a port to linkagg
*/
int32
sys_humber_linkagg_add_port(uint8 tid, uint16 gport)
{
    uint8 index = 0;
    uint8 gchip = 0;
    int32 ret = CTC_E_NONE;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();

    /*Sanity check*/
    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }
    SYS_TID_VALID_CHECK(tid);

    CTC_GLOBAL_PORT_CHECK(gport);
    gchip = SYS_MAP_GPORT_TO_GCHIP(gport);
    CTC_GLOBAL_CHIPID_CHECK(gchip);

    /*do add*/
    LINKAGG_LOCK;
    p_linkagg = ctc_vector_get(p_linkagg_master->p_linkagg_vector, tid);

    if(NULL == p_linkagg)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg Not Exist!\n");
        ret = CTC_E_LINKAGG_NOT_EXIST;
        goto OUT;
    }


    if(CTC_MAX_LINKAGG_MEMBER_PORT == p_linkagg->port_cnt)
    {
        SYS_LINKAGG_DEBUG_INFO("The member of linkagg group reach Max, add member port fail!\n");
        ret = CTC_E_EXCEED_MAX_SIZE;
        goto OUT;
    }

    /*get the first unused pos*/
    if ((ret = _sys_humber_linkagg_get_fisrt_unused_pos(p_linkagg, &index)) < 0)
    {
        goto OUT;
    }

    p_linkagg->port[index].gport = gport;
    p_linkagg->port[index].valid = 1;
    (p_linkagg->port_cnt)++;

    /*write asic table*/
    if ((ret = _sys_humber_linkagg_update_table(p_linkagg, TRUE, index)) < 0)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg update asic table fail!\n");
        goto OUT;
    }


    OUT:
        LINKAGG_UNLOCK;
        return ret;
}

/**
 @brief The function is to remove the port from linkagg
*/
int32
sys_humber_linkagg_remove_port(uint8 tid, uint16 gport)
{
    int32 ret = CTC_E_NONE;
    uint8 index= 0;
    uint8 tail_idx = 0;
    uint8 gchip = 0;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();

    /*Sanity check*/
    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_TID_VALID_CHECK(tid);

    CTC_GLOBAL_PORT_CHECK(gport);
    gchip = SYS_MAP_GPORT_TO_GCHIP(gport);
    CTC_GLOBAL_CHIPID_CHECK(gchip);

    LINKAGG_LOCK;
    p_linkagg = ctc_vector_get(p_linkagg_master->p_linkagg_vector, tid);

    if(NULL == p_linkagg)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg Not Exist!\n");
        ret = CTC_E_LINKAGG_NOT_EXIST;
        goto OUT;
    }

    if (0 == p_linkagg->port_cnt)
    {
        SYS_LINKAGG_DEBUG_INFO("The member num of linkagg is zero, remove fail!\n");
        ret = CTC_E_MEMBER_PORT_NOT_EXIST;
        goto OUT;
    }

    /*check if port is a member of linkagg*/
    if(FALSE == _sys_humber_linkagg_port_is_member(p_linkagg, gport, &index))
    {
        SYS_LINKAGG_DEBUG_INFO("The port is not exist in linkagg group, remove fail!\n");
        ret = CTC_E_MEMBER_PORT_NOT_EXIST;
        goto OUT;
    }

    tail_idx = p_linkagg->port_cnt - 1;
    kal_memmove(&(p_linkagg->port[index]), &(p_linkagg->port[tail_idx]), sizeof(sys_linkagg_port_t));

    p_linkagg->port[tail_idx].valid = 0;
    p_linkagg->port[tail_idx].gport
    = CTC_MAP_LPORT_TO_GPORT(CTC_INVALID_CHIPID, SYS_RESERVED_INTERNAL_PORT_FOR_DROP);
    (p_linkagg->port_cnt)--;

    /*write asic table*/
    if ((ret = _sys_humber_linkagg_update_table(p_linkagg, FALSE, index)) < 0)
    {
        SYS_LINKAGG_DEBUG_INFO("Linkagg update asic table fail!\n");
        goto OUT;
    }

    OUT:
        LINKAGG_UNLOCK;
        return ret;
}

/**
 @brief The function is to get the first local member port of linkagg
*/
int32
sys_humber_linkagg_get_1st_local_port(uint8 tid, uint16 *p_gport, uint8 *local_cnt)
{
    int32 index = 0;
    uint8 lchip_id = 0;
    uint8 gchip_id = 0;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();

    /*Sanity check*/
    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_gport);
    CTC_PTR_VALID_CHECK(local_cnt);
    SYS_TID_VALID_CHECK(tid);

    /*do it*/
    LINKAGG_LOCK;
    p_linkagg = ctc_vector_get(p_linkagg_master->p_linkagg_vector, tid);
	*local_cnt = 0;

    if (NULL == p_linkagg)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_LINKAGG_NOT_EXIST, p_linkagg_master->p_linkagg_mutex);
    }

    for(index = 0; index < (p_linkagg->port_cnt); index++)
    {
        gchip_id = SYS_MAP_GPORT_TO_GCHIP(p_linkagg->port[index].gport);

        if(TRUE == sys_humber_chip_is_local(gchip_id, &lchip_id))
        {
            *p_gport = (p_linkagg->port[index].gport);
	     (*local_cnt) ++;
        }
    }
    if(*local_cnt > 0)
    {
        LINKAGG_UNLOCK;
        return CTC_E_NONE;
    }

    LINKAGG_UNLOCK;
    return CTC_E_LOCAL_PORT_NOT_EXIST;
}

/**
 @brief The function is to show member ports of linkagg.
*/
int32
sys_humber_linkagg_show_ports(uint8 tid, uint16 *p_gports, uint8 *cnt)
{
    uint8 idx = 0;
    sys_linkagg_t *p_linkagg = NULL;

    SYS_LINKAGG_DEBUG_FUNC();

    if (NULL == p_linkagg_master)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_TID_VALID_CHECK(tid);

    p_linkagg = ctc_vector_get(p_linkagg_master->p_linkagg_vector, tid);
    if (NULL == p_linkagg)
    {
        return CTC_E_LINKAGG_NOT_EXIST;
    }

    for (idx = 0; idx < p_linkagg->port_cnt; idx++)
    {
        p_gports[idx] = p_linkagg->port[idx].gport;
    }

    *cnt = p_linkagg->port_cnt;

    return CTC_E_NONE;
}



