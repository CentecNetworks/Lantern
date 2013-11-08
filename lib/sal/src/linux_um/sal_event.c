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
#include <sys/time.h>
#include <pthread.h>

struct sal_event
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool signaled;
    bool auto_reset;
};

sal_err_t sal_event_create(sal_event_t **pevt, bool auto_reset)
{
    sal_event_t *event;

    SAL_MALLOC(event, sal_event_t *, sizeof(sal_event_t));
    if (!event)
        return ENOMEM;

    pthread_mutex_init(&event->mutex, NULL);
    pthread_cond_init(&event->cond, NULL);
    event->signaled = FALSE;
    event->auto_reset = auto_reset;

    *pevt = event;
    return 0;
}

void sal_event_destroy(sal_event_t *event)
{
    pthread_cond_destroy(&event->cond);
    pthread_mutex_destroy(&event->mutex);
    SAL_FREE(event);
}

void sal_event_set(sal_event_t *event)
{
    pthread_mutex_lock(&event->mutex);
    if (!event->signaled)
    {
        event->signaled = TRUE;
        pthread_cond_broadcast(&event->cond);
    }
    pthread_mutex_unlock(&event->mutex);
}

void sal_event_reset(sal_event_t *event)
{
    pthread_mutex_lock(&event->mutex);
    if (event->signaled)
        event->signaled = FALSE;
    pthread_mutex_unlock(&event->mutex);
}

bool sal_event_wait(sal_event_t *event, int timeout)
{
    struct timeval now;
    struct timespec tspec;
    int ret;

    if (timeout >= 0)
    {
        gettimeofday(&now, NULL);
        now.tv_sec += timeout / 1000;
        now.tv_usec += (timeout % 1000) * 1000;
        if (now.tv_usec >= 1000000)
        {
            now.tv_sec++;
            now.tv_usec -= 1000000;
        }

        tspec.tv_sec = now.tv_sec;
        tspec.tv_nsec = now.tv_usec * 1000;
    }

    pthread_mutex_lock(&event->mutex);
    while (!event->signaled)
    {
        if (timeout < 0)
            ret = pthread_cond_wait(&event->cond, &event->mutex);
        else
            ret = pthread_cond_timedwait(&event->cond, &event->mutex, &tspec);

        if (ret == ETIMEDOUT)
        {
            pthread_mutex_unlock(&event->mutex);

            return FALSE;
        }
    }

    if (event->auto_reset)
        event->signaled = FALSE;
    pthread_mutex_unlock(&event->mutex);

    return TRUE;
}
