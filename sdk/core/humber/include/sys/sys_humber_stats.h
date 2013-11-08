/**
 @file sys_humber_stats.h

 @date 2009-12-15

 @version v2.0

*/
#ifndef _SYS_HUMBER_STATS_H
#define _SYS_HUMBER_STATS_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_const.h"
#include "ctc_stats.h"
#include "ctc_hash.h"


/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/\
 #define SYS_STATS_DBG_INFO(FMT, ...)                          \
                    {                                                      \
                       CTC_DEBUG_OUT_INFO(stats, stats, STATS_SYS,FMT,##__VA_ARGS__);\
                    }

#define SYS_STATS_DBG_FUNC()                          \
                    {\
                    CTC_DEBUG_OUT_FUNC(stats, stats, STATS_SYS);\
                    }

enum sys_stats_mac_ram_e
{
    SYS_STATS_MAC_STATS_RAM0,
    SYS_STATS_MAC_STATS_RAM1,
    SYS_STATS_MAC_STATS_RAM2,
    SYS_STATS_MAC_STATS_RAM3,
    SYS_STATS_MAC_STATS_RAM4,
    SYS_STATS_MAC_STATS_RAM5,
    SYS_STATS_MAC_STATS_RAM6,
    SYS_STATS_MAC_STATS_RAM7,
    SYS_STATS_MAC_STATS_RAM8,
    SYS_STATS_MAC_STATS_RAM9,
    SYS_STATS_MAC_STATS_RAM10,
    SYS_STATS_MAC_STATS_RAM11,

    SYS_STATS_CPUMAC_STATS_RAM,

    SYS_STATS_XGMAC_STATS_RAM0,
    SYS_STATS_XGMAC_STATS_RAM1,
    SYS_STATS_XGMAC_STATS_RAM2,
    SYS_STATS_XGMAC_STATS_RAM3,
    SYS_STATS_SGMAC_STATS_RAM0,
    SYS_STATS_SGMAC_STATS_RAM1,
    SYS_STATS_SGMAC_STATS_RAM2,
    SYS_STATS_SGMAC_STATS_RAM3,

    SYS_STATS_MAC_STATS_RAM_MAX
};
typedef enum sys_stats_mac_ram_e sys_stats_mac_ram_t;

/*mac statistics type*/
enum sys_stats_mac_rec_stats_type_e
{
    SYS_STATS_MAC_RCV_GOOD_UCAST,
    SYS_STATS_MAC_RCV_GOOD_MCAST,
    SYS_STATS_MAC_RCV_GOOD_BCAST,
    SYS_STATS_MAC_RCV_GOOD_PAUSE,
    SYS_STATS_MAC_RCV_GOOD_CONTROL,
    SYS_STATS_MAC_RCV_JABBER,
    SYS_STATS_MAC_RCV_COLLISION,
    SYS_STATS_MAC_RCV_FCS_ERROR,
    SYS_STATS_MAC_RCV_ALIGNMENT_ERROR,
    SYS_STATS_MAC_RCV_MAC_OVERRUN,
    SYS_STATS_MAC_RCV_GOOD_OVERSIZE,
    SYS_STATS_MAC_RCV_GOOD_UNDERSIZE,
    SYS_STATS_MAC_RCV_GOOD_OAM,
    SYS_STATS_MAC_RCV_GOOD_63B,
    SYS_STATS_MAC_RCV_BAD_63B,
    SYS_STATS_MAC_RCV_GOOD_1519B,
    SYS_STATS_MAC_RCV_BAD_1519B,
    SYS_STATS_MAC_RCV_GOOD_JUMBO,
    SYS_STATS_MAC_RCV_BAD_JUMBO,
    SYS_STATS_MAC_RCV_64B,
    SYS_STATS_MAC_RCV_127B,
    SYS_STATS_MAC_RCV_255B,
    SYS_STATS_MAC_RCV_511B,
    SYS_STATS_MAC_RCV_1023B,
    SYS_STATS_MAC_RCV_1518B,

    SYS_STATS_MAC_RCV_MAX
};
typedef enum sys_stats_mac_rec_stats_type_e sys_stats_mac_rec_stats_type_t;

enum sys_stats_mac_snd_stats_type_e
{
    SYS_STATS_MAC_SEND_UCAST = 0x19,
    SYS_STATS_MAC_SEND_MCAST,
    SYS_STATS_MAC_SEND_BCAST,
    SYS_STATS_MAC_SEND_PAUSE,
    SYS_STATS_MAC_SEND_CONTROL,
    SYS_STATS_MAC_SEND_OAM,
    SYS_STATS_MAC_SEND_63B,
    SYS_STATS_MAC_SEND_64B,
    SYS_STATS_MAC_SEND_127B,
    SYS_STATS_MAC_SEND_255B,
    SYS_STATS_MAC_SEND_511B,
    SYS_STATS_MAC_SEND_1023B,
    SYS_STATS_MAC_SEND_1518B,
    SYS_STATS_MAC_SEND_1519B,
    SYS_STATS_MAC_SEND_JUMBO,
    SYS_STATS_MAC_SEND_MAC_UNDERRUN,
    SYS_STATS_MAC_SEND_FCS_ERROR,

    SYS_STATS_MAC_SEND_MAX
};
typedef enum sys_stats_mac_snd_stats_type_e sys_stats_mac_snd_stats_type_t;

/* port statistics type */
enum sys_stats_port_based_mac_stats_type_e
{
    SYS_STATS_PORT_UCAST,
    SYS_STATS_PORT_ROUTED_MAC,
    SYS_STATS_PORT_MCAST,
    SYS_STATS_PORT_BCAST,

    SYS_STATS_PORT_BASED_MAC_MAX
};
typedef enum sys_stats_port_based_mac_stats_type_e sys_stats_port_based_mac_stats_type_t;

enum sys_stats_port_based_protocol_stats_type_e
{
    SYS_STATS_PORT_IPV4,
    SYS_STATS_PORT_IPV6,
    SYS_STATS_PORT_MPLS,
    SYS_STATS_PORT_PROTOCOL_DEFAULT,

    SYS_STATS_PORT_BASED_PROTOCOL_MAX
};
typedef enum sys_stats_port_based_protocol_stats_type_e sys_stats_port_based_protocol_stats_type_t;

/*ingress phb type*/
enum sys_stats_igs_phb_stats_type_e
{
    SYS_STATS_INGRESS_PHB_IN,
    SYS_STATS_INGRESS_PHB_OUT,
    SYS_STATS_INGRESS_PHB_MARKDOWN,
    SYS_STATS_INGRESS_PHB_MARKDROP,

    SYS_STATS_INGRESS_PHB_MAX
};
typedef enum sys_stats_igs_phb_stats_type_e sys_stats_igs_phb_stats_type_t;

/*egress phb type*/
enum sys_stats_egs_phb_stats_type_e
{
    SYS_STATS_EGRESS_PHB_OUT,
    SYS_STATS_EGRESS_PHB_MARKDOWN,

    SYS_STATS_EGRESS_PHB_MAX
};
typedef enum sys_stats_egs_phb_stats_type_e sys_stats_egs_phb_stats_type_t;


/* ingress global forwarding stats type */
enum sys_stats_igs_global_fwd_stats_type_e
{
    SYS_STATS_IPV4_UCAST_ROUTED,
    SYS_STATS_IPV4_MCAST_ROUTED,
    SYS_STATS_IPV6_UCAST_ROUTED,
    SYS_STATS_IPV6_MCAST_ROUTED,
    SYS_STATS_IPV4_UCAST_ROUTE_ESCAPE,
    SYS_STATS_IPV4_MCAST_ROUTE_ESCAPE,
    SYS_STATS_IPV6_UCAST_ROUTE_ESCAPE,
    SYS_STATS_IPV6_MCAST_ROUTE_ESCAPE,
    SYS_STATS_MPLS_UCAST_SWITCHED,
    SYS_STATS_MPLS_MCAST_SWITCHED,
    SYS_STATS_MPLS_UCAST_SWITCH_FATAL,
    SYS_STATS_MPLS_MCAST_SWITCH_FATAL,
    SYS_STATS_BRIDGE_BCAST,
    SYS_STATS_BRIDGE_MCAST,
    SYS_STATS_BRIDGE_UCAST,
    SYS_STATS_BRIDGE_ESCAPE,

    SYS_STATS_INGRESS_GLOBAL_FWD_MAX
};
typedef enum sys_stats_igs_global_fwd_stats_type_e sys_stats_igs_global_fwd_stats_type_t;

/* egress global forwarding stats type */
enum sys_stats_egs_global_fwd_stats_type_e
{
    SYS_STATS_PAYLOAD_IPV4_UCAST_ROUTED,
    SYS_STATS_PAYLOAD_IPV4_MCAST_ROUTED,
    SYS_STATS_PAYLOAD_IPV6_UCAST_ROUTED,
    SYS_STATS_PAYLOAD_IPV6_MCAST_ROUTED,
    SYS_STATS_PAYLOAD_BRIDGE_UCAST,
    SYS_STATS_PAYLOAD_BRIDGE_MCAST,
    SYS_STATS_PAYLOAD_BRIDGE_BCAST,
    SYS_STATS_PAYLOAD_NO_OP,
    SYS_STATS_L3EDIT_NO_OP,
    SYS_STATS_L3EDIT_MPLS,
    SYS_STATS_L3EDIT_NAT,
    SYS_STATS_L3EDIT_TUNNEL,
    SYS_STATS_L3EDIT_FLEX,
    SYS_STATS_L2EDIT_NO_OP,
    SYS_STATS_L2EDIT_ETH,
    SYS_STATS_L2EDIT_FLEX,

    SYS_STATS_EGRESS_GLOBAL_FWD_MAX
};
typedef enum sys_stats_egs_global_fwd_stats_type_e sys_stats_egs_global_fwd_stats_type_t;

/*ingress phb storage structor*/
struct sys_stats_igs_phb_s
{
    uint32 phb_in_pkts;
    uint64 phb_in_bytes;
    uint32 phb_out_pkts;
    uint64 phb_out_bytes;
    uint32 phb_markdown_pkts;
    uint64 phb_markdown_bytes;
    uint32 phb_markdrop_pkts;
    uint64 phb_markdrop_bytes;
};
typedef struct sys_stats_igs_phb_s sys_stats_igs_phb_t;

/*egress phb storage structor*/
struct sys_stats_egs_phb_s
{
    uint32 phb_out_pkts;
    uint64 phb_out_bytes;
    uint32 phb_markdown_pkts;
    uint64 phb_markdown_bytes;
};
typedef struct sys_stats_egs_phb_s sys_stats_egs_phb_t;

/*policing storage structor*/
struct sys_stats_policing_s
{
    uint64 policing_confirm_pkts;
    uint64 policing_confirm_bytes;
    uint64 policing_exceed_pkts;
    uint64 policing_exceed_bytes;
    uint64 policing_violate_pkts;
    uint64 policing_violate_bytes;
};
typedef struct sys_stats_policing_s sys_stats_policing_t;

/*queue storage structor*/
struct sys_stats_queue_s
{
    uint64 queue_drop_pkts;
    uint64 queue_drop_bytes;
    uint64 queue_deq_pkts;
    uint64 queue_deq_bytes;
};
typedef struct sys_stats_queue_s sys_stats_queue_t;

struct sys_stats_fwd_stats_s
{
    uint16 stats_ptr;
    uint64 packet_count;
    uint64 byte_count;
};
typedef struct sys_stats_fwd_stats_s sys_stats_fwd_stats_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_stats_init(void);

/*Mac Based Stats*/
extern int32
sys_humber_stats_set_mac_packet_length_mtu1(uint16 gport, uint16 length);
extern int32
sys_humber_stats_get_mac_packet_length_mtu1(uint16 gport, uint16* p_length);

extern int32
sys_humber_stats_set_mac_packet_length_mtu2(uint16 gport, uint16 length);
extern int32
sys_humber_stats_get_mac_packet_length_mtu2(uint16 gport, uint16* p_length);

extern int32
sys_humber_stats_set_mac_dot1q_subtract_bytes(uint16 gport, uint8 byte);
extern int32
sys_humber_stats_get_mac_dot1q_subtract_bytes(uint16 gport, uint8* p_byte);

extern int32
sys_humber_stats_get_mac_rx_stats(uint16 gport, ctc_stats_mac_rec_t* p_stats);
extern int32
sys_humber_stats_reset_mac_rx_stats(uint16 gport);

extern int32
sys_humber_stats_get_mac_tx_stats(uint16 gport, ctc_stats_mac_snd_t* p_stats);
extern int32
sys_humber_stats_reset_mac_tx_stats(uint16 gport);

/*Port Based Stats*/
extern int32
sys_humber_stats_set_igs_port_stats_option(ctc_stats_port_stats_option_type_t type);
extern int32
sys_humber_stats_get_igs_port_stats_option(ctc_stats_port_stats_option_type_t* p_type);
extern int32
sys_humber_stats_set_egs_port_stats_option(ctc_stats_port_stats_option_type_t type);
extern int32
sys_humber_stats_get_egs_port_stats_option(ctc_stats_port_stats_option_type_t* p_type);

extern int32
sys_humber_stats_get_igs_port_stats(uint16 gport, ctc_stats_port_t* p_stats);
extern int32
sys_humber_stats_reset_igs_port_stats(uint16 gport);
extern int32
sys_humber_stats_get_egs_port_stats(uint16 gport,       ctc_stats_port_t* p_stats);
extern int32
sys_humber_stats_reset_egs_port_stats(uint16 gport);

/*PHB Stats*/
extern int32
sys_humber_stats_set_phb_cop_num(uint8 lchip, ctc_direction_t direction, uint8 cop_num);
extern int32
sys_humber_stats_get_phb_cop_num(uint8 lchip, ctc_direction_t direction, uint8* p_cop_num);
extern int32
sys_humber_stats_get_igs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop, sys_stats_igs_phb_t* p_stats);
extern int32
sys_humber_stats_reset_igs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop);
extern int32
sys_humber_stats_get_egs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop, sys_stats_egs_phb_t* p_stats);
extern int32
sys_humber_stats_reset_egs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop);

/*Port log*/
extern int32
sys_humber_stats_set_port_log_discard_stats_enable(bool enable);
extern int32
sys_humber_stats_get_port_log_discard_stats_enable(bool* p_enable);
extern int32
sys_humber_stats_get_igs_port_log_stats(uint16 gport, ctc_stats_basic_t* p_stats);
extern int32
sys_humber_stats_reset_igs_port_log_stats(uint16 gport);
extern int32
sys_humber_stats_get_egs_port_log_stats(uint16 gport, ctc_stats_basic_t* p_stats);
extern int32
sys_humber_stats_reset_egs_port_log_stats(uint16 gport);


/*Global FWD Stats*/
extern int32
sys_humber_stats_get_igs_global_fwd_stats(ctc_stats_igs_global_fwd_t* p_stats);
extern int32
sys_humber_stats_reset_igs_global_fwd_stats(void);

extern int32
sys_humber_stats_get_egs_global_fwd_stats(ctc_stats_egs_global_fwd_t* p_stats);
extern int32
sys_humber_stats_reset_egs_global_fwd_stats(void);

/*Forwarding Stats*/
extern int32
sys_humber_stats_get_fwd_stats_enable(ctc_stats_fwd_type_t fwd_stats_type, bool* p_enable);

extern int32
sys_humber_stats_create_statsptr(uint8 lchip, uint8 stats_szie, uint16* p_stats_ptr);
extern int32
sys_humber_stats_delete_statsptr(uint8 lchip, uint8 stats_szie, uint16 stats_ptr);

extern int32
sys_humber_stats_get_flow_stats(uint8 lchip, uint16 stats_ptr, ctc_stats_basic_t* p_stats);
extern int32
sys_humber_stats_reset_flow_stats(uint8 lchip, uint16 stats_ptr);

extern int32
sys_humber_stats_set_policing_en(uint8 lchip, bool enable);
extern int32
sys_humber_stats_get_policing_en(uint8 lchip, bool* p_enable);
extern int32
sys_humber_stats_get_policing_stats(uint8 lchip, uint16 stats_ptr, sys_stats_policing_t* p_stats);
extern int32
sys_humber_stats_reset_policing_stats(uint8 lchip, uint16 stats_ptr);

extern int32
sys_humber_stats_set_queue_en(uint8 lchip, bool enable);
extern int32
sys_humber_stats_get_queue_en(uint8 lchip, bool* p_enable);
extern int32
sys_humber_stats_get_queue_stats(uint8 lchip, uint16 stats_ptr, sys_stats_queue_t* p_stats);
extern int32
sys_humber_stats_reset_queue_stats(uint8 lchip, uint16 stats_ptr);

extern int32
sys_humber_stats_set_saturate_en(ctc_stats_type_t stats_type, bool enable);
extern int32
sys_humber_stats_get_saturate_en(ctc_stats_type_t stats_type, bool* p_enable);

extern int32
sys_humber_stats_set_hold_en(ctc_stats_type_t stats_type, bool enable);
extern int32
sys_humber_stats_get_hold_en(ctc_stats_type_t stats_type, bool* p_enable);

extern int32
sys_humber_stats_set_clear_after_read_en(ctc_stats_type_t stats_type, bool enable);
extern int32
sys_humber_stats_get_clear_after_read_en(ctc_stats_type_t stats_type, bool* p_enable);

extern int32
sys_humber_stats_set_pkt_cnt_threshold(uint16 threshold);

extern int32
sys_humber_stats_get_pkt_cnt_threshold(uint16* p_threshold);

extern int32
sys_humber_stats_set_byte_cnt_threshold(uint16 threshold);

extern int32
sys_humber_stats_get_byte_cnt_threshold(uint16* p_threshold);

extern int32
sys_humber_stats_set_fifo_depth_threshold(uint8 threshold);

extern int32
sys_humber_stats_get_fifo_depth_threshold(uint8* p_threshold);

extern int32
sys_humber_stats_intr_callback_func(uint8* gchip);

#endif
