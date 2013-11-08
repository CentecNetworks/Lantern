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

struct sal_mutex
{
    struct semaphore sem;
};

sal_err_t sal_mutex_create(sal_mutex_t **pmutex)
{
    sal_mutex_t *mutex;

    SAL_MALLOC(mutex, sal_mutex_t *, sizeof(sal_mutex_t));
    if (!mutex)
        return ENOMEM;

    init_MUTEX(&mutex->sem);

    *pmutex = mutex;
    return 0;
}

void sal_mutex_destroy(sal_mutex_t *mutex)
{
    SAL_FREE(mutex);
}

void sal_mutex_lock(sal_mutex_t *mutex)
{
    down(&mutex->sem);
}

void sal_mutex_unlock(sal_mutex_t *mutex)
{
    up(&mutex->sem);
}

bool sal_mutex_try_lock(sal_mutex_t *mutex)
{
    return down_trylock(&mutex->sem) == 0;
}
