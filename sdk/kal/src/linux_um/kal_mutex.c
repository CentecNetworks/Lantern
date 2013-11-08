#include "kal.h"
#include <pthread.h>

struct kal_mutex
{
    pthread_mutex_t lock;
};

kal_err_t kal_mutex_create(kal_mutex_t **pmutex)
{
    kal_mutex_t *mutex;

    KAL_MALLOC(mutex, kal_mutex_t *, sizeof(kal_mutex_t));
    if (!mutex)
        return ENOMEM;

    pthread_mutex_init(&mutex->lock, NULL);

    *pmutex = mutex;
    return 0;
}

void kal_mutex_destroy(kal_mutex_t *mutex)
{
    pthread_mutex_destroy(&mutex->lock);
    KAL_FREE(mutex);
}

void kal_mutex_lock(kal_mutex_t *mutex)
{
    pthread_mutex_lock(&mutex->lock);
}

void kal_mutex_unlock(kal_mutex_t *mutex)
{
    pthread_mutex_unlock(&mutex->lock);
}

bool kal_mutex_try_lock(kal_mutex_t *mutex)
{
    return pthread_mutex_trylock(&mutex->lock) != EBUSY;
}

