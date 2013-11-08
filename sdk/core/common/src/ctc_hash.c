 /****************************************************************************
 *file ctc_hash.c

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

 The file define  HASH arithmetic lib
 ****************************************************************************/

#include "ctc_hash.h"

ctc_hash_t *
ctc_hash_create(uint16 block_num,uint16 block_size,uint32 (*hash_key) (), bool(*hash_cmp) ())
{
   ctc_hash_t *hash = NULL;

    hash = mem_malloc(MEM_HASH_MODULE, sizeof (ctc_hash_t));

    if (NULL == hash)
    {
        return NULL;
    }
    kal_memset(hash ,0,sizeof (ctc_hash_t) );
    hash->index = mem_malloc (MEM_HASH_MODULE,  block_num*sizeof (void *) );

    if (NULL == hash->index)
    {
        mem_free(hash);
        return NULL;
    }
    kal_memset(hash->index ,0,sizeof (void *) * block_num);
    hash->block_size= block_size;
    hash->block_num = block_num;
    hash->hash_key = hash_key;
    hash->hash_cmp = hash_cmp;
    return hash;
}



void *
ctc_hash_lookup (ctc_hash_t *hash, void *data)
{
    uint32 key = 0;
    uint32 index = 0;
     uint32 hash_size = 0;
    uint16 idx_1d = 0;
    uint16 idx_2d = 0;
    ctc_hash_backet_t *backet = NULL;

    if (!hash)
      return NULL;

    hash_size =hash->block_size* hash->block_num;
    key = (*hash->hash_key) (data);
    index = key % hash_size;

    idx_1d  = index / hash->block_size ;
    idx_2d  = index % hash->block_size ;

    if (!hash->index[idx_1d])
    {
        return NULL;
    }

    for (backet = hash->index[idx_1d][idx_2d]; backet != NULL; backet = backet->next)
    {
        if (backet->key == key
            && (*hash->hash_cmp)(backet->data, data) == TRUE)
        return backet->data;
    }

    return NULL;
}

void *
ctc_hash_lookup2 (ctc_hash_t *hash, void *data, uint32 *hash_index)
{
    uint32 key = 0;
    uint32 index = 0;
     uint32 hash_size = 0;
    uint16 idx_1d = 0;
    uint16 idx_2d = 0;
    ctc_hash_backet_t *backet = NULL;

    if (!hash || !hash_index)
      return NULL;

    hash_size =hash->block_size* hash->block_num;
    key = (*hash->hash_key) (data);

    index = key % hash_size;
    *hash_index = index;
    idx_1d  = index / hash->block_size ;
    idx_2d  = index % hash->block_size ;

    if (!hash->index[idx_1d])
    {
        return NULL;
    }

    for (backet = hash->index[idx_1d][idx_2d]; backet != NULL; backet = backet->next)
    {
        if (backet->key == key
            && (*hash->hash_cmp)(backet->data, data) == TRUE)
        return backet->data;
    }

    return NULL;
}

int32
ctc_hash_traverse(ctc_hash_t* hash, hash_traversal_fn fn,void *data)
{
    uint32 count = 0;
    uint16 index1 = 0;
    uint16 index2 = 0;
    uint32 hash_size = 0;
    ctc_hash_backet_t *backet = NULL;
    int32 ret = 0;

    if (!hash)
        return -1;

    if (hash->count == 0)
    {
        return 0;
    }
    hash_size =hash->block_size * hash->block_num;

    for (index1 = 0; index1 < hash->block_num; index1++)
    {
        if (!hash->index[index1])
        {
            continue;
        }
        for (index2 = 0; index2 < hash->block_size; index2++)
        {
            for (backet = hash->index[index1][index2]; backet != NULL; backet = backet->next)
            {
                count++;
                if ((ret = (*fn)(backet->data, data)) < 0)
                    {

                    return ret;
                    }

                if (count >= hash->count)
                {
                    return 0;
                }
            }
        }
    }
    return 0;

}

/* Traverse hash special array according to KEY */
extern int32
ctc_hash_traverse2(ctc_hash_t* hash, hash_traversal_fn fn,void *data)
{
    uint32 key = 0;
    uint32 index = 0;
    uint32 hash_size = 0;
    uint16 idx_1d = 0;
    uint16 idx_2d = 0;
    ctc_hash_backet_t *backet = NULL;
    int32 ret = 0;

    if (!hash)
    {
        return -1;
    }
    hash_size =hash->block_size * hash->block_num;
    key = (*hash->hash_key) (data);
    index = key % hash_size;

    idx_1d  = index / hash->block_size ;
    idx_2d  = index % hash->block_size ;
    if (!hash->index[idx_1d])
    {
        return -1;
    }

    for (backet = hash->index[idx_1d][idx_2d]; backet != NULL; backet = backet->next)
    {
        if ((ret = (*fn)(backet->data, data)) < 0)
            return ret;
    }

    if (NULL == backet)
    {
        return 1;
    }

    return 0;
}



/*traverse  all throught, without check count*/
int32
ctc_hash_traverse_through(ctc_hash_t* hash, hash_traversal_fn fn,void *data)
{
    uint32 count = 0;
    uint16 index1 = 0;
    uint16 index2 = 0;
    uint32 hash_size = 0;
    ctc_hash_backet_t *backet = NULL;
    ctc_hash_backet_t *backet_next = NULL;

    int32 ret = 0;

    if (!hash)
        return -1;

    if (hash->count == 0)
    {
        return 0;
    }
    count = hash->count;

    hash_size =hash->block_size * hash->block_num;

    for (index1 = 0; index1 < hash->block_num; index1++)
    {
        if (!hash->index[index1])
        {
            continue;
        }
        for (index2 = 0; index2 < hash->block_size; index2++)
        {
            if (!hash->index[index1][index2])
            {
                continue;
            }

            for (backet = hash->index[index1][index2]; backet != NULL; backet = backet_next)
            {
                backet_next = backet->next;
                count--;
                if ((ret = (*fn)(backet->data, data)) < 0)
                {
                    return ret;
                }

                if (count == 0)
                {
                    return 0;
                }
            }
        }
    }
    return 0;

}

void *
ctc_hash_insert (ctc_hash_t *hash, void *data)
{
    uint32 key = 0;
    uint32 index = 0;
    uint32 hash_size = 0;
    uint16 idx_1d = 0;
    uint16 idx_2d = 0;
    ctc_hash_backet_t *backet = NULL;


    if (!hash)
        return NULL;

    key = (*hash->hash_key) (data);
    hash_size =hash->block_size * hash->block_num;
    index = key % hash_size;
    idx_1d  = index / hash->block_size ;
    idx_2d  = index % hash->block_size ;
    if ( idx_1d >= hash->block_num)
    {
        return NULL;
    }
    if (!hash->index[idx_1d])
    {
        hash->index[idx_1d] = (void *)mem_malloc(MEM_VECTOR_MODULE,
                                                         hash->block_size * sizeof(void *));
        if ( hash->index[idx_1d])
         {
            kal_memset( hash->index[idx_1d], 0,  hash->block_size * sizeof(void *));
        }

    }
    if (!hash->index[idx_1d])
    {
        return NULL;
    }

    backet = mem_malloc (MEM_HASH_MODULE, sizeof (ctc_hash_backet_t));
    if (!backet)
        return NULL;

    backet->data = data;
    backet->key = key;
    backet->next = hash->index[idx_1d][idx_2d];

    hash->index[idx_1d][idx_2d] = backet;

    hash->count++;

    return backet->data;
}

int32
ctc_hash_get_count (ctc_hash_t *hash, uint32* count)
{
    *count = hash->count ;
    return 0;
}

void *
ctc_hash_remove (ctc_hash_t *hash, void *data)
{
   uint32 key = 0;
   uint32 index = 0;
   uint32 hash_size = 0;
   uint16 idx_1d = 0;
   uint16 idx_2d = 0;
   ctc_hash_backet_t *backet = NULL;
   ctc_hash_backet_t *backet_next= NULL;

   hash_size =hash->block_size* hash->block_num;
   key = (*hash->hash_key) (data);
   index = key % hash_size;
   idx_1d  = index / hash->block_size ;
   idx_2d  = index % hash->block_size ;

   if (!hash->index[idx_1d])
   {
        return NULL;
   }

   for (backet = hash->index[idx_1d][idx_2d],backet_next = backet?backet->next:NULL;
       backet; backet = backet_next, backet_next = backet?backet->next:NULL)
   {
      if ( backet->key == key
           && (*hash->hash_cmp) (backet->data, data) == TRUE )
        { /*head node*/
           hash->index[idx_1d][idx_2d] =  backet_next;
           mem_free(backet);
           hash->count--;
           return data;
        }

       if (backet_next
         && backet_next->key == key
           && (*hash->hash_cmp) (backet_next->data, data) == TRUE)
       {
           backet->next = backet_next->next;
           mem_free(backet_next);
           hash->count--;
           return data;
       }
   }
   return NULL;
}

void
ctc_hash_traverse_remove(ctc_hash_t *hash, hash_traversal_fn fn, void *data)
{
    uint16 index1 = 0;
    uint16 index2 = 0;
    ctc_hash_backet_t *backet = NULL;
    ctc_hash_backet_t *backet_next = NULL;

    for (index1 = 0; index1 < hash->block_num; index1++)
    {
       if (!hash->index[index1])
       {
           continue;
       }
       for (index2 = 0; index2 < hash->block_size; index2++)
       {
           for (backet = hash->index[index1][index2], backet_next = backet?backet->next:NULL;
               backet;     backet = backet_next, backet_next = backet?backet->next:NULL)
           {
               if ((backet == hash->index[index1][index2]) && (*fn) (backet->data, data) == TRUE)
               {
                    hash->index[index1][index2] =  backet_next;
                    mem_free(backet);
                    hash->count--;

                    if ( hash->count == 0)
                    {
                       return;
                    }
                    continue;
               }

               if (backet_next && ((*fn) (backet_next->data, data) == TRUE))
               {
                    backet->next = backet_next->next;
                    mem_free(backet_next);
                    backet_next = backet;
                    hash->count--;
                    if (hash->count == 0)
                    {
                       return;
                    }
               }

           }
       }
    }
}

/* Traverse hash special array according to KEY */
void
ctc_hash_traverse2_remove(ctc_hash_t* hash, hash_traversal_fn fn,void *data)
{
    uint32 key = 0;
    uint32 index = 0;
    uint32 hash_size = 0;
    uint16 idx_1d = 0;
    uint16 idx_2d = 0;
    ctc_hash_backet_t *backet = NULL;
    ctc_hash_backet_t *backet_next = NULL;

    if (!hash)
    {
        return;
    }
    hash_size =hash->block_size * hash->block_num;
    key = (*hash->hash_key) (data);
    index = key % hash_size;

    idx_1d  = index / hash->block_size ;
    idx_2d  = index % hash->block_size ;
    if (!hash->index[idx_1d])
    {
        return;
    }

    for (backet = hash->index[idx_1d][idx_2d], backet_next = backet?backet->next:NULL;
           backet; backet = backet_next, backet_next = backet?backet->next:NULL)
    {
        (*fn) (backet->data, data);
    }
}

void ctc_hash_free (ctc_hash_t * hash)
{
    uint16 index1 = 0;
    uint16 index2 = 0;
    ctc_hash_backet_t *backet = NULL;
    ctc_hash_backet_t *backet_next = NULL;

    for (index1 = 0; index1 < hash->block_num; index1++)
    {
        if (!hash->index[index1])
        {
            continue;
        }
        for (index2 = 0; index2 <  hash->block_size; index2++)
        {
            for (backet = hash->index[index1][index2] ; backet; backet = backet_next)
            {
                backet_next = backet->next;
                mem_free (backet);
            }

        }
        mem_free ( hash->index[index1]);
    }
    mem_free ( hash->index);
    mem_free ( hash);
}

