/**
 @file sys_humber_opf.h

 @date 2009-10-22

 @version v2.0

 opf  -offset pool freelist
*/

#ifndef _SYS_HUMBER_OPF_H_
#define _SYS_HUMBER_OPF_H_

/****************************************************************************
 *
* Header Files
*
****************************************************************************/

#include "ctc_debug.h"

 /****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define SYS_OPF_DBG_INFO(FMT, ...)                          \
             { \
                 CTC_DEBUG_OUT_INFO(opf, opf, OPF_SYS, FMT, ##__VA_ARGS__);\
             }
#define SYS_OPF_DBG_FUNC(FMT, ...)                          \
            { \
              CTC_DEBUG_OUT_FUNC(opf, opf, OPF_SYS);\
            }


enum sys_humber_opf_type
{
    /*module ower self-define,format OPF_TYPE_XXXX*/
    OPF_USRID_VLAN_KEY,
    OPF_USRID_MAC_KEY,
    OPF_USRID_IPV4_KEY,
    OPF_USRID_IPV6_KEY,
    LOCAL_MET_DSFWD_SRAM,   /*Local DsMet and DsFwd*/
    LOCAL_NEXTHOP_SRAM,     /*Nexthop 4w and 8w*/
    L2EDIT_SRAM,            /*L2edit 4w and 8w*/
    L3EDIT_SRAM,            /*L3edit 4w and 8w*/
    FDB_SRAM_HASH_COLLISION_KEY,
    NHID_INTERNAL,
    OPF_ACL_INGRESS_LABEL,
    OPF_ACL_EGRESS_LABEL,
    OPF_ACL_PBR_LABEL,
    OPF_QOS_INGRESS_LABEL,
    OPF_QOS_EGRESS_LABEL,
    ACL_MAC_MPLS_IPV4_KEY,
    ACL_IPV6_KEY,
    ACL_PBR_IPV4_KEY,
    ACL_PBR_IPV6_KEY,
    QOS_MAC_MPLS_IPV4_KEY,
    QOS_IPV6_KEY,
    OPF_QOS_FLOW_POLICER,
    OPF_QOS_FLOW_POLICER_WITH_STATS,
    OPF_QOS_POLICER_PROFILE,
    FWD_STATS_SRAM,
    OPF_QUEUE_DROP_PROFILE,
    OPF_QUEUE_SHAPE_PROFILE,
    OPF_GROUP_SHAPE_PROFILE,
    OPF_QUEUE_GROUP,
    OPF_SERVICE_QUEUE,
    OPF_OAM_TCAM_KEY, /* used by DsEthOamKey, DsPbtOamKey, DsMplsOmaLableKey.. */
    OPF_OAM_MEP_RMEP, /* used by DsEthMep, dsEthRmep, DsMplsMep, DsMplsRmep */
    OPF_OAM_MA,       /* used by DsMa */
    OPF_OAM_MA_NAME,  /* used by DsMaName */
    OPF_IPV4_UC_BLOCK,
    OPF_IPV6_UC_BLOCK,
    OPF_IPV4_MC_BLOCK,
    OPF_IPV6_MC_BLOCK,
    OPF_ACL_FWD_SRAM,
    ACL_MAC_MPLS_IPV4_KEY_HEAD,
    ACL_MAC_MPLS_IPV4_KEY_TAIL,
    ACL_IPV6_KEY_HEAD,
    ACL_IPV6_KEY_TAIL,
    ACL_PBR_IPV4_KEY_HEAD,
    ACL_PBR_IPV4_KEY_TAIL,
    ACL_PBR_IPV6_KEY_HEAD,
    ACL_PBR_IPV6_KEY_TAIL,
    QOS_MAC_MPLS_IPV4_KEY_HEAD,
    QOS_MAC_MPLS_IPV4_KEY_TAIL,
    QOS_IPV6_KEY_HEAD,
    QOS_IPV6_KEY_TAIL,
    OPF_FOAM_MEP, /* start from 2 */
    OPF_FOAM_MA,  /* start from 0 */
    OPF_TUNNEL_IPV4_SA,
    OPF_TUNNEL_IPV6_IP,
    OPF_TUNNEL_IPV4_IPUC,
    OPF_TUNNEL_IPV6_IPUC,
    OPF_FOAM_MA_NAME,  /* start from 0 */
    OPF_OFP_NH_ID,
    OPF_OFP_GLB_MET,
    OPF_OFP_GLB_NH,
    OPF_OFP_QOS_MAC_ENTRY_ID,
    OPF_OFP_QOS_IPV4_ENTRY_ID,
    OPF_OFP_QOS_MPLS_ENTRY_ID,
    OPF_OFP_TNL_SERVICE_ID,
    OPF_OFP_FLOW_ID,
    OPF_OFP_LINKAGG_TID,
    MAX_OPF_TBL_NUM
};

struct sys_humber_opf_s
{
    uint8 pool_type; /*enum sys_humber_opf_type*/
    uint8 pool_index;
    uint16 reserved ; /*reserved*/
};
typedef  struct sys_humber_opf_s sys_humber_opf_t;


struct sys_humber_opf_entry_s
 {
     struct sys_humber_opf_entry_s *prev;
     struct sys_humber_opf_entry_s *next;
     uint32 size;
     uint32 offset;
 };
typedef struct sys_humber_opf_entry_s sys_humber_opf_entry_t;

struct sys_humber_opf_master_s
{
    sys_humber_opf_entry_t ***ppp_opf_pre;
    sys_humber_opf_entry_t ***ppp_opf_rev;
    uint32             *start_offset_a[MAX_OPF_TBL_NUM];
    uint32             *max_size_a[MAX_OPF_TBL_NUM];
    uint32             *max_offset_for_pre_alloc[MAX_OPF_TBL_NUM];
    uint32             *min_offset_for_rev_alloc[MAX_OPF_TBL_NUM];
    uint8              *is_reserve[MAX_OPF_TBL_NUM];
    uint8              max_tbl_num[MAX_OPF_TBL_NUM];
};
typedef struct sys_humber_opf_master_s sys_humber_opf_master_t;

/****************************************************************************
 *
* Function
*
*****************************************************************************/

extern int32
sys_humber_opf_init( enum sys_humber_opf_type , uint8 pool_num );

extern int32
sys_humber_opf_init_offset( sys_humber_opf_t *opf , uint32 start_offset,uint32 max_size);
extern int32
sys_humber_opf_reserve_size_for_reverse_alloc(sys_humber_opf_t *opf,uint32 block_size);

extern int32
sys_humber_opf_alloc_offset(sys_humber_opf_t *opf,uint32 block_size,uint32*offset);
extern int32
sys_humber_opf_reverse_alloc_offset(sys_humber_opf_t *opf,uint32 block_size,uint32*offset);

/*alloc multiple offset*/
extern int32
sys_humber_opf_alloc_multiple_offset(sys_humber_opf_t *opf,uint8 multiple,uint32 block_size,uint32*offset);
extern int32
sys_humber_opf_reverse_alloc_multiple_offset(sys_humber_opf_t *opf,uint8 multiple,uint32 block_size,uint32*offset);

extern int32
sys_humber_opf_free_offset(sys_humber_opf_t *opf,uint32 block_size,uint32 offset);

extern int32
sys_humber_opf_print_alloc_info(sys_humber_opf_t *opf);
extern int32
sys_humber_opf_print_sample_info(sys_humber_opf_t *opf);

extern int32
sys_humber_opf_alloc_offset_from_position(sys_humber_opf_t *opf, uint32 block_size, uint32 begin);
extern int32
sys_humber_opf_alloc_offset_last(sys_humber_opf_t *opf, uint32 block_size, uint32*offset);

#endif /*_SYS_HUMBER_OPF_H_*/
