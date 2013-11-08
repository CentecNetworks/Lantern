
/**
 @file sys_humber_l2_fdb.c

 @date 2009-10-19

 @version v2.0

The file implement   FDB/L2 unicast /mac filtering /mac security/L2 mcast functions
*/
#include "kal.h"
#include "ctc_error.h"



#include "sys_humber_opf.h"
#include "sys_humber_l2_fdb.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_ftm.h"
#include "sys_humber_chip.h"
#include "sys_humber_port.h"
#include "sys_humber_learning_aging.h"
/* XXX_OSS_TRIM: disabled. */
//#include "sys_humber_aps.h"
#include "sys_humber_mcast.h"
#include "sys_humber_hash.h"

#include "drv_io.h"
#include "drv_humber.h"


/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

#define SYS_L2_FDB_HASH_GOLDEN_RATIO   0x9e3779b9

#define SYS_L2_FDB_SYS_DFT_ENTRY_SIZE     1

#define SYS_L2_FDB_POOL_BLOCK_SIZE   (8*1024)
#define SYS_L2_FDB_LAST_POOL_BLOCK_MIN_SIZE  (16*1024)

#define SYS_L2_FDB_SW_HASH_TBL_SIZE     (64*1024)
#define SYS_L2_FDB_SW_HASH_BLOCK_SIZE     1024
#define SYS_FDB_TBL_VEC_BLOCK_SIZE   2048
#define SYS_FDB_GPORT_VEC_NUM          (256*31 +128)
#define SYS_FDB_GPORT_VEC_BLOCK_SIZE  64
#define SYS_FDB_VLAN_VEC_BLOCK_SIZE  128

/*default vlan hash*/
#define SYS_L2_FDB_DFT_ENTRY_HASH_TBL_SIZE     (48*4094)
#define SYS_L2_FDB_DFT_ENTRY_HASH_BLOCK_SIZE     1024


/*mac hash*/
#define SYS_L2_FDB_MAC_ENTRY_HASH_TBL_SIZE     (64*1024)
#define SYS_L2_FDB_MAC_ENTRY_HASH_BLOCK_SIZE     1024

#define SYS_FDB_DISCARD_FWD_PTR 0xFFFFF

#define SYS_FDB_INVALID_VPLS_PORT  0x1FFF

#define SYS_FDB_FDB_HASH_MIX(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

#define    SYS_L2_FDB_MAX_VALUE_CHECK_WITH_UNLOCK(var,max_value,lock) \
    {\
	    if(var > max_value) CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_INVALID_PARAM, lock);\
    }

#define    SYS_L2_FDB_MIN_VALUE_CHECK_WITH_UNLOCK(var,min_value,lock)	\
    {\
        if(var < min_value) CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_INVALID_PARAM, lock);\
    }

#define SYS_L2_FDB_GLOBAL_PORT_CHECK_WITH_UNLOCK(gport,lock) \
    if (gport >CTC_MAX_GPORT_ID || (gport >= 0x1E00 && gport <= 0x1EFF))\
        {CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_INVALID_GLOBAL_PORT, lock);}



#define SYS_L2_FDB_FLUSH_COUNT_SET(flush_fdb_cnt_per_loop)  \
    {\
        flush_fdb_cnt_per_loop = pl2_master->flush_fdb_cnt_per_loop;\
    }


#define SYS_L2_FDB_FLUSH_COUNT_PROCESS(flush_fdb_cnt_per_loop)  \
    {\
        if ( flush_fdb_cnt_per_loop > 0 )\
        {\
            flush_fdb_cnt_per_loop--;\
            if ( 0 == flush_fdb_cnt_per_loop )\
            {\
                return CTC_E_OPERATION_PAUSE;\
            }\
        }\
    }

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
struct sys_l2_fdb_port_node_s
{
    ctc_linklist_t* port_fdb_list;
    uint32 dynmac_count;
    uint32 local_dynmac_count;
};
typedef struct sys_l2_fdb_port_node_s sys_l2_fdb_port_node_t;

struct sys_l2_fdb_build_hw_entry_s
{
    ctc_l2_addr_t l2_addr;
    uint32 fwd_ptr;
    uint8 fdb_type;
    uint8 rewrite;
    uint16 src_gport;
    uint32 sw_hash_index;
    uint8 port_dft_valid;
    uint8 aps_en;
};
typedef struct sys_l2_fdb_build_hw_entry_s sys_l2_fdb_build_hw_entry_t;

struct sys_l2_fdb_entry_s
{
   uint32  do_asic_hash_count;
   uint32  L2MC_count;
   uint32  static_count;
   uint32  default_count;
    uint32  total_count;
};
typedef struct sys_l2_fdb_entry_s sys_l2_fdb_entry_t;


enum sys_l2_fdb_type_e {
    SYS_L2_FDB_TYPE_NORMAL_FDB,
    SYS_L2_FDB_TYPE_VLAN_DFT,
    SYS_L2_FDB_TYPE_MCAST_FDB,
    SYS_L2_FDB_TYPE_SYS_DFT
};

sys_l2_master_t* pl2_master = NULL;


struct sys_l2_fdb_mac_hash_info_s
{
    mac_addr_t mac;
    uint32 query_flag;
    ctc_l2_fdb_query_rst_t* query_rst;
    uint32 count_index;
};
typedef struct sys_l2_fdb_mac_hash_info_s sys_l2_fdb_mac_hash_info_t;


/**
 @brief flush infomation when flush all
*/
struct sys_l2_fdb_flush_info_s
{
    uint32 flush_fdb_cnt_per_loop; /**< flush flush_fdb_cnt_per_loop entries one time if flush_fdb_cnt_per_loop>0,
                                        flush all entries if flush_fdb_cnt_per_loop=0 */
    uint8 chip_num;                /**< the number of chip */
    uint8 rsv1;
    uint16 rsv2;
};
typedef struct sys_l2_fdb_flush_info_s sys_l2_fdb_flush_info_t;


#define L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list)\
    { \
       sys_l2_fdb_vlan_node_t* fid_node_lkp;\
       fid_node_lkp = _sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(fid,0,0);\
       fdb_list = fid_node_lkp?fid_node_lkp->vlan_fdb_list:NULL;\
    }

#define L2_FDB_GET_FID_NODE_BY_FID(fid, fid_node)\
    { \
       fid_node = _sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(fid,0,0);\
    }

#define L2_FDB_GET_DFT_NODE_BY_FID_PORT(fid, gport, port_valid, p_dft_node)\
    { \
       sys_l2_fdb_vlan_node_t* fid_node_lkp;\
       fid_node_lkp = _sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(fid,gport,port_valid);\
       p_dft_node = fid_node_lkp?(fid_node_lkp->fdb_dft_node):NULL;\
    }

#define L2_FDB_GET_FID_NODE_BY_FID_PORT(fid, gport, port_vlaid, fid_node)\
    { \
       fid_node = _sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(fid,gport,port_vlaid);\
    }

uint8 status[65536]={0};
/****************************************************************************
 *
* Function
*
*****************************************************************************/

static int32
_sys_humber_l2_fdb_get_hash_bit_num(uint32 bucket_num, uint8* bit_num)
{
    CTC_PTR_VALID_CHECK(bit_num);

    switch (bucket_num)
    {
    case 256:
        *bit_num = 0;
        break;
    case 512:
        *bit_num = 1;
        break;
     case 1024:
        *bit_num = 2;
        break;
    case 2*1024:
        *bit_num = 3;
        break;
    case 4*1024:
        *bit_num = 4;
        break;
    case 8*1024:
        *bit_num = 5;
        break;
    case 16*1024:
        *bit_num = 6;
        break;
    case 32*1024:
        *bit_num = 7;
        break;
    default:
        return CTC_E_UNEXPECT;;

    }

    return CTC_E_NONE;
}


static uint32
_sys_humber_l2_fdb_hash_make (sys_l2_node_t* backet)
{
    uint32 a = 0, b = 0, c = 0, tmp = 0, i = 0;

    if (!backet)
        return 0;

    for (i = CTC_ETH_ADDR_LEN - 1; i >  1; i --)
    {
        tmp  = backet->key.mac[i];
        a += tmp << ((i - 2)*8);
    }

    tmp = backet->key.mac[1];
    b += tmp << 24;
    tmp = backet->key.mac[0];
    b += tmp << 16;
    b += backet->key.fid;

    a += SYS_L2_FDB_HASH_GOLDEN_RATIO;
    b += SYS_L2_FDB_HASH_GOLDEN_RATIO;
    c += 8; /*length*/

    SYS_FDB_FDB_HASH_MIX(a,b,c);

    return (c%(pl2_master->sw_hash_tbl_size));
}

static bool
_sys_humber_l2_fdb_hash_compare (sys_l2_node_t* backet_node,sys_l2_node_t* lkup_node)
{
    if (!backet_node || !lkup_node)
    {
        return FALSE;
    }

    if ( !kal_memcmp (backet_node->key.mac, lkup_node->key.mac, CTC_ETH_ADDR_LEN)
        && (backet_node->key.fid == lkup_node->key.fid ))
    {
        return TRUE;
    }

    return FALSE;
}

/* normal fdb hash opearation*/
static int32
_sys_humber_l2_fdb_add_to_hash_table(sys_l2_node_t* p_fdb_node)
{
    ctc_hash_insert(pl2_master->fdb_hash, p_fdb_node);
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_remove_from_hash_table(sys_l2_node_t* p_fdb_node)
{
    ctc_hash_remove(pl2_master->fdb_hash, p_fdb_node);
    return CTC_E_NONE;
}

static sys_l2_node_t*
_sys_humber_l2_fdb_lkup_from_hash_table(sys_l2_node_t* p_fdb_node)
{
    return ctc_hash_lookup2(pl2_master->fdb_hash, p_fdb_node, &p_fdb_node->index);
}


static uint32
_sys_humber_l2_fdb_fid_entry_hash_make (sys_l2_fdb_vlan_node_t* backet)
{

    uint32 a = 0, b = 0, c = 0;

    if (!backet)
        return 0;

    a += backet->gport << 16;
    a += backet->fid;

    a += SYS_L2_FDB_HASH_GOLDEN_RATIO;
    b += SYS_L2_FDB_HASH_GOLDEN_RATIO;

    c += 4;

    SYS_FDB_FDB_HASH_MIX(a,b,c);

    return c%(SYS_L2_FDB_DFT_ENTRY_HASH_TBL_SIZE);

}


static bool
_sys_humber_l2_fdb_fid_entry_hash_compare (sys_l2_fdb_vlan_node_t* backet_node,sys_l2_fdb_vlan_node_t* lkup_node)
{
    if (!backet_node || !lkup_node)
    {
        return FALSE;
    }

    if ( (backet_node->fid == lkup_node->fid ) && (backet_node->gport == lkup_node->gport ))
    {
        return TRUE;
    }

    return FALSE;
}


/*fdb fid entry hash opearation*/

static int32
_sys_humber_l2_fdb_fid_entry_add_to_hash_table(sys_l2_fdb_vlan_node_t* fid_node)
{

    fid_node->gport = fid_node->port_valid ? fid_node->gport:0xFFFF;
    ctc_hash_insert(pl2_master->fdb_dft_entry_hash, fid_node);
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_fid_entry_remove_from_hash_table(sys_l2_fdb_vlan_node_t* fid_node)
{
    ctc_hash_remove(pl2_master->fdb_dft_entry_hash, fid_node);
    return CTC_E_NONE;
}

sys_l2_fdb_vlan_node_t*
_sys_humber_l2_fdb_fid_entry_lkup_from_hash_table(uint16 fid, uint16 gport, uint8 port_valid)
{
   sys_l2_fdb_vlan_node_t fid_node_tmp;
   kal_memset(&fid_node_tmp, 0, sizeof(sys_l2_fdb_vlan_node_t));

   fid_node_tmp.fid   = fid;
   fid_node_tmp.gport = port_valid ? gport:0xFFFF;

   return ctc_hash_lookup(pl2_master->fdb_dft_entry_hash, &fid_node_tmp);
}


static uint32
_sys_humber_l2_fdb_mac_entry_hash_make (sys_l2_node_t* backet)
{
    uint32 a = 0, b = 0, c = 0, tmp = 0, i = 0;

    if (!backet)
        return 0;

    for (i = CTC_ETH_ADDR_LEN - 1; i >  1; i --)
    {
        tmp  = backet->key.mac[i];
        a += tmp << ((i - 2)*8);
    }

    tmp = backet->key.mac[1];
    b += tmp << 8;
    tmp = backet->key.mac[0];
    b += tmp;

    a += SYS_L2_FDB_HASH_GOLDEN_RATIO;
    b += SYS_L2_FDB_HASH_GOLDEN_RATIO;
    c += 6; /*length*/

    SYS_FDB_FDB_HASH_MIX(a,b,c);

    return (c%(SYS_L2_FDB_MAC_ENTRY_HASH_TBL_SIZE));

}


/*fdb mac entry hash opearation*/

static int32
_sys_humber_l2_fdb_mac_entry_add_to_hash_table(sys_l2_node_t* p_fdb_node)
{
    ctc_hash_insert(pl2_master->fdb_mac_hash, p_fdb_node);
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_mac_entry_remove_from_hash_table(sys_l2_node_t* p_fdb_node)
{
    ctc_hash_remove(pl2_master->fdb_mac_hash, p_fdb_node);
    return CTC_E_NONE;
}



static int32
_sys_humber_l2_fdb_add_to_fdb_port_list(sys_l2_node_t* l2_node)
{
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_fdb_port_node_t *port_fdb_node = NULL;

    if (l2_node->port_entey != NULL)
    {
        return CTC_E_ENTRY_EXIST;
    }

    if(l2_node->gport == CTC_MAX_UINT16_VALUE)
    {
       return CTC_E_NONE;
    }

    port_fdb_node = ctc_vector_get(pl2_master->gport_vec, l2_node->gport);


    if (NULL == port_fdb_node)
    {
      port_fdb_node = (sys_l2_fdb_port_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_fdb_port_node_t));
      if (NULL == port_fdb_node)
      {
          return CTC_E_NO_MEMORY;
      }

      kal_memset(port_fdb_node, 0, sizeof(sys_l2_fdb_port_node_t));
      port_fdb_node->port_fdb_list = ctc_list_new();
      if (NULL == port_fdb_node->port_fdb_list)
      {
          mem_free(port_fdb_node);
          port_fdb_node = NULL;
          return CTC_E_NO_MEMORY;
      }
      ctc_vector_add(pl2_master->gport_vec, l2_node->gport ,port_fdb_node);
    }
    fdb_list = port_fdb_node->port_fdb_list;
    l2_node->port_entey = ctc_listnode_add_tail(fdb_list, l2_node);
    if (NULL == l2_node->port_entey)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }
    if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
     if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
     {
         port_fdb_node->local_dynmac_count++;
     }
     port_fdb_node->dynmac_count++;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_remove_from_fdb_port_list (sys_l2_node_t* l2_node)
{
    ctc_linklist_t* fdb_list = NULL;
   sys_l2_fdb_port_node_t *port_fdb_node = NULL;

    CTC_PTR_VALID_CHECK(l2_node->port_entey);

    if(l2_node->gport == CTC_MAX_UINT16_VALUE)
    {
       return CTC_E_NONE;
    }
    port_fdb_node = ctc_vector_get(pl2_master->gport_vec, l2_node->gport);

    if (NULL == port_fdb_node || NULL == port_fdb_node->port_fdb_list)
    {
       return CTC_E_ENTRY_NOT_EXIST;
    }
    fdb_list = port_fdb_node->port_fdb_list;
    if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
       if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
       {
        port_fdb_node->local_dynmac_count = port_fdb_node->local_dynmac_count?port_fdb_node->local_dynmac_count--:0;
       }
       port_fdb_node->dynmac_count = port_fdb_node->dynmac_count?port_fdb_node->dynmac_count--:0;
     }
    ctc_listnode_delete_node(fdb_list,  l2_node->port_entey );
    l2_node->port_entey = NULL;
    if (0 == CTC_LISTCOUNT(fdb_list))
    {
        ctc_list_free(fdb_list);
	    mem_free(port_fdb_node);
        ctc_vector_del(pl2_master->gport_vec, l2_node->gport);
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_add_to_fdb_vlan_list(sys_l2_node_t* l2_node)
{

    sys_l2_fdb_vlan_node_t  *fid_node = NULL;

    if (l2_node->vlan_entey != NULL)
    {
        return CTC_E_ENTRY_EXIST;
    }
    if(l2_node->key.fid  == CTC_MAX_UINT16_VALUE)
    {
       return CTC_E_NONE;
    }

    L2_FDB_GET_FID_NODE_BY_FID(l2_node->key.fid, fid_node);
    if (NULL == fid_node)
    {
      fid_node = (sys_l2_fdb_vlan_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_fdb_vlan_node_t));
      if (NULL == fid_node)
      {
          return CTC_E_NO_MEMORY;
      }

      kal_memset(fid_node, 0, sizeof(sys_l2_fdb_vlan_node_t));
      fid_node->vlan_fdb_list = ctc_list_new();
      if (NULL == fid_node->vlan_fdb_list)
      {
          mem_free(fid_node);
          fid_node = NULL;
          return CTC_E_NO_MEMORY;
      }
      fid_node->fid = l2_node->key.fid;
      fid_node->port_valid = 0;
      _sys_humber_l2_fdb_fid_entry_add_to_hash_table(fid_node);
    }

    l2_node->vlan_entey = ctc_listnode_add_tail(fid_node->vlan_fdb_list, l2_node);
    if (NULL == l2_node->vlan_entey)
    {
        return CTC_E_NO_MEMORY;
    }
    if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
       if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
       {
           fid_node->local_dynmac_count++;
       }
       fid_node->dynmac_count++;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_remove_from_fdb_vlan_list (sys_l2_node_t* l2_node)
{

    sys_l2_fdb_vlan_node_t* fid_node = NULL;

    CTC_PTR_VALID_CHECK(l2_node->vlan_entey);

    if(l2_node->key.fid == CTC_MAX_UINT16_VALUE)
    {
       return CTC_E_NONE;
    }

    L2_FDB_GET_FID_NODE_BY_FID(l2_node->key.fid, fid_node);
    if (NULL == fid_node )
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }
    if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
       if(!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
       {
         fid_node->local_dynmac_count--;
       }
        fid_node->dynmac_count--;
    }

     ctc_listnode_delete_node(fid_node->vlan_fdb_list, l2_node->vlan_entey );
    l2_node->vlan_entey = NULL;
    if (CTC_LISTCOUNT(fid_node->vlan_fdb_list) == 0)
    {
        if (NULL == fid_node->fdb_dft_node)
        {
            ctc_list_delete(fid_node->vlan_fdb_list);
            _sys_humber_l2_fdb_fid_entry_remove_from_hash_table(fid_node);
            mem_free(fid_node);
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_add_to_dflt_entry(sys_l2_node_t* pfdb_node, uint16 gport, uint8 port_valid)
{
    sys_l2_fdb_vlan_node_t* p_fid_node = NULL;

    CTC_PTR_VALID_CHECK(pfdb_node);
    CTC_FID_RANGE_CHECK(pfdb_node->key.fid);

    L2_FDB_GET_FID_NODE_BY_FID_PORT(pfdb_node->key.fid, gport,port_valid, p_fid_node);
    if (NULL == p_fid_node )
    {
        p_fid_node = (sys_l2_fdb_vlan_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_fdb_vlan_node_t));
        if (NULL == p_fid_node)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_fid_node,0,sizeof(sys_l2_fdb_vlan_node_t));
        p_fid_node->vlan_fdb_list = ctc_list_new();
        if (NULL == p_fid_node->vlan_fdb_list)
        {
            mem_free(p_fid_node);
            p_fid_node = NULL;
            return CTC_E_NO_MEMORY;
        }
        p_fid_node->fid = pfdb_node->key.fid;
        p_fid_node->gport = gport;
        p_fid_node->port_valid = port_valid;
        _sys_humber_l2_fdb_fid_entry_add_to_hash_table(p_fid_node);
    }
    p_fid_node->fdb_dft_node = pfdb_node;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_fdb_remove_from_dflt_entry(sys_l2_node_t* pfdb_node, uint16 gport, uint8 port_valid)
{
    sys_l2_fdb_vlan_node_t* p_fid_node = NULL;

    CTC_PTR_VALID_CHECK(pfdb_node);
    CTC_FID_RANGE_CHECK(pfdb_node->key.fid);

    L2_FDB_GET_FID_NODE_BY_FID_PORT(pfdb_node->key.fid, gport,port_valid,p_fid_node);
    if (NULL == p_fid_node)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }
    p_fid_node->fdb_dft_node  = NULL;

    if(CTC_LISTCOUNT(p_fid_node->vlan_fdb_list) == 0)
    {
       ctc_list_free(p_fid_node->vlan_fdb_list);
       _sys_humber_l2_fdb_fid_entry_remove_from_hash_table(p_fid_node);
       mem_free(p_fid_node);
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_l2_build_macda_entry(sys_l2_fdb_build_hw_entry_t* p_bd_hw_entry, ds_mac_t* macda)
{
    uint32 flag = p_bd_hw_entry->l2_addr.flag;

    macda->fwd_ptr = p_bd_hw_entry->fwd_ptr;

    switch (p_bd_hw_entry->fdb_type)
    {
        case SYS_L2_FDB_TYPE_NORMAL_FDB:
            macda->source_port_check_en = 1;
            macda->global_src_port = p_bd_hw_entry->l2_addr.gport;
            macda->mac_sa_exception_en = 0;
            macda->learn_source0 = 0;
            macda->learn_source1 = 0;
            macda->mac_known = 1;
            if (!(flag & CTC_L2_FLAG_IS_STATIC))
            {
                macda->src_mismatch_learn_en = 1;
            }

            if(p_bd_hw_entry->aps_en)
            {
                macda->src_mismatch_learn_en = 0;
            }

            if (flag & CTC_L2_FLAG_BIND_PORT)
            {
                macda->src_mismatch_discard = 1;
            }

            if (CTC_L2_FDB_SRC_PORT_TYPE_VPLS == p_bd_hw_entry->l2_addr.gsrcport_type)
            {
                macda->global_src_port = p_bd_hw_entry->src_gport;
                macda->esp_key_or_oam = p_bd_hw_entry->l2_addr.logic_port;
                macda->learn_source0 = 1;  /* from vpls port / qinq service queue */
                macda->learn_source1 = 0;
            }

            if (CTC_L2_FDB_SRC_PORT_TYPE_PBB == p_bd_hw_entry->l2_addr.gsrcport_type)
            {
                macda->global_src_port = p_bd_hw_entry->src_gport;
                macda->learn_source0 = 0;  /* from PBB PIP */
                macda->learn_source1 = 1;
            }
            if(flag & CTC_L2_FLAG_RAW_PKT_ELOG_CPU )
            {
                macda->source_port_check_en = 0;
                macda->learn_en = 0;
                macda->src_mismatch_learn_en = 0;
                macda->src_mismatch_discard = 0;
            }
         if(flag & CTC_L2_FLAG_UCAST_DISCARD )
            {
                macda->ucast_discard = 1;
            }
            break;

        case SYS_L2_FDB_TYPE_VLAN_DFT:
            macda->learn_en = 1;
            macda->mac_known = 0;
            macda->mcast_discard = 0;
            break;

        case SYS_L2_FDB_TYPE_MCAST_FDB:
             macda->mac_known = 1;
            break;

        case SYS_L2_FDB_TYPE_SYS_DFT:
            macda->fwd_ptr = SYS_FDB_DISCARD_FWD_PTR;
            macda->learn_en = 0;
            macda->mac_known = 0;

             break;
        default:
            break;

    }

    if (flag & CTC_L2_FLAG_DISCARD)
    {
        macda->fwd_ptr = SYS_FDB_DISCARD_FWD_PTR;
    }

    if (flag & CTC_L2_FLAG_SRC_DISCARD)
    {
        macda->src_discard = 1;
        macda->mac_sa_exception_en = 0;
    }

    if (flag & CTC_L2_FLAG_SRC_DISCARD_TOCPU)
    {
        macda->src_discard = 1;
        macda->mac_sa_exception_en = 1;
    }

    if (flag & CTC_L2_FLAG_COPY_TO_CPU)
    {
        macda->mac_da_exception_en = 1;
        /* Reserved for normal MACDA copy_to_cpu */
        /* #define SYS_L2PDU_PER_PORT_ACTION_INDEX_RSV_MACDA_TO_CPU 15 */
        macda->exception_sub_index = 15;
    }

    if (flag & CTC_L2_FLAG_PROTOCOL_ENTRY)
    {
        macda->proto_exception_en = 1;
    }
    macda->storm_ctl_en = 1;
    return CTC_E_NONE;
}

static int32
_sys_humber_l2_build_mackey_entry(sys_l2_fdb_build_hw_entry_t* p_bd_hw_entry, tbl_entry_t* mackey)
{

    mac_addr_t mac;
    mac_addr_t mac_mask;

    ds_mac_key_t *data = (ds_mac_key_t *)mackey->data_entry;
    ds_mac_key_t *mask = (ds_mac_key_t *)mackey->mask_entry;

    kal_memset(data, 0, sizeof(ds_mac_key_t));
    kal_memset(mask, 0, sizeof(ds_mac_key_t));

    kal_memcpy(mac, p_bd_hw_entry->l2_addr.mac, sizeof(mac_addr_t));
    kal_memcpy(mac_mask, p_bd_hw_entry->l2_addr.mask, sizeof(mac_addr_t));

    data->table_id0 = MACDA_TABLEID0_C;
    data->table_id1 = MACDA_TABLEID1_C;

    data->mapped_mac_da_lower = mac[5]|(mac[4]<<8)|(mac[3]<<16)|(mac[2]<<24);
    data->mapped_mac_da_upper = (mac[0]<<8)|mac[1];
    data->mapped_vlan_id =p_bd_hw_entry->l2_addr.fid;

    switch (p_bd_hw_entry->fdb_type)
    {
        case SYS_L2_FDB_TYPE_NORMAL_FDB:
        case SYS_L2_FDB_TYPE_MCAST_FDB:
            if (p_bd_hw_entry->l2_addr.mask_valid)
            {
                mask->mapped_mac_da_lower =  mac_mask[5]|(mac_mask[4]<<8)|(mac_mask[3]<<16)|(mac_mask[2]<<24);
                mask->mapped_mac_da_upper =  (mac_mask[0]<<8)|mac_mask[1];
            }
            else
            {
                mask->mapped_mac_da_lower =  0xffffffff;
                mask->mapped_mac_da_upper = 0xffff;
            }

            if (p_bd_hw_entry->l2_addr.fid != 0xFFFF)
            {
                mask->mapped_vlan_id = 0xFFFF;
            }
            else
            {
                mask->mapped_vlan_id = 0;
            }
            break;

        case SYS_L2_FDB_TYPE_VLAN_DFT:
            mask->mapped_mac_da_lower = 0;
            mask->mapped_mac_da_upper = 0;
            mask->mapped_vlan_id =  0xffff;
            if (p_bd_hw_entry->port_dft_valid)
            {
                data->gbl_src_port = p_bd_hw_entry->src_gport;
                mask->gbl_src_port = 0x1FFF;
            }
            SYS_FDB_DBG_INFO("DATA fid: 0x%x port: 0x%x\n",data->mapped_vlan_id , data->gbl_src_port);
            SYS_FDB_DBG_INFO("MASK fid: 0x%x port: 0x%x\n",mask->mapped_vlan_id , mask->gbl_src_port);
            break;

        case SYS_L2_FDB_TYPE_SYS_DFT:
            kal_memset(mask, 0, sizeof(ds_mac_key_t));
            break;

        default:
            break;
    }
    mask->table_id0 = 0xF;
    mask->table_id1 = 0xF;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_write_hash_key(uint8 chip_id, sys_l2_node_t* pl2_node)
{
    uint32 index = 0;
    uint32 cmd = 0;
    ds_mac_hash_key0_t mac_hash_key;

    index = pl2_node->index - pl2_master->hash_base;
    kal_memset(&mac_hash_key,0,sizeof(mac_hash_key));

    mac_hash_key.mapped_vlanid = pl2_node->key.fid;
    mac_hash_key.mapped_mach  = (pl2_node->key.mac[0] << 8)  + pl2_node->key.mac[1];
    mac_hash_key.mapped_macl = (pl2_node->key.mac[2] << 24) + (pl2_node->key.mac[3] << 16) +(pl2_node->key.mac[4] << 8 )+ pl2_node->key.mac[5];

    cmd = DRV_IOW(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &mac_hash_key));

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_asic_hash_lkp_by_direction(uint8 chip_id,
                             sys_l2_node_t* l2_node, uint8 left,
                             hash_ds_ctl_cpu_key_status_t* hash_cpu_status)
{
    uint32 bucket_left = 0, bucket_right = 0;
    uint8 hash_key[8] = {0};
    uint8 bucket_status = 0;


    hash_key[7] = (l2_node->key.fid >> 8) & 0xFF;
    hash_key[6] = l2_node->key.fid & 0xFF;
    hash_key[5] = l2_node->key.mac[0];
    hash_key[4] = l2_node->key.mac[1];
    hash_key[3] = l2_node->key.mac[2];
    hash_key[2] = l2_node->key.mac[3];
    hash_key[1] = l2_node->key.mac[4];
    hash_key[0] = l2_node->key.mac[5];
    hash_cpu_status->cpu_lu_index = 0;

    if (left)
    {

        bucket_left = sys_humber_hash_generate_mac_hash0(hash_key, pl2_master->hash_bit_num);
        bucket_status = pl2_master->hash_status[bucket_left] &0x03;
        if (bucket_status != 3)
        {
            if (bucket_status == 2 || bucket_status == 0)
            {
                hash_cpu_status->cpu_lu_index = bucket_left*4 + 0;
                pl2_master->hash_status[bucket_left] |= 1;
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_left*4 + 1;
                pl2_master->hash_status[bucket_left]  |= 1 << 1;
            }
        }
        else
        {
            hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
        }
    }
    else
    {
        bucket_right = sys_humber_hash_generate_mac_hash1(hash_key, pl2_master->hash_bit_num);
        bucket_status = (pl2_master->hash_status[bucket_right] >> 2) & 0x03;

        if (bucket_status != 3)
        {
            if (bucket_status == 2 || bucket_status == 0)
            {
                hash_cpu_status->cpu_lu_index = bucket_right*4 + 2;
                pl2_master->hash_status[bucket_right] |= 1 << 2;
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_right*4 + 3;
                pl2_master->hash_status[bucket_right] |= 1 << 3;
            }
        }
        else
        {
            hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
        }
    }
    return CTC_E_NONE;
}


static int32
_sys_humber_l2_asic_hash_lkp(uint8 chip_id,
                             sys_l2_node_t* l2_node,
                             hash_ds_ctl_cpu_key_status_t* hash_cpu_status)
{
    uint32 bucket_left = 0, bucket_right = 0;
    uint8 hash_key[8] = {0};
    uint8 bucket_left_status = 0,bucket_right_status =0;
    uint8 bucket_left_cnt = 0,bucket_right_cnt =0;
    uint8 use_left_block = 0;
    uint8 find_flag = 0;
    uint32  fdb_index = 0;
    uint32  fdb_index2 = 0;
    sys_l2_node_t  *p_hash_l2_node;
    hash_ds_ctl_cpu_key_status_t  hash_ds_ctl_status;
    
    kal_memset(&hash_ds_ctl_status,0,sizeof(hash_ds_ctl_cpu_key_status_t));
    
    hash_key[7] = (l2_node->key.fid >> 8) & 0xFF;
    hash_key[6] = l2_node->key.fid & 0xFF;
    hash_key[5] = l2_node->key.mac[0];
    hash_key[4] = l2_node->key.mac[1];
    hash_key[3] = l2_node->key.mac[2];
    hash_key[2] = l2_node->key.mac[3];
    hash_key[1] = l2_node->key.mac[4];
    hash_key[0] = l2_node->key.mac[5];
    hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;

    bucket_left = sys_humber_hash_generate_mac_hash0(hash_key, pl2_master->hash_bit_num);
    bucket_left_status = pl2_master->hash_status[bucket_left] &0x03;
   
    bucket_right = sys_humber_hash_generate_mac_hash1(hash_key, pl2_master->hash_bit_num);
    bucket_right_status = (pl2_master->hash_status[bucket_right] >> 2) & 0x03;
   
    bucket_left_cnt    = 0;
    bucket_right_cnt  = 0;
   
    if (bucket_left_status != 0 )
    {
        bucket_left_cnt  = (bucket_left_status   == 3) ?2:1;
    }
   
    if (bucket_right_status != 0 )
    {
        bucket_right_cnt  = (bucket_right_status   == 3) ?2:1;
    }
   
    if ((bucket_left_status == bucket_right_status)  || (bucket_left_status ==3) || (bucket_right_status ==3))
    {  // [0:0] [1:1] [0:2][1:2]  [2:0]  [2:1]  [2:2]
         if ((bucket_left_status != 3 ) || ( bucket_right_status != 3)) //[0:0] [1:1] [0:2][1:2]  [2:0]  [2:1]
         {
             if ((bucket_left_status != 3) &&  (bucket_right_status != 3)) // [0:0][1:1]
             {
                 if (pl2_master->do_left_hash_count  <=  pl2_master->do_right_hash_count )
                 {
                     use_left_block  = 1;
                 }
                 else
                 {
                     use_left_block  = 0;
                 }
             }
             else
             {   // [0:2][1:2]  [2:0]  [2:1]
                 use_left_block  = bucket_left_status != 3;
             }
        }
        else //[2:2]
        {
            uint32 cmd = 0 ;
            ds_mac_t macda;
            uint16 new_fdb_index = 0;
            ds_mac_hash_key0_t mac_hash_key;
   
            kal_memset(&macda, 0, sizeof(ds_mac_t));
            kal_memset(&mac_hash_key, 0, sizeof(ds_mac_hash_key0_t));
            find_flag  = 0;
            hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
            hash_ds_ctl_status.cpu_lu_index    = DRV_HASH_INVALID_INDEX;
   
            if (pl2_master->do_hash_no_conflict_mode)
            {  //[2:2]
   
                fdb_index =  bucket_left*4 +  pl2_master->hash_base ;
                p_hash_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, fdb_index);
                if (!p_hash_l2_node)
                {
                    SYS_FDB_DBG_INFO("get error!\n");
                    return CTC_E_NONE;
                }
   
                _sys_humber_l2_asic_hash_lkp_by_direction(chip_id, p_hash_l2_node, 0, &hash_ds_ctl_status);
   
                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_fdb_index  = hash_ds_ctl_status.cpu_lu_index;
                    find_flag = 1;
                }
                else
                {
                    fdb_index =  bucket_left*4 + 1 +  pl2_master->hash_base ;
                    p_hash_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, fdb_index);
                    if (!p_hash_l2_node)
                    {
                        SYS_FDB_DBG_INFO("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l2_asic_hash_lkp_by_direction(chip_id, p_hash_l2_node, 0, &hash_ds_ctl_status);
                }
   
                if (find_flag == 1)
                    goto FOUND;
   
   
                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_fdb_index  = hash_ds_ctl_status.cpu_lu_index ;
                    find_flag = 1;
                }
                else
                {
                    fdb_index =  bucket_right*4 + 2 + pl2_master->hash_base ;
                    p_hash_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, fdb_index);
                    if (!p_hash_l2_node)
                    {
                        SYS_FDB_DBG_INFO("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l2_asic_hash_lkp_by_direction(chip_id, p_hash_l2_node, 1, &hash_ds_ctl_status);
   
                }
   
                if (find_flag == 1)
                    goto FOUND;
   
                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_fdb_index = hash_ds_ctl_status.cpu_lu_index ;
                    find_flag = 1;
                }
                else
                {
                    fdb_index =  bucket_right*4 + 3 + pl2_master->hash_base;
                    p_hash_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, fdb_index);
                    if (!p_hash_l2_node)
                    {
                        SYS_FDB_DBG_INFO("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l2_asic_hash_lkp_by_direction(chip_id, p_hash_l2_node, 1, &hash_ds_ctl_status);
   
                }
   
                if (find_flag == 1)
                    goto FOUND;
   
                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_fdb_index = hash_ds_ctl_status.cpu_lu_index ;
                    find_flag = 1;
                }
   
FOUND:
                if (find_flag == 1)
                {
                    uint16 hash_index = 0;
                    uint8 index = 0;
   
                    /*move old entry to new position*/
                    cmd = DRV_IOR(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
                    drv_tbl_ioctl(chip_id,  p_hash_l2_node ->index, cmd, &macda);
   
   
                    cmd = DRV_IOW(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
                    drv_tbl_ioctl(chip_id, new_fdb_index + pl2_master->hash_base, cmd, &macda);
   
                    /*delete old hash key*/
                    mac_hash_key.mapped_vlanid = 0xFFFF;
                    cmd = DRV_IOW(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
                    drv_tbl_ioctl(chip_id,  p_hash_l2_node ->index - pl2_master->hash_base, cmd, &mac_hash_key);
   
                    hash_index  =  p_hash_l2_node->index - pl2_master->hash_base;
                    index = hash_index % 4;
                    if (pl2_master->do_hash_count > 0)
                    {
                        pl2_master->do_hash_count--;
                        if (index < 2 )
                        {
                            pl2_master->do_left_hash_count--;
   
                        }
                        else
                        {
                            pl2_master->do_right_hash_count--;
                        }
                    }

                    CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, p_hash_l2_node ->index, FALSE));

                    ctc_vector_del(pl2_master->fdb_tbl_vec, p_hash_l2_node->index);
                    fdb_index2  =  p_hash_l2_node->index;
   
                    /*add new hash key*/
                    p_hash_l2_node->index  = new_fdb_index + pl2_master->hash_base ;
                    _sys_humber_l2_write_hash_key( chip_id, p_hash_l2_node);
                    ctc_vector_add(pl2_master->fdb_tbl_vec, p_hash_l2_node->index, p_hash_l2_node);
   
                    hash_index  =  p_hash_l2_node->index - pl2_master->hash_base;
                    index = hash_index % 4;
                    if (index < 2 )
                    {
                        pl2_master->do_left_hash_count++;
   
                    }
                    else
                    {
                        pl2_master->do_right_hash_count++;
                    }
                    pl2_master->do_hash_count++;
                    hash_cpu_status->cpu_lu_index  = fdb_index -  pl2_master->hash_base;

                    if (p_hash_l2_node->flag & SYS_L2_NODE_FLAG_IS_STATIC)
                    {
                        CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, p_hash_l2_node->index, FALSE));
                    }
                    else
                    {
                        CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, p_hash_l2_node->index, TRUE));
                    }
                }
   
            }
            return CTC_E_NONE;
        }
   
    }
    else
    {
           /* [0:1]  / [1:0]  */
           use_left_block  = bucket_left_cnt < bucket_right_cnt;
    }
   
    if (use_left_block)
    {
        if (bucket_left_status == 2 || bucket_left_status == 0)
        {
              hash_cpu_status->cpu_lu_index = bucket_left*4 + 0;
              pl2_master->hash_status[bucket_left] |=  1;
        }
        else
        {
             hash_cpu_status->cpu_lu_index = bucket_left*4 + 1;
             pl2_master->hash_status[bucket_left] |=  1 << 1;
        }
    }
    else
    {
          if (bucket_right_status == 2 || bucket_right_status == 0)
           {
               hash_cpu_status->cpu_lu_index = bucket_right*4 +2;
               pl2_master->hash_status[bucket_right] |= 1<<2;
           }
           else
           {
               hash_cpu_status->cpu_lu_index = bucket_right*4 +3;
               pl2_master->hash_status[bucket_right] |= 1<<3;
           }
    }


    return CTC_E_NONE;
}

static int32
_sys_humber_l2_remove_fdb_from_hw(uint8 chip_id, sys_l2_node_t* l2_node)
{
    uint32 index = 0;
    uint32 cmd = 0;
    ds_mac_hash_key0_t mac_hash_key;

    if (!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_ASIC_HASH))
    {
        index = l2_node->index - pl2_master->tcam_base;
        CTC_ERROR_RETURN(drv_tcam_tbl_remove(chip_id, DS_MAC_KEY, index));
    }
    else
    {
        index = l2_node->index - pl2_master->hash_base;


        mac_hash_key.mapped_vlanid = 0xFFFF;

        cmd = DRV_IOW(IOC_TABLE, DS_MAC_HASH_KEY0, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, index, cmd, &mac_hash_key));
   }

   CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, l2_node->index, FALSE));

   return CTC_E_NONE;
}

static int32
_sys_humber_l2_free_index(sys_l2_node_t* l2_node, sys_l2_fdb_build_hw_entry_t* p_bd_hw_entry)
{
    uint32 opf_index = 0;
    uint32 hash_index = 0;
    uint32 bucket_index = 0;
    uint8 index = 0;
    sys_humber_opf_t opf;

    if (!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_ASIC_HASH))
    {
        opf_index = l2_node->index - pl2_master->tcam_base;
        opf.pool_type = FDB_SRAM_HASH_COLLISION_KEY;
        opf.pool_index = opf_index/SYS_L2_FDB_POOL_BLOCK_SIZE;
        if (opf.pool_index >=  pl2_master->offset_pool_num)
        {
            opf.pool_index  = pl2_master->offset_pool_num - 1;
        }
        CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, opf_index));
        pl2_master->pool_status[opf.pool_index] = 0;

    }
    else
    {
       hash_index  =  l2_node->index - pl2_master->hash_base;
       bucket_index = hash_index/4;
       index = hash_index%4;
       pl2_master->hash_status[bucket_index] &= ~(1<<index);
       if(pl2_master->do_hash_count > 0)
       {
           pl2_master->do_hash_count--;
           if (index == 0 || index == 1 )
           {
               pl2_master->do_left_hash_count--;

           }
           else
           {
               pl2_master->do_right_hash_count--;
           }
       }

    }
    if  (!CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_L2MC)
    	&& !CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_STATIC))
     {
         pl2_master->dynmac_count--;
        if (!CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
        {
            pl2_master->local_dynmac_count--;
        }
     }

    return CTC_E_NONE;
}

static int32
_sys_hubmer_l2_fdb_offset_alloc(uint32 *offset, uint32 *hash_index)
{
    uint32 pool_index = 0;
    uint32 ret = CTC_E_NO_ALLOC_OFFSET;
    sys_humber_opf_t opf;
    int16 i = 0;

    opf.pool_type = FDB_SRAM_HASH_COLLISION_KEY;

    if (pl2_master->offset_pool_num >= 2)
    {
        /* according the hash key to alloc offset */
        pool_index = (*hash_index)%(pl2_master->offset_pool_num-1);

        if ( pl2_master->pool_status[pool_index] != 1)
        {
            opf.pool_index = pool_index;
            ret = sys_humber_opf_alloc_offset(&opf, 1, offset);
            if(ret == CTC_E_NONE )
            {
                return CTC_E_NONE;
            }
            pl2_master->pool_status[pool_index] = 1;
        }

        /*look up offset from 0 to pool number end */
        if (pool_index < (pl2_master->offset_pool_num>>1))
        {
            for (i = 0; i < pl2_master->offset_pool_num-1; i++)
            {
                if ( pl2_master->pool_status[(uint16)i] != 1)
                {
                    opf.pool_index = (uint16)i;
                    ret = sys_humber_opf_alloc_offset(&opf, 1, offset);

                    if(ret == CTC_E_NONE )
                    {
                        return CTC_E_NONE;
                    }
                    pl2_master->pool_status[(uint16)i] = 1;
                }
            }
        }
        else  /*look up offset from pool number end to 0 */
        {
            for (i = pl2_master->offset_pool_num-2; i  >= 0; i--)
            {
                if ( pl2_master->pool_status[(uint16)i] != 1)
                {
                    opf.pool_index = (uint16)i;
                    ret = sys_humber_opf_alloc_offset(&opf, 1, offset);

                    if(ret == CTC_E_NONE)
                    {
                        return CTC_E_NONE;
                    }
                    pl2_master->pool_status[(uint16)i] = 1;
                }
            }
        }

    }


      /*look up offset in last pool number */
       if ( pl2_master->pool_status[pl2_master->offset_pool_num-1] != 1)
       {
            opf.pool_index = pl2_master->offset_pool_num-1;
            ret = sys_humber_opf_alloc_offset(&opf, 1, offset);
            if(ret == CTC_E_NONE)
            {
                return CTC_E_NONE;
            }
            pl2_master->pool_status[pl2_master->offset_pool_num-1] = 1;
       }


    return ret;

}


static int32
_sys_humber_l2_build_index(sys_l2_node_t* l2_node, sys_l2_fdb_build_hw_entry_t* p_bd_hw_entry)
{
    uint8 do_asic_hash = 0;
    hash_ds_ctl_cpu_key_status_t hash_cpu_status;
    int32 ret = CTC_E_NONE;
    uint32 offset = 0;
    sys_humber_opf_t opf;

    CTC_PTR_VALID_CHECK(l2_node);
    CTC_PTR_VALID_CHECK(p_bd_hw_entry);

    l2_node->index = CTC_MAX_UINT32_VALUE;
    kal_memset(&hash_cpu_status, 0, sizeof(hash_ds_ctl_cpu_key_status_t));

    do_asic_hash =  (l2_node->key.fid != 0xFFFF)
                          &&( p_bd_hw_entry->fdb_type == SYS_L2_FDB_TYPE_NORMAL_FDB
                            ||p_bd_hw_entry->fdb_type == SYS_L2_FDB_TYPE_MCAST_FDB)
                           && ( pl2_master->do_hash_count < pl2_master->asic_hash_size);

    if (do_asic_hash)
    {
       ret = _sys_humber_l2_asic_hash_lkp(0, l2_node, &hash_cpu_status);
    }

    do_asic_hash = do_asic_hash && (hash_cpu_status.cpu_lu_index != DRV_HASH_INVALID_INDEX);
    if (!do_asic_hash)
    {
        CTC_UNSET_FLAG(l2_node->flag, SYS_L2_NODE_FLAG_IS_ASIC_HASH);
        switch (p_bd_hw_entry->fdb_type)
        {
            case SYS_L2_FDB_TYPE_NORMAL_FDB:
            case SYS_L2_FDB_TYPE_MCAST_FDB:
                ret = _sys_hubmer_l2_fdb_offset_alloc(&offset,&p_bd_hw_entry->sw_hash_index);
                break;
            case SYS_L2_FDB_TYPE_VLAN_DFT:
                opf.pool_type = FDB_SRAM_HASH_COLLISION_KEY;
                opf.pool_index = pl2_master->offset_pool_num-1;
                ret = sys_humber_opf_reverse_alloc_offset(&opf, 1, &offset);
                break;
            default:
                SYS_FDB_DBG_INFO("invalid FDB type");
                ret = CTC_E_INVALID_PARAM;
                break;
        }
        if (ret == CTC_E_NONE)
        {
            l2_node->index = offset + pl2_master->tcam_base;
        }

    }
    else
    {
        uint16 hash_index = 0;
        uint16 index = 0;

        CTC_SET_FLAG(l2_node->flag, SYS_L2_NODE_FLAG_IS_ASIC_HASH);
        l2_node->index = hash_cpu_status.cpu_lu_index + pl2_master->hash_base;
        pl2_master->do_hash_count++;


        hash_index  =  l2_node->index - pl2_master->hash_base;
        index = hash_index % 4;
        if (index == 0 || index == 1 )
        {
            pl2_master->do_left_hash_count++;

        }
        else
        {
            pl2_master->do_right_hash_count++;
        }


    }

    if (ret!= CTC_E_NONE &&  pl2_master->do_hash_count >= pl2_master->asic_hash_size)
    {
        ret =  CTC_E_FDB_ENTRY_FULL;
    }

    if  (ret == CTC_E_NONE && !CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_L2MC)
    	&& !CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_STATIC))
     {
         pl2_master->dynmac_count++;
        if (!CTC_FLAG_ISSET(l2_node->flag , SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
        {
            pl2_master->local_dynmac_count++;
        }
     }

    return ret;
}

static int32
_sys_humber_l2_build_hw_entry(uint8 chip_id, sys_l2_node_t* l2_node,
                              sys_l2_fdb_build_hw_entry_t* p_bd_hw_entry)
{
    uint32 cmd = 0;
    int32 ret = 0;
    ds_mac_t macda;

    kal_memset(&macda, 0, sizeof(ds_mac_t));

    /*build_macda_entry */
    _sys_humber_l2_build_macda_entry(p_bd_hw_entry, &macda);

    if (!CTC_FLAG_ISSET(l2_node->flag, SYS_L2_NODE_FLAG_IS_ASIC_HASH))
    {
        tbl_entry_t  tcam_key;
        ds_mac_key_t mackey_data ;
        ds_mac_key_t mackey_mask;

        tcam_key.data_entry = (uint32 *)&mackey_data;
        tcam_key.mask_entry = (uint32 *)&mackey_mask;

        cmd = DRV_IOW(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
        ret = drv_tbl_ioctl(chip_id, l2_node->index, cmd, &macda);

        /*build  tcam  key */
        _sys_humber_l2_build_mackey_entry( p_bd_hw_entry, &tcam_key);
        cmd = DRV_IOW(IOC_TABLE, DS_MAC_KEY, DRV_ENTRY_FLAG);
        ret = ret?ret:drv_tbl_ioctl(chip_id, l2_node->index - pl2_master->tcam_base, cmd, &tcam_key);
    }
    else
    {
        cmd = DRV_IOW(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
        ret = drv_tbl_ioctl(chip_id,  l2_node->index, cmd, &macda);

        ret = ret?ret:_sys_humber_l2_write_hash_key(chip_id,l2_node);
    }

    /*set aging bit for dynamic fdb entry*/
    if (l2_node->flag & SYS_L2_NODE_FLAG_IS_STATIC)
    {
        CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, l2_node->index, FALSE));
    }
    else
    {
        CTC_ERROR_RETURN(sys_humber_aging_set_aging_status(chip_id, l2_node->index, TRUE));
    }

    return ret;
}


/**
 @brief add a fdb entry
*/
int32
sys_humber_l2_add_fdb(ctc_l2_addr_t* l2_addr)
{
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_nh_offset_array_t fwd_array;
    sys_l2_node_t* p_l2_node = NULL;
    uint8 chip_num = 0;
    uint8 chip_id = 0;
    int32 ret = 0;
    sys_l2_node_t l2_node_tmp;

    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2_addr);
    SYS_L2_FDB_FID_CHECK(l2_addr->fid);
    CTC_GLOBAL_PORT_CHECK(l2_addr->gport);

    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d flag:0x%x, gport:%d,  gsrcport_type:%d, virtual_port:%d\n",
                     l2_addr->mac[0], l2_addr->mac[1], l2_addr->mac[2],
                     l2_addr->mac[3], l2_addr->mac[4], l2_addr->mac[5],
                     l2_addr->fid, l2_addr->flag, l2_addr->gport,
                     l2_addr->gsrcport_type, l2_addr->logic_port);


    l2_node_tmp.key.fid = l2_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2_addr->mac, sizeof(mac_addr_t));


L2_LOCK;

    if (!CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_DISCARD) )
    {
        if( CTC_FLAG_ISSET(l2_addr->flag,CTC_L2_FLAG_RAW_PKT_ELOG_CPU))
        {
           ret = sys_humber_rawpkt_elog_cpu_get_dsfwd_offset(l2_addr->gport,  fwd_array);
        }
        else if( CTC_FLAG_ISSET(l2_addr->flag,CTC_L2_FLAG_SERVICE_QUEUE))
        {
           ret = sys_humber_srv_queue_get_dsfwd_offset(l2_addr->gport,  fwd_array);
        }
        else
        {
            ret =  sys_humber_brguc_get_dsfwd_offset(l2_addr->gport, CTC_FLAG_ISSET(l2_addr->flag,CTC_L2_FLAG_PORTUNTAGGED), fwd_array);
        }
    }
    else
    {
         l2_addr->gport = CTC_MAX_UINT16_VALUE;
    }

    if(CTC_E_NONE != ret)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    chip_num = sys_humber_get_local_chip_num();
    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);

    if (NULL != p_l2_node)
    {
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_MCAST_ENTRY_EXIST, pl2_master->l2_mutex);
        }

        if (!CTC_FLAG_ISSET(l2_addr->flag, CTC_L2_FLAG_IS_STATIC)
            && (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)))
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_EXIST, pl2_master->l2_mutex);
        }
        bd_hw_entry.rewrite = 1;

         CTC_UNSET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC);
         CTC_UNSET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN);
    }
    else
    {
        bd_hw_entry.rewrite = 0;
        p_l2_node = (sys_l2_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_node_t));
        kal_memset(p_l2_node, 0, sizeof(sys_l2_node_t));
    }

   if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_IS_STATIC))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC);
    }
    else  if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_REMOTE_DYNAMIC))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN);
    }

    if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_SYSTEM_RSV))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV);
    }

    bd_hw_entry.fdb_type  = SYS_L2_FDB_TYPE_NORMAL_FDB;


    kal_memcpy(& bd_hw_entry.l2_addr, l2_addr, sizeof(ctc_l2_addr_t));
    bd_hw_entry.src_gport = l2_addr->gport;
    bd_hw_entry.sw_hash_index= l2_node_tmp.index;

    if (0 == bd_hw_entry.rewrite)
    {
        p_l2_node->key.fid = l2_addr->fid;
        kal_memcpy(p_l2_node->key.mac, l2_addr->mac, sizeof(mac_addr_t));
        p_l2_node->gport = l2_addr->gport;

        ret = _sys_humber_l2_build_index( p_l2_node, &bd_hw_entry);
        if (CTC_E_NONE != ret)
        {
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        ret = _sys_humber_l2_fdb_add_to_fdb_port_list(p_l2_node);
        if (CTC_E_NONE != ret )
        {
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        ret = _sys_humber_l2_fdb_add_to_fdb_vlan_list(p_l2_node);
        if (CTC_E_NONE != ret)
        {
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        _sys_humber_l2_fdb_add_to_hash_table(p_l2_node);
        _sys_humber_l2_fdb_mac_entry_add_to_hash_table(p_l2_node);

    }
    else
    {
        if (l2_addr->flag & CTC_L2_FLAG_DISCARD)
        {
            _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
            p_l2_node->gport = CTC_MAX_UINT16_VALUE;
        }
        else  if (l2_addr->gport != p_l2_node->gport)
        {
            _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
            p_l2_node->gport = l2_addr->gport;
            _sys_humber_l2_fdb_add_to_fdb_port_list(p_l2_node);
        }

    }


    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        bd_hw_entry.fwd_ptr = fwd_array[chip_id];

        ret =  _sys_humber_l2_build_hw_entry(chip_id, p_l2_node, &bd_hw_entry);

        if (CTC_E_NONE != ret && (0 == bd_hw_entry.rewrite))
        {
            _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
            _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
            _sys_humber_l2_fdb_remove_from_hash_table(&l2_node_tmp);
            _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(&l2_node_tmp);
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        SYS_FDB_DBG_INFO("Add ucast fdb: chip_id:%d ds_mac_index:%x ds_fwd_offset:0x%x\n",
                         chip_id, p_l2_node->index, bd_hw_entry.fwd_ptr);
    }

    ctc_vector_add(pl2_master->fdb_tbl_vec, p_l2_node->index, p_l2_node);


L2_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_l2_remove_fdb(ctc_l2_addr_t* l2_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
    int32 ret = 0;
    uint8 chip_id = 0;
    uint8 chip_num = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2_addr);
    SYS_L2_FDB_FID_CHECK(l2_addr->fid);
    CTC_GLOBAL_PORT_CHECK(l2_addr->gport);
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d  \n",
                                    l2_addr->mac[0],l2_addr->mac[1],l2_addr->mac[2],
                                    l2_addr->mac[3],l2_addr->mac[4],l2_addr->mac[5],
                                    l2_addr->fid);


    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    l2_node_tmp.key.fid = l2_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

L2_LOCK;
    chip_num = sys_humber_get_local_chip_num();
    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    /* remove hw entry */
    for (chip_id = 0;chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM;chip_id++)
    {
        ret =  _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
        if(ret != CTC_E_NONE)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_HW_OP_FAIL, pl2_master->l2_mutex);
        }
    }

    /* remove sw entry */
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
    _sys_humber_l2_fdb_remove_from_hash_table(&l2_node_tmp);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(&l2_node_tmp);

    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec,p_l2_node->index);
    mem_free(p_l2_node);
L2_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_l2_remove_fdb_by_index(uint32 index)
{
    sys_l2_node_t* p_l2_node = NULL;
    int32 ret = 0;
    uint8 chip_id = 0;
    uint8 chip_num = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

      SYS_L2_FDB_INIT_CHECK();

L2_LOCK;
    chip_num = sys_humber_get_local_chip_num();
    p_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, index);

    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    /*remove hw entry*/
    for (chip_id = 0;chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM;chip_id++)
    {
        ret =  _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
        if (ret != CTC_E_NONE)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_HW_OP_FAIL, pl2_master->l2_mutex);
        }
    }

    /*remove sw entry*/
     _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
     _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
     _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
    ret = _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
    if (ret != CTC_E_NONE)
    {
        if (p_l2_node != NULL)
        {
            mem_free(p_l2_node);
        }
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    bd_hw_entry.fdb_type  = SYS_L2_FDB_TYPE_NORMAL_FDB;
    ret = _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    if (ret != CTC_E_NONE)
    {
        if(p_l2_node != NULL)
        {
            mem_free(p_l2_node);
        }
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }
    ctc_vector_del(pl2_master->fdb_tbl_vec,p_l2_node->index);
    mem_free(p_l2_node);
L2_UNLOCK;

    return CTC_E_NONE;
}
int32
sys_humber_l2_get_fdb_by_index(uint32 index, ctc_l2_addr_t* l2_addr)
{
    sys_l2_node_t* p_l2_node = NULL;

    SYS_L2_FDB_INIT_CHECK();

 L2_LOCK;

    p_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, index);

    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }
    l2_addr->fid = p_l2_node->key.fid;

    l2_addr->gport = p_l2_node->gport;

    if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        l2_addr->flag  = CTC_L2_FLAG_IS_STATIC;
    }
    kal_memcpy(l2_addr->mac, p_l2_node->key.mac, sizeof(mac_addr_t));

L2_UNLOCK;

    return CTC_E_NONE;


}
int32
sys_humber_l2_add_fdb_with_nexthop(ctc_l2_addr_t* l2_addr, uint32 nhp_id)
{
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_nh_offset_array_t fwd_array;
    sys_l2_node_t* p_l2_node = NULL;
    uint8 chip_num = 0;
    uint8 chip_id = 0;
    uint8  aps_brg_en = 0;
    uint16 dest_id = 0;
    int32 ret = 0;
    sys_l2_node_t l2_node_tmp;

    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2_addr);
    SYS_L2_FDB_FID_CHECK(l2_addr->fid);

    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d flag:0x%x, gport:%d,  gsrcport_type:%d, virtual_port:%d\n",
                     l2_addr->mac[0], l2_addr->mac[1], l2_addr->mac[2],
                     l2_addr->mac[3], l2_addr->mac[4], l2_addr->mac[5],
                     l2_addr->fid, l2_addr->flag, l2_addr->gport,
                     l2_addr->gsrcport_type, l2_addr->logic_port);



    l2_node_tmp.key.fid = l2_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

L2_LOCK;
    ret = sys_humber_nh_get_dsfwd_offset(nhp_id, fwd_array);
    if (CTC_E_NONE != ret)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL != p_l2_node)
    {
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_MCAST_ENTRY_EXIST, pl2_master->l2_mutex);
        }

        if (!CTC_FLAG_ISSET(l2_addr->flag, CTC_L2_FLAG_IS_STATIC)
            && CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_EXIST, pl2_master->l2_mutex);
        }
        bd_hw_entry.rewrite = 1;
         CTC_UNSET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC);
         CTC_UNSET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN);
    }
    else
    {
        bd_hw_entry.rewrite = 0;
        p_l2_node = (sys_l2_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_node_t));
        kal_memset(p_l2_node, 0, sizeof(sys_l2_node_t));
    }

    if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_IS_STATIC))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC);
    }
    else  if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_REMOTE_DYNAMIC))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN);
    }

    if (CTC_FLAG_ISSET(l2_addr->flag , CTC_L2_FLAG_SYSTEM_RSV))
    {
        CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV);
    }

    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    kal_memcpy(& bd_hw_entry.l2_addr, l2_addr, sizeof(ctc_l2_addr_t));
    bd_hw_entry.src_gport = l2_addr->gport;
    bd_hw_entry.sw_hash_index= l2_node_tmp.index;

    ret = sys_humber_nh_get_port(nhp_id, &aps_brg_en,&dest_id);
    if (ret != CTC_E_NONE)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    bd_hw_entry.l2_addr.gport  = dest_id;

    if (0 == bd_hw_entry.rewrite)
    {
        p_l2_node->key.fid = l2_addr->fid;
        kal_memcpy(p_l2_node->key.mac, l2_addr->mac, sizeof(mac_addr_t));
        p_l2_node->gport = bd_hw_entry.l2_addr.gport;

        ret = _sys_humber_l2_build_index(p_l2_node, &bd_hw_entry);
        if (CTC_E_NONE != ret )
        {
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        ret = _sys_humber_l2_fdb_add_to_fdb_port_list(p_l2_node);
        if (CTC_E_NONE != ret )
        {
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

       ret = _sys_humber_l2_fdb_add_to_fdb_vlan_list(p_l2_node);
       if (CTC_E_NONE != ret)
       {
           _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
           _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
           mem_free(p_l2_node);
           CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
       }
        _sys_humber_l2_fdb_add_to_hash_table(p_l2_node);
        _sys_humber_l2_fdb_mac_entry_add_to_hash_table(p_l2_node);
    }
    else
    {
         _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
           p_l2_node->gport = bd_hw_entry.l2_addr.gport;
           _sys_humber_l2_fdb_add_to_fdb_port_list(p_l2_node);

    }

    chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        bd_hw_entry.fwd_ptr = fwd_array[chip_id];

        ret = _sys_humber_l2_build_hw_entry( chip_id, p_l2_node, &bd_hw_entry);
        if (CTC_E_NONE != ret && (0 == bd_hw_entry.rewrite))
        {
              _sys_humber_l2_fdb_add_to_fdb_port_list(p_l2_node);
             _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
             _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
             _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
             _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
             mem_free(p_l2_node);
             CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        SYS_FDB_DBG_INFO("Add ucast fdb: chip_id:%d ds_mac_index:%x ds_fwd_offset:0x%x\n",
                         chip_id, p_l2_node->index, bd_hw_entry.fwd_ptr);
    }
    ctc_vector_add(pl2_master->fdb_tbl_vec, p_l2_node->index, p_l2_node);
L2_UNLOCK;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2_get_fdb_entry_by_port(uint16 gport,ctc_l2_fdb_query_flag_t query_flag, ctc_l2_fdb_query_rst_t* query_rst)
{
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_fdb_port_node_t  *port_fdb_mode = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    ctc_listnode_t* listnode = NULL;
    uint32 index  = 0;
    uint32 count_index  = 0;
    uint32 max_count = query_rst->buffer_len / sizeof(ctc_l2_addr_t);

    query_rst->is_end = 0;
    port_fdb_mode = ctc_vector_get(pl2_master->gport_vec, gport);
    if(NULL == port_fdb_mode || NULL == port_fdb_mode->port_fdb_list)
    {
        query_rst->is_end = 1;
        return count_index;
    }
    fdb_list =port_fdb_mode->port_fdb_list;

    listnode = CTC_LISTHEAD(fdb_list);
    while (listnode!= NULL &&index < query_rst->start_index)
    {
        CTC_NEXTNODE(listnode);
        index++;
    }

    for (index = query_rst->start_index; \
         listnode && count_index < max_count;
         CTC_NEXTNODE(listnode ), index++)
    {
        p_l2_node = listnode->data;
          switch(query_flag)
         {
          case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
              if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
             if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_ALL:
          default:
             break;
         }
        (query_rst->buffer[count_index]).fid = p_l2_node->key.fid;
        (query_rst->buffer[count_index]).gport= p_l2_node->gport;

        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            query_rst->buffer[count_index].flag  = CTC_L2_FLAG_IS_STATIC;
        }
        kal_memcpy((query_rst->buffer[count_index]).mac, p_l2_node->key.mac, sizeof(mac_addr_t));
        count_index++;
    }

    query_rst->next_query_index = index;

    if (listnode == NULL)
    {
        query_rst->is_end = 1;
    }
    return count_index;
}

static uint32
_sys_humber_l2_get_fdb_entry_by_vlan(uint16 fid,ctc_l2_fdb_query_flag_t query_flag, ctc_l2_fdb_query_rst_t* query_rst)
{
    ctc_listnode_t* listnode = NULL;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    uint32 count_index = 0;
    uint32 index = 0;
    uint32 max_count = query_rst->buffer_len / sizeof(ctc_l2_addr_t);

    query_rst->is_end = FALSE;
    if (fid > pl2_master->max_fid_value)
    {
        query_rst->is_end = TRUE;
        return count_index;
    }

    L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list);

    if (NULL == fdb_list)
    {
        query_rst->is_end = TRUE;
        return count_index;
    }

    listnode = CTC_LISTHEAD(fdb_list);
    while (listnode != NULL && index < query_rst->start_index)
    {
        CTC_NEXTNODE(listnode);
        index++;
    }

    for (index = query_rst->start_index; \
         listnode && count_index < max_count;
         CTC_NEXTNODE(listnode), index++)
    {
        p_l2_node = listnode->data;
         switch(query_flag)
         {
          case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
              if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
             if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_ALL:
          default:
             break;
         }
        (query_rst->buffer[count_index]).fid = p_l2_node->key.fid;
        (query_rst->buffer[count_index]).gport= p_l2_node->gport;

        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            query_rst->buffer[count_index].flag  = CTC_L2_FLAG_IS_STATIC;
        }
        kal_memcpy((query_rst->buffer[count_index]).mac, p_l2_node->key.mac, sizeof(mac_addr_t));
        count_index++;
    }



    query_rst->next_query_index = index;

    if (listnode == NULL)
    {
        query_rst->is_end = 1;
    }

    return count_index;
}

static int32
_sys_humber_l2_get_fdb_entry_by_port_vlan(uint16 gport,uint16 fid,ctc_l2_fdb_query_flag_t query_flag, ctc_l2_fdb_query_rst_t* query_rst)
{
    ctc_listnode_t* listnode = NULL;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    uint32 count_index = 0;
    uint32 index = 0;
    uint32 max_count = query_rst->buffer_len / sizeof(ctc_l2_addr_t);
    CTC_FID_RANGE_CHECK(fid);

    L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list);

    if (NULL == fdb_list)
    {
        query_rst->is_end = TRUE;
        return count_index;
    }

    listnode = CTC_LISTHEAD(fdb_list);
    while (listnode != NULL && index < query_rst->start_index)
    {
        CTC_NEXTNODE(listnode);
        index++;
    }

      for (index = query_rst->start_index; \
         listnode && count_index < max_count;
         CTC_NEXTNODE(listnode), index++)
    {
        p_l2_node = listnode->data;

        if(p_l2_node->gport != gport)
         {
           continue;
         }
        switch(query_flag)
         {
          case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
              if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
             if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_ALL:
          default:
             break;
         }

        (query_rst->buffer[count_index]).fid = p_l2_node->key.fid;
        (query_rst->buffer[count_index]).gport= p_l2_node->gport;

        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            query_rst->buffer[count_index].flag  = CTC_L2_FLAG_IS_STATIC;
        }
        kal_memcpy((query_rst->buffer[count_index]).mac, p_l2_node->key.mac, sizeof(mac_addr_t));
        count_index++;
    }

    query_rst->next_query_index = index;

    if (listnode == NULL)
    {
        query_rst->is_end = 1;
    }

    return count_index;
}


static int32
_sys_humber_l2_get_mac_entry(sys_l2_node_t *p_l2_node, sys_l2_fdb_mac_hash_info_t *mac_hash_info)
{
    uint32 max_index = 0;
    uint32 start_index = 0;
    uint32 index = 0;

    if (NULL == p_l2_node || NULL == mac_hash_info)
    {
        return 0;
    }

    start_index = mac_hash_info->query_rst->start_index;
    max_index = mac_hash_info->query_rst->buffer_len / sizeof(ctc_l2_addr_t);

    if (mac_hash_info->count_index >= max_index)
    {
        return -1;
    }

    if (kal_memcmp (mac_hash_info->mac, p_l2_node->key.mac, CTC_ETH_ADDR_LEN))
    {
        return 0;
    }

    if (mac_hash_info->query_rst->next_query_index++ < start_index)
    {
        return 0;
    }

    switch(mac_hash_info->query_flag)
    {
    case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
        if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
        ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case CTC_L2_FDB_ENTRY_ALL:
        default:
        break;
    }

    index = mac_hash_info->count_index;

    (mac_hash_info->query_rst->buffer[index]).fid = p_l2_node->key.fid;
    (mac_hash_info->query_rst->buffer[index]).gport= p_l2_node->gport;

    if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
          mac_hash_info->query_rst->buffer[index].flag  = CTC_L2_FLAG_IS_STATIC;
    }
    kal_memcpy((mac_hash_info->query_rst->buffer[index]).mac, p_l2_node->key.mac, sizeof(mac_addr_t));
    mac_hash_info->count_index++;

    return 0;
}

static int32
_sys_humber_l2_get_fdb_entry_by_mac(mac_addr_t mac, ctc_l2_fdb_query_flag_t query_flag, ctc_l2_fdb_query_rst_t* query_rst)
{
     sys_l2_fdb_mac_hash_info_t mac_hash_info;
     int32 ret = 0;
     hash_traversal_fn fun = NULL;

     kal_memset(&mac_hash_info, 0, sizeof(sys_l2_fdb_mac_hash_info_t));
     kal_memcpy(mac_hash_info.mac, mac, sizeof(mac_addr_t));
     mac_hash_info.query_flag = query_flag;
     mac_hash_info.query_rst = query_rst;
     query_rst->next_query_index = 0;

     fun = (hash_traversal_fn) _sys_humber_l2_get_mac_entry;

     ret = ctc_hash_traverse2(pl2_master->fdb_mac_hash, fun, &mac_hash_info);
     if (ret > 0)
     {
          query_rst->is_end = 1;
     }

    return mac_hash_info.count_index;
}


static int32
_sys_humber_l2_get_fdb_entry_by_mac_vlan(mac_addr_t mac,uint16 fid,ctc_l2_fdb_query_flag_t query_flag,ctc_l2_fdb_query_rst_t* query_rst)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
    uint32 max_count = query_rst->buffer_len / sizeof(ctc_l2_addr_t);

    SYS_L2_FDB_FID_CHECK(fid);

    if(max_count > 1)
    {
       return 0;
    }

    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    l2_node_tmp.key.fid =fid;
    kal_memcpy(l2_node_tmp.key.mac, mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if(NULL == p_l2_node)
     {
        return 0;
    }

    switch (query_flag)
    {
   case  CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
       if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_DYNAMIC:           /**<  all dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:           /**<  all local chip's dynamic fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_ALL:                  /**<  all fdb record */
    default:
    break;
     }

    query_rst->buffer[0].fid = p_l2_node->key.fid;
    query_rst->buffer[0].gport= p_l2_node->gport;

    if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        query_rst->buffer[0].flag  = CTC_L2_FLAG_IS_STATIC;
    }
    kal_memcpy(query_rst->buffer[0].mac, p_l2_node->key.mac, sizeof(mac_addr_t));
    query_rst->is_end  = TRUE;

   return 1;

}

static int32
_sys_humber_l2_get_fdb_entry_by_all(ctc_l2_fdb_query_flag_t query_flag, ctc_l2_fdb_query_rst_t* query_rst)
{
    uint32 block_index = 0;
    uint32 index = 0;
    uint32 valid_index = 0;
    uint32 vec_index = 0;
    uint32 max_valid_size = 0;
    sys_l2_node_t* p_l2_node = NULL;
    uint32 count_index  = 0;
    uint32 max_count = query_rst->buffer_len / sizeof(ctc_l2_addr_t);

    query_rst->is_end = FALSE;
    block_index = query_rst->start_index / pl2_master->fdb_tbl_vec->block_size;
    index = query_rst->start_index % pl2_master->fdb_tbl_vec->block_size;

    for ( ; count_index < max_count && block_index < pl2_master->fdb_tbl_vec->max_block_num;
            block_index++, index = 0)
    {
        valid_index = 0;
        max_valid_size = ctc_vector_get_size(pl2_master->fdb_tbl_vec, block_index);
        if (max_valid_size == 0)
        {
            continue;
        }

        vec_index = block_index * pl2_master->fdb_tbl_vec->block_size;
        for ( ; index < pl2_master->fdb_tbl_vec->block_size
                  && count_index < max_count; index++)
        {
            p_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, vec_index + index);
            if (NULL == p_l2_node)
            {
                continue;
            }
            switch(query_flag)
             {
              case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
                  if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
                 if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
                  if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
                   ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_ALL:
              default:
                 break;

             }
            (query_rst->buffer[count_index]).fid = p_l2_node->key.fid;
            (query_rst->buffer[count_index]).gport= p_l2_node->gport;
            kal_memcpy((query_rst->buffer[count_index]).mac, p_l2_node->key.mac, sizeof(mac_addr_t));
            if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                query_rst->buffer[count_index].flag  = CTC_L2_FLAG_IS_STATIC;
            }
            count_index++;
            valid_index++;

            query_rst->next_query_index = vec_index + index + 1;

            if (valid_index  == max_valid_size )
            {
                break;
            }
        }
    }

    if ((valid_index == max_valid_size)
        && (block_index == pl2_master->fdb_tbl_vec->max_block_num))
    {
        query_rst->is_end  = TRUE;
    }

    return count_index;
}

/**
 @brief Query fdb enery according to specified query condition

*/
int32
sys_humber_l2_get_fdb_entry(ctc_l2_fdb_query_t* pQuery,
                            ctc_l2_fdb_query_rst_t* query_rst)

{
    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(pQuery);

    pQuery->count = 0;

L2_LOCK;
    switch (pQuery->query_type)
    {
        case CTC_L2_FDB_ENTRY_OP_BY_VID:
            pQuery->count = _sys_humber_l2_get_fdb_entry_by_vlan(pQuery->fid,pQuery->query_flag, query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;

        case CTC_L2_FDB_ENTRY_OP_BY_PORT:
            pQuery->count = _sys_humber_l2_get_fdb_entry_by_port(pQuery->gport,pQuery->query_flag, query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;
      case CTC_L2_FDB_ENTRY_OP_BY_PORT_VLAN:
            pQuery->count = _sys_humber_l2_get_fdb_entry_by_port_vlan(pQuery->gport,pQuery->fid,pQuery->query_flag, query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;
      case CTC_L2_FDB_ENTRY_OP_BY_MAC_VLAN:
            pQuery->count = _sys_humber_l2_get_fdb_entry_by_mac_vlan(pQuery->mac,pQuery->fid,pQuery->query_flag, query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;
        case CTC_L2_FDB_ENTRY_OP_BY_MAC:
            pQuery->count  = _sys_humber_l2_get_fdb_entry_by_mac(pQuery->mac,pQuery->query_flag,query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;

        case CTC_L2_FDB_ENTRY_OP_ALL:
            pQuery->count = _sys_humber_l2_get_fdb_entry_by_all(pQuery->query_flag,query_rst);
            if (0 == pQuery->count)
            {
                query_rst->is_end = TRUE;
            }
            break;

        default:
            pQuery->count = 0;
            query_rst->is_end = TRUE;
            break;
    }
L2_UNLOCK;

  return CTC_E_NONE;
}


/**
 * get some type fdb count by specified port,not include the default entry num
*/
static uint32
_sys_humber_l2_get_fdb_count_by_port(ctc_l2_fdb_query_flag_t query_flag,uint16 gport)
{
    uint32 count = 0;
    uint32 hash_cout =0;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    ctc_listnode_t *listnode = NULL;
    sys_l2_fdb_port_node_t *port_fdb_mode = NULL;

    /* Check gport Id */
    CTC_GLOBAL_PORT_CHECK(gport);

    port_fdb_mode = ctc_vector_get(pl2_master->gport_vec, gport);
    if(NULL == port_fdb_mode || NULL == port_fdb_mode->port_fdb_list)
    {
       return count;
    }
    fdb_list = port_fdb_mode->port_fdb_list;
    switch(query_flag)
     {
      case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
         CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
        {
            if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                continue ;
            }
           count++;
        }

         break;
      case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
         count = port_fdb_mode->dynmac_count;

         break;
      case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
         count = port_fdb_mode->local_dynmac_count;

         break;
      case CTC_L2_FDB_ENTRY_ALL:
      CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
      {
          if (NULL == p_l2_node)
          {
              kal_printf("    %-15s",  "null entry");

          }
          else if(p_l2_node->flag & SYS_L2_NODE_FLAG_IS_ASIC_HASH)
          {
              kal_printf("   %.2x%.2x:%.2x%.2x:%.2x%.2x",  p_l2_node->key.mac[0], p_l2_node->key.mac[1], p_l2_node->key.mac[2], p_l2_node->key.mac[3], p_l2_node->key.mac[4],
                         p_l2_node->key.mac[5]);
              count++;
              hash_cout++;
          }
          else
          {

              kal_printf("   tcam %.2x%.2x:%.2x%.2x:%.2x%.2x",  p_l2_node->key.mac[0], p_l2_node->key.mac[1], p_l2_node->key.mac[2], p_l2_node->key.mac[3], p_l2_node->key.mac[4],
                         p_l2_node->key.mac[5]);
              count++;
          }
      }
        kal_printf("  cout:%d hash_cout:%d   \n",count,hash_cout);
       //  count =  CTC_LISTCOUNT(fdb_list);
       break;
      default:

         break;
     }

    return count;

}

/**
 * get some type fdb count by specified vid,not include the default entry num
*/
static uint32
_sys_humber_l2_get_fdb_count_by_vlan(ctc_l2_fdb_query_flag_t query_flag,uint16 fid)
{
    uint32 count = 0;
    sys_l2_fdb_vlan_node_t* fid_node = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    ctc_listnode_t *listnode = NULL;

    /* Check fid */
    CTC_FID_RANGE_CHECK(fid);

    if (fid > pl2_master->max_fid_value)
    {
        return 0;
    }

    L2_FDB_GET_FID_NODE_BY_FID(fid, fid_node);
    if (NULL == fid_node || NULL == fid_node->vlan_fdb_list)
    {
        return 0;
    }

    switch(query_flag)
     {
      case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
         CTC_LIST_LOOP(fid_node->vlan_fdb_list, p_l2_node, listnode)
        {
            if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                continue ;
            }
           count++;
        }
        break;
      case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
          count = fid_node->dynmac_count;

         break;
      case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
          count = fid_node->local_dynmac_count;

         break;
      case CTC_L2_FDB_ENTRY_ALL:
         count =  CTC_LISTCOUNT(fid_node->vlan_fdb_list);
      default:

         break;
     }

    return count;
}

static int32
_sys_humber_l2_get_fdb_count_by_port_vlan(ctc_l2_fdb_query_flag_t query_flag,uint16 gport,uint16 fid)
{
    sys_l2_node_t* p_l2_node = NULL;
    ctc_linklist_t* fdb_list = NULL;
    ctc_listnode_t *listnode = NULL;
    uint32 count = 0;

    /* Check gport ID and fid */
    CTC_FID_RANGE_CHECK(fid);
    CTC_GLOBAL_PORT_CHECK(gport);

    if (fid > pl2_master->max_fid_value)
    {
        return 0;
    }

    L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list);
    if (NULL == fdb_list)
    {
        return 0;
    }

     switch(query_flag)
     {
      case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
         CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
        {
            if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                continue ;
            }
           if(p_l2_node->gport != gport)
           {
             continue ;
           }
           count++;
        }

         break;
      case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
         CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
        {
            if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                continue ;
            }
            if(p_l2_node->gport != gport)
           {
             continue ;
           }
           count++;
        }
         break;
      case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
         CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
        {
            if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
                && !CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
            {
                continue ;
            }
            if(p_l2_node->gport != gport)
           {
             continue ;
           }
           count++;
        }
         break;
      case CTC_L2_FDB_ENTRY_ALL:
        CTC_LIST_LOOP(fdb_list, p_l2_node, listnode)
        {
            if(p_l2_node->gport != gport)
           {
             continue ;
           }
           count++;
        }
        break;
      default:

         break;
     }

    return count;
}


static int32
_sys_humber_l2_get_mac_count(sys_l2_node_t *p_l2_node, sys_l2_fdb_mac_hash_info_t *mac_hash_info)
{

    if (NULL == p_l2_node || NULL == mac_hash_info)
    {
        return 0;
    }

    if (kal_memcmp (mac_hash_info->mac, p_l2_node->key.mac, CTC_ETH_ADDR_LEN))
    {
        return 0;
    }

    switch(mac_hash_info->query_flag)
    {
    case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
        if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
        ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return 0;
        }
        break;
    case CTC_L2_FDB_ENTRY_ALL:
        default:
        break;
    }


    mac_hash_info->count_index++;

    return 0;
}


static int32
_sys_humber_l2_get_fdb_count_by_mac(ctc_l2_fdb_query_flag_t query_flag,mac_addr_t mac)
{
     sys_l2_fdb_mac_hash_info_t mac_hash_info;
     hash_traversal_fn fun = NULL;

     kal_memset(&mac_hash_info, 0, sizeof(sys_l2_fdb_mac_hash_info_t));
     kal_memcpy(mac_hash_info.mac, mac, sizeof(mac_addr_t));
     mac_hash_info.query_flag = query_flag;

     fun = (hash_traversal_fn) _sys_humber_l2_get_mac_count;

     ctc_hash_traverse2(pl2_master->fdb_mac_hash, fun, &mac_hash_info);

     return mac_hash_info.count_index;
}

static int32
_sys_humber_l2_get_fdb_count_by_mac_vlan(ctc_l2_fdb_query_flag_t query_flag,mac_addr_t mac,uint16 fid)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;

    SYS_L2_FDB_INIT_CHECK();
    SYS_L2_FDB_FID_CHECK(fid);


    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    l2_node_tmp.key.fid =fid;
    kal_memcpy(l2_node_tmp.key.mac, mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if(NULL == p_l2_node)
     {
        return 0;
    }

    switch (query_flag)
    {
   case  CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
       if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_DYNAMIC:           /**<  all dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:           /**<  all local chip's dynamic fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
        {
           return 0;
        }
         break;
   case CTC_L2_FDB_ENTRY_ALL:                  /**<  all fdb record */
    default:
        break;
     }

   return 1;

}

/**
@brief only count unicast FDB entries
*/
static uint32
_sys_humber_l2_get_all_fdb_count(ctc_l2_fdb_query_flag_t query_flag)
{
    uint32 max_valid_size = 0;
    uint32 count = 0;
    uint32 block_index = 0;
    uint32 val_index = 0;
    sys_l2_node_t* p_l2_node = NULL;
    uint32 vec_index = 0;

    switch(query_flag)
    {
    case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
       count = pl2_master->dynmac_count;
       return count;
    case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
        count = pl2_master->local_dynmac_count;
        return count;
    case CTC_L2_FDB_ENTRY_ALL:
    default:
       break;
    }

    for (block_index = 0; block_index < pl2_master->fdb_tbl_vec->max_block_num ; block_index++)
    {
        max_valid_size = ctc_vector_get_size(pl2_master->fdb_tbl_vec, block_index);
        if (max_valid_size == 0)
        {
            continue;
        }

        vec_index = block_index * pl2_master->fdb_tbl_vec->block_size;
        for (val_index = 0; val_index < pl2_master->fdb_tbl_vec->block_size; val_index++)
        {
            p_l2_node = ctc_vector_get(pl2_master->fdb_tbl_vec, vec_index + val_index);

            if (NULL == p_l2_node)
            {
                continue;
            }
            switch(query_flag)
             {
              case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
                  if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
                 if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
                  if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
                   ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_ALL:
              default:
                 break;

             }

             count++;


        }
    }

    return count;
}


/**
   * get some type fdb count, not include the default entry num
*/
int32
sys_humber_l2_get_fdb_count(ctc_l2_fdb_query_t* pQuery)
{

     SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(pQuery);

    pQuery->count = 0;

L2_LOCK;
    switch (pQuery->query_type)
    {
      case CTC_L2_FDB_ENTRY_OP_BY_VID:
          pQuery->count = _sys_humber_l2_get_fdb_count_by_vlan(pQuery->query_flag,pQuery->fid);
          break;

      case CTC_L2_FDB_ENTRY_OP_BY_PORT:
          pQuery->count = _sys_humber_l2_get_fdb_count_by_port(pQuery->query_flag,pQuery->gport);
          break;
      case CTC_L2_FDB_ENTRY_OP_BY_PORT_VLAN:
          pQuery->count = _sys_humber_l2_get_fdb_count_by_port_vlan(pQuery->query_flag,pQuery->gport,pQuery->fid);
          break;
     case CTC_L2_FDB_ENTRY_OP_BY_MAC_VLAN:
          pQuery->count =  _sys_humber_l2_get_fdb_count_by_mac_vlan(pQuery->query_flag,pQuery->mac,pQuery->fid);
          break;
      case CTC_L2_FDB_ENTRY_OP_BY_MAC:
          pQuery->count = _sys_humber_l2_get_fdb_count_by_mac(pQuery->query_flag,pQuery->mac);
          break;

      case CTC_L2_FDB_ENTRY_OP_ALL:
          pQuery->count = _sys_humber_l2_get_all_fdb_count(pQuery->query_flag);
          break;

      default:
          pQuery->count = 0;
          break;
    }
L2_UNLOCK;

    return CTC_E_NONE;
}


static bool
_sys_humber_l2_flush_static_fdb(sys_l2_node_t* p_fdb_node, void* p_flush_info)
{
    uint8 lchip = 0;
    int32 ret = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_l2_node_t* p_node = NULL;
    sys_l2_fdb_flush_info_t *p_static_flush_info = NULL;

    CTC_PTR_VALID_CHECK(p_fdb_node);
    CTC_PTR_VALID_CHECK(p_flush_info);

    p_node = p_fdb_node;

    p_static_flush_info = (sys_l2_fdb_flush_info_t *)p_flush_info;

    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
    {
        return FALSE;
    }

    if (!CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        return FALSE;
    }

    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        return FALSE;
    }

    if (0 == p_static_flush_info->flush_fdb_cnt_per_loop )
    {
        return FALSE;
    }

    /* 1)delete hw entry */
    for (lchip = 0; (lchip < p_static_flush_info->chip_num) && (lchip < MAX_LOCAL_CHIP_NUM); lchip++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(lchip, p_node);
        if (ret != CTC_E_NONE)
        {
            return FALSE;
        }
    }

    /* 2)delete sw entry */
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec, p_node->index);
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_node);
    mem_free(p_node);

    if ( (p_static_flush_info->flush_fdb_cnt_per_loop) > 0 )
    {
        --p_static_flush_info->flush_fdb_cnt_per_loop;
    }

    return TRUE;
}


static bool
_sys_humber_l2_flush_dynimic_fdb(sys_l2_node_t* p_fdb_node, void* p_flush_info)
{
    uint8 lchip = 0;
    int32 ret = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_l2_node_t* p_node = NULL;
    sys_l2_fdb_flush_info_t *p_static_flush_info = NULL;

    CTC_PTR_VALID_CHECK(p_fdb_node);
    CTC_PTR_VALID_CHECK(p_flush_info);

    p_node = p_fdb_node;

    p_static_flush_info = (sys_l2_fdb_flush_info_t *)p_flush_info;

    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
    {
        return FALSE;
    }

    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        return FALSE;
    }
     if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        return FALSE;
    }

    if (0 == p_static_flush_info->flush_fdb_cnt_per_loop )
    {
        return FALSE;
    }

    /* 1)delete hw entry */
    for (lchip = 0; (lchip < p_static_flush_info->chip_num) && (lchip < MAX_LOCAL_CHIP_NUM); lchip++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(lchip, p_node);
        if (ret != CTC_E_NONE)
        {
            return FALSE;
        }
    }

    /* 2)delete sw entry */
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec, p_node->index);
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_node);
    mem_free(p_node);

    if ( (p_static_flush_info->flush_fdb_cnt_per_loop) > 0 )
    {
        --p_static_flush_info->flush_fdb_cnt_per_loop;
    }

    return TRUE;
}
static bool
_sys_humber_l2_flush_local_dynimic_fdb(sys_l2_node_t* p_fdb_node, void* p_flush_info)
{
    uint8 lchip = 0;
    int32 ret = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_l2_node_t* p_node = NULL;
    sys_l2_fdb_flush_info_t *p_static_flush_info = NULL;

    CTC_PTR_VALID_CHECK(p_fdb_node);
    CTC_PTR_VALID_CHECK(p_flush_info);

    p_node = p_fdb_node;

    p_static_flush_info = (sys_l2_fdb_flush_info_t *)p_flush_info;


    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
    {
        return FALSE;
    }

    if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        return FALSE;
    }
     if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
    {
        return FALSE;
    }
     if (CTC_FLAG_ISSET(p_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        return FALSE;
    }

    if (0 == p_static_flush_info->flush_fdb_cnt_per_loop )
    {
        return FALSE;
    }

    /* 1)delete hw entry */
    for (lchip = 0; (lchip < p_static_flush_info->chip_num) && (lchip < MAX_LOCAL_CHIP_NUM); lchip++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(lchip, p_node);
        if (ret != CTC_E_NONE)
        {
            return FALSE;
        }
    }

    /* 2)delete sw entry */
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec, p_node->index);
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_node);
    mem_free(p_node);

    if ( (p_static_flush_info->flush_fdb_cnt_per_loop) > 0 )
    {
        --p_static_flush_info->flush_fdb_cnt_per_loop;
    }

    return TRUE;
}

static bool
_sys_humber_l2_flush_all_fdb(sys_l2_node_t* p_fdb_node, void* p_flush_info)
{
    uint8 lchip = 0;
    int32 ret = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_l2_fdb_flush_info_t *p_static_flush_info = NULL;

    CTC_PTR_VALID_CHECK(p_fdb_node);
    CTC_PTR_VALID_CHECK(p_flush_info);

    p_static_flush_info = (sys_l2_fdb_flush_info_t *)p_flush_info;


    if (CTC_FLAG_ISSET(p_fdb_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
    {
        return FALSE;
    }

    if (CTC_FLAG_ISSET(p_fdb_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        return FALSE;
    }

    if (0 == p_static_flush_info->flush_fdb_cnt_per_loop )
    {
        return FALSE;
    }

    /* 1)delete hw entry */
    for (lchip = 0; (lchip < p_static_flush_info->chip_num) && (lchip < MAX_LOCAL_CHIP_NUM); lchip++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(lchip, p_fdb_node);
        if (ret != CTC_E_NONE)
        {
            return FALSE;
        }
    }

    /* 2)delete sw entry */
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_fdb_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec, p_fdb_node->index);
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_fdb_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_fdb_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_fdb_node);
    mem_free(p_fdb_node);

    if ( (p_static_flush_info->flush_fdb_cnt_per_loop) > 0 )
    {
        --p_static_flush_info->flush_fdb_cnt_per_loop;
    }

    return TRUE;
}

static int32
_sys_humber_l2_fdb_flush_all(uint8 chip_num, uint8 flush_flag)
{
    sys_l2_fdb_flush_info_t flush_info;
    hash_traversal_fn fun;

    flush_info.flush_fdb_cnt_per_loop
        = pl2_master->flush_fdb_cnt_per_loop ? pl2_master->flush_fdb_cnt_per_loop : CTC_MAX_UINT32_VALUE;
    flush_info.chip_num = chip_num;

    switch (flush_flag)
    {
        case CTC_L2_FDB_ENTRY_STATIC:
            fun = (hash_traversal_fn) _sys_humber_l2_flush_static_fdb;
            break;

        case CTC_L2_FDB_ENTRY_DYNAMIC:
            fun = (hash_traversal_fn) _sys_humber_l2_flush_dynimic_fdb;
            break;

        case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:
            fun = (hash_traversal_fn) _sys_humber_l2_flush_local_dynimic_fdb;
            break;

        case CTC_L2_FDB_ENTRY_ALL:
            fun = (hash_traversal_fn) _sys_humber_l2_flush_all_fdb;
            break;

        default:
            return CTC_E_NONE;
    }

    ctc_hash_traverse_remove(pl2_master->fdb_hash, fun, &flush_info);

    if ( 0 ==  pl2_master->flush_fdb_cnt_per_loop || flush_info.flush_fdb_cnt_per_loop > 0 )
    {
        return CTC_E_NONE;
    }
    return CTC_E_OPERATION_PAUSE;
}

/**
 @brief flush fdb entry by port
*/
static int32
_sys_humber_l2_fdb_flush_by_port(uint8 chip_num, uint16 gport, uint32 flush_flag)
{
    ctc_listnode_t* next_node = NULL;
    ctc_listnode_t* node = NULL;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_fdb_port_node_t  *port_fdb_node = NULL;
    int32 ret = 0;
    uint8 chip_id = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

    /* Check gport Id */
    CTC_GLOBAL_PORT_CHECK(gport);

    uint32 flush_fdb_cnt_per_loop;
    SYS_L2_FDB_FLUSH_COUNT_SET(flush_fdb_cnt_per_loop);

    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    port_fdb_node = ctc_vector_get(pl2_master->gport_vec, gport);
    fdb_list = port_fdb_node?port_fdb_node->port_fdb_list:NULL;


    CTC_LIST_LOOP_DEL(fdb_list, p_l2_node, node, next_node)
    {
        switch(flush_flag)
         {
          case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
              if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
                ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
             if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
                ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_ALL:
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
          default:
             break;

         }

            /* 1)delete hw entry  */
            for (chip_id = 0;chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM;chip_id++)
            {
                ret = _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
                if (ret != CTC_E_NONE)
                {
                    return ret;
                }
            }

            /* 2)delete sw entry  */
            ctc_vector_del( pl2_master->fdb_tbl_vec,p_l2_node->index);
            _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
            _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
           _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
           _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
            ret = _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
            if(ret != CTC_E_NONE)
            {
                break;
            }

            mem_free(p_l2_node);
            p_l2_node = NULL;

            SYS_L2_FDB_FLUSH_COUNT_PROCESS(flush_fdb_cnt_per_loop);
        }

   return ret;
}


/**
 @brief flush fdb entry by port+vlan
*/
static int32
_sys_humber_l2_fdb_flush_by_port_vlan(uint8 chip_num, uint16 gport,uint16 fid, uint32 flush_flag)
{
    ctc_listnode_t* next_node = NULL;
    ctc_listnode_t* node = NULL;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    int32 ret = 0;
    uint8 chip_id = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

    uint32 flush_fdb_cnt_per_loop;
    SYS_L2_FDB_FLUSH_COUNT_SET(flush_fdb_cnt_per_loop);

    /* Check fid and gport Id */
    CTC_FID_RANGE_CHECK(fid);
    CTC_GLOBAL_PORT_CHECK(gport);

    L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list);

    CTC_LIST_LOOP_DEL(fdb_list, p_l2_node, node, next_node)
    {

            switch(flush_flag)
             {
              case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
                  if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
                    ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
                 {
                    continue;
                 }
                 break;
              case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
                 if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
                    ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
                  if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
                   ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
                   ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
                 {
                    continue;
                 }
                 break;
              case CTC_L2_FDB_ENTRY_ALL:
                  if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
                 {
                    continue;
                 }
              default:
                 break;

             }


                 if(p_l2_node->gport != gport)
                 {
                    continue;
                 }

                /* 1)delete hw entry  */
                for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
                {
                    ret = _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
                    if (ret != CTC_E_NONE)
                    {
                        return ret;
                    }
                }

                /* 2)delete sw entry  */
                bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
                _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
                ctc_vector_del( pl2_master->fdb_tbl_vec, p_l2_node->index);
                _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
                _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
                _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
                ret = _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
                if (ret != CTC_E_NONE)
                {
                    break;
                }

                mem_free(p_l2_node);
                p_l2_node = NULL;

                SYS_L2_FDB_FLUSH_COUNT_PROCESS(flush_fdb_cnt_per_loop);
     }

   return ret;
}

/**
 @brief flush fdb entry by mac+vlan
*/
static int32
_sys_humber_l2_fdb_flush_by_mac_vlan(uint8 chip_num,mac_addr_t mac,uint16 fid, uint32 flush_flag)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
     uint8 chip_id = 0;
    int32 ret = 0;

    SYS_L2_FDB_INIT_CHECK();
    SYS_L2_FDB_FID_CHECK(fid);


    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    l2_node_tmp.key.fid =fid;
    kal_memcpy(l2_node_tmp.key.mac, mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL == p_l2_node )
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    switch (flush_flag)
    {
   case  CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
       if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return CTC_E_ENTRY_NOT_EXIST;
        }
         break;
   case CTC_L2_FDB_ENTRY_DYNAMIC:           /**<  all dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
           return CTC_E_ENTRY_NOT_EXIST;
        }
         break;
   case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:           /**<  all local chip's dynamic fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN))
        {
           return CTC_E_ENTRY_NOT_EXIST;
        }
         break;
    case CTC_L2_FDB_ENTRY_ALL:                  /**<  all fdb record */
    default:
        break;
     }

    if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }
       chip_num = sys_humber_get_local_chip_num();
    /* remove hw entry */
    for (chip_id = 0;chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM;chip_id++)
    {
        ret =  _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
        if(ret != CTC_E_NONE)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_HW_OP_FAIL, pl2_master->l2_mutex);
        }
    }

    /* remove sw entry */
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(&l2_node_tmp);
    _sys_humber_l2_fdb_remove_from_hash_table(&l2_node_tmp);

    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    ctc_vector_del(pl2_master->fdb_tbl_vec,p_l2_node->index);
    mem_free(p_l2_node);

     return CTC_E_NONE;

}

/**
 @brief flush fdb entry by vlan
*/
static int32
_sys_humber_l2_fdb_flush_by_vlan(uint8 chip_num,uint16 fid, uint32 flush_flag)
{
    ctc_listnode_t* next_node = NULL;
    ctc_listnode_t* node = NULL;
    ctc_linklist_t* fdb_list = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    int32 ret = 0;
    uint8 chip_id = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

    uint32 flush_fdb_cnt_per_loop;
    SYS_L2_FDB_FLUSH_COUNT_SET(flush_fdb_cnt_per_loop);

    CTC_FID_RANGE_CHECK(fid);

    L2_FDB_GET_FDB_LIST_BY_FID(fid, fdb_list);

    CTC_LIST_LOOP_DEL(fdb_list, p_l2_node, node, next_node)
    {
         switch(flush_flag)
         {
          case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
              if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
             if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC)
               ||CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
             break;
          case CTC_L2_FDB_ENTRY_ALL:
              if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_SYSTEM_RSV))
             {
                continue;
             }
          default:
             break;

         }


        /* 1)delete hw entry  */
        for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
        {
            ret = _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
            if (ret != CTC_E_NONE)
            {
                return ret;
            }
        }

        /* 2)delete sw entry  */
        bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_NORMAL_FDB;
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        ctc_vector_del( pl2_master->fdb_tbl_vec, p_l2_node->index);
        _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
        _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
        _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
        ret = _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
        if (ret != CTC_E_NONE)
        {
            break;
        }

        mem_free(p_l2_node);
        p_l2_node = NULL;

        SYS_L2_FDB_FLUSH_COUNT_PROCESS(flush_fdb_cnt_per_loop);

    }

    return ret;
}

/**
 @brief flush fdb entry by mac if the fid = 0xffff
*/
int32
_sys_humber_l2_fdb_remove_mac(sys_l2_node_t *p_l2_node, sys_l2_fdb_mac_hash_info_t *mac_hash_info)
{
    int32 ret = 0;
    uint8 chip_id = 0;
    uint8 chip_num = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

    if (NULL == p_l2_node || NULL == mac_hash_info)
    {
        return FALSE;
    }

    if (kal_memcmp (mac_hash_info->mac, p_l2_node->key.mac, CTC_ETH_ADDR_LEN))
    {
        return FALSE;
    }

    switch(mac_hash_info->query_flag)
    {
    case CTC_L2_FDB_ENTRY_STATIC:             /**<  all static fdb record */
        if (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
            return FALSE;
        }
        break;
    case  CTC_L2_FDB_ENTRY_DYNAMIC:          /**<  all dynamic fdb record */
        if ( CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
             return FALSE;
        }
        break;
    case CTC_L2_FDB_ENTRY_LOCAL_DYNAMIC:            /**<  all local chip's dynamic fdb record */
        if (CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_REMOTE_DYN)
            || CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
        {
          return FALSE;
        }
        break;
    case CTC_L2_FDB_ENTRY_ALL:
        break;
    default:
        break;

    }

    chip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
        if (ret != CTC_E_NONE)
        {
            return FALSE;
        }
    }
    /* 2)delete sw entry  */
    bd_hw_entry.fdb_type  = SYS_L2_FDB_TYPE_NORMAL_FDB;
    _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    ctc_vector_del( pl2_master->fdb_tbl_vec, p_l2_node->index);
    _sys_humber_l2_fdb_remove_from_fdb_port_list(p_l2_node);
    _sys_humber_l2_fdb_remove_from_fdb_vlan_list(p_l2_node);
    _sys_humber_l2_fdb_mac_entry_remove_from_hash_table(p_l2_node);
    ret = _sys_humber_l2_fdb_remove_from_hash_table(p_l2_node);
    if (ret != CTC_E_NONE)
    {
        return FALSE;
    }

    if (p_l2_node)
    {
        mem_free(p_l2_node);
        p_l2_node = NULL;
    }

    return TRUE;
}



/**
 @brief flush fdb entry by mac
*/
static int32
_sys_humber_l2_fdb_flush_by_mac(uint8 chip_num, mac_addr_t mac, uint8 flush_flag)
{
     sys_l2_fdb_mac_hash_info_t mac_hash_info;
     hash_traversal_fn fun = NULL;

     kal_memset(&mac_hash_info, 0, sizeof(sys_l2_fdb_mac_hash_info_t));
     kal_memcpy(mac_hash_info.mac, mac, sizeof(mac_addr_t));
     mac_hash_info.query_flag = flush_flag;

     fun = (hash_traversal_fn) _sys_humber_l2_fdb_remove_mac;

     ctc_hash_traverse2_remove(pl2_master->fdb_mac_hash, fun, &mac_hash_info);

     return CTC_E_NONE;
}


/**
 @brief flush fdb entry according to pFlush
*/
int32
sys_humber_l2_fdb_flush(ctc_l2_fdb_flush_t* pFlush)
{
    int32 ret = CTC_E_NONE;
    uint8 chip_num = 0;
    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(pFlush);

L2_LOCK;
    chip_num = sys_humber_get_local_chip_num();
    switch (pFlush->flush_type)
    {
        case CTC_L2_FDB_ENTRY_OP_BY_VID:
            ret = _sys_humber_l2_fdb_flush_by_vlan(chip_num, pFlush->fid, pFlush->flush_flag);

            break;
        case CTC_L2_FDB_ENTRY_OP_BY_PORT:
            ret = _sys_humber_l2_fdb_flush_by_port(chip_num, pFlush->gport, pFlush->flush_flag);

            break;
       case CTC_L2_FDB_ENTRY_OP_BY_PORT_VLAN:
            ret = _sys_humber_l2_fdb_flush_by_port_vlan(chip_num, pFlush->gport, pFlush->fid,pFlush->flush_flag);

            break;
       case CTC_L2_FDB_ENTRY_OP_BY_MAC_VLAN:
              ret = _sys_humber_l2_fdb_flush_by_mac_vlan(chip_num, pFlush->mac, pFlush->fid,pFlush->flush_flag);
            break;
        case CTC_L2_FDB_ENTRY_OP_BY_MAC:
            ret = _sys_humber_l2_fdb_flush_by_mac(chip_num, pFlush->mac, pFlush->flush_flag);

	     break;
        case CTC_L2_FDB_ENTRY_OP_ALL:
            ret = _sys_humber_l2_fdb_flush_all(chip_num, pFlush->flush_flag);

            break;
        default:
            ret = CTC_E_INVALID_PARAM;
            break;

    }
L2_UNLOCK;

    return ret;
}


/**
 @brief add an entry in the mcast table
*/
int32
sys_humber_l2mcast_add_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_nh_offset_array_t nhp_array;
    sys_nh_param_mcast_group_t  nh_mcast_group;
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
    sys_l2_mcast_node_t* mcast_node = NULL;
    int32 ret = 0;
    uint8 chip_num = 0;
    uint8 chip_id = 0;
    uint8 chip_id2= 0;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2mc_addr);
    CTC_FID_RANGE_CHECK(l2mc_addr->fid);

    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d  \n",
                      l2mc_addr->mac[0],l2mc_addr->mac[1],l2mc_addr->mac[2],
                      l2mc_addr->mac[3],l2mc_addr->mac[4],l2mc_addr->mac[5],
                      l2mc_addr->fid);

    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));
    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));

    l2_node_tmp.key.fid = l2mc_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2mc_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

L2_LOCK;

    if (l2mc_addr->l2mc_grp_id > pl2_master->max_group_num)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_INVALID_PARAM, pl2_master->l2_mutex);
    }

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL != p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_EXIST, pl2_master->l2_mutex);

    }
    p_l2_node = (sys_l2_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_node_t));
    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);
    }

    kal_memset(p_l2_node, 0, sizeof(sys_l2_node_t));

    kal_memcpy(&p_l2_node->key, &l2_node_tmp.key, sizeof(sys_l2_mackey_t));
    CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC | SYS_L2_NODE_FLAG_IS_L2MC);
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_MCAST_FDB;
    bd_hw_entry.l2_addr.fid = l2mc_addr->fid;
    kal_memcpy(&bd_hw_entry.l2_addr.mac, l2mc_addr->mac, sizeof(mac_addr_t));
    bd_hw_entry.l2_addr.flag = l2mc_addr->flag;
    bd_hw_entry.sw_hash_index= l2_node_tmp.index;

    ret = _sys_humber_l2_build_index( p_l2_node, &bd_hw_entry);
    if (CTC_E_NONE != ret)
    {
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    mcast_node = (sys_l2_mcast_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_mcast_node_t));
    if(NULL == mcast_node)
    {
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);

    }

    for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
    {
        /* initiate head of link list for 32 global chips */
        mcast_node->local_member_list[chip_id] = ctc_slist_new();
        if (NULL == mcast_node->local_member_list[chip_id]  )
        {
            for (chip_id2 = 0; chip_id2 < chip_id; chip_id2 ++)
            {
                ctc_slist_delete(mcast_node->local_member_list[chip_id2] );
            }
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);
        }
    }

    /*1) add sw entry */
   ctc_listnode_add(pl2_master->mcast_grp_list, mcast_node);

    p_l2_node->vlan_entey = (ctc_listnode_t *)mcast_node;

    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));
    //ret = sys_humber_brgmc_nh_create(l2mc_addr->l2mc_grp_id, NULL, &(mcast_node->l2mc_nhp_id), nhp_array);
    ret = sys_humber_mcast_nh_create(l2mc_addr->l2mc_grp_id, &nh_mcast_group);
    if (ret != CTC_E_NONE)
    {
        for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
        {
            ctc_slist_delete(mcast_node->local_member_list[chip_id] );
        }
        ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(mcast_node);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }
    mcast_node->l2mc_nhp_id = nh_mcast_group.nhid;
    kal_memcpy(nhp_array, nh_mcast_group.fwd_offset, sizeof(sys_nh_offset_array_t));

    /*
       add nexthop id to group arrary for update
    */
    pl2_master->p_group_array[l2mc_addr->l2mc_grp_id] = nh_mcast_group.nhid;

    ret = _sys_humber_l2_fdb_add_to_hash_table(p_l2_node);
    if (ret != CTC_E_NONE)
    {

        sys_humber_mcast_nh_delete(mcast_node->l2mc_nhp_id);

        for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
        {
            ctc_slist_delete(mcast_node->local_member_list[chip_id] );
        }
        ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(mcast_node);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    /* 3)write hw */
    chip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < chip_num; chip_id++)
    {
        bd_hw_entry.fwd_ptr = nhp_array[chip_id];
        ret = _sys_humber_l2_build_hw_entry(chip_id, p_l2_node, &bd_hw_entry);
        if (ret != CTC_E_NONE)
        {

            sys_humber_mcast_nh_delete(mcast_node->l2mc_nhp_id);

            for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
            {
                ctc_slist_delete(mcast_node->local_member_list[chip_id] );
            }
            ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
            mem_free(mcast_node);
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            _sys_humber_l2_fdb_remove_from_hash_table(&l2_node_tmp);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }

        SYS_FDB_DBG_INFO("Create mcast group:chip_id:%d mac index:0x%x  nexthopid:%d  ds_fwd_offset:0x%x\n",
                         chip_id, p_l2_node->index,  mcast_node->l2mc_nhp_id, bd_hw_entry.fwd_ptr);
    }
L2_UNLOCK;

    return ret;
}

/**
 @brief Remove an entry in the multicast table
*/
int32
sys_humber_l2mcast_remove_addr(ctc_l2_mcast_addr_t* l2mc_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
    sys_l2_mcast_node_t* mcast_node = NULL;
    int32 ret = 0;
    uint8 chip_id = 0;
    uint8 max_local_chip_num = 0;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;

     SYS_L2_FDB_INIT_CHECK();
     CTC_PTR_VALID_CHECK(l2mc_addr);
     CTC_FID_RANGE_CHECK(l2mc_addr->fid);

     SYS_FDB_DBG_FUNC();
     SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d  \n",
                      l2mc_addr->mac[0], l2mc_addr->mac[1], l2mc_addr->mac[2],
                      l2mc_addr->mac[3], l2mc_addr->mac[4], l2mc_addr->mac[5],
                      l2mc_addr->fid);

     kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));

     l2_node_tmp.key.fid = l2mc_addr->fid;
     kal_memcpy(l2_node_tmp.key.mac, l2mc_addr->mac, sizeof(mac_addr_t));
     l2_node_tmp.flag = 0;

L2_LOCK;
    max_local_chip_num = sys_humber_get_local_chip_num();
    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL == p_l2_node || !CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }
    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
    if (NULL == mcast_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NONE, pl2_master->l2_mutex);
    }

    if(CTC_FLAG_ISSET(l2mc_addr->flag, CTC_L2_FLAG_KEEP_EMPTY_ENTRY))
     {
          sys_humber_mc_remove_all_member(mcast_node->local_member_list, mcast_node->l2mc_nhp_id);
          CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NONE, pl2_master->l2_mutex);
     }

    /* 1) delete hw entry */
    for (chip_id = 0; chip_id < max_local_chip_num; chip_id++)
    {
        ret = _sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node);
        if (ret != CTC_E_NONE)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }
    }

    sys_humber_mc_remove_all_member(mcast_node->local_member_list, mcast_node->l2mc_nhp_id);

     sys_humber_mcast_nh_delete(mcast_node->l2mc_nhp_id);


     for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
     {
          ctc_slist_delete(mcast_node->local_member_list[chip_id]);
     }
    ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
    mem_free(mcast_node);

    bd_hw_entry.fdb_type  = SYS_L2_FDB_TYPE_MCAST_FDB;
    _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    _sys_humber_l2_fdb_remove_from_hash_table(&l2_node_tmp);
    mem_free(p_l2_node);
L2_UNLOCK;

    return CTC_E_NONE;
}
/**
 @brief set vlan default entry's operation for unknown multicast or unicast.
*/
int32
sys_humber_l2_set_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    sys_l2_node_t              *p_l2_node = NULL;
    int32                       ret = 0;
    int32                       ret_state = 0;
    int32                       cmd = 0;
    uint8                       chip_num = 0;
    uint8                       chip_id = 0;
    ds_mac_t                    macda;

    SYS_L2_FDB_INIT_CHECK();
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("fid :%d\n",l2dflt_addr->fid);

    L2_LOCK;

    chip_num = sys_humber_get_local_chip_num();
    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if (NULL == p_l2_node || NULL == p_l2_node->vlan_entey)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_DEFAULT_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        cmd = DRV_IOR(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
        ret = drv_tbl_ioctl(chip_id, p_l2_node->index, cmd, &macda);

        if (ret != CTC_E_NONE)
        {
            ret_state = ret;

            continue;
        }

        if (l2dflt_addr->flag & CTC_L2_DFT_VLAN_FLAG_UNKOWN_UCAST_DROP)
        {
            macda.ucast_discard = 1;
        }
        else
        {
            macda.ucast_discard = 0;
        }

        if (l2dflt_addr->flag & CTC_L2_DFT_VLAN_FLAG_UNKOWN_MCAST_DROP)
        {
            macda.mcast_discard = 1;
        }
        else
        {
            macda.mcast_discard = 0;
        }

        if (l2dflt_addr->flag & CTC_L2_DFT_VLAN_FLAG_PROTOCOL_EXCP_TOCPU)
        {
            macda.proto_exception_en = 1;
        }
        else
        {
            macda.proto_exception_en = 0;
        }

        cmd = DRV_IOW(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
        ret = drv_tbl_ioctl(chip_id, p_l2_node->index, cmd, &macda);

        if (ret != CTC_E_NONE)
        {
            ret_state = ret;
        }
    }

    L2_UNLOCK;

    return ret_state;
}
/**
 @brief get vlan default entry's operation for unknown multicast or unicast.
*/
int32
sys_humber_l2_get_default_entry_features(ctc_l2dflt_addr_t* l2dflt_addr)
{
    sys_l2_node_t              *p_l2_node = NULL;
    int32                       ret = 0;
    int32                       cmd = 0;
    uint8                       chip_num = 0;
    uint8                       chip_id = 0;
    ds_mac_t                    macda;

    SYS_L2_FDB_INIT_CHECK();
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("fid :%d\n",l2dflt_addr->fid);

    L2_LOCK;
    chip_num = sys_humber_get_local_chip_num();

    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if (NULL == p_l2_node || NULL == p_l2_node->vlan_entey)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_DEFAULT_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }


    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        cmd = DRV_IOR(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
        ret = drv_tbl_ioctl(chip_id, p_l2_node->index, cmd, &macda);

        if (ret == CTC_E_NONE)
        {
            chip_id = MAX_LOCAL_CHIP_NUM;
        }

        if (macda.ucast_discard == 1)
        {
            l2dflt_addr->flag |= CTC_L2_DFT_VLAN_FLAG_UNKOWN_UCAST_DROP;
        }

        if (macda.mcast_discard == 1)
        {
            l2dflt_addr->flag |= CTC_L2_DFT_VLAN_FLAG_UNKOWN_MCAST_DROP;
        }

        if (macda.proto_exception_en == 1)
        {
            l2dflt_addr->flag |= CTC_L2_DFT_VLAN_FLAG_PROTOCOL_EXCP_TOCPU;
        }

    }
    L2_UNLOCK;

    return ret;
}
/**
 @brief Add a given port/port list to  a existed multicast group.
*/
int32
sys_humber_l2mcast_add_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_mc_member_info_t l2mcast_member;
    int32 ret = 0;
    uint8  aps_brg_en = 0;
    uint16 dest_id = 0;
    sys_l2_node_t l2_node_tmp;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2mc_addr);
    CTC_FID_RANGE_CHECK(l2mc_addr->fid);
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d  ,gport:%d \n",
                      l2mc_addr->mac[0],l2mc_addr->mac[1],l2mc_addr->mac[2],
                      l2mc_addr->mac[3],l2mc_addr->mac[4],l2mc_addr->mac[5],
                      l2mc_addr->fid,l2mc_addr->member.mem_port);

    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    kal_memset(&l2mcast_member, 0, sizeof(sys_mc_member_info_t));

    l2_node_tmp.key.fid = l2mc_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2mc_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

    if (l2mc_addr->member_invalid)
    {
       return CTC_E_L2MCAST_MEMBER_INVALID;
    }

     if(l2mc_addr->with_nh)
     {
           CTC_ERROR_RETURN(sys_humber_nh_get_port(l2mc_addr->member.nh_id,&aps_brg_en,&dest_id));
          {
               l2mcast_member.dest_id = dest_id;
          }
          l2mcast_member.ref_nhid = l2mc_addr->member.nh_id;

      }
      else
      {
            sys_nh_brguc_nhid_info_t brguc_nhid_info;

            kal_memset(&brguc_nhid_info,0,sizeof(sys_nh_brguc_nhid_info_t));

            if ( CTC_FLAG_ISSET(l2mc_addr->flag, CTC_L2_FLAG_RAW_PKT_ELOG_CPU))
            {
                brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_RAW_PACKET_ELOG_CPU;
                CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(l2mc_addr->member.mem_port, &brguc_nhid_info));
                l2mc_addr->with_nh = 1;
                l2mcast_member.dest_id = l2mc_addr->member.mem_port;

                l2mc_addr->member.nh_id = brguc_nhid_info.ram_pkt_nhid;
                l2mcast_member.ref_nhid = brguc_nhid_info.ram_pkt_nhid;
            }
            else  if( CTC_FLAG_ISSET(l2mc_addr->flag, CTC_L2_FLAG_PORTUNTAGGED))
            {
                brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_UNTAGGED;
                CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(l2mc_addr->member.mem_port, &brguc_nhid_info));
                l2mc_addr->with_nh = 1;
                l2mcast_member.dest_id = l2mc_addr->member.mem_port;

                l2mc_addr->member.nh_id = brguc_nhid_info.brguc_untagged_nhid;
                l2mcast_member.ref_nhid = brguc_nhid_info.brguc_untagged_nhid;
            }
            else  if( CTC_FLAG_ISSET(l2mc_addr->flag, CTC_L2_FLAG_SERVICE_QUEUE))
            {
                brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_SERVICE_QUEUE;
                CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(l2mc_addr->member.mem_port, &brguc_nhid_info));
                l2mc_addr->with_nh = 1;
                l2mcast_member.dest_id = l2mc_addr->member.mem_port;

                l2mc_addr->member.nh_id = brguc_nhid_info.srv_queue_nhid;
                l2mcast_member.ref_nhid = brguc_nhid_info.srv_queue_nhid;
            }
            else
            {
                l2mcast_member.dest_id = l2mc_addr->member.mem_port;
                CTC_GLOBAL_PORT_CHECK(l2mcast_member.dest_id);
            }

      }

L2_LOCK;
    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL == p_l2_node || !CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }
    mcast_node = ( sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);

    {
        if (l2mc_addr->with_nh)
        {
            l2mcast_member.member_type  = SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH;
        }
        else
        {
            l2mcast_member.member_type  = SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS;
        }

        if(l2mc_addr->remote_chip)
        {/*can add remote member with nexthop */
            l2mcast_member.member_type = SYS_NH_PARAM_MCAST_MEM_REMOTE;
            l2mcast_member.remote_chip = TRUE;
        }
        ret = sys_humber_mc_add_member(mcast_node->local_member_list, &l2mcast_member, mcast_node->l2mc_nhp_id);
       if(ret != CTC_E_NONE)
       {
          CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L2MCAST_ADD_MEMBER_FAILED, pl2_master->l2_mutex);
       }
    }

L2_UNLOCK;

    return ret;
}

/**
 @brief Remove a given port/port list to  a existed multicast group.
*/
int32
sys_humber_l2mcast_remove_member(ctc_l2_mcast_addr_t* l2mc_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_mc_member_info_t l2mcast_member;
    uint16 dest_id = 0;
    uint8   aps_brg_en = 0;
    int32 ret = 0;

    sys_l2_node_t l2_node_tmp;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2mc_addr);
    CTC_FID_RANGE_CHECK(l2mc_addr->fid);
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("mac:%.2x%.2x.%.2x%.2x%.2x%.2x ,fid :%d  ,gport:%d \n",
                      l2mc_addr->mac[0],l2mc_addr->mac[1],l2mc_addr->mac[2],
                      l2mc_addr->mac[3],l2mc_addr->mac[4],l2mc_addr->mac[5],
                      l2mc_addr->fid,l2mc_addr->member.mem_port);

    kal_memset(&l2_node_tmp, 0, sizeof(l2_node_tmp));
    kal_memset(&l2mcast_member, 0, sizeof(sys_mc_member_info_t));

    l2_node_tmp.key.fid = l2mc_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2mc_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;


    if (l2mc_addr->member_invalid)
    {
        return CTC_E_L2MCAST_MEMBER_INVALID;
    }
     if(l2mc_addr->with_nh)
     {
          CTC_ERROR_RETURN(sys_humber_nh_get_port(l2mc_addr->member.nh_id,&aps_brg_en,&dest_id));
          {
               l2mcast_member.dest_id = dest_id;
          }
          l2mcast_member.ref_nhid = l2mc_addr->member.nh_id;

      }
      else
      {
        l2mcast_member.dest_id = l2mc_addr->member.mem_port;
        CTC_GLOBAL_PORT_CHECK(l2mcast_member.dest_id);
      }

L2_LOCK;
    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if ( (NULL == p_l2_node )
       || (!CTC_FLAG_ISSET(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_L2MC)))
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    mcast_node = ( sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
    {
       l2mcast_member.remote_chip = l2mc_addr->remote_chip;
       sys_humber_mc_remove_member(mcast_node->local_member_list, &l2mcast_member, mcast_node->l2mc_nhp_id);
       if(ret != CTC_E_NONE)
       {
          CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
       }
    }

L2_UNLOCK;

    return CTC_E_NONE;
}

/**
* op for default entry
*/

int32
sys_humber_l2_add_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_l2_node_t* p_l2_node = NULL;
    int32 ret = 0;
    uint8 chip_num = 0;
    uint8 chip_id = 0;
    uint8 chip_id2 = 0;
    sys_nh_offset_array_t nhp_array;
    sys_nh_param_mcast_group_t  nh_mcast_group;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2dflt_addr);
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    CTC_GLOBAL_PORT_CHECK(l2dflt_addr->gport);

    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));
    kal_memset(&nh_mcast_group, 0, sizeof(sys_nh_param_mcast_group_t));
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("fid :%d port:0x%x ,l2mc_grp_id:%d \n",l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->l2mc_grp_id);

L2_LOCK;

    if (l2dflt_addr->l2mc_grp_id > pl2_master->max_group_num)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_INVALID_PARAM, pl2_master->l2_mutex);
    }

    chip_num = sys_humber_get_local_chip_num();
    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if (NULL == p_l2_node)
    {
        p_l2_node = (sys_l2_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_node_t));
        if (NULL == p_l2_node)
        {
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);
        }
        kal_memset(p_l2_node, 0, sizeof(sys_l2_node_t));
    }
    else
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_EXIST, pl2_master->l2_mutex);
    }

    p_l2_node->key.fid = l2dflt_addr->fid;
    CTC_SET_FLAG(p_l2_node->flag, SYS_L2_NODE_FLAG_IS_STATIC);
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_VLAN_DFT;
    bd_hw_entry.l2_addr.fid = l2dflt_addr->fid;
    bd_hw_entry.port_dft_valid = l2dflt_addr->port_valid;
    bd_hw_entry.src_gport = l2dflt_addr->gport;

    ret = _sys_humber_l2_build_index(p_l2_node, &bd_hw_entry);
    if (CTC_E_NONE != ret )
    {
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    mcast_node = (sys_l2_mcast_node_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_mcast_node_t));
    if (NULL == mcast_node)
    {
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);
    }

    for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id++)
    {
        /* initiate head of link list for 32 global chips */
        mcast_node->local_member_list[chip_id] = ctc_slist_new();
        if (NULL == mcast_node->local_member_list[chip_id]  )
        {
            for (chip_id2 = 0; chip_id2 < chip_id; chip_id2 ++)
            {
                ctc_slist_delete(mcast_node->local_member_list[chip_id2] );
            }
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_NO_MEMORY, pl2_master->l2_mutex);
        }
    }

    ctc_listnode_add_tail(pl2_master->mcast_grp_list, mcast_node);

    p_l2_node->vlan_entey = (ctc_listnode_t *)mcast_node;
    p_l2_node->key.fid = l2dflt_addr->fid;

    /* 1)write sw entry */
    ret = _sys_humber_l2_fdb_add_to_dflt_entry(p_l2_node, l2dflt_addr->gport, l2dflt_addr->port_valid);
    if (ret != CTC_E_NONE)
    {
        for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
       {
          ctc_slist_delete(mcast_node->local_member_list[chip_id] );
       }
       ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);

        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(mcast_node);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    //ret = sys_humber_mc_nh_create(l2mc_grp_id, NULL, &(mcast_node->l2mc_nhp_id), nhp_array);
    ret = sys_humber_mcast_nh_create(l2dflt_addr->l2mc_grp_id, &nh_mcast_group);
    if (ret != CTC_E_NONE)
    {
         _sys_humber_l2_fdb_remove_from_dflt_entry(p_l2_node, l2dflt_addr->gport, l2dflt_addr->port_valid);
        for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
       {
          ctc_slist_delete(mcast_node->local_member_list[chip_id] );
       }
        ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
        _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
        mem_free(mcast_node);
        mem_free(p_l2_node);
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }

    mcast_node->l2mc_nhp_id = nh_mcast_group.nhid;
    kal_memcpy(nhp_array, nh_mcast_group.fwd_offset, sizeof(sys_nh_offset_array_t));

    /*
       add nexthop id to group arrary for update
    */
    pl2_master->p_group_array[l2dflt_addr->l2mc_grp_id] = nh_mcast_group.nhid;

    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        bd_hw_entry.fwd_ptr  = nhp_array[chip_id];
        ret = _sys_humber_l2_build_hw_entry(chip_id, p_l2_node, &bd_hw_entry);
        if (ret != CTC_E_NONE)
        {
            _sys_humber_l2_fdb_remove_from_dflt_entry(p_l2_node, l2dflt_addr->gport, l2dflt_addr->port_valid);
            sys_humber_mcast_nh_delete(mcast_node->l2mc_nhp_id);
            for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id++)
            {
                ctc_slist_delete(mcast_node->local_member_list[chip_id] );
            }
            ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
            _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
            mem_free(mcast_node);
            mem_free(p_l2_node);
            CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
        }
        SYS_FDB_DBG_INFO("Create mcast group: chip_id:%d,mac index:0x%x grp_id %d \
                          nexthopid:%d  ds_fwd_offset:0x%x\n",chip_id,p_l2_node->index,
                          l2dflt_addr->l2mc_grp_id,mcast_node->l2mc_nhp_id,bd_hw_entry.fwd_ptr);
      }
L2_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_l2_remove_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    int32 ret = 0;
    uint8 chip_num = 0;
    uint8 chip_id = 0;
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;


    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2dflt_addr);
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    CTC_GLOBAL_PORT_CHECK(l2dflt_addr->gport);

    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));
    SYS_FDB_DBG_FUNC();
    SYS_FDB_DBG_INFO("fid :%d  port:0x%x\n",l2dflt_addr->fid, l2dflt_addr->gport);

L2_LOCK;
    chip_num = sys_humber_get_local_chip_num();
    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
    if (NULL == mcast_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    sys_humber_mc_remove_all_member(mcast_node->local_member_list,  mcast_node->l2mc_nhp_id);
    sys_humber_mcast_nh_delete(mcast_node->l2mc_nhp_id);

    for (chip_id = 0; chip_id < CTC_MAX_HUMBER_CHIP_NUM; chip_id ++)
    {
        ctc_slist_delete(mcast_node->local_member_list[chip_id] );
    }

    ctc_listnode_delete(pl2_master->mcast_grp_list, mcast_node);
    mem_free(mcast_node);

    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_VLAN_DFT;
    bd_hw_entry.l2_addr.fid = l2dflt_addr->fid;
    ret = _sys_humber_l2_free_index(p_l2_node, &bd_hw_entry);
    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(_sys_humber_l2_remove_fdb_from_hw(chip_id, p_l2_node), \
                                     pl2_master->l2_mutex);
    }

    _sys_humber_l2_fdb_remove_from_dflt_entry(p_l2_node, l2dflt_addr->gport, l2dflt_addr->port_valid);
    mem_free(p_l2_node);
L2_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_l2_add_port_to_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_mc_member_info_t l2mcast_member;
    uint16 dest_id = 0;
    uint8  aps_brg_en = 0;
    int32 ret = 0;
    sys_nh_brguc_nhid_info_t brguc_nhid_info;

    kal_memset(&brguc_nhid_info,0,sizeof(sys_nh_brguc_nhid_info_t));

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2dflt_addr);
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    SYS_FDB_DBG_INFO("fid :%d  port:0x%x\n",l2dflt_addr->fid, l2dflt_addr->gport);
    SYS_FDB_DBG_INFO("member port:0x%x, remote-chip:%d\n", l2dflt_addr->member.mem_port, l2dflt_addr->remote_chip);

     kal_memset(&l2mcast_member, 0, sizeof(sys_mc_member_info_t));
     if(l2dflt_addr->with_nh)
     {
          CTC_ERROR_RETURN(sys_humber_nh_get_port(l2dflt_addr->member.nh_id,&aps_brg_en,&dest_id));
          {
               l2mcast_member.dest_id = dest_id;
          }
          l2mcast_member.ref_nhid = l2dflt_addr->member.nh_id;

      }
      else
      {
        l2mcast_member.dest_id = l2dflt_addr->member.mem_port;
        CTC_GLOBAL_PORT_CHECK(l2mcast_member.dest_id);

        if(l2dflt_addr->port_unattged)
        {
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE;
            CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(l2dflt_addr->member.mem_port, &brguc_nhid_info));
            l2mcast_member.ref_nhid = brguc_nhid_info.brguc_untagged_nhid;
            l2dflt_addr->with_nh = TRUE;
        }
        else if(l2dflt_addr->port_srv_queue)
        {
            brguc_nhid_info.nh_type_flag = SYS_NH_PARAM_BRGUC_SUB_TYPE_NONE;
            CTC_ERROR_RETURN(sys_humber_brguc_get_nhid(l2dflt_addr->member.mem_port, &brguc_nhid_info));
            l2mcast_member.ref_nhid = brguc_nhid_info.srv_queue_nhid;
            l2dflt_addr->with_nh = TRUE;
        }

      }
L2_LOCK;
    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if(NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
    if (l2dflt_addr->with_nh)
    {
    	l2mcast_member.member_type  = SYS_NH_PARAM_MCAST_MEM_LOCAL_WITH_NH;
    }
    else
    {
    	l2mcast_member.member_type  = SYS_NH_PARAM_BRGMC_MEM_LOCAL_WO_VLANTRANS;
    }

    if (l2dflt_addr->remote_chip)
    {
    	l2mcast_member.member_type = SYS_NH_PARAM_MCAST_MEM_REMOTE;
        l2mcast_member.remote_chip = TRUE;
    }

    ret = sys_humber_mc_add_member(mcast_node->local_member_list, &l2mcast_member, mcast_node->l2mc_nhp_id);
    if(ret != CTC_E_NONE)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_L2MCAST_ADD_MEMBER_FAILED, pl2_master->l2_mutex);
    }


    if (ret != CTC_E_NONE)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }
 L2_UNLOCK;

    return CTC_E_NONE;
}

int32
sys_humber_l2_remove_port_from_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{

    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;
    sys_mc_member_info_t l2mcast_member;

    uint16 dest_id = 0;
    uint8  aps_brg_en = 0;
    int32 ret = 0;

    SYS_L2_FDB_INIT_CHECK();
    CTC_PTR_VALID_CHECK(l2dflt_addr);
    CTC_FID_RANGE_CHECK(l2dflt_addr->fid);
    SYS_FDB_DBG_INFO("fid :%d  port:0x%x\n",l2dflt_addr->fid, l2dflt_addr->gport);
    SYS_FDB_DBG_INFO("member port:0x%x, remote-chip:%d\n", l2dflt_addr->member.mem_port, l2dflt_addr->remote_chip);

    kal_memset(&l2mcast_member, 0, sizeof(sys_mc_member_info_t));


    if(l2dflt_addr->with_nh)
     {
          CTC_ERROR_RETURN(sys_humber_nh_get_port(l2dflt_addr->member.nh_id,&aps_brg_en,&dest_id));
          {
              l2mcast_member.dest_id = dest_id;
          }
          l2mcast_member.ref_nhid = l2dflt_addr->member.nh_id;

      }
      else
      {
          l2mcast_member.dest_id = l2dflt_addr->member.mem_port;
	      CTC_GLOBAL_PORT_CHECK(l2mcast_member.dest_id);
      }

 L2_LOCK;
    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if (NULL == p_l2_node)
    {
        CTC_ERROR_RETURN_WITH_UNLOCK(CTC_E_ENTRY_NOT_EXIST, pl2_master->l2_mutex);
    }

    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);

    l2mcast_member.remote_chip = l2dflt_addr->remote_chip;
    sys_humber_mc_remove_member(mcast_node->local_member_list, &l2mcast_member, mcast_node->l2mc_nhp_id);
    if(ret != CTC_E_NONE)
    {
    	CTC_ERROR_RETURN_WITH_UNLOCK(ret, pl2_master->l2_mutex);
    }
 L2_UNLOCK;

    return CTC_E_NONE;
}


static int32
_sys_humber_l2_add_sys_default_entry(void)
{
    /* add a system default entry,the entry will be discarded */
    tbl_entry_t tcam_key;
    ds_mac_key_t mackey_data;
    ds_mac_key_t mackey_mask;
    sys_humber_opf_t opf;
    sys_l2_fdb_build_hw_entry_t bd_hw_entry;
    ds_mac_t macda;
    uint32 cmd = 0;
    uint32 sys_dft_offset = 0;
    int32 ret = 0;
    uint8 chip_num = 0;
    uint8 chip_id = 0;

    if (pl2_master == NULL)
    {
        return CTC_E_NOT_INIT;
    }

    kal_memset(&macda, 0, sizeof(ds_mac_t));
    kal_memset(&bd_hw_entry, 0, sizeof(sys_l2_fdb_build_hw_entry_t));
    kal_memset(&mackey_data, 0, sizeof(mackey_data));
    kal_memset(&mackey_mask, 0, sizeof(mackey_mask));
    kal_memset(&opf, 0, sizeof(opf));

    tcam_key.data_entry = (uint32 *)&mackey_data;
    tcam_key.mask_entry = (uint32 *)&mackey_mask;

    opf.pool_type = FDB_SRAM_HASH_COLLISION_KEY;
    opf.pool_index = pl2_master->offset_pool_num-1;
    ret = sys_humber_opf_reverse_alloc_offset(&opf, 1, &sys_dft_offset);
    if (ret != CTC_E_NONE)
    {
       return ret;
    }

    /* build  tcam  data */
    bd_hw_entry.fdb_type = SYS_L2_FDB_TYPE_SYS_DFT;
    bd_hw_entry.l2_addr.flag = CTC_L2_FLAG_IS_STATIC | CTC_L2_FLAG_DISCARD|CTC_L2_FLAG_PROTOCOL_ENTRY;

    _sys_humber_l2_build_macda_entry( &bd_hw_entry, &macda);

    /* build  tcam  key */
    _sys_humber_l2_build_mackey_entry(&bd_hw_entry, &tcam_key);

    chip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < chip_num && chip_id < MAX_LOCAL_CHIP_NUM; chip_id++)
    {
       cmd = DRV_IOW(IOC_TABLE, DS_MAC, DRV_ENTRY_FLAG);
       CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id, (pl2_master->tcam_base + sys_dft_offset), cmd, &macda));

       cmd = DRV_IOW(IOC_TABLE, DS_MAC_KEY, DRV_ENTRY_FLAG);
       CTC_ERROR_RETURN(drv_tbl_ioctl(chip_id,  sys_dft_offset, cmd, &tcam_key));
    }

    return CTC_E_NONE;
}

int32
sys_humber_l2_fdb_get_fdb_tbl_range(uint32* start ,uint32* end)
{

    SYS_L2_FDB_INIT_CHECK();

    *start = 0;
    *end = pl2_master->max_sram_size - 1;

    return CTC_E_NONE;
}


static int32
_sys_humber_show_all_fdb_status(sys_l2_node_t* p_fdb_node, sys_l2_fdb_entry_t *pl2_fdb_entry)
{
    if (NULL == p_fdb_node)
    {
        return CTC_E_INVALID_PTR;
    }
    if (CTC_FLAG_ISSET(p_fdb_node->flag, SYS_L2_NODE_FLAG_IS_STATIC))
    {
        pl2_fdb_entry->static_count++;
    }
    if (CTC_FLAG_ISSET(p_fdb_node->flag, SYS_L2_NODE_FLAG_IS_L2MC))
    {
        pl2_fdb_entry->L2MC_count++;
    }
    return CTC_E_NONE;
}



static void
_sys_humber_show_fdb_dflt_entry_status(sys_l2_fdb_entry_t  *pl2_fdb_entry)
{
    uint16  index = 0;

    sys_l2_node_t  *p_l2_node = NULL;

    for (index = 0; index <= pl2_master->max_fid_value; index++)
    {
        L2_FDB_GET_DFT_NODE_BY_FID_PORT(index, 0, 0, p_l2_node);
        if (NULL == p_l2_node)
        {
            continue;
        }
        pl2_fdb_entry->default_count++;
        pl2_fdb_entry->static_count++;
        pl2_fdb_entry->total_count++;
    }

}


 int32
_sys_humber_l2_asic_hash_index(mac_addr_t mac,
                             uint16  fid)
{
    uint32 bucket_left = 0, bucket_right = 0;
    uint8 hash_key[8] = {0};


    hash_key[7] = (fid >> 8) & 0xFF;
    hash_key[6] = fid & 0xFF;
    hash_key[5] = mac[0];
    hash_key[4] = mac[1];
    hash_key[3] = mac[2];
    hash_key[2] = mac[3];
    hash_key[1] = mac[4];
    hash_key[0] = mac[5];



    bucket_left = sys_humber_hash_generate_mac_hash0(hash_key, pl2_master->hash_bit_num);

    bucket_right = sys_humber_hash_generate_mac_hash1(hash_key, pl2_master->hash_bit_num);

   kal_printf("bucket_left:%u  bucket_left:%u" ,bucket_left,bucket_right);


    return CTC_E_NONE;
}

int32
sys_humber_show_l2_fdb_status(void)
{
    sys_l2_fdb_entry_t l2_fdb_entry;
    uint8 pool_index =0;
    kal_memset(&l2_fdb_entry,0,sizeof(l2_fdb_entry));


    SYS_FDB_DBG_INFO("-------------------------L2 FDB Status---------------------\n");
    _sys_humber_show_fdb_dflt_entry_status(&l2_fdb_entry);
    ctc_hash_traverse(pl2_master->fdb_hash,
                               (hash_traversal_fn) _sys_humber_show_all_fdb_status,
                               &l2_fdb_entry);
     l2_fdb_entry.total_count += pl2_master->fdb_hash->count;
     l2_fdb_entry.total_count +=1 ; /*System default entry*/
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Mac table size", pl2_master->max_sram_size );
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "hash table size", pl2_master->asic_hash_size );
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Total count", l2_fdb_entry.total_count );
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Do asic hash count", pl2_master->do_hash_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Do left hash count", pl2_master->do_left_hash_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Do right hash count", pl2_master->do_right_hash_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "local_dynmac_count",pl2_master->local_dynmac_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "dynmac_count",pl2_master->dynmac_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "L2MC count",l2_fdb_entry.L2MC_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Static count", l2_fdb_entry.static_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "Default entry count",l2_fdb_entry.default_count);
    SYS_FDB_DBG_INFO("%-20s:%4u \n", "System default entry count",1);
    for (pool_index = 0; pool_index < pl2_master->offset_pool_num; pool_index++)
    {
        SYS_FDB_DBG_INFO("%-20s %4d: %4d\n", "offset pool status",pool_index,pl2_master->pool_status[pool_index]);
    }
    return CTC_E_NONE;
}

// MODIFIED_BY_CENTEC: add this api for openflow only
int32
sys_humber_l2_get_nhid_by_mcast_group_id(uint32 group_id, uint32 *p_nhid)
{
    if (group_id > pl2_master->max_group_num)
    {
        return CTC_E_INVALID_PARAM;
    }

    *p_nhid = pl2_master->p_group_array[group_id];

    return CTC_E_NONE;
}



int32
sys_humber_l2_fdb_init(void* l2_fdb_global_cfg)
{
    int32 ret = 0;
    uint32 entry_num = 0;
    uint32 max_fdb_size = 0;
    uint32 bucket_num = 0;
    uint8 vec_block_num = 0;
    ipe_hash_lookup_result_ctl_t hash_rslt_ctl;
    uint32 cmd =0;
    uint32 hash_collision_sram_size = 0;
    uint8  pool_num = 0;
    uint8 i = 0;
    sys_humber_opf_t opf;
    uint32 start_offset =0;
    uint32 last_pool_block_size = 0;
    uint32 mcast_group_num = 0;
    uint32 glb_dsmet_entry_num = 0, local_dsmet_entry_num = 0;

    CTC_PTR_VALID_CHECK(l2_fdb_global_cfg);
    ctc_l2_fdb_global_cfg_t* p_l2_fdb_global_cfg = (ctc_l2_fdb_global_cfg_t*)l2_fdb_global_cfg;

    if (pl2_master != NULL)
    {
        return CTC_E_NONE;
    }

    pl2_master = (sys_l2_master_t *)mem_malloc(MEM_FDB_MODULE, sizeof(sys_l2_master_t));
    if (NULL == pl2_master)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(pl2_master, 0, sizeof(sys_l2_master_t));

    ret = kal_mutex_create(&(pl2_master->l2_mutex));
    if (ret || !(pl2_master->l2_mutex))
    {
        goto ERROR_FREE_MEM;
    }

    /*get size from tcam allocation module*/
    sys_alloc_get_table_entry_num(DS_MAC_KEY, &entry_num);


    pl2_master->max_fid_value = CTC_MAX_FID_ID;

    /*1 default entry is system default discard entry */
    hash_collision_sram_size = entry_num ;
    pl2_master->sw_hash_tbl_size = SYS_L2_FDB_SW_HASH_TBL_SIZE;

    /* alloc fdb hash size  */
    pl2_master->fdb_hash = ctc_hash_create(SYS_L2_FDB_SW_HASH_TBL_SIZE / SYS_L2_FDB_SW_HASH_BLOCK_SIZE, SYS_L2_FDB_SW_HASH_BLOCK_SIZE,
                                           _sys_humber_l2_fdb_hash_make,
                                           _sys_humber_l2_fdb_hash_compare);
    if (NULL ==  pl2_master->fdb_hash)
    {
    goto ERROR_FREE_MEM;
    }

    //pl2_master->sw_hash_tbl_size = SYS_L2_FDB_SW_HASH_TBL_SIZE;

    /* alloc fdb default entry hash size  */
    pl2_master->fdb_dft_entry_hash = ctc_hash_create(SYS_L2_FDB_DFT_ENTRY_HASH_TBL_SIZE / SYS_L2_FDB_DFT_ENTRY_HASH_BLOCK_SIZE, SYS_L2_FDB_DFT_ENTRY_HASH_BLOCK_SIZE,
                                           _sys_humber_l2_fdb_fid_entry_hash_make,
                                           _sys_humber_l2_fdb_fid_entry_hash_compare);
    if (NULL == pl2_master->fdb_dft_entry_hash)
    {
    goto ERROR_FREE_MEM;
    }

    /* alloc fdb default entry hash size  */
    pl2_master->fdb_dft_entry_hash = ctc_hash_create(SYS_L2_FDB_DFT_ENTRY_HASH_TBL_SIZE / SYS_L2_FDB_DFT_ENTRY_HASH_BLOCK_SIZE, SYS_L2_FDB_DFT_ENTRY_HASH_BLOCK_SIZE,
                                           _sys_humber_l2_fdb_fid_entry_hash_make,
                                           _sys_humber_l2_fdb_fid_entry_hash_compare);
    if (NULL == pl2_master->fdb_dft_entry_hash)
    {
    goto ERROR_FREE_MEM;
    }


    /* alloc fdb mac hash size  */
    pl2_master->fdb_mac_hash = ctc_hash_create(SYS_L2_FDB_MAC_ENTRY_HASH_TBL_SIZE / SYS_L2_FDB_MAC_ENTRY_HASH_BLOCK_SIZE, SYS_L2_FDB_MAC_ENTRY_HASH_BLOCK_SIZE,
                                           _sys_humber_l2_fdb_mac_entry_hash_make,
                                           _sys_humber_l2_fdb_hash_compare);
    if (NULL == pl2_master->fdb_mac_hash)
    {
    goto ERROR_FREE_MEM;
    }


    /* hash collision sram opf init */
    if (hash_collision_sram_size > SYS_L2_FDB_LAST_POOL_BLOCK_MIN_SIZE)
    {
        pool_num =(hash_collision_sram_size -SYS_L2_FDB_LAST_POOL_BLOCK_MIN_SIZE) /SYS_L2_FDB_POOL_BLOCK_SIZE ;
        last_pool_block_size =  hash_collision_sram_size - (pool_num*SYS_L2_FDB_POOL_BLOCK_SIZE) ;
        pool_num = pool_num +1;
    }
    else
    {
        pool_num = 1;
        last_pool_block_size = hash_collision_sram_size;
    }

    pl2_master->offset_pool_num = pool_num;
    pl2_master->pool_status = (uint8 *)mem_malloc(MEM_FDB_MODULE, pool_num*sizeof(uint8));
    if (NULL == pl2_master->pool_status)
    {
        goto ERROR_FREE_MEM;
    }
    kal_memset(pl2_master->pool_status, 0, pool_num*sizeof(uint8));
    sys_humber_opf_init(FDB_SRAM_HASH_COLLISION_KEY, pool_num);
    for (i = 0; i < (pool_num-1); i++)
    {
        opf.pool_type= FDB_SRAM_HASH_COLLISION_KEY;
        opf.pool_index = i;
        start_offset = SYS_L2_FDB_POOL_BLOCK_SIZE*i;
        sys_humber_opf_init_offset(&opf,start_offset, SYS_L2_FDB_POOL_BLOCK_SIZE);
    }
    /*last pool*/
    opf.pool_type= FDB_SRAM_HASH_COLLISION_KEY;
    opf.pool_index = i;
    start_offset = SYS_L2_FDB_POOL_BLOCK_SIZE*i;
    sys_humber_opf_init_offset(&opf,start_offset, last_pool_block_size);


    /*reserve default_entry_num  in last pool*/
    sys_humber_opf_reserve_size_for_reverse_alloc(&opf, p_l2_fdb_global_cfg->default_entry_rsv_num);
    /* alloc mcast group list */
    pl2_master->mcast_grp_list = ctc_list_new();
    if (NULL == pl2_master->mcast_grp_list)
    {
        goto ERROR_FREE_MEM;
    }

    /* set the num of entries need to be deleted one time */
    pl2_master->flush_fdb_cnt_per_loop = p_l2_fdb_global_cfg->flush_fdb_cnt_per_loop;

    /* init fdb vector for storing all fdb entries */
    max_fdb_size = entry_num;
    sys_alloc_get_table_entry_num(DS_MAC_HASH_KEY0, &entry_num);
    max_fdb_size += entry_num;
    pl2_master->max_sram_size = max_fdb_size;
    pl2_master->asic_hash_size = entry_num;
    pl2_master->do_hash_count = 0;

    bucket_num = pl2_master->asic_hash_size/4;
    if(bucket_num > 0)
    {
        /* init hash status for hash bucket*/
        pl2_master->hash_status = (uint8 *)mem_malloc(MEM_FDB_MODULE, bucket_num*sizeof(uint8));
        if (NULL == pl2_master->hash_status)
        {
            goto ERROR_FREE_MEM;
        }

        kal_memset(pl2_master->hash_status, 0, bucket_num*sizeof(uint8));
        _sys_humber_l2_fdb_get_hash_bit_num(bucket_num,&(pl2_master->hash_bit_num));
    }

    vec_block_num = max_fdb_size /SYS_FDB_TBL_VEC_BLOCK_SIZE + ((max_fdb_size%SYS_FDB_TBL_VEC_BLOCK_SIZE) != 0);
    pl2_master->fdb_tbl_vec = ctc_vector_init(vec_block_num, SYS_FDB_TBL_VEC_BLOCK_SIZE);
    if (NULL == pl2_master->fdb_tbl_vec)
    {
        goto ERROR_FREE_MEM;
    }

    vec_block_num = (vec_block_num > 4)?4:vec_block_num;
    ctc_vector_reserve( pl2_master->fdb_tbl_vec,vec_block_num); /*reserved 1~4K fdb vector*/

    /* init port vector for storing port associated  fdb entries*/
    vec_block_num  = SYS_FDB_GPORT_VEC_NUM / SYS_FDB_GPORT_VEC_BLOCK_SIZE;
    pl2_master->gport_vec = ctc_vector_init(vec_block_num, SYS_FDB_GPORT_VEC_BLOCK_SIZE);
    if (NULL == pl2_master->gport_vec)
    {
        goto ERROR_FREE_MEM;
    }
    kal_memset(&hash_rslt_ctl, 0, sizeof(ipe_hash_lookup_result_ctl_t));
    cmd = DRV_IOR(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(0, 0, cmd, &hash_rslt_ctl);

    if (hash_rslt_ctl.mac_da_lookup_table_base_pos)
    {
        pl2_master->hash_base = hash_rslt_ctl.mac_da_lookup_table_base << 8;
    }
    else
    {
        pl2_master->tcam_base = hash_rslt_ctl.mac_da_lookup_table_base << 8;

    }
    _sys_humber_l2_add_sys_default_entry();
    pl2_master->do_hash_no_conflict_mode = 1;

   /*temp for glbal mcast offset*/
   CTC_ERROR_RETURN(sys_alloc_get_met_dsfwd_table_info(&glb_dsmet_entry_num, &local_dsmet_entry_num));
   mcast_group_num = glb_dsmet_entry_num + 1;
   pl2_master->max_group_num = mcast_group_num;
   pl2_master->p_group_array = (uint32 *)mem_malloc(MEM_FDB_MODULE, mcast_group_num*sizeof(uint32));
   if (NULL == pl2_master->p_group_array)
   {
        goto ERROR_FREE_MEM;
   }

    return CTC_E_NONE;

ERROR_FREE_MEM:
    if (pl2_master->l2_mutex)
    {
        kal_mutex_destroy(pl2_master->l2_mutex);
        pl2_master->l2_mutex = NULL;
    }

    if (pl2_master->p_group_array)
    {
        mem_free(pl2_master->p_group_array);
    }

    if (pl2_master->mcast_grp_list)
    {
        ctc_list_free(pl2_master->mcast_grp_list);
    }

    if (pl2_master->fdb_hash)
    {
        ctc_hash_free(pl2_master->fdb_hash);
    }

    if (pl2_master->fdb_dft_entry_hash)
    {
        ctc_hash_free(pl2_master->fdb_dft_entry_hash);
    }

    if (pl2_master->fdb_mac_hash)
    {
        ctc_hash_free(pl2_master->fdb_mac_hash);
    }

    if (pl2_master->fdb_tbl_vec)
    {
        ctc_vector_release(pl2_master->fdb_tbl_vec);
    }

    if (pl2_master->gport_vec)
    {
        ctc_vector_release(pl2_master->gport_vec);
    }

    if (pl2_master->hash_status)
    {
        mem_free(pl2_master->hash_status);
        pl2_master->hash_status = NULL;
    }

    if (pl2_master->pool_status)
    {
        mem_free(pl2_master->pool_status);
        pl2_master->pool_status = NULL;
    }

    if (pl2_master)
    {
        mem_free(pl2_master);
        pl2_master = NULL;
    }

    return CTC_E_NOT_INIT;
}


int32
sys_humber_l2_dump_default_entry(ctc_l2dflt_addr_t* l2dflt_addr)
{
    sys_l2_node_t* p_l2_node = NULL;
    sys_l2_mcast_node_t* mcast_node = NULL;

    L2_FDB_GET_DFT_NODE_BY_FID_PORT(l2dflt_addr->fid, l2dflt_addr->gport, l2dflt_addr->port_valid, p_l2_node);
    if(NULL == p_l2_node)
    {
         CTC_ERROR_RETURN(CTC_E_ENTRY_NOT_EXIST);
    }
    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
    sys_humber_dump_member_list(mcast_node->local_member_list);

    return CTC_E_NONE;
}
int32
sys_humber_l2_dump_l2mc_entry(ctc_l2_mcast_addr_t* l2mc_addr)
{
   sys_l2_node_t* p_l2_node = NULL;
    sys_l2_node_t l2_node_tmp;
     sys_l2_mcast_node_t* mcast_node = NULL;

    l2_node_tmp.key.fid = l2mc_addr->fid;
    kal_memcpy(l2_node_tmp.key.mac, l2mc_addr->mac, sizeof(mac_addr_t));
    l2_node_tmp.flag = 0;

    p_l2_node = _sys_humber_l2_fdb_lkup_from_hash_table(&l2_node_tmp);
    if (NULL == p_l2_node)
    {
      CTC_ERROR_RETURN(CTC_E_ENTRY_NOT_EXIST);
    }
    mcast_node = (sys_l2_mcast_node_t *)(p_l2_node->vlan_entey);
	if (NULL != mcast_node)
	{
        sys_humber_dump_member_list(mcast_node->local_member_list);
	}

    return CTC_E_NONE;
}

