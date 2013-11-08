#ifndef __KAL_TIMER_H__
#define __KAL_TIMER_H__

/**
 * @file kal_timer.h
 */

/**
 * @defgroup timer Timers
 * @{
 */

/** Timer Object */

#ifdef _KAL_LINUX_UM
typedef uint32 kal_timer_t;
#else
typedef struct kal_timer kal_timer_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialize timer manager
 */
kal_err_t kal_timer_init(void);

/**
 * Deinitialize timer manager
 */
void kal_timer_fini(void);

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
                           void *arg);

/**
 * Destroy timer
 *
 * @param timer
 */
void kal_timer_destroy(kal_timer_t *timer);

/**
 * Start timer
 *
 * @param timer
 * @param timeout
 *
 * @return
 */
kal_err_t kal_timer_start(kal_timer_t *timer, uint32 timeout);

/**
 * Stop timer
 *
 * @param timer
 *
 * @return
 */
kal_err_t kal_timer_stop(kal_timer_t *timer);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup timer */

#endif /* !__KAL_TIMER_H__ */

