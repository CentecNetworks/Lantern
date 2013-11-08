/**
 @file sys_humber_l2_fdb.h

 @date 2009-10-19

 @version v2.0

The file defines Macro, stored data structure for Normal FDB module
*/
#ifndef _SYS_HUMBER_L2_FDB_H_
#define _SYS_HUMBER_L2_FDB_H_

#include "kal.h"
#include "ctc_l2.h"
#include "ctc_hash.h"
#include "ctc_vector.h"
#include "ctc_linklist.h"
#include "ctc_const.h"


/**********************************************************************************
              Define Typedef,define and Data Structure
***********************************************************************************/


struct sys_l2_mackey_s
{
    mac_addr_t mac;
    uint16 fid;
};
typedef struct sys_l2_mackey_s sys_l2_mackey_t;

enum sys_l2_node_flag_e
{
    SYS_L2_NODE_FLAG_IS_ASIC_HASH       = 0x0001,
    SYS_L2_NODE_FLAG_IS_STATIC          = 0x0004,
    SYS_L2_NODE_FLAG_IS_L2MC              = 0x0008,
    SYS_L2_NODE_FLAG_IS_REMOTE_DYN     = 0x0010,
    SYS_L2_NODE_FLAG_IS_SYSTEM_RSV    = 0x0040, /**< unicast static FDB, if set ,indicate the entry is system mac, it can't be deleted by flush api, using for MAC DA */
    MAX_SYS_L2_NODE_FLAG,
};
typedef enum sys_l2_node_flag_e sys_l2_node_flag_t;

/**
 @brief  L2 node
*/
struct sys_l2_node_s
{
    sys_l2_mackey_t key;
    uint32 index;

    uint16 flag;    /**< it's the sys_l2_node_flag_t value */
    uint16 gport;
    /*in ucast FDB entry ,the pointer is point to vlan_fdb_list,else ,
      the pointer is point to NULL*/
    ctc_listnode_t *port_entey;
     /*in ucast FDB entry  ,the pointer is point to vlan_fdb_list,and in Mcast FDB entry
       the pointer is point to mcast_group_list,else  the pointer is point to NULL*/
    ctc_listnode_t  *vlan_entey;
};
typedef struct sys_l2_node_s sys_l2_node_t;

/**
 @brief   independent L2 multicast address.
*/
struct sys_l2_mcast_node_s
{
    uint32 l2mc_nhp_id;   /**< mcast nexthop id */
    ctc_slist_t *local_member_list[CTC_MAX_HUMBER_CHIP_NUM];
};
typedef struct sys_l2_mcast_node_s sys_l2_mcast_node_t;


struct sys_l2_master_s
{
    uint16 max_fid_value;
    uint8  hash_bit_num;               /* fdb hash bit num */
    uint8  offset_pool_num;
    uint32 sw_hash_tbl_size;          /* get size from tcam allocation */
    uint32 hash_base;                 /* fdb hash action table base address */
    uint32 tcam_base;                 /* fdb tcam action table base address */
    uint32 max_sram_size;            /* fdb sram total size */

    uint32 asic_hash_size;            /* total asic hash  size */
    uint32 do_hash_count;
    uint32 do_left_hash_count;
    uint32 do_right_hash_count;
    uint32 do_hash_no_conflict_mode;

    uint32 dynmac_count;
    uint32 local_dynmac_count;

    /*store node*/
    ctc_hash_t* fdb_hash;
    ctc_hash_t* fdb_dft_entry_hash;
    ctc_hash_t* fdb_mac_hash;

/*   ctc_hash_t *l2_fdb_hash; */
    ctc_linklist_t* mcast_grp_list;
    kal_mutex_t* l2_mutex;
    ctc_vector_t* fdb_tbl_vec;
    ctc_vector_t* gport_vec;
//ctc_vector_t* fid_vec;
    ctc_vector_t* lport_dynmac_vec;

    uint32 *p_group_array;  /*get mcast nexthop id by group id*/
    uint32  max_group_num;


    uint8 *hash_status; /*  hash status of bucket*/
    uint8 *pool_status; /*  pool status of offset*/

    uint32 flush_fdb_cnt_per_loop; /* delete all entries if flush_fdb_cnt_per_loop=0,
                                                           or delete  flush_fdb_cnt_per_loop entries one time*/
};
typedef struct sys_l2_master_s sys_l2_master_t;

struct sys_l2_fdb_vlan_node_s
{
     ctc_linklist_t* vlan_fdb_list;
     sys_l2_node_t* fdb_dft_node;
     uint32 dynmac_count;
     uint32 local_dynmac_count;
     uint16 gport;
     uint16 fid;
     uint8 port_valid;
};
typedef struct sys_l2_fdb_vlan_node_s sys_l2_fdb_vlan_node_t;


/**********************************************************************************
                        Defines and Macros
***********************************************************************************/
#define L2_LOCK \
    if (pl2_master->l2_mutex) kal_mutex_lock(pl2_master->l2_mutex)
#define L2_UNLOCK \
    if (pl2_master->l2_mutex) kal_mutex_unlock(pl2_master->l2_mutex)


#define SYS_L2_FDB_FID_CHECK(fid) \
    {\
        if( (fid != 0xFFFF && fid > pl2_master->max_fid_value))\
            return CTC_E_INVALID_PARAM;\
    }

#define SYS_L2_FDB_INIT_CHECK() \
    {\
        if(  pl2_master == NULL)\
            return CTC_E_NOT_INIT;\
    }

 #define SYS_FDB_DBG_INFO(FMT, ...)                          \
                    {                                                      \
                       CTC_DEBUG_OUT_INFO(l2, fdb, L2_FDB_SYS,FMT,##__VA_ARGS__);\
                    }

#define SYS_FDB_DBG_FUNC()                          \
                    {\
                        CTC_DEBUG_OUT_FUNC(l2, fdb, L2_FDB_SYS);\
                    }


/**@}*/ /*end of @defgroup  fdb FDB */


/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
/**
 @addtogroup fdb FDB
 @{
*/

/**
 @brief init the db and alloc the fdb percent
*/
extern int32
sys_humber_l2_fdb_init(void* l2_fdb_global_cfg);


/**
 @brief get nhid by mcast group id
*/
extern int32
sys_humber_l2_get_nhid_by_mcast_group_id(uint32 group_id, uint32 *p_nhid);


/**
 @brief lookup default hash table with FID
*/
extern sys_l2_fdb_vlan_node_t*
_sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(uint16 fid, uint16 gport, uint8 port_valid);

/**
@brief set vlan default entry's operation for unknown multicast or unicast.
*/
int32
sys_humber_l2_set_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr);

/**
@brief get vlan default entry's operation for unknown multicast or unicast.
*/
int32
sys_humber_l2_get_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr);

/**
 @brief  get fdb table range
*/
 int32
 sys_humber_l2_fdb_get_fdb_tbl_range(uint32 *start , uint32 *end);

 /**
 @brief get some type fdb count
*/
extern int32
sys_humber_l2_get_fdb_count(ctc_l2_fdb_query_t* pQuery);
 /**
  @brief Query fdb enery according to specified query condition

 */
extern int32
sys_humber_l2_get_fdb_entry(ctc_l2_fdb_query_t* pQuery,
                                                          ctc_l2_fdb_query_rst_t* query_rst);


/**
 @brief add a fdb entry
*/
extern int32
sys_humber_l2_add_fdb(ctc_l2_addr_t* l2_addr);

/**
 @brief delete a fdb entry
*/
extern int32
sys_humber_l2_remove_fdb(ctc_l2_addr_t* l2_addr );

/**
 @brief delete a fdb entry by index
*/
extern int32
sys_humber_l2_remove_fdb_by_index(uint32 index);
/**
 @brief get a fdb entry by index
*/
extern int32
sys_humber_l2_get_fdb_by_index(uint32 index, ctc_l2_addr_t* l2_addr);

/**
 @brief add a fdb entry with nhp
*/
extern int32
sys_humber_l2_add_fdb_with_nexthop(ctc_l2_addr_t* l2_addr, uint32 nhp_id);

/**
 @brief flush fdb entry
*/
extern int32
sys_humber_l2_fdb_flush(ctc_l2_fdb_flush_t* pFlush);


/**
 @brief add a default entry
*/
extern int32
sys_humber_l2_add_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);

/**
 @brief remove a default entry
*/
extern int32
sys_humber_l2_remove_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);

/**
 @brief add a port into default entry
*/
extern int32
sys_humber_l2_add_port_to_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);

/**
 @brief remove a port from default entry
*/
extern int32
sys_humber_l2_remove_port_from_default_entry(ctc_l2dflt_addr_t* l2dflt_addr);


/**@}*/ /*end of @addtogroup fdb FDB  */

/**
 @addtogroup l2mcast L2MCAST
 @{
*/
/**
 @brief add an entry in the mcast table
*/
extern int32
sys_humber_l2mcast_add_addr(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief Remove an entry in the multicast table
*/
extern int32
sys_humber_l2mcast_remove_addr(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief Add a given port/port list to  a existed multicast group.
*/
extern int32
sys_humber_l2mcast_add_member(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 @brief Remove a given port/port list to  a existed multicast group.
*/
extern int32
sys_humber_l2mcast_remove_member(ctc_l2_mcast_addr_t* l2mc_addr);

/**
 only use internal testing
*/
extern int32 sys_humber_show_l2_fdb_status(void);

/**@}*/ /*end of @addtogroup   */


#endif /*end of _SYS_HUMBER_L2_FDB_H_*/


