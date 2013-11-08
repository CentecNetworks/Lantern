/**
  @file drv_tbl_reg.h

  @date 2010-07-22

  @version v5.1

  The file implement driver IOCTL defines and macros
*/

#ifndef _DRV_TBL_REG_H_
#define _DRV_TBL_REG_H_

#include "drv_common.h"
#include "drv_enum.h"

/**********************************************************************************
              Define Typedef, define and Data Structure
***********************************************************************************/
#define PRINTF_CHECK_PROCESS 0      /* printf button about check mem alloc process */

#define MAX_ENTRY_NUMBER   256
#define INDEX_BASE_OFFSET    8

#define DRV_INT_TCAM_DATA_ASIC_BASE 0x11000000
#define DRV_INT_TCAM_MASK_ASIC_BASE 0x12000000
#define DRV_INT_TCAM_MAX_ENTRY_NUM  (16 * 1024)

#define DRV_EXT_TCAM_DATA_ASIC_BASE 0x13000000
#define DRV_EXT_TCAM_MASK_ASIC_BASE 0x14000000
#define DRV_EXT_TCAM_MAX_ENTRY_NUM  (256 * 1024)

/* check chip id valid */
extern uint8 drv_init_chip_num;
#define DRV_CHIP_ID_VALID_CHECK(chip_id) \
if ((drv_init_chip_num) <= (chip_id))\
{\
    DRV_DBG_INFO("\nERROR! INVALID ChipID! chip_num is %d, chipid: %d, file:%s line:%d function:%s\n",drv_init_chip_num,chip_id,__FILE__,__LINE__,__FUNCTION__);\
    return DRV_E_INVALID_CHIP;\
}

#define TBL_ID_VALID(tbl_id)\
((tbl_id >= 0) && (tbl_id < (MAX_TBL_NUM)) && \
     (DRV_TBL_GET_INFOPTR(tbl_id) != NULL))\

#define REG_ID_VALID(reg_id)\
    ((reg_id >= 0) && (reg_id < (MAX_REG_NUM)) && \
         (DRV_REG_GET_INFOPTR(reg_id) != NULL))\

/* check table id valid */
#define DRV_TBL_ID_VALID_CHECK(tbl_id) \
if ((tbl_id) >= (MAX_TBL_NUM))\
{\
    DRV_DBG_INFO("\nERROR! INVALID TblID! TblID: %d, file:%s line:%d function:%s\n",tbl_id,__FILE__,__LINE__,__FUNCTION__);\
    return DRV_E_INVALID_TBL;\
}

/* check register id valid */
#define DRV_REG_ID_VALID_CHECK(reg_id) \
if ((reg_id) >= (MAX_REG_NUM))\
{\
    DRV_DBG_INFO("\nERROR! INVALID RegID! regid: %d, file:%s line:%d function:%s\n",reg_id,__FILE__,__LINE__,__FUNCTION__);\
    return DRV_E_INVALID_REG;\
}

/* check field id */
#define DRV_FIELD_ID_VALID_CHECK(field_id)\
if(MAX_FIELD_NUM <= (field_id))\
{\
    DRV_DBG_INFO("\nERROR! INVALID FieldID! fieldid: %d, file:%s line:%d function:%s\n",field_id,__FILE__,__LINE__,__FUNCTION__);\
    return DRV_E_INVALID_FLD;\
}

/* get table infomation pointer */
#define DRV_TBL_GET_INFOPTR(tbl_id)        (&drv_tbls_list[tbl_id])
#define DRV_TBL_GET_INFO(tbl_id)           (drv_tbls_list[tbl_id])
#define DRV_TBL_ENTRY_SIZE(tbl_id)	       (DRV_TBL_GET_INFO(tbl_id).entry_size)
#define DRV_TBL_ENTRY_OFFSET_GET(tbl_id)   (DRV_TBL_GET_INFO(tbl_id).key_size)
#define DRV_TBL_MAX_INDEX(tbl_id)          (DRV_TBL_GET_INFO(tbl_id).max_index_num)

/* get register infomation pointer */
#define DRV_REG_GET_INFOPTR(reg_id)	       (&drv_regs_list[reg_id])
#define DRV_REG_GET_INFO(reg_id)		   (drv_regs_list[reg_id])
#define DRV_REG_ENTRY_SIZE(reg_id)         (DRV_REG_GET_INFO(reg_id).entry_size)
#define DRV_REG_MAX_INDEX(reg_id)          (DRV_REG_GET_INFO(reg_id).max_index_num)


/* OAM HASH and IPE LKP HASH is not the same at realize method, Need to think!!!*/
#define DRV_HASH_KEY_TBL_ID_VALID_CHECK(tbl_id)        \
if ((DS_MAC_HASH_KEY0 != (tbl_id))                     \
    && (DS_MAC_HASH_KEY1 != (tbl_id))                  \
    && (DS_IPV4_UCAST_HASH_KEY0 != (tbl_id))           \
    && (DS_IPV4_UCAST_HASH_KEY1 != (tbl_id))           \
    && (DS_IPV4_MCAST_HASH_KEY0 != (tbl_id))           \
    && (DS_IPV4_MCAST_HASH_KEY1 != (tbl_id))           \
    && (DS_IPV6_UCAST_HASH_KEY0 != (tbl_id))           \
    && (DS_IPV6_UCAST_HASH_KEY1 != (tbl_id))           \
    && (DS_IPV6_MCAST_HASH_KEY0 != (tbl_id))           \
    && (DS_IPV6_MCAST_HASH_KEY1 != (tbl_id))           \
    && (DS_ETH_OAM_HASH_KEY0 != (tbl_id))              \
    && (DS_ETH_OAM_HASH_KEY1 != (tbl_id))              \
    && (DS_PBT_OAM_HASH_KEY0 != (tbl_id))              \
    && (DS_PBT_OAM_HASH_KEY1 != (tbl_id))              \
    && (DS_MPLS_OAM_LABEL_HASH_KEY0 != (tbl_id))       \
    && (DS_MPLS_OAM_LABEL_HASH_KEY1 != (tbl_id))       \
    && (DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0 != (tbl_id))   \
    && (DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1 != (tbl_id))   \
    && (DS_ETH_OAM_RMEP_HASH_KEY0 != (tbl_id))         \
    && (DS_ETH_OAM_RMEP_HASH_KEY1 != (tbl_id)))        \
{\
    DRV_DBG_INFO("\nERROR! INVALID Hash Key TblID! TblID: %d, file:%s line:%d function:%s\n",tbl_id,__FILE__,__LINE__,__FUNCTION__);\
    return DRV_E_INVALID_TBL;\
}

/* Check whether the table is tcam key or not */
#define DRV_SRAM_IS_TCAM_KEY(tbl_id)       \
         ((DS_MAC_KEY == (tbl_id))            \
         ||(DS_ACL_MAC_KEY == (tbl_id))      \
         ||(DS_ACL_IPV4_KEY == (tbl_id))     \
         ||(DS_ACL_MPLS_KEY == (tbl_id))     \
         ||(DS_ACL_IPV6_KEY == (tbl_id))     \
         ||(DS_QOS_MAC_KEY == (tbl_id))      \
         ||(DS_QOS_IPV4_KEY == (tbl_id))     \
         ||(DS_QOS_MPLS_KEY == (tbl_id))     \
         ||(DS_QOS_IPV6_KEY == (tbl_id))     \
         ||(DS_IPV4_UCAST_ROUTE_KEY == (tbl_id))     \
         ||(DS_IPV4_MCAST_ROUTE_KEY == (tbl_id))     \
         ||(DS_IPV6_UCAST_ROUTE_KEY == (tbl_id))     \
         ||(DS_IPV6_MCAST_ROUTE_KEY == (tbl_id))     \
         ||(DS_IPV4_NAT_KEY == (tbl_id))          \
         ||(DS_IPV4_PBR_DUALDA_KEY == (tbl_id))   \
         ||(DS_USER_ID_IPV4_KEY == (tbl_id))      \
         ||(DS_USER_ID_IPV6_KEY == (tbl_id))      \
         ||(DS_USER_ID_MAC_KEY == (tbl_id))       \
         ||(DS_USER_ID_VLAN_KEY == (tbl_id))      \
         ||(DS_IPV6_NAT_KEY == (tbl_id))          \
         ||(DS_IPV6_PBR_DUALDA_KEY == (tbl_id))   \
         ||(DS_ETH_OAM_KEY == (tbl_id))           \
         ||(DS_ETH_OAM_RMEP_KEY == (tbl_id))      \
         ||(DS_MPLS_OAM_LABEL_KEY == (tbl_id))    \
         ||(DS_MPLS_OAM_IPV4_TTSI_KEY == (tbl_id))   \
         ||(DS_PBT_OAM_KEY == (tbl_id)))

/* Check whether the table is nexthop share table or not */
#define DRV_SRAM_IS_NEXTHOP_SHARE_TBL(tbl_id)   \
         ((DS_NEXTHOP8W == (tbl_id))               \
         ||(DS_NEXTHOP == (tbl_id)))

/* Check whether the table is layer2 edit share table or not */
#define DRV_SRAM_IS_L2EDIT_SHARE_TBL(tbl_id)   \
       ((DS_L2_EDIT_ETH4W == (tbl_id))       \
       || (DS_L2_EDIT_ETH8W == (tbl_id))     \
       || (DS_L2_EDIT_FLEX4W == (tbl_id))    \
       || (DS_L2_EDIT_FLEX8W == (tbl_id))    \
       || (DS_L2_EDIT_PBB4W == (tbl_id))     \
       || (DS_L2_EDIT_PBB8W == (tbl_id))     \
       || (DS_L2_EDIT_LOOPBACK == (tbl_id)))

/* Check whether the table is layer3 edit share table or not */
#define DRV_SRAM_IS_L3EDIT_SHARE_TBL(tbl_id)  \
        ((DS_L3EDIT_MPLS4W == (tbl_id))         \
        || (DS_L3EDIT_MPLS8W == (tbl_id))       \
        || (DS_L3EDIT_NAT4W == (tbl_id))        \
        || (DS_L3EDIT_NAT8W == (tbl_id))        \
        || (DS_L3EDIT_TUNNEL_V4 == (tbl_id))    \
        || (DS_L3EDIT_TUNNEL_V6 == (tbl_id))    \
        || (DS_L3EDIT_FLEX == (tbl_id))         \
        || (DS_L3EDIT_LOOP_BACK == (tbl_id)))

#define DRV_SRAM_IS_OAM_CHAN_TBL(tbl_id)         \
         ((DS_ETH_OAM_CHAN == (tbl_id))            \
         ||(DS_MPLS_PBT_OAM_CHAN == (tbl_id))      \
         ||(DS_RMEP_CHAN == (tbl_id))              \
         ||(DS_MEP_CHAN_TABLE == (tbl_id)))

#define DRV_SRAM_IS_OAM_MEP_TBL(tbl_id)         \
         ((DS_ETH_MEP == (tbl_id))                \
         ||(DS_ETH_RMEP == (tbl_id))              \
         ||(DS_MPLS_MEP == (tbl_id))             \
         ||(DS_MPLS_RMEP == (tbl_id)))


#define FIELD_STR_DSCP 1

/* field data stucture */
struct fields_s
{
/*
    field_id_t field_id;
*/
    uint32   len;
    uint8    word_offset;	/* if entry_size = 8bytes, word_offset = 0 or 1*/
    uint8    bit_offset;    /* bit offset in one word of a entry, range 0~31 */
#ifdef FIELD_STR_DSCP
    char*    field_name;
#else
#endif

};
typedef struct fields_s fields_t;

/* table data stucture */
struct tables_s
{
    uint32	 hw_mask_base;
    uint32	 hw_data_base;
    uint32 	 max_index_num;

    /* Only use for tcam key table,
       then entry_size means the full key size,
       and key_size shows the allocated tcam key length. (unit: Byte)*/
    uint8  	 key_size;

    uint8  	 entry_size;
	uint8	 num_fields;
	fields_t* ptr_fields;
};
typedef struct tables_s  tables_t;

/* register data stucture */
struct registers_s
{
	uint32	 hw_data_base;
	uint32	 max_index_num;
	uint16	 entry_size;
	uint8	 num_fields;
	fields_t* ptr_fields;
};
typedef struct registers_s  registers_t;


/******** Driver dynic table allocation use ********/
#define ADDR_16K_DDR_MEM_START  0x41C0000
#define SIZE_16K_DDR_MEM        (16 * 1024)
#define ADDR_96K_DDR_MEM_START  0x4200000
#define SIZE_96K_DDR_MEM        (96 * 1024)
#define ADDR_48K_DDR_MEM_START  0x4200000
#define SIZE_48K_DDR_MEM        (48 * 1024)
#define ADDR_256K_DDR_MEM_START 0x5000000
#define SIZE_256K_DDR_MEM       (256 * 1024)

/* define DDR memory type */
enum ddr_mem_type
{
    DDR_MEM_INNER = 0,
    DDR_MEM_EXT = 1
};
typedef enum ddr_mem_type ddr_mem_type_e;

/* dynimic table allocation infomation structure */
struct drv_dynic_tbl_alloc_info_s
{
    uint32 entry_num;
    tbl_id_t dynic_tbl_id;
    ddr_mem_type_e mem_type;
    uint32 entry_size;
    bool is_acl_v4_mac_tbl;
    bool is_qos_v4_mac_tbl;
    bool is_l2edit_tbl;
    bool is_l3edit_tbl;
    bool is_nexthop_tbl;
};
typedef struct drv_dynic_tbl_alloc_info_s drv_dynic_tbl_alloc_info_t;

/* tcam table allocation infomation structure */
struct drv_tcam_alloc_info_s
{
    uint32 entry_num;
    tbl_id_t key_tbl_id;
    bool in_embeded;
    uint32 key_size;
    uint32 full_key_size;
    bool is_acl_v4_mac_tbl;
    bool is_qos_v4_mac_tbl;
    bool is_oam_tbl;
};
typedef struct drv_tcam_alloc_info_s drv_tcam_alloc_info_t;

/**********************************************************************************
                      Define external transfer var
***********************************************************************************/
/* sdk has a global pointer arry for per-chip */
extern registers_t drv_regs_list[MAX_REG_NUM];
extern tables_t drv_tbls_list[MAX_TBL_NUM];

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @brief get byte order type by testing methord
*/
extern int32
drv_preinit(void);


/**
 @brief according to table field id to find the field property
*/
extern fields_t*
drv_tbl_field_find(tbl_id_t tbl_id, fld_id_t field_id);


/**
 @brief according to register field id to find the field property
*/
extern fields_t*
drv_reg_field_find(reg_id_t reg_id, fld_id_t field_id);


/**
 @brief set a field of a table data entry in memory
*/
extern int32
drv_tbl_field_set(tbl_id_t tbl_id, fld_id_t field_id,
                  uint32* entry, uint32 value);


/**
 @brief get a field of a table data entry in memory
*/
extern int32
drv_tbl_field_get(tbl_id_t tbl_id, fld_id_t field_id,
                  uint32* entry, uint32* value);


/**
 @brief set a field of a register data entry in memory
*/
extern int32
drv_reg_field_set(reg_id_t reg_id, fld_id_t field_id,
                  uint32* entry, uint32 value);


/**
 @brief get a field of a register data entry in memory
*/
extern int32
drv_reg_field_get(reg_id_t reg_id, fld_id_t field_id,
                  uint32* entry, uint32* value);


/**
 @brief Get a field word offset of a register data entry
*/
extern int32
drv_reg_field_get_word_offset(reg_id_t reg_id, fld_id_t field_id, uint32* word_offset);


/**
 @brief register a table to the table directory
*/
extern int32
drv_tbl_register(tbl_id_t tbl_id, uint32 mask_offset,
                 uint32 data_offset, uint32 max_idx, uint8 entry_size,
                 uint8 key_size, uint8 num_f, fields_t* ptr_f);


/**
 @brief register a reg to the register directory
*/
extern int32
drv_reg_register(reg_id_t reg_id, uint32 data_offset, uint32 max_idx,
                 uint16 entry_size, uint8 num_f, fields_t* ptr_f);

/**
 @brief cmodel cli use to init chip number
*/
extern int32
drv_init_humber_chipnum(uint8 chip_num);


/**
 @brief check memory allocation process
*/
extern int32
drv_check_mem_allocation_process(void);


/**
 @brief
*/
extern int32
drv_sram_reg_ds_to_entry(reg_id_t reg_id, void* ds, uint32* entry);

/**
 @brief
*/
extern int32
drv_sram_reg_entry_to_ds(reg_id_t reg_id, uint32* entry, void* ds);

/**
 @brief
*/
extern int32
drv_sram_tbl_ds_to_entry(tbl_id_t tbl_id, void* ds, void* entry);

/**
 @brief
*/
extern int32
drv_sram_tbl_entry_to_ds(tbl_id_t tbl_id, void* entry, void* ds);

/**
 @brief
*/
extern int32
drv_tcam_tbl_ds_to_entry(tbl_id_t tbl_id, void* ds, void* entry);

/**
 @brief
*/
extern int32
drv_tcam_tbl_entry_to_ds(tbl_id_t tbl_id, void* entry, void* ds);

#endif  /*end of _DRV_TBL_REG_H_*/

