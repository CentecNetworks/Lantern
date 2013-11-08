#include "kal.h"

struct kal_mutex
{
    struct semaphore sem;
};

kal_err_t kal_mutex_create(kal_mutex_t **pmutex)
{
    kal_mutex_t *mutex;

    KAL_MALLOC(mutex, kal_mutex_t *, sizeof(kal_mutex_t));
    if (!mutex)
        return ENOMEM;

    init_MUTEX(&mutex->sem);

    *pmutex = mutex;
    return 0;
}

void kal_mutex_destroy(kal_mutex_t *mutex)
{
    KAL_FREE(mutex);
}

void kal_mutex_lock(kal_mutex_t *mutex)
{
    down(&mutex->sem);
}

void kal_mutex_unlock(kal_mutex_t *mutex)
{
    up(&mutex->sem);
}

bool kal_mutex_try_lock(kal_mutex_t *mutex)
{
    return down_trylock(&mutex->sem) == 0;
}

