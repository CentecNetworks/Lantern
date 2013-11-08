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

#ifndef __SAL_TIMER_H__
#define __SAL_TIMER_H__

/**
 * @file sal_timer.h
 */

/**
 * @defgroup timer Timers
 * @{
 */

/** Timer Object */
typedef struct sal_timer sal_timer_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialize timer manager
 */
sal_err_t sal_timer_init(void);

/**
 * Deinitialize timer manager
 */
void sal_timer_fini(void);

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
                           void *arg);

/**
 * Destroy timer
 *
 * @param timer
 */
void sal_timer_destroy(sal_timer_t *timer);

/**
 * Start timer
 *
 * @param timer
 * @param timeout
 *
 * @return
 */
sal_err_t sal_timer_start(sal_timer_t *timer, uint32_t timeout);

/**
 * Stop timer
 *
 * @param timer
 *
 * @return
 */
sal_err_t sal_timer_stop(sal_timer_t *timer);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup timer */

#endif /* !__SAL_TIMER_H__ */
