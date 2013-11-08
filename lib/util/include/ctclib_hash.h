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

#ifndef _CTCLIB_HASH_H_
#define _CTCLIB_HASH_H_

#include "sal.h"

/* Default hash table size.  */
#define CTCLIB_HASHTABSIZE     1024

struct ctclib_hash_backet_s
{
    /* Linked list.  */
    struct ctclib_hash_backet_s *next;

    /* Hash key. */
    uint32 key;

    /* Data.  */
    void *data;
};
typedef struct ctclib_hash_backet_s ctclib_hash_backet_t;

struct ctclib_hash_s
{
  /* Hash bucket. */
  ctclib_hash_backet_t **index;

  /* Hash table size. */
  uint32 size;

  /* Key make function. */
  uint32 (*hash_key) (void *);

  /* Data compare function. */
  bool (*hash_cmp) (void *, void *);

  /* Bucket alloc. */
  uint32 count;
};
typedef struct ctclib_hash_s ctclib_hash_t;

ctclib_hash_t *ctclib_hash_create (uint32 (*) (void *), bool (*) (void *, void *));
ctclib_hash_t *ctclib_hash_create_size (uint32, uint32 (*) (void *), bool (*) (void *, void *));
#ifdef _USER_KERNEL_SHM_
ctclib_hash_t *ctclib_hash_shm_create_size (uint32, uint32 (*) (), bool (*) ());
void *ctclib_hash_shm_get (ctclib_hash_t *, void *, void * (*) ());
#endif
void *ctclib_hash_get (ctclib_hash_t *, void *, void * (*) (void *));

#ifdef _CENTEC_
void *ctclib_hash_get2(ctclib_hash_t *hash, void *data,  void *arg, void * (*alloc_func) (void *, void *));
#endif
void *ctclib_hash_alloc_intern (void *);
void *ctclib_hash_lookup (ctclib_hash_t *, void *);
void *ctclib_hash_release (ctclib_hash_t *, void *);

void ctclib_hash_iterate (ctclib_hash_t *,
                   void (*) (ctclib_hash_backet_t *, void *), void *);
void ctclib_hash_iterate2 (ctclib_hash_t *,
                    void (*) (ctclib_hash_backet_t *, void *, void *),
                    void *, void *);
void ctclib_hash_iterate3 (ctclib_hash_t *,
                    void (*) (ctclib_hash_backet_t *, void *, void *, void *),
                    void *, void *, void *);
void
ctclib_hash_iterate4 (ctclib_hash_t *hash,
               void (*func) (ctclib_hash_backet_t *, void *, void *, void *, void *),
               void *arg1, void *arg2, void *arg3, void *arg4);

void ctclib_hash_clean (ctclib_hash_t *, void (*) (void *));
void ctclib_hash_free (ctclib_hash_t *);

#endif /* _CTCLIB_HASH_H_ */
