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

#ifndef _SYSCMD_READLINE_H_
#define _SYSCMD_READLINE_H_

#include "param_check.h"

#define FULL_NAME_FLASH              "/mnt/flash"
#define FULL_NAME_CFCARD             "/mnt/cf"
#define FULL_NAME_UDISK              "/mnt/udisk"
#define FULL_NAME_EXT_FLASH          "/mnt/data"
#define SHOW_NAME_FLASH              "flash:"
#define MANAGEMENT_IP_STR            "management ip address"

#define M_SYSTEM_LOG_FILE            FULL_NAME_FLASH"/syslog"

#define DHCPSNOOPING_SAVE_BINDING_FILE  FULL_NAME_FLASH"/dhcpsnooping"
#define DHCPSNOOPING_SAVE_BINDING_FILE6  FULL_NAME_FLASH"/dhcpv6snooping"

#define LOGIN_DIRECTORY              FULL_NAME_FLASH
#define STARTUP_CONFIG_FILE_NAME     "startup-config.conf"
#define STARTUP_CONFIG_FILE_PATH     FULL_NAME_FLASH"/"STARTUP_CONFIG_FILE_NAME

#define M_BOOT_IMAGE_PATH               FULL_NAME_FLASH"/boot"
#define M_BOOT_IMAGE_PATH_SHOW               SHOW_NAME_FLASH"/boot"
#define M_CUR_BOOT_IMAGE_LINK_NAME      "uImage"
#define M_CUR_BOOT_IMAGE_LINK_FULLNAME  FULL_NAME_FLASH"/boot/"M_CUR_BOOT_IMAGE_LINK_NAME

#define SYSCMD_MAX_ARG_NUM           32
#define MAX_LINE_CHAR 128

enum network_file_e
{
    NETWORK_TFTP,
    NETWORK_FTP,
    NETWORK_MAX_TYPE
};
typedef enum network_file_e network_file_t;

struct network_dir_s
{
    char *name; /* network method name */
    char *usage; /* format string */
    network_file_t network_file; /* network method type */
    //network_op_t *ops; /* process functions */
};
typedef struct network_dir_s network_dir_t;

typedef enum diskfile
{
    E_DISK_FILE_FLASH = 0,
    E_DISK_FILE_CFCARD = 1,
    E_DISK_FILE_UDISK = 2,
    E_DISK_FILE_EXT_FLASH = 3,
    E_DISK_FILE_MAX
} diskfile_e;

enum syslimit_memchk_state_e
{
    SYSLIMIT_MEMCHECK_STATE_CRITICAL,    /* < threshold0 */
    SYSLIMIT_MEMCHECK_STATE_ALERT,    /* > threshold0 & < threshold1 */
    SYSLIMIT_MEMCHECK_STATE_WARNING,    /* > threshold1 & < threshold2 */
    SYSLIMIT_MEMCHECK_STATE_NORMAL,    /* > threshold2 */
    SYSLIMIT_MEMCHECK_STATE_INVALID
};
typedef enum syslimit_memchk_state_e syslimit_memchk_state_t;

typedef u_int32_t syslimit_flags_t;
#define SYSLIMIT_TYPE_MEM      0x01
#define SYSLIMIT_TYPE_MSG      0x02

struct rootdir_s
{
    diskfile_e etype; /* disk file type */
    char *real_name;
    char *show_name;
};
typedef struct rootdir_s rootdir_t;

/* callback function for iterate root dirs */
typedef void DISK_ITER_FUNC(rootdir_t *rdir, void *arg);

typedef struct system_file_s
{
    char *filename; /* file name */
    char *usage; /* usage string */
} system_file_t;

void gen_iter_disk_cfg(DISK_ITER_FUNC pfunc, void *arg);
int gen_check_and_gen_showname(char *szOrig, char *szShow);
int gen_validate_relative_path(char *pszFullName);
void gen_path_getparents(char *name);
int gen_validate_path(char *pszPath);
int gen_check_and_get_filename(char *filename, char *outfile, size_t outsize);
int syscmd_is_directory(char *filename);
int syscmd_file_exist(char *filename);
int syscmd_file_can_read(char *filename);
int gen_is_bin_file(char *file);
int syscmd_is_file(char *filename);
int syscmd_check_actual_directory_exist(char * filename);
int syscmd_copy_file(char *src_file, char *dst_file);
int memmgr_free_cached_mem();
int copy_file_with_progress(char *src_file, char *dst_file);
int app_show_mem_summary_info(int *total, int *free, int *buf);
int syslimit_mem_threshold_check();
#endif
