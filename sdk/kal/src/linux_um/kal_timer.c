#include "kal.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>



/*****************************************************************************
 * typdefs
 *****************************************************************************/
static bool kal_timer_activated;
static bool ctckal_timer_activated;
static bool is_inited = FALSE;

static pthread_t timer_pthread;

enum timer_state_e
{
    TIMER_INVALID,
    TIMER_PENDING,
    TIMER_RUNNING,
    TIMER_STOPPED,
    TIMER_USER
};
typedef enum timer_state_e timer_state_t;

/* global timer storage */
struct timer_attr_s
{
    timer_state_t state;
    uint32 elapsed;
    uint32 remaining;
    struct timeval start;
    void (*func)(void *);
};
typedef struct timer_attr_s timer_attr_t;

#define TIMER_STORAGE_SIZE 256  /* can be configured */
static timer_attr_t timer_storage[TIMER_STORAGE_SIZE];
static pthread_mutex_t timer_storage_mutex;
static pthread_mutex_t timer_concurrent_mutex;

/**
  @brief   This function substract timer x from timer y

  @param[out] result  time interval between x and y
  @param[in] x the start timer
  @param[in] y the end timer

  @return when x - y > 0 return the time interval between x and y, otherwise return negative value
 */
static int32 substract_timeval(struct timeval *result,
                                struct timeval *x, struct timeval *y)
{
    if (   (x->tv_sec < y->tv_sec)
        || ((x->tv_sec == y->tv_sec) && (x->tv_usec < y->tv_usec)))
    {
        KAL_LOG_INFO( "x:tv_sec %d, tv_usec %d; y:tv_sec %d, tv_usec %d\n",
                    x->tv_sec, x->tv_usec, y->tv_sec, y->tv_usec);
        return -1;
    }

    if (x->tv_usec > y->tv_usec)
        result->tv_usec = x->tv_usec - y->tv_usec;
    else
    {
        result->tv_usec = x->tv_usec + 1000000 - y->tv_usec;
        result->tv_sec = x->tv_sec - y->tv_sec;
    }

    return 0;
}

/**
  @brief This function get the timer from timer pool

  @param[in] void

  @return when successful return the index, otherwise return a nagative value
*/
static int32 get_timer(void)
{
    int32 i;

    for (i = 0; i < TIMER_STORAGE_SIZE; i++)
    {
        if (TIMER_INVALID == timer_storage[i].state)
        {
            timer_storage[i].state = TIMER_PENDING;
            break;
        }
    }

    if (TIMER_STORAGE_SIZE == i)
        return -1;

    return i;
}

/**
  @brief  This function fill the timer with expire time and recore the current tick

  @param[in] timer_attr the expire time

  @return if set expire time and record the current tick successful return the none error, otherwise return a negative value
*/
static int32 reset_timer(timer_attr_t *timer_attr)
{
    int32 ret;
    struct timezone tz;

    ret = gettimeofday(&timer_attr->start, &tz);
    timer_attr->remaining = timer_attr->elapsed;

    if (ret != 0)
    {
        KAL_LOG_INFO(  "gettimeofday: %d\n", ret);
        timer_attr->state = TIMER_INVALID;
        return -1;
    }

    timer_attr->state = TIMER_RUNNING;

    return 0;
}

/**
 @brief  the function check whether the timer been expired

 @param[in] idx the timer index
 @param[in] current the current time

 @return if the timer been expired return TRUR otherwise return FALSE
*/
static INLINE int32 check_expiration(int32 idx, struct timeval *current)
{
    int32 sec, usec;
    int32 expire_sec, expire_usec;

    sec = timer_storage[idx].remaining / 1000;
    usec = (timer_storage[idx].remaining % 1000) * 1000;

    expire_sec = timer_storage[idx].start.tv_sec + sec;
    expire_usec = timer_storage[idx].start.tv_usec + usec;
    if (expire_usec > 1000000)
    {
        expire_usec -= 1000000;
        expire_sec++;
    }

    if (current->tv_sec > expire_sec)
        return TRUE;
    else if (current->tv_sec == expire_sec
          && current->tv_usec > expire_usec)
        return TRUE;

    return FALSE;
}

/**
  @brief the function update the timers in timer pools using the current time
            and execute the function when the timer is expired

  @param[in] current the currrent time

  @return 0 if none error occurs, otherwise return a negnative value
*/
static INLINE int32 update_timers(struct timeval *current)
{
    int32 ret = 0;
    bool expired;
    int32 i;

    for (i = 0; i < TIMER_STORAGE_SIZE; ++i)
    {
        if (TIMER_RUNNING != timer_storage[i].state)
        {
            continue;
        }
        expired = check_expiration(i, current);
        if (expired)
        {
            if (!kal_timer_activated)
            {
                KAL_LOG_INFO( "!kal_timer_activated");
                ret = -1;
                goto UPDATE_TIMER_ERROR;
            }

            pthread_mutex_lock(&timer_concurrent_mutex);
            ret = reset_timer(&timer_storage[i]);
            timer_storage[i].func((void *)&i);
            pthread_mutex_unlock(&timer_concurrent_mutex);

            if (ret != 0)
            {
                ret = -1;
                goto UPDATE_TIMER_ERROR;
            }
        }
    }
UPDATE_TIMER_ERROR:
    return ret;
}

/**
  @brief the function is the daemon threads for update all timer in pools regularly

  @param[in] param not used yet

  @return 0 if none error occures, otherwise return a negnative value
*/
static void *kal_timer_func(void *param)
{
    int32 ret = 0;
    struct timeval current;
    struct timezone tz;

    while (kal_timer_activated)
    {
        usleep(1000);

        ret = gettimeofday(&current, &tz);
        if (ret != 0)
        {
            KAL_LOG_INFO("gettimeofday: %d\n", ret);
            kal_timer_activated = FALSE;
            break;
        }

        ret = update_timers(&current);
        if (ret != 0)
        {
            kal_timer_activated = FALSE;
            break;
        }
    }

    return NULL;
}

/**
  @brief: activate timer thread to start checking all the timers

  @param[in] void

  @return 0 if create thread successfully, otherwise return a negnative value
*/
static INLINE int32 activate_timer(void)
{
    int32 ret;

    kal_timer_activated = TRUE;

    /* create a thread for the timer */
    ret = pthread_create(&timer_pthread, NULL, kal_timer_func, NULL);
    if (ret != 0)
    {
        KAL_LOG_INFO("pthread_create: %d\n", ret);
        kal_timer_activated = FALSE;
        return -1;
    }

    return 0;
}

/**
 @brief  create a timer and set the function when timeout to be called

 @param[in]  ptimer a pointer to pointer used to store the timer index
 @param[in]  func the function to be called after timer expired
 @param[in]  arg the argument for func

 @return 0 if none error, otherwise return a negnative value
*/
kal_err_t kal_timer_create(kal_timer_t **ptimer,  void (*func)(void *), void *arg)
{
    int32 ret = 0;
    int32 idx = -1;
    struct timezone tz;
    kal_timer_t *temp_timer = NULL;

    pthread_mutex_lock(&timer_storage_mutex);
    temp_timer = (kal_timer_t *)mem_malloc(MEM_KAL_MODULE, sizeof(kal_timer_t));

    if (NULL == temp_timer || NULL == func)
    {
        ret = -1;
        goto CREATE_TIMER_ERROR;
    }

    idx = get_timer();
    if (idx < 0)
    {
        ret = -2;
        goto CREATE_TIMER_ERROR;
    }

    timer_storage[idx].elapsed = 0;
    timer_storage[idx].remaining = 0;
    timer_storage[idx].func = func;
    timer_storage[idx].state = TIMER_STOPPED;
    ret = gettimeofday(&timer_storage[idx].start, &tz);
    if (ret != 0)
    {
        ret = -5;
        goto CREATE_TIMER_ERROR;
    }

    *temp_timer= (kal_timer_t)idx;

    if (!ctckal_timer_activated)
    {
        ret = activate_timer();
        if (ret != 0)
        {
            ret = -6;
            goto CREATE_TIMER_ERROR;
        }
    }

    *ptimer = temp_timer;

    pthread_mutex_unlock(&timer_storage_mutex);
    return ret;

CREATE_TIMER_ERROR:
    if (temp_timer)
		    mem_free(temp_timer);
    if (idx >= 0)
        timer_storage[idx].state = TIMER_INVALID;
    pthread_mutex_unlock(&timer_storage_mutex);

    return ret;
}

/**
  @brief The function start the timer created by kal_timer_create

  @param[in] timer     the timer created by kal_timer_create
  @param[in] timeout  the timer expire value the unit is ms

  @return 0 if none error, otherwise return a negnative value
 */
kal_err_t kal_timer_start(kal_timer_t *timer, uint32 timeout)
{
    int32 ret = 0;
    kal_timer_t idx;

    pthread_mutex_lock(&timer_storage_mutex);

    if (!timer)
    {
        KAL_LOG_INFO( "ERROR: start a null point timer\n");
        ret = -1;
        goto START_TIMER_ERROR;
    }

    idx = *timer;
    if (idx >= TIMER_STORAGE_SIZE)
    {
        KAL_LOG_INFO( "Error: idx = %d\n", idx);
        ret = -1;
        goto START_TIMER_ERROR;
    }

    if (TIMER_INVALID == timer_storage[idx].state)
    {
        KAL_LOG_INFO( "%s %d Error: timer state = %ld\n", timer_storage[idx].state);
        ret = -1;
        goto START_TIMER_ERROR;
    }

    ret = gettimeofday(&timer_storage[idx].start, NULL);
    if (ret != 0)
    {
        KAL_LOG_INFO("gettimeofday: %d\n", ret);
        ret = -1;
        goto START_TIMER_ERROR;
    }

    if (timeout)
    {
        timer_storage[idx].remaining= timeout;
        timer_storage[idx].elapsed = timeout;
    }
	  timer_storage[idx].state = TIMER_RUNNING;

START_TIMER_ERROR:
    pthread_mutex_unlock(&timer_storage_mutex);

    return ret;
}

/**
  @brief The function destroy a timer

  @param[in] timer: the timer created by kal_timer_create

  @return void
 */
void kal_timer_destroy(kal_timer_t *timer)
{
    kal_timer_t idx;
    int32 i;

    pthread_mutex_lock(&timer_storage_mutex);
    if (timer)
  	{
  	    idx = *timer;
		    mem_free(timer);
  	}
  	else
  	{
  	    KAL_LOG_INFO("kal_timer_destroy: to destory a NULL point timer\n");
  	    goto DESTROY_TIMER_ERROR;
  	}

    if (idx >= TIMER_STORAGE_SIZE
        || TIMER_INVALID == timer_storage[idx].state)
        goto DESTROY_TIMER_ERROR;

    timer_storage[idx].state = TIMER_INVALID;

    for (i = 0; i < TIMER_STORAGE_SIZE; i++)
    {
        if (timer_storage[i].state != TIMER_INVALID)
            goto DESTROY_TIMER_ERROR;
    }
    kal_timer_activated = FALSE;

DESTROY_TIMER_ERROR:
    pthread_mutex_unlock(&timer_storage_mutex);
}

/**
   @brief   temporally stop the timer, but not reset it

   @param[in] timer specified the timer index

   @return If successful, return zero otherwise a negnative value
*/
kal_err_t kal_timer_stop(kal_timer_t *timer)
{
    int32 ret = 0;
    int32 idx;
    struct timeval current;
    struct timezone tz;
    struct timeval result;

    pthread_mutex_lock(&timer_storage_mutex);
    memset(&result, 0, sizeof(struct timeval));

    if (!timer)
    {
        KAL_LOG_INFO( "ERROR: stop a null point timer\n");
        ret = -1;
        goto STOP_TIMER_ERROR;
    }

    idx = (int32)*timer;
    if (idx < 0 || idx >= TIMER_STORAGE_SIZE)
    {
        KAL_LOG_INFO( "Error: idx = %d\n", idx);
        ret = -1;
        goto STOP_TIMER_ERROR;
    }

    if (timer_storage[idx].state != TIMER_RUNNING)
    {
        KAL_LOG_INFO( "Error: timer state = %ld\n", timer_storage[idx].state);
        ret = -1;
        goto STOP_TIMER_ERROR;
    }

    ret = gettimeofday(&current, &tz);
    if (ret != 0)
    {
        KAL_LOG_INFO( "gettimeofday: %d\n", ret);
        ret = -1;
        goto STOP_TIMER_ERROR;
    }

    ret = substract_timeval(&result, &current, &timer_storage[idx].start);
    if (ret != 0)
    {
        ret = -1;
        goto STOP_TIMER_ERROR;
    }

    timer_storage[idx].remaining -= ((result.tv_sec * 1000)
                                     + (result.tv_usec / 1000));

    timer_storage[idx].state = TIMER_STOPPED;

STOP_TIMER_ERROR:

    pthread_mutex_unlock(&timer_storage_mutex);
	  return ret;
}

/**
  @brief initialize the timer module

  @param[in] void

  @return If successful, return zero otherwise,
          an error number shall be returned to indicate the error
*/
kal_err_t kal_timer_init(void)
{
    int32 ret = 0;

    if (!is_inited)
    {
        memset(timer_storage, 0, sizeof(timer_attr_t) * TIMER_STORAGE_SIZE);
        ctckal_timer_activated = FALSE;
        ret = pthread_mutex_init(&timer_storage_mutex, NULL);
        ret += pthread_mutex_init(&timer_concurrent_mutex, NULL);
        is_inited = TRUE;
    }

	  return ret;
}

/**
 @brief de-initialize the timer module

 @param[in] void

 @return void
*/
void kal_timer_fini(void)
{
    if (is_inited)
    {
      pthread_mutex_destroy(&timer_storage_mutex);
      pthread_mutex_destroy(&timer_concurrent_mutex);
      kal_timer_activated = FALSE;
      is_inited = FALSE;
    }
}

