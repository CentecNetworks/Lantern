/**
 @file sys_humber_pdu.c

 @date 2009-12-30

 @version v2.0

---file comments----
*/

#include "kal.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "sys_humber_pdu.h"
#include "sys_humber_chip.h"
#include "sys_humber_port.h"
#include "sys_humber_l3if.h"

#include "drv_io.h"
#include "drv_humber.h"


/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
#define MAX_SYS_PDU_L2PDU_BASED_L2HDR_PTL_ENTRY 16
#define MAX_SYS_PDU_L2PDU_BASED_MACDA_LOW24_ENTRY 8
#define MAX_SYS_PDU_L2PDU_BASED_MACDA_ENTRY 4
#define MAX_SYS_PDU_L2PDU_BASED_L3TYPE_ENTRY 15

#define MAX_SYS_PDU_L3PDU_BASED_L3HDR_PROTO 16
#define MAX_SYS_PDU_L3PDU_BASED_PORT 16


/***************************************************************
*
*  Functions
*
***************************************************************/
static int32
_sys_humber_l2pdu_classify_l2pdu_by_l2hdr_proto(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE0;
    uint32 ether_type = 0;
    uint32 ether_type_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_L2HDR_PTL_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    ether_type = entry->l2hdr_proto;

    local_chip_num = sys_humber_get_local_chip_num();

    ether_type_field_id += index_step * index;

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM3, ether_type_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &ether_type));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_classified_key_by_l2hdr_proto(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE0;
    uint32 ether_type = 0;
    uint32 ether_type_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM3_CAM3_ETHER_TYPE0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_L2HDR_PTL_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    ether_type_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM3, ether_type_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &ether_type));
    entry->l2hdr_proto = ether_type;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_classify_l2pdu_by_macda(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK1_BIT47_TO32
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT47_TO32;
    uint32 mac_da_mask_bit31_to0_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT31_TO0;
    uint32 mac_da_mask_bit47_to32_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT47_TO32;
    uint32 mac_da_value_bit31_to0_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_VALUE0_BIT31_TO0;
    uint32 mac_da_value_bit47_to32_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_VALUE0_BIT47_TO32;

    uint32 mac_da_mask_bit31_to0 = 0;
    uint32 mac_da_mask_bit47_to32 = 0;
    uint32 mac_da_value_bit31_to0 = 0;
    uint32 mac_da_value_bit47_to32 = 0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    mac_da_mask_bit31_to0_field_id += index_step * index;
    mac_da_mask_bit47_to32_field_id += index_step * index;
    mac_da_value_bit31_to0_field_id += index_step * index;
    mac_da_value_bit47_to32_field_id += index_step * index;

    mac_da_mask_bit31_to0 =  entry->l2pdu_by_mac.mac_mask[2] << 24
                            |entry->l2pdu_by_mac.mac_mask[3] << 16
                            |entry->l2pdu_by_mac.mac_mask[4] << 8
                            |entry->l2pdu_by_mac.mac_mask[5];

    mac_da_mask_bit47_to32 = entry->l2pdu_by_mac.mac_mask[0] << 8
                            |entry->l2pdu_by_mac.mac_mask[1];

    mac_da_value_bit31_to0 = entry->l2pdu_by_mac.mac[2] << 24
                            |entry->l2pdu_by_mac.mac[3] << 16
                            |entry->l2pdu_by_mac.mac[4] << 8
                            |entry->l2pdu_by_mac.mac[5];

    mac_da_value_bit47_to32 = entry->l2pdu_by_mac.mac[0] << 8
                            | entry->l2pdu_by_mac.mac[1];

    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_mask_bit31_to0_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_mask_bit31_to0));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_mask_bit47_to32_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_mask_bit47_to32));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_value_bit31_to0_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_value_bit31_to0));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_value_bit47_to32_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_value_bit47_to32));
     }

     return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_classified_key_by_macda(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  local_chip_num = 0;
    uint8  chip_id = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK1_BIT47_TO32
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT47_TO32;
    uint32 mac_da_mask_bit31_to0 = 0;
    uint32 mac_da_mask_bit47_to32 = 0;
    uint32 mac_da_value_bit31_to0 = 0;
    uint32 mac_da_value_bit47_to32 = 0;
    uint32 mac_da_mask_bit31_to0_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT31_TO0;
    uint32 mac_da_mask_bit47_to32_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_MASK0_BIT47_TO32;
    uint32 mac_da_value_bit31_to0_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_VALUE0_BIT31_TO0;
    uint32 mac_da_value_bit47_to32_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_MAC_DA_VALUE0_BIT47_TO32;

    if (index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    mac_da_mask_bit31_to0_field_id += index_step * index;
    mac_da_mask_bit47_to32_field_id += index_step * index;
    mac_da_value_bit31_to0_field_id += index_step * index;
    mac_da_value_bit47_to32_field_id += index_step * index;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_mask_bit31_to0_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_mask_bit31_to0));

        cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_mask_bit47_to32_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_mask_bit47_to32));

        cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_value_bit31_to0_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_value_bit31_to0));

        cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, mac_da_value_bit47_to32_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &mac_da_value_bit47_to32));

        entry->l2pdu_by_mac.mac_mask[2] = (mac_da_mask_bit31_to0 >> 24) & 0xff;
        entry->l2pdu_by_mac.mac_mask[3] = (mac_da_mask_bit31_to0 >> 16) & 0xff;
        entry->l2pdu_by_mac.mac_mask[4] = (mac_da_mask_bit31_to0 >> 8) & 0xff;
        entry->l2pdu_by_mac.mac_mask[5] = (mac_da_mask_bit31_to0) & 0xff;

        entry->l2pdu_by_mac.mac_mask[0] = (mac_da_mask_bit47_to32 >> 8) & 0xff;
        entry->l2pdu_by_mac.mac_mask[1] = (mac_da_mask_bit47_to32 ) & 0xff;

        entry->l2pdu_by_mac.mac[2] = (mac_da_value_bit31_to0 >> 24) & 0xff;
        entry->l2pdu_by_mac.mac[3] = (mac_da_value_bit31_to0 >> 16) & 0xff;
        entry->l2pdu_by_mac.mac[4] = (mac_da_value_bit31_to0 >> 8) & 0xff;
        entry->l2pdu_by_mac.mac[5] = (mac_da_value_bit31_to0) & 0xff;

        entry->l2pdu_by_mac.mac[0] = (mac_da_value_bit47_to32 >> 8) & 0xff;
        entry->l2pdu_by_mac.mac[1] = (mac_da_value_bit47_to32) & 0xff;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_classify_l2pdu_by_macda_low24(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK0;
    uint32 cam2_mac_da_mask = 0;
    uint32 cam2_mac_da_value = 0;
    uint32 cam2_mac_da_mask_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK0;
    uint32 cam2_mac_da_value_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_VALUE0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_LOW24_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    cam2_mac_da_mask_field_id += index_step * index;
    cam2_mac_da_value_field_id += index_step * index;

    if((entry->l2pdu_by_mac.mac[0] != 0x01) || (entry->l2pdu_by_mac.mac[1] != 0x80)
        || (entry->l2pdu_by_mac.mac[2] != 0xc2))
    {
        return CTC_E_INVALID_MACDA;
    }
    if((entry->l2pdu_by_mac.mac_mask[0] != 0xff) || (entry->l2pdu_by_mac.mac_mask[1] != 0xff)
        || (entry->l2pdu_by_mac.mac_mask[2] != 0xff))
    {
        return CTC_E_INVALID_MACDA_MASK;
    }

    local_chip_num = sys_humber_get_local_chip_num();

    cam2_mac_da_mask = entry->l2pdu_by_mac.mac_mask[2] << 24 | entry->l2pdu_by_mac.mac_mask[3] << 16
                     | entry->l2pdu_by_mac.mac_mask[4] << 8  |entry->l2pdu_by_mac.mac_mask[5];

    cam2_mac_da_value = entry->l2pdu_by_mac.mac[2] << 24 | entry->l2pdu_by_mac.mac[3] << 16
                      | entry->l2pdu_by_mac.mac[4] << 8  | entry->l2pdu_by_mac.mac[5];

    cam2_mac_da_mask &= 0xffffff;
    cam2_mac_da_value &= 0xffffff;

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM2, cam2_mac_da_mask_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &cam2_mac_da_mask));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM2, cam2_mac_da_value_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &cam2_mac_da_value));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_classified_key_by_macda_low24(uint8 index, ctc_pdu_l2pdu_key_t* entry)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK0;
    uint32 cam2_mac_da_mask = 0;
    uint32 cam2_mac_da_value = 0;
    uint32 cam2_mac_da_mask_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_MASK0;
    uint32 cam2_mac_da_value_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM2_CAM2_MAC_DA_VALUE0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_LOW24_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    cam2_mac_da_mask_field_id += index_step * index;
    cam2_mac_da_value_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM2, cam2_mac_da_mask_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &cam2_mac_da_mask));

    entry->l2pdu_by_mac.mac_mask[2] = (cam2_mac_da_mask >> 24) & 0xff;
    entry->l2pdu_by_mac.mac_mask[3] = (cam2_mac_da_mask >> 16) & 0xff;
    entry->l2pdu_by_mac.mac_mask[4] = (cam2_mac_da_mask >> 8) & 0xff;
    entry->l2pdu_by_mac.mac_mask[5] = (cam2_mac_da_mask) & 0xff;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM2, cam2_mac_da_value_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &cam2_mac_da_value));

    entry->l2pdu_by_mac.mac[2] = (cam2_mac_da_value>> 24) & 0xff;
    entry->l2pdu_by_mac.mac[3] = (cam2_mac_da_value >> 16) & 0xff;
    entry->l2pdu_by_mac.mac[4] = (cam2_mac_da_value >> 8) & 0xff;
    entry->l2pdu_by_mac.mac[5] = (cam2_mac_da_value) & 0xff;

    return CTC_E_NONE;
}

/**
 @brief  Classify layer2 pdu based on macda,macda-low24 bit, layer2 header protocol
*/
extern int32
sys_humber_l2pdu_classify_l2pdu(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key)
{
    CTC_PTR_VALID_CHECK(key);

    switch(l2pdu_type)
    {
        case CTC_PDU_L2PDU_TYPE_L2HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_classify_l2pdu_by_l2hdr_proto(index, key));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_classify_l2pdu_by_macda(index, key));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA_LOW24:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_classify_l2pdu_by_macda_low24(index, key));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

extern int32
sys_humber_l2pdu_get_classified_key(ctc_pdu_l2pdu_type_t l2pdu_type, uint8 index,
                                                                           ctc_pdu_l2pdu_key_t* key)
{
    CTC_PTR_VALID_CHECK(key);


    switch(l2pdu_type)
    {
        case CTC_PDU_L2PDU_TYPE_L2HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_classified_key_by_l2hdr_proto(index, key));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_classified_key_by_macda(index, key));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA_LOW24:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_classified_key_by_macda_low24(index, key));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_set_global_action_by_l3type(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    ipe_bpdu_escape_ctl_t escap_ctl;
    int8   index_step = IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX1
                      - IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX0;
    uint32 protocol_exception_sub = 0;
    uint32 protocol_exception_sub_field_id = IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_CTC_PARSER_L3_TYPE)
    {
        return CTC_E_INVALID_INDEX;
    }

    protocol_exception_sub_field_id += index_step * index;
    protocol_exception_sub = ctl->action_index;

    kal_memset(&escap_ctl, 0, sizeof(ipe_bpdu_escape_ctl_t));

    local_chip_num = sys_humber_get_local_chip_num();
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOR(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));
        if(ctl->bypass_all)
        {
            escap_ctl.protocol_bypass_all |= (1 << index);
        }
        else
        {
            escap_ctl.protocol_bypass_all &= (~(1 << index));
        }

        if(ctl->entry_valid)
        {
            escap_ctl.protocol_escape |= (1 << index);
        }
        else
        {
            escap_ctl.protocol_escape &= (~(1 << index));
        }

        if(ctl->copy_to_cpu)
        {
            escap_ctl.protocol_exception |= (1 << index);
        }
        else
        {
            escap_ctl.protocol_exception &= (~(1 << index));
        }

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_ESCAPE_CTL, protocol_exception_sub_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &protocol_exception_sub));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_global_action_by_layer3_type(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX1
                      - IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX0;
    uint32 protocol_exception_sub = 0;
    uint32 protocol_exception_sub_field_id = IPE_BPDU_ESCAPE_CTL_PROTOCOL_EXCEPTION_SUB_INDEX0;
    ipe_bpdu_escape_ctl_t escap_ctl;

    if(index >= MAX_CTC_PARSER_L3_TYPE)
    {
        return CTC_E_INVALID_INDEX;
    }

    kal_memset(&escap_ctl, 0, sizeof(ipe_bpdu_escape_ctl_t));

    protocol_exception_sub_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &escap_ctl));

    ctl->bypass_all = (escap_ctl.protocol_bypass_all & (1 << index))?1:0;
    ctl->copy_to_cpu = (escap_ctl.protocol_exception & (1 << index))?1:0;
    ctl->entry_valid = (escap_ctl.protocol_escape & (1 << index))?1:0;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_ESCAPE_CTL, protocol_exception_sub_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &protocol_exception_sub));

    ctl->action_index = protocol_exception_sub;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_set_global_action_by_bpdu(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    ipe_bpdu_escape_ctl_t escap_ctl;

    kal_memset(&escap_ctl, 0, sizeof(ipe_bpdu_escape_ctl_t));

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOR(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));

        escap_ctl.bpdu_bypass_all = ctl->bypass_all?1:0;
        escap_ctl.bpdu_exception = ctl->copy_to_cpu?1:0;
        escap_ctl.bpdu_exception_sub_index = ctl->action_index;

        if (ctl->entry_valid)
            SET_BIT(escap_ctl.bpdu_escape_en, 2);
        else
            CLEAR_BIT(escap_ctl.bpdu_escape_en, 2);

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));
     }
    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_global_action_by_bpdu(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    ipe_bpdu_escape_ctl_t escap_ctl;

    kal_memset(&escap_ctl, 0, sizeof(ipe_bpdu_escape_ctl_t));
    cmd = DRV_IOR(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &escap_ctl));


    ctl->bypass_all = escap_ctl.bpdu_bypass_all;
    ctl->copy_to_cpu = escap_ctl.bpdu_exception;
    ctl->action_index = escap_ctl.bpdu_exception_sub_index;

    if (IS_BIT_SET(escap_ctl.bpdu_escape_en, 2))
         ctl->entry_valid = 1;
   else
	  ctl->entry_valid = 0;
    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_set_global_action_by_l2hdr_proto(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    uint32 entry_valid = 0;
    uint32 escape_bypass_all = 0;
    uint32 exception_sub_index = 0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ENTRY_VALID0;
    uint32 escape_bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_EXCEPTION_SUB_INDEX0;

    if (index >= MAX_SYS_PDU_L2PDU_BASED_L2HDR_PTL_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    entry_valid_field_id += index_step * index;
    escape_bypass_all_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;
    entry_valid = ctl->entry_valid;
    escape_bypass_all = ctl->bypass_all;
    exception_sub_index = ctl->action_index;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, entry_valid_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &entry_valid));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, escape_bypass_all_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escape_bypass_all));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, exception_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception_sub_index));
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_global_action_by_l2hdr_proto(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL0;
    uint32 bypass_all = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ESCAPE_BYPASS_ALL0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3_CAM3_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_L2HDR_PTL_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    bypass_all_field_id += index_step * index;
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, bypass_all_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &bypass_all));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, entry_valid_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &entry_valid));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, exception_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception_sub_index));

    ctl->bypass_all = bypass_all;
    ctl->entry_valid = entry_valid;
    ctl->action_index = exception_sub_index;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_set_global_action_by_macda(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL0;
    uint32 escape_bypass_all = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 escape_bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    escape_bypass_all_field_id += index_step * index;
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;
    escape_bypass_all = ctl->bypass_all?1:0;
    entry_valid = ctl->entry_valid?1:0;
    exception_sub_index = ctl->action_index;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, escape_bypass_all_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escape_bypass_all));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, entry_valid_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &entry_valid));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, exception_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception_sub_index));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_global_action_by_macda(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL0;
    uint32 bypass_all = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ESCAPE_BYPASS_ALL0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT_CAM1_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    bypass_all_field_id += index_step * index;
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, bypass_all_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &bypass_all));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, entry_valid_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &entry_valid));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, exception_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception_sub_index));

    ctl->bypass_all = bypass_all;
    ctl->entry_valid = entry_valid;
    ctl->action_index = exception_sub_index;

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_set_global_action_by_macda_low24(uint8 index, ctc_pdu_global_l2pdu_action_t*ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_EXCEPTION_SUB_INDEX1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_EXCEPTION_SUB_INDEX0;
    uint32 escape_bypass_all = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 escape_bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ESCAPE_BYPASS_ALL0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_LOW24_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    escape_bypass_all = ctl->bypass_all?1:0;
    entry_valid = ctl->entry_valid?1:0;
    exception_sub_index = ctl->action_index;

    escape_bypass_all_field_id += index_step * index;
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, escape_bypass_all_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escape_bypass_all));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, entry_valid_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &entry_valid));

        cmd = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, exception_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception_sub_index));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_get_global_action_by_macda_low24(uint8 index, ctc_pdu_global_l2pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ESCAPE_BYPASS_ALL1
                      - IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ESCAPE_BYPASS_ALL0;
    uint32 escape_bypass_all = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 escape_bypass_all_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ESCAPE_BYPASS_ALL0;
    uint32 entry_valid_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2_CAM2_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L2PDU_BASED_MACDA_LOW24_ENTRY)
    {
        return CTC_E_INVALID_INDEX;
    }

    escape_bypass_all_field_id += index_step * index;
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, escape_bypass_all_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &escape_bypass_all));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, entry_valid_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &entry_valid));

    cmd = DRV_IOR(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, exception_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception_sub_index));

    ctl->bypass_all = escape_bypass_all;
    ctl->entry_valid = entry_valid;
    ctl->action_index = exception_sub_index;

    return CTC_E_NONE;
}


/**
 @brief  Set layer2 pdu global property
*/
extern int32
sys_humber_l2pdu_set_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action)
{
    CTC_PTR_VALID_CHECK(action);

    if( action->action_index > MAX_SYS_L2PDU_PER_PORT_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }
    switch(l2pdu_type)
    {
        case CTC_PDU_L2PDU_TYPE_L3TYPE:
            /*the index is equal the layer3 type value*/
            CTC_ERROR_RETURN(_sys_humber_l2pdu_set_global_action_by_l3type(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_BPDU:
            /*the max index num is 1 for bpdu*/
            CTC_ERROR_RETURN(_sys_humber_l2pdu_set_global_action_by_bpdu(0, action));
            break;
        case CTC_PDU_L2PDU_TYPE_L2HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_set_global_action_by_l2hdr_proto(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_set_global_action_by_macda(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA_LOW24:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_set_global_action_by_macda_low24(index, action));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

extern int32
sys_humber_l2pdu_get_global_action(ctc_pdu_l2pdu_type_t l2pdu_type,uint8 index,
                                                                                  ctc_pdu_global_l2pdu_action_t* action)
{
    CTC_PTR_VALID_CHECK(action);

    switch(l2pdu_type)
    {
        case CTC_PDU_L2PDU_TYPE_L2HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_global_action_by_l2hdr_proto(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_global_action_by_macda(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_MACDA_LOW24:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_global_action_by_macda_low24(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_L3TYPE:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_global_action_by_layer3_type(index, action));
            break;
        case CTC_PDU_L2PDU_TYPE_BPDU:
            CTC_ERROR_RETURN(_sys_humber_l2pdu_get_global_action_by_bpdu(0, action));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

/**
 @brief  Per port control layer2 pdu action
*/
extern int32
sys_humber_l2pdu_set_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t action)
{
    uint16 excp2_en = 0;
    uint16 excp2_discard = 0;

    if( action_index > MAX_SYS_L2PDU_PER_PORT_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }

    switch(action)
    {
        case CTC_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU:
            /*excption2En set, exception2Discard set*/
            CTC_ERROR_RETURN(sys_humber_port_get_exception_en(gport, &excp2_en));
            CTC_SET_FLAG(excp2_en, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_en(gport, excp2_en));

            CTC_ERROR_RETURN(sys_humber_port_get_exception_discard(gport, &excp2_discard));
            CTC_SET_FLAG(excp2_discard, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_discard(gport, excp2_discard));
            break;
        case CTC_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU:
            /*excption2En set , exception2Discard clear*/
            CTC_ERROR_RETURN(sys_humber_port_get_exception_en(gport, &excp2_en));
            CTC_SET_FLAG(excp2_en, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_en(gport, excp2_en));

            CTC_ERROR_RETURN(sys_humber_port_get_exception_discard(gport, &excp2_discard));
            CTC_UNSET_FLAG(excp2_discard, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_discard(gport, excp2_discard));

            break;
        case CTC_PDU_L2PDU_ACTION_TYPE_FWD:
            /*excption2En clear , exception2Discard clear*/
            CTC_ERROR_RETURN(sys_humber_port_get_exception_en(gport, &excp2_en));
            CTC_UNSET_FLAG(excp2_en, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_en(gport, excp2_en));

            CTC_ERROR_RETURN(sys_humber_port_get_exception_discard(gport, &excp2_discard));
            CTC_UNSET_FLAG(excp2_discard, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_discard(gport, excp2_discard));
            break;
        case CTC_PDU_L2PDU_ACTION_TYPE_DISCARD:
            /*excption2En clear , exception2Discard set*/
            CTC_ERROR_RETURN(sys_humber_port_get_exception_en(gport, &excp2_en));
            CTC_UNSET_FLAG(excp2_en, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_en(gport, excp2_en));

            CTC_ERROR_RETURN(sys_humber_port_get_exception_discard(gport, &excp2_discard));
            CTC_SET_FLAG(excp2_discard, (1 << action_index));
            CTC_ERROR_RETURN(sys_humber_port_set_exception_discard(gport, excp2_discard));
            break;
        default:
            return CTC_E_INVALID_PARAM;
            break;
    }

    return CTC_E_NONE;
}

extern int32
sys_humber_l2pdu_get_port_action(uint16 gport, uint8 action_index,
                                                                             ctc_pdu_port_l2pdu_action_t *action)
{
    uint16 excp2_en = 0;
    uint16 excp2_discard = 0;

    uint8 excp2_en_flag = 0;
    uint8 excp2_discard_flag = 0;

    if( action_index > MAX_SYS_L2PDU_PER_PORT_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }

    /*excption2En set, exception2Discard set*/
    CTC_ERROR_RETURN(sys_humber_port_get_exception_en(gport, &excp2_en));
    if(CTC_FLAG_ISSET(excp2_en, (1 << action_index)))
    {
        excp2_en_flag = 1;
    }
    CTC_ERROR_RETURN(sys_humber_port_get_exception_discard(gport, &excp2_discard));
    if(CTC_FLAG_ISSET(excp2_discard, (1 << action_index)))
    {
        excp2_discard_flag = 1;
    }
    if(excp2_en_flag && excp2_discard_flag )
    {
        *action = CTC_PDU_L2PDU_ACTION_TYPE_REDIRECT_TO_CPU;
    }
    else if(excp2_en_flag)
    {
        *action = CTC_PDU_L2PDU_ACTION_TYPE_COPY_TO_CPU;
    }
    else if(excp2_discard_flag)
    {
        *action = CTC_PDU_L2PDU_ACTION_TYPE_DISCARD;
    }
    else
    {
        *action = CTC_PDU_L2PDU_ACTION_TYPE_FWD;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_classify_l3pdu_by_l3hdr_proto(uint8 index, ctc_pdu_l3pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL1
                      - IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL0;
    uint32 l3_header_protocol = 0;
    uint32 l3_header_protocol_id = IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_L3HDR_PROTO)
    {
        return CTC_E_INVALID_INDEX;
    }

    l3_header_protocol_id += index_step * index;
    l3_header_protocol = entry->l3hdr_proto;

    local_chip_num = sys_humber_get_local_chip_num();
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM, l3_header_protocol_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &l3_header_protocol));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_get_classified_key_by_l3hdr_proto(uint8 index, ctc_pdu_l3pdu_key_t* entry)
{
    uint32 cmd = 0;
    int8   index_step = IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL1
                      - IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL0;
    uint32 l3_header_protocol = 0;
    uint32 l3_header_protocol_id = IPE_EXCEPTION3_CAM_L3_HEADER_PROTOCOL0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_L3HDR_PROTO)
    {
        return CTC_E_INVALID_INDEX;
    }

    l3_header_protocol_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM, l3_header_protocol_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &l3_header_protocol));

    entry->l3hdr_proto = l3_header_protocol;

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_classify_l3pdu_by_layer4_port(uint8 index, ctc_pdu_l3pdu_key_t* entry)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_EXCEPTION3_CAM2_IS_UDP_MASK1
                      - IPE_EXCEPTION3_CAM2_IS_UDP_MASK0;
    uint32 dest_port = 0;
    uint32 is_tcp_mask = 0;
    uint32 is_tcp_value = 0;
    uint32 is_udp_mask = 0;
    uint32 is_udp_value = 0;
    uint32 dest_port_field_id = IPE_EXCEPTION3_CAM2_DEST_PORT0;
    uint32 is_tcp_mask_field_id = IPE_EXCEPTION3_CAM2_IS_TCP_MASK0;
    uint32 is_tcp_value_field_id = IPE_EXCEPTION3_CAM2_IS_TCP_VALUE0;
    uint32 is_udp_mask_field_id = IPE_EXCEPTION3_CAM2_IS_UDP_MASK0;
    uint32 is_udp_value_field_id = IPE_EXCEPTION3_CAM2_IS_UDP_VALUE0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_PORT)
    {
        return CTC_E_INVALID_INDEX;
    }
    if (entry->l3pdu_by_port.is_tcp &&  entry->l3pdu_by_port.is_udp)
    {
        return CTC_E_INVALID_PARAM;
    }

    dest_port_field_id += index_step * index;
    is_tcp_mask_field_id += index_step * index;
    is_tcp_value_field_id += index_step * index;
    is_udp_mask_field_id += index_step * index;
    is_udp_value_field_id += index_step * index;

    dest_port = entry->l3pdu_by_port.dest_port;
    is_tcp_mask = 1;
    is_tcp_value = entry->l3pdu_by_port.is_tcp?1:0;
    is_udp_mask = 1;
    is_udp_value = entry->l3pdu_by_port.is_udp?1:0;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, dest_port_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &dest_port));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, is_tcp_mask_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &is_tcp_mask));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, is_tcp_value_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &is_tcp_value));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, is_udp_mask_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &is_udp_mask));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, is_udp_value_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &is_udp_value));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_get_classified_key_by_layer4_port(uint8 index, ctc_pdu_l3pdu_key_t* entry)
{
    uint32 cmd = 0;
    int8   index_step = IPE_EXCEPTION3_CAM2_IS_UDP_MASK1
                      - IPE_EXCEPTION3_CAM2_IS_UDP_MASK0;
    uint32 dest_port = 0;
    uint32 is_tcp = 0;
    uint32 is_udp = 0;
    uint32 dest_port_field_id = IPE_EXCEPTION3_CAM2_DEST_PORT0;
    uint32 is_tcp_field_id = IPE_EXCEPTION3_CAM2_IS_TCP_VALUE0;
    uint32 is_udp_field_id = IPE_EXCEPTION3_CAM2_IS_UDP_VALUE0;

    if (index >= MAX_SYS_PDU_L3PDU_BASED_PORT)
    {
        return CTC_E_INVALID_INDEX;
    }

    dest_port_field_id += index_step * index;
    is_tcp_field_id += index_step * index;
    is_udp_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM2, dest_port_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &dest_port));

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM2, is_tcp_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &is_tcp));

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM2, is_udp_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &is_udp));

    entry->l3pdu_by_port.dest_port = dest_port;
    entry->l3pdu_by_port.is_tcp = is_tcp;
    entry->l3pdu_by_port.is_udp = is_udp;

    return CTC_E_NONE;
}

/**
 @brief  Classify layer3 pdu based on layer3 header protocol, layer4 dest port
*/
extern int32
sys_humber_l3pdu_classify_l3pdu(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index,
                                                                           ctc_pdu_l3pdu_key_t* key)
{
    CTC_PTR_VALID_CHECK(key);
    switch(l3pdu_type)
    {
        case CTC_PDU_L3PDU_TYPE_L3HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_classify_l3pdu_by_l3hdr_proto(index, key));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_PORT:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_classify_l3pdu_by_layer4_port(index, key));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

extern int32
sys_humber_l3pdu_get_classified_key(ctc_pdu_l3pdu_type_t l3pdu_type, uint8 index, ctc_pdu_l3pdu_key_t* key)
{
    CTC_PTR_VALID_CHECK(key);
    switch(l3pdu_type)
    {
        case CTC_PDU_L3PDU_TYPE_L3HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_get_classified_key_by_l3hdr_proto(index, key));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_PORT:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_get_classified_key_by_layer4_port(index, key));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_set_global_action_by_l3hdr_proto(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    int8   index_step = IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID1
                      - IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID0;
    uint32 entry_valid_field_id = IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_EXCEPTION3_CAM_RESULT_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_L3HDR_PROTO)
    {
        return CTC_E_INVALID_INDEX;
    }

    entry_valid = ctl->entry_valid?1:0;
    exception_sub_index = ctl->action_index;

    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    local_chip_num = sys_humber_get_local_chip_num();

    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM_RESULT, entry_valid_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &entry_valid));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM_RESULT, exception_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception_sub_index));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_get_global_action_by_l3hdr_proto(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    int8   index_step = IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID1
                      - IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID0;
    uint32 entry_valid_field_id = IPE_EXCEPTION3_CAM_RESULT_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_EXCEPTION3_CAM_RESULT_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_L3HDR_PROTO)
    {
        return CTC_E_INVALID_INDEX;
    }

    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM_RESULT, entry_valid_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &entry_valid));

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM_RESULT, exception_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception_sub_index));

    ctl->entry_valid = entry_valid;
    ctl->action_index = exception_sub_index;

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_set_global_action_by_layer4_port(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID1
                      - IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 entry_valid_field_id = IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_EXCEPTION3_CAM2_RESULT_CAM2_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_PORT)
    {
        return CTC_E_INVALID_INDEX;
    }

    entry_valid = ctl->entry_valid?1:0;
    exception_sub_index = ctl->action_index;

    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    local_chip_num = sys_humber_get_local_chip_num();
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2_RESULT, entry_valid_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &entry_valid));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2_RESULT, exception_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception_sub_index));
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_get_global_action_by_layer4_port(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID1
                      - IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID0;
    uint32 entry_valid = 0;
    uint32 exception_sub_index = 0;
    uint32 entry_valid_field_id = IPE_EXCEPTION3_CAM2_RESULT_CAM2_ENTRY_VALID0;
    uint32 exception_sub_index_field_id = IPE_EXCEPTION3_CAM2_RESULT_CAM2_EXCEPTION_SUB_INDEX0;

    if(index >= MAX_SYS_PDU_L3PDU_BASED_PORT)
    {
        return CTC_E_INVALID_INDEX;
    }
    entry_valid_field_id += index_step * index;
    exception_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM2_RESULT, entry_valid_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &entry_valid));

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CAM2_RESULT, exception_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception_sub_index));

    ctl->entry_valid = entry_valid;
    ctl->action_index = exception_sub_index;

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_set_global_action_by_layer4_type(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    uint8  chip_id = 0;
    uint8  local_chip_num = 0;
    int8   index_step = IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX1
                      - IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX0;
    uint32 exception3_sub_index_field_id = IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX0;
    uint32 exception3_sub_index = 0;

    ipe_exception3_ctl_t escap_ctl;
    kal_memset(&escap_ctl, 0, sizeof(ipe_exception3_ctl_t));

    CTC_PTR_VALID_CHECK(ctl);
    if(index >= MAX_CTC_PARSER_L4_TYPE)
    {
        return CTC_E_INVALID_INDEX;
    }

    exception3_sub_index_field_id += index_step * index;
    exception3_sub_index = ctl->action_index;

    local_chip_num = sys_humber_get_local_chip_num();
    for(chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));

        if (ctl->entry_valid)
        {
            escap_ctl.protocol_exception_en |= (1 <<  index);
        }
        else
        {
            escap_ctl.protocol_exception_en &= (~(1 <<  index));
        }

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CTL, exception3_sub_index_field_id);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &exception3_sub_index));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_get_global_action_by_layer4_type(uint8 index, ctc_pdu_global_l3pdu_action_t* ctl)
{
    uint32 cmd = 0;
    int8   index_step = IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX1
                      - IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX0;
    uint32 exception3_sub_index = 0;
    uint32 exception3_sub_index_field_id = IPE_EXCEPTION3_CTL_PROTOCOL_EXCEPTION3_SUB_INDEX0;

    ipe_exception3_ctl_t escap_ctl;
    kal_memset(&escap_ctl, 0, sizeof(ipe_exception3_ctl_t));

    if(index >= MAX_CTC_PARSER_L4_TYPE)
    {
        return CTC_E_INVALID_INDEX;
    }

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &escap_ctl));

    ctl->entry_valid = (escap_ctl.protocol_exception_en >> index) & 0x1;

    exception3_sub_index_field_id += index_step * index;

    cmd = DRV_IOR(IOC_REG, IPE_EXCEPTION3_CTL, exception3_sub_index_field_id);
    CTC_ERROR_RETURN(drv_reg_ioctl(0, 0, cmd, &exception3_sub_index));
    ctl->action_index = exception3_sub_index;

    return CTC_E_NONE;
}

/**
 @brief  Set layer3 pdu global property
*/
extern int32
sys_humber_l3pdu_set_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index, ctc_pdu_global_l3pdu_action_t* action)
{
    CTC_PTR_VALID_CHECK(action);

    if( action->action_index > MAX_SYS_L3PDU_PER_L3IF_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }

    switch(l3pdu_type)
    {
        case CTC_PDU_L3PDU_TYPE_L3HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_set_global_action_by_l3hdr_proto(index, action));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_PORT:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_set_global_action_by_layer4_port(index, action));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_TYPE:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_set_global_action_by_layer4_type(index, action));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

extern int32
sys_humber_l3pdu_get_global_action(ctc_pdu_l3pdu_type_t l3pdu_type,uint8 index, ctc_pdu_global_l3pdu_action_t* action)
{
    switch(l3pdu_type)
    {
        case CTC_PDU_L3PDU_TYPE_L3HDR_PROTO:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_get_global_action_by_l3hdr_proto(index, action));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_PORT:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_get_global_action_by_layer4_port(index, action));
            break;
        case CTC_PDU_L3PDU_TYPE_LAYER4_TYPE:
            CTC_ERROR_RETURN(_sys_humber_l3pdu_get_global_action_by_layer4_type(index, action));
            break;
        default:
            break;
    }
    return CTC_E_NONE;
}

/**
 @brief  Per layer3 interface control layer3 pdu action
*/
extern int32
sys_humber_l3pdu_set_l3if_action(uint16 l3ifid, uint8 action_index, ctc_pdu_l3if_l3pdu_action_t action)
{
    if( action_index > MAX_SYS_L3PDU_PER_L3IF_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }

    switch(action)
    {
        case CTC_PDU_L3PDU_ACTION_TYPE_FWD:
            CTC_ERROR_RETURN(sys_humber_l3if_set_exception3_en(l3ifid, action_index, 0));
            break;
        case CTC_PDU_L3PDU_ACTION_TYPE_COPY_TO_CPU:
            CTC_ERROR_RETURN(sys_humber_l3if_set_exception3_en(l3ifid, action_index, 1));
            break;
        default:
            break;
    }

    return CTC_E_NONE;
}

int32
sys_humber_l3pdu_get_l3if_action(uint16 l3ifid, uint8 action_index, ctc_pdu_l3if_l3pdu_action_t *action)
{

    bool enbale = 0;

    if(action_index > MAX_SYS_L3PDU_PER_L3IF_ACTION_INDEX)
    {
        return CTC_E_INVALID_ACTION_INDEX;
    }

    CTC_ERROR_RETURN(sys_humber_l3if_get_exception3_en( l3ifid,  action_index, &enbale));

    if(enbale == TRUE)
    {
        *action = CTC_PDU_L3PDU_ACTION_TYPE_COPY_TO_CPU;
    }
    else
    {
         *action = CTC_PDU_L3PDU_ACTION_TYPE_FWD;
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l2pdu_exception_enable_init()
{
    ipe_bpdu_escape_ctl_t bpdu_ctl;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 cmd = 0;

    kal_memset(&bpdu_ctl, 0, sizeof(ipe_bpdu_escape_ctl_t));

    local_chip_num = sys_humber_get_local_chip_num();
    cmd = DRV_IOW(IOC_REG, IPE_BPDU_ESCAPE_CTL, DRV_ENTRY_FLAG);

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        /* bpdu escape is disable */
        bpdu_ctl.bpdu_escape_en = 0x1B;
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &bpdu_ctl));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_exception_enable_init()
{
    uint8 local_chip_num = 0;
    ipe_exception3_ctl_t escap_ctl;
    uint8 chip_id = 0;
    uint32 cmd = 0;

    kal_memset(&escap_ctl, 0, sizeof(ipe_exception3_ctl_t));

    local_chip_num = sys_humber_get_local_chip_num();

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        escap_ctl.exception_cam_en = 1;
        escap_ctl.exception_cam_en2 = 1;

        cmd = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CTL, DRV_ENTRY_FLAG);
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &escap_ctl));
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_l2pdu_cam_init()
{
    int32 cmd_cam, cmd_rst, cmd_cam2, cmd_rst2, cmd_cam3, cmd_rst3;
    uint8  local_chip_num;
    uint8 chip_id = 0;
    ipe_bpdu_protocol_escape_cam_t cam;
    ipe_bpdu_protocol_escape_cam2_t cam2;
    ipe_bpdu_protocol_escape_cam3_t cam3;
    ipe_bpdu_protocol_escape_cam_result_t cam_rst;
    ipe_bpdu_protocol_escape_cam_result2_t cam2_rst;
    ipe_bpdu_protocol_escape_cam_result3_t cam3_rst;

    kal_memset(&cam, 0, sizeof(ipe_bpdu_protocol_escape_cam_t));
    kal_memset(&cam2, 0, sizeof(ipe_bpdu_protocol_escape_cam2_t));
    kal_memset(&cam3, 0, sizeof(ipe_bpdu_protocol_escape_cam3_t));

    kal_memset(&cam_rst, 0, sizeof(ipe_bpdu_protocol_escape_cam_result_t));
    kal_memset(&cam2_rst, 0, sizeof(ipe_bpdu_protocol_escape_cam_result2_t));
    kal_memset(&cam3_rst, 0, sizeof(ipe_bpdu_protocol_escape_cam_result3_t));

    local_chip_num = sys_humber_get_local_chip_num();

    cmd_cam = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM, DRV_ENTRY_FLAG);
    cmd_rst = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT, DRV_ENTRY_FLAG);

    cmd_cam2 = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM2, DRV_ENTRY_FLAG);
    cmd_rst2 = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT2, DRV_ENTRY_FLAG);

    cmd_cam3 = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM3, DRV_ENTRY_FLAG);
    cmd_rst3 = DRV_IOW(IOC_REG, IPE_BPDU_PROTOCOL_ESCAPE_CAM_RESULT3, DRV_ENTRY_FLAG);

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_cam, &cam));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_rst, &cam_rst));

        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_cam2, &cam2));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_rst2, &cam2_rst));

        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_cam3, &cam3));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_rst3, &cam3_rst));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_l3pdu_cam_init()
{
     int32 cmd_cam, cmd_rst, cmd_cam2, cmd_rst2;
     uint8 chip_id = 0;
     uint8  local_chip_num;

     ipe_exception3_cam_t cam;
     ipe_exception3_cam_result_t cam_rst;

     ipe_exception3_cam2_t cam2;
     ipe_exception3_cam2_result_t cam2_rst;

     kal_memset(&cam, 0, sizeof(ipe_exception3_cam_t));
     kal_memset(&cam_rst, 0, sizeof(ipe_exception3_cam_result_t));

     kal_memset(&cam2, 0, sizeof(ipe_exception3_cam2_t));
     kal_memset(&cam2_rst, 0, sizeof(ipe_exception3_cam2_result_t));

     cmd_cam = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM, DRV_ENTRY_FLAG);
     cmd_rst = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM_RESULT, DRV_ENTRY_FLAG);

     cmd_cam2 = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2, DRV_ENTRY_FLAG);
     cmd_rst2 = DRV_IOW(IOC_REG, IPE_EXCEPTION3_CAM2_RESULT, DRV_ENTRY_FLAG);

     local_chip_num = sys_humber_get_local_chip_num();

     for (chip_id = 0; chip_id < local_chip_num; chip_id++)
     {
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_cam, &cam));
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_rst, &cam_rst));
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_cam2, &cam2));
            CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd_rst2, &cam2_rst));
     }

     return CTC_E_NONE;
}

/**
 @brief init pdu module
*/
extern int32
sys_humber_pdu_init(void)
{
    CTC_ERROR_RETURN(_sys_humber_l2pdu_exception_enable_init());

    CTC_ERROR_RETURN(_sys_humber_l2pdu_cam_init());

    CTC_ERROR_RETURN(_sys_humber_l3pdu_exception_enable_init());

    CTC_ERROR_RETURN(_sys_humber_l3pdu_cam_init());

    return CTC_E_NONE;
}
