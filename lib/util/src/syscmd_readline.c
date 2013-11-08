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
* Header Files
*
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "syscmd_readline.h"
#include "param_check.h"
#include "sal.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define ctc_cli_out printf

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
rootdir_t actual_root_dir[] = {
    {E_DISK_FILE_FLASH,      FULL_NAME_FLASH,       "flash:"},
    {E_DISK_FILE_UDISK,      FULL_NAME_UDISK,       "udisk:"},
    {E_DISK_FILE_EXT_FLASH,  FULL_NAME_EXT_FLASH,   "ext-flash:"}
};

network_dir_t network_root_dir[] = {
    {
        "tftp://",
        "tftp://hostname_or_ip:port/file_name",
        NETWORK_TFTP,
    },
    {
        "ftp://",
        "ftp://username:password@host:port/file_name",
        NETWORK_FTP,
    }
};

/****************************************************************************
 *
* Functions
*
*****************************************************************************/

/*******************************************************************************
 * Name:    gen_iter_disk_cfg
 * Purpose: iterate disk cfg array and call pfunc to process it
 * Input:
 *   pfunc: callback function
 *   arg: callback argument
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void
gen_iter_disk_cfg(DISK_ITER_FUNC pfunc, void *arg)
{
    int nCnt = sizeof(actual_root_dir) / sizeof(actual_root_dir[0]);
    rootdir_t *rdir = NULL;
    int nIdx = 0;

    if (NULL == pfunc)
    {
        return;
    }

    for (nIdx = 0; nIdx < nCnt; nIdx++)
    {
        rdir = &actual_root_dir[nIdx];
        pfunc(rdir, arg);
    }
    return;
}

/*******************************************************************************
 * Name:    gen_check_and_gen_showname
 * Purpose: check original path and build showing string
 * Input:
 *   szOrig: absolute path
 *   szShow: showing path
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
gen_check_and_gen_showname(char *szOrig, char *szShow)
{
    int nCnt = 0;
    int nIdx = 0;
    rootdir_t *rdir = NULL;

    if (NULL == szOrig || NULL == szShow)
    {
        return -1;
    }

    nCnt = sizeof(actual_root_dir) / sizeof(actual_root_dir[0]);
    for (nIdx = 0; nIdx < nCnt; nIdx++)
    {
        rdir = &actual_root_dir[nIdx];
        if (!sal_strncmp(szOrig, rdir->real_name,
                         sal_strlen(rdir->real_name)))
        {
            if (!sal_strcmp(szOrig, rdir->real_name))
            {
                sal_snprintf(szShow, M_FULLPATH_MAX_LEN,
                        "%s/", rdir->show_name);
            }
            else if ('/' == szOrig[sal_strlen(rdir->real_name)])
            {
                sal_snprintf(szShow, M_FULLPATH_MAX_LEN, "%s%s",
                        rdir->show_name,
                        szOrig + sal_strlen(rdir->real_name));
            }
            else
            {
                sal_snprintf(szShow, M_FULLPATH_MAX_LEN, "%s/%s",
                        rdir->show_name,
                        szOrig + sal_strlen(rdir->real_name));
            }
            return 0;
        }
    }

    return -1;

}

/*******************************************************************************
 * Name:    gen_validate_path
 * Purpose: validate full path
 * Input:
 *   pszPath: path name
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
gen_validate_path(char *pszPath)
{
    char **ppNameArr = NULL;
    unsigned long nCnt = 0;
    int i = 0;
    int nRet = 0;

    if (NULL == pszPath)
    {
        return -1;
    }

    if (sal_strlen(pszPath) > M_FULLPATH_MAX_LEN)
    {
        return -1;
    }

    if (NULL != sal_strstr(pszPath, "//"))
    {
        return -1;
    }

    if (split_tokens(pszPath, sal_strlen(pszPath), M_FULLPATH_DEPTH_MAX,
              "/", &nCnt, &ppNameArr) != 0)
    {
        return -1;
    }

    for (i = 0; i < nCnt; i++)
    {
        if (NULL == ppNameArr[i])
        {
            continue;
        }
        if (check_filename(ppNameArr[i]) != 0)
        {
            nRet = -1;
            break;
        }
    }
    free_tokens(&ppNameArr);
    return nRet;
}

/*******************************************************************************
 * Name:    gen_validate_relative_path
 * Purpose: validate relative path
 * Input:
 *   pszFullName: path name
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note: Created by Percy Wang 2007-12-21
 ******************************************************************************/
int
gen_validate_relative_path(char *pszFullName)
{
    int nCnt = 0;
    int nRet = 0;
    int nIdx = 0;
    rootdir_t *rdir = NULL;
    char *pszTmpName;
    int nValLen = 0;

    if (NULL == pszFullName)
    {
        return -1;
    }

    if (sal_strlen(pszFullName) > M_FULLPATH_MAX_LEN)
    {
        return -1;
    }

    pszTmpName = pszFullName;

    if (sal_strchr (pszFullName, ':') != NULL)
    {
        nCnt = sizeof(actual_root_dir) / sizeof(actual_root_dir[0]);
        for (nIdx = 0; nIdx < nCnt; nIdx++)
        {
            rdir = &actual_root_dir[nIdx];
            nValLen = sal_strlen(rdir->show_name);
            if (!sal_strncmp(pszFullName, rdir->show_name,
                             sal_strlen(rdir->show_name)))
            {
                if (!sal_strcmp(pszFullName, rdir->show_name))
                {
                    return 0;
                }
                else
                {
                    pszTmpName = pszFullName + nValLen;
                    break;
                }

            }
        }

    }

    nRet = gen_validate_path(pszTmpName);

    return nRet;
}

/*******************************************************************************
 * Name:    gen_path_getparents
 * Purpose: parse .. so we don't compromise security
 * Input:
 *   name: path with .. and .
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
void gen_path_getparents(char *name)
{
    int l, w;

    /* Four passes, as per RFC 1808 */
    /* a) remove ./ path segments */
    for (l = 0, w = 0; name[l] != '\0';)
    {
        if (name[l] == '.' && name[l + 1] == '/' && (l == 0 || name[l - 1] == '/'))
        {
            l += 2;
        }
        else
        {
            name[w++] = name[l++];
        }
    }

    /* b) remove trailing . path, segment */
    if (w == 1 && name[0] == '.')
    {
        w--;
    }
    else if (w > 1 && name[w - 1] == '.' && name[w - 2] == '/')
    {
        w--;
    }
    name[w] = '\0';

    /* c) remove all xx/../ segments. (including leading ../ and /../) */
    l = 0;

    while (name[l] != '\0')
    {
        if (name[l] == '.' && name[l + 1] == '.' && name[l + 2] == '/' &&
            (l == 0 || name[l - 1] == '/'))
        {
            register int m = l + 3, n;

            l = l - 2;
            if (l >= 0)
            {
                while (l >= 0 && name[l] != '/')
                {
                    l--;
                }
                l++;
            }
            else
            {
                l = 0;
            }
            n = l;
            while ((name[n] = name[m]))
            {
                (++n, ++m);
            }
        }
        else
        {
            ++l;
        }
    }

    /* d) remove trailing xx/.. segment. */
    if (l == 2 && name[0] == '.' && name[1] == '.')
    {
        name[0] = '\0';
    }
    else if (l > 2 && name[l - 1] == '.' && name[l - 2] == '.' && name[l - 3] == '/')
    {
        l = l - 4;
        if (l >= 0)
        {
            while (l >= 0 && name[l] != '/')
            {
                l--;
            }
            l++;
        }
        else
        {
            l = 0;
        }
        name[l] = '\0';
    }
    return;
}

/*******************************************************************************
 * Name:    gen_check_and_get_filename
 * Purpose: check and return absolute filename
 * Input:
 *   filename: file name start with drive
 *   outsize: out buffer size
 * Output:
 *   outfile: output file name
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
gen_check_and_get_filename(char *filename, char *outfile, size_t outsize)
{
    char szFullName[M_FULLPATH_MAX_LEN];
    rootdir_t *rdir = NULL;
    int nValLen = 0;
    int nLen = 0;
    int nCnt = 0;
    int nIdx = 0;

    if (NULL == filename || NULL == outfile || 0 > outsize)
    {
        return -1;
    }

    if (sal_strlen(filename) >= M_FULLPATH_MAX_LEN)
    {
        ctc_cli_out("%% File or directory name length overflow.\n");
        return -1;
    }

    sal_snprintf(szFullName, M_FULLPATH_MAX_LEN, filename);

    if (gen_validate_relative_path(szFullName) != 0)
    {
        return -1;
    }

    gen_path_getparents(szFullName);
    nLen = sal_strlen(szFullName);

    nCnt = sizeof(actual_root_dir) / sizeof(actual_root_dir[0]);
    for (nIdx = 0; nIdx < nCnt; nIdx++)
    {
        rdir = &actual_root_dir[nIdx];
        nValLen = sal_strlen(rdir->show_name);
        if (!sal_strncmp(szFullName, rdir->show_name,
                         sal_strlen(rdir->show_name)))
        {
            if (!sal_strcmp(szFullName, rdir->show_name))
            {
                sal_snprintf(outfile, outsize, "%s",
                        rdir->real_name);
            }
            else if ('/' == szFullName[nValLen])
            {
                sal_snprintf(outfile, outsize, "%s%s",
                        rdir->real_name, szFullName + nValLen);
                if ((sal_strlen(rdir->real_name) + sal_strlen(szFullName + nValLen))
                        >= M_FULLPATH_MAX_LEN)
                {
                    ctc_cli_out("%% File or directory name length overflow.\n");
                    return -1;
                }
            }
            else
            {
                sal_snprintf(outfile, outsize, "%s/%s",
                        rdir->real_name, szFullName + nValLen);
                if ((sal_strlen(rdir->real_name) + sal_strlen(szFullName + nValLen) + 1)
                        >= M_FULLPATH_MAX_LEN)
                {
                    ctc_cli_out("%% File or directory name length overflow.\n");
                    return -1;
                }
            }
            if (gen_validate_path(outfile) != 0)
            {
                return -1;
            }
            return 0;
        }
    }

    /* is it .. or . or raw path
     * let connect it with current working directory and check it
     */
    szFullName[0] = '\0';
    szFullName[M_FULLPATH_MAX_LEN - 1] = '\0';
    if (getcwd(szFullName, M_FULLPATH_MAX_LEN) == NULL)
    {
        ctc_cli_out("%% Get current working directory failed: %s\n",
                    sal_strerror(errno));
        return -1;
    }

    nLen = sal_strlen(szFullName);
    if ('/' != filename[0])
    {
        sal_snprintf(szFullName + nLen, M_FULLPATH_MAX_LEN - nLen, "/%s", filename);
        if ((nLen + sal_strlen(filename) + 1) >= M_FULLPATH_MAX_LEN)
        {
            ctc_cli_out("%% File or directory name length overflow.\n");
            return -1;
        }
    }
    else
    {
        sal_snprintf(szFullName + nLen, M_FULLPATH_MAX_LEN - nLen, "%s", filename);
        if ((nLen + sal_strlen(filename)) >= M_FULLPATH_MAX_LEN)
        {
            ctc_cli_out("%% File or directory name length overflow.\n");
            return -1;
        }
    }
    gen_path_getparents(szFullName);
    nLen = sal_strlen(szFullName);
    nCnt = sizeof(actual_root_dir) / sizeof(actual_root_dir[0]);
    for (nIdx = 0; nIdx < nCnt; nIdx++)
    {
        rdir = &actual_root_dir[nIdx];
        nValLen = sal_strlen(rdir->real_name);
        if (!sal_strncmp(szFullName, rdir->real_name,
                         sal_strlen(rdir->real_name)))
        {
            sal_snprintf(outfile, outsize, szFullName);

            if (gen_validate_path(outfile) != 0)
            {
                return -1;
            }
            return 0;
        }
        if (!sal_strncmp(szFullName, rdir->real_name, nLen))
        {
            /* this directory not showing for user
             * setting it to root directory of this part
             */
            return -1;
        }
    }

    return -1;
}

int
syscmd_is_directory(char *filename)
{
    struct stat stat_buf;

    if (!stat(filename, &stat_buf))
    {
        if (S_ISDIR(stat_buf.st_mode))
        {
            return 1;
        }
    }
    return 0;
}

int
syscmd_file_exist(char *filename)
{
    struct stat stat_buf;

    if (!stat(filename, &stat_buf))
    {
        return 1;
    }
    return 0;
}


int
syscmd_file_can_read(char *filename)
{
    struct stat stat_buf;

#define FLAG_ISSET(V,F)        (((V) & (F)) == (F))

    if (!stat(filename, &stat_buf))
    {
        if (FLAG_ISSET(stat_buf.st_mode, S_IRUSR))
        {
            return 1;
        }
    }
    return 0;
}

/*******************************************************************************
 * Name:    gen_is_bin_file
 * Purpose: check to determine if this file is bin file
 * Input:
 *   file: file name
 * Output:
 * Return:
 *   success: 1
 *   failed : 0
 * Note:
 ******************************************************************************/
int
gen_is_bin_file(char *file)
{
    char twobytes[2];
    FILE *fp = NULL;

    if (NULL == file)
    {
        return 0;
    }

    if ((fp = fopen(file, "r")) == NULL)
    {
        return 0;
    }

    /* don't try to look ahead if the input is unseekable */
    if (fseek(fp, 0, SEEK_SET))
    {
        fclose(fp);
        return 0;
    }

    if (fread(twobytes, 2, 1, fp) == 1)
    {
        switch((twobytes[0] + (twobytes[1]<<8)) & 0xFFFF)
        {
        case 0407:
        case 0410:
        case 0413:
        case 0405:
        case 0411:
        case 0x457f:
        case 0177545:
        case 0x8b1f: /* it's gzip file */
        case 0x1f8b:
        case 0x0527: /* it's image file */
        case 0x2705:
            (void)fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int
syscmd_is_file(char *filename)
{
    struct stat stat_buf;

    if (!stat(filename, &stat_buf))
    {
        if (S_ISREG (stat_buf.st_mode))
        {
            return 1;
        }
    }

    return 0;
}

int
syscmd_check_actual_directory_exist(char * filename)
{
    int nIdx = 0;
    int nValLen = 0;
    int nCnt = sizeof(actual_root_dir)/sizeof(actual_root_dir[0]);
    rootdir_t  * rdir = NULL;

    for(nIdx = 0; nIdx < nCnt; nIdx ++)
    {
        rdir = & actual_root_dir[nIdx];
        nValLen = sal_strlen(filename);
        if(!sal_strncmp(filename, rdir->real_name, nValLen))
        {
            if(syscmd_is_directory(filename))
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
    }
    return 0;
}

int memmgr_free_cached_mem()
{
  system("sync && echo 3 > /proc/sys/vm/drop_caches");
  return 0;
}

#define BUFFER_COUNT  4096
int
syscmd_copy_file(char *src_file, char *dst_file)
{
    FILE *src_fp = NULL, *dst_fp = NULL;
    char buf[BUFFER_COUNT];
    int ret = 0, count;
    int w_count = 0;
    int amount = 0;

    if (!sal_strcmp(src_file, dst_file))
    {
        return 0;
    }

    src_fp = sal_fopen(src_file, "r");
    if (NULL == src_fp)
    {
        ret = -1;
        goto error;
    }

    dst_fp = sal_fopen(dst_file, "w+");
    if (NULL == dst_fp)
    {
        ret = -2;
        goto error;
    }

    while ((count = sal_fread(buf, sizeof(char), BUFFER_COUNT, src_fp)) > 0)
    {
        w_count = sal_fwrite(buf, sizeof(char), count, dst_fp);

        if (w_count < count)
        {
            ret = -2;
            goto error;
        }

        /* check the memory on 1M boundary */
        amount += count;
        if(amount & 0x100000)
        {
            ret = -3;
            goto error;
        }
    }
error:
    if (NULL != src_fp)
    {
        sal_fclose(src_fp);
    }
    if (NULL != dst_fp)
    {
        sal_fclose(dst_fp);
    }
    memmgr_free_cached_mem();

    return ret;
}

/*******************************************************************************
 * Name:    copy_file_with_progress
 * Purpose: copy file and show progress
 * Input:
 *   src_file: source file
 *   dst_file: dest file
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
#define HASHBYTES (50 * 1024)
#define MEMCHECK  (4 * 1024 *1024) /*4M*/
int
copy_file_with_progress(char *src_file, char *dst_file)
{
    FILE *src_fp = NULL, *dst_fp = NULL;
    char buf[BUFFER_COUNT];
    int ret = 0, count;
    int amount = 0;
    int hashbytes = 0;
    int memcheck = 0;
    struct timeval tstart;
    struct timeval tstop;

    if (!sal_strcmp(src_file, dst_file))
    {
        return 0;
    }

    src_fp = sal_fopen(src_file, "r");
    if (NULL == src_fp)
    {
        ret = -1;
        goto error;
    }

    dst_fp = sal_fopen(dst_file, "w+");
    if (NULL == dst_fp)
    {
        ret = -2;
        goto error;
    }

    (void)gettimeofday(&tstart, NULL);
    while ((count = sal_fread(buf, sizeof(char), BUFFER_COUNT, src_fp)) > 0)
    {
        ret = sal_fwrite(buf, sizeof(char), count, dst_fp);
        if (ret <= 0)
        {
            (void)gettimeofday(&tstop, NULL);
            goto error;
        }
        amount += count;

        if(amount >= memcheck)
        {
            if((syslimit_mem_threshold_check() < 0))
            {
                ret = -3;
                goto error;
            }
            memcheck += MEMCHECK;
        }

        while (amount >= hashbytes)
        {
            ctc_cli_out(".");
            (void) fflush(stdout);
            hashbytes += HASHBYTES;
        }
    }
    (void)gettimeofday(&tstop, NULL);

error:
    if (NULL != src_fp)
    {
        sal_fclose(src_fp);
    }
    if (NULL != dst_fp)
    {
        sal_fclose(dst_fp);
    }
    if (amount && ( ret >= 0))
    {
        double  delta;

        /* compute delta in 1/10's second units */
        delta = ((tstop.tv_sec * 10.) + (tstop.tv_usec / 100000)) -
                ((tstart.tv_sec * 10.) + (tstart.tv_usec / 100000));
        delta = delta / 10.;    /* back to seconds */
        ctc_cli_out("\n%lu bytes in %.1f seconds, %.0f kbytes/second\n",
                amount, delta, (amount * 1.) / (1024 * delta));
    }
    return ret;
}

/*******************************************************************************
 * Name:   app_show_mem_summary_info
 * Purpose: show summary memory info
 * Input:
 * Output:  total, free, buf
 * Return:
 *   success: 0
 *   failed : -1
 * Note: N/A
 ******************************************************************************/
int
app_show_mem_summary_info(int *total, int *free, int *buf)
{
  FILE *pFile = NULL;
  char strLine[MAX_LINE_CHAR] = {0};
  char strName[MAX_LINE_CHAR]= {0};
  int nTotal, nFree, nBuffers;

  pFile = fopen("/proc/meminfo", "r");
  if((NULL == pFile) || (feof(pFile)))
  {
    ctc_cli_out("%% Can not open meminfo file.");
    return -1;
  }

  memset(strLine, 0, sizeof(strLine));
  memset(strName, 0, sizeof(strName));

  /*first line is total memory size*/
  fgets(strLine, MAX_LINE_CHAR, pFile);
  sscanf(strLine, "%s%d", strName, &nTotal);

  /*second line is free memory size*/
  fgets(strLine, MAX_LINE_CHAR, pFile);
  sscanf(strLine, "%s%d", strName, &nFree);

  /*third line is buffer memory size*/
  fgets(strLine, MAX_LINE_CHAR, pFile);
  sscanf(strLine, "%s%d", strName, &nBuffers);

  fclose(pFile);

  *total = nTotal;
  *free = nFree;
  *buf = nBuffers;

  return 0;
}

/*******************************************************************************
 * Name:   syslimit_mem_threshold_check
 * Purpose: check memory
 * Input:   N/A
 * Output:  N/A
 * Return:
 *   free enough: 0
 *   free less than expected : -1
 * Note: N/A
 ******************************************************************************/
int
syslimit_mem_threshold_check()
{
    int total = 0;
    int free = 0;
    int buf = 0;

    memmgr_free_cached_mem();
    app_show_mem_summary_info(&total, &free, &buf);

    if(free < total*0.2)
    {
        return -1;
    }

    return 0;
}
