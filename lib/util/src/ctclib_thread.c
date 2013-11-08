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

#include <poll.h>
#include "sal.h"
#include "ctclib_thread.h"

#ifndef _CENTEC_
#define _CENTEC_
#endif

#ifdef _CENTEC_
#define MAX_UNUSED_THREAD_COUNT  1000
#endif

/* get boot time */
sal_time_t _g_ctclib_last_upsec = 0;
struct sal_timeval _g_ctclib_last_boottime = {0, 0};
thread_master_t *_g_p_ctclib_thread_master = NULL;

struct sal_timeval
ctclib_timeval_adjust (struct sal_timeval a)
{
    while (a.tv_usec >= CTCLIB_ONE_SEC_MICROSECOND)
    {
        a.tv_usec -= CTCLIB_ONE_SEC_MICROSECOND;
        a.tv_sec++;
    }
    while (a.tv_usec < 0)
    {
        a.tv_usec += CTCLIB_ONE_SEC_MICROSECOND;
        a.tv_sec--;
    }

    if (a.tv_sec < 0)
    {
        a.tv_sec = 0;
        a.tv_usec = 10;
    }

    return a;
}

struct sal_timeval
ctclib_timeval_subtract (struct sal_timeval a, struct sal_timeval b)
{
    struct sal_timeval ret;

    ret.tv_usec = a.tv_usec - b.tv_usec;
    ret.tv_sec = a.tv_sec - b.tv_sec;

    return ctclib_timeval_adjust(ret);
}

struct sal_timeval
ctclib_timeval_add (struct sal_timeval a, struct sal_timeval b)
{
    struct sal_timeval ret;

    ret.tv_sec = a.tv_sec + b.tv_sec;
    ret.tv_usec = a.tv_usec + b.tv_usec;

    return ctclib_timeval_adjust(ret);
}

static inline int32
ctclib_timeval_cmp (struct sal_timeval a, struct sal_timeval b)
{
    return (a.tv_sec == b.tv_sec ? a.tv_usec - b.tv_usec : a.tv_sec - b.tv_sec);
}

static inline struct sal_timeval *
_ctclib_time_boottime(struct sal_timeval *tv)
{
    /*
     * (struct timezone *)-1 means get relative time/uptime
     * refer to kernel/time.c sys_gettimeofday()
     */
    gettimeofday(tv, (struct timezone *)-1);
    _g_ctclib_last_upsec = tv->tv_sec;
    _g_ctclib_last_boottime.tv_sec = tv->tv_sec;
    _g_ctclib_last_boottime.tv_usec = tv->tv_usec;
    return tv;
}

struct sal_timeval *
ctclib_time_boottime(struct sal_timeval *tv)
{
    tv->tv_sec = _g_ctclib_last_boottime.tv_sec;
    tv->tv_usec = _g_ctclib_last_boottime.tv_usec;
    return tv;
}

/* Allocate new thread master.  */
thread_master_t *
ctclib_thread_master_create()
{
    SAL_ASSERT(NULL == _g_p_ctclib_thread_master);

    _g_p_ctclib_thread_master = (thread_master_t *)sal_calloc(sizeof(thread_master_t));
#ifdef _CENTEC_
    _g_p_ctclib_thread_master->poll_list =  (struct pollfd *)sal_calloc(sizeof(struct pollfd)*CTCLIB_THREAD_MAX_POLL);
#endif
    return _g_p_ctclib_thread_master;
}

thread_master_t *
ctclib_thread_get_master()
{
    return _g_p_ctclib_thread_master;
}

bool
ctclib_thread_expired(thread_t *t)
{
    struct sal_timeval timer_now;

    _ctclib_time_boottime(&timer_now);
    if (ctclib_timeval_cmp(timer_now, t->u.sands) >= 0)
    {
        return TRUE;
    }

    return FALSE;
}

/* check remain timeval */
void
ctclib_thread_timer_remain_timeval(thread_t *t, struct sal_timeval *tv)
{
    struct sal_timeval timer_now;

    if (NULL == t)
    {
        return;
    }

    _ctclib_time_boottime(&timer_now);
    if (ctclib_timeval_cmp(t->u.sands, timer_now) >= 0)
    {
        ctclib_timersub(&t->u.sands, &timer_now, tv);
    }
    else
    {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }

    return;
}

/* get expire timeval */
inline struct sal_timeval
ctclib_thread_expire_timeval(thread_t *t)
{
    struct sal_timeval tv = {0, 0};
    struct sal_timeval timer_now;

    if (NULL != t)
    {
        _ctclib_time_boottime(&timer_now);
        ctclib_timersub(&t->u.sands, &timer_now, &tv);
        sal_gettimeofday(&timer_now);
        tv = CTCLIB_TV_ADD(timer_now, tv);
    }

    return tv;
}

/* Add a new thread to the list.  */
void
ctclib_thread_list_add (thread_list_t *list, thread_t *thread)
{
    thread->next = NULL;
    thread->prev = list->tail;
    if (list->tail)
    {
        list->tail->next = thread;
    }
    else
    {
        list->head = thread;
    }
    list->tail = thread;
    list->count++;
    if(list->count > list->max_count)
        list->max_count = list->count;
}

/* Add a new thread just after the point. If point is NULL, add to top. */
static void
_ctclib_thread_list_add_after (thread_list_t *list,
                thread_t *point, thread_t *thread)
{
    thread->prev = point;
    if (point)
    {
        if (point->next)
        {
            point->next->prev = thread;
        }
        else
        {
            list->tail = thread;
        }
        thread->next = point->next;
        point->next = thread;
    }
    else
    {
        if (list->head)
        {
            list->head->prev = thread;
        }
        else
        {
            list->tail = thread;
        }
        thread->next = list->head;
        list->head = thread;
    }
    list->count++;
    if(list->count > list->max_count)
        list->max_count = list->count;
}

/* Delete a thread from the list. */
static thread_t *
ctclib_thread_list_delete (thread_list_t *list, thread_t *thread)
{
    if (thread->next)
    {
        thread->next->prev = thread->prev;
    }
    else
    {
        list->tail = thread->prev;
    }

    if (thread->prev)
    {
        thread->prev->next = thread->next;
    }
    else
    {
        list->head = thread->next;
    }
    thread->next = thread->prev = NULL;
    list->count--;
    return thread;
}

/* Delete top of the list and return it. */
static thread_t *
ctclib_thread_trim_head (thread_list_t *list)
{
    if (list->head)
    {
        return ctclib_thread_list_delete (list, list->head);
    }
    return NULL;
}

/* Move thread to unuse list. */
static void
ctclib_thread_add_unuse (thread_master_t *m, thread_t *thread)
{
    SAL_ASSERT(NULL != m);
    SAL_ASSERT(NULL == thread->next);
    SAL_ASSERT(NULL == thread->prev);
    SAL_ASSERT(CTCLIB_THREAD_UNUSED == thread->type);

#ifdef _CENTEC_
    if (m->unuse.count >= MAX_UNUSED_THREAD_COUNT)
    {
        sal_free(thread);
    }
    else
#endif /* _CENTEC_ */
    {
        ctclib_thread_list_add(&m->unuse, thread);
    }
}

/* Free all unused thread. */
static void
ctclib_thread_list_free (thread_master_t *m, thread_list_t *list)
{
    thread_t *t = NULL;
    thread_t *next = NULL;

    for (t = list->head; t; t = next)
    {
        next = t->next;
        sal_free(t);
        list->count--;
        m->alloc--;
    }
}

void
ctclib_thread_list_execute (thread_master_t *m, thread_list_t *list)
{
    thread_t *thread = NULL;

    thread = ctclib_thread_trim_head(list);
    if (thread != NULL)
    {
        ctclib_thread_execute(m, thread->func, thread->arg, thread->u.val);
        thread->type = CTCLIB_THREAD_UNUSED;
        ctclib_thread_add_unuse(m, thread);
    }
}

void
ctclib_thread_list_clear (thread_master_t *m, thread_list_t *list)
{
    thread_t *thread = NULL;

    while ((thread = ctclib_thread_trim_head(list)))
    {
      thread->type = CTCLIB_THREAD_UNUSED;
      ctclib_thread_add_unuse(m, thread);
    }
}

/* Stop thread scheduler. */
void
ctclib_thread_master_finish (thread_master_t *m)
{
    int32 i = 0;

    ctclib_thread_list_free(m, &m->queue_high);
    ctclib_thread_list_free(m, &m->queue_middle);
    ctclib_thread_list_free(m, &m->queue_low);
    ctclib_thread_list_free(m, &m->read);
    ctclib_thread_list_free(m, &m->read_high);
    ctclib_thread_list_free(m, &m->write);
    for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
    {
        ctclib_thread_list_free(m, &m->timer[i]);
    }
    ctclib_thread_list_free(m, &m->event);
    ctclib_thread_list_free(m, &m->event_low);
    ctclib_thread_list_free(m, &m->unuse);

    sal_free(m);
}

/* Thread list is empty or not.  */
int32
ctclib_thread_empty (thread_list_t *list)
{
    return list->head ? FALSE : TRUE;
}

/* Return remain time in second. */
uint32
ctclib_thread_timer_remain_second (thread_t *thread)
{
    struct sal_timeval timer_now;

    if (NULL == thread)
    {
        return 0;
    }

#ifndef _CENTEC_
    sal_gettimeofday(&timer_now);
#else
    _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */

    if (thread->u.sands.tv_sec - timer_now.tv_sec > 0)
    {
        return thread->u.sands.tv_sec - timer_now.tv_sec;
    }
    else
    {
        return 0;
    }
}

uint32
ctclib_thread_timer_remain_msec (thread_t *thread)
{
    struct sal_timeval timer_now;

    if (NULL == thread)
    {
        return 0;
    }

#ifndef _CENTEC_
    sal_gettimeofday(&timer_now);
#else
    _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */

    if (thread->u.sands.tv_sec - timer_now.tv_sec > 0)
    {
        return ((thread->u.sands.tv_sec - timer_now.tv_sec) * 1000
            + (thread->u.sands.tv_usec - timer_now.tv_usec) / 1000);
    }
    else if ((thread->u.sands.tv_sec - timer_now.tv_sec == 0)
      && (thread->u.sands.tv_usec - timer_now.tv_usec > 0))
    {
        return ((thread->u.sands.tv_usec - timer_now.tv_usec) / 1000);
    }
    else
    {
        return 0;
    }
}

/* Get new thread.  */
thread_t *
ctclib_thread_get (thread_master_t *m, int32 type,
            int32 (*func) (thread_t *), void *arg)
{
    thread_t *thread = NULL;

    if (m->unuse.head)
    {
        thread = ctclib_thread_trim_head(&m->unuse);
    }
    else
    {
        thread = sal_calloc(sizeof (thread_t));
        if (NULL == thread)
        {
            return NULL;
        }
        m->alloc++;
    }
    thread->type = type;
    thread->master = m;
    thread->func = func;
    thread->arg = arg;

    return thread;
}

/* Keep track of the maximum file descriptor for read/write. */
static void
_ctclib_thread_update_max_fd (thread_master_t *m, int32 fd)
{
    if (m && m->max_fd < fd)
    {
        m->max_fd = fd;
    }
}

/* Add new read thread. */
thread_t *
ctclib_thread_add_read (thread_master_t *m,
        int32 (*func) (thread_t *), void *arg, int32 fd)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    if (fd < 0)
    {
        return NULL;
    }

    thread = ctclib_thread_get(m, CTCLIB_THREAD_READ, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    _ctclib_thread_update_max_fd(m, fd);
    SAL_FD_SET(fd, &m->readfd);
    thread->u.fd = fd;
    ctclib_thread_list_add(&m->read, thread);

    return thread;
}

/* Add new high priority read thread. */
thread_t *
ctclib_thread_add_read_high (thread_master_t *m,
                 int32 (*func) (thread_t *), void *arg, int32 fd)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    if (fd < 0)
    {
        return NULL;
    }

    thread = ctclib_thread_get(m, CTCLIB_THREAD_READ_HIGH, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    _ctclib_thread_update_max_fd(m, fd);
    SAL_FD_SET(fd, &m->readfd);
    thread->u.fd = fd;
    ctclib_thread_list_add(&m->read_high, thread);

    return thread;
}

#ifdef _CENTEC_
/* Add new urgency priority read thread. */
thread_t *
ctclib_thread_add_read_urgency (thread_master_t *m,
                 int32 (*func) (ctclib_thread_t *), void *arg, int32 fd)
{
    thread_t *thread = NULL;
    struct pollfd * poll_list = (struct pollfd *)m->poll_list;

    SAL_ASSERT(NULL != m);

    if (fd < 0)
    {
        return NULL;
    }

    thread = ctclib_thread_get(m, CTCLIB_THREAD_READ_URG, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    _ctclib_thread_update_max_fd(m, fd);
    SAL_FD_SET(fd, &m->readfd);
    thread->u.fd = fd;
    poll_list[m->poll_num].fd = fd;
    poll_list[m->poll_num].events = POLLIN;
    m->poll_num++;
    ctclib_thread_list_add(&m->read_urg, thread);

    return thread;
}

void
ctclib_thread_delete_from_polllist(thread_t *thread)
{
    int i;
    struct thread_master *m;
    struct pollfd * poll_list;

    SAL_ASSERT(NULL != thread);

    m = thread->master;
    poll_list = (struct pollfd *)m->poll_list;
    for (i =0; i < m->poll_num; i++)
    {
        if (poll_list[i].fd == thread->u.fd)
        {
            poll_list[i].fd = poll_list[m->poll_num-1].fd;
            poll_list[i].events = poll_list[m->poll_num-1].events;
            m->poll_num --;
            return;
        }
    }

    return;
}
#endif

/* Add new write thread. */
thread_t *
ctclib_thread_add_write (thread_master_t *m,
                 int32 (*func) (thread_t *), void *arg, int32 fd)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    if (fd < 0 || SAL_FD_ISSET(fd, &m->writefd))
    {
        return NULL;
    }

    thread = ctclib_thread_get(m, CTCLIB_THREAD_WRITE, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    _ctclib_thread_update_max_fd(m, fd);
    SAL_FD_SET(fd, &m->writefd);
    thread->u.fd = fd;
    ctclib_thread_list_add(&m->write, thread);

    return thread;
}

static void
_ctclib_thread_add_timer_common (thread_master_t *m, thread_t *thread)
{
#ifndef TIMER_NO_SORT
    thread_t *tt = NULL;
#endif /* TIMER_NO_SORT */
    m->index = 0;

    /* Set index.  */
    thread->index = m->index;

    /* Sort by timeval. */
#ifdef TIMER_NO_SORT
    ctclib_thread_list_add(&m->timer[m->index], thread);
#else
    for (tt = m->timer[m->index].tail; tt; tt = tt->prev)
    {
        if (ctclib_timeval_cmp(thread->u.sands, tt->u.sands) >= 0)
        {
            break;
        }
    }
    _ctclib_thread_list_add_after(&m->timer[m->index], tt, thread);
#endif /* TIMER_NO_SORT */

    /* Increment timer slot index.  */
    m->index++;
    m->index %= CTCLIB_THREAD_TIMER_SLOT;
}

/* Add timer event thread. */
thread_t *
ctclib_thread_add_timer (thread_master_t *m,
                 int32 (*func) (thread_t *),
                 void *arg, int32 timer)
{
    struct sal_timeval timer_now;
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);
    sal_memset(&timer_now, 0, sizeof(timer_now));
    thread = ctclib_thread_get(m, CTCLIB_THREAD_TIMER, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

#ifndef _CENTEC_
    sal_gettimeofday(&timer_now);
#else
    _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */
    timer_now.tv_sec += timer;
    thread->u.sands = timer_now;

    /* Common process.  */
    _ctclib_thread_add_timer_common(m, thread);

    return thread;
}

/* Add timer event thread. */
thread_t *
ctclib_thread_add_timer_timeval (thread_master_t *m,
                          int32 (*func) (thread_t *), void *arg,
                          struct sal_timeval timer)
{
    struct sal_timeval timer_now;
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);
    sal_memset(&timer_now, 0, sizeof(timer_now));
    thread = ctclib_thread_get(m, CTCLIB_THREAD_TIMER, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    /* Do we need jitter here? */
#ifndef _CENTEC_
    sal_gettimeofday(&timer_now);
#else
    _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */
    timer_now.tv_sec += timer.tv_sec;
    timer_now.tv_usec += timer.tv_usec;
    while (timer_now.tv_usec >= CTCLIB_TV_USEC_PER_SEC)
    {
      timer_now.tv_sec++;
      timer_now.tv_usec -= CTCLIB_TV_USEC_PER_SEC;
    }

    /* Correct negative value.  */
    if (timer_now.tv_sec < 0)
    {
        timer_now.tv_sec = CTCLIB_TIME_MAX_TV_SEC;
    }
    if (timer_now.tv_usec < 0)
    {
        timer_now.tv_usec = CTCLIB_TIME_MAX_TV_USEC;
    }

    thread->u.sands = timer_now;

    /* Common process.  */
    _ctclib_thread_add_timer_common(m, thread);

    return thread;
}

/* Add timer event thread with "millisecond" resolution */
thread_t *
ctclib_thread_add_timer_msec (thread_master_t *m,
                                int32 (*func) (thread_t *),
                                void *arg, int32 timer)
{
    struct sal_timeval trel;

    SAL_ASSERT(NULL != m);

    trel.tv_sec = timer / 1000;
    trel.tv_usec = 1000*(timer % 1000);

    return ctclib_thread_add_timer_timeval(m, func, arg, trel);
}

/* Add simple event thread. */
thread_t *
ctclib_thread_add_event (thread_master_t *m,
        int32 (*func) (thread_t *), void *arg, int32 val)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    thread = ctclib_thread_get(m, CTCLIB_THREAD_EVENT, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    thread->u.val = val;
    ctclib_thread_list_add(&m->event, thread);

    return thread;
}

/* Add simple event thread. */
thread_t *
ctclib_thread_add_enqueue_low (thread_master_t *m,
        int32 (*func) (thread_t *), void *arg, int32 val)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    thread = ctclib_thread_get(m, CTCLIB_THREAD_QUEUE, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    ctclib_thread_enqueue_low (m, thread);

    return thread;
}


/* Add low priority event thread. */
thread_t *
ctclib_thread_add_event_low (thread_master_t *m,
                      int32 (*func) (thread_t *), void *arg, int32 val)
{
    thread_t *thread = NULL;

    SAL_ASSERT(NULL != m);

    thread = ctclib_thread_get(m, CTCLIB_THREAD_EVENT_LOW, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    thread->u.val = val;
    ctclib_thread_list_add(&m->event_low, thread);

    return thread;
}

/* Add pending read thread. */
thread_t *
ctclib_thread_add_read_pend (thread_master_t *m,
                        int32 (*func) (thread_t *), void *arg, int32 val)
{
    thread_t *thread;

    SAL_ASSERT(NULL != m);

    thread = ctclib_thread_get(m, CTCLIB_THREAD_READ_PEND, func, arg);
    if (NULL == thread)
    {
        return NULL;
    }

    thread->u.val = val;
    ctclib_thread_list_add(&m->read_pend, thread);

    return thread;
}

/* Cancel thread from scheduler. */
void
ctclib_thread_cancel (thread_t *thread)
{
    switch (thread->type)
    {
    case CTCLIB_THREAD_READ:
        SAL_FD_CLR(thread->u.fd, &thread->master->readfd);
        ctclib_thread_list_delete(&thread->master->read, thread);
        break;
    case CTCLIB_THREAD_READ_HIGH:
        SAL_FD_CLR(thread->u.fd, &thread->master->readfd);
        ctclib_thread_list_delete(&thread->master->read_high, thread);
        break;
    case CTCLIB_THREAD_WRITE:
        SAL_ASSERT(SAL_FD_ISSET(thread->u.fd, &thread->master->writefd));
        SAL_FD_CLR(thread->u.fd, &thread->master->writefd);
        ctclib_thread_list_delete(&thread->master->write, thread);
        break;
    case CTCLIB_THREAD_TIMER:
        ctclib_thread_list_delete(&thread->master->timer[(int32)thread->index], thread);
        break;
    case CTCLIB_THREAD_EVENT:
        ctclib_thread_list_delete(&thread->master->event, thread);
        break;
    case CTCLIB_THREAD_READ_PEND:
        ctclib_thread_list_delete(&thread->master->read_pend, thread);
        break;
    case CTCLIB_THREAD_EVENT_LOW:
        ctclib_thread_list_delete(&thread->master->event_low, thread);
        break;
    case CTCLIB_THREAD_QUEUE:
        switch (thread->priority)
        {
        case CTCLIB_THREAD_PRIORITY_HIGH:
            ctclib_thread_list_delete(&thread->master->queue_high, thread);
            break;
        case CTCLIB_THREAD_PRIORITY_MIDDLE:
            ctclib_thread_list_delete(&thread->master->queue_middle, thread);
            break;
        case CTCLIB_THREAD_PRIORITY_LOW:
            ctclib_thread_list_delete(&thread->master->queue_low, thread);
            break;
        }
        break;
#ifdef _CENTEC_
    case CTCLIB_THREAD_READ_URG:
        SAL_FD_CLR(thread->u.fd, &thread->master->readfd);
        ctclib_thread_delete_from_polllist(thread);
        ctclib_thread_list_delete(&thread->master->read_urg, thread);
        break;
#endif
    default:
        break;
    }

    thread->type = CTCLIB_THREAD_UNUSED;
#ifdef _CENTEC_
    ctclib_thread_add_unuse(thread->master, thread);
#endif
}

/* Delete all events which has argument value arg. */
void
ctclib_thread_cancel_event (thread_master_t *m, void *arg)
{
    thread_t *thread = NULL;
    thread_t *t = NULL;

    thread = m->event.head;
    while (thread)
    {
        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->event, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }

    /* Since Event could have been Queued search queue_high */
    thread = m->queue_high.head;
    while (thread)
    {
        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->queue_high, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }

    return;
}

/* Delete all low-events which has argument value arg */
void
ctclib_thread_cancel_event_low (thread_master_t *m, void *arg)
{
    thread_t *thread = NULL;
    thread_t *t = NULL;

    thread = m->event_low.head;
    while (thread)
    {
        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->event_low, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }

    /* Since Event could have been Queued search queue_low */
    thread = m->queue_low.head;
    while (thread)
    {
        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->queue_low, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }

  return;
}

/* Delete all read events which has argument value arg. */
void
ctclib_thread_cancel_read (thread_master_t *m, void *arg)
{
    thread_t *thread = NULL;

    thread = m->read.head;
    while (thread)
    {
        thread_t *t;

        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->read, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }
}

/* Delete all write events which has argument value arg. */
void
ctclib_thread_cancel_write (thread_master_t *m, void *arg)
{
    thread_t *thread = NULL;

    thread = m->write.head;
    while (thread)
    {
        thread_t *t;
        t = thread;
        thread = t->next;

        if (t->arg == arg)
        {
            ctclib_thread_list_delete(&m->write, t);
            t->type = CTCLIB_THREAD_UNUSED;
            ctclib_thread_add_unuse(m, t);
        }
    }
}

/* Delete all timer events which has argument value arg. */
void
ctclib_thread_cancel_timer (thread_master_t *m, void *arg)
{
    thread_t *thread = NULL;
    int32 i = 0;

    for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
    {
        thread = m->timer[i].head;
        while (thread)
        {
            thread_t *t;

            t = thread;
            thread = t->next;

            if (t->arg == arg)
            {
                ctclib_thread_list_delete(&m->timer[i], t);
                t->type = CTCLIB_THREAD_UNUSED;
                ctclib_thread_add_unuse(m, t);
            }
        }
    }
}

#ifdef RTOS_DEFAULT_WAIT_TIME
struct sal_timeval *
_ctclib_thread_timer_wait (thread_master_t *m, struct sal_timeval *timer_val)
{
    timer_val->tv_sec = 1;
    timer_val->tv_usec = 0;
    return timer_val;
}
#else /* ! RTOS_DEFAULT_WAIT_TIME */
#ifdef HAVE_RTOS_TIMER
struct sal_timeval *
_ctclib_thread_timer_wait (thread_master_t *m, struct sal_timeval *timer_val)
{
    rtos_set_time (timer_val);
    return timer_val;
}
#else /* ! HAVE_RTOS_TIMER */
#ifdef HAVE_RTOS_TIC
struct sal_timeval *
_ctclib_thread_timer_wait (thread_master_t *m, struct sal_timeval *timer_val)
{
    timer_val->tv_sec = 0;
    timer_val->tv_usec = 10;
    return timer_val;
}
#else /* ! HAVE_RTOS_TIC */
#ifdef TIMER_NO_SORT
struct sal_timeval *
_ctclib_thread_timer_wait (thread_master_t *m, struct sal_timeval *timer_val)
{
    struct sal_timeval timer_now;
    struct sal_timeval timer_min;
    struct sal_timeval *timer_wait = NULL;
    thread_t *thread = NULL;
    int32 i = 0;

    timer_wait = NULL;

    for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
    {
        for (thread = m->timer[i].head; thread; thread = thread->next)
        {
            if (! timer_wait)
            {
                timer_wait = &thread->u.sands;
            }
            else if (ctclib_timeval_cmp(thread->u.sands, *timer_wait) < 0)
            {
                timer_wait = &thread->u.sands;
            }
        }
    }

    if (timer_wait)
    {
        timer_min = *timer_wait;

#ifndef _CENTEC_
        ctclib_time_tzcurrent(&timer_now);
#else
        _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */
        timer_min = ctclib_timeval_subtract(timer_min, timer_now);

        if (timer_min.tv_sec < 0)
        {
            timer_min.tv_sec = 0;
            timer_min.tv_usec = 10;
        }

        *timer_val = timer_min;
        return timer_val;
    }
    return NULL;
}
#else /* ! TIMER_NO_SORT */
/* Pick up smallest timer.  */
struct sal_timeval *
_ctclib_thread_timer_wait (thread_master_t *m, struct sal_timeval *timer_val)
{
    struct sal_timeval timer_now;
    struct sal_timeval timer_min;
    struct sal_timeval *timer_wait = NULL;
    thread_t *thread = NULL;
    int32 i = 0;

    timer_wait = NULL;

    for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
    {
        if ((thread = m->timer[i].head) != NULL)
        {
            if (! timer_wait)
            {
                timer_wait = &thread->u.sands;
            }
            else if (ctclib_timeval_cmp(thread->u.sands, *timer_wait) < 0)
            {
                timer_wait = &thread->u.sands;
            }
        }
    }

    if (timer_wait)
    {
        timer_min = *timer_wait;

#ifndef _CENTEC_
        ctclib_time_tzcurrent(&timer_now);
#else
        _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */
        timer_min = ctclib_timeval_subtract(timer_min, timer_now);

        if (timer_min.tv_sec < 0)
        {
            timer_min.tv_sec = 0;
            timer_min.tv_usec = 10;
        }

        *timer_val = timer_min;
        return timer_val;
    }

    return NULL;
}
#endif /* TIMER_NO_SORT */
#endif /* HAVE_RTOS_TIC */
#endif /* HAVE_RTOS_TIMER */
#endif /* RTOS_DEFAULT_WAIT_TIME */


thread_t *
ctclib_thread_run (thread_master_t *m, thread_t *thread,
        thread_t *fetch)
{
    *fetch = *thread;
    thread->type = CTCLIB_THREAD_UNUSED;
    ctclib_thread_add_unuse(m, thread);
    return fetch;
}

void
ctclib_thread_enqueue_high (thread_master_t *m, thread_t *thread)
{
    thread->type = CTCLIB_THREAD_QUEUE;
    thread->priority = CTCLIB_THREAD_PRIORITY_HIGH;
    ctclib_thread_list_add(&m->queue_high, thread);
}

void
ctclib_thread_enqueue_middle (thread_master_t *m, thread_t *thread)
{
    thread->type = CTCLIB_THREAD_QUEUE;
    thread->priority = CTCLIB_THREAD_PRIORITY_MIDDLE;
    ctclib_thread_list_add(&m->queue_middle, thread);
}

void
ctclib_thread_enqueue_low (thread_master_t *m, thread_t *thread)
{
    thread->type = CTCLIB_THREAD_QUEUE;
    thread->priority = CTCLIB_THREAD_PRIORITY_LOW;
    ctclib_thread_list_add(&m->queue_low, thread);
}

/* When the file is ready move to queue.  */
int32
ctclib_thread_process_fd (thread_master_t *m, thread_list_t *list,
            sal_fd_set_t *fdset, sal_fd_set_t *mfdset)
{
    thread_t *thread = NULL;
    thread_t *next = NULL;
    int32 ready = 0;
    int i;
    struct pollfd *poll_list = (struct pollfd *)m->poll_list;

    for (thread = list->head; thread; thread = next)
    {
        next = thread->next;

        if (SAL_FD_ISSET(CTCLIB_THREAD_FD(thread), fdset))
        {
            SAL_FD_CLR(CTCLIB_THREAD_FD(thread), mfdset);
            ctclib_thread_list_delete(list, thread);
#ifdef _CENTEC_
            if (thread->type == CTCLIB_THREAD_READ_URG)
            {
                ctclib_thread_enqueue_high(m, thread);
                for(i = 0; i < m->poll_num; i++)
                {
                    if (thread->u.fd == poll_list[i].fd)
                    {
                        poll_list[i].fd = poll_list[m->poll_num-1].fd;
                        poll_list[i].events = poll_list[m->poll_num-1].events;
                        poll_list[i].revents = poll_list[m->poll_num-1].revents;
                        m->poll_num --;
                        break;
                    }
                }
            }
            else
#endif
            ctclib_thread_enqueue_middle(m, thread);
            ready++;
        }
    }

    return ready;
}

#ifdef _CENTEC_
void
ctclib_thread_process_urgency_fd (thread_master_t *m, thread_list_t *list,
            int fd)
{
    thread_t *thread = NULL;
    thread_t *next = NULL;

    for (thread = list->head; thread; thread = next)
    {
        next = thread->next;

        if (thread->u.fd == fd)
        {
            SAL_FD_CLR(CTCLIB_THREAD_FD(thread), &m->readfd);
            ctclib_thread_list_delete(list, thread);
            ctclib_thread_enqueue_high(m, thread);
            return;
        }
    }

    return;
}
#endif

/* Fetch next ready thread. */
thread_t *
ctclib_thread_fetch (thread_master_t *m, thread_t *fetch)
{
    int32 num = 0;
    thread_t *thread = NULL;
    thread_t *next = NULL;
    sal_fd_set_t readfd;
    sal_fd_set_t writefd;
    sal_fd_set_t exceptfd;
    struct sal_timeval timer_now;
    struct sal_timeval timer_val;
    struct sal_timeval *timer_wait = NULL;;
    struct sal_timeval timer_nowait;
    int32 i = 0;
#ifdef _CENTEC_
    int ret;
    struct pollfd *poll_list = (struct pollfd *)m->poll_list;
#endif

#ifdef RTOS_DEFAULT_WAIT_TIME
    /* 1 sec might not be optimized */
    timer_nowait.tv_sec = 1;
    timer_nowait.tv_usec = 0;
#else
    timer_nowait.tv_sec = 0;
    timer_nowait.tv_usec = 0;
#endif /* RTOS_DEFAULT_WAIT_TIME */

    while (1)
    {
#ifdef _CENTEC_
        if (m->poll_num > 0)
        {
            ret = poll(poll_list, m->poll_num, 0);
            if (ret > 0)
            {
                for (i = 0; i < m->poll_num; i++)
                {
                    if(poll_list[i].revents&POLLIN)
                    {
                        ctclib_thread_process_urgency_fd(m, &m->read_urg, poll_list[i].fd);
                        poll_list[i].fd = poll_list[m->poll_num-1].fd;
                        poll_list[i].events = poll_list[m->poll_num-1].events;
                        poll_list[i].revents = poll_list[m->poll_num-1].revents;
                        m->poll_num --;
                        i--;
                    }
                }
            }
        }
#endif

        /* Pending read is exception. */
        if ((thread = ctclib_thread_trim_head(&m->read_pend)) != NULL)
        {
            return ctclib_thread_run(m, thread, fetch);
        }

        /* Check ready queue.  */
        if ((thread = ctclib_thread_trim_head(&m->queue_high)) != NULL)
        {
            return ctclib_thread_run(m, thread, fetch);
        }

        if ((thread = ctclib_thread_trim_head(&m->queue_middle)) != NULL)
        {
            return ctclib_thread_run(m, thread, fetch);
        }

        if ((thread = ctclib_thread_trim_head(&m->queue_low)) != NULL)
        {
            return ctclib_thread_run(m, thread, fetch);
        }

        /* Check all of available events.  */

        /* Check events.  */
        while ((thread = ctclib_thread_trim_head(&m->event)) != NULL)
        {
            ctclib_thread_enqueue_high(m, thread);
        }

        /* Check timer.  */
#ifndef _CENTEC_
        ctclib_time_tzcurrent(&timer_now);
#else
        _ctclib_time_boottime(&timer_now);
#endif /* !_CENTEC_ */

        for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
        {
            for (thread = m->timer[i].head; thread; thread = next)
            {
                next = thread->next;
                if (ctclib_timeval_cmp(timer_now, thread->u.sands) >= 0)
                {
                    ctclib_thread_list_delete(&m->timer[i], thread);
                    ctclib_thread_enqueue_middle(m, thread);
                }
#ifndef TIMER_NO_SORT
                else
                {
                    break;
                }
#endif /* TIMER_NO_SORT */
            }
        }

        /* Structure copy.  */
        readfd = m->readfd;
        writefd = m->writefd;
        exceptfd = m->exceptfd;

        /* Check any thing to be execute.  */
        if (m->queue_high.head || m->queue_middle.head || m->queue_low.head)
        {
            timer_wait = &timer_nowait;
        }
        else
        {
            timer_wait = _ctclib_thread_timer_wait(m, &timer_val);
        }
#define sal_select select
        /* First check for sockets.  Return immediately.  */
        num = sal_select (m->max_fd + 1, &readfd, &writefd, &exceptfd, timer_wait);

        /* Error handling.  */
        if (num < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

#ifdef _CENTEC_
            /* log_sys(M_MOD_LIB, E_ERROR, "Thread error: errno = %d", errno); */
            if (m->queue_high.head || m->queue_middle.head || m->queue_low.head)
            {
                continue;
            }
#else
            return NULL;
#endif
        }

        /* File descriptor is readable/writable.  */
        if (num > 0)
        {
#ifdef _CENTEC_
            ctclib_thread_process_fd(m, &m->read_urg, &readfd, &m->readfd);
#endif

            /* High priority read thread. */
            ctclib_thread_process_fd(m, &m->read_high, &readfd, &m->readfd);

            /* Normal priority read thread. */
            ctclib_thread_process_fd(m, &m->read, &readfd, &m->readfd);

            /* Write thread. */
            ctclib_thread_process_fd(m, &m->write, &writefd, &m->writefd);
        }

        /* Low priority events. */
        if ((thread = ctclib_thread_trim_head(&m->event_low)) != NULL)
        {
            ctclib_thread_enqueue_middle(m, thread);
        }
    }
}

/* Call the thread.  */
void
ctclib_thread_call (thread_t *thread)
{
    (*thread->func)(thread);
}

/* Fake execution of the thread with given argument.  */
thread_t *
ctclib_thread_execute (thread_master_t *m,
                int32 (*func)(thread_t *),
                void *arg,
                int32 val)
{
    thread_t dummy;

    sal_memset(&dummy, 0, sizeof (thread_t));

    dummy.type = CTCLIB_THREAD_EVENT;
    dummy.master = NULL;
    dummy.func = func;
    dummy.arg = arg;
    dummy.u.val = val;
    ctclib_thread_call(&dummy);

    return NULL;
}

/* Real time OS support routine.  */
#ifdef HAVE_RTOS_TIC
#ifdef RTOS_EXECUTE_ONE_THREAD
int32
ctclib_lib_tic (thread_master_t *master, thread_t *thread)
{
    if (ctclib_thread_fetch(master, thread))
    {
        ctclib_thread_call(thread);
        /* To indicate that a thread has run.  */
        return(1);
    }
    /* To indicate that no thread has run yet.  */
    return (0);
}
#else /* ! RTOS_EXECUTE_ONE_THREAD */
int32
ctclib_lib_tic (thread_master_t *master, thread_t *thread)
{
    while (ctclib_thread_fetch(master, thread))
    {
        ctclib_thread_call(thread);
    }
    return(0);
}
#endif /* ! RTOS_EXECUTE_ONE_THREAD */
#endif /* HAVE_RTOS_TIC */

void
ctclib_thread_select_arg_init(struct thread_select_arg *pSelArg)
{
    if (NULL == pSelArg)
    {
        return;
    }

    FD_ZERO(&pSelArg->readfd);
    FD_ZERO(&pSelArg->writefd);
    FD_ZERO(&pSelArg->exceptfd);
    pSelArg->max_fd = -1;
    return;
}

struct thread_select_arg *
ctclib_thread_get_select_arg(thread_master_t *m, struct thread_select_arg *pSelArg)
{
    if (NULL == m || NULL == pSelArg)
    {
        return NULL;
    }

    pSelArg->readfd = m->readfd;
    pSelArg->writefd = m->writefd;
    pSelArg->exceptfd = m->exceptfd;
    pSelArg->max_fd = m->max_fd;
    return pSelArg;
}

void
ctclib_thread_fetch_pre_process(thread_master_t *m)
{
    struct thread *thread = NULL;
    struct thread ttmp;
    struct thread *fetch = &ttmp;

    if (NULL == m)
    {
        return;
    }

    /* Pending read is exception. */
    while ((thread = ctclib_thread_trim_head (&m->read_pend)) != NULL)
    {
        fetch = ctclib_thread_run (m, thread, fetch);
        ctclib_thread_call(fetch);
    }

    /* Check ready queue.  */
    while ((thread = ctclib_thread_trim_head (&m->queue_high)) != NULL)
    {
        fetch = ctclib_thread_run (m, thread, fetch);
        ctclib_thread_call(fetch);
    }

    while ((thread = ctclib_thread_trim_head (&m->queue_middle)) != NULL)
    {
        fetch = ctclib_thread_run (m, thread, fetch);
        ctclib_thread_call(fetch);
    }

    while ((thread = ctclib_thread_trim_head (&m->queue_low)) != NULL)
    {
        fetch = ctclib_thread_run (m, thread, fetch);
        ctclib_thread_call(fetch);
    }

    /* Check events.  */
    while ((thread = ctclib_thread_trim_head (&m->event)) != NULL)
    {
        ctclib_thread_enqueue_high (m, thread);
    }

    return;
}

struct sal_timeval *
ctclib_thread_fetch_calc_timeouts(thread_master_t *m, struct sal_timeval *timer_wait)
{
    struct thread *thread = NULL;
    struct thread *next = NULL;
    static struct sal_timeval timer_val;
    struct sal_timeval timer_nowait;
    int i;

    if (NULL == m)
    {
        return NULL;
    }

    timer_nowait.tv_sec = 0;
    timer_nowait.tv_usec = 0;

    for (i = 0; i < CTCLIB_THREAD_TIMER_SLOT; i++)
    {
        for (thread = m->timer[i].head; thread; thread = next)
        {
            next = thread->next;
            if (ctclib_thread_expired(thread))
            {
                ctclib_thread_list_delete (&m->timer[i], thread);
                ctclib_thread_enqueue_middle (m, thread);
            }
#ifndef TIMER_NO_SORT
            else
            {
                break;
            }
#endif /* TIMER_NO_SORT */
        }

        /* Check any thing to be execute.  */
        if (m->queue_high.head || m->queue_middle.head || m->queue_low.head)
        {
            *timer_wait = timer_nowait;
        }
        else
        {
            timer_wait = _ctclib_thread_timer_wait (m, &timer_val);
        }
    }

    return timer_wait;
}

void
ctclib_thread_fetch_post_process(thread_master_t *m, struct thread_select_arg *pSelArg)
{
    struct thread *thread;

    if (NULL == m || NULL == pSelArg)
    {
        return;
    }

    /* File descriptor is readable/writable.  */
    /* High priority read thread. */
    ctclib_thread_process_fd (m, &m->read_high, &pSelArg->readfd, &m->readfd);

    /* Normal priority read thread. */
    ctclib_thread_process_fd (m, &m->read, &pSelArg->readfd, &m->readfd);

    /* Write thread. */
    ctclib_thread_process_fd (m, &m->write, &pSelArg->writefd, &m->writefd);

    /* Low priority events. */
    if ((thread = ctclib_thread_trim_head (&m->event_low)) != NULL)
    {
        ctclib_thread_enqueue_low (m, thread);
    }
    return;
}
