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

struct sal_mem_pool
{
    uint32 num;
    size_t size;
};

void *sal_malloc(size_t size)
{
    return malloc(size);
}

void *sal_calloc(size_t size)
{
    return calloc(1, size);
}

void *sal_malloc_atomic(size_t size)
{
    return malloc(size);
}

void sal_free(void *p)
{
    free(p);
}

void sal_malloc_failed(const char *file, int line, size_t size)
{
    sal_log(SAL_LL_ERROR, file, line, "malloc(%d) failed!", size);
}

int sal_mem_pool_create(sal_mem_pool_t **mem_pool, const char *name,
            size_t size, size_t align, uint32 min_nr)
{
    SAL_MALLOC(*mem_pool, sal_mem_pool_t *, sizeof(sal_mem_pool_t));
    if(*mem_pool==NULL)
        return ENOMEM;
    (*mem_pool)->num = 0;
    (*mem_pool)->size = size;
    return 0;
}

void sal_mem_pool_destroy(sal_mem_pool_t *mem_pool)
{
    if(mem_pool->num!=0)
        SAL_LOG_INFO("fail to destroy this memory pool!" );
    else
        SAL_FREE(mem_pool);
}

#ifdef _SAL_DEBUG
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic,
                const char *file, int line)
{
    void *temp;

    temp = malloc(mem_pool->size);
    if(temp)
    {
        mem_pool->num++;
    }
    else
        sal_log(SAL_LL_ERROR, file, line, "malloc(%d) failed!",mem_pool->size);

    return temp;
}
#else
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic)
{
    void *temp;

    temp = malloc(mem_pool->size);
    if(temp)
        mem_pool->num++;

    return temp;
}
#endif

void sal_mem_pool_free(sal_mem_pool_t *mem_pool, void *p)
{
    SAL_FREE(p);
    mem_pool->num--;
}
