/**
  @file drv_io.h

  @date 2010-02-26

  @version v5.1

  The file implement driver IOCTL defines and macros
*/

#ifndef _DRV_IO_H_
#define _DRV_IO_H_

/*#include "drv_tbl_reg.h"*/
#include "drv_enum.h"
#include "drv_common.h"
#include "drv_humber.h"



/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/
#define DRV_IOR(mem, memid, fieldid) \
    DRV_IOC(DRV_IOC_READ, (mem), (memid), (fieldid))
#define DRV_IOW(mem, memid, fieldid) \
    DRV_IOC(DRV_IOC_WRITE, (mem), (memid), (fieldid))



/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @brief the table I/O control API
*/
extern int32
drv_tbl_ioctl(uint8 chip_id, int32 index, uint32 cmd, void* val );

/**
 @brief the register I/O control API
*/
extern int32
drv_reg_ioctl(uint8 chip_id, int32 index, uint32 cmd, void* val );

/**
 @brief remove tcam entry interface according to key id and index
*/
extern int32
drv_tcam_tbl_remove(uint8 chip_id, int32 tbl_id, uint32 index);

/**
 @brief hash driver I/O interface include write and delete operation
*/
extern int32
drv_hash_key_ioctl(uint8 chip_id, uint32 tbl_id, uint32 tbl_idx,
                   uint32* key, hash_op_type_e operation_type);

/**
 @brief hash driver lookup I/O interface
*/
extern int32
drv_hash_key_lkup_index(uint8 chip_id,
                        cpu_req_hash_key_type_e cpu_hashkey_type,
                        uint32* key,
                        hash_ds_ctl_cpu_key_status_t* hash_cpu_status);



#endif /*end of _DRV_IO_H*/

