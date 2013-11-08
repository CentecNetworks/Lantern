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

static const char *ll2str[] =
{
    KERN_CRIT    "FATAL %s(%d): %s\n",
    KERN_ERR     "ERROR %s(%d): %s\n",
    KERN_WARNING "WARN  %s(%d): %s\n",
    KERN_INFO    "INFO  %s(%d): %s\n",
    KERN_DEBUG   "DEBUG %s(%d): %s\n",
};

int sal_log(int log_level,
            const char *file,
            int line,
            const char *fmt,
            ...)
{
    char fmtbuf[0x100];
    va_list ap;

    if (log_level < SAL_LL_FATAL)
        log_level = SAL_LL_FATAL;
    else if (log_level > SAL_LL_DEBUG)
        log_level = SAL_LL_DEBUG;

    if (log_level < SAL_LL_INFO)
    {
        snprintf(fmtbuf, 0x100, ll2str[log_level], file, line, fmt);
        fmtbuf[0xFF] = 0;
    }
    else
    {
        strncpy(fmtbuf, fmt, 0x100);
        fmtbuf[0xFF] = 0;
    }

    va_start(ap, fmt);
    vprintk(fmtbuf, ap);
    va_end(ap);

    return 0;
}

EXPORT_SYMBOL(sal_log);
