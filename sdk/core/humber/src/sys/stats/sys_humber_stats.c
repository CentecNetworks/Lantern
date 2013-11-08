/**
 @file sys_humber_stats.c

 @date 2009-12-22

 @version v2.0

*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_error.h"
#include "ctc_macro.h"
#include "ctc_stats.h"
#include "sys_humber_stats.h"
#include "sys_humber_ftm.h"
#include "sys_humber_chip.h"
#include "sys_humber_opf.h"
#include "sys_humber_register.h"
#include "drv_humber_data_path.h"
#include "drv_io.h"

/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/
#define MAC_BASED_STATS_GMAC_RAM_DEPTH      256
#define MAC_BASED_STATS_XGMAC_RAM_DEPTH 48
#define MAC_BASED_STATS_SGMAC_RAM_DEPTH      48

#define SYS_STATS_DEFAULT_WRR 2
#define SYS_STATS_DEFAULT_FIFO_DEPTH_THRESHOLD      15
#define SYS_STATS_DEFAULT_BYTE_THRESHOLD 0x800
#define SYS_STATS_DEFAULT_PACKET_THRESHOLD     0x800

#define SYS_STATS_PHB_PORT_NUM_MAX 63

#define SYS_STATS_FWD_INTERNAL_SRAM_MAX 4096

#define SYS_STATS_POLICER_CONFIRM_BASE 0
#define SYS_STATS_POLICER_NOT_CONFIRM_BASE 256
#define SYS_STATS_POLICER_VIOLATE_BASE 512

#define SYS_STATS_POLICER_NO_CONFLICT_CONFIRM_BASE 0
#define SYS_STATS_POLICER_NO_CONFLICT_NOT_CONFIRM_BASE 512
#define SYS_STATS_POLICER_NO_CONFLICT_VIOLATE_BASE 1024

#define SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID 320
#define SYS_STATS_POLICER_NO_CONFLICT_STATS_NUM   512

#define SYS_STATS_QUEUE_DEQ_BASE 2816
#define SYS_STATS_QUEUE_DEQ_NO_CONFLICT_BASE 2048
#define SYS_STATS_QUEUE_DEQ_OPENFLOW_BASE 3584   /* 768(policer) + 2816(forwarding) = 3584 */
#define SYS_STATS_POLICER_OPENFLOW_CONFIRM_BASE 0
#define SYS_STATS_POLICER_OPENFLOW_NOT_CONFIRM_BASE 256
#define SYS_STATS_POLICER_OPENFLOW_VIOLATE_BASE 512


#define SYS_STATS_POLICER_CONFIRM_BASE_WITH_EXT_QDR 4096 /*4k*/
#define SYS_STATS_POLICER_NOT_CONFIRM_BASE_WITH_EXT_QDR 24576 /*24k*/
#define SYS_STATS_POLICER_VIOLATE_BASE_WITH_EXT_QDR 49152 /*48k*/
#define SYS_STATS_QUEUE_DEQ_BASE_WITH_EXT_QDR 0
#define SYS_STATS_QUEUE_DROP_BASE_WITH_EXT_QDR 2048

#define SYS_STATS_DYN_INDEX_BASE_WITH_EXT_QDR 73728 /*72K*/
#define SYS_STATS_DYN_INDEX_MAX_WITH_EXT_QDR 36864      /*36K*/
#define SYS_STATS_VLAN_TAG_LEN 4

#define SYS_STATS_MTU_PKT_MIN_LENGTH 1024
#define SYS_STATS_MTU_PKT_MAX_LENGTH 16383
#define SYS_STATS_MTU1_PKT_DFT_LENGTH 1518
#define SYS_STATS_MTU2_PKT_DFT_LENGTH 1536
#define SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE 4

#define SYS_STATS_DEFAULT_CPU_MAC_PORT 53

#define SYS_STATS_MAX_FIFO_DEPTH 16

#define IS_GMAC_STATS(mac_ram_type) \
        ((mac_ram_type == SYS_STATS_MAC_STATS_RAM0) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM1) || \
         (mac_ram_type == SYS_STATS_MAC_STATS_RAM2) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM3) || \
         (mac_ram_type == SYS_STATS_MAC_STATS_RAM4) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM5) || \
         (mac_ram_type == SYS_STATS_MAC_STATS_RAM6) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM7) || \
         (mac_ram_type == SYS_STATS_MAC_STATS_RAM8) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM9) || \
         (mac_ram_type == SYS_STATS_MAC_STATS_RAM10) || (mac_ram_type == SYS_STATS_MAC_STATS_RAM11) || \
         (mac_ram_type == SYS_STATS_CPUMAC_STATS_RAM)  )

#define IS_MAC_PORT(lport) \
       ((lport<=51) || (lport == SYS_STATS_DEFAULT_CPU_MAC_PORT))

#define IS_GMAC_PORT(lport) \
       (lport<=47)

#define IS_SGMAC_PORT(lport) \
       ((lport>=48) && (lport<=51))

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/

union sys_macstats_u
{
    quadmacapp0_stats_ram_t gmac_stats;    /*gmac, cpu mac*/
    sgmac0_sgmac_stats_ram_t sxgmac_stats;    /*sgmac, xgmac*/
};
typedef union sys_macstats_u sys_macstats_t;

struct sys_stats_master_s
{
    uint8 port_opt_in;
    uint8 port_opt_out;

    uint8 log_port_discard_en;
    uint8 saturate_en[CTC_STATS_TYPE_MAX];
    uint8 hold_en[CTC_STATS_TYPE_MAX];
    uint8 clear_read_en[CTC_STATS_TYPE_MAX];

    uint8 policing_en[MAX_LOCAL_CHIP_NUM];
    uint8 queue_en[MAX_LOCAL_CHIP_NUM];
    uint8 fwd_stats_en[CTC_STATS_FWD_TYPE_MAX];

    uint8 fifo_depth_threshold;
    uint16 pkt_cnt_threshold;
    uint16 byte_cnt_threshold;

    uint16 mtu1_length[SYS_STATS_MAC_STATS_RAM_MAX];
    uint16 mtu2_length[SYS_STATS_MAC_STATS_RAM_MAX];
    uint16 dot1q_subtract[SYS_STATS_CPUMAC_STATS_RAM+1];

};
typedef struct sys_stats_master_s sys_stats_master_t;

sys_stats_master_t *stats_master = NULL;

static ctc_hash_t* sys_fwd_stats_hash[CTC_MAX_LOCAL_CHIP_NUM];
#define SYS_FWD_STATS_HASH_BLOCK_NUM      16
#define SYS_FWD_STATS_HASH_BLOCK_SIZE    256  /* total 8 * 512 = 4096 stats */

#define SYS_STATS_INIT_CHECK() \
    {\
        if(  stats_master == NULL)\
            return CTC_E_NOT_INIT;\
    }


/****************************************************************************
 *
* Function
*
*****************************************************************************/

static uint32
_sys_humber_stats_hash_make(sys_stats_fwd_stats_t* pst_key)
{
    return pst_key->stats_ptr;
}

static bool
_sys_humber_stats_hash_cmp (sys_stats_fwd_stats_t* pv_node_a, sys_stats_fwd_stats_t* pv_node_b)
{
    if (pv_node_a->stats_ptr != pv_node_b->stats_ptr)
    {
        return FALSE;
    }

    return TRUE;
}

static int32
_sys_humber_stats_init_start(void)
{
    drv_work_platform_type_t platform_type;

    drv_get_platform_type(&platform_type);
    if (platform_type == HW_PLATFORM)
    {
        uint32 cmd = 0;
        uint32 tmp = 1;
        uint8 lchip;
        uint8 chip_num = 0;

        chip_num = sys_humber_get_local_chip_num();

        for (lchip = 0; lchip < chip_num && lchip < MAX_LOCAL_CHIP_NUM; lchip++)
        {
            /* ingress port,phb: write IPE_STATS_INIT_IPE_PHB_INTF to init*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_PHB_INTF, IPE_STATS_INIT_IPE_PHB_INTF_INIT_IPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* ingress global fwd: write IPE_STATS_INIT_IPE_OVERALL_FWD to init*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_OVERALL_FWD, IPE_STATS_INIT_IPE_OVERALL_FWD_INIT_IPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* ingress port log: write IPE_STATS_INIT_IPE_PORT_LOG to init*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_PORT_LOG, IPE_STATS_INIT_IPE_PORT_LOG_INIT_IPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress port,phb: write EPE_STATS_INIT_EPE_PHB_INTF to init*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_PHB_INTF, EPE_STATS_INIT_EPE_PHB_INTF_INIT_EPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress global fwd: write EPE_STATS_INIT_EPE_OVERALL_FWD to init*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_OVERALL_FWD, EPE_STATS_INIT_EPE_OVERALL_FWD_INIT_EPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress port log: write EPE_STATS_INIT_EPE_PORT_LOG to init*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_PORT_LOG, EPE_STATS_INIT_EPE_PORT_LOG_INIT_EPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* fwd: write STATISTICS_INIT to init*/
            cmd = DRV_IOW(IOC_REG, STATISTICS_INIT, STATISTICS_INIT_CPU_INIT);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_deinit_start(void)
{

    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 lchip;
    uint8 chip_num = 0;
    drv_work_platform_type_t platform_type;

    drv_get_platform_type(&platform_type);

    if (platform_type == HW_PLATFORM)
    {
        chip_num = sys_humber_get_local_chip_num();

        for (lchip = 0; lchip < chip_num && lchip < MAX_LOCAL_CHIP_NUM; lchip++)
        {
            /* ingress port,phb: write IPE_STATS_INIT_IPE_PHB_INTF to deinit*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_PHB_INTF, IPE_STATS_INIT_IPE_PHB_INTF_INIT_IPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* ingress global fwd: write IPE_STATS_INIT_IPE_OVERALL_FWD to deinit*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_OVERALL_FWD, IPE_STATS_INIT_IPE_OVERALL_FWD_INIT_IPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* ingress port log: write IPE_STATS_INIT_IPE_PORT_LOG to init*/
            cmd = DRV_IOW(IOC_REG, IPE_STATS_INIT_IPE_PORT_LOG, IPE_STATS_INIT_IPE_PORT_LOG_INIT_IPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress port,phb: write EPE_STATS_INIT_EPE_PHB_INTF to deinit*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_PHB_INTF, EPE_STATS_INIT_EPE_PHB_INTF_INIT_EPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress global fwd: write EPE_STATS_INIT_EPE_OVERALL_FWD to deinit*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_OVERALL_FWD, EPE_STATS_INIT_EPE_OVERALL_FWD_INIT_EPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* egress port log: write EPE_STATS_INIT_EPE_PORT_LOG to init*/
            cmd = DRV_IOW(IOC_REG, EPE_STATS_INIT_EPE_PORT_LOG, EPE_STATS_INIT_EPE_PORT_LOG_INIT_EPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

            /* fwd: write STATISTICS_INIT to deinit*/
            cmd = DRV_IOW(IOC_REG, STATISTICS_INIT, STATISTICS_INIT_CPU_INIT);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_init_done(void)
{
    drv_work_platform_type_t platform_type;
    uint32 cmd = 0;
    uint32 tmp = 0;
    uint8 lchip;
    uint8 chip_num = 0;

    drv_get_platform_type(&platform_type);

    if (platform_type == HW_PLATFORM)
    {
        chip_num = sys_humber_get_local_chip_num();

        for (lchip = 0; lchip < chip_num && lchip < MAX_LOCAL_CHIP_NUM; lchip++)
        {
            /*ingress port,phb: read IPE_STATS_INIT_DONE_IPE_PHB_INTF to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, IPE_STATS_INIT_DONE_IPE_PHB_INTF, IPE_STATS_INIT_DONE_IPE_PHB_INTF_INIT_DONE_IPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /*ingress global fwd: read IPE_STATS_INIT_DONE_IPE_OVERALL_FWD to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, IPE_STATS_INIT_DONE_IPE_OVERALL_FWD, IPE_STATS_INIT_DONE_IPE_OVERALL_FWD_INIT_DONE_IPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /* ingress port log: read IPE_STATS_INIT_DONE_IPE_PORT_LOG to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, IPE_STATS_INIT_DONE_IPE_PORT_LOG, IPE_STATS_INIT_DONE_IPE_PORT_LOG_INIT_DONE_IPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /*egress port,phb: read EPE_STATS_INIT_DONE_EPE_PHB_INTF to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, EPE_STATS_INIT_DONE_EPE_PHB_INTF, EPE_STATS_INIT_DONE_EPE_PHB_INTF_INIT_DONE_EPE_PHB_INTF);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /*egress global fwd: read EPE_STATS_INIT_DONE_EPE_OVERALL_FWD to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, EPE_STATS_INIT_DONE_EPE_OVERALL_FWD, EPE_STATS_INIT_DONE_EPE_OVERALL_FWD_INIT_DONE_EPE_OVERALL_FWD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /* egress port log: read EPE_STATS_INIT_DONE_EPE_PORT_LOG to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, EPE_STATS_INIT_DONE_EPE_PORT_LOG, EPE_STATS_INIT_DONE_EPE_PORT_LOG_INIT_DONE_EPE_PORT_LOG);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

            /*fwd: read STATISTICS_INIT_DONE to decide whether init has done*/
            cmd = DRV_IOR(IOC_REG, STATISTICS_INIT_DONE, STATISTICS_INIT_DONE_CPU_INIT);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            if (0 == tmp)
            {
                return CTC_E_NOT_INIT;
            }

        }

    }
    return CTC_E_NONE;
}

static int32
_sys_humber_stats_mac_stats_init(uint8 lchip)
{
    uint32 reg_step = 0;
    uint32 reg_id = 0;
    uint32 field_id = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;
    uint32 index = 0;

    /* Xgmac */
    for(index=SYS_STATS_XGMAC_STATS_RAM0; index<=SYS_STATS_XGMAC_STATS_RAM3; index++)
    {
        if(drv_humber_xgmac_is_enable(index-SYS_STATS_XGMAC_STATS_RAM0))
        {
            /*mtu1 default value set*/
            reg_step = XGMAC1_XGMAC_STATS_MTU1 - XGMAC0_XGMAC_STATS_MTU1;
            reg_id = XGMAC0_XGMAC_STATS_MTU1 + (index-SYS_STATS_XGMAC_STATS_RAM0)*reg_step;
            field_id = XGMAC0_XGMAC_STATS_MTU1_PACKET_LEN_MTU1;
            tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[index] = (uint16)tmp;

            /*mtu2 default value set*/
            reg_step = XGMAC1_XGMAC_STATS_MTU2 - XGMAC0_XGMAC_STATS_MTU2;
            reg_id = XGMAC0_XGMAC_STATS_MTU2 + (index-SYS_STATS_XGMAC_STATS_RAM0)*reg_step;
            field_id = XGMAC0_XGMAC_STATS_MTU2_PACKET_LEN_MTU2;
            tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[index] = (uint16)tmp;
        }
    }

    /* Qmac */
    for(index=SYS_STATS_MAC_STATS_RAM1; index<=SYS_STATS_MAC_STATS_RAM9; index++)
    {
        if(drv_humber_qmac_is_enable(index))
        {
            /*mtu1 default value set*/
            reg_step = QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU1 - QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU1;
            reg_id = QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU1 + (index-SYS_STATS_MAC_STATS_RAM1)*reg_step;
            field_id = QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1;
            tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[index] = (uint16)tmp;

            /*mtu2 default value set*/
            reg_step = QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU2 - QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU2;
            reg_id = QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU2 + (index-SYS_STATS_MAC_STATS_RAM1)*reg_step;
            field_id = QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2;
            tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[index] = (uint16)tmp;

            /*dot1q default subtract value set*/
            reg_step = QUADMACAPP2_QUAD_MAC_APP_DOT1Q_DELTA_BYTES - QUADMACAPP1_QUAD_MAC_APP_DOT1Q_DELTA_BYTES;
            reg_id = QUADMACAPP1_QUAD_MAC_APP_DOT1Q_DELTA_BYTES + (index-SYS_STATS_MAC_STATS_RAM1)*reg_step;
            field_id = QUADMACAPP1_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES;
            tmp = SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[index] = (uint16)tmp;
        }
    }

    if(drv_humber_qmac_is_enable(SYS_STATS_MAC_STATS_RAM0))
    {
        /*mtu1 default value set*/
        tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                       QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM0] = (uint16)tmp;

        /*mtu2 default value set*/
        tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                       QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM0] = (uint16)tmp;

        /*dot1q default subtract value set*/
        tmp = SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                       QUADMACAPP0_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM0] = (uint16)tmp;
    }

    if(drv_humber_qmac_is_enable(SYS_STATS_MAC_STATS_RAM10))
    {
        /*mtu1 default value set*/
        tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                       QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM10] = (uint16)tmp;

        /*mtu2 default value set*/
        tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                       QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM10] = (uint16)tmp;

        /*dot1q default subtract value set*/
        tmp = SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                       QUADMACAPP10_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM10] = (uint16)tmp;
    }

    if(drv_humber_qmac_is_enable(SYS_STATS_MAC_STATS_RAM11))
    {
        /*mtu1 default value set*/
        tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                       QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM11] = (uint16)tmp;

        /*mtu2 default value set*/
        tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                       QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM11] = (uint16)tmp;

        /*dot1q default subtract value set*/
        tmp = SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE;
        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                       QUADMACAPP11_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM11] = (uint16)tmp;
    }

    /* Sgmac */
    for(index=SYS_STATS_SGMAC_STATS_RAM0; index<=SYS_STATS_SGMAC_STATS_RAM3; index++)
    {
        if(drv_humber_sgmac_is_enable(index-SYS_STATS_SGMAC_STATS_RAM0))
        {
            /*mtu1 default value set*/
            reg_step = SGMAC1_SGMAC_STATS_MTU1 - SGMAC0_SGMAC_STATS_MTU1;
            reg_id = SGMAC0_SGMAC_STATS_MTU1 + (index-SYS_STATS_SGMAC_STATS_RAM0)*reg_step;
            field_id = SGMAC0_SGMAC_STATS_MTU1_PACKET_LEN_MTU1;
            tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[index] = (uint16)tmp;

            /*mtu2 default value set*/
            reg_step = SGMAC1_SGMAC_STATS_MTU2 - SGMAC0_SGMAC_STATS_MTU2;
            reg_id = SGMAC0_SGMAC_STATS_MTU2 + (index-SYS_STATS_SGMAC_STATS_RAM0)*reg_step;
            field_id = SGMAC0_SGMAC_STATS_MTU2_PACKET_LEN_MTU2;
            tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
            cmd = DRV_IOW(IOC_REG, reg_id, field_id);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[index] = (uint16)tmp;
        }
    }

    /* cpumac */
    /*mtu1 default value set*/
    tmp = SYS_STATS_MTU1_PKT_DFT_LENGTH;
    cmd = DRV_IOW(IOC_REG, CPU_MAC_PACKET_LEN_MTU1, CPU_MAC_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    stats_master->mtu1_length[SYS_STATS_CPUMAC_STATS_RAM] = (uint16)tmp;

    /*mtu2 default value set*/
    tmp = SYS_STATS_MTU2_PKT_DFT_LENGTH;
    cmd = DRV_IOW(IOC_REG, CPU_MAC_PACKET_LEN_MTU2, CPU_MAC_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    stats_master->mtu2_length[SYS_STATS_CPUMAC_STATS_RAM] = (uint16)tmp;

    /*dot1q default subtract value set*/
    tmp = SYS_STATS_DOT1Q_SUBTRACT_DFT_BYTE;
    cmd = DRV_IOW(IOC_REG, CPU_MAC_DOT1Q_DELTA_BYTES, CPU_MAC_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
    stats_master->dot1q_subtract[SYS_STATS_CPUMAC_STATS_RAM] = (uint8)tmp;

    return CTC_E_NONE;
}

int32
sys_humber_stats_init(void)
{
    uint8 lchip;
    uint8 chip_num = 0;
    uint8 ext_qdr_en = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;
    int32 ret = CTC_E_NONE;
    uint32 stats_mode = 0;
    uint32 offset_start = 0, offset_num = 0;
    sys_humber_opf_t opf;

    kal_memset(&opf, 0, sizeof(sys_humber_opf_t));

    CTC_ERROR_RETURN(_sys_humber_stats_init_start());

    chip_num = sys_humber_get_local_chip_num();

    /*init global variable*/
    if (NULL != stats_master)
    {
        return CTC_E_NONE;
    }

    MALLOC_POINTER(sys_stats_master_t, stats_master);
    if (NULL == stats_master)
    {
        return CTC_E_NO_MEMORY;
    }
    kal_memset(stats_master, 0, sizeof(sys_stats_master_t));

    /*fwd stats dynamic alloc sram*/
    if((ret = sys_humber_opf_init(FWD_STATS_SRAM, chip_num)) < 0)
    {
        goto error;
    }

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
    sys_fwd_stats_hash[lchip] = ctc_hash_create(
        SYS_FWD_STATS_HASH_BLOCK_NUM,
        SYS_FWD_STATS_HASH_BLOCK_SIZE, 
        _sys_humber_stats_hash_make, 
        _sys_humber_stats_hash_cmp);

        /* default stats WRR configuration */
        tmp = SYS_STATS_DEFAULT_WRR;
        cmd = DRV_IOW(IOC_REG, STATISTICS_EPE_ACL_CREDIT, STATISTICS_EPE_ACL_CREDIT_EPE_ACL_CREDIT);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        cmd = DRV_IOW(IOC_REG, STATISTICS_IPE_FWD_CREDIT, STATISTICS_IPE_FWD_CREDIT_IPE_FWD_CREDIT);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        cmd = DRV_IOW(IOC_REG, STATISTICS_POLICING_CREDIT, STATISTICS_POLICING_CREDIT_POLICING_CREDIT);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        cmd = DRV_IOW(IOC_REG, STATISTICSQ_MGR_CREDIT, STATISTICSQ_MGR_CREDIT_Q_MGR_CREDIT);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        /* default stats interrupt configuration */
        stats_master->fifo_depth_threshold = SYS_STATS_DEFAULT_FIFO_DEPTH_THRESHOLD;
        tmp = SYS_STATS_DEFAULT_FIFO_DEPTH_THRESHOLD;
        cmd = DRV_IOW(IOC_REG, STATISTICS_FIFO_DEPTH_THRESHOLD, STATISTICS_FIFO_DEPTH_THRESHOLD_FIFO_DEPTH_THRESHOLD);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        stats_master->byte_cnt_threshold = SYS_STATS_DEFAULT_BYTE_THRESHOLD;
        tmp = SYS_STATS_DEFAULT_BYTE_THRESHOLD;
        cmd = DRV_IOW(IOC_REG, STATISTICS_BYTE_COUNT_THRESHOLD, STATISTICS_BYTE_COUNT_THRESHOLD_BYTE_COUNT_THRESHOLD);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        stats_master->pkt_cnt_threshold = SYS_STATS_DEFAULT_PACKET_THRESHOLD;
        tmp = SYS_STATS_DEFAULT_PACKET_THRESHOLD;
        cmd = DRV_IOW(IOC_REG, STATISTICS_PACKET_COUNT_THRESHOLD, STATISTICS_PACKET_COUNT_THRESHOLD_PACKET_COUNT_THRESHOLD);
        if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
        {
           goto error;
        }

        if((ret = sys_alloc_get_ext_qdr_en(&ext_qdr_en))<0)
        {
            goto error;
        }

        if(ext_qdr_en)
        {
            /*queue stats base*/
            cmd = DRV_IOW(IOC_REG, Q_MGR_QUE_DEQ_STATS_BASE, Q_MGR_QUE_DEQ_STATS_BASE_Q_MGR_QUE_DEQ_STATS_BASE);
            tmp = SYS_STATS_QUEUE_DEQ_BASE_WITH_EXT_QDR >> 8;
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_QUEUE_DEQ] = 1;

            cmd = DRV_IOW(IOC_REG, Q_MGR_QUE_DROP_STATS_BASE, Q_MGR_QUE_DROP_STATS_BASE_STATS_BASE);
            tmp = SYS_STATS_QUEUE_DROP_BASE_WITH_EXT_QDR >> 8;
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_QUEUE_DROP] = 1;

            /*policer stats base*/
            cmd = DRV_IOW(IOC_REG, POLICING_STATS_CONFIRM_BASE_PTR, POLICING_STATS_CONFIRM_BASE_PTR_STATS_CONFIRM_BASE_PTR);
            tmp = SYS_STATS_POLICER_CONFIRM_BASE_WITH_EXT_QDR >> 8;
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_GREEN] = 1;

            cmd = DRV_IOW(IOC_REG, POLICING_STATS_NOT_CONFIRM_BASE_PTR, POLICING_STATS_NOT_CONFIRM_BASE_PTR_STATS_NOT_CONFIRM_BASE_PTR);
            tmp = SYS_STATS_POLICER_NOT_CONFIRM_BASE_WITH_EXT_QDR >> 8;
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_YELLOW] = 1;

            cmd = DRV_IOW(IOC_REG, POLICING_STATS_NOT_CONFIRM_BASE_PTR, POLICING_STATS_NOT_CONFIRM_BASE_PTR_STATS_VIOLATE_BASE_PTR);
            tmp = SYS_STATS_POLICER_VIOLATE_BASE_WITH_EXT_QDR >> 8;
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_RED] = 1;

            stats_master->fwd_stats_en[CTC_STATS_FLOW] = 1;
            stats_master->fwd_stats_en[CTC_STATS_MPLS] = 1;

            /*fwd stats dynamic alloc sram*/
            /*call function get max queue id*/
            opf.pool_type = FWD_STATS_SRAM;
            opf.pool_index = lchip;
            if((ret = sys_humber_opf_init_offset(&opf, SYS_STATS_DYN_INDEX_BASE_WITH_EXT_QDR, SYS_STATS_DYN_INDEX_MAX_WITH_EXT_QDR)) < 0)
            {
                goto error;
            }
        }
        else
        {
            CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));
            /*queue stats base.only deq,no drop*/
            cmd = DRV_IOW(IOC_REG, Q_MGR_QUE_DEQ_STATS_BASE, Q_MGR_QUE_DEQ_STATS_BASE_Q_MGR_QUE_DEQ_STATS_BASE);
            if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
            {
                tmp = SYS_STATS_QUEUE_DEQ_NO_CONFLICT_BASE >> 8;
            }
            else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
            {
                tmp = SYS_STATS_QUEUE_DEQ_OPENFLOW_BASE >> 8;
            }
            else
            {
                tmp = SYS_STATS_QUEUE_DEQ_BASE >> 8;
            }
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_QUEUE_DEQ] = 1;

            /*policer stats base*/
            cmd = DRV_IOW(IOC_REG, POLICING_STATS_CONFIRM_BASE_PTR, POLICING_STATS_CONFIRM_BASE_PTR_STATS_CONFIRM_BASE_PTR);
            if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
            {
                tmp = SYS_STATS_POLICER_NO_CONFLICT_CONFIRM_BASE >> 8;
            }
            else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
            {
                tmp = SYS_STATS_POLICER_OPENFLOW_CONFIRM_BASE >> 8;
            }
            else
            {
                tmp = SYS_STATS_POLICER_CONFIRM_BASE >> 8;
            }
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_GREEN] = 1;

            cmd = DRV_IOW(IOC_REG, POLICING_STATS_NOT_CONFIRM_BASE_PTR, POLICING_STATS_NOT_CONFIRM_BASE_PTR_STATS_NOT_CONFIRM_BASE_PTR);
            if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
            {
                tmp = SYS_STATS_POLICER_NO_CONFLICT_NOT_CONFIRM_BASE >> 8;
            }
            else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
            {
                tmp = SYS_STATS_POLICER_OPENFLOW_NOT_CONFIRM_BASE >> 8;
            }
            else
            {
                tmp = SYS_STATS_POLICER_NOT_CONFIRM_BASE >> 8;
            }
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_YELLOW] = 1;

            cmd = DRV_IOW(IOC_REG, POLICING_STATS_NOT_CONFIRM_BASE_PTR, POLICING_STATS_NOT_CONFIRM_BASE_PTR_STATS_VIOLATE_BASE_PTR);
            if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
            {
                tmp = SYS_STATS_POLICER_NO_CONFLICT_VIOLATE_BASE >> 8;
            }
            else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
            {
                tmp = SYS_STATS_POLICER_OPENFLOW_VIOLATE_BASE >> 8;
            }
            else
            {
                tmp = SYS_STATS_POLICER_VIOLATE_BASE >> 8;
            }
            if((ret = drv_reg_ioctl(lchip, 0, cmd, &tmp)) < 0)
            {
                goto error;
            }
            stats_master->fwd_stats_en[CTC_STATS_POLICING_RED] = 1;

            stats_master->fwd_stats_en[CTC_STATS_FLOW] = 1;
            stats_master->fwd_stats_en[CTC_STATS_MPLS] = 1;

            /*fwd stats dynamic alloc sram*/
            /*call function get max queue id*/
            opf.pool_type = FWD_STATS_SRAM;
            opf.pool_index = lchip;
            /*from 0 to 2047*/
            offset_start = 0;
            offset_num = 2048;

            offset_start = 768; /* flow policer stats 256*3 = 768 */
            offset_num = 2816;  /* 2560 flow stats is enough for 2560 flows.
                                   4 for default entry 
                                   252 stats for group stats */
                                /* 512 stats left, for future queue stats */
            if((ret = sys_humber_opf_init_offset(&opf, offset_start, offset_num)) < 0)
            {
                goto error;
            }
            if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
            {
                uint32 temp_stats_offset = 0;
                /*
                  The assignment of 4k stats sram.
                  320(a) 192(b) 320(c) 192(d) 320(e) 192(f) 512(g) 2048(h)
                  a, c, e    : for confirm/exceed/violate statistics of policer
                  b, d, f, g : for forwarding statistics
                  h          : for dequeue stats
                */

                ret |= sys_humber_opf_alloc_offset(&opf,
                    SYS_STATS_POLICER_NO_CONFLICT_STATS_NUM, &temp_stats_offset);
                ret |= sys_humber_opf_alloc_offset(&opf,
                    SYS_STATS_POLICER_NO_CONFLICT_STATS_NUM, &temp_stats_offset);
                ret |= sys_humber_opf_alloc_offset(&opf,
                    SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID, &temp_stats_offset);
                ret |= sys_humber_opf_free_offset(&opf,
                    SYS_STATS_POLICER_NO_CONFLICT_STATS_NUM - SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID,
                    SYS_STATS_POLICER_NO_CONFLICT_CONFIRM_BASE + SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID);
                ret |= sys_humber_opf_free_offset(&opf,
                    SYS_STATS_POLICER_NO_CONFLICT_STATS_NUM - SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID,
                    SYS_STATS_POLICER_NO_CONFLICT_NOT_CONFIRM_BASE + SYS_STATS_POLICER_NO_CONFLICT_STATS_VALID);
                if (ret)
                {
                    goto error;
                }
            }
        }

        /*default mac stats configuration*/
        if((ret = _sys_humber_stats_mac_stats_init(lchip)) < 0)
        {
            goto error;
        }
    }

    if((ret = _sys_humber_stats_init_done()) < 0)
    {
        goto error;
    }

    return CTC_E_NONE;

    error:
        _sys_humber_stats_deinit_start();

        return ret;
}

static int32
_sys_humber_stats_get_mac_ram_type(uint16 gport, uint8* ram_type)
{
    uint8 lchip = 0;
    uint8 lport = 0;

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    if(!IS_MAC_PORT(lport))
    {
        return CTC_E_STATS_PORT_NOT_MAP_TO_MAC;
    }

    if(IS_GMAC_PORT(lport))
    {
        if(drv_humber_gmac_is_enable(lport))
        {
            *ram_type = lport>>2;
        }
        else if((lport%12 == 0) && drv_humber_xgmac_is_enable(lport/12))
        {
            *ram_type = (lport/12) + SYS_STATS_XGMAC_STATS_RAM0;
        }
        else
        {
            return CTC_E_STATS_PORT_NOT_ENABLE;
        }
    }
    else
    {
        if(IS_SGMAC_PORT(lport))
        {
            if(drv_humber_sgmac_is_enable(lport-48))
            {
                *ram_type = (lport&0x3) + SYS_STATS_SGMAC_STATS_RAM0;
            }
            else
            {
                return CTC_E_STATS_PORT_NOT_ENABLE;
            }
        }
        else
        {
            *ram_type = SYS_STATS_CPUMAC_STATS_RAM;
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_mac_packet_length_mtu1(uint16 gport, uint16 length)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    uint16 mtu2_len = 0;
    uint32 cmd = 0;
    uint32 tmp = length;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_MIN_VALUE_CHECK(tmp, SYS_STATS_MTU_PKT_MIN_LENGTH);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    mtu2_len = stats_master->mtu2_length[mac_ram_type];
    if(tmp >= mtu2_len)
    {
        return CTC_E_STATS_MTU1_GREATER_MTU2;
    }
    CTC_MAX_VALUE_CHECK(tmp, SYS_STATS_MTU_PKT_MAX_LENGTH);

    if(stats_master->mtu1_length[mac_ram_type] == tmp)
    {
        return CTC_E_NONE;
    }

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP3_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP4_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM4] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP5_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM5] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP6_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM6] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP7_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM7] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP8_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM8] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP9_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM9] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM10] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU1, \
                           QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_MAC_STATS_RAM11] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, XGMAC0_XGMAC_STATS_MTU1, XGMAC0_XGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_XGMAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, XGMAC1_XGMAC_STATS_MTU1, XGMAC1_XGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_XGMAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, XGMAC2_XGMAC_STATS_MTU1, XGMAC2_XGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_XGMAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, XGMAC3_XGMAC_STATS_MTU1, XGMAC3_XGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_XGMAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, SGMAC0_SGMAC_STATS_MTU1, SGMAC0_SGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_SGMAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, SGMAC1_SGMAC_STATS_MTU1, SGMAC1_SGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_SGMAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, SGMAC2_SGMAC_STATS_MTU1, SGMAC2_SGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_SGMAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, SGMAC3_SGMAC_STATS_MTU1, SGMAC3_SGMAC_STATS_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_SGMAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOW(IOC_REG, CPU_MAC_PACKET_LEN_MTU1, CPU_MAC_PACKET_LEN_MTU1_PACKET_LEN_MTU1);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu1_length[SYS_STATS_CPUMAC_STATS_RAM] = (uint16)tmp;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_mac_packet_length_mtu1(uint16 gport, uint16* p_length)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_length);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(lport, &mac_ram_type));

    *p_length = stats_master->mtu1_length[mac_ram_type];

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_mac_packet_length_mtu2(uint16 gport, uint16 length)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    uint16 mtu1_len = 0;
    uint32 cmd = 0;
    uint32 tmp = length;

    SYS_STATS_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_MIN_VALUE_CHECK(tmp, SYS_STATS_MTU_PKT_MIN_LENGTH);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    /*mtu2 length must greater than mtu1 length*/
    mtu1_len = stats_master->mtu1_length[mac_ram_type];
    if(tmp <= mtu1_len)
    {
        return CTC_E_STATS_MTU2_LESS_MTU1;
    }
    CTC_MAX_VALUE_CHECK(tmp, SYS_STATS_MTU_PKT_MAX_LENGTH);

    if(stats_master->mtu2_length[mac_ram_type] == tmp)
    {
        return CTC_E_NONE;
    }

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP0_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP1_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP2_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP3_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP4_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM4] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP5_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM5] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP6_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM6] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP7_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM7] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP8_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM8] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP9_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM9] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP10_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM10] = (uint16)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU2, \
                           QUADMACAPP11_QUAD_MAC_APP_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_MAC_STATS_RAM11] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, XGMAC0_XGMAC_STATS_MTU2, XGMAC0_XGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_XGMAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, XGMAC1_XGMAC_STATS_MTU2, XGMAC1_XGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_XGMAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, XGMAC2_XGMAC_STATS_MTU2, XGMAC2_XGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_XGMAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, XGMAC3_XGMAC_STATS_MTU2, XGMAC3_XGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_XGMAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, SGMAC0_SGMAC_STATS_MTU2, SGMAC0_SGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_SGMAC_STATS_RAM0] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, SGMAC1_SGMAC_STATS_MTU2, SGMAC1_SGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_SGMAC_STATS_RAM1] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, SGMAC2_SGMAC_STATS_MTU2, SGMAC2_SGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_SGMAC_STATS_RAM2] = (uint16)tmp;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, SGMAC3_SGMAC_STATS_MTU2, SGMAC3_SGMAC_STATS_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_SGMAC_STATS_RAM3] = (uint16)tmp;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOW(IOC_REG, CPU_MAC_PACKET_LEN_MTU2, CPU_MAC_PACKET_LEN_MTU2_PACKET_LEN_MTU2);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->mtu2_length[SYS_STATS_CPUMAC_STATS_RAM] = (uint16)tmp;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_mac_packet_length_mtu2(uint16 gport, uint16* p_length)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_length);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(lport, &mac_ram_type));

    *p_length = stats_master->mtu2_length[mac_ram_type];

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_dot1q_subtract_bytes_check(uint8 byte)
{
    if((byte != 0)&&(byte != 4)&&(byte != 8)&&(byte != 12))
    {
        return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_mac_dot1q_subtract_bytes(uint16 gport, uint8 byte)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    uint32 cmd = 0;
    uint32 tmp = byte;

    SYS_STATS_INIT_CHECK();
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    CTC_ERROR_RETURN(_sys_humber_stats_dot1q_subtract_bytes_check(byte));

    /*get mac index ,channel of lport from function to justify mac ram*/
    if(IS_GMAC_PORT(lport))
    {
        mac_ram_type = lport>>2;
    }
    else
    {
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT == lport)
        {
            mac_ram_type = SYS_STATS_CPUMAC_STATS_RAM;
        }
        else
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP0_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM0] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP1_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM1] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP2_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM2] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP3_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM3] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP4_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM4] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP5_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM5] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP6_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM6] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP7_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM7] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP8_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM8] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP9_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM9] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP10_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM10] = (uint8)tmp;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_DOT1Q_DELTA_BYTES, \
                           QUADMACAPP11_QUAD_MAC_APP_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_MAC_STATS_RAM11] = (uint8)tmp;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOW(IOC_REG, CPU_MAC_DOT1Q_DELTA_BYTES, CPU_MAC_DOT1Q_DELTA_BYTES_DOT1Q_DELTA_BYTES);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            stats_master->dot1q_subtract[SYS_STATS_CPUMAC_STATS_RAM] = (uint8)tmp;
            break;

        default:
            return CTC_E_INVALID_PARAM;

    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_mac_dot1q_subtract_bytes(uint16 gport, uint8* p_byte)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_byte);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*judge mac ram type,max equal to cpu mac*/
    /*get mac index ,channel of lport from function to justify mac ram*/
    if(IS_GMAC_PORT(lport))
    {
        mac_ram_type = lport>>2;
    }
    else
    {
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT == lport)
        {
            mac_ram_type = SYS_STATS_CPUMAC_STATS_RAM;
        }
        else
        {
            return CTC_E_INVALID_PARAM;
        }
    }

    *p_byte = stats_master->dot1q_subtract[mac_ram_type];

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_mac_stats_to_basic(bool is_gmac, sys_macstats_t mac_stats, ctc_stats_basic_t* basic_stats)
{
    uint64 tmp = 0;

    CTC_PTR_VALID_CHECK(basic_stats);

    /*judge gmac or xgmac,sgmac through mac ram type*/
    if(TRUE == is_gmac)
    {
        tmp = mac_stats.gmac_stats.frame_cnt_data_hi;
        tmp <<= 32;
        tmp |= mac_stats.gmac_stats.frame_cnt_data_lo;
        basic_stats->packet_count = tmp;

        tmp = 0;
        tmp = mac_stats.gmac_stats.byte_cnt_data_hi;
        tmp <<= 32;
        tmp |= mac_stats.gmac_stats.byte_cnt_data_lo;
        basic_stats->byte_count = tmp;
    }
    else
    {
        tmp = mac_stats.sxgmac_stats.frame_cnt_data_high;
        tmp <<= 32;
        tmp |= mac_stats.sxgmac_stats.frame_cnt_data_low;
        basic_stats->packet_count = tmp;

        tmp = 0;
        tmp = mac_stats.sxgmac_stats.byte_cnt_data_high;
        tmp <<= 32;
        tmp |= mac_stats.sxgmac_stats.byte_cnt_data_low;
        basic_stats->byte_count = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_mac_rx_stats(uint16 gport, ctc_stats_mac_rec_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    int32 base = 0;
    uint16 number = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    sys_macstats_t mac_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&mac_stats, 0, sizeof(sys_macstats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_mac_rec_t));

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP0_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP1_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP2_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP3_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP4_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP5_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP6_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP7_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP8_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP9_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP10_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP11_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, XGMAC0_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, XGMAC1_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, XGMAC2_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, XGMAC3_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, SGMAC0_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, SGMAC1_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, SGMAC2_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, SGMAC3_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOR(IOC_TABLE, CPUMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("mac_ram_type:%d, lport:%d, base:%d\n", mac_ram_type, lport, base);

    if(IS_GMAC_STATS(mac_ram_type))
	{
        /*gmac, cpu mac*/
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT != lport)
        {
            base = (lport&0x3) * (number / 4);
        }

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_JABBER+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->jabber_pkts = stats.packet_count;
        p_stats->jabber_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_COLLISION+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->collision_pkts = stats.packet_count;
        p_stats->collision_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_FCS_ERROR+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->fcs_error_pkts = stats.packet_count;
        p_stats->fcs_error_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_ALIGNMENT_ERROR+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->alignment_error_pkts = stats.packet_count;
        p_stats->alignment_error_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_MAC_OVERRUN+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->mac_overrun_pkts = stats.packet_count;
        p_stats->mac_overrun_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_UNDERSIZE+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->good_undersize_pkts = stats.packet_count;
        p_stats->good_undersize_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_63B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->good_63_pkts = stats.packet_count;
        p_stats->good_63_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_63B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->bad_63_pkts = stats.packet_count;
        p_stats->bad_63_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_1519B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->good_1519_pkts = stats.packet_count;
        p_stats->good_1519_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_1519B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->bad_1519_pkts = stats.packet_count;
        p_stats->bad_1519_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_JUMBO+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->good_jumbo_pkts = stats.packet_count;
        p_stats->good_jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_JUMBO+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->bad_jumbo_pkts = stats.packet_count;
        p_stats->bad_jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_64B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_64 = stats.packet_count;
        p_stats->bytes_64 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_127B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_65_to_127 = stats.packet_count;
        p_stats->bytes_65_to_127 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_255B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_128_to_255 = stats.packet_count;
        p_stats->bytes_128_to_255 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_511B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_256_to_511 = stats.packet_count;
        p_stats->bytes_256_to_511 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_1023B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_512_to_1023 = stats.packet_count;
        p_stats->bytes_512_to_1023 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_1518B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_1024_to_1518 = stats.packet_count;
        p_stats->bytes_1024_to_1518 = stats.byte_count;
	}
    else
    {
        /*sgmac, xgmac*/
        base = 0;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_JABBER+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->jabber_pkts = stats.packet_count;
        p_stats->jabber_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_COLLISION+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->collision_pkts = stats.packet_count;
        p_stats->collision_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_FCS_ERROR+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->fcs_error_pkts = stats.packet_count;
        p_stats->fcs_error_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_ALIGNMENT_ERROR+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->alignment_error_pkts = stats.packet_count;
        p_stats->alignment_error_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_MAC_OVERRUN+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->mac_overrun_pkts = stats.packet_count;
        p_stats->mac_overrun_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_UNDERSIZE+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->good_undersize_pkts = stats.packet_count;
        p_stats->good_undersize_bytes = stats.byte_count;

        /*gmac and sgmac,xgmac is different since offset 11 SYS_STATS_MAC_RCV_GOOD_OAM*/
        p_stats->gmac_good_oam_pkts = 0;
        p_stats->gmac_good_oam_bytes = 0;
        base = -1;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_63B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->good_63_pkts = stats.packet_count;
        p_stats->good_63_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_63B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->bad_63_pkts = stats.packet_count;
        p_stats->bad_63_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_1519B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->good_1519_pkts = stats.packet_count;
        p_stats->good_1519_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_1519B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->bad_1519_pkts = stats.packet_count;
        p_stats->bad_1519_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_GOOD_JUMBO+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->good_jumbo_pkts = stats.packet_count;
        p_stats->good_jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_BAD_JUMBO+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->bad_jumbo_pkts = stats.packet_count;
        p_stats->bad_jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_64B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_64 = stats.packet_count;
        p_stats->bytes_64 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_127B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_65_to_127 = stats.packet_count;
        p_stats->bytes_65_to_127 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_255B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_128_to_255 = stats.packet_count;
        p_stats->bytes_128_to_255 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_511B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_256_to_511 = stats.packet_count;
        p_stats->bytes_256_to_511 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_1023B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_512_to_1023 = stats.packet_count;
        p_stats->bytes_512_to_1023 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_RCV_1518B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_1024_to_1518 = stats.packet_count;
        p_stats->bytes_1024_to_1518 = stats.byte_count;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_mac_rx_stats(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    uint16 index = 0;
    int32 base = 0;
    uint16 number = 0;
    uint32 cmd = 0;
    sys_macstats_t mac_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP0_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP1_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP2_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP3_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP4_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP5_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP6_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP7_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP8_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP9_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP10_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP11_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, XGMAC0_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, XGMAC1_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, XGMAC2_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, XGMAC3_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, SGMAC0_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, SGMAC1_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, SGMAC2_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, SGMAC3_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOW(IOC_TABLE, CPUMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }


    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("mac_ram_type:%d, lport:%d, base:%d\n", mac_ram_type, lport, base);

    kal_memset(&mac_stats, 0, sizeof(sys_macstats_t));

    if(IS_GMAC_STATS(mac_ram_type))
	{
        /*gmac, cpu mac*/
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT != lport)
        {
            base = (lport&0x3) * (number / 4);
        }

        for(index=SYS_STATS_MAC_RCV_GOOD_UCAST+base;index<SYS_STATS_MAC_RCV_MAX+base;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &mac_stats.gmac_stats));
        }
    }
    else
    {
        /*sgmac, xgmac*/
        /*gmac and sgmac,xgmac is different since offset 11 SYS_STATS_MAC_RCV_GOOD_OAM
          gmac has 25 rev stats type, while sgmac,xgmac only has 24*/
        base = 0;
        for(index=SYS_STATS_MAC_RCV_GOOD_UCAST+base;index<SYS_STATS_MAC_RCV_MAX+base-1;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &mac_stats.sxgmac_stats));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_mac_tx_stats(uint16 gport, ctc_stats_mac_snd_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    int32 base = 0;
    uint16 number = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    sys_macstats_t mac_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&mac_stats, 0, sizeof(sys_macstats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_mac_snd_t));

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP0_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP1_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP2_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP3_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP4_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP5_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP6_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP7_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP8_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP9_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP10_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOR(IOC_TABLE, QUADMACAPP11_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, XGMAC0_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, XGMAC1_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, XGMAC2_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, XGMAC3_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOR(IOC_TABLE, SGMAC0_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOR(IOC_TABLE, SGMAC1_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOR(IOC_TABLE, SGMAC2_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOR(IOC_TABLE, SGMAC3_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOR(IOC_TABLE, CPUMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    /*gmac and sgmac,xgmac is different since offset 11 SYS_STATS_MAC_RCV_GOOD_OAM*/
    if(IS_GMAC_STATS(mac_ram_type))
	{
        /*gmac, cpu mac*/
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT != lport)
        {
            base = (lport&0x3) * (number / 4);
        }

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_63B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_63 = stats.packet_count;
        p_stats->bytes_63 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_64B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_64 = stats.packet_count;
        p_stats->bytes_64 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_127B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_65_to_127 = stats.packet_count;
        p_stats->bytes_65_to_127 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_255B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_128_to_255 = stats.packet_count;
        p_stats->bytes_128_to_255 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_511B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_256_to_511 = stats.packet_count;
        p_stats->bytes_256_to_511 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1023B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_512_to_1023 = stats.packet_count;
        p_stats->bytes_512_to_1023 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1518B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_1024_to_1518 = stats.packet_count;
        p_stats->bytes_1024_to_1518 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1519B+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->pkts_1519 = stats.packet_count;
        p_stats->bytes_1519 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_JUMBO+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->jumbo_pkts = stats.packet_count;
        p_stats->jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_FCS_ERROR+base, cmd, &mac_stats.gmac_stats));
        _sys_humber_stats_mac_stats_to_basic(TRUE, mac_stats, &stats);
        p_stats->fcs_error_pkts = stats.packet_count;
        p_stats->fcs_error_bytes = stats.byte_count;
    }
    else
    {
        /*sgmac, xgmac*/
        base = - 1;

        p_stats->good_oam_pkts = 0;
        p_stats->good_oam_bytes = 0;

        base = -2;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_63B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_63 = stats.packet_count;
        p_stats->bytes_63 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_64B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_64 = stats.packet_count;
        p_stats->bytes_64 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_127B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_65_to_127 = stats.packet_count;
        p_stats->bytes_65_to_127 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_255B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_128_to_255 = stats.packet_count;
        p_stats->bytes_128_to_255 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_511B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_256_to_511 = stats.packet_count;
        p_stats->bytes_256_to_511 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1023B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_512_to_1023 = stats.packet_count;
        p_stats->bytes_512_to_1023 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1518B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_1024_to_1518 = stats.packet_count;
        p_stats->bytes_1024_to_1518 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_1519B+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->pkts_1519 = stats.packet_count;
        p_stats->bytes_1519 = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_JUMBO+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->jumbo_pkts = stats.packet_count;
        p_stats->jumbo_bytes = stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MAC_SEND_FCS_ERROR+base, cmd, &mac_stats.sxgmac_stats));
        _sys_humber_stats_mac_stats_to_basic(FALSE, mac_stats, &stats);
        p_stats->fcs_error_pkts = stats.packet_count;
        p_stats->fcs_error_bytes = stats.byte_count;
    }

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("mac_ram_type:%d, lport:%d, base:%d\n", mac_ram_type, lport, base);

    return CTC_E_NONE;
}


int32
sys_humber_stats_reset_mac_tx_stats(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 mac_ram_type = 0;
    uint16 index = 0;
    int32 base = 0;
    uint16 number = 0;
    uint32 cmd = 0;
    sys_macstats_t mac_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    /*get mac index ,channel of lport from function to justify mac ram*/
    CTC_ERROR_RETURN(_sys_humber_stats_get_mac_ram_type(gport, &mac_ram_type));

    switch(mac_ram_type)
    {
        case SYS_STATS_MAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP0_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP1_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP2_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP3_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM4:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP4_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM5:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP5_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM6:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP6_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM7:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP7_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM8:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP8_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM9:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP9_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM10:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP10_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_MAC_STATS_RAM11:
            cmd = DRV_IOW(IOC_TABLE, QUADMACAPP11_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, XGMAC0_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, XGMAC1_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, XGMAC2_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_XGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, XGMAC3_XGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_XGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM0:
            cmd = DRV_IOW(IOC_TABLE, SGMAC0_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM1:
            cmd = DRV_IOW(IOC_TABLE, SGMAC1_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM2:
            cmd = DRV_IOW(IOC_TABLE, SGMAC2_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_SGMAC_STATS_RAM3:
            cmd = DRV_IOW(IOC_TABLE, SGMAC3_SGMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_SGMAC_RAM_DEPTH;
            break;

        case SYS_STATS_CPUMAC_STATS_RAM:
            cmd = DRV_IOW(IOC_TABLE, CPUMAC_STATS_RAM, DRV_ENTRY_FLAG);
            number = MAC_BASED_STATS_GMAC_RAM_DEPTH;
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    kal_memset(&mac_stats, 0, sizeof(sys_macstats_t));

    /*gmac and sgmac,xgmac is different since offset 11 SYS_STATS_MAC_RCV_GOOD_OAM*/
    if(IS_GMAC_STATS(mac_ram_type))
    {
        /*gmac, cpu mac*/
        if(SYS_STATS_DEFAULT_CPU_MAC_PORT != lport)
        {
            base = (lport&0x3) * (number / 4);
        }

        for(index=SYS_STATS_MAC_SEND_UCAST+base;index<SYS_STATS_MAC_SEND_MAX+base;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &mac_stats.gmac_stats));
        }
    }
    else
    {
        /*sgmac, xgmac*/
        base = - 1;

        for(index=SYS_STATS_MAC_SEND_UCAST+base;index<SYS_STATS_MAC_SEND_MAX+base-1;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &mac_stats.sxgmac_stats));
        }
    }

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("mac_ram_type:%d, lport:%d, base:%d\n", mac_ram_type, lport, base);

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_ds_stats_to_basic(ds_forwarding_stats_t ds_stats, ctc_stats_basic_t* basic_stats)
{
    uint64 tmp = 0;

    CTC_PTR_VALID_CHECK(basic_stats);

    basic_stats->packet_count = ds_stats.packet_count;

    tmp = ds_stats.byte_count_upper0;
    tmp <<= 3;
    tmp |= ds_stats.byte_count_upper;
    tmp <<= 32;
    tmp |= ds_stats.byte_count_lower;
    basic_stats->byte_count = tmp;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_igs_global_fwd_stats(ctc_stats_igs_global_fwd_t* p_stats)
{
    uint8 lchip;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    CTC_PTR_VALID_CHECK(p_stats);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(ctc_stats_igs_global_fwd_t));

    cmd = DRV_IOR(IOC_TABLE, IPE_STATSRAMIPEOVERALLFWD, DRV_ENTRY_FLAG);

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV4_UCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv4_ucast_rut_pkts += (uint32)stats.packet_count;
        p_stats->ipv4_ucast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV4_MCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv4_mcast_rut_pkts += (uint32)stats.packet_count;
        p_stats->ipv4_mcast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV6_UCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv6_ucast_rut_pkts += (uint32)stats.packet_count;
        p_stats->ipv6_ucast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV6_MCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv6_mcast_rut_pkts += (uint32)stats.packet_count;
        p_stats->ipv6_mcast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV4_UCAST_ROUTE_ESCAPE, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv4_ucast_rut_esp_pkts += (uint32)stats.packet_count;
        p_stats->ipv4_ucast_rut_esp_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV4_MCAST_ROUTE_ESCAPE, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv4_mcast_rut_esp_pkts += (uint32)stats.packet_count;
        p_stats->ipv4_mcast_rut_esp_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV6_UCAST_ROUTE_ESCAPE, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv6_ucast_rut_esp_pkts += (uint32)stats.packet_count;
        p_stats->ipv6_ucast_rut_esp_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_IPV6_MCAST_ROUTE_ESCAPE, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->ipv6_mcast_rut_esp_pkts += (uint32)stats.packet_count;
        p_stats->ipv6_mcast_rut_esp_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MPLS_UCAST_SWITCHED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->mpls_ucast_swt_pkts += (uint32)stats.packet_count;
        p_stats->mpls_ucast_swt_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MPLS_MCAST_SWITCHED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->mpls_mcast_swt_pkts += (uint32)stats.packet_count;
        p_stats->mpls_mcast_swt_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MPLS_UCAST_SWITCH_FATAL, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->mpls_ucast_swt_ftl_pkts += (uint32)stats.packet_count;
        p_stats->mpls_ucast_swt_ftl_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_MPLS_MCAST_SWITCH_FATAL, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->mpls_mcast_swt_ftl_pkts += (uint32)stats.packet_count;
        p_stats->mpls_mcast_swt_ftl_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_BRIDGE_BCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->bdg_bcast_pkts += (uint32)stats.packet_count;
        p_stats->bdg_bcast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_BRIDGE_MCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->bdg_mcast_pkts += (uint32)stats.packet_count;
        p_stats->bdg_mcast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_BRIDGE_UCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->bdg_ucast_pkts += (uint32)stats.packet_count;
        p_stats->bdg_ucast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_BRIDGE_ESCAPE, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->bdg_escape_pkts += (uint32)stats.packet_count;
        p_stats->bdg_escape_bytes += stats.byte_count;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_igs_global_fwd_stats(void)
{
    uint8 lchip;
    uint8 chip_num = 0;
    uint16 index = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    cmd = DRV_IOW(IOC_TABLE, IPE_STATSRAMIPEOVERALLFWD, DRV_ENTRY_FLAG);

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        for(index=SYS_STATS_IPV4_UCAST_ROUTED;index<SYS_STATS_INGRESS_GLOBAL_FWD_MAX;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_egs_global_fwd_stats(ctc_stats_egs_global_fwd_t* p_stats)
{
    uint8 lchip;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(ctc_stats_egs_global_fwd_t));

    cmd = DRV_IOR(IOC_TABLE, EPE_STATSRAMEPEOVERALLFWD, DRV_ENTRY_FLAG);

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_IPV4_UCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_ipv4_ucast_rut_pkts += (uint32)stats.packet_count;
        p_stats->pld_ipv4_ucast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_IPV4_MCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_ipv4_mcast_rut_pkts += (uint32)stats.packet_count;
        p_stats->pld_ipv4_mcast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_IPV6_UCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_ipv6_ucast_rut_pkts += (uint32)stats.packet_count;
        p_stats->pld_ipv6_ucast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_IPV6_MCAST_ROUTED, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_ipv6_mcast_rut_pkts += (uint32)stats.packet_count;
        p_stats->pld_ipv6_mcast_rut_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_BRIDGE_UCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_bdg_ucast_pkts += (uint32)stats.packet_count;
        p_stats->pld_bdg_ucast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_BRIDGE_MCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_bdg_mcast_pkts += (uint32)stats.packet_count;
        p_stats->pld_bdg_mcast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_BRIDGE_BCAST, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->pld_bdg_bcast_pkts += (uint32)stats.packet_count;
        p_stats->pld_bdg_bcast_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_PAYLOAD_NO_OP, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->payload_no_op_pkts += (uint32)stats.packet_count;
        p_stats->payload_no_op_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L3EDIT_NO_OP, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l3_edit_no_op_pkts += (uint32)stats.packet_count;
        p_stats->l3_edit_no_op_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L3EDIT_MPLS, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l3_edit_mpls_pkts += (uint32)stats.packet_count;
        p_stats->l3_edit_mpls_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L3EDIT_NAT, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l3_edit_nat_pkts += (uint32)stats.packet_count;
        p_stats->l3_edit_nat_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L3EDIT_TUNNEL, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l3_edit_tunnel_pkts += (uint32)stats.packet_count;
        p_stats->l3_edit_tunnel_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L3EDIT_FLEX, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l3_edit_flex_pkts += (uint32)stats.packet_count;
        p_stats->l3_edit_flex_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L2EDIT_NO_OP, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l2_edit_no_op_pkts += (uint32)stats.packet_count;
        p_stats->l2_edit_no_op_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L2EDIT_ETH, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l2_edit_eth_pkts += (uint32)stats.packet_count;
        p_stats->l2_edit_eth_bytes += stats.byte_count;

        CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, SYS_STATS_L2EDIT_FLEX, cmd, &ds_stats));
        _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
        p_stats->l2_edit_flex_pkts += (uint32)stats.packet_count;
        p_stats->l2_edit_flex_bytes += stats.byte_count;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_egs_global_fwd_stats(void)
{
    uint8 lchip;
    uint8 chip_num = 0;
    uint16 index = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    cmd = DRV_IOW(IOC_TABLE, EPE_STATSRAMEPEOVERALLFWD, DRV_ENTRY_FLAG);

    chip_num = sys_humber_get_local_chip_num();

    for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
    {
        for(index=SYS_STATS_PAYLOAD_IPV4_UCAST_ROUTED;index<SYS_STATS_EGRESS_GLOBAL_FWD_MAX;index++)
        {
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
        }
    }
    return CTC_E_NONE;
}

static int32
_sys_humber_stats_reset_igs_port_stats_all(uint8 lchip)
{
    uint16 lport = 0;
    uint16 gport = 0;

    for(lport=0; lport<MAX_LOCAL_PORT_NUM; lport++)
    {
        gport = (lchip<<CTC_LOCAL_PORT_LENGTH) + (lport&CTC_LOCAL_PORT_MASK);
        sys_humber_stats_reset_igs_port_stats(gport);
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_reset_egs_port_stats_all(uint8 lchip)
{
    uint16 lport = 0;
    uint16 gport = 0;

    for(lport=0; lport<MAX_LOCAL_PORT_NUM; lport++)
    {
        gport = (lchip<<CTC_LOCAL_PORT_LENGTH) + (lport&CTC_LOCAL_PORT_MASK);
        sys_humber_stats_reset_egs_port_stats(gport);
    }

    return CTC_E_NONE;
}


static int32
_sys_humber_stats_set_port_based_null_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_NULL)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable phb stats*/
                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_NULL;
        }
    }
    else if(CTC_EGRESS == direction)
    {
        if(stats_master->port_opt_out != CTC_STATS_PORT_BASED_NULL)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_egs_port_stats_all(lchip);

                /*config reg EPE_HDR_EDIT_CTL*/
                /*INTERFACE_STATS_EN = 0, */
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disbale phb stats*/
                /*config reg EPE_HDR_EDIT_CTL*/
                /*PHB_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_out = CTC_STATS_PORT_BASED_NULL;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_set_port_based_basic_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_BASIC)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable phb stats*/
                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_BASIC;
        }
    }
    else if(CTC_EGRESS == direction)
    {
        if(stats_master->port_opt_out != CTC_STATS_PORT_BASED_BASIC)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_egs_port_stats_all(lchip);

                /*config reg EPE_HDR_EDIT_CTL*/
                /*INTERFACE_STATS_EN = 1, */
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disbale phb stats*/
                /*config reg EPE_HDR_EDIT_CTL*/
                /*PHB_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_out = CTC_STATS_PORT_BASED_BASIC;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_set_port_based_mac_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_MAC)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 1*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable phb stats*/
                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_MAC;
        }

        return CTC_E_NONE;
    }

    /*egress port do not have port based mac stats*/
    return CTC_E_INVALID_PARAM;
}

static int32
_sys_humber_stats_set_port_based_protocol_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_PROTOCOL)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable phb stats*/
                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_PROTOCOL;
        }
    }
    else if(CTC_EGRESS == direction)
    {
        if(stats_master->port_opt_out != CTC_STATS_PORT_BASED_PROTOCOL)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_egs_port_stats_all(lchip);

                /*config reg EPE_HDR_EDIT_CTL*/
                /*INTERFACE_STATS_EN = 1, */
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 1*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disbale phb stats*/
                /*config reg EPE_HDR_EDIT_CTL*/
                /*PHB_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_out = CTC_STATS_PORT_BASED_PROTOCOL;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_set_phb_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_PHB)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable port stats*/
                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_PHB;
        }
    }
    else if(CTC_EGRESS == direction)
    {
        if(stats_master->port_opt_out != CTC_STATS_PORT_BASED_PHB)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_egs_port_stats_all(lchip);

                /*config reg EPE_HDR_EDIT_CTL*/
                /*PHB_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disbale port stats*/
                /*config reg EPE_HDR_EDIT_CTL*/
                /*INTERFACE_STATS_EN = 0, */
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_out = CTC_STATS_PORT_BASED_PHB;
        }
    }

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_set_phb_per_port_stats_en(ctc_direction_t direction)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 tmp = 0;
    uint32 cmd = 0;

    if(CTC_INGRESS == direction)
    {
        if(stats_master->port_opt_in != CTC_STATS_PORT_BASED_PHB_PER_PORT)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_igs_port_stats_all(lchip);

                /*config reg IPE_CLASSIFICATION_CTL*/
                /*PHB_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 1*/
                cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disable port stats*/
                /*config reg IPE_DS_VLAN_CTL*/
                /*INTERFACE_STATS_EN = 0*/
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*MAC_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_MAC_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, IPE_DS_VLAN_CTL, IPE_DS_VLAN_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_in = CTC_STATS_PORT_BASED_PHB_PER_PORT;
        }
    }
    else if(CTC_EGRESS == direction)
    {
        if(stats_master->port_opt_out != CTC_STATS_PORT_BASED_PHB_PER_PORT)
        {
            chip_num = sys_humber_get_local_chip_num();

            for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
            {
                /*clear former stats first*/
                _sys_humber_stats_reset_egs_port_stats_all(lchip);

                /*config reg EPE_HDR_EDIT_CTL*/
                /*PHB_STATS_EN = 1*/
                tmp = 1;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PHB_PER_PORT_STATS_EN = 1*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PER_PORT_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*disbale port stats*/
                /*config reg EPE_HDR_EDIT_CTL*/
                /*INTERFACE_STATS_EN = 0, */
                tmp = 0;
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_INTERFACE_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

                /*PROTOCOL_BASED_STATS_EN = 0*/
                cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PROTOCOL_BASED_STATS_EN);
                CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
            }

            stats_master->port_opt_out = CTC_STATS_PORT_BASED_PHB_PER_PORT;
        }
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_igs_port_stats_option(ctc_stats_port_stats_option_type_t type)
{
    SYS_STATS_INIT_CHECK();

    switch(type)
    {
        case CTC_STATS_PORT_BASED_NULL:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_null_stats_en(CTC_INGRESS));
            break;
        case CTC_STATS_PORT_BASED_BASIC:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_basic_stats_en(CTC_INGRESS));
            break;
        case CTC_STATS_PORT_BASED_MAC:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_mac_stats_en(CTC_INGRESS));
            break;
        case CTC_STATS_PORT_BASED_PROTOCOL:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_protocol_stats_en(CTC_INGRESS));
            break;
        case CTC_STATS_PORT_BASED_PHB:
            CTC_ERROR_RETURN(_sys_humber_stats_set_phb_stats_en(CTC_INGRESS));
            break;
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            CTC_ERROR_RETURN(_sys_humber_stats_set_phb_per_port_stats_en(CTC_INGRESS));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_igs_port_stats_option(ctc_stats_port_stats_option_type_t* p_type)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_type);

    *p_type = stats_master->port_opt_in;

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_egs_port_stats_option(ctc_stats_port_stats_option_type_t type)
{
    SYS_STATS_INIT_CHECK();

    switch(type)
    {
        case CTC_STATS_PORT_BASED_NULL:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_null_stats_en(CTC_EGRESS));
            break;
        case CTC_STATS_PORT_BASED_BASIC:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_basic_stats_en(CTC_EGRESS));
            break;
        case CTC_STATS_PORT_BASED_PROTOCOL:
            CTC_ERROR_RETURN(_sys_humber_stats_set_port_based_protocol_stats_en(CTC_EGRESS));
            break;
        case CTC_STATS_PORT_BASED_PHB:
            CTC_ERROR_RETURN(_sys_humber_stats_set_phb_stats_en(CTC_EGRESS));
            break;
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            CTC_ERROR_RETURN(_sys_humber_stats_set_phb_per_port_stats_en(CTC_EGRESS));
            break;
        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_egs_port_stats_option(ctc_stats_port_stats_option_type_t* p_type)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_type);

    *p_type = stats_master->port_opt_out;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_igs_port_stats(uint16 gport, ctc_stats_port_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(ctc_stats_port_t));

    CTC_ERROR_RETURN(sys_humber_stats_get_igs_port_stats_option(&(p_stats->type)));

    cmd = DRV_IOR(IOC_TABLE, IPE_STATSRAMIPEPHBINTF, DRV_ENTRY_FLAG);
    basic_index = lport<<2;

    switch(p_stats->type)
    {
        case CTC_STATS_PORT_BASED_BASIC:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.basic_port.port_pkts = stats.packet_count;
            p_stats->stats.basic_port.port_bytes = stats.byte_count;
            break;

        case CTC_STATS_PORT_BASED_MAC:
            index = basic_index + SYS_STATS_PORT_UCAST;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.mac_port.port_ucast_pkts = stats.packet_count;
            p_stats->stats.mac_port.port_ucast_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_ROUTED_MAC;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.mac_port.port_rut_mac_pkts = stats.packet_count;
            p_stats->stats.mac_port.port_rut_mac_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_MCAST;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.mac_port.port_mcast_pkts = stats.packet_count;
            p_stats->stats.mac_port.port_mcast_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_BCAST;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.mac_port.port_bcast_pkts = stats.packet_count;
            p_stats->stats.mac_port.port_bcast_bytes = stats.byte_count;
            break;

        case CTC_STATS_PORT_BASED_PROTOCOL:
            index = basic_index + SYS_STATS_PORT_IPV4;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_ipv4_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_ipv4_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_IPV6;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_ipv6_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_ipv6_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_MPLS;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_mpls_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_mpls_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_PROTOCOL_DEFAULT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_prtol_dft_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_prtol_dft_bytes = stats.byte_count;
            break;
        case CTC_STATS_PORT_BASED_NULL:
            return CTC_E_STATS_PORT_STATS_NO_TYPE;
        case CTC_STATS_PORT_BASED_PHB:
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            return CTC_E_STATS_PHB_STATS_INVALID;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_igs_port_stats(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint32 cmd = 0;
    ctc_stats_port_stats_option_type_t type = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    CTC_ERROR_RETURN(sys_humber_stats_get_igs_port_stats_option(&type));

    cmd = DRV_IOW(IOC_TABLE, IPE_STATSRAMIPEPHBINTF, DRV_ENTRY_FLAG);
    basic_index = lport<<2;

    switch(type)
    {
        case CTC_STATS_PORT_BASED_BASIC:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            break;

        case CTC_STATS_PORT_BASED_MAC:
            for(index = basic_index+SYS_STATS_PORT_UCAST;index < basic_index+SYS_STATS_PORT_BASED_MAC_MAX;index++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            }
            break;

        case CTC_STATS_PORT_BASED_PROTOCOL:
            for(index = basic_index+SYS_STATS_PORT_IPV4;index < basic_index+SYS_STATS_PORT_BASED_PROTOCOL_MAX;index++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            }
            break;
        case CTC_STATS_PORT_BASED_NULL:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            break;
        case CTC_STATS_PORT_BASED_PHB:
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            return CTC_E_STATS_PHB_STATS_INVALID;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_egs_port_stats(uint16 gport,        ctc_stats_port_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);
    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(ctc_stats_port_t));

    CTC_ERROR_RETURN(sys_humber_stats_get_egs_port_stats_option(&(p_stats->type)));

    cmd = DRV_IOR(IOC_TABLE, EPE_STATSRAMEPEPHBINTF, DRV_ENTRY_FLAG);
    basic_index = lport<<2;

    switch(p_stats->type)
    {
        case CTC_STATS_PORT_BASED_BASIC:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.basic_port.port_pkts = stats.packet_count;
            p_stats->stats.basic_port.port_bytes = stats.byte_count;
            break;

        case CTC_STATS_PORT_BASED_PROTOCOL:
            index = basic_index + SYS_STATS_PORT_IPV4;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_ipv4_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_ipv4_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_IPV6;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_ipv6_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_ipv6_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_MPLS;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_mpls_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_mpls_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_PORT_PROTOCOL_DEFAULT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->stats.prtol_port.port_prtol_dft_pkts = stats.packet_count;
            p_stats->stats.prtol_port.port_prtol_dft_bytes = stats.byte_count;
            break;
        case CTC_STATS_PORT_BASED_NULL:
            return CTC_E_STATS_PORT_STATS_NO_TYPE;
        case CTC_STATS_PORT_BASED_PHB:
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            return CTC_E_STATS_PHB_STATS_INVALID;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_egs_port_stats(uint16 gport)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint32 cmd = 0;
    ctc_stats_port_stats_option_type_t type = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    CTC_ERROR_RETURN(sys_humber_stats_get_egs_port_stats_option(&type));

    cmd = DRV_IOW(IOC_TABLE, EPE_STATSRAMEPEPHBINTF, DRV_ENTRY_FLAG);
    basic_index = lport<<2;

    switch(type)
    {
        case CTC_STATS_PORT_BASED_BASIC:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            break;

        case CTC_STATS_PORT_BASED_PROTOCOL:
            for(index = basic_index+SYS_STATS_PORT_IPV4;index < basic_index+SYS_STATS_PORT_BASED_PROTOCOL_MAX;index++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            }
            break;
        case CTC_STATS_PORT_BASED_NULL:
            index = lport;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index, cmd, &ds_stats));
            break;
        case CTC_STATS_PORT_BASED_PHB:
        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            return CTC_E_STATS_PHB_STATS_INVALID;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}


int32
sys_humber_stats_set_phb_cop_num(uint8 lchip, ctc_direction_t direction, uint8 cop_num)
{
    uint32 cmd = 0;
    uint32 tmp = cop_num;

    SYS_STATS_INIT_CHECK();
    SYS_STATS_INIT_CHECK();

    if(CTC_INGRESS == direction)
    {
        cmd = DRV_IOW(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PORT_STATS_SHIFT);
    }
    else if(CTC_EGRESS == direction)
    {
        cmd = DRV_IOW(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PORT_STATS_SHIFT);
    }

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, 0, cmd, &tmp));

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_phb_cop_num(uint8 lchip, ctc_direction_t direction, uint8* p_cop_num)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_cop_num);

    if(CTC_INGRESS == direction)
    {
        cmd = DRV_IOR(IOC_REG, IPE_CLASSIFICATION_CTL, IPE_CLASSIFICATION_CTL_PHB_PORT_STATS_SHIFT);
    }
    else if(CTC_EGRESS == direction)
    {
        cmd = DRV_IOR(IOC_REG, EPE_HDR_EDIT_CTL, EPE_HDR_EDIT_CTL_PHB_PORT_STATS_SHIFT);
    }

    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, 0, cmd, &tmp));

    *p_cop_num = (uint8)tmp;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_igs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop, sys_stats_igs_phb_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 cop_num = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint16 base = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);
    CTC_MIN_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB);
    CTC_MAX_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB_PER_PORT);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_STATS_PHB_PORT_NUM_MAX);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(sys_stats_igs_phb_t));

    sys_humber_stats_get_phb_cop_num(lchip, CTC_INGRESS, &cop_num);

    cmd = DRV_IOR(IOC_TABLE, IPE_STATSRAMIPEPHBINTF, DRV_ENTRY_FLAG);

    switch(type)
    {
        case CTC_STATS_PORT_BASED_PHB:
            basic_index = (phb_cop&cop_num)<<2;

            index = basic_index + SYS_STATS_INGRESS_PHB_IN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_in_pkts = stats.packet_count;
            p_stats->phb_in_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_out_pkts = stats.packet_count;
            p_stats->phb_out_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdown_pkts = stats.packet_count;
            p_stats->phb_markdown_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_MARKDROP;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdrop_pkts = stats.packet_count;
            p_stats->phb_markdrop_bytes = stats.byte_count;
            break;

        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            basic_index = (lport<<(2+cop_num)) + ((phb_cop&cop_num)<<2);

            index = basic_index + SYS_STATS_INGRESS_PHB_IN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_in_pkts = stats.packet_count;
            p_stats->phb_in_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_out_pkts = stats.packet_count;
            p_stats->phb_out_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdown_pkts = stats.packet_count;
            p_stats->phb_markdown_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_INGRESS_PHB_MARKDROP;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdrop_pkts = stats.packet_count;
            p_stats->phb_markdrop_bytes = stats.byte_count;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_igs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 cop_num = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint16 base = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_MIN_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB);
    CTC_MAX_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB_PER_PORT);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_STATS_PHB_PORT_NUM_MAX);

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    sys_humber_stats_get_phb_cop_num(lchip, CTC_INGRESS, &cop_num);

    cmd = DRV_IOW(IOC_TABLE, IPE_STATSRAMIPEPHBINTF, DRV_ENTRY_FLAG);

    switch(type)
    {
        case CTC_STATS_PORT_BASED_PHB:
            basic_index = (phb_cop&cop_num)<<2;
            for(index = basic_index+SYS_STATS_INGRESS_PHB_IN;index < basic_index+SYS_STATS_INGRESS_PHB_MAX;index++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            }
            break;

        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            basic_index = (lport<<(2+cop_num)) + ((phb_cop&cop_num)<<2);
            for(index = basic_index+SYS_STATS_INGRESS_PHB_IN;index < basic_index+SYS_STATS_INGRESS_PHB_MAX;index++)
            {
                CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            }
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_egs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop, sys_stats_egs_phb_t* p_stats)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 cop_num = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint16 base = 0;
    uint32 cmd = 0;
    ctc_stats_basic_t stats;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);
    CTC_MIN_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB);
    CTC_MAX_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB_PER_PORT);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_STATS_PHB_PORT_NUM_MAX);

    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(&ds_stats, 0, sizeof(ds_stats));
    kal_memset(p_stats, 0, sizeof(sys_stats_egs_phb_t));

    sys_humber_stats_get_phb_cop_num(lchip, CTC_EGRESS, &cop_num);

    cmd = DRV_IOR(IOC_TABLE, EPE_STATSRAMEPEPHBINTF, DRV_ENTRY_FLAG);

    switch(type)
    {
        case CTC_STATS_PORT_BASED_PHB:
            basic_index = (phb_cop&cop_num)<<2;

            index = basic_index + SYS_STATS_EGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_out_pkts = stats.packet_count;
            p_stats->phb_out_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_EGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdown_pkts = stats.packet_count;
            p_stats->phb_markdown_bytes = stats.byte_count;
            break;

        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            basic_index = (lport<<(2+cop_num)) + ((phb_cop&cop_num)<<2);

            index = basic_index + SYS_STATS_EGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_out_pkts = stats.packet_count;
            p_stats->phb_out_bytes = stats.byte_count;

            index = basic_index + SYS_STATS_EGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            _sys_humber_stats_ds_stats_to_basic(ds_stats, &stats);
            p_stats->phb_markdown_pkts = stats.packet_count;
            p_stats->phb_markdown_bytes = stats.byte_count;
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_egs_phb_stats(uint16 gport, ctc_stats_port_stats_option_type_t type, uint8 phb_cop)
{
    uint8 lchip = 0;
    uint8 lport = 0;
    uint8 cop_num = 0;
    uint16 index = 0;
    uint16 basic_index = 0;
    uint16 base = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_MIN_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB);
    CTC_MAX_VALUE_CHECK(type, CTC_STATS_PORT_BASED_PHB_PER_PORT);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);
    CTC_MAX_VALUE_CHECK(lport, SYS_STATS_PHB_PORT_NUM_MAX);

    kal_memset(&ds_stats, 0, sizeof(ds_stats));

    sys_humber_stats_get_phb_cop_num(lchip, CTC_EGRESS, &cop_num);

    cmd = DRV_IOW(IOC_TABLE, EPE_STATSRAMEPEPHBINTF, DRV_ENTRY_FLAG);

    switch(type)
    {
        case CTC_STATS_PORT_BASED_PHB:
            basic_index = (phb_cop&cop_num)<<2;

            index = basic_index + SYS_STATS_EGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));

            index = basic_index + SYS_STATS_EGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            break;

        case CTC_STATS_PORT_BASED_PHB_PER_PORT:
            basic_index = (lport<<(2+cop_num)) + ((phb_cop&cop_num)<<2);

            index = basic_index + SYS_STATS_EGRESS_PHB_OUT;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));

            index = basic_index + SYS_STATS_EGRESS_PHB_MARKDOWN;
            CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, index+base, cmd, &ds_stats));
            break;

        default:
            return CTC_E_INVALID_PARAM;
    }

    return CTC_E_NONE;

}

int32
sys_humber_stats_set_port_log_discard_stats_enable(bool enable)
{
    uint8 lchip = 0, lchip_num = 0;
    uint32 cmd = 0, tmp = 0;

    SYS_STATS_INIT_CHECK();

    tmp = (FALSE == enable)?0:1;

    if(stats_master->log_port_discard_en != enable)
    {
        lchip_num = sys_humber_get_local_chip_num();
        for(lchip=0; lchip<lchip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, IPE_FORWARD_CTL, IPE_FORWARD_CTL_LOG_PORT_DISCARD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        stats_master->log_port_discard_en = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_port_log_discard_stats_enable(bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->log_port_discard_en)?TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_igs_port_log_stats(uint16 gport, ctc_stats_basic_t* p_stats)
{
    uint8 lchip = 0, lport = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    cmd = DRV_IOR(IOC_TABLE, IPE_STATSRAMIPEPORTLOG, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &ds_stats));
    _sys_humber_stats_ds_stats_to_basic(ds_stats, p_stats);

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_igs_port_log_stats(uint16 gport)
{
    uint8 lchip = 0, lport = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));

    cmd = DRV_IOW(IOC_TABLE, IPE_STATSRAMIPEPORTLOG, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &ds_stats));

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_egs_port_log_stats(uint16 gport, ctc_stats_basic_t* p_stats)
{
    uint8 lchip = 0, lport = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    cmd = DRV_IOR(IOC_TABLE, EPE_STATSRAMEPEPORTLOG, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &ds_stats));
    _sys_humber_stats_ds_stats_to_basic(ds_stats, p_stats);

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_egs_port_log_stats(uint16 gport)
{
    uint8 lchip = 0, lport = 0;
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();

    SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport);

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));

    cmd = DRV_IOW(IOC_TABLE, EPE_STATSRAMEPEPORTLOG, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, lport, cmd, &ds_stats));

    return CTC_E_NONE;
}

static int32
_sys_stats_fwd_stats_entry_create(uint8 lchip, uint16 stats_ptr)
{
    sys_stats_fwd_stats_t *p_fwd_stats;

    p_fwd_stats = (sys_stats_fwd_stats_t *)mem_malloc(MEM_STATS_MODULE, sizeof(sys_stats_fwd_stats_t));
    if (NULL == p_fwd_stats)
    {
        return CTC_E_NO_MEMORY;
    }

    kal_memset(p_fwd_stats, 0, sizeof(sys_stats_fwd_stats_t));
    p_fwd_stats->stats_ptr = stats_ptr;

    ctc_hash_insert(sys_fwd_stats_hash[lchip], p_fwd_stats);

    return CTC_E_NONE;
}

static int32
_sys_stats_fwd_stats_entry_delete(uint8 lchip, uint16 stats_ptr, sys_stats_fwd_stats_t* p_fwd_stats)
{
    CTC_PTR_VALID_CHECK(p_fwd_stats);

    ctc_hash_remove(sys_fwd_stats_hash[lchip], p_fwd_stats);

    mem_free(p_fwd_stats);

    return CTC_E_NONE;
}

static int32
_sys_stats_fwd_stats_entry_lookup(uint8 lchip, uint16 stats_ptr, sys_stats_fwd_stats_t** pp_fwd_stats)
{
    sys_stats_fwd_stats_t lookup_key;

    CTC_PTR_VALID_CHECK(pp_fwd_stats);

    *pp_fwd_stats = NULL;

    lookup_key.stats_ptr = stats_ptr;
    *pp_fwd_stats = ctc_hash_lookup(sys_fwd_stats_hash[lchip], &lookup_key);

    return CTC_E_NONE;
}

int32
sys_humber_stats_create_statsptr(uint8 lchip, uint8 stats_szie, uint16* p_stats_ptr)
{
    uint32 offset = 0;
    sys_humber_opf_t opf;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats_ptr);

    kal_memset(&opf, 0, sizeof(sys_humber_opf_t));

    opf.pool_type = FWD_STATS_SRAM;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_alloc_offset(&opf, stats_szie, &offset));

    *p_stats_ptr = offset;

    /*add to fwd stats list,special for acl/qos,mpls stats*/
    CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_create(lchip, offset));

    return CTC_E_NONE;
}

int32
sys_humber_stats_delete_statsptr(uint8 lchip, uint8 stats_szie, uint16 stats_ptr)
{
    uint32 offset = 0;
    sys_humber_opf_t opf;
    sys_stats_fwd_stats_t *fwd_stats;

    SYS_STATS_INIT_CHECK();

    kal_memset(&opf, 0, sizeof(sys_humber_opf_t));

    offset = stats_ptr;
    opf.pool_type = FWD_STATS_SRAM;
    opf.pool_index = lchip;
    CTC_ERROR_RETURN(sys_humber_opf_free_offset(&opf, stats_szie, offset));

    /*remove from fwd stats list,special for acl/qos,mpls stats*/
    CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_lookup(lchip, stats_ptr, &fwd_stats));
    if(NULL != fwd_stats)
    {
        CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_delete(lchip, stats_ptr, fwd_stats));
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_fwd_stats_enable(ctc_stats_fwd_type_t fwd_stats_type, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->fwd_stats_en[fwd_stats_type])?TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_flow_stats(uint8 lchip, uint16 stats_ptr, ctc_stats_basic_t* p_stats)
{
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;
    sys_stats_fwd_stats_t *fwd_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    fwd_stats = NULL;

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    cmd = DRV_IOR(IOC_TABLE, DS_FORWARDING_STATS, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, stats_ptr, cmd, &ds_stats));
    _sys_humber_stats_ds_stats_to_basic(ds_stats, p_stats);

    CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_lookup(lchip, stats_ptr, &fwd_stats));
    if(NULL != fwd_stats)
    {
        /*add to db*/
        if(stats_master->clear_read_en[CTC_STATS_TYPE_FWD])
        {
            fwd_stats->packet_count += p_stats->packet_count;
            fwd_stats->byte_count += p_stats->byte_count;

            p_stats->packet_count = fwd_stats->packet_count;
            p_stats->byte_count = fwd_stats->byte_count;
        }
        else
        {
            fwd_stats->packet_count = p_stats->packet_count;
            fwd_stats->byte_count = p_stats->byte_count;
        }

    }
    else
    {
        fwd_stats = (sys_stats_fwd_stats_t *)mem_malloc(MEM_STATS_MODULE, sizeof(sys_stats_fwd_stats_t));
        if (!fwd_stats)
        {
            return CTC_E_NO_MEMORY;
        }

        fwd_stats->stats_ptr = stats_ptr;
        fwd_stats->packet_count = p_stats->packet_count;
        fwd_stats->byte_count = p_stats->byte_count;

        ctc_hash_insert(sys_fwd_stats_hash[lchip], fwd_stats);
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_flow_stats(uint8 lchip, uint16 stats_ptr)
{
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;
    sys_stats_fwd_stats_t *fwd_stats;

    SYS_STATS_INIT_CHECK();

    fwd_stats = NULL;

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));

    cmd = DRV_IOW(IOC_TABLE, DS_FORWARDING_STATS, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, stats_ptr, cmd, &ds_stats));

    CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_lookup(lchip, stats_ptr, &fwd_stats));
    if(NULL != fwd_stats)
    {
        fwd_stats->packet_count = 0;
        fwd_stats->byte_count = 0;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_policing_en(uint8 lchip, bool enable)
{
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    tmp = (FALSE == enable)?0:1;

    if(stats_master->policing_en[lchip] != enable)
    {
        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_VIOLATE);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_CONFIRM);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        cmd = DRV_IOW(IOC_REG, POLICING_CTRL0, POLICING_CTRL0_STATS_EN_NOT_CONFIRM);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        stats_master->policing_en[lchip] = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_policing_en(uint8 lchip, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->policing_en[lchip])?TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_policing_stats(uint8 lchip, uint16 stats_ptr, sys_stats_policing_t* p_stats)
{
    uint8 ext_qdr_en = 0;
    uint32 index = 0;
    ctc_stats_basic_t basic_stats;
    uint32 stats_mode = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    kal_memset(&basic_stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(p_stats, 0, sizeof(sys_stats_policing_t));

    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));
    if(ext_qdr_en)
    {
        /*green*/
        index = stats_ptr + SYS_STATS_POLICER_CONFIRM_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_confirm_pkts = basic_stats.packet_count;
        p_stats->policing_confirm_bytes = basic_stats.byte_count;

        /*yellow*/
        index = stats_ptr + SYS_STATS_POLICER_NOT_CONFIRM_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_exceed_pkts = basic_stats.packet_count;
        p_stats->policing_exceed_bytes = basic_stats.byte_count;

        /*red*/
        index = stats_ptr + SYS_STATS_POLICER_VIOLATE_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_violate_pkts = basic_stats.packet_count;
        p_stats->policing_violate_bytes = basic_stats.byte_count;
    }
    else
    {
        /*green*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_CONFIRM_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_CONFIRM_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_CONFIRM_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_confirm_pkts = basic_stats.packet_count;
        p_stats->policing_confirm_bytes = basic_stats.byte_count;

        /*yellow*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_NOT_CONFIRM_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_NOT_CONFIRM_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_NOT_CONFIRM_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_exceed_pkts = basic_stats.packet_count;
        p_stats->policing_exceed_bytes = basic_stats.byte_count;

        /*red*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_VIOLATE_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_VIOLATE_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_VIOLATE_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->policing_violate_pkts = basic_stats.packet_count;
        p_stats->policing_violate_bytes = basic_stats.byte_count;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_policing_stats(uint8 lchip, uint16 stats_ptr)
{
    uint8 ext_qdr_en = 0;
    uint32 index = 0;
    uint32 stats_mode = 0;

    SYS_STATS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));
    if(ext_qdr_en)
    {
        /*green*/
        index = stats_ptr + SYS_STATS_POLICER_CONFIRM_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));

        /*yellow*/
        index = stats_ptr + SYS_STATS_POLICER_NOT_CONFIRM_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));

        /*red*/
        index = stats_ptr + SYS_STATS_POLICER_VIOLATE_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));
    }
    else
    {
        /*green*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_CONFIRM_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_CONFIRM_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_CONFIRM_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));

        /*yellow*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_NOT_CONFIRM_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_NOT_CONFIRM_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_NOT_CONFIRM_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));

        /*red*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_NO_CONFLICT_VIOLATE_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_POLICER_OPENFLOW_VIOLATE_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_POLICER_VIOLATE_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_queue_en(uint8 lchip, bool enable)
{
    uint8 ext_qdr_en = 0;
    uint32 cmd = 0;
    uint32 tmp = enable;

    SYS_STATS_INIT_CHECK();

    tmp = (FALSE == enable)?0:1;

    if(stats_master->queue_en[lchip] != enable)
    {
        cmd = DRV_IOW(IOC_REG, Q_MGR_QUE_DEQ_STATS_BASE, Q_MGR_QUE_DEQ_STATS_BASE_STATS_EN);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));

        CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
        if(ext_qdr_en)
        {
            cmd = DRV_IOW(IOC_REG, Q_MGR_QUE_DROP_STATS_BASE, Q_MGR_QUE_DROP_STATS_BASE_STATS_EN);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }
        stats_master->queue_en[lchip] = tmp;
    }
    return CTC_E_NONE;
}

int32
sys_humber_stats_get_queue_en(uint8 lchip, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->queue_en[lchip])?TRUE:FALSE;

    return CTC_E_NONE;

}

int32
sys_humber_stats_get_queue_stats(uint8 lchip, uint16 stats_ptr, sys_stats_queue_t* p_stats)
{
    uint8 ext_qdr_en = 0;
    uint32 index = 0;
    ctc_stats_basic_t basic_stats;
    uint32 stats_mode = 0;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    kal_memset(&basic_stats, 0, sizeof(ctc_stats_basic_t));
    kal_memset(p_stats, 0, sizeof(sys_stats_queue_t));

    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));

    if(ext_qdr_en)
    {
        /*de-queue*/
        index = stats_ptr + SYS_STATS_QUEUE_DEQ_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->queue_deq_pkts = basic_stats.packet_count;
        p_stats->queue_deq_bytes = basic_stats.byte_count;

        /*drop queue*/
        index = stats_ptr + SYS_STATS_QUEUE_DROP_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->queue_drop_pkts = basic_stats.packet_count;
        p_stats->queue_drop_bytes = basic_stats.byte_count;
    }
    else
    {
        /*de-queue*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_NO_CONFLICT_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_OPENFLOW_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_BASE;
        }

        CTC_ERROR_RETURN(sys_humber_stats_get_flow_stats(lchip, index, &basic_stats));
        p_stats->queue_deq_pkts = basic_stats.packet_count;
        p_stats->queue_deq_bytes = basic_stats.byte_count;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_reset_queue_stats(uint8 lchip, uint16 stats_ptr)
{
    uint8 ext_qdr_en = 0;
    uint32 index = 0;
    uint32 stats_mode = 0;

    SYS_STATS_INIT_CHECK();

    CTC_ERROR_RETURN(sys_alloc_get_ext_qdr_en(&ext_qdr_en));
    CTC_ERROR_RETURN(sys_humber_global_ctl_get(CTC_GLOBAL_STATS_MODE, &stats_mode));

    if(ext_qdr_en)
    {
        /*de-queue*/
        index = stats_ptr + SYS_STATS_QUEUE_DEQ_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));

        /*drop queue*/
        index = stats_ptr + SYS_STATS_QUEUE_DROP_BASE_WITH_EXT_QDR;
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));
    }
    else
    {
        /*de-queue*/
        if(stats_mode == CTC_GLOBAL_STATS_NO_CONFLICT_MODE)
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_NO_CONFLICT_BASE;
        }
        else if(stats_mode == CTC_GLOBAL_STATS_OPENFLOW_MODE)
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_OPENFLOW_BASE;
        }
        else
        {
            index = stats_ptr + SYS_STATS_QUEUE_DEQ_BASE;
        }
        CTC_ERROR_RETURN(sys_humber_stats_reset_flow_stats(lchip, index));
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_saturate_en(ctc_stats_type_t stats_type, bool enable)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    chip_num = sys_humber_get_local_chip_num();
    tmp = (FALSE == enable)?0:1;

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("chip num:%d, enable:%d\n",chip_num,enable);

    if(stats_master->saturate_en[stats_type] != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            switch(stats_type)
            {
                case CTC_STATS_TYPE_IGS_PORT_PHB:
                    /*ingress port,phb*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_PHB_INTF, IPE_STATS_CTL_IPE_PHB_INTF_SATURATE_EN_IPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_IGS_GLOBAL_FWD:
                    /*ingress global fwd*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_OVERALL_FWD, IPE_STATS_CTL_IPE_OVERALL_FWD_SATURATE_EN_IPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_PORT_PHB:
                    /*egress port,phb*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_PHB_INTF, EPE_STATS_CTL_EPE_PHB_INTF_SATURATE_EN_EPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_GLOBAL_FWD:
                    /*egress global fwd*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_OVERALL_FWD, EPE_STATS_CTL_EPE_OVERALL_FWD_SATURATE_EN_EPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_FWD:
                    /*fwd*/
                    cmd = DRV_IOW(IOC_REG, STATISTICS_CTL, STATISTICS_CTL_SATURATE_EN);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_GMAC:
                    /*MAC*/
                    if(drv_humber_qmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                       QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(4))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(5))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(6))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(7))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(8))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(9))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(10))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(11))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_XGMAC:
                    /*XGMAC*/
                    if(drv_humber_xgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC0_XGMAC_STATS_CONFIG, XGMAC0_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC1_XGMAC_STATS_CONFIG, XGMAC1_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC2_XGMAC_STATS_CONFIG, XGMAC2_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC3_XGMAC_STATS_CONFIG, XGMAC3_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_SGMAC:
                    /*SGMAC*/
                    if(drv_humber_sgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC0_SGMAC_STATS_CONFIG, SGMAC0_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC1_SGMAC_STATS_CONFIG, SGMAC1_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC2_SGMAC_STATS_CONFIG, SGMAC2_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC3_SGMAC_STATS_CONFIG, SGMAC3_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_CPUMAC:
                    /*CPU MAC*/
                    cmd = DRV_IOW(IOC_REG, CPU_MAC_STATS_UPDATE_CTRL, CPU_MAC_STATS_UPDATE_CTRL_INCR_SATURATE);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        stats_master->saturate_en[stats_type] = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_saturate_en(ctc_stats_type_t stats_type, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->saturate_en[stats_type])?TRUE:FALSE;

    return CTC_E_NONE;
}


int32
sys_humber_stats_set_hold_en(ctc_stats_type_t stats_type, bool enable)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    chip_num = sys_humber_get_local_chip_num();
    tmp = (FALSE == enable)?0:1;

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("chip num:%d, enable:%d\n",chip_num,enable);

    if(stats_master->hold_en[stats_type] != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            switch(stats_type)
            {
                case CTC_STATS_TYPE_IGS_PORT_PHB:
                    /*ingress port,phb*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_PHB_INTF, IPE_STATS_CTL_IPE_PHB_INTF_STATS_HOLD_IPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_IGS_GLOBAL_FWD:
                    /*ingress global fwd*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_OVERALL_FWD, IPE_STATS_CTL_IPE_OVERALL_FWD_STATS_HOLD_IPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_PORT_PHB:
                    /*egress port,phb*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_PHB_INTF, EPE_STATS_CTL_EPE_PHB_INTF_STATS_HOLD_EPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_GLOBAL_FWD:
                    /*egress global fwd*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_OVERALL_FWD, EPE_STATS_CTL_EPE_OVERALL_FWD_STATS_HOLD_EPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_FWD:
                    /*fwd*/
                    cmd = DRV_IOW(IOC_REG, STATISTICS_CTL, STATISTICS_CTL_STATS_HOLD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_GMAC:
                    /*MAC*/
                    if(drv_humber_qmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                       QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(4))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(5))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(6))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(7))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(8))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(9))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(10))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(11))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_XGMAC:
                    /*XGMAC*/
                    if(drv_humber_xgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC0_XGMAC_STATS_CONFIG, XGMAC0_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC1_XGMAC_STATS_CONFIG, XGMAC1_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC2_XGMAC_STATS_CONFIG, XGMAC2_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC3_XGMAC_STATS_CONFIG, XGMAC3_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_SGMAC:
                    /*SGMAC*/
                    if(drv_humber_sgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC0_SGMAC_STATS_CONFIG, SGMAC0_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC1_SGMAC_STATS_CONFIG, SGMAC1_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC2_SGMAC_STATS_CONFIG, SGMAC2_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC3_SGMAC_STATS_CONFIG, SGMAC3_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_CPUMAC:
                    /*CPU MAC*/
                    cmd = DRV_IOW(IOC_REG, CPU_MAC_STATS_UPDATE_CTRL, CPU_MAC_STATS_UPDATE_CTRL_INCR_HOLD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        stats_master->hold_en[stats_type] = tmp;
    }
    return CTC_E_NONE;
}

int32
sys_humber_stats_get_hold_en(ctc_stats_type_t stats_type, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->hold_en[stats_type])?TRUE:FALSE;

    return CTC_E_NONE;
}


int32
sys_humber_stats_set_clear_after_read_en(ctc_stats_type_t stats_type, bool enable)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    chip_num = sys_humber_get_local_chip_num();
    tmp = (FALSE == enable)?0:1;

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("chip num:%d, enable:%d\n",chip_num,enable);

    if(stats_master->clear_read_en[stats_type] != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            switch(stats_type)
            {
                case CTC_STATS_TYPE_IGS_PORT_PHB:
                    /*ingress port,phb*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_PHB_INTF, IPE_STATS_CTL_IPE_PHB_INTF_CLEAR_ON_READ_IPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_IGS_GLOBAL_FWD:
                    /*ingress global fwd*/
                    cmd = DRV_IOW(IOC_REG, IPE_STATS_CTL_IPE_OVERALL_FWD, IPE_STATS_CTL_IPE_OVERALL_FWD_CLEAR_ON_READ_IPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_PORT_PHB:
                    /*egress port,phb*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_PHB_INTF, EPE_STATS_CTL_EPE_PHB_INTF_CLEAR_ON_READ_EPE_PHB_INTF);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_EGS_GLOBAL_FWD:
                    /*egress global fwd*/
                    cmd = DRV_IOW(IOC_REG, EPE_STATS_CTL_EPE_OVERALL_FWD, EPE_STATS_CTL_EPE_OVERALL_FWD_CLEAR_ON_READ_EPE_OVERALL_FWD);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_FWD:
                    /*fwd*/
                    cmd = DRV_IOW(IOC_REG, STATISTICS_CTL, STATISTICS_CTL_CLEAR_ON_READ);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                case CTC_STATS_TYPE_GMAC:
                    /*MAC*/
                    if(drv_humber_qmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                       QUADMACAPP0_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP1_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP2_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP3_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(4))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP4_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(5))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP5_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(6))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP6_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(7))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP7_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(8))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP8_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(9))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP9_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(10))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP10_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_qmac_is_enable(11))
                    {
                        cmd = DRV_IOW(IOC_REG, QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL, \
                                   QUADMACAPP11_QUAD_MAC_APP_STATS_UPDATE_CTRL_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_XGMAC:
                    /*XGMAC*/
                    if(drv_humber_xgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC0_XGMAC_STATS_CONFIG, XGMAC0_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC1_XGMAC_STATS_CONFIG, XGMAC1_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC2_XGMAC_STATS_CONFIG, XGMAC2_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_xgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, XGMAC3_XGMAC_STATS_CONFIG, XGMAC3_XGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_SGMAC:
                    /*SGMAC*/
                    if(drv_humber_sgmac_is_enable(0))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC0_SGMAC_STATS_CONFIG, SGMAC0_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(1))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC1_SGMAC_STATS_CONFIG, SGMAC1_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(2))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC2_SGMAC_STATS_CONFIG, SGMAC2_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }

                    if(drv_humber_sgmac_is_enable(3))
                    {
                        cmd = DRV_IOW(IOC_REG, SGMAC3_SGMAC_STATS_CONFIG, SGMAC3_SGMAC_STATS_CONFIG_INCR_SATURATE);
                        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    }
                    break;

                case CTC_STATS_TYPE_CPUMAC:
                    /*CPU MAC*/
                    cmd = DRV_IOW(IOC_REG, CPU_MAC_STATS_UPDATE_CTRL, CPU_MAC_STATS_UPDATE_CTRL_CLEAR_ON_READ);
                    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
                    break;

                default:
                    return CTC_E_INVALID_PARAM;
            }
        }

        stats_master->clear_read_en[stats_type] = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_clear_after_read_en(ctc_stats_type_t stats_type, bool* p_enable)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_enable);

    *p_enable = (stats_master->clear_read_en[stats_type])?TRUE:FALSE;

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_pkt_cnt_threshold(uint16 threshold)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("packet count threshold:%d\n", threshold);

    chip_num = sys_humber_get_local_chip_num();
    tmp = threshold;

    if(stats_master->pkt_cnt_threshold != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, STATISTICS_PACKET_COUNT_THRESHOLD, STATISTICS_PACKET_COUNT_THRESHOLD_PACKET_COUNT_THRESHOLD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        stats_master->pkt_cnt_threshold = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_pkt_cnt_threshold(uint16* p_threshold)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_threshold);

    *p_threshold = stats_master->pkt_cnt_threshold;

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_byte_cnt_threshold(uint16 threshold)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("byte count threshold:%d\n", threshold);

    chip_num = sys_humber_get_local_chip_num();
    tmp = threshold;

    if(stats_master->byte_cnt_threshold != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, STATISTICS_BYTE_COUNT_THRESHOLD, STATISTICS_BYTE_COUNT_THRESHOLD_BYTE_COUNT_THRESHOLD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        stats_master->byte_cnt_threshold = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_byte_cnt_threshold(uint16* p_threshold)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_threshold);

    *p_threshold = stats_master->byte_cnt_threshold;

    return CTC_E_NONE;
}

int32
sys_humber_stats_set_fifo_depth_threshold(uint8 threshold)
{
    uint8 lchip = 0;
    uint8 chip_num = 0;
    uint32 cmd = 0;
    uint32 tmp = 0;

    SYS_STATS_INIT_CHECK();

    SYS_STATS_DBG_FUNC();
    SYS_STATS_DBG_INFO("fifo depth threshold:%d\n", threshold);

    chip_num = sys_humber_get_local_chip_num();
    tmp = threshold;

    if(stats_master->fifo_depth_threshold != tmp)
    {
        for(lchip=0; lchip<chip_num && lchip<MAX_LOCAL_CHIP_NUM; lchip++)
        {
            cmd = DRV_IOW(IOC_REG, STATISTICS_FIFO_DEPTH_THRESHOLD, STATISTICS_FIFO_DEPTH_THRESHOLD_FIFO_DEPTH_THRESHOLD);
            CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &tmp));
        }

        stats_master->fifo_depth_threshold = tmp;
    }

    return CTC_E_NONE;
}

int32
sys_humber_stats_get_fifo_depth_threshold(uint8* p_threshold)
{
    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_threshold);

    *p_threshold = stats_master->fifo_depth_threshold;

    return CTC_E_NONE;
}

static int32
_sys_humber_stats_get_fwd_stats(uint8 lchip, uint16 stats_ptr, ctc_stats_basic_t* p_stats)
{
    uint32 cmd = 0;
    ds_forwarding_stats_t ds_stats;

    SYS_STATS_INIT_CHECK();
    CTC_PTR_VALID_CHECK(p_stats);

    kal_memset(&ds_stats, 0, sizeof(ds_forwarding_stats_t));
    kal_memset(p_stats, 0, sizeof(ctc_stats_basic_t));

    cmd = DRV_IOR(IOC_TABLE, DS_FORWARDING_STATS, DRV_ENTRY_FLAG);
    CTC_ERROR_RETURN(drv_tbl_ioctl(lchip, stats_ptr, cmd, &ds_stats));
    _sys_humber_stats_ds_stats_to_basic(ds_stats, p_stats);

    return CTC_E_NONE;
}

int32
sys_humber_stats_intr_callback_func(uint8* gchip)
{
    uint8 lchip = 0, i = 0;
    uint32 cmd = 0, depth = 0, stats_ptr = 0, stats_base = 0;
    ctc_stats_basic_t stats;
    sys_stats_fwd_stats_t *fwd_stats;

    fwd_stats = NULL;
    kal_memset(&stats, 0, sizeof(ctc_stats_basic_t));

    if(TRUE != sys_humber_chip_is_local(*gchip, &lchip))
    {
        return CTC_E_INVALID_PARAM;
    }

    /*get ext qdr stats base*/
    cmd = DRV_IOR(IOC_REG, STATISTICS_CTL, STATISTICS_CTL_STATS_BASE_PTR);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &stats_base));

    /*get stats ptr fifo depth*/
    cmd = DRV_IOR(IOC_REG, STATISTICS_THRESHOLD_FIFO_DEPTH, STATISTICS_THRESHOLD_FIFO_DEPTH_THRESHOLD_FIFO_DEPTH);
    CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &depth));

    if(depth > SYS_STATS_MAX_FIFO_DEPTH)
    {
        return CTC_E_EXCEED_MAX_SIZE;
    }

    /*get stats ptr from fifo*/
    for(i = 0; i<depth; i++)
    {
        cmd = DRV_IOR(IOC_REG, STATISTICS_SATU_ADDR, STATISTICS_SATU_ADDR_SATU_ADDR_RD_DATA);
        CTC_ERROR_RETURN(drv_reg_ioctl(lchip, 0, cmd, &stats_ptr));

        if(stats_ptr > 4095)
        {
            stats_ptr = stats_ptr - stats_base + 4096;
        }

        /*get stats from stats ptr*/
        CTC_ERROR_RETURN(_sys_humber_stats_get_fwd_stats(lchip, stats_ptr, &stats));

        CTC_ERROR_RETURN(_sys_stats_fwd_stats_entry_lookup(lchip, stats_ptr, &fwd_stats));
        if(NULL == fwd_stats)
        {
            fwd_stats = (sys_stats_fwd_stats_t *)mem_malloc(MEM_STATS_MODULE, sizeof(sys_stats_fwd_stats_t));
            if (!fwd_stats)
            {
                return CTC_E_NO_MEMORY;
            }

            fwd_stats->stats_ptr = stats_ptr;
            fwd_stats->packet_count = stats.packet_count;
            fwd_stats->byte_count = stats.byte_count;

            ctc_hash_insert(sys_fwd_stats_hash[lchip], fwd_stats);

        }
        else
        {
            fwd_stats->packet_count += stats.packet_count;
            fwd_stats->byte_count += stats.byte_count;
        }
    }

    return CTC_E_NONE;
}

