/****************************************************************************
 *file ctc_avl_tree.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

 The file define  avl-tree arithmetic lib
 ****************************************************************************/

#ifndef _CTC_AVL_TREE_H_
#define _CTC_AVL_TREE_H_

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @addtogroup avltree AVLTREE
 @{
*/

/**
 @brief Define avl tree node
*/
 typedef struct ctc_avl_node
 {
     struct ctc_avl_node *left;     /**< Left node. */
     struct ctc_avl_node *right;    /**< Right node. */
     struct ctc_avl_node *parent;   /**< Parent node. */
     int32 balance;                 /**< Balance factor. */
     int32 rank;                    /**< Relative position of node in own subtree i.e. number of nodes in left subtree + 1. */
     int32 flag;                    /**< 0 -> left child of its parent or is root of the tree. 1 -> right child of its parent. */
     void *info;                    /**< Data. */
 } ctc_avl_node_t;

/**
 @brief Define avl tree
*/
typedef struct ctc_avl_tree
{
  int32 max_nodes ;                                     /**< Maximum number of bytes for data. */
  int32 (*compare_function) (void *data1, void *data2); /**< Compare function. */
  char *data;                                           /**< Array for preallocated entries. */
  struct ctc_avl_node *root;                            /**< Root. */
  struct ctc_avl_node *free_list;                       /**< Free list. */
  int32 count;                                          /**< Number of entries in tree. */
} ctc_avl_tree_t;

typedef int32 (*avl_traversal_fn)(void *data, void *user_data);
typedef int32 (*avl_traversal2_fn)(void *data, void *user_data1,
                                 void *user_data2);
typedef int32 (*avl_traversal3_fn)(void *data, void *user_data1,
                                 void *user_data2, void *user_data3);
#define CTC_AVL_NODE_INFO(n)       (n)->info
#define CTC_AVL_COUNT(t)           (t)->count
#define CTC_AVL_NODE_LEFT(n)       (n)->left
#define CTC_AVL_NODE_RIGHT(n)      (n)->right

#define CTC_AVL_TREE_LOOP(T,V,N)                              \
  if (T)                                                  \
    for ((N) = avl_top ((T)); (N); (N) = avl_next ((N)))  \
      if (((V) = (N)->info) != NULL)


#define CTC_AVL_LOOP_DEL(T,V,N) \
  if (T) \
    for ((N) = avl_top (T); N ;  \
         (N) = avl_top (T))\
      if (((V) = (N)->info) != NULL)

/**< Function declarations. */

/**< Delete node from AVL tree. */
int32 ctc_avl_delete_node (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *node);

/**< If the max_nodes is 0, no preallocation is done. Every node is allocated first from the free_list and then from the system memory. */
int32 ctc_avl_create (struct ctc_avl_tree **ctc_avl_tree,
                      int32 max_nodes,
                      int32 (*compare_function) (void *data1, void *data2));

/**< Traverse tree. */
int32 ctc_avl_traverse (struct ctc_avl_tree *ctc_avl_tree, avl_traversal_fn fn, void *data);

int32 ctc_avl_traverse2 (struct ctc_avl_tree *ctc_avl_tree, avl_traversal2_fn fn,
                         void *data1, void *data2);

int32 ctc_avl_traverse3 (struct ctc_avl_tree *ctc_avl_tree, avl_traversal3_fn fn,
                         void *data1, void *data2, void *data3);

/**< Tree left bottom node */
struct ctc_avl_node *
ctc_avl_left_bottom_node (struct ctc_avl_tree *ctc_avl_tree,struct ctc_avl_node *left_node);
/**< Tree right bottom node . */
struct ctc_avl_node *
ctc_avl_right_bottom_node (struct ctc_avl_tree *ctc_avl_tree,struct ctc_avl_node *right_node);

/**< Tree top. */
struct ctc_avl_node *ctc_avl_top (struct ctc_avl_tree *ctc_avl_tree);

/**< Get next entry in AVL tree. */
struct ctc_avl_node* ctc_avl_getnext (struct ctc_avl_tree *ctc_avl_tree, struct ctc_avl_node *node);

/**< Delete AVL node containing the data. */
int32 ctc_avl_remove (struct ctc_avl_tree *ctc_avl_tree, void *data);

/**< Insert a node in AVL tree. */
int32 ctc_avl_insert (struct ctc_avl_tree *ctc_avl_tree, void *data);


/**< Lookup AVL tree. */
struct ctc_avl_node *ctc_avl_search (struct ctc_avl_tree *ctc_avl_tree, void *data);

/**< Get next node. */
struct ctc_avl_node* ctc_avl_next (struct ctc_avl_node *node);

/**< Get  AVL tree node count. */
int32 ctc_avl_get_tree_size(struct ctc_avl_tree *p_avl_tree);

/**< Tree cleanup. Remove all nodes from tree but do not free the tree. */
int32 ctc_avl_tree_cleanup (struct ctc_avl_tree *ctc_avl_tree, void (*avl_data_free)(void *ptr));

/**< Free AVL tree . */
int32 ctc_avl_tree_free (struct ctc_avl_tree **pp_avl_tree, void (*avl_data_free)(void *ptr));

void* ctc_avl_lookup_min (struct ctc_avl_tree *tree);
void* ctc_avl_lookup_max (struct ctc_avl_tree *tree);

/**@} end of @defgroup avltree  */

#endif /* _AVL_TREE_H_ */
