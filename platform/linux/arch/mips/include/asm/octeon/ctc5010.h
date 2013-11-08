#ifndef __CTC5010_H
#define __CTC5010_H

/*support 256M flash, bug21815 */
#define GLB_EPLD_BASE 0x17000000
#define GLB_FLASH_BASE 0x1a000000
#define GLB_BOARD_SERIES_BASE (GLB_EPLD_BASE+0x10000)
#define GLB_BOARD_VER_BASE (GLB_EPLD_BASE+0x20000)

#define GLB_EPLD_NEW_BASE 0x10000000
#define GLB_BOARD_SERIES_NEW_BASE (GLB_EPLD_NEW_BASE+0x10000)
#define GLB_BOARD_VER_NEW_BASE (GLB_EPLD_NEW_BASE+0x20000)
#define GLB_FLASH_NEW_BASE (GLB_EPLD_NEW_BASE+0x00100000)

/*must the same order with glb_hw_define.h*/
enum glb_board_series_e
{
    GLB_BOARD_SERIES_E300 = 0,
    GLB_BOARD_SERIES_E600,
    GLB_BOARD_SERIES_E800,
    GLB_BOARD_SERIES_EX2000,
    GLB_BOARD_SERIES_EX3000,
    GLB_BOARD_SERIES_E310,
    GLB_BOARD_SERIES_E810,
    GLB_BOARD_SERIES_CPCI,
    GLB_BOARD_SERIES_SEOUL = 0x8,
    GLB_BOARD_SERIES_E330,
    GLB_BOARD_SERIES_B330 = 0xb,
    GLB_BOARD_SERIES_HUMBER_DEMO = 0xf,
    GLB_BOARD_SERIES_MAX
};
typedef enum glb_board_series_e glb_board_series_t;

enum seoul_board_type_e
{
    BOARD_SEOUL_G24EU = 1,   
    BOARD_SEOUL_MAX
} ;
typedef enum seoul_board_type_e seoul_board_type_T;

enum e330_board_type_e
{
    BOARD_E330_48T = 1,
    BOARD_E330_24T = 2,
    BOARD_E330_48S_E = 3,
    BOARD_E330_48S = 4,
    BOARD_E330_48T_L = 5,
    BOARD_E330_MAX
} ;
typedef enum e330_board_type_e e330_board_type_T;

enum b330_board_type_e
{
    BOARD_B330_48T = 1,
    BOARD_B330_MAX
} ;
typedef enum b330_board_type_e b330_board_type_T;

enum humber_demo_board_type_e
{
    BOARD_DEMO_HUMBER = 1,   
    BOARD_DEMO_HUMBER_MAX=0xfc,
} ;
typedef enum humber_demo_board_type_e humber_demo_board_type_t;


enum board_version_e
{
    BOARD_VERSION_0 = 0,
    BOARD_VERSION_1,
    BOARD_VERSION_2,
    BOARD_VERSION_3,
    BOARD_VERSION_4,
    BOARD_VERSION_MAX
};
typedef enum board_version_e board_version_t;

#define SEOUL_G24EU_RST_OFFSET 0xB

#define EPLD_GPIO_MODE_SWITCH 0x7
#define EPLD_GPIO_ENABLE_REG 0x8

#define ASIC_RESET_OFFSET    0x9

struct ctc_board_s
{
    u8 board_series ;           /*board series*/
    u8 board_type ;             /*board type*/
    u8 board_ver ;                 /*board version*/
    u8 flash_type;              /*flash type*/
    u8 ddr_type;                /*ddr type*/
    u8 asic_chip_num;           /*asic chip number*/
    u8 asic0_normal_irq;         /*chip 0 normal irq number*/
    u8 asic0_fatal_irq;          /*chip 0 fatal irq number*/
    u8 asic1_normal_irq;         /*chip 1 normal irq number*/
    u8 asic1_fatal_irq;          /*chip 1 fatal irq number*/
    u8 normal_irq_count;         /*normal irq count,now just support 2 normal irq*/
    u8 normal_irq_0;         /*normal irq 0 number*/
    u8 normal_irq_1;         /*normal irq 1 number*/
    u8 normal_int0_offset;    /*normal interrupt 0 offset*/
    u8 normal_int0_mask_offset;    /*normal interrupt 0 mask offset*/
    u8 normal_int1_offset;    /*normal interrupt 1 offset, NOTE: including POE int*/
    u8 normal_int1_mask_offset;    /*normal interrupt 1 mask offset, NOTE: including POE mask*/
    u8 poe_irq;                    /* poe irq number */
    u8 poe_irq_offset;             /* poe irq offset */
    u8 poe_irq_mask_offset;        /* poe irq mask offset */
    u8 poe_support;                /* poe support: if 1, support, othterwise 0 */
    u8 foam_support;    /*whether support foam*/
    u8 foam_normal_irq;         /*foam fpga normal irq number*/
    u8 foam_fatal_irq;         /*foam fpga fatal irq number*/
    u8 ctrlfpga_support;    /*whether support ctrl fpga*/
    u8 watchdog_support;    /*whether support watch dog*/    
    u8 watchdog_en_gpio;    /*enable watch dog gpio*/
    u8 watchdog_feed_gpio;    /*feed watch dog gpio*/
    u8 watchdog_is_feed;      /*whether to feed watchdog, bug18151*/
    u8 bootbus_updated;       /*whether the bootbus is updated*/
    void __iomem *epld_logic_addr;/*epld logic address*/
};
typedef struct ctc_board_s ctc_board_t;

#define ASIC_FATAL_IRQ              44  /* GPIO 4 */
#define ASIC_NORMAL_IRQ             45  /* GPIO 5 */
#define SEOUL_FOAM_NORMAL_IRQ       46  /* GPIO 6 */
#define SEOUL_CTC_HW_NORMAL_IRQ_0       47  /* GPIO 7 */

#define CTC_HW_NORMAL_IRQ_0                46  /* GPIO 6 */
#define CTC_HW_NORMAL_IRQ_1                47  /* GPIO 7 */
#define CTC_HW_POE_IRQ                     48  /* GPIO 8 */

#define HUMBER_DEMO_CTC_HW_NORMAL_IRQ_0       44  /* GPIO 4 */
#define HUMBER_DEMO_ASIC_FATAL_IRQ          45  /* GPIO 5 */
#define HUMBER_DEMO_ASIC_NORMAL_IRQ       47  /* GPIO 7 */


#define WATCHDOG_EN_GPIO        14  /* the GPIO pin which used to enable GPIO feed dog */
#define WATCHDOG_FEED_GPIO                15  /* the GPIO pin which used to feed dog */

#define CTC_NORMAL_INT0_OFFSET        0x10  /* normal interrupt 0 offset */
#define CTC_NORMAL_INT1_OFFSET        0x11  /* normal interrupt 1 offset NOTE: include POE int */
#define CTC_NORMAL_INT0_MASK_OFFSET        0x12  /* normal interrupt 0 mask offset */
#define CTC_NORMAL_INT1_MASK_OFFSET        0x13  /* normal interrupt 1 mask offset NOTE: include POE int mask */

#define CTC_POE_INT_OFFSET            CTC_NORMAL_INT1_OFFSET
#define CTC_POE_INT_MASK_OFFSET       CTC_NORMAL_INT1_MASK_OFFSET

#define CTC_DAUGHTER_CARD_TYPE             0x23  /*extend card present status and type*/
#define CTC_DAUGHTER_CARD_VER              0x1f  /*extend card version*/
#define CTC_DAUGHTER_CARD_SWT              0x1a  /*extend card switch flag cfg*/

#define CTC_EPLD_LED_OFFSET        0x5  /* epld led ctrl offset */
#define CTC_EPLD_POE_PRESENT_OFFSET 0x24  /* epld poe present offset */

#define CTC_CTLFPGA_NORMAL_INT_OFFSET  0x14
#define CTC_CTLFPGA_NORMAL_INT_MASK_OFFSET  0x34
int ctc_boards_fixup(void);
int ctc_board_is_e330_48T(void);

#endif /* __CTC5010_H */
