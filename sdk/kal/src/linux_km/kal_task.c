#include "kal.h"
#include <linux/sched.h>

struct kal_task
{
    struct task_struct *ktask;
    const char *name;
    void (*start)(void *);
    void *arg;
    struct completion started;
    struct completion done;
};

static int kthread_start(void *arg)
{
    kal_task_t *task = (kal_task_t *)arg;

    daemonize(task->name);
    complete(&task->started);
    schedule();

    current->security = task;
    (*task->start)(task->arg);
    current->security = NULL;

    complete(&task->done);

    return 0;
}

kal_err_t kal_task_create(kal_task_t **ptask,
                          char *name,
                          size_t stack_size,
                          int prio,
                          void (*start)(void *),
                          void *arg)
{
    kal_task_t *task;
    int pid;

    KAL_MALLOC(task, kal_task_t *, sizeof(kal_task_t));
    if (!task)
        return ENOMEM;
    task->name = name;
    task->start = start;
    task->arg = arg;
    init_completion(&task->started);
    init_completion(&task->done);

    pid = kernel_thread(kthread_start, task, CLONE_KERNEL);
    if (pid < 0)
    {
        KAL_FREE(task);

        return -pid;
    }

    wait_for_completion(&task->started);
    task->ktask = find_task_by_pid(pid);
    *ptask = task;

    return 0;
}

void kal_task_destroy(kal_task_t *task)
{
    wait_for_completion(&task->done);
    KAL_FREE(task);
}

void kal_task_exit()
{
    kal_task_t *task = (kal_task_t *)current->security;

    current->security = NULL;
    complete_and_exit(&task->done, 0);
}

void kal_task_sleep(uint32_t msec)
{
    __set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout(msecs_to_jiffies(msec));
}

void kal_task_yield()
{
    yield();
}

void kal_gettime(kal_systime_t* tv)
{
    struct timeval temp_tv;

    kal_memset(&temp_tv, 0, sizeof(temp_tv));

    do_gettimeofday(&temp_tv);

    tv->tv_sec = temp_tv.tv_sec;
    tv->tv_usec = temp_tv.tv_usec;
}

void kal_getuptime(struct timespec *ts)
{
    ktime_get_ts(ts);
}
