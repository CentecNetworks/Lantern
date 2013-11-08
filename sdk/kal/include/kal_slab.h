#ifndef __KAL_SLAB_H__
#define __KAL_SLAB_H__

#ifndef _KAL_VXWORKS
typedef struct kal_slab_list_pointer_node_s
{
    struct kal_slab_list_pointer_node_s* p_next;
    struct kal_slab_list_pointer_node_s* p_prev;
} kal_slab_list_pointer_node_t;

typedef struct
{
    kal_slab_list_pointer_node_t head;
    uint32      count;
} kal_slab_list_pointer_t;

#define _KAL_SLAB_LHEAD(p_list) (p_list->head.p_next)
#define _KAL_SLAB_LTAIL(p_list) (p_list->head.p_prev)

#define _kal_slab_container_of(p, type, member) \
(type *)((int8 *)p - (int32)&((type *)0)->member)

typedef struct mem_cache mem_cache_t;

void mem_cache_init(void);

mem_cache_t *mem_cache_create(int obj_size);

void mem_cache_destroy(mem_cache_t *cache);

void *mem_cache_alloc(mem_cache_t *cache);

void mem_cache_free(/*mem_cache_t *cache, */void *p);

typedef struct mem_slab
{
    kal_slab_list_pointer_node_t list_node;
    mem_cache_t *cache;
    void *vm;
    void *left;
    int32 used;
    int32 limit;
    void *free_list;
} mem_slab_t;

struct mem_cache
{
    kal_slab_list_pointer_t full_slabs;
    kal_slab_list_pointer_t free_slabs;
    int32 obj_size;
    int32 slab_size;
};

static INLINE mem_slab_t *vm_to_slab(void *p)
{
    return *(mem_slab_t **)((int32 )p - sizeof(mem_slab_t *));
}
static INLINE int mem_cache_get_obj_size(mem_cache_t *cache)
{
    return cache->obj_size;
}

static INLINE int mem_cache_get_obj_size_by_ptr(void *p)
{
    mem_slab_t *slab;

    slab = vm_to_slab(p);

    return slab->cache->obj_size;
}
#endif

#endif /* !__CTC_SLAB_H__ */
