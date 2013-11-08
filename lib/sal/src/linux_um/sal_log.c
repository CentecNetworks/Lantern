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
    "FATAL",
    "ERROR",
    "WARN ",
    "INFO ",
    "DEBUG"
};

int sal_log(int log_level,
            const char *file,
            int line,
            const char *fmt,
            ...)
{
    va_list ap;

    if (log_level < SAL_LL_FATAL)
        log_level = SAL_LL_FATAL;
    else if (log_level > SAL_LL_DEBUG)
        log_level = SAL_LL_DEBUG;

    if (log_level < SAL_LL_INFO)
        printf("%s %s:%d: ", ll2str[log_level], file, line);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    if (log_level < SAL_LL_INFO)
        printf("\n");

    return 0;
}
