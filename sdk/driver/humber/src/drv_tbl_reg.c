/**
  @file drv_tbl_reg.c

  @date 2010-07-22

  @version v5.1

  The file implement driver IOCTL defines and macros
*/

#include "drv_tbl_reg.h"
#include "drv_humber.h"
#include "drv_io.h"
#include "drv_cfg.h"
/**********************************************************************************
              Define Global declarations, Typedef, define and Data Structure
***********************************************************************************/
extern uint8 current_local_chip_num;
extern uint8 current_chip_type;

/* check memory allocation use */
enum tcam_key_type_e
{
    MAC_KEY = 0,
    ACL_MAC_KEY = 1,
    ACL_IPV4_KEY = 2,
    ACL_IPV6_KEY = 3,
    QOS_MAC_KEY = 4,
    QOS_IPV4_KEY = 5,
    QOS_IPV6_KEY = 6,
    IPV4_UCAST_ROUTE_KEY = 7,
    IPV4_MCAST_ROUTE_KEY = 8,
    IPV6_UCAST_ROUTE_KEY = 9,
    IPV6_MCAST_ROUTE_KEY = 10,
    IPV4_NAT_KEY = 11,
    IPV4_PBR_DUALDA_KEY = 12,
    USER_ID_IPV4_KEY = 13,
    USER_ID_IPV6_KEY = 14,
    USER_ID_MAC_KEY = 15,
    USER_ID_VLAN_KEY = 16,
    IPV6_NAT_KEY = 17,
    IPV6_PBR_DUALDA_KEY = 18,
    ETH_OAM_KEY = 19,
    ETH_OAM_RMEP_KEY = 20,
    MPLS_OAM_LABEL_KEY = 21,
    MPLS_OAM_IPV4_TTSI_KEY = 22,
    PBT_OAM_KEY = 23,
    MAX_KEY_TYPE = 24,
};
typedef enum tcam_key_type_e tcam_key_type_t;

/* use for checking the allocation cfg */
struct tcam_alloc_check_point_s
{
    uint32 index_shift;
    uint32 index_base;
    uint32 table_base;
    uint32 use_int_tcam;
    uint32 key_size;
};
typedef struct tcam_alloc_check_point_s tcam_alloc_check_point_t;

/* global store driver tbl or reg register info */
registers_t drv_regs_list[MAX_REG_NUM];
tables_t drv_tbls_list[MAX_TBL_NUM];

/* Define all dynic table id list */
static tbl_id_t all_dynic_tbl_id_name[] =
{
    DS_MAC_ACL,
    DS_IPV4_ACL,
    DS_MPLS_ACL,
    DS_IPV6_ACL,
    DS_MAC_QOS,
    DS_IPV4_QOS,
    DS_MPLS_QOS,
    DS_IPV6_QOS,
    DS_IPV4_UCAST_DA,
    DS_IPV4_MCAST_DA,
    DS_IPV6_UCAST_DA,
    DS_IPV6_MCAST_DA,
    DS_IPV4_UCAST_PBR_DUAL_DA,
    DS_IPV6_UCAST_PBR_DUAL_DA,
    DS_IPV4_UCAST_SA,
    DS_IPV6_UCAST_SA,
    DS_IPV4_MCAST_RPF,
    DS_IPV6_MCAST_RPF,
    DS_IPV4_SA_NAT,
    DS_IPV6_SA_NAT,
    DS_USER_ID_VLAN,
    DS_USER_ID_MAC,
    DS_USER_ID_IPV4,
    DS_USER_ID_IPV6,
    DS_L2_EDIT_ETH4W,
    DS_L2_EDIT_ETH8W,
    DS_L2_EDIT_FLEX4W,
    DS_L2_EDIT_FLEX8W,
    DS_L2_EDIT_LOOPBACK,
    DS_L2_EDIT_PBB8W,
    DS_L2_EDIT_PBB4W,
    DS_L3EDIT_MPLS4W,
    DS_L3EDIT_MPLS8W,
    DS_L3EDIT_NAT4W,
    DS_L3EDIT_NAT8W,
    DS_L3EDIT_TUNNEL_V4,
    DS_L3EDIT_TUNNEL_V6,
    DS_L3EDIT_FLEX,
    DS_L3EDIT_LOOP_BACK,
    DS_MET_ENTRY,
    DS_MPLS,
    DS_NEXTHOP,
    DS_NEXTHOP8W,
    DS_MAC,
    DS_FWD,
};

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @brief register a table to the table directory
*/
int32
drv_tbl_register(tbl_id_t tbl_id, uint32 mask_offset,
                 uint32 data_offset, uint32 max_idx, uint8 entry_size,
                 uint8 key_size, uint8 num_f, fields_t* ptr_f)
{
    tables_t* ptr = NULL;

    /* table valid check */
    DRV_PTR_VALID_CHECK(ptr_f)
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    ptr = &drv_tbls_list[tbl_id];

    ptr->hw_mask_base = mask_offset;
    ptr->hw_data_base = data_offset;
    ptr->max_index_num = max_idx;
    ptr->entry_size = entry_size;
    ptr->key_size = key_size;
    ptr->num_fields = num_f;
    ptr->ptr_fields = ptr_f;

    return DRV_E_NONE;
}

/**
 @brief Register a table field to the table field directory
*/
fields_t *
drv_tbl_field_find(tbl_id_t tbl_id, fld_id_t field_id)
{
    fields_t* first_f = NULL;
    uint8 num_f;

    if (MAX_TBL_NUM <= tbl_id)
    {
        DRV_DBG_INFO("\nERROR! INVALID TblID! TblID: %d, file:%s line:%d function:%s\n",
                    tbl_id,__FILE__,__LINE__,__FUNCTION__);
        return NULL;
    }

    first_f = DRV_TBL_GET_INFO(tbl_id).ptr_fields;
    num_f = DRV_TBL_GET_INFO(tbl_id).num_fields;

    if (field_id >= num_f)
    {
        return NULL;
    }

    return &first_f[field_id];

}



/**
 @brief Set a field of a table data entry in memory
*/
int32
drv_tbl_field_set(tbl_id_t tbl_id, fld_id_t field_id,
                  uint32* entry, uint32 value)
{
    fields_t* field = NULL;
    uint32 mask;

    DRV_PTR_VALID_CHECK(entry)
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* talbe field support check */
    field = drv_tbl_field_find(tbl_id, field_id);

    if (NULL == field)
    {
        DRV_DBG_INFO("\nERROR (drv_tbl_field_set): tbl-%d, field-%d is not supported.\n", tbl_id, field_id);
        return DRV_E_INVALID_FLD;
    }

    if (field->len < 32)
    {
        mask = (1 << field->len) - 1;
        if ((value & ~mask) != 0)
        {
            /* check if value is too big for this field */
            DRV_DBG_INFO("\nERROR (drv_tbl_field_set): tbl-%d, field-%d, value is too big for this field.\n", tbl_id, field_id);
            return DRV_E_FIELD_OVER;
        }
    }
    else
    {
        mask = -1;
    }

    /* set to memory */
    entry[field->word_offset] = (entry[field->word_offset] & (~(mask<<field->bit_offset))) | (value << field->bit_offset);

    return DRV_E_NONE;
}

/**
 @brief Get a field of a table data entry in memory
*/
int32
drv_tbl_field_get(tbl_id_t tbl_id, fld_id_t field_id,
                  uint32* entry, uint32* value)
{
    fields_t* field = NULL;
    uint32 val;

    DRV_PTR_VALID_CHECK(entry)
    DRV_PTR_VALID_CHECK(value)
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    /* talbe field support check */
    field = drv_tbl_field_find(tbl_id, field_id);

    if (NULL == field)
    {
        DRV_DBG_INFO("\nERROR (drv_tbl_field_get): tbl-%d, field-%d is not supported.\n", tbl_id, field_id);
        return DRV_E_INVALID_FLD;
    }

    /* get from memory */
    val = entry[field->word_offset] >> field->bit_offset;

    if (field->len < 32)
    {
        *value = val & ((1 << field->len)-1);
    }
    else
    {
        *value = val;
    }

    return DRV_E_NONE;
}

/**
 @brief register a reg to the register directory and register field info of this reg together
*/
int32
drv_reg_register(reg_id_t reg_id, uint32 data_offset, uint32 max_idx,
                 uint16 entry_size, uint8 num_f, fields_t* ptr_f)
{
    registers_t* ptr = NULL;

    /* register valid check */
    DRV_PTR_VALID_CHECK(ptr_f)
    DRV_REG_ID_VALID_CHECK(reg_id);

    ptr = &drv_regs_list[reg_id];

    ptr->hw_data_base = data_offset;
    ptr->max_index_num = max_idx;
    ptr->entry_size = entry_size;
    ptr->num_fields = num_f;
    ptr->ptr_fields = ptr_f;

    return DRV_E_NONE;
}

/**
 @brief register a register field to the register field directory
*/
fields_t *
drv_reg_field_find(reg_id_t reg_id, fld_id_t field_id)
{
    fields_t* first_f = NULL;
    uint8 num_f;

    if (MAX_REG_NUM <= reg_id)
    {
        DRV_DBG_INFO("\nERROR! INVALID RegID! RegID: %d, file:%s line:%d function:%s\n",
                    reg_id, __FILE__, __LINE__, __FUNCTION__);
        return NULL;
    }

    first_f = DRV_REG_GET_INFO(reg_id).ptr_fields;
    num_f = DRV_REG_GET_INFO(reg_id).num_fields;

    if (field_id >= num_f)
    {
        return NULL;
    }

    return &first_f[field_id];

}


/**
 @brief Get a field of a register data entry in memory
*/
int32
drv_reg_field_get(reg_id_t reg_id, fld_id_t field_id,
                  uint32* entry, uint32* value)
{
    fields_t* field = NULL;
    uint32 val;

    DRV_PTR_VALID_CHECK(entry)
    DRV_PTR_VALID_CHECK(value)
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* register field support check */
    field = drv_reg_field_find(reg_id, field_id);

    if (field == NULL)
    {
        KAL_LOG_ERROR("\nERROR (drv_reg_field_get): reg-%d, field-%d is not supported.\n", reg_id, field_id);
        return DRV_E_INVALID_FLD;
    }

    /* get from memory */
    val = entry[field->word_offset] >> field->bit_offset;

    if (field->len < 32)
    {
        *value = val & ((1 << field->len)-1);
    }
    else
    {
        *value = val;
    }

    return DRV_E_NONE;
}


/**
 @brief Get a field word offset of a register data entry
*/
int32
drv_reg_field_get_word_offset(reg_id_t reg_id, fld_id_t field_id, uint32* word_offset)
{
    fields_t* field = NULL;

    DRV_REG_ID_VALID_CHECK(reg_id);

    /* register field support check */
    field = drv_reg_field_find(reg_id, field_id);

    if (field == NULL)
    {
        KAL_LOG_ERROR("\nERROR (drv_reg_field_get): reg-%d, field-%d is not supported.\n", reg_id, field_id);
        return DRV_E_INVALID_FLD;
    }

    *word_offset = field->word_offset;

    return DRV_E_NONE;
}


/**
 @brief Set a field of a register data entry in memory
*/
int32
drv_reg_field_set(reg_id_t reg_id, fld_id_t field_id,
                  uint32* entry, uint32 value)
{
    fields_t* field = NULL;
    uint32 mask;

    DRV_PTR_VALID_CHECK(entry)
    DRV_REG_ID_VALID_CHECK(reg_id);

    /* register field support check */
    field = drv_reg_field_find(reg_id, field_id);
    if (field == NULL)
    {
        KAL_LOG_ERROR("\nERROR (drv_reg_field_set): reg-%d, field-%d is not supported.\n", reg_id, field_id);
        return DRV_E_INVALID_FLD;
    }

    if (field->len < 32)
    {
        mask = (1 << field->len) - 1;

        if ((value & ~mask) != 0)
        {
            /* check if value is too big for this field */
            KAL_LOG_ERROR("\nERROR (drv_reg_field_set): reg-%d, field-%d, value is too big for this field.\n", reg_id, field_id);
            return DRV_E_FIELD_OVER;
        }
    }
    else
    {
        mask = -1;
    }

    /* set to memory */
    entry[field->word_offset] = (entry[field->word_offset] & (~ (mask<<field->bit_offset))) | (value << field->bit_offset);

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_sram_reg_ds_to_entry(reg_id_t reg_id, void* ds, uint32* entry)
{
    uint32* p_ds = NULL;
    uint16 ds_size = 0; /* unit: bytes */

    DRV_REG_ID_VALID_CHECK(reg_id);

    ds_size = DRV_REG_ENTRY_SIZE(reg_id);
    p_ds = (uint32 *)ds;    /* Here need to notice: if the ds size is less
                               than uint32, the imperative type conversion
                               may be produce problem. */

    kal_memcpy(entry, p_ds, ds_size);

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_sram_reg_entry_to_ds(reg_id_t reg_id, uint32* entry, void* ds)
{
    uint32* p_ds = NULL;
    uint16 ds_size = 0; /* unit: bytes */

    DRV_REG_ID_VALID_CHECK(reg_id);
    p_ds = (uint32 *)ds;    /* Here need to notice: if the ds size is less
                               than uint32, the imperative type conversion
                               may be produce problem. */

    ds_size = DRV_REG_ENTRY_SIZE(reg_id);

    kal_memset((uint8 *)ds, 0, ds_size);

    kal_memcpy(p_ds, entry, ds_size);

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_sram_tbl_ds_to_entry(tbl_id_t tbl_id, void* ds, void* entry)
{
    uint32* p_ds = NULL;
    uint16 ds_size = 0; /* unit: bytes */

    DRV_TBL_ID_VALID_CHECK(tbl_id);

    ds_size = DRV_TBL_ENTRY_SIZE(tbl_id);
    p_ds = (uint32 *)ds;    /* Here need to notice: if the ds size is less
                               than uint32, the imperative type conversion
                               may be produce problem. */

    kal_memcpy(entry, p_ds, ds_size);

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_sram_tbl_entry_to_ds(tbl_id_t tbl_id, void* entry, void* ds)
{
    uint32* p_ds = NULL;
    uint16 ds_size = 0; /* unit: bytes */

    DRV_TBL_ID_VALID_CHECK(tbl_id);
    p_ds = (uint32 *)ds;    /* Here need to notice: if the ds size is less
                               than uint32, the imperative type conversion
                               may be produce problem. */

    ds_size = DRV_TBL_ENTRY_SIZE(tbl_id);

    kal_memset((uint8 *)ds, 0, ds_size);

    kal_memcpy(p_ds, entry, ds_size);

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_tcam_tbl_ds_to_entry(tbl_id_t tbl_id, void* ds, void* entry)
{
    int32 ret = DRV_E_NONE;
    uint32* ds_data = NULL;
    uint32* ds_mask = NULL;

    uint32* entry_data = NULL;
    uint32* entry_mask = NULL;

    tbl_entry_t* p_ds = NULL;
    tbl_entry_t* p_entry = NULL;

    DRV_PTR_VALID_CHECK(ds);
    DRV_PTR_VALID_CHECK(entry);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    p_ds = (tbl_entry_t *)ds;
    p_entry = (tbl_entry_t *)entry;
    ds_data = p_ds->data_entry;
    ds_mask = p_ds->mask_entry;
    entry_data = p_entry->data_entry;
    entry_mask = p_entry->mask_entry;

    DRV_PTR_VALID_CHECK(ds_data);
    DRV_PTR_VALID_CHECK(ds_mask);
    DRV_PTR_VALID_CHECK(entry_data);
    DRV_PTR_VALID_CHECK(entry_mask);

    ret = drv_sram_tbl_ds_to_entry(tbl_id, ds_data, entry_data);
    if (ret < DRV_E_NONE)
    {
        return ret;
    }

    ret = drv_sram_tbl_ds_to_entry(tbl_id, ds_mask, entry_mask);
    if (ret < DRV_E_NONE)
    {
        return ret;
    }

    return DRV_E_NONE;
}

/**
 @brief
*/
int32
drv_tcam_tbl_entry_to_ds (tbl_id_t tbl_id, void* entry, void* ds)
{
    int32 ret = DRV_E_NONE;
    uint32* ds_data = NULL;
    uint32* ds_mask = NULL;

    uint32* entry_data = NULL;
    uint32* entry_mask = NULL;

    tbl_entry_t* p_ds = NULL;
    tbl_entry_t* p_entry = NULL;

    DRV_PTR_VALID_CHECK(ds);
    DRV_PTR_VALID_CHECK(entry);
    DRV_TBL_ID_VALID_CHECK(tbl_id);

    p_ds = (tbl_entry_t *)ds;
    p_entry = (tbl_entry_t *)entry;
    ds_data = p_ds->data_entry;
    ds_mask = p_ds->mask_entry;
    entry_data = p_entry->data_entry;
    entry_mask = p_entry->mask_entry;

    DRV_PTR_VALID_CHECK(ds_data);
    DRV_PTR_VALID_CHECK(ds_mask);
    DRV_PTR_VALID_CHECK(entry_data);
    DRV_PTR_VALID_CHECK(entry_mask);

    ret = drv_sram_tbl_entry_to_ds(tbl_id, entry_data, ds_data);
    if (ret < DRV_E_NONE)
    {
        return ret;
    }

    ret = drv_sram_tbl_entry_to_ds(tbl_id, entry_mask, ds_mask);
    if (ret < DRV_E_NONE)
    {
        return ret;
    }

    return DRV_E_NONE;
}

static int32
_drv_check_tcam_cfg(tcam_alloc_check_point_t* check_info, uint32 table_id)
{
    tables_t* ptr = NULL;
    int32 ret = DRV_E_NONE;
    //char tbl_name[200] = {0};

    /* table valid check */
    DRV_PTR_VALID_CHECK(check_info)
    DRV_TBL_ID_VALID_CHECK(table_id);

    ptr = &drv_tbls_list[table_id];

    if ((0 == check_info->index_base)
        && (0 == ptr->max_index_num)
        && (0 == ptr->hw_data_base))
    {
        //diag_getstr_byid(TABLE_ID, table_id, tbl_name);
        DRV_DBG_INFO("+++ table_id: %d is not allocated memory!\n", table_id);
        return ret;
    }

    if (check_info->use_int_tcam)
    {
        /* check data base */
        if (((ptr->hw_data_base - DRV_INT_TCAM_DATA_ASIC_BASE)/16)
             != (check_info->index_base<<8))
        {
            DRV_DBG_INFO("+++drv_tbls_list.hw_DATA_base is not match with cfg!\n");
            ret = DRV_E_INVALID_CFG_NO_MATCH;
            goto error;
        }

        /* check mask base */
        if (((ptr->hw_mask_base - DRV_INT_TCAM_MASK_ASIC_BASE)/16)
             != (check_info->index_base<<8))
        {
            DRV_DBG_INFO("+++ drv_tbls_list.hw_MASK_base is not match with cfg!\n");
            ret = DRV_E_INVALID_CFG_NO_MATCH;
            goto error;
        }
    }
    else
    {
        /* check data base */
        if (((ptr->hw_data_base - DRV_EXT_TCAM_DATA_ASIC_BASE)/16)
             != (check_info->index_base<<8))
        {
            DRV_DBG_INFO("+++ drv_tbls_list.hw_DATA_base is not match with cfg!\n");
            ret = DRV_E_INVALID_CFG_NO_MATCH;
            goto error;
        }

        /* check mask base */
        if (((ptr->hw_mask_base - DRV_EXT_TCAM_MASK_ASIC_BASE)/16)
             != (check_info->index_base<<8))
        {
            DRV_DBG_INFO("+++ drv_tbls_list.hw_MASK_base is not match with cfg!\n");
            ret = DRV_E_INVALID_CFG_NO_MATCH;
            goto error;
        }
    }

    /* check keysize */
    if (ptr->key_size != (16<<check_info->key_size))
    {
        DRV_DBG_INFO("+++ drv_tbls_list.KEY_SIZE is not match with cfg!\n");
        ret = DRV_E_INVALID_CFG_NO_MATCH;
        goto error;
    }

    return ret;

error:
  // diag_getstr_byid(TABLE_ID, table_id, tbl_name);
   DRV_DBG_INFO("+++ TableId : %d\n", table_id);
   DRV_DBG_INFO("\n+++ Dump drv_tbls_list\n");
   DRV_DBG_INFO("hw_data_base = 0x%x\n", ptr->hw_data_base);
   DRV_DBG_INFO("hw_mask_base = 0x%x\n", ptr->hw_mask_base);
   DRV_DBG_INFO("max_index_num = 0x%x\n", ptr->max_index_num);
   DRV_DBG_INFO("key_size = 0x%x\n", ptr->key_size);
   DRV_DBG_INFO("entry_size = 0x%x\n", ptr->entry_size);

   DRV_DBG_INFO("\n+++ Dump configuration\n");
   DRV_DBG_INFO("index_base = 0x%x\n", check_info->index_base);
   DRV_DBG_INFO("index_shift = 0x%x\n", check_info->index_shift);
   DRV_DBG_INFO("table_base = 0x%x\n", check_info->table_base);
   DRV_DBG_INFO("use_int_tcam = 0x%x\n", check_info->use_int_tcam);
   DRV_DBG_INFO("key_size = 0x%x\n", check_info->key_size);
   DRV_DBG_INFO("++++++++++++++++++++++++\n");

   return ret;
}


static int32
_drv_check_tcam_alloc_process(void)
{
    /* temp var */
    uint32 cmd = 0;
    uint32 chip_id = 0;
    uint32 i = 0;
    uint32 key_id = 0;
    uint32 expect_key_id = 0;
    uint32 table_id = 0;

    /* check points (do not check lookupInst) */
    tcam_alloc_check_point_t check_info;
    tcam_alloc_check_point_t epe_aclqos_check_info_tmp;

    ipe_lookup_ctl_t lkp_ctl;
    ipe_lookup_result_ctl_t rslt_ctl;

    ipe_user_id_ctl_t user_id_ctl;
    epe_acl_qos_ctl_t qacl_c_ctl;

    oam_lookup_result_ctl_t oam_lookup_rst_ctl;
    oam_lookup_ctl_t oam_lookup_ctl;

    /* ipe lkp register */
    kal_memset(&lkp_ctl, 0, sizeof(lkp_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &lkp_ctl));
    kal_memset(&rslt_ctl, 0, sizeof(rslt_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &rslt_ctl));

    /* userId register */
    kal_memset(&user_id_ctl, 0, sizeof(user_id_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_USER_ID_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &user_id_ctl));

    /* epe aclqos register */
    kal_memset(&qacl_c_ctl, 0, sizeof(qacl_c_ctl));
    cmd = DRV_IOR(IOC_REG, EPE_ACL_QOS_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id ,0, cmd, &qacl_c_ctl));

    /* oam register */
    kal_memset(&oam_lookup_ctl, 0, sizeof(oam_lookup_ctl));
    cmd = DRV_IOR(IOC_REG, OAM_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &oam_lookup_ctl));
    kal_memset(&oam_lookup_rst_ctl, 0, sizeof(oam_lookup_rst_ctl));
    cmd = DRV_IOR(IOC_REG, OAM_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &oam_lookup_rst_ctl));

    /* start check each key lkp control cfg */
    for (i = 0; i < MAX_KEY_TYPE; i++)
    {
        kal_memset(&check_info, 0, sizeof(check_info));
        kal_memset(&epe_aclqos_check_info_tmp, 0, sizeof(epe_aclqos_check_info_tmp));
        key_id = 0;
        table_id = 0;
        expect_key_id = 0;

        switch (i)
        {
            case MAC_KEY:
                /* check macda cfg */
                check_info.index_shift = (rslt_ctl.mac_da_lookup_result_ctl0>>16)&0x3;
                check_info.index_base = (rslt_ctl.mac_da_lookup_result_ctl1>>16)&0xFFF;
                check_info.table_base = rslt_ctl.mac_da_lookup_result_ctl1&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.mac_da_lookup_ctl>>14) & 0x1);
                check_info.key_size = (lkp_ctl.mac_da_lookup_ctl>>12) & 0x3;
                key_id = (lkp_ctl.mac_da_lookup_ctl>>8) & 0xF;
                expect_key_id = MACDA_TABLEID0_C;
                table_id = DS_MAC_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ MACDaKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                /* check macsa cfg */
                check_info.index_shift = (rslt_ctl.mac_sa_lookup_result_ctl0>>16)&0x3;
                check_info.index_base = (rslt_ctl.mac_sa_lookup_result_ctl1>>16)&0xFFF;
                check_info.table_base = rslt_ctl.mac_sa_lookup_result_ctl1&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.mac_sa_lookup_ctl>>14) & 0x1);
                check_info.key_size = (lkp_ctl.mac_sa_lookup_ctl>>12) & 0x3;
                key_id = (lkp_ctl.mac_sa_lookup_ctl>>8) & 0xF;
                expect_key_id = MACDA_TABLEID0_C;
                table_id = DS_MAC_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ MACSaKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case ACL_IPV6_KEY:
                /* check acl ipv6 cfg */
                check_info.index_shift = (rslt_ctl.acl_lookup_result_ctl00>>16)&0x3;
                check_info.index_base = (rslt_ctl.acl_lookup_result_ctl01>>16)&0xFFF;
                check_info.table_base = rslt_ctl.acl_lookup_result_ctl01&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl0>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl0>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl0>>8) & 0xF;
                expect_key_id = ACL_IPV6_TABLEID0_C;
                table_id = DS_ACL_IPV6_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ IPE AclIpv6Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                /* check EPE aclqos control register cfg */
                epe_aclqos_check_info_tmp.index_shift = (qacl_c_ctl.acl_lookup_result_ctl038_to32>>4)&0x3;
                epe_aclqos_check_info_tmp.index_base = (qacl_c_ctl.acl_lookup_result_ctl031_to0>>12)&0xFFF;
                epe_aclqos_check_info_tmp.table_base = qacl_c_ctl.acl_lookup_result_ctl031_to0&0xFFF;
                epe_aclqos_check_info_tmp.use_int_tcam = !((qacl_c_ctl.acl_qos_lookup_ctl0>>14) & 0x1);
                epe_aclqos_check_info_tmp.key_size = ((qacl_c_ctl.acl_qos_lookup_ctl0>> 12)&0x3);
                key_id = (qacl_c_ctl.acl_qos_lookup_ctl0>>8) & 0xF;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ EPE AclIpv6Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case ACL_IPV4_KEY:
                /* check acl ipv4 cfg */
                check_info.index_shift = (rslt_ctl.acl_lookup_result_ctl10>>16)&0x3;
                check_info.index_base = (rslt_ctl.acl_lookup_result_ctl11>>16)&0xFFF;
                check_info.table_base = rslt_ctl.acl_lookup_result_ctl11&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl1>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl1>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl1>>8) & 0xF;
                expect_key_id = ACL_IPV4_TABLEID0_C;
                table_id = DS_ACL_IPV4_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ IPE AclIpv4Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                /* check EPE aclqos control register cfg */
                epe_aclqos_check_info_tmp.index_shift = (qacl_c_ctl.acl_lookup_result_ctl138_to32>>4)&0x3;
                epe_aclqos_check_info_tmp.index_base = (qacl_c_ctl.acl_lookup_result_ctl131_to0>>12)&0xFFF;
                epe_aclqos_check_info_tmp.table_base = qacl_c_ctl.acl_lookup_result_ctl131_to0&0xFFF;
                epe_aclqos_check_info_tmp.use_int_tcam = !((qacl_c_ctl.acl_qos_lookup_ctl1>>14) & 0x1);
                epe_aclqos_check_info_tmp.key_size = ((qacl_c_ctl.acl_qos_lookup_ctl1>> 12)&0x3);
                key_id = (qacl_c_ctl.acl_qos_lookup_ctl1>>8) & 0xF;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ EPE AclIpv4Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case ACL_MAC_KEY:
                /* check acl mac cfg */
                check_info.index_shift = (rslt_ctl.acl_lookup_result_ctl20>>16)&0x3;
                check_info.index_base = (rslt_ctl.acl_lookup_result_ctl21>>16)&0xFFF;
                check_info.table_base = rslt_ctl.acl_lookup_result_ctl21&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl2>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl2>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl2>>8) & 0xF;
                expect_key_id = ACL_MAC_TABLEID0_C;
                table_id = DS_ACL_MAC_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ IPE AclMacKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                /* check EPE aclqos control register cfg */
                epe_aclqos_check_info_tmp.index_shift = (qacl_c_ctl.acl_lookup_result_ctl238_to32>>4)&0x3;
                epe_aclqos_check_info_tmp.index_base = (qacl_c_ctl.acl_lookup_result_ctl231_to0>>12)&0xFFF;
                epe_aclqos_check_info_tmp.table_base = qacl_c_ctl.acl_lookup_result_ctl231_to0&0xFFF;
                epe_aclqos_check_info_tmp.use_int_tcam = !((qacl_c_ctl.acl_qos_lookup_ctl2>>14) & 0x1);
                epe_aclqos_check_info_tmp.key_size = ((qacl_c_ctl.acl_qos_lookup_ctl2>> 12)&0x3);
                key_id = (qacl_c_ctl.acl_qos_lookup_ctl2>>8) & 0xF;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ EPE AclMacKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case QOS_IPV6_KEY:
                /* check qos ipv6 cfg */
                check_info.index_shift = (rslt_ctl.qos_lookup_result_ctl00>>16)&0x3;
                check_info.index_base = (rslt_ctl.qos_lookup_result_ctl01>>16)&0xFFF;
                check_info.table_base = rslt_ctl.qos_lookup_result_ctl01&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl0>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl0>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl0>>8) & 0xF;
                expect_key_id = QOS_IPV6_TABLEID0_C;
                table_id = DS_QOS_IPV6_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ QosIpv6Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case QOS_IPV4_KEY:
                /* check qos ipv4 cfg */
                check_info.index_shift = (rslt_ctl.qos_lookup_result_ctl10>>16)&0x3;
                check_info.index_base = (rslt_ctl.qos_lookup_result_ctl11>>16)&0xFFF;
                check_info.table_base = rslt_ctl.qos_lookup_result_ctl11&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl1>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl1>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl1>>8) & 0xF;
                expect_key_id = QOS_IPV4_TABLEID0_C;
                table_id = DS_QOS_IPV4_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ QosIpv4Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case QOS_MAC_KEY:
                /* check qos mac cfg */
                check_info.index_shift = (rslt_ctl.qos_lookup_result_ctl20>>16)&0x3;
                check_info.index_base = (rslt_ctl.qos_lookup_result_ctl21>>16)&0xFFF;
                check_info.table_base = rslt_ctl.qos_lookup_result_ctl21&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.acl_qos_lookup_ctl2>>14) & 0x1);
                check_info.key_size = (lkp_ctl.acl_qos_lookup_ctl2>>12) & 0x3;
                key_id = (lkp_ctl.acl_qos_lookup_ctl2>>8) & 0xF;
                expect_key_id = QOS_MAC_TABLEID0_C;
                table_id = DS_QOS_MAC_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ QosMacKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV4_UCAST_ROUTE_KEY:
                /* check ipv4 Uc Da cfg */
                check_info.index_shift = (rslt_ctl.ip_da_lookup_result_ctl00>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_da_lookup_result_ctl01>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_da_lookup_result_ctl01&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_da_lookup_ctl0>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_da_lookup_ctl0>>12) & 0x3;
                key_id = (lkp_ctl.ip_da_lookup_ctl0>>8) & 0xF;
                expect_key_id = UCAST_IPV4_TABLEID0_C;
                table_id = DS_IPV4_UCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv4UcKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                /* check ipv4 Uc Sa rpf cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl00>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl01>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl01&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl0>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl0>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl0>>8) & 0xF;
                expect_key_id = UCAST_IPV4_TABLEID0_C;
                table_id = DS_IPV4_UCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv4UcRPFKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV4_MCAST_ROUTE_KEY:
                /* check ipv4 Mc cfg */
                check_info.index_shift = (rslt_ctl.ip_da_lookup_result_ctl10>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_da_lookup_result_ctl11>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_da_lookup_result_ctl11&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_da_lookup_ctl1>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_da_lookup_ctl1>>12) & 0x3;
                key_id = (lkp_ctl.ip_da_lookup_ctl1>>8) & 0xF;
                expect_key_id = MCAST_IPV4_TABLEID0_C;
                table_id = DS_IPV4_MCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv4McKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV6_UCAST_ROUTE_KEY:
                /* check ipv6 uc cfg */
                check_info.index_shift = (rslt_ctl.ip_da_lookup_result_ctl20>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_da_lookup_result_ctl21>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_da_lookup_result_ctl21&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_da_lookup_ctl2>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_da_lookup_ctl2>>12) & 0x3;
                key_id = (lkp_ctl.ip_da_lookup_ctl2>>8) & 0xF;
                expect_key_id = UCAST_IPV6_TABLEID0_C;
                table_id = DS_IPV6_UCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv6UcKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                /* check ipv6 Uc Sa rpf cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl10>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl11>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl11&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl1>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl1>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl1>>8) & 0xF;
                expect_key_id = UCAST_IPV6_TABLEID0_C;
                table_id = DS_IPV6_UCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv6UcRPFKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));

                break;

            case IPV6_MCAST_ROUTE_KEY:
                /* check ipv6 Mc cfg */
                check_info.index_shift = (rslt_ctl.ip_da_lookup_result_ctl30>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_da_lookup_result_ctl31>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_da_lookup_result_ctl31&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_da_lookup_ctl3>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_da_lookup_ctl3>>12) & 0x3;
                key_id = (lkp_ctl.ip_da_lookup_ctl3>>8) & 0xF;
                expect_key_id = MCAST_IPV6_TABLEID0_C;
                table_id = DS_IPV6_MCAST_ROUTE_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv6McKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV4_NAT_KEY:
                /* check ipv4 NAT cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl20>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl21>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl21&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl2>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl2>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl2>>8) & 0xF;
                expect_key_id = NAT_IPV4_TABLEID0_C;
                table_id = DS_IPV4_NAT_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv4Sa NAT Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV6_NAT_KEY:
                /* check ipv6 NAT cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl30>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl31>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl31&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl3>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl3>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl3>>8) & 0xF;
                expect_key_id = NAT_IPV6_TABLEID0_C;
                table_id = DS_IPV6_NAT_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv6Sa NAT Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV4_PBR_DUALDA_KEY:
                /* check ipv4 PBR cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl40>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl41>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl41&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl4>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl4>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl4>>8) & 0xF;
                expect_key_id = PBR_IPV4_TABLEID0_C;
                table_id = DS_IPV4_PBR_DUALDA_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv4Sa PBR Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case IPV6_PBR_DUALDA_KEY:
                /* check ipv6 PBR cfg */
                check_info.index_shift = (rslt_ctl.ip_sa_lookup_result_ctl50>>16)&0x3;
                check_info.index_base = (rslt_ctl.ip_sa_lookup_result_ctl51>>16)&0xFFF;
                check_info.table_base = rslt_ctl.ip_sa_lookup_result_ctl51&0xFFF;
                check_info.use_int_tcam = !((lkp_ctl.ip_sa_lookup_ctl5>>14) & 0x1);
                check_info.key_size = (lkp_ctl.ip_sa_lookup_ctl5>>12) & 0x3;
                key_id = (lkp_ctl.ip_sa_lookup_ctl5>>8) & 0xF;
                expect_key_id = PBR_IPV6_TABLEID0_C;
                table_id = DS_IPV6_PBR_DUALDA_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ Ipv6Sa PBR Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case USER_ID_MAC_KEY:
                 /* check UserId Mac cfg */
                check_info.index_shift = user_id_ctl.index_shift0;
                check_info.index_base = user_id_ctl.index_base0;
                check_info.table_base = user_id_ctl.table_base0;
                check_info.use_int_tcam = !user_id_ctl.use_ext_tcam0;
                check_info.key_size = user_id_ctl.key_size0;
                key_id = user_id_ctl.table_id0;
                expect_key_id = USERID_MAC_TABLEID0_C;
                table_id = DS_USER_ID_MAC_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ UserId MAC Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case USER_ID_IPV6_KEY:
                 /* check UserId ipv6 cfg */
                check_info.index_shift = user_id_ctl.index_shift1;
                check_info.index_base = user_id_ctl.index_base1;
                check_info.table_base = user_id_ctl.table_base1;
                check_info.use_int_tcam = !user_id_ctl.use_ext_tcam1;
                check_info.key_size = user_id_ctl.key_size1;
                key_id = user_id_ctl.table_id1;
                expect_key_id = USERID_IPV6_TABLEID0_C;
                table_id = DS_USER_ID_IPV6_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ UserId Ipv6 Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case USER_ID_IPV4_KEY:
                 /* check UserId ipv4 cfg */
                check_info.index_shift = user_id_ctl.index_shift2;
                check_info.index_base = user_id_ctl.index_base2;
                check_info.table_base = user_id_ctl.table_base2;
                check_info.use_int_tcam = !user_id_ctl.use_ext_tcam2;
                check_info.key_size = user_id_ctl.key_size2;
                key_id = user_id_ctl.table_id2;
                expect_key_id = USERID_IPV4_TABLEID0_C;
                table_id = DS_USER_ID_IPV4_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ UserId Ipv4 Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case USER_ID_VLAN_KEY:
                 /* check UserId Vlan cfg */
                check_info.index_shift = user_id_ctl.index_shift3;
                check_info.index_base = user_id_ctl.index_base3;
                check_info.table_base = user_id_ctl.table_base3;
                check_info.use_int_tcam = !user_id_ctl.use_ext_tcam3;
                check_info.key_size = user_id_ctl.key_size3;
                key_id = user_id_ctl.table_id3;
                expect_key_id = USERID_VLAN_TABLEID_C;
                table_id = DS_USER_ID_VLAN_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ UserId Vlan Key tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case ETH_OAM_KEY:
                /* check EthOamKey cfg */
                check_info.index_shift = oam_lookup_rst_ctl.index_shift;
                check_info.index_base = oam_lookup_rst_ctl.index_base;
                check_info.table_base = oam_lookup_rst_ctl.table_base;
                check_info.use_int_tcam = !oam_lookup_ctl.oam_lkup_ext_tcam_en;
                check_info.key_size = oam_lookup_ctl.oam_lkup_key_size;
                key_id = oam_lookup_ctl.oam_lkup_table_id;
                expect_key_id = OAM_TABLEID;
                table_id = DS_ETH_OAM_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ EthOamKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case ETH_OAM_RMEP_KEY:
                /* check EthOamRMEPKey cfg */
                check_info.index_shift = oam_lookup_rst_ctl.index_shift;
                check_info.index_base = oam_lookup_rst_ctl.index_base;
                check_info.table_base = oam_lookup_rst_ctl.table_base;
                check_info.use_int_tcam = !oam_lookup_ctl.oam_lkup_ext_tcam_en;
                check_info.key_size = oam_lookup_ctl.oam_lkup_key_size;
                key_id = oam_lookup_ctl.oam_lkup_table_id;
                expect_key_id = OAM_TABLEID;
                table_id = DS_ETH_OAM_RMEP_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ EthOamRMEPKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case MPLS_OAM_LABEL_KEY:
                /* check MplsOamLabelKey cfg */
                check_info.index_shift = oam_lookup_rst_ctl.index_shift;
                check_info.index_base = oam_lookup_rst_ctl.index_base;
                check_info.table_base = oam_lookup_rst_ctl.table_base;
                check_info.use_int_tcam = !oam_lookup_ctl.oam_lkup_ext_tcam_en;
                check_info.key_size = oam_lookup_ctl.oam_lkup_key_size;
                key_id = oam_lookup_ctl.oam_lkup_table_id;
                expect_key_id = OAM_TABLEID;
                table_id = DS_MPLS_OAM_LABEL_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ MplsOamLabelKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case MPLS_OAM_IPV4_TTSI_KEY:
                /* check MplsOamIpv4Ttsi cfg */
                check_info.index_shift = oam_lookup_rst_ctl.index_shift;
                check_info.index_base = oam_lookup_rst_ctl.index_base;
                check_info.table_base = oam_lookup_rst_ctl.table_base;
                check_info.use_int_tcam = !oam_lookup_ctl.oam_lkup_ext_tcam_en;
                check_info.key_size = oam_lookup_ctl.oam_lkup_key_size;
                key_id = oam_lookup_ctl.oam_lkup_table_id;
                expect_key_id = OAM_TABLEID;
                table_id = DS_MPLS_OAM_IPV4_TTSI_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ MplsOamIpv4Ttsi tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            case PBT_OAM_KEY:
                /* check PbtOamKey cfg */
                check_info.index_shift = oam_lookup_rst_ctl.index_shift;
                check_info.index_base = oam_lookup_rst_ctl.index_base;
                check_info.table_base = oam_lookup_rst_ctl.table_base;
                check_info.use_int_tcam = !oam_lookup_ctl.oam_lkup_ext_tcam_en;
                check_info.key_size = oam_lookup_ctl.oam_lkup_key_size;
                key_id = oam_lookup_ctl.oam_lkup_table_id;
                expect_key_id = OAM_TABLEID;
                table_id = DS_PBT_OAM_KEY;
                if ((key_id != expect_key_id) && (0 != drv_tbls_list[table_id].max_index_num))
                {
                    DRV_DBG_INFO("+++ PbtOamKey tableID check ERROR!\n");
                    DRV_DBG_INFO("+++ Register cfg tableid = 0x%x! And expect value = 0x%x\n", key_id, expect_key_id);
                    return DRV_E_INVALID_CFG_TBL_ID;
                }
                DRV_IF_ERROR_RETURN(_drv_check_tcam_cfg(&check_info, table_id));
                break;

            default:
                break;
        }
    }

    return DRV_E_NONE;
}

static int32
_drv_check_hash_table_alloc_process(void)
{
    /* Note: still not check HashLkpCtl.**bitnum, HashResultLkpCtl.tableBase */
    uint32 cmd = 0;
    uint32 chip_id = 0;
    /*uint32 sel_96k = 0;*/
    uint32 hashkey_base = TB_INFO_HASH_MEM_OFFSET;
    uint32 i = 0;
    uint32 list_len = 0;
    int32 ret = DRV_E_NONE;
    uint32 bit_num, bit_num_sa, table_base = 0, table_base_sa, use_hash = 0, use_hash_sa;
    //char table_name[200] = {0};
    tables_t *ptr = NULL;

    uint32 hash_tbl_id_list[] = {DS_MAC_HASH_KEY0, DS_IPV4_UCAST_HASH_KEY0,
                            DS_IPV4_MCAST_HASH_KEY0, DS_IPV6_UCAST_HASH_KEY0,
                            DS_IPV6_MCAST_HASH_KEY0};

    hash_ds_ctl_lookup_ctl_t hash_lkp_ctl;
    ipe_lookup_ctl_t lkp_ctl;

    kal_memset(&lkp_ctl, 0, sizeof(lkp_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &lkp_ctl));

    /*
    cmd = DRV_IOR(IOC_REG, HASH_KEY_SELECT, HASH_KEY_SELECT_CFG_HASH_KEY_SELECT98K);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &sel_96k));
    */

    kal_memset(&hash_lkp_ctl, 0, sizeof(hash_lkp_ctl));
    cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &hash_lkp_ctl));

    list_len = sizeof(hash_tbl_id_list)/sizeof(uint32);

    for (i = 0; i < list_len; i++)
    {
//        kal_memset(table_name, 0, sizeof(table_name));
        switch (hash_tbl_id_list[i])
        {
            case DS_MAC_HASH_KEY0:
                use_hash = (lkp_ctl.mac_da_lookup_ctl>>15)&0x1;
                use_hash_sa = (lkp_ctl.mac_sa_lookup_ctl>>15)&0x1;
                bit_num = hash_lkp_ctl.mac_da_bits_num;
                bit_num_sa = hash_lkp_ctl.mac_sa_bits_num;
                table_base = hash_lkp_ctl.mac_da_table_base;
                table_base_sa = hash_lkp_ctl.mac_sa_table_base;
                if (bit_num != bit_num_sa)
                {
                    DRV_DBG_INFO("+++ HASH_DS_CTL_LOOKUP_CTL DA bitNum is not equal with SA bitNum\n");
                    return DRV_E_INVALID_CFG_NO_MATCH;
                }

                if (table_base != table_base_sa)
                {
                    DRV_DBG_INFO("+++ HASH_DS_CTL_LOOKUP_CTL DA table_base is not equal with SA table_base\n");
                    return DRV_E_INVALID_CFG_NO_MATCH;
                }

                if (use_hash != use_hash_sa)
                {
                    DRV_DBG_INFO("+++ HASH_DS_CTL_LOOKUP_CTL DA use_hash is not equal with SA table_base\n");
                    return DRV_E_INVALID_CFG_NO_MATCH;
                }
                break;

            case DS_IPV4_UCAST_HASH_KEY0:
                use_hash = (lkp_ctl.ip_da_lookup_ctl0>>15)&0x1;
              /*  bit_num = hash_lkp_ctl.ipv4_ucast_bits_num;*/
                table_base = hash_lkp_ctl.ipv4_ucast_table_base;
                break;

            case DS_IPV4_MCAST_HASH_KEY0:
                use_hash = (lkp_ctl.ip_da_lookup_ctl1>>15)&0x1;
            /*    bit_num = hash_lkp_ctl.ipv4_mcast_bits_num;*/
                table_base = hash_lkp_ctl.ipv4_mcast_table_base;
                break;

            case DS_IPV6_UCAST_HASH_KEY0:
                use_hash = (lkp_ctl.ip_da_lookup_ctl2>>15)&0x1;
              /*   bit_num = hash_lkp_ctl.ipv6_ucast_bits_num; */
                table_base = hash_lkp_ctl.ipv6_ucast_table_base;
                break;

            case DS_IPV6_MCAST_HASH_KEY0:
                use_hash = (lkp_ctl.ip_da_lookup_ctl3>>15)&0x1;
              /*  bit_num = hash_lkp_ctl.ipv6_mcast_bits_num; */
                table_base = hash_lkp_ctl.ipv6_mcast_table_base;
                break;

            default:
                break;
        }

        ptr = &drv_tbls_list[hash_tbl_id_list[i]];
        //diag_getstr_byid(TABLE_ID, hash_tbl_id_list[i], table_name);

        if (!use_hash)
        {
           // DRV_DBG_INFO("%s hash key table not enable use_hash!\n", table_name);
            continue;
        }

        if ((!ptr->max_index_num) && (!table_base) && (!ptr->hw_data_base))
        {
            continue;
        }

        if ((((ptr->hw_data_base - hashkey_base)/16)>>10) != table_base)
        {
//            DRV_DBG_INFO("%s hash key table's cfg is not match with drv_tbls_list!\n", table_name);
            ret = DRV_E_INVALID_CFG_NO_MATCH;
            goto error;
        }
    }
    return ret;

error:
    DRV_DBG_INFO("+++ HashKeyTableId : %d\n", hash_tbl_id_list[i]);
    DRV_DBG_INFO("\n+++ Dump drv_tbls_list\n");
    DRV_DBG_INFO("hw_data_base = 0x%x\n", ptr->hw_data_base);
    DRV_DBG_INFO("hw_mask_base = 0x%x\n", ptr->hw_mask_base);
    DRV_DBG_INFO("max_index_num = 0x%x\n", ptr->max_index_num);
    DRV_DBG_INFO("key_size = 0x%x\n", ptr->key_size);
    DRV_DBG_INFO("entry_size = 0x%x\n", ptr->entry_size);
    DRV_DBG_INFO("\n+++ Dump configuration\n");
    DRV_DBG_INFO("table_base = 0x%x\n", table_base);
    DRV_DBG_INFO("Note: hash key sram base = 0x%x\n", hashkey_base);
    DRV_DBG_INFO("According to drv_tbls_list, the table_base cfg value should be = 0x%x",
              (((ptr->hw_data_base - hashkey_base)/16)>>8));
    DRV_DBG_INFO("++++++++++++++++++++++++\n");

    return ret;
}

static int32
_drv_check_dynic_table_alloc_process(void)
{
    /* temp var */
    uint32 cmd = 0;
    uint32 chip_id = 0;
    uint32 i = 0;
    uint32 table_id = 0;
    //char table_name[300] = {0};

    ipe_lookup_result_ctl_t rslt_ctl;
    ipe_forward_ctl_t fwd_ctl;
    epe_next_hop_ctl_t nhp_ctl;
    met_fifo_ctl_t metfifo_ctl;
    ipe_user_id_ctl_t user_id_ctl;

    uint32 table_base = 0;
    uint32 size = sizeof(all_dynic_tbl_id_name)/sizeof(tbl_id_t);

    kal_memset(&rslt_ctl, 0, sizeof(rslt_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &rslt_ctl));

    kal_memset(&fwd_ctl, 0, sizeof(fwd_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_FORWARD_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &fwd_ctl));

    kal_memset(&nhp_ctl, 0, sizeof(nhp_ctl));
    cmd = DRV_IOR(IOC_REG, EPE_NEXT_HOP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &nhp_ctl));

    kal_memset(&metfifo_ctl, 0, sizeof(metfifo_ctl));
    cmd = DRV_IOR(IOC_REG, MET_FIFO_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &metfifo_ctl));

    /* userId register */
    kal_memset(&user_id_ctl, 0, sizeof(user_id_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_USER_ID_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &user_id_ctl));

    for (i = 0; i < size; i++)
    {
        table_id = all_dynic_tbl_id_name[i];
//        kal_memset(table_name, 0, sizeof(table_name));
        switch (table_id)
        {
            /* check Action each table */
            case DS_MAC:
                table_base = rslt_ctl.mac_da_lookup_result_ctl1&0xFFF;
                if (table_base != (rslt_ctl.mac_sa_lookup_result_ctl1&0xFFF))
                {
                    DRV_DBG_INFO("+++ MACSA and MacDA table base cfg is not equal!!!\n");
                    DRV_DBG_INFO("+++ CFG MACSA table base cfg = 0x%x, CFG MACDA table base cfg = 0x%x\n",
                               (rslt_ctl.mac_sa_lookup_result_ctl1&0xFFF),
                               (rslt_ctl.mac_da_lookup_result_ctl1&0xFFF));
                    return DRV_E_INVALID_CFG_TBL_BASE;
                }
                break;

            case DS_IPV4_UCAST_DA:
                table_base = rslt_ctl.ip_da_lookup_result_ctl01&0xFFF;
                break;

            case DS_IPV4_MCAST_DA:
                table_base = rslt_ctl.ip_da_lookup_result_ctl11&0xFFF;
                break;

            case DS_IPV6_UCAST_DA:
                table_base = rslt_ctl.ip_da_lookup_result_ctl21&0xFFF;
                break;

            case DS_IPV6_MCAST_DA:
                table_base = rslt_ctl.ip_da_lookup_result_ctl31&0xFFF;
                break;

            case DS_MAC_ACL:
                table_base = rslt_ctl.acl_lookup_result_ctl21&0xFFF;
                break;

            case DS_IPV4_ACL:
                table_base = rslt_ctl.acl_lookup_result_ctl11&0xFFF;
                break;

            case DS_MPLS_ACL:
                table_base = rslt_ctl.acl_lookup_result_ctl11&0xFFF;
                break;

            case DS_IPV6_ACL:
                table_base = rslt_ctl.acl_lookup_result_ctl01&0xFFF;
                break;

            case DS_MAC_QOS:
                table_base = rslt_ctl.qos_lookup_result_ctl21&0xFFF;
                break;

            case DS_IPV4_QOS:
                table_base = rslt_ctl.qos_lookup_result_ctl11&0xFFF;
                break;

            case DS_MPLS_QOS:
                table_base = rslt_ctl.qos_lookup_result_ctl11&0xFFF;
                break;

            case DS_IPV6_QOS:
                table_base = rslt_ctl.qos_lookup_result_ctl01&0xFFF;
                break;

            case DS_IPV4_UCAST_PBR_DUAL_DA:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl41&0xFFF;
                break;

            case DS_IPV6_UCAST_PBR_DUAL_DA:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl51&0xFFF;
                break;

            case DS_IPV4_UCAST_SA:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl01&0xFFF;
                break;

            case DS_IPV6_UCAST_SA:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl11&0xFFF;
                break;

            case DS_IPV4_MCAST_RPF:
                table_base = rslt_ctl.ds_ipv4_mcast_rpf_table_base;
                break;

            case DS_IPV6_MCAST_RPF:
                table_base = rslt_ctl.ds_ipv6_mcast_rpf_table_base;
                break;

            case DS_IPV4_SA_NAT:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl21&0xFFF;
                break;

            case DS_IPV6_SA_NAT:
                table_base = rslt_ctl.ip_sa_lookup_result_ctl31&0xFFF;
                break;

            case DS_USER_ID_MAC:
                table_base = user_id_ctl.table_base0;
                break;

            case DS_USER_ID_IPV6:
                table_base = user_id_ctl.table_base1;
                break;

            case DS_USER_ID_IPV4:
                table_base = user_id_ctl.table_base2;
                break;

            case DS_USER_ID_VLAN:
                table_base = user_id_ctl.table_base3;
                break;

            /* check DsFwd each table */
            case DS_FWD:
                table_base = ((fwd_ctl.ds_fwd_base0<<2) | fwd_ctl.ds_fwd_base1);
                break;

            /* check NextHop each table */
            case DS_NEXTHOP:
            case DS_NEXTHOP8W:
                table_base = nhp_ctl.ds_next_hop_table_base;
                break;

            /* check L2 edit each table */
            case DS_L2_EDIT_ETH4W:
            case DS_L2_EDIT_ETH8W:
            case DS_L2_EDIT_FLEX4W:
            case DS_L2_EDIT_FLEX8W:
            case DS_L2_EDIT_LOOPBACK:
            case DS_L2_EDIT_PBB8W:
            case DS_L2_EDIT_PBB4W:
                table_base = nhp_ctl.ds_l2_edit_table_base;
                break;

            /* check L3 edit each table */
            case DS_L3EDIT_MPLS4W:
            case DS_L3EDIT_MPLS8W:
            case DS_L3EDIT_NAT4W:
            case DS_L3EDIT_NAT8W:
            case DS_L3EDIT_TUNNEL_V4:
            case DS_L3EDIT_TUNNEL_V6:
            case DS_L3EDIT_FLEX:
            case DS_L3EDIT_LOOP_BACK:
                table_base = nhp_ctl.ds_l3_edit_table_base;
                break;

            case DS_MET_ENTRY:
                table_base = metfifo_ctl.ds_met_entry_base;
                break;

            case DS_MPLS:
                table_base = ((rslt_ctl.ds_mpls_table_base1<<4) | rslt_ctl.ds_mpls_table_base0);
                break;

            default:
                break;
        }

        //kal_memset(table_name, 0, 300);
        //diag_getstr_byid(TABLE_ID, table_id, table_name);

        if ((drv_tbls_list[table_id].max_index_num != 0) || (table_base != 0))
        {
            if ((table_base<<8)
                != ((drv_tbls_list[table_id].hw_data_base - ADDR_16K_DDR_MEM_START)/16))
            {
                //DRV_DBG_INFO("+++ %s table base cfg is not match with driver list!!!\n", table_name);
                //DRV_DBG_INFO("+++ %s table_base_cfg = 0x%x, driver_list_hwdatabase = 0x%x\n",
                //            table_name,
                //            table_base,
                //            drv_tbls_list[table_id].hw_data_base);
                DRV_DBG_INFO("+++ Match cfg should be = 0x%x\n",
                            ((drv_tbls_list[table_id].hw_data_base-ADDR_16K_DDR_MEM_START)/16)>>8);
                return DRV_E_INVALID_CFG_TBL_BASE;
            }
#if PRINTF_CHECK_PROCESS
            //DRV_DBG_INFO("$$$$ %s table_base check Pass!\n", table_name);
#endif
        }
        else
        {
#if PRINTF_CHECK_PROCESS
            //DRV_DBG_INFO("+++ %s table not allocate memory!\n", table_name);
#endif
        }
    }

    return DRV_E_NONE;
}

static int32
_drv_check_vlan_table_alloc_process(void)
{
    tables_t *tbl_info_ptr = NULL;
    ipe_ds_vlan_ctl_t ipe_ds_vlan_ctl;
    epe_next_hop_ctl_t epe_nexthop_ctl;

    uint32 hd_base = 0, sw_base = 0;
    uint32 cmd = 0;
    uint8 chip_id = 0;

    kal_memset(&ipe_ds_vlan_ctl, 0, sizeof(ipe_ds_vlan_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_DS_VLAN_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_ds_vlan_ctl));

    kal_memset(&epe_nexthop_ctl, 0, sizeof(epe_nexthop_ctl));
    cmd = DRV_IOR(IOC_REG, EPE_NEXT_HOP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &epe_nexthop_ctl));

    /* check DSVLAN table base */
    tbl_info_ptr = &drv_tbls_list[DS_VLAN];
    sw_base = ((tbl_info_ptr->hw_data_base - SHARED_DS_VLAN_MEM_OFFSET)/16)>>8;

    hd_base = ipe_ds_vlan_ctl.ds_vlan_table_base_bit19_to12<<4
                  | ipe_ds_vlan_ctl.ds_vlan_table_base_bit11_to8;

    if (sw_base != hd_base)
    {
#if PRINTF_CHECK_PROCESS
        DRV_DBG_INFO("$$$$ IPE_DS_VLAN_CTL's DSVLAN table base cfg is not match with drv_tbls_list[DS_VLAN]!\n");
#endif
        return DRV_E_INVALID_CFG_TBL_BASE;
    }

    hd_base = epe_nexthop_ctl.ds_vlan_table_base;

    if (sw_base != hd_base)
    {
#if PRINTF_CHECK_PROCESS
        DRV_DBG_INFO("$$$$ EPE_NEXT_HOP_CTL's DSVLAN table base cfg is not match with drv_tbls_list[DS_VLAN]!\n");
#endif
        return DRV_E_INVALID_CFG_TBL_BASE;
    }

    /* check DsVlanStatus table base */
    tbl_info_ptr = &drv_tbls_list[DS_VLAN_STATUS];
    sw_base = ((tbl_info_ptr->hw_data_base - SHARED_DS_VLAN_MEM_OFFSET)/16) >> 8;

    hd_base = ipe_ds_vlan_ctl.ds_vlan_status_table_base;

    if (sw_base != hd_base)
    {
#if PRINTF_CHECK_PROCESS
        DRV_DBG_INFO("$$$$ IPE_DS_VLAN_CTL's DsVlanStatus table base cfg is not match with drv_tbls_list[DsVlanStatus]!\n");
#endif
        return DRV_E_INVALID_CFG_TBL_BASE;
    }

    hd_base = epe_nexthop_ctl.ds_vlan_status_table_base;

    if (sw_base != hd_base)
    {
#if PRINTF_CHECK_PROCESS
        DRV_DBG_INFO("$$$$ EPE_NEXT_HOP_CTL's DsVlanStatus table base cfg is not match with drv_tbls_list[DsVlanStatus]!\n");
#endif
        return DRV_E_INVALID_CFG_TBL_BASE;
    }

    return DRV_E_NONE;
}


/**
 @brief check memory allocation process
*/
int32
drv_check_mem_allocation_process(void)
{
    int32 ret = DRV_E_NONE;

    /* check tcam allocation */
    ret = _drv_check_tcam_alloc_process();
    if (ret < 0)
    {
        KAL_LOG_ERROR("%% Check Tcam Memory allocation Fail!\n");
        return DRV_E_INVALID_TCAM_ALLOC;
    }


    /* check hash key allocation */
    ret = _drv_check_hash_table_alloc_process();
    if (ret < 0)
    {
        KAL_LOG_ERROR("%% Check Hash table Memory allocation Fail!\n");
        return DRV_E_INVALID_TCAM_ALLOC;
    }

    /* check other dynimic table allocation */
    ret = _drv_check_dynic_table_alloc_process();
    if (ret < 0)
    {
        KAL_LOG_ERROR("%% Check Dynic table Memory allocation Fail!\n");
        return DRV_E_INVALID_TCAM_ALLOC;
    }

    /* check other vlan table allocation */
    ret = _drv_check_vlan_table_alloc_process();
    if (ret < 0)
    {
        KAL_LOG_ERROR("%% Check vlan table Memory allocation Fail!\n");
        return DRV_E_INVALID_TCAM_ALLOC;
    }

    return DRV_E_NONE;

}


