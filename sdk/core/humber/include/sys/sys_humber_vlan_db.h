/**
 @file sys_humber_vlan_db.h

 @date 2009-10-17

 @version v2.0

*/
#ifndef _SYS_HUMBER_VLAN_DB_H
#define _SYS_HUMBER_VLAN_DB_H

/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"
#include "ctc_vlan.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
struct sys_vlan_node_s
{
    uint8  *port_bitmap;
};
typedef struct sys_vlan_node_s sys_vlan_node_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_vlan_db_init(void);

extern int32
sys_humber_vlan_db_deinit(void);

extern int32
sys_humber_vlan_db_add_vlan(uint16 vlan_ptr);

extern int32
sys_humber_vlan_db_remove_vlan(uint16 vlan_ptr);

extern bool
sys_humber_vlan_db_lookup(uint16 vlan_ptr);

extern bool
sys_humber_vlan_db_has_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport);

extern int32
sys_humber_vlan_db_add_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport);

extern int32
sys_humber_vlan_db_get_ports(uint16 vlan_ptr, ctc_port_bitmap_t *port_bitmap);

extern int32
sys_humber_vlan_db_remove_port(uint8 chip_id, uint16 vlan_ptr, uint8 lport);

#endif

