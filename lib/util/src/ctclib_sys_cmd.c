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

#include <signal.h>
#include "sal.h"
#include "ctclib_sys_cmd.h"

int
ctclib_reconstruct_system_cmd_exec_str(void* sys_cmd)
{
    execl("/bin/sh", "sh", "-c", (char*)sys_cmd, (char *)0);
    _exit(127); /* exec error */
}

int32
ctclib_reconstruct_system_cmd_chld_clone(sal_sys_cmd_clone_fn func,char *cmdstring)
{
    pid_t pid;
    int status;
    struct sigaction ignore,saveintr,savequit;
    sigset_t chldmask,savemask;
    void* stack;

#define MAX_STACK 8192

    if (cmdstring == NULL)
        return(1); /* always a command processor with UNIX */

    stack = sal_malloc(MAX_STACK);
    if(NULL == stack)
    {
        return(-1);
    }

    ignore.sa_handler = SIG_IGN; /* ignore SIGINT and SIGQUIT */
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;
    if (sigaction(SIGINT, &ignore, &saveintr) < 0)
    {
	    sal_free(stack);
        return(-1);
    }

    if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
    {
        sal_free(stack);
        return(-1);
    }

    sigemptyset(&chldmask); /* now block SIGCHLD */
    sigaddset(&chldmask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
    {
	    sal_free(stack);
        return(-1);
    }

    /* child only shares memory with parent */
    /* note: unlike system() command, child process needn't to restore previous signal
        as sharing memory with parent. */
    if ((pid = clone(func,(char*)stack+MAX_STACK,CLONE_VM,(void*)cmdstring)) < 0) {
        status = -1; /* probably out of processes */
    }

    while (waitpid(pid, &status, __WCLONE|__WALL) < 0)
    {
        if (errno != EINTR) {
            status = -1; /* error other than EINTR from waitpid() */
            break;
        }
    }

    /* restore previous signal actions & reset signal mask */
    if (sigaction(SIGINT, &saveintr, NULL) < 0)
    {
        sal_free(stack);
        return(-1);
    }
    if (sigaction(SIGQUIT, &savequit, NULL) < 0)
    {
	    sal_free(stack);
        return(-1);
    }
    if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
    {
        sal_free(stack);
        return(-1);
    }

    sal_free(stack);
    return(status);
}
