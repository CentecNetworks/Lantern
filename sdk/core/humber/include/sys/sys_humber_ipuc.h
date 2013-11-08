/**
 @file sys_humber_ipuc.h

 @date 2009-12-05

 @version v2.0

*/
 #ifndef _SYS_HUMBER_IPUC_H
 #define _SYS_HUMBER_IPUC_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

#include "ctc_ipuc.h"
#include "ctc_l3if.h"
#include "ctc_const.h"
#include "sys_humber_nexthop_api.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define SYS_IPUC_INIT_CHECK \
    { \
        if(p_ipuc_master == NULL) \
            return CTC_E_NOT_INIT; \
    }

#define SYS_IPUC_DBG_DUMP(FMT, ...)                            \
    {                                                        \
        CTC_DEBUG_OUT_INFO(ipuc, ipuc, IPUC_SYS, FMT, ##__VA_ARGS__); \
    }

#define SYS_IPUC_MASK_LEN_CHECK(ver, len)         \
    {                                                 \
        if((len) > p_ipuc_master->max_mask_len[ver])         \
            return CTC_E_INVALID_PARAM;               \
    }

#define SYS_IPUC_KEY_MAP(p_ctc_ipuc, p_sys_ipuc)  \
    {                                                 \
        (p_sys_ipuc)->vrf_id = (p_ctc_ipuc)->vrf_id;  \
        (p_sys_ipuc)->masklen = (p_ctc_ipuc)->masklen;\
        (p_sys_ipuc)->ip_ver = (p_ctc_ipuc)->ip_ver;  \
        kal_memcpy(&(p_sys_ipuc)->ip, &(p_ctc_ipuc)->ip, p_ipuc_master->addr_len[(p_ctc_ipuc)->ip_ver]);\
        kal_memcpy(&(p_sys_ipuc)->ip_sa, &(p_ctc_ipuc)->ip_sa, p_ipuc_master->addr_len[(p_ctc_ipuc)->ip_ver]);\
        (p_sys_ipuc)->tunnel_info.tunnel_packet_type = (p_ctc_ipuc)->tunnel_info.tunnel_packet_type ;\
        (p_sys_ipuc)->tunnel_info.is_set_gre_key = (p_ctc_ipuc)->tunnel_info.is_set_gre_key ;\
        (p_sys_ipuc)->tunnel_info.gre_key = (p_ctc_ipuc)->tunnel_info.gre_key ;\
    }

#define SYS_IPUC_DATA_MAP(p_ctc_ipuc, p_sys_ipuc) \
    {                                                 \
        (p_sys_ipuc)->nh_id = (p_ctc_ipuc)->nh_id;    \
        (p_sys_ipuc)->route_flag = (p_ctc_ipuc)->route_flag; \
        (p_sys_ipuc)->l3_inf = (p_ctc_ipuc)->l3_inf; \
        (p_sys_ipuc)->is_tunnel = (p_ctc_ipuc)->is_tunnel; \
        (p_sys_ipuc)->is_ipsa = (p_ctc_ipuc)->is_ipsa; \
    }

#define SYS_IPUC_HASH_KEY_MAP(HASH_KEY, DATA)                   \
    {                                                           \
        if((DATA)->ip_ver == CTC_IP_VER_4)                      \
        {                                                       \
            HASH_KEY[5] = ((DATA)->vrf_id>> 8) & 0xFF;          \
            HASH_KEY[4] = (DATA)->vrf_id & 0xFF;                \
            HASH_KEY[3] = ((DATA)->ip.ipv4.ip32 >> 24) & 0xFF;  \
            HASH_KEY[2] = ((DATA)->ip.ipv4.ip32 >> 16) & 0xFF;  \
            HASH_KEY[1] = ((DATA)->ip.ipv4.ip32 >> 8) & 0xFF;   \
            HASH_KEY[0] = (DATA)->ip.ipv4.ip32 & 0xFF;          \
        }                                                       \
        else                                                    \
        {                                                       \
            HASH_KEY[17] = ((DATA)->vrf_id>> 8) & 0xFF;         \
            HASH_KEY[16] = (DATA)->vrf_id & 0xFF;               \
            HASH_KEY[15] = (DATA->ip.ipv6.ip32[3]>>24) & 0xff;  \
            HASH_KEY[14] = (DATA->ip.ipv6.ip32[3]>>16) & 0xff;  \
            HASH_KEY[13] = (DATA->ip.ipv6.ip32[3]>>8) & 0xff;   \
            HASH_KEY[12] = DATA->ip.ipv6.ip32[3] & 0xff;        \
            HASH_KEY[11] = (DATA->ip.ipv6.ip32[2]>>24) & 0xff;  \
            HASH_KEY[10] = (DATA->ip.ipv6.ip32[2]>>16) & 0xff;  \
            HASH_KEY[9] =  (DATA->ip.ipv6.ip32[2]>>8) & 0xff;   \
            HASH_KEY[8] =  DATA->ip.ipv6.ip32[2] & 0xff;        \
            HASH_KEY[7] =  (DATA->ip.ipv6.ip32[1]>>24) & 0xff;  \
            HASH_KEY[6] =  (DATA->ip.ipv6.ip32[1]>>16) & 0xff;  \
            HASH_KEY[5] =  (DATA->ip.ipv6.ip32[1]>>8) & 0xff;   \
            HASH_KEY[4] =  DATA->ip.ipv6.ip32[1] & 0xff;        \
            HASH_KEY[3] =  (DATA->ip.ipv6.ip32[0]>>24) & 0xff;  \
            HASH_KEY[2] =  (DATA->ip.ipv6.ip32[0]>>16) & 0xff;  \
            HASH_KEY[1] =  (DATA->ip.ipv6.ip32[0]>>8) & 0xff;   \
            HASH_KEY[0] =  DATA->ip.ipv6.ip32[0] & 0xff;        \
        }                                                       \
    }

#define IPV4_MASK(ip, len)  \
    {                           \
        uint32 mask = (len) ? ~((1 << (CTC_IPV4_ADDR_LEN_IN_BIT - (len))) - 1) : 0;\
        (ip) &= mask;             \
    }

#define IPV6_MASK(ip, len)  \
    {                           \
        uint8 feedlen = CTC_IPV6_ADDR_LEN_IN_BIT - (len);\
        uint8 sublen = feedlen % 32;\
        int16 index = feedlen / 32;\
        if(sublen)              \
        {                       \
            uint32 mask = ~((1 << sublen) - 1);\
            (ip)[(uint16)index] &= mask;  \
        }                       \
        index--;                \
        for(; index >= 0; index--)\
        {                       \
            (ip)[(uint16)index] = 0;      \
        }                       \
    }

#define SYS_IP_ADDR_MASK(ip, len, ver) \
    {                           \
        if(CTC_IP_VER_4 == (ver)) \
        { \
            IPV4_MASK((ip).ipv4, (len)); \
        } \
        else                    \
        { \
            IPV6_MASK((ip).ipv6, (len)); \
        } \
    }

#define SYS_IP_ADDRESS_SORT(val)           \
    {                                          \
        if(CTC_IP_VER_6 == (val->ip_ver))      \
        {                                      \
            uint32 t;                          \
            t = val->ip.ipv6[0];               \
            val->ip.ipv6[0] = val->ip.ipv6[3]; \
            val->ip.ipv6[3] = t;               \
                                               \
            t = val->ip.ipv6[1];               \
            val->ip.ipv6[1] = val->ip.ipv6[2]; \
            val->ip.ipv6[2] = t;               \
        }                                      \
    }

#define SYS_IP_TUNNEL_VALID_CHECK(is_tunnel, ver) \
    {                           \
        if( (is_tunnel) && (!p_ipuc_master->tunnel_en[ver]) ) \
            return CTC_E_TUNNEL_INVALID; \
    }

#define SYS_IP_TUNNEL_TYPE_CHECK(is_tunnel, type) \
    {                           \
        if( (is_tunnel) && (CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE !=type) && \
            (CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV4 !=type) && \
            (CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV6 !=type)) \
            return CTC_E_INVALID_PARAM; \
    }

#define SYS_IP_TUNNEL_GRE_CHECK(type, is_set_grekye) \
    {                           \
        if((is_set_grekye) && (CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE !=type)) \
            return CTC_E_INVALID_PARAM; \
    }

#define SYS_IP_TUNNEL_IPSA_CHECK(type, p_ctc_ipuc,ip) \
    {                           \
        ipv6_addr_t tmp_addr; \
        if(((p_ctc_ipuc)->is_ipsa) && (CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE ==type)) \
            return CTC_E_INVALID_PARAM; \
        if(((p_ctc_ipuc)->is_ipsa) && ((p_ctc_ipuc)->ip_ver == CTC_IP_VER_6 ))\
        { \
            kal_memcpy(&tmp_addr, &((ip).ipv6), CTC_IPV6_ADDR_LEN_IN_BYTE); \
            ((ip).ipv6)[0] = tmp_addr[3]; \
            ((ip).ipv6)[1] = tmp_addr[2]; \
            ((ip).ipv6)[2] = tmp_addr[1]; \
            ((ip).ipv6)[3] = tmp_addr[0]; \
        }\
    }

#define SYS_IP_TUNNEL_ROUTE_FLAG_CHECK(flag,is_tunnel, l3if_en) \
    {                           \
        if( ((!is_tunnel ) && l3if_en) || \
            (((CTC_IPUC_FLAG_RPF_CHECK & (flag)) == 0) && \
            ((CTC_IPUC_FLAG_ICMP_CHECK & (flag)) == 0) && \
            (is_tunnel ) && l3if_en)) \
            return CTC_E_INVALID_L3IF; \
        if( (is_tunnel ) && (CTC_FLAG_ISSET((flag),CTC_IPUC_FLAG_NEIGHBOR))\
            && (CTC_FLAG_ISSET((flag),CTC_IPUC_FLAG_CONNECT)))\
            return CTC_E_INVALID_PARAM; \
    }

#define SYS_IP_CHECK_VERSION_ENABLE(ver, vrfid) \
    {                           \
        if(!p_ipuc_master->version_en[ver]) \
        { \
            return CTC_E_VERSION_DISABLE;\
        } \
        if((vrfid) > p_ipuc_master->max_vrfid[ver]) \
        { \
            return CTC_E_INVALID_VRF; \
        } \
    }

#define SYS_IPUC_NHID_CHECK(nhid, is_ecmp) \
    { \
        if(is_ecmp) \
        { \
            if((nhid) < p_ipuc_master->max_external_nhid) \
            { \
                return CTC_E_INVALID_NHID; \
            } \
        } \
        else \
        { \
            if((nhid) >= p_ipuc_master->max_external_nhid) \
            { \
                return CTC_E_INVALID_NHID; \
            } \
        } \
    }

#define SYS_IP_FLAG_CHECK(p_ctc_ipuc) \
    {                           \
        if(CTC_FLAG_ISSET((p_ctc_ipuc)->route_flag, CTC_IPUC_FLAG_CONNECT) && \
           (p_ctc_ipuc)->is_ecmp_nh) \
        { \
            return CTC_E_INVALID_PARAM; \
        } \
        if(((p_ctc_ipuc)->masklen) < p_ipuc_master->max_mask_len[(p_ctc_ipuc)->ip_ver]) \
        { \
            CTC_UNSET_FLAG((p_ctc_ipuc)->route_flag, CTC_IPUC_FLAG_NEIGHBOR); \
        } \
        if((!CTC_FLAG_ISSET((p_ctc_ipuc)->route_flag, CTC_IPUC_FLAG_CONNECT)) && (!((p_ctc_ipuc)->is_tunnel_l3if)))\
        { \
            (p_ctc_ipuc)->l3_inf = CTC_L3IF_INVALID_L3IF_ID; \
        } \
        (p_ctc_ipuc)->route_flag &= MAX_CTC_IPUC_FLAG; \
    }


#define SYS_IPUC_FLAG_DEFAULT        0x80       /* Packets hitting this route will be copied to CPU */
#define SYS_IPUC_ERROR_BIT_NUM       0xFF       /* if HASH not init bit number will equal to 0xFF*/

/* packet type */
enum sys_ipuc_packet_type_e
{
    ETH_PKT,
    IPV4_PKT,
    MPLS_PKT,
    IPV6_PKT,
    MPLS_MCAST_PKT,
    FLEX_PKT1,
    FLEX_PKT2,
    RESERVED,
};

typedef enum sys_ipuc_packet_type_e  sys_ipuc_packet_type_t;
/* hash type */
enum sys_ipuc_hash_type_e
{
    SYS_IPUC_HASH_LEFT,
    SYS_IPUC_HASH_RIGHT,
    SYS_IPUC_HASH_MAX,
};
typedef enum sys_ipuc_hash_type_e  sys_ipuc_hash_type_t;

union ipv4_addr_union_s
{
    uint32 ip32;
    uint16 ip16[2];
    uint8 ip8[4];
};
typedef union ipv4_addr_union_s ipv4_addr_union_t;

union ipv6_addr_union_s
{
    uint32 ip32[CTC_IPV6_ADDR_LEN];
    uint16 ip16[CTC_IPV6_ADDR_LEN*2];
    uint8  ip8[CTC_IPV6_ADDR_LEN*4];
};
typedef union ipv6_addr_union_s ipv6_addr_union_t;

/**
 @brief  Define tunnel_info structure used in ipuc parameter
*/
struct sys_tunnel_info_s
{
    uint32 gre_key;   /**< GRE key  */
    uint8 tunnel_packet_type;   /**< set pld type */
    uint8 is_set_gre_key;   /**< GRE encap information */
    uint16 rsv;
};
typedef struct sys_tunnel_info_s sys_tunnel_info_t;

struct sys_ipuc_info_s
{
    uint32 key_offset;
    uint32 nh_id;
    uint16 vrf_id;
    uint16 l3_inf;
    uint8 route_flag;
    uint8 masklen;
    uint8 ip_ver;
    uint8 in_sram;
    uint8 ecpn;
    uint8 is_tunnel;
    uint8 is_ipsa;
    uint8 is_tunnel_l3if;
    sys_tunnel_info_t tunnel_info;
    union
    {
        ipv4_addr_union_t ipv4;
        ipv6_addr_union_t ipv6;
    }ip;
    union
    {
        ipv4_addr_union_t ipv4;
        ipv6_addr_union_t ipv6;
    }ip_sa;
};
typedef struct sys_ipuc_info_s sys_ipuc_info_t;

struct sys_ipv4_info_s
{
    uint32 key_offset;
//    sys_nh_offset_array_t fwd_offset;
    uint32 nh_id;
    uint16 vrf_id;
    uint16 l3_inf;

    uint8 route_flag;
    uint8 masklen;
    uint8 ip_ver;
    uint8 in_sram;

    uint8 ecpn;
    uint8 is_tunnel;
    uint8 is_ipsa;
    uint8 is_tunnel_l3if;

    sys_tunnel_info_t tunnel_info;
    ipv4_addr_union_t ipv4;
    uint32 rsv[3];
    ipv4_addr_union_t ipv4_sa;
};
typedef struct sys_ipv4_info_s sys_ipv4_info_t;

struct sys_ipv6_info_s
{
    uint32 key_offset;
//    sys_nh_offset_array_t fwd_offset;
    uint32 nh_id;
    uint16 vrf_id;
    uint16 l3_inf;

    uint8 route_flag;
    uint8 masklen;
    uint8 ip_ver;
    uint8 in_sram;

    uint8 ecpn;
    uint8 is_tunnel;
    uint8 is_ipsa;
    uint8 is_tunnel_l3if;

    sys_tunnel_info_t tunnel_info;
    ipv6_addr_union_t ipv6;
    ipv6_addr_union_t ipv6_sa;
};
typedef struct sys_ipv6_info_s sys_ipv6_info_t;

typedef int32 (* ipuc_write_key_t)(sys_ipuc_info_t* p_ipuc_info, void* p_key);

typedef int16 (* ipuc_cal_hash_index_t)(uint8* sed, uint32 bit_num);
struct sys_ipuc_master_s
{
    kal_mutex_t* mutex;
    ipuc_write_key_t write_hash_key[MAX_CTC_IP_VER];
    ipuc_write_key_t write_key[MAX_CTC_IP_VER];
    ipuc_cal_hash_index_t cal_hash_index[MAX_CTC_IP_VER][SYS_IPUC_HASH_MAX];
    uint32 sa_table_id[MAX_CTC_IP_VER];
    uint32 da_table_id[MAX_CTC_IP_VER];
    uint32 key_table_id[MAX_CTC_IP_VER];
    uint32 hashkey_table_id[MAX_CTC_IP_VER];
    uint32 hashkey_type[MAX_CTC_IP_VER];
    uint32 hash_base[MAX_CTC_IP_VER];
    uint32 tcam_base[MAX_CTC_IP_VER];
    uint32 do_hash_count[MAX_CTC_IP_VER];
    uint32 do_left_hash_count[MAX_CTC_IP_VER];
    uint32 do_right_hash_count[MAX_CTC_IP_VER];
    uint32 hash_array_num;
    uint32 max_external_nhid;
    uint8 *hash_status;

    uint16 max_vrfid[MAX_CTC_IP_VER];

    uint8 max_mask_len[MAX_CTC_IP_VER];
    uint8 addr_len[MAX_CTC_IP_VER];

    uint8 info_size[MAX_CTC_IP_VER];
    uint8 version_en[MAX_CTC_IP_VER];

    uint8 tunnel_en[MAX_CTC_IP_VER];
    uint8 asic_hash_en[MAX_CTC_IP_VER];

    uint8 do_hash_no_conflict_mode[MAX_CTC_IP_VER];
    uint8 hash_bit_num[MAX_CTC_IP_VER];

    uint8 cpu_rpf;
    uint8 is_ipucsa_allocated;
    uint8 rsv0[2];
};
typedef struct sys_ipuc_master_s sys_ipuc_master_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_ipuc_init(void);

extern int32
sys_humber_ipuc_add(ctc_ipuc_param_t* p_ipuc_param);

extern int32
sys_humber_ipuc_remove(ctc_ipuc_param_t* p_ipuc_param);

/* now hash next-hop need not to set > 512 */
extern int32
sys_humber_ipuc_add_default_entry(uint8 ip_ver, uint32 nh_id);

extern int32
sys_humber_ipuc_cpu_rpf_check(bool enable);

extern int32
sys_humber_ipuc_write_key(sys_ipuc_info_t* p_ipuc_info);

extern int32
sys_humber_ipuc_remove_key(sys_ipuc_info_t* p_ipuc_info);

extern int32
sys_humber_ipuc_set_route_ctl(ctc_ipuc_route_ctl_t *p_route_ctl_info);

int32
sys_humber_ipuc_set_lookup_ctl(ctc_ipuc_lookup_ctl_t *p_lookup_ctl_info);


#endif

