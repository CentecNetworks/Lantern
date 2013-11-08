/**
 @file sys_humber_ipuc_db.h

 @date 2009-12-05

 @version v2.0

*/
 #ifndef _SYS_HUMBER_IPUC_DB_H
 #define _SYS_HUMBER_IPUC_DB_H
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"

#include "ctc_port.h"
#include "ctc_const.h"
#include "ctc_hash.h"
#include "sys_humber_sort.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
/* hash table size */
#define IPUC_IPV4_HASH_MASK     0xFFF
#define IPUC_IPV6_HASH_MASK     0xFFF

typedef int32 (* p_sys_ipuc_hash_make_cb_t)(sys_ipuc_info_t* );

struct sys_ipuc_db_master_s
{
    ctc_hash_t* ipuc_hash[MAX_CTC_IP_VER];
    sys_sort_block_t ipv4_blocks[CTC_IPV4_ADDR_LEN_IN_BIT + 1];
    sys_sort_block_t ipv6_blocks[CTC_IPV6_ADDR_LEN_IN_BIT + 1];
    sys_sort_block_t ipv4_tunnel_blocks[CTC_IPV4_ADDR_LEN_IN_BIT + 1];
    sys_sort_block_t ipv6_tunnel_blocks[CTC_IPV6_ADDR_LEN_IN_BIT + 1];
    sys_sort_key_info_t ipuc_sort_key_info[MAX_CTC_IP_VER];
    sys_sort_key_info_t ipuc_tunnel_sort_key_info[MAX_CTC_IP_VER];
    skinfo_2a p_ipuc_offset_array[MAX_CTC_IP_VER];
    skinfo_2a p_ipuc_tunnel_offset_array[MAX_CTC_IP_VER];
};
typedef struct sys_ipuc_db_master_s sys_ipuc_db_master_t;

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/
extern int32
sys_humber_ipuc_db_lookup(sys_ipuc_info_t** pp_ipuc_info);

extern int32
sys_humber_ipuc_db_index_lookup(sys_ipuc_info_t** pp_ipuc_info);

extern int32
sys_humber_ipuc_db_add(sys_ipuc_info_t* p_ipuc_info);

extern int32
sys_humber_ipuc_db_remove(sys_ipuc_info_t* p_ipuc_info);

extern int32
sys_humber_ipuc_db_get_offset(sys_ipuc_info_t* p_ipuc_info);

extern int32
sys_humber_ipuc_db_init(void);

#endif

