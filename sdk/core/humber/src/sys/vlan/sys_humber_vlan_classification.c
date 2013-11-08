/**
 @file sys_humber_vlan_classification.c

 @date 2009-12-30

 @version v2.0
*/

#include "ctc_debug.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "sys_humber_vlan_classification.h"
#include "sys_humber_usrid.h"
#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"

#include "drv_humber.h"
#include "drv_io.h"

/******************************************************************************
*
*   Macros, Globals and Defines
*
*******************************************************************************/
static bool is_vlan_class_init = FALSE;


#define SYS_VLAN_CLASS_DEBUG_INFO(FMT, ...) \
            { \
                CTC_DEBUG_OUT_INFO(vlan, vlan_class, VLAN_CLASS_SYS, FMT, ##__VA_ARGS__);\
            }

#define SYS_VLAN_CLASS_DEBUG_FUNC() \
            {\
                CTC_DEBUG_OUT_FUNC(vlan, vlan_class, VLAN_CLASS_SYS);\
            }


/******************************************************************************
*
*   Functions
*
*******************************************************************************/

/******************************************************************************
*
*   protocol vlan
*******************************************************************************/

static int32
_sys_humber_protocol_vlan_add_entry(ctc_parser_l3_type_t l3_type, uint16 vlan_id)
{
    int32 ret = CTC_E_NONE;
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint32 cmd = 0;
    ds_protocol_vlan_t ds_pro_vlan;


    CTC_VLAN_RANGE_CHECK(vlan_id);

	kal_memset(&ds_pro_vlan, 0,sizeof(ds_protocol_vlan_t));
    ds_pro_vlan.protocol_vlan_id       = vlan_id;
    ds_pro_vlan.protocol_vlan_id_valid = 1;

    cmd = DRV_IOW(IOC_TABLE, DS_PROTOCOL_VLAN, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        ret = ret? ret:drv_tbl_ioctl(chip_id, l3_type, cmd, &ds_pro_vlan);
    }

    return ret;
}

static int32
_sys_humber_protocol_vlan_remove_entry(ctc_parser_l3_type_t l3_type)
{
    int32 ret = CTC_E_NONE;
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint32 cmd = 0;
    ds_protocol_vlan_t ds_pro_vlan;

    kal_memset(&ds_pro_vlan, 0, sizeof(ds_protocol_vlan_t));
    cmd = DRV_IOW(IOC_TABLE, DS_PROTOCOL_VLAN, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        ret = ret? ret:drv_tbl_ioctl(chip_id, l3_type, cmd, &ds_pro_vlan);
    }

    return ret;
}







int32
sys_humber_vlan_classification_init(void)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    uint8 idx = 0;
    int32 ret = CTC_E_NONE;
    uint32 cmd = 0;
    uint32 entry_num =0;
    ds_protocol_vlan_t pro_vlan;


    if (TRUE == is_vlan_class_init)
    {
        return CTC_E_NONE;
    }

    kal_memset(&pro_vlan, 0, sizeof(ds_protocol_vlan_t));
    cmd = DRV_IOW(IOC_TABLE, DS_PROTOCOL_VLAN, DRV_ENTRY_FLAG);

    lchip_num = sys_humber_get_local_chip_num();
    sys_alloc_get_table_entry_num(DS_PROTOCOL_VLAN, &entry_num);

    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        for (idx = 0; idx < entry_num; idx++)
        {
            ret = ret? ret:drv_tbl_ioctl(chip_id, idx, cmd, &pro_vlan);
        }
    }

    is_vlan_class_init = TRUE;

    return CTC_E_NONE;
}
/*
int32
sys_humber_vlan_classification_deinit(void)
{
    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }

    is_vlan_class_init = FALSE;

    return CTC_E_NONE;
}
*/
int32
sys_humber_vlan_add_vlan_classification_entry(ctc_vlan_class_t *p_vlan_class)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    mac_addr_t tmp;
    sys_usrid_mac_entry_t usrid_mac;
    sys_usrid_ipv4_entry_t usrid_ipv4;
    sys_usrid_ipv6_entry_t usrid_ipv6;


    SYS_VLAN_CLASS_DEBUG_FUNC();
    SYS_VLAN_CLASS_DEBUG_INFO("------vlan class----------\n");
    SYS_VLAN_CLASS_DEBUG_INFO(" type                         :%d\n", p_vlan_class->type);

    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_class);
    CTC_VLAN_RANGE_CHECK(p_vlan_class->vlan_id);

    kal_memset(&tmp, 0, sizeof(mac_addr_t));
    kal_memset(&usrid_mac, 0, sizeof(sys_usrid_mac_entry_t));
    kal_memset(&usrid_ipv4, 0, sizeof(sys_usrid_ipv4_entry_t));
    kal_memset(&usrid_ipv6, 0, sizeof(sys_usrid_ipv6_entry_t));

    lchip_num = sys_humber_get_local_chip_num();

    switch (p_vlan_class->type)
    {
        case CTC_VLAN_CLASS_MAC:
            {
                SYS_VLAN_CLASS_DEBUG_INFO(" macsa                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_mac.macsa[0],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[1],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[2],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[3],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[4],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[5]);
                usrid_mac.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_mac.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[1]));
                usrid_mac.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_mac.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_mac.macsa[5]);

                usrid_mac.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_mac.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[1]));
                usrid_mac.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_mac.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_mac.macsa_mask[5]);

                SYS_VLAN_CLASS_DEBUG_INFO(" macda                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_mac.macda[0],
                                            p_vlan_class->vlan_class.vlan_mac.macda[1],
                                            p_vlan_class->vlan_class.vlan_mac.macda[2],
                                            p_vlan_class->vlan_class.vlan_mac.macda[3],
                                            p_vlan_class->vlan_class.vlan_mac.macda[4],
                                            p_vlan_class->vlan_class.vlan_mac.macda[5]);
                usrid_mac.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_mac.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[1]));
                usrid_mac.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_mac.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_mac.macda[5]);


                usrid_mac.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_mac.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[1]));
                usrid_mac.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_mac.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_mac.macda_mask[5]);


                if (0 != p_vlan_class->vlan_class.vlan_mac.l3_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l3type                     :%d\n", p_vlan_class->vlan_class.vlan_mac.l3_type);
                    usrid_mac.valid.l3_type_valid = 1;
                    usrid_mac.usrid_key_entry.l3_type = p_vlan_class->vlan_class.vlan_mac.l3_type;
                }
                usrid_mac.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                usrid_mac.ds_entry_usrid.usr_vlan_ptr = 0x1FFF;

                if(p_vlan_class->is_svlan)
                {
                    usrid_mac.ds_entry_usrid.binding_data_h.usr_svlan_valid = 1;
                    usrid_mac.ds_entry_usrid.binding_data_h.usr_svlan_id = p_vlan_class->vlan_id;
                }
                else
                {
                    usrid_mac.ds_entry_usrid.binding_data_m.usr_cvlan_valid = 1;
                    usrid_mac.ds_entry_usrid.binding_data_m.usr_cvlan_id = p_vlan_class->vlan_id;
                }

                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_mac.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_add_mac_entry(&usrid_mac));
                }
            }
            break;

        case CTC_VLAN_CLASS_IPV4:
            {
                /* ipv4 SA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" v4_sa                        :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa & 0xFF);

                SYS_VLAN_CLASS_DEBUG_INFO(" v4_smask                     :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask & 0xFF);

                usrid_ipv4.usrid_key_entry.ipv4_sa_mask = p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask;
                usrid_ipv4.usrid_key_entry.ipv4_sa = p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa;

                /* ipv4 DA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" v4_da                        :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_da & 0xFF);

                SYS_VLAN_CLASS_DEBUG_INFO(" v4_dmask                     :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask & 0xFF);

                usrid_ipv4.usrid_key_entry.ipv4_da_mask = p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask;
                usrid_ipv4.usrid_key_entry.ipv4_da = p_vlan_class->vlan_class.vlan_ipv4.ipv4_da;

                /* MAC SA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" macsa                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[0],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[1],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[2],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[3],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[4],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[5]);
                usrid_ipv4.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_ipv4.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[1]));
                usrid_ipv4.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_ipv4.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv4.macsa[5]);

                usrid_ipv4.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[1]));
                usrid_ipv4.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[5]);
                /* MAC DA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" macda                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[0],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[1],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[2],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[3],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[4],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[5]);
                usrid_ipv4.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_ipv4.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[1]));
                usrid_ipv4.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_ipv4.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv4.macda[5]);


                usrid_ipv4.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_ipv4.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[1]));
                usrid_ipv4.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_ipv4.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv4.macda_mask[5]);

                if (0 != p_vlan_class->vlan_class.vlan_ipv4.l3_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l3type                       :%d\n",
                                                p_vlan_class->vlan_class.vlan_ipv4.l3_type);
                    usrid_ipv4.valid.l3_type_valid = 1;
                    usrid_ipv4.usrid_key_entry.l3_type = p_vlan_class->vlan_class.vlan_ipv4.l3_type;
                }

                if (CTC_PARSER_L3_TYPE_IPV4 == p_vlan_class->vlan_class.vlan_ipv4.l3_type)
                {
                    if (0 != p_vlan_class->vlan_class.vlan_ipv4.l4src_port)
                    {
                        SYS_VLAN_CLASS_DEBUG_INFO(" l4srcport                    :%d\n",
                                                    p_vlan_class->vlan_class.vlan_ipv4.l4src_port);
                        usrid_ipv4.valid.l4_src_port_valid = 1;
                        usrid_ipv4.usrid_key_entry.l4_src_port = p_vlan_class->vlan_class.vlan_ipv4.l4src_port;
                    }

                    if (0 != p_vlan_class->vlan_class.vlan_ipv4.l4dest_port)
                    {
                        SYS_VLAN_CLASS_DEBUG_INFO(" l4destport                   :%d\n",
                                                    p_vlan_class->vlan_class.vlan_ipv4.l4dest_port);
                        usrid_ipv4.valid.l4_dest_port_valid = 1;
                        usrid_ipv4.usrid_key_entry.l4_dest_port = p_vlan_class->vlan_class.vlan_ipv4.l4dest_port;
                    }

                    if (CTC_PARSER_L4_TYPE_TCP == p_vlan_class->vlan_class.vlan_ipv4.l4_type)
                    {
                        SYS_VLAN_CLASS_DEBUG_INFO(" l4type                       :tcp\n");
                        usrid_ipv4.valid.is_tcp_valid = 1;
                        usrid_ipv4.usrid_key_entry.is_tcp = 1;
                    }

                    if (CTC_PARSER_L4_TYPE_UDP == p_vlan_class->vlan_class.vlan_ipv4.l4_type)
                    {
                        SYS_VLAN_CLASS_DEBUG_INFO(" l4type                       :udp\n");
                        usrid_ipv4.valid.is_udp_valid = 1;
                        usrid_ipv4.usrid_key_entry.is_udp = 1;
                    }
                }

                usrid_ipv4.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                usrid_ipv4.ds_entry_usrid.usr_vlan_ptr = 0x1FFF;

                if(p_vlan_class->is_svlan)
                {
                    usrid_ipv4.ds_entry_usrid.binding_data_h.usr_svlan_valid = 1;
                    usrid_ipv4.ds_entry_usrid.binding_data_h.usr_svlan_id = p_vlan_class->vlan_id;
                }
                else
                {
                    usrid_ipv4.ds_entry_usrid.binding_data_m.usr_cvlan_valid = 1;
                    usrid_ipv4.ds_entry_usrid.binding_data_m.usr_cvlan_id = p_vlan_class->vlan_id;
                }

                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_ipv4.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_add_ipv4_entry(&usrid_ipv4));
                }
            }
            break;

        case CTC_VLAN_CLASS_IPV6:
            {
               /* IPV6 SA*/
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_smask),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_smask),sizeof(ipv6_addr_t));
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_sa),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_sa),sizeof(ipv6_addr_t));
               /* IPV6 DA*/
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_dmask),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_dmask),sizeof(ipv6_addr_t));
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_da),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_da),sizeof(ipv6_addr_t));

               /* MAC SA*/
                usrid_ipv6.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_ipv6.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[1]));
                usrid_ipv6.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_ipv6.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv6.macsa[5]);

                usrid_ipv6.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[1]));
                usrid_ipv6.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[5]);
                /* MAC DA*/
                usrid_ipv6.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_ipv6.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[1]));
                usrid_ipv6.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_ipv6.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv6.macda[5]);


                usrid_ipv6.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_ipv6.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[1]));
                usrid_ipv6.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_ipv6.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv6.macda_mask[5]);


                if (0 != p_vlan_class->vlan_class.vlan_ipv6.l4src_port)
                {
                    usrid_ipv6.valid.l4_src_port_valid = 1;
                    usrid_ipv6.usrid_key_entry.l4_src_port = p_vlan_class->vlan_class.vlan_ipv6.l4src_port;
                }

                if (0 != p_vlan_class->vlan_class.vlan_ipv6.l4dest_port)
                {
                    usrid_ipv6.valid.l4_dest_port_valid = 1;
                    usrid_ipv6.usrid_key_entry.l4_dest_port = p_vlan_class->vlan_class.vlan_ipv6.l4dest_port;
                }

                if (CTC_PARSER_L4_TYPE_TCP == p_vlan_class->vlan_class.vlan_ipv6.l4_type)
                {
                    usrid_ipv6.valid.is_tcp_valid = 1;
                    usrid_ipv6.usrid_key_entry.is_tcp = 1;
                }

                if (CTC_PARSER_L4_TYPE_UDP == p_vlan_class->vlan_class.vlan_ipv6.l4_type)
                {
                    usrid_ipv6.valid.is_udp_valid = 1;
                    usrid_ipv6.usrid_key_entry.is_udp = 1;
                }
                usrid_ipv6.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                usrid_ipv6.ds_entry_usrid.usr_vlan_ptr = 0x1FFF;

                if(p_vlan_class->is_svlan)
                {
                    usrid_ipv6.ds_entry_usrid.binding_data_h.usr_svlan_valid = 1;
                    usrid_ipv6.ds_entry_usrid.binding_data_h.usr_svlan_id = p_vlan_class->vlan_id;
                }
                else
                {
                    usrid_ipv6.ds_entry_usrid.binding_data_m.usr_cvlan_valid = 1;
                    usrid_ipv6.ds_entry_usrid.binding_data_m.usr_cvlan_id = p_vlan_class->vlan_id;
                }

                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_ipv6.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_add_ipv6_entry(&usrid_ipv6));
                }
            }
            break;

        case CTC_VLAN_CLASS_PROTOCOL:
			CTC_ERROR_RETURN(_sys_humber_protocol_vlan_add_entry(p_vlan_class->vlan_class.vlan_protocol.l3_type,	p_vlan_class->vlan_id));

			break;
        default:
            return CTC_E_VLAN_CLASS_INVALID_TYPE;

    }

    SYS_VLAN_CLASS_DEBUG_INFO("--------------------------------\n");
    SYS_VLAN_CLASS_DEBUG_INFO(" vlan                         :%d\n", p_vlan_class->vlan_id);

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_vlan_classification_entry(ctc_vlan_class_t *p_vlan_class)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    mac_addr_t tmp;
    sys_usrid_mac_entry_t usrid_mac;
    sys_usrid_ipv4_entry_t usrid_ipv4;
    sys_usrid_ipv6_entry_t usrid_ipv6;

    SYS_VLAN_CLASS_DEBUG_FUNC();
    SYS_VLAN_CLASS_DEBUG_INFO("------vlan class----------\n");
    SYS_VLAN_CLASS_DEBUG_INFO(" type                         :%d\n", p_vlan_class->type);

    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }
    CTC_PTR_VALID_CHECK(p_vlan_class);

    kal_memset(&tmp, 0, sizeof(mac_addr_t));
    kal_memset(&usrid_mac, 0, sizeof(sys_usrid_mac_entry_t));
    kal_memset(&usrid_ipv4, 0, sizeof(sys_usrid_ipv4_entry_t));
    kal_memset(&usrid_ipv6, 0, sizeof(sys_usrid_ipv6_entry_t));

    lchip_num = sys_humber_get_local_chip_num();

    switch (p_vlan_class->type)
    {
        case CTC_VLAN_CLASS_MAC:
            {
                SYS_VLAN_CLASS_DEBUG_INFO(" macsa                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_mac.macsa[0],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[1],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[2],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[3],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[4],
                                            p_vlan_class->vlan_class.vlan_mac.macsa[5]);
                usrid_mac.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_mac.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[1]));
                usrid_mac.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_mac.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_mac.macsa[5]);

                usrid_mac.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_mac.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[1]));
                usrid_mac.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_mac.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_mac.macsa_mask[5]);

                SYS_VLAN_CLASS_DEBUG_INFO(" macda                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_mac.macda[0],
                                            p_vlan_class->vlan_class.vlan_mac.macda[1],
                                            p_vlan_class->vlan_class.vlan_mac.macda[2],
                                            p_vlan_class->vlan_class.vlan_mac.macda[3],
                                            p_vlan_class->vlan_class.vlan_mac.macda[4],
                                            p_vlan_class->vlan_class.vlan_mac.macda[5]);
                usrid_mac.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_mac.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[1]));
                usrid_mac.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_mac.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_mac.macda[4] << 8)
                                                    |p_vlan_class->vlan_class.vlan_mac.macda[5]);

                usrid_mac.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_mac.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[1]));
                usrid_mac.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_mac.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_mac.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_mac.macda_mask[5]);

                if (0 != p_vlan_class->vlan_class.vlan_mac.l3_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l3type                     :%d\n", p_vlan_class->vlan_class.vlan_mac.l3_type);
                    usrid_mac.valid.l3_type_valid = 1;
                    usrid_mac.usrid_key_entry.l3_type = p_vlan_class->vlan_class.vlan_mac.l3_type;
                }
                usrid_mac.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_mac.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_delete_mac_entry(&usrid_mac));
                }
            }
            break;

        case CTC_VLAN_CLASS_IPV4:
            {
               SYS_VLAN_CLASS_DEBUG_INFO(" v4_sa                        :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa & 0xFF);

               SYS_VLAN_CLASS_DEBUG_INFO(" v4_smask                     :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask & 0xFF);
               usrid_ipv4.usrid_key_entry.ipv4_sa_mask = p_vlan_class->vlan_class.vlan_ipv4.ipv4_smask;
               usrid_ipv4.usrid_key_entry.ipv4_sa = p_vlan_class->vlan_class.vlan_ipv4.ipv4_sa;

               SYS_VLAN_CLASS_DEBUG_INFO(" v4_da                        :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_da >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_da & 0xFF);

               SYS_VLAN_CLASS_DEBUG_INFO(" v4_dmask                     :%d.%d.%d.%d\n",
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 24) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 16) & 0xFF,
                                    (p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask >> 8) & 0xFF,
                                     p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask & 0xFF);
               usrid_ipv4.usrid_key_entry.ipv4_da_mask = p_vlan_class->vlan_class.vlan_ipv4.ipv4_dmask;
               usrid_ipv4.usrid_key_entry.ipv4_da = p_vlan_class->vlan_class.vlan_ipv4.ipv4_da;

               /* MAC SA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" macsa                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[0],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[1],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[2],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[3],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[4],
                                            p_vlan_class->vlan_class.vlan_ipv4.macsa[5]);
                usrid_ipv4.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_ipv4.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[1]));
                usrid_ipv4.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_ipv4.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv4.macsa[5]);

                usrid_ipv4.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[1]));
                usrid_ipv4.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv4.macsa_mask[5]);
                /* MAC DA*/
                SYS_VLAN_CLASS_DEBUG_INFO(" macda                       :0x%X%X%X%X%X%X\n",
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[0],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[1],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[2],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[3],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[4],
                                            p_vlan_class->vlan_class.vlan_ipv4.macda[5]);
                usrid_ipv4.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_ipv4.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[1]));
                usrid_ipv4.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_ipv4.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv4.macda[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv4.macda[5]);


                usrid_ipv4.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_ipv4.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[1]));
                usrid_ipv4.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_ipv4.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv4.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv4.macda_mask[5]);


                if (0 != p_vlan_class->vlan_class.vlan_ipv4.l3_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l3type                       :%d\n",
                                                p_vlan_class->vlan_class.vlan_ipv4.l3_type);
                    usrid_ipv4.valid.l3_type_valid = 1;
                    usrid_ipv4.usrid_key_entry.l3_type = p_vlan_class->vlan_class.vlan_ipv4.l3_type;
                }

                if (0 != p_vlan_class->vlan_class.vlan_ipv4.l4src_port)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l4srcport                    :%d\n",
                                                p_vlan_class->vlan_class.vlan_ipv4.l4src_port);
                    usrid_ipv4.valid.l4_src_port_valid = 1;
                    usrid_ipv4.usrid_key_entry.l4_src_port = p_vlan_class->vlan_class.vlan_ipv4.l4src_port;
                }

                if (0 != p_vlan_class->vlan_class.vlan_ipv4.l4dest_port)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l4destport                   :%d\n",
                                                p_vlan_class->vlan_class.vlan_ipv4.l4dest_port);
                    usrid_ipv4.valid.l4_dest_port_valid = 1;
                    usrid_ipv4.usrid_key_entry.l4_dest_port = p_vlan_class->vlan_class.vlan_ipv4.l4dest_port;
                }

                if (CTC_PARSER_L4_TYPE_TCP == p_vlan_class->vlan_class.vlan_ipv4.l4_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l4type                       :tcp\n");
                    usrid_ipv4.valid.is_tcp_valid = 1;
                    usrid_ipv4.usrid_key_entry.is_tcp = 1;
                }

                if (CTC_PARSER_L4_TYPE_UDP == p_vlan_class->vlan_class.vlan_ipv4.l4_type)
                {
                    SYS_VLAN_CLASS_DEBUG_INFO(" l4type                       :udp\n");
                    usrid_ipv4.valid.is_udp_valid = 1;
                    usrid_ipv4.usrid_key_entry.is_udp = 1;
                }
                usrid_ipv4.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;
                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_ipv4.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_delete_ipv4_entry(&usrid_ipv4));
                }
            }
            break;

        case CTC_VLAN_CLASS_IPV6:
            {
               /* IPV6 SA*/
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_smask),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_smask),sizeof(ipv6_addr_t));
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_sa),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_sa),sizeof(ipv6_addr_t));
               /* IPV6 DA*/
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_dmask),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_dmask),sizeof(ipv6_addr_t));
               kal_memcpy(&(usrid_ipv6.usrid_key_entry.ipv6_da),
                            &(p_vlan_class->vlan_class.vlan_ipv6.ipv6_da),sizeof(ipv6_addr_t));
               /* MAC SA*/
                usrid_ipv6.usrid_key_entry.macsa_h = ((p_vlan_class->vlan_class.vlan_ipv6.macsa[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[1]));
                usrid_ipv6.usrid_key_entry.macsa_l = ((p_vlan_class->vlan_class.vlan_ipv6.macsa[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macsa[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv6.macsa[5]);

                usrid_ipv6.usrid_key_entry.macsa_mask_h = ((p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[1]));
                usrid_ipv6.usrid_key_entry.macsa_mask_l = ((p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv6.macsa_mask[5]);
                /* MAC DA*/
                usrid_ipv6.usrid_key_entry.macda_h = ((p_vlan_class->vlan_class.vlan_ipv6.macda[0] << 8)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[1]));
                usrid_ipv6.usrid_key_entry.macda_l = ((p_vlan_class->vlan_class.vlan_ipv6.macda[2] << 24)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[3] << 16)
                                                    |(p_vlan_class->vlan_class.vlan_ipv6.macda[4] << 8)
                                                    | p_vlan_class->vlan_class.vlan_ipv6.macda[5]);


                usrid_ipv6.usrid_key_entry.macda_mask_h = ((p_vlan_class->vlan_class.vlan_ipv6.macda_mask[0] << 8)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[1]));
                usrid_ipv6.usrid_key_entry.macda_mask_l = ((p_vlan_class->vlan_class.vlan_ipv6.macda_mask[2] << 24)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[3] << 16)
                                                         |(p_vlan_class->vlan_class.vlan_ipv6.macda_mask[4] << 8)
                                                         | p_vlan_class->vlan_class.vlan_ipv6.macda_mask[5]);

                if (0 != p_vlan_class->vlan_class.vlan_ipv6.l4src_port)
                {
                    usrid_ipv6.valid.l4_src_port_valid = 1;
                    usrid_ipv6.usrid_key_entry.l4_src_port = p_vlan_class->vlan_class.vlan_ipv6.l4src_port;
                }

                if (0 != p_vlan_class->vlan_class.vlan_ipv6.l4dest_port)
                {
                    usrid_ipv6.valid.l4_dest_port_valid = 1;
                    usrid_ipv6.usrid_key_entry.l4_dest_port = p_vlan_class->vlan_class.vlan_ipv6.l4dest_port;
                }

                if (CTC_PARSER_L4_TYPE_TCP == p_vlan_class->vlan_class.vlan_ipv6.l4_type)
                {
                    usrid_ipv6.valid.is_tcp_valid = 1;
                    usrid_ipv6.usrid_key_entry.is_tcp = 1;
                }

                if (CTC_PARSER_L4_TYPE_UDP == p_vlan_class->vlan_class.vlan_ipv6.l4_type)
                {
                    usrid_ipv6.valid.is_udp_valid = 1;
                    usrid_ipv6.usrid_key_entry.is_udp = 1;
                }
                usrid_ipv6.usrid_key_entry.usrid_label = SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS;

                for (chip_id = 0; chip_id < lchip_num; chip_id++)
                {
                    usrid_ipv6.usrid_key_entry.chip_id = chip_id;
                    CTC_ERROR_RETURN(sys_humber_usrid_delete_ipv6_entry(&usrid_ipv6));
                }
            }
            break;

        case CTC_VLAN_CLASS_PROTOCOL:
			CTC_ERROR_RETURN(_sys_humber_protocol_vlan_remove_entry(p_vlan_class->vlan_class.vlan_protocol.l3_type));

			break;
        default:
            return CTC_E_VLAN_CLASS_INVALID_TYPE;

    }

    return CTC_E_NONE;
}




int32
sys_humber_vlan_flush_vlan_classification_entry(ctc_vlan_class_type_t type)
{
    int32 ret = 0;
	uint32 entry_num =0;
	uint8 chip_id = 0;
	uint8 lchip_num = 0;
	uint32 cmd = 0;
	ds_protocol_vlan_t ds_pro_vlan;
	uint8 pro_vlan_idx =0;

    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }

    switch(type)
    {
        case CTC_VLAN_CLASS_MAC:
            ret = sys_humber_usrid_remove_mac_all_by_label(SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS);

            break;
        case CTC_VLAN_CLASS_IPV4:
            ret = sys_humber_usrid_remove_ipv4_all_by_label(SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS);

            break;
        case CTC_VLAN_CLASS_IPV6:
            ret = sys_humber_usrid_remove_ipv6_all_by_label(SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS);

            break;
        case CTC_VLAN_CLASS_PROTOCOL:
			sys_alloc_get_table_entry_num(DS_PROTOCOL_VLAN, &entry_num);

			lchip_num = sys_humber_get_local_chip_num();
            for (chip_id = 0; chip_id < lchip_num; chip_id++)
            {
                for (pro_vlan_idx = 0; pro_vlan_idx < entry_num; pro_vlan_idx++)
                {
                    kal_memset(&ds_pro_vlan, 0, sizeof(ds_protocol_vlan_t));
                    cmd = DRV_IOW(IOC_TABLE, DS_PROTOCOL_VLAN, DRV_ENTRY_FLAG);
	                ret = ret? ret:drv_tbl_ioctl(chip_id, pro_vlan_idx, cmd, &ds_pro_vlan);
                }
            }

            break;
        default:
            return CTC_E_VLAN_CLASS_INVALID_TYPE;
    }

    return ret;
}









int32
sys_humber_vlan_add_vlan_classification_default_entry(ctc_vlan_class_type_t type, ctc_vlan_miss_t *p_action)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    int32 (*p_fun)(uint8, uint8, sys_usrid_ds_entry_t*) = NULL;
    sys_usrid_ds_entry_t usrid_entry;

    SYS_VLAN_CLASS_DEBUG_FUNC();
    SYS_VLAN_CLASS_DEBUG_INFO("------vlan classify----------\n");
    SYS_VLAN_CLASS_DEBUG_INFO("type                       :%d\n(0-mac, 1-ipv4, 2-ipv6, 3-none)\n", type);

    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }

    switch(type)
    {
        case CTC_VLAN_CLASS_MAC:
            p_fun = sys_humber_usrid_add_mac_default_entry_per_label;

            break;

        case CTC_VLAN_CLASS_IPV4:
            p_fun = sys_humber_usrid_add_ipv4_default_entry_per_label;

            break;

        case CTC_VLAN_CLASS_IPV6:
            p_fun = sys_humber_usrid_add_ipv6_default_entry_per_label;

            break;
        case CTC_VLAN_CLASS_PROTOCOL:
            return CTC_E_UNEXPECT;

            break;

        default:
            return CTC_E_VLAN_CLASS_INVALID_TYPE;
    }

    kal_memset(&usrid_entry, 0, sizeof(sys_usrid_ds_entry_t));

    if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DO_NOTHING))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DISCARD))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_TO_CPU))
    {
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.by_pass_all = 1;
        usrid_entry.usrid_exception_en = 1;
        usrid_entry.fwd_ptr_valid = 1;
        usrid_entry.binding_data_l.fwd_ptr = 0xFFFF;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_SVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->svlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_h.usr_svlan_valid = 1;
        usrid_entry.binding_data_h.usr_svlan_id = p_action->svlan_id;
    }
    else if (CTC_FLAG_ISSET(p_action->flag, CTC_VLAN_MISS_ACTION_DEF_CVLAN))
    {
        CTC_VLAN_RANGE_CHECK(p_action->cvlan_id);
        usrid_entry.usr_vlan_ptr = 0x1FFF;
        usrid_entry.binding_data_m.usr_cvlan_valid = 1;
        usrid_entry.binding_data_m.usr_cvlan_id = p_action->cvlan_id;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN((*p_fun)(chip_id, SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS, &usrid_entry));
    }

    return CTC_E_NONE;
}

int32
sys_humber_vlan_remove_vlan_classification_default_entry(ctc_vlan_class_type_t type)
{
    uint8 chip_id = 0;
    uint8 lchip_num = 0;
    int32 (*p_fun)(uint8, uint8) = NULL;

    SYS_VLAN_CLASS_DEBUG_FUNC();
    SYS_VLAN_CLASS_DEBUG_INFO("------vlan classify----------\n");
    SYS_VLAN_CLASS_DEBUG_INFO("type                       :%d\n(0-mac, 1-ipv4, 2-ipv6, 3-none)\n", type);

    if (FALSE == is_vlan_class_init)
    {
        return CTC_E_NOT_INIT;
    }

    switch(type)
    {
        case CTC_VLAN_CLASS_MAC:
            p_fun = sys_humber_usrid_delete_mac_default_entry_per_label;

            break;

        case CTC_VLAN_CLASS_IPV4:
            p_fun = sys_humber_usrid_delete_ipv4_default_entry_per_label;

            break;

        case CTC_VLAN_CLASS_IPV6:
            p_fun = sys_humber_usrid_delete_ipv6_default_entry_per_label;

            break;

        case CTC_VLAN_CLASS_PROTOCOL:
            return CTC_E_UNEXPECT;

            break;

        default:
            return CTC_E_VLAN_CLASS_INVALID_TYPE;
    }

    lchip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < lchip_num; chip_id++)
    {
        CTC_ERROR_RETURN((*p_fun)(chip_id, SYS_USRID_RESERVE_LABEL_FOR_VLAN_CLASS));
    }

    return CTC_E_NONE;
}







