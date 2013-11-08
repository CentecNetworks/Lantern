/**
 @file drv_error.h

 @date 2010-02-25

 @version v5.1

 The file contains all tables and regs' struct declaration
*/

#ifndef _DRV_HUMBER_H_
#define _DRV_HUMBER_H_

#define C_MODEL_BASE 0


#if (HOST_IS_LE == 1)
#include "drv_humber_le.h"
#endif

#if (HOST_IS_LE == 0)
#include "drv_humber_be.h"
#endif

#include "drv_enum.h"
#include "drv_tbl_reg.h"

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/

/**
 @brief define the byte order
*/
enum host_type_e
{
    HOST_LE = 0,     /**< little edian */
    HOST_BE = 1      /**< big edian */
};
typedef enum host_type_e host_type_t;

/* Define I/O operation's memory type */
enum ioc_mem_type
{
    IOC_REG,    /**< register */
    IOC_TABLE   /**< table */
};

/* Define cpu req hash key type */
enum cpu_req_hash_key_type
{
    CPU_HASH_KEY_TYPE_MAC_DA = 0,
    CPU_HASH_KEY_TYPE_MAC_SA = 1,
    CPU_HASH_KEY_TYPE_IPV4_UC = 2,
    CPU_HASH_KEY_TYPE_IPV4_MC = 3,
    CPU_HASH_KEY_TYPE_IPV6_UC = 4,
    CPU_HASH_KEY_TYPE_IPV6_MC = 5,
    CPU_HASH_KEY_TYPE_RESERVED0 = 6,
    CPU_HASH_KEY_TYPE_RESERVED1 = 7
};
typedef enum cpu_req_hash_key_type cpu_req_hash_key_type_e;

/* Hash add/delete operation type */
enum hash_op_type
{
    HASH_OP_TP_ADD_ENTRY = 0,
    HASH_OP_TP_DEL_ENTRY_BY_KEY = 1,
    HASH_OP_TP_DEL_ENTRY_BY_INDEX = 2,
    HASH_OP_TP_MAX_VALUE = 3
};
typedef enum hash_op_type hash_op_type_e;

/**
*/
enum drv_work_env_type_e
{
    NO_CMODEL = 0,
    WITH_CMODEL = 1,
    MAX_ENV_OP_TYPE = 2
};
typedef enum drv_work_env_type_e drv_work_env_type_t;

enum drv_work_platform_type_e
{
    HW_PLATFORM = 0,
    SW_SIM_PLATFORM = 1,
    MAX_WORK_PLATFORM = 2
};
typedef enum drv_work_platform_type_e drv_work_platform_type_t;


/* driver io callback function set struct */
struct drv_io_callback_fun_s
{
    int32(*drv_sram_reg_read)(uint8, reg_id_t, uint32, uint32*);
    int32(*drv_sram_reg_write)(uint8, reg_id_t, uint32, uint32*);
    int32(*drv_sram_reg_word_write)(uint8, reg_id_t, uint32, uint32, uint32*);
    int32(*drv_sram_tbl_read)(uint8, tbl_id_t, uint32, uint32*);
    int32(*drv_sram_tbl_write)(uint8, tbl_id_t, uint32, uint32*);

    int32(*drv_indirect_sram_tbl_ioctl)(uint8, uint32, uint32, void*);

    int32(*drv_tcam_tbl_read)(uint8, tbl_id_t, uint32, tbl_entry_t*);
    int32(*drv_tcam_tbl_write)(uint8, tbl_id_t, uint32, tbl_entry_t*);
    int32(*drv_tcam_tbl_remove)(uint8, tbl_id_t, uint32);
    int32(*drv_ext_tcam_reg_read)(uint8, reg_id_t, uint32, void*);
    int32(*drv_ext_tcam_reg_write)(uint8, reg_id_t, uint32, void*);

    int32(*drv_hash_key_add_entry)(uint8, void*);
    int32(*drv_hash_key_del_entry_by_key)(uint8, void*);
    int32(*drv_hash_key_del_entry_by_index)(uint8, void*);
    int32(*drv_hash_key_lookup)(uint8, uint32*, hash_ds_ctl_cpu_key_status_t*, cpu_req_hash_key_type_e);

    int32(*drv_sram_read_entry)(uint8, uint32, uint32*, int32);
    int32(*drv_sram_write_entry)(uint8, uint32, uint32*, int32);

    int32 (*drv_sram_tbl_ds_to_entry)(tbl_id_t, void*, void*);
    int32 (*drv_tcam_tbl_ds_to_entry)(tbl_id_t, void*, void*);
    int32 (*drv_sram_tbl_entry_to_ds)(tbl_id_t, void*, void*);
    int32 (*drv_tcam_tbl_entry_to_ds)(tbl_id_t, void*, void*);
    int32 (*drv_tbl_field_set)(tbl_id_t, fld_id_t, uint32*, uint32);
    int32 (*drv_tbl_field_get)(tbl_id_t, fld_id_t, uint32* , uint32*);
    int32 (*drv_sram_reg_ds_to_entry)(reg_id_t, void*, uint32*);
    int32 (*drv_sram_reg_entry_to_ds)(reg_id_t, uint32*, void*);
    int32 (*drv_reg_field_set)(reg_id_t, fld_id_t, uint32*, uint32);
    int32 (*drv_reg_field_get)(reg_id_t, fld_id_t, uint32*, uint32*);
    int32 (*drv_reg_field_get_word_offset)(reg_id_t, fld_id_t, uint32*);

    int16(*generate_mac_hash0)(uint8*, uint32);
    int16(*generate_mac_hash1)(uint8*, uint32);
    int16(*generate_ipv4_hash0)(uint8*, uint32);
    int16(*generate_ipv4_hash1)(uint8*, uint32);
    int16(*generate_ipv6_hash0)(uint8*, uint32);
    int16(*generate_ipv6_hash1)(uint8*, uint32);
    int8(*generate_bheader_crc8)(uint8*);
    int8(*generate_oam_lkup_hash0)(uint8*, uint32, uint8);
    int8(*generate_oam_lkup_hash1)(uint8*, uint32, uint8);
};
typedef struct drv_io_callback_fun_s drv_io_callback_fun_t;


/**
  @define external use Vars
*/
extern drv_io_callback_fun_t drv_io_api[MAX_LOCAL_CHIP_NUM];

/**
  @define driver init interface
*/
extern int32
drv_init(uint8 chip_num);

/**
  @define get chip number interface
*/
extern int32
drv_get_chipnum(uint8 *chipnum);

/**
  @driver get table register infomation interface
*/
extern int32
drv_get_tbl_info(uint32 tbl_id, tables_t *tbl_info_ptr);

/**
  @driver get reg register infomation interface
*/
extern int32
drv_get_reg_info(uint32 reg_id, registers_t *reg_info_ptr);

/**
  @driver g get driver work  platform  type
*/
extern int32
drv_get_platform_type(drv_work_platform_type_t *platform_type);


/**
  @driver get driver work  environmnet type
*/
extern int32
drv_get_workenv_type(drv_work_env_type_t *workenv_type);

/**
 @brief humber driver register process
*/
extern int32
drv_humber_register(void);

/**
 @brief set external chip id
*/
extern int32
drv_set_external_chip_id(uint8 chip_id);

/**
 @brief set external chip id
*/
extern int32
drv_get_external_chip_id(void);

/**
 @brief get if external chip is supported
*/
extern bool
drv_is_external_chip_supported(void);


#endif /*end of _DRV_HUMBER_H_*/

