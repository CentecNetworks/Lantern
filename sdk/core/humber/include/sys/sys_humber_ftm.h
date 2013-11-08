/**
 @file sys_humber_ftm.h

 @date 2009-11-03

 @version v2.0

 alloc  -memory initiate
*/

#ifndef _SYS_HUMBER_ALLOC_H
#define _SYS_HUMBER_ALLOC_H
#include "ctc_ftm.h"

#define INTERNAL_TCAM_TOTAL_ENTRY       (16 * 1024)
#define EXTERANL_TCAM_TOTAL_ENTRY       (256 * 1024)
#define BYTES_PER_TCAM_ENTRY            16
#define BYTES_PER_HASH_ENTRY            16

#define TABLE_BASE_OFFSET               8

#define INT_TCAM_DATA_BASE              0x11000000
#define INT_TCAM_MASK_BASE              0x12000000
#define EXT_TCAM_DATA_BASE              0x13000000
#define EXT_TCAM_MASK_BASE              0x14000000

#define EMBEDDED_SRAM_48K_BASE          0x18000000
#define EMBEDDED_SRAM_96K_BASE          0x16000000
#define SRAM_HASH_KEY_BASE              0x0d200000
#define SRAM_ASSOCIATE_DATA_BASE        0x041c0000
#define SRAM_VLAN_BASE                  0x0c480000

/*external sram/tcam*/
#define EXT_SRAM_ASSOCIATE_DATA_BASE     0x05000000
#define IBM_TCAM_BLOCK_SIZE             1024
#define IBM_TCAM_SUPER_BLOCK_SIZE       4096
#define IBM_TCAM_SUPER_BLOCK_NUM        4
#define NL9K_TCAM_BLOCK_SIZE            (4*1024)
#define NL9K_TCAM_SUPER_BLOCK_SIZE      (16*1024)
#define NL9K_TCAM_SUPER_BLOCK_NUM       16


#define ACLQOS_MAC_IPV4_DUAL_LOOKUP_LTR_ID  0
#define ACLQOS_IPV6_DUAL_LOOKUP_LTR_ID  1

#define DYNAMIC_TABLE_LIST_END          0

#define SYS_INT_TCAM_PHYSICAL_SIZE      (16*1024)
#define SYS_EXT_TCAM_PHYSICAL_SIZE      (256*1024)
#define SYS_HASH_48K_PHYSICAL_SIZE          (48*1024)
#define SYS_HASH_96K_PHYSICAL_SIZE          (96*1024)
#define SYS_VLAN_PHYSICAL_SIZE          (8*1024)
#define SYS_OAM_RESERVED_SIZE           256
#define SYS_INT_SRAM_PHYSICAL_SIZE      (112*1024)
#define SYS_EXT_SRAM_PHYSICAL_SIZE      (256*1024)

#define SYS_EXT_QDR_PHYSICAL_SIZE       (256*1024)
#define MAX_NL9K_TCAM_LTR_ID            32

 #define SYS_ALLOC_DBG_DUMP(FMT, ...)                          \
                    {                                                      \
                       CTC_DEBUG_OUT_DUMP(FMT,##__VA_ARGS__);\
                    }

 #define SYS_ALLOC_DBG_INFO(FMT, ...)                          \
                    {                                                      \
                       CTC_DEBUG_OUT_INFO(alloc, alloc, ALLOC_SYS,FMT,##__VA_ARGS__);\
                    }

#define SYS_ALLOC_DBG_FUNC()                          \
                    {\
                        CTC_DEBUG_OUT_FUNC(alloc, alloc, ALLOC_SYS);\
                    }

struct sys_table_info_s
{
    uint32 max_index;
    uint32 entry_size;
};
typedef struct sys_table_info_s sys_table_info_t;
struct sys_alloc_tbl_id_s
{
    ctc_ftm_key_type_t key_type;
    uint32 tcam_key_tbl_id;
    uint32 hash_key_tbl_id;
    uint32 associate_tbl_id;
    uint32 associate_tbl_id_rpf;
    char *tcam_key_id_str;
    char *hash_key_id_str;
    char *associate_tbl_id_str;
    char *associate_tbl_id_rpf_str;
};
typedef struct sys_alloc_tbl_id_s sys_alloc_tbl_id_t;

struct sys_alloc_lkp_info_s
{
    uint32 tcam_key_id;      /* ds_mac_key, etc. */
    uint32 hash_key_id;
    uint32 associate_id;
    uint16 alloc_key_type;
    uint8 tcam_tbl_id;      /* tbl_id in tcam entry */
    uint8 agingEn;
    uint32 lkp_ctl_field_id;
    uint32 lkp_rslt_ctl_field_id0;
    uint32 lkp_rslt_ctl_field_id1;

};
typedef struct sys_alloc_lkp_info_s sys_alloc_lkp_info_t;

struct sys_alloc_allocated_tcam_info_s
{
    uint8    ltr_id;
    uint8    is_ext_tcam;
    uint8   is_hash_used;
    uint8   entry_size;
    uint32  index_shift;
    uint32  index_base;
    uint32  table_id;
    uint32  max_index;
};
typedef struct sys_alloc_allocated_tcam_info_s sys_alloc_allocated_tcam_info_t;

struct sys_alloc_allocated_sram_info_s
{
    uint8    is_ext_sram;
    uint32  table_base;
};
typedef struct sys_alloc_allocated_sram_info_s sys_alloc_allocated_sram_info_t;
struct sys_alloc_tcam_info_s
{
    /* global used variables */
    uint16 start_block_id;              /* block/segment range of key, start block */
    uint16 end_block_id;                /* block/segment range of key, end block */

    /* NL9K register variables */
    uint16 ltr_id;                      /* logical table id of key */
    uint16 ltr_index;                   /* logical table register address index */

    /* IDT register variables */
    uint8 ss_sr_index;                  /* search scheme selece register 0-31 */
    uint8 database_select;              /* can suppport database0, database1, database2, database3 */
    uint8 output_select;                /* Now ASIC only support output0 or output1 or Both */
    uint8 is_ext_tcam;
    uint8 is_ext_tcam_valid;            /* if tcam key in mem_profile exist and valid, than this value is set*/
    uint8 blk_width;
};
typedef struct sys_alloc_tcam_info_s sys_alloc_tcam_info_t;

struct sys_alloc_info_s
{
    uint32 int_tcam_used_entry;
    uint32 ext_tcam_used_entry;
    uint32 hash_used_entry;
    uint8 is_aclqos_dual_lookup;
    uint8 is_merge_aclqos_mac_ip_key;
    uint8 is_hash_48ksize;
    uint8 is_ipuc_sa_enable;

    /* set by TCAM Allocation Process, use for NL9K Tcam Initiation. */
    sys_alloc_tcam_info_t tcam_info[CTC_FTM_KEY_TYPE_MAX];

    uint32 qos_group;                   /* record qos group info for IBM Tcam Initiation */
    uint32 met_table_base;              /* record met table beginning address. */
    uint32 nh_table_base;               /* record nexthop table beginning address. */
    uint32 l2edit_table_base;           /* record l2edit table beginning address. */
    uint32 l3edit_table_base;           /* record l3edit table beginning address. */
    uint32 current_sram_offset;         /* record Sram usage, which is shared by Associate and Dynamic Table */
    uint32 current_ext_sram_offset;
    uint32 sram_for_tcam_and_hash_num;  /* record tcam/hash sram  usage*/
    uint32 mpls_num;                    /* record tcam/hash sram  usage*/
    uint32 global_met_entry_num;
    uint32 local_met_dsfwd_entry_num;
    uint32 global_nh_entry_num;
    uint32 local_nh_entry_num;
    uint32 l2edit_entry_num;
    uint32 l3edit_entry_num;
    uint32 ip_tunnel_table_size;

    uint8 is_mpls_store_ext_sram;
    uint8 is_met_store_ext_sram;
    uint8 is_nh_store_ext_sram;
    uint8 is_l2edit_store_ext_sram;
    uint8 is_l3edit_store_ext_sram;
    uint8 is_ext_qdr_en;
    uint8 disable_merge_mac_ip_key_physical;
};
typedef struct sys_alloc_info_s sys_alloc_info_t;

extern int32
sys_humber_show_alloc_info(void);

extern int32
sys_alloc_get_table_entry_num(uint32 table_id, uint32 *entry_num);

extern int32
sys_alloc_get_ipucsa_enable(uint8 *enable);

extern int32
sys_alloc_get_table_address(uint32 table_id, uint32 *table_address);

extern int32
sys_alloc_get_met_dsfwd_table_info(uint32 *global_met_entry_num, uint32 *local_met_dsfwd_entry_num);

extern int32
sys_alloc_get_ip_tunnel_size(uint32 *ip_tunnel_size);

extern int32
sys_humber_mem_alloc(ctc_ftm_profile_info_t *profile_info);

sys_alloc_info_t *
sys_alloc_get_alloc_info_ptr(void);

extern int32
sys_alloc_get_dual_lookup_en(uint8 *dual_lkp_en, uint8 *merge_mackey_en);

extern int32
sys_alloc_get_sram_table_base(uint32    tbl_id, sys_alloc_allocated_sram_info_t* sram_info);
extern uint32
sys_alloc_get_sram_database(uint32     tbl_id);
extern int32
sys_alloc_get_tcam_key_alloc_info(uint32   tbl_id, sys_alloc_allocated_tcam_info_t *tcam_info);

extern int32
sys_alloc_get_ext_qdr_en(uint8* ext_qdr_en);

extern int32
sys_alloc_get_ext_sram_en(uint8* ext_sram_en);

extern int32
sys_alloc_get_is_hash_48k(uint8* hash_48k);

#endif










