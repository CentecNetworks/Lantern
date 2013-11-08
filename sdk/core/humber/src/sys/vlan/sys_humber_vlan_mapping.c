/**
 @file sys_humber_vlan_mapping.c

 @date 2010-1-4

 @version v2.0

*/

#include "ctc_debug.h"
#include "ctc_error.h"
#include "sys_humber_register.h"
#include "sys_humber_vlan_mapping.h"
#include "sys_humber_usrid.h"
#include "sys_humber_chip.h"
#include "sys_humber_vlan.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_queue_api.h"
#include "sys_humber_queue_enq.h"
/******************************************************************************
*
*   Macros and Defines
*
*******************************************************************************/
static bool is_vlan_mapping_init = FALSE;

/******************************************************************************
*
*   Functions
*
*******************************************************************************/
int32
sys_humber_vlan_mapping_init(void)
{
    if (TRUE == is_vlan_mapping_init)
    {
        return CTC_E_NONE;
    }

    is_vlan_mapping_init = TRUE;

    return CTC_E_NONE;
}

int32
sys_humber_vlan_add_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t *p_vlan_mapping)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint8 lport = 0;
    uint16 service_id = 0;
    sys_usrid_vlan_entry_t usrid_vlan;
    sys_nh_offset_array_t offset;
    sys_humber_vlan_link_t *p_head = NULL;
    sys_humber_vlan_link_t *p_tmp  = NULL;
    uint32 sum;
    int32 ret = 0;
    uint32 service_policer_num = 0;
    bool ingress_enable = 0;
    bool egress_enable = 0;

    uint8  is_svid_en     = 0;
    uint8  is_vpls_en     = 0;
    uint8  is_aps_en      = 0;
    uint8  is_fid_en      = 0;
    uint8  is_nh_en       = 0;
    uint8  is_srvc_en      = 0;
    uint8  is_cvid_en     = 0;

    is_svid_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_SVID         );
    is_vpls_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT);
    is_aps_en     = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_APS_SELECT   );
    is_fid_en     = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_FID          );
    is_nh_en      = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_NHID         );
    is_srvc_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID   );
    is_cvid_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_CVID         );

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" gport                     :%d\n", gport);
    SYS_VLAN_MAPPING_DEBUG_INFO(" key value                 :%d\n", p_vlan_mapping->key);
    SYS_VLAN_MAPPING_DEBUG_INFO(" action                    :0x%X\n", p_vlan_mapping->action);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svid                      :0x%X\n", p_vlan_mapping->old_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvid                      :0x%X\n", p_vlan_mapping->old_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svlan end                 :0x%X\n", p_vlan_mapping->svlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvlan end                 :0x%X\n", p_vlan_mapping->cvlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO("-------mapping to-----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" new svid                  :%d\n", p_vlan_mapping->u1.new_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" new cvid                  :%d\n", p_vlan_mapping->u2.new_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" fid                       :%d\n", p_vlan_mapping->u3.fid);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_mapping);
    CTC_GLOBAL_PORT_CHECK(gport);

    kal_memset(&usrid_vlan, 0, sizeof(usrid_vlan));

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CVID)
    {
        if ((p_vlan_mapping->old_cvid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }

        usrid_vlan.valid.igs_cvid_valid = 1;
        usrid_vlan.usrid_key_entry.ingress_cvid = (p_vlan_mapping->old_cvid & 0xFFF);
        usrid_vlan.usrid_key_entry.igs_cvid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CTAG_COS)
    {
        usrid_vlan.valid.ctag_cos_valid = 1;
        usrid_vlan.usrid_key_entry.ctag_cos = p_vlan_mapping->old_ccos;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_SVID)
    {
        if ((p_vlan_mapping->old_svid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_vlan.valid.igs_svid_valid = 1;
        usrid_vlan.usrid_key_entry.ingress_svid = (p_vlan_mapping->old_svid & 0xFFF);
        usrid_vlan.usrid_key_entry.igs_svid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_STAG_COS)
    {
        usrid_vlan.valid.stag_cos_valid = 1;
        usrid_vlan.usrid_key_entry.stag_cos =  p_vlan_mapping->old_scos;
    }

    if (((is_svid_en + is_vpls_en + is_aps_en) > 1)
        ||((is_fid_en + is_nh_en) > 1)
        ||((is_srvc_en + is_cvid_en) > 1))
    {
        return CTC_E_INVALID_PARAM;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT)
    {
        if (p_vlan_mapping->u1.vpls_src_port > 0x1fff)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    if ((p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPWS) &&
        (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPLS))
    {
        return CTC_E_INVALID_PARAM;
    }

    usrid_vlan.ds_entry_usrid.usr_vlan_ptr = 0x1FFF;
    usrid_vlan.valid.src_port_valid = 1;
    usrid_vlan.usrid_key_entry.global_port = gport;

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_SVID)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->u1.new_svid);
        usrid_vlan.ds_entry_usrid.binding_data_h.usr_svlan_valid = 1;
        usrid_vlan.ds_entry_usrid.binding_data_h.usr_svlan_id = p_vlan_mapping->u1.new_svid;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT)
    {
        usrid_vlan.ds_entry_usrid.binding_data_h.vpls_src_port_valid = 1;
        usrid_vlan.ds_entry_usrid.binding_data_h.vpls_src_port = p_vlan_mapping->u1.vpls_src_port;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID)
    {
        CTC_ERROR_RETURN(sys_humber_get_physical_service_id(p_vlan_mapping->u2.service_id, &service_id));
        usrid_vlan.ds_entry_usrid.binding_data_m.svr_id_en = 1;
        usrid_vlan.ds_entry_usrid.binding_data_m.svr_id = service_id;

        CTC_ERROR_RETURN(sys_humber_get_service_ingress_queue_enable(&ingress_enable));
        CTC_ERROR_RETURN(sys_humber_get_service_egress_queue_enable(&egress_enable));
        if(ingress_enable && egress_enable)
        {
            usrid_vlan.ds_entry_usrid.src_queue_select = (TRUE == p_vlan_mapping->is_igs_srvq)? 1:0;
        }
        else
        {
            usrid_vlan.ds_entry_usrid.src_queue_select = 0;
        }

        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_SERVICE_ACL_EN)
        {
            usrid_vlan.ds_entry_usrid.binding_data_h.srv_aclqos_en = 1;
        }

        CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_SERVICE_POLICER_NUM, &service_policer_num));
        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_SERVICE_POLICER_EN)
        {
            if ( service_id < service_policer_num )
            {
                usrid_vlan.ds_entry_usrid.binding_data_m.svr_policer_valid = 1;
            }
            else
            {
                return CTC_E_VLAN_MAPPING_INVALID_SERVICE_ID;
            }
        }
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_APS_SELECT)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->protected_vlan);
        CTC_APS_GROUP_ID_CHECK(p_vlan_mapping->u1.aps_select_group_id);
        usrid_vlan.ds_entry_usrid.aps_select_valid  = 1;
        usrid_vlan.ds_entry_usrid.usr_vlan_ptr = p_vlan_mapping->protected_vlan;
        usrid_vlan.ds_entry_usrid.binding_data_h.aps_select_group_valid = 1;
        usrid_vlan.ds_entry_usrid.binding_data_h.aps_select_group_id = p_vlan_mapping->u1.aps_select_group_id;
        usrid_vlan.ds_entry_usrid.binding_data_h.aps_select_protecting_path = (p_vlan_mapping->is_working_path == TRUE)? 0:1;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_CVID)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->u2.new_cvid);
        usrid_vlan.ds_entry_usrid.binding_data_m.usr_cvlan_valid = 1;
        usrid_vlan.ds_entry_usrid.binding_data_m.usr_cvlan_id = p_vlan_mapping->u2.new_cvid;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_FID)
    {
        CTC_FID_RANGE_CHECK(p_vlan_mapping->u3.fid);
        usrid_vlan.ds_entry_usrid.vrfid_valid = 1;
        usrid_vlan.ds_entry_usrid.binding_data_l.vrf_id = p_vlan_mapping->u3.fid;
    }


    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPLS)
    {
        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPLS_LRN_DIS)
        {
            usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_DIS;
        }
        else if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_MACLIMIT_EN)
        {
            usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPLS_PTR_MACLIMIT_EN;
        }
        else
        {
            usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_EN;
        }
    }

     if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
     {
         CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_vlan_mapping->u3.nh_id, offset));
         if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPWS)
         {
            usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPWS_PTR;
         }
         else if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID)
         {
            usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_SVQ_PTR;
         }
         else
         {
              usrid_vlan.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_SWITCH_PTR;
         }
     }


    if (0 != p_vlan_mapping->svlan_end && (p_vlan_mapping->svlan_end >= p_vlan_mapping->old_svid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_svid, p_vlan_mapping->svlan_end, &p_head, &sum);

        usrid_vlan.valid.igs_svid_valid = 1;
        if (CTC_IS_LINKAGG_PORT(gport))
        {
            lchip_num = sys_humber_get_local_chip_num();
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                usrid_vlan.usrid_key_entry.lchip = lchip;
                if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
                {
                    usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
                    usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
                }

                for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
                {
                    usrid_vlan.usrid_key_entry.ingress_svid = p_tmp->usValue;
                    usrid_vlan.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                    ret = ret? ret:sys_humber_usrid_add_vlan_entry(&usrid_vlan);

                }
            }
        }
        else
        {
            SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
            usrid_vlan.usrid_key_entry.lchip = lchip;
            if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
            {
                usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
                usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
            }

            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_vlan.usrid_key_entry.ingress_svid = p_tmp->usValue;
                usrid_vlan.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_add_vlan_entry(&usrid_vlan);

            }
        }

        sys_humber_vlan_range_free_node(p_head);
        if (NULL != p_head)
        {
            mem_free(p_head);
        }

        return ret;
    }

    if (0 != p_vlan_mapping->cvlan_end && (p_vlan_mapping->cvlan_end >= p_vlan_mapping->old_cvid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_cvid, p_vlan_mapping->cvlan_end, &p_head, &sum);

        usrid_vlan.valid.igs_cvid_valid = 1;
        if (CTC_IS_LINKAGG_PORT(gport))
        {
            lchip_num = sys_humber_get_local_chip_num();
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                usrid_vlan.usrid_key_entry.lchip = lchip;
                if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
                {
                    usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
                    usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
                }

                for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
                {
                    usrid_vlan.usrid_key_entry.ingress_cvid = p_tmp->usValue;
                    usrid_vlan.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                    ret = ret? ret:sys_humber_usrid_add_vlan_entry(&usrid_vlan);

                }
            }
        }
        else
        {
            SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
            usrid_vlan.usrid_key_entry.lchip = lchip;
            if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
            {
                usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
                usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
            }

            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_vlan.usrid_key_entry.ingress_cvid = p_tmp->usValue;
                usrid_vlan.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_add_vlan_entry(&usrid_vlan);

            }
        }

        sys_humber_vlan_range_free_node(p_head);
        if (NULL != p_head)
        {
            mem_free(p_head);
        }
        return CTC_E_NONE;
    }


    if (CTC_IS_LINKAGG_PORT(gport))
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_vlan.usrid_key_entry.lchip = lchip;
            if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
            {
                usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
                usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
            }

            CTC_ERROR_RETURN(sys_humber_usrid_add_vlan_entry(&usrid_vlan));
        }
    }
    else
    {
        SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
        usrid_vlan.usrid_key_entry.lchip = lchip;
        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
        {
            usrid_vlan.ds_entry_usrid.fwd_ptr_valid = 1;
            usrid_vlan.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
        }

        CTC_ERROR_RETURN(sys_humber_usrid_add_vlan_entry(&usrid_vlan));
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t *p_vlan_mapping)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint8 lport = 0;
    sys_usrid_vlan_entry_t usrid_vlan;
    sys_humber_vlan_link_t *p_head = NULL;
    sys_humber_vlan_link_t *p_tmp  = NULL;
    uint32 sum = 0;
    int32 ret = 0;

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" gport                      :%d\n", gport);
    SYS_VLAN_MAPPING_DEBUG_INFO(" key value                  :%d\n", p_vlan_mapping->key);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svid                       :%X\n", p_vlan_mapping->old_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvid                       :%X\n", p_vlan_mapping->old_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svlan end                  :%X\n", p_vlan_mapping->svlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvlan end                  :%X\n", p_vlan_mapping->cvlan_end);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_mapping);
    CTC_GLOBAL_PORT_CHECK(gport);

    kal_memset(&usrid_vlan, 0, sizeof(usrid_vlan));

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CVID)
    {
        if ((p_vlan_mapping->old_cvid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_vlan.valid.igs_cvid_valid = 1;
        usrid_vlan.usrid_key_entry.ingress_cvid = (p_vlan_mapping->old_cvid & 0xFFF);
        usrid_vlan.usrid_key_entry.igs_cvid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CTAG_COS)
    {
        usrid_vlan.valid.ctag_cos_valid = 1;
        usrid_vlan.usrid_key_entry.ctag_cos =  p_vlan_mapping->old_ccos;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_SVID)
    {
        if ((p_vlan_mapping->old_svid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_vlan.valid.igs_svid_valid = 1;
        usrid_vlan.usrid_key_entry.ingress_svid = (p_vlan_mapping->old_svid & 0xFFF);
        usrid_vlan.usrid_key_entry.igs_svid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_STAG_COS)
    {
        usrid_vlan.valid.stag_cos_valid = 1;
        usrid_vlan.usrid_key_entry.stag_cos =  p_vlan_mapping->old_scos;
    }

    usrid_vlan.valid.src_port_valid = 1;
    usrid_vlan.usrid_key_entry.global_port = gport;

    if (0 != p_vlan_mapping->svlan_end && (p_vlan_mapping->svlan_end >= p_vlan_mapping->old_svid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));
        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_svid, p_vlan_mapping->svlan_end, &p_head, &sum);

        usrid_vlan.valid.igs_svid_valid = 1;
        if (CTC_IS_LINKAGG_PORT(gport))
        {
            lchip_num = sys_humber_get_local_chip_num();
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                usrid_vlan.usrid_key_entry.lchip = lchip;
                for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
                {
                    usrid_vlan.usrid_key_entry.ingress_svid = p_tmp->usValue;
                    usrid_vlan.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                    ret = ret? ret:sys_humber_usrid_delete_vlan_entry(&usrid_vlan);

                }
            }
        }
        else
        {
            SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
            usrid_vlan.usrid_key_entry.lchip = lchip;
            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_vlan.usrid_key_entry.ingress_svid = p_tmp->usValue;
                usrid_vlan.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_delete_vlan_entry(&usrid_vlan);

            }
        }

        sys_humber_vlan_range_free_node(p_head);
        if (NULL != p_head)
        {
            mem_free(p_head);
        }

        return CTC_E_NONE;
    }

    if (0 != p_vlan_mapping->cvlan_end && (p_vlan_mapping->cvlan_end >= p_vlan_mapping->old_cvid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_cvid, p_vlan_mapping->cvlan_end, &p_head, &sum);

        usrid_vlan.valid.igs_cvid_valid = 1;
        if (CTC_IS_LINKAGG_PORT(gport))
        {
            lchip_num = sys_humber_get_local_chip_num();
            for (lchip = 0; lchip < lchip_num; lchip++)
            {
                usrid_vlan.usrid_key_entry.lchip = lchip;
                for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
                {
                    usrid_vlan.usrid_key_entry.ingress_cvid = p_tmp->usValue;
                    usrid_vlan.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                    ret = ret? ret:sys_humber_usrid_delete_vlan_entry(&usrid_vlan);

                }
            }
        }
        else
        {
            SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
            usrid_vlan.usrid_key_entry.lchip = lchip;
            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_vlan.usrid_key_entry.ingress_cvid = p_tmp->usValue;
                usrid_vlan.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_delete_vlan_entry(&usrid_vlan);
            }
        }

        sys_humber_vlan_range_free_node(p_head);
        if (NULL != p_head)
        {
            mem_free(p_head);
        }

        return CTC_E_NONE;
    }

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_vlan.usrid_key_entry.lchip = lchip;
            CTC_ERROR_RETURN(sys_humber_usrid_delete_vlan_entry(&usrid_vlan));
        }
    }
    else
    {
        SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
        usrid_vlan.usrid_key_entry.lchip = lchip;
        CTC_ERROR_RETURN(sys_humber_usrid_delete_vlan_entry(&usrid_vlan));
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_add_vlan_mapping_default_entry(uint16 gport, ctc_vlan_miss_t *p_action)
{
    sys_usrid_ds_entry_t usrid_entry;
    uint16               service_id;

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" gport                      :%d\n", gport);
    SYS_VLAN_MAPPING_DEBUG_INFO(" flag value                 :%d\n", p_action->flag);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_GLOBAL_PORT_CHECK(gport);

    kal_memset(&usrid_entry, 0, sizeof(sys_usrid_ds_entry_t));

    if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DO_NOTHING))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DISCARD))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_TO_CPU))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.usrid_exception_en = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_SVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->svlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_h.usr_svlan_valid = 1;
        usrid_entry.binding_data_h.usr_svlan_id = p_action->svlan_id;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_CVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->cvlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_m.usr_cvlan_valid = 1;
        usrid_entry.binding_data_m.usr_cvlan_id = p_action->cvlan_id;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_SERVICE_QUEUE))
    {
        CTC_ERROR_RETURN(sys_humber_get_physical_service_id(p_action->service_id, &service_id));
        usrid_entry.binding_data_m.svr_id_en = 1;
        usrid_entry.binding_data_m.svr_id = service_id;

        usrid_entry.usr_vlan_ptr = SYS_VLAN_SVQ_PTR;
        usrid_entry.src_queue_select = (TRUE == p_action->is_igs_srvq)? 1:0;
    }

    CTC_ERROR_RETURN(sys_humber_usrid_add_vlan_default_entry_per_port(gport, &usrid_entry));

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_vlan_mapping_default_entry(uint16 gport)
{
    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" gport                      :%d\n", gport);

    CTC_GLOBAL_PORT_CHECK(gport);
    CTC_ERROR_RETURN(sys_humber_usrid_delete_vlan_default_entry_per_port(gport));

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_vlan_mapping_all_by_port(uint16 gport)
{
    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_ERROR_RETURN(sys_humber_usrid_remove_vlan_all_by_port(gport));

    return CTC_E_NONE;
}

int32
sys_humber_vlan_add_flex_vlan_mapping_entry(uint8 label_id, ctc_flex_vlan_mapping_t *p_vlan_mapping)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint16 service_id = 0;
    sys_usrid_ipv4_entry_t usrid_ipv4;
    sys_nh_offset_array_t offset;
    sys_humber_vlan_link_t *p_head = NULL;
    sys_humber_vlan_link_t *p_tmp  = NULL;
    uint32 sum = 0;
    int32 ret = 0;

    uint8  is_svid_en     = 0;
    uint8  is_vpls_en     = 0;
    uint8  is_aps_en      = 0;
    uint8  is_fid_en      = 0;
    uint8  is_nh_en       = 0;
    uint8  is_srvc_en      = 0;
    uint8  is_cvid_en     = 0;

    is_svid_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_SVID         );
    is_vpls_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT);
    is_aps_en     = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_APS_SELECT   );
    is_fid_en     = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_FID          );
    is_nh_en      = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_NHID         );
    is_srvc_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID   );
    is_cvid_en    = CTC_FLAG_ISSET(p_vlan_mapping->action , CTC_VLAN_MAPPING_OUTPUT_CVID         );

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" label_id                  :%d\n",   label_id);
    SYS_VLAN_MAPPING_DEBUG_INFO(" key value                 :%d\n",   p_vlan_mapping->key);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svid                      :0x%X\n", p_vlan_mapping->old_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvid                      :0x%X\n", p_vlan_mapping->old_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svlan end                 :0x%X\n", p_vlan_mapping->svlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvlan end                 :0x%X\n", p_vlan_mapping->cvlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" macsa                       :0x%X%X.%X%X.%X%X\n",
                            p_vlan_mapping->macsa[0],
                            p_vlan_mapping->macsa[1],
                            p_vlan_mapping->macsa[2],
                            p_vlan_mapping->macsa[3],
                            p_vlan_mapping->macsa[4],
                            p_vlan_mapping->macsa[5]);
    SYS_VLAN_MAPPING_DEBUG_INFO(" macda                       :0x%X%X.%X%X.%X%X\n",
                            p_vlan_mapping->macda[0],
                            p_vlan_mapping->macda[1],
                            p_vlan_mapping->macda[2],
                            p_vlan_mapping->macda[3],
                            p_vlan_mapping->macda[4],
                            p_vlan_mapping->macda[5]);
    SYS_VLAN_MAPPING_DEBUG_INFO("-------mapping to-----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" new svid                  :%d\n", p_vlan_mapping->u1.new_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" new cvid                  :%d\n", p_vlan_mapping->u2.new_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" fid                       :%d\n", p_vlan_mapping->u3.fid);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_mapping);

    if (label_id > 63)
    {
        return CTC_E_INVALID_PARAM;
    }

    kal_memset(&usrid_ipv4, 0, sizeof(sys_usrid_ipv4_entry_t));

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CVID)
    {
        if ((p_vlan_mapping->old_cvid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_ipv4.valid.igs_cvid_valid = 1;
        usrid_ipv4.usrid_key_entry.cvid = (p_vlan_mapping->old_cvid & 0xFFF);
        usrid_ipv4.usrid_key_entry.igs_cvid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CTAG_COS)
    {
        usrid_ipv4.valid.ctag_cos_valid = 1;
        usrid_ipv4.usrid_key_entry.ctag_cos =  p_vlan_mapping->old_ccos;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_SVID)
    {
        if ((p_vlan_mapping->old_svid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_ipv4.valid.igs_svid_valid = 1;
        usrid_ipv4.usrid_key_entry.svid = (p_vlan_mapping->old_svid & 0xFFF);
        usrid_ipv4.usrid_key_entry.igs_svid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_STAG_COS)
    {
        usrid_ipv4.valid.stag_cos_valid = 1;
        usrid_ipv4.usrid_key_entry.stag_cos =  p_vlan_mapping->old_scos;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_MAC_SA)
    {
        usrid_ipv4.usrid_key_entry.macsa_h = ((p_vlan_mapping->macsa[0] << 8)
                                            |(p_vlan_mapping->macsa[1]));
        usrid_ipv4.usrid_key_entry.macsa_l = ((p_vlan_mapping->macsa[2] << 24)
                                            |(p_vlan_mapping->macsa[3] << 16)
                                            |(p_vlan_mapping->macsa[4] << 8)
                                            | p_vlan_mapping->macsa[5]);

        usrid_ipv4.usrid_key_entry.macsa_mask_h = ((p_vlan_mapping->macsa_mask[0] << 8)
                                                  |(p_vlan_mapping->macsa_mask[1]));
        usrid_ipv4.usrid_key_entry.macsa_mask_l = ((p_vlan_mapping->macsa_mask[2] << 24)
                                                  |(p_vlan_mapping->macsa_mask[3] << 16)
                                                  |(p_vlan_mapping->macsa_mask[4] << 8)
                                                  | p_vlan_mapping->macsa_mask[5]);
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_MAC_DA)
    {
        usrid_ipv4.usrid_key_entry.macda_h = ((p_vlan_mapping->macda[0] << 8)
                                            |(p_vlan_mapping->macda[1]));
        usrid_ipv4.usrid_key_entry.macda_l = ((p_vlan_mapping->macda[2] << 24)
                                            |(p_vlan_mapping->macda[3] << 16)
                                            |(p_vlan_mapping->macda[4] << 8)
                                            | p_vlan_mapping->macda[5]);

        usrid_ipv4.usrid_key_entry.macda_mask_h = ((p_vlan_mapping->macda_mask[0] << 8)
                                                  |(p_vlan_mapping->macda_mask[1]));
        usrid_ipv4.usrid_key_entry.macda_mask_l = ((p_vlan_mapping->macda_mask[2] << 24)
                                                  |(p_vlan_mapping->macda_mask[3] << 16)
                                                  |(p_vlan_mapping->macda_mask[4] << 8)
                                                  | p_vlan_mapping->macda_mask[5]);
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_IPV4_SA)
    {
        usrid_ipv4.usrid_key_entry.ipv4_sa = p_vlan_mapping->ipv4_sa;
        usrid_ipv4.usrid_key_entry.ipv4_sa_mask = p_vlan_mapping->ipv4_smask;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_IPV4_DA)
    {
        usrid_ipv4.usrid_key_entry.ipv4_da = p_vlan_mapping->ipv4_da;
        usrid_ipv4.usrid_key_entry.ipv4_da_mask = p_vlan_mapping->ipv4_dmask;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L2_TYPE)
    {
        usrid_ipv4.valid.l2_type_valid = 1;
        usrid_ipv4.usrid_key_entry.l2_type = p_vlan_mapping->l2_type & 0xF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L3_TYPE)
    {
        usrid_ipv4.valid.l3_type_valid = 1;
        usrid_ipv4.usrid_key_entry.l3_type = p_vlan_mapping->l3_type & 0xF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L4_SRCPORT)
    {
        usrid_ipv4.valid.l4_src_port_valid = 1;
        usrid_ipv4.usrid_key_entry.l4_src_port = p_vlan_mapping->l4src_port &0xFFFF;
    }

        if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L4_DSTPORT)
    {
        usrid_ipv4.valid.l4_dest_port_valid = 1;
        usrid_ipv4.usrid_key_entry.l4_dest_port = p_vlan_mapping->l4dest_port &0xFFFF;
    }

    if (((is_svid_en + is_vpls_en + is_aps_en) > 1)
        ||((is_fid_en + is_nh_en) > 1)
        ||((is_srvc_en + is_cvid_en) > 1))
    {
        return CTC_E_INVALID_PARAM;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT)
    {
        if (p_vlan_mapping->u1.vpls_src_port > 0x1fff)
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = 0x1FFF;
    usrid_ipv4.usrid_key_entry.usrid_label = label_id;

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_SVID)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->u1.new_svid);
        usrid_ipv4.ds_entry_usrid.binding_data_h.usr_svlan_valid = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_h.usr_svlan_id = p_vlan_mapping->u1.new_svid;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_VPLS_SRC_PORT)
    {
        usrid_ipv4.ds_entry_usrid.binding_data_h.vpls_src_port_valid = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_h.vpls_src_port = p_vlan_mapping->u1.vpls_src_port;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_SERVICE_ID)
    {
        CTC_ERROR_RETURN(sys_humber_get_physical_service_id(p_vlan_mapping->u2.service_id, &service_id));
        usrid_ipv4.ds_entry_usrid.binding_data_m.svr_id_en = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_m.svr_id = service_id;

        usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_SVQ_PTR;
        usrid_ipv4.ds_entry_usrid.src_queue_select = (TRUE == p_vlan_mapping->is_igs_srvq)? 1:0;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_APS_SELECT)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->protected_vlan);
        CTC_APS_GROUP_ID_CHECK(p_vlan_mapping->u1.aps_select_group_id);
        usrid_ipv4.ds_entry_usrid.aps_select_valid  = 1;
        usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = p_vlan_mapping->protected_vlan;
        usrid_ipv4.ds_entry_usrid.binding_data_h.aps_select_group_valid = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_h.aps_select_group_id = p_vlan_mapping->u1.aps_select_group_id;
        usrid_ipv4.ds_entry_usrid.binding_data_h.aps_select_protecting_path = (p_vlan_mapping->is_working_path == TRUE)? 0:1;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_CVID)
    {
        CTC_VLAN_RANGE_CHECK(p_vlan_mapping->u2.new_cvid);
        usrid_ipv4.ds_entry_usrid.binding_data_m.usr_cvlan_valid = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_m.usr_cvlan_id = p_vlan_mapping->u2.new_cvid;
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_FID)
    {
        CTC_FID_RANGE_CHECK(p_vlan_mapping->u3.fid);
        usrid_ipv4.ds_entry_usrid.vrfid_valid = 1;
        usrid_ipv4.ds_entry_usrid.binding_data_l.vrf_id = p_vlan_mapping->u3.fid;
    }


    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPLS)
    {
        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPLS_LRN_DIS)
        {
            usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_DIS;
        }
        else
        {
            usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPLS_PTR_LRN_EN;
        }
    }

    if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
    {
        CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_vlan_mapping->u3.nh_id, offset));
       if (p_vlan_mapping->action & CTC_VLAN_MAPPING_FLAG_VPWS)
       {
          usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_VPWS_PTR;
       }
    else
           {
              usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = SYS_VLAN_SWITCH_PTR;
           }
     }

    if (0 != p_vlan_mapping->svlan_end && (p_vlan_mapping->svlan_end >= p_vlan_mapping->old_svid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_svid, p_vlan_mapping->svlan_end, &p_head, &sum);

        usrid_ipv4.valid.igs_svid_valid = 1;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_ipv4.usrid_key_entry.chip_id = lchip;
            if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
            {
                usrid_ipv4.ds_entry_usrid.fwd_ptr_valid = 1;
                usrid_ipv4.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
            }

            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_ipv4.usrid_key_entry.svid = p_tmp->usValue;
                usrid_ipv4.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_add_ipv4_entry(&usrid_ipv4);

            }
        }

        sys_humber_vlan_range_free_node(p_head);

        return ret;
    }

    if (0 != p_vlan_mapping->cvlan_end && (p_vlan_mapping->cvlan_end >= p_vlan_mapping->old_cvid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_cvid, p_vlan_mapping->cvlan_end, &p_head, &sum);

        usrid_ipv4.valid.igs_cvid_valid = 1;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_ipv4.usrid_key_entry.chip_id = lchip;
            if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
            {
                usrid_ipv4.ds_entry_usrid.fwd_ptr_valid = 1;
                usrid_ipv4.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
            }

            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_ipv4.usrid_key_entry.cvid = p_tmp->usValue;
                usrid_ipv4.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_add_ipv4_entry(&usrid_ipv4);

            }
        }

        sys_humber_vlan_range_free_node(p_head);

        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        usrid_ipv4.usrid_key_entry.chip_id = lchip;
        if (p_vlan_mapping->action & CTC_VLAN_MAPPING_OUTPUT_NHID)
        {
            usrid_ipv4.ds_entry_usrid.fwd_ptr_valid = 1;
            usrid_ipv4.ds_entry_usrid.binding_data_l.fwd_ptr = offset[lchip];
        }

        CTC_ERROR_RETURN(sys_humber_usrid_add_ipv4_entry(&usrid_ipv4));
    }

    return CTC_E_NONE;
}


int32
sys_humber_vlan_remove_flex_vlan_mapping_entry(uint8 label_id, ctc_flex_vlan_mapping_t *p_vlan_mapping)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    sys_usrid_ipv4_entry_t usrid_ipv4;
    sys_humber_vlan_link_t *p_head = NULL;
    sys_humber_vlan_link_t *p_tmp  = NULL;
    uint32 sum = 0;
    int32 ret = CTC_E_NONE;

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" label_id                  :%d\n",  label_id);
    SYS_VLAN_MAPPING_DEBUG_INFO(" key value                  :%d\n", p_vlan_mapping->key);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svid                       :%X\n", p_vlan_mapping->old_svid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvid                       :%X\n", p_vlan_mapping->old_cvid);
    SYS_VLAN_MAPPING_DEBUG_INFO(" svlan end                  :%X\n", p_vlan_mapping->svlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" cvlan end                  :%X\n", p_vlan_mapping->cvlan_end);
    SYS_VLAN_MAPPING_DEBUG_INFO(" macsa                       :0x%X%X.%X%X.%X%X\n",
                            p_vlan_mapping->macsa[0],
                            p_vlan_mapping->macsa[1],
                            p_vlan_mapping->macsa[2],
                            p_vlan_mapping->macsa[3],
                            p_vlan_mapping->macsa[4],
                            p_vlan_mapping->macsa[5]);
    SYS_VLAN_MAPPING_DEBUG_INFO(" macda                       :0x%X%X.%X%X.%X%X\n",
                            p_vlan_mapping->macda[0],
                            p_vlan_mapping->macda[1],
                            p_vlan_mapping->macda[2],
                            p_vlan_mapping->macda[3],
                            p_vlan_mapping->macda[4],
                            p_vlan_mapping->macda[5]);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_mapping);

    if (label_id > 63)
    {
        return CTC_E_INVALID_PARAM;
    }

    kal_memset(&usrid_ipv4, 0, sizeof(usrid_ipv4));

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CVID)
    {
        if ((p_vlan_mapping->old_cvid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_ipv4.valid.igs_cvid_valid = 1;
        usrid_ipv4.usrid_key_entry.cvid = (p_vlan_mapping->old_cvid & 0xFFF);
        usrid_ipv4.usrid_key_entry.igs_cvid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_CTAG_COS)
    {
        usrid_ipv4.valid.ctag_cos_valid = 1;
        usrid_ipv4.usrid_key_entry.ctag_cos =  p_vlan_mapping->old_ccos;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_SVID)
    {
        if ((p_vlan_mapping->old_svid & 0xFFF) >= CTC_MAX_VLAN_ID)
        {
            return CTC_E_INVALID_VLAN_ID;
        }
        usrid_ipv4.valid.igs_svid_valid = 1;
        usrid_ipv4.usrid_key_entry.svid = (p_vlan_mapping->old_svid & 0xFFF);
        usrid_ipv4.usrid_key_entry.igs_svid_mask = 0xFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_STAG_COS)
    {
        usrid_ipv4.valid.stag_cos_valid = 1;
        usrid_ipv4.usrid_key_entry.stag_cos =  p_vlan_mapping->old_scos;
    }


    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_MAC_SA)
    {
        usrid_ipv4.usrid_key_entry.macsa_h = ((p_vlan_mapping->macsa[0] << 8)
                                            |(p_vlan_mapping->macsa[1]));
        usrid_ipv4.usrid_key_entry.macsa_l = ((p_vlan_mapping->macsa[2] << 24)
                                            |(p_vlan_mapping->macsa[3] << 16)
                                            |(p_vlan_mapping->macsa[4] << 8)
                                            | p_vlan_mapping->macsa[5]);

        usrid_ipv4.usrid_key_entry.macsa_mask_h = ((p_vlan_mapping->macsa_mask[0] << 8)
                                                  |(p_vlan_mapping->macsa_mask[1]));
        usrid_ipv4.usrid_key_entry.macsa_mask_l = ((p_vlan_mapping->macsa_mask[2] << 24)
                                                  |(p_vlan_mapping->macsa_mask[3] << 16)
                                                  |(p_vlan_mapping->macsa_mask[4] << 8)
                                                  | p_vlan_mapping->macsa_mask[5]);

    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_MAC_DA)
    {
        usrid_ipv4.usrid_key_entry.macda_h = ((p_vlan_mapping->macda[0] << 8)
                                            |(p_vlan_mapping->macda[1]));
        usrid_ipv4.usrid_key_entry.macda_l = ((p_vlan_mapping->macda[2] << 24)
                                            |(p_vlan_mapping->macda[3] << 16)
                                            |(p_vlan_mapping->macda[4] << 8)
                                            | p_vlan_mapping->macda[5]);

        usrid_ipv4.usrid_key_entry.macda_mask_h = ((p_vlan_mapping->macda_mask[0] << 8)
                                                  |(p_vlan_mapping->macda_mask[1]));
        usrid_ipv4.usrid_key_entry.macda_mask_l = ((p_vlan_mapping->macda_mask[2] << 24)
                                                  |(p_vlan_mapping->macda_mask[3] << 16)
                                                  |(p_vlan_mapping->macda_mask[4] << 8)
                                                  | p_vlan_mapping->macda_mask[5]);
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_IPV4_SA)
    {
        usrid_ipv4.usrid_key_entry.ipv4_sa = p_vlan_mapping->ipv4_sa;
        usrid_ipv4.usrid_key_entry.ipv4_sa_mask = p_vlan_mapping->ipv4_smask;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_IPV4_DA)
    {
        usrid_ipv4.usrid_key_entry.ipv4_sa = p_vlan_mapping->ipv4_da;
        usrid_ipv4.usrid_key_entry.ipv4_sa_mask = p_vlan_mapping->ipv4_dmask;
    }


    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L2_TYPE)
    {
        usrid_ipv4.valid.l2_type_valid = 1;
        usrid_ipv4.usrid_key_entry.l2_type = p_vlan_mapping->l2_type & 0xF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L3_TYPE)
    {
        usrid_ipv4.valid.l3_type_valid = 1;
        usrid_ipv4.usrid_key_entry.l3_type = p_vlan_mapping->l3_type & 0xF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L4_SRCPORT)
    {
        usrid_ipv4.valid.l4_src_port_valid = 1;
        usrid_ipv4.usrid_key_entry.l4_src_port = p_vlan_mapping->l4src_port &0xFFFF;
    }

    if (p_vlan_mapping->key & CTC_VLAN_MAPPING_KEY_L4_DSTPORT)
    {
        usrid_ipv4.valid.l4_dest_port_valid = 1;
        usrid_ipv4.usrid_key_entry.l4_dest_port = p_vlan_mapping->l4dest_port &0xFFFF;
    }

    usrid_ipv4.usrid_key_entry.usrid_label = label_id;

    if (0 != p_vlan_mapping->svlan_end && (p_vlan_mapping->svlan_end >= p_vlan_mapping->old_svid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));
        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_svid, p_vlan_mapping->svlan_end, &p_head, &sum);

        usrid_ipv4.valid.igs_svid_valid = 1;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_ipv4.usrid_key_entry.chip_id = lchip;
            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_ipv4.usrid_key_entry.svid = p_tmp->usValue;
                usrid_ipv4.usrid_key_entry.igs_svid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_delete_ipv4_entry(&usrid_ipv4);

            }
        }

        sys_humber_vlan_range_free_node(p_head);

        return CTC_E_NONE;
    }

    if (0 != p_vlan_mapping->cvlan_end && (p_vlan_mapping->cvlan_end >= p_vlan_mapping->old_cvid))
    {
        p_head = mem_malloc(MEM_VLAN_MODULE, sizeof(sys_humber_vlan_link_t));
        if (NULL == p_head)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_head, 0, sizeof(sys_humber_vlan_link_t));

        sys_humber_vlan_range_split_vlan(p_vlan_mapping->old_cvid, p_vlan_mapping->cvlan_end, &p_head, &sum);

        usrid_ipv4.valid.igs_cvid_valid = 1;

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            usrid_ipv4.usrid_key_entry.chip_id = lchip;
            for (p_tmp = p_head->pstNodeNext; NULL != p_tmp; p_tmp = p_tmp->pstNodeNext)
            {
                usrid_ipv4.usrid_key_entry.cvid = p_tmp->usValue;
                usrid_ipv4.usrid_key_entry.igs_cvid_mask = p_tmp->usMask;
                ret = ret? ret:sys_humber_usrid_delete_ipv4_entry(&usrid_ipv4);

            }
        }

        sys_humber_vlan_range_free_node(p_head);

        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        usrid_ipv4.usrid_key_entry.chip_id = lchip;
        CTC_ERROR_RETURN(sys_humber_usrid_delete_ipv4_entry(&usrid_ipv4));
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_add_flex_vlan_mapping_default_entry(uint8 label_id, ctc_vlan_miss_t *p_action)
{
    sys_usrid_ds_entry_t usrid_entry;
    uint16               service_id;
    uint8 lchip = 0;
    uint8 lchip_num = 0;

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" label_id                  :%d\n", label_id);
    SYS_VLAN_MAPPING_DEBUG_INFO(" flag value                :%d\n", p_action->flag);

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }

    if (label_id > 63)
    {
        return CTC_E_INVALID_PARAM;
    }

    kal_memset(&usrid_entry, 0, sizeof(sys_usrid_ds_entry_t));

    if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DO_NOTHING))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DISCARD))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_TO_CPU))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.usrid_exception_en = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_SVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->svlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_h.usr_svlan_valid = 1;
        usrid_entry.binding_data_h.usr_svlan_id = p_action->svlan_id;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_CVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->cvlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_m.usr_cvlan_valid = 1;
        usrid_entry.binding_data_m.usr_cvlan_id = p_action->cvlan_id;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_SERVICE_QUEUE))
    {
        CTC_ERROR_RETURN(sys_humber_get_physical_service_id(p_action->service_id, &service_id));
        usrid_entry.binding_data_m.svr_id_en = 1;
        usrid_entry.binding_data_m.svr_id = service_id;

        usrid_entry.usr_vlan_ptr = SYS_VLAN_SVQ_PTR;
        usrid_entry.src_queue_select = (TRUE == p_action->is_igs_srvq)? 1:0;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_usrid_add_ipv4_default_entry_per_label(lchip,label_id, &usrid_entry));
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_flex_vlan_mapping_default_entry(uint8 label_id)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;

    if (FALSE == is_vlan_mapping_init)
    {
        return CTC_E_NOT_INIT;
    }

    if (label_id > 63)
    {
        return CTC_E_INVALID_PARAM;
    }

    SYS_VLAN_MAPPING_DEBUG_FUNC();
    SYS_VLAN_MAPPING_DEBUG_INFO("------vlan mapping----------\n");
    SYS_VLAN_MAPPING_DEBUG_INFO(" label_id                 :%d\n",   label_id);

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_usrid_delete_ipv4_default_entry_per_label(lchip,label_id));
    }

    return CTC_E_NONE;
}




