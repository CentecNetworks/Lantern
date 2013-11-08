/****************************************************************************
 *file ctc_debug.c

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

  This file contains  debug header file.
 ****************************************************************************/

#include "kal.h"
#include "ctc_debug.h"
#include "ctc_error.h"
#include "ctc_linklist.h"

#define CTC_DEBUG_MODULE_REG(mod, sub)     \
ctc_listnode_add(ctc_debug_list,&ctc_##mod##_##sub##_debug_handle);


static struct ctc_linklist  *ctc_debug_list;
uint8    g_ctc_debug_on = 1;
int32 ctc_debug_module_reg();


/**************************************************************************
 *
 * Defines and Macros
 *
 **************************************************************************/
CTC_DEBUG_IMPLEMENT(qos, process);
CTC_DEBUG_IMPLEMENT(qos, queue);
CTC_DEBUG_IMPLEMENT(acl, acl);
CTC_DEBUG_IMPLEMENT(cpu, traffic);
CTC_DEBUG_IMPLEMENT(aclqos, label);
CTC_DEBUG_IMPLEMENT(aclqos, entry);
CTC_DEBUG_IMPLEMENT(qos, policer);
CTC_DEBUG_IMPLEMENT(qos, class);

CTC_DEBUG_IMPLEMENT(l3if, l3if);
CTC_DEBUG_IMPLEMENT(ipuc, ipuc);
CTC_DEBUG_IMPLEMENT(ipmc, ipmc);
CTC_DEBUG_IMPLEMENT(mcast, mcast);
CTC_DEBUG_IMPLEMENT(mpls, mpls);

CTC_DEBUG_IMPLEMENT(l2, fdb);
CTC_DEBUG_IMPLEMENT(l2, learning_aging);
CTC_DEBUG_IMPLEMENT(l2, stp);
CTC_DEBUG_IMPLEMENT(vlan, vlan_class);
CTC_DEBUG_IMPLEMENT(vlan, vlan_mapping);
CTC_DEBUG_IMPLEMENT(vlan, vlan_switching);
CTC_DEBUG_IMPLEMENT(vlan, protocol_vlan);
CTC_DEBUG_IMPLEMENT(vlan, vlan);
CTC_DEBUG_IMPLEMENT(mirror, mirror);
CTC_DEBUG_IMPLEMENT(dot1x, dot1x);
CTC_DEBUG_IMPLEMENT(security, security);
CTC_DEBUG_IMPLEMENT(oam, cfm);
CTC_DEBUG_IMPLEMENT(oam,efm);
CTC_DEBUG_IMPLEMENT(oam, pbx);
CTC_DEBUG_IMPLEMENT(ptp, ptp);

CTC_DEBUG_IMPLEMENT(interrupt, interrupt);

CTC_DEBUG_IMPLEMENT(chip, chip);
CTC_DEBUG_IMPLEMENT(port, port);
CTC_DEBUG_IMPLEMENT(linkagg, linkagg);
CTC_DEBUG_IMPLEMENT(nexthop, nexthop);
CTC_DEBUG_IMPLEMENT(sacl, sacl);
CTC_DEBUG_IMPLEMENT(opf, opf);
CTC_DEBUG_IMPLEMENT(stats, stats);
CTC_DEBUG_IMPLEMENT(alloc, alloc);
CTC_DEBUG_IMPLEMENT(memmngr, memmngr);


/****************************************************************************
 *
* SDK module add it's debug info to list
*
* NOTE:
* 1, pattern: CTC_DEBUG_MODULE_REG(module, submodule);
* 2, module and submodule name length should less than 20 characters
*
****************************************************************************/
int32
ctc_debug_module_reg()
{

    CTC_DEBUG_MODULE_REG(qos, process);
    CTC_DEBUG_MODULE_REG(qos, queue);
    CTC_DEBUG_MODULE_REG(acl, acl);
    CTC_DEBUG_MODULE_REG(cpu, traffic);
    CTC_DEBUG_MODULE_REG(aclqos, label);
    CTC_DEBUG_MODULE_REG(aclqos, entry);
    CTC_DEBUG_MODULE_REG(qos, policer);
    CTC_DEBUG_MODULE_REG(qos, class);

    CTC_DEBUG_MODULE_REG(ipuc, ipuc);
    CTC_DEBUG_MODULE_REG(ipmc, ipmc);
    CTC_DEBUG_MODULE_REG(mcast, mcast);
    CTC_DEBUG_MODULE_REG(mpls, mpls);
    CTC_DEBUG_MODULE_REG(l3if, l3if);

    CTC_DEBUG_MODULE_REG(l2, fdb);
    CTC_DEBUG_MODULE_REG(l2, learning_aging);
    CTC_DEBUG_MODULE_REG(l2, stp);
    CTC_DEBUG_MODULE_REG(vlan, vlan_class);
    CTC_DEBUG_MODULE_REG(vlan, vlan_mapping);
    CTC_DEBUG_MODULE_REG(vlan, vlan_switching);
    CTC_DEBUG_MODULE_REG(vlan, protocol_vlan);
    CTC_DEBUG_MODULE_REG(vlan, vlan);
    CTC_DEBUG_MODULE_REG(mirror, mirror);
    CTC_DEBUG_MODULE_REG(dot1x, dot1x);
    CTC_DEBUG_MODULE_REG(security, security);
    CTC_DEBUG_MODULE_REG(oam, cfm);
    CTC_DEBUG_MODULE_REG(oam, efm);
    CTC_DEBUG_MODULE_REG(ptp, ptp);

    CTC_DEBUG_MODULE_REG(interrupt, interrupt);

    CTC_DEBUG_MODULE_REG(chip, chip);
    CTC_DEBUG_MODULE_REG(port, port);
    CTC_DEBUG_MODULE_REG(linkagg, linkagg);
    CTC_DEBUG_MODULE_REG(nexthop, nexthop);
    CTC_DEBUG_MODULE_REG(sacl, sacl);
    CTC_DEBUG_MODULE_REG(opf, opf);
    CTC_DEBUG_MODULE_REG(stats, stats);
    CTC_DEBUG_MODULE_REG(alloc, alloc);
    CTC_DEBUG_MODULE_REG(memmngr, memmngr);

    return CTC_E_NONE;
}

int32
ctc_debug_init()
{

	ctc_debug_list = ctc_list_new ();
    CTC_ERROR_RETURN(ctc_debug_module_reg());
    CTC_ERROR_RETURN (ctc_debug_enable(TRUE));

    return CTC_E_NONE;
}

int32
ctc_debug_cmd_handle(ctc_debug_list_t *info)
{
    ctc_debug_list_t *debug_info;
    struct ctc_listnode *node;

    CTC_LIST_LOOP (ctc_debug_list, debug_info, node)
    {
        if (0 == kal_strcmp(debug_info->module, info->module) &&
            0 == kal_strcmp(debug_info->submodule, info->submodule))
        {
            debug_info->flags = info->flags;
            break;
        }

    }
    return CTC_E_NONE;
}

int32
ctc_debug_cmd_clearall ()
{

 struct ctc_listnode *node;
    ctc_debug_list_t *debug_info;

    CTC_LIST_LOOP (ctc_debug_list, debug_info, node)
    {
        debug_info->flags = 0;
    }


    return CTC_E_NONE;
}

int32
ctc_debug_enable (bool enable)
{
    g_ctc_debug_on = (enable == TRUE)?1:0;

    return CTC_E_NONE;
}
bool
ctc_get_debug_enable ()
{
    return g_ctc_debug_on?TRUE:FALSE;
}


int32
ctc_debug_set_flag(char *module,char *submodule, uint32 typeenum,uint8 debug_level,bool debug_on)
{

    ctc_debug_list_t *debug_info;
    struct ctc_listnode *node;

    CTC_LIST_LOOP (ctc_debug_list, debug_info, node)
    {
        if (0 == kal_strcmp(debug_info->module, module) &&
            0 == kal_strcmp(debug_info->submodule,submodule))
        {
            if (debug_on == TRUE)
            {
                debug_info->flags |= (1 << (typeenum));
                debug_info->level = debug_level;
            }
            else
            {
                debug_info->flags &= ~(1 << (typeenum));
                debug_info->level  = 0;
            }
            break;
        }

    }
    return CTC_E_NONE;

}
bool
ctc_debug_get_flag(char *module,char *submodule, uint32 typeenum,uint8 *level)
{

    ctc_debug_list_t *debug_info;
    struct ctc_listnode *node;

    CTC_LIST_LOOP (ctc_debug_list, debug_info, node)
    {
        if (0 == kal_strcmp(debug_info->module, module) &&
            0 == kal_strcmp(debug_info->submodule,submodule))
        {
            *level = debug_info->level;
            return (debug_info->flags & (1 << (typeenum)))?TRUE:FALSE;
        }

    }
    return FALSE;

}

bool
ctc_debug_check_flag(ctc_debug_list_t flag_info, uint32 typeenum,uint8 level)
{
   if(0 == g_ctc_debug_on)
    {
       return FALSE;
    }

   if(level == CTC_DEBUG_LEVEL_DUMP)
        return TRUE;
    if (typeenum < (sizeof(uint32) * 8))
    {
        if ((flag_info.flags & (1 << (typeenum))) && (flag_info.level & level))
            return TRUE;
    }
    return FALSE;
}

