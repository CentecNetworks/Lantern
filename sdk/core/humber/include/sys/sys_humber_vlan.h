/**
 @file sys_humber_vlan.h

 @date 2009-10-17

 @version v2.0

*/
 #ifndef _SYS_HUMBER_VLAN_H
 #define _SYS_HUMBER_VLAN_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"
#include "ctc_vector.h"
#include "ctc_vlan.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define SYS_MAX_VLAN_BLOCK_NUM  4

#define SYS_VLAN_SWITCH_PTR     0                       /*reserve for vlan switching*/
#define SYS_VLAN_SVQ_PTR        4095                    /*reserve for service queue*/
#define SYS_VLAN_VPWS_PTR       (4096 + 768 - 1)        /*reserve for vpws*/
#define SYS_VLAN_VPLS_PTR_LRN_EN    (SYS_VLAN_VPWS_PTR - 1) /*reserve for vpls (learning enable)*/
#define SYS_VLAN_VPLS_PTR_LRN_DIS   (SYS_VLAN_VPWS_PTR - 2) /*reserve for vpls (learning disable)*/
#define SYS_VLAN_VPLS_PTR_MACLIMIT_EN  (SYS_VLAN_VPWS_PTR - 3) /*reserve for vpls (MAC limit enable)*/
#define SYS_VLAN_INVALID_PTR    0xFFFF

#define SYS_VLAN_ROUTED_PORT    512                     /*number of vlan for phy-if & sub-if*/
#define SYS_VLAN_ROUTED_PORT_BASE   (CTC_MAX_VLAN_ID + 1)

#define VLAN_LOCK \
        if (p_vlan_master->p_vlan_mutex) kal_mutex_lock(p_vlan_master->p_vlan_mutex)

#define VLAN_UNLOCK \
        if(p_vlan_master->p_vlan_mutex) kal_mutex_unlock(p_vlan_master->p_vlan_mutex)

#define CTC_ERROR_RETURN_WITH_VLAN_UNLOCK(op)\
        { \
            int32 rv; \
            if((rv = (op)) < 0) \
            { \
                kal_mutex_unlock(p_vlan_master->p_vlan_mutex);\
                return (rv);\
            };\
        }

#define SYS_VLAN_DEBUG_INFO(FMT, ...) \
        { \
            CTC_DEBUG_OUT_INFO(vlan, vlan, VLAN_SYS,FMT,##__VA_ARGS__);\
        }

#define SYS_VLAN_DEBUG_FUNC() \
        {\
            CTC_DEBUG_OUT_FUNC(vlan, vlan, VLAN_SYS);\
        }

#define VLAN_DB_SET(old, cur)\
        {\
            if (cur == old)\
            {\
                VLAN_UNLOCK;\
                return CTC_E_NONE;\
            }\
            else\
            {\
                old = cur;\
            }\
        }

#define VLAN_DB_GET(p_value, old)\
        {\
            *p_value = old;\
        }

enum sys_vlanptr_type_e
{
    SYS_VLAN_PTR_TYPE_VID,
    SYS_VLAN_PTR_TYPE_PHY_IF,
    SYS_VLAN_PTR_TYPE_SUB_IF,
    SYS_VLAN_PTR_TYPE_VLAN_IF,
    SYS_VLAN_PTR_TYPE_VPLS_LRN_DIS,
    SYS_VLAN_PTR_TYPE_VPLS_LRN_EN,
    SYS_VLAN_PTR_TYPE_VPLS_MACLIMIT_EN,
    SYS_VLAN_PTR_TYPE_VPWS,
};
typedef enum sys_vlanptr_type_e sys_vlanptr_type_t;


struct sys_vlan_info_s
{
    sys_vlanptr_type_t vlan_ptr_type;
    uint16 vid;
    uint16 gport;

    uint16 if_id;
};
typedef struct sys_vlan_info_s sys_vlan_info_t;

struct sys_vlan_prop_s
{
    uint8 trans_en;
    uint8 route_en;
    uint8 rece_en;
    uint8 learn_en;

    uint8 stp_id;
    uint8 bridge_en;
    uint16 vrf_id;

    uint16 if_id;
    uint16 resv;
};
typedef struct sys_vlan_prop_s sys_vlan_prop_t;

struct sys_vlan_master_s
{
    kal_mutex_t *p_vlan_mutex;
    ctc_vector_t *p_vlan_vector;
    sys_vlan_info_t *routed_port_vlan;
};
typedef struct sys_vlan_master_s sys_vlan_master_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_vlan_init(void);

extern int32
sys_humber_vlan_create_vlan(sys_vlan_info_t *p_vlan_info);
extern int32
sys_humber_vlan_remove_vlan(sys_vlan_info_t *p_vlan_info);

extern int32
sys_humber_vlan_add_port(sys_vlan_info_t *p_vlan_info, uint16 gport);
extern int32
sys_humber_vlan_remove_port(sys_vlan_info_t *p_vlan_info, uint16 gport);

extern int32
sys_humber_vlan_get_ports(sys_vlan_info_t *p_vlan_info, ctc_port_bitmap_t *port_bitmap);

extern int32
sys_humber_vlan_set_receive_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_receive_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_transmit_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_transmit_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_fid(sys_vlan_info_t *p_vlan_info, uint16 fid);
extern int32
sys_humber_vlan_get_fid(sys_vlan_info_t *p_vlan_info, uint16 *fid);

extern int32
sys_humber_vlan_set_vrfid(sys_vlan_info_t *p_vlan_info, uint16 vrfid);
extern int32
sys_humber_vlan_get_vrfid(sys_vlan_info_t *p_vlan_info, uint16 *vrfid);

extern int32
sys_humber_vlan_set_bridge_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_bridge_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_replace_dscp_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_replace_dscp_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_ether_oam_valid(sys_vlan_info_t *p_vlan_info, bool valid);
extern int32
sys_humber_vlan_get_ether_oam_valid(sys_vlan_info_t *p_vlan_info, bool *valid);

extern int32
sys_humber_vlan_set_md_level(sys_vlan_info_t *p_vlan_info, uint8 md_level);
extern int32
sys_humber_vlan_get_md_level(sys_vlan_info_t *p_vlan_info, uint8 *md_level);

extern int32
sys_humber_vlan_set_egress_ether_oam_valid(sys_vlan_info_t *p_vlan_info, uint8 valid);
extern int32
sys_humber_vlan_get_egress_ether_oam_valid(sys_vlan_info_t *p_vlan_info, uint8 *valid);

extern int32
sys_humber_vlan_set_egress_md_level(sys_vlan_info_t *p_vlan_info, uint8 md_level);
extern int32
sys_humber_vlan_get_egress_md_level(sys_vlan_info_t *p_vlan_info, uint8 *md_level);

extern int32
sys_humber_vlan_set_igmp_snoop_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_igmp_snoop_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_learning_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_learning_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_stpid(sys_vlan_info_t *p_vlan_info, uint8 stpid);
extern int32
sys_humber_vlan_get_stpid(sys_vlan_info_t *p_vlan_info, uint8 *stpid);

extern int32
sys_humber_vlan_set_l3if_id(sys_vlan_info_t *p_vlan_info, uint16 if_id);
extern int32
sys_humber_vlan_get_l3if_id(sys_vlan_info_t *p_vlan_info, uint16 *if_id);

extern int32
sys_humber_vlan_set_src_queue_select(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_src_queue_select(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_dhcp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t type);
extern int32
sys_humber_vlan_get_dhcp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t *type);

extern int32
sys_humber_vlan_set_arp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t type);
extern int32
sys_humber_vlan_get_arp_excp_type(sys_vlan_info_t *p_vlan_info, ctc_exception_type_t *type);

extern int32
sys_humber_vlan_set_mac_security_vlan_discard(sys_vlan_info_t *p_vlan_info, bool discard);
extern int32
sys_humber_vlan_get_mac_security_vlan_discard(sys_vlan_info_t *p_vlan_info, bool *discard);

extern int32
sys_humber_vlan_set_vlan_security_excp_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_vlan_security_excp_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_get_vlan_ptr(sys_vlan_info_t *p_vlan_info, uint16 *vlan_ptr);

/*********************************************************
                 IPV4/IPV6  configuration
**********************************************************/
extern int32
sys_humber_vlan_set_route_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_route_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_v4ucast_en(sys_vlan_info_t * p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_v4ucast_en(sys_vlan_info_t * p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_v4mcast_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_v4mcast_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_v6ucast_en(sys_vlan_info_t * p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_v6ucast_en(sys_vlan_info_t * p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_v6mcast_en(sys_vlan_info_t *p_vlan_info, bool enable);
extern int32
sys_humber_vlan_get_v6mcast_en(sys_vlan_info_t *p_vlan_info, bool *enable);

extern int32
sys_humber_vlan_set_v4ucastSa_type(sys_vlan_info_t * p_vlan_info, uint8  ipsa_lkup_type);
extern int32
sys_humber_vlan_get_v4ucastSa_type(sys_vlan_info_t * p_vlan_info, uint8 *ipSa_lkup_type);

extern int32
sys_humber_vlan_set_v6ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8  ipsa_lkup_type);
extern int32
sys_humber_vlan_get_v6ucastSa_type(sys_vlan_info_t *p_vlan_info, uint8 *ipSa_lkup_type);

#endif

