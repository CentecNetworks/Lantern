/**
 @file sys_humber_usrid.c

 @date 2009-10-19

 @version v2.0

 The file contains all usrid APIs of sys layer
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"

#include "sys_humber_usrid.h"
#include "sys_humber_usrid_db.h"
#include "sys_humber_chip.h"
#include "sys_humber_port.h"
#include "sys_humber_ftm.h"

#include "drv_io.h"
#include "drv_humber.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
typedef ds_user_id_vlan_t ds_user_id_t;

#define SYS_VLAN_MIN                   1
#define SYS_VLAN_MAX                   4094
#define SYS_VLAN_MASK_ALL              0xFFF
#define SYS_VLAN_SPLIT_MAX             20
#define SYS_VLANNODE_SINGLE            1
#define SYS_VLANNODE_MULTI             2
sys_usrid_mutex_t *p_mutex = NULL;

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
static int32
_sys_humber_usrid_add_vlan_default_entry_global(void)
{
    uint32 cmd_key, cmd_ds;
    uint8 i, local_chip_num;
    tbl_entry_t usrid_entry;
    uint32 max_entry_num;
    uint32 key_offset;
    ds_user_id_vlan_key_t usrid_vlankey_data, usrid_vlankey_mask;
    ds_user_id_vlan_t usrid_vlan_ds;

    kal_memset(&usrid_vlankey_data, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_vlankey_mask, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_vlan_ds, 0, sizeof(ds_user_id_vlan_t));

    /*build key & mask*/
    usrid_vlankey_data.table_id = USERID_VLAN_TABLEID_C;
    usrid_vlankey_mask.table_id = 0xF;

    /*build usrid ds : do-nothing*/
    usrid_vlan_ds.user_vlan_ptr = 0x1FFF;
    usrid_vlan_ds.binding_datah = 0x1FFF;

    usrid_vlan_ds.binding_datam = 0xBFFF;

    usrid_vlan_ds.ds_fwd_ptr_valid = 0;
    usrid_vlan_ds.binding_datal = 0xFFFF;

    /*write asic*/
    local_chip_num = sys_humber_get_local_chip_num();

    sys_alloc_get_table_entry_num(DS_USER_ID_VLAN_KEY, &max_entry_num);
    key_offset = max_entry_num - 1;

    usrid_entry.data_entry = (uint32 *)&usrid_vlankey_data;
    usrid_entry.mask_entry = (uint32 *)&usrid_vlankey_mask;

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        /*ds*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_ds, &usrid_vlan_ds));

        /*key & mask*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_key, &usrid_entry));
    }

    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_add_vlan_default_entry_global_for_srv_en(void)
{
    uint32 cmd_key, cmd_ds;
    uint8 i, local_chip_num;
    tbl_entry_t usrid_entry;
    uint32 max_entry_num;
    uint32 key_offset;
    ds_user_id_vlan_key_t usrid_vlankey_data, usrid_vlankey_mask;
    ds_user_id_vlan_t usrid_vlan_ds;

    kal_memset(&usrid_vlankey_data, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_vlankey_mask, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_vlan_ds, 0, sizeof(ds_user_id_vlan_t));

    /*build key & mask*/
    usrid_vlankey_data.table_id = USERID_VLAN_TABLEID_C;
    usrid_vlankey_mask.table_id = 0xF;

    /*build usrid ds : src_aclqos_en */
    usrid_vlan_ds.user_vlan_ptr = 0x1FFF;
    usrid_vlan_ds.binding_datah = 0x9FFF;

    usrid_vlan_ds.binding_datam = 0x8000;

    usrid_vlan_ds.ds_fwd_ptr_valid = 0;
    usrid_vlan_ds.binding_datal = 0xFFFF;

    /*write asic*/
    local_chip_num = sys_humber_get_local_chip_num();

    sys_alloc_get_table_entry_num(DS_USER_ID_VLAN_KEY, &max_entry_num);
    key_offset = max_entry_num - 2;

    usrid_entry.data_entry = (uint32 *)&usrid_vlankey_data;
    usrid_entry.mask_entry = (uint32 *)&usrid_vlankey_mask;

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        /*ds*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_ds, &usrid_vlan_ds));

        /*key & mask*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_key, &usrid_entry));
    }

    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_add_mac_default_entry_global(void)
{
    uint32 cmd_key, cmd_ds;
    uint8 i, local_chip_num;
    tbl_entry_t usrid_entry;
    uint32 max_entry_num;
    uint32 key_offset;
    ds_user_id_mac_key_t usrid_mackey_data, usrid_mackey_mask;
    ds_user_id_mac_t usrid_mac_ds;

    kal_memset(&usrid_mackey_data, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&usrid_mackey_mask, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&usrid_mac_ds, 0, sizeof(ds_user_id_mac_t));

    /*build key & mask*/
    usrid_mackey_data.table_id0 = USERID_MAC_TABLEID0_C;
    usrid_mackey_mask.table_id0 = 0xF;

    usrid_mackey_data.table_id1 = USERID_MAC_TABLEID1_C;
    usrid_mackey_mask.table_id1 = 0xF;

    /*build usrid ds: do-nothing*/
    usrid_mac_ds.user_vlan_ptr = 0x1FFF;
    usrid_mac_ds.binding_datah = 0x1FFF;

    usrid_mac_ds.binding_datam = 0xBFFF;

    usrid_mac_ds.ds_fwd_ptr_valid = 0;
    usrid_mac_ds.binding_datal = 0xFFFF;


    /*write asic*/
    local_chip_num = sys_humber_get_local_chip_num();

    sys_alloc_get_table_entry_num(DS_USER_ID_MAC_KEY, &max_entry_num);
    key_offset = max_entry_num - 1;

    usrid_entry.data_entry = (uint32 *)&usrid_mackey_data;
    usrid_entry.mask_entry = (uint32 *)&usrid_mackey_mask;

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        /*ds*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_ds, &usrid_mac_ds));

        /*key & mask*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_key, &usrid_entry));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_add_ipv4_default_entry_global(void)
{
    uint32 cmd_key, cmd_ds;
    uint8 i, local_chip_num;
    tbl_entry_t usrid_entry;
    uint32 max_entry_num;
    uint32 key_offset;
    ds_user_id_ipv4_key_t usrid_ipv4key_data, usrid_ipv4key_mask;
    ds_user_id_ipv4_t usrid_ipv4_ds;

    kal_memset(&usrid_ipv4key_data, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&usrid_ipv4key_mask, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&usrid_ipv4_ds, 0, sizeof(ds_user_id_ipv4_t));

    /*build key & mask*/
    usrid_ipv4key_data.table_id0 = USERID_IPV4_TABLEID0_C;
    usrid_ipv4key_mask.table_id0 = 0xF;

    usrid_ipv4key_data.table_id1 = USERID_IPV4_TABLEID1_C;
    usrid_ipv4key_mask.table_id1 = 0xF;

    usrid_ipv4key_data.table_id2 = USERID_IPV4_TABLEID2_C;
    usrid_ipv4key_mask.table_id2 = 0xF;

    usrid_ipv4key_data.table_id3 = USERID_IPV4_TABLEID3_C;
    usrid_ipv4key_mask.table_id3 = 0xF;

    /*build usrid ds: do-nothing*/
    usrid_ipv4_ds.user_vlan_ptr = 0x1FFF;
    usrid_ipv4_ds.binding_datah = 0x1FFF;

    usrid_ipv4_ds.binding_datam = 0xBFFF;

    usrid_ipv4_ds.ds_fwd_ptr_valid = 0;
    usrid_ipv4_ds.binding_datal = 0xFFFF;


    /*write asic*/
    local_chip_num = sys_humber_get_local_chip_num();

    sys_alloc_get_table_entry_num(DS_USER_ID_IPV4_KEY, &max_entry_num);
    key_offset = max_entry_num - 1;

    usrid_entry.data_entry = (uint32 *)&usrid_ipv4key_data;
    usrid_entry.mask_entry = (uint32 *)&usrid_ipv4key_mask;

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        /*ds*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_ds, &usrid_ipv4_ds));

        /*key & mask*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_key, &usrid_entry));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_add_ipv6_default_entry_global(void)
{
    uint32 cmd_key, cmd_ds;
    uint8 i, local_chip_num;
    tbl_entry_t usrid_entry;
    uint32 max_entry_num;
    uint32 key_offset;
    ds_user_id_ipv6_key_t usrid_ipv6key_data, usrid_ipv6key_mask;
    ds_user_id_ipv6_t usrid_ipv6_ds;

    kal_memset(&usrid_ipv6key_data, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&usrid_ipv6key_mask, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&usrid_ipv6_ds, 0, sizeof(ds_user_id_ipv6_t));

    /*build key & mask*/
    usrid_ipv6key_data.table_id0 = USERID_IPV6_TABLEID0_C;
    usrid_ipv6key_mask.table_id0 = 0xF;
    usrid_ipv6key_data.table_id1 = USERID_IPV6_TABLEID1_C;
    usrid_ipv6key_mask.table_id1 = 0xF;
    usrid_ipv6key_data.table_id2 = USERID_IPV6_TABLEID2_C;
    usrid_ipv6key_mask.table_id2 = 0xF;
    usrid_ipv6key_data.table_id3 = USERID_IPV6_TABLEID3_C;
    usrid_ipv6key_mask.table_id3 = 0xF;
    usrid_ipv6key_data.table_id4 = USERID_IPV6_TABLEID4_C;
    usrid_ipv6key_mask.table_id4 = 0xF;
    usrid_ipv6key_data.table_id5 = USERID_IPV6_TABLEID5_C;
    usrid_ipv6key_mask.table_id5 = 0xF;
    usrid_ipv6key_data.table_id6 = USERID_IPV6_TABLEID6_C;
    usrid_ipv6key_mask.table_id6 = 0xF;
    usrid_ipv6key_data.table_id7 = USERID_IPV6_TABLEID7_C;
    usrid_ipv6key_mask.table_id7 = 0xF;


    /*build usrid ds: do-thing*/
    usrid_ipv6_ds.user_vlan_ptr = 0x1FFF;
    usrid_ipv6_ds.binding_datah = 0x1FFF;

    usrid_ipv6_ds.binding_datam = 0xBFFF;

    usrid_ipv6_ds.ds_fwd_ptr_valid = 0;
    usrid_ipv6_ds.binding_datal = 0xFFFF;

    /*write asic*/
    local_chip_num = sys_humber_get_local_chip_num();

    sys_alloc_get_table_entry_num(DS_USER_ID_IPV6_KEY, &max_entry_num);
    key_offset = max_entry_num - 1;

    usrid_entry.data_entry = (uint32 *)&usrid_ipv6key_data;
    usrid_entry.mask_entry = (uint32 *)&usrid_ipv6key_mask;

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6, DRV_ENTRY_FLAG);

    for (i = 0; i < local_chip_num; i++)
    {
        /*ds*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_ds, &usrid_ipv6_ds));

        /*key & mask*/
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, key_offset, cmd_key, &usrid_entry));
    }
    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_vlan_init(void)
{

    kal_mutex_create(&(p_mutex->usrid_vlan_mutex));

    if (NULL == p_mutex->usrid_vlan_mutex)
    {
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    /* add global default entry to enable service aclqos */
    CTC_ERROR_RETURN(_sys_humber_usrid_add_vlan_default_entry_global_for_srv_en());
    /*add global default entry*/
    CTC_ERROR_RETURN(_sys_humber_usrid_add_vlan_default_entry_global());

    /*init usrid vlan db*/
    CTC_ERROR_RETURN(sys_humber_usrid_db_vlan_init());

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_mac_init(void)
{

    kal_mutex_create(&(p_mutex->usrid_mac_mutex));

    if (NULL == p_mutex->usrid_mac_mutex)
    {
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    /*add global default entry*/
    CTC_ERROR_RETURN(_sys_humber_usrid_add_mac_default_entry_global());

    /*init usrid mac db*/
    CTC_ERROR_RETURN(sys_humber_usrid_db_mac_init());

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_ipv4_init(void)
{
    kal_mutex_create(&(p_mutex->usrid_ipv4_mutex));

    if (NULL == p_mutex->usrid_ipv4_mutex)
    {
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    /*add default entry*/
    CTC_ERROR_RETURN(_sys_humber_usrid_add_ipv4_default_entry_global());

    /*init usrid ipv4 db*/
    CTC_ERROR_RETURN(sys_humber_usrid_db_ipv4_init());

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_ipv6_init(void)
{
    kal_mutex_create(&(p_mutex->usrid_ipv6_mutex));

    if (NULL == p_mutex->usrid_ipv6_mutex)
    {
        return CTC_E_FAIL_CREATE_MUTEX;
    }

    /*add default entry*/
    CTC_ERROR_RETURN(_sys_humber_usrid_add_ipv6_default_entry_global());
    /*init usrid ipv6 db*/
    CTC_ERROR_RETURN(sys_humber_usrid_db_ipv6_init());

    return CTC_E_NONE;
}

int32
sys_humber_usrid_init(void)
{
    uint32 max_entry_num = 0;

    /*init usrid db*/
    if (NULL != p_mutex)
    {
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(sys_humber_usrid_db_init());

    p_mutex = (sys_usrid_mutex_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_mutex_t));

    if (NULL == p_mutex)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_mutex, 0, sizeof(sys_usrid_mutex_t));

    /*init usrid vlan, if support*/
    sys_alloc_get_table_entry_num(DS_USER_ID_VLAN_KEY, &max_entry_num);

    if (0 != max_entry_num)
    {
        SYS_USRID_DBG_INFO("usrid vlan is supported, max index is = %d\n", max_entry_num);
        CTC_ERROR_RETURN(_sys_humber_usrid_vlan_init());
    }

    /*init usrid mac, if support*/
    sys_alloc_get_table_entry_num(DS_USER_ID_MAC_KEY, &max_entry_num);

    if (0 != max_entry_num)
    {
        SYS_USRID_DBG_INFO("usrid mac is supported, max index is = %d\n", max_entry_num);
        CTC_ERROR_RETURN(_sys_humber_usrid_mac_init());
    }

    /*init usrid ipv4, if support*/
    sys_alloc_get_table_entry_num(DS_USER_ID_IPV4_KEY, &max_entry_num);

    if (0 != max_entry_num)
    {
        SYS_USRID_DBG_INFO("usrid ipv4 is supported, max index is = %d\n", max_entry_num);
        CTC_ERROR_RETURN(_sys_humber_usrid_ipv4_init());
    }

    /*init usrid ipv6, if support*/
    sys_alloc_get_table_entry_num(DS_USER_ID_IPV6_KEY, &max_entry_num);

    if (0 != max_entry_num)
    {
        SYS_USRID_DBG_INFO("usrid ipv6 is supported, max index is = %d\n", max_entry_num);
        CTC_ERROR_RETURN(_sys_humber_usrid_ipv6_init());
    }

   return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_drv_vlankey_entry(tbl_entry_t *p_usrid_vlankey, sys_usrid_vlan_entry_t *p_entry)
{
    ds_user_id_vlan_key_t *p_data, *p_mask;
    p_data = (ds_user_id_vlan_key_t *)p_usrid_vlankey->data_entry;
    p_mask = (ds_user_id_vlan_key_t *)p_usrid_vlankey->mask_entry;

    CTC_PTR_VALID_CHECK(p_usrid_vlankey);
    CTC_PTR_VALID_CHECK(p_entry);

    p_data->table_id = USERID_VLAN_TABLEID_C;
    p_mask->table_id = 0xF;

    if (p_entry->valid.src_port_valid)
    {
        p_data->global_src_port = p_entry->usrid_key_entry.global_port;
        p_mask->global_src_port = 0x1FFF;
    }

    if (p_entry->valid.igs_cvid_valid)
    {
        p_data->cvlan_idh = (p_entry->usrid_key_entry.ingress_cvid >> 6) & 0x3F;
        p_data->cvlan_idl = (p_entry->usrid_key_entry.ingress_cvid & 0x3F);
        p_mask->cvlan_idh = (p_entry->usrid_key_entry.igs_cvid_mask >> 6) & 0x3F;
        p_mask->cvlan_idl = (p_entry->usrid_key_entry.igs_cvid_mask & 0x3F);
    }

    if (p_entry->valid.igs_svid_valid)
    {
        p_data->svlan_id = p_entry->usrid_key_entry.ingress_svid;
        p_mask->svlan_id = p_entry->usrid_key_entry.igs_svid_mask;
    }

    if (p_entry->valid.sgmac_valid)
    {
        p_data->from_sgmac = p_entry->usrid_key_entry.from_sgmac;
        p_mask->from_sgmac = 0x1;
    }

    if (p_entry->valid.vc_valid)
    {
        p_data->customer_id = p_entry->usrid_key_entry.customer_id;
        p_mask->customer_id = 0xFFFFF000;
    }

    if (p_entry->valid.gre_valid)
    {
        p_data->customer_id = p_entry->usrid_key_entry.customer_id;
        p_mask->customer_id = 0xFFFFFFFF;
    }

    if (p_entry->valid.isid_valid)
    {
        p_data->customer_id = p_entry->usrid_key_entry.customer_id;
        p_mask->customer_id = 0xFFFFFF00;
    }

    if (p_entry->valid.ctag_cos_valid)
    {
        p_data->customer_id &= 0xFFFFFF1F;
        p_data->customer_id |= (p_entry->usrid_key_entry.ctag_cos << 5);
        p_mask->customer_id |= (0x7 << 5);
    }

    if (p_entry->valid.stag_cos_valid)
    {
        p_data->customer_id &= 0xFFFFFFF1;
        p_data->customer_id |= (p_entry->usrid_key_entry.stag_cos << 1);
        p_mask->customer_id |= (0x7 << 1);
    }

    if (p_entry->valid.ctag_cfi_valid)
    {
        p_data->customer_id &= 0xFFFFFFEF;
        p_data->customer_id |= (p_entry->usrid_key_entry.ctag_cfi << 4);
        p_mask->customer_id |= (0x1 << 4);
    }

    if (p_entry->valid.stag_cfi_valid)
    {
        p_data->customer_id &= 0xFFFFFFFE;
        p_data->customer_id |= p_entry->usrid_key_entry.stag_cfi;
        p_mask->customer_id |= 0x1;
    }

    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_build_drv_mackey_entry(tbl_entry_t * p_usrid_mackey, sys_usrid_mac_entry_t * p_entry)
{
    uint32 cmd = 0;
    uint32 val = 0;
    ds_user_id_mac_key_t *p_data, *p_mask;

    CTC_PTR_VALID_CHECK(p_usrid_mackey);
    CTC_PTR_VALID_CHECK(p_entry);

    p_data = (ds_user_id_mac_key_t *)p_usrid_mackey->data_entry;
    p_mask = (ds_user_id_mac_key_t *)p_usrid_mackey->mask_entry;

    p_data->table_id0 = USERID_MAC_TABLEID0_C;
    p_mask->table_id0 = 0xF;

    p_data->table_id1 = USERID_MAC_TABLEID1_C;
    p_mask->table_id1 = 0xF;

    p_data->user_id_label = p_entry->usrid_key_entry.usrid_label;
    p_mask->user_id_label = 0x3F;

    p_data->mac_sa_upper = p_entry->usrid_key_entry.macsa_h;
    p_data->mac_sa_lower = p_entry->usrid_key_entry.macsa_l;
    p_mask->mac_sa_upper = p_entry->usrid_key_entry.macsa_mask_h;
    p_mask->mac_sa_lower = p_entry->usrid_key_entry.macsa_mask_l;

    p_data->mac_da_upper = p_entry->usrid_key_entry.macda_h;
    p_data->mac_da_lower = p_entry->usrid_key_entry.macda_l;
    p_mask->mac_da_upper = p_entry->usrid_key_entry.macda_mask_h;
    p_mask->mac_da_lower = p_entry->usrid_key_entry.macda_mask_l;

    /*for call register to decide which vlan is first*/
    cmd = DRV_IOR(IOC_REG, IPE_USER_ID_CTL, IPE_USER_ID_CTL_USER_ID_MAC_KEY_SVLAN_FIRST);
    CTC_ERROR_RETURN(drv_reg_ioctl(p_entry->usrid_key_entry.chip_id, 0, cmd, &val));

    if (p_entry->valid.igs_cvid_valid)
    {
        if (0 == val)
        {
            p_data->vlan_id1 = p_entry->usrid_key_entry.igs_cvid;
            p_mask->vlan_id1 = 0xFFF;
        }
        else
        {
            p_data->vlan_id211to8  = (p_entry->usrid_key_entry.igs_cvid >> 8) & 0xF;
            p_data->vlan_id27to0   = (p_entry->usrid_key_entry.igs_cvid & 0xFF);
            p_mask->vlan_id211to8  =  0xF;
            p_mask->vlan_id27to0   =  0xFF;
        }
    }

    if (p_entry->valid.igs_svid_valid)
    {
        if (0 == val)
        {
            p_data->vlan_id211to8  = (p_entry->usrid_key_entry.igs_svid >> 8) & 0xF;
            p_data->vlan_id27to0   = (p_entry->usrid_key_entry.igs_svid & 0xFF);
            p_mask->vlan_id211to8  =  0xF;
            p_mask->vlan_id27to0   =  0xFF;
        }
        else
        {
            p_data->vlan_id1 = p_entry->usrid_key_entry.igs_svid;
            p_mask->vlan_id1 = 0xFFF;
        }
    }

    if (p_entry->valid.stag_cos_valid)
    {
        if (0 == val)
        {
            p_data->cos2 = p_entry->usrid_key_entry.stag_cos;
            p_mask->cos2 = 0x7;
        }
        else
        {
            p_data->cos1 = p_entry->usrid_key_entry.stag_cos;
            p_mask->cos1 = 0x7;
        }
    }

    if (p_entry->valid.stag_cfi_valid)
    {
        if (0 == val)
        {
            p_data->cfi2 = p_entry->usrid_key_entry.stag_cfi;
            p_mask->cfi2 = 0x1;
        }
        else
        {
            p_data->cfi1 = p_entry->usrid_key_entry.stag_cfi;
            p_mask->cfi1 = 0x1;
        }
    }

    if (p_entry->valid.ctag_cos_valid)
    {
        if (0 == val)
        {
            p_data->cos1 = p_entry->usrid_key_entry.ctag_cos;
            p_mask->cos1 = 0x7;
        }
        else
        {
            p_data->cos2 = p_entry->usrid_key_entry.ctag_cos;
            p_mask->cos2 = 0x7;
        }
    }

    if (p_entry->valid.ctag_cfi_valid)
    {
        if (0 == val)
        {
            p_data->cfi1 = p_entry->usrid_key_entry.ctag_cfi;
            p_mask->cfi1 = 0x1;
        }
        else
        {
            p_data->cfi2 = p_entry->usrid_key_entry.ctag_cfi;
            p_mask->cfi2 = 0x1;
        }
    }

    if (p_entry->valid.l2_type_valid)
    {
        p_data->layer2_type = p_entry->usrid_key_entry.l2_type;
        p_mask->layer2_type = 0xF;
    }

    if (p_entry->valid.l3_type_valid)
    {
        p_data->layer3_type = p_entry->usrid_key_entry.l3_type;
        p_mask->layer3_type = 0xF;
    }

    if (p_entry->valid.sgmac_valid)
    {
        p_data->from_sgmac = p_entry->usrid_key_entry.from_sgmac;
        p_mask->from_sgmac = 0x1;
    }

    if (p_entry->valid.exception2_valid)
    {
        p_data->exp2 = p_entry->usrid_key_entry.exception2;
        p_mask->exp2 = 0x1;
    }

    if (p_entry->valid.exp_subindex_valid)
    {
        p_data->exp_sub_idx = p_entry->usrid_key_entry.exp_subindex;
        p_mask->exp_sub_idx = 0xF;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_drv_ipv4key_entry(tbl_entry_t * p_usrid_ipv4key, sys_usrid_ipv4_entry_t * p_entry)
{
    ds_user_id_ipv4_key_t *p_data, *p_mask;

    CTC_PTR_VALID_CHECK(p_usrid_ipv4key);
    CTC_PTR_VALID_CHECK(p_entry);

    p_data = (ds_user_id_ipv4_key_t *)p_usrid_ipv4key->data_entry;
    p_mask = (ds_user_id_ipv4_key_t *)p_usrid_ipv4key->mask_entry;

    p_data->table_id0 = USERID_IPV4_TABLEID0_C;
    p_mask->table_id0 = 0xF;
    p_data->table_id1 = USERID_IPV4_TABLEID1_C;
    p_mask->table_id1 = 0xF;
    p_data->table_id2 = USERID_IPV4_TABLEID2_C;
    p_mask->table_id2 = 0xF;
    p_data->table_id3 = USERID_IPV4_TABLEID3_C;
    p_mask->table_id3 = 0xF;

    p_data->user_id_label = p_entry->usrid_key_entry.usrid_label;
    p_mask->user_id_label = 0x3F;

    p_data->mac_da_upper = p_entry->usrid_key_entry.macda_h;
    p_mask->mac_da_upper = p_entry->usrid_key_entry.macda_mask_h;
    p_data->mac_da_lower = p_entry->usrid_key_entry.macda_l;
    p_mask->mac_da_lower = p_entry->usrid_key_entry.macda_mask_l;

    p_data->mac_sa_upper = p_entry->usrid_key_entry.macsa_h;
    p_mask->mac_sa_upper = p_entry->usrid_key_entry.macsa_mask_h;
    p_data->mac_sa_lower = p_entry->usrid_key_entry.macsa_l;
    p_mask->mac_sa_lower = p_entry->usrid_key_entry.macsa_mask_l;

    if (p_entry->valid.ether_type_valid)
    {
        p_data->ether_type_upper = p_entry->usrid_key_entry.ether_type >> 8;
        p_mask->ether_type_upper = 0xFF;
        p_data->ether_type_lower = p_entry->usrid_key_entry.ether_type & 0xFF;
        p_mask->ether_type_lower = 0xFF;
    }

    if (p_entry->valid.l2_type_valid)
    {
        p_data->layer2_type = p_entry->usrid_key_entry.l2_type;
        p_mask->layer2_type = 0xF;
    }

    if (p_entry->valid.igs_cvid_valid)
    {
        p_data->cvlan_id = p_entry->usrid_key_entry.cvid;
        p_mask->cvlan_id = p_entry->usrid_key_entry.igs_cvid_mask;
    }

    if (p_entry->valid.ctag_cos_valid)
    {
        p_data->ctag_cos = p_entry->usrid_key_entry.ctag_cos;
        p_mask->ctag_cos = 0x7;
    }

    if (p_entry->valid.ctag_cfi_valid)
    {
        p_data->ctag_cfi = p_entry->usrid_key_entry.ctag_cfi;
        p_mask->ctag_cfi = 0x1;
    }

    if (p_entry->valid.igs_svid_valid)
    {
        p_data->svlan_id = p_entry->usrid_key_entry.svid;
        p_mask->svlan_id = p_entry->usrid_key_entry.igs_svid_mask;;
    }

    if (p_entry->valid.stag_cos_valid)
    {
        p_data->stag_cos = p_entry->usrid_key_entry.stag_cos;
        p_mask->stag_cos = 0x7;
    }

    if (p_entry->valid.stag_cfi_valid)
    {
        p_data->stag_cfi = p_entry->usrid_key_entry.stag_cfi;
        p_mask->stag_cfi = 0x1;
    }

    p_data->ip_da = p_entry->usrid_key_entry.ipv4_da;
    p_mask->ip_da = p_entry->usrid_key_entry.ipv4_da_mask;
    p_data->ip_sa = p_entry->usrid_key_entry.ipv4_sa;
    p_mask->ip_sa = p_entry->usrid_key_entry.ipv4_sa_mask;

    if (p_entry->valid.dscp_valid)
    {
        p_data->dscp = p_entry->usrid_key_entry.dscp;
        p_mask->dscp = 0x3F;
    }

    if (p_entry->valid.frag_info_valid)
    {
        p_data->frag_info = p_entry->usrid_key_entry.frag_info;
        p_mask->frag_info = 0x3;
    }

    if (p_entry->valid.ip_hdr_error_valid)
    {
        p_data->ip_header_error = p_entry->usrid_key_entry.ip_hdr_error;
        p_mask->ip_header_error = 0x1;
    }

    if (p_entry->valid.ip_option_valid)
    {
        p_data->ip_options = p_entry->usrid_key_entry.ip_option;
        p_mask->ip_options = 0x1;
    }

    if (p_entry->valid.routed_packet_valid)
    {
        p_data->routed_packet = p_entry->usrid_key_entry.routed_packet;
        p_mask->routed_packet = 0x1;
    }

    if (p_entry->valid.l3_type_valid)
    {
        p_data->layer3_type = p_entry->usrid_key_entry.l3_type;
        p_mask->layer3_type = 0xF;
    }

    if (p_entry->valid.is_tcp_valid)
    {
        p_data->is_tcp = p_entry->usrid_key_entry.is_tcp;
        p_mask->is_tcp = 0x1;
    }

    if(p_entry->valid.is_udp_valid)
    {
        p_data->is_udp = p_entry->usrid_key_entry.is_udp;
        p_mask->is_udp = 0x1;
    }

    if (p_entry->valid.is_app_valid)
    {
        p_data->is_application = p_entry->usrid_key_entry.is_app;
        p_mask->is_application = 0x1;
    }

    if (p_entry->valid.l4_src_port_valid)
    {
        p_data->l4source_port = p_entry->usrid_key_entry.l4_src_port;
        p_mask->l4source_port = 0xFFFF;
    }

    if (p_entry->valid.l4_dest_port_valid)
    {
        p_data->l4dest_port = p_entry->usrid_key_entry.l4_dest_port;
        p_mask->l4dest_port = 0xFFFF;
    }

    /*this field's mask TBD*/
    if (p_entry->valid.l4info_mapped_valid)
    {
        p_data->l4info_mapped = p_entry->usrid_key_entry.l4_info_mapped;
        p_mask->l4info_mapped = 0xFFFF;
    }

    if (p_entry->valid.sgmac_valid)
    {
        p_data->from_sgmac = p_entry->usrid_key_entry.from_sgmac;
        p_mask->from_sgmac = 0x1;
    }

    if (p_entry->valid.exception2_valid)
    {
        p_data->exp2 = p_entry->usrid_key_entry.exception2_en;
        p_mask->exp2 = 0x1;
    }

    if (p_entry->valid.exp_subindex_valid)
    {
        p_data->exp_sub_idx = p_entry->usrid_key_entry.exp_sub_index;
        p_mask->exp_sub_idx = 0xF;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_drv_ipv6key_entry(tbl_entry_t * p_usrid_ipv6key, sys_usrid_ipv6_entry_t * p_entry)
{
    ds_user_id_ipv6_key_t *p_data, *p_mask;

    CTC_PTR_VALID_CHECK(p_usrid_ipv6key);
    CTC_PTR_VALID_CHECK(p_entry);

    p_data = (ds_user_id_ipv6_key_t *)p_usrid_ipv6key->data_entry;
    p_mask = (ds_user_id_ipv6_key_t *)p_usrid_ipv6key->mask_entry;

    p_data->table_id0 = USERID_IPV6_TABLEID0_C;
    p_mask->table_id0 = 0xF;
    p_data->table_id1 = USERID_IPV6_TABLEID1_C;
    p_mask->table_id1 = 0xF;
    p_data->table_id2 = USERID_IPV6_TABLEID2_C;
    p_mask->table_id2 = 0xF;
    p_data->table_id3 = USERID_IPV6_TABLEID3_C;
    p_mask->table_id3 = 0xF;
    p_data->table_id4 = USERID_IPV6_TABLEID4_C;
    p_mask->table_id4 = 0xF;
    p_data->table_id5 = USERID_IPV6_TABLEID5_C;
    p_mask->table_id5 = 0xF;
    p_data->table_id6 = USERID_IPV6_TABLEID6_C;
    p_mask->table_id6 = 0xF;
    p_data->table_id7 = USERID_IPV6_TABLEID7_C;
    p_mask->table_id7 = 0xF;

    p_data->user_id_label = p_entry->usrid_key_entry.usrid_label;
    p_mask->user_id_label = 0x3F;

    p_data->mac_da_upper = p_entry->usrid_key_entry.macda_h;
    p_mask->mac_da_upper = p_entry->usrid_key_entry.macda_mask_h;
    p_data->mac_da_lower = p_entry->usrid_key_entry.macda_l;
    p_mask->mac_da_lower = p_entry->usrid_key_entry.macda_mask_l;

    p_data->mac_sa_upper = p_entry->usrid_key_entry.macsa_h;
    p_mask->mac_sa_upper = p_entry->usrid_key_entry.macsa_mask_h;
    p_data->mac_sa_lower = p_entry->usrid_key_entry.macsa_l;
    p_mask->mac_sa_lower = p_entry->usrid_key_entry.macsa_mask_l;

    if (p_entry->valid.ether_type_valid)
    {
        p_data->ether_type15to8 = p_entry->usrid_key_entry.ether_type >> 8;
        p_mask->ether_type15to8 = 0xFF;
        p_data->ether_type7to0 = p_entry->usrid_key_entry.ether_type & 0xFF;
        p_mask->ether_type7to0 = 0xFF;
    }

    if (p_entry->valid.l2_type_valid)
    {
        p_data->layer2_type = p_entry->usrid_key_entry.l2_type;
        p_mask->layer2_type = 0xF;
    }

    if (p_entry->valid.igs_cvid_valid)
    {
        p_data->cvlan_id = p_entry->usrid_key_entry.cvid;
        p_mask->cvlan_id = 0xFFF;
    }

    if (p_entry->valid.ctag_cos_valid)
    {
        p_data->ctag_cos = p_entry->usrid_key_entry.ctag_cos;
        p_mask->ctag_cos = 0x7;
    }

    if (p_entry->valid.ctag_cfi_valid)
    {
        p_data->ctag_cfi = p_entry->usrid_key_entry.ctag_cfi;
        p_mask->ctag_cfi = 0x1;
    }

    if (p_entry->valid.igs_svid_valid)
    {
        p_data->svlan_id = p_entry->usrid_key_entry.svid;
        p_mask->svlan_id = 0xFFF;
    }

    if (p_entry->valid.stag_cos_valid)
    {
        p_data->stag_cos = p_entry->usrid_key_entry.stag_cos;
        p_mask->stag_cos = 0x7;
    }

    if (p_entry->valid.stag_cfi_valid)
    {
        p_data->stag_cfi = p_entry->usrid_key_entry.stag_cfi;
        p_mask->stag_cfi = 0x1;
    }

    /*ipda*/
    p_data->ip_da31_to0 = p_entry->usrid_key_entry.ipv6_da[3];
    p_mask->ip_da31_to0 = p_entry->usrid_key_entry.ipv6_dmask[3];

    p_data->ip_da63_to32 = p_entry->usrid_key_entry.ipv6_da[2];
    p_mask->ip_da63_to32 = p_entry->usrid_key_entry.ipv6_dmask[2];

    p_data->ip_da71_to64 = (p_entry->usrid_key_entry.ipv6_da[1]) & 0xFF;
    p_mask->ip_da71_to64 = (p_entry->usrid_key_entry.ipv6_dmask[1]) & 0xFF;

    p_data->ip_da103_to72 = (p_entry->usrid_key_entry.ipv6_da[1] >> 8)
                           | ((p_entry->usrid_key_entry.ipv6_da[0] & 0xFF) << 24);
    p_mask->ip_da103_to72 = (p_entry->usrid_key_entry.ipv6_dmask[1] >> 8)
                           | ((p_entry->usrid_key_entry.ipv6_dmask[0] & 0xFF) << 24);

    p_data->ip_da127_to104 = (p_entry->usrid_key_entry.ipv6_da[0] >> 8);
    p_mask->ip_da127_to104 = (p_entry->usrid_key_entry.ipv6_dmask[0] >> 8);

    /*ipsa*/
    p_data->ip_sa31_to0 = p_entry->usrid_key_entry.ipv6_sa[3];
    p_mask->ip_sa31_to0 = p_entry->usrid_key_entry.ipv6_smask[3];

    p_data->ip_sa63_to32 = p_entry->usrid_key_entry.ipv6_sa[2];
    p_mask->ip_sa63_to32 = p_entry->usrid_key_entry.ipv6_sa[2];

    p_data->ip_sa71_to64 = (p_entry->usrid_key_entry.ipv6_sa[1]) & 0xFF;
    p_mask->ip_sa71_to64 = (p_entry->usrid_key_entry.ipv6_smask[1]) & 0xFF;

    p_data->ip_sa103_to72 = (p_entry->usrid_key_entry.ipv6_sa[1] >> 8)
                           | ((p_entry->usrid_key_entry.ipv6_sa[0] & 0xFF) << 24);

    p_mask->ip_sa103_to72 = (p_entry->usrid_key_entry.ipv6_smask[1] >> 8)
                           | ((p_entry->usrid_key_entry.ipv6_smask[0] & 0xFF) << 24);

    p_data->ip_sa127_to104 = (p_entry->usrid_key_entry.ipv6_sa[0] >> 8);
    p_mask->ip_sa127_to104 = (p_entry->usrid_key_entry.ipv6_smask[0] >> 8);


    if (p_entry->valid.dscp_valid)
    {
        p_data->dscp = p_entry->usrid_key_entry.dscp;
        p_mask->dscp = 0x3F;
    }

    if (p_entry->valid.frag_info_valid)
    {
        p_data->frag_info = p_entry->usrid_key_entry.frag_info;
        p_mask->frag_info = 0x3;
    }

    if (p_entry->valid.ip_hdr_error_valid)
    {
        p_data->ip_header_error = p_entry->usrid_key_entry.ip_hdr_err;
        p_mask->ip_header_error = 0x1;
    }

    if (p_entry->valid.ipv6_ext_hdr_valid)
    {
        p_data->ipv6_externsion_headers = p_entry->usrid_key_entry.ipv6_ext_hdr;
        p_mask->ipv6_externsion_headers = 0xFF;
    }

    if (p_entry->valid.ipv6_flow_label_valid)
    {
        p_data->ipv6_flow_label = p_entry->usrid_key_entry.ipv6_flow_label;
        p_mask->ipv6_flow_label = 0xFFFFF;
    }

    if (p_entry->valid.ip_option_valid)
    {
        p_data->ip_options = p_entry->usrid_key_entry.ip_option;
        p_mask->ip_options = 0x1;
    }

    if (p_entry->valid.routed_packet_valid)
    {
        p_data->routed_packet = p_entry->usrid_key_entry.routed_packet;
        p_mask->routed_packet = 0x1;
    }

    if (p_entry->valid.l3_type_valid)
    {
        p_data->layer3_type = p_entry->usrid_key_entry.l3_type;
        p_mask->layer3_type = 0xF;
    }

    if (p_entry->valid.is_tcp_valid)
    {
        p_data->is_tcp = p_entry->usrid_key_entry.is_tcp;
        p_mask->is_tcp = 0x1;
    }

    if(p_entry->valid.is_udp_valid)
    {
        p_data->is_udp = p_entry->usrid_key_entry.is_udp;
        p_mask->is_udp = 0x1;
    }

    if (p_entry->valid.is_app_valid)
    {
        p_data->is_app = p_entry->usrid_key_entry.is_app;
        p_mask->is_app = 0x1;
    }

    if (p_entry->valid.l4_src_port_valid)
    {
        p_data->l4source_port = p_entry->usrid_key_entry.l4_src_port;
        p_mask->l4source_port = 0xFFFF;
    }

    if (p_entry->valid.l4_dest_port_valid)
    {
        p_data->l4dest_port = p_entry->usrid_key_entry.l4_dest_port;
        p_mask->l4dest_port = 0xFFFF;
    }

    /*this field's mask TBD*/
    if (p_entry->valid.l4info_mapped_valid)
    {
        p_data->l4info_mapped = p_entry->usrid_key_entry.l4_info_map;
        p_mask->l4info_mapped = 0xFFFF;
    }

    if (p_entry->valid.sgmac_valid)
    {
        p_data->from_sgmac = p_entry->usrid_key_entry.from_sgmac;
        p_mask->from_sgmac = 0x1;
    }

    if (p_entry->valid.exception2_valid)
    {
        p_data->exp2 = p_entry->usrid_key_entry.exception2;
        p_mask->exp2 = 0x1;
    }

    if (p_entry->valid.exp_subindex_valid)
    {
        p_data->exp_sub_idx = p_entry->usrid_key_entry.exp_subindex;
        p_mask->exp_sub_idx = 0xF;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_drv_assoicate_entry(ds_user_id_t *p_drv_entry, sys_usrid_db_ds_node_t *p_ds_node)
{
    CTC_PTR_VALID_CHECK(p_drv_entry);
    CTC_PTR_VALID_CHECK(p_ds_node);

    p_drv_entry->exception_en = p_ds_node->usrid_exception_en;
    p_drv_entry->by_pass_all = p_ds_node->by_pass_all;
    p_drv_entry->aps_select_valid = p_ds_node->aps_select_valid;
    p_drv_entry->src_queue_select = p_ds_node->src_queue_select;
    p_drv_entry->vpls_port_type = p_ds_node->vpls_port_type;
    p_drv_entry->src_communicate_port = p_ds_node->communicate_port;
    p_drv_entry->user_vlan_ptr = p_ds_node->usr_vlan_ptr;
    p_drv_entry->binding_datah = p_ds_node->binding_data_h;
    p_drv_entry->ds_fwd_ptr_valid = p_ds_node->fwd_ptr_valid;
    p_drv_entry->binding_en = p_ds_node->binding_en;
    p_drv_entry->binding_mac_sa = (p_ds_node->binding_macsa | p_ds_node->stats_ptr_valid );
    p_drv_entry->binding_datam = p_ds_node->binding_data_m;
    p_drv_entry->binding_datal = p_ds_node->binding_data_l;

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_db_vlankey_node(sys_usrid_db_vlankey_node_t *p_node, sys_usrid_vlankey_entry_t *p_entry)
{
    CTC_PTR_VALID_CHECK(p_node);
    CTC_PTR_VALID_CHECK(p_entry);

    p_node->label_id            = p_entry->label_id;
    p_node->igs_cvid            = p_entry->ingress_cvid;
    p_node->igs_svid            = p_entry->ingress_svid;
    p_node->igs_cvid_mask       = p_entry->igs_cvid_mask;
    p_node->igs_svid_mask       = p_entry->igs_svid_mask;
    p_node->ctag_cos            = p_entry->ctag_cos;
    p_node->ctag_cfi            = p_entry->ctag_cfi;
    p_node->stag_cos            = p_entry->stag_cos;
    p_node->stag_cfi            = p_entry->stag_cfi;
    p_node->global_src_port     = p_entry->global_port;
    p_node->customer_id         = p_entry->customer_id;
    p_node->from_sgmac          = p_entry->from_sgmac;
    p_node->exception2          = p_entry->exception2;
    p_node->exp_subindex        = p_entry->exp_subindex;

    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_build_db_mackey_node(sys_usrid_db_mackey_node_t *p_node, sys_usrid_mackey_entry_t *p_entry)
{
    CTC_PTR_VALID_CHECK(p_node);
    CTC_PTR_VALID_CHECK(p_entry);

    p_node->chip_id         = p_entry->chip_id;
    p_node->label_id        = p_entry->usrid_label;
    p_node->cvid            = p_entry->igs_cvid;
    p_node->svid            = p_entry->igs_svid;
    p_node->ctag_cos        = p_entry->ctag_cos;
    p_node->ctag_cfi        = p_entry->ctag_cfi;
    p_node->stag_cos        = p_entry->stag_cos;
    p_node->stag_cfi        = p_entry->stag_cfi;
    p_node->macda_h         = p_entry->macda_h;
    p_node->macda_l         = p_entry->macda_l;
    p_node->macsa_h         = p_entry->macsa_h;
    p_node->macsa_l         = p_entry->macsa_l;
    p_node->macda_mask_h    = p_entry->macda_mask_h;
    p_node->macsa_mask_h    = p_entry->macsa_mask_h;
    p_node->macda_mask_l    = p_entry->macda_mask_l;
    p_node->macsa_mask_l    = p_entry->macsa_mask_l;
    p_node->l2_type         = p_entry->l2_type;
    p_node->l3_type         = p_entry->l3_type;
    p_node->from_sgmac      = p_entry->from_sgmac;
    p_node->exception2      = p_entry->exception2;
    p_node->exp_sub_index   = p_entry->exp_subindex;

    return CTC_E_NONE;

}

static int32
_sys_humber_usrid_build_db_ipv4key_node(sys_usrid_db_ipv4key_node_t *p_node, sys_usrid_ipv4key_entry_t *p_entry)
{
    CTC_PTR_VALID_CHECK(p_node);
    CTC_PTR_VALID_CHECK(p_entry);

    p_node->chip_id = p_entry->chip_id;
    p_node->label_id = p_entry->usrid_label;

    p_node->macda_h = p_entry->macda_h;
    p_node->macda_l = p_entry->macda_l;
    p_node->macsa_h = p_entry->macsa_h;
    p_node->macsa_l = p_entry->macsa_l;

	p_node->macda_mask_h = p_entry->macda_mask_h;
    p_node->macda_mask_l = p_entry->macda_mask_l;
    p_node->macsa_mask_h = p_entry->macsa_mask_h;
    p_node->macsa_mask_l = p_entry->macsa_mask_l;
    p_node->l2_type = p_entry->l2_type;
    p_node->ether_type = p_entry->ether_type;

    p_node->cvid = p_entry->cvid;
    p_node->ctag_cos = p_entry->ctag_cos;
    p_node->ctag_cfi = p_entry->ctag_cfi;
    p_node->svid = p_entry->svid;
    p_node->stag_cos = p_entry->stag_cos;
    p_node->stag_cfi = p_entry->stag_cfi;

	p_node->igs_cvid_mask  = p_entry->igs_cvid_mask;
    p_node->igs_svid_mask  = p_entry->igs_svid_mask;

    p_node->l3_type = p_entry->l3_type;

    p_node->ipv4_da = p_entry->ipv4_da;
    p_node->ipv4_da_mask = p_entry->ipv4_da_mask;
    p_node->ipv4_sa = p_entry->ipv4_sa;
    p_node->ipv4_sa_mask = p_entry->ipv4_sa_mask;

    p_node->dscp = p_entry->dscp;
    p_node->frag_info = p_entry->frag_info;
    p_node->ip_hdr_error = p_entry->ip_hdr_error;
    p_node->ip_option = p_entry->ip_option;
    p_node->routed_packet = p_entry->routed_packet;

    p_node->is_tcp = p_entry->is_tcp;
    p_node->is_udp = p_entry->is_udp;
    p_node->is_app = p_entry->is_app;
    p_node->l4_src_port = p_entry->l4_src_port;
    p_node->l4_dest_port = p_entry->l4_dest_port;
    p_node->l4_info_mapped = p_entry->l4_info_mapped;

    p_node->exception2_en = p_node->exception2_en;
    p_node->exp_sub_index = p_node->exp_sub_index;
    p_node->from_sgmac = p_node->from_sgmac;

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_db_ipv6key_node(sys_usrid_db_ipv6key_node_t *p_node, sys_usrid_ipv6key_entry_t *p_entry)
{
    CTC_PTR_VALID_CHECK(p_node);
    CTC_PTR_VALID_CHECK(p_entry);

    p_node->chip_id = p_entry->chip_id;
    p_node->label_id = p_entry->usrid_label;

    p_node->macda_h = p_entry->macda_h;
    p_node->macda_l = p_entry->macda_l;
    p_node->macsa_h = p_entry->macsa_h;
    p_node->macsa_l = p_entry->macsa_l;
    p_node->macda_mask_h = p_entry->macda_mask_h;
    p_node->macda_mask_l = p_entry->macda_mask_l;
    p_node->macsa_mask_h = p_entry->macsa_mask_h;
    p_node->macsa_mask_l = p_entry->macsa_mask_l;

    p_node->l2_type = p_entry->l2_type;
    p_node->ether_type = p_entry->ether_type;

    p_node->cvid = p_entry->cvid;
    p_node->ctag_cos = p_entry->ctag_cos;
    p_node->ctag_cfi = p_entry->ctag_cfi;
    p_node->svid = p_entry->svid;
    p_node->stag_cos = p_entry->stag_cos;
    p_node->stag_cfi = p_entry->stag_cfi;

    p_node->l3_type = p_entry->l3_type;

    kal_memcpy(p_node->ipv6_da, p_entry->ipv6_da, sizeof(uint32) * 4);
    kal_memcpy(p_node->ipv6_dmask, p_entry->ipv6_dmask, sizeof(uint32) * 4);
    kal_memcpy(p_node->ipv6_sa, p_entry->ipv6_sa, sizeof(uint32) * 4);
    kal_memcpy(p_node->ipv6_smask, p_entry->ipv6_smask, sizeof(uint32) * 4);

    p_node->ipv6_flow_label = p_entry->ipv6_flow_label;
    p_node->ipv6_ext_hdr = p_entry->ipv6_ext_hdr;
    p_node->dscp = p_entry->dscp;
    p_node->frag_info = p_entry->frag_info;
    p_node->ip_hdr_err = p_entry->ip_hdr_err;
    p_node->ip_option = p_entry->ip_option;
    p_node->routed_packet = p_entry->routed_packet;

    p_node->is_tcp = p_entry->is_tcp;
    p_node->is_udp = p_entry->is_udp;
    p_node->is_app = p_entry->is_app;
    p_node->l4_src_port = p_entry->l4_src_port;
    p_node->l4_dest_port = p_entry->l4_dest_port;
    p_node->l4_info_map = p_entry->l4_info_map;

    p_node->exception2= p_node->exception2;
    p_node->exp_subindex= p_node->exp_subindex;
    p_node->from_sgmac = p_entry->from_sgmac;

    return CTC_E_NONE;
}

static int32
_sys_humber_usrid_build_db_ds_node(sys_usrid_db_ds_node_t * p_ds_node, sys_usrid_ds_entry_t * p_ds_entry)
{
    p_ds_node->usrid_exception_en = p_ds_entry->usrid_exception_en;
    p_ds_node->by_pass_all = p_ds_entry->by_pass_all;
    p_ds_node->aps_select_valid = p_ds_entry->aps_select_valid;
    p_ds_node->src_queue_select  = p_ds_entry->src_queue_select;
    p_ds_node->vpls_port_type = p_ds_entry->vpls_port_type;
    p_ds_node->communicate_port = p_ds_entry->communicate_port;
    p_ds_node->usr_vlan_ptr = p_ds_entry->usr_vlan_ptr;
    p_ds_node->fwd_ptr_valid = p_ds_entry->fwd_ptr_valid;
    p_ds_node->binding_en = p_ds_entry->binding_en;
    p_ds_node->binding_macsa = p_ds_entry->binding_macsa;
    p_ds_node->stats_ptr_valid = p_ds_entry->stats_ptr_valid;

    if (p_ds_entry->binding_en)
    {
        p_ds_node->binding_data_h = p_ds_entry->binding_data_h.binding_data;
        p_ds_node->binding_data_m = p_ds_entry->binding_data_m.binding_data;
        p_ds_node->binding_data_l = p_ds_entry->binding_data_l.binding_data;
    }
    else
    {
        /*binding data [15:0]*/
        if (p_ds_entry->fwd_ptr_valid)
        {
            p_ds_node->binding_data_l = p_ds_entry->binding_data_l.fwd_ptr;
        }
        else if (p_ds_entry->stats_ptr_valid)
        {
            p_ds_node->binding_data_l = p_ds_entry->binding_data_l.stats_ptr;
        }
        else if (p_ds_entry->vrfid_valid)
        {
            p_ds_node->binding_data_l = p_ds_entry->binding_data_l.vrf_id;
        }
        else
        {
            /*invalid vrfid value, denote this bindingdata has no meaning*/
            p_ds_node->binding_data_l = 0xFFFF;
        }

        /*binding data [31:16]*/
        if (p_ds_entry->binding_data_m.svr_id_en)
        {
            p_ds_node->binding_data_m =
            (p_ds_entry->binding_data_m.svr_id_en << 15) | (p_ds_entry->binding_data_m.svr_policer_valid << 14)
            | (p_ds_entry->binding_data_m.svr_id);
        }
        else if (p_ds_entry->binding_data_m.usr_cvlan_valid)
        {
            p_ds_node->binding_data_m =
            (p_ds_entry->binding_data_m.svr_id_en << 15) | (p_ds_entry->binding_data_m.usr_cvlan_valid << 14)
            | (p_ds_entry->binding_data_m.usr_cvlan_id);
        }
        else if (p_ds_entry->binding_data_m.flow_policer_valid)
        {
            p_ds_node->binding_data_m =
            (p_ds_entry->binding_data_m.svr_id_en << 15) | (p_ds_entry->binding_data_m.flow_policer_ptr);
        }
        else
        {
            /*denote this bindingdata has no meaning*/
            p_ds_node->binding_data_m = 0xBFFF;
        }

        /*binding data [47:32]*/
        if (p_ds_entry->binding_data_h.vpls_src_port_valid)
        {
            p_ds_node->binding_data_h =
            (p_ds_entry->binding_data_h.srv_aclqos_en << 15)
            | (p_ds_entry->binding_data_h.vpls_src_port_valid << 14)
            | (p_ds_entry->binding_data_h.vpls_src_port);
        }
        else
        {
            if (p_ds_entry->binding_data_h.usr_svlan_valid)
            {
                p_ds_node->binding_data_h =
                (p_ds_entry->binding_data_h.srv_aclqos_en << 15)
                | (p_ds_entry->binding_data_h.vpls_src_port_valid << 14)
                | (p_ds_entry->binding_data_h.aps_select_protecting_path << 13)
                | (p_ds_entry->binding_data_h.aps_select_group_valid << 12)
                | (p_ds_entry->binding_data_h.usr_svlan_id);
            }
            else if (p_ds_entry->binding_data_h.aps_select_group_valid)
            {
                p_ds_node->binding_data_h =
                (p_ds_entry->binding_data_h.srv_aclqos_en << 15)
                | (p_ds_entry->binding_data_h.vpls_src_port_valid << 14)
                | (p_ds_entry->binding_data_h.aps_select_protecting_path << 13)
                | (p_ds_entry->binding_data_h.aps_select_group_valid << 12)
                | (p_ds_entry->binding_data_h.aps_select_group_id);
            }
            else
            {
                p_ds_node->binding_data_h =
                (p_ds_entry->binding_data_h.srv_aclqos_en << 15)
                | (p_ds_entry->binding_data_h.vpls_src_port_valid << 14)
                | (p_ds_entry->binding_data_h.aps_select_protecting_path << 13)
                | 0x1FFF;
            }
        }
    }

    return CTC_E_NONE;

}

int32
sys_humber_usrid_add_vlan_entry(sys_usrid_vlan_entry_t *p_entry)
{
    uint32 cmd_key, cmd_ds;
    int32 ret = CTC_E_NONE;
    uint8 lchip;
    uint16 gport;
    sys_usrid_db_vlan_node_t *p_vlan_node = NULL;
	 sys_usrid_db_vlan_node_t vlan_node;
    ds_user_id_vlan_key_t usrid_vlankey_data, usrid_vlankey_mask;
    ds_user_id_t ds_usrid_vlan;
    tbl_entry_t ds_usrid_vlankey;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }

/*debug info */
    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("---------usrid_vlan_key-----------\n");
    SYS_USRID_DBG_INFO(" global port             :%d\n", p_entry->usrid_key_entry.global_port);
    SYS_USRID_DBG_INFO(" cvlan_id                :%d\n", p_entry->usrid_key_entry.ingress_cvid);
    SYS_USRID_DBG_INFO(" cvlan_mask              :%d\n", p_entry->usrid_key_entry.igs_cvid_mask);
    SYS_USRID_DBG_INFO(" svlan_id                :%d\n", p_entry->usrid_key_entry.ingress_svid);
    SYS_USRID_DBG_INFO(" svlan_mask              :%d\n", p_entry->usrid_key_entry.igs_svid_mask);
    SYS_USRID_DBG_INFO("-------usrid_associate_data-------\n");
    SYS_USRID_DBG_INFO(" bypassall               :%d\n vlan_ptr                :0x%x\n",
    p_entry->ds_entry_usrid.by_pass_all,
    p_entry->ds_entry_usrid.usr_vlan_ptr);

    if (p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO(" binding macsa            :0x%x%x%x\n", p_entry->ds_entry_usrid.binding_data_h.binding_data,
       p_entry->ds_entry_usrid.binding_data_l.binding_data, p_entry->ds_entry_usrid.binding_data_m.binding_data);
    }

    if (p_entry->ds_entry_usrid.binding_en && !p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO(" binding global port      :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.binding_data);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.vpls_src_port_valid)
    {
        SYS_USRID_DBG_INFO(" vpls src port           :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.vpls_src_port);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.usr_svlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr svlan id            :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.usr_svlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.aps_select_group_valid)
    {
        SYS_USRID_DBG_INFO(" aps select group id     :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.aps_select_group_id);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.svr_id_en)
    {
        SYS_USRID_DBG_INFO(" service id              :0x%x\n", p_entry->ds_entry_usrid.binding_data_m.svr_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr cvlan id            :%d\n", p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.flow_policer_valid)
    {
        SYS_USRID_DBG_INFO(" flow policer ptr        :%d\n", p_entry->ds_entry_usrid.binding_data_m.flow_policer_ptr);
    }

    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" ds_fwd_ptr              :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.fwd_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.stats_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" stats_ptr               :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.stats_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.vrfid_valid)
    {
        SYS_USRID_DBG_INFO(" vrfid                   :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.vrf_id);
    }
/*End debug info*/
    lchip = p_entry->usrid_key_entry.lchip;

    if (p_entry->valid.src_port_valid)
    {
        gport = p_entry->usrid_key_entry.global_port;
    }
    else
    {
        gport = CTC_MAX_UINT16_VALUE;
    }

    kal_memset(&vlan_node, 0, sizeof(sys_usrid_db_vlan_node_t));

    /*build usrid vlan node*/
    kal_memcpy(&(vlan_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_vlankey_node(&(vlan_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ds_node(&(vlan_node.ds_node_usrid), &(p_entry->ds_entry_usrid)));

    /*build usrid key & mask & ds*/
    kal_memset(&usrid_vlankey_data, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_vlankey_mask, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&ds_usrid_vlan, 0, sizeof(ds_user_id_t));
    kal_memset(&ds_usrid_vlankey, 0, sizeof(tbl_entry_t));

    ds_usrid_vlankey.data_entry = (uint32 *)&usrid_vlankey_data;
    ds_usrid_vlankey.mask_entry = (uint32 *)&usrid_vlankey_mask;

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_vlankey_entry(&ds_usrid_vlankey, p_entry));

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN, DRV_ENTRY_FLAG);

    USRID_VLAN_LOCK;

	/*write DB*/
    p_vlan_node = sys_humber_usrid_db_find_node(lchip, gport, SYS_USRID_VLAN_KEY, &vlan_node);
    if (NULL != p_vlan_node)
	{
	    uint32   key_offset=0;
	    key_offset =   p_vlan_node->key_offset;
		kal_memcpy(p_vlan_node,&vlan_node, sizeof(sys_usrid_db_vlan_node_t));
		p_vlan_node->key_offset = key_offset;

	}
	else
	{
 	  p_vlan_node =
       (sys_usrid_db_vlan_node_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_db_vlan_node_t));

       if (NULL == p_vlan_node)
       {
          ret = CTC_E_NO_MEMORY;
          goto error;
       }
 	   kal_memcpy(p_vlan_node,&vlan_node, sizeof(sys_usrid_db_vlan_node_t));
       if ((ret = sys_humber_usrid_db_insert_node(lchip, gport, SYS_USRID_VLAN_KEY, (void *)p_vlan_node)) < 0)
       {
            mem_free(p_vlan_node);
            goto error;
       }
	}

	CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_assoicate_entry(&ds_usrid_vlan,  &(p_vlan_node->ds_node_usrid)));


    /* do write asic*/
    if ((ret = drv_tbl_ioctl(lchip, p_vlan_node->key_offset, cmd_ds, &ds_usrid_vlan)) < 0)
    {
        goto error;
    }
    if ((ret = drv_tbl_ioctl(lchip, p_vlan_node->key_offset, cmd_key, &ds_usrid_vlankey)) < 0)
    {
        goto error;
    }
    SYS_USRID_DBG_INFO("Index of added usrid vlan entry == %d\n", p_vlan_node->key_offset);

    error:
        USRID_VLAN_UNLOCK;
        return ret;
}

int32
sys_humber_usrid_delete_vlan_entry(sys_usrid_vlan_entry_t *p_entry)
{
    int32 ret = CTC_E_NONE;
    uint8 lchip;
    uint16 gport;
    sys_usrid_db_vlan_node_t *p_vlan_node = NULL;
	sys_usrid_db_vlan_node_t vlan_node;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("---------usrid_vlan_key-----------\n");
    SYS_USRID_DBG_INFO(" global port             :%d\n", p_entry->usrid_key_entry.global_port);
    SYS_USRID_DBG_INFO(" cvlan_id                :%d\n", p_entry->usrid_key_entry.ingress_cvid);
    SYS_USRID_DBG_INFO(" cvlan_mask              :%d\n", p_entry->usrid_key_entry.igs_cvid_mask);
    SYS_USRID_DBG_INFO(" svlan_id                :%d\n", p_entry->usrid_key_entry.ingress_svid);
    SYS_USRID_DBG_INFO(" svlan_mask              :%d\n", p_entry->usrid_key_entry.igs_svid_mask);

    lchip = p_entry->usrid_key_entry.lchip;
    if (p_entry->valid.src_port_valid)
    {
        gport = p_entry->usrid_key_entry.global_port;
    }
    else
    {
        gport = CTC_MAX_UINT16_VALUE;
    }

    kal_memset(&vlan_node, 0, sizeof(sys_usrid_db_vlan_node_t));
    /*build usrid vlan node*/
    kal_memcpy(&(vlan_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_vlankey_node(&(vlan_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    USRID_VLAN_LOCK;
    /*write DB*/
    if (NULL == (p_vlan_node = sys_humber_usrid_db_delete_node(lchip, gport, SYS_USRID_VLAN_KEY, &vlan_node)))
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }

    SYS_USRID_DBG_INFO("Index of the deleted usrid vlan entry == %d\n", p_vlan_node->key_offset);
    /*remove entry from asic*/
    if((ret = drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, p_vlan_node->key_offset)) < 0)
    {
        goto error;
    }

    error:
        USRID_VLAN_UNLOCK;

        if (p_vlan_node)
        {
            mem_free(p_vlan_node);
        }

        return ret;
}

int32
sys_humber_usrid_add_mac_entry(sys_usrid_mac_entry_t *p_entry)
{
    sys_usrid_db_mac_node_t *p_mac_node = NULL;
	sys_usrid_db_mac_node_t mac_node;
    ds_user_id_mac_key_t usrid_mackey_data, usrid_mackey_mask;
    ds_user_id_t ds_usrid_mac;
    tbl_entry_t ds_usrid_mackey;
    uint8 chip_id;
    uint8 label_id;
    int32 ret = CTC_E_NONE;
    uint32 cmd_key, cmd_ds;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_mac_key-------------\n");
    SYS_USRID_DBG_INFO(" chip id                 :%d\n label_id                :%d\n",
    p_entry->usrid_key_entry.chip_id,
    p_entry->usrid_key_entry.usrid_label);
    SYS_USRID_DBG_INFO(" cvlan_id                :%d\n", p_entry->usrid_key_entry.igs_cvid);
    SYS_USRID_DBG_INFO(" svlan_id                :%d\n", p_entry->usrid_key_entry.igs_svid);
    SYS_USRID_DBG_INFO("macsa = 0x%X%X\n", p_entry->usrid_key_entry.macsa_h, p_entry->usrid_key_entry.macsa_l);
    SYS_USRID_DBG_INFO("macda = 0x%X%X\n", p_entry->usrid_key_entry.macda_h, p_entry->usrid_key_entry.macda_l);
    SYS_USRID_DBG_INFO("-------usrid_associate_data--------\n");
    SYS_USRID_DBG_INFO(" bypassall               :%d\n vlan_ptr                :0x%x\n",
    p_entry->ds_entry_usrid.by_pass_all,
    p_entry->ds_entry_usrid.usr_vlan_ptr);

    if (p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO(" binding macsa            :0x%x%x%x\n", p_entry->ds_entry_usrid.binding_data_h.binding_data,
       p_entry->ds_entry_usrid.binding_data_l.binding_data, p_entry->ds_entry_usrid.binding_data_m.binding_data);
    }

    if (p_entry->ds_entry_usrid.binding_en && !p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO(" binding global port      :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.binding_data);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.vpls_src_port_valid)
    {
        SYS_USRID_DBG_INFO(" vpls src port           :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.vpls_src_port);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.usr_svlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr svlan id            :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.usr_svlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.aps_select_group_valid)
    {
        SYS_USRID_DBG_INFO(" aps select group id     :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.aps_select_group_id);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.svr_id_en)
    {
        SYS_USRID_DBG_INFO(" service id              :0x%x\n", p_entry->ds_entry_usrid.binding_data_m.svr_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr cvlan id            :%d\n", p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.flow_policer_valid)
    {
        SYS_USRID_DBG_INFO(" flow policer ptr        :%d\n", p_entry->ds_entry_usrid.binding_data_m.flow_policer_ptr);
    }

    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" ds_fwd_ptr              :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.fwd_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.stats_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" stats_ptr               :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.stats_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.vrfid_valid)
    {
        SYS_USRID_DBG_INFO(" vrfid                   :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.vrf_id);
    }
    /*End debug info*/
    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);


    kal_memset(&mac_node, 0, sizeof(sys_usrid_db_mac_node_t));

    /*build usrid mac node*/
    kal_memcpy(&(mac_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_mackey_node(&(mac_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ds_node(&(mac_node.ds_node_usrid), &(p_entry->ds_entry_usrid)));

    /*build drv usrid key & mask & ds*/
    kal_memset(&usrid_mackey_data, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&usrid_mackey_mask, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&ds_usrid_mac, 0, sizeof(ds_user_id_mac_t));
    kal_memset(&ds_usrid_mackey, 0, sizeof(tbl_entry_t));

    ds_usrid_mackey.data_entry = (uint32 *)&usrid_mackey_data;
    ds_usrid_mackey.mask_entry = (uint32 *)&usrid_mackey_mask;

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_mackey_entry(&ds_usrid_mackey, p_entry));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_assoicate_entry(&ds_usrid_mac, &(mac_node.ds_node_usrid)));

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC, DRV_ENTRY_FLAG);

    USRID_MAC_LOCK;
	 p_mac_node = sys_humber_usrid_db_find_node(chip_id, label_id, SYS_USRID_MAC_KEY, &mac_node);
    if (NULL != p_mac_node)
	{
	    /*entry already exists, then update the node*/
		uint32   key_offset=0;;
	    key_offset =   p_mac_node->key_offset;
		kal_memcpy(p_mac_node,&mac_node, sizeof(sys_usrid_db_mac_node_t));
		p_mac_node->key_offset = key_offset;
	}
	else
	{
  	  p_mac_node =
      (sys_usrid_db_mac_node_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_db_mac_node_t));

      if (NULL == p_mac_node)
      {
           ret = CTC_E_NO_MEMORY;
           goto error;
      }

	   kal_memcpy(p_mac_node,&mac_node, sizeof(sys_usrid_db_mac_node_t));
       if ((ret = sys_humber_usrid_db_insert_node(chip_id, label_id, SYS_USRID_MAC_KEY, (void *)p_mac_node)) < 0)
       {
           mem_free(p_mac_node);
           goto error;
       }
	}


    SYS_USRID_DBG_INFO("Index of added usrid mac entry == %d\n", p_mac_node->key_offset);

    /*do write asic*/
    if ((ret = drv_tbl_ioctl(chip_id, p_mac_node->key_offset, cmd_ds, &ds_usrid_mac)) < 0)
    {
        goto error;
    }

    if ((ret = drv_tbl_ioctl(chip_id, p_mac_node->key_offset, cmd_key, &ds_usrid_mackey)) < 0)
    {
        goto error;
    }

    error:
        USRID_MAC_UNLOCK;
        return ret;
}

int32
sys_humber_usrid_delete_mac_entry(sys_usrid_mac_entry_t *p_entry)
{
    uint8 chip_id;
    int32 ret = CTC_E_NONE;
    uint8 label_id;
    sys_usrid_db_mac_node_t *p_mac_node = NULL;
    sys_usrid_db_mac_node_t mac_node;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_mac_key-------------\n");
    SYS_USRID_DBG_INFO(" chip id                 :%d\n label_id                :%d\n",
    p_entry->usrid_key_entry.chip_id,
    p_entry->usrid_key_entry.usrid_label);
    SYS_USRID_DBG_INFO("macsa = 0x%X%X\n", p_entry->usrid_key_entry.macsa_h, p_entry->usrid_key_entry.macsa_l);


    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);


    kal_memset(&mac_node, 0, sizeof(sys_usrid_db_mac_node_t));
    /*build usrid vlan node*/

    kal_memcpy(&(mac_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_mackey_node(&(mac_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    USRID_MAC_LOCK;
    /*write DB*/
    if (NULL == (p_mac_node = sys_humber_usrid_db_delete_node(chip_id, label_id, SYS_USRID_MAC_KEY, &mac_node)))
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }
    SYS_USRID_DBG_INFO("Index of the deleted usrid mac entry == %d\n", p_mac_node->key_offset);

    /*remove entry from asic*/
    if ((ret = drv_tcam_tbl_remove(chip_id, DS_USER_ID_MAC_KEY, p_mac_node->key_offset)) < 0)
    {
       goto error;
    }

    error:
        USRID_MAC_UNLOCK;

        if (p_mac_node)
        {
            mem_free(p_mac_node);
        }

        return ret;
}


int32
sys_humber_usrid_add_ipv4_entry(sys_usrid_ipv4_entry_t *p_entry)
{
    uint8 chip_id;
    uint8 label_id;
     int32 ret = CTC_E_NONE;
    uint32 cmd_key, cmd_ds;
    sys_usrid_db_ipv4_node_t *p_ipv4_node = NULL;
	sys_usrid_db_ipv4_node_t  ipv4_node;
    ds_user_id_ipv4_key_t usrid_ipv4key_data, usrid_ipv4key_mask;
    ds_user_id_t ds_usrid_ipv4;
    tbl_entry_t ds_usrid_ipv4key;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_ipv4_key-------------\n");
    SYS_USRID_DBG_INFO(" chip id                 :%d\n label_id                :%d\n",
    p_entry->usrid_key_entry.chip_id,
    p_entry->usrid_key_entry.usrid_label);

    SYS_USRID_DBG_INFO(" cvlan_id                :%d\n", p_entry->usrid_key_entry.cvid);
    SYS_USRID_DBG_INFO(" cvlan_mask              :%d\n", p_entry->usrid_key_entry.igs_cvid_mask);
    SYS_USRID_DBG_INFO(" svlan_id                :%d\n", p_entry->usrid_key_entry.svid);
    SYS_USRID_DBG_INFO(" svlan_mask              :%d\n", p_entry->usrid_key_entry.igs_svid_mask);

	SYS_USRID_DBG_INFO("macsa = 0x%X%X\n", p_entry->usrid_key_entry.macsa_h, p_entry->usrid_key_entry.macsa_l);
    SYS_USRID_DBG_INFO("macda = 0x%X%X\n", p_entry->usrid_key_entry.macda_h, p_entry->usrid_key_entry.macda_l);


	SYS_USRID_DBG_INFO("macsa mask = 0x%X%X\n", p_entry->usrid_key_entry.macsa_mask_h, p_entry->usrid_key_entry.macsa_mask_l);
    SYS_USRID_DBG_INFO("macda mask = 0x%X%X\n", p_entry->usrid_key_entry.macda_mask_h, p_entry->usrid_key_entry.macda_mask_l);

    SYS_USRID_DBG_INFO(" ipsa         = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_sa >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_sa & 0xFF);
    SYS_USRID_DBG_INFO(" ipsa mask    = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_sa_mask & 0xFF);


    SYS_USRID_DBG_INFO(" ipda         = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_da >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_da >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_da >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_da & 0xFF);
    SYS_USRID_DBG_INFO(" ipda mask    = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_da_mask >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_da_mask >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_da_mask >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_da_mask & 0xFF);

    SYS_USRID_DBG_INFO(" is_tcp                  :%d\n is_udp                  :%d\n",
    p_entry->usrid_key_entry.is_tcp, p_entry->usrid_key_entry.is_udp);
    SYS_USRID_DBG_INFO(" l4_src_port             :0x%x\n l4_dest_port            :0x%x\n",
    p_entry->usrid_key_entry.l4_src_port, p_entry->usrid_key_entry.l4_dest_port);

    SYS_USRID_DBG_INFO("--------usrid_associate_data---------\n");
    SYS_USRID_DBG_INFO(
    " bypassall               :%d\n vlan_ptr                :0x%x\n",
    p_entry->ds_entry_usrid.by_pass_all,
    p_entry->ds_entry_usrid.usr_vlan_ptr);

    if (p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO("binding macsa          :0x%x%x%x\n", p_entry->ds_entry_usrid.binding_data_h.binding_data,
       p_entry->ds_entry_usrid.binding_data_l.binding_data, p_entry->ds_entry_usrid.binding_data_m.binding_data);
    }

    if (p_entry->ds_entry_usrid.binding_en && !p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO("binding global port    :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.binding_data);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.vpls_src_port_valid)
    {
        SYS_USRID_DBG_INFO(" vpls src port           :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.vpls_src_port);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.usr_svlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr svlan id            :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.usr_svlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.aps_select_group_valid)
    {
        SYS_USRID_DBG_INFO(" aps select group id     :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.aps_select_group_id);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.svr_id_en)
    {
        SYS_USRID_DBG_INFO(" service id              :0x%x\n", p_entry->ds_entry_usrid.binding_data_m.svr_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr cvlan id            :%d\n", p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.flow_policer_valid)
    {
        SYS_USRID_DBG_INFO(" flow policer ptr        :%d\n", p_entry->ds_entry_usrid.binding_data_m.flow_policer_ptr);
    }

    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" ds_fwd_ptr              :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.fwd_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.stats_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" stats_ptr               :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.stats_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.vrfid_valid)
    {
        SYS_USRID_DBG_INFO(" vrfid                   :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.vrf_id);
    }
/*End debug info*/
    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);

    kal_memset(&ipv4_node, 0, sizeof(sys_usrid_db_ipv4_node_t));
    /*build usrid ipv4 node*/
    kal_memcpy(&(ipv4_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ipv4key_node(&(ipv4_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ds_node(&(ipv4_node.ds_node_usrid), &(p_entry->ds_entry_usrid)));

    /*build usrid key & mask & ds*/
    kal_memset(&usrid_ipv4key_data, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&usrid_ipv4key_mask, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&ds_usrid_ipv4, 0, sizeof(ds_user_id_t));
    kal_memset(&ds_usrid_ipv4key, 0, sizeof(tbl_entry_t));

    ds_usrid_ipv4key.data_entry = (uint32 *)&usrid_ipv4key_data;
    ds_usrid_ipv4key.mask_entry = (uint32 *)&usrid_ipv4key_mask;

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_ipv4key_entry(&ds_usrid_ipv4key, p_entry));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_assoicate_entry(&ds_usrid_ipv4, &(ipv4_node.ds_node_usrid)));

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4, DRV_ENTRY_FLAG);

    USRID_IPV4_LOCK;



	/*write DB*/
    p_ipv4_node = sys_humber_usrid_db_find_node(chip_id, label_id, SYS_USRID_IPV4_KEY, &ipv4_node);
    if (NULL != p_ipv4_node)
	{
	     /*entry already exists, then update the node*/
		uint32   key_offset=0;;
	    key_offset =   p_ipv4_node->key_offset;
		kal_memcpy(p_ipv4_node,&ipv4_node, sizeof(sys_usrid_db_ipv4_node_t));
		p_ipv4_node->key_offset = key_offset;

	}
	else
	{
 	   p_ipv4_node =
       (sys_usrid_db_ipv4_node_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_db_ipv4_node_t));

       if (NULL == p_ipv4_node)
       {
           ret = CTC_E_NO_MEMORY;
           goto error;
       }
 	   kal_memcpy(p_ipv4_node,&ipv4_node, sizeof(sys_usrid_db_ipv4_node_t));
      if ((ret = sys_humber_usrid_db_insert_node(chip_id, label_id, SYS_USRID_IPV4_KEY, (void *)p_ipv4_node)) < 0)
       {
           mem_free(p_ipv4_node);
           goto error;
       }
	}

    SYS_USRID_DBG_INFO("Index of added usrid ipv4 entry == %d\n", p_ipv4_node->key_offset);

    /* do write asic*/
    if ((ret = drv_tbl_ioctl(chip_id, p_ipv4_node->key_offset, cmd_ds, &ds_usrid_ipv4)) < 0)
    {
        goto error;
    }

    if ((ret = drv_tbl_ioctl(chip_id, p_ipv4_node->key_offset, cmd_key, &ds_usrid_ipv4key)) < 0)
    {
        goto error;
    }

    error:
        USRID_IPV4_UNLOCK;
        return ret;

}

int32
sys_humber_usrid_delete_ipv4_entry(sys_usrid_ipv4_entry_t *p_entry)
{
    uint8 chip_id;
    uint8 label_id;
     int32 ret = CTC_E_NONE;
    sys_usrid_db_ipv4_node_t *p_ipv4_node = NULL;
    sys_usrid_db_ipv4_node_t  ipv4_node;
    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_ipv4_key-------------\n");
    SYS_USRID_DBG_INFO(" chip id                 :%d\n label_id                :%d\n",
    p_entry->usrid_key_entry.chip_id,
    p_entry->usrid_key_entry.usrid_label);

    SYS_USRID_DBG_INFO(" ipsa         = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_sa >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_sa & 0xFF);
    SYS_USRID_DBG_INFO(" ipsa mask    = %d.%d.%d.%d\n",
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 24) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 16) & 0xFF,
                        (p_entry->usrid_key_entry.ipv4_sa_mask >> 8) & 0xFF,
                        p_entry->usrid_key_entry.ipv4_sa_mask & 0xFF);
    SYS_USRID_DBG_INFO(" is_tcp                  :%d\n is_udp                  :%d\n",
    p_entry->usrid_key_entry.is_tcp, p_entry->usrid_key_entry.is_udp);
    SYS_USRID_DBG_INFO(" l4_src_port             :0x%x\n l4_dest_port             :0x%x\n",
    p_entry->usrid_key_entry.l4_src_port, p_entry->usrid_key_entry.l4_dest_port);
    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);


    kal_memset(&ipv4_node, 0, sizeof(sys_usrid_db_ipv4_node_t));
    /*build usrid vlan node*/

    kal_memcpy(&(ipv4_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ipv4key_node(&(ipv4_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    USRID_IPV4_LOCK;
    /*write DB*/
    if (NULL == (p_ipv4_node = sys_humber_usrid_db_delete_node(chip_id, label_id, SYS_USRID_IPV4_KEY, &ipv4_node)))
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }

    SYS_USRID_DBG_INFO("Index of the deleted usrid ipv4 entry == %d\n", p_ipv4_node->key_offset);

    /*remove entry from asic*/
    if ((ret = drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV4_KEY, p_ipv4_node->key_offset)) < 0)
    {
        goto error;
    }

    error:
        USRID_IPV4_UNLOCK;

        if (p_ipv4_node)
        {
            mem_free(p_ipv4_node);
        }

        return ret;
}

int32
sys_humber_usrid_add_ipv6_entry(sys_usrid_ipv6_entry_t *p_entry)
{
    uint8 chip_id;
    uint8 label_id;
     int32 ret = CTC_E_NONE;
    uint32 cmd_key, cmd_ds;
    uint8 i;
    sys_usrid_db_ipv6_node_t *p_ipv6_node = NULL;
	sys_usrid_db_ipv6_node_t ipv6_node;
    ds_user_id_ipv6_key_t usrid_ipv6key_data, usrid_ipv6key_mask;
    ds_user_id_t ds_usrid_ipv6;
    tbl_entry_t ds_usrid_ipv6key;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_ipv6_key-------------\n");
    SYS_USRID_DBG_INFO(" chip id                 :%d\n label_id                :%d\n",
    p_entry->usrid_key_entry.chip_id,
    p_entry->usrid_key_entry.usrid_label);

    SYS_USRID_DBG_INFO(" ipsa = 0x");
    for(i = 0; i < 4; i++)
    {
        SYS_USRID_DBG_INFO("%x:", p_entry->usrid_key_entry.ipv6_da[i]);
    }
    SYS_USRID_DBG_INFO("\n");

    SYS_USRID_DBG_INFO(" ipsa = 0x");
    for(i = 0; i < 4; i++)
    {
        SYS_USRID_DBG_INFO("%x:", p_entry->usrid_key_entry.ipv6_sa[i]);
    }
    SYS_USRID_DBG_INFO("\n");

    SYS_USRID_DBG_INFO(" is_tcp                  :%d\n is_udp                  :%d\n",
    p_entry->usrid_key_entry.is_tcp, p_entry->usrid_key_entry.is_udp);
    SYS_USRID_DBG_INFO(" l4_src_port             :0x%x\n l4_dest_port             :0x%x\n",
    p_entry->usrid_key_entry.l4_src_port, p_entry->usrid_key_entry.l4_dest_port);
    SYS_USRID_DBG_INFO("-------usrid_associate_data-------\n");
    SYS_USRID_DBG_INFO(
    " bypassall               :%d\n vlan_ptr                :0x%x\n",
    p_entry->ds_entry_usrid.by_pass_all,
    p_entry->ds_entry_usrid.usr_vlan_ptr);

    if (p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO("binding macsa          :0x%x%x%x\n", p_entry->ds_entry_usrid.binding_data_h.binding_data,
       p_entry->ds_entry_usrid.binding_data_l.binding_data, p_entry->ds_entry_usrid.binding_data_m.binding_data);
    }

    if (p_entry->ds_entry_usrid.binding_en && !p_entry->ds_entry_usrid.binding_macsa)
    {
       SYS_USRID_DBG_INFO("binding global port    :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.binding_data);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.vpls_src_port_valid)
    {
        SYS_USRID_DBG_INFO(" vpls src port           :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.vpls_src_port);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.usr_svlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr svlan id            :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.usr_svlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_h.aps_select_group_valid)
    {
        SYS_USRID_DBG_INFO(" aps select group id     :0x%x\n", p_entry->ds_entry_usrid.binding_data_h.aps_select_group_id);
    }

    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.svr_id_en)
    {
        SYS_USRID_DBG_INFO(" service id              :0x%x\n", p_entry->ds_entry_usrid.binding_data_m.svr_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_valid)
    {
        SYS_USRID_DBG_INFO(" usr cvlan id            :%d\n", p_entry->ds_entry_usrid.binding_data_m.usr_cvlan_id);
    }
    if (!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.binding_data_m.flow_policer_valid)
    {
        SYS_USRID_DBG_INFO(" flow policer ptr        :%d\n", p_entry->ds_entry_usrid.binding_data_m.flow_policer_ptr);
    }

    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" ds_fwd_ptr              :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.fwd_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.stats_ptr_valid)
    {
        SYS_USRID_DBG_INFO(" stats_ptr               :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.stats_ptr);
    }
    if(!p_entry->ds_entry_usrid.binding_en && p_entry->ds_entry_usrid.vrfid_valid)
    {
        SYS_USRID_DBG_INFO(" vrfid                   :0x%x\n", p_entry->ds_entry_usrid.binding_data_l.vrf_id);
    }
/*End debug info*/
    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);

    kal_memset(&ipv6_node, 0, sizeof(sys_usrid_db_ipv6_node_t));

    /*build usrid ipv6 node*/
    kal_memcpy(&(ipv6_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ipv6key_node(&(ipv6_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ds_node(&(ipv6_node.ds_node_usrid), &(p_entry->ds_entry_usrid)));

    /*build usrid key & mask & ds*/
    kal_memset(&usrid_ipv6key_data, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&usrid_ipv6key_mask, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&ds_usrid_ipv6, 0, sizeof(ds_user_id_t));
    kal_memset(&ds_usrid_ipv6key, 0, sizeof(tbl_entry_t));

    ds_usrid_ipv6key.data_entry = (uint32 *)&usrid_ipv6key_data;
    ds_usrid_ipv6key.mask_entry = (uint32 *)&usrid_ipv6key_mask;

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_ipv6key_entry(&ds_usrid_ipv6key, p_entry));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_drv_assoicate_entry(&ds_usrid_ipv6, &(ipv6_node.ds_node_usrid)));

    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6_KEY, DRV_ENTRY_FLAG);
    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6, DRV_ENTRY_FLAG);

    USRID_IPV6_LOCK;
    /*write DB*/
	p_ipv6_node = sys_humber_usrid_db_find_node(chip_id, label_id, SYS_USRID_IPV6_KEY, &ipv6_node);
    if (NULL != p_ipv6_node)
	{
		/*entry already exists, then update the node*/
		uint32   key_offset=0;;
	    key_offset =   p_ipv6_node->key_offset;
		kal_memcpy(p_ipv6_node,&ipv6_node, sizeof(sys_usrid_db_ipv6_node_t));
		p_ipv6_node->key_offset = key_offset;

	}
	else
	{
 	   p_ipv6_node =
       (sys_usrid_db_ipv6_node_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_db_ipv6_node_t));

       if (NULL == p_ipv6_node)
       {
           ret = CTC_E_NO_MEMORY;
           goto error;
       }
 	   kal_memcpy(p_ipv6_node,&ipv6_node, sizeof(sys_usrid_db_ipv6_node_t));
       if ((ret = sys_humber_usrid_db_insert_node(chip_id, label_id, SYS_USRID_IPV6_KEY, (void *)p_ipv6_node)) <0)
       {
           mem_free(p_ipv6_node);
           goto error;
       }
	}

    SYS_USRID_DBG_INFO("Index of added usrid ipv6 entry == %d\n", p_ipv6_node->key_offset);

    /* do write asic*/
    if ((ret = drv_tbl_ioctl(chip_id, p_ipv6_node->key_offset, cmd_ds, &ds_usrid_ipv6)) < 0)
    {
        goto error;
    }

    if ((ret = drv_tbl_ioctl(chip_id, p_ipv6_node->key_offset, cmd_key, &ds_usrid_ipv6key)) < 0)
    {
        goto error;
    }

    error:
        USRID_IPV6_UNLOCK;
        return ret;

}

int32
sys_humber_usrid_delete_ipv6_entry(sys_usrid_ipv6_entry_t *p_entry)
{
     int32 ret = CTC_E_NONE;
    uint8 chip_id;
    uint8 label_id;
    uint8 i;
    sys_usrid_db_ipv6_node_t *p_ipv6_node = NULL;
    sys_usrid_db_ipv6_node_t ipv6_node;

    CTC_PTR_VALID_CHECK(p_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("----------usrid_ipv4_key-------------\n");
    SYS_USRID_DBG_INFO("label_id = %d\n", p_entry->usrid_key_entry.usrid_label);
    SYS_USRID_DBG_INFO("macda_h = 0x%x, macda_l = 0x%x, macsa_h = 0x%x, macsa_l = 0x%x\n",
    p_entry->usrid_key_entry.macda_h, p_entry->usrid_key_entry.macda_l, p_entry->usrid_key_entry.macsa_h, p_entry->usrid_key_entry.macsa_l);
    SYS_USRID_DBG_INFO("ipda = 0x");
    for(i = 0; i < 4; i++)
    {
        SYS_USRID_DBG_INFO("%x ", p_entry->usrid_key_entry.ipv6_da[i]);
    }
    SYS_USRID_DBG_INFO("\n");
    SYS_USRID_DBG_INFO("ipsa = 0x");
    for(i = 0; i < 4; i++)
    {
        SYS_USRID_DBG_INFO("%x ", p_entry->usrid_key_entry.ipv6_sa[i]);
    }
    SYS_USRID_DBG_INFO("\n");
    SYS_USRID_DBG_INFO("cvlan_id = 0x%x, svlan_id = 0x%x\n", p_entry->usrid_key_entry.cvid, p_entry->usrid_key_entry.svid);
    SYS_USRID_DBG_INFO("is_Tcp = %d, is_Udp = %d\n", p_entry->usrid_key_entry.is_tcp, p_entry->usrid_key_entry.is_udp);
    SYS_USRID_DBG_INFO("l4_src_port = %d, l4_dest_port = %d\n", p_entry->usrid_key_entry.l4_src_port, p_entry->usrid_key_entry.l4_dest_port);

    chip_id = p_entry->usrid_key_entry.chip_id;
    label_id = p_entry->usrid_key_entry.usrid_label;

    SYS_LOCAL_CHIPID_CHECK(chip_id);
    SYS_USRID_LABEL_CHECK(label_id);


    kal_memset(&ipv6_node, 0, sizeof(sys_usrid_db_ipv6_node_t));
    /*build usrid vlan node*/

    kal_memcpy(&(ipv6_node.valid), &(p_entry->valid), sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(
    _sys_humber_usrid_build_db_ipv6key_node(&(ipv6_node.usrid_key_node), &(p_entry->usrid_key_entry)));

    USRID_IPV6_LOCK;
    /*write DB*/
    if (NULL == (p_ipv6_node = sys_humber_usrid_db_delete_node(chip_id, label_id, SYS_USRID_IPV6_KEY, &ipv6_node)))
    {
        ret = CTC_E_ENTRY_NOT_EXIST;
        goto error;
    }

    SYS_USRID_DBG_INFO("Index of the deleted usrid ipv6 entry == %d\n", p_ipv6_node->key_offset);

    /*remove entry from asic*/
    if ((ret = drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV6_KEY, p_ipv6_node->key_offset)) < 0)
    {
        goto error;
    }

    error:
        USRID_IPV6_UNLOCK;

        if (p_ipv6_node)
        {
            mem_free(p_ipv6_node);
        }

        return ret;
}


int32
sys_humber_usrid_add_vlan_default_entry_per_port(uint16 gport, sys_usrid_ds_entry_t *usrid_def_entry)
{
    int32 ret = 0;
    uint8 lchip;
    uint8 gchip;
    uint8 lchip_num;
    uint32 offset;
    uint32 cmd_key, cmd_ds;
    tbl_entry_t usrid_key;
    ds_user_id_vlan_key_t key_data, key_mask;
    ds_user_id_t usrid_ds;
    sys_usrid_db_ds_node_t ds_node;

    CTC_PTR_VALID_CHECK(usrid_def_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    kal_memset(&key_data, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&key_mask, 0, sizeof(ds_user_id_vlan_key_t));
    kal_memset(&usrid_ds, 0, sizeof(ds_user_id_t));

    /*build key & mask*/
    key_data.table_id = USERID_VLAN_TABLEID_C;
    key_mask.table_id = 0xF;

    key_data.global_src_port = gport;
    key_mask.global_src_port = 0x1FFF;

    usrid_key.data_entry = (uint32 *)&key_data;
    usrid_key.mask_entry = (uint32 *)&key_mask;

    CTC_ERROR_RETURN(_sys_humber_usrid_build_db_ds_node(&ds_node, usrid_def_entry));
    CTC_ERROR_RETURN(_sys_humber_usrid_build_drv_assoicate_entry(&usrid_ds, &ds_node));

    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN, DRV_ENTRY_FLAG);
    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_VLAN_KEY, DRV_ENTRY_FLAG);

    USRID_VLAN_LOCK;

    if (CTC_IS_LINKAGG_PORT(gport))
    {

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            ret = sys_humber_usrid_db_alloc_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &offset);
            SYS_USRID_DBG_INFO("add per port:%d default entry, local chip=%d, offset=%d!\n", gport, lchip, offset);
            ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_ds, &usrid_ds);
            ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_key, &usrid_key);
        }
    }
    else
    {
        gchip = gport >> CTC_LOCAL_PORT_LENGTH;
        if(!sys_humber_chip_is_local(gchip, &lchip))
        {
            USRID_VLAN_UNLOCK;
            return ret;
        }
        ret = ret? ret:sys_humber_usrid_db_alloc_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &offset);
        SYS_USRID_DBG_INFO("add per port:%d default entry, local chip=%d, offset=%d!\n", gport, lchip, offset);
        ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_ds, &usrid_ds);
        ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_key, &usrid_key);

    }
    USRID_VLAN_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_delete_vlan_default_entry_per_port(uint16 gport)
{
    int32 ret = 0;
    uint8 lchip;
    uint8 lchip_num;
    uint8 gchip;
    uint32 offset;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    USRID_VLAN_LOCK;

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            ret = ret? ret:sys_humber_usrid_db_free_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &offset);
            SYS_USRID_DBG_INFO("remove per port:%d default entry, local chip=%d, offset=%d!\n", gport, lchip, offset);
            ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, offset);
        }
    }
    else
    {
        gchip = gport >> CTC_LOCAL_PORT_LENGTH;
        if(!sys_humber_chip_is_local(gchip, &lchip))
        {
            USRID_VLAN_UNLOCK;
            return ret;
        }

        ret = ret? ret:sys_humber_usrid_db_free_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &offset);
        SYS_USRID_DBG_INFO("remove per port:%d default entry, local chip=%d, offset=%d!\n", gport, lchip, offset);
        ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, offset);
    }

    USRID_VLAN_UNLOCK;

    return ret;
}


int32
sys_humber_usrid_add_mac_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry)
{
    int32 ret = 0;
    uint32 offset;
    uint32 cmd_key, cmd_ds;
    tbl_entry_t usrid_key;
    ds_user_id_mac_key_t key_data, key_mask;
    ds_user_id_t usrid_ds;
    sys_usrid_db_ds_node_t ds_node;

    SYS_USRID_LABEL_CHECK(label_id);
    CTC_PTR_VALID_CHECK(usrid_def_entry);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    kal_memset(&key_data, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&key_mask, 0, sizeof(ds_user_id_mac_key_t));
    kal_memset(&usrid_ds, 0, sizeof(ds_user_id_t));

    key_data.table_id0 = USERID_MAC_TABLEID0_C;
    key_mask.table_id0 = 0xF;

    key_data.table_id1 = USERID_MAC_TABLEID1_C;
    key_mask.table_id1 = 0xF;

    key_data.user_id_label = label_id;
    key_mask.user_id_label = 0x3F;

    usrid_key.data_entry = (uint32 *)&key_data;
    usrid_key.mask_entry = (uint32 *)&key_mask;

    CTC_ERROR_RETURN(_sys_humber_usrid_build_db_ds_node(&ds_node, usrid_def_entry));
    CTC_ERROR_RETURN(_sys_humber_usrid_build_drv_assoicate_entry(&usrid_ds, &ds_node));

    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC, DRV_ENTRY_FLAG);
    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_MAC_KEY, DRV_ENTRY_FLAG);

    USRID_MAC_LOCK;
    ret = ret? ret:sys_humber_usrid_db_alloc_resv_offset(lchip, label_id, SYS_USRID_MAC_KEY, &offset);
    SYS_USRID_DBG_INFO("Add USRID_MAC per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_ds, &usrid_ds);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_key, &usrid_key);
    USRID_MAC_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_delete_mac_default_entry_per_label(uint8 lchip, uint8 label_id)
{
    int32 ret = 0;
    uint32 offset = 0;

    SYS_USRID_LABEL_CHECK(label_id);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    USRID_MAC_LOCK;
    ret = ret? ret:sys_humber_usrid_db_free_resv_offset(lchip, label_id, SYS_USRID_MAC_KEY, &offset);
    SYS_USRID_DBG_INFO("Remove USRID_MAC per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_MAC_KEY, offset);
    USRID_MAC_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_add_ipv4_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry)
{
    int32 ret = 0;
    uint32 offset;
    uint32 cmd_key, cmd_ds;
    tbl_entry_t usrid_key;
    ds_user_id_ipv4_key_t key_data, key_mask;
    ds_user_id_t usrid_ds;
    sys_usrid_db_ds_node_t ds_node;

    CTC_PTR_VALID_CHECK(usrid_def_entry);
    SYS_USRID_LABEL_CHECK(label_id);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    kal_memset(&key_data, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&key_mask, 0, sizeof(ds_user_id_ipv4_key_t));
    kal_memset(&usrid_ds, 0, sizeof(ds_user_id_t));

        /*build key & mask*/
    key_data.table_id0 = USERID_IPV4_TABLEID0_C;
    key_mask.table_id0 = 0xF;
    key_data.table_id1 = USERID_IPV4_TABLEID1_C;
    key_mask.table_id1 = 0xF;
    key_data.table_id2 = USERID_IPV4_TABLEID2_C;
    key_mask.table_id2 = 0xF;
    key_data.table_id3 = USERID_IPV4_TABLEID3_C;
    key_mask.table_id3 = 0xF;

    key_data.user_id_label = label_id;
    key_mask.user_id_label = 0x3F;

    usrid_key.data_entry = (uint32 *)&key_data;
    usrid_key.mask_entry = (uint32 *)&key_mask;

    CTC_ERROR_RETURN(_sys_humber_usrid_build_db_ds_node(&ds_node, usrid_def_entry));
    CTC_ERROR_RETURN(_sys_humber_usrid_build_drv_assoicate_entry(&usrid_ds, &ds_node));

    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4, DRV_ENTRY_FLAG);
    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV4_KEY, DRV_ENTRY_FLAG);

    USRID_IPV4_LOCK;
    ret = ret? ret:sys_humber_usrid_db_alloc_resv_offset(lchip, label_id, SYS_USRID_IPV4_KEY, &offset);
    SYS_USRID_DBG_INFO("Add USRID_IPV4 per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_ds, &usrid_ds);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_key, &usrid_key);
    USRID_IPV4_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_delete_ipv4_default_entry_per_label(uint8 lchip, uint8 label_id)
{
    int32 ret = 0;
    uint32 offset;

    SYS_USRID_LABEL_CHECK(label_id);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    USRID_IPV4_LOCK;
    ret = ret? ret:sys_humber_usrid_db_free_resv_offset(lchip, label_id, SYS_USRID_IPV4_KEY, &offset);
    SYS_USRID_DBG_INFO("Remove USRID_IPV4 per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_IPV4_KEY, offset);
    USRID_IPV4_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_add_ipv6_default_entry_per_label(uint8 lchip, uint8 label_id, sys_usrid_ds_entry_t *usrid_def_entry)
{
    int32 ret = 0;
    uint32 offset;
    uint32 cmd_key, cmd_ds;
    tbl_entry_t usrid_key;
    ds_user_id_ipv6_key_t key_data, key_mask;
    ds_user_id_t usrid_ds;
    sys_usrid_db_ds_node_t ds_node;

    CTC_PTR_VALID_CHECK(usrid_def_entry);
    SYS_USRID_LABEL_CHECK(label_id);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    kal_memset(&key_data, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&key_mask, 0, sizeof(ds_user_id_ipv6_key_t));
    kal_memset(&usrid_ds, 0, sizeof(ds_user_id_t));

    /*build key & mask*/
    key_data.table_id0 = USERID_IPV6_TABLEID0_C;
    key_mask.table_id0 = 0xF;
    key_data.table_id1 = USERID_IPV6_TABLEID1_C;
    key_mask.table_id1 = 0xF;
    key_data.table_id2 = USERID_IPV6_TABLEID2_C;
    key_mask.table_id2 = 0xF;
    key_data.table_id3 = USERID_IPV6_TABLEID3_C;
    key_mask.table_id3 = 0xF;
    key_data.table_id4 = USERID_IPV6_TABLEID4_C;
    key_mask.table_id4 = 0xF;
    key_data.table_id5 = USERID_IPV6_TABLEID5_C;
    key_mask.table_id5 = 0xF;
    key_data.table_id6 = USERID_IPV6_TABLEID6_C;
    key_mask.table_id6 = 0xF;
    key_data.table_id7 = USERID_IPV6_TABLEID7_C;
    key_mask.table_id7 = 0xF;

    key_data.user_id_label = label_id;
    key_mask.user_id_label = 0x3F;

    usrid_key.data_entry = (uint32 *)&key_data;
    usrid_key.mask_entry = (uint32 *)&key_mask;

    CTC_ERROR_RETURN(_sys_humber_usrid_build_db_ds_node(&ds_node, usrid_def_entry));
    CTC_ERROR_RETURN(_sys_humber_usrid_build_drv_assoicate_entry(&usrid_ds, &ds_node));

    cmd_ds = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6, DRV_ENTRY_FLAG);
    cmd_key = DRV_IOW(IOC_TABLE, DS_USER_ID_IPV6_KEY, DRV_ENTRY_FLAG);

    USRID_IPV6_LOCK;
    ret = ret? ret:sys_humber_usrid_db_alloc_resv_offset(lchip, label_id, SYS_USRID_IPV6_KEY, &offset);
    SYS_USRID_DBG_INFO("Add USRID_IPV6 per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_ds, &usrid_ds);
    ret = ret? ret:drv_tbl_ioctl(lchip, offset, cmd_key, &usrid_key);
    USRID_IPV6_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_delete_ipv6_default_entry_per_label(uint8 lchip, uint8 label_id)
{
    int32 ret = 0;
    uint32 offset;

    SYS_USRID_LABEL_CHECK(label_id);
    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    USRID_IPV6_LOCK;
    ret = ret? ret:sys_humber_usrid_db_free_resv_offset(lchip, label_id, SYS_USRID_IPV6_KEY, &offset);
    SYS_USRID_DBG_INFO("Remove USRID_IPV6 per label:%d default entry, local chip=%d, offset=%d!\n", label_id, lchip, offset);
    ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_IPV6_KEY, offset);
    USRID_IPV6_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_remove_vlan_all_by_port(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 lchip_num = 0;
    uint16 index = 0;
    int32 ret = 0;
    uint32 key_offset = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    CTC_GLOBAL_PORT_CHECK(gport);
    lchip_num = sys_humber_get_local_chip_num();

    USRID_VLAN_LOCK;
    do
    {
        if (CTC_IS_LINKAGG_PORT(gport))
        {
            index = CTC_MAP_GPORT_TO_LPORT(gport) + MAX_PORT_NUM_PER_CHIP;

            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                ret = sys_humber_usrid_db_remove_node_by_label_port(lchip, index, SYS_USRID_VLAN_KEY, &key_offset);
                ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, key_offset);
            }
        }
        else
        {
            SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
            index = lport;

            ret = sys_humber_usrid_db_remove_node_by_label_port(lchip, index, SYS_USRID_VLAN_KEY, &key_offset);
            ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, key_offset);
        }
    }while(CTC_E_NONE == ret);

    /*per port default entry*/
    if (CTC_IS_LINKAGG_PORT(gport))
    {
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            ret = sys_humber_usrid_db_free_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &key_offset);
            ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, key_offset);
        }
    }
    else
    {
        SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
        ret = sys_humber_usrid_db_free_resv_offset(lchip, gport, SYS_USRID_VLAN_KEY, &key_offset);
        ret = ret? ret:drv_tcam_tbl_remove(lchip, DS_USER_ID_VLAN_KEY, key_offset);
    }

    ret = (CTC_E_ENTRY_NOT_EXIST == ret)? CTC_E_NONE:ret;
    USRID_VLAN_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_remove_mac_all_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    int32 ret = 0;
    uint32 offset = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);
    lchip_num = sys_humber_get_local_chip_num();

    USRID_MAC_LOCK;
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        do
        {
            ret = sys_humber_usrid_db_remove_node_by_label_port(chip_id, label_id, SYS_USRID_MAC_KEY, &offset);

            ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_MAC_KEY, offset);

        }while(CTC_E_NONE == ret);

        /*remove default entry*/
        ret = sys_humber_usrid_db_free_resv_offset(chip_id, label_id, SYS_USRID_MAC_KEY, &offset);
        ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_MAC_KEY, offset);
    }

    ret = (CTC_E_ENTRY_NOT_EXIST == ret)? CTC_E_NONE: ret;
    USRID_MAC_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_remove_ipv4_all_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    int32 ret = 0;
    uint32 offset = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    USRID_IPV4_LOCK;
    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        do
        {
            ret = sys_humber_usrid_db_remove_node_by_label_port(chip_id, label_id, SYS_USRID_IPV4_KEY, &offset);

            ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV4_KEY, offset);

        }while(CTC_E_NONE == ret);

        ret = sys_humber_usrid_db_free_resv_offset(chip_id, label_id, SYS_USRID_IPV4_KEY, &offset);
        ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV4_KEY, offset);
    }

    ret = (CTC_E_ENTRY_NOT_EXIST == ret)? CTC_E_NONE: ret;
    USRID_IPV4_UNLOCK;

    return ret;
}

int32
sys_humber_usrid_remove_ipv6_all_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    int32 ret = 0;
    uint32 offset = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);
    lchip_num = sys_humber_get_local_chip_num();

    USRID_IPV6_LOCK;
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        do
        {
            ret = sys_humber_usrid_db_remove_node_by_label_port(chip_id, label_id, SYS_USRID_IPV6_KEY, &offset);

            ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV6_KEY, offset);

        }while(CTC_E_NONE == ret);

        ret = sys_humber_usrid_db_free_resv_offset(chip_id, label_id, SYS_USRID_IPV6_KEY, &offset);
        ret = ret? ret:drv_tcam_tbl_remove(chip_id, DS_USER_ID_IPV6_KEY, offset);

    }

    ret = (CTC_E_ENTRY_NOT_EXIST == ret)? CTC_E_NONE: ret;
    USRID_IPV6_UNLOCK;

    return ret;
}

static void
_sys_humber_usrid_show_vlan_default_entry(uint8 chip_id, uint16 gport)
{
    int32 ret = 0;
    uint32 key_offset = 0;
    uint32 cmd = 0;
    uint8 lchip = 0;
    ds_user_id_vlan_t ds_user_id_vlan;

    ret = sys_humber_usrid_db_get_resv_offset(chip_id, gport, SYS_USRID_VLAN_KEY, &key_offset);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Default entry                       :none\n");
        SYS_USRID_DBG_INFO("========================================\n");
        return;
    }
    SYS_USRID_DBG_INFO("Default entry\n");

    cmd = DRV_IOR(IOC_TABLE, DS_USER_ID_VLAN, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, key_offset, cmd, &ds_user_id_vlan);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Read associate data error\n");
        return;
    }
    SYS_USRID_DBG_INFO("Key offset                          :%d\n", key_offset);
    SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", ds_user_id_vlan.by_pass_all);
    SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", ds_user_id_vlan.user_vlan_ptr);

    if (ds_user_id_vlan.binding_en)
    {
        SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", ds_user_id_vlan.binding_en);
        if (ds_user_id_vlan.binding_mac_sa)
        {
            SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                ds_user_id_vlan.binding_datah,
                                ds_user_id_vlan.binding_datam,
                                ds_user_id_vlan.binding_datal);
        }
        else
        {
            SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                ds_user_id_vlan.binding_datah,
                                ds_user_id_vlan.binding_datam,
                                ds_user_id_vlan.binding_datal);
        }

    }
    else if (ds_user_id_vlan.ds_fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", ds_user_id_vlan.binding_datal);
    }
    else if (ds_user_id_vlan.binding_mac_sa)
    {
        SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", ds_user_id_vlan.binding_datal);
    }
    else if (0xFFFF != ds_user_id_vlan.binding_datal)
    {
        SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", ds_user_id_vlan.binding_datal);
    }

    /*binding data high*/
    if (CTC_FLAG_ISSET(ds_user_id_vlan.binding_datah , 0x8000))
    {
        SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_vlan.binding_datah, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
        SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                         (ds_user_id_vlan.binding_datah & 0x1FFF));
    }
    else if (!CTC_FLAG_ISSET(ds_user_id_vlan.binding_datah, 0x1000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                    (ds_user_id_vlan.binding_datah & 0xFFF));

    }
    else if ((ds_user_id_vlan.aps_select_valid)
    && (0xFFF !=(ds_user_id_vlan.binding_datah & 0xFFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
        SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                            (ds_user_id_vlan.binding_datah & 0xFFF));
    }

    if (ds_user_id_vlan.aps_select_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
        SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                            ds_user_id_vlan.binding_datah >> 13 & 0x1);
    }

    /*binding data middle*/
    if (CTC_FLAG_ISSET(ds_user_id_vlan.binding_datam , 0xc000))
    {
        SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_vlan.binding_datam, 0x8000)
    && (0x3FFF != (ds_user_id_vlan.binding_datam & 0x3FFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
        SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                            (ds_user_id_vlan.binding_datam & 0x3FFF));

    }
    else if (CTC_FLAG_ISSET(ds_user_id_vlan.binding_datam, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
        SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                            (ds_user_id_vlan.binding_datam & 0xFFF));
    }
    else if (0x3FFF != (ds_user_id_vlan.binding_datam & 0x3FFF))
    {
        SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                             (ds_user_id_vlan.binding_datam & 0x3FFF));

    }

    SYS_USRID_DBG_INFO("========================================\n");
    return;
}


int32
sys_humber_usrid_show_vlan_by_port(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint8 lport = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    CTC_GLOBAL_PORT_CHECK(gport);

    lchip_num = sys_humber_get_local_chip_num();

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            sys_humber_usrid_db_show_vlan_by_port(lchip, gport);
            _sys_humber_usrid_show_vlan_default_entry(lchip, gport);
        }
    }
    else
    {
        SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
        sys_humber_usrid_db_show_vlan_by_port(lchip, gport);
        _sys_humber_usrid_show_vlan_default_entry(lchip, gport);
    }

    return CTC_E_NONE;
}

static void
_sys_humber_usrid_show_mac_default_entry(uint8 chip_id, uint8 label_id)
{
    int32 ret = 0;
    uint32 key_offset = 0;
    uint32 cmd = 0;
    uint8 lchip = 0;
    ds_user_id_mac_t ds_user_id_mac;

    ret = sys_humber_usrid_db_get_resv_offset(lchip, label_id, SYS_USRID_MAC_KEY, &key_offset);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Default entry                       :none\n");
        SYS_USRID_DBG_INFO("========================================\n");
        return;
    }
    SYS_USRID_DBG_INFO("Default entry\n");

    cmd = DRV_IOR(IOC_TABLE, DS_USER_ID_MAC, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, key_offset, cmd, &ds_user_id_mac);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Read associate data error\n");
        return;
    }
    SYS_USRID_DBG_INFO("Key offset                          :%d\n", key_offset);
    SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", ds_user_id_mac.by_pass_all);
    SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", ds_user_id_mac.user_vlan_ptr);

    if (ds_user_id_mac.binding_en)
    {
        SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", ds_user_id_mac.binding_en);
        if (ds_user_id_mac.binding_mac_sa)
        {
            SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                ds_user_id_mac.binding_datah,
                                ds_user_id_mac.binding_datam,
                                ds_user_id_mac.binding_datal);
        }
        else
        {
            SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                ds_user_id_mac.binding_datah,
                                ds_user_id_mac.binding_datam,
                                ds_user_id_mac.binding_datal);
        }

    }
    else if (ds_user_id_mac.ds_fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", ds_user_id_mac.binding_datal);
    }
    else if (ds_user_id_mac.binding_mac_sa)
    {
        SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", ds_user_id_mac.binding_datal);
    }
    else if (0xFFFF != ds_user_id_mac.binding_datal)
    {
        SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", ds_user_id_mac.binding_datal);
    }

    /*binding data high*/
    if (CTC_FLAG_ISSET(ds_user_id_mac.binding_datah , 0x8000))
    {
        SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_mac.binding_datah, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
        SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                         (ds_user_id_mac.binding_datah & 0x1FFF));
    }
    else if (!CTC_FLAG_ISSET(ds_user_id_mac.binding_datah, 0x1000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                    (ds_user_id_mac.binding_datah & 0xFFF));

    }
    else if ((ds_user_id_mac.aps_select_valid)
    && (0xFFF !=(ds_user_id_mac.binding_datah & 0xFFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
        SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                            (ds_user_id_mac.binding_datah & 0xFFF));
    }

    if (ds_user_id_mac.aps_select_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
        SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                            ds_user_id_mac.binding_datah >> 13 & 0x1);
    }

    /*binding data middle*/
    if (CTC_FLAG_ISSET(ds_user_id_mac.binding_datam , 0xc000))
    {
        SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_mac.binding_datam, 0x8000)
    && (0x3FFF != (ds_user_id_mac.binding_datam & 0x3FFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
        SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                            (ds_user_id_mac.binding_datam & 0x3FFF));

    }
    else if (CTC_FLAG_ISSET(ds_user_id_mac.binding_datam, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
        SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                            (ds_user_id_mac.binding_datam & 0xFFF));
    }
    else if (0x3FFF != (ds_user_id_mac.binding_datam & 0x3FFF))
    {
        SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                             (ds_user_id_mac.binding_datam & 0x3FFF));

    }

    SYS_USRID_DBG_INFO("========================================\n");
    return;
}

int32
sys_humber_usrid_show_mac_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_mac_by_label(chip_id, label_id);
        _sys_humber_usrid_show_mac_default_entry(chip_id, label_id);
    }

    return CTC_E_NONE;
}

static void
_sys_humber_usrid_show_ipv4_default_entry(uint8 chip_id, uint8 label_id)
{
    int32 ret = 0;
    uint32 key_offset = 0;
    uint32 cmd = 0;
    uint8 lchip = 0;
    ds_user_id_ipv4_t ds_user_id_ipv4;

    ret = sys_humber_usrid_db_get_resv_offset(lchip, label_id, SYS_USRID_IPV4_KEY, &key_offset);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Default entry                       :none\n");
        SYS_USRID_DBG_INFO("========================================\n");
        return;
    }
    SYS_USRID_DBG_INFO("Default entry\n");

    cmd = DRV_IOR(IOC_TABLE, DS_USER_ID_IPV4, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, key_offset, cmd, &ds_user_id_ipv4);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Read associate data error\n");
        return;
    }
    SYS_USRID_DBG_INFO("Key offset                          :%d\n", key_offset);
    SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", ds_user_id_ipv4.by_pass_all);
    SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", ds_user_id_ipv4.user_vlan_ptr);

    if (ds_user_id_ipv4.binding_en)
    {
        SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", ds_user_id_ipv4.binding_en);
        if (ds_user_id_ipv4.binding_mac_sa)
        {
            SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                ds_user_id_ipv4.binding_datah,
                                ds_user_id_ipv4.binding_datam,
                                ds_user_id_ipv4.binding_datal);
        }
        else
        {
            SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                ds_user_id_ipv4.binding_datah,
                                ds_user_id_ipv4.binding_datam,
                                ds_user_id_ipv4.binding_datal);
        }

    }
    else if (ds_user_id_ipv4.ds_fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", ds_user_id_ipv4.binding_datal);
    }
    else if (ds_user_id_ipv4.binding_mac_sa)
    {
        SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", ds_user_id_ipv4.binding_datal);
    }
    else if (0xFFFF != ds_user_id_ipv4.binding_datal)
    {
        SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", ds_user_id_ipv4.binding_datal);
    }

    /*binding data high*/
    if (CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datah , 0x8000))
    {
        SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datah, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
        SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                         (ds_user_id_ipv4.binding_datah & 0x1FFF));
    }
    else if (!CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datah, 0x1000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                    (ds_user_id_ipv4.binding_datah & 0xFFF));

    }
    else if ((ds_user_id_ipv4.aps_select_valid)
    && (0xFFF !=(ds_user_id_ipv4.binding_datah & 0xFFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
        SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                            (ds_user_id_ipv4.binding_datah & 0xFFF));
    }

    if (ds_user_id_ipv4.aps_select_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
        SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                            ds_user_id_ipv4.binding_datah >> 13 & 0x1);
    }

    /*binding data middle*/
    if (CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datam , 0xc000))
    {
        SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datam, 0x8000)
    && (0x3FFF != (ds_user_id_ipv4.binding_datam & 0x3FFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
        SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                            (ds_user_id_ipv4.binding_datam & 0x3FFF));

    }
    else if (CTC_FLAG_ISSET(ds_user_id_ipv4.binding_datam, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
        SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                            (ds_user_id_ipv4.binding_datam & 0xFFF));
    }
    else if (0x3FFF != (ds_user_id_ipv4.binding_datam & 0x3FFF))
    {
        SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                             (ds_user_id_ipv4.binding_datam & 0x3FFF));

    }

    SYS_USRID_DBG_INFO("========================================\n");
    return;
}
int32
sys_humber_usrid_show_ipv4_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_ipv4_by_label(chip_id, label_id);
        _sys_humber_usrid_show_ipv4_default_entry(chip_id, label_id);
    }

    return CTC_E_NONE;
}


void
_sys_humber_usrid_show_ipv6_default_entry(uint8 chip_id, uint8 label_id)
{
    int32 ret = 0;
    uint32 key_offset = 0;
    uint32 cmd = 0;
    uint8 lchip = 0;
    ds_user_id_ipv6_t ds_user_id_ipv6;

    ret = sys_humber_usrid_db_get_resv_offset(lchip, label_id, SYS_USRID_IPV6_KEY, &key_offset);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Default entry                       :none\n");
        SYS_USRID_DBG_INFO("========================================\n");
        return;
    }
    SYS_USRID_DBG_INFO("Default entry\n");

    cmd = DRV_IOR(IOC_TABLE, DS_USER_ID_IPV6, DRV_ENTRY_FLAG);
    ret = drv_tbl_ioctl(lchip, key_offset, cmd, &ds_user_id_ipv6);
    if (ret < 0)
    {
        SYS_USRID_DBG_INFO("Read associate data error\n");
        return;
    }
    SYS_USRID_DBG_INFO("Key offset                          :%d\n", key_offset);
    SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", ds_user_id_ipv6.by_pass_all);
    SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", ds_user_id_ipv6.user_vlan_ptr);

    if (ds_user_id_ipv6.binding_en)
    {
        SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", ds_user_id_ipv6.binding_en);
        if (ds_user_id_ipv6.binding_mac_sa)
        {
            SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                ds_user_id_ipv6.binding_datah,
                                ds_user_id_ipv6.binding_datam,
                                ds_user_id_ipv6.binding_datal);
        }
        else
        {
            SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                ds_user_id_ipv6.binding_datah,
                                ds_user_id_ipv6.binding_datam,
                                ds_user_id_ipv6.binding_datal);
        }

    }
    else if (ds_user_id_ipv6.ds_fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", ds_user_id_ipv6.binding_datal);
    }
    else if (ds_user_id_ipv6.binding_mac_sa)
    {
        SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", ds_user_id_ipv6.binding_datal);
    }
    else if (0xFFFF != ds_user_id_ipv6.binding_datal)
    {
        SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", ds_user_id_ipv6.binding_datal);
    }

    /*binding data high*/
    if (CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datah , 0x8000))
    {
        SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datah, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
        SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                         (ds_user_id_ipv6.binding_datah & 0x1FFF));
    }
    else if (!CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datah, 0x1000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                    (ds_user_id_ipv6.binding_datah & 0xFFF));

    }
    else if ((ds_user_id_ipv6.aps_select_valid)
    && (0xFFF !=(ds_user_id_ipv6.binding_datah & 0xFFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
        SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                            (ds_user_id_ipv6.binding_datah & 0xFFF));
    }

    if (ds_user_id_ipv6.aps_select_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
        SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                            ds_user_id_ipv6.binding_datah >> 13 & 0x1);
    }

    /*binding data middle*/
    if (CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datam , 0xc000))
    {
        SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
    }

    if (CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datam, 0x8000)
    && (0x3FFF != (ds_user_id_ipv6.binding_datam & 0x3FFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
        SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                            (ds_user_id_ipv6.binding_datam & 0x3FFF));

    }
    else if (CTC_FLAG_ISSET(ds_user_id_ipv6.binding_datam, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
        SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                            (ds_user_id_ipv6.binding_datam & 0xFFF));
    }
    else if (0x3FFF != (ds_user_id_ipv6.binding_datam & 0x3FFF))
    {
        SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                             (ds_user_id_ipv6.binding_datam & 0x3FFF));

    }

    SYS_USRID_DBG_INFO("========================================\n");
    return;

}

int32
sys_humber_usrid_show_ipv6_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }

    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_ipv6_by_label(chip_id, label_id);
        _sys_humber_usrid_show_ipv6_default_entry(chip_id, label_id);
    }

    return CTC_E_NONE;
}

int32
sys_humber_usrid_show_vlan_count_by_port(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_vlan_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    CTC_GLOBAL_PORT_CHECK(gport);

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        sys_humber_usrid_db_show_vlan_count_by_port(0xFF, gport);
    }
    else
    {
        SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
        sys_humber_usrid_db_show_vlan_count_by_port(lchip, gport);
    }

    return CTC_E_NONE;
}

int32
sys_humber_usrid_show_mac_count_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_mac_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_mac_count_by_label(chip_id, label_id);
    }

    return CTC_E_NONE;
}

int32
sys_humber_usrid_show_ipv4_count_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv4_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_ipv4_count_by_label(chip_id, label_id);
    }

    return CTC_E_NONE;
}

int32
sys_humber_usrid_show_ipv6_count_by_label(uint8 label_id)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;

    if ((NULL == p_mutex) || (NULL == p_mutex->usrid_ipv6_mutex))
    {
        return CTC_E_NOT_INIT;
    }
    SYS_USRID_LABEL_CHECK(label_id);

    lchip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        sys_humber_usrid_db_show_ipv6_count_by_label(chip_id, label_id);
    }

    return CTC_E_NONE;
}

void sys_humber_vlan_range_calc_vlanmask(uint16  usVlanStart, uint16  usVlanEnd, uint16  usMaskBits,
                               uint16 *usValue, uint16 *usMask)
{
    *usMask = SYS_VLAN_MASK_ALL;

    if (usVlanStart == usVlanEnd)
    {
        *usValue = usVlanStart;
        *usMask = SYS_VLAN_MASK_ALL;
        return;
    }

    if ((SYS_VLAN_MIN == usVlanStart) && (SYS_VLAN_MAX == usVlanEnd))
    {
        *usMask  = 0;
        *usValue = 0;
    }
    else if (usVlanStart == SYS_VLAN_MIN)
    {
        *usMask  = (uint16)(((uint32)(*usMask) >> usMaskBits) << usMaskBits);
        *usValue = 0;
    }
    else
    {
        *usMask  = (uint16)(((uint32)(*usMask) >> usMaskBits) << usMaskBits);
        *usValue = usVlanStart;
    }

    if(0 == *usValue)
    {
        *usValue = 1;
    }
    return;
}

void sys_humber_vlan_range_free_node(sys_humber_vlan_link_t *head)
{
    sys_humber_vlan_link_t *pTmp  = head ? head->pstNodeNext : NULL;
     sys_humber_vlan_link_t *pTmp1 = NULL;
    while (NULL != pTmp)
    {
        pTmp1 = pTmp->pstNodeNext;
        mem_free(pTmp);
	pTmp = pTmp1;
    }
    head->pstNodeNext = NULL;
}
uint32 sys_humber_vlan_range_insert_node(sys_humber_vlan_link_t** head, uint16 usVlanStart,
                                  uint16 usVlanEnd, uint16 usMaskBits,
                                  uint8 type)
{
    sys_humber_vlan_link_t* pstTmp     = *head;
    sys_humber_vlan_link_t* pstTmp1    = NULL;
    sys_humber_vlan_link_t* pstNewNode = NULL;
    uint32             i = 0;

    while (pstTmp != 0)
    {
        if ((usVlanEnd > pstTmp->usVlanEnd))
        {
            pstTmp1 = pstTmp;
            pstTmp = pstTmp->pstNodeNext;
        }
        else
        {
            if (pstTmp == (*head))
            {
                if (SYS_VLANNODE_SINGLE == type)
                {
                    for (i = usVlanStart; i <= usVlanEnd; i++)
                    {
                        if (i == usVlanStart)
                        {
                            pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                            if (NULL == pstNewNode)
                            {

                                return CTC_E_NO_MEMORY;
                            }

                            pstNewNode->usVlanStart = pstNewNode->usVlanEnd = (uint16)i;
                            pstNewNode->usMaskBits = usMaskBits;
                            sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd,
                                                  pstNewNode->usMaskBits,  &(pstNewNode->usValue), &(pstNewNode->usMask));
                            pstNewNode->pstNodeNext = (*head);
                            (*head) = pstNewNode;
                            pstTmp = pstNewNode;
                        }
                        else
                        {
                            pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                            if (NULL == pstNewNode)
                            {
                                return CTC_E_NO_MEMORY;
                            }

                            pstNewNode->usVlanStart = pstNewNode->usVlanEnd = (uint16)i;
                            pstNewNode->usMaskBits = usMaskBits;

                            sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd,
                                                  pstNewNode->usMaskBits,
                                                  &(pstNewNode->usValue), &(pstNewNode->usMask));

                            pstNewNode->pstNodeNext = pstTmp->pstNodeNext; /* New node is after the inserted-node*/
                            pstTmp->pstNodeNext = pstNewNode;
                            pstTmp = pstNewNode;
                        }
                    }
                }
                else  /* insert range vlan*/
                {
                    pstNewNode = (sys_humber_vlan_link_t*)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                    if (NULL == pstNewNode)
                    {
                        return CTC_E_NO_MEMORY;
                    }

                    pstNewNode->usVlanStart = usVlanStart;
                    pstNewNode->usVlanEnd  = usVlanEnd;
                    pstNewNode->usMaskBits = usMaskBits;

                    sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd, pstNewNode->usMaskBits,
                                          &(pstNewNode->usValue), &(pstNewNode->usMask));

                    pstNewNode->pstNodeNext = (*head);
                    (*head) = pstNewNode;
                }
            }
            else  /* insert middle node*/
            {
                if (SYS_VLANNODE_SINGLE == type)
                {
                    for (i = usVlanStart; i <= usVlanEnd; i++)
                    {
                        pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                        if (NULL == pstNewNode)
                        {
                            return CTC_E_NO_MEMORY;
                        }

                        if (NULL != pstTmp1)
                        {
                            pstNewNode->usVlanStart = pstNewNode->usVlanEnd = (uint16)i;
                            pstNewNode->pstNodeNext = pstTmp1->pstNodeNext;
                            pstNewNode->usMaskBits = usMaskBits;
                            sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd,
                                                  pstNewNode->usMaskBits,
                                                  &(pstNewNode->usValue), &(pstNewNode->usMask));

                            pstTmp1->pstNodeNext = pstNewNode;
                            pstTmp1 = pstNewNode;
                        }
                    }
                }
                else
                {
                    pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                    if (NULL == pstNewNode)
                    {
                        return CTC_E_NO_MEMORY;
                    }

                    pstNewNode->usVlanStart = usVlanStart;
                    pstNewNode->usVlanEnd  = usVlanEnd;
                    pstNewNode->usMaskBits = usMaskBits;
                    sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd, pstNewNode->usMaskBits,
                                          &(pstNewNode->usValue), &(pstNewNode->usMask));

                    if (NULL != pstTmp1)
                    {
                        pstNewNode->pstNodeNext = pstTmp1->pstNodeNext;
                        pstTmp1->pstNodeNext = pstNewNode;
                        pstTmp1 = pstNewNode;
                    }
                }
            }

            return CTC_E_NONE;   /* Stop after insert operation*/
        }
    }

    /*insert last node*/
    if (SYS_VLANNODE_SINGLE == type) /* insert step by step*/
    {
        if (0 == (*head)) /*linklist is empty*/
        {
            for (i = usVlanStart; i <= usVlanEnd; i++)
            {
                if (i == usVlanStart) /* fisrt node need modifcation*/
                {
                    *head = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                    if (NULL == *head)
                    {
                        return CTC_E_NO_MEMORY;
                    }

                    (*head)->usVlanStart = (*head)->usVlanEnd = (uint16)i;
                    (*head)->pstNodeNext = NULL;
                    (*head)->usMaskBits = usMaskBits;
                    sys_humber_vlan_range_calc_vlanmask((*head)->usVlanStart, (*head)->usVlanEnd, (*head)->usMaskBits,
                                          &((*head)->usValue), &((*head)->usMask));

                    pstTmp1 = *head;
                }
                else
                {
                    pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                    if (NULL == pstNewNode)
                    {
                        return CTC_E_NO_MEMORY;
                    }

                    pstNewNode->usVlanStart = pstNewNode->usVlanEnd = (uint16)i;
                    pstNewNode->usMaskBits = usMaskBits;
                    sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd, pstNewNode->usMaskBits,
                                          &(pstNewNode->usValue), &(pstNewNode->usMask));

                    pstNewNode->pstNodeNext = NULL;
                    if (NULL != pstTmp1)/*for pc-lint*/
                    {
                        pstTmp1->pstNodeNext = pstNewNode;
                        pstTmp1 = pstNewNode;
                    }
                }
            }
        }
        else  /* insert to the lastest when link is not empty*/
        {
            for (i = usVlanStart; i <= usVlanEnd; i++)
            {
                pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
                if (NULL == pstNewNode)
                {

                    return CTC_E_NO_MEMORY;
                }

                pstNewNode->usVlanStart = pstNewNode->usVlanEnd = (uint16)i;
                pstNewNode->usMaskBits = usMaskBits;
                sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd, pstNewNode->usMaskBits,
                                      &(pstNewNode->usValue), &(pstNewNode->usMask));

                pstNewNode->pstNodeNext = NULL;
                if (NULL != pstTmp1)/*for pc-lint*/
                {
                    pstTmp1->pstNodeNext = pstNewNode;
                    pstTmp1 = pstNewNode;
                }
            }
        }
    }
    else
    {
        if (NULL == (*head)) /*linklist is empty*/
        {
            *head = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
            if (NULL == *head)
            {

                return CTC_E_NO_MEMORY;
            }

            (*head)->usVlanStart = usVlanStart;
            (*head)->usVlanEnd   = usVlanEnd;
            (*head)->pstNodeNext = 0;
            (*head)->usMaskBits  = usMaskBits;
            sys_humber_vlan_range_calc_vlanmask((*head)->usVlanStart, (*head)->usVlanEnd, (*head)->usMaskBits,
                                  &((*head)->usValue), &((*head)->usMask));
        }
        else
        {
            pstNewNode = (sys_humber_vlan_link_t *)mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));
            if (NULL == pstNewNode)
            {

                return CTC_E_NO_MEMORY;
            }

            pstNewNode->usVlanStart = usVlanStart;
            pstNewNode->usVlanEnd  = usVlanEnd;
            pstNewNode->usMaskBits = usMaskBits;
            sys_humber_vlan_range_calc_vlanmask(pstNewNode->usVlanStart, pstNewNode->usVlanEnd, pstNewNode->usMaskBits,
                                  &(pstNewNode->usValue), &(pstNewNode->usMask));

            pstNewNode->pstNodeNext = NULL;
            if (NULL != pstTmp1) /*for pc-lint*/
            {
                pstTmp1->pstNodeNext = pstNewNode;
                pstTmp1 = pstNewNode;
            }
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_range_split_vlan(uint16 usVlanStart, uint16 usVlanEnd, sys_humber_vlan_link_t** head, uint32 *sum)
{
    int32 n = 0;
    int32 i = 0;
    int32 j = 0;

    bool    startvalid = TRUE;
    bool    endvalid   = TRUE;
    uint16      usVlanStart1 = 0;
    uint16      usVlanEnd1   = 0;
    uint32       ulTmp        = 1;
    uint32       ulRet       = CTC_E_NONE;
    uint32       a[SYS_VLAN_SPLIT_MAX];  /*store n: n meaning 2^n*/

    if ((usVlanEnd < SYS_VLAN_MIN) || (usVlanEnd > SYS_VLAN_MAX)) /*for pc-lint*/
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(a, 0,SYS_VLAN_SPLIT_MAX * sizeof(uint16));
    if (usVlanStart == usVlanEnd) /* Case: not range vlan*/
    {
        ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd, 0, SYS_VLANNODE_SINGLE);
        (*sum)++;
        return ulRet;
    }

    if ((SYS_VLAN_MIN == usVlanStart) && (SYS_VLAN_MAX == usVlanEnd)) /* Case: vlan range 1-4094 */
    {
        ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd, 12, SYS_VLANNODE_MULTI);
        (*sum)++;
        return ulRet;
    }

    while (ulTmp <= usVlanEnd)  /*find the max number*/
    {
        ulTmp = ulTmp << 1;
        n++;
    }

    n = n - 1;
    ulTmp /= 2;

    if (n <= 0) /*for pc-lint*/
    {
        return CTC_E_NO_MEMORY;
    }

    for (i = n - 1; i >= 0; i--) /*Caculate all the number littler than Max number*/
    {
        j = i;
        a[i] = 1;
        while (j >= 0)
        {
            a[i] = (uint16)(a[i] << 1);
            j--;
        }
    }

    if ((usVlanStart == 1) && (usVlanEnd == (a[n - 1] * 2 - 1)))  /*Mask = Normal(mask) +1, for 1 to 2^n-1*/
    {
        ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd, (uint16)(n + 1), SYS_VLANNODE_MULTI);
        (*sum)++;
        return ulRet;
    }

    if (usVlanStart <= ulTmp) /*Closer to two end from ulTmp when ulTmp between usVlanStart and usVlanEnd */
    {
        usVlanStart1 = (uint16)ulTmp;
        usVlanEnd1 = (uint16)(ulTmp - 1);
    }
    else /*ulTmp littler than usVlanStart*/
    {
        for (i = n - 1; i >= 0; i--)
        {
            if ((usVlanEnd / a[i]) > (usVlanStart / a[i]))
            {
                usVlanStart1 = (uint16)((usVlanEnd / a[i]) * a[i]);
                usVlanEnd1 = (uint16)((usVlanEnd / a[i]) * a[i] - 1);
                break;
            }
        }

        if (i < 0)/*Not found (n, n+1)*/
        {
            if ((0 == usVlanStart % 2) && (usVlanEnd == (usVlanStart + 1))) /*First vlan is even (2*x) */
            {
                ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd, 1, SYS_VLANNODE_MULTI);
                (*sum)++;
            }
            else
            {
                ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd, 0, SYS_VLANNODE_SINGLE);
                (*sum)++;
            }

            return ulRet;
        }
    }

    for (i = n - 1; i >= 0; i--)
    {
        if (TRUE == endvalid) /*find the after*/
        {
            if (((usVlanStart1 + a[i] - 1) <= usVlanEnd))
            {
                ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart1, (uint16)(usVlanStart1 + a[i] - 1), (uint16)(i + 1),
                                                SYS_VLANNODE_MULTI);
                if (CTC_E_NONE != ulRet)
                {
                    return ulRet;
                }
                (*sum)++;

                if (usVlanEnd == usVlanStart1 + a[i] - 1) /*Finsh the find*/
                {
                    endvalid = FALSE;
                }
                else
                {
                    usVlanStart1 = (uint16)(usVlanStart1 + a[i]);
                }
            }
        }

        if (TRUE == startvalid)/*find the befor*/
        {
            if (usVlanStart == 1) /*special for start vlan 1*/
            {
                if (0 == usVlanEnd1 - (a[i] - 1))/*Locate at begin*/
                {
                    if (1 == usVlanEnd1) /*vlan is 1*/
                    {
                        ulRet = sys_humber_vlan_range_insert_node(head, 1, usVlanEnd1, 0, SYS_VLANNODE_SINGLE);
                        if (CTC_E_NONE != ulRet)
                        {
                            return ulRet;
                        }
                        (*sum)++;
                    }
                    else
                    {
                        ulRet = sys_humber_vlan_range_insert_node(head, 1, usVlanEnd1, (uint16)(i + 1), SYS_VLANNODE_MULTI); /*插入1到end1之间的*/
                        if (CTC_E_NONE != ulRet)
                        {
                            return ulRet;
                        }
                        (*sum)++;
                    }

                    startvalid = FALSE;
                }
                else /*Not the begin*/
                {
                    ulRet = sys_humber_vlan_range_insert_node(head, (uint16)(usVlanEnd1 - (a[i] - 1)), usVlanEnd1, (uint16)(i + 1),
                                                    SYS_VLANNODE_MULTI);
                    if (CTC_E_NONE != ulRet)
                    {
                        return ulRet;
                    }
                    (*sum)++;

                    usVlanEnd1 = (uint16)(usVlanEnd1 - a[i]);
                }
            }
            else /*start vlan is not 1*/
            {
                if (usVlanEnd1 >= (a[i] - 1))
                {
                    if ((usVlanEnd1 - (a[i] - 1)) >= usVlanStart)
                    {
                        ulRet = sys_humber_vlan_range_insert_node(head, (uint16)(usVlanEnd1 - (a[i] - 1)), usVlanEnd1,
                                                        (uint16)(i + 1), SYS_VLANNODE_MULTI);
                        if (CTC_E_NONE != ulRet)
                        {
                            return ulRet;
                        }
                        (*sum)++;

                        if (usVlanStart == usVlanEnd1 - (a[i] - 1))
                        {
                            startvalid = FALSE;
                        }
                        else
                        {
                            usVlanEnd1 = (uint16)(usVlanEnd1 - a[i]);
                        }
                    }
                }
            }
        }
    }

    if (TRUE == startvalid)
    {
        if (usVlanEnd1 >= usVlanStart) /**/
        {
            if ((usVlanEnd1 - usVlanStart) >= 0)
            {
                ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart, usVlanEnd1, 0, SYS_VLANNODE_SINGLE);
                if (CTC_E_NONE != ulRet)
                {
                    return ulRet;
                }
                (*sum)++;
            }
        }
    }

    if (TRUE == endvalid)
    {
        if ((usVlanEnd - usVlanStart1) >= 0)
        {
            ulRet = sys_humber_vlan_range_insert_node(head, usVlanStart1, usVlanEnd, 0, SYS_VLANNODE_SINGLE);
            if (CTC_E_NONE != ulRet)
            {
                return ulRet;
            }
            (*sum)++;
        }
    }

    return CTC_E_NONE;
}


int32
sys_humber_test_vlan_range(uint16 VlanStart, uint16 VlanEnd)
{
    uint32 sum;
    sys_humber_vlan_link_t *p_head = NULL;
    sys_humber_vlan_link_t *p_tmp = NULL;

    p_head = mem_malloc(MEM_USRID_MODULE, sizeof(sys_humber_vlan_link_t));

    if (NULL == p_head)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

    sys_humber_vlan_range_split_vlan(VlanStart, VlanEnd, &p_head, &sum);

    printf("The sum value is:   %d\r\n", sum);


        printf("usVlanStart     :%d\n\r", p_head->usVlanStart);
        printf("usVlanEnd       :%d\n\r", p_head->usVlanEnd);
        printf("usMaskBits      :%d\n\r", p_head->usMaskBits);
        printf("usValue         :%d\n\r", p_head->usValue);
        printf("usMask          :%d\n\r", p_head->usMask);

    for(p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
    {
        printf("usVlanStart     :%d\n\r", p_tmp->usVlanStart);
        printf("usVlanEnd       :%d\n\r", p_tmp->usVlanEnd);
        printf("usMaskBits      :%d\n\r", p_tmp->usMaskBits);
        printf("usValue         :%d\n\r", p_tmp->usValue);
        printf("usMask          :%d\n\r", p_tmp->usMask);
    }
    return CTC_E_NONE;
}



