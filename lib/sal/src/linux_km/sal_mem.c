/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "sal.h"
#include "linux/slab.h"
#include "linux/mempool.h"
#include "linux/types.h"
#include "linux/gfp.h"

struct sal_mem_pool
{
    mempool_t *mempool;
    kmem_cache_t *cache;
};

void *sal_malloc(size_t size)
{
    return kmalloc(size, GFP_KERNEL);
}

void *sal_calloc(size_t size)
{
    void *ptr = kmalloc(size, GFP_KERNEL);
    if (ptr)
    {
        memset(ptr, 0, sizeof(size));
    }
    return ptr;
}

void *sal_malloc_atomic(size_t size)
{
    return kmalloc(size, GFP_ATOMIC);
}

void sal_free(void *p)
{
    kfree(p);
}

void sal_malloc_failed(const char *file, int line, size_t size)
{
    sal_log(SAL_LL_ERROR, file, line, "malloc(%d) failed!", size);
}

int sal_mem_pool_create(sal_mem_pool_t **mem_pool, const char *name,
    size_t size, size_t align, uint32 min_nr)
{
    mempool_t *pool;

    SAL_MALLOC(*mem_pool, sal_mem_pool_t *, sizeof(sal_mem_pool_t));
    if(*mem_pool==NULL)
        return ENOMEM;

    (*mem_pool)->cache = kmem_cache_create(name,size,align,0,NULL,NULL);
    if(!((*mem_pool)->cache))
    {
        SAL_FREE(*mem_pool);
        return ENOMEM;
    }

    pool = mempool_create(min_nr,mempool_alloc_slab,mempool_free_slab,
                (*mem_pool)->cache);
    if(!pool)
    {
        kmem_cache_destroy((*mem_pool)->cache);
        SAL_FREE(*mem_pool);
        return ENOMEM;
    }
    (*mem_pool)->mempool = pool;

    return 0;
}

void sal_mem_pool_destroy(sal_mem_pool_t *mem_pool)
{
    mempool_destroy(mem_pool->mempool);
    kmem_cache_destroy(mem_pool->cache);
    SAL_FREE(mem_pool);
}

#ifdef _SAL_DEBUG
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic,
            const char *file, int line)
{
    gfp_t gfp_mask;
    void *ret;

    if(atomic)
        gfp_mask = GFP_ATOMIC;
    else
        gfp_mask = GFP_KERNEL;

    ret = mempool_alloc(mem_pool->mempool,gfp_mask);
    if(!ret)
        sal_log(SAL_LL_ERROR, file, line, "malloc failed!");

    return(ret);
}
#else
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic)
{
    gfp_t gfp_mask;

    if(atomic)
        gfp_mask = GFP_ATOMIC;
    else
        gfp_mask = GFP_KERNEL;

    return(mempool_alloc(mem_pool->mempool,gfp_mask));
}
#endif

void sal_mem_pool_free(sal_mem_pool_t *mem_pool, void *p)
{
    mempool_free(p, mem_pool->mempool);
}


EXPORT_SYMBOL(sal_malloc);
EXPORT_SYMBOL(sal_free);
EXPORT_SYMBOL(sal_malloc_failed);
