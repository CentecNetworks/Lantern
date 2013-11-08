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

/**
 * Create an async queue
 *
 * @param p_async_queue
 * @param max_queue_len
 *
 * @return
 */
 struct queue_member
{
    struct queue_member *next;
    void *data;
};
typedef struct queue_member queue_member_t;

struct sal_async_queue
{
    sal_event_t  *event;
    queue_member_t *head;
    queue_member_t *tail;
    uint32_t current_length;
    uint32_t max_length;
};

sal_mutex_t* p_sal_async_queue_mutex=NULL;

sal_err_t sal_async_queue_create(sal_async_queue_t **p_async_queue,
                                                                    uint32_t max_queue_len)
{
    sal_async_queue_t *new_queue;
    int ret;
    SAL_MALLOC(new_queue, sal_async_queue_t*, sizeof(sal_async_queue_t));
    if(new_queue==NULL)
        return ENOMEM;
    if(NULL == p_sal_async_queue_mutex)
    {
        ret = sal_mutex_create(&p_sal_async_queue_mutex);
        if(ret < 0)
            return ENOMEM;
    }
    sal_event_create(&(new_queue->event), TRUE);
    new_queue->current_length = 0;
    new_queue->max_length = max_queue_len;
    new_queue->head = 0;
    new_queue->tail = 0;
    *p_async_queue = new_queue;
    return 0;
}
/**
 * Destroy async queue
 *
 * @param async_queue
 */
void sal_async_queue_destroy(sal_async_queue_t *async_queue)
{
    sal_iflags_t iflag;
    iflag = sal_hwi_lock();
    if(async_queue->current_length!=0)
        SAL_LOG_INFO("couldn't destroy the queue because it isn't empty");
    else
    {
        sal_event_destroy(async_queue->event);
        SAL_FREE(async_queue);
    }
    sal_hwi_unlock(iflag);
    if(NULL != p_sal_async_queue_mutex)
    {
        sal_mutex_destroy(p_sal_async_queue_mutex);
        p_sal_async_queue_mutex = NULL;
    }
}

/**
 * Send data
 *
 * @param async_queue
 * @param data
 *
 * @return
 */
sal_err_t sal_async_queue_put(sal_async_queue_t *async_queue, void *data)
{
    queue_member_t* new_member;
    sal_iflags_t iflag;

    iflag = sal_hwi_lock();
    if(async_queue==NULL)
    {
        sal_hwi_unlock(iflag);
        return EADDRNOTAVAIL;
    }
    if(async_queue->current_length>=async_queue->max_length)
    {
        sal_hwi_unlock(iflag);
        return ERANGE;
    }
    else
    {
        SAL_MALLOC_ATOMIC(new_member, queue_member_t*, sizeof(queue_member_t));
        if(new_member==NULL)
        {
            sal_hwi_unlock(iflag);
            return ENOMEM;
        }
        new_member->data = data;
        new_member->next = NULL;
        if(async_queue->current_length==0)
        {
            async_queue->tail = async_queue->head = new_member;
        }
        else
        {
            async_queue->tail->next = new_member;
            async_queue->tail = new_member;
        }
        async_queue->current_length++;
        sal_event_set(async_queue->event);
    }
    sal_hwi_unlock(iflag);
    return 0;
}

/**
 * Receive data
 *
 * @param async_queue
 * @param timeout
 * @param pdata
 *
 * @return
 */
sal_err_t sal_async_queue_get(sal_async_queue_t *async_queue, int timeout,
                                                            void **pdata)
{
    sal_iflags_t iflag;
    queue_member_t* deleted_member;
    iflag = sal_hwi_lock();
    if(async_queue==NULL)
    {
        sal_hwi_unlock(iflag);
        return EADDRNOTAVAIL;
    }
    if(async_queue->current_length==0)
    {
        sal_event_reset(async_queue->event);
        sal_hwi_unlock(iflag);
        if(sal_event_wait(async_queue->event, timeout))
        {
            iflag = sal_hwi_lock();
            deleted_member = async_queue->head;
            if(async_queue->current_length==1)
                async_queue->head = async_queue->tail = NULL;
            else
                async_queue->head = async_queue->head->next;
            *pdata=deleted_member->data;
            SAL_FREE(deleted_member);
            async_queue->current_length--;
            sal_hwi_unlock(iflag);
        }
        else
        {
            return ETIMEDOUT;
        }
    }
    else
    {
        deleted_member = async_queue->head;
        if(async_queue->current_length>1)
            async_queue->head = async_queue->head->next;
        else
            async_queue->head = async_queue->tail = NULL;
        *pdata = deleted_member->data;
        SAL_FREE(deleted_member);
        async_queue->current_length--;
        sal_hwi_unlock(iflag);
    }
    return 0;
}

/**
 * Get data length
 *
 * @param async_queue
 * @param p_current
 * @param p_max
 *
 * @return
 */
sal_err_t sal_async_queue_get_count(sal_async_queue_t *async_queue,
    uint32_t *p_current, uint32_t *p_max)
{
    *p_current = async_queue->current_length;
    *p_max = async_queue->max_length;
    return 0;
}
