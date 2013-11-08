/**
 @file sys_humber_alloc.c

 @date 2009-11-12

 @version v2.0

 This file provides all sys alloc function
*/
#include "kal.h"

#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_ftm.h"
#include "ctc_debug.h"
#include "sys_humber_ftm.h"
#include "sys_humber_chip.h"

#include "drv_enum.h"
#include "drv_cfg.h"
#include "drv_tbl_reg.h"
#include "drv_humber.h"
#include "drv_io.h"
#include "drv_humber_data_path.h"

struct sys_tcam_key_info_s
{
    uint32 key_id;      /* ds_mac_key, etc. */
    uint32 internal_key_type;      /* ds_mac_key, etc. */
    uint32 table_id;

};
typedef struct sys_tcam_key_info_s sys_tcam_key_info_t;
/* check table id valid */
#define SYS_TBL_ID_VALID_CHECK(tbl_id) \
if (MAX_TBL_NUM <= (tbl_id))\
{\
    SYS_ALLOC_DBG_INFO("\nERROR! INVALID TblID! TblID: %d, file:%s line:%d function:%s\n",tbl_id,__FILE__,__LINE__,__FUNCTION__);\
    return CTC_E_INVALID_TBL;\
}

/* check register id valid */
#define SYS_REG_ID_VALID_CHECK(reg_id) \
if (MAX_REG_NUM <= (reg_id))\
{\
    SYS_ALLOC_DBG_INFO("\nERROR! INVALID RegID! regid: %d, file:%s line:%d function:%s\n",reg_id,__FILE__,__LINE__,__FUNCTION__);\
    return CTC_E_INVALID_REG;\
}

/* Global Variable */
sys_alloc_info_t alloc_info;


sys_alloc_tbl_id_t sys_alloc_tbl_id_list[CTC_FTM_KEY_TYPE_MAX] =
{
{CTC_FTM_KEY_TYPE_IPV6_UCAST, DS_IPV6_UCAST_ROUTE_KEY,DS_IPV6_UCAST_HASH_KEY0,DS_IPV6_UCAST_DA,         DS_IPV6_UCAST_SA,  "DS_IPV6_UCAST_ROUTE_KEY","DS_IPV6_UCAST_HASH_KEY0","DS_IPV6_UCAST_DA",         "DS_IPV6_UCAST_SA"},
{CTC_FTM_KEY_TYPE_ACL_IPV6,         DS_ACL_IPV6_KEY,        0,                      DS_IPV6_ACL,              0,                 "DS_ACL_IPV6_KEY",        NULL,                     "DS_IPV6_ACL",              NULL},
{CTC_FTM_KEY_TYPE_ACL_MAC_IPV4,     DS_ACL_IPV4_KEY,        0,                      DS_IPV4_ACL,              0,                 "DS_ACL_IPV4_KEY",        NULL,                     "DS_IPV4_ACL",              NULL},
{CTC_FTM_KEY_TYPE_IPV4_UCAST, DS_IPV4_UCAST_ROUTE_KEY,DS_IPV4_UCAST_HASH_KEY0,DS_IPV4_UCAST_DA,         DS_IPV4_UCAST_SA,  "DS_IPV4_UCAST_ROUTE_KEY","DS_IPV4_UCAST_HASH_KEY0","DS_IPV4_UCAST_DA",         "DS_IPV4_UCAST_SA"},
{CTC_FTM_KEY_TYPE_IPV4_MCAST, DS_IPV4_MCAST_ROUTE_KEY,DS_IPV4_MCAST_HASH_KEY0,DS_IPV4_MCAST_DA,         DS_IPV4_MCAST_RPF, "DS_IPV4_MCAST_ROUTE_KEY","DS_IPV4_MCAST_HASH_KEY0","DS_IPV4_MCAST_DA",         "DS_IPV4_MCAST_RPF"},
{CTC_FTM_KEY_TYPE_IPV4_NAT,         DS_IPV4_NAT_KEY,        0,                      DS_IPV4_SA_NAT,           0,                 "DS_IPV4_NAT_KEY",        NULL,                     "DS_IPV4_SA_NAT",           NULL},
{CTC_FTM_KEY_TYPE_IPV4_PBR,  DS_IPV4_PBR_DUALDA_KEY, 0,                      DS_IPV4_UCAST_PBR_DUAL_DA,0,                 "DS_IPV4_PBR_DUALDA_KEY", NULL,                     "DS_IPV4_UCAST_PBR_DUAL_DA",NULL},
{CTC_FTM_KEY_TYPE_USERID_MAC,      DS_USER_ID_MAC_KEY,     0,                      DS_USER_ID_MAC,           0,                 "DS_USER_ID_MAC_KEY",     NULL,                     "DS_USER_ID_MAC",           NULL},
{CTC_FTM_KEY_TYPE_USERID_VLAN,     DS_USER_ID_VLAN_KEY,    0,                      DS_USER_ID_VLAN,          0,                 "DS_USER_ID_VLAN_KEY",    NULL,                     "DS_USER_ID_VLAN",          NULL},
{CTC_FTM_KEY_TYPE_IPV6_MCAST, DS_IPV6_MCAST_ROUTE_KEY,DS_IPV6_MCAST_HASH_KEY0,DS_IPV6_MCAST_DA,         DS_IPV6_MCAST_RPF, "DS_IPV6_MCAST_ROUTE_KEY","DS_IPV6_MCAST_HASH_KEY0","DS_IPV6_MCAST_DA",         "DS_IPV6_MCAST_RPF"},
{CTC_FTM_KEY_TYPE_IPV6_NAT,         DS_IPV6_NAT_KEY,        0,                      DS_IPV6_SA_NAT,           0,                 "DS_IPV6_NAT_KEY",        NULL,                     "DS_IPV6_SA_NAT",           NULL},
{CTC_FTM_KEY_TYPE_FIB,              DS_MAC_KEY,             DS_MAC_HASH_KEY0,       DS_MAC,                   0,                 "DS_MAC_KEY",             "DS_MAC_HASH_KEY0",       "DS_MAC",                   NULL},
{CTC_FTM_KEY_TYPE_IPV6_PBR,  DS_IPV6_PBR_DUALDA_KEY, 0,                      DS_IPV6_UCAST_PBR_DUAL_DA,0,                 "DS_IPV6_PBR_DUALDA_KEY", NULL,                     "DS_IPV6_UCAST_PBR_DUAL_DA",NULL},
{CTC_FTM_KEY_TYPE_USERID_IPV4,     DS_USER_ID_IPV4_KEY,    0,                      DS_USER_ID_IPV4,          0,                 "DS_USER_ID_IPV4_KEY",    NULL,                     "DS_USER_ID_IPV4",          NULL},
{CTC_FTM_KEY_TYPE_USERID_IPV6,     DS_USER_ID_IPV6_KEY,    0,                      DS_USER_ID_IPV6,          0,                 "DS_USER_ID_IPV6_KEY",    NULL,                     "DS_USER_ID_IPV6",          NULL},
{CTC_FTM_KEY_TYPE_OAM,              DS_ETH_OAM_KEY,         0,                      0,                        0,                 "DS_ETH_OAM_KEY",         NULL,                     NULL,                       NULL},
{CTC_FTM_KEY_TYPE_QOS_MAC_IPV4,     DS_QOS_IPV4_KEY,        0,                      DS_IPV4_QOS,              0,                 "DS_QOS_IPV4_KEY",        NULL,                     "DS_IPV4_QOS",              NULL},
{CTC_FTM_KEY_TYPE_QOS_IPV6,         DS_QOS_IPV6_KEY,        0,                      DS_IPV6_QOS,              0,                 "DS_QOS_IPV6_KEY",        NULL,                     "DS_IPV6_QOS",              NULL},
{0,0,0,0,0,0,0}
};

int32
sys_humber_show_alloc_info(void)
{
    uint32 key_type = 0;
    uint32 tcam_key_tbl_id = 0;
    uint32 associate_tbl_id = 0;
    uint32 associate_tbl_id_rpf = 0;
    uint32 hash_key_tbl_id = 0;
    uint32 max_index = 0;
    uint32 associate_max_index = 0;
    uint32 associate_rpf_max_index = 0;
    uint32 key_size = 0;
    uint32 table_size = 0;
    uint32 rpf_table_size = 0;
    uint32 total_entry, in_use_entry, not_in_use_entry;
    uint32 key_entry_rang_from = 0;
    uint32 key_entry_rang_to = 0;
    uint32 data_entry_rang_from = 0;
    uint32 data_entry_rang_to = 0;
    uint32 data_rpf_entry_rang_from = 0;
    uint32 data_rpf_entry_rang_to = 0;

    char *tcam_key_id_str;
    char *hash_key_id_str;
    char *associate_tbl_id_str;
    char *associate_tbl_id_rpf_str;
    tables_t *table_info_ptr = NULL;
    tables_t *associate_tbl_info_ptr = NULL;
    tables_t *associate_rpf_tbl_info_ptr = NULL;

    /* Show internal Tcam alloc info */
    total_entry         = SYS_INT_TCAM_PHYSICAL_SIZE;
    in_use_entry        = alloc_info.int_tcam_used_entry;
    not_in_use_entry    = total_entry - in_use_entry;
    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Internal Tcam                                       %-d\n", total_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  In Use                                              %-d\n", in_use_entry);
    SYS_ALLOC_DBG_DUMP("  Free                                                %-d\n", not_in_use_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Key Type             Key Size  Max Idx  Entry Range   Associated Table    Table Size  Max Idx  Entry Range\n");
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    for(key_type = 0; key_type < CTC_FTM_KEY_TYPE_MAX; key_type++)
    {
        tcam_key_tbl_id             = sys_alloc_tbl_id_list[key_type].tcam_key_tbl_id;
        associate_tbl_id            = sys_alloc_tbl_id_list[key_type].associate_tbl_id;
        associate_tbl_id_rpf        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf;
        tcam_key_id_str             = sys_alloc_tbl_id_list[key_type].tcam_key_id_str;
        associate_tbl_id_str        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_str;
        associate_tbl_id_rpf_str    = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf_str;

        if (NULL == tcam_key_id_str)
        {
            continue;
        }

        table_info_ptr      = &drv_tbls_list[tcam_key_tbl_id];
        /*ext tcam*/
        if(table_info_ptr->hw_data_base >= EXT_TCAM_DATA_BASE)
        {
            continue;
        }
        max_index           = table_info_ptr->max_index_num;
        key_size            = table_info_ptr->key_size;
        key_size            = (key_size / 16) * 80;


        if (0 != associate_tbl_id)
        {
            associate_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id];
            if (associate_tbl_info_ptr->hw_data_base >= EXT_SRAM_ASSOCIATE_DATA_BASE)
            {
                /* associated data adress is wrong */
                SYS_ALLOC_DBG_INFO("Tcam: Associated table adress is wrong!\n");
                return CTC_E_UNEXPECT;
            }
            associate_max_index    = associate_tbl_info_ptr->max_index_num;
            table_size            = associate_tbl_info_ptr->entry_size;
            table_size            = (table_size / 16) * 80;
        }

        if (0 != associate_tbl_id_rpf)
        {
            associate_rpf_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id_rpf];
            if (associate_rpf_tbl_info_ptr->hw_data_base >= EXT_SRAM_ASSOCIATE_DATA_BASE)
            {
                /* associated rpf data adress is wrong */
                SYS_ALLOC_DBG_INFO("Tcam: Associated rpf table adress is wrong!\n");
                return CTC_E_UNEXPECT;
            }
            associate_rpf_max_index    = associate_rpf_tbl_info_ptr->max_index_num;
            rpf_table_size            = associate_rpf_tbl_info_ptr->entry_size;
            rpf_table_size            = (rpf_table_size / 16) * 80;
        }

        /* Tcam Key info */
        if (0 == max_index)
        {
            key_entry_rang_from = 0;
            key_entry_rang_to = 0;
        }
        else
        {
            key_entry_rang_from = (table_info_ptr->hw_data_base - INT_TCAM_DATA_BASE) / 16;
            key_entry_rang_to = key_entry_rang_from + max_index * (table_info_ptr->key_size / 16) - 1;
        }

        /* associated data table info */
        if (0 == associate_max_index || 0 == associate_tbl_id)
        {
            data_entry_rang_from = 0;
            data_entry_rang_to = 0;
        }
        else
        {
            data_entry_rang_from = (associate_tbl_info_ptr->hw_data_base - SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_entry_rang_to = data_entry_rang_from + associate_max_index - 1;
        }

        /* rpf associated data table info */
        if (0 == associate_rpf_max_index || 0 == associate_tbl_id_rpf)
        {
            data_rpf_entry_rang_from = 0;
            data_rpf_entry_rang_to = 0;
        }
        else
        {
            data_rpf_entry_rang_from = (associate_rpf_tbl_info_ptr->hw_data_base - SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_rpf_entry_rang_to = data_rpf_entry_rang_from + associate_rpf_max_index - 1;
        }

        if (0 != max_index)
        {
            SYS_ALLOC_DBG_DUMP("  %-25s%4d%8d%8x~%-8x", tcam_key_id_str, key_size, max_index, key_entry_rang_from, key_entry_rang_to);
            if (0 != associate_tbl_id)
            {
                SYS_ALLOC_DBG_DUMP("%-25s%5d%8d%8x~%-8x\n", associate_tbl_id_str, table_size, associate_max_index, data_entry_rang_from, data_entry_rang_to);
            } else {
                SYS_ALLOC_DBG_DUMP("\n");
            }
            if (0 != associate_tbl_id_rpf)
            {
                SYS_ALLOC_DBG_DUMP("  %54s%-25s%5d%8d%8x~%-8x\n"," ",\
                    associate_tbl_id_rpf_str, rpf_table_size, associate_rpf_max_index, data_rpf_entry_rang_from, data_rpf_entry_rang_to);
            }
        }
    }

    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    /*Show external Tcam alloc info*/
    total_entry         = SYS_EXT_TCAM_PHYSICAL_SIZE;
    in_use_entry        = alloc_info.ext_tcam_used_entry;
    not_in_use_entry    = total_entry - in_use_entry;
    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  External Tcam                                       %-d\n", total_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  In Use                                              %-d\n", in_use_entry);
    SYS_ALLOC_DBG_DUMP("  Free                                                %-d\n", not_in_use_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Key Type             Key Size  Max Idx  Entry Range   Associated Table    Table Size  Max Idx  Entry Range\n");
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    for(key_type = 0; key_type < CTC_FTM_KEY_TYPE_MAX; key_type++)
    {
        tcam_key_tbl_id             = sys_alloc_tbl_id_list[key_type].tcam_key_tbl_id;
        associate_tbl_id            = sys_alloc_tbl_id_list[key_type].associate_tbl_id;
        associate_tbl_id_rpf        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf;
        tcam_key_id_str             = sys_alloc_tbl_id_list[key_type].tcam_key_id_str;
        associate_tbl_id_str        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_str;
        associate_tbl_id_rpf_str    = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf_str;

        if (NULL == tcam_key_id_str)
        {
            continue;
        }


        table_info_ptr      = &drv_tbls_list[tcam_key_tbl_id];
        /*int tcam*/
        if((table_info_ptr->hw_data_base < EXT_TCAM_DATA_BASE)&&(table_info_ptr->hw_data_base >= INT_TCAM_DATA_BASE))
        {
            continue;
        }
        max_index           = table_info_ptr->max_index_num;
        key_size            = table_info_ptr->key_size;
        key_size            = (key_size / 16) * 80;

        if (0 != associate_tbl_id)
        {
            associate_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id];
            if (associate_tbl_info_ptr->hw_data_base < EXT_SRAM_ASSOCIATE_DATA_BASE\
                && associate_tbl_info_ptr->hw_data_base >= SRAM_ASSOCIATE_DATA_BASE)
            {
                /* Ext Tcam key associated data adress is wrong */
                SYS_ALLOC_DBG_INFO("Ext Tcam: Associated table adress is wrong!\n");
                return CTC_E_UNEXPECT;
            }
            associate_max_index    = associate_tbl_info_ptr->max_index_num;
            table_size            = associate_tbl_info_ptr->entry_size;
            table_size            = (table_size / 16) * 80;
        }

        if (0 != associate_tbl_id_rpf)
        {
            associate_rpf_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id_rpf];
            if (associate_rpf_tbl_info_ptr->hw_data_base < EXT_SRAM_ASSOCIATE_DATA_BASE\
                && associate_rpf_tbl_info_ptr->hw_data_base >= SRAM_ASSOCIATE_DATA_BASE)
            {
                /* Ext Tcam Key associated rpf data adress is wrong */
                SYS_ALLOC_DBG_INFO("Ext Tcam: Associated rpf table adress is wrong!\n");
                return CTC_E_UNEXPECT;
            }
            associate_rpf_max_index    = associate_rpf_tbl_info_ptr->max_index_num;
            rpf_table_size            = associate_rpf_tbl_info_ptr->entry_size;
            rpf_table_size            = (rpf_table_size / 16) * 80;
        }

        /* Tcam Key info */
        if (0 == max_index)
        {
            key_entry_rang_from = 0;
            key_entry_rang_to = 0;
        }
        else
        {
            key_entry_rang_from = (table_info_ptr->hw_data_base - EXT_TCAM_DATA_BASE) / 16;
            key_entry_rang_to = key_entry_rang_from + max_index * (table_info_ptr->key_size / 16) - 1;
        }

        /* associated data table info */
        if (0 == associate_max_index || 0 == associate_tbl_id)
        {
            data_entry_rang_from = 0;
            data_entry_rang_to = 0;
        }
        else
        {
            data_entry_rang_from = (associate_tbl_info_ptr->hw_data_base - EXT_SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_entry_rang_to = data_entry_rang_from + associate_max_index - 1;
        }

        /* rpf associated data table info */
        if (0 == associate_rpf_max_index || 0 == associate_tbl_id_rpf)
        {
            data_rpf_entry_rang_from = 0;
            data_rpf_entry_rang_to = 0;
        }
        else
        {
            data_rpf_entry_rang_from = (associate_rpf_tbl_info_ptr->hw_data_base - EXT_SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_rpf_entry_rang_to = data_rpf_entry_rang_from + associate_rpf_max_index - 1;
        }


        if (0 != max_index)
        {
            SYS_ALLOC_DBG_DUMP("  %-25s%4d%8d%8x~%-8x", tcam_key_id_str, key_size, max_index, key_entry_rang_from, key_entry_rang_to);
            if (0 != associate_tbl_id)
            {
                SYS_ALLOC_DBG_DUMP("%-25s%5d%8d%8x~%-8x\n", associate_tbl_id_str, table_size, associate_max_index, data_entry_rang_from, data_entry_rang_to);
            } else {
                SYS_ALLOC_DBG_DUMP("\n");
            }
            if (0 != associate_tbl_id_rpf)
            {
                SYS_ALLOC_DBG_DUMP("  %54s%-25s%5d%8d%8x~%-8x\n"," ",\
                    associate_tbl_id_rpf_str, rpf_table_size, associate_rpf_max_index, data_rpf_entry_rang_from, data_rpf_entry_rang_to);
            }
        }

    }

    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    /* Show Hash alloc info */
    if(alloc_info.is_hash_48ksize)
    {
        total_entry = SYS_HASH_48K_PHYSICAL_SIZE;
    }
    else
    {
        total_entry = SYS_HASH_96K_PHYSICAL_SIZE;
    }

    in_use_entry        = alloc_info.hash_used_entry;
    not_in_use_entry    = total_entry - in_use_entry;

    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Hash Key                                            %-d\n", total_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  In Use                                              %-d\n", in_use_entry);
    SYS_ALLOC_DBG_DUMP("  Free                                                %-d\n", not_in_use_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Key Type             Key Size  Max Idx  Entry Range   Associated Table    Table Size  Max Idx  Entry Range\n");
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    for(key_type = 0; key_type < CTC_FTM_KEY_TYPE_MAX; key_type++)
    {
        hash_key_tbl_id             = sys_alloc_tbl_id_list[key_type].hash_key_tbl_id;
        associate_tbl_id            = sys_alloc_tbl_id_list[key_type].associate_tbl_id;
        associate_tbl_id_rpf        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf;
        hash_key_id_str             = sys_alloc_tbl_id_list[key_type].hash_key_id_str;
        associate_tbl_id_str        = sys_alloc_tbl_id_list[key_type].associate_tbl_id_str;
        associate_tbl_id_rpf_str    = sys_alloc_tbl_id_list[key_type].associate_tbl_id_rpf_str;
        if (NULL == hash_key_id_str)
        {
            /* Hash key only suppor Mac, Ipv4/6 */
            continue;
        }

        table_info_ptr      = &drv_tbls_list[hash_key_tbl_id];
        max_index           = table_info_ptr->max_index_num;
        key_size            = table_info_ptr->key_size;
        key_size            = (key_size / 16) * 80;

        if (0 != associate_tbl_id)
        {
            associate_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id];
            associate_max_index    = associate_tbl_info_ptr->max_index_num;
            table_size            = associate_tbl_info_ptr->entry_size;
            table_size            = (table_size / 16) * 80;
        }

        if (0 != associate_tbl_id_rpf)
        {
            associate_rpf_tbl_info_ptr     = &drv_tbls_list[associate_tbl_id_rpf];
            associate_rpf_max_index    = associate_rpf_tbl_info_ptr->max_index_num;
            rpf_table_size            = associate_rpf_tbl_info_ptr->entry_size;
            rpf_table_size            = (rpf_table_size / 16) * 80;
        }

        /* Hash Key info */
        if (0 == max_index)
        {
            key_entry_rang_from = 0;
            key_entry_rang_to = 0;
        }
        else
        {
            key_entry_rang_from = (table_info_ptr->hw_data_base - SRAM_HASH_KEY_BASE) / 16;
            key_entry_rang_to = key_entry_rang_from + max_index * (table_info_ptr->key_size / 16) - 1;
        }

        /* associated data table info */
        if (0 == associate_max_index || 0 == associate_tbl_id)
        {
            data_entry_rang_from = 0;
            data_entry_rang_to = 0;
        }
        else
        {
            data_entry_rang_from = (associate_tbl_info_ptr->hw_data_base - SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_entry_rang_to = data_entry_rang_from + associate_max_index - 1;
        }

        /* rpf associated data table info */
        if (0 == associate_rpf_max_index || 0 == associate_tbl_id_rpf)
        {
            data_rpf_entry_rang_from = 0;
            data_rpf_entry_rang_to = 0;
        }
        else
        {
            data_rpf_entry_rang_from = (associate_rpf_tbl_info_ptr->hw_data_base - SRAM_ASSOCIATE_DATA_BASE) / 16;
            data_rpf_entry_rang_to = data_rpf_entry_rang_from + associate_rpf_max_index - 1;
        }


        if (0 != max_index)
        {
            SYS_ALLOC_DBG_DUMP("  %-25s%4d%8d%8x~%-8x", hash_key_id_str, key_size, max_index, key_entry_rang_from, key_entry_rang_to);
            if (0 != associate_tbl_id)
            {
                SYS_ALLOC_DBG_DUMP("%-25s%5d%8d%8x~%-8x\n", associate_tbl_id_str, table_size, associate_max_index, data_entry_rang_from, data_entry_rang_to);
            } else {
                SYS_ALLOC_DBG_DUMP("\n");
            }
            if (0 != associate_tbl_id_rpf)
            {
                SYS_ALLOC_DBG_DUMP("  %54s%-25s%5d%8d%8x~%-8x\n"," ",\
                    associate_tbl_id_rpf_str, rpf_table_size, associate_rpf_max_index, data_rpf_entry_rang_from, data_rpf_entry_rang_to);
            }
        }

    }
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");\
    if(alloc_info.is_hash_48ksize)
    {
        total_entry = (96+16)*1024;
    }
    else
    {
        total_entry = (48+16)*1024;
    }

    in_use_entry        = alloc_info.current_sram_offset;
    not_in_use_entry    = total_entry - in_use_entry;
    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Internal SRAM                                       %-d\n", total_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  In Use                                              %-d\n", in_use_entry);
    SYS_ALLOC_DBG_DUMP("  Free                                                %-d\n", not_in_use_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    if(!alloc_info.is_mpls_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","MPLS", alloc_info.mpls_num);
    }

    /* Show Dynamic table alloc info */
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    if(!alloc_info.is_met_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Global Met Table",alloc_info.global_met_entry_num);
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Local Met & DsFwd Table", alloc_info.local_met_dsfwd_entry_num);
    }
    if(!alloc_info.is_nh_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Global Nexthop Table",alloc_info.global_nh_entry_num);
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Local Nexthop Table", alloc_info.local_nh_entry_num);
    }
    if(!alloc_info.is_l2edit_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","L2edit Table", alloc_info.l2edit_entry_num);
    }
    if(!alloc_info.is_l3edit_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","L3edit Table", alloc_info.l3edit_entry_num);
    }

    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    total_entry         = SYS_EXT_SRAM_PHYSICAL_SIZE;
    in_use_entry        = alloc_info.current_ext_sram_offset;
    not_in_use_entry    = total_entry - in_use_entry;
    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  External SRAM                                       %-d\n", total_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  In Use                                              %-d\n", in_use_entry);
    SYS_ALLOC_DBG_DUMP("  Free                                                %-d\n", not_in_use_entry);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");

    if(alloc_info.is_mpls_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","MPLS", alloc_info.mpls_num);
    }

    /* Show Dynamic table alloc info */
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    if(alloc_info.is_met_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Global Met Table", alloc_info.global_met_entry_num);
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Local Met & DsFwd Table", alloc_info.local_met_dsfwd_entry_num);
    }
    if(alloc_info.is_nh_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Global Nexthop Table",alloc_info.global_nh_entry_num);
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n", "Local Nexthop Table", alloc_info.local_nh_entry_num);
    }
    if(alloc_info.is_l2edit_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","L2edit Table", alloc_info.l2edit_entry_num);
    }
    if(alloc_info.is_l3edit_store_ext_sram)
    {
        SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","L3edit Table", alloc_info.l3edit_entry_num);
    }

    SYS_ALLOC_DBG_DUMP("\n--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("  Other Global Configurations Info\n");
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","IS_ACLQOS_DUAL_LOOKUP", alloc_info.is_aclqos_dual_lookup);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","IS_MERGE_ACLQOS_MAC_IP_KEY", alloc_info.is_merge_aclqos_mac_ip_key);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","QOS_GROUP", alloc_info.qos_group);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","IS_HASH_48KSIZE", alloc_info.is_hash_48ksize);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","IS_EXT_QDR_EN", alloc_info.is_ext_qdr_en);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-d\n","IS_IPUC_SA_ENABLE", alloc_info.is_ipuc_sa_enable);
    SYS_ALLOC_DBG_DUMP(" %-28s:%-#x\n","SRAM_FOR_TCAM_AND_HASH_NUM", alloc_info.sram_for_tcam_and_hash_num);
    SYS_ALLOC_DBG_DUMP("--------------------------------------------------------------------------------------------------------------\n");
    SYS_ALLOC_DBG_DUMP("\n");

    return CTC_E_NONE;
}

int32
sys_alloc_get_is_hash_48k(uint8* hash_48k)
{
    CTC_PTR_VALID_CHECK(hash_48k);

    *hash_48k = alloc_info.is_hash_48ksize;

    return CTC_E_NONE;
}

int32
sys_alloc_get_ext_qdr_en(uint8* ext_qdr_en)
{
    CTC_PTR_VALID_CHECK(ext_qdr_en);

    *ext_qdr_en = alloc_info.is_ext_qdr_en;

    return CTC_E_NONE;
}

int32
sys_alloc_get_ext_sram_en(uint8* ext_sram_en)
{
    CTC_PTR_VALID_CHECK(ext_sram_en);

    *ext_sram_en = alloc_info.current_ext_sram_offset ? TRUE : FALSE;

    return CTC_E_NONE;
}


int32
sys_alloc_get_dual_lookup_en(uint8 *dual_lkp_en, uint8 *merge_mackey_en)
{
    CTC_PTR_VALID_CHECK(dual_lkp_en);
    CTC_PTR_VALID_CHECK(merge_mackey_en);

    *dual_lkp_en        = alloc_info.is_aclqos_dual_lookup;
    *merge_mackey_en    = alloc_info.is_merge_aclqos_mac_ip_key;

    return CTC_E_NONE;
}

int32
sys_alloc_get_table_entry_num(uint32 table_id, uint32 *entry_num)
{
    SYS_TBL_ID_VALID_CHECK(table_id);
    CTC_PTR_VALID_CHECK(entry_num);

    *entry_num = drv_tbls_list[table_id].max_index_num;

    return CTC_E_NONE;
}

int32
sys_alloc_get_ipucsa_enable(uint8 *enable)
{
     *enable = alloc_info.is_ipuc_sa_enable;
      return CTC_E_NONE;
}
int32
sys_alloc_get_table_address(uint32 table_id, uint32 *table_address)
{
    SYS_TBL_ID_VALID_CHECK(table_id);
    CTC_PTR_VALID_CHECK(table_address);

    *table_address = drv_tbls_list[table_id].hw_data_base;

    return CTC_E_NONE;
}

int32
sys_alloc_get_ip_tunnel_size(uint32 *ip_tunnel_size)
{
    CTC_PTR_VALID_CHECK(ip_tunnel_size);

    *ip_tunnel_size = alloc_info.ip_tunnel_table_size;

    return CTC_E_NONE;
}

int32
sys_alloc_get_met_dsfwd_table_info(uint32 *global_met_entry_num, uint32 *local_met_dsfwd_entry_num)
{
    CTC_PTR_VALID_CHECK(global_met_entry_num);
    CTC_PTR_VALID_CHECK(local_met_dsfwd_entry_num);

    *global_met_entry_num   = alloc_info.global_met_entry_num;
    *local_met_dsfwd_entry_num    = alloc_info.local_met_dsfwd_entry_num;

    return CTC_E_NONE;
}

sys_alloc_info_t *
sys_alloc_get_alloc_info_ptr(void)
{
    return (&alloc_info);
}

int32
sys_alloc_check_key_space(ctc_ftm_profile_info_t *profile_info)
{

    uint32  ext_tcam_physical_size = profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_TCAM_SIZE]  ;
    uint32  hash_physical_size     = profile_info->table_size[CTC_FTM_TBL_TYPE_HASH];
    uint32  value;
    uint32  key_type;
    uint32  int_tcam_used_entry = 0;
    uint32  ext_tcam_used_entry = 0;
    uint32  hash_used_entry     = 0;

    ctc_ftm_key_info_t *key_info;

    for(key_type = 0; key_type < profile_info->key_info_size; key_type++)
    {
        key_info = &profile_info->key_info[key_type];

        value = key_info->key_size * key_info->max_key_index;

        if(value && (CTC_FTM_KEY_SIZE_80_BIT!=key_info->key_size)
            && (CTC_FTM_KEY_SIZE_160_BIT!=key_info->key_size)
            && (CTC_FTM_KEY_SIZE_320_BIT!=key_info->key_size)
            && (CTC_FTM_KEY_SIZE_640_BIT!=key_info->key_size))
        {
            return CTC_E_ALLOCATION_INVALID_KEY_SIZE;
        }

        if(CTC_FTM_INT_TCAM == key_info->key_media)
        {
            if(CTC_FTM_KEY_SIZE_640_BIT == key_info->key_size)
            {
                return CTC_E_ALLOCATION_INVALID_KEY_SIZE;
            }
        }

        if ((value % 256) != 0)
        {
            /* The minimal increase of entry offset is 256 */
            SYS_ALLOC_DBG_INFO("The minimal increase of entry offset is 256\n");
            SYS_ALLOC_DBG_INFO("value = %d, key_index = %d\n", value,
                                key_info->max_key_index);

            return CTC_E_ALLOCATION_INVALID_ENTRY_SIZE;
        }

        switch(key_info->key_media)
        {
            case CTC_FTM_INT_TCAM:
                int_tcam_used_entry += value;
                break;
            case CTC_FTM_EXT_TCAM:
                ext_tcam_used_entry += value;
                break;
            case CTC_FTM_HASH:
                hash_used_entry += value;
                break;
            default :
                return CTC_E_INVALID_PARAM;
                break;
        }
    }

    if (int_tcam_used_entry > SYS_INT_TCAM_PHYSICAL_SIZE)
    {
        SYS_ALLOC_DBG_INFO("Internal tcam key exceeds "
                           "Internal TCAM physical size!\n");
        return CTC_E_ALLOCATION_EXCEED_INT_TCAM_PHYSIZE;
    }
    if (ext_tcam_used_entry > ext_tcam_physical_size)
    {
        SYS_ALLOC_DBG_INFO("External tcam key exceeds "
                           "External TCAM physical size!\n");
        return CTC_E_ALLOCATION_EXCEED_EXT_TCAM_PHYSIZE;
    }

    if(hash_physical_size != 48*1024 && hash_physical_size != 96*1024 )
    {
        SYS_ALLOC_DBG_INFO("Invalid HASH physical size!\n");
        return CTC_E_ALLOCATION_INVALID_HASH_PHYSIZE;
    }

    if (hash_used_entry > hash_physical_size)
    {
        SYS_ALLOC_DBG_INFO("HASH key exceeds HASH physical size!\n");
        return CTC_E_ALLOCATION_EXCEED_HASH_PHYSIZE;
    }

    /* Pass key space checking! */
    /* record total entry in alloc_info */
    alloc_info.int_tcam_used_entry = int_tcam_used_entry;
    alloc_info.ext_tcam_used_entry = ext_tcam_used_entry;
    alloc_info.hash_used_entry     = hash_used_entry;

    alloc_info.is_aclqos_dual_lookup      = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_ACL_QOS_DUAL_LKUP);
    /*always to be 1,merge mac&ip key*/
    alloc_info.is_merge_aclqos_mac_ip_key = 1;

    alloc_info.is_hash_48ksize =(profile_info->table_size[CTC_FTM_TBL_TYPE_HASH] == 48*1024);
    alloc_info.is_ipuc_sa_enable = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_IPUC_SA_EN);
    alloc_info.is_mpls_store_ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_MPLS_IN_EXT_SRAM);
    alloc_info.is_met_store_ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_MET_IN_EXT_SRAM);
    alloc_info.is_nh_store_ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_NH_IN_EXT_SRAM);
    alloc_info.is_l2edit_store_ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_L2EDIT_IN_EXT_SRAM);
    alloc_info.is_l3edit_store_ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_L3EDIT_IN_EXT_SRAM);
    alloc_info.disable_merge_mac_ip_key_physical = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_DISABLE_MERGE_MAC_IP_KEY_PHYSICAL);

    return CTC_E_NONE;
}

int32
sys_alloc_ibm_tcam_key(ctc_ftm_profile_info_t *profile_info)
{
    uint8 local_chip_num = 0;
    uint8 qos_group = 0;
    uint8 index = 0;
    uint32 tcam_key_tbl_id = 0;
    uint32 key_type = 0;
    uint16 key_counter = 0;
    uint32 key_size = 0;
    uint32 max_key_index = 0;
    uint32 allocated_tcam_entry = 0;
    tables_t *table_info_ptr = NULL;
    ctc_ftm_key_info_t *key_info  = NULL;

    local_chip_num = sys_humber_get_local_chip_num();

    /* init order must follow sys_alloc_tbl_id_list */
    for (key_counter = 0; key_counter < CTC_FTM_KEY_TYPE_MAX; key_counter ++)
    {
        for(key_type = 0; key_type < profile_info->key_info_size; key_type++)
        {
            key_info = &profile_info->key_info[key_type];
            /* we only care about internal tcam here */
            if (key_info->key_media != CTC_FTM_INT_TCAM)
            {
                continue;
            }

            /* scan the sys_alloc_tbl_id_list for a correct associated table */
            if (sys_alloc_tbl_id_list[key_counter].key_type == key_info->key_id)
            {
                break;
            }
        }

        if (key_type == profile_info->key_info_size)
        {
            continue;
        }

        if(sys_alloc_tbl_id_list[key_counter].tcam_key_tbl_id == 0)
        {
            continue;
        }

        key_size       = key_info->key_size;
        max_key_index  = key_info->max_key_index;

        if (0 == max_key_index)
        {
            /* entry num is 0, means the key needn't allocation.  */
            continue;
        }


        /* an error key type */
        if (key_counter == CTC_FTM_KEY_TYPE_MAX)
        {
            continue;
        }
        alloc_info.tcam_info[key_info->key_id].is_ext_tcam = 0;
        alloc_info.tcam_info[key_info->key_id].is_ext_tcam_valid = 1;
        tcam_key_tbl_id = sys_alloc_tbl_id_list[key_counter].tcam_key_tbl_id;
        table_info_ptr  = &drv_tbls_list[tcam_key_tbl_id];
        /* The absolute address of a given table = Memory Base + (table offset * 16) */
        table_info_ptr->hw_data_base    = (allocated_tcam_entry << 4) + INT_TCAM_DATA_BASE;
        table_info_ptr->hw_mask_base    = (allocated_tcam_entry << 4) + INT_TCAM_MASK_BASE;
        table_info_ptr->key_size        = key_size * BYTES_PER_TCAM_ENTRY;
        table_info_ptr->max_index_num   = max_key_index;

        if (DS_QOS_MAC_KEY == tcam_key_tbl_id
            || DS_QOS_IPV4_KEY == tcam_key_tbl_id
            || DS_QOS_IPV6_KEY == tcam_key_tbl_id)
        {
            /*acl qos with same key type should location at same type tcam*/
            if((DS_QOS_MAC_KEY == tcam_key_tbl_id)
                && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam_valid == 1)
                && alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam))
            {
                continue;
            }
            if((DS_QOS_IPV4_KEY == tcam_key_tbl_id)
                && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam_valid == 1)
                && alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam))
            {
                continue;
            }
            if((DS_QOS_IPV6_KEY == tcam_key_tbl_id)
                && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam_valid == 1)
                && alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_IPV6].is_ext_tcam))
            {
                continue;
            }

            /* if dual lookup, acl qos can't be stored in the same super block. */
            for(index=(allocated_tcam_entry/IBM_TCAM_SUPER_BLOCK_SIZE);
                index<((allocated_tcam_entry+max_key_index*key_size)/IBM_TCAM_SUPER_BLOCK_SIZE+1); index++)
            {
                SET_BIT(qos_group, index);
            }
        }

        /* update tcam total entry that consumed by the given key */
        allocated_tcam_entry += max_key_index * key_size;

        if (alloc_info.disable_merge_mac_ip_key_physical)
        {
            if ((DS_QOS_IPV4_KEY == tcam_key_tbl_id) && (key_size > 1))
            {
                for(index=(allocated_tcam_entry/IBM_TCAM_SUPER_BLOCK_SIZE);
                    index<((allocated_tcam_entry+(max_key_index+64)*key_size/2)/IBM_TCAM_SUPER_BLOCK_SIZE+1); index++)
                {
                    SET_BIT(qos_group, index);
                }
                allocated_tcam_entry += (max_key_index+64) * key_size / 2;
            }

            if ((DS_ACL_IPV4_KEY == tcam_key_tbl_id) && (max_key_index > 0))
            {
                return CTC_E_NOT_SUPPORT;
            }
        }

        if (allocated_tcam_entry > SYS_INT_TCAM_PHYSICAL_SIZE)
        {
            SYS_ALLOC_DBG_INFO("Internal tcam key exceeds Internal TCAM physical size!\n");
            return CTC_E_NO_MEMORY;
        }

    }

    /* record qos group for IBM Tcam Initiation */
    alloc_info.qos_group = qos_group;

    return CTC_E_NONE;
}

int32
sys_alloc_nl_tcam_key(ctc_ftm_profile_info_t *profile_info)
{
    uint8 local_chip_num = 0;
    uint32 key_type = 0;
    uint32 key_size = 0;
    uint32 max_key_index = 0;
    uint32 tcam_key_tbl_id = 0;
    uint32 allocated_tcam_entry = 0;
    uint32 current_block_size = 0;  /* 1 80b, 2 160b, 3 320b, 4 640b. */
    uint32 current_block_id = 0;    /* max block id is 15. */
    uint32 updated_block_id = 0;
    uint32 current_ltr_id = 2;  /* ltr_id 0, 1,is reserved for MAC, IPv4, IPv6 AclQos Dual Lookup respectively.  */
    uint16 key_counter = 0;
    tables_t *table_info_ptr = NULL;
    ctc_ftm_key_info_t *key_info = NULL;

    local_chip_num = sys_humber_get_local_chip_num();

    for (key_counter = 0; key_counter < CTC_FTM_KEY_TYPE_MAX; key_counter ++)
    {
        for(key_type = 0; key_type < profile_info->key_info_size; key_type++)
        {
            key_info = &profile_info->key_info[key_type];

            if (key_info->key_media != CTC_FTM_EXT_TCAM)
            {
                continue;
            }

            /* scan the sys_alloc_tbl_id_list for a correct associated table */
            if (sys_alloc_tbl_id_list[key_counter].key_type == key_info->key_id)
            {
                break;
            }
        }

        if (key_type == profile_info->key_info_size)
        {
            continue;
        }

        if(sys_alloc_tbl_id_list[key_counter].tcam_key_tbl_id == 0)
        {
            continue;
        }

        key_size            = key_info->key_size;
        if(CTC_FTM_KEY_SIZE_80_BIT == key_size)
        {
            alloc_info.tcam_info[key_info->key_id].blk_width = 0;
        }
        else if(CTC_FTM_KEY_SIZE_160_BIT == key_size)
        {
            alloc_info.tcam_info[key_info->key_id].blk_width = 1;
        }
        else if(CTC_FTM_KEY_SIZE_320_BIT == key_size)
        {
            alloc_info.tcam_info[key_info->key_id].blk_width = 2;
        }
        else if(CTC_FTM_KEY_SIZE_640_BIT == key_size)
        {
            alloc_info.tcam_info[key_info->key_id].blk_width = 3;
        }
        else
        {
            continue;
        }
        max_key_index       = key_info->max_key_index;

        if (0 == max_key_index)
        {
            /* entry num is 0, means the key needn't allocation.  */
            continue;
        }

        /* scan the sys_alloc_tbl_id_list for a correct associated table */
        for (key_counter = 0; key_counter < CTC_FTM_KEY_TYPE_MAX; key_counter ++)
        {
            if (sys_alloc_tbl_id_list[key_counter].key_type == key_info->key_id)
            {
                break;
            }

        }
        /* an error key type */
        if (key_counter == CTC_FTM_KEY_TYPE_MAX)
        {
            continue;
        }
        alloc_info.tcam_info[key_info->key_id].is_ext_tcam = 1;
        alloc_info.tcam_info[key_info->key_id].is_ext_tcam_valid = 1;

        tcam_key_tbl_id = sys_alloc_tbl_id_list[key_counter].tcam_key_tbl_id;

        /*acl qos with same key type should location at same type tcam*/
        if((DS_QOS_MAC_KEY == tcam_key_tbl_id)
            && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam_valid == 1)
            && (!(alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam))))
        {
            continue;
        }

        if((DS_QOS_IPV4_KEY == tcam_key_tbl_id)
            && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam_valid == 1)
            && (!(alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_MAC_IPV4].is_ext_tcam))))
        {
            continue;
        }

        if((DS_QOS_IPV6_KEY == tcam_key_tbl_id)
            && ((alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_IPV6].is_ext_tcam_valid == 1)
            && (!(alloc_info.tcam_info[CTC_FTM_KEY_TYPE_ACL_IPV6].is_ext_tcam))))
        {
            continue;
        }

        /* current size is different with pervious size  */
        if (key_size != current_block_size)
        {
            /* the given key of different size should be stored in different block. */
            /* in this case, don't care about AclQoS Dual Lookup. */
            current_block_size = key_size;
            current_block_id = allocated_tcam_entry / NL9K_TCAM_BLOCK_SIZE;
            if(0 != (allocated_tcam_entry % NL9K_TCAM_BLOCK_SIZE))
            {
                /* if current entry offset is not the beginning of a block */
                /* move entry offset to the next block to store the current key. */
                allocated_tcam_entry = (current_block_id + 1) * NL9K_TCAM_BLOCK_SIZE;
                current_block_id ++;
            }
            else
            {
                /* if current entry offset is happen to be the start of a block */
                /* no need to adjust entry offset. */
            }

            table_info_ptr  = &drv_tbls_list[tcam_key_tbl_id];

            /* The absolute address of a given table = Memory Base + (table offset * 16) */
            table_info_ptr->hw_data_base    = (allocated_tcam_entry << 4) + EXT_TCAM_DATA_BASE;
            table_info_ptr->hw_mask_base    = (allocated_tcam_entry << 4)+ EXT_TCAM_MASK_BASE;
            table_info_ptr->max_index_num   = max_key_index;
            table_info_ptr->key_size        = key_size * BYTES_PER_TCAM_ENTRY;

            /* update tcam total entry that consumed by the given key */
            allocated_tcam_entry += max_key_index * key_size;

            updated_block_id = (allocated_tcam_entry - 1) / NL9K_TCAM_BLOCK_SIZE;

            /* record Allocation Info for NL9K TCAM Initiation. */
            alloc_info.tcam_info[key_info->key_id].start_block_id   = current_block_id;
            alloc_info.tcam_info[key_info->key_id].end_block_id     = updated_block_id;

            /* AclMac and QosMac share the same TCAM.LtrId */
            if (CTC_FTM_KEY_TYPE_ACL_MAC_IPV4== key_info->key_id || CTC_FTM_KEY_TYPE_QOS_MAC_IPV4== key_info->key_id)
            {
                alloc_info.tcam_info[key_info->key_id].ltr_id = ACLQOS_MAC_IPV4_DUAL_LOOKUP_LTR_ID;
            }
            else if (CTC_FTM_KEY_TYPE_ACL_IPV6 == key_info->key_id || CTC_FTM_KEY_TYPE_QOS_IPV6 == key_info->key_id)
            {
                alloc_info.tcam_info[key_info->key_id].ltr_id = ACLQOS_IPV6_DUAL_LOOKUP_LTR_ID;
            }
            else
            {
                /* current key is not Acl or Qos key. */
                /* record current ltrId for NL9K TCAM Initiation. */
                alloc_info.tcam_info[key_info->key_id].ltr_id = current_ltr_id;
                current_ltr_id ++;
            }
        }
        else
        {
            /* the given key of the same size can be stored in the same block, excepting Acl Qos Key. */
            current_block_id = allocated_tcam_entry / NL9K_TCAM_BLOCK_SIZE;

            if ((CTC_FTM_KEY_TYPE_ACL_IPV6 == key_type || CTC_FTM_KEY_TYPE_ACL_MAC_IPV4 == key_type
                || CTC_FTM_KEY_TYPE_QOS_IPV6 == key_type || CTC_FTM_KEY_TYPE_QOS_MAC_IPV4 == key_type))
            {
                /* considering Dual Lookup, Acl and Qos key can't be stored in the same block. */
                if(0 != (allocated_tcam_entry % NL9K_TCAM_BLOCK_SIZE))
                {
                    /* and if updated entry offset is not the start of a block,  */
                    /* move entry offset to the next block to store the next key. */
                    allocated_tcam_entry = (current_block_id + 1) * NL9K_TCAM_BLOCK_SIZE;
                    current_block_id ++;
                }
            }

            table_info_ptr  = &drv_tbls_list[tcam_key_tbl_id];

            /* The absolute address of a given table = Memory address + (table offset * 16) */
            table_info_ptr->hw_data_base    = (allocated_tcam_entry << 4) + EXT_TCAM_DATA_BASE;
            table_info_ptr->hw_mask_base    = (allocated_tcam_entry << 4) + EXT_TCAM_MASK_BASE;
            table_info_ptr->max_index_num   = max_key_index;
            table_info_ptr->key_size        = key_size * BYTES_PER_TCAM_ENTRY;

            /* update tcam total entry that consumed by the given key */
            allocated_tcam_entry += max_key_index * key_size;
            updated_block_id = (allocated_tcam_entry - 1) / NL9K_TCAM_BLOCK_SIZE;
            /* record Allocation Info for NL9K TCAM Initiation. */
            alloc_info.tcam_info[key_info->key_id].start_block_id   = current_block_id;
            alloc_info.tcam_info[key_info->key_id].end_block_id     = updated_block_id;
            if ((CTC_FTM_KEY_TYPE_ACL_IPV6 == key_info->key_id || CTC_FTM_KEY_TYPE_ACL_MAC_IPV4 == key_info->key_id
                || CTC_FTM_KEY_TYPE_QOS_IPV6 == key_info->key_id || CTC_FTM_KEY_TYPE_QOS_MAC_IPV4 == key_info->key_id))
            {
                /* AclMac and QosMac share the same TCAM.LtrId */
                if (CTC_FTM_KEY_TYPE_ACL_MAC_IPV4== key_info->key_id || CTC_FTM_KEY_TYPE_QOS_MAC_IPV4== key_info->key_id)
                {
                    alloc_info.tcam_info[key_info->key_id].ltr_id = ACLQOS_MAC_IPV4_DUAL_LOOKUP_LTR_ID;
                }
                else if (CTC_FTM_KEY_TYPE_ACL_IPV6 == key_info->key_id || CTC_FTM_KEY_TYPE_QOS_IPV6 == key_info->key_id)
                {
                    alloc_info.tcam_info[key_info->key_id].ltr_id = ACLQOS_IPV6_DUAL_LOOKUP_LTR_ID;
                }
                else
                {
                    /* don't care if not AclQosKey type. */
                }
            }
            else
            {
                /* current key is not Acl or Qos key. */
                /* record current ltrId for NL9K TCAM Initiation. */
                alloc_info.tcam_info[key_info->key_id].ltr_id = current_ltr_id;
                current_ltr_id ++;
            }
        }
    }

    /* check if ltr_id exceeds TCAM limitation */
    if (current_ltr_id >= MAX_NL9K_TCAM_LTR_ID)
    {
        /* NL9K Tcam only supports 32 ltr. */
        SYS_ALLOC_DBG_INFO("Error: NL9K Tcam only supports 32 ltr!");
        return CTC_E_NO_MEMORY;
    }

    return CTC_E_NONE;

}

static int32
_sys_alloc_init_acl_qos_key(void)
{
    tables_t *table_info_ptr = NULL;
    tables_t *aclqos_ip_table_ptr = NULL;
    tables_t *aclqos_mpls_table_ptr = NULL;

    /*ACL and QOS share the same keysize, if ACL entry number = = 0, use QOS keysize*/
    if (!drv_tbls_list[DS_ACL_IPV4_KEY].max_index_num)
    {
        drv_tbls_list[DS_ACL_IPV4_KEY].key_size = drv_tbls_list[DS_QOS_IPV4_KEY].key_size;
    }
    if (!drv_tbls_list[DS_ACL_IPV6_KEY].max_index_num)
    {
        drv_tbls_list[DS_ACL_IPV6_KEY].key_size = drv_tbls_list[DS_QOS_IPV6_KEY].key_size;
    }

    /* DsAclMacKey and DsAclIpv4Key share the same TCAM space. */
    table_info_ptr = &drv_tbls_list[DS_ACL_MAC_KEY];
    aclqos_ip_table_ptr = &drv_tbls_list[DS_ACL_IPV4_KEY];
    aclqos_mpls_table_ptr = &drv_tbls_list[DS_ACL_MPLS_KEY];

    table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    table_info_ptr->key_size      = aclqos_ip_table_ptr->key_size;
    aclqos_mpls_table_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    aclqos_mpls_table_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    aclqos_mpls_table_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    aclqos_mpls_table_ptr->key_size      = aclqos_ip_table_ptr->key_size;

    table_info_ptr = &drv_tbls_list[DS_QOS_MAC_KEY];
    aclqos_ip_table_ptr = &drv_tbls_list[DS_QOS_IPV4_KEY];
    aclqos_mpls_table_ptr = &drv_tbls_list[DS_QOS_MPLS_KEY];

    table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    table_info_ptr->key_size        = aclqos_ip_table_ptr->key_size;
    aclqos_mpls_table_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    aclqos_mpls_table_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    aclqos_mpls_table_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    aclqos_mpls_table_ptr->key_size      = aclqos_ip_table_ptr->key_size;

    /* special process for DS_QOS_MAC_KEY */
    if ((alloc_info.disable_merge_mac_ip_key_physical)
        && (aclqos_ip_table_ptr->key_size > 1))
    {
        table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base
            + aclqos_ip_table_ptr->max_index_num * aclqos_ip_table_ptr->key_size;
        table_info_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base
            + aclqos_ip_table_ptr->max_index_num * aclqos_ip_table_ptr->key_size;

        /* For simple processing, we just let mac entry number equal to ipv4 entry number plus 64 temporarily */
        table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num + 64;
        table_info_ptr->key_size        = aclqos_ip_table_ptr->key_size / 2;
    }

    return CTC_E_NONE;
}

static int32
_sys_alloc_init_oam_key(void)
{
    tables_t *table_info_ptr = NULL;
    tables_t *eth_oam_key_ptr = NULL;

    eth_oam_key_ptr = &drv_tbls_list[DS_ETH_OAM_KEY];

    table_info_ptr = &drv_tbls_list[DS_PBT_OAM_KEY];
    table_info_ptr->hw_data_base    = eth_oam_key_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = eth_oam_key_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = eth_oam_key_ptr->max_index_num;
    table_info_ptr->key_size        = eth_oam_key_ptr->key_size;

    table_info_ptr = &drv_tbls_list[DS_MPLS_OAM_LABEL_KEY];
    table_info_ptr->hw_data_base    = eth_oam_key_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = eth_oam_key_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = eth_oam_key_ptr->max_index_num;
    table_info_ptr->key_size        = eth_oam_key_ptr->key_size;

    table_info_ptr = &drv_tbls_list[DS_MPLS_OAM_IPV4_TTSI_KEY];
    table_info_ptr->hw_data_base    = eth_oam_key_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = eth_oam_key_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = eth_oam_key_ptr->max_index_num;
    table_info_ptr->key_size        = eth_oam_key_ptr->key_size;

    table_info_ptr = &drv_tbls_list[DS_ETH_OAM_RMEP_KEY];
    table_info_ptr->hw_data_base    = eth_oam_key_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = eth_oam_key_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = eth_oam_key_ptr->max_index_num;
    table_info_ptr->key_size        = eth_oam_key_ptr->key_size;

    return CTC_E_NONE;
}



int32
sys_alloc_tcam_key(ctc_ftm_profile_info_t *profile_info)
{
    if (0 != alloc_info.int_tcam_used_entry)
    {
        /* Internal Tcam Enabled */
        CTC_ERROR_RETURN(sys_alloc_ibm_tcam_key(profile_info));
    }
    if (0 != alloc_info.ext_tcam_used_entry)
    {
        /* External Tcam Enabled. */
        if (CTC_FTM_TCAM_TYPE_NL9K == profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_TCAM_TYPE])
        {
            CTC_ERROR_RETURN(sys_alloc_nl_tcam_key(profile_info));
        }
        else
        {
            /* compatible with other TCAM allcotion */
            SYS_ALLOC_DBG_INFO("Tcam: %d is a Unrecognized Tcam Teyp!\n", profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_TCAM_TYPE]);
            return CTC_E_NO_MEMORY;
        }
    }

    /* adjust mac/ip aclQos table, considering mac/ip key merging. */
    _sys_alloc_init_acl_qos_key();

    /* OAM related 5 key share the same 256 tcam */
    _sys_alloc_init_oam_key();

    return CTC_E_NONE;
}

int32
sys_alloc_hash_key(ctc_ftm_profile_info_t *profile_info)
{
    uint8 local_chip_num = 0;
    uint32 hash_key_tbl_id = 0;
    uint32 key_type = 0;
    uint32 key_size = 0;
    uint32 max_key_index = 0;
    uint32 allocated_hash_entry = 0;
    uint16 key_counter = 0;
    tables_t *table_info_ptr = NULL;
    ctc_ftm_key_info_t *key_info;

    local_chip_num = sys_humber_get_local_chip_num();

    for(key_type = 0; key_type < profile_info->key_info_size; key_type++)
    {
        key_info = &profile_info->key_info[key_type];

        if (key_info->key_media != CTC_FTM_HASH)
        {
            continue;
        }
        key_size            = key_info->key_size;
        max_key_index       = key_info->max_key_index;

        if (0 == max_key_index)
        {
            /* entry num is 0, means the key needn't allocation.  */
            continue;
        }
        /* scan the sys_alloc_tbl_id_list for a correct associated table */
        for (key_counter = 0; key_counter < CTC_FTM_KEY_TYPE_MAX; key_counter ++)
        {
            if (sys_alloc_tbl_id_list[key_counter].key_type == key_info->key_id)
            {
                break;
            }
        }
        /* an error key type */
        if (key_counter == CTC_FTM_KEY_TYPE_MAX)
        {
            continue;
        }

        hash_key_tbl_id = sys_alloc_tbl_id_list[key_counter].hash_key_tbl_id;

        if (0 == hash_key_tbl_id)
        {
            /* Hash key only suppor Mac, Ipv4, Ipv6 */
            SYS_ALLOC_DBG_INFO("The key type of %d don't support "
                               "Hash lookup!\n", key_type);
            return CTC_E_NO_MEMORY;
        }

        if((max_key_index&(max_key_index-1)) != 0)
        {
            /* HASH key should be a power of 2 */
            SYS_ALLOC_DBG_INFO("The key size of %d don't support "
                               "Hash lookup, Hash key size"
                               " should be a power of 2!\n", max_key_index);
            return CTC_E_ALLOCATION_INVALID_HASH_PHYSIZE;

        }

        table_info_ptr = &drv_tbls_list[hash_key_tbl_id];

        /* The absolute address of a given table = Memory Base + (table offset * 16) */
        table_info_ptr->hw_data_base   = (allocated_hash_entry << 4) + SRAM_HASH_KEY_BASE;
        table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
        table_info_ptr->max_index_num  = max_key_index;
        table_info_ptr->key_size       = key_size * BYTES_PER_HASH_ENTRY;

        /* update tcam total entry that consumed by the given key */
        allocated_hash_entry += max_key_index * key_size;
    }

    return CTC_E_NONE;
}


static int32
_sys_alloc_init_acl_qos_associate(void)
{
    tables_t *table_info_ptr = NULL;
    tables_t *aclqos_ip_table_ptr = NULL;
    tables_t *aclqos_mpls_table_ptr = NULL;

    /* DsAclMacKey and DsAclIpv4Key share the same TCAM space. */
    table_info_ptr = &drv_tbls_list[DS_MAC_ACL];
    aclqos_ip_table_ptr = &drv_tbls_list[DS_IPV4_ACL];
    aclqos_mpls_table_ptr = &drv_tbls_list[DS_MPLS_ACL];

    table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    aclqos_mpls_table_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    aclqos_mpls_table_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    aclqos_mpls_table_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;

    table_info_ptr = &drv_tbls_list[DS_MAC_QOS];
    aclqos_ip_table_ptr = &drv_tbls_list[DS_IPV4_QOS];
    aclqos_mpls_table_ptr = &drv_tbls_list[DS_MPLS_QOS];

    table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    table_info_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;
    aclqos_mpls_table_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base;
    aclqos_mpls_table_ptr->hw_mask_base    = aclqos_ip_table_ptr->hw_mask_base;
    aclqos_mpls_table_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num;

    if (alloc_info.disable_merge_mac_ip_key_physical)
    {
        table_info_ptr->hw_data_base    = aclqos_ip_table_ptr->hw_data_base + (aclqos_ip_table_ptr->max_index_num << 4);
        table_info_ptr->hw_mask_base    = INVALID_MASK_OFFSET;
        /* For simple processing, we just let mac entry number equal to ipv4 entry number plus 64 temporarily */
        table_info_ptr->max_index_num   = aclqos_ip_table_ptr->max_index_num + 64;
    }
    return CTC_E_NONE;
}

sys_tcam_key_info_t sys_tcam_key_info_list[] =
{
{DS_MAC_KEY,                      CTC_FTM_KEY_TYPE_FIB,              MACDA_TABLEID0_C   },
{DS_MAC_HASH_KEY0,                CTC_FTM_KEY_TYPE_FIB,              0  },
{DS_MAC_HASH_KEY1,                CTC_FTM_KEY_TYPE_FIB,              0   },
{DS_ACL_MAC_KEY,                  CTC_FTM_KEY_TYPE_ACL_MAC_IPV4,     ACL_MAC_TABLEID0_C   },
{DS_QOS_MAC_KEY,                  CTC_FTM_KEY_TYPE_QOS_MAC_IPV4,     QOS_MAC_TABLEID0_C   },
{DS_ACL_IPV4_KEY,                 CTC_FTM_KEY_TYPE_ACL_MAC_IPV4,     ACL_IPV4_TABLEID0_C  },
{DS_QOS_IPV4_KEY,                 CTC_FTM_KEY_TYPE_QOS_MAC_IPV4,     QOS_IPV4_TABLEID0_C   },
{DS_ACL_MPLS_KEY,                 CTC_FTM_KEY_TYPE_ACL_MAC_IPV4,     ACL_IPV4_TABLEID0_C   },
{DS_QOS_MPLS_KEY,                 CTC_FTM_KEY_TYPE_QOS_MAC_IPV4,     QOS_IPV4_TABLEID0_C   },
{DS_ACL_IPV6_KEY,                 CTC_FTM_KEY_TYPE_ACL_IPV6,         ACL_IPV6_TABLEID0_C   },
{DS_QOS_IPV6_KEY,                 CTC_FTM_KEY_TYPE_QOS_IPV6,         QOS_IPV6_TABLEID0_C   },
{DS_IPV4_UCAST_ROUTE_KEY,         CTC_FTM_KEY_TYPE_IPV4_UCAST, UCAST_IPV4_TABLEID0_C   },
{DS_IPV4_MCAST_ROUTE_KEY,         CTC_FTM_KEY_TYPE_IPV4_MCAST,MCAST_IPV4_TABLEID0_C },
{DS_IPV4_NAT_KEY,                 CTC_FTM_KEY_TYPE_IPV4_NAT,NAT_IPV4_TABLEID0_C },
{DS_IPV4_PBR_DUALDA_KEY,          CTC_FTM_KEY_TYPE_IPV4_PBR,PBR_IPV4_TABLEID0_C },
{DS_IPV4_UCAST_HASH_KEY0,         CTC_FTM_KEY_TYPE_IPV4_UCAST,0 },
{DS_IPV4_UCAST_HASH_KEY1,         CTC_FTM_KEY_TYPE_IPV4_UCAST,0 },
{DS_IPV4_MCAST_HASH_KEY0,         CTC_FTM_KEY_TYPE_IPV4_MCAST,0 },
{DS_IPV4_MCAST_HASH_KEY1,         CTC_FTM_KEY_TYPE_IPV4_MCAST,0 },
{DS_IPV6_UCAST_ROUTE_KEY,         CTC_FTM_KEY_TYPE_IPV6_UCAST,UCAST_IPV6_TABLEID0_C },
{DS_IPV6_NAT_KEY,                 CTC_FTM_KEY_TYPE_IPV6_NAT,NAT_IPV6_TABLEID0_C },
{DS_IPV6_PBR_DUALDA_KEY,          CTC_FTM_KEY_TYPE_IPV6_PBR,PBR_IPV6_TABLEID0_C },
{DS_IPV6_MCAST_ROUTE_KEY,         CTC_FTM_KEY_TYPE_IPV6_MCAST,MCAST_IPV6_TABLEID0_C },
{DS_IPV6_UCAST_HASH_KEY0,         CTC_FTM_KEY_TYPE_IPV6_UCAST,0 },
{DS_IPV6_MCAST_HASH_KEY0,         CTC_FTM_KEY_TYPE_IPV6_UCAST,0 },
{DS_IPV6_UCAST_HASH_KEY1,         CTC_FTM_KEY_TYPE_IPV6_UCAST,0 },
{DS_IPV6_MCAST_HASH_KEY1,         CTC_FTM_KEY_TYPE_IPV6_UCAST,0 },
{DS_USER_ID_VLAN_KEY,             CTC_FTM_KEY_TYPE_USERID_MAC,USERID_VLAN_TABLEID_C },
{DS_USER_ID_MAC_KEY,              CTC_FTM_KEY_TYPE_USERID_VLAN,USERID_MAC_TABLEID0_C },
{DS_USER_ID_IPV4_KEY,             CTC_FTM_KEY_TYPE_USERID_IPV4,USERID_IPV4_TABLEID0_C },
{DS_USER_ID_IPV6_KEY,             CTC_FTM_KEY_TYPE_USERID_IPV6,USERID_IPV6_TABLEID0_C },
{DS_ETH_OAM_KEY,                  CTC_FTM_KEY_TYPE_OAM,OAM_TABLEID},
{DS_ETH_OAM_HASH_KEY0,            CTC_FTM_KEY_TYPE_OAM,0 },
{DS_ETH_OAM_HASH_KEY1,            CTC_FTM_KEY_TYPE_OAM,0 },
{DS_PBT_OAM_KEY,                  CTC_FTM_KEY_TYPE_OAM,0 },
{DS_PBT_OAM_HASH_KEY0,            CTC_FTM_KEY_TYPE_OAM,0 },
{DS_PBT_OAM_HASH_KEY1,            CTC_FTM_KEY_TYPE_OAM,0 },
{DS_MPLS_OAM_LABEL_KEY,           CTC_FTM_KEY_TYPE_OAM,OAM_TABLEID },
{DS_MPLS_OAM_LABEL_HASH_KEY0,     CTC_FTM_KEY_TYPE_OAM,0 },
{DS_MPLS_OAM_LABEL_HASH_KEY1,     CTC_FTM_KEY_TYPE_OAM,0 },
{DS_MPLS_OAM_IPV4_TTSI_KEY,       CTC_FTM_KEY_TYPE_OAM,OAM_TABLEID },
{DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0, CTC_FTM_KEY_TYPE_OAM,0 },
{DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1, CTC_FTM_KEY_TYPE_OAM,0 },
{DS_ETH_OAM_RMEP_KEY,            CTC_FTM_KEY_TYPE_OAM,OAM_TABLEID },
{DS_ETH_OAM_RMEP_HASH_KEY0,       CTC_FTM_KEY_TYPE_OAM,0 },
{DS_ETH_OAM_RMEP_HASH_KEY1,      CTC_FTM_KEY_TYPE_OAM,0 },

};

uint32
sys_alloc_get_sram_database(uint32  tbl_id)
{
    uint8 internal_key_id;

    CTC_VALUE_RANGE_CHECK(tbl_id, DS_MAC_ACL, DS_ETH_OAM_RMEP_HASH_KEY1);

    switch(tbl_id)
    {
        case DS_MAC_ACL: /* 204 */
        case DS_IPV4_ACL: /* 205 */
        case DS_MPLS_ACL: /* 206 */
        case DS_MAC_QOS: /* 208 */
        case DS_IPV4_QOS: /* 209 */
        case DS_MPLS_QOS: /* 210 */
            tbl_id = DS_ACL_MAC_KEY;
            break;
        case DS_IPV6_ACL: /* 207 */
        case DS_IPV6_QOS: /* 211 */
            tbl_id = DS_ACL_IPV6_KEY;
            break;
        case DS_IPV4_UCAST_DA: /* 212 */
        case DS_IPV4_UCAST_SA: /* 218 */
            tbl_id = DS_IPV4_UCAST_ROUTE_KEY;
            break;
        case DS_IPV4_SA_NAT: /* 222 */
            tbl_id = DS_IPV4_NAT_KEY;
            break;
        case DS_IPV4_MCAST_DA: /* 213 */
        case DS_IPV4_MCAST_RPF: /* 220 */
            tbl_id = DS_IPV4_MCAST_ROUTE_KEY;
            break;
        case DS_IPV6_MCAST_DA: /* 215 */
        case DS_IPV6_MCAST_RPF: /* 221 */
            tbl_id = DS_IPV6_MCAST_ROUTE_KEY;
            break;
        case DS_IPV6_UCAST_DA: /* 214 */
        case DS_IPV6_UCAST_SA: /* 219 */
            tbl_id = DS_IPV6_UCAST_ROUTE_KEY;
            break;
        case DS_IPV6_SA_NAT: /* 223 */
            tbl_id = DS_IPV6_NAT_KEY;
            break;
        case DS_IPV4_UCAST_PBR_DUAL_DA: /* 216 */
            tbl_id = DS_IPV4_PBR_DUALDA_KEY;
            break;
        case DS_IPV6_UCAST_PBR_DUAL_DA: /* 217 */
            tbl_id = DS_IPV6_PBR_DUALDA_KEY;
            break;
        case DS_MAC: /* 251 */
            tbl_id = DS_MAC_KEY;
            break;
        case DS_USER_ID_VLAN: /* 224 */
            tbl_id = DS_USER_ID_VLAN_KEY;
            break;
        case DS_USER_ID_MAC: /* 225 */
            tbl_id = DS_USER_ID_MAC_KEY;
            break;
        case DS_USER_ID_IPV4: /* 226 */
            tbl_id = DS_USER_ID_IPV4_KEY;
            break;
        case DS_USER_ID_IPV6: /* 227 */
            tbl_id = DS_USER_ID_IPV6_KEY;
            break;
        case DS_L2_EDIT_ETH4W: /* 228 */
        case DS_L2_EDIT_ETH8W: /* 229 */
        case DS_L2_EDIT_FLEX4W: /* 230 */
        case DS_L2_EDIT_FLEX8W: /* 231 */
        case DS_L2_EDIT_LOOPBACK: /* 232 */
        case DS_L2_EDIT_PBB8W: /* 233 */
        case DS_L2_EDIT_PBB4W: /* 234 */
            return ((alloc_info.is_l2edit_store_ext_sram) ? EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
            break;
        case DS_L3EDIT_MPLS4W: /* 235 */
        case DS_L3EDIT_MPLS8W: /* 236 */
        case DS_L3EDIT_NAT4W: /* 237 */
        case DS_L3EDIT_NAT8W: /* 238 */
        case DS_L3EDIT_TUNNEL_V4: /* 239 */
        case DS_L3EDIT_TUNNEL_V6: /* 240 */
        case DS_L3EDIT_FLEX: /* 241 */
        case DS_L3EDIT_LOOP_BACK: /* 242 */
            return ((alloc_info.is_l3edit_store_ext_sram) ? EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
            break;
        case DS_MPLS: /* 244 */
            return ((alloc_info.is_mpls_store_ext_sram) ? EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
            break;
        case DS_MET_ENTRY: /* 243 */
        case DS_FWD: /* 252 */
            return ((alloc_info.is_met_store_ext_sram) ? EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
            break;
        case DS_NEXTHOP: /* 245 */
        case DS_NEXTHOP8W: /* 246 */
            return ((alloc_info.is_nh_store_ext_sram) ? EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
            break;

        case DS_POLICER: /* 247 */
        case DS_FORWARDING_STATS: /* 248 */
            return SRAM_ASSOCIATE_DATA_BASE; /*need double check*/
            break;
        case DS_VLAN: /* 249 */
        case DS_VLAN_STATUS: /* 250 */
        case DS_ETH_OAM_CHAN: /* 253 */
        case DS_MPLS_PBT_OAM_CHAN: /* 254 */
        case DS_RMEP_CHAN: /* 255 */
        case DS_ETH_MEP: /* 256 */
        case DS_ETH_RMEP: /* 257 */
        case DS_MPLS_MEP: /* 258 */
        case DS_MPLS_RMEP: /* 259 */
            return SRAM_ASSOCIATE_DATA_BASE;
            break;
        default:
            break;
    }


    if (tbl_id >= DS_MAC_KEY &&  tbl_id <= DS_ETH_OAM_RMEP_HASH_KEY1 )
    {
         internal_key_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].internal_key_type;
         /*if use 96k hash ipuc & MAC association data store in external sram*/
          if ((0 == alloc_info.is_hash_48ksize) &&
                (internal_key_id == CTC_FTM_KEY_TYPE_IPV4_UCAST
            || internal_key_id == CTC_FTM_KEY_TYPE_IPV4_NAT
            || internal_key_id == CTC_FTM_KEY_TYPE_IPV4_PBR
            || internal_key_id == CTC_FTM_KEY_TYPE_IPV6_UCAST
            || internal_key_id == CTC_FTM_KEY_TYPE_IPV6_NAT
            || internal_key_id == CTC_FTM_KEY_TYPE_IPV6_PBR
            || internal_key_id == CTC_FTM_KEY_TYPE_FIB))
            {
                return EXT_SRAM_ASSOCIATE_DATA_BASE;
            }
            return ((alloc_info.tcam_info[internal_key_id].is_ext_tcam) ?EXT_SRAM_ASSOCIATE_DATA_BASE:SRAM_ASSOCIATE_DATA_BASE);
    }
    return SRAM_ASSOCIATE_DATA_BASE;
}


int32
sys_alloc_get_sram_table_base(uint32  tbl_id,sys_alloc_allocated_sram_info_t * sram_info)
{
    CTC_PTR_VALID_CHECK(sram_info);

    if (sys_alloc_get_sram_database(tbl_id) == SRAM_ASSOCIATE_DATA_BASE)
    {
        sram_info->is_ext_sram = 0;
        sram_info->table_base = ((drv_tbls_list[tbl_id].hw_data_base - SRAM_ASSOCIATE_DATA_BASE) / 16) >> 8;
    }
    else
    {
        sram_info->is_ext_sram = 1;
        if(alloc_info.is_hash_48ksize)
        {
            sram_info->table_base = ((drv_tbls_list[tbl_id].hw_data_base - EXT_SRAM_ASSOCIATE_DATA_BASE) / 16 + (16 + 96)*1024) >> 8;
        }
        else
        {
            sram_info->table_base = ((drv_tbls_list[tbl_id].hw_data_base - EXT_SRAM_ASSOCIATE_DATA_BASE) / 16 + (16 + 48)*1024) >> 8;
        }
    }

    return CTC_E_NONE;
}


INLINE uint32
_sys_alloc_get_tcam_key_index_shift(tbl_id_t  tcamkey)
{
    uint32 key_size = 0;

    key_size = drv_tbls_list[tcamkey].key_size;
    key_size = key_size / BYTES_PER_TCAM_ENTRY / 2;
    key_size = (key_size == 4)? 3 : (key_size & 0x3);

    return key_size;
}

int32
sys_alloc_get_tcam_key_alloc_info(uint32  tbl_id, sys_alloc_allocated_tcam_info_t *tcam_info)
{
    uint32 tcam_index_base = 0;
    uint32 index_base = 0;
    uint8 internal_key_id;

    CTC_VALUE_RANGE_CHECK(tbl_id, DS_MAC_KEY, DS_ETH_OAM_RMEP_HASH_KEY1);

    switch(tbl_id)
    {
        case DS_MAC_KEY:/* 260 */
        case DS_IPV4_UCAST_ROUTE_KEY:/* 271 */
        case DS_IPV4_MCAST_ROUTE_KEY:/* 272 */
        case DS_IPV6_UCAST_ROUTE_KEY:/* 279 */
        case DS_IPV6_MCAST_ROUTE_KEY:/* 282 */
        case DS_USER_ID_VLAN_KEY:/* 287 */
        case DS_USER_ID_MAC_KEY:/* 288 */
        case DS_USER_ID_IPV4_KEY:/* 289 */
        case DS_USER_ID_IPV6_KEY:/* 290 */
        case DS_IPV4_PBR_DUALDA_KEY:/* 274 */
        case DS_IPV6_PBR_DUALDA_KEY:/* 281 */
            {
                internal_key_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].internal_key_type;
                tcam_info->is_ext_tcam = alloc_info.tcam_info[internal_key_id].is_ext_tcam;
                tcam_info->ltr_id = tcam_info->is_ext_tcam ?alloc_info.tcam_info[internal_key_id].ltr_id:0;

                if (tbl_id == DS_MAC_KEY)
                {
                        tcam_info->is_hash_used =  (drv_tbls_list[DS_MAC_HASH_KEY0].max_index_num !=0) ;
                }
                else if(tbl_id == DS_IPV4_UCAST_ROUTE_KEY)
                {
                         tcam_info->is_hash_used =  (drv_tbls_list[DS_IPV4_UCAST_HASH_KEY0].max_index_num !=0) ;
                }
                else if(tbl_id == DS_IPV6_UCAST_ROUTE_KEY)
                {
                         tcam_info->is_hash_used =  (drv_tbls_list[DS_IPV6_UCAST_HASH_KEY0].max_index_num !=0) ;
                }
                else
                {
                    tcam_info->is_hash_used  = 0;
                }

                tcam_info->table_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].table_id;
                tcam_info->index_shift = _sys_alloc_get_tcam_key_index_shift(tbl_id);
                tcam_index_base = tcam_info->is_ext_tcam?EXT_TCAM_DATA_BASE:INT_TCAM_DATA_BASE;
                if (0 != drv_tbls_list[tbl_id].max_index_num)
                {
                    index_base = drv_tbls_list[tbl_id].hw_data_base;
                    index_base = ((index_base - tcam_index_base) / 16) >> 8;
                }
                else
                {
                    index_base = 0;
                }
                tcam_info->index_base = index_base;
                tcam_info->max_index = drv_tbls_list[tbl_id].max_index_num;
            }
            break;
        case DS_MAC_HASH_KEY0:/* 261 */
        case DS_MAC_HASH_KEY1:/* 262 */
        case DS_IPV4_UCAST_HASH_KEY0:/* 275 */
        case DS_IPV4_UCAST_HASH_KEY1:/* 276 */
        case DS_IPV6_UCAST_HASH_KEY1:/* 285 */
        case DS_IPV6_UCAST_HASH_KEY0:/* 283 */
            {
                internal_key_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].internal_key_type;
                tcam_info->is_ext_tcam = alloc_info.tcam_info[internal_key_id].is_ext_tcam;
                tcam_info->ltr_id = 0;
                tcam_info->is_hash_used =  (drv_tbls_list[tbl_id].max_index_num !=0) ;
                tcam_info->table_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].table_id;
                tcam_info->entry_size = drv_tbls_list[tbl_id].entry_size;
                tcam_info->max_index = drv_tbls_list[tbl_id].max_index_num;
                tcam_info->index_base = drv_tbls_list[tbl_id].hw_data_base;
            }
            break;
        case DS_ACL_MAC_KEY:/* 263 */
        case DS_QOS_MAC_KEY:/* 264 */
        case DS_ACL_IPV4_KEY:/* 265 */
        case DS_ACL_MPLS_KEY:/* 267 */
        case DS_QOS_IPV4_KEY:/* 266 */
        case DS_QOS_MPLS_KEY:/* 268 */
        case DS_ACL_IPV6_KEY:/* 269 */
        case DS_QOS_IPV6_KEY:/* 270 */
            {
                internal_key_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].internal_key_type;
                tcam_info->is_ext_tcam = alloc_info.tcam_info[internal_key_id].is_ext_tcam;
                if (tcam_info->is_ext_tcam )
                {
                    tcam_info->ltr_id = alloc_info.tcam_info[internal_key_id].ltr_id;

                }
                else
                {
                    tcam_info->ltr_id = alloc_info.is_aclqos_dual_lookup?1:0;
                }

                tcam_info->is_hash_used = 0;
                tcam_info->table_id = sys_tcam_key_info_list[tbl_id - DS_MAC_KEY].table_id;
                tcam_info->index_shift = _sys_alloc_get_tcam_key_index_shift(tbl_id);
                tcam_index_base = tcam_info->is_ext_tcam?EXT_TCAM_DATA_BASE:INT_TCAM_DATA_BASE;
                if (0 != drv_tbls_list[tbl_id].max_index_num)
                {
                    index_base  = drv_tbls_list[tbl_id].hw_data_base;
                    index_base  = ((index_base - tcam_index_base) / 16) >> 8;
                }
                else
                {
                    index_base = 0;
                }
                tcam_info->index_base = index_base;
                tcam_info->max_index = drv_tbls_list[tbl_id].max_index_num;
            }
            break;
        case DS_IPV4_MCAST_HASH_KEY0:/* 277 */
        case DS_IPV4_MCAST_HASH_KEY1:/* 278 */
        case DS_IPV6_MCAST_HASH_KEY0:/* 284 */
        case DS_IPV6_MCAST_HASH_KEY1:/* 286 */
        case DS_IPV4_NAT_KEY:/* 273 */
        case DS_IPV6_NAT_KEY:/* 280 */
            break;

        case DS_ETH_OAM_KEY:/* 291 */
        case DS_ETH_OAM_HASH_KEY0:/* 292 */
        case DS_ETH_OAM_HASH_KEY1:/* 293 */
        case DS_PBT_OAM_KEY:/* 294 */
        case DS_PBT_OAM_HASH_KEY0:/* 295 */
        case DS_PBT_OAM_HASH_KEY1:/* 296 */
        case DS_MPLS_OAM_LABEL_KEY:/* 297 */
        case DS_MPLS_OAM_LABEL_HASH_KEY0:/* 298 */
        case DS_MPLS_OAM_LABEL_HASH_KEY1:/* 299 */
        case DS_MPLS_OAM_IPV4_TTSI_KEY:/* 300 */
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY0:/* 301 */
        case DS_MPLS_OAM_IPV4_TTSI_HASH_KEY1:/* 302 */
        case DS_ETH_OAM_RMEP_KEY:/* 303 */
        case DS_ETH_OAM_RMEP_HASH_KEY0:/* 304 */
        case DS_ETH_OAM_RMEP_HASH_KEY1:/* 305 */
            break;
        default:
            break;
    }

    return CTC_E_NONE;
}

/* Allocation Sequence:  */
/* according to ctc_key_type_t */
int32
sys_alloc_associate_table(ctc_ftm_profile_info_t *profile_info)
{
    uint32 key_type = 0;
    uint32 i = 0;
    uint32 total_key_index = 0;
    uint32 associate_tbl_id = 0;
    uint32 tcam_key_tbl_id = 0;
    uint32 sram_base = 0;
    uint32 allocated_sram_entry = 0;
    uint32 allocated_ext_sram_entry = 0;
    tables_t *table_info_ptr = NULL;
    uint32 key_counter = 0;


    /* 按照 sys_alloc_tbl_id_list 数组里的顺序做初始化 */
    for(i = 0; i < CTC_FTM_KEY_TYPE_MAX; i++)
    {
        /* initial total index number */
        total_key_index = 0;
        if(sys_alloc_tbl_id_list[i].tcam_key_tbl_id == 0)
        {
            continue;
        }
        key_type = sys_alloc_tbl_id_list[i].key_type;
        for (key_counter = 0; key_counter < profile_info->key_info_size; key_counter ++)
        {
            if (key_type == profile_info->key_info[key_counter].key_id)
            {
                total_key_index += profile_info->key_info[key_counter].max_key_index;
            }
        }

        if (total_key_index == 0 || key_type == CTC_FTM_KEY_TYPE_OAM )
        {
            /* current KEY is invalid, no need allocation,
               turn to the next key instantly. */
            continue;
        }

        /* in case the index is less than 256, say 64, it must be changed to 256 */
        /* this operation will waste SRAM but can save TCAM */
        total_key_index = ((total_key_index - 1) / 256 + 1) * 256;

        associate_tbl_id = sys_alloc_tbl_id_list[i].associate_tbl_id;
        tcam_key_tbl_id = sys_alloc_tbl_id_list[i].tcam_key_tbl_id;

        table_info_ptr = &drv_tbls_list[associate_tbl_id];

        sram_base = sys_alloc_get_sram_database(tcam_key_tbl_id);

        if(EXT_SRAM_ASSOCIATE_DATA_BASE==sram_base)
        {
            /* The absolute address of a given table = Memory Base + (table offset * 16) */
            table_info_ptr->hw_data_base   = (allocated_ext_sram_entry << 4) + sram_base;
            table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
            table_info_ptr->max_index_num  = total_key_index;

            allocated_ext_sram_entry += total_key_index;
        }
        else
        {
            /* The absolute address of a given table = Memory Base + (table offset * 16) */
            table_info_ptr->hw_data_base   = (allocated_sram_entry << 4) + sram_base;
            table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
            table_info_ptr->max_index_num  = total_key_index;

            allocated_sram_entry += total_key_index;

            if (alloc_info.disable_merge_mac_ip_key_physical)
            {
                if (DS_IPV4_QOS == associate_tbl_id)
                {
                    allocated_sram_entry += total_key_index;
                }
            }
        }


            /* one RPF KEY can reference two associate tables */
           associate_tbl_id = sys_alloc_tbl_id_list[i].associate_tbl_id_rpf;


           if (TRUE == CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_IPUC_SA_EN)
            || associate_tbl_id == DS_IPV4_MCAST_RPF
            ||  associate_tbl_id == DS_IPV6_MCAST_RPF)
           {
               if (0 != associate_tbl_id)
               {
                /* There's a valid RPF table for current KEY */
                table_info_ptr  = &drv_tbls_list[associate_tbl_id];

                if(EXT_SRAM_ASSOCIATE_DATA_BASE==sram_base)
                {
                    /* The absolute address of a given table = Memory Base + (table offset * 16) */
                    table_info_ptr->hw_data_base   = (allocated_ext_sram_entry << 4) + sram_base;
                    table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
                    table_info_ptr->max_index_num  = total_key_index;

                    allocated_ext_sram_entry += total_key_index;
                }
                else
                {
                    /* The absolute address of a given table = Memory Base + (table offset * 16) */
                    table_info_ptr->hw_data_base   = (allocated_sram_entry << 4) + sram_base;
                    table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
                    table_info_ptr->max_index_num  = total_key_index;

                    allocated_sram_entry += total_key_index;
                }
            }
        }

    }
    /* record sram usage, which is shared by Associate and Dyanmic Table. */
    alloc_info.current_sram_offset = allocated_sram_entry;
    alloc_info.current_ext_sram_offset = allocated_ext_sram_entry;

    /* adjust mac/ip aclQos table, considering mac/ip key merging. */
    _sys_alloc_init_acl_qos_associate();
    alloc_info.sram_for_tcam_and_hash_num = allocated_sram_entry + allocated_ext_sram_entry;

    /*_sys_alloc_init_vlan_associate(profile_info);*/

    return CTC_E_NONE;
}

int32
sys_alloc_mpls_table(ctc_ftm_profile_info_t *profile_info)
{
    uint8 ext_sram = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_MPLS_IN_EXT_SRAM);
    uint32 allocated_sram_entry = 0;
    uint32 mpls_table_entry = profile_info->table_size[CTC_FTM_TBL_TYPE_MPLS];
    uint32 sram_physical_size = 0;
    tables_t *table_info_ptr = NULL;

    /*mpls store in ext sram*/
    if(ext_sram)
    {
        allocated_sram_entry = alloc_info.current_ext_sram_offset;
        sram_physical_size = profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_SRAM_SIZE];
    }
    else /*mpls store in int sram*/
    {
        allocated_sram_entry = alloc_info.current_sram_offset;
        sram_physical_size = profile_info->table_size[CTC_FTM_TBL_TYPE_ASS_DYN_SRAM];
    }



    if (0 != mpls_table_entry)
    {
        if ((allocated_sram_entry +mpls_table_entry) > sram_physical_size)
        {
            /* exceeds Sram physical limitalion. */
            SYS_ALLOC_DBG_INFO("There's no more internal sram for table allocation!\n");
            return CTC_E_NO_MEMORY;
        }

        if (allocated_sram_entry % 256 != 0)
        {
            allocated_sram_entry = (allocated_sram_entry / 256 + 1) * 256;
        }

        table_info_ptr = &drv_tbls_list[DS_MPLS];

        if(ext_sram)
        {
            table_info_ptr->hw_data_base = (allocated_sram_entry << 4) + EXT_SRAM_ASSOCIATE_DATA_BASE;
        }
        else
        {
            table_info_ptr->hw_data_base = (allocated_sram_entry << 4) + SRAM_ASSOCIATE_DATA_BASE;
        }
        table_info_ptr->hw_mask_base    = INVALID_MASK_OFFSET;
        table_info_ptr->max_index_num   = mpls_table_entry;

        allocated_sram_entry            += mpls_table_entry;
        alloc_info.mpls_num = mpls_table_entry;
        if(ext_sram)
        {
            alloc_info.current_ext_sram_offset = allocated_sram_entry;
        }
        else
        {
            alloc_info.current_sram_offset = allocated_sram_entry;
        }
    }
    else
    {
        /* table entry == 0, means Mpls is disabled, do nothing. */
    }

    return CTC_E_NONE;
}

/* Dynamica Table: */
/* 1/6 is global met and nexthop */
/* 2/6 is local met and nexthop */
/* 3/6 is local dynamic table */
int32
sys_alloc_dynamic_table(ctc_ftm_profile_info_t *profile_info)
{
    uint32 allocated_sram_entry = alloc_info.current_sram_offset;
    uint32 allocated_ext_sram_entry = alloc_info.current_ext_sram_offset;
    uint32 sram_physical_size = profile_info->table_size[CTC_FTM_TBL_TYPE_ASS_DYN_SRAM];
    uint32 ext_sram_physical_size = profile_info->table_size[CTC_FTM_TBL_TYPE_EXT_SRAM_SIZE];
    uint32 dynamic_tbl_index = 0;
    uint32 local_dynamic_tbl_id = 0;
    uint32 nexthop_entry_num = 0;

    tables_t *table_info_ptr = NULL;

    tbl_id_t l2edit_tbl_list[] =
        {DS_L2_EDIT_ETH4W,
         DS_L2_EDIT_ETH8W,
         DS_L2_EDIT_FLEX4W,
         DS_L2_EDIT_FLEX8W,
         DS_L2_EDIT_LOOPBACK,
         DS_L2_EDIT_PBB4W,
         DS_L2_EDIT_PBB8W,
         0};

    tbl_id_t l3edit_tbl_list[] =
        {DS_L3EDIT_MPLS4W,
         DS_L3EDIT_MPLS8W,
         DS_L3EDIT_NAT4W,
         DS_L3EDIT_NAT8W,
         DS_L3EDIT_TUNNEL_V4,
         DS_L3EDIT_TUNNEL_V6,
         DS_L3EDIT_LOOP_BACK,
         DS_L3EDIT_FLEX,
         0};

    alloc_info.global_met_entry_num = profile_info->table_size[CTC_FTM_TBL_TYPE_GLB_MET];
    alloc_info.global_nh_entry_num = profile_info->table_size[CTC_FTM_TBL_TYPE_GLB_NH];
    alloc_info.local_nh_entry_num = profile_info->table_size[CTC_FTM_TBL_TYPE_LOCAL_NH];
    alloc_info.l2edit_entry_num = profile_info->table_size[CTC_FTM_TBL_TYPE_L2EDIT];
    alloc_info.l3edit_entry_num = profile_info->table_size[CTC_FTM_TBL_TYPE_L3EDIT];
    alloc_info.ip_tunnel_table_size = profile_info->table_size[CTC_FTM_TBL_TYPE_IP_TUNNEL];

    if (allocated_sram_entry % 256 != 0)
    {
        allocated_sram_entry = (allocated_sram_entry / 256 + 1) * 256;
    }

    if(allocated_ext_sram_entry % 256 != 0)
    {
        allocated_ext_sram_entry = (allocated_ext_sram_entry / 256 + 1) * 256;
    }

    if (allocated_sram_entry > sram_physical_size)
    {
        /* exceeds int Sram physical limitalion. */
        SYS_ALLOC_DBG_INFO("There's no more int sram for table allocation!\n");
        return CTC_E_NO_MEMORY;
    }

    if (allocated_ext_sram_entry > ext_sram_physical_size)
    {
        /* exceeds ext Sram physical limitalion. */
        SYS_ALLOC_DBG_INFO("There's no more ext sram for table allocation!\n");
        return CTC_E_NO_MEMORY;
    }

    /* init nexthop table. */
    nexthop_entry_num = alloc_info.global_nh_entry_num + alloc_info.local_nh_entry_num;
    if(TRUE == CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_NH_IN_EXT_SRAM))
    {
        alloc_info.nh_table_base = (allocated_ext_sram_entry << 4) + EXT_SRAM_ASSOCIATE_DATA_BASE;
        allocated_ext_sram_entry += nexthop_entry_num;
        if(allocated_ext_sram_entry % 256 != 0)
        {
            allocated_ext_sram_entry = (allocated_ext_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_ext_sram_offset = allocated_ext_sram_entry;
    }
    else
    {
        alloc_info.nh_table_base = (allocated_sram_entry << 4) + SRAM_ASSOCIATE_DATA_BASE;
        allocated_sram_entry += nexthop_entry_num;
        if (allocated_sram_entry % 256 != 0)
        {
            allocated_sram_entry = (allocated_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_sram_offset = allocated_sram_entry;
    }

    table_info_ptr = &drv_tbls_list[DS_NEXTHOP];
    table_info_ptr->hw_mask_base = INVALID_MASK_OFFSET;
    table_info_ptr->hw_data_base = alloc_info.nh_table_base;
    table_info_ptr->max_index_num = nexthop_entry_num;

    table_info_ptr = &drv_tbls_list[DS_NEXTHOP8W];
    table_info_ptr->hw_mask_base = INVALID_MASK_OFFSET;
    table_info_ptr->hw_data_base = alloc_info.nh_table_base;
    table_info_ptr->max_index_num = nexthop_entry_num;

    /* init l2edit table. */
    if(TRUE == CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_L2EDIT_IN_EXT_SRAM))
    {
        alloc_info.l2edit_table_base = (allocated_ext_sram_entry << 4) + EXT_SRAM_ASSOCIATE_DATA_BASE;
        allocated_ext_sram_entry += alloc_info.l2edit_entry_num;
        if(allocated_ext_sram_entry % 256 != 0)
        {
            allocated_ext_sram_entry = (allocated_ext_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_ext_sram_offset = allocated_ext_sram_entry;
    }
    else
    {
        alloc_info.l2edit_table_base = (allocated_sram_entry << 4) + SRAM_ASSOCIATE_DATA_BASE;
        allocated_sram_entry += alloc_info.l2edit_entry_num;
        if (allocated_sram_entry % 256 != 0)
        {
            allocated_sram_entry = (allocated_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_sram_offset = allocated_sram_entry;
    }

    dynamic_tbl_index = 0;
    local_dynamic_tbl_id = l2edit_tbl_list[dynamic_tbl_index];
    while (DYNAMIC_TABLE_LIST_END != local_dynamic_tbl_id)
    {
        /* value 0 means dynamic table list reaches its end; */
        table_info_ptr = &drv_tbls_list[local_dynamic_tbl_id];
        table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
        table_info_ptr->hw_data_base   = alloc_info.l2edit_table_base;
        table_info_ptr->max_index_num  = alloc_info.l2edit_entry_num;

        dynamic_tbl_index ++;
        local_dynamic_tbl_id = l2edit_tbl_list[dynamic_tbl_index];
    }

    /* init l3edit table. */
    if(TRUE == CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_L3EDIT_IN_EXT_SRAM))
    {
        alloc_info.l3edit_table_base = (allocated_ext_sram_entry << 4) + EXT_SRAM_ASSOCIATE_DATA_BASE;
        allocated_ext_sram_entry += alloc_info.l3edit_entry_num;
        if(allocated_ext_sram_entry % 256 != 0)
        {
            allocated_ext_sram_entry = (allocated_ext_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_ext_sram_offset = allocated_ext_sram_entry;
    }
    else
    {
        alloc_info.l3edit_table_base = (allocated_sram_entry << 4) + SRAM_ASSOCIATE_DATA_BASE;
        allocated_sram_entry += alloc_info.l3edit_entry_num;
        if (allocated_sram_entry % 256 != 0)
        {
            allocated_sram_entry = (allocated_sram_entry / 256 + 1) * 256;
        }
        alloc_info.current_sram_offset = allocated_sram_entry;
    }

    dynamic_tbl_index = 0;
    local_dynamic_tbl_id = l3edit_tbl_list[dynamic_tbl_index];
    while (DYNAMIC_TABLE_LIST_END != local_dynamic_tbl_id)
    {
        /* value 0 means dynamic table list reaches its end; */
        table_info_ptr = &drv_tbls_list[local_dynamic_tbl_id];
        table_info_ptr->hw_mask_base   = INVALID_MASK_OFFSET;
        table_info_ptr->hw_data_base   = alloc_info.l3edit_table_base;
        table_info_ptr->max_index_num  = alloc_info.l3edit_entry_num;

        dynamic_tbl_index ++;
        local_dynamic_tbl_id = l3edit_tbl_list[dynamic_tbl_index];
    }

    /* init met and dsfwd table. */
    if(TRUE == CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_MET_IN_EXT_SRAM))
    {
        alloc_info.met_table_base = (allocated_ext_sram_entry << 4) + EXT_SRAM_ASSOCIATE_DATA_BASE;
        if (allocated_ext_sram_entry + alloc_info.global_met_entry_num >= ext_sram_physical_size)
        {
            return CTC_E_NO_MEMORY;
        }
        alloc_info.local_met_dsfwd_entry_num = ext_sram_physical_size - allocated_ext_sram_entry - alloc_info.global_met_entry_num;
        alloc_info.current_ext_sram_offset += (ext_sram_physical_size - allocated_ext_sram_entry);
    }
    else
    {
        alloc_info.met_table_base = (allocated_sram_entry << 4) + SRAM_ASSOCIATE_DATA_BASE;
        if (allocated_sram_entry + alloc_info.global_met_entry_num >= sram_physical_size)
        {
            return CTC_E_NO_MEMORY;
        }
        alloc_info.local_met_dsfwd_entry_num = sram_physical_size - allocated_sram_entry - alloc_info.global_met_entry_num;
        alloc_info.current_sram_offset += (sram_physical_size - allocated_sram_entry);
    }

    table_info_ptr = &drv_tbls_list[DS_MET_ENTRY];
    table_info_ptr->hw_mask_base = INVALID_MASK_OFFSET;
    table_info_ptr->hw_data_base = alloc_info.met_table_base;
    table_info_ptr->max_index_num = alloc_info.global_met_entry_num + alloc_info.local_met_dsfwd_entry_num;

    table_info_ptr = &drv_tbls_list[DS_FWD];
    table_info_ptr->hw_mask_base = INVALID_MASK_OFFSET;
    table_info_ptr->hw_data_base = alloc_info.met_table_base;
    table_info_ptr->max_index_num = alloc_info.global_met_entry_num + alloc_info.local_met_dsfwd_entry_num;

    return CTC_E_NONE;
}

int32
sys_alloc_ext_qdr_table(ctc_ftm_profile_info_t *profile_info)
{
    if((profile_info->table_size[CTC_FTM_TBL_TYPE_FLOW_PLOCIER]+profile_info->table_size[CTC_FTM_TBL_TYPE_FWD_STATS]-4096*2) > SYS_EXT_QDR_PHYSICAL_SIZE)
    {
        SYS_ALLOC_DBG_INFO("There's no more ext qdr for table allocation!\n");
        return CTC_E_NO_MEMORY;
    }

    alloc_info.is_ext_qdr_en = CTC_FLAG_ISSET(profile_info->flag, CTC_FTM_FLAG_EXT_QDR_EN);
    drv_tbls_list[DS_POLICER].max_index_num = profile_info->table_size[CTC_FTM_TBL_TYPE_FLOW_PLOCIER];
    drv_tbls_list[DS_FORWARDING_STATS].max_index_num = profile_info->table_size[CTC_FTM_TBL_TYPE_FWD_STATS];

    return CTC_E_NONE;
}
static int32
_sys_alloc_ibm_internal_tcam_initialize(void)
{
    uint32 cmd, init_done = 0;
    tcam_ctl_int_init_ctrl_t init_control;
    uint32 timeout = 10000;
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    uint32 value = 0;
    uint8 sup_blk = 0;
    uint32 qos_bitmap = 0;

    local_chip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        /* clean up */
        kal_memset(&init_control, 0, sizeof(init_control));

        /* write initialize control */
        init_control.cfg_init_start_addr = 0;
        init_control.cfg_init_end_addr = 16 * 1024 - 1;
        init_control.cfg_init_en = 1;

        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_INIT_CTRL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_control));

        /* polling CPU request send completed */
        while (!init_done)
        {
            cmd = DRV_IOR(IOC_REG, TCAM_CTL_INT_INIT_CTRL, TCAM_CTL_INT_INIT_CTRL_CFG_INIT_DONE);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_done));
            timeout--;
            if (!timeout)
            {
                SYS_ALLOC_DBG_INFO("IBM internal TCAM Init Timeout!!\n");
                break;
            }
        }

        /* clean tcamInitControl register */
        kal_memset(&init_control, 0, sizeof(init_control));
        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_INIT_CTRL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_control));

        /*init lookup block*/
        value = 0xFFFF;
        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_KEY_SIZE_CFG, TCAM_CTL_INT_KEY_SIZE_CFG_KEY80_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        value = 0xFF;
        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_KEY_SIZE_CFG, TCAM_CTL_INT_KEY_SIZE_CFG_KEY160_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        value = 0xF;
        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_KEY_SIZE_CFG, TCAM_CTL_INT_KEY_SIZE_CFG_KEY320_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

        /*init qos lookup block*/
        for(sup_blk=0; sup_blk<4; sup_blk++)
        {
            if(alloc_info.qos_group&(1<<sup_blk))
            {
                qos_bitmap |= (1<<(sup_blk*2));
                qos_bitmap |= (1<<(sup_blk*2+1));
            }
        }

        cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_KEY_TYPE_CFG, TCAM_CTL_INT_KEY_TYPE_CFG_QOS_KEY_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &qos_bitmap));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_bitmap_gen(uint8 start, uint8 end, uint8 value, uint8 shift, uint32* bitmap)
{
    uint8 index = 0;

    CTC_PTR_VALID_CHECK(bitmap);

    for(index=start; index<=end; index++)
    {
        *bitmap |= value<<(shift*index);
    }

    return CTC_E_NONE;
}
static int32
_sys_alloc_nl9k_external_tcam_initialize(void)
{

    uint8 lchip_num = 0, lchip = 0, key_select = 0, port_select = 0;
    uint16 blk_id = 0, start_blk_id = 0, end_blk_id = 0;
    uint32 cmd = 0, init_done = 0, timeout = 10000;
    uint32 key_type = 0, ltr_id = 0, bitmap = 0;
    uint32 index = 0;

    tcam_ctl_ext_init_ctl_t init_control;
    nl9k_ctl_reg_ltr_t ext_tcam_cfg, ext_tcam_org_cfg;
    nl9k_ctl_reg_bmr_t bmr_t;
    nl9k_ctl_reg_bcr_t bcr_t;

    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* clean up */
        kal_memset(&init_control, 0, sizeof(init_control));

        /* write initialize control */
        init_control.cfg_init_start_addr = 0;
        init_control.cfg_init_end_addr = 64 * 1024 - 1;
        init_control.cfg_init_en = 1;

        cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_INIT_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_control));

        /* polling CPU request send completed */
        while (!init_done)
        {
            cmd = DRV_IOR(IOC_REG, TCAM_CTL_EXT_INIT_CTL, TCAM_CTL_EXT_INIT_CTL_CFG_INIT_DONE);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_done));
            timeout--;
            if (!timeout)
            {
                SYS_ALLOC_DBG_INFO("Netlogic 9K external TCAM Init Timeout!!\n");
                break;
            }
        }

        /* clean tcamInitControl register */
        kal_memset(&init_control, 0, sizeof(init_control));
        cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_INIT_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_control));
    }


    /*config netlogic9000 with different key type*/
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        for(key_type=0; key_type<CTC_FTM_KEY_TYPE_MAX; key_type++)
        {
            if(alloc_info.tcam_info[key_type].is_ext_tcam)
            {
                kal_memset(&bmr_t, 0, sizeof(bmr_t));
                kal_memset(&bcr_t, 0, sizeof(bcr_t));
                kal_memset(&ext_tcam_cfg, 0, sizeof(nl9k_ctl_reg_ltr_t));
                kal_memset(&ext_tcam_org_cfg, 0, sizeof(nl9k_ctl_reg_ltr_t));

                ltr_id = alloc_info.tcam_info[key_type].ltr_id;
                start_blk_id = alloc_info.tcam_info[key_type].start_block_id;
                end_blk_id = alloc_info.tcam_info[key_type].end_block_id;
                key_select = alloc_info.tcam_info[key_type].blk_width;

                /*parallel search result port select, acl key high index,other key low index,include qos key*/
                /*port 0->high 20 bit, port 1->low 20 bit*/
                if((CTC_FTM_KEY_TYPE_ACL_MAC_IPV4==key_type) || (CTC_FTM_KEY_TYPE_ACL_IPV6==key_type))
                {
                    port_select = 0;
                }
                else
                {
                    port_select = 1;
                }

                if(end_blk_id<32)
                {
                    /*config BSR Register#0*/
                    bitmap = 0;
                    CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id, end_blk_id, 1, 1, &bitmap));
                    ext_tcam_cfg.bsr0_31_to0 = bitmap;
                    bitmap = 0;

                    if(end_blk_id<16)
                    {
                        /*config PSR Register#0 parallel search block select 0~15*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id, end_blk_id, port_select, 2, &bitmap));
                        ext_tcam_cfg.psr0_31_to0 = bitmap;
                        bitmap = 0;
                    }
                    else
                    {
                        if(start_blk_id>=16)
                        {
                            /*config PSR Register#0 parallel search block select 16~31*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-16, end_blk_id-16, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_63_to32 = bitmap;
                            bitmap = 0;
                        }
                        else
                        {
                            /*config PSR Register#0 parallel search block select 0~15*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id, 15, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_31_to0 = bitmap;
                            bitmap = 0;

                            /*config PSR Register#0 parallel search block select 16~31*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id-16, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_63_to32 = bitmap;
                            bitmap = 0;
                        }
                    }

                    /*config PSR Register#0 super block key select 0~7*/
                    CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id/4, end_blk_id/4, key_select, 2, &bitmap));
                    ext_tcam_cfg.psr0_79_to64 = bitmap;
                    bitmap = 0;
                }
                else
                {
                    if(start_blk_id>=32)
                    {
                        /*config BSR Register#0*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-32, end_blk_id-32, 1, 1, &bitmap));
                        ext_tcam_cfg.bsr0_63_to32 = bitmap;
                        bitmap = 0;

                        if(end_blk_id<48)
                        {
                            /*config PSR Register#1 parallel search block select 32~47*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-32, end_blk_id-32, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr1_31_to0 = bitmap;
                            bitmap = 0;
                        }
                        else
                        {
                            if(start_blk_id>=48)
                            {
                                /*config PSR Register#1 parallel search block select 48~63*/
                                CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-48, end_blk_id-48, port_select, 2, &bitmap));
                                ext_tcam_cfg.psr1_63_to32 = bitmap;
                                bitmap = 0;
                            }
                            else
                            {
                                /*config PSR Register#1 parallel search block select 32~47*/
                                CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-32, 15, port_select, 2, &bitmap));
                                ext_tcam_cfg.psr1_31_to0 = bitmap;
                                bitmap = 0;

                                /*config PSR Register#1 parallel search block select 48~63*/
                                CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id-48, port_select, 2, &bitmap));
                                ext_tcam_cfg.psr1_63_to32 = bitmap;
                                bitmap = 0;
                            }
                        }

                        /*config PSR Register#1 super block key select 8~15*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id/4-8, end_blk_id/4-8, key_select, 2, &bitmap));
                        ext_tcam_cfg.psr1_79_to64 = bitmap;
                        bitmap = 0;
                    }
                    else
                    {
                        /*config BSR*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id, 31, 1, 1, &bitmap));
                        ext_tcam_cfg.bsr0_31_to0 = bitmap;
                        bitmap = 0;

                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id-32, 1, 1, &bitmap));
                        ext_tcam_cfg.bsr0_63_to32 = bitmap;
                        bitmap = 0;

                        if(start_blk_id>=16)
                        {
                            /*config PSR Register#0 parallel search block select 16~31*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id-16, end_blk_id-16, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_63_to32 = bitmap;
                            bitmap = 0;
                        }
                        else
                        {
                            /*config PSR Register#0 parallel search block select 0~15*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id, 15, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_31_to0 = bitmap;
                            bitmap = 0;

                            /*config PSR Register#0 parallel search block select 16~31*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, 15, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr0_63_to32 = bitmap;
                            bitmap = 0;
                        }

                        if(end_blk_id<48)
                        {
                            /*config PSR Register#1 parallel search block select 32~47*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id-32, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr1_31_to0 = bitmap;
                            bitmap = 0;
                        }
                        else
                        {
                            /*config PSR Register#1 parallel search block select 32~47*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, 15, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr1_31_to0 = bitmap;
                            bitmap = 0;

                            /*config PSR Register#1 parallel search block select 48~63*/
                            CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id-48, port_select, 2, &bitmap));
                            ext_tcam_cfg.psr1_63_to32 = bitmap;
                            bitmap = 0;
                        }

                        /*config PSR Register#0 super block key select 0~7*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(start_blk_id/4, 7, key_select, 2, &bitmap));
                        ext_tcam_cfg.psr0_79_to64 = bitmap;
                        bitmap = 0;

                        /*config PSR Register#1 super block key select 8~15*/
                        CTC_ERROR_RETURN(_sys_humber_bitmap_gen(0, end_blk_id/4-8, key_select, 2, &bitmap));
                        ext_tcam_cfg.psr1_79_to64 = bitmap;
                        bitmap = 0;
                    }
                }

                /*config KCR*/
                /*KPU#0 for 80bit key:0x3, KPU#1 for 160bit key:0xF,
                  KPU#2 for 320bit key:0xFF, KPU#3 for 640bit key:0xFFFF*/
                ext_tcam_cfg.kcr_31_to0= 0xF0003;
                ext_tcam_cfg.kcr_63_to32 = 0xFFFF00FF;

                if((ACLQOS_MAC_IPV4_DUAL_LOOKUP_LTR_ID == ltr_id) || (ACLQOS_IPV6_DUAL_LOOKUP_LTR_ID == ltr_id))
                {
                    cmd = DRV_IOR(IOC_REG, NL9K_CTL_REG_LTR, DRV_ENTRY_FLAG);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, ltr_id, cmd, &ext_tcam_org_cfg));

                    ext_tcam_cfg.bsr0_31_to0 |= ext_tcam_org_cfg.bsr0_31_to0;
                    ext_tcam_cfg.bsr0_63_to32 |= ext_tcam_org_cfg.bsr0_63_to32;
                    ext_tcam_cfg.bsr0_79_to64 |= ext_tcam_org_cfg.bsr0_79_to64;
                    ext_tcam_cfg.psr0_31_to0 |= ext_tcam_org_cfg.psr0_31_to0;
                    ext_tcam_cfg.psr0_63_to32 |= ext_tcam_org_cfg.psr0_63_to32;
                    ext_tcam_cfg.psr0_79_to64 |= ext_tcam_org_cfg.psr0_79_to64;
                    ext_tcam_cfg.psr1_31_to0 |= ext_tcam_org_cfg.psr1_31_to0;
                    ext_tcam_cfg.psr1_63_to32 |= ext_tcam_org_cfg.psr1_63_to32;
                    ext_tcam_cfg.psr1_79_to64 |= ext_tcam_org_cfg.psr1_79_to64;
                }

                cmd = DRV_IOW(IOC_REG, NL9K_CTL_REG_LTR, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, ltr_id, cmd, &ext_tcam_cfg));

                /*config BCR*/
                for(blk_id=start_blk_id; blk_id<=end_blk_id; blk_id++)
                {
                    for(index=blk_id*16; index<(blk_id+1)*16; index++)
                    {
                        /*BMR0~BMR3*/
                        cmd = DRV_IOW(IOC_REG, NL9K_CTL_REG_BMR0, DRV_ENTRY_FLAG);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, index, cmd, &bmr_t));
                    }

                    for(index=blk_id*4; index<(blk_id+1)*4; index++)
                    {
                        /*BMR4*/
                        cmd = DRV_IOW(IOC_REG, NL9K_CTL_REG_BMR1, DRV_ENTRY_FLAG);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, index, cmd, &bmr_t));
                    }

                    bcr_t.blk_en = 1;
                    bcr_t.blk_width = key_select;
                    cmd = DRV_IOW(IOC_REG, NL9K_CTL_REG_BCR, DRV_ENTRY_FLAG);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, blk_id, cmd, &bcr_t));
                }
            }
        }
    }

    return CTC_E_NONE;
}

int32
sys_alloc_tcam_initialize(void)
{
    uint8 lchip_num = 0, lchip = 0;
    uint32 cmd = 0, tmp = 0;

    if(drv_humber_ext_tcam_is_enable())
    {
        /*ext tcam index would add this index base,so should set to 0*/
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, TCAM_ARB_EXT_INDEX_BASE, TCAM_ARB_EXT_INDEX_BASE_EXT_INDEX_BASE);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }
    }

    CTC_ERROR_RETURN(_sys_alloc_ibm_internal_tcam_initialize());

    if(drv_humber_ext_tcam_is_enable())
    {
        CTC_ERROR_RETURN(_sys_alloc_nl9k_external_tcam_initialize());
    }

    return CTC_E_NONE;
}

int32
sys_alloc_hash_mem_initialize(void)
{
    uint32 cmd, init_done = 0;
    hash_ds_ctl_init_ctl_t init_control;
    uint32 timeout = 10000;
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    uint32 hash_key_sel = 1;
    uint32 hash_ram_init_vlaue = 1;


    if (alloc_info.is_hash_48ksize)
    {
        hash_key_sel = 0;
    }

    local_chip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        cmd = DRV_IOW(IOC_REG, HASH_KEY_SELECT, HASH_KEY_SELECT_CFG_HASH_KEY_SELECT98K);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &hash_key_sel));

        hash_ram_init_vlaue = 1;
        cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_MISC_CTL, HASH_DS_CTL_MISC_CTL_HASH_TAB_INIT_VALUE_SEL);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &hash_ram_init_vlaue));

        /* clean up */
        kal_memset(&init_control, 0, sizeof(init_control));

        /* write initialize control */
        init_control.hash_tab_init_start_addr = 0;
        if (hash_key_sel)
        {
            init_control.hash_tab_init_end_addr = 96 * 1024 - 1;
        }
        else
        {
            init_control.hash_tab_init_end_addr = 48 * 1024 - 1;
        }
        init_control.hash_tab_init_en = 1;

        cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_INIT_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_control));

        /* polling CPU request send completed */
        while (!init_done)
        {
            cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_INIT_CTL, HASH_DS_CTL_INIT_CTL_HASH_TAB_INIT_DONE);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &init_done));
            timeout--;
            if (!timeout)
            {
                SYS_ALLOC_DBG_INFO("Hash Ram Init Timeout!!\n");
                return CTC_E_HASH_MEM_INIT_TIMEOUT;
            }
        }

    }

    return CTC_E_NONE;
}

int32
sys_humber_mem_alloc(ctc_ftm_profile_info_t *profile_info)
{
    drv_work_platform_type_t platform_type;

    CTC_PTR_VALID_CHECK(profile_info);

    CTC_ERROR_RETURN(sys_alloc_check_key_space(profile_info));
    CTC_ERROR_RETURN(sys_alloc_tcam_key(profile_info));
    CTC_ERROR_RETURN(sys_alloc_hash_key(profile_info));
    CTC_ERROR_RETURN(sys_alloc_associate_table(profile_info));
    CTC_ERROR_RETURN(sys_alloc_mpls_table(profile_info));
    CTC_ERROR_RETURN(sys_alloc_dynamic_table(profile_info));
    CTC_ERROR_RETURN(sys_alloc_ext_qdr_table(profile_info));
    drv_get_platform_type(&platform_type);
    if (platform_type == HW_PLATFORM )
    {
        CTC_ERROR_RETURN(sys_alloc_tcam_initialize());
        CTC_ERROR_RETURN(sys_alloc_hash_mem_initialize());
    }


    return CTC_E_NONE;
}
