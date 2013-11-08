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

#ifndef __SAL_ASYNC_QUEUE_H__
#define __SAL_ASYNC_QUEUE_H__

/**
 * @file sal_async_queue.h
 */

/**
 * @defgroup async_queue Asynchronous Queues
 * @{
 */

/** Async Queue Object */
typedef struct sal_async_queue sal_async_queue_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Create an async queue
 *
 * @param p_async_queue
 * @param max_queue_len
 *
 * @return
 */
sal_err_t sal_async_queue_create(sal_async_queue_t **p_async_queue, uint32_t max_queue_len);

/**
 * Destroy async queue
 *
 * @param async_queue
 */
void sal_async_queue_destroy(sal_async_queue_t *async_queue);

/**
 * Send data
 *
 * @param async_queue
 * @param data
 *
 * @return
 */
sal_err_t sal_async_queue_put(sal_async_queue_t *async_queue, void *data);

/**
 * Receive data
 *
 * @param async_queue
 * @param timeout
 * @param pdata
 *
 * @return
 */
sal_err_t sal_async_queue_get(sal_async_queue_t *async_queue, int timeout, void **pdata);

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
    uint32_t *p_current, uint32_t *p_max);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup async_queue */

#endif /* !__SAL_ASYNC_QUEUE_H__ */
