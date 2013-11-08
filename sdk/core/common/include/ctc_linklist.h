/****************************************************************************
 *file ctc_linklist.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

  This file contains  single/double linklist arithmetic lib
 ****************************************************************************/

#ifndef _LINKLIST_H_
#define _LINKLIST_H_

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
/*
 * ************************  Double-linked List   ***************************
 */
typedef void (*ctc_list_del_cb_t) (void *val);
typedef int32 (*ctc_list_cmp_cb_t) (void *val1, void *val2);

/**
 @brief  Define list node
*/
typedef struct ctc_listnode
{
    struct ctc_listnode *next;      /**< pointer on struct type ctc_listnode */
    struct ctc_listnode *prev;      /**< pointer on struct type ctc_listnode */
    void *data;                     /**< void type pointer */
} ctc_listnode_t;

/**
 @brief  Define linklist
*/
typedef struct ctc_linklist
{
    struct ctc_listnode *head;  /**< pointer on struct type ctc_listnode */
    struct ctc_listnode *tail;  /**< pointer on struct type ctc_listnode */
    uint32 count;               /**< unsigned 32-bit variable */
    ctc_list_cmp_cb_t cmp;      /**< ctc_list_cmp_cb_t type variable */
    ctc_list_del_cb_t del;      /**< ctc_list_cmp_cb_t type variable */
} ctc_linklist_t;

#define CTC_NEXTNODE(X) ((X) = (X)->next)
#define CTC_LISTHEAD(X) (((X) != NULL) ? ((X)->head) : NULL)
#define CTC_LISTTAIL(X) (((X) != NULL) ? ((X)->tail) : NULL)
#define CTC_LISTCOUNT(X) (((X) != NULL) ? ((X)->count) : 0)
#define ctc_listcount(X) (((X) != NULL) ? ((X)->count) : 0)
#define CTC_LIST_ISEMPTY(X) ((X)->head == NULL && (X)->tail == NULL)
#define ctc_list_isempty(X) ((X)->head == NULL && (X)->tail == NULL)
#define CTC_GETDATA(X) (((X) != NULL) ? (X)->data : NULL)

/* Prototypes. */
struct ctc_linklist *ctc_list_new(void);
struct ctc_linklist *ctc_list_create(ctc_list_cmp_cb_t cmp_cb, ctc_list_del_cb_t del_cb);

void ctc_list_free (struct ctc_linklist *);

struct ctc_listnode *ctc_listnode_add (struct ctc_linklist *, void *);
struct ctc_listnode *ctc_listnode_add_sort (struct ctc_linklist *, void *);
struct ctc_listnode *ctc_listnode_add_head (struct ctc_linklist *, void *);
struct ctc_listnode *ctc_listnode_add_tail (struct ctc_linklist *, void *);

void ctc_listnode_delete (struct ctc_linklist *, void *);
void ctc_listnode_delete_node (struct ctc_linklist *, ctc_listnode_t *);
struct ctc_listnode *ctc_listnode_lookup (struct ctc_linklist *, void *);
void ctc_list_delete (struct ctc_linklist *);
void ctc_list_delete_all_node (struct ctc_linklist *);


/* List iteration macro. */
#define CTC_LIST_LOOP(L,V,N) \
  if (L) \
    for ((N) = (L)->head; (N); (N) = (N)->next) \
      if (((V) = (N)->data) != NULL)

/* List reverse iteration macro. */
#define CTC_LIST_REV_LOOP(L,V,N) \
  if (L) \
    for ((N) = (L)->tail; (N); (N) = (N)->prev) \
      if (((V) = (N)->data) != NULL)

/* List reverse iteration macro. */
#define CTC_LIST_REV_LOOP_DEL(L,V,N,NN) \
  if (L) \
    for ((N) = (L)->tail, NN = ((N)!=NULL) ? (N)->prev : NULL; \
         (N); \
         (N) = (NN), NN = ((N)!=NULL) ? (N)->prev : NULL) \
      if (((V) = (N)->data) != NULL)

/* List iteration macro.
 * It allows to delete N and V in the middle of the loop
 */
#define CTC_LIST_LOOP_DEL(L,V,N,NN) \
  if (L) \
    for ((N) = (L)->head, NN = ((N)!=NULL) ? (N)->next : NULL;  \
         (N);                                                   \
         (N) = (NN), NN = ((N)!=NULL) ? (N)->next : NULL)       \
      if (((V) = (N)->data) != NULL)


/* List node add macro.  */
#define CTC_LISTNODE_ADD(L,N) \
  { \
    (N)->next = NULL; \
    (N)->prev = (L)->tail; \
    if ((L)->head == NULL) \
      (L)->head = (N); \
    else \
      (L)->tail->next = (N); \
    (L)->tail = (N); \
  }

/* List node delete macro.  */
#define CTC_LISTNODE_DELETE(L,N) \
  { \
    if ((N)->prev) \
      (N)->prev->next = (N)->next; \
    else \
      (L)->head = (N)->next; \
    if ((N)->next) \
      (N)->next->prev = (N)->prev; \
    else \
      (L)->tail = (N)->prev; \
  }


/*
 * ************************  Double-linked List ,and don't store none data   ***************************
 */
 /**
 @brief  Define ipuc list pointer node
*/
 typedef struct ctc_list_pointer_node_s
 {
     struct ctc_list_pointer_node_s* p_next;    /**< ctc_list_pointer_node_s type pointer */
     struct ctc_list_pointer_node_s* p_prev;    /**< ctc_list_pointer_node_s type pointer */
 } ctc_list_pointer_node_t;

/**
 @brief  Define list pointer
*/
 typedef struct
 {
     ctc_list_pointer_node_t head;  /**< ctc_list_pointer_node_t type variable */
     uint32      count;             /**< unsigned 32-bit variable */
 } ctc_list_pointer_t;

/* Define a list */
#define _CTC_LIST_DEF(list) ctc_list_pointer_node_t list = {{NULL, &list.head}}

#define _CTC_LHEAD(p_list) ((p_list)->head.p_next)
#define _CTC_LTAIL(p_list) ((p_list)->head.p_prev)

INLINE void
ctc_list_pointer_init(ctc_list_pointer_t* p_list);

INLINE void
ctc_list_pointer_insert_head(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node);
INLINE void
ctc_list_pointer_insert_tail(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node);

INLINE void
ctc_list_pointer_insert_after(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node1, ctc_list_pointer_node_t* p_node2);
INLINE void
ctc_list_pointer_insert_before(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node1, ctc_list_pointer_node_t* p_node2);

INLINE ctc_list_pointer_node_t*
ctc_list_pointer_delete_head(ctc_list_pointer_t* p_list);

INLINE ctc_list_pointer_node_t*
ctc_list_pointer_delete_tail(ctc_list_pointer_t* p_list);

INLINE void
ctc_list_pointer_delete(ctc_list_pointer_t* p_list, ctc_list_pointer_node_t* p_node);
INLINE int
ctc_list_pointer_empty(ctc_list_pointer_t* p_list);
INLINE ctc_list_pointer_node_t*
ctc_list_pointer_head(ctc_list_pointer_t* p_list);


INLINE ctc_list_pointer_node_t*
ctc_list_pointer_node_tail(ctc_list_pointer_t* p_list);

INLINE ctc_list_pointer_node_t*
ctc_list_pointer_next(ctc_list_pointer_node_t* p_node);
INLINE ctc_list_pointer_node_t*
ctc_list_pointer_prev(ctc_list_pointer_node_t* p_node);

#define CTC_LIST_POINTER_ISEMPTY(X) (((X)->head.p_next == NULL) && ((X)->head.p_prev == &((X)->head)))

#define CTC_LIST_POINTER_COUNT(X) (((X) != NULL) ? ((X)->count) : 0)

#define CTC_LIST_POINTER_LOOP(p_node, p_list) \
  if (p_list) \
for (p_node = ctc_list_pointer_head(p_list); p_node; p_node = ctc_list_pointer_next(p_node))

/* List iteration macro.
 * It allows to delete N and V in the middle of the loop
 */
#define CTC_LIST_POINTER_LOOP_DEL(p_node,p_next_node, p_list) \
if (p_list) \
for (p_node = ctc_list_pointer_head(p_list), p_next_node = ((p_node)!=NULL) ? (p_node)->p_next : NULL;\
p_node;p_node = p_next_node, p_next_node = ((p_node)!=NULL) ? (p_node)->p_next : NULL)


#define CTC_LIST_POINTER_LOOP_R(p_node, p_list) \
if (p_list) \
for (p_node = ctc_list_pointer_node_tail(p_list); p_node; p_node = ctc_list_pointer_prev(p_node))

#define _ctc_container_of(p, type, member) \
(type *)((char *)p - (int)&((type *)0)->member)

/*
 * ************************  single-linked List   ***************************
 */

/**
 @brief  Define slist node
*/
typedef struct ctc_slistnode_s
{
    struct ctc_slistnode_s *next;   /**< ctc_slistnode_s type pointer */
} ctc_slistnode_t;

/**
 @brief  Define slist
*/
typedef struct ctc_slist_s
{
    struct ctc_slistnode_s *head;   /**< ctc_slistnode_s type pointer */
    struct ctc_slistnode_s *tail;   /**< ctc_slistnode_s type pointer */
    uint32 count;                   /**< unsigned 32-bit variable */
} ctc_slist_t;

#define CTC_SLISTCOUNT(X) (((X) != NULL) ? ((X)->count) : 0)
#define CTC_SLISTHEAD(X) (((X) != NULL) ? ((X)->head) : NULL)
#define CTC_SLISTTAIL(X) (((X) != NULL) ? ((X)->tail) : NULL)
#define CTC_SLISTNEXTNODE(X) ((X) =((X) != NULL) ? ((X)->next) : NULL)


/* Prototypes. */
struct ctc_slist_s *ctc_slist_new(void);
void ctc_slist_free (struct ctc_slist_s *);

ctc_slistnode_t *ctc_slist_add_head (struct ctc_slist_s *, ctc_slistnode_t *);
ctc_slistnode_t *ctc_slist_add_tail (struct ctc_slist_s *, ctc_slistnode_t *);

void ctc_slist_delete_node(struct ctc_slist_s *, ctc_slistnode_t *);
void ctc_slist_delete_node2(struct ctc_slist_s *slist,ctc_slistnode_t *, ctc_slistnode_t *);

void ctc_slist_delete (struct ctc_slist_s *);
void ctc_slist_delete_all_node (struct ctc_slist_s *);

#define CTC_SLIST_ISEMPTY(X) (((X)->head == NULL) && ((X)->tail == NULL))

/* List iteration macro. */
#define CTC_SLIST_LOOP(L,N) \
  if (L) \
    for ((N) = (L)->head; (N); (N) = (N)->next) \



/* List iteration macro.
 * It allows to delete N and V in the middle of the loop
 */
#define CTC_SLIST_LOOP_DEL(L,N,NN) \
  if (L) \
    for ((N) = (L)->head, NN = ((N)!=NULL) ? (N)->next : NULL;  \
         (N);                                                   \
         (N) = (NN), NN = ((N)!=NULL) ? (N)->next : NULL)       \


#endif /* _SLINKLIST_H_ */

