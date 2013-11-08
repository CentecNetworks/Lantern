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

#ifndef __CTCLIB_THREAD_H__
#define __CTCLIB_THREAD_H__

struct thread_select_arg
{
    fd_set readfd;
    fd_set writefd;
    fd_set exceptfd;
    int max_fd;
};

/* Linked list of thread. */
struct thread_list
{
    struct thread *head;
    struct thread *tail;
    uint32 count;
    uint32 max_count;
} ;
typedef struct thread_list thread_list_t;
typedef struct thread_list ctclib_thread_list_t; /*This typedef is used for centec code*/


/* Master of the threads. */
struct thread_master
{
    /* Priority based queue.  */
    struct thread_list queue_high;
    struct thread_list queue_middle;
    struct thread_list queue_low;

    /* Timer */
#define CTCLIB_THREAD_TIMER_SLOT           4
    int32 index;
    struct thread_list timer[CTCLIB_THREAD_TIMER_SLOT];

    /* Thread to be executed.  */
    struct thread_list read_pend;
#ifdef _CENTEC_
    struct thread_list read_urg;
#endif
    struct thread_list read_high;
    struct thread_list read;
    struct thread_list write;
    struct thread_list event;
    struct thread_list event_low;
    struct thread_list unuse;

    sal_fd_set_t readfd;
    sal_fd_set_t writefd;
    sal_fd_set_t exceptfd;
    int32 max_fd;
    uint32 alloc;

#ifdef _CENTEC_
#define CTCLIB_THREAD_MAX_POLL           100
    void *poll_list;
    int poll_num;
#endif
} ;
typedef struct thread_master thread_master_t;
typedef struct thread_master ctclib_thread_master_t;/*This typedef is used for centec code*/

/* Thread structure. */
struct thread
{
    /* Linked list.  */
    struct thread *next;
    struct thread *prev;

    /* Pointer to the struct thread_master.  */
    struct thread_master *master;

    /* Event function.  */
    int (*func) (struct thread *);

    /* Event argument.  */
    void *arg;

    /* Thread type.  */
    int32 type;

    /* Priority.  */
    int32 priority;
#define CTCLIB_THREAD_PRIORITY_HIGH         0
#define CTCLIB_THREAD_PRIORITY_MIDDLE       1
#define CTCLIB_THREAD_PRIORITY_LOW          2

    /* Thread timer index.  */
    int32 index;

    /* Arguments.  */
    union
    {
        /* Second argument of the event.  */
        int32 val;

        /* File descriptor in case of read/write.  */
        int32 fd;

        /* Rest of time sands value.  */
        struct sal_timeval sands;
    } u;

#ifdef HAVE_HA
  /* Additions for the HA dependent processing. */
  /* This values will go with the timer checkpoint. */
    struct sal_timeval stime;  /* Timer's absolute starting time. */
    struct sal_timeval period; /* Timer's period. */
#endif /* HAVE_HA */
} ;
typedef struct thread thread_t;
typedef struct thread ctclib_thread_t;/*This typedef is used for centec code*/

/* Thread types.  */
#define CTCLIB_THREAD_READ             0
#define CTCLIB_THREAD_WRITE            1
#define CTCLIB_THREAD_TIMER            2
#define CTCLIB_THREAD_EVENT            3
#define CTCLIB_THREAD_QUEUE            4
#define CTCLIB_THREAD_UNUSED           5
#define CTCLIB_THREAD_READ_HIGH        6
#define CTCLIB_THREAD_READ_PEND        7
#define CTCLIB_THREAD_EVENT_LOW        8
#define CTCLIB_THREAD_READ_URG        9

/* Macros.  */
#define CTCLIB_THREAD_ARG(X)           ((X)->arg)
#define CTCLIB_THREAD_FD(X)            ((X)->u.fd)
#define CTCLIB_THREAD_VAL(X)           ((X)->u.val)
#if 0 /* Modified by Alexander Liu 2007-09-18 15:14 */
#define CTCLIB_THREAD_TIME_VAL(X)      ((X)->u.sands)
#else
#define CTCLIB_THREAD_TIME_VAL(X)      ctclib_thread_expire_timeval((X))
extern struct sal_timeval ctclib_thread_expire_timeval(thread_t *t);
#endif /* !_CENTEC_ */

#define CTCLIB_THREAD_READ_ON(master,thread,func,arg,sock) \
  do { \
    if (! thread) \
      thread = ctclib_thread_add_read (master, func, arg, sock); \
  } while (0)

#define CTCLIB_THREAD_WRITE_ON(master,thread,func,arg,sock) \
  do { \
    if (! thread) \
      thread = ctclib_thread_add_write (master, func, arg, sock); \
  } while (0)

#define CTCLIB_THREAD_TIMER_ON(master,thread,func,arg,time) \
  do { \
    if (! thread) \
      thread = ctclib_thread_add_timer (master, func, arg, time); \
  } while (0)

#define CTCLIB_THREAD_TIMER_ON_MSEC(master,thread,func,arg,time) \
  do { \
    if (! thread) \
      thread = ctclib_thread_add_timer_msec(master, func, arg, time); \
  } while (0)

#define CTCLIB_THREAD_OFF(thread) \
  do { \
    if (thread) \
      { \
        ctclib_thread_cancel (thread); \
        thread = NULL; \
      } \
  } while (0)

#define CTCLIB_THREAD_READ_OFF(thread)   CTCLIB_THREAD_OFF(thread)
#define CTCLIB_THREAD_WRITE_OFF(thread)  CTCLIB_THREAD_OFF(thread)
#define CTCLIB_THREAD_TIMER_OFF(thread)  CTCLIB_THREAD_OFF(thread)
#define CTCLIB_TV_ADD(A,B)         ctclib_timeval_add ((A), (B))
#define CTCLIB_ONE_SEC_MICROSECOND (1000000)
#define CTCLIB_TV_USEC_PER_SEC      CTCLIB_ONE_SEC_MICROSECOND

#define CTCLIB_TIME_MAX_TV_SEC         0x7fffffff
#define CTCLIB_TIME_MAX_TV_USEC        0x7fffffff

#define ctclib_timersub(a, b, result)                    \
do {                                                     \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;        \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;     \
    if ((result)->tv_usec < 0) {                         \
        --(result)->tv_sec;                              \
        (result)->tv_usec += 1000000;                    \
    }                                                    \
} while (0)

/* Prototypes.  */
thread_master_t*
ctclib_thread_master_create ();
thread_master_t *
ctclib_thread_get_master();
void
ctclib_thread_master_finish (thread_master_t *);
void
ctclib_thread_list_add (thread_list_t *, thread_t *);
void
ctclib_thread_list_execute (thread_master_t *, thread_list_t *);
void
ctclib_thread_list_clear (thread_master_t*, thread_list_t *);
thread_t *
ctclib_thread_get (thread_master_t *, int32, int32 (*) (thread_t *), void *);
thread_t *
ctclib_thread_add_read (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_read_high (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_read_urgency (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_write (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_timer (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_timer_timeval (thread_master_t *, int32 (*)(thread_t *), void *, struct sal_timeval);
thread_t *
ctclib_thread_add_timer_msec (thread_master_t *, int32 (*) (thread_t *), void *arg, int32);
thread_t *
ctclib_thread_add_event (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_enqueue_low (thread_master_t *m, int32 (*func) (thread_t *), void *arg, int32 val);
thread_t *
ctclib_thread_add_event_low (thread_master_t *, int32 (*)(thread_t *), void *, int32);
thread_t *
ctclib_thread_add_read_pend (thread_master_t *, int32 (*) (thread_t *), void *, int32);
void
ctclib_thread_cancel (thread_t *);
void
ctclib_thread_cancel_event (thread_master_t *, void *);
void
ctclib_thread_cancel_event_low (thread_master_t *, void *);
void
ctclib_thread_cancel_timer (thread_master_t *, void *);
void
ctclib_thread_cancel_write (thread_master_t *, void *);
void
ctclib_thread_cancel_read (thread_master_t *, void *);
thread_t *
ctclib_thread_fetch (thread_master_t *, thread_t *);
thread_t *
ctclib_thread_execute (thread_master_t *, int32 (*)(thread_t *), void *, int32);
void
ctclib_thread_call (thread_t *);
uint32
ctclib_thread_timer_remain_second (thread_t *);
uint32
ctclib_thread_timer_remain_msec (thread_t *);
bool
ctclib_thread_expired(thread_t *);
void
ctclib_thread_timer_remain_timeval(thread_t *, struct sal_timeval *);
int32
ctclib_thread_process_fd (thread_master_t *, thread_list_t *,
            sal_fd_set_t *, sal_fd_set_t *);
void
ctclib_thread_select_arg_init(struct thread_select_arg *pSelArg);
struct thread_select_arg *
ctclib_thread_get_select_arg(thread_master_t *m, struct thread_select_arg *pSelArg);
void
ctclib_thread_fetch_pre_process(thread_master_t *);
struct sal_timeval *
ctclib_thread_fetch_calc_timeouts(thread_master_t *m, struct sal_timeval *timer_wait);
void
ctclib_thread_fetch_post_process(thread_master_t *m, struct thread_select_arg *pSelArg);
struct sal_timeval *
ctclib_time_boottime(struct sal_timeval *tv);
void
ctclib_thread_enqueue_low (thread_master_t *m, thread_t *thread);
#endif /* __CTCLIB_THREAD_H__ */
