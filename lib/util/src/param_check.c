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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <netdb.h>

#include "param_check.h"
#include "sal.h"
#include "ctc_image.h"

#include <sys/statfs.h>

#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <assert.h>

#ifndef XCALLOC
#define XCALLOC(mtype, size)  calloc(1, size)
#endif
#ifndef XMALLOC
#define XMALLOC(mtype, size)  malloc(size)
#endif
#ifndef XREALLOC
#define XREALLOC(mtype, ptr, size) realloc (ptr, size)
#endif
#ifndef XFREE
#define XFREE(mtype, ptr) free(ptr)
#endif
extern char* strdup_ctc(char* str);
#define XSTRDUP(mtype, str) strdup_ctc (str)

/* file size unit enum */
typedef enum
{
    NONE = 0,
    KILO = 1,
    MEGA = 2,
    GIGA = 3,
    TERA = 4,
    PETA = 5,
    EXA = 6
} unit_type;

/* These three arrays MUST be in sync!  XXX make a struct */
static unit_type units[] = { NONE, KILO, MEGA, GIGA, TERA, PETA, EXA };
static char scale_chars[] = "BKMGTPE";
#define SCALE_LENGTH (sizeof(units)/sizeof(units[0]))
static long long scale_factors[] = {
    1LL,
    1024LL,
    1024LL*1024,
    1024LL*1024*1024,
    1024LL*1024*1024*1024,
    1024LL*1024*1024*1024*1024,
    1024LL*1024*1024*1024*1024*1024,
};
extern int memmgr_free_cached_mem();

/*******************************************************************************
 * Name:    fmt_scaled
 * Purpose:
 *   Format the given "number" into human-readable form in "result".
 *   Result must point to an allocated buffer of length FMT_SCALED_STRSIZE.
 * Input:
 *   number: number value
 *   result: convert result buffer
 * Output: N/A
 *   result: convert result
 * Return:
 *   success: 0
 *   failed: -1
 * Note: N/A
 ******************************************************************************/
int fmt_scaled(long long number, char *result)
{
    long long abval, fract = 0;
    unsigned int i;
    unit_type unit = NONE;

    abval = (number < 0LL) ? -number : number;    /* no long long_abs yet */

    /* Not every negative long long has a positive representation.
     * Also check for numbers that are just too darned big to format
     */
    if (abval < 0 || abval / 1024 >= scale_factors[SCALE_LENGTH-1]) {
        errno = ERANGE;
        return -1;
    }

    /* scale whole part; get unscaled fraction */
    for (i = 0; i < SCALE_LENGTH; i++) {
        if (abval/1024 < scale_factors[i]) {
            unit = units[i];
            fract = (i == 0) ? 0 : abval % scale_factors[i];
            number /= scale_factors[i];
            if (i > 0)
                fract /= scale_factors[i - 1];
            break;
        }
    }

    fract = (10 * fract + 512) / 1024;
    /* if the result would be >= 10, round main number */
    if (fract == 10) {
        if (number >= 0)
            number++;
        else
            number--;
        fract = 0;
    }

    if (number == 0)
        strncpy(result, "0B", FMT_SCALED_STRSIZE);
    else if (unit == NONE || number >= 100 || number <= -100) {
        if (fract >= 5) {
            if (number >= 0)
                number++;
            else
                number--;
        }
        (void)snprintf(result, FMT_SCALED_STRSIZE, "%lld%c",
            number, scale_chars[unit]);
    } else
        (void)snprintf(result, FMT_SCALED_STRSIZE, "%lld.%1lld%c",
            number, fract, scale_chars[unit]);

    return 0;
}


/*******************************************************************************
 * Name:    check_hostname
 * Purpose: check hostname
 *   Only support [0-9A-Za-z.-_]
 * Input:  pszHostname: host name string
 * Output: N/A
 * Return:
 *   success: 0
 *   fail: -1
 * Note: N/A
 ******************************************************************************/
int check_hostname(const char *pszHostname)
{
    int nLen = 0;
    char *p = NULL;

    if (NULL == pszHostname)
    {
        return -1;
    }

    /* check the string length */
    nLen = sal_strlen(pszHostname);
    if ((nLen < M_HOSTNAME_MINSIZE) || (nLen > M_HOSTNAME_MAXSIZE))
    {
        return -1;
    }

    /*check the first character*/
    if(!sal_isalpha(*pszHostname))
    {
        return -1;
    }

    /* check the chars */
    p = (char *)(pszHostname + 1);
    while ('\0' != *p)
    {
        if ((sal_isdigit(*p))
            || (sal_isalpha(*p))
            || ('.' == *p)
            || ('-' == *p)
            || ('_' == *p))
        {
            p++;
            continue;
        }

        return -1;
    }

    /*check the last character*/
    p--;
    if(!(sal_isdigit(*p) || sal_isalpha(*p)))
    {
        return -1;
    }

    return 0;
}

int
system_cmd_file_exist(char *filename)
{
    struct stat stat_buf;

    if (!sal_stat(filename, &stat_buf))
    {
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * Name:    check_disk_free space
 * Purpose: check disk free space
 * Input:
 *   pszName: key name string
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
 int64_t
 check_disk_free_space(char* name)
 {
     struct statfs st;
     int64_t llSize = 0ll;

     if (statfs(name, &st) != 0)
     {
         return -1;
     }

     llSize = (u_int64_t)st.f_bavail * st.f_bsize;
     return llSize;
 }

/*******************************************************************************
 * Name:    check_space_available
 * Purpose: check disk is available for copy or not
 * Input:
 *   pszName: key name string
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
 int
 check_space_available(char * src_filename, char * dst_filename, int64_t size)
 {
    int64_t llsize = 0ll;
    FILE* fp = NULL;
    char *dst_dir = NULL;
    char *p_save_ptr = NULL;
    char *p_tmp = NULL;
    int ret = 0;

    /* getting dst_filename string copy */
    p_tmp = sal_strdup(dst_filename);
    if (NULL == p_tmp)
    {
        errno = ENOMEM;
        ret =  -1;
        goto out;
    }

    /* remove /mnt from string */
    dst_dir = sal_strtok_r(p_tmp, "/", &p_save_ptr);
    if(NULL == dst_dir)
    {
        ret = -1;
        goto out;
    }

    if(!sal_strncmp(dst_dir, "tmp", 3))
    {
        ret = 0;
        goto out;
    }

    /* get the disk name */
    dst_dir = sal_strtok_r(NULL, "/", &p_save_ptr);
    if(NULL == dst_dir)
    {
        ret = -1;
        goto out;
    }

    if (size == 0 && system_cmd_file_exist(dst_filename))
    {
        fp = fopen(dst_filename, "r" );
        if (fp == NULL)
        {
            ret = -1;
            goto out;
        }

        fseek(fp, 0L, SEEK_END );
        size = (int64_t)ftell(fp);
        fclose(fp);
    }

    if(!sal_strncmp(dst_dir, "flash", 5))
    {
        llsize = check_disk_free_space("/mnt/flash");
    }
    else if (!sal_strncmp(dst_dir, "data", 4))
    {
        llsize = check_disk_free_space("/mnt/data");
    }
    else if(!sal_strncmp(dst_dir, "udisk", 5))
    {
        llsize = check_disk_free_space("/mnt/udisk");
    }
    else
    {
        ret = -1;
        goto out;
    }

    if (llsize == -1)
    {
        ret = -1;
        goto out;
    }

    fp = sal_fopen(src_filename, "r" );
    if (fp == NULL)
    {
        ret = -2;
        goto out;
    }

    sal_fseek(fp, 0L, SEEK_END );
    sal_ftell(fp);

    if(!sal_strncmp(dst_dir, "udisk", 5))
    {
        if((llsize + size) < (int64_t)sal_ftell(fp))
        {
            ret = -3;
            goto out;
        }

        ret =  0;
        goto out;
    }

    /* we left 1M space in flash for important manipulation */
    if ((llsize + size) < (int64_t)sal_ftell(fp)
        || (llsize + size - (int64_t)sal_ftell(fp)) < 1024*1024)
    {
        ret = -3;
        goto out;
    }

out:
    if (p_tmp)
    {
        sal_free(p_tmp);
    }
    if (fp)
    {
        sal_fclose(fp);
    }
    return ret;
 }

 /*******************************************************************************
 * Name:    check_filename
 * Purpose: checking filename string validity
 *   Only support [0-9A-Za-z.-_()]
 * Input:  pszFilename: file name string
 * Output: N/A
 * Return:
 *   success: 0
 *   fail: -1
 *
 ******************************************************************************/
int check_filename(const char *pszFilename)
{
    int nLen = 0;
    char *p = NULL;

    if (NULL == pszFilename)
    {
        return -1;
    }

    /* check the string length */
    nLen = strlen(pszFilename);
    if (nLen < M_FILENAME_MINSIZE || nLen >= M_FILENAME_MAXSIZE)
    {
        return -2;
    }

    /* check the chars */
    p = (char *)pszFilename;
    while ('\0' != *p)
    {
        if ((sal_isdigit(*p))
            || (sal_isupper(*p))
            || (sal_islower(*p))
            || ('.' == *p)
            || ('-' == *p)
            || ('_' == *p)
            || ('(' == *p)
            || (')' == *p))
        {
            p++;
            continue;
        }

        return -1;
    }

    return 0;
}

 /*******************************************************************************
 * Name:    split_tokens
 * Purpose: split tokens (we only support 100 tokens)
 * Input:
 *   pszBuf: string buffer
 *   nLen: string length
 *   nMaxToks: limit token numbers
 *   pszDelimiters: delimiters
 *   pfCheck: validate function. return 0 for success, others for fail
 * Output:
 *   pnToks: token numbers
 *   ppTokArr: token array
 * Return:
 *   success:  0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
split_tokens(char *pszBuf, size_t nLen, unsigned long nMaxToks,
             char *pszDelimiters, unsigned long *pnToks, char ***pppTokArr)
{
    char *pTmp = NULL;
    char *pTok = NULL;
    char **ppTokArr = NULL;
    char *pSavePtr = NULL;
    unsigned long nArrSize = nMaxToks + 1;
    unsigned long nCurTok = 0;

    /* validate params */
    if (NULL == pszBuf || nLen <= 0
        || nMaxToks <= 0 || nMaxToks > 128
        || NULL == pszDelimiters || NULL == pnToks
        || NULL == pppTokArr)
    {
        return -1;
    }

    /* getting string copy */
    pTmp = XSTRDUP(0, pszBuf);
    if (NULL == pTmp)
    {
        errno = ENOMEM;
        return -1;
    }

    /* calloc token array */
    ppTokArr = XCALLOC(0, sizeof(char *) * nArrSize);
    if (NULL == ppTokArr)
    {
        errno = ENOMEM;
        goto error_out;
    }

    pTok = sal_strtok_r(pTmp, pszDelimiters, &pSavePtr);
    while (NULL != pTok)
    {
        nCurTok++;
        if (nCurTok > nMaxToks)
        {
            errno = EINVAL;
            goto error_out;
        }

        /* save current token */
        ppTokArr[nCurTok - 1] = XSTRDUP(0, pTok);
        if (NULL == ppTokArr[nCurTok - 1])
        {
            errno = ENOMEM;
            goto error_out;
        }

        /* process next token */
        pTok = sal_strtok_r(NULL, pszDelimiters, &pSavePtr);
    }

    /* success */
    *pnToks = nCurTok;
    *pppTokArr = ppTokArr;

    /* free memory */
    if (NULL != pTmp)
    {
        XFREE(0, pTmp);
    }
    return 0;

error_out:
    if (NULL != pTmp)
    {
        XFREE(0, pTmp);
    }

    free_tokens(&ppTokArr);
    return -1;
}

/*******************************************************************************
 * Name:    free_tokens
 * Purpose: free split result
 * Input:
 *   pppTokArr: token array
 * Output:
 * Return:
 *   success:  0
 *   failed : -1
 * Note:
 ******************************************************************************/
int free_tokens(char ***pppTokArr)
{
    char **ppTokArr = NULL;
    int i = 0;

    if (NULL == pppTokArr || NULL == *pppTokArr)
    {
        return -1;
    }
    ppTokArr = *pppTokArr;

    i = 0;
    while (1)
    {
        if (NULL == ppTokArr[i])
        {
            break;
        }

        XFREE(0, ppTokArr[i]);
        i++;
    }

    XFREE(0, ppTokArr);
    return 0;
}

/*******************************************************************************
 * Name:    check_username
 * Purpose: check username
 *   it length range is [1, M_MAX_USERNAME_LEN),
 *   Only support [0-9A-Za-z.-_], and start with alpha
 * Input:
 *   username: username string
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
check_username(char *username)
{
    char *p = username;

    if (NULL == p || !sal_isalpha(*p))
    {
        return -1;
    }

    if (sal_strlen(username) >= M_MAX_USERNAME_LEN)
    {
        return -1;
    }

    /* check the chars */
    while ('\0' != *p)
    {
        if ((sal_isdigit(*p))
            || (sal_isupper(*p))
            || (sal_islower(*p))
            || ('.' == *p)
            || ('-' == *p)
            || ('_' == *p))
        {
            p++;
            continue;
        }

        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Name:    check_password
 * Purpose: check password string,
 *    limit it length not overflow, and it must being printable
 * Input:
 *   pin: password string
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
check_password(char *pin)
{
    char *p = pin;

    if (NULL == pin)
    {
        return -1;
    }

    if (sal_strlen(pin) >= M_MAX_PASSWORD_LEN)
    {
        return -1;
    }

    while ('\0' != *p)
    {
        if (!sal_isprint(*p))
        {
            return -1;
        }

        p++;
    }
    return 0;
}

/* include ipv6 and ipv4 and hostname */
int
gen_check_ipaddress(char *p)
{
    int len;
    int error;
    struct addrinfo hints, *res, *res_save;

    if (NULL == p || '\0' == *p)
    {
        return -1;
    }
    len = sal_strlen(p);

    if (len >= MAXHOSTNAMELEN)
    {
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    error = getaddrinfo(p, NULL, &hints, &res_save);
    if (error)
    {
        return -1;
    }
    res = res_save;
    if (!res)
    {
        return -1;
    }

    freeaddrinfo(res_save);
    return 0;
}


/*******************************************************************************
 * Name:    do_encrypt
 * Purpose: do encrypt, have '\0' at the buffer end
 * Input:
 *   pw: password
 *   ibuf: input buffer
 *   ilen: input length
 *   olen: output length
 * Output:
 *   obuf: output buffer
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
do_encrypt(char *pw, char *ibuf, size_t ilen,
           char *obuf, size_t olen)
{
#if 0
    unsigned char ebuf[M_PASSWORD_BUFSIZE];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char key[EVP_MAX_KEY_LENGTH];
    EVP_CIPHER_CTX ectx;
    char *pout = NULL;
    int ebuflen = 0;
    int nretlen = 0;
    int nleftlen = olen;
    int i = 0;

    if (NULL == ibuf || NULL == ebuf)
    {
        return -1;
    }

    obuf[0] = '\0';
    sal_memcpy(iv, INIT_VECTOR, sizeof(iv));
    EVP_BytesToKey(M_ALG, EVP_md5(), (unsigned char*)"salu", (unsigned char*)pw, strlen(pw), 1, key, iv);
    EVP_CIPHER_CTX_init(&ectx);
    EVP_CipherInit_ex(&ectx, M_ALG, NULL, key, iv, E_PASSWORD_ENCRYPT);
    EVP_CipherUpdate(&ectx, ebuf, &ebuflen, (unsigned char*)ibuf, ilen);
    if (ebuflen < 0)
    {
        EVP_CIPHER_CTX_cleanup(&ectx);
        obuf[0] = '\0';
        return -1;
    }

    pout = (char*)obuf;
    for (i = 0; i < ebuflen; i++)
    {
        if (nleftlen < 3)
        {
            /* output buffer overflow */
            obuf[0] = '\0';
            return -1;
        }

        nretlen = sal_snprintf(pout, nleftlen, "%02x", (ebuf[i] & 0xff));
        nleftlen -= nretlen;
        pout += nretlen;
    }

    EVP_CipherFinal_ex(&ectx, ebuf, &ebuflen);
    EVP_CIPHER_CTX_cleanup(&ectx);
    if (ebuflen < 0)
    {
        obuf[0] = '\0';
        return -1;
    }
    for (i = 0; i < ebuflen; i++)
    {
        if (nleftlen < 3)
        {
            /* output buffer overflow */
            obuf[0] = '\0';
            return -1;
        }

        nretlen = sal_snprintf(pout, nleftlen, "%02x", (ebuf[i] & 0xff));
        nleftlen -= nretlen;
        pout += nretlen;
    }

    if ('\0' == obuf[0])
    {
        return -1;
    }
#endif
    return 0;
}

/*******************************************************************************
 * Name:    do_decrypt
 * Purpose: do decrypt, return string will have '\0'
 * Input:
 *   pw: password
 *   ibuf: input buffer
 *   ilen: input length
 *   olen: output buffer length
 * Output:
 *   obuf: output buffer
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
do_decrypt(char *pw, char *ibuf, size_t ilen,
           char *obuf, size_t olen)
{
#if 0
    char buf[M_PASSWORD_BUFSIZE];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char key[EVP_MAX_KEY_LENGTH];
    EVP_CIPHER_CTX ectx;
    char *pout = NULL;
    int nretlen = 0;
    int i = 0;

    if (ibuf == NULL || NULL == obuf)
    {
        return -1;
    }

    obuf[0] = '\0';
    pout = buf;
    for (i = 0; i < (ilen - 1); i += 2)
    {
        unsigned short value = 0;
        sal_sscanf((ibuf + i), "%2hx", &value);
        *pout = (value & 0xff);
        pout++;
    }

    sal_memcpy(iv, INIT_VECTOR, sizeof(iv));
    EVP_BytesToKey(M_ALG, EVP_md5(), (unsigned char*)"salu", (unsigned char*)pw, strlen(pw), 1, key, iv);
    EVP_CIPHER_CTX_init(&ectx);
    EVP_CipherInit_ex(&ectx, M_ALG, NULL, key, iv, E_PASSWORD_DECRYPT);
    EVP_CipherUpdate(&ectx, (unsigned char*)obuf, &nretlen, (unsigned char*)buf, pout - buf);
    if (nretlen < 0)
    {
        EVP_CIPHER_CTX_cleanup(&ectx);
        obuf[0] = '\0';
        return -1;
    }

    pout = (char*)obuf + nretlen;
    EVP_CipherFinal_ex(&ectx, (unsigned char*)pout, &nretlen);
    EVP_CIPHER_CTX_cleanup(&ectx);
    if (nretlen < 0)
    {
        obuf[0] = '\0';
        return -1;
    }

    pout[nretlen] = '\0';
    if ('\0' == obuf[0])
    {
        return -1;
    }
#endif
    return 0;
}

/* Check enable password.  */
int
host_password_check (char *passwd, char *encrypt, char *input)
{
#ifndef _CENTEC_
  char cryptbuf[16];
#endif
  char *str;
  int ret;

  /* Get enable password.  */
  if (encrypt)
    str = encrypt;
  else
    str = passwd;

  if (input == NULL)
    {
      /* Just check password is set or not.  */
      if (str == NULL)
    return 1;
    }
  else
    {
      if (str != NULL)
    {
      if (encrypt)
#ifndef _CENTEC_
        ret = sal_strcmp (pal_crypt (input, str, cryptbuf), str);
#else
        {
            char szPassword[M_PASSWORD_BUFSIZE];
            if (do_encrypt(M_DEFAULT_PW_STR, input, sal_strlen(input),
                           szPassword, M_PASSWORD_BUFSIZE) != 0)
            {
                return 0;
            }
            ret = sal_strcmp (szPassword, str);
        }
#endif
      else
        ret = sal_strcmp (str, input);

      if (ret == 0)
        return 1;
    }
    }
  return 0;
}

/*******************************************************************************
 * Name:    check_key_name
 * Purpose: check key name for RSA and others
 * Input:
 *   pszName: key name string
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
check_key_name(char *pszName)
{
    char *p = pszName;

    if (NULL == p || !sal_isalpha(*p))
    {
        return -1;
    }

    if (sal_strlen(p) >= M_MAX_KEYNAME_LEN)
    {
        return -1;
    }

    /* check the chars */
    while ('\0' != *p)
    {
        if ((sal_isdigit(*p))
            || (sal_isupper(*p))
            || (sal_islower(*p))
            || ('.' == *p)
            || ('-' == *p)
            || ('_' == *p))
        {
            p++;
            continue;
        }

        return -1;
    }

    return 0;
}

/*******************************************************************************
 * Name:    check_ipaddr
 * Purpose: checking p string validity
 *   Only support [0-9.]
 * Input:  ipaddr : ip address
 * Output: N/A
 * Return:
 *   success: 0
 *   fail: -1
 * Note: N/A
 ******************************************************************************/

int check_ip(const char *ipaddr)
{
    int nLen = 0;
    char *p = NULL;

    if (NULL == ipaddr)
    {
        return -1;
    }

    /* check the string length */
    nLen = strlen(ipaddr);
    if (nLen < M_HOSTNAME_MINSIZE || nLen > M_HOSTNAME_MAXSIZE)
    {
        return -1;
    }

    /* check the chars */
    p = (char *)ipaddr;
    while ('\0' != *p)
    {
        if ((sal_isdigit(*p))
            || ('.' == *p))
        {
            p++;
            continue;
        }

        return -1;
    }

    return 0;
}

/* crc 32 table */
const u_int32_t crc_table[256] =
{
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
/*******************************************************************************
 * Name:    gen_crc32
 * Purpose: calculate crc32
 * Input:
 *   crc: crc
 *   buf: buffer
 *   len: buffer length
 * Output:
 * Return: crc
 * Note:
 ******************************************************************************/
#if 0
u_int32_t
gen_crc32(u_int32_t crc, char *buf, size_t len)
{
    sal_assert(buf != NULL);
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}
#endif
u_int32_t
special_gen_crc32(u_int32_t crc, char *buf, size_t len)
{
    sal_assert(buf != NULL);
    //crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    //return crc ^ 0xffffffffL;
    return crc ;
}

/*******************************************************************************
 * Name:    check_image_file
 * Purpose: check centec image file
 * Input:
 *   filename: image file full name
 * Output:
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
#ifndef O_BINARY
#define O_BINARY    0
#endif
#define M_IMAGE_OPEN_MODE   (O_RDONLY|O_BINARY)
#define M_IMAGE_SET_ERRMSG(...)   \
do \
{ \
    if (errmsg) \
    { \
        sal_snprintf(errmsg, size, __VA_ARGS__); \
    } \
} while (0)

#define BUFFER_COUNT  4096

int
check_image_file(char *filename, char *errmsg, size_t size, bool_t bCRC)
{
    FILE *ifd;
    ctc_image_header_t header;
    ctc_image_header_t *hdr = &header;
    int ret = -1;
    int len = 0,count = 0;
    char *data = NULL;
    uint32_t checksum = 0l;
    uint32_t temp_crc = 0;
    char buf[BUFFER_COUNT];

    if (NULL == filename)
    {
        sal_assert(0);
        M_IMAGE_SET_ERRMSG("Internal error");
        return -1;
    }

    if ((ifd = fopen(filename, "r"))  == NULL)
    {
        M_IMAGE_SET_ERRMSG("Failed to open %s: %s", filename, sal_strerror(errno));
        goto error_out;
    }

    /*
     * create copy of header so that we can blank out the
     * checksum field for checking - this can't be done
     * on the PROT_READ mapped data.
     */
    if(fread(hdr, sizeof(ctc_image_header_t), 1, ifd) != 1)
    {
        M_IMAGE_SET_ERRMSG("Invalid image file");
        goto error_out;
    }

    if (ntohl(hdr->ih_magic) != CTC_IH_MAGIC)
    {
        M_IMAGE_SET_ERRMSG("Invalid image file: bad magic number");
        goto error_out;
    }

    data = (char *)hdr;
    len  = sizeof(ctc_image_header_t);
    checksum = sal_ntoh32(hdr->ih_hcrc);
    hdr->ih_hcrc = sal_hton32(0);  /* clear for re-calculation */
    temp_crc = temp_crc ^ 0xffffffffL;
    temp_crc = special_gen_crc32(temp_crc, data, len);

    temp_crc = temp_crc ^ 0xffffffffL;

    if (temp_crc != checksum)
    {
        M_IMAGE_SET_ERRMSG("Invalid image file: bad header checksum");
        goto error_out;
    }

    temp_crc = 0;
    if (bCRC != 0)
    {
        temp_crc = temp_crc ^ 0xffffffffL;
        while((count = fread(buf, sizeof(char), BUFFER_COUNT, ifd)) > 0)
        {
            if(count == 0)
                break;
            temp_crc = special_gen_crc32(temp_crc, buf, count);
        }
        temp_crc = temp_crc ^ 0xffffffffL;
        if (temp_crc != sal_ntoh32(hdr->ih_dcrc))
        {
            M_IMAGE_SET_ERRMSG("Invalid image file: has corrupted data");
            goto error_out;
        }

    }
    ret = 0;

error_out:


    if (ifd > 0)
    {
        fclose(ifd);
    }

    /*Free cached memory*/
    memmgr_free_cached_mem();

    return ret;
}

/*******************************************************************************
 * Name:    get_package_from_image
 * Purpose: getting package name string from image file,
 *          will don't do crc validation
 * Input:
 *   filename: image file name
 *   size: version buf size
 * Output:
 *   version: version string
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
get_packagename_from_image(char *filename, char *pkgname, size_t size)
{
    ctc_image_header_t hdr;
    FILE *fp = NULL;

    if (NULL == filename || NULL == pkgname)
    {
        return -1;
    }

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }

    if (fread(&hdr, sizeof(ctc_image_header_t), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    sal_snprintf(pkgname, size, hdr.ih_name);
    return 0;
}

/*******************************************************************************
 * Name:    get_createtime_from_image
 * Purpose: getting creation timestamp from image file,
 *          will don't do crc validation
 * Input:
 *   filename: image file name
 * Output:
 *   ptime: time pointer
 * Return:
 *   success: 0
 *   failed : -1
 * Note:
 ******************************************************************************/
int
get_createtime_from_image(char *filename, u_int32_t *ptime)
{
    ctc_image_header_t hdr;
    FILE *fp = NULL;

    if (NULL == filename || NULL == ptime)
    {
        return -1;
    }

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }

    if (fread(&hdr, sizeof(ctc_image_header_t), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    *ptime = sal_ntoh32(hdr.ih_time);
    return 0;
}

/*******************************************************************************
 * Name:    format time string
 * Purpose: call strftime to format string
 * Input:
 *   pszBuf: buffer to hold result
 *   nLen: buffer length
 *   pszFormat: time string format
 *   nTime: time value
 * Output:
 *   pszBuf: result string
 * Return:
 *   success: pszBuf
 *   failed:  NULL
 * Note: N/A
 ******************************************************************************/
char *format_time_str(char *pszBuf, size_t nLen,
                               char *pszFormat, time_t nTime)
{
    struct tm *tm = NULL;

    if (NULL == pszBuf || NULL == pszFormat)
    {
        return NULL;
    }

    tm = localtime(&nTime);
    sal_time_strf(pszBuf, nLen, pszFormat, tm);
    return pszBuf;
}
