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

#ifndef __GLB_MACRO_H__
#define __GLB_MACRO_H__

/**
 @brief define bit operations
*/
#define GLB_IS_BIT_SET(flag, bit)   (((flag) & (1 << (bit))) ? 1: 0)
#define GLB_SET_BIT(flag, bit)      (flag) = (flag) | (1 << (bit))
#define GLB_CLEAR_BIT(flag, bit)    (flag) = (flag) & (~(1 << (bit)))

#define GLB_SET_FLAG(VAL,FLAG)          (VAL) = (VAL) | (FLAG)
#define GLB_UNSET_FLAG(VAL,FLAG)        (VAL) = (VAL) & ~(FLAG)
#define GLB_FLAG_ISSET(VAL,FLAG)        (((VAL) & (FLAG)) == (FLAG))

/**
 @brief define mcast addr check
*/
#define GLB_IS_MCAST_MAC_ADDR(mac0) ((mac0&0x1) == 1)

#define GLB_PTR_VALID_CHECK(ptr, err_code)    \
        if ( NULL == (ptr) )                      \
        {                                           \
            return err_code;                      \
        }

#define GLB_MEM_PTR_CHECK(ptr, err_code)\
        if ( NULL == (ptr) )\
        {\
            return err_code;\
        }

#define GLB_MAX_VALUE_CHECK(var,max_value, err_code) \
        if((var) > (max_value))\
        {\
            return err_code;\
        }

#define GLB_EQUAL_CHECK(var, value, err_code) \
do { \
        if((var) != (value)) \
        { \
            return err_code; \
        } \
} while(0)

#define GLB_CTC_SWAP16(val) \
        ((uint16)( \
                (((uint16)(val) & (uint16)0x00ffU) << 8) | \
                (((uint16)(val) & (uint16)0xff00U) >> 8) ))

#define GLB_CTC_SWAP32(val) \
        ((uint32)( \
                (((uint32)(val) & (uint32)0x000000ffUL) << 24) | \
                (((uint32)(val) & (uint32)0x0000ff00UL) <<  8) | \
                (((uint32)(val) & (uint32)0x00ff0000UL) >>  8) | \
                (((uint32)(val) & (uint32)0xff000000UL) >> 24) ))

#define UINT64_TO_OCTET_STR(data, str)                              \
do{                                                                 \
    int32 i;                                                        \
    str.buf = mem_malloc(MEM_STATS_MODULE, sizeof(uint64));         \
    str.size = sizeof(uint64);                                      \
    for(i = 0; i < sizeof(uint64); i++)                             \
    {                                                               \
        str.buf[i] = (uint8)((data >> (i*8))&0xFF);                 \
    }                                                               \
}while(0)

#define OCTET_STR_TO_UINT64(data, str)                              \
do{                                                                 \
    int32 i;                                                        \
    data = 0;                                                       \
    for(i = 0; i < str.size; i++)                                   \
    {                                                               \
        data += ((uint64)str.buf[i]) << (i*8);                      \
    }                                                               \
}while(0)

#define FREE_OCTET_STR(str) mem_free(str.buf)

/* 2^64 - 1 = 18,446,744,073,709,551,615 */
#define GLB_INT64_DEC_LENGTH    24
#define INT64_TO_STR(data, str)                                     \
do{                                                                 \
    sal_memset(str, 0x00, GLB_INT64_DEC_LENGTH);                    \
    sal_snprintf(str, GLB_INT64_DEC_LENGTH, "%lld", data);          \
}while(0)

#define UINT64_TO_STR(data, str)                                    \
do{                                                                 \
    sal_memset(str, 0x00, GLB_INT64_DEC_LENGTH);                    \
    sal_snprintf(str, GLB_INT64_DEC_LENGTH, "%llu", data);          \
}while(0)

#endif /* __GLB_MACRO_H__*/
