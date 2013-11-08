/**
 @file drv_io.c

 @date 2010-02-26

 @version v5.1

 The file contains all driver I/O interface realization
*/

#include "drv_cfg.h"
#include "drv_enum.h"
#include "drv_tbl_reg.h"
#include "drv_io.h"
#include "drv_model_io.h"
#include "drv_humber_parity_error.h"

/**********************************************************************************
              Define Gloabal var, Typedef, define and Data Structure
***********************************************************************************/
/**********************************************************************************
                      Function interfaces realization
***********************************************************************************/

/**
 @brief The function is the table I/O control API
*/
int32
drv_tbl_ioctl(uint8 chip_id, int32 index, uint32 cmd, void* val)
{
    int32 action;
    tbl_entry_t entry;
    tbl_id_t tbl_id;
    uint16 field_id;
    uint32 data_entry[MAX_ENTRY_WORD] = {0}, mask_entry[MAX_ENTRY_WORD] = {0};
    tables_t* tbl_ptr = NULL;

    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_PTR_VALID_CHECK(val);

    action = DRV_IOC_OP(cmd);
    tbl_id = DRV_IOC_MEMID(cmd);
    field_id = DRV_IOC_FIELDID(cmd);

        switch (tbl_id)
        {
            /*For this table should call special tbl ioctl*/
        case DS_POLICER:
        case DS_FORWARDING_STATS:
            if (drv_io_api[chip_id].drv_indirect_sram_tbl_ioctl)
            {
                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_indirect_sram_tbl_ioctl(chip_id, index, cmd, val));
                return DRV_E_NONE;
            }
            break;
        default:
            break;
        }

        kal_memset(&entry, 0, sizeof(entry));
        entry.data_entry = data_entry;
        entry.mask_entry = mask_entry;

        DRV_TBL_ID_VALID_CHECK(tbl_id);

        tbl_ptr = DRV_TBL_GET_INFOPTR(tbl_id);

        /* operating all the entry */
        if (DRV_ENTRY_FLAG == field_id)
        {
            switch (action)
            {
            case DRV_IOC_WRITE:
                if (INVALID_MASK_OFFSET == tbl_ptr->hw_mask_base)
                {
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_ds_to_entry(tbl_id, val, data_entry));
                    if (drv_io_api[chip_id].drv_sram_tbl_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_write(chip_id, tbl_id, index, data_entry));
                    }
                }
                else
                {
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_ds_to_entry(tbl_id, val, (tbl_entry_t *)&entry));
                    if (drv_io_api[chip_id].drv_tcam_tbl_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_write(chip_id, tbl_id, index, &entry));
                    }
                }
                break;
            case DRV_IOC_READ:
                if (INVALID_MASK_OFFSET == tbl_ptr->hw_mask_base)
                {
                    if (drv_io_api[chip_id].drv_sram_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_read(chip_id, tbl_id, index, (uint32 *)data_entry));
                    }
                    /* use for co-simulation to store reading tbl infomation in cmodel */
                    //DRV_IF_ERROR_RETURN(store_tbl_ram_info_during_run_cmodel(chip_id, tbl_id, index, data_entry));
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_entry_to_ds(tbl_id, (uint32 *)data_entry, val));
                }
                else
                {
                    if (drv_io_api[chip_id].drv_tcam_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_read(chip_id, tbl_id, index, &entry));
                    }
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_entry_to_ds(tbl_id, (tbl_entry_t *)&entry, val));
                }
                break;
            default:
                break;
            }
        }
        /* operating one field of the entry */
        else
        {
            switch (action)
            {
            case DRV_IOC_WRITE:
                if (INVALID_MASK_OFFSET == tbl_ptr->hw_mask_base)
                {
                    if (drv_io_api[chip_id].drv_sram_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_read(chip_id, tbl_id, index, data_entry));
                    }
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tbl_field_set(tbl_id, field_id, data_entry, *(uint32*)val));
                    if (drv_io_api[chip_id].drv_sram_tbl_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_write(chip_id, tbl_id, index, data_entry));
                    }
                }
                else
                {
                    /*For tcam entry , we could only operate its data's filed, app should not opreate tcam field*/
                    if (drv_io_api[chip_id].drv_tcam_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_read(chip_id, tbl_id, index, &entry));
                    }
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tbl_field_set(tbl_id, field_id, entry.data_entry, *(uint32*)val));
                    if (drv_io_api[chip_id].drv_tcam_tbl_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_write(chip_id, tbl_id, index, &entry));

                    }
                }
                break;
            case DRV_IOC_READ:
                if (INVALID_MASK_OFFSET == tbl_ptr->hw_mask_base)
                {
                    if (drv_io_api[chip_id].drv_sram_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_tbl_read(chip_id, tbl_id, index, data_entry));
                    }
                    /* use for co-simulation to store reading tbl infomation in cmodel */
                    //DRV_IF_ERROR_RETURN(store_tbl_ram_info_during_run_cmodel(chip_id, tbl_id, index, data_entry));
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tbl_field_get(tbl_id, field_id, data_entry, (uint32*)val));
                }
                else
                {
                    /*For tcam entry , we could only operate its data's filed, app should not opreate tcam field*/
                    if (drv_io_api[chip_id].drv_tcam_tbl_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_read(chip_id, tbl_id, index, &entry));
                    }
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tbl_field_get(tbl_id, field_id, (entry.data_entry), (uint32*)val));
                }
                break;
            default:
                break;
            }
        }

   return DRV_E_NONE;
}

/**
 @brief The function is the register I/O control API
*/
int32
drv_reg_ioctl(uint8 chip_id, int32 index, uint32 cmd, void* val)
{
    int32 action;
    uint32 entry[MAX_ENTRY_WORD] = {0};
    int32 size;
    reg_id_t reg_id;
    uint16 field_id;
    uint32 word_offset = 0;

    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_PTR_VALID_CHECK(val);

    action = DRV_IOC_OP(cmd);
    reg_id = DRV_IOC_MEMID(cmd);
    field_id = DRV_IOC_FIELDID(cmd);


        DRV_REG_ID_VALID_CHECK(reg_id);

        size = DRV_REG_ENTRY_SIZE(reg_id);

        kal_memset((uint8*)entry, 0, sizeof(entry));

        if (DRV_ENTRY_FLAG == field_id)
        {
            registers_t* reg_ptr = NULL;
            reg_ptr = DRV_REG_GET_INFOPTR(reg_id);

            switch (action)
            {
            case DRV_IOC_WRITE:
                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_ds_to_entry(reg_id, val, entry));

                /* Write External Tcam's control register */
                if (drv_regs_list[reg_id].hw_data_base >= TCAM_EXT_REG_RAM_OFFSET)
                {
                    if (drv_io_api[chip_id].drv_ext_tcam_reg_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_ext_tcam_reg_write(chip_id, reg_id, index, val));
                        return DRV_E_NONE;
                    }
                }

                if (drv_io_api[chip_id].drv_sram_reg_write)
                {
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_write(chip_id, reg_id, index, entry));
                }
                break;

            case DRV_IOC_READ:
                /* Read External Tcam's control register */
                if (drv_regs_list[reg_id].hw_data_base >= TCAM_EXT_REG_RAM_OFFSET)
                {
                    if (drv_io_api[chip_id].drv_ext_tcam_reg_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_ext_tcam_reg_read(chip_id, reg_id, index, val));
                        return DRV_E_NONE;
                    }
                }

                if (drv_io_api[chip_id].drv_sram_reg_read)
                {
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_read(chip_id, reg_id, index, entry));
                }

                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_entry_to_ds(reg_id, entry, val));
                break;

            default:
                break;
            }
        }
        else
        {
            switch (action)
            {
                case DRV_IOC_WRITE:
                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_reg_field_get_word_offset(reg_id, field_id, &word_offset));

                    if (drv_io_api[chip_id].drv_sram_reg_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_read(chip_id, reg_id, index, entry));
                    }

                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_reg_field_set(reg_id, field_id, entry, *(uint32*)val));

                    if (drv_io_api[chip_id].drv_sram_reg_word_write)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_word_write(chip_id, reg_id, index, word_offset, entry));
                    }
                    break;

                case DRV_IOC_READ:
                    if (drv_io_api[chip_id].drv_sram_reg_read)
                    {
                        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_sram_reg_read(chip_id, reg_id, index, entry));
                    }

                    DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_reg_field_get(reg_id, field_id, entry, (uint32*)val));
                    break;

                default:
                    break;
            }
        }

    return DRV_E_NONE;
}

/**
 @brief remove tcam entry interface according to key id and index
*/
int32
drv_tcam_tbl_remove(uint8 chip_id, int32 tbl_id, uint32 index)
{
    if (drv_io_api[chip_id].drv_tcam_tbl_remove)
    {
        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_tcam_tbl_remove(chip_id, tbl_id, index));
    }
    return DRV_E_NONE;
}

/**
 @brief The function is the hash I/O control API
*/
/*
if delete operation, hash_idx is the appointed delete index or lkp delete index
if write operation, hash_idx is lkp write index
*/
int32
drv_hash_key_ioctl(uint8 chip_id,
                   uint32 tbl_id,
                   uint32 tbl_idx,
                   uint32* key,
                   hash_op_type_e operation_type)
{
    hash_add_para_s add_para;
    hash_del_para_s del_para;
    uint32 data_base=0, start_data_addr=0, entry_size=0;
    uint32 data[8];

    /* check para */
    DRV_CHIP_ID_VALID_CHECK(chip_id);

    if (operation_type >= HASH_OP_TP_MAX_VALUE)
    {
        return DRV_E_INVALID_PARAM;
    }

    switch (operation_type)
    {
        /* Write operation */
        case HASH_OP_TP_ADD_ENTRY:
            kal_memset(&add_para, 0, sizeof(add_para));
            add_para.table_id = tbl_id;
            add_para.table_index = tbl_idx;
            add_para.key = key;
            if (drv_io_api[chip_id].drv_hash_key_add_entry)
            {
                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_hash_key_add_entry(chip_id, (void*)(&add_para)));
            }
            else
            {
                 return DRV_E_INVALID_PARAM;
            }

            /* process parity error */
            data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
            entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);
            start_data_addr = data_base + entry_size * tbl_idx;
            drv_humber_mem_mapping_write(chip_id, tbl_id, start_data_addr, key, entry_size);
            break;
        /* Delete according to lkp result */
        case HASH_OP_TP_DEL_ENTRY_BY_KEY:
            kal_memset(&del_para, 0, sizeof(del_para));
            del_para.table_id = tbl_id;
            del_para.table_index = tbl_idx;
            del_para.key = key;

            if (drv_io_api[chip_id].drv_hash_key_del_entry_by_key)
            {
                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_hash_key_del_entry_by_key(chip_id, (void*)(&del_para)));
            }
            else
            {
                return DRV_E_INVALID_PARAM;
            }
            break;
        /* Delete according to the appointed index */
        case HASH_OP_TP_DEL_ENTRY_BY_INDEX:
            kal_memset(&del_para, 0, sizeof(del_para));
            del_para.table_id = tbl_id;
            del_para.table_index = tbl_idx;
            del_para.key = key;
            if (drv_io_api[chip_id].drv_hash_key_del_entry_by_index)
            {
                DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_hash_key_del_entry_by_index(chip_id, (void*)(&del_para)));
            }
            else
            {
                 return DRV_E_INVALID_PARAM;
            }

            /* process parity error */
            data_base = DRV_TBL_GET_INFO(tbl_id).hw_data_base;
            entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);
            start_data_addr = data_base + entry_size * tbl_idx;
            kal_memset((uint8*)data, 0xFF, sizeof(data));
            drv_humber_mem_mapping_write(chip_id, tbl_id, start_data_addr, data, entry_size);
            break;
        default:
            return DRV_E_INVALID_PARAM;
    }
/* mac & ipv4 v6's hash realize method is different with OAM
Attation:
mac: the DS_MAC_HASH_KEY0 and DS_MAC_HASH_KEY1 tables' address is distant
     cmodel will realize only one table: DS_MAC_HASH_KEY0;
Oam: DS_ETH_OAM_HASH_KEY0 ram is sequential, and DS_ETH_OAM_HASH_KEY1
     is sequential, so cmodel use two table: DS_ETH_OAM_HASH_KEY0 and
     DS_ETH_OAM_HASH_KEY1.
*/
    return DRV_E_NONE;
}

/**
 @brief Hash lookup I/O control API
*/
int32
drv_hash_key_lkup_index(uint8 chip_id,
                        cpu_req_hash_key_type_e cpu_hashkey_type,
                        uint32* key,
                        hash_ds_ctl_cpu_key_status_t* hash_cpu_status)
{
    if (drv_io_api[chip_id].drv_hash_key_lookup)
    {
        DRV_IF_ERROR_RETURN(drv_io_api[chip_id].drv_hash_key_lookup(chip_id, key, hash_cpu_status, cpu_hashkey_type));
    }
    else
    {
        hash_cpu_status->cpu_key_hit = FALSE;
        hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
    }
    return DRV_E_NONE;
}
