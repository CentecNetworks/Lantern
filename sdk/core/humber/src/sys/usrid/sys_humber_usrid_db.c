/**
 @file sys_humber_usrid_db.c

 @date 2009-10-10

 @version v2.0

 To define DB fucntion of usrid
*/

/****************************************************************
*
* Header Files
*
***************************************************************/
#include "ctc_debug.h"
#include "ctc_const.h"
#include "ctc_error.h"
#include "sys_humber_usrid.h"
#include "sys_humber_usrid_db.h"
#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_ftm.h"

#include "drv_io.h"
/****************************************************************
*
* Global and Declaration
*
****************************************************************/
static sys_usrid_db_t *p_usrid_db = NULL;
/****************************************************************
*
* Functions
*
****************************************************************/

static ctc_avl_tree_t**
_sys_humber_usrid_db_get_tree(uint8 lchip, uint32 key_type, uint16 label_or_port)
{
    switch (key_type)
    {
        case SYS_USRID_VLAN_KEY:
            return &(p_usrid_db[lchip].vlan_tree[label_or_port]);

        case SYS_USRID_MAC_KEY:
            return &(p_usrid_db[lchip].mac_tree[label_or_port]);

        case SYS_USRID_IPV4_KEY:
            return &(p_usrid_db[lchip].ipv4_tree[label_or_port]);

        case SYS_USRID_IPV6_KEY:
            return &(p_usrid_db[lchip].ipv6_tree[label_or_port]);

        default:
            return NULL;
    }

}

static int32
_sys_humber_usrid_db_offset_init(uint32 key_type)
{
    uint8 lchip, lchip_num;
    uint32 table_id;
    uint32 entry_num;
    sys_humber_opf_t opf;

    switch(key_type)
    {
        case SYS_USRID_VLAN_KEY:
            table_id = DS_USER_ID_VLAN_KEY;
            break;

        case SYS_USRID_MAC_KEY:
            table_id = DS_USER_ID_MAC_KEY;
            break;

        case SYS_USRID_IPV4_KEY:
            table_id = DS_USER_ID_IPV4_KEY;
            break;

        case SYS_USRID_IPV6_KEY:
            table_id = DS_USER_ID_IPV6_KEY;
            break;

        default:
            return CTC_E_INVALID_USRID_KEY;
    }

    lchip_num = sys_humber_get_local_chip_num();
    sys_alloc_get_table_entry_num(table_id, &entry_num);

    /*reserve 1 global default entry.
    per port(linkagg)/per label default entry can be supported by OPF*/

    if (0 != entry_num)
    {
        CTC_ERROR_RETURN(sys_humber_opf_init(key_type, lchip_num));

        for (lchip = 0; lchip< lchip_num; lchip++)
        {
            opf.pool_index = lchip;
            opf.pool_type = key_type;
            CTC_ERROR_RETURN(sys_humber_opf_init_offset(&opf, 0, entry_num - 2));
        }
    }

    return CTC_E_NONE;
}

static int32
db_usrid_vlan_cmp_fun(void *data1, void *data2)
{
    int32 ret = 0;
    sys_usrid_valid_t *p_valid_1, *p_valid_2;
    sys_usrid_db_vlankey_node_t *p_node_1, *p_node_2;

    p_valid_1 = &(((sys_usrid_db_vlan_node_t *)data1)->valid);
    p_valid_2 = &(((sys_usrid_db_vlan_node_t *)data2)->valid);

    p_node_1 = &(((sys_usrid_db_vlan_node_t *)data1)->usrid_key_node);
    p_node_2 = &(((sys_usrid_db_vlan_node_t *)data2)->usrid_key_node);

    if (0 != (ret = kal_memcmp(p_valid_1, p_valid_2, sizeof(sys_usrid_valid_t))))
    {
        return ret;
    }

    /*cvid*/
    if ((p_valid_1->igs_cvid_valid) && (p_valid_2->igs_cvid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->igs_cvid_mask, p_node_2->igs_cvid_mask);
        CTC_INTEGER_CMP(p_node_1->igs_cvid, p_node_2->igs_cvid);
    }

    /*svid*/
    if ((p_valid_1->igs_svid_valid) && (p_valid_2->igs_svid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->igs_svid_mask, p_node_2->igs_svid_mask);
        CTC_INTEGER_CMP(p_node_1->igs_svid, p_node_2->igs_svid);
    }

    /*gre key*/
    if ((p_valid_1->gre_valid) && (p_valid_2->gre_valid))
    {
        CTC_INTEGER_CMP(p_node_1->customer_id & 0xFFFFFFFF, p_node_2->customer_id & 0xFFFFFFFF);
    }

    /*isid*/
    if ((p_valid_1->isid_valid) && (p_valid_2->isid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->customer_id & 0xFFFFFF00, p_node_2->customer_id & 0xFFFFFF00);
    }

    /*vc label*/
    if ((p_valid_1->vc_valid) && (p_valid_2->vc_valid))
    {
        CTC_INTEGER_CMP(p_node_1->customer_id & 0xFFFFF000, p_node_2->customer_id & 0xFFFFF000);
    }

    /*ctag cos*/
    if ((p_valid_1->ctag_cos_valid) && (p_valid_2->ctag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cos, p_node_2->ctag_cos);
    }

    /*stag cos*/
    if ((p_valid_1->stag_cos_valid) && (p_valid_2->stag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cos, p_node_2->stag_cos);
    }

    /*ctag cfi*/
    if ((p_valid_1->ctag_cfi_valid) && (p_valid_2->ctag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cfi, p_node_2->ctag_cfi);
    }

    /*stag cfi*/
    if ((p_valid_1->stag_cfi_valid) && (p_valid_2->stag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cfi, p_node_2->stag_cfi);
    }

    /*from sgmac*/
    if ((p_valid_1->sgmac_valid) && (p_valid_2->sgmac_valid))
    {
        CTC_INTEGER_CMP(p_node_1->from_sgmac, p_node_2->from_sgmac);
    }

    /*exception2*/
    if ((p_valid_1->exception2_valid) && (p_valid_2->exception2_valid))
    {
         CTC_INTEGER_CMP(p_node_1->exception2, p_node_2->exception2);
    }

    /*exception sub index*/
    if ((p_valid_1->exp_subindex_valid) && (p_valid_2->exp_subindex_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exp_subindex, p_node_2->exp_subindex);
    }

    return CTC_E_NONE;
}

static int32
db_usrid_mac_cmp_fun(void *data1, void *data2)
{
    int32 ret = 0;
    sys_usrid_valid_t *p_valid_1, *p_valid_2;
    sys_usrid_db_mackey_node_t *p_node_1, *p_node_2;

    p_valid_1 = &(((sys_usrid_db_mac_node_t *)data1)->valid);
    p_valid_2 = &(((sys_usrid_db_mac_node_t *)data2)->valid);

    p_node_1 = &(((sys_usrid_db_mac_node_t *)data1)->usrid_key_node);
    p_node_2 = &(((sys_usrid_db_mac_node_t *)data2)->usrid_key_node);

    if (0 != (ret = kal_memcmp(p_valid_1, p_valid_2, sizeof(sys_usrid_valid_t))))
    {
        return ret;
    }

    CTC_INTEGER_CMP(p_node_1->macsa_mask_h, p_node_2->macsa_mask_h);
    CTC_INTEGER_CMP(p_node_1->macsa_h, p_node_2->macsa_h);

    CTC_INTEGER_CMP(p_node_1->macsa_mask_l, p_node_2->macsa_mask_l);
    CTC_INTEGER_CMP(p_node_1->macsa_l, p_node_2->macsa_l);

    if ((p_valid_1->igs_cvid_valid) && (p_valid_2->igs_cvid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->cvid, p_node_2->cvid);
    }

    if ((p_valid_1->igs_svid_valid) && (p_valid_2->igs_svid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->svid, p_node_2->svid);
    }

    CTC_INTEGER_CMP(p_node_1->macda_mask_h, p_node_2->macda_mask_h);
    CTC_INTEGER_CMP(p_node_1->macda_h, p_node_2->macda_h);

    CTC_INTEGER_CMP(p_node_1->macda_mask_l, p_node_2->macda_mask_l);
    CTC_INTEGER_CMP(p_node_1->macda_l, p_node_2->macda_l);

    if ((p_valid_1->ctag_cos_valid) && (p_valid_2->ctag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cos, p_node_2->ctag_cos);
    }

    if ((p_valid_1->ctag_cfi_valid) && (p_valid_2->ctag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cfi, p_node_2->ctag_cfi);
    }

    if ((p_valid_1->stag_cos_valid) && (p_valid_2->stag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cos, p_node_2->stag_cos);
    }

    if ((p_valid_1->stag_cfi_valid) && (p_valid_2->stag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cfi, p_node_2->stag_cfi);
    }

    if ((p_valid_1->l2_type_valid) && (p_valid_2->l2_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l2_type, p_node_2->l2_type);
    }

    if ((p_valid_1->l3_type_valid) && (p_valid_2->l3_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l3_type, p_node_2->l3_type);
    }

    if ((p_valid_1->sgmac_valid) && (p_valid_2->sgmac_valid))
    {
        CTC_INTEGER_CMP(p_node_1->from_sgmac, p_node_2->from_sgmac);
    }

    if ((p_valid_1->exception2_valid) && (p_valid_2->exception2_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exception2, p_node_2->exception2);
    }

    if ((p_valid_1->exp_subindex_valid) && (p_valid_2->exp_subindex_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exp_sub_index, p_node_2->exp_sub_index);
    }

    return 0;
}

static int32
db_usrid_ipv4_cmp_fun(void *data1, void *data2)
{
    int32 ret = 0;
    sys_usrid_valid_t *p_valid_1, *p_valid_2;
    sys_usrid_db_ipv4key_node_t *p_node_1, *p_node_2;

    p_valid_1 = &(((sys_usrid_db_ipv4_node_t *)data1)->valid);
    p_valid_2 = &(((sys_usrid_db_ipv4_node_t *)data2)->valid);

    p_node_1 = &(((sys_usrid_db_ipv4_node_t *)data1)->usrid_key_node);
    p_node_2 = &(((sys_usrid_db_ipv4_node_t *)data2)->usrid_key_node);

    if (0 != (ret = kal_memcmp(p_valid_1, p_valid_2, sizeof(sys_usrid_valid_t))))
    {
        return ret;
    }

    CTC_INTEGER_CMP(p_node_1->ipv4_sa_mask, p_node_2->ipv4_sa_mask);
    CTC_INTEGER_CMP(p_node_1->ipv4_sa, p_node_2->ipv4_sa);
    CTC_INTEGER_CMP(p_node_1->ipv4_da_mask, p_node_2->ipv4_da_mask);
    CTC_INTEGER_CMP(p_node_1->ipv4_da, p_node_2->ipv4_da);

    CTC_INTEGER_CMP(p_node_1->macsa_mask_h, p_node_2->macsa_mask_h);
    CTC_INTEGER_CMP(p_node_1->macsa_h, p_node_2->macsa_h);

    CTC_INTEGER_CMP(p_node_1->macsa_mask_l, p_node_2->macsa_mask_l);
    CTC_INTEGER_CMP(p_node_1->macsa_l, p_node_2->macsa_l);

    CTC_INTEGER_CMP(p_node_1->macda_mask_h, p_node_2->macda_mask_h);
    CTC_INTEGER_CMP(p_node_1->macda_h, p_node_2->macda_h);

    CTC_INTEGER_CMP(p_node_1->macda_mask_l, p_node_2->macda_mask_l);
    CTC_INTEGER_CMP(p_node_1->macda_l, p_node_2->macda_l);

    if ((p_valid_1->igs_svid_valid) && (p_valid_2->igs_svid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->svid, p_node_2->svid);
    }

    if ((p_valid_1->igs_cvid_valid) && (p_valid_2->igs_cvid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->cvid, p_node_2->cvid);
    }

    if ((p_valid_1->l3_type_valid) && (p_valid_2->l3_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l3_type, p_node_2->l3_type);
    }

    if ((p_valid_1->l4_src_port_valid) && (p_valid_2->l4_src_port_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_src_port, p_node_2->l4_src_port);
    }

    if ((p_valid_1->l4_dest_port_valid) && (p_valid_2->l4_dest_port_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_dest_port, p_node_2->l4_dest_port);
    }

    if ((p_valid_1->dscp_valid) && (p_valid_2->dscp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->dscp, p_node_2->dscp);
    }

    if ((p_valid_1->ctag_cos_valid) && (p_valid_2->ctag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cos, p_node_2->ctag_cos);
    }

    if ((p_valid_1->ctag_cfi_valid) && (p_valid_2->ctag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cfi, p_node_2->ctag_cfi);
    }

    if ((p_valid_1->stag_cos_valid) && (p_valid_2->stag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cos, p_node_2->stag_cos);
    }

    if ((p_valid_1->stag_cfi_valid) && (p_valid_2->stag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cfi, p_node_2->stag_cfi);
    }

    if ((p_valid_1->frag_info_valid) && (p_valid_2->frag_info_valid))
    {
        CTC_INTEGER_CMP(p_node_1->frag_info, p_node_2->frag_info);
    }

    if ((p_valid_1->l4info_mapped_valid) && (p_valid_2->l4info_mapped_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_info_mapped, p_node_2->l4_info_mapped);
    }

    if ((p_valid_1->is_tcp_valid) && (p_valid_2->is_tcp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_tcp, p_node_2->is_tcp);
    }

    if ((p_valid_1->is_udp_valid) && (p_valid_2->is_udp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_udp, p_node_2->is_udp);
    }

    if ((p_valid_1->sgmac_valid) && (p_valid_2->sgmac_valid))
    {
        CTC_INTEGER_CMP(p_node_1->from_sgmac, p_node_2->from_sgmac);
    }

    if ((p_valid_1->ip_option_valid) && (p_valid_2->ip_option_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ip_option, p_node_2->ip_option);
    }

    if ((p_valid_1->exception2_valid) && (p_valid_2->exception2_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exception2_en, p_node_2->exception2_en);
    }

    if ((p_valid_1->exp_subindex_valid) && (p_valid_2->exp_subindex_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exp_sub_index, p_node_2->exp_sub_index);
    }

    if ((p_valid_1->ether_type_valid) && (p_valid_2->ether_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ether_type, p_node_2->ether_type);
    }

    if ((p_valid_1->routed_packet_valid) && (p_valid_2->routed_packet_valid))
    {
        CTC_INTEGER_CMP(p_node_1->routed_packet, p_node_2->routed_packet);
    }

    if ((p_valid_1->ip_hdr_error_valid) && (p_valid_2->ip_hdr_error_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ip_hdr_error, p_node_2->ip_hdr_error);
    }

    if ((p_valid_1->is_app_valid) && (p_valid_2->is_app_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_app, p_node_2->is_app);
    }


    return 0;
}
static int32
db_usrid_ipv6_cmp_fun(void *data1, void *data2)
{
    uint8 idx = 0;
    int32 ret = 0;
    sys_usrid_valid_t *p_valid_1, *p_valid_2;
    sys_usrid_db_ipv6key_node_t *p_node_1, *p_node_2;

    p_valid_1 = &(((sys_usrid_db_ipv6_node_t *)data1)->valid);
    p_valid_2 = &(((sys_usrid_db_ipv6_node_t *)data2)->valid);

    p_node_1 = &(((sys_usrid_db_ipv6_node_t *)data1)->usrid_key_node);
    p_node_2 = &(((sys_usrid_db_ipv6_node_t *)data2)->usrid_key_node);

    if (0 != (ret = kal_memcmp(p_valid_1, p_valid_2, sizeof(sys_usrid_valid_t))))
    {
        return ret;
    }

    for (idx = 0; idx < 4; idx++)
    {
        CTC_INTEGER_CMP(p_node_1->ipv6_smask[idx], p_node_2->ipv6_smask[idx]);
        CTC_INTEGER_CMP(p_node_1->ipv6_sa[idx], p_node_2->ipv6_sa[idx]);

        CTC_INTEGER_CMP(p_node_1->ipv6_dmask[idx], p_node_2->ipv6_dmask[idx]);
        CTC_INTEGER_CMP(p_node_1->ipv6_da[idx], p_node_2->ipv6_da[idx]);
    }


    CTC_INTEGER_CMP(p_node_1->macsa_mask_h, p_node_2->macsa_mask_h);
    CTC_INTEGER_CMP(p_node_1->macsa_h, p_node_2->macsa_h);
    CTC_INTEGER_CMP(p_node_1->macsa_mask_l, p_node_2->macsa_mask_l);
    CTC_INTEGER_CMP(p_node_1->macsa_l, p_node_2->macsa_l);

    CTC_INTEGER_CMP(p_node_1->macda_mask_h, p_node_2->macda_mask_h);
    CTC_INTEGER_CMP(p_node_1->macda_h, p_node_2->macda_h);
    CTC_INTEGER_CMP(p_node_1->macda_mask_l, p_node_2->macda_mask_l);
    CTC_INTEGER_CMP(p_node_1->macda_l, p_node_2->macda_l);


    if ((p_valid_1->igs_svid_valid) && (p_valid_2->igs_svid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->svid, p_node_2->svid);
    }

    if ((p_valid_1->igs_cvid_valid) && (p_valid_2->igs_cvid_valid))
    {
        CTC_INTEGER_CMP(p_node_1->cvid, p_node_2->cvid);
    }

    if ((p_valid_1->l3_type_valid) && (p_valid_2->l3_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l3_type, p_node_2->l3_type);
    }

    if ((p_valid_1->l4_src_port_valid) && (p_valid_2->l4_src_port_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_src_port, p_node_2->l4_src_port);
    }

    if ((p_valid_1->l4_dest_port_valid) && (p_valid_2->l4_dest_port_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_dest_port, p_node_2->l4_dest_port);
    }

    if ((p_valid_1->dscp_valid) && (p_valid_2->dscp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->dscp, p_node_2->dscp);
    }

    if ((p_valid_1->ctag_cos_valid) && (p_valid_2->ctag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cos, p_node_2->ctag_cos);
    }

    if ((p_valid_1->ctag_cfi_valid) && (p_valid_2->ctag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ctag_cfi, p_node_2->ctag_cfi);
    }

    if ((p_valid_1->stag_cos_valid) && (p_valid_2->stag_cos_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cos, p_node_2->stag_cos);
    }

    if ((p_valid_1->stag_cfi_valid) && (p_valid_2->stag_cfi_valid))
    {
        CTC_INTEGER_CMP(p_node_1->stag_cfi, p_node_2->stag_cfi);
    }

    if ((p_valid_1->frag_info_valid) && (p_valid_2->frag_info_valid))
    {
        CTC_INTEGER_CMP(p_node_1->frag_info, p_node_2->frag_info);
    }

    if ((p_valid_1->l4info_mapped_valid) && (p_valid_2->l4info_mapped_valid))
    {
        CTC_INTEGER_CMP(p_node_1->l4_info_map, p_node_2->l4_info_map);
    }

    if ((p_valid_1->is_tcp_valid) && (p_valid_2->is_tcp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_tcp, p_node_2->is_tcp);
    }

    if ((p_valid_1->is_udp_valid) && (p_valid_2->is_udp_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_udp, p_node_2->is_udp);
    }

    if ((p_valid_1->sgmac_valid) && (p_valid_2->sgmac_valid))
    {
        CTC_INTEGER_CMP(p_node_1->from_sgmac, p_node_2->from_sgmac);
    }

    if ((p_valid_1->ip_option_valid) && (p_valid_2->ip_option_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ip_option, p_node_2->ip_option);
    }

    if ((p_valid_1->exception2_valid) && (p_valid_2->exception2_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exception2, p_node_2->exception2);
    }

    if ((p_valid_1->exp_subindex_valid) && (p_valid_2->exp_subindex_valid))
    {
        CTC_INTEGER_CMP(p_node_1->exp_subindex, p_node_2->exp_subindex);
    }

    if ((p_valid_1->ether_type_valid) && (p_valid_2->ether_type_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ether_type, p_node_2->ether_type);
    }

    if ((p_valid_1->ipv6_ext_hdr_valid) && (p_valid_2->ipv6_ext_hdr_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ipv6_ext_hdr, p_node_2->ipv6_ext_hdr);
    }

    if ((p_valid_1->ipv6_flow_label_valid) && (p_valid_2->ipv6_flow_label_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ipv6_flow_label, p_node_2->ipv6_flow_label);
    }

    if ((p_valid_1->routed_packet_valid) && (p_valid_2->routed_packet_valid))
    {
        CTC_INTEGER_CMP(p_node_1->routed_packet, p_node_2->routed_packet);
    }

    if ((p_valid_1->ip_hdr_error_valid) && (p_valid_2->ip_hdr_error_valid))
    {
        CTC_INTEGER_CMP(p_node_1->ip_hdr_err, p_node_2->ip_hdr_err);
    }

    if ((p_valid_1->is_app_valid) && (p_valid_2->is_app_valid))
    {
        CTC_INTEGER_CMP(p_node_1->is_app, p_node_2->is_app);
    }

    return 0;
}

static int32
_sys_humber_usrid_db_create_tree(uint8 lchip, uint32 key_type, uint16 label_or_port)
{
    int32 (*p_fun)(void*, void*);
    uint32 table_id;
    uint32 entry_num;
    ctc_avl_tree_t **pp_tree = NULL;

    switch (key_type)
    {
        case SYS_USRID_VLAN_KEY:
            table_id = DS_USER_ID_VLAN_KEY;
            p_fun = db_usrid_vlan_cmp_fun;
            break;

        case SYS_USRID_MAC_KEY:
            table_id = DS_USER_ID_MAC_KEY;
            p_fun = db_usrid_mac_cmp_fun;
            break;

        case SYS_USRID_IPV4_KEY:
            table_id = DS_USER_ID_IPV4_KEY;
            p_fun = db_usrid_ipv4_cmp_fun;
            break;

        case SYS_USRID_IPV6_KEY:
            table_id = DS_USER_ID_IPV6_KEY;
            p_fun = db_usrid_ipv6_cmp_fun;
            break;

        default:
            return CTC_E_INVALID_USRID_KEY;
    }

    pp_tree = _sys_humber_usrid_db_get_tree(lchip, key_type, label_or_port);

    if (NULL == pp_tree)
    {
        return CTC_E_INVALID_USRID_KEY;
    }

    sys_alloc_get_table_entry_num(table_id, &entry_num);
    if (0 == entry_num)
    {
        return CTC_E_INVALID_USRID_KEY;
    }

    CTC_ERROR_RETURN(ctc_avl_create(pp_tree, 0, p_fun));

    return CTC_E_NONE;
}

/**

*/
int32
sys_humber_usrid_db_init(void)
{
    uint8 chip_id;
    uint8 lchip_num;

    SYS_USRID_DBG_FUNC();

    if (NULL != p_usrid_db)
    {
        SYS_USRID_DBG_INFO( "Init usrid db, usrid db isn't NULL!\n");
        return CTC_E_NONE;
    }

    lchip_num = sys_humber_get_local_chip_num();

    p_usrid_db = mem_malloc(MEM_USRID_MODULE, sizeof(sys_usrid_db_t) * lchip_num);
    if (NULL == p_usrid_db)
    {
        SYS_USRID_DBG_INFO("Malloc usrid db fail, init usrid db fail!\n");
        return CTC_E_NO_MEMORY;
    }

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        kal_memset(p_usrid_db[chip_id].vlankey_default_offset, 0xFF,
        sizeof(uint16)*(MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM + 1));

        kal_memset(p_usrid_db[chip_id].mackey_default_offset, 0xFF, sizeof(uint16) * SYS_USRID_MAX_LABEL);
        kal_memset(p_usrid_db[chip_id].ipv4key_default_offset, 0xFF, sizeof(uint16) * SYS_USRID_MAX_LABEL);
        kal_memset(p_usrid_db[chip_id].ipv6key_default_offset, 0xFF, sizeof(uint16) * SYS_USRID_MAX_LABEL);
    }

    return CTC_E_NONE;
}

/**

*/
int32
sys_humber_usrid_db_vlan_init(void)
{
    uint8 lchip;
    uint8 lchip_num;
    uint16 lport;

    SYS_USRID_DBG_FUNC();
    /*init the offset tree*/
    CTC_ERROR_RETURN(_sys_humber_usrid_db_offset_init(SYS_USRID_VLAN_KEY));

    /*init node db tree*/
    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_usrid_db[lchip].vlan_tree
        = mem_malloc(MEM_USRID_MODULE, sizeof(ctc_avl_tree_t*) * (MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM + 1));
        if (NULL == p_usrid_db[lchip].vlan_tree)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_usrid_db[lchip].vlan_tree, 0, sizeof(ctc_avl_tree_t *)* (MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM + 1));

        for(lport = 0; lport < (MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM + 1); lport++)
        {
            if (NULL == p_usrid_db[lchip].vlan_tree[lport])
            {
                CTC_ERROR_RETURN(_sys_humber_usrid_db_create_tree(lchip, SYS_USRID_VLAN_KEY, lport));
            }
            else
            {
                SYS_USRID_DBG_INFO("usrid db vlan tree isn't NULL, initial fail, chip =%d, lport =%d\n",
                                    lchip, lport);
            }
        }
    }

    return CTC_E_NONE;
}

/**

*/
int32
sys_humber_usrid_db_mac_init(void)
{
    uint8 lchip;
    uint8 lchip_num;
    uint8 label;

    SYS_USRID_DBG_FUNC();
    /*init the offset tree*/
    CTC_ERROR_RETURN(_sys_humber_usrid_db_offset_init(SYS_USRID_MAC_KEY));

    /*init node db tree*/
    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_usrid_db[lchip].mac_tree
        = mem_malloc(MEM_USRID_MODULE, sizeof(ctc_avl_tree_t*) * SYS_USRID_MAX_LABEL);
        if (NULL == p_usrid_db[lchip].mac_tree)
        {
            return CTC_E_NO_MEMORY;
        }
        kal_memset(p_usrid_db[lchip].mac_tree, 0, sizeof(ctc_avl_tree_t *)* SYS_USRID_MAX_LABEL);

        for (label = 0; label < SYS_USRID_MAX_LABEL; label++)
        {
            if (NULL == p_usrid_db[lchip].mac_tree[label])
            {
                CTC_ERROR_RETURN(_sys_humber_usrid_db_create_tree(lchip, SYS_USRID_MAC_KEY, label));
            }
            else
            {
                SYS_USRID_DBG_INFO("usrid db mac tree isn't NULL, initial fail!\n");
            }
        }
    }

    return CTC_E_NONE;
}

/**

*/
int32
sys_humber_usrid_db_ipv4_init(void)
{
    uint8 lchip;
    uint8 lchip_num;
    uint8 label;

    SYS_USRID_DBG_FUNC();
    /*init offset tree*/
    CTC_ERROR_RETURN(_sys_humber_usrid_db_offset_init(SYS_USRID_IPV4_KEY));

    /*init node db tree*/
    lchip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_usrid_db[lchip].ipv4_tree
        = mem_malloc(MEM_USRID_MODULE, sizeof(ctc_avl_tree_t*) * SYS_USRID_MAX_LABEL);

        kal_memset(p_usrid_db[lchip].ipv4_tree, 0, sizeof(ctc_avl_tree_t *)* SYS_USRID_MAX_LABEL);

        for (label = 0; label < SYS_USRID_MAX_LABEL; label++)
        {
            if (NULL == p_usrid_db[lchip].ipv4_tree[label])
            {
                CTC_ERROR_RETURN(_sys_humber_usrid_db_create_tree(lchip, SYS_USRID_IPV4_KEY, label));
            }
            else
            {
                SYS_USRID_DBG_INFO("usrid db ipv4 tree isn't NULL, initial fail!\n");
            }
        }
    }

    return CTC_E_NONE;
}

/**

*/
int32
sys_humber_usrid_db_ipv6_init(void)
{
    uint8 lchip;
    uint8 lchip_num;
    uint8 label;

    SYS_USRID_DBG_FUNC();
    /*init offset tree*/
    CTC_ERROR_RETURN(_sys_humber_usrid_db_offset_init(SYS_USRID_IPV6_KEY));

    /*init node db tree*/
    lchip_num = sys_humber_get_local_chip_num();

    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        p_usrid_db[lchip].ipv6_tree
        = mem_malloc(MEM_USRID_MODULE, sizeof(ctc_avl_tree_t*) * SYS_USRID_MAX_LABEL);

        kal_memset(p_usrid_db[lchip].ipv6_tree, 0, sizeof(ctc_avl_tree_t *)* SYS_USRID_MAX_LABEL);

        for (label = 0; label < SYS_USRID_MAX_LABEL; label++)
        {
            if (NULL == p_usrid_db[lchip].ipv6_tree[label])
            {
                CTC_ERROR_RETURN(_sys_humber_usrid_db_create_tree(lchip, SYS_USRID_IPV6_KEY, label));
            }
            else
            {
                SYS_USRID_DBG_INFO("usrid vlan db tree isn't NULL, success!\n");
            }
        }
    }

    return CTC_E_NONE;
}

/**
 @brief The function is to alloc one offset in tcam
*/

int32
sys_humber_usrid_db_alloc_offset(uint8 lchip, uint32 key_type, uint32 *p_key_offset)
{
    sys_humber_opf_t opf;

    CTC_PTR_VALID_CHECK(p_key_offset);

    opf.pool_type = key_type;
    opf.pool_index = lchip;

    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, 1, p_key_offset));

    return CTC_E_NONE;
}

/**
 @brief The funtion is to free the offset
*/
int32
sys_humber_usrid_db_free_offset(uint8 lchip, uint32 key_type, uint32 key_offset)
{
    sys_humber_opf_t opf;

    opf.pool_index = lchip;
    opf.pool_type = key_type;

    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, key_offset));

    return CTC_E_NONE;
}

int32
sys_humber_usrid_db_alloc_resv_offset(uint8 lchip, uint16 label_or_gport, uint32 key_type, uint32 *p_key_offset)
{
    uint16 index = 0;
    uint16 *p_offset = 0;
    sys_humber_opf_t opf;

    switch (key_type)
    {
        case OPF_USRID_VLAN_KEY:
            if (CTC_IS_LINKAGG_PORT(label_or_gport))
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport) + MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport);
            }
            p_offset = &(p_usrid_db[lchip].vlankey_default_offset[index]);

            break;

        case OPF_USRID_MAC_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].mackey_default_offset[index]);

            break;

        case OPF_USRID_IPV4_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].ipv4key_default_offset[index]);

            break;

        case OPF_USRID_IPV6_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].ipv6key_default_offset[index]);

            break;

        default:
            return CTC_E_INVALID_PARAM;
    }


    if (CTC_MAX_UINT16_VALUE != *p_offset)
    {
        *p_key_offset = *p_offset;

        return CTC_E_ENTRY_EXIST;
    }

    opf.pool_type = key_type;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_reverse_alloc_offset(&opf, 1, p_key_offset));

    *p_offset = *p_key_offset;

    return CTC_E_NONE;
}

int32
sys_humber_usrid_db_free_resv_offset(uint8 lchip, uint16 label_or_gport, uint32 key_type, uint32 *p_key_offset)
{
    uint16 index = 0;
    uint16 *p_offset = NULL;
    sys_humber_opf_t opf;

    switch (key_type)
    {
        case OPF_USRID_VLAN_KEY:
            if(CTC_IS_LINKAGG_PORT(label_or_gport))
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport) + MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport);
            }
            p_offset = &(p_usrid_db[lchip].vlankey_default_offset[index]);

            break;

        case OPF_USRID_MAC_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].mackey_default_offset[index]);

            break;
        case OPF_USRID_IPV4_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].ipv4key_default_offset[index]);

            break;

         case OPF_USRID_IPV6_KEY:
            index = label_or_gport;
            p_offset = &(p_usrid_db[lchip].ipv6key_default_offset[index]);

            break;

         default:
            return CTC_E_INVALID_PARAM;
    }

    if (CTC_MAX_UINT16_VALUE == *p_offset)
    {
        *p_key_offset = CTC_MAX_UINT16_VALUE;
        return CTC_E_ENTRY_NOT_EXIST;
    }

    opf.pool_type = key_type;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, 1, *p_offset));

    *p_key_offset = *p_offset;
    *p_offset = CTC_MAX_UINT16_VALUE;

    return CTC_E_NONE;
}

int32
sys_humber_usrid_db_get_resv_offset(uint8 chip_id, uint16 gport_or_label, uint32 key_type, uint32 *p_key_offset)
{
    uint16 index = 0;
    uint16 *p_offset = NULL;

    switch (key_type)
    {
        case OPF_USRID_VLAN_KEY:
            if(CTC_IS_LINKAGG_PORT(gport_or_label))
            {
                index = CTC_MAP_GPORT_TO_LPORT(gport_or_label)+ MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(gport_or_label);
            }
            p_offset = &(p_usrid_db[chip_id].vlankey_default_offset[index]);

            break;

        case OPF_USRID_MAC_KEY:
            index = gport_or_label;
            p_offset = &(p_usrid_db[chip_id].mackey_default_offset[index]);

            break;
        case OPF_USRID_IPV4_KEY:
            index = gport_or_label;
            p_offset = &(p_usrid_db[chip_id].ipv4key_default_offset[index]);

            break;

         case OPF_USRID_IPV6_KEY:
            index = gport_or_label;
            p_offset = &(p_usrid_db[chip_id].ipv6key_default_offset[index]);

            break;

         default:
            return CTC_E_INVALID_PARAM;
    }

    if (CTC_MAX_UINT16_VALUE == *p_offset)
    {
        *p_key_offset = CTC_MAX_UINT16_VALUE;
        return CTC_E_ENTRY_NOT_EXIST;
    }

    *p_key_offset = *p_offset;

    return CTC_E_NONE;
}

int32
sys_humber_usrid_db_insert_node(uint8 lchip, uint16 label_or_gport, uint32 key_type, void *p_node)
{
    uint16 index = 0;
    uint32 key_offset;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;

    CTC_PTR_VALID_CHECK(p_node);

    switch (key_type)
    {
        case SYS_USRID_VLAN_KEY:
            if (CTC_MAX_UINT16_VALUE == label_or_gport)
            {
                index = MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM;
            }
            else if (CTC_IS_LINKAGG_PORT(label_or_gport))
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport)+ MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport);
            }

            break;

        case SYS_USRID_MAC_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV4_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV6_KEY:
            index = label_or_gport;

            break;

        default:
            return CTC_E_INVALID_USRID_KEY;
    }


    pp_avl_tree =  _sys_humber_usrid_db_get_tree(lchip, key_type, index);
    if (NULL == pp_avl_tree)
    {
        return CTC_E_INVALID_USRID_KEY;
    }

    p_avl_node = ctc_avl_search(*pp_avl_tree, p_node);
    if (NULL != p_avl_node)
    {
        /* If existed, then update */
        *(uint32 *)((uint32)p_node + sizeof(sys_usrid_valid_t)) = *(uint32 *)((uint32)(p_avl_node->info) + sizeof(sys_usrid_valid_t));
        return CTC_E_NONE;
    }

    CTC_ERROR_RETURN(sys_humber_usrid_db_alloc_offset(lchip, key_type, &key_offset));
    *(uint32 *)((uint32)p_node + sizeof(sys_usrid_valid_t)) = key_offset;

    if (0 != ctc_avl_insert(*pp_avl_tree, p_node))
    {
        sys_humber_usrid_db_free_offset(lchip, key_type, key_offset);
        return CTC_E_UNEXPECT;
    }

    return CTC_E_NONE;
}

void *
sys_humber_usrid_db_delete_node(uint8 lchip, uint16 label_or_gport, uint32 key_type, void *p_node)
{
    uint16 index = 0;
    uint32 key_offset;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;
    void *pData = NULL;

	if(!p_node)
	{
	 	return NULL;
	}

    switch (key_type)
    {
        case SYS_USRID_VLAN_KEY:
            if (CTC_MAX_UINT16_VALUE == label_or_gport)
            {
                index = MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM;
            }
            else if (CTC_IS_LINKAGG_PORT(label_or_gport))
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport)+ MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport);
            }

            break;

        case SYS_USRID_MAC_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV4_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV6_KEY:
            index = label_or_gport;

            break;

        default:
            return NULL;
    }

    pp_avl_tree = _sys_humber_usrid_db_get_tree(lchip, key_type, index);
    if (NULL == pp_avl_tree)
    {
        return NULL;
    }

    p_avl_node = ctc_avl_search(*pp_avl_tree, p_node);
    if (NULL == p_avl_node)
    {
        return NULL;
    }
	pData = p_avl_node->info;

    key_offset = *(uint32 *)((uint32)(p_avl_node->info) + sizeof(sys_usrid_valid_t));

    if (CTC_E_NONE != sys_humber_usrid_db_free_offset(lchip, key_type , key_offset))
    {
        return NULL;
    }

    if (CTC_E_NONE != ctc_avl_delete_node(*pp_avl_tree, p_avl_node))
    {
        return NULL;
    }

    return pData;
}

void *
sys_humber_usrid_db_find_node(uint8 lchip, uint16 label_or_gport, uint32 key_type, void *p_node)
{
    uint16 index = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;

    if(!p_node)
	{
	 	return NULL;
	}

    switch (key_type)
    {
        case SYS_USRID_VLAN_KEY:
            if (CTC_MAX_UINT16_VALUE == label_or_gport)
            {
                index = MAX_PORT_NUM_PER_CHIP + CTC_MAX_LINKAGG_GROUP_NUM;
            }
            else if (CTC_IS_LINKAGG_PORT(label_or_gport))
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport)+ MAX_PORT_NUM_PER_CHIP;
            }
            else
            {
                index = CTC_MAP_GPORT_TO_LPORT(label_or_gport);
            }

            break;

        case SYS_USRID_MAC_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV4_KEY:
            index = label_or_gport;

            break;

        case SYS_USRID_IPV6_KEY:
            index = label_or_gport;

            break;

        default:
            return NULL;
    }

    pp_avl_tree = _sys_humber_usrid_db_get_tree(lchip, key_type, index);
    if (NULL == pp_avl_tree)
    {
        return NULL;
    }

    p_avl_node = ctc_avl_search(*pp_avl_tree, p_node);
    if (NULL == p_avl_node)
    {
        return NULL;
    }
    return p_avl_node->info;


}

int32
sys_humber_usrid_db_remove_node_by_label_port(uint8 lchip, uint16 index, uint32 key_type, uint32 *key_offset)
{
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;

    pp_avl_tree = _sys_humber_usrid_db_get_tree(lchip, key_type, index);
    if (NULL == pp_avl_tree)
    {
        return CTC_E_INVALID_USRID_KEY;
    }

    p_avl_node = ctc_avl_top(*pp_avl_tree);

    if (NULL == p_avl_node)
    {
        return CTC_E_ENTRY_NOT_EXIST;
    }

    *key_offset = *(uint32*)((uint32)(p_avl_node->info) + sizeof(sys_usrid_valid_t));

    CTC_ERROR_RETURN(sys_humber_usrid_db_free_offset(lchip, key_type, *key_offset));

    CTC_ERROR_RETURN(ctc_avl_delete_node(*pp_avl_tree, p_avl_node));

    if (p_avl_node->info)
    {
        mem_free(p_avl_node->info);
    }

    return CTC_E_NONE;
}

void static
_sys_humber_usrid_db_show_vlan_node(sys_usrid_db_vlan_node_t *p_vlan_node)
{
    if (NULL == p_vlan_node)
    {
        return;
    }

    SYS_USRID_DBG_INFO("Key offset                          :%d\n", p_vlan_node->key_offset);

    if (p_vlan_node->valid.igs_cvid_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- cvid                 :0x%x\n",
                            p_vlan_node->usrid_key_node.igs_cvid);
    }

    if (p_vlan_node->valid.igs_svid_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- svid                 :0x%x\n",
                            p_vlan_node->usrid_key_node.igs_svid);
    }

    if (p_vlan_node->valid.vc_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- vc label             :0x%x\n",
                            p_vlan_node->usrid_key_node.customer_id >> 12);
    }

    if (p_vlan_node->valid.gre_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- gre key              :0x%x\n",
                            p_vlan_node->usrid_key_node.customer_id);
    }

    if (p_vlan_node->valid.isid_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- gre key              :0x%x\n",
                            p_vlan_node->usrid_key_node.customer_id >> 8);
    }

    if (p_vlan_node->valid.stag_cos_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- stag cos             :%d\n",
                            p_vlan_node->usrid_key_node.stag_cos);
    }

    if (p_vlan_node->valid.stag_cfi_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- stag cfi             :%d\n",
                            p_vlan_node->usrid_key_node.stag_cfi);
    }

    if (p_vlan_node->valid.ctag_cos_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- ctag cos             :%d\n",
                            p_vlan_node->usrid_key_node.ctag_cos);
    }

    if (p_vlan_node->valid.ctag_cfi_valid)
    {
        SYS_USRID_DBG_INFO("Key info    -- ctag cfi             :%d\n",
                            p_vlan_node->usrid_key_node.ctag_cfi);
    }

    SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n",
                        p_vlan_node->ds_node_usrid.by_pass_all);
    SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n",
                        p_vlan_node->ds_node_usrid.usr_vlan_ptr);

    if (p_vlan_node->ds_node_usrid.binding_en)
    {
        SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n",
                        p_vlan_node->ds_node_usrid.binding_en);
        if (p_vlan_node->ds_node_usrid.binding_macsa)
        {
            SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                p_vlan_node->ds_node_usrid.binding_data_h,
                                p_vlan_node->ds_node_usrid.binding_data_m,
                                p_vlan_node->ds_node_usrid.binding_data_l);
        }
        else
        {
            SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                p_vlan_node->ds_node_usrid.binding_data_h,
                                p_vlan_node->ds_node_usrid.binding_data_m,
                                p_vlan_node->ds_node_usrid.binding_data_l);
        }

    }
    else if (p_vlan_node->ds_node_usrid.fwd_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n",
                            p_vlan_node->ds_node_usrid.binding_data_l);
    }
    else if (p_vlan_node->ds_node_usrid.stats_ptr_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n",
                            p_vlan_node->ds_node_usrid.binding_data_l);
    }
    else if (0xFFFF != p_vlan_node->ds_node_usrid.binding_data_l)
    {
        SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n",
                            p_vlan_node->ds_node_usrid.binding_data_l);
    }

    /*binding data high*/
    if (CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_h , 0x8000))
    {
        SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
    }

    if (CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_h, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
        SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                         (p_vlan_node->ds_node_usrid.binding_data_h & 0x1FFF));
    }
    else if (!CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_h, 0x1000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                    (p_vlan_node->ds_node_usrid.binding_data_h & 0xFFF));

    }
    else if ((p_vlan_node->ds_node_usrid.aps_select_valid)
    && (0xFFF !=(p_vlan_node->ds_node_usrid.binding_data_h & 0xFFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
        SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                            (p_vlan_node->ds_node_usrid.binding_data_h & 0xFFF));
    }

    if (p_vlan_node->ds_node_usrid.aps_select_valid)
    {
        SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
        SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                            p_vlan_node->ds_node_usrid.binding_data_h >> 13 & 0x1);
    }

    /*binding data middle*/
    if (CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_m , 0xc000))
    {
        SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
    }

    if (CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_m, 0x8000)
    && (0x3FFF != (p_vlan_node->ds_node_usrid.binding_data_m & 0x3FFF)))
    {
        SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
        SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                            (p_vlan_node->ds_node_usrid.binding_data_m & 0x3FFF));

    }
    else if (CTC_FLAG_ISSET(p_vlan_node->ds_node_usrid.binding_data_m, 0x4000))
    {
        SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
        SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                            (p_vlan_node->ds_node_usrid.binding_data_m & 0xFFF));
    }
    else if (0x3FFF != (p_vlan_node->ds_node_usrid.binding_data_m & 0x3FFF))
    {
        SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                             (p_vlan_node->ds_node_usrid.binding_data_m & 0x3FFF));

    }

    SYS_USRID_DBG_INFO("================\n");
    return;
}

void
sys_humber_usrid_db_show_vlan_by_port(uint8 chip_id, uint16 gport)
{
    uint32 index = 0;
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;
    sys_usrid_db_vlan_node_t *p_vlan_node = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        index = CTC_MAP_GPORT_TO_LPORT(gport) + MAX_PORT_NUM_PER_CHIP;
        SYS_USRID_DBG_INFO("Show usrid info on linkagg 0x%x, ", gport);

        pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_VLAN_KEY, index);
        count = CTC_AVL_COUNT(*pp_avl_tree);

        SYS_USRID_DBG_INFO("of chip:%d, total count:%d\n", chip_id, count);
        SYS_USRID_DBG_INFO("========================================\n");
        p_avl_node = ctc_avl_top(*pp_avl_tree);
        if (NULL == p_avl_node)
        {
            return;
        }

        do
        {
            p_vlan_node = (sys_usrid_db_vlan_node_t *)p_avl_node->info;
            _sys_humber_usrid_db_show_vlan_node(p_vlan_node);

        }while(NULL != (p_avl_node = ctc_avl_next(p_avl_node)));

    }
    else
    {
        index = CTC_MAP_GPORT_TO_LPORT(gport);
        SYS_USRID_DBG_INFO("Show usrid info on port 0x%x, ", gport);

        pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_VLAN_KEY, index);
        count = CTC_AVL_COUNT(*pp_avl_tree);

        SYS_USRID_DBG_INFO("of chip:%d, total count:%d\n", chip_id, count);
        SYS_USRID_DBG_INFO("========================================\n");

        p_avl_node = ctc_avl_top(*pp_avl_tree);
        if (NULL == p_avl_node)
        {
            return;
        }

        do
        {
            p_vlan_node = (sys_usrid_db_vlan_node_t *)p_avl_node->info;
            _sys_humber_usrid_db_show_vlan_node(p_vlan_node);

        }while(NULL != (p_avl_node = ctc_avl_next(p_avl_node)));
    }
    return;
}


void
sys_humber_usrid_db_show_mac_by_label(uint8 chip_id, uint8 label_id)
{
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;
    sys_usrid_db_mac_node_t *p_mac_node = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_MAC_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);
    SYS_USRID_DBG_INFO("Show usrid mac info on chip:%d, label:%d, total count%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");

    p_avl_node = ctc_avl_top(*pp_avl_tree);
    if (NULL == p_avl_node)
    {
        return;
    }

    do
    {
        p_mac_node = (sys_usrid_db_mac_node_t *)p_avl_node->info;

        SYS_USRID_DBG_INFO("Key offset                          :%d\n", p_mac_node->key_offset);

        if (p_mac_node->valid.igs_cvid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- cvid                 :0x%x\n", p_mac_node->usrid_key_node.cvid);
        }

        if (p_mac_node->valid.igs_svid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- svid                 :0x%x\n", p_mac_node->usrid_key_node.svid);
        }

        if ((0 != p_mac_node->usrid_key_node.macsa_mask_h) || (0 != p_mac_node->usrid_key_node.macsa_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macsa mask           :0x%x%x\n",
                                p_mac_node->usrid_key_node.macsa_mask_h, p_mac_node->usrid_key_node.macsa_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macsa                :0x%x%x\n",
                                p_mac_node->usrid_key_node.macsa_h, p_mac_node->usrid_key_node.macsa_l);
        }

        if ((0 != p_mac_node->usrid_key_node.macda_mask_h) || (0 != p_mac_node->usrid_key_node.macda_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macda mask           :0x%x%x\n",
                                p_mac_node->usrid_key_node.macda_mask_h, p_mac_node->usrid_key_node.macda_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macda                :0x%x%x\n",
                                p_mac_node->usrid_key_node.macda_h, p_mac_node->usrid_key_node.macda_l);
        }

        if (p_mac_node->valid.stag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cos             :%d\n", p_mac_node->usrid_key_node.stag_cos);
        }

        if (p_mac_node->valid.stag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cfi             :%d\n", p_mac_node->usrid_key_node.stag_cfi);
        }

        if (p_mac_node->valid.ctag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cos             :%d\n", p_mac_node->usrid_key_node.ctag_cos);
        }

        if (p_mac_node->valid.ctag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cfi             :%d\n", p_mac_node->usrid_key_node.ctag_cfi);
        }

        if (p_mac_node->valid.l2_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer2 type          :%d\n", p_mac_node->usrid_key_node.l2_type);
        }

        if (p_mac_node->valid.l3_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer3 type          :%d\n", p_mac_node->usrid_key_node.l3_type);
        }

        SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", p_mac_node->ds_node_usrid.by_pass_all);
        SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", p_mac_node->ds_node_usrid.usr_vlan_ptr);

        if (p_mac_node->ds_node_usrid.binding_en)
        {
            SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", p_mac_node->ds_node_usrid.binding_en);
            if (p_mac_node->ds_node_usrid.binding_macsa)
            {
                SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                    p_mac_node->ds_node_usrid.binding_data_h,
                                    p_mac_node->ds_node_usrid.binding_data_m,
                                    p_mac_node->ds_node_usrid.binding_data_l);
            }
            else
            {
                SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                    p_mac_node->ds_node_usrid.binding_data_h,
                                    p_mac_node->ds_node_usrid.binding_data_m,
                                    p_mac_node->ds_node_usrid.binding_data_l);
            }

        }
        else if (p_mac_node->ds_node_usrid.fwd_ptr_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", p_mac_node->ds_node_usrid.binding_data_l);
        }
        else if (p_mac_node->ds_node_usrid.stats_ptr_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", p_mac_node->ds_node_usrid.binding_data_l);
        }
        else if (0xFFFF != p_mac_node->ds_node_usrid.binding_data_l)
        {
            SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", p_mac_node->ds_node_usrid.binding_data_l);
        }

        /*binding data high*/
        if (CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_h , 0x8000))
        {
            SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
        }

        if (CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_h, 0x4000))
        {
            SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
            SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                             (p_mac_node->ds_node_usrid.binding_data_h & 0x1FFF));
        }
        else if (!CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_h, 0x1000))
        {
            SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                        (p_mac_node->ds_node_usrid.binding_data_h & 0xFFF));

        }
        else if ((p_mac_node->ds_node_usrid.aps_select_valid)
        && (0xFFF !=(p_mac_node->ds_node_usrid.binding_data_h & 0xFFF)))
        {
            SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
            SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                                (p_mac_node->ds_node_usrid.binding_data_h & 0xFFF));
        }

        if (p_mac_node->ds_node_usrid.aps_select_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
            SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                                p_mac_node->ds_node_usrid.binding_data_h >> 13 & 0x1);
        }

        /*binding data middle*/
        if (CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_m , 0xc000))
        {
            SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
        }

        if (CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_m, 0x8000)
        && (0x3FFF != (p_mac_node->ds_node_usrid.binding_data_m & 0x3FFF)))
        {
            SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
            SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                                (p_mac_node->ds_node_usrid.binding_data_m & 0x3FFF));

        }
        else if (CTC_FLAG_ISSET(p_mac_node->ds_node_usrid.binding_data_m, 0x4000))
        {
            SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
            SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                                (p_mac_node->ds_node_usrid.binding_data_m & 0xFFF));
        }
        else if (0x3FFF != (p_mac_node->ds_node_usrid.binding_data_m & 0x3FFF))
        {
            SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                                 (p_mac_node->ds_node_usrid.binding_data_m & 0x3FFF));

        }

        SYS_USRID_DBG_INFO("================\n");
    }while(NULL != (p_avl_node = ctc_avl_next(p_avl_node)));

    return;
}

void
sys_humber_usrid_db_show_ipv4_by_label(uint8 chip_id, uint8 label_id)
{
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;
    sys_usrid_db_ipv4_node_t *p_ipv4_node = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_IPV4_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);

    SYS_USRID_DBG_INFO("Show usrid ipv4 info on chip:%d label:%d, total count:%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");
    p_avl_node = ctc_avl_top(*pp_avl_tree);
    if (NULL == p_avl_node)
    {
        return;
    }

    do
    {
        p_ipv4_node = (sys_usrid_db_ipv4_node_t *)p_avl_node->info;

        SYS_USRID_DBG_INFO("Key offset                          :%d\n", p_ipv4_node->key_offset);

        SYS_USRID_DBG_INFO("Key info    -- ipsa             :%d.%d.%d.%d\n",
                        (p_ipv4_node->usrid_key_node.ipv4_sa >> 24) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_sa >> 16) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_sa >> 8) & 0xFF,
                        p_ipv4_node->usrid_key_node.ipv4_sa & 0xFF);

        SYS_USRID_DBG_INFO("Key info    -- smask            :%d.%d.%d.%d\n",
                        (p_ipv4_node->usrid_key_node.ipv4_sa_mask >> 24) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_sa_mask >> 16) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_sa_mask >> 8) & 0xFF,
                        p_ipv4_node->usrid_key_node.ipv4_sa_mask & 0xFF);

        SYS_USRID_DBG_INFO("Key info    -- ipda             :%d.%d.%d.%d\n",
                        (p_ipv4_node->usrid_key_node.ipv4_da >> 24) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_da >> 16) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_da >> 8) & 0xFF,
                        p_ipv4_node->usrid_key_node.ipv4_da & 0xFF);

        SYS_USRID_DBG_INFO("Key info    -- damask           :%d.%d.%d.%d\n",
                        (p_ipv4_node->usrid_key_node.ipv4_da_mask >> 24) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_da_mask >> 16) & 0xFF,
                        (p_ipv4_node->usrid_key_node.ipv4_da_mask >> 8) & 0xFF,
                        p_ipv4_node->usrid_key_node.ipv4_da_mask & 0xFF);

        if (p_ipv4_node->valid.l4_src_port_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- l4 srcport           :0x%x\n", p_ipv4_node->usrid_key_node.l4_src_port);
        }

        if (p_ipv4_node->valid.l4_dest_port_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- l4 destport           :0x%x\n", p_ipv4_node->usrid_key_node.l4_dest_port);
        }

        if (p_ipv4_node->valid.igs_cvid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- cvid                 :0x%x\n", p_ipv4_node->usrid_key_node.cvid);
        }

        if (p_ipv4_node->valid.igs_svid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- svid                 :0x%x\n", p_ipv4_node->usrid_key_node.svid);
        }

        if ((0 != p_ipv4_node->usrid_key_node.macsa_mask_h) || (0 != p_ipv4_node->usrid_key_node.macsa_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macsa mask           :0x%x%x\n",
                                p_ipv4_node->usrid_key_node.macsa_mask_h, p_ipv4_node->usrid_key_node.macsa_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macsa                :0x%x%x\n",
                                p_ipv4_node->usrid_key_node.macsa_h, p_ipv4_node->usrid_key_node.macsa_l);
        }

        if ((0 != p_ipv4_node->usrid_key_node.macda_mask_h) || (0 != p_ipv4_node->usrid_key_node.macda_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macda mask           :0x%x%x\n",
                                p_ipv4_node->usrid_key_node.macda_mask_h, p_ipv4_node->usrid_key_node.macda_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macda                :0x%x%x\n",
                                p_ipv4_node->usrid_key_node.macda_h, p_ipv4_node->usrid_key_node.macda_l);
        }

        if (p_ipv4_node->valid.stag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cos             :%d\n", p_ipv4_node->usrid_key_node.stag_cos);
        }

        if (p_ipv4_node->valid.stag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cfi             :%d\n", p_ipv4_node->usrid_key_node.stag_cfi);
        }

        if (p_ipv4_node->valid.ctag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cos             :%d\n", p_ipv4_node->usrid_key_node.ctag_cos);
        }

        if (p_ipv4_node->valid.ctag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cfi             :%d\n", p_ipv4_node->usrid_key_node.ctag_cfi);
        }

        if (p_ipv4_node->valid.l2_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer2 type          :%d\n", p_ipv4_node->usrid_key_node.l2_type);
        }

        if (p_ipv4_node->valid.l3_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer3 type          :%d\n", p_ipv4_node->usrid_key_node.l3_type);
        }

        if (p_ipv4_node->valid.dscp_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- dscp                 :%d\n", p_ipv4_node->usrid_key_node.dscp);
        }

        if (p_ipv4_node->valid.frag_info_valid)
        {
            SYS_USRID_DBG_INFO("Key info    --fragement             :%d\n", p_ipv4_node->usrid_key_node.frag_info);
        }

        if (p_ipv4_node->valid.ip_option_valid)
        {
            SYS_USRID_DBG_INFO("key info    -- ip option            :%d\n", p_ipv4_node->usrid_key_node.ip_option);
        }

        SYS_USRID_DBG_INFO("Action info -- by pass all          :%d\n", p_ipv4_node->ds_node_usrid.by_pass_all);
        SYS_USRID_DBG_INFO("Action info -- usr vlan ptr         :0x%x\n", p_ipv4_node->ds_node_usrid.usr_vlan_ptr);

        if (p_ipv4_node->ds_node_usrid.binding_en)
        {
            SYS_USRID_DBG_INFO("Action info -- binding enable       :%d\n", p_ipv4_node->ds_node_usrid.binding_en);
            if (p_ipv4_node->ds_node_usrid.binding_macsa)
            {
                SYS_USRID_DBG_INFO("Action info -- binding macsa        :0x%x%x%x\n",
                                    p_ipv4_node->ds_node_usrid.binding_data_h,
                                    p_ipv4_node->ds_node_usrid.binding_data_m,
                                    p_ipv4_node->ds_node_usrid.binding_data_l);
            }
            else
            {
                SYS_USRID_DBG_INFO("Action info -- binding src port     :0x%x%x%x\n",
                                    p_ipv4_node->ds_node_usrid.binding_data_h,
                                    p_ipv4_node->ds_node_usrid.binding_data_m,
                                    p_ipv4_node->ds_node_usrid.binding_data_l);
            }

        }
        else if (p_ipv4_node->ds_node_usrid.fwd_ptr_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- fwd ptr              :0x%x\n", p_ipv4_node->ds_node_usrid.binding_data_l);
        }
        else if (p_ipv4_node->ds_node_usrid.stats_ptr_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- stats ptr            :0x%x\n", p_ipv4_node->ds_node_usrid.binding_data_l);
        }
        else if (0xFFFF != p_ipv4_node->ds_node_usrid.binding_data_l)
        {
            SYS_USRID_DBG_INFO("Action info -- vrfid                :0x%x\n", p_ipv4_node->ds_node_usrid.binding_data_l);
        }

        /*binding data high*/
        if (CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_h , 0x8000))
        {
            SYS_USRID_DBG_INFO("Action info -- service aclqos en    :1\n");
        }

        if (CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_h, 0x4000))
        {
            SYS_USRID_DBG_INFO("Action info -- vpls src port valid  :1\n");
            SYS_USRID_DBG_INFO("Action info -- vpls src port        :0x%x\n",
                             (p_ipv4_node->ds_node_usrid.binding_data_h & 0x1FFF));
        }
        else if (!CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_h, 0x1000))
        {
            SYS_USRID_DBG_INFO("Action info -- usr svlan id         :0x%x\n",
                        (p_ipv4_node->ds_node_usrid.binding_data_h & 0xFFF));

        }
        else if ((p_ipv4_node->ds_node_usrid.aps_select_valid)
        && (0xFFF !=(p_ipv4_node->ds_node_usrid.binding_data_h & 0xFFF)))
        {
            SYS_USRID_DBG_INFO("Action info -- aps select group     :1\n");
            SYS_USRID_DBG_INFO("Action info -- aps select group id  :0x%x\n",
                                (p_ipv4_node->ds_node_usrid.binding_data_h & 0xFFF));
        }

        if (p_ipv4_node->ds_node_usrid.aps_select_valid)
        {
            SYS_USRID_DBG_INFO("Action info -- aps select valid     :1\n");
            SYS_USRID_DBG_INFO("Action info -- protecting path      :%d\n",
                                p_ipv4_node->ds_node_usrid.binding_data_h >> 13 & 0x1);
        }

        /*binding data middle*/
        if (CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_m , 0xc000))
        {
            SYS_USRID_DBG_INFO("Action info -- service policer valid:1\n");
        }

        if (CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_m, 0x8000)
        && (0x3FFF != (p_ipv4_node->ds_node_usrid.binding_data_m & 0x3FFF)))
        {
            SYS_USRID_DBG_INFO("Action info -- service id enable    :1\n");
            SYS_USRID_DBG_INFO("Action info -- service id           :0x%x\n",
                                (p_ipv4_node->ds_node_usrid.binding_data_m & 0x3FFF));

        }
        else if (CTC_FLAG_ISSET(p_ipv4_node->ds_node_usrid.binding_data_m, 0x4000))
        {
            SYS_USRID_DBG_INFO("Action info -- usr cvlan valid      :1\n");
            SYS_USRID_DBG_INFO("Action info -- usr cvlan id         :0x%x\n",
                                (p_ipv4_node->ds_node_usrid.binding_data_m & 0xFFF));
        }
        else if (0x3FFF != (p_ipv4_node->ds_node_usrid.binding_data_m & 0x3FFF))
        {
            SYS_USRID_DBG_INFO("Action info -- flow policer ptr     :0x%x\n",
                                 (p_ipv4_node->ds_node_usrid.binding_data_m & 0x3FFF));

        }

        SYS_USRID_DBG_INFO("================\n");
    }while(NULL != (p_avl_node = ctc_avl_next(p_avl_node)));

    return;
}

void
sys_humber_usrid_db_show_ipv6_by_label(uint8 chip_id, uint8 label_id)
{
    uint8 i;
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;
    ctc_avl_node_t *p_avl_node = NULL;
    sys_usrid_db_ipv6_node_t *p_ipv6_node = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_IPV6_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);

    SYS_USRID_DBG_INFO("Show usrid ipv4 info on chip:%d label:%d, total count:%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");
    p_avl_node = ctc_avl_top(*pp_avl_tree);
    if (NULL == p_avl_node)
    {
        return;
    }

    do
    {
        p_ipv6_node = (sys_usrid_db_ipv6_node_t *)p_avl_node->info;

        SYS_USRID_DBG_INFO("Key offset                          :%d\n", p_ipv6_node->key_offset);

        SYS_USRID_DBG_INFO("Key info    -- v6sa                 :0x");
        for (i = 0; i < 4; i++)
        {
            SYS_USRID_DBG_INFO("%x:", p_ipv6_node->usrid_key_node.ipv6_sa[i]);
        }
        SYS_USRID_DBG_INFO("\n");

        SYS_USRID_DBG_INFO("Key info    -- v6_smask             :0x");
        for (i = 0; i < 4; i++)
        {
            SYS_USRID_DBG_INFO("%x:", p_ipv6_node->usrid_key_node.ipv6_smask[i]);
        }
        SYS_USRID_DBG_INFO("\n");

        SYS_USRID_DBG_INFO("Key info    -- v6da                 :0x");
        for (i = 0; i < 4; i++)
        {
            SYS_USRID_DBG_INFO("%x:", p_ipv6_node->usrid_key_node.ipv6_da[i]);
        }
        SYS_USRID_DBG_INFO("\n");

        SYS_USRID_DBG_INFO("Key info    -- v6_dmask             :0x");
        for (i = 0; i < 4; i++)
        {
            SYS_USRID_DBG_INFO("%x:", p_ipv6_node->usrid_key_node.ipv6_dmask[i]);
        }
        SYS_USRID_DBG_INFO("\n");

        if (p_ipv6_node->valid.l4_src_port_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- l4 srcport           :0x%x\n", p_ipv6_node->usrid_key_node.l4_src_port);
        }

        if (p_ipv6_node->valid.l4_dest_port_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- l4 destport          :0x%x\n", p_ipv6_node->usrid_key_node.l4_dest_port);
        }

        if (p_ipv6_node->valid.igs_cvid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- cvid                 :0x%x\n", p_ipv6_node->usrid_key_node.cvid);
        }

        if (p_ipv6_node->valid.igs_svid_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- svid                 :0x%x\n", p_ipv6_node->usrid_key_node.svid);
        }

        if ((0 != p_ipv6_node->usrid_key_node.macsa_mask_h) || (0 != p_ipv6_node->usrid_key_node.macsa_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macsa mask           :0x%x%x\n",
                                p_ipv6_node->usrid_key_node.macsa_mask_h, p_ipv6_node->usrid_key_node.macsa_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macsa                :0x%x%x\n",
                                p_ipv6_node->usrid_key_node.macsa_h, p_ipv6_node->usrid_key_node.macsa_l);
        }

        if ((0 != p_ipv6_node->usrid_key_node.macda_mask_h) || (0 != p_ipv6_node->usrid_key_node.macda_mask_l))
        {
            SYS_USRID_DBG_INFO("Key info    -- macda mask           :0x%x%x\n",
                                p_ipv6_node->usrid_key_node.macda_mask_h, p_ipv6_node->usrid_key_node.macda_mask_l);
            SYS_USRID_DBG_INFO("Key info    -- macda                :0x%x%x\n",
                                p_ipv6_node->usrid_key_node.macda_h, p_ipv6_node->usrid_key_node.macda_l);
        }

        if (p_ipv6_node->valid.stag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cos             :%d\n", p_ipv6_node->usrid_key_node.stag_cos);
        }

        if (p_ipv6_node->valid.stag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- stag cfi             :%d\n", p_ipv6_node->usrid_key_node.stag_cfi);
        }

        if (p_ipv6_node->valid.ctag_cos_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cos             :%d\n", p_ipv6_node->usrid_key_node.ctag_cos);
        }

        if (p_ipv6_node->valid.ctag_cfi_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- ctag cfi             :%d\n", p_ipv6_node->usrid_key_node.ctag_cfi);
        }

        if (p_ipv6_node->valid.l2_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer2 type          :%d\n", p_ipv6_node->usrid_key_node.l2_type);
        }

        if (p_ipv6_node->valid.l3_type_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- layer3 type          :%d\n", p_ipv6_node->usrid_key_node.l3_type);
        }

        if (p_ipv6_node->valid.dscp_valid)
        {
            SYS_USRID_DBG_INFO("Key info    -- dscp                 :%d\n", p_ipv6_node->usrid_key_node.dscp);
        }

        if (p_ipv6_node->valid.frag_info_valid)
        {
            SYS_USRID_DBG_INFO("Key info    --fragement             :%d\n", p_ipv6_node->usrid_key_node.frag_info);
        }

        if (p_ipv6_node->valid.ip_option_valid)
        {
            SYS_USRID_DBG_INFO("key info    -- ip option            :%d\n", p_ipv6_node->usrid_key_node.ip_option);
        }

        SYS_USRID_DBG_INFO("================\n");
    }while(NULL != (p_avl_node = ctc_avl_next(p_avl_node)));

    return;
}

void
sys_humber_usrid_db_show_vlan_count_by_port(uint8 chip_id, uint16 gport)
{
    uint32 index = 0;
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;

    SYS_USRID_DBG_INFO("========================================\n");

    if (CTC_IS_LINKAGG_PORT(gport))
    {
        index = CTC_MAP_GPORT_TO_LPORT(gport) + MAX_PORT_NUM_PER_CHIP;
        SYS_USRID_DBG_INFO("Show usrid_vlan count on linkagg 0x%x\n", gport);

        pp_avl_tree = _sys_humber_usrid_db_get_tree(0, SYS_USRID_VLAN_KEY, index);
        count = CTC_AVL_COUNT(*pp_avl_tree);

        SYS_USRID_DBG_INFO("total count:%d\n", count);
        SYS_USRID_DBG_INFO("========================================\n");
    }
    else
    {
        index = CTC_MAP_GPORT_TO_LPORT(gport);
        SYS_USRID_DBG_INFO("Show usrid_vlan count on port 0x%x\n", gport);

        pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_VLAN_KEY, index);
        count = CTC_AVL_COUNT(*pp_avl_tree);

        SYS_USRID_DBG_INFO("total count:%d\n", count);
        SYS_USRID_DBG_INFO("========================================\n");
    }

    return;
}

void
sys_humber_usrid_db_show_mac_count_by_label(uint8 chip_id, uint8 label_id)
{
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_MAC_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);
    SYS_USRID_DBG_INFO("Show usrid_mac count on chip:%d, label:%d\n total count%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");

    return;
}

void
sys_humber_usrid_db_show_ipv4_count_by_label(uint8 chip_id, uint8 label_id)
{
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_IPV4_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);
    SYS_USRID_DBG_INFO("Show usrid_ipv4 count on chip:%d, label:%d\n total count%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");

    return;
}

void
sys_humber_usrid_db_show_ipv6_count_by_label(uint8 chip_id, uint8 label_id)
{
    uint32 count = 0;
    ctc_avl_tree_t **pp_avl_tree = NULL;

    SYS_USRID_DBG_FUNC();
    SYS_USRID_DBG_INFO("========================================\n");

    pp_avl_tree = _sys_humber_usrid_db_get_tree(chip_id, SYS_USRID_IPV6_KEY, label_id);
    count = CTC_AVL_COUNT(*pp_avl_tree);
    SYS_USRID_DBG_INFO("Show usrid_ipv6 count on chip:%d, label:%d\n total count%d\n", chip_id, label_id, count);
    SYS_USRID_DBG_INFO("========================================\n");

    return;
}



