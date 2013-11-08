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

#ifndef __PARAM_CHECK_H__
#define __PARAM_CHECK_H__

/* key name */
#define M_MAX_KEYNAME_LEN       32
#define M_KEYNAME_LIMIT_STR     "starting with alphanumeric and can only include [0-9a-zA-Z.-_]. Its length range is [1, 32)."
#ifdef _CENTEC_
#define M_COMMUNITY_LIMIT_STR     "starting with alphabetic and can only include [0-9a-zA-Z.-_]. Its length range is [1, 256)."
#endif

/* username */
#define M_MAX_USERNAME_LEN      32
#define M_USERNAME_LIMIT_STR    "can only include [0-9a-zA-Z.-_], and must start with alphanumeric. Its length range is [1, 32)."

/* hostname buffer size */
#define MAXHOSTNAMELEN 64
#define M_HOSTNAME_MAXSIZE    (MAXHOSTNAMELEN - 1)
#define M_HOSTNAME_MINSIZE    1

/* filename buffer size */
#define M_FILENAME_MAXSIZE      48
#define M_FILENAME_MINSIZE      1
#define M_FILENAME_LIMIT_STR    "can only include [0-9a-zA-Z.-_]. Its length range is [1, 48)."

#define M_FULLPATH_MAX_LEN    256
#define M_FULLPATH_DEPTH_MAX    32

/* password length limit */
#define M_MAX_PASSWORD_LEN      64
#define M_PASSWORD_BUFSIZE      256
#define M_PASSWORD_LIMIT_STR    "can only include printable characters and its length range is [1, 64)."
/* password process flags */
#define E_PASSWORD_ENCRYPT      0x1
#define E_PASSWORD_DECRYPT      0x0
/* default password for encrypt/decrypt */
#define M_DEFAULT_PW_STR    "centecnetworks.com"
/* INIT VEECTOR for encrypt/decrypt */
#define INIT_VECTOR         "c1entec2"
#define M_ALG               EVP_des_ede3_cbc()

/*
 * "Human-readable" output uses 4 digits max, and puts a unit suffix at
 * the end.  Makes output compact and easy-to-read esp. on huge disks.
 */
#define FMT_SCALED_STRSIZE  7   /* minus sign, 4 digits, suffix, null byte */

/* Privilege level.  */
#ifdef _CENTEC_
#define PRIVILEGE_MIN         1
#define PRIVILEGE_LEVEL_TWO         2
#define PRIVILEGE_LEVEL_THREE        3
#define PRIVILEGE_VR_MAX     4

#define PRIVILEGE_CENTEC_MIN   (PRIVILEGE_MIN+10)
#define PRIVILEGE_CENTEC_LEVEL_TWO         (PRIVILEGE_LEVEL_TWO+10)
#define PRIVILEGE_CENTEC_LEVEL_THREE        (PRIVILEGE_LEVEL_THREE+10)
#define PRIVILEGE_CENTEC_MAX     (PRIVILEGE_VR_MAX+10)
#else
#define PRIVILEGE_MIN         1
#define PRIVILEGE_VR_MAX     15
#endif
#define PRIVILEGE_PVR_MAX    16

#define PRIVILEGE_NORMAL    PRIVILEGE_MIN
#ifdef HAVE_VR
#define PRIVILEGE_MAX       (PRIVILEGE_PVR_MAX)
#define PRIVILEGE_ENABLE(V)                                                   \
    ((V)->id == 0 ? PRIVILEGE_PVR_MAX : PRIVILEGE_VR_MAX)
#else /* HAVE_VR */
#define PRIVILEGE_MAX       (PRIVILEGE_VR_MAX)
#define PRIVILEGE_ENABLE(V) (PRIVILEGE_VR_MAX)
#endif /* HAVE_VR */

typedef unsigned char bool_t;
/* Format the given "number" into human-readable form in "result".
 * Result must point to an allocated buffer of length FMT_SCALED_STRSIZE.
 * Return 0 on success, -1 and errno set if error.
 */
int fmt_scaled(long long number, char *result);
int check_hostname(const char *pszHostname);
int check_filename(const char *pszFilename);
extern int check_space_available(char * src_filename, char * dst_filename, int64_t size);

/* process tokens */
int split_tokens(char *pszBuf, size_t nLen, unsigned long nMaxToks,
                 char *pszDelimiters, unsigned long *pnToks, char ***pppTokArr);
int free_tokens(char ***pppTokArr);

/* username & password limit */
int check_username(char *username);
int check_password(char *pin);
int gen_check_ipaddress(char *hostname_or_address); /* support ipv4 and ipv6 */
int do_encrypt(char *pw, char *ibuf, size_t ilen,
           char *obuf, size_t olen);
int do_decrypt(char *pw, char *ibuf, size_t ilen,
           char *obuf, size_t olen);
int host_password_check (char *passwd, char *encrypt, char *input);

int check_key_name(char *pszName);

int check_ip(const char *ipaddr);
int
get_packagename_from_image(char *filename, char *pkgname, size_t size);
int
get_createtime_from_image(char *filename, u_int32_t *ptime);
char *format_time_str(char *pszBuf, size_t nLen,
                               char *pszFormat, time_t nTime);
int
check_image_file(char *filename, char *errmsg, size_t size, bool_t bCRC);
#endif /* !__PARAM_CHECK_H__ */
