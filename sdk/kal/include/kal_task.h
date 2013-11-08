#ifndef __KAL_TASK_H__
#define __KAL_TASK_H__

/**
 * @file kal_task.h
 */

/**
 * @defgroup task Tasks
 * @{
 */

/** Task Object */
typedef struct kal_task kal_task_t;

#ifdef __cplusplus
extern "C"
{
#endif


/** system time */
typedef struct kal_systime_s
{
    unsigned int tv_sec;     /* seconds */
    unsigned int tv_usec;    /* microseconds */
} kal_systime_t;




/**
 * Create a new task
 *
 * @param[out] ptask
 * @param[in]  name
 * @param[in]  stack_size
 * @param[in]  start
 * @param[in]  arg
 *
 * @return
 */
kal_err_t kal_task_create(kal_task_t **ptask,
                          char *name,
                          size_t stack_size,
                          int prio,
                          void (*start)(void *),
                          void *arg);

/**
 * Destroy the task
 *
 * @param[in] task
 */
void kal_task_destroy(kal_task_t *task);

/**
 * Exit the current executing task
 */
void kal_task_exit(void);

/**
 * Sleep for <em>msec</em> milliseconds
 *
 * @param[in] msec
 */
void kal_task_sleep(uint32_t msec);

/**
 * Yield the processor voluntarily to other tasks
 */
void kal_task_yield(void);

/**
 * Get current time
 *
 * @param tv
 */
void kal_gettime(kal_systime_t* tv);

/**
 * Get uptime since last startup
 *
 * @param ts
 */
void kal_getuptime(struct timespec *ts);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup task */

#endif /* !__KAL_TASK_H__ */

