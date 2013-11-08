#ifndef __KAL_EVENT_H__
#define __KAL_EVENT_H__

/**
 * @file kal_event.h
 */

/**
 * @defgroup event Events
 * @{
 */

/**
 * The kal_event_t object is used to signal between threads that a particular
 * event has occurred
 */
typedef struct kal_event kal_event_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Create a new event object
 *
 * @param[out] pevt
 * @param[in]  auto_reset
 *
 * @return
 */
kal_err_t kal_event_create(kal_event_t **pevt, bool auto_reset);

/**
 * Destroy the event object
 *
 * @param[in] event
 */
void kal_event_destroy(kal_event_t *event);

/**
 * Set the given event to the signaled state
 *
 * @param[in] event
 */
void kal_event_set(kal_event_t *event);

/**
 * Set the given event to the non-signaled state
 *
 * @param[in] event
 */
void kal_event_reset(kal_event_t *event);

/**
 * The <em>kal_event_wait</em> function returns when the given event is in the
 * signaled state or the <em>timeout</em> interval elapses
 *
 * @param[in] event
 * @param[in] timeout
 *
 * @retval TRUE  The event is in the signaled state
 * @retval FALSE The wait is timed out
 */
bool kal_event_wait(kal_event_t *event, int32 timeout);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup event */

#endif /* !__KAL_EVENT_H__ */

