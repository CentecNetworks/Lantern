/**
 @file sys_humber_vlan_classification.h

 @date 2009-12-15

 @version v2.0

 The file contains all apis of sys Vlan Classification
*/

#ifndef _SYS_HUMBER_VLAN_CLASSIFICATION_H
#define _SYS_HUMBER_VLAN_CLASSIFICATION_H

#include "kal.h"
#include "ctc_vlan.h"
#include "ctc_vector.h"
/***********************************************
*
*   Macros and Defines
*
***********************************************/

/***********************************************
*
*   Functions
*
***********************************************/
extern int32
sys_humber_vlan_classification_init();

extern int32
sys_humber_vlan_add_vlan_classification_entry(ctc_vlan_class_t *vlan_class_info);

extern int32
sys_humber_vlan_remove_vlan_classification_entry(ctc_vlan_class_t *vlan_class_info);

extern int32
sys_humber_vlan_flush_vlan_classification_entry(ctc_vlan_class_type_t type);


extern int32
sys_humber_vlan_add_vlan_classification_default_entry(ctc_vlan_class_type_t type, ctc_vlan_miss_t *p_action);

extern int32
sys_humber_vlan_remove_vlan_classification_default_entry(ctc_vlan_class_type_t type);



#endif

