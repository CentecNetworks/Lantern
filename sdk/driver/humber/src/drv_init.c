#include "drv_tbl_reg.h"
#include "drv_io.h"
#include "drv_model_io.h"
#include "drv_chip_io.h"

/* use for testing cpu endian */
struct endian_test_e
{
    uint32 test1    :1;
    uint32 test2    :31;
};
typedef struct endian_test_e endian_test_t;

/* Record Driver init chip number */
uint8 drv_init_chip_num;

/*Record Driver init external chip id*/
uint8 current_external_chip_id = 0xff;

/* driver IO callback function */
drv_io_callback_fun_t drv_io_api[MAX_LOCAL_CHIP_NUM];

drv_work_platform_type_t g_plaform_type;
drv_work_platform_type_t g_workenv_type;
static bool drv_init_flag;

extern int16 generate_mac_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_mac_hash1(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv4_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv4_hash1(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv6_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv6_hash1(uint8 * sed, uint32 bit_num);
extern int8 generate_oam_lkup_hash0(uint8* sed, uint32 bit_num,uint8 crc);
extern int8 generate_oam_lkup_hash1(uint8* sed, uint32 bit_num,uint8 crc);

/**
 @brief init chip number and operation type in driver
*/
static int32
_drv_init_chipnum(uint8 chip_num)
{
    drv_init_chip_num = chip_num;
    return DRV_E_NONE;
}

/**
 @brief Install driver I/O API
*/
static int32
_drv_install_io_api(drv_work_platform_type_t  platform_type, uint8 chip_num)
{
    uint8 chip_id = 0;

    for (chip_id = 0; chip_id < chip_num; chip_id++)
    {
        drv_io_api[chip_id].drv_sram_tbl_ds_to_entry = drv_sram_tbl_ds_to_entry;
        drv_io_api[chip_id].drv_tcam_tbl_ds_to_entry = drv_tcam_tbl_ds_to_entry;
        drv_io_api[chip_id].drv_sram_tbl_entry_to_ds = drv_sram_tbl_entry_to_ds;
        drv_io_api[chip_id].drv_tcam_tbl_entry_to_ds = drv_tcam_tbl_entry_to_ds;
        drv_io_api[chip_id].drv_tbl_field_set = drv_tbl_field_set;
        drv_io_api[chip_id].drv_tbl_field_get = drv_tbl_field_get;
        drv_io_api[chip_id].drv_sram_reg_ds_to_entry = drv_sram_reg_ds_to_entry;
        drv_io_api[chip_id].drv_sram_reg_entry_to_ds = drv_sram_reg_entry_to_ds;
        drv_io_api[chip_id].drv_reg_field_set = drv_reg_field_set;
        drv_io_api[chip_id].drv_reg_field_get = drv_reg_field_get;
        drv_io_api[chip_id].drv_reg_field_get_word_offset = drv_reg_field_get_word_offset;

        if (platform_type == HW_PLATFORM)   /* Operation real chip */
        {
            /* Sram operation I/O interface (according to table's index) */
            drv_io_api[chip_id].drv_sram_reg_read = &drv_chip_sram_reg_read;
            drv_io_api[chip_id].drv_sram_reg_write = &drv_chip_sram_reg_write;
            drv_io_api[chip_id].drv_sram_reg_word_write = &drv_chip_sram_reg_word_write;
            drv_io_api[chip_id].drv_sram_tbl_read = &drv_chip_sram_tbl_read;
            drv_io_api[chip_id].drv_sram_tbl_write = &drv_chip_sram_tbl_write;

            /* Sram operation I/O interface (according to address) */
            drv_io_api[chip_id].drv_sram_read_entry = &drv_chip_read_sram_entry;
            drv_io_api[chip_id].drv_sram_write_entry = &drv_chip_write_sram_entry;

            /* Sram indirect operation I/O interface */
            drv_io_api[chip_id].drv_indirect_sram_tbl_ioctl = &drv_chip_indirect_sram_tbl_ioctl;

            /* Tcam operation I/O interface */
            drv_io_api[chip_id].drv_tcam_tbl_read = &drv_chip_tcam_tbl_read;
            drv_io_api[chip_id].drv_tcam_tbl_write = &drv_chip_tcam_tbl_write;
            drv_io_api[chip_id].drv_tcam_tbl_remove = &drv_chip_tcam_tbl_remove;
            drv_io_api[chip_id].drv_ext_tcam_reg_read = &drv_chip_ext_tcam_reg_read;
            drv_io_api[chip_id].drv_ext_tcam_reg_write = &drv_chip_ext_tcam_reg_write;

            /* Hash operation I/O interface */
            drv_io_api[chip_id].drv_hash_key_add_entry = &drv_chip_hash_key_add_entry;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_key = &drv_chip_hash_key_del_entry_by_key;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_index = &drv_chip_hash_key_del_entry_by_index;
            drv_io_api[chip_id].drv_hash_key_lookup = &drv_chip_hash_lookup;

            /* hash */
            drv_io_api[chip_id].generate_mac_hash0 = NULL;
            drv_io_api[chip_id].generate_mac_hash0 = NULL;
            drv_io_api[chip_id].generate_ipv4_hash0 = NULL;
            drv_io_api[chip_id].generate_ipv4_hash1 = NULL;
            drv_io_api[chip_id].generate_ipv6_hash0 = NULL;
            drv_io_api[chip_id].generate_ipv6_hash1 = NULL;
            drv_io_api[chip_id].generate_oam_lkup_hash0 = NULL;
            drv_io_api[chip_id].generate_oam_lkup_hash1 = NULL;

        }
        else if (platform_type == SW_SIM_PLATFORM)/* Operation memory model */
        {
#if (SDK_WORK_PLATFORM == 1 )
            /* Sram operation I/O interface */
            drv_io_api[chip_id].drv_sram_reg_read = &drv_model_sram_reg_read;
            drv_io_api[chip_id].drv_sram_reg_write = &drv_model_sram_reg_write;
            drv_io_api[chip_id].drv_sram_reg_word_write = &drv_model_sram_reg_word_write;
            drv_io_api[chip_id].drv_sram_tbl_read = &drv_model_sram_tbl_read;
            drv_io_api[chip_id].drv_sram_tbl_write = &drv_model_sram_tbl_write;

            /* Sram operation I/O interface (according to address) */
            drv_io_api[chip_id].drv_sram_read_entry = &drv_model_read_sram_entry;
            drv_io_api[chip_id].drv_sram_write_entry = &drv_model_write_sram_entry;

            drv_io_api[chip_id].drv_indirect_sram_tbl_ioctl = NULL;

            /* Tcam operation I/O interface */
            drv_io_api[chip_id].drv_tcam_tbl_read = &drv_model_tcam_tbl_read;
            drv_io_api[chip_id].drv_tcam_tbl_write = &drv_model_tcam_tbl_write;
            drv_io_api[chip_id].drv_tcam_tbl_remove = &drv_model_tcam_tbl_remove;
            drv_io_api[chip_id].drv_ext_tcam_reg_read = NULL;
            drv_io_api[chip_id].drv_ext_tcam_reg_write = NULL;

#if (SDK_WORK_ENV == 1)
            /* Hash operation I/O interface */
            drv_io_api[chip_id].drv_hash_key_add_entry = &drv_model_hash_key_add_entry;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_key = &drv_model_hash_key_del_entry;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_index = &drv_model_hash_key_del_entry;
            drv_io_api[chip_id].drv_hash_key_lookup = &drv_model_hash_lookup;

            /* hash */
            drv_io_api[chip_id].generate_mac_hash0 = generate_mac_hash0;
            drv_io_api[chip_id].generate_mac_hash1 = generate_mac_hash1;
            drv_io_api[chip_id].generate_ipv4_hash0 = generate_ipv4_hash0;
            drv_io_api[chip_id].generate_ipv4_hash1 = generate_ipv4_hash1;
            drv_io_api[chip_id].generate_ipv6_hash0 = generate_ipv6_hash0;
            drv_io_api[chip_id].generate_ipv6_hash1 = generate_ipv6_hash1;
            drv_io_api[chip_id].generate_oam_lkup_hash0 = generate_oam_lkup_hash0;
            drv_io_api[chip_id].generate_oam_lkup_hash1 = generate_oam_lkup_hash1;
#else
                /* Hash operation I/O interface */
            drv_io_api[chip_id].drv_hash_key_add_entry = NULL;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_key = NULL;
            drv_io_api[chip_id].drv_hash_key_del_entry_by_index = NULL;
            drv_io_api[chip_id].drv_hash_key_lookup = NULL;

            /* hash */
            drv_io_api[chip_id].generate_mac_hash0 = NULL;
            drv_io_api[chip_id].generate_mac_hash1 = NULL;
            drv_io_api[chip_id].generate_ipv4_hash0 = NULL;
            drv_io_api[chip_id].generate_ipv4_hash1 = NULL;
            drv_io_api[chip_id].generate_ipv6_hash0 = NULL;
            drv_io_api[chip_id].generate_ipv6_hash1 = NULL;
            drv_io_api[chip_id].generate_oam_lkup_hash0 = NULL;
            drv_io_api[chip_id].generate_oam_lkup_hash1 = NULL;

#endif
#endif

        }
        else
        {
            return DRV_E_INVALD_RUNNING_ENV_TYPE;
        }
    }


    return DRV_E_NONE;
}

static int32
_drv_mem_usrctrl_init(drv_work_platform_type_t platform_type)
{
    uint8 chip_id = 0;

    if (platform_type == HW_PLATFORM)
    {
        for (chip_id = 0; chip_id < drv_init_chip_num; chip_id++)
        {
            /* initialize tcam I/O mutex */
            DRV_IF_ERROR_RETURN(drv_chip_tcam_mutex_init(chip_id));

            /* initialize tbl sram I/O mutex */
            DRV_IF_ERROR_RETURN(drv_chip_tbl_mutex_init(chip_id));

            /* initialize reg sram I/O mutex */
            DRV_IF_ERROR_RETURN(drv_chip_reg_mutex_init(chip_id));
        }
    }
    else
    {
        return DRV_E_NONE;
    }

    return DRV_E_NONE;
}


/**
  @driver init interface
*/
int32 drv_init(uint8 chip_num)
{
    if (drv_init_flag)
    {
        return DRV_E_NONE;
    }

    g_plaform_type = SDK_WORK_PLATFORM;
    g_workenv_type = SDK_WORK_ENV;

    _drv_init_chipnum(chip_num);
    DRV_IF_ERROR_RETURN(_drv_install_io_api(g_plaform_type, chip_num));
    DRV_IF_ERROR_RETURN(drv_humber_register());
    DRV_IF_ERROR_RETURN(_drv_mem_usrctrl_init(g_plaform_type));

    drv_init_flag = 1;

    return DRV_E_NONE;
}

/**
  @driver get chip number interface
*/
int32 drv_get_chipnum(uint8 *chipnum)
{
    if (drv_init_flag)
    {
        *chipnum = drv_init_chip_num;
        return DRV_E_NONE;
    }
    else
    {
        DRV_DBG_INFO("@@@ERROR, Humber driver is not initialized!\n");
        return DRV_E_INVALID_CHIP;
    }
}

/**
  @driver get table register infomation interface
*/
int32 drv_get_tbl_info(uint32 tbl_id, tables_t *tbl_info_ptr)
{
    DRV_TBL_ID_VALID_CHECK(tbl_id);
    DRV_PTR_VALID_CHECK(tbl_info_ptr);
    if ((&(drv_tbls_list[tbl_id]))!= NULL)
    {
        kal_memcpy(tbl_info_ptr, &(drv_tbls_list[tbl_id]), sizeof(tables_t));
    }
    else
    {
        DRV_DBG_INFO("@@@ERROR, Humber driver does not register tbl 0x%x !\n", tbl_id);
        return DRV_E_INVALID_TBL;
    }

    return DRV_E_NONE;
}

/**
  @driver get reg register infomation interface
*/
int32 drv_get_reg_info(uint32 reg_id, registers_t *reg_info_ptr)
{
    DRV_REG_ID_VALID_CHECK(reg_id);
    DRV_PTR_VALID_CHECK(reg_info_ptr);
    if ((&(drv_regs_list[reg_id]))!= NULL)
    {
        kal_memcpy(reg_info_ptr, &(drv_regs_list[reg_id]), sizeof(registers_t));
    }
    else
    {
        DRV_DBG_INFO("@@@ERROR, Humber driver does not register reg 0x%x !\n", reg_id);
        return DRV_E_INVALID_REG;
    }

    return DRV_E_NONE;
}

/**
  @driver get environmnet type interface
*/
int32 drv_get_platform_type(drv_work_platform_type_t *plaform_type)
{
    if (drv_init_flag)
    {
       *plaform_type = g_plaform_type;
        return DRV_E_NONE;
    }
    else
    {
        DRV_DBG_INFO("@@@ERROR, Humber driver is not initialized!\n");
        return DRV_E_INVALID_PARAM;
    }
}

int32
drv_get_workenv_type(drv_work_env_type_t *workenv_type)
{
  if (drv_init_flag)
    {
       *workenv_type = g_workenv_type;
        return DRV_E_NONE;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

  return DRV_E_NONE;
}

int32
drv_set_external_chip_id(uint8 chip_id)
{
    current_external_chip_id = chip_id;
    return DRV_E_NONE;
}

int32
drv_get_external_chip_id(void)
{
    return current_external_chip_id;
}

bool
drv_is_external_chip_supported(void)
{
    if(current_external_chip_id != 0xff)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
