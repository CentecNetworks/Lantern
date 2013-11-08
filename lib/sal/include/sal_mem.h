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

#ifndef __SAL_MEM_H__
#define __SAL_MEM_H__

/**
 * @file sal_mem.h
 */


/**
 * @defgroup mem Memory Allocation
 * @{
 */

/**
 * @brief Allocate dynamic memory
 *
 * Wrapper on sal_malloc(). Memory allocation failures will be traced.
 *
 * @param[out] p
 * @param[in]  type
 * @param[in]  size
 *
 * @see sal_malloc
 */
#define SAL_MALLOC(p, type, size)                    \
do                                                   \
{                                                    \
    p = (type)sal_malloc(size);                      \
    if (!p)                                          \
        sal_malloc_failed(__FILE__, __LINE__, size); \
} while (0)

/**
 * @brief Allocate dynamic memory without blocking
 *
 * Wrapper on sal_malloc_atomic(). Memory allocation failures will be traced.
 *
 * @param[out] p
 * @param[in]  type
 * @param[in]  size
 *
 * @see sal_malloc_atomic
 */
#define SAL_MALLOC_ATOMIC(p, type, size)             \
do                                                   \
{                                                    \
    p = (type)sal_malloc_atomic(size);               \
    if (!p)                                          \
        sal_malloc_failed(__FILE__, __LINE__, size); \
} while (0)

/**
 * @brief Free dynamic memory
 *
 * Wrapper on sal_free(). After the memory pointed by @e p is freed, @e p is
 * assigned NULL
 *
 * @param[in] p
 *
 * @see sal_free
 */
#define SAL_FREE(p) \
do                  \
{                   \
    sal_free(p);    \
    p = NULL;       \
} while (0)

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Allocate dynamic memory
 *
 * @param[in] size
 * @retval 0    NULL if the request fails
 * @retval >0   A pointer to the allocated memory
 */
void *sal_malloc(size_t size);

void *sal_calloc(size_t size);

/**
 * @brief Allocate dynamic memory without blocking
 *
 * @param[in] size
 * @retval 0    NULL if the request fails
 * @retval >0   A pointer to the allocated memory
 */
void *sal_malloc_atomic(size_t size);

/**
 * @brief Free dynamic memory
 *
 * @param[in] p
 */
void sal_free(void *p);

/**
 * @brief Trace memory allocation failure
 *
 * @param[in] file
 * @param[in] line
 * @param[in] size
 */
void sal_malloc_failed(const char *file, int line, size_t size);

/**
 * Memory pool object
 */
typedef struct sal_mem_pool sal_mem_pool_t;

/**
 * Wrapper on sal_mem_pool_alloc(). Memory allocation failures will be traced.
 *
 * @param p
 * @param type
 * @param mem_pool
 * @param atomic
 */
#ifdef _SAL_DEBUG
#define SAL_MEM_POOL_ALLOC(p, type, mem_pool, atomic)                   \
do                                                                      \
{                                                                       \
    p = (type)sal_mem_pool_alloc(mem_pool, atomic, __FILE__, __LINE__);  \
} while (0)
#else
#define SAL_MEM_POOL_ALLOC(p, type, mem_pool, atomic)                   \
do                                                                      \
{                                                                       \
    p = (type)sal_mem_pool_alloc(mem_pool, atomic);                      \
    if (!p)                                                             \
        sal_malloc_failed(__FILE__, __LINE__, size);                    \
} while (0)
#endif

/**
 * Wrapper on sal_mem_pool_free(). After the memory pointed by @e p is freed, @e p is
 * assigned NULL
 *
 * @param mem_pool
 * @param p
 */
#define SAL_MEM_POOL_FREE(mem_pool, p)  \
do                                      \
{                                       \
    sal_mem_pool_free(mem_pool, p);     \
    p = NULL;                           \
} while (0)

int sal_mem_pool_create(sal_mem_pool_t **mem_pool, const char *name,
    size_t size, size_t align, uint32 min_nr);
void sal_mem_pool_destroy(sal_mem_pool_t *mem_pool);

#ifdef _SAL_DEBUG
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic, const char *file,
    int line);
#else
void *sal_mem_pool_alloc(sal_mem_pool_t *mem_pool, bool atomic);
#endif

void sal_mem_pool_free(sal_mem_pool_t *mem_pool, void *p);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup mem */

#endif /* !__SAL_MEM_H__ */
