/**
 @file ctc_packet_driver.c

 @date 2012-3-15

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "kal.h"
#include "ctc_error.h"
#include "ctc_macro.h"
#include "ctc_const.h"
#include "ctc_aclqos.h"
#include "ctc_packet_driver.h"

#include "drv_humber.h"
#include "sys_humber_ftm.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define BRIDGE_NEXTHOP_PTR              0x3FFFC
#define BYPASS_NEXTHOP_PTR              0x3FFFD
#define MIRROR_NEXTHOP_PTR              0x3FFFE
#define UNTAG_NEXTHOP_PTR               0x3FFFF
#define CPU_PKT_CPU_HDR_LEN 16  /* cpu header length */
#define CPU_PKT_BRG_HDR_LEN 32  /* cpu bridge header */

#define CTC_QOS_PRIORITY_MAX 63
#define CTC_COS_MAX_VALUE    7

struct ctc_cpu_mac_header_s
{
    mac_addr_t cpu_mac_da;        /* destination eth addr */
    mac_addr_t cpu_mac_sa;        /* source ether addr */
    uint16 cpu_hdr_type;          /* packet type ID field */
    uint16 cpu_hdr_reserved;
};
typedef struct ctc_cpu_mac_header_s ctc_cpu_mac_header_t;

enum ctc_swap_direction
{
    HOST_TO_NETWORK,
    NETWORK_TO_HOST
};

/*CRC-8, poly= x^8 + x^2 + x^1 + x^0 ,init=0*/
static uint8 ctc_pkt_drv_crc8_table[]=
{
    0x00,   0x07,   0x0E,   0x09,   0x1C,   0x1B,   0x12,   0x15,
    0x38,   0x3F,   0x36,   0x31,   0x24,   0x23,   0x2A,   0x2D,
    0x70,   0x77,   0x7E,   0x79,   0x6C,   0x6B,   0x62,   0x65,
    0x48,   0x4F,   0x46,   0x41,   0x54,   0x53,   0x5A,   0x5D,
    0xE0,   0xE7,   0xEE,   0xE9,   0xFC,   0xFB,   0xF2,   0xF5,
    0xD8,   0xDF,   0xD6,   0xD1,   0xC4,   0xC3,   0xCA,   0xCD,
    0x90,   0x97,   0x9E,   0x99,   0x8C,   0x8B,   0x82,   0x85,
    0xA8,   0xAF,   0xA6,   0xA1,   0xB4,   0xB3,   0xBA,   0xBD,
    0xC7,   0xC0,   0xC9,   0xCE,   0xDB,   0xDC,   0xD5,   0xD2,
    0xFF,   0xF8,   0xF1,   0xF6,   0xE3,   0xE4,   0xED,   0xEA,
    0xB7,   0xB0,   0xB9,   0xBE,   0xAB,   0xAC,   0xA5,   0xA2,
    0x8F,   0x88,   0x81,   0x86,   0x93,   0x94,   0x9D,   0x9A,
    0x27,   0x20,   0x29,   0x2E,   0x3B,   0x3C,   0x35,   0x32,
    0x1F,   0x18,   0x11,   0x16,   0x03,   0x04,   0x0D,   0x0A,
    0x57,   0x50,   0x59,   0x5E,   0x4B,   0x4C,   0x45,   0x42,
    0x6F,   0x68,   0x61,   0x66,   0x73,   0x74,   0x7D,   0x7A,
    0x89,   0x8E,   0x87,   0x80,   0x95,   0x92,   0x9B,   0x9C,
    0xB1,   0xB6,   0xBF,   0xB8,   0xAD,   0xAA,   0xA3,   0xA4,
    0xF9,   0xFE,   0xF7,   0xF0,   0xE5,   0xE2,   0xEB,   0xEC,
    0xC1,   0xC6,   0xCF,   0xC8,   0xDD,   0xDA,   0xD3,   0xD4,
    0x69,   0x6E,   0x67,   0x60,   0x75,   0x72,   0x7B,   0x7C,
    0x51,   0x56,   0x5F,   0x58,   0x4D,   0x4A,   0x43,   0x44,
    0x19,   0x1E,   0x17,   0x10,   0x05,   0x02,   0x0B,   0x0C,
    0x21,   0x26,   0x2F,   0x28,   0x3D,   0x3A,   0x33,   0x34,
    0x4E,   0x49,   0x40,   0x47,   0x52,   0x55,   0x5C,   0x5B,
    0x76,   0x71,   0x78,   0x7F,   0x6A,   0x6D,   0x64,   0x63,
    0x3E,   0x39,   0x30,   0x37,   0x22,   0x25,   0x2C,   0x2B,
    0x06,   0x01,   0x08,   0x0F,   0x1A,   0x1D,   0x14,   0x13,
    0xAE,   0xA9,   0xA0,   0xA7,   0xB2,   0xB5,   0xBC,   0xBB,
    0x96,   0x91,   0x98,   0x9F,   0x8A,   0x8D,   0x84,   0x83,
    0xDE,   0xD9,   0xD0,   0xD7,   0xC2,   0xC5,   0xCC,   0xCB,
    0xE6,   0xE1,   0xE8,   0xEF,   0xFA,   0xFD,   0xF4,   0xF3
};

/****************************************************************************
*
* Function
*
*****************************************************************************/
int32
ctc_pkt_drv_swap32(uint32 *data, int32 len, uint8 direction)
{
    int32 cnt;

    for (cnt = 0; cnt < len; cnt ++)
    {
        if (HOST_TO_NETWORK == direction)
        {
            data[cnt] = kal_htonl(data[cnt]);
        }
        else
        {
            data[cnt] = kal_ntohl(data[cnt]);
        }
    }

    return DRV_E_NONE;
}

/****************************************************************************
 * Name:      _ctclib_crc8
 * Purpose:    calculate x8 + x2 + x + 1 CRC8.
 * Parameters:
 *  Input:     data -- pointer of buffer will calc CRC8]
 *            len  -- data buffer length.
 *            init_crc -- initialise CRC8.
 * Output:     none.
 * Return:     crc -- new crc.
 * Note:       none.
****************************************************************************/
static INLINE uint8
ctc_pkt_drv_crc8(uint8 *data, int32 len, uint8 init_crc)
{
    int32 cnt;
    uint8 crc = init_crc;

    for (cnt = 0; cnt < len; cnt++)
    {
        crc = ctc_pkt_drv_crc8_table[(crc ^ data[cnt])];
    }

    return crc;
}

/*******************************************************************************
 * Name         : ctc_pkt_drv_rx_process
 * Purpose      : decapsulate packets received from cpu port
 * Input        : pkt: the packet which have cpu header, bridge header and data
                  len: packet length
 * Output       : decap_info: the info decap from bridge header
 * Return       : CTC_E_NONE
 * Note         : 
*******************************************************************************/
int32 
ctc_pkt_drv_decap_packet(void* pkt, uint16 len, ctc_pkt_drv_hdr_rx_info_t* rx_info)
{
    void* pkt_data = NULL;
    uint16 data_len = 0;
    humber_packet_header_t* p_bridge_header = NULL;

    CTC_PTR_VALID_CHECK(pkt);
    CTC_PTR_VALID_CHECK(rx_info);
    CTC_PTR_VALID_CHECK(rx_info->pkt);
    if (len <= (CPU_PKT_CPU_HDR_LEN + CPU_PKT_BRG_HDR_LEN))
    {
        return CTC_E_INVALID_PARAM;
    }
    if (rx_info->pkt_len < (len - CPU_PKT_CPU_HDR_LEN - CPU_PKT_BRG_HDR_LEN))
    {
        return CTC_E_INVALID_PARAM;        
    }
    
    /* 1. strip cpu header */
    pkt = (void*)((uint32)pkt + CPU_PKT_CPU_HDR_LEN);
    
    /* 2. get bridge header and packet data*/
    p_bridge_header = (humber_packet_header_t*)pkt;
    pkt_data = (void*)((uint32)pkt + CPU_PKT_BRG_HDR_LEN);
    data_len = len - (CPU_PKT_CPU_HDR_LEN + CPU_PKT_BRG_HDR_LEN);
    
    /* 3. decap bridge header */
    rx_info->reason.mod = (p_bridge_header->nxt_hop_ptr >> 15) & 0x7;
    rx_info->reason.exp_index = (p_bridge_header->nxt_hop_ptr >> 9) & 0x3F;
    rx_info->reason.sub.exp_sub_index = (uint8)(p_bridge_header->nxt_hop_ptr & 0xF);
    
    /* 4. encap output info */
    rx_info->pkt_type = p_bridge_header->pkt_type;
    rx_info->src_gport = p_bridge_header->src_port;
    rx_info->vrfid = p_bridge_header->vrfid_timestamp_63_48;
    rx_info->svlan_id = p_bridge_header->src_vid;
    rx_info->cvlan_id = p_bridge_header->src_cvid;
    rx_info->svlanid_valid = p_bridge_header->src_svid_vld;
    rx_info->cvlanid_valid = p_bridge_header->src_cvid_vld;
    rx_info->src_cos = p_bridge_header->src_cos;
    rx_info->priority = p_bridge_header->priority;
    rx_info->color = p_bridge_header->color;
    rx_info->timestamp.seconds = p_bridge_header->src_vlanptr_or_timestamp_79_64
                                 | p_bridge_header->vrfid_timestamp_63_48
                                 | p_bridge_header->l4srcport_or_vplssrcport_oamtype;
    rx_info->timestamp.nanoseconds = p_bridge_header->flowid_servecid_or_oamportid;

    kal_memcpy(rx_info->pkt, pkt_data, data_len);
    
    return CTC_E_NONE;
}

/*******************************************************************************
 * Name         : ctc_pkt_drv_bridge_header_encap
 * Purpose      : encapsulate packets from up protocol layer
 * Input        : encap_info: the packet encapsulation info
                  len: buffer length
 * Output       : buffer: the buffer which used to store packet include 
                      cpu header, bridge header and data
                  len: output packet length
 * Return       : CTC_E_NONE
 * Note         : 
*******************************************************************************/
int32
ctc_pkt_drv_encap_packet(ctc_pkt_drv_hdr_tx_info_t* tx_info, void* pkt, uint16* len)
{
    uint32 hash = 0;
    ctc_cpu_mac_header_t cpu_mac_header;
    humber_packet_header_t bridge_header;

    CTC_PTR_VALID_CHECK(tx_info);
    CTC_PTR_VALID_CHECK(pkt);
    CTC_PTR_VALID_CHECK(len);
    CTC_PTR_VALID_CHECK(tx_info->pkt);
    /* 4 is crc length */
    if (*len < (CPU_PKT_CPU_HDR_LEN + CPU_PKT_BRG_HDR_LEN + tx_info->pkt_len + 4))
    {
        return CTC_E_INVALID_PARAM;
    }

    kal_memset(&cpu_mac_header, 0, sizeof(ctc_cpu_mac_header_t));
    kal_memset(&bridge_header, 0, sizeof(humber_packet_header_t));

    /* 1. encapsulate cpu header, 16byte */
    cpu_mac_header.cpu_mac_sa[0] = 0xFF;
    cpu_mac_header.cpu_mac_sa[1] = 0xFF;
    cpu_mac_header.cpu_mac_sa[2] = 0xFF;
    cpu_mac_header.cpu_mac_sa[3] = 0xFF;
    cpu_mac_header.cpu_mac_sa[4] = 0xFF;
    cpu_mac_header.cpu_mac_sa[5] = 0x00;
    cpu_mac_header.cpu_mac_da[0] = 0xFE;
    cpu_mac_header.cpu_mac_da[1] = 0xFD;
    cpu_mac_header.cpu_mac_da[2] = 0x0;
    cpu_mac_header.cpu_mac_da[3] = 0x0;
    cpu_mac_header.cpu_mac_da[4] = 0x0;
    cpu_mac_header.cpu_mac_da[5] = 0x1;
    cpu_mac_header.cpu_hdr_type  = 0x5A5A;
    
    /* 2. encap bridge header by encap info, 32byte*/
    /* 2.1 encap destmap */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_MCAST_FLAG)
    {   /*mcast*/
        uint32 global_met_entry_num = 0;
        uint32 local_met_dsfwd_entry_num = 0;

        sys_alloc_get_met_dsfwd_table_info(&global_met_entry_num, &local_met_dsfwd_entry_num);
        CTC_GLOBAL_CHIPID_CHECK(tx_info->dest_chipid);
        CTC_MAX_VALUE_CHECK(tx_info->destid, global_met_entry_num);
        bridge_header.nxt_hop_ptr = 0;        
        bridge_header.multi_cast = 1;                       /* 0:unicast, 1:multicast */
        bridge_header.dest_chip_id = tx_info->dest_chipid;  /* dest global chipId */
        bridge_header.dest_id = tx_info->destid;            /* dest group id */
    }
    else
    {  /*ucast*/
        CTC_GLOBAL_CHIPID_CHECK(tx_info->dest_chipid);
        CTC_GLOBAL_PORT_CHECK(tx_info->destid);
        bridge_header.multi_cast = 0;                       /* 0:unicast, 1:multicast */
        bridge_header.dest_chip_id = tx_info->dest_chipid;  /* dest global chipId */
        bridge_header.dest_id = tx_info->destid;            /* dest local port */
    }

    /* 2.2 encap nexthop ptr */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_NEXTHOP_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->nh_type, CTC_PKT_DRV_NEXTHOP_TYPE_USER);
        
        if(tx_info->nh_type == CTC_PKT_DRV_NEXTHOP_TYPE_BRIDGE)
        {
            bridge_header.nxt_hop_ptr = BRIDGE_NEXTHOP_PTR;     /* can do vlantag edit */
        }
        else if(tx_info->nh_type == CTC_PKT_DRV_NEXTHOP_TYPE_BYPASS)
        {
            bridge_header.nxt_hop_ptr = CTC_PKT_DRV_NEXTHOP_TYPE_UNTAG;     /* no edit, should input final packet */
        }
        else if(tx_info->nh_type == CTC_PKT_DRV_NEXTHOP_TYPE_UNTAG)
        {
            bridge_header.nxt_hop_ptr = UNTAG_NEXTHOP_PTR;      /* can remove vlantags */
        }
        else if(tx_info->nh_type == CTC_PKT_DRV_NEXTHOP_TYPE_USER)
        {
            bridge_header.nxt_hop_ptr = tx_info->nh_offset;  /* can do any edit which support */
            if(tx_info->flag & CTC_PKT_DRV_ENCAP_NEXTHOP8W_FLAG)
            {
                bridge_header.next_hop_ext = 1;
            }
        }
    }
    else
    {
        bridge_header.nxt_hop_ptr = BYPASS_NEXTHOP_PTR;
    }

    /* 2.3 encap packet type */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_PKT_TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->pkt_type, CTC_PKT_DRV_FLEX_PKT2);
        bridge_header.pkt_type = tx_info->pkt_type;
    }
    else
    {
        bridge_header.pkt_type = CTC_PKT_DRV_ETH_PKT;
    }
    
    /* 2.4 encap operation type */
    if(tx_info->flag & CTC_PKT_DRV_ENCAP_OPT_TYPE_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->operation_type, CTC_PKT_DRV_OPERATION_L3MCAST);
        bridge_header.operation_type = tx_info->operation_type;
    }
    else
    {
        bridge_header.operation_type = CTC_PKT_DRV_OPERATION_NORMAL;
    }

    /* 2.5 encap critical packet type */
    if(tx_info->flag & CTC_PKT_DRV_ENCAP_CRITICAL_FLAG)
    {
        bridge_header.critical_pkt = 1;
    }
    
    /* 2.6 encap svlan id */
    if(tx_info->flag & CTC_PKT_DRV_ENCAP_SRC_SVID_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(tx_info->svlan_id);
        bridge_header.src_vid = tx_info->svlan_id;        /* source svlanId*/
        bridge_header.src_svid_vld = 1;
        bridge_header.src_vlanptr_or_timestamp_79_64 = tx_info->svlan_id;  /* vlan ptr */
    }

    /* 2.7 encap cvlan id */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_SRC_CVID_FLAG)
    {
        CTC_VLAN_RANGE_CHECK(tx_info->cvlan_id);
        bridge_header.src_cvid = tx_info->cvlan_id;        /* source cvlanId*/
        bridge_header.src_cvid_vld = 1;
    }

    /* 2.8 encap src cos */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_SRC_COS_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->src_cos, CTC_COS_MAX_VALUE);
        bridge_header.src_cos = tx_info->src_cos;
    }

    /* 2.9 encap ttl */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_TTL_FLAG)
    {
        bridge_header.ttl_or_oam_defect = tx_info->ttl;
    }
    else
    {
        bridge_header.ttl_or_oam_defect = 63;
    }

    /* 2.10 encap header hash for linkagg */
    if(tx_info->flag & CTC_PKT_DRV_ENCAP_HASH_FLAG)
    {
        hash = tx_info->hash;
        bridge_header.hd_hash_2_to_0 = hash & 0x7;
        bridge_header.hd_hash_7_to_3 = (hash >> 3) & 0x1F;
    }

    /* 2.11 encap timestamp */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_TIMESTAMP_FLAG)
    {
        if (tx_info->ts_offset > tx_info->pkt_len || tx_info->ts_version > 2)
        {
            return CTC_E_INVALID_PARAM;
        }
        bridge_header.ttl_or_oam_defect = tx_info->ts_offset;
        bridge_header.pbb_srcport_type_or_l4srcport_vld = 0x2;
        bridge_header.pbb_srcport_type_or_l4srcport_vld |= 1;
    }
    
    /* 2.12 encap priority */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_PRIORITY_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->priority, CTC_QOS_PRIORITY_MAX);
        bridge_header.priority = tx_info->priority;
    }
    else
    {
        bridge_header.priority = 63; /* highest priority */
    }

    /* 2.13 encap color */
    if (tx_info->flag & CTC_PKT_DRV_ENCAP_COLOR_FLAG)
    {
        CTC_MAX_VALUE_CHECK(tx_info->color, CTC_QOS_COLOR_GREEN);
        bridge_header.color = tx_info->color;
    }
    else
    {
        bridge_header.color = CTC_QOS_COLOR_GREEN; /* highest priority */
    }

    
    bridge_header.src_port = tx_info->src_gport;                /* global source port */
    bridge_header.hdr_type = 1;                                 /* for humber, it should be 1 */
    bridge_header.srcport_isolate_id = 0x3F;                    /* disable port isolate */
    bridge_header.flowid_servecid_or_oamportid |= 0xFF << 16;   /* disable flowid */
    bridge_header.pkt_len = tx_info->pkt_len + 4;               /* add CRC 4 bytes */
    ctc_pkt_drv_swap32((uint32*)&bridge_header, sizeof(humber_packet_header_t)/4, HOST_TO_NETWORK);
    /* calc bridge header CRC */
    bridge_header.hdr_crc = ctc_pkt_drv_crc8((uint8 *)&bridge_header, CPU_PKT_BRG_HDR_LEN, 0);

    /* 3. encap output info */
    kal_memcpy(pkt, &cpu_mac_header, sizeof(ctc_cpu_mac_header_t));
    kal_memcpy(pkt+CPU_PKT_CPU_HDR_LEN, &bridge_header, sizeof(humber_packet_header_t));
    kal_memcpy(pkt+CPU_PKT_CPU_HDR_LEN+CPU_PKT_BRG_HDR_LEN, tx_info->pkt, tx_info->pkt_len);

    return CTC_E_NONE;

}

