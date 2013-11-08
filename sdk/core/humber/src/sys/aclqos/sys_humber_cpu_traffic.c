/**
 @file sys_humber_cpu_traffic.c

 @date 2010-01-13

 @version v2.0

*/

/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_common.h"
#include "ctc_hash.h"
#include "ctc_cpu_traffic.h"
#include "ctc_queue.h"

#include "sys_humber_cpu_traffic.h"
#include "sys_humber_chip.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_queue_sch.h"
#include "sys_humber_queue_shape.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"

#include "sys_humber_internal_port.h"

#include "drv_io.h"
#include "drv_enum.h"

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
uint8 sys_excp_max_sub_index[MAX_CTC_EXCEPTION];
uint8 sys_excp_queue_num = 0;
bool  has_calc_excp_queue_num = FALSE;

sys_cpu_traffic_info_t sys_cpu_traffic_info = {
    {
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL2Span0",   "Ingress L2 span0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL2Span1",	  "Ingress L2 span1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL2Span2",   "Ingress L2 span2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL2Span3",   "Ingress L2 span3"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL3Span0",   "Ingress L3 span0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL3Span1",   "Ingress L3 span1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL3Span2",   "Ingress L3 span2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressL3Span3",   "Ingress L3 span3"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressACLLog0",   "Ingress ACL log0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressACLLog1",   "Ingress ACL log1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressACLLog2",   "Ingress ACL log2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressACLLog3",   "Ingress ACL log3"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressQoSLog0",   "Ingress QoS log0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressQoSLog1",   "Ingress QoS log1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressQoSLog2",   "Ingress QoS log2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressQoSLog3",   "Ingress QoS log3"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL2Span0",    "Egress L2 span0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL2Span1",    "Egress L2 span1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL2Span2",    "Egress L2 span2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL2Span3",    "Egress L2 span3"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL3Span0",    "Egress L3 span0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL3Span1",    "Egress L3 span1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL3Span2",    "Egress L3 span2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressL3Span3",    "Egress L3 span3"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressACLLog0",    "Egress ACL log0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressACLLog1",    "Egress ACL log1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressACLLog2",    "Egress ACL log2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressACLLog3",    "Egress ACL log3"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressQoSLog0",    "Egress QoS log0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressQoSLog1",    "Egress QoS log1"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressQoSLog2",    "Egress QoS log2"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressQoSLog3",    "Egress QoS log3"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress0",         "User-ID"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress1",         "Protocol VLAN"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress2",         "Bridge"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress3",         "Route IP-DA"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress4",         "Route ICMP"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress5",         "Learning cache full"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress6",         "Multicast RPF check fail"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Ingress7",         "Security"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress0",          "egress mtu check fail and original packet don't frag"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress1",          "egress mtu check fail and original packet can frag"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress2",          "egress TTL = 0"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress3",          "egress multicast TTL threshold"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress4",          "Unused"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress5",          "egress tunnel mtu check fail and original packet don't frag"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress6",          "egress tunnel mtu check fail and original packet can frag"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "Egress7",          "egress BFD UDP"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressPortLog",	    "Ingress port log"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressSeqnumCheck", "Ingress sequence number check fail"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressParserPTP",   "Ingress parser PTP"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "IngressOAM",         "Ingress OAM"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressPortLog",      "Egress port log"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressSeqnumCheck",  "Egress sequence number check fail"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressParserPTP",    "Egress parser PTP"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "EgressOAM",          "Egress OAM"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException0",    "Ucast IP header error or IP martion address - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException1",    "Ucast IP options - CPU"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException2",    "Ucast GRE unknown option or protocol - CPU"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException3",    "Ucast ISATAP source address check failure - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException4",    "Ucast IP TTL check failure - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException5",    "Ucast RPF failure - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException6",    "Ucast or Mcast more RPF - CPU"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException7",    "Ucast Link ID check failure - CPU"},

        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException8",    "MPLS label out of range - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException9",    "MPLS Sbit error - discard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException10",   "MPLS TTL check failure - diacard"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException11",   "Unused"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException12",   "IGMP Snooping packet - CPU"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException13",   "Unused"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException14",   "Unused"},
        {0, SYS_CPU_TRAFFIC_DEFAULT_IND_RATE, "FatalException15",   "Unused"}
    },

    {
        {75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000},
        {75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000, 75000},
        {75000, 75000, 75000, 75000, 75000},
        {75000, 75000, 75000, 75000, 75000, 75000, 75000},
        {75000, 75000, 75000, 75000}
    },

    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },

    SYS_CPU_TRAFFIC_DEFAULT_TOTAL_RATE
};

#define SYS_CPU_TRAFFIC_INIT_CHECK() \
    {\
        if(  sys_excp_queue_num == 0)\
            return CTC_E_NOT_INIT;\
    }

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/

/**
 @brief Mapping packet-to-CPU reason to a class level. Each packet-to-CPU reason corresponds to a queue,
        CPU queues among different classes get bandwidth in SP mode.
*/
int32
sys_humber_packet_tocpu_set_reason_class(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint8 class)
{
    uint8  lchip, lchip_num;
    uint16 queue_id;

    SYS_CPU_TRAFFIC_INIT_CHECK();
    CTC_MAX_VALUE_CHECK(reason, MAX_CTC_EXCEPTION - 1);
    CTC_MAX_VALUE_CHECK(class, CTC_MAX_QUEUE_CLASS_NUM - 1);
    CTC_MAX_VALUE_CHECK(sub_idx, sys_excp_max_sub_index[reason]);

    SYS_CPU_TRAFFIC_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_EXCP_CPU, reason, sub_idx, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, class));
    }

    if(CTC_EXCEPTION_INGRESS_2 == reason)
    {
        sys_cpu_traffic_info.sub_index_class[0][sub_idx] = class;
    }
    else if(CTC_EXCEPTION_INGRESS_3 == reason)
    {
        sys_cpu_traffic_info.sub_index_class[1][sub_idx] = class;
    }
    else if(CTC_EXCEPTION_INGRESS_7 == reason)
    {
        sys_cpu_traffic_info.sub_index_class[2][sub_idx] = class;
    }
    else if(CTC_EXCEPTION_INGRESS_PARSER_PTP == reason)
    {
        sys_cpu_traffic_info.sub_index_class[3][sub_idx] = class;
    }
    else if(CTC_EXCEPTION_EGRESS_PARSER_PTP == reason)
    {
        sys_cpu_traffic_info.sub_index_class[4][sub_idx] = class;
    }
    else
    {
        sys_cpu_traffic_info.excp_info[reason].class = class;
    }

    SYS_CPU_TRAFFIC_DBG_INFO("reason = %d, sub_idx = %d, class = %d\n", reason, sub_idx, class);

    return CTC_E_NONE;
}


/**
 @brief Set the individual rate limit for the given packet-to-CPU reason.
*/
int32
sys_humber_packet_tocpu_limit_individual_rate(ctc_packet_tocpu_reason_t reason, uint8 sub_idx, uint32 rate)
{
    ctc_queue_shape_t shape;
    uint8  lchip, lchip_num;
    uint16 queue_id;

    SYS_CPU_TRAFFIC_INIT_CHECK();
    CTC_MAX_VALUE_CHECK(reason, MAX_CTC_EXCEPTION - 1);
    if (rate != CTC_SPECIAL_CPU_TRAFFIC_RATE)
    {
        CTC_MAX_VALUE_CHECK(rate, SYS_MAX_SHAPE_RATE);
    }
    CTC_MAX_VALUE_CHECK(sub_idx, sys_excp_max_sub_index[reason]);

    SYS_CPU_TRAFFIC_DBG_FUNC();

    CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_EXCP_CPU, reason, sub_idx, &queue_id));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (CTC_SPECIAL_CPU_TRAFFIC_RATE == rate)
        {
            CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));
            SYS_CPU_TRAFFIC_DBG_INFO("chip = %d, reason = %d, sub_idx = %d, no rate limit\n", lchip, reason, sub_idx);
        }
        else
        {
            kal_memset(&shape, 0, sizeof(shape));
            shape.cir = rate / lchip_num;
            shape.cbs = SYS_SPECIAL_SHAPE_BURST;
            shape.pir = rate / lchip_num;
            shape.pbs = SYS_SPECIAL_SHAPE_BURST;
            CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape(lchip, queue_id, &shape));
            SYS_CPU_TRAFFIC_DBG_INFO("chip = %d, reason = %d, sub_idx = %d, rate = %d\n", lchip, reason, sub_idx, rate/lchip_num);
        }
    }

    if(CTC_EXCEPTION_INGRESS_2 == reason)
    {
        sys_cpu_traffic_info.sub_index_rate[0][sub_idx] = rate;
    }
    else if(CTC_EXCEPTION_INGRESS_3 == reason)
    {
        sys_cpu_traffic_info.sub_index_rate[1][sub_idx] = rate;
    }
    else if(CTC_EXCEPTION_INGRESS_7 == reason)
    {
        sys_cpu_traffic_info.sub_index_rate[2][sub_idx] = rate;
    }
    else if(CTC_EXCEPTION_INGRESS_PARSER_PTP == reason)
    {
        sys_cpu_traffic_info.sub_index_rate[3][sub_idx] = rate;
    }
    else if(CTC_EXCEPTION_EGRESS_PARSER_PTP == reason)
    {
        sys_cpu_traffic_info.sub_index_rate[4][sub_idx] = rate;
    }
    else
    {
        sys_cpu_traffic_info.excp_info[reason].rate = rate;
    }

    return CTC_E_NONE;
}


/**
 @brief Set the total rate limit for all packet-to-CPU reasons.
*/
int32
sys_humber_packet_tocpu_limit_total_rate(uint32 rate)
{
    ctc_port_shape_t shape;
    uint8  lchip, lchip_num;

    SYS_CPU_TRAFFIC_INIT_CHECK();

    if (rate != CTC_SPECIAL_CPU_TRAFFIC_RATE)
    {
        CTC_MAX_VALUE_CHECK(rate, SYS_MAX_SHAPE_RATE);
    }

    SYS_CPU_TRAFFIC_DBG_FUNC();

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {

            if (CTC_SPECIAL_CPU_TRAFFIC_RATE == rate)
            {
                /* unset cpu channel shaping */
                CTC_ERROR_RETURN(sys_humber_queue_unset_channel_shape(lchip, SYS_CPU_CHANNEL_ID));

                SYS_CPU_TRAFFIC_DBG_INFO("chip = %d, no total rate limit\n", lchip);
            }
            else
            {
                kal_memset(&shape, 0, sizeof(shape));
                shape.pir = rate / lchip_num;
                shape.pbs = SYS_SPECIAL_SHAPE_BURST;

                /* set channel shaping */
                CTC_ERROR_RETURN(sys_humber_queue_set_channel_shape(lchip, SYS_CPU_CHANNEL_ID, &shape));

                SYS_CPU_TRAFFIC_DBG_INFO("chip = %d, total rate = %d\n", lchip, rate/lchip_num);
            }

    }

    sys_cpu_traffic_info.total_rate = rate;

    return CTC_E_NONE;
}

/**
 @brief Set shaping for the given queue in normal cpu forward.
*/
int32
sys_humber_cpu_traffic_set_fwd_queue_shape(uint8 qid, ctc_queue_shape_t* p_shape)
{
    uint8 lchip, lchip_num;
    uint16 queue_id;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_NORMAL_CPU, SYS_CPU_CHANNEL_ID, qid, &queue_id));

        CTC_ERROR_RETURN(sys_humber_queue_set_queue_shape(lchip, queue_id, p_shape));
    }

    return CTC_E_NONE;
}

/**
 @brief Cancel shaping for the given queue in noraml cpu forward.
*/
int32
sys_humber_cpu_traffic_unset_fwd_queue_shape(uint8 qid)
{
    uint8 lchip, lchip_num;
    uint16 queue_id;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_NORMAL_CPU, SYS_CPU_CHANNEL_ID, qid, &queue_id));

        CTC_ERROR_RETURN(sys_humber_queue_unset_queue_shape(lchip, queue_id));
    }

    return CTC_E_NONE;
}

/**
 @brief Set class for the given queue in normal cpu forward.
*/
int32
sys_humber_cpu_traffic_set_fwd_queue_class(uint8 qid, uint8 class)
{
    uint8 lchip, lchip_num;
    uint16 queue_id;

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_queue_get_queue_id(CTC_QUEUE_TYPE_NORMAL_CPU, SYS_CPU_CHANNEL_ID, qid, &queue_id));

        CTC_ERROR_RETURN(sys_humber_queue_set_class(lchip, queue_id, class));
    }

    return CTC_E_NONE;
}

/**
 @brief Get exception queue number.
*/
uint8
sys_humber_get_excp_queue_num(void)
{
    uint8  excp;

    if (!has_calc_excp_queue_num)
    {
        /* default init max_sub_index = 0 for all exceptions, indicating no sub-exceptions */
        kal_memset(sys_excp_max_sub_index, 0, sizeof(sys_excp_max_sub_index));

        /* Some special exceptions have multiple sub indices */
        sys_excp_max_sub_index[CTC_EXCEPTION_INGRESS_2] = 15;
        sys_excp_max_sub_index[CTC_EXCEPTION_INGRESS_3] = 15;
        sys_excp_max_sub_index[CTC_EXCEPTION_INGRESS_7] = 4;
        sys_excp_max_sub_index[CTC_EXCEPTION_INGRESS_PARSER_PTP] = 6;
        sys_excp_max_sub_index[CTC_EXCEPTION_EGRESS_PARSER_PTP]  = 3;

        /* compute the number of exception queues */
        sys_excp_queue_num = MAX_CTC_EXCEPTION;
        for (excp = CTC_EXCEPTION_INGRESS_0; excp < MAX_CTC_EXCEPTION; excp++)
        {
            sys_excp_queue_num += sys_excp_max_sub_index[excp];
        }

        has_calc_excp_queue_num = TRUE;
    }

    return sys_excp_queue_num;
}


/**
 @brief Get exception queue offset.
*/
int32
sys_humber_get_excp_queue_offset(ctc_packet_tocpu_reason_t reason, uint16* p_queue_id)
{
    uint16 excp;

    SYS_CPU_TRAFFIC_INIT_CHECK();
    CTC_MAX_VALUE_CHECK(reason, MAX_CTC_EXCEPTION - 1);
    CTC_PTR_VALID_CHECK(p_queue_id);

    *p_queue_id = 0;

    for (excp = 0; excp < reason; excp++)
    {
        *p_queue_id += sys_excp_max_sub_index[excp] + 1;
    }

    return CTC_E_NONE;
}


/**
 @brief Get fatal exception index base.
*/
static uint8
_sys_humber_get_fatal_excp_index_base(void)
{
    uint8 index_base = 0;

    sys_excp_queue_num = sys_humber_get_excp_queue_num();

    index_base = sys_excp_queue_num - (CTC_FATAL_EXCEPTION_15 - CTC_FATAL_EXCEPTION_0 + 1);

    return index_base;
}



/**
 @brief This function is used to set exception (packet to port or local cpu or drop)

 @param[in] excp_idx fatal exception index

 @param[in] dest_id  dest_type ==CTC_EXCP_DEST_TO_LOCAL_PORT or CTC_EXCP_DEST_TO_REMOTE_PORT indicate gloal port id,
                     else dest_type == CTC_EXCP_DEST_TO_LOCAL_CPU indicate cpu mac index

 @param[in] dest_type  judge the packet to loacl cpu or remote cpu or drop

 @return CTC_E_XXX
 */
int32
sys_humber_cpu_traffic_set_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    uint32 cmd              = 0;
    uint32 field_destmap    = 0;
    uint8 lport             = 0;
    uint8 gchip             = 0;
    uint32 sub_idx_en       = 0;
    uint16 excp_queue_base  = 0;
    uint32 dsnh_offset      = 0;
    uint8 lchip             = 0;
    uint8 lchip_num         = 0;
    ds_met_fifo_excp_t met_fifo_excp;
    buf_retrv_exception_mem_t bufrev_exp;

    CTC_MAX_VALUE_CHECK(excp, CTC_EXCEPTION_EGRESS_OAM);

    kal_memset(&met_fifo_excp,0,sizeof(ds_met_fifo_excp_t));
    kal_memset(&bufrev_exp,0,sizeof(buf_retrv_exception_mem_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (CTC_EXCP_DEST_TO_REMOTE_PORT== dest_type )  /* to remote port */
        {
             /*
             dest_id = gchip + lport
             */
            lport = CTC_MAP_GPORT_TO_LPORT(dest_id);
            gchip = SYS_MAP_GPORT_TO_GCHIP(dest_id);
            field_destmap = SYS_BUILD_EXCP_DESTMAP_REMOTE(gchip, lport);
            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_REMOTECPU_NH, &dsnh_offset);
            sub_idx_en = 0;
        }
        else if(CTC_EXCP_DEST_TO_LOCAL_CPU == dest_type) /* to local cpu port */
        {
            sys_humber_get_gchip_id(lchip,&gchip);
            CTC_ERROR_RETURN(sys_humber_get_excp_queue_offset(excp,&excp_queue_base));

            field_destmap = SYS_BUILD_EXCP_DESTMAP_LOCAL(gchip, excp_queue_base);
             /*  dsnh_offset = excp << 4; */
            dsnh_offset = CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_NORMAL_EXCP,excp,dest_id);
            sub_idx_en = IS_SUB_IDX_EN(excp);

        }
        else if(CTC_EXCP_DEST_TO_LOCAL_PORT == dest_type) /* to local port */
        {
             /*
             dest_id = gchip + lport
             */
            lport = CTC_MAP_GPORT_TO_LPORT(dest_id);
            gchip = SYS_MAP_GPORT_TO_GCHIP(dest_id);
            field_destmap = SYS_BUILD_EXCP_DESTMAP_LOCAL_PORT(gchip, lport);
             sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH, &dsnh_offset);
            sub_idx_en = 0;

        }
        else if (CTC_EXCP_DEST_TO_DROP == dest_type)  /* to drop */
        {
            sys_humber_get_gchip_id(lchip,&gchip);
            field_destmap = SYS_HBNH_ENCODE_DESTMAP(0, gchip, SYS_RESERVED_INTERNAL_PORT_FOR_DROP);
            sub_idx_en = 0;
        }
        met_fifo_excp.next_hop_ext = 0;
        met_fifo_excp.length_adjust_type = 0;
        met_fifo_excp.dest_map = field_destmap;
        met_fifo_excp.exception_sub_index_en = sub_idx_en;

        /* write metfifo */
        cmd = DRV_IOW(IOC_TABLE, DS_MET_FIFO_EXCP,DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, excp, cmd, &met_fifo_excp));


        bufrev_exp.exception_data = dsnh_offset;
        bufrev_exp.exception_sub_index_en = sub_idx_en;

        /* write buf_retrive_excp_mem */
        cmd = DRV_IOW(IOC_TABLE, BUF_RETRV_EXCEPTION_MEM, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, excp, cmd, &bufrev_exp));

    }

    return CTC_E_NONE;

}


/**
 @brief This function is used to set fatal exception (packet to port or local cpu or drop)

 @param[in] excp_idx, fatal exception index

 @param[in] dest_id, the global phy port

 @param[in] dest_type, judge the packet to loacl cpu or remote cpu or drop

 @return CTC_E_XXX
 */
int32
sys_humber_cpu_traffic_set_fatal_exception(uint8 excp, uint16 dest_id, ctc_excp_dest_type_t dest_type)
{
    uint8  gchip                = 0;
    uint32 offset               = 0;
    uint8  lport                = 0;
    uint32 dsnh_offset          = 0;
    uint32 field_destmap        = 0;
    uint32 field_nexthop_ptr    = 0;
    uint32 fwd_cmd              = 0;
    uint8 fatal_excp_queue_base = 0;
    uint8 fatal_excp_queue      = 0;
    uint8 lchip                 = 0;
    uint8 lchip_num             = 0;
    ds_fwd_t dsfwd;


    CTC_MAX_VALUE_CHECK(excp, CTC_FATAL_EXCEPTION_15 - CTC_FATAL_EXCEPTION_0);
    kal_memset(&dsfwd, 0, sizeof(dsfwd));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {

        if (CTC_EXCP_DEST_TO_REMOTE_PORT == dest_type)  /* to remote port */
        {
         /*
           dest_id: gchip+lport
        */
            lport = CTC_MAP_GPORT_TO_LPORT(dest_id);
            gchip = SYS_MAP_GPORT_TO_GCHIP(dest_id);
            field_destmap = SYS_BUILD_EXCP_DESTMAP_REMOTE(gchip, lport);

            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_REMOTECPU_NH, &dsnh_offset);
            field_nexthop_ptr = dsnh_offset;
        }
        else if(CTC_EXCP_DEST_TO_LOCAL_CPU == dest_type) /* to local cpu port */
        {
           sys_humber_get_gchip_id(lchip,&gchip);
            fatal_excp_queue_base = _sys_humber_get_fatal_excp_index_base();
            fatal_excp_queue = excp + fatal_excp_queue_base;

            field_destmap = SYS_BUILD_EXCP_DESTMAP_LOCAL(gchip, fatal_excp_queue);
            /*field_nexthop_ptr = (excp + CTC_FATAL_EXCEP_UCAST_IP_HDR_ERROR)<<4;  */

           field_nexthop_ptr =  CTC_CPU_PKTTOCPU_BUILDUP_NHPTR(CTC_CPU_PKTTOCPU_MOD_FATAL_EXCP, excp, dest_id);

        }
        else if(CTC_EXCP_DEST_TO_LOCAL_PORT == dest_type) /* to local port */
        {
             /*
             dest_id = gchip + lport
             */
            sys_humber_get_gchip_id(lchip,&gchip);
            lport = CTC_MAP_GPORT_TO_LPORT(dest_id);
            field_destmap = SYS_BUILD_EXCP_DESTMAP_LOCAL_PORT(gchip, lport);
            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH, &dsnh_offset);
            field_nexthop_ptr = dsnh_offset;

        }
        else if (CTC_EXCP_DEST_TO_DROP == dest_type)   /* to drop */
        {
            sys_humber_get_gchip_id(lchip,&gchip);
            field_destmap = SYS_HBNH_ENCODE_DESTMAP(0, gchip, SYS_RESERVED_INTERNAL_PORT_FOR_DROP);
            sys_humber_nh_get_resolved_offset(SYS_HBNH_RES_OFFSET_TYPE_BYPASS_NH, &dsnh_offset);
            field_nexthop_ptr = dsnh_offset;
        }

        /*get fatal exception resver offset*/
        CTC_ERROR_RETURN(sys_humber_nh_get_fatal_excp_dsnh_offset(lchip,&offset));
        offset = offset + excp;

        dsfwd.dest_map = field_destmap;
        dsfwd.next_hop_ptr = field_nexthop_ptr;

        /*write dsfwd*/
        fwd_cmd = DRV_IOW(IOC_REG, DS_FWD, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, fwd_cmd, &dsfwd));

    }

    return CTC_E_NONE;

}



static int32
_sys_humber_set_cpu_traffic_default_info(void)
{
    ctc_packet_tocpu_reason_t reason;
    uint8 sub_idx;

    for (reason = 0; reason < MAX_CTC_EXCEPTION; reason++)
    {
        for (sub_idx = 0; sub_idx <= sys_excp_max_sub_index[reason]; sub_idx++)
        {
            CTC_ERROR_RETURN(
                sys_humber_packet_tocpu_limit_individual_rate(reason, sub_idx, sys_cpu_traffic_info.excp_info[reason].rate));
        }
    }

    CTC_ERROR_RETURN(sys_humber_packet_tocpu_limit_total_rate(sys_cpu_traffic_info.total_rate));

    return CTC_E_NONE;
}


int32
sys_humber_cpu_traffic_show_exception(uint8 excp)
{
    uint32 cmd              = 0;
    uint32 field_destmap    = 0;
    uint16 dest_id          = 0;
    uint8 dest_type         = 0;


    CTC_MAX_VALUE_CHECK(excp, CTC_EXCEPTION_EGRESS_OAM);

    /* read destmap from metfifo */
    cmd = DRV_IOR(IOC_TABLE, DS_MET_FIFO_EXCP, DS_MET_FIFO_EXCP_DEST_MAP);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, excp, cmd, &field_destmap));

    dest_type = (field_destmap >> 12)&0xF;
    dest_id = ((field_destmap>>16)&0x1F)<<8 | (field_destmap&0xFF);

    if (SYS_QSEL_TYPE_SGMAC_PORT_TO_CPU == dest_type)  /* to remote port */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("exception %2d    :remote cpu, remote cpu port:0x%x\r\n",excp,dest_id);
    }
    else if(SYS_QSEL_TYPE_EXCP_CPU == dest_type) /* to local cpu port */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("exception %2d    :local cpu \r\n",excp);
    }
    else if( SYS_RESERVED_INTERNAL_PORT_FOR_DROP == dest_id)     /* to drop */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("exception %2d    :drop \r\n",excp);
    }
    else
    {
        SYS_CPU_TRAFFIC_DBG_INFO("exception %2d    :local port:0x%x\r\n",excp,dest_id);
    }

    return CTC_E_NONE;

}

int32
sys_humber_cpu_traffic_show_fatal_exception(uint8 excp)
{
    uint32 cmd              = 0;
    uint32 field_destmap    = 0;
    uint32 offset           = 0;
    uint16 dest_id          = 0;
    uint8 dest_type         = 0;

    CTC_MAX_VALUE_CHECK(excp, CTC_FATAL_EXCEPTION_15 - CTC_FATAL_EXCEPTION_0);

    /*get fatal exception resver offset*/
    CTC_ERROR_RETURN(sys_humber_nh_get_fatal_excp_dsnh_offset(0,&offset));
    offset = offset + excp;

    /* read destmap from metfifo */
    cmd = DRV_IOR(IOC_REG, DS_FWD, DS_FWD_DEST_MAP);
    CTC_ERROR_RETURN(drv_tbl_ioctl(0, offset, cmd, &field_destmap));

    dest_type = (field_destmap >> 12)&0xF;
    dest_id = ((field_destmap>>16)&0x1F)<<8 | (field_destmap&0xFF);

    if (SYS_QSEL_TYPE_SGMAC_PORT_TO_CPU == dest_type)  /* to remote port */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("fatal exception %2d    :remote cpu, remote cpu port:0x%x\r\n",excp,dest_id);
    }
    else if(SYS_QSEL_TYPE_EXCP_CPU == dest_type) /* to local cpu port */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("fatal exception %2d    :local cpu \r\n",excp);
    }
    else if( SYS_RESERVED_INTERNAL_PORT_FOR_DROP == dest_id)     /* to drop */
    {
        SYS_CPU_TRAFFIC_DBG_INFO("fatal exception %2d    :drop \r\n",excp);
    }
    else
    {
        SYS_CPU_TRAFFIC_DBG_INFO("fatal exception %2d    :local port:0x%x\r\n",excp,dest_id);
    }


    return CTC_E_NONE;

}


int32
sys_humber_show_cpu_traffic_info(uint8 mode)
{
    ctc_packet_tocpu_reason_t reason;
    uint8 sub_index;

    SYS_CPU_TRAFFIC_INIT_CHECK();

    SYS_CPU_TRAFFIC_DBG_INFO("=====================================================================================\n");
    SYS_CPU_TRAFFIC_DBG_INFO("reason number:                    %d\n", MAX_CTC_EXCEPTION);
    if (sys_cpu_traffic_info.total_rate != CTC_SPECIAL_CPU_TRAFFIC_RATE)
    {
        SYS_CPU_TRAFFIC_DBG_INFO("total rate (kbps) of all reasons: %u\n", sys_cpu_traffic_info.total_rate / (1000 / 8));
    }
    else
    {
        SYS_CPU_TRAFFIC_DBG_INFO("total rate (kbps) of all reasons: none\n");
    }


    SYS_CPU_TRAFFIC_DBG_INFO("-------------------------------------------------------------------------------------\n");
    SYS_CPU_TRAFFIC_DBG_INFO("reason             sub-index  rate (kbps)   class  Usage\n");
    SYS_CPU_TRAFFIC_DBG_INFO("-------------------------------------------------------------------------------------\n");

    reason = (mode == SYS_SHOW_CPU_TRAFFIC_INFO_MODE_BRIEF) ? CTC_EXCEPTION_INGRESS_0 : 0;
    for (; reason < MAX_CTC_EXCEPTION; reason++)
    {
        if (sys_cpu_traffic_info.excp_info[reason].rate != CTC_SPECIAL_CPU_TRAFFIC_RATE)
        {
            if(CTC_EXCEPTION_INGRESS_2 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13u %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        sys_cpu_traffic_info.sub_index_rate[0][sub_index] / (1000 / 8),
                        sys_cpu_traffic_info.sub_index_class[0][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_3 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13u %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        sys_cpu_traffic_info.sub_index_rate[1][sub_index] / (1000 / 8),
                        sys_cpu_traffic_info.sub_index_class[1][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_7 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13u %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        sys_cpu_traffic_info.sub_index_rate[2][sub_index] / (1000 / 8),
                        sys_cpu_traffic_info.sub_index_class[2][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_PARSER_PTP == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13u %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        sys_cpu_traffic_info.sub_index_rate[3][sub_index] / (1000 / 8),
                        sys_cpu_traffic_info.sub_index_class[3][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_EGRESS_PARSER_PTP == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13u %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        sys_cpu_traffic_info.sub_index_rate[4][sub_index] / (1000 / 8),
                        sys_cpu_traffic_info.sub_index_class[4][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else
            {
                SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10s %-13u %-6d %-s\n",
                sys_cpu_traffic_info.excp_info[reason].name,
                "/",
                sys_cpu_traffic_info.excp_info[reason].rate /(1000/8),
                sys_cpu_traffic_info.excp_info[reason].class,
                sys_cpu_traffic_info.excp_info[reason].desp);
            }

        }
        else
        {
            if(CTC_EXCEPTION_INGRESS_2 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13s %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        "none",
                        sys_cpu_traffic_info.sub_index_class[0][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_3 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13s %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        "none",
                        sys_cpu_traffic_info.sub_index_class[1][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_7 == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13s %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        "none",
                        sys_cpu_traffic_info.sub_index_class[2][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_INGRESS_PARSER_PTP == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13s %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        "none",
                        sys_cpu_traffic_info.sub_index_class[3][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else if(CTC_EXCEPTION_EGRESS_PARSER_PTP == reason)
            {
                for(sub_index=0; sub_index<=sys_excp_max_sub_index[reason]; sub_index++)
                {
                    SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10d %-13s %-6d %-s\n",
                        sys_cpu_traffic_info.excp_info[reason].name,
                        sub_index,
                        "none",
                        sys_cpu_traffic_info.sub_index_class[4][sub_index],
                        sys_cpu_traffic_info.excp_info[reason].desp);
                }
            }
            else
            {
                SYS_CPU_TRAFFIC_DBG_INFO("%-18s %-10s %-13s %-6d %-s\n",
                    sys_cpu_traffic_info.excp_info[reason].name,
                    "/",
                    "none",
                    sys_cpu_traffic_info.excp_info[reason].class,
                    sys_cpu_traffic_info.excp_info[reason].desp);
            }

        }
    }

    return CTC_E_NONE;
}

/**
 @brief CPU traffic initialization.
*/
int32
sys_humber_cpu_traffic_init(void)
{

    uint8 max_excp_index = 0;

    uint8 excp           = 0;

    sys_excp_queue_num = sys_humber_get_excp_queue_num();


    /* Init all non-fatabl exceptions to local cpu */
    for (excp = 0; excp < CTC_FATAL_EXCEPTION_0; excp++)
    {
          CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_exception(excp, 0, CTC_EXCP_DEST_TO_LOCAL_CPU));
    }

    /* Init fatal exception to local cpu */
    max_excp_index = CTC_FATAL_EXCEPTION_15 - CTC_FATAL_EXCEPTION_0;
    for (excp = 0; excp <= max_excp_index; excp++)
    {
          CTC_ERROR_RETURN(sys_humber_cpu_traffic_set_fatal_exception(excp,0,CTC_EXCP_DEST_TO_LOCAL_CPU));

    }

    CTC_ERROR_RETURN(_sys_humber_set_cpu_traffic_default_info());

    return CTC_E_NONE;
}



