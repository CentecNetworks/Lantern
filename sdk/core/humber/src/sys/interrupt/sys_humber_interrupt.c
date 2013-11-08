/**
 @file sys_humber_interrupt.c

 @date 2010-1-20

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"
#include "ctc_macro.h"
#include "ctc_interrupt.h"
#include "sys_humber_interrupt.h"
#include "sys_humber_ftm.h"
#include "sys_humber_chip.h"

#include "drv_io.h"
#include "drv_humber_data_path.h"
#include "drv_humber_parity_error.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define INTR_TYPE_JUDGE(type, mask) (((type)&(mask)) == (mask))

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
uint8 normal_interrupt_init = 0;
uint8 fatal_interrupt_init = 0;


#define SYS_NORMAL_INTERRUPT_INIT_CHECK() \
    {\
        if(  normal_interrupt_init == 0)\
            return CTC_E_NOT_INIT;\
    }

#define SYS_FATAL_INTERRUPT_INIT_CHECK() \
    {\
        if(  fatal_interrupt_init == 0)\
            return CTC_E_NOT_INIT;\
    }

#define SYS_PARITY_ERROR_LOG(tbl_id, sram_type) \
    {\
        status->is_parity_error_occur = TRUE;\
        status->parity_error_sram_type = sram_type;\
        status->parity_error_static_tbl_id = tbl_id;\
        status->parity_error_index = parity_fail_addr;\
        status->parity_error_asic_addr = info.asic_addr;\
        kal_memcpy(status->parity_error_asic_data, info.asic_data, sizeof(info.asic_data));\
        status->parity_error_mapping_addr = info.mapping_addr;\
        kal_memcpy(status->parity_error_mapping_data, info.mapping_data, sizeof(info.mapping_data));\
    }


enum sys_interrupt_parity_error_sram_type
{
    SYS_INTERRUPT_SRAM_16K_SIZE = 1,
    SYS_INTERRUPT_SRAM_48K_SIZE,
    SYS_INTERRUPT_SRAM_96K_SIZE,
    SYS_INTERRUPT_EXT_SRAM_256K_SIZE
};
typedef enum sys_interrupt_parity_error_sram_type sys_interrupt_parity_error_sram_type_e;

/****************************************************************************
 *
* Function
*
*****************************************************************************/
int32
sys_humber_interrupt_get_normal_intr_status(uint8 lchip, uint8 type, bool* p_enable)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    CTC_PTR_VALID_CHECK(p_enable);
    SYS_NORMAL_INTERRUPT_INIT_CHECK();

    cmd = DRV_IOR(IOC_REG, HUMBERSUP_NORMAL_INTR_VALUE_SET, HUMBERSUP_NORMAL_INTR_VALUE_SET_NORMAL_INTR_VALUE_SET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d\n",lchip, tmp);

    if (IS_BIT_SET(tmp, type))
    {
        *p_enable = TRUE;
    }
    else
    {
        *p_enable = FALSE;
    }
    return CTC_E_NONE;


}

int32
sys_humber_interrupt_get_all_normal_intr_status(uint8 lchip, uint32* p_bitmap)
{
    uint32 cmd = 0;

    CTC_PTR_VALID_CHECK(p_bitmap);
    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;

    cmd = DRV_IOR(IOC_REG, HUMBERSUP_NORMAL_INTR_VALUE_SET, HUMBERSUP_NORMAL_INTR_VALUE_SET_NORMAL_INTR_VALUE_SET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, p_bitmap));

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_enable_normal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d\n",lchip, type);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_RESET_NORMAL, STATISTICS_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_MASK_RESET, IPEAGING_NORMAL_INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /*disable bit 5, bit 5 always be 1 in PTP_INTERRUPT_VALUE_SET to send intr to normal interrupt*/
            tmp = 0x20;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_MASK_RESET_NORMAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_ALL:
            tmp = 0x3ffff;
            cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_RESET, HUMBERSUP_NORMAL_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            return CTC_E_NONE;
        default:
            return CTC_E_INVALID_PARAM;
    }

    /*finally enable nomarl interrupt for special intr*/
    cmd = DRV_IOR(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_RESET, HUMBERSUP_NORMAL_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    tmp |= 1<<type;
    cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_RESET, HUMBERSUP_NORMAL_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_disable_normal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d\n",lchip, type);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_SET_NORMAL, STATISTICS_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_MASK_SET, IPEAGING_NORMAL_INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_MASK_SET_NORMAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_ALL:
            tmp = 0x3ffff;
            cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_SET, HUMBERSUP_NORMAL_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            return CTC_E_NONE;
        default:
            return CTC_E_INVALID_PARAM;
    }

    /*finally disable nomarl interrupt for special intr*/
    cmd = DRV_IOR(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_SET, HUMBERSUP_NORMAL_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    tmp |= 1<<type;
    cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_SET, HUMBERSUP_NORMAL_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_clear_normal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d\n",lchip, type);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_VALUE_RESET_NORMAL, STATISTICS_INTR_VALUE_RESET_NORMAL_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_VALUE_RESET, IPEAGING_NORMAL_INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_VALUE_RESET_NORMAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    /*finally clear nomarl interrupt for special intr*/
    cmd = DRV_IOR(IOC_REG, HUMBERSUP_NORMAL_INTR_VALUE_RESET, HUMBERSUP_NORMAL_INTR_VALUE_RESET_NORMAL_INTR_VALUE_RESET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    tmp |= 1<<type;
    cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_VALUE_RESET, HUMBERSUP_NORMAL_INTR_VALUE_RESET_NORMAL_INTR_VALUE_RESET);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_enable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d, bit offset:%d\n",lchip, type, bit_offset);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            CTC_MAX_VALUE_CHECK(bit_offset, 2);

            cmd = DRV_IOR(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp |= 1<<bit_offset;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_clear_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d, bit offset:%d\n",lchip, type, bit_offset);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            CTC_MAX_VALUE_CHECK(bit_offset, 2);

            cmd = DRV_IOR(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp |= 1<<bit_offset;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_disable_sub_normal_intr(uint8 lchip, ctc_interrupt_normal_intr_type_t type, uint8 bit_offset)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_NORMAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d, bit offset:%d\n",lchip, type, bit_offset);

    switch(type)
    {
        case CTC_INTERRUPT_NORMAL_PTP_INTR:
            CTC_MAX_VALUE_CHECK(bit_offset, 2);

            cmd = DRV_IOR(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp |= 1<<bit_offset;
            cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_NORMAL_OAM_INTR:
        case CTC_INTERRUPT_NORMAL_STATISTICS_INTR:
        case CTC_INTERRUPT_NORMAL_LEARNING_INTR:
        case CTC_INTERRUPT_NORMAL_AGING_INTR:
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_interrupt_enable_fatal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint32 mem_id = 0;
    uint32 field_id = 0;

    switch(type)
    {
        case CTC_INTERRUPT_FATAL0_MAC_MUX_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, MUX_AGG0_INTERRUPT_FATAL, MUX_AGG0_INTERRUPT_FATAL_MASK_RESET0_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG1_INTERRUPT_FATAL, MUX_AGG1_INTERRUPT_FATAL_MASK_RESET1_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG2_INTERRUPT_FATAL, MUX_AGG2_INTERRUPT_FATAL_MASK_RESET2_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG3_INTERRUPT_FATAL, MUX_AGG3_INTERRUPT_FATAL_MASK_RESET3_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_CPU_MAC_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_INTERRUPT, CPUMAC_GMAC_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_TX_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, NET_TX_INTERRUPT, NET_TX_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_RX_INTR:
            tmp = 0x1FF;
            cmd = DRV_IOW(IOC_REG, NET_RX_INTERRUPT, NET_RX_INTERRUPT_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP2_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP3_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP4_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP5_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP6_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP7_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP8_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP9_INTR:
            tmp = 0x3F;
            mem_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL +
                (QUADMACAPP2_QUAD_MAC_APP_INTERRUPT_FATAL-QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL)*(type-CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR);
            field_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_RESET0;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP10_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_GMAC0_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC1_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC2_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC3_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC4_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC5_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC6_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC7_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC8_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC9_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC10_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC11_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC12_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC13_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC14_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC15_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC16_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC17_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC18_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC19_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC20_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC21_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC22_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC23_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC24_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC25_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC26_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC27_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC28_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC29_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC30_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC31_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC32_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC33_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC34_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC35_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC36_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC37_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC38_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC39_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC40_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC41_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC42_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC43_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC44_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC45_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC46_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC47_INTR:
            tmp = 0xFF;
            mem_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT +
                (GMAC1_GMACWRAPPER_GMAC_INTERRUPT-GMAC0_GMACWRAPPER_GMAC_INTERRUPT)*(type-CTC_INTERRUPT_FATAL0_GMAC0_INTR);
            field_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT_MASK_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            /* mask off some bits (pcsRxRateFifoFull,signalDetect) */
            tmp = 0x30;
            mem_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT +
                (GMAC1_GMACWRAPPER_GMAC_INTERRUPT-GMAC0_GMACWRAPPER_GMAC_INTERRUPT)*(type-CTC_INTERRUPT_FATAL0_GMAC0_INTR);
            field_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_STATS_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INTR_MASK_RESET, EPE_STATS_INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_EDIT_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_INTERRUPT_FATAL, EPE_HDR_EDIT_INTERRUPT_FATAL_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_CLASSIFICATION_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_INTERRUPT_FATAL, EPE_CLASSIFICATION_INTERRUPT_FATAL_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_ACLQOS_INTR:
            tmp = 0x3FFF;
            cmd = DRV_IOW(IOC_REG, INTR_MASK_RESET, INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_PROC_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_PROC_INTR_MASK_RESET, EPE_HDR_PROC_INTR_MASK_RESET_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_NEXTHOP_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, NEXT_HOP_INTR_MASK_RESET, NEXT_HOP_INTR_MASK_RESET_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_ADJ_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_ADJUST_INTERRUPT, EPE_HDR_ADJUST_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_EPE_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_IPE_AGING_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, IPE_AGING_INTR_MASK_RESET, IPE_AGING_INTR_MASK_RESET_IPE_AGING_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_STATS_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INTR_MASK_RESET, IPE_STATS_INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_FWD_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_FORWARD_INTERRUPT, IPE_FORWARD_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PKT_PROC_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, INTERRUPT, INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_LKP_MGR_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_INTERRUPT, IPE_LOOKUP_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_INTF_MAPPER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT0, IPE_INTF_MAPPER_INTERRUPT0_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT1, IPE_INTF_MAPPER_INTERRUPT1_MASK_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
            break;
        case CTC_INTERRUPT_FATAL2_IPE_HDR_ADJ_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_INTERRUPT, IPE_HDR_ADJ_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_IPE_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR:
            tmp = 0x7FFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_VOQ_INTERRUPT, FABRIC_VOQ_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* mask off some bit (fabricVoqCellInfoParityError) */
            tmp = 0x40;
            cmd = DRV_IOW(IOC_REG, FABRIC_VOQ_INTERRUPT, FABRIC_VOQ_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_SER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_MASK_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RXQ_INTR:
            tmp = 0x7F;
            cmd = DRV_IOW(IOC_REG, FABRIC_RXQ_INTERRUPT_FATAL, FABRIC_RXQ_INTERRUPT_FATAL_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RTS_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_RTS_INTERRUPT_FATAL, FABRIC_RTS_INTERRUPT_FATAL_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_GTS_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, FABRIC_GTS_INTR_MASK_RESET, FABRIC_GTS_INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_DSF_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_RESET2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CRB_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, FABRIC_CRB_INTERRUPT, FABRIC_CRB_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_MASK_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_SGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = SGMAC0_SGMAC_INTERRUPT_MASK_RESET +
                (SGMAC1_SGMAC_INTERRUPT_MASK_RESET-SGMAC0_SGMAC_INTERRUPT_MASK_RESET)*(type-CTC_INTERRUPT_FATAL2_SGMAC0_INTR);
            field_id = SGMAC0_SGMAC_INTERRUPT_MASK_RESET_MASK_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* mask off some bits (signalDetect 0-3) */
            tmp = 0x1e000;
            mem_id = SGMAC0_SGMAC_INTERRUPT_MASK_SET +
                (SGMAC1_SGMAC_INTERRUPT_MASK_SET-SGMAC0_SGMAC_INTERRUPT_MASK_SET)*(type-CTC_INTERRUPT_FATAL2_SGMAC0_INTR);
            field_id = SGMAC0_SGMAC_INTERRUPT_MASK_SET_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_XGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = XGMAC0_XGMAC_INTERRUPT_MASK_RESET +
                (XGMAC1_XGMAC_INTERRUPT_MASK_RESET-XGMAC0_XGMAC_INTERRUPT_MASK_RESET)*(type-CTC_INTERRUPT_FATAL2_XGMAC0_INTR);
            field_id = XGMAC0_XGMAC_INTERRUPT_MASK_RESET_MASK_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* mask off some bits (signalDetect 0-3) */
            tmp = 0x1e000;
            mem_id = XGMAC0_XGMAC_INTERRUPT_MASK_SET +
                (XGMAC1_XGMAC_INTERRUPT_MASK_SET-XGMAC0_XGMAC_INTERRUPT_MASK_SET)*(type-CTC_INTERRUPT_FATAL2_XGMAC0_INTR);
            field_id = XGMAC0_XGMAC_INTERRUPT_MASK_SET_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_DS_HASH_KEY_TBL_INTR:
            break;

        case CTC_INTERRUPT_FATAL3_HASH_CTL_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_INTERRUPT_FATAL, HASH_DS_CTL_INTERRUPT_FATAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_INTR, TCAM_CTL_EXT_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_INT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_INTR, TCAM_CTL_INT_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_PB_CTL_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, PB_CTL_INTERRUPT, PB_CTL_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QDR_ARB_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, QDR_ARB_INTERRUPT, QDR_ARB_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TB_INFO_ARB_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT0, TB_INFO_ARB_INTERRUPT0_MASK_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT1, TB_INFO_ARB_INTERRUPT1_MASK_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_ARB_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, TCAM_ARB_INTERRUPT, TCAM_ARB_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_SHARED_DS_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, SHARED_DS_INTERRUPT, SHARED_DS_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STP_STATE_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STP_STATE_INTR_MASK_RESET, STP_STATE_INTR_MASK_RESET_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STATISTICS_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_RESET, STATISTICS_INTR_MASK_RESET_INTR_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_POLICING_INTR:
            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR0_MASK_RESET, POLICING_INTR0_MASK_RESET_INTR0_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR1_MASK_RESET, POLICING_INTR1_MASK_RESET_INTR1_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* mask off some bit (Internal.rdPolicerAckFifoDataParityErr, Internal.rdProfileAckFifoDataParityErr) */
            tmp = 0x11000;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR0_MASK_SET, POLICING_INTR0_MASK_SET_INTR0_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_MET_FIFO_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_MASK_RESET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_MASK_RESET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_ELOOP_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, E_LOOP_INTERRUPT_FATAL, E_LOOP_INTERRUPT_FATAL_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_RETRV_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_RETRV_INTERRUPT_FATAL, BUF_RETRV_INTERRUPT_FATAL_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_STORE_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_RESET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_RESET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_RESET_FATAL2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* mask off some bit (bufStoreChannelInfoParityError) */
            tmp = 0x1000000;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_SET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QMGR_QUE_ENTRY_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SUB_CH_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SUB_CH_INTERRUPT, Q_MGR_SUB_CH_INTERRUPT_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SCH_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SCH_INTERRUPT, Q_MGR_SCH_INTERRUPT_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            /* mask off some bits (grpContextMemParityErr, grpCacheMemParityErr, outProfNextQuePtrMemParityErr
                queShpStateMemParityErr, queStateMemParityErr, subChLinkStateMemParityErr) */
            tmp = 0x295800;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SCH_INTERRUPT, Q_MGR_SCH_INTERRUPT_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_LINKLIST_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, Q_MGR_LINK_LIST_INTERRUPT, Q_MGR_LINK_LIST_INTERRUPT_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_ENQ_INTR:
            tmp = 0x3FFFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_ENQ_INTERRUPT, Q_MGR_ENQ_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_OAM_PROC_INTR:
            tmp = 0xFFFF;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_FATAL, OAM_PROC_INTERRUPT_FATAL_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_LKP_INTR:
            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, OAM_LOOKUP_INTERRUPT, OAM_LOOKUP_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_FWD_INTR:
            tmp = 0xFFF;
            cmd = DRV_IOW(IOC_REG, OAM_FWD_INTERRUPT_FATAL, OAM_FWD_INTERRUPT_FATAL_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, OAM_PARSER_INTERRUPT_FATAL, OAM_PARSER_INTERRUPT_FATAL_MASK_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QDR_CTL_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, QDR_CTL_INTERRUPT, QDR_CTL_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_DDR_CTL_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, TB_INFO_EXT_DDR_CTL_INTERRUPT, TB_INFO_EXT_DDR_CTL_INTERRUPT_MASK_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    if(type >= CTC_INTERRUPT_FATAL3_INTR)  /*fatal3 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR3_MASK_RESET, FATAL_INTR3_MASK_RESET_FATAL_INTR3_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL3_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_MASK_RESET, FATAL_INTR3_MASK_RESET_FATAL_INTR3_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL2_INTR)  /*fatal2 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR2_MASK_RESET, FATAL_INTR2_MASK_RESET_FATAL_INTR2_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL2_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_MASK_RESET, FATAL_INTR2_MASK_RESET_FATAL_INTR2_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL1_INTR)  /*fatal1 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR1_MASK_RESET, FATAL_INTR1_MASK_RESET_FATAL_INTR1_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL1_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_MASK_RESET, FATAL_INTR1_MASK_RESET_FATAL_INTR1_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else  /*fatal0 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR0_MASK_RESET, FATAL_INTR0_MASK_RESET_FATAL_INTR0_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<type;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_MASK_RESET, FATAL_INTR0_MASK_RESET_FATAL_INTR0_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_interrupt_disable_fatal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint32 mem_id = 0;
    uint32 field_id = 0;

    switch(type)
    {
        case CTC_INTERRUPT_FATAL0_MAC_MUX_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, MUX_AGG0_INTERRUPT_FATAL, MUX_AGG0_INTERRUPT_FATAL_MASK_SET0_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG1_INTERRUPT_FATAL, MUX_AGG1_INTERRUPT_FATAL_MASK_SET1_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG2_INTERRUPT_FATAL, MUX_AGG2_INTERRUPT_FATAL_MASK_SET2_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG3_INTERRUPT_FATAL, MUX_AGG3_INTERRUPT_FATAL_MASK_SET3_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_CPU_MAC_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_INTERRUPT, CPUMAC_GMAC_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_TX_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, NET_TX_INTERRUPT, NET_TX_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_RX_INTR:
            tmp = 0x1FF;
            cmd = DRV_IOW(IOC_REG, NET_RX_INTERRUPT, NET_RX_INTERRUPT_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP2_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP3_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP4_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP5_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP6_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP7_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP8_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP9_INTR:
            tmp = 0x3F;
            mem_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL +
                (QUADMACAPP2_QUAD_MAC_APP_INTERRUPT_FATAL-QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL)*(type-CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR);
            field_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_SET0;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP10_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL0_GMAC0_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC1_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC2_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC3_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC4_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC5_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC6_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC7_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC8_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC9_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC10_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC11_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC12_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC13_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC14_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC15_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC16_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC17_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC18_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC19_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC20_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC21_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC22_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC23_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC24_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC25_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC26_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC27_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC28_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC29_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC30_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC31_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC32_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC33_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC34_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC35_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC36_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC37_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC38_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC39_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC40_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC41_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC42_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC43_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC44_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC45_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC46_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC47_INTR:
            tmp = 0xFF;
            mem_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT +
                (GMAC1_GMACWRAPPER_GMAC_INTERRUPT-GMAC0_GMACWRAPPER_GMAC_INTERRUPT)*(type-CTC_INTERRUPT_FATAL0_GMAC0_INTR);
            field_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_EPE_STATS_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INTR_MASK_SET, EPE_STATS_INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_EDIT_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_INTERRUPT_FATAL, EPE_HDR_EDIT_INTERRUPT_FATAL_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_CLASSIFICATION_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_INTERRUPT_FATAL, EPE_CLASSIFICATION_INTERRUPT_FATAL_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_ACLQOS_INTR:
            tmp = 0x3FFF;
            cmd = DRV_IOW(IOC_REG, INTR_MASK_SET, INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_PROC_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_PROC_INTR_MASK_SET, EPE_HDR_PROC_INTR_MASK_SET_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_NEXTHOP_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, NEXT_HOP_INTR_MASK_SET, NEXT_HOP_INTR_MASK_SET_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_ADJ_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_ADJUST_INTERRUPT, EPE_HDR_ADJUST_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_EPE_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_IPE_AGING_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, IPE_AGING_INTR_MASK_SET, IPE_AGING_INTR_MASK_SET_IPE_AGING_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_STATS_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INTR_MASK_SET, IPE_STATS_INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_FWD_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_FORWARD_INTERRUPT, IPE_FORWARD_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PKT_PROC_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, INTERRUPT, INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_LKP_MGR_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_INTERRUPT, IPE_LOOKUP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_INTF_MAPPER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT0, IPE_INTF_MAPPER_INTERRUPT0_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT1, IPE_INTF_MAPPER_INTERRUPT1_MASK_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
            break;
        case CTC_INTERRUPT_FATAL2_IPE_HDR_ADJ_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_INTERRUPT, IPE_HDR_ADJ_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_IPE_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR:
            tmp = 0x7FFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_VOQ_INTERRUPT, FABRIC_VOQ_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_SER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_MASK_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RXQ_INTR:
            tmp = 0x7F;
            cmd = DRV_IOW(IOC_REG, FABRIC_RXQ_INTERRUPT_FATAL, FABRIC_RXQ_INTERRUPT_FATAL_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RTS_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_RTS_INTERRUPT_FATAL, FABRIC_RTS_INTERRUPT_FATAL_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_GTS_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, FABRIC_GTS_INTR_MASK_SET, FABRIC_GTS_INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_DSF_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_MASK_SET2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CRB_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, FABRIC_CRB_INTERRUPT, FABRIC_CRB_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_MASK_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_SGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = SGMAC0_SGMAC_INTERRUPT_MASK_SET +
                (SGMAC1_SGMAC_INTERRUPT_MASK_SET-SGMAC0_SGMAC_INTERRUPT_MASK_SET)*(type-CTC_INTERRUPT_FATAL2_SGMAC0_INTR);
            field_id = SGMAC0_SGMAC_INTERRUPT_MASK_SET_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_XGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = XGMAC0_XGMAC_INTERRUPT_MASK_SET +
                (XGMAC1_XGMAC_INTERRUPT_MASK_SET-XGMAC0_XGMAC_INTERRUPT_MASK_SET)*(type-CTC_INTERRUPT_FATAL2_XGMAC0_INTR);
            field_id = XGMAC0_XGMAC_INTERRUPT_MASK_SET_MASK_SET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_DS_HASH_KEY_TBL_INTR:
            break;

        case CTC_INTERRUPT_FATAL3_HASH_CTL_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_INTERRUPT_FATAL, HASH_DS_CTL_INTERRUPT_FATAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_INTR, TCAM_CTL_EXT_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_INT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_INTR, TCAM_CTL_INT_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_PB_CTL_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, PB_CTL_INTERRUPT, PB_CTL_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QDR_ARB_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, QDR_ARB_INTERRUPT, QDR_ARB_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TB_INFO_ARB_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT0, TB_INFO_ARB_INTERRUPT0_MASK_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT1, TB_INFO_ARB_INTERRUPT1_MASK_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_ARB_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, TCAM_ARB_INTERRUPT, TCAM_ARB_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_SHARED_DS_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, SHARED_DS_INTERRUPT, SHARED_DS_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STP_STATE_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STP_STATE_INTR_MASK_SET, STP_STATE_INTR_MASK_SET_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STATISTICS_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_SET, STATISTICS_INTR_MASK_SET_INTR_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_POLICING_INTR:
            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR0_MASK_SET, POLICING_INTR0_MASK_SET_INTR0_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR1_MASK_SET, POLICING_INTR1_MASK_SET_INTR1_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_MET_FIFO_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_MASK_SET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_MASK_SET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_ELOOP_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, E_LOOP_INTERRUPT_FATAL, E_LOOP_INTERRUPT_FATAL_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_RETRV_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_RETRV_INTERRUPT_FATAL, BUF_RETRV_INTERRUPT_FATAL_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_STORE_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_SET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_SET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_MASK_SET_FATAL2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QMGR_QUE_ENTRY_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SUB_CH_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SUB_CH_INTERRUPT, Q_MGR_SUB_CH_INTERRUPT_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SCH_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SCH_INTERRUPT, Q_MGR_SCH_INTERRUPT_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_LINKLIST_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, Q_MGR_LINK_LIST_INTERRUPT, Q_MGR_LINK_LIST_INTERRUPT_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_ENQ_INTR:
            tmp = 0x3FFFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_ENQ_INTERRUPT, Q_MGR_ENQ_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_OAM_PROC_INTR:
            tmp = 0xFFFF;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_FATAL, OAM_PROC_INTERRUPT_FATAL_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_LKP_INTR:
            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, OAM_LOOKUP_INTERRUPT, OAM_LOOKUP_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_FWD_INTR:
            tmp = 0xFFF;
            cmd = DRV_IOW(IOC_REG, OAM_FWD_INTERRUPT_FATAL, OAM_FWD_INTERRUPT_FATAL_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, OAM_PARSER_INTERRUPT_FATAL, OAM_PARSER_INTERRUPT_FATAL_MASK_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QDR_CTL_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, QDR_CTL_INTERRUPT, QDR_CTL_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_DDR_CTL_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, TB_INFO_EXT_DDR_CTL_INTERRUPT, TB_INFO_EXT_DDR_CTL_INTERRUPT_MASK_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    if(type >= CTC_INTERRUPT_FATAL3_INTR)  /*fatal3 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR3_MASK_SET, FATAL_INTR3_MASK_SET_FATAL_INTR3_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL3_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_MASK_SET, FATAL_INTR3_MASK_SET_FATAL_INTR3_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL2_INTR)  /*fatal2 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR2_MASK_SET, FATAL_INTR2_MASK_SET_FATAL_INTR2_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL2_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_MASK_SET, FATAL_INTR2_MASK_SET_FATAL_INTR2_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL1_INTR)  /*fatal1 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR1_MASK_SET, FATAL_INTR1_MASK_SET_FATAL_INTR1_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL1_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_MASK_SET, FATAL_INTR1_MASK_SET_FATAL_INTR1_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else  /*fatal0 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR0_MASK_SET, FATAL_INTR0_MASK_SET_FATAL_INTR0_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<type;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_MASK_SET, FATAL_INTR0_MASK_SET_FATAL_INTR0_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_interrupt_clear_fatal_intr(uint8 lchip, uint8 type)
{
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint32 mem_id = 0;
    uint32 field_id = 0;

    switch(type)
    {
        case CTC_INTERRUPT_FATAL0_MAC_MUX_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, MUX_AGG0_INTERRUPT_FATAL, MUX_AGG0_INTERRUPT_FATAL_VALUE_RESET0_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG1_INTERRUPT_FATAL, MUX_AGG1_INTERRUPT_FATAL_VALUE_RESET1_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG2_INTERRUPT_FATAL, MUX_AGG2_INTERRUPT_FATAL_VALUE_RESET2_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, MUX_AGG3_INTERRUPT_FATAL, MUX_AGG3_INTERRUPT_FATAL_VALUE_RESET3_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_CPU_MAC_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, CPUMAC_GMAC_INTERRUPT, CPUMAC_GMAC_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_TX_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, NET_TX_INTERRUPT, NET_TX_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_NET_RX_INTR:
            tmp = 0x1FF;
            cmd = DRV_IOW(IOC_REG, NET_RX_INTERRUPT, NET_RX_INTERRUPT_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP2_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP3_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP4_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP5_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP6_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP7_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP8_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP9_INTR:
            tmp = 0x3F;
            mem_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL +
                (QUADMACAPP2_QUAD_MAC_APP_INTERRUPT_FATAL-QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL)*(type-CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR);
            field_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_RESET0;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP10_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL0_GMAC0_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC1_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC2_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC3_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC4_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC5_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC6_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC7_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC8_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC9_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC10_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC11_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC12_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC13_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC14_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC15_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC16_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC17_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC18_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC19_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC20_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC21_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC22_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC23_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC24_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC25_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC26_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC27_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC28_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC29_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC30_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC31_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC32_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC33_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC34_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC35_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC36_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC37_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC38_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC39_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC40_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC41_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC42_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC43_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC44_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC45_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC46_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC47_INTR:
            tmp = 0xFF;
            mem_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT +
                (GMAC1_GMACWRAPPER_GMAC_INTERRUPT-GMAC0_GMACWRAPPER_GMAC_INTERRUPT)*(type-CTC_INTERRUPT_FATAL0_GMAC0_INTR);
            field_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT_VALUE_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_EPE_STATS_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INTR_VALUE_RESET, EPE_STATS_INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_EDIT_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_INTERRUPT_FATAL, EPE_HDR_EDIT_INTERRUPT_FATAL_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_CLASSIFICATION_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, EPE_CLASSIFICATION_INTERRUPT_FATAL, EPE_CLASSIFICATION_INTERRUPT_FATAL_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_ACLQOS_INTR:
            tmp = 0x3FFF;
            cmd = DRV_IOW(IOC_REG, INTR_VALUE_RESET, INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_PROC_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_PROC_INTR_VALUE_RESET, EPE_HDR_PROC_INTR_VALUE_RESET_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_NEXTHOP_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, NEXT_HOP_INTR_VALUE_RESET, NEXT_HOP_INTR_VALUE_RESET_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_ADJ_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, EPE_HDR_ADJUST_INTERRUPT, EPE_HDR_ADJUST_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_EPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_EPE_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_IPE_AGING_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, IPE_AGING_INTR_VALUE_RESET, IPE_AGING_INTR_VALUE_RESET_IPE_AGING_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_STATS_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INTR_VALUE_RESET, IPE_STATS_INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_FWD_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_FORWARD_INTERRUPT, IPE_FORWARD_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PKT_PROC_INTR:
            tmp = 0x7FFFFF;
            cmd = DRV_IOW(IOC_REG, INTERRUPT, INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_LKP_MGR_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, IPE_LOOKUP_INTERRUPT, IPE_LOOKUP_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_INTF_MAPPER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT0, IPE_INTF_MAPPER_INTERRUPT0_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, IPE_INTF_MAPPER_INTERRUPT1, IPE_INTF_MAPPER_INTERRUPT1_VALUE_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
            break;
        case CTC_INTERRUPT_FATAL2_IPE_HDR_ADJ_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_HDR_ADJ_INTERRUPT, IPE_HDR_ADJ_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PARSER_INTR:
            tmp = 0x3FFFF;
            cmd = DRV_IOW(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_IPE_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR:
            tmp = 0x7FFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_VOQ_INTERRUPT, FABRIC_VOQ_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_SER_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_VALUE_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RXQ_INTR:
            tmp = 0x7F;
            cmd = DRV_IOW(IOC_REG, FABRIC_RXQ_INTERRUPT_FATAL, FABRIC_RXQ_INTERRUPT_FATAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RTS_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_RTS_INTERRUPT_FATAL, FABRIC_RTS_INTERRUPT_FATAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_GTS_INTR:
            tmp = 0x7FF;
            cmd = DRV_IOW(IOC_REG, FABRIC_GTS_INTR_VALUE_RESET, FABRIC_GTS_INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_DSF_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_RESET2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CRB_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, FABRIC_CRB_INTERRUPT, FABRIC_CRB_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_VALUE_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_SGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = SGMAC0_SGMAC_INTERRUPT_STATUS_RESET +
                (SGMAC1_SGMAC_INTERRUPT_STATUS_RESET-SGMAC0_SGMAC_INTERRUPT_STATUS_RESET)*(type-CTC_INTERRUPT_FATAL2_SGMAC0_INTR);
            field_id = SGMAC0_SGMAC_INTERRUPT_STATUS_RESET_VALUE_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL2_XGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC3_INTR:
            tmp = 0x1FFFF;
            mem_id = XGMAC0_XGMAC_INTERRUPT_STATUS_RESET +
                (XGMAC1_XGMAC_INTERRUPT_STATUS_RESET-XGMAC0_XGMAC_INTERRUPT_STATUS_RESET)*(type-CTC_INTERRUPT_FATAL2_XGMAC0_INTR);
            field_id = XGMAC0_XGMAC_INTERRUPT_STATUS_RESET_VALUE_RESET;
            cmd = DRV_IOW(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_DS_HASH_KEY_TBL_INTR:
            break;
        case CTC_INTERRUPT_FATAL3_HASH_CTL_INTR:
            tmp = 0x3FF;
            cmd = DRV_IOW(IOC_REG, HASH_DS_CTL_INTERRUPT_FATAL, HASH_DS_CTL_INTERRUPT_FATAL_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_EXT_INTR, TCAM_CTL_EXT_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_INT_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, TCAM_CTL_INT_INTR, TCAM_CTL_INT_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_PB_CTL_INTR:
            tmp = 0x3F;
            cmd = DRV_IOW(IOC_REG, PB_CTL_INTERRUPT, PB_CTL_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QDR_ARB_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, QDR_ARB_INTERRUPT, QDR_ARB_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TB_INFO_ARB_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT0, TB_INFO_ARB_INTERRUPT0_VALUE_RESET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, TB_INFO_ARB_INTERRUPT1, TB_INFO_ARB_INTERRUPT1_VALUE_RESET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_ARB_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, TCAM_ARB_INTERRUPT, TCAM_ARB_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_SHARED_DS_INTR:
            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, SHARED_DS_INTERRUPT, SHARED_DS_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STP_STATE_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, STP_STATE_INTR_VALUE_RESET, STP_STATE_INTR_VALUE_RESET_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_STATISTICS_INTR:
            tmp = 0x1FFFFFF;
            cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_VALUE_RESET, STATISTICS_INTR_VALUE_RESET_INTR_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_POLICING_INTR:
            tmp = 0xFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR0_VALUE_RESET, POLICING_INTR0_VALUE_RESET_INTR0_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0x1FFFFFFF;
            cmd = DRV_IOW(IOC_REG, POLICING_INTR1_VALUE_RESET, POLICING_INTR1_VALUE_RESET_INTR1_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_MET_FIFO_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_VALUE_RESET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_VALUE_RESET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_ELOOP_INTR:
            tmp = 0x1;
            cmd = DRV_IOW(IOC_REG, E_LOOP_INTERRUPT_FATAL, E_LOOP_INTERRUPT_FATAL_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_RETRV_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_RETRV_INTERRUPT_FATAL, BUF_RETRV_INTERRUPT_FATAL_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_BUF_STORE_INTR:
            tmp = 0xFFFFFFFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_RESET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_RESET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            tmp = 0xFF;
            cmd = DRV_IOW(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_RESET_FATAL2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QMGR_QUE_ENTRY_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SUB_CH_INTR:
            tmp = 0x1FFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SUB_CH_INTERRUPT, Q_MGR_SUB_CH_INTERRUPT_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SCH_INTR:
            tmp = 0x3FFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_SCH_INTERRUPT, Q_MGR_SCH_INTERRUPT_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_LINKLIST_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, Q_MGR_LINK_LIST_INTERRUPT, Q_MGR_LINK_LIST_INTERRUPT_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_ENQ_INTR:
            tmp = 0x3FFFFFF;
            cmd = DRV_IOW(IOC_REG, Q_MGR_ENQ_INTERRUPT, Q_MGR_ENQ_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_OAM_PROC_INTR:
            tmp = 0xFFFF;
            cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_FATAL, OAM_PROC_INTERRUPT_FATAL_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_LKP_INTR:
            tmp = 0x1FFF;
            cmd = DRV_IOW(IOC_REG, OAM_LOOKUP_INTERRUPT, OAM_LOOKUP_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_FWD_INTR:
            tmp = 0xFFF;
            cmd = DRV_IOW(IOC_REG, OAM_FWD_INTERRUPT_FATAL, OAM_FWD_INTERRUPT_FATAL_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR:
            tmp = 0xF;
            cmd = DRV_IOW(IOC_REG, OAM_PARSER_INTERRUPT_FATAL, OAM_PARSER_INTERRUPT_FATAL_VALUE_RESET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;

        case CTC_INTERRUPT_FATAL3_QDR_CTL_INTR:
            tmp = 0x1F;
            cmd = DRV_IOW(IOC_REG, QDR_CTL_INTERRUPT, QDR_CTL_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        case CTC_INTERRUPT_FATAL3_DDR_CTL_INTR:
            tmp = 0x7;
            cmd = DRV_IOW(IOC_REG, TB_INFO_EXT_DDR_CTL_INTERRUPT, TB_INFO_EXT_DDR_CTL_INTERRUPT_VALUE_RESET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    if(type >= CTC_INTERRUPT_FATAL3_INTR)  /*fatal3 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR3_VALUE_RESET, FATAL_INTR3_VALUE_RESET_FATAL_INTR3_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL3_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_VALUE_RESET, FATAL_INTR3_VALUE_RESET_FATAL_INTR3_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL2_INTR)  /*fatal2 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR2_VALUE_RESET, FATAL_INTR2_VALUE_RESET_FATAL_INTR2_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL2_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_VALUE_RESET, FATAL_INTR2_VALUE_RESET_FATAL_INTR2_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else if(type >= CTC_INTERRUPT_FATAL1_INTR)  /*fatal1 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR1_VALUE_RESET, FATAL_INTR1_VALUE_RESET_FATAL_INTR1_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<(type-CTC_INTERRUPT_FATAL1_INTR);
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_VALUE_RESET, FATAL_INTR1_VALUE_RESET_FATAL_INTR1_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }
    else  /*fatal0 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR0_VALUE_RESET, FATAL_INTR0_VALUE_RESET_FATAL_INTR0_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp |= 1<<type;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_VALUE_RESET, FATAL_INTR0_VALUE_RESET_FATAL_INTR0_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t * status)
{
    uint32 cmd = 0;
    uint32 mem_id = 0;
    uint32 field_id = 0;
    uint32 parity_fail_addr = 0;
    drv_chip_parity_error_info_t info;

    kal_memset(&info, 0, sizeof(info));
    kal_memset(status, 0, sizeof(*status));
    status->action = CTC_INTERRUPT_FATAL_INTR_LOG;

    switch(type)
    {
        case CTC_INTERRUPT_FATAL0_MAC_MUX_INTR:
            cmd = DRV_IOR(IOC_REG, MUX_AGG0_INTERRUPT_FATAL, MUX_AGG0_INTERRUPT_FATAL_VALUE_SET0_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, MUX_AGG1_INTERRUPT_FATAL, MUX_AGG1_INTERRUPT_FATAL_VALUE_SET1_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            cmd = DRV_IOR(IOC_REG, MUX_AGG2_INTERRUPT_FATAL, MUX_AGG2_INTERRUPT_FATAL_VALUE_SET2_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr2));

            cmd = DRV_IOR(IOC_REG, MUX_AGG3_INTERRUPT_FATAL, MUX_AGG3_INTERRUPT_FATAL_VALUE_SET3_INTERRUPT_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr3));

            if((status->sub_fatal_intr0 & 0xFFFF) || (status->sub_fatal_intr1 & 0xFFFF)
                || (status->sub_fatal_intr2 & 0xFFFF) || (status->sub_fatal_intr3 & 0xFFFF))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL0_CPU_MAC_INTR:
            cmd = DRV_IOR(IOC_REG, CPUMAC_GMAC_INTERRUPT, CPUMAC_GMAC_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x2FE)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL0_NET_TX_INTR:
            cmd = DRV_IOR(IOC_REG, NET_TX_INTERRUPT, NET_TX_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x7)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of pktMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x10)
            {
                cmd = DRV_IOR(IOC_REG, NET_TX_PARITY_FAIL_RECORD, NET_TX_PARITY_FAIL_RECORD_PKT_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL0_NET_RX_INTR:
            cmd = DRV_IOR(IOC_REG, NET_RX_INTERRUPT, NET_RX_INTERRUPT_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xDF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of pktBufParityError, only log fail addr */
            if(status->sub_fatal_intr0 & 0x20)
            {
                cmd = DRV_IOR(IOC_REG, NET_RX_PARITY_FAIL_RECORD, NET_RX_PARITY_FAIL_RECORD_PKT_BUF_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;

        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR:
            cmd = DRV_IOR(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP0_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x33FFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP2_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP3_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP4_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP5_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP6_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP7_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP8_INTR:
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP9_INTR:
            mem_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL +
                (QUADMACAPP2_QUAD_MAC_APP_INTERRUPT_FATAL-QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL)*(type-CTC_INTERRUPT_FATAL0_QUAD_MAC_APP1_INTR);
            field_id = QUADMACAPP1_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_SET0;
            cmd = DRV_IOR(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x33FFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP10_INTR:
            cmd = DRV_IOR(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP10_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x33FFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR:
            cmd = DRV_IOR(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL, QUADMACAPP11_QUAD_MAC_APP_INTERRUPT_FATAL_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x33FFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL0_GMAC0_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC1_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC2_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC3_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC4_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC5_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC6_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC7_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC8_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC9_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC10_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC11_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC12_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC13_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC14_INTR:
        case CTC_INTERRUPT_FATAL0_GMAC15_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC16_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC17_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC18_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC19_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC20_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC21_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC22_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC23_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC24_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC25_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC26_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC27_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC28_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC29_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC30_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC31_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC32_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC33_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC34_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC35_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC36_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC37_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC38_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC39_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC40_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC41_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC42_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC43_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC44_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC45_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC46_INTR:
        case CTC_INTERRUPT_FATAL1_GMAC47_INTR:
            mem_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT +
                (GMAC1_GMACWRAPPER_GMAC_INTERRUPT-GMAC0_GMACWRAPPER_GMAC_INTERRUPT)*(type-CTC_INTERRUPT_FATAL0_GMAC0_INTR);
            field_id = GMAC0_GMACWRAPPER_GMAC_INTERRUPT_VALUE_SET;
            cmd = DRV_IOR(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xCE)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;

        case CTC_INTERRUPT_FATAL2_EPE_STATS_INTR:
            cmd = DRV_IOR(IOC_REG, EPE_STATS_INTR_VALUE_SET, EPE_STATS_INTR_VALUE_SET_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1EF3F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_EDIT_INTR:
            cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_INTERRUPT_FATAL, EPE_HDR_EDIT_INTERRUPT_FATAL_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x6003FF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of epeHeaderEditSgmacPriorityMapMemParityError */
            if(status->sub_fatal_intr0 & 0x100000)
            {
                cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_PARITY_FAIL_RECORD,
                    EPE_HDR_EDIT_PARITY_FAIL_RECORD_EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(EPE_HEADER_EDIT_SGMAC_PRIORITY_MAP_MEM, 0);
            }

            /* process parity error of dsL2EditLpbkMemParityError */
            if(status->sub_fatal_intr0 & 0x80000)
            {
                cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_PARITY_FAIL_RECORD,
                    EPE_HDR_EDIT_PARITY_FAIL_RECORD_DS_L2_EDIT_LPBK_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(EPE_HDR_EDIT_L2_EDIT_LOOPBACK_RAM, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_CLASSIFICATION_INTR:
            cmd = DRV_IOR(IOC_REG, EPE_CLASSIFICATION_INTERRUPT_FATAL, EPE_CLASSIFICATION_INTERRUPT_FATAL_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3E)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_ACLQOS_INTR:
            cmd = DRV_IOR(IOC_REG, INTR_VALUE_SET, INTR_VALUE_SET_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3EDE)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_PROC_INTR:
            cmd = DRV_IOR(IOC_REG, EPE_HDR_PROC_INTR_VALUE_SET, EPE_HDR_PROC_INTR_VALUE_SET_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1FF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsL3EditTunnelV4IpSaMemParityError or dsL3EditTunnelV6IpMemParityError */
            if(status->sub_fatal_intr0 & 0x400)
            {
                cmd = DRV_IOR(IOC_REG, DS_L3_EDIT_TUNNEL_V6_IP_MEM_PARITY_CTL, DS_L3_EDIT_TUNNEL_V6_IP_MEM_PARITY_CTL_DS_L3_EDIT_TUNNEL_V6_IP_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_L3_EDIT_TUNNEL_V6_IP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_L3_EDIT_TUNNEL_V6_IP, 0);

                cmd = DRV_IOR(IOC_REG, DS_L3_EDIT_TUNNEL_V4_IP_SA_PARITY_CTL, DS_L3_EDIT_TUNNEL_V4_IP_SA_PARITY_CTL_DS_L3_EDIT_TUNNEL_V4_IP_SA_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_L3_EDIT_TUNNEL_V4_IP_SA, 0, parity_fail_addr, &info));
                if(!info.is_same)
                {
                    SYS_PARITY_ERROR_LOG(DS_L3_EDIT_TUNNEL_V4_IP_SA, 0);
                }
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_NEXTHOP_INTR:
            cmd = DRV_IOR(IOC_REG, NEXT_HOP_INTR_VALUE_SET, NEXT_HOP_INTR_VALUE_SET_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3FF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of editPriorityMapMemRdDataError */
            if(status->sub_fatal_intr0 & 0x10000)
            {
                cmd = DRV_IOR(IOC_REG, PARITY_FAIL_RECORD, PARITY_FAIL_RECORD_EDIT_PRIORITY_MAP_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, EPE_EDIT_PRIORITY_MAP_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(EPE_EDIT_PRIORITY_MAP_TABLE, 0);
            }

            /* process parity error of dsDestPortMemRdDataError */
            if(status->sub_fatal_intr0 & 0x8000)
            {
                cmd = DRV_IOR(IOC_REG, PARITY_FAIL_RECORD, PARITY_FAIL_RECORD_DS_DEST_PORT_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_DEST_PORT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_DEST_PORT, 0);
            }

            /* process parity error of dsDestInterfaceMemRdDataError */
            if(status->sub_fatal_intr0 & 0x4000)
            {
                cmd = DRV_IOR(IOC_REG, PARITY_FAIL_RECORD, PARITY_FAIL_RECORD_DS_DEST_INTERFACE_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_DEST_INTERFACE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_DEST_INTERFACE, 0);
            }

            /* process parity error of dsVplsPortMemRdDataError */
            if(status->sub_fatal_intr0 & 0x2000)
            {
                cmd = DRV_IOR(IOC_REG, PARITY_FAIL_RECORD, PARITY_FAIL_RECORD_DS_VPLS_PORT_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_VPLS_PORT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_VPLS_PORT, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_HDR_ADJ_INTR:
            cmd = DRV_IOR(IOC_REG, EPE_HDR_ADJUST_INTERRUPT, EPE_HDR_ADJUST_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsDestPhyPortParityError */
            if(status->sub_fatal_intr0 & 0x8)
            {
                cmd = DRV_IOR(IOC_REG, EPE_HDR_ADJUST_PARITY_FAIL_RECORD, EPE_HDR_ADJUST_PARITY_FAIL_RECORD_DS_DEST_PHY_PORT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_DEST_PHY_PORT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_DEST_PHY_PORT, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_EPE_PARSER_INTR:
            cmd = DRV_IOR(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_EPE_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x36DBF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL2_IPE_AGING_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_AGING_INTR_VALUE_SET, IPE_AGING_INTR_VALUE_SET_IPE_AGING_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1E)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_STATS_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_STATS_INTR_VALUE_SET, IPE_STATS_INTR_VALUE_SET_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1EF3F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_FWD_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_FORWARD_INTERRUPT, IPE_FORWARD_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1D5A0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsApsBridgeTableParityError */
            if(status->sub_fatal_intr0 & 0x8)
            {
                cmd = DRV_IOR(IOC_REG, IPE_FORWARD_APS_BRIDGE_TABLE_PARITY_FAIL_RECORD,
                    IPE_FORWARD_APS_BRIDGE_TABLE_PARITY_FAIL_RECORD_DS_APS_BRIDGE_TABLE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, APSBRIDGETABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(APSBRIDGETABLE, 0);
            }

            /* process parity error of dsApsSelectTableParityError */
            if(status->sub_fatal_intr0 & 0x4)
            {
                cmd = DRV_IOR(IOC_REG, IPE_FORWARD_APS_BRIDGE_TABLE_PARITY_FAIL_RECORD,
                    IPE_FORWARD_APS_SELECT_TABLE_PARITY_FAIL_RECORD_DS_APS_SELECT_TABLE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, APSSELECTTABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(APSSELECTTABLE, 0);
            }

            /* process parity error of dsFwdExtTableParityError */
            if(status->sub_fatal_intr0 & 0x2)
            {
                cmd = DRV_IOR(IOC_REG, IPE_FORWARD_APS_BRIDGE_TABLE_PARITY_FAIL_RECORD,
                    IPE_FORWARD_FWD_EXT_TABLE_PARITY_FAIL_RECORD_DS_FWD_EXT_TABLE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, FWDEXTTABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(FWDEXTTABLE, 0);
            }

            /* process parity error of dsSequenceNumberTableParityError */
            if(status->sub_fatal_intr0 & 0x1)
            {
                cmd = DRV_IOR(IOC_REG, IPE_FORWARD_APS_BRIDGE_TABLE_PARITY_FAIL_RECORD,
                    IPE_FORWARD_SEQUENCE_NUMBER_TABLE_PARITY_FAIL_RECORD_DS_SEQUENCE_NUMBER_TABLE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, SEQUENCENUMBERTABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(SEQUENCENUMBERTABLE, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PKT_PROC_INTR:
            cmd = DRV_IOR(IOC_REG, INTERRUPT, INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsPbbMacTabParityError */
            if(status->sub_fatal_intr0 & 0x40000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_PKT_PROC_PARITY_FAIL_RECORD, IPE_PKT_PROC_PARITY_FAIL_RECORD_DS_PBB_MAC_TAB_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, IPE_DS_PBB_MAC_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(IPE_DS_PBB_MAC_TABLE, 0);
            }

            /* process parity error of dsBidiPimGroupParityError */
            if(status->sub_fatal_intr0 & 0x20000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_PKT_PROC_PARITY_FAIL_RECORD, IPE_PKT_PROC_PARITY_FAIL_RECORD_DS_BIDI_PIM_GROUP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_BIDI_PIM_GROUP_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_BIDI_PIM_GROUP_TABLE, 0);
            }

            /* process parity error of claCosMapTabParityError */
            if(status->sub_fatal_intr0 & 0x4000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_PKT_PROC_PARITY_FAIL_RECORD, IPE_PKT_PROC_PARITY_FAIL_RECORD_CLA_COS_MAP_TAB_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, IPE_CLASSIFICATION_COS_MAP_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(IPE_CLASSIFICATION_COS_MAP_TABLE, 0);
            }

            /* process parity error of claDscpMapTabParityError */
            if(status->sub_fatal_intr0 & 0x2000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_PKT_PROC_PARITY_FAIL_RECORD, IPE_PKT_PROC_PARITY_FAIL_RECORD_CLA_DSCP_MAP_TAB_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, IPE_CLASSIFICATION_DSCP_MAP_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(IPE_CLASSIFICATION_DSCP_MAP_TABLE, 0);
            }

            /* process parity error of claPreMapTabParityError */
            if(status->sub_fatal_intr0 & 0x1000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_PKT_PROC_PARITY_FAIL_RECORD, IPE_PKT_PROC_PARITY_FAIL_RECORD_CLA_PRE_MAP_TAB_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(IPE_CLASSIFICATION_PRECEDENCE_MAP_TABLE, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_LKP_MGR_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_INTERRUPT, IPE_LOOKUP_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xF7F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsMplsCtlMemParityError */
            if(status->sub_fatal_intr0 & 0x80)
            {
                cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_PARITY_FAIL_ADDR, IPE_LOOKUP_PARITY_FAIL_ADDR_DS_MPLS_CTL_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_MPLS_CTL, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_MPLS_CTL, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_INTF_MAPPER_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_INTERRUPT0, IPE_INTF_MAPPER_INTERRUPT0_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_INTERRUPT1, IPE_INTF_MAPPER_INTERRUPT1_VALUE_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0x1FFFFFF) || (status->sub_fatal_intr1 & 0))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of  dsRouterMacParityError   */
            if(status->sub_fatal_intr0 & 0x40000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_ROUTER_MAC_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_ROUTER_MAC, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_ROUTER_MAC, 0);
            }

            /* process parity error of   dsVrfParityError */
            if(status->sub_fatal_intr0 & 0x20000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_VRF_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_VRF, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_VRF, 0);
            }

            /* process parity error of  dsSrcInterfaceParityError */
            if(status->sub_fatal_intr0 & 0x10000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_SRC_INTERFACE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_SRC_INTERFACE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_SRC_INTERFACE, 0);
            }

            /* process parity error of  dsProtocolVlanParityError */
            if(status->sub_fatal_intr0 & 0x8000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_PROTOCOL_VLAN_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_PROTOCOL_VLAN, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_PROTOCOL_VLAN, 0);
            }

            /* process parity error of  dsSrcPortParityError */
            if(status->sub_fatal_intr0 & 0x4000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_SRC_PORT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_SRC_PORT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_SRC_PORT, 0);
            }

            /* process parity error of  dsPhyPortExtParityError */
            if(status->sub_fatal_intr0 & 0x2000000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_INTF_MAPPER_PARITY_FAIL_RECORD, IPE_INTF_MAPPER_PARITY_FAIL_RECORD_DS_PHY_PORT_EXT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_PHY_PORT_EXT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_PHY_PORT_EXT, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_HDR_ADJ_INTR:
            cmd = DRV_IOR(IOC_REG, IPE_HDR_ADJ_INTERRUPT, IPE_HDR_ADJ_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1000FFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of cmpcResWordRamParityError, only log fail addr*/
            if(status->sub_fatal_intr0 & 0x800000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_HDR_ADJ_PARITY_FAIL_RECORD, IPE_HDR_ADJ_PARITY_FAIL_RECORD_CMPC_RES_WORD_RAM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of msgRamParityErro, only log fail addr*/
            if(status->sub_fatal_intr0 & 0x400000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_HDR_ADJ_PARITY_FAIL_RECORD, IPE_HDR_ADJ_PARITY_FAIL_RECORD_MSG_RAM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of cmpcFirstWordRamParityError, only log fail addr*/
            if(status->sub_fatal_intr0 & 0x200000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_HDR_ADJ_PARITY_FAIL_RECORD, IPE_HDR_ADJ_PARITY_FAIL_RECORD_CMPC_FIRST_WORD_RAM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of dsPhyPortParityError */
            if(status->sub_fatal_intr0 & 0x100000)
            {
                cmd = DRV_IOR(IOC_REG, IPE_HDR_ADJ_PARITY_FAIL_RECORD, IPE_HDR_ADJ_PARITY_FAIL_RECORD_DS_PHY_PORT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_PHY_PORT, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_PHY_PORT, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_IPE_PARSER_INTR:
            cmd = DRV_IOR(IOC_REG, PARSER_INTERRUPT, PARSER_INTERRUPT_IPE_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x36DBF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_VOQ_INTERRUPT, FABRIC_VOQ_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x600F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_SER_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, FABRIC_SER_INTERRUPT, FABRIC_SER_INTERRUPT_VALUE_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0) || (status->sub_fatal_intr1 & 0xFFFFFFFF))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RXQ_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_RXQ_INTERRUPT_FATAL, FABRIC_RXQ_INTERRUPT_FATAL_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x4C)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of wrCtrlResWordParityError, only log fail addr */
            if(status->sub_fatal_intr0 & 0x2)
            {
                cmd = DRV_IOR(IOC_REG, FABRIC_RXQ_PARITY_FAIL_RECORD, FABRIC_RXQ_PARITY_FAIL_RECORD_WR_CTRL_RES_WORD_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of rdCtrlResWordParityError, only log fail addr */
            if(status->sub_fatal_intr0 & 0x1)
            {
                cmd = DRV_IOR(IOC_REG, FABRIC_RXQ_PARITY_FAIL_RECORD, FABRIC_RXQ_PARITY_FAIL_RECORD_RD_CTRL_RES_WORD_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_RTS_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_RTS_INTERRUPT_FATAL, FABRIC_RTS_INTERRUPT_FATAL_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_GTS_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_GTS_INTR_VALUE_SET, FABRIC_GTS_INTR_VALUE_SET_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3C0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_DSF_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            cmd = DRV_IOR(IOC_REG, FABRIC_DSF_INTERRUPT, FABRIC_DSF_INTERRUPT_VALUE_SET2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr2));

            if((status->sub_fatal_intr0 & 0xFFFFFFFF) || (status->sub_fatal_intr1 & 0)
                || (status->sub_fatal_intr2 & 0))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CRB_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_CRB_INTERRUPT, FABRIC_CRB_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR:
            cmd = DRV_IOR(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, FABRIC_CAS_INTERRUPT, FABRIC_CAS_INTERRUPT_VALUE_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0xFFFFFFFF) || (status->sub_fatal_intr1 & 0x9))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL2_SGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_SGMAC3_INTR:
            mem_id = SGMAC0_SGMAC_INTERRUPT_STATUS_SET +
                (SGMAC1_SGMAC_INTERRUPT_STATUS_SET-SGMAC0_SGMAC_INTERRUPT_STATUS_SET)*(type-CTC_INTERRUPT_FATAL2_SGMAC0_INTR);
            field_id = SGMAC0_SGMAC_INTERRUPT_STATUS_SET_VALUE_SET;
            cmd = DRV_IOR(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1EFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;

        case CTC_INTERRUPT_FATAL2_XGMAC0_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC1_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC2_INTR:
        case CTC_INTERRUPT_FATAL2_XGMAC3_INTR:
            mem_id = XGMAC0_XGMAC_INTERRUPT_STATUS_SET +
                (XGMAC1_XGMAC_INTERRUPT_STATUS_SET-XGMAC0_XGMAC_INTERRUPT_STATUS_SET)*(type-CTC_INTERRUPT_FATAL2_XGMAC0_INTR);
            field_id = XGMAC0_XGMAC_INTERRUPT_STATUS_SET_VALUE_SET;
            cmd = DRV_IOR(IOC_REG, mem_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1EFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;
        case CTC_INTERRUPT_FATAL3_DS_HASH_KEY_TBL_INTR:
            break;
        case CTC_INTERRUPT_FATAL3_HASH_CTL_INTR:
            cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_INTERRUPT_FATAL, HASH_DS_CTL_INTERRUPT_FATAL_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of hashTable50KParityErr */
            if(status->sub_fatal_intr0 & 0x3C0)
            {
                cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_PARITY_RECORD, HASH_DS_CTL_PARITY_RECORD_HASH_TABLE50K_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, MAX_TBL_NUM, DRV_MEM_MAPPING_INT_48K_SRAM, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(MAX_TBL_NUM, SYS_INTERRUPT_SRAM_48K_SIZE);
            }

            /* process parity error of hashTable98KParityErr */
            if(status->sub_fatal_intr0 & 0x3C)
            {
                cmd = DRV_IOR(IOC_REG, HASH_DS_CTL_PARITY_RECORD, HASH_DS_CTL_PARITY_RECORD_HASH_TABLE98K_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, MAX_TBL_NUM, DRV_MEM_MAPPING_INT_96K_SRAM, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(MAX_TBL_NUM, SYS_INTERRUPT_SRAM_96K_SIZE);
            }
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR:
            cmd = DRV_IOR(IOC_REG, TCAM_CTL_EXT_INTR, TCAM_CTL_EXT_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_CTL_INT_INTR:
            cmd = DRV_IOR(IOC_REG, TCAM_CTL_INT_INTR, TCAM_CTL_INT_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_PB_CTL_INTR:
            cmd = DRV_IOR(IOC_REG, PB_CTL_INTERRUPT, PB_CTL_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3f)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_QDR_ARB_INTR:
            cmd = DRV_IOR(IOC_REG, QDR_ARB_INTERRUPT, QDR_ARB_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_TB_INFO_ARB_INTR:
            cmd = DRV_IOR(IOC_REG, TB_INFO_ARB_INTERRUPT0, TB_INFO_ARB_INTERRUPT0_VALUE_SET0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, TB_INFO_ARB_INTERRUPT1, TB_INFO_ARB_INTERRUPT1_VALUE_SET1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0xFFFFFFFF) || (status->sub_fatal_intr1 & 0xFFF))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of sramParityError */
            if(status->sub_fatal_intr1 & 0x1000)
            {
                cmd = DRV_IOR(IOC_REG, TB_INFO_ARB_PARITY_FAIL_RECORD, TB_INFO_ARB_PARITY_FAIL_RECORD_SRAM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, MAX_TBL_NUM, DRV_MEM_MAPPING_INT_16K_SRAM, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(MAX_TBL_NUM, SYS_INTERRUPT_SRAM_16K_SIZE);
            }
            break;
        case CTC_INTERRUPT_FATAL3_TCAM_ARB_INTR:
            cmd = DRV_IOR(IOC_REG, TCAM_ARB_INTERRUPT, TCAM_ARB_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3EFFEF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_SHARED_DS_INTR:
            cmd = DRV_IOR(IOC_REG, SHARED_DS_INTERRUPT, SHARED_DS_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsLinkAggrGroupParityError  */
            if(status->sub_fatal_intr0 & 0x80)
            {
                cmd = DRV_IOR(IOC_REG, SHARED_DS_PARITY_FAIL_RECORD, SHARED_DS_PARITY_FAIL_RECORD_DS_LINK_AGGR_GROUP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_LINK_AGGREAGATION_GROUP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_LINK_AGGREAGATION_GROUP, 0);
            }

            /* process parity error of dsVlanTabParityError */
            if(status->sub_fatal_intr0 & 0x40)
            {
                cmd = DRV_IOR(IOC_REG, SHARED_DS_PARITY_FAIL_RECORD, SHARED_DS_PARITY_FAIL_RECORD_DS_VLAN_TAB_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                if(parity_fail_addr < (4096 + 768))
                {
                    CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_VLAN, 0, parity_fail_addr, &info));
                    SYS_PARITY_ERROR_LOG(DS_VLAN, 0);
                }
                else
                {
                    CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_VLAN_STATUS, 0, parity_fail_addr - (4096 + 768), &info));
                    SYS_PARITY_ERROR_LOG(DS_VLAN_STATUS, 0);
                }
            }
            break;

        case CTC_INTERRUPT_FATAL3_STP_STATE_INTR:
            cmd = DRV_IOR(IOC_REG, STP_STATE_INTR_VALUE_SET, STP_STATE_INTR_VALUE_SET_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL3_STATISTICS_INTR:
            cmd = DRV_IOR(IOC_REG, STATISTICS_INTR_VALUE_SET, STATISTICS_INTR_VALUE_SET_INTR_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3FFFFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET_MODULE;
            }
            break;
        case CTC_INTERRUPT_FATAL3_POLICING_INTR:
            cmd = DRV_IOR(IOC_REG, POLICING_INTR0_VALUE_SET, POLICING_INTR0_VALUE_SET_INTR0_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, POLICING_INTR1_VALUE_SET, POLICING_INTR1_VALUE_SET_INTR1_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0xCCF33) || (status->sub_fatal_intr1 & 0x1FEFFFDB))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_MET_FIFO_INTR:
            cmd = DRV_IOR(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_VALUE_SET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, MET_FIFO_INTERRUPT, MET_FIFO_INTERRUPT_VALUE_SET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            if((status->sub_fatal_intr0 & 0xFFF92FFF) || (status->sub_fatal_intr1 & 0))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsApsBridgeMcastParityError */
            if(status->sub_fatal_intr1 & 0x8)
            {
                cmd = DRV_IOR(IOC_REG, MET_FIFO_PARITY_FAIL_RECORD, MET_FIFO_PARITY_FAIL_RECORD_DS_APS_BRIDGE_MCAST_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_APS_BRIDGE_MCAST, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_APS_BRIDGE_MCAST, 0);
            }

            /* process parity error of dsLinkAggBitmapParityError */
            if(status->sub_fatal_intr1 & 0x4)
            {
                cmd = DRV_IOR(IOC_REG, MET_FIFO_PARITY_FAIL_RECORD, MET_FIFO_PARITY_FAIL_RECORD_DS_LINK_AGG_BITMAP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_LINK_AGG_BITMAP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_LINK_AGG_BITMAP, 0);
            }

            /* process parity error of dsLinkAggBlockMaskParityError */
            if(status->sub_fatal_intr1 & 0x2)
            {
                cmd = DRV_IOR(IOC_REG, MET_FIFO_PARITY_FAIL_RECORD, MET_FIFO_PARITY_FAIL_RECORD_DS_LINK_AGG_BLOCK_MASK_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_LINK_AGG_BLOCK_MASK, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_LINK_AGG_BLOCK_MASK, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_ELOOP_INTR:
            cmd = DRV_IOR(IOC_REG, E_LOOP_INTERRUPT_FATAL, E_LOOP_INTERRUPT_FATAL_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of eLoopParityError, only log fail addr */
            if(status->sub_fatal_intr0 & 0x1)
            {
                cmd = DRV_IOR(IOC_REG, E_LOOP_PARITY_FAIL_RECORD, E_LOOP_PARITY_FAIL_RECORD_ELOOP_MEM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_BUF_RETRV_INTR:
            cmd = DRV_IOR(IOC_REG, BUF_RETRV_INTERRUPT_FATAL, BUF_RETRV_INTERRUPT_FATAL_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x900FFFF1)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_BUF_STORE_INTR:
            cmd = DRV_IOR(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_SET_FATAL0);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            cmd = DRV_IOR(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_SET_FATAL1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr1));

            cmd = DRV_IOR(IOC_REG, BUF_STORE_INTERRUPT, BUF_STORE_INTERRUPT_VALUE_SET_FATAL2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr2));

            if((status->sub_fatal_intr0 & 0xFFFFFFFF) || (status->sub_fatal_intr1 & 0xF0F003FF)
                || (status->sub_fatal_intr2 & 0xFF))
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL3_QMGR_QUE_ENTRY_INTR:
            cmd = DRV_IOR(IOC_REG, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT, Q_MGR_TABLE_QUE_ENTRY_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x1C)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SUB_CH_INTR:
            cmd = DRV_IOR(IOC_REG, Q_MGR_SUB_CH_INTERRUPT, Q_MGR_SUB_CH_INTERRUPT_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xFF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_SCH_INTR:
            cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_INTERRUPT, Q_MGR_SCH_INTERRUPT_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x10040F)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of drrDeficitMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x40000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_PARITY_FAIL_RECORD, Q_MGR_SCH_PARITY_FAIL_RECORD_DS_QUEUE_DRR_DEFICIT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of inProfNextQuePtrMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x8000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_PARITY_FAIL_RECORD, Q_MGR_SCH_PARITY_FAIL_RECORD_DS_IN_PROFILE_NEXT_QUEUE_PTR_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of queMapMemParityErr */
            if(status->sub_fatal_intr0 & 0x400)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_PARITY_FAIL_RECORD, Q_MGR_SCH_PARITY_FAIL_RECORD_DS_QUEUE_MAP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_QUEUE_MAP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_QUEUE_MAP, 0);
            }

            /* process parity error of queShpMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x200)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_PARITY_FAIL_RECORD, Q_MGR_SCH_PARITY_FAIL_RECORD_DS_QUEUE_SHAPE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of grpShpMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x40)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_SCH_PARITY_FAIL_RECORD, Q_MGR_SCH_PARITY_FAIL_RECORD_DS_GROUP_SHAPE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_LINKLIST_INTR:
            cmd = DRV_IOR(IOC_REG, Q_MGR_LINK_LIST_INTERRUPT, Q_MGR_LINK_LIST_INTERRUPT_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x7)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of qLinkStateMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x4)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_LINK_LIST_PARITY_FAIL_RECORD, Q_MGR_LINK_LIST_PARITY_FAIL_RECORD_DSQ_LINK_STATE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of qLinkListMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x2)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_LINK_LIST_PARITY_FAIL_RECORD, Q_MGR_LINK_LIST_PARITY_FAIL_RECORD_DSQ_LINK_LIST_ECC_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_QMGR_ENQ_INTR:
            cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_INTERRUPT, Q_MGR_ENQ_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xa003FF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of queDepthMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x2000000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_QUEUE_DEPTH_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }

            /* process parity error of dsQueDropProfIdParityError */
            if(status->sub_fatal_intr0 & 0x1000000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_QUE_DROP_PROF_ID_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_QUEUE_DROP_PROFILE_ID, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_QUEUE_DROP_PROFILE_ID, 0);
            }

            /* process parity error of dsEgressResrcThrdParityError */
            if(status->sub_fatal_intr0 & 0x100000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_EGRESS_RESRC_THRD_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DSQ_MGR_EGRESS_RESRC_THRESHOLD, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DSQ_MGR_EGRESS_RESRC_THRESHOLD, 0);
            }

            /* process parity error of dsHeadHashModParityError  */
            if(status->sub_fatal_intr0 & 0x80000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_HEAD_HASH_MOD_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_HEAD_HASH_MOD, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_HEAD_HASH_MOD, 0);
            }

            /* process parity error of dsLinkAggrNumParityError   */
            if(status->sub_fatal_intr0 & 0x40000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_LINK_AGGR_NUM_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_LINK_AGG_MEMBER_NUM, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_LINK_AGG_MEMBER_NUM, 0);
            }

            /* process parity error of dsLinkAggrParityError */
            if(status->sub_fatal_intr0 & 0x20000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_LINK_AGGR_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_LINK_AGGREGATION, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_LINK_AGGREGATION, 0);
            }

            /* process parity error of dsQueDropProfParityError */
            if(status->sub_fatal_intr0 & 0x10000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_QUE_DROP_PROF_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_QUEUE_DROP_PROFILE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_QUEUE_DROP_PROFILE, 0);
            }

            /* process parity error of dsQueNumGenCtlParityError */
            if(status->sub_fatal_intr0 & 0x8000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_QUE_NUM_GEN_CTL_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_QUEUE_NUM_GEN_CTL, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_QUEUE_NUM_GEN_CTL, 0);
            }

            /* process parity error of dsQueueIpgIndexParityError */
            if(status->sub_fatal_intr0 & 0x4000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_QUEUE_IPG_INDEX_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_QUEUE_IPG_INDEX, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_QUEUE_IPG_INDEX, 0);
            }

            /* process parity error of dsServiceQueueHashKeyParityError */
            if(status->sub_fatal_intr0 & 0x2000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_SERVICE_QUEUE_HASH_KEY_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_SERVICE_QUEUE_HASH_KEY, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_SERVICE_QUEUE_HASH_KEY, 0);
            }

            /* process parity error of dsServiceQueueParityError */
            if(status->sub_fatal_intr0 & 0x1000)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_SERVICE_QUEUE_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_SERVICE_QUEUE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_SERVICE_QUEUE, 0);
            }

            /* process parity error of dsSgmacMapParityError */
            if(status->sub_fatal_intr0 & 0x800)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_SGMAC_MAP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_SGMAC_MAP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_SGMAC_MAP, 0);
            }

            /* process parity error of egressResrcCountMemParityErr, only log fail addr */
            if(status->sub_fatal_intr0 & 0x400)
            {
                cmd = DRV_IOR(IOC_REG, Q_MGR_ENQ_PARITY_FAIL_RECORD, Q_MGR_ENQ_PARITY_FAIL_RECORD_DS_EGRESS_RESRC_COUNT_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;

        case CTC_INTERRUPT_FATAL3_OAM_PROC_INTR:
            cmd = DRV_IOR(IOC_REG, OAM_PROC_INTERRUPT_FATAL, OAM_PROC_INTERRUPT_FATAL_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x2ED0)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsRmepMepIndexErrIntr */
            if(status->sub_fatal_intr0 & 0x8000)
            {
                cmd = DRV_IOR(IOC_REG, OAMPROC_OAM_PROC_CFG_ERR_RECORD, OAMPROC_OAM_PROC_CFG_ERR_RECORD_DS_RMEP_MEP_INDEX_CFG_ERR_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                /*only log error addr*/
                status->is_parity_error_occur = TRUE;
                status->parity_error_index = parity_fail_addr;
            }

            /* process parity error of dsPortPropertyParityError */
            if(status->sub_fatal_intr0 & 0x4000)
            {
                cmd = DRV_IOR(IOC_REG, OAM_PROC_PARITY_FAIL_RECORD, OAM_PROC_PARITY_FAIL_RECORD_DS_PORT_PROPERTY_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, OAM_DS_PORT_PROPERTY, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(OAM_DS_PORT_PROPERTY, 0);
            }

            /* process parity error of dsMpParityError */
            if(status->sub_fatal_intr0 & 0x8)
            {
                cmd = DRV_IOR(IOC_REG, OAM_PROC_PARITY_FAIL_RECORD, OAM_PROC_PARITY_FAIL_RECORD_DS_MP_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_ETH_MEP, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_ETH_MEP, 0);
            }

            /* process parity error of dsMaParityError */
            if(status->sub_fatal_intr0 & 0x4)
            {
                cmd = DRV_IOR(IOC_REG, OAM_PROC_PARITY_FAIL_RECORD, OAM_PROC_PARITY_FAIL_RECORD_DS_MA_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, OAM_DS_MA, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(OAM_DS_MA, 0);
            }

            /* process parity error of dsMaNameParityError */
            if(status->sub_fatal_intr0 & 0x2)
            {
                cmd = DRV_IOR(IOC_REG, OAM_PROC_PARITY_FAIL_RECORD, OAM_PROC_PARITY_FAIL_RECORD_DS_MA_NAME_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, OAM_DS_MA_NAME, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(OAM_DS_MA_NAME, 0);
            }

            /* process parity error of dsIccParityError */
            if(status->sub_fatal_intr0 & 0x1)
            {
                cmd = DRV_IOR(IOC_REG, OAM_PROC_PARITY_FAIL_RECORD, OAM_PROC_PARITY_FAIL_RECORD_DS_ICC_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, OAM_DS_ICC, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(OAM_DS_ICC, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_OAM_LKP_INTR:
            cmd = DRV_IOR(IOC_REG, OAM_LOOKUP_INTERRUPT, OAM_LOOKUP_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3FF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of dsOamHashKeyParityError */
            if(status->sub_fatal_intr0 & 0x1000)
            {
                cmd = DRV_IOR(IOC_REG, OAM_LKUP_PARITY_FAIL_RECORD, OAM_LKUP_PARITY_FAIL_RECORD_DS_OAM_HASH_KEY_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                parity_fail_addr = parity_fail_addr/2; /*sdk merge two 72bit to 144bit*/
                if(parity_fail_addr < 256)
                {
                    CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_ETH_OAM_HASH_KEY0, 0, parity_fail_addr, &info));
                    SYS_PARITY_ERROR_LOG(DS_ETH_OAM_HASH_KEY0, 0);
                }
                else
                {
                    CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_ETH_OAM_HASH_KEY1, 0, parity_fail_addr-256, &info));
                    SYS_PARITY_ERROR_LOG(DS_ETH_OAM_HASH_KEY1, 0);
                }
            }

            /* process parity error of dsMepChanParityError */
            if(status->sub_fatal_intr0 & 0x800)
            {
                cmd = DRV_IOR(IOC_REG, OAM_LKUP_PARITY_FAIL_RECORD, OAM_LKUP_PARITY_FAIL_RECORD_DS_MEP_CHAN_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, DS_MEP_CHAN_TABLE, 0, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(DS_MEP_CHAN_TABLE, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_OAM_FWD_INTR:
            cmd = DRV_IOR(IOC_REG, OAM_FWD_INTERRUPT_FATAL, OAM_FWD_INTERRUPT_FATAL_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xFC7)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;
        case CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR:
            cmd = DRV_IOR(IOC_REG, OAM_PARSER_INTERRUPT_FATAL, OAM_PARSER_INTERRUPT_FATAL_VALUE_SET_FATAL);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }
            break;

        case CTC_INTERRUPT_FATAL3_QDR_CTL_INTR:
            cmd = DRV_IOR(IOC_REG, QDR_CTL_INTERRUPT, QDR_CTL_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0xF)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of parityError, only log fail addr */
            if(status->sub_fatal_intr0 & 0x10)
            {
                cmd = DRV_IOR(IOC_REG, QDR_CTL_PARITY_FAIL_RECORD, QDR_CTL_PARITY_FAIL_RECORD_QDR_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                SYS_PARITY_ERROR_LOG(0, 0);
            }
            break;
        case CTC_INTERRUPT_FATAL3_DDR_CTL_INTR:
            cmd = DRV_IOR(IOC_REG, TB_INFO_EXT_DDR_CTL_INTERRUPT, TB_INFO_EXT_DDR_CTL_INTERRUPT_VALUE_SET);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &status->sub_fatal_intr0));

            if(status->sub_fatal_intr0 & 0x3)
            {
                status->action = CTC_INTERRUPT_FATAL_INTR_RESET;
            }

            /* process parity error of parityError */
            if(status->sub_fatal_intr0 & 0x4)
            {
                cmd = DRV_IOR(IOC_REG, TB_INFO_EXT_DDR_CTL_PARITY_FAIL_RECORD, TB_INFO_EXT_DDR_CTL_PARITY_FAIL_RECORD_DDR_PARITY_FAIL_ADDR);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &parity_fail_addr));

                CTC_ERROR_RETURN(drv_humber_parity_error_handle(lchip, MAX_TBL_NUM, DRV_MEM_MAPPING_EXT_256K_SRAM, parity_fail_addr, &info));
                SYS_PARITY_ERROR_LOG(MAX_TBL_NUM, SYS_INTERRUPT_EXT_SRAM_256K_SIZE);
            }
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_get_fatal_intr_status(uint8 lchip, uint8 type, ctc_interrupt_fatal_intr_status_t* status)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    CTC_PTR_VALID_CHECK(status);
    SYS_FATAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, interrupt type:%d\n",lchip, type);

    kal_memset(status, 0, sizeof(ctc_interrupt_fatal_intr_status_t));

    if(type >= CTC_INTERRUPT_FATAL3_INTR)  /*fatal3 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR3_VALUE_SET, FATAL_INTR3_VALUE_SET_FATAL_INTR3_VALUE_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        if (IS_BIT_SET(tmp, type-CTC_INTERRUPT_FATAL3_INTR))
        {
            CTC_ERROR_RETURN(_sys_humber_interrupt_get_fatal_intr_status(lchip, type, status));
        }
    }
    else if(type >= CTC_INTERRUPT_FATAL2_INTR)  /*fatal2 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR2_VALUE_SET, FATAL_INTR2_VALUE_SET_FATAL_INTR2_VALUE_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        if (IS_BIT_SET(tmp, type-CTC_INTERRUPT_FATAL2_INTR))
        {
            CTC_ERROR_RETURN(_sys_humber_interrupt_get_fatal_intr_status(lchip, type, status));
        }
    }
    else if(type >= CTC_INTERRUPT_FATAL1_INTR)  /*fatal1 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR1_VALUE_SET, FATAL_INTR1_VALUE_SET_FATAL_INTR1_VALUE_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        if (IS_BIT_SET(tmp, type-CTC_INTERRUPT_FATAL1_INTR))
        {
            CTC_ERROR_RETURN(_sys_humber_interrupt_get_fatal_intr_status(lchip, type, status));
        }
    }
    else  /*fatal0 interrupt*/
    {
        cmd = DRV_IOR(IOC_REG, FATAL_INTR0_VALUE_SET, FATAL_INTR0_VALUE_SET_FATAL_INTR0_VALUE_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        if (IS_BIT_SET(tmp, type))
        {
            CTC_ERROR_RETURN(_sys_humber_interrupt_get_fatal_intr_status(lchip, type, status));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_enable_fatal_intr(uint8 lchip, uint8 type)
{
    SYS_FATAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, fatal interrupt type:%d\n",lchip, type);

    CTC_ERROR_RETURN(_sys_humber_interrupt_enable_fatal_intr(lchip, type));

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_disable_fatal_intr(uint8 lchip, uint8 type)
{
    SYS_FATAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, fatal interrupt type:%d\n",lchip, type);

    CTC_ERROR_RETURN(_sys_humber_interrupt_disable_fatal_intr(lchip, type));
    return CTC_E_NONE;
}

int32
sys_humber_interrupt_clear_fatal_intr(uint8 lchip, uint8 type)
{
    SYS_FATAL_INTERRUPT_INIT_CHECK();
    SYS_INTERRUPT_DBG_FUNC()   ;
    SYS_INTERRUPT_DBG_INFO("chip id:%d, fatal interrupt type:%d\n",lchip, type);

    CTC_ERROR_RETURN(_sys_humber_interrupt_clear_fatal_intr(lchip, type));
    return CTC_E_NONE;
}

static int32
_sys_humber_normal_interrupt_reg_init_start(void)
{
    uint8 lchip;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 chip_num = 0;

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        /* disable sub interrupt*/
        /*ipe aging normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_MASK_SET, IPEAGING_NORMAL_INTR_MASK_SET_INTR_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*statistics normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_SET_NORMAL, STATISTICS_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ptp engine normal intr*/
        tmp = 0x3F;
        cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ipe pkt proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*oam proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_MASK_SET_NORMAL);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*disable sup nomarl interrupt*/
        tmp = 0x3FFFF;
        cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_SET, HUMBERSUP_NORMAL_INTR_MASK_SET_NORMAL_INTR_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    normal_interrupt_init = 1;

    return CTC_E_NONE;
}

static int32
_sys_humber_normal_interrupt_reg_init_end(void)
{
    uint8 lchip;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 chip_num = 0;

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        /* clear interrupt status before open interrupt*/
        /*ipe aging normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_VALUE_RESET, IPEAGING_NORMAL_INTR_VALUE_RESET_INTR_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*statistics normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_VALUE_RESET_NORMAL, STATISTICS_INTR_VALUE_RESET_NORMAL_INTR_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ptp engine normal intr*/
        tmp = 0x3F;
        cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ipe pkt proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*oam proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_VALUE_RESET_NORMAL);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*enable interrupt*/
        /*ipe aging normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, IPEAGING_NORMAL_INTR_MASK_RESET, IPEAGING_NORMAL_INTR_MASK_RESET_INTR_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*statistics normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, STATISTICS_INTR_MASK_RESET_NORMAL, STATISTICS_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ptp engine normal intr*/
        tmp = 0x1F;
        cmd = DRV_IOW(IOC_REG, PTP_INTERRUPT, PTP_INTERRUPT_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*ipe pkt proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, INTERRUPT_NORMAL, INTERRUPT_NORMAL_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*oam proc normal intr*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, OAM_PROC_INTERRUPT_NORMAL, OAM_PROC_INTERRUPT_NORMAL_MASK_RESET_NORMAL);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*clear sup nomarl interrupt*/
        tmp = 0x3FFFF;
        cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_VALUE_RESET, HUMBERSUP_NORMAL_INTR_VALUE_RESET_NORMAL_INTR_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*enable sup nomarl interrupt*/
        tmp = 0x3FFFF;
        cmd = DRV_IOW(IOC_REG, HUMBERSUP_NORMAL_INTR_MASK_RESET, HUMBERSUP_NORMAL_INTR_MASK_RESET_NORMAL_INTR_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*global intr enable*/
        tmp = 0x1;
        cmd = DRV_IOW(IOC_REG, HUMBER_INTR_ENABLE, HUMBER_INTR_ENABLE_HUMBER_INTR_ENABLE);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    }

    normal_interrupt_init = 1;

    return CTC_E_NONE;
}

static int32
_sys_humber_fatal_interrupt_reg_init_start(void)
{
    uint8 lchip;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 chip_num = 0;
    uint8 type = 0;

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        /*disable sup fatal interrupt*/
        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_MASK_SET, FATAL_INTR0_MASK_SET_FATAL_INTR0_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_MASK_SET, FATAL_INTR1_MASK_SET_FATAL_INTR1_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_MASK_SET, FATAL_INTR2_MASK_SET_FATAL_INTR2_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_MASK_SET, FATAL_INTR3_MASK_SET_FATAL_INTR3_MASK_SET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        for(type = 0; type < CTC_INTERRUPT_FATAL_INTR_TYPE_MAX; type++)
        {
            if(type >= CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR &&
                type <= CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR)
            {
                if(!drv_humber_qmac_is_enable(type - CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR))
                    continue;
            }
            if(type >= CTC_INTERRUPT_FATAL0_GMAC0_INTR &&
                type <= CTC_INTERRUPT_FATAL1_GMAC47_INTR)
            {
                if(!drv_humber_gmac_is_enable(type - CTC_INTERRUPT_FATAL0_GMAC0_INTR))
                    continue;
            }


            if(type >= CTC_INTERRUPT_FATAL2_SGMAC0_INTR &&
                type <= CTC_INTERRUPT_FATAL2_XGMAC3_INTR)
            {
                if(0 == (type - CTC_INTERRUPT_FATAL2_SGMAC0_INTR)/4)
                {
                    if(!drv_humber_sgmac_is_enable(type - CTC_INTERRUPT_FATAL2_SGMAC0_INTR))
                    {
                        continue;
                    }
                }
                else
                {
                    if(!drv_humber_xgmac_is_enable(type - CTC_INTERRUPT_FATAL2_XGMAC0_INTR))
                    {
                        continue;
                    }
                }
            }

            if(type >= CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR &&
                type <= CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR)
            {
                if(!drv_humber_fabric_is_enable())
                    continue;
            }

            if((!drv_humber_ext_tcam_is_enable() &&
                CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR == type) ||
                (!drv_humber_ext_ddr_is_enable() &&
                CTC_INTERRUPT_FATAL3_DDR_CTL_INTR == type) ||
                (!drv_humber_ext_qdr_is_enable() &&
                CTC_INTERRUPT_FATAL3_QDR_CTL_INTR == type))
            {
                continue;
            }

            /*下面4个模块默认disable*/
            if(CTC_INTERRUPT_FATAL3_OAM_PROC_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_LKP_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_FWD_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR == type)
            {
                continue;
            }

            /*disable sub modules fatal interrupt*/
            CTC_ERROR_RETURN(_sys_humber_interrupt_disable_fatal_intr(lchip, type));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_fatal_interrupt_reg_init_end(void)
{
    uint8 lchip;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 chip_num = 0;
    uint8 type = 0;
    uint32 fatal_intr0 = 0;
    uint32 fatal_intr1 = 0;
    uint32 fatal_intr2 = 0;
    uint32 fatal_intr3 = 0;

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        for(type = 0; type < CTC_INTERRUPT_FATAL_INTR_TYPE_MAX; type++)
        {
            if(type >= CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR &&
                type <= CTC_INTERRUPT_FATAL0_QUAD_MAC_APP11_INTR)
            {
                if(!drv_humber_qmac_is_enable(type - CTC_INTERRUPT_FATAL0_QUAD_MAC_APP0_INTR))
                    continue;
            }
            if(type >= CTC_INTERRUPT_FATAL0_GMAC0_INTR &&
                type <= CTC_INTERRUPT_FATAL1_GMAC47_INTR)
            {
                if(!drv_humber_gmac_is_enable(type - CTC_INTERRUPT_FATAL0_GMAC0_INTR))
                    continue;
            }


            if(type >= CTC_INTERRUPT_FATAL2_SGMAC0_INTR &&
                type <= CTC_INTERRUPT_FATAL2_XGMAC3_INTR)
            {
                if(0 == (type - CTC_INTERRUPT_FATAL2_SGMAC0_INTR)/4)
                {
                    if(!drv_humber_sgmac_is_enable(type - CTC_INTERRUPT_FATAL2_SGMAC0_INTR))
                    {
                        continue;
                    }
                }
                else
                {
                    if(!drv_humber_xgmac_is_enable(type - CTC_INTERRUPT_FATAL2_XGMAC0_INTR))
                    {
                        continue;
                    }
                }
            }

            if(type >= CTC_INTERRUPT_FATAL2_FABRIC_VOQ_INTR &&
                type <= CTC_INTERRUPT_FATAL2_FABRIC_CAS_INTR)
            {
                if(!drv_humber_fabric_is_enable())
                    continue;
            }

            if((!drv_humber_ext_tcam_is_enable() &&
                CTC_INTERRUPT_FATAL3_TCAM_CTL_EXT_INTR == type) ||
                (!drv_humber_ext_ddr_is_enable() &&
                CTC_INTERRUPT_FATAL3_DDR_CTL_INTR == type) ||
                (!drv_humber_ext_qdr_is_enable() &&
                CTC_INTERRUPT_FATAL3_QDR_CTL_INTR == type))
            {
                continue;
            }

            /*下面4个模块默认disable*/
            if(CTC_INTERRUPT_FATAL3_OAM_PROC_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_LKP_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_FWD_INTR == type ||
                CTC_INTERRUPT_FATAL3_OAM_PARSER_INTR == type)
            {
                continue;
            }

            /*clear sub modules fatal interrupt*/
            CTC_ERROR_RETURN(_sys_humber_interrupt_clear_fatal_intr(lchip, type));

            /*enable sub modules fatal interrupt*/
            CTC_ERROR_RETURN(_sys_humber_interrupt_enable_fatal_intr(lchip, type));

            if(type < CTC_INTERRUPT_FATAL1_INTR)
            {
                fatal_intr0 |= 1 << type;
            }
            else if(type < CTC_INTERRUPT_FATAL2_INTR)
            {
                fatal_intr1 |= 1 << (type - CTC_INTERRUPT_FATAL1_INTR);
            }
            else if(type < CTC_INTERRUPT_FATAL3_INTR)
            {
                fatal_intr2 |= 1 << (type - CTC_INTERRUPT_FATAL2_INTR);
            }
            else if(type < CTC_INTERRUPT_FATAL_INTR_TYPE_MAX)
            {
                fatal_intr3 |= 1 << (type - CTC_INTERRUPT_FATAL3_INTR);
            }
        }

        /*clear sup fatal interrupt*/
        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_VALUE_RESET, FATAL_INTR0_VALUE_RESET_FATAL_INTR0_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_VALUE_RESET, FATAL_INTR1_VALUE_RESET_FATAL_INTR1_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_VALUE_RESET, FATAL_INTR2_VALUE_RESET_FATAL_INTR2_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = 0xffffffff;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_VALUE_RESET, FATAL_INTR3_VALUE_RESET_FATAL_INTR3_VALUE_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        /*enable sup fatal interrupt*/
        tmp = fatal_intr0;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR0_MASK_RESET, FATAL_INTR0_MASK_RESET_FATAL_INTR0_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = fatal_intr1;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR1_MASK_RESET, FATAL_INTR1_MASK_RESET_FATAL_INTR1_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = fatal_intr2;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR2_MASK_RESET, FATAL_INTR2_MASK_RESET_FATAL_INTR2_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        tmp = fatal_intr3;
        cmd = DRV_IOW(IOC_REG, FATAL_INTR3_MASK_RESET, FATAL_INTR3_MASK_RESET_FATAL_INTR3_MASK_RESET);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

    }

    fatal_interrupt_init = 1;

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_reg_init_start(void)
{
    CTC_ERROR_RETURN(_sys_humber_normal_interrupt_reg_init_start());
    CTC_ERROR_RETURN(_sys_humber_fatal_interrupt_reg_init_start());

    return CTC_E_NONE;
}

int32
sys_humber_interrupt_reg_init_end(void)
{
    CTC_ERROR_RETURN(_sys_humber_normal_interrupt_reg_init_end());
    CTC_ERROR_RETURN(_sys_humber_fatal_interrupt_reg_init_end());

    return CTC_E_NONE;
}

