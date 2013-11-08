/**
 @file sys_humber_opf.c

 @date 2009-10-22

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/

#include "kal.h"
#include "ctc_const.h"
#include "ctc_error.h"
#include "sys_humber_opf.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

struct sys_offset_node_s
{
    ctc_slistnode_t head;
    uint32 start;
    uint32 end;
};
typedef struct sys_offset_node_s sys_offset_node_t;

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/

sys_humber_opf_master_t *p_opf_master = NULL;
/****************************************************************************
 *
* Function prototype
*
*****************************************************************************/
static int32
_sys_humber_opf_pre_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset);
static int32
_sys_humber_opf_reverse_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset);

/****************************************************************************
 *
* Function
*
*****************************************************************************/

int32
sys_humber_opf_init(enum sys_humber_opf_type opf_type, uint8 pool_num)
{
    uint8 type_index = 0;

    if (NULL == p_opf_master)
    {
        p_opf_master = (sys_humber_opf_master_t *)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_master_t));
        CTC_PTR_VALID_CHECK(p_opf_master);

        p_opf_master->ppp_opf_pre = (sys_humber_opf_entry_t ***)mem_malloc(MEM_OPF_MODULE, MAX_OPF_TBL_NUM * sizeof(void*));
        CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_pre);
        kal_memset(p_opf_master->ppp_opf_pre, 0, MAX_OPF_TBL_NUM * sizeof(void*));


        p_opf_master->ppp_opf_rev = (sys_humber_opf_entry_t ***)mem_malloc(MEM_OPF_MODULE, MAX_OPF_TBL_NUM * sizeof(void*));
        CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_rev);
        kal_memset(p_opf_master->ppp_opf_rev, 0, MAX_OPF_TBL_NUM * sizeof(void*));

    }

    type_index = opf_type;

    if (type_index >= MAX_OPF_TBL_NUM )
    {

        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (NULL != p_opf_master->ppp_opf_pre[type_index] || NULL != p_opf_master->ppp_opf_rev[type_index])
    {
        return CTC_E_NONE;
    }


    p_opf_master->ppp_opf_pre[type_index] = (sys_humber_opf_entry_t **)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(void*));
    CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_pre[type_index]);
    kal_memset(p_opf_master->ppp_opf_pre[type_index], 0, pool_num * sizeof(void*));

    p_opf_master->ppp_opf_rev[type_index] = (sys_humber_opf_entry_t **)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(void*));
    CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_rev[type_index]);
    kal_memset(p_opf_master->ppp_opf_rev[type_index], 0, pool_num * sizeof(void*));

    p_opf_master->start_offset_a[type_index] = (uint32 *)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(uint32));
    CTC_PTR_VALID_CHECK(p_opf_master->start_offset_a[type_index]);

    p_opf_master->max_size_a[type_index] = (uint32 *)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(uint32));
    CTC_PTR_VALID_CHECK(p_opf_master->max_size_a[type_index]);

    p_opf_master->max_offset_for_pre_alloc[type_index] = (uint32 *)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(uint32));
    CTC_PTR_VALID_CHECK(p_opf_master->max_offset_for_pre_alloc[type_index]);

    p_opf_master->min_offset_for_rev_alloc[type_index] = (uint32 *)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(uint32));
    CTC_PTR_VALID_CHECK(p_opf_master->max_offset_for_pre_alloc[type_index]);

    p_opf_master->is_reserve[type_index] = (uint8 *)mem_malloc(MEM_OPF_MODULE, pool_num * sizeof(uint8));
    CTC_PTR_VALID_CHECK(p_opf_master->is_reserve[type_index]);


    p_opf_master->max_tbl_num[type_index] = pool_num;

	return CTC_E_NONE;
}

int32
sys_humber_opf_init_offset( sys_humber_opf_t *opf , uint32 start_offset,uint32 max_size)
{
    uint8 type_index = 0;
    uint8 pool_index = 0;

    CTC_PTR_VALID_CHECK(opf);
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[opf->pool_type])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (NULL != p_opf_master->ppp_opf_pre[type_index][pool_index])
    {
        return CTC_E_NONE;
    }

    p_opf_master->ppp_opf_pre[type_index][pool_index] = (sys_humber_opf_entry_t *)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
    CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_pre[type_index][pool_index]);

    p_opf_master->ppp_opf_rev[type_index][pool_index] = (sys_humber_opf_entry_t *)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
    CTC_PTR_VALID_CHECK(p_opf_master->ppp_opf_rev[type_index][pool_index]);


    p_opf_master->start_offset_a[type_index][pool_index] = start_offset;
    p_opf_master->max_size_a[type_index][pool_index] = max_size;

    p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = start_offset;

    p_opf_master->min_offset_for_rev_alloc[type_index][pool_index] =  start_offset + max_size;
    p_opf_master->is_reserve[type_index][pool_index] = FALSE;

    p_opf_master->ppp_opf_pre[type_index][pool_index]->next = NULL;
    p_opf_master->ppp_opf_pre[type_index][pool_index]->prev = NULL;
    p_opf_master->ppp_opf_pre[type_index][pool_index]->offset = start_offset;
    p_opf_master->ppp_opf_pre[type_index][pool_index]->size = max_size;

    p_opf_master->ppp_opf_rev[type_index][pool_index]->next = NULL;
    p_opf_master->ppp_opf_rev[type_index][pool_index]->prev = NULL;
    p_opf_master->ppp_opf_rev[type_index][pool_index]->offset = start_offset;
    p_opf_master->ppp_opf_rev[type_index][pool_index]->size = max_size;

    return CTC_E_NONE;
}

int32
sys_humber_opf_reserve_size_for_reverse_alloc(sys_humber_opf_t *opf,uint32 block_size)
{
   uint8 type_index = 0;
    uint8 pool_index = 0;
    uint32 start_offset = 0;
    uint32 max_size = 0;


    CTC_PTR_VALID_CHECK(opf);
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }
    if (block_size != 0)
    {
        start_offset = p_opf_master->start_offset_a[type_index][pool_index];
        max_size = p_opf_master->max_size_a[type_index][pool_index];

        p_opf_master->is_reserve[type_index][pool_index] = TRUE;
        p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = start_offset + max_size - block_size;
        p_opf_master->min_offset_for_rev_alloc[type_index][pool_index] = start_offset + max_size - block_size;
    }
   return CTC_E_NONE;
}

int32
sys_humber_opf_reverse_alloc_offset(sys_humber_opf_t *opf,uint32 block_size,uint32*offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 end_offset = 0;
    uint32 revise_offset = 0;
    uint32 max_size = 0;
    uint32 min_offset = 0;
    sys_humber_opf_entry_t *entry, *next;

    CTC_PTR_VALID_CHECK(opf);
    *offset = CTC_MAX_UINT32_VALUE;
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }
    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;


    entry = p_opf_master->ppp_opf_rev[type_index][pool_index];

    if (block_size == 0)
    {
        block_size = 1;
    }

    while (entry)
    {
        revise_offset = (end_offset - entry->offset - block_size) + start_offset;
        if (entry->size >= block_size
           && (revise_offset >= p_opf_master->max_offset_for_pre_alloc[type_index][pool_index]))
        {
            break;
        }
        else
        {
            entry = entry->next;
        }
    }

    if (!entry)
    {
        SYS_OPF_DBG_INFO(
                        "type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);

        return CTC_E_NO_ALLOC_OFFSET;
    }

    *offset = revise_offset;

    if (entry->size > block_size)
    {
        entry->size -= block_size;
        entry->offset += block_size;
        min_offset = entry->offset;
    }
    else
    {
        /* entry->size == block_size */
        entry->size -= block_size;
        entry->offset += block_size;
        min_offset = entry->offset;
        next = entry->next;

        if (entry->prev && next)
        {
            entry->prev->next = next;
            next->prev = entry->prev;

            mem_free(entry);
        }
        else if (next)
        {
            /* entry->prev == NULL && next != NULL */
            p_opf_master->ppp_opf_rev[type_index][pool_index] = next;
            next->prev = entry->prev;

            mem_free(entry);
        }
        /* next == NULL in this condition this node cannot be free */
    }

    if (*offset <  p_opf_master->min_offset_for_rev_alloc[type_index][pool_index])
    {
        p_opf_master->min_offset_for_rev_alloc[type_index][pool_index] = *offset;
    }

	return CTC_E_NONE;
}

static int32
_sys_humber_opf_reverse_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 end_offset = 0;
    uint32 revise_offset = 0;
    uint32 max_size = 0;
    uint32 prev_offset = 0;
    sys_humber_opf_entry_t *entry, *next, *prev, *node;

    CTC_PTR_VALID_CHECK(opf);

    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;

    if (block_size > max_size)
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }
    /*revise offset for pre-allocation freelist*/
    revise_offset = end_offset - offset - block_size + start_offset;

    if (revise_offset < start_offset || revise_offset > end_offset )
    {
        SYS_OPF_DBG_INFO( "invalid offset: %d\n", revise_offset);

        return CTC_E_INVALID_PARAM;
    }
    entry = p_opf_master->ppp_opf_rev[type_index][pool_index];

    while (entry != NULL)
    {
        if (revise_offset + block_size > entry->offset)
        {
            prev_offset = entry->offset + entry->size;
            entry = entry->next;
        }
        else if (revise_offset + block_size == entry->offset)
        {
            /*change offset and block size */
            entry->offset = revise_offset;
            entry->size += block_size;
            break;
        }
        else
        {
            if (revise_offset < prev_offset)
            {
                /* an error condition */
                entry = NULL;
                break;
            }
            /* insert a new free node */
            node = (sys_humber_opf_entry_t*)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));

            CTC_PTR_VALID_CHECK(node);

            node->offset = revise_offset;
            node->size = block_size;
            prev = entry->prev;

            if (prev)
            {
                prev->next = node;
            }
            else
            {
                /* if the enry is the first node, we must update global database */
                p_opf_master->ppp_opf_rev[type_index][pool_index] = node;
            }
            node->next = entry;
            node->prev = prev;
            entry->prev = node;
            entry = node;
            break;
        }
    }

    if (entry != NULL)
    {
        prev = entry->prev;
        next = entry->next;

        if (prev && (prev_offset == entry->offset))
        {
            prev->next = next;

            if (next)
            {
                next->prev = prev;
            }
            prev->size += entry->size;
            mem_free(entry);

            entry = prev;
        }

        if ((!entry->next) && (!p_opf_master->is_reserve[type_index][pool_index]))
        {
            p_opf_master->min_offset_for_rev_alloc[type_index][pool_index] = end_offset - entry->offset + start_offset;
        }
    }
    else
    {
        SYS_OPF_DBG_INFO("invalid offset: %d invalid block_size:%d\n",
                          revise_offset, block_size);
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_opf_alloc_offset(sys_humber_opf_t *opf,uint32 block_size,uint32*offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 max_size = 0;
    uint32 end_offset = 0;
    uint32 revise_offset =0;

    sys_humber_opf_entry_t *entry, *next;

    CTC_PTR_VALID_CHECK(opf);
    *offset = CTC_MAX_UINT32_VALUE;
    type_index = opf->pool_type;
    pool_index = opf->pool_index;


    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (block_size == 0)
    {
        block_size = 1;
    }

    if (block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }

    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];

    while (entry)
    {
        revise_offset = entry->offset + block_size -1 ;

        if (entry->size >= block_size
           && (revise_offset <  p_opf_master->min_offset_for_rev_alloc[type_index][pool_index]))
        {
            break;
        }
        else
        {
            entry = entry->next;
        }
    }

    if (!entry)
    {
        SYS_OPF_DBG_INFO(
                        "type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);

        return CTC_E_NO_ALLOC_OFFSET;
    }

    *offset = entry->offset;

    entry->size -= block_size;
    if(entry->size == 0)
    {
        next = entry->next;
        if(entry->prev)
        {
            entry->prev->next = next;
        }
        else
        {
            p_opf_master->ppp_opf_pre[type_index][pool_index] = next;
        }
        if(next)
        {
            next->prev = entry->prev;
        }
        mem_free(entry);
    }
    else
    {
        entry->offset += block_size;
    }

    if ((*offset + block_size) > p_opf_master->max_offset_for_pre_alloc[type_index][pool_index])
    {
        p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = *offset + block_size;
    }

	return CTC_E_NONE;
}

int32
sys_humber_opf_alloc_offset_last(sys_humber_opf_t *opf, uint32 block_size, uint32*offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 max_size = 0;
    uint32 end_offset = 0;
    uint32 revise_offset =0;

    sys_humber_opf_entry_t *entry, *next, *select = NULL;

    CTC_PTR_VALID_CHECK(opf);
    *offset = CTC_MAX_UINT32_VALUE;
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;

    if(type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO("invalid type index : %d\n", type_index);
        return CTC_E_INVALID_PARAM;
    }

    if(pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO("invalid pool index : %d\n", pool_index);
        return CTC_E_INVALID_PARAM;
    }

    if(block_size == 0)
    {
        block_size = 1;
    }

    if(block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO("invalid block_size:%d\n", block_size);
        return CTC_E_INVALID_PARAM;
    }

    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];

    while(entry)
    {
        revise_offset = (end_offset - entry->offset - block_size) + start_offset;

        if (entry->size >= block_size &&
            revise_offset < p_opf_master->min_offset_for_rev_alloc[type_index][pool_index])
        {
            select = entry;
        }
        entry = entry->next;
    }

    if(!select)
    {
        SYS_OPF_DBG_INFO("type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);
        return CTC_E_NO_ALLOC_OFFSET;
    }

    *offset = select->offset + select->size - block_size;

    select->size -= block_size;
    if(select->size == 0)
    {
        next = select->next;
        if(select->prev)
        {
            select->prev->next = next;
        }
        else
        {
            p_opf_master->ppp_opf_pre[type_index][pool_index] = next;
        }
        if(next)
        {
            next->prev = select->prev;
        }
        mem_free(select);
    }

    if((*offset + block_size) > p_opf_master->max_offset_for_pre_alloc[type_index][pool_index])
    {
        p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = *offset + block_size;
    }

	return CTC_E_NONE;
}


int32
sys_humber_opf_alloc_offset_from_position(sys_humber_opf_t *opf, uint32 block_size, uint32 begin)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 max_size = 0;
    uint32 end_offset = 0;
    int32 revise_offset =0;
    uint32 end;

    sys_humber_opf_entry_t *entry, *next, *node;

    CTC_PTR_VALID_CHECK(opf);
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;

    if(type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO("invalid type index : %d\n", type_index);
        return CTC_E_INVALID_PARAM;
    }

    if(pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO("invalid pool index : %d\n", pool_index);
        return CTC_E_INVALID_PARAM;
    }

    if(block_size == 0)
    {
        block_size = 1;
    }

    if(block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO("invalid block_size:%d\n", block_size);
        return CTC_E_INVALID_PARAM;
    }

    end = begin + block_size - 1;

    revise_offset = end_offset - begin - block_size + start_offset;
    if( (revise_offset > p_opf_master->min_offset_for_rev_alloc[type_index][pool_index]))
    {
        return CTC_E_NO_OFFSET_LEFT;
    }

    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];
    while(entry)
    {
        if(begin >= entry->offset + entry->size)
        {
            entry = entry->next;
        }
        else
        {
            if(begin < entry->offset || entry->offset + entry->size < begin + block_size)
            {
                entry = NULL;
            }
            break;
        }
    }

    if(!entry)
    {
        SYS_OPF_DBG_INFO("type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);
        return CTC_E_NO_ALLOC_OFFSET;
    }

    if(entry->offset == begin || entry->offset + entry->size - 1 == end)
    {
        entry->size -= block_size;
        if(entry->size == 0)
        {
            next = entry->next;
            if(entry->prev)
            {
                entry->prev->next = next;
            }
            else
            {
                p_opf_master->ppp_opf_pre[type_index][pool_index] = next;
            }
            if(next)
            {
                next->prev = entry->prev;
            }
            mem_free(entry);
        }
        else if(entry->offset == begin)
        {
            entry->offset = end + 1;
        }
    }
    else
    {
        /* insert a new free node */
        node = (sys_humber_opf_entry_t*)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
        CTC_PTR_VALID_CHECK(node);

        node->offset = end + 1;
        node->size = entry->offset + entry->size - end - 1;
        node->prev = entry;
        node->next = entry->next;

        entry->size = begin - entry->offset;
        entry->next = node;
    }

    if(end + 1 > p_opf_master->max_offset_for_pre_alloc[type_index][pool_index])
    {
        p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = end + 1;
    }

	return CTC_E_NONE;
}

/*alloc multiple offset*/
extern int32
sys_humber_opf_alloc_multiple_offset(sys_humber_opf_t *opf,uint8 multiple,uint32 block_size,uint32*offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 remainder = 0;
    uint32 skip_value = 0;
    uint32 start_offset = 0;
    uint32 end_offset = 0;
    uint32 revise_offset =0;
    uint32 max_size = 0;
    sys_humber_opf_entry_t *entry, *next;
    sys_humber_opf_entry_t  *node;

    CTC_PTR_VALID_CHECK(opf);
    *offset = CTC_MAX_UINT32_VALUE;
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];
    end_offset   = start_offset + max_size;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (block_size == 0)
    {
        block_size = 1;
    }

    if (block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }

    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];

    while (entry)
    {
        revise_offset = entry->offset + block_size -1 ;

        if (entry->size >= block_size
            && (revise_offset <=  p_opf_master->min_offset_for_rev_alloc[type_index][pool_index]))
        {
            remainder = entry->offset % multiple;

            skip_value = remainder ? (multiple - remainder): remainder;

            if (entry->size >= (skip_value + block_size))
            {
                if (remainder)
                {
                    /*insert a node, allocate offset from first multiple offset*/
                    node = (sys_humber_opf_entry_t*) mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
                    kal_memset(node, 0, sizeof(sys_humber_opf_entry_t));
                    node->size = entry->size - skip_value;
                    node->offset = entry->offset + skip_value;
                    node->prev = entry;
                    node->next = entry->next;

                    entry->size = skip_value;
                    entry->next = node;
                    entry = node;
                }
                break;
            }
        }
        entry = entry->next;
    }

    if (!entry)
    {
        SYS_OPF_DBG_INFO(
                        "type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);

        return CTC_E_NO_ALLOC_OFFSET;
    }


    *offset = entry->offset;

    entry->size -= block_size;
    if(entry->size == 0)
    {
        next = entry->next;
        if(entry->prev)
        {
            entry->prev->next = next;
        }
        else
        {
            p_opf_master->ppp_opf_pre[type_index][pool_index] = next;
        }
        if(next)
        {
            next->prev = entry->prev;
        }
        mem_free(entry);
    }
    else
    {
        entry->offset += block_size;
    }

    if ((*offset + block_size) > p_opf_master->max_offset_for_pre_alloc[type_index][pool_index])
    {
        p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = *offset +block_size;
    }

    return CTC_E_NONE;
}


extern int32
sys_humber_opf_reverse_alloc_multiple_offset(sys_humber_opf_t *opf,uint8 multiple,uint32 block_size,uint32*offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset = 0;
    uint32 end_offset = 0;
    uint32 revise_offset = 0;
    uint32 max_size = 0;
    uint32 remainder = 0;
    uint32 skip_value = 0;
    uint32 min_offset = 0;
    sys_humber_opf_entry_t *entry, *next;
    sys_humber_opf_entry_t  *node;

    CTC_PTR_VALID_CHECK(opf);
    *offset = CTC_MAX_UINT32_VALUE;
    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n", pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (block_size == 0)
    {
        block_size = 1;
    }

    if (block_size > p_opf_master->max_size_a[type_index][pool_index])
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }
    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size = p_opf_master->max_size_a[type_index][pool_index];
    end_offset = start_offset + max_size;


    entry = p_opf_master->ppp_opf_rev[type_index][pool_index];

    while (entry)
    {
        revise_offset = (end_offset - entry->offset - block_size) + start_offset;
        if (entry->size >= block_size
           && (revise_offset >= p_opf_master->max_offset_for_pre_alloc[type_index][pool_index]))
        {
            remainder = revise_offset % multiple;
            skip_value = multiple - remainder;

            if (entry->size >= (remainder ? (skip_value + block_size) : block_size))
            {
                if (remainder)
                {
                    /*insert a node, allocate offset from first multiple offset*/
                    node = (sys_humber_opf_entry_t*) mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
                    kal_memset(node, 0, sizeof(sys_humber_opf_entry_t));
                    node->size = entry->size - skip_value;
                    node->offset = entry->offset + skip_value;
                    node->prev = entry;
                    node->next = entry->next;

                    entry->size = skip_value;
                    entry->next = node;
                    entry = node;
                }
                break;
            }
        }
            entry = entry->next;
    }


    if (!entry)
    {
        SYS_OPF_DBG_INFO(
                        "type_index=%d pool_index=%d This pool don't have enough memory!\n", type_index, pool_index);

        return CTC_E_NO_ALLOC_OFFSET;
    }
    revise_offset = (end_offset - entry->offset - block_size) + start_offset;

    if (entry->size > block_size)
    {
        entry->size -= block_size;
        entry->offset += block_size;
    }
    else
    {
        /* entry->size == block_size */
        entry->size -= block_size;
        entry->offset += block_size;
        min_offset = entry->offset;
        next = entry->next;

        if (entry->prev && next)
        {
            entry->prev->next = next;
            next->prev = entry->prev;

            mem_free(entry);
        }
        else if (next)
        {
            /* entry->prev == NULL && next != NULL */
            p_opf_master->ppp_opf_rev[type_index][pool_index] = next;
            next->prev = entry->prev;

            mem_free(entry);
        }
        /* next == NULL in this condition this node cannot be free */
    }

    *offset = revise_offset ;

    if (*offset <  p_opf_master->min_offset_for_rev_alloc[type_index][pool_index])
    {
        p_opf_master->min_offset_for_rev_alloc[type_index][pool_index] = *offset;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_opf_pre_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;
    uint32 start_offset;
    uint32 max_size = 0;
    sys_humber_opf_entry_t *entry, *prev = NULL, *next = NULL, *node;
    uint8  is_merged = 0;

    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    start_offset = p_opf_master->start_offset_a[type_index][pool_index];
    max_size     = p_opf_master->max_size_a[type_index][pool_index];

    if (block_size == 0)
    {
        block_size = 1;
    }

    if (block_size > max_size)
    {
        SYS_OPF_DBG_INFO( "invalid block_size:%d\n", block_size);

        return CTC_E_INVALID_PARAM;
    }

    if (offset < start_offset || offset + block_size > (start_offset + max_size))
    {
        SYS_OPF_DBG_INFO( "invalid offset: %d\n", offset);

        return CTC_E_INVALID_PARAM;
    }

    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];
    while(entry != NULL)
    {
        /* |              ..********      | */
        if(offset + block_size == entry->offset)
        {
            /*change offset and block size */
            entry->offset = offset;
            entry->size += block_size;
            is_merged = 1;
            break;
        }
        /* |              ********..      | */
        if(entry->offset + entry->size == offset)
        {
            /*change block size */
            entry->size += block_size;
            is_merged = 1;
            if(entry->next && entry->next->offset == entry->offset + entry->size)
            {
                next = entry->next;
                entry->size += next->size;
                entry->next = next->next;
                if(entry->next)
                {
                    entry->next->prev = entry;
                }
                mem_free(next);
            }
            break;
        }
        /* |            ..  ********      | */
        if(offset + block_size < entry->offset)
        {
            break;
        }
        /* |            ********  ..      | */
        if(offset > entry->offset + entry->size)
        {
            prev = entry;
            entry = entry->next;
            continue;
        }
        /* |            ********          | */
        /* |              ..              | */
        SYS_OPF_DBG_INFO("invalid offset: %d invalid block_size:%d\n",
                          offset, block_size);
        return CTC_E_INVALID_PARAM;
    }

    if(!is_merged)
    {
        node = (sys_humber_opf_entry_t*)mem_malloc(MEM_OPF_MODULE, sizeof(sys_humber_opf_entry_t));
        CTC_PTR_VALID_CHECK(node);

        node->offset = offset;
        node->size = block_size;
        node->prev = prev;
        node->next = entry;

        if(prev)
        {
            prev->next = node;
        }
        else
        {
            p_opf_master->ppp_opf_pre[type_index][pool_index] = node;
        }

        if(entry)
        {
            entry->prev = node;
        }
    }
    else
    {
        if( (p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] <= entry->offset + entry->size)
            && (!p_opf_master->is_reserve[type_index][pool_index]))
        {
            p_opf_master->max_offset_for_pre_alloc[type_index][pool_index] = entry->offset;
        }
    }

    return CTC_E_NONE;
}
int32
sys_humber_opf_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset)
{
    uint8  type_index = 0;
    uint8  pool_index = 0;

    CTC_PTR_VALID_CHECK(opf);

    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        SYS_OPF_DBG_INFO( "invalid type index : %d\n", type_index);

        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        SYS_OPF_DBG_INFO( "invalid pool index : %d\n",  pool_index);

        return CTC_E_INVALID_PARAM;
    }

    if (offset >= p_opf_master->max_offset_for_pre_alloc[type_index][pool_index])
    {
        return  _sys_humber_opf_reverse_free_offset(opf, block_size, offset);
    }
    else
    {
        return  _sys_humber_opf_pre_free_offset(opf, block_size, offset);
    }
}

static int32
sys_humber_opf_print_used_offset_list(ctc_slist_t *opf_used_offset_list,sys_humber_opf_t *opf)
{
    uint8 all_offset_len = 0;
    uint8 row_length = 32;
    char out_offset[11];
    char all_offset_one_row[32];
    sys_offset_node_t *offset_node = NULL;
    ctc_slistnode_t *node = NULL, *next_node = NULL;

    CTC_PTR_VALID_CHECK(opf_used_offset_list);

    SYS_OPF_DBG_INFO( "used offset:\n");
    SYS_OPF_DBG_INFO( "-----------------------------------\n");

    kal_memset(all_offset_one_row, 0, sizeof(all_offset_one_row));
    /* print and free all nodes */
    if (0 == opf_used_offset_list->count)
    {
        if( NULL == p_opf_master->ppp_opf_pre[opf->pool_type][opf->pool_index])
        {
            SYS_OPF_DBG_INFO("all offset used\n");
        }
        else
        {
            SYS_OPF_DBG_INFO("no offset used\n");
        }
    }
    else
    {
         CTC_SLIST_LOOP_DEL(opf_used_offset_list, node, next_node)
        {
            offset_node = _ctc_container_of(node, sys_offset_node_t, head);
            kal_memset( out_offset, 0, sizeof(out_offset) );
            if (offset_node->start != offset_node->end)
            {
                kal_sprintf( out_offset, "%d-%d,", offset_node->start,offset_node->end);
            }
            else
            {
                kal_sprintf( out_offset, "%d,", offset_node->start);
            }

            if((all_offset_len + kal_strlen(out_offset)) >= row_length)
            {
                all_offset_one_row[kal_strlen(all_offset_one_row)-1] = '\0';
                SYS_OPF_DBG_INFO("%s\n", all_offset_one_row);
                kal_memset(all_offset_one_row, 0, sizeof(all_offset_one_row));
                kal_strcat(all_offset_one_row, out_offset);
            }
            else
            {
                kal_strcat(all_offset_one_row, out_offset);
            }
            all_offset_len = kal_strlen(all_offset_one_row);

        }
    }

    all_offset_one_row[kal_strlen(all_offset_one_row)-1] = '\0';
    SYS_OPF_DBG_INFO("%s\n", all_offset_one_row);
    SYS_OPF_DBG_INFO( "-----------------------------------\n");

    return CTC_E_NONE;
}

static int32
sys_humber_opf_free_used_offset_list(ctc_slist_t *opf_used_offset_list)
{
    ctc_slistnode_t *node = NULL, *next_node = NULL;

    CTC_PTR_VALID_CHECK(opf_used_offset_list);

    CTC_SLIST_LOOP_DEL(opf_used_offset_list, node, next_node)
    {
        ctc_slist_delete_node(opf_used_offset_list, node);
        mem_free(node);
    }

    ctc_slist_delete(opf_used_offset_list);
    opf_used_offset_list = NULL;
    node = NULL;
    next_node = NULL;

    return CTC_E_NONE;
}

int32
sys_humber_opf_print_alloc_info(sys_humber_opf_t *opf)
{
    uint8 type_index = 0;
    uint8 pool_index = 0;
    uint32 offset_index = 0;
    uint32 min_offset = 0;
    uint32 max_offset = 0;
    uint32 pre_start = 0;
    uint32 pre_end = 0;
    uint32 start = 0;
    uint32 end = 0 ;

    sys_humber_opf_entry_t* entry;
    sys_offset_node_t* offset_node = NULL;
    ctc_slist_t* offset_slist_pr = NULL;
    ctc_slist_t* offset_slist_bf = NULL;
    ctc_slistnode_t *tmp_head_node = NULL;

    CTC_PTR_VALID_CHECK(opf);

    offset_slist_pr = ctc_slist_new();
    CTC_PTR_VALID_CHECK(offset_slist_pr);

    offset_slist_bf = ctc_slist_new();
    CTC_PTR_VALID_CHECK(offset_slist_bf);

    type_index = opf->pool_type;
    pool_index = opf->pool_index;

      if (type_index >= MAX_OPF_TBL_NUM)
      {
          SYS_OPF_DBG_INFO( "invalid type index: %d \n", type_index);

          return CTC_E_INVALID_PARAM;
      }

      if (pool_index >= p_opf_master->max_tbl_num[type_index])
      {
          SYS_OPF_DBG_INFO( "invalid pool index:%d\n", pool_index);

          return CTC_E_INVALID_PARAM;
      }
     max_offset = p_opf_master->start_offset_a[type_index][pool_index] + p_opf_master->max_size_a[type_index][pool_index];
     min_offset = p_opf_master->start_offset_a[type_index][pool_index];
     SYS_OPF_DBG_INFO( "min offset:%d \n", min_offset);
     SYS_OPF_DBG_INFO( "max offset:%d \n",max_offset);
     SYS_OPF_DBG_INFO( "current allocated max offset for previous allocation scheme:%d \n", p_opf_master->max_offset_for_pre_alloc[type_index][pool_index]);
     SYS_OPF_DBG_INFO( "current allocated min offset for reverse allocation scheme:%d \n", p_opf_master->min_offset_for_rev_alloc[type_index][pool_index]);

     SYS_OPF_DBG_INFO( "previous allocation scheme\n");
     SYS_OPF_DBG_INFO( "%6s    %7s    %4s\n", "index","offset","size");
     SYS_OPF_DBG_INFO( "-----------------------------------\n");

     pre_end = max_offset - 1;

     entry = p_opf_master->ppp_opf_pre[type_index][pool_index];
     /* walk through the entry list */
     while (entry)
     {
        SYS_OPF_DBG_INFO("%6d    %7d    %4d \n", offset_index,entry->offset,entry->size);

        if (min_offset == entry->offset)
        {
            pre_start = entry->size;
        }
        else
        {
            start = pre_start;
            end = entry->offset - 1;

            /* offset_slist_pr add node */
            offset_node = (sys_offset_node_t *)mem_malloc(MEM_OPF_MODULE, sizeof(sys_offset_node_t));
            if (NULL == offset_node)
            {
                return CTC_E_NO_MEMORY;
            }
            offset_node->start = start;
            offset_node->end = end;
            offset_node->head.next = NULL;

            /* different from offset_slist_bf */
            ctc_slist_add_tail(offset_slist_pr, &offset_node->head);
            pre_start = entry->offset + entry->size;
        }

        entry = entry->next;
        offset_index++;
     }
     SYS_OPF_DBG_INFO( "-----------------------------------\n");
     SYS_OPF_DBG_INFO( "\nreverse allocation scheme\n");
     SYS_OPF_DBG_INFO( "%6s    %7s    %4s\n", "index","offset","size");
     SYS_OPF_DBG_INFO( "-----------------------------------\n");
     offset_index = 0;
     entry = p_opf_master->ppp_opf_rev[type_index][pool_index];
     /* walk through the entry list */
     while (entry)
     {
        SYS_OPF_DBG_INFO("%6d    %7d    %4d \n", offset_index, entry->offset, entry->size);

        if (min_offset == entry->offset)
        {
            pre_end = max_offset - entry->size -1;
        }
        else
        {
            start = max_offset - entry->offset;
            end = pre_end;

            /* offset_slist_bf add node */
            offset_node = (sys_offset_node_t *)mem_malloc(MEM_OPF_MODULE, sizeof(sys_offset_node_t));
            if (NULL == offset_node)
            {
                return CTC_E_NO_MEMORY;
            }
            offset_node->start = start;
            offset_node->end = end;
            offset_node->head.next = NULL;
            ctc_slist_add_head(offset_slist_bf, &offset_node->head);

            pre_end = max_offset - entry->offset - entry->size -1;
        }

        entry = entry->next;
        offset_index++;
     }
     SYS_OPF_DBG_INFO( "-----------------------------------\n");

    /* merge offset_slist_pr and offset_slist_bf */
    tmp_head_node = CTC_SLISTHEAD(offset_slist_bf);
    ctc_slist_add_tail(offset_slist_pr, tmp_head_node);

    CTC_ERROR_RETURN(sys_humber_opf_print_used_offset_list(offset_slist_pr,opf));
    CTC_ERROR_RETURN(sys_humber_opf_free_used_offset_list(offset_slist_pr));
    ctc_slist_delete(offset_slist_bf);

    tmp_head_node = NULL;

    return CTC_E_NONE;
}

int32
sys_humber_opf_print_sample_info(sys_humber_opf_t *opf)
{
    uint8 type_index = 0;
    uint8 pool_index = 0;
    sys_humber_opf_entry_t *entry;

    type_index = opf->pool_type;
    pool_index = opf->pool_index;

    if (type_index >= MAX_OPF_TBL_NUM)
    {
        return CTC_E_INVALID_PARAM;
    }

    if (pool_index >= p_opf_master->max_tbl_num[type_index])
    {
        return CTC_E_INVALID_PARAM;
    }
    entry = p_opf_master->ppp_opf_pre[type_index][pool_index];

    /* walk through the entry list */
    while(entry)
    {
        SYS_OPF_DBG_INFO("offset %u    size %u\n", entry->offset, entry->size);
        entry = entry->next;
    }

    return CTC_E_NONE;
}




