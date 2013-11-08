#include "kal.h"
struct kal_mem_pool
{
    uint32 num;
    size_t size;
};


void *kal_malloc_atomic(size_t size)
{
    return malloc(size);
}


void kal_malloc_failed(const char *file, int line, size_t size)
{
    kal_log(KAL_LL_ERROR, file, line, "malloc(%d) failed!", size);
}

int kal_mem_pool_create(kal_mem_pool_t **mem_pool, const char *name,
			size_t size, size_t align, uint32 min_nr)
{
    KAL_MALLOC(*mem_pool, kal_mem_pool_t *, sizeof(kal_mem_pool_t));
    if(*mem_pool==NULL)
        return ENOMEM;
    (*mem_pool)->num = 0;
    (*mem_pool)->size = size;
    return 0;
}

void kal_mem_pool_destroy(kal_mem_pool_t *mem_pool)
{
    if(mem_pool->num!=0)
        KAL_LOG_INFO("fail to destroy this memory pool!" );
    else
        KAL_FREE(mem_pool);
}

#ifdef _KAL_DEBUG
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic,
				const char *file, int line)
{
    void *temp;

    temp = malloc(mem_pool->size);
    if(temp)
    {
        mem_pool->num++;
    }
    else
        kal_log(KAL_LL_ERROR, file, line, "malloc(%d) failed!",mem_pool->size);

    return temp;
}
#else
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic)
{
    void *temp;

    temp = malloc(mem_pool->size);
    if(temp)
        mem_pool->num++;

    return temp;
}
#endif

void kal_mem_pool_free(kal_mem_pool_t *mem_pool, void *p)
{
    KAL_FREE(p);
    mem_pool->num--;
}

