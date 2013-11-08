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

#ifndef __SAL_INTR_H__
#define __SAL_INTR_H__

/**
 * @file sal_intr.h
 */

/**
 * @defgroup intr Interrupt Locking
 * @{
 */

/**
 * Interrupt flags
 */
typedef unsigned long sal_iflags_t;

/**
 * Lock hardware interrupts
 *
 * @return Saved interrupt flags
 */
static inline sal_iflags_t sal_hwi_lock(void);

/**
 * Unlock hardware interrupts
 *
 * @param iflags Saved interrupt flags returned by sal_hwi_lock()
 */
static inline void sal_hwi_unlock(sal_iflags_t iflags);

/**
 * Lock software interrupts
 */
static inline void sal_swi_lock(void);

/**
 * Unlock software interrupts
 */
static inline void sal_swi_unlock(void);

#ifdef _SAL_LINUX_KM
static inline sal_iflags_t sal_hwi_lock()
{
    unsigned long iflags;

    local_irq_save(iflags);

    return iflags;
}

static inline void sal_hwi_unlock(sal_iflags_t iflags)
{
    local_irq_restore(iflags);
}

static inline void sal_swi_lock()
{
    local_bh_disable();
}

static inline void sal_swi_unlock()
{
    local_bh_enable();
}
#else /* _SAL_LINUX_UM */
extern sal_mutex_t* p_sal_async_queue_mutex;
static inline sal_iflags_t sal_hwi_lock(void)
{
    sal_mutex_lock(p_sal_async_queue_mutex);
    return 0;
}

static inline void sal_hwi_unlock(sal_iflags_t iflags)
{
    iflags = iflags;
    sal_mutex_unlock(p_sal_async_queue_mutex);
}

static inline void sal_swi_lock(void)
{
}

static inline void sal_swi_unlock(void)
{
}
#endif

/**@}*/ /* End of @defgroup intr */

#endif /* !__SAL_INTR_H__ */
