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

 #include <sys/types.h>

#ifndef  _GENLOG_USR
#define  _GENLOG_USR

#define MAX_IP_LEN 256
#define MAX_USR_NAME_LEN 32
#define MAX_LOG_TIME_LEN 64

/* Used by reentrant functions */
struct genlog_data
{
    int	log_file;
    int	connected;
    int	opened;
    int	log_stat;
    const char *log_tag;
    int log_fac;
    int log_mask;
    char log_time_str[MAX_LOG_TIME_LEN];
    uint16 rate_limit_type;
};

/* Log item format result type */
typedef enum eFmtRtn
{
    FMT_FAIL = -1,
    FMT_SUCCESS = 0,
} E_FMT_RTN;

/* Formatting function pointer type */
typedef E_FMT_RTN(*FMT_FUNC) (char **, size_t *);

/* extra log info type */
typedef struct extra_log_info
{
    char ip_addr[MAX_IP_LEN+1];  /* login ip address */
    char usr_name[MAX_USR_NAME_LEN+1]; /* login username */
    int cmd_level;           /* the operating command level */
    int oprt_rtn; /* operating result */
    FMT_FUNC fmt_func; /* operating message formating function */
} extra_log_info;

#define SYSLOG_DATA_INIT {-1, 0, 0, 0, (const char *)0, LOG_USER, 0xff, {0}, 0}
#define EXTRA_LOG_INFO_INIT {"", "", -1, -1, NULL}
#define	_PATH_LOG	"/dev/log"

#endif   /* ----- #ifndef _GENLOG_USR  ----- */
