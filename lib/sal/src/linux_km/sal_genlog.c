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

/*******************************************************************************
 *
 * Header files
 *
 ******************************************************************************/

#include "sal.h"
#include "genlog.h"
#include "genlog_inner.h"

/*******************************************************************************
 * Name:    klog_format
 * Purpose: format message
 * Input:
 *   eModule: module id
 *   eSeverity: security level
 *   alrm_log: is this a alarm log?
 *   fmt: message content
 * Output:
 * Return:
 * Note:
 ******************************************************************************/
static void
klog_format(E_MODULE eModule, E_SEVERITY eSeverity, char alrm_log, const char *fmt)
{
    char *p;
    int cnt;
    int tbuf_left, prlen = 0;
    int pri = (int) eSeverity;
    const char * log_tag = NULL;

#define	TBUF_LEN	1024
    char tbuf[TBUF_LEN] = { 0 };

    if (!(eModule > M_MOD_UNKNOWN && eModule < M_MOD_MAX &&
          eSeverity >= E_EMERGENCY && eSeverity <= E_DEBUG) || NULL == fmt)
    {
        return;
    }

#define	INTERNALLOG	LOG_ERR|LOG_CONS|LOG_PERROR|LOG_PID
    /* Check for invalid bits. */
    if (pri & ~(LOG_PRIMASK | LOG_FACMASK))
    {
        pri &= LOG_PRIMASK | LOG_FACMASK;
    }
    if (!LOG_MASK(LOG_PRI(pri)))
    {
        return;
    }

    p = tbuf;
    tbuf_left = TBUF_LEN;

#define	DEC()	\
     do \
     { \
         if (prlen < 0)          \
         { \
             prlen = 0;      \
         } \
         if (prlen >= tbuf_left)     \
         { \
             prlen = tbuf_left - 1;  \
         } \
         p += prlen;         \
         tbuf_left -= prlen;    \
     } \
     while (0)

    prlen = snprintf(p, tbuf_left, "<%d>", pri);
    DEC();

    log_tag = (const char *) gModuleNameArr[eModule].pszName;

    if (log_tag != NULL )
    {
        prlen = snprintf(p, tbuf_left, "%s: ", log_tag);
        DEC();
    }

    if (strlen(fmt) > 0)
    {
        prlen = snprintf(p, tbuf_left, fmt);
        DEC();
    }

#if 0
    /* add alarm log prefix */
    if (1 == alrm_log)
    {
        prlen = snprintf(p, tbuf_left, ALRM_LOG_PREFIX);
        DEC();
    }
#endif
    prlen = snprintf(p, tbuf_left, "\n");
    DEC();

    cnt = p - tbuf;
    printk(tbuf);
}

/*******************************************************************************
 * Name:    log_sys
 * Purpose: add one system log
 * Input:
 *   eModule: module id
 *   eSeverity: severity level
 *   fmt: message content
 * Output:
 * Return: N/A
 * Note: log kernel message
 ******************************************************************************/
void
log_sys(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...)
{
    va_list ap;
    char buf[0x100] = {'\0'};

    va_start(ap, fmt);
    vsnprintf(buf, 0xFF, fmt, ap);
    va_end(ap);

    klog_format(eModule, eSeverity, 0, buf);
}

/*******************************************************************************
 * Name:    log_sys
 * Purpose: add one system log
 * Input:
 *   eModule: module id
 *   eSeverity: severity level
 *   fmt: message content
 * Output:
 * Return: N/A
 * Note: log kernel message
 ******************************************************************************/
void
log_alarm(E_MODULE eModule, E_ALRM_LEVEL eSeverity, const char *fmt, ...)
{
    va_list ap;
    E_SEVERITY sev;
    char buf[0x100] = {'\0'};
    sev = (E_SEVERITY)eSeverity;

    va_start(ap, fmt);
    vsnprintf(buf, 0xFF, fmt, ap);
    va_end(ap);

    klog_format(eModule, sev, 1, buf);
}
