#ifndef _DRV_HUMBER_DATA_PATH_H
#define _DRV_HUMBER_DATA_PATH_H

#include "kal.h"
#include "dal.h"

#define DRV_MAX_CHIP_NUM 2

#define DRV_MAX_GMAC_NUM 48
#define DRV_MAX_XGMAC_NUM 4
#define DRV_MAX_SGMAC_NUM 4
#define DRV_MAX_QMAC_NUM 12
#define DRV_CPUMAC_CLK_SERDES DRV_SERDES_ITEM_NTSG40

enum drv_chip_item_type_e
{
    DRV_CHIP_ITEM_TYPE_CHIP_ID = 1,
};
typedef enum drv_chip_item_type_e drv_chip_item_type_t;

enum drv_reset_type_e
{
    CHIP_RESET_TCAM_CORE,
    CHIP_RESET_TCAM_POWER,
    CHIP_RESET_GLB,
    CHIP_RESET_DLLOCK,
    CHIP_RESET_PCI,
    CHIP_RESET_SUP,
    CHIP_RESET_RESET_TYPE_MAX
};
typedef enum drv_reset_type_e drv_reset_type_t;

enum drv_chip_type_e
{
    CHIP_HUMBER,
    CHIP_MAX
};
typedef enum drv_chip_type_e drv_chip_type_t;

enum drv_tcam_type_e
{
    TCAM_UNUSE_EXT,
    TCAM_USE_EXT,
    TCAM_TYPE_END
};
typedef enum drv_tcam_type_e drv_tcam_type_t;

enum drv_sram_type_e
{
    SRAM_UNUSE_EXT,
    SRAM_USE_EXT,
    SRAM_TYPE_END
};
typedef enum drv_sram_type_e drv_sram_type_t;

enum drv_io_type_e
{
    CHIP_IO_SUP,
    CHIP_IO_PCI,
    CHIP_IO_TYPE_END
};
typedef enum drv_io_type_e drv_io_type_t;

enum drv_cpumac_speed_e
{
    CPUMAC_SPEED_100M,
    CPUMAC_SPEED_1G,
    CPUMAC_SPEED_MAX
};
typedef enum drv_cpumac_speed_e drv_cpumac_speed_t;

enum drv_cpumac_refclk_e
{
    CPUMAC_REFCLK_INTERNAL,
    CPUMAC_REFCLK_EXTERNAL,
    CPUMAC_REFCLK_MAX
};
typedef enum drv_cpumac_refclk_e drv_cpumac_refclk_t;

enum drv_serdes_item_type_e
{
    DRV_SERDES_ITEM_NTSG0 = 0,
    DRV_SERDES_ITEM_NTSG1,
    DRV_SERDES_ITEM_NTSG2,
    DRV_SERDES_ITEM_NTSG3,
    DRV_SERDES_ITEM_NTSG4,
    DRV_SERDES_ITEM_NTSG5,
    DRV_SERDES_ITEM_NTSG6,
    DRV_SERDES_ITEM_NTSG7,
    DRV_SERDES_ITEM_NTSG8,
    DRV_SERDES_ITEM_NTSG9,
    DRV_SERDES_ITEM_NTSG10,
    DRV_SERDES_ITEM_NTSG11,
    DRV_SERDES_ITEM_NTSG12,
    DRV_SERDES_ITEM_NTSG13,
    DRV_SERDES_ITEM_NTSG14,
    DRV_SERDES_ITEM_NTSG15,
    DRV_SERDES_ITEM_NTSG16,
    DRV_SERDES_ITEM_NTSG17,
    DRV_SERDES_ITEM_NTSG18,
    DRV_SERDES_ITEM_NTSG19,
    DRV_SERDES_ITEM_NTSG20,
    DRV_SERDES_ITEM_NTSG21,
    DRV_SERDES_ITEM_NTSG22,
    DRV_SERDES_ITEM_NTSG23,
    DRV_SERDES_ITEM_NTSG24,
    DRV_SERDES_ITEM_NTSG25,
    DRV_SERDES_ITEM_NTSG26,
    DRV_SERDES_ITEM_NTSG27,
    DRV_SERDES_ITEM_NTSG28,
    DRV_SERDES_ITEM_NTSG29,
    DRV_SERDES_ITEM_NTSG30,
    DRV_SERDES_ITEM_NTSG31,
    DRV_SERDES_ITEM_NTSG32,
    DRV_SERDES_ITEM_NTSG33,
    DRV_SERDES_ITEM_NTSG34,
    DRV_SERDES_ITEM_NTSG35,
    DRV_SERDES_ITEM_NTSG36,
    DRV_SERDES_ITEM_NTSG37,
    DRV_SERDES_ITEM_NTSG38,
    DRV_SERDES_ITEM_NTSG39,
    DRV_SERDES_ITEM_NTSG40,
    DRV_SERDES_ITEM_NTSG41,
    DRV_SERDES_ITEM_NTSG42,
    DRV_SERDES_ITEM_NTSG43,
    DRV_SERDES_ITEM_NTSG44,
    DRV_SERDES_ITEM_NTSG45,
    DRV_SERDES_ITEM_NTSG46,
    DRV_SERDES_ITEM_NTSG47,

    DRV_SERDES_ITEM_UPSG0,
    DRV_SERDES_ITEM_UPSG1,
    DRV_SERDES_ITEM_UPSG2,
    DRV_SERDES_ITEM_UPSG3,
    DRV_SERDES_ITEM_UPSG4,
    DRV_SERDES_ITEM_UPSG5,
    DRV_SERDES_ITEM_UPSG6,
    DRV_SERDES_ITEM_UPSG7,
    DRV_SERDES_ITEM_UPSG8,
    DRV_SERDES_ITEM_UPSG9,
    DRV_SERDES_ITEM_UPSG10,
    DRV_SERDES_ITEM_UPSG11,
    DRV_SERDES_ITEM_UPSG12,
    DRV_SERDES_ITEM_UPSG13,
    DRV_SERDES_ITEM_UPSG14,
    DRV_SERDES_ITEM_UPSG15
};
typedef enum drv_serdes_item_type_e drv_serdes_item_type_t;

enum drv_serdes_type_e
{
    DRV_SERDES_TYPE_GE = 0,
    DRV_SERDES_TYPE_SG,
    DRV_SERDES_TYPE_XG,
    DRV_SERDES_TYPE_FB,
    DRV_SERDES_TYPE_NULL
};
typedef enum drv_serdes_type_e drv_serdes_type_t;

enum drv_serdes_speed_e
{
    DRV_SERDES_SPPED_1DOT25G = 0,
    DRV_SERDES_SPPED_3DOT125G,
    DRV_SERDES_SPPED_3DOT75G,
    DRV_SERDES_SPPED_4DOT0625G,
    DRV_SERDES_SPPED_5G,
    DRV_SERDES_SPPED_6DOT25G,
    DRV_SERDES_SPPED_1DOT56G
};
typedef enum drv_serdes_speed_e drv_serdes_speed_t;

enum drv_serdes_drive_strength_e
{
    DRV_SERDES_DRV_STRENGTH_LOW = 0,
    DRV_SERDES_DRV_STRENGTH_MID,
    DRV_SERDES_DRV_STRENGTH_HIGH,
    DRV_SERDES_DRV_STRENGTH_MAX
};
typedef enum drv_serdes_drive_strength_e drv_serdes_drive_strength_t;


enum drv_pll_item_type_e
{
    DRV_PLL_ITEM_CORE = 0,
    DRV_PLL_ITEM_HSS4G,
    DRV_PLL_ITEM_HSS6G,
    DRV_PLL_ITEM_TABLE,
    DRV_PLL_ITEM_TCAM,
    DRV_PLL_ITEM_MAX,
};
typedef enum drv_pll_item_type_e drv_pll_item_type_t;

enum drv_ext_mem_item_type_e
{
    DRV_EXT_MEM_ITEM_DDR = 0,
    DRV_EXT_MEM_ITEM_QDR,
    DRV_EXT_MEM_ITEM_TCAM,
    DRV_EXT_MEM_ITEM_MAX,
};
typedef enum drv_ext_mem_item_type_e drv_ext_mem_item_type_t;

enum drv_mdio_item_type_e
{
    DRV_MDIO_ITEM_MDIO0 = 0,
    DRV_MDIO_ITEM_MDIO1,
    DRV_MDIO_ITEM_MDIO2
};
typedef enum drv_mdio_item_type_e drv_mdio_item_type_t;

enum drv_led_item_type_e
{
    DRV_LED_ITEM_LED0 = 0,
    DRV_LED_ITEM_LED1,
    DRV_LED_ITEM_LED2,
    DRV_LED_ITEM_LED3
};
typedef enum drv_led_item_type_e drv_led_item_type_t;

enum drv_misc_item_type_e
{
    DRV_MISC_ITEM_CPU_MAC = 0,
    DRV_MISC_ITEM_PTP_ENGINE,
    DRV_MISC_ITEM_CPU_MAC_REF_CLK,
};
typedef enum drv_misc_item_type_e drv_misc_item_type_t;

enum drv_fabric_item_type_e
{
    DRV_FABRIC_ITEM_MASTER = 0,
    DRV_FABRIC_ITEM_FABRIC_TIMER1,
    DRV_FABRIC_ITEM_FABRIC_TIMER2,
    DRV_FABRIC_ITEM_FABRIC_TIMER3_EXPIRE,
    DRV_FABRIC_ITEM_FABRIC_TIMER3_START_TO_CHECK
};
typedef enum drv_fabric_item_type_e drv_fabric_item_type_t;

enum drv_calendar_item_type_e
{
    DRV_CALENDAR_PTR_ITEM_MAC_MUX = 0,
    DRV_CALENDAR_PTR_ITEM_NET_TX
};
typedef enum drv_calendar_item_type_e drv_calendar_item_type_t;


struct drv_chip_item_s
{
    uint32 seq;
};
typedef struct drv_chip_item_s drv_chip_item_t;

struct drv_serdes_item_t
{
    uint8 type;
    uint8 speed;
    uint8 drv_strength;
};
typedef struct drv_serdes_item_t drv_serdes_item_t;

struct drv_pll_item_s
{
    bool is_used;
    uint32 input;
    uint32 output_a;/*output_a and output_b show PLL output, some PLL have two output. */
    uint32 output_b;
    uint32 cfg1;
    uint32 cfg2;
    uint32 cfg3;
};
typedef struct drv_pll_item_s drv_pll_item_t;

struct drv_ext_mem_item_s
{
    bool is_used;
    uint32 freq;
};
typedef struct drv_ext_mem_item_s drv_ext_mem_item_t;

struct drv_calendar_item_s
{
    uint32 entry;
    uint32 value;
};
typedef struct drv_calendar_item_s drv_calendar_item_t;

struct drv_normal_item_s
{
    uint32 value;
};
typedef struct drv_normal_item_s drv_normal_item_t;

struct drv_fabric_item_s
{
    uint8 is_master;
    uint32 timer1;
    uint32 timer2;
    uint32 timer3_expire;
    uint32 timer3_check;
    uint32 bandwidth;
};
typedef struct drv_fabric_item_s drv_fabric_item_t;

typedef struct drv_para_pair_s
{
    const char  *para_name;                    /* the parameter name */
    int32 (*fun_ptr)(const char *line, void *argus); /* get the value from line */
    void *argus;                               /* parameter for fun_ptr */
}drv_para_pair_t;

struct datapath_debug_s
{
    uint32 chip_id;
    uint32 addr;
    uint32 oper;
    uint32 value;
};
typedef struct datapath_debug_s datapath_debug_t;


struct drv_chip_info_s
{
    uint32  chip_id;
    drv_chip_type_t chip_type;
    drv_cpumac_speed_t cpumac_speed;
    drv_io_type_t chip_io_type;
    uint8 ptp_quanta;
    uint8 cpu_macda[6];
    uint8 cpu_macsa[6];
};
typedef struct drv_chip_info_s drv_chip_info_t;

struct drv_datapath_info_s
{
    drv_chip_item_t chip_item;
    drv_serdes_item_t serdes_item[64];
    drv_pll_item_t pll_item[5]; /*0-core,1-table,2-tcam,3-hss4g,4-hss6g*/
    drv_ext_mem_item_t ext_mem_item[3]; /*0-ddr,1-qdr,2-tcam*/
    drv_normal_item_t mdio_item[3];
    drv_normal_item_t led_item[4];
    drv_normal_item_t misc_item[3]; /*0-humberCpuMac,1-ptpEngine,2-HumberCpuMacRefClk*/
    drv_normal_item_t fabric_item[5];
    drv_normal_item_t calendar_ptr_item[2];
    drv_calendar_item_t calendar_entry_item[228];
};
typedef struct drv_datapath_info_s drv_datapath_info_t;


struct drv_datapath_master_s
{
    uint32 chip_seq;
    uint32 cpu_mac_speed;
    uint32 rev;/*chip revision*/
    uint8 cpu_mac_ref_clk; /*detail see drv_cpumac_refclk_t*/
    uint8 is_hss4g;
    uint8 fabric_intf_en;
    uint8 ptp_quanta[DRV_MAX_CHIP_NUM];
    uint8 ptp_quanta_ratio[DRV_MAX_CHIP_NUM];
    uint8 ptp_en;
    uint8 ext_ddr_en;
    uint8 ext_qdr_en;
    uint8 ext_tcam_en;
    uint32 pll_out[DRV_PLL_ITEM_MAX]; /* detail see drv_pll_item_type_t */
    uint32 dl_lock[DRV_EXT_MEM_ITEM_MAX]; /* tcam has no dll lock status. */
    uint32 mac_mux_cal_ptr;
    uint32 net_tx_cal_ptr;
    uint8 gmac_en[DRV_MAX_GMAC_NUM];
    uint8 qmac_en[DRV_MAX_QMAC_NUM]; /* store Qmac enable info. */
    uint8 hss_en[6]; /*store hss4G 0~5 which hss enable, and which hss disable*/
    uint8 xgmac_en[DRV_MAX_XGMAC_NUM];
    uint8 sgmac_en[DRV_MAX_SGMAC_NUM];
    uint8 fabric_en[16];
    uint8 cpu_bandwidth;
    uint8 oam_bandwidth;
    uint8 iloop_bandwidth;
    uint8 eloop_bandwidth;
    uint8 sgmac_bandwidth;
    uint8 gmac_bandwidth; /*include xgmac and gmac */
    uint32 ipe_bandwidth; /*include xgmac, gmac, sgmac */
    uint32 mac_mux_cal_entry[52];
    uint32 net_tx_cal_entry[176];
    drv_fabric_item_t fabric_info;
};
typedef struct drv_datapath_master_s drv_datapath_master_t;

typedef int32 (*chip_reset_cb)(uint8 type, uint32 flag);
extern int32
drv_humber_init_total(chip_reset_cb reset_cb, drv_chip_info_t chip_info, char* datapath_config_file);
extern uint8 drv_humber_xgmac_is_enable(uint8 xgmac_id);
extern uint8 drv_humber_sgmac_is_enable(uint8 sgmac_id);
extern uint8 drv_humber_qmac_is_enable(uint8 qmac_id);
extern uint8 drv_humber_gmac_is_enable(uint8 gmac_id);
extern uint8 drv_humber_old_version();
extern uint8 drv_humber_cpumac_use_internal_refclk();
extern uint8 drv_humber_fabric_is_enable();
extern uint32 drv_humber_get_core_freq();
extern uint8 drv_humber_get_ptp_en();
extern uint8 drv_humber_get_ptp_quanta(uint8 lchip);
extern uint8 drv_humber_get_ptp_quanta_ratio(uint8 lchip);
extern uint8 drv_humber_ext_tcam_is_enable();
extern uint8 drv_humber_ext_ddr_is_enable();
extern uint8 drv_humber_ext_qdr_is_enable();
void drv_humber_init_sw_emu();
#if 0
extern int32 drv_humber_send_test_pkt(uint16 chip, uint16 gport, uint16 count, uint16 vlan, uint8 *mac);
#endif
#endif
