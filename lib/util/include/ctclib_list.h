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

#ifndef __CTCLIB_LIST_H__
#define __CTCLIB_LIST_H__
#include "sal.h"

#ifdef CTCLIB_LIST_MACRO_DEBUG
#define _CTCLIB_L_INVALIDATE(a) (a) = ((void *)-1)
#else
#define _CTCLIB_L_INVALIDATE(a)
#endif

/*
 * Singly-linked List definitions.
 */
#define CTCLIB_SLIST_HEAD(name, type)                      \
struct name {                               \
    struct type *slh_first; /* first element */         \
}

#define CTCLIB_SLIST_HEAD_INITIALIZER(head)                    \
    { NULL }

#define CTCLIB_SLIST_ENTRY(type)                       \
struct {                                \
    struct type *sle_next;  /* next element */          \
}

/*
 * Singly-linked List access methods.
 */
#define CTCLIB_SLIST_FIRST(head)   ((head)->slh_first)
#define CTCLIB_SLIST_END(head)     NULL
#define CTCLIB_SLIST_EMPTY(head)   (CTCLIB_SLIST_FIRST(head) == CTCLIB_SLIST_END(head))
#define CTCLIB_SLIST_NEXT(elm, field)  ((elm)->field.sle_next)

#define CTCLIB_SLIST_FOREACH(var, head, field)                 \
    for((var) = CTCLIB_SLIST_FIRST(head);                  \
        (var) != CTCLIB_SLIST_END(head);                   \
        (var) = CTCLIB_SLIST_NEXT(var, field))

#define CTCLIB_SLIST_FOREACH_PREVPTR(var, varp, head, field)           \
    for ((varp) = &CTCLIB_SLIST_FIRST((head));             \
        ((var) = *(varp)) != CTCLIB_SLIST_END(head);           \
        (varp) = &CTCLIB_SLIST_NEXT((var), field))

/*
 * Singly-linked List functions.
 */
#define CTCLIB_SLIST_INIT(head) {                      \
    CTCLIB_SLIST_FIRST(head) = CTCLIB_SLIST_END(head);                \
}

#define CTCLIB_SLIST_INSERT_AFTER(slistelm, elm, field) do {           \
    (elm)->field.sle_next = (slistelm)->field.sle_next;     \
    (slistelm)->field.sle_next = (elm);             \
} while (0)

#define CTCLIB_SLIST_INSERT_HEAD(head, elm, field) do {            \
    (elm)->field.sle_next = (head)->slh_first;          \
    (head)->slh_first = (elm);                  \
} while (0)

#define CTCLIB_SLIST_REMOVE_NEXT(head, elm, field) do {            \
    (elm)->field.sle_next = (elm)->field.sle_next->field.sle_next;  \
} while (0)

#define CTCLIB_SLIST_REMOVE_HEAD(head, field) do {             \
    (head)->slh_first = (head)->slh_first->field.sle_next;      \
} while (0)

#define CTCLIB_SLIST_REMOVE(head, elm, type, field) do {           \
    if ((head)->slh_first == (elm)) {               \
        CTCLIB_SLIST_REMOVE_HEAD((head), field);           \
    } else {                            \
        struct type *curelm = (head)->slh_first;        \
                                    \
        while (curelm->field.sle_next != (elm))         \
            curelm = curelm->field.sle_next;        \
        curelm->field.sle_next =                \
            curelm->field.sle_next->field.sle_next;     \
        _CTCLIB_L_INVALIDATE((elm)->field.sle_next);           \
    }                               \
} while (0)

#if 0
/*Replaced by  Doubly-linked List*/
/*
 * List functions.
 */
#define LIST_INIT(head) do {                        \
    LIST_FIRST(head) = LIST_END(head);              \
} while (0)

#define LIST_INSERT_AFTER(listelm, elm, field) do {         \
    if (((elm)->field.le_next = (listelm)->field.le_next) != NULL)  \
        (listelm)->field.le_next->field.le_prev =       \
            &(elm)->field.le_next;              \
    (listelm)->field.le_next = (elm);               \
    (elm)->field.le_prev = &(listelm)->field.le_next;       \
} while (0)

#define LIST_INSERT_BEFORE(listelm, elm, field) do {            \
    (elm)->field.le_prev = (listelm)->field.le_prev;        \
    (elm)->field.le_next = (listelm);               \
    *(listelm)->field.le_prev = (elm);              \
    (listelm)->field.le_prev = &(elm)->field.le_next;       \
} while (0)

#define LIST_INSERT_HEAD(head, elm, field) do {             \
    if (((elm)->field.le_next = (head)->lh_first) != NULL)      \
        (head)->lh_first->field.le_prev = &(elm)->field.le_next;\
    (head)->lh_first = (elm);                   \
    (elm)->field.le_prev = &(head)->lh_first;           \
} while (0)

#define LIST_REMOVE(elm, field) do {                    \
    if ((elm)->field.le_next != NULL)               \
        (elm)->field.le_next->field.le_prev =           \
            (elm)->field.le_prev;               \
    *(elm)->field.le_prev = (elm)->field.le_next;           \
    _CTCLIB_Q_INVALIDATE((elm)->field.le_prev);                \
    _CTCLIB_Q_INVALIDATE((elm)->field.le_next);                \
} while (0)

#define LIST_REPLACE(elm, elm2, field) do {             \
    if (((elm2)->field.le_next = (elm)->field.le_next) != NULL) \
        (elm2)->field.le_next->field.le_prev =          \
            &(elm2)->field.le_next;             \
    (elm2)->field.le_prev = (elm)->field.le_prev;           \
    *(elm2)->field.le_prev = (elm2);                \
    _CTCLIB_Q_INVALIDATE((elm)->field.le_prev);                \
    _CTCLIB_Q_INVALIDATE((elm)->field.le_next);                \
} while (0)
#endif
/*
 * Doubly-linked List
 */
struct ctclib_list_node_s
{
    struct ctclib_list_node_s* p_next;
    struct ctclib_list_node_s* p_prev;
};
typedef struct ctclib_list_node_s ctclib_list_node_t;

struct ctclib_list_s
{
    ctclib_list_node_t head;
};
typedef struct ctclib_list_s ctclib_list_t;

/* Define a list */
#define CTCLIB_LIST_DEF(list) ctclib_list_t list = {{NULL, &list.head}}

#define CTCLIB_LHEAD(p_list) (p_list->head.p_next)
#define CTCLIB_LTAIL(p_list) (p_list->head.p_prev)

static inline void
ctclib_list_init(ctclib_list_t* p_list)
{
    CTCLIB_LHEAD(p_list) = NULL;
    CTCLIB_LTAIL(p_list) = &p_list->head;
}

static inline void
ctclib_list_insert_head(ctclib_list_t* p_list, ctclib_list_node_t* p_node)
{
    if ((p_node->p_next = CTCLIB_LHEAD(p_list)) != NULL)
        p_node->p_next->p_prev = p_node;
    else
        CTCLIB_LTAIL(p_list) = p_node;
    CTCLIB_LHEAD(p_list) = p_node;
    p_node->p_prev = &p_list->head;
}

static inline void
ctclib_list_insert_tail(ctclib_list_t* p_list, ctclib_list_node_t* p_node)
{
    p_node->p_next = NULL;
    p_node->p_prev = CTCLIB_LTAIL(p_list);
    CTCLIB_LTAIL(p_list)->p_next = p_node;
    CTCLIB_LTAIL(p_list) = p_node;
}

static inline void
ctclib_list_insert_after(ctclib_list_t* p_list, ctclib_list_node_t* p_node1, ctclib_list_node_t* p_node2)
{
    if ((p_node2->p_next = p_node1->p_next) != NULL)
        p_node2->p_next->p_prev = p_node2;
    else
        CTCLIB_LTAIL(p_list) = p_node2;
    p_node1->p_next = p_node2;
    p_node2->p_prev = p_node1;
}

static inline void
ctclib_list_insert_before(ctclib_list_t* p_list, ctclib_list_node_t* p_node1, ctclib_list_node_t* p_node2)
{
    p_list = p_list; /* TODO unused parameter */
    p_node2->p_next = p_node1;
    p_node2->p_prev = p_node1->p_prev;
    p_node1->p_prev->p_next = p_node2;
    p_node1->p_prev = p_node2;
}

static inline ctclib_list_node_t*
ctclib_list_delete_head(ctclib_list_t* p_list)
{
    ctclib_list_node_t* p_node = CTCLIB_LHEAD(p_list);

    if ((CTCLIB_LHEAD(p_list) = p_node->p_next) != NULL)
        CTCLIB_LHEAD(p_list)->p_prev = &p_list->head;
    else
        CTCLIB_LTAIL(p_list) = &p_list->head;

    return p_node;
}

static inline ctclib_list_node_t*
ctclib_list_delete_tail(ctclib_list_t* p_list)
{
    ctclib_list_node_t* p_node = CTCLIB_LTAIL(p_list);

    CTCLIB_LTAIL(p_list) = p_node->p_prev;
    CTCLIB_LTAIL(p_list)->p_next = NULL;

    return p_node;
}

static inline void
ctclib_list_delete(ctclib_list_t* p_list, ctclib_list_node_t* p_node)
{
    if (p_node->p_next != NULL)
        p_node->p_next->p_prev = p_node->p_prev;
    else
        CTCLIB_LTAIL(p_list) = p_node->p_prev;
    p_node->p_prev->p_next = p_node->p_next;
}

static inline int
ctclib_list_empty(ctclib_list_t* p_list)
{
    return CTCLIB_LHEAD(p_list) == NULL;
}

static inline ctclib_list_node_t*
ctclib_list_head(ctclib_list_t* p_list)
{
    return CTCLIB_LHEAD(p_list);
}

static inline ctclib_list_node_t*
ctclib_list_tail(ctclib_list_t* p_list)
{
    return CTCLIB_LTAIL(p_list)->p_prev->p_next;
}

static inline ctclib_list_node_t*
ctclib_list_next(ctclib_list_node_t* p_node)
{
    return p_node->p_next;
};

static inline ctclib_list_node_t*
ctclib_list_prev(ctclib_list_node_t* p_node)
{
    return p_node->p_prev->p_prev->p_next;
}

#define ctclib_list_for_each(p_node, p_list) \
for (p_node = ctclib_list_head(p_list); p_node; p_node = ctclib_list_next(p_node))

#define ctclib_list_for_each_r(p_node, p_list) \
for (p_node = ctclib_list_tail(p_list); p_node; p_node = ctclib_list_prev(p_node))

#define ctclib_container_of(p, type, member) \
(type *)((char *)p - (int)&((type *)0)->member)

#endif /* !__CTCLIB_LIST_H__ */
