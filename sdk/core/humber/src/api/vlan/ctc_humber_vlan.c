/**
 @file ctc_humber_vlan.c

 @date 2009-10-17

 @version v2.0

 The file contains all vlan APIs
*/

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_error.h"
#include "ctc_debug.h"
#include "ctc_humber_vlan.h"
#include "sys_humber_vlan.h"
#include "sys_humber_vlan_classification.h"
#include "sys_humber_vlan_mapping.h"
#include "sys_humber_nexthop_api.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/

/**
 @brief init the vlan module
*/
int32
ctc_humber_vlan_init(ctc_vlan_global_cfg_t* vlan_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_vlan_init());
    CTC_ERROR_RETURN(sys_humber_vlan_classification_init());
    CTC_ERROR_RETURN(sys_humber_vlan_mapping_init());

    return CTC_E_NONE;
}

/**
 @brief The function is to create a vlan
*/
int32
ctc_humber_vlan_create_vlan(uint16 vlan_id)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments info vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_create_vlan(&vlan_info));

    return CTC_E_NONE;
}

/**
 @brief The function is to remove the vlan
*/
int32
ctc_humber_vlan_remove_vlan(uint16 vlan_id)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;


    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan(&vlan_info));

    return CTC_E_NONE;
}

/**
 @brief The function is to add member port to a vlan
*/
int32
ctc_humber_vlan_add_port(uint16 vlan_id, uint16 gport)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_add_port(&vlan_info, gport));

    return CTC_E_NONE;
}

/**
 @brief The function is to show vlan's member port
*/
int32
ctc_humber_vlan_get_ports(uint16 vlan_id, ctc_port_bitmap_t* port_bitmap)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_ports(&vlan_info, port_bitmap));

    return CTC_E_NONE;
}

/**
 @brief The function is to remove member port to a vlan
*/
int32
ctc_humber_vlan_remove_port(uint16 vlan_id, uint16 gport)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_remove_port(&vlan_info, gport));

    return CTC_E_NONE;
}

/**
 @brief The function is to set receive enable on vlan
*/
int32
ctc_humber_vlan_set_receive_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d, receive enable:%d!\n", vlan_id, enable);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_receive_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to get receive on vlan
*/
int32
ctc_humber_vlan_get_receive_en(uint16 vlan_id, bool* enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d!\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_receive_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to set tranmit enable on vlan
*/
int32
ctc_humber_vlan_set_transmit_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d, transmit enable:%d!\n", vlan_id, enable);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_transmit_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to get tranmit on vlan
*/
int32
ctc_humber_vlan_get_transmit_en(uint16 vlan_id, bool* enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d!\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_transmit_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to set bridge enable on vlan
*/
int32
ctc_humber_vlan_set_bridge_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d, bridge enable:%d!\n", vlan_id, enable);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_bridge_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to get bridge on vlan
*/
int32
ctc_humber_vlan_get_bridge_en(uint16 vlan_id, bool* enable)
{
   sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d!\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_bridge_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The fucntion is to set fid of vlan
*/
int32
ctc_humber_vlan_set_fid(uint16 vlan_id, uint16 fid)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d, vrfid:%d\n", vlan_id, fid);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_fid(&vlan_info, fid));

    return CTC_E_NONE;
}

/**
 @brief The fucntion is to get vrfid of vlan
*/
int32
ctc_humber_vlan_get_fid(uint16 vlan_id, uint16* fid)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_fid(&vlan_info, fid));

    return CTC_E_NONE;
}

/**
 @brief The function is set mac learning enable/disable on the vlan
*/
int32
ctc_humber_vlan_set_learning_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_learning_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is get mac learning enable/disable on the vlan
*/
int32
ctc_humber_vlan_get_learning_en(uint16 vlan_id, bool* enable)
{
    sys_vlan_info_t vlan_info;

    CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_CTC, "Argments list vlan_id:%d\n", vlan_id);

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_learning_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to set igmp snooping enable on the vlan
*/
int32
ctc_humber_vlan_set_igmp_snoop_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_igmp_snoop_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to get igmp snooping enable of the vlan
*/
int32
ctc_humber_vlan_get_igmp_snoop_en(uint16 vlan_id, bool* enable)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_igmp_snoop_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to set dhcp exception action of the vlan
*/
int32
ctc_humber_vlan_set_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t type)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_dhcp_excp_type(&vlan_info, type));

    return CTC_E_NONE;
}

/**
 @brief The function is to get dhcp exception action of the vlan
*/
int32
ctc_humber_vlan_get_dhcp_excp_type(uint16 vlan_id, ctc_exception_type_t* type)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_dhcp_excp_type(&vlan_info, type));

    return CTC_E_NONE;
}

/**
 @brief The function is to set arp exception action of the vlan
*/
int32
ctc_humber_vlan_set_arp_excp_type(uint16 vlan_id, ctc_exception_type_t type)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_arp_excp_type(&vlan_info, type));

    return CTC_E_NONE;
}

/**
 @brief The function is to get arp exception action of the vlan
*/
int32
ctc_humber_vlan_get_arp_excp_type(uint16 vlan_id, ctc_exception_type_t* type)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_arp_excp_type(&vlan_info, type));

    return CTC_E_NONE;
}

/**
 @brief The function is to set replace packet's dscp by classify of vlan
*/
int32
ctc_humber_vlan_set_replace_dscp_en(uint16 vlan_id, bool enable)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_set_replace_dscp_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/**
 @brief The function is to set replace packet's dscp by classify of vlan
*/
int32
ctc_humber_vlan_get_replace_dspc_en(uint16 vlan_id, bool* enable)
{
    sys_vlan_info_t vlan_info;

    vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VID;
    vlan_info.vid           = vlan_id;

    CTC_ERROR_RETURN(sys_humber_vlan_get_replace_dscp_en(&vlan_info, enable));

    return CTC_E_NONE;
}

/****************Begin vlan classification*******************************/
/**
 @brief The function is to add one vlan classification rule
*/
int32
ctc_humber_vlan_add_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class)
{
    CTC_ERROR_RETURN(sys_humber_vlan_add_vlan_classification_entry(p_vlan_class));
    return CTC_E_NONE;
}

/**
 @brief The fucntion is to remove on vlan classification rule
*/
int32
ctc_humber_vlan_remove_vlan_classification_entry(ctc_vlan_class_t* p_vlan_class)
{
    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan_classification_entry(p_vlan_class));
    return CTC_E_NONE;
}




/**
 @brief The fucntion is to flush vlan classification by type
*/
int32
ctc_humber_vlan_flush_vlan_classification_entry(ctc_vlan_class_type_t type)
{
    CTC_ERROR_RETURN(sys_humber_vlan_flush_vlan_classification_entry(type));
    return CTC_E_NONE;
}


/**
 @brief Add vlan classification default entry per label
*/
int32
ctc_humber_vlan_add_vlan_classification_default_entry(ctc_vlan_class_type_t type, ctc_vlan_miss_t* p_action)
{
    CTC_ERROR_RETURN(sys_humber_vlan_add_vlan_classification_default_entry(type, p_action));
    return CTC_E_NONE;
}
/**
 @brief Remove vlan classification default entry per label
*/
int32
ctc_humber_vlan_remove_vlan_classification_default_entry(ctc_vlan_class_type_t type)
{
    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan_classification_default_entry(type));
    return CTC_E_NONE;
}

/**
 @brief The function is to add one vlan mapping entry on the port in IPE of Humber
*/
int32
ctc_humber_vlan_add_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping)
{
    CTC_ERROR_RETURN(sys_humber_vlan_add_vlan_mapping_entry(gport, p_vlan_mapping));
    return CTC_E_NONE;
}

/**
 @brief The function is to remove one vlan mapping entry on the port in IPE of Humber
*/
int32
ctc_humber_vlan_remove_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t* p_vlan_mapping)
{
    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan_mapping_entry(gport, p_vlan_mapping));
    return CTC_E_NONE;
}

/**
 @brief The function is to add vlan mapping default entry on the port
*/
int32
ctc_humber_vlan_add_vlan_mapping_default_entry(uint16 gport, ctc_vlan_miss_t* p_action)
{
    CTC_ERROR_RETURN(sys_humber_vlan_add_vlan_mapping_default_entry(gport, p_action));
    return CTC_E_NONE;
}

/**
 @brief The function is to remove vlan mapping miss match default entry of port
*/
int32
ctc_humber_vlan_remove_vlan_mapping_default_entry(uint16 gport)
{
    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan_mapping_default_entry(gport));
    return CTC_E_NONE;
}

/**
 @brief The function is to remove all vlan mapping entries on port
*/
int32
ctc_humber_vlan_remove_vlan_mapping_all_by_port(uint16 gport)
{
    CTC_ERROR_RETURN(sys_humber_vlan_remove_vlan_mapping_all_by_port(gport));
    return CTC_E_NONE;
}


