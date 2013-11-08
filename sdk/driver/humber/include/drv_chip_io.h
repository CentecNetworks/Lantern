/**
 @file drv_chip_io.h

 @date 2010-07-23

 @version v5.1

 The file contains all driver I/O interface realization
*/

#ifndef _DRV_CHIP_IO_H
#define _DRV_CHIP_IO_H


#include "kal.h"
#include "dal.h"
#include "drv_enum.h"
#include "drv_common.h"
#include "drv_humber.h"

/**
 @brief Embeded IBM Tcam lkp process
  Only use for the show forwarding tools
*/
extern int32
drv_chip_ibm_tcam_lookup(uint8 chip_id, uint8* key, int32 keysize, bool dual_lkp,
                        uint32* result_index0, uint32* result_index1);

/**
 @brief External NL9k Tcam lkp process
  Only use for the show forwarding tools
*/
extern int32
drv_chip_nl9k_tcam_lookup(uint8 chip_id, uint8* key, int32 keysize, bool dual_lkp,
                          uint32 ltr_id, uint32* result_index0, uint32* result_index1);


/**
 @brief The function write data to a sram memory location for parity error
*/
extern int32
drv_chip_write_sram_for_parity_error(uint8 chip_id, uint32 addr, uint32* data, int32 len);

/**
 @brief The function read data from a sram memory location for parity error
*/
extern int32
drv_chip_sram_read_for_parity_error(uint8 chip_id, uint32 addr, uint32* data, int32 len);

/**
 @brief write table data to a sram memory location on real chip
*/
extern int32
drv_chip_sram_tbl_write(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, uint32* data);


/**
 @brief read table data from a sram memory location on real chip
*/
extern int32
drv_chip_sram_tbl_read(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, uint32* data);

/**
 @brief Real sram indirect operation I/O
*/
int32
drv_chip_indirect_sram_tbl_ioctl(uint8 chip_id, uint32 index,
                                uint32 cmd, void* val);

/**
 @brief write register data to a sram memory location on real chip
*/
extern int32
drv_chip_sram_reg_write(uint8 chip_id, reg_id_t reg_id,
                        uint32 index, uint32* data);


/**
 @brief The function write register data to a sram memory location
*/
extern int32
drv_chip_sram_reg_word_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32 word_offset, uint32* data);

/**
 @brief write register data to a sram memory location on real chip
*/
extern int32
drv_chip_sram_reg_read(uint8 chip_id, reg_id_t reg_id,
                       uint32 index, uint32* data);


/**
 @brief write real tcam interface
*/
extern int32
drv_chip_tcam_tbl_write(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, tbl_entry_t* entry);


/**
 @brief read real tcam interface
*/
extern int32
drv_chip_tcam_tbl_read(uint8 chip_id, tbl_id_t tbl_id,
                       uint32 index, tbl_entry_t *entry);


/**
 @brief remove real tcam entry interface
*/
extern int32
drv_chip_tcam_tbl_remove(uint8 chip_id, tbl_id_t tbl_id, uint32 index);

/**
 @brief write real external tcam's control register interface
*/
extern int32
drv_chip_ext_tcam_reg_write(uint8 chip_id, reg_id_t reg_id, uint32 index, void* ds);


/**
 @brief read real external tcam's control register interface
*/
extern int32
drv_chip_ext_tcam_reg_read(uint8 chip_id, reg_id_t reg_id, uint32 index, void* ds);

/**
 @brief add hash entry after lkp operation on real chip
*/
extern int32
drv_chip_hash_key_add_entry(uint8 chip_id, void* add_para);


/**
 @brief delete hash entry according to detailed key value on real chip
*/
extern int32
drv_chip_hash_key_del_entry_by_key(uint8 chip_id, void* del_para);


/**
 @brief delete hash entry according to hash index on real chip
*/
extern int32
drv_chip_hash_key_del_entry_by_index(uint8 chip_id, void* del_para);

/**
 @brief Hash lookup I/O control API on real chip
*/
extern int32
drv_chip_hash_lookup(uint8 chip_id,
                      uint32* key,
                      hash_ds_ctl_cpu_key_status_t* hash_cpu_status,
                      cpu_req_hash_key_type_e cpu_hashkey_type);


/**
 @brief sram write I/O control API (write data into one address) on real chip
*/
extern int32
drv_chip_write_sram_entry(uint8 chip_id, uint32 addr,
                                uint32* data, int32 len);

/**
 @brief sram read I/O control API (read data from one address) on real chip
*/
extern int32
drv_chip_read_sram_entry(uint8 chip_id, uint32 addr,
                                uint32* data, int32 len);

/**
 @brief tcam mutex init
*/
extern int32
drv_chip_tcam_mutex_init(uint8 chip_id);


/**
 @brief table mutex init
*/
extern int32
drv_chip_tbl_mutex_init(uint8 chip_id);

/**
 @brief register mutex init
*/
extern int32
drv_chip_reg_mutex_init(uint8 chip_id);

#endif

