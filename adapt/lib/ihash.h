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
 * @brief This file is the header file of ihash.c
 */

#ifndef IHASH_H
#define IHASH_H 1

#include "hmap.h"
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

    struct ihash_node
    {
        struct hmap_node node;
        uint32_t key;
        void *data;
    };

    struct ihash
    {
        struct hmap map;
    };

#define IHASH_INITIALIZER(IHASH) { HMAP_INITIALIZER(&(IHASH)->map) }

#define IHASH_FOR_EACH(IHASH_NODE, IHASH) \
    HMAP_FOR_EACH (IHASH_NODE, node, &(IHASH)->map)

#define IHASH_FOR_EACH_SAFE(IHASH_NODE, NEXT, IHASH) \
    HMAP_FOR_EACH_SAFE (IHASH_NODE, NEXT, node, &(IHASH)->map)

    void
    ihash_init(struct ihash *);
    void
    ihash_destroy(struct ihash *);
    void
    ihash_destroy_free_data(struct ihash *);
    void
    ihash_clear(struct ihash *);
    void
    ihash_clear_free_data(struct ihash *);
    bool
    ihash_is_empty(const struct ihash *);
    size_t
    ihash_count(const struct ihash *);
    struct ihash_node *
    ihash_add(struct ihash *, const uint32_t, const void *);
    struct ihash_node *
    ihash_add_nocopy(struct ihash *, uint32_t, const void *);
    bool
    ihash_add_once(struct ihash *, const uint32_t, const void *);
    void
    ihash_add_assert(struct ihash *, const uint32_t, const void *);
    struct ihash_node *
    ihash_find(const struct ihash *, const uint32_t);
    void
    ihash_delete(struct ihash *, struct ihash_node *);
    uint32_t
    ihash_steal(struct ihash *, struct ihash_node *);
    bool
    ihash_contains(const struct ihash *, const uint32_t);
    const struct ihash_node **
    ihash_sort(const struct ihash *);
    void
    ihash_print_value_int(struct ihash *);
    void
    ihash_print_value_ivec(struct ihash *);

#ifdef  __cplusplus
}
#endif

#endif /* ihash.h */
