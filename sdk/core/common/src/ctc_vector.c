
 /****************************************************************************
 *file ctc_vector.c

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-11

 *version v2.0

 Vector containers are implemented as dynamic arrays; Just as regular arrays, vector containers
  can use offsets on regular pointers to elements
 ****************************************************************************/

#include "kal.h"
#include "ctc_vector.h"

#define CTC_MAX_VEC_BLOCK_NUM   256

/* Vector structure. */
struct ctc_vector_db_s
{
  ctc_vector_t vec;
  uint32  vec_size[CTC_MAX_VEC_BLOCK_NUM];
  void ***pp_array;
};
typedef struct ctc_vector_db_s  ctc_vector_db_t;


/**
   @brief   add data to vector according to the index
 */

bool
ctc_vector_add(ctc_vector_t *vec, uint32 index, void *data)
{
    uint8 idx_1d = 0;
    uint32 idx_2d = 0;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec || !p_vector->pp_array)
    {
        return FALSE;
    }

    idx_1d  = (uint8)(index / p_vector->vec.block_size);
    idx_2d  = index % p_vector->vec.block_size;

    if ( idx_1d >= p_vector->vec.max_block_num)
    {
        return FALSE;
    }
    if (!p_vector->pp_array[idx_1d])
    {
        p_vector->pp_array[idx_1d] = (void *)mem_malloc(MEM_VECTOR_MODULE,
                                                        p_vector->vec.block_size * sizeof(void *));
        if ( p_vector->pp_array[idx_1d])
        {
            kal_memset( p_vector->pp_array[idx_1d], 0, p_vector->vec.block_size * sizeof(void *));
        }

    }

    if (!p_vector->pp_array[idx_1d])
    {
        return FALSE;
    }
    if (!p_vector->pp_array[idx_1d][idx_2d]
        && data)
    {
        p_vector->vec_size[idx_1d]++;
    }
    p_vector->pp_array[idx_1d][idx_2d] = data;

    return TRUE;

}
/**
   @brief   delete data from vector according to the index
 */

void *
ctc_vector_del (ctc_vector_t *vec, uint32 index)
{
    uint32  idx_1d = 0;
    uint32  idx_2d = 0;
   void *p_data = NULL;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec || !p_vector->pp_array)
    {
        return NULL;
    }

    idx_1d  = index / p_vector->vec.block_size;
    idx_2d  = index % p_vector->vec.block_size;

    if (idx_1d >= p_vector->vec.max_block_num
        || !p_vector->pp_array[idx_1d])
    {
        return NULL;
    }
    if ( p_vector->pp_array[idx_1d][idx_2d] )
    {
        p_vector->vec_size[idx_1d]--;
    }
    p_data = p_vector->pp_array[idx_1d][idx_2d];
    p_vector->pp_array[idx_1d][idx_2d] = NULL;
    if(p_vector->vec_size[idx_1d] == 0)
    {
        mem_free(p_vector->pp_array[idx_1d] );
        p_vector->pp_array[idx_1d] = NULL;
    }

    return p_data;

}

/**
   @brief   get data from vector according to the index
 */

void *
ctc_vector_get (ctc_vector_t *vec, uint32 index )
{
    uint32  idx_1d = 0;
    uint32  idx_2d = 0;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec || !p_vector->pp_array)
    {
        return NULL ;
    }

    idx_1d  = index / p_vector->vec.block_size;
    idx_2d  = index % p_vector->vec.block_size;
    if (idx_1d >= p_vector->vec.max_block_num
      || !p_vector->pp_array[idx_1d])
    {
        return NULL ;
    }

    return p_vector->pp_array[idx_1d][idx_2d];

}

int32
ctc_vector_traverse (ctc_vector_t *vec, vector_traversal_fn fn,void *data)
{
    uint32  idx_1d, idx_2d;
    int32 ret = 0;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec || !p_vector->pp_array)
    {
        return 0;
    }

    for(idx_1d = 0; idx_1d < p_vector->vec.max_block_num; idx_1d++)
    {
        if(!p_vector->pp_array[idx_1d])
        {
            continue;
        }
        for(idx_2d = 0; idx_2d < p_vector->vec.block_size; idx_2d++)
        {
            if ((ret = (*fn)(p_vector->pp_array[idx_1d][idx_2d], data)) < 0)
                return ret;
        }
    }

    return 0;
}

/**
   @brief   release  a  vector
 */
void
ctc_vector_reserve (ctc_vector_t *vec, uint8 block_num )
{
    uint32  idx_1d = 0;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec || !p_vector->pp_array)
    {
        return ;
    }

    if (block_num > p_vector->vec.max_block_num)
    {
        return ;
    }

    for (idx_1d = 0; idx_1d < block_num; idx_1d++)\
    {
        if (!p_vector->pp_array[idx_1d])
        {
            p_vector->pp_array[idx_1d] = (void *)mem_malloc(MEM_VECTOR_MODULE,
                                                            p_vector->vec.block_size * sizeof(void *));
            if ( p_vector->pp_array[idx_1d])
            {
               kal_memset( p_vector->pp_array[idx_1d], 0, p_vector->vec.block_size * sizeof(void *));
            }
        }
    }

}

/**
   @brief    Reserve space
 */
void
ctc_vector_release(struct ctc_vector_s *vec)
{
    uint32  idx_1d = 0;
    ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

    if (!vec)
    {
        return ;
    }
    if(!p_vector->pp_array)
    {
        mem_free(p_vector);
        return ;
    }
    for (idx_1d = 0; idx_1d < p_vector->vec.max_block_num; idx_1d++)\
    {
        if (p_vector->pp_array[idx_1d])
        {
            mem_free(p_vector->pp_array[idx_1d]);
            p_vector->pp_array[idx_1d] = NULL;
            p_vector->vec_size[idx_1d] = 0;
        }
    }

    mem_free(p_vector->pp_array);
    p_vector->pp_array = NULL;
    mem_free(p_vector);
}
 /**
 @brief    Return  a block allocated status
*/
extern bool
ctc_vector_get_block_status(ctc_vector_t* vec, uint8 block_index )
 {
   uint32  idx_1d = 0;
   ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;

   if (!vec || !p_vector->pp_array)
   {
       return FALSE;
   }

   if (p_vector->pp_array[idx_1d])
   {
       return TRUE;
   }
   return FALSE;
 }
   /**
  @brief    return the number of  stored member

  @param[in] vec      a Vector structure
  @param[in] index    block's index
  @return   uint32
 */
 extern uint32
 ctc_vector_get_size(ctc_vector_t* vec, uint8 block_index )
  {
      ctc_vector_db_t *p_vector = (ctc_vector_db_t *)vec;
      if (!vec || !p_vector->pp_array)
      {
          return 0 ;
      }
      if (block_index >= p_vector->vec.max_block_num)
      {
          return 0 ;
      }

     return p_vector->vec_size[block_index];

  }

/**
   @brief    Create a Vector structure
 */
ctc_vector_t *
ctc_vector_init (uint8 max_block_num, uint32 block_size)
{
   ctc_vector_db_t *p_vector = NULL;

   p_vector = (ctc_vector_db_t *)mem_malloc(MEM_VECTOR_MODULE, sizeof(ctc_vector_db_t));

   if (!p_vector)
   {
       return NULL;
   }
   kal_memset( p_vector , 0, sizeof(ctc_vector_db_t));
   p_vector->vec.max_block_num = max_block_num;
   p_vector->vec.block_size = block_size;
   p_vector->pp_array = (void ***)mem_malloc(MEM_VECTOR_MODULE, max_block_num*sizeof(void *));

   if (!p_vector->pp_array)
   {
       mem_free(p_vector);
       return NULL;
   }
   kal_memset( p_vector->pp_array, 0, max_block_num * sizeof(void *));

   return (ctc_vector_t*)&p_vector->vec;
}
