/**
 @file sys_humber_nexthop_ip_tunnel.c

 @date 2011-05-23

 @version v2.0

 The file contains all nexthop ip tunnel related callback function
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_linklist.h"

#include "sys_humber_chip.h"
#include "sys_humber_internal_port.h"
#include "sys_humber_nexthop_api.h"
#include "sys_humber_nexthop.h"
#include "sys_humber_vlan.h"
#include "sys_humber_opf.h"


#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

#define SYS_NH_IP_TUNNEL_IPV4_SA_MAX_INDEX 64
#define SYS_NH_IP_TUNNEL_IPV6_IP_MAX_INDEX 256

#define SYS_NH_IP_TUNNEL_IPV6_PROTO_TYPE 41
#define SYS_NH_IP_TUNNEL_IPV4_PROTO_TYPE 4


struct sys_ip_tunnel_ip_node_s
{
    ctc_slistnode_t head;

    union
    {
        ip_addr_t ipv4;
        ipv6_addr_t ipv6;

    } ip;

    uint32  count;
    uint32  index;
    uint8  ip_ver;
    uint8  lchip;

};
typedef struct sys_ip_tunnel_ip_node_s sys_ip_tunnel_ip_node_t;


struct sys_ip_tunnel_master_s
{
    ctc_slist_t* ip_list;
};
typedef struct sys_ip_tunnel_master_s sys_ip_tunnel_master_t;

sys_ip_tunnel_master_t* p_ip_tunnel_master = NULL;



int32
sys_humber_nh_ip_tunnel_init()
{
    sys_humber_opf_t opf = {0};
    uint8 lchip, curr_chip_num = 0;

    p_ip_tunnel_master = (sys_ip_tunnel_master_t *)
                         mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_ip_tunnel_master_t));

    if (NULL == p_ip_tunnel_master)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(p_ip_tunnel_master, 0, sizeof(sys_ip_tunnel_master_t));

    p_ip_tunnel_master->ip_list = ctc_slist_new();

    if (NULL == p_ip_tunnel_master->ip_list)
    {
        return CTC_E_NOT_INIT;
    }

    curr_chip_num = sys_humber_get_local_chip_num();
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_TUNNEL_IPV4_SA, curr_chip_num));
    CTC_ERROR_RETURN(sys_humber_opf_init(OPF_TUNNEL_IPV6_IP, curr_chip_num));

    for (lchip = 0; lchip < curr_chip_num; lchip++)
    {
        /*IPv4 SA */
        opf.pool_type=  OPF_TUNNEL_IPV4_SA;
        opf.pool_index = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf,0, SYS_NH_IP_TUNNEL_IPV4_SA_MAX_INDEX));


        /*IPv6 SA and DA */
        opf.pool_type = OPF_TUNNEL_IPV6_IP;
        opf.pool_index  = lchip;
        CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf,0, SYS_NH_IP_TUNNEL_IPV6_IP_MAX_INDEX));
    }

    return CTC_E_NONE;
}


 static INLINE int32
 _sys_humber_nh_ip_tunnel_get_dsnh_entry_num(uint16 entry_flag, uint8 *p_entry_num)
{
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_entry_num);

    if(CTC_FLAG_ISSET(entry_flag, SYS_NH_INFO_IP_TUNNEL_FLAG_USE_DSNH8W))
    {
        *p_entry_num = 2;
    }
    else
    {
        *p_entry_num = 1;
    }

    return CTC_E_NONE;
}


static sys_ip_tunnel_ip_node_t *
_sys_humber_nh_ip_tunnel_find_ip(uint8 lchip, uint8 ip_ver, void *ip_addr, uint32 *p_index)
{
    int32 lcmp = 0;
    sys_ip_tunnel_ip_node_t *p_ip_node = NULL;
    ctc_slistnode_t         *lisnode = NULL;


    SYS_NH_DBG_FUNC();

    CTC_SLIST_LOOP(p_ip_tunnel_master->ip_list, lisnode)
    {
        p_ip_node = _ctc_container_of(lisnode, sys_ip_tunnel_ip_node_t, head);

        if (CTC_IP_VER_4 == ip_ver)
        {
            lcmp = kal_memcmp(&(p_ip_node->ip.ipv4), ip_addr, sizeof(ip_addr_t));
        }
        else
        {
            lcmp = kal_memcmp(&(p_ip_node->ip.ipv6), ip_addr, sizeof(ipv6_addr_t));
        }

        if ((p_ip_node->ip_ver == ip_ver) &&
            (p_ip_node->lchip == lchip) &&
            ((0 == lcmp) || (*p_index == p_ip_node->index)))
        {
            *p_index = p_ip_node->index;
            return p_ip_node;
        }
    }

    return NULL;
}

static int32
_sys_humber_nh_ip_tunnel_add_ip(uint8 lchip, uint8 ip_ver, void *ip_addr, uint32 index)
{
    sys_ip_tunnel_ip_node_t *p_ip_node = NULL;

    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(ip_addr);

   /*first ipsa adress for same ipsa index*/
    p_ip_node = (sys_ip_tunnel_ip_node_t *) mem_malloc(MEM_NEXTHOP_MODULE, sizeof(sys_ip_tunnel_ip_node_t));
    kal_memset(p_ip_node,0,sizeof(sys_ip_tunnel_ip_node_t));

    p_ip_node->lchip = lchip;
    p_ip_node->ip_ver = ip_ver;
    p_ip_node->index = index;

    p_ip_node->count++;

    if (CTC_IP_VER_4 == ip_ver)
    {
        kal_memcpy(&(p_ip_node->ip.ipv4), ip_addr, sizeof(ip_addr_t));
    }
    else
    {
        kal_memcpy(&(p_ip_node->ip.ipv6), ip_addr, sizeof(ipv6_addr_t));
    }

    ctc_slist_add_tail(p_ip_tunnel_master->ip_list, &p_ip_node->head);


    return CTC_E_NONE;
}

static int32
_sys_humber_nh_ip_tunnel_del_ip(sys_ip_tunnel_ip_node_t *p_ip_node)
{
    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_ip_node);

    ctc_slist_delete_node(p_ip_tunnel_master->ip_list, &p_ip_node->head);
    mem_free(p_ip_node);

    return CTC_E_NONE;
}


static int32
_sys_humber_nh_ip_tunnel_ip_idx_alloc(uint8 lchip, uint8 ip_ver, void *p_ip_addr, uint32 *p_index)
{
    uint32 ret    = CTC_E_NONE;
    uint32 index  = 0xFFFFF;
    uint32 cmd    = 0;
    uint32 tbl_id = 0;
    sys_humber_opf_t opf       = {0};
    ipv6_addr_t      ipv6_addr = {0};
    ip_addr_t        ipv4_addr = {0};
    ds_l3_edit_tunnel_v4_ip_sa_t tunnel_v4_ip_sa = {0};
    ds_l3_edit_tunnel_v6_ip_t    tunnel_v6_ip    = {0};
    void *p_ip_addr_tunnel_tbl         = NULL;
    sys_ip_tunnel_ip_node_t *p_ip_node = NULL;

    SYS_NH_DBG_FUNC();

    p_ip_node = _sys_humber_nh_ip_tunnel_find_ip(lchip ,ip_ver, p_ip_addr, &index);

    if (NULL == p_ip_node)
    {
        opf.pool_type = (CTC_IP_VER_4 == ip_ver)?OPF_TUNNEL_IPV4_SA:
                                                 OPF_TUNNEL_IPV6_IP;
        opf.pool_index = lchip;

        CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, &index));
        ret = _sys_humber_nh_ip_tunnel_add_ip(lchip,ip_ver, p_ip_addr, index);
        if (CTC_E_NONE != ret)
        {
           sys_humber_opf_free_offset(&opf, 1, index);
           return ret;
        }

        if (CTC_IP_VER_4 == ip_ver)
        {
            tbl_id = DS_L3_EDIT_TUNNEL_V4_IP_SA;
            kal_memcpy(&(ipv4_addr), p_ip_addr, sizeof(ip_addr_t));
            tunnel_v4_ip_sa.ip_sa =  ipv4_addr;
            p_ip_addr_tunnel_tbl = &tunnel_v4_ip_sa;
        }
        else
        {
            tbl_id = DS_L3_EDIT_TUNNEL_V6_IP;
            kal_memcpy(&(ipv6_addr), p_ip_addr, sizeof(ipv6_addr_t));
            tunnel_v6_ip.ip_sa127_to96 = ipv6_addr[0];
            tunnel_v6_ip.ip_sa95_to64  = ipv6_addr[1];
            tunnel_v6_ip.ip_sa63_to32  = ipv6_addr[2];
            tunnel_v6_ip.ip_sa31_to0   = ipv6_addr[3];

            p_ip_addr_tunnel_tbl = &tunnel_v6_ip;
        }

        cmd = DRV_IOW(IOC_TABLE, tbl_id, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, p_ip_addr_tunnel_tbl));

    }
    else
    {
        p_ip_node->count++;
    }

    *p_index = index;


    return ret;
}


static int32
_sys_humber_nh_ip_tunnel_ip_idx_free(uint8 lchip, uint8 ip_ver, uint32 index)
{
    int32  ret = CTC_E_NONE;
    ipv6_addr_t ip_addr  = {0};
    sys_humber_opf_t opf = {0};
    sys_ip_tunnel_ip_node_t *p_ip_node = NULL;

    SYS_NH_DBG_FUNC();

    p_ip_node = _sys_humber_nh_ip_tunnel_find_ip(lchip, ip_ver, &ip_addr, &index);

    if (NULL == p_ip_node)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    if (1 == p_ip_node->count)

    {
         CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_del_ip(p_ip_node));

        opf.pool_type = (CTC_IP_VER_4 == ip_ver)?OPF_TUNNEL_IPV4_SA:
                                                 OPF_TUNNEL_IPV6_IP;
        opf.pool_index = lchip;
        ret = sys_humber_opf_free_offset(&opf, 1, index);
    }
    else
    {
        p_ip_node->count--;
    }

    return ret;
}

static INLINE int32
_sys_humber_nh_ip_tunnel_v6_ecap_ip_header(ctc_ip_nh_param_t *p_nh_param,
                                                          ds_l3edit_tunnel_v6_t *p_dsl3edit_tunnel_v6,
                                                          sys_nh_info_ip_tunnel_edit_info_t* p_edit_db)
{
    int32 ret = CTC_E_NONE;

    ctc_ip_nh_tunne_info_t *p_tunnel_info = &(p_nh_param->tunnel_info);

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsl3edit_tunnel_v6);
    CTC_PTR_VALID_CHECK(p_edit_db);

    /*dscp*/
    switch (p_tunnel_info->dscp_type)
    {
        case CTC_IP_NH_TUNNEL_DSCP_USE_USER_DEFINE:
            p_dsl3edit_tunnel_v6->derive_dscp = FALSE;
            //CTC_MAX_VALUE_CHECK(p_tunnel_info->dscp_or_tos, 0x7);
            p_dsl3edit_tunnel_v6->tos = p_tunnel_info->dscp_or_tos;
            break;

        case CTC_IP_NH_TUNNEL_DSCP_FROM_SRC_DSCP:
            p_dsl3edit_tunnel_v6->derive_dscp = TRUE;
            p_dsl3edit_tunnel_v6->tos = 0<<0;
            break;

        case CTC_IP_NH_TUNNEL_DSCP_FROM_PKT_PRIORITY:
            p_dsl3edit_tunnel_v6->derive_dscp = TRUE;
            p_dsl3edit_tunnel_v6->tos = 1<<0;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    /*ttl*/
    p_dsl3edit_tunnel_v6->ttl     =  p_tunnel_info->ttl;
    p_dsl3edit_tunnel_v6->map_ttl = CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,
                                                   CTC_IP_NH_TUNNEL_FLAG_MAP_TTL);

    if(CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK))
    {
        p_dsl3edit_tunnel_v6->mtu_check_en = TRUE;
        p_dsl3edit_tunnel_v6->gre_key3116 = p_tunnel_info->mtu_size & 0xFFFF;

    }

    return ret;
}

static INLINE int32
_sys_humber_nh_ip_tunnel_v6_ecap_gre_or_udp(ctc_ip_nh_param_t *p_nh_param,
                                                            ds_l3edit_tunnel_v6_t *p_dsl3edit_tunnel_v6)
{
    int32 ret = CTC_E_NONE;
    ctc_ip_nh_tunne_info_t *p_tunnel_info = &(p_nh_param->tunnel_info);

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsl3edit_tunnel_v6);

    switch (p_tunnel_info->encap_type)
    {
        case CTC_IP_TUNNEL_NH_ENCAP_TYPE_NONE:
             p_dsl3edit_tunnel_v6->inner_header_valid = FALSE;
             switch (p_nh_param->opcode)
             {
                  case CTC_IP_NH_OP_IPV4_IN_IPV6:
                   p_dsl3edit_tunnel_v6->ip_protocol_type = SYS_NH_IP_TUNNEL_IPV4_PROTO_TYPE;
                      break;

                  default:
                      return CTC_E_INVALID_PARAM;
              }
             break;

        case CTC_IP_TUNNEL_NH_ENCAP_TYPE_GRE:

             p_dsl3edit_tunnel_v6->inner_header_valid = TRUE;
             p_dsl3edit_tunnel_v6->gre_flags = p_tunnel_info->encap_info.gre_info.gre_flag;

             if (0 == p_dsl3edit_tunnel_v6->gre_flags)
             {
                 p_dsl3edit_tunnel_v6->inner_header_type = 0;
             }
             else if(1 == p_dsl3edit_tunnel_v6->gre_flags)
             {
                 p_dsl3edit_tunnel_v6->inner_header_type = 0;
                 SYS_NH_DBG_INFO("SDK Not support sequence now\n");
                 return CTC_E_INVALID_PARAM;
             }
             else if(2 == p_dsl3edit_tunnel_v6->gre_flags)
             {
                 p_dsl3edit_tunnel_v6->inner_header_type = 1;
                 p_dsl3edit_tunnel_v6->gre_key150= p_tunnel_info->encap_info.gre_info.gre_key;
             }
             else if(3 == p_dsl3edit_tunnel_v6->gre_flags)
             {
                 p_dsl3edit_tunnel_v6->inner_header_type = 1;
                 SYS_NH_DBG_INFO("SDK Not support sequence now\n");
                 return CTC_E_INVALID_PARAM;
             }
             else
             {
                 SYS_NH_DBG_INFO("Asic Not support\n");
                 return CTC_E_INVALID_PARAM;
             }

             switch (p_nh_param->opcode)
             {
                  case CTC_IP_NH_OP_IPV4_IN_IPV6:
                   p_dsl3edit_tunnel_v6->gre_protocol = 0x0800;
                      break;

                  default:
                      return CTC_E_INVALID_PARAM;
              }
             break;


        default:
            return CTC_E_INVALID_PARAM;
    }


    return ret;

}


static INLINE int32
_sys_humber_nh_ip_tunnel_v4_ecap_ip_header(ctc_ip_nh_param_t *p_nh_param,
                                                          ds_l3edit_tunnel_v4_t *p_dsl3edit_tunnel_v4,
                                                          sys_nh_info_ip_tunnel_edit_info_t* p_edit_db)
{
    int32 ret = CTC_E_NONE;
    ctc_ip_nh_tunne_info_t *p_tunnel_info = &(p_nh_param->tunnel_info);

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsl3edit_tunnel_v4);
    CTC_PTR_VALID_CHECK(p_edit_db);

    /*identfication*/
    p_dsl3edit_tunnel_v4->ip_identific_type = 0;

    /*dscp*/
    switch (p_tunnel_info->dscp_type)
    {
        case CTC_IP_NH_TUNNEL_DSCP_USE_USER_DEFINE:
            p_dsl3edit_tunnel_v4->derive_dscp = FALSE;
            CTC_MAX_VALUE_CHECK(p_tunnel_info->dscp_or_tos, 0x3F);
            p_dsl3edit_tunnel_v4->dscp = p_tunnel_info->dscp_or_tos;
            break;

        case CTC_IP_NH_TUNNEL_DSCP_FROM_SRC_DSCP:
            p_dsl3edit_tunnel_v4->derive_dscp = TRUE;
            p_dsl3edit_tunnel_v4->dscp = 0<<0;
            break;

        case CTC_IP_NH_TUNNEL_DSCP_FROM_PKT_PRIORITY:
            p_dsl3edit_tunnel_v4->derive_dscp = TRUE;
            p_dsl3edit_tunnel_v4->dscp = 1<<0;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    /*ttl*/
    p_dsl3edit_tunnel_v4->ttl     =  p_tunnel_info->ttl;
    p_dsl3edit_tunnel_v4->map_ttl = CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,
                                                   CTC_IP_NH_TUNNEL_FLAG_MAP_TTL);

    /*dont frag*/
    p_dsl3edit_tunnel_v4->dont_frag = CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,
                                                    CTC_IP_NH_TUNNEL_FLAG_DONT_FRAG);

    p_dsl3edit_tunnel_v4->copy_dont_frag = CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,
                                                    CTC_IP_NH_TUNNEL_FLAG_COPY_DONT_FRAG);


    p_dsl3edit_tunnel_v4->mtu_check_en = CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,
                                                    CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK);

    /*ipsa*/
    if (CTC_IP_NH_TUNEEL_TYPE_6TO4 == p_tunnel_info->tunnel_type)
    {
        p_dsl3edit_tunnel_v4->t6to4_tunnel_sa = TRUE;
        if(CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag,CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK))
        {
            p_dsl3edit_tunnel_v4->ip_sa = (((p_nh_param->tunnel_info.mtu_size)& 0x3fff)<<16);
        }
    }
    else if(CTC_IP_TUNNEL_NH_ENCAP_TYPE_GRE == p_tunnel_info->encap_type)
    {
        p_dsl3edit_tunnel_v4->gre_flags = p_tunnel_info->encap_info.gre_info.gre_flag;

        if (CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag, CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK))
        {
            p_dsl3edit_tunnel_v4->ip_sa = (((p_nh_param->tunnel_info.mtu_size)& 0x3fff) << 16);
        }
        if (!CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag, CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK) &&
            ( 2 != p_dsl3edit_tunnel_v4->gre_flags ) && (3 != p_dsl3edit_tunnel_v4->gre_flags) )
        {
            p_dsl3edit_tunnel_v4->ip_sa = p_tunnel_info->ip_sa.ipv4;
        }
        else
        {
            p_edit_db->ip_idx[0].ipv4_sa_idx_valid = 1;
            p_edit_db->ip_idx[1].ipv4_sa_idx_valid = 1;
        }
    }
    else
    {
        p_dsl3edit_tunnel_v4->ip_sa = p_tunnel_info->ip_sa.ipv4;
    }


    /*ipda*/
    if ( (CTC_IP_NH_TUNEEL_TYPE_6TO4 == p_tunnel_info->tunnel_type) ||
         ( CTC_IP_NH_TUNNEL_TYPE_6TO4_MANUAL == p_tunnel_info->tunnel_type ) )
    {
        p_dsl3edit_tunnel_v4->t6to4_tunnel = TRUE;
    }
    else if((CTC_IP_NH_TUNEEL_TYPE_AUTO == p_tunnel_info->tunnel_type) ||
           (CTC_IP_NH_TUNEEL_TYPE_ISATAP == p_tunnel_info->tunnel_type))
    {
       p_dsl3edit_tunnel_v4->is_atp_tunnel = TRUE;

    }
    else
    {
        p_dsl3edit_tunnel_v4->ip_da = p_tunnel_info->ip_da.ipv4;
    }


    return ret;
}

static INLINE int32
_sys_humber_nh_ip_tunnel_v4_ecap_gre_or_udp(ctc_ip_nh_param_t *p_nh_param,
                                                            ds_l3edit_tunnel_v4_t *p_dsl3edit_tunnel_v4)
{
    int32 ret = CTC_E_NONE;
    ctc_ip_nh_tunne_info_t *p_tunnel_info = &(p_nh_param->tunnel_info);

    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsl3edit_tunnel_v4);

    switch (p_tunnel_info->encap_type)
    {
        case CTC_IP_TUNNEL_NH_ENCAP_TYPE_NONE:
             p_dsl3edit_tunnel_v4->inner_header_valid = FALSE;
             switch (p_nh_param->opcode)
             {
                  case CTC_IP_NH_OP_IPV6_IN_IPV4:
                   p_dsl3edit_tunnel_v4->ip_protocol_type = SYS_NH_IP_TUNNEL_IPV6_PROTO_TYPE;
                      break;
                  case CTC_IP_NH_OP_ETH_IN_IPV4_FOR_MIRROR:
                    break;
                  default:
                      return CTC_E_INVALID_PARAM;
              }
             break;

        case CTC_IP_TUNNEL_NH_ENCAP_TYPE_GRE:

             p_dsl3edit_tunnel_v4->inner_header_valid = TRUE;
             p_dsl3edit_tunnel_v4->gre_flags = p_tunnel_info->encap_info.gre_info.gre_flag;

             if (0 == p_dsl3edit_tunnel_v4->gre_flags)
             {
                 p_dsl3edit_tunnel_v4->inner_header_type = 0;
             }
             else if(1 == p_dsl3edit_tunnel_v4->gre_flags)
             {
                 p_dsl3edit_tunnel_v4->inner_header_type = 0;
                 SYS_NH_DBG_INFO("SDK Not support sequence now\n");
                 return CTC_E_INVALID_PARAM;
             }
             else if(2 == p_dsl3edit_tunnel_v4->gre_flags)
             {
                if (CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag, CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK) &&
                    p_tunnel_info->encap_info.gre_info.gre_key > CTC_MAX_UINT16_VALUE)
                {
                    /* If check mtu, the humber can support 16bits tunnel id */
                    return CTC_E_INVALID_PARAM;
                }
                else if (!CTC_FLAG_ISSET(p_tunnel_info->tunnel_flag, CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK) &&
                    p_tunnel_info->encap_info.gre_info.gre_key > CTC_MAX_UINT32_VALUE)
                {
                    /* If not check mtu, the humber can support 32bits tunnel id */
                    return CTC_E_INVALID_PARAM;
                }
                p_dsl3edit_tunnel_v4->inner_header_type = 1;
                p_dsl3edit_tunnel_v4->gre_key_udp_dest_port = p_tunnel_info->encap_info.gre_info.gre_key;
             }
             else if(3 == p_dsl3edit_tunnel_v4->gre_flags)
             {
                 p_dsl3edit_tunnel_v4->inner_header_type = 1;
                 SYS_NH_DBG_INFO("SDK Not support sequence now\n");
                 return CTC_E_INVALID_PARAM;
             }
             else
             {
                 SYS_NH_DBG_INFO("Asic Not support\n");
                 return CTC_E_INVALID_PARAM;
             }

             switch (p_nh_param->opcode)
             {
                  case CTC_IP_NH_OP_IPV6_IN_IPV4:
                   p_dsl3edit_tunnel_v4->gre_protocol_udp_src_port = 0x86DD;
                      break;
                  case CTC_IP_NH_OP_ETH_IN_IPV4_FOR_MIRROR:
                   p_dsl3edit_tunnel_v4->gre_protocol_udp_src_port = 0x6558 ;
                   break;
                  default:
                      return CTC_E_INVALID_PARAM;
              }
             break;

        case CTC_IP_TUNNEL_NH_ENCAP_TYPE_UDP:
             SYS_NH_DBG_INFO("Not support now\n");
             return CTC_E_INVALID_PARAM;

        default:
            return CTC_E_INVALID_PARAM;
    }


    return ret;

}

static INLINE int32
_sys_humber_nh_ip_tunnel_build_dsl3edit(ctc_ip_nh_param_t *p_nh_param,
                                                   void *p_ds_l3edit_tunnel,
                                                   sys_nh_info_ip_tunnel_edit_info_t* p_edit_db,
                                                   bool is_v4_tunnel)
{
    ds_l3edit_tunnel_v4_t *p_dsl3edit_tunnel_v4 = NULL;
    ds_l3edit_tunnel_v6_t *p_dsl3edit_tunnel_v6 = NULL;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_ds_l3edit_tunnel);
    CTC_PTR_VALID_CHECK(p_edit_db);

    if (is_v4_tunnel)
    {
        p_edit_db->dsl3edit_type = SYS_NH_L3EDIT_TYPE_TUNNEL_V4;
        p_dsl3edit_tunnel_v4 = (ds_l3edit_tunnel_v4_t *) p_ds_l3edit_tunnel;
        CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_v4_ecap_ip_header(p_nh_param, p_dsl3edit_tunnel_v4,p_edit_db));
        CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_v4_ecap_gre_or_udp(p_nh_param, p_dsl3edit_tunnel_v4));
    }
    else
    {
        p_edit_db->dsl3edit_type = SYS_NH_L3EDIT_TYPE_TUNNEL_V6;
        p_dsl3edit_tunnel_v6 = (ds_l3edit_tunnel_v6_t *) p_ds_l3edit_tunnel;
        CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_v6_ecap_ip_header(p_nh_param, p_dsl3edit_tunnel_v6,p_edit_db));
        CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_v6_ecap_gre_or_udp(p_nh_param, p_dsl3edit_tunnel_v6));
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_ip_tunnel_build_dsl2edit(ctc_ip_nh_param_t *p_nh_param,
                                                    sys_nh_db_dsl2editeth4w_t* p_dsl2edit)
{
    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsl2edit);

    kal_memset(p_dsl2edit, 0, sizeof(sys_nh_db_dsl2editeth4w_t));

    if (CTC_FLAG_ISSET(p_nh_param->tunnel_info.tunnel_flag,CTC_IP_NH_TUNNEL_FLAG_LOOPBACK))
    {
        p_dsl2edit->hdr.entry_type = SYS_NH_ENTRY_TYPE_NULL;
    }
    else
    {
        kal_memcpy(p_dsl2edit->mac_da, p_nh_param->mac, sizeof(mac_addr_t));
        p_dsl2edit->output_vid = p_nh_param->oif.vid;
        p_dsl2edit->ouput_vlan_is_svlan = p_nh_param->oif.outervlan_is_svlan;
        p_dsl2edit->hdr.entry_type = SYS_NH_ENTRY_TYPE_L2EDIT_ETH_4W;
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_ip_tunnel_build_dsnh(ctc_ip_nh_param_t *p_nh_param,
                                                sys_nh_param_dsnh_t *p_dsnh_param,
                                                sys_nh_info_ip_tunnel_edit_info_t* p_edit_db,
                                                uint32 dsnh_offset,
                                                bool use_dsnh8w)
{
    sys_vlan_info_t vlan_info;
    uint8 gchip,lchip;

    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_nh_param);
    CTC_PTR_VALID_CHECK(p_dsnh_param);
    CTC_PTR_VALID_CHECK(p_edit_db);

    kal_memset(p_dsnh_param, 0, sizeof(sys_nh_param_dsnh_t));
    p_dsnh_param->dsnh_offset = dsnh_offset;
    SYS_NH_DBG_INFO("DsNexthop Offset %d, useDsNexthop8W = %d, IPOpCode = %d\n",
        dsnh_offset, use_dsnh8w, p_nh_param->opcode);

    switch(p_nh_param->opcode)
    {
        case CTC_IP_NH_OP_IPV6_IN_IPV4:
        case CTC_IP_NH_OP_IPV4_IN_IPV6:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_ROUTE;
            break;
        case CTC_IP_NH_OP_ETH_IN_IPV4_FOR_MIRROR:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_MIRROR;
            break;
        case CTC_IP_NH_OP_ETH_IN_GRE:
            p_dsnh_param->dsnh_type = SYS_NH_PARAM_DSNH_TYPE_IP_TUNNEL_ENCAP_OP_GRE;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    vlan_info.vid = p_nh_param->oif.vid;
    vlan_info.gport = p_nh_param->oif.gport;
    switch(p_nh_param->oif.oif_type)
    {
        case CTC_NH_OIF_TYPE_VLAN_PORT:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_VLAN_IF;
            break;
        case CTC_NH_OIF_TYPE_ROUTED_PORT:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_PHY_IF;
            break;
        case CTC_NH_OIF_TYPE_SUB_IF:
            vlan_info.vlan_ptr_type = SYS_VLAN_PTR_TYPE_SUB_IF;
            break;
        default:
            return CTC_E_L3IF_INVALID_IF_TYPE;
    }
    p_edit_db->gport = p_nh_param->oif.gport;


    if (CTC_IS_LINKAGG_PORT( p_edit_db->gport ))
    {
        /*store l3ifid into ip tunnel nexthop db*/
        CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &p_dsnh_param->dest_vlan_ptr));
        CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_edit_db->l3ifid)));
        SYS_NH_DBG_INFO("Layer3IfId = %d\n", p_edit_db->l3ifid);
    }
    else
    {
        gchip = SYS_MAP_GPORT_TO_GCHIP( p_edit_db->gport );
        if (sys_humber_chip_is_local(gchip, &lchip))
        {
            /*store l3ifid into ipuc nexthop db*/
            CTC_ERROR_RETURN(sys_humber_vlan_get_vlan_ptr(&vlan_info, &p_dsnh_param->dest_vlan_ptr));
            CTC_ERROR_RETURN(sys_humber_vlan_get_l3if_id(&vlan_info, &(p_edit_db->l3ifid)));
            SYS_NH_DBG_INFO("Layer3IfId = %d\n", p_edit_db->l3ifid);
        }
        else
        {
            p_dsnh_param->dest_vlan_ptr = SYS_HUMBER_DSNH_DESTVLANPTR_SPECIAL;
        }
    }

    SYS_NH_DBG_INFO("DestVlanPtr = %d\n", p_dsnh_param->dest_vlan_ptr);

    return CTC_E_NONE;
}


static INLINE int32
_sys_humber_nh_ip_tunnel_write_dsl3edit(uint8 lchip,
                                    ctc_ip_nh_param_t *p_ip_nh_param,
                                    void *p_ds_l3edit_tunnel,
                                    sys_nh_info_ip_tunnel_edit_info_t* p_edit_db)
{
    int32 ret = CTC_E_NONE;
    ds_l3edit_tunnel_v4_t *p_dsl3edit_tunnel_v4 = NULL;
    ds_l3edit_tunnel_v6_t *p_dsl3edit_tunnel_v6 = NULL;

    switch(p_edit_db->dsl3edit_type)
    {
    case SYS_NH_L3EDIT_TYPE_TUNNEL_V4:

         p_dsl3edit_tunnel_v4 = (ds_l3edit_tunnel_v4_t *)p_ds_l3edit_tunnel;
        /*ipsa*/
         if (p_edit_db->ip_idx[lchip].ipv4_sa_idx_valid)
        {
            CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_ip_idx_alloc(lchip, CTC_IP_VER_4, \
                                                  &p_ip_nh_param->tunnel_info.ip_sa.ipv4,\
                                                  &(p_edit_db->ip_idx[lchip].ipv4_sa_idx)));

            if (CTC_FLAG_ISSET(p_ip_nh_param->tunnel_info.tunnel_flag, CTC_IP_NH_TUNNEL_FLAG_MTU_CHECK))
            {
                p_dsl3edit_tunnel_v4->ip_sa = ((((p_ip_nh_param->tunnel_info.mtu_size)& 0x3fff)<<16) )| \
                                             (p_edit_db->ip_idx[lchip].ipv4_sa_idx &0x3f);
            }
            else
            {
                p_dsl3edit_tunnel_v4->ip_sa = (p_ip_nh_param->tunnel_info.encap_info.gre_info.gre_key & 0xffff0000 )| \
                                             (p_edit_db->ip_idx[lchip].ipv4_sa_idx &0x3f);
            }
        }

        /* Allocate new dsl3edit v4 entry*/
        ret = sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4, 1, \
                                                    &p_edit_db->dsl3edit_offset[lchip]);
        if (CTC_E_NONE != ret)
        {
             /*Callback Free ipsa index*/
             if (p_edit_db->ip_idx[lchip].ipv4_sa_idx_valid)
             {
                _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_4, \
                                                    p_edit_db->ip_idx[lchip].ipv4_sa_idx);
             }
             return ret;
        }

        /* Write L3edit v4  HW table*/
        ret = sys_humber_nh_write_asic_table(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4, \
                                                        p_edit_db->dsl3edit_offset[lchip], \
                                                        p_dsl3edit_tunnel_v4);
        if (CTC_E_NONE != ret)
        {
             /*Callback Free ipsa index*/
             if (p_edit_db->ip_idx[lchip].ipv4_sa_idx_valid)
             {
                _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_4, \
                                      p_edit_db->ip_idx[lchip].ipv4_sa_idx);
             }
              /*Callback Free l3edit offset index*/
             sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4, 1, \
                                  p_edit_db->dsl3edit_offset[lchip]);

             return ret;
        }

        SYS_NH_DBG_INFO("Lchip = %d, dsl3edit_tunnel_v4, offset = %d\n",
            lchip, p_edit_db->dsl3edit_offset[lchip]);
        break;

    case SYS_NH_L3EDIT_TYPE_TUNNEL_V6:

         p_dsl3edit_tunnel_v6 = (ds_l3edit_tunnel_v6_t *)p_ds_l3edit_tunnel;

        /*ipsa*/
        CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_ip_idx_alloc(lchip, CTC_IP_VER_6, \
                                             &p_ip_nh_param->tunnel_info.ip_sa.ipv6, \
                                             &(p_edit_db->ip_idx[lchip].ipv6_sa_idx)));
        p_dsl3edit_tunnel_v6->ipsa_index = p_edit_db->ip_idx[lchip].ipv6_sa_idx;

        /*ipda*/
        ret = _sys_humber_nh_ip_tunnel_ip_idx_alloc(lchip, CTC_IP_VER_6, \
                                 &p_ip_nh_param->tunnel_info.ip_da.ipv6, \
                                 &(p_edit_db->ip_idx[lchip].ipv6_da_idx));
        p_dsl3edit_tunnel_v6->ipda_index = p_edit_db->ip_idx[lchip].ipv6_da_idx;

        if (CTC_E_NONE != ret)
        {
             /*Callback Free ipsa index*/
             _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6,\
                                  p_edit_db->ip_idx[lchip].ipv6_sa_idx);
             return ret;
        }

        /* Allocate new dsl3edit ip tunnel v4 entry*/
        ret = sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6, 1, \
                                                    &p_edit_db->dsl3edit_offset[lchip]);
        if (CTC_E_NONE != ret)
        {
             /*Callback Free ipsa index*/
             _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6,\
                                  p_edit_db->ip_idx[lchip].ipv6_sa_idx);
             /*Callback Free ipda index*/
             _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6,\
                                  p_edit_db->ip_idx[lchip].ipv6_da_idx);
             return ret;
        }

        /* Write HW table*/
        ret = sys_humber_nh_write_asic_table(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6, \
                                                        p_edit_db->dsl3edit_offset[lchip],
                                                        p_dsl3edit_tunnel_v6);

        if (CTC_E_NONE != ret)
        {
             /*Callback Free ipsa index*/
             _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6,\
                                  p_edit_db->ip_idx[lchip].ipv6_sa_idx);
             /*Callback Free ipda index*/
             _sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6,\
                                  p_edit_db->ip_idx[lchip].ipv6_da_idx);
             /*Callback Free l3edit v6 offset*/
              sys_humber_nh_offset_free(lchip, SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6, 1, \
                           p_edit_db->dsl3edit_offset[lchip]);
             return ret;
        }

        SYS_NH_DBG_INFO("Lchip = %d, dsl3edit_tunnel_v6, offset = %d\n",
            lchip, p_edit_db->dsl3edit_offset[lchip]);
        break;

    case SYS_NH_L3EDIT_TYPE_NONE:
        SYS_NH_DBG_INFO("Lchip = %d, No DsL3Edit Table\n", lchip);
        break;

    default:
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;

}


static INLINE int32
_sys_humber_nh_ip_tunnel_op_edit_table(ctc_ip_nh_param_t *p_ip_nh_param,
                                  sys_nh_info_ip_tunnel_edit_info_t* p_edit_db, uint8 curr_chip_num,
                                  bool use_dsnh8w, uint32 dsnh_offset)
{
    uint8 gchip, tmp_lchip, lchip;
    ds_l3edit_tunnel_v4_t dsl3edit_tunnel_v4;
    ds_l3edit_tunnel_v6_t dsl3edit_tunnel_v6;
    void *p_ds_l3edit_tunnel_t = NULL;
    sys_nh_db_dsl2editeth4w_t dsl2edit;
    sys_nh_param_dsnh_t dsnh_param;
    bool is_glb_alloc_nexthop;
    bool is_v4_tunnel = FALSE;
    bool is_loopback = FALSE;
    uint16 gport = 0;
    ctc_vlan_egress_edit_info_t vlan_info;

    SYS_NH_DBG_FUNC();
    CTC_PTR_VALID_CHECK(p_ip_nh_param);
    CTC_PTR_VALID_CHECK(p_edit_db)

    if (CTC_IP_NH_OP_IPV6_IN_IPV4 == p_ip_nh_param->opcode ||
        CTC_IP_NH_OP_ETH_IN_IPV4_FOR_MIRROR  == p_ip_nh_param->opcode ||
        CTC_IP_NH_OP_ETH_IN_GRE == p_ip_nh_param->opcode)
    {
        kal_memset(&dsl3edit_tunnel_v4,0,sizeof(ds_l3edit_tunnel_v4_t));
        p_ds_l3edit_tunnel_t = &dsl3edit_tunnel_v4;
        is_v4_tunnel = TRUE;
    }
    else if (CTC_IP_NH_OP_IPV4_IN_IPV6 == p_ip_nh_param->opcode)
    {
        kal_memset(&dsl3edit_tunnel_v6,0,sizeof(ds_l3edit_tunnel_v4_t));
        p_ds_l3edit_tunnel_t = &dsl3edit_tunnel_v6;
        is_v4_tunnel = FALSE;
    }
    else
    {
        return CTC_E_INVALID_PARAM;
    }

    /*1. Build dsL3Edit*/
    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_build_dsl3edit(p_ip_nh_param,
                                      p_ds_l3edit_tunnel_t, p_edit_db, is_v4_tunnel));

    /*2. Build dsL2Edit*/
    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_build_dsl2edit(p_ip_nh_param, &dsl2edit));

    is_glb_alloc_nexthop = sys_humber_is_glb_alloc_nexthop();

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*if need loopback, get egress loopback port*/
        if (CTC_FLAG_ISSET(p_ip_nh_param->tunnel_info.tunnel_flag,CTC_IP_NH_TUNNEL_FLAG_LOOPBACK))
        {
            CTC_ERROR_RETURN(sys_humber_internal_port_get_eloop_port(CTC_INTERNAL_PORT_ELOOP_TYPE_ROUTE,
                                                                     lchip, &gport));
            is_loopback = TRUE;
            p_ip_nh_param->oif.gport = gport;
            p_ip_nh_param->oif.oif_type = CTC_NH_OIF_TYPE_ROUTED_PORT;
        }
        else
        {
            gchip = SYS_MAP_GPORT_TO_GCHIP(p_ip_nh_param->oif.gport);
            sys_humber_chip_is_local(gchip, &tmp_lchip);
            is_loopback = FALSE;
        }

            /*3. Build dsNexthop*/
         CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_build_dsnh(p_ip_nh_param,
                                     &dsnh_param, p_edit_db, dsnh_offset, use_dsnh8w));

        if(CTC_IS_LINKAGG_PORT(p_ip_nh_param->oif.gport) || (lchip == tmp_lchip)
            || is_glb_alloc_nexthop || is_loopback)
        {
            /*Write DsL3Edit to Asic*/
            CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_write_dsl3edit(lchip,p_ip_nh_param, \
                                                    p_ds_l3edit_tunnel_t, p_edit_db));

            /*Write DsL2Edit to Asic*/
            if(SYS_NH_ENTRY_TYPE_NULL != dsl2edit.hdr.entry_type)
            {
                sys_nh_db_dsl2editeth4w_t *p_dsl2edit;
                p_dsl2edit = &dsl2edit;
                p_dsl2edit->hdr.lchip = lchip;
                CTC_ERROR_RETURN(sys_humber_nh_db_set_entry((sys_nh_db_com_entry_t**)(&p_dsl2edit)));
                p_edit_db->p_dsl2edit_info[lchip] = p_dsl2edit;
                p_edit_db->dsl2edit_type = SYS_NH_L2EDIT_TYPE_ETH_4W;
                dsnh_param.l2edit_ptr = p_dsl2edit->hdr.offset;
                dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_ETH_4W;
                SYS_NH_DBG_INFO("Lchip = %d, DsL2EditEth4w, offset = %d\n", lchip, p_dsl2edit->hdr.offset);
            }
            else
            {
                p_edit_db->p_dsl2edit_info[lchip] = NULL;
                p_edit_db->dsl2edit_type = SYS_NH_L2EDIT_TYPE_NONE;
                dsnh_param.l2edit_ptr = 0x7FFF;
                dsnh_param.l2edit_type = SYS_NH_L2EDIT_TYPE_NONE;
                SYS_NH_DBG_INFO("Lchip = %d, No DsL2EditEth4W table\n", lchip);
            }

            /*Write DsNexthop to Asic*/
            dsnh_param.l3dit_type = p_edit_db->dsl3edit_type;
            dsnh_param.l3edit_ptr = p_edit_db->dsl3edit_offset[lchip];

            dsnh_param.lchip = lchip;

            if( 2 == p_ip_nh_param->tunnel_info.dscp_type )
            {
                dsnh_param.dscp_en = 1;
            }
            else
            {
                dsnh_param.dscp_en = 0;
            }

            if (p_ip_nh_param->tunnel_info.strip_svlan)
            {
                memset(&vlan_info, 0, sizeof(vlan_info));
                dsnh_param.p_vlan_info = &vlan_info;

                vlan_info.svlan_edit_type = CTC_VLAN_EGRESS_EDIT_STRIP_VLAN;

                /* strip cvlan only when stripping svlan */
                if (p_ip_nh_param->tunnel_info.strip_cvlan)
                {
                    vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_STRIP_VLAN;
                }
                else
                {
                    vlan_info.cvlan_edit_type = CTC_VLAN_EGRESS_EDIT_KEEP_VLAN_UNCHANGE;
                }
            }

            if (use_dsnh8w)
            {
                SYS_NH_DBG_INFO("Lchip = %d, Write DsNexthop8w Table\n", lchip);
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh8w(&dsnh_param));
            }
            else
            {
                SYS_NH_DBG_INFO("Lchip = %d, Write DsNexthop4w Table\n", lchip);
                CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsnh4w(&dsnh_param));
            }

            CTC_SET_FLAG(p_edit_db->lchip_bmp, (1 << lchip));

        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_nh_create_ip_tunnel_cb(sys_nh_param_com_t* p_com_nh_para,
                                             sys_nh_info_com_t* p_com_db)
{
    uint8   lchip          = 0;
    uint8   curr_chip_num  = 0;
    uint8   dsnh_entry_num = 0;
    uint16  gport          = 0;
    bool    use_dsnh8w     = FALSE;
    sys_nh_param_ip_tunnel_t* p_nh_param  = NULL;
    sys_nh_info_ip_tunnel_t * p_nhdb      = NULL;
    ctc_ip_nh_param_t *p_nh_ip_param      = NULL;
    sys_nh_param_dsfwd_t     dsfwd_param = { 0 };

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_com_nh_para);
    CTC_PTR_VALID_CHECK(p_com_db);
    CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IP_TUNNEL, p_com_nh_para->hdr.nh_param_type);
    p_nh_param = (sys_nh_param_ip_tunnel_t*)(p_com_nh_para);

    p_nh_ip_param = &(p_nh_param->ip_nh_param);

    p_nhdb     = (sys_nh_info_ip_tunnel_t*)(p_com_db);
    p_nhdb->hdr.nh_entry_type = SYS_HUMBER_NH_TYPE_IP_TUNNEL;
    p_nhdb->dsnh_offset = p_nh_param->dsnh_offset;
    curr_chip_num = sys_humber_get_local_chip_num();

    if(CTC_FLAG_ISSET(p_nh_ip_param->flag,CTC_IP_NH_FLAG_USE_DSNH8W))
    {
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_IP_TUNNEL_FLAG_USE_DSNH8W);
        use_dsnh8w = TRUE;
    }

    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_get_dsnh_entry_num(p_nhdb->hdr.nh_entry_flags,
                                                                 &dsnh_entry_num));
    CTC_ERROR_RETURN(sys_humber_nh_check_glb_nh_sram_offset(p_nhdb->dsnh_offset,dsnh_entry_num, TRUE));

    /*1.Create unresolved ip-tunnel nh*/
    if(CTC_FLAG_ISSET(p_nh_ip_param->flag,CTC_IP_NH_FLAG_UNROV))
    {
        sys_nh_param_special_t nh_para_spec;
        sys_nh_info_special_t nhdb_spec;
        kal_memset(&nhdb_spec, 0, sizeof(nhdb_spec));
        kal_memset(&nh_para_spec, 0, sizeof(nh_para_spec));
        nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
        nh_para_spec.hdr.is_internal_nh = TRUE;
        SYS_NH_DBG_INFO("Create unresolved ip tunnel nexthop\n");

        CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
            sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));
        for(lchip = 0; lchip < curr_chip_num; lchip++)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = nhdb_spec.hdr.dsfwd_info[lchip].dsfwd_offset;
            SYS_NH_DBG_INFO("Unresolved ip tunnel nexthop's dsfwd offset is 0x%x\n",
                p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset);
        }
        CTC_SET_FLAG(p_nhdb->hdr.nh_entry_flags, SYS_NH_INFO_FLAG_IS_UNROV);
        CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_offset,
                                                               dsnh_entry_num, TRUE));
        return CTC_E_NONE;
    }

    /*2.Create normal ip-tunnel nh*/
    /*2.1 op dsnh,l2edit,l3edit*/
    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_op_edit_table(p_nh_ip_param,
                            &(p_nhdb->ip_tunnel_edit_info), curr_chip_num,
                            use_dsnh8w,p_nh_param->dsnh_offset));

    dsfwd_param.is_mcast = FALSE;
    dsfwd_param.dsnh_offset = p_nh_param->dsnh_offset;
    dsfwd_param.nexthop_ext = use_dsnh8w;

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*if need loopback, get egress loopback port*/
        if (CTC_FLAG_ISSET(p_nh_ip_param->tunnel_info.tunnel_flag,CTC_IP_NH_TUNNEL_FLAG_LOOPBACK))
        {
            CTC_ERROR_RETURN(sys_humber_internal_port_get_eloop_port(CTC_INTERNAL_PORT_ELOOP_TYPE_ROUTE,
                                                                     lchip, &gport));
        }
        else
        {
            gport = p_nh_ip_param->oif.gport;
        }

        dsfwd_param.dest_chipid = SYS_MAP_GPORT_TO_GCHIP(gport);
        dsfwd_param.dest_id = CTC_MAP_GPORT_TO_LPORT(gport);

        /*2.2 Build DsFwd Table*/
        if (p_nh_param->hdr.dsfwd_valid)
        {
            p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset = p_nh_param->hdr.dsfwd_offset[lchip];
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_nh_offset_alloc(lchip, SYS_NH_ENTRY_TYPE_FWD, 1,
                                         &(p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset)));
            SYS_NH_DBG_INFO("Lchip = %d, DsFwdOffset = %d\n", lchip,
                (p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset));
            p_nh_param->hdr.dsfwd_offset[lchip] = (p_nhdb->hdr.dsfwd_info[lchip].dsfwd_offset);
        }
        dsfwd_param.p_dsfwd_info = &(p_nhdb->hdr.dsfwd_info[lchip]);
        dsfwd_param.lchip = lchip;
        SYS_NH_DBG_INFO("DsFwd(Lchip : %d) :: DestChipId = %d, DestId = %d"
            "DsNexthop Offset = %d, DsNexthopExt = %d\n", lchip,
            dsfwd_param.dest_chipid, dsfwd_param.dest_id,
            dsfwd_param.dsnh_offset, dsfwd_param.nexthop_ext);
        /*2.3 Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));

    }

    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhdb->dsnh_offset,
        dsnh_entry_num, TRUE));

    return CTC_E_NONE;
}

 static INLINE int32
 _sys_humber_nh_ip_tunnel_unset_dsedit(uint8 lchip,
                                                   sys_nh_info_ip_tunnel_edit_info_t* p_editinfo)
{
     SYS_NH_DBG_FUNC();

     /*1. Free DsL3Edit offset*/
     switch(p_editinfo->dsl3edit_type)
     {
         case SYS_NH_L3EDIT_TYPE_TUNNEL_V4:
             CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
                                                        SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V4, 1, \
                                                        p_editinfo->dsl3edit_offset[lchip]));

             /*Free ipsa index*/
             if (p_editinfo->ip_idx[lchip].ipv4_sa_idx_valid)
             {
                CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_4, p_editinfo->ip_idx[lchip].ipv4_sa_idx));
             }

             SYS_NH_DBG_INFO("Free DsL3Edit tunnel v4 Offset = %d, ipsa_index = %d\n",
                                                    p_editinfo->dsl3edit_offset[lchip],
                                                    p_editinfo->ip_idx[lchip].ipv4_sa_idx);

             break;

         case SYS_NH_L3EDIT_TYPE_TUNNEL_V6:
             CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
                                                        SYS_NH_ENTRY_TYPE_L3EDIT_TUNNEL_V6, 1, \
                                                        p_editinfo->dsl3edit_offset[lchip]));

             SYS_NH_DBG_INFO("Free DsL3Edit tunnel v6 Offset=%d, ipsa_index=%d, ipda_index =%d\n",
                                                      p_editinfo->dsl3edit_offset[lchip],
                                                      p_editinfo->ip_idx[lchip].ipv6_sa_idx,p_editinfo->ip_idx[lchip].ipv6_da_idx);
             /*Free ipsa index*/

             CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6, p_editinfo->ip_idx[lchip].ipv6_sa_idx));
                /*Free ipda index*/
             CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_ip_idx_free(lchip, CTC_IP_VER_6, p_editinfo->ip_idx[lchip].ipv6_da_idx));


             SYS_NH_DBG_INFO("Free DsL3Edit tunnel v6 Offset=%d, ipsa_index=%d, ipda_index =%d\n",
                                                      p_editinfo->dsl3edit_offset[lchip],
                                                      p_editinfo->ip_idx[lchip].ipv6_sa_idx,p_editinfo->ip_idx[lchip].ipv6_da_idx);
             break;

         default:
             return CTC_E_INVALID_PARAM;
     }

     /*2. Free DsL2Edit offset*/
     if (p_editinfo->p_dsl2edit_info[lchip])
     {
         /*1. Op dsL2Edit*/
         CTC_ERROR_RETURN(sys_humber_nh_db_remove_entry(
         (sys_nh_db_com_entry_t*)(p_editinfo->p_dsl2edit_info[lchip])));
         p_editinfo->p_dsl2edit_info[lchip] = NULL;
     }

    return CTC_E_NONE;
}


int32
sys_humber_nh_delete_ip_tunnel_cb(sys_nh_info_com_t *p_data)
{

    uint8 lchip, curr_chip_num, dsnh_entry_num = 0;
    uint32 entry_type = SYS_HUMBER_NH_TYPE_IP_TUNNEL;
    sys_nh_param_dsfwd_t    dsfwd_param = {0};
    sys_nh_info_ip_tunnel_t *p_nhinfo   = NULL;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_data);
    CTC_EQUAL_CHECK(entry_type, p_data->hdr.nh_entry_type);
    p_nhinfo = (sys_nh_info_ip_tunnel_t *)(p_data);
    curr_chip_num = sys_humber_get_local_chip_num();

    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags,
                                                                 &dsnh_entry_num));

    /*2. Delete this ip tunnel nexthop*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        dsfwd_param.p_dsfwd_info = &p_nhinfo->hdr.dsfwd_info[lchip];
        dsfwd_param.lchip = lchip;
        dsfwd_param.drop_pkt = 1;
        /*Write table*/
        CTC_ERROR_RETURN(sys_humber_nh_write_entry_dsfwd(&dsfwd_param));
        /*Free DsFwd offset*/
        CTC_ERROR_RETURN(sys_humber_nh_offset_free(lchip, \
            SYS_NH_ENTRY_TYPE_FWD, 1, p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset));

        if(CTC_FLAG_ISSET(p_nhinfo->ip_tunnel_edit_info.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_unset_dsedit(lchip, &(p_nhinfo->ip_tunnel_edit_info)));
            CTC_UNSET_FLAG(p_nhinfo->ip_tunnel_edit_info.lchip_bmp, (1 << lchip));
        }
    }

    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_offset,
                                                           dsnh_entry_num, FALSE));

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_ip_tunnel_fwd_to_spec(sys_nh_param_ip_tunnel_t *p_nhpara,
                                                            sys_nh_info_ip_tunnel_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_param_special_t nh_para_spec;
    sys_nh_info_special_t  nhdb_spec;
    sys_nh_ref_list_node_t *p_ref_nh_list = NULL;
    SYS_NH_DBG_FUNC();

    kal_memset(&nh_para_spec, 0, sizeof(sys_nh_param_special_t));
    kal_memset(&nhdb_spec, 0, sizeof(sys_nh_info_special_t));
    curr_chip_num = sys_humber_get_local_chip_num();
    nh_para_spec.hdr.dsfwd_valid = TRUE;
    nh_para_spec.hdr.nh_param_type = SYS_HUMBER_NH_TYPE_UNROV;
    nh_para_spec.hdr.is_internal_nh = TRUE;

    CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags,
                                                                 &dsnh_entry_num));

    /*1.1 update dsfwd to unrov nh's dsfwd, assign dsfwd ptr*/
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        nh_para_spec.hdr.dsfwd_offset[lchip] = p_nhinfo->hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    /*1.2 update dsfwd to unrov nh's dsfwd, write dsfwd*/
    CTC_ERROR_RETURN(sys_humber_nh_create_special_cb((
        sys_nh_param_com_t*)(&nh_para_spec), (sys_nh_info_com_t*)(&nhdb_spec)));
    CTC_SET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_IP_TUNNEL_FLAG_IS_UNROV);

    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item((sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo,
                                                         p_nhpara->hdr.nhid));
    }

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /*1.2 Remove DsL2Edit & DsNexthop*/
        if(CTC_FLAG_ISSET(p_nhinfo->ip_tunnel_edit_info.lchip_bmp, (1 << lchip)))
        {
            CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_unset_dsedit(lchip, &(p_nhinfo->ip_tunnel_edit_info)));
            CTC_UNSET_FLAG(p_nhinfo->ip_tunnel_edit_info.lchip_bmp, (1 << lchip));
        }
    }

    return CTC_E_NONE;
}

static INLINE int32
_sys_humber_nh_update_ip_tunnel_fwd_attr(sys_nh_param_ip_tunnel_t *p_nhpara,
                                                      sys_nh_info_ip_tunnel_t *p_nhinfo)
{
    uint8 lchip, curr_chip_num, dsnh_entry_num;
    sys_nh_info_ip_tunnel_t nhinfo_tmp;
    sys_nh_ref_list_node_t *p_ref_nh_list;
    SYS_NH_DBG_FUNC();

    curr_chip_num = sys_humber_get_local_chip_num();
    /*Build nhpara*/
    p_nhpara->hdr.dsfwd_valid = TRUE;
    _sys_humber_nh_ip_tunnel_get_dsnh_entry_num(p_nhinfo->hdr.nh_entry_flags, &dsnh_entry_num);
    memcpy(&nhinfo_tmp, p_nhinfo, sizeof(sys_nh_info_ip_tunnel_t));
    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        p_nhpara->hdr.dsfwd_offset[lchip] = nhinfo_tmp.hdr.dsfwd_info[lchip].dsfwd_offset;
    }

    p_nhpara->dsnh_offset = p_nhinfo->dsnh_offset;
    CTC_ERROR_RETURN(sys_humber_nh_set_glb_nh_sram_offset(p_nhinfo->dsnh_offset, dsnh_entry_num, FALSE));
    if(CTC_FLAG_ISSET(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_IP_TUNNEL_FLAG_USE_DSNH8W))
    {
        CTC_SET_FLAG(p_nhpara->ip_nh_param.flag, CTC_IP_NH_FLAG_USE_DSNH8W);
    }
    CTC_ERROR_RETURN(sys_humber_nh_create_ip_tunnel_cb((
        sys_nh_param_com_t*)p_nhpara, (sys_nh_info_com_t*)p_nhinfo));
    CTC_UNSET_FLAG(p_nhinfo->hdr.nh_entry_flags, SYS_NH_INFO_MPLS_FLAG_IS_UNROV);

    for(p_ref_nh_list = p_nhinfo->hdr.p_ref_nh_list; p_ref_nh_list; p_ref_nh_list = p_ref_nh_list->p_next)
    {
        CTC_ERROR_RETURN(sys_humber_nh_ecmp_update_item((sys_nh_info_ecmp_t*)p_ref_nh_list->p_ref_nhinfo, p_nhpara->hdr.nhid));
    }

    for(lchip = 0; lchip < curr_chip_num; lchip ++)
    {
        /* Remove dsl2edit, dsnh*/
        for(lchip = 0; lchip < curr_chip_num; lchip ++)
        {
            /*1.2 Remove DsL2Edit & DsNexthop*/
            if(CTC_FLAG_ISSET(nhinfo_tmp.ip_tunnel_edit_info.lchip_bmp, (1 << lchip)))
            {
                 CTC_ERROR_RETURN(_sys_humber_nh_ip_tunnel_unset_dsedit(lchip, &(nhinfo_tmp.ip_tunnel_edit_info)));
            }
            CTC_UNSET_FLAG(nhinfo_tmp.ip_tunnel_edit_info.lchip_bmp, (1 << lchip));
        }

    }

    return CTC_E_NONE;

}


int32
sys_humber_nh_update_ip_tunnel_cb(sys_nh_info_com_t* p_nh_db,
                                              sys_nh_param_com_t* p_para)
{
    sys_nh_info_ip_tunnel_t *p_nh_info;
    sys_nh_param_ip_tunnel_t* p_nh_para;

    SYS_NH_DBG_FUNC();

    CTC_PTR_VALID_CHECK(p_para);
    CTC_PTR_VALID_CHECK(p_nh_db);
    //CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IP_TUNNEL, p_para->hdr.nh_param_type);
    //CTC_EQUAL_CHECK(SYS_HUMBER_NH_TYPE_IP_TUNNEL, p_nh_db->hdr.nh_entry_type);
    p_nh_info = (sys_nh_info_ip_tunnel_t *)(p_nh_db);
    p_nh_para = (sys_nh_param_ip_tunnel_t*)(p_para);

    switch(p_nh_para->change_type)
    {
     case SYS_NH_CHANGE_TYPE_FWD_TO_UNROV:
            CTC_ERROR_RETURN(_sys_humber_nh_update_ip_tunnel_fwd_to_spec(p_nh_para, p_nh_info));
            break;

     case SYS_NH_CHANGE_TYPE_UPDATE_FWD_ATTR:
            CTC_ERROR_RETURN(_sys_humber_nh_update_ip_tunnel_fwd_attr(p_nh_para, p_nh_info));
            break;
     case SYS_NH_CHANGE_TYPE_UNROV_TO_FWD:
           if(!CTC_FLAG_ISSET(p_nh_info->hdr.nh_entry_flags, SYS_NH_INFO_IP_TUNNEL_FLAG_IS_UNROV))
                return CTC_E_NH_ISNT_UNROV;
            CTC_ERROR_RETURN(_sys_humber_nh_update_ip_tunnel_fwd_attr(p_nh_para, p_nh_info));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }


    return CTC_E_NONE;
}



int32
sys_humber_nh_debug_ip_tunnel()
{
    sys_ip_tunnel_ip_node_t *p_ip_node = NULL;
    ctc_slistnode_t *lisnode;

    char buf[CTC_IPV6_ADDR_STR_LEN];
    //#define SYS_IP_MASK_LEN 7
    //char buf2[SYS_IP_MASK_LEN];

    CTC_SLIST_LOOP(p_ip_tunnel_master->ip_list, lisnode)
    {
        p_ip_node = _ctc_container_of(lisnode, sys_ip_tunnel_ip_node_t, head);

        if (CTC_IP_VER_4 == p_ip_node->ip_ver)
        {
            uint32 tempip = kal_ntohl(p_ip_node->ip.ipv4);
            kal_inet_ntop(AF_INET, &tempip, buf, CTC_IPV6_ADDR_STR_LEN);
            //kal_strncat(buf, buf2, SYS_IP_MASK_LEN);
            SYS_NH_DBG_INFO("IPv4 Node: lchip = %d, index = %d, count = %d, ipv4_addr = %-23s\r\n",
                            p_ip_node->lchip, p_ip_node->index,  p_ip_node->count,  buf);
        }
        else
        {
            uint32 ipv6_address[4] = {0,0,0,0};
            ipv6_address[0] = kal_htonl(p_ip_node->ip.ipv6[3]);
            ipv6_address[1] = kal_htonl(p_ip_node->ip.ipv6[2]);
            ipv6_address[2] = kal_htonl(p_ip_node->ip.ipv6[1]);
            ipv6_address[3] = kal_htonl(p_ip_node->ip.ipv6[0]);
            kal_inet_ntop(AF_INET6, ipv6_address, buf, CTC_IPV6_ADDR_STR_LEN);
            SYS_NH_DBG_INFO("IPv6 Node: lchip = %d, index = %d, count = %d, ipv6_addr = %-44s\r\n",
                            p_ip_node->lchip, p_ip_node->index,  p_ip_node->count, buf);
        }
    }

    return CTC_E_NONE;
}


