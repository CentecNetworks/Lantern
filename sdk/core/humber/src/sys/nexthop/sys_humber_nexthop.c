/**
 @file sys_humber_nexthop.c

 @date 2009-09-16

 @version v2.0

 The file contains all nexthop module core logic
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_common.h"
#include "ctc_linklist.h"
#include "ctc_cpu_traffic.h"
#include "ctc_aps.h"

#include "sys_humber_opf.h"
#include "sys_humber_chip.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_api.h"
#include "sys_humber_ftm.h"
#include "sys_humber_register.h"
#include "sys_humber_cpu_traffic.h"
#include "sys_humber_stats.h"

#include "sys_humber_vlan.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_l3if.h"

#include "drv_enum.h"
#include "drv_io.h"
#include "drv_humber.h"
#include "drv_tbl_reg.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define SYS_HUMBER_CRSCNT_MAX_MMEMBER_NUM  256
#define SYS_HUMBER_INVALID_STATS_PTR 0xFFFF

#define SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(_dsnh)    \
    {                                                      \
        _dsnh.replace_ctag_cos = 0;                         \
        _dsnh.copy_ctag_cos = 0;                            \
        _dsnh.derive_stag_cos = 1;                          \
        _dsnh.stag_cfi = 1;                         \
    }

#define SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(_dsnh, _dest_vlan_ptr, _pld_op, \
                                            _l2rw_type, _l3rw_type)         \
    {                                                                    \
        _dsnh.dest_vlan_ptr = _dest_vlan_ptr;                               \
        _dsnh.payload_operation = _pld_op;                                  \
        _dsnh.l2_rewrite_type = _l2rw_type;                                 \
        _dsnh.l3_rewrite_type = _l3rw_type;                                 \
    }

#define SYS_HUMBER_NH_DSNH8W_BUILD_L2EDITPTR_L3EDITPTR(__dsnh,      \
                            __dsnh_l2editptr, __dsnh_l3editptr)     \
    {                                                            \
        __dsnh.l2edit_ptr11to0 = __dsnh_l2editptr & 0xFFF;          \
        __dsnh.l2edit_ptr18to12 = (__dsnh_l2editptr>> 12) & 0x7F;   \
        __dsnh.l3edit_ptr170 = __dsnh_l3editptr & 0x3FFFF;          \
        __dsnh.l3edit_ptr18 = ((__dsnh_l3editptr >> 18) & 0x1);     \
        __dsnh.l3edit_ptr19 = ((__dsnh_l3editptr >> 19) & 0x1);     \
    }

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/

static sys_humber_nh_master_t* p_sys_humber_nh_master = NULL;
static sys_nh_table_entry_info_t nh_table_info_array[] =
{
        /*SYS_NH_ENTRY_TYPE_FWD*/
        {
        DS_FWD,
        1,
        LOCAL_MET_DSFWD_SRAM,
        1,
        } ,
        /*SYS_NH_ENTRY_TYPE_MET*/
        {
        DS_MET_ENTRY,
        1,
        LOCAL_MET_DSFWD_SRAM,
        0,
        } ,
        /*SYS_NH_ENTRY_TYPE_NEXTHOP_4W*/
        {
        DS_NEXTHOP,
        1,
        LOCAL_NEXTHOP_SRAM,
        0,
        },
        /*SYS_NH_ENTRY_TYPE_NEXTHOP_8W*/
        {
        DS_NEXTHOP8W,
        2,
        LOCAL_NEXTHOP_SRAM,
        1,
        } ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W*/
        {
        DS_L2_EDIT_ETH4W,
        1,
        L2EDIT_SRAM,
        0,
        } ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W*/
        {
        DS_L2_EDIT_ETH8W,
        2,
        L2EDIT_SRAM,
        1,
        }  ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_FLEX_4W*/
        {
        DS_L2_EDIT_FLEX4W,
        1,
        L2EDIT_SRAM,
        0,
        }  ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_FLEX_8W*/
        {
        DS_L2_EDIT_FLEX8W,
        2,
        L2EDIT_SRAM,
        1,
        }  ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_PBB_4W*/
        {
        DS_L2_EDIT_PBB4W,
        1,
        L2EDIT_SRAM,
        0,
        }  ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_PBB_8W*/
        {
        DS_L2_EDIT_PBB8W,
        2,
        L2EDIT_SRAM,
        1,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L2EDIT_LPBK*/
        {
        DS_L2_EDIT_LOOPBACK,
        1,
        L2EDIT_SRAM,
        0,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_NAT_4W*/
        {
        DS_L3EDIT_NAT4W,
        1,
        L3EDIT_SRAM,
        0,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_NAT_8W*/
        {
        DS_L3EDIT_NAT8W,
        2,
        L3EDIT_SRAM,
        1,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_4W*/
        {
        DS_L3EDIT_MPLS4W,
        1,
        L3EDIT_SRAM,
        0,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_MPLS_8W*/
        {
        DS_L3EDIT_MPLS8W,
        2,
        L3EDIT_SRAM,
        1,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4*/
        {
        DS_L3EDIT_TUNNEL_V4,
        2,
        L3EDIT_SRAM,
        1,
        }
        ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6*/
        {
        DS_L3EDIT_TUNNEL_V6,
        2,
        L3EDIT_SRAM,
        1,
        }  ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_FLEX*/
        {
        DS_L3EDIT_FLEX,
        1,
        L3EDIT_SRAM,
        0,
        } ,
        /*SYS_NH_ENTRY_TYPE_L3EDIT_LPBK*/
        {
        DS_L3EDIT_LOOP_BACK,
        1,
        L3EDIT_SRAM,
        0,
        }                                                                      ,
}
                                                                                ;

static int32 _sys_humber_nh_offset_init();
static int32 _sys_humber_nh_db_dsl2editeth4w_cmp(void* p_data_new, void* p_data_old);
extern int32 _sys_humber_nh_build_dsnh4w_edit_ptr(ds_nexthop_t *dsnh,  sys_nh_param_dsnh_t *p_dsnh_param);

/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief This function is to initialize avl tree in nexthop module,
        this avl is used to maintains shared entry

 @param[in] p_master, defined in sys_humber_nexthop.h

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_avl_init(sys_humber_nh_master_t* p_master)
{
    int32 ret;
    SYS_NH_DBG_FUNC();

    ret = ctc_avl_create (&p_master->dsl2edit4w_tree, 0, _sys_humber_nh_db_dsl2editeth4w_cmp);
    if(ret)
        return CTC_E_CANT_CREATE_AVL;

    return CTC_E_NONE;
}

/**
 @brief This function is used to initilize internal nexthop id pool

 @param[out] pp_nhid_vec, inernal nexthop id vector

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_external_id_init(ctc_vector_t** pp_nhid_vec,uint32 max_external_nhid)
{
    ctc_vector_t *p_nhid_vec;
    uint32 block_size = 0;

    SYS_NH_DBG_FUNC();
    /*1. Creat vector, use vector to store external nh_info*/
    block_size = max_external_nhid / SYS_HUMBER_NH_EXTERNAL_NHID_MAX_BLK_NUM;
    p_nhid_vec = ctc_vector_init(SYS_HUMBER_NH_EXTERNAL_NHID_MAX_BLK_NUM, block_size);
    if(NULL == p_nhid_vec)
        return CTC_E_NO_MEMORY;

    *pp_nhid_vec = p_nhid_vec;

    return CTC_E_NONE;
};


/**
 @brief This function is used to initilize internal nexthop id pool

 @param[out] pp_nhid_vec, inernal nexthop id vector

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_internal_id_init(ctc_vector_t** pp_nhid_vec,uint32 start_internal_nhid)
{
    ctc_vector_t *p_nhid_vec;
    sys_humber_opf_t opf;
    uint32 nh_id_num;

    SYS_NH_DBG_FUNC();
    /*1. Creat vector, use vector to store internal nh_info*/
    p_nhid_vec = ctc_vector_init(SYS_HUMBER_NH_INTERNAL_NHID_MAX_BLK_NUM,
        SYS_HUMBER_NH_INTERNAL_NHID_BLK_SIZE);
    if(NULL == p_nhid_vec)
        return CTC_E_NO_MEMORY;

    *pp_nhid_vec = p_nhid_vec;

    /*2. Init nhid pool, use opf to alloc/free internal nhid*/
    CTC_ERROR_RETURN(sys_humber_opf_init(NHID_INTERNAL, 1));
    opf.pool_type = NHID_INTERNAL;
    opf.pool_index = 0;
    nh_id_num = (SYS_HUMBER_NH_INTERNAL_NHID_MAX_BLK_NUM *\
        SYS_HUMBER_NH_INTERNAL_NHID_BLK_SIZE);
    CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf,
        start_internal_nhid, nh_id_num));

    return CTC_E_NONE;
};

/**
 @brief This function is used to create and init nexthop module master data

 @param[in] p_init_data, defined in sys_humber_nexthop.h,
            data used to initialize the master data

 @param[out] pp_nexthop_master, defined in sys_humber_nexthop.h

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_master_new(sys_humber_nh_master_t** pp_nexthop_master,uint32 boundary_of_extnl_intnl_nhid)
{
    sys_humber_nh_master_t *p_master;
    uint8 lchip = 0;
    SYS_NH_DBG_FUNC();
    if(NULL == pp_nexthop_master || (NULL != *pp_nexthop_master))
        return CTC_E_INVALID_PTR;

    /*1. allocate memory for nexthop master*/
    p_master = mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_humber_nh_master_t));
    if(NULL == p_master )
        return CTC_E_NO_MEMORY;

    kal_memset(p_master, 0, sizeof(sys_humber_nh_master_t));

    p_master->glb_alloc_nexhop = 1;
    p_master->remote_met_use_nhp = 1;
    p_master->use_epe_parser_ttl_in_ipmc = 1;
    /*2. AVL tree init*/
    CTC_ERROR_RETURN(_sys_humber_nh_avl_init(p_master));

    /*3. Create Mutex*/
    SYS_NH_CREAT_LOCK(p_master->p_mutex);

     /*4. Nexthop external vector init*/
    CTC_ERROR_RETURN(_sys_humber_nh_external_id_init((&p_master->external_nhid_vec), boundary_of_extnl_intnl_nhid));

    /*5. Nexthop internal vector init*/
    CTC_ERROR_RETURN(_sys_humber_nh_internal_id_init(&(p_master->internal_nhid_vec),boundary_of_extnl_intnl_nhid));

    /*6. get dsnexthop offset by vlan_id*/
    for (lchip = 0; lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip++)
    {
        p_master->p_dsnh_offset_by_vlan[lchip] = mem_malloc(MEM_NEXTHOP_MODULE, CTC_MAX_VLAN_ID*sizeof(uint32));
        if (NULL == p_master->p_dsnh_offset_by_vlan[lchip])
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_master->p_dsnh_offset_by_vlan[lchip], 0xFF, CTC_MAX_VLAN_ID*sizeof(uint32));
    }
    /*Return PTR*/
    *pp_nexthop_master = p_master;

    return CTC_E_NONE;
}

/**
 @brief This function is register nexthop module callback function

 @param[in] nh_param_type, nexthop type

 @param[in] nh_create_cb, callback function used to create nexthop

 @param[in] nh_del_cb, callback function used to delete nexthop

 @param[in] nh_update_cb, callback function used to update nexthop

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_register_callback(sys_humber_nh_master_t* p_master, uint16 nh_param_type,
    p_sys_nh_create_cb_t nh_create_cb, p_sys_nh_delete_cb_t nh_del_cb,
    p_sys_nh_update_cb_t nh_update_cb)
{
    p_master->callbacks_nh_create[nh_param_type] = nh_create_cb;
    p_master->callbacks_nh_delete[nh_param_type] = nh_del_cb;
    p_master->callbacks_nh_update[nh_param_type] = nh_update_cb;
    return CTC_E_NONE;
};


/**
 @brief This function is initialize nexthop module
        callback function for each nexthop type

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_callback_init(sys_humber_nh_master_t* p_master)
{
    SYS_NH_DBG_FUNC();

    /* 1. Ucast bridge */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_BRGUC,
            &sys_humber_nh_create_brguc_cb,
            &sys_humber_nh_delete_brguc_cb, NULL));

    /* 2. Mcast bridge */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_MCAST,
            &sys_humber_nh_create_mcast_cb,
            &sys_humber_nh_delete_mcast_cb,
            &sys_humber_nh_update_mcast_cb));

    /* 3. IPUC */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_IPUC,
            &sys_humber_nh_create_ipuc_cb,
            &sys_humber_nh_delete_ipuc_cb,
            &sys_humber_nh_update_ipuc_cb));

    /* 4. ECMP */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_ECMP,
            &sys_humber_nh_create_ecmp_cb,
            &sys_humber_nh_delete_ecmp_cb,
            &sys_humber_nh_update_ecmp_cb));

    /* 5. MPLS */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_MPLS,
            &sys_humber_nh_create_mpls_cb,
            &sys_humber_nh_delete_mpls_cb,
            &sys_humber_nh_update_mpls_cb));

    /* 6. Drop */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_DROP,
            &sys_humber_nh_create_special_cb,
            &sys_humber_nh_delete_special_cb, NULL));

    /* 7. ToCPU */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_TOCPU,
            &sys_humber_nh_create_special_cb,
            &sys_humber_nh_delete_special_cb, NULL));

    /* 8. Unresolve */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_UNROV,
            &sys_humber_nh_create_special_cb,
            &sys_humber_nh_delete_special_cb, NULL));

    /* 9. ILoop */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_ILOOP,
            &sys_humber_nh_create_iloop_cb,
            &sys_humber_nh_delete_iloop_cb,
            &sys_humber_nh_update_iloop_cb));

    /* 10. rspan */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_RSPAN,
            &sys_humber_nh_create_rspan_cb,
            &sys_humber_nh_delete_rspan_cb,
            NULL));
    /* 11. oam /downmep */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_DOWNMEP,
            &sys_humber_nh_create_downmep_cb,
            &sys_humber_nh_delete_downmep_cb, NULL));

    /* 12. ip-tunnel */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_IP_TUNNEL,
            &sys_humber_nh_create_ip_tunnel_cb,
            &sys_humber_nh_delete_ip_tunnel_cb,
            &sys_humber_nh_update_ip_tunnel_cb));

    /* 13. flexible */
    CTC_ERROR_RETURN(_sys_humber_nh_register_callback(p_master,
            SYS_HUMBER_NH_TYPE_FLEX,
            &sys_humber_nh_create_flex_cb,
            &sys_humber_nh_delete_flex_cb,
            NULL));

    return CTC_E_NONE;
};

/**
 @brief This function is used to get nexthop module master data

 @param[out] p_nh_master, nexthop module master data

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_get_nh_master(sys_humber_nh_master_t** p_nh_master)
{
    if(NULL == p_sys_humber_nh_master)
        return CTC_E_NH_NOT_INIT;
    *p_nh_master = p_sys_humber_nh_master;
    return CTC_E_NONE;
}


/**
 @brief This function is deinit nexthop module
        callback function for each nexthop type

 @return CTC_E_XXX
 */
int32
sys_humber_nh_global_dync_entry_set_default(uint8 lchip, uint32 min_offset, uint32 max_offset)
{
    uint32 offset, met_cmd;
    ds_met_entry_t dsmet;

    SYS_NH_DBG_FUNC();

    /* assign default value for met entry. the default value is discarding packets */
    kal_memset(&dsmet, 0, sizeof(ds_met_entry_t));
    dsmet.next_met_entry_ptr = SYS_HUMBER_NH_MET_END_REPLICATE_OFFSET;
    dsmet.end_local_rep = SYS_HUMBER_NH_BOOL_TRUE;
    dsmet.ucast_id_lower = SYS_HUMBER_NH_MET_DROP_UCAST_ID_LOWER;
    dsmet.ucast_id_upper = SYS_HUMBER_NH_MET_DROP_UCAST_ID_UPPER;
    met_cmd = DRV_IOW(IOC_TABLE, DS_MET_ENTRY, DRV_ENTRY_FLAG);

    for (offset = min_offset; offset <= max_offset; offset++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, met_cmd, &dsmet));
    }

    return CTC_E_NONE;
};

/**
 @brief This function is used to init resolved dsnexthop for bridge

 */
static INLINE int32
_sys_humber_nh_dsnh_init_for_brg(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    ds_nexthop_t dsnh;

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    p_offset_attr->offset_base = ((SYS_HUMBER_DSNH_INTERNAL_BASE << \
        SYS_HUMBER_DSNH_INTERNAL_SHIFT) |
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_BRG);
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    cmd = DRV_IOW(IOC_TABLE, EPE_NEXT_HOP_INTERNAL4W, DRV_ENTRY_FLAG);
    SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);
    dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
    dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
    dsnh.payload_operation = SYS_NH_OP_BRIDGE;
    dsnh.dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
    dsnh.replace_dscp = 1;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_BRG, cmd, &dsnh));

    return CTC_E_NONE;
}
/**
 @brief This function is used to init resolved dsnexthop for port unatgged  mode bridge(apply for hybrid port  unatgged mode)

 */
static INLINE int32
_sys_humber_nh_dsnh_init_for_untagged_brg(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    ds_nexthop_t dsnh;

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    p_offset_attr->offset_base = ((SYS_HUMBER_DSNH_INTERNAL_BASE << \
        SYS_HUMBER_DSNH_INTERNAL_SHIFT) |
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_UNTAGGED_BRG);
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    cmd = DRV_IOW(IOC_TABLE, EPE_NEXT_HOP_INTERNAL4W, DRV_ENTRY_FLAG);
    SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);
    dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE ;
    dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_FALSE;
    dsnh.payload_operation = SYS_NH_OP_BRIDGE;
    dsnh.dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
    dsnh.replace_dscp = 1;

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_UNTAGGED_BRG, cmd, &dsnh));

    return CTC_E_NONE;
}

/**
 @brief This function is used to init resolved dsnexthop for egress bypass
 */
static INLINE int32
_sys_humber_nh_dsnh_init_for_bypass(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    ds_nexthop_t dsnh;

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    p_offset_attr->offset_base = ((SYS_HUMBER_DSNH_INTERNAL_BASE << \
        SYS_HUMBER_DSNH_INTERNAL_SHIFT) |
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_BYPASS);
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    cmd = DRV_IOW(IOC_TABLE, EPE_NEXT_HOP_INTERNAL4W, DRV_ENTRY_FLAG);
    dsnh.by_pass_all = 1;
    dsnh.replace_dscp = 1;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_BYPASS, cmd, &dsnh));

    return CTC_E_NONE;
}

/**
 @brief This function is used to init resolved dsnexthop for mirror
 */
static INLINE int32
_sys_humber_nh_dsnh_init_for_mirror(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    ds_nexthop_t dsnh;

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    p_offset_attr->offset_base = ((SYS_HUMBER_DSNH_INTERNAL_BASE << \
        SYS_HUMBER_DSNH_INTERNAL_SHIFT) |
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_MIRROR);
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    cmd = DRV_IOW(IOC_TABLE, EPE_NEXT_HOP_INTERNAL4W, DRV_ENTRY_FLAG);
    dsnh.by_pass_all = 1;
    dsnh.payload_operation = SYS_NH_OP_MIRROR;
    dsnh.derive_stag_cos = 1;
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,
        SYS_HUMBER_DSNH4WREG_INDEX_FOR_MIRROR, cmd, &dsnh));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_dsnh_init_for_swap_mac(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    uint32 nhp_ptr = 0;
    uint32 l2edit_ptr = 0;
    ds_nexthop_t dsnh;
    ds_l2_edit_eth8w_t ds_l2_edit_8w;
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_param_dsnh_t dsnh_param;
    int32 ret = CTC_E_NONE;

    CTC_PTR_VALID_CHECK(p_offset_attr);

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    kal_memset(&ds_l2_edit_8w, 0, sizeof(ds_l2_edit_eth8w_t));
    kal_memset(&dsnh_param, 0, sizeof(dsnh_param));
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    nhp_ptr = p_nh_master->max_glb_nh_sram_offset - SYS_HBNH_RES_DSNH_PTR_SWAP_MAC;
    p_offset_attr->offset_base = nhp_ptr;
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    dsnh.by_pass_all = 1;
    dsnh.l2_rewrite_type = SYS_NH_L2EDIT_TYPE_ETH_MAC;
    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W, 1,  &l2edit_ptr));
    dsnh_param.l2edit_ptr = l2edit_ptr;
    ret = _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh, &dsnh_param);
    if (ret < 0)
    {
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_L2EDIT_ETH_8W, 1,  l2edit_ptr));
        return CTC_E_INVALID_DSEDIT_PTR;
    }

    ds_l2_edit_8w.derive_mcast_mac = 1;
    ds_l2_edit_8w.type = 0;
    ds_l2_edit_8w.overwrite_ether_type = 0;
    ds_l2_edit_8w.mac_sa_valid = 1;

    cmd = DRV_IOW(IOC_TABLE, DS_L2_EDIT_ETH8W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, l2edit_ptr, cmd, &ds_l2_edit_8w));

    cmd = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, nhp_ptr, cmd, &dsnh));

    return CTC_E_NONE;
}


static INLINE int32
_sys_humber_nh_dsnh_init_for_remote_cpu(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{

    uint32 cmd;
    uint32 nhp_ptr = 0;
    ds_nexthop_t dsnh;
    sys_humber_nh_master_t* p_nh_master = NULL;
    CTC_PTR_VALID_CHECK(p_offset_attr);

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    nhp_ptr = p_nh_master->max_glb_nh_sram_offset - SYS_HBNH_RES_DSNH_PTR_REMOTE_CPU;
    p_offset_attr->offset_base = nhp_ptr;
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    dsnh.by_pass_all = 1;
    dsnh.payload_operation = SYS_NH_OP_MIRROR;
    dsnh.replace_dscp = 1;

    cmd = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,nhp_ptr, cmd, &dsnh));


    return CTC_E_NONE;
}

/**
 @brief This function is used to init resolved dsnexthop for raw packet elog cpu(oam engine)
 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_dsnh_init_for_ram_packet_elog_cpu(uint8 lchip, sys_nh_offset_attr_t *p_offset_attr)
{
    uint32 cmd;
    uint32 l2edit_ptr = 0;
    uint32 nhp_ptr = 0;
    uint8 gchip = 0;
    ds_nexthop_t dsnh;
    ds_l2_edit_loopback_t ds_l2_lbk;
    sys_humber_nh_master_t* p_nh_master = NULL;
    CTC_PTR_VALID_CHECK(p_offset_attr);

    SYS_NH_DBG_FUNC();

    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    kal_memset(&ds_l2_lbk, 0, sizeof(ds_l2_edit_loopback_t));

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    nhp_ptr = p_nh_master->max_glb_nh_sram_offset - SYS_HBNH_RES_DSNH_PTR_RAW_PACKET_ELOG_CPU;
    p_offset_attr->offset_base = nhp_ptr;
    p_offset_attr->entry_size = 1;
    p_offset_attr->entry_num = 1;

    dsnh.by_pass_all = 0;
    dsnh.payload_operation = SYS_NH_OP_NONE;
    dsnh.l2_rewrite_type = SYS_NH_L2EDIT_TYPE_LOOPBACK;
    dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
    dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
    dsnh.dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
    dsnh.derive_stag_cos = 1;

    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_L2EDIT_LPBK, 1,  &l2edit_ptr));
    dsnh.l2edit_ptr = l2edit_ptr;

    CTC_ERROR_RETURN(sys_humber_get_gchip_id(lchip, &gchip));
    ds_l2_lbk.lb_dest_map = SYS_HBNH_ENCODE_DESTMAP(0, gchip, CTC_HUMBER_LPORT_TO_CPU) ;
    ds_l2_lbk.lb_next_hop_ext= 0;
    ds_l2_lbk.lb_next_hop_ptr = CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_ELOOP_FWD_CPU,0,0);;

    cmd = DRV_IOW(IOC_TABLE, DS_L2_EDIT_LOOPBACK, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,dsnh.l2edit_ptr, cmd, &ds_l2_lbk));

    cmd = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,
    nhp_ptr, cmd, &dsnh));

    return CTC_E_NONE;
}

/**
 @brief This function is used to init resolved dsnexthop for raw packet elog cpu(oam engine)
 */
static INLINE int32
_sys_humber_nh_l2edit_init_for_ipmc_phyif(uint8 lchip, uint32 *p_offset_attr)
{
    uint32 cmd;
    uint32 l2edit_ptr = 0;
    ds_l2_edit_eth4w_t ds_l2_edit_4w;

    kal_memset(&ds_l2_edit_4w, 0, sizeof(ds_l2_edit_eth4w_t));
    ds_l2_edit_4w.derive_mcast_mac = 1;

    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W, 1,  &l2edit_ptr));

    cmd = DRV_IOW(IOC_TABLE, DS_L2_EDIT_ETH4W, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip,l2edit_ptr, cmd, &ds_l2_edit_4w));

    *p_offset_attr = l2edit_ptr;


    return CTC_E_NONE;
}

static int32
_sys_humber_ipe_fwd_ctl_init(uint8 lchip, uint32  fatal_excep_base)
{
    int32 cmd, i;
    uint32 table_base = 0;
    uint8 global_chip_id;
    ipe_forward_ctl_t fwd_ctl;
    uint8 is_hash_48k = 0;

    if(fatal_excep_base > 0xFFFF)
    {
        return CTC_E_INVALID_PARAM;
    }
    kal_memset(&fwd_ctl, 0, sizeof(ipe_forward_ctl_t));
    fwd_ctl.discard_fatal = 0;
    fwd_ctl.force_excep_local_phy_port = 1;
    fwd_ctl.ds_fwd_index_base_fatal = fatal_excep_base;
    fwd_ctl.log_on_discard = 0x1F;
    fwd_ctl.service_id_en_cfg = 1;
    CTC_ERROR_RETURN(sys_humber_get_gchip_id(lchip, &global_chip_id));
    fwd_ctl.humber_id = global_chip_id;
    fwd_ctl.vpls_src_port_en = 1;
    fwd_ctl.flow_id_en = 1;
    fwd_ctl.vrf_id_en = 1;
    fwd_ctl.ds_fwd_stats_base = 0;
    fwd_ctl.pw_sequence_number_absence_check_en = 1;
    fwd_ctl.pw_sequence_number_zero_check_en = 0;
    fwd_ctl.sequence_number_disable_check_en = 1;
    fwd_ctl.sequence_number_exception_en = 1;

    for (i = CTC_FATAL_EXCEP_IGMP_SNOOPING_PACKET; i >= CTC_FATAL_EXCEP_UCAST_IP_HDR_ERROR ; i--)
    {
        switch (i)
        {
        case CTC_FATAL_EXCEP_UCAST_IP_HDR_ERROR:
        case CTC_FATAL_EXCEP_UCAST_IP_OPTION:
        case CTC_FATAL_EXCEP_UCAST_GRE_UNKNOWN:
        case CTC_FATAL_EXCEP_ISATAP_SA_FAIL:
            /*case CTC_FATAL_EXCEP_UCAST_IP_TTL_CHECK_FAIL:*/
        case CTC_FATAL_EXCEP_UCAST_RPF_FAIL:
        case CTC_FATAL_EXCEP_MPLS_LABEL_OUT_RANGE:
        case CTC_FATAL_EXCEP_MPLS_SBIT_ERROR:
            /* case CTC_FATAL_EXCEP_MPLS_TTL_FAIL:*/
            fwd_ctl.discard_fatal |= 1 << (i-CTC_FATAL_EXCEP_UCAST_IP_HDR_ERROR);
            break;
        default:
            break;
        }
    }

    sys_alloc_get_table_address(DS_FWD, &table_base);
    if(table_base>=0x05000000)
    {
        CTC_ERROR_RETURN(sys_alloc_get_is_hash_48k(&is_hash_48k));
        if(is_hash_48k)
        {
            fwd_ctl.ds_fwd_base0 = (((table_base - EXT_SRAM_ASSOCIATE_DATA_BASE)/16+(16+96)*1024) >> 10) & 0x3FF;
            fwd_ctl.ds_fwd_base1 = (((table_base - EXT_SRAM_ASSOCIATE_DATA_BASE)/16+(16+96)*1024) >> 8) & 0x3;
        }
        else
        {
            fwd_ctl.ds_fwd_base0 = (((table_base - EXT_SRAM_ASSOCIATE_DATA_BASE)/16+(16+48)*1024) >> 10) & 0x3FF;
            fwd_ctl.ds_fwd_base1 = (((table_base - EXT_SRAM_ASSOCIATE_DATA_BASE)/16+(16+48)*1024) >> 8) & 0x3;
        }
    }
    else
    {
        fwd_ctl.ds_fwd_base0 = (((table_base - SRAM_ASSOCIATE_DATA_BASE)/16) >> 10) & 0x3FF;
        fwd_ctl.ds_fwd_base1 = (((table_base - SRAM_ASSOCIATE_DATA_BASE)/16) >> 8) & 0x3;
    }
    cmd = DRV_IOW(IOC_REG, IPE_FORWARD_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &fwd_ctl));

    return CTC_E_NONE;
}


static INLINE int32
_sys_humber_nh_fatal_excep_init(uint8 lchip, uint32 *p_max_offset, uint32 *p_fatal_excp_base)
{
    uint32 offset, base_offset;

    base_offset = (*p_max_offset - ((1 << SYS_HUMBER_RESV_OFFSET_FATAL_EXCEP_SHIFT))) + 1;
    base_offset = base_offset >> SYS_HUMBER_RESV_OFFSET_FATAL_EXCEP_SHIFT;
    /*1. init fwd ctl*/
    CTC_ERROR_RETURN(_sys_humber_ipe_fwd_ctl_init(lchip, base_offset));

    offset = base_offset << SYS_HUMBER_RESV_OFFSET_FATAL_EXCEP_SHIFT;
    /*3. save fatal exception's dsfwd offset*/
    *p_fatal_excp_base = offset;

    *p_max_offset = offset - 1;

    return CTC_E_NONE;
}


/**
 @brief This function is used to get and init crossconnect fwd base

 @param[in] lchip, local chip id

 @param[in] is_port, port cross connect or vlan cross connect

 @param[out] p_crscnt_base, cross connect base fwdptr

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_crscnt_init(uint8 lchip, bool is_port, uint32 *p_max_offset, uint32 *p_crscnt_base)
{
    uint32 cmd;
    uint32 crscnt_base;

    SYS_NH_DBG_FUNC();

    if (is_port)
    {
        crscnt_base = (*p_max_offset - SYS_HUMBER_CRSCNT_MAX_MMEMBER_NUM + 1);
        crscnt_base = crscnt_base >> SYS_HUMBER_RESV_OFFSET_CRS_CNT_SHIFT;
        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PORT_CROSS_CONNECT_FWD_BASE);
    }
    else
    {
        cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_VLAN_CROSS_CONNECT_FWD_BASE);
    }
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &crscnt_base));
    *p_crscnt_base = crscnt_base << SYS_HUMBER_RESV_OFFSET_CRS_CNT_SHIFT;
    *p_max_offset = *p_crscnt_base - 1;
    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_local_dyn_offset_resv_proc(uint8 lchip, sys_humber_nh_master_t* p_master, uint32 *p_max_offset)
{
    uint32 tmp_offset;

    tmp_offset = *p_max_offset;
    /*1. Reserve and init dsfwd for fatal exception*/
    CTC_ERROR_RETURN(_sys_humber_nh_fatal_excep_init(lchip, &tmp_offset,
                     &p_master->fatal_excp_base[lchip]));

    /*2. Init port crossconnect*/
    CTC_ERROR_RETURN(_sys_humber_nh_crscnt_init(lchip, TRUE, &tmp_offset,
                     &p_master->port_crscnt_base[lchip]));

    *p_max_offset = tmp_offset;
    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_offset_init_dsnh4w_edit_offset(sys_humber_nh_master_t* p_master)
{
    uint32 edit_ptr_bits_num = 0;
    uint32 cmd = 0;

     cmd = DRV_IOR(IOC_REG, EPE_NEXT_HOP_CTL, EPE_NEXT_HOP_CTL_EDIT_PTR_BITS_NUM);
     CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &edit_ptr_bits_num));
     p_master->edit_ptr_bits_num = edit_ptr_bits_num;

      return CTC_E_NONE;
}
/**
 @brief This function is used to initilize dynamic offset, inclue offset pool,
        and resolved offset

 @param[in] p_master, nexthop module master data

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_offset_init(sys_humber_nh_master_t* p_master)
{
    sys_humber_opf_t opf;
    uint8 lchip, curr_chip_num;
    sys_alloc_info_t * p_alloc_info;
    uint32 local_met_dsfwd_num = 0;
    uint32 total_met_dsfwd_num = 0;

    SYS_NH_DBG_FUNC();

    _sys_humber_nh_offset_init_dsnh4w_edit_offset(p_master);

    curr_chip_num = sys_humber_get_local_chip_num();
    CTC_ERROR_RETURN(sys_humber_opf_init(LOCAL_MET_DSFWD_SRAM, curr_chip_num));
    CTC_ERROR_RETURN(sys_humber_opf_init(LOCAL_NEXTHOP_SRAM, curr_chip_num));
    CTC_ERROR_RETURN(sys_humber_opf_init(L2EDIT_SRAM, curr_chip_num));
    CTC_ERROR_RETURN(sys_humber_opf_init(L3EDIT_SRAM, curr_chip_num));

    /*Init resolved offset in dynamic table*/
    p_alloc_info = sys_alloc_get_alloc_info_ptr();

    for(lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*0. Init global dsmet entry, assign default value for met entry.
        The default value is discarding packets, for distributing system, Other chip's met entry
        maybe configured before this chip's*/
        CTC_ERROR_RETURN(sys_humber_nh_global_dync_entry_set_default(
            lchip, 0, (p_alloc_info->global_met_entry_num - 1)));


        /*Init resolved offset in register*/
        /*1. DsNexthop offset for bridge, */
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_brg(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_BRIDGE_NH])));

        /*2. DsNexthop for untagged bridge*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_untagged_brg(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_UNTAGGED_BRIDGE_NH])));


        /*3. DsNexthop for bypassall*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_bypass(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH])));

        /*4. DsNexthop for mirror*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_mirror(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_MIRROR_NH])));

        /*5. Init offset pull*/
        /*5.1 Init Local DsNexthop 4w & 8w offset pull*/
        if((p_alloc_info->global_nh_entry_num + p_alloc_info->local_nh_entry_num) >=
            (SYS_HUMBER_DSNH_INTERNAL_BASE << SYS_HUMBER_DSNH_INTERNAL_SHIFT))
        {
            return CTC_E_EXCEED_MAX_DSNH_OFFSET;
        }
        opf.pool_type = LOCAL_NEXTHOP_SRAM;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, p_alloc_info->global_nh_entry_num, p_alloc_info->local_nh_entry_num));
        if (p_master->local_nh_4w_num > 0)
        {
            CTC_ERROR_RETURN(sys_humber_opf_reserve_size_for_reverse_alloc(&opf,
                (p_alloc_info->local_nh_entry_num - p_master->local_nh_4w_num)));
        }

        /*5.2 Init L2Edit 4w & 8w offset pull*/
        opf.pool_type = L2EDIT_SRAM;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, p_alloc_info->l2edit_entry_num));
        if (p_master->l2_edit_4w_num > 0)
        {
            CTC_ERROR_RETURN(sys_humber_opf_reserve_size_for_reverse_alloc(&opf,
                (p_alloc_info->l2edit_entry_num - p_master->l2_edit_4w_num)));
        }

        /*5.3 Init L3Edit 4w & 8w offset pull*/
        opf.pool_type = L3EDIT_SRAM;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, p_alloc_info->l3edit_entry_num));
        if (p_master->l3_edit_4w_num > 0)
        {
            CTC_ERROR_RETURN(sys_humber_opf_reserve_size_for_reverse_alloc(&opf,
                (p_alloc_info->l3edit_entry_num - p_master->l3_edit_4w_num)));
        }

        /*5.4 Init occupied global met offset*/
        p_master->max_glb_met_sram_offset = p_alloc_info->global_met_entry_num - 1;
        p_master->p_occupid_met_offset_bmp = (uint32 *)mem_malloc(MEM_NEXTHOP_MODULE,
            (sizeof(uint32) * (p_master->max_glb_met_sram_offset/BITS_NUM_OF_WORD + 1)));
        kal_memset(p_master->p_occupid_met_offset_bmp, 0,
            (sizeof(uint32) * (p_master->max_glb_met_sram_offset/BITS_NUM_OF_WORD + 1)));

        /*5.5 Init occupied global nexthop offset*/
        p_master->max_glb_nh_sram_offset = p_alloc_info->global_nh_entry_num - 1;
        p_master->p_occupid_nh_offset_bmp = (uint32 *)mem_malloc(MEM_NEXTHOP_MODULE,
            (sizeof(uint32) * (p_master->max_glb_nh_sram_offset/BITS_NUM_OF_WORD + 1)));
        kal_memset(p_master->p_occupid_nh_offset_bmp, 0,
            (sizeof(uint32) * (p_master->max_glb_nh_sram_offset/BITS_NUM_OF_WORD + 1)));

        /*6. Init Local DsMet & dsfwd offset pull*/
        /*6.1 Reserve dsfwd offset */
        local_met_dsfwd_num = p_alloc_info->local_met_dsfwd_entry_num;
        total_met_dsfwd_num = local_met_dsfwd_num + p_alloc_info->global_met_entry_num;
        if(local_met_dsfwd_num <= p_master->acl_redirect_fwd_ptr_num)
        {
            return CTC_E_NO_OFFSET_LEFT;
        }
        total_met_dsfwd_num -= p_master->acl_redirect_fwd_ptr_num;
        CTC_ERROR_RETURN(_sys_humber_nh_local_dyn_offset_resv_proc(lchip, p_master, &total_met_dsfwd_num));
        local_met_dsfwd_num = total_met_dsfwd_num - p_alloc_info->global_met_entry_num;
        /*6.2 Init Local DsMet & dsfwd offset pull*/
        opf.pool_type = LOCAL_MET_DSFWD_SRAM;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, p_alloc_info->global_met_entry_num, local_met_dsfwd_num));
        if (p_master->local_met_num > 0)
        {
            if (p_master->local_met_num >= local_met_dsfwd_num)
            {
                return CTC_E_NO_OFFSET_LEFT;
            }
            CTC_ERROR_RETURN(sys_humber_opf_reserve_size_for_reverse_alloc(&opf,
                (local_met_dsfwd_num - p_master->local_met_num)));
        }

        /*7. DsNexthop for swap mac*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_swap_mac(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_SWAPMAC_NH])));

        /*8. DsNexthop for raw packet to cpu or oam engine*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_ram_packet_elog_cpu(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_RAW_PACKET_ELOG_CPU])));

        /*9. DsNexthop for remote cpu*/
        CTC_ERROR_RETURN(_sys_humber_nh_dsnh_init_for_remote_cpu(lchip,
                &(p_master->sys_hbnh_resolved_offset[lchip]\
                [SYS_HBNH_RES_OFFSET_TYPE_REMOTECPU_NH])));

        /*10. l2edit for ipmc phy if*/
        CTC_ERROR_RETURN(_sys_humber_nh_l2edit_init_for_ipmc_phyif(lchip,
        &(p_master->ipmc_phyif_resolved_l2edit[lchip])));

    }

     /*7. init for ip tunnel*/
     CTC_ERROR_RETURN(sys_humber_nh_ip_tunnel_init());

    return CTC_E_NONE;
};



/**
 @brief This function is used to get fatal exception dsnexthop offset

 @param[in] p_offset, fatal exception dsnexthop offset

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_fatal_excp_dsnh_offset(uint8 lchip, uint32 *p_offset)
{
   sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_PTR_VALID_CHECK(p_offset);


    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_offset);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    *p_offset = p_nh_master->fatal_excp_base[lchip];


    return CTC_E_NONE;
}

int32
sys_humber_nh_check_glb_nh_sram_offset(uint32 start_offset, uint32 entry_num,
                                        bool should_not_inuse)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 *p_bmp, curr_offset;
    int32 i;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if (NULL == (p_bmp = p_nh_master->p_occupid_nh_offset_bmp))
    {
        return CTC_E_NOT_INIT;
    }
    if(0 == entry_num)
        return CTC_E_NONE;

    if ((start_offset + entry_num - 1) > (p_nh_master->max_glb_nh_sram_offset - SYS_HBNH_RES_DSNH_PTR_MAX))
        return CTC_E_GLB_SRAM_INDEX_EXCEED;

    for (i = 0; i < entry_num; i++)
    {
        curr_offset = start_offset + i;
        if (should_not_inuse && CTC_FLAG_ISSET(p_bmp[((curr_offset) >> BITS_SHIFT_OF_WORD)],
            (1 << (curr_offset & BITS_MASK_OF_WORD))))
        {
            return CTC_E_GLB_SRAM_IS_INUSE;
        }
        if((!should_not_inuse) && (!CTC_FLAG_ISSET(p_bmp[((curr_offset) >> BITS_SHIFT_OF_WORD)],
            (1 << (curr_offset & BITS_MASK_OF_WORD)))))
        {
            return CTC_E_GLB_SRAM_ISNOT_INUSE;
        }
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_check_glb_met_sram_offset(uint32 start_offset, uint32 entry_num,
                                        bool should_not_inuse)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 *p_bmp, curr_offset;
    int32 i;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if (NULL == (p_bmp = p_nh_master->p_occupid_met_offset_bmp))
    {
        return CTC_E_NOT_INIT;
    }
    if(0 == entry_num)
    {
        return CTC_E_NONE;
    }

    if ((start_offset + entry_num - 1) > p_nh_master->max_glb_met_sram_offset)
    {
        return CTC_E_GLB_SRAM_INDEX_EXCEED;
    }

    for (i = 0; i < entry_num; i++)
    {
        curr_offset = start_offset + i;
        if (should_not_inuse && CTC_FLAG_ISSET(p_bmp[((curr_offset) >> BITS_SHIFT_OF_WORD)],
            (1 << (curr_offset & BITS_MASK_OF_WORD))))
        {
            return CTC_E_GLB_SRAM_IS_INUSE;
        }
        if((!should_not_inuse) && (!CTC_FLAG_ISSET(p_bmp[((curr_offset) >> BITS_SHIFT_OF_WORD)],
            (1 << (curr_offset & BITS_MASK_OF_WORD)))))
        {
            return CTC_E_GLB_SRAM_ISNOT_INUSE;
        }
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_set_glb_nh_sram_offset(uint32 start_offset, uint32 entry_num, bool is_set)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 *p_bmp, curr_offset;
    int32 i;

    if(0 == entry_num)
        return CTC_E_NONE;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if ((start_offset + entry_num - 1) > p_nh_master->max_glb_nh_sram_offset)
        return CTC_E_GLB_SRAM_INDEX_EXCEED;

    if (NULL == (p_bmp = p_nh_master->p_occupid_nh_offset_bmp))
    {
        return CTC_E_NOT_INIT;
    }
    for (i = 0; i < entry_num; i++)
    {
        curr_offset = start_offset + i;
        if (is_set)
        {
            CTC_SET_FLAG(p_bmp[(curr_offset >> BITS_SHIFT_OF_WORD)],
                         (1 << (curr_offset & BITS_MASK_OF_WORD)));
        }
        else
        {
            CTC_UNSET_FLAG(p_bmp[(curr_offset >> BITS_SHIFT_OF_WORD)],
                           (1 << (curr_offset & BITS_MASK_OF_WORD)));
        }
    }
    return CTC_E_NONE;
}

int32
sys_humber_nh_set_glb_met_sram_offset(uint32 start_offset, uint32 entry_num, bool is_set)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 *p_bmp, curr_offset;
    int32 i;

    if(0 == entry_num)
        return CTC_E_NONE;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if ((start_offset + entry_num - 1) > p_nh_master->max_glb_met_sram_offset)
        return CTC_E_GLB_SRAM_INDEX_EXCEED;

    if (NULL == (p_bmp = p_nh_master->p_occupid_met_offset_bmp))
    {
        return CTC_E_NOT_INIT;
    }
    for (i = 0; i < entry_num; i++)
    {
        curr_offset = start_offset + i;
        if (is_set)
        {
            CTC_SET_FLAG(p_bmp[(curr_offset >> BITS_SHIFT_OF_WORD)],
                         (1 << (curr_offset & BITS_MASK_OF_WORD)));
        }
        else
        {
            CTC_UNSET_FLAG(p_bmp[(curr_offset >> BITS_SHIFT_OF_WORD)],
                           (1 << (curr_offset & BITS_MASK_OF_WORD)));
        }
    }
    return CTC_E_NONE;
}

/**
 @brief This function is used to get nexthop module master data

 @return TRUE/FALSE
 */
bool
sys_humber_is_glb_alloc_nexthop(void)
{
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    return p_nh_master->glb_alloc_nexhop?TRUE:FALSE;
}

/**
 @brief This function is used to get nexthop module master data

 @return TRUE/FALSE
 */
bool
sys_humber_is_remote_met_use_nexthop(void)
{
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    return p_nh_master->remote_met_use_nhp?TRUE:FALSE;
}
/**
 @brief This function is used to Write/Read asic table

 @param[in] lchip, the local chip id of the opeartion

 @param[in] table_type, Table id of the operation entry

 @param[in] offset, offset of the operation entry

 @param[in] value, the value of to be writed

 @param[out] value, the read value

 @return CTC_E_XXX
 */
int32
sys_humber_nh_write_asic_table(uint8 lchip,
    sys_nh_entry_table_type_t table_type, uint32 offset, void* value)
{
    uint32 cmd;
    tbl_id_t tbl_id;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("lchip = %d, table_type = %d, offset = %d\n",
            lchip, table_type, offset);

    /*Sanity check*/
    CTC_PTR_VALID_CHECK(value);

    tbl_id = nh_table_info_array[table_type].table_id;
    cmd = DRV_IOW(IOC_TABLE, tbl_id, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, value));
    return CTC_E_NONE;
};

int32
sys_humber_nh_get_asic_table(uint8 lchip,
    sys_nh_entry_table_type_t table_type, uint32 offset, void* value)
{
    uint32 cmd;
    tbl_id_t tbl_id;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("lchip = %d, table_type = %d, offset = %d\n",
            lchip, table_type, offset);

    /*Sanity check*/
    CTC_PTR_VALID_CHECK(value);

    tbl_id = nh_table_info_array[table_type].table_id;
    cmd = DRV_IOR(IOC_TABLE, tbl_id, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, value));

    return CTC_E_NONE;
}

/**
 @brief This function is used to build and write dsl2edit eth4w

 @param[in] p_dsmet_param, param used to build dsmet entry

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_write_entry_dsl2editeth4w(sys_nh_db_com_entry_t* p_com_entry)
{
    sys_nh_db_dsl2editeth4w_t* p_dsl2edit_para;
    ds_l2_edit_eth4w_t dsl2edit;
    SYS_NH_DBG_FUNC();

    p_dsl2edit_para = (sys_nh_db_dsl2editeth4w_t*)(p_com_entry);
    kal_memset(&dsl2edit, 0, sizeof(ds_l2_edit_eth4w_t));

    dsl2edit.mac_dah = (p_dsl2edit_para->mac_da[0] << 8 | p_dsl2edit_para->mac_da[1]);
    dsl2edit.mac_dal = (p_dsl2edit_para->mac_da[2] << 24 | p_dsl2edit_para->mac_da[3] << 16 |
        p_dsl2edit_para->mac_da[4] << 8 | p_dsl2edit_para->mac_da[5] );
    dsl2edit.output_vlan_id = p_dsl2edit_para->output_vid;

    if(dsl2edit.output_vlan_id != CTC_MAX_VLAN_ID)
    {
        dsl2edit.output_vlan_id_valid = 1;
    }
    if (p_dsl2edit_para->ouput_vlan_is_svlan)
    {
        dsl2edit.output_vlanid_is_svlan = 1;
    }
    if(CTC_FLAG_ISSET(p_dsl2edit_para->flags, SYS_NH_DSL2EDIT_DERIVE_MCAST_MAC))
    {
        dsl2edit.derive_mcast_mac = 1;
    }

    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(p_dsl2edit_para->hdr.lchip, SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W,
            p_dsl2edit_para->hdr.offset, &dsl2edit));

    return CTC_E_NONE;
}

static INLINE int32
sys_humber_nh_dsnh_build_vlan_info(ds_nexthop_t *p_dsnh,
                                     ctc_vlan_egress_edit_info_t *p_vlan_info)
{
#define SYS_HUMBER_NH_DSNH4W_SVLAN_OP_DISABLE_FLAG    (1 << 1)

    if (CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_VLAN_SWAP_EN))
    {
        /*1. cvlan_tag_op_disable = 1*/
        p_dsnh->replace_ctag_cos = SYS_HUMBER_NH_BOOL_FALSE;
        p_dsnh->copy_ctag_cos = SYS_HUMBER_NH_BOOL_TRUE;

        /*2. svlan_tag_op_disable = 1*/
        p_dsnh->derive_stag_cos = SYS_HUMBER_NH_BOOL_TRUE;
        CTC_SET_FLAG(p_dsnh->stag_cos, SYS_HUMBER_NH_DSNH4W_SVLAN_OP_DISABLE_FLAG);

        /*3. tagged_mode = 1*/
        p_dsnh->tagged_mode = SYS_HUMBER_NH_BOOL_TRUE;

        /*Unset  parametre's cvid valid bit, other with output
        cvlan id valid will be overwrited in function
        sys_humber_nh_dsnh4w_assign_vid*/
        CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID);
        CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID);

        /*Swap TPID*/
        if (CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_TPID_SWAP_EN))
        {
            p_dsnh->output_svlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        }
        /*Swap Cos*/
        if (CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_COS_SWAP_EN))
        {
            p_dsnh->output_cvlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        }
    }
    else
    {
        switch(p_vlan_info->svlan_edit_type)
        {
            case CTC_VLAN_EGRESS_EDIT_NONE:
                /*Don't use DsNexthop's output svlanid*/
                p_dsnh->output_svlan_id_valid = SYS_HUMBER_NH_BOOL_FALSE;
                CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID);
                p_dsnh->svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE:
                p_dsnh->derive_stag_cos = SYS_HUMBER_NH_BOOL_TRUE;
                CTC_SET_FLAG(p_dsnh->stag_cos, SYS_HUMBER_NH_DSNH4W_SVLAN_OP_DISABLE_FLAG);
                p_dsnh->svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsnh->output_svlan_id_valid = SYS_HUMBER_NH_BOOL_FALSE;
                CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID);
                break;

            case CTC_VLAN_EGRESS_EDIT_INSERT_VLAN:
                if(CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN == p_vlan_info->cvlan_edit_type ||
                    CTC_VLAN_EGRESS_EDIT_NONE == p_vlan_info->cvlan_edit_type)
                    return CTC_E_VLAN_EDIT_CONFLICT;
                p_dsnh->tagged_mode = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsnh->svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN:
                p_dsnh->svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_STRIP_VLAN:
                p_dsnh->svlan_tagged = SYS_HUMBER_NH_BOOL_FALSE;
                break;

            default:
                return CTC_E_INVALID_VLAN_EDIT_TYPE;
        }

        switch(p_vlan_info->cvlan_edit_type)
        {
            case CTC_VLAN_EGRESS_EDIT_NONE:
                /*Don't use DsNexthop's output cvlanid*/
                p_dsnh->output_cvlan_id_valid = SYS_HUMBER_NH_BOOL_FALSE;
                CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID);
                p_dsnh->cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE:
                p_dsnh->replace_ctag_cos = SYS_HUMBER_NH_BOOL_FALSE;
                p_dsnh->copy_ctag_cos = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsnh->cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsnh->output_cvlan_id_valid = SYS_HUMBER_NH_BOOL_FALSE;
                CTC_UNSET_FLAG(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID);
                break;

            case CTC_VLAN_EGRESS_EDIT_INSERT_VLAN:
                if(CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN == p_vlan_info->svlan_edit_type ||
                    CTC_VLAN_EGRESS_EDIT_NONE == p_vlan_info->svlan_edit_type)
                    return CTC_E_VLAN_EDIT_CONFLICT;
                p_dsnh->tagged_mode = SYS_HUMBER_NH_BOOL_TRUE;
                p_dsnh->cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_REPLACE_VLAN:
                p_dsnh->cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
                break;

            case CTC_VLAN_EGRESS_EDIT_STRIP_VLAN:
                p_dsnh->cvlan_tagged = SYS_HUMBER_NH_BOOL_FALSE;
                break;

            default:
                return CTC_E_INVALID_VLAN_EDIT_TYPE;
        }
        if (p_vlan_info->replace_stag_cos_valid)
        {
            /* when not derive, new stag cos is DsNextHop.stag_cos*/
            p_dsnh->derive_stag_cos = 0;
            p_dsnh->stag_cfi = 0;
            p_dsnh->stag_cos = p_vlan_info->stag_cos;
        }
        else if (p_vlan_info->map_stag_cos_valid)
        {
            /* new stag cos is mapped from priority.*/
            p_dsnh->derive_stag_cos = 1;
            p_dsnh->stag_cfi = 1;
        }
        else
        {
            /* new stag cos is copied from the original packet. */
            p_dsnh->derive_stag_cos = 1;
            p_dsnh->stag_cfi = 0;
        }
    }

    return CTC_E_NONE;
}

static INLINE int32
sys_humber_nh_dsnh4w_assign_vid(ds_nexthop_t *p_dsnh,
                                     ctc_vlan_egress_edit_info_t *p_vlan_info)
{
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID))
    {
        p_dsnh->output_cvlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh->l2edit_ptr = p_vlan_info->output_cvid;
    }
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID))
    {
        p_dsnh->output_svlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh->l3edit_ptr &= 0x3F000;
        p_dsnh->l3edit_ptr |= p_vlan_info->output_svid & 0xFFF;
    }
    return CTC_E_NONE;
}

static INLINE int32
sys_humber_nh_dsnh8w_assign_vid(ds_nexthop8w_t *p_dsnh8w,
                                     ctc_vlan_egress_edit_info_t *p_vlan_info)
{
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID))
    {
        p_dsnh8w->output_cvlan_id_valid_ext = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh8w->output_cvlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh8w->output_cvlan_id_ext = p_vlan_info->output_cvid;
    }
    if(CTC_FLAG_ISSET(p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID))
    {
        p_dsnh8w->output_svlan_id_valid_ext = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh8w->output_svlan_id_valid = SYS_HUMBER_NH_BOOL_TRUE;
        p_dsnh8w->output_svlan_id_ext = p_vlan_info->output_svid;
    }

    return CTC_E_NONE;
}

int32
 _sys_humber_nh_build_dsnh4w_edit_ptr(ds_nexthop_t *dsnh,  sys_nh_param_dsnh_t *p_dsnh_param)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint32 l3edit_ptr = 0;
    uint32 l2edit_ptr = 0;
    uint32    max_l3edit_offset;
    uint32  max_l2edit_offset;
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    switch (p_nh_master->edit_ptr_bits_num)
    {
    case 1: /*l3edit:20bit,l2edit :10bit*/
        max_l3edit_offset = 1 << 20;
        max_l2edit_offset =   1 << 10;
        l3edit_ptr = p_dsnh_param->l3edit_ptr & 0x3FFFF;
        l2edit_ptr =   (p_dsnh_param->l2edit_ptr & 0x3FF) | ((p_dsnh_param->l3edit_ptr >> 18) << 10) ;
        break;
    case 2: /*l3edit:19 bit,l2edit :11bit*/
        max_l3edit_offset = 1 << 19 ;
        max_l2edit_offset =   1 << 11 ;
        l3edit_ptr = p_dsnh_param->l3edit_ptr & 0x3FFFF;
        l2edit_ptr =  ((p_dsnh_param->l3edit_ptr >> 18) << 11) | (p_dsnh_param->l2edit_ptr & 0x7FF);
        break;
    case 3: /*l3edit:17 bit,l2edit :13   bit*/
        max_l3edit_offset = 1 << 17 ;
        max_l2edit_offset =   1 << 13 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x1FFFF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 17);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;
        break;
    case 4:/*l3edit:16 bit,l2edit :14   bit*/
        max_l3edit_offset = 1 << 16 ;
        max_l2edit_offset =   1 << 14 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0xFFFF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 16);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;
        break;
    case 5:/*l3edit:15 bit,l2edit :15   bit*/
        max_l3edit_offset = 1 << 15 ;
        max_l2edit_offset =   1 << 15 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x7FFF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 15);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;
        break;
    case 6:/*l3edit:14 bit,l2edit :16   bit*/
        max_l3edit_offset = 1 << 14 ;
        max_l2edit_offset =   1 << 16 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x3FFF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 14);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;
        break;
    case 7:/*l3edit:13 bit,l2edit :17   bit*/
        max_l3edit_offset = 1 << 13 ;
        max_l2edit_offset =   1 << 17 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x1FFF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 13);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;

        break;
    case 8:/*l3edit:12 bit,l2edit :18   bit*/
        max_l3edit_offset = 1 << 12 ;
        max_l2edit_offset =   1 << 18 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0xFFF) | ( (p_dsnh_param->l2edit_ptr >> 12) << 12);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;

        break;
    case 9:/*l3edit:11 bit,l2edit :19   bit*/
        max_l3edit_offset = 1 << 11 ;
        max_l2edit_offset =   1 << 19 ;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x7FF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 11);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;

        break;
    case 10: /*l3edit:10 bit,l2edit :20   bit*/
        max_l3edit_offset = 1 << 10;
        max_l2edit_offset =   1 << 20;
        l3edit_ptr = (p_dsnh_param->l3edit_ptr & 0x3FF) |  ((p_dsnh_param->l2edit_ptr >> 12) << 10);
        l2edit_ptr =  p_dsnh_param->l2edit_ptr & 0xFFF;

        break;
    default:/*l3edit:18 bit,l2edit :12   bit*/
        max_l3edit_offset = 1 << 18 ;
        max_l2edit_offset =   1 << 12 ;
        l3edit_ptr = p_dsnh_param->l3edit_ptr & 0x3FFFF;
        l2edit_ptr = p_dsnh_param->l2edit_ptr& 0xFFF;

        break;
    }

     max_l3edit_offset -= 1;
     max_l2edit_offset -= 1;

     if (p_dsnh_param->l2edit_ptr > max_l2edit_offset )
     {
         SYS_NH_DBG_INFO("L2Edit   offset %u exceed  max offset %u \n",
                      p_dsnh_param->l2edit_ptr , max_l2edit_offset);
         return CTC_E_NO_OFFSET_LEFT;
     }

     if (  p_dsnh_param->l3edit_ptr > max_l3edit_offset )
     {
         SYS_NH_DBG_INFO("L3Edit   offset %u exceed  max offset %u \n",
                      p_dsnh_param->l3edit_ptr , max_l3edit_offset);
         return CTC_E_NO_OFFSET_LEFT;
     }
     dsnh->l2edit_ptr = l2edit_ptr;
     dsnh->l3edit_ptr = l3edit_ptr;
     return CTC_E_NONE;

}
/**
 @brief This function is used to build and write dsnexthop4w table

 @param[in] p_dsnh_param, param used to build dsnexthop4w entry

 @return CTC_E_XXX
 */

int32
sys_humber_nh_write_entry_dsnh4w(sys_nh_param_dsnh_t *p_dsnh_param)
{
    ds_nexthop_t dsnh;
    uint32 op_bridge = SYS_NH_OP_BRIDGE_INNER;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    if(p_dsnh_param->vpls_nexthop)
    {
        op_bridge = SYS_NH_OP_BRIDGE_VPLS;
    }

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_dsnh_param);
    kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
    SYS_NH_DBG_INFO("\n\
        lchip = %d, \n\
        dsnh_offset = %d, \n\
        dsnh_type = %d\n, \
        l2EditPtr(outputCvlanID) = %d, \n\
        l3EditPtr(outputSvlanId) = %d \n",
        p_dsnh_param->lchip,
        p_dsnh_param->dsnh_offset,
        p_dsnh_param->dsnh_type,
        p_dsnh_param->l2edit_ptr,
        p_dsnh_param->l3edit_ptr);

    dsnh.replace_dscp = 1;
    switch(p_dsnh_param->dsnh_type)
    {
        case SYS_NH_PARAM_DSNH_TYPE_IPUC:
            /*In asic, the mask and shift value can be configued, here shuold use function to get the variable value, not macro*/
            _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);

            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                SYS_NH_L3EDIT_TYPE_NONE);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            break;
   case SYS_NH_PARAM_DSNH_TYPE_IPMC:
            /*In asic, the mask and shift value can be configued, here shuold use function to get the variable value, not macro*/
            _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                SYS_NH_L3EDIT_TYPE_NONE);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            if (p_nh_master->use_epe_parser_ttl_in_ipmc)
            {
                dsnh.derive_stag_cos = SYS_HUMBER_NH_BOOL_TRUE;
                dsnh.stag_cos = 0x01;
            }
            break;
        case SYS_NH_PARAM_DSNH_TYPE_UPMEP:
               SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);
               dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
               dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
               SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_BRIDGE, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
             break;
        case SYS_NH_PARAM_DSNH_TYPE_BRGUC:
            /*Set default cos action*/
            SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);
             _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_BRIDGE, p_dsnh_param->l2edit_type,
                p_dsnh_param->l3dit_type); 
            if(p_dsnh_param->p_vlan_info)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info(&dsnh, p_dsnh_param->p_vlan_info));
                CTC_ERROR_RETURN(sys_humber_nh_dsnh4w_assign_vid(&dsnh, p_dsnh_param->p_vlan_info));
            }
            break;
        case SYS_NH_PARAM_DSNH_TYPE_DOWNMEP:
                  /*Set default cos action*/
            SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);

              dsnh.derive_stag_cos = 1;  /*svlan_tag_op_disable*/
              dsnh.stag_cos  = 1 <<1;
              dsnh.replace_ctag_cos = 0;  /*cvlan_tag_op_disable*/
              dsnh.copy_ctag_cos = 1;
             SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_BRIDGE, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
            break;
        case SYS_NH_PARAM_DSNH_TYPE_RSPAN:
              dsnh.by_pass_all = 0;
              dsnh.derive_stag_cos = 1;  /*restore vlantag for mirror*/
              dsnh.replace_ctag_cos = 1;  /*mirrorTagAdd*/
              dsnh.output_svlan_id_valid =1;
              dsnh.l3edit_ptr = p_dsnh_param->p_vlan_info->output_svid;
             SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_MIRROR, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
            break;

        case SYS_NH_PARAM_DSNH_TYPE_BYPASS:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_NONE, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
            dsnh.by_pass_all = SYS_HUMBER_NH_BOOL_TRUE;
            break;
      case SYS_NH_PARAM_DSNH_TYPE_MPLS_OP_NONE:
              return CTC_E_SHOULD_USE_DSNH8W;
            break;
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_NONE:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_NONE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                p_dsnh_param->l3dit_type);
              _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_ROUTE:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                p_dsnh_param->l3dit_type);
             _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_L2VPN:
            if(p_dsnh_param->p_vlan_info)
            {
                if((CTC_FLAG_ISSET(p_dsnh_param->p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_CVID_VALID)) ||
                    (CTC_FLAG_ISSET(p_dsnh_param->p_vlan_info->edit_flag, CTC_VLAN_EGRESS_EDIT_OUPUT_SVID_VALID)))
                {
                    return CTC_E_SHOULD_USE_DSNH8W;
                }
                else
                {
                    /*Set default cos action*/
                    SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);

                    SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                        op_bridge, SYS_NH_L2EDIT_TYPE_ETH_4W,
                        p_dsnh_param->l3dit_type);
                    CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info(&dsnh, p_dsnh_param->p_vlan_info));
                     _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
                }
            }
            else
                CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_NONE:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_NONE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                SYS_NH_L3EDIT_TYPE_NONE);
            _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_ROUTE:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                SYS_NH_L3EDIT_TYPE_NONE);
              _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_MPLS:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, SYS_NH_L2EDIT_TYPE_ETH_4W,
                SYS_NH_L3EDIT_TYPE_NONE);
              _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN:
            /*Set default cos action*/
            SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh);

            if(p_dsnh_param->p_vlan_info)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info(&dsnh, p_dsnh_param->p_vlan_info));
                CTC_ERROR_RETURN(sys_humber_nh_dsnh4w_assign_vid(&dsnh, p_dsnh_param->p_vlan_info));
            }
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_BRIDGE, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN_NO_OP:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_NONE, SYS_NH_L2EDIT_TYPE_NONE,
                SYS_NH_L3EDIT_TYPE_NONE);
            break;

        case SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_ROUTE:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_ROUTE, p_dsnh_param->l2edit_type,
                p_dsnh_param->l3dit_type);
             _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            dsnh.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.svlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.cvlan_tagged = SYS_HUMBER_NH_BOOL_TRUE;
            dsnh.replace_dscp = p_dsnh_param->dscp_en;
            break;

        case SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_MIRROR:
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_MIRROR, p_dsnh_param->l2edit_type,
                p_dsnh_param->l3dit_type);
             _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
             dsnh.derive_stag_cos = 1;  /*restore vlantag for mirror*/
             dsnh.replace_ctag_cos = 0;  /*mirrorTagAdd*/
             dsnh.replace_dscp = p_dsnh_param->dscp_en;
             break;

        case SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_GRE:
            if (!p_dsnh_param->p_vlan_info)
            {
                SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                    SYS_NH_OP_NONE, p_dsnh_param->l2edit_type,
                    p_dsnh_param->l3dit_type);
                 _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            }
            else
            {
                /* Need op bridge to strip vlan before encapsulation */
                SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh, p_dsnh_param->dest_vlan_ptr,
                    SYS_NH_OP_BRIDGE, p_dsnh_param->l2edit_type,
                    p_dsnh_param->l3dit_type);
                CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info(&dsnh, p_dsnh_param->p_vlan_info));
                 _sys_humber_nh_build_dsnh4w_edit_ptr(&dsnh,p_dsnh_param);
            }
            break;
        default:
            return CTC_E_INVALID_DSNH_TYPE;
    }
    SYS_NH_DBG_INFO("DsNH44444W :: DestVlanPtr = %d, PldOP = %d, L2RewriteType = %d, "
        "L3RewriteType = %d, Bypass = %d, copyCtagCos = %d, "
        "CvlanTagged = %d, DeriveStagCos = %d, L2EditPtr = %d, L3EditPtr = %d, "
        "MtuCheckEn = %d, OutputCvlanVlid = %d, OutputSvlanVlid = %d, "
        "ReplaceCtagCos = %d, ReplaceDscp = %d, ServiceAclQosEn = %d, "
        "ServicePolicerValid = %d, StagCfi = %d, StagCos = %d, "
        "SvlanTagged = %d, TaggedMode = %d",
        dsnh.dest_vlan_ptr, dsnh.payload_operation,
        dsnh.l2_rewrite_type, dsnh.l3_rewrite_type,
        dsnh.by_pass_all, dsnh.copy_ctag_cos, dsnh.cvlan_tagged,
        dsnh.derive_stag_cos, dsnh.l2edit_ptr, dsnh.l3edit_ptr,
        dsnh.mtu_check_en, dsnh.output_cvlan_id_valid,
        dsnh.output_svlan_id_valid, dsnh.replace_ctag_cos,
        dsnh.replace_dscp, dsnh.service_acl_qos_en, dsnh.service_policer_vld,
        dsnh.stag_cfi, dsnh.stag_cos, dsnh.svlan_tagged, dsnh.tagged_mode);

    if(p_dsnh_param->use_ttl_from_pkt)
    {
        dsnh.stag_cos |= SYS_HUMBER_NH_BOOL_TRUE;
        dsnh.derive_stag_cos = SYS_HUMBER_NH_BOOL_TRUE;
    }
    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(p_dsnh_param->lchip,
        SYS_NH_ENTRY_TYPE_NEXTHOP_4W, p_dsnh_param->dsnh_offset, &dsnh));

    return CTC_E_NONE;
}

int32
sys_humber_nh_write_entry_dsnh8w(sys_nh_param_dsnh_t *p_dsnh_param)
{
    ds_nexthop8w_t dsnh_8w;
    uint32 op_bridge = SYS_NH_OP_BRIDGE_INNER;

    if(p_dsnh_param->vpls_nexthop)
    {
        op_bridge = SYS_NH_OP_BRIDGE_VPLS;
    }

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_dsnh_param);
    kal_memset(&dsnh_8w, 0, sizeof(ds_nexthop8w_t));
    SYS_NH_DBG_INFO("\n\
        lchip = %d, \n\
        dsnh_offset = %d, \n\
        dsnh_type = %d\n, \
        l2EditPtr(outputCvlanID) = %d, \n\
        l3EditPtr(outputSvlanId) = %d \n",
        p_dsnh_param->lchip,
        p_dsnh_param->dsnh_offset,
        p_dsnh_param->dsnh_type,
        p_dsnh_param->l2edit_ptr,
        p_dsnh_param->l3edit_ptr);

    dsnh_8w.replace_dscp = 1;
    dsnh_8w.vpls_dest_port = p_dsnh_param->vpls_port;
    dsnh_8w.vpls_port_check = p_dsnh_param->vpls_port_check;
    switch(p_dsnh_param->dsnh_type)
    {
        case SYS_NH_PARAM_DSNH_TYPE_BRGUC:
            SYS_HUMBER_NH_DSNH_SET_DEFAULT_COS_ACTION(dsnh_8w);
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh_8w, p_dsnh_param->dest_vlan_ptr,
                SYS_NH_OP_BRIDGE, p_dsnh_param->l2edit_type,
                p_dsnh_param->l3dit_type); 
            SYS_HUMBER_NH_DSNH8W_BUILD_L2EDITPTR_L3EDITPTR(dsnh_8w,
                                                           p_dsnh_param->l2edit_ptr,
                                                           p_dsnh_param->l3edit_ptr);
            if(p_dsnh_param->p_vlan_info)
            {
                CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info((ds_nexthop_t *)(&dsnh_8w), p_dsnh_param->p_vlan_info));
                CTC_ERROR_RETURN(sys_humber_nh_dsnh8w_assign_vid(&dsnh_8w, p_dsnh_param->p_vlan_info));
            }
            break;
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_L2VPN:
            if(p_dsnh_param->p_vlan_info)
            {
                    SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh_8w, p_dsnh_param->dest_vlan_ptr,
                        op_bridge, SYS_NH_L2EDIT_TYPE_ETH_4W,
                        p_dsnh_param->l3dit_type);
                    SYS_NH_DBG_INFO("dsnh8w.dest_vlanptr = %d, payloadOp = %d, \
                        l2RewriteType = %d, l3RewriteType = %d\n",
                        dsnh_8w.dest_vlan_ptr, dsnh_8w.payload_operation,
                        dsnh_8w.l2_rewrite_type, dsnh_8w.l3_rewrite_type);
                    CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info((ds_nexthop_t *)(&dsnh_8w)/*Have same structure*/,
                        p_dsnh_param->p_vlan_info));
                    CTC_ERROR_RETURN(sys_humber_nh_dsnh8w_assign_vid(&dsnh_8w, p_dsnh_param->p_vlan_info));
            }
            else
                CTC_ERROR_RETURN(CTC_E_INVALID_PARAM);
            dsnh_8w.mtu_check_en = SYS_HUMBER_NH_BOOL_TRUE;
            SYS_HUMBER_NH_DSNH8W_BUILD_L2EDITPTR_L3EDITPTR(dsnh_8w,
                                                           p_dsnh_param->l2edit_ptr,
                                                           p_dsnh_param->l3edit_ptr);
            break;

        case SYS_NH_PARAM_DSNH_TYPE_MPLS_OP_NONE:
             if(p_dsnh_param->p_vlan_info)
            {
                    SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh_8w, p_dsnh_param->dest_vlan_ptr,
                        SYS_NH_OP_BRIDGE, SYS_NH_L2EDIT_TYPE_NONE,
                        SYS_NH_L3EDIT_TYPE_NONE);
                    SYS_NH_DBG_INFO("dsnh8w.dest_vlanptr = %d, payloadOp = %d, \
                        l2RewriteType = %d, l3RewriteType = %d\n",
                        dsnh_8w.dest_vlan_ptr, dsnh_8w.payload_operation,
                        dsnh_8w.l2_rewrite_type, dsnh_8w.l3_rewrite_type);
                    CTC_ERROR_RETURN(sys_humber_nh_dsnh_build_vlan_info((ds_nexthop_t *)(&dsnh_8w)/*Have same structure*/,
                        p_dsnh_param->p_vlan_info));
                    CTC_ERROR_RETURN(sys_humber_nh_dsnh8w_assign_vid(&dsnh_8w, p_dsnh_param->p_vlan_info));
            }
            else
            {
            dsnh_8w.cvlan_tagged = 1;
            dsnh_8w.svlan_tagged = 1;
            /*svlan_tag_op_disable*/
            dsnh_8w.derive_stag_cos = 1;
            dsnh_8w.stag_cos = 1<<1;
            /*cvlan_tag_op_disable*/
            dsnh_8w.replace_ctag_cos = 0;
            dsnh_8w.copy_ctag_cos = 1;
            SYS_HUMBER_NH_DSNH_ASSIGN_COM_FILED(dsnh_8w, SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL,SYS_NH_OP_BRIDGE_VPLS,
                                                SYS_NH_L2EDIT_TYPE_NONE,
                                                SYS_NH_L3EDIT_TYPE_NONE);
            }
            SYS_NH_DBG_INFO("dsnh8w.dest_vlanptr = %d, payloadOp = %d, \
            l2RewriteType = % d, l3RewriteType = % d\n",
                            dsnh_8w.dest_vlan_ptr, dsnh_8w.payload_operation,
                            dsnh_8w.l2_rewrite_type, dsnh_8w.l3_rewrite_type);

            break;
        case SYS_NH_PARAM_DSNH_TYPE_IPUC:
        case SYS_NH_PARAM_DSNH_TYPE_IPMC:
        case SYS_NH_PARAM_DSNH_TYPE_BYPASS:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_NONE:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_PUSH_OP_ROUTE:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_NONE:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_ROUTE:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_MPLS:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN:
        case SYS_NH_PARAM_DSNH_TYPE_MPLS_POP_OP_L2VPN_NO_OP:

            return CTC_E_SHOULD_USE_DSNH4W;
        default:
            return CTC_E_INVALID_DSNH_TYPE;
    }
    dsnh_8w.stag_cos |= SYS_HUMBER_NH_BOOL_TRUE;
    SYS_NH_DBG_INFO("DsNH88888W :: DestVlanPtr = %d, PldOP = %d, L2RewriteType = %d, "
        "L3RewriteType = %d, Bypass = %d, copyCtagCos = %d, "
        "CvlanTagged = %d, DeriveStagCos = %d, L2EditPtr12to0 = %d, "
        "L2EditPtr19to13 = %d, L3EditPtr17to0 = %d, l3edit18 = %d, "
        "l3edit19 = %d, CommuityPort = %d, "
        "MtuCheckEn = %d, OutputCvlanVlid = %d, OutputSvlanVlid = %d, "
        "ReplaceCtagCos = %d, ReplaceDscp = %d, ServiceAclQosEn = %d, "
        "ServicePolicerValid = %d, StagCfi = %d, StagCos = %d, "
        "SvlanTagged = %d, TaggedMode = %d",
        dsnh_8w.dest_vlan_ptr, dsnh_8w.payload_operation,
        dsnh_8w.l2_rewrite_type, dsnh_8w.l3_rewrite_type,
        dsnh_8w.by_pass_all, dsnh_8w.copy_ctag_cos, dsnh_8w.cvlan_tagged,
        dsnh_8w.derive_stag_cos, dsnh_8w.l2edit_ptr11to0, dsnh_8w.l2edit_ptr18to12,
        dsnh_8w.l3edit_ptr170, dsnh_8w.l3edit_ptr18, dsnh_8w.l3edit_ptr19, dsnh_8w.community_port,
        dsnh_8w.mtu_check_en, dsnh_8w.output_cvlan_id_valid,
        dsnh_8w.output_svlan_id_valid, dsnh_8w.replace_ctag_cos,
        dsnh_8w.replace_dscp, dsnh_8w.service_acl_qos_en, dsnh_8w.service_policer_valid,
        dsnh_8w.stag_cfi, dsnh_8w.stag_cos, dsnh_8w.svlan_tagged, dsnh_8w.tagged_mode);

    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(p_dsnh_param->lchip,
        SYS_NH_ENTRY_TYPE_NEXTHOP_8W, p_dsnh_param->dsnh_offset, &dsnh_8w));

    return CTC_E_NONE;
}

/**
 @brief This function is used to get dsfwd info

 @param[in] p_dsfwd_param, param used to build dsfwd entry

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_entry_dsfwd(uint8 lchip, uint32 dsfwd_offset, void* p_dsfwd)
{
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_dsfwd);

    CTC_ERROR_RETURN(sys_humber_nh_get_asic_table(lchip, SYS_NH_ENTRY_TYPE_FWD,
        dsfwd_offset, p_dsfwd));

    return CTC_E_NONE;
}

/**
 @brief This function is used to build and write dsfwd table

 @param[in] p_dsfwd_param, param used to build dsfwd entry

 @return CTC_E_XXX
 */
int32
sys_humber_nh_write_entry_dsfwd(sys_nh_param_dsfwd_t *p_dsfwd_param)
{
    ds_fwd_t dsfwd;
    uint8 lchip = 0;
    bool enable = 0;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_dsfwd_param);
    CTC_PTR_VALID_CHECK(p_dsfwd_param->p_dsfwd_info);

    kal_memset(&dsfwd, 0, sizeof(ds_fwd_t));
    SYS_NH_DBG_INFO("\n\
        lchip = %d, \n\
        dsfwd_offset = %d, \n\
        dest_chip_id = %d, \n\
        dest_id = %d, \n\
        dropPkt = %d, \n\
        dsnh_offset = %d, \n\
        is_mcast = %d\n",
        p_dsfwd_param->lchip,
        p_dsfwd_param->p_dsfwd_info->dsfwd_offset,
        p_dsfwd_param->dest_chipid,
        p_dsfwd_param->dest_id,
        p_dsfwd_param->drop_pkt,
        p_dsfwd_param->dsnh_offset,
        p_dsfwd_param->is_mcast);

    if(p_dsfwd_param->dsnh_offset > 0xFFFFF)
        return CTC_E_EXCEED_MAX_DSNH_OFFSET;

    CTC_ERROR_RETURN(sys_humber_get_service_queue_enable(&enable));

    dsfwd.next_hop_ptr = (p_dsfwd_param->dsnh_offset) & 0xFFFFF;
    dsfwd.aps_type = p_dsfwd_param->aps_type;
    if(p_dsfwd_param->nexthop_ext)
    {
        dsfwd.next_hop_ext = SYS_HUMBER_NH_BOOL_TRUE;
    }
    else
    {
        dsfwd.next_hop_ext = SYS_HUMBER_NH_BOOL_FALSE;
    }

    if(p_dsfwd_param->drop_pkt)
    {
        dsfwd.dest_map = SYS_HUMBER_NH_DROP_DESTMAP;
    }
    else
    {
        if(p_dsfwd_param->service_queue_en)
        {
            if(!enable)
            {
                return CTC_E_SERVICE_QUEUE_NOT_INITIALIZED;
            }
            p_dsfwd_param->dest_id &=0xFFF;
            p_dsfwd_param->dest_id |= (SYS_QSEL_TYPE_SERVICE << 12); /*queueseltype:destmap[15:12]*/
        }
        else
        {

            /*refer to sys_humber_queue_enq.c, the internal port is from 64 ~ xx ,and the queue select type == SYS_QSEL_TYPE_INTERNAL_PORT*/
            if((FALSE == p_dsfwd_param->is_mcast)
                && (dsfwd.aps_type != CTC_APS_BRIDGE && dsfwd.aps_type != CTC_APS_SELECT )
                &&  sys_humber_chip_is_local(p_dsfwd_param->dest_chipid, &lchip)
                &&((p_dsfwd_param->dest_id & 0xFF) > SYS_STATIC_INT_PORT_END))
            {
                p_dsfwd_param->dest_id |= (SYS_QSEL_TYPE_INTERNAL_PORT << 12); /*queueseltype:destmap[15:12]*/
            }

            if((FALSE == p_dsfwd_param->is_mcast)
                && (dsfwd.aps_type != CTC_APS_BRIDGE && dsfwd.aps_type != CTC_APS_SELECT )
                &&  sys_humber_chip_is_local(p_dsfwd_param->dest_chipid, &lchip)
                && ((p_dsfwd_param->dest_id & 0xFF) >= SYS_STATIC_INT_PORT_START)
                && ((p_dsfwd_param->dest_id & 0xFF) <= SYS_STATIC_INT_PORT_END))
             {
                p_dsfwd_param->dest_id |= (SYS_QSEL_TYPE_STATIC_INT_PORT << 12); /*queueseltype:destmap[15:12]*/
             }
        }
        dsfwd.dest_map = SYS_HBNH_ENCODE_DESTMAP(p_dsfwd_param->is_mcast,
                                 p_dsfwd_param->dest_chipid, p_dsfwd_param->dest_id);
    }

    if(p_dsfwd_param->sequence_chk_en)
    {
        dsfwd.sequence_number_chk_en = TRUE;

    }
    dsfwd.stats_ptr_lower = p_dsfwd_param->stats_ptr & 0xfff;
    dsfwd.stats_ptr_upper = (p_dsfwd_param->stats_ptr >> 12) & 0xf;
    dsfwd.stats_valid       =  p_dsfwd_param->stats_valid;

    CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(p_dsfwd_param->lchip, SYS_NH_ENTRY_TYPE_FWD,
        p_dsfwd_param->p_dsfwd_info->dsfwd_offset, &dsfwd));

    return CTC_E_NONE;
}


/**
 @brief AVL compare function for dsl2editeth4w tree

 @param[in] p_data_new, node to be added

 @param[in] p_data_old, node existed in the AVL tree

 @return CTC_E_XXX
 */
static int32
_sys_humber_nh_db_dsl2editeth4w_cmp(void* p_data_new, void* p_data_old)
{
    sys_nh_db_dsl2editeth4w_t *p_new = (sys_nh_db_dsl2editeth4w_t*)p_data_new;
    sys_nh_db_dsl2editeth4w_t *p_old = (sys_nh_db_dsl2editeth4w_t*)p_data_old;
    SYS_NH_DBG_FUNC();

    if((NULL == p_new) || (NULL == p_old))
        return -1;

    if(p_new->hdr.lchip > p_old->hdr.lchip)
        return 1;

    if(p_new->hdr.lchip < p_old->hdr.lchip)
        return -1;

    if(p_new->output_vid > p_old->output_vid)
        return 1;

    if(p_new->output_vid < p_old->output_vid)
        return -1;

    if(p_new->flags > p_old->flags)
        return 1;

    if(p_new->flags < p_old->flags)
        return -1;

    if(p_new->ouput_vlan_is_svlan > p_old->ouput_vlan_is_svlan)
        return 1;

    if(p_new->ouput_vlan_is_svlan < p_old->ouput_vlan_is_svlan)
        return -1;

    return kal_memcmp(p_new->mac_da, p_old->mac_da, sizeof(mac_addr_t));
}

/**
 @brief Get AVL tree root by AVL node type

 @param[in] entry_type, AVL node type

 @param[out] pp_avl_root, AVL tree root

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_db_get_avl_by_type(
    sys_nh_entry_table_type_t entry_type, struct ctc_avl_tree** pp_avl_root)
{
    sys_humber_nh_master_t *p_master;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("Entry type = %d\n", entry_type);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_master));

    switch(entry_type)
    {
        case SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W :
            *pp_avl_root = p_master->dsl2edit4w_tree;
            break;

        case SYS_NH_ENTRY_TYPE_NEXTHOP_4W :
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get AVL node size by AVL node type

 @param[in] entry_type, AVL node type

 @param[out] p_size, node size

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_db_get_node_size_by_type(
    sys_nh_entry_table_type_t entry_type, uint32 *p_size)
{
    sys_humber_nh_master_t *p_master;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("Entry type = %d\n", entry_type);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_master));

    switch(entry_type)
    {
        case SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W :
            *p_size = sizeof(sys_nh_db_dsl2editeth4w_t);
            break;

        case SYS_NH_ENTRY_TYPE_NEXTHOP_4W :
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get nh db entry size

 @param[in] entry_type, AVL node type

 @param[out] p_size, node size

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_get_nhentry_size_by_nhtype(
    sys_humber_nh_type_t nh_type, uint32 *p_size)
{
    sys_humber_nh_master_t *p_master;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("NH type = %d\n", nh_type);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_master));

    switch(nh_type)
    {
        case SYS_HUMBER_NH_TYPE_BRGUC :
            *p_size = sizeof(sys_nh_info_brguc_t);
            break;

        case SYS_HUMBER_NH_TYPE_MCAST:
            *p_size = sizeof(sys_nh_info_mcast_t);
            break;

        case SYS_HUMBER_NH_TYPE_IPUC :
            *p_size = sizeof(sys_nh_info_ipuc_t);
            break;

        case SYS_HUMBER_NH_TYPE_ECMP :
            *p_size = sizeof(sys_nh_info_ecmp_t);
            break;

        case SYS_HUMBER_NH_TYPE_MPLS :
            *p_size = sizeof(sys_nh_info_mpls_t);
            break;

        case SYS_HUMBER_NH_TYPE_DROP:
        case SYS_HUMBER_NH_TYPE_TOCPU:
        case SYS_HUMBER_NH_TYPE_UNROV:
        case SYS_HUMBER_NH_TYPE_ILOOP:
            *p_size = sizeof(sys_nh_info_special_t);
            break;
        case SYS_HUMBER_NH_TYPE_RSPAN :
            *p_size = sizeof(sys_nh_info_rspan_t);
            break;
        case SYS_HUMBER_NH_TYPE_DOWNMEP:
            *p_size = sizeof(sys_nh_info_downmep_t);
            break;
        case SYS_HUMBER_NH_TYPE_IP_TUNNEL:
            *p_size = sizeof(sys_nh_info_ip_tunnel_t); /* not used for IP Tunnel*/
            break;
        case SYS_HUMBER_NH_TYPE_FLEX:
            *p_size = sizeof(sys_nh_info_flex_t);
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Get AVL node size by AVL node type

 @param[in] entry_type, AVL node type

 @param[out] p_size, node size

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_db_write_entry(sys_nh_db_com_entry_t* p_com_entry)
{
    sys_humber_nh_master_t *p_master;
    sys_nh_entry_table_type_t entry_type = SYS_NH_ENTRY_TYPE_NULL;
    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("Entry type = %d\n", entry_type);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_master));
    entry_type = p_com_entry->hdr.entry_type;

    switch(entry_type)
    {
        case SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W :
            _sys_humber_nh_write_entry_dsl2editeth4w(p_com_entry);
            break;

        case SYS_NH_ENTRY_TYPE_NEXTHOP_4W :
        default:
            return CTC_E_INVALID_PARAM;
    }
    return CTC_E_NONE;
}

/**
 @brief Shared entry lookup in AVL treee

 @param[in,out] pp_com_entry, the search node,  If found the node, return the node pointer,
             if can't found the node, return NULL

 @return CTC_E_XXX
 */
static int32
_sys_humber_nh_db_lookup(sys_nh_db_com_entry_t** pp_com_entry)
{
    sys_nh_entry_table_type_t entry_type;
    ctc_avl_tree_t *p_avl_root = NULL;
    ctc_avl_node_t *p_node = NULL;
    SYS_NH_DBG_FUNC();

    /* 1. init */
    entry_type = ((*pp_com_entry)->hdr.entry_type);
    CTC_ERROR_RETURN(_sys_humber_nh_db_get_avl_by_type(entry_type, &p_avl_root));

    /*2. Do lookup*/
    p_node = ctc_avl_search(p_avl_root,  (*pp_com_entry));
    if(p_node && p_node->info)
    {
        *pp_com_entry = p_node->info;
    }
    else
    {
        *pp_com_entry = NULL;
    }

    return CTC_E_NONE;
}

/**
 @brief Insert node into AVL tree

 @param[in,out] pp_com_entry, the node infomation to be inserted,
                return the inserted node's pointer

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_db_insert(sys_nh_db_com_entry_t* p_com_entry)
{
    int32 ret = CTC_E_NONE;
    sys_nh_entry_table_type_t entry_type;
    ctc_avl_tree_t *p_avl_root = NULL;

    SYS_NH_DBG_FUNC();

    /* 1. init */
    entry_type = (p_com_entry->hdr.entry_type);
    CTC_ERROR_RETURN(_sys_humber_nh_db_get_avl_by_type(entry_type, &p_avl_root));

    /*2. Insert node*/
    ret = ctc_avl_insert(p_avl_root,  (p_com_entry));

    return ret;
}

int32
sys_humber_nh_db_set_entry(sys_nh_db_com_entry_t** pp_com_entry)
{
    sys_nh_db_com_entry_t* p_new_entry = NULL;
    sys_nh_entry_table_type_t entry_type;
    uint32 entry_num = 1;
    uint32 node_size, new_offset;
    int32 ret = CTC_E_NONE;

    CTC_PTR_VALID_CHECK(pp_com_entry);
    CTC_PTR_VALID_CHECK(*pp_com_entry);
    entry_type = (*pp_com_entry)->hdr.entry_type;
    p_new_entry = *pp_com_entry;
    CTC_ERROR_RETURN(_sys_humber_nh_db_lookup(&p_new_entry));

    if(p_new_entry)
    /*Found node*/
    {
        p_new_entry->hdr.ref_cnt++;
        *pp_com_entry = p_new_entry;
        return CTC_E_NONE;
    }

    /*Insert new one*/
    CTC_ERROR_RETURN(sys_humber_nh_offset_alloc((*pp_com_entry)->hdr.lchip,
                    (*pp_com_entry)->hdr.entry_type, entry_num, &new_offset));
    CTC_ERROR_RETURN(_sys_humber_nh_db_get_node_size_by_type(entry_type, &node_size));
    p_new_entry = (sys_nh_db_com_entry_t*)mem_malloc(MEM_NEXTHOP_MODULE, node_size);
    if(NULL == p_new_entry)
        return CTC_E_NO_MEMORY;
    kal_memcpy(p_new_entry, *pp_com_entry, node_size);
    p_new_entry->hdr.ref_cnt = 1;/*New create node*/
    p_new_entry->hdr.offset = new_offset;
    ret = _sys_humber_nh_db_write_entry(p_new_entry);
    ret = ret ? ret : _sys_humber_nh_db_insert(p_new_entry);
    if(ret)
    {
        mem_free(p_new_entry);
        *pp_com_entry = NULL;
        CTC_ERROR_RETURN(ret);
    }

    *pp_com_entry = p_new_entry;

    return CTC_E_NONE;
}

int32
sys_humber_nh_db_remove_entry(sys_nh_db_com_entry_t* p_com_entry)
{
    sys_nh_entry_table_type_t entry_type;
    uint32 entry_num = 1, refcnt;
    ctc_avl_tree_t *p_avl_root = NULL;

    CTC_PTR_VALID_CHECK(p_com_entry);
    refcnt = p_com_entry->hdr.ref_cnt - 1;
    if((refcnt) > 0)
    {
        p_com_entry->hdr.ref_cnt = refcnt;
        return CTC_E_NONE;
    }

    entry_type = (p_com_entry->hdr.entry_type);
    CTC_ERROR_RETURN(_sys_humber_nh_db_get_avl_by_type(entry_type, &p_avl_root));
    CTC_ERROR_RETURN(sys_humber_nh_offset_free(p_com_entry->hdr.lchip,
                    p_com_entry->hdr.entry_type, entry_num, p_com_entry->hdr.offset));
    CTC_ERROR_RETURN(ctc_avl_remove(p_avl_root, p_com_entry));
    mem_free(p_com_entry);

    return CTC_E_NONE;
}

int32
sys_humber_nh_db_update_entry(sys_nh_db_com_entry_t* p_old_entry,
                              sys_nh_db_com_entry_t** pp_new_entry)
{
    CTC_PTR_VALID_CHECK(p_old_entry);
    CTC_PTR_VALID_CHECK(pp_new_entry);
    CTC_PTR_VALID_CHECK(*pp_new_entry);

    CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry(p_old_entry));
    CTC_ERROR_RETURN(sys_humber_nh_db_set_entry(pp_new_entry));

    return CTC_E_NONE;
}

/**
 @brief This function is used to get nexthop information by nexthop id

 @param[in] nhid, the nexthop id

 @param[in] p_nh_master, nexthop module master data

 @param[out] pp_nhinfo, nexthop information pointer to be got

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_api_get_nhinfo_by_nhid(uint32 nhid, sys_humber_nh_master_t* p_nh_master,
                                      sys_nh_info_com_t** pp_nhinfo)
{
    sys_nh_info_com_t *p_nh_info;

    if(nhid < p_nh_master->max_external_nhid )
    {
        p_nh_info = ctc_vector_get(p_nh_master->external_nhid_vec,nhid);
    }
    else if(nhid < SYS_HUMBER_NH_INTERNAL_NHID_MAX)
    {
        p_nh_info = ctc_vector_get(p_nh_master->internal_nhid_vec,
            SYS_NH_GET_VECTOR_INDEX_BY_NHID(nhid));
    }
    else
        return CTC_E_INVALID_NHID;

    if(NULL == p_nh_info)
    {
        return CTC_E_NH_NOT_EXIST;
    }

    *pp_nhinfo = p_nh_info;

    return CTC_E_NONE;
}

int32
sys_humber_nh_get_nhinfo_by_nhid(uint32 nhid, sys_nh_info_com_t** pp_nhinfo)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_info_com_t* p_nhinfo = NULL;

    CTC_PTR_VALID_CHECK(pp_nhinfo);
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

    *pp_nhinfo = p_nhinfo;

    return CTC_E_NONE;
}

/**
 @brief This function is used to get DsNexthop offset by NexthopID

 @param[in] nhid, Nexthop ID

 @param[out] p_dsnh_offset, DsNexthop offset

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_dsnh_offset_by_nhid(uint32 nhid, uint32 *p_dsnh_offset,uint8 *p_use_dsnh8w)
{
    sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));


    *p_use_dsnh8w = FALSE;
    switch(p_nhinfo->hdr.nh_entry_type)
    {
    case SYS_HUMBER_NH_TYPE_BRGUC:
            {
                sys_nh_info_brguc_t *p_brguc_info;
                p_brguc_info = (sys_nh_info_brguc_t *)p_nhinfo;
                *p_dsnh_offset = p_brguc_info->dsnh_info.dsnh_offset;

            }
            break;
    case SYS_HUMBER_NH_TYPE_MPLS:
            {
                sys_nh_info_mpls_t *p_mpls_info;
                p_mpls_info = (sys_nh_info_mpls_t *)(p_nhinfo);

                  if(CTC_FLAG_ISSET(p_mpls_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_USE_DSNH8W))
                  {
                       *p_use_dsnh8w = TRUE;
                  }
                 *p_dsnh_offset  = p_mpls_info->dsnh_offset;
            }
            break;
    case SYS_HUMBER_NH_TYPE_RSPAN:
            {
               sys_nh_info_rspan_t *p_rspan_info = (sys_nh_info_rspan_t *)p_nhinfo;
               *p_dsnh_offset = p_rspan_info->dsnh_info.dsnh_offset;
            }
            break;
    case SYS_HUMBER_NH_TYPE_IPUC:
            {
               sys_nh_info_ipuc_t *p_ipuc_info = (sys_nh_info_ipuc_t *)p_nhinfo;
               *p_dsnh_offset = p_ipuc_info->dsnh_info.dsnh_offset;
            }
            break;

    case SYS_HUMBER_NH_TYPE_IP_TUNNEL:
            {
               sys_nh_info_ip_tunnel_t *p_ip_tunnel_info = (sys_nh_info_ip_tunnel_t *)p_nhinfo;
               *p_dsnh_offset = p_ip_tunnel_info->dsnh_offset;
            }
            break;

    case SYS_HUMBER_NH_TYPE_FLEX:
            {
               sys_nh_info_flex_t *p_flex_info = (sys_nh_info_flex_t *)p_nhinfo;

                if(CTC_FLAG_ISSET(p_flex_info->hdr.nh_entry_flags, SYS_NH_INFO_FLEX_FLAG_USE_DSNH8W))
                {
                   *p_use_dsnh8w = TRUE;
                }
               *p_dsnh_offset = p_flex_info->dsnh_info.dsnh_offset;
            }
            break;

    default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


extern int32
sys_humber_nh_get_dsnh_offset_by_nhid_downmep(uint32 nhid, uint8 lchip,uint32 *p_dsnh_offset)
{
   sys_nh_info_com_t *p_nhinfo;
   sys_nh_info_downmep_t *p_downmep_info = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

    p_downmep_info = (sys_nh_info_downmep_t *)p_nhinfo;
   *p_dsnh_offset = p_downmep_info->p_dsnh_info[lchip].dsnh_offset;

    return CTC_E_NONE;
}

/**
 @brief This function is used to get destport by NexthopID

 @param[in] nhid, Nexthop ID

 @param[out] gport, gport

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_port(uint32 nhid, uint8 *aps_en,uint16 *gport)
{
    sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

   *aps_en = 0;

    switch(p_nhinfo->hdr.nh_entry_type)
    {
     case SYS_HUMBER_NH_TYPE_BRGUC:
         {
                sys_nh_info_brguc_t  *p_brguc_info;
                p_brguc_info = (sys_nh_info_brguc_t *)p_nhinfo;
                if((p_brguc_info->nh_sub_type ==SYS_NH_PARAM_BRGUC_SUB_TYPE_BASIC || p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_VLAN_EDIT
                || p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_BYPASS || p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU
                || p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED||p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE))
                {
                    *gport = p_brguc_info->dest.dest_gport;
                }
                else if(p_brguc_info->nh_sub_type == SYS_NH_PARAM_BRGUC_SUB_TYPE_APS_VLAN_EDIT)
                {
                   *gport = p_brguc_info->dest.aps_bridge_group_id;
                   *aps_en = 1;

                }
                else
                {
                      return CTC_E_INVALID_PARAM ;
                }
         }
          break;
    case SYS_HUMBER_NH_TYPE_IPUC:
            {
                sys_nh_info_ipuc_t *p_ipuc_info;
                p_ipuc_info = (sys_nh_info_ipuc_t *)(p_nhinfo);
                if (CTC_FLAG_ISSET(p_ipuc_info->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
                {
                    *gport = CTC_MAX_UINT16_VALUE;
                }
                else
                {
                    *gport = p_ipuc_info->gport;
                }
                *aps_en = FALSE;
            }
            break;
    case SYS_HUMBER_NH_TYPE_MPLS:
            {
                  sys_nh_info_mpls_t *p_mpls_info;
                  p_mpls_info = (sys_nh_info_mpls_t *)(p_nhinfo);

                 if(CTC_FLAG_ISSET(p_mpls_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_IS_UNROV))
                  {
                       *gport = CTC_MAX_UINT16_VALUE;
                       *aps_en = 0;
                  }
                 else if  (CTC_FLAG_ISSET(p_mpls_info->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_APS_EN))
                  {
                         *gport = p_mpls_info->aps_group_id;
                          *aps_en = 1;
                  }
                  else
                  {
                       *gport = p_mpls_info->working_path.gport;
                       *aps_en = 0;
                  }
            }
            break;
    case  SYS_HUMBER_NH_TYPE_DROP:
    case  SYS_HUMBER_NH_TYPE_TOCPU:
            {
                 *gport = CTC_MAX_UINT16_VALUE;
                 *aps_en = 0;
            }
            break;
    case SYS_HUMBER_NH_TYPE_IP_TUNNEL:
            {
                sys_nh_info_ip_tunnel_t *p_ip_tunnel_info;
                p_ip_tunnel_info = (sys_nh_info_ip_tunnel_t *)(p_nhinfo);
                if (CTC_FLAG_ISSET(p_ip_tunnel_info->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
                {
                    *gport = CTC_MAX_UINT16_VALUE;
                }
                else
                {
                    *gport = p_ip_tunnel_info->ip_tunnel_edit_info.gport;
                }
                *aps_en = FALSE;
            }
            break;
    case SYS_HUMBER_NH_TYPE_FLEX:
            {
                sys_nh_info_flex_t *p_flex_info;
                p_flex_info = (sys_nh_info_flex_t *)(p_nhinfo);
                if (CTC_FLAG_ISSET(p_flex_info->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
                {
                    *gport = CTC_MAX_UINT16_VALUE;
                }
                else
                {
                    *gport = p_flex_info->gport;
                }
                *aps_en = FALSE;
            }
            break;
    default :
        return CTC_E_INVALID_PARAM;
    }

   return CTC_E_NONE;
}




/**
 @brief This function is used to allocate a new internal nexthop id

 @param[out] p_nhid, New allocated nexthop id

 @return CTC_E_XXX
 */
static INLINE int32
_sys_humber_nh_new_nh_id(uint32 * p_nhid)
{
    sys_humber_opf_t opf;

    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_nhid);
    opf.pool_type = NHID_INTERNAL;
    opf.pool_index = 0;

    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, p_nhid));
    SYS_NH_DBG_INFO("New allocated nhid = %d\n", *p_nhid);

    return CTC_E_NONE;
};

/**
 @brief This function is used to free a nexthop inforamtion data

 @param[in] nhid, nexthop id to be freed

 @return CTC_E_XXX
 */
int32
sys_humber_nh_info_free(uint32 nhid)
{
    sys_humber_opf_t opf;
    sys_nh_info_com_t* p_nhinfo = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("Need freed nhid = %d\n", nhid);
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    if( (nhid >= p_nh_master->max_external_nhid) &&
        (nhid < SYS_HUMBER_NH_INTERNAL_NHID_MAX) )
    {
        opf.pool_type = NHID_INTERNAL;
        opf.pool_index = 0;
        CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, nhid));
        p_nhinfo = (sys_nh_info_com_t*)ctc_vector_del(p_nh_master->internal_nhid_vec,
            (nhid - p_nh_master->max_external_nhid));
    }
    else if(nhid < p_nh_master->max_external_nhid)
    {
        p_nhinfo = (sys_nh_info_com_t*)ctc_vector_del(p_nh_master->external_nhid_vec,nhid);
    }
    else
        return CTC_E_INVALID_PARAM;

    mem_free(p_nhinfo);

    return CTC_E_NONE;
};

/**
 @brief This function is used to get resolved offset

 @param[in] type, Resolved offset type

 @param[out] p_offset, the offset to be got

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_resolved_offset(sys_humber_nh_res_offset_type_t type, uint32* p_offset)
{
    sys_nh_offset_attr_t* p_res_offset;
    sys_humber_nh_master_t* p_nh_master = NULL;
    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_offset);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    /*Always get chip 0's offset, all chip should be same,
    maybe we don't need to store this value per-chip*/
    p_res_offset = &(p_nh_master->sys_hbnh_resolved_offset[0][type]);
    *p_offset = p_res_offset->offset_base;
    SYS_NH_DBG_INFO("Type = %d, resolved offset = %d\n", type, *p_offset);

    return CTC_E_NONE;
}


/**
 @brief This function is used to update port_crscnt nexthop

 @param[in] p_crscnt_param, point to sys_nh_param_crscnt_t

 @return CTC_E_XXX
 */
int32
sys_humber_nh_update_port_crscnt_nexthop(sys_nh_param_crscnt_t *p_crscnt_param)
{
    sys_humber_nh_master_t* p_nh_master = NULL;
    uint8 lchip, lport;
    uint32 crscnt_dsfwd_ptr;
    uint32 dsnh_ptr;
    uint16 destport = 0;
    uint16 gport = 0;

    sys_nh_param_dsfwd_t dsfwd_param;
    sys_nh_info_dsfwd_t dsfwd_info;


    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_crscnt_param);
    kal_memset(&dsfwd_param, 0, sizeof(sys_nh_param_dsfwd_t));
    kal_memset(&dsfwd_info, 0, sizeof(sys_nh_info_dsfwd_t));


    gport = p_crscnt_param->srcport;
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    SYS_NH_LOCK(p_nh_master->p_mutex);
    crscnt_dsfwd_ptr =  p_nh_master->port_crscnt_base[lchip] + lport;

     if (p_crscnt_param->swap_mac)
     {
         CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_SWAPMAC_NH, &dsnh_ptr),
            p_nh_master->p_mutex);
     }
     else
     {
         CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH, &dsnh_ptr),
            p_nh_master->p_mutex);
     }

    destport = p_crscnt_param->destport;

    dsfwd_param.lchip = lchip;
    dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(destport);;
    dsfwd_param.dest_chipid = CTC_MAP_GPORT_TO_GCHIP(destport);
    dsfwd_param.dsnh_offset = dsnh_ptr;
    dsfwd_param.p_dsfwd_info = &dsfwd_info;
    dsfwd_info.dsfwd_offset = crscnt_dsfwd_ptr;

    CTC_ERROR_RETURN_WITH_UNLOCK(
        sys_humber_nh_write_entry_dsfwd(&dsfwd_param),
         p_nh_master->p_mutex);

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}


/**
 @brief This function is used to alloc dynamic offset

 @param[in] lchip, the local chip id of the opeartion

 @param[in] entry_type, entry type

 @param[in] entry_num, entry number to be allocated

 @param[out] p_offset, the allocated offset to be return

 @return CTC_E_XXX
 */
int32
sys_humber_nh_offset_alloc(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint32* p_offset)
{
    sys_humber_opf_t opf;
    uint32 entry_size;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_offset);

    opf.pool_index = lchip;
    entry_size = nh_table_info_array[entry_type].entry_size;
    opf.pool_type = nh_table_info_array[entry_type].opf_pool_type;
    entry_size = entry_size * entry_num;

    if (nh_table_info_array[entry_type].alloc_dir == 1)
    {
        CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_offset(&opf, entry_size, p_offset));
    }
    else
    {
       CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, entry_size, p_offset));
    }

    SYS_NH_DBG_INFO("lchip = %d, entry_type = %d, entry_number = %d, offset = 0x%x\n",
            lchip, entry_type, entry_num, *p_offset);
    return CTC_E_NONE;
}

int32
sys_humber_nh_offset_alloc_with_multiple(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint16 multi, uint32* p_offset)
{
    sys_humber_opf_t opf;
    uint32 entry_size;
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_offset);

    opf.pool_index = lchip;
    entry_size = nh_table_info_array[entry_type].entry_size;
    opf.pool_type = nh_table_info_array[entry_type].opf_pool_type;
    entry_size = entry_size * entry_num;
    if (nh_table_info_array[entry_type].alloc_dir == 1)
    {
        CTC_ERROR_RETURN(sys_humber_opf_alloc_multiple_offset(&opf, multi, entry_size, p_offset));
    }
    else
    {
           CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_multiple_offset(&opf, multi, entry_size, p_offset));
    }

    SYS_NH_DBG_INFO("lchip = %d, entry_type = %d, entry_number = %d, multi = %d, offset = 0x%x\n",
            lchip, entry_type, entry_num, multi, *p_offset);

    return CTC_E_NONE;
}

int32
sys_humber_nh_reverse_offset_alloc(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint32* p_offset)
{
    sys_humber_opf_t opf;
    uint32 entry_size;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_offset);

    opf.pool_index = lchip;
    entry_size = nh_table_info_array[entry_type].entry_size;
    opf.pool_type = nh_table_info_array[entry_type].opf_pool_type;
    entry_size = entry_size * entry_num;

    CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_offset(&opf, entry_size, p_offset));

    SYS_NH_DBG_INFO("lchip = %d, entry_type = %d, entry_number = %d, offset = 0x%x\n",
            lchip, entry_type, entry_num, *p_offset);
    return CTC_E_NONE;
}

int32
sys_humber_nh_reverse_offset_alloc_with_multiple(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                           uint32 entry_num, uint16 multi, uint32* p_offset)
{
    sys_humber_opf_t opf;
    uint32 entry_size;
    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_offset);

    opf.pool_index = lchip;
    entry_size = nh_table_info_array[entry_type].entry_size;
    opf.pool_type = nh_table_info_array[entry_type].opf_pool_type;
    entry_size = entry_size * entry_num;
    CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_multiple_offset(&opf, multi, entry_size, p_offset));
    SYS_NH_DBG_INFO("lchip = %d, entry_type = %d, entry_number = %d, multi = %d, offset = 0x%x\n",
            lchip, entry_type, entry_num, multi, *p_offset);

    return CTC_E_NONE;
}

/**
 @brief This function is used to free dynamic offset

 @param[in] lchip, the local chip id of the opeartion

 @param[in] entry_type, entry type

 @param[in] entry_num, entry number to be allocated

 @param[in] offset, the offset to be freed

 @return CTC_E_XXX
 */
int32
sys_humber_nh_offset_free(uint8 lchip, sys_nh_entry_table_type_t entry_type,
                          uint32 entry_num, uint32 offset)
{
    sys_humber_opf_t opf;
    uint32 entry_size;

    SYS_NH_DBG_FUNC();
    SYS_NH_DBG_INFO("lchip = %d, entry_type = %d, entry_number = %d, offset = %d\n",
            lchip, entry_type, entry_num, offset);
    opf.pool_index = lchip;
    opf.pool_type = nh_table_info_array[entry_type].opf_pool_type;
    entry_size = nh_table_info_array[entry_type].entry_size;
    entry_size = entry_size * entry_num;

    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, entry_size, offset));

    return CTC_E_NONE;
}

/**
 @brief This function is used to create nexthop

 @param[in,out] p_nh_com_para, the parameters used to create nexthop,
                dsfwd_offset will be returned if exist,
                and if create internal nexthop, will return nhid,

 @return CTC_E_XXX
 */
int32
sys_humber_nh_api_create(sys_nh_param_com_t* p_nh_com_para)
{
    uint8 lchip = 0, lchip_num = 0;
    sys_humber_nh_type_t nh_type;
    p_sys_nh_create_cb_t nh_sem_map_cb;
    sys_nh_info_com_t* p_nhinfo = NULL;
    int32 ret = CTC_E_NONE;
    uint32 tmp_nh_id, db_entry_size = 0;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();
    /*1. Sanity check and init*/
    if(NULL == p_nh_com_para)
        return CTC_E_INVALID_PARAM;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);
    /*Check If this nexthop is exist*/
    if((!(p_nh_com_para->hdr.is_internal_nh)) &&
        (NULL != ctc_vector_get(p_nh_master->external_nhid_vec,p_nh_com_para->hdr.nhid)))
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NH_EXIST;
    }
    nh_type = p_nh_com_para->hdr.nh_param_type;
    ret = _sys_humber_nh_get_nhentry_size_by_nhtype(nh_type, &db_entry_size);
    if(ret)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_INVALID_NH_TYPE;
    }
    p_nhinfo = mem_malloc(MEM_NEXTHOP_MODULE, db_entry_size);
    if(NULL == p_nhinfo)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_nhinfo, 0, db_entry_size);

    /*2. Semantic mapping Callback*/
    nh_sem_map_cb = p_nh_master->callbacks_nh_create[nh_type];
    ret = (*nh_sem_map_cb)(p_nh_com_para, p_nhinfo);
    if(ret)
    {
        mem_free(p_nhinfo);
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return ret;
    }

    /*init stats_ptr.0xffff indicates invalid*/
    lchip_num = sys_humber_get_local_chip_num();
    for(lchip=0; lchip<lchip_num; lchip++)
    {
        p_nhinfo->hdr.dsfwd_info[lchip].stats_ptr = 0xFFFF;
    }

    /*3. Store nh infor into nh_array*/
    if(p_nh_com_para->hdr.is_internal_nh)
    {
        /*alloc a new nhid*/
        ret = ret ? ret : _sys_humber_nh_new_nh_id(&tmp_nh_id);
        if(FALSE == ctc_vector_add(p_nh_master->internal_nhid_vec,
                (tmp_nh_id - p_nh_master->max_external_nhid ), p_nhinfo))
        {
            SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_NO_MEMORY;
        }
        p_nh_com_para->hdr.nhid = tmp_nh_id;/*Write back the nhid*/
    }
    else
    {
        if(FALSE == ctc_vector_add(p_nh_master->external_nhid_vec,
                (p_nh_com_para->hdr.nhid ), p_nhinfo))
        {
            SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_NO_MEMORY;
        }
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return ret;
};

/**
 @brief This function is used to delete nexthop

 @param[in] nhid, nexthop id

 @param[in] p_nh_db, the pointer used to store nexthop information

 @return CTC_E_XXX
 */
int32
sys_humber_nh_api_delete(uint32 nhid, sys_humber_nh_type_t nhid_type)
{
    p_sys_nh_delete_cb_t nh_del_cb;
    sys_humber_nh_type_t nh_type;
    sys_nh_info_com_t *p_nh_info = NULL ;
    int32 ret = CTC_E_NONE;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    nh_type = p_nh_info->hdr.nh_entry_type;
    nh_del_cb = p_nh_master->callbacks_nh_delete[nh_type];
    /*p_nh_info will be freed in the callback function*/
    ret = (*nh_del_cb)(p_nh_info);

    ret = ret ? ret : sys_humber_nh_info_free(nhid);

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return ret;
};

/**
 @brief This function is used to update nexthop

 @param[in] nhid, nexthop id

 @param[in] p_nh_db, the pointer used to store nexthop information

 @param[in] p_nh_com_para, parameters used to update the nexthop

 @return CTC_E_XXX
 */
int32
sys_humber_nh_api_update(uint32 nhid, sys_nh_param_com_t* p_nh_com_para)
{
    p_sys_nh_update_cb_t nh_update_cb;
    sys_humber_nh_type_t nh_type;
    sys_nh_info_com_t *p_nh_info;
    int32 ret = CTC_E_NONE;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    nh_type = p_nh_com_para->hdr.nh_param_type;
    if((nh_type != p_nh_info->hdr.nh_entry_type) && ( SYS_HUMBER_NH_TYPE_IP_TUNNEL != p_nh_info->hdr.nh_entry_type))
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NHID_NOT_MATCH_NHTYPE;
    }
    nh_update_cb = p_nh_master->callbacks_nh_update[nh_type];
    ret = (*nh_update_cb)(p_nh_info, p_nh_com_para);

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return ret;
};

/**
 @brief This function is used to get dsFwd offset array

 @param[in] nhid, nexthop id

 @param[in] offset_array, dsfwd offset array

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_dsfwd_offset(uint32 nhid,
        sys_nh_offset_array_t offset_array)
{
    sys_nh_info_com_t *p_nhinfo;
    int32 ret = 0;
    uint8 lchip;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo),
        p_nh_master->p_mutex);

    for (lchip = 0; lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip ++)
    {
        offset_array[lchip] =  p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return ret;
}

/**
 @brief This function is used to get l3ifid

 @param[in] nhid, nexthop id

 @param[out] p_l3ifid, l3 interface id

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_l3ifid(uint32 nhid, uint16 *p_l3ifid)
{
    sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_info_ipuc_t *p_ipuc_info;

    CTC_PTR_VALID_CHECK(p_l3ifid);
    SYS_NH_DBG_FUNC();

    if (nhid == SYS_HUMBER_NH_RESOLVED_NHID_FOR_DROP ||
        nhid == SYS_HUMBER_NH_RESOLVED_NHID_FOR_TOCPU)
    {
        *p_l3ifid = SYS_L3IF_INVALID_L3IF_ID;
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo),
        p_nh_master->p_mutex);

    switch(p_nhinfo->hdr.nh_entry_type)
    {
        case SYS_HUMBER_NH_TYPE_IPUC:
            p_ipuc_info = (sys_nh_info_ipuc_t *)(p_nhinfo);
            *p_l3ifid = p_ipuc_info->l3ifid;
            break;

        case SYS_HUMBER_NH_TYPE_MPLS:
        {
            sys_nh_info_mpls_t *p_mpls_info;
            p_mpls_info = (sys_nh_info_mpls_t *)(p_nhinfo);
            *p_l3ifid = p_mpls_info->working_path.l3ifid;
            break;
        }

        case SYS_HUMBER_NH_TYPE_IP_TUNNEL:
        {
            sys_nh_info_ip_tunnel_t *p_ip_tunnel_info;
            p_ip_tunnel_info = (sys_nh_info_ip_tunnel_t *)(p_nhinfo);
            *p_l3ifid = p_ip_tunnel_info->ip_tunnel_edit_info.l3ifid;
            break;
        }

        default:
            SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_INVALID_NH_TYPE;
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}
extern int32
sys_humber_nh_get_statsptr(uint32 nhid,  sys_nh_u16_array_t stats_ptr)
{
  sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;

    SYS_NH_DBG_FUNC();

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo),
        p_nh_master->p_mutex);

    switch(p_nhinfo->hdr.nh_entry_type)
    {
     case SYS_HUMBER_NH_TYPE_MCAST:
     case SYS_HUMBER_NH_TYPE_BRGUC:
            stats_ptr[0] = p_nhinfo->hdr.dsfwd_info[0].stats_ptr;
            stats_ptr[1] = p_nhinfo->hdr.dsfwd_info[1].stats_ptr;
            break;
        default:
             SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_INVALID_NH_TYPE;
    }
    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}
int32
sys_humber_nh_get_l3ifid_nolock(uint32 nhid, uint16 *p_l3ifid)
{
    sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;
    sys_nh_info_ipuc_t *p_ipuc_info;

    CTC_PTR_VALID_CHECK(p_l3ifid);
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

    switch(p_nhinfo->hdr.nh_entry_type)
    {
        case SYS_HUMBER_NH_TYPE_IPUC:
            p_ipuc_info = (sys_nh_info_ipuc_t *)(p_nhinfo);
            *p_l3ifid = p_ipuc_info->l3ifid;
            break;

        case SYS_HUMBER_NH_TYPE_MPLS:
        {
            sys_nh_info_mpls_t *p_mpls_info;
            p_mpls_info = (sys_nh_info_mpls_t *)(p_nhinfo);
            *p_l3ifid = p_mpls_info->working_path.l3ifid;
            break;
        }

        default:
            return CTC_E_INVALID_NH_TYPE;
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_get_dsfwd_offset_nolock(uint32 nhid,
        sys_nh_offset_array_t offset_array)
{
    sys_nh_info_com_t *p_nhinfo;
    int32 ret = 0;
    uint8 lchip;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

    for (lchip = 0; lchip < MAX_LOCAL_CHIP_NUM; lchip ++)
    {
        offset_array[lchip] =  p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    return ret;
}

/**
 @brief This function is used to get nexthop entry flags

 @param[in] nh_id, nexthop id

 @param[out] p_l3ifid, l3 interface id

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_flags_nolock(uint32 nhid, uint16 *p_nh_flags)
{
    sys_nh_info_com_t *p_nhinfo;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_PTR_VALID_CHECK(p_nh_flags);
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    CTC_ERROR_RETURN(_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nhinfo));

    *p_nh_flags = p_nhinfo->hdr.nh_entry_flags;

    return CTC_E_NONE;
}

/**
 @brief This function is used to get bypass dsnexthop offset

 @param[in] p_offset, bypass dsnexthop offset

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_bypass_dsnh_offset(uint32 *p_offset)
{
    int32 ret;

    CTC_PTR_VALID_CHECK(p_offset);
    ret = sys_humber_nh_get_resolved_offset(
          SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH, p_offset);

    return ret;
}

/**
 @brief This function is used to get mirror dsnexthop offset

 @param[in] p_offset, mirror dsnexthop offset

 @return CTC_E_XXX
 */
int32
sys_humber_nh_get_mirror_dsnh_offset(uint32 *p_offset)
{
    int32 ret;

    CTC_PTR_VALID_CHECK(p_offset);
    ret = sys_humber_nh_get_resolved_offset(
          SYS_HBNH_RES_OFFSET_TYPE_MIRROR_NH, p_offset);

    return ret;
}

int32
sys_humber_nh_get_ipmc_phyif_rsv_l2edit_offset(uint8 lchip, uint32 *p_l2edit_offset)
{
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

   *p_l2edit_offset = p_nh_master->ipmc_phyif_resolved_l2edit[lchip];

    return CTC_E_NONE;
}


int32
sys_humber_nh_add_remove_dsnh_offset_by_vlan(uint16 vlan_id, bool is_add)
{
    ds_nexthop_t dsnh;
    uint32 cmd = 0;
    uint32 dsnh_offset = 0;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    sys_vlan_info_t vlan_info;
    uint16 vlan_ptr = 0;

    sys_humber_nh_master_t* p_nh_master = NULL;
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (is_add)
        {
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_NEXTHOP_4W, 1, &dsnh_offset));
            SYS_NH_DBG_INFO("Vlan offset alloc:%x\n", dsnh_offset);
            kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
            dsnh.replace_ctag_cos  = 0;
            dsnh.copy_ctag_cos     = 0;
            dsnh.derive_stag_cos   = 1;
            dsnh.stag_cfi          = 1;
            dsnh.cvlan_tagged      = 1;
            dsnh.svlan_tagged      = 1;
            dsnh.replace_dscp      = 1;
            dsnh.payload_operation = SYS_NH_OP_BRIDGE;

            kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
            vlan_info.vid = vlan_id;
            sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr);

            dsnh.dest_vlan_ptr     = vlan_ptr;
            cmd = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsnh_offset, cmd, &dsnh));

            p_nh_master->p_dsnh_offset_by_vlan[lchip][vlan_id] = dsnh_offset;
        }
        else
        {

            dsnh_offset = p_nh_master->p_dsnh_offset_by_vlan[lchip][vlan_id];
            CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_NEXTHOP_4W, 1, dsnh_offset));
            SYS_NH_DBG_INFO("Vlan offset free:%x\n", dsnh_offset);
            kal_memset(&dsnh, 0, sizeof(ds_nexthop_t));
            cmd = DRV_IOW(IOC_TABLE, DS_NEXTHOP, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsnh_offset, cmd, &dsnh));

            p_nh_master->p_dsnh_offset_by_vlan[lchip][vlan_id] = 0xFFFFFFFF;
        }
    }

    return CTC_E_NONE;
}


int32
sys_humber_nh_get_dsnh_offset_by_vlan(uint16 vlan_id, uint8 lchip, uint32 *p_offset)
{

    sys_humber_nh_master_t* p_nh_master = NULL;
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));

    if (0xFFFFFFFF != p_nh_master->p_dsnh_offset_by_vlan[lchip][vlan_id])
    {
        *p_offset = p_nh_master->p_dsnh_offset_by_vlan[lchip][vlan_id];
        return CTC_E_NONE;
    }

    return CTC_E_UNEXPECT;
}



/**
 @brief This function is used to get max global nh offset

 @param[in] offset, offset to be checked

 @return CTC_E_XXX
 */
int32
sys_humber_nh_check_max_glb_nh_sram_offset(uint32 offset)
{
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if(offset > (p_nh_master->max_glb_nh_sram_offset - SYS_HBNH_RES_DSNH_PTR_MAX))
        return CTC_E_GLB_SRAM_INDEX_EXCEED;

    return CTC_E_NONE;
}

/**
 @brief This function is used to get max global met offset

 @param[in] offset, offset to be checked

 @return CTC_E_XXX
 */
int32
sys_humber_nh_check_max_glb_met_sram_offset(uint32 offset)
{
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    if(offset > p_nh_master->max_glb_met_sram_offset)
        return CTC_E_GLB_SRAM_INDEX_EXCEED;

    return CTC_E_NONE;
}


/**
 @brief This function is used to initilize nexthop module

 @param[in] dyn_tbl_timeout, microsecond, used for delay free

 @return CTC_E_XXX
 */
int32
sys_humber_nh_init(ctc_nh_global_cfg_t * nh_cfg)
{
    sys_humber_nh_master_t* p_master;
    int32 ret = 0;

    SYS_NH_DBG_FUNC();
    /*Nexthop module have initialize*/
    if(NULL != (p_master = p_sys_humber_nh_master))
        return CTC_E_NONE;

    /*1. Create master*/
    CTC_ERROR_RETURN(_sys_humber_nh_master_new(&p_master,nh_cfg->max_external_nhid));

    p_sys_humber_nh_master = p_master;
    p_sys_humber_nh_master->max_external_nhid = nh_cfg->max_external_nhid;
    p_sys_humber_nh_master->acl_redirect_fwd_ptr_num = nh_cfg->acl_redirect_fwd_ptr_num;
    p_sys_humber_nh_master->local_met_num = nh_cfg->local_met_num;
    p_sys_humber_nh_master->local_nh_4w_num = nh_cfg->local_nh_4w_num;
    p_sys_humber_nh_master->l2_edit_4w_num = nh_cfg->l2_edit_4w_num;
    p_sys_humber_nh_master->l3_edit_4w_num = nh_cfg->l3_edit_4w_num;
    /*2. Install Semantic callback*/
    if (CTC_E_NONE != _sys_humber_nh_callback_init(p_master))
    {
        p_sys_humber_nh_master = NULL;
    }

    /*3. Offset  init*/
    ret = _sys_humber_nh_offset_init(p_master);
    if (ret < 0)
    {
        p_sys_humber_nh_master = NULL;
        return ret;
    }
    return CTC_E_NONE;
}


/*Following function just for debug*/
int32 sys_humber_nh_debug_get_nh_master(sys_humber_nh_master_t** p_nh_master)
{
    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(p_nh_master));
    return CTC_E_NONE;
}

int32
sys_humber_nh_debug_get_nhinfo_by_nhid(uint32 nhid, sys_humber_nh_master_t* p_nh_master,
                                      sys_nh_info_com_t** pp_nhinfo)
{
    return (_sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, pp_nhinfo));
}

int32
sys_humber_nh_add_stats_action(uint32 nhid)
{
    uint8 lchip, lchip_num;
    int32 ret = CTC_E_NONE;
    uint32 offset;
    sys_nh_u16_array_t stats_ptr;
    ds_fwd_t dsfwd;
    sys_nh_info_com_t *p_nh_info = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    if(!p_nh_info)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        /*16'h0xFFFF indicates invalid*/
        if(SYS_HUMBER_INVALID_STATS_PTR != p_nh_info->hdr.dsfwd_info[lchip].stats_ptr)
        {
            SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_NH_STATS_EXIST;
        }
    }

    /*get stats ptr*/
    kal_memset(stats_ptr, 0xFF, sizeof(sys_nh_u16_array_t));
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        ret = sys_humber_stats_create_statsptr(lchip, 1, &stats_ptr[lchip]);
        if(CTC_E_NONE != ret)
        {
            break;
        }
        p_nh_info->hdr.dsfwd_info[lchip].stats_ptr = stats_ptr[lchip];
    }

    /*for rollback*/
    if(CTC_E_NONE != ret)
    {
        for(lchip = 0; lchip < lchip_num; lchip++)
        {
            if(SYS_HUMBER_INVALID_STATS_PTR != stats_ptr[lchip])
            {
                sys_humber_stats_delete_statsptr(lchip, 1, stats_ptr[lchip]);
                p_nh_info->hdr.dsfwd_info[lchip].stats_ptr = SYS_HUMBER_INVALID_STATS_PTR;
            }
        }

        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return ret;
    }

    /*write hw*/
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        offset = p_nh_info->hdr.dsfwd_info[lchip].dsfwd_offset;
        CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_get_asic_table(lchip, SYS_NH_ENTRY_TYPE_FWD, offset, &dsfwd),
            p_nh_master->p_mutex);

        dsfwd.stats_ptr_lower = stats_ptr[lchip] & 0xFFF;
        dsfwd.stats_ptr_upper = (stats_ptr[lchip] >> 12) & 0xF;
        dsfwd.stats_valid = 1;

        CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_write_asic_table(lchip, SYS_NH_ENTRY_TYPE_FWD, offset, &dsfwd),
            p_nh_master->p_mutex);
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);
    return CTC_E_NONE;
}

int32
sys_humber_nh_del_stats_action(uint32 nhid)
{
    uint8 lchip, lchip_num;
    uint16 stats_ptr;
    uint32 offset;
    ds_fwd_t dsfwd;
    sys_nh_info_com_t *p_nh_info = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    if(!p_nh_info)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        /*16'h0xFFFF indicates invalid*/
        if(SYS_HUMBER_INVALID_STATS_PTR == p_nh_info->hdr.dsfwd_info[lchip].stats_ptr)
        {
            SYS_NH_UNLOCK(p_nh_master->p_mutex);
            return CTC_E_NH_STATS_NOT_EXIST;
        }

        stats_ptr = p_nh_info->hdr.dsfwd_info[lchip].stats_ptr ;
        CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_stats_delete_statsptr( lchip, 1, stats_ptr),
            p_nh_master->p_mutex);
        p_nh_info->hdr.dsfwd_info[lchip].stats_ptr = SYS_HUMBER_INVALID_STATS_PTR;

        offset = p_nh_info->hdr.dsfwd_info[lchip].dsfwd_offset;
        CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_get_asic_table(lchip, SYS_NH_ENTRY_TYPE_FWD, offset, &dsfwd),
            p_nh_master->p_mutex);

        dsfwd.stats_ptr_lower = 0xFFF;
        dsfwd.stats_ptr_upper = 0xF;
        dsfwd.stats_valid = 0;

        CTC_ERROR_RETURN_WITH_UNLOCK(
            sys_humber_nh_write_asic_table(lchip, SYS_NH_ENTRY_TYPE_FWD, offset, &dsfwd),
            p_nh_master->p_mutex);
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}

int32
sys_humber_nh_get_stats_result(uint32 nhid, ctc_stats_basic_t* p_stats)
{
    uint8 lchip, lchip_num;
    sys_nh_info_com_t *p_nh_info = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    ctc_stats_basic_t stats;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);

    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    if(!p_nh_info)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        /*16'h0xFFFF indicates invalid*/
        if(SYS_HUMBER_INVALID_STATS_PTR != p_nh_info->hdr.dsfwd_info[lchip].stats_ptr)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(
                sys_humber_stats_get_flow_stats(lchip, p_nh_info->hdr.dsfwd_info[lchip].stats_ptr, &stats),
                p_nh_master->p_mutex);

            p_stats->byte_count += stats.byte_count;
            p_stats->packet_count += stats.packet_count;
        }
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}

int32
sys_humber_nh_reset_stats_result(uint32 nhid)
{
    uint8 lchip, lchip_num;
    sys_nh_info_com_t *p_nh_info = NULL;
    sys_humber_nh_master_t* p_nh_master = NULL;
    ctc_stats_basic_t stats;

    CTC_ERROR_RETURN(_sys_humber_nh_get_nh_master(&p_nh_master));
    SYS_NH_LOCK(p_nh_master->p_mutex);
    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));

    CTC_ERROR_RETURN_WITH_UNLOCK(
        _sys_humber_nh_api_get_nhinfo_by_nhid(nhid, p_nh_master, &p_nh_info),
        p_nh_master->p_mutex);

    if(!p_nh_info)
    {
        SYS_NH_UNLOCK(p_nh_master->p_mutex);
        return CTC_E_NH_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        /*16'h0xFFFF indicates invalid*/
        if(SYS_HUMBER_INVALID_STATS_PTR != p_nh_info->hdr.dsfwd_info[lchip].stats_ptr)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(
                sys_humber_stats_reset_flow_stats(lchip, p_nh_info->hdr.dsfwd_info[lchip].stats_ptr),
                p_nh_master->p_mutex);
        }
    }

    SYS_NH_UNLOCK(p_nh_master->p_mutex);

    return CTC_E_NONE;
}

int32
sys_humer_nh_get_max_external_nhid(uint32* nhid)
{
    SYS_NH_DBG_FUNC();
    /*Nexthop module initialize check*/
    if(NULL == p_sys_humber_nh_master)
        return CTC_E_NOT_INIT;

    *nhid = p_sys_humber_nh_master->max_external_nhid;
    return CTC_E_NONE;
}

