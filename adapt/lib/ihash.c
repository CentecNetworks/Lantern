/** Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @file
 * @brief This file implements a hashmap database (with integer key) and its operations
 */

#include <config.h>
#include <assert.h>
#include "ihash.h"
#include "hash.h"
#include "ofp_types.h"
#include "ofp_macro.h"
#include "vlog.h"

VLOG_DEFINE_THIS_MODULE(ofproto_ihash);

static struct ihash_node *
ihash_find__(const struct ihash *, const uint32_t key, size_t hash);

static size_t
hash_key(uint32_t key)
{
    return hash_int(key, 0);
}

void
ihash_init(struct ihash *sh)
{
    hmap_init(&sh->map);
}

void
ihash_destroy(struct ihash *sh)
{
    if (sh)
    {
        ihash_clear(sh);
        hmap_destroy(&sh->map);
    }
}

/* Like ihash_destroy(), but also free() each node's 'data'. */
void
ihash_destroy_free_data(struct ihash *sh)
{
    if (sh)
    {
        ihash_clear_free_data(sh);
        hmap_destroy(&sh->map);
    }
}

void
ihash_clear(struct ihash *sh)
{
    struct ihash_node *node, *next;

    IHASH_FOR_EACH_SAFE (node, next, sh)
    {
        hmap_remove(&sh->map, &node->node);
        /* free(node->name); */
        free(node);
    }
}

/* Like ihash_clear(), but also free() each node's 'data'. */
void
ihash_clear_free_data(struct ihash *sh)
{
    struct ihash_node *node, *next;

    IHASH_FOR_EACH_SAFE (node, next, sh)
    {
        hmap_remove(&sh->map, &node->node);
        free(node->data);
        /* free(node->name); */
        free(node);
    }
}

bool
ihash_is_empty(const struct ihash *ihash)
{
    return hmap_is_empty(&ihash->map);
}

size_t
ihash_count(const struct ihash *ihash)
{
    return hmap_count(&ihash->map);
}

static struct ihash_node *
ihash_add_nocopy__(struct ihash *sh, uint32_t key, const void *data, size_t hash)
{
    struct ihash_node *node = xmalloc(sizeof *node);
    node->key = key;
    node->data = (void *) data;
    hmap_insert(&sh->map, &node->node, hash);
    return node;
}

/* It is the caller's responsibility to avoid duplicate names, if that is
 * desirable. */
struct ihash_node *
ihash_add_nocopy(struct ihash *sh, uint32_t key, const void *data)
{
    return ihash_add_nocopy__(sh, key, data, hash_key(key));
}

/* It is the caller's responsibility to avoid duplicate key, if that is
 * desirable. */
struct ihash_node *
ihash_add(struct ihash *sh, const uint32_t key, const void *data)
{
    return ihash_add_nocopy(sh, key, data);
}

bool
ihash_add_once(struct ihash *sh, const uint32_t key, const void *data)
{
    if (!ihash_find(sh, key))
    {
        ihash_add(sh, key, data);
        return true;
    }
    else
    {
        return false;
    }
}

void
ihash_add_assert(struct ihash *sh, const uint32_t key, const void *data)
{
    bool added OVS_UNUSED = ihash_add_once(sh, key, data);
    assert(added);
}

/* Deletes 'node' from 'sh' and frees the node's name.  The caller is still
 * responsible for freeing the node's data, if necessary. */
void
ihash_delete(struct ihash *sh, struct ihash_node *node)
{
    /* free(ihash_steal(sh, node)); */
    ihash_steal(sh, node);
}

/* Deletes 'node' from 'sh'.  Neither the node's name nor its data is freed;
 * instead, ownership is transferred to the caller.  Returns the node's
 * name. */
uint32_t
ihash_steal(struct ihash *sh, struct ihash_node *node)
{
    uint32_t key = node->key;

    hmap_remove(&sh->map, &node->node);
    free(node);
    return key;
}

bool
ihash_contains(const struct ihash *sh, const uint32_t key)
{
    if (ihash_find(sh, key) == NULL)
    {
        return false;
    }
    return true;
}

static struct ihash_node *
ihash_find__(const struct ihash *sh, const uint32_t key, size_t hash)
{
    struct ihash_node *node;

    HMAP_FOR_EACH_WITH_HASH (node, node, hash, &sh->map)
    {
        if (node->key == key)
        {
            return node;
        }
    }
    return NULL;
}

/* If there are duplicates, returns a random element. */
struct ihash_node *
ihash_find(const struct ihash *sh, const uint32_t key)
{
    return ihash_find__(sh, key, hash_key(key));
}

static int
compare_ints(const void *a_, const void *b_)
{
    return *((uint32_t*) a_) - *((uint32_t*) b_);
}

const struct ihash_node **
ihash_sort(const struct ihash *sh)
{
    if (ihash_is_empty(sh))
    {
        return NULL;
    }
    else
    {
        const struct ihash_node **nodes;
        struct ihash_node *node;
        size_t i, n;

        n = ihash_count(sh);
        nodes = xmalloc(n * sizeof *nodes);
        i = 0;
        IHASH_FOR_EACH (node, sh)
        {
            nodes[i++] = node;
        }assert(i == n);

        qsort(nodes, n, sizeof *nodes, compare_ints);

        return nodes;
    }
}

void
ihash_print_value_int(struct ihash *sh)
{
    uint32_t key = 0;
    uint32_t data = 0;
    int i = 0;
    struct ihash_node *node, *next;
    IHASH_FOR_EACH_SAFE (node, next, sh)
    {
        key = node->key;
        data = *((uint32_t *) (node->data));
        OFP_DEBUG_PRINT("i: %u key: %u value: %u\n", i, key, data);
        i++;
    }
}
