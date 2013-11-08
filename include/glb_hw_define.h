/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _GLB_HW_DEFINE_H_
#define _GLB_HW_DEFINE_H_

#include "sal.h"
#include "glb_phy_define.h"
#include "glb_eeprom_define.h"

#define GLB_BOARD_TYPE_STRING         "board_type"
#define GLB_PRODUCT_SERIES_STRING     "product_series"
#define GLB_EPLD_VERSION_STRING       "epld_ver"
#define GLB_EPLD_DATE_STRING          "epld_date"
#define GLB_EPLD_TIME_STRING          "epld_time"
#define GLB_FPGA_VERSION_STRING       "fpga_ver"
#define GLB_HW_VERSION_STRING         "hw_ver"
#define GLB_HW_FLASH_SIZE             "flash_size"
#define GLB_HW_DRAM_SIZE              "sdram_size"
#define GLB_HW_ECC_SUPPORT            "ecc"
#define GLB_HW_CPU_POSITION           "cpu_position"
#define GLB_HW_CPU_CARD_VER           "cpu_card_ver"
#define GLB_HW_CPU_CARD_TYPE          "cpu_card_type"
#define GLB_DCARD_TYPE_STRING         "daughter_card_type"
#define GLB_DCARD_VER_STRING          "daughter_card_ver"

#define MAX_SERIAL_NO_LEN           12
#define MAX_BOOTROM_VER_LEN         32
#define MAX_EEPROM_OEM_INFO_LEN      256
#define M_FULLPATH_MAX_LEN      256
#define DATAPATH_LEN_MAX 128

#define GLB_INTR_HUMBER_FATAL_CHIP_DEV_NAME "/dev/ctc_asic_fatal"
#define GLB_INTR_HUMBER_NORMAL_CHIP0_DEV_NAME "/dev/ctc_asic_normal"
#ifdef FOAM_SUPPORT
#define GLB_INTR_FOAM_NORMAL_DEV_NAME "/dev/ctc_foam_normal"
#endif /*FOAM_SUPPORT*/
#define GLB_STARTUP_CONFIG_FILE_PATH     "/mnt/flash/startup-config.conf"

#define GLB_BOARD_TYPE_UNKNOWN      0xff

#define GLB_INVALID_SLOT_NO            -1

#define GLB_SLOT_NUM_MIN            1
#define GLB_SLOT_NUM_MAX_6SLOT      6
#define GLB_SLOT_NUM_MAX_14SLOT     14
#define GLB_SLOT_NUM_MAX            14

#define DEFAULT_LOW_TMPR    5
#define DEFAULT_HIGH_TMPR   75
#define DEFAULT_CRIT_TMPR   90

#define MAX_TMPR_SENSOR_NUM     6
#define MAX_PSU_NUM             3
#define MAX_FAN_TRAY_NUM        2
#define MAX_FAN_IN_ONE_TRAY     8
#define MAX_POE_PORT_NUM        48

#define TRANSCEIVER_RX_POWER_HIGH 9
#define TRANSCEIVER_RX_POWER_LOW     8
#define TRANSCEIVER_TEMP_HIGH        7
#define TRANSCEIVER_TEMP_LOW         6
#define TRANSCEIVER_VCC_HIGH         5
#define TRANSCEIVER_VCC_LOW          4
#define TRANSCEIVER_TX_BIAS_HIGH     3
#define TRANSCEIVER_TX_BIAS_LOW      2
#define TRANSCEIVER_TX_POWER_HIGH    1
#define TRANSCEIVER_TX_POWER_LOW     0

#define GLB_GE_PORT_MAX_SPEED    0
#define GLB_XG_PORT_MAX_SPEED    1

enum glb_asic_fatal_type_e
{
    GLB_ASIC_FATAL_INTR_RESET,
    GLB_ASIC_FATAL_INTR_RESET_MODULE,
    GLB_ASIC_FATAL_INTR_PARITY,
    GLB_ASIC_FATAL_INTR_LOG,
    GLB_ASIC_FATAL_INTR_MAX
};
typedef enum glb_asic_fatal_type_e glb_asic_fatal_type_t;

enum glb_reset_type_e
{
    GLB_RESET_TCAM_CORE,
    GLB_RESET_TCAM_POWER,
    GLB_RESET_GLB,
    GLB_RESET_DLLOCK,
    GLB_RESET_PCI,
    GLB_RESET_SUP,
#ifdef FOAM_SUPPORT
    GLB_RESET_FOAM_GLB,
    GLB_RESET_FOAM_PCI,
#endif /*FOAM_SUPPORT*/

    GLB_RESET_RESET_TYPE_MAX
};
typedef enum glb_reset_type_e glb_reset_type_t;

#define MAX_EEPROM_OEM_INFO_LEN      256

/*This board type id is just for humberDemo board with cavium cpu.*/
#define GLB_BOARD_HUMBER_DEMO_SPECIAL   0xfc
#define GLB_BOARD_HUMBER_DEMO   0x1
#define GLB_BOARD_SEOUL_G24EU   0x1
#define GLB_BOARD_E330_48T     0x1
#define GLB_BOARD_E330_24T     0x2
#define GLB_BOARD_E330_48S_E   0x3
#define GLB_BOARD_E330_48S     0x4
#define GLB_BOARD_B330_48T     0x1
typedef enum
{
    E_SYS_LED = 0,        
    E_RUN_LED,        
    E_ALARM_LED,
    E_FAN_LED,
    E_PSU1_LED,
    E_PSU2_LED,
    E_POE_LED,
    E_MAX_LED,
}glb_led_type_t;

typedef enum
{
    E_LED_SYS_INIT = 0,
    E_LED_SYS_RUN,
    E_LED_SYS_ABNORMAL,    
    E_LED_SYS_ALARM,
    E_LED_SYS_NO_ALARM,
    E_LED_SYS_ABSENT,
    E_LED_STAT_MAX
}glb_led_stat_t;

#ifdef _CTC_OF_
enum sys_event_e
{
    SYS_EVENT_RUN = 0,
    SYS_EVENT_TEMP,
    SYS_EVENT_FAN,
    SYS_EVENT_PSU,
    SYS_EVENT_MAX,
};
typedef enum sys_event_e sys_event_t;

enum sys_led_e
{
    SYS_LED_NORMAL = 0,
    SYS_LED_ALARM = 1,    
};
typedef enum sys_led_e sys_led_t;
#endif

/* EPLD board type Defines */
enum glb_board_series_e
{
    GLB_SERIES_E300 = 0,
    GLB_SERIES_E600,
    GLB_SERIES_E800,
    GLB_SERIES_EX2000,
    GLB_SERIES_EX3000,
    GLB_SERIES_E310,
    GLB_SERIES_E810,
    GLB_SERIES_CPCI,
    GLB_SERIES_SEOUL,
    GLB_SERIES_E330,
    GLB_SERIES_B330 = 0xb,
    GLB_SERIES_HUMBER_DEMO = 0xf,
    GLB_SERIES_MAX
};
typedef enum glb_board_series_e glb_board_series_t;

struct glb_board_type_s
{
    uint8 series;
    uint8 type;
};
typedef struct glb_board_type_s glb_board_type_t;

enum glb_chassis_type_e
{
    GLB_CHASSIS_2_SLOT,
    GLB_CHASSIS_6_SLOT,
    GLB_CHASSIS_14_SLOT,
    GLB_CHASSIS_PIZZA_BOX,
    GLB_CHASSIS_TYPE_MAX    
};
typedef enum glb_chassis_type_e glb_chassis_type_t;

enum glb_supcard_state_e
{
    GLB_SUPCARD_STATE_INIT,
    GLB_SUPCARD_STATE_ACTIVE,
    GLB_SUPCARD_STATE_STANDBY,
    GLB_SUPCARD_STATE_INVALID
};
typedef enum glb_supcard_state_e glb_supcard_state_t;
enum glb_card_fun_type_e
{
    GLB_ACTIVE_SUP_CARD,
    GLB_STANDBY_SUP_CARD,
    GLB_FWD_LINE_CARD,
    GLB_BOARD_FUN_TYPE_MAX
};
typedef enum glb_card_fun_type_e glb_card_fun_type_t;
struct glb_phy_hdl_s
{

};
typedef struct glb_phy_hdl_s glb_phy_hdl_t;

struct glb_mac_hdl_s
{

};
typedef struct glb_mac_hdl_s glb_mac_hdl_t;

struct glb_fiber_s
{

};
typedef struct glb_fiber_s glb_fiber_t;

enum glb_card_state_e
{
    BOARD_ABSENT = 0,
    BOARD_PRESENT,
    BOARD_FIRST_CREATED,
    BOARD_TYPE_CONFLICT,
    BOARD_HW_INSUFFICIENT,
    BOARD_STATE_MAX
} ;
typedef enum glb_card_state_e glb_card_state_t;

enum glb_port_state_e
{
    PORT_ABSENT = 0,
    PORT_PRESENT,
    PORT_FIRST_CREATED,
    PORT_STATE_MAX
};
typedef enum glb_port_state_e glb_port_state_t;

typedef enum glb_port_poe_e
{
    NOT_POE_PORT = 0,
    POE_PORT
} glb_port_poe_t;

enum glb_chip_stats_e
{
    LC_CHIP_STM_UNINITED,
    LC_CHIP_INITING,
    LC_CHIP_INIT_DONE,
    LC_CHIP_INIT_FAIL,
    LC_CHIP_SYNC_DONE,
    LC_CHIP_INIT_ABSENT,
};
typedef enum glb_chip_stats_e glb_chip_stats_t;

enum glb_tcam_type_e
{
    GLB_TCAM_NULL = 0,
    /*NL device*/
    GLB_TCAM_NL5512,
    GLB_TCAM_NL6512,
    GLB_TCAM_NL9K,
    /*IDT device*/
    GLB_TCAM_IDT75K72100,
    GLB_TCAM_IDT75S10010A,
    GLB_TCAM_IDT75S10005A,
    MAX_GLB_TCAM_TYPE,
};
typedef enum glb_tcam_type_e glb_tcam_type_t;

enum glb_sram_type_e
{
    GLB_SRAM_NULL,
    GLB_SRAM_18MBIT,
    GLB_SRAM_36MBIT,
    GLB_SRAM_72MBIT,
    GLB_SRAM_TYPE_MAX,
};
typedef enum glb_sram_type_e glb_sram_type_t;

enum phy_interrupt_mode_e
{
    GLB_PHY_INTERRUPT_MODE_NULL,
    GLB_PHY_INTERRUPT_MODE_FPGA,
    GLB_PHY_INTERRUPT_MODE_EPLD,
    GLB_PHY_INTERRUPT_MODE_MAX,
};
typedef enum phy_interrupt_mode_e phy_interrupt_mode_t;

enum poe_interrupt_mode_e
{
    GLB_POE_INTERRUPT_MODE_NULL,
    GLB_POE_INTERRUPT_MODE_FPGA,
    GLB_POE_INTERRUPT_MODE_EPLD,
    GLB_POE_INTERRUPT_MODE_MAX,
};
typedef enum poe_interrupt_mode_e poe_interrupt_mode_t;

struct card_stats_s
{
    struct timeval  attach_time;
    uint32        attach_count;
    struct timeval  ready_time;
    uint32        ready_count;
    struct timeval  absent_time;
    uint32        absent_count;    
};
typedef struct card_stats_s card_stats_t;

struct glb_humber_dev_s
{
    uint8 pbqdr_sram_type;    /* packet buffer qdr */
    uint8 piqdr_sram_type;    /* packet info qdr */
    uint8 tbqdr_sram_type;    /* table info qdr */    
    uint8 ddr_sram_type;      /*external sram type*/
    uint8 tcam_type;          /*external tcam type*/

};
typedef struct glb_humber_dev_s glb_humber_dev_t;

struct glb_chip_dev_s
{
    uint8 glb_chip_type;
    glb_chip_stats_t chip_states;
    void *p_priv_info;
};
typedef struct glb_chip_dev_s glb_chip_dev_t;

struct glb_port_range_s
{
    uint8 port_start;
    uint8 port_end;
};
typedef struct glb_port_range_s glb_port_range_t;

#define GLB_VCT_PAIR_ACCURACY 3
#define GLB_VCT_PAIR_NORMAL_ACCURACY    5
#define GLB_VCT_PAIR_OPEN_SHORT_ACCURACY    8
#define GLB_VCT_PAIR_CROSS_ACCURACY        10

#define GLB_VCT_DETECT_TIME  12000

typedef struct
{    
    int32   low;
    int32   high;
    int32   crit;
} tmpr_cfg_t;

typedef struct
{
    uint32 pm;
    uint32 legacy_cap;
    uint32 budget;
    uint32 budget_reserve;
    uint32 budget_threshold;
} poe_cfg_t;

typedef struct
{
    uint32 enable;
    uint32 budget;
} chsm_poe_cfg_t;

struct tmpr_status_s
{
    int32   tmpr_val; /*real time value*/
};
typedef struct tmpr_status_s tmpr_status_t;

typedef enum
{
    E330_NM_NULL,
    E330_NM_2SFP_PLUS,
    E330_NM_4SFP_PLUS,
    E330_NM_4SFP,
    E330_NM_UNSUPPORT,
}E330_daughter_card_e;

#define E330_EXT_VER_UNKNOWN 0x0
#define E330_EXT_XGCARD_VER_1 0x3
#define E330_EXT_XGCARD_VER_2 0x2

typedef enum
{
    PSU_SHOW_FULL_STATUS = 0,
    PSU_SHOW_GOOD_SIG,
    PSU_SHOW_MAX
} psu_type_e;

struct psu_status_s
{
    int8 psu_fan; /*1 mean fail, 0 mean ok*/
    int8 psu_mode; /* 1 mean AC, 0 mean DC*/
    int8 psu_work_status; /*1 mean fail, 0 mean ok.*/
    int8 psu_absent;/*1 mean absent, 0 mean present*/
    int8 shutdown;/*1 mean shutdown, 0 mean enable*/
    int8 psu_type;
};
typedef struct psu_status_s psu_status_t;

#define FAN_SPEED_FULL  100
#define FAN_FULL_SPEED_TEMP  80
#define FAN_SPEED_HIGH  70
#define FAN_HIGH_SPEED_TEMP  65
#define FAN_SPEED_LOW   30
#define FAN_LOW_SPEED_TEMP  50
#define FAN_SPEED_STALL 0
#define FAN_SPEED_TEMP_HYST 2//hysteresis threshold about temperature value for speed adjust.

/*This struct describe the fan module information.*/
struct fan_status_s
{
    uint8 num; /* fan number in one fan tray*/
    uint8 use_flag; /*mean this data is useful*/
    uint8 speed_adjust; /*whether fan support speed adjust, 0 mean no, 1 mean yes.*/
    uint8 present; /*fan tray present information*/
    uint8 status; /*every fan work status in one fan tray, each bit show one fan status. 0 mean fail, 1 mean ok.*/
    uint16 set_speed; /*config speed value*/
    uint16 get_speed[MAX_FAN_IN_ONE_TRAY]; /*get real speed value*/ 
};
typedef struct fan_status_s fan_status_t;

enum glb_phy_led_type_e
{
    GLB_PHY_LED_DEFAULT,             /* 1G phy:port has one led, copper.*/
    GLB_PHY_LED_1,                   /* 1G phy:port has one led, fiber; 10G phy: led1-link,led2-act*/
    GLB_PHY_LED_2,                   /* 1G phy:port has two led, fiber; 10G phy: led1-rx link and act,led2-same to led1*/
    GLB_PHY_LED_3,                   /* 1G phy: special for E330-48T combo port.
                                               fiber led no orange, only green.*/
    GLB_PHY_LED_4,                   /* 1G phy:combo port has two led, fiber only has green, no orange, other same to 
                                               upper LED default mode. so for fiber, 100M & 1000M led mode are same*/                                           
    GLB_PHY_LED_MAX,
};
typedef enum glb_phy_led_type_e glb_phy_led_type_t;

enum glb_remote_pair_e
{
    GLB_REMOTE_PAIR_ABSENT = 0,    /* remote pair is absent */  
    GLB_REMOTE_PAIR_PRESENT,       /* remote pair is present */ 
    GLB_REMOTE_PAIR_MAX
};
typedef enum glb_remote_pair_e glb_remote_pair_t;

enum glb_vct_status_e
{
    GLB_VCT_STATUS_CLOSED = 0,     /* VCT Mod is not start */  
    GLB_VCT_STATUS_RUNNING,        /* VCT Mod is running   */
    GLB_VCT_STATUS_FINISHED,       /* VCT Mod is finished  */
    GLB_VCT_STATUS_MAX,

};
typedef enum glb_vct_status_e glb_vct_status_t;

enum glb_pair_state_e
{
    GLB_PAIR_STATE_NORMAL = 0x0,                /* Correctly terminated pair */
    GLB_PAIR_STATE_ABNORMAL_OPEN = 0x1,         /* Open pair */
    GLB_PAIR_STATE_ABNORMAL_SHORTED = 0x2,      /* Short pair */
    GLB_PAIR_STATE_ABNORMAL_TERMINATION = 0x4,  /* Abnormal termination */
    GLB_PAIR_STATE_CROSS_PAIR_SHORT_A = 0x8,    /* Cross-pair short to pair A */
    GLB_PAIR_STATE_CROSS_PAIR_SHORT_B = 0x9,    /* Cross-pair short to pair B */
    GLB_PAIR_STATE_CROSS_PAIR_SHORT_C = 0xa,    /* Cross-pair short to pair C */
    GLB_PAIR_STATE_CROSS_PAIR_SHORT_D = 0xb,    /* Cross-pair short to pair D */
    GLB_PAIR_STATE_ABNORMAL_CROSS_PAIR_A = 0xc, /* Abnormal cross-pair coupling, pair A */
    GLB_PAIR_STATE_ABNORMAL_CROSS_PAIR_B = 0xd, /* Abnormal cross-pair coupling, pair B */
    GLB_PAIR_STATE_ABNORMAL_CROSS_PAIR_C = 0xe, /* Abnormal cross-pair coupling, pair C */
    GLB_PAIR_STATE_ABNORMAL_CROSS_PAIR_D = 0xf, /* Abnormal cross-pair coupling, pair D */
    GLB_PAIR_STATE_MAX
};
typedef enum glb_pair_state_e glb_pair_state_t;


struct glb_pair_info_s
{
    uint16 pair_length;
    uint16 pair_accuracy;
    glb_pair_state_t pair_status;
};
typedef struct glb_pair_info_s glb_pair_info_t; 

struct glb_port_cable_info_s
{
    glb_pair_info_t pair_A;
    glb_pair_info_t pair_B;
    glb_pair_info_t pair_C;
    glb_pair_info_t pair_D;
    glb_remote_pair_t remote_pair;
    uint16 cable_speed;
    uint32 port_link_up;    /* if non-zero, link is up; otherwise link is down */
    uint32 port_enable;     /* if non-zero,  is phy is noshutdown; otherwise phy is shutdown */
    glb_vct_status_t status_change;
};
typedef struct glb_port_cable_info_s glb_port_cable_info_t;

typedef struct glb_poe_sys_stat_info_s
{    
    uint32 budget;  /* system max comsumption limit (milli-watt), accuracy 100 mw*/
    uint32 cur_consump;   /* system total real time comsumption power (milli-watt)*/
    uint32 aver_consump;  /* count info */
    uint32 peak_consump;  /* count info */
    uint8  budget_reserved; /* percentage, from mgt info */
    uint8  budget_warn_threshold; /* percentage, from mgt info */
    uint8  pm;  /* poe power management mechanism */
    uint8  legacy_cap;  /* system enable or disable POE legacy PD detection */
    uint32 cur_volt;       /* current voltage (milli-voltage)*/
    uint32 aver_volt;
    uint32 peak_volt;
    uint32 poe_psu_status;
} glb_poe_sys_stat_info_t;

typedef struct glb_poe_port_stat_s
{
    uint8 admin; /* each port enable, disable or force POE */
    uint8 oper; /* status: on | off | faulty | power-deny */
    uint8 class; /* IEEE af/at standard class0/1/2/3/4 */
    uint32 cur_consump; /* port consumption (milli-watt), accuracy 100 mw */
    uint32 aver_consump; /* average */ 
    uint32 peak_consump; /* history peak */
    uint32 budget; /* port consumption limit (milli-watt) is valid when static pm mode */
    uint8 priority;
} glb_poe_port_stat_info_t;

typedef struct glb_poe_sys_stat_change_s
{    
    int budget_change;  /* if 1, changed */
    int cur_consump_change;   /* if 1, changed */
    int aver_consump_change;  /* if 1, changed */
    int peak_consump_change;  /* if 1, changed */
    int budget_reserved_change; /* if 1, changed */
    int budget_warn_threshold_change; /* if 1, changed */
    int pm_change;  /* if 1, changed */
    int legacy_cap_change;  /* if 1, changed */
    int cur_volt_change;    /* if 1, changed */
    int aver_volt_change;   /* if 1, changed */
    int peak_volt_change;   /* if 1, changed */
} glb_poe_sys_stat_change_t;

typedef struct glb_poe_port_stat_change_s
{
    int admin_change; /* if 1, changed */
    int oper_change;  /* if 1, changed */
    int class_change; /* if 1, changed */
    int cur_consump_change;  /* if 1, changed */
    int aver_consump_change; /* if 1, changed */
    int peak_consump_change; /* if 1, changed */
    int budget_change;       /* if 1, changed */
    int priority_change;     /* if 1, changed */
} glb_poe_port_stat_change_t;

enum glb_cpu_type_e
{
    GLB_CPU_PPC_MPC8247 = 0,                   /* POWPERPC:MPC8247*/
    GLB_CPU_MIPS_CN5010,                   /* MIPS:CN5010*/
    GLB_CPU_MAX,
};
typedef enum glb_cpu_type_e glb_cpu_type_t;

typedef struct glb_card_s glb_card_t;

struct glb_port_s
{
    uint8 port_idx;                 /* idx of port, start from NO.1 */
    uint8 mac_idx;                  /* mac index of ASIC chip */
    uint8 logic_port_idx;           /* logic port of ASIC chip, local phy port */
    uint8 mux_enable;               /* mux enable flag */
    uint8 logic_slot_idx;           /* logic slot id */
    uint8 glb_chip_idx;             /* global chip id */
    uint8 local_chip_idx;           /* local chip id */
    uint8 is_combo;                 /* is a combo port or not. src */
    uint8 is_combo_to;              /* is this port combo to another port.*/
    uint8 combo_port_idx;           /* which port does this port is combo with*/
    uint8 port_max_speed;           /* port sustaining max speed. 1 sustain 10G; 0 sustain 1G.*/
    uint8 panel_slot_no;  /*slot number show on panel, for e330-48t, NM-4SFP, NM-2SFP+, NM-4SFP+*/
    uint8 panel_port_no;  /*port number show on panel, for e330-48t, NM-4SFP, NM-2SFP+, NM-4SFP+*/
    uint8 poe_support;              /* support poe or not */
    glb_phy_type_t phy_type;        /* phy type */
    glb_lb_phy_t lp_type;           /* lp type*/
    glb_port_state_t port_state;    /* port state */
    glb_phy_state_t port_status;    /* port physical state */    
    glb_port_cable_info_t port_cable_status;   /* port cable state */
    void* p_fiber;                  /* fiber module information and handler */
    uint8 fiber_scan_cnt;
    uint8 fiber_access_err_cnt;     /* fiber module access error count*/
    uint8 create_done;              /* port created in hsrv and hagt*/
                                    /* 1 stand for current configure to port , 
                                      0 stand for not created*/
    /* field entered by users */
    glb_port_cfg_t port_cfg;            
    glb_card_t* p_host_card;  /* board information of port */
    glb_poe_port_stat_info_t    poe_port_status;
};
typedef struct glb_port_s glb_port_t;

struct glb_card_s
{
    glb_board_type_t board_type;
    glb_board_type_t config_board_type;
    glb_card_state_t card_state;
    glb_card_fun_type_t card_fun_type;   /* working mode */
    char serial_no[MAX_SERIAL_NO_LEN+1];
    char bootrom_ver[MAX_BOOTROM_VER_LEN+1];
    char cfg_profile_name[DATAPATH_LEN_MAX+1]; /*store datapath profile name*/
    uint8 hw_ver;
    uint8 sw_ver;
    uint8 epld_ver;
    uint32 epld_date;
    uint16 epld_time;
    uint8 fpga_ver;
    uint32 flash_size;/* the CPU subsystem flash size */
    uint32 dram_size; /* the CPU subsystem dram size */
    uint8 tcam_type;
    uint8 sram_type;
    uint8 phy_chip_num;
    uint8 asic_chip_num;
    uint8 l2switch_num;
    uint8 sensor_num;
    uint8 poe_port_num;
    uint8 fan_module_num;
    uint8 fan_module_ver; /*store fan module version*/
    uint8 psu_module_num;
    uint8 daughter_card_type;
    uint8 daughter_card_ver; /*store daughter card version*/
    uint16 logic_slot_no;
    uint16 phy_slot_no;
    uint16 port_num;
    uint16 bootup_diag_level;
    card_stats_t stats;
    glb_port_range_t* p_port_range;  /* the array which can get the port range by phy chip index */
    glb_chip_dev_t* p_asic_chip;
    glb_port_t** pp_port;
#ifdef _CTC_OF_
    int dev_port_gport[256];
    int gport_dev_port[256];
#endif
    void* p_sess;   /* line card connection session with CHSM */    
    /* non-network port PHY */
    /* thermal sensor */
    tmpr_cfg_t          tmpr_cfg;
    tmpr_status_t       tmpr_status[MAX_TMPR_SENSOR_NUM];
    /*psu*/
    psu_status_t        psu_status[MAX_PSU_NUM];    
    uint8    psu_poe_status;
    /*fan*/
    fan_status_t        fan_status[MAX_FAN_TRAY_NUM];
    /*poe*/
    poe_cfg_t           poe_cfg;
    glb_poe_sys_stat_info_t     poe_sys_status;
    /* eeprom */
    eeprom_info_t* p_eeprom_alloc;    /* eeprom info  */
    void *p_board_priv;   /* supervisor card or line card specific attributes. SYSMAC */
    uint8 phy_interrupt_mode;/* where we can get phy interrupt info, fpga or epld*/
    uint8 poe_interrupt_mode;
    uint8 ptp_quanta;
#ifdef FOAM_SUPPORT
    uint8 is_foam_supported;
#endif /*FOAM_SUPPORT*/
    uint8 watchdog_supported;
    glb_cpu_type_t cpu_type;
};
#endif /* !_GLB_HW_DEFINE_H_ */
