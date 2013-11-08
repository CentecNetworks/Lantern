/**
 @file sys_humber_ipuc.c

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

#include "ctc_ipuc.h"
#include "sys_humber_chip.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_ipuc.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_ipuc_db.h"
#include "sys_humber_ftm.h"
#include "sys_humber_pdu.h"
#include "sys_humber_parser_io.h"
#include "sys_humber_hash.h"

#include "drv_humber.h"

#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
sys_ipuc_master_t *p_ipuc_master = NULL;

#define SYS_IPUC_CREAT_LOCK                   \
    {                                             \
        kal_mutex_create(&p_ipuc_master->mutex); \
        if(NULL == p_ipuc_master->mutex)  \
        { \
            CTC_ERROR_RETURN(CTC_E_FAIL_CREATE_MUTEX); \
        } \
    }

#define SYS_IPUC_LOCK \
    kal_mutex_lock(p_ipuc_master->mutex)

#define SYS_IPUC_UNLOCK \
    kal_mutex_unlock(p_ipuc_master->mutex)

#define CTC_ERROR_RETURN_IPUC_UNLOCK(op) \
    { \
        int32 rv = (op); \
        if (rv < 0) \
        { \
            kal_mutex_unlock(p_ipuc_master->mutex);\
            return(rv); \
        } \
    }

#define CTC_RETURN_IPUC_UNLOCK(op) \
    { \
        kal_mutex_unlock(p_ipuc_master->mutex);\
        return(op); \
    }

/****************************************************************************
 *
* Function
*
*****************************************************************************/
static int32
_sys_humber_l3_asic_hash_lkp_by_direction(uint8 chip_id,
                             sys_ipuc_info_t* p_ipuc_info, uint8 left,
                             hash_ds_ctl_cpu_key_status_t* hash_cpu_status)
{
    uint32 bucket_left = 0, bucket_right = 0;
    uint8 hash_key[18] = {0};
    uint8 bucket_status = 0;

    SYS_IPUC_HASH_KEY_MAP(hash_key,p_ipuc_info);
    hash_cpu_status->cpu_lu_index = 0;

    if (left)
    {
        bucket_left = p_ipuc_master->cal_hash_index[p_ipuc_info->ip_ver][SYS_IPUC_HASH_LEFT]
                        ((uint8 *)hash_key, p_ipuc_master->hash_bit_num[p_ipuc_info->ip_ver]);
        bucket_status = (p_ipuc_master->hash_status[bucket_left] >> (4*p_ipuc_info->ip_ver)) &0x03;
        if (bucket_status != 3)
        {
            if (bucket_status == 2 || bucket_status == 0)
            {
                if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
                {
                    hash_cpu_status->cpu_lu_index = bucket_left*4  + 0;
                    p_ipuc_master->hash_status[bucket_left] |= 1;
                }
                else
                {
                    hash_cpu_status->cpu_lu_index = bucket_left*2  + 0;
                    p_ipuc_master->hash_status[bucket_left]  |= 3<<4;
                }
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_left*4 + 1;
                p_ipuc_master->hash_status[bucket_left]  |= 1 << 1;
            }
        }
        else
        {
            hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
        }
    }
    else
    {
        bucket_right = p_ipuc_master->cal_hash_index[p_ipuc_info->ip_ver][SYS_IPUC_HASH_RIGHT]
                        ((uint8 *)hash_key, p_ipuc_master->hash_bit_num[p_ipuc_info->ip_ver]);
        bucket_status = (p_ipuc_master->hash_status[bucket_right] >> 2 >> (4*p_ipuc_info->ip_ver)) & 0x03;

        if (bucket_status != 3)
        {
            if (bucket_status == 2 || bucket_status == 0)
            {
                if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
                {
                    hash_cpu_status->cpu_lu_index = bucket_right*4 + 2;
                    p_ipuc_master->hash_status[bucket_right] |= 1 << 2 ;
                }
                else
                {
                    hash_cpu_status->cpu_lu_index = bucket_right*2  + 1;
                    p_ipuc_master->hash_status[bucket_right]  |= 3<<4<<2;
                }
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_right*4 + 3;
                p_ipuc_master->hash_status[bucket_right] |= 1 << 3;
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
_sys_humber_ipuc_write_ipsa(sys_ipuc_info_t* p_ipuc_info, sys_rpf_info_t* p_rpf_info)
{
    uint8 chip_num = 0;
    ds_ipv4_ucast_sa_t dsipsa;
    uint8 i;
    uint32 cmd;

    if (0 == p_ipuc_master->is_ipucsa_allocated)
    {
        return CTC_E_NONE;
    }

    kal_memset(&dsipsa, 0, sizeof(ds_ipv4_ucast_sa_t));
    dsipsa.if_id0 = p_rpf_info[0].oif_id;
    dsipsa.if_id1 = p_rpf_info[1].oif_id;
    dsipsa.if_id2 = p_rpf_info[2].oif_id;
    dsipsa.if_id3 = p_rpf_info[3].oif_id;
    dsipsa.if_id_valid0 = (p_rpf_info[0].ref > 0 ? 1 : 0);
    dsipsa.if_id_valid1 = (p_rpf_info[1].ref > 0 ? 1 : 0);
    dsipsa.if_id_valid2 = (p_rpf_info[2].ref > 0 ? 1 : 0);
    dsipsa.if_id_valid3 = (p_rpf_info[3].ref > 0 ? 1 : 0);
    dsipsa.check_en = CTC_FLAG_ISSET(p_ipuc_info->route_flag, CTC_IPUC_FLAG_RPF_CHECK);
    if(dsipsa.check_en && p_rpf_info[4].ref > 0)
    {
        if(p_ipuc_master->cpu_rpf)
        {
            dsipsa.ipsa_more_rpf_if = 1;
        }
        else
        {
            dsipsa.check_en = 0;
        }
    }

    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_ipuc_info->key_offset, cmd, &dsipsa));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipuc_write_ipda(sys_ipuc_info_t* p_ipuc_info, uint16 oif_id, sys_nh_offset_array_t fwd_offset)
{
    ds_ipv4_ucast_da_t dsipda;
    uint8 chip_num = 0;
    uint8 i;
    uint32 cmd;

    kal_memset(&dsipda, 0, sizeof(ds_ipv4_ucast_da_t));
    dsipda.ttl_check_en = CTC_FLAG_ISSET(p_ipuc_info->route_flag, CTC_IPUC_FLAG_TTL_CHECK);
    dsipda.icmp_check_en = CTC_FLAG_ISSET(p_ipuc_info->route_flag, CTC_IPUC_FLAG_ICMP_CHECK);
    dsipda.exp3_ctl_en = CTC_FLAG_ISSET(p_ipuc_info->route_flag, CTC_IPUC_FLAG_PROTOCOL_ENTRY);
    dsipda.ip_da_exception_en = CTC_FLAG_ISSET(p_ipuc_info->route_flag, CTC_IPUC_FLAG_CPU);
    if(dsipda.ip_da_exception_en)
    {
        dsipda.excep_sub_index = SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU; /*must be equal to 15,refer to sys_humber_pdu.h :SYS_L3PDU_PER_L3IF_ACTION_INDEX_RSV_IPDA_TO_CPU*/
    }

    dsipda.vrf_id = oif_id & 0xfff;
    dsipda.equal_cost_path_num = p_ipuc_info->ecpn & 0x3;
    dsipda.equal_cost_path_num2 = p_ipuc_info->ecpn >> 2;

    if(p_ipuc_info->is_tunnel)
    {
        dsipda.tunnel_payload_offset_type = 0;
        dsipda.tunnel_packet_type = p_ipuc_info->tunnel_info.tunnel_packet_type;

        if(CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE == p_ipuc_info->tunnel_info.tunnel_packet_type)
        {
            dsipda.payload_select = 3;
            if(p_ipuc_info->tunnel_info.is_set_gre_key)
            {
                /* gre key is not be striped */
                dsipda.tunnel_payload_offset = 4;
                dsipda.tunnel_gre_options = 0x2;
            }
            else
            {
                dsipda.tunnel_payload_offset = 4;
                dsipda.tunnel_gre_options = 0x0;
            }
        }
        else
        {
            if(CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV6 == p_ipuc_info->tunnel_info.tunnel_packet_type)
            {
                dsipda.tunnel_packet_type = IPV6_PKT;
            }
            else if(CTC_IPUC_TUNNEL_PAYLOAD_TYPE_TCP == p_ipuc_info->tunnel_info.tunnel_packet_type ||
                CTC_IPUC_TUNNEL_PAYLOAD_TYPE_UDP == p_ipuc_info->tunnel_info.tunnel_packet_type||
                CTC_IPUC_TUNNEL_PAYLOAD_TYPE_IPV4 == p_ipuc_info->tunnel_info.tunnel_packet_type)
            {
                dsipda.tunnel_packet_type = IPV4_PKT;
            }else
            {
                dsipda.tunnel_packet_type = RESERVED;
            }
            dsipda.payload_select = 2;
            dsipda.tunnel_payload_offset = 0;
        }
    }

    chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        dsipda.ds_fwd_ptr = fwd_offset[i] & 0xfffff;
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_ipuc_info->key_offset, cmd, &dsipda));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv4_write_key(sys_ipuc_info_t* p_ipuc_info, void* p_key)
{
    ds_ipv4_ucast_route_key_t *p_dsipkey, *p_dsipkeymask;
    tbl_entry_t* p_tbl_ipkey = p_key;
    uint32 cmd = 0;
    ipe_lookup_ctl_t ipe_lookup_ctl;

    p_dsipkey = (ds_ipv4_ucast_route_key_t*)p_tbl_ipkey->data_entry;
    p_dsipkeymask = (ds_ipv4_ucast_route_key_t*)p_tbl_ipkey->mask_entry;

    kal_memset(p_dsipkey, 0, sizeof(ds_ipv4_ucast_route_key_t));
    kal_memset(p_dsipkeymask, 0, sizeof(ds_ipv4_ucast_route_key_t));
    p_dsipkey->vrf_idl = p_ipuc_info->vrf_id & 0xf;
    p_dsipkey->vrf_idh = p_ipuc_info->vrf_id >> 4;
    p_dsipkey->ip_da = p_ipuc_info->ip.ipv4.ip32;
    p_dsipkey->table_id0 = UCAST_IPV4_TABLEID0_C;
    p_dsipkey->table_id1 = UCAST_IPV4_TABLEID0_C;


    if(p_ipuc_info->is_tunnel)
    {
        p_dsipkey->layer4_type = p_ipuc_info->tunnel_info.tunnel_packet_type;

        if(CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE == p_ipuc_info->tunnel_info.tunnel_packet_type)
        {
            /*80bits*/
            p_dsipkey->ip_sa = p_ipuc_info->tunnel_info.gre_key;
            p_dsipkeymask->ip_sa = 0xffffffff;

            /*160bits*/
            p_dsipkey->l4_dest_port = p_ipuc_info->tunnel_info.gre_key >> 16;
            p_dsipkey->l4_source_port = p_ipuc_info->tunnel_info.gre_key & 0xffff;
            p_dsipkeymask->l4_source_port = 0xffff;
            p_dsipkeymask->l4_dest_port = 0xffff;

            if (p_ipuc_info->tunnel_info.is_set_gre_key)
            {
                if (!p_ipuc_info->tunnel_info.gre_key)
                {
                    p_dsipkeymask->ip_sa = 0;
                    p_dsipkeymask->l4_source_port = 0;
                    p_dsipkeymask->l4_dest_port = 0;
                }
            }
            else
            {
                p_dsipkey->ip_sa = 0;
                p_dsipkey->l4_dest_port = 0;
                p_dsipkey->l4_source_port = 0;
            }
        }

        p_dsipkeymask->layer4_type = 0xf;
    }


    if(p_ipuc_info->vrf_id)
    {
        p_dsipkey->lkp_mode =  1;
        p_dsipkey->ip_sa = p_ipuc_info->vrf_id;
        p_dsipkeymask->ip_sa = 0xffff;
    }

    /*ipsa*/
    if( p_ipuc_info->is_tunnel )
    {
        cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &ipe_lookup_ctl));

        if( p_ipuc_info->is_ipsa  )
        {
            p_dsipkey->ip_sa = p_ipuc_info->ip_sa.ipv4.ip32;
            p_dsipkeymask->ip_sa = 0xffffffff;
        }
        else
        {
            if( ipe_lookup_ctl.ipv4_ucast_route_key_sa_en )
            {
                p_dsipkey->ip_sa = 0;
                p_dsipkeymask->ip_sa = 0xffffffff;
            }
        }
    }

    p_dsipkeymask->table_id0 = 0xf;
    p_dsipkeymask->table_id1 = 0xf;
    if(!CTC_FLAG_ISSET(p_ipuc_info->route_flag, SYS_IPUC_FLAG_DEFAULT))
    {
        p_dsipkeymask->lkp_mode = 1;
        p_dsipkeymask->vrf_idl = 0xf;
        p_dsipkeymask->vrf_idh = 0xfff;
        IPV4_LEN_TO_MASK(p_dsipkeymask->ip_da, p_ipuc_info->masklen);
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv6_write_key(sys_ipuc_info_t* p_ipuc_info, void* p_key)
{
    ds_ipv6_ucast_route_key_t *p_dsipkey, *p_dsipkeymask;
    ipv6_addr_t   ipv6_mask;
    tbl_entry_t* p_tbl_ipkey = p_key;

    p_dsipkey = (ds_ipv6_ucast_route_key_t*)p_tbl_ipkey->data_entry;
    p_dsipkeymask = (ds_ipv6_ucast_route_key_t*)p_tbl_ipkey->mask_entry;
    kal_memset(p_dsipkey, 0, sizeof(ds_ipv6_ucast_route_key_t));
    kal_memset(p_dsipkeymask, 0, sizeof(ds_ipv6_ucast_route_key_t));

    p_dsipkey->vrf_idl = p_ipuc_info->vrf_id & 0xf;
    p_dsipkey->vrf_idh = p_ipuc_info->vrf_id >> 4;
    p_dsipkey->ip_da0 = p_ipuc_info->ip.ipv6.ip32[0];
    p_dsipkey->ip_da1 = p_ipuc_info->ip.ipv6.ip32[1];
    p_dsipkey->ip_da2 = p_ipuc_info->ip.ipv6.ip32[2] & 0xff;
    p_dsipkey->ip_da3 = (p_ipuc_info->ip.ipv6.ip32[2] >> 8) | (p_ipuc_info->ip.ipv6.ip32[3] << 24);
    p_dsipkey->ip_da4 = p_ipuc_info->ip.ipv6.ip32[3] >> 8;
    p_dsipkey->table_id0 = UCAST_IPV6_TABLEID0_C;
    p_dsipkey->table_id1 = UCAST_IPV6_TABLEID0_C;
    p_dsipkey->table_id2 = UCAST_IPV6_TABLEID0_C;
    p_dsipkey->table_id3 = UCAST_IPV6_TABLEID0_C;
    p_dsipkey->table_id4 = UCAST_IPV6_TABLEID1_C;
    p_dsipkey->table_id5 = UCAST_IPV6_TABLEID1_C;
    p_dsipkey->table_id6 = UCAST_IPV6_TABLEID1_C;
    p_dsipkey->ipv6_rtk_table_id7 = UCAST_IPV6_TABLEID1_C;

    if(p_ipuc_info->is_tunnel)
    {
        p_dsipkey->layer4_type = p_ipuc_info->tunnel_info.tunnel_packet_type;

         if(CTC_IPUC_TUNNEL_PAYLOAD_TYPE_GRE == p_ipuc_info->tunnel_info.tunnel_packet_type)
        {
            p_dsipkey->ip_sa0 = p_ipuc_info->tunnel_info.gre_key;
            p_dsipkeymask->ip_sa0 = 0xffffffff;

            p_dsipkey->l4_dest_port = (p_ipuc_info->tunnel_info.gre_key >> 16);
            p_dsipkey->l4_source_port = (p_ipuc_info->tunnel_info.gre_key & 0xffff);
            p_dsipkeymask->l4_dest_port = 0xffff;
            p_dsipkeymask->l4_source_port = 0xffff;
        }

        p_dsipkeymask->layer4_type = 0xf;

        if(p_ipuc_info->is_ipsa)
        {
            p_dsipkey->ip_sa0 = p_ipuc_info->ip_sa.ipv6.ip32[0];
            p_dsipkey->ip_sa1 = p_ipuc_info->ip_sa.ipv6.ip32[1];
            p_dsipkey->ip_sa2 = p_ipuc_info->ip_sa.ipv6.ip32[2] & 0xff;
            p_dsipkey->ip_sa3 = (p_ipuc_info->ip_sa.ipv6.ip32[2] >> 8) | (p_ipuc_info->ip_sa.ipv6.ip32[3] << 24);
            p_dsipkey->ip_sa4 = p_ipuc_info->ip_sa.ipv6.ip32[3] >> 8;

            p_dsipkeymask->ip_sa0 = 0xffffffff;
            p_dsipkeymask->ip_sa1 = 0xffffffff;
            p_dsipkeymask->ip_sa2 = 0xff;
            p_dsipkeymask->ip_sa3 = 0xffffffff;
            p_dsipkeymask->ip_sa4 = 0xffffff;
        }

    }

    p_dsipkeymask->table_id0 = 0xf;
    p_dsipkeymask->table_id1 = 0xf;
    p_dsipkeymask->table_id2 = 0xf;
    p_dsipkeymask->table_id3 = 0xf;
    p_dsipkeymask->table_id4 = 0xf;
    p_dsipkeymask->table_id5 = 0xf;
    p_dsipkeymask->table_id6 = 0xf;
    p_dsipkeymask->ipv6_rtk_table_id7 = 0xf;
    if(!CTC_FLAG_ISSET(p_ipuc_info->route_flag, SYS_IPUC_FLAG_DEFAULT))
    {
        p_dsipkeymask->vrf_idl = 0xf;
        p_dsipkeymask->vrf_idh = 0xfff;
        IPV6_LEN_TO_MASK(ipv6_mask, p_ipuc_info->masklen);
        p_dsipkeymask->ip_da0 = ipv6_mask[0];
        p_dsipkeymask->ip_da1 = ipv6_mask[1];
        p_dsipkeymask->ip_da2 = ipv6_mask[2] & 0xff;
        p_dsipkeymask->ip_da3 = (ipv6_mask[2] >> 8) | (ipv6_mask[3] << 24);
        p_dsipkeymask->ip_da4 = ipv6_mask[3] >> 8;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv4_hash_write_key(sys_ipuc_info_t* p_ipuc_info, void* p_ipkey)
{
    ds_ipv4_ucast_hash_key0_t *p_dsipkey;

    p_dsipkey = p_ipkey;

    kal_memset(p_dsipkey, 0, sizeof(ds_ipv4_ucast_hash_key0_t));
    p_dsipkey->key_vrfid = p_ipuc_info->vrf_id;
    p_dsipkey->key_mapped_ip = p_ipuc_info->ip.ipv4.ip32;

    return CTC_E_NONE;
}

static int32
_sys_humber_ipv6_hash_write_key(sys_ipuc_info_t* p_ipuc_info, void* p_ipkey)
{
    ds_ipv6_ucast_hash_key0_t *p_dsipkey;

    p_dsipkey = p_ipkey;

    kal_memset(p_dsipkey, 0, sizeof(ds_ipv6_ucast_hash_key0_t));
    p_dsipkey->vrfid0 = p_ipuc_info->vrf_id & 0x7;
    p_dsipkey->vrfid1 = (p_ipuc_info->vrf_id >> 3) & 0x7;
    p_dsipkey->vrfid2 = (p_ipuc_info->vrf_id >> 6) & 0x7;
    p_dsipkey->vrfid3 = (p_ipuc_info->vrf_id >> 9) & 0x7;
    p_dsipkey->key_ipda0 = p_ipuc_info->ip.ipv6.ip32[0];
    p_dsipkey->key_ipda1 = p_ipuc_info->ip.ipv6.ip32[1];
    p_dsipkey->key_ipda2 = p_ipuc_info->ip.ipv6.ip32[2];
    p_dsipkey->key_ipda3 = p_ipuc_info->ip.ipv6.ip32[3];

    return CTC_E_NONE;
}

/**
 @brief function of add ipuc key entry

 @param[in] p_ipuc_info, information used to add ipuc key

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_write_key(sys_ipuc_info_t* p_ipuc_info)
{
    uint8 chip_num = 0;
    uint8 lchip;

    chip_num = sys_humber_get_local_chip_num();
    if(p_ipuc_info->in_sram)
    {
        ds_ipv6_ucast_hash_key0_t dsip_hashkey;

        p_ipuc_master->write_hash_key[p_ipuc_info->ip_ver](p_ipuc_info, &dsip_hashkey);

        for(lchip = 0; lchip < chip_num; lchip++)
        {
            CTC_ERROR_RETURN(drv_hash_key_ioctl(lchip, p_ipuc_master->hashkey_table_id[p_ipuc_info->ip_ver],
                p_ipuc_info->key_offset - p_ipuc_master->hash_base[p_ipuc_info->ip_ver], (uint32 *)&dsip_hashkey, HASH_OP_TP_ADD_ENTRY));
        }
    }
    else
    {
        uint32 cmd;
        tbl_entry_t tbl_ipkey;
        ds_ipv6_ucast_route_key_t dsipkey, dsipkeymask;

        tbl_ipkey.data_entry = (uint32*)&dsipkey;
        tbl_ipkey.mask_entry = (uint32*)&dsipkeymask;

        p_ipuc_master->write_key[p_ipuc_info->ip_ver](p_ipuc_info, &tbl_ipkey);
        cmd = DRV_IOW(IOC_TABLE, p_ipuc_master->key_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
        for(lchip = 0; lchip < chip_num; lchip++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, p_ipuc_info->key_offset - p_ipuc_master->tcam_base[p_ipuc_info->ip_ver], cmd, &tbl_ipkey));
        }
    }

    return CTC_E_NONE;
}

/**
 @brief function of remove ipuc key entry

 @param[in] p_ipuc_info, information used to remove ipuc key

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_remove_key(sys_ipuc_info_t* p_ipuc_info)
{
    uint8 chip_num = 0;
    uint8 lchip = 0;
    ds_ipv6_ucast_hash_key0_t dsip_hashkey;

    if(p_ipuc_info->in_sram)
    {
        p_ipuc_master->write_hash_key[p_ipuc_info->ip_ver](p_ipuc_info, &dsip_hashkey);
    }

    chip_num = sys_humber_get_local_chip_num();
    for(lchip = 0; lchip < chip_num; lchip++)
    {
        if(p_ipuc_info->in_sram)
        {
            drv_hash_key_ioctl(lchip, p_ipuc_master->hashkey_table_id[p_ipuc_info->ip_ver], p_ipuc_info->key_offset - p_ipuc_master->hash_base[p_ipuc_info->ip_ver], (uint32 *)&dsip_hashkey, HASH_OP_TP_DEL_ENTRY_BY_INDEX);
        }

        else
        {
            drv_tcam_tbl_remove(lchip, p_ipuc_master->key_table_id[p_ipuc_info->ip_ver], p_ipuc_info->key_offset - p_ipuc_master->tcam_base[p_ipuc_info->ip_ver]);
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_ipuc_move_entry(sys_ipuc_info_t* p_ipuc_info, uint32 new_index)
{
    /* move old key and ad to new position */

    /* move ipda entry */
    ds_ipv4_ucast_da_t dsipda;
    ds_ipv4_ucast_sa_t dsipsa;
    ds_ipv6_ucast_hash_key0_t dsip_hashkey;
    uint8 chip_num = 0;
    uint8 i = 0;
    uint32 cmd;
    chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOR(IOC_TABLE, p_ipuc_master->da_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_ipuc_info->key_offset, cmd, &dsipda));

    cmd = DRV_IOW(IOC_TABLE, p_ipuc_master->da_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_index, cmd, &dsipda));
    }

    /* move ipsa entry */
    i = 0;
    cmd = DRV_IOR(IOC_TABLE, p_ipuc_master->sa_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(i, p_ipuc_info->key_offset, cmd, &dsipsa));

    cmd = DRV_IOW(IOC_TABLE, p_ipuc_master->sa_table_id[p_ipuc_info->ip_ver], DRV_ENTRY_FLAG);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(i, new_index, cmd, &dsipsa));
    }

    /* move ipuc key entry */
    i = 0;
    p_ipuc_master->write_hash_key[p_ipuc_info->ip_ver](p_ipuc_info, &dsip_hashkey);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_hash_key_ioctl(i, p_ipuc_master->hashkey_table_id[p_ipuc_info->ip_ver],
            new_index - p_ipuc_master->hash_base[p_ipuc_info->ip_ver], (uint32 *)&dsip_hashkey, HASH_OP_TP_ADD_ENTRY));
    }

    p_ipuc_master->write_hash_key[p_ipuc_info->ip_ver](p_ipuc_info, &dsip_hashkey);
    for(i = 0; i < chip_num; i++)
    {
        CTC_ERROR_RETURN(drv_hash_key_ioctl(i, p_ipuc_master->hashkey_table_id[p_ipuc_info->ip_ver],
            p_ipuc_info->key_offset - p_ipuc_master->hash_base[p_ipuc_info->ip_ver], (uint32 *)&dsip_hashkey, HASH_OP_TP_DEL_ENTRY_BY_INDEX));
    }
    p_ipuc_info->key_offset = new_index;
    return CTC_E_NONE;
}

static int32
_sys_humber_l3_asic_hash_lkp(uint8 chip_id, sys_ipuc_info_t* p_ipuc_info,
                                    hash_ds_ctl_cpu_key_status_t* hash_cpu_status)
{
    uint32 bucket_left = 0, bucket_right = 0;
    uint8 hash_key[18] = {0};
    uint8 bucket_left_status = 0,bucket_right_status =0;
    uint8 bucket_left_cnt = 0,bucket_right_cnt =0;
    uint8 use_left_block = 0;
    uint8 find_flag = 0;
    sys_ipuc_info_t * p_hash_node;
    sys_ipuc_info_t ipuc_data;
    hash_ds_ctl_cpu_key_status_t  hash_ds_ctl_status;

    kal_memset(&hash_ds_ctl_status,0,sizeof(hash_ds_ctl_cpu_key_status_t));

    SYS_IPUC_HASH_KEY_MAP(hash_key,p_ipuc_info);
    p_hash_node = &ipuc_data;
    SYS_IPUC_KEY_MAP(p_ipuc_info, p_hash_node);

    hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
    if( p_ipuc_master->hash_bit_num[p_ipuc_info->ip_ver] == SYS_IPUC_ERROR_BIT_NUM)
    {
        return CTC_E_NONE;
    }

    bucket_left = p_ipuc_master->cal_hash_index[p_ipuc_info->ip_ver][SYS_IPUC_HASH_LEFT]
        (hash_key, p_ipuc_master->hash_bit_num[p_ipuc_info->ip_ver]);
    bucket_left_status = (p_ipuc_master->hash_status[bucket_left] >>(4*p_ipuc_info->ip_ver)) &0x03;

    bucket_right = p_ipuc_master->cal_hash_index[p_ipuc_info->ip_ver][SYS_IPUC_HASH_RIGHT]
        (hash_key, p_ipuc_master->hash_bit_num[p_ipuc_info->ip_ver]);
    bucket_right_status = (p_ipuc_master->hash_status[bucket_right]>>(4*p_ipuc_info->ip_ver) >> 2) & 0x03;

    bucket_left_cnt   = 0;
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
    {  /*[0:0] [1:1] [0:2][1:2]  [2:0]  [2:1]  [2:2] */
         if ((bucket_left_status != 3 ) || ( bucket_right_status != 3)) /*[0:0] [1:1] [0:2][1:2]  [2:0]  [2:1] */
         {
            if ((bucket_left_status != 3) &&  (bucket_right_status != 3)) /* [0:0][1:1]*/
            {
                if (p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver]  <=
                   p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver] )
                {
                    use_left_block  = 1;
                }
                else
                {
                    use_left_block  = 0;
                }
            }
            else
            {  /* [0:2][1:2]  [2:0]  [2:1] */
                 use_left_block  = bucket_left_status != 3;
            }
        }
        else /*[2:2] */
        {
            uint32 new_index = 0;
            uint32 old_index = 0;
            find_flag  = 0;
            hash_cpu_status->cpu_lu_index = DRV_HASH_INVALID_INDEX;
            hash_ds_ctl_status.cpu_lu_index    = DRV_HASH_INVALID_INDEX;

            if (p_ipuc_master->do_hash_no_conflict_mode[p_ipuc_info->ip_ver])
            {
                old_index =  bucket_left*(4>>p_ipuc_info->ip_ver) +
                    p_ipuc_master->hash_base[p_ipuc_info->ip_ver] ;
                p_hash_node = &ipuc_data;
                p_hash_node->key_offset = old_index;
                sys_humber_ipuc_db_index_lookup(&p_hash_node);
                if (!p_hash_node)
                {
                    SYS_IPUC_DBG_DUMP("get error!\n");
                    return CTC_E_NONE;
                }

                _sys_humber_l3_asic_hash_lkp_by_direction(chip_id, p_hash_node, 0, &hash_ds_ctl_status);

                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_index  = hash_ds_ctl_status.cpu_lu_index+ p_ipuc_master->hash_base[p_ipuc_info->ip_ver];
                    find_flag = 1;
                    goto FOUND;
                }
                else if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
                {
                    old_index =  bucket_left*4  + 1 +  p_ipuc_master->hash_base[p_ipuc_info->ip_ver] ;
                    p_hash_node = &ipuc_data;
                    p_hash_node->key_offset = old_index;
                    sys_humber_ipuc_db_index_lookup(&p_hash_node);
                    if (!p_hash_node)
                    {
                        SYS_IPUC_DBG_DUMP("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l3_asic_hash_lkp_by_direction(chip_id, p_hash_node, 0, &hash_ds_ctl_status);
                }

                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_index  = hash_ds_ctl_status.cpu_lu_index + p_ipuc_master->hash_base[p_ipuc_info->ip_ver];
                    find_flag = 1;
                    goto FOUND;
                }
                else
                {
                    old_index =  bucket_right*(4>>p_ipuc_info->ip_ver) + (2>>p_ipuc_info->ip_ver)
                        + p_ipuc_master->hash_base[p_ipuc_info->ip_ver] ;
                    p_hash_node = &ipuc_data;
                    p_hash_node->key_offset = old_index;
                    sys_humber_ipuc_db_index_lookup(&p_hash_node);
                    if (!p_hash_node)
                    {
                        SYS_IPUC_DBG_DUMP("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l3_asic_hash_lkp_by_direction(chip_id, p_hash_node, 1, &hash_ds_ctl_status);

                }

                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_index = hash_ds_ctl_status.cpu_lu_index + p_ipuc_master->hash_base[p_ipuc_info->ip_ver] ;
                    find_flag = 1;
                }
                else if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
                {
                    old_index =  bucket_right*4  + 3 + p_ipuc_master->hash_base[p_ipuc_info->ip_ver];
                    p_hash_node = &ipuc_data;
                    p_hash_node->key_offset = old_index;
                    sys_humber_ipuc_db_index_lookup(&p_hash_node);
                    if (!p_hash_node)
                    {
                        SYS_IPUC_DBG_DUMP("get error!\n");
                        return CTC_E_NONE;
                    }
                    _sys_humber_l3_asic_hash_lkp_by_direction(chip_id, p_hash_node, 1, &hash_ds_ctl_status);

                }

                if (find_flag == 1)
                    goto FOUND;

                if (hash_ds_ctl_status.cpu_lu_index != DRV_HASH_INVALID_INDEX )
                {
                    new_index = hash_ds_ctl_status.cpu_lu_index + p_ipuc_master->hash_base[p_ipuc_info->ip_ver];
                    find_flag = 1;
                }

FOUND:
                if (find_flag == 1)
                {
                    uint32 hash_index = 0;
                    uint8 index = 0;

                    /*move old entry to new position*/
                    _sys_humber_ipuc_move_entry(p_hash_node, new_index);

                    /*increase new hash key counter*/
                    hash_index  =  p_hash_node->key_offset- p_ipuc_master->hash_base[p_ipuc_info->ip_ver];
                    index = hash_index % (4>>p_ipuc_info->ip_ver) ;
                    if (index < (2>>p_ipuc_info->ip_ver) )
                    {
                        p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver]++;
                    }
                    else
                    {
                        p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver]++;
                    }
                    p_ipuc_master->do_hash_count[p_ipuc_info->ip_ver]++;
                    hash_cpu_status->cpu_lu_index  = old_index - p_ipuc_master->hash_base[p_ipuc_info->ip_ver];

                    /*add new item to old place ,no need to decresae old hash counter*/
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
        SYS_IPUC_DBG_DUMP("LEFT HASH count = %d bucket = 0x%x",
            p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver],bucket_left);
        if (bucket_left_status == 2 || bucket_left_status == 0)
        {
            if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
            {
                hash_cpu_status->cpu_lu_index = bucket_left*4  + 0;
                p_ipuc_master->hash_status[bucket_left] |=  1 ;
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_left*2  + 0;
                p_ipuc_master->hash_status[bucket_left] |=  3<<4 ;
            }

        }
        else
        {
            hash_cpu_status->cpu_lu_index = bucket_left*4  + 1;
            p_ipuc_master->hash_status[bucket_left] |=  1 << 1;
        }
        p_ipuc_master->do_left_hash_count[p_ipuc_info->ip_ver]++;
    }
    else
    {
        SYS_IPUC_DBG_DUMP("RIGHT HASH count = %d bucket = 0x%x",
            p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver],bucket_right);
        if (bucket_right_status == 2 || bucket_right_status == 0)
        {
            if(p_ipuc_info->ip_ver == CTC_IP_VER_4)
            {
                hash_cpu_status->cpu_lu_index = bucket_right*4 +2 ;
                p_ipuc_master->hash_status[bucket_right] |= 1<<2 ;
            }
            else
            {
                hash_cpu_status->cpu_lu_index = bucket_right*2  + 1;
                p_ipuc_master->hash_status[bucket_right] |=  3<<4<<2 ;
            }
        }
        else
        {
            hash_cpu_status->cpu_lu_index = bucket_right*4 + 3;
            p_ipuc_master->hash_status[bucket_right] |= 1<<3;
        }
        p_ipuc_master->do_right_hash_count[p_ipuc_info->ip_ver]++;
    }
    SYS_IPUC_DBG_DUMP("  offset = 0x%x\n",hash_cpu_status->cpu_lu_index);
    p_ipuc_master->do_hash_count[p_ipuc_info->ip_ver]++;
    return CTC_E_NONE;
}

static int32
_sys_humber_ipuc_ecmp_add(ctc_ipuc_param_t* p_ipuc_param)
{
    sys_ipuc_info_t  *p_ipuc_data, ipuc_data;
    sys_nh_info_ecmp_t* p_nhinfo;
    int32 ret;
    hash_ds_ctl_cpu_key_status_t hash_cpu_status;
    sys_nh_offset_array_t fwd_offset;

    /* prepare data */
    p_ipuc_data = &ipuc_data;
    SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);

    SYS_IPUC_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_db_lookup(&p_ipuc_data));
    if(p_ipuc_data) /* route have exist*/
    {
        CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_EXIST);
    }

    SYS_IPUC_UNLOCK;

    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_nh_get_nhinfo_by_nhid(p_ipuc_param->nh_id, (sys_nh_info_com_t**)&p_nhinfo));
    if(SYS_HUMBER_NH_TYPE_ECMP != p_nhinfo->hdr.nh_entry_type)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_ipuc_param->nh_id, fwd_offset));

    p_ipuc_data = mem_malloc(MEM_IPUC_MODULE,  p_ipuc_master->info_size[p_ipuc_param->ip_ver]);
    if(NULL == p_ipuc_data)
    {
        return CTC_E_NO_MEMORY;
    }

    SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);
    SYS_IPUC_DATA_MAP(p_ipuc_param, p_ipuc_data);
    p_ipuc_data->in_sram = FALSE;
    p_ipuc_data->ecpn = p_nhinfo->valid_item_cnt - 1;

    SYS_IPUC_LOCK;
    /* get the hash offset */
    if(p_ipuc_data->masklen == p_ipuc_master->max_mask_len[p_ipuc_data->ip_ver] &&
       p_ipuc_master->asic_hash_en[p_ipuc_data->ip_ver])
    {
        ret = _sys_humber_l3_asic_hash_lkp(0, p_ipuc_data, &hash_cpu_status);
        if(ret)
        {
            mem_free(p_ipuc_data);
            CTC_ERROR_RETURN_IPUC_UNLOCK(ret);
        }

        if(hash_cpu_status.cpu_lu_index != DRV_HASH_INVALID_INDEX)
        {
            p_ipuc_data->in_sram = TRUE;
            p_ipuc_data->key_offset = hash_cpu_status.cpu_lu_index + p_ipuc_master->hash_base[p_ipuc_data->ip_ver];
        }
        p_ipuc_data->in_sram = FALSE;
    }

    ret = sys_humber_ipuc_db_add(p_ipuc_data);
    if(ret)
    {
        mem_free(p_ipuc_data);
        CTC_ERROR_RETURN_IPUC_UNLOCK(ret);
    }

    SYS_IPUC_UNLOCK;

    /* write ipda entry */
    CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipda(p_ipuc_data, p_nhinfo->rpf_array[0].oif_id, fwd_offset));

    /* write ipsa entry */
    CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipsa(p_ipuc_data, p_nhinfo->rpf_array));

    /* write ipuc key entry */
    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_write_key(p_ipuc_data));

    return CTC_E_NONE;
}

static int32
_sys_humber_ipuc_ecmp_remove(ctc_ipuc_param_t* p_ipuc_param)
{
    sys_ipuc_info_t  *p_ipuc_data, ipuc_data;

    /* prepare data */
    p_ipuc_data = &ipuc_data;
    SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);

    SYS_IPUC_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_db_lookup(&p_ipuc_data));
    if(!p_ipuc_data)
    {
        CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(p_ipuc_data->nh_id != p_ipuc_param->nh_id)
    {
        CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    /* write ipuc key entry */
    sys_humber_ipuc_remove_key(p_ipuc_data);

    sys_humber_ipuc_db_remove(p_ipuc_data);

    SYS_IPUC_UNLOCK;

    mem_free(p_ipuc_data);

    return CTC_E_NONE;
}

/**
 @brief function of add ip route

 @param[in] p_ipuc_param, parameters used to add ip route

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_add(ctc_ipuc_param_t* p_ipuc_param)
{
    sys_ipuc_info_t  *p_ipuc_data, ipuc_data;
    int32 ret;

    SYS_IPUC_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_ipuc_param);
    SYS_IP_FLAG_CHECK(p_ipuc_param);
    SYS_IPUC_NHID_CHECK(p_ipuc_param->nh_id, p_ipuc_param->is_ecmp_nh);
    CTC_IP_VER_CHECK(p_ipuc_param->ip_ver);
    SYS_IP_CHECK_VERSION_ENABLE(p_ipuc_param->ip_ver, p_ipuc_param->vrf_id);
    SYS_IPUC_MASK_LEN_CHECK(p_ipuc_param->ip_ver, p_ipuc_param->masklen);
    SYS_IP_ADDRESS_SORT(p_ipuc_param);
    SYS_IP_ADDR_MASK(p_ipuc_param->ip, p_ipuc_param->masklen, p_ipuc_param->ip_ver);
    SYS_IP_TUNNEL_VALID_CHECK(p_ipuc_param->is_tunnel,p_ipuc_param->ip_ver);
    SYS_IP_TUNNEL_TYPE_CHECK(p_ipuc_param->is_tunnel,p_ipuc_param->tunnel_info.tunnel_packet_type);
    SYS_IP_TUNNEL_GRE_CHECK(p_ipuc_param->tunnel_info.tunnel_packet_type,p_ipuc_param->tunnel_info.is_set_gre_key);
    SYS_IP_TUNNEL_ROUTE_FLAG_CHECK(p_ipuc_param->route_flag,p_ipuc_param->is_tunnel,p_ipuc_param->is_tunnel_l3if);

    if(p_ipuc_param->is_ecmp_nh)
    {
        return _sys_humber_ipuc_ecmp_add(p_ipuc_param);
    }

    /* prepare data */
    p_ipuc_data = &ipuc_data;
    SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);

    SYS_IPUC_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_db_lookup(&p_ipuc_data));
    if(p_ipuc_data) /* route have exist */
    {
        uint32 hash_key_offset = 0;
        uint32 tcam_key_offset = 0;
        sys_nh_param_ecmp_api_t nh_param;
        if(p_ipuc_data->ecpn >= CTC_MAX_ECPN - 1)
        {
            /* ecmp number exceed */
            CTC_RETURN_IPUC_UNLOCK(CTC_E_EXCEED_MAX_SIZE);
        }

        if(p_ipuc_data->nh_id == p_ipuc_param->nh_id)
        {
            /* this route have installed */
            CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_EXIST);
        }

        if(CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_CONNECT) ||
            CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_CONNECT))
        {
            /* connect route must not do ecmp */
            CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_EXIST);
        }

        if((!CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_NEIGHBOR)) && p_ipuc_data->in_sram)
        {
            /*must move key form sram to tcam, alloc tcam offset first*/
            hash_key_offset = p_ipuc_data->key_offset;
            ret = sys_humber_ipuc_db_get_offset(p_ipuc_data);
            if(ret)
            {
                p_ipuc_data->key_offset = hash_key_offset;
                CTC_ERROR_RETURN_IPUC_UNLOCK(ret);
            }
        }

        kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
        nh_param.ref_nh_id = p_ipuc_param->nh_id;
        nh_param.oif_need = TRUE;
        if(p_ipuc_data->ecpn == 0)
        {
            CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ecmp_nh_create(&nh_param, &p_ipuc_data->nh_id));
        }
        else
        {
            nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_ADD_ITEM;

            CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ecmp_nh_update(p_ipuc_data->nh_id, &nh_param));
        }
        p_ipuc_data->ecpn++;

        if((!CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_NEIGHBOR)) && p_ipuc_data->in_sram)
        {
            /*move key from sram to tcam*/
            p_ipuc_data->route_flag = (p_ipuc_param)->route_flag;
            p_ipuc_data->in_sram = FALSE;

            /* write ipda entry */
            CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipda(p_ipuc_data, nh_param.p_rpf_array[0].oif_id, nh_param.fwd_offset));

            /* write ipsa entry */
            CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipsa(p_ipuc_data, nh_param.p_rpf_array));

            /* write ipuc key in tcam*/
            CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_write_key(p_ipuc_data));

            /* remove ipuc key from hash */
            p_ipuc_data->in_sram = TRUE;
            tcam_key_offset = p_ipuc_data->key_offset;
            p_ipuc_data->key_offset = hash_key_offset;
            sys_humber_ipuc_remove_key(p_ipuc_data);

            p_ipuc_data->key_offset = tcam_key_offset;
            p_ipuc_data->in_sram = FALSE;
        }
        else
        {
            /* write ipda entry */
            CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipda(p_ipuc_data, nh_param.p_rpf_array[0].oif_id, nh_param.fwd_offset));

            /* write ipsa entry */
            if(nh_param.oif_changed)
            {
                CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipsa(p_ipuc_data, nh_param.p_rpf_array));
            }
        }
    }
    else
    {
        sys_rpf_info_t rpf_info[CTC_MAX_ECPN];
        hash_ds_ctl_cpu_key_status_t hash_cpu_status;
        sys_nh_offset_array_t fwd_offset;

        SYS_IPUC_UNLOCK;

        CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(p_ipuc_param->nh_id, fwd_offset));
        kal_memset(rpf_info, 0, sizeof(rpf_info));
        if(CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_CONNECT) )
        {
            rpf_info[0].oif_id = p_ipuc_param->l3_inf;
            rpf_info[0].ref = 1;
        }
        else if( ! p_ipuc_param->is_tunnel )
        {
            CTC_ERROR_RETURN(sys_humber_nh_get_l3ifid(p_ipuc_param->nh_id, &rpf_info[0].oif_id));
            rpf_info[0].ref = 1;
        }
        else if( p_ipuc_param->is_tunnel )
        {
            ret = sys_humber_nh_get_l3ifid(p_ipuc_param->nh_id, &rpf_info[0].oif_id);
            if( ret )
            {
                if( p_ipuc_param->is_tunnel_l3if )
                {
                    rpf_info[0].oif_id = p_ipuc_param->l3_inf;
                    rpf_info[0].ref = 1;
                }
                else if (CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_RPF_CHECK) ||
                    CTC_FLAG_ISSET((p_ipuc_param)->route_flag, CTC_IPUC_FLAG_ICMP_CHECK))
                {
                    return CTC_E_NEED_L3IF;
                }
                else
                {
                    rpf_info[0].ref = 0;
                }
            }
            else
            {
                if( p_ipuc_param->is_tunnel_l3if )
                {
                    return CTC_E_INVALID_L3IF;
                }
                else
                {
                    rpf_info[0].ref = 1;
                }
            }
        }

        p_ipuc_data = mem_malloc(MEM_IPUC_MODULE,  p_ipuc_master->info_size[p_ipuc_param->ip_ver]);
        if(NULL == p_ipuc_data)
        {
            return CTC_E_NO_MEMORY;
        }

        SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);
        SYS_IPUC_DATA_MAP(p_ipuc_param, p_ipuc_data);
        p_ipuc_data->in_sram = FALSE;
        p_ipuc_data->ecpn = 0;

        SYS_IPUC_LOCK;
        /* get the hash offset */
        if(p_ipuc_data->masklen == p_ipuc_master->max_mask_len[p_ipuc_data->ip_ver] &&
           CTC_FLAG_ISSET(p_ipuc_data->route_flag, CTC_IPUC_FLAG_NEIGHBOR))
        {
            ret = _sys_humber_l3_asic_hash_lkp(0, p_ipuc_data, &hash_cpu_status);
            if(ret)
            {
                mem_free(p_ipuc_data);
                CTC_ERROR_RETURN_IPUC_UNLOCK(ret);
            }

            if(hash_cpu_status.cpu_lu_index != DRV_HASH_INVALID_INDEX)
            {
                p_ipuc_data->in_sram = TRUE;
                p_ipuc_data->key_offset = hash_cpu_status.cpu_lu_index + p_ipuc_master->hash_base[p_ipuc_data->ip_ver];
            }
        }

        ret = sys_humber_ipuc_db_add(p_ipuc_data);
        if(ret)
        {
            mem_free(p_ipuc_data);
            CTC_ERROR_RETURN_IPUC_UNLOCK(ret);
        }

        /* write ipda entry */
        CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipda(p_ipuc_data, rpf_info[0].oif_id, fwd_offset));

        /* write ipsa entry */
        CTC_ERROR_RETURN_IPUC_UNLOCK(_sys_humber_ipuc_write_ipsa(p_ipuc_data, rpf_info));

        /* write ipuc key entry */
        CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_write_key(p_ipuc_data));
    }

    SYS_IPUC_UNLOCK;

    return CTC_E_NONE;
}

/**
 @brief function of remove ip route

 @param[in] p_ipuc_param, parameters used to remove ip route

 @return CTC_E_XXX
 */
int32
sys_humber_ipuc_remove(ctc_ipuc_param_t* p_ipuc_param)
{
    sys_ipuc_info_t  *p_ipuc_data, ipuc_data;

    SYS_IPUC_INIT_CHECK;

    CTC_PTR_VALID_CHECK(p_ipuc_param);
    SYS_IPUC_NHID_CHECK(p_ipuc_param->nh_id, p_ipuc_param->is_ecmp_nh);
    CTC_IP_VER_CHECK(p_ipuc_param->ip_ver);
    SYS_IP_CHECK_VERSION_ENABLE(p_ipuc_param->ip_ver, p_ipuc_param->vrf_id);
    SYS_IPUC_MASK_LEN_CHECK(p_ipuc_param->ip_ver, p_ipuc_param->masklen);
    SYS_IP_ADDRESS_SORT(p_ipuc_param);
    SYS_IP_ADDR_MASK(p_ipuc_param->ip, p_ipuc_param->masklen, p_ipuc_param->ip_ver);
    SYS_IP_TUNNEL_VALID_CHECK(p_ipuc_param->is_tunnel,p_ipuc_param->ip_ver);
    SYS_IP_TUNNEL_TYPE_CHECK(p_ipuc_param->is_tunnel,p_ipuc_param->tunnel_info.tunnel_packet_type);
    SYS_IP_TUNNEL_GRE_CHECK(p_ipuc_param->tunnel_info.tunnel_packet_type,p_ipuc_param->tunnel_info.is_set_gre_key);

    if(p_ipuc_param->is_ecmp_nh)
    {
        return _sys_humber_ipuc_ecmp_remove(p_ipuc_param);
    }

    /* prepare data */
    p_ipuc_data = &ipuc_data;
    SYS_IPUC_KEY_MAP(p_ipuc_param, p_ipuc_data);

    SYS_IPUC_LOCK;
    /* lookup for ipuc entrise */
    CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ipuc_db_lookup(&p_ipuc_data));
    if(!p_ipuc_data)
    {
        CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
    }

    if(p_ipuc_data->ecpn == 0)
    {
        if(p_ipuc_data->nh_id != p_ipuc_param->nh_id)
        {
            CTC_RETURN_IPUC_UNLOCK(CTC_E_ENTRY_NOT_EXIST);
        }

        /* write ipuc key entry */
        sys_humber_ipuc_remove_key(p_ipuc_data);

        sys_humber_ipuc_db_remove(p_ipuc_data);

        mem_free(p_ipuc_data);
    }
    else
    {
        sys_nh_param_ecmp_api_t nh_param;
        uint32 ecmp_nhid;

        kal_memset(&nh_param, 0, sizeof(sys_nh_param_ecmp_api_t));
        nh_param.ref_nh_id = p_ipuc_param->nh_id;
        nh_param.change_type = SYS_NH_CHANGE_TYPE_ECMP_REMOVE_ITEM;
        nh_param.oif_need = TRUE;
        ecmp_nhid = p_ipuc_data->nh_id;

        CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ecmp_nh_update(ecmp_nhid, &nh_param));
        p_ipuc_data->ecpn--;

        if(p_ipuc_data->ecpn == 0)
        {
            p_ipuc_data->nh_id = nh_param.ref_nh_id;
        }

        /* write ipda entry */
        _sys_humber_ipuc_write_ipda(p_ipuc_data, nh_param.p_rpf_array[0].oif_id, nh_param.fwd_offset);

        /* write ipsa entry */
        if(nh_param.oif_changed)
        {
            _sys_humber_ipuc_write_ipsa(p_ipuc_data, nh_param.p_rpf_array);
        }

        if(p_ipuc_data->ecpn == 0)
        {
            CTC_ERROR_RETURN_IPUC_UNLOCK(sys_humber_ecmp_nh_delete(ecmp_nhid));
        }
    }

    SYS_IPUC_UNLOCK;

    return CTC_E_NONE;
}

/* now hash next-hop need not to set > 512 */
int32
sys_humber_ipuc_add_default_entry(uint8 ip_ver, uint32 nh_id)
{
    sys_rpf_info_t rpf_info[CTC_MAX_ECPN];
    sys_ipuc_info_t ipuc_data;
    uint32 table_size;
    sys_nh_offset_array_t fwd_offset;

    SYS_IPUC_INIT_CHECK;
    CTC_IP_VER_CHECK(ip_ver);

    if(!p_ipuc_master->version_en[ip_ver])
    {
        return CTC_E_VERSION_DISABLE;
    }

    kal_memset(rpf_info, 0, sizeof(rpf_info));
    kal_memset(&ipuc_data, 0, sizeof(sys_ipuc_info_t));

    CTC_ERROR_RETURN(sys_humber_nh_get_dsfwd_offset(nh_id, fwd_offset));

    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(p_ipuc_master->key_table_id[ip_ver], &table_size));
    ipuc_data.key_offset = table_size - 1;
    ipuc_data.route_flag = SYS_IPUC_FLAG_DEFAULT | CTC_IPUC_FLAG_RPF_CHECK;
    ipuc_data.ip_ver = ip_ver;

    /* write ipda entry */
    CTC_ERROR_RETURN(_sys_humber_ipuc_write_ipda(&ipuc_data, 0, fwd_offset));
    /* write ipsa entry */
    CTC_ERROR_RETURN(_sys_humber_ipuc_write_ipsa(&ipuc_data, rpf_info));
    /* write ipuc key entry */
    CTC_ERROR_RETURN(sys_humber_ipuc_write_key(&ipuc_data));

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_cpu_rpf_check(bool enable)
{
    SYS_IPUC_INIT_CHECK;

    p_ipuc_master->cpu_rpf = enable;
    return CTC_E_NONE;
}

int32
sys_humber_ipuc_init(void)
{
    ipe_hash_lookup_result_ctl_t hash_rslt_ctl = {0};
    sys_alloc_allocated_tcam_info_t  tcam_info = {0};
    uint32 cmd = 0;
    uint8 ip_ver;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    uint32 table_size_ipv4, table_size_ipv6;
    parser_layer3_protocol_cam_t cam = {0};
    parser_layer3_protocol_cam_valid_t cam_valid = {0};
    uint32 bucket_num[MAX_CTC_IP_VER] = {0};
    hash_ds_ctl_lookup_ctl_t hash_ctl_tmp;
    uint32 valid_bucket_num[] = {256, 512, 1024, 2*1024, 4*1024, 8*1024, 16*1024, 32*1024};
    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV4_UCAST_ROUTE_KEY, &table_size_ipv4));
    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV6_UCAST_ROUTE_KEY, &table_size_ipv6));
    if (!table_size_ipv4 && !table_size_ipv6)
    {
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV4_UCAST_HASH_KEY0, &bucket_num[CTC_IP_VER_4]));
    CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(DS_IPV6_UCAST_HASH_KEY0, &bucket_num[CTC_IP_VER_6]));

    p_ipuc_master = mem_malloc(MEM_IPUC_MODULE, sizeof(sys_ipuc_master_t));
    if(NULL == p_ipuc_master)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_ipuc_master,0,sizeof(sys_ipuc_master_t));

    p_ipuc_master->write_key[CTC_IP_VER_4] = _sys_humber_ipv4_write_key;
    p_ipuc_master->write_key[CTC_IP_VER_6] = _sys_humber_ipv6_write_key;

    p_ipuc_master->write_hash_key[CTC_IP_VER_4] = _sys_humber_ipv4_hash_write_key;
    p_ipuc_master->write_hash_key[CTC_IP_VER_6] = _sys_humber_ipv6_hash_write_key;

    p_ipuc_master->hashkey_type[CTC_IP_VER_4] = CPU_HASH_KEY_TYPE_IPV4_UC;
    p_ipuc_master->hashkey_type[CTC_IP_VER_6] = CPU_HASH_KEY_TYPE_IPV6_UC;

    p_ipuc_master->sa_table_id[CTC_IP_VER_4] = DS_IPV4_UCAST_SA;
    p_ipuc_master->sa_table_id[CTC_IP_VER_6] = DS_IPV6_UCAST_SA;

    p_ipuc_master->da_table_id[CTC_IP_VER_4] = DS_IPV4_UCAST_DA;
    p_ipuc_master->da_table_id[CTC_IP_VER_6] = DS_IPV6_UCAST_DA;

    p_ipuc_master->key_table_id[CTC_IP_VER_4] = DS_IPV4_UCAST_ROUTE_KEY;
    p_ipuc_master->key_table_id[CTC_IP_VER_6] = DS_IPV6_UCAST_ROUTE_KEY;

    p_ipuc_master->hashkey_table_id[CTC_IP_VER_4] = DS_IPV4_UCAST_HASH_KEY0;
    p_ipuc_master->hashkey_table_id[CTC_IP_VER_6] = DS_IPV6_UCAST_HASH_KEY0;

    p_ipuc_master->info_size[CTC_IP_VER_4] = sizeof(sys_ipv4_info_t);
    p_ipuc_master->info_size[CTC_IP_VER_6] = sizeof(sys_ipv6_info_t);

    p_ipuc_master->cpu_rpf = FALSE;

    sys_alloc_get_ipucsa_enable(&p_ipuc_master->is_ipucsa_allocated);

    p_ipuc_master->max_vrfid[CTC_IP_VER_4] = CTC_MAX_IPV4_VRFID;
    p_ipuc_master->max_vrfid[CTC_IP_VER_6] = CTC_MAX_IPV6_VRFID;

    p_ipuc_master->max_mask_len[CTC_IP_VER_4] = CTC_IPV4_ADDR_LEN_IN_BIT;
    p_ipuc_master->max_mask_len[CTC_IP_VER_6] = CTC_IPV6_ADDR_LEN_IN_BIT;

    p_ipuc_master->addr_len[CTC_IP_VER_4] = CTC_IPV4_ADDR_LEN_IN_BYTE;
    p_ipuc_master->addr_len[CTC_IP_VER_6] = CTC_IPV6_ADDR_LEN_IN_BYTE;

    p_ipuc_master->version_en[CTC_IP_VER_4] = FALSE;
    p_ipuc_master->version_en[CTC_IP_VER_6] = FALSE;

    p_ipuc_master->tunnel_en[CTC_IP_VER_4] = FALSE;
    p_ipuc_master->tunnel_en[CTC_IP_VER_6] = FALSE;

    sys_alloc_get_tcam_key_alloc_info(DS_IPV4_UCAST_ROUTE_KEY, &tcam_info);
    p_ipuc_master->asic_hash_en[CTC_IP_VER_4] = tcam_info.is_hash_used;

    sys_alloc_get_tcam_key_alloc_info(DS_IPV6_UCAST_ROUTE_KEY, &tcam_info);
    p_ipuc_master->asic_hash_en[CTC_IP_VER_6] = tcam_info.is_hash_used;

    cmd = DRV_IOR(IOC_REG, IPE_HASH_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &hash_rslt_ctl));

    if(hash_rslt_ctl.ipv4_ucast_lookup_table_base_pos)
    {
        p_ipuc_master->hash_base[CTC_IP_VER_4] = hash_rslt_ctl.ipv4_ucast_lookup_table_base << 8;
        p_ipuc_master->tcam_base[CTC_IP_VER_4] = 0;
    }
    else
    {
        p_ipuc_master->tcam_base[CTC_IP_VER_4] = hash_rslt_ctl.ipv4_ucast_lookup_table_base << 8;
        p_ipuc_master->hash_base[CTC_IP_VER_4] = 0;
    }

    if(hash_rslt_ctl.ipv6_ucast_lookup_table_base_pos)
    {
        p_ipuc_master->hash_base[CTC_IP_VER_6] = hash_rslt_ctl.ipv6_ucast_lookup_table_base << 8;
        p_ipuc_master->tcam_base[CTC_IP_VER_6] = 0;
    }
    else
    {
        p_ipuc_master->tcam_base[CTC_IP_VER_6] = hash_rslt_ctl.ipv6_ucast_lookup_table_base << 8;
        p_ipuc_master->hash_base[CTC_IP_VER_6] = 0;
    }

    p_ipuc_master->cal_hash_index[CTC_IP_VER_4][SYS_IPUC_HASH_LEFT] = sys_humber_hash_generate_ipv4_hash0;
    p_ipuc_master->cal_hash_index[CTC_IP_VER_4][SYS_IPUC_HASH_RIGHT] = sys_humber_hash_generate_ipv4_hash1;

    p_ipuc_master->cal_hash_index[CTC_IP_VER_6][SYS_IPUC_HASH_LEFT] = sys_humber_hash_generate_ipv6_hash0;
    p_ipuc_master->cal_hash_index[CTC_IP_VER_6][SYS_IPUC_HASH_RIGHT] = sys_humber_hash_generate_ipv6_hash1;

    /* init hash status for hash bucket*/
    cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_LOOKUP_CTL, DRV_ENTRY_FLAG);
    DRV_IF_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &hash_ctl_tmp));
    if (bucket_num[CTC_IP_VER_4] > 512)
    {
        p_ipuc_master->hash_bit_num[CTC_IP_VER_4] = hash_ctl_tmp.ipv4_ucast_bits_num;
        p_ipuc_master->hash_array_num = valid_bucket_num[hash_ctl_tmp.ipv4_ucast_bits_num];
    }
    else
    {
        p_ipuc_master->hash_bit_num[CTC_IP_VER_4] = SYS_IPUC_ERROR_BIT_NUM;
    }

    if (bucket_num[CTC_IP_VER_6] > 256)
    {
        p_ipuc_master->hash_bit_num[CTC_IP_VER_6] = hash_ctl_tmp.ipv6_ucast_bits_num;
        p_ipuc_master->hash_array_num =
            (p_ipuc_master->hash_array_num > valid_bucket_num[hash_ctl_tmp.ipv6_ucast_bits_num])?
            p_ipuc_master->hash_array_num:valid_bucket_num[hash_ctl_tmp.ipv6_ucast_bits_num];
    }
    else
    {
        p_ipuc_master->hash_bit_num[CTC_IP_VER_6] = SYS_IPUC_ERROR_BIT_NUM;
    }

    if (p_ipuc_master->hash_array_num > 0)
    {
        p_ipuc_master->hash_status = (uint8 *)mem_malloc(MEM_IPUC_MODULE,
            (p_ipuc_master->hash_array_num)*sizeof(uint8));
        if (NULL == p_ipuc_master->hash_status)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_ipuc_master->hash_status, 0, (p_ipuc_master->hash_array_num)*sizeof(uint8));
        p_ipuc_master->do_hash_no_conflict_mode[CTC_IP_VER_4] = TRUE;
        p_ipuc_master->do_hash_no_conflict_mode[CTC_IP_VER_6] = TRUE;
    }

    SYS_IPUC_CREAT_LOCK;

    CTC_ERROR_RETURN(sys_humber_ipuc_db_init());

    for(ip_ver=0; ip_ver<MAX_CTC_IP_VER; ip_ver++)
    {
        if(TRUE == p_ipuc_master->version_en[ip_ver])
        {
            sys_humber_ipuc_add_default_entry(ip_ver, CTC_NH_RESERVED_NHID_FOR_DROP);
        }
    }

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        /*add parser type for IPv4inIPv6*/
        kal_memset(&cam, 0, sizeof(parser_layer3_protocol_cam_t));
        kal_memset(&cam_valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                            sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
        cam.layer3_protocol_cam_additional_offset12 = 0;
        cam.layer3_protocol_cam_layer3_header_protocol12 = 0x04;
        cam.layer3_protocol_cam_layer3_header_protocol_mask12 = 0xff & 0xff;
        cam.layer3_protocol_cam_layer3_type12 = 0x03;
        cam.layer3_protocol_cam_layer3_type_mask12 = 0x0c & 0xf;
        cam.layer3_protocol_cam_layer4_type12 = 0x04;
        cam_valid.layer3_cam_entry_valid |= (1<<12);
        CTC_ERROR_RETURN(sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                    sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));

        /*add parser type for IPv6GRE*/
        kal_memset(&cam, 0, sizeof(parser_layer3_protocol_cam_t));
        kal_memset(&cam_valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                            sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
        cam.layer3_protocol_cam_additional_offset13 = 0;
        cam.layer3_protocol_cam_layer3_header_protocol13 = 0x2f;
        cam.layer3_protocol_cam_layer3_header_protocol_mask13 = 0xff;
        cam.layer3_protocol_cam_layer3_type13 = 0x03;
        cam.layer3_protocol_cam_layer3_type_mask13 = 0x0c;
        cam.layer3_protocol_cam_layer4_type13 = 0x03;
        cam_valid.layer3_cam_entry_valid |= (1<<13);
        CTC_ERROR_RETURN(sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                    sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));


    }
    CTC_ERROR_RETURN(sys_humer_nh_get_max_external_nhid( &p_ipuc_master->max_external_nhid ));

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_state_show(void)
{
    uint32 table_size;
    uint32 i;

    SYS_IPUC_INIT_CHECK;

    for(i = 0; i < MAX_CTC_IP_VER; i++)
    {
        CTC_ERROR_RETURN(sys_alloc_get_table_entry_num(p_ipuc_master->key_table_id[i], &table_size));
        SYS_IPUC_DBG_DUMP("%-4s %-8s    Table %d\n\r", (i==CTC_IP_VER_4)?"IPv4":"IPv6", p_ipuc_master->version_en[i] ? "Enabled" : "Disabled", table_size);
    }

    SYS_IPUC_DBG_DUMP("CPU RPF Check %s\n\r", p_ipuc_master->cpu_rpf ? "Enabled" : "Disabled");

    return CTC_E_NONE;
}

int32
sys_humber_ipuc_set_route_ctl(ctc_ipuc_route_ctl_t *p_route_ctl_info)
{
    uint8 local_chip_num;
    uint8 chip_id;
    uint32 cmd = 0;
    ipe_route_ctl_t ipe_route_ctl;

    cmd = DRV_IOR(IOC_REG, IPE_ROUTE_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &ipe_route_ctl));

    if(CTC_FLAG_ISSET(p_route_ctl_info->valid_flag, CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_EN))
    {
        ipe_route_ctl.martian_check_en_bit10_to6 = (p_route_ctl_info->martian_check_en >> 6) & 0x1F;
        ipe_route_ctl.martian_check_en_bit5_to0  = p_route_ctl_info->martian_check_en & 0x3F;
    }
    if(CTC_FLAG_ISSET(p_route_ctl_info->valid_flag, CTC_IPUC_ROUTE_CTL_MARTIAN_CHECK_DISABLE))
    {
        ipe_route_ctl.martian_addr_check_dis = p_route_ctl_info->martian_check_disable;
    }
    if(CTC_FLAG_ISSET(p_route_ctl_info->valid_flag, CTC_IPUC_ROUTE_CTL_MCAST_ADDRESS_CHECK))
    {
        ipe_route_ctl.mcast_addr_match_check_dis = p_route_ctl_info->mcast_address_match_check_disable;
    }
    if(CTC_FLAG_ISSET(p_route_ctl_info->valid_flag, CTC_IPUC_ROUTE_CTL_TTL_LIMIT))
    {
        ipe_route_ctl.ip_ttl_limit = p_route_ctl_info->ip_ttl_limit;
    }

    local_chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, IPE_ROUTE_CTL, DRV_ENTRY_FLAG);
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_route_ctl));
    }

    return CTC_E_NONE;
}


int32
sys_humber_ipuc_set_lookup_ctl(ctc_ipuc_lookup_ctl_t* p_lookup_ctl_info)
{
    uint8 local_chip_num;
    uint8 chip_id;
    uint32 cmd = 0;
    ipe_lookup_ctl_t ipe_lookup_ctl;

    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &ipe_lookup_ctl));


    if(CTC_FLAG_ISSET(p_lookup_ctl_info->valid_flag, CTC_IPUC_lookup_CTL_IPSA_LOOKUP_EN))
    {
        ipe_lookup_ctl.ipv4_ucast_route_key_sa_en = p_lookup_ctl_info->ipv4_ucast_route_key_sa_en & 0x1;
    }
    else if(CTC_FLAG_ISSET(p_lookup_ctl_info->valid_flag, CTC_IPUC_IPv6_lookup_CTL_IPSA_LOOKUP_EN))
    {
        ipe_lookup_ctl.ipv6_ucast_route_key_sa_en = p_lookup_ctl_info->ipv6_ucast_route_key_sa_en & 0x1;
    }
    local_chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_CTL, DRV_ENTRY_FLAG);
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ipe_lookup_ctl));
    }


    return CTC_E_NONE;
}

