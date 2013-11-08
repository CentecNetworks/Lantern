/**
 @file sys_humber_parser.h

 @date 2009-12-22

 @version v2.0

---file comments----
*/

#ifndef _SYS_HUMBER_PARSER_H
#define _SYS_HUMBER_PARSER_H

#include "kal.h"
#include "ctc_const.h"
#include "ctc_parser.h"


#define SYS_PARSER_DEBUG_INFO(FMT, ...) \
            { \
                CTC_DEBUG_OUT_INFO(parser, parser, PARSER_SYS,FMT,##__VA_ARGS__);\
            }

#define SYS_PARSER_DEBUG_FUNC() \
            {\
                CTC_DEBUG_OUT_FUNC(parser, parser, PARSER_SYS);\
            }


/*set tpid macro*/
#define SYS_PAS_ETH_CVLAN_TPID          0x8100
#define SYS_PAS_ETH_BVLAN_TPID          0x88a8
#define SYS_PAS_ETH_SVLAN_TPID0        0x88a8
#define SYS_PAS_ETH_SVLAN_TPID1         0x9100
#define SYS_PAS_ETH_SVLAN_TPID2         0x8100
#define SYS_PAS_ETH_SVLAN_TPID3         0x88a8
#define SYS_PAS_ETH_ITAG_TPID               0x88e7

#define SYS_PAS_ETH_MAX_LENGTH          1536
#define SYS_PAS_ETH_VLAN_PAS_NUM     2

#define SYS_PAS_PBB_VLAN_PAS_NUM    2

/**
 @brief set l2 hash field
*/
extern int32
sys_humber_parser_set_l2_hash(ctc_parser_l2_ctl_fld_t *l2ctl);

/**
 @brief get hash computing field
*/
extern int32
sys_humber_parser_get_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl);

/**
 @brief set tpid
*/
extern int32
sys_humber_parser_set_tpid(ctc_parser_l2_tpid_t type, uint16 tpid);

/**
 @brief get tpid with some type
*/
extern int32
sys_humber_parser_get_tpid(ctc_parser_l2_tpid_t type, uint16* tpid);

/**
 @brief set snap allow none zero oui
*/
extern int32
sys_humber_parser_set_snap_allow_none_zero_oui(uint8 enable);

/**
 @brief get snap allow none zero oui
*/
extern int32
sys_humber_parser_get_snap_allow_none_zero_oui(uint8* enable);

/**
 @brief set max_length,based on the value differentiate type or length
*/
extern int32
sys_humber_parser_set_max_length_filed(uint16 max_length);

/**
 @brief get max_length value
*/
extern int32
sys_humber_parser_get_max_length_filed(uint16* max_length);

/**
 @brief set parser quad vlan enable
*/
extern int32
sys_humber_parser_set_parser_quad_vlan_en(uint8 enable);

/**
 @brief get parser quad vlan
*/
extern int32
sys_humber_parser_get_parser_quad_vlan_en(uint8* enable);

/**
 @brief set vlan parser num
*/
extern int32
sys_humber_parser_set_vlan_parser_num(uint8 vlan_num);

/**
 @brief get vlan parser num
*/
extern int32
sys_humber_parser_get_vlan_parser_num(uint8* vlan_num);

/**
 @brief set pbb parser ctl info
*/
extern int32
sys_humber_parser_set_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl);

/**
 @brief get pbb parser ctl info
*/
extern int32
sys_humber_parser_get_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl);

/**
 @brief set l2 flex ctl info
*/
extern int32
sys_humber_parser_set_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl);

/**
 @brief get l2 flex ctl info
*/
extern int32
sys_humber_parser_get_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl);

/**
 @brief add l2type
*/
extern int32
sys_humber_parser_add_l2_type(uint8 index, ctc_parser_l2_ptl_entry_t* entry);

/**
 @brief get l3type by protocol
*/
extern int32
sys_humber_parser_get_l2_type(ctc_parser_l2_ptl_entry_t *p_entry, ctc_parser_l3_type_t *p_l3_type);

/**
 @brief set the entry invalid based on the index
*/
extern int32
sys_humber_parser_remove_l2_type(uint8 index);

extern int32
sys_humber_parser_l2_enable_l3_type(ctc_parser_l3_type_t l3_type, bool enable);

/**
 @brief set ip hash ctl reg
*/
extern int32
sys_humber_parser_set_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl);

/**
 @brief get ip hash ctl reg info
*/
extern int32
sys_humber_parser_get_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl);

/**
 @brief set parser ipv6 ctl reg
*/
extern int32
sys_humber_parser_set_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl);

/**
 @brief get parser ipv6 ctl reg
*/
extern int32
sys_humber_parser_get_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl);

/**
 @brief set parser mpls ctl reg
*/
extern int32
sys_humber_parser_set_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl);

/**
 @brief get parser mpls ctl reg
*/
extern int32
sys_humber_parser_get_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl);

/**
 @brief set parser l3flex ctl reg
*/
extern int32
sys_humber_parser_set_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl);

/**
 @brief get parser l3flex ctl reg
*/
extern int32
sys_humber_parser_get_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl);

/**
 @brief add l3type,can add a new l3type,addition offset for the type,can get the layer4 type
*/
extern int32
sys_humber_parser_add_l3_type(uint8 index, ctc_parser_l3_ptl_entry_t* entry);

/**
 @brief set the entry invalid based on the index
*/
extern int32
sys_humber_parser_remove_l3_type(uint8 index);

extern int32
sys_humber_parser_l3_enable_l4_type(ctc_parser_l4_type_t l4_type, bool enable);

/**
 @brief set layer4 flag option ctl
*/
int32
sys_humber_parser_set_layer4_flag_op_ctl(uint8 lchip, uint8 index, ctc_parser_l4flag_op_ctl_t* l4flag_op_ctl);

/**
 @brief set layer4 port option selection
*/
int32
sys_humber_parser_set_layer4_port_op_sel(uint8 lchip, ctc_parser_l4_port_op_sel_t* l4port_op_sel);

/**
 @brief set layer4 port option ctl
*/
int32
sys_humber_parser_set_layer4_port_op_ctl(uint8 lchip, uint8 index, ctc_parser_l4_port_op_ctl_t* l4port_op_ctl);


/**
 @brief set layer4 parser reg
*/
extern int32
sys_humber_parser_set_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl);

/**
 @brief get layer4 parser info
*/
extern int32
sys_humber_parser_get_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl);

/**
 @brief set layer4 type for applicaiton
*/
extern int32
sys_humber_parser_set_layer4_type(uint8 index, ctc_parser_l4_ptl_entry_t* entry);


/**
 @brief init parser module
*/
extern int32
sys_humber_parser_init(void);

#endif

