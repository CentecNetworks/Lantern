#include "kal.h"
#include <pthread.h>

typedef struct vx_mutex_s
{
    int8 mutex_opaque_type;
}*vx_mutex_t;

struct kal_mutex{
    vx_mutex_t kal_vx_mutex;
};
kal_err_t kal_mutex_create(kal_mutex_t **pmutex)
{
    SEM_ID sem;
    kal_mutex_t *mutex;

    mutex = (kal_mutex_t *)mem_malloc(MEM_KAL_MODULE, sizeof(kal_mutex_t));
    if (!mutex)
        return -1;
    sem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE |  SEM_INVERSION_SAFE);
    mutex->kal_vx_mutex = (vx_mutex_t)sem;
    *pmutex = mutex;

    return 0;
}

void
kal_mutex_destroy(kal_mutex_t *mutex)
{
    SEM_ID sem = (SEM_ID)(mutex->kal_vx_mutex);

    assert(sem);

    semDelete(sem);
}

void
kal_mutex_lock(kal_mutex_t *mutex)
{
    SEM_ID sem = (SEM_ID)(mutex->kal_vx_mutex);

    assert(sem);

    semTake(sem, WAIT_FOREVER);
}

void
kal_mutex_unlock(kal_mutex_t *mutex)
{
    SEM_ID sem = (SEM_ID)(mutex->kal_vx_mutex);

    semGive(sem);
}


