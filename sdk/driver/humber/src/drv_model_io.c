/**
 @file drv_model_io.c

 @date 2010-07-23

 @version v5.1

 The file contains all driver I/O interface realization
*/
#if (SDK_WORK_PLATFORM == 1)
#include "drv_model_io.h"
#include "sram_model.h"
#include "tcam_model.h"

extern uint32* int_tcam_data_base[MAX_LOCAL_CHIP_NUM];
extern uint32* int_tcam_mask_base[MAX_LOCAL_CHIP_NUM];
extern uint32* int_tcam_wbit[MAX_LOCAL_CHIP_NUM];
extern uint32* ext_tcam_data_base[MAX_LOCAL_CHIP_NUM];
extern uint32* ext_tcam_mask_base[MAX_LOCAL_CHIP_NUM];
extern uint32* ext_tcam_wbit[MAX_LOCAL_CHIP_NUM];
extern uint32 max_int_tcam_data_base;
extern uint32 max_int_tcam_mask_base;
extern uint32 max_ext_tcam_data_base;
extern uint32 max_ext_tcam_mask_base;
extern uint8* sram_model_reg_wbit[MAX_LOCAL_CHIP_NUM][MAX_REG_NUM];
extern uint8* sram_model_tbl_wbit[MAX_LOCAL_CHIP_NUM][MAX_TBL_NUM];

/* used to record whether the hash entry is written(unit: hash entry 72bits) */
uint8 drv_hash_entry_left_valid[4096] = {0};
uint8 drv_hash_entry_right_valid[4096] = {0};

uint8 drv_hash_entry_ipv4u_left_valid[4096] = {0};
uint8 drv_hash_entry_ipv4u_right_valid[4096] = {0};
uint8 drv_hash_entry_ipv4m_left_valid[4096] = {0};
uint8 drv_hash_entry_ipv4m_right_valid[4096] = {0};

uint8 drv_hash_entry_ipv6u_left_valid[4096] = {0};
uint8 drv_hash_entry_ipv6u_right_valid[4096] = {0};
uint8 drv_hash_entry_ipv6m_left_valid[4096] = {0};
uint8 drv_hash_entry_ipv6m_right_valid[4096] = {0};

extern int16 generate_mac_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_mac_hash1(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv4_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv4_hash1(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv6_hash0(uint8 * sed, uint32 bit_num);
extern int16 generate_ipv6_hash1(uint8 * sed, uint32 bit_num);
extern int8 generate_oam_lkup_hash0(uint8* sed, uint32 bit_num,uint8 crc);
extern int8 generate_oam_lkup_hash1(uint8* sed, uint32 bit_num,uint8 crc);

/**
 @brief sram table model set entry write bit
*/
void
drv_model_sram_tbl_set_wbit(uint8 chip_id, uint32 tbl_id, uint32 index)
{
    uint32 tbl_size = 0;

    tbl_size = DRV_TBL_ENTRY_SIZE(tbl_id);

    if (SRAM_MODEL_IS_NEXTHOP_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_NEXTHOP))
        {
            sram_model_tbl_wbit[chip_id][DS_NEXTHOP][index] = TRUE;
            sram_model_tbl_wbit[chip_id][DS_NEXTHOP][index + 1] = TRUE;
        }
        else
        {
            sram_model_tbl_wbit[chip_id][tbl_id][index] = TRUE;
        }
    }
    else if (SRAM_MODEL_IS_L2EDIT_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_L2_EDIT_ETH4W))
        {
            sram_model_tbl_wbit[chip_id][DS_L2_EDIT_ETH4W][index] = TRUE;
            sram_model_tbl_wbit[chip_id][DS_L2_EDIT_ETH4W][index + 1] = TRUE;
        }
        else
        {
            sram_model_tbl_wbit[chip_id][tbl_id][index] = TRUE;
        }
    }
    else if (SRAM_MODEL_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_L3EDIT_MPLS4W))
        {
            sram_model_tbl_wbit[chip_id][DS_L3EDIT_MPLS4W][index] = TRUE;
            sram_model_tbl_wbit[chip_id][DS_L3EDIT_MPLS4W][index + 1] = TRUE;
        }
        else
        {
            sram_model_tbl_wbit[chip_id][tbl_id][index] = TRUE;
        }
    }
    else
    {
        sram_model_tbl_wbit[chip_id][tbl_id][index] = TRUE;
    }
}

/**
 @brief sram table model get entry write bit
*/
uint8
drv_model_sram_tbl_get_wbit(uint8 chip_id, uint32 tbl_id, uint32 index)
{
    uint32 tbl_size = 0;

    tbl_size = DRV_TBL_ENTRY_SIZE(tbl_id);

    if (SRAM_MODEL_IS_NEXTHOP_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_NEXTHOP))
        {
            return sram_model_tbl_wbit[chip_id][DS_NEXTHOP][index];
        }
        else
        {
            return sram_model_tbl_wbit[chip_id][tbl_id][index];
        }
    }
    else if (SRAM_MODEL_IS_L2EDIT_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_L2_EDIT_ETH4W))
        {
            return sram_model_tbl_wbit[chip_id][DS_L2_EDIT_ETH4W][index];
        }
        else
        {
            return sram_model_tbl_wbit[chip_id][tbl_id][index];
        }
    }
    else if (SRAM_MODEL_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        if (tbl_size != DRV_TBL_ENTRY_SIZE(DS_L3EDIT_MPLS4W))
        {
           return sram_model_tbl_wbit[chip_id][DS_L3EDIT_MPLS4W][index];
        }
        else
        {
            return sram_model_tbl_wbit[chip_id][tbl_id][index];
        }
    }
    else
    {
       return sram_model_tbl_wbit[chip_id][tbl_id][index];
    }
}

/**
 @brief sram register model set entry write bit
*/
void
drv_model_sram_reg_set_wbit(uint8 chip_id, uint32 reg_id, uint32 index)
{
    sram_model_reg_wbit[chip_id][reg_id][index] = TRUE;
}

/**
 @brief sram table model get entry write bit
*/
uint8
drv_model_sram_reg_get_wbit(uint8 chip_id, uint32 reg_id, uint32 index)
{
    return sram_model_reg_wbit[chip_id][reg_id][index];
}


/**
 @brief Real sram direct write operation I/O
*/
int32
drv_model_write_sram_entry(uint8 chip_id, uint32 addr,
                               			 uint32* data, int32 len)
{
    DRV_IF_ERROR_RETURN(sram_model_write(chip_id, addr, data, len));
    return DRV_E_NONE;
}

/**
 @brief Real sram direct read operation I/O
*/
int32
drv_model_read_sram_entry(uint8 chip_id, uint32 addr,
                               			 uint32* data, int32 len)
{
    DRV_IF_ERROR_RETURN(sram_model_read(chip_id, addr, data, len));
    return DRV_E_NONE;
}

/**
 @brief The function write table data to a sram memory location
*/
int32
drv_model_sram_tbl_write(uint8 chip_id, tbl_id_t tbl_id, uint32 index, uint32* data)
{
    #define DYN_SRAM_ENTRY_BYTE 16
    uint32 data_base, start_data_addr, entry_size;
    uint32 max_index_num;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* check if the index num exceed the max index num of the tbl */
    data_base = cmodel_tbls_info[chip_id][tbl_id].sw_data_base;
    entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);
    max_index_num = DRV_TBL_MAX_INDEX(tbl_id);

    if (DRV_SRAM_IS_NEXTHOP_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L2EDIT_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        max_index_num = max_index_num * (entry_size / DYN_SRAM_ENTRY_BYTE);
        start_data_addr = data_base + DYN_SRAM_ENTRY_BYTE * index;
    }
    else
    {
        start_data_addr = data_base + index * entry_size;
    }

    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_tbl): chip-0x%x, tbl-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, tbl_id, index, max_index_num);
        return DRV_E_INVALID_TBL;
    }

    DRV_IF_ERROR_RETURN(drv_model_write_sram_entry(chip_id, start_data_addr, data, entry_size));

    drv_model_sram_tbl_set_wbit(chip_id, tbl_id, index);

    return DRV_E_NONE;
}

/**
 @brief The function read table data from a sram memory location
*/
int32
drv_model_sram_tbl_read(uint8 chip_id, tbl_id_t tbl_id, uint32 index, uint32* data)
{
    #define DYN_SRAM_ENTRY_BYTE 16
    uint32 data_base, start_data_addr, entry_size;
    uint32 max_index_num;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* check if the index num exceed the max index num of the tbl */
    max_index_num = DRV_TBL_MAX_INDEX(tbl_id);
    data_base =  cmodel_tbls_info[chip_id][tbl_id].sw_data_base;
    entry_size = DRV_TBL_ENTRY_SIZE(tbl_id);

    /* According to index value to get the table 's index address in sw */
    if (DRV_SRAM_IS_NEXTHOP_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L2EDIT_SHARE_TBL(tbl_id)
        || DRV_SRAM_IS_L3EDIT_SHARE_TBL(tbl_id))
    {
        max_index_num = max_index_num * (entry_size / DYN_SRAM_ENTRY_BYTE);
        start_data_addr = data_base + DYN_SRAM_ENTRY_BYTE * index;
    }
    else
    {
        start_data_addr = data_base + index * entry_size;
    }

    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_read_sram_tbl): chip-0x%x, tbl-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, tbl_id, index, max_index_num);
        return DRV_E_INVALID_TBL;
    }

    DRV_IF_ERROR_RETURN(drv_model_read_sram_entry(chip_id, start_data_addr, data, entry_size));

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_model_sram_reg_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    uint32 fifo_depth = 0;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
        return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  =  cmodel_regs_info[chip_id][reg_id].sw_data_base + index * bytes;


    DRV_IF_ERROR_RETURN(drv_model_write_sram_entry(chip_id, addr, data, bytes));


    drv_model_sram_reg_set_wbit(chip_id, reg_id, index);

    if (IPE_AGING_FIFO_RAM == reg_id)  /* aging fifo register */
    {
        DRV_IF_ERROR_RETURN(drv_model_sram_reg_read(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        if (fifo_depth < 16) /* AgingDepth is no more than 16(agingFifo's max depth) */
        {
            fifo_depth = fifo_depth + 1;
            DRV_IF_ERROR_RETURN(drv_model_sram_reg_write(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        }
        else if (fifo_depth > 16)
        {
            DRV_DBG_INFO("\nAgingFifo depth exceeds 16 ! The value is %d\n\r", fifo_depth);
        }
    }

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_model_sram_reg_word_write(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32 word_offset, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    uint32 fifo_depth = 0;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
        DRV_DBG_INFO("\nERROR (drv_write_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
        return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  =  cmodel_regs_info[chip_id][reg_id].sw_data_base + index * bytes + word_offset * sizeof(uint32);


    DRV_IF_ERROR_RETURN(drv_model_write_sram_entry(chip_id, addr, &data[word_offset], sizeof(uint32)));


    drv_model_sram_reg_set_wbit(chip_id, reg_id, index);

    if (IPE_AGING_FIFO_RAM == reg_id)  /* aging fifo register */
    {
        DRV_IF_ERROR_RETURN(drv_model_sram_reg_read(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        if (fifo_depth < 16) /* AgingDepth is no more than 16(agingFifo's max depth) */
        {
            fifo_depth = fifo_depth + 1;
            DRV_IF_ERROR_RETURN(drv_model_sram_reg_write(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        }
        else if (fifo_depth > 16)
        {
            DRV_DBG_INFO("\nAgingFifo depth exceeds 16 ! The value is %d\n\r", fifo_depth);
        }
    }

    return DRV_E_NONE;
}

/**
 @brief The function write register data to a sram memory location
*/
int32
drv_model_sram_reg_read(uint8 chip_id, reg_id_t reg_id, uint32 index, uint32* data)
{
    int32 bytes;
    uint32 addr;
    uint32 max_index_num;
    uint32 fifo_depth = 0;

    DRV_PTR_VALID_CHECK(data);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* check if the index num exceed the max index num of the reg */
    max_index_num = DRV_REG_MAX_INDEX(reg_id);
    if (max_index_num <= index)
    {
		DRV_DBG_INFO("\nERROR (drv_read_sram_reg): chip-0x%x, reg-0x%x, index-0x%x exceeds the max_index-0x%x.\n", chip_id, reg_id, index, max_index_num);
		return DRV_E_INVALID_REG;
    }

    bytes = DRV_REG_ENTRY_SIZE(reg_id);
    addr  = cmodel_regs_info[chip_id][reg_id].sw_data_base + index * bytes;


    DRV_IF_ERROR_RETURN(drv_model_read_sram_entry(chip_id, addr, data, bytes));

    if (IPE_AGING_FIFO_RAM == reg_id)  /* aging fifo register */
    {
        DRV_IF_ERROR_RETURN(drv_model_sram_reg_read(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        if ((fifo_depth != 0) && (fifo_depth <= 16)) /* AgingDepth is no more than 16(agingFifo's max depth) */
        {
            fifo_depth = fifo_depth - 1;
            DRV_IF_ERROR_RETURN(drv_model_sram_reg_write(chip_id, IPE_AGING_FIFO_DEPTH, 0, &fifo_depth));
        }
        else if (fifo_depth > 16)
        {
            DRV_DBG_INFO("\nAgingFifo depth exceeds 16 ! The value is %d\n\r", fifo_depth);
        }
    }

    return DRV_E_NONE;
}

/**
 @brief write tcam interface (include operate model and real tcam)
*/
int32
drv_model_tcam_tbl_write(uint8 chip_id, tbl_id_t tbl_id, uint32 index, tbl_entry_t* entry)
{
    #define TCAM_MIN_UNIT_BYTES 16
    uint32* mask = NULL;
    uint32* data = NULL;
    tables_t* tbl = &drv_tbls_list[tbl_id];
    uint32 hw_database_addr = tbl->hw_data_base;
    uint32 hw_maskbase_addr;
    uint32 entry_size = tbl->entry_size;
    uint32 key_size = tbl->key_size;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_num_each_idx, entry_idx, offset;
    uint32 tcam_model_data_base, tcam_model_mask_base;
    uint32 data_addr, mask_addr;
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_PTR_VALID_CHECK(entry);
    DRV_PTR_VALID_CHECK(entry->data_entry);
    DRV_PTR_VALID_CHECK(entry->mask_entry);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    max_int_tcam_data_base_tmp = max_int_tcam_data_base;
    max_ext_tcam_data_base_tmp = max_ext_tcam_data_base;

    hw_maskbase_addr = tbl->hw_mask_base;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)int_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)int_tcam_mask_base[chip_id];
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)ext_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)ext_tcam_mask_base[chip_id];
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
    entry_num_each_idx = key_size/16;
    offset = (entry_size - key_size) / 4;

    data = entry->data_entry + offset;
    mask = entry->mask_entry + offset;

    for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
    {
        data_addr = tcam_model_data_base
                           + (hw_database_addr - tcam_asic_data_base)
                           + (index * entry_num_each_idx + entry_idx) * TCAM_MIN_UNIT_BYTES;
        mask_addr = tcam_model_mask_base
                           + (hw_maskbase_addr - tcam_asic_mask_base)
                           + (index * entry_num_each_idx + entry_idx) * TCAM_MIN_UNIT_BYTES;
        DRV_IF_ERROR_RETURN(tcam_model_write(chip_id, data_addr, data + entry_idx*4, TCAM_MIN_UNIT_BYTES));
        DRV_IF_ERROR_RETURN(tcam_model_write(chip_id, mask_addr, mask + entry_idx*4, TCAM_MIN_UNIT_BYTES));
    }

    return DRV_E_NONE;
}

/**
 @brief read tcam interface (include operate model and real tcam)
*/
int32
drv_model_tcam_tbl_read(uint8 chip_id, tbl_id_t tbl_id, uint32 index, tbl_entry_t *entry)
{
    #define TCAM_MIN_UNIT_BYTES 16
    uint32* mask = NULL;
    uint32* data = NULL;
    tables_t* tbl = &drv_tbls_list[tbl_id];
	uint32 hw_database_addr = tbl->hw_data_base;
	uint32 hw_maskbase_addr;
	uint32 entry_size = tbl->entry_size;
    uint32 key_size = tbl->key_size;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_num_each_idx, entry_idx, offset;
    uint32 tcam_model_data_base, tcam_model_mask_base;
    uint32 data_addr, mask_addr;
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_PTR_VALID_CHECK(entry);
    DRV_PTR_VALID_CHECK(entry->data_entry);
    DRV_PTR_VALID_CHECK(entry->mask_entry);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    max_int_tcam_data_base_tmp = max_int_tcam_data_base;
    max_ext_tcam_data_base_tmp = max_ext_tcam_data_base;
    hw_maskbase_addr = tbl->hw_mask_base;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)int_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)int_tcam_mask_base[chip_id];
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
            && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)ext_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)ext_tcam_mask_base[chip_id];
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
	entry_num_each_idx = key_size/16;
    offset = (entry_size - key_size) / 4;

	data = entry->data_entry + offset;
	mask = entry->mask_entry + offset;

	for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
	{
        data_addr = tcam_model_data_base
                           + (hw_database_addr - tcam_asic_data_base)
                           + (index * entry_num_each_idx + entry_idx) * TCAM_MIN_UNIT_BYTES;
        mask_addr = tcam_model_mask_base
                           + (hw_maskbase_addr - tcam_asic_mask_base)
                           + (index * entry_num_each_idx + entry_idx) * TCAM_MIN_UNIT_BYTES;
        DRV_IF_ERROR_RETURN(tcam_model_read(chip_id, data_addr, data + entry_idx*4, TCAM_MIN_UNIT_BYTES));
        DRV_IF_ERROR_RETURN(tcam_model_read(chip_id, mask_addr, mask + entry_idx*4, TCAM_MIN_UNIT_BYTES));
	}

	return DRV_E_NONE;
}

/**
 @brief remove tcam entry interface (include operate model and real tcam)
*/
int32
drv_model_tcam_tbl_remove(uint8 chip_id, tbl_id_t tbl_id, uint32 index)
{
    #define TCAM_MIN_UNIT_BYTES 16
    tables_t* tbl = NULL;
    uint32 tcam_asic_data_base, tcam_asic_mask_base;
    uint32 entry_idx, entry_num_each_idx;
    uint32 hw_database_addr, key_size;
    uint32 tcam_model_data_base, tcam_model_mask_base;
    uint32 sw_data_addr;
    uint32 max_int_tcam_data_base_tmp;
    uint32 max_ext_tcam_data_base_tmp;

    DRV_CHIP_ID_VALID_CHECK(chip_id);

    if (!DRV_SRAM_IS_TCAM_KEY(tbl_id))
    {
        DRV_DBG_INFO("\nERROR! INVALID Tcam key TblID! TblID: %d, file:%s line:%d function:%s\n",tbl_id,__FILE__,__LINE__,__FUNCTION__);\
        return DRV_E_INVALID_TBL;
    }

    tbl = &drv_tbls_list[tbl_id];
    hw_database_addr = tbl->hw_data_base;
    max_int_tcam_data_base_tmp = max_int_tcam_data_base;
    max_ext_tcam_data_base_tmp = max_ext_tcam_data_base;
    key_size = tbl->key_size;

    if ((hw_database_addr >= DRV_INT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_int_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_INT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_INT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)int_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)int_tcam_mask_base[chip_id];
    }
    else if ((hw_database_addr >= DRV_EXT_TCAM_DATA_ASIC_BASE)
        && (hw_database_addr < max_ext_tcam_data_base_tmp))
    {
        tcam_asic_data_base = DRV_EXT_TCAM_DATA_ASIC_BASE;
        tcam_asic_mask_base = DRV_EXT_TCAM_MASK_ASIC_BASE;
        tcam_model_data_base = (uint32)ext_tcam_data_base[chip_id];
        tcam_model_mask_base = (uint32)ext_tcam_mask_base[chip_id];
    }
    else
    {
        DRV_DBG_INFO("\nTcam key id 0x%x 's database is not correct, please check it!\n", tbl_id);
        return DRV_E_TCAM_KEY_DATA_ADDRESS;
    }

    /* the key's each index includes 80bit entry number */
    entry_num_each_idx = key_size/16;

    /* according to keysize to clear tcam wbit(one bit each 80 bits tcam entry)*/
    for (entry_idx = 0; entry_idx < entry_num_each_idx; ++entry_idx)
    {
        sw_data_addr = tcam_model_data_base
                       + (hw_database_addr - tcam_asic_data_base)
                       + (index * entry_num_each_idx + entry_idx) * TCAM_MIN_UNIT_BYTES;;
        DRV_IF_ERROR_RETURN(tcam_model_remove(chip_id, sw_data_addr));
    }
    return DRV_E_NONE;
}

/**
 @brief model hash valid bit operation (include set and clean)
*/
static int32
_drv_model_hash_valid_bit_operation(cpu_req_hash_key_type_e hash_key_type,
                          uint32 index, hash_op_type_e op_type, uint32 key_size)
{
    uint8* vbit_array_left = NULL;
    uint8* vbit_array_right = NULL;
    uint8* vbit_array_temp = NULL;

    if (op_type >= HASH_OP_TP_MAX_VALUE)
    {
        return DRV_E_INVALID_PARAM;
    }

    if (hash_key_type >= CPU_HASH_KEY_TYPE_RESERVED0)
    {
        return DRV_E_INVALID_PARAM;
    }

    switch (hash_key_type)
    {
        case CPU_HASH_KEY_TYPE_MAC_DA:
        case CPU_HASH_KEY_TYPE_MAC_SA:
            vbit_array_left = drv_hash_entry_left_valid;
            vbit_array_right = drv_hash_entry_right_valid;
            break;
        case CPU_HASH_KEY_TYPE_IPV4_UC:
            vbit_array_left = drv_hash_entry_ipv4u_left_valid;
            vbit_array_right = drv_hash_entry_ipv4u_right_valid;
            break;
        case CPU_HASH_KEY_TYPE_IPV4_MC:
            vbit_array_left = drv_hash_entry_ipv4m_left_valid;
            vbit_array_right = drv_hash_entry_ipv4m_right_valid;
            break;
        case CPU_HASH_KEY_TYPE_IPV6_UC:
            vbit_array_left = drv_hash_entry_ipv6u_left_valid;
            vbit_array_right = drv_hash_entry_ipv6u_right_valid;
            //DRV_DBG_INFO("$$$$$ V6uc TYPE\n");
            break;
        case CPU_HASH_KEY_TYPE_IPV6_MC:
            vbit_array_left = drv_hash_entry_ipv6m_left_valid;
            vbit_array_right = drv_hash_entry_ipv6m_right_valid;
            break;
        default:
            return DRV_E_INVALID_PARAM;
    }

    /* each bucket have 4 * 72 bits, sw use 16 Bytes to sim the 72 bits */
    switch (index%((4 * 16)/key_size))
    {
        case 0:
            vbit_array_temp = vbit_array_left;
            break;
        case 1:
            if (key_size == 16)
            {
                vbit_array_temp = vbit_array_left;
            }
            else
            {
                vbit_array_temp = vbit_array_right;
            }
            break;
        case 2:
        case 3:
            vbit_array_temp = vbit_array_right;
            break;
        default:
            return DRV_E_INVALID_PARAM;
    }

    if (HASH_OP_TP_ADD_ENTRY == op_type)
    {
        if (!IS_BIT_SET(vbit_array_temp[index/8], index%8))
        {
            SET_BIT(vbit_array_temp[index/8], index%8);
            //DRV_DBG_INFO("$$$$$ write vbit addr: 0x%x\n", (uint32)(&vbit_array_temp[index/8]));
            //DRV_DBG_INFO("$$$$$ index: 0x%x\n", index);
        }
    }
    else
    {
        if (IS_BIT_SET(vbit_array_temp[index/8], index%8))
        {
            CLEAR_BIT(vbit_array_temp[index/8], index%8);
            //DRV_DBG_INFO("$$$$$ clear vbit addr: 0x%x\n", (uint32)(&vbit_array_temp[index/8]));
        }
    }

    return DRV_E_NONE;
}


/**
 @brief add hash entry after lkp operation on memory model
*/
int32
drv_model_hash_key_add_entry(uint8 chip_id, void* add_para)
{
    uint32 cmd;
    ds_mac_hash_key0_t* mac_key = NULL;
    ds_ipv4_ucast_hash_key0_t* v4uc_key = NULL;
    ds_ipv4_mcast_hash_key0_t* v4mc_key = NULL;
    ds_ipv6_ucast_hash_key0_t* v6uc_key = NULL;
    ds_ipv6_mcast_hash_key0_t* v6mc_key = NULL;
    ds_eth_oam_hash_key0_t* eth_oam_key0 = NULL;
    ds_eth_oam_hash_key1_t* eth_oam_key1 = NULL;
    ds_pbt_oam_hash_key0_t* pbt_oam_key0 = NULL;
    ds_pbt_oam_hash_key1_t* pbt_oam_key1 = NULL;
    ds_mpls_oam_label_hash_key0_t* mpls_lbl_oam_key0 = NULL;
    ds_mpls_oam_label_hash_key1_t* mpls_lbl_oam_key1 = NULL;
    ds_mpls_oam_ipv4_ttsi_hash_key0_t* mpls_v4_oam_key0 = NULL;
    ds_mpls_oam_ipv4_ttsi_hash_key1_t* mpls_v4_oam_key1 = NULL;
    ds_eth_oam_rmep_hash_key0_t* eth_oam_rmep_key0 = NULL;
    ds_eth_oam_rmep_hash_key1_t* eth_oam_rmep_key1 = NULL;
    hash_add_para_s* para = (hash_add_para_s *)add_para;
    uint32 key_size = drv_tbls_list[para->table_id].entry_size;

    cmd = DRV_IOW(IOC_TABLE, para->table_id, DRV_ENTRY_FLAG);
    switch (para->table_id)
    {
        case DS_MAC_HASH_KEY0:
            mac_key = (ds_mac_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, mac_key));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_MAC_DA, para->table_index, HASH_OP_TP_ADD_ENTRY, key_size));
            break;
        case DS_IPV4_UCAST_HASH_KEY0:
            v4uc_key = (ds_ipv4_ucast_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, v4uc_key));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV4_UC, para->table_index, HASH_OP_TP_ADD_ENTRY, key_size));
            break;
        case DS_IPV4_MCAST_HASH_KEY0:
            v4mc_key = (ds_ipv4_mcast_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, v4mc_key));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV4_MC, para->table_index, HASH_OP_TP_ADD_ENTRY, key_size));
            break;
        case DS_IPV6_UCAST_HASH_KEY0:
            v6uc_key = (ds_ipv6_ucast_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, v6uc_key));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV6_UC, para->table_index, HASH_OP_TP_ADD_ENTRY, key_size));
            break;
        case DS_IPV6_MCAST_HASH_KEY0:
            v6mc_key = (ds_ipv6_mcast_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, v6mc_key));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV6_MC, para->table_index, HASH_OP_TP_ADD_ENTRY, key_size));
            break;
        /* oam need to consider ?? */
        case DS_ETH_OAM_HASH_KEY0:
            eth_oam_key0 = (ds_eth_oam_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, eth_oam_key0));
            break;
        case DS_ETH_OAM_HASH_KEY1:
            eth_oam_key1 = (ds_eth_oam_hash_key1_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, eth_oam_key1));
            break;
        case DS_PBT_OAM_HASH_KEY0:
            pbt_oam_key0 = (ds_pbt_oam_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, pbt_oam_key0));
            break;
        case DS_PBT_OAM_HASH_KEY1:
            pbt_oam_key1 = (ds_pbt_oam_hash_key1_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, pbt_oam_key1));
            break;
        case DS_MPLS_OAM_LABEL_HASH_KEY0:
            mpls_lbl_oam_key0 = (ds_mpls_oam_label_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, mpls_lbl_oam_key0));
            break;
        case DS_MPLS_OAM_LABEL_HASH_KEY1:
            mpls_lbl_oam_key1 = (ds_mpls_oam_label_hash_key1_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, mpls_lbl_oam_key1));
            break;
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0:
            mpls_v4_oam_key0 = (ds_mpls_oam_ipv4_ttsi_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, mpls_v4_oam_key0));
            break;
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1:
            mpls_v4_oam_key1 = (ds_mpls_oam_ipv4_ttsi_hash_key1_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, mpls_v4_oam_key1));
            break;
        case DS_ETH_OAM_RMEP_HASH_KEY0:
            eth_oam_rmep_key0 = (ds_eth_oam_rmep_hash_key0_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, eth_oam_rmep_key0));
            break;
        case DS_ETH_OAM_RMEP_HASH_KEY1:
            eth_oam_rmep_key1 = (ds_eth_oam_rmep_hash_key1_t *)para->key;
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, eth_oam_rmep_key1));
            break;
        default:
            break;
    }

    //sram_model_tbl_wbit[chip_id][para->table_id][para->table_index] = TRUE;
    drv_model_sram_tbl_set_wbit(chip_id, para->table_id, para->table_index);

    return DRV_E_NONE;
}


/**
 @brief delete hash entry on memory model
*/
int32
drv_model_hash_key_del_entry(uint8 chip_id, void* del_para)
{
    uint32 cmd;
    uint32 invalid_mac_hash_value = 0xFFFF;
    uint32 invalid_v4uc_hash_value = 0xFFFF;
    uint32 invalid_v4mc_hash_value = 0xFFFF;
    uint32 invalid_v6uc_hash_value = 0x7;
    uint32 invalid_v6mc_hash_value = 0x7;
    uint32 invalid_oam_hash_entry = TRUE;
    uint32 key_size = 0;
    hash_del_para_s* para = (hash_del_para_s *)del_para;

    key_size = drv_tbls_list[para->table_id].entry_size;
    switch (para->table_id)
    {
        case DS_MAC_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_MAC_HASH_KEY0, DS_MAC_HASH_KEY0_MAPPED_VLANID);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_mac_hash_value));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_MAC_DA, para->table_index, HASH_OP_TP_DEL_ENTRY_BY_KEY, key_size));
            break;
        case DS_IPV4_UCAST_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_UCAST_HASH_KEY0, DS_IPV4_UCAST_HASH_KEY0_KEY_VRFID);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v4uc_hash_value));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV4_UC, para->table_index, HASH_OP_TP_DEL_ENTRY_BY_KEY, key_size));
            break;
        case DS_IPV4_MCAST_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_MCAST_HASH_KEY0, DS_IPV4_MCAST_HASH_KEY0_KEY_VRFID);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v4mc_hash_value));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV4_MC, para->table_index, HASH_OP_TP_DEL_ENTRY_BY_KEY, key_size));
            break;
        case DS_IPV6_UCAST_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DS_IPV6_UCAST_HASH_KEY0_VRFID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6uc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DS_IPV6_UCAST_HASH_KEY0_VRFID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6uc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DS_IPV6_UCAST_HASH_KEY0_VRFID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6uc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DS_IPV6_UCAST_HASH_KEY0_VRFID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6uc_hash_value));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV6_UC, para->table_index, HASH_OP_TP_DEL_ENTRY_BY_KEY, key_size));
            break;
        case DS_IPV6_MCAST_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_MCAST_HASH_KEY0, DS_IPV6_MCAST_HASH_KEY0_VRFID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6mc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_MCAST_HASH_KEY0, DS_IPV6_MCAST_HASH_KEY0_VRFID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6mc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_MCAST_HASH_KEY0, DS_IPV6_MCAST_HASH_KEY0_VRFID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6mc_hash_value));
            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_MCAST_HASH_KEY0, DS_IPV6_MCAST_HASH_KEY0_VRFID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_v6mc_hash_value));
            DRV_IF_ERROR_RETURN(_drv_model_hash_valid_bit_operation(CPU_HASH_KEY_TYPE_IPV6_MC, para->table_index, HASH_OP_TP_DEL_ENTRY_BY_KEY, key_size));
            break;
        case DS_ETH_OAM_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY0, DS_ETH_OAM_HASH_KEY0_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY0, DS_ETH_OAM_HASH_KEY0_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY0, DS_ETH_OAM_HASH_KEY0_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY0, DS_ETH_OAM_HASH_KEY0_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_ETH_OAM_HASH_KEY1:
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY1, DS_ETH_OAM_HASH_KEY1_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY1, DS_ETH_OAM_HASH_KEY1_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY1, DS_ETH_OAM_HASH_KEY1_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_HASH_KEY1, DS_ETH_OAM_HASH_KEY1_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_PBT_OAM_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY0, DS_PBT_OAM_HASH_KEY0_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY0, DS_PBT_OAM_HASH_KEY0_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY0, DS_PBT_OAM_HASH_KEY0_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY0, DS_PBT_OAM_HASH_KEY0_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_PBT_OAM_HASH_KEY1:
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY1, DS_PBT_OAM_HASH_KEY1_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY1, DS_PBT_OAM_HASH_KEY1_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY1, DS_PBT_OAM_HASH_KEY1_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_PBT_OAM_HASH_KEY1, DS_PBT_OAM_HASH_KEY1_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_MPLS_OAM_LABEL_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY0, DS_MPLS_OAM_LABEL_HASH_KEY0_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY0, DS_MPLS_OAM_LABEL_HASH_KEY0_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY0, DS_MPLS_OAM_LABEL_HASH_KEY0_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY0, DS_MPLS_OAM_LABEL_HASH_KEY0_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_MPLS_OAM_LABEL_HASH_KEY1:
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY1, DS_MPLS_OAM_LABEL_HASH_KEY1_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY1, DS_MPLS_OAM_LABEL_HASH_KEY1_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY1, DS_MPLS_OAM_LABEL_HASH_KEY1_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_LABEL_HASH_KEY1, DS_MPLS_OAM_LABEL_HASH_KEY1_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1:
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1, DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_ETH_OAM_RMEP_HASH_KEY0:
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY0, DS_ETH_OAM_RMEP_HASH_KEY0_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY0, DS_ETH_OAM_RMEP_HASH_KEY0_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY0, DS_ETH_OAM_RMEP_HASH_KEY0_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY0, DS_ETH_OAM_RMEP_HASH_KEY0_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        case DS_ETH_OAM_RMEP_HASH_KEY1:
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY1, DS_ETH_OAM_RMEP_HASH_KEY1_INVALID0);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY1, DS_ETH_OAM_RMEP_HASH_KEY1_INVALID1);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY1, DS_ETH_OAM_RMEP_HASH_KEY1_INVALID2);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            cmd = DRV_IOW(IOC_TABLE, DS_ETH_OAM_RMEP_HASH_KEY1, DS_ETH_OAM_RMEP_HASH_KEY1_INVALID3);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, para->table_index, cmd, &invalid_oam_hash_entry));
            break;
        default:
            break;
    }

    sram_model_tbl_wbit[chip_id][para->table_id][para->table_index] = FALSE;

    return DRV_E_NONE;
}

/**
 @brief hash lookup interface on memory model
*/
int32
drv_model_hash_lookup(uint8 chip_id,
                      uint32* key,
                      hash_ds_ctl_cpu_key_status_t* hash_cpu_status,
                      cpu_req_hash_key_type_e cpu_hashkey_type)
{
    hash_ds_ctl_lookup_ctl_t hash_ctl_tmp;
    hash_lkp_ctl_t hash_ctl;
    uint32 bucket_left = 0, bucket_right = 0;
    uint32 index_l0 = 0, index_l1 = 0, index_r0 = 0, index_r1 = 0;
    uint8 hash_key[18] = {0};
    ds_mac_hash_key0_t mac_hash_key;
    ds_ipv4_ucast_hash_key0_t ipv4_uc_hash_key;
    ds_ipv4_mcast_hash_key0_t ipv4_mc_hash_key;
    ds_ipv6_ucast_hash_key0_t ipv6_uc_hash_key;
    ds_ipv6_mcast_hash_key0_t ipv6_mc_hash_key;
    ds_mac_hash_key0_t* mac_key = NULL;
    ds_ipv4_ucast_hash_key0_t* v4uc_key = NULL;
    ds_ipv4_mcast_hash_key0_t* v4mc_key = NULL;
    ds_ipv6_ucast_hash_key0_t* v6uc_key = NULL;
    ds_ipv6_mcast_hash_key0_t* v6mc_key = NULL;
    uint32 cmd;


    if (cpu_hashkey_type >= CPU_HASH_KEY_TYPE_RESERVED0)
    {
        return DRV_E_INVALID_PARAM;
    }

    DRV_PTR_VALID_CHECK(key);
    DRV_PTR_VALID_CHECK(hash_cpu_status);
    DRV_CHIP_ID_VALID_CHECK(chip_id);
    hash_cpu_status->cpu_key_hit = FALSE;
    hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;

    kal_memset(&hash_ctl_tmp, 0, sizeof(hash_ctl_tmp));
    kal_memset(&hash_ctl, 0, sizeof(hash_ctl));
    kal_memset(&mac_hash_key, 0, sizeof(mac_hash_key));
    kal_memset(&ipv4_uc_hash_key, 0, sizeof(ipv4_uc_hash_key));
    kal_memset(&ipv4_mc_hash_key, 0, sizeof(ipv4_mc_hash_key));
    kal_memset(&ipv6_uc_hash_key, 0, sizeof(ipv6_uc_hash_key));
    kal_memset(&ipv6_mc_hash_key, 0, sizeof(ipv6_mc_hash_key));

    if (CPU_HASH_KEY_TYPE_MAC_DA == cpu_hashkey_type)   /*--- mac ---*/
    {
        mac_key = (ds_mac_hash_key0_t *)key;
        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_ctl_tmp));
        hash_ctl.bits_num = hash_ctl_tmp.mac_da_bits_num;
        hash_ctl.tbl_base = hash_ctl_tmp.mac_da_table_base;
        hash_key[7] = (mac_key->mapped_vlanid >> 8) & 0xFF;
        hash_key[6] = mac_key->mapped_vlanid & 0xFF;
        hash_key[5] = (mac_key->mapped_mach>> 8) & 0xFF;
        hash_key[4] = mac_key->mapped_mach & 0xFF;
        hash_key[3] = (mac_key->mapped_macl >> 24) & 0xFF;
        hash_key[2] = (mac_key->mapped_macl >> 16) & 0xFF;
        hash_key[1] = (mac_key->mapped_macl >> 8) & 0xFF;
        hash_key[0] = mac_key->mapped_macl & 0xFF;
		if (drv_io_api[chip_id].generate_mac_hash0 && drv_io_api[chip_id].generate_mac_hash1)
		{
			bucket_left = drv_io_api[chip_id].generate_mac_hash0(hash_key, hash_ctl.bits_num);
			bucket_right = drv_io_api[chip_id].generate_mac_hash1(hash_key, hash_ctl.bits_num);
		}
		else
		{
			hash_cpu_status->cpu_key_hit = FALSE;
			hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
			return DRV_E_NONE;
		}
        index_l0 = (bucket_left << 2) | 0;
        index_l1 = (bucket_left << 2) | 1;
        index_r0 = (bucket_right << 2) | 2;
        index_r1 = (bucket_right << 2) | 3;

        /* check left bucket's first 72 bits entry */
        if (!IS_BIT_SET(drv_hash_entry_left_valid[index_l0/8], index_l0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l0;
            DRV_DBG_INFO("Index_Left0: 0x%X\n", index_l0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l0, cmd, &mac_hash_key));
            if ((mac_hash_key.mapped_mach == mac_key->mapped_mach)
                && (mac_hash_key.mapped_macl == mac_key->mapped_macl)
                && (mac_hash_key.mapped_vlanid != 0xFFFF)
                && (mac_hash_key.mapped_vlanid == mac_key->mapped_vlanid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l0;
                return DRV_E_NONE;
            }
        }

        /* check left bucket's secend 72 bits entry */
        if (!IS_BIT_SET(drv_hash_entry_left_valid[index_l1/8], index_l1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l1;
            DRV_DBG_INFO("Index_Left1: 0x%X\n", index_l1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l1, cmd, &mac_hash_key));
            if ((mac_hash_key.mapped_mach == mac_key->mapped_mach)
                && (mac_hash_key.mapped_macl == mac_key->mapped_macl)
                && (mac_hash_key.mapped_vlanid != 0xFFFF)
                && (mac_hash_key.mapped_vlanid == mac_key->mapped_vlanid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l1;
                return DRV_E_NONE;
            }
        }

        /* check right bucket's first 72 bits entry */
        if (!IS_BIT_SET(drv_hash_entry_right_valid[index_r0/8], index_r0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r0;
            DRV_DBG_INFO("Index_Right0: 0x%X\n", index_r0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r0, cmd, &mac_hash_key));
            if ((mac_hash_key.mapped_mach == mac_key->mapped_mach)
                && (mac_hash_key.mapped_macl == mac_key->mapped_macl)
                && (mac_hash_key.mapped_vlanid != 0xFFFF)
                && (mac_hash_key.mapped_vlanid == mac_key->mapped_vlanid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r0;
                return DRV_E_NONE;
            }
        }

        /* check right bucket's secend 72 bits entry */
        if (!IS_BIT_SET(drv_hash_entry_right_valid[index_r1/8], index_r1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r1;
            DRV_DBG_INFO("Index_Right1: 0x%X\n", index_r1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r1, cmd, &mac_hash_key));
            if ((mac_hash_key.mapped_mach == mac_key->mapped_mach)
                && (mac_hash_key.mapped_macl == mac_key->mapped_macl)
                && (mac_hash_key.mapped_vlanid != 0xFFFF)
                && (mac_hash_key.mapped_vlanid == mac_key->mapped_vlanid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r1;
                return DRV_E_NONE;
            }
        }

        if ((!hash_cpu_status->cpu_key_hit)
            && (DRV_HASH_INVALID_INDEX == hash_cpu_status->cpu_lu_index))     /* hash violate */
        {
            DRV_DBG_INFO("Hash violate!!! So need tcam entry to resolve.\n");
        }
    }
    else if (CPU_HASH_KEY_TYPE_IPV4_UC == cpu_hashkey_type)   /*--- ipv4 ucast ---*/
    {
        /*  table_id = DS_IPV4_UCAST_HASH_KEY0;  ipv4uc hash key */
        if (0 == DRV_TBL_MAX_INDEX(DS_IPV4_UCAST_HASH_KEY0))
        {
            return DRV_E_NONE;
        }

        v4uc_key = (ds_ipv4_ucast_hash_key0_t *)key;
        hash_key[5] = (v4uc_key->key_vrfid >> 8) & 0xFF;
        hash_key[4] = v4uc_key->key_vrfid & 0xFF;
        hash_key[3] = (v4uc_key->key_mapped_ip >> 24) & 0xFF;
        hash_key[2] = (v4uc_key->key_mapped_ip >> 16) & 0xFF;
        hash_key[1] = (v4uc_key->key_mapped_ip >> 8) & 0xFF;
        hash_key[0] = v4uc_key->key_mapped_ip & 0xFF;

        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_ctl_tmp));
        hash_ctl.bits_num = hash_ctl_tmp.ipv4_ucast_bits_num;
        hash_ctl.tbl_base = hash_ctl_tmp.ipv4_ucast_table_base;
		if (drv_io_api[chip_id].generate_ipv4_hash0 && drv_io_api[chip_id].generate_ipv4_hash1)
		{
			bucket_left = drv_io_api[chip_id].generate_ipv4_hash0(hash_key, hash_ctl.bits_num);
			bucket_right = drv_io_api[chip_id].generate_ipv4_hash1(hash_key, hash_ctl.bits_num);
		}
		else
		{
			hash_cpu_status->cpu_key_hit = FALSE;
			hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
			return DRV_E_NONE;
		}
        index_l0 = bucket_left << 2;
        index_l1 = bucket_left << 2 | 1;
        index_r0 = bucket_right << 2 | 2;
        index_r1 = bucket_right << 2 | 3;

        if (!IS_BIT_SET(drv_hash_entry_ipv4u_left_valid[index_l0/8], index_l0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l0;
            DRV_DBG_INFO("Index_Left0: 0x%X\n", index_l0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l0, cmd, &ipv4_uc_hash_key));
            if ((ipv4_uc_hash_key.key_mapped_ip == v4uc_key->key_mapped_ip)
                && (ipv4_uc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_uc_hash_key.key_vrfid == v4uc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4u_left_valid[index_l1/8], index_l1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l1;
            DRV_DBG_INFO("Index_Left1: 0x%X\n", index_l1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l1, cmd, &ipv4_uc_hash_key));
            if ((ipv4_uc_hash_key.key_mapped_ip == v4uc_key->key_mapped_ip)
                && (ipv4_uc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_uc_hash_key.key_vrfid == v4uc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l1;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4u_right_valid[index_r0/8], index_r0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r0;
            DRV_DBG_INFO("Index_Right0: 0x%X\n", index_r0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r0, cmd, &ipv4_uc_hash_key));
            if ((ipv4_uc_hash_key.key_mapped_ip == v4uc_key->key_mapped_ip)
                && (ipv4_uc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_uc_hash_key.key_vrfid == v4uc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4u_right_valid[index_r1/8], index_r1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r1;
            DRV_DBG_INFO("Index_Right1: 0x%X\n", index_r1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r1, cmd, &ipv4_uc_hash_key));
            if ((ipv4_uc_hash_key.key_mapped_ip == v4uc_key->key_mapped_ip)
                && (ipv4_uc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_uc_hash_key.key_vrfid == v4uc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r1;
                return DRV_E_NONE;
            }
        }

        if ((!hash_cpu_status->cpu_key_hit)
            && (DRV_HASH_INVALID_INDEX == hash_cpu_status->cpu_lu_index))     /* hash violate */
        {
            DRV_DBG_INFO("Hash violate!!! So need tcam entry to resolve.\n");
        }
    }
    else if (CPU_HASH_KEY_TYPE_IPV4_MC == cpu_hashkey_type) /*--- ipv4 mcast ---*/
    {
        v4mc_key = (ds_ipv4_mcast_hash_key0_t *)key;
        hash_key[5] = (v4mc_key->key_vrfid >> 8) & 0xFF;
        hash_key[4] = v4mc_key->key_vrfid & 0xFF;
        hash_key[3] = (v4mc_key->key_mapped_ip >> 24) & 0xFF;
        hash_key[2] = (v4mc_key->key_mapped_ip >> 16) & 0xFF;
        hash_key[1] = (v4mc_key->key_mapped_ip >> 8) & 0xFF;
        hash_key[0] = v4mc_key->key_mapped_ip & 0xFF;

        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_ctl_tmp));
        hash_ctl.bits_num = hash_ctl_tmp.ipv4_mcast_bits_num;
        hash_ctl.tbl_base = hash_ctl_tmp.ipv4_mcast_table_base;
		if (drv_io_api[chip_id].generate_ipv4_hash0 && drv_io_api[chip_id].generate_ipv4_hash1)
		{
			bucket_left = drv_io_api[chip_id].generate_ipv4_hash0(hash_key, hash_ctl.bits_num);
			bucket_right = drv_io_api[chip_id].generate_ipv4_hash1(hash_key, hash_ctl.bits_num);
		}
		else
		{
			hash_cpu_status->cpu_key_hit = FALSE;
			hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
			return DRV_E_NONE;
		}
        index_l0 = bucket_left << 2;
        index_l1 = bucket_left << 2 | 1;
        index_r0 = bucket_right << 2 | 2;
        index_r1 = bucket_right << 2 | 3;
        /* here can not lkp the same key twice, the code need to resolve it */
        if (!IS_BIT_SET(drv_hash_entry_ipv4m_left_valid[index_l0/8], index_l0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l0;
            DRV_DBG_INFO("Index_Left0: 0x%X\n", index_l0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_MCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l0, cmd, &ipv4_mc_hash_key));
            if ((ipv4_mc_hash_key.key_mapped_ip == v4mc_key->key_mapped_ip)
                && (ipv4_mc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_mc_hash_key.key_vrfid == v4mc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4m_left_valid[index_l1/8], index_l1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l1;
            DRV_DBG_INFO("Index_Left1: 0x%X\n", index_l1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_MCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l1, cmd, &ipv4_mc_hash_key));
            if ((ipv4_mc_hash_key.key_mapped_ip == v4mc_key->key_mapped_ip)
                && (ipv4_mc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_mc_hash_key.key_vrfid == v4mc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l1;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4m_right_valid[index_r0/8], index_r0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r0;
            DRV_DBG_INFO("Index_Right0: 0x%X\n", index_r0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_MCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r0, cmd, &ipv4_mc_hash_key));
            if ((ipv4_mc_hash_key.key_mapped_ip == v4mc_key->key_mapped_ip)
                && (ipv4_mc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_mc_hash_key.key_vrfid == v4mc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv4m_right_valid[index_r1/8], index_r1 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r1;
            DRV_DBG_INFO("Index_Right1: 0x%X\n", index_r1);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV4_MCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r1, cmd, &ipv4_mc_hash_key));
            if ((ipv4_mc_hash_key.key_mapped_ip == v4mc_key->key_mapped_ip)
                && (ipv4_mc_hash_key.key_vrfid != 0xFFFF)
                && (ipv4_mc_hash_key.key_vrfid == v4mc_key->key_vrfid))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r1;
                return DRV_E_NONE;
            }
        }

        if ((!hash_cpu_status->cpu_key_hit)
            && (DRV_HASH_INVALID_INDEX == hash_cpu_status->cpu_lu_index))     /* hash violate */
        {
            DRV_DBG_INFO("Hash violate!!! So need tcam entry to resolve.\n");
        }
    }
    else if (CPU_HASH_KEY_TYPE_IPV6_UC == cpu_hashkey_type)    /*--- ipv6 ucast ---*/
    {
        /*  table_id = DS_IPV6_UCAST_HASH_KEY0;  ipv6uc hash key */
        if (0 == DRV_TBL_MAX_INDEX(DS_IPV6_UCAST_HASH_KEY0))
        {
            return DRV_E_NONE;
        }

        v6uc_key = (ds_ipv6_ucast_hash_key0_t *)key;
        hash_key[17] = ((v6uc_key->vrfid3&0x7)<<1)|((v6uc_key->vrfid2>>2)&1);
        hash_key[16] = (v6uc_key->vrfid0&0x7)
                       |((v6uc_key->vrfid1&0x7)<<3)
                       |((v6uc_key->vrfid2&0x3)<<6);

        hash_key[15] = (v6uc_key->key_ipda3>>24) & 0xff;
        hash_key[14] = (v6uc_key->key_ipda3>>16) & 0xff;
        hash_key[13] = (v6uc_key->key_ipda3>>8) & 0xff;
        hash_key[12] = v6uc_key->key_ipda3 & 0xff;

        hash_key[11] = (v6uc_key->key_ipda2>>24) & 0xff;
        hash_key[10] = (v6uc_key->key_ipda2>>16) & 0xff;
        hash_key[9] = (v6uc_key->key_ipda2>>8) & 0xff;
        hash_key[8] = v6uc_key->key_ipda2 & 0xff;

        hash_key[7] = (v6uc_key->key_ipda1>>24) & 0xff;
        hash_key[6] = (v6uc_key->key_ipda1>>16) & 0xff;
        hash_key[5] = (v6uc_key->key_ipda1>>8) & 0xff;
        hash_key[4] = v6uc_key->key_ipda1 & 0xff;

        hash_key[3] = (v6uc_key->key_ipda0>>24) & 0xff;
        hash_key[2] = (v6uc_key->key_ipda0>>16) & 0xff;
        hash_key[1] = (v6uc_key->key_ipda0>>8) & 0xff;
        hash_key[0] = v6uc_key->key_ipda0 & 0xff;

        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_ctl_tmp));
        hash_ctl.bits_num = hash_ctl_tmp.ipv6_ucast_bits_num;
        hash_ctl.tbl_base = hash_ctl_tmp.ipv6_ucast_table_base;
		if (drv_io_api[chip_id].generate_ipv6_hash0 && drv_io_api[chip_id].generate_ipv6_hash1)
		{
			bucket_left = drv_io_api[chip_id].generate_ipv6_hash0(hash_key, hash_ctl.bits_num);
			bucket_right = drv_io_api[chip_id].generate_ipv6_hash1(hash_key, hash_ctl.bits_num);
		}
		else
		{
			hash_cpu_status->cpu_key_hit = FALSE;
			hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
			return DRV_E_NONE;
		}
        index_l0 = bucket_left << 1;
        index_r0 = bucket_right << 1 | 1;

        if (!IS_BIT_SET(drv_hash_entry_ipv6u_left_valid[index_l0/8], index_l0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l0;
            DRV_DBG_INFO("V6Uc Index_Left0: 0x%X\n", index_l0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l0, cmd, &ipv6_uc_hash_key));
            if ((ipv6_uc_hash_key.key_ipda0 == v6uc_key->key_ipda0)
                && (ipv6_uc_hash_key.key_ipda1 == v6uc_key->key_ipda1)
                && (ipv6_uc_hash_key.key_ipda2 == v6uc_key->key_ipda2)
                && (ipv6_uc_hash_key.key_ipda3 == v6uc_key->key_ipda3)
                && (ipv6_uc_hash_key.vrfid3 == v6uc_key->vrfid3)
                && (ipv6_uc_hash_key.vrfid2 == v6uc_key->vrfid2)
                && (ipv6_uc_hash_key.vrfid1 == v6uc_key->vrfid1)
                && (ipv6_uc_hash_key.vrfid0 == v6uc_key->vrfid0))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv6u_right_valid[index_r0/8], index_r0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r0;
            DRV_DBG_INFO("V6Uc Index_Right0: 0x%X\n", index_r0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r0, cmd, &ipv6_uc_hash_key));
            if ((ipv6_uc_hash_key.key_ipda0 == v6uc_key->key_ipda0)
                && (ipv6_uc_hash_key.key_ipda1 == v6uc_key->key_ipda1)
                && (ipv6_uc_hash_key.key_ipda2 == v6uc_key->key_ipda2)
                && (ipv6_uc_hash_key.key_ipda3 == v6uc_key->key_ipda3)
                && (ipv6_uc_hash_key.vrfid3 == v6uc_key->vrfid3)
                && (ipv6_uc_hash_key.vrfid2 == v6uc_key->vrfid2)
                && (ipv6_uc_hash_key.vrfid1 == v6uc_key->vrfid1)
                && (ipv6_uc_hash_key.vrfid0 == v6uc_key->vrfid0))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r0;
                return DRV_E_NONE;
            }
        }

        if ((!hash_cpu_status->cpu_key_hit)
            && (DRV_HASH_INVALID_INDEX == hash_cpu_status->cpu_lu_index))     /* hash violate */
        {
            DRV_DBG_INFO("Hash violate!!! So need tcam entry to resolve.\n");
        }
    }
    else                                              /*--- ipv6 mcast ---*/
    {
        v6mc_key = (ds_ipv6_mcast_hash_key0_t *)key;;
        hash_key[17] = ((v6mc_key->vrfid3&0x7)<<1)|((v6mc_key->vrfid2>>2)&1);
        hash_key[16] = (v6mc_key->vrfid0&0x7)
                       |((v6mc_key->vrfid1&0x7)<<3)
                       |((v6mc_key->vrfid2&0x3)<<6);

        hash_key[15] = (v6mc_key->key_ipda3>>24) & 0xff;
        hash_key[14] = (v6mc_key->key_ipda3>>16) & 0xff;
        hash_key[13] = (v6mc_key->key_ipda3>>8) & 0xff;
        hash_key[12] = v6mc_key->key_ipda3 & 0xff;

        hash_key[11] = (v6mc_key->key_ipda2>>24) & 0xff;
        hash_key[10] = (v6mc_key->key_ipda2>>16) & 0xff;
        hash_key[9] = (v6mc_key->key_ipda2>>8) & 0xff;
        hash_key[8] = v6mc_key->key_ipda2 & 0xff;

        hash_key[7] = (v6mc_key->key_ipda1>>24) & 0xff;
        hash_key[6] = (v6mc_key->key_ipda1>>16) & 0xff;
        hash_key[5] = (v6mc_key->key_ipda1>>8) & 0xff;
        hash_key[4] = v6mc_key->key_ipda1 & 0xff;

        hash_key[3] = (v6mc_key->key_ipda0>>24) & 0xff;
        hash_key[2] = (v6mc_key->key_ipda0>>16) & 0xff;
        hash_key[1] = (v6mc_key->key_ipda0>>8) & 0xff;
        hash_key[0] = v6mc_key->key_ipda0 & 0xff;

        cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
        DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_ctl_tmp));
        hash_ctl.bits_num = hash_ctl_tmp.ipv6_mcast_bits_num;
        hash_ctl.tbl_base = hash_ctl_tmp.ipv6_mcast_table_base;
		if (drv_io_api[chip_id].generate_ipv6_hash0 && drv_io_api[chip_id].generate_ipv6_hash1)
		{
			bucket_left = drv_io_api[chip_id].generate_ipv6_hash0(hash_key, hash_ctl.bits_num);
			bucket_right = drv_io_api[chip_id].generate_ipv6_hash1(hash_key, hash_ctl.bits_num);
		}
		else
		{
			hash_cpu_status->cpu_key_hit = FALSE;
			hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
			return DRV_E_NONE;
		}
        index_l0 = bucket_left << 1;
        index_r0 = bucket_right << 1 | 1;

        if (!IS_BIT_SET(drv_hash_entry_ipv6m_left_valid[index_l0/8], index_l0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_l0;
            DRV_DBG_INFO("Index_Left0: 0x%X\n", index_l0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_l0, cmd, &ipv6_mc_hash_key));
            if ((ipv6_mc_hash_key.key_ipda0 == v6mc_key->key_ipda0)
                && (ipv6_mc_hash_key.key_ipda1 == v6mc_key->key_ipda1)
                && (ipv6_mc_hash_key.key_ipda2 == v6mc_key->key_ipda2)
                && (ipv6_mc_hash_key.key_ipda3 == v6mc_key->key_ipda3)
                && (ipv6_mc_hash_key.vrfid3 == v6mc_key->vrfid3)
                && (ipv6_mc_hash_key.vrfid2 == v6mc_key->vrfid2)
                && (ipv6_mc_hash_key.vrfid1 == v6mc_key->vrfid1)
                && (ipv6_mc_hash_key.vrfid0 == v6mc_key->vrfid0))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_l0;
                return DRV_E_NONE;
            }
        }

        if (!IS_BIT_SET(drv_hash_entry_ipv6m_right_valid[index_r0/8], index_r0 % 8))
        {
            hash_cpu_status->cpu_key_hit = FALSE;
            hash_cpu_status->cpu_lu_index = index_r0;
            DRV_DBG_INFO("Index_Right0: 0x%X\n", index_r0);
            return DRV_E_NONE;
        }
        else
        {
            cmd = DRV_IOR(IOC_TABLE, DS_IPV6_UCAST_HASH_KEY0, DRV_ENTRY_FLAG);
            DRV_IF_ERROR_RETURN(drv_tbl_ioctl(chip_id, index_r0, cmd, &ipv6_mc_hash_key));
            if ((ipv6_mc_hash_key.key_ipda0 == v6mc_key->key_ipda0)
                && (ipv6_mc_hash_key.key_ipda1 == v6mc_key->key_ipda1)
                && (ipv6_mc_hash_key.key_ipda2 == v6mc_key->key_ipda2)
                && (ipv6_mc_hash_key.key_ipda3 == v6mc_key->key_ipda3)
                && (ipv6_mc_hash_key.vrfid3 == v6mc_key->vrfid3)
                && (ipv6_mc_hash_key.vrfid2 == v6mc_key->vrfid2)
                && (ipv6_mc_hash_key.vrfid1 == v6mc_key->vrfid1)
                && (ipv6_mc_hash_key.vrfid0 == v6mc_key->vrfid0))
            {
                hash_cpu_status->cpu_key_hit = TRUE;
                hash_cpu_status->cpu_lu_index = index_r0;
                return DRV_E_NONE;
            }
        }

        if ((!hash_cpu_status->cpu_key_hit)
            && (DRV_HASH_INVALID_INDEX == hash_cpu_status->cpu_lu_index))     /* hash violate */
        {
            DRV_DBG_INFO("Hash violate!!! So need tcam entry to resolve.\n");
        }
    }

    return DRV_E_NONE;
}
#endif

