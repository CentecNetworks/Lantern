/**
 @file sys_humber_mpls.h

 @date 2010-03-11

 @version v2.0

*/
 #ifndef _SYS_HUMBER_MPLS_H
 #define _SYS_HUMBER_MPLS_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

#include "ctc_mpls.h"
#include "ctc_const.h"
#include "ctc_stats.h"

/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define SYS_MPLS_TBL_BLOCK_SIZE   256
#define SYS_MPLS_MAX_L3VPN_VRF_ID 0xBFFE
#define SYS_MPLS_VPLS_SRC_PORT_NUM 0xFFF

#define SYS_MPLS_INIT_CHECK \
    { \
        if(p_mpls_master == NULL) \
            return CTC_E_NOT_INIT; \
    }

#define SYS_MPLS_ILM_SPACE_CHECK(spaceid)         \
    {                                                 \
        if(!p_mpls_master->space[spaceid].enable) \
            return CTC_E_INVALID_PARAM;               \
    }

#define SYS_MPLS_ILM_LABEL_CHECK(spaceid, label)         \
    {                                                 \
        if(!spaceid)         \
        { \
            if((label) >= p_mpls_master->space[spaceid].size) \
                return CTC_E_INVALID_PARAM;               \
        } \
        else if((label) < p_mpls_master->min_int_label || \
                (label) - p_mpls_master->min_int_label >= p_mpls_master->space[spaceid].size) \
            return CTC_E_INVALID_PARAM;               \
    }

#define SYS_MPLS_NHID_EXTERNAL_VALID_CHECK(nhid)           \
    {  \
        if(nhid >= p_mpls_master->max_external_nhid ){  \
            return CTC_E_INVALID_NHID;        \
        };  \
    }


#define SYS_MPLS_ILM_DATA_CHECK(p_ilm)         \
    {                                                 \
        if((p_ilm)->id_type >= CTC_MPLS_MAX_ID)   \
            return CTC_E_INVALID_PARAM;               \
        if((p_ilm)->id_type == CTC_MPLS_ID_SERVICE && \
           (p_ilm)->type != CTC_MPLS_LABEL_TYPE_VPLS && \
           (p_ilm)->type != CTC_MPLS_LABEL_TYPE_VPWS) \
            return CTC_E_INVALID_PARAM;  \
        if((p_ilm)->id_type == CTC_MPLS_ID_APS_SELECT && \
           (p_ilm)->flw_vrf_srv_aps.aps_select_grp_id >= CTC_MAX_APS_GROUP_NUM) \
            return CTC_E_INVALID_APS_GROUP_ID;  \
        if((p_ilm)->id_type == CTC_MPLS_ID_SERVICE && \
           (p_ilm)->flw_vrf_srv_aps.service_id == 0) \
            return CTC_E_INVALID_PARAM;  \
        if((p_ilm)->id_type == CTC_MPLS_ID_VRF && \
           (p_ilm)->type != CTC_MPLS_LABEL_TYPE_L3VPN) \
            return CTC_E_INVALID_PARAM;  \
        if((p_ilm)->id_type == CTC_MPLS_ID_VRF && \
           (p_ilm)->flw_vrf_srv_aps.vrf_id > SYS_MPLS_MAX_L3VPN_VRF_ID)   \
            return CTC_E_INVALID_PARAM;               \
        if((p_ilm)->type >= CTC_MPLS_MAX_LABEL_TYPE)         \
            return CTC_E_INVALID_PARAM;               \
        if((p_ilm)->model >= CTC_MPLS_MAX_TUNNEL_MODE)         \
            return CTC_E_INVALID_PARAM;               \
        if((p_ilm)->type == CTC_MPLS_LABEL_TYPE_VPLS && (p_ilm)->spaceid) \
            return CTC_E_INVALID_PARAM;  \
    }

#define SYS_MPLS_ILM_DATA_MASK(p_ilm)         \
    {                                                 \
        if((p_ilm)->type == CTC_MPLS_LABEL_TYPE_VPWS || (p_ilm)->type == CTC_MPLS_LABEL_TYPE_VPLS) \
        { \
            (p_ilm)->model = CTC_MPLS_TUNNEL_MODE_PIPE; \
        } \
        if((p_ilm)->type == CTC_MPLS_LABEL_TYPE_NORMAL || (p_ilm)->type == CTC_MPLS_LABEL_TYPE_L3VPN) \
        { \
            (p_ilm)->cwen = FALSE; \
        } \
        if((p_ilm)->type != CTC_MPLS_LABEL_TYPE_VPLS || (p_ilm)->pwid > SYS_MPLS_VPLS_SRC_PORT_NUM) \
        { \
            (p_ilm)->pwid = 0xffff; \
        } \
        if((p_ilm)->cwen) \
        { \
            (p_ilm)->cwen = TRUE; \
        } \
        if((p_ilm)->vpls_port_type) \
        { \
            (p_ilm)->vpls_port_type = TRUE; \
        } \
    }

#define SYS_MPLS_KEY_MAP(p_ctc_mpls, p_sys_mpls)   \
    {                                                  \
        (p_sys_mpls)->spaceid = (p_ctc_mpls)->spaceid; \
        (p_sys_mpls)->label = (p_ctc_mpls)->label;     \
    }

#define SYS_MPLS_DATA_MAP(p_ctc_mpls, p_sys_mpls)  \
    {                                              \
        (p_sys_mpls)->nh_id = (p_ctc_mpls)->nh_id; \
        (p_sys_mpls)->pwid = (p_ctc_mpls)->pwid;   \
        (p_sys_mpls)->id_type = (p_ctc_mpls)->id_type; \
        (p_sys_mpls)->type = (p_ctc_mpls)->type;   \
        (p_sys_mpls)->model = (p_ctc_mpls)->model;   \
        (p_sys_mpls)->cwen = (p_ctc_mpls)->cwen;   \
        (p_sys_mpls)->pop = (p_ctc_mpls)->pop;     \
        (p_sys_mpls)->vpls_port_type = (p_ctc_mpls)->vpls_port_type;     \
        (p_sys_mpls)->oam_en = (p_ctc_mpls)->oam_en;     \
    }

#define SYS_MPLS_VC_CHECK(p_vc)         \
    {                                                 \
        if((p_vc)->bindtype >= CTC_MPLS_MAX_BIND_TYPE)         \
            return CTC_E_INVALID_PARAM;               \
        if((p_vc)->bindtype != CTC_MPLS_BIND_ETHERNET && \
           (p_vc)->vlanid >= CTC_MAX_VLAN_ID) \
            return CTC_E_INVALID_VLAN_ID; \
    }

#define SYS_MPLS_L2VPN_DATA_CHECK(p_vc)         \
    {                                                 \
        if((p_vc)->l2vpntype >= CTC_MPLS_MAX_L2VPN_TYPE)         \
            return CTC_E_INVALID_PARAM;               \
        if((p_vc)->l2vpntype == CTC_MPLS_L2VPN_VPWS) \
        { \
            if(((p_vc)->u.pw_nh_id) < SYS_HUMBER_NH_RESOLVED_NHID_MAX ||      \
            ((p_vc)->u.pw_nh_id) >= (p_mpls_master->max_external_nhid))        \
                return CTC_E_INVALID_NHID;  \
        } \
        else \
        { \
            if(((p_vc)->u.vpls_info.fid) >= CTC_MAX_FID_ID) \
                return CTC_E_INVALID_FID_ID;  \
            if((p_vc)->u.vpls_info.vpls_src_port > SYS_MPLS_VPLS_SRC_PORT_NUM) \
            { \
                (p_vc)->u.vpls_info.vpls_src_port = 0xffff; \
            } \
            if((p_vc)->u.vpls_info.vpls_port_type) \
            { \
                (p_vc)->u.vpls_info.vpls_port_type = TRUE; \
            } \
        } \
    }

#define SYS_MPLS_L2VPN_AC_DATA_CHECK(p_ac)         \
    {                                                 \
        if(((p_ac))->l2vpntype >= CTC_MPLS_MAX_L2VPN_TYPE)         \
            return CTC_E_INVALID_PARAM;               \
        if(((p_ac))->l2vpntype == CTC_MPLS_L2VPN_VPWS) \
        { \
            if(((p_ac)->u.pw_nh_id) < SYS_HUMBER_NH_RESOLVED_NHID_MAX ||      \
            (((p_ac))->u.pw_nh_id) >= (p_mpls_master->max_external_nhid))        \
            return CTC_E_INVALID_NHID;                    \
        } \
        else \
        { \
            CTC_FID_RANGE_CHECK(((p_ac))->u.fid); \
        } \
    }

#define SYS_MPLS_DBG_DUMP(FMT, ...)                      \
    {                                                        \
        CTC_DEBUG_OUT_INFO(mpls, mpls, MPLS_SYS, FMT, ##__VA_ARGS__); \
    }

struct sys_mpls_ilm_s
{
    uint32 key_offset;
    uint32 label;
    uint32 nh_id;
    uint16 pwid;
    uint16 flw_vrf_srv;
    uint16 aps_group_id;
    uint8 id_type;
    uint8 spaceid;
    uint8 type;
    uint8 model;
    uint8 cwen;
    uint8 pop;
    uint8 ecpn;
    uint8 aps_select_ppath;
    uint8 vpls_port_type;
    uint8 oam_en;
    uint16 stats_ptr[CTC_MAX_LOCAL_CHIP_NUM];
};
typedef struct sys_mpls_ilm_s sys_mpls_ilm_t;

struct sys_mpls_space_s
{
    ctc_vector_t* p_vet;
    uint32 size;
    uint32 base;
    uint8 enable;
};
typedef struct sys_mpls_space_s sys_mpls_space_t;

typedef int32 (* mpls_write_ilm_t)(sys_mpls_ilm_t* p_ilm_info, void* dsmpls);
typedef int32 (* mpls_write_pw_t)(sys_usrid_vlan_entry_t* p_userid_entry, ctc_mpls_l2vpn_pw_t* p_mpls_pw);


struct sys_mpls_master_s
{
    kal_mutex_t* mutex;
    mpls_write_ilm_t write_ilm[CTC_MPLS_MAX_LABEL_TYPE];
    mpls_write_pw_t write_pw[CTC_MPLS_MAX_L2VPN_TYPE];
    uint32 min_int_label;
    sys_mpls_space_t space[CTC_MPLS_SPACE_NUMBER];
    uint32 max_external_nhid;
};
typedef struct sys_mpls_master_s sys_mpls_master_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_mpls_init(ctc_mpls_init_t* p_mpls_info);

extern int32
sys_humber_mpls_add_ilm(ctc_mpls_ilm_t* p_mpls_ilm);
extern int32
sys_humber_mpls_update_ilm(ctc_mpls_ilm_t* p_mpls_ilm);
extern int32
sys_humber_mpls_del_ilm(ctc_mpls_ilm_t* p_mpls_ilm);

extern int32
sys_humber_mpls_get_ilm(uint32 nh_id[CTC_MAX_ECPN], ctc_mpls_ilm_t* p_mpls_ilm);

extern int32
sys_humber_mpls_add_stats(ctc_mpls_stats_index_t* stats_index);

extern int32
sys_humber_mpls_del_stats(ctc_mpls_stats_index_t* stats_index);

extern int32
sys_humber_mpls_get_stats(ctc_mpls_stats_index_t* stats_index, ctc_stats_basic_t* p_stats);

extern int32
sys_humber_mpls_reset_stats(ctc_mpls_stats_index_t* stats_index);

extern int32
sys_humber_mpls_add_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw);

extern int32
sys_humber_mpls_del_l2vpn_pw(ctc_mpls_l2vpn_pw_t* p_mpls_pw);

extern int32
sys_humber_mpls_reset_in_sqn(uint8 index);

extern int32
sys_humber_mpls_reset_out_sqn(uint8 index);

extern int32
sys_humber_mpls_check_label_used(uint16 spaceid, uint32 label, bool *used);

#endif


