#include "kal.h"

struct kal_event
{
    spinlock_t lock;
    wait_queue_head_t wq;
    bool signaled;
    bool auto_reset;
};

kal_err_t kal_event_create(kal_event_t **pevt, bool auto_reset)
{
    kal_event_t *event;

    KAL_MALLOC(event, kal_event_t *, sizeof(kal_event_t));
    if (!event)
        return ENOMEM;

    spin_lock_init(&event->lock);
    init_waitqueue_head(&event->wq);
    event->signaled = FALSE;
    event->auto_reset = auto_reset;

    *pevt = event;
    return 0;
}

void kal_event_destroy(kal_event_t *event)
{
    KAL_FREE(event);
}

void kal_event_set(kal_event_t *event)
{
    unsigned long flags;

    spin_lock_irqsave(&event->lock, flags);
    if (!event->signaled)
    {
        event->signaled = TRUE;
        spin_unlock_irqrestore(&event->lock, flags);
        wake_up_interruptible(&event->wq);

        return;
    }
    spin_unlock_irqrestore(&event->lock, flags);
}

void kal_event_reset(kal_event_t *event)
{
    unsigned long flags;

    spin_lock_irqsave(&event->lock, flags);
    if (event->signaled)
        event->signaled = FALSE;
    spin_unlock_irqrestore(&event->lock, flags);
}

bool kal_event_wait(kal_event_t *event, int timeout)
{
    DEFINE_WAIT(wait);
    unsigned long flags;

    if (timeout > 0)
        timeout = msecs_to_jiffies(timeout);

    spin_lock_irqsave(&event->lock, flags);
    while (!event->signaled)
    {
        prepare_to_wait(&event->wq, &wait, TASK_INTERRUPTIBLE);
        spin_unlock_irqrestore(&event->lock, flags);

        if (timeout < 0)
            schedule();
        else if (timeout > 0)
            timeout = schedule_timeout(timeout);

        if (timeout == 0)
        {
            finish_wait(&event->wq, &wait);

            return FALSE;
        }

        finish_wait(&event->wq, &wait);
        spin_lock_irqsave(&event->lock, flags);
    }

    if (event->auto_reset)
        event->signaled = FALSE;
    spin_unlock_irqrestore(&event->lock, flags);

    return TRUE;
}

