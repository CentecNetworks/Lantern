/**
 @file sys_humber_hash.h

 @date 2010-02-25

 @version v5.1

 The file implement the hash external transfer interfaces declaration
*/
#ifndef _SYS_HUMBER_HASH_H
#define _SYS_HUMBER_HASH_H

#include "kal.h"

/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/


/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup hash
 @{
*/

/**
 @brief left bucket mac hash arithmetic
*/
extern int16
sys_humber_hash_generate_mac_hash0(uint8* sed, uint32 bit_num);

/**
 @brief right bucket mac hash arithmetic
*/
extern int16
sys_humber_hash_generate_mac_hash1(uint8* sed, uint32 bit_num);

/**
 @brief left bucket ipv4 hash arithmetic
*/
extern int16
sys_humber_hash_generate_ipv4_hash0(uint8* sed, uint32 bit_num);

/**
 @brief right bucket ipv4 hash arithmetic
*/
extern int16
sys_humber_hash_generate_ipv4_hash1(uint8* sed, uint32 bit_num);

/**
 @brief left bucket ipv6 hash arithmetic
*/
extern int16
sys_humber_hash_generate_ipv6_hash0(uint8* sed, uint32 bit_num);

/**
 @brief right bucket ipv6 hash arithmetic
*/
extern int16
sys_humber_hash_generate_ipv6_hash1(uint8* sed, uint32 bit_num);


/**@}*/ /*end of @addtogroup*/

#endif /*end of _DRV_HUMBER_HASH_H*/

