#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>

#include "kal.h"
#include "kal_slab.h"

#define M_SLAB_SHIFT 16
#define M_SLAB_SIZE  (1 << M_SLAB_SHIFT)
#define M_SLAB_MASK  (~(M_SLAB_SIZE - 1))

#define M_ALIGN(n) (((n) + 3) & ~3)

#define M_ASSERT(expr)


static mem_cache_t internal_cache;

static void kal_slab_list_pointer_delete(kal_slab_list_pointer_t* p_list, kal_slab_list_pointer_node_t* p_node)
{
    if (p_node->p_next != NULL)
        p_node->p_next->p_prev = p_node->p_prev;
    else
        _KAL_SLAB_LTAIL(p_list) = p_node->p_prev;
    p_node->p_prev->p_next = p_node->p_next;

     if ( p_list->count != 0)
    {
        p_list->count --;
    }
}

static int kal_slab_list_pointer_empty(kal_slab_list_pointer_t* p_list)
{
    return _KAL_SLAB_LHEAD(p_list) == NULL;
}

static kal_slab_list_pointer_node_t* kal_slab_list_pointer_head(kal_slab_list_pointer_t* p_list)
{
    return _KAL_SLAB_LHEAD(p_list);
}

static void kal_slab_list_pointer_init(kal_slab_list_pointer_t* p_list)
{
    _KAL_SLAB_LHEAD(p_list) = NULL;
    _KAL_SLAB_LTAIL(p_list) = &p_list->head;
    p_list->count = 0;
}

static void kal_slab_list_pointer_insert_head(kal_slab_list_pointer_t* p_list, kal_slab_list_pointer_node_t* p_node)
{
    if ((p_node->p_next = _KAL_SLAB_LHEAD(p_list)) != NULL)
        p_node->p_next->p_prev = p_node;
    else
        _KAL_SLAB_LTAIL(p_list) = p_node;
    _KAL_SLAB_LHEAD(p_list) = p_node;
    p_node->p_prev = &p_list->head;

    p_list->count ++;
}

static void kal_slab_list_pointer_insert_tail(kal_slab_list_pointer_t* p_list, kal_slab_list_pointer_node_t* p_node)
{
    p_node->p_next = NULL;
    p_node->p_prev = _KAL_SLAB_LTAIL(p_list);
    _KAL_SLAB_LTAIL(p_list)->p_next = p_node;
    _KAL_SLAB_LTAIL(p_list) = p_node;
    p_list->count ++;
}

static INLINE void *vm_alloc(int32 size)
{
    return mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

static INLINE void vm_free(void *p, int32 size)
{
    munmap(p, size);
}

static void alloc_slab(mem_cache_t *cache)
{
    mem_slab_t *slab;
    void *vm;
    int32 vm_limit;

    vm = vm_alloc(cache->slab_size/*M_SLAB_SIZE*/);
    if (!vm || vm == MAP_FAILED)
    {
        return;
    }

    if (cache == &internal_cache) {
        vm_limit = cache->slab_size/*M_SLAB_SIZE*/ - M_ALIGN(sizeof(mem_slab_t));
        slab = (mem_slab_t *)(vm + vm_limit);
    }
    else {
        vm_limit = cache->slab_size/*M_SLAB_SIZE*/;
        slab = mem_cache_alloc(&internal_cache);

        if (!slab) {
            vm_free(vm, cache->slab_size/*M_SLAB_SIZE*/);
            return;
        }
    }

    slab->cache = cache;
    slab->vm = vm;
    slab->left = vm;
    slab->used = 0;
    slab->limit = vm_limit / M_ALIGN(cache->obj_size + sizeof(mem_slab_t *));
    slab->free_list = NULL;

    kal_slab_list_pointer_insert_tail(&cache->free_slabs, &slab->list_node);

}

static void free_slab(mem_cache_t *cache, mem_slab_t *slab)
{
    vm_free(slab->vm, cache->slab_size/*M_SLAB_SIZE*/);
    if (cache != &internal_cache)
        mem_cache_free(/*&internal_cache, */slab);
}

void mem_cache_init()
{
    union __uu {
        mem_cache_t cache;
        mem_slab_t slab;
    };


    kal_slab_list_pointer_init(&internal_cache.full_slabs);
    kal_slab_list_pointer_init(&internal_cache.free_slabs);
    internal_cache.obj_size = sizeof(union __uu);
    internal_cache.slab_size = M_SLAB_SIZE;
}

mem_cache_t *mem_cache_create(int32 obj_size)
{
    mem_cache_t *cache;

    cache = mem_cache_alloc(&internal_cache);
    if (cache) {
        kal_slab_list_pointer_init(&cache->full_slabs);
        kal_slab_list_pointer_init(&cache->free_slabs);
        cache->obj_size = obj_size;

        if (obj_size < 0x1000)
            cache->slab_size = M_SLAB_SIZE;
        else
            cache->slab_size = obj_size + sizeof(mem_slab_t *);
    }

    return cache;
}

void mem_cache_destroy(mem_cache_t *cache)
{
    M_ASSERT(kal_slab_list_pointer_empty(&cache->full_slabs));
    M_ASSERT(kal_slab_list_pointer_empty(&cache->free_slabs));
    mem_cache_free(/*&internal_cache, */cache);
}

void *mem_cache_alloc(mem_cache_t *cache)
{
    mem_slab_t *slab;
    void *p;

    if (kal_slab_list_pointer_empty(&cache->free_slabs))
        alloc_slab(cache);

    if (kal_slab_list_pointer_empty(&cache->free_slabs))
        return NULL;

    slab = _kal_slab_container_of(kal_slab_list_pointer_head(&cache->free_slabs), mem_slab_t, list_node);
    if (slab->free_list) {
        p = slab->free_list;
        slab->free_list = *(void **)p;
    }
    else {
        p = slab->left;
        *(mem_slab_t **)p = slab;
        p += sizeof(mem_slab_t *);
        slab->left += M_ALIGN(cache->obj_size + sizeof(mem_slab_t *));
    }

    slab->used++;
    if (slab->used == slab->limit) {
        kal_slab_list_pointer_delete(&cache->free_slabs, &slab->list_node);
        kal_slab_list_pointer_insert_tail(&cache->full_slabs, &slab->list_node);
    }

    return p;
}

void mem_cache_free(/*mem_cache_t *cache, */void *p)
{
    mem_slab_t *slab;
    mem_cache_t *cache;


    slab = vm_to_slab(p);
    M_ASSERT(slab);
    cache = slab->cache;

    *(void **)p = slab->free_list;
    slab->free_list = p;
    slab->used--;

    if (slab->used == slab->limit - 1) {
        kal_slab_list_pointer_delete(&cache->full_slabs, &slab->list_node);
        kal_slab_list_pointer_insert_head(&cache->free_slabs, &slab->list_node);
    }

    if (slab->used == 0) {
        kal_slab_list_pointer_delete(&cache->free_slabs, &slab->list_node);
        free_slab(cache, slab);
    }
}



