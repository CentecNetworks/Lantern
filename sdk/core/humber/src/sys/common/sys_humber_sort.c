/**
 @file sys_humber_sort.c

 @date 2009-12-19

 @version v2.0

 The file contains all sort related function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "sys_humber_chip.h"
#include "sys_humber_sort.h"

#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/



/****************************************************************************
 *
* Function
*
*****************************************************************************/
static int32
_sys_humber_sort_key_do_alloc_offset(sys_sort_key_info_t* key_info, uint32* p_offset)
{
    sys_humber_opf_t opf;
    sys_sort_block_t* block;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    opf.pool_type= key_info->type;
    opf.pool_index = key_info->block_id;

    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, p_offset));

    /* adjust block info */
    CTC_MAX_VALUE_CHECK(block->used_of_num, block->all_of_num);
    ++(block->used_of_num);

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_do_free_offset(sys_sort_key_info_t* key_info, uint32 offset)
{
    sys_sort_block_t* block;
    sys_humber_opf_t opf;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    opf.pool_type= key_info->type;
    opf.pool_index = key_info->block_id;

    /* 2. do it */
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, offset));
    /* adjust block info */
    CTC_NOT_EQUAL_CHECK(0, block->used_of_num);
    --(block->used_of_num);

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_block_has_avil_offset(sys_sort_key_info_t* key_info, bool* p_has)
{
    sys_sort_block_t* block = NULL;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    CTC_MAX_VALUE_CHECK(block->used_of_num, block->all_of_num);

    /* 2. do it */
    if(block->all_of_num == block->used_of_num)
    {
        *p_has = FALSE;
    }
    else
    {
        *p_has = TRUE;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_adj_block_id(sys_sort_key_info_t* key_info, sys_sort_block_dir_t dir, uint32* adj_block_id)
{
    int32 i;
    sys_sort_block_t* block;

    block = key_info->block;

    if(SYS_SORT_BLOCK_DIR_UP == dir)
    {
        for(i = key_info->block_id - 1; i >= 0; i--)
        {
            if(FALSE == block[i].is_block_can_shrink)
            {
                return CTC_E_NO_RESOURCE;
            }
            CTC_MAX_VALUE_CHECK(block[i].used_of_num, block[i].all_of_num);

            if((block[i].all_of_num - block[i].used_of_num == 1) && (block[i].used_of_num == 0))
            {
                continue;
            }
            if(block[i].all_of_num - block[i].used_of_num >= 1)
            {
                *adj_block_id = i;
                break;
            }
        }
        if(i < 0)
        {
            return CTC_E_NO_RESOURCE;
        }
    }
    else
    {
        for(i = key_info->block_id + 1; i < key_info->max_block_num; i++)
        {
            if(FALSE == block[i].is_block_can_shrink)
            {
                return CTC_E_NO_RESOURCE;
            }
            CTC_MAX_VALUE_CHECK(block[i].used_of_num, block[i].all_of_num);

            if((block[i].all_of_num - block[i].used_of_num == 1) && (block[i].used_of_num == 0))
            {
                continue;
            }
            if(block[i].all_of_num - block[i].used_of_num >= 1)
            {
                *adj_block_id = i;
                break;
            }
        }
        if(i == key_info->max_block_num)
        {
            return CTC_E_NO_RESOURCE;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_do_block_shrink(sys_sort_key_info_t* key_info, sys_sort_block_dir_t dir)
{
    sys_sort_block_t* block;
    uint32 old_offset, new_offset;
    sys_humber_opf_t opf;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    opf.pool_type= key_info->type;
    opf.pool_index = key_info->block_id;

    /* 2. do it */
    if(SYS_SORT_BLOCK_DIR_UP == dir)
    {
        /* 1) alloc one offset, which is supposed to be down side */
        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &new_offset));

        /* 2) move the top item to the "new offset", causing the HARD & DB entry move as well */
        old_offset = block->all_left_b;

        CTC_ERROR_RETURN((key_info->sort_key_syn_key)(new_offset, old_offset));

        /* 3) adjust the border property */
        CTC_NOT_EQUAL_CHECK(CTC_MAX_UINT32_VALUE, block->all_left_b);
        ++(block->all_left_b);
    }
    else
    {
        /* 1) alloc one offset, which is supposed to be up side */
        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset_last(&opf, 1, &new_offset));

        /* 2) move the top item to the "new offset", causing the HARD & DB entry move as well */
        old_offset = block->all_right_b;
        CTC_ERROR_RETURN((key_info->sort_key_syn_key)(new_offset, old_offset));

        /* 3) adjust the border property */
        CTC_NOT_EQUAL_CHECK(0, block->all_right_b);
        --(block->all_right_b);
    };

    /*4.adjust block info*/
    CTC_MAX_VALUE_CHECK(block->used_of_num, block->all_of_num);
    --(block->all_of_num);

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_do_block_grow_with_dir(sys_sort_key_info_t* key_info, sys_sort_block_dir_t dir)
{
    sys_sort_block_t* block;
    uint32 desired_offset;
    sys_humber_opf_t opf;

    /* take ONE offset in the direction "dir", put it into the offset pool, and that's it */

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);

    /* 2. do it */
    /* 1) get offset */
    if(SYS_SORT_BLOCK_DIR_UP == dir)
    {
        CTC_NOT_EQUAL_CHECK(0, block->all_left_b);
        --(block->all_left_b);
        desired_offset = block->all_left_b;
    }
    else
    {
        CTC_NOT_EQUAL_CHECK(CTC_MAX_UINT32_VALUE, block->all_right_b);
        ++(block->all_right_b);
        desired_offset = block->all_right_b;
    }

    CTC_MAX_VALUE_CHECK(block->all_left_b, block->all_right_b);

    /* 2) put the offset into the offset DB */
    opf.pool_type= key_info->type;
    opf.pool_index = key_info->block_id;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, desired_offset));

    /* 4) adjust block info */
    CTC_NOT_EQUAL_CHECK(CTC_MAX_UINT32_VALUE, block->all_of_num);
    ++(block->all_of_num);

    return DRV_E_NONE;
}

static int32
_sys_humber_sort_key_block_grow_with_dir(sys_sort_key_info_t* key_info, sys_sort_block_dir_t dir)
{
    sys_sort_block_dir_t adj_dir;
    uint32 adj_block_id=0;
    sys_sort_key_info_t adj_key_info;
    uint32 i;

    /* 1. sanity check & init */
    SYS_SORT_CHECK_KEY_BLOCK_EXIST(key_info, dir);
    CTC_ERROR_RETURN(_sys_humber_sort_key_adj_block_id(key_info, dir, &adj_block_id));
    adj_dir = SYS_SORT_OPPO_DIR(dir);
    kal_memcpy(&adj_key_info, key_info, sizeof(sys_sort_key_info_t));
    adj_key_info.block_id = adj_block_id;

    if(dir == SYS_SORT_BLOCK_DIR_DOWN)
    {
        for( i = adj_block_id; i > key_info->block_id; i--)
        {
            adj_key_info.block_id = i;
            CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_shrink(&adj_key_info, adj_dir));
            adj_key_info.block_id = i - 1;
            CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_grow_with_dir(&adj_key_info, dir));
        }
    }
    else
    {
        for( i = adj_block_id; i < key_info->block_id; i++)
        {
            adj_key_info.block_id = i;
            CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_shrink(&adj_key_info, adj_dir));
            adj_key_info.block_id = i + 1;
            CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_grow_with_dir(&adj_key_info, dir));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_sort_key_block_grow(sys_sort_key_info_t* key_info)
{
    sys_sort_block_dir_t first_dir, second_dir;
    int32 ret;
    sys_sort_block_t* block = NULL;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    first_dir = block->preferred_dir;
    second_dir = SYS_SORT_OPPO_DIR(first_dir);

    /* 2. do it */
    ret = _sys_humber_sort_key_block_grow_with_dir(key_info, first_dir);
    if(CTC_E_NONE != ret)
    {
        CTC_ERROR_RETURN(_sys_humber_sort_key_block_grow_with_dir(key_info, second_dir));
    }

    return CTC_E_NONE;
}

/**
 @brief function of alloc offset

 @param[in] key_info, key of sort data
 @param[out] p_offset, alloced offset
 @return CTC_E_XXX
 */
int32
sys_humber_sort_key_alloc_offset(sys_sort_key_info_t* key_info, uint32* p_offset)
{
    bool has;

    /* 1. sanity check & init */
    SYS_SORT_CHECK_KEY_INFO(key_info);
    CTC_PTR_VALID_CHECK(p_offset);

    /* 2. do it */
    CTC_ERROR_RETURN(_sys_humber_sort_key_block_has_avil_offset(key_info, &has));
    if(!has)
    {
        CTC_ERROR_RETURN(_sys_humber_sort_key_block_grow(key_info));
    }

    CTC_ERROR_RETURN(_sys_humber_sort_key_do_alloc_offset(key_info, p_offset));

    return CTC_E_NONE;
}

/**
 @brief function of free offset

 @param[in] key_info, key of sort data
 @param[in] p_offset, offset should be free
 @return CTC_E_XXX
 */
int32
sys_humber_sort_key_free_offset(sys_sort_key_info_t* key_info, uint32 offset)
{
    sys_sort_block_t* block;
    sys_sort_key_info_t grow_key_info;

    /* 1. sanity check & init */
    block = &(key_info->block[key_info->block_id]);
    kal_memcpy(&grow_key_info, key_info, sizeof(sys_sort_key_info_t));

    /* 2. do it */
    CTC_ERROR_RETURN(_sys_humber_sort_key_do_free_offset(key_info, offset));
    if(block->all_of_num == 1)
    {
        return CTC_E_NONE;
    }

    if(offset < block->init_left_b)
    {
        grow_key_info.block_id -= 1;
        CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_shrink(key_info, SYS_SORT_BLOCK_DIR_UP));
        CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_grow_with_dir(&grow_key_info, SYS_SORT_BLOCK_DIR_DOWN));
    }
    else if(offset > block->init_right_b)
    {
        grow_key_info.block_id += 1;
        CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_shrink(key_info, SYS_SORT_BLOCK_DIR_DOWN));
        CTC_ERROR_RETURN(_sys_humber_sort_key_do_block_grow_with_dir(&grow_key_info, SYS_SORT_BLOCK_DIR_UP));
    }

    return CTC_E_NONE;
}

int32
sys_humber_sort_key_init_block(sys_sort_block_init_info_t* init_info)
{
    uint32 offset;

    /* 1. check */
    CTC_PTR_VALID_CHECK(init_info->block);
    CTC_MAX_VALUE_CHECK(init_info->boundary_l, init_info->boundary_r);

    /*2.do it*/

    /* 3). init the first of-node */
    CTC_ERROR_RETURN(sys_humber_opf_init_offset(init_info->opf, 0, init_info->max_offset_num));
    if(init_info->boundary_l > 0)
    {
        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(init_info->opf, init_info->boundary_l, &offset));
    }
    if(init_info->boundary_r + 1 < init_info->max_offset_num)
    {
        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset_from_position(init_info->opf,
            init_info->max_offset_num - init_info->boundary_r - 1, init_info->boundary_r + 1));
    }

    init_info->block->all_left_b = init_info->boundary_l;
    init_info->block->all_right_b = init_info->boundary_r;
    init_info->block->all_of_num = init_info->boundary_r - init_info->boundary_l + 1;
    init_info->block->used_of_num = 0;
    init_info->block->preferred_dir = init_info->dir;
    init_info->block->init_left_b = init_info->boundary_l;
    init_info->block->init_right_b = init_info->boundary_r;
    init_info->block->is_block_can_shrink = init_info->is_block_can_shrink;

    return CTC_E_NONE;
}

int32
sys_humber_sort_key_init_offset_array(skinfo_2a* pp_offset_array, uint32 max_offset_num)
{
    uint32 num, left;
    int32 i = 0;
    skinfo_a* p_offset_array;

    if(max_offset_num > SORT_KEY_OFFSET_ARRAY_UNIT * SORT_KEY_OFFSET_ARRAY_MAX_NUM)
    {
        return CTC_E_NO_RESOURCE;
    }

    num = max_offset_num / SORT_KEY_OFFSET_ARRAY_UNIT;
    left = max_offset_num % SORT_KEY_OFFSET_ARRAY_UNIT;

    if(0 == left)
    {
        p_offset_array = mem_malloc(MEM_SORT_KEY_MODULE, num * sizeof(skinfo_a));
        kal_memset(p_offset_array, 0, num * sizeof(skinfo_a));
    }
    else
    {
        p_offset_array = mem_malloc(MEM_SORT_KEY_MODULE, (num + 1) * sizeof(skinfo_a));
        kal_memset(p_offset_array, 0, (num + 1) * sizeof(skinfo_a));
    }

    for(i = 0; i < num; i++)
    {
        p_offset_array[i] = mem_malloc(MEM_SORT_KEY_MODULE,
                                       SORT_KEY_OFFSET_ARRAY_UNIT * sizeof(skinfo_t));
        kal_memset(p_offset_array[i], 0, SORT_KEY_OFFSET_ARRAY_UNIT * sizeof(skinfo_t));
    }

    if(left != 0)
    {
        p_offset_array[i] = mem_malloc(MEM_SORT_KEY_MODULE, left * sizeof(skinfo_t));
        kal_memset(p_offset_array[i], 0, left * sizeof(skinfo_t));
    }

    *pp_offset_array = p_offset_array;

    return CTC_E_NONE;
}


