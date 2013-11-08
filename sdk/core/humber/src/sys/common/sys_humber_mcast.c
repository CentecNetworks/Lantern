/**
 @file sys_humber_mcast.c

 @date 2010-1-20

 @version v2.0

 shared by both L2 and L3 multicast
*/
#include "kal.h"
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_linklist.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_chip.h"
#include "sys_humber_mcast.h"

 /*if SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY == 1,SDK automatically add remote entry.By default,the SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY should be 0*/
#define SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY  0


struct sys_mcast_add_analyze_member_s
{
     ctc_slist_t *p_local_member_list;
     sys_mc_member_info_t *p_target_member_info;
     uint8 is_member_existed;
     uint8 is_first_local_member_to_add;
};
typedef  struct sys_mcast_add_analyze_member_s sys_mcast_add_analyze_member_t;

struct sys_mcast_remove_analyze_member_s
{
    ctc_slist_t *p_local_member_list;
    ctc_slistnode_t *pre_or_next_node;
    ctc_slistnode_t *del_node;
    sys_mc_member_info_t *p_target_member_info;
    uint8 is_member_existed;
    uint8 is_last_local_member_to_remove;
};
typedef  struct sys_mcast_remove_analyze_member_s sys_mcast_remove_analyze_member_t;

static int32
 _sys_humber_mc_add_analyze_member_list(sys_mcast_add_analyze_member_t *p_param)
{
    uint16 target_global_port = 0;
    uint16 target_vlan_id = 0;
    uint32 ref_nhid = 0;
    uint8   aps_protection_path = 0;
    uint8   is_remote_mem = 0;
    ctc_slistnode_t *lisnode = NULL;
    ctc_slist_t *p_local_member_list = NULL;
    sys_local_member_node_t *p_local_member = NULL;

    CTC_PTR_VALID_CHECK(p_param);
    CTC_PTR_VALID_CHECK(p_param->p_local_member_list);

    p_local_member_list = p_param->p_local_member_list;

    /* initiate output value */
    p_param->is_member_existed              = FALSE;
    p_param->is_first_local_member_to_add   = FALSE;

    target_global_port  = p_param->p_target_member_info->dest_id;
    target_vlan_id      = p_param->p_target_member_info->vlan_id;
    ref_nhid            = p_param->p_target_member_info->ref_nhid;
    aps_protection_path = p_param->p_target_member_info->aps_protection_path;
    is_remote_mem       = p_param->p_target_member_info->remote_chip;

    if (CTC_SLISTCOUNT( p_local_member_list) == 0)
    {
        /* local member does NOT exist on target chip */
        p_param->is_member_existed              = FALSE;
        p_param->is_first_local_member_to_add   = TRUE;

        return CTC_E_NONE;
    }

    CTC_SLIST_LOOP(p_local_member_list, lisnode)
    {
       p_local_member = _ctc_container_of(lisnode, sys_local_member_node_t, head);
       if (!p_local_member  )
       {
            continue ;
       }
        if ((( (p_local_member->member_info.member_type == SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE)
                 && (aps_protection_path == p_local_member->member_info.aps_protection_path) )
               || (p_local_member->member_info.member_type != SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE) )
             && (ref_nhid == p_local_member->member_info.ref_nhid )
             && (target_global_port == p_local_member->member_info.dest_id)
             && (target_vlan_id == p_local_member->member_info.vlan_id)
             && (is_remote_mem == p_local_member->member_info.remote_chip))
        {
            /* once find the same member, stop search and return instantly*/
            p_param->is_member_existed = TRUE;
            return CTC_E_NONE;
        }

    }
    return  CTC_E_NONE;
}



static int32
_sys_humber_mc_add_local_member(sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id, uint8 target_chip, uint8 is_linkagg_member)
{
    uint8 local_chip_id = 0;
    uint8 local_chip_number = 0;
    sys_nh_param_mcast_member_t nexthop_member_info;
    sys_nh_param_mcast_group_t nh_mcast_group;

    CTC_PTR_VALID_CHECK(p_target_member_info);

    kal_memset(&nexthop_member_info, 0, sizeof(sys_nh_param_mcast_member_t));
    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));

    nexthop_member_info.member_type     = p_target_member_info->member_type;

    switch(p_target_member_info->member_type)
    {
    case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
    case SYS_NH_PARAM_BRGMC_MEM_RAPS:
        nexthop_member_info.destid = CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.port_check_discard = TRUE;
        break;
    case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
        nexthop_member_info.destid =  p_target_member_info->aps_group_id;
        nexthop_member_info.ref_nhid = p_target_member_info->ref_nhid;
        nexthop_member_info.is_protection_path = p_target_member_info->aps_protection_path;
        nexthop_member_info.port_check_discard = TRUE;
        break;
    case SYS_NH_PARAM_IPMC_MEM_LOCAL:
    case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
        nexthop_member_info.destid =  CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.vid    = p_target_member_info->vlan_id;
        nexthop_member_info.l3if_type = p_target_member_info->l3if_type;
        nexthop_member_info.port_check_discard = TRUE;
        break;
    case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
        nexthop_member_info.ref_nhid = p_target_member_info->ref_nhid;
        nexthop_member_info.destid = CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.port_check_discard = TRUE;
        break;
    case SYS_NH_PARAM_MCAST_MEM_REMOTE:
         if(SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY)
          {
             return CTC_E_INVALID_PARAM;
          }
        nexthop_member_info.destid = CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.ref_nhid = p_target_member_info->ref_nhid;
        break;
    default:
          return CTC_E_INVALID_PARAM;
        break;

    }


    nh_mcast_group.nhid = nexthop_id;
    if (p_target_member_info->is_upmep ||
        SYS_NH_PARAM_BRGMC_MEM_RAPS == p_target_member_info->member_type)
    {
        nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_ADD_SPECIAL_MEMBER;
    }
    else
    {
       nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_ADD_MEMBER;
    }

    if (is_linkagg_member)
    {
        local_chip_number = sys_humber_get_local_chip_num();
        for (local_chip_id = 0; local_chip_id < local_chip_number; local_chip_id++)
        {
             SYS_MCAST_DBG_INFO("add_local_member:lchip:%d gport:%d \n", local_chip_id, p_target_member_info->dest_id);

             /* IP Multicast Nexthop Process */
             nexthop_member_info.lchip           = local_chip_id;
             nexthop_member_info.is_linkagg = SYS_NH_PARAM_IS_LINKAGG_TRUE;

             kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
             CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
        }
    }
    else
    {
         if (FALSE == sys_humber_chip_is_local(target_chip, &local_chip_id))
         {
             return  CTC_E_NONE;
         }

         SYS_MCAST_DBG_INFO("add_local_member:lchip:%d gport:%d \n", local_chip_id, p_target_member_info->dest_id);

         /* IP Multicast Nexthop Process */
         nexthop_member_info.is_linkagg = SYS_NH_PARAM_IS_LINKAGG_FALSE;
         nexthop_member_info.lchip           = local_chip_id;

         kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
         CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));

    }

    return  CTC_E_NONE;
}

static int32
_sys_humber_mc_add_remote_chip_member(uint32 nexthop_id,  uint8 target_chip)
{
    uint8 local_chip_id = 0;
    uint8 global_chip_id = 0;
    uint8 local_chip_number = 0;
    sys_nh_param_mcast_member_t nexthop_member_info;
    sys_nh_param_mcast_group_t nh_mcast_group;

    kal_memset(&nexthop_member_info, 0, sizeof(sys_nh_param_mcast_member_t));
    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));
    nh_mcast_group.nhid = nexthop_id;
    nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_ADD_MEMBER;

    local_chip_number = sys_humber_get_local_chip_num();
    for (local_chip_id = 0; local_chip_id < local_chip_number; local_chip_id++)
    {
        sys_humber_get_gchip_id(local_chip_id, &global_chip_id);
        if (target_chip == global_chip_id)
        {
            continue ;
        }
        /* target chip is NOT the local chip on current line card */
        /* add remote member on local chip, pointing to target chip */
        SYS_MCAST_DBG_INFO("add_remote_chip_member:lchip:%d gport:%d \n", local_chip_id, target_chip);

        /* IP Multicast Nexthop Process */
        nexthop_member_info.destid = target_chip;
        nexthop_member_info.is_linkagg = SYS_NH_PARAM_IS_LINKAGG_FALSE;
        nexthop_member_info.lchip           = local_chip_id;
        nexthop_member_info.member_type     = SYS_NH_PARAM_MCAST_MEM_REMOTE;
        kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
        CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
    }


    return  CTC_E_NONE;
};


int32
sys_humber_mc_add_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id)
{
    uint8 is_linkagg_member = FALSE;
    uint8 target_chip = 0;
    uint8 loop_gchip = 0;
    sys_local_member_node_t *p_local_member_node = NULL;
    sys_mcast_add_analyze_member_t param;
    uint8 max_global_chip_id = CTC_MAX_GCHIP_CHIP_ID;

    /*target chip indicates the location of member port */
    target_chip = SYS_MAP_GPORT_TO_GCHIP(p_target_member_info->dest_id);

    CTC_PTR_VALID_CHECK(p_local_member_list);
    CTC_PTR_VALID_CHECK(p_target_member_info);
    CTC_GLOBAL_PORT_CHECK(p_target_member_info->dest_id);

    kal_memset(&param,0,sizeof(param));
    param.p_local_member_list = p_local_member_list[target_chip];
    param.p_target_member_info = p_target_member_info;

    CTC_ERROR_RETURN(_sys_humber_mc_add_analyze_member_list(&param));

    if (param.is_member_existed)
    {
        SYS_MCAST_DBG_INFO("This member can NOT be added when already existed!\n");
        return CTC_E_NONE;
    }

    if (CTC_LINKAGG_CHIPID == target_chip)
    {
       /*for linkAgg member, add a local member to all global chips */

        is_linkagg_member = TRUE;

        /*local member*/
        CTC_ERROR_RETURN(_sys_humber_mc_add_local_member( p_target_member_info, nexthop_id,   target_chip, is_linkagg_member));

        if (param.is_first_local_member_to_add && SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY)
        {
            for (loop_gchip = 0; loop_gchip <= max_global_chip_id; loop_gchip++)
            {
                if ( p_local_member_list[loop_gchip]->count != 0)
                {
                    continue ;
                }
                /*remote chip member*/
                CTC_ERROR_RETURN(_sys_humber_mc_add_remote_chip_member( nexthop_id,   loop_gchip));
            }
        }
    }
    else
    {
        if (target_chip > max_global_chip_id)
        {
            return CTC_E_IPMC_EXCEED_GLOBAL_CHIP;
        }
        /*for not linkAgg member, add local member only to target chip */
        is_linkagg_member = FALSE;

        /* there's local member previously, add local member on target chip instantly */
        CTC_ERROR_RETURN(_sys_humber_mc_add_local_member( p_target_member_info, nexthop_id, target_chip, is_linkagg_member));

        if (param.is_first_local_member_to_add && SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY
          && 0 == CTC_SLISTCOUNT( p_local_member_list[CTC_LINKAGG_CHIPID]))
        {
            /* there's no local member on the target chip, consider remote member in addition. */
            CTC_ERROR_RETURN(_sys_humber_mc_add_remote_chip_member(  nexthop_id,   target_chip));
        }
    }

    /* for linkAgg member, add member node to local_member_list[0x1F] */
    p_local_member_node = (sys_local_member_node_t *)mem_malloc(MEM_LINKLIST_MODULE, sizeof(sys_local_member_node_t));
    if (NULL == p_local_member_node)
    {
        SYS_MCAST_DBG_INFO("ERROR: There's no memory available!");
        return CTC_E_NO_MEMORY;
    }
    p_local_member_node->head.next = NULL;
    kal_memcpy(& p_local_member_node->member_info,p_target_member_info,sizeof(sys_mc_member_info_t));
    ctc_slist_add_tail(p_local_member_list[target_chip],&p_local_member_node->head);

    return CTC_E_NONE;
}

static int32
_sys_humber_mc_remove_analyze_member_list(sys_mcast_remove_analyze_member_t *p_param)

{
    uint16 target_global_port = 0;
    uint16 target_vlan_id = 0;
    uint32 ref_nhid = 0;
    uint8 aps_protection_path = 0;
    uint8 is_remote_mem = 0;
    sys_local_member_node_t *p_local_member = NULL;
    sys_local_member_node_t *p_next_local_member = NULL;
    ctc_slist_t *p_local_member_list = NULL;
    ctc_slistnode_t *lisnode= NULL;
    ctc_slistnode_t *next_lisnode= NULL;

    CTC_PTR_VALID_CHECK(p_param);
    CTC_PTR_VALID_CHECK(p_param->p_local_member_list);

    p_local_member_list = p_param->p_local_member_list;

    /* initiate output value */
    p_param->is_member_existed              = FALSE;
    p_param->is_last_local_member_to_remove = FALSE;

    target_global_port  = p_param->p_target_member_info->dest_id;
    target_vlan_id      = p_param->p_target_member_info->vlan_id;
    ref_nhid            = p_param->p_target_member_info->ref_nhid;
    aps_protection_path = p_param->p_target_member_info->aps_protection_path;
    is_remote_mem       = p_param->p_target_member_info->remote_chip;

    if (0 == CTC_SLISTCOUNT(p_local_member_list))
    {
        /* local member does NOT exist on target chip, return instantly */
        p_param->is_member_existed = FALSE;
        return CTC_E_NONE;
    }

    CTC_SLIST_LOOP_DEL(p_local_member_list, lisnode,next_lisnode)
    {
        p_local_member = _ctc_container_of(lisnode, sys_local_member_node_t, head);
        p_next_local_member = next_lisnode?_ctc_container_of(next_lisnode, sys_local_member_node_t, head):NULL;
        if (lisnode == CTC_SLISTHEAD(p_local_member_list))
        {
              if(p_local_member
                 && (( (p_local_member->member_info.member_type == SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE)
                        && (aps_protection_path == p_local_member->member_info.aps_protection_path) )
                     || (p_local_member->member_info.member_type != SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE) )
                 && (ref_nhid == p_local_member->member_info.ref_nhid )
                 && (target_global_port == p_local_member->member_info.dest_id)
                 && (target_vlan_id == p_local_member->member_info.vlan_id)
                 && (is_remote_mem == p_local_member->member_info.remote_chip))
            {
                p_param->pre_or_next_node = next_lisnode;
                p_param->del_node = lisnode;
                p_param->is_member_existed = TRUE;
                /*rewrite member information*/
                kal_memcpy(p_param->p_target_member_info,&p_local_member->member_info,sizeof(sys_mc_member_info_t));

                break;
            }
        }
         if(p_next_local_member
              &&(( (p_next_local_member->member_info.member_type == SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE)
                    && (aps_protection_path == p_next_local_member->member_info.aps_protection_path) )
                 || (p_next_local_member->member_info.member_type != SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE) )
              && (ref_nhid == p_next_local_member->member_info.ref_nhid )
              && (target_global_port == p_next_local_member->member_info.dest_id)
              && (target_vlan_id == p_next_local_member->member_info.vlan_id)
              && (is_remote_mem == p_next_local_member->member_info.remote_chip)
           )
        {
            p_param->pre_or_next_node = lisnode;
            p_param->del_node = next_lisnode;
            p_param->is_member_existed = TRUE;
            /*rewrite member information*/
            kal_memcpy(p_param->p_target_member_info, &p_next_local_member->member_info, sizeof(sys_mc_member_info_t));


            break;
        }
        /* record the previous node for deleting */
    }

    if (p_param->is_member_existed == TRUE
      && CTC_SLISTCOUNT(p_local_member_list) == 1)
    {
        p_param->is_last_local_member_to_remove = TRUE;
    }
    return  CTC_E_NONE;
}

static int32
_sys_humber_mc_remove_local_member(sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id,  uint8 target_chip, uint8 is_linkagg_member)
{
    uint8 local_chip_id = 0;
    uint8 local_chip_number = 0;
    sys_nh_param_mcast_member_t nexthop_member_info;
    sys_nh_param_mcast_group_t nh_mcast_group;

    CTC_PTR_VALID_CHECK(p_target_member_info);

    kal_memset(&nexthop_member_info, 0, sizeof(sys_nh_param_mcast_member_t));
    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));

    nexthop_member_info.member_type     = p_target_member_info->member_type;
    switch(p_target_member_info->member_type)
    {
    case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS:
    case SYS_NH_PARAM_BRGMC_MEM_RAPS:
        nexthop_member_info.destid = CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        break;
    case SYS_NH_PARAM_BRGMC_MEM_LOCAL_WITH_APS_BRIDGE:
        nexthop_member_info.destid =  p_target_member_info->aps_group_id;
        nexthop_member_info.ref_nhid = p_target_member_info->ref_nhid;
        nexthop_member_info.is_protection_path = p_target_member_info->aps_protection_path;
        break;
    case SYS_NH_PARAM_IPMC_MEM_LOCAL:
        nexthop_member_info.destid =  CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.vid    = p_target_member_info->vlan_id;
       nexthop_member_info.l3if_type = p_target_member_info->l3if_type;
        break;
    case SYS_NH_PARAM_BRGMC_MEM_UPMEP:
        nexthop_member_info.destid =  CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        nexthop_member_info.vid    = p_target_member_info->vlan_id;
        break;
    case SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH:
        nexthop_member_info.ref_nhid = p_target_member_info->ref_nhid;
        nexthop_member_info.destid = CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        break;
    case SYS_NH_PARAM_MCAST_MEM_REMOTE:
        nexthop_member_info.destid =CTC_MAP_GPORT_TO_LPORT(p_target_member_info->dest_id);
        break;
    default:
        break;

    }

    nh_mcast_group.nhid = nexthop_id;
    if (p_target_member_info->is_upmep ||
        SYS_NH_PARAM_BRGMC_MEM_RAPS == p_target_member_info->member_type)
    {
        nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_DEL_SPECIAL_MEMBER;
    }
    else
    {
       nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_DEL_MEMBER;
    }

    if (is_linkagg_member)
    {
        local_chip_number = sys_humber_get_local_chip_num();

        for (local_chip_id = 0; local_chip_id < local_chip_number; local_chip_id++)
        {
            SYS_MCAST_DBG_INFO("remove_local_member:lchip:%d gport:%d \n", local_chip_id, nexthop_member_info.destid);
            /* IP Multicast Nexthop Process */
            nexthop_member_info.is_linkagg = SYS_NH_PARAM_IS_LINKAGG_TRUE;
            nexthop_member_info.lchip           = local_chip_id;
           kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
           CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
        }
    }
    else
    {

        if (FALSE == sys_humber_chip_is_local(target_chip, &local_chip_id))
        {
          return  CTC_E_NONE;
        }
        SYS_MCAST_DBG_INFO("remove_local_member:lchip:%d gport:%d \n", local_chip_id, nexthop_member_info.destid);

        nexthop_member_info.is_linkagg = SYS_NH_PARAM_IS_LINKAGG_FALSE;
        nexthop_member_info.lchip           = local_chip_id;
        kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
        CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
    }

    return  CTC_E_NONE;
}
static int32
_sys_humber_mc_remove_remote_chip_member( uint32 nexthop_id,  uint8 target_chip)
{
    uint8 local_chip_id = 0;
    uint8 global_chip_id = 0;
    uint8 local_chip_number = 0;
    sys_nh_param_mcast_member_t nexthop_member_info;
    sys_nh_param_mcast_group_t nh_mcast_group;

    kal_memset(&nexthop_member_info, 0, sizeof(sys_nh_param_mcast_member_t));
    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));

    nh_mcast_group.nhid = nexthop_id;
    nh_mcast_group.opcode = SYS_HBNH_PARAM_MCAST_DEL_MEMBER;

    local_chip_number = sys_humber_get_local_chip_num();
    for (local_chip_id = 0; local_chip_id < local_chip_number; local_chip_id++)
    {
        sys_humber_get_gchip_id(local_chip_id, &global_chip_id);

        if (target_chip == global_chip_id)
        {
            continue ;
        }
        /* target chip is NOT the local chip on current line card */
        /* remove remote member from local chip, pointing to target chip */
        SYS_MCAST_DBG_INFO("remove_remote_chip_member:lchip:%d gport:%d \n", local_chip_id, target_chip);

        /* Layer2 Multicast Nexthop Process */
        nexthop_member_info.destid = target_chip;
        nexthop_member_info.lchip           = local_chip_id;
        nexthop_member_info.member_type     = SYS_NH_PARAM_MCAST_MEM_REMOTE;
       // CTC_ERROR_RETURN(sys_humber_brgmc_nh_remove_member(nexthop_id, &nexthop_member_info));
        kal_memcpy(&(nh_mcast_group.mem_info), &nexthop_member_info, sizeof(sys_nh_param_mcast_member_t));
        CTC_ERROR_RETURN(sys_humber_mcast_nh_update(&nh_mcast_group));
    }


    return  CTC_E_NONE;
}

int32
sys_humber_mc_remove_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id)
{

    uint8 is_linkagg_member = FALSE;
    uint8 target_chip = 0;
    uint8 loop_gchip = 0;
    uint8 max_global_chip_id = CTC_MAX_GCHIP_CHIP_ID;
    sys_mcast_remove_analyze_member_t param;
    sys_local_member_node_t *p_local_member = NULL;

    /*target chip indicates the location of member port*/
    target_chip = SYS_MAP_GPORT_TO_GCHIP(p_target_member_info->dest_id);

    CTC_PTR_VALID_CHECK(p_local_member_list);
    CTC_PTR_VALID_CHECK(p_target_member_info);

    /* target chip indicates the location of member port */
    CTC_GLOBAL_PORT_CHECK(p_target_member_info->dest_id);

    kal_memset(&param,0,sizeof(param));
    param.p_local_member_list = p_local_member_list[target_chip];
    param.p_target_member_info = p_target_member_info;

    CTC_ERROR_RETURN(_sys_humber_mc_remove_analyze_member_list(&param));

    if (FALSE == param.is_member_existed)
    {
        SYS_MCAST_DBG_INFO("This member can NOT be removed when doesn't existed!\n");
        return CTC_E_ENTRY_NOT_EXIST;
    }

    if (CTC_LINKAGG_CHIPID == target_chip)
    {

        /* for linkAgg member, remove a local member from all global chips */
        is_linkagg_member = TRUE;
        if (param.is_last_local_member_to_remove && SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY)
        {
            for (loop_gchip = 0; loop_gchip <= max_global_chip_id; loop_gchip++)
            {
                if ( p_local_member_list[loop_gchip]->count != 0)
                {
                    continue ;
                }
                /* remote chip member */
                CTC_ERROR_RETURN(_sys_humber_mc_remove_remote_chip_member(nexthop_id,  loop_gchip));
            }
        }
         /* local member */
        CTC_ERROR_RETURN(_sys_humber_mc_remove_local_member( param.p_target_member_info, nexthop_id,  target_chip, is_linkagg_member));


    }
    else
    {
        if (target_chip > max_global_chip_id)
        {
            return CTC_E_IPMC_EXCEED_GLOBAL_CHIP;
        }
        /* for not linkAgg member, remove local member only from target chip */
        is_linkagg_member = FALSE;
        if (param.is_last_local_member_to_remove && SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY
          && 0 == CTC_SLISTCOUNT(p_local_member_list[CTC_LINKAGG_CHIPID]))
        {
            /* the member to be removed is the last member on the target chip, consider remote member in addition. */
            CTC_ERROR_RETURN(_sys_humber_mc_remove_remote_chip_member( nexthop_id,   target_chip));
        }
        /* there're other members still existed on the target chip, remove local member instantly */
       CTC_ERROR_RETURN(_sys_humber_mc_remove_local_member( param.p_target_member_info, nexthop_id,   target_chip, is_linkagg_member));

    }

    p_local_member = _ctc_container_of(param.del_node, sys_local_member_node_t, head);
    ctc_slist_delete_node2(p_local_member_list[target_chip],param.pre_or_next_node,param.del_node);
    mem_free(p_local_member);
    p_local_member = NULL;
    return CTC_E_NONE;
}

int32
sys_humber_mc_remove_all_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], uint32 nexthop_id)
{
    uint8 is_linkagg_member = FALSE;
    uint8 linkagg_member_num = 0;

    uint8 loop_gchip = 0;
    uint8 max_global_chip_id = CTC_MAX_GCHIP_CHIP_ID;
    sys_local_member_node_t *p_local_member= NULL;
    sys_mc_member_info_t target_member_info;
    ctc_slistnode_t *lisnode = NULL;
    ctc_slistnode_t *next_lisnode= NULL;


    /*1) delete all remote chip*/

    if (SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY)
    {
        linkagg_member_num = CTC_SLISTCOUNT(p_local_member_list[CTC_LINKAGG_CHIPID]);

        for (loop_gchip = 0; loop_gchip <= max_global_chip_id; loop_gchip ++)
        {
            if (CTC_SLISTCOUNT(p_local_member_list[loop_gchip]) != 0
                || linkagg_member_num != 0)
            {
                _sys_humber_mc_remove_remote_chip_member(nexthop_id,   loop_gchip);

            }
        }
    }

    /*2) delete all local member chip (chip : gchip can be 0~29)
        and delete remote entry when  SYS_MCAST_AUTO_REMOTE_CHIP_ENTRY == 0
    */
    is_linkagg_member = FALSE;
    for (loop_gchip = 0; loop_gchip <= max_global_chip_id; loop_gchip ++)
    {
        CTC_SLIST_LOOP_DEL(p_local_member_list[loop_gchip], lisnode, next_lisnode)
        {
            p_local_member = _ctc_container_of(lisnode, sys_local_member_node_t, head);
            kal_memcpy(&target_member_info,&p_local_member->member_info,sizeof(sys_mc_member_info_t));
            _sys_humber_mc_remove_local_member(&target_member_info, nexthop_id,  loop_gchip,is_linkagg_member);

            mem_free(p_local_member);
        }
        ctc_slist_delete_all_node(p_local_member_list[loop_gchip]);
    }

    /*3) delete all local member chip(chip :linkagg)*/
    loop_gchip = CTC_LINKAGG_CHIPID;
    is_linkagg_member = TRUE;
    CTC_SLIST_LOOP_DEL(p_local_member_list[loop_gchip], lisnode, next_lisnode)
    {

        p_local_member = _ctc_container_of(lisnode, sys_local_member_node_t, head);
        kal_memcpy(&target_member_info,&p_local_member->member_info,sizeof(sys_mc_member_info_t));
       _sys_humber_mc_remove_local_member(&target_member_info, nexthop_id,  loop_gchip,is_linkagg_member);
         mem_free(p_local_member);
    }
    ctc_slist_delete_all_node(p_local_member_list[loop_gchip]);


  return CTC_E_NONE;
}

int32
sys_humber_dump_member_list(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM])
{
    uint8 chip_id = 0;
    uint32 count = 0;
    uint16 dest_id;
    uint16 vlan_id;

    ctc_slistnode_t *lisnode = NULL;
    sys_local_member_node_t *p_local_member = NULL;

    SYS_MCAST_DBG_INFO("\n-----------------------------------------------------------------------------\n");
    SYS_MCAST_DBG_INFO("%-5s %-5s %-8s %-8s %-10s %-5s %5s","Chip","GPort", "ref_nhid","aps-grp","l3if-type"," Vlan","Remote");
    SYS_MCAST_DBG_INFO("\n-----------------------------------------------------------------------------\n");

    for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id++)
    {
        count = p_local_member_list[chip_id]->count;
        if (count != 0)
        {
            CTC_SLIST_LOOP(p_local_member_list[chip_id], lisnode)
            {

                p_local_member  = _ctc_container_of(lisnode, sys_local_member_node_t, head);

                dest_id     = p_local_member->member_info.dest_id;
                vlan_id         = p_local_member->member_info.vlan_id;
               SYS_MCAST_DBG_INFO("%-5d %-5d %-8d %-8d %-10d %-5d %5d\n",
                chip_id,  dest_id, p_local_member->member_info.ref_nhid, p_local_member->member_info.aps_group_id, p_local_member->member_info.l3if_type, vlan_id,p_local_member->member_info.remote_chip);
            }
        }
    }
    SYS_MCAST_DBG_INFO("-----------------------------------------------------------------------------\n\n");

    return  CTC_E_NONE;
}

