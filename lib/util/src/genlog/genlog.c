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

/****************************************************************************
 *
 * Head files
 *
 ****************************************************************************/
 #include "sal.h"
#include <signal.h>
#include <sys/socket.h>
#include <assert.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#include "genlog.h"
#include "genlog_inner.h"
#include "genlog_usr.h"

/***************************************************************************
 *
 * Globals and declarations
 *
 ***************************************************************************/
static struct genlog_data s_log_data = SYSLOG_DATA_INIT; /* common log envirionment */
static struct extra_log_info s_extra_info = EXTRA_LOG_INFO_INIT; /* extra info, eg, the user name and ip */
static int s_log_slot_no = 0;

static void connectlog_r(struct genlog_data *);  /* (re)connect to syslogd */
#if 0
static void disconnectlog_r(struct genlog_data *); /* disconnect from syslogd */
#endif
static void openlog_r(const char *ident, int logstat, int logfac, struct genlog_data *data);
static void closelog_r(struct genlog_data *data);
static void vsyslog_r(int pri, struct genlog_data *data, const char *fmt, va_list ap);

/*******************************************************************************
 * Name:    log_get_rate_limit_arr
 * Purpose: return glb log data
 * Input: None
 * Output:
 * Return: glb_log_rate_limit_arr
 ******************************************************************************/
log_message_rate_limit_t *
log_get_rate_limit_arr(void)
{
    return g_log_rate_limit_arr;
}

/*******************************************************************************
 * Name:    log_strlcpy
 * Purpose: security string copy
 * Input:
 *   dst: destination
 *   src: source
 *   size: length
 * Output:
 * Return:
 *   success: the length of src
 *   failed : the length of src
 * Note: used for string copy
 ******************************************************************************/
static size_t
log_strlcpy(char *dst, const char *src, size_t size)
{
    if (size)
    {
        strncpy(dst, src, size - 1);
        dst[size - 1] = '\0';
    }
    else
    {
        dst[0] = '\0';
    }
    return strlen(src);
}


/*******************************************************************************
 * Name:    format_oprt_log
 * Purpose: formatting operating log
 * Input:
 *   pszOutput: result
 *   pLen: result length
 * Output: pszOutput
 * Return:
 *   success: FMT_SUCCESS
 *   failed : FMT_FAIL
 * Note:
 ******************************************************************************/
static E_FMT_RTN
format_oprt_log(char **pszOutput,  size_t * pLen)
{
    assert(pszOutput && pLen);
    if (NULL == s_extra_info.ip_addr)
    {
        return FMT_FAIL;
    }
#define LEFT_LEN 100
    *pLen = strlen(s_extra_info.ip_addr) + strlen(s_extra_info.usr_name)  + LEFT_LEN;
    *pszOutput = (char *) malloc(*pLen + 1);

    if (!(*pszOutput))
    {
        return FMT_FAIL;
    }

    snprintf(*pszOutput, *pLen, "user=%s;ip=%s;cmdlevel=%d;opresult=%d;",
        s_extra_info.usr_name, s_extra_info.ip_addr, s_extra_info.cmd_level, s_extra_info.oprt_rtn);

    return FMT_SUCCESS;
}

/*******************************************************************************
 * Name:    format_alrm_log
 * Purpose: formatting device alarm log
 * Input:
 *   pszOutput: result
 *   pLen: result length
 * Output: pszOutput
 * Return:
 *   success: FMT_SUCCESS
 *   failed : FMT_FAIL
 * Note:
 ******************************************************************************/
static E_FMT_RTN
format_alrm_log(char **pszOutput,  size_t * pLen)
{
    if (NULL == s_extra_info.ip_addr)
    {
        return FMT_FAIL;
    }
#define ALRM_PREFIX_LEN 10
    *pLen = ALRM_PREFIX_LEN;
    *pszOutput = (char *) malloc(*pLen + 1);
    memset(*pszOutput, 0, *pLen + 1);

    if (!(*pszOutput))
    {
        return FMT_FAIL;
    }

    snprintf(*pszOutput, *pLen, ALRM_LOG_PREFIX);

    return FMT_SUCCESS;
}


/*******************************************************************************
 * Name:    genlog_open
 * Purpose: open genlog log
 * Input: N/A
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
genlog_open(void)
{
    openlog_r("", LOG_CONS | LOG_NDELAY, LOG_USER, &s_log_data);
}

/*******************************************************************************
 * Name:    genlog_close
 * Purpose: close genlog
 * Input: N/A
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
genlog_close(void)
{
    closelog_r(&s_log_data);
}

/*******************************************************************************
 * Name:    log_init_line
 * Purpose: initialize access information
 * Input:
 *   ip_addr: the client ip address
 *   usr_name: the access user name
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
log_init_line(const char * ip_addr, const char * usr_name)
{
    assert(NULL != ip_addr && NULL != usr_name);
    log_strlcpy(s_extra_info.ip_addr, ip_addr, MAX_IP_LEN+1);
    log_strlcpy(s_extra_info.usr_name, usr_name, MAX_USR_NAME_LEN+1);
}


/*******************************************************************************
 * Name:    log_sys
 * Purpose: add one system log
 * Input:
 *   eModule: module id
 *   eSeverity: log severity level
 *   fmt: the log content
 * Output:
 * Return: N/A
 * Note: log length should be limited in 1024 bytes
 ******************************************************************************/
void
log_sys(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...)
{
    va_list ap;

    assert(eModule>M_MOD_UNKNOWN && eModule<M_MOD_MAX &&  eSeverity>=E_EMERGENCY && eSeverity <=E_DEBUG);

    s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
    if (!s_log_data.log_tag)
    {
        return;
    }

    s_log_data.rate_limit_type = LOG_RATE_LIMIT_TYPE_DEF;
    s_extra_info.fmt_func = NULL;
    va_start(ap, fmt);
    vsyslog_r(eSeverity, &s_log_data, fmt, ap);
    va_end(ap);
}

/*******************************************************************************
 * Name:    log_sys_rate_limit
 * Purpose: add one system log while may drop by ratelimit
 * Input:
 *   eModule: module id
 *   eSeverity: log severity level
 *   uint16: rate limit type
 *   fmt: the log content
 * Output:
 * Return: N/A
 * Note: log length should be limited in 1024 bytes
 ******************************************************************************/
void
log_sys_rate_limit(E_MODULE eModule, E_SEVERITY eSeverity, uint16 rateLimitType, const char *fmt, ...)
{
    va_list ap;

    assert(eModule > M_MOD_UNKNOWN && eModule < M_MOD_MAX
        && eSeverity >= E_EMERGENCY && eSeverity <= E_DEBUG);

    s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
    if (!s_log_data.log_tag)
    {
        return;
    }
    s_log_data.rate_limit_type = rateLimitType;
    s_extra_info.fmt_func = NULL;
    va_start(ap, fmt);
    vsyslog_r(eSeverity, &s_log_data, fmt, ap);
    va_end(ap);
}

/*******************************************************************************
 * Name:    copy_meta_str
 * Purpose: copy string with meta charachter disable
 * Input:
 *   src: the source string
 * Output:
 * Return:
 *   dest string, 0 for NULL
 * Note: the client response to free the dest memory
 ******************************************************************************/
static char *
copy_meta_str(const char * src)
{
    int length = 0;
    char *dest = NULL;
    const char *p1 = src;
    char *p2 = NULL;

    if (!src)
    {
        return NULL;
    }

    length = strlen(src);
    p2 = dest = (char *)malloc(2*length + 1);
    if (!p2)
    {
        return NULL;
    }
    memset(dest, 0, 2*length + 1);

    while (*p1)
    {
        if ('%' == *p1)
        {
            *p2++ = *p1;
        }
        *p2++ = *p1++;
    }
    dest[2*length] = '\0';
    return dest;
}

/*******************************************************************************
 * Name:    log_oper
 * Purpose: add on operating log
 * Input:
 *   eModule: module id
 *   eSeverity: log severity level
 *   cl: command level
 *   rlt: operating result
 *   fmt: the log content
 * Output:
 * Return: N/A
 * Note: log length should be limited in 1024 bytes
 ******************************************************************************/
void
log_oper(E_MODULE eModule, E_SEVERITY eSeverity, CMD_LEVEL cl, E_OPRT_RLT rlt,
         const char *fmt, ...)
{
    va_list ap;
    int old_facility;
    char *real_oper = NULL;

    assert(eModule>M_MOD_UNKNOWN && eModule<M_MOD_MAX &&
            eSeverity>=E_EMERGENCY && eSeverity <=E_DEBUG);

    s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
    if (!s_log_data.log_tag)
    {
        return;
    }

    old_facility = s_log_data.log_fac;
    s_log_data.log_fac = LOG_LOCAL5;
    s_log_data.rate_limit_type = LOG_RATE_LIMIT_TYPE_DEF;

    s_extra_info.fmt_func = &format_oprt_log;
    s_extra_info.oprt_rtn = rlt;
    s_extra_info.cmd_level = cl;

    real_oper = copy_meta_str(fmt);
    if (!real_oper)
    {
        return;
    }

    va_start(ap, fmt);
    vsyslog_r(eSeverity, &s_log_data, real_oper, ap);
    va_end(ap);

    free(real_oper);
    real_oper = NULL;
    s_log_data.log_fac = old_facility;
}

/*******************************************************************************
 * Name:    log_diag
 * Purpose: add an diag log
 * Input:
 *   eModule: module id
 *   eSeverity: log severity level
 *   cl: command level
 *   rlt: operating result
 *   fmt: the log content
 * Output:
 * Return: N/A
 * Note: log length should be limited in 1024 bytes
 ******************************************************************************/
void
log_diag(E_MODULE eModule, E_SEVERITY eSeverity, const char *fmt, ...)
{
    va_list ap;
    E_SEVERITY sev;
    int old_facility;

    assert(eModule > M_MOD_UNKNOWN && eModule < M_MOD_MAX &&
    eSeverity >= E_EMERGENCY && eSeverity <= E_DEBUG);

    old_facility = s_log_data.log_fac;
    s_log_data.log_fac = LOG_LOCAL7;
     s_log_data.rate_limit_type = LOG_RATE_LIMIT_TYPE_DEF;

    sev = (E_SEVERITY)eSeverity;
    s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
    if (!s_log_data.log_tag)
    {
        return;
    }
    s_extra_info.fmt_func = NULL;

    va_start(ap, fmt);
    vsyslog_r(sev, &s_log_data, fmt, ap);
    va_end(ap);

    s_log_data.log_fac = old_facility;
}

/*******************************************************************************
 * Name:    log_diag_rate_limit
 * Purpose: add one system log while may drop by ratelimit
 * Input:
 *   eModule: module id
 *   eSeverity: log severity level
 *   uint16: rate limit type
 *   fmt: the log content
 * Output:
 * Return: N/A
 * Note: log length should be limited in 1024 bytes
 ******************************************************************************/
void
log_diag_rate_limit(E_MODULE eModule, E_SEVERITY eSeverity, uint16 rateLimitType, const char *fmt, ...)
{
     va_list ap;
     E_SEVERITY sev;
     int old_facility;

     assert(eModule > M_MOD_UNKNOWN && eModule < M_MOD_MAX &&
     eSeverity >= E_EMERGENCY && eSeverity <= E_DEBUG);

     old_facility = s_log_data.log_fac;
     s_log_data.log_fac = LOG_LOCAL7;

     sev = (E_SEVERITY)eSeverity;
     s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
     if (!s_log_data.log_tag)
     {
         return;
     }
     s_log_data.rate_limit_type = rateLimitType;
     s_extra_info.fmt_func = NULL;

     va_start(ap, fmt);
     vsyslog_r(sev, &s_log_data, fmt, ap);
     va_end(ap);

     s_log_data.log_fac = old_facility;
}

/*******************************************************************************
 * Name:    log_alarm
 * Purpose: add one device alarm log
 * Input:
 *   eModule: module id
 *   eSeverity: alarm level
 *   fmt: log content
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
log_alarm(E_MODULE eModule, E_ALRM_LEVEL eSeverity, const char *fmt, ...)
{
    va_list ap;
    E_SEVERITY sev;
    int old_facility;

    assert(eModule>M_MOD_UNKNOWN && eModule<M_MOD_MAX &&
            eSeverity>=HIGH && eSeverity <=MINOR);

    old_facility = s_log_data.log_fac;
    s_log_data.log_fac = LOG_LOCAL6;
    s_log_data.rate_limit_type = LOG_RATE_LIMIT_TYPE_DEF;

    sev = (E_SEVERITY)eSeverity;
    s_log_data.log_tag = (const char *) gModuleNameArr[eModule].pszName;
    if (!s_log_data.log_tag)
    {
        return;
    }
    s_extra_info.fmt_func = NULL;

    va_start(ap, fmt);
    vsyslog_r(sev, &s_log_data, fmt, ap);
    va_end(ap);

    s_log_data.log_fac = old_facility;
}


/*******************************************************************************
 * Name:    vsyslog_r
 * Purpose: generating one log
 * Input:
 *   pri: priority
 *   data: the global log environment
 *   fmt: the message
 *   ap: first variable parameter
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
static void
vsyslog_r(int pri, struct genlog_data *data, const char *fmt, va_list ap)
{
    int cnt;
    char ch, *p, *t;
    time_t now;
    int saved_errno, error;

#define	TBUF_LEN	2048
#define	FMT_LEN		1024
    char *stdp = NULL;
    char tbuf[TBUF_LEN];
    char fmt_cpy[FMT_LEN];
    int tbuf_left, fmt_left, prlen;
    struct timeval tz;
    static int sec = 0;

    signal(SIGPIPE, SIG_IGN);

    /* Check for invalid bits. */
    if (pri & ~(LOG_PRIMASK | LOG_FACMASK))
    {
        pri &= LOG_PRIMASK | LOG_FACMASK;
    }

    /* Check priority against setlogmask values. */
    if (!(LOG_MASK(LOG_PRI(pri)) & data->log_mask))
    {
        return;
    }

    saved_errno = errno;

    /* Set default facility if none specified. */
    if ((pri & LOG_FACMASK) == 0)
    {
        pri |= data->log_fac;
    }

    /* If we have been called through syslog(), no need for reentrancy. */
    ctclib_time_boottime(&tz);
    if (tz.tv_sec != sec)
    {
        now = tz.tv_sec;
        sec = tz.tv_sec;
        strftime(s_log_data.log_time_str, MAX_LOG_TIME_LEN, "%h %e %T ", localtime(&now));
    }

    if (s_log_data.rate_limit_type)
    {
        /*outof bound*/
        if (s_log_data.rate_limit_type >= LOG_RATE_LIMIT_TYPE_MAX)
        {
            return;
        }
        if (g_log_rate_limit_arr[s_log_data.rate_limit_type].next_sec > sec)
        {
            g_log_rate_limit_arr[s_log_data.rate_limit_type].suppress_cnt++;
            return;
        }
        g_log_rate_limit_arr[s_log_data.rate_limit_type].next_sec =
                                        sec + g_log_rate_limit_arr[s_log_data.rate_limit_type].log_interval;
    }

    p = tbuf;
    tbuf_left = TBUF_LEN;

#define DEC()                         \
    do                                \
    {                                 \
        if (prlen < 0)                \
        {                             \
            prlen = 0;                \
        }                             \
        if (prlen >= tbuf_left)       \
        {                             \
            prlen = tbuf_left - 1;    \
        }                             \
        p += prlen;                   \
        tbuf_left -= prlen;           \
    }                                 \
    while (0);

    prlen = snprintf(p, tbuf_left, "<%d>", pri);
    DEC();

    /*
     * syslog-ng-d will expand time automagically for reentrant case, and
     * for normal case, just do like before
     */
    prlen = snprintf(p, tbuf_left, "%s", s_log_data.log_time_str);
    DEC();

    if (data->log_stat & LOG_PERROR)
    {
        stdp = p;
    }
    if (data->log_tag == NULL)
    {
        data->log_tag = "genlog";
    }
    if (data->log_tag != NULL)
    {
        prlen = snprintf(p, tbuf_left, "%s", data->log_tag);
        DEC();
    }
    if (data->log_stat & LOG_PID)
    {
        prlen = snprintf(p, tbuf_left, "[%ld]", (long) getpid());
        DEC();
    }
    if (data->log_tag != NULL)
    {
        if (tbuf_left > 1)
        {
            *p++ = ':';
            tbuf_left--;
        }
        if (tbuf_left > 1)
        {
            *p++ = ' ';
            tbuf_left--;
        }
    }

    if (s_log_slot_no)
    {
        prlen = snprintf(p, tbuf_left, "LC%d ", s_log_slot_no);
        DEC();
    }

    /* strerror() is not reentrant */
    for (t = fmt_cpy, fmt_left = FMT_LEN; (ch = *fmt); ++fmt)
    {
        if (ch == '%' && fmt[1] == 'm')
        {
            ++fmt;
            if (data == &s_log_data)
            {
                prlen = snprintf(t, fmt_left, "%s", strerror(saved_errno));
            }
            else
            {
                prlen = snprintf(t, fmt_left, "Error %d", saved_errno);
            }
            if (prlen < 0)
            {
                prlen = 0;
            }
            if (prlen >= fmt_left)
            {
                prlen = fmt_left - 1;
            }
            t += prlen;
            fmt_left -= prlen;
        }
        else if (ch == '%' && fmt[1] == '%' && fmt_left > 2)
        {
            *t++ = '%';
            *t++ = '%';
            fmt++;
            fmt_left -= 2;
        }
        else
        {
            if (fmt_left > 1)
            {
                *t++ = ch;
                fmt_left--;
            }
        }
    }
    *t = '\0';

    if (format_oprt_log == s_extra_info.fmt_func)
    {
        E_FMT_RTN fmt_rtn = FMT_FAIL;
        char *pszOutput = NULL;
        size_t len = 0;

        /* Format extra log info if required */
        fmt_rtn = s_extra_info.fmt_func(&pszOutput, &len);
        prlen = snprintf(p, tbuf_left, pszOutput);
        DEC();
        if (FMT_FAIL != fmt_rtn)
        {
            free(pszOutput);
            pszOutput = NULL;
        }
    }

    if (format_alrm_log == s_extra_info.fmt_func)
    {
        E_FMT_RTN fmt_rtn = FMT_FAIL;
        char *pszOutput = NULL;
        size_t len = 0;

        prlen = vsnprintf(p, tbuf_left, fmt_cpy, ap);
        DEC();

        /* Format extra log info if required */
        fmt_rtn = s_extra_info.fmt_func(&pszOutput, &len);
        prlen = snprintf(p, tbuf_left, pszOutput);
        DEC();
        if (FMT_FAIL != fmt_rtn)
        {
            free(pszOutput);
            pszOutput = NULL;
        }
    }

    else if (format_oprt_log == s_extra_info.fmt_func)
    {
        prlen = snprintf(p, tbuf_left, fmt_cpy);
        DEC();
    }

    else
    {
        prlen = vsnprintf(p, tbuf_left, fmt_cpy, ap);
        DEC();
    }

    prlen = snprintf(p, tbuf_left, "\n");
    DEC();

    cnt = p - tbuf;

    /* Output to stderr if required */
    if (data->log_stat & LOG_PERROR)
    {
        struct iovec iov[2];

        iov[0].iov_base = stdp;
        iov[0].iov_len = cnt - (stdp - tbuf);
        iov[1].iov_base = "\n";
        iov[1].iov_len = 1;
        (void) writev(STDERR_FILENO, iov, 2);
    }

    /* Get connected, output the message to the local logger. */
    if (!data->opened)
    {
        openlog_r(data->log_tag, data->log_stat, 0, data);
    }
    connectlog_r(data);

    errno = 0;
    if ((error = send(data->log_file, tbuf, cnt, 0)) < 0)
    {
        if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS))
        {
            /* syslog-ng maybe restart, so we reopen the connection,
               then send this message again */
            if (errno != ENOBUFS)
            {
                closelog_r(data);
                openlog_r("", LOG_CONS | LOG_NDELAY, LOG_USER, &s_log_data);
                error = send(data->log_file, tbuf, cnt, 0);
            }
        }
    }

    if (data != &s_log_data)
    {
        closelog_r(data);
    }
}

void
sock_set_nonblocking(int fd, int on)
{
    ioctl (fd, FIONBIO, (void *) &on);
}

/*******************************************************************************
 * Name:    connectlog_r
 * Purpose: connecting to syslog-ng daemon
 * Input:
 *   data: the log environment
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
static void
connectlog_r(struct genlog_data *data)
{
    struct sockaddr_un syslog_addr;

    if (-1 == data->log_file)
    {
        if (-1 == (data->log_file = socket(AF_UNIX, SOCK_DGRAM, 0)))
        {
            return;
        }
        (void) fcntl(data->log_file, F_SETFD, 1);
        /*set mode to block*/
        sock_set_nonblocking(data->log_file, 0);
    }

    if (-1 != data->log_file && !data->connected)
    {
        memset(&syslog_addr, 0, sizeof(syslog_addr));
        syslog_addr.sun_family = AF_UNIX;
        log_strlcpy(syslog_addr.sun_path, _PATH_LOG,
                    sizeof(syslog_addr.sun_path));

        errno = 0;
        if (connect(data->log_file, (struct sockaddr *) &syslog_addr,
                    sizeof(struct sockaddr)) < 0)
        {
            if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS))
            {
                (void) close(data->log_file);
                data->log_file = -1;
            }
            else
            {
                data->connected = 1;
            }
        }
        else
        {
            data->connected = 1;
        }
    }

}

/*******************************************************************************
 * Name:    disconnectlog_r
 * Purpose: disconnecting from syslog-ng daemon
 * Input:
 *   data: the log environment
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
#if 0
static void
disconnectlog_r(struct genlog_data *data)
{
    /*
     * If the user closed the FD and opened another in the same slot,
     * that's their problem.  They should close it before calling on
     * system services.
     */
    if (data->log_file != -1)
    {
        close(data->log_file);
        data->log_file = -1;
    }
    data->connected = 0;        /* retry connect */
}
#endif

/*******************************************************************************
 * Name:    openlog_r
 * Purpose: open log
 * Input:
 *   ident: the pragram name
 *   logstat: log option
 *   logfac: facility
 *   data: the log environment
 * Output:
 * Return: N/A
 * Note: reentrant
 ******************************************************************************/
static void
openlog_r(const char *ident, int logstat, int logfac, struct genlog_data *data)
{
    if (ident != NULL)
    {
        data->log_tag = ident;
    }
    data->log_stat = logstat;
    if (logfac != 0 && (logfac & ~LOG_FACMASK) == 0)
    {
        data->log_fac = logfac;
    }

    if (data->log_stat & LOG_NDELAY) /* open immediately */
    {
        connectlog_r(data);
    }
    data->opened = 1;           /* ident and facility has been set */
}

/*******************************************************************************
 * Name:    closelog_r
 * Purpose: clear log environment
 * Input:
 *   data: the log environment
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
static void
closelog_r(struct genlog_data *data)
{
    if (data->log_file != -1)
    {
        (void) close(data->log_file);
    }
    data->log_file = -1;
    data->connected = 0;
    data->log_tag = NULL;
}

/*******************************************************************************
 * Name:    genlog_set_slot_no
 * Purpose: set genlog add slot no
 * Input:
 *   phy_slot_no: physical slot NO. if 0, not add "LC*" in log message
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
genlog_set_slot_no(int phy_slot_no)
{
    s_log_slot_no = phy_slot_no;
}
