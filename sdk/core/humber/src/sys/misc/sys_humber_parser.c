/**
 @file sys_humber_parser.c

 @date 2009-12-22

 @version v2.0

---file comments----
*/

#include "kal.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "sys_humber_parser.h"
#include "sys_humber_chip.h"
#include "sys_humber_parser_io.h"

/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/

#define SYS_PAS_L2_CAM_L2CAM_VALUE_IPV4 0x400800
#define SYS_PAS_L2_CAM_L2CAM_VALUE_IPV6 0x4086DD
#define SYS_PAS_L2_CAM_L2CAM_VALUE_MPLS 0x408847
#define SYS_PAS_L2_CAM_L2CAM_VALUE_MPLS_MCAST 0x408848
#define SYS_PAS_L2_CAM_L2CAM_VALUE_ARP 0x400806
#define SYS_PAS_L2_CAM_L2CAM_VALUE_RARP 0x408035
#define SYS_PAS_L2_CAM_L2CAM_VALUE_EAPOL 0x40888E
#define SYS_PAS_L2_CAM_L2CAM_VALUE_PPP2B 0x240021
#define SYS_PAS_L2_CAM_L2CAM_VALUE_PPP1B 0x258800
#define SYS_PAS_L2_CAM_L2CAM_VALUE_SNAP 0x430800
#define SYS_PAS_L2_CAM_L2CAM_VALUE_SAP 0x520800
#define SYS_PAS_L2_CAM_L2CAM_VALUE_ETHER_OAM 0x408902
#define SYS_PAS_L2_CAM_L2CAM_VALUE_SLOW_PROTO 0x408809
#define SYS_PAS_L2_CAM_L2CAM_VALUE_PBB 0x4088e7
#define SYS_PAS_L2_CAM_L2CAM_VALUE_PTP 0x4088f7


#define SYS_PAS_IPV6_EXT_LEVEL_HOP_BY_HOP 1
#define SYS_PAS_IPV6_EXT_LEVEL_DESTINATION 8
#define SYS_PAS_IPV6_EXT_LEVEL_ROUTING         3
#define SYS_PAS_IPV6_EXT_LEVEL_FRAG                4
#define SYS_PAS_IPV6_EXT_LEVEL_AH                    5
#define SYS_PAS_IPV6_EXT_LEVEL_ESP                  6

#define SYS_PAS_MAX_L4_PTL_CAM_ENTRY_NUM 2

#define SYS_PAS_MAX_IPV6_EXT_LEVEL 8
#define SYS_PAS_MAX_L3FLEX_BYTE_SEL 8


#define SYS_PAS_MAX_LAYER4_LEN_OP_ENTRY 15
#define SYS_PAS_MAX_UDP_APP_OP_ENTRY 4

#define SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_BASE CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_BASE
#define SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_MAX  CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_MAX

#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_BASE  8
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_8        8
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_9        9
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_10     10
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_11     11
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_12     12
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_13     13
#define SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_14     14

#define SYS_PAS_CAM_INDEX_CHECK(index,min,max) \
    {\
        if(index < min || index >= max )\
            return CTC_E_INVALID_PARAM;\
    }

/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/

/**
 @brief set l2 hash field
*/
extern int32
sys_humber_parser_set_l2_hash(ctc_parser_l2_ctl_fld_t *l2ctl)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    parser_ethernet_ctl_t eth_ctl;

    CTC_PTR_VALID_CHECK(l2ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&eth_ctl, 0, sizeof(parser_ethernet_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_ethernet_ctl_entry(lchip, &eth_ctl));

        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_VLAN_HASH)
        {
            eth_ctl.vlan_hash_en = l2ctl->vlan_hash?1:0;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_COS_HASH)
        {
            eth_ctl.cos_hash_en = l2ctl->cos_hash?1:0;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_L2_HDR_PRL_HASH)
        {
            eth_ctl.layer2_header_protocol_hash_en = l2ctl->l2hdr_ptl_hash?1:0;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_MOD_HASH)
        {
            eth_ctl.mod_hash_en = l2ctl->mod_hash?1:0;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_PORT_HASH)
        {
            eth_ctl.port_hash_en = l2ctl->port_hash?1:0;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_VLAN_HASH_MODE)

        {
            eth_ctl.vlan_hash_mode = (l2ctl->vlan_hash_mode ?1:0);
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_COS_ECMP_HASH)
        {
            eth_ctl.cos_ecmp_hash_en = l2ctl->cos_ecmp_hash?1:0;
        }
        if(CTC_FLAG_ISSET((l2ctl->flags), CTC_PARSER_L2_HASH_FIELD_ECMP_MACSA_HASH))
        {
            if(l2ctl->ecmp_macsa_hash)
            {
                eth_ctl.mac_hash_disable &= (~(1 << 0));
            }
            else
            {
                eth_ctl.mac_hash_disable |= (1 << 0);
            }
        }
         if(CTC_FLAG_ISSET((l2ctl->flags), CTC_PARSER_L2_HASH_FIELD_ECMP_MACDA_HASH))
        {
            if(l2ctl->ecmp_macda_hash)
            {
                eth_ctl.mac_hash_disable &= (~(1 << 1));

            }
            else
            {
                eth_ctl.mac_hash_disable  |= (1 << 1);

            }

        }
         if(CTC_FLAG_ISSET((l2ctl->flags), CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACSA_HASH))
        {
            if(l2ctl->linkagg_macsa_hash)
            {
                eth_ctl.mac_hash_disable &= (~(1 << 2));

            }
            else
            {
                eth_ctl.mac_hash_disable |= (1 << 2);

            }
        }
        if(CTC_FLAG_ISSET((l2ctl->flags), CTC_PARSER_L2_HASH_FIELD_LINKAGG_MACDA_HASH))
        {
            if(l2ctl->linkagg_macda_hash)
            {
                eth_ctl.mac_hash_disable &= (~(1 << 3));

            }
            else
            {
                eth_ctl.mac_hash_disable |= (1 << 3);

            }

        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_ISID_HASH)
        {
            eth_ctl.isid_hash_en = l2ctl->isid_hash;
        }
        if((l2ctl->flags) & CTC_PARSER_L2_HASH_FIELD_IPCP_HASH)
        {
            eth_ctl.i_pcp_hash_en = l2ctl->ipcp_hash;
        }

        CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_ethernet_ctl_entry(lchip, &eth_ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get hash computing field
*/
extern int32
sys_humber_parser_get_l2_hash(ctc_parser_l2_ctl_fld_t* l2ctl)
{
    uint8 lchip = 0;
    parser_ethernet_ctl_t eth_ctl;

    CTC_PTR_VALID_CHECK(l2ctl);

    kal_memset(&eth_ctl, 0, sizeof(parser_ethernet_ctl_t));
    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_ethernet_ctl_entry(lchip, &eth_ctl));

    l2ctl->vlan_hash = eth_ctl.vlan_hash_en;
    l2ctl->cos_hash = eth_ctl.cos_hash_en;
    l2ctl->l2hdr_ptl_hash = eth_ctl.layer2_header_protocol_hash_en;
    l2ctl->mod_hash = eth_ctl.mod_hash_en;

    l2ctl->port_hash = eth_ctl.port_hash_en;
    l2ctl->vlan_hash_mode = eth_ctl.vlan_hash_mode;
    l2ctl->cos_ecmp_hash = eth_ctl.cos_ecmp_hash_en;
    l2ctl->isid_hash = eth_ctl.isid_hash_en;
    l2ctl->ipcp_hash = eth_ctl.i_pcp_hash_en;

    l2ctl->ecmp_macsa_hash = !(eth_ctl.mac_hash_disable & 0x1);
    l2ctl->ecmp_macda_hash = !((eth_ctl.mac_hash_disable >> 1) & 0x1);
    l2ctl->linkagg_macsa_hash = !((eth_ctl.mac_hash_disable >> 2) & 0x1);
    l2ctl->linkagg_macda_hash = !((eth_ctl.mac_hash_disable >> 3) & 0x1);

    return CTC_E_NONE;
}

/**
 @brief set tpid
*/
extern int32
sys_humber_parser_set_tpid(ctc_parser_l2_tpid_t type, uint16 tpid)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;

    local_chip_num = sys_humber_get_local_chip_num();


    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        switch(type)
        {
            case CTC_PARSER_L2_TPID_CVLAN_TPID:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_cvlan_tpid(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_ITAG_TPID:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_i_tag_tpid(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_BLVAN_TPID:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_bvlan_tpid(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_SVLAN_TPID_0:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_svlan_tpid0(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_SVLAN_TPID_1:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_svlan_tpid1(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_SVLAN_TPID_2:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_svlan_tpid2(lchip, tpid));
                break;
            case CTC_PARSER_L2_TPID_SVLAN_TPID_3:
                CTC_ERROR_RETURN(sys_humber_parser_io_set_svlan_tpid3(lchip, tpid));
                break;
            default:
                return CTC_E_INVALID_PARAM;
                break;
        }
    }

    return CTC_E_NONE;
}

/**
 @brief get tpid with some type
*/
extern int32
sys_humber_parser_get_tpid(ctc_parser_l2_tpid_t type, uint16* tpid)
{
    uint8 lchip = 0;
    uint32 value = 0;

    CTC_PTR_VALID_CHECK(tpid);

    switch(type)
    {
        case CTC_PARSER_L2_TPID_CVLAN_TPID:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_cvlan_tpid(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_ITAG_TPID:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_i_tag_tpid(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_BLVAN_TPID:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_bvlan_tpid(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_SVLAN_TPID_0:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_svlan_tpid0(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_SVLAN_TPID_1:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_svlan_tpid1(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_SVLAN_TPID_2:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_svlan_tpid2(lchip, &value));
            break;
        case CTC_PARSER_L2_TPID_SVLAN_TPID_3:
            CTC_ERROR_RETURN(sys_humber_parser_io_get_svlan_tpid3(lchip, &value));
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;
    }

   *tpid = value;

    return CTC_E_NONE;
}

/**
 @brief set snap allow none zero oui
*/
extern int32
sys_humber_parser_set_snap_allow_none_zero_oui(uint8 enable)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    enable = enable? 1:0;
    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_parser_io_set_allow_non_zero_oui(lchip, enable));
    }

    return CTC_E_NONE;
}

/**
 @brief get snap allow none zero oui
*/
extern int32
sys_humber_parser_get_snap_allow_none_zero_oui(uint8* enable)
{
    uint8 lchip = 0;
    uint32 tmp = 0;
    CTC_ERROR_RETURN(sys_humber_parser_io_get_allow_non_zero_oui(lchip, &tmp));
    *enable = tmp;
    return CTC_E_NONE;
}

/**
 @brief set max_length,based on the value differentiate type or length
*/
extern int32
sys_humber_parser_set_max_length_filed(uint16 max_length)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_parser_io_set_max_length_field(lchip, max_length));
    }

    return CTC_E_NONE;
}

/**
 @brief get max_length value
*/
extern int32
sys_humber_parser_get_max_length_filed(uint16* max_length)
{
    uint8 lchip = 0;
    uint32 tmp = 0;
    CTC_ERROR_RETURN(sys_humber_parser_io_get_max_length_field(lchip, &tmp));
    *max_length = tmp;
    return CTC_E_NONE;
}

/**
 @brief set parser quad vlan enable
*/
extern int32
sys_humber_parser_set_parser_quad_vlan_en(uint8 enable)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;

    enable = enable?1:0;

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_parser_io_set_parsing_quad_vlan(lchip, enable));
    }

    return CTC_E_NONE;
}

/**
 @brief get parser quad vlan
*/
extern int32
sys_humber_parser_get_parser_quad_vlan_en(uint8* enable)
{
    uint8 lchip = 0;
    uint32 tmp = 0;
    CTC_ERROR_RETURN(sys_humber_parser_io_get_parsing_quad_vlan(lchip, &tmp));
    *enable = tmp;
    return CTC_E_NONE;
}

/**
 @brief set vlan parser num
*/
extern int32
sys_humber_parser_set_vlan_parser_num(uint8 vlan_num)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;

    if(vlan_num > 3)
    {
        return CTC_E_INVALID_PARAM;
    }

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(sys_humber_parser_io_set_vlan_parsing_num(lchip, vlan_num));
    }

    return CTC_E_NONE;
}

/**
 @brief get vlan parser num
*/
extern int32
sys_humber_parser_get_vlan_parser_num(uint8* vlan_num)
{
    uint8 lchip = 0;
    uint32 tmp = 0;
    CTC_ERROR_RETURN(sys_humber_parser_io_get_vlan_parsing_num(lchip, &tmp));
    *vlan_num = tmp;
    return CTC_E_NONE;
}

/**
 @brief set pbb parser ctl info
*/
extern int32
sys_humber_parser_set_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    parser_pbb_ctl_t pbb_ctl;
    CTC_PTR_VALID_CHECK(pbb_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&pbb_ctl, 0, sizeof(parser_pbb_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_pbb_ctl_entry(lchip, &pbb_ctl));

        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_COS_ECMP_HASH)
        {
            pbb_ctl.pbb_cos_ecmp_hash_en = pbb_parser_ctl->cos_ecmp_hash?1:0;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_VLAN_HASH)
        {
            pbb_ctl.pbb_vlan_hash_en = pbb_parser_ctl->vlan_hash?1:0;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_COS_HASH)
        {
            pbb_ctl.pbb_cos_hash_en = pbb_parser_ctl->cos_hash?1:0;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_CMAC_HASH_ECMP_MACSA)
        {
            if(pbb_parser_ctl->ecmp_macsa_hash)
            {
                pbb_ctl.c_mac_hash_disable &= (~(1 << 0));
            }
            else
            {
                pbb_ctl.c_mac_hash_disable |= (1 << 0);
            }
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_CMAC_HASH_ECMP_MACDA)
        {
            if(pbb_parser_ctl->ecmp_macda_hash)
            {
                pbb_ctl.c_mac_hash_disable &= (~(1 << 1));
            }
            else
            {
                pbb_ctl.c_mac_hash_disable |= (1 << 1);
            }
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_CMAC_HASH_LINKAGG_MACSA)
        {
            if(pbb_parser_ctl->linkagg_macsa_hash)
            {
                pbb_ctl.c_mac_hash_disable  &= (~(1 << 2));
            }
            else
            {
                pbb_ctl.c_mac_hash_disable |= (1 << 2);
            }
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_CMAC_HASH_LINKAGG_MACDA)
        {
            if(pbb_parser_ctl->linkagg_macsa_hash)
            {
                pbb_ctl.c_mac_hash_disable  &= (~(1 << 3));
            }
            else
            {
                pbb_ctl.c_mac_hash_disable |= (1 << 3);
            }
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_NCA_VAL)
        {
            pbb_ctl.nca_value = pbb_parser_ctl->nca_value_en?1:0;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_OUTER_VLAN_IS_CVLAN)
        {
            pbb_ctl.c_mac_outer_vlan_is_cvlan = pbb_parser_ctl->outer_vlan_is_cvlan?1:0;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_VLAN_PAS_NUM)
        {
            if(pbb_parser_ctl->vlan_parsing_num > 3)
            {
                return CTC_E_INVALID_PARAM;
            }
            pbb_ctl.pbb_vlan_parsing_num = pbb_parser_ctl->vlan_parsing_num;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_PBB_OAM_ETHER_TYPE_OFFSET)
        {
            if(pbb_parser_ctl->pbb_oam_ether_type_offset > 0x3f)
            {
                return CTC_E_INVALID_PARAM;
            }
            pbb_ctl.pbb_oam_ether_type_offset = pbb_parser_ctl->pbb_oam_ether_type_offset;
        }
        if(pbb_parser_ctl->flags & CTC_PARSER_PBB_FLAGS_PBB_OAM_ETHER_TYPE)
        {
            pbb_ctl.pbb_oam_ether_type = pbb_parser_ctl->pbb_oam_ether_type;
        }
        CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_pbb_ctl_entry(lchip, &pbb_ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get pbb parser ctl info
*/
extern int32
sys_humber_parser_get_pbb_parser_ctl(ctc_parser_pbb_ctl_t* pbb_parser_ctl)
{
    uint8 lchip = 0;
    parser_pbb_ctl_t pbb_ctl;
    CTC_PTR_VALID_CHECK(pbb_parser_ctl);


    kal_memset(&pbb_ctl, 0, sizeof(parser_pbb_ctl_t));
    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_pbb_ctl_entry(lchip, &pbb_ctl));

    pbb_parser_ctl->cos_ecmp_hash = pbb_ctl.pbb_cos_ecmp_hash_en;
    pbb_parser_ctl->cos_hash = pbb_ctl.pbb_cos_hash_en;
    pbb_parser_ctl->vlan_hash = pbb_ctl.pbb_vlan_hash_en;
    pbb_parser_ctl->nca_value_en = pbb_ctl.nca_value;

    pbb_parser_ctl->ecmp_macsa_hash = !pbb_ctl.c_mac_hash_disable & 0x1;
    pbb_parser_ctl->ecmp_macda_hash = !(pbb_ctl.c_mac_hash_disable >> 1) & 0x1;
    pbb_parser_ctl->linkagg_macsa_hash = !(pbb_ctl.c_mac_hash_disable >> 2) & 0x1;
    pbb_parser_ctl->linkagg_macda_hash = !(pbb_ctl.c_mac_hash_disable >> 3) & 0x1;

    pbb_parser_ctl->outer_vlan_is_cvlan = pbb_ctl.c_mac_outer_vlan_is_cvlan;
    pbb_parser_ctl->pbb_oam_ether_type = pbb_ctl.pbb_oam_ether_type;
    pbb_parser_ctl->pbb_oam_ether_type_offset = pbb_ctl.pbb_oam_ether_type_offset;
    pbb_parser_ctl->vlan_parsing_num = pbb_ctl.pbb_vlan_parsing_num;


    return CTC_E_NONE;
}

/**
 @brief set l2 flex ctl info
*/
extern int32
sys_humber_parser_set_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    uint8 local_chip_num = 0;
    uint8 lchip = 0;
    parser_layer2_flex_ctl_t flex;
    CTC_PTR_VALID_CHECK(l2flex_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&flex, 0, sizeof(parser_layer2_flex_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer2_flex_ctl_entry(lchip, &flex));
        if(l2flex_ctl->byte_sel > 0x1f)
        {
            return CTC_E_INVALID_PARAM;
        }
        if(l2flex_ctl->ptl_byte_sel > 0x1f)
        {
            return CTC_E_INVALID_PARAM;
        }
        switch(index)
        {
            case 0:
                if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL0)
                {
                    flex.layer2_byte_select0 = l2flex_ctl->byte_sel;
                }
                if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_PTL_BYTE_SEL0)
                {

                    flex.layer2_protocol_byte_select0 = l2flex_ctl->ptl_byte_sel;
                }
                break;
            case 1:
                if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL1)
                {
                    flex.layer2_byte_select1 = l2flex_ctl->byte_sel;
                }
                 if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_PTL_BYTE_SEL1)
                {
                    flex.layer2_protocol_byte_select1 = l2flex_ctl->ptl_byte_sel;
                }
                 break;
             case 2:
                if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL2)
                {
                    flex.layer2_byte_select2 = l2flex_ctl->byte_sel;
                }
                break;
            case 3:
                if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL3)
                {
                    flex.layer2_byte_select3 = l2flex_ctl->byte_sel;
                }
                break;
            case 4:
                 if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL4)
                {
                    flex.layer2_byte_select4 = l2flex_ctl->byte_sel;
                }
                 break;
             case 5:
                 if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_BYTE_SEL5)
                {
                    flex.layer2_byte_select5 = l2flex_ctl->byte_sel;
                }
                 break;
             default:
                return CTC_E_INVALID_PARAM;
                break;
        }

        if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_MIN_LEN)
        {
            flex.layer2_min_length = l2flex_ctl->min_length;
        }
        if(l2flex_ctl->flags & CTC_PARSER_L2FLEX_FLAGS_L2_BASIC_OFFSET)
        {
            flex.layer2_basic_offset = l2flex_ctl->l2_basic_offset;
        }

        CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_layer2_flex_ctl_entry(lchip, &flex));
    }

    return CTC_E_NONE;
}

/**
 @brief get l2 flex ctl info
*/
extern int32
sys_humber_parser_get_l2flex_ctl(uint8 index, ctc_parser_l2flex_ctl_t* l2flex_ctl)
{
    uint8 lchip = 0;
    parser_layer2_flex_ctl_t flex;
    CTC_PTR_VALID_CHECK(l2flex_ctl);

    kal_memset(&flex, 0, sizeof(parser_layer2_flex_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer2_flex_ctl_entry(lchip, &flex));
    switch(index)
    {
        case 0:
            l2flex_ctl->byte_sel = flex.layer2_byte_select0;
            l2flex_ctl->ptl_byte_sel = flex.layer2_protocol_byte_select0;
            break;
        case 1:
            l2flex_ctl->byte_sel = flex.layer2_byte_select1;
            l2flex_ctl->ptl_byte_sel = flex.layer2_protocol_byte_select1;
            break;
        case 2:
            l2flex_ctl->byte_sel = flex.layer2_byte_select2;
            break;
        case 3:
            l2flex_ctl->byte_sel = flex.layer2_byte_select3;
            break;
        case 4:
            l2flex_ctl->byte_sel = flex.layer2_byte_select4;
            break;
        case 5:
            l2flex_ctl->byte_sel = flex.layer2_byte_select5;
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;

    }

    l2flex_ctl->l2_basic_offset = flex.layer2_basic_offset;
    l2flex_ctl->min_length = flex.layer2_min_length;

    return CTC_E_NONE;
}

/**
 @brief add l2type
*/
extern int32
sys_humber_parser_add_l2_type(uint8 index, ctc_parser_l2_ptl_entry_t* entry)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer2_protocol_cam_t cam;
    parser_layer2_protocol_cam_valid_t cam_valid;
    /*???should use a variable to count already configed entry*/
    CTC_PTR_VALID_CHECK(entry);


   if (    entry->l3_type < CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_BASE
	|| entry->l3_type > CTC_PARSER_L3_TYPE_RSV_USER_DEFINE_MAX)
   {
      return CTC_E_INVALID_PARAM ;
   }
    if((entry->addition_offset > 0xf)
        ||(entry->l2_type > 0xf))
    {
        return CTC_E_INVALID_PARAM;
    }

    local_chip_num = sys_humber_get_local_chip_num();

    /*only write the user defined entry*/
    if(index > SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_MAX - SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_BASE)
    {
  	    return CTC_E_INVALID_PARAM;
    }
    index = index + SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_BASE;

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&cam, 0, sizeof(parser_layer2_protocol_cam_t));
        kal_memset(&cam_valid, 0, sizeof(parser_layer2_protocol_cam_valid_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer2_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                            sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));

        entry->isEth = entry->isEth?1:0;
        entry->isPPP = entry->isPPP?1:0;
        entry->isSAP = entry->isSAP?1:0;

        switch(index)
        {
            case 3:
            {
                cam.layer2_protocol_cam_additional_offset3 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type3 = entry->l3_type;
                cam.layer2_protocol_cam_mask3 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value3 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 4:
            {
                cam.layer2_protocol_cam_additional_offset4 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type4 = entry->l3_type;
                cam.layer2_protocol_cam_mask4 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value4 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 5:
            {
                cam.layer2_protocol_cam_additional_offset5 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type5 = entry->l3_type;
                cam.layer2_protocol_cam_mask5 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value5 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 6:
            {
                cam.layer2_protocol_cam_additional_offset6 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type6 = entry->l3_type;
                cam.layer2_protocol_cam_mask6 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value6 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 7:
            {
                cam.layer2_protocol_cam_additional_offset7 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type7 = entry->l3_type;
                cam.layer2_protocol_cam_mask7 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value7 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 8:
            {
                cam.layer2_protocol_cam_additional_offset8 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type8 = entry->l3_type;
                cam.layer2_protocol_cam_mask8 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value8 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 9:
            {
                cam.layer2_protocol_cam_additional_offset9 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type9 = entry->l3_type;
                cam.layer2_protocol_cam_mask9 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value9 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 10:
            {
                cam.layer2_protocol_cam_additional_offset10 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type10 = entry->l3_type;
                cam.layer2_protocol_cam_mask10 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value10 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 11:
            {
                cam.layer2_protocol_cam_additional_offset11 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type11 = entry->l3_type;
                cam.layer2_protocol_cam_mask11 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value11 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 12:
            {
                cam.layer2_protocol_cam_additional_offset12 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type12 = entry->l3_type;
                cam.layer2_protocol_cam_mask12 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value12 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 13:
            {
                cam.layer2_protocol_cam_additional_offset13 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type13 = entry->l3_type;
                cam.layer2_protocol_cam_mask13 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value13 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 14:
            {
                cam.layer2_protocol_cam_additional_offset14 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type14 = entry->l3_type;
                cam.layer2_protocol_cam_mask14 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value14 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            case 15:
            {
                cam.layer2_protocol_cam_additional_offset15 = entry->addition_offset;
                cam.layer2_protocol_cam_layer3_type15 = entry->l3_type;
                cam.layer2_protocol_cam_mask15 = entry->mask & 0x7fffff;
                cam.layer2_protocol_cam_value15 = (entry->isEth <<22) | (entry->isPPP << 21)
                                                                        | (entry->isSAP << 20)
                                                                        | (entry->l2_type << 16)
                                                                        | (entry->l2hdr_ptl);
                break;
            }
            default:
            {
                return CTC_E_INVALID_PARAM;
                break;
            }
        }
        cam_valid.layer2_cam_entry_valid |= (1<< index);

        CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_layer2_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                    sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));
    }

    return CTC_E_NONE;
}

/**
 @brief set the entry invalid based on the index
*/
extern int32
sys_humber_parser_remove_l2_type(uint8 index)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer2_protocol_cam_valid_t cam_valid;

    /*9: customer can config 10 entry at most, 6 entry have been config in system initialization.
    MAX_CTC_PARSER_L2_TYPE - CTC_PARSER_L2_TYPE_RAW_SNAP*/

    local_chip_num = sys_humber_get_local_chip_num();
    /*6: already configed entry in system initialization*/
    index = index + SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_BASE;

    if(index > SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_MAX)
    {
  	    return CTC_E_INVALID_PARAM;
    }

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&cam_valid, 0, sizeof(parser_layer2_protocol_cam_valid_t));
        CTC_ERROR_RETURN(
                                sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));

        if(CTC_IS_BIT_SET(cam_valid.layer2_cam_entry_valid, index))
        {
            cam_valid.layer2_cam_entry_valid &= (~(1 << index));
            CTC_ERROR_RETURN(
                        sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_parser_get_l2_type(ctc_parser_l2_ptl_entry_t *p_entry, ctc_parser_l3_type_t *p_l3_type)
{
    uint8 index;
    uint32 field_id;
    uint32 cmd;
    uint32 value;
    uint32 mask;
    uint32 l3_type;
    uint32 compare_value;
    parser_layer2_protocol_cam_valid_t cam_valid;

    CTC_ERROR_RETURN(sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(0, &cam_valid));

    compare_value = (p_entry->isEth <<22) | (p_entry->isPPP << 21) | (p_entry->isSAP << 20)
                     | (p_entry->l2_type << 16) | (p_entry->l2hdr_ptl);

    for (index = 0; index <= SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_MAX; index++)
    {
        if (!CTC_IS_BIT_SET(cam_valid.layer2_cam_entry_valid, index))
        {
            continue;
        }

        field_id = PARSER_LAYER2_PROTOCOL_CAM_LAYER2_PROTOCOL_CAM_VALUE0 + index /*index*3*/;
        cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM, field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &value));

        field_id = PARSER_LAYER2_PROTOCOL_CAM_LAYER2_PROTOCOL_CAM_MASK0 + index;
        cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM, field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &mask));

        field_id = PARSER_LAYER2_PROTOCOL_CAM_LAYER2_PROTOCOL_CAM_LAYER3_TYPE0 + index /*index *3*/;
        cmd = DRV_IOR(IOC_REG, PARSER_LAYER2_PROTOCOL_CAM, field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &l3_type));

        if ((value & mask) == (compare_value & mask))
        {
            *p_l3_type = l3_type;
            return CTC_E_NONE;
        }
    }

    *p_l3_type = MAX_CTC_PARSER_L3_TYPE;

    return CTC_E_ENTRY_NOT_EXIST;
}


int32
sys_humber_parser_l2_enable_l3_type(ctc_parser_l3_type_t l3_type, bool enable)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    parser_layer2_protocol_cam_valid_t cam_valid;

    lchip_num = sys_humber_get_local_chip_num();

    CTC_MAX_VALUE_CHECK(l3_type, CTC_PARSER_L3_TYPE_RSV_USER_FLEXL3);

    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        kal_memset(&cam_valid, 0, sizeof(parser_layer2_protocol_cam_valid_t));
        CTC_ERROR_RETURN(sys_humber_parser_io_get_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));

        if(FALSE == enable)  /*disable*/
        {
            if(CTC_IS_BIT_SET(cam_valid.layer2_cam_entry_valid, l3_type))
            {
                cam_valid.layer2_cam_entry_valid &= (~(1 << l3_type));
            }
            else
            {
                return CTC_E_NONE;
            }
        }
        else  /*enable*/
        {
            if(CTC_IS_BIT_SET(cam_valid.layer2_cam_entry_valid, l3_type))
            {
                return CTC_E_NONE;
            }
            else
            {
                cam_valid.layer2_cam_entry_valid |= (1 << l3_type);
            }
        }

        CTC_ERROR_RETURN(sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(lchip, &cam_valid));
    }
    return CTC_E_NONE;
}

/**
 @brief set ip hash ctl reg
*/
extern int32
sys_humber_parser_set_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{

    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_ip_hash_ctl_t ctl;
    CTC_PTR_VALID_CHECK(hash_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_ip_hash_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_ip_hash_ctl_entry(lchip, &ctl));

        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_USE_IP_HASH)
        {
            ctl.use_ip_hash = hash_ctl->use_ip_hash?1:0;
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_PROTOCOL_ECMP_HASH)
        {
            ctl.protocol_ecmp_hash_en = hash_ctl->ptl_ecmp_hash?1:0;
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_DSCP_ECMP_HASH)
        {
            ctl.dscp_ecmp_hash_en = hash_ctl->dscp_ecmp_hash?1:0;
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_PROTOCOL_HASH)
        {
            ctl.protocol_hash_en = hash_ctl->ptl_hash?1:0;
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_FLOW_LABEL_ECMP_HASH)
        {
            ctl.flow_label_ecmp_hash_en = hash_ctl->flow_label_ecmp_hash?1:0;
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_ECMP_IPSA_HASH)
        {
            if(hash_ctl->ecmp_ipsa_hash)
            {
                ctl.ip_hash_disable &= (~(1 << 0));
            }
            else
            {
                ctl.ip_hash_disable |= (1 << 0);
            }
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_ECMP_IPDA_HASH)
        {
            if(hash_ctl->ecmp_ipda_hash)
            {
                ctl.ip_hash_disable &= (~(1 << 1));
            }
            else
            {
                ctl.ip_hash_disable  |= (1 << 1);
            }

        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPSA_HASH)
        {
            if(hash_ctl->linkagg_ipsa_hash)
            {
                ctl.ip_hash_disable &= (~(1 << 2));
            }
            else
            {
                ctl.ip_hash_disable  |= (1 << 2);
            }
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_LINKAGG_IPDA_HASH)
        {
            if(hash_ctl->linkagg_ipda_hash)
            {
                ctl.ip_hash_disable&= (~(1 << 3));
            }
            else
            {
                ctl.ip_hash_disable  |= (1 << 3);;
            }
        }
        if(hash_ctl->flags & CTC_PARSER_IP_HASH_FLAGS_SMALL_FRAG_OFFSET)
        {
            if(hash_ctl->small_frag_offset > 3)
            {
                return CTC_E_INVALID_PARAM;
            }
            ctl.small_fragment_offset = hash_ctl->small_frag_offset;
        }

        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_ip_hash_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get ip hash ctl reg info
*/
extern int32
sys_humber_parser_get_ip_hash_ctl(ctc_parser_ip_hash_ctl_t* hash_ctl)
{

    uint8 lchip = 0;
    parser_ip_hash_ctl_t ctl;
    CTC_PTR_VALID_CHECK(hash_ctl);

    kal_memset(&ctl, 0, sizeof(parser_ip_hash_ctl_t));
    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_ip_hash_ctl_entry(lchip, &ctl));

    hash_ctl->ecmp_ipda_hash = !((ctl.ip_hash_disable >> 1) & 0x1);
    hash_ctl->ecmp_ipsa_hash = !((ctl.ip_hash_disable) & 0x1);
    hash_ctl->linkagg_ipda_hash =!((ctl.ip_hash_disable >> 3) & 0x1);
    hash_ctl->linkagg_ipsa_hash = !((ctl.ip_hash_disable >> 2) & 0x1);

    hash_ctl->flow_label_ecmp_hash = ctl.flow_label_ecmp_hash_en;
    hash_ctl->dscp_ecmp_hash = ctl.dscp_ecmp_hash_en;
    hash_ctl->ptl_ecmp_hash = ctl.protocol_ecmp_hash_en;
    hash_ctl->ptl_hash = ctl.protocol_hash_en;

    hash_ctl->small_frag_offset = ctl.small_fragment_offset;
    hash_ctl->use_ip_hash = ctl.use_ip_hash;

    return CTC_E_NONE;
}

/**
 @brief set parser ipv6 ctl reg
*/
extern int32
sys_humber_parser_set_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_ipv6_ctl_t ctl;

    CTC_PTR_VALID_CHECK(ipv6_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_ipv6_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_ipv6_ctl_entry(lchip, &ctl));

        if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_FLEX_EXT_HDR0)
        {
            ctl.ipv6_flex_ext_header0 = ipv6_ctl->flex_ext_hdr0;
        }
        if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_FLEX_EXT_HDR1)
        {
            ctl.ipv6_flex_ext_header1 = ipv6_ctl->flex_ext_hdr1;
        }
        if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_ERR_OP_EN)
        {
            ctl.parser_ipv6_error_option_en = ipv6_ctl->ipv6_err_option_en?1:0;
        }
        if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_DIS)
        {
            ctl.ipv6_ext_disable = ipv6_ctl->ipv6_ext_dis?1:0;
        }
        if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_RSV_L3_HDR_PTL)
        {
            ctl.reserved_l3_header_protocol = ipv6_ctl->rsv_l3_hdr_ptl;
        }

        if(ipv6_ctl->ext_hdr_level.ext_level > 0xf)
        {
            return CTC_E_INVALID_PARAM;
        }
        switch(index)
        {
            case 0:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL0)
                {
                    ctl.ipv6_ext_level0 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en0 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift0 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options0 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 1:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL1)
                {
                    ctl.ipv6_ext_level1 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en1 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift1 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options1 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 2:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL2)
                {
                    ctl.ipv6_ext_level2 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en2 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift2 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options2 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 3:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL3)
                {
                    ctl.ipv6_ext_level3 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en3 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift3 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options3 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 4:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL4)
                {
                    ctl.ipv6_ext_level4 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en4 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift4 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options4 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 5:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL5)
                {
                    ctl.ipv6_ext_level5 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en5 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift5 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options5 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 6:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL6)
                {
                    ctl.ipv6_ext_level6 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en6 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift6 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options6 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            case 7:
                if(ipv6_ctl->flags & CTC_PARSER_IPV6_CTL_FLAGS_EXT_HDR_LEVEL7)
                {
                    ctl.ipv6_ext_level7 = ipv6_ctl->ext_hdr_level.ext_level;
                    ctl.ipv6_ext_level_check_en7 = ipv6_ctl->ext_hdr_level.ext_level_chk_en?1:0;
                    ctl.ipv6_ext_shift7 = ipv6_ctl->ext_hdr_level.ext_shift?1:0;
                    ctl.ipv6_set_ip_options7 = ipv6_ctl->ext_hdr_level.set_ip_options?1:0;
                }
                break;
            default:
                return CTC_E_INVALID_PARAM;
                break;
        }

        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_ipv6_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get parser ipv6 ctl reg
*/
extern int32
sys_humber_parser_get_ipv6_ctl(uint8 index, ctc_parser_ipv6_ctl_t* ipv6_ctl)
{
    parser_ipv6_ctl_t ctl;

    CTC_PTR_VALID_CHECK(ipv6_ctl);

    kal_memset(&ctl, 0, sizeof(parser_ipv6_ctl_t));
    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_ipv6_ctl_entry(0, &ctl));
    ipv6_ctl->flex_ext_hdr0 = ctl.ipv6_flex_ext_header0;
    ipv6_ctl->flex_ext_hdr1 = ctl.ipv6_flex_ext_header1;
    ipv6_ctl->ipv6_err_option_en = ctl.parser_ipv6_error_option_en;
    ipv6_ctl->ipv6_ext_dis = ctl.ipv6_ext_disable;
    ipv6_ctl->rsv_l3_hdr_ptl = ctl.reserved_l3_header_protocol;

    switch(index)
    {
        case 0:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level0;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en0;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift0;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options0;
            break;
        case 1:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level1;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en1;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift1;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options1;
            break;
        case 2:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level2;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en2;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift2;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options2;
            break;
        case 3:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level3;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en3;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift3;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options3;
            break;
        case 4:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level4;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en4;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift4;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options4;
            break;
        case 5:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level5;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en5;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift5;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options5;
            break;
        case 6:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level6;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en6;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift6;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options6;
            break;
        case 7:
            ipv6_ctl->ext_hdr_level.ext_level = ctl.ipv6_ext_level7;
            ipv6_ctl->ext_hdr_level.ext_level_chk_en = ctl.ipv6_ext_level_check_en7;
            ipv6_ctl->ext_hdr_level.ext_shift = ctl.ipv6_ext_shift7;
            ipv6_ctl->ext_hdr_level.set_ip_options = ctl.ipv6_set_ip_options7;
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;
    }

    return CTC_E_NONE;
}

/**
 @brief set parser mpls ctl reg
*/
extern int32
sys_humber_parser_set_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_mpls_ctl_t ctl;
    CTC_PTR_VALID_CHECK(mpls_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_mpls_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_mpls_ctl_entry(lchip, &ctl));

        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_USE_MPLS_HASH)
        {
            ctl.use_mpls_hash = mpls_ctl->use_mpls_hash?1:0;
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_PTL_ECMP_HASH)
        {
            ctl.mpls_protocol_ecmp_hash_en = mpls_ctl->ptl_ecmp_hash?1:0;
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_DSCP_ECMP_HASH)
        {
            ctl.mpls_dscp_ecmp_hash_en = mpls_ctl->dscp_ecmp_hash?1:0;
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_PTL_HASH)
        {
            ctl.mpls_protocol_hash_en = mpls_ctl->ptl_hash?1:0;
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_FLOW_LABEL_ECMP_HASH)
        {
            ctl.mpls_flow_label_ecmp_hash_en = mpls_ctl->flow_label_ecmp_hash?1:0;
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_ECMP_IPSA_HASH)
        {
            if(mpls_ctl->ecmp_ipsa_hash)
            {
                ctl.mpls_ip_hash_disable &= (~(1 << 0));
            }
            else
            {
                ctl.mpls_ip_hash_disable |= (1 << 0);
            }
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_ECMP_IPDA_HASH)
        {
            if(mpls_ctl->ecmp_ipda_hash)
            {
                ctl.mpls_ip_hash_disable &= (~(1 << 1));
            }
            else
            {
                ctl.mpls_ip_hash_disable |= (1 << 1);
            }
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_LINKAGG_IPSA_HASH)
        {
            if(mpls_ctl->linkagg_ipsa_hash)
            {
                ctl.mpls_ip_hash_disable &= (~(1 << 2));
            }
            else
            {
                ctl.mpls_ip_hash_disable |= (1 << 2);
            }
        }
        if(mpls_ctl->flags & CTC_PARSER_MPLS_CTL_LINKAGG_IPDA_HASH)
        {
            if(mpls_ctl->linkagg_ipda_hash)
            {
                ctl.mpls_ip_hash_disable &= (~(1 << 3));
            }
            else
            {
                ctl.mpls_ip_hash_disable |= (1 << 3);
            }
        }

        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_mpls_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get parser mpls ctl reg
*/
extern int32
sys_humber_parser_get_mpls_ctl(ctc_parser_mpls_ctl_fld_t* mpls_ctl)
{
    parser_mpls_ctl_t ctl;
    kal_memset(&ctl, 0, sizeof(parser_mpls_ctl_t));

    CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_mpls_ctl_entry(0, &ctl));

    mpls_ctl->dscp_ecmp_hash = ctl.mpls_dscp_ecmp_hash_en;
    mpls_ctl->ecmp_ipda_hash = !((ctl.mpls_ip_hash_disable >> 1) & 0x1);
    mpls_ctl->ecmp_ipsa_hash = !((ctl.mpls_ip_hash_disable) & 0x1);
    mpls_ctl->linkagg_ipda_hash = !((ctl.mpls_ip_hash_disable >> 3) & 0x1);
    mpls_ctl->linkagg_ipsa_hash = !((ctl.mpls_ip_hash_disable >> 2) & 0x1);

    mpls_ctl->ptl_ecmp_hash = ctl.mpls_protocol_ecmp_hash_en;
    mpls_ctl->ptl_hash = ctl.mpls_protocol_hash_en;
    mpls_ctl->use_mpls_hash = ctl.use_mpls_hash;
    mpls_ctl->flow_label_ecmp_hash = ctl.mpls_flow_label_ecmp_hash_en;

    return CTC_E_NONE;
}

/**
 @brief set parser l3flex ctl reg
*/
extern int32
sys_humber_parser_set_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer3_flex_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l3flex_ctl);

    CTC_MAX_VALUE_CHECK(l3flex_ctl->byte_sel, 31);
    CTC_MAX_VALUE_CHECK(l3flex_ctl->ptl_byte_sel, 31);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_layer3_flex_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer3_flex_ctl_entry(lchip, &ctl));
        if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_L3MIN_LEN)
        {
            ctl.layer3_min_length = l3flex_ctl->l3min_length;
        }
        if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_L3_BASIC_OFFSET)
        {
	     ctl.layer3_basic_offset = l3flex_ctl->l3_basic_offset;
        }
        if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_PTL_BYTE_SEL)
        {
            if(l3flex_ctl->ptl_byte_sel > 0x1f)
            {
                return CTC_E_INVALID_PARAM;
            }
            ctl.layer3_protocol_byte_select = l3flex_ctl->ptl_byte_sel;
        }

        if(l3flex_ctl->byte_sel > 0x1f)
        {
            return CTC_E_INVALID_PARAM;
        }
        switch(index)
        {
            case 0:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL0)
                {
                    ctl.layer3_byte_select0 = l3flex_ctl->byte_sel;
                }
                 break;
             case 1:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL1)
                {
                    ctl.layer3_byte_select1 = l3flex_ctl->byte_sel;
                }
                 break;
             case 2:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL2)
                {
                    ctl.layer3_byte_select2 = l3flex_ctl->byte_sel;
                }
                 break;
             case 3:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL3)
                {
                    ctl.layer3_byte_select3 = l3flex_ctl->byte_sel;
                }
                 break;
             case 4:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL4)
                {
                    ctl.layer3_byte_select4 = l3flex_ctl->byte_sel;
                }
                 break;
             case 5:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL5)
                {
                    ctl.layer3_byte_select5 = l3flex_ctl->byte_sel;
                }
                 break;
             case 6:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL6)
                {
                    ctl.layer3_byte_select6 = l3flex_ctl->byte_sel;
                }
                 break;
             case 7:
                 if(l3flex_ctl->flags & CTC_PARSER_L3FLEX_FLAGS_BYTE_SEL7)
                {
                    ctl.layer3_byte_select7 = l3flex_ctl->byte_sel;
                }
                 break;
             default:
                return CTC_E_INVALID_PARAM;
                break;
        }

       CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer3_flex_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get parser l3flex ctl reg
*/
extern int32
sys_humber_parser_get_l3flex_ctl(uint8 index, ctc_parser_l3flex_ctl_t* l3flex_ctl)
{
    parser_layer3_flex_ctl_t ctl;
    CTC_PTR_VALID_CHECK(l3flex_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer3_flex_ctl_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer3_flex_ctl_entry(0, &ctl));
        l3flex_ctl->l3min_length = ctl.layer3_min_length;
        l3flex_ctl->l3_basic_offset = ctl.layer3_basic_offset;
        l3flex_ctl->ptl_byte_sel = ctl.layer3_protocol_byte_select;

        switch(index)
        {
            case 0:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select0;
                break;
            case 1:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select1;
                break;
            case 2:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select2;
                break;
            case 3:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select3;
                break;
            case 4:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select4;
                break;
            case 5:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select5;
                break;
            case 6:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select6;
                break;
            case 7:
                l3flex_ctl->byte_sel = ctl.layer3_byte_select7;
                break;
            default:
                return CTC_E_INVALID_PARAM;
                break;
        }


    return CTC_E_NONE;
}

/**
 @brief add l3type,can add a new l3type,addition offset for the type,can get the layer4 type
*/
extern int32
sys_humber_parser_add_l3_type(uint8 index, ctc_parser_l3_ptl_entry_t* entry)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer3_protocol_cam_t cam;
    parser_layer3_protocol_cam_valid_t cam_valid;

    CTC_PTR_VALID_CHECK(entry);

    if((entry->addition_offset > 0xf)
        ||(entry->l3_type > 0xf)
        ||(entry->l4_type > 0xf))
    {
        return CTC_E_INVALID_PARAM;
    }
    local_chip_num = sys_humber_get_local_chip_num();

    /*only write user defined entry*/
    index = index + SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_BASE;

   if(index > SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_14)
   {
      return CTC_E_INVALID_PARAM;
   }

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&cam, 0, sizeof(parser_layer3_protocol_cam_t));
        kal_memset(&cam_valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                            sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));

        switch(index)
        {
            case SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_8:
                cam.layer3_protocol_cam_additional_offset8 = entry->addition_offset;
                cam.layer3_protocol_cam_layer3_header_protocol8 = entry->l3_hdr_ptl;
                cam.layer3_protocol_cam_layer3_header_protocol_mask8 = entry->l3_hdr_ptl_mask & 0xff;
                cam.layer3_protocol_cam_layer3_type8 = entry->l3_type;
                cam.layer3_protocol_cam_layer3_type_mask8 = entry->l3_type_mask & 0xf;
                cam.layer3_protocol_cam_layer4_type8 = entry->l4_type;
                break;
            case SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_9:
                cam.layer3_protocol_cam_additional_offset9 = entry->addition_offset;
                cam.layer3_protocol_cam_layer3_header_protocol9 = entry->l3_hdr_ptl;
                cam.layer3_protocol_cam_layer3_header_protocol_mask9 = entry->l3_hdr_ptl_mask & 0xff;
                cam.layer3_protocol_cam_layer3_type9 = entry->l3_type;
                cam.layer3_protocol_cam_layer3_type_mask9 = entry->l3_type_mask & 0xf;
                cam.layer3_protocol_cam_layer4_type9 = entry->l4_type;
                break;
         case SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_10:
                cam.layer3_protocol_cam_additional_offset10 = entry->addition_offset;
                cam.layer3_protocol_cam_layer3_header_protocol10 = entry->l3_hdr_ptl;
                cam.layer3_protocol_cam_layer3_header_protocol_mask10 = entry->l3_hdr_ptl_mask & 0xff;
                cam.layer3_protocol_cam_layer3_type10 = entry->l3_type;
                cam.layer3_protocol_cam_layer3_type_mask10 = entry->l3_type_mask & 0xf;
                cam.layer3_protocol_cam_layer4_type10 = entry->l4_type;
                break;
            case SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_11:
                cam.layer3_protocol_cam_additional_offset11 = entry->addition_offset;
                cam.layer3_protocol_cam_layer3_header_protocol11 = entry->l3_hdr_ptl;
                cam.layer3_protocol_cam_layer3_header_protocol_mask11 = entry->l3_hdr_ptl_mask & 0xff;
                cam.layer3_protocol_cam_layer3_type11 = entry->l3_type;
                cam.layer3_protocol_cam_layer3_type_mask11 = entry->l3_type_mask & 0xf;
                cam.layer3_protocol_cam_layer4_type11 = entry->l4_type;
                break;
         case SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_14:
                cam.layer3_protocol_cam_additional_offset14 = entry->addition_offset;
                cam.layer3_protocol_cam_layer3_header_protocol14 = entry->l3_hdr_ptl;
                cam.layer3_protocol_cam_layer3_header_protocol_mask14 = entry->l3_hdr_ptl_mask & 0xff;
                cam.layer3_protocol_cam_layer3_type14 = entry->l3_type;
                cam.layer3_protocol_cam_layer3_type_mask14 = entry->l3_type_mask & 0xf;
                cam.layer3_protocol_cam_layer4_type14 = entry->l4_type;
                break;
         default:
                return CTC_E_INVALID_PARAM;
                break;
        }
        cam_valid.layer3_cam_entry_valid |= (1<<index);

        CTC_ERROR_RETURN(sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(lchip, &cam));
        CTC_ERROR_RETURN(
                    sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
    }

    return CTC_E_NONE;
}

/**
 @brief set the entry invalid based on the index
*/
extern int32
sys_humber_parser_remove_l3_type(uint8 index)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer3_protocol_cam_valid_t cam_valid;

    /*8:customer can config 9 entry at most, 7 entry have been config in system initialization.
    MAX_CTC_PARSER_L3_TYPE - CTC_PARSER_L3_TYPE_RARP*/

    /*6: already configed entry in system initialization*/
   index = index + SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_BASE;
   if( (index > SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_14)||
        (index == SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_12)||
        (index == SYS_PARSER_L3PRO_CAM_RSV_USER_DEFINE_13))
   {
      return CTC_E_INVALID_PARAM;
   }

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&cam_valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));
        CTC_ERROR_RETURN(
                            sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
            if(CTC_IS_BIT_SET(cam_valid.layer3_cam_entry_valid, index))
            {
                cam_valid.layer3_cam_entry_valid &= ~ (1 << index);
                CTC_ERROR_RETURN(
                            sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
            }
    }

    return CTC_E_NONE;
}

int32
sys_humber_parser_l3_enable_l4_type(ctc_parser_l4_type_t l4_type, bool enable)
{
    uint8 lchip = 0;
    uint8 lchip_num = 0;
    parser_layer3_protocol_cam_valid_t cam_valid;

    lchip_num = sys_humber_get_local_chip_num();

    CTC_MAX_VALUE_CHECK(l4_type, CTC_PARSER_L4_TYPE_ANY_PROTO);

    for(lchip = 0; lchip < lchip_num; lchip++)
    {
        kal_memset(&cam_valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));
        CTC_ERROR_RETURN(sys_humber_parser_io_get_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));

        if(FALSE == enable)  /*disable*/
        {
            if(CTC_IS_BIT_SET(cam_valid.layer3_cam_entry_valid, l4_type))
            {
                cam_valid.layer3_cam_entry_valid &= (~(1 << l4_type));
            }
            else
            {
                return CTC_E_NONE;
            }
        }
        else  /*enable*/
        {
            if(CTC_IS_BIT_SET(cam_valid.layer3_cam_entry_valid, l4_type))
            {
                return CTC_E_NONE;
            }
            else
            {
                cam_valid.layer3_cam_entry_valid |= (1 << l4_type);
            }
        }

        CTC_ERROR_RETURN(sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &cam_valid));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_set_layer4_hash_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    bool l4hash_op = FALSE;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_l4_hash_ctl_t hash_ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&hash_ctl, 0, sizeof(parser_l4_hash_ctl_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_l4_hash_ctl_entry(lchip, &hash_ctl));


        if(l4_parser_ctl->l4hash_ctl.field_flags & CTC_PARSER_L4_FLAGS_USE_L4HASH)

        {
            l4hash_op = TRUE;

            hash_ctl.use_layer4_hash = l4_parser_ctl->l4hash_ctl.use_l4hash?1:0;

        }

        if(l4_parser_ctl->l4hash_ctl.field_flags & CTC_PARSER_L4_FLAGS_SRC_PORT_HASH)

        {
            l4hash_op = TRUE;

            hash_ctl.source_port_hash_en = l4_parser_ctl->l4hash_ctl.src_port_hash?1:0;

        }

        if(l4_parser_ctl->l4hash_ctl.field_flags & CTC_PARSER_L4_FLAGS_DST_PORT_HASH)

        {
            l4hash_op = TRUE;

            hash_ctl.dest_port_hash_en = l4_parser_ctl->l4hash_ctl.dst_port_hash?1:0;

        }

        if(l4_parser_ctl->l4hash_ctl.field_flags & CTC_PARSER_L4_FLAGS_SRC_PORT_ECMP_HASH)

        {
            l4hash_op = TRUE;

            hash_ctl.source_port_ecmp_hash_en = l4_parser_ctl->l4hash_ctl.src_port_ecmp_hash?1:0;

        }

        if(l4_parser_ctl->l4hash_ctl.field_flags & CTC_PARSER_L4_FLAGS_DST_PORT_ECMP_HASH)

        {
            l4hash_op = TRUE;

            hash_ctl.dest_port_ecmp_hash_en = l4_parser_ctl->l4hash_ctl.dst_port_ecmp_hash?1:0;

        }
        if(l4hash_op)
        {
            CTC_ERROR_RETURN(
                        sys_humber_parser_io_set_parser_l4_hash_ctl_entry(lchip, &hash_ctl));
        }

    }
    return CTC_E_NONE;

}


static int32
_sys_humber_parser_get_layer4_hash_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{
    parser_l4_hash_ctl_t hash_ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);
    kal_memset(&hash_ctl, 0, sizeof(parser_l4_hash_ctl_t));

    CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_l4_hash_ctl_entry(0, &hash_ctl));


    l4_parser_ctl->l4hash_ctl.dst_port_ecmp_hash = hash_ctl.dest_port_ecmp_hash_en;
    l4_parser_ctl->l4hash_ctl.dst_port_hash = hash_ctl.dest_port_hash_en;
    l4_parser_ctl->l4hash_ctl.src_port_ecmp_hash = hash_ctl.source_port_ecmp_hash_en;
    l4_parser_ctl->l4hash_ctl.src_port_hash = hash_ctl.source_port_hash_en;
    l4_parser_ctl->l4hash_ctl.use_l4hash = hash_ctl.use_layer4_hash;


    return CTC_E_NONE;

}


int32
sys_humber_parser_set_layer4_flag_op_ctl(uint8 lchip, uint8 index, ctc_parser_l4flag_op_ctl_t* l4flag_op_ctl)

{
    bool op_en = FALSE;
    parser_layer4_flag_op_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4flag_op_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer4_flag_op_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer4_flag_op_ctl_entry(lchip, &ctl));
    switch(index)
    {

        case 0:
            if(l4flag_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_FLAG_OP_CTL0)
            {
                op_en = TRUE;
                ctl.layer4_op_and_or0 = l4flag_op_ctl->op_and_or ?1:0;
                ctl.layer4_op_flags_mask0 = l4flag_op_ctl->flags_mask & 0x3f;
            }
            break;
        case 1:
            if(l4flag_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_FLAG_OP_CTL1)
            {
                op_en = TRUE;
                ctl.layer4_op_and_or1 = l4flag_op_ctl->op_and_or ?1:0;
                ctl.layer4_op_flags_mask1 = l4flag_op_ctl->flags_mask & 0x3f;
            }
            break;
        case 2:
            if(l4flag_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_FLAG_OP_CTL2)
            {
                op_en = TRUE;
                ctl.layer4_op_and_or2 = l4flag_op_ctl->op_and_or ?1:0;
                ctl.layer4_op_flags_mask2 = l4flag_op_ctl->flags_mask & 0x3f;
            }
            break;
        case 3:
            if(l4flag_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_FLAG_OP_CTL3)
            {
                op_en = TRUE;
                ctl.layer4_op_and_or3 = l4flag_op_ctl->op_and_or ?1:0;
                ctl.layer4_op_flags_mask3 = l4flag_op_ctl->flags_mask & 0x3f;
            }
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;

    }

    if(op_en)
    {
        CTC_ERROR_RETURN(
        sys_humber_parser_io_set_parser_layer4_flag_op_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}


int32
sys_humber_parser_set_layer4_port_op_sel(uint8 lchip, ctc_parser_l4_port_op_sel_t* l4port_op_sel)

{
    bool op_en = FALSE;
    parser_layer4_port_op_sel_t ctl;

    CTC_PTR_VALID_CHECK(l4port_op_sel);

    kal_memset(&ctl, 0, sizeof(parser_layer4_port_op_sel_t));

    CTC_ERROR_RETURN(

        sys_humber_parser_io_get_parser_layer4_port_op_sel_entry(lchip, &ctl));
    if(l4port_op_sel->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_SEL_DEST_PORT)

    {
        op_en = TRUE;

         ctl.layer4_op_dest_port = l4port_op_sel->op_dest_port;

    }
    if(op_en)
    {
        CTC_ERROR_RETURN(
        sys_humber_parser_io_set_parser_layer4_port_op_sel_entry(lchip, &ctl));
    }

    return CTC_E_NONE;

}


int32
sys_humber_parser_set_layer4_port_op_ctl(uint8 lchip, uint8 index, ctc_parser_l4_port_op_ctl_t* l4port_op_ctl)

{
    bool op_en = FALSE;
    parser_layer4_port_op_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4port_op_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer4_port_op_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer4_port_op_ctl_entry(lchip, &ctl));
    switch(index)
    {

        case 0:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL0)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max0 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min0 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 1:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL1)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max1 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min1 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 2:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL2)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max2 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min2 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 3:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL3)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max3 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min3 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 4:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL4)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max4 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min4 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 5:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL5)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max5 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min5 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 6:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL6)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max6 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min6 = l4port_op_ctl->layer4_port_min;
            }
            break;
        case 7:
            if(l4port_op_ctl->field_flags & CTC_PARSER_L4_FLAGS_PORT_OP_CTL7)
            {
                op_en = TRUE;
                ctl.layer4_op_port_max7 = l4port_op_ctl->layer4_port_max;
                ctl.layer4_op_port_min7 = l4port_op_ctl->layer4_port_min;
            }
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;

    }

    if(op_en)
    {
        CTC_ERROR_RETURN(
        sys_humber_parser_io_set_parser_layer4_port_op_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_set_udp_app_op_ctl(uint8 index, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{
    bool op_en = FALSE;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_udp_app_op_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_udp_app_op_ctl_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_udp_app_op_ctl_entry(lchip, &ctl));
        switch(index)
        {

            case 0:
                if(l4_parser_ctl->udp_app_op_ctl.field_flags & CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL0)
                {
                    op_en = TRUE;
                    ctl.udp_app_mask0 = l4_parser_ctl->udp_app_op_ctl.udp_app_mask;
                    ctl.udp_app_value0 = l4_parser_ctl->udp_app_op_ctl.udp_app_value;
                }
                break;
            case 1:
                if(l4_parser_ctl->udp_app_op_ctl.field_flags & CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL1)
                {
                    op_en = TRUE;
                    ctl.udp_app_mask1 = l4_parser_ctl->udp_app_op_ctl.udp_app_mask;
                    ctl.udp_app_value1 = l4_parser_ctl->udp_app_op_ctl.udp_app_value;
                }
                break;
            case 2:
                if(l4_parser_ctl->udp_app_op_ctl.field_flags & CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL2)
                {
                    op_en = TRUE;
                    ctl.udp_app_mask2 = l4_parser_ctl->udp_app_op_ctl.udp_app_mask;
                    ctl.udp_app_value2 = l4_parser_ctl->udp_app_op_ctl.udp_app_value;
                }
                break;
            case 3:
                if(l4_parser_ctl->udp_app_op_ctl.field_flags & CTC_PARSER_L4_FLAGS_UDP_APP_OP_CTL3)
                {
                    op_en = TRUE;
                    ctl.udp_app_mask3 = l4_parser_ctl->udp_app_op_ctl.udp_app_mask;
                    ctl.udp_app_value3 = l4_parser_ctl->udp_app_op_ctl.udp_app_value;
                }
                break;
            default:
                return CTC_E_INVALID_PARAM;
                break;

        }

        if(op_en)
        {
            CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_udp_app_op_ctl_entry(lchip, &ctl));
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_get_udp_app_op_ctl(uint8 index, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    parser_udp_app_op_ctl_t ctl;
    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    kal_memset(&ctl, 0, sizeof(parser_udp_app_op_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_udp_app_op_ctl_entry(0, &ctl));


    switch(index)
    {
        case 0:
            l4_parser_ctl->udp_app_op_ctl.udp_app_mask =ctl.udp_app_mask0;
            l4_parser_ctl->udp_app_op_ctl.udp_app_value =ctl.udp_app_value0;
            break;
        case 1:
            l4_parser_ctl->udp_app_op_ctl.udp_app_mask =ctl.udp_app_mask1;
            l4_parser_ctl->udp_app_op_ctl.udp_app_value =ctl.udp_app_value1;
            break;
        case 2:
            l4_parser_ctl->udp_app_op_ctl.udp_app_mask =ctl.udp_app_mask2;
            l4_parser_ctl->udp_app_op_ctl.udp_app_value =ctl.udp_app_value2;
            break;
        case 3:
            l4_parser_ctl->udp_app_op_ctl.udp_app_mask =ctl.udp_app_mask3;
            l4_parser_ctl->udp_app_op_ctl.udp_app_value =ctl.udp_app_value3;
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;
    }


    return CTC_E_NONE;
}


static int32
_sys_humber_parser_set_layer4_len_op_ctl(uint8 index, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{
    bool op_en = FALSE;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer4_length_op_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_layer4_length_op_ctl_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer4_length_op_ctl_entry(lchip, &ctl));
        if(l4_parser_ctl->l4len_op_ctl.length > 0x3fff)
        {
            return CTC_E_INVALID_PARAM;
        }
        switch(index)
        {

            case 0:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL0)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length1 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 1:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL1)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length2 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 2:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL2)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length3 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 3:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL3)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length4 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 4:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL4)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length5 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 5:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL5)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length6 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 6:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL6)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length7 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 7:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL7)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length8 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 8:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL8)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length9 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 9:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL9)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length10 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 10:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL10)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length11 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 11:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL11)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length12 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 12:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL12)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length13 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 13:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL13)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length14 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            case 14:
                if (l4_parser_ctl->l4len_op_ctl.l4len_op_flags & CTC_PARSER_L4LEN_OP_FLAGS_CTL14)
                {
                    op_en = TRUE;
                    ctl.layer4_op_length15 = l4_parser_ctl->l4len_op_ctl.length;
                }
                break;
            default:
                return CTC_E_INVALID_PARAM;
                break;
        }

        if(op_en)
        {
            CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_length_op_ctl_entry(lchip, &ctl));
        }

    }

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_get_layer4_len_op_ctl(uint8 index, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{

    parser_layer4_length_op_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer4_length_op_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer4_length_op_ctl_entry(0, &ctl));

    switch(index)
    {
        case 0:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length1;
            break;
        case 1:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length2;
            break;
        case 2:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length3;
            break;
        case 3:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length4;
            break;
        case 4:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length5;
            break;
        case 5:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length6;
            break;
        case 6:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length7;
            break;
        case 7:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length8;
            break;
        case 8:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length9;
            break;
        case 9:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length10;
            break;
        case 10:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length11;
            break;
        case 11:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length12;
            break;
        case 12:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length13;
            break;
        case 13:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length14;
            break;
        case 14:
            l4_parser_ctl->l4len_op_ctl.length = ctl.layer4_op_length15;
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;
    }


    return CTC_E_NONE;
}



static int32
_sys_humber_parser_set_layer4_flex_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{
    bool op_en = FALSE;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer4_flex_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_layer4_flex_ctl_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer4_flex_ctl_entry(lchip, &ctl));
        if(l4_parser_ctl->l4flex_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4FLEX_CTL_BYTE_SEL0)
        {
            op_en = TRUE;
            if(l4_parser_ctl->l4flex_ctl.byte_select0 > 0x1f)
            {
                return CTC_E_INVALID_PARAM;
            }
            ctl.layer4_byte_select0 = l4_parser_ctl->l4flex_ctl.byte_select0;

        }

        if(l4_parser_ctl->l4flex_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4FLEX_CTL_BYTE_SEL1)

        {
            op_en = TRUE;
            if(l4_parser_ctl->l4flex_ctl.byte_select1 > 0x1f)
            {
                return CTC_E_INVALID_PARAM;
            }
            ctl.layer4_byte_select1 = l4_parser_ctl->l4flex_ctl.byte_select1;

        }

        if(l4_parser_ctl->l4flex_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4FLEX_CTL_APP_MIN_LEN)

        {
            op_en = TRUE;
            if(l4_parser_ctl->l4flex_ctl.l4_app_min_len > 0x1f)
            {
                return  CTC_E_INVALID_PARAM;
            }
            ctl.layer4_app_min_length = l4_parser_ctl->l4flex_ctl.l4_app_min_len;

        }

        if(l4_parser_ctl->l4flex_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4FLEX_CTL_MIN_LEN)
        {
            op_en = TRUE;
            if(l4_parser_ctl->l4flex_ctl.l4_min_len > 0x1f)
            {
                return CTC_E_INVALID_PARAM;
            }
            ctl.layer4_min_length = l4_parser_ctl->l4flex_ctl.l4_min_len;
        }
        if(op_en)
        {
            CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_layer4_flex_ctl_entry(lchip, &ctl));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_parser_get_layer4_flex_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    parser_layer4_flex_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer4_flex_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer4_flex_ctl_entry(0, &ctl));


    l4_parser_ctl->l4flex_ctl.byte_select0 = ctl.layer4_byte_select0;
    l4_parser_ctl->l4flex_ctl.byte_select1 = ctl.layer4_byte_select1;
    l4_parser_ctl->l4flex_ctl.l4_app_min_len = ctl.layer4_app_min_length;
    l4_parser_ctl->l4flex_ctl.l4_min_len = ctl.layer4_min_length;

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_set_layer4_ptp_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)

{
    bool op_en = FALSE;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_layer4_ptp_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    local_chip_num = sys_humber_get_local_chip_num();

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&ctl, 0, sizeof(parser_layer4_ptp_ctl_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_layer4_ptp_ctl_entry(lchip, &ctl));

        if(l4_parser_ctl->l4ptp_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4PTP_CTL_PTP_EN)
        {
            op_en = TRUE;

            ctl.ptp_en = l4_parser_ctl->l4ptp_ctl.ptp_en?1:0;

        }

         if(l4_parser_ctl->l4ptp_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4PTP_CTL_PORT0)

        {
            op_en = TRUE;

            ctl.ptp_port0 = l4_parser_ctl->l4ptp_ctl.ptp_port0;

        }

          if(l4_parser_ctl->l4ptp_ctl.field_flags & CTC_PARSER_L4_FLAGS_L4PTP_CTL_PORT1)

        {
            op_en = TRUE;

            ctl.ptp_port1 = l4_parser_ctl->l4ptp_ctl.ptp_port1;

        }

        if(op_en)
        {
            CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_layer4_ptp_ctl_entry(lchip, &ctl));
        }
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_parser_get_layer4_ptp_ctl(ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    parser_layer4_ptp_ctl_t ctl;

    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    kal_memset(&ctl, 0, sizeof(parser_layer4_ptp_ctl_t));

    CTC_ERROR_RETURN(
        sys_humber_parser_io_get_parser_layer4_ptp_ctl_entry(0, &ctl));


    l4_parser_ctl->l4ptp_ctl.ptp_en = ctl.ptp_en;
    l4_parser_ctl->l4ptp_ctl.ptp_port0 = ctl.ptp_port0;
    l4_parser_ctl->l4ptp_ctl.ptp_port1 = ctl.ptp_port1;

    return CTC_E_NONE;
}

/**
 @brief set layer4 parser reg
*/
extern int32
sys_humber_parser_set_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    CTC_PTR_VALID_CHECK(l4_parser_ctl);

    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4HASH)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_set_layer4_hash_ctl( l4_parser_ctl));
    }
    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_UDP_APP_OP_CTL)
    {
        if(l4_parser_ctl->udp_app_op_ctl.index >= SYS_PAS_MAX_UDP_APP_OP_ENTRY)
        {
            return CTC_E_INVALID_PARAM;
        }

        CTC_ERROR_RETURN(_sys_humber_parser_set_udp_app_op_ctl(l4_parser_ctl->udp_app_op_ctl.index, l4_parser_ctl));
    }
    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4LEN_OP_CTL)
    {
        if(l4_parser_ctl->l4len_op_ctl.index >= SYS_PAS_MAX_LAYER4_LEN_OP_ENTRY)
        {
            return CTC_E_INVALID_PARAM;
        }
        CTC_ERROR_RETURN(_sys_humber_parser_set_layer4_len_op_ctl(l4_parser_ctl->l4len_op_ctl.index, l4_parser_ctl));
    }
    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4FLEX_CTL)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_set_layer4_flex_ctl(l4_parser_ctl));
    }
    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4PTP_CTL)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_set_layer4_ptp_ctl(l4_parser_ctl));
    }

    return CTC_E_NONE;
}

/**
 @brief get layer4 parser info
*/
extern int32
sys_humber_parser_get_layer4_parser_ctl(ctc_parser_layer4_tbl_flags_t tbl_flags, ctc_parser_layer4_parser_ctl_t* l4_parser_ctl)
{
    CTC_PTR_VALID_CHECK(l4_parser_ctl);
    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4HASH)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_get_layer4_hash_ctl( l4_parser_ctl));
    }

    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_UDP_APP_OP_CTL)
    {
        if(l4_parser_ctl->udp_app_op_ctl.index >= SYS_PAS_MAX_UDP_APP_OP_ENTRY)
        {
            return CTC_E_INVALID_PARAM;
        }
        CTC_ERROR_RETURN(_sys_humber_parser_get_udp_app_op_ctl(l4_parser_ctl->udp_app_op_ctl.index, l4_parser_ctl));
    }

    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4LEN_OP_CTL)
    {
        if(l4_parser_ctl->l4len_op_ctl.index >= SYS_PAS_MAX_LAYER4_LEN_OP_ENTRY)
        {
            return CTC_E_INVALID_PARAM;
        }
        CTC_ERROR_RETURN(_sys_humber_parser_get_layer4_len_op_ctl(l4_parser_ctl->l4len_op_ctl.index, l4_parser_ctl));
    }


    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4FLEX_CTL)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_get_layer4_flex_ctl(l4_parser_ctl));
    }


    if(tbl_flags & CTC_PARSER_LAYER4_TBL_FLAGS_L4PTP_CTL)
    {
        CTC_ERROR_RETURN(_sys_humber_parser_get_layer4_ptp_ctl(l4_parser_ctl));
    }

    return CTC_E_NONE;
}

/**

 @brief add l4type
*/
int32
sys_humber_parser_set_layer4_type(uint8 index, ctc_parser_l4_ptl_entry_t* entry)
{
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    parser_application_cam_t cam;

    /*the max entry num is 2*/
    if(index >= SYS_PAS_MAX_L4_PTL_CAM_ENTRY_NUM)
    {
        return CTC_E_INVALID_PARAM;
    }
    CTC_PTR_VALID_CHECK(entry);

    local_chip_num = sys_humber_get_local_chip_num();
    if((entry->byte0_select > 0x1f)
        ||(entry->byte1_select > 0x1f)
        ||(entry->tcp_flag_value > 0x3f))
    {
        return CTC_E_INVALID_PARAM;
    }
    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        kal_memset(&cam, 0, sizeof(parser_application_cam_t));

        CTC_ERROR_RETURN(
            sys_humber_parser_io_get_parser_application_cam_entry(lchip, &cam));

            switch(index)
            {

                case 0:
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_VALUE)
                    {
                        cam.application_cam_source_port_value0 = entry->src_port_value;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_DST_PORT_VALUE)
                    {
                        cam.application_cam_dest_port_value0 = entry->dst_port_value;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_APP_TYPE)
                    {
                         cam.application_cam_application_type0 = entry->application_type?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE0_SEL)
                    {
                         if (entry->byte0_select > 0x1F)
                         {
                              return CTC_E_INVALID_PARAM;
                         }
                        cam.application_cam_byte0_select0 = entry->byte0_select;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE_SEL_DEST)
                    {
                        cam.application_cam_byte_select_dest0 = entry->byte_select_dest?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_ENTRY_VLD)
                    {
                        cam.application_cam_entry_valid0 = entry->entry_vld?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE1_SEL)
                    {
                         if (entry->byte1_select > 0x1F)
                         {
                              return CTC_E_INVALID_PARAM;
                         }
                         cam.application_cam_byte1_select0 = entry->byte1_select;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_MASK)
                    {
                        cam.application_cam_source_port_mask0 = entry->source_port_mask?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_DEST_PORT_MASK)
                    {
                        cam.application_cam_dest_port_mask0 = entry->dest_port_mask?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_MASK)
                    {
                        if (entry->tcp_flag_mask > 0x3F)
                        {
                            return CTC_E_INVALID_PARAM;
                        }
                        cam.application_cam_tcp_flag_mask0 = entry->tcp_flag_mask & 0x3f;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_IS_TCP_MASK)
                    {
                        cam.application_cam_is_tcp_mask0 = entry->is_tcp_mask ?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_IS_TCP_VALUE)
                    {
                        cam.application_cam_is_tcp_value0 = entry->is_tcp_value?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_VALUE)
                    {
                        if (entry->tcp_flag_value > 0x3F)
                        {
                            return CTC_E_INVALID_PARAM;
                        }
                       cam.application_cam_tcp_flag_value0 = entry->tcp_flag_value;
                    }
                    break;
                case 1:
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_VALUE)
                    {
                        cam.application_cam_source_port_value1 = entry->src_port_value;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_DST_PORT_VALUE)
                    {
                        cam.application_cam_dest_port_value1 = entry->dst_port_value;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_APP_TYPE)
                    {
                        cam.application_cam_application_type1 = entry->application_type?1:0;

                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE0_SEL)
                    {
                         if (entry->byte0_select > 0x1F)
                         {
                              return CTC_E_INVALID_PARAM;
                         }
                        cam.application_cam_byte0_select1 = entry->byte0_select;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE_SEL_DEST)
                    {
                        cam.application_cam_byte_select_dest1 = entry->byte_select_dest?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_ENTRY_VLD)
                    {
                        cam.application_cam_entry_valid1 = entry->entry_vld?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_BYTE1_SEL)
                    {
                         if (entry->byte1_select > 0x1F)
                         {
                              return CTC_E_INVALID_PARAM;
                         }
                        cam.application_cam_byte1_select1 = entry->byte1_select;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_SRC_PORT_MASK)
                    {
                        cam.application_cam_source_port_mask1 = entry->source_port_mask?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_DEST_PORT_MASK)
                    {
                        cam.application_cam_dest_port_mask1 = entry->dest_port_mask?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_MASK)
                    {
                        if (entry->tcp_flag_mask > 0x3F)
                        {
                            return CTC_E_INVALID_PARAM;
                        }
                        cam.application_cam_tcp_flag_mask1 = entry->tcp_flag_mask & 0x3f;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_IS_TCP_MASK)
                    {
                        cam.application_cam_is_tcp_mask1 = entry->is_tcp_mask?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_IS_TCP_VALUE)
                    {
                        cam.application_cam_is_tcp_value1 = entry->is_tcp_value?1:0;
                    }
                    if(entry->flags & CTC_PARSER_L4_PTL_FLAGS_TCP_FLAG_VALUE)
                    {
                        if (entry->tcp_flag_value > 0x3F)
                        {
                            return CTC_E_INVALID_PARAM;
                        }
                        cam.application_cam_tcp_flag_value1 = entry->tcp_flag_value;
                    }
                    break;
                default:
                    return CTC_E_INVALID_PARAM;
                    break;
            }

        CTC_ERROR_RETURN(
                sys_humber_parser_io_set_parser_application_cam_entry(lchip, &cam));
    }

    return CTC_E_NONE;
}

static int32
 _sys_humber_parser_l2pro_cam_init(uint8 local_chip_num)
{
    uint8 lchip;
    parser_layer2_protocol_cam_t cam;
    parser_layer2_protocol_cam_valid_t valid;

    kal_memset(&cam, 0, sizeof(parser_layer2_protocol_cam_t));
    kal_memset(&valid, 0, sizeof(parser_layer2_protocol_cam_valid_t));

    /*none*/
    /*ip*/

    /*ipv4*/
    cam.layer2_protocol_cam_layer3_type2 = CTC_PARSER_L3_TYPE_IPV4;
    cam.layer2_protocol_cam_additional_offset2 = 0;
    cam.layer2_protocol_cam_value2 = SYS_PAS_L2_CAM_L2CAM_VALUE_IPV4;
    cam.layer2_protocol_cam_mask2 = 0x40FFFF;

 /*
    index : 3~15 reserved for user define
    #define SYS_PARSER_L2PRO_CAM_RSV_USER_DEFINE_BASE   CTC_PARSER_L3_TYPE_IPV6
 */
    valid.layer2_cam_entry_valid = 0x4;
    /*mpls*/
    cam.layer2_protocol_cam_layer3_type4 = CTC_PARSER_L3_TYPE_MPLS;
    cam.layer2_protocol_cam_additional_offset4 = 0;
    cam.layer2_protocol_cam_value4 = SYS_PAS_L2_CAM_L2CAM_VALUE_MPLS;
    cam.layer2_protocol_cam_mask4 = 0x40FFFF;

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer2_protocol_cam_valid_entry(lchip, &valid));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer2_protocol_cam_entry(lchip, &cam));
    }

    return CTC_E_NONE;

}

static int32
 _sys_humber_parser_pbb_ctl_init(uint8 local_chip_num)
{

    uint8 lchip;
    parser_pbb_ctl_t ctl;

    kal_memset(&ctl, 0, sizeof(parser_pbb_ctl_t));

    ctl.pbb_vlan_parsing_num = SYS_PAS_PBB_VLAN_PAS_NUM;

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_pbb_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;
}

static int32
 _sys_humber_parser_packet_type_map_init(uint8 local_chip_num)
{

    uint8 lchip;
    parser_packet_type_table_t packet_type;

    kal_memset(&packet_type, 0, sizeof(parser_packet_type_table_t));

    packet_type.packet_type_layer2_type0 = CTC_PARSER_L2_TYPE_ETH_V2;
    packet_type.packet_type_layer3_type0 = CTC_PARSER_L3_TYPE_NONE;
    packet_type.packet_type_layer2_type1 = CTC_PARSER_L2_TYPE_NONE;
    packet_type.packet_type_layer3_type1 = CTC_PARSER_L3_TYPE_IPV4;
    packet_type.packet_type_layer2_type2 = CTC_PARSER_L2_TYPE_NONE;
    packet_type.packet_type_layer3_type2 = CTC_PARSER_L3_TYPE_MPLS;
    packet_type.packet_type_layer2_type3 = CTC_PARSER_L2_TYPE_NONE;
    packet_type.packet_type_layer3_type3 = CTC_PARSER_L3_TYPE_IPV6;
    packet_type.packet_type_layer2_type4 = CTC_PARSER_L2_TYPE_NONE;
    packet_type.packet_type_layer3_type4 = CTC_PARSER_L3_TYPE_IP;
    packet_type.packet_type_layer2_type5 = CTC_PARSER_L2_TYPE_PPP_1B;
    packet_type.packet_type_layer3_type5 = CTC_PARSER_L3_TYPE_NONE;
    packet_type.packet_type_layer2_type6 = CTC_PARSER_L2_TYPE_NONE; /*CTC_PARSER_L2_TYPE_PPP_2B;*/
    packet_type.packet_type_layer3_type6 = CTC_PARSER_L3_TYPE_NONE;
    packet_type.packet_type_layer2_type7 = CTC_PARSER_L2_TYPE_NONE;
    packet_type.packet_type_layer3_type7 = CTC_PARSER_L3_TYPE_NONE;

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_packet_type_table_entry(lchip, &packet_type));

    }

    return CTC_E_NONE;
}

static int32
_sys_humber_parser_ipv6_ctl_init(uint8 local_chip_num)
{
    parser_ipv6_ctl_t ctl;
    uint8 lchip = 0;

    kal_memset(&ctl, 0, sizeof(parser_ipv6_ctl_t));
    /*hop by hop*/
    ctl.ipv6_set_ip_options0 = 1;
    ctl.ipv6_ext_shift0 = 0;
    ctl.ipv6_ext_level0 = SYS_PAS_IPV6_EXT_LEVEL_HOP_BY_HOP;
    ctl.ipv6_ext_level_check_en0 = 1;

    /*destination*/
    ctl.ipv6_set_ip_options1 = 0;
    ctl.ipv6_ext_shift1 = 0;
    ctl.ipv6_ext_level1 = SYS_PAS_IPV6_EXT_LEVEL_DESTINATION;
    ctl.ipv6_ext_level_check_en1 = 1;

    /*routing*/
    ctl.ipv6_set_ip_options2 = 0;
    ctl.ipv6_ext_shift2 = 0;
    ctl.ipv6_ext_level2 = SYS_PAS_IPV6_EXT_LEVEL_ROUTING;
    ctl.ipv6_ext_level_check_en2 = 1;

    /*fragment*/
    ctl.ipv6_set_ip_options3 = 0;
    ctl.ipv6_ext_shift3 = 0;
    ctl.ipv6_ext_level3 = SYS_PAS_IPV6_EXT_LEVEL_FRAG;
    ctl.ipv6_ext_level_check_en3 = 1;

    /*ah*/
    ctl.ipv6_set_ip_options4 = 0;
    ctl.ipv6_ext_shift4 = 1;
    ctl.ipv6_ext_level4 = SYS_PAS_IPV6_EXT_LEVEL_AH;
    ctl.ipv6_ext_level_check_en4 = 1;

    /*esp*/
    ctl.ipv6_set_ip_options5 = 0;
    ctl.ipv6_ext_shift5 = 0;
    ctl.ipv6_ext_level5 = SYS_PAS_IPV6_EXT_LEVEL_ESP;
    ctl.ipv6_ext_level_check_en5 = 1;

    ctl.parser_ipv6_error_option_en = 1;

    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_ipv6_ctl_entry(lchip, &ctl));
    }

    return CTC_E_NONE;

}

static int32
_sys_humber_parser_l3pro_cam_init(uint8 local_chip_num)
{
    uint8 lchip;
    parser_layer3_protocol_cam_t cam;
    parser_layer3_protocol_cam_valid_t valid;

    kal_memset(&cam, 0, sizeof(parser_layer3_protocol_cam_t));
    kal_memset(&valid, 0, sizeof(parser_layer3_protocol_cam_valid_t));

    /* layer4 type is none*/
    /*l3 IP/IPv4/IPv6, l4 TCP*/
    cam.layer3_protocol_cam_layer4_type0 = CTC_PARSER_L4_TYPE_TCP;
    cam.layer3_protocol_cam_layer3_type_mask0 = 0xC;
    cam.layer3_protocol_cam_layer3_header_protocol_mask0 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol0 = 6;

    /*l3 IP/IPv4/IPv6, l4 UDP*/
    cam.layer3_protocol_cam_layer4_type1 = CTC_PARSER_L4_TYPE_UDP;
    cam.layer3_protocol_cam_layer3_type_mask1 = 0xC;
    cam.layer3_protocol_cam_layer3_header_protocol_mask1 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol1 = 17;

    /* l3 IPv4, GRE*/
    cam.layer3_protocol_cam_layer4_type2 = CTC_PARSER_L4_TYPE_GRE;
    cam.layer3_protocol_cam_layer3_type_mask2 = 0xF;
    cam.layer3_protocol_cam_layer3_type2 = CTC_PARSER_L3_TYPE_IPV4;
    cam.layer3_protocol_cam_layer3_header_protocol_mask2 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol2 = 47;

    /*l3 IPv4, IP-in-Ip*/
    cam.layer3_protocol_cam_layer4_type3 = CTC_PARSER_L4_TYPE_IPINIP;
    cam.layer3_protocol_cam_layer3_type_mask3 = 0xF;
    cam.layer3_protocol_cam_layer3_type3 = CTC_PARSER_L3_TYPE_IPV4;
    cam.layer3_protocol_cam_layer3_header_protocol_mask3 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol3 = 4;

    /*l3 IPv4, IPv6-in-Ip*/
    cam.layer3_protocol_cam_layer4_type4 = CTC_PARSER_L4_TYPE_V6INIP;
    cam.layer3_protocol_cam_layer3_type_mask4 = 0xF;
    cam.layer3_protocol_cam_layer3_type4 = CTC_PARSER_L3_TYPE_IPV4;
    cam.layer3_protocol_cam_layer3_header_protocol_mask4 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol4 = 41;

    /*l3 IP/IPv4/IPv6, l4 ICMP*/
    cam.layer3_protocol_cam_layer4_type5 = CTC_PARSER_L4_TYPE_ICMP;
    cam.layer3_protocol_cam_layer3_type_mask5 = 0xC;
    cam.layer3_protocol_cam_layer3_type5 = CTC_PARSER_L3_TYPE_NONE;
    cam.layer3_protocol_cam_layer3_header_protocol_mask5 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol5 = 1;

    /*l3 IPv4, l4 IGMP*/
    cam.layer3_protocol_cam_layer4_type6 = CTC_PARSER_L4_TYPE_IGMP;
    cam.layer3_protocol_cam_layer3_type_mask6 = 0xF;
    cam.layer3_protocol_cam_layer3_type6 = CTC_PARSER_L3_TYPE_IPV4;
    cam.layer3_protocol_cam_layer3_header_protocol_mask6 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol6 = 2;

    /* l3 IPv6, ICMPv6(include IGMP for IPv6)*/
    cam.layer3_protocol_cam_layer4_type7 = CTC_PARSER_L4_TYPE_ICMP;
    cam.layer3_protocol_cam_layer3_type_mask7 = 0xF;
    cam.layer3_protocol_cam_layer3_type7 = CTC_PARSER_L3_TYPE_IPV6;
    cam.layer3_protocol_cam_layer3_header_protocol_mask7 = 0xFF;
    cam.layer3_protocol_cam_layer3_header_protocol7 = 58;

    /* layer4 type is any proto*/
    cam.layer3_protocol_cam_layer4_type15 = CTC_PARSER_L4_TYPE_ANY_PROTO;
    cam.layer3_protocol_cam_layer3_header_protocol15 = 0;
    cam.layer3_protocol_cam_layer3_type15 = CTC_PARSER_L3_TYPE_NONE;
    cam.layer3_protocol_cam_layer3_type_mask15 = 0xC;
    cam.layer3_protocol_cam_layer3_header_protocol_mask15 = 0;

    valid.layer3_cam_entry_valid = 0x80FF;

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer3_protocol_cam_valid_entry(lchip, &valid));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer3_protocol_cam_entry(lchip, &cam));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_parser_app_cam_init(uint8 local_chip_num)
{
    uint8 lchip;
    parser_application_cam_t cam;

    kal_memset(&cam, 0, sizeof(parser_application_cam_t));

    cam.application_cam_application_type0 = 0;
    cam.application_cam_application_type1 = 0;
    cam.application_cam_source_port_value0 = 0;
    cam.application_cam_source_port_value1 = 0;

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_application_cam_entry(lchip, &cam));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_parser_l4_reg_ctl_init(uint8 local_chip_num)
{
    uint8 lchip;
    parser_layer4_length_op_ctl_t l4len_opctl;
    parser_layer4_port_op_sel_t port_opsel;
    parser_layer4_port_op_ctl_t port_opctl;
    parser_udp_app_op_ctl_t udp_app_opctl;
    parser_layer4_flag_op_ctl_t l4flag_opctl;
    parser_layer4_flex_ctl_t l4flex_ctl;
    parser_layer4_ptp_ctl_t l4ptp_ctl;
    parser_l4_hash_ctl_t l4hash_ctl;

    kal_memset(&l4hash_ctl, 0, sizeof(parser_l4_hash_ctl_t));
    kal_memset(&l4len_opctl, 0, sizeof(parser_application_cam_t));
    kal_memset(&port_opsel, 0, sizeof(parser_layer4_port_op_sel_t));
    kal_memset(&port_opctl, 0, sizeof(parser_layer4_port_op_ctl_t));
    kal_memset(&udp_app_opctl, 0, sizeof(parser_udp_app_op_ctl_t));
    kal_memset(&l4flag_opctl, 0, sizeof(parser_layer4_flag_op_ctl_t));
    kal_memset(&l4flex_ctl, 0, sizeof(parser_layer4_flex_ctl_t));
    kal_memset(&l4ptp_ctl, 0, sizeof(parser_layer4_ptp_ctl_t));

    l4len_opctl.layer4_op_length1 = 0x1;
    l4len_opctl.layer4_op_length2 = 0x2;
    l4len_opctl.layer4_op_length3 = 0x4;
    l4len_opctl.layer4_op_length4 = 0x8;
    l4len_opctl.layer4_op_length5 = 0x10;
    l4len_opctl.layer4_op_length6 = 0x20;
    l4len_opctl.layer4_op_length7 = 0x40;
    l4len_opctl.layer4_op_length8 = 0x80;
    l4len_opctl.layer4_op_length9 = 0x100;
    l4len_opctl.layer4_op_length10 = 0x200;
    l4len_opctl.layer4_op_length11 = 0x400;
    l4len_opctl.layer4_op_length12 = 0x800;
    l4len_opctl.layer4_op_length13 = 0x1000;
    l4len_opctl.layer4_op_length14 = 0x2000;

    port_opsel.layer4_op_dest_port = 0xf;

    port_opctl.layer4_op_port_max0 = 0x4;
    port_opctl.layer4_op_port_min0 = 0x1;
    port_opctl.layer4_op_port_max1 = 0x8;
    port_opctl.layer4_op_port_min1 =  0x2;
    port_opctl.layer4_op_port_max2 = 0x10;
    port_opctl.layer4_op_port_min2 = 0x4;
    port_opctl.layer4_op_port_max3 = 0x20;
    port_opctl.layer4_op_port_min3 = 0x8;
    port_opctl.layer4_op_port_max4 = 0x40;
    port_opctl.layer4_op_port_min4 = 0x10;
    port_opctl.layer4_op_port_max5 = 0x80;
    port_opctl.layer4_op_port_min5 = 0x20;
    port_opctl.layer4_op_port_max6 = 0x100;
    port_opctl.layer4_op_port_min6 = 0x40;
    port_opctl.layer4_op_port_max7 = 0x200;
    port_opctl.layer4_op_port_min7 = 0x80;

    udp_app_opctl.udp_app_mask0 = 0xffff;
    udp_app_opctl.udp_app_value0 = 0;
    udp_app_opctl.udp_app_mask1 = 0xffff;
    udp_app_opctl.udp_app_value1 = 0;
    udp_app_opctl.udp_app_mask2 = 0xffff;
    udp_app_opctl.udp_app_value2 = 0;
    udp_app_opctl.udp_app_mask3 = 0xffff;
    udp_app_opctl.udp_app_value3 = 0;

    l4flag_opctl.layer4_op_and_or0 = 0;
    l4flag_opctl.layer4_op_flags_mask0 = 0;
    l4flag_opctl.layer4_op_and_or1 = 0;
    l4flag_opctl.layer4_op_flags_mask1 = 0x3f;
    l4flag_opctl.layer4_op_and_or2 = 1;
    l4flag_opctl.layer4_op_flags_mask2 = 0;
    l4flag_opctl.layer4_op_and_or3 = 1;
    l4flag_opctl.layer4_op_flags_mask3 = 0x3f;

    l4flex_ctl.layer4_byte_select0 = 2;
    l4flex_ctl.layer4_byte_select1 = 3;

    l4ptp_ctl.ptp_en = 1;
    l4ptp_ctl.ptp_port0 = 319;
    l4ptp_ctl.ptp_port1 = 320;

    l4hash_ctl.source_port_hash_en = 0;

    for(lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_length_op_ctl_entry(lchip, &l4len_opctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_port_op_sel_entry(lchip, &port_opsel));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_port_op_ctl_entry(lchip, &port_opctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_udp_app_op_ctl_entry(lchip, &udp_app_opctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_flag_op_ctl_entry(lchip, &l4flag_opctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_flex_ctl_entry(lchip, &l4flex_ctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_layer4_ptp_ctl_entry(lchip, &l4ptp_ctl));
        CTC_ERROR_RETURN(
            sys_humber_parser_io_set_parser_l4_hash_ctl_entry(lchip, &l4hash_ctl));
    }

    return CTC_E_NONE;

}
/**
 @brief init parser module
*/
extern int32
sys_humber_parser_init(void)
{

    uint8 local_chip_num = 0;

    local_chip_num = sys_humber_get_local_chip_num();

    CTC_ERROR_RETURN(
        _sys_humber_parser_l2pro_cam_init(local_chip_num));

    CTC_ERROR_RETURN(
        _sys_humber_parser_pbb_ctl_init(local_chip_num));

    CTC_ERROR_RETURN(
        _sys_humber_parser_packet_type_map_init(local_chip_num));

    /*l3 parser init*/
    CTC_ERROR_RETURN(
        _sys_humber_parser_ipv6_ctl_init(local_chip_num));
    CTC_ERROR_RETURN(
        _sys_humber_parser_l3pro_cam_init(local_chip_num));

    /*l4 parser init*/
    CTC_ERROR_RETURN(
        _sys_humber_parser_app_cam_init(local_chip_num));
    CTC_ERROR_RETURN(
        _sys_humber_parser_l4_reg_ctl_init(local_chip_num));

    return CTC_E_NONE;

}

