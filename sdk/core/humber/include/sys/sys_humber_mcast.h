/****************************************************************************
 * sys_humber_mcast.h :  shared by both L2 and L3 multicast
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History:
 * Revision :   R0.01
 * Author   :   Chenyg
 * Date     :   2009-12-30
 ****************************************************************************/
#ifndef _SYS_HUMEBR_MCAST_H
#define _SYS_HUMEBR_MCAST_H
#include "ctc_const.h"
#include "ctc_linklist.h"
#define SYS_MCAST_DBG_INFO(FMT, ...) \
                    { \
                       CTC_DEBUG_OUT_INFO(mcast, mcast, MCAST_SYS, FMT,##__VA_ARGS__); \
                    }


#define SYS_IPMC_L3IF_TYPE_CHECK(type)\
    {\
        if ((type) >= MAX_L3IF_TYPE_NUM)\
        {\
            return CTC_E_BAD_L3IF_TYPE;\
        }\
    }


/* define DS record member attribute */
struct sys_mc_member_info_s
{
    uint16  dest_id;/*local portid, LAGID without 0x1F(eg.0x1F01), global dest chipid*/
    uint16  vlan_id;
    uint8    member_type;   /*sys_nh_param_mcast_member_type_t*/
    uint8    aps_protection_path;
    uint16  aps_group_id;
    uint32  ref_nhid;
    uint8   rsv0;
    uint8   remote_chip;
    uint8   l3if_type;
    uint8   is_upmep;
};
typedef struct sys_mc_member_info_s sys_mc_member_info_t;


struct sys_local_member_node_s
{
    ctc_slistnode_t head;
    sys_mc_member_info_t member_info;
};
typedef struct sys_local_member_node_s sys_local_member_node_t;


extern int32
sys_humber_mc_add_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id);
extern int32
sys_humber_mc_remove_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], sys_mc_member_info_t *p_target_member_info, uint32 nexthop_id);
extern int32
sys_humber_mc_remove_all_member(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM], uint32 nexthop_id);
extern int32
sys_humber_dump_member_list(ctc_slist_t *p_local_member_list[CTC_MAX_HUMBER_CHIP_NUM]);


#endif /*_SYS_HUMEBR_MCAST_H*/

