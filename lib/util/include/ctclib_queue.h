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

#ifndef _CTCLIB_QUEUE_H_
#define _CTCLIB_QUEUE_H_

#ifdef CTCLIB_QUEUE_MACRO_DEBUG
#define _CTCLIB_Q_INVALIDATE(a) (a) = ((void *)-1)
#else
#define _CTCLIB_Q_INVALIDATE(a)
#endif


/*
 * Simple queue definitions.
 */
#define CTCLIB_SIMPLEQ_HEAD(name, type)                    \
struct name {                               \
    struct type *sqh_first; /* first element */         \
    struct type **sqh_last; /* addr of last next element */     \
}

#define CTCLIB_SIMPLEQ_HEAD_INITIALIZER(head)                  \
    { NULL, &(head).sqh_first }

#define CTCLIB_SIMPLEQ_ENTRY(type)                     \
struct {                                \
    struct type *sqe_next;  /* next element */          \
}

/*
 * Simple queue access methods.
 */
#define CTCLIB_SIMPLEQ_FIRST(head)     ((head)->sqh_first)
#define CTCLIB_SIMPLEQ_END(head)       NULL
#define CTCLIB_SIMPLEQ_EMPTY(head)     (CTCLIB_SIMPLEQ_FIRST(head) == CTCLIB_SIMPLEQ_END(head))
#define CTCLIB_SIMPLEQ_NEXT(elm, field)    ((elm)->field.sqe_next)

#define CTCLIB_SIMPLEQ_FOREACH(var, head, field)               \
    for((var) = CTCLIB_SIMPLEQ_FIRST(head);                \
        (var) != CTCLIB_SIMPLEQ_END(head);                 \
        (var) = CTCLIB_SIMPLEQ_NEXT(var, field))

/*
 * Simple queue functions.
 */
#define CTCLIB_SIMPLEQ_INIT(head) do {                     \
    (head)->sqh_first = NULL;                   \
    (head)->sqh_last = &(head)->sqh_first;              \
} while (0)

#define CTCLIB_SIMPLEQ_INSERT_HEAD(head, elm, field) do {          \
    if (((elm)->field.sqe_next = (head)->sqh_first) == NULL)    \
        (head)->sqh_last = &(elm)->field.sqe_next;      \
    (head)->sqh_first = (elm);                  \
} while (0)

#define CTCLIB_SIMPLEQ_INSERT_TAIL(head, elm, field) do {          \
    (elm)->field.sqe_next = NULL;                   \
    *(head)->sqh_last = (elm);                  \
    (head)->sqh_last = &(elm)->field.sqe_next;          \
} while (0)

#define CTCLIB_SIMPLEQ_INSERT_AFTER(head, listelm, elm, field) do {        \
    if (((elm)->field.sqe_next = (listelm)->field.sqe_next) == NULL)\
        (head)->sqh_last = &(elm)->field.sqe_next;      \
    (listelm)->field.sqe_next = (elm);              \
} while (0)

#define CTCLIB_SIMPLEQ_REMOVE_HEAD(head, field) do {           \
    if (((head)->sqh_first = (head)->sqh_first->field.sqe_next) == NULL) \
        (head)->sqh_last = &(head)->sqh_first;          \
} while (0)

/*
 * Tail queue definitions.
 */
#define CTCLIB_TAILQ_HEAD(name, type)                      \
struct name {                               \
    struct type *tqh_first; /* first element */         \
    struct type **tqh_last; /* addr of last next element */     \
}

#define CTCLIB_TAILQ_HEAD_INITIALIZER(head)                    \
    { NULL, &(head).tqh_first }

#define CTCLIB_TAILQ_ENTRY(type)                       \
struct {                                \
    struct type *tqe_next;  /* next element */          \
    struct type **tqe_prev; /* address of previous next element */  \
}

/*
 * tail queue access methods
 */
#define CTCLIB_TAILQ_FIRST(head)       ((head)->tqh_first)
#define CTCLIB_TAILQ_END(head)         NULL
#define CTCLIB_TAILQ_NEXT(elm, field)      ((elm)->field.tqe_next)
#define CTCLIB_TAILQ_LAST(head, headname)                  \
    (*(((struct headname *)((head)->tqh_last))->tqh_last))
/* XXX */
#define CTCLIB_TAILQ_PREV(elm, headname, field)                \
    (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))
#define CTCLIB_TAILQ_EMPTY(head)                       \
    (CTCLIB_TAILQ_FIRST(head) == CTCLIB_TAILQ_END(head))

#define CTCLIB_TAILQ_FOREACH(var, head, field)                 \
    for((var) = CTCLIB_TAILQ_FIRST(head);                  \
        (var) != CTCLIB_TAILQ_END(head);                   \
        (var) = CTCLIB_TAILQ_NEXT(var, field))

#define CTCLIB_TAILQ_FOREACH_REVERSE(var, head, headname, field)       \
    for((var) = CTCLIB_TAILQ_LAST(head, headname);             \
        (var) != CTCLIB_TAILQ_END(head);                   \
        (var) = CTCLIB_TAILQ_PREV(var, headname, field))

/*
 * Tail queue functions.
 */
#define CTCLIB_TAILQ_INIT(head) do {                       \
    (head)->tqh_first = NULL;                   \
    (head)->tqh_last = &(head)->tqh_first;              \
} while (0)

#define CTCLIB_TAILQ_INSERT_HEAD(head, elm, field) do {            \
    if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)    \
        (head)->tqh_first->field.tqe_prev =         \
            &(elm)->field.tqe_next;             \
    else                                \
        (head)->tqh_last = &(elm)->field.tqe_next;      \
    (head)->tqh_first = (elm);                  \
    (elm)->field.tqe_prev = &(head)->tqh_first;         \
} while (0)

#define CTCLIB_TAILQ_INSERT_TAIL(head, elm, field) do {            \
    (elm)->field.tqe_next = NULL;                   \
    (elm)->field.tqe_prev = (head)->tqh_last;           \
    *(head)->tqh_last = (elm);                  \
    (head)->tqh_last = &(elm)->field.tqe_next;          \
} while (0)

#define CTCLIB_TAILQ_INSERT_AFTER(head, listelm, elm, field) do {      \
    if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL)\
        (elm)->field.tqe_next->field.tqe_prev =         \
            &(elm)->field.tqe_next;             \
    else                                \
        (head)->tqh_last = &(elm)->field.tqe_next;      \
    (listelm)->field.tqe_next = (elm);              \
    (elm)->field.tqe_prev = &(listelm)->field.tqe_next;     \
} while (0)

#define CTCLIB_TAILQ_INSERT_BEFORE(listelm, elm, field) do {           \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;      \
    (elm)->field.tqe_next = (listelm);              \
    *(listelm)->field.tqe_prev = (elm);             \
    (listelm)->field.tqe_prev = &(elm)->field.tqe_next;     \
} while (0)

#define CTCLIB_TAILQ_REMOVE(head, elm, field) do {             \
    if (((elm)->field.tqe_next) != NULL)                \
        (elm)->field.tqe_next->field.tqe_prev =         \
            (elm)->field.tqe_prev;              \
    else                                \
        (head)->tqh_last = (elm)->field.tqe_prev;       \
    *(elm)->field.tqe_prev = (elm)->field.tqe_next;         \
    _CTCLIB_Q_INVALIDATE((elm)->field.tqe_prev);               \
    _CTCLIB_Q_INVALIDATE((elm)->field.tqe_next);               \
} while (0)

#define CTCLIB_TAILQ_REPLACE(head, elm, elm2, field) do {          \
    if (((elm2)->field.tqe_next = (elm)->field.tqe_next) != NULL)   \
        (elm2)->field.tqe_next->field.tqe_prev =        \
            &(elm2)->field.tqe_next;                \
    else                                \
        (head)->tqh_last = &(elm2)->field.tqe_next;     \
    (elm2)->field.tqe_prev = (elm)->field.tqe_prev;         \
    *(elm2)->field.tqe_prev = (elm2);               \
    _CTCLIB_Q_INVALIDATE((elm)->field.tqe_prev);               \
    _CTCLIB_Q_INVALIDATE((elm)->field.tqe_next);               \
} while (0)

/*
 * Circular queue definitions.
 */
#define CTCLIB_CIRCLEQ_HEAD(name, type)                    \
struct name {                               \
    struct type *cqh_first;     /* first element */     \
    struct type *cqh_last;      /* last element */      \
}

#define CTCLIB_CIRCLEQ_HEAD_INITIALIZER(head)                  \
    { CTCLIB_CIRCLEQ_END(&head), CTCLIB_CIRCLEQ_END(&head) }

#define CTCLIB_CIRCLEQ_ENTRY(type)                     \
struct {                                \
    struct type *cqe_next;      /* next element */      \
    struct type *cqe_prev;      /* previous element */      \
}

/*
 * Circular queue access methods
 */
#define CTCLIB_CIRCLEQ_FIRST(head)     ((head)->cqh_first)
#define CTCLIB_CIRCLEQ_LAST(head)      ((head)->cqh_last)
#define CTCLIB_CIRCLEQ_END(head)       ((void *)(head))
#define CTCLIB_CIRCLEQ_NEXT(elm, field)    ((elm)->field.cqe_next)
#define CTCLIB_CIRCLEQ_PREV(elm, field)    ((elm)->field.cqe_prev)
#define CTCLIB_CIRCLEQ_EMPTY(head)                     \
    (CTCLIB_CIRCLEQ_FIRST(head) == CTCLIB_CIRCLEQ_END(head))

#define CTCLIB_CIRCLEQ_FOREACH(var, head, field)               \
    for((var) = CTCLIB_CIRCLEQ_FIRST(head);                \
        (var) != CTCLIB_CIRCLEQ_END(head);                 \
        (var) = CTCLIB_CIRCLEQ_NEXT(var, field))

#define CTCLIB_CIRCLEQ_FOREACH_REVERSE(var, head, field)           \
    for((var) = CTCLIB_CIRCLEQ_LAST(head);                 \
        (var) != CTCLIB_CIRCLEQ_END(head);                 \
        (var) = CTCLIB_CIRCLEQ_PREV(var, field))

/*
 * Circular queue functions.
 */
#define CTCLIB_CIRCLEQ_INIT(head) do {                     \
    (head)->cqh_first = CTCLIB_CIRCLEQ_END(head);              \
    (head)->cqh_last = CTCLIB_CIRCLEQ_END(head);               \
} while (0)

#define CTCLIB_CIRCLEQ_INSERT_AFTER(head, listelm, elm, field) do {        \
    (elm)->field.cqe_next = (listelm)->field.cqe_next;      \
    (elm)->field.cqe_prev = (listelm);              \
    if ((listelm)->field.cqe_next == CTCLIB_CIRCLEQ_END(head))     \
        (head)->cqh_last = (elm);               \
    else                                \
        (listelm)->field.cqe_next->field.cqe_prev = (elm);  \
    (listelm)->field.cqe_next = (elm);              \
} while (0)

#define CTCLIB_CIRCLEQ_INSERT_BEFORE(head, listelm, elm, field) do {       \
    (elm)->field.cqe_next = (listelm);              \
    (elm)->field.cqe_prev = (listelm)->field.cqe_prev;      \
    if ((listelm)->field.cqe_prev == CTCLIB_CIRCLEQ_END(head))     \
        (head)->cqh_first = (elm);              \
    else                                \
        (listelm)->field.cqe_prev->field.cqe_next = (elm);  \
    (listelm)->field.cqe_prev = (elm);              \
} while (0)

#define CTCLIB_CIRCLEQ_INSERT_HEAD(head, elm, field) do {          \
    (elm)->field.cqe_next = (head)->cqh_first;          \
    (elm)->field.cqe_prev = CTCLIB_CIRCLEQ_END(head);          \
    if ((head)->cqh_last == CTCLIB_CIRCLEQ_END(head))          \
        (head)->cqh_last = (elm);               \
    else                                \
        (head)->cqh_first->field.cqe_prev = (elm);      \
    (head)->cqh_first = (elm);                  \
} while (0)

#define CTCLIB_CIRCLEQ_INSERT_TAIL(head, elm, field) do {          \
    (elm)->field.cqe_next = CTCLIB_CIRCLEQ_END(head);          \
    (elm)->field.cqe_prev = (head)->cqh_last;           \
    if ((head)->cqh_first == CTCLIB_CIRCLEQ_END(head))         \
        (head)->cqh_first = (elm);              \
    else                                \
        (head)->cqh_last->field.cqe_next = (elm);       \
    (head)->cqh_last = (elm);                   \
} while (0)

#define CTCLIB_CIRCLEQ_REMOVE(head, elm, field) do {               \
    if ((elm)->field.cqe_next == CTCLIB_CIRCLEQ_END(head))         \
        (head)->cqh_last = (elm)->field.cqe_prev;       \
    else                                \
        (elm)->field.cqe_next->field.cqe_prev =         \
            (elm)->field.cqe_prev;              \
    if ((elm)->field.cqe_prev == CTCLIB_CIRCLEQ_END(head))         \
        (head)->cqh_first = (elm)->field.cqe_next;      \
    else                                \
        (elm)->field.cqe_prev->field.cqe_next =         \
            (elm)->field.cqe_next;              \
    _CTCLIB_Q_INVALIDATE((elm)->field.cqe_prev);               \
    _CTCLIB_Q_INVALIDATE((elm)->field.cqe_next);               \
} while (0)

#define CTCLIB_CIRCLEQ_REPLACE(head, elm, elm2, field) do {            \
    if (((elm2)->field.cqe_next = (elm)->field.cqe_next) ==     \
        CTCLIB_CIRCLEQ_END(head))                      \
        (head).cqh_last = (elm2);               \
    else                                \
        (elm2)->field.cqe_next->field.cqe_prev = (elm2);    \
    if (((elm2)->field.cqe_prev = (elm)->field.cqe_prev) ==     \
        CTCLIB_CIRCLEQ_END(head))                      \
        (head).cqh_first = (elm2);              \
    else                                \
        (elm2)->field.cqe_prev->field.cqe_next = (elm2);    \
    _CTCLIB_Q_INVALIDATE((elm)->field.cqe_prev);               \
    _CTCLIB_Q_INVALIDATE((elm)->field.cqe_next);               \
} while (0)

#endif  /* !_CTCLIB_QUEUE_H_ */
