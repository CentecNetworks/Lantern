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
#include <pthread.h>

struct sal_mutex
{
    pthread_mutex_t lock;
};

struct sal_cond
{
    pthread_cond_t cond;
};

sal_err_t sal_mutex_recursive_create(sal_mutex_t** pmutex)
{
    sal_mutex_t* mutex;
    pthread_mutexattr_t attr;

    SAL_MALLOC(mutex, sal_mutex_t *, sizeof(sal_mutex_t));
    if (!mutex)
        return ENOMEM;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex->lock, &attr);
    pthread_mutexattr_destroy(&attr);

    *pmutex = mutex;
    return 0;
}

sal_err_t sal_mutex_create(sal_mutex_t** pmutex)
{
    sal_mutex_t* mutex;

    SAL_MALLOC(mutex, sal_mutex_t *, sizeof(sal_mutex_t));
    if (!mutex)
        return ENOMEM;

    pthread_mutex_init(&mutex->lock, NULL);

    *pmutex = mutex;
    return 0;
}

sal_err_t sal_cond_create(sal_cond_t** pcond)
{
    sal_cond_t* cond;

    SAL_MALLOC(cond, sal_cond_t *, sizeof(sal_cond_t));
    if (!cond)
        return ENOMEM;

    pthread_cond_init(&cond->cond, NULL);

    *pcond = cond;
    return 0;
}

void sal_mutex_destroy(sal_mutex_t *mutex)
{
    pthread_mutex_destroy(&mutex->lock);
    SAL_FREE(mutex);
}

void sal_mutex_lock(sal_mutex_t *mutex)
{
    pthread_mutex_lock(&mutex->lock);
}

void sal_mutex_unlock(sal_mutex_t *mutex)
{
    pthread_mutex_unlock(&mutex->lock);
}

bool sal_mutex_try_lock(sal_mutex_t *mutex)
{
    return pthread_mutex_trylock(&mutex->lock) != EBUSY;
}

void sal_task_cond_signal(sal_cond_t* p_cond)
{
    pthread_cond_signal(&p_cond->cond);
}

void sal_task_cond_wait(sal_mutex_t* p_mutex, sal_cond_t* p_cond)
{
    pthread_cond_wait(&p_cond->cond, &p_mutex->lock);
}
