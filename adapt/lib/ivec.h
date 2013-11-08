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
 * @file
 * @brief This file is the header file of ivec.c
 */

#ifndef IVEC_H
#define IVEC_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

    struct ivec
    {
        uint32_t *values;
        size_t n;
        size_t allocated;
    };

#define IVEC_EMPTY_INITIALIZER { NULL, 0, 0 }

    void
    ivec_init(struct ivec *);
    void
    ivec_destroy(struct ivec *);
    void
    ivec_clear(struct ivec *);
    bool
    ivec_is_empty(const struct ivec *);
    void
    ivec_add(struct ivec *, const uint32_t);
    void
    ivec_add_nocopy(struct ivec *, uint32_t);
    void
    ivec_del(struct ivec *, const uint32_t);
    void
    ivec_sort(struct ivec *);
    bool
    ivec_contains(const struct ivec *, const uint32_t);
    size_t
    ivec_find(const struct ivec *, const uint32_t);
    int
    ivec_find_value(const struct ivec *, const uint32_t, uint32_t *, uint32_t *);
    bool
    ivec_is_sorted(const struct ivec *);
    int
    ivec_get_lower(const struct ivec *, const uint32_t, uint32_t *);
    int
    ivec_get_higher(const struct ivec *, const uint32_t, uint32_t *);
    uint32_t
    ivec_get_last(const struct ivec *);
    void
    ivec_print(const struct ivec *);

#define IVEC_FOR_EACH(INDEX, VALUE, IVEC)        \
    for ((INDEX) = 0;                           \
         ((INDEX) < (IVEC)->n                   \
          ? (VALUE) = (IVEC)->values[INDEX], 1    \
          : 0);                                 \
         (INDEX)++)

#ifdef  __cplusplus
}
#endif

#endif /* ivec.h */
