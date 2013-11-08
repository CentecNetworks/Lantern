 /****************************************************************************
 *file ctc_hash.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

 The file define  HASH arithmetic lib
 ****************************************************************************/

#ifndef _CTC_HASH_H_
#define _CTC_HASH_H_
#include "kal.h"

 struct ctc_hash_backet_s
 {
     /**< Linked list.  */
     struct ctc_hash_backet_s *next;

     /**< Hash key. */
     uint32 key;

     /**< Data.  */
     void *data;
 };
typedef struct ctc_hash_backet_s ctc_hash_backet_t;

struct ctc_hash_s
{
    /**< Hash backet. */
    ctc_hash_backet_t ***index;

    /**< Hash table size= block_size*block_num */

    /**< Hash table block number. */
    uint16 block_num;

    /**< Hash table block size. */
    uint16 block_size;

    /**< current Hash backet size. */
    uint32 count;

    /**< Key make function. */
    uint32 (*hash_key) (void *data);

    /**< Data compare function. */
    bool (*hash_cmp) (void *backet_data , void *data);

};
typedef struct ctc_hash_s ctc_hash_t;

typedef int32 (*hash_traversal_fn)(void *backet_data, void *user_data);

/**< Create hash table*/
extern ctc_hash_t *
ctc_hash_create(uint16 block_num,uint16 block_size, uint32 (*hash_key) (), bool(*hash_cmp) ());

/**< Lookup a node from hash table */
extern inline void *
ctc_hash_lookup (ctc_hash_t* hash, void *data);

extern void *
ctc_hash_lookup2 (ctc_hash_t *hash, void *data, uint32 *hash_key);

/**< Traverse hash table */
extern int32
ctc_hash_traverse(ctc_hash_t* hash, hash_traversal_fn fn,void *data);

/**< Traverse hash special array according to KEY */
extern int32
ctc_hash_traverse2(ctc_hash_t* hash, hash_traversal_fn fn,void *data);
extern int32
ctc_hash_traverse_through(ctc_hash_t* hash, hash_traversal_fn fn,void *data);

/**< Insert a node to hash table */
extern inline void *
ctc_hash_insert(ctc_hash_t* hash, void *data);

extern int32
ctc_hash_get_count (ctc_hash_t *hash, uint32* count);

/**< Remove a node from hash table */
extern inline void *
ctc_hash_remove(ctc_hash_t* hash, void *data);

/**< remove  node from hash table  */
extern void
ctc_hash_traverse_remove(ctc_hash_t *hash, hash_traversal_fn fn, void *data);

extern void
ctc_hash_traverse2_remove(ctc_hash_t* hash, hash_traversal_fn fn,void *data);

extern void
ctc_hash_free(ctc_hash_t* hash);

#endif


