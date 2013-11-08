/**
 @file sys_humber_vlan_mapping.h

 @date 2009-12-11

 @version v2.0


*/

#ifndef _SYS_HUMBER_VLAN_MAPPING_H
#define _SYS_HUMBER_VLAN_MAPPING_H

#include "kal.h"
#include "ctc_vlan.h"
/**************************************************************
*
* Macro and Defines
*
***************************************************************/
#define SYS_VLAN_MAPPING_DEBUG_INFO(FMT, ...) \
            { \
                CTC_DEBUG_OUT_INFO(vlan, vlan_mapping, VLAN_MAPPING_SYS, FMT, ##__VA_ARGS__);\
            }

#define SYS_VLAN_MAPPING_DEBUG_FUNC() \
            {\
                CTC_DEBUG_OUT_FUNC(vlan, vlan_mapping, VLAN_MAPPING_SYS);\
            }

/**************************************************************
*
* Function
*
***************************************************************/
extern int32
sys_humber_vlan_mapping_init(void);

extern int32
sys_humber_vlan_add_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t *p_vlan_mapping);

extern int32
sys_humber_vlan_remove_vlan_mapping_entry(uint16 gport, ctc_vlan_mapping_t *p_vlan_mapping);

extern int32
sys_humber_vlan_add_vlan_mapping_default_entry(uint16 gport, ctc_vlan_miss_t *p_action);

extern int32
sys_humber_vlan_remove_vlan_mapping_default_entry(uint16 gport);

extern int32
sys_humber_vlan_remove_vlan_mapping_all_by_port(uint16 gport);

extern int32
sys_humber_vlan_add_flex_vlan_mapping_entry(uint8 label_id, ctc_flex_vlan_mapping_t *p_vlan_mapping);

extern int32
sys_humber_vlan_remove_flex_vlan_mapping_entry(uint8 label_id, ctc_flex_vlan_mapping_t *p_vlan_mapping);

extern int32
sys_humber_vlan_add_flex_vlan_mapping_default_entry(uint8 label_id, ctc_vlan_miss_t *p_action);

extern int32
sys_humber_vlan_remove_flex_vlan_mapping_default_entry(uint8 label_id);

#endif

