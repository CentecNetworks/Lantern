/**
 @file

 @date 2010/02/22

 @version v5.1

 This file contains those macro & enum definitions and global var
*/

#ifndef _DRV_COMMON_H_
#define _DRV_COMMON_H_

#include "kal.h"
#include "drv_error.h"

/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/
extern uint8 current_local_chip_num;
extern uint8 current_global_chip_num;
extern uint16 current_global_port_num;
extern uint8 current_external_chip_id;

#define MAX_LOCAL_CHIP_NUM 2
#define MAX_LOCAL_PORT_NUM 256

#define MAX_ENTRY_WORD 64    /**< define table/reg entry's max words */
#define MAX_ENTRY_BYTE 128   /**< define table/reg entry's max bytes */

#define CONDITIONAL_BREAK(exp)      if((exp)) break
#define CONDITIONAL_CONTINUE(exp)   if((exp)) continue

#define DRV_IOC_DIR_BITS      2
#define DRV_IOC_MEM_BITS      1        /* 1: table, 0: register */
#define DRV_IOC_MEMID_BITS    13       /* table/register ID */
#define DRV_IOC_FIELDID_BITS  16
#define DRV_ENTRY_FLAG        0x1fff   /* when fieldid equals this value, it represent
                                          the operation is applied to the whole entry */
#define DRV_HASH_INVALID_INDEX 0x1FFFF
#define DRV_IOC_DIR_MASK     ((1 << DRV_IOC_DIR_BITS)-1)
#define DRV_IOC_MEM_MASK     ((1 << DRV_IOC_MEM_BITS)-1)
#define DRV_IOC_MEMID_MASK   ((1 << DRV_IOC_MEMID_BITS)-1)
#define DRV_IOC_FIELDID_MASK ((1 << DRV_IOC_FIELDID_BITS)-1)
#define DRV_IOC_FIELDID_SHIFT 0
#define DRV_IOC_MEMID_SHIFT  (DRV_IOC_FIELDID_SHIFT + DRV_IOC_FIELDID_BITS)
#define DRV_IOC_MEM_SHIFT    (DRV_IOC_MEMID_SHIFT + DRV_IOC_MEMID_BITS)
#define DRV_IOC_DIR_SHIFT    (DRV_IOC_MEM_SHIFT + DRV_IOC_MEM_BITS)
#define DRV_IOC_OP(cmd)      (((cmd) >> DRV_IOC_DIR_SHIFT)&DRV_IOC_DIR_MASK)
#define DRV_IOC_MEMID(cmd)   (((cmd) >> DRV_IOC_MEMID_SHIFT)&DRV_IOC_MEMID_MASK)
#define DRV_IOC_FIELDID(cmd) (((cmd) >> DRV_IOC_FIELDID_SHIFT)&DRV_IOC_FIELDID_MASK)

#define CHIP_IS_EXTERNAL_CHIP(chip_id)  ((chip_id) == current_external_chip_id)

/* directions */
#define DRV_IOC_READ  1U
#define DRV_IOC_WRITE 2U

#define DRV_IOC(dir, mem, memid, fieldid) \
    (((dir) << DRV_IOC_DIR_SHIFT) | \
    ((mem) << DRV_IOC_MEM_SHIFT) | \
    ((memid) << DRV_IOC_MEMID_SHIFT) | \
    ((fieldid) << DRV_IOC_FIELDID_SHIFT))


typedef int32 mutex_t;

/* Tcam data mask storage structure */
struct tbl_entry_s
{
    uint32* data_entry;
    uint32* mask_entry;
};
typedef struct tbl_entry_s tbl_entry_t;


/**
 @brief check whether the operation's return value is error or not
*/
#define DRV_IF_ERROR_RETURN(op) \
{ \
    int32 rv; \
    if ((rv = (op)) < 0) \
    { \
        return(rv); \
    }\
}


/**
 @brief define the pointer valid check
*/
#define DRV_PTR_VALID_CHECK(ptr)\
if (NULL == (ptr))\
{\
    return DRV_E_INVALID_PTR;\
}


#ifdef SDK_IN_DEBUG_VER
#define DRV_DBG_INFO(FMT, ...)                          \
                    { \
                      kal_printf(FMT, ##__VA_ARGS__); \
                    }

#define DRV_DBG_FUNC()                          \
                    {\
                         kal_printf("\n%s()\n", __FUNCTION__); \
                    }
 #else
    #define DRV_DBG_INFO(FMT, ...)
    #define DRV_DBG_FUNC()
 #endif


#endif /*end of _DRV_COMMON_H*/

