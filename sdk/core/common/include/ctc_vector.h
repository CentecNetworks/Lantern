 /****************************************************************************
 *file ctc_vector.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-11

 *version v2.0

 Vector containers are implemented as dynamic arrays; Just as regular arrays, vector containers
  can use offsets on regular pointers to elements
 ****************************************************************************/


#ifndef _CTC_VECTOR_H
#define _CTC_VECTOR_H
/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include  "kal.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/* define Vector structure*/
struct ctc_vector_s
{
  uint32  block_size;       /**< vector block size */
  uint16  reserved1;        /**< Reserved1 */
  uint8   max_block_num;    /**< vector block max num */
  uint8   reserved2;        /**< Reserved2 */
};
typedef struct ctc_vector_s  ctc_vector_t;

typedef int32 (*vector_traversal_fn)(void *array_data, void *user_data);

/*
 *brief    Create a Vector structure

 *param[in] max_block_num    the max number of vector's block
 *param[in] block_size       the size  of  per vector's block
 *return  ctc_vector_t *     return the pointer of  vector

*/
extern ctc_vector_t *
ctc_vector_init (uint8 max_block_num, uint32 block_size);

/*
 *brief    Release a Vector

 *param[in] vec    a Vector structure
 *return    void
*/

extern void
ctc_vector_release(ctc_vector_t* vec);

/*
 *brief    Traverse a Vector

 *param[in] vec    a Vector structure

 *param[in] fn     The user function using traverse the vector

 *param[in] data   The user data using traverse the vector

 *return    void
*/

extern int32
ctc_vector_traverse (ctc_vector_t *vec, vector_traversal_fn fn, void *data);

/*
 *brief    Reserve some space Vector

 *param[in] vec         a Vector structure
 *param[in] block_num   the reserved number of vector's block
 *return    void
*/
extern void
ctc_vector_reserve (ctc_vector_t* vec, uint8 block_num );

/*
 *brief    Get a data pointer from a Vector

 *param[in] vec    a Vector structure
 *param[in] index    vector's index
 *return    void *
*/
extern void *
ctc_vector_get (ctc_vector_t* vec, uint32 index );
/*
 *brief    Delete a data pointer to a Vector

 *param[in] vec    a Vector structure
 *param[in] index  vector's index
 *return    void
*/
extern void *
ctc_vector_del (ctc_vector_t* vec, uint32 index);

/*
 *brief    Add a data pointer to a Vector

 *param[in] vec     a Vector structure
 *param[in] index   vector's index
 *param[in] data    data pointer
 *return TURE/FALSE
*/
extern  bool
ctc_vector_add(ctc_vector_t* vec, uint32 index, void* data);

 /**
 *brief    Return  a block allocated status

 *param[in] vec    a Vector structure
 *param[in] block_index  block's index
 *return    TRUE/FALSE
*/
extern bool
ctc_vector_get_block_status(ctc_vector_t* vec, uint8 block_index );

 /*
 *brief    Return the number of  stored member

 *param[in] vec      a Vector structure
 *param[in] block_index    block's index
 *return   uint32
*/
extern uint32
ctc_vector_get_size(ctc_vector_t* vec, uint8 block_index );
#endif /* _CTC_VECTOR_H */
