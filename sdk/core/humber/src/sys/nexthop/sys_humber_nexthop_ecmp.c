/**
 @file sys_humber_nexthop_ecmp.c

 @date 2009-12-23

 @version v2.0

 The file contains all ecmp related function
*/

/****************************************************************************
*
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "sys_humber_chip.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_l3if.h"

#include "drv_io.h"

/****************************************************************************
*
* Global and Declaration
*
*****************************************************************************/

/****************************************************************************
*
* Defines and Macros
*
*****************************************************************************/
#define SYS_ECMP_ITEM_CHANGE_NH(p_ecmpdb, index, nh_id) \
    { \
        if(p_ecmpdb->item_array[index].oper_nh_id != nh_id) \
        { \
            uint8 chipid; \
            sys_nh_offset_array_t offset_array; \
            ds_fwd_t dsfwd; \
            uint8 chip_num = sys_humber_get_local_chip_num(); \
            CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset_nolock(nh_id, offset_array)); \
            for(chipid = 0; chipid < chip_num; chipid++) \
            { \
                CTC_ERROR_RETURN(sys_humber_nh_get_entry_dsfwd(chipid, offset_array[chipid], &dsfwd)); \
                CTC_ERROR_RETURN(sys_humber_nh_write_asic_table(chipid, SYS_NH_ENTRY_TYPE_FWD, \
                                     p_ecmpdb->hdr.dsfwd_info[chipid].dsfwd_offset + index, &dsfwd)); \
            } \
            p_ecmpdb->item_array[index].oper_nh_id = nh_id; \
        } \
    }

#define SYS_ECMP_GET_ITEM_INDEX(p_ecmpdb, index, nh_id) \
    { \
        for((index) = 0; (index) < (p_ecmpdb)->valid_item_cnt; (index)++) \
        { \
            if((p_ecmpdb)->item_array[index].nh_id == (nh_id)) \
            { \
                break; \
            } \
        } \
    }

/****************************************************************************
*
* Function
*
*****************************************************************************/
static INLINE int32
_sys_humber_nh_ecmp_alloc_dsfwd(sys_nh_info_dsfwd_t* p_dsfwd_offset)
{
    uint8 i, chip_num;

    chip_num = sys_humber_get_local_chip_num();
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(sys_humber_nh_reverse_offset_alloc(i, SYS_NH_ENTRY_TYPE_FWD, CTC_MAX_ECPN,
                                                    &p_dsfwd_offset[i].dsfwd_offset));
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_ecmp_free_dsfwd(sys_nh_info_dsfwd_t* p_dsfwd_offset)
{
    uint8 i, chip_num;

    chip_num = sys_humber_get_local_chip_num();
    for (i = 0; i < chip_num; i++)
    {
        sys_humber_nh_offset_free(i, SYS_NH_ENTRY_TYPE_FWD, CTC_MAX_ECPN,
                                                   p_dsfwd_offset[i].dsfwd_offset);
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ecmp_sync_item(sys_nh_info_ecmp_t* p_nhdb)
{
    uint8 useful_item_cnt = 0;
    uint32 useful_nh_id[CTC_MAX_ECPN];
    uint8 i, j = 0;

    for(i = 0; i < p_nhdb->valid_item_cnt; i++)
    {
        if(!p_nhdb->item_array[i].is_oper_valid)
        {
            useful_nh_id[useful_item_cnt] = p_nhdb->item_array[i].nh_id;
            useful_item_cnt++;

            SYS_ECMP_ITEM_CHANGE_NH(p_nhdb, i, p_nhdb->item_array[i].nh_id);
        }
    }

    if(0 == useful_item_cnt)
    {
        for(i = 0; i < p_nhdb->valid_item_cnt; i++)
        {
            SYS_ECMP_ITEM_CHANGE_NH(p_nhdb, i, p_nhdb->item_array[i].nh_id);
        }
    }
    else
    {
        for(i = 0; i < p_nhdb->valid_item_cnt; i++)
        {
            if(p_nhdb->item_array[i].is_oper_valid)
            {
                SYS_ECMP_ITEM_CHANGE_NH(p_nhdb, i, useful_nh_id[j % useful_item_cnt]);
                j++;
            }
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ecmp_add_oif(sys_nh_info_ecmp_t* p_nhdb, uint16 l3if)
{
    uint8 i;

    for(i = 0; i < p_nhdb->oif_cnt; i++)
    {
        if(p_nhdb->rpf_array[i].oif_id == l3if)
        {
            p_nhdb->rpf_array[i].ref++;
            p_nhdb->oif_changed = FALSE;
            return CTC_E_NONE;
        }
    }

    p_nhdb->rpf_array[p_nhdb->oif_cnt].oif_id = l3if;
    p_nhdb->rpf_array[p_nhdb->oif_cnt].ref = 1;
    p_nhdb->oif_cnt++;
    p_nhdb->oif_changed = TRUE;

    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ecmp_remove_oif(sys_nh_info_ecmp_t* p_nhdb, uint32 nh_id, uint16 l3if)
{
    uint8 i;

    for(i = 0; i < p_nhdb->oif_cnt; i++)
    {
        if(p_nhdb->rpf_array[i].oif_id == l3if)
        {
            break;
        }
    }

    p_nhdb->rpf_array[i].ref--;
    if(p_nhdb->rpf_array[i].ref > 0)
    {
        p_nhdb->oif_changed = FALSE;
        return CTC_E_NONE;
    }

    p_nhdb->oif_cnt--;
    p_nhdb->rpf_array[i].oif_id = p_nhdb->rpf_array[p_nhdb->oif_cnt].oif_id;
    p_nhdb->rpf_array[i].ref = p_nhdb->rpf_array[p_nhdb->oif_cnt].ref;
    p_nhdb->rpf_array[p_nhdb->oif_cnt].oif_id = 0;
    p_nhdb->rpf_array[p_nhdb->oif_cnt].ref = 0;

    p_nhdb->oif_changed = TRUE;
    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ecmp_add_item(sys_nh_info_ecmp_t* p_nhdb, uint32 nh_id, sys_info_ecmp_t *p_ecmpinfo)
{
    uint8 pos;

    SYS_ECMP_GET_ITEM_INDEX(p_nhdb, pos, nh_id);
    if(pos < p_nhdb->valid_item_cnt)
    {
        return CTC_E_ENTRY_EXIST;
    }

    p_nhdb->item_array[p_nhdb->valid_item_cnt].nh_id = nh_id;
    p_nhdb->item_array[p_nhdb->valid_item_cnt].oper_nh_id = SYS_HUMBER_NH_INVALID_NHID;
    if(CTC_FLAG_ISSET(p_ecmpinfo->p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
    {
        p_nhdb->item_array[p_nhdb->valid_item_cnt].is_oper_valid = TRUE;
    }
    else
    {
        p_nhdb->item_array[p_nhdb->valid_item_cnt].is_oper_valid = FALSE;
    }

    if(p_nhdb->oif_need)
    {
        _sys_humber_nh_ecmp_add_oif(p_nhdb, p_ecmpinfo->l3oif);
    }

    CTC_ERROR_RETURN(sys_humber_nh_add_ref_item(nh_id, (sys_nh_info_com_t*)p_nhdb, p_ecmpinfo));

    p_nhdb->valid_item_cnt++;

    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ecmp_remove_item(sys_nh_info_ecmp_t* p_nhdb, uint32 nh_id, sys_info_ecmp_t *p_ecmpinfo)
{
    uint8 pos;

    SYS_ECMP_GET_ITEM_INDEX(p_nhdb, pos, nh_id);
    if(pos >= p_nhdb->valid_item_cnt)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    p_nhdb->valid_item_cnt--;
    p_nhdb->item_array[pos].nh_id = p_nhdb->item_array[p_nhdb->valid_item_cnt].nh_id;
    p_nhdb->item_array[pos].is_oper_valid = p_nhdb->item_array[p_nhdb->valid_item_cnt].is_oper_valid;

    if(p_nhdb->oif_need)
    {
        _sys_humber_nh_ecmp_remove_oif(p_nhdb, nh_id, p_ecmpinfo->l3oif);
    }

    CTC_ERROR_RETURN(sys_humber_nh_remove_ref_item(nh_id, (sys_nh_info_com_t*)p_nhdb, p_ecmpinfo));

    return CTC_E_NONE;
}

int32
sys_humber_nh_ecmp_update_item(sys_nh_info_ecmp_t* p_nhdb, uint32 nh_id)
{
    uint16 nh_entry_flags;
    uint8 pos;

    SYS_ECMP_GET_ITEM_INDEX(p_nhdb, pos, nh_id);
    if(pos >= p_nhdb->valid_item_cnt)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_nh_get_flags_nolock(nh_id, &nh_entry_flags));

    p_nhdb->item_array[pos].oper_nh_id = SYS_HUMBER_NH_INVALID_NHID;
    if(CTC_FLAG_ISSET(nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV))
    {
        p_nhdb->item_array[pos].is_oper_valid = TRUE;
    }
    else
    {
        p_nhdb->item_array[pos].is_oper_valid = FALSE;
    }

    CTC_ERROR_RETURN(_sys_humber_nh_ecmp_sync_item(p_nhdb));

    return CTC_E_NONE;
}


int32
sys_humber_nh_create_ecmp_cb(sys_nh_param_com_t* p_com_nh_param, sys_nh_info_com_t* p_com_db)
{
    sys_nh_param_ecmp_t* p_nh_param;
    sys_nh_param_ecmp_api_t* p_nh_api_param;
    sys_nh_info_ecmp_t* p_nhdb;
    uint8 chip_num;
    uint8 i;
    sys_info_ecmp_t ecmpinfo1, ecmpinfo2;
    int32 ret;

    SYS_NH_DBG_FUNC();

    p_nh_param = (sys_nh_param_ecmp_t*)(p_com_nh_param);
    p_nhdb = (sys_nh_info_ecmp_t*)(p_com_db);
    p_nhdb->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_ECMP;
    p_nh_api_param = p_nh_param->p_ecmp_param;
    p_nhdb->oif_need = p_nh_api_param->oif_need;

    CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(p_nh_param->hdr.nhid, &ecmpinfo1.p_nhinfo));
    CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(p_nh_api_param->ref_nh_id, &ecmpinfo2.p_nhinfo));
    if(p_nhdb->oif_need)
    {
        SYS_NH_ECMP_OIF_SET(ecmpinfo1);
        SYS_NH_ECMP_OIF_SET(ecmpinfo2);
    }

    CTC_ERROR_RETURN(_sys_humber_nh_ecmp_alloc_dsfwd(p_nhdb->hdr.dsfwd_info));

    ret = _sys_humber_nh_ecmp_add_item(p_nhdb, p_nh_param->hdr.nhid, &ecmpinfo1);
    if(ret)
    {
        _sys_humber_nh_ecmp_free_dsfwd(p_nhdb->hdr.dsfwd_info);
        return ret;
    }
    ret = _sys_humber_nh_ecmp_add_item(p_nhdb, p_nh_api_param->ref_nh_id, &ecmpinfo2);
    if(ret)
    {
        _sys_humber_nh_ecmp_free_dsfwd(p_nhdb->hdr.dsfwd_info);
        return ret;
    }

    ret = _sys_humber_nh_ecmp_sync_item(p_nhdb);
    if(ret)
    {
        _sys_humber_nh_ecmp_free_dsfwd(p_nhdb->hdr.dsfwd_info);
        return ret;
    }

    chip_num = sys_humber_get_local_chip_num();
    for(i = 0; i < chip_num; i++)
    {
        p_nh_api_param->fwd_offset[i] = p_nhdb->hdr.dsfwd_info[i].dsfwd_offset;
    }
    p_nh_api_param->p_rpf_array = p_nhdb->rpf_array;
    p_nh_api_param->oif_cnt = p_nhdb->oif_cnt;
    p_nh_api_param->oif_changed = p_nhdb->oif_changed;

    return CTC_E_NONE;
}

int32
sys_humber_nh_update_ecmp_cb(sys_nh_info_com_t* p_com_db, sys_nh_param_com_t* p_com_nh_param)
{
    sys_nh_param_ecmp_t* p_nh_param;
    sys_nh_param_ecmp_api_t* p_nh_api_param;
    sys_nh_info_ecmp_t* p_nhdb;
    sys_info_ecmp_t ecmpinfo;


    SYS_NH_DBG_FUNC();

    p_nh_param = (sys_nh_param_ecmp_t*)(p_com_nh_param);
    p_nhdb = (sys_nh_info_ecmp_t*)(p_com_db);
    p_nh_api_param = p_nh_param->p_ecmp_param;
    p_nhdb->oif_need = p_nh_api_param->oif_need;

    CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(p_nh_api_param->ref_nh_id, &ecmpinfo.p_nhinfo));
    if(p_nhdb->oif_need)
    {
        SYS_NH_ECMP_OIF_SET(ecmpinfo);
    }

    switch(p_nh_api_param->change_type)
    {
        case SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM :
            CTC_ERROR_RETURN(_sys_humber_nh_ecmp_add_item(p_nhdb, p_nh_api_param->ref_nh_id, &ecmpinfo));
            CTC_ERROR_RETURN(_sys_humber_nh_ecmp_sync_item(p_nhdb));
            break;

        case SYS_NH_CHANGE_TYPE_ECMP_REMOVE_ITEM :
            CTC_ERROR_RETURN(_sys_humber_nh_ecmp_remove_item(p_nhdb, p_nh_api_param->ref_nh_id, &ecmpinfo));
            if(p_nhdb->valid_item_cnt == 1)
            {
                p_nh_api_param->ref_nh_id = p_nhdb->item_array[0].nh_id;
                p_nh_param->hdr.nhid = p_nhdb->item_array[0].nh_id;
            }

            CTC_ERROR_RETURN(_sys_humber_nh_ecmp_sync_item(p_nhdb));
            break;

        default :
            return CTC_E_INVALID_PARAM;
    }

    p_nh_api_param->p_rpf_array = p_nhdb->rpf_array;
    p_nh_api_param->oif_cnt = p_nhdb->oif_cnt;
    p_nh_api_param->oif_changed = p_nhdb->oif_changed;
    CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset_nolock(p_nh_param->hdr.nhid, p_nh_api_param->fwd_offset));

    return CTC_E_NONE;
}

int32
sys_humber_nh_delete_ecmp_cb(sys_nh_info_com_t* p_com_db)
{
    sys_nh_info_ecmp_t* p_nhdb;
    uint8 i;
    sys_info_ecmp_t ecmpinfo;

    SYS_NH_DBG_FUNC();

    p_nhdb = (sys_nh_info_ecmp_t*)(p_com_db);

    for(i = 0; i < p_nhdb->valid_item_cnt; i++)
    {
        if(!sys_humber_nh_get_nhinfo_by_nhid(p_nhdb->item_array[i].nh_id, &ecmpinfo.p_nhinfo))
        {
            sys_humber_nh_remove_ref_item(p_nhdb->item_array[i].nh_id, (sys_nh_info_com_t*)p_nhdb, &ecmpinfo);
        }
    }

    CTC_ERROR_RETURN(_sys_humber_nh_ecmp_free_dsfwd(p_nhdb->hdr.dsfwd_info));

    return CTC_E_NONE;
}


