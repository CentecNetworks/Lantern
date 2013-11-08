/**
 @file sys_humber_aclqos_label.h

 @date 2009-11-30

 @version v2.0

 The file defines macro, data structure, and function for acl/qos label component
*/

#ifndef _SYS_HUMBER_ACLQOS_LABEL_H_
#define _SYS_HUMBER_ACLQOS_LABEL_H_

#include "ctc_debug.h"

#include "sys_humber_aclqos_entry.h"

/*********************************************************************
 *
 * macros and variable declaration
 *
 *********************************************************************/
#define SYS_ACLQOS_LABEL_DBG_INFO(FMT, ...) \
    { \
        CTC_DEBUG_OUT_INFO(aclqos, label, ACLQOS_LABEL_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_ACLQOS_LABEL_DBG_FUNC() \
    { \
        CTC_DEBUG_OUT_FUNC(aclqos, label, ACLQOS_LABEL_SYS); \
    }

#define SYS_ACLQOS_DEL_LABEL_CHECK(label) \
    { \
        if (label >= CTC_GLOBAL_ACL_INGRESS_LABEL_ID_HEAD) \
        { \
            return CTC_E_INVALID_PARAM; \
        } \
    }

#define SYS_ACLQOS_APPLY_LABEL_CHECK(label) \
    { \
        if (label >= CTC_GLOBAL_ACL_INGRESS_LABEL_ID_HEAD) \
        { \
            return CTC_E_INVALID_PARAM; \
        } \
    }

#define SYS_ACL_L3IFID_VAILD_CHECK(l3if_id)\
      if (l3if_id > MAX_CTC_L3IF_ID )\
      {\
          return CTC_E_L3IF_INVALID_IF_ID;\
      }


#define IS_PORT_LABEL(type) ((type == SYS_PORT_ACL_LABEL) || (type == SYS_PORT_QOS_LABEL))
#define IS_VLAN_LABEL(type) ((type == SYS_VLAN_ACL_LABEL) || (type == SYS_VLAN_QOS_LABEL))
#define IS_PBR_LABEL(type)  (type == SYS_PBR_ACL_LABEL)
#define IS_ACL_LABEL(type)  ((type == SYS_PORT_ACL_LABEL) || (type == SYS_VLAN_ACL_LABEL) \
                            || (type == SYS_PBR_ACL_LABEL))
#define IS_QOS_LABEL(type)  ((type == SYS_PORT_QOS_LABEL) || (type == SYS_VLAN_QOS_LABEL))


/*********************************************************************
 *
 * data structure
 *
 *********************************************************************/

enum sys_aclqos_label_type_e
{
    SYS_PORT_ACL_LABEL,
    SYS_VLAN_ACL_LABEL,
    SYS_PBR_ACL_LABEL,
    SYS_PORT_QOS_LABEL,
    SYS_VLAN_QOS_LABEL,
    SYS_SERVICE_ACLQOS_LABEL,

    MAX_SYS_ACLQOS_LABEL
};
typedef enum sys_aclqos_label_type_e sys_aclqos_label_type_t;


struct sys_aclqos_label_index_s
{
    uint16  index;      /**< either be acl/qos label, or be service id if service acl/qos enabled */
    uint16  index_mask;

    ctc_list_pointer_t entry_list[MAX_CTC_ACLQOS_KEY];  /**< sys_aclqos_entry_t */
};
typedef struct sys_aclqos_label_index_s sys_aclqos_label_index_t;

/**
 @brief  acl/qos label
*/
struct sys_aclqos_label_s
{
    uint32 id;

    sys_aclqos_label_index_t *p_index[CTC_MAX_LOCAL_CHIP_NUM];

    uint8  type;           /**< SYS_XXX_LABEL */
    uint8  dir;
    uint16 ref;        /**< a label might be shared by multiple interfaces */
};
typedef struct sys_aclqos_label_s sys_aclqos_label_t;


/*********************************************************************
 *
 * function declaration
 *
 *********************************************************************/

/**
 @brief Lookup acl/qos label according to the given label id
*/
extern int32
sys_humber_aclqos_label_lookup(uint32 label_id, uint8 is_service_label, sys_aclqos_label_t** pp_label);


/**
 @brief Lookup acl/qos enable/disable on the given port
*/
extern int32
sys_humber_aclqos_port_lkp_enable(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable);


/**
 @brief create acl/qos port label
*/
extern int32
sys_humber_aclqos_port_label_new(ctc_aclqos_label_type_t type, uint32 label_id);


/**
 @brief delete acl/qos port label
*/
extern int32
sys_humber_aclqos_port_label_delete(ctc_aclqos_label_type_t type, uint32 label_id);


/**
 @brief set acl/qos label for the given port
*/
extern int32
sys_humber_aclqos_port_label_apply(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32 label_id);

/**
 @brief unset acl/qos label for the given port
*/
extern int32
sys_humber_aclqos_port_label_unapply(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type);

extern int32
sys_humber_aclqos_port_label_id_get(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32* label_id);


/**
 @brief Lookup acl/qos enable/disable on the given vlan
*/
extern int32
sys_humber_aclqos_vlan_lkp_enable(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t label_type, bool enable);

/**
 @brief craete acl/qos vlan label
*/
extern int32
sys_humber_aclqos_vlan_label_new(ctc_aclqos_label_type_t type, uint32 label_id);


/**
 @brief delete acl/qos vlan label
*/
extern int32
sys_humber_aclqos_vlan_label_delete(ctc_aclqos_label_type_t type, uint32 label_id);


/**
 @brief set acl/qos label for the given vlan
*/
extern int32
sys_humber_aclqos_vlan_label_apply(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32 label_id);

/**
 @brief unset acl/qos label for the given vlan
*/
extern int32
sys_humber_aclqos_vlan_label_unapply(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type);

extern int32
sys_humber_aclqos_vlan_label_id_get(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t type, uint32* label_id);


/**
 @brief Lookup acl pbr enable/disable on the given l3 interface
*/
extern int32
sys_humber_acl_pbr_lkp_enable(uint16 l3if_id, bool enable);

/**
 @brief create acl pbr label
*/
extern int32
sys_humber_acl_pbr_label_new(uint32 label_id);

/**
 @brief delete acl pbr label
*/
extern int32
sys_humber_acl_pbr_label_delete(uint32 label_id);

/**
 @brief set acl pbr label for the given l3 interface
*/
extern int32
sys_humber_acl_pbr_label_apply(uint16 l3if_id, uint32 label_id);

/**
 @brief unset acl pbr label for the given l3 interface
*/
extern int32
sys_humber_acl_pbr_label_unapply(uint16 l3if_id);

extern int32
sys_humber_acl_pbr_label_id_get(uint16 l3if_id, uint32* label_id);


extern int32
sys_humber_aclqos_service_label_new(uint16 service_id);

extern int32
sys_humber_aclqos_service_label_delete(uint16 service_id);

/**
 @brief show allocated acl/qos label for the given port
*/
extern int32
sys_humber_show_aclqos_label_by_port(uint16 gport, ctc_direction_t dir, ctc_aclqos_label_type_t ctc_type);


/**
 @brief show allocated acl/qos label for the given vlan
*/
extern int32
sys_humber_show_aclqos_label_by_vlan(uint16 vid, ctc_direction_t dir, ctc_aclqos_label_type_t ctc_type);

extern int32
sys_humber_show_aclqos_label_by_service(uint16 service_id);

/**
 @brief show acl/qos label by the given label id
*/
extern int32
sys_humber_aclqos_label_show_by_id(uint32 label_id);

/**
 @brief show allocated acl/qos label for the given l3 interface
*/
int32
sys_humber_show_acl_pbr_label_by_l3if(uint16 l3if_id);

/**
 @brief acl/qos label component initialization
*/
extern int32
sys_humber_aclqos_label_init(void);


#endif

