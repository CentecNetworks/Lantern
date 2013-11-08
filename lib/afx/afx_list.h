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

#ifndef __AFX_LIST_H__
#define __AFX_LIST_H__

/*
 * Doubly-linked List
 */
typedef struct afx_list_node
{
    struct afx_list_node* p_next;
    struct afx_list_node* p_prev;
} afx_list_node_t;

typedef struct
{
    afx_list_node_t head;
} afx_list_t;

/* Define a list */
#define AFX_LIST_DEF(list) afx_list_t list = {{NULL, &list.head}}

#define LHEAD(p_list) (p_list->head.p_next)
#define LTAIL(p_list) (p_list->head.p_prev)

static inline void
afx_list_init(afx_list_t* p_list)
{
    LHEAD(p_list) = NULL;
    LTAIL(p_list) = &p_list->head;
}

static inline void
afx_list_insert_head(afx_list_t* p_list, afx_list_node_t* p_node)
{
    if ((p_node->p_next = LHEAD(p_list)) != NULL)
        p_node->p_next->p_prev = p_node;
    else
        LTAIL(p_list) = p_node;
    LHEAD(p_list) = p_node;
    p_node->p_prev = &p_list->head;
}

static inline void
afx_list_insert_tail(afx_list_t* p_list, afx_list_node_t* p_node)
{
    p_node->p_next = NULL;
    p_node->p_prev = LTAIL(p_list);
    LTAIL(p_list)->p_next = p_node;
    LTAIL(p_list) = p_node;
}

static inline void
afx_list_insert_after(afx_list_t* p_list, afx_list_node_t* p_node1, afx_list_node_t* p_node2)
{
    if ((p_node2->p_next = p_node1->p_next) != NULL)
        p_node2->p_next->p_prev = p_node2;
    else
        LTAIL(p_list) = p_node2;
    p_node1->p_next = p_node2;
    p_node2->p_prev = p_node1;
}

static inline void
afx_list_insert_before(afx_list_t* p_list, afx_list_node_t* p_node1, afx_list_node_t* p_node2)
{
    p_list = p_list;
    p_node2->p_next = p_node1;
    p_node2->p_prev = p_node1->p_prev;
    p_node1->p_prev->p_next = p_node2;
    p_node1->p_prev = p_node2;
}

static inline afx_list_node_t*
afx_list_delete_head(afx_list_t* p_list)
{
    afx_list_node_t* p_node = LHEAD(p_list);

    if (p_node == NULL)
        return NULL;

    if ((LHEAD(p_list) = p_node->p_next) != NULL)
        LHEAD(p_list)->p_prev = &p_list->head;
    else
        LTAIL(p_list) = &p_list->head;

    return p_node;
}

static inline afx_list_node_t*
afx_list_delete_tail(afx_list_t* p_list)
{
    afx_list_node_t* p_node = LTAIL(p_list);

    if (p_node == &p_list->head)
        return NULL;

    LTAIL(p_list) = p_node->p_prev;
    LTAIL(p_list)->p_next = NULL;

    return p_node;
}

static inline void
afx_list_delete(afx_list_t* p_list, afx_list_node_t* p_node)
{
    if (p_node->p_next != NULL)
        p_node->p_next->p_prev = p_node->p_prev;
    else
        LTAIL(p_list) = p_node->p_prev;
    p_node->p_prev->p_next = p_node->p_next;
}

static inline int
afx_list_empty(afx_list_t* p_list)
{
    return LHEAD(p_list) == NULL;
}

static inline afx_list_node_t*
afx_list_head(afx_list_t* p_list)
{
    return LHEAD(p_list);
}

static inline afx_list_node_t*
afx_list_tail(afx_list_t* p_list)
{
    return LTAIL(p_list)->p_prev->p_next;
}

static inline afx_list_node_t*
afx_list_next(afx_list_node_t* p_node)
{
    return p_node->p_next;
};

static inline afx_list_node_t*
afx_list_prev(afx_list_node_t* p_node)
{
    return p_node->p_prev->p_prev->p_next;
}

#define afx_list_for_each(p_node, p_list) \
for (p_node = afx_list_head(p_list); p_node; p_node = afx_list_next(p_node))

#define afx_list_for_each_safe(p_node, p_next, p_list) \
for (p_node = afx_list_head(p_list), p_next = p_node?afx_list_next(p_node):NULL; p_node; \
    p_node = p_next, p_next = p_node?afx_list_next(p_node):NULL)

#define afx_list_for_each_r(p_node, p_list) \
for (p_node = afx_list_tail(p_list); p_node; p_node = afx_list_prev(p_node))

#endif /* !__AFX_LIST_H__ */

