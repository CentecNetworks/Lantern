/**
 @file sys_humber_aclqos_label.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/

#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_macro.h"
#include "ctc_vector.h"
#include "ctc_aclqos.h"
#include "ctc_debug.h"
#include "ctc_hash.h"

#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_port.h"
#include "sys_humber_l3if.h"
#include "sys_humber_vlan.h"
#include "sys_humber_aclqos_label.h"
#include "sys_humber_queue_enq.h"

/****************************************************************************
 *
 * Defines and Macros
 *
 ****************************************************************************/
#define MAX_ACLQOS_INGRESS_LABEL_START              0
#define MAX_ACLQOS_INGRESS_LABEL_NUM                128
#define MAX_ACLQOS_EGRESS_LABEL_START               128
#define MAX_ACLQOS_EGRESS_LABEL_NUM                 128
#define MAX_ACL_PBR_LABEL_NUM                       64
#define MAX_ACLQOS_LABEL_REF_NUM        0xFFFF
#define SYS_ACLQOS_LABEL_BUCKET_SIZE    32

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/


ctc_hash_t* p_aclqos_label_hash;
ctc_hash_t* p_service_label_hash;
ctc_vector_t *p_aclqos_port_label[CTC_MAX_LOCAL_CHIP_NUM][CTC_BOTH_DIRECTION][2];
ctc_vector_t *p_aclqos_vlan_label[CTC_BOTH_DIRECTION][2];
ctc_vector_t *p_acl_pbr_label;

/****************************************************************************
  *
  * Function
  *
  ****************************************************************************/

/**
 @brief get l3 interface property type by combination of direction, label type, and bool flag
               indicating if it is intended to get label or enable status
*/
static int32
_sys_humber_aclqos_get_l3if_prop_type (ctc_direction_t dir, ctc_aclqos_label_type_t type,
                                       bool is_label, sys_l3if_aclaos_property_t* p_l3if_prop)
{
    CTC_PTR_VALID_CHECK(p_l3if_prop);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    switch(type)
    {
        case CTC_ACL_LABEL:
            if (CTC_INGRESS == dir)
            {
                *p_l3if_prop = is_label ? SYS_L3IF_PROP_L3ACL_LABEL : SYS_L3IF_PROP_L3ACL_EN;
            }
            else
            {
                *p_l3if_prop = is_label ? SYS_L3IF_EGS_PROP_L3ACL_LABEL : SYS_L3IF_EGS_PROP_L3ACL_EN;
            }
            break;

        case CTC_QOS_LABEL:
            if (CTC_INGRESS == dir)
            {
                *p_l3if_prop = is_label ? SYS_L3IF_PROP_L3QOS_LABEL : SYS_L3IF_PROP_L3QOS_LKUP_EN;
            }
            else
            {
                *p_l3if_prop = is_label ? SYS_L3IF_EGS_PROP_L3QOS_LABEL : SYS_L3IF_EGS_PROP_L3QOS_LOOKUP_EN;
            }
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief get acl/qos label from the given port
*/
static int32
_sys_humber_aclqos_get_port_label(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, sys_aclqos_label_t** pp_label)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lport = 0;

    CTC_PTR_VALID_CHECK(pp_label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    p_label = ctc_vector_get(p_aclqos_port_label[lchip][dir][type], lport);

    *pp_label = p_label;

    return CTC_E_NONE;
}


/**
 @brief set acl/qos label to the given port
*/
static int32
_sys_humber_aclqos_set_port_label(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint8 label)
{
    SYS_ACLQOS_LABEL_DBG_FUNC();

    switch(type)
    {
        case CTC_ACL_LABEL:
            CTC_ERROR_RETURN(sys_humber_port_set_l2acl_label(gport, dir, label));
            break;

        case CTC_QOS_LABEL:
            CTC_ERROR_RETURN(sys_humber_port_set_l2qos_label(gport, dir, label));
            break;

        default:
            return CTC_E_ACLQOS_INVALID_LABEL_TYPE;
    }

    return CTC_E_NONE;
}


/**
 @brief get acl/qos label from the given vlan
*/
static int32
_sys_humber_aclqos_get_vlan_label(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type,
                                  sys_aclqos_label_t** pp_label)
{
    sys_aclqos_label_t *p_label;

    CTC_PTR_VALID_CHECK(pp_label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    p_label = ctc_vector_get(p_aclqos_vlan_label[dir][type], vid);

    *pp_label = p_label;

    return CTC_E_NONE;
}


/**
 @brief set acl/qos label to the given l3 interface
*/
static int32
_sys_humber_aclqos_set_vlan_label(uint16 l3if_id, ctc_direction_t dir, ctc_aclqos_label_type_t type,
                                  uint32 label[CTC_MAX_LOCAL_CHIP_NUM])
{
    sys_l3if_aclaos_property_t l3if_prop;

    CTC_PTR_VALID_CHECK(label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* set acl/qos label on vlan interface */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_l3if_prop_type(dir, type, TRUE, &l3if_prop));

    CTC_ERROR_RETURN(sys_humber_l3if_set_aclqos_property(l3if_id, l3if_prop, label));

    return CTC_E_NONE;
}


/**
 @brief unset acl/qos label to the given l3 interface
*/
static int32
_sys_humber_aclqos_unset_vlan_label(uint16 l3if_id, ctc_direction_t dir, ctc_aclqos_label_type_t type)
{
    uint32 label[CTC_MAX_LOCAL_CHIP_NUM];
    sys_l3if_aclaos_property_t l3if_prop;

    CTC_PTR_VALID_CHECK(label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* unset acl/qos label on vlan interface */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_l3if_prop_type(dir, type, TRUE, &l3if_prop));

    kal_memset(label, 0, sizeof(label));
    CTC_ERROR_RETURN(sys_humber_l3if_set_aclqos_property(l3if_id, l3if_prop, label));

    return CTC_E_NONE;
}

/**
 @brief get acl pbr label from the given l3 interface
*/
static int32
_sys_humber_acl_get_pbr_label(uint16 l3if_id, sys_aclqos_label_t** pp_label)
{
    sys_aclqos_label_t *p_label;

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);
    CTC_PTR_VALID_CHECK(pp_label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    p_label = ctc_vector_get(p_acl_pbr_label, l3if_id);

    *pp_label = p_label;

    return CTC_E_NONE;
}


/**
 @brief set acl pbr label to the given l3 interface
*/
static int32
_sys_humber_acl_set_pbr_label(uint16 l3if_id, uint32 label)
{
    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);

    /* set acl pbr label on l3 interface */
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id, CTC_L3IF_PROP_PBR_LABEL, label));

    return CTC_E_NONE;
}


/**
 @brief unset acl pbr label to the given l3 interface
*/
static int32
_sys_humber_acl_unset_pbr_label(uint16 l3if_id)
{
    uint32 label = 0;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);

    /* unset acl pbr label on l3 interface */
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id, CTC_L3IF_PROP_PBR_LABEL, label));

    return CTC_E_NONE;
}


static uint32
_sys_humber_aclqos_label_hash_key(void* data)
{
    sys_aclqos_label_t *p_label = (sys_aclqos_label_t *)data;

    return p_label->id;
}


static bool
_sys_humber_aclqos_label_hash_cmp(void* data1, void* data2)
{
    sys_aclqos_label_t *p_label1 = (sys_aclqos_label_t *)data1;
    sys_aclqos_label_t *p_label2 = (sys_aclqos_label_t *)data2;

    return (p_label1->id == p_label2->id);
}

/**
 @brief Lookup acl/qos label according to the given label id
*/
int32
sys_humber_aclqos_label_lookup(uint32 label_id, uint8 is_service_label, sys_aclqos_label_t** pp_label)
{
    sys_aclqos_label_t label;

    CTC_NOT_ZERO_CHECK(label_id);
    CTC_PTR_VALID_CHECK(pp_label);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    label.id = label_id;

    if(is_service_label)
    {
        *pp_label = ctc_hash_lookup(p_service_label_hash, &label);
    }
    else
    {
        *pp_label = ctc_hash_lookup(p_aclqos_label_hash, &label);
    }

    return CTC_E_NONE;
}

 /**
  @brief Lookup acl/qos enable/disable on the given port
 */
 int32
 sys_humber_aclqos_port_lkp_enable(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable)
 {
    SYS_ACLQOS_LABEL_DBG_FUNC();

    switch(label_type)
    {
        case CTC_ACL_LABEL:
            CTC_ERROR_RETURN(sys_humber_port_set_l2acl_prio(gport, dir, enable));
            CTC_ERROR_RETURN(sys_humber_port_set_l2acl_enable(gport, dir, enable));
            break;

        case CTC_QOS_LABEL:
            CTC_ERROR_RETURN(sys_humber_port_set_l2qos_prio(gport, dir, enable));
            CTC_ERROR_RETURN(sys_humber_port_set_l2qos_enable(gport, dir, enable));
            break;

        default:
            return CTC_E_ACLQOS_INVALID_LABEL_TYPE;
    }

    return CTC_E_NONE;
 }

/**
 @brief Lookup acl/qos enable/disable on the given vlan
*/
int32
sys_humber_aclqos_vlan_lkp_enable(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable)
 {
    uint8 lchip = 0;
    uint16 l3if_id = 0;
    sys_l3if_aclaos_property_t l3if_prop;
    sys_vlan_info_t vlan_info;
    uint32 lkp_enable[CTC_MAX_LOCAL_CHIP_NUM];

    kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
    vlan_info.vid = vid;
    CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &l3if_id));

    /* enable acl/qos on vlan interface */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_l3if_prop_type(dir, label_type, FALSE, &l3if_prop));

    for (lchip = 0; lchip < CTC_MAX_LOCAL_CHIP_NUM; lchip++)
    {
        lkp_enable[lchip] = (uint32)(enable);
    }

    CTC_ERROR_RETURN(sys_humber_l3if_set_aclqos_property(l3if_id, l3if_prop, lkp_enable));

    return CTC_E_NONE;
 }

/**
 @brief Lookup acl pbr enable/disable on the given l3 interface
*/
int32
sys_humber_acl_pbr_lkp_enable(uint16 l3if_id, bool enable)
{
    uint32 lkp_type;

    /* enable acl pbr on l3 interface */
    if(TRUE == enable)
    {
        lkp_type = CTC_L3IF_IPSA_LKUP_TYPE_PBR;
    }
    else
    {
        lkp_type = CTC_L3IF_IPSA_LKUP_TYPE_NONE;
    }
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id, CTC_L3IF_PROP_IPV4_SA_TYPE, lkp_type));
    CTC_ERROR_RETURN(sys_humber_l3if_set_property(l3if_id, CTC_L3IF_PROP_IPV6_SA_TYPE, lkp_type));

    return CTC_E_NONE;
}

 /**
 @brief create acl/qos port label
*/
int32
sys_humber_aclqos_port_label_new(ctc_aclqos_label_type_t type, uint32 label_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0, lchip_num = 0;
    ctc_aclqos_key_type_t key_type;
    sys_aclqos_label_index_t *p_index;
    int32 ret = 0;

    CTC_NOT_ZERO_CHECK(label_id);
    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if label_id is already created */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_LABEL_IN_USE;
    }

    /* create label */
    p_label = (sys_aclqos_label_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_t));
    if (!p_label)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_label, 0, sizeof(sys_aclqos_label_t));
    p_label->id   = label_id;
    p_label->type = (type == CTC_ACL_LABEL) ? SYS_PORT_ACL_LABEL : SYS_PORT_QOS_LABEL;
    p_label->dir  = 0xff;
    p_label->ref  = 0;

    /*global aclqos process*/
    switch(p_label->id)
    {
        case CTC_GLOBAL_ACL_INGRESS_LABEL_ID_HEAD:
            p_label->dir = CTC_INGRESS;
            break;

        case CTC_GLOBAL_ACL_INGRESS_LABEL_ID_TAIL:
            p_label->dir = CTC_INGRESS;
            break;

        case CTC_GLOBAL_ACL_EGRESS_LABEL_ID_HEAD:
            p_label->dir = CTC_EGRESS;
            break;

        case CTC_GLOBAL_ACL_EGRESS_LABEL_ID_TAIL:
            p_label->dir = CTC_EGRESS;
            break;

        case CTC_GLOBAL_QOS_INGRESS_LABEL_ID_HEAD:
            p_label->dir = CTC_INGRESS;
            break;

        case CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL:
            p_label->dir = CTC_INGRESS;
            break;

        case CTC_GLOBAL_QOS_EGRESS_LABEL_ID_HEAD:
            p_label->dir = CTC_EGRESS;
            break;

        case CTC_GLOBAL_QOS_EGRESS_LABEL_ID_TAIL:
            p_label->dir = CTC_EGRESS;
            break;

        default:
            break;
    }

    if(0xff != p_label->dir)
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                ret = CTC_E_NO_MEMORY;
                goto ERR;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));
            if(CTC_INGRESS == p_label->dir)
            {
                p_index->index = 0;
                p_index->index_mask = 0x80;
            }
            else
            {
                p_index->index = 0x80;
                p_index->index_mask = 0x80;
            }

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[lchip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("chip = %d, global label index = %d\n", lchip, p_label->p_index[lchip]->index);
        }
    }

    if(NULL == ctc_hash_insert(p_aclqos_label_hash, p_label))
    {
        if(0xff != p_label->dir)
        {
            ret = CTC_E_UNEXPECT;
            goto ERR;
        }

        mem_free(p_label);
        return CTC_E_UNEXPECT;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("create port acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("===========================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("label_id = %d, type = %d\n", label_id, type);

    return CTC_E_NONE;

ERR:
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_label->p_index[lchip])
        {
            mem_free(p_label->p_index[lchip]);
        }
    }
    mem_free(p_label);

    return ret;
}


/**
 @brief delete acl/qos port label
*/
int32
sys_humber_aclqos_port_label_delete(ctc_aclqos_label_type_t type, uint32 label_id)
{
    sys_aclqos_label_t *p_label;

    SYS_ACLQOS_DEL_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if label_id is exist */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    CTC_EQUAL_CHECK(p_label->ref, 0);

    /* release the label */
    ctc_hash_remove(p_aclqos_label_hash, p_label);
    mem_free(p_label);

    SYS_ACLQOS_LABEL_DBG_INFO("remove port acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("=================================");
    SYS_ACLQOS_LABEL_DBG_INFO("label id = %d, type = %d\n", label_id, type);

    return CTC_E_NONE;
}


/**
 @brief set acl/qos label for the given port
*/
int32
sys_humber_aclqos_port_label_apply(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32 label_id)
{
    uint8 lchip = 0;
    uint8 local_chip = 0, lchip_num = 0;
    uint8 lport = 0;
    int32 ret = 0;
    uint32 index;
    sys_aclqos_label_t *p_label;
    sys_humber_opf_t opf;
    sys_aclqos_label_index_t *p_index;
    ctc_aclqos_key_type_t key_type;

    SYS_ACLQOS_APPLY_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /* check if port has been allocated with a label */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_port_label(gport, dir, type, &p_label));
    if (p_label)     /* have enabled on this port, return silently */
    {
        return CTC_E_ACLQOS_HAVE_ENABLED;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    if (!IS_PORT_LABEL(p_label->type))
    {
        return CTC_E_ACLQOS_DIFFERENT_TYPE;
    }

    CTC_NOT_EQUAL_CHECK(p_label->ref, MAX_ACLQOS_LABEL_REF_NUM);

    if (p_label->dir == 0xff)
    {
        p_label->dir = dir;
    }
    else if (p_label->dir != dir)
    {
        return CTC_E_ACLQOS_DIFFERENT_DIR;
    }

    /* For first apply, should allocate physical label index. */
    if(0 == p_label->ref)
    {
        if(type == CTC_ACL_LABEL)
        {
            if(CTC_INGRESS == dir)
            {
                opf.pool_type = OPF_ACL_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_ACL_EGRESS_LABEL;
            }
        }
        else
        {
            if(CTC_INGRESS == dir)
            {
                opf.pool_type = OPF_QOS_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_QOS_EGRESS_LABEL;
            }
        }

        lchip_num = sys_humber_get_local_chip_num();
        for (local_chip = 0; local_chip < lchip_num; local_chip++)
        {
            opf.pool_index = local_chip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &index));

            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                sys_humber_opf_free_offset(&opf, 1, index);
                ret = CTC_E_NO_MEMORY;
                goto ERR;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));
            p_index->index = index;
            p_index->index_mask = 0xFF;

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[local_chip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("chip = %d, label index = %d\n", local_chip, p_label->p_index[local_chip]->index);
        }
    }

    if(FALSE == ctc_vector_add(p_aclqos_port_label[lchip][dir][type], lport, p_label))
    {
        if(0 == p_label->ref)   /*first apply*/
        {
            ret = CTC_E_UNEXPECT;
            goto ERR;
        }
        return CTC_E_UNEXPECT;
    }

    p_label->ref ++;

    /* write to asic */
    ret = _sys_humber_aclqos_set_port_label(gport, dir, type, p_label->p_index[lchip]->index);
    if(CTC_E_NONE != ret)
    {
        p_label->ref --;
        ctc_vector_del(p_aclqos_port_label[lchip][dir][type], lport);

        if(0 == p_label->ref)   /*first apply*/
        {
            goto ERR;
        }
        return ret;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("install port acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("gport = %d, dir = %d, label_id = %d\n", gport, dir, label_id);
    SYS_ACLQOS_LABEL_DBG_INFO("ref = %d\n", p_label->ref);

    return CTC_E_NONE;

ERR:    /*process first apply rollback*/
    for (local_chip = 0; local_chip < lchip_num; local_chip++)
    {
        opf.pool_index = local_chip;
        if (p_label->p_index[local_chip])
        {
            sys_humber_opf_free_offset(&opf, 1, p_label->p_index[local_chip]->index);
            mem_free(p_label->p_index[local_chip]);
        }
    }

    return ret;
}

/**
 @brief unset acl/qos label for the given port
*/
int32
sys_humber_aclqos_port_label_unapply(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    sys_aclqos_label_t *p_label;
    sys_humber_opf_t opf;
    uint8 local_chip = 0, lchip_num = 0;
    ctc_aclqos_key_type_t key_type;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /* get port label */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_port_label(gport, dir, type, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    CTC_PTR_VALID_CHECK(p_label->p_index[lchip]);

    CTC_NOT_EQUAL_CHECK(p_label->ref, 0);
    p_label->ref--;

    /* For last unapply, should free physical label index. */
    if(0 == p_label->ref)
    {
        if(type == CTC_ACL_LABEL)
        {
            if(CTC_INGRESS == p_label->dir)
            {
                opf.pool_type = OPF_ACL_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_ACL_EGRESS_LABEL;
            }
        }
        else
        {
            if(CTC_INGRESS == p_label->dir)
            {
                opf.pool_type = OPF_QOS_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_QOS_EGRESS_LABEL;
            }
        }

        lchip_num = sys_humber_get_local_chip_num();
        for (local_chip = 0; local_chip < lchip_num; local_chip++)
        {
            if (!p_label->p_index[local_chip])
            {
                continue;
            }

            /* check label usage status */
            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                if (!CTC_LIST_POINTER_ISEMPTY(&(p_label->p_index[local_chip]->entry_list[key_type])))
                {
                    p_label->ref ++;
                    return CTC_E_ACLQOS_LABEL_IN_USE;
                }
            }

            opf.pool_index = local_chip;
            sys_humber_opf_free_offset(&opf, 1, p_label->p_index[local_chip]->index);

            mem_free(p_label->p_index[local_chip]);
        }
    }

    ctc_vector_del(p_aclqos_port_label[lchip][dir][type], lport);

    SYS_ACLQOS_LABEL_DBG_INFO("uninstall port acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("=================================");
    SYS_ACLQOS_LABEL_DBG_INFO("gport = %d, dir = %d, type = %d\n", gport, dir, type);

    return CTC_E_NONE;
}


int32
sys_humber_aclqos_port_label_id_get(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32* label_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lport = 0;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    CTC_PTR_VALID_CHECK(label_id);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /* get port label */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_port_label(gport, dir, type, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    /* get label id */
    *label_id = p_label->id;

    SYS_ACLQOS_LABEL_DBG_INFO("get port acl/qos label id\n");
    SYS_ACLQOS_LABEL_DBG_INFO("=================================");
    SYS_ACLQOS_LABEL_DBG_INFO("gport = %d, dir = %d, type = %d, label id = %d\n", gport, dir, type, *label_id);

    return CTC_E_NONE;
}


/**
 @brief create acl/qos vlan label
*/
int32
sys_humber_aclqos_vlan_label_new(ctc_aclqos_label_type_t type, uint32 label_id)
{
    sys_aclqos_label_t *p_label;

    CTC_NOT_ZERO_CHECK(label_id);
    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* Check if the label_id is already created */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_LABEL_IN_USE;
    }

    /* create label */
    p_label = (sys_aclqos_label_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_t));
    if (!p_label)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_label, 0, sizeof(sys_aclqos_label_t));
    p_label->id   = label_id;
    p_label->type = (type == CTC_ACL_LABEL) ? SYS_VLAN_ACL_LABEL : SYS_VLAN_QOS_LABEL;
    p_label->dir  = 0xff;
    p_label->ref  = 0;

    if(NULL == ctc_hash_insert(p_aclqos_label_hash, p_label))
    {
        mem_free(p_label);
        return CTC_E_UNEXPECT;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("create   vlan acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("===========================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("label_id = %d, type = %d\n", label_id, type);

    return CTC_E_NONE;
}


/**
 @brief delete acl/qos vlan label
*/
int32
sys_humber_aclqos_vlan_label_delete(ctc_aclqos_label_type_t type, uint32 label_id)
{
    sys_aclqos_label_t *p_label;

    SYS_ACLQOS_DEL_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if label_id is exist */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    CTC_EQUAL_CHECK(p_label->ref, 0);

    /* release the label*/
    ctc_hash_remove(p_aclqos_label_hash, p_label);
    mem_free(p_label);

    SYS_ACLQOS_LABEL_DBG_INFO("remove vlan acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("label id = %d, type = %d\n", label_id, type);

    return CTC_E_NONE;
}


/**
 @brief set acl/qos label for the given vlan
*/
int32
sys_humber_aclqos_vlan_label_apply(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32 label_id)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    uint16 l3if_id;
    int32 ret = 0;
    uint32 index[CTC_MAX_LOCAL_CHIP_NUM];
    sys_vlan_info_t vlan_info;
    sys_aclqos_label_t *p_label;
    sys_humber_opf_t opf;
    sys_aclqos_label_index_t *p_index;
    ctc_aclqos_key_type_t key_type;
    uint32 offset;

    SYS_ACLQOS_APPLY_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if a label is allocated to the vlan */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_vlan_label(vid, dir, type, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_HAVE_ENABLED;
    }

    /* get vlan acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    if (!IS_VLAN_LABEL(p_label->type))
    {
        return CTC_E_ACLQOS_DIFFERENT_TYPE;
    }

    CTC_NOT_EQUAL_CHECK(p_label->ref, MAX_ACLQOS_LABEL_REF_NUM);

    if (p_label->dir == 0xff)
    {
        p_label->dir = dir;
    }
    else if (p_label->dir != dir)
    {
        return CTC_E_ACLQOS_DIFFERENT_DIR;
    }

    /* For first apply, should allocate physical label index. */
    if(0 == p_label->ref)
    {
        if(type == CTC_ACL_LABEL)
        {
            if(CTC_INGRESS == dir)
            {
                opf.pool_type = OPF_ACL_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_ACL_EGRESS_LABEL;
            }
        }
        else
        {
            if(CTC_INGRESS == dir)
            {
                opf.pool_type = OPF_QOS_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_QOS_EGRESS_LABEL;
            }
        }

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                sys_humber_opf_free_offset(&opf, 1, offset);
                ret = CTC_E_NO_MEMORY;
                goto ERR;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));
            p_index->index = offset;
            p_index->index_mask = 0xFF;

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[lchip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("chip = %d, label index = %d\n", lchip, p_label->p_index[lchip]->index);
        }

    }


    kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
    vlan_info.vid = vid;
    ret = sys_humber_vlan_get_l3if_id(&vlan_info, &l3if_id);
    if(CTC_E_NONE != ret)
    {
        if(0 == p_label->ref)   /*first apply*/
        {
            goto ERR;
        }
        return ret;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        index[lchip] = p_label->p_index[lchip]->index;
    }

    if(FALSE == ctc_vector_add(p_aclqos_vlan_label[dir][type], vid, p_label))
    {
        if(0 == p_label->ref)   /*first apply*/
        {
            ret = CTC_E_UNEXPECT;
            goto ERR;
        }
        return CTC_E_UNEXPECT;
    }

    p_label->ref ++;

    ret = _sys_humber_aclqos_set_vlan_label(l3if_id, dir, type, index);
    if(CTC_E_NONE != ret)
    {
        p_label->ref --;
        ctc_vector_del(p_aclqos_vlan_label[dir][type], vid);

        if(0 == p_label->ref)   /*first apply*/
        {
            goto ERR;
        }
        return ret;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("install vlan acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("vid = %d, dir = %d, label_id = %d\n", vid, dir, label_id);
    SYS_ACLQOS_LABEL_DBG_INFO("ref = %d\n", p_label->ref);

    return CTC_E_NONE;

ERR:    /*process first apply rollback*/
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        opf.pool_index = lchip;
        if (p_label->p_index[lchip])
        {
            sys_humber_opf_free_offset(&opf, 1, p_label->p_index[lchip]->index);
            mem_free(p_label->p_index[lchip]);
        }
    }

    return ret;
}

/**
 @brief unset acl/qos label for the given vlan
*/
int32
sys_humber_aclqos_vlan_label_unapply(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type)
{
    uint16 l3if_id;
    sys_vlan_info_t vlan_info;
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0, lchip_num = 0;
    ctc_aclqos_key_type_t key_type;
    sys_humber_opf_t opf;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* get vlan acl/qos label */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_vlan_label(vid, dir, type, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    CTC_NOT_EQUAL_CHECK(p_label->ref, 0);
    p_label->ref --;

    /* For last unapply, should free physical label index. */
    if(0 == p_label->ref)
    {
        /* release the label index*/
        if(type == CTC_ACL_LABEL)
        {
            if(CTC_INGRESS == p_label->dir)
            {
                opf.pool_type = OPF_ACL_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_ACL_EGRESS_LABEL;
            }
        }
        else
        {
            if(CTC_INGRESS == p_label->dir)
            {
                opf.pool_type = OPF_QOS_INGRESS_LABEL;
            }
            else
            {
                opf.pool_type = OPF_QOS_EGRESS_LABEL;
            }
        }

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            if (!p_label->p_index[lchip])
            {
                continue;
            }

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                if (!CTC_LIST_POINTER_ISEMPTY(&(p_label->p_index[lchip]->entry_list[key_type])))
                {
                    p_label->ref ++;
                    return CTC_E_ACLQOS_LABEL_IN_USE;
                }
            }

            opf.pool_index = lchip;
            sys_humber_opf_free_offset(&opf, 1, p_label->p_index[lchip]->index);

            mem_free(p_label->p_index[lchip]);
        }
    }

    /* write to asic */
    kal_memset(&vlan_info, 0, sizeof(sys_vlan_info_t));
    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
    vlan_info.vid = vid;
    CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &l3if_id));
    CTC_ERROR_RETURN(_sys_humber_aclqos_unset_vlan_label(l3if_id, dir, type));

    /* remove vlan acl/qos label from db */
    ctc_vector_del(p_aclqos_vlan_label[dir][type], vid);

    SYS_ACLQOS_LABEL_DBG_INFO("unstall vlan acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("vid = %d, dir = %d, type = %d\n", vid, dir, type);

    return CTC_E_NONE;
}

int32
sys_humber_aclqos_vlan_label_id_get(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32* label_id)
{
    sys_aclqos_label_t *p_label;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    CTC_PTR_VALID_CHECK(label_id);

    /* get vlan acl/qos label */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_vlan_label(vid, dir, type, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    /* get label id */
    *label_id = p_label->id;

    SYS_ACLQOS_LABEL_DBG_INFO("get vlan acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("vid = %d, dir = %d, type = %d, label id = %d\n", vid, dir, type, *label_id);

    return CTC_E_NONE;
}

/**
 @brief create acl pbr label
*/
int32
sys_humber_acl_pbr_label_new(uint32 label_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0, lchip_num = 0;
    ctc_aclqos_key_type_t key_type;
    sys_aclqos_label_index_t *p_index;
    int32 ret = 0;

    CTC_NOT_ZERO_CHECK(label_id);
    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* Check if the label_id is already created */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_LABEL_IN_USE;
    }

    /* create label */
    p_label = (sys_aclqos_label_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_t));
    if (!p_label)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_label, 0, sizeof(sys_aclqos_label_t));
    p_label->id   = label_id;
    p_label->type = SYS_PBR_ACL_LABEL;
    p_label->dir  = 0xff;
    p_label->ref  = 0;

    if((CTC_GLOBAL_ACL_PBR_LABEL_ID_HEAD == p_label->id)
        || (CTC_GLOBAL_ACL_PBR_LABEL_ID_TAIL == p_label->id))
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                ret = CTC_E_NO_MEMORY;
                goto ERR;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[lchip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("chip = %d, global pbr label index = %d\n", lchip, p_label->p_index[lchip]->index);
        }
    }

    if(NULL == ctc_hash_insert(p_aclqos_label_hash, p_label))
    {
        if((CTC_GLOBAL_ACL_PBR_LABEL_ID_HEAD == p_label->id)
            || (CTC_GLOBAL_ACL_PBR_LABEL_ID_TAIL == p_label->id))
        {
            ret = CTC_E_UNEXPECT;
            goto ERR;
        }

        mem_free(p_label);
        return CTC_E_UNEXPECT;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("create   pbr acl label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("===========================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("label_id = %d\n", label_id);

    return CTC_E_NONE;

ERR:
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_label->p_index[lchip])
        {
            mem_free(p_label->p_index[lchip]);
        }
    }
    mem_free(p_label);

    return ret;
}

/**
 @brief delete acl pbr label
*/
int32
sys_humber_acl_pbr_label_delete(uint32 label_id)
{
    sys_aclqos_label_t *p_label;

    SYS_ACLQOS_DEL_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if label_id is exist */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    CTC_EQUAL_CHECK(p_label->ref, 0);

    /* release the label*/
    ctc_hash_remove(p_aclqos_label_hash, p_label);
    mem_free(p_label);

    SYS_ACLQOS_LABEL_DBG_INFO("remove pbr label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("label id = %d\n", label_id);

    return CTC_E_NONE;
}

/**
 @brief set acl pbr label for the given l3 interface
*/
int32
sys_humber_acl_pbr_label_apply(uint16 l3if_id, uint32 label_id)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    int32 ret = 0;
    sys_aclqos_label_t *p_label;
    sys_humber_opf_t opf;
    sys_aclqos_label_index_t *p_index;
    ctc_aclqos_key_type_t key_type;
    uint32 offset;

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);
    SYS_ACLQOS_APPLY_LABEL_CHECK(label_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* check if a label is allocated to the l3 interface */
    CTC_ERROR_RETURN(_sys_humber_acl_get_pbr_label(l3if_id, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_HAVE_ENABLED;
    }

    /* get acl pbr label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if (!IS_PBR_LABEL(p_label->type))
    {
        return CTC_E_ACLQOS_DIFFERENT_TYPE;
    }

    CTC_NOT_EQUAL_CHECK(p_label->ref, MAX_ACLQOS_LABEL_REF_NUM);

    /* For first apply, should allocate physical label index. */
    if(0 == p_label->ref)
    {
        /* For pbr, use the same physical label for all chips. */
        opf.pool_type = OPF_ACL_PBR_LABEL;
        opf.pool_index = 0;
        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &offset));

        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                sys_humber_opf_free_offset(&opf, 1, offset);
                return CTC_E_NO_MEMORY;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));
            p_index->index = offset;
            p_index->index_mask = 0xFF;

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[lchip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("chip = %d, label index = %d\n", lchip, p_label->p_index[lchip]->index);
        }

    }

    if(FALSE == ctc_vector_add(p_acl_pbr_label, l3if_id, p_label))
    {
        if(0 == p_label->ref)   /*first apply*/
        {
            ret = CTC_E_UNEXPECT;
            goto ERR;
        }
        return CTC_E_UNEXPECT;
    }

    p_label->ref ++;

    ret = _sys_humber_acl_set_pbr_label(l3if_id, p_label->p_index[0]->index);
    if(CTC_E_NONE != ret)
    {
        p_label->ref --;
        ctc_vector_del(p_acl_pbr_label, l3if_id);

        if(0 == p_label->ref)   /*first apply*/
        {
            goto ERR;
        }
        return ret;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("install pbr label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("l3if_id = %d, label_id = %d\n", l3if_id, label_id);
    SYS_ACLQOS_LABEL_DBG_INFO("ref = %d\n", p_label->ref);

    return CTC_E_NONE;

ERR:    /*process first apply rollback*/
    sys_humber_opf_free_offset(&opf, 1, p_label->p_index[0]->index);
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_label->p_index[lchip])
        {
            mem_free(p_label->p_index[lchip]);
        }
    }

    return ret;
}

/**
 @brief unset acl pbr label for the given l3 interface
*/
int32
sys_humber_acl_pbr_label_unapply(uint16 l3if_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0, lchip_num = 0;
    ctc_aclqos_key_type_t key_type;
    sys_humber_opf_t opf;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);

    /* get vlan acl/qos label */
    CTC_ERROR_RETURN(_sys_humber_acl_get_pbr_label(l3if_id, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    CTC_NOT_EQUAL_CHECK(p_label->ref, 0);
    p_label->ref --;

    /* For last unapply, should free physical label index. */
    if(0 == p_label->ref)
    {
        lchip_num = sys_humber_get_local_chip_num();
        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            if (!p_label->p_index[lchip])
            {
                continue;
            }

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                if (!CTC_LIST_POINTER_ISEMPTY(&(p_label->p_index[lchip]->entry_list[key_type])))
                {
                    p_label->ref ++;
                    return CTC_E_ACLQOS_LABEL_IN_USE;
                }
            }
        }

        opf.pool_type = OPF_ACL_PBR_LABEL;
        opf.pool_index = 0;
        sys_humber_opf_free_offset(&opf, 1, p_label->p_index[0]->index);

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            mem_free(p_label->p_index[lchip]);
        }
    }

    /* write to asic */
    CTC_ERROR_RETURN(_sys_humber_acl_unset_pbr_label(l3if_id));

    /* remove acl pbr label from db */
    ctc_vector_del(p_acl_pbr_label, l3if_id);

    SYS_ACLQOS_LABEL_DBG_INFO("unstall acl pbr label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("l3if_id = %d\n", l3if_id);

    return CTC_E_NONE;
}

int32
sys_humber_acl_pbr_label_id_get(uint16 l3if_id, uint32* label_id)
{
    sys_aclqos_label_t *p_label;

    SYS_ACLQOS_LABEL_DBG_FUNC();

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);
    CTC_PTR_VALID_CHECK(label_id);

    /* get vlan acl/qos label */
    CTC_ERROR_RETURN(_sys_humber_acl_get_pbr_label(l3if_id, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_NOT_ENABLED;
    }

    /* get label id */
    *label_id = p_label->id;

    SYS_ACLQOS_LABEL_DBG_INFO("get acl pbr label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("l3if_id = %d, label id = %d\n", l3if_id, *label_id);

    return CTC_E_NONE;
}

int32
sys_humber_aclqos_service_label_new(uint16 service_id)
{
    sys_aclqos_label_t *p_label;
    sys_service_config_t *p_service;
    sys_service_config_t service;
    sys_aclqos_label_index_t *p_index;
    uint8 lchip, lchip_num;
    ctc_aclqos_key_type_t key_type;
    int32 ret = 0;

    CTC_NOT_ZERO_CHECK(service_id);

    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* we do not lookup service to avoid creating service firstly, 
       we just use logical service id as physical service id */
    kal_memset(&service, 0, sizeof(service));
    service.logical_service_id  = service_id;
    service.physical_service_id = service_id;
    p_service = &service;
    /* check if label_id is already created */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(service_id, 1, &p_label));
    if (p_label)
    {
        return CTC_E_ACLQOS_LABEL_IN_USE;
    }

    /* create label */
    p_label = (sys_aclqos_label_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_t));
    if (!p_label)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_label, 0, sizeof(sys_aclqos_label_t));
    p_label->id = service_id;
    p_label->type = SYS_SERVICE_ACLQOS_LABEL;
    p_label->dir = 0xff;
    p_label->ref = 0;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if(!p_label->p_index[lchip])
        {
            p_index = (sys_aclqos_label_index_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_label_index_t));
            if (!p_index)
            {
                ret = CTC_E_NO_MEMORY;
                goto ERR;
            }

            kal_memset(p_index, 0, sizeof(sys_aclqos_label_index_t));
            p_index->index = p_service->physical_service_id;
            p_index->index_mask = 0xFFFF;

            for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
            {
                ctc_list_pointer_init(&p_index->entry_list[key_type]);
            }

            p_label->p_index[lchip] = p_index;

            SYS_ACLQOS_LABEL_DBG_INFO("index = %d\n", p_label->p_index[lchip]->index);
        }
    }

    if(NULL == ctc_hash_insert(p_service_label_hash, p_label))
    {
        ret = CTC_E_UNEXPECT;
        goto ERR;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("create service label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("===========================================\n");
    SYS_ACLQOS_LABEL_DBG_INFO("local service id = %d, physical service id = %d\n", service_id,p_service->physical_service_id);

    return CTC_E_NONE;

ERR:
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (p_label->p_index[lchip])
        {
            mem_free(p_label->p_index[lchip]);
        }
    }
    mem_free(p_label);
    return ret;
}

int32
sys_humber_aclqos_service_label_delete(uint16 service_id)
{
    sys_aclqos_label_t *p_label;
    sys_service_config_t *p_service;
    sys_service_config_t service;
    uint8 lchip, lchip_num;
    ctc_aclqos_key_type_t key_type = 0;


    SYS_ACLQOS_LABEL_DBG_FUNC();

    /* we do not lookup service to avoid creating service firstly, 
       we just use logical service id as physical service id */
    kal_memset(&service, 0, sizeof(service));
    service.logical_service_id  = service_id;
    service.physical_service_id = service_id;
    p_service = &service;

    /* check if label_id is exist */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(service_id, 1, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        /* check label usage status */
        for (key_type = CTC_ACLQOS_MAC_KEY; key_type < MAX_CTC_ACLQOS_KEY; key_type++)
        {
            if (!CTC_LIST_POINTER_ISEMPTY(&(p_label->p_index[lchip]->entry_list[key_type])))
            {
                return CTC_E_ACLQOS_LABEL_IN_USE;
            }
        }

        mem_free(p_label->p_index[lchip]);
    }

    ctc_hash_remove(p_service_label_hash, p_label);
    mem_free(p_label);

    SYS_ACLQOS_LABEL_DBG_INFO("remove service acl/qos label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("=================================");
    SYS_ACLQOS_LABEL_DBG_INFO("local service id = %d, physical service id = %d\n", service_id,p_service->physical_service_id);

    return CTC_E_NONE;
}


/**
 @brief show allocated acl/qos label for the given port
*/
int32
sys_humber_show_aclqos_label_by_port(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type)
{
    sys_aclqos_label_t *p_label;
    uint8 lport = 0;
    uint8 lchip = 0;
    char *type_str;

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);


    p_label = ctc_vector_get(p_aclqos_port_label[lchip][dir][type], lport);
    if (p_label)
    {
        if (IS_ACL_LABEL(p_label->type))
        {
            type_str = "acl label";
        }
        else
        {
            type_str = "qos label";
        }

        SYS_ACLQOS_LABEL_DBG_INFO("local chip    local port    dir    label id    type            label\n");
        SYS_ACLQOS_LABEL_DBG_INFO("==========    ==========    ===    ========    ============    =====\n");
        SYS_ACLQOS_LABEL_DBG_INFO("%10d    %10d    %3d    %8u    %12s    %5d\n",
                   lchip, lport, dir, p_label->id, type_str, p_label->p_index[lchip]->index);
    }

    return CTC_E_NONE;
}

/**
 @brief show allocated acl/qos label for the given vlan
*/
int32
sys_humber_show_aclqos_label_by_vlan(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    char *type_str;

    CTC_VLAN_RANGE_CHECK(vid);

    lchip_num = sys_humber_get_local_chip_num();

    p_label = ctc_vector_get(p_aclqos_vlan_label[dir][type], vid);
    if (p_label)
    {
        if (IS_ACL_LABEL(p_label->type))
        {
            type_str = "acl label";
        }
        else
        {
            type_str = "qos label";
        }

        SYS_ACLQOS_LABEL_DBG_INFO("local chip    vlan id    dir    label id    type            label\n");
        SYS_ACLQOS_LABEL_DBG_INFO("==========    =======    ===    ========    ============    =====\n");

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            if (!p_label->p_index[lchip])
                continue;

            SYS_ACLQOS_LABEL_DBG_INFO("%10d    %7d    %3d    %8u    %12s    %5d\n",
                lchip, vid, dir, p_label->id, type_str, p_label->p_index[lchip]->index);
        }
    }

    return CTC_E_NONE;
}

/**
 @brief show allocated acl/qos label for the given vlan
*/
int32
sys_humber_show_aclqos_label_by_service(uint16 service_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lchip_num = 0;

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(service_id, 1, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    SYS_ACLQOS_LABEL_DBG_INFO("local chip    service id      type            label\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==========    ==========    ============    =====\n");

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
            continue;

        SYS_ACLQOS_LABEL_DBG_INFO("%d        %8u            service label %5d\n",
            lchip, p_label->id, p_label->p_index[lchip]->index);
    }

    return CTC_E_NONE;
}


/**
 @brief show allocated acl/qos label for the given l3 interface
*/
int32
sys_humber_show_acl_pbr_label_by_l3if(uint16 l3if_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    char *type_str;

    SYS_ACL_L3IFID_VAILD_CHECK(l3if_id);

    lchip_num = sys_humber_get_local_chip_num();

    p_label = ctc_vector_get(p_acl_pbr_label, l3if_id);
    if (p_label)
    {
        type_str = "acl pbr label";

        SYS_ACLQOS_LABEL_DBG_INFO("local chip    l3if id    label id    type            label\n");
        SYS_ACLQOS_LABEL_DBG_INFO("==========    =======    ========    ============    =====\n");

        for (lchip = 0; lchip < lchip_num; lchip++)
        {
            if (!p_label->p_index[lchip])
                continue;

            SYS_ACLQOS_LABEL_DBG_INFO("%10d    %7d    %8u    %12s    %5d\n",
                lchip, l3if_id, p_label->id, type_str, p_label->p_index[lchip]->index);
        }
    }

    return CTC_E_NONE;
}


/**
 @brief show acl/qos label by the given label id
*/
int32
sys_humber_aclqos_label_show_by_id(uint32 label_id)
{
    sys_aclqos_label_t *p_label;
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    char *type_str;

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        SYS_ACLQOS_LABEL_DBG_INFO("%% Can not find label with id = %d\n", label_id);
        return CTC_E_NONE;
    }

    if (IS_ACL_LABEL(p_label->type))
    {
        type_str = "acl label";
    }
    else
    {
        type_str = "qos label";
    }

    SYS_ACLQOS_LABEL_DBG_INFO("local chip    label id    type            label    ref number\n");
    SYS_ACLQOS_LABEL_DBG_INFO("==========    ========    ============    =====    ==========\n");
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
            continue;

        SYS_ACLQOS_LABEL_DBG_INFO("%10d    %8u    %12s    %5d    %10d\n", lchip, p_label->id,
            type_str, p_label->p_index[lchip]->index, p_label->ref);
    }

    return CTC_E_NONE;
}


/**
 @brief acl/qos label component initialization
*/
int32
sys_humber_aclqos_label_init(void)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    ctc_direction_t dir;
    sys_humber_opf_t opf;

    p_aclqos_label_hash = ctc_hash_create(1,SYS_ACLQOS_LABEL_BUCKET_SIZE,
                          _sys_humber_aclqos_label_hash_key,
                          _sys_humber_aclqos_label_hash_cmp);

    p_service_label_hash = ctc_hash_create(1,SYS_ACLQOS_LABEL_BUCKET_SIZE,
                          _sys_humber_aclqos_label_hash_key,
                          _sys_humber_aclqos_label_hash_cmp);

    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_ACL_INGRESS_LABEL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_ACL_EGRESS_LABEL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_ACL_PBR_LABEL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QOS_INGRESS_LABEL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_QOS_EGRESS_LABEL, CTC_MAX_LOCAL_CHIP_NUM));

    /* init acl/qos label for each local chip, direction */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        /* acl label opf for each local chip */
        opf.pool_type  = OPF_ACL_INGRESS_LABEL;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MAX_ACLQOS_INGRESS_LABEL_START, MAX_ACLQOS_INGRESS_LABEL_NUM));

        opf.pool_type  = OPF_ACL_EGRESS_LABEL;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MAX_ACLQOS_EGRESS_LABEL_START, MAX_ACLQOS_EGRESS_LABEL_NUM));

        /* acl/qos label referenced db by port and vlan */
        for (dir = CTC_INGRESS; dir < CTC_BOTH_DIRECTION; dir++)
        {
            p_aclqos_port_label[lchip][dir][CTC_ACL_LABEL] = ctc_vector_init(8, MAX_PORT_NUM_PER_CHIP/8);
            if (0 == lchip)
            {
                p_aclqos_vlan_label[dir][CTC_ACL_LABEL] = ctc_vector_init(32, 4096/32);
            }
        }

        /* acl label referenced db by pbr */
        opf.pool_type  = OPF_ACL_PBR_LABEL;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, MAX_ACL_PBR_LABEL_NUM));
        if (0 == lchip)
        {
            p_acl_pbr_label = ctc_vector_init(32, (MAX_CTC_L3IF_ID+31)/32);
        }

        /* qos label opf for each local chip */
        opf.pool_type  = OPF_QOS_INGRESS_LABEL;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MAX_ACLQOS_INGRESS_LABEL_START, MAX_ACLQOS_INGRESS_LABEL_NUM));

        opf.pool_type  = OPF_QOS_EGRESS_LABEL;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, MAX_ACLQOS_EGRESS_LABEL_START, MAX_ACLQOS_EGRESS_LABEL_NUM));

        /* acl/qos label referenced db by port and vlan */
        for (dir = CTC_INGRESS; dir < CTC_BOTH_DIRECTION; dir++)
        {
            p_aclqos_port_label[lchip][dir][CTC_QOS_LABEL] = ctc_vector_init(8, MAX_PORT_NUM_PER_CHIP/8);
            if (0 == lchip)
            {
                p_aclqos_vlan_label[dir][CTC_QOS_LABEL] = ctc_vector_init(32, 4096/32);
            }
        }
    }

    /*init global aclqos label id*/
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_ACL_LABEL, CTC_GLOBAL_ACL_INGRESS_LABEL_ID_HEAD));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_ACL_LABEL, CTC_GLOBAL_ACL_INGRESS_LABEL_ID_TAIL));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_ACL_LABEL, CTC_GLOBAL_ACL_EGRESS_LABEL_ID_HEAD));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_ACL_LABEL, CTC_GLOBAL_ACL_EGRESS_LABEL_ID_TAIL));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_QOS_LABEL, CTC_GLOBAL_QOS_INGRESS_LABEL_ID_HEAD));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_QOS_LABEL, CTC_GLOBAL_QOS_INGRESS_LABEL_ID_TAIL));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_QOS_LABEL, CTC_GLOBAL_QOS_EGRESS_LABEL_ID_HEAD));
    CTC_ERROR_RETURN(sys_humber_aclqos_port_label_new(CTC_QOS_LABEL, CTC_GLOBAL_QOS_EGRESS_LABEL_ID_TAIL));
    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_new(CTC_GLOBAL_ACL_PBR_LABEL_ID_HEAD));
    CTC_ERROR_RETURN(sys_humber_acl_pbr_label_new(CTC_GLOBAL_ACL_PBR_LABEL_ID_TAIL));

    return CTC_E_NONE;
}


