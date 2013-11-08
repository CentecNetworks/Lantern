#include "kal.h"
#include <limits.h>
#include <pthread.h>

struct kal_task
{
    pthread_t pth;
    void (*start)(void *);
    void *arg;
};

static void *start_thread(void *arg)
{
    kal_task_t *task = (kal_task_t *)arg;
    (*task->start)(task->arg);

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
    pthread_attr_t attr;
    int ret;

    KAL_MALLOC(task, kal_task_t *, sizeof(kal_task_t));
    if (!task)
        return ENOMEM;
    task->start = start;
    task->arg = arg;

    if ((stack_size != 0) && (stack_size < PTHREAD_STACK_MIN))
        stack_size = PTHREAD_STACK_MIN;

    pthread_attr_init(&attr);
    if (stack_size)
        pthread_attr_setstacksize(&attr, stack_size);

    ret = pthread_create(&task->pth, &attr, start_thread, task);
    if (ret)
    {
        KAL_FREE(task);

        return ret;
    }

    *ptask = task;
    return 0;
}

void kal_task_destroy(kal_task_t *task)
{
    pthread_join(task->pth, NULL);
    KAL_FREE(task);
}

void kal_task_exit()
{
    pthread_exit(0);
}

void kal_task_sleep(uint32_t msec)
{
    struct timespec ts;
    int err;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    while (1) {
        err = nanosleep(&ts, &ts);
        if (err == 0)
            break;

        if (errno != EINTR)
            break;
    }
}

void kal_task_yield()
{
    sched_yield();
}

void kal_gettime(kal_systime_t* tv)
{
    struct timeval temp_tv;

    kal_memset(&temp_tv, 0, sizeof(temp_tv));

    gettimeofday(&temp_tv, NULL);

    tv->tv_sec = temp_tv.tv_sec;
    tv->tv_usec = temp_tv.tv_usec;
}

void kal_getuptime(struct timespec *ts)
{
    int fd;
    unsigned long sec, njiffy;
    char buf[64];

    fd = open("/proc/uptime",O_RDONLY);
    if (fd == -1)
    {
        perror("failed to open /proc/uptime:");
        return ;
    }
    memset(buf,0,64);
    read(fd,buf,64);
    sscanf(buf,"%lu.%lu", &sec, &njiffy);
    ts->tv_sec = sec;
    ts->tv_nsec = njiffy*1000000000/100;
    close(fd);
}

