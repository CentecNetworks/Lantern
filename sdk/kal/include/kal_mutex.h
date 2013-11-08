#ifndef __KAL_MUTEX_H__
#define __KAL_MUTEX_H__

/**
 * @file kal_mutex.h
 */

/**
 * @defgroup mutex Mutexes
 * @{
 */

/** Mutex Object */
typedef struct kal_mutex kal_mutex_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Create a new mutex
 *
 * @param[out] pmutex
 *
 * @return
 */
kal_err_t kal_mutex_create(kal_mutex_t **pmutex);

/**
 * Destroy the mutex
 *
 * @param[in] mutex
 */
void kal_mutex_destroy(kal_mutex_t *mutex);

/**
 * Lock the mutex
 *
 * @param[in] mutex
 */
void kal_mutex_lock(kal_mutex_t *mutex);

/**
 * Unlock the mutex
 *
 * @param[in] mutex
 */
void kal_mutex_unlock(kal_mutex_t *mutex);

/**
 * Try to lock the mutex
 *
 * @param[in] mutex
 *
 * @return
 */
bool kal_mutex_try_lock(kal_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup mutex */

#endif /* !__KAL_MUTEX_H__ */

