#ifndef __KAL_MEM_H__
#define __KAL_MEM_H__

#ifndef _KAL_VXWORKS
/**
 * @file kal_mem.h
 */

/**
 * @defgroup mem Memory Allocation
 * @{
 */

/**
 * @brief Allocate dynamic memory
 *
 * Wrapper on kal_malloc(). Memory allocation failures will be traced.
 *
 * @param[out] p
 * @param[in]  type
 * @param[in]  size
 *
 * @see kal_malloc
 */
#define KAL_MALLOC(p, type, size)              \
    {                                                    \
        p = kal_malloc(size);                      \
    }

/**
 * @brief Allocate dynamic memory without blocking
 *
 * Wrapper on kal_malloc_atomic(). Memory allocation failures will be traced.
 *
 * @param[out] p
 * @param[in]  type
 * @param[in]  size
 *
 * @see kal_malloc_atomic
 */
#define KAL_MALLOC_ATOMIC(p, type, size)             \
    {                                                    \
        p = (type)kal_malloc(size); \
    }

/**
 * @brief Free dynamic memory
 *
 * Wrapper on kal_free(). After the memory pointed by @e p is freed, @e p is
 * assigned NULL
 *
 * @param[in] p
 *
 * @see kal_free
 */
#define KAL_FREE(p) \
    {                   \
        kal_free(p);    \
        p = NULL;       \
    }

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Trace memory allocation failure
 *
 * @param[in] file
 * @param[in] line
 * @param[in] size
 */
void kal_malloc_failed(const char *file, int line, size_t size);

/**
 * Memory pool object
 */
typedef struct kal_mem_pool kal_mem_pool_t;

/**
 * Wrapper on kal_mem_pool_alloc(). Memory allocation failures will be traced.
 *
 * @param p
 * @param type
 * @param mem_pool
 * @param atomic
 */
#ifdef _KAL_DEBUG
#define KAL_MEM_POOL_ALLOC(p, type, mem_pool, atomic)                   \
    {                                                                       \
        p = (type)kal_mem_pool_alloc(mem_pool, atomic, __FILE__, __LINE__);  \
    }
#else
#define KAL_MEM_POOL_ALLOC(p, type, mem_pool, atomic)                   \
    {                                                                       \
        p = (type)kal_mem_pool_alloc(mem_pool, atomic);                      \
        if (!p)                                                             \
            kal_malloc_failed(__FILE__, __LINE__, size);                    \
    }
#endif

/**
 * Wrapper on kal_mem_pool_free(). After the memory pointed by @e p is freed, @e p is
 * assigned NULL
 *
 * @param mem_pool
 * @param p
 */
#define KAL_MEM_POOL_FREE(mem_pool, p)  \
    {                                       \
        kal_mem_pool_free(mem_pool, p);     \
        p = NULL;                           \
    }

int kal_mem_pool_create(kal_mem_pool_t **mem_pool, const char *name,
	size_t size, size_t align, uint32 min_nr);
void kal_mem_pool_destroy(kal_mem_pool_t *mem_pool);

#ifdef _KAL_DEBUG
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic, const char *file,
	int line);
#else
void *kal_mem_pool_alloc(kal_mem_pool_t *mem_pool, bool atomic);
#endif

void kal_mem_pool_free(kal_mem_pool_t *mem_pool, void *p);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup mem */
#endif
#endif /* !__KAL_MEM_H__ */

