#include "kal.h"
#include "linux/slab.h"
#include"linux/mempool.h"
#include"linux/types.h"
#include"linux/gfp.h"

struct kal_mem_pool
{
    mempool_t *mempool;
    kmem_cache_t *cache;
};


void *kal_malloc_atomic(size_t size)
{
    return kmalloc(size, GFP_ATOMIC);
}



void kal_malloc_failed(const char *file, int line, size_t size)
{
    kal_log(KAL_LL_ERROR, file, line, "malloc(%d) failed!", size);
}

int kal_mem_pool_create(kal_mem_pool_t **mem_pool, const char *name,
	size_t size, size_t align, uint32 min_nr)
{
    mempool_t *pool;

    KAL_MALLOC(*mem_pool, kal_mem_pool_t *, sizeof(kal_mem_pool_t));
    if(*mem_pool==NULL)
        return ENOMEM;

    (*mem_pool)->cache = kmem_cache_create(name,size,align,0,NULL,NULL);
    if(!((*mem_pool)->cache))
    {
        KAL_FREE(*mem_pool);
        return ENOMEM;
    }

    pool = mempool_create(min_nr,mempool_alloc_slab,mempool_free_slab,
				(*mem_pool)->cache);
    if(!pool)
    {
        kmem_cache_destroy((*mem_pool)->cache);
        KAL_FREE(*mem_pool);
        return ENOMEM;
    }
    (*mem_pool)->mempool = pool;

    return 0;
}

void kal_mem_pool_destroy(kal_mem_pool_t *mem_pool)
{
    mempool_destroy(mem_pool->mempool);
    kmem_cache_destroy(mem_pool->cache);
    KAL_FREE(mem_pool);
}

#ifdef _KAL_DEBUG
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic,
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
        kal_log(KAL_LL_ERROR, file, line, "malloc failed!");

    return(ret);
}
#else
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic)
{
    gfp_t gfp_mask;

    if(atomic)
        gfp_mask = GFP_ATOMIC;
    else
        gfp_mask = GFP_KERNEL;

    return(mempool_alloc(mem_pool->mempool,gfp_mask));
}
#endif

void kal_mem_pool_free(kal_mem_pool_t *mem_pool, void *p)
{
    mempool_free(p, mem_pool->mempool);
}


EXPORT_SYMBOL(kal_malloc);
EXPORT_SYMBOL(kal_free);
EXPORT_SYMBOL(kal_malloc_failed);

