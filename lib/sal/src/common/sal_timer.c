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

#include "sal.h"
#include "sal_task.h"
#include"sal_mutex.h"
#include"sal_event.h"
#include"sal_mem.h"

enum timer_state
{
    TIMER_RUNNING,
    TIMER_STOPPED,
};
typedef enum timer_state timer_state_t;

struct sal_timer
{
    timer_state_t state;
    struct timeval end;
    int32 timeout;
    void (*timer_handler)(void *);
    void *user_param;
    sal_timer_t *next;
    sal_timer_t *prev;
};

static sal_mutex_t *timer_mutex;
static sal_event_t  *timer_event;
static sal_timer_t *head;
static sal_task_t  *timer_task;
static bool is_exit;
static bool is_inited;

int32
sal_timer_compare(struct timeval timer1,struct timeval timer2)
{
    int32 ret;
    ret = (int32)timer1.tv_sec-(int32)timer2.tv_sec;
    if(ret!=0)
        return ret;
    else
    {
        ret = (int32)timer1.tv_usec-(int32)timer2.tv_usec;
        return ret;
    }
}

static inline struct sal_timeval *
_sal_timer_boottime(struct sal_timeval *tv)
{
    /*
     * (struct timezone *)-1 means get relative time/uptime
     * refer to kernel/time.c sys_gettimeofday()
     */
    gettimeofday(tv, (struct timezone *)-1);
    return tv;
}
void
sal_timer_check(void *param)
{
    struct timeval now;
    int32 timeout;
    int32 ret;
    uint32 sec, usec;
    uint32 exp_sec,exp_usec;
    sal_timer_t * temp_timer;
    sal_timer_t * timer;
    sal_timer_t *head_tmp = NULL;
    while(1)
    {
        if (is_exit==TRUE)
            sal_task_exit();

        sal_mutex_lock(timer_mutex);
        if(head!=NULL)
        {
            /*sal_getuptime(&ts);
            now.tv_sec = ts.tv_sec;
            now.tv_usec = ts.tv_nsec/1000;*/
            _sal_timer_boottime(&now);
            ret = sal_timer_compare(now,head->end);

            if(ret>=0)
            {
                head_tmp = head;
                head->timer_handler(head->user_param);
                if(head_tmp != head)
                {
                    sal_mutex_unlock(timer_mutex);
                    continue;
                }
                sec = head->timeout/1000;
                usec = (head->timeout%1000)*1000;
                exp_sec = head->end.tv_sec + sec;
                exp_usec = head->end.tv_usec + usec;

                if(exp_usec>=1000000)
                {
                    exp_usec-=1000000;
                    exp_sec++;
                }
                head->end.tv_sec = exp_sec;
                head->end.tv_usec = exp_usec;
                if(head->next!=NULL
                    &&sal_timer_compare(head->end, head->next->end)>=0)
                {
                    timer = head;
                    head = head->next;
                    head->prev = NULL;
                    temp_timer = head;
                    timer->prev = NULL;
                    timer->next = NULL;
                    while(1)
                    {
                        ret = sal_timer_compare(timer->end,temp_timer->end);
                        if(ret<0)
                        {
                                temp_timer->prev->next = timer;
                                timer->prev = temp_timer->prev;
                                temp_timer->prev = timer;
                                timer->next = temp_timer;
                                break;
                        }
                        else if(temp_timer->next!=NULL)
                        {
                            temp_timer = temp_timer->next;
                        }
                        else
                        {
                            temp_timer->next = timer;
                            timer->prev = temp_timer;
                            break;
                        }
                    }
                }

            sal_mutex_unlock(timer_mutex);

            }
            else
            {
                if(head->end.tv_sec !=now.tv_sec)
                {
                    if(head->end.tv_usec>=now.tv_sec)
                        timeout = (head->end.tv_sec -now.tv_sec)*1000+
                            (head->end.tv_usec-now.tv_usec)/1000;
                    else
                        timeout = (head->end.tv_sec -now.tv_sec-1)*1000+1000-
                            (now.tv_usec-head->end.tv_usec)/1000;
                }
                else
                    timeout = (head->end.tv_usec-now.tv_usec)/1000;

                sal_mutex_unlock(timer_mutex);
                sal_event_wait(timer_event,timeout);
            }
        }
        else
        {
                sal_mutex_unlock(timer_mutex);
                sal_event_wait(timer_event,-1);
        }

    }
}

/**
 * Initialize timer manager
 */
sal_err_t sal_timer_init()
{
    if(!is_inited)
    {
        sal_mutex_create(&timer_mutex);
        sal_event_create(&timer_event, TRUE);
        is_exit = FALSE;
        /* reduce thread stack memory */
        sal_task_create(&timer_task,"timer manager",256*1024,sal_timer_check,NULL);
        is_inited = TRUE;
        SAL_LOG_INFO("time manager is opened successfully");
    }
    else
        SAL_LOG_INFO("Notice!!!time manager has been opened");
    return 0;
}

/**
 * Deinitialize timer manager
 */
void sal_timer_fini()
{
    if(is_inited)
    {
        if(head==NULL)
        {
            is_exit = TRUE;
            sal_event_set(timer_event);
            sal_task_destroy(timer_task);
            sal_mutex_destroy(timer_mutex);
            sal_event_destroy(timer_event);
            is_inited = FALSE;
            SAL_LOG_INFO("time manager is closed successfully");
        }
        else
            SAL_LOG_INFO("Notice!!!Please close all timers before closing timer manager");
    }
    else
    {
        SAL_LOG_INFO("Notice!!!no manager is opened");
    }
}

/**
 * Create a timer
 *
 * @param ptimer
 * @param func
 * @param arg
 *
 * @return
 */
sal_err_t sal_timer_create(sal_timer_t **ptimer,
                           void (*func)(void *),
                           void *arg)
{
    sal_timer_t *temp_timer;
    SAL_MALLOC(temp_timer, sal_timer_t*, sizeof(sal_timer_t));
    if(!temp_timer)
        return ENOMEM;
    temp_timer->state = TIMER_STOPPED;
    temp_timer->timer_handler = func;
    temp_timer->user_param = arg;
    temp_timer->next = NULL;
    temp_timer->prev = NULL;
    *ptimer = temp_timer;
    return 0;
}

/**
 * Destroy timer
 *
 * @param timer
 */
void sal_timer_destroy(sal_timer_t *timer)
{
    if(timer->state == TIMER_RUNNING)
        sal_timer_stop(timer);
    SAL_FREE(timer);
}
/**
 * Start timer
 *
 * @param timer
 * @param timeout
 *
 * @return
 */
sal_err_t sal_timer_start(sal_timer_t *timer, uint32_t timeout)
{
    struct timeval now;
    /*struct timespec ts;*/
    int32 ret;
    uint32 sec, usec;
    uint32 exp_sec,exp_usec;
    sal_timer_t * temp_timer;
    if(timer!=NULL)
    {
        if(timer->state!=TIMER_RUNNING)
        {
            /*sal_getuptime(&ts);
            now.tv_sec = ts.tv_sec;
            now.tv_usec = ts.tv_nsec/1000;*/
            _sal_timer_boottime(&now);
            sec = timeout/1000;
            usec = (timeout%1000)*1000;
            exp_sec = now.tv_sec + sec;
            exp_usec = now.tv_usec + usec;
            if(exp_usec>=1000000)
            {
                exp_usec-=1000000;
                exp_sec++;
            }
            timer->end.tv_sec = exp_sec;
            timer->end.tv_usec = exp_usec;
            timer->timeout = timeout;
            sal_mutex_lock(timer_mutex);
            if(head==NULL)
            {
                head = timer;
                sal_event_set(timer_event);
            }
            else
            {
                temp_timer = head;
                while(1)
                {
                    ret = sal_timer_compare(timer->end,temp_timer->end);
                    if(ret<0)
                    {
                        if(temp_timer->prev==NULL)
                        {
                            timer->next = temp_timer ;
                            temp_timer->prev = timer;
                            head = timer;
                            sal_event_set(timer_event);
                            break;
                        }
                        else
                        {
                            temp_timer->prev->next = timer;
                            timer->prev = temp_timer->prev;
                            temp_timer->prev = timer;
                            timer->next = temp_timer;
                            break;
                        }
                    }
                    else if(temp_timer->next!=NULL)
                    {
                        temp_timer = temp_timer->next;
                    }
                    else
                    {
                        temp_timer->next = timer;
                        timer->prev = temp_timer;
                        break;
                    }
                }
            }
            timer->state = TIMER_RUNNING;
            sal_mutex_unlock(timer_mutex);
        }
    }
    else
            SAL_LOG_INFO("invalid timer pointer");
    return 0;
}
/**
 * Stop timer
 *
 * @param timer
 *
 * @return
 */
sal_err_t sal_timer_stop(sal_timer_t *timer)
{
    sal_timer_t * temp_timer;
    if(timer!=NULL)
    {
        if(timer->state ==TIMER_RUNNING)
        {
            /*sal_mutex_lock(timer_mutex);*/
            temp_timer = head;
            while(1)
            {
            if(temp_timer == timer)
            {
                if(temp_timer->prev==NULL)
                {
                    if(temp_timer->next==NULL)
                    {
                        head = NULL;
                        break;
                    }
                    else
                    {
                        head = temp_timer->next;
                        head->prev = NULL;
                        break;
                    }
                }
                else if(temp_timer->next==NULL)
                {
                    temp_timer->prev->next = NULL;
                    break;
                }
                else
                {
                    temp_timer->next->prev = temp_timer->prev;
                    temp_timer->prev->next = temp_timer->next;
                    break;
                }
            }
            else
                temp_timer = temp_timer->next;
            }
            /*sal_mutex_unlock(timer_mutex);*/
            timer->next = NULL;
            timer->prev = NULL;
            timer->state = TIMER_STOPPED;
        }
    }
    else
            SAL_LOG_INFO("invalid timer pointer");
    return 0;
}
