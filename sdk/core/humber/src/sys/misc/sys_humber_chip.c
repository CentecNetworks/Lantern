/**
 @file sys_humber_chip.c

 @date 2009-10-19

 @version v2.0

 The file define APIs of chip of sys layer
*/
/****************************************************************************
 *
 * Header Files
 *
 ****************************************************************************/
#include "ctc_error.h"
#include "sys_humber_chip.h"
#include "sys_humber_ftm.h"

#include "drv_io.h"
#include "drv_humber.h"
#include "drv_humber_data_path.h"
#include "drv_humber_parity_error.h"

/****************************************************************************
 *
 * Global and static
 *
 *****************************************************************************/
 static sys_chip_master_t *p_chip_master = NULL;

/****************************************************************************
 *
 * Function
 *
 *****************************************************************************/

/**
 @brief The function is to initialize the chip module and set the local chip number of the linecard
*/
int32
sys_humber_chip_init(uint8 lchip_num)
{
    int32 ret;
    if (NULL != p_chip_master)
    {
        return CTC_E_NONE;
    }

    if (lchip_num > CTC_MAX_LOCAL_CHIP_NUM)
    {
        return CTC_E_INVALID_CHIP_NUM;
    }

    p_chip_master = (sys_chip_master_t *)mem_malloc(MEM_SYSTEM_MODULE, sizeof(sys_chip_master_t));

    if (NULL == p_chip_master)
    {
        return CTC_E_NO_MEMORY;
    }

    p_chip_master->lchip_num    = lchip_num;
    kal_memset(p_chip_master->g_chip_id, CTC_INVALID_CHIPID, sizeof(uint8)*CTC_MAX_LOCAL_CHIP_NUM);

    ret = drv_init(lchip_num);
    if (ret < 0)
    {
        mem_free(p_chip_master);
        ret = CTC_E_DRV_FAIL;
    }

    return ret;
}

/**
 @brief The function is to initialize datapath
*/
int32
sys_humber_data_path_init(ctc_chip_reset_cb reset_cb, ctc_chip_datapath_t* chip_datapath,
                                char* datapath_config_file)
{
    int32 ret;
    drv_chip_info_t drv_chip_info;
    drv_work_platform_type_t platform_type;

    if (NULL == p_chip_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(chip_datapath);

    ret = drv_get_platform_type(&platform_type);
    if (ret < 0)
    {
        return CTC_E_INVALID_PARAM;
    }

    if (platform_type == SW_SIM_PLATFORM)
    {
        drv_humber_init_sw_emu();
        return CTC_E_NONE;
    }

    kal_memset(&drv_chip_info, 0, sizeof(drv_chip_info_t));
    drv_chip_info.chip_id       = chip_datapath->chip_id;
    drv_chip_info.chip_io_type  = chip_datapath->chip_io_type;
    drv_chip_info.chip_type     = chip_datapath->chip_type;
    drv_chip_info.cpumac_speed  = chip_datapath->cpumac_speed;
    drv_chip_info.ptp_quanta    = chip_datapath->ptp_quanta;

    CTC_ERROR_RETURN(drv_humber_init_total(reset_cb, drv_chip_info, datapath_config_file));

    return CTC_E_NONE;
}

/**
 @brief The function is to initialize the parity error
*/
int32
sys_humber_parity_error_init(void)
{
    uint8 lchip = 0, lchip_num = 0;

    drv_chip_parity_error_init_t drv_parity_error;
    sys_alloc_info_t* p_alloc_info;

    p_alloc_info = sys_alloc_get_alloc_info_ptr();
    drv_parity_error.is_hash_48ksize = p_alloc_info->is_hash_48ksize;

    lchip_num = p_chip_master->lchip_num;
    for (lchip = 0; lchip < lchip_num; lchip++)
    {
        drv_parity_error.chip_id = lchip;

        CTC_ERROR_RETURN(drv_humber_mem_mapping_init(&drv_parity_error));
    }

    return CTC_E_NONE;
}

/**
 @brief The function is to get the local chip num
*/
uint8
sys_humber_get_local_chip_num(void)
{
    if (NULL == p_chip_master)
    {
        return 0;
    }

    return p_chip_master->lchip_num;
}

/**
 @brief The function is to set chip's global chip id
*/
int32
sys_humber_set_gchip_id(uint8 lchip_id, uint8 gchip_id)
{
    if (NULL == p_chip_master)
    {
        return CTC_E_NOT_INIT;
    }

    if (lchip_id >= p_chip_master->lchip_num)
    {
        return CTC_E_INVALID_LOCAL_CHIPID;
    }

    CTC_GLOBAL_CHIPID_CHECK(gchip_id);

    p_chip_master->g_chip_id[lchip_id] = gchip_id;

    return CTC_E_NONE;
}

/**
 @brief The function is to get chip's global chip id
*/
int32
sys_humber_get_gchip_id(uint8 lchip_id, uint8 *gchip_id)
{
    if (NULL == p_chip_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(gchip_id);

    *gchip_id = p_chip_master->g_chip_id[lchip_id];

    return CTC_E_NONE;
}

/**
 @brief The function is to judge whether the chip is local
*/
bool
sys_humber_chip_is_local(uint8 gchip_id, uint8 *lchip_id)
{
    uint8 chip_id;

    if (NULL == p_chip_master)
    {
        *lchip_id = CTC_INVALID_CHIPID;
        return FALSE;
    }

    for (chip_id = 0; chip_id < p_chip_master->lchip_num; chip_id++)
    {
        if ((gchip_id != CTC_INVALID_CHIPID)&&(p_chip_master->g_chip_id[chip_id] == gchip_id))
        {
            *lchip_id = chip_id;
            return TRUE;
        }
    }

    *lchip_id = CTC_INVALID_CHIPID;
    return FALSE;
}



/**
 @brief The function is to set chip's global cfg

*/
int32
sys_humber_set_chip_global_cfg(ctc_chip_global_cfg_t *chip_cfg)
{
    int32 cmd_sa, cmd_da, cmd_ctl, cmd_type;

    cpu_mac_sa_t cpu_macsa;
    cpu_mac_da_t cpu_macda;
    cpu_mac_ctl_t mac_ctl;
    cpu_mac_type_t mac_type;


    if (NULL == p_chip_master)
    {
        return CTC_E_NOT_INIT;
    }

    CTC_PTR_VALID_CHECK(chip_cfg);

    if(chip_cfg->lchip > p_chip_master->lchip_num)
     {
         return CTC_E_INVALID_LOCAL_CHIPID;
    }

    kal_memset(&cpu_macsa, 0, sizeof(cpu_mac_sa_t));
    kal_memset(&cpu_macda, 0, sizeof(cpu_mac_da_t));
    kal_memset(&mac_ctl, 0, sizeof(cpu_mac_ctl_t));
    kal_memset(&mac_type, 0, sizeof(cpu_mac_type_t));

    cpu_macsa.cpu_mac_sa31_to0 = chip_cfg->cpu_mac_sa[2] << 24 | chip_cfg->cpu_mac_sa[3] << 16 | chip_cfg->cpu_mac_sa[4] << 8 | chip_cfg->cpu_mac_sa[5];
    cpu_macsa.cpu_mac_sa47_to32 = chip_cfg->cpu_mac_sa[0] << 8 | chip_cfg->cpu_mac_sa[1] ;


    cpu_macda.cpu_mac0_da31_to0 = chip_cfg->cpu_mac_da[0][2] << 24 | chip_cfg->cpu_mac_da[0][3] << 16 | chip_cfg->cpu_mac_da[0][4] << 8 | chip_cfg->cpu_mac_da[0][5];
    cpu_macda.cpu_mac0_da47_to32 = chip_cfg->cpu_mac_da[0][0] << 8 | chip_cfg->cpu_mac_da[0][1] ;

    cpu_macda.cpu_mac1_da31_to0 = chip_cfg->cpu_mac_da[1][2] << 24 | chip_cfg->cpu_mac_da[1][3] << 16 | chip_cfg->cpu_mac_da[1][4] << 8 | chip_cfg->cpu_mac_da[1][5];
    cpu_macda.cpu_mac1_da47_to32 = chip_cfg->cpu_mac_da[1][0] << 8 | chip_cfg->cpu_mac_da[1][1] ;

    cpu_macda.cpu_mac2_da31_to0 = chip_cfg->cpu_mac_da[2][2] << 24 | chip_cfg->cpu_mac_da[2][3] << 16 | chip_cfg->cpu_mac_da[2][4] << 8 | chip_cfg->cpu_mac_da[2][5];
    cpu_macda.cpu_mac2_da47_to32 = chip_cfg->cpu_mac_da[2][0] << 8 | chip_cfg->cpu_mac_da[2][1] ;

    cpu_macda.cpu_mac3_da31_to0 = chip_cfg->cpu_mac_da[3][2] << 24 | chip_cfg->cpu_mac_da[3][3] << 16 | chip_cfg->cpu_mac_da[3][4] << 8 | chip_cfg->cpu_mac_da[3][5];
    cpu_macda.cpu_mac3_da47_to32 = chip_cfg->cpu_mac_da[3][0] << 8 | chip_cfg->cpu_mac_da[3][1] ;

    cpu_macda.cpu_mac4_da31_to0 = chip_cfg->cpu_mac_da[4][2] << 24 | chip_cfg->cpu_mac_da[4][3] << 16 | chip_cfg->cpu_mac_da[4][4] << 8 | chip_cfg->cpu_mac_da[4][5];
    cpu_macda.cpu_mac4_da47_to32 = chip_cfg->cpu_mac_da[4][0] << 8 | chip_cfg->cpu_mac_da[4][1] ;

    cpu_macda.cpu_mac5_da31_to0 = chip_cfg->cpu_mac_da[5][2] << 24 | chip_cfg->cpu_mac_da[5][3] << 16 | chip_cfg->cpu_mac_da[5][4] << 8 | chip_cfg->cpu_mac_da[5][5];
    cpu_macda.cpu_mac5_da47_to32 = chip_cfg->cpu_mac_da[5][0] << 8 | chip_cfg->cpu_mac_da[5][1] ;

    cpu_macda.cpu_mac6_da31_to0 = chip_cfg->cpu_mac_da[6][2] << 24 | chip_cfg->cpu_mac_da[6][3] << 16 | chip_cfg->cpu_mac_da[6][4] << 8 | chip_cfg->cpu_mac_da[6][5];
    cpu_macda.cpu_mac6_da47_to32 = chip_cfg->cpu_mac_da[6][0] << 8 | chip_cfg->cpu_mac_da[6][1] ;

    cpu_macda.cpu_mac7_da31_to0 = chip_cfg->cpu_mac_da[7][2] << 24 | chip_cfg->cpu_mac_da[7][3] << 16 | chip_cfg->cpu_mac_da[7][4] << 8 | chip_cfg->cpu_mac_da[7][5];
    cpu_macda.cpu_mac7_da47_to32 = chip_cfg->cpu_mac_da[7][0] << 8 | chip_cfg->cpu_mac_da[7][1] ;



    mac_ctl.ingress_remove_en = 1;
    mac_ctl.egress_add_en = 1;
    mac_ctl.egress_fifo_afull_thrd = 0x12;

    mac_type.cpu_mac_type = 0x5A5A;

    cmd_sa = DRV_IOW(IOC_REG, CPU_MAC_SA, DRV_ENTRY_FLAG);
    cmd_da = DRV_IOW(IOC_REG, CPU_MAC_DA, DRV_ENTRY_FLAG);
    cmd_ctl = DRV_IOW(IOC_REG, CPU_MAC_CTL, DRV_ENTRY_FLAG);
    cmd_type = DRV_IOW(IOC_REG, CPU_MAC_TYPE, DRV_ENTRY_FLAG);



        CTC_ERROR_RETURN(drv_reg_ioctl(chip_cfg->lchip, 0, cmd_sa, &cpu_macsa));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_cfg->lchip, 0, cmd_da, &cpu_macda));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_cfg->lchip, 0, cmd_ctl, &mac_ctl));
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_cfg->lchip, 0, cmd_type, &mac_type));


    return CTC_E_NONE;



}


int32
sys_humber_get_chip_clock(uint16 *freq)
{
    *freq = drv_humber_get_core_freq();

    return CTC_E_NONE;
}

