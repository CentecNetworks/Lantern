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

#ifndef __SAL_MUTEX_H__
#define __SAL_MUTEX_H__

/**
 * @file sal_mutex.h
 */

/**
 * @defgroup mutex Mutexes
 * @{
 */

/** Mutex Object */
typedef struct sal_mutex sal_mutex_t;

typedef struct sal_cond sal_cond_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Create a new recursive mutex
 *
 * @param[out] pmutex
 *
 * @return
 */
sal_err_t sal_mutex_recursive_create(sal_mutex_t **pmutex);

/**
 * Create a new mutex
 *
 * @param[out] pmutex
 *
 * @return
 */
sal_err_t sal_mutex_create(sal_mutex_t **pmutex);

/**
 * Create a new condition
 *
 * @param[out] pcond
 *
 * @return
 */
sal_err_t sal_cond_create(sal_cond_t** pcond);

/**
 * Destroy the mutex
 *
 * @param[in] mutex
 */
void sal_mutex_destroy(sal_mutex_t *mutex);

/**
 * Lock the mutex
 *
 * @param[in] mutex
 */
void sal_mutex_lock(sal_mutex_t *mutex);

/**
 * Unlock the mutex
 *
 * @param[in] mutex
 */
void sal_mutex_unlock(sal_mutex_t *mutex);

/**
 * Try to lock the mutex
 *
 * @param[in] mutex
 *
 * @return
 */
bool sal_mutex_try_lock(sal_mutex_t *mutex);

/**
 * Signal a condition
 */
void sal_task_cond_signal(sal_cond_t* p_cond);

/**
 * Wait on a condition
 */
void sal_task_cond_wait(sal_mutex_t* p_mutex, sal_cond_t* p_cond);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup mutex */

#endif /* !__SAL_MUTEX_H__ */
