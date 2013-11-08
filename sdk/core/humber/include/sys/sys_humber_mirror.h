/**
 @file sys_humber_mirror.h

 @date 2009-10-21

 @version v2.0

*/

#ifndef _SYS_HUMBER_MIRROR_H
#define _SYS_HUMBER_MIRROR_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/

#include "ctc_mirror.h"
#include "ctc_debug.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define SYS_MIRROR_SESSION_NUM 4

#define SYS_MIRROR_INGRESS_L2_SPAN_INDEX_BASE      0
#define SYS_MIRROR_INGRESS_L3_SPAN_INDEX_BASE      4
#define SYS_MIRROR_INGRESS_ACL_LOG_INDEX_BASE      8
#define SYS_MIRROR_INGRESS_QOS_LOG_INDEX_BASE      12
#define SYS_MIRROR_EGRESS_L2_SPAN_INDEX_BASE       16
#define SYS_MIRROR_EGRESS_L3_SPAN_INDEX_BASE       20
#define SYS_MIRROR_EGRESS_ACL_LOG_INDEX_BASE       24
#define SYS_MIRROR_EGRESS_QOS_LOG_INDEX_BASE       28



#define SYS_MIRROR_DISCARD_CHECK(flag)\
    if (0 !=(flag & (~(CTC_MIRROR_QOSLOG_DISCARD|CTC_MIRROR_ACLLOG_DISCARD| \
        CTC_MIRROR_L3SPAN_DISCARD|CTC_MIRROR_L2SPAN_DISCARD)))) \
        return CTC_E_INVALID_PARAM

#define SYS_MIRROR_DBG_INFO(FMT, ...) \
                    {\
                       CTC_DEBUG_OUT_INFO(mirror, mirror, MIRROR_SYS, FMT, ##__VA_ARGS__);\
                    }

#define SYS_MIRROR_DBG_FUNC() \
                    {\
                        CTC_DEBUG_OUT_FUNC(mirror, mirror, MIRROR_SYS);\
                    }

/****************************************************************************
*
* Function
*
*****************************************************************************/
extern int32
sys_humber_mirror_init(void);

extern int32
sys_humber_mirror_set_port_enable(uint16 gport, ctc_direction_t dir, uint8 session_id);

extern int32
sys_humber_mirror_set_port_disable(uint16 gport, ctc_direction_t dir);

extern int32
sys_humber_mirror_get_port_info(uint16 gport, ctc_direction_t dir, bool *enable, uint8 *session_id);

extern int32
sys_humber_mirror_set_vlan_enable(uint16 vlan_id, ctc_direction_t dir, uint8 session_id);

extern int32
sys_humber_mirror_set_vlan_disable(uint16 vlan_id, ctc_direction_t dir);

extern int32
sys_humber_mirror_get_vlan_info(uint16 vlan_id, ctc_direction_t dir, bool *enable, uint8 *session_id);

extern int32
sys_humber_mirror_set_dest(ctc_mirror_dest_t *mirror);

extern int32
sys_humber_mirror_rspan_set_dest(ctc_mirror_dest_t *mirror, uint32 nh_id);

extern int32
sys_humber_mirror_rspan_escape_en(bool enable);

extern int32
sys_humber_mirror_rspan_escape_mac(ctc_mirror_rspan_escape_t escape);

extern int32
sys_humber_mirror_unset_dest(ctc_mirror_dest_t *mirror);

extern int32
sys_humber_mirror_set_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool enable);

extern int32
sys_humber_mirror_get_mirror_discard(ctc_direction_t dir, ctc_mirror_discard_t discard_flag, bool *enable);


#endif
