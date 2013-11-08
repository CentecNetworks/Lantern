#include "kal.h"
#include <limits.h>


typedef struct vx_task_s{
    char thread_type;
} *vx_task_t;

struct kal_task
{
    vx_task_t pthread;
};

kal_err_t kal_task_create(kal_task_t **ptask,
                          char *name,
                          size_t stack_size,
                          int prio,
                          void (*start)(void *),
                          void *arg)
{
    int rv;
    kal_task_t *task;
    sigset_t new_mask, orig_mask;

    task = (kal_task_t *)mem_malloc(MEM_KAL_MODULE, sizeof(kal_task_t));
    if (!task)
        return ENOMEM;

    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);
    rv = taskSpawn(name, prio, (VX_UNBREAKABLE | VX_FP_TASK), stack_size, (FUNCPTR)start,
		   PTR_TO_INT(arg), 0, 0, 0, 0, 0, 0, 0, 0, 0);
    sigprocmask(SIG_SETMASK, &orig_mask, NULL);

    if (rv == ERROR)
    {
	    return -1;
    }
    else
    {
	    task->pthread = (vx_task_t)INT_TO_PTR(rv);
        *ptask = task;
        return 0;
    }
}

void
kal_task_destroy(kal_task_t *task)
{
    taskDelete(PTR_TO_INT(task->pthread));
    mem_free(task);
}

void kal_task_exit(int rc)
{
    exit(rc);
}


void kal_task_sleep(uint32_t msec)
{
    taskDelay(msec);
}

void kal_task_yield()
{
#if 0
    sched_yield();
#endif
}

void kal_gettime(kal_systime_t* tv)
{
    struct timespec temp_tv;

    kal_memset(&temp_tv, 0, sizeof(temp_tv));

    clock_gettime(CLOCK_REALTIME, &temp_tv);

    tv->tv_sec = temp_tv.tv_sec;
    tv->tv_usec = temp_tv.tv_nsec / 1000;
}

void kal_getuptime(struct timespec *ts)
{
}

