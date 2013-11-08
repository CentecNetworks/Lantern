/**
 @file sys_humber_aclqos_entry.c

 @date 2009-10-16

 @version v2.0

*/

/****************************************************************************
  *
  * Header Files
  *
  ****************************************************************************/
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_common.h"
#include "ctc_macro.h"
#include "ctc_debug.h"
#include "ctc_hash.h"
#include "ctc_stats.h"
#include "ctc_parser.h"
#include "ctc_aclqos.h"
#include "ctc_cpu_traffic.h"

#include "sys_humber_opf.h"
#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"
#include "sys_humber_parser.h"
#include "sys_humber_stats.h"
#include "sys_humber_aclqos_label.h"
#include "sys_humber_qos_policer.h"
#include "sys_humber_aclqos_entry.h"
#include "sys_humber_queue_enq.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_internal_port.h"
/* XXX_OSS_TRIM: #include "sys_humber_mirror.h" */

#include "drv_enum.h"
#include "drv_humber.h"
#include "drv_tbl_reg.h"
#include "drv_io.h"

/****************************************************************************
  *
  * Defines and Macros
  *
  ****************************************************************************/

#define INVALID_OP_INDEX             0xFF
#define INVALID_ENTRY_OFFSET         0xFFFFFFFF
#define SYS_ACL_INVALID_INDEX        -1

#define SPECIAL_STATS_PTR            0xFFFF
#define SPECIAL_POLICER_PTR          0xFFFF

#define MAX_TCP_FALG_REF_NUM         0xFFFF
#define MAX_L4_PORT_REF_NUM          0xFFFF

#define ACL_REDIRECT_BUCKET_SIZE     64

#define IS_ACL_TABLE(type)  ((type == SYS_PORT_ACL_LABEL) || \
        ((type == SYS_VLAN_ACL_LABEL) && (sys_aclqos_entry_ctl.is_dual_aclqos_lookup)))

#define IS_QOS_TABLE(type)  (((type == SYS_PORT_QOS_LABEL) || (type == SYS_VLAN_QOS_LABEL)) || \
        ((type == SYS_VLAN_ACL_LABEL) && (!sys_aclqos_entry_ctl.is_dual_aclqos_lookup)))

#define IS_PBR_TABLE(type)  (type == SYS_PBR_ACL_LABEL)

/**
 @brief  acl redirect data structure
*/
struct sys_acl_redirect_s
{
    uint32 nhid;
    uint32 ds_fwd_offset;
    uint32 ref;
};
typedef struct sys_acl_redirect_s sys_acl_redirect_t;


/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/
/*static char *sys_aclqos_entry_type_name[MAX_CTC_ACLQOS_KEY] = {
    "mac", "ipv4", "mpls", "ipv6"
};
*/

static sys_aclqos_entry_ctl_t sys_aclqos_entry_ctl;
static ctc_hash_t *p_sys_acl_redirect_hash[CTC_MAX_LOCAL_CHIP_NUM];

static sys_aclqos_entry_ctl_t* acl_master = &sys_aclqos_entry_ctl;

/****************************************************************************
 *
 * Function
 *
 ****************************************************************************/


/**
 @brief Acl redirect hash key hook.
*/
static INLINE uint32
_sys_humber_acl_redirect_hash_key(void* data)
{
    sys_acl_redirect_t* p_acl_redirect = (sys_acl_redirect_t *)data;

    return p_acl_redirect->nhid;
}

/**
 @brief Acl redirect hash comparison hook.
*/
static INLINE bool
_sys_humber_acl_redirect_hash_cmp(void* data1, void* data2)
{
    sys_acl_redirect_t *p_acl_redirect1 = (sys_acl_redirect_t *)data1;
    sys_acl_redirect_t *p_acl_redirect2 = (sys_acl_redirect_t *)data2;

    if (p_acl_redirect1->nhid == p_acl_redirect2->nhid)
    {
        return TRUE;
    }

    return FALSE;
}


/**
 @brief copy dsfwd for acl redirect
*/
static int32
_sys_humber_acl_copy_dsfwd(uint8 lchip, uint32 src_fwd_ptr, uint32 dest_fwd_ptr)
{
    uint32 cmd;
    ds_fwd_t dsfwd;

    kal_memset(&dsfwd, 0, sizeof(ds_fwd_t));

    cmd = DRV_IOR(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, src_fwd_ptr, cmd, &dsfwd));

    cmd = DRV_IOW(IOC_TABLE, DS_FWD, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dest_fwd_ptr, cmd, &dsfwd));

    return CTC_E_NONE;
}


/**
 @brief map acl/qos action from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_action(uint8 lchip, sys_aclqos_label_t* p_label,
                              ctc_aclqos_action_t* p_ctc_action,
                              sys_aclqos_action_t* p_sys_action)
{
    ctc_direction_t dir;
    sys_nh_offset_array_t offset_array;
    sys_humber_opf_t opf;
    uint32 ds_fwd_offset = 0;
    uint32 ds_fwd_base = 0;
    sys_nh_info_ecmp_t* p_nhinfo = 0;
    sys_acl_redirect_t acl_redirect;
    sys_acl_redirect_t* p_acl_redirect = 0;
    uint32 ret = 0;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_action);
    CTC_PTR_VALID_CHECK(p_sys_action);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    kal_memset(&opf, 0, sizeof(opf));
    kal_memset(&acl_redirect, 0, sizeof(acl_redirect));

    dir = p_label->dir;

    /* deny */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DISCARD_FLAG)
    {
        p_sys_action->flag.discard = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.discard = 1\n");
    }

    /* deny bridging */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DENY_BRIDGE_FLAG)
    {
        p_sys_action->flag.deny_bridge = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_bridge = 1\n");
    }

    /* deny learning */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DENY_LEARN_FLAG)
    {
        p_sys_action->flag.deny_learning = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_learning = 1\n");
    }

    /* deny route */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DENY_ROUTE_FLAG)
    {
        p_sys_action->flag.deny_route = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_route = 1\n");
    }

    /* deny replace cos */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DENY_REPLACE_COS_FLAG)
    {
        p_sys_action->flag.deny_replace_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_replace_cos = 1\n");
    }

    /* deny replace dscp */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_DENY_REPLACE_DSCP_FLAG)
    {
        p_sys_action->flag.deny_replace_dscp = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_replace_dscp = 1\n");
    }

    /* stats */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_STATS_FLAG)
    {
        if (p_ctc_action->flag & CTC_ACLQOS_ACTION_FLOW_ID_FLAG)
        {
            return CTC_E_ACLQOS_COLLISION_FIELD;
        }

        p_sys_action->flag.stats = 1;
        p_sys_action->stats_or_flowid.stats_ptr = SPECIAL_STATS_PTR;
        /*stats ptr is allocated by sdk user, so we just use it here */
        if (p_ctc_action->stats_ptr >= 4096)
        {
            return CTC_E_INVALID_PARAM;
        }
        p_sys_action->stats_or_flowid.stats_ptr = p_ctc_action->stats_ptr;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->stats_ptr = %d\n", p_sys_action->stats_or_flowid.stats_ptr);
    }

    /* flow id */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_FLOW_ID_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_action->flow_id, 254);
        p_sys_action->flag.flow_id = 1;
        /* the queue select and flowId should be revert */
        p_sys_action->stats_or_flowid.flow_id = 255 - p_ctc_action->flow_id;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flow_id = %d\n", p_sys_action->stats_or_flowid.flow_id);
    }

    /* flow policer */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_FLOW_POLICER_FLAG)
    {
        CTC_ERROR_RETURN(sys_humber_qos_flow_policer_bind(lchip, p_ctc_action->policer_id));

        p_sys_action->flag.flow_policer = 1;
        p_sys_action->policer_id = p_ctc_action->policer_id;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->policer_id = 0x%x\n", p_sys_action->policer_id);
    }

    /* copy to cpu */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_COPY_TO_CPU_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_action->log_weight, 0xF);
        p_ctc_action->log_weight = p_ctc_action->log_weight ? p_ctc_action->log_weight : 0xF;

        p_sys_action->flag.random_log = 1;
        p_sys_action->acl_log_id = CTC_ACLQOS_LOG_SESSION_3;
        p_sys_action->random_threshold_shift = p_ctc_action->log_weight;

        SYS_ACLQOS_ENTRY_DBG_INFO("copy to cpu, logid is 3 weight %d\n", p_ctc_action->log_weight);
    }

    /* random log */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_RAND_LOG_FLAG)
    {
        if (p_ctc_action->flag & CTC_ACLQOS_ACTION_COPY_TO_CPU_FLAG)
        {
            return CTC_E_INVALID_PARAM;
        }

        CTC_MAX_VALUE_CHECK(p_ctc_action->log_weight, 0xF);
        CTC_MAX_VALUE_CHECK(p_ctc_action->log_session_id, CTC_ACLQOS_LOG_SESSION_3-1);

        p_sys_action->flag.random_log = 1;
        p_sys_action->acl_log_id = p_ctc_action->log_session_id;
        p_sys_action->random_threshold_shift = p_ctc_action->log_weight;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.random_log = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->acl_log_id = %d\n", p_sys_action->acl_log_id);
    }

    /* priority */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PRIORITY_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_action->priority, 63);

        p_sys_action->flag.priority = 1;
        p_sys_action->priority = p_ctc_action->priority;
        p_sys_action->color = p_ctc_action->color;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.priority = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->priority = %d\n", p_sys_action->priority);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->color = %d\n", p_sys_action->color);
    }

    /* qos policy */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_TRUST_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_action->trust, MAX_CTC_QOS_TRUST - 1);
        p_sys_action->flag.trust = 1;
        p_sys_action->trust = p_ctc_action->trust;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.trust = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->trust = %d\n", p_sys_action->trust);
    }

    /* ds forward */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_REDIRECT_FLAG)
    {
        acl_redirect.nhid = p_ctc_action->fwd.fwd_nh_id;
        p_acl_redirect = ctc_hash_lookup(p_sys_acl_redirect_hash[lchip], &acl_redirect);
        if(!p_acl_redirect)
        {
            /* new acl redirect */
            p_acl_redirect = (sys_acl_redirect_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_acl_redirect_t));
            if (!p_acl_redirect)
            {
                return CTC_E_NO_MEMORY;
            }
            kal_memset(p_acl_redirect, 0, sizeof(sys_acl_redirect_t));

            CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_ctc_action->fwd.fwd_nh_id, offset_array));

            /*alloc ds_fwd_offset of acl*/
            opf.pool_type = OPF_ACL_FWD_SRAM;
            opf.pool_index = lchip;
            ret = sys_humber_opf_alloc_offset(&opf, 1, &ds_fwd_offset);
            if(ret)
            {
                mem_free(p_acl_redirect);
                return ret;
            }
            p_sys_action->ds_fwd_ptr = ds_fwd_offset;

            /*copy dsfwd for the dsfwdptr of the key only have 12bit*/
            ds_fwd_base = sys_aclqos_entry_ctl.acl_fwd_base & 0xFFFFFF00;
            ret = _sys_humber_acl_copy_dsfwd(lchip, offset_array[lchip], ds_fwd_base + p_sys_action->ds_fwd_ptr);
            if(ret)
            {
                sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset);
                mem_free(p_acl_redirect);
                return ret;
            }

            /* insert into hash table */
            p_acl_redirect->nhid = p_ctc_action->fwd.fwd_nh_id;
            p_acl_redirect->ds_fwd_offset = ds_fwd_offset;
            p_acl_redirect->ref ++;
            if (!ctc_hash_insert(p_sys_acl_redirect_hash[lchip], p_acl_redirect))
            {
                sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset);
                mem_free(p_acl_redirect);
                return CTC_E_NO_MEMORY;
            }

            SYS_ACLQOS_ENTRY_DBG_INFO("nh_id = %u, ds_fwd_ptr = %u, acl_ds_fwd_ptr = %u\n",
                p_ctc_action->fwd.fwd_nh_id, offset_array[lchip], ds_fwd_base + p_sys_action->ds_fwd_ptr);
        }
        else
        {
            p_sys_action->ds_fwd_ptr = p_acl_redirect->ds_fwd_offset;
            p_acl_redirect->ref ++;

            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->ds_fwd_ptr = %d\n", p_sys_action->ds_fwd_ptr);
        }

        p_sys_action->fwd.fwd_nh_id = p_ctc_action->fwd.fwd_nh_id;

        p_sys_action->flag.fwd = 1;
        p_sys_action->flag.deny_bridge = 1;
        p_sys_action->flag.deny_route = 1;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_bridge = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.deny_route = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.fwd = 1\n");
    }

    /* pbr fwd */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_FWD_FLAG)
    {
        p_sys_action->flag.pbr_fwd = 1;
        CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_ctc_action->fwd.fwd_nh_id, offset_array));

        p_sys_action->pbr_fwd_ptr = offset_array[lchip];
        p_sys_action->fwd.fwd_nh_id = p_ctc_action->fwd.fwd_nh_id;
        if (!(p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_ECMP_FLAG))
        {
            CTC_ERROR_RETURN(sys_humber_nh_get_l3ifid(p_ctc_action->fwd.fwd_nh_id, &p_sys_action->pbr_vrfid));
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->pbr_vrfid = %d\n", p_sys_action->pbr_vrfid);
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_fwd = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->pbr_fwd_ptr = %d\n", p_sys_action->pbr_fwd_ptr);
    }
    else if ((!(p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_DENY_FLAG))
        && (SYS_PBR_ACL_LABEL == p_label->type))
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_fwd = 0\n");
        return CTC_E_ACL_PBR_ENTRY_NO_NXTTHOP;
    }

    /* pbr ttl-check */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_TTL_CHECK_FLAG)
    {
        p_sys_action->flag.pbr_ttl_check = 1;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_ttl_check = 1\n");
    }

    /* pbr icmp-check */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_ICMP_CHECK_FLAG)
    {
        p_sys_action->flag.pbr_icmp_check = 1;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_icmp_check = 1\n");
    }

    /* pbr copy-to-cpu */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_COPY_TO_CPU_FLAG)
    {
        p_sys_action->flag.pbr_copy_to_cpu = 1;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_copy_to_cpu = 1\n");
    }

    /* pbr ecmp */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_ECMP_FLAG)
    {
        p_sys_action->flag.pbr_ecmp = 1;
        CTC_ERROR_RETURN(sys_humber_nh_get_nhinfo_by_nhid(p_ctc_action->fwd.fwd_nh_id, (sys_nh_info_com_t**)&p_nhinfo));
        p_sys_action->pbr_ecpn = p_nhinfo->valid_item_cnt - 1;
        p_sys_action->pbr_vrfid = p_nhinfo->rpf_array[0].oif_id;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_ecpn = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->pbr_ecpn = %d\n", p_sys_action->pbr_ecpn);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->pbr_vrfid = %d\n", p_sys_action->pbr_vrfid);
    }

    /* pbr deny */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_PBR_DENY_FLAG)
    {
        p_sys_action->flag.pbr_deny = 1;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.pbr_deny = 1\n");
    }

    /* invalid */
    if (p_ctc_action->flag & CTC_ACLQOS_ACTION_INVALID_FLAG)
    {
        p_sys_action->flag.invalid = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_action->flag.invalid = 1\n");
    }

    return CTC_E_NONE;
}


/**
 @brief map acl/qos mac key from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_mac_key(uint8 lchip, sys_aclqos_label_t* p_label,
                               ctc_aclqos_mac_key_t* p_ctc_key,
                               sys_aclqos_mac_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* mac da */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_MACDA_FLAG)
    {
        p_sys_key->flag.macda = 1;
        kal_memcpy(p_sys_key->mac_da, p_ctc_key->mac_da, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_da_mask, p_ctc_key->mac_da_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da[0], p_sys_key->mac_da[1], p_sys_key->mac_da[2],
                                  p_sys_key->mac_da[3], p_sys_key->mac_da[4], p_sys_key->mac_da[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da_mask[0], p_sys_key->mac_da_mask[1], p_sys_key->mac_da_mask[2],
                                  p_sys_key->mac_da_mask[3], p_sys_key->mac_da_mask[4], p_sys_key->mac_da_mask[5]);
    }

    /* mac sa */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_MACSA_FLAG)
    {
        p_sys_key->flag.macsa = 1;
        kal_memcpy(p_sys_key->mac_sa, p_ctc_key->mac_sa, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_sa_mask, p_ctc_key->mac_sa_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_sa = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa[0], p_sys_key->mac_sa[1], p_sys_key->mac_sa[2],
                                  p_sys_key->mac_sa[3], p_sys_key->mac_sa[4], p_sys_key->mac_sa[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa_mask[0], p_sys_key->mac_sa_mask[1], p_sys_key->mac_sa_mask[2],
                                  p_sys_key->mac_sa_mask[3], p_sys_key->mac_sa_mask[4], p_sys_key->mac_sa_mask[5]);
    }

    /* src cos */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->cos, 7);
        p_sys_key->flag.cos = 1;
        p_sys_key->cos = p_ctc_key->cos;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cos = %d\n", p_sys_key->cos);
    }

    /* cvlan */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_CVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->cvlan);
		p_sys_key->flag.cvlan = 1;
        p_sys_key->cvlan = p_ctc_key->cvlan;
        if(TRUE == p_ctc_key->cvlan_mask_valid)
        {
		    p_sys_key->cvlan_mask = p_ctc_key->cvlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->cvlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cvlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan = %d\n", p_sys_key->cvlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan_mask = %d\n", p_sys_key->cvlan_mask);
    }

    /* ctag cos */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_CTAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cos, 0x7);
        p_sys_key->ctag_cos_cfi = p_ctc_key->ctag_cos << 1;
        p_sys_key->flag.ctag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    /* ctag cfi */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_CTAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cfi, 0x1);
        p_sys_key->ctag_cos_cfi += p_ctc_key->ctag_cfi;
        p_sys_key->flag.ctag_cfi = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    /* svlan */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_SVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->svlan);
        p_sys_key->flag.svlan = 1;
        p_sys_key->svlan = p_ctc_key->svlan;
        if(TRUE == p_ctc_key->svlan_mask_valid)
        {
		    p_sys_key->svlan_mask = p_ctc_key->svlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->svlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.svlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan = %d\n", p_sys_key->svlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan_mask = %d\n", p_sys_key->svlan_mask);
    }

    /* stag cos */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_STAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cos, 0x7);
        p_sys_key->stag_cos_cfi = p_ctc_key->stag_cos << 1;
        p_sys_key->flag.stag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    /* stag cfi */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_STAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cfi, 0x1);
        p_sys_key->flag.stag_cfi = 1;
        p_sys_key->stag_cos_cfi += p_ctc_key->stag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    /* eth type */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_ETHTYPE_FLAG)
    {
        p_sys_key->flag.eth_type = 1;
        p_sys_key->eth_type = p_ctc_key->eth_type;
        if(TRUE == p_ctc_key->eth_type_mask_valid)
        {
		    p_sys_key->eth_type_mask = p_ctc_key->eth_type_mask;
        }
        else
        {
            p_sys_key->eth_type_mask = 0xFFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.eth_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->eth_type = %d\n", p_sys_key->eth_type);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->eth_type_mask = %d\n", p_sys_key->eth_type_mask);
    }

    /* l2 type */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_L2TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l2_type, 0xF);
        p_sys_key->flag.l2_type = 1;
        p_sys_key->l2_type = p_ctc_key->l2_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l2_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l2_type = %d\n", p_sys_key->l2_type);
    }

    /* l3 type */
    if (p_ctc_key->flag & CTC_ACLQOS_MAC_KEY_L3TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l3_type, 0xF);
        p_sys_key->flag.l3_type = 1;
        p_sys_key->l3_type = p_ctc_key->l3_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l3_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l3_type = %d\n", p_sys_key->l3_type);
    }

    /* support dual acl lookup */
    if (IS_ACL_TABLE(p_label->type))
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->acl_label = p_label_index->index;
        p_sys_key->acl_label_mask= p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);

        p_sys_key->table_id0 = ACL_MAC_TABLEID0_C;
        p_sys_key->table_id1 = ACL_MAC_TABLEID1_C;
        p_sys_key->table_id2 = ACL_MAC_TABLEID2_C;
        p_sys_key->table_id3 = ACL_MAC_TABLEID3_C;
    }
    else if (IS_QOS_TABLE(p_label->type))
    {
        p_sys_key->flag.qos_label = 1;
        p_sys_key->qos_label = p_label_index->index;
        p_sys_key->qos_label_mask= p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_MAC_TABLEID0_C;
        p_sys_key->table_id1 = QOS_MAC_TABLEID1_C;
        p_sys_key->table_id2 = QOS_MAC_TABLEID2_C;
        p_sys_key->table_id3 = QOS_MAC_TABLEID3_C;
    }
    else if(SYS_SERVICE_ACLQOS_LABEL == p_label->type)
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->flag.qos_label = 1;

        p_sys_key->acl_label = (p_label_index->index>>8)&0xFF;
        p_sys_key->acl_label_mask = (p_label_index->index_mask>>8)&0xFF;
        p_sys_key->qos_label = (p_label_index->index)&0xFF;
        p_sys_key->qos_label_mask = (p_label_index->index_mask)&0xFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_MAC_TABLEID0_C;
        p_sys_key->table_id1 = QOS_MAC_TABLEID1_C;
        p_sys_key->table_id2 = QOS_MAC_TABLEID2_C;
        p_sys_key->table_id3 = QOS_MAC_TABLEID3_C;
    }

    /* If the entry is global entry, it should can also match service label */
    if (IS_QOS_TABLE(p_label->type))
    {
        if (p_label->id >= CTC_GLOBAL_QOS_INGRESS_LABEL_ID_HEAD &&
            p_label->id <= CTC_GLOBAL_QOS_EGRESS_LABEL_ID_TAIL)
        {
            p_sys_key->flag.is_glb_entry = 1;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief map acl/qos ipv4 key from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_ipv4_key(uint8 lchip, sys_aclqos_label_t* p_label,
                                ctc_aclqos_ipv4_key_t* p_ctc_key,
                                sys_aclqos_ipv4_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_IPDA_FLAG)
    {
        p_sys_key->flag.ipda = 1;
        p_sys_key->ip_da = p_ctc_key->ip_da;
        p_sys_key->ip_da_mask = p_ctc_key->ip_da_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da = %d.%d.%d.%d\n",
                                  p_sys_key->ip_da >> 24, (p_sys_key->ip_da >> 16) & 0x00FF,
                                  (p_sys_key->ip_da >> 8) & 0x0000FF, p_sys_key->ip_da & 0x000000FF);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da_mask = %d.%d.%d.%d\n",
                                  p_sys_key->ip_da_mask >> 24, (p_sys_key->ip_da_mask >> 16) & 0x00FF,
                                  (p_sys_key->ip_da_mask >> 8) & 0x0000FF, p_sys_key->ip_da_mask & 0x000000FF);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_IPSA_FLAG)
    {
        p_sys_key->flag.ipsa = 1;
        p_sys_key->ip_sa = p_ctc_key->ip_sa;
        p_sys_key->ip_sa_mask = p_ctc_key->ip_sa_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa = %d.%d.%d.%d\n",
                                  p_sys_key->ip_sa >> 24, (p_sys_key->ip_sa >> 16) & 0x00FF,
                                  (p_sys_key->ip_sa >> 8) & 0x0000FF, p_sys_key->ip_sa & 0x000000FF);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa_mask = %d.%d.%d.%d\n",
                                  p_sys_key->ip_sa_mask >> 24, (p_sys_key->ip_sa_mask >> 16) & 0x00FF,
                                  (p_sys_key->ip_sa_mask >> 8) & 0x0000FF, p_sys_key->ip_sa_mask & 0x000000FF);
    }

    /* layer 4 information */
    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_L4PROTO_FLAG)
    {
        if (1 == p_ctc_key->l4_protocol)        /* ICMP */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped = 1;
            p_sys_key->l4info_mapped_mask = 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);

            /* icmp type */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_ICMPTYPE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port = (p_ctc_key->icmp_type << 8) & 0xFF00;
                p_sys_key->l4_src_port_mask |= 0xFF00;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp type\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }

            /* icmp code */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_ICMPCODE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port |= p_ctc_key->icmp_code & 0x00FF;
                p_sys_key->l4_src_port_mask |= 0x00FF;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp code\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }
        }
        else if (2 == p_ctc_key->l4_protocol)   /* IGMP */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped = 2;
            p_sys_key->l4info_mapped_mask = 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by igmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);

            if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_IGMPTYPE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port = (p_ctc_key->igmp_type << 8) & 0xFF00;
                p_sys_key->l4_src_port_mask |= 0xFF00;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by igmp type\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }
        }
        else if ((6 == p_ctc_key->l4_protocol) || (17 == p_ctc_key->l4_protocol))   /* TCP or UDP */
        {
            if (6 == p_ctc_key->l4_protocol)    /* TCP */
            {
                p_sys_key->flag.is_tcp = 1;
                p_sys_key->is_tcp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);
            }
            else    /* UDP */
            {
                p_sys_key->flag.is_udp = 1;
                p_sys_key->is_udp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
            }

            /* l4 src port when op is equal, other op relies on parser */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_L4SRCPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_src_port.operator)
                {
                    p_sys_key->flag.l4_src_port = 1;
                    p_sys_key->l4_src_port = p_ctc_key->l4_src_port.l4_port_min;
                    p_sys_key->l4_src_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %d\n", p_sys_key->l4_src_port);
                }
            }

            /* l4 dst port when op is equal, other op relies on parser */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_L4DSTPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_dst_port.operator)
                {
                    p_sys_key->flag.l4_dst_port = 1;
                    p_sys_key->l4_dst_port = p_ctc_key->l4_dst_port.l4_port_min;
                    p_sys_key->l4_dst_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_dst_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_dst_port = %d\n", p_sys_key->l4_dst_port);
                }
            }
        }
        else    /* other layer 4 protocol type */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped =  p_ctc_key->l4_protocol;
            p_sys_key->l4info_mapped_mask = 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by other l4 protocol\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
        }
    }

    /* ip fragment */
    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_FRAG_FLAG)
    {
        p_sys_key->flag.frag_info = 1;

        if (CTC_ACLQOS_IP_FRAG_NONE == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 0;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_NON_INITIAL == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_TINY == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 3;
        }
        else if (CTC_ACLQOS_IP_FRAG_LAST == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 3;
            p_sys_key->frag_info_mask = 3;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.frag_info = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info = %d\n", p_sys_key->frag_info);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info_mask = %d\n", p_sys_key->frag_info_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_DSCP_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 0x3F);
        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp;
        p_sys_key->dscp_mask = 0x3F;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_PREC_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 7);

        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp<<3;
        p_sys_key->dscp_mask = 0x38;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_OPTION_FLAG)
    {
        p_sys_key->flag.ip_option = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ip_option = 1\n");
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_ROUTEDPKT_FLAG)
    {
        p_sys_key->flag.routed_packet = 1;
        p_sys_key->routed_packet = p_ctc_key->routed_packet;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.routed_packet = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->routed_packet = %d\n", p_sys_key->routed_packet);
    }

    /* set or not set? */
    p_sys_key->flag.ip_hdr_error = 0;
    p_sys_key->ip_hdr_error = 0;

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_MACDA_FLAG)
    {
        p_sys_key->flag.macda = 1;
        kal_memcpy(p_sys_key->mac_da, p_ctc_key->mac_da, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_da_mask, p_ctc_key->mac_da_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da[0], p_sys_key->mac_da[1], p_sys_key->mac_da[2],
                                  p_sys_key->mac_da[3], p_sys_key->mac_da[4], p_sys_key->mac_da[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da_mask[0], p_sys_key->mac_da_mask[1], p_sys_key->mac_da_mask[2],
                                  p_sys_key->mac_da_mask[3], p_sys_key->mac_da_mask[4], p_sys_key->mac_da_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_MACSA_FLAG)
    {
        p_sys_key->flag.macsa = 1;
        kal_memcpy(p_sys_key->mac_sa, p_ctc_key->mac_sa, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_sa_mask, p_ctc_key->mac_sa_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_sa = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa[0], p_sys_key->mac_sa[1], p_sys_key->mac_sa[2],
                                  p_sys_key->mac_sa[3], p_sys_key->mac_sa[4], p_sys_key->mac_sa[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa_mask[0], p_sys_key->mac_sa_mask[1], p_sys_key->mac_sa_mask[2],
                                  p_sys_key->mac_sa_mask[3], p_sys_key->mac_sa_mask[4], p_sys_key->mac_sa_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->cos, 7);
        p_sys_key->flag.cos = 1;
        p_sys_key->cos = p_ctc_key->cos;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cos = %d\n", p_sys_key->cos);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_CVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->cvlan);
        p_sys_key->flag.cvlan = 1;
        p_sys_key->cvlan = p_ctc_key->cvlan;
        if(TRUE == p_ctc_key->cvlan_mask_valid)
        {
		    p_sys_key->cvlan_mask = p_ctc_key->cvlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->cvlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cvlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan = %d\n", p_sys_key->cvlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan_mask = %d\n", p_sys_key->cvlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_CTAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cos, 0x7);
        p_sys_key->ctag_cos_cfi = p_ctc_key->ctag_cos << 1;
        p_sys_key->flag.ctag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_CTAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cfi, 0x1);
        p_sys_key->flag.ctag_cfi = 1;
        p_sys_key->ctag_cos_cfi += p_ctc_key->ctag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_SVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->svlan);
        p_sys_key->flag.svlan = 1;
        p_sys_key->svlan = p_ctc_key->svlan;
		if(TRUE == p_ctc_key->svlan_mask_valid)
        {
		    p_sys_key->svlan_mask = p_ctc_key->svlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->svlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.svlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan = %d\n", p_sys_key->svlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan_mask = %d\n", p_sys_key->svlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_STAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cos, 0x7);
        p_sys_key->stag_cos_cfi = p_ctc_key->stag_cos << 1;
        p_sys_key->flag.stag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_STAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cfi, 0x1);
        p_sys_key->flag.stag_cfi = 1;
        p_sys_key->stag_cos_cfi += p_ctc_key->stag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_L2TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l2_type, 0xF);
        p_sys_key->flag.l2_type = 1;
        p_sys_key->l2_type = p_ctc_key->l2_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l2_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l2_type = %d\n", p_sys_key->l2_type);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV4_KEY_L3TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l3_type, 0xF);
        p_sys_key->flag.l3_type = 1;
        p_sys_key->l3_type = p_ctc_key->l3_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l3_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l3_type = %d\n", p_sys_key->l3_type);
    }

    /* support dual acl lookup */
    if (IS_ACL_TABLE(p_label->type))
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->acl_label = p_label_index->index;
        p_sys_key->acl_label_mask = p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);

        p_sys_key->table_id0 = ACL_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = ACL_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = ACL_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = ACL_IPV4_TABLEID3_C;
    }
    else if (IS_QOS_TABLE(p_label->type))
    {
        p_sys_key->flag.qos_label = 1;
        p_sys_key->qos_label = p_label_index->index;
        p_sys_key->qos_label_mask = p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV4_TABLEID3_C;
    }
    else if(SYS_SERVICE_ACLQOS_LABEL == p_label->type)
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->flag.qos_label = 1;

        p_sys_key->acl_label = (p_label_index->index>>8)&0xFF;
        p_sys_key->acl_label_mask = (p_label_index->index_mask>>8)&0xFF;
        p_sys_key->qos_label = (p_label_index->index)&0xFF;
        p_sys_key->qos_label_mask = (p_label_index->index_mask)&0xFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV4_TABLEID3_C;
    }

    /* If the entry is global entry, it should can also match service label */
    if (IS_QOS_TABLE(p_label->type))
    {
        if (p_label->id >= CTC_GLOBAL_QOS_INGRESS_LABEL_ID_HEAD &&
            p_label->id <= CTC_GLOBAL_QOS_EGRESS_LABEL_ID_TAIL)
        {
            p_sys_key->flag.is_glb_entry = 1;
        }
    }

    return CTC_E_NONE;
}

/**
 @brief map acl/qos mpls key from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_mpls_key(uint8 lchip, sys_aclqos_label_t* p_label,
                                ctc_aclqos_mpls_key_t* p_ctc_key,
                                sys_aclqos_mpls_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_MACDA_FLAG)
    {
        p_sys_key->flag.macda = 1;
        kal_memcpy(p_sys_key->mac_da, p_ctc_key->mac_da, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_da_mask, p_ctc_key->mac_da_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da[0], p_sys_key->mac_da[1], p_sys_key->mac_da[2],
                                  p_sys_key->mac_da[3], p_sys_key->mac_da[4], p_sys_key->mac_da[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da_mask[0], p_sys_key->mac_da_mask[1], p_sys_key->mac_da_mask[2],
                                  p_sys_key->mac_da_mask[3], p_sys_key->mac_da_mask[4], p_sys_key->mac_da_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_MACSA_FLAG)
    {
        p_sys_key->flag.macsa = 1;
        kal_memcpy(p_sys_key->mac_sa, p_ctc_key->mac_sa, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_sa_mask, p_ctc_key->mac_sa_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_sa = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa[0], p_sys_key->mac_sa[1], p_sys_key->mac_sa[2],
                                  p_sys_key->mac_sa[3], p_sys_key->mac_sa[4], p_sys_key->mac_sa[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa_mask[0], p_sys_key->mac_sa_mask[1], p_sys_key->mac_sa_mask[2],
                                  p_sys_key->mac_sa_mask[3], p_sys_key->mac_sa_mask[4], p_sys_key->mac_sa_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->cos, 7);
        p_sys_key->flag.cos = 1;
        p_sys_key->cos = p_ctc_key->cos;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cos = %d\n", p_sys_key->cos);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_CVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->cvlan);
        p_sys_key->flag.cvlan = 1;
        p_sys_key->cvlan = p_ctc_key->cvlan;
		if(TRUE == p_ctc_key->cvlan_mask_valid)
        {
		    p_sys_key->cvlan_mask = p_ctc_key->cvlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->cvlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cvlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan = %d\n", p_sys_key->cvlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan_mask = %d\n", p_sys_key->cvlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_CTAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cos, 0x7);
        p_sys_key->ctag_cos_cfi = p_ctc_key->ctag_cos << 1;
        p_sys_key->flag.ctag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_CTAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cfi, 0x1);
        p_sys_key->flag.ctag_cfi = 1;
        p_sys_key->ctag_cos_cfi += p_ctc_key->ctag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_SVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->svlan);
        p_sys_key->flag.svlan = 1;
        p_sys_key->svlan = p_ctc_key->svlan;
		if(TRUE == p_ctc_key->svlan_mask_valid)
        {
		    p_sys_key->svlan_mask = p_ctc_key->svlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->svlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.svlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan = %d\n", p_sys_key->svlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan_mask = %d\n", p_sys_key->svlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_STAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cos, 0x7);
        p_sys_key->stag_cos_cfi = p_ctc_key->stag_cos << 1;
        p_sys_key->flag.stag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_STAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cfi, 0x1);
        p_sys_key->flag.stag_cfi = 1;
        p_sys_key->stag_cos_cfi += p_ctc_key->stag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_L2TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l2_type, 0xF);
        p_sys_key->flag.l2_type = 1;
        p_sys_key->l2_type = p_ctc_key->l2_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l2_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l2_type = %d\n", p_sys_key->l2_type);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_ROUTEDPKT_FLAG)
    {
        p_sys_key->flag.routed_packet = 1;
        p_sys_key->routed_packet = p_ctc_key->routed_packet;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.routed_packet = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->routed_packet = %d\n", p_sys_key->routed_packet);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_LABEL0_FLAG)
    {
        p_sys_key->flag.label0 = 1;
        p_sys_key->mpls_label0 = p_ctc_key->mpls_label0;
        p_sys_key->mpls_label0_mask = p_ctc_key->mpls_label0_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.label0 = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label0 = %d\n", p_sys_key->mpls_label0);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label0_mask = %d\n", p_sys_key->mpls_label0_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_LABEL1_FLAG)
    {
        p_sys_key->flag.label1 = 1;
        p_sys_key->mpls_label1 = p_ctc_key->mpls_label1;
        p_sys_key->mpls_label1_mask = p_ctc_key->mpls_label1_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.label1 = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label1 = %d\n", p_sys_key->mpls_label1);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label1_mask = %d\n", p_sys_key->mpls_label1_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_LABEL2_FLAG)
    {
        p_sys_key->flag.label2 = 1;
        p_sys_key->mpls_label2 = p_ctc_key->mpls_label2;
        p_sys_key->mpls_label2_mask = p_ctc_key->mpls_label2_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.label2 = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label2 = %d\n", p_sys_key->mpls_label2);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label2_mask = %d\n", p_sys_key->mpls_label2_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_MPLS_KEY_LABEL3_FLAG)
    {
        p_sys_key->flag.label3 = 1;
        p_sys_key->mpls_label3 = p_ctc_key->mpls_label3;
        p_sys_key->mpls_label3_mask = p_ctc_key->mpls_label3_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.label3 = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label3 = %d\n", p_sys_key->mpls_label3);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mpls_label3_mask = %d\n", p_sys_key->mpls_label3_mask);
    }

    /* support dual acl lookup */
    if (IS_ACL_TABLE(p_label->type))
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->acl_label = p_label_index->index;
        p_sys_key->acl_label_mask= p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);

        p_sys_key->table_id0 = ACL_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = ACL_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = ACL_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = ACL_IPV4_TABLEID3_C;
    }
    else if (IS_QOS_TABLE(p_label->type))
    {
        p_sys_key->flag.qos_label = 1;
        p_sys_key->qos_label = p_label_index->index;
        p_sys_key->qos_label_mask = p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV4_TABLEID3_C;
    }
    else if(SYS_SERVICE_ACLQOS_LABEL == p_label->type)
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->flag.qos_label = 1;

        p_sys_key->acl_label = (p_label_index->index>>8)&0xFF;
        p_sys_key->acl_label_mask = (p_label_index->index_mask>>8)&0xFF;
        p_sys_key->qos_label = (p_label_index->index)&0xFF;
        p_sys_key->qos_label_mask = (p_label_index->index_mask)&0xFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV4_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV4_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV4_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV4_TABLEID3_C;
    }

    return CTC_E_NONE;
}


/**
 @brief map acl/qos ipv6 key from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_ipv6_key(uint8 lchip, sys_aclqos_label_t* p_label,
                                ctc_aclqos_ipv6_key_t* p_ctc_key,
                                sys_aclqos_ipv6_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_IPDA_FLAG)
    {
        p_sys_key->flag.ipda = 1;
        kal_memcpy(p_sys_key->ip_da, p_ctc_key->ip_da, sizeof(ipv6_addr_t));
        kal_memcpy(p_sys_key->ip_da_mask, p_ctc_key->ip_da_mask, sizeof(ipv6_addr_t));
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_da[0], p_sys_key->ip_da[1],
                                  p_sys_key->ip_da[2], p_sys_key->ip_da[3]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da_mask = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_da_mask[0], p_sys_key->ip_da_mask[1],
                                  p_sys_key->ip_da_mask[2], p_sys_key->ip_da_mask[3]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_IPSA_FLAG)
    {
        p_sys_key->flag.ipsa = 1;
        kal_memcpy(p_sys_key->ip_sa, p_ctc_key->ip_sa, sizeof(ipv6_addr_t));
        kal_memcpy(p_sys_key->ip_sa_mask, p_ctc_key->ip_sa_mask, sizeof(ipv6_addr_t));
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_sa[0], p_sys_key->ip_sa[1],
                                  p_sys_key->ip_sa[2], p_sys_key->ip_sa[3]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa_mask = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_sa_mask[0], p_sys_key->ip_sa_mask[1],
                                  p_sys_key->ip_sa_mask[2], p_sys_key->ip_sa_mask[3]);
    }

    /* layer 4 information */
    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_L4PROTO_FLAG)
    {
        if (58 == p_ctc_key->l4_protocol)        /* ICMP */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped = 58;
            p_sys_key->l4info_mapped_mask = 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%4x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%4x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);

            /* icmp type */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_ICMPTYPE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port = (p_ctc_key->icmp_type << 8) & 0xFF00;
                p_sys_key->l4_src_port_mask |= 0xFF00;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp type\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }

            /* icmp code */
            if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_ICMPCODE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port |= p_ctc_key->icmp_code & 0x00FF;
                p_sys_key->l4_src_port_mask |= 0x00FF;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp code\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }
        }
        else if ((6 == p_ctc_key->l4_protocol) || (17 == p_ctc_key->l4_protocol))   /* TCP or UDP */
        {
            if (6 == p_ctc_key->l4_protocol)    /* TCP */
            {
                p_sys_key->flag.is_tcp = 1;
                p_sys_key->is_tcp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);
            }
            else    /* UDP */
            {
                p_sys_key->flag.is_udp = 1;
                p_sys_key->is_udp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
            }

            if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_L4SRCPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_src_port.operator)
                {
                    p_sys_key->flag.l4_src_port = 1;
                    p_sys_key->l4_src_port = p_ctc_key->l4_src_port.l4_port_min;
                    p_sys_key->l4_src_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %d\n", p_sys_key->l4_src_port);
                }
            }

            if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_L4DSTPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_dst_port.operator)
                {
                    p_sys_key->flag.l4_dst_port = 1;
                    p_sys_key->l4_dst_port = p_ctc_key->l4_dst_port.l4_port_min;
                    p_sys_key->l4_dst_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_dst_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_dst_port = %d\n", p_sys_key->l4_dst_port);
                }
            }
        }
        else    /* other layer 4 protocol type */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped = p_ctc_key->l4_protocol;
            p_sys_key->l4info_mapped_mask = 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by other l4 protocol\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%4x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%4x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
        }
    }

    if (p_sys_key->flag.is_tcp && (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_TCPFLAG_FLAG))
    {
        p_sys_key->flag.tcp_flag = 1;
        /* parse tcp flag, rely on parser module */
        /* TBD */
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_EXTHDR_FLAG)
    {
        p_sys_key->flag.ext_hdr = 1;
        p_sys_key->ext_hdr = p_ctc_key->ext_hdr;
        p_sys_key->ext_hdr_mask = p_ctc_key->ext_hdr;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ext_hdr = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ext_hdr = %2x\n", p_sys_key->ext_hdr);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ext_hdr_mask = %2x\n", p_sys_key->ext_hdr_mask);
    }

    /* ip fragment */
    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_FRAG_FLAG)
    {
        p_sys_key->flag.frag_info = 1;

        if (CTC_ACLQOS_IP_FRAG_NONE == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 0;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_NON_INITIAL == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_TINY == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 3;
        }
        else if (CTC_ACLQOS_IP_FRAG_LAST == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 3;
            p_sys_key->frag_info_mask = 3;
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.frag_info = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info = %d\n", p_sys_key->frag_info);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->rag_info_mask = %d\n", p_sys_key->frag_info_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_DSCP_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 0x3F);
        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp;
        p_sys_key->dscp_mask = 0x3F;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_PREC_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 7);

        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp<<3;
        p_sys_key->dscp_mask = 0x38;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_OPTION_FLAG)
    {
        p_sys_key->flag.ip_option = 1;
        p_sys_key->ip_option = p_ctc_key->ip_option;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ip_option = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_option = %d\n", p_sys_key->ip_option);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_ROUTEDPKT_FLAG)
    {
        p_sys_key->flag.routed_packet = 1;
        p_sys_key->routed_packet = p_ctc_key->routed_packet;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.routed_packet = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->routed_packet = %d\n", p_sys_key->routed_packet);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_FLOWLABEL_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->flow_label, 0xFFFFF);
        p_sys_key->flag.flow_label = 1;
        p_sys_key->flow_label = p_ctc_key->flow_label;
        p_sys_key->flow_label_mask = 0xFFFFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.flow_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flow_label = %5x\n", p_sys_key->flow_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flow_label_mask = %5x\n", p_sys_key->flow_label_mask);
    }

    /* set or not set? */
    p_sys_key->flag.ip_hdr_error = 0;
    p_sys_key->ip_hdr_error = 0;

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_MACDA_FLAG)
    {
        p_sys_key->flag.macda = 1;
        kal_memcpy(p_sys_key->mac_da, p_ctc_key->mac_da, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_da_mask, p_ctc_key->mac_da_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da[0], p_sys_key->mac_da[1], p_sys_key->mac_da[2],
                                  p_sys_key->mac_da[3], p_sys_key->mac_da[4], p_sys_key->mac_da[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_da_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_da_mask[0], p_sys_key->mac_da_mask[1], p_sys_key->mac_da_mask[2],
                                  p_sys_key->mac_da_mask[3], p_sys_key->mac_da_mask[4], p_sys_key->mac_da_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_MACSA_FLAG)
    {
        p_sys_key->flag.macsa = 1;
        kal_memcpy(p_sys_key->mac_sa, p_ctc_key->mac_sa, CTC_ETH_ADDR_LEN);
        kal_memcpy(p_sys_key->mac_sa_mask, p_ctc_key->mac_sa_mask, CTC_ETH_ADDR_LEN);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.macsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_sa = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa[0], p_sys_key->mac_sa[1], p_sys_key->mac_sa[2],
                                  p_sys_key->mac_sa[3], p_sys_key->mac_sa[4], p_sys_key->mac_sa[5]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->mac_sa_mask = %2x%2x:%2x%2x:%2x%2x\n",
                                  p_sys_key->mac_sa_mask[0], p_sys_key->mac_sa_mask[1], p_sys_key->mac_sa_mask[2],
                                  p_sys_key->mac_sa_mask[3], p_sys_key->mac_sa_mask[4], p_sys_key->mac_sa_mask[5]);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->cos, 7);
        p_sys_key->flag.cos = 1;
        p_sys_key->cos = p_ctc_key->cos;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cos = %d\n", p_sys_key->cos);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_CVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->cvlan);
        p_sys_key->flag.cvlan = 1;
        p_sys_key->cvlan = p_ctc_key->cvlan;
		if(TRUE == p_ctc_key->cvlan_mask_valid)
        {
		    p_sys_key->cvlan_mask = p_ctc_key->cvlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->cvlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.cvlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan = %d\n", p_sys_key->cvlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->cvlan_mask = %d\n", p_sys_key->cvlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_CTAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cos, 0x7);
        p_sys_key->ctag_cos_cfi = p_ctc_key->ctag_cos << 1;
        p_sys_key->flag.ctag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_CTAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->ctag_cfi, 0x1);
        p_sys_key->flag.ctag_cfi = 1;
        p_sys_key->ctag_cos_cfi += p_ctc_key->ctag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ctag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ctag_cos_cfi = %d\n", p_sys_key->ctag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_SVLAN_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(p_ctc_key->svlan);
        p_sys_key->flag.svlan = 1;
        p_sys_key->svlan = p_ctc_key->svlan;
		if(TRUE == p_ctc_key->svlan_mask_valid)
        {
		    p_sys_key->svlan_mask = p_ctc_key->svlan_mask & 0xFFF;
        }
        else
        {
            p_sys_key->svlan_mask = 0xFFF;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.svlan = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan = %d\n", p_sys_key->svlan);
		SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->svlan_mask = %d\n", p_sys_key->svlan_mask);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_STAG_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cos, 0x7);
        p_sys_key->stag_cos_cfi = p_ctc_key->stag_cos << 1;
        p_sys_key->flag.stag_cos = 1;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cos = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_STAG_CFI_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->stag_cfi, 0x1);
        p_sys_key->flag.stag_cfi = 1;
        p_sys_key->stag_cos_cfi += p_ctc_key->stag_cfi;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.stag_cfi = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->stag_cos_cfi = %d\n", p_sys_key->stag_cos_cfi);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_ETHTYPE_FLAG)
    {
        p_sys_key->flag.eth_type = 1;
        p_sys_key->eth_type = p_ctc_key->eth_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.eth_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->eth_type = %d\n", p_sys_key->eth_type);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_L2TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l2_type, 0xF);
        p_sys_key->flag.l2_type = 1;
        p_sys_key->l2_type = p_ctc_key->l2_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l2_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l2_type = %d\n", p_sys_key->l2_type);
    }

    if (p_ctc_key->flag & CTC_ACLQOS_IPV6_KEY_L3TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->l3_type, 0xF);
        p_sys_key->flag.l3_type = 1;
        p_sys_key->l3_type = p_ctc_key->l3_type;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l3_type = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l3_type = %d\n", p_sys_key->l3_type);
    }

    /* support dual acl lookup */
    if (IS_ACL_TABLE(p_label->type))
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->acl_label = p_label_index->index;
        p_sys_key->acl_label_mask = p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);

        p_sys_key->table_id0 = ACL_IPV6_TABLEID0_C;
        p_sys_key->table_id1 = ACL_IPV6_TABLEID1_C;
        p_sys_key->table_id2 = ACL_IPV6_TABLEID2_C;
        p_sys_key->table_id3 = ACL_IPV6_TABLEID3_C;
        p_sys_key->table_id4 = ACL_IPV6_TABLEID4_C;
        p_sys_key->table_id5 = ACL_IPV6_TABLEID5_C;
        p_sys_key->table_id6 = ACL_IPV6_TABLEID6_C;
        p_sys_key->table_id7 = ACL_IPV6_TABLEID7_C;
    }
    else if (IS_QOS_TABLE(p_label->type))
    {
        p_sys_key->flag.qos_label = 1;
        p_sys_key->qos_label = p_label_index->index;
        p_sys_key->qos_label_mask = p_label_index->index_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV6_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV6_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV6_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV6_TABLEID3_C;
        p_sys_key->table_id4 = QOS_IPV6_TABLEID4_C;
        p_sys_key->table_id5 = QOS_IPV6_TABLEID5_C;
        p_sys_key->table_id6 = QOS_IPV6_TABLEID6_C;
        p_sys_key->table_id7 = QOS_IPV6_TABLEID7_C;
    }
    else if(SYS_SERVICE_ACLQOS_LABEL == p_label->type)
    {
        p_sys_key->flag.acl_label = 1;
        p_sys_key->flag.qos_label = 1;

        p_sys_key->acl_label = (p_label_index->index>>8)&0xFF;
        p_sys_key->acl_label_mask = (p_label_index->index_mask>>8)&0xFF;
        p_sys_key->qos_label = (p_label_index->index)&0xFF;
        p_sys_key->qos_label_mask = (p_label_index->index_mask)&0xFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.acl_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.qos_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->acl_label = %d\n", p_sys_key->acl_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->qos_label = %d\n", p_sys_key->qos_label);

        p_sys_key->table_id0 = QOS_IPV6_TABLEID0_C;
        p_sys_key->table_id1 = QOS_IPV6_TABLEID1_C;
        p_sys_key->table_id2 = QOS_IPV6_TABLEID2_C;
        p_sys_key->table_id3 = QOS_IPV6_TABLEID3_C;
        p_sys_key->table_id4 = QOS_IPV6_TABLEID4_C;
        p_sys_key->table_id5 = QOS_IPV6_TABLEID5_C;
        p_sys_key->table_id6 = QOS_IPV6_TABLEID6_C;
        p_sys_key->table_id7 = QOS_IPV6_TABLEID7_C;
    }

    return CTC_E_NONE;
}


/**
 @brief map acl pbr ipv4 key from ctc layer to sys layer
*/
static int32
_sys_humber_acl_map_pbr_ipv4_key(uint8 lchip, sys_aclqos_label_t* p_label,
                                ctc_acl_pbr_ipv4_key_t* p_ctc_key,
                                sys_acl_pbr_ipv4_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_IPDA_FLAG)
    {
        p_sys_key->flag.ipda = 1;
        p_sys_key->ip_da = p_ctc_key->ip_da;
        p_sys_key->ip_da_mask = p_ctc_key->ip_da_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da = %d.%d.%d.%d\n",
                                  p_sys_key->ip_da >> 24, (p_sys_key->ip_da >> 16) & 0x00FF,
                                  (p_sys_key->ip_da >> 8) & 0x0000FF, p_sys_key->ip_da & 0x000000FF);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da_mask = %d.%d.%d.%d\n",
                                  p_sys_key->ip_da_mask >> 24, (p_sys_key->ip_da_mask >> 16) & 0x00FF,
                                  (p_sys_key->ip_da_mask >> 8) & 0x0000FF, p_sys_key->ip_da_mask & 0x000000FF);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_IPSA_FLAG)
    {
        p_sys_key->flag.ipsa = 1;
        p_sys_key->ip_sa = p_ctc_key->ip_sa;
        p_sys_key->ip_sa_mask = p_ctc_key->ip_sa_mask;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa = %d.%d.%d.%d\n",
                                  p_sys_key->ip_sa >> 24, (p_sys_key->ip_sa >> 16) & 0x00FF,
                                  (p_sys_key->ip_sa >> 8) & 0x0000FF, p_sys_key->ip_sa & 0x000000FF);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa_mask = %d.%d.%d.%d\n",
                                  p_sys_key->ip_sa_mask >> 24, (p_sys_key->ip_sa_mask >> 16) & 0x00FF,
                                  (p_sys_key->ip_sa_mask >> 8) & 0x0000FF, p_sys_key->ip_sa_mask & 0x000000FF);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_VRFID_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->vrf_id, CTC_MAX_IPV4_VRFID);
        p_sys_key->flag.vrf_id = 1;
        p_sys_key->vrf_id = p_ctc_key->vrf_id;
        p_sys_key->vrf_id_mask = 0xFFFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.vrf_id = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->vrf_id = 0x%x\n", p_sys_key->vrf_id);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->vrf_id_mask = 0x%x\n", p_sys_key->vrf_id_mask);

    }

    /* layer 4 information */
    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_L4PROTO_FLAG)
    {
        if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_LENGTH_FLAG)
        {
            CTC_MAX_VALUE_CHECK(p_ctc_key->len_index, SYS_PAS_MAX_LAYER4_LEN_OP_INDEX);
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |= p_ctc_key->len_index << 12;
            p_sys_key->l4info_mapped_mask |= 0xF000;

            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);
        }

        if (1 == p_ctc_key->l4_protocol)        /* ICMP */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |= 1;
            p_sys_key->l4info_mapped_mask |= 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);

            /* icmp type */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_ICMPTYPE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port = (p_ctc_key->icmp_type << 8) & 0xFF00;
                p_sys_key->l4_src_port_mask |= 0xFF00;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp type\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }

            /* icmp code */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_ICMPCODE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port |= p_ctc_key->icmp_code & 0x00FF;
                p_sys_key->l4_src_port_mask |= 0x00FF;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp code\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }
        }
        else if ((6 == p_ctc_key->l4_protocol) || (17 == p_ctc_key->l4_protocol))   /* TCP or UDP */
        {
            if (6 == p_ctc_key->l4_protocol)    /* TCP */
            {
                p_sys_key->flag.is_tcp = 1;
                p_sys_key->is_tcp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);
            }
            else    /* UDP */
            {
                p_sys_key->flag.is_udp = 1;
                p_sys_key->is_udp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
            }

            /* l4 src port when op is equal, other op relies on parser */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_L4SRCPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_src_port.operator)
                {
                    p_sys_key->flag.l4_src_port = 1;
                    p_sys_key->l4_src_port = p_ctc_key->l4_src_port.l4_port_min;
                    p_sys_key->l4_src_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %d\n", p_sys_key->l4_src_port);
                }
            }

            /* l4 dst port when op is equal, other op relies on parser */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_L4DSTPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_dst_port.operator)
                {
                    p_sys_key->flag.l4_dst_port = 1;
                    p_sys_key->l4_dst_port = p_ctc_key->l4_dst_port.l4_port_min;
                    p_sys_key->l4_dst_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_dst_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_dst_port = %d\n", p_sys_key->l4_dst_port);
                }
            }
        }
        else    /* other layer 4 protocol type */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |=  p_ctc_key->l4_protocol;
            p_sys_key->l4info_mapped_mask |= 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by other l4 protocol\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
        }
    }

    /* ip fragment */
    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_FRAG_FLAG)
    {
        p_sys_key->flag.frag_info = 1;

        if (CTC_ACLQOS_IP_FRAG_NONE == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 0;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_NON_INITIAL == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_TINY == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 3;
        }
        else if (CTC_ACLQOS_IP_FRAG_LAST == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 3;
            p_sys_key->frag_info_mask = 3;
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.frag_info = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info = %d\n", p_sys_key->frag_info);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info_mask = %d\n", p_sys_key->frag_info_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_DSCP_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 0x3F);
        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp;
        p_sys_key->dscp_mask = 0x3F;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV4_KEY_PREC_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 7);

        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp<<3;
        p_sys_key->dscp_mask = 0x38;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    /* pbr label */
    p_sys_key->pbr_label = p_label_index->index;
    p_sys_key->pbr_label_mask = p_label_index->index_mask;
    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->pbr_label = %d\n", p_sys_key->pbr_label);

    p_sys_key->table_id0 = PBR_IPV4_TABLEID0_C;
    p_sys_key->table_id1 = PBR_IPV4_TABLEID1_C;

    return CTC_E_NONE;
}


/**
 @brief map acl/qos ipv6 key from ctc layer to sys layer
*/
static int32
_sys_humber_acl_map_pbr_ipv6_key(uint8 lchip, sys_aclqos_label_t* p_label,
                                ctc_acl_pbr_ipv6_key_t* p_ctc_key,
                                sys_acl_pbr_ipv6_key_t* p_sys_key)
{
    sys_aclqos_label_index_t* p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_IPDA_FLAG)
    {
        p_sys_key->flag.ipda = 1;
        kal_memcpy(p_sys_key->ip_da, p_ctc_key->ip_da, sizeof(ipv6_addr_t));
        kal_memcpy(p_sys_key->ip_da_mask, p_ctc_key->ip_da_mask, sizeof(ipv6_addr_t));
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipda = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_da[0], p_sys_key->ip_da[1],
                                  p_sys_key->ip_da[2], p_sys_key->ip_da[3]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_da_mask = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_da_mask[0], p_sys_key->ip_da_mask[1],
                                  p_sys_key->ip_da_mask[2], p_sys_key->ip_da_mask[3]);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_IPSA_FLAG)
    {
        p_sys_key->flag.ipsa = 1;
        kal_memcpy(p_sys_key->ip_sa, p_ctc_key->ip_sa, sizeof(ipv6_addr_t));
        kal_memcpy(p_sys_key->ip_sa_mask, p_ctc_key->ip_sa_mask, sizeof(ipv6_addr_t));
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ipsa = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_sa[0], p_sys_key->ip_sa[1],
                                  p_sys_key->ip_sa[2], p_sys_key->ip_sa[3]);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ip_sa_mask = %8x:%8x:%8x:%8x\n",
                                  p_sys_key->ip_sa_mask[0], p_sys_key->ip_sa_mask[1],
                                  p_sys_key->ip_sa_mask[2], p_sys_key->ip_sa_mask[3]);
    }

    /* layer 4 information */
    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_L4PROTO_FLAG)
    {
        if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_LENGTH_FLAG)
        {
            CTC_MAX_VALUE_CHECK(p_ctc_key->len_index, SYS_PAS_MAX_LAYER4_LEN_OP_INDEX);
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |= p_ctc_key->len_index << 12;
            p_sys_key->l4info_mapped_mask |= 0xF000;

            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%x\n", p_sys_key->l4info_mapped_mask);
        }

        if (58 == p_ctc_key->l4_protocol)        /* ICMP */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |= 58;
            p_sys_key->l4info_mapped_mask |= 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by icmp\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%4x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%4x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);

            /* icmp type */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_ICMPTYPE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port = (p_ctc_key->icmp_type << 8) & 0xFF00;
                p_sys_key->l4_src_port_mask |= 0xFF00;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp type\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }

            /* icmp code */
            if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_ICMPCODE_FLAG)
            {
                p_sys_key->flag.l4_src_port = 1;
                p_sys_key->l4_src_port |= p_ctc_key->icmp_code & 0x00FF;
                p_sys_key->l4_src_port_mask |= 0x00FF;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, set by icmp code\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %4x\n", p_sys_key->l4_src_port);
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port_mask = %4x\n", p_sys_key->l4_src_port_mask);
            }
        }
        else if ((6 == p_ctc_key->l4_protocol) || (17 == p_ctc_key->l4_protocol))   /* TCP or UDP */
        {
            if (6 == p_ctc_key->l4_protocol)    /* TCP */
            {
                p_sys_key->flag.is_tcp = 1;
                p_sys_key->is_tcp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);
            }
            else    /* UDP */
            {
                p_sys_key->flag.is_udp = 1;
                p_sys_key->is_udp = 1;
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
                SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
            }

            if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_L4SRCPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_src_port.operator)
                {
                    p_sys_key->flag.l4_src_port = 1;
                    p_sys_key->l4_src_port = p_ctc_key->l4_src_port.l4_port_min;
                    p_sys_key->l4_src_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_src_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_src_port = %d\n", p_sys_key->l4_src_port);
                }
            }

            if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_L4DSTPORT_FLAG)
            {
                if (CTC_ACLQOS_L4PORT_OPERATOR_EQ == p_ctc_key->l4_dst_port.operator)
                {
                    p_sys_key->flag.l4_dst_port = 1;
                    p_sys_key->l4_dst_port = p_ctc_key->l4_dst_port.l4_port_min;
                    p_sys_key->l4_dst_port_mask = 0xFFFF;
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4_dst_port = 1, operator is equal\n");
                    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4_dst_port = %d\n", p_sys_key->l4_dst_port);
                }
            }
        }
        else    /* other layer 4 protocol type */
        {
            p_sys_key->flag.l4info_mapped = 1;
            p_sys_key->l4info_mapped |= p_ctc_key->l4_protocol;
            p_sys_key->l4info_mapped_mask |= 0x00FF;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.l4info_mapped = 1, set by other l4 protocol\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped = 0x%4x\n", p_sys_key->l4info_mapped);
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->l4info_mapped_mask = 0x%4x\n", p_sys_key->l4info_mapped_mask);

            p_sys_key->flag.is_tcp = 1;
            p_sys_key->is_tcp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_tcp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_tcp = %d\n", p_sys_key->is_tcp);

            p_sys_key->flag.is_udp = 1;
            p_sys_key->is_udp = 0;
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.is_udp = 1\n");
            SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->is_udp = %d\n", p_sys_key->is_udp);
        }
    }

    if (p_sys_key->flag.is_tcp && (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_TCPFLAG_FLAG))
    {
        p_sys_key->flag.tcp_flag = 1;
        /* parse tcp flag, rely on parser module */
        /* TBD */
    }

    /* ip fragment */
    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_FRAG_FLAG)
    {
        p_sys_key->flag.frag_info = 1;

        if (CTC_ACLQOS_IP_FRAG_NONE == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 0;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_NON_INITIAL == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 2;
        }
        else if (CTC_ACLQOS_IP_FRAG_TINY == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 2;
            p_sys_key->frag_info_mask = 3;
        }
        else if (CTC_ACLQOS_IP_FRAG_LAST == p_ctc_key->ip_frag)
        {
            p_sys_key->frag_info = 3;
            p_sys_key->frag_info_mask = 3;
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.frag_info = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info = %d\n", p_sys_key->frag_info);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->frag_info_mask = %d\n", p_sys_key->frag_info_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_VRFID_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->vrf_id, CTC_MAX_IPV6_VRFID);
        p_sys_key->flag.vrf_id = 1;
        p_sys_key->vrf_id = p_ctc_key->vrf_id;
        p_sys_key->vrf_id_mask = 0xFFFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.vrf_id = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->vrf_id = 0x%x\n", p_sys_key->vrf_id);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->vrf_id_mask = 0x%x\n", p_sys_key->vrf_id_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_DSCP_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->dscp, 0x3F);
        p_sys_key->flag.dscp = 1;
        p_sys_key->dscp = p_ctc_key->dscp;
        p_sys_key->dscp_mask = 0x3F;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.dscp = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp = %d\n", p_sys_key->dscp);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->dscp_mask = %d\n", p_sys_key->dscp_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_EXTHDR_FLAG)
    {
        p_sys_key->flag.ext_hdr = 1;
        p_sys_key->ext_hdr = p_ctc_key->ext_hdr;
        p_sys_key->ext_hdr_mask = p_ctc_key->ext_hdr;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.ext_hdr = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ext_hdr = %2x\n", p_sys_key->ext_hdr);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->ext_hdr_mask = %2x\n", p_sys_key->ext_hdr_mask);
    }

    if (p_ctc_key->flag & CTC_ACL_PBR_IPV6_KEY_FLOWLABEL_FLAG)
    {
        CTC_MAX_VALUE_CHECK(p_ctc_key->flow_label, 0xFFFFF);
        p_sys_key->flag.flow_label = 1;
        p_sys_key->flow_label = p_ctc_key->flow_label;
        p_sys_key->flow_label_mask = 0xFFFFF;
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flag.flow_label = 1\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flow_label = %5x\n", p_sys_key->flow_label);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->flow_label_mask = %5x\n", p_sys_key->flow_label_mask);
    }

    /* pbr label */
    p_sys_key->pbr_label = p_label_index->index;
    p_sys_key->pbr_label_mask = p_label_index->index_mask;
    SYS_ACLQOS_ENTRY_DBG_INFO("p_sys_key->pbr_label = %d\n", p_sys_key->pbr_label);

    p_sys_key->table_id0 = PBR_IPV6_TABLEID0_C;
    p_sys_key->table_id1 = PBR_IPV6_TABLEID1_C;
    p_sys_key->table_id2 = PBR_IPV6_TABLEID2_C;
    p_sys_key->table_id3 = PBR_IPV6_TABLEID3_C;
    p_sys_key->table_id4 = PBR_IPV6_TABLEID4_C;
    p_sys_key->table_id5 = PBR_IPV6_TABLEID5_C;
    p_sys_key->table_id6 = PBR_IPV6_TABLEID6_C;
    p_sys_key->table_id7 = PBR_IPV6_TABLEID7_C;

    return CTC_E_NONE;
}


/**
 @brief map acl/qos key from ctc layer to sys layer
*/
static int32
_sys_humber_aclqos_map_key(uint8 lchip, sys_aclqos_label_t *p_label,
                           ctc_aclqos_key_t *p_ctc_key,
                           sys_aclqos_key_t *p_sys_key)
{
    sys_aclqos_label_index_t *p_label_index;

    CTC_PTR_VALID_CHECK(p_label);
    CTC_PTR_VALID_CHECK(p_ctc_key);
    CTC_PTR_VALID_CHECK(p_sys_key);

    p_label_index = p_label->p_index[lchip];
    CTC_PTR_VALID_CHECK(p_label_index);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    p_sys_key->type = p_ctc_key->type;

    if(SYS_PBR_ACL_LABEL != p_label->type)
    {
        switch(p_sys_key->type)
        {
            case CTC_ACLQOS_MAC_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_aclqos_map_mac_key(lchip, p_label, &p_ctc_key->key_info.mac_key, &p_sys_key->key_info.mac_key));
                break;

            case CTC_ACLQOS_IPV4_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_aclqos_map_ipv4_key(lchip, p_label, &p_ctc_key->key_info.ipv4_key, &p_sys_key->key_info.ipv4_key));
                break;

            case CTC_ACLQOS_MPLS_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_aclqos_map_mpls_key(lchip, p_label, &p_ctc_key->key_info.mpls_key, &p_sys_key->key_info.mpls_key));
                break;

            case CTC_ACLQOS_IPV6_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_aclqos_map_ipv6_key(lchip, p_label, &p_ctc_key->key_info.ipv6_key, &p_sys_key->key_info.ipv6_key));
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else
    {
        switch(p_sys_key->type)
        {
            case CTC_ACL_PBR_IPV4_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_acl_map_pbr_ipv4_key(lchip, p_label, &p_ctc_key->key_info.pbr_ipv4_key, &p_sys_key->key_info.pbr_ipv4_key));
                break;

            case CTC_ACL_PBR_IPV6_KEY:
                CTC_ERROR_RETURN(
                    _sys_humber_acl_map_pbr_ipv6_key(lchip, p_label, &p_ctc_key->key_info.pbr_ipv6_key, &p_sys_key->key_info.pbr_ipv6_key));
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos action
*/
static int32
_sys_humber_aclqos_build_ds_action(uint8 lchip, sys_aclqos_action_t* p_sys_action, ds_mac_acl_t* p_ds_action)
{
    uint32 policer_ptr;

    CTC_PTR_VALID_CHECK(p_sys_action);
    CTC_PTR_VALID_CHECK(p_ds_action);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    p_ds_action->discard_packet = p_sys_action->flag.discard;
    p_ds_action->deny_learning = p_sys_action->flag.deny_learning;
    p_ds_action->deny_bridge = p_sys_action->flag.deny_bridge;
    p_ds_action->deny_route = p_sys_action->flag.deny_route;
    p_ds_action->deny_replace_cos = p_sys_action->flag.deny_replace_cos;
    p_ds_action->deny_replace_dscp = p_sys_action->flag.deny_replace_dscp;
    p_ds_action->stats_mode = p_sys_action->flag.stats_mode;

    if(p_sys_action->flag.stats)
    {
        p_ds_action->stats_ptr = p_sys_action->stats_or_flowid.stats_ptr;
    }
    else
    {
        p_ds_action->stats_ptr = SPECIAL_STATS_PTR;
    }

    if (p_sys_action->flag.flow_id)
    {
        p_ds_action->stats_ptr = (p_sys_action->stats_or_flowid.flow_id & 0xFF) | 0xFF00;
    }

    if (p_sys_action->flag.flow_policer)
    {
        CTC_ERROR_RETURN(sys_humber_qos_policer_index_get(lchip, p_sys_action->policer_id, &policer_ptr));
        p_ds_action->flow_policer_ptr = policer_ptr & 0xFFFF;
    }
    else
    {
        p_ds_action->flow_policer_ptr = SPECIAL_POLICER_PTR;
    }

    if (p_sys_action->flag.random_log)
    {
        p_ds_action->random_log_en = 1;
        p_ds_action->random_threshold_shift = p_sys_action->random_threshold_shift;
        p_ds_action->acl_log_id = p_sys_action->acl_log_id;
    }

    if (p_sys_action->flag.priority)
    {
        p_ds_action->priority_valid = 1;
        p_ds_action->priority = p_sys_action->priority;
        p_ds_action->color = p_sys_action->color;
    }

    if (p_sys_action->flag.trust)
    {
        p_ds_action->qos_policy = p_sys_action->trust;
    }
    else
    {
        p_ds_action->qos_policy = 7;
    }

    if (p_sys_action->flag.fwd)
    {
        p_ds_action->fwd_ptr = p_sys_action->ds_fwd_ptr;
    }
    else
    {
        p_ds_action->fwd_ptr = 0xFFF;
    }

    return CTC_E_NONE;
}


/**
 @brief build ds acl pbr action
*/
static int32
_sys_humber_acl_pbr_build_ds_action(uint8 lchip, sys_aclqos_action_t* p_sys_action, ds_ipv4_ucast_pbr_dual_da_t* p_ds_action)
{

    CTC_PTR_VALID_CHECK(p_sys_action);
    CTC_PTR_VALID_CHECK(p_ds_action);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(p_sys_action->flag.pbr_ttl_check)
    {
        p_ds_action->ttl_check_en = 1;
    }

    if(p_sys_action->flag.pbr_icmp_check)
    {
        p_ds_action->icmp_check_en = 1;
    }

    if(p_sys_action->flag.pbr_copy_to_cpu)
    {
        p_ds_action->ip_da_exception_en = 1;
        p_ds_action->excep_sub_index = 15; /*SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU must be equal to 15,refer to sys_humber_pdu.h :SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU*/
    }

    if(p_sys_action->flag.pbr_deny)
    {
        p_ds_action->deny_pbr = 1;
    }

    p_ds_action->equal_cost_path_num = p_sys_action->pbr_ecpn & 0x3;
    p_ds_action->equal_cost_path_num2 = p_sys_action->pbr_ecpn >> 2;

    p_ds_action->vrf_id = p_sys_action->pbr_vrfid & 0xfff;
    p_ds_action->ds_fwd_ptr = p_sys_action->pbr_fwd_ptr;

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos mac key
*/
static int32
_sys_humber_aclqos_build_ds_mac_key(sys_aclqos_mac_key_t* p_sys_key,
                                    ds_acl_mac_key_t* p_ds_key,
                                    ds_acl_mac_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* mac da */
    if (p_sys_key->flag.macda)
    {
        p_ds_key->mac_dah = (p_sys_key->mac_da[0] << 8) | (p_sys_key->mac_da[1]);
        p_ds_key->mac_dal = (p_sys_key->mac_da[2] << 24) | (p_sys_key->mac_da[3] << 16) |
                                                  (p_sys_key->mac_da[4] << 8) | (p_sys_key->mac_da[5]);

        p_ds_mask->mac_dah = (p_sys_key->mac_da_mask[0] << 8) | (p_sys_key->mac_da_mask[1]);
        p_ds_mask->mac_dal = (p_sys_key->mac_da_mask[2] << 24) | (p_sys_key->mac_da_mask[3] << 16) |
                                                     (p_sys_key->mac_da_mask[4] << 8) | (p_sys_key->mac_da_mask[5]);
    }

    /* mac sa */
    if (p_sys_key->flag.macsa)
    {
        p_ds_key->mac_sah = (p_sys_key->mac_sa[0] << 8) | (p_sys_key->mac_sa[1]);
        p_ds_key->mac_sal = (p_sys_key->mac_sa[2] << 24) | (p_sys_key->mac_sa[3] << 16) |
                                                  (p_sys_key->mac_sa[4] << 8) | (p_sys_key->mac_sa[5]);

        p_ds_mask->mac_sah = (p_sys_key->mac_sa_mask[0] << 8) | (p_sys_key->mac_sa_mask[1]);
        p_ds_mask->mac_sal = (p_sys_key->mac_sa_mask[2] << 24) | (p_sys_key->mac_sa_mask[3] << 16) |
                                                     (p_sys_key->mac_sa_mask[4] << 8) | (p_sys_key->mac_sa_mask[5]);
    }

    /* vlan ptr */
    if (p_sys_key->flag.vlan_ptr)
    {
        p_ds_key->vlan_ptr = p_sys_key->vlan_ptr;
        p_ds_mask->vlan_ptr = 0x3FFF;
    }

    /* cos */
    if (p_sys_key->flag.cos)
    {
        p_ds_key->cos = p_sys_key->cos;
        p_ds_mask->cos = 0x7;
    }

    /* cvlan id */
    if (p_sys_key->flag.cvlan)
    {
        p_ds_key->cvlan_id11to5 = p_sys_key->cvlan >> 5;
        p_ds_key->cvlan_id4to0 = p_sys_key->cvlan & 0x1F;

        p_ds_mask->cvlan_id11to5 = p_sys_key->cvlan_mask >> 5;
        p_ds_mask->cvlan_id4to0 = p_sys_key->cvlan_mask & 0x1F;
    }

    /* ctag cos */
    if (p_sys_key->flag.ctag_cos)
    {
        p_ds_key->ctag_cos = p_sys_key->ctag_cos_cfi >> 1;
        p_ds_mask->ctag_cos = 0x7;
    }

    /* ctag cfi */
    if (p_sys_key->flag.ctag_cfi)
    {
        p_ds_key->ctag_cfi = p_sys_key->ctag_cos_cfi & 0x1;
        p_ds_mask->ctag_cfi = 0x1;
    }

    /* svlan id */
    if (p_sys_key->flag.svlan)
    {
        p_ds_key->svlan_id = p_sys_key->svlan;
        p_ds_mask->svlan_id = p_sys_key->svlan_mask;
    }

    /* stag cos */
    if (p_sys_key->flag.stag_cos)
    {
        p_ds_key->stag_cos = p_sys_key->stag_cos_cfi >> 1;
        p_ds_mask->stag_cos = 0x7;
    }

    /* stag cfi */
    if (p_sys_key->flag.stag_cfi)
    {
        p_ds_key->stag_cfi = p_sys_key->stag_cos_cfi & 0x1;
        p_ds_mask->stag_cfi = 0x1;
    }

    /* ethernet type */
    if (p_sys_key->flag.eth_type)
    {
        p_ds_key->ether_type = p_sys_key->eth_type;
        p_ds_mask->ether_type = p_sys_key->eth_type_mask;
    }

    /* layer 2 type */
    if (p_sys_key->flag.l2_type)
    {
        p_ds_key->layer2_type = p_sys_key->l2_type;
        p_ds_mask->layer2_type = 0xF;
    }

    /* layer 3 type */
    if (p_sys_key->flag.l3_type)
    {
        p_ds_key->layer3_type = p_sys_key->l3_type;
        p_ds_mask->layer3_type = 0xF;
    }

    /* acl label */
    if (p_sys_key->flag.acl_label)
    {
        p_ds_key->acl_labelh = p_sys_key->acl_label >> 4;
        p_ds_key->acl_labell = p_sys_key->acl_label & 0xF;

        p_ds_mask->acl_labelh = p_sys_key->acl_label_mask >> 4;
        p_ds_mask->acl_labell = p_sys_key->acl_label_mask & 0xF;
    }

    /* qos label */
    if (p_sys_key->flag.qos_label)
    {
        p_ds_key->qos_label = p_sys_key->qos_label;
        p_ds_mask->qos_label = p_sys_key->qos_label_mask;
    }

    /* l2 qos label */
    if (p_sys_key->flag.l2_qos_label)
    {
        p_ds_key->l2_qos_label = p_sys_key->l2_qos_label;
        p_ds_mask->l2_qos_label = p_sys_key->l2_qos_label_mask;
    }

    /* l3 qos label */
    if (p_sys_key->flag.l3_qos_label)
    {
        p_ds_key->l3_qos_label = p_sys_key->l3_qos_label;
        p_ds_mask->l3_qos_label = p_sys_key->l3_qos_label_mask;
    }

    /*service label*/
    if((p_sys_key->flag.acl_label)&&(p_sys_key->flag.qos_label))
    {
        p_ds_key->is_label = 0;
        p_ds_mask->is_label = 1;
    }
    else
    {
        p_ds_key->is_label = 1;
        p_ds_mask->is_label = 1;
    }
    /* If the entry is global entry, it should can also match service label */
    if (p_sys_key->flag.is_glb_entry)
    {
        p_ds_mask->is_label = 0;
    }

    p_ds_key->is_ip_key = 0;
    p_ds_mask->is_ip_key = 1;

    p_ds_key->tableid0 = p_sys_key->table_id0;
    p_ds_key->tableid1 = p_sys_key->table_id1;
    p_ds_key->tableid2 = p_sys_key->table_id2;
    p_ds_key->tableid3 = p_sys_key->table_id3;

    p_ds_mask->tableid0 = 0xF;
    p_ds_mask->tableid1 = 0xF;
    p_ds_mask->tableid2 = 0xF;
    p_ds_mask->tableid3 = 0xF;

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos ipv4 key
*/
static int32
_sys_humber_aclqos_build_ds_ipv4_key(sys_aclqos_ipv4_key_t* p_sys_key,
                                     sys_aclqos_sub_entry_info_t* p_info,
                                     ds_acl_ipv4_key_t* p_ds_key,
                                     ds_acl_ipv4_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_info);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* ip da */
    if (p_sys_key->flag.ipda)
    {
        p_ds_key->ip_da = p_sys_key->ip_da;
        p_ds_mask->ip_da = p_sys_key->ip_da_mask;
    }

    /* ip sa */
    if (p_sys_key->flag.ipsa)
    {
        p_ds_key->ip_sa = p_sys_key->ip_sa;
        p_ds_mask->ip_sa = p_sys_key->ip_sa_mask;
    }

    /* l4 info mapped */
    if (p_sys_key->flag.l4info_mapped && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4info_mapped = p_sys_key->l4info_mapped;
        p_ds_mask->l4info_mapped = p_sys_key->l4info_mapped_mask;
    }

    /* is application */
    if (p_sys_key->flag.is_application && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_application = p_sys_key->is_application;
        p_ds_mask->is_application = 1;
    }

    /* is tcp */
    if (p_sys_key->flag.is_tcp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_tcp = p_sys_key->is_tcp;
        p_ds_mask->is_tcp = 1;
    }

    /* is udp */
    if (p_sys_key->flag.is_udp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_udp = p_sys_key->is_udp;
        p_ds_mask->is_udp = 1;
    }

    /* l4 source port */
    if (p_sys_key->flag.l4_src_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_source_port = p_sys_key->l4_src_port;
        p_ds_mask->l4_source_port = p_sys_key->l4_src_port_mask;
    }

    if (p_info->flag.l4_src_port)
    {
        p_ds_key->l4_source_port = p_info->l4_src_port;
        p_ds_mask->l4_source_port = p_info->l4_src_port_mask;
    }

    /* l4 destination port */
    if (p_sys_key->flag.l4_dst_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_dest_port = p_sys_key->l4_dst_port;
        p_ds_mask->l4_dest_port = p_sys_key->l4_dst_port_mask;
    }

    if (p_info->flag.l4_dst_port)
    {
        p_ds_key->l4_dest_port = p_info->l4_dst_port;
        p_ds_mask->l4_dest_port = p_info->l4_dst_port_mask;
    }

    /* dscp */
    if (p_sys_key->flag.dscp)
    {
        p_ds_key->dscp = p_sys_key->dscp;
        p_ds_mask->dscp = p_sys_key->dscp_mask;
    }

    /* ip fragement */
    if (p_sys_key->flag.frag_info)
    {
        p_ds_key->frag_info = p_sys_key->frag_info;
        p_ds_mask->frag_info = p_sys_key->frag_info_mask;
    }

    if (p_info->flag.frag_info)
    {
        p_ds_key->frag_info = p_info->frag_info;
        p_ds_mask->frag_info = p_info->frag_info_mask;
    }

    /* ip option */
    if (p_sys_key->flag.ip_option)
    {
        p_ds_key->ip_options = 1;
        p_ds_mask->ip_options = 1;
    }

    /* ip header error */
    if (p_sys_key->flag.ip_hdr_error)
    {
        p_ds_key->ip_header_error = p_sys_key->ip_hdr_error;
        p_ds_mask->ip_header_error = 1;
    }

    /* routed packet */
    if (p_sys_key->flag.routed_packet)
    {
        p_ds_key->routed_packet = p_sys_key->routed_packet;
        p_ds_mask->routed_packet = 1;
    }

    /* mac da */
    if (p_sys_key->flag.macda)
    {
        p_ds_key->macda_upper = (p_sys_key->mac_da[0] << 8) |(p_sys_key->mac_da[1]);
        p_ds_key->macda_lower = (p_sys_key->mac_da[2] << 24) | (p_sys_key->mac_da[3] << 16) |
                                                           (p_sys_key->mac_da[4] << 8) | (p_sys_key->mac_da[5]);

        p_ds_mask->macda_upper = (p_sys_key->mac_da_mask[0] << 8) | (p_sys_key->mac_da_mask[1]);
        p_ds_mask->macda_lower = (p_sys_key->mac_da_mask[2] << 24) | (p_sys_key->mac_da_mask[3] << 16) |
                                                               (p_sys_key->mac_da_mask[4] << 8) | (p_sys_key->mac_da_mask[5]);
    }

    /* mac sa */
    if (p_sys_key->flag.macsa)
    {
        p_ds_key->mac_sa_upper = (p_sys_key->mac_sa[0] << 8) |(p_sys_key->mac_sa[1]);
        p_ds_key->mac_sa_lower = (p_sys_key->mac_sa[2] << 24) | (p_sys_key->mac_sa[3] << 16) |
                                                           (p_sys_key->mac_sa[4] << 8) | (p_sys_key->mac_sa[5]);

        p_ds_mask->mac_sa_upper = (p_sys_key->mac_sa_mask[0] << 8) | (p_sys_key->mac_sa_mask[1]);
        p_ds_mask->mac_sa_lower = (p_sys_key->mac_sa_mask[2] << 24) | (p_sys_key->mac_sa_mask[3] << 16) |
                                                                (p_sys_key->mac_sa_mask[4] << 8) | (p_sys_key->mac_sa_mask[5]);
    }

    /* cos */
    if (p_sys_key->flag.cos)
    {
        p_ds_key->cos = p_sys_key->cos;
        p_ds_mask->cos = 7;
    }

    /* cvlan id */
    if (p_sys_key->flag.cvlan)
    {
        p_ds_key->cvlan_id = p_sys_key->cvlan;
        p_ds_mask->cvlan_id = p_sys_key->cvlan_mask;
    }

    /* ctag cos */
    if (p_sys_key->flag.ctag_cos)
    {
        p_ds_key->ctag_cos = p_sys_key->ctag_cos_cfi >> 1;
        p_ds_mask->ctag_cos = 7;
    }

    /* ctag cfi */
    if (p_sys_key->flag.ctag_cfi)
    {
        p_ds_key->ctag_cfi = p_sys_key->ctag_cos_cfi & 0x1;
        p_ds_mask->ctag_cfi = 1;
    }

    /* svlan id */
    if (p_sys_key->flag.svlan)
    {
        p_ds_key->svlan_id = p_sys_key->svlan;
        p_ds_mask->svlan_id = p_sys_key->svlan_mask;
    }

    /* stag cos */
    if (p_sys_key->flag.stag_cos)
    {
        p_ds_key->stag_cos = p_sys_key->stag_cos_cfi >> 1;
        p_ds_mask->stag_cos = 7;
    }

    /* stag cfi */
    if (p_sys_key->flag.stag_cfi)
    {
        p_ds_key->stag_cfi = p_sys_key->stag_cos_cfi & 0x1;
        p_ds_mask->stag_cfi = 1;
    }

    /* l2 type */
    if (p_sys_key->flag.l2_type)
    {
        p_ds_key->layer2_type = p_sys_key->l2_type;
        p_ds_mask->layer2_type = 0xF;
    }

    /* l3 type */
    if (p_sys_key->flag.l3_type)
    {
        p_ds_key->layer3_type = p_sys_key->l3_type;
        p_ds_mask->layer3_type = 0xF;
    }

    /* acl label */
    if (p_sys_key->flag.acl_label)
    {
        p_ds_key->acl_label_upper = p_sys_key->acl_label >> 4;
        p_ds_key->acl_label_lower = p_sys_key->acl_label & 0xF;

        p_ds_mask->acl_label_upper = p_sys_key->acl_label_mask >> 4;
        p_ds_mask->acl_label_lower = p_sys_key->acl_label_mask & 0xF;
    }

    /* qos label */
    if (p_sys_key->flag.qos_label)
    {
        p_ds_key->qos_label = p_sys_key->qos_label;

        p_ds_mask->qos_label = p_sys_key->qos_label_mask;
    }

    /* l2 qos label */
    if (p_sys_key->flag.l2_qos_label)
    {
        p_ds_key->l2_qos_label = p_sys_key->l2_qos_label;
        p_ds_mask->l2_qos_label = p_sys_key->l2_qos_label_mask;
    }

    /* l3 qos label */
    if (p_sys_key->flag.l3_qos_label)
    {
        p_ds_key->l3_qos_label = p_sys_key->l3_qos_label;
        p_ds_mask->l3_qos_label = p_sys_key->l3_qos_label_mask;
    }

    /*service label*/
    if((p_sys_key->flag.acl_label)&&(p_sys_key->flag.qos_label))
    {
        p_ds_key->is_label = 0;
        p_ds_mask->is_label = 1;
    }
    else
    {
        p_ds_key->is_label = 1;
        p_ds_mask->is_label = 1;
    }
    /* If the entry is global entry, it should can also match service label */
    if (p_sys_key->flag.is_glb_entry)
    {
        p_ds_mask->is_label = 0;
    }

    p_ds_key->is_ip_key = 1;
    p_ds_mask->is_ip_key = 1;

    p_ds_key->is_mpls_key = 0;
    p_ds_mask->is_mpls_key = 1;

    p_ds_key->tableid0 = p_sys_key->table_id0;
    p_ds_key->tableid1 = p_sys_key->table_id1;
    p_ds_key->tableid2 = p_sys_key->table_id2;
    p_ds_key->tableid3 = p_sys_key->table_id3;

    p_ds_mask->tableid0 = 0xF;
    p_ds_mask->tableid1 = 0xF;
    p_ds_mask->tableid2 = 0xF;
    p_ds_mask->tableid3 = 0xF;

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos mpls key
*/
static int32
_sys_humber_aclqos_build_ds_mpls_key(sys_aclqos_mpls_key_t* p_sys_key,
                                     ds_acl_mpls_key_t* p_ds_key,
                                     ds_acl_mpls_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* mac da */
    if (p_sys_key->flag.macda)
    {
        p_ds_key->macda_upper = (p_sys_key->mac_da[0] << 8) | (p_sys_key->mac_da[1]);
        p_ds_key->macda_lower = (p_sys_key->mac_da[2] << 24) | (p_sys_key->mac_da[3] << 16) |
                                                           (p_sys_key->mac_da[4] << 8) | (p_sys_key->mac_da[5]);

        p_ds_mask->macda_upper = (p_sys_key->mac_da_mask[0] << 8) | (p_sys_key->mac_da_mask[1]);
        p_ds_mask->macda_lower = (p_sys_key->mac_da_mask[2] << 24) | (p_sys_key->mac_da_mask[3] << 16) |
                                                               (p_sys_key->mac_da_mask[4] << 8) | (p_sys_key->mac_da_mask[5]);
    }

    /* mac sa */
    if (p_sys_key->flag.macsa)
    {
        p_ds_key->mac_sa_upper = (p_sys_key->mac_sa[0] << 8) | (p_sys_key->mac_sa[1]);
        p_ds_key->mac_sa_lower = (p_sys_key->mac_sa[2] << 24) | (p_sys_key->mac_sa[3] << 16) |
                                                             (p_sys_key->mac_sa[4] << 8) | (p_sys_key->mac_sa[5]);

        p_ds_mask->mac_sa_upper = (p_sys_key->mac_sa_mask[0] << 8) | (p_sys_key->mac_sa_mask[1]);
        p_ds_mask->mac_sa_lower = (p_sys_key->mac_sa_mask[2] << 24) | (p_sys_key->mac_sa_mask[3] << 16) |
                                                                (p_sys_key->mac_sa_mask[4] << 8) | (p_sys_key->mac_sa_mask[5]);
    }

    /* cos */
    if (p_sys_key->flag.cos)
    {
        p_ds_key->cos = p_sys_key->cos;
        p_ds_mask->cos = 0x7;
    }

    /* cvlan id */
    if (p_sys_key->flag.cvlan)
    {
        p_ds_key->cvlan_id = p_sys_key->cvlan;
        p_ds_mask->cvlan_id = p_sys_key->cvlan_mask;
    }

    /* ctag cos */
    if (p_sys_key->flag.ctag_cos)
    {
        p_ds_key->ctag_cos = p_sys_key->ctag_cos_cfi >> 1;
        p_ds_mask->ctag_cos = 0x7;
    }

    /* ctag cfi */
    if (p_sys_key->flag.ctag_cfi)
    {
        p_ds_key->ctag_cfi = p_sys_key->ctag_cos_cfi & 0x1;
        p_ds_mask->ctag_cfi = 0x1;
    }

    /* svlan id */
    if (p_sys_key->flag.svlan)
    {
        p_ds_key->svlan_id = p_sys_key->svlan;
        p_ds_mask->svlan_id = p_sys_key->svlan_mask;
    }

    /* stag cos */
    if (p_sys_key->flag.stag_cos)
    {
        p_ds_key->stag_cos = p_sys_key->stag_cos_cfi >> 1;
        p_ds_mask->stag_cos = 0x7;
    }

    /* stag cfi */
    if (p_sys_key->flag.stag_cfi)
    {
        p_ds_key->stag_cfi = p_sys_key->stag_cos_cfi & 0x1;
        p_ds_mask->stag_cfi = 0x1;
    }

    /* layer 2 type */
    if (p_sys_key->flag.l2_type)
    {
        p_ds_key->layer2_type = p_sys_key->l2_type;
        p_ds_mask->layer2_type = 0xF;
    }

    /* mpls label 0 */
    if (p_sys_key->flag.label0)
    {
        p_ds_key->mpls_label0 = p_sys_key->mpls_label0;
        p_ds_mask->mpls_label0 = p_sys_key->mpls_label0_mask;
    }

    /* mpls label 1 */
    if (p_sys_key->flag.label1)
    {
        p_ds_key->mpls_label1 = p_sys_key->mpls_label1;
        p_ds_mask->mpls_label1 = p_sys_key->mpls_label1_mask;
    }

    /* mpls label 2 */
    if (p_sys_key->flag.label2)
    {
        p_ds_key->mpls_label2 = p_sys_key->mpls_label2;
        p_ds_mask->mpls_label2 = p_sys_key->mpls_label2_mask;
    }

    /* mpls label 3 */
    if (p_sys_key->flag.label3)
    {
        p_ds_key->mpls_label331to28 = (p_sys_key->mpls_label3 >> 28)&0xF;
        p_ds_key->mpls_label327to0 = p_sys_key->mpls_label3 & 0xFFFFFFF;

        p_ds_mask->mpls_label331to28 = (p_sys_key->mpls_label3_mask >> 28)&0xF;
        p_ds_mask->mpls_label327to0 = p_sys_key->mpls_label3_mask & 0xFFFFFFF;
    }

    /* routed packet */
    if (p_sys_key->flag.routed_packet)
    {
        p_ds_key->route_pkt = p_sys_key->routed_packet;
        p_ds_mask->route_pkt = 1;
    }

    /* acl label */
    if (p_sys_key->flag.acl_label)
    {
        p_ds_key->acl_label_upper = (p_sys_key->acl_label >> 4)&0xF;
        p_ds_key->acl_label_lower = p_sys_key->acl_label & 0xF;

        p_ds_mask->acl_label_upper = (p_sys_key->acl_label_mask >> 4)&0xF;
        p_ds_mask->acl_label_lower = p_sys_key->acl_label_mask & 0xF;
    }

    /* qos label */
    if (p_sys_key->flag.qos_label)
    {
        p_ds_key->qos_label = p_sys_key->qos_label;
        p_ds_mask->qos_label = p_sys_key->qos_label_mask;
    }

    /* l2 qos label */
    if (p_sys_key->flag.l2_qos_label)
    {
        p_ds_key->l2_qos_label = p_sys_key->l2_qos_label;
        p_ds_mask->l2_qos_label = p_sys_key->l2_qos_label_mask;
    }

    /* l3 qos label */
    if (p_sys_key->flag.l3_qos_label)
    {
        p_ds_key->l3_qos_label = p_sys_key->l3_qos_label;
        p_ds_mask->l3_qos_label = p_sys_key->l3_qos_label_mask;
    }

    /*service label*/
    if((p_sys_key->flag.acl_label)&&(p_sys_key->flag.qos_label))
    {
        p_ds_key->is_label = 0;
        p_ds_mask->is_label = 1;
    }
    else
    {
        p_ds_key->is_label = 1;
        p_ds_mask->is_label = 1;
    }

    p_ds_key->is_ip_key = 1;
    p_ds_mask->is_ip_key = 1;

    p_ds_key->is_mpls_key = 1;
    p_ds_mask->is_mpls_key = 1;

    p_ds_key->tableid0 = p_sys_key->table_id0;
    p_ds_key->tableid1 = p_sys_key->table_id1;
    p_ds_key->tableid2 = p_sys_key->table_id2;
    p_ds_key->tableid3 = p_sys_key->table_id3;

    p_ds_mask->tableid0 = 0xF;
    p_ds_mask->tableid1 = 0xF;
    p_ds_mask->tableid2 = 0xF;
    p_ds_mask->tableid3 = 0xF;

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos ipv6 key
*/
static int32
_sys_humber_aclqos_build_ds_ipv6_key(sys_aclqos_ipv6_key_t* p_sys_key,
                                     sys_aclqos_sub_entry_info_t* p_info,
                                     ds_acl_ipv6_key_t* p_ds_key,
                                     ds_acl_ipv6_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_info);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* ip da */
    if (p_sys_key->flag.ipda)
    {
        p_ds_key->ip_da127to104 = (p_sys_key->ip_da[0]>>8)&0xFFFFFF;
        p_ds_key->ip_da103to72 = ((p_sys_key->ip_da[0]&0xFF)<<24) | ((p_sys_key->ip_da[1]>>8)&0xFFFFFF);
        p_ds_key->ip_da71to64 = p_sys_key->ip_da[1]&0xFF;
        p_ds_key->ip_da63to32 = p_sys_key->ip_da[2];
        p_ds_key->ip_da31to0 = p_sys_key->ip_da[3];


        p_ds_mask->ip_da127to104 = (p_sys_key->ip_da_mask[0]>>8)&0xFFFFFF;
        p_ds_mask->ip_da103to72 = ((p_sys_key->ip_da_mask[0]&0xFF)<<24) | ((p_sys_key->ip_da_mask[1]>>8)&0xFFFFFF);
        p_ds_mask->ip_da71to64 = p_sys_key->ip_da_mask[1]&0xFF;
        p_ds_mask->ip_da63to32 = p_sys_key->ip_da_mask[2];
        p_ds_mask->ip_da31to0 = p_sys_key->ip_da_mask[3];
    }

    /* ip sa */
    if (p_sys_key->flag.ipsa)
    {
        p_ds_key->ip_sa127to104 = (p_sys_key->ip_sa[0]>>8)&0xFFFFFF;
        p_ds_key->ip_sa103to72 = ((p_sys_key->ip_sa[0]&0xFF)<<24) | ((p_sys_key->ip_sa[1]>>8)&0xFFFFFF);
        p_ds_key->ip_sa71to64 = p_sys_key->ip_sa[1]&0xFF;
        p_ds_key->ip_sa63to32 = p_sys_key->ip_sa[2];
        p_ds_key->ip_sa31to0 = p_sys_key->ip_sa[3];


        p_ds_mask->ip_sa127to104 = (p_sys_key->ip_sa_mask[0]>>8)&0xFFFFFF;
        p_ds_mask->ip_sa103to72 = ((p_sys_key->ip_sa_mask[0]&0xFF)<<24) | ((p_sys_key->ip_sa_mask[1]>>8)&0xFFFFFF);
        p_ds_mask->ip_sa71to64 = p_sys_key->ip_sa_mask[1]&0xFF;
        p_ds_mask->ip_sa63to32 = p_sys_key->ip_sa_mask[2];
        p_ds_mask->ip_sa31to0 = p_sys_key->ip_sa_mask[3];
    }

    /* l4 info mapped */
    if (p_sys_key->flag.l4info_mapped && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4info_mapped = p_sys_key->l4info_mapped;
        p_ds_mask->l4info_mapped = p_sys_key->l4info_mapped_mask;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4info_mapped = %04x\n", p_ds_key->l4info_mapped);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4info_mapped = %04x\n", p_ds_mask->l4info_mapped);
    }

    /* is application */
    if (p_sys_key->flag.is_application && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_application = p_sys_key->is_application;
        p_ds_mask->is_application = 1;
    }

    /* is tcp */
    if (p_sys_key->flag.is_tcp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_tcp = p_sys_key->is_tcp;
        p_ds_mask->is_tcp = 1;
    }

    /* is udp */
    if (p_sys_key->flag.is_udp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_udp = p_sys_key->is_udp;
        p_ds_mask->is_udp = 1;
    }

    /* l4 source port */
    if (p_sys_key->flag.l4_src_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_source_port15to12 = p_sys_key->l4_src_port >> 12;
        p_ds_key->l4_source_port11to4 = (p_sys_key->l4_src_port >> 4) & 0xFF;
        p_ds_key->l4_source_port3to0 = p_sys_key->l4_src_port & 0xF;

        p_ds_mask->l4_source_port15to12 = p_sys_key->l4_src_port_mask >> 12;
        p_ds_mask->l4_source_port11to4 = (p_sys_key->l4_src_port_mask >> 4) & 0xFF;
        p_ds_mask->l4_source_port3to0 = p_sys_key->l4_src_port_mask & 0xF;
    }

    if (p_info->flag.l4_src_port)
    {
        p_ds_key->l4_source_port15to12 = p_info->l4_src_port >> 12;
        p_ds_key->l4_source_port11to4 = (p_info->l4_src_port >> 4) & 0xFF;
        p_ds_key->l4_source_port3to0 = p_info->l4_src_port & 0xF;

        p_ds_mask->l4_source_port15to12 = p_info->l4_src_port_mask >> 12;
        p_ds_mask->l4_source_port11to4 = (p_info->l4_src_port_mask >> 4) & 0xFF;
        p_ds_mask->l4_source_port3to0 = p_info->l4_src_port_mask & 0xF;
    }

    /* l4 destination port */
    if (p_sys_key->flag.l4_dst_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_destport = p_sys_key->l4_dst_port;
        p_ds_mask->l4_destport = p_sys_key->l4_dst_port_mask;
    }

    if((p_sys_key->flag.is_tcp && p_sys_key->is_tcp)||(p_sys_key->flag.is_udp && p_sys_key->is_udp))
    {
        p_ds_key->l4destport_or_l4info15to6 = (p_sys_key->l4_dst_port >> 6) & 0x3FF;
        p_ds_key->l4destport_or_l4info5to0 = p_sys_key->l4_dst_port & 0x3F;

        p_ds_mask->l4destport_or_l4info15to6 = (p_sys_key->l4_dst_port_mask >> 6) & 0x3FF;
        p_ds_mask->l4destport_or_l4info5to0 = p_sys_key->l4_dst_port_mask & 0x3F;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4destport_or_l4info15to6 = (p_sys_key->l4_dst_port >> 6) & 0x3FF = %03x\n", p_ds_key->l4destport_or_l4info15to6);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4destport_or_l4info5to0 = p_sys_key->l4_dst_port & 0x3F = %02x\n", p_ds_key->l4destport_or_l4info5to0);

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4destport_or_l4info15to6 = (p_sys_key->l4_dst_port_mask >> 6) & 0x3FF = %03x\n",
                p_ds_mask->l4destport_or_l4info15to6);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4destport_or_l4info5to0 = p_sys_key->l4_dst_port_mask & 0x3F = %02x\n",
                p_ds_mask->l4destport_or_l4info5to0);
    }
    else
    {
        p_ds_key->l4destport_or_l4info15to6 = (p_sys_key->l4info_mapped >> 6) & 0x3FF;
        p_ds_key->l4destport_or_l4info5to0 = p_sys_key->l4info_mapped & 0x3F;

        p_ds_mask->l4destport_or_l4info15to6 = (p_sys_key->l4info_mapped_mask >> 6 ) & 0x3FF;
        p_ds_mask->l4destport_or_l4info5to0 = p_sys_key->l4info_mapped_mask & 0x3F;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4destport_or_l4info15to6 = (p_sys_key->l4info_mapped >> 6) & 0x3FF = %03x\n",
                p_ds_key->l4destport_or_l4info15to6);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4destport_or_l4info5to0 = p_sys_key->l4info_mapped & 0x3F = %03x\n",
                p_ds_key->l4destport_or_l4info5to0);

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4destport_or_l4info15to6 = (p_sys_key->l4info_mapped_mask >> 6 ) & 0x3FF = %03x\n",
                p_ds_mask->l4destport_or_l4info15to6);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4destport_or_l4info5to0 = p_sys_key->l4info_mapped_mask & 0x3F = %02x\n",
                p_ds_mask->l4destport_or_l4info5to0);
    }

    if (p_info->flag.l4_dst_port)
    {
        p_ds_key->l4_destport = p_info->l4_dst_port;
        p_ds_mask->l4_destport = p_info->l4_dst_port_mask;
    }

    /* dscp */
    if (p_sys_key->flag.dscp)
    {
        p_ds_key->dscp = p_sys_key->dscp;
        p_ds_mask->dscp = p_sys_key->dscp_mask;
    }

    /* ip fragement */
    if (p_sys_key->flag.frag_info)
    {
        p_ds_key->frag_info = p_sys_key->frag_info;
        p_ds_mask->frag_info = p_sys_key->frag_info_mask;
    }

    if (p_info->flag.frag_info)
    {
        p_ds_key->frag_info = p_info->frag_info;
        p_ds_mask->frag_info = p_info->frag_info_mask;
    }

    /* ip option */
    if (p_sys_key->flag.ip_option)
    {
        p_ds_key->ip_options = p_sys_key->ip_option;
        p_ds_mask->ip_options = 1;
    }

    /* ip header error */
    if (p_sys_key->flag.ip_hdr_error)
    {
        p_ds_key->ip_head_error = p_sys_key->ip_hdr_error;
        p_ds_mask->ip_head_error = 1;
    }

    /* routed packet */
    if (p_sys_key->flag.routed_packet)
    {
        p_ds_key->routed_packet = p_sys_key->routed_packet;
        p_ds_mask->routed_packet = 1;
    }

    /* extension header */
    if (p_sys_key->flag.ext_hdr)
    {
        p_ds_key->ipv6_extension_headers = p_sys_key->ext_hdr;
        p_ds_mask->ipv6_extension_headers = p_sys_key->ext_hdr_mask;
    }

    if (p_info->flag.ext_hdr)
    {
        p_ds_key->ipv6_extension_headers = p_info->ext_hdr;
        p_ds_mask->ipv6_extension_headers = p_info->ext_hdr_mask;
    }

    /* flow label */
    if (p_sys_key->flag.flow_label)
    {
        p_ds_key->ipv6_flow_label = p_sys_key->flow_label;
        p_ds_mask->ipv6_flow_label = p_sys_key->flow_label_mask;
    }

    /* mac da */
    if (p_sys_key->flag.macda)
    {
        p_ds_key->mac_da_upper = (p_sys_key->mac_da[0] << 8) |(p_sys_key->mac_da[1]);
        p_ds_key->mac_da_lower = (p_sys_key->mac_da[2] << 24) | (p_sys_key->mac_da[3] << 16) |
                                                              (p_sys_key->mac_da[4] << 8) | (p_sys_key->mac_da[5]);

        p_ds_mask->mac_da_upper = (p_sys_key->mac_da_mask[0] << 8) | (p_sys_key->mac_da_mask[1]);
        p_ds_mask->mac_da_lower = (p_sys_key->mac_da_mask[2] << 24) | (p_sys_key->mac_da_mask[3] << 16) |
                                                                 (p_sys_key->mac_da_mask[4] << 8) | (p_sys_key->mac_da_mask[5]);
    }

    /* mac sa */
    if (p_sys_key->flag.macsa)
    {
        p_ds_key->mac_sa_upper = (p_sys_key->mac_sa[0] << 8) |(p_sys_key->mac_sa[1]);
        p_ds_key->mac_sa_lower = (p_sys_key->mac_sa[2] << 24) | (p_sys_key->mac_sa[3] << 16) |
                                                             (p_sys_key->mac_sa[4] << 8) | (p_sys_key->mac_sa[5]);

        p_ds_mask->mac_sa_upper = (p_sys_key->mac_sa_mask[0] << 8) | (p_sys_key->mac_sa_mask[1]);
        p_ds_mask->mac_sa_lower = (p_sys_key->mac_sa_mask[2] << 24) | (p_sys_key->mac_sa_mask[3] << 16) |
                                                                (p_sys_key->mac_sa_mask[4] << 8) | (p_sys_key->mac_sa_mask[5]);
    }

    /* cos */
    if (p_sys_key->flag.cos)
    {
        p_ds_key->cos = p_sys_key->cos;
        p_ds_mask->cos = 7;
    }

    /* cvlan id */
    if (p_sys_key->flag.cvlan)
    {
        p_ds_key->cvlan_id = p_sys_key->cvlan;
        p_ds_mask->cvlan_id = p_sys_key->cvlan_mask;
    }

    /* ctag cos */
    if (p_sys_key->flag.ctag_cos)
    {
        p_ds_key->ctag_cos = p_sys_key->ctag_cos_cfi >> 1;
        p_ds_mask->ctag_cos = 7;
    }

    /* ctag cfi */
    if (p_sys_key->flag.ctag_cfi)
    {
        p_ds_key->ctag_cfi = p_sys_key->ctag_cos_cfi & 0x1;
        p_ds_mask->ctag_cfi = 1;
    }

    /* svlan id */
    if (p_sys_key->flag.svlan)
    {
        p_ds_key->svlan_id = p_sys_key->svlan;
        p_ds_mask->svlan_id = p_sys_key->svlan_mask;
    }

    /* stag cos */
    if (p_sys_key->flag.stag_cos)
    {
        p_ds_key->stag_cos = p_sys_key->stag_cos_cfi >> 1;
        p_ds_mask->stag_cos = 7;
    }

    /* stag cfi */
    if (p_sys_key->flag.stag_cfi)
    {
        p_ds_key->stag_cfi = p_sys_key->stag_cos_cfi & 0x1;
        p_ds_mask->stag_cfi = 1;
    }

    /* l2 type */
    if (p_sys_key->flag.l2_type)
    {
        p_ds_key->layer2_type = p_sys_key->l2_type;
        p_ds_mask->layer2_type = 0xF;
    }

    /* l3 type */
    if (p_sys_key->flag.l3_type)
    {
        p_ds_key->layer3_type = p_sys_key->l3_type;
        p_ds_mask->layer3_type = 0xF;
    }

    /* acl label */
    if (p_sys_key->flag.acl_label)
    {
        p_ds_key->acl_label_upper = p_sys_key->acl_label >> 6;
        p_ds_key->acl_label_middle = (p_sys_key->acl_label >> 4) & 0x3;
        p_ds_key->acl_label_lower = p_sys_key->acl_label & 0xF;

        p_ds_mask->acl_label_upper = p_sys_key->acl_label_mask >> 6;
        p_ds_mask->acl_label_middle = (p_sys_key->acl_label_mask >> 4) & 0x3;
        p_ds_mask->acl_label_lower = p_sys_key->acl_label_mask & 0xF;
    }

    /* qos label */
    if (p_sys_key->flag.qos_label)
    {
        p_ds_key->qos_label_upper = p_sys_key->qos_label >> 6;
        p_ds_key->qos_label_lower = p_sys_key->qos_label & 0x3F;

        p_ds_mask->qos_label_upper = p_sys_key->qos_label_mask >> 6;
        p_ds_mask->qos_label_lower = p_sys_key->qos_label_mask & 0x3F;
    }

    /* l2 qos label */
    if (p_sys_key->flag.l2_qos_label)
    {
        p_ds_key->l2_qos_label = p_sys_key->l2_qos_label;
        p_ds_mask->l2_qos_label = p_sys_key->l2_qos_label_mask;
    }

    /* l3 qos label */
    if (p_sys_key->flag.l3_qos_label)
    {
        p_ds_key->l3_qos_label = p_sys_key->l3_qos_label;
        p_ds_mask->l3_qos_label = p_sys_key->l3_qos_label_mask;
    }

    /*service label*/
    if((p_sys_key->flag.acl_label)&&(p_sys_key->flag.qos_label))
    {
        p_ds_key->is_label = 0;
        p_ds_mask->is_label = 1;
    }
    else
    {
        p_ds_key->is_label = 1;
        p_ds_mask->is_label = 1;
    }

    p_ds_key->tableid0 = p_sys_key->table_id0;
    p_ds_key->tableid1 = p_sys_key->table_id1;
    p_ds_key->tableid2 = p_sys_key->table_id2;
    p_ds_key->tableid3 = p_sys_key->table_id3;
    p_ds_key->tableid4 = p_sys_key->table_id4;
    p_ds_key->tableid5 = p_sys_key->table_id5;
    p_ds_key->tableid6 = p_sys_key->table_id6;
    p_ds_key->aclqos_ipv6key_tableid7 = p_sys_key->table_id7;

    p_ds_mask->tableid0 = 0xF;
    p_ds_mask->tableid1 = 0xF;
    p_ds_mask->tableid2 = 0xF;
    p_ds_mask->tableid3 = 0xF;
    p_ds_mask->tableid4 = 0xF;
    p_ds_mask->tableid5 = 0xF;
    p_ds_mask->tableid6 = 0xF;
    p_ds_key->aclqos_ipv6key_tableid7 = 0xF;

    return CTC_E_NONE;
}


/**
 @brief build ds acl pbr ipv4 key
*/
static int32
_sys_humber_acl_pbr_build_ds_ipv4_key(sys_acl_pbr_ipv4_key_t* p_sys_key,
                                     sys_aclqos_sub_entry_info_t* p_info,
                                     ds_ipv4_pbr_dualda_key_t* p_ds_key,
                                     ds_ipv4_pbr_dualda_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_info);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* ip da */
    if (p_sys_key->flag.ipda)
    {
        p_ds_key->ip_sa = p_sys_key->ip_da;         /*must use ipsa*/
        p_ds_mask->ip_sa = p_sys_key->ip_da_mask;
    }

    /* ip sa */
    if (p_sys_key->flag.ipsa)
    {
        p_ds_key->ip_da = p_sys_key->ip_sa;         /*must use ipda*/
        p_ds_mask->ip_da = p_sys_key->ip_sa_mask;
    }

    /* vrfid */
    if (p_sys_key->flag.vrf_id)
    {
        p_ds_key->vrf_idh = p_sys_key->vrf_id >> 4;
        p_ds_mask->vrf_idh = p_sys_key->vrf_id_mask >> 4;
        p_ds_key->vrf_idl = p_sys_key->vrf_id & 0x000F;
        p_ds_mask->vrf_idl = p_sys_key->vrf_id_mask & 0x000F;
    }

    /* l4 info mapped */
    if (p_sys_key->flag.l4info_mapped && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4info_mapped = p_sys_key->l4info_mapped;
        p_ds_mask->l4info_mapped = p_sys_key->l4info_mapped_mask;
    }

    /* is tcp */
    if (p_sys_key->flag.is_tcp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_tcp = p_sys_key->is_tcp;
        p_ds_mask->is_tcp = 1;
    }

    /* is udp */
    if (p_sys_key->flag.is_udp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_udp = p_sys_key->is_udp;
        p_ds_mask->is_udp = 1;
    }

    /* l4 source port */
    if (p_sys_key->flag.l4_src_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_source_port = p_sys_key->l4_src_port;
        p_ds_mask->l4_source_port = p_sys_key->l4_src_port_mask;
    }

    if (p_info->flag.l4_src_port)
    {
        p_ds_key->l4_source_port = p_info->l4_src_port;
        p_ds_mask->l4_source_port = p_info->l4_src_port_mask;
    }

    /* l4 destination port */
    if (p_sys_key->flag.l4_dst_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_dest_port = p_sys_key->l4_dst_port;
        p_ds_mask->l4_dest_port = p_sys_key->l4_dst_port_mask;
    }

    if (p_info->flag.l4_dst_port)
    {
        p_ds_key->l4_dest_port = p_info->l4_dst_port;
        p_ds_mask->l4_dest_port = p_info->l4_dst_port_mask;
    }

    /* dscp */
    if (p_sys_key->flag.dscp)
    {
        p_ds_key->dscp = p_sys_key->dscp;
        p_ds_mask->dscp = p_sys_key->dscp_mask;
    }

    /* ip fragement */
    if (p_sys_key->flag.frag_info)
    {
        p_ds_key->frag_info = p_sys_key->frag_info;
        p_ds_mask->frag_info = p_sys_key->frag_info_mask;
    }

    if (p_info->flag.frag_info)
    {
        p_ds_key->frag_info = p_info->frag_info;
        p_ds_mask->frag_info = p_info->frag_info_mask;
    }

    /* ip option */
    if (p_sys_key->flag.ip_option)
    {
        p_ds_key->ip_options = 1;
        p_ds_mask->ip_options = 1;
    }

    /* pbr label */
    p_ds_key->pbr_label = p_sys_key->pbr_label;
    p_ds_mask->pbr_label = p_sys_key->pbr_label_mask;


    p_ds_key->table_id0 = p_sys_key->table_id0;
    p_ds_key->table_id1 = p_sys_key->table_id1;

    p_ds_mask->table_id0 = 0xF;
    p_ds_mask->table_id1 = 0xF;

    return CTC_E_NONE;
}


/**
 @brief build ds acl pbr ipv6 key
*/
static int32
_sys_humber_acl_pbr_build_ds_ipv6_key(sys_acl_pbr_ipv6_key_t* p_sys_key,
                                     sys_aclqos_sub_entry_info_t* p_info,
                                     ds_ipv6_pbr_dualda_key_t* p_ds_key,
                                     ds_ipv6_pbr_dualda_key_t* p_ds_mask)
{
    CTC_PTR_VALID_CHECK(p_sys_key);
    CTC_PTR_VALID_CHECK(p_info);
    CTC_PTR_VALID_CHECK(p_ds_key);
    CTC_PTR_VALID_CHECK(p_ds_mask);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* ip da */
    if (p_sys_key->flag.ipda)
    {
        p_ds_key->ip_sa4 = (p_sys_key->ip_da[0]>>8)&0xFFFFFF;
        p_ds_key->ip_sa3 = ((p_sys_key->ip_da[0]&0xFF)<<24) | ((p_sys_key->ip_da[1]>>8)&0xFFFFFF);
        p_ds_key->ip_sa2 = p_sys_key->ip_da[1]&0xFF;
        p_ds_key->ip_sa1 = p_sys_key->ip_da[2];
        p_ds_key->ip_sa0 = p_sys_key->ip_da[3];


        p_ds_mask->ip_sa4 = (p_sys_key->ip_da_mask[0]>>8)&0xFFFFFF;
        p_ds_mask->ip_sa3 = ((p_sys_key->ip_da_mask[0]&0xFF)<<24) | ((p_sys_key->ip_da_mask[1]>>8)&0xFFFFFF);
        p_ds_mask->ip_sa2 = p_sys_key->ip_da_mask[1]&0xFF;
        p_ds_mask->ip_sa1 = p_sys_key->ip_da_mask[2];
        p_ds_mask->ip_sa0 = p_sys_key->ip_da_mask[3];
    }

    /* ip sa */
    if (p_sys_key->flag.ipsa)
    {
        p_ds_key->ip_da4 = (p_sys_key->ip_sa[0]>>8)&0xFFFFFF;
        p_ds_key->ip_da3 = ((p_sys_key->ip_sa[0]&0xFF)<<24) | ((p_sys_key->ip_sa[1]>>8)&0xFFFFFF);
        p_ds_key->ip_da2 = p_sys_key->ip_sa[1]&0xFF;
        p_ds_key->ip_da1 = p_sys_key->ip_sa[2];
        p_ds_key->ip_da0 = p_sys_key->ip_sa[3];


        p_ds_mask->ip_da4 = (p_sys_key->ip_sa_mask[0]>>8)&0xFFFFFF;
        p_ds_mask->ip_da3 = ((p_sys_key->ip_sa_mask[0]&0xFF)<<24) | ((p_sys_key->ip_sa_mask[1]>>8)&0xFFFFFF);
        p_ds_mask->ip_da2 = p_sys_key->ip_sa_mask[1]&0xFF;
        p_ds_mask->ip_da1 = p_sys_key->ip_sa_mask[2];
        p_ds_mask->ip_da0 = p_sys_key->ip_sa_mask[3];
    }

    /* l4 info mapped */
    if (p_sys_key->flag.l4info_mapped && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_info_mapped = p_sys_key->l4info_mapped;
        p_ds_mask->l4_info_mapped = p_sys_key->l4info_mapped_mask;

        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_key->l4info_mapped = %04x\n", p_ds_key->l4_info_mapped);
        SYS_ACLQOS_ENTRY_DBG_INFO("p_ds_mask->l4info_mapped = %04x\n", p_ds_mask->l4_info_mapped);
    }

    /* is tcp */
    if (p_sys_key->flag.is_tcp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_tcp = p_sys_key->is_tcp;
        p_ds_mask->is_tcp = 1;
    }

    /* is udp */
    if (p_sys_key->flag.is_udp && (!p_info->flag.reset_l4info))
    {
        p_ds_key->is_udp = p_sys_key->is_udp;
        p_ds_mask->is_udp = 1;
    }

    /* l4 source port */
    if (p_sys_key->flag.l4_src_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_source_port = p_sys_key->l4_src_port;
        p_ds_mask->l4_source_port = p_sys_key->l4_src_port_mask;
    }

    if (p_info->flag.l4_src_port)
    {
        p_ds_key->l4_source_port = p_info->l4_src_port;
        p_ds_mask->l4_source_port = p_info->l4_src_port_mask;
    }

    /* l4 destination port */
    if (p_sys_key->flag.l4_dst_port && (!p_info->flag.reset_l4info))
    {
        p_ds_key->l4_dest_port = p_sys_key->l4_dst_port;
        p_ds_mask->l4_dest_port = p_sys_key->l4_dst_port_mask;
    }

    if (p_info->flag.l4_dst_port)
    {
        p_ds_key->l4_dest_port = p_info->l4_dst_port;
        p_ds_mask->l4_dest_port = p_info->l4_dst_port_mask;
    }

    /* vrfid */
    if (p_sys_key->flag.vrf_id)
    {
        p_ds_key->vrf_idh = p_sys_key->vrf_id >> 4;
        p_ds_mask->vrf_idh = p_sys_key->vrf_id_mask >> 4;
        p_ds_key->vrf_idl = p_sys_key->vrf_id & 0x000F;
        p_ds_mask->vrf_idl = p_sys_key->vrf_id_mask & 0x000F;
    }

    /* dscp */
    if (p_sys_key->flag.dscp)
    {
        p_ds_key->dscp = p_sys_key->dscp;
        p_ds_mask->dscp = p_sys_key->dscp_mask;
    }

    /* ip fragement */
    if (p_sys_key->flag.frag_info)
    {
        p_ds_key->frag_info = p_sys_key->frag_info;
        p_ds_mask->frag_info = p_sys_key->frag_info_mask;
    }

    if (p_info->flag.frag_info)
    {
        p_ds_key->frag_info = p_info->frag_info;
        p_ds_mask->frag_info = p_info->frag_info_mask;
    }

    /* ip option */
    if (p_sys_key->flag.ip_option)
    {
        p_ds_key->ip_options = 1;
        p_ds_mask->ip_options = 1;
    }

    /* extension header */
    if (p_sys_key->flag.ext_hdr)
    {
        p_ds_key->ipv6_extension_headers = p_sys_key->ext_hdr;
        p_ds_mask->ipv6_extension_headers = p_sys_key->ext_hdr_mask;
    }

    if (p_info->flag.ext_hdr)
    {
        p_ds_key->ipv6_extension_headers = p_info->ext_hdr;
        p_ds_mask->ipv6_extension_headers = p_info->ext_hdr_mask;
    }

    /* flow label */
    if (p_sys_key->flag.flow_label)
    {
        p_ds_key->ipv6_flow_labelh = p_sys_key->flow_label >> 8;
        p_ds_key->ipv6_flow_labell = p_sys_key->flow_label & 0xFF;

        p_ds_mask->ipv6_flow_labelh = p_sys_key->flow_label_mask >> 8;
        p_ds_mask->ipv6_flow_labell = p_sys_key->flow_label & 0xFF;
    }

    /* pbr label */
    p_ds_key->pbr_label = p_sys_key->pbr_label;
    p_ds_mask->pbr_label = p_sys_key->pbr_label_mask;

    p_ds_key->table_id0 = p_sys_key->table_id0;
    p_ds_key->table_id1 = p_sys_key->table_id1;
    p_ds_key->table_id2 = p_sys_key->table_id2;
    p_ds_key->table_id3 = p_sys_key->table_id3;
    p_ds_key->table_id4 = p_sys_key->table_id4;
    p_ds_key->table_id5 = p_sys_key->table_id5;
    p_ds_key->table_id6 = p_sys_key->table_id6;
    p_ds_key->ipv6_rtk_table_id7 = p_sys_key->table_id7;

    p_ds_mask->table_id0 = 0xF;
    p_ds_mask->table_id1 = 0xF;
    p_ds_mask->table_id2 = 0xF;
    p_ds_mask->table_id3 = 0xF;
    p_ds_mask->table_id4 = 0xF;
    p_ds_mask->table_id5 = 0xF;
    p_ds_mask->table_id6 = 0xF;
    p_ds_key->ipv6_rtk_table_id7 = 0xF;

    return CTC_E_NONE;
}

/**
 @brief get acl/qos action table id
*/
static int32
_sys_humber_aclqos_get_action_table_id(sys_aclqos_label_type_t label_type,
                                       ctc_aclqos_key_type_t key_type,
                                       tbl_id_t* p_table_id)
{
    CTC_PTR_VALID_CHECK(p_table_id);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (IS_ACL_TABLE(label_type))
    {
        switch(key_type)
        {
            case CTC_ACLQOS_MAC_KEY:
                *p_table_id = DS_MAC_ACL;
                break;

            case CTC_ACLQOS_IPV4_KEY:
                *p_table_id = DS_IPV4_ACL;
                break;

            case CTC_ACLQOS_MPLS_KEY:
                *p_table_id = DS_MPLS_ACL;
                break;

            case CTC_ACLQOS_IPV6_KEY:
                *p_table_id = DS_IPV6_ACL;
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else if (IS_QOS_TABLE(label_type) || (SYS_SERVICE_ACLQOS_LABEL == label_type))
    {
        switch(key_type)
        {
            case CTC_ACLQOS_MAC_KEY:
                *p_table_id = DS_MAC_QOS;
                break;

            case CTC_ACLQOS_IPV4_KEY:
                *p_table_id = DS_IPV4_QOS;
                break;

            case CTC_ACLQOS_MPLS_KEY:
                *p_table_id = DS_MPLS_QOS;
                break;

            case CTC_ACLQOS_IPV6_KEY:
                *p_table_id = DS_IPV6_QOS;
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else if (IS_PBR_TABLE(label_type))
    {
        if(CTC_ACL_PBR_IPV4_KEY == key_type)
        {
            *p_table_id = DS_IPV4_UCAST_PBR_DUAL_DA;
        }
        else if(CTC_ACL_PBR_IPV6_KEY == key_type)
        {
            *p_table_id = DS_IPV6_UCAST_PBR_DUAL_DA;
        }
        else
        {
            return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else
    {
        return CTC_E_ACLQOS_INVALID_LABEL_TYPE;
    }

    return CTC_E_NONE;
}



/**
 @brief get acl/qos key table id
*/
static int32
_sys_humber_aclqos_get_key_table_id(sys_aclqos_label_type_t label_type,
                                    ctc_aclqos_key_type_t key_type,
                                    tbl_id_t* p_table_id)
{
    CTC_PTR_VALID_CHECK(p_table_id);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (IS_ACL_TABLE(label_type))
    {
        switch(key_type)
        {
            case CTC_ACLQOS_MAC_KEY:
                *p_table_id = DS_ACL_MAC_KEY;
                break;

            case CTC_ACLQOS_IPV4_KEY:
                *p_table_id = DS_ACL_IPV4_KEY;
                break;

            case CTC_ACLQOS_MPLS_KEY:
                *p_table_id = DS_ACL_MPLS_KEY;
                break;

            case CTC_ACLQOS_IPV6_KEY:
                *p_table_id = DS_ACL_IPV6_KEY;
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else if (IS_QOS_TABLE(label_type) || (SYS_SERVICE_ACLQOS_LABEL == label_type))
    {
        switch(key_type)
        {
            case CTC_ACLQOS_MAC_KEY:
                *p_table_id = DS_QOS_MAC_KEY;
                break;

            case CTC_ACLQOS_IPV4_KEY:
                *p_table_id = DS_QOS_IPV4_KEY;
                break;

            case CTC_ACLQOS_MPLS_KEY:
                *p_table_id = DS_QOS_MPLS_KEY;
                break;

            case CTC_ACLQOS_IPV6_KEY:
                *p_table_id = DS_QOS_IPV6_KEY;
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else if (IS_PBR_TABLE(label_type))
    {
        if(CTC_ACL_PBR_IPV4_KEY == key_type)
        {
            *p_table_id = DS_IPV4_PBR_DUALDA_KEY;
        }
        else if(CTC_ACL_PBR_IPV6_KEY == key_type)
        {
            *p_table_id = DS_IPV6_PBR_DUALDA_KEY;
        }
        else
        {
            return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }
    else
    {
        return CTC_E_ACLQOS_INVALID_LABEL_TYPE;
    }

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos action and write it to chip
*/
static int32
_sys_humber_aclqos_write_action_to_chip(uint8 lchip, tbl_id_t table_id,
                                        sys_aclqos_action_t* p_action,
                                        sys_aclqos_sub_entry_info_t* p_info)
{
    ds_mac_acl_t ds_action;
    uint32 cmd_action;

    CTC_PTR_VALID_CHECK(p_action);
    CTC_PTR_VALID_CHECK(p_info);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    kal_memset(&ds_action, 0, sizeof(ds_mac_acl_t));

    CTC_ERROR_RETURN(_sys_humber_aclqos_build_ds_action(lchip, p_action, &ds_action));

    cmd_action = DRV_IOW(IOC_TABLE, table_id, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_action, &ds_action));

    SYS_ACLQOS_ENTRY_DBG_FUNC();
    SYS_ACLQOS_ENTRY_DBG_INFO("action index:%d\n",p_info->offset);

    return CTC_E_NONE;
}


/**
 @brief build ds acl pbr action and write it to chip
*/
static int32
_sys_humber_acl_pbr_write_action_to_chip(uint8 lchip, tbl_id_t table_id,
                                        sys_aclqos_action_t* p_action,
                                        sys_aclqos_sub_entry_info_t* p_info)
{
    ds_ipv4_ucast_pbr_dual_da_t ds_action;
    uint32 cmd_action;

    CTC_PTR_VALID_CHECK(p_action);
    CTC_PTR_VALID_CHECK(p_info);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    kal_memset(&ds_action, 0, sizeof(ds_ipv4_ucast_pbr_dual_da_t));

    CTC_ERROR_RETURN(_sys_humber_acl_pbr_build_ds_action(lchip, p_action, &ds_action));

    cmd_action = DRV_IOW(IOC_TABLE, table_id, DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_action, &ds_action));

    SYS_ACLQOS_ENTRY_DBG_FUNC();
    SYS_ACLQOS_ENTRY_DBG_INFO("action index:%d\n",p_info->offset);

    return CTC_E_NONE;
}


/**
 @brief build ds acl/qos key and write it to chip
*/
static int32
_sys_humber_aclqos_write_key_to_chip(uint8 lchip, tbl_id_t table_id,
                                     sys_aclqos_key_t* p_key,
                                     sys_aclqos_sub_entry_info_t* p_info)
{
    tbl_entry_t ds_key;
    uint32 cmd_key;

    CTC_PTR_VALID_CHECK(p_key);
    CTC_PTR_VALID_CHECK(p_info);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    cmd_key = DRV_IOW(IOC_TABLE, table_id, DRV_ENTRY_FLAG);

    switch(table_id)
    {
        case DS_ACL_MAC_KEY:
        case DS_QOS_MAC_KEY:
        {
            ds_acl_mac_key_t ds_mac_key, ds_mac_mask;

            kal_memset(&ds_mac_key, 0, sizeof(ds_acl_mac_key_t));
            kal_memset(&ds_mac_mask, 0, sizeof(ds_acl_mac_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_build_ds_mac_key(&p_key->key_info.mac_key, &ds_mac_key, &ds_mac_mask));

            ds_key.data_entry = (uint32 *)&ds_mac_key;
            ds_key.mask_entry = (uint32 *)&ds_mac_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        case DS_ACL_IPV4_KEY:
        case DS_QOS_IPV4_KEY:
        {
            ds_acl_ipv4_key_t ds_ipv4_key, ds_ipv4_mask;

            kal_memset(&ds_ipv4_key, 0, sizeof(ds_acl_ipv4_key_t));
            kal_memset(&ds_ipv4_mask, 0, sizeof(ds_acl_ipv4_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_build_ds_ipv4_key(&p_key->key_info.ipv4_key, p_info, &ds_ipv4_key, &ds_ipv4_mask));

            ds_key.data_entry = (uint32 *)&ds_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        case DS_ACL_MPLS_KEY:
        case DS_QOS_MPLS_KEY:
        {
            ds_acl_mpls_key_t ds_mpls_key, ds_mpls_mask;

            kal_memset(&ds_mpls_key, 0, sizeof(ds_acl_mpls_key_t));
            kal_memset(&ds_mpls_mask, 0, sizeof(ds_acl_mpls_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_build_ds_mpls_key(&p_key->key_info.mpls_key, &ds_mpls_key, &ds_mpls_mask));

            ds_key.data_entry = (uint32 *)&ds_mpls_key;
            ds_key.mask_entry = (uint32 *)&ds_mpls_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        case DS_ACL_IPV6_KEY:
        case DS_QOS_IPV6_KEY:
        {
            ds_acl_ipv6_key_t ds_ipv6_key, ds_ipv6_mask;

            kal_memset(&ds_ipv6_key, 0, sizeof(ds_acl_ipv6_key_t));
            kal_memset(&ds_ipv6_mask, 0, sizeof(ds_acl_ipv6_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_build_ds_ipv6_key(&p_key->key_info.ipv6_key, p_info, &ds_ipv6_key, &ds_ipv6_mask));

            ds_key.data_entry = (uint32 *)&ds_ipv6_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv6_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        case DS_IPV4_PBR_DUALDA_KEY:
        {
            ds_ipv4_pbr_dualda_key_t ds_pbr_ipv4_key, ds_pbr_ipv4_mask;

            kal_memset(&ds_pbr_ipv4_key, 0, sizeof(ds_ipv4_pbr_dualda_key_t));
            kal_memset(&ds_pbr_ipv4_mask, 0, sizeof(ds_ipv4_pbr_dualda_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_acl_pbr_build_ds_ipv4_key(&p_key->key_info.pbr_ipv4_key, p_info, &ds_pbr_ipv4_key, &ds_pbr_ipv4_mask));

            ds_key.data_entry = (uint32 *)&ds_pbr_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_pbr_ipv4_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        case DS_IPV6_PBR_DUALDA_KEY:
        {
            ds_ipv6_pbr_dualda_key_t ds_pbr_ipv6_key, ds_pbr_ipv6_mask;

            kal_memset(&ds_pbr_ipv6_key, 0, sizeof(ds_ipv6_pbr_dualda_key_t));
            kal_memset(&ds_pbr_ipv6_mask, 0, sizeof(ds_ipv6_pbr_dualda_key_t));
            CTC_ERROR_RETURN(
                _sys_humber_acl_pbr_build_ds_ipv6_key(&p_key->key_info.pbr_ipv6_key, p_info, &ds_pbr_ipv6_key, &ds_pbr_ipv6_mask));

            ds_key.data_entry = (uint32 *)&ds_pbr_ipv6_key;
            ds_key.mask_entry = (uint32 *)&ds_pbr_ipv6_mask;

            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_info->offset, cmd_key, &ds_key));
            break;
        }

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


/**
 @brief delete acl/qos entry from chip
*/
static int32
_sys_humber_aclqos_delete_entry_from_chip(uint8 lchip, sys_aclqos_label_t *p_label,
                                          ctc_aclqos_key_type_t key_type,
                                          uint32 offset)
{
    tbl_id_t table_id;
    ds_mac_acl_t ds_action;
    uint32 cmd;

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* remove acl/qos key */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_key_table_id(p_label->type, key_type, &table_id));

    CTC_ERROR_RETURN(drv_tcam_tbl_remove(lchip, table_id, offset));

    /* remove acl/qos action */
    CTC_ERROR_RETURN(_sys_humber_aclqos_get_action_table_id(p_label->type, key_type, &table_id));

    kal_memset(&ds_action, 0, sizeof(ds_action));
    cmd = DRV_IOW(IOC_TABLE, table_id, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, offset, cmd, &ds_action));

    return CTC_E_NONE;
}

/**
 @brief write entry to chip
*/
static int32
_sys_humber_aclqos_write_entry_to_chip(uint8 lchip, sys_aclqos_label_t* p_label,
                                       sys_aclqos_entry_t* p_entry,
                                       sys_aclqos_sub_entry_info_t* p_info)
{
    tbl_id_t action_tbl_id, key_tbl_id;

    CTC_PTR_VALID_CHECK(p_entry);
    CTC_PTR_VALID_CHECK(p_info);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* write acl/qos action */
    CTC_ERROR_RETURN(
        _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

    if (IS_PBR_TABLE(p_label->type))
    {
        CTC_ERROR_RETURN(
            _sys_humber_acl_pbr_write_action_to_chip(lchip, action_tbl_id, &p_entry->action, p_info));
    }
    else
    {
        CTC_ERROR_RETURN(
            _sys_humber_aclqos_write_action_to_chip(lchip, action_tbl_id, &p_entry->action, p_info));
    }

    /* write acl/qos key */
    CTC_ERROR_RETURN(
        _sys_humber_aclqos_get_key_table_id(p_label->type, p_entry->key.type, &key_tbl_id));

    CTC_ERROR_RETURN(
        _sys_humber_aclqos_write_key_to_chip(lchip, key_tbl_id, &p_entry->key, p_info));

    return CTC_E_NONE;
}

/**
 @brief remove all sub entries in an acl/qos entry
*/
static int32
_sys_humber_aclqos_remove_sub_entry(uint8 lchip, sys_aclqos_label_t* p_label,
                                    sys_aclqos_entry_t* p_entry)
{
    if (0 == p_entry->action.flag.invalid)
    {
        _sys_humber_aclqos_delete_entry_from_chip(lchip, p_label, p_entry->key.type, p_entry->block_index);
    }

    return CTC_E_NONE;
}
/**
 @brief add acl/qos entry into entry list in the given label
*/
static int32
_sys_humber_aclqos_add_entry_to_db(sys_acl_block_t* pb, sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_label_index_t* p_index = NULL;
    ctc_list_pointer_t *p_list = NULL;
    sys_aclqos_label_t* p_label;

    p_label = p_entry->p_label;
    p_index = p_label->p_index[0];
    p_list = &p_index->entry_list[p_entry->key.type];
    ctc_list_pointer_insert_tail(p_list, &p_entry->head);
    
    /* add to hash */
    ctc_hash_insert(acl_master->entry, p_entry);
    /* add to block */
    pb->entries[p_entry->block_index] = p_entry;

    /* free_count-- */
    /* ingore the default entry */
    if (p_entry->block_index < pb->entry_count)
    {
        (pb->free_count)--;
    }

    return CTC_E_NONE;
}

/**
 @brief remove acl/qos entry from entry list in the given label
*/
static int32
_sys_humber_aclqos_remove_entry_from_db(sys_acl_block_t* pb, sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_label_index_t* p_index = NULL;
    ctc_list_pointer_t *p_list = NULL;
    sys_aclqos_label_t* p_label;
    sys_humber_opf_t opf;
    uint32 ds_fwd_offset = 0;
    sys_acl_redirect_t acl_redirect;
    sys_acl_redirect_t* p_acl_redirect = 0;

    p_label = p_entry->p_label;
    p_index = p_label->p_index[0];
    p_list = &p_index->entry_list[p_entry->key.type];

    /* unbind flow policer */
    if (p_entry->action.flag.flow_policer)
    {
        sys_humber_qos_flow_policer_unbind(pb->lchip, p_entry->action.policer_id);
    }

    /* free acl_fwd_ptr, It's no need to free sram */
    if (p_entry->action.flag.fwd)
    {
        acl_redirect.nhid = p_entry->action.fwd.fwd_nh_id;
        p_acl_redirect = ctc_hash_lookup(p_sys_acl_redirect_hash[pb->lchip], &acl_redirect);
        if(p_acl_redirect)
        {
            p_acl_redirect->ref --;

            if(p_acl_redirect->ref == 0)
            {
                opf.pool_type = OPF_ACL_FWD_SRAM;
                opf.pool_index = pb->lchip;
                ds_fwd_offset = p_entry->action.ds_fwd_ptr;
                CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset));

                ctc_hash_remove(p_sys_acl_redirect_hash[pb->lchip], p_acl_redirect);
                mem_free(p_acl_redirect);
            }
        }
        else
        {
            return CTC_E_ACLQOS_INVALID_ACTION;
        }
    }

    /* remove from hash */
    ctc_hash_remove(acl_master->entry, p_entry);

    /* remove from block */
    pb->entries[p_entry->block_index] = NULL;

    /* free_count++ */
    /* ingore the default entry */
    if (p_entry->block_index < pb->entry_count)
    {
        (pb->free_count)++;
    }
    
    if (p_entry->head.p_prev)
    {
        ctc_list_pointer_delete(p_list, &p_entry->head);
    }
    mem_free(p_entry);

    return CTC_E_NONE;
}


static int32
_sys_humber_aclqos_entry_write(sys_acl_block_t* pb, sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_sub_entry_info_t info;

    CTC_PTR_VALID_CHECK(p_entry);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    memset(&info, 0, sizeof(info));
    info.offset = p_entry->block_index;

    CTC_ERROR_RETURN(
        _sys_humber_aclqos_write_entry_to_chip(0, p_entry->p_label, p_entry, &info));

    /* add to db */
    CTC_ERROR_RETURN(_sys_humber_aclqos_add_entry_to_db(pb, p_entry));

    return CTC_E_NONE;
}

/**
 @brief remove acl/qos entry
*/
static int32
_sys_humber_aclqos_entry_remove(sys_acl_block_t* pb, sys_aclqos_entry_t* p_entry)
{
    CTC_PTR_VALID_CHECK(p_entry);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    /* remove all sub entries from tcam and db */
    CTC_ERROR_RETURN(_sys_humber_aclqos_remove_sub_entry(0, p_entry->p_label, p_entry));

    /* remove entry from db */
    CTC_ERROR_RETURN(_sys_humber_aclqos_remove_entry_from_db(pb, p_entry));

    return CTC_E_NONE;
}



/**
 @brief show the given mac entry content
*/
int32
_sys_humber_show_aclqos_mac_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_aclqos_mac_key_t *p_key;

    CTC_PTR_VALID_CHECK(p_entry);

    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.mac_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");
    if (p_action->flag.discard)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("deny");
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("permit");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa");
    if (p_key->flag.macsa)
    {
        if ((0xFFFF == *(uint16 *)&p_key->mac_sa_mask[0]) &&
            (0xFFFFFFFF == *(uint32 *)&p_key->mac_sa_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" host %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                       p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5]);
        }
        else if ((0 == *(uint16 *)&p_key->mac_sa_mask[0]) &&
                 (0 == *(uint32 *)&p_key->mac_sa_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                       p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5],
                       p_key->mac_sa_mask[0], p_key->mac_sa_mask[1], p_key->mac_sa_mask[2],
                       p_key->mac_sa_mask[3], p_key->mac_sa_mask[4], p_key->mac_sa_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" any");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da");
    if (p_key->flag.macda)
    {
        if ((0xFFFF == *(uint16 *)&p_key->mac_da_mask[0]) &&
            (0xFFFFFFFF == *(uint32 *)&p_key->mac_da_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" host %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                       p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5]);
        }
        else if ((0 == *(uint16 *)&p_key->mac_da_mask[0]) &&
                 (0 == *(uint32 *)&p_key->mac_da_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                       p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5],
                       p_key->mac_da_mask[0], p_key->mac_da_mask[1], p_key->mac_da_mask[2],
                       p_key->mac_da_mask[3], p_key->mac_da_mask[4], p_key->mac_da_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" any");
    }

    if (p_key->flag.cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" cos %d", p_key->cos);
    }
    if (p_key->flag.cvlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-vlan %d mask :%d", p_key->cvlan, p_key->cvlan_mask);
    }
    if (p_key->flag.ctag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-cos %d", p_key->ctag_cos_cfi >> 1);
    }
    if (p_key->flag.ctag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-cfi %d", p_key->ctag_cos_cfi & 0x1);
    }
    if (p_key->flag.svlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-vlan %d mask :%d", p_key->svlan, p_key->svlan_mask);
    }
    if (p_key->flag.stag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-cos %d", p_key->stag_cos_cfi >> 1);
    }
    if (p_key->flag.stag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-cfi %d", p_key->stag_cos_cfi & 0x1);
    }
    if (p_key->flag.l2_type)
    {
        switch (p_key->l2_type)
        {
            case ACL_L2TYPE_ETH_V2:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type eth2");
                break;

            case ACL_L2TYPE_ETH_SAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type sap");
                break;

            case ACL_L2TYPE_ETH_SNAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type snap");
                break;

            case ACL_L2TYPE_PPP_2B:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type ppp2b");
                break;

            case ACL_L2TYPE_PPP_1B:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type ppp1b");
                break;

            case ACL_L2TYPE_RAW_SNAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type raw");

            default:
                break;
        }
    }
    if (p_key->flag.eth_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" eth-type %d mask :%d", p_key->eth_type, p_key->eth_type_mask);
    }
    if (p_key->flag.l3_type)
    {
        if (ACL_L3TYPE_ARP == p_key->l3_type)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l3-type arp");
        }
        else if (ACL_L3TYPE_RARP == p_key->l3_type)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l3-type rarp");
        }
    }

    if (p_action->flag.stats)
        SYS_ACLQOS_ENTRY_DBG_INFO(" stats");
    if (p_action->flag.deny_replace_cos)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-cos");

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    if (p_key->flag.acl_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->acl_label);
    }
    else if (p_key->flag.qos_label && (!sys_aclqos_entry_ctl.is_dual_aclqos_lookup))
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->qos_label);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("qos-actions: \n");
        if (p_action->flag.trust)
        {
            switch(p_action->trust)
            {
                case CTC_QOS_TRUST_PORT:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust port\n");
                    break;

                case CTC_QOS_TRUST_OUTER:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust outer\n");
                    break;

                case CTC_QOS_TRUST_COS:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust cos\n");
                    break;

                case CTC_QOS_TRUST_DSCP:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust dscp\n");
                    break;

                default:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust ip-prec\n");
                    break;
            }
        }

        if (p_action->flag.priority)
        {
            switch(p_action->color)
            {
                case CTC_QOS_COLOR_GREEN:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color green\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_YELLOW:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color yellow\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_RED:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color red\n", p_action->priority);
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        if (p_action->flag.flow_id)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("    flow-id %d\n", p_action->stats_or_flowid.flow_id);
        }

        if (p_action->flag.flow_policer)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("flow policer-id %d\n", p_action->policer_id);
        }
        if (p_action->flag.discard)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("discard\n");
        }
        if (p_action->flag.fwd)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("fwd_nh_id %d\n", p_action->fwd.fwd_nh_id);
        }
        if (p_action->flag.deny_route)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_route\n");
        }
        if (p_action->flag.deny_learning)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_learning\n");
        }
        if (p_action->flag.deny_bridge)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_bridge\n");
        }
        SYS_ACLQOS_ENTRY_DBG_INFO("qos-label: %d\n", p_key->qos_label);
    }
    if (p_key->flag.l2_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l2-qos-label: %d\n", p_key->l2_qos_label);
    }
    if (p_key->flag.l3_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l3-qos-label: %d\n", p_key->l3_qos_label);
    }


    SYS_ACLQOS_ENTRY_DBG_INFO("index:");

    SYS_ACLQOS_ENTRY_DBG_INFO(" %d", p_entry->block_index);

    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");

    return CTC_E_NONE;
}


/**
 @brief show the given ipv4 entry content
*/
int32
_sys_humber_show_aclqos_ipv4_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_aclqos_ipv4_key_t *p_key;
    uint32 addr;
    char ip_addr[16];

    CTC_PTR_VALID_CHECK(p_entry);

    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.ipv4_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");
    if (p_action->flag.discard)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("deny");
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("permit");
    }

    if (p_key->flag.ipsa)
    {
        if (0xFFFFFFFF == p_key->ip_sa_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa host");
            addr = kal_ntohl(p_key->ip_sa);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
        }
        else if (0 == p_key->ip_sa_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa");
            addr = kal_ntohl(p_key->ip_sa);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
            addr = kal_ntohl(p_key->ip_sa_mask);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO("  %s", ip_addr);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
    }

    if (p_key->flag.ipda)
    {
        if (0xFFFFFFFF == p_key->ip_da_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da host");
            addr = kal_ntohl(p_key->ip_da);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
        }
        else if (0 == p_key->ip_da_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da");
            addr = kal_ntohl(p_key->ip_da);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
            addr = kal_ntohl(p_key->ip_da_mask);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO("  %s", ip_addr);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
    }

    if (p_key->flag.dscp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  dscp %d (ip-prec %d)", p_key->dscp, p_key->dscp>>3);
    }
    if (p_key->flag.frag_info && 0 != p_key->frag_info)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  fragments");
    }
    if (p_key->flag.routed_packet)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  routed-packet");
    }
    if (p_key->flag.ip_option)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  options");
    }

    if (p_key->flag.is_tcp && p_key->flag.is_udp)
    {
        if (1 == (p_key->l4info_mapped & 0xFF))   /* icmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" icmp");
        }
        else if (2 == (p_key->l4info_mapped & 0xFF))  /* igmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" igmp");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l4-protocol %d", p_key->l4info_mapped & 0xFF);
        }
    }
    else if (p_key->flag.is_udp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  udp");
    }
    else if (p_key->flag.is_tcp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" tcp");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");
    if (p_key->flag.macsa)
    {
        if (0xFFFF == *(uint16 *)p_key->mac_sa_mask &&
            0xFFFFFFFF == *(uint32 *)(p_key->mac_sa_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa host %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5]);
        }
        else if (0 == *(uint16 *)p_key->mac_sa_mask && 0 == *(uint32 *)(p_key->mac_sa_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("  mac-sa %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5],
                p_key->mac_sa_mask[0], p_key->mac_sa_mask[1], p_key->mac_sa_mask[2],
                p_key->mac_sa_mask[3], p_key->mac_sa_mask[4], p_key->mac_sa_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa any");
    }

    if (p_key->flag.macda)
    {
        if (0xFFFF == *(uint16 *)p_key->mac_da_mask &&
            0xFFFFFFFF == *(uint32 *)(p_key->mac_da_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da host %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5]);
        }
        else if (0 == *(uint16 *)p_key->mac_da_mask && 0 == *(uint32 *)(p_key->mac_da_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("  mac-da %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5],
                p_key->mac_da_mask[0], p_key->mac_da_mask[1], p_key->mac_da_mask[2],
                p_key->mac_da_mask[3], p_key->mac_da_mask[4], p_key->mac_da_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da any");
    }

    if (p_key->flag.cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" cos %d", p_key->cos);
    }
    if (p_key->flag.cvlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-vlan %d mask :%d", p_key->cvlan, p_key->cvlan_mask);
    }
    if (p_key->flag.ctag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-cos %d", p_key->ctag_cos_cfi >> 1);
    }
    if (p_key->flag.ctag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-cfi %d", p_key->ctag_cos_cfi & 1);
    }
    if (p_key->flag.svlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-vlan %d mask :%d", p_key->svlan, p_key->svlan_mask);
    }
    if (p_key->flag.stag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-cos %d", p_key->stag_cos_cfi >> 1);
    }
    if (p_key->flag.stag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-cfi %d", p_key->stag_cos_cfi & 1);
    }
    if (p_key->flag.l2_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  layer2-type %d", p_key->l2_type);
    }
    if (p_key->flag.l3_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" layer3-type %d", p_key->l3_type);
    }

    if (p_action->flag.stats)
        SYS_ACLQOS_ENTRY_DBG_INFO(" stats");
    if (p_action->flag.deny_replace_cos)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-cos");
    if (p_action->flag.deny_replace_dscp)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-dscp");

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    if (p_key->flag.acl_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->acl_label);
    }
    else if (p_key->flag.qos_label && !sys_aclqos_entry_ctl.is_dual_aclqos_lookup)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->qos_label);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("qos-actions: \n");
        if (p_action->flag.trust)
        {
            switch(p_action->trust)
            {
                case CTC_QOS_TRUST_PORT:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust port\n");
                    break;

                case CTC_QOS_TRUST_OUTER:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust outer\n");
                    break;

                case CTC_QOS_TRUST_COS:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust cos\n");
                    break;

                case CTC_QOS_TRUST_DSCP:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust dscp\n");
                    break;

                default:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust ip-prec\n");
                    break;
            }
        }

        if (p_action->flag.priority)
        {
            switch(p_action->color)
            {
                case CTC_QOS_COLOR_GREEN:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color green\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_YELLOW:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color yellow\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_RED:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color red\n", p_action->priority);
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        if (p_action->flag.flow_id)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("    flow-id %d\n", p_action->stats_or_flowid.flow_id);
        }

        if (p_action->flag.flow_policer)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("flow policer-id %d\n", p_action->policer_id);
        }
        
        if (p_action->flag.discard)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("discard\n");
        }
        if (p_action->flag.fwd)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("fwd_nh_id %d\n", p_action->fwd.fwd_nh_id);
        }
        if (p_action->flag.deny_route)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_route\n");
        }
        if (p_action->flag.deny_learning)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_learning\n");
        }
        if (p_action->flag.deny_bridge)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("deny_bridge\n");
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("qos-label: %d\n", p_key->qos_label);
    }
    if (p_key->flag.l2_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l2-qos-label: %d\n", p_key->l2_qos_label);
    }
    if (p_key->flag.l3_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l3-qos-label: %d\n", p_key->l3_qos_label);
    }


    SYS_ACLQOS_ENTRY_DBG_INFO("index:");

    SYS_ACLQOS_ENTRY_DBG_INFO(" %d", p_entry->block_index);

    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");

    return CTC_E_NONE;
}

/**
 @brief show the given pbr ipv4 entry content
*/
int32
_sys_humber_show_acl_pbr_ipv4_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_acl_pbr_ipv4_key_t *p_key;
    uint32 addr;
    char ip_addr[16];

    CTC_PTR_VALID_CHECK(p_entry);

    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.pbr_ipv4_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");

    if (p_key->flag.ipsa)
    {
        if (0xFFFFFFFF == p_key->ip_sa_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa host");
            addr = kal_ntohl(p_key->ip_sa);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
        }
        else if (0 == p_key->ip_sa_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa");
            addr = kal_ntohl(p_key->ip_sa);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
            addr = kal_ntohl(p_key->ip_sa_mask);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO("  %s", ip_addr);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
    }

    if (p_key->flag.ipda)
    {
        if (0xFFFFFFFF == p_key->ip_da_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da host");
            addr = kal_ntohl(p_key->ip_da);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
        }
        else if (0 == p_key->ip_da_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da");
            addr = kal_ntohl(p_key->ip_da);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO(" %s", ip_addr);
            addr = kal_ntohl(p_key->ip_da_mask);
            kal_inet_ntop(AF_INET, &addr, ip_addr, sizeof(ip_addr));
            SYS_ACLQOS_ENTRY_DBG_INFO("  %s", ip_addr);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
    }

    if (p_key->flag.dscp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  dscp %d (ip-prec %d)", p_key->dscp, p_key->dscp>>3);
    }
    if (p_key->flag.frag_info && 0 != p_key->frag_info)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  fragments");
    }
    if (p_key->flag.ip_option)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  options");
    }

    if (p_key->flag.is_tcp && p_key->flag.is_udp)
    {
        if (1 == (p_key->l4info_mapped & 0xFF))   /* icmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" icmp");
        }
        else if (2 == (p_key->l4info_mapped & 0xFF))  /* igmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" igmp");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l4-protocol %d", p_key->l4info_mapped & 0xFF);
        }
    }
    else if (p_key->flag.is_udp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  udp");
    }
    else if (p_key->flag.is_tcp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" tcp");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    SYS_ACLQOS_ENTRY_DBG_INFO(" acl-pbr-label: %d\n", p_key->pbr_label);

    SYS_ACLQOS_ENTRY_DBG_INFO(" pbr action:\n");

    if ((p_action->flag.pbr_ecmp) || (p_action->pbr_ecpn))
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" fwd-to-ecmp-nhid %d\n", p_action->fwd.fwd_nh_id);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" fwd-to-nhid %d\n", p_action->fwd.fwd_nh_id);
    }

    if(p_action->flag.pbr_ttl_check)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ttl-check\n");
    }

    if(p_action->flag.pbr_icmp_check)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" icmp-check\n");
    }

    if(p_action->flag.pbr_copy_to_cpu)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" copy_to_cpu\n");
    }

    if(p_action->flag.pbr_deny)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-pbr\n");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("index:");


    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");

    return CTC_E_NONE;
}

/**
 @brief show the given ipv6 entry content
*/
int32
_sys_humber_show_aclqos_ipv6_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_aclqos_ipv6_key_t *p_key;
    char buf[CTC_IPV6_ADDR_STR_LEN];
    ipv6_addr_t ipv6_addr;


    CTC_PTR_VALID_CHECK(p_entry);

    kal_memset(buf, 0, CTC_IPV6_ADDR_STR_LEN);
    kal_memset(ipv6_addr, 0, sizeof(ipv6_addr_t));
    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.ipv6_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");
    if (p_action->flag.discard)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("deny");
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("permit");
    }

    if (p_key->flag.ipsa)
    {
        if ((0xFFFFFFFF == p_key->ip_sa_mask[0])&&(0xFFFFFFFF == p_key->ip_sa_mask[1]) \
            &&(0xFFFFFFFF == p_key->ip_sa_mask[2])&&(0xFFFFFFFF == p_key->ip_sa_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa host ");

            ipv6_addr[0] = kal_htonl(p_key->ip_sa[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
        else if ((0 == p_key->ip_sa_mask[0])&&(0 == p_key->ip_sa_mask[1]) \
            &&(0 == p_key->ip_sa_mask[2])&&(0 == p_key->ip_sa_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa");
            ipv6_addr[0] = kal_htonl(p_key->ip_sa[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa[3]);
            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s ", buf);

            ipv6_addr[0] = kal_htonl(p_key->ip_sa_mask[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa_mask[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa_mask[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa_mask[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
    }

    kal_memset(buf, 0, CTC_IPV6_ADDR_STR_LEN);

    if (p_key->flag.ipda)
    {
        if ((0xFFFFFFFF == p_key->ip_da_mask[0])&&(0xFFFFFFFF == p_key->ip_da_mask[1]) \
            &&(0xFFFFFFFF == p_key->ip_da_mask[2])&&(0xFFFFFFFF == p_key->ip_da_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da host ");

            ipv6_addr[0] = kal_htonl(p_key->ip_da[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
        else if ((0 == p_key->ip_da_mask[0])&&(0 == p_key->ip_da_mask[1]) \
            &&(0 == p_key->ip_da_mask[2])&&(0 == p_key->ip_da_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da");
            ipv6_addr[0] = kal_htonl(p_key->ip_da[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da[3]);
            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s ", buf);

            ipv6_addr[0] = kal_htonl(p_key->ip_da_mask[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da_mask[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da_mask[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da_mask[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
    }

    if (p_key->flag.dscp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  dscp %d (ip-prec %d)", p_key->dscp, p_key->dscp>>3);
    }
    if (p_key->flag.frag_info && 0 != p_key->frag_info)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  fragments");
    }
    if (p_key->flag.routed_packet)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  routed-packet");
    }
    if (p_key->flag.ip_option)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  options");
    }
    if(p_key->flag.flow_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  flow-label:%d",p_key->flow_label);
    }
    if(p_key->flag.ext_hdr)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  extension-head:%d",p_key->ext_hdr);
    }

    if (p_key->flag.is_tcp && p_key->flag.is_udp)
    {
        if (1 == (p_key->l4info_mapped & 0xFF))   /* icmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" icmp");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l4-protocol %d", p_key->l4info_mapped & 0xFF);
        }
    }
    else if (p_key->flag.is_udp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  udp");
    }
    else if (p_key->flag.is_tcp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" tcp");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");
    if (p_key->flag.macsa)
    {
        if (0xFFFF == *(uint16 *)p_key->mac_sa_mask &&
            0xFFFFFFFF == *(uint32 *)(p_key->mac_sa_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa host %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5]);
        }
        else if (0 == *(uint16 *)p_key->mac_sa_mask && 0 == *(uint32 *)(p_key->mac_sa_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("  mac-sa %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5],
                p_key->mac_sa_mask[0], p_key->mac_sa_mask[1], p_key->mac_sa_mask[2],
                p_key->mac_sa_mask[3], p_key->mac_sa_mask[4], p_key->mac_sa_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa any");
    }

    if (p_key->flag.macda)
    {
        if (0xFFFF == *(uint16 *)p_key->mac_da_mask &&
            0xFFFFFFFF == *(uint32 *)(p_key->mac_da_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da host %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5]);
        }
        else if (0 == *(uint16 *)p_key->mac_da_mask && 0 == *(uint32 *)(p_key->mac_da_mask+2))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("  mac-da %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5],
                p_key->mac_da_mask[0], p_key->mac_da_mask[1], p_key->mac_da_mask[2],
                p_key->mac_da_mask[3], p_key->mac_da_mask[4], p_key->mac_da_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da any");
    }

    if (p_key->flag.cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" cos %d", p_key->cos);
    }
    if (p_key->flag.cvlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-vlan %d mask :%d", p_key->cvlan, p_key->cvlan_mask);
    }
    if (p_key->flag.ctag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-cos %d", p_key->ctag_cos_cfi >> 1);
    }
    if (p_key->flag.ctag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  c-cfi %d", p_key->ctag_cos_cfi & 1);
    }
    if (p_key->flag.svlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-vlan %d mask :%d", p_key->svlan, p_key->svlan_mask);
    }
    if (p_key->flag.stag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-cos %d", p_key->stag_cos_cfi >> 1);
    }
    if (p_key->flag.stag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  s-cfi %d", p_key->stag_cos_cfi & 1);
    }
    if (p_key->flag.l2_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  layer2-type %d", p_key->l2_type);
    }
    if (p_key->flag.l3_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" layer3-type %d", p_key->l3_type);
    }

    if (p_action->flag.stats)
        SYS_ACLQOS_ENTRY_DBG_INFO(" stats");
    if (p_action->flag.deny_replace_cos)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-cos");
    if (p_action->flag.deny_replace_dscp)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-dscp");

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    if (p_key->flag.acl_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->acl_label);
    }
    else if (p_key->flag.qos_label && !sys_aclqos_entry_ctl.is_dual_aclqos_lookup)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->qos_label);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("qos-actions: \n");
        if (p_action->flag.trust)
        {
            switch(p_action->trust)
            {
                case CTC_QOS_TRUST_PORT:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust port\n");
                    break;

                case CTC_QOS_TRUST_OUTER:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust outer\n");
                    break;

                case CTC_QOS_TRUST_COS:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust cos\n");
                    break;

                case CTC_QOS_TRUST_DSCP:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust dscp\n");
                    break;

                default:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust ip-prec\n");
                    break;
            }
        }

        if (p_action->flag.priority)
        {
            switch(p_action->color)
            {
                case CTC_QOS_COLOR_GREEN:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color green\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_YELLOW:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color yellow\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_RED:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color red\n", p_action->priority);
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        if (p_action->flag.flow_id)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("    flow-id %d\n", p_action->stats_or_flowid.flow_id);
        }

        if (p_action->flag.flow_policer)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("flow policer-id %d\n", p_action->policer_id);
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("qos-label: %d\n", p_key->qos_label);
    }
    if (p_key->flag.l2_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l2-qos-label: %d\n", p_key->l2_qos_label);
    }
    if (p_key->flag.l3_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l3-qos-label: %d\n", p_key->l3_qos_label);
    }


    SYS_ACLQOS_ENTRY_DBG_INFO("index:");


    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");

    return CTC_E_NONE;
}

/**
 @brief show the given ipv6 entry content
*/
int32
_sys_humber_show_acl_pbr_ipv6_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_acl_pbr_ipv6_key_t *p_key;
    char buf[CTC_IPV6_ADDR_STR_LEN];
    ipv6_addr_t ipv6_addr;


    CTC_PTR_VALID_CHECK(p_entry);

    kal_memset(buf, 0, CTC_IPV6_ADDR_STR_LEN);
    kal_memset(ipv6_addr, 0, sizeof(ipv6_addr_t));
    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.pbr_ipv6_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");

    if (p_key->flag.ipsa)
    {
        if ((0xFFFFFFFF == p_key->ip_sa_mask[0])&&(0xFFFFFFFF == p_key->ip_sa_mask[1]) \
            &&(0xFFFFFFFF == p_key->ip_sa_mask[2])&&(0xFFFFFFFF == p_key->ip_sa_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa host ");

            ipv6_addr[0] = kal_htonl(p_key->ip_sa[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
        else if ((0 == p_key->ip_sa_mask[0])&&(0 == p_key->ip_sa_mask[1]) \
            &&(0 == p_key->ip_sa_mask[2])&&(0 == p_key->ip_sa_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa");
            ipv6_addr[0] = kal_htonl(p_key->ip_sa[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa[3]);
            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s ", buf);

            ipv6_addr[0] = kal_htonl(p_key->ip_sa_mask[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_sa_mask[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_sa_mask[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_sa_mask[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-sa any");
    }

    kal_memset(buf, 0, CTC_IPV6_ADDR_STR_LEN);

    if (p_key->flag.ipda)
    {
        if ((0xFFFFFFFF == p_key->ip_da_mask[0])&&(0xFFFFFFFF == p_key->ip_da_mask[1]) \
            &&(0xFFFFFFFF == p_key->ip_da_mask[2])&&(0xFFFFFFFF == p_key->ip_da_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da host ");

            ipv6_addr[0] = kal_htonl(p_key->ip_da[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
        else if ((0 == p_key->ip_da_mask[0])&&(0 == p_key->ip_da_mask[1]) \
            &&(0 == p_key->ip_da_mask[2])&&(0 == p_key->ip_da_mask[3]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da");
            ipv6_addr[0] = kal_htonl(p_key->ip_da[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da[3]);
            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s ", buf);

            ipv6_addr[0] = kal_htonl(p_key->ip_da_mask[0]);
            ipv6_addr[1] = kal_htonl(p_key->ip_da_mask[1]);
            ipv6_addr[2] = kal_htonl(p_key->ip_da_mask[2]);
            ipv6_addr[3] = kal_htonl(p_key->ip_da_mask[3]);

            kal_inet_ntop(AF_INET6, ipv6_addr, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_ACLQOS_ENTRY_DBG_INFO("%20s", buf);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ip-da any");
    }

    if (p_key->flag.dscp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  dscp %d (ip-prec %d)", p_key->dscp, p_key->dscp>>3);
    }
    if (p_key->flag.frag_info && 0 != p_key->frag_info)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  fragments");
    }
    if (p_key->flag.ip_option)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  options");
    }
    if(p_key->flag.flow_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  flow-label:%d",p_key->flow_label);
    }
    if(p_key->flag.ext_hdr)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  extension-head:%d",p_key->ext_hdr);
    }

    if (p_key->flag.is_tcp && p_key->flag.is_udp)
    {
        if (1 == (p_key->l4info_mapped & 0xFF))   /* icmp */
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" icmp");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" l4-protocol %d", p_key->l4info_mapped & 0xFF);
        }
    }
    else if (p_key->flag.is_udp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("  udp");
    }
    else if (p_key->flag.is_tcp)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" tcp");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    SYS_ACLQOS_ENTRY_DBG_INFO(" acl-pbr-label: %d\n", p_key->pbr_label);

    SYS_ACLQOS_ENTRY_DBG_INFO(" pbr action:\n");

    if ((p_action->flag.pbr_ecmp) || (p_action->pbr_ecpn))
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" fwd-to-ecmp-nhid %d\n", p_action->fwd.fwd_nh_id);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" fwd-to-nhid %d\n", p_action->fwd.fwd_nh_id);
    }

    if(p_action->flag.pbr_ttl_check)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" ttl-check\n");
    }

    if(p_action->flag.pbr_icmp_check)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" icmp-check\n");
    }

    if(p_action->flag.pbr_copy_to_cpu)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" copy_to_cpu\n");
    }

    if(p_action->flag.pbr_deny)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-pbr\n");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO("index:");


    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");

    return CTC_E_NONE;
}

/**
 @brief show the given mpls entry content
*/
int32
_sys_humber_show_aclqos_mpls_entry(sys_aclqos_entry_t* p_entry)
{
    sys_aclqos_action_t *p_action;
    sys_aclqos_mpls_key_t *p_key;


    CTC_PTR_VALID_CHECK(p_entry);

    p_action = &p_entry->action;
    p_key = &p_entry->key.key_info.mpls_key;

    SYS_ACLQOS_ENTRY_DBG_INFO("entry_id = %u\n", p_entry->entry_id);
    SYS_ACLQOS_ENTRY_DBG_INFO("=======================================\n");
    if (p_action->flag.discard)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("deny");
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("permit");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO(" mac-sa");
    if (p_key->flag.macsa)
    {
        if ((0xFFFF == *(uint16 *)&p_key->mac_sa_mask[0]) &&
            (0xFFFFFFFF == *(uint32 *)&p_key->mac_sa_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" host %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                       p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5]);
        }
        else if ((0 == *(uint16 *)&p_key->mac_sa_mask[0]) &&
                 (0 == *(uint32 *)&p_key->mac_sa_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_sa[0], p_key->mac_sa[1], p_key->mac_sa[2],
                       p_key->mac_sa[3], p_key->mac_sa[4], p_key->mac_sa[5],
                       p_key->mac_sa_mask[0], p_key->mac_sa_mask[1], p_key->mac_sa_mask[2],
                       p_key->mac_sa_mask[3], p_key->mac_sa_mask[4], p_key->mac_sa_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" any");
    }

    SYS_ACLQOS_ENTRY_DBG_INFO(" mac-da");
    if (p_key->flag.macda)
    {
        if ((0xFFFF == *(uint16 *)&p_key->mac_da_mask[0]) &&
            (0xFFFFFFFF == *(uint32 *)&p_key->mac_da_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" host %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                       p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5]);
        }
        else if ((0 == *(uint16 *)&p_key->mac_da_mask[0]) &&
                 (0 == *(uint32 *)&p_key->mac_da_mask[2]))
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" %02x%02x.%02x%02x.%02x%02x %02x%02x.%02x%02x.%02x%02x",
                       p_key->mac_da[0], p_key->mac_da[1], p_key->mac_da[2],
                       p_key->mac_da[3], p_key->mac_da[4], p_key->mac_da[5],
                       p_key->mac_da_mask[0], p_key->mac_da_mask[1], p_key->mac_da_mask[2],
                       p_key->mac_da_mask[3], p_key->mac_da_mask[4], p_key->mac_da_mask[5]);
        }
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" any");
    }

    if (p_key->flag.cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" cos %d", p_key->cos);
    }
    if (p_key->flag.cvlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-vlan %d mask :%d", p_key->cvlan, p_key->cvlan_mask);
    }
    if (p_key->flag.ctag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-cos %d", p_key->ctag_cos_cfi >> 1);
    }
    if (p_key->flag.ctag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" c-cfi %d", p_key->ctag_cos_cfi & 1);
    }
    if (p_key->flag.svlan)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-vlan %d mask :%d", p_key->svlan, p_key->svlan_mask);
    }
    if (p_key->flag.stag_cos)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-cos %d", p_key->stag_cos_cfi >> 1);
    }
    if (p_key->flag.stag_cfi)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" s-cfi %d", p_key->stag_cos_cfi & 1);
    }
    if (p_key->flag.l2_type)
    {
        switch (p_key->l2_type)
        {
            case ACL_L2TYPE_ETH_V2:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type eth2");
                break;

            case ACL_L2TYPE_ETH_SAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type sap");
                break;

            case ACL_L2TYPE_ETH_SNAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type snap");
                break;

            case ACL_L2TYPE_PPP_2B:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type ppp2b");
                break;

            case ACL_L2TYPE_PPP_1B:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type ppp1b");
                break;

            case ACL_L2TYPE_RAW_SNAP:
                SYS_ACLQOS_ENTRY_DBG_INFO(" l2-type raw");

            default:
                break;
        }
    }

    if(p_key->flag.routed_packet)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO(" routed-packet");
    }

    if(p_key->flag.label0)
    {
        if(0 == p_key->mpls_label0_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label0 any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label0:%d",(p_key->mpls_label0>>12)&0xFFFFF);
        }
    }
    if(p_key->flag.label1)
    {
        if(0 == p_key->mpls_label1_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label1 any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label1:%d",(p_key->mpls_label1>>12)&0xFFFFF);
        }
    }
    if(p_key->flag.label2)
    {
        if(0 == p_key->mpls_label2_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label2 any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label2:%d",(p_key->mpls_label2>>12)&0xFFFFF);
        }
    }
    if(p_key->flag.label3)
    {
        if(0 == p_key->mpls_label3_mask)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label3 any");
        }
        else
        {
            SYS_ACLQOS_ENTRY_DBG_INFO(" mpls-label3:%d",(p_key->mpls_label3>>12)&0xFFFFF);
        }
    }

    if (p_action->flag.stats)
        SYS_ACLQOS_ENTRY_DBG_INFO(" stats");
    if (p_action->flag.deny_replace_cos)
        SYS_ACLQOS_ENTRY_DBG_INFO(" deny-replace-cos");

    SYS_ACLQOS_ENTRY_DBG_INFO("\n");

    if (p_key->flag.acl_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->acl_label);
    }
    else if (p_key->flag.qos_label && !sys_aclqos_entry_ctl.is_dual_aclqos_lookup)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("acl-label: %d\n", p_key->qos_label);
    }
    else
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("qos-actions: \n");
        if (p_action->flag.trust)
        {
            switch(p_action->trust)
            {
                case CTC_QOS_TRUST_PORT:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust port\n");
                    break;

                case CTC_QOS_TRUST_OUTER:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust outer\n");
                    break;

                case CTC_QOS_TRUST_COS:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust cos\n");
                    break;

                case CTC_QOS_TRUST_DSCP:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust dscp\n");
                    break;

                default:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    trust ip-prec\n");
                    break;
            }
        }

        if (p_action->flag.priority)
        {
            switch(p_action->color)
            {
                case CTC_QOS_COLOR_GREEN:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color green\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_YELLOW:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color yellow\n", p_action->priority);
                    break;

                case CTC_QOS_COLOR_RED:
                    SYS_ACLQOS_ENTRY_DBG_INFO("    priority %d color red\n", p_action->priority);
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        if (p_action->flag.flow_id)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("    flow-id %d\n", p_action->stats_or_flowid.flow_id);
        }

        if (p_action->flag.flow_policer)
        {
            SYS_ACLQOS_ENTRY_DBG_INFO("flow policer-id %d\n", p_action->policer_id);
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("qos-label: %d\n", p_key->qos_label);
    }
    if (p_key->flag.l2_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l2-qos-label: %d\n", p_key->l2_qos_label);
    }
    if (p_key->flag.l3_qos_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("l3-qos-label: %d\n", p_key->l3_qos_label);
    }


    SYS_ACLQOS_ENTRY_DBG_INFO("index:");


    SYS_ACLQOS_ENTRY_DBG_INFO("\n\n");
    return CTC_E_NONE;
}




#define DO_REORDER(move_num, all)  ((move_num) >= ((all) / g_sys_humber_acl_reorder_ratio))

uint16 g_sys_humber_acl_reorder_ratio = 100;

/**
 @brief Acl global entry hash key hook.
*/
static INLINE uint32
_sys_humber_acl_entry_hash_key(void* data)
{
    sys_acl_entry_t* p_entry = (sys_acl_entry_t *)data;

    return p_entry->entry_id;
}

/**
 @brief Acl global entry hash comparison hook.
*/
static INLINE bool
_sys_humber_acl_entry_hash_cmp(void* data1, void* data2)
{
    sys_acl_entry_t *p_entry1 = (sys_acl_entry_t *)data1;
    sys_acl_entry_t *p_entry2 = (sys_acl_entry_t *)data2;

    if (p_entry1->entry_id == p_entry2->entry_id)
    {
        return TRUE;
    }

    return FALSE;
}

/*
 *get sys entry node by entry id
 */
static int32
_sys_humber_acl_get_sys_entry_by_eid(uint32 eid, sys_acl_entry_t** sys_entry_out)
{
    sys_acl_entry_t* p_sys_entry_lkup = NULL;
    sys_acl_entry_t sys_entry;

    CTC_PTR_VALID_CHECK(sys_entry_out);
    SYS_ACL_DBG_FUNC();

    memset(&sys_entry, 0, sizeof(sys_acl_entry_t));
    sys_entry.entry_id = eid;

    p_sys_entry_lkup = ctc_hash_lookup(acl_master->entry, &sys_entry);

    *sys_entry_out = p_sys_entry_lkup;

    return CTC_E_NONE;
}

/*
 *move entry in hardware table to an new index.
 */
static int32
_sys_humber_acl_entry_move_hw(sys_acl_entry_t* pe,
                              int32 tcam_idx_new)
{
    int32 tcam_idx_old = pe->block_index;
    sys_aclqos_sub_entry_info_t sub_info;

    CTC_PTR_VALID_CHECK(pe);
    SYS_ACL_DBG_FUNC();

    /* add first */
    memset(&sub_info, 0, sizeof(sub_info));
    sub_info.offset = tcam_idx_new;
    
    CTC_ERROR_RETURN(_sys_humber_aclqos_write_entry_to_chip(
        0, pe->p_label, pe, &sub_info));

    /* then delete */
    CTC_ERROR_RETURN(_sys_humber_aclqos_delete_entry_from_chip(
        0, pe->p_label, pe->key.type, tcam_idx_old));

    /* set new_index */
    pe->block_index = tcam_idx_new;

    return CTC_E_NONE;
}

/*
 *move entry to a new place with amount steps.
 */
static int32
_sys_humber_acl_entry_move(sys_acl_entry_t* pe, int32 amount)
{
    int32  tcam_idx_old = 0; /* Original entry tcam index.    */
    int32  tcam_idx_new = 0; /* Next tcam index for the entry.*/
    sys_acl_block_t*         pb;         /* Field slice control.          */

    uint8                    asic_type;

    SYS_ACL_DBG_FUNC();

    CTC_PTR_VALID_CHECK(pe);

    asic_type = acl_master->asic_type[pe->key.type];

    pb = &acl_master->block[asic_type];
    if (amount == 0)
    {
        SYS_ACL_DBG_INFO("    %%INFO: amount == 0\n");
        return (CTC_E_NONE);
    }

    tcam_idx_old = pe->block_index;
    tcam_idx_new = tcam_idx_old + amount;

    /* Move the hardware entry.*/
    CTC_ERROR_RETURN(_sys_humber_acl_entry_move_hw(pe, tcam_idx_new));

    /* Move the software entry.*/

    pb->entries[tcam_idx_old] = NULL;
    pb->entries[tcam_idx_new] = pe;
    pe->block_index = tcam_idx_new;

    return (CTC_E_NONE);
}

/*
 *shift up entries from target entry to prev null entry
 */
static int32
_sys_humber_acl_entry_shift_up(sys_acl_block_t* pb,
                               int32 target_index,
                               int32 prev_null_index)
{
    int32                   temp;

    /* Input parameter check. */
    CTC_PTR_VALID_CHECK(pb);
    SYS_ACL_DBG_FUNC();

    temp = prev_null_index;

    /* start from prev-1:
     *  prev + 1 -- > prev
     *  prev + 2 -- > prev + 1
     *  ...
     *  target -- > target - 1
     */
    while ((temp < target_index))
    {
        /* Move the entry at the next index to the prev. empty index. */

        /* Perform entry move. */
        CTC_ERROR_RETURN(_sys_humber_acl_entry_move(pb->entries[temp + 1], -1));

        temp++;
    }

    return (CTC_E_NONE);
}

/*
 *shift down entries from target entry to next null entry
 */
static int32
_sys_humber_acl_entry_shift_down(sys_acl_block_t* pb,
                                 int32 target_index,
                                 int32 next_null_index)
{
    int32              temp;

    /* Input parameter check. */
    CTC_PTR_VALID_CHECK(pb);
    SYS_ACL_DBG_FUNC();

    temp = next_null_index;

    /*
     * Move entries one step down
     *     starting from the last entry
     */
    while (temp > target_index)
    {

        /* Perform entry move. */
        CTC_ERROR_RETURN(_sys_humber_acl_entry_move(pb->entries[temp - 1], 1));

        temp--;
    }

    return (CTC_E_NONE);
}

typedef struct
{
    uint16 t_idx; /* target index */
    uint16 o_idx; /* old index */

}_fpa_target_t;

static int32
_sys_humber_acl_reorder(sys_acl_block_t* pb, int32 bottom_idx, uint8 extra_num)
{
    int32   idx;
    int32   t_idx;
    int32   o_idx;
    _fpa_target_t* target_a = NULL;
    int32   ret = 0;
    uint8   move_ok;
/*    static double time;*/
/*    clock_t begin,end;*/

    uint32  full_num;
    uint32  real_num;            /* actual entry number */
    uint32  free_num;
    uint32  left_num;

/*    begin = clock();*/
    CTC_PTR_VALID_CHECK(pb);

    extra_num = extra_num ? 1 : 0; /*extra num is 1 for new entry */

    /* malloc a new array based on new exist entry*/
    full_num = pb->entry_count - extra_num; /* if extra_num == 1, reserve last */
    free_num = pb->free_count;

    real_num = pb->entry_count - pb->free_count;
    target_a = (_fpa_target_t*) mem_malloc(MEM_ACLQOS_MODULE, real_num * sizeof(_fpa_target_t));
    if(!target_a)
    {
        return CTC_E_NO_MEMORY;
    }
    memset(target_a, 0, sizeof(real_num * sizeof(_fpa_target_t)));
    /* MALLOC_ZERO(MEM_ACLQOS_MODULE, target_a, real_num * sizeof(_fpa_target_t)) */

    /* save target idx to array */
    for (t_idx = 0; t_idx < real_num; t_idx++)
    {
        target_a[t_idx].t_idx = (full_num * t_idx) / real_num;
    }

    /* save old idx to array */
    o_idx = 0;
    for (idx = 0; idx < pb->entry_count ; idx++) /* through all entry */
    {
        if (pb->entries[idx])
        {
            target_a[o_idx].o_idx = idx;
            o_idx++;
        }
    }

    left_num   = real_num;
    while (left_num) /* move_num */
    {
        SYS_ACL_DBG_INFO("left_num %d, real_num %d\n", left_num, real_num);

        for(idx = 0; idx < left_num; idx++)
        {
            move_ok  = 0;

            if (target_a[idx].o_idx == target_a[idx].t_idx) /* stay */
            {
                SYS_ACL_DBG_INFO("stay !\n");
                kal_memmove(&target_a[idx], &target_a[idx+1], (left_num - idx -1)*sizeof(_fpa_target_t));
                left_num--;
                idx--;
            }
            else
            {
                if (target_a[idx].o_idx < target_a[idx].t_idx)/* move down */
                {
                    if ((idx == left_num - 1) || (target_a[idx + 1].o_idx > target_a[idx].t_idx))
                    {
                        move_ok = 1;
                    }
                }
                else /* move up */
                {
                    if ((idx == 0)|| (target_a[idx - 1].o_idx > target_a[idx].t_idx))
                    {
                        move_ok = 1;
                    }
                }

                if (move_ok)
                {
                    SYS_ACL_DBG_INFO(" move from %d to %d!\n", target_a[idx].o_idx, target_a[idx].t_idx);
                    /* move idx to temp */
                    CTC_ERROR_GOTO(_sys_humber_acl_entry_move
                        (pb->entries[target_a[idx].o_idx], (target_a[idx].t_idx - target_a[idx].o_idx)), ret, cleanup);
                    kal_memmove(&target_a[idx], &target_a[idx+1], (left_num - idx -1)*sizeof(_fpa_target_t));
                    left_num--;
                    idx--;
                }
            }
        }
    }

    mem_free(target_a);

/*    end = clock();*/
/*    time = time + ((double)(end - begin) / CLOCKS_PER_SEC);*/
/*    SYS_ACL_DBG_INFO(" t:%lf", time);*/

    return CTC_E_NONE;

cleanup:
    mem_free(target_a);
    return ret;
}

static int32
_sys_humber_acl_lookup_block_index(uint32 entry_id,
                                   uint16* block_index)
{
    sys_acl_entry_t* pe = NULL;
    
    *block_index = SYS_ACL_INVALID_INDEX;
    _sys_humber_acl_get_sys_entry_by_eid(entry_id, &pe);
    if (pe)
    {
        *block_index = pe->block_index;
    }
    
    return CTC_E_NONE;
}

static int32
_sys_humber_acl_shift_all_entries_down(sys_acl_block_t* pb)
{
    int32   prev_entry_idx = 0;
    int32   left_num;
    int32   idx;
    int32   null_idx = 0;

    left_num = pb->entry_count - pb->free_count;

    idx = pb->entry_count - 1;

    while(left_num)
    {
        if (pb->entries[idx])
        {
            idx --;
            left_num --;
            continue;
        }
        
        null_idx = idx;
        
        for(prev_entry_idx = null_idx; prev_entry_idx >= 0; prev_entry_idx--)
        {
            if (pb->entries[prev_entry_idx])
            {
                _sys_humber_acl_entry_move(pb->entries[prev_entry_idx], null_idx - prev_entry_idx);
                break;
            }
        }
        if (prev_entry_idx < 0)
        {
            break;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_acl_shift_all_entries_up(sys_acl_block_t* pb)
{
    int32   next_entry_idx = 0;
    int32   left_num;
    int32   idx;
    int32   null_idx = 0;

    left_num = pb->entry_count - pb->free_count;
    
    idx = 0;
    
    while(left_num)
    {
        if (pb->entries[idx])
        {
            idx ++;
            left_num --;
            continue;
        }
        
        null_idx = idx;
        
        for(next_entry_idx = null_idx; next_entry_idx < pb->entry_count; next_entry_idx++)
        {
            if (pb->entries[next_entry_idx])
            {
                _sys_humber_acl_entry_move(pb->entries[next_entry_idx], null_idx - next_entry_idx);
                break;
            }
        }
        if (next_entry_idx >= pb->entry_count)
        {
            break;
        }
    }

    return CTC_E_NONE;
}

/*
 * worst is best
 */
static int32
_sys_humber_acl_magic_reorder(sys_acl_block_t* pb, uint32 after_entry_id)
{
    if ((pb->entry_count - pb->free_count) > SYS_ACL_REMEMBER_BASE)
    {
        return CTC_E_NONE;
    }

    if (after_entry_id == CTC_ACLQOS_ENTRY_ID_HEAD)
    {
        pb->after_0_cnt ++;
    }
    else if (after_entry_id < CTC_ACLQOS_ENTRY_ID_TAIL)
    {
        pb->after_1_cnt ++;
    }
    
    if ((pb->entry_count - pb->free_count) == SYS_ACL_REMEMBER_BASE)
    {
        /*after_0_cnt  after_1_cnt
              10          0
              9           1
              8           2
              7           3
              .           .
              3           7
              2           8
              1           9
              0           10
          after_0_cnt  after_1_cnt
        */
        if (pb->after_0_cnt >= 4 + pb->after_1_cnt)
        {
            _sys_humber_acl_shift_all_entries_down(pb);
        }
        else if (pb->after_1_cnt >= 4 + pb->after_0_cnt)
        {
            _sys_humber_acl_shift_all_entries_up(pb);
        }
    }    

    return CTC_E_NONE;
}


/*
 * worst is best
 */
static int32
_sys_humber_acl_get_block_index(sys_acl_block_t* pb, uint32 after_entry_id,
                                   uint16* block_index, uint16* shift_amount)
{
    int32   bottom_idx = 0;
    int32   target_idx = 0;
    int32   first_entry_idx = 0;
    int32   idx;
    int32   prev_null_idx = 0;
    int32   next_null_idx = 0;
    int32   shift_up_amount = 0;
    int32   shift_down_amount = 0;
    uint16  after_idx = 0;

    CTC_PTR_VALID_CHECK(pb);
    CTC_PTR_VALID_CHECK(block_index);
    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if (pb->free_count < 1)
    {
        return CTC_E_ACL_GET_BLOCK_INDEX_FAILED;
    }

    *shift_amount = 0;
    
    _sys_humber_acl_magic_reorder(pb, after_entry_id);
    
    if (after_entry_id == CTC_ACLQOS_ENTRY_ID_TAIL)
    {
        next_null_idx = SYS_ACL_INVALID_INDEX;
        bottom_idx = pb->entry_count + pb->entry_dft_max - 1;

        for (idx = pb->entry_count; idx <= bottom_idx; idx++)
        {
            if (pb->entries[idx] == NULL)
            {
                next_null_idx = idx;
                break;
            }
        }
        target_idx = next_null_idx;
        pb->entry_dft_cnt ++;
    }
    else if (pb->entry_count == pb->free_count) 
    {
        target_idx = pb->entry_count - 1;
    }
    else if (after_entry_id == CTC_ACLQOS_ENTRY_ID_HEAD)
    {
        /*get last not null index*/
        first_entry_idx = SYS_ACL_INVALID_INDEX;

        for (idx = 0; idx < pb->entry_count; idx++)
        {
            if (pb->entries[idx] != NULL)
            {
                first_entry_idx = idx;
                break;
            }
        }

        if (first_entry_idx == 0)
        {
            next_null_idx = SYS_ACL_INVALID_INDEX;
            target_idx = 0;

            /* search first next NULL entry from top down */
            for (idx = target_idx; idx < pb->entry_count; idx++)
            {
                if (pb->entries[idx] == NULL)
                {
                    next_null_idx = idx;
                    break;
                }
            }

            shift_down_amount = next_null_idx - target_idx - 1;

            CTC_ERROR_RETURN(_sys_humber_acl_entry_shift_down(pb,
                                                               target_idx, next_null_idx));
            *shift_amount = shift_down_amount;
        }
        else
        {
            target_idx = first_entry_idx - 1;
        }
    }
    else
    {
        CTC_ERROR_RETURN(_sys_humber_acl_lookup_block_index(after_entry_id, &after_idx));
        
        next_null_idx = SYS_ACL_INVALID_INDEX;
        prev_null_idx = SYS_ACL_INVALID_INDEX;
        
        for (idx = after_idx ; idx >= 0; idx--)
        {
            if (pb->entries[idx] == NULL)
            {
                prev_null_idx = idx;
                break;
            }
        }
        for (idx = after_idx ; idx < pb->entry_count; idx++)
        {
            if (pb->entries[idx] == NULL)
            {
                next_null_idx = idx;
                break;
            }
        }

        if (prev_null_idx == SYS_ACL_INVALID_INDEX)
        {
            shift_up_amount = pb->entry_count;
        }
        else 
        {
            shift_up_amount = after_idx - prev_null_idx;
        }
        
        if (next_null_idx == SYS_ACL_INVALID_INDEX)
        {
            shift_down_amount = pb->entry_count;
        }
        else
        {
            shift_down_amount = next_null_idx - (after_idx + 1);
        }
        
        if (shift_down_amount <= shift_up_amount)
        {
            /* shift down */
            target_idx = after_idx + 1;
            *shift_amount = shift_down_amount;
            CTC_ERROR_RETURN(_sys_humber_acl_entry_shift_down(pb,
                                                               target_idx, next_null_idx));
        }
        else
        {
            /* shift up */
            target_idx = after_idx;
            *shift_amount = shift_up_amount;
            CTC_ERROR_RETURN(_sys_humber_acl_entry_shift_up(pb,
                                                               target_idx, prev_null_idx));
        }
    }

    *block_index = target_idx;

    return CTC_E_NONE;

}


/**
 @brief insert acl/qos entry into entry list in the given label,
        entry id = 0 indicate inserting at front, otherwise after the entry id
*/
int32
sys_humber_aclqos_entry_insert(uint32 label_id, ctc_aclqos_label_type_t label_type, uint32 entry_id, ctc_aclqos_entry_t* p_ctc_entry)
{
    sys_aclqos_entry_t *p_sys_entry[CTC_MAX_LOCAL_CHIP_NUM];
    sys_aclqos_action_t *p_sys_action;
    sys_aclqos_key_t *p_sys_key;
    sys_aclqos_label_t *p_label = NULL;
    uint8 lchip, lchip_num;
    int32 ret;
    uint8 is_service_label = 0;
    uint16 block_index = 0;
    sys_acl_block_t* pb = NULL;
    uint8 asic_type;
    uint16 shift_amount = 0;

    CTC_PTR_VALID_CHECK(p_ctc_entry);
    CTC_MIN_VALUE_CHECK(p_ctc_entry->entry_id, 1);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else if(CTC_ACL_LABEL == label_type) /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    kal_memset(p_sys_entry, 0, sizeof(p_sys_entry));


    asic_type = acl_master->asic_type[p_ctc_entry->key.type];
    pb = &acl_master->block[asic_type];

    /* add entry */
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        _sys_humber_acl_get_sys_entry_by_eid(p_ctc_entry->entry_id, &p_sys_entry[lchip]);
        if (p_sys_entry[lchip])
        {
            ret = CTC_E_ACLQOS_ENTRY_EXIST;
            p_sys_entry[lchip] = NULL;
            goto ERR;
        }

        p_sys_entry[lchip] = (sys_aclqos_entry_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_aclqos_entry_t));
        if (!p_sys_entry[lchip])
        {
            ret = CTC_E_NO_MEMORY;
            goto ERR;
        }

        kal_memset(p_sys_entry[lchip], 0, sizeof(sys_aclqos_entry_t));

        p_sys_entry[lchip]->entry_id = p_ctc_entry->entry_id;

        p_sys_action = &p_sys_entry[lchip]->action;
        p_sys_key = &p_sys_entry[lchip]->key;

        p_sys_entry[lchip]->p_label = p_label;
        /* map acl/qos action */
        ret = _sys_humber_aclqos_map_action(lchip, p_label, &p_ctc_entry->action, p_sys_action);
        if (ret)
        {
            goto ERR;
        }

        /* map acl/qos key */
        ret = _sys_humber_aclqos_map_key(lchip, p_label, &p_ctc_entry->key, p_sys_key);
        if (ret)
        {
            goto ERR;
        }

        /* get entry offset */

        ret = _sys_humber_acl_get_block_index(pb, entry_id, &block_index, &shift_amount);
        if (ret)
        {
            goto ERR;
        }
        p_sys_entry[lchip]->block_index = block_index;

        /* write entries */
        ret = _sys_humber_aclqos_entry_write(pb, p_sys_entry[lchip]);
        if (ret)
        {
            goto ERR;
        }

        if (DO_REORDER(shift_amount, pb->entry_count))
        {
            _sys_humber_acl_reorder(pb, pb->entry_count - 1, 0);
        }

    }

    return CTC_E_NONE;

ERR:
    lchip_num = lchip + 1;
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }
        if (p_sys_entry[lchip])
        {
            _sys_humber_aclqos_entry_remove(pb, p_sys_entry[lchip]);
        }
    }

    return ret;
}


/**
 @brief delete acl/qos entry from entry list in the given label
*/
int32
sys_humber_aclqos_entry_delete(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num;
    uint8 is_service_label = 0;
    uint8            asic_type;
    sys_acl_block_t* pb = NULL;

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {

        is_service_label = 1;
    }

    /* get the acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else if(CTC_ACL_LABEL == label_type)/*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
        if (!p_entry)
        {
            return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
        }
    }
    
    asic_type = acl_master->asic_type[p_entry->key.type];
    pb = &acl_master->block[asic_type];

    CTC_ERROR_RETURN(_sys_humber_aclqos_entry_remove(pb, p_entry));
    
    if (pb->free_count == pb->entry_count)
    {
        pb->after_0_cnt = 0;
        pb->after_1_cnt = 0;
    }

    return CTC_E_NONE;
}


/**
 @brief get acl/qos entry stats result
*/
int32
sys_humber_aclqos_entry_stats_result_get(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_stats_basic_t* p_stats)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num, tmp = 0;
    ctc_stats_basic_t stats_result;
    uint8 is_service_label = 0;

    CTC_PTR_VALID_CHECK(p_stats);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    /* get the acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            tmp++;
            continue;
        }

        /* lookup by entry id */
        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
        if (!p_entry)
        {
            return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
        }

        /* get stats result */
        if (p_entry->action.flag.stats)
        {
            CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, p_entry->action.stats_or_flowid.stats_ptr, &stats_result));
            p_stats->packet_count += stats_result.packet_count;
            p_stats->byte_count += stats_result.byte_count;
        }
    }

    if(tmp == lchip_num)
    {
        return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
    }
    else
    {
        return CTC_E_NONE;
    }
}


/**
 @brief reset acl/qos entry stats result
*/
int32
sys_humber_aclqos_entry_stats_result_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num, tmp = 0;
    uint8 is_service_label = 0;

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    /* get the acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            tmp ++;
            continue;
        }

        /* lookup by entry id */
        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
        if (!p_entry)
        {
            return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
        }

        /* reset stats result */
        if (p_entry->action.flag.stats)
        {
            CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, p_entry->action.stats_or_flowid.stats_ptr));
        }
    }

    if(tmp == lchip_num)
    {
        return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
    }
    else
    {
        return CTC_E_NONE;
    }

}


int32
sys_humber_aclqos_all_entry_stats_result_reset(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type)
{
    uint8 lchip, lchip_num;
    sys_aclqos_label_t *p_label = NULL;
    sys_aclqos_label_index_t *p_index = NULL;
    ctc_list_pointer_t *p_list = NULL;
    ctc_list_pointer_node_t *pos = NULL;
    sys_aclqos_entry_t *pos_entry = NULL;
    uint8 is_service_label = 0;

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        p_index = p_label->p_index[lchip];
        CTC_PTR_VALID_CHECK(p_index);

        /* lookup goes from tail to head */
        p_list = &p_index->entry_list[entry_type];
        CTC_LIST_POINTER_LOOP_R(pos, p_list)
        {
            pos_entry = _ctc_container_of(pos, sys_aclqos_entry_t, head);
            if (pos_entry->action.flag.stats)
            {
                CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, pos_entry->action.stats_or_flowid.stats_ptr));
            }
        }

    }

    return CTC_E_NONE;
}


/**
 @brief delete all acl/qos entry from entry list in the given label
*/
int32
sys_humber_aclqos_entry_delete_all(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type)
{
    sys_aclqos_label_t *p_label;
    ctc_list_pointer_t *p_list;
    ctc_list_pointer_node_t *p_node, *p_next_node;
    sys_aclqos_entry_t *p_entry = NULL;
    uint8 lchip, lchip_num;
    uint8 is_service_label = 0;
    uint8            asic_type;
    sys_acl_block_t* pb = NULL;

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else if(CTC_ACL_LABEL == label_type)/*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    asic_type = acl_master->asic_type[entry_type];
    pb = &acl_master->block[asic_type];

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        p_list = &(p_label->p_index[lchip]->entry_list[entry_type]);
        CTC_LIST_POINTER_LOOP_DEL(p_node, p_next_node, p_list)
        {
            p_entry = _ctc_container_of(p_node, sys_aclqos_entry_t, head);

            /* remove the entry */
            CTC_ERROR_RETURN(_sys_humber_aclqos_entry_remove(pb, p_entry));
        }

    }

    return CTC_E_NONE;
}

/**
 @brief To invalid an ACL entry, which indicates removing the entry from hardware table but still
        stay in software table.
*/
int32
sys_humber_aclqos_entry_set_invalid(uint32 label_id, ctc_aclqos_key_type_t entry_type, uint32 entry_id, bool invalid)
{
    return CTC_E_NONE;
}

/**
 @brief set acl/qos entry action
*/
int32
sys_humber_aclqos_entry_action_add(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num;
    uint32 tmp, cmd;
    tbl_id_t action_tbl_id;
    fld_id_t action_fld_id;
    uint8 is_service_label = 0;
    sys_nh_offset_array_t offset_array;
    uint16 pbr_vrfId = 0;
    uint32 policer_ptr = 0;
    uint32 ds_fwd_offset = 0;
    uint32 ds_fwd_base = 0;
    uint32 ds_fwd_offset_old = 0;
    uint32 nhid_old = 0;
    sys_acl_redirect_t acl_redirect;
    sys_acl_redirect_t* p_acl_redirect = 0;
    sys_humber_opf_t opf;
    int32 ret = 0;

    kal_memset(&acl_redirect, 0, sizeof(acl_redirect));
    kal_memset(&opf, 0, sizeof(opf));

    CTC_PTR_VALID_CHECK(p_action);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    /* get the acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        /* lookup by entry id */
        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
        if (!p_entry)
        {
            return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_FLOW_POLICER_FLAG)
        {
            if (p_entry->action.flag.flow_policer)
            {
                sys_humber_qos_flow_policer_unbind(lchip, p_entry->action.policer_id);
            }

            /* bind flow policer to chip */
            CTC_ERROR_RETURN(sys_humber_qos_flow_policer_bind(lchip, p_action->policer_id));

            CTC_ERROR_RETURN(sys_humber_qos_policer_index_get(lchip, p_action->policer_id, &policer_ptr));
            policer_ptr = policer_ptr & 0xFFFF;

            /* write to chip */
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            action_fld_id = DS_MAC_ACL_FLOW_POLICER_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &policer_ptr));

            p_entry->action.flag.flow_policer = 1;
            p_entry->action.policer_id = p_action->policer_id;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_STATS_FLAG)
        {
            if (p_entry->action.flag.flow_id)
            {
                return CTC_E_ACLQOS_COLLISION_FIELD;
            }

            if (p_entry->action.flag.stats)
            {
                return CTC_E_NONE;
            }

            p_entry->action.stats_or_flowid.stats_ptr = SPECIAL_STATS_PTR;
            CTC_ERROR_RETURN(sys_humber_stats_create_statsptr(lchip, 1, &p_entry->action.stats_or_flowid.stats_ptr));
            tmp = p_entry->action.stats_or_flowid.stats_ptr;

            /* write to chip */
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            action_fld_id = DS_MAC_ACL_STATS_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.stats = 1;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_REDIRECT_FLAG)
        {
            if((p_entry->action.flag.fwd) && (p_entry->action.fwd.fwd_nh_id == p_action->fwd.fwd_nh_id))
            {
                return CTC_E_NONE;
            }

            /*save original ds_fwd_ptr and nhid for free operation*/
            ds_fwd_offset_old = p_entry->action.ds_fwd_ptr;
            nhid_old = p_entry->action.fwd.fwd_nh_id;

            /*look up if the nhid has been used for acl redirect*/
            acl_redirect.nhid = p_action->fwd.fwd_nh_id;
            p_acl_redirect = ctc_hash_lookup(p_sys_acl_redirect_hash[lchip], &acl_redirect);
            if(!p_acl_redirect)
            {
                /* new acl redirect */
                p_acl_redirect = (sys_acl_redirect_t *)mem_malloc(MEM_ACLQOS_MODULE, sizeof(sys_acl_redirect_t));
                if (!p_acl_redirect)
                {
                    return CTC_E_NO_MEMORY;
                }
                kal_memset(p_acl_redirect, 0, sizeof(sys_acl_redirect_t));

                CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_action->fwd.fwd_nh_id, offset_array));

                /*alloc ds_fwd_offset of acl*/
                opf.pool_type = OPF_ACL_FWD_SRAM;
                opf.pool_index = lchip;
                ret = sys_humber_opf_alloc_offset(&opf, 1, &ds_fwd_offset);
                if(ret)
                {
                    mem_free(p_acl_redirect);
                    return ret;
                }
                p_entry->action.ds_fwd_ptr = ds_fwd_offset;

                /*copy dsfwd for the dsfwdptr of the key only have 12bit*/
                ds_fwd_base = sys_aclqos_entry_ctl.acl_fwd_base & 0xFFFFFF00;
                ret = _sys_humber_acl_copy_dsfwd(lchip, offset_array[lchip], ds_fwd_base + ds_fwd_offset);
                if(ret)
                {
                    sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset);
                    mem_free(p_acl_redirect);
                    return ret;
                }

                /* insert into hash table */
                p_acl_redirect->nhid = p_action->fwd.fwd_nh_id;
                p_acl_redirect->ds_fwd_offset = ds_fwd_offset;
                p_acl_redirect->ref ++;
                if (!ctc_hash_insert(p_sys_acl_redirect_hash[lchip], p_acl_redirect))
                {
                    sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset);
                    mem_free(p_acl_redirect);
                    return CTC_E_NO_MEMORY;
                }
            }
            else
            {
                p_entry->action.ds_fwd_ptr = p_acl_redirect->ds_fwd_offset;
                p_acl_redirect->ref ++;
            }

            /* write to chip */
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            tmp = p_entry->action.ds_fwd_ptr;
            action_fld_id = DS_MAC_ACL_FWD_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = 1;
            action_fld_id = DS_MAC_ACL_DENY_BRIDGE + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = 1;
            action_fld_id = DS_MAC_ACL_DENY_ROUTE + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            /*free unused old ds_fwd_ptr*/
            if (p_entry->action.flag.fwd)
            {
                acl_redirect.nhid = nhid_old;
                p_acl_redirect = ctc_hash_lookup(p_sys_acl_redirect_hash[lchip], &acl_redirect);
                if(p_acl_redirect)
                {
                    p_acl_redirect->ref --;

                    if(p_acl_redirect->ref == 0)
                    {
                        opf.pool_type = OPF_ACL_FWD_SRAM;
                        opf.pool_index = lchip;
                        CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, ds_fwd_offset_old));

                        ctc_hash_remove(p_sys_acl_redirect_hash[lchip], p_acl_redirect);
                        mem_free(p_acl_redirect);
                    }
                }
                else
                {
                    return CTC_E_ACLQOS_INVALID_ACTION;
                }
            }

            p_entry->action.fwd.fwd_nh_id = p_action->fwd.fwd_nh_id;
            p_entry->action.flag.fwd = 1;
            p_entry->action.flag.deny_bridge = 1;
            p_entry->action.flag.deny_route = 1;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_RAND_LOG_FLAG)
        {
            CTC_MAX_VALUE_CHECK(p_action->log_weight, 15);
            CTC_MAX_VALUE_CHECK(p_action->log_session_id, SYS_ACLQOS_MAX_SESSION_NUM-1);

            /* write to chip */
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = 1;
            action_fld_id = DS_MAC_ACL_RANDOM_LOG_EN + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = p_action->log_session_id;
            action_fld_id = DS_MAC_ACL_ACL_LOG_ID + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = p_action->log_weight;
            action_fld_id = DS_MAC_ACL_RANDOM_THRESHOLD_SHIFT + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.random_log = 1;
            p_entry->action.acl_log_id = p_action->log_session_id;
            p_entry->action.random_threshold_shift = p_action->log_weight;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_PRIORITY_FLAG)
        {
            CTC_MAX_VALUE_CHECK(p_action->priority, 63);
            CTC_MAX_VALUE_CHECK(p_action->color, 3);

            /* write to chip */
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = p_action->priority;
            action_fld_id = DS_MAC_ACL_PRIORITY + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = p_action->color;
            action_fld_id = DS_MAC_ACL_COLOR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = 1;
            action_fld_id = DS_MAC_ACL_PRIORITY_VALID + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.priority = 1;
            p_entry->action.priority = p_action->priority;
            p_entry->action.color = p_action->color;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_TRUST_FLAG)
        {
            CTC_MAX_VALUE_CHECK(p_action->trust, MAX_CTC_QOS_TRUST - 1);

            /* write to chip */
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = p_action->trust;
            action_fld_id = DS_MAC_ACL_QOS_POLICY + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.trust = 1;
            p_entry->action.trust = p_action->trust;
        }

        /*Update pbr fwd action, not support ecmp.*/
        if (p_action->flag & CTC_ACLQOS_ACTION_PBR_FWD_FLAG)
        {
            if (p_action->flag & CTC_ACLQOS_ACTION_PBR_ECMP_FLAG)
            {
                return CTC_E_ACLQOS_INVALID_ACTION;
            }

            if (p_entry->action.pbr_ecpn > 0)
            {
                return CTC_E_ACLQOS_INVALID_ACTION;
            }

            CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_action->fwd.fwd_nh_id, offset_array));
            CTC_ERROR_RETURN(sys_humber_nh_get_l3ifid(p_action->fwd.fwd_nh_id, &pbr_vrfId));

            /* write to chip */
            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write pbr_fwd_ptr to asic*/
            tmp = offset_array[lchip];
            action_fld_id = DS_IPV4_UCAST_PBR_DUAL_DA_DS_FWD_PTR + (DS_IPV6_UCAST_PBR_DUAL_DA_ISATAP_CHECK_EN -
                DS_IPV4_UCAST_PBR_DUAL_DA_ISATAP_CHECK_EN) * (action_tbl_id - DS_IPV4_UCAST_PBR_DUAL_DA);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            /*write vrf_id to asic*/
            tmp = pbr_vrfId;
            action_fld_id = DS_IPV4_UCAST_PBR_DUAL_DA_VRF_ID + (DS_IPV6_UCAST_PBR_DUAL_DA_ISATAP_CHECK_EN -
                DS_IPV4_UCAST_PBR_DUAL_DA_ISATAP_CHECK_EN) * (action_tbl_id - DS_IPV4_UCAST_PBR_DUAL_DA);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.pbr_fwd_ptr = offset_array[lchip];
            p_entry->action.fwd.fwd_nh_id = p_action->fwd.fwd_nh_id;
            p_entry->action.pbr_vrfid = pbr_vrfId;
        }

    }

    return CTC_E_NONE;
}


/**
 @brief reset acl/qos entry action
*/
int32
sys_humber_aclqos_entry_action_delete(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t entry_type, uint32 entry_id, ctc_aclqos_action_t* p_action)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num;
    uint32 tmp, cmd;
    tbl_id_t action_tbl_id;
    fld_id_t action_fld_id;
    uint8 is_service_label = 0;
    sys_acl_redirect_t acl_redirect;
    sys_acl_redirect_t* p_acl_redirect = 0;
    sys_humber_opf_t opf;

    kal_memset(&acl_redirect, 0, sizeof(acl_redirect));
    kal_memset(&opf, 0, sizeof(opf));

    CTC_PTR_VALID_CHECK(p_action);

    SYS_ACLQOS_ENTRY_DBG_FUNC();

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    /* get the acl/qos label */
    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }
    else /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_LABEL_NOT_EXIST;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        /* lookup by entry id */
        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
        if (!p_entry)
        {
            return CTC_E_ACLQOS_ENTRY_NOT_EXIST;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_FLOW_POLICER_FLAG)
        {
            if (!p_entry->action.flag.flow_policer)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            /* write to chip */
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            action_fld_id = DS_MAC_ACL_FLOW_POLICER_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            tmp = 0xFFFF;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            /* unbind flow policer from chip */
            CTC_ERROR_RETURN(sys_humber_qos_flow_policer_unbind(lchip, p_entry->action.policer_id));

            p_entry->action.flag.flow_policer = 0;
            p_entry->action.policer_id = 0;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_STATS_FLAG)
        {
            if (!p_entry->action.flag.stats)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            tmp = SPECIAL_STATS_PTR;

            /* write to chip */
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            action_fld_id = DS_MAC_ACL_STATS_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.stats = 0;
            sys_humber_stats_delete_statsptr(lchip, 1, p_entry->action.stats_or_flowid.stats_ptr);
            CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, p_entry->action.stats_or_flowid.stats_ptr));
            p_entry->action.stats_or_flowid.stats_ptr = SPECIAL_STATS_PTR;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_REDIRECT_FLAG)
        {
            if (!p_entry->action.flag.fwd)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            /* write to chip to delete the action*/
            CTC_ERROR_RETURN(
                    _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            tmp = 0xFFF;
            action_fld_id = DS_MAC_ACL_FWD_PTR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = 0;
            action_fld_id = DS_MAC_ACL_DENY_BRIDGE + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            tmp = 0;
            action_fld_id = DS_MAC_ACL_DENY_ROUTE + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            /*free unused ds_fwd_ptr*/
            acl_redirect.nhid = p_entry->action.fwd.fwd_nh_id;
            p_acl_redirect = ctc_hash_lookup(p_sys_acl_redirect_hash[lchip], &acl_redirect);
            if(p_acl_redirect)
            {
                p_acl_redirect->ref --;

                if(p_acl_redirect->ref == 0)
                {
                    opf.pool_type = OPF_ACL_FWD_SRAM;
                    opf.pool_index = lchip;
                    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_entry->action.ds_fwd_ptr));

                    ctc_hash_remove(p_sys_acl_redirect_hash[lchip], p_acl_redirect);
                    mem_free(p_acl_redirect);
                }
            }
            else
            {
                return CTC_E_ACLQOS_INVALID_ACTION;
            }

            p_entry->action.fwd.fwd_nh_id = 0;
            p_entry->action.flag.fwd = 0;
            p_entry->action.flag.deny_bridge = 0;
            p_entry->action.flag.deny_route = 0;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_RAND_LOG_FLAG)
        {
            if(!p_entry->action.flag.random_log)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = 0;
            action_fld_id = DS_MAC_ACL_RANDOM_LOG_EN + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            action_fld_id = DS_MAC_ACL_ACL_LOG_ID + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            action_fld_id = DS_MAC_ACL_RANDOM_THRESHOLD_SHIFT + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.random_log = 0;
            p_entry->action.acl_log_id = 0;
            p_entry->action.random_threshold_shift = 0;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_PRIORITY_FLAG)
        {
            if(!p_entry->action.flag.priority)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = 0;

            action_fld_id = DS_MAC_ACL_PRIORITY_VALID + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            action_fld_id = DS_MAC_ACL_PRIORITY + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            action_fld_id = DS_MAC_ACL_COLOR + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.priority = 0;
            p_entry->action.priority = 0;
            p_entry->action.color = 0;
        }

        if(p_action->flag & CTC_ACLQOS_ACTION_TRUST_FLAG)
        {
            if(!p_entry->action.flag.trust)
            {
                return CTC_E_ENTRY_NOT_EXIST;
            }

            CTC_ERROR_RETURN(
                _sys_humber_aclqos_get_action_table_id(p_label->type, p_entry->key.type, &action_tbl_id));

            /*write to asic*/
            tmp = 0x7;
            action_fld_id = DS_MAC_ACL_QOS_POLICY + (DS_IPV4_ACL_STATS_MODE - DS_MAC_ACL_STATS_MODE) * (action_tbl_id - DS_MAC_ACL);
            cmd = DRV_IOW(IOC_TABLE, action_tbl_id, action_fld_id);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_entry->block_index, cmd, &tmp));

            p_entry->action.flag.trust = 0;
            p_entry->action.trust = 0;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief show the given entry information
*/
int32
sys_humber_show_aclqos_entry(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t ctc_type, uint32 entry_id)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    uint8 lchip, lchip_num;
    uint8 is_service_label = 0;

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("%% Not found label with id = %u\n", label_id);
        return CTC_E_NONE;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else if(CTC_ACL_LABEL == label_type) /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
            continue;

        _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);

        if (!p_entry)
        {
          /*  SYS_ACLQOS_ENTRY_DBG_INFO("%% Not found %s entry with id = %u\n", sys_aclqos_entry_type_name[ctc_type], entry_id);
            */
            return CTC_E_NONE;
        }

        SYS_ACLQOS_ENTRY_DBG_INFO("************ chip = %d ************\n", lchip);
        switch(p_entry->key.type)
        {
            case CTC_ACLQOS_MAC_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_aclqos_mac_entry(p_entry));
                break;

            case CTC_ACLQOS_IPV4_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv4_entry(p_entry));
                break;

            case CTC_ACLQOS_MPLS_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_aclqos_mpls_entry(p_entry));
                break;

            case CTC_ACLQOS_IPV6_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv6_entry(p_entry));
                break;

            case CTC_ACL_PBR_IPV4_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv4_entry(p_entry));
                break;

            case CTC_ACL_PBR_IPV6_KEY:
                CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv6_entry(p_entry));
                break;

            default:
                return CTC_E_ACLQOS_INVALID_KEY_TYPE;
        }
    }

    return CTC_E_NONE;
}


/**
 @brief show the given entry information
*/
int32
sys_humber_show_all_aclqos_entry(uint32 label_id, ctc_aclqos_label_type_t label_type, ctc_aclqos_key_type_t ctc_type)
{
    sys_aclqos_label_t *p_label;
    sys_aclqos_entry_t *p_entry;
    ctc_list_pointer_t *p_list;
    ctc_list_pointer_node_t *p_node;
    uint8 lchip, lchip_num;
    uint8 is_service_label = 0;

    if(CTC_SERVICE_LABEL == label_type)
    {
        is_service_label = 1;
    }

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, is_service_label, &p_label));
    if (!p_label)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("%% Not found label with id = %u\n", label_id);
        return CTC_E_NONE;
    }

    if(CTC_QOS_LABEL == label_type) /*CTC_QOS_LABEL*/
    {
        if(IS_ACL_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }
    else if(CTC_ACL_LABEL == label_type) /*CTC_ACL_LABEL*/
    {
        if(IS_QOS_LABEL(p_label->type))
        {
            return CTC_E_ACLQOS_DIFFERENT_TYPE;
        }
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
            continue;

        SYS_ACLQOS_ENTRY_DBG_INFO("************ chip = %d ************\n", lchip);

        p_list = &(p_label->p_index[lchip]->entry_list[ctc_type]);
        CTC_LIST_POINTER_LOOP(p_node, p_list)
        {
            p_entry = _ctc_container_of(p_node, sys_aclqos_entry_t, head);
            switch(p_entry->key.type)
            {
                case CTC_ACLQOS_MAC_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_aclqos_mac_entry(p_entry));
                    break;

                case CTC_ACLQOS_IPV4_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv4_entry(p_entry));
                    break;

                case CTC_ACLQOS_MPLS_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_aclqos_mpls_entry(p_entry));
                    break;

                case CTC_ACLQOS_IPV6_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv6_entry(p_entry));
                    break;

                case CTC_ACL_PBR_IPV4_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv4_entry(p_entry));
                    break;

                case CTC_ACL_PBR_IPV6_KEY:
                    CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv6_entry(p_entry));
                    break;

                default:
                    return CTC_E_ACLQOS_INVALID_KEY_TYPE;
            }
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_show_aclqos_entry_id(uint32 entry_id)
{
    sys_aclqos_entry_t *p_entry = NULL;
    _sys_humber_acl_get_sys_entry_by_eid(entry_id, &p_entry);
    if (!p_entry)
    {
        return CTC_E_NONE;
    }
    switch(p_entry->key.type)
    {
        case CTC_ACLQOS_MAC_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_aclqos_mac_entry(p_entry));
            break;

        case CTC_ACLQOS_IPV4_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv4_entry(p_entry));
            break;

        case CTC_ACLQOS_MPLS_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_aclqos_mpls_entry(p_entry));
            break;

        case CTC_ACLQOS_IPV6_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_aclqos_ipv6_entry(p_entry));
            break;

        case CTC_ACL_PBR_IPV4_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv4_entry(p_entry));
            break;

        case CTC_ACL_PBR_IPV6_KEY:
            CTC_ERROR_RETURN(_sys_humber_show_acl_pbr_ipv6_entry(p_entry));
            break;

        default:
            return CTC_E_ACLQOS_INVALID_KEY_TYPE;
    }

    return CTC_E_NONE;
}

int32
_sys_humber_show_aclqos_block(sys_acl_block_t* pb)
{
    int32 i;
    sys_aclqos_entry_t* p_entry = NULL;
    
    SYS_ACLQOS_ENTRY_DBG_INFO("block_number : %d\n", pb->block_number);
    SYS_ACLQOS_ENTRY_DBG_INFO("block_type   : %d\n", pb->block_type);
    SYS_ACLQOS_ENTRY_DBG_INFO("entry_max    : %d\n", pb->entry_count);
    SYS_ACLQOS_ENTRY_DBG_INFO("entry_cnt    : %d\n", pb->entry_count - pb->free_count);
    SYS_ACLQOS_ENTRY_DBG_INFO("entry_dft_cnt: %d\n", pb->entry_dft_cnt);
    SYS_ACLQOS_ENTRY_DBG_INFO("free_count   : %d\n", pb->free_count);
    SYS_ACLQOS_ENTRY_DBG_INFO("lchip        : %d\n", pb->lchip);
    SYS_ACLQOS_ENTRY_DBG_INFO("after_0_cnt  : %d\n", pb->after_0_cnt);
    SYS_ACLQOS_ENTRY_DBG_INFO("after_1_cnt  : %d\n", pb->after_1_cnt);

    SYS_ACLQOS_ENTRY_DBG_INFO("Entries      :\n");
    SYS_ACLQOS_ENTRY_DBG_INFO(" ----- ---------- ---------- ----- ------\n");
    SYS_ACLQOS_ENTRY_DBG_INFO(" %5s %10s %10s %5s %6s\n", 
        "index", "entry_id", "label", "type", "offset");
    SYS_ACLQOS_ENTRY_DBG_INFO(" ----- ---------- ---------- ----- ------\n");
    for (i = 0; i < pb->entry_count + pb->entry_dft_cnt; i++)
    {
        sys_aclqos_label_t *p_label;
        
        p_entry = pb->entries[i];
        if (!p_entry)
        {
            continue;
        }

        p_label = p_entry->p_label;
        SYS_ACLQOS_ENTRY_DBG_INFO(" %5d", i);
        SYS_ACLQOS_ENTRY_DBG_INFO(" %10u", p_entry->entry_id);
        SYS_ACLQOS_ENTRY_DBG_INFO(" %10u", p_label->id);
        SYS_ACLQOS_ENTRY_DBG_INFO(" %5d", p_entry->key.type);
        SYS_ACLQOS_ENTRY_DBG_INFO(" %6d\n", p_entry->block_index);
    }
    SYS_ACLQOS_ENTRY_DBG_INFO(" ----- ---------- ---------- ----- ------\n");

    return CTC_E_NONE;
}

int32
sys_humber_show_aclqos_block(int32 asic_type)
{
    sys_acl_block_t* pb;

    if (SYS_ACL_ASIC_TYPE_MAC == asic_type || SYS_ACL_ASIC_TYPE_MAX == asic_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("----------------MAC BLOCK DB-------------------\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("-----------------------------------------------\n");
        pb = &sys_aclqos_entry_ctl.block[SYS_ACL_ASIC_TYPE_MAC];
        _sys_humber_show_aclqos_block(pb);
        SYS_ACLQOS_ENTRY_DBG_INFO("-----------------------------------------------\n\n");
    }

    if (SYS_ACL_ASIC_TYPE_IPV4 == asic_type || SYS_ACL_ASIC_TYPE_MAX == asic_type)
    {
        SYS_ACLQOS_ENTRY_DBG_INFO("----------------IPV4 BLOCK DB------------------\n");
        SYS_ACLQOS_ENTRY_DBG_INFO("-----------------------------------------------\n");
        pb = &sys_aclqos_entry_ctl.block[SYS_ACL_ASIC_TYPE_IPV4];
        _sys_humber_show_aclqos_block(pb);
        SYS_ACLQOS_ENTRY_DBG_INFO("-----------------------------------------------\n\n");
    }

    return CTC_E_NONE;
}

int32
sys_humber_acl_get_all_permit_entry_stats(uint32 label_id, ctc_stats_basic_t* entry_stats)
{
    uint8 lchip, lchip_num;
    uint16 stats_ptr;
    ctc_aclqos_key_type_t key_type;
    sys_aclqos_label_t *p_label = NULL;
    sys_aclqos_label_index_t *p_index = NULL;
    ctc_list_pointer_t *p_list = NULL;
    ctc_list_pointer_node_t *pos = NULL;
    sys_aclqos_entry_t *pos_entry = NULL;
    ctc_stats_basic_t basic_stats;

    CTC_ERROR_RETURN(sys_humber_aclqos_label_lookup(label_id, 0, &p_label));
    if (!p_label)
    {
        return CTC_E_ACLQOS_LABEL_NOT_EXIST;
    }

    if(IS_QOS_LABEL(p_label->type))
    {
        return CTC_E_ACLQOS_DIFFERENT_TYPE;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        if (!p_label->p_index[lchip])
        {
            continue;
        }

        p_index = p_label->p_index[lchip];
        CTC_PTR_VALID_CHECK(p_index);

        for(key_type=CTC_ACLQOS_MAC_KEY; key_type<MAX_CTC_ACLQOS_KEY; key_type++)
        {
            /* lookup goes from tail to head */
            p_list = &p_index->entry_list[key_type];
            CTC_LIST_POINTER_LOOP_R(pos, p_list)
            {
                pos_entry = _ctc_container_of(pos, sys_aclqos_entry_t, head);
                if ((!pos_entry->action.flag.discard)&&(pos_entry->action.flag.stats))
                {
                    stats_ptr = pos_entry->action.stats_or_flowid.stats_ptr;
                    CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, stats_ptr, &basic_stats));
                    entry_stats->packet_count += basic_stats.packet_count;
                    entry_stats->byte_count += basic_stats.byte_count;
                }
            }
        }

    }

    return CTC_E_NONE;
}

static int32
_sys_humber_aclqos_add_default_entry(bool deny)
{
    ds_mac_acl_t ds_action;
    tbl_entry_t  ds_key;
    ds_acl_mac_key_t  ds_mac_key, ds_mac_mask;
    ds_acl_ipv4_key_t ds_ipv4_key, ds_ipv4_mask;
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 index;

    kal_memset(&ds_action, 0, sizeof(ds_mac_acl_t));
    kal_memset(&ds_key, 0, sizeof(tbl_entry_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        ds_action.discard_packet = deny ? 1 : 0;
        ds_action.fwd_ptr          = 0xFFF;    /*indicates fwd_ptr is not valid*/
        ds_action.qos_policy       = 0x7;      /*indicates qos_policy is not valid */
        ds_action.stats_ptr        = 0xFFFF;   /*indicates stats_ptr is not valid*/
        ds_action.flow_policer_ptr = 0xFFFF;   /*indicates flow_policer_ptr is not valid*/

        {
            index = acl_master->block[SYS_ACL_ASIC_TYPE_MAC].entry_count +
                    acl_master->block[SYS_ACL_ASIC_TYPE_MAC].entry_dft_max - 1;
            acl_master->block[SYS_ACL_ASIC_TYPE_MAC].entry_dft_cnt ++;

            kal_memset(&ds_mac_key, 0, sizeof(ds_acl_mac_key_t));
            kal_memset(&ds_mac_mask, 0, sizeof(ds_acl_mac_key_t));
            ds_mac_key.tableid0 = QOS_MAC_TABLEID0_C;
            ds_mac_key.tableid1 = QOS_MAC_TABLEID1_C;
            ds_mac_key.tableid2 = QOS_MAC_TABLEID2_C;
            ds_mac_key.tableid3 = QOS_MAC_TABLEID3_C;
            ds_mac_mask.tableid0 = 0xF;
            ds_mac_mask.tableid1 = 0xF;
            ds_mac_mask.tableid2 = 0xF;
            ds_mac_mask.tableid3 = 0xF;
            ds_key.data_entry = (uint32 *)&ds_mac_key;
            ds_key.mask_entry = (uint32 *)&ds_mac_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_QOS_MAC_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_MAC_QOS, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));

            /* IPv4 QOS */
            index = acl_master->block[SYS_ACL_ASIC_TYPE_IPV4].entry_count +
                    acl_master->block[SYS_ACL_ASIC_TYPE_IPV4].entry_dft_max - 1;
            acl_master->block[SYS_ACL_ASIC_TYPE_IPV4].entry_dft_cnt ++;

            kal_memset(&ds_ipv4_key, 0, sizeof(ds_acl_ipv4_key_t));
            kal_memset(&ds_ipv4_mask, 0, sizeof(ds_acl_ipv4_key_t));
            ds_ipv4_key.tableid0 = QOS_IPV4_TABLEID0_C;
            ds_ipv4_key.tableid1 = QOS_IPV4_TABLEID1_C;
            ds_ipv4_key.tableid2 = QOS_IPV4_TABLEID2_C;
            ds_ipv4_key.tableid3 = QOS_IPV4_TABLEID3_C;
            ds_ipv4_mask.tableid0 = 0xF;
            ds_ipv4_mask.tableid1 = 0xF;
            ds_ipv4_mask.tableid2 = 0xF;
            ds_ipv4_mask.tableid3 = 0xF;
            ds_key.data_entry = (uint32 *)&ds_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_QOS_IPV4_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_QOS, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_acl_pbr_add_default_entry()
{
    sys_humber_opf_t opf;
    ds_ipv4_ucast_pbr_dual_da_t ds_action;
    tbl_entry_t  ds_key;
    ds_ipv4_pbr_dualda_key_t ds_ipv4_key, ds_ipv4_mask;
    ds_ipv6_pbr_dualda_key_t ds_ipv6_key, ds_ipv6_mask;
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 index;

    kal_memset(&ds_action, 0, sizeof(ds_mac_acl_t));
    kal_memset(&ds_key, 0, sizeof(tbl_entry_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        ds_action.deny_pbr = 1;        /*indicates do normal route*/

        /* IPv4 PBR */
        if (sys_aclqos_entry_ctl.pbr_ipv4_acl_entry_num > 0)
        {
            if (sys_aclqos_entry_ctl.global_pbr_entry_tail_num > 0)
            {
                opf.pool_type = ACL_PBR_IPV4_KEY_TAIL;
            }
            else
            {
                opf.pool_type = ACL_PBR_IPV4_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_offset(&opf, 1, &index));

            kal_memset(&ds_ipv4_key, 0, sizeof(ds_ipv4_pbr_dualda_key_t));
            kal_memset(&ds_ipv4_mask, 0, sizeof(ds_ipv4_pbr_dualda_key_t));
            ds_ipv4_key.table_id0 = PBR_IPV4_TABLEID0_C;
            ds_ipv4_key.table_id1 = PBR_IPV4_TABLEID1_C;
            ds_ipv4_mask.table_id0 = 0xF;
            ds_ipv4_mask.table_id1 = 0xF;
            ds_key.data_entry = (uint32 *)&ds_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_PBR_DUALDA_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_UCAST_PBR_DUAL_DA, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }

        /* IPv6 PBR */
        if (sys_aclqos_entry_ctl.pbr_ipv6_acl_entry_num > 0)
        {
            if (sys_aclqos_entry_ctl.global_pbr_entry_tail_num > 0)
            {
                opf.pool_type = ACL_PBR_IPV6_KEY_TAIL;
            }
            else
            {
                opf.pool_type = ACL_PBR_IPV6_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_offset(&opf, 1, &index));

            kal_memset(&ds_ipv6_key, 0, sizeof(ds_ipv6_pbr_dualda_key_t));
            kal_memset(&ds_ipv6_mask, 0, sizeof(ds_ipv6_pbr_dualda_key_t));
            ds_ipv6_key.table_id0 = PBR_IPV6_TABLEID0_C;
            ds_ipv6_key.table_id1 = PBR_IPV6_TABLEID1_C;
            ds_ipv6_key.table_id2 = PBR_IPV6_TABLEID2_C;
            ds_ipv6_key.table_id3 = PBR_IPV6_TABLEID3_C;
            ds_ipv6_key.table_id4 = PBR_IPV6_TABLEID4_C;
            ds_ipv6_key.table_id5 = PBR_IPV6_TABLEID5_C;
            ds_ipv6_key.table_id6 = PBR_IPV6_TABLEID6_C;
            ds_ipv6_key.ipv6_rtk_table_id7 = PBR_IPV6_TABLEID7_C;
            ds_ipv6_mask.table_id0 = 0xF;
            ds_ipv6_mask.table_id1 = 0xF;
            ds_ipv6_mask.table_id2 = 0xF;
            ds_ipv6_mask.table_id3 = 0xF;
            ds_ipv6_mask.table_id4 = 0xF;
            ds_ipv6_mask.table_id5 = 0xF;
            ds_ipv6_mask.table_id6 = 0xF;
            ds_ipv6_mask.ipv6_rtk_table_id7 = 0xF;
            ds_key.data_entry = (uint32 *)&ds_ipv6_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv6_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_PBR_DUALDA_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_UCAST_PBR_DUAL_DA, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_acl_add_tiny_fragment_default_entry(bool deny)
{
    sys_humber_opf_t opf;
    ds_ipv4_acl_t ds_action;
    tbl_entry_t  ds_key;
    ds_acl_ipv4_key_t ds_ipv4_key, ds_ipv4_mask;
    ds_acl_ipv6_key_t ds_ipv6_key, ds_ipv6_mask;
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 index;
    ipe_lookup_ctl_t lkp_ctl1;
    uint8  is_ext_tcam;
    uint8  key_size;

    kal_memset(&ds_action, 0, sizeof(ds_ipv4_acl_t));
    kal_memset(&ds_key, 0, sizeof(tbl_entry_t));

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        ds_action.discard_packet = deny ? 1 : 0;
        ds_action.fwd_ptr          = 0xFFF;    /*indicates fwd_ptr is not valid*/
        ds_action.qos_policy       = 0x7;      /*indicates qos_policy is not valid */
        ds_action.stats_ptr        = 0xFFFF;   /*indicates stats_ptr is not valid*/
        ds_action.flow_policer_ptr = 0xFFFF;   /*indicates flow_policer_ptr is not valid*/

        if (sys_aclqos_entry_ctl.mac_ipv4_acl_entry_num > 0)
        {
            /* IPv4 ACL */
            if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
            {
                opf.pool_type = ACL_MAC_MPLS_IPV4_KEY_HEAD;
            }
            else
            {
                opf.pool_type = ACL_MAC_MPLS_IPV4_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

            kal_memset(&ds_ipv4_key, 0, sizeof(ds_acl_ipv4_key_t));
            kal_memset(&ds_ipv4_mask, 0, sizeof(ds_acl_ipv4_key_t));
            ds_ipv4_key.tableid0 = ACL_IPV4_TABLEID0_C;
            ds_ipv4_key.tableid1 = ACL_IPV4_TABLEID1_C;
            ds_ipv4_key.tableid2 = ACL_IPV4_TABLEID2_C;
            ds_ipv4_key.tableid3 = ACL_IPV4_TABLEID3_C;
            ds_ipv4_key.frag_info = 2;
            ds_ipv4_mask.tableid0 = 0xF;
            ds_ipv4_mask.tableid1 = 0xF;
            ds_ipv4_mask.tableid2 = 0xF;
            ds_ipv4_mask.tableid3 = 0xF;
            ds_ipv4_mask.frag_info = 3;
            ds_key.data_entry = (uint32 *)&ds_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_ACL_IPV4_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_ACL, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }

        /* IPv6 ACL */
        cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &lkp_ctl1));

        is_ext_tcam = (lkp_ctl1.acl_qos_lookup_ctl0 >> 14) & 0x1;
        key_size = (lkp_ctl1.acl_qos_lookup_ctl0 >> 12) & 0x3;
        if ((sys_aclqos_entry_ctl.ipv6_acl_entry_num > 0) && is_ext_tcam && (key_size == 3))    /*The 3 means  640 bits key*/
        {
            if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
            {
                opf.pool_type = ACL_IPV6_KEY_HEAD;
            }
            else
            {
                opf.pool_type = ACL_IPV6_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

            kal_memset(&ds_ipv6_key, 0, sizeof(ds_acl_ipv6_key_t));
            kal_memset(&ds_ipv6_mask, 0, sizeof(ds_acl_ipv6_key_t));
            ds_ipv6_key.tableid0 = ACL_IPV6_TABLEID0_C;
            ds_ipv6_key.tableid1 = ACL_IPV6_TABLEID1_C;
            ds_ipv6_key.tableid2 = ACL_IPV6_TABLEID2_C;
            ds_ipv6_key.tableid3 = ACL_IPV6_TABLEID3_C;
            ds_ipv6_key.tableid4 = ACL_IPV6_TABLEID4_C;
            ds_ipv6_key.tableid5 = ACL_IPV6_TABLEID5_C;
            ds_ipv6_key.tableid6 = ACL_IPV6_TABLEID6_C;
            ds_ipv6_key.aclqos_ipv6key_tableid7 = ACL_IPV6_TABLEID7_C;
            ds_ipv6_key.frag_info = 2;
            ds_ipv6_mask.tableid0 = 0xF;
            ds_ipv6_mask.tableid1 = 0xF;
            ds_ipv6_mask.tableid2 = 0xF;
            ds_ipv6_mask.tableid3 = 0xF;
            ds_ipv6_mask.tableid4 = 0xF;
            ds_ipv6_mask.tableid5 = 0xF;
            ds_ipv6_mask.tableid6 = 0xF;
            ds_ipv6_mask.aclqos_ipv6key_tableid7 = 0xF;
            ds_ipv6_mask.frag_info = 3;
            ds_key.data_entry = (uint32 *)&ds_ipv6_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv6_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_ACL_IPV6_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_ACL, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }

        /*for dual acl lookup, should add tiny fragment default entry into qos table*/
        if(!sys_aclqos_entry_ctl.is_dual_aclqos_lookup)
        {
            if (sys_aclqos_entry_ctl.mac_ipv4_qos_entry_num > 0)
            {
                /* IPv4 ACL */
                if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
                {
                    opf.pool_type = QOS_MAC_MPLS_IPV4_KEY_HEAD;
                }
                else
                {
                    opf.pool_type = QOS_MAC_MPLS_IPV4_KEY;
                }
                opf.pool_index = lchip;
                CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

                kal_memset(&ds_ipv4_key, 0, sizeof(ds_acl_ipv4_key_t));
                kal_memset(&ds_ipv4_mask, 0, sizeof(ds_acl_ipv4_key_t));
                ds_ipv4_key.tableid0 = QOS_IPV4_TABLEID0_C;
                ds_ipv4_key.tableid1 = QOS_IPV4_TABLEID1_C;
                ds_ipv4_key.tableid2 = QOS_IPV4_TABLEID2_C;
                ds_ipv4_key.tableid3 = QOS_IPV4_TABLEID3_C;
                ds_ipv4_key.frag_info = 2;
                ds_ipv4_mask.tableid0 = 0xF;
                ds_ipv4_mask.tableid1 = 0xF;
                ds_ipv4_mask.tableid2 = 0xF;
                ds_ipv4_mask.tableid3 = 0xF;
                ds_ipv4_mask.frag_info = 3;
                ds_key.data_entry = (uint32 *)&ds_ipv4_key;
                ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

                cmd = DRV_IOW(IOC_TABLE, DS_QOS_IPV4_KEY, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

                cmd = DRV_IOW(IOC_TABLE, DS_IPV4_QOS, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
            }

            /* IPv6 ACL */
            if ((sys_aclqos_entry_ctl.ipv6_qos_entry_num > 0) && is_ext_tcam && (key_size == 3))    /*The 3 means  640 bits key*/
            {
                if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
                {
                    opf.pool_type = QOS_IPV6_KEY_HEAD;
                }
                else
                {
                    opf.pool_type = QOS_IPV6_KEY;
                }
                opf.pool_index = lchip;
                CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

                kal_memset(&ds_ipv6_key, 0, sizeof(ds_acl_ipv6_key_t));
                kal_memset(&ds_ipv6_mask, 0, sizeof(ds_acl_ipv6_key_t));
                ds_ipv6_key.tableid0 = QOS_IPV6_TABLEID0_C;
                ds_ipv6_key.tableid1 = QOS_IPV6_TABLEID1_C;
                ds_ipv6_key.tableid2 = QOS_IPV6_TABLEID2_C;
                ds_ipv6_key.tableid3 = QOS_IPV6_TABLEID3_C;
                ds_ipv6_key.tableid4 = QOS_IPV6_TABLEID4_C;
                ds_ipv6_key.tableid5 = QOS_IPV6_TABLEID5_C;
                ds_ipv6_key.tableid6 = QOS_IPV6_TABLEID6_C;
                ds_ipv6_key.aclqos_ipv6key_tableid7 = QOS_IPV6_TABLEID7_C;
                ds_ipv6_key.frag_info = 2;
                ds_ipv6_mask.tableid0 = 0xF;
                ds_ipv6_mask.tableid1 = 0xF;
                ds_ipv6_mask.tableid2 = 0xF;
                ds_ipv6_mask.tableid3 = 0xF;
                ds_ipv6_mask.tableid4 = 0xF;
                ds_ipv6_mask.tableid5 = 0xF;
                ds_ipv6_mask.tableid6 = 0xF;
                ds_ipv6_mask.aclqos_ipv6key_tableid7 = 0xF;
                ds_ipv6_mask.frag_info = 3;
                ds_key.data_entry = (uint32 *)&ds_ipv6_key;
                ds_key.mask_entry = (uint32 *)&ds_ipv6_mask;

                cmd = DRV_IOW(IOC_TABLE, DS_QOS_IPV6_KEY, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

                cmd = DRV_IOW(IOC_TABLE, DS_IPV6_QOS, DRV_ENTRY_FLAG);
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
            }

        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_qos_add_tiny_fragment_default_entry(bool deny)
{
    sys_humber_opf_t opf;
    ds_ipv4_acl_t ds_action;
    tbl_entry_t  ds_key;
    ds_acl_ipv4_key_t ds_ipv4_key, ds_ipv4_mask;
    ds_acl_ipv6_key_t ds_ipv6_key, ds_ipv6_mask;
    uint8  lchip, lchip_num;
    uint32 cmd;
    uint32 index;
    ipe_lookup_ctl_t lkp_ctl1;
    uint8  is_ext_tcam;
    uint8  key_size;

    kal_memset(&ds_action, 0, sizeof(ds_ipv4_acl_t));
    kal_memset(&ds_key, 0, sizeof(tbl_entry_t));

    if (!sys_aclqos_entry_ctl.is_dual_aclqos_lookup)
    {
        /*not enable qos*/
        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        ds_action.discard_packet = deny ? 1 : 0;
        ds_action.fwd_ptr          = 0xFFF;    /*indicates fwd_ptr is not valid*/
        ds_action.qos_policy       = 0x7;      /*indicates qos_policy is not valid */
        ds_action.stats_ptr        = 0xFFFF;   /*indicates stats_ptr is not valid*/
        ds_action.flow_policer_ptr = 0xFFFF;   /*indicates flow_policer_ptr is not valid*/

        if (sys_aclqos_entry_ctl.mac_ipv4_qos_entry_num > 0)
        {
            /* IPv4 QOS */
            if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
            {
                opf.pool_type = QOS_MAC_MPLS_IPV4_KEY_HEAD;
            }
            else
            {
                opf.pool_type = QOS_MAC_MPLS_IPV4_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

            kal_memset(&ds_ipv4_key, 0, sizeof(ds_qos_ipv4_key_t));
            kal_memset(&ds_ipv4_mask, 0, sizeof(ds_qos_ipv4_key_t));
            ds_ipv4_key.tableid0 = QOS_IPV4_TABLEID0_C;
            ds_ipv4_key.tableid1 = QOS_IPV4_TABLEID1_C;
            ds_ipv4_key.tableid2 = QOS_IPV4_TABLEID2_C;
            ds_ipv4_key.tableid3 = QOS_IPV4_TABLEID3_C;
            ds_ipv4_key.frag_info = 2;
            ds_ipv4_mask.tableid0 = 0xF;
            ds_ipv4_mask.tableid1 = 0xF;
            ds_ipv4_mask.tableid2 = 0xF;
            ds_ipv4_mask.tableid3 = 0xF;
            ds_ipv4_mask.frag_info = 3;
            ds_key.data_entry = (uint32 *)&ds_ipv4_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv4_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_QOS_IPV4_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV4_QOS, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }

        /* IPv6 QOS */
        cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &lkp_ctl1));

        is_ext_tcam = (lkp_ctl1.acl_qos_lookup_ctl0 >> 14) & 0x1;
        key_size = (lkp_ctl1.acl_qos_lookup_ctl0 >> 12) & 0x3;
        if ((sys_aclqos_entry_ctl.ipv6_qos_entry_num > 0) && is_ext_tcam && (key_size == 3))    /*The 3 means  640 bits key*/
        {
            if (sys_aclqos_entry_ctl.global_aclqos_entry_head_num > 0)
            {
                opf.pool_type = QOS_IPV6_KEY_HEAD;
            }
            else
            {
                opf.pool_type = QOS_IPV6_KEY;
            }
            opf.pool_index = lchip;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 0, &index));

            kal_memset(&ds_ipv6_key, 0, sizeof(ds_qos_ipv6_key_t));
            kal_memset(&ds_ipv6_mask, 0, sizeof(ds_qos_ipv6_key_t));
            ds_ipv6_key.tableid0 = QOS_IPV6_TABLEID0_C;
            ds_ipv6_key.tableid1 = QOS_IPV6_TABLEID1_C;
            ds_ipv6_key.tableid2 = QOS_IPV6_TABLEID2_C;
            ds_ipv6_key.tableid3 = QOS_IPV6_TABLEID3_C;
            ds_ipv6_key.tableid4 = QOS_IPV6_TABLEID4_C;
            ds_ipv6_key.tableid5 = QOS_IPV6_TABLEID5_C;
            ds_ipv6_key.tableid6 = QOS_IPV6_TABLEID6_C;
            ds_ipv6_key.aclqos_ipv6key_tableid7 = QOS_IPV6_TABLEID7_C;
            ds_ipv6_key.frag_info = 2;
            ds_ipv6_mask.tableid0 = 0xF;
            ds_ipv6_mask.tableid1 = 0xF;
            ds_ipv6_mask.tableid2 = 0xF;
            ds_ipv6_mask.tableid3 = 0xF;
            ds_ipv6_mask.tableid4 = 0xF;
            ds_ipv6_mask.tableid5 = 0xF;
            ds_ipv6_mask.tableid6 = 0xF;
            ds_ipv6_mask.aclqos_ipv6key_tableid7 = 0xF;
            ds_ipv6_mask.frag_info = 3;
            ds_key.data_entry = (uint32 *)&ds_ipv6_key;
            ds_key.mask_entry = (uint32 *)&ds_ipv6_mask;

            cmd = DRV_IOW(IOC_TABLE, DS_QOS_IPV6_KEY, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_key));

            cmd = DRV_IOW(IOC_TABLE, DS_IPV6_QOS, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_action));
        }
    }

    return CTC_E_NONE;
}


/**
 @brief acl/qos init lookup control register
*/
static int32
_sys_humber_aclqos_init_lookup_ctl()
{
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 cmd = 0;
    ipe_lookup_ctl_t ipe_lookup_ctl;
	epe_acl_qos_ctl_t epe_aclqos_ctl;

    kal_memset(&ipe_lookup_ctl, 0, sizeof(ipe_lookup_ctl));
	kal_memset(&epe_aclqos_ctl, 0, sizeof(epe_aclqos_ctl));

    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        /* read IPE_LOOKUP_CTL */
        cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_lookup_ctl));

        /*init dual_acl_lookup*/
        ipe_lookup_ctl.dual_acl_lookup = !sys_aclqos_entry_ctl.is_dual_aclqos_lookup;

        /* write IPE_LOOKUP_CTL */
        cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_lookup_ctl));

		/* read EPE_LOOKUP_CTL */
        cmd = DRV_IOR(IOC_REG, EPE_ACL_QOS_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &epe_aclqos_ctl));

        /*init dual_acl_lookup*/
        epe_aclqos_ctl.dual_acl_lookup = !sys_aclqos_entry_ctl.is_dual_aclqos_lookup;

        /* write EPE_LOOKUP_CTL */
        cmd = DRV_IOW(IOC_REG, EPE_ACL_QOS_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &epe_aclqos_ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief acl/qos ctl init
*/
static int32
_sys_humber_aclqos_init_ctl()
{
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 cmd = 0;
    uint32 ds_fwd_ptr_base = 0;

    ds_fwd_ptr_base = sys_aclqos_entry_ctl.acl_fwd_base >> 8;
    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        /* write IPE_ACL_QOS_CTL */
        cmd = DRV_IOW(IOC_REG, IPE_ACL_QOS_CTL, IPE_ACL_QOS_CTL_DS_FWD_PTR_BASE);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ds_fwd_ptr_base));
    }

    return CTC_E_NONE;

}

/**
 @brief acl/qos entry component initialization
*/
int32
sys_humber_aclqos_entry_init(ctc_aclqos_global_cfg_t* aclqos_global_cfg)
{
    uint8 chip_id, lchip_num;
    uint32 entry_num;
    sys_humber_opf_t opf;
    uint32 glb_met_entry_num = 0, local_met_dsfwd_entry_num = 0;
    uint32 fwd_opf_start_offset = 0;
    uint32 acl_head = 0, acl_tail = 0;
    uint32 pbr_head = 0, pbr_tail = 0;
    sys_acl_block_t*    pb;

    CTC_MAX_VALUE_CHECK(aclqos_global_cfg->entry_sort_mode, MAX_CTC_ACLQOS_ENTRY_SORT_MODE-1);

    kal_memset(&sys_aclqos_entry_ctl, 0, sizeof(sys_aclqos_entry_ctl));

    sys_aclqos_entry_ctl.entry_sort_mode = aclqos_global_cfg->entry_sort_mode;
    
    sys_alloc_info_t *alloc_info = 0;
    alloc_info = sys_alloc_get_alloc_info_ptr();
    sys_aclqos_entry_ctl.disable_merge_mac_ip_key_physical = alloc_info->disable_merge_mac_ip_key_physical;

    /* when disable_merge_mac_ip_key_physical, only support per system sort */
    if (sys_aclqos_entry_ctl.disable_merge_mac_ip_key_physical
        && (sys_aclqos_entry_ctl.entry_sort_mode != CTC_ACLQOS_ENTRY_SORT_MODE_PER_SYSTEM))
    {
        return CTC_E_NOT_SUPPORT;
    }

    acl_head = aclqos_global_cfg->global_aclqos_entry_head_num;
    acl_tail = aclqos_global_cfg->global_aclqos_entry_tail_num;
    pbr_head = aclqos_global_cfg->global_pbr_entry_head_num;
    pbr_tail = aclqos_global_cfg->global_pbr_entry_tail_num;
    if (CTC_ACLQOS_ENTRY_SORT_MODE_PER_SYSTEM == sys_aclqos_entry_ctl.entry_sort_mode)
    {
        /* only label entry mode has head and tail */
        acl_head = 0;
        acl_tail = 0;
        pbr_head = 0;
        pbr_tail = 0;
    }

    sys_aclqos_entry_ctl.global_aclqos_entry_head_num = acl_head;
    sys_aclqos_entry_ctl.global_aclqos_entry_tail_num = acl_tail;
    sys_aclqos_entry_ctl.global_pbr_entry_head_num = pbr_head;
    sys_aclqos_entry_ctl.global_pbr_entry_tail_num = pbr_tail;

    CTC_ERROR_RETURN(
        sys_alloc_get_dual_lookup_en(&sys_aclqos_entry_ctl.is_dual_aclqos_lookup,
                                     &sys_aclqos_entry_ctl.is_merge_mac_ip_key));

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        p_sys_acl_redirect_hash[chip_id] = ctc_hash_create(1,ACL_REDIRECT_BUCKET_SIZE,
                                    _sys_humber_acl_redirect_hash_key,
                                    _sys_humber_acl_redirect_hash_cmp);
    }

    /*acl redirect*/
    if(aclqos_global_cfg->acl_redirect_fwd_ptr_num > 0)
    {
        CTC_ERROR_RETURN(sys_alloc_get_met_dsfwd_table_info(&glb_met_entry_num, &local_met_dsfwd_entry_num));
        sys_aclqos_entry_ctl.acl_fwd_base = glb_met_entry_num + local_met_dsfwd_entry_num - aclqos_global_cfg->acl_redirect_fwd_ptr_num;
        fwd_opf_start_offset = sys_aclqos_entry_ctl.acl_fwd_base & 0x000000FF;
    }

    CTC_ERROR_RETURN(_sys_humber_aclqos_init_lookup_ctl());
    CTC_ERROR_RETURN(_sys_humber_aclqos_init_ctl());
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_MAC_MPLS_IPV4_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_IPV6_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV4_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV6_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_MAC_MPLS_IPV4_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_IPV6_KEY, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_ACL_FWD_SRAM, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_MAC_MPLS_IPV4_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_MAC_MPLS_IPV4_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_IPV6_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_IPV6_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV4_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV4_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV6_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(ACL_PBR_IPV6_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_MAC_MPLS_IPV4_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_MAC_MPLS_IPV4_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_IPV6_KEY_HEAD, CTC_MAX_LOCAL_CHIP_NUM));
    CTC_ERROR_RETURN(sys_humber_opf_init(QOS_IPV6_KEY_TAIL, CTC_MAX_LOCAL_CHIP_NUM));

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        /* ACL redirect */
        if(aclqos_global_cfg->acl_redirect_fwd_ptr_num > 0)
        {
            opf.pool_type = OPF_ACL_FWD_SRAM;
            opf.pool_index = chip_id;
            CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, fwd_opf_start_offset, aclqos_global_cfg->acl_redirect_fwd_ptr_num));
        }
    }

    sys_aclqos_entry_ctl.entry = ctc_hash_create(
            SYS_ACL_HASH_BLOCK_NUM, 
            SYS_ACL_HASH_BLOCK_SIZE, 
            _sys_humber_acl_entry_hash_key,
            _sys_humber_acl_entry_hash_cmp);
    if (sys_aclqos_entry_ctl.disable_merge_mac_ip_key_physical)
    {
        uint32 size;
        
        CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_QOS_IPV4_KEY, &entry_num));
        pb = &sys_aclqos_entry_ctl.block[SYS_ACL_ASIC_TYPE_IPV4];
        pb->entry_dft_max = 5;
        pb->entry_count = entry_num - pb->entry_dft_max;
        pb->free_count  = pb->entry_count;
        pb->block_type  = SYS_ACL_ASIC_TYPE_IPV4;

        size = sizeof(sys_acl_entry_t*) * entry_num;
        pb->entries = (sys_acl_entry_t**)mem_malloc
                (MEM_ACLQOS_MODULE, size);
        
        if (NULL == pb->entries && size)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(pb->entries, 0, size);

        /* mac key use IPV4's entry number */
        CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_QOS_IPV4_KEY, &entry_num));
        pb = &sys_aclqos_entry_ctl.block[SYS_ACL_ASIC_TYPE_MAC];
        pb->entry_dft_max = 5;
        pb->entry_count = entry_num - pb->entry_dft_max;
        pb->free_count  = pb->entry_count;
        pb->block_type  = SYS_ACL_ASIC_TYPE_MAC;
        pb->lchip       = 0;

        size = sizeof(sys_acl_entry_t*) * entry_num;
        pb->entries = (sys_acl_entry_t**)mem_malloc
                (MEM_ACLQOS_MODULE, size);
        
        if (NULL == pb->entries && size)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(pb->entries, 0, size);

    }
    acl_master->asic_type[CTC_ACLQOS_MAC_KEY]  = SYS_ACL_ASIC_TYPE_MAC;
    acl_master->asic_type[CTC_ACLQOS_IPV4_KEY] = SYS_ACL_ASIC_TYPE_IPV4;


    CTC_ERROR_RETURN(_sys_humber_aclqos_add_default_entry(FALSE));
    CTC_ERROR_RETURN(_sys_humber_acl_pbr_add_default_entry());

    return CTC_E_NONE;
}


