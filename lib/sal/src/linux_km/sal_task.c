/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "sal.h"
#include <linux/sched.h>

struct sal_task
{
    struct task_struct *ktask;
    const char *name;
    void (*start)(void *);
    void *arg;
    struct completion started;
    struct completion done;
};

static int _sal_thread_start(void *arg)
{
    sal_task_t *task = (sal_task_t *)arg;

    daemonize(task->name);
    complete(&task->started);
    schedule();

    current->security = task;
    (*task->start)(task->arg);
    current->security = NULL;

    complete(&task->done);

    return 0;
}

sal_err_t sal_task_create(sal_task_t **ptask,
                          const char *name,
                          size_t stack_size,
                          void (*start)(void *),
                          void *arg)
{
    sal_task_t *task;
    int pid;

    SAL_MALLOC(task, sal_task_t *, sizeof(sal_task_t));
    if (!task)
        return ENOMEM;
    task->name = name;
    task->start = start;
    task->arg = arg;
    init_completion(&task->started);
    init_completion(&task->done);

    pid = kernel_thread(_sal_thread_start, task, CLONE_KERNEL);
    if (pid < 0)
    {
        SAL_FREE(task);

        return -pid;
    }

    wait_for_completion(&task->started);
    task->ktask = find_task_by_pid(pid);
    *ptask = task;

    return 0;
}

void sal_task_destroy(sal_task_t *task)
{
    wait_for_completion(&task->done);
    SAL_FREE(task);
}

void sal_task_exit()
{
    sal_task_t *task = (sal_task_t *)current->security;

    current->security = NULL;
    complete_and_exit(&task->done, 0);
}

void sal_task_sleep(uint32_t msec)
{
    __set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_timeout(msecs_to_jiffies(msec));
}

void sal_task_yield()
{
    yield();
}

void sal_gettimeofday(struct timeval *tv)
{
    do_gettimeofday(tv);
}

void sal_getuptime(struct timespec *ts)
{
    ktime_get_ts(ts);
}
