#include "kal.h"
#include "kal_task.h"
#include "kal_mutex.h"
#include "kal_event.h"
#include "kal_mem.h"

#ifdef _KAL_LINUX_UM

#else

enum timer_state
{
    TIMER_RUNNING,
    TIMER_STOPPED,
};
typedef enum timer_state timer_state_t;

struct kal_timer
{
    timer_state_t state;
    struct timeval end;
    int32 timeout;
    void (*timer_handler)(void *);
    void *user_param;
    kal_timer_t *next;
    kal_timer_t *prev;
};

#ifndef _KAL_VXWORKS
static kal_mutex_t *timer_mutex;
static kal_event_t  *timer_event;
static kal_timer_t *head;
static kal_task_t  *timer_task;
static bool is_exit;
static bool is_inited;
#endif

int32
kal_timer_compare(struct timeval timer1,struct timeval timer2)
{
#ifndef _KAL_VXWORKS
    int32 ret;
    ret = (int32)timer1.tv_sec-(int32)timer2.tv_sec;
    if(ret!=0)
        return ret;
    else
    {
        ret = (int32)timer1.tv_usec-(int32)timer2.tv_usec;
        return ret;
    }
#endif
    return 0;
}

void
kal_timer_check(void *param)
{
#ifndef _KAL_VXWORKS
    struct timeval now;
    struct timespec ts;
    int32 timeout;
    int32 ret;
    uint32 sec, usec;
    uint32 exp_sec,exp_usec;
    kal_timer_t * temp_timer;
    kal_timer_t * timer;
    while(1)
    {
        if (is_exit==TRUE)
            kal_task_exit();

        kal_mutex_lock(timer_mutex);
        if(head!=NULL)
        {
            kal_getuptime(&ts);
            now.tv_sec = ts.tv_sec;
            now.tv_usec = ts.tv_nsec/1000;
            ret = kal_timer_compare(now,head->end);

            if(ret>=0)
            {
                head->timer_handler(head->user_param);
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
                    &&kal_timer_compare(head->end, head->next->end)>=0)
                {
                    timer = head;
                    head = head->next;
                    head->prev = NULL;
                    temp_timer = head;
                    timer->prev = NULL;
                    timer->next = NULL;
                    while(1)
                    {
                        ret = kal_timer_compare(timer->end,temp_timer->end);
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

            kal_mutex_unlock(timer_mutex);

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

                kal_mutex_unlock(timer_mutex);
                kal_event_wait(timer_event,timeout);
            }
        }
        else
        {
                kal_mutex_unlock(timer_mutex);
                kal_event_wait(timer_event,-1);
        }

    }
#endif
}
/**
 * Initialize timer manager
 */
kal_err_t kal_timer_init()
{
#ifndef _KAL_VXWORKS
    if(!is_inited)
    {
        kal_mutex_create(&timer_mutex);
        kal_event_create(&timer_event, TRUE);
        is_exit = FALSE;
        kal_task_create(&timer_task,"timer manager",0,0,kal_timer_check,NULL);
        is_inited = TRUE;
        KAL_LOG_INFO("time manager is opened sucessfully \n");
    }
    else
        KAL_LOG_INFO("Notice!!!time manager has been opened\n");
    return 0;
#endif
    return 0;
}

/**
 * Deinitialize timer manager
 */
void kal_timer_fini()
{
#ifndef _KAL_VXWORKS
    if(is_inited)
    {
        if(head==NULL)
        {
            is_exit = TRUE;
            kal_event_set(timer_event);
            kal_task_destroy(timer_task);
            kal_mutex_destroy(timer_mutex);
            kal_event_destroy(timer_event);
            is_inited = FALSE;
            KAL_LOG_INFO("time manager is closed sucessfully");
        }
        else
            KAL_LOG_INFO("Notice!!!Please close all timers before closing timer manager");
    }
    else
    {
        KAL_LOG_INFO("Notice!!!no manager is opened");
    }
#endif
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
kal_err_t kal_timer_create(kal_timer_t **ptimer,
                           void (*func)(void *),
                           void *arg)
{
#ifndef _KAL_VXWORKS
    kal_timer_t *temp_timer;
    temp_timer = (kal_timer_t *)mem_malloc(MEM_KAL_MODULE, sizeof(kal_timer_t));
    if(!temp_timer)
        return ENOMEM;
    temp_timer->state = TIMER_STOPPED;
    temp_timer->timer_handler = func;
    temp_timer->user_param = arg;
    temp_timer->next = NULL;
    temp_timer->prev = NULL;
    *ptimer = temp_timer;
#endif
    return 0;
}
/**
 * Destroy timer
 *
 * @param timer
 */
void kal_timer_destroy(kal_timer_t *timer)
{
#ifndef _KAL_VXWORKS
    if(timer->state == TIMER_RUNNING)
        kal_timer_stop(timer);
    mem_free(timer);
#endif
}
/**
 * Start timer
 *
 * @param timer
 * @param timeout
 *
 * @return
 */
kal_err_t kal_timer_start(kal_timer_t *timer, uint32_t timeout)
{
#ifndef _KAL_VXWORKS
    struct timeval now;
    struct timespec ts;
    int32 ret;
    uint32 sec, usec;
    uint32 exp_sec,exp_usec;
    kal_timer_t * temp_timer;
    if(timer!=NULL)
    {
        if(timer->state!=TIMER_RUNNING)
        {
            kal_getuptime(&ts);
            now.tv_sec = ts.tv_sec;
            now.tv_usec = ts.tv_nsec/1000;
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
            kal_mutex_lock(timer_mutex);
            if(head==NULL)
            {
                head = timer;
                kal_event_set(timer_event);
            }
            else
            {
                temp_timer = head;
                while(1)
                {
                    ret = kal_timer_compare(timer->end,temp_timer->end);
                    if(ret<0)
                    {
                        if(temp_timer->prev==NULL)
                        {
                            timer->next = temp_timer ;
                            temp_timer->prev = timer;
                            head = timer;
                            kal_event_set(timer_event);
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
            kal_mutex_unlock(timer_mutex);
        }
    }
    else
            KAL_LOG_INFO("invalid timer pointer");
#endif
    return 0;
}
/**
 * Stop timer
 *
 * @param timer
 *
 * @return
 */
kal_err_t kal_timer_stop(kal_timer_t *timer)
{
#ifndef _KAL_VXWORKS
    kal_timer_t * temp_timer;
    if(timer!=NULL)
    {
        if(timer->state ==TIMER_RUNNING)
        {
            kal_mutex_lock(timer_mutex);
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
            kal_mutex_unlock(timer_mutex);
            timer->next = NULL;
            timer->prev = NULL;
            timer->state = TIMER_STOPPED;
        }
    }
    else
            KAL_LOG_INFO("invalid timer pointer");
#endif
    return 0;
}
#endif
