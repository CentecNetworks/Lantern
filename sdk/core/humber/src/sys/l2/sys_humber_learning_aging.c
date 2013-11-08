/**
 @file sys_humber_learning_aging.c

 @date 2010-3-16

 @version v2.0

---file comments----
*/
#include "ctc_const.h"
#include "ctc_error.h"
#include "ctc_vector.h"
#include "ctc_learning_aging.h"
#include "ctc_interrupt.h"
#include "ctc_debug.h"

#include "sys_humber_l2_fdb.h"
#include "sys_humber_chip.h"
#include "sys_humber_interrupt.h"
#include "sys_humber_learning_aging.h"

#include "drv_io.h"
#include "drv_humber.h"
#include "drv_humber_data_path.h"

#define SYS_LEARNING_CACHE_MAX_INDEX 16
#define SYS_AGING_BASE_SHIFT 8
#define SYS_AGING_STATUS_CHECK_BIT_LEN 16

#define SYS_AGING_DEFAULT_THRESHOLD 1
#define MIN_SYS_AGING_INTERVAL 1

/**
 @brief enum type about aging scan width
*/
enum sys_aging_scan_width_e
{
    SYS_AGING_SCAN_WIDTH_16K = 0,   /**< aging scan range from 0~16K */
    SYS_AGING_SCAN_WIDTH_32K,         /**< aging scan range from 0~32K */
    SYS_AGING_SCAN_WIDTH_64K,        /**< aging scan range from 0~64K */
    SYS_AGING_SCAN_WIDTH_128K,       /**< aging scan range from 0~128K */
    SYS_AGING_SCAN_WIDTH_256K,       /**< aging scan range from 0~256K */
};
typedef enum sys_aging_scan_width_e sys_aging_scan_width_t;


#define SYS_LEARNING_AGING_DBG_INFO(FMT, ...)\
                    {\
                        CTC_DEBUG_OUT_INFO(l2, learning_aging, L2_LEARNING_AGING_SYS, FMT, ##__VA_ARGS__);\
                    }

#define SYS_LEARNING_AGING_DBG_FUNC()\
                    {\
                        CTC_DEBUG_OUT_FUNC(l2, learning_aging, L2_LEARNING_AGING_SYS);\
                    }

/**
 @brief struct type about aging info
*/
struct sys_learning_aging_ds_s
{
    uint32  aging_interval;
    uint32  aging_base ;
    uint16   aging_width;
    uint8    fifo_threshold;
    uint8    always_cpu_learning;
    uint8    learning_cache_int_thrd;
    uint8   learning_exception_en;
    uint16   rsv0;
    uint16   learn_en;  
};
typedef struct sys_learning_aging_ds_s sys_learning_aging_ds_t;


sys_learning_aging_ds_t  g_aging_master;


int32
sys_humber_set_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;

    ipe_learning_cache_valid_t learn_cache_vld;

    CTC_PTR_VALID_CHECK(p_learning_action);
    SYS_LEARNING_AGING_DBG_INFO("Setting Learning Cache threshold: %d, action: %d\n",
		p_learning_action->cache_threshold, p_learning_action->action);


     kal_memset(&learn_cache_vld, 0, sizeof(learn_cache_vld));
     cmd = DRV_IOR(IOC_REG,    IPE_LEARNING_CACHE_VALID,  DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &learn_cache_vld));

    switch (p_learning_action->action)
    {
        case  CTC_LEARNING_ACTION_ALWAYS_CPU:
                 learn_cache_vld.always_cpu_learning = 1;
                 learn_cache_vld.exception_en            = 1;
		  break;

	case  CTC_LEARNING_ACTION_CACHE_FULL_TO_CPU:
               learn_cache_vld.exception_en = 1;
	        learn_cache_vld.always_cpu_learning = 0;
              if (p_learning_action->cache_threshold >= SYS_LEARNING_CACHE_MAX_INDEX)
              {
                   SYS_LEARNING_AGING_DBG_INFO("Learning threshold cfg exceeds the max value 0x1F!\n");
                   return CTC_E_LEARNING_AND_AGING_INVALID_LEARNING_THRESHOLD;
              }
              learn_cache_vld.learning_cache_int_thrd = p_learning_action->cache_threshold & 0x1F;
		 break;

	case  CTC_LEARNING_ACTION_CACHE_ONLY:
               learn_cache_vld.always_cpu_learning = 0;
               learn_cache_vld.exception_en = 0;
		 if (p_learning_action->cache_threshold >= SYS_LEARNING_CACHE_MAX_INDEX)
              {
                   SYS_LEARNING_AGING_DBG_INFO("Learning threshold cfg exceeds the max value 0x1F!\n");
                   return CTC_E_LEARNING_AND_AGING_INVALID_LEARNING_THRESHOLD;
              }
              learn_cache_vld.learning_cache_int_thrd = p_learning_action->cache_threshold & 0x1F;
		break;
	case CTC_LEARNING_ACTION_DONLEARNING:
		 learn_cache_vld.always_cpu_learning = 1;
               learn_cache_vld.exception_en = 0;
		break;

	default:
		 return CTC_E_NONE;
    }


    cmd = DRV_IOW(IOC_REG,  IPE_LEARNING_CACHE_VALID, DRV_ENTRY_FLAG);
    local_chip_num = sys_humber_get_local_chip_num();
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &learn_cache_vld));
    }

    g_aging_master.always_cpu_learning = learn_cache_vld.always_cpu_learning;
    g_aging_master.learning_exception_en = learn_cache_vld.exception_en;
   g_aging_master.learning_cache_int_thrd = learn_cache_vld.learning_cache_int_thrd;
    return CTC_E_NONE;
}

int32
sys_humber_get_learning_action(ctc_learning_action_info_t* p_learning_action)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    ipe_learning_cache_valid_t learn_cache_vld;

    CTC_PTR_VALID_CHECK(p_learning_action);

    kal_memset(&learn_cache_vld, 0, sizeof(learn_cache_vld));
    cmd = DRV_IOR(IOC_REG,   IPE_LEARNING_CACHE_VALID,  DRV_ENTRY_FLAG);

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &learn_cache_vld));

    if (1 == learn_cache_vld.exception_en )
    {
        if ( 1 == learn_cache_vld.always_cpu_learning )
        {
             p_learning_action->action = CTC_LEARNING_ACTION_ALWAYS_CPU;
        }
	 else
	 {
             p_learning_action->action = CTC_LEARNING_ACTION_CACHE_FULL_TO_CPU;
	      p_learning_action->cache_threshold  = learn_cache_vld.learning_cache_int_thrd & 0x1F;
	 }
    }
    else
    {
        if ( 0 == learn_cache_vld.always_cpu_learning )
        {
               p_learning_action->action = CTC_LEARNING_ACTION_CACHE_ONLY;
	        p_learning_action->cache_threshold  = learn_cache_vld.learning_cache_int_thrd & 0x1F;
        }
        else
        {
              p_learning_action->action = CTC_LEARNING_ACTION_DONLEARNING;
        }
    }

    return CTC_E_NONE;
}


int32
sys_humber_learning_get_cache_entry_valid_bitmap(uint8 lchip, uint16* entry_vld_bitmap)
{
    uint32 cmd = 0;
    uint32 entry_vld = 0;

    cmd = DRV_IOR(IOC_REG,
                  IPE_LEARNING_CACHE_VALID,
                  IPE_LEARNING_CACHE_VALID_LEARNING_ENTRY_VALID);

    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &entry_vld));

    *entry_vld_bitmap = entry_vld&0xFFFF;

    return CTC_E_NONE;
}


int32
sys_humber_learning_read_learning_cache(uint8 lchip,
                                        uint16 entry_vld_bitmap,
                                        ctc_learning_cache_t* l2_lc)
{
    uint32 cmd = 0;
    uint8 index = 0;
    uint8 entry_idx = 0;
    ipe_learning_cache_t lc;

    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Learning] Show Learning cache data: \n");

    /* check PARA */
    CTC_PTR_VALID_CHECK(l2_lc);

    l2_lc->entry_num = 0;
    if (0 == entry_vld_bitmap)
    {
        return CTC_E_NONE;
    }
    SYS_LEARNING_AGING_DBG_INFO("$$$[Learning] Show Learning cache data: \n");

    for (index = 0, entry_idx = 0; index < SYS_LEARNING_CACHE_MAX_INDEX; index++)
    {
        /* Get info from the learning cache */
        if (entry_vld_bitmap & (1 << index))
        {
            cmd = DRV_IOR(IOC_TABLE, IPE_LEARNING_CACHE, DRV_ENTRY_FLAG);
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &lc));

            SYS_LEARNING_AGING_DBG_INFO("$$$[Learning] Cache index = %d\n", index);
            SYS_LEARNING_AGING_DBG_INFO("mac = 0x%04x%08x ; fid = 0x%x ; port = %d \n",
                                         lc.mac_sa_msb, lc.mac_sa_lsb,
                                         lc.mapped_vlan_id, lc.global_src_port);
            SYS_LEARNING_AGING_DBG_INFO("cmac = 0x%04x%08x ; is_vpls_src_port = %d ; is_ether_oam = %d \n",
                                         lc.cmac_sa_msb, lc.cmac_sa_lsb,
                                         lc.is_vpls_src_port, lc.is_ether_oam);
            SYS_LEARNING_AGING_DBG_INFO("cvlan_id = 0x%x ; svlan_id = 0x%x ; ether_oam_md_level = %d \n",
                                         lc.cvlan_id, lc.svlan_id,
                                         lc.ether_oam_md_level);

            l2_lc->learning_entry[entry_idx].cvlan_id = lc.cvlan_id;
            l2_lc->learning_entry[entry_idx].is_logic_port = lc.is_vpls_src_port;
            l2_lc->learning_entry[entry_idx].is_ether_oam = lc.is_ether_oam;
            l2_lc->learning_entry[entry_idx].svlan_id = lc.svlan_id;
            l2_lc->learning_entry[entry_idx].ether_oam_md_level = lc.ether_oam_md_level;
            l2_lc->learning_entry[entry_idx].global_src_port = lc.global_src_port;
            l2_lc->learning_entry[entry_idx].mapped_vlan_id = lc.mapped_vlan_id;
            l2_lc->learning_entry[entry_idx].mac_sa_32to47 = lc.mac_sa_msb;
            l2_lc->learning_entry[entry_idx].mac_sa_0to31 = lc.mac_sa_lsb;
            l2_lc->learning_entry[entry_idx].cmac_sa_32to47 = lc.cmac_sa_msb;
            l2_lc->learning_entry[entry_idx].cmac_sa_0to31 = lc.cmac_sa_lsb;

            entry_idx++;

        }
        /* clear cache valid register's entry valid bitmap */

    }

    l2_lc->entry_num = entry_idx;

    return CTC_E_NONE;
}

int32
sys_humber_learning_clear_learning_cache(uint8 lchip,
                                        uint16 entry_vld_bitmap)
{
    ipe_learning_cache_valid_t learn_cache_vld;
    uint32 cmd = 0;
    drv_work_platform_type_t platform_type;

    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Learning] Clear Learning cache data: \n");

    CTC_ERROR_RETURN(drv_get_platform_type(&platform_type));


    learn_cache_vld.always_cpu_learning = g_aging_master.always_cpu_learning ;
    learn_cache_vld.exception_en =    g_aging_master.learning_exception_en;
    learn_cache_vld.learning_cache_int_thrd = g_aging_master.learning_cache_int_thrd;
    if (platform_type == HW_PLATFORM)
    {
        learn_cache_vld.learning_entry_valid = entry_vld_bitmap;  /*must write one clear */
    }
    else
    {
        learn_cache_vld.learning_entry_valid = 0;
    }

    cmd = DRV_IOW(IOC_REG, IPE_LEARNING_CACHE_VALID, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &learn_cache_vld));

    return CTC_E_NONE;
}



int32
sys_humber_aging_set_fifo_threshold(uint8 threshold)
{
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 threshold_tmp = 0;

    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Setting Aging threshold: 0x%x\n",threshold);

    if (threshold > 0x1F)
    {
        SYS_LEARNING_AGING_DBG_INFO("Aging threshold cfg exceeds the max value 0x1F!\n");
        return CTC_E_LEARNING_AND_AGING_INVALID_AGING_THRESHOLD;
    }

    threshold_tmp = threshold & 0x1F;

    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_FIFO_DEPTH_THRESHOLD);

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &threshold_tmp));
    }
    g_aging_master.fifo_threshold = threshold;

    return CTC_E_NONE;
}


int32
sys_humber_aging_get_fifo_threshold(uint8* threshold)
{

    CTC_PTR_VALID_CHECK(threshold);
    SYS_LEARNING_AGING_DBG_FUNC();

    *threshold = g_aging_master.fifo_threshold;


    return CTC_E_NONE;
}


/*
uint32 interval   seconds
*/
int32
sys_humber_aging_set_aging_interval(uint32 age_seconds)
{
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
     uint32 aging_interval = 0;
    uint32 aging_width = 0;
    uint32 max_age_seconds= 0;
    uint32  core_frequecy = 0;

    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Setting Aging interval: %d s\n",age_seconds);

    core_frequecy = drv_humber_get_core_freq();
    aging_width = g_aging_master.aging_width;
    max_age_seconds = CTC_MAX_UINT32_VALUE/(core_frequecy*1000000) * aging_width;

    if (age_seconds < MIN_SYS_AGING_INTERVAL || age_seconds > max_age_seconds )
    {
        return CTC_E_AGING_INVALID_INTERVAL;
    }

    /*CTC_AGING_SCAN_WIDTH_128K    CLOCK_FREQUENCY == 575000000 */
     /* (575000000 * age_seconds)/(8*1024)   -->   (575 *1000000 *age_seconds)/(8*1024) -> (age_seconds) * ((575 *1000000)/aging_width) */

    aging_interval =age_seconds *1000 * ((core_frequecy *1000)/aging_width);

    cmd = DRV_IOW(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_AGING_INTERVAL);
   local_chip_num = sys_humber_get_local_chip_num();
    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &aging_interval));
    }
    g_aging_master.aging_interval = age_seconds;
   SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Setting Aging aging_interval: %u max_age_seconds:%u \n",aging_interval,max_age_seconds);

    return CTC_E_NONE;
}

int32
sys_humber_aging_get_aging_interval(uint32 *age_seconds)
{

   *age_seconds = g_aging_master.aging_interval;
    return CTC_E_NONE;
}

int32
sys_humber_aging_set_aging_scan_en(bool enable)
{
    uint32 enable_tmp = (enable == TRUE)?1:0;
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Setting Aging aging_scan_en = %d\n", enable);

    SYS_LEARNING_AGING_DBG_FUNC();
    if (TRUE == enable)
    {
        SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Open Aging ScanEn \n");
    }

    else
    {
        SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Close Aging ScanEn \n");
    }


    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_SCAN_EN);

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &enable_tmp));

    }
    return CTC_E_NONE;

}

int32
sys_humber_aging_get_aging_scan_en(bool* enable)
{

    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint32 enable_tmp = 0;

    cmd = DRV_IOR(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_SCAN_EN);

    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &enable_tmp));
    *enable = enable_tmp & 0x1;

    return CTC_E_NONE;

}

int32
sys_humber_aging_set_stop_scan_timer_expired(bool enable)
{
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint8 local_chip_num = 0;
    uint32 enable_tmp = enable?1:0;

    SYS_LEARNING_AGING_DBG_FUNC();

     SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] stop scan timer expired:%d \n",enable_tmp);


    local_chip_num = sys_humber_get_local_chip_num();

    cmd = DRV_IOW(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_STOP_ON_MAX_PTR);

    for (chip_id = 0; chip_id < local_chip_num; chip_id++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &enable_tmp));
    }

    return CTC_E_NONE;
}

int32
sys_humber_aging_get_stop_scan_timer_expired(bool* enable)
{
    uint32 cmd = 0;
    uint8 chip_id = 0;
    uint32 enable_tmp = 0;

    cmd = DRV_IOR(IOC_REG,
                  IPE_AGING_CTL,
                  IPE_AGING_CTL_STOP_ON_MAX_PTR);

    CTC_ERROR_RETURN(drv_reg_ioctl(chip_id, 0, cmd, &enable_tmp));

    *enable = enable_tmp ?TRUE:FALSE;;

    return CTC_E_NONE;
}

int32
sys_humber_aging_set_aging_status(uint8 lchip, uint32 entry_index,bool enable)
{

    uint32 cmd = 0;

    uint32 aging_base = 0;

    uint32 dsaging_idx = 0;
    uint32 dsaging_vaule = 0;

    uint16 aging_status = 0;
    uint16 aging_vld = 0;

    uint16 bitmap_index = 0;

    SYS_LEARNING_AGING_DBG_FUNC();


    aging_base = g_aging_master.aging_base;

    dsaging_idx = (entry_index + (aging_base<< SYS_AGING_BASE_SHIFT))/SYS_AGING_STATUS_CHECK_BIT_LEN;
    cmd = DRV_IOR(IOC_TABLE, IPE_AGING_RAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsaging_idx, cmd, &dsaging_vaule));

    aging_status = dsaging_vaule&0xFFFF;
    aging_vld = (dsaging_vaule>>16)&0xFFFF;

    bitmap_index = (entry_index + (aging_base<< SYS_AGING_BASE_SHIFT))%SYS_AGING_STATUS_CHECK_BIT_LEN;

    /* status bit = 0: enable aging, 1: disable aging */
    if (TRUE == enable)
    {
        SET_BIT(aging_status, bitmap_index);
        SET_BIT(aging_vld, bitmap_index);
    }
    else
    {
        CLEAR_BIT(aging_status, bitmap_index);
        CLEAR_BIT(aging_vld, bitmap_index);
    }

    dsaging_vaule = (aging_vld<<16) | aging_status;

    cmd = DRV_IOW(IOC_TABLE, IPE_AGING_RAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsaging_idx, cmd, &dsaging_vaule));

    SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] fifo_ptr:%d aging_idx:%d dsaging_vaule = 0x%x,  age_status = %d\n",entry_index,dsaging_idx,dsaging_vaule, enable);

    return CTC_E_NONE;
}

int32
sys_humber_aging_get_aging_index_status(uint8 lchip, uint32 aging_index,
                                 ctc_aging_status_t* age_status)
{
    uint32 cmd = 0;
    uint32 aging_index_status = 0;
    uint16 aging_ptr_valid = 0;
     uint16 need_aging_valid = 0;

    CTC_PTR_VALID_CHECK(age_status);

    cmd = DRV_IOR(IOC_TABLE, IPE_AGING_RAM, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, aging_index, cmd, &aging_index_status));

    aging_ptr_valid = (aging_index_status >> 16) & 0xFFFF;
    need_aging_valid = ~(aging_index_status & 0xFFFF);
    age_status->aging_valid_bitmap = aging_ptr_valid & need_aging_valid;

    return CTC_E_NONE;
}

int32
sys_humber_aging_read_aging_fifo(uint8 lchip,
                                 ctc_aging_fifo_info_t* fifo_info_ptr)
{
    uint8 fifo_idx = 0;
    uint8 index = 0;
    uint32 cmd = 0;
    int32 ret = 0;
    ipe_aging_fifo_depth_t aging_fifo_depth;
    ipe_aging_fifo_ram_t aging_fifo_ram;

    SYS_LEARNING_AGING_DBG_FUNC();
    SYS_LEARNING_AGING_DBG_INFO("$$$[Aging] Show Aging Fifo Content \n");

    CTC_PTR_VALID_CHECK(fifo_info_ptr);


    fifo_info_ptr->aging_base =  g_aging_master.aging_base;
    cmd = DRV_IOR(IOC_REG, IPE_AGING_FIFO_DEPTH, DRV_ENTRY_FLAG);
    ret = drv_reg_ioctl(lchip, 0, cmd, &aging_fifo_depth);

    /* Get aging index from aging FIFO according CTC_AGING_FIFO_DEPTH*/
    for (index = 0;  index < aging_fifo_depth.ipe_aging_fifo_depth; index++)
    {
        cmd = DRV_IOR(IOC_REG, IPE_AGING_FIFO_RAM, DRV_ENTRY_FLAG);
        ret = drv_reg_ioctl(lchip, 0, cmd, &aging_fifo_ram);
        if (ret < 0 )
        {
            continue;
        }
        fifo_info_ptr->aging_index_array[fifo_idx] = aging_fifo_ram.aging_fifo_ptr;
        fifo_idx++;

    }

    fifo_info_ptr->fifo_idx_num = fifo_idx;

    return CTC_E_NONE;
}

int32
sys_humber_learning_set_learning_en(bool enable)
{
    uint8  lchip;
    uint8 local_chip_num = 0;
     
    uint32 cmd;
    uint32 value = 0;
    local_chip_num = sys_humber_get_local_chip_num();
    
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        if (enable)
        {
            value = 0xFFFF;
            cmd = DRV_IOW(IOC_REG, IPE_LEARNING_CACHE_VALID, IPE_LEARNING_CACHE_VALID_LEARNING_ENTRY_VALID);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &value));

            CTC_ERROR_RETURN(sys_humber_interrupt_clear_normal_intr(lchip, CTC_INTERRUPT_NORMAL_LEARNING_INTR));
            CTC_ERROR_RETURN(sys_humber_interrupt_enable_normal_intr(lchip, CTC_INTERRUPT_NORMAL_LEARNING_INTR));
            
        }
        else
        {
 
            CTC_ERROR_RETURN(sys_humber_interrupt_disable_normal_intr(lchip, CTC_INTERRUPT_NORMAL_LEARNING_INTR)); 
        }
    }
    g_aging_master.learn_en = (enable == TRUE)?1:0;
    return CTC_E_NONE;
    
}

static int32
_sys_humber_learning_init(void)
{
    uint32 cmd = 0;
     uint8 lchip = 0;
    uint8 local_chip_num = 0;
    ipe_learning_cache_valid_t learn_cache_vld;

    local_chip_num = sys_humber_get_local_chip_num();
    /*
    IpeLearningCacheValid.exceptionEn = 1:
    When alway cpu learning, set exceptionEn, then each needed learning pkt will send to cpu.
    When interrupt learning, set exceptionEn, cache full, then remained need learning pkt will send to cpu.
    */
    cmd = DRV_IOW(IOC_REG,
                  IPE_LEARNING_CACHE_VALID,
                  DRV_ENTRY_FLAG);

    kal_memset(&learn_cache_vld, 0, sizeof(ipe_learning_cache_valid_t));
    learn_cache_vld.always_cpu_learning = 0;
    learn_cache_vld.exception_en = 0;
    learn_cache_vld.learning_cache_int_thrd = 0;
    learn_cache_vld.learning_entry_valid = 0xFFFF;  /*must write one clear */



    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &learn_cache_vld));
    }
    g_aging_master.always_cpu_learning = 0;
    g_aging_master.learning_cache_int_thrd = 0;
    g_aging_master.learning_exception_en = 0;
    return CTC_E_NONE;
}

static int32
_sys_humber_aging_init(void)
{
    uint32 cmd = 0;
    uint8 lchip = 0;
    uint8 local_chip_num = 0;
    ipe_aging_ctl_t ipe_aging_ctl;
    uint32 dsaging_vaule = 0;
    uint32 dsaging_idx = 0;
    uint32 min_ptr = 0, max_ptr = 0;
      ipe_lookup_result_ctl_t rslt_ctl;

    local_chip_num = sys_humber_get_local_chip_num();


    SYS_LEARNING_AGING_DBG_FUNC();

    kal_memset(&rslt_ctl, 0, sizeof(rslt_ctl));
    cmd = DRV_IOR(IOC_REG, IPE_LOOKUP_RESULT_CTL, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &rslt_ctl));
    g_aging_master.aging_base= (rslt_ctl.mac_sa_lookup_result_ctl0 & 0xFFF);

    kal_memset(&ipe_aging_ctl, 0, sizeof(ipe_aging_ctl));

    /* Get FDB tabel's start and end index value */
    CTC_ERROR_RETURN(sys_humber_l2_fdb_get_fdb_tbl_range(&min_ptr, &max_ptr));
    ipe_aging_ctl.min_ptr = 0 ;

    if (max_ptr % SYS_AGING_STATUS_CHECK_BIT_LEN)
    {
       max_ptr = max_ptr/SYS_AGING_STATUS_CHECK_BIT_LEN + 1;
    }
    else
    {
        max_ptr = max_ptr / SYS_AGING_STATUS_CHECK_BIT_LEN;
    }
    if (max_ptr > 0x3FFF)
    { /*only support aging 256K entry*/
       ipe_aging_ctl.max_ptr  = 0x3FFF;
    }
    else
    {
       ipe_aging_ctl.max_ptr  = max_ptr;
    }
     g_aging_master.aging_width =   ipe_aging_ctl.max_ptr -  ipe_aging_ctl.min_ptr;


    /* The corresponding aging status bit clears to 0 after CPU read */
    ipe_aging_ctl.software_read_clear = 1;
    /* Aging scanning stops when aging FIFO is almost full */
    ipe_aging_ctl.scan_pause_on_fifo_full = 1;
    /* Aging scanning stops on maxPtr */
    ipe_aging_ctl.stop_on_max_ptr = 0;
    /* DsAging contains 16 bits of entry valid indication and 16 bits of aging status */
    ipe_aging_ctl.entry_valid_check_en = 1;
    /* An interrupt will be generated when aging FIFO's depth reachs this value */
    ipe_aging_ctl.fifo_depth_threshold = 0;
    ipe_aging_ctl.scan_en = 0;
    cmd = DRV_IOW(IOC_REG,  IPE_AGING_CTL,   DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &ipe_aging_ctl));
    }

    cmd = DRV_IOW(IOC_TABLE, IPE_AGING_RAM, DRV_ENTRY_FLAG);
    for (lchip = 0; lchip < local_chip_num; lchip++)
    {
        for (dsaging_idx = 0; dsaging_idx < 16384; dsaging_idx++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, dsaging_idx, cmd, &dsaging_vaule));
        }
    }
    sys_humber_aging_set_aging_interval(300); /*30s*/
    sys_humber_aging_set_fifo_threshold(SYS_AGING_DEFAULT_THRESHOLD);

    return CTC_E_NONE;
}

int32
sys_humber_learning_aging_init(void)
{
    CTC_ERROR_RETURN(_sys_humber_learning_init());
    CTC_ERROR_RETURN(_sys_humber_aging_init());
    return CTC_E_NONE;
}

