/**
  @file drv_model_io.h

  @date 2010-07-23

  @version v5.1

  The file implement driver IOCTL defines and macros
*/
#ifndef _DRV_MODEL_IO_H_
#define _DRV_MODEL_IO_H_

#include "kal.h"
#include "drv_enum.h"
#include "drv_common.h"
#include "drv_humber.h"
#include "drv_io.h"

struct hash_add_para_t
{
    uint32 table_id;
    uint32 table_index;
    uint32* key;
};
typedef struct hash_add_para_t hash_add_para_s;

struct hash_del_para_t
{
    uint32 table_id;
    uint32 table_index;
    uint32* key;
};
typedef struct hash_del_para_t hash_del_para_s;

struct hash_lkp_ctl_s
{
    uint32 rsv1                   :13;
    uint32 bits_num            :3;
    uint32 rsv2                     :9;
    uint32 tbl_base             :7;
};
typedef struct hash_lkp_ctl_s hash_lkp_ctl_t;

/**
 @brief set the table entry's write bit
*/
extern void
drv_model_sram_tbl_set_wbit(uint8 chip_id, uint32 tbl_id, uint32 index);

/**
 @brief get the table entry's write bit
*/
extern uint8
drv_model_sram_tbl_get_wbit(uint8 chip_id, uint32 tbl_id, uint32 index);

/**
 @brief set the register entry's write bit
*/
extern void
drv_model_sram_reg_set_wbit(uint8 chip_id, uint32 reg_id, uint32 index);

/**
 @brief get the register entry's write bit
*/
extern uint8
drv_model_sram_reg_get_wbit(uint8 chip_id, uint32 reg_id, uint32 index);

/**
 @brief write table data to a sram memory location on memory model(according to one address)
*/
int32
drv_model_write_sram_entry(uint8 chip_id, uint32 addr,
                                         uint32* data, int32 len);
/**
 @brief read table data to a sram memory location on memory model(according to one address)
*/
int32
drv_model_read_sram_entry(uint8 chip_id, uint32 addr,
                                         uint32* data, int32 len);

/**
 @brief write table data to a sram memory location on memory model
*/
extern int32
drv_model_sram_tbl_write(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, uint32* data);


/**
 @brief read table data from a sram memory location on memory model
*/
extern int32
drv_model_sram_tbl_read(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, uint32* data);


/**
 @brief write register data to a sram memory location on memory model
*/
extern int32
drv_model_sram_reg_write(uint8 chip_id, reg_id_t reg_id,
                        uint32 index, uint32* data);

/**
 @brief The function write register data to a sram memory location
*/
extern int32
drv_model_sram_reg_word_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32 word_offset, uint32* data);

/**
 @brief write register data to a sram memory location on memory model
*/
extern int32
drv_model_sram_reg_read(uint8 chip_id, reg_id_t reg_id,
                       uint32 index, uint32* data);


/**
 @brief write tcam interface on memory model
*/
extern int32
drv_model_tcam_tbl_write(uint8 chip_id, tbl_id_t tbl_id,
                        uint32 index, tbl_entry_t* entry);


/**
 @brief read tcam interface on memory model
*/
extern int32
drv_model_tcam_tbl_read(uint8 chip_id, tbl_id_t tbl_id,
                       uint32 index, tbl_entry_t *entry);


/**
 @brief remove tcam entry interface on memory model
*/
extern int32
drv_model_tcam_tbl_remove(uint8 chip_id, tbl_id_t tbl_id, uint32 index);


/**
 @brief add hash entry after lkp operation on memory model
*/
extern int32
drv_model_hash_key_add_entry(uint8 chip_id, void* add_para);


/**
 @brief delete hash entry on memory model
*/
extern int32
drv_model_hash_key_del_entry(uint8 chip_id, void* del_para);

/**
 @brief hash lookup interface on memory model
*/
extern int32
drv_model_hash_lookup(uint8 chip_id,
                      uint32* key,
                      hash_ds_ctl_cpu_key_status_t* hash_cpu_status,
                      cpu_req_hash_key_type_e cpu_hashkey_type);


extern uint8
drv_model_sram_reg_get_wbit(uint8 chip_id, uint32 reg_id, uint32 index);



extern uint8
drv_model_sram_tbl_get_wbit(uint8 chip_id, uint32 tbl_id, uint32 index);

extern void
drv_sram_tbl_set_wbit(uint8 chip_id, uint32 tbl_id, uint32 index);

#endif
