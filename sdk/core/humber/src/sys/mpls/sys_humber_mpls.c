/**
 @file sys_humber_mpls.c

 @date 2010-03-12

 @version v2.0

 The file contains all mpls related function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"

#include "ctc_mpls.h"
#include "ctc_vector.h"
#include "drv_humber.h"
#include "drv_io.h"
#include "sys_humber_chip.h"
#include "sys_humber_usrid.h"
#include "sys_humber_mpls.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_queue_api.h"
#include "sys_humber_ftm.h"
#include "sys_humber_usrid.h"
#include "sys_humber_vlan.h"
#include "sys_humber_nexthop.h"
#include "ctc_stats.h"
#include "sys_humber_stats.h"

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
sys_mpls_master_t *p_mpls_master = NULL;

#define SYS_MPLS_CREAT_LOCK                   \
{                                             \
    kal_mutex_create(&p_mpls_master->mutex); \
    if(NULL == p_mpls_master->mutex)  \
    { \
        CTC_ERROR_RETURN(CTC_E_FAIL_CREATE_MUTEX); \
    } \
}

#define SYS_MPLS_LOCK \
    kal_mutex_lock(p_mpls_master->mutex)

#define SYS_MPLS_UNLOCK \
    kal_mutex_unlock(p_mpls_master->mutex)

#define CTC_ERROR_RETURN_MPLS_UNLOCK(op) \
    { \
        int32 rv = (op); \
        if (rv < 0) \
        { \
            kal_mutex_unlock(p_mpls_master->mutex);\
            return rv; \
        } \
    }

#define CTC_RETURN_MPLS_UNLOCK(op) \
    { \
        kal_mutex_unlock(p_mpls_master->mutex);\
        return(op); \
    }



static int32
_sys_humber_mpls_ilm_normal(sys_mpls_ilm_t* p_ilm_info, void* mpls)
{
    ds_mpls_t *dsmpls = mpls;

    if(p_ilm_info->pop)
    {
        dsmpls->scontinue = TRUE;
    }

    dsmpls->offset_bytes = 1;
    dsmpls->equal_cost_path_num10 = p_ilm_info->ecpn & 0x3;
    dsmpls->equal_cost_path_num2 = p_ilm_info->ecpn >> 2 & 0x1;

    if(CTC_MPLS_TUNNEL_MODE_UNIFORM != p_ilm_info->model)
    {
        dsmpls->use_label_ttl = TRUE;
    }
    dsmpls->use_label_exp = TRUE;
    dsmpls->over_write_priority = TRUE;

    dsmpls->packet_type = PKT_TYPE_IPV4;

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_ilm_l3vpn(sys_mpls_ilm_t* p_ilm_info, void* mpls)
{
    ds_mpls_t *dsmpls = mpls;

    dsmpls->offset_bytes = 1;
    dsmpls->is_vc_label = TRUE;
    dsmpls->sbit = TRUE;
    dsmpls->s_bit_check_en = TRUE;


    if(CTC_MPLS_TUNNEL_MODE_UNIFORM != p_ilm_info->model)
    {
        dsmpls->use_label_ttl = TRUE;
    }
    dsmpls->use_label_exp = TRUE;
    dsmpls->over_write_priority = TRUE;

    dsmpls->packet_type = PKT_TYPE_IPV4;

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_ilm_vpws(sys_mpls_ilm_t* p_ilm_info, void* mpls)
{
    ds_mpls_t *dsmpls = mpls;
    sys_nh_info_com_t* p_nhinfo;
    uint8 iloop_flag = 0;

    dsmpls->is_vc_label = TRUE;
    dsmpls->sbit = TRUE;
    dsmpls->s_bit_check_en = TRUE;


    if(p_ilm_info->id_type == CTC_MPLS_ID_SERVICE)
    {
        CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(p_ilm_info->nh_id, &p_nhinfo));
        if(p_nhinfo->hdr.nh_entry_type == SYS_HUMBER_NH_TYPE_ILOOP)
        {
            iloop_flag = 1;
        }

        /* vpls source port, should be invalid 0xfff */
        dsmpls->ttl_threshhold = 0x7f;
        dsmpls->equal_cost_path_num2 = 0x1;
        dsmpls->equal_cost_path_num10 = 0x3;
        dsmpls->priority_path_en = 0x1;
        dsmpls->ttl_decrease_mode = 0x1;
    }

    if(iloop_flag == 1)
    {
        dsmpls->offset_bytes = 0;
    }
    else if (p_ilm_info->cwen)
    {
        dsmpls->offset_bytes = 2;
    }
    else
    {
        dsmpls->offset_bytes = 1;
    }

    dsmpls->use_label_ttl = TRUE;
    dsmpls->use_label_exp = TRUE;

    dsmpls->over_write_priority = TRUE;
    dsmpls->packet_type = PKT_TYPE_ETH;

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_ilm_vpls(sys_mpls_ilm_t* p_ilm_info, void* mpls)
{
    ds_mpls_t* dsmpls = mpls;
    dsmpls->is_vc_label = TRUE;
    dsmpls->sbit = TRUE;
    dsmpls->s_bit_check_en = TRUE;


    if(p_ilm_info->pwid <= SYS_MPLS_VPLS_SRC_PORT_NUM)
    {
        /* vpls source port, namely pwid*/
        dsmpls->ttl_threshhold = p_ilm_info->pwid & 0x7f;
        dsmpls->equal_cost_path_num2 = (p_ilm_info->pwid >> 7) & 0x1;
        dsmpls->equal_cost_path_num10 = (p_ilm_info->pwid >> 8) & 0x3;
        dsmpls->priority_path_en = (p_ilm_info->pwid >> 10) & 0x1;
        dsmpls->ttl_decrease_mode = (p_ilm_info->pwid >> 11) & 0x1;


    }

    dsmpls->offset_bytes = 0;

    dsmpls->use_label_ttl = TRUE;
    dsmpls->use_label_exp = TRUE;

    dsmpls->over_write_priority = TRUE;

    dsmpls->packet_type = PKT_TYPE_ETH;

    return CTC_E_NONE;
}


static int32
_sys_humber_mpls_write_ilm(sys_mpls_ilm_t* p_mpls_info, sys_nh_offset_array_t fwd_offset)
{
    ds_mpls_t dsmpls;
    uint8 chip_num;
    uint8 i;
    uint32 cmd;

    kal_memset(&dsmpls, 0, sizeof(ds_mpls_t));

    /* flow policer ptr */
    if(p_mpls_info->id_type == CTC_MPLS_ID_APS_SELECT)
    {
        dsmpls.aps_select_valid = 1;
        dsmpls.flow_policer_ptr7to0 = p_mpls_info->aps_group_id & 0xff;
        dsmpls.flow_policer_ptr10to8 = (p_mpls_info->aps_group_id >> 8) & 0x7;
        dsmpls.flow_policer_ptr11 = (p_mpls_info->aps_group_id >> 11) & 0x1;
        dsmpls.flow_policer_ptr15to12 = (p_mpls_info->aps_group_id >> 12) & 0x3;
        dsmpls.flow_policer_ptr15to12 |= 0x4;
        if(p_mpls_info->aps_select_ppath)
        {
            dsmpls.flow_policer_ptr15to12 |= 0x8;
        }
    }
    else if (p_mpls_info->id_type == CTC_MPLS_ID_FLOW)
    {
        extern int32 
            sys_humber_qos_policer_index_get(uint8 lchip, uint32 plc_id, uint32* p_index);
        
        uint32 policer_ptr;
        CTC_ERROR_RETURN(sys_humber_qos_policer_index_get(0, p_mpls_info->flw_vrf_srv, &policer_ptr));
        
        dsmpls.flow_policer_ptr7to0 = policer_ptr & 0xff;
        dsmpls.flow_policer_ptr10to8 = (policer_ptr >> 8) & 0x7;
        dsmpls.flow_policer_ptr11 = (policer_ptr >> 11) & 0x1;
        dsmpls.flow_policer_ptr15to12 = (policer_ptr >> 12) & 0xf;

    }
    else
    {
        dsmpls.flow_policer_ptr7to0 = p_mpls_info->flw_vrf_srv & 0xff;
        dsmpls.flow_policer_ptr10to8 = (p_mpls_info->flw_vrf_srv >> 8) & 0x7;
        dsmpls.flow_policer_ptr11 = (p_mpls_info->flw_vrf_srv >> 11) & 0x1;
        dsmpls.flow_policer_ptr15to12 = (p_mpls_info->flw_vrf_srv >> 12) & 0xf;
    }

    if (p_mpls_info->type != CTC_MPLS_LABEL_TYPE_NORMAL)
    {  //vc label

        if (p_mpls_info->cwen)
        {
            dsmpls.ttl_check_mode = 1;     /* control word exist */
        }

        if (p_mpls_info->id_type == CTC_MPLS_ID_VRF
            || p_mpls_info->id_type == CTC_MPLS_ID_SERVICE
          || ((p_mpls_info->type == CTC_MPLS_LABEL_TYPE_VPLS || p_mpls_info->type == CTC_MPLS_LABEL_TYPE_VPWS) &&  p_mpls_info->pwid <= SYS_MPLS_VPLS_SRC_PORT_NUM))
        {
            dsmpls.ttl_check_mode |= (1<<1);
        }
    }
    else
    {  //normal
        dsmpls.ttl_check_mode = (1<<1);  //only support global ttl check
    }

    dsmpls.oam_check = (p_mpls_info->oam_en) ? TRUE : FALSE;
    p_mpls_master->write_ilm[p_mpls_info->type](p_mpls_info, &dsmpls);

    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        dsmpls.ds_fwd_ptr = fwd_offset[i] & 0xfffff;
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_mpls_info->key_offset, cmd, &dsmpls));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_remove_ilm(sys_mpls_ilm_t* p_mpls_info)
{
    ds_mpls_t dsmpls;
    uint8 chip_num;
    uint8 i;
    uint32 cmd;

    kal_memset(&dsmpls, 0, sizeof(ds_mpls_t));
    dsmpls.flow_policer_ptr7to0 = 0xff;
    dsmpls.flow_policer_ptr10to8 = 0x7;
    dsmpls.flow_policer_ptr11 = 0x1;
    dsmpls.flow_policer_ptr15to12 = 0xf;
    dsmpls.ds_fwd_ptr = 0xfffff;

    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        drv_tbl_ioctl(i, p_mpls_info->key_offset, cmd, &dsmpls);
    }

    return CTC_E_NONE;
}

/**
 @brief function of lookup mpls ilm information

 @param[in] pp_mpls_info, information used for lookup mpls ilm entry

 @param[out] pp_mpls_info, information of mpls ilm entry finded

 @return CTC_E_XXX
 */
static int32
_sys_humber_mpls_db_lookup(sys_mpls_ilm_t** pp_mpls_info)
{
    sys_mpls_ilm_t* p_mpls_info;
    uint32 offset = (*pp_mpls_info)->label;

    /* get mpls entry index */
    if((*pp_mpls_info)->spaceid)
    {
        offset -= p_mpls_master->min_int_label;
    }


    p_mpls_info = ctc_vector_get(p_mpls_master->space[(*pp_mpls_info)->spaceid].p_vet, offset);


    *pp_mpls_info = p_mpls_info;

    return CTC_E_NONE;
}

/**
 @brief function of add mpls ilm information

 @param[in] p_mpls_info, information should be maintained for mpls

 @return CTC_E_XXX
 */
static int32
_sys_humber_mpls_db_add(sys_mpls_ilm_t* p_mpls_info)
{
    uint32 offset = p_mpls_info->label;

    /* get mpls entry index */
    if(p_mpls_info->spaceid)
    {
        offset -= p_mpls_master->min_int_label;
    }

    p_mpls_info->key_offset = p_mpls_master->space[p_mpls_info->spaceid].base + offset;
    CTC_ERROR_RETURN(ctc_vector_add(p_mpls_master->space[p_mpls_info->spaceid].p_vet, offset, p_mpls_info));

    return CTC_E_NONE;
}

/**
 @brief function of remove mpls ilm information

 @param[in] p_ipuc_info, information maintained by mpls

 @return CTC_E_XXX
 */
static int32
_sys_humber_mpls_db_remove(sys_mpls_ilm_t* p_mpls_info)
{
    uint32 offset = p_mpls_info->label;

    /* get mpls entry index */
    if(p_mpls_info->spaceid)
    {
        offset -= p_mpls_master->min_int_label;
    }

    ctc_vector_del(p_mpls_master->space[p_mpls_info->spaceid].p_vet, offset);

    return CTC_E_NONE;
}

/**
 @brief function of add mpls ilm entry

 @param[in] p_mpls_ilm, parameters used to add mpls ilm entry

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_add_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;
    int32 ret;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_mpls_ilm);
    SYS_MPLS_NHID_EXTERNAL_VALID_CHECK(p_mpls_ilm->nh_id);
    SYS_MPLS_ILM_SPACE_CHECK(p_mpls_ilm->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(p_mpls_ilm->spaceid, p_mpls_ilm->label);
    SYS_MPLS_ILM_DATA_CHECK(p_mpls_ilm);
    SYS_MPLS_ILM_DATA_MASK(p_mpls_ilm);

    if(p_mpls_ilm->id_type == CTC_MPLS_ID_SERVICE)
    {
        CTC_ERROR_RETURN(sys_humber_get_physical_service_id(p_mpls_ilm->flw_vrf_srv_aps.service_id, &p_mpls_ilm->flw_vrf_srv_aps.service_id));
        p_mpls_ilm->flw_vrf_srv_aps.service_id |= 0xC000;
    }

    /* prepare data */
    p_ilm_data = &ilm_data;
    SYS_MPLS_KEY_MAP(p_mpls_ilm, p_ilm_data);

    SYS_MPLS_LOCK;
    /* lookup for mpls entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(p_ilm_data) /* ilm have exist */
    {
        sys_nh_param_ecmp_api_t nh_param;

        /* VC label must not do ecmp */
        if(p_ilm_data->type != CTC_MPLS_LABEL_TYPE_NORMAL)
        {
            CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_EXIST);
        }

        if(p_ilm_data->id_type != p_mpls_ilm->id_type)
        {
            CTC_ERROR_RETURN_MPLS_UNLOCK(CTC_E_UNEXPECT);
        }

        if(p_ilm_data->ecpn >= CTC_MAX_ECPN - 1)
        {
            /* ecmp number exceed */
            CTC_RETURN_MPLS_UNLOCK(CTC_E_EXCEED_MAX_SIZE);
        }

        if(p_ilm_data->nh_id == p_mpls_ilm->nh_id)
        {
            /* this ilm have installed */
            CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_EXIST);
        }

        kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
        nh_param.ref_nh_id = p_mpls_ilm->nh_id;
        if(p_ilm_data->ecpn == 0)
        {
            CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_ecmp_nh_create(&nh_param, &p_ilm_data->nh_id));
        }
        else
        {
            nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM;

            CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_ecmp_nh_update(p_ilm_data->nh_id, &nh_param));
        }
        p_ilm_data->ecpn++;
        switch(p_mpls_ilm->id_type)
        {
            case CTC_MPLS_ID_FLOW:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.flow_id;
                break;
            case CTC_MPLS_ID_VRF:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.vrf_id;
                break;
            case CTC_MPLS_ID_SERVICE:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.service_id;
                break;
            case CTC_MPLS_ID_APS_SELECT:
                p_ilm_data->aps_group_id = p_mpls_ilm->flw_vrf_srv_aps.aps_select_grp_id;
                p_ilm_data->aps_select_ppath = p_mpls_ilm->aps_select_protect_path;
                break;
            default:
                p_ilm_data->flw_vrf_srv = 0xffff;
                break;
        }
        /* write mpls ilm entry */
        CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_write_ilm(p_ilm_data, nh_param.fwd_offset));
    }
    else
    {
        sys_nh_offset_array_t fwd_offset;

        SYS_MPLS_UNLOCK;

        CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_mpls_ilm->nh_id, fwd_offset));

        p_ilm_data = mem_malloc(MEM_MPLS_MODULE, sizeof(sys_mpls_ilm_t));
        if(NULL == p_ilm_data)
        {
            return CTC_E_NO_MEMORY;
        }

        SYS_MPLS_KEY_MAP(p_mpls_ilm, p_ilm_data);
        SYS_MPLS_DATA_MAP(p_mpls_ilm, p_ilm_data);

        p_ilm_data->ecpn = 0;
        switch(p_mpls_ilm->id_type)
        {
            case CTC_MPLS_ID_FLOW:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.flow_id;
                break;
            case CTC_MPLS_ID_VRF:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.vrf_id;
                break;
            case CTC_MPLS_ID_SERVICE:
                p_ilm_data->flw_vrf_srv = p_mpls_ilm->flw_vrf_srv_aps.service_id;
                break;
            case CTC_MPLS_ID_APS_SELECT:
                p_ilm_data->aps_group_id = p_mpls_ilm->flw_vrf_srv_aps.aps_select_grp_id;
                p_ilm_data->aps_select_ppath = p_mpls_ilm->aps_select_protect_path;
                break;
            default:
                p_ilm_data->flw_vrf_srv = 0xffff;
                break;
        }
        SYS_MPLS_LOCK;
        ret = _sys_humber_mpls_db_add(p_ilm_data);
        if(ret)
        {
            mem_free(p_ilm_data);
            CTC_ERROR_RETURN_MPLS_UNLOCK(ret);
        }

        /* write mpls ilm entry */
        ret = _sys_humber_mpls_write_ilm(p_ilm_data, fwd_offset);
        if(ret)
        {
            _sys_humber_mpls_db_remove(p_ilm_data);
            mem_free(p_ilm_data);
            CTC_ERROR_RETURN_MPLS_UNLOCK(ret);
        }
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_update_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;
    uint8 i;
    uint8 chip_num;
    uint32 cmd;
    uint32 use_label;
    uint32 oam_check;
    sys_nh_offset_array_t fwd_offset;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_mpls_ilm);
    SYS_MPLS_ILM_SPACE_CHECK(p_mpls_ilm->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(p_mpls_ilm->spaceid, p_mpls_ilm->label);
    if(p_mpls_ilm->model >= CTC_MPLS_MAX_TUNNEL_MODE)
        return CTC_E_INVALID_PARAM;

    /* prepare data */
    p_ilm_data = &ilm_data;
    SYS_MPLS_KEY_MAP(p_mpls_ilm, p_ilm_data);

    SYS_MPLS_LOCK;
    /* lookup for mpls entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(p_ilm_data) /* ilm have exist */
    {
        chip_num = sys_humber_get_local_chip_num();
        if(p_mpls_ilm->model != p_ilm_data->model)
        {
            if(CTC_MPLS_TUNNEL_MODE_UNIFORM != p_mpls_ilm->model)
            {
                use_label = TRUE;
            }
            else
            {
                use_label = FALSE;
            }

            cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DS_MPLS_USE_LABEL_TTL);
            for(i = 0; i < chip_num; i++)
            {
                CTC_ERROR_RETURN_MPLS_UNLOCK(drv_tbl_ioctl(i, p_ilm_data->key_offset, cmd, &use_label));
            }

            cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DS_MPLS_USE_LABEL_EXP);
            for(i = 0; i < chip_num; i++)
            {
                CTC_ERROR_RETURN_MPLS_UNLOCK(drv_tbl_ioctl(i, p_ilm_data->key_offset, cmd, &use_label));
            }

            p_ilm_data->model = p_mpls_ilm->model;
        }

        if (p_mpls_ilm->oam_en != p_ilm_data->oam_en)
        {
            oam_check = (p_mpls_ilm->oam_en) ? TRUE : FALSE;
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DS_MPLS_OAM_CHECK);
            for(i = 0; i < chip_num; i++)
            {
                CTC_ERROR_RETURN_MPLS_UNLOCK(drv_tbl_ioctl(i, p_ilm_data->key_offset, cmd, &oam_check));
                p_ilm_data->oam_en = p_mpls_ilm->oam_en;
            }
        }

        if (p_mpls_ilm->nh_id != p_ilm_data->nh_id)
        {
            CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_mpls_ilm->nh_id, fwd_offset));
            cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DS_MPLS_DS_FWD_PTR);
            for(i = 0; i < chip_num; i++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_ilm_data->key_offset, cmd, &fwd_offset[i]));
            }
            p_ilm_data->nh_id = p_mpls_ilm->nh_id;
        }
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief function of remove mpls ilm entry

 @param[in] p_ipuc_param, parameters used to remove mpls ilm entry

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_del_ilm(ctc_mpls_ilm_t* p_mpls_ilm)
{
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_mpls_ilm);
    SYS_MPLS_NHID_EXTERNAL_VALID_CHECK(p_mpls_ilm->nh_id);
    SYS_MPLS_ILM_SPACE_CHECK(p_mpls_ilm->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(p_mpls_ilm->spaceid, p_mpls_ilm->label);

    /* prepare data */
    p_ilm_data = &ilm_data;
    SYS_MPLS_KEY_MAP(p_mpls_ilm, p_ilm_data);

    SYS_MPLS_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(p_ilm_data->ecpn == 0)
    {
        if(p_ilm_data->nh_id != p_mpls_ilm->nh_id)
        {
            CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
        }

        /* write mpls ilm entry */
        _sys_humber_mpls_remove_ilm(p_ilm_data);

        _sys_humber_mpls_db_remove(p_ilm_data);

        mem_free(p_ilm_data);
    }
    else
    {
        sys_nh_param_ecmp_api_t nh_param;
        uint32 ecmp_nhid;
        kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
        nh_param.ref_nh_id = p_mpls_ilm->nh_id;
        nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_REMOVE_ITEM;
        ecmp_nhid = p_ilm_data->nh_id;

        CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_ecmp_nh_update(ecmp_nhid, &nh_param));
        p_ilm_data->ecpn--;

        if(p_ilm_data->ecpn == 0)
        {
            p_ilm_data->nh_id = nh_param.ref_nh_id;
        }

        /* write mpls ilm entry */
        _sys_humber_mpls_write_ilm(p_ilm_data, nh_param.fwd_offset);

        if(p_ilm_data->ecpn == 0)
        {
            CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_ecmp_nh_delete(ecmp_nhid));
        }
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_get_ilm(uint32 nh_id[CTC_MAX_ECPN], ctc_mpls_ilm_t* p_mpls_ilm)
{
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;
    uint8 i;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(nh_id);
    CTC_PTR_VALID_CHECK(p_mpls_ilm);
    SYS_MPLS_ILM_SPACE_CHECK(p_mpls_ilm->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(p_mpls_ilm->spaceid, p_mpls_ilm->label);

    /* prepare data */
    p_ilm_data = &ilm_data;
    SYS_MPLS_KEY_MAP(p_mpls_ilm, p_ilm_data);

    SYS_MPLS_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(p_ilm_data->ecpn != 0)
    {
        sys_nh_info_ecmp_t* p_nhinfo;

        CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_nh_get_nhinfo_by_nhid(p_ilm_data->nh_id, (sys_nh_info_com_t**)&p_nhinfo));
        if(SYS_HUMBER_NH_TYPE_ECMP != p_nhinfo->hdr.nh_entry_type)
        {
            CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
        }

        for(i = 0; i < p_nhinfo->valid_item_cnt; i++)
        {
            nh_id[i] = p_nhinfo->item_array[i].nh_id;
        }
    }
    else
    {
        nh_id[0] = p_ilm_data->nh_id;
    }

    p_mpls_ilm->pwid = p_ilm_data->pwid;
    p_mpls_ilm->id_type = p_ilm_data->id_type;
    p_mpls_ilm->type = p_ilm_data->type;
    p_mpls_ilm->model = p_ilm_data->model;
    p_mpls_ilm->cwen = p_ilm_data->cwen;
    p_mpls_ilm->pop = p_ilm_data->pop;
    p_mpls_ilm->nh_id = p_ilm_data->nh_id;
    p_mpls_ilm->id_type = p_ilm_data->id_type;

    switch(p_ilm_data->id_type)
    {
        case CTC_MPLS_ID_FLOW:
            p_mpls_ilm->flw_vrf_srv_aps.flow_id = p_ilm_data->flw_vrf_srv;
            break;
        case CTC_MPLS_ID_VRF:
            p_mpls_ilm->flw_vrf_srv_aps.vrf_id =  p_ilm_data->flw_vrf_srv;
            break;
        case CTC_MPLS_ID_SERVICE:
            p_mpls_ilm->flw_vrf_srv_aps.service_id =  p_ilm_data->flw_vrf_srv;
            break;
        case CTC_MPLS_ID_APS_SELECT:
            p_mpls_ilm->flw_vrf_srv_aps.aps_select_grp_id = p_ilm_data->aps_group_id;
            p_mpls_ilm->aps_select_protect_path =  p_ilm_data->aps_select_ppath;
            break;
        default:
            break;
    }

    for(i = p_ilm_data->ecpn + 1; i < CTC_MAX_ECPN; i++)
    {
        nh_id[i] = CTC_INVLD_NH_ID;
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_add_stats(ctc_mpls_stats_index_t* stats_index)
{
    uint8 lchip, lchip_num;
    sys_nh_u16_array_t stats_ptr;
    uint32 cmd;
    int32 ret = CTC_E_NONE;
    ds_mpls_t dsmpls;
    sys_mpls_ilm_t *p_ilm_data, ilm_data;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(stats_index);
    SYS_MPLS_ILM_SPACE_CHECK(stats_index->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(stats_index->spaceid, stats_index->label);

    /* prepare data */
    p_ilm_data = &ilm_data;
    p_ilm_data->label = stats_index->label;
    p_ilm_data->spaceid = stats_index->spaceid;

    SYS_MPLS_LOCK;
    /* lookup for mpls entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(CTC_MPLS_ID_STATS == p_ilm_data->id_type)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_MPLS_ENTRY_STATS_EXIST);
    }
    else if(CTC_MPLS_ID_NULL != p_ilm_data->id_type)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_MPLS_ENTRY_NOT_SUPPORT_STATS);
    }

    kal_memset(stats_ptr, 0xFF, sizeof(sys_nh_u16_array_t));
    lchip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        /*alloc stats_ptr*/
        ret = sys_humber_stats_create_statsptr(lchip, 1, &stats_ptr[lchip]);
        if(CTC_E_NONE != ret)
        {
            break;
        }

        p_ilm_data->stats_ptr[lchip] = stats_ptr[lchip];
    }

    /*for rollback*/
    if(CTC_E_NONE != ret)
    {
        for(lchip = 0; lchip < lchip_num; lchip++)
        {
            if(0xFFFF != stats_ptr[lchip])
            {
                sys_humber_stats_delete_statsptr(lchip, 1, stats_ptr[lchip]);
                p_ilm_data->stats_ptr[lchip] = 0xFFFF;
            }
        }

        CTC_ERROR_RETURN_MPLS_UNLOCK(ret);
    }

    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOR(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN_MPLS_UNLOCK(drv_tbl_ioctl(lchip, p_ilm_data->key_offset, cmd, &dsmpls));

        dsmpls.stats_ptr_mode = 1;
        dsmpls.flow_policer_ptr7to0 = stats_ptr[lchip] & 0xff;
        dsmpls.flow_policer_ptr10to8 = (stats_ptr[lchip] >> 8) & 0x7;
        dsmpls.flow_policer_ptr11 = (stats_ptr[lchip] >> 11) & 0x1;
        dsmpls.flow_policer_ptr15to12 = (stats_ptr[lchip] >> 12) & 0xf;

        cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN_MPLS_UNLOCK(drv_tbl_ioctl(lchip, p_ilm_data->key_offset, cmd, &dsmpls));
    }

    p_ilm_data->id_type = CTC_MPLS_ID_STATS;

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_del_stats(ctc_mpls_stats_index_t* stats_index)
{
    uint8 lchip, lchip_num;
    uint16 stats_ptr;
    uint32 cmd;
    ds_mpls_t dsmpls;
    sys_mpls_ilm_t *p_ilm_data, ilm_data;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(stats_index);
    SYS_MPLS_ILM_SPACE_CHECK(stats_index->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(stats_index->spaceid, stats_index->label);

    /* prepare data */
    p_ilm_data = &ilm_data;
    p_ilm_data->label = stats_index->label;
    p_ilm_data->spaceid = stats_index->spaceid;

    SYS_MPLS_LOCK;
    /* lookup for mpls entrise */
    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(CTC_MPLS_ID_STATS != p_ilm_data->id_type)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_MPLS_ENTRY_STATS_NOT_EXIST);
    }

    lchip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        cmd = DRV_IOR(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
        drv_tbl_ioctl(lchip, p_ilm_data->key_offset, cmd, &dsmpls);

        /*free stats_ptr*/
        stats_ptr = (dsmpls.flow_policer_ptr7to0 & 0xff) | ((dsmpls.flow_policer_ptr10to8 & 0x7)<<8) |
                    ((dsmpls.flow_policer_ptr11 & 0x1)<<11) | ((dsmpls.flow_policer_ptr15to12 &0xf)<<12);
        sys_humber_stats_delete_statsptr(lchip, 1, stats_ptr);

        dsmpls.stats_ptr_mode = 0;
        /*16'hFFFF indicates invalid*/
        dsmpls.flow_policer_ptr7to0 = 0xff;
        dsmpls.flow_policer_ptr10to8 = 0x7;
        dsmpls.flow_policer_ptr11 = 0x1;
        dsmpls.flow_policer_ptr15to12 = 0xf;

        cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);
        drv_tbl_ioctl(lchip, p_ilm_data->key_offset, cmd, &dsmpls);

        p_ilm_data->stats_ptr[lchip] = 0xFFFF;
    }

    p_ilm_data->id_type = CTC_MPLS_ID_NULL;

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_get_stats(ctc_mpls_stats_index_t* stats_index, ctc_stats_basic_t* p_stats)
{
    uint8 lchip, lchip_num;
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;
    ctc_stats_basic_t stats;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(stats_index);
    CTC_PTR_VALID_CHECK(p_stats);
    SYS_MPLS_ILM_SPACE_CHECK(stats_index->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(stats_index->spaceid, stats_index->label);
    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    p_ilm_data = &ilm_data;
    p_ilm_data->label = stats_index->label;
    p_ilm_data->spaceid = stats_index->spaceid;

    SYS_MPLS_LOCK;

    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(CTC_MPLS_ID_STATS != p_ilm_data->id_type)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_MPLS_ENTRY_NOT_SUPPORT_STATS);
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_stats_get_flow_stats(lchip, p_ilm_data->stats_ptr[lchip], &stats));

        p_stats->byte_count += stats.byte_count;
        p_stats->packet_count += stats.packet_count;
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_mpls_reset_stats(ctc_mpls_stats_index_t* stats_index)
{
    uint8 lchip, lchip_num;
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;
    ctc_stats_basic_t stats;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(stats_index);
    SYS_MPLS_ILM_SPACE_CHECK(stats_index->spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(stats_index->spaceid, stats_index->label);
    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));

    p_ilm_data = &ilm_data;
    p_ilm_data->label = stats_index->label;
    p_ilm_data->spaceid = stats_index->spaceid;

    SYS_MPLS_LOCK;

    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(!p_ilm_data)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(CTC_MPLS_ID_STATS != p_ilm_data->id_type)
    {
        CTC_RETURN_MPLS_UNLOCK(CTC_E_MPLS_ENTRY_NOT_SUPPORT_STATS);
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN_MPLS_UNLOCK(sys_humber_stats_reset_flow_stats(lchip, p_ilm_data->stats_ptr[lchip]));
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_add_vpws_pw(sys_usrid_vlan_entry_t* p_userid_entry, ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    sys_vlan_info_t vlan_info;
    uint16 vlan_ptr;
    sys_nh_offset_array_t fwd_offset;
    uint8 chip_num;
    uint8 i;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VPWS;
    CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr));
    p_userid_entry->ds_entry_usrid.usr_vlan_ptr = vlan_ptr & 0x1fff;

    CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_mpls_pw->u.pw_nh_id, fwd_offset));
    p_userid_entry->ds_entry_usrid.fwd_ptr_valid = TRUE;

    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        p_userid_entry->usrid_key_entry.lchip = i;
        p_userid_entry->ds_entry_usrid.binding_data_l.fwd_ptr = fwd_offset[i] & 0xffff;
        CTC_ERROR_RETURN(sys_humber_usrid_add_vlan_entry(p_userid_entry));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_mpls_add_vpls_pw(sys_usrid_vlan_entry_t* p_userid_entry, ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    sys_vlan_info_t vlan_info;
    uint16 vlan_ptr;
    uint8 chip_num;
    uint8 i;

    vlan_info.vlan_ptr_type = p_mpls_pw->learn_disable?SYS_VLAN_PTR_TYPE_VPLS_LRN_DIS:
                                                      SYS_VLAN_PTR_TYPE_VPLS_LRN_EN;
    CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &vlan_ptr));
    p_userid_entry->ds_entry_usrid.usr_vlan_ptr = vlan_ptr & 0x1fff;
    p_userid_entry->ds_entry_usrid.vrfid_valid = TRUE;
    p_userid_entry->ds_entry_usrid.binding_data_l.vrf_id = p_mpls_pw->u.vpls_info.fid;
    p_userid_entry->ds_entry_usrid.vpls_port_type = p_mpls_pw->u.vpls_info.vpls_port_type;
    if(p_mpls_pw->u.vpls_info.vpls_src_port <= SYS_MPLS_VPLS_SRC_PORT_NUM)
    {
        p_userid_entry->ds_entry_usrid.binding_data_h.vpls_src_port = p_mpls_pw->u.vpls_info.vpls_src_port & 0x1fff;
        p_userid_entry->ds_entry_usrid.binding_data_h.vpls_src_port_valid = TRUE;
    }

    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        p_userid_entry->usrid_key_entry.lchip = i;
        CTC_ERROR_RETURN(sys_humber_usrid_add_vlan_entry(p_userid_entry));
    }

    return CTC_E_NONE;
}

/**
 @brief function of add l2vpn pw entry

 @param[in] p_mpls_pw, parameters used to add l2vpn pw entry

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_add_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    sys_usrid_vlan_entry_t userid_entry;

    SYS_MPLS_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_mpls_pw);
    SYS_MPLS_ILM_LABEL_CHECK(0, p_mpls_pw->label);
    SYS_MPLS_L2VPN_DATA_CHECK(p_mpls_pw);

    kal_memset(&userid_entry, 0, sizeof(sys_usrid_vlan_entry_t));
    userid_entry.valid.vc_valid = TRUE;
    userid_entry.usrid_key_entry.customer_id = p_mpls_pw->label << 12;
    userid_entry.ds_entry_usrid.binding_data_h.srv_aclqos_en = p_mpls_pw->service_aclqos_enable;

    CTC_ERROR_RETURN(p_mpls_master->write_pw[p_mpls_pw->l2vpntype](&userid_entry, p_mpls_pw));

    return CTC_E_NONE;
}

/**
 @brief function of remove l2vpn pw entry

 @param[in] label, vc label of the l2vpn pw entry

 @return CTC_E_XXX
 */
int32
sys_humber_mpls_del_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw)
{
    sys_usrid_vlan_entry_t userid_entry;
    uint8 chip_num;
    uint8 i;

    SYS_MPLS_INIT_CHECK;

    SYS_MPLS_ILM_LABEL_CHECK(0, p_mpls_pw->label);

    kal_memset(&userid_entry, 0, sizeof(sys_usrid_vlan_entry_t));
    userid_entry.valid.vc_valid = TRUE;
    userid_entry.usrid_key_entry.customer_id = p_mpls_pw->label << 12;

    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        userid_entry.usrid_key_entry.lchip = i;
        CTC_ERROR_RETURN(sys_humber_usrid_delete_vlan_entry(&userid_entry));
    }

    return CTC_E_NONE;
}

int32
sys_humber_mpls_reset_in_sqn(uint8 index)
{
    sequencenumbertable_t ds_seq_num;
    uint8 chip_num;
    uint8 i;
    uint32 cmd;

    ds_seq_num.sequence_number = 1;

    cmd = DRV_IOW(IOC_TABLE, SEQUENCENUMBERTABLE, DRV_ENTRY_FLAG);
    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, index, cmd, &ds_seq_num));
    }

    return CTC_E_NONE;
}

int32
sys_humber_mpls_reset_out_sqn(uint8 index)
{
    ds_l3_edit_sequence_num_t ds_edit_mpls_seq;
    uint8 chip_num;
    uint8 i;
    uint32 cmdr, cmdw;
    uint8 sub_index = index >> 2;
    uint8 offset = index & 0x3;

    cmdr = DRV_IOR(IOC_TABLE, DS_L3_EDIT_SEQUENCE_NUM, DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_TABLE, DS_L3_EDIT_SEQUENCE_NUM, DRV_ENTRY_FLAG);
    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, sub_index, cmdr, &ds_edit_mpls_seq));
        switch(offset)
        {
            case 0:
                ds_edit_mpls_seq.seq_num31_to0 = 1;
                break;
            case 1:
                ds_edit_mpls_seq.seq_num63_to32 = 1;
                break;
            case 2:
                ds_edit_mpls_seq.seq_num95_to64 = 1;
                break;
            case 3:
                ds_edit_mpls_seq.seq_num127_to96 = 1;
                break;
            default:
                break;
        }
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, sub_index, cmdw, &ds_edit_mpls_seq));
    }

    return CTC_E_NONE;
}

int32
sys_humber_mpls_init_entries(void)
{
    ds_mpls_t dsmpls;
    uint8 chip_num;
    uint32 i, j, k;
    uint32 cmd;

    kal_memset(&dsmpls, 0, sizeof(ds_mpls_t));
    dsmpls.flow_policer_ptr7to0 = 0xff;
    dsmpls.flow_policer_ptr10to8 = 0x7;
    dsmpls.flow_policer_ptr11 = 0x1;
    dsmpls.flow_policer_ptr15to12 = 0xf;
    dsmpls.ds_fwd_ptr = 0xfffff;

    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, DS_MPLS, DRV_ENTRY_FLAG);

    /* ilm entries */
    for(i = 0; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if(p_mpls_master->space[i].enable)
        {
            for(j = 0; j < p_mpls_master->space[i].size; j++)
            {
                for(k = 0; k < chip_num; k++)
                {
                    CTC_ERROR_RETURN(drv_tbl_ioctl(k, p_mpls_master->space[i].base + j, cmd, &dsmpls));
                }
            }
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_mpls_init(ctc_mpls_init_t* p_mpls_info)
{
    ds_mpls_ctl_t mpls_ctl;
    ipe_lookup_result_ctl_t rslt_ctl;
    uint32 cmdr, cmdw;
    int i, lchip, chip_num;
    uint32 base;
    uint8 block_num;

    /* check the parameter */
    CTC_PTR_VALID_CHECK(p_mpls_info);
    for(i = 0; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if(p_mpls_info->space_info[i].enable &&
           p_mpls_info->space_info[i].sizetype > 15)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    p_mpls_master = mem_malloc(MEM_MPLS_MODULE, sizeof(sys_mpls_master_t));
    if(NULL == p_mpls_master)
    {
        return CTC_E_NO_MEMORY;
    }

    chip_num = sys_humber_get_local_chip_num();

    /* global space */
    if(p_mpls_info->space_info[0].sizetype > 3)
    {
        block_num = 1 << (p_mpls_info->space_info[0].sizetype - 3);
    }
    else
    {
        block_num = 1;
    }
    cmdr = DRV_IOR(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmdr, &rslt_ctl));
    p_mpls_master->space[0].base = rslt_ctl.label_base_global << 8;
    p_mpls_master->space[0].size = SYS_MPLS_TBL_BLOCK_SIZE * (1 << p_mpls_info->space_info[0].sizetype);
    p_mpls_master->space[0].p_vet = ctc_vector_init(block_num, SYS_MPLS_TBL_BLOCK_SIZE * 8);
    if(NULL == p_mpls_master->space[0].p_vet)
    {
        return CTC_E_NO_MEMORY;
    }

    rslt_ctl.label_space_size_type_global = p_mpls_info->space_info[0].sizetype;
    for(lchip = 0; lchip < chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmdw, &rslt_ctl));
    }
    base = rslt_ctl.label_base_global + (1 << p_mpls_info->space_info[0].sizetype);

    kal_memset(&mpls_ctl, 0, sizeof(ds_mpls_ctl_t));
    mpls_ctl.num_of_label = CTC_MPLS_LABEL_NUM_PROCESS;
    cmdw = DRV_IOW(IOC_TABLE, DS_MPLS_CTL, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, 0, cmdw, &mpls_ctl));
    }
    p_mpls_master->space[0].enable = p_mpls_info->space_info[0].enable;

    /* interface space */
    for(i = 1; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if(p_mpls_info->space_info[i].enable)
        {
            if(p_mpls_info->space_info[i].sizetype > 3)
            {
                block_num = 1 << (p_mpls_info->space_info[i].sizetype - 3);
            }
            else
            {
                block_num = 1;
            }
            p_mpls_master->space[i].p_vet = ctc_vector_init(block_num, SYS_MPLS_TBL_BLOCK_SIZE * 8);
            if(NULL == p_mpls_master->space[i].p_vet)
            {
                return CTC_E_NO_MEMORY;
            }
            p_mpls_master->space[i].base = base << 8;
            p_mpls_master->space[i].size = SYS_MPLS_TBL_BLOCK_SIZE * (1 << p_mpls_info->space_info[i].sizetype);
            p_mpls_master->space[i].enable = TRUE;

            mpls_ctl.interface_label_valid = TRUE;
            mpls_ctl.label_base = base;
            mpls_ctl.label_space_sizetype = p_mpls_info->space_info[i].sizetype;

            base += (1 << p_mpls_info->space_info[i].sizetype);
        }
        else
        {
            p_mpls_master->space[i].enable = FALSE;

            mpls_ctl.interface_label_valid = FALSE;
            mpls_ctl.label_base = 0;
            mpls_ctl.label_space_sizetype = 0;
        }

        for(lchip = 0; lchip < chip_num; lchip++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, i, cmdw, &mpls_ctl));
        }
    }

    /* min interface label, the label littler than it must be a global label */
    p_mpls_master->min_int_label = rslt_ctl.min_interface_label << 8;

    /* call back function */
    p_mpls_master->write_ilm[CTC_MPLS_LABEL_TYPE_NORMAL] = _sys_humber_mpls_ilm_normal;
    p_mpls_master->write_ilm[CTC_MPLS_LABEL_TYPE_L3VPN] = _sys_humber_mpls_ilm_l3vpn;
    p_mpls_master->write_ilm[CTC_MPLS_LABEL_TYPE_VPWS] = _sys_humber_mpls_ilm_vpws;
    p_mpls_master->write_ilm[CTC_MPLS_LABEL_TYPE_VPLS] = _sys_humber_mpls_ilm_vpls;


    p_mpls_master->write_pw[CTC_MPLS_L2VPN_VPWS] = _sys_humber_mpls_add_vpws_pw;
    p_mpls_master->write_pw[CTC_MPLS_L2VPN_VPLS] = _sys_humber_mpls_add_vpls_pw;
    CTC_ERROR_RETURN(sys_humer_nh_get_max_external_nhid( &p_mpls_master->max_external_nhid ));

    SYS_MPLS_CREAT_LOCK;

    sys_humber_mpls_init_entries();

    return CTC_E_NONE;
}

int32
sys_humber_mpls_space_show(void)
{
    uint32 i;

    SYS_MPLS_INIT_CHECK;

    SYS_MPLS_DBG_DUMP("The minimum interface space label is %d\n\r", p_mpls_master->min_int_label);
    SYS_MPLS_DBG_DUMP("Space    Size\n\r");

    for(i = 0; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if(p_mpls_master->space[i].enable)
        {
            SYS_MPLS_DBG_DUMP("%-8d %d\n\r", i, p_mpls_master->space[i].size);
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_show_ilm_info(sys_mpls_ilm_t* p_ilm_data, void* data)
{


    if(!p_ilm_data)
    {
        return CTC_E_NONE;
    }

    SYS_MPLS_DBG_DUMP("%-7d %-10d", p_ilm_data->spaceid, p_ilm_data->label);
    switch(p_ilm_data->type)
    {
    case  CTC_MPLS_LABEL_TYPE_NORMAL:     /**< This label is a normal label */
        SYS_MPLS_DBG_DUMP("%-7s %-6d %-8d %-9d", "-",
                          p_ilm_data->ecpn, p_ilm_data->key_offset, p_ilm_data->nh_id);
        break;
    case  CTC_MPLS_LABEL_TYPE_L3VPN:       /**< This label is a l3vpn VC label */
        SYS_MPLS_DBG_DUMP("%-7s %-6d %-8d %-9d",  "L3VPN",
                          p_ilm_data->ecpn, p_ilm_data->key_offset, p_ilm_data->nh_id);
        break;
    case  CTC_MPLS_LABEL_TYPE_VPWS:        /**< This label is a vpws VC label */
        SYS_MPLS_DBG_DUMP("%-7s %-6d %-8d %-9d", "VPWS",
                          p_ilm_data->ecpn, p_ilm_data->key_offset, p_ilm_data->nh_id);
        break;
    case  CTC_MPLS_LABEL_TYPE_VPLS:       /**< This label is a vpls VC label */
        SYS_MPLS_DBG_DUMP("%-7s %-6d %-8d %-9d", "VPLS",
                          p_ilm_data->ecpn, p_ilm_data->key_offset, p_ilm_data->nh_id);
        break;
    default:
        break;
    }

    if(p_ilm_data->type == CTC_MPLS_LABEL_TYPE_VPLS)
    {
        SYS_MPLS_DBG_DUMP("%-7d", p_ilm_data->pwid);
    }
    else
    {
        SYS_MPLS_DBG_DUMP("-      ");
    }

    if(p_ilm_data->type == CTC_MPLS_LABEL_TYPE_L3VPN)
    {
        SYS_MPLS_DBG_DUMP("%-8d", p_ilm_data->flw_vrf_srv);
    }
    else
    {
        SYS_MPLS_DBG_DUMP("-       ");
    }

    if(p_ilm_data->type == CTC_MPLS_LABEL_TYPE_NORMAL ||
       p_ilm_data->type == CTC_MPLS_LABEL_TYPE_L3VPN)
    {
        if(p_ilm_data->model == CTC_MPLS_TUNNEL_MODE_UNIFORM)
        {
            SYS_MPLS_DBG_DUMP("U      ");
        }
        else if(p_ilm_data->model == CTC_MPLS_TUNNEL_MODE_SHORT_PIPE)
        {
            SYS_MPLS_DBG_DUMP("S      ");
        }
        else
        {
            SYS_MPLS_DBG_DUMP("P      ");
        }
        SYS_MPLS_DBG_DUMP("-");
    }
    else
    {
        SYS_MPLS_DBG_DUMP("P      ");
        if(p_ilm_data->cwen)
        {
            SYS_MPLS_DBG_DUMP("U");
        }
        else
        {
            SYS_MPLS_DBG_DUMP("-");
        }
    }
    SYS_MPLS_DBG_DUMP("\n\r");

    return CTC_E_NONE;
}

int32
sys_humber_mpls_ilm_show(void)
{
    int i;

    SYS_MPLS_INIT_CHECK;

    SYS_MPLS_DBG_DUMP("Mode : U-Uniform     S-Short pipe     P-Pipe\n\r");
    SYS_MPLS_DBG_DUMP("Space   Label     Type    Ecpn   Offset   NH ID    PW     VRF     Mode   CW\n\r");

    for(i = 0; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if(p_mpls_master->space[i].enable)
        {
            ctc_vector_traverse(p_mpls_master->space[i].p_vet, (vector_traversal_fn)_sys_humber_show_ilm_info, NULL);
        }
    }

    return CTC_E_NONE;
}
uint32 g_ilm_count= 0;

static int32
_sys_humber_get_ilm_count(sys_mpls_ilm_t* p_ilm_data, void* data)
{
    if (p_ilm_data != NULL)
    {
        g_ilm_count++;
    }
     return CTC_E_NONE;
}

int32
sys_humber_mpls_ilm_count(void)
{
    int i;
    uint32 count = 0;

    SYS_MPLS_INIT_CHECK;

    for(i = 0; i < CTC_MPLS_SPACE_NUMBER; i++)
    {
        if (p_mpls_master->space[i].enable)
        {
            g_ilm_count = 0;
            ctc_vector_traverse(p_mpls_master->space[i].p_vet, (vector_traversal_fn)_sys_humber_get_ilm_count, &count);
            SYS_MPLS_DBG_DUMP("Mpls space %d ,ilm count:%-10d\n\r", i, g_ilm_count);
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_mpls_sqn_show(uint8 index)
{
    sequencenumbertable_t ds_seq_num;
    ds_l3_edit_sequence_num_t ds_edit_mpls_seq;
    uint32 out_seq = 0;
    uint8 sub_index = index >> 2;
    uint8 offset = index & 0x3;
    uint32 cmd;

    cmd = DRV_IOR(IOC_TABLE, SEQUENCENUMBERTABLE, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, index, cmd, &ds_seq_num));

    cmd = DRV_IOR(IOC_TABLE, DS_L3_EDIT_SEQUENCE_NUM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, sub_index, cmd, &ds_edit_mpls_seq));

    switch(offset)
    {
        case 0:
            out_seq = ds_edit_mpls_seq.seq_num31_to0;
            break;
        case 1:
            out_seq = ds_edit_mpls_seq.seq_num63_to32;
            break;
        case 2:
            out_seq = ds_edit_mpls_seq.seq_num95_to64;
            break;
        case 3:
            out_seq = ds_edit_mpls_seq.seq_num127_to96;
            break;
        default:
            break;
    }

    SYS_MPLS_DBG_DUMP("Incoming sequence number %d\n\r", ds_seq_num.sequence_number);
    SYS_MPLS_DBG_DUMP("Outgoing sequence number %d\n\r", out_seq);

    return CTC_E_NONE;
}

int32
sys_humber_mpls_check_label_used(uint16 spaceid, uint32 label, bool *used)
{
    sys_mpls_ilm_t  *p_ilm_data, ilm_data;

    *used = FALSE;

    SYS_MPLS_INIT_CHECK;

    SYS_MPLS_ILM_SPACE_CHECK(spaceid);
    SYS_MPLS_ILM_LABEL_CHECK(spaceid, label);

    p_ilm_data = &ilm_data;
    p_ilm_data->label = label;
    p_ilm_data->spaceid = spaceid;

    SYS_MPLS_LOCK;

    CTC_ERROR_RETURN_MPLS_UNLOCK(_sys_humber_mpls_db_lookup(&p_ilm_data));
    if(p_ilm_data)
    {
        *used = TRUE;
    }

    SYS_MPLS_UNLOCK;

    return CTC_E_NONE;
}
