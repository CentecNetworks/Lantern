#include "kal.h"




/*****************************************************************************
 * typdefs
 *****************************************************************************/

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
    uint32 elapsed;
    uint32 remaining;
    void (*func)(void *);
};
typedef struct timer_attr_s timer_attr_t;


/**
  @brief The function start the timer created by kal_timer_create

  @param[in] timer     the timer created by kal_timer_create
  @param[in] timeout  the timer expire value the unit is ms

  @return 0 if none error, otherwise return a negnative value
 */
kal_err_t kal_timer_start(kal_timer_t *timer, uint32 timeout)
{
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
    return 0;
}

/**
  @brief the function update the timers in timer pools using the current time
            and execute the function when the timer is expired

  @param[in] current the currrent time

  @return 0 if none error occurs, otherwise return a negnative value
*/
static INLINE int32 update_timers(struct timeval *current)
{
    return 0;
}

/**
  @brief: activate timer thread to start checking all the timers

  @param[in] void

  @return 0 if create thread successfully, otherwise return a negnative value
*/
static INLINE int32 activate_timer(void)
{
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
    return 0;
}


/**
  @brief The function destroy a timer

  @param[in] timer: the timer created by kal_timer_create

  @return void
 */
void kal_timer_destroy(kal_timer_t *timer)
{
    return;
}

/**
   @brief   temporally stop the timer, but not reset it

   @param[in] timer specified the timer index

   @return If successful, return zero otherwise a negnative value
*/
kal_err_t kal_timer_stop(kal_timer_t *timer)
{
    return 0;
}

/**
  @brief initialize the timer module

  @param[in] void

  @return If successful, return zero otherwise,
          an error number shall be returned to indicate the error
*/
kal_err_t kal_timer_init(void)
{

    return 0;
}

/**
 @brief de-initialize the timer module

 @param[in] void

 @return void
*/
void kal_timer_fini(void)
{
    return;
}

