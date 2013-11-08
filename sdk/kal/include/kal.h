#ifndef __KAL_H__
#define __KAL_H__

/**
 * @file kal.h
 */

/*
#if !defined(_KAL_LINUX_KM) && !defined(_KAL_LINUX_UM)
#define _KAL_LINUX_UM
#endif
*/

#if defined(SDK_IN_KERNEL)
#undef _KAL_LINUX_KM
#define _KAL_LINUX_KM
#elif defined(SDK_IN_USERMODE)
#undef _KAL_LINUX_UM
#define _KAL_LINUX_UM
#elif defined(SDK_IN_VXWORKS)
#undef _KAL_VXWORKS
#define _KAL_VXWORKS
#endif

#define _KAL_DEBUG
#if defined(_KAL_LINUX_KM)
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#elif defined(_KAL_LINUX_UM)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <netpacket/packet.h>
#elif defined(_KAL_VXWORKS)
#include <vxWorks.h>
#include <taskLib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <timers.h>
#define _SDK_NOT_READLINE_
#endif
/**
 * @defgroup types Basic Types
 * @{
 */
#if defined(_KAL_LINUX_UM)

#ifndef int8_defined
#define int8_defined

typedef int8_t      int8;   /**< 8-bit  signed integer */
typedef int16_t     int16;  /**< 16-bit signed integer */
typedef int32_t     int32;  /**< 32-bit signed integer */
typedef int64_t     int64;  /**< 64-bit signed integer */
typedef uint8_t     uint8;  /**< 8-bit  unsigned integer */
typedef uint16_t    uint16; /**< 16-bit unsigned integer */
typedef uint32_t     uint32; /**< 32-bit unsigned integer */
typedef uint64_t    uint64; /**< 64-bit unsigned integer */
typedef float       float32; /**< 32-bit signed float */
typedef double      float64; /**< 64-bit signed float */


#endif /* !int8_defined */
#include <stdbool.h>

#elif defined(_KAL_LINUX_KM)
typedef char           int8;   /**< 8-bit  signed integer */
typedef short          int16;  /**< 16-bit signed integer */
typedef int            int32;  /**< 32-bit signed integer */
typedef long long      int64;  /**< 64-bit signed integer */
typedef unsigned char  uint8;  /**< 8-bit  unsigned integer */
typedef unsigned short uint16; /**< 16-bit unsigned integer */
typedef unsigned long   uint32; /**< 32-bit unsigned integer */
typedef unsigned long long  uint64; /**< 64-bit unsigned integer */
typedef int            bool;   /**< Boolean type */
typedef float          float32; /**< 32-bit signed float */
typedef double         float64; /**< 64-bit signed float */
#elif defined(_KAL_VXWORKS)
typedef signed char    int8;   /**< 8-bit  signed integer */
typedef signed short   int16;  /**< 16-bit signed integer */
typedef signed int     int32;  /**< 32-bit signed integer */
typedef signed long long  int64;  /**< 64-bit signed integer */
typedef unsigned char  uint8;  /**< 8-bit  unsigned integer */
typedef unsigned short uint16; /**< 16-bit unsigned integer */
typedef unsigned int   uint32; /**< 32-bit unsigned integer */
typedef unsigned long long  uint64; /**< 64-bit unsigned integer */
typedef int            bool;   /**< Boolean type */
typedef float          float32; /**< 32-bit signed float */
typedef double         float64; /**< 64-bit signed float */
#endif
/**@}*/ /* End of @defgroup types */

#undef TRUE
#ifndef TRUE
#define TRUE 1
#endif

#undef FALSE
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

/* INLINE define */
#ifndef INLINE
#ifdef _MSC_VER
#define INLINE __inline
#elif __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

#include "kal_error.h"
#include "kal_log.h"
#include "kal_mem.h"
#include "kal_task.h"
#include "kal_event.h"
#include "kal_mutex.h"
#include "kal_timer.h"
#include "kal_intr.h"
#include "kal_slab.h"
#include "kal_memmngr.h"

#define BOOLEAN_BIT(b) ((b) ? 1 : 0)

/* file operation */
#undef kal_open
#define kal_open open

#undef kal_close
#define kal_close close

#undef kal_read
#define kal_read read

#undef kal_write
#define kal_write write
/*memory */
#undef kal_malloc
#define kal_malloc   malloc

#undef kal_realloc
#define kal_realloc realloc

#undef kal_free
#define kal_free   free

#undef kal_memcpy
#define kal_memcpy    memcpy

#undef kal_memset
#define kal_memset  memset

#undef kal_memcmp
#define kal_memcmp   memcmp

#undef kal_memmove
#define kal_memmove  memmove
/*string*/
#undef kal_vprintf
#define kal_vprintf vprintf

#undef kal_sprintf
#define kal_sprintf sprintf

#undef kal_sscanf
#define kal_sscanf sscanf

#undef kal_strcpy
#define kal_strcpy strcpy

#undef kal_strncpy
#define kal_strncpy strncpy

#undef kal_strcat
#define kal_strcat strcat

#undef kal_strncat
#define kal_strncat strncat

#undef kal_strcmp
#define kal_strcmp strcmp

#undef kal_strncmp
#define kal_strncmp strncmp

#undef kal_strlen
#define kal_strlen strlen

#undef kal_fprintf
#define kal_fprintf fprintf

#undef kal_strto32
#define kal_strtos32(x,y,z) strtol((char*)x,(char**)y,z)

#undef kal_strtou32
#define kal_strtou32(x,y,z) strtoul((char*)x,(char**)y,z)

#undef kal_strchr
#define kal_strchr strchr

#undef kal_strstr
#define kal_strstr strstr

#undef kal_strrchr
#define kal_strrchr strrchr

#undef kal_strspn
#define kal_strspn strspn

#undef kal_strerror
#define kal_strerror strerror

#undef kal_strtok
#define kal_strtok strtok

#undef kal_strtol
#define kal_strtol strtol


#undef kal_strtok_r
#define kal_strtok_r strtok_r

#undef kal_tolower
#undef kal_toupper
#define kal_tolower tolower
#define kal_toupper toupper

#undef kal_isspace
#undef kal_isdigit
#undef kal_isxdigit
#undef kal_isalpha
#undef kal_isalnum
#undef kal_isupper
#undef kal_islower
#define kal_isspace isspace
#define kal_isdigit isdigit
#define kal_isxdigit isxdigit
#define kal_isalpha isalpha
#define kal_isalnum isalnum
#define kal_isupper isupper
#define kal_islower islower
#define kal_isprint isprint

#undef kal_ntohl
#undef kal_htonl
#undef kal_ntohs
#undef kal_htons

#define kal_ntohl ntohl
#define kal_htonl htonl
#define kal_ntohs ntohs
#define kal_htons htons

#define kal_printf  kal_print_func

#ifndef _KAL_VXWORKS
#undef kal_snprintf
#define kal_snprintf snprintf

#undef kal_vsnprintf
#define kal_vsnprintf vsnprintf

#undef kal_vsprintf
#define kal_vsprintf vsprintf

#undef kal_strcasecmp
#define kal_strcasecmp strcasecmp

#undef kal_strncasecmp
#define kal_strncasecmp strncasecmp

#undef kal_inet_pton
#define kal_inet_pton inet_pton

#undef kal_inet_ntop
#define kal_inet_ntop inet_ntop
#else
extern int32 kal_vsnprintf(int8 *buf, int32 bufsize, const int8 *fmt, va_list ap);
extern int32 kal_vsprintf(int8 *buf, const int8 *fmt, va_list ap);
extern int32 kal_snprintf(int8 *buf, int32 bufsize, const int8 *fmt, ...);
extern const int8 *kal_inet_ntop(int32 af, void *src, int8 *dst, uint32 size);
extern int kal_inet_pton(int32 af, const int8 *src, void *dst);
extern int kal_strcasecmp (const int8 *s1, const int8 *s2);
#endif

#undef kal_qsort
#define kal_qsort qsort

#define SET_BIT(flag, bit)      (flag) = (flag) | (1 << (bit))
#define CLEAR_BIT(flag, bit)    (flag) = (flag) & (~(1 << (bit)))
#define IS_BIT_SET(flag, bit)   (((flag) & (1 << (bit))) ? 1: 0)

#define SET_BIT_RANGE(dst,src,s_bit,len) \
    { \
        uint8 i = 0;\
        for (i = 0; i < len; i++)\
        {\
            if(IS_BIT_SET(src,i))\
            {\
                SET_BIT(dst,(s_bit+i));\
            }\
            else\
            {\
                CLEAR_BIT(dst,(s_bit+i));\
            }\
        }\
    }

#ifdef _KAL_VXWORKS
#define PTR_TO_INT(x)       ((uint32)(((uint32)(x))&0xFFFFFFFF))
#define INT_TO_PTR(x)       ((void *)(uint32)(x))

struct in6_addr
{
    union
    {
        uint8       u6_addr8[16];
        uint16      u6_addr16[8];
        uint32      u6_addr32[4];
    }
    in6_u;
#define s6_addr         in6_u.u6_addr8
#define s6_addr16       in6_u.u6_addr16
#define s6_addr32       in6_u.u6_addr32
};

#define AF_INET6	10  /* IP version 6 */
#endif

extern kal_err_t kal_init(void);
extern void kal_deinit(void);

#endif /* !__KAL_H__ */

