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

#include "afx.h"

struct afx_mio
{
    int fd;
    afx_io_dir_t dir;
    afx_mio_cb_t cb;
    void *arg;
    afx_list_node_t node;
    afx_list_t *list;
    struct pollfd *pollfd;
};

struct afx_timer
{
    afx_timer_cb_t cb;
    void *arg;
    struct timeval end;
    uint32_t timeout;
    int is_on;
    afx_list_node_t node;
};

struct func_arg
{
    afx_mio_cb_t cb;
    void *arg;
};

static int have_changed;
static int exit_flag;
static AFX_LIST_DEF(timer_list);
static AFX_LIST_DEF(mio_list);
static AFX_LIST_DEF(ready_mio_list);


/*
 * Event Loop
 */
int afx_event_loop_create()
{
    return 0;
}

void afx_event_loop_destroy()
{
    if (!afx_list_empty(&timer_list))
        printf("memory leak !!!have not destroyed all timers before exit");
    if (!afx_list_empty(&mio_list))
        printf("memory leak!!!have not destroyed all mios before exit");
}

static int afx_timer_insert(afx_timer_t *p_timer)
{
    int ret;
    afx_list_node_t* p_node;
    afx_timer_t *timer;

    afx_list_for_each(p_node, &timer_list)
    {
        timer = afx_container_of(p_node, afx_timer_t, node);
        ret = timer->end.tv_sec - p_timer->end.tv_sec;
        if (!ret)
            ret = timer->end.tv_usec - p_timer->end.tv_usec;
        if (ret>0)
        {
            afx_list_insert_before(&timer_list, p_node, &p_timer->node);
            break;
        }
    }

    if (!p_node)
        afx_list_insert_tail(&timer_list, &p_timer->node);

    return 0;
}

int afx_event_loop_run()
{
    struct timeval now;
    struct pollfd *p1_ufds = NULL, *p2_ufds;
    afx_list_node_t *p_node;
    afx_list_node_t *p_next;
    afx_timer_t *p_timer;
    afx_mio_t *p_mio;
    int nfds=0, timeout, ret, delta_sec, delta_usec;
    uint32_t sec, exp_sec, usec, exp_usec;

    while (!exit_flag)
    {
        timeout = -1;
        while (!afx_list_empty(&timer_list))
        {
            afx_get_cur_time(&now);
            p_node = afx_list_head(&timer_list);
            p_timer = afx_container_of(p_node, afx_timer_t, node);
            ret = p_timer->end.tv_sec - now.tv_sec;
            if (!ret)
                ret = p_timer->end.tv_usec - now.tv_usec;
            if (ret>0)
            {
                delta_sec = p_timer->end.tv_sec - now.tv_sec;
                delta_usec = p_timer->end.tv_usec - now.tv_usec;
                if (delta_usec<0)
                    timeout = delta_sec*1000 -1000 + (1000000+delta_usec)/1000;
                else
                    timeout = delta_sec*1000  + delta_usec/1000;
                break;
            }
            sec = p_timer->timeout/1000;
            usec = (p_timer->timeout%1000)*1000;
            exp_sec = p_timer->end.tv_sec + sec;
            exp_usec = p_timer->end.tv_usec + usec;
            if (exp_usec>=1000000)
            {
                exp_usec-=1000000;
                exp_sec++;
            }
            p_timer->end.tv_sec = exp_sec;
            p_timer->end.tv_usec = exp_usec;
            afx_list_delete(&timer_list, p_node);
            afx_timer_insert(p_timer);
            (*p_timer->cb)(p_timer->arg);
        }

        if (have_changed)
        {
            nfds = 0;
            have_changed = 0;
            afx_list_for_each(p_node, &mio_list)
                nfds++;
            if (p1_ufds)
            {
                free(p1_ufds);
                p1_ufds = NULL;
            }
            p1_ufds = (struct pollfd*)malloc(sizeof(struct pollfd)*nfds);
            if (!p1_ufds)
               return -ENOMEM;
            p2_ufds = p1_ufds;
            afx_list_for_each(p_node, &mio_list)
            {
                p_mio = afx_container_of(p_node, afx_mio_t, node);
                p_mio->pollfd = p2_ufds;
                p2_ufds->fd = p_mio->fd;
                switch (p_mio->dir)
                {
                    case AFX_IO_IN:
                        p2_ufds->events = POLLIN;
                        break;
                    case AFX_IO_OUT:
                        p2_ufds->events = POLLOUT;
                        break;
                    case AFX_IO_ANY:
                        p2_ufds->events = POLLIN|POLLOUT;
                        break;
                }
                p2_ufds++;
            }
        }

        ret = poll(p1_ufds, nfds, timeout);
        switch (ret)
        {
            case -1:
                break;
            case 0:
                break;
            default:
                afx_list_for_each_safe(p_node, p_next, &mio_list) {
                    p_mio = afx_container_of(p_node, afx_mio_t, node);
                    p2_ufds = p_mio->pollfd;
                    if ((p2_ufds->events|POLLERR|POLLHUP) & p2_ufds->revents) {
                        afx_list_delete(&mio_list, &p_mio->node);
                        afx_list_insert_tail(&ready_mio_list, &p_mio->node);
                        p_mio->list = &ready_mio_list;
                    }
                }

                while (!afx_list_empty(&ready_mio_list)) {
                    p_node = afx_list_head(&ready_mio_list);
                    p_mio = afx_container_of(p_node, afx_mio_t, node);
                    afx_list_delete(&ready_mio_list, &p_mio->node);
                    afx_list_insert_tail(&mio_list, &p_mio->node);
                    p_mio->list = &mio_list;
                    (*p_mio->cb)(p_mio->fd, p_mio->dir, p_mio->arg);
                }
                break;
        }
    }
    if (p1_ufds)
    {
        free(p1_ufds);
        p1_ufds = NULL;
    }
    exit_flag = 0;
    return 0;
}

void afx_event_loop_exit()
{
    exit_flag = 1;
}

/*
 * Multiplexed IO
 */
int
afx_mio_create(afx_mio_t **pp_mio,
               int fd,
               afx_io_dir_t dir,
               afx_mio_cb_t cb,
               void *arg)
{
    afx_mio_t *p_mio;

    p_mio = (afx_mio_t*)malloc(sizeof(afx_mio_t));
    if (!p_mio)
        return -ENOMEM;
    p_mio->fd = fd;
    p_mio->dir = dir;
    p_mio->cb = cb;
    p_mio->arg = arg;
    *pp_mio = p_mio;
    p_mio->list = &mio_list;
    p_mio->pollfd = NULL;
    afx_list_insert_tail(&mio_list, &p_mio->node);
    have_changed = 1;
    return 0;
}

void afx_mio_destroy(afx_mio_t *p_mio)
{
    afx_list_delete(p_mio->list, &p_mio->node);
    free(p_mio);
    have_changed = 1;
}

int afx_mio_get_count(afx_mio_t *p_mio)
{
    int count = 0;
    afx_list_node_t* p_node;
    afx_list_for_each(p_node, p_mio->list)
    {
        count ++;
    }
    return count;
}

/* A fixup to let caller to set arg after call afx_mio_create immediately. */
int
afx_mio_setarg(afx_mio_t *p_mio, void *arg)
{
    p_mio->arg = arg;
    return 0;
}

/*
 * Timers
 */
int
afx_timer_create(afx_timer_t **pp_timer,
                 afx_timer_cb_t cb,
                 void *arg)
{
    afx_timer_t *p_timer;

    p_timer = malloc(sizeof(afx_timer_t));
    if (!p_timer)
        return -ENOMEM;
    p_timer->cb = cb;
    p_timer->arg = arg;
    p_timer->is_on = 0;
    *pp_timer = p_timer;
    return 0;
}

int afx_timer_destroy(afx_timer_t *p_timer)
{
    afx_timer_stop(p_timer);
    free(p_timer);
    return 0;
}

int afx_timer_start(afx_timer_t *p_timer, uint32_t timeout)
{
    struct timeval now;
    uint32_t sec, exp_sec, usec, exp_usec;

    if (p_timer->is_on)
        return -1;
    afx_get_cur_time(&now);
    sec = timeout/1000;
    usec = (timeout%1000)*1000;
    exp_sec = now.tv_sec + sec;
    exp_usec = now.tv_usec + usec;
    if (exp_usec>=1000000)
    {
        exp_usec-=1000000;
        exp_sec++;
    }
    p_timer->end.tv_sec = exp_sec;
    p_timer->end.tv_usec = exp_usec;
    p_timer->timeout = timeout;
    afx_timer_insert(p_timer);
    p_timer->is_on = 1;
    return 0;
}

int afx_timer_stop(afx_timer_t *p_timer)
{
    if (!p_timer->is_on)
    	return -1;
    afx_list_delete(&timer_list, &p_timer->node);
    p_timer->is_on = 0;
    return 0;
}

/* XXX: Use monotonic clock because it never go backward and is not affected by
 * timezone/daylightsaving settings. (OVS also prefer monotonic clock.)
 * Reference: man clock_gettime say 'sufficent recent versions GNU libc and
 * the Linux kernel support the CLOCK_MONOTONIC', so it's quite safe because
 * most of the system running our code will have a 2.6 kernel and it's sufficent
 * new.
 * AFX is the only asynchronous mechanism we used on V series product, so we only
 * need to replace the 'gettimeofday' call here. */
int afx_get_cur_time(struct timeval *tv)
{
    int ret;
    struct timespec temp_tv;
    memset(&temp_tv, 0, sizeof(temp_tv));
    ret = clock_gettime(CLOCK_MONOTONIC, &temp_tv);
    tv->tv_sec = temp_tv.tv_sec;
    tv->tv_usec = temp_tv.tv_nsec / 1000;
    return ret;
}
