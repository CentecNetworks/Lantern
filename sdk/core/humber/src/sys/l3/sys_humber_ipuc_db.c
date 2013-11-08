/**
 @file sys_humber_ipuc_db.c

 @date 2009-12-09

 @version v2.0

 The file contains all ipuc related function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_debug.h"
#include "ctc_linklist.h"

#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"
#include "sys_humber_ipuc.h"
#include "sys_humber_ipuc_db.h"

#include "drv_humber.h"
#include "drv_io.h"


/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define MIX(a,b,c) \
{ \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
extern sys_ipuc_master_t *p_ipuc_master;

sys_ipuc_db_master_t *p_ipuc_db_master = NULL;
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @brief function of make hash key for ipuc table

 @param[in] p_ipuc_info, information should be maintained for ipuc

 @return CTC_E_XXX
 */
static uint32
_sys_humber_ipv4_hash_make(sys_ipuc_info_t* p_ipuc_info)
{
    uint32 a, b, c;

    /* Set up the internal state */
    a = b = c = 0xdeadbeef + (((uint32)96)<<2);

    a += p_ipuc_info->ip.ipv4.ip32;
    b += p_ipuc_info->tunnel_info.gre_key;
    c += p_ipuc_info->vrf_id;
    c += (p_ipuc_info->masklen << 16);
    c += (p_ipuc_info->tunnel_info.tunnel_packet_type << 24 );
    MIX(a,b,c);

    a += p_ipuc_info->ip_sa.ipv4.ip32;
    b += p_ipuc_info->tunnel_info.is_set_gre_key;
    final(a,b,c);

    return (c & IPUC_IPV4_HASH_MASK);
}

static uint32
_sys_humber_ipv6_hash_make(sys_ipuc_info_t* p_ipuc_info)
{
    uint32 a, b, c;
    /* Set up the internal state */
    a = b = c = 0xdeadbeef + (((uint32)192)<<2);

    a += p_ipuc_info->ip.ipv6.ip32[0];
    b += p_ipuc_info->ip.ipv6.ip32[1];
    c += p_ipuc_info->ip.ipv6.ip32[2];
    MIX(a,b,c);

    a += p_ipuc_info->ip.ipv6.ip32[3];
    b += p_ipuc_info->tunnel_info.gre_key;
    c += p_ipuc_info->vrf_id;
    c += (p_ipuc_info->masklen << 16);
    c += (p_ipuc_info->tunnel_info.tunnel_packet_type << 24 );
    MIX(a,b,c);

    a += p_ipuc_info->ip_sa.ipv6.ip32[0];
    b += p_ipuc_info->ip_sa.ipv6.ip32[1];
    c += p_ipuc_info->ip_sa.ipv6.ip32[2];
    MIX(a,b,c);

    a += p_ipuc_info->ip_sa.ipv6.ip32[3];
    b += p_ipuc_info->tunnel_info.is_set_gre_key;
    final(a,b,c);

    return (c & IPUC_IPV6_HASH_MASK);
}

static bool
_sys_humber_ipv4_hash_cmp(sys_ipuc_info_t* p_ipuc_info1, sys_ipuc_info_t* p_ipuc_info)
{
    if(p_ipuc_info1->vrf_id != p_ipuc_info->vrf_id)
        return FALSE;

    if(p_ipuc_info1->masklen != p_ipuc_info->masklen)
        return FALSE;

    if(p_ipuc_info1->ip.ipv4.ip32 != p_ipuc_info->ip.ipv4.ip32)
        return FALSE;

    if(p_ipuc_info1->ip_sa.ipv4.ip32 != p_ipuc_info->ip_sa.ipv4.ip32)
        return FALSE;

    if(p_ipuc_info1->tunnel_info.tunnel_packet_type!= p_ipuc_info->tunnel_info.tunnel_packet_type)
        return FALSE;

    if(p_ipuc_info1->tunnel_info.gre_key!= p_ipuc_info->tunnel_info.gre_key)
        return FALSE;

    if(p_ipuc_info1->tunnel_info.is_set_gre_key!= p_ipuc_info->tunnel_info.is_set_gre_key)
        return FALSE;

    return TRUE;
}

static bool
_sys_humber_ipv6_hash_cmp(sys_ipuc_info_t* p_ipuc_info1, sys_ipuc_info_t* p_ipuc_info)
{
    if(p_ipuc_info1->vrf_id != p_ipuc_info->vrf_id)
        return FALSE;

    if(p_ipuc_info1->masklen != p_ipuc_info->masklen)
        return FALSE;

    if(p_ipuc_info1->ip.ipv6.ip32[3] != p_ipuc_info->ip.ipv6.ip32[3])
        return FALSE;

    if(p_ipuc_info1->ip.ipv6.ip32[2] != p_ipuc_info->ip.ipv6.ip32[2])
        return FALSE;

    if(p_ipuc_info1->ip.ipv6.ip32[1] != p_ipuc_info->ip.ipv6.ip32[1])
        return FALSE;

    if(p_ipuc_info1->ip.ipv6.ip32[0] != p_ipuc_info->ip.ipv6.ip32[0])
        return FALSE;

    if(p_ipuc_info1->ip_sa.ipv6.ip32[3] != p_ipuc_info->ip_sa.ipv6.ip32[3])
        return FALSE;

    if(p_ipuc_info1->ip_sa.ipv6.ip32[2] != p_ipuc_info->ip_sa.ipv6.ip32[2])
        return FALSE;

    if(p_ipuc_info1->ip_sa.ipv6.ip32[1] != p_ipuc_info->ip_sa.ipv6.ip32[1])
        return FALSE;

    if(p_ipuc_info1->ip_sa.ipv6.ip32[0] != p_ipuc_info->ip_sa.ipv6.ip32[0])
        return FALSE;

    if(p_ipuc_info1->tunnel_info.tunnel_packet_type!= p_ipuc_info->tunnel_info.tunnel_packet_type)
        return FALSE;

    if(p_ipuc_info1->tunnel_info.gre_key!= p_ipuc_info->tunnel_info.gre_key)
        return FALSE;

    if(p_ipuc_info1->tunnel_info.is_set_gre_key!= p_ipuc_info->tunnel_info.is_set_gre_key)
        return FALSE;

    return TRUE;
}

/**
 @brief function of add ip route information

 @param[in] p_ipuc_info, information should be maintained for ipuc

 @return CTC_E_XXX
 */
static int32
_sys_humber_ipuc_db_add(sys_ipuc_info_t* p_ipuc_info)
{
    ctc_hash_insert(p_ipuc_db_master->ipuc_hash[p_ipuc_info->ip_ver], p_ipuc_info);

    return CTC_E_NONE;
}

/**
 @brief function of add ip route information

 @param[in] p_ipuc_info, information should be maintained for ipuc

 @return CTC_E_XXX
 */
static int32
_sys_humber_ipuc_db_remove(sys_ipuc_info_t* p_ipuc_info)
{
    ctc_hash_remove(p_ipuc_db_master->ipuc_hash[p_ipuc_info->ip_ver], p_ipuc_info);

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_db_get_offset(sys_ipuc_info_t* p_ipuc_info)
{

    if( p_ipuc_info->is_tunnel )
    {
        if(p_ipuc_info->ip_ver == CTC_IP_VER_6)
        {
            /*1. init*/
            p_ipuc_db_master->ipuc_tunnel_sort_key_info[p_ipuc_info->ip_ver].block_id = p_ipuc_master->max_mask_len[p_ipuc_info->ip_ver] - p_ipuc_info->masklen;
 
            /*2. do it*/
            CTC_ERROR_RETURN(sys_humber_sort_key_alloc_offset(&p_ipuc_db_master->ipuc_tunnel_sort_key_info[p_ipuc_info->ip_ver], &p_ipuc_info->key_offset));
 
            p_ipuc_info->key_offset += p_ipuc_master->tcam_base[p_ipuc_info->ip_ver];
 
            SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[p_ipuc_info->ip_ver], p_ipuc_info->key_offset) = p_ipuc_info;
        }
        else
        {
            sys_humber_opf_t  opf;
            opf.pool_type = OPF_TUNNEL_IPV4_IPUC;
            opf.pool_index = 0;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &p_ipuc_info->key_offset));
        }
    }
    else
    {
        if(p_ipuc_info->ip_ver == CTC_IP_VER_6)
        {
            /*1. init*/
            p_ipuc_db_master->ipuc_sort_key_info[p_ipuc_info->ip_ver].block_id = p_ipuc_master->max_mask_len[p_ipuc_info->ip_ver] - p_ipuc_info->masklen;
 
            /*2. do it*/
            CTC_ERROR_RETURN(sys_humber_sort_key_alloc_offset(&p_ipuc_db_master->ipuc_sort_key_info[p_ipuc_info->ip_ver], &p_ipuc_info->key_offset));
 
            p_ipuc_info->key_offset += p_ipuc_master->tcam_base[p_ipuc_info->ip_ver];
 
            SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[p_ipuc_info->ip_ver], p_ipuc_info->key_offset) = p_ipuc_info;
        }
        else
        {
            sys_humber_opf_t  opf;
            opf.pool_type = OPF_IPV4_UC_BLOCK;
            opf.pool_index = 0;
            CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &p_ipuc_info->key_offset));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipuc_db_free_offset(sys_ipuc_info_t* p_ipuc_info)
{
    if( p_ipuc_info->is_tunnel )
    {
        if(p_ipuc_info->ip_ver == CTC_IP_VER_6)
        {
            /*1. init*/
            p_ipuc_db_master->ipuc_tunnel_sort_key_info[p_ipuc_info->ip_ver].block_id = p_ipuc_master->max_mask_len[p_ipuc_info->ip_ver] - p_ipuc_info->masklen;

            /*2. do it*/
            SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[p_ipuc_info->ip_ver], p_ipuc_info->key_offset) = NULL;
            p_ipuc_info->key_offset -= p_ipuc_master->tcam_base[p_ipuc_info->ip_ver];

            CTC_ERROR_RETURN(sys_humber_sort_key_free_offset(&p_ipuc_db_master->ipuc_tunnel_sort_key_info[p_ipuc_info->ip_ver],  p_ipuc_info->key_offset));
        }
        else
        {
            sys_humber_opf_t  opf;
            opf.pool_type = OPF_TUNNEL_IPV4_IPUC;
            opf.pool_index = 0;
            CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_ipuc_info->key_offset));        
        }

    }
    else
    {
        if(p_ipuc_info->ip_ver == CTC_IP_VER_6)
        {
            /*1. init*/
            p_ipuc_db_master->ipuc_sort_key_info[p_ipuc_info->ip_ver].block_id = p_ipuc_master->max_mask_len[p_ipuc_info->ip_ver] - p_ipuc_info->masklen;

            /*2. do it*/
            SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[p_ipuc_info->ip_ver], p_ipuc_info->key_offset) = NULL;
            p_ipuc_info->key_offset -= p_ipuc_master->tcam_base[p_ipuc_info->ip_ver];

            CTC_ERROR_RETURN(sys_humber_sort_key_free_offset(&p_ipuc_db_master->ipuc_sort_key_info[p_ipuc_info->ip_ver],  p_ipuc_info->key_offset));
        }
        else
        {
            sys_humber_opf_t  opf;
            opf.pool_type = OPF_IPV4_UC_BLOCK;
            opf.pool_index = 0;
            CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, p_ipuc_info->key_offset));        
        }
    }

    return CTC_E_NONE;
}
int32 _sys_humber_ipuc_db_free_sram(sys_ipuc_info_t* p_ipuc_info)
{
    uint8 index = 0;
    uint32 bucket_index = 0;

    bucket_index = (p_ipuc_info->key_offset - p_ipuc_master->hash_base[p_ipuc_info->ip_ver])/(4>>p_ipuc_info->ip_ver);
    index = (p_ipuc_info->key_offset - p_ipuc_master->hash_base[p_ipuc_info->ip_ver])%(4>>p_ipuc_info->ip_ver);

    if(p_ipuc_master->do_hash_count[p_ipuc_info->ip_ver] > 0)
    {
        p_ipuc_master->do_hash_count[p_ipuc_info->ip_ver]--;
        if(CTC_IP_VER_4 == p_ipuc_info->ip_ver)
        {
           p_ipuc_master->hash_status[bucket_index] &= ~(1<<index);
           if (index == 0 || index == 1 )
           {
              p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver]--;
           }
           else
           {
              p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver]--;
           }
        }
        else
        {
           p_ipuc_master->hash_status[bucket_index] &= ~(3<<(index*2)<<4);
           if (index == 0 )
           {
              p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver]--;
           }
           else
           {
              p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver]--;
           }

        }
    }
    return CTC_E_NONE;
}
int32
sys_humber_ipv4_syn_key(uint32 new_offset, uint32 old_offset)
{
    sys_ipuc_info_t* p_ipuc_info;
    ds_ipv4_ucast_da_t dsipda;
    ds_ipv4_ucast_sa_t dsipsa;
    uint8 chip_num = 0;
    uint8 i;
    uint32 cmdr, cmdw;

    p_ipuc_info = SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_4], old_offset);

    if(NULL == p_ipuc_info)
    {
        return CTC_E_NONE;
    }

    if(new_offset == old_offset)
    {
        return CTC_E_ENTRY_EXIST;
    }

    /* add key to new offset */
    chip_num = sys_humber_get_local_chip_num();
    cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipda));
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipda));
    }

    if (p_ipuc_master->is_ipucsa_allocated)
    {

        cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
        cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
        for (i = 0; i < chip_num; i++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipsa));
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipsa));
        }
    }

    p_ipuc_info->key_offset = new_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];
    CTC_ERROR_RETURN(sys_humber_ipuc_write_key(p_ipuc_info));

    /* remove key from old offset */
    p_ipuc_info->key_offset = old_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];
    CTC_ERROR_RETURN(sys_humber_ipuc_remove_key(p_ipuc_info));

    p_ipuc_info->key_offset = new_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];


    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_4], new_offset) =
        SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_4], old_offset);
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_4], old_offset) = NULL;

    return CTC_E_NONE;
}

int32
sys_humber_ipv4_tunnel_syn_key(uint32 new_offset, uint32 old_offset)
{
    sys_ipuc_info_t* p_ipuc_info;
    ds_ipv4_ucast_da_t dsipda;
    ds_ipv4_ucast_sa_t dsipsa;
    uint8 chip_num = 0;
    uint8 i;
    uint32 cmdr, cmdw;

    p_ipuc_info = SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_4], old_offset);

    if(NULL == p_ipuc_info)
    {
        return CTC_E_NONE;
    }

    if(new_offset == old_offset)
    {
        return CTC_E_ENTRY_EXIST;
    }

    /* add key to new offset */
    chip_num = sys_humber_get_local_chip_num();
    cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipda));
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipda));
    }

    if (p_ipuc_master->is_ipucsa_allocated)
    {

        cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
        cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_4], DRV_ENTRY_FLAG);
        for (i = 0; i < chip_num; i++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipsa));
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipsa));
        }
    }

    p_ipuc_info->key_offset = new_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];
    CTC_ERROR_RETURN(sys_humber_ipuc_write_key(p_ipuc_info));

    /* remove key from old offset */
    p_ipuc_info->key_offset = old_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];
    CTC_ERROR_RETURN(sys_humber_ipuc_remove_key(p_ipuc_info));

    p_ipuc_info->key_offset = new_offset + p_ipuc_master->tcam_base[CTC_IP_VER_4];

    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_4], new_offset) =
        SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_4], old_offset);
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_4], old_offset) = NULL;

    return CTC_E_NONE;
}



int32
sys_humber_ipv6_syn_key(uint32 new_offset, uint32 old_offset)
{
    sys_ipuc_info_t* p_ipuc_info;
    ds_ipv6_ucast_da_t dsipda;
    ds_ipv6_ucast_sa_t dsipsa;
    uint8 chip_num = 0;
    uint8 i;
    uint32 cmdr, cmdw;

    p_ipuc_info = SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_6], old_offset);
    if(NULL == p_ipuc_info)
    {
        return CTC_E_NONE;
    }

    if(new_offset == old_offset)
    {
        return CTC_E_ENTRY_EXIST;
    }

    /* add key to new offset */
    chip_num = sys_humber_get_local_chip_num();
    cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipda));
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipda));
    }
    if (p_ipuc_master->is_ipucsa_allocated)
    {
        cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
        cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
        for (i = 0; i < chip_num; i++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipsa));
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipsa));
        }
    }

    p_ipuc_info->key_offset = new_offset;
    CTC_ERROR_RETURN(sys_humber_ipuc_write_key(p_ipuc_info));

    /* remove key from old offset */
    p_ipuc_info->key_offset = old_offset;
    CTC_ERROR_RETURN(sys_humber_ipuc_remove_key(p_ipuc_info));

    p_ipuc_info->key_offset = new_offset;
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_6], new_offset) =
        SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_6], old_offset);
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_6], old_offset) = NULL;


    return CTC_E_NONE;
}

int32
sys_humber_ipv6_tunnel_syn_key(uint32 new_offset, uint32 old_offset)
{
    sys_ipuc_info_t* p_ipuc_info;
    ds_ipv6_ucast_da_t dsipda;
    ds_ipv6_ucast_sa_t dsipsa;
    uint8 chip_num = 0;
    uint8 i;
    uint32 cmdr, cmdw;

    p_ipuc_info = SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_6], old_offset);
    if(NULL == p_ipuc_info)
    {
        return CTC_E_NONE;
    }

    if(new_offset == old_offset)
    {
        return CTC_E_ENTRY_EXIST;
    }

    /* add key to new offset */
    chip_num = sys_humber_get_local_chip_num();
    cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
    cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipda));
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipda));
    }
    if (p_ipuc_master->is_ipucsa_allocated)
    {
        cmdr = DRV_IOR(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
        cmdw = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[CTC_IP_VER_6], DRV_ENTRY_FLAG);
        for (i = 0; i < chip_num; i++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, old_offset, cmdr, &dsipsa));
            CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_offset, cmdw, &dsipsa));
        }
    }

    p_ipuc_info->key_offset = new_offset;
    CTC_ERROR_RETURN(sys_humber_ipuc_write_key(p_ipuc_info));

    /* remove key from old offset */
    p_ipuc_info->key_offset = old_offset;
    CTC_ERROR_RETURN(sys_humber_ipuc_remove_key(p_ipuc_info));

    p_ipuc_info->key_offset = new_offset;
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_6], new_offset) =
        SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_6], old_offset);
    SORT_KEY_GET_OFFSET_PTR(p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_6], old_offset) = NULL;


    return CTC_E_NONE;
}


/**
 @brief function of lookup ip route information

 @param[in] pp_ipuc_info, information used for lookup ipuc entry
 @param[out] pp_ipuc_info, information of ipuc entry finded

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_db_lookup(sys_ipuc_info_t** pp_ipuc_info)
{
    sys_ipuc_info_t* p_ipuc_info;

    p_ipuc_info = ctc_hash_lookup(p_ipuc_db_master->ipuc_hash[(*pp_ipuc_info)->ip_ver], *pp_ipuc_info);

    *pp_ipuc_info = p_ipuc_info;

    return CTC_E_NONE;
}

int32
_sys_humber_ipuc_db_index_lookup_cb(void * backet_data, void * user_date )
{
    sys_ipuc_info_t* p_ipuc_info_backet ;
    sys_ipuc_info_t** p_ipuc_info_user ;

    p_ipuc_info_backet = (sys_ipuc_info_t*) backet_data;
    p_ipuc_info_user =  (sys_ipuc_info_t**) user_date;

    if(p_ipuc_info_backet->key_offset == (*p_ipuc_info_user)->key_offset )
    {
        *p_ipuc_info_user = backet_data;
        return -1;
    }
    return CTC_E_NONE;
}

/**
 @brief function of lookup ip route information by hash offset

 @param[in] pp_ipuc_info, information used for lookup ipuc entry
 @param[out] pp_ipuc_info, information of ipuc entry finded

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_db_index_lookup(sys_ipuc_info_t** pp_ipuc_info)
{
    int32 ret;
    ret = ctc_hash_traverse(p_ipuc_db_master->ipuc_hash[(*pp_ipuc_info)->ip_ver],
        (hash_traversal_fn) _sys_humber_ipuc_db_index_lookup_cb,pp_ipuc_info);
    if(0 == ret)
    {
        * pp_ipuc_info = NULL;
    }
    return CTC_E_NONE;
}


/**
 @brief function of add ip route information

 @param[in] p_ipuc_info, information should be maintained for ipuc

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_db_add(sys_ipuc_info_t* p_ipuc_info)
{
    if(!p_ipuc_info->in_sram)
    {
        CTC_ERROR_RETURN(sys_humber_ipuc_db_get_offset(p_ipuc_info));
    }

    CTC_ERROR_RETURN(_sys_humber_ipuc_db_add(p_ipuc_info));

    return CTC_E_NONE;
}

/**
 @brief function of remove ip route information

 @param[in] p_ipuc_info, information maintained by ipuc

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_db_remove(sys_ipuc_info_t* p_ipuc_info)
{
    if(!p_ipuc_info->in_sram)
    {
        _sys_humber_ipuc_db_free_offset(p_ipuc_info);
    }
    else
    {
        _sys_humber_ipuc_db_free_sram(p_ipuc_info);
    }

    CTC_ERROR_RETURN(_sys_humber_ipuc_db_remove(p_ipuc_info));

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv4_db_init(void)
{
    uint32 table_size, block_size, ip_tunnel_size;
    sys_sort_block_init_info_t init_info;
    sys_humber_opf_t opf;

    /* 1. data init */
    kal_memset(p_ipuc_db_master->ipv4_blocks, 0, sizeof(p_ipuc_db_master->ipv4_blocks));
    kal_memset(&init_info, 0, sizeof(sys_sort_block_init_info_t));
    init_info.user_data_of = NULL;
    init_info.dir = SYS_SORT_BLOCK_DIR_DOWN;

    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV4_UCAST_ROUTE_KEY, &table_size));
    if(table_size < CTC_IPV4_ADDR_LEN_IN_BIT )
    {
        return CTC_E_NO_RESOURCE;
    }
    /*===============================tunnel init===============================*/
    CTC_ERROR_RETURN(sys_alloc_get_ip_tunnel_size(&ip_tunnel_size));
    if(table_size < ip_tunnel_size )
    {
        return CTC_E_NO_RESOURCE;
    }

    if(ip_tunnel_size < CTC_IPV4_ADDR_LEN_IN_BIT)
    {
        ip_tunnel_size = 0;
    }
    else
    {
        sys_humber_opf_init(OPF_TUNNEL_IPV4_IPUC, CTC_IPV4_ADDR_LEN_IN_BIT + 1);

         opf.pool_type= OPF_TUNNEL_IPV4_IPUC;
         opf.pool_index = 0;
         init_info.opf = &opf;
         init_info.max_offset_num = ip_tunnel_size - 1;
         CTC_ERROR_RETURN(sys_humber_opf_init_offset(init_info.opf, 0, init_info.max_offset_num));
        p_ipuc_master->tunnel_en[CTC_IP_VER_4] = TRUE;
    }

    /*===============================normal IP init===============================*/

    if( table_size - ip_tunnel_size >= CTC_IPV4_ADDR_LEN_IN_BIT)
    {
        /* 3) create avl tree for normal IP every block */
        block_size = ( table_size - ip_tunnel_size) / CTC_IPV4_ADDR_LEN_IN_BIT;
        opf.pool_type= OPF_IPV4_UC_BLOCK;
        opf.pool_index = 0;
        init_info.opf = &opf;
        init_info.max_offset_num = table_size - 1;
        sys_humber_opf_init(OPF_IPV4_UC_BLOCK, CTC_IPV4_ADDR_LEN_IN_BIT + 1);

        CTC_ERROR_RETURN(sys_humber_opf_init_offset(init_info.opf, ip_tunnel_size, init_info.max_offset_num));

        p_ipuc_master->version_en[CTC_IP_VER_4] = TRUE;
    }
    p_ipuc_master->version_en[CTC_IP_VER_4] = (p_ipuc_master->version_en[CTC_IP_VER_4])? 
                (p_ipuc_master->version_en[CTC_IP_VER_4]): p_ipuc_master->tunnel_en[CTC_IP_VER_4];

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv6_db_init()
{
    int32 i;
    uint32 table_size, block_size, ip_tunnel_size;
    sys_sort_block_init_info_t init_info;
    sys_humber_opf_t opf;

    /* 1. data init */
    kal_memset(p_ipuc_db_master->ipv6_blocks, 0, sizeof(p_ipuc_db_master->ipv6_blocks));
    kal_memset(&init_info, 0, sizeof(sys_sort_block_init_info_t));
    init_info.user_data_of = NULL;
    init_info.dir = SYS_SORT_BLOCK_DIR_DOWN;

    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV6_UCAST_ROUTE_KEY, &table_size));
    if(table_size < CTC_IPV6_ADDR_LEN_IN_BIT )
    {
        return CTC_E_NO_RESOURCE;
    }

    CTC_ERROR_RETURN(sys_alloc_get_ip_tunnel_size(&ip_tunnel_size));

    /* tunnel IPv6 init */
    if( (ip_tunnel_size < CTC_IPV6_ADDR_LEN_IN_BIT) || \
        ( table_size - ip_tunnel_size < CTC_IPV6_ADDR_LEN_IN_BIT) )
    {
        ip_tunnel_size = 0;
    }
    else
    {
        block_size = (ip_tunnel_size*2) / CTC_IPV6_ADDR_LEN_IN_BIT;

        sys_humber_opf_init(OPF_TUNNEL_IPV6_IPUC, CTC_IPV6_ADDR_LEN_IN_BIT + 1);
        opf.pool_type= OPF_TUNNEL_IPV6_IPUC;
        init_info.opf = &opf;
        init_info.max_offset_num = ip_tunnel_size*2 - 1;

        /* bolck 0, mask len 128, offset 0-(ipv6_block_size * 64 - 64) */
        init_info.block = &p_ipuc_db_master->ipv6_tunnel_blocks[0];
        init_info.boundary_l = 0;
        init_info.boundary_r = block_size * 64 - 64;
        init_info.is_block_can_shrink = TRUE;
        opf.pool_index = 0;
        CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

        /* block 1-63, mask len 65-127, offset 1 */
        for(i = 1; i < 64; i++)
        {
            init_info.block = &p_ipuc_db_master->ipv6_tunnel_blocks[i];
            init_info.boundary_l = init_info.boundary_r + 1;
            init_info.boundary_r = init_info.boundary_l;
            opf.pool_index++;
            CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));
        }

        /*block 64, mask len 64 offset block_size*/
        init_info.block = &p_ipuc_db_master->ipv6_tunnel_blocks[64];
        init_info.boundary_l = block_size * 64;
        init_info.boundary_r = block_size * (64 + 1) - 1;
        opf.pool_index++;
        CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

        /*block 65 - 127, mask len 1 - 63, offset 1*/
        for (i = 65; i < 128; i++)
        {
            init_info.block = &p_ipuc_db_master->ipv6_tunnel_blocks[i];
            init_info.boundary_l = init_info.boundary_r + 1;
            init_info.boundary_r = init_info.boundary_l;
            opf.pool_index++;
            CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));
        }

        /*block 128, mask len 0, offset block_size block_size * 64 + 64 - 1 ~ last*/
        init_info.block = &p_ipuc_db_master->ipv6_tunnel_blocks[128];
        init_info.boundary_l = block_size * 64 + block_size + (127 - 65 + 1);   /*init_info.boundary_r + 1;*/
        init_info.boundary_r = ip_tunnel_size*2 - 2;
        init_info.dir = SYS_SORT_BLOCK_DIR_UP;
        opf.pool_index++;
        CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

        CTC_ERROR_RETURN(sys_humber_sort_key_init_offset_array(
                             (skinfo_t***)&p_ipuc_db_master->p_ipuc_tunnel_offset_array[CTC_IP_VER_6], table_size));
        p_ipuc_master->tunnel_en[CTC_IP_VER_6] = TRUE;
    }

    /* normal IPv6 init */
    block_size = ( table_size - ip_tunnel_size*2) / CTC_IPV6_ADDR_LEN_IN_BIT;

    sys_humber_opf_init(OPF_IPV6_UC_BLOCK, CTC_IPV6_ADDR_LEN_IN_BIT + 1);
    opf.pool_type= OPF_IPV6_UC_BLOCK;
    init_info.opf = &opf;
    init_info.max_offset_num = table_size - 1;

    /* bolck 0, mask len 128, offset 0-(ipv6_block_size * 64 - 64) */
    init_info.block = &p_ipuc_db_master->ipv6_blocks[0];
    init_info.boundary_l = ip_tunnel_size*2;
    init_info.boundary_r = init_info.boundary_l + block_size * 64 - 64;
    init_info.is_block_can_shrink = TRUE;
    opf.pool_index = 0;
    CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

    /* block 1-63, mask len 65-127, offset 1 */
    for(i = 1; i < 64; i++)
    {
        init_info.block = &p_ipuc_db_master->ipv6_blocks[i];
        init_info.boundary_l = init_info.boundary_r + 1;
        init_info.boundary_r = init_info.boundary_l;
        opf.pool_index++;
        CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));
    }

    /*block 64, mask len 64 offset block_size*/
    init_info.block = &p_ipuc_db_master->ipv6_blocks[64];
    init_info.boundary_l = ip_tunnel_size*2 + block_size * 64;
    init_info.boundary_r = ip_tunnel_size*2 + block_size * (64 + 1) - 1;
    opf.pool_index++;
    CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

    /*block 65 - 127, mask len 1 - 63, offset 1*/
    for (i = 65; i < 128; i++)
    {
        init_info.block = &p_ipuc_db_master->ipv6_blocks[i];
        init_info.boundary_l = init_info.boundary_r + 1;
        init_info.boundary_r = init_info.boundary_l;
        opf.pool_index++;
        CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));
    }

    /*block 128, mask len 0, offset block_size block_size * 64 + 64 - 1 ~ last*/
    init_info.block = &p_ipuc_db_master->ipv6_blocks[128];
    init_info.boundary_l = ip_tunnel_size*2 + block_size * 64 + block_size + (127 - 65 + 1);   /*init_info.boundary_r + 1;*/
    init_info.boundary_r = table_size - 2;
    init_info.dir = SYS_SORT_BLOCK_DIR_UP;
    opf.pool_index++;
    CTC_ERROR_RETURN(sys_humber_sort_key_init_block(&init_info));

    CTC_ERROR_RETURN(sys_humber_sort_key_init_offset_array(
                         (skinfo_t***)&p_ipuc_db_master->p_ipuc_offset_array[CTC_IP_VER_6], table_size));

    p_ipuc_master->version_en[CTC_IP_VER_6] = TRUE;

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_db_init(void)
{
    p_ipuc_db_master = mem_malloc(MEM_IPUC_MODULE, sizeof(sys_ipuc_db_master_t));
    if (NULL == p_ipuc_db_master)
    {
        return CTC_E_NO_MEMORY;
    }

    p_ipuc_db_master->ipuc_hash[CTC_IP_VER_4] = ctc_hash_create(1, (IPUC_IPV4_HASH_MASK + 1),
                                                                _sys_humber_ipv4_hash_make, _sys_humber_ipv4_hash_cmp);
    if ( !p_ipuc_db_master->ipuc_hash[CTC_IP_VER_4])
    {
        return CTC_E_NO_MEMORY;
    }
    p_ipuc_db_master->ipuc_hash[CTC_IP_VER_6] = ctc_hash_create(1, (IPUC_IPV6_HASH_MASK + 1),
                                                                _sys_humber_ipv6_hash_make, _sys_humber_ipv6_hash_cmp);

    if (!p_ipuc_db_master->ipuc_hash[CTC_IP_VER_6])
    {
        return CTC_E_NO_MEMORY;
    }
    _sys_humber_ipv4_db_init();
    _sys_humber_ipv6_db_init();

    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_4].block = p_ipuc_db_master->ipv4_blocks;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_4].max_block_num = CTC_IPV4_ADDR_LEN_IN_BIT + 1;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_4].sort_key_syn_key = sys_humber_ipv4_syn_key;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_4].type = OPF_IPV4_UC_BLOCK;

    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_6].block = p_ipuc_db_master->ipv6_blocks;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_6].max_block_num = CTC_IPV6_ADDR_LEN_IN_BIT + 1;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_6].sort_key_syn_key = sys_humber_ipv6_syn_key;
    p_ipuc_db_master->ipuc_sort_key_info[CTC_IP_VER_6].type = OPF_IPV6_UC_BLOCK;

    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_4].block = p_ipuc_db_master->ipv4_tunnel_blocks;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_4].max_block_num = CTC_IPV4_ADDR_LEN_IN_BIT + 1;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_4].sort_key_syn_key = sys_humber_ipv4_tunnel_syn_key;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_4].type = OPF_TUNNEL_IPV4_IPUC;

    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_6].block = p_ipuc_db_master->ipv6_tunnel_blocks;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_6].max_block_num = CTC_IPV6_ADDR_LEN_IN_BIT + 1;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_6].sort_key_syn_key = sys_humber_ipv6_tunnel_syn_key;
    p_ipuc_db_master->ipuc_tunnel_sort_key_info[CTC_IP_VER_6].type = OPF_TUNNEL_IPV6_IPUC;

    return CTC_E_NONE;
}

int32
sys_humber_show_ipuc_info(sys_ipuc_info_t* p_ipuc_data, void* data)
{
    char buf[CTC_IPV6_ADDR_STR_LEN];
    char buf3[CTC_IPV6_ADDR_STR_LEN];
#define SYS_IPUC_MASK_LEN 7
    char buf2[SYS_IPUC_MASK_LEN];

    kal_snprintf(buf2, SYS_IPUC_MASK_LEN, "/%d", p_ipuc_data->masklen);
    if(p_ipuc_data->ip_ver == CTC_IP_VER_4)
    {
        uint32 tempip = kal_ntohl(p_ipuc_data->ip.ipv4.ip32);
        kal_inet_ntop(AF_INET, &tempip, buf, CTC_IPV6_ADDR_STR_LEN);
        kal_strncat(buf, buf2, SYS_IPUC_MASK_LEN);
        SYS_IPUC_DBG_DUMP("%-5d %-23s", p_ipuc_data->vrf_id, buf);
    }
    else
    {
        uint32 ipv6_address[4] = {0,0,0,0};

        ipv6_address[0] = kal_htonl(p_ipuc_data->ip.ipv6.ip32[0]);
        ipv6_address[1] = kal_htonl(p_ipuc_data->ip.ipv6.ip32[1]);
        ipv6_address[2] = kal_htonl(p_ipuc_data->ip.ipv6.ip32[2]);
        ipv6_address[3] = kal_htonl(p_ipuc_data->ip.ipv6.ip32[3]);

        kal_inet_ntop(AF_INET6, ipv6_address, buf, CTC_IPV6_ADDR_STR_LEN);
        kal_strncat(buf, buf2, SYS_IPUC_MASK_LEN);
        SYS_IPUC_DBG_DUMP("%-5d %-44s", p_ipuc_data->vrf_id, buf);
    }

    buf2[0] = '\0';
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_RPF_CHECK))
    {
        kal_strncat(buf2, "R", 1);
    }
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_TTL_CHECK))
    {
        kal_strncat(buf2, "T", 1);
    }
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_ICMP_CHECK))
    {
        kal_strncat(buf2, "I", 1);
    }
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_CPU))
    {
        kal_strncat(buf2, "C", 1);
    }
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_NEIGHBOR))
    {
        kal_strncat(buf2, "N", 1);
    }
    if(CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_CONNECT))
    {
        kal_strncat(buf2, "X", 1);
    }
    if(p_ipuc_data->is_tunnel)
    {
        kal_strncat(buf2, "U", 1);
    }
    if(p_ipuc_data->ip_ver == CTC_IP_VER_4)
    {
        if(p_ipuc_data->is_ipsa)
        {
            uint32 tempip = kal_ntohl(p_ipuc_data->ip_sa.ipv4.ip32);
            kal_inet_ntop(AF_INET, &tempip, buf3, CTC_IPV6_ADDR_STR_LEN);
            SYS_IPUC_DBG_DUMP("       %-23s\n\r",  buf3);
        }
        else
        {
            SYS_IPUC_DBG_DUMP("       --\n\r");
        }

    }
    else
    {
        if(p_ipuc_data->is_ipsa)
        {
            uint32 ipv6_address[4] = {0,0,0,0};

            ipv6_address[0] = kal_htonl(p_ipuc_data->ip_sa.ipv6.ip32[0]);
            ipv6_address[1] = kal_htonl(p_ipuc_data->ip_sa.ipv6.ip32[1]);
            ipv6_address[2] = kal_htonl(p_ipuc_data->ip_sa.ipv6.ip32[2]);
            ipv6_address[3] = kal_htonl(p_ipuc_data->ip_sa.ipv6.ip32[3]);

            kal_inet_ntop(AF_INET6, ipv6_address, buf3, CTC_IPV6_ADDR_STR_LEN);
            SYS_IPUC_DBG_DUMP("       %-44s\n\r", buf3);
        }
        else
        {
            SYS_IPUC_DBG_DUMP("       --\n\r");
        }
    }
    return CTC_E_NONE;
}

int32
sys_humber_ipuc_db_show(ctc_ip_ver_t ip_ver)
{
    SYS_IPUC_DBG_DUMP("Offset : T-TCAM    S-SRAM\n\r");
    SYS_IPUC_DBG_DUMP("Flags : R-RPF check    T-TTL check    I-ICMP redirect check    C-Send to CPU\n\r");
    SYS_IPUC_DBG_DUMP("        N-Neighbor     X-Connect    U-TUNNEL\n\r");

    if(ip_ver == CTC_IP_VER_4)
    {
        SYS_IPUC_DBG_DUMP("VRF   Route               Ecpn Offset    NHID L3If PLDTP GREFLG GREKEY       Flags   IPSA\n\r");
    }
    else
    {
        SYS_IPUC_DBG_DUMP("VRF   Route                                    Ecpn Offset    NHID L3If PLDTP GREFLG GREKEY       Flags   IPSA\n\r\n\r");
    }

    ctc_hash_traverse(p_ipuc_db_master->ipuc_hash[ip_ver], (hash_traversal_fn)sys_humber_show_ipuc_info, NULL);

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_db_show_count(void)
{
    uint32 i, j, tcam_count,tcam_count2;
    uint8 * p_tmp;

    for(i = 0; i < MAX_CTC_IP_VER; i++)
    {
        tcam_count = 0;
        tcam_count2 = 0;
        for(j = 0; j < p_ipuc_db_master->ipuc_sort_key_info[i].max_block_num; j++)
        {
            tcam_count += p_ipuc_db_master->ipuc_sort_key_info[i].block[j].used_of_num;
        }
        for(j = 0; j < p_ipuc_db_master->ipuc_tunnel_sort_key_info[i].max_block_num; j++)
        {
            tcam_count2 += p_ipuc_db_master->ipuc_tunnel_sort_key_info[i].block[j].used_of_num;
        }
        SYS_IPUC_DBG_DUMP("IPv%c total %d routes, %d using tcam, %d tunnel using tcam.\r\n",
            (i==CTC_IP_VER_4)?'4':'6', p_ipuc_db_master->ipuc_hash[i]->count, tcam_count,tcam_count2);
    }

    SYS_IPUC_DBG_DUMP("IPv4 left hash use %d ; right hash use %d  ; total use %d\r\n"
            "IPv6 left hash use %d ; right hash use %d  ; total use %d\r\n",
            p_ipuc_master->do_left_hash_count[CTC_IP_VER_4],
            p_ipuc_master->do_right_hash_count[CTC_IP_VER_4],
            p_ipuc_master->do_hash_count[CTC_IP_VER_4],
            p_ipuc_master->do_left_hash_count[CTC_IP_VER_6],
            p_ipuc_master->do_right_hash_count[CTC_IP_VER_6],
            p_ipuc_master->do_hash_count[CTC_IP_VER_6]);

    j = 0;
    tcam_count2 = 0;
    SYS_IPUC_DBG_DUMP("          0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\r\n");
    p_tmp = p_ipuc_master->hash_status;
    SYS_IPUC_DBG_DUMP("0x%03x :",tcam_count2);
    for(i = 0 ; i< p_ipuc_master->hash_array_num ; i ++)
    {
        tcam_count = *p_tmp;
        p_tmp++;
        tcam_count &= 0xFF;
        SYS_IPUC_DBG_DUMP("0x%02x ",tcam_count);
        j++;
        if( (p_ipuc_master->hash_array_num  - i != 1) & (j == 16))
        {
            tcam_count2++;
            SYS_IPUC_DBG_DUMP("\r\n0x%03x :",tcam_count2);
            j = 0;
        }
    }
    SYS_IPUC_DBG_DUMP("\r\n");
    return CTC_E_NONE;
}

int32
sys_humber_ipuc_offset_show(uint8 flag, uint8 blockid)
{
    sys_sort_block_t* p_block;
    sys_humber_opf_t opf;
    ctc_ip_ver_t ip_ver = 0;

    ip_ver = flag & 0x01;

    if(blockid > p_ipuc_master->max_mask_len[ip_ver])
    {
        SYS_IPUC_DBG_DUMP("Error Block Index\n\r");
        return CTC_E_NONE;
    }

    if( 0 != (flag & 0x10))
    {
        p_block = &p_ipuc_db_master->ipuc_tunnel_sort_key_info[ip_ver].block[blockid];
    }
    else
    {
        p_block = &p_ipuc_db_master->ipuc_sort_key_info[ip_ver].block[blockid];
    }

    SYS_IPUC_DBG_DUMP("All From %d To %d Init From %d To %d All %d Use %d Dir %d shrink %d\n\r",
        p_block->all_left_b, p_block->all_right_b, p_block->init_left_b, p_block->init_right_b,
        p_block->all_of_num, p_block->used_of_num, p_block->preferred_dir, p_block->is_block_can_shrink);

    opf.pool_type = p_ipuc_db_master->ipuc_sort_key_info[ip_ver].type;
    opf.pool_index = blockid;
    sys_humber_opf_print_sample_info(&opf);

    return CTC_E_NONE;
}

