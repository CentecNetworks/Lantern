/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include <sys/mman.h>

#include "drv_error.h"
#include "drv_common.h"
#include "drv_humber_data_path.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

#define DATAPATH_WRITE_CHIP(CHIP, ADDRESS, VALUE)    \
        if(dal_usrctrl_write_chip((CHIP), (ADDRESS), (VALUE))) \
            return DRV_E_DATAPATH_WRITE_CHIP_FAIL
#define DATAPATH_READ_CHIP(CHIP, ADDRESS, VALUE)     \
        if(dal_usrctrl_read_chip((CHIP), (ADDRESS), (uint32)(VALUE))) \
            return DRV_E_DATAPATH_READ_CHIP_FAIL

#define EMPTY_LINE(C)     ((C) == '\0' ||(C) == '\r' || (C) == '\n' )
#define WHITE_SPACE(C)    ((C) == '\t' || (C) == ' ')

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/
static drv_datapath_master_t datapath_master;


/****************************************************************************
 *
* Function
*
*****************************************************************************/
/***Parser datapath config file tools start *************************/
static bool is_used;
static uint32 value;
static uint32 entry;

static drv_chip_item_type_t chip_item;

static drv_serdes_item_type_t serdes_item;
static drv_serdes_type_t serdes_type;
static drv_serdes_speed_t serdes_speed;
static drv_serdes_drive_strength_t serdes_drive_strength;

static drv_pll_item_type_t pll_item;
static uint32 pll_input;
static uint32 pll_output_a;
static uint32 pll_output_b;
static uint32 pll_cfg1;
static uint32 pll_cfg2;
static uint32 pll_cfg3;

static drv_ext_mem_item_type_t ext_mem_item;
static uint32 ext_mem_freq;

static drv_misc_item_type_t misc_item;

static drv_mdio_item_type_t mdio_item;
static drv_led_item_type_t led_item;
static drv_fabric_item_type_t fabric_item;
static drv_calendar_item_type_t calendar_ptr_item;
static drv_calendar_item_type_t calendar_entry_item;

static uint32 debug_chip_id;
static uint32 debug_addr;
static uint32 debug_oper;
static uint32 debug_value;


static int32
drv_datapath_read_bool_info(const char* line, void* para);

static int32
drv_datapath_read_size_info(const char* line, void* para);

static int32
drv_datapath_read_chip_item(const char* line, void* para);

static int32
drv_datapath_read_serdes_type_info(const char* line, void* para);

static int32
drv_datapath_read_serdes_drive_strength_info(const char* line, void* para);

static int32
drv_datapath_read_serdes_item(const char* line, void* para);

static int32
drv_datapath_read_pll_item(const char* line, void* para);

static int32
drv_datapath_read_ext_mem_item(const char* line, void* para);

static int32
drv_datapath_read_mdio_item(const char* line, void* para);

static int32
drv_datapath_read_led_item(const char* line, void* para);

static int32
drv_datapath_read_misc_item(const char* line, void* para);

static int32
drv_datapath_read_cpu_mac_type_info(const char* line, void* para);

static int32
drv_datapath_read_fabric_item(const char* line, void* para);

static int32
drv_datapath_read_calendar_item(const char* line, void* para);

static int32
drv_datapath_read_oper_info(const char* line, void* para);

drv_para_pair_t g_chip_para_pair_t[] =
{
{"[CHIP_ITEM]", drv_datapath_read_chip_item, &chip_item},
{"[CHIP_SEQ]", drv_datapath_read_size_info, &value},
{NULL, NULL, NULL}
};

drv_para_pair_t g_serdes_para_pair_t[] =
{
{"[SERDES_ITEM]", drv_datapath_read_serdes_item, &serdes_item},
{"[SERDES_TYPE]", drv_datapath_read_serdes_type_info, &serdes_type},
{"[SERDES_SPEED]", drv_datapath_read_size_info, &serdes_speed},
{"[SERDES_DRIVESTRENGTH]", drv_datapath_read_serdes_drive_strength_info, &serdes_drive_strength},
{NULL, NULL, NULL}
};

drv_para_pair_t g_pll_para_pair_t[] =
{
{"[PLL_ITEM]", drv_datapath_read_pll_item, &pll_item},
{"[PLL_USED]", drv_datapath_read_bool_info, &is_used},
{"[PLL_INPUT]", drv_datapath_read_size_info, &pll_input},
{"[PLL_OUTPUTA]", drv_datapath_read_size_info, &pll_output_a},
{"[PLL_OUTPUTB]", drv_datapath_read_size_info, &pll_output_b},
{"[PLL_CFG1]", drv_datapath_read_size_info, &pll_cfg1},
{"[PLL_CFG2]", drv_datapath_read_size_info, &pll_cfg2},
{"[PLL_CFG3]", drv_datapath_read_size_info, &pll_cfg3},
{NULL, NULL, NULL}
};

drv_para_pair_t g_ext_mem_para_pair_t[] =
{
{"[EXTMEMORY_ITEM]", drv_datapath_read_ext_mem_item, &ext_mem_item},
{"[EXTMEMORY_USED]", drv_datapath_read_bool_info, &is_used},
{"[EXTMEMORY_FREQ]", drv_datapath_read_size_info, &ext_mem_freq},
{NULL, NULL, NULL}
};

drv_para_pair_t g_mdio_para_pair_t[] =
{
{"[MDIO_ITEM]", drv_datapath_read_mdio_item, &mdio_item},
{"[MDIO_USED]", drv_datapath_read_bool_info, &is_used},
{NULL, NULL, NULL}
};

drv_para_pair_t g_led_para_pair_t[] =
{
{"[LED_ITEM]", drv_datapath_read_led_item, &led_item},
{"[LED_USED]", drv_datapath_read_bool_info, &is_used},
{NULL, NULL, NULL}
};

drv_para_pair_t g_misc_para_pair_t[] =
{
{"[MISC_ITEM]", drv_datapath_read_misc_item, &misc_item},
{"[MISC_USED]", NULL, &value},
{NULL, NULL, NULL}
};

drv_para_pair_t g_fabric_para_pair_t[] =
{
{"[FABRIC_ITEM]", drv_datapath_read_fabric_item, &fabric_item},
{"[FABRIC_VALUE]", NULL, &value},
{NULL, NULL, NULL}
};

drv_para_pair_t g_calendar_ptr_para_pair_t[] =
{
{"[CALENDARPTR_ITEM]", drv_datapath_read_calendar_item, &calendar_ptr_item},
{"[CALENDARPTR_VALUE]", drv_datapath_read_size_info, &value},
{NULL, NULL, NULL}
};

drv_para_pair_t g_calendar_entry_para_pair_t[] =
{
{"[CALENDARENTRY_ITEM]", drv_datapath_read_calendar_item, &calendar_entry_item},
{"[CALENDARENTRY_ENTRY]", drv_datapath_read_size_info, &entry},
{"[CALENDARENTRY_VALUE]", drv_datapath_read_size_info, &value},
{NULL, NULL, NULL}
};

drv_para_pair_t g_debug_para_pair_t[] =
{
{"[Chip]", drv_datapath_read_size_info, &debug_chip_id},
{"[Addr]", drv_datapath_read_size_info, &debug_addr},
{"[Oper]", drv_datapath_read_oper_info, &debug_oper},
{"[Value]", drv_datapath_read_size_info, &debug_value},
{NULL, NULL, NULL}
};


static int32
drv_datapath_string_atrim(char* output, const char* input)
{
    char *p = NULL;

    DRV_PTR_VALID_CHECK(input);
    DRV_PTR_VALID_CHECK(output);

    /*trim left space*/
    while(*input != '\0')
    {
        if(WHITE_SPACE(*input))
        {
            ++input;
        }
        else
        {
            break;
        }
    }
    kal_strcpy(output, input);
    /*trim right space*/
    p = output + kal_strlen(output) - 1;

    while(p >= output)
    {
      /*skip empty line*/
        if(WHITE_SPACE(*p) || ('\r' == (*p)) || ('\n' == (*p)))
        {
            --p;
        }
        else
        {
            break;
        }
    }

    *(++p) = '\0';

    return DRV_E_NONE;
}

static int32
drv_datapath_read_bool_info(const char* line, void* para)
{
    char *ch = NULL;
    bool *alloc_info = (bool *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if(0 == kal_strncmp(ch, "Y", kal_strlen("Y")))
    {
        *alloc_info = 1;
    }
    else if(0 == kal_strncmp(ch, "N", kal_strlen("N")))
    {
        *alloc_info = 0;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_size_info(const char* line, void* para)
{
    uint32 total = 0;
    uint32 *mem_size = (uint32 *)para;
    char *ch = NULL;

    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (!kal_memcmp(ch, "0x", 2) || !kal_memcmp(ch, "0X", 2))
    {
        total = kal_strtou32(ch, NULL, 16);

        *mem_size = total;
    }
    else if(!kal_memcmp(ch, "NA", 2))
    {
        *mem_size = 0;
    }
    else
    {
        kal_sscanf(ch, "%u", mem_size);
    }

    return 0;
}

static int32
drv_datapath_read_chip_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_chip_item_type_t *chip_item = (drv_chip_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "ChipId", kal_strlen("ChipId")) == 0)
    {
        *chip_item = DRV_CHIP_ITEM_TYPE_CHIP_ID;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_serdes_type_info(const char* line, void* para)
{
    char *ch = NULL;
    drv_serdes_type_t *serdes_type = (drv_serdes_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "GE", kal_strlen("GE")) == 0)
    {
        *serdes_type = DRV_SERDES_TYPE_GE;
    }
    else if (kal_strncmp(ch, "SG", kal_strlen("SG")) == 0)
    {
        *serdes_type = DRV_SERDES_TYPE_SG;
    }
    else if (kal_strncmp(ch, "XG", kal_strlen("XG")) == 0)
    {
        *serdes_type = DRV_SERDES_TYPE_XG;
    }
    else if (kal_strncmp(ch, "FB", kal_strlen("FB")) == 0)
    {
        *serdes_type = DRV_SERDES_TYPE_FB;
    }
    else if (kal_strncmp(ch, "NA", kal_strlen("NA")) == 0)
    {
        *serdes_type = DRV_SERDES_TYPE_NULL;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_serdes_drive_strength_info(const char* line, void* para)
{
    char *ch = NULL;
    drv_serdes_drive_strength_t *serdes_drive_strength = (drv_serdes_drive_strength_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "High", kal_strlen("High")) == 0)
    {
        *serdes_drive_strength = DRV_SERDES_DRV_STRENGTH_HIGH;
    }
    else if (kal_strncmp(ch, "Mid", kal_strlen("Mid")) == 0)
    {
        *serdes_drive_strength = DRV_SERDES_DRV_STRENGTH_MID;
    }
    else if (kal_strncmp(ch, "Low", kal_strlen("Low")) == 0)
    {
        *serdes_drive_strength = DRV_SERDES_DRV_STRENGTH_LOW;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_serdes_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_serdes_item_type_t *serdes_item = (drv_serdes_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "NTSG10", kal_strlen("NTSG10")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG10;
    }
    else if (kal_strncmp(ch, "NTSG11", kal_strlen("NTSG11")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG11;
    }
    else if (kal_strncmp(ch, "NTSG12", kal_strlen("NTSG12")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG12;
    }
    else if (kal_strncmp(ch, "NTSG13", kal_strlen("NTSG13")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG13;
    }
    else if (kal_strncmp(ch, "NTSG14", kal_strlen("NTSG14")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG14;
    }
    else if (kal_strncmp(ch, "NTSG15", kal_strlen("NTSG15")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG15;
    }
    else if (kal_strncmp(ch, "NTSG16", kal_strlen("NTSG16")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG16;
    }
    else if (kal_strncmp(ch, "NTSG17", kal_strlen("NTSG17")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG17;
    }
    else if (kal_strncmp(ch, "NTSG18", kal_strlen("NTSG18")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG18;
    }
    else if (kal_strncmp(ch, "NTSG19", kal_strlen("NTSG19")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG19;
    }
    else if (kal_strncmp(ch, "NTSG20", kal_strlen("NTSG20")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG20;
    }
    else if (kal_strncmp(ch, "NTSG21", kal_strlen("NTSG21")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG21;
    }
    else if (kal_strncmp(ch, "NTSG22", kal_strlen("NTSG22")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG22;
    }
    else if (kal_strncmp(ch, "NTSG23", kal_strlen("NTSG23")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG23;
    }
    else if (kal_strncmp(ch, "NTSG24", kal_strlen("NTSG24")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG24;
    }
    else if (kal_strncmp(ch, "NTSG25", kal_strlen("NTSG25")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG25;
    }
    else if (kal_strncmp(ch, "NTSG26", kal_strlen("NTSG26")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG26;
    }
    else if (kal_strncmp(ch, "NTSG27", kal_strlen("NTSG27")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG27;
    }
    else if (kal_strncmp(ch, "NTSG28", kal_strlen("NTSG82")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG28;
    }
    else if (kal_strncmp(ch, "NTSG29", kal_strlen("NTSG29")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG29;
    }
    else if (kal_strncmp(ch, "NTSG30", kal_strlen("NTSG30")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG30;
    }
    else if (kal_strncmp(ch, "NTSG31", kal_strlen("NTSG31")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG31;
    }
    else if (kal_strncmp(ch, "NTSG32", kal_strlen("NTSG32")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG32;
    }
    else if (kal_strncmp(ch, "NTSG33", kal_strlen("NTSG33")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG33;
    }
    else if (kal_strncmp(ch, "NTSG34", kal_strlen("NTSG34")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG34;
    }
    else if (kal_strncmp(ch, "NTSG35", kal_strlen("NTSG35")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG35;
    }
    else if (kal_strncmp(ch, "NTSG36", kal_strlen("NTSG36")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG36;
    }
    else if (kal_strncmp(ch, "NTSG37", kal_strlen("NTSG37")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG37;
    }
    else if (kal_strncmp(ch, "NTSG38", kal_strlen("NTSG38")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG38;
    }
    else if (kal_strncmp(ch, "NTSG39", kal_strlen("NTSG39")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG39;
    }
    else if (kal_strncmp(ch, "NTSG40", kal_strlen("NTSG40")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG40;
    }
    else if (kal_strncmp(ch, "NTSG41", kal_strlen("NTSG41")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG41;
    }
    else if (kal_strncmp(ch, "NTSG42", kal_strlen("NTSG42")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG42;
    }
    else if (kal_strncmp(ch, "NTSG43", kal_strlen("NTSG43")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG43;
    }
    else if (kal_strncmp(ch, "NTSG44", kal_strlen("NTSG44")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG44;
    }
    else if (kal_strncmp(ch, "NTSG45", kal_strlen("NTSG45")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG45;
    }
    else if (kal_strncmp(ch, "NTSG46", kal_strlen("NTSG46")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG46;
    }
    else if (kal_strncmp(ch, "NTSG47", kal_strlen("NTSG47")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG47;
    }
    else if (kal_strncmp(ch, "NTSG0", kal_strlen("NTSG0")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG0;
    }
    else if (kal_strncmp(ch, "NTSG1", kal_strlen("NTSG1")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG1;
    }
    else if (kal_strncmp(ch, "NTSG2", kal_strlen("NTSG2")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG2;
    }
    else if (kal_strncmp(ch, "NTSG3", kal_strlen("NTSG3")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG3;
    }
    else if (kal_strncmp(ch, "NTSG4", kal_strlen("NTSG4")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG4;
    }
    else if (kal_strncmp(ch, "NTSG5", kal_strlen("NTSG5")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG5;
    }
    else if (kal_strncmp(ch, "NTSG6", kal_strlen("NTSG6")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG6;
    }
    else if (kal_strncmp(ch, "NTSG7", kal_strlen("NTSG7")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG7;
    }
    else if (kal_strncmp(ch, "NTSG8", kal_strlen("NTSG8")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG8;
    }
    else if (kal_strncmp(ch, "NTSG9", kal_strlen("NTSG9")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_NTSG9;
    }
    else if (kal_strncmp(ch, "UPSG10", kal_strlen("UPSG10")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG10;
    }
    else if (kal_strncmp(ch, "UPSG11", kal_strlen("UPSG11")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG11;
    }
    else if (kal_strncmp(ch, "UPSG12", kal_strlen("UPSG12")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG12;
    }
    else if (kal_strncmp(ch, "UPSG13", kal_strlen("UPSG13")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG13;
    }
    else if (kal_strncmp(ch, "UPSG14", kal_strlen("UPSG14")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG14;
    }
    else if (kal_strncmp(ch, "UPSG15", kal_strlen("UPSG15")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG15;
    }
    else if (kal_strncmp(ch, "UPSG0", kal_strlen("UPSG0")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG0;
    }
    else if (kal_strncmp(ch, "UPSG1", kal_strlen("UPSG1")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG1;
    }
    else if (kal_strncmp(ch, "UPSG2", kal_strlen("UPSG2")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG2;
    }
    else if (kal_strncmp(ch, "UPSG3", kal_strlen("UPSG3")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG3;
    }
    else if (kal_strncmp(ch, "UPSG4", kal_strlen("UPSG4")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG4;
    }
    else if (kal_strncmp(ch, "UPSG5", kal_strlen("UPSG5")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG5;
    }
    else if (kal_strncmp(ch, "UPSG6", kal_strlen("UPSG6")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG6;
    }
    else if (kal_strncmp(ch, "UPSG7", kal_strlen("UPSG7")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG7;
    }
    else if (kal_strncmp(ch, "UPSG8", kal_strlen("UPSG8")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG8;
    }
    else if (kal_strncmp(ch, "UPSG9", kal_strlen("UPSG9")) == 0)
    {
        *serdes_item = DRV_SERDES_ITEM_UPSG9;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_pll_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_pll_item_type_t *pll_item = (drv_pll_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "Core", kal_strlen("Core")) == 0)
    {
        *pll_item = DRV_PLL_ITEM_CORE;
    }
    else if (kal_strncmp(ch, "Hss4G", kal_strlen("Hss4G")) == 0)
    {
        *pll_item = DRV_PLL_ITEM_HSS4G;
    }
    else if (kal_strncmp(ch, "Hss6G", kal_strlen("Hss6G")) == 0)
    {
        *pll_item = DRV_PLL_ITEM_HSS6G;
    }
    else if (kal_strncmp(ch, "Table", kal_strlen("Table")) == 0)
    {
        *pll_item = DRV_PLL_ITEM_TABLE;
    }
    else if (kal_strncmp(ch, "Tcam", kal_strlen("Tcam")) == 0)
    {
        *pll_item = DRV_PLL_ITEM_TCAM;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_ext_mem_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_ext_mem_item_type_t *ext_mem_item = (drv_ext_mem_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "DDR", kal_strlen("DDR")) == 0)
    {
        *ext_mem_item = DRV_EXT_MEM_ITEM_DDR;
    }
    else if (kal_strncmp(ch, "QDR", kal_strlen("QDR")) == 0)
    {
        *ext_mem_item = DRV_EXT_MEM_ITEM_QDR;
    }
    else if (kal_strncmp(ch, "TCAM", kal_strlen("TCAM")) == 0)
    {
        *ext_mem_item = DRV_EXT_MEM_ITEM_TCAM;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_mdio_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_mdio_item_type_t *mdio_item = (drv_mdio_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "Mdio0", kal_strlen("Mdio0")) == 0)
    {
        *mdio_item = DRV_MDIO_ITEM_MDIO0;
    }
    else if (kal_strncmp(ch, "Mdio1", kal_strlen("Mdio1")) == 0)
    {
        *mdio_item = DRV_MDIO_ITEM_MDIO1;
    }
    else if (kal_strncmp(ch, "Mdio2", kal_strlen("Mdio2")) == 0)
    {
        *mdio_item = DRV_MDIO_ITEM_MDIO2;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_led_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_led_item_type_t *led_item = (drv_led_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "Led0", kal_strlen("Led0")) == 0)
    {
        *led_item = DRV_LED_ITEM_LED0;
    }
    else if (kal_strncmp(ch, "Led1", kal_strlen("Led1")) == 0)
    {
        *led_item = DRV_LED_ITEM_LED1;
    }
    else if (kal_strncmp(ch, "Led2", kal_strlen("Led2")) == 0)
    {
        *led_item = DRV_LED_ITEM_LED2;
    }
    else if (kal_strncmp(ch, "Led3", kal_strlen("Led3")) == 0)
    {
        *led_item = DRV_LED_ITEM_LED3;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_misc_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_misc_item_type_t *misc_item = (drv_misc_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }
    if(kal_strncmp(ch, "HumberCpuMacRefClk", kal_strlen("HumberCpuMacRefClk")) == 0)
    {
        *misc_item = DRV_MISC_ITEM_CPU_MAC_REF_CLK;
    }
    else if (kal_strncmp(ch, "HumberCpuMac", kal_strlen("HumberCpuMac")) == 0)
    {
        *misc_item = DRV_MISC_ITEM_CPU_MAC;
    }
    else if (kal_strncmp(ch, "PtpEngine", kal_strlen("PtpEngine")) == 0)
    {
        *misc_item = DRV_MISC_ITEM_PTP_ENGINE;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_cpu_mac_type_info(const char* line, void* para)
{
    char *ch = NULL;
    drv_cpumac_speed_t *cpu_mac_type = (drv_cpumac_speed_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "FE", kal_strlen("FE")) == 0)
    {
        *cpu_mac_type = CPUMAC_SPEED_100M;
    }
    else if (kal_strncmp(ch, "GE", kal_strlen("GE")) == 0)
    {
        *cpu_mac_type = CPUMAC_SPEED_1G;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}


static int32
drv_datapath_read_cpu_mac_ref_clk(const char* line, void* para)
{
    char *ch = NULL;
    drv_cpumac_refclk_t *cpu_mac_ref_clk = (drv_cpumac_refclk_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);
  
    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }
    
    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "Internal", kal_strlen("Internal")) == 0)
    {
        *cpu_mac_ref_clk = CPUMAC_REFCLK_INTERNAL;
    }
    else if (kal_strncmp(ch, "External", kal_strlen("External")) == 0)
    {
        *cpu_mac_ref_clk = CPUMAC_REFCLK_EXTERNAL;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }
    
    return DRV_E_NONE;
}

static int32
drv_datapath_read_fabric_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_fabric_item_type_t *fabric_item = (drv_fabric_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);
  
    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }
    
    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "Master", kal_strlen("Master")) == 0)
    {
        *fabric_item = DRV_FABRIC_ITEM_MASTER;
    }
    else if (kal_strncmp(ch, "FabricTimer1", kal_strlen("FabricTimer1")) == 0)
    {
        *fabric_item = DRV_FABRIC_ITEM_FABRIC_TIMER1;
    }
    else if (kal_strncmp(ch, "FabricTimer2", kal_strlen("FabricTimer2")) == 0)
    {
        *fabric_item = DRV_FABRIC_ITEM_FABRIC_TIMER2;
    }
    else if (kal_strncmp(ch, "FabricTimer3-Expire", kal_strlen("FabricTimer3-Expire")) == 0)
    {
        *fabric_item = DRV_FABRIC_ITEM_FABRIC_TIMER3_EXPIRE;
    }
    else if(kal_strncmp(ch, "FabricTimer3-StartChk", kal_strlen("FabricTimer3-StartChk")) == 0)
    {
        *fabric_item = DRV_FABRIC_ITEM_FABRIC_TIMER3_START_TO_CHECK;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_calendar_item(const char* line, void* para)
{
    char *ch = NULL;
    drv_calendar_item_type_t *calendar_item = (drv_calendar_item_type_t *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if (kal_strncmp(ch, "MacMux", kal_strlen("MacMux")) == 0)
    {
        *calendar_item = DRV_CALENDAR_PTR_ITEM_MAC_MUX;
    }
    else if (kal_strncmp(ch, "NetTx", kal_strlen("NetTx")) == 0)
    {
        *calendar_item = DRV_CALENDAR_PTR_ITEM_NET_TX;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

static int32
drv_datapath_read_oper_info(const char* line, void* para)
{
    char *ch = NULL;
    uint32 *oper = (uint32 *)para;
    DRV_PTR_VALID_CHECK(line);
    DRV_PTR_VALID_CHECK(para);

    ch = kal_strstr(line, "=");
    if(NULL == ch)
    {
        return DRV_E_INVALID_PARAM;
    }
    else
    {
        ch++;
    }

    while(kal_isspace((unsigned char)*ch))
    {
        ch++;
    }

    if(0 == kal_strncmp(ch, "write", kal_strlen("write")))
    {
        *oper = 1;
    }
    else if(0 == kal_strncmp(ch, "read", kal_strlen("read")))
    {
        *oper = 0;
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}


int32
drv_read_datapath_profile(const char* file_name, drv_datapath_info_t* datapath_info)
{
    char    string[128];
    char    line[128];
    FILE   *local_fp = NULL;
    int32   ret;
    uint32  count = 0;
    uint32  para_count = 0;
    void   *p_temp = NULL;

    DRV_PTR_VALID_CHECK(file_name);
    DRV_PTR_VALID_CHECK(datapath_info);

    /*Give the default value.*/
    datapath_info->misc_item[DRV_MISC_ITEM_CPU_MAC_REF_CLK].value = CPUMAC_REFCLK_EXTERNAL;
    
    /*OPEN FILE*/
    local_fp = fopen(file_name, "r");

    if((NULL == local_fp) || feof(local_fp))
    {
        return DRV_E_INVALID_PARAM;
    }
    /*parse profile*/
    while(!feof(local_fp))
    {
        kal_memset(string, 0, sizeof(string));
        fgets(string, 128, local_fp);

        /*comment line*/
        if('#' == string[0])
          continue;
        /*trim left and right space*/
        kal_memset(line, 0, sizeof(line));

        ret = drv_datapath_string_atrim(line, string);

        if(EMPTY_LINE(line[0]))
        {
          continue;
        }

        count = 0;
        while(g_chip_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_chip_para_pair_t[count].para_name, line,
                   kal_strlen(g_chip_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_chip_para_pair_t[count].argus;

                g_chip_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_chip_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->chip_item.seq = value;

            para_count = 0;
            chip_item = 0;
            value = 0;
        }

        count = 0;
        while(g_serdes_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_serdes_para_pair_t[count].para_name, line,
                   kal_strlen(g_serdes_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_serdes_para_pair_t[count].argus;

                g_serdes_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_serdes_para_pair_t[count].para_name != NULL) &&
             (para_count == 4))
        {
            datapath_info->serdes_item[serdes_item].type = serdes_type;
            datapath_info->serdes_item[serdes_item].speed = serdes_speed;
            datapath_info->serdes_item[serdes_item].drv_strength = serdes_drive_strength;

            para_count = 0;
            serdes_item = 0;
            serdes_type = 0;
            serdes_speed = 0;
            serdes_drive_strength = 0;
        }

        count = 0;
        while(g_pll_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_pll_para_pair_t[count].para_name, line,
                   kal_strlen(g_pll_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_pll_para_pair_t[count].argus;

                g_pll_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_pll_para_pair_t[count].para_name != NULL) &&
             (para_count == 8))
        {
            datapath_info->pll_item[pll_item].is_used = is_used;
            datapath_info->pll_item[pll_item].input = pll_input;
            datapath_info->pll_item[pll_item].output_a = pll_output_a;
            datapath_info->pll_item[pll_item].output_b = pll_output_b;
            datapath_info->pll_item[pll_item].cfg1 = pll_cfg1;
            datapath_info->pll_item[pll_item].cfg2 = pll_cfg2;
            datapath_info->pll_item[pll_item].cfg3 = pll_cfg3;

            para_count = 0;
            pll_item = 0;
            is_used = 0;
            pll_input = 0;
            pll_output_a = 0;
            pll_output_b = 0;
            pll_cfg1 = 0;
            pll_cfg2 = 0;
            pll_cfg3 = 0;
        }

        count = 0;
        while(g_ext_mem_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_ext_mem_para_pair_t[count].para_name, line,
                   kal_strlen(g_ext_mem_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_ext_mem_para_pair_t[count].argus;

                g_ext_mem_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_ext_mem_para_pair_t[count].para_name != NULL) &&
             (para_count == 3))
        {
            datapath_info->ext_mem_item[ext_mem_item].is_used = is_used;
            datapath_info->ext_mem_item[ext_mem_item].freq = ext_mem_freq;

            para_count = 0;
            ext_mem_item = 0;
            is_used = 0;
            ext_mem_freq = 0;
        }

        count = 0;
        while(g_mdio_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_mdio_para_pair_t[count].para_name, line,
                   kal_strlen(g_mdio_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_mdio_para_pair_t[count].argus;

                g_mdio_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_mdio_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->mdio_item[mdio_item].value = is_used;

            para_count = 0;
            mdio_item = 0;
            is_used = 0;
        }

        count = 0;
        while(g_led_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_led_para_pair_t[count].para_name, line,
                   kal_strlen(g_led_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_led_para_pair_t[count].argus;

                g_led_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_led_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->led_item[led_item].value = is_used;

            para_count = 0;
            led_item = 0;
            is_used = 0;
        }

        count = 0;
        while(g_misc_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_misc_para_pair_t[count].para_name, line,
                   kal_strlen(g_misc_para_pair_t[count].para_name)) == 0)
            {
                if(kal_strncmp(g_misc_para_pair_t[count].para_name, "[MISC_USED]", 11) == 0)
                {
                    p_temp = g_misc_para_pair_t[count].argus;

                    if(DRV_MISC_ITEM_CPU_MAC == misc_item)
                    {
                        drv_datapath_read_cpu_mac_type_info(line, p_temp);
                    }
                    else if(DRV_MISC_ITEM_PTP_ENGINE == misc_item) 
                    {
                        drv_datapath_read_bool_info(line, p_temp);
                    }
                    else /*DRV_MISC_ITEM_CPU_MAC_REF_CLK*/ 
                    {
                        drv_datapath_read_cpu_mac_ref_clk(line, p_temp);
                    }

                    para_count ++;
                }
                else
                {
                    p_temp = g_misc_para_pair_t[count].argus;
                    g_misc_para_pair_t[count].fun_ptr(line, p_temp);
                    para_count ++;
                }

                break;
            }
            count++;
        }
        if ((g_misc_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->misc_item[misc_item].value = value;
            para_count = 0;
            misc_item = 0;
            value = 0;
        }

        count = 0;
        while(g_fabric_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_fabric_para_pair_t[count].para_name, line,
                   kal_strlen(g_fabric_para_pair_t[count].para_name)) == 0)
            {
                if(kal_strncmp(g_fabric_para_pair_t[count].para_name, "[FABRIC_VALUE]", 14) == 0)
                {
                    p_temp = g_fabric_para_pair_t[count].argus;

                    if(DRV_FABRIC_ITEM_MASTER == fabric_item)
                    {
                        drv_datapath_read_bool_info(line, p_temp);
                    }
                    else /*DRV_FABRIC_ITEM_TIMER1~3*/
                    {
                        drv_datapath_read_size_info(line, p_temp);
                    }

                    para_count ++;
                }
                else
                {
                    p_temp = g_fabric_para_pair_t[count].argus;
                    g_fabric_para_pair_t[count].fun_ptr(line, p_temp);
                    para_count ++;
                }

                break;
            }
            count++;
        }
        if ((g_fabric_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->fabric_item[fabric_item].value = value;

            para_count = 0;
            fabric_item = 0;
            value = 0;
        }

        count = 0;
        while(g_calendar_ptr_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_calendar_ptr_para_pair_t[count].para_name, line,
                   kal_strlen(g_calendar_ptr_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_calendar_ptr_para_pair_t[count].argus;

                g_calendar_ptr_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_calendar_ptr_para_pair_t[count].para_name != NULL) &&
             (para_count == 2))
        {
            datapath_info->calendar_ptr_item[calendar_ptr_item].value = value;

            para_count = 0;
            calendar_ptr_item = 0;
            value = 0;
        }

        count = 0;
        while(g_calendar_entry_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_calendar_entry_para_pair_t[count].para_name, line,
                   kal_strlen(g_calendar_entry_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_calendar_entry_para_pair_t[count].argus;

                g_calendar_entry_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_calendar_entry_para_pair_t[count].para_name != NULL) &&
             (para_count == 3))
        {
            if(DRV_CALENDAR_PTR_ITEM_MAC_MUX == calendar_entry_item)
            {
                datapath_info->calendar_entry_item[entry].entry = entry;
                datapath_info->calendar_entry_item[entry].value = value;
            }
            else /*DRV_CALENDAR_PTR_ITEM_NET_TX*/
            {
                datapath_info->calendar_entry_item[entry+52].entry = entry;
                datapath_info->calendar_entry_item[entry+52].value = value;
            }

            para_count = 0;
            calendar_entry_item = 0;
            entry = 0;
            value = 0;
        }
    }

    /*close file*/
    fclose(local_fp);
    local_fp = NULL;

    return DRV_E_NONE;
}
#if 0
int32
drv_datapath_debug_print(drv_datapath_info_t* datapath_info)
{
    uint8 index = 0;

    fprintf(fp, "drv_datapath_debug_print1\n");

    fprintf(fp, "Chip item:\n");
    fprintf(fp, "\tseq:%d\n", datapath_info->chip_item.seq);

    fprintf(fp, "Serdes item:\n");
    for(index=0; index<64; index++)
    {
        fprintf(fp, "\tindex:%d, type:%d, speed:%d, drive strength:%d\n", index, datapath_info->serdes_item[index].type,
                datapath_info->serdes_item[index].speed,datapath_info->serdes_item[index].drv_strength);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print2\n");
    fprintf(fp, "Pll item:\n");
    for(index=0; index<5; index++)
    {
        fprintf(fp, "\tindex:%d, is_used:%d, input:%d, outputa:%d, outputb:%d, cfg1:0x%x, cfg2:0x%x, cfg3:0x%x\n", index,
                datapath_info->pll_item[index].is_used, datapath_info->pll_item[index].input,
                datapath_info->pll_item[index].output_a, datapath_info->pll_item[index].output_b,
                datapath_info->pll_item[index].cfg1, datapath_info->pll_item[index].cfg2,
                datapath_info->pll_item[index].cfg3);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print3\n");
    fprintf(fp, "Ext mem item:\n");
    for(index=0; index<3; index++)
    {
        fprintf(fp, "\tindex:%d, is_used:%d, freq:%d\n", index, datapath_info->ext_mem_item[index].is_used,
                datapath_info->ext_mem_item[index].freq);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print4\n");
    fprintf(fp, "Mdio item:\n");
    for(index=0; index<3; index++)
    {
        fprintf(fp, "\tindex:%d, value:%d\n", index, datapath_info->mdio_item[index].value);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print5\n");
    fprintf(fp, "Led item:\n");
    for(index=0; index<4; index++)
    {
        fprintf(fp, "\tindex:%d, value:%d\n", index, datapath_info->led_item[index].value);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print6\n");
    fprintf(fp, "Misc item:\n");
    for(index=0; index<3; index++)
    {
        fprintf(fp, "\tindex:%d, value:%d\n", index, datapath_info->misc_item[index].value);
    }
    fprintf(fp, "\n");
    fprintf(fp, "drv_datapath_debug_print7\n");
    fprintf(fp, "Fabric item:\n");
    for(index=0; index<5; index++)
    {
        fprintf(fp, "\tindex:%d, value:%d\n", index, datapath_info->fabric_item[index].value);
    }
    fprintf(fp, "\n");
#if 0
    kal_printf("drv_datapath_debug_print8\n");
    kal_printf("Calendar ptr item:\n");
    for(index=0; index<2; index++)
    {
        kal_printf("\tindex:%d, value:%d\n", index, datapath_info->calendar_ptr_item[index].value);
    }
    kal_printf("\n");
    kal_printf("drv_datapath_debug_print9\n");
    kal_printf("Calendar entry item:\n");
    for(index=0; index<228; index++)
    {
        kal_printf("\tindex:%d, entry:%d, value:%d\n", index, datapath_info->calendar_entry_item[index].entry,
                datapath_info->calendar_entry_item[index].value);
    }
    kal_printf("\n");
#endif
    return DRV_E_NONE;
}

static void
_drv_uint32_to_str(uint32 src, char dest[8])
{
    int8 i = 7, j = 0;
    uint32 value, sum;

    kal_memset(dest, 0, 8);
    if (0 == src)
    {
        dest[0] = 48;
        return ;
    }

    sum = src;
    while (sum)
    {
        value = sum % 16;
        if(value >= 10)
        {
            dest[i--] = value - 10 + 97;
        }
        else
        {
            dest[i--] = value + 48;
        }

        if (i < 0)
            break;

        sum = sum/16;
    }

    /*move the string to the front*/
    for (j = 0; j < (7 - i); j++)
        dest[j] = dest[i + j + 1];

    for (; j <= 7; j++)
        dest[j] = 0;
}

static FILE *dst_file;

int32
generate_result(datapath_debug_t* datapath_debug)
{
    char str[9];

    dst_file = fopen("datapath_debug_new.txt", "a+");
    if(NULL == dst_file)
    {
        printf("fail to open dst_file\n");
        return -1;
    }

    fputs("[Chip]=0x", dst_file);
    kal_memset(str, 0, 9);
    _drv_uint32_to_str(datapath_debug->chip_id, str);
    str[8] = '\0';
    fputs(str, dst_file);
    fputs("\n", dst_file);

    fputs("[Addr]=0x", dst_file);
    kal_memset(str, 0, 9);
    _drv_uint32_to_str(datapath_debug->addr, str);
    str[8] = '\0';
    fputs(str, dst_file);
    fputs("\n", dst_file);

    fputs("[Oper]=read\n", dst_file);

    fputs("[Value]=0x", dst_file);
    kal_memset(str, 0, 9);
    _drv_uint32_to_str(datapath_debug->value, str);
    str[8] = '\0';
    fputs(str, dst_file);
    fputs("\n", dst_file);

    fputs("\n", dst_file);

    fclose(dst_file);
    dst_file = NULL;

    return 0;
}

int32
drv_read_datapath_debug(const int8* file_name)
{
    char    string[128];
    char    line[128];
    FILE   *fp = NULL;
    int32   ret;
    uint32  count = 0;
    uint32  para_count = 0;
    void   *p_temp = NULL;
    datapath_debug_t datapath_debug;
    uint32 read_value = 0;

    DRV_PTR_VALID_CHECK(file_name);

    /*OPEN FILE*/
    fp = fopen((char*)file_name, "r");

    if((NULL == fp) || feof(fp))
    {
        return DRV_E_INVALID_PARAM;
    }
    /*parse profile*/
    while(!feof(fp))
    {
        kal_memset(string, 0, sizeof(string));
        fgets(string, 128, fp);

        /*comment line*/
        if('#' == string[0])
            continue;
        /*trim left and right space*/
        kal_memset(line, 0, sizeof(line));

        ret = drv_datapath_string_atrim(line, string);

        if(EMPTY_LINE(line[0]))
        {
            continue;
        }

        count = 0;
        while(g_debug_para_pair_t[count].para_name != NULL)
        {
            if (kal_strncmp(g_debug_para_pair_t[count].para_name, line,
                   kal_strlen(g_debug_para_pair_t[count].para_name)) == 0)
            {
                p_temp = g_debug_para_pair_t[count].argus;

                g_debug_para_pair_t[count].fun_ptr(line, p_temp);
                para_count ++;
                break;
            }
            count++;
        }
        if ((g_debug_para_pair_t[count].para_name != NULL) &&
             (para_count == 4))
        {
            datapath_debug.chip_id = debug_chip_id;
            datapath_debug.addr = debug_addr;
            datapath_debug.oper = debug_oper;
            datapath_debug.value = debug_value;

            #if 0
            kal_printf("chip:%d, addr:0x%x, oper:%d, value:0x%x\n", datapath_debug.chip_id, datapath_debug.addr,
                                                            datapath_debug.oper, datapath_debug.value);

            #else
            DATAPATH_READ_CHIP(datapath_debug.chip_id, datapath_debug.addr, &read_value);
            datapath_debug.value = read_value;
            #endif
            generate_result(&datapath_debug);

            para_count = 0;
            debug_chip_id = 0;
            debug_addr = 0;
            debug_oper = 0;
            debug_value = 0;
        }
    }

    /*close file*/
    fclose(fp);
    fp = NULL;

    return DRV_E_NONE;
}
#endif
/***Parser datapath config file tools end   *************************/

int32
drv_humber_optimize_hss(uint32 chip_id, uint32* drv_strength)
{
    uint32 strength_val, i, value;    
    
    for(i=0; i<16; i++)
    {
        /* for mid, needn't do optimize. */
        if(drv_strength[i] == DRV_SERDES_DRV_STRENGTH_MID)
        {
            continue;
        }
        else if(drv_strength[i] == DRV_SERDES_DRV_STRENGTH_HIGH)
        {
            strength_val = 0x2;
        }
        else if(drv_strength[i] == DRV_SERDES_DRV_STRENGTH_LOW)
        {
            strength_val = 0x0;
        }
        else 
        {
            return DRV_E_INVALID_PARAM;
        }
        value = 0x80000019 + 0x20 * i;
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x12);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value);

        value = 0x80000010 + 0x20 * i;
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, strength_val);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value);
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x3f);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value+0x1);
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x10);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value+0x2);
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x0);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value+0x3);

        value = 0x8000001a + 0x20 * i;
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0xa);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value);

        value = 0x80000018 + 0x20 * i;
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x1);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value);
        DATAPATH_WRITE_CHIP(chip_id, 0xf4, 0x0);
        DATAPATH_WRITE_CHIP(chip_id, 0xf0, value);
    }
    return DRV_E_NONE;
}

int32
drv_humber_init_pll_item(uint32 chip_id, drv_pll_item_type_t pll_item_type, drv_pll_item_t* pll_item)
{
    uint32 addr = 0;
    uint32 value_pll_ctl1 = 0, value_pll_ctl2 = 0, value_pll_ctl3 = 0, value_pll_rst = 0;

    if(pll_item->is_used)
    {
        switch(pll_item_type)
        {
            case DRV_PLL_ITEM_CORE:
            case DRV_PLL_ITEM_HSS4G:
            case DRV_PLL_ITEM_HSS6G:
            case DRV_PLL_ITEM_TABLE:
            case DRV_PLL_ITEM_TCAM:
                value_pll_ctl1 = pll_item->cfg1;
                value_pll_ctl2 = pll_item->cfg2;
                value_pll_ctl3 = pll_item->cfg3;
                value_pll_rst = 0x00000000;
                break;
            default:
                return DRV_E_INVALID_PARAM;
        }
    }
    else
    {
        if((pll_item_type>=DRV_PLL_ITEM_CORE)&&(pll_item_type<DRV_PLL_ITEM_MAX))
        {
            value_pll_rst = 0x00000001;
            addr = 0x00000090 + pll_item_type*0x10;
            DATAPATH_WRITE_CHIP(chip_id, addr, value_pll_rst);

            return DRV_E_NONE;
        }
        else
        {
            return DRV_E_INVALID_PARAM;
        }
    }

    addr = 0x00000094 + pll_item_type*0x10;
    DATAPATH_WRITE_CHIP(chip_id, addr, value_pll_ctl1);

    addr = 0x00000098 + pll_item_type*0x10;
    DATAPATH_WRITE_CHIP(chip_id, addr, value_pll_ctl2);

    addr = 0x0000009c + pll_item_type*0x10;
    DATAPATH_WRITE_CHIP(chip_id, addr, value_pll_ctl3);

    addr = 0x00000090 + pll_item_type*0x10;
    DATAPATH_WRITE_CHIP(chip_id, addr, value_pll_rst);

    return DRV_E_NONE;
}

int32
drv_humber_clk_enable(uint32 chip_id)
{
    /* # 1. Configure all of modules enable clock and globalEnClk and xgmac0RegClkEn*/
    /* #	 configure globalEnClk and xgmac0RegClkEn*/
    /*GlobalGatedClkCtl (Offset: 0x000004e4)*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000004e4, 0x00000101); 
    /* # 2. Release ResetSupRegDecode**/
    /* #	 [27]-->resetSupRegDecodeIpeEpeGrp*/
    /* #	 [26]-->resetSupRegDecodeShareGrp*/
    /* #	 [25]-->resetSupRegDecodeBsrGrp*/
    /* #	 [24]-->resetSupRegDecodeGmacGrp*/
    /* #	 [23]-->resetSupRegDecodeNetGrp*/
    /* #	 [22]-->resetSupRegDecodeFabricGrp*/
    /* #	 [21]-->resetSupRegDecodeXgmacGrp*/
    /* #	 [20]-->resetSupRegDecodeCore*/
    /*ResetIntRelated(offset:0x00000020)*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000030, 0x000fffff);

    return DRV_E_NONE;
}

int32
drv_humber_clk_disable(uint32 chip_id)
{
    /* # 1. Disable all of modules enable clock and globalEnClk and xgmac0RegClkEn*/
    /* #	 configure globalEnClk and xgmac0RegClkEn*/
    /*GlobalGatedClkCtl (Offset: 0x000004e4)*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000004e4, 0x00000000); 
    /* # Disable all of modules enable clock*/
    /*ModuleGatedClkCtl (Offset: 0x000004f0)*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f0, 0x00000000); 
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f4, 0x00000000); 
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f8, 0x00000010); //expect TcamMacroInt
    
    return DRV_E_NONE;
}


int32
drv_humber_init_ntsg_serdes_item(uint32 chip_id, drv_datapath_info_t* datapath_info)
{
    uint8 hss_macro_idx, lindex;
    uint32 addr,value, value_rx, value_tx;
    drv_serdes_item_t* serdes_item;
    
    for(hss_macro_idx=0; hss_macro_idx<6; hss_macro_idx++)
    {
        /*1. init Hss4G Macro */
        if(datapath_master.hss_en[hss_macro_idx]==0)
        {
            /* 1.1 Cfg Hss4g PdwnPLL to 1 */
            addr = 0x00000184 + hss_macro_idx*0x80;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x10000000);
            continue;
        }
        else
        {
            /* 1.1 Cfg Hss4G PdwnPLL to 0*/
            addr = 0x00000184 + hss_macro_idx*0x80;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
            /* 1.2 Cfg Hss4G link Rx/Tx pwrDwn 0, because reset need all link pwrDwn to 0*/
            for(lindex=0; lindex<8; lindex++)
            {
                addr = 0x0000018c + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, 0);                
                addr = 0x000001ac + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, 0);
            }
            /* 1.3 Release Hss4G reset*/
            addr = 0x00000180 + hss_macro_idx*0x80;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
            /* 1.4 Set Hss4G reset */
            addr = 0x00000180 + hss_macro_idx*0x80;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x1);
        }
        /*2. load serdes config */
        value_rx = 0x00022450;
        value_tx = 0x00070354;
        for(lindex=0; lindex<8; lindex++)
        {
            serdes_item = &(datapath_info->serdes_item[hss_macro_idx*8+lindex]);
            if(serdes_item->type != DRV_SERDES_TYPE_NULL)
            {
                if(serdes_item->speed == DRV_SERDES_SPPED_1DOT25G)
                {
                    value_rx = 0x00022450;
                    value_tx = 0x00070354;
                    break;
                }
                else if(serdes_item->speed == DRV_SERDES_SPPED_3DOT125G)
                {
                    value_rx = 0x000224c0;
                    value_tx = 0x000703c4;
                    break;
                }
                else
                {
                    return DRV_E_INVALID_PARAM;
                }
            }
        }
        for(lindex=0; lindex<8; lindex++)
        {
            serdes_item = &(datapath_info->serdes_item[hss_macro_idx*8+lindex]);
            if(serdes_item->type != DRV_SERDES_TYPE_NULL)
            {
                switch(serdes_item->drv_strength)
                {
                    case DRV_SERDES_DRV_STRENGTH_HIGH:
                        if(serdes_item->speed == DRV_SERDES_SPPED_1DOT25G)
                        {
                            value = 0x00000102;
                            value_rx = 0x00022450;
                            value_tx = 0x00070354;
                        }
                        else if(serdes_item->speed == DRV_SERDES_SPPED_3DOT125G)
                        {
                            value = 0x00000103;
                            value_rx = 0x000224c0;
                            value_tx = 0x000703c4;
                        }
                        else
                        {
                            return DRV_E_INVALID_PARAM;
                        }
                        break;
                    case DRV_SERDES_DRV_STRENGTH_MID:
                        if(serdes_item->speed == DRV_SERDES_SPPED_1DOT25G)
                        {
                            value = 0x00000102;
                            value_rx = 0x00022450;
                            value_tx = 0x00040354;
                        }
                        else if(serdes_item->speed == DRV_SERDES_SPPED_3DOT125G)
                        {
                            value = 0x00000103;
                            value_rx = 0x000224c0;
                            value_tx = 0x000403c4;
                        }
                        else
                        {
                            return DRV_E_INVALID_PARAM;
                        }
                        break;
                    case DRV_SERDES_DRV_STRENGTH_LOW:
                        if(serdes_item->speed == DRV_SERDES_SPPED_1DOT25G)
                        {
                            value = 0x00000102;
                            value_rx = 0x00022450;
                            value_tx = 0x00010354;
                        }
                        else if(serdes_item->speed == DRV_SERDES_SPPED_3DOT125G)
                        {
                            value = 0x00000103;
                            value_rx = 0x000224c0;
                            value_tx = 0x000103c4;
                        }
                        else
                        {
                            return DRV_E_INVALID_PARAM;
                        }
                        break;
                    default:
                        return DRV_E_INVALID_PARAM;                
                }
                /*write data*/
                addr = 0x00000184 + hss_macro_idx*0x80;
                DATAPATH_WRITE_CHIP(chip_id, addr, value);            
                /*rx port*/
                addr = 0x0000018c + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, value_rx);
                /*tx port*/
                addr = 0x000001ac + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, value_tx);
            }
            else /*DRV_SERDES_TYPE_NULL, only power up for reset.*/
            {
                /*For NA link, value_rx and value_tx use other link configure*/
                addr = 0x0000018c + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, value_rx);
                
                addr = 0x000001ac + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_WRITE_CHIP(chip_id, addr, value_tx);
            }
        }
        /*3. Release Hss4G reset*/
        addr = 0x00000180 + hss_macro_idx*0x80;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    
    return DRV_E_NONE;
}

int32
drv_humber_check_ntsg_serdes(uint32 chip_id)
{
    int32 hss_macro_idx;
    uint32 addr,value;
    
    for(hss_macro_idx=0; hss_macro_idx<6; hss_macro_idx++)
    {
        if(datapath_master.hss_en[hss_macro_idx]==1)
        {
            addr = 0x00000130 + hss_macro_idx*0x8;
            DATAPATH_READ_CHIP(chip_id, addr, &value);
            if((value & 0x2000) != 0x2000)
                return DRV_E_DATAPATH_CHECK_HSS_READY_FAIL;
        }
    }
    return DRV_E_NONE;
}

int32
drv_humber_pwdn_ntsg_serdes_link(uint32 chip_id)
{
    uint8 hss_macro_idx, lindex;
    uint32 addr,value;
    /*Old humber version not support powerdown serdes*/
    if(drv_humber_old_version())
    {
        return 0;
    }
    for(hss_macro_idx=0; hss_macro_idx<6; hss_macro_idx++)
    {
        /*If Hss4G used, power down all link in datapath, this link will be power up when port enable*/
        if(datapath_master.hss_en[hss_macro_idx]==1)        
        {
            for(lindex=0; lindex<8; lindex++)
            {
                /* power down Rx link*/
                addr = 0x0000018c + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_READ_CHIP(chip_id, addr, &value);
                value |= 0x1;
                DATAPATH_WRITE_CHIP(chip_id, addr, value);
                
                /*If cpumac use internal refclk, serdes 40 Tx link shouldn't be power down always.*/
                if(drv_humber_cpumac_use_internal_refclk() && ((hss_macro_idx*8+lindex)==DRV_CPUMAC_CLK_SERDES))
                {
                    continue;
                }                
                /* power down Tx link*/
                addr = 0x000001ac + hss_macro_idx*0x80 + lindex*4;
                DATAPATH_READ_CHIP(chip_id, addr, &value);
                value |= 0x1;                
                DATAPATH_WRITE_CHIP(chip_id, addr, value);
            }
        }
    }   
    return DRV_E_NONE;
}

/* For 3.125G */
int32
drv_humber_config_Hss6G_3dot125G(uint32 chip_id, uint32 upsg_bitmap, drv_pll_item_t* pll_item)
{
    /*This update for 625M clock output */
    if(pll_item->output_a == 625)
    {
        /*Config Hss6G Rx -- DivSel*/
        DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000021);
        /*Config Hss6G Tx -- Ts config to 1'b1 and DivSel*/
        DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000001);
    }
    else
    {
        /*Config Hss6G Rx -- DivSel*/
        DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000020);
        /*Config Hss6G Tx -- Ts config to 1'b1 and DivSel*/
        DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000000);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x0000011c, upsg_bitmap);
    /*release Hss6GTx and Hss6GRx reset*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000120, 0x00000000);
    kal_task_sleep(3);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000710);
    /*RX port Rotator Offset Compensition set to 0,when no-DFEmode*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010243);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000008);
    /*signal detect level*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010247);
    return DRV_E_NONE;
}

/* For 3.75G and 4.75G */
int32
drv_humber_config_Hss6G_3dot75G(uint32 chip_id, uint32 upsg_bitmap)
{
    /*Config Hss6G Rx -- DivSel->8x*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000021);
    /*Config Hss6G Tx -- Ts config to 1'b1 and DivSel*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0000011c, upsg_bitmap);
    /*release Hss6GTx and Hss6GRx reset*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000120, 0x00000000);
    kal_task_sleep(3);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000710);
    /*RX port Rotator Offset Compensition set to 0,when no-DFEmode*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010243);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000008);
    /*signal detect level*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010247);            
    /*config 2/3 PLL out rate*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000003);
    /*TX LoFreq overrid register*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x8000020C);
    /*RX LoFreq overrid register*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x8001020C);
    return DRV_E_NONE;
}

/* For 5G, current not adjust. */
int32
drv_humber_config_Hss6G_5G(uint32 chip_id, uint32 upsg_bitmap)
{
    DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000020);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0000011c, upsg_bitmap);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000120, 0x00000000);
    kal_task_sleep(3);
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000710);
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010243);
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000008);
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010247);
    return DRV_E_NONE;
}

/* For 6.25G */
int32
drv_humber_config_Hss6G_6dot25G(uint32 chip_id, uint32 upsg_bitmap)
{
    /*Config Hss6G Rx -- DivSel*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000020);
    /*Config Hss6G Tx -- Ts config to 1'b1 and DivSel*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0000011c, upsg_bitmap);
    /*release Hss6GTx and Hss6GRx reset*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000120, 0x00000000);
    kal_task_sleep(3);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000008);
    /*signal detect level*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010247);
    return DRV_E_NONE;
}

/* For 1.56G */
int32
drv_humber_config_Hss6G_1dot56G(uint32 chip_id, uint32 upsg_bitmap)
{
    /*Config Hss6G Rx -- DivSel*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000124, 0x00000020);
    /*Config Hss6G Tx -- Ts config to 1'b1 and DivSel*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000114, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0000011c, upsg_bitmap);
    /*release Hss6GTx and Hss6GRx reset*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00000110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000120, 0x00000000);
    kal_task_sleep(3);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000710);
    /*RX port Rotator Offset Compensition set to 0,when no-DFEmode*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010243);
    /*HssWriteData*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x00000008);
    /*signal detect level*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010247);
    return DRV_E_NONE;
}

int32
drv_humber_config_bus_width_for_relate_link(uint32 chip_id, uint32 upsg_en)
{
    uint8 index;
    for(index=0; index<16; index++)
    {
        if((1<<index)&upsg_en)
        {
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000000+0x20*index);
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010000+0x20*index);  
        }
    }
    return DRV_E_NONE;
}

int32
drv_humber_init_upsg_serdes_item(uint32 chip_id, uint32* upsg_en, uint32* speed, uint32* strength, drv_pll_item_t* pll_item)
{
    int32 ret=DRV_E_NONE, index;
    uint32 upsg_bitmap, local_speed, local_strength[16];

    upsg_bitmap = upsg_en[0] | upsg_en[1];
    
    /* two type of upsg: SG and Fabric, two type of speed;
       supportted speed group include: 3.125 and 6.25; 1.56 and 3.125 */
    if(((upsg_en[0]!=0) && (upsg_en[1]!=0)) && (speed[0] != speed[1]))
    {
        if((speed[0] == DRV_SERDES_SPPED_3DOT125G)&&(speed[1] == DRV_SERDES_SPPED_6DOT25G))
        {
            /*This 3.125G and 6.25G also use 3.125G cfg. */
            ret = drv_humber_config_Hss6G_3dot125G(chip_id, upsg_bitmap, pll_item);
            /*change bus width to 20bits in half rate for related link */
            /*cfg 3.125G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[0]);
            /*cfg 6.25G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000000C);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[1]);
        }
        else if((speed[0] == DRV_SERDES_SPPED_6DOT25G)&&(speed[1] == DRV_SERDES_SPPED_3DOT125G))
        {
            /*This 3.125G and 6.25G also use 3.125G cfg. */
            ret += drv_humber_config_Hss6G_3dot125G(chip_id, upsg_bitmap, pll_item);
            /*change bus width to 20bits in half rate for related link */
            /*cfg 3.125G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[1]);
            /*cfg 6.25G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000000C);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[0]);
        }
        else if((speed[0] == DRV_SERDES_SPPED_1DOT56G)&&(speed[1] == DRV_SERDES_SPPED_3DOT125G))
        {
            /*This 1.56G and 3.125G also use 1.56 cfg. */
            ret += drv_humber_config_Hss6G_1dot56G(chip_id, upsg_bitmap);
            /*change bus width to 20bits in half rate for related link */
            /*cfg 1.56G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002E);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[0]);
            /*cfg 3.125G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[1]);
        }
        else if((speed[0] == DRV_SERDES_SPPED_3DOT125G)&&(speed[1] == DRV_SERDES_SPPED_1DOT56G))
        {
            /*This 1.56G and 3.125G also use 1.56 cfg. */
            ret += drv_humber_config_Hss6G_1dot56G(chip_id, upsg_bitmap);
            /*change bus width to 20bits in half rate for related link */
            /*cfg 1.56G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002E);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[1]);
            /*cfg 3.125G */
            DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
            ret += drv_humber_config_bus_width_for_relate_link(chip_id, upsg_en[0]);
        }
        else
        {
            return DRV_E_DATAPATH_UPSG_SPEED_ERROR;
        }
    }
    else /* two type of upsg use same speed or only one type of upsg. */
    {
        if(upsg_en[0]!=0) /* SG */
        {
            local_speed = speed[0];
        }
        else /* Fabric */
        {
            local_speed = speed[1];
        }
        switch(local_speed)
        {
            case DRV_SERDES_SPPED_3DOT125G:
                ret = drv_humber_config_Hss6G_3dot125G(chip_id, upsg_bitmap, pll_item);
                /*change bus width to 20bits in half rate*/
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000220);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010240); 
                break;
            case DRV_SERDES_SPPED_3DOT75G:
            case DRV_SERDES_SPPED_4DOT0625G:
                ret = drv_humber_config_Hss6G_3dot75G(chip_id, upsg_bitmap);
                /*change bus width to 20bits in half rate*/
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002C);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000220);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010240);
                break;
            case DRV_SERDES_SPPED_5G:
                ret = drv_humber_config_Hss6G_5G(chip_id, upsg_bitmap);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002D);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000220);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010240);
                break;
            case DRV_SERDES_SPPED_6DOT25G:
                ret = drv_humber_config_Hss6G_6dot25G(chip_id, upsg_bitmap);
                /*change bus width to 20bits in half rate*/
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000000C);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000220);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010240);
                break;
            case DRV_SERDES_SPPED_1DOT56G:
                ret = drv_humber_config_Hss6G_1dot56G(chip_id, upsg_bitmap);
                /*change bus width to 20bits in half rate*/
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F4, 0x0000002E);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80000220);
                DATAPATH_WRITE_CHIP(chip_id, 0x000000F0, 0x80010240);
                break;
            default:
                return DRV_E_INVALID_PARAM;    
        }
    }    
    if(ret != DRV_E_NONE)
        return ret;
    for(index=0; index<16; index++)
    {
        if((1<<index)&(upsg_en[0]))
            local_strength[index] = strength[0];
        else if((1<<index)&(upsg_en[1]))
            local_strength[index] = strength[1];
        else
            local_strength[index] = DRV_SERDES_DRV_STRENGTH_MID;
    }
    ret = drv_humber_optimize_hss(chip_id, local_strength);
    if(ret != DRV_E_NONE)
        return ret;
    return DRV_E_NONE;
}

int32
drv_humber_init_hss(uint32 chip_id)
{
    uint8 index = 0;
    uint32 value = 0;
    
    /* #1. Configure cfgSgmacUse4GHss, cfgSelectFabric0..3, cfgSelectXgmac0..3*/
    /*XgmacSelectCtl*/
    for(index=0; index<DRV_MAX_XGMAC_NUM; index++)
    {
        if(datapath_master.xgmac_en[index])
        {
            value |= 1<<index;
        }
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x000004b0, value); 
    DATAPATH_WRITE_CHIP(chip_id, 0x000004b4, ~value); 
    
    /*FabricSelectCtl*/
    if(datapath_master.fabric_intf_en) /*Fabric*/
    {
        value = 0;
        for(index=0; index<16; index++)
        {
            if(datapath_master.fabric_en[index])
            {
                value |= 1<<(index/4);
            }
        }
        DATAPATH_WRITE_CHIP(chip_id, 0x000004b8, value); 
        DATAPATH_WRITE_CHIP(chip_id, 0x000004bc, value); 

        /*SgmacUse4GCoreCtl*/
//        DATAPATH_WRITE_CHIP(chip_id, 0x000004e0, 0x0000ffff);
    }
    
    if(datapath_master.is_hss4g)/*Sgmac use 4G*/
    {
        value = 0;
        for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
        {
            if(datapath_master.sgmac_en[index])
            {
                value |= 1<<index;
            }
        }
        DATAPATH_WRITE_CHIP(chip_id, 0x000004b8, value); 
        DATAPATH_WRITE_CHIP(chip_id, 0x000004bc, ~value); 

        /*SgmacUse4GCoreCtl*/
//        DATAPATH_WRITE_CHIP(chip_id, 0x000004e0, 0x00010000);
    }
    else /*Sgmac use 6G or not use sgmac*/
    {
        value = 0;
        for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
        {
            if(datapath_master.sgmac_en[index])
            {
                value |= 1<<index;
            }
        }
        if(value != 0)
        {
            DATAPATH_WRITE_CHIP(chip_id, 0x000004b8, ~value);
            DATAPATH_WRITE_CHIP(chip_id, 0x000004bc, ~value);
        }

        /*SgmacUse4GCoreCtl*/
//        DATAPATH_WRITE_CHIP(chip_id, 0x000004e0, 0x00000000);
    }

    if(datapath_master.is_hss4g)/*Sgmac use 4G*/
    {
        /*SgmacUse4GCoreCtl*/
        DATAPATH_WRITE_CHIP(chip_id, 0x000004e0, 0x00010000);
    }
    else /*sgmac not use 4G ,maybe use 6G, maybe use fabric*/
    {
        value = 0;    
        for(index=0; index<16; index++)
        {
            if(datapath_master.fabric_en[index])
            {
                value |= 1<<(index/4);
                value |= 1<<(index/4+8);
            }
        }
        /*SgmacUse4GCoreCtl*/
        DATAPATH_WRITE_CHIP(chip_id, 0x000004e0, value);        
    }
   
    /*GlobalGatedClkCtl*/
    //DATAPATH_WRITE_CHIP(chip_id, 0x000004e4, 0x00000000);

    /*ModuleGatedClkCtl*/
    /*enClkSupGmacWrapper*/
    value = 0;
    for(index=0; index<32; index++)
    {
        if(datapath_master.gmac_en[index])
        {
            value |= 1<<index;
        }
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f0, value);

    value = 0;
    /*enClkSupGmacWrapper*/
    for(index=0; index<16; index++)
    {
        if(datapath_master.gmac_en[index+32])
        {
            value |= 1<<index;
        }
    }

    /*enClkSupQuadMacApp*/
    for(index=0; index<DRV_MAX_QMAC_NUM; index++)
    {
        if(datapath_master.qmac_en[index])
        {
            value |= 1<<(index+16);
        }
    }

    /*enClkSupSgmac*/
    for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
    {
        if(datapath_master.sgmac_en[index])
        {
            value |= 1<<(index+28);
        }
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f4, value);

    value = 0;
    if(datapath_master.fabric_intf_en) /*fabric enable*/
    {
        value = 0x1ff0;
    }
    else
    {
        value = 0x10;
    }

    /*enClkSupXgmac*/
    for(index=0; index<DRV_MAX_XGMAC_NUM; index++)
    {
        if(datapath_master.xgmac_en[index])
        {
            value |= 1<<index;
        }
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x000004f8, value);
    /*ResetIntRelated*/
       value = 0;
       for(index=0; index<DRV_MAX_QMAC_NUM; index++) /*resetSupQuadMacApp*/
       {
           if(!datapath_master.qmac_en[index])
           {
               value |= 1<<index;
           }
       }
       for(index=0; index<DRV_MAX_XGMAC_NUM; index++) /*resetSupXgMac*/
       {
           if(!datapath_master.xgmac_en[index])
           {
               value |= 1<<(index+16);
           }
       }
       for(index=0; index<DRV_MAX_SGMAC_NUM; index++) /*resetSupSgMac*/
       {
           if(!datapath_master.sgmac_en[index])
           {
               value |= 1<<(index+20);
           }
       }
       
       if(!datapath_master.fabric_intf_en) /*fabric disbale*/ /*resetSupFabricInterface*/
       {
           value |= 1<<30;
       }
       DATAPATH_WRITE_CHIP(chip_id, 0x0000002c, value);
    
       value = 0;
       if(!datapath_master.ext_ddr_en)
       {
           value |= 1<<15;  /*resetSupExtDdrCtl */
       }
       if(!datapath_master.ext_qdr_en)
       {
           value |= 1<<16;  /*resetSupQdrCtl */
       }
       if(!datapath_master.ext_tcam_en)
       {
           value |= 1<<14;  /*resetSupTcamCtlExt */
       }
       DATAPATH_WRITE_CHIP(chip_id, 0x00000030, value);

    return DRV_E_NONE;
}


int32
drv_humber_reset_gmac_item(uint32 chip_id, uint8 is_reset, uint8 is_reg_reset)
{
    uint8 index = 0;
    uint32 value0_15 = 0, value16_31 = 0, value32_47 = 0;
    uint8 offset = 0;

    if(is_reg_reset) /*resetSupGmacReg*/
    {
        offset = 2;
    }
    else /*resetSupGmac*/
    {
        offset = 1;
    }

    DATAPATH_READ_CHIP(chip_id, 0x00000020, &value0_15);
    DATAPATH_READ_CHIP(chip_id, 0x00000024, &value16_31);
    DATAPATH_READ_CHIP(chip_id, 0x00000028, &value32_47);
    for(index=DRV_SERDES_ITEM_NTSG0; index<=DRV_SERDES_ITEM_NTSG47; index++)
    {
        if(is_reset)
        {
            if(index <= DRV_SERDES_ITEM_NTSG15)
            {
                if(datapath_master.gmac_en[index])
                {
                    value0_15 |= offset<<(index*2);
                }
            }
            else if(index <= DRV_SERDES_ITEM_NTSG31)
            {
                if(datapath_master.gmac_en[index])
                {
                    value16_31 |= offset<<((index-DRV_SERDES_ITEM_NTSG16)*2);
                }
            }
            else /*DRV_SERDES_ITEM_NSG32 <= index <= DRV_SERDES_ITEM_NSG47*/
            {
                if(datapath_master.gmac_en[index])
                {
                    value32_47 |= offset<<((index-DRV_SERDES_ITEM_NTSG32)*2);
                }
            }
        }
        else
        {
            if(index <= DRV_SERDES_ITEM_NTSG15)
            {
                if(datapath_master.gmac_en[index])
                {
                    value0_15 &= ~(offset<<(index*2));
                }
            }
            else if(index <= DRV_SERDES_ITEM_NTSG31)
            {
                if(datapath_master.gmac_en[index])
                {
                    value16_31 &= ~(offset<<((index-DRV_SERDES_ITEM_NTSG16)*2));
                }
            }
            else /*DRV_SERDES_ITEM_NSG32 <= index <= DRV_SERDES_ITEM_NSG47*/
            {
                if(datapath_master.gmac_en[index])
                {
                    value32_47 &= ~(offset<<((index-DRV_SERDES_ITEM_NTSG32)*2));
                }
            }
        }
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x00000020, value0_15);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000024, value16_31);
    DATAPATH_WRITE_CHIP(chip_id, 0x00000028, value32_47);

    return DRV_E_NONE;
}

int32
drv_humber_init_gmac_item(uint32 chip_id, drv_serdes_item_type_t serdes_item_type)
{
    /*cfg sgmiiEndianModeCfg and sigDetActiveValue*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00010210+serdes_item_type*0x10000, 0x00000003); 

    /*default disable PTP*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00010314+serdes_item_type*0x10000, 0x00000000);

    /* Min packet len is 64 byte */
    DATAPATH_WRITE_CHIP(chip_id, 0x10010 + serdes_item_type*0x10000, 0x40);
    return DRV_E_NONE;
}

int32
drv_humber_init_xgmac_item(uint32 chip_id, drv_serdes_item_type_t serdes_item_type)
{
    /*#1, enable xgmac*/
    /*DRV_SERDES_ITEM_UPSG0~DRV_SERDES_ITEM_UPSG15*/
    /*XgmacConfig4*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004a000c+(serdes_item_type/4)*0x10000, 0x52c00713);

    /*XgmacStatsInit*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004a002c+(serdes_item_type/4)*0x10000, 0x00000001);
    /*default disable PTP*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004a02cc+(serdes_item_type/4)*0x10000, 0x00000000);

    return DRV_E_NONE;
}

int32
drv_humber_init_sgmac_item(uint32 chip_id, drv_serdes_item_type_t serdes_item_type)
{
    uint32 offset = 0;

    /*#1, enable sgmac*/
    if((serdes_item_type>=DRV_SERDES_ITEM_NTSG16)&&(serdes_item_type<=DRV_SERDES_ITEM_NTSG31))
    {
        offset = serdes_item_type - DRV_SERDES_ITEM_NTSG16;
    }
    else /*type:DRV_SERDES_ITEM_UPSG0~DRV_SERDES_ITEM_UPSG15*/
    {
        offset = serdes_item_type - DRV_SERDES_ITEM_UPSG0;
    }

    /*SgmacConfig4*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004e000c+(offset/4)*0x10000, 0x52c60713);

    /*SgmacStatsInit*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004e002c+(offset/4)*0x10000, 0x00000001);
    /*default disable PTP*/
    DATAPATH_WRITE_CHIP(chip_id, 0x004e0300+(offset/4)*0x10000, 0x00000000);


    return DRV_E_NONE;
}

int32
drv_humber_init_mac_item(uint32 chip_id, drv_serdes_item_type_t serdes_item_type, drv_serdes_item_t* mac_item)
{
    switch(mac_item->type)
    {
        case DRV_SERDES_TYPE_GE:
            drv_humber_init_gmac_item(chip_id, serdes_item_type);
            break;
        case DRV_SERDES_TYPE_XG:
            drv_humber_init_xgmac_item(chip_id, serdes_item_type);
            break;
        case DRV_SERDES_TYPE_SG:
            drv_humber_init_sgmac_item(chip_id, serdes_item_type);
            break;
        case DRV_SERDES_TYPE_FB:
        case DRV_SERDES_TYPE_NULL:
            break;
        default:
            return DRV_E_INVALID_PARAM;
    }

    return DRV_E_NONE;
}

int32
drv_humber_init_cpu_mac(uint32 chip_id)
{
    /* #1. reset cpu mac */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450008, 0x00000001); 
    /* #2. set MiscMacClkCtl.cfgCpuMacClkInternal and reset mdio, then release mdio*/
    if(drv_humber_cpumac_use_internal_refclk())
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x000004c0, 0x00110020); 
        DATAPATH_WRITE_CHIP(chip_id, 0x000004c0, 0x00010020);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x000004c0, 0x00100020); 
        DATAPATH_WRITE_CHIP(chip_id, 0x000004c0, 0x00000020);
    }
    /* #3. wait for clock stable, delay 1ms*/
    kal_task_sleep(1);
    /* #4. configure CPU mac speedMode*/
    if(CPUMAC_SPEED_100M == datapath_master.cpu_mac_speed)
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x00450100, 0x00000001); /* #100M */
    }
    else if(CPUMAC_SPEED_1G == datapath_master.cpu_mac_speed)
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x00450100, 0x00000002); /* #1G */ 
    }
    else
    {
        return DRV_E_INVALID_PARAM;
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x00450104, 0x0005e29a); /* Tx disable*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00450108, 0x0000001c); /* Rx disable*/

    return DRV_E_NONE;
}

int32
drv_humber_init_mdio_item(uint32 chip_id, drv_mdio_item_type_t mdio_item_type, drv_normal_item_t* mdio_item)
{
    uint32 value;

    if(mdio_item->value) /* mdio is used*/
    {
        /*release resetSupXgMac*/
        DATAPATH_READ_CHIP(chip_id, 0x0000002c, &value);
        value &= ~(1<<(mdio_item_type+16));
        DATAPATH_WRITE_CHIP(chip_id, 0x0000002c, value);
        /*enable correspond xgmac to enable mdio*/
        DATAPATH_READ_CHIP(chip_id, 0x000004f8, &value);
        value |= (1<<mdio_item_type);
        DATAPATH_WRITE_CHIP(chip_id, 0x000004f8, value);
        /*release xgmac mdio soft reset*/
        DATAPATH_WRITE_CHIP(chip_id, 0x004a0010+mdio_item_type*0x10000, 0x000000f2);
    }
    return DRV_E_NONE;
}

int32
drv_humber_init_led_item(uint32 chip_id, drv_led_item_type_t led_item_type, drv_normal_item_t* led_item)
{
    uint32 addr = 0, value = 0;

    if(led_item->value) /*is used*/
    {
        /*timer*/
    }
    else
    {
        value = 0x00000000;
    }

    addr = 0x000004a0 + led_item_type*4;
    DATAPATH_WRITE_CHIP(chip_id, addr, value);

    return DRV_E_NONE;
}

int32
drv_humber_init_ext_mem_item(chip_reset_cb reset_cb, uint32 chip_id,
                drv_ext_mem_item_type_t ext_mem_item_type, drv_ext_mem_item_t* ext_mem_item)
{
    uint32 value = 0;

    if(DRV_EXT_MEM_ITEM_DDR == ext_mem_item_type)
    {
        if(ext_mem_item->is_used)
        {
            /* Release DDR/QDR/TCAM 1xclock reset, HumberSup.EnableRam1xClkCtl*/
            DATAPATH_READ_CHIP(chip_id, 0x000004c4, &value);
            value &= ~(1 << ext_mem_item_type);
            DATAPATH_WRITE_CHIP(chip_id, 0x000004c4, value);

            /* Configure DDR MPMI*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0000048c, 0x1f133011);

            /* need config DdrDlCtl, then release DLRESET pin,wait until DLlock*/
            DATAPATH_WRITE_CHIP(chip_id, 0x000004d0, 0x18110000);

            /* Release resetCoreExtDdrMacroTx*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0000048c, 0x1f133010);

            kal_task_sleep(50);
            reset_cb(CHIP_RESET_DLLOCK, 0);
            kal_task_sleep(10);
            reset_cb(CHIP_RESET_DLLOCK, 1);
            kal_task_sleep(10);
            reset_cb(CHIP_RESET_DLLOCK, 0);
            kal_task_sleep(10);

            /*check DL lock status*/
            DATAPATH_READ_CHIP(chip_id, 0x000004dc, &value);

            if((value & datapath_master.dl_lock[DRV_EXT_MEM_ITEM_DDR])==0)
            {
                return DRV_E_DATAPATH_CHECK_DL_LOCK_FAIL;
            }

            /* Release resetCoreExtDdrMacroRx*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000488, 0x00000000);

            /* # Release[11]-->resetSupTbInfoArb*/
            /* # Release[15]-->resetSupExtDdrCtl*/
            value = 0;
            DATAPATH_READ_CHIP(chip_id, 0x00000030, &value);
            value &= ~((1<<11)|(1<<15));
            DATAPATH_WRITE_CHIP(chip_id, 0x00000030, value);
        }
    }
    else if(DRV_EXT_MEM_ITEM_QDR == ext_mem_item_type)
    {
        if(ext_mem_item->is_used)
        {
            /* Release DDR/QDR/TCAM 1xclock reset, HumberSup.EnableRam1xClkCtl*/
            DATAPATH_READ_CHIP(chip_id, 0x000004c4, &value);
            value &= ~(1 << ext_mem_item_type);
            DATAPATH_WRITE_CHIP(chip_id, 0x000004c4, value);

            /* Configure QDR MPMI*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000484, 0x1f133011);

            /* need config QdrDlCtl, then release DLRESET pin,wait until DLlock*/
            DATAPATH_WRITE_CHIP(chip_id, 0x000004d4, 0x18110000);

            /* Release resetCoreExtQdrMacroTx*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000484, 0x1f133010);

            kal_task_sleep(50);
            reset_cb(CHIP_RESET_DLLOCK, 0);
            kal_task_sleep(10);
            reset_cb(CHIP_RESET_DLLOCK, 1);
            kal_task_sleep(10);
            reset_cb(CHIP_RESET_DLLOCK, 0);
            kal_task_sleep(10);

            /*check DL lock status*/
            DATAPATH_READ_CHIP(chip_id, 0x000004dc, &value);

            if((value & datapath_master.dl_lock[DRV_EXT_MEM_ITEM_QDR]) == 0)
            {
                return DRV_E_DATAPATH_CHECK_DL_LOCK_FAIL;
            }

            /* Release resetCoreExtQdrMacroRx*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000480, 0x00000000);

            /* # Release[16]-->resetSupQdrCtl*/
            value = 0;
            DATAPATH_READ_CHIP(chip_id, 0x00000030, &value);
            value &= ~(1<<16);
            DATAPATH_WRITE_CHIP(chip_id, 0x00000030, value);

            /* config Qdr work status*/
            /*QdrCtlCfg*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0e000000, 0x20000511);
            DATAPATH_WRITE_CHIP(chip_id, 0x0e000000, 0x20000513);
            DATAPATH_WRITE_CHIP(chip_id, 0x0e000000, 0x20000511);

            /* init Qdr*/
            /*QdrInitCtl*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0e000024, 0x0103ffff);
            kal_task_sleep(10);
        }
    }
    else /*DRV_EXT_MEM_ITEM_TCAM*/
    {
        if(ext_mem_item->is_used)
        {
            /* Release DDR/QDR/TCAM 1xclock reset, HumberSup.EnableRam1xClkCtl*/
            DATAPATH_READ_CHIP(chip_id, 0x000004c4, &value);
            value &= ~(1 << ext_mem_item_type);
            DATAPATH_WRITE_CHIP(chip_id, 0x000004c4, value);
            /* Configure TCAM MPMI*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000494, 0x1f300021);

            /* Release resetCoreExtTcamMacroTx*/
            DATAPATH_WRITE_CHIP(chip_id, 0x00000494, 0x1f300020);
            kal_task_sleep(10);

            /*release TCAM power on reset*/
            reset_cb(CHIP_RESET_TCAM_POWER, 1);
            kal_task_sleep(10);

            /* Release resetCoreExtTcamMacroRx*/
            /* #wait ExtTcam clock back*/
            kal_task_sleep(10);
            DATAPATH_WRITE_CHIP(chip_id, 0x00000490, 0x00000000);

            /* Release [14]-->resetSupTcamCtlExt*/
            value = 0;
            DATAPATH_READ_CHIP(chip_id, 0x00000030, &value);
            value &= ~(1<<14);
            DATAPATH_WRITE_CHIP(chip_id, 0x00000030, value);

            /* Configure TcamCtlExt.TcamCtlExtSetup*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800000, 0x11000431);

            reset_cb(CHIP_RESET_TCAM_CORE, 1);
            kal_task_sleep(10);

            /* Send 100 NOPs*/
            /* #wait ExtTcam clock back*/
            kal_task_sleep(10);

            /* #configure TcamCtlExtBistCtl.cfgNopSendNum to 99*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800074, 0X00006325);

            /* #configure TcamCtlExtAccess.cpuReq and TcamCtlExtAccess.cpuReqType to NOP*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800004, 0x8d000000);

            /* wait NOP send done*/
            /* #polling register TcamCtlExtBistPointer.cfgTrainintDone*/
            kal_task_sleep(10);

            /* close Parity check*/
            /* #write TcamCtlExtWriteData*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800010, 0x00000000);
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800014, 0x00000000);
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800018, 0x00000002);

            /* #write TcamCtlExtWriteMask*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800020, 0x00000000);
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800024, 0x00000000);
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800028, 0x00000000);

            /* #write TcamCtlExtAccess*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800004, 0x84000001);

            /* TcamCtlExt initialize,512K*80bit*/
            /* # TcamCtlExtInitCtrl [19:0] -> cfgInitStartAddr*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800050, 0x00000000);

            /* # [28] -> cfgInitEn;[24]->cfgInitDone;[19:0]->cfgInitEndAddr*/
            DATAPATH_WRITE_CHIP(chip_id, 0x0b800054, 0x1007ffff);
            kal_task_sleep(10);
        }

        /* # Release [13]-->resetSupTcamCtlInt*/
        value = 0;
        DATAPATH_READ_CHIP(chip_id, 0x00000030, &value);
        value &= ~(1<<13);
        DATAPATH_WRITE_CHIP(chip_id, 0x00000030, value);

        /* TcamCtlInt initialize 16K*80bit*/
        /* # TcamCtlIntInitCtrl [13:0] -> cfgInitStartAddr*/
        DATAPATH_WRITE_CHIP(chip_id, 0x0bc00050, 0x00000000);

        /* # [28] -> cfgInitEn;[24]->cfgInitDone;[13:0]->cfgInitEndAddr*/
        DATAPATH_WRITE_CHIP(chip_id, 0x0bc00054, 0x10003fff);
    }

    return DRV_E_NONE;
}

int32
drv_humber_init_fabric_item(uint32 chip_id, drv_fabric_item_t* fabric_info)
{
    uint32 value = 0;

    /*config fabric*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00600044, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x00600034, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x00600010, 0x00000001);

    /*config fabricRts work status*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00610020, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x00610024, 0x00000001);

    /*config fabricCas work status*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00620000, 0x00020200);

    /*config fabricSer work status*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00630034, 0x00000014);
    DATAPATH_WRITE_CHIP(chip_id, 0x00630018, 0x0000FFFF);
    DATAPATH_WRITE_CHIP(chip_id, 0x0063001C, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x006300B4, 0x00000001);
    /*Update depend on ASIC simulation 2012-04-21*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00630070, 0x000008b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x00630074, 0x00000001);

    if(fabric_info->is_master) /*master*/
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0063007C, 0x00000001);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0063007C, 0x00000000);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x00630068, 0x00000020);
    DATAPATH_WRITE_CHIP(chip_id, 0x00630064, 0x00000869);
    DATAPATH_WRITE_CHIP(chip_id, 0x00630060, 0x00000007);
    DATAPATH_WRITE_CHIP(chip_id, 0x00630014, 0x00000008);

    /*fabric Dsf*/
    value = fabric_info->timer1;
    DATAPATH_WRITE_CHIP(chip_id, 0x00640004, value);
    value = fabric_info->timer2;
    DATAPATH_WRITE_CHIP(chip_id, 0x00640008, value);
    value = fabric_info->timer3_expire | (fabric_info->timer3_check<<16);
    DATAPATH_WRITE_CHIP(chip_id, 0x0064000C, value);

    DATAPATH_WRITE_CHIP(chip_id, 0x00640010, 0x00000020);
    DATAPATH_WRITE_CHIP(chip_id, 0x00640014, 0x00000101);

    /*fabric Gts*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00650000, 0x000A0201);
    DATAPATH_WRITE_CHIP(chip_id, 0x00650004, 0x00000001);

    /*fabric Rxq*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00670088, 0x00000005);
    return DRV_E_NONE;
}

int32
drv_humber_init_datapath(uint32 chip_id)
{
    uint32 value = 0;
    uint32 addr;
    uint32 i;

    /*##############################################*/
    /*# OamProc (base address: 0x00a8_0000)*/
    /*##############################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00a80480, 0x1);
    DATAPATH_WRITE_CHIP(chip_id, 0x00a80488, 0x1);
    DATAPATH_WRITE_CHIP(chip_id, 0x00a80490, 0x1);
	/* ##############################################*/
    /* #PtpEngine (base address: 0x0041_0000)*/
    /* ##############################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00410000, 0x00000011); /* #config PtpEngine.ptpEn and PtpEngine.ntpv4En		 */
    /* ##############################################*/
    /* #QuadMacApp (base address: 0x0001_0000)*/
    /* ##############################################*/
    /* #QuadMacApp0..11 init assert*/
    /* #QuadMacApp0..11 stats ram init*/
    for(i=0; i<DRV_MAX_QMAC_NUM; i++)
    {
        if(datapath_master.qmac_en[i])
        {
            DATAPATH_WRITE_CHIP(chip_id, 0x00310024+i*0x10000, 0x00000001); /* #[0]->QuadMacAppXInit */
        }
    }

    /* ##############################################*/
    /* #MacMux (base address: 0x003d_0000)*/
    /* ##############################################*/
    value = 0;
    /*calEntry0~7*/
    for(i=0; i<8; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<(i*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0000, value);

    value = 0;
    /*calEntry8~15*/
    for(i=8; i<16; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-8)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0004, value);

    value = 0;
    /*calEntry16~23*/
    for(i=16; i<24; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-16)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0008, value);

    value = 0;
    /*calEntry24~31*/
    for(i=24; i<32; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-24)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d000c, value);

    value = 0;
    /*calEntry32~39*/
    for(i=32; i<40; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-32)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0010, value);

    value = 0;
    /*calEntry40~47*/
    for(i=40; i<48; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-40)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0014, value);

    value = 0;
    /*calEntry48~51*/
    for(i=48; i<52; i++)
    {
        value |= datapath_master.mac_mux_cal_entry[i]<<((i-48)*4);
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003d0018, value);

    DATAPATH_WRITE_CHIP(chip_id, 0x003d0040, datapath_master.mac_mux_cal_ptr);
    /* #config Calender to 88G mode #default value is ok*/
    /* ##############################################*/
    /* #NetRx (base address: 0x000a_0000)*/
    /* ##############################################*/
    /* #NetRx init after this NetRx works*/
    DATAPATH_WRITE_CHIP(chip_id, 0x003e0018, 0x00000001); /* #NetRxFreeListInit.freeListInit */
    DATAPATH_WRITE_CHIP(chip_id, 0x003e0288, 0x00000001); /* #NetRxChannelInfoCtrl.channelInfoInit */
    /* ##############################################*/
    /* #IPE (base address: 0x0800_0000)*/
    /* ##############################################*/
    /* #IpeHdrAdjust: baseAddr = 0x0800_0000*/
    /* #init the cmpcResWordRam after this the HdrAdjust works.*/
    DATAPATH_WRITE_CHIP(chip_id, 0x08000170, 0x00000001); /* #CmpcResWordRamInit.cmpcResWordRamInit */
    /* #config the net port and ILoop WRR, this should consider the loopBack stream */
    DATAPATH_WRITE_CHIP(chip_id, 0x08000164, 0x00440028); /* #netWrrWeight, lpbkWrrWeight */
    /* #IpePktProcInit init DsBidiPimGroupMem and DsPbbMacTab*/
    DATAPATH_WRITE_CHIP(chip_id, 0x08c0002c, 0x00000003);
    /* ##############################################*/
    /* #IpeIntfMapper:	 baseAddr = 0x0840_0000*/
    /* #IpeIntfMapperInit*/
    DATAPATH_WRITE_CHIP(chip_id, 0x08400028, 0x0000003f); 
    DATAPATH_WRITE_CHIP(chip_id, 0x084000e8, 0x063f0f2f); 
    /* ################################*/
    /* ####Policing*/
    /* ################################*/
    /* #config the IPE/EPE fifo threshold */
    DATAPATH_WRITE_CHIP(chip_id, 0x0047002c, 0x0008000d);
    /* ################################*/
    /* ####Statistics*/
    /* ################################*/
    /* #init statistics ram*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00480004, 0x00000001); /* #initialize */
    /* ###############################*/
    /* #####IpeStats  ###############*/
    /* ###############################*/
    /* #init ram*/
    DATAPATH_WRITE_CHIP(chip_id, 0x09800010, 0x00000001); /* #IpeStatsInitIpeOverallFwd */
    DATAPATH_WRITE_CHIP(chip_id, 0x09800004, 0x00000001); /* #IpeStatsInitIpePhbIntf */
    DATAPATH_WRITE_CHIP(chip_id, 0x09800030, 0x00000001); /* #IpeStatsInitIpePortLog */
    /* ###############################*/
    /* #IpeForward*/
    /* ##DiscardCntRa*/
    /* ###############################*/
    /* #IpeAging (BaseAddr= 0x09400000)*/
    /* #init IpeAging*/
    DATAPATH_WRITE_CHIP(chip_id, 0x09400000, 0x00080002); /* #configure the init data to 0 modify by zhouw for init dsaging all zero */
    DATAPATH_WRITE_CHIP(chip_id, 0x09400028, 0x00000001); /* #IpeAgingInit-->[0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0940001c, 0xffffffff); /* #reset interrupt after ram init */
    /* ##############################################*/
    /* #MetFifo (base address: 0x0a40_0000)*/
    /* ##############################################*/
    /* #MetFifo: WrPtr*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a400050, 0x00000100); /* #MetFifoWrPtr:UCAST_HI_START_PTR[27:16],UCAST_LO_START_PTR[11:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a400054, 0x02000300); /* #MetFifoWrPtr:MCAST_HI_START_PTR[27:16],MCAST_LO_START_PTR[11:0] */
    /* #Metfifo: RdPtr*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a400058, 0x00000100); /* #MetFifoRdPtr:UCAST_HI_RD_PTR[27:16],UCAST_LO_RD_PTR[11:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a40005c, 0x02000300); /* #MetFifoRdPtr:MCAST_HI_Rd_PTR[27:16],MCAST_LO_RD_PTR[11:0] */
    /* #MetFifo: MsgCnt*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a400048, 0x00000000); /* #MetFifoMsgCnt:UCAST_HI_MSG_CNT[28:16],UCAST_LO_MSG_CNT[12:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a40004c, 0x00000000); /* #MetFifoMsgCnt:MCAST_HI_MSG_CNT[28:16],MCAST_LO_MSG_CNT[12:0] */
    /* #MetFifo: MetFifoWrrCtl*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a40001c, 0x08204081); /* #MetFifoWrrCtl:MAX_PENDING_MCAST[28:24],UCAST_HI_WEIGHT[23:18],UCAST_LO_WEIGHT[17:12],MCAST_HI_WEIGHT[11:6],MCAST_LO_WEIGHT[5:0] */
    /* #MetFifo: MetFifoMaxInitCnt*/
    /* #need to initialize the 24K Rcd entries*/
    /* #MetFifo: MetFifoInit*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a400038, 0x00000001); /* #MetFifoInit[0] */
    /* ##############################################*/
    /* #BufStore (base address: 0x0a80_0000)*/
    /* ##############################################*/
    /* #Configure the BufferStoreResrcThreshold*/
    for (i=0; i<256; i++)
    {
        addr = 0x0a806000 + i*8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x00640064);
        addr = 0x0a806000 + i*8 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x00640064);
    }
    /* #condifure the BufStoreStallThreshold*/
    for (i=0; i<64; i++)
    {
        addr = 0x0a80c000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x00500040);
    }
    /* #BufStore: BufStoreFreeListControl0..11*/
    /* #init BufStore linklist pointer*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800080, 0x00000000); /* #BufStoreFreeListControl:FREE_LIST_HEAD_PTR[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800084, 0x00005fff); /* #BufStoreFreeListControl:FREE_LIST_TAIL_PTR[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800088, 0x00026000); /* #BufStoreFreeListControl:minFreeListBufCnt[31:16], */
    /* #Configure BufStoreSharedResrcInfo*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800020, 0x00003000); /* #BufStoreSharedResrcInfo:SHARED_RESRC_THRESHOLD[15:0] */
    /* #BufStore: BufStoreLinkListTableCtrl*/
    /* #init linkList table*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a8000a0, 0x00005fff); /* #BufStoreLinkListTableCtrl: */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a8000a4, 0x00000001); /* #BUFSTORE_LINK_LIST_TABLE_INIT[0],trig */
    /* #BufStore: BufStoreChannelInfoCtrl*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800030, 0x00000001); /* #BUFSTORE_CHANNEL_INFO_INIT[0], */
    /* #BufStore: BufStoreResrcCntCtrl*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800050, 0x00000001); /* #BUFSTORE_RESRCCNTRAMCTRL[0], */
    /* #config the bufStore input wrr*/

    /*need to calculate bandwith*/
    /*BufStoreInputFifoWrrWeight*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801000, datapath_master.fabric_info.bandwidth);/* #inputFifoWrrWeightFabricRxq */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801004, datapath_master.ipe_bandwidth); /* #inputFifoWrrWeightIpe */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801008, datapath_master.eloop_bandwidth); /* #inputFifoWrrWeightELoop */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a80100c, datapath_master.oam_bandwidth); /* #inputFifoWrrWeightOAM */
    /*BufStoreInputFifoWrrWeight*/
#if 0
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801000, 0x000000a0); /* #inputFifoWrrWeightFabricRxq */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801004, 0x00000002); /* #inputFifoWrrWeightIpe */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a801008, 0x00000002); /* #inputFifoWrrWeightELoop */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a80100c, 0x00000001); /* #inputFifoWrrWeightOAM */
#endif

    /* #bufstore resrcMgr disable*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800220, 0x0000001f); /* ####DEBUG */
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800224, 0x000003f1); /* #BUFFSTORE MISC CTRL MAX_BUF_CNT*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0a800228, 0x3ee00021); /* #BUFFSTORE MISC CTRL MAX_FRAME_SIZE*/

    /* ##############################################*/
    /* #QMgr (base address: 0x0600_0000)*/
    /* ##############################################*/
    /* ##############################################*/
    /* #QMgrEnq: BaseAddr= 0x06000000*/
    /* #QMgrQWriteCtl*/
    DATAPATH_WRITE_CHIP(chip_id, 0x06000014, 0x00000001); /* #qMgrEnqInit */
    /* #config QMgrEnq DsHeadHashMod. update 2012-04-19 jqiu for linkagg func.*/    
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c800, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c804, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c808, 0x00000249);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c80c, 0x01111111);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c810, 0x00000492);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c814, 0x02222222);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c818, 0x000000db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c81c, 0x03333333);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c820, 0x00000324);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c824, 0x04444444);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c828, 0x0000042d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c82c, 0x05555555);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c830, 0x00000046);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c834, 0x06666666);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c838, 0x00000288);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c83c, 0x07777777);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c840, 0x000004d1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c844, 0x08888888);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c848, 0x0000011a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c84c, 0x00999999);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c850, 0x00000223);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c854, 0x010aaaaa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c858, 0x0000046c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c85c, 0x0210bbbb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c860, 0x00000085);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c864, 0x03210ccc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c868, 0x000002ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c86c, 0x043210dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c870, 0x00000510);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c874, 0x0543210e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c878, 0x00000019);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c87c, 0x06543210);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c880, 0x00000262);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c884, 0x07654321);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c888, 0x000004ab);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c88c, 0x08765432);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c890, 0x000000c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c894, 0x00876543);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c898, 0x0000030d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c89c, 0x01987654);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8a0, 0x00000416);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8a4, 0x02098765);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8a8, 0x00000058);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8ac, 0x031a9876);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8b0, 0x000002a1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8b4, 0x0420a987);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8b8, 0x000004ea);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8bc, 0x0531ba98);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8c0, 0x00000103);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8c4, 0x06420ba9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8c8, 0x0000020c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8cc, 0x07531cba);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8d0, 0x00000455);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8d4, 0x086420cb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8d8, 0x0000009e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8dc, 0x007531dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8e0, 0x000002e0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8e4, 0x0186420d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8e8, 0x00000529);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8ec, 0x0297531e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8f0, 0x00000002);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8f4, 0x03086420);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8f8, 0x0000024b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c8fc, 0x04197531);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c900, 0x00000494);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c904, 0x052a8642);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c908, 0x000000dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c90c, 0x06309753);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c910, 0x00000326);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c914, 0x0741a864);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c918, 0x00000428);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c91c, 0x0852b975);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c920, 0x00000041);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c924, 0x00630a86);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c928, 0x0000028a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c92c, 0x01741b97);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c930, 0x000004d3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c934, 0x02852ca8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c938, 0x0000011c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c93c, 0x039630b9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c940, 0x00000225);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c944, 0x040741ca);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c948, 0x0000046e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c94c, 0x051852db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c950, 0x00000080);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c954, 0x0629630c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c958, 0x000002c9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c95c, 0x073a741d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c960, 0x00000512);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c964, 0x0840852e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c968, 0x0000001b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c96c, 0x00519630);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c970, 0x00000264);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c974, 0x0162a741);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c978, 0x000004ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c97c, 0x0273b852);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c980, 0x000000c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c984, 0x03840963);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c988, 0x00000308);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c98c, 0x04951a74);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c990, 0x00000411);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c994, 0x05062b85);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c998, 0x0000005a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c99c, 0x06173c96);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9a0, 0x000002a3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9a4, 0x072840a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9a8, 0x000004ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9ac, 0x083951b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9b0, 0x00000105);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9b4, 0x004a62c9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9b8, 0x0000020e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9bc, 0x015073da);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9c0, 0x00000450);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9c4, 0x0261840b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9c8, 0x00000099);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9cc, 0x0372951c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9d0, 0x000002e2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9d4, 0x0483a62d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9d8, 0x0000052b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9dc, 0x0594b73e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9e0, 0x00000004);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9e4, 0x06050840);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9e8, 0x0000024d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9ec, 0x07161951);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9f0, 0x00000496);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9f4, 0x08272a62);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9f8, 0x000000d8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600c9fc, 0x00383b73);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca00, 0x00000321);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca04, 0x01494c84);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca08, 0x0000042a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca0c, 0x025a5095);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca10, 0x00000043);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca14, 0x036061a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca18, 0x0000028c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca1c, 0x047172b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca20, 0x000004d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca24, 0x058283c8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca28, 0x0000011e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca2c, 0x069394d9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca30, 0x00000220);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca34, 0x0704a50a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca38, 0x00000469);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca3c, 0x0815b61b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca40, 0x00000082);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca44, 0x0026072c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca48, 0x000002cb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca4c, 0x0137183d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca50, 0x00000514);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca54, 0x0248294e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca58, 0x0000001d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca5c, 0x03593a50);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca60, 0x00000266);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca64, 0x046a4b61);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca68, 0x000004a8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca6c, 0x05705c72);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca70, 0x000000c1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca74, 0x06816083);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca78, 0x0000030a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca7c, 0x07927194);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca80, 0x00000413);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca84, 0x080382a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca88, 0x0000005c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca8c, 0x001493b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca90, 0x000002a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca94, 0x0125a4c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca98, 0x000004ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ca9c, 0x0236b5d8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caa0, 0x00000100);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caa4, 0x03470609);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caa8, 0x00000209);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caac, 0x0458171a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cab0, 0x00000452);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cab4, 0x0569282b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cab8, 0x0000009b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cabc, 0x067a393c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cac0, 0x000002e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cac4, 0x07804a4d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cac8, 0x0000052d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cacc, 0x08915b5e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cad0, 0x00000006);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cad4, 0x00026c60);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cad8, 0x00000248);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cadc, 0x01137071);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cae0, 0x00000491);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cae4, 0x02248182);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cae8, 0x000000da);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caec, 0x03359293);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caf0, 0x00000323);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caf4, 0x0446a3a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600caf8, 0x0000042c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cafc, 0x0557b4b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb00, 0x00000045);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb04, 0x066805c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb08, 0x0000028e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb0c, 0x077916d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb10, 0x000004d0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb14, 0x088a2708);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb18, 0x00000119);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb1c, 0x00903819);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb20, 0x00000222);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb24, 0x0101492a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb28, 0x0000046b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb2c, 0x02125a3b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb30, 0x00000084);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb34, 0x03236b4c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb38, 0x000002cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb3c, 0x04347c5d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb40, 0x00000516);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb44, 0x0545806e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb48, 0x00000018);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb4c, 0x06569170);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb50, 0x00000261);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb54, 0x0767a281);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb58, 0x000004aa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb5c, 0x0878b392);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb60, 0x000000c3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb64, 0x008904a3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb68, 0x0000030c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb6c, 0x019a15b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb70, 0x00000415);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb74, 0x020026c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb78, 0x0000005e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb7c, 0x031137d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb80, 0x000002a0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb84, 0x04224807);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb88, 0x000004e9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb8c, 0x05335918);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb90, 0x00000102);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb94, 0x06446a29);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb98, 0x0000020b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cb9c, 0x07557b3a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cba0, 0x00000454);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cba4, 0x08668c4b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cba8, 0x0000009d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbac, 0x0077905c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbb0, 0x000002e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbb4, 0x0188a16d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbb8, 0x00000528);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbbc, 0x0299b27e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbc0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbc4, 0x030a0380);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbc8, 0x0000024a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbcc, 0x04101491);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbd0, 0x00000493);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbd4, 0x052125a2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbd8, 0x000000dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbdc, 0x063236b3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbe0, 0x00000325);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbe4, 0x074347c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbe8, 0x0000042e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbec, 0x085458d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbf0, 0x00000040);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbf4, 0x00656906);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbf8, 0x00000289);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cbfc, 0x01767a17);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc00, 0x000004d2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc04, 0x02878b28);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc08, 0x0000011b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc0c, 0x03989c39);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc10, 0x00000224);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc14, 0x0409a04a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc18, 0x0000046d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc1c, 0x051ab15b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc20, 0x00000086);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc24, 0x0620026c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc28, 0x000002c8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc2c, 0x0731137d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc30, 0x00000511);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc34, 0x0842248e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc38, 0x0000001a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc3c, 0x00533590);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc40, 0x00000263);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc44, 0x016446a1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc48, 0x000004ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc4c, 0x027557b2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc50, 0x000000c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc54, 0x038668c3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc58, 0x0000030e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc5c, 0x049779d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc60, 0x00000410);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc64, 0x05088a05);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc68, 0x00000059);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc6c, 0x06199b16);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc70, 0x000002a2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc74, 0x072aac27);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc78, 0x000004eb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc7c, 0x0830b038);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc80, 0x00000104);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc84, 0x00410149);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc88, 0x0000020d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc8c, 0x0152125a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc90, 0x00000456);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc94, 0x0263236b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc98, 0x00000098);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cc9c, 0x0374347c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cca0, 0x000002e1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cca4, 0x0485458d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cca8, 0x0000052a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccac, 0x0596569e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccb0, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccb4, 0x060767a0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccb8, 0x0000024c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccbc, 0x071878b1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccc0, 0x00000495);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccc4, 0x082989c2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccc8, 0x000000de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cccc, 0x003a9ad3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccd0, 0x00000320);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccd4, 0x0140ab04);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccd8, 0x00000429);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccdc, 0x0251bc15);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cce0, 0x00000042);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cce4, 0x03620026);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cce8, 0x0000028b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccec, 0x04731137);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccf0, 0x000004d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccf4, 0x05842248);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccf8, 0x0000011d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ccfc, 0x06953359);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd00, 0x00000226);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd04, 0x0706446a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd08, 0x00000468);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd0c, 0x0817557b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd10, 0x00000081);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd14, 0x0028668c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd18, 0x000002ca);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd1c, 0x0139779d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd20, 0x00000513);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd24, 0x024a88ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd28, 0x0000001c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd2c, 0x035099b0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd30, 0x00000265);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd34, 0x0461aac1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd38, 0x000004ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd3c, 0x0572bbd2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd40, 0x000000c0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd44, 0x06830c03);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd48, 0x00000309);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd4c, 0x07941014);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd50, 0x00000412);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd54, 0x08052125);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd58, 0x0000005b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd5c, 0x00163236);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd60, 0x000002a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd64, 0x01274347);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd68, 0x000004ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd6c, 0x02385458);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd70, 0x00000106);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd74, 0x03496569);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd78, 0x00000208);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd7c, 0x045a767a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd80, 0x00000451);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd84, 0x0560878b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd88, 0x0000009a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd8c, 0x0671989c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd90, 0x000002e3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd94, 0x0782a9ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd98, 0x0000052c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cd9c, 0x0893babe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cda0, 0x00000005);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cda4, 0x00040bc0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cda8, 0x0000024e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdac, 0x01151cd1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdb0, 0x00000490);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdb4, 0x02262002);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdb8, 0x000000d9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdbc, 0x03373113);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdc0, 0x00000322);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdc4, 0x04484224);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdc8, 0x0000042b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdcc, 0x05595335);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdd0, 0x00000044);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdd4, 0x066a6446);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdd8, 0x0000028d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cddc, 0x07707557);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cde0, 0x000004d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cde4, 0x08818668);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cde8, 0x00000118);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdec, 0x00929779);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdf0, 0x00000221);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdf4, 0x0103a88a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdf8, 0x0000046a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cdfc, 0x0214b99b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce00, 0x00000083);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce04, 0x03250aac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce08, 0x000002cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce0c, 0x04361bbd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce10, 0x00000515);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce14, 0x05472cce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce18, 0x0000001e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce1c, 0x065830d0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce20, 0x00000260);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce24, 0x07694101);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce28, 0x000004a9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce2c, 0x087a5212);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce30, 0x000000c2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce34, 0x00806323);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce38, 0x0000030b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce3c, 0x01917434);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce40, 0x00000414);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce44, 0x02028545);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce48, 0x0000005d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce4c, 0x03139656);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce50, 0x000002a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce54, 0x0424a767);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce58, 0x000004e8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce5c, 0x0535b878);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce60, 0x00000101);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce64, 0x06460989);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce68, 0x0000020a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce6c, 0x07571a9a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce70, 0x00000453);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce74, 0x08682bab);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce78, 0x0000009c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce7c, 0x00793cbc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce80, 0x000002e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce84, 0x018a40cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce88, 0x0000052e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce8c, 0x029051de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce94, 0x03016200);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce98, 0x00000249);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ce9c, 0x04127311);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cea0, 0x00000492);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cea4, 0x05238422);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cea8, 0x000000db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ceac, 0x06349533);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ceb0, 0x00000324);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ceb4, 0x0745a644);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ceb8, 0x0000042d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cebc, 0x0856b755);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cec0, 0x00000046);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cec4, 0x00670866);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cec8, 0x00000288);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cecc, 0x01781977);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ced0, 0x000004d1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ced4, 0x02892a88);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ced8, 0x0000011a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cedc, 0x039a3b99);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cee0, 0x00000223);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cee4, 0x04004caa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cee8, 0x0000046c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600ceec, 0x051150bb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cef0, 0x00000085);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cef4, 0x062261cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cef8, 0x000002ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cefc, 0x073372dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf00, 0x00000510);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf04, 0x0844830e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf08, 0x00000019);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf0c, 0x00559410);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf10, 0x00000262);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf14, 0x0166a521);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf18, 0x000004ab);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf1c, 0x0277b632);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf20, 0x000000c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf24, 0x03880743);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf28, 0x0000030d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf2c, 0x04991854);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf30, 0x00000416);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf34, 0x050a2965);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf38, 0x00000058);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf3c, 0x06103a76);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf40, 0x000002a1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf44, 0x07214b87);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf48, 0x000004ea);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf4c, 0x08325c98);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf50, 0x00000103);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf54, 0x004360a9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf58, 0x0000020c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf5c, 0x015471ba);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf60, 0x00000455);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf64, 0x026582cb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf68, 0x0000009e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf6c, 0x037693dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf70, 0x000002e0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf74, 0x0487a40d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf78, 0x00000529);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf7c, 0x0598b51e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf80, 0x00000002);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf84, 0x06090620);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf88, 0x0000024b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf8c, 0x071a1731);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf90, 0x00000494);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf94, 0x08202842);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf98, 0x000000dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cf9c, 0x00313953);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfa0, 0x00000326);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfa4, 0x01424a64);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfa8, 0x00000428);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfac, 0x02535b75);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfb0, 0x00000041);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfb4, 0x03646c86);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfb8, 0x0000028a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfbc, 0x04757097);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfc0, 0x000004d3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfc4, 0x058681a8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfc8, 0x0000011c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfcc, 0x069792b9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfd0, 0x00000225);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfd4, 0x0708a3ca);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfd8, 0x0000046e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfdc, 0x0819b4db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfe0, 0x00000080);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfe4, 0x002a050c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfe8, 0x000002c9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cfec, 0x0130161d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cff0, 0x00000512);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cff4, 0x0241272e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cff8, 0x0000001b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0600cffc, 0x03523830);
     
    /* ##############################################*/
    /* #QMgrLinkList: BaseAddr= 0x06400000*/
    /* #write bay0 0x06400018 0x7ffb	  #the default value is 0x7ffb*/
    DATAPATH_WRITE_CHIP(chip_id, 0x06400020, 0x00000001); /* #qMgrLinkListInit */
    /* ##############################################*/
    /* #QMgrSch: BaseAddr= 0x06800000*/
    /* #Note: DsQueDrrWeight, DsQueMap configured by SW*/
    DATAPATH_WRITE_CHIP(chip_id, 0x06800010, 0x00000001); /* #qMgrSchInit */
    /* ##############################################*/
    /* #QMgrSubCh: BaseAddr= 0x06c00000*/
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00010, 0x00000001); /* #qMgrSubChInit */

    /* Init QMgrInterfaceWrrWeightCfg */
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00080, datapath_master.ipe_bandwidth); /* #networkWtCfg[7:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00084, datapath_master.fabric_info.bandwidth); /* #fabricWtCfg[7:0] */
    value = datapath_master.oam_bandwidth+datapath_master.iloop_bandwidth+datapath_master.cpu_bandwidth;
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00088, value); /* #miscWtCfg[7:0] cpumac/iloop/oam */
    DATAPATH_WRITE_CHIP(chip_id, 0x06c0008c, datapath_master.eloop_bandwidth); /* #eLoopWtCfg[7:0] */
    /* Init QMgrMiscInterfaceWrrWeightCfg*/
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00090, datapath_master.iloop_bandwidth); /* #iLoopWtCfg[7:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00094, datapath_master.cpu_bandwidth); /* #cpuWtCfg[7:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x06c00098, datapath_master.oam_bandwidth); /* #oamWtCfg[7:0] */

    /* #init these credit*/
    /*DsChCredit*/
    for (i=0; i<256; i++) /* #DsCredit for NetWork/CpuMac/Loopback */
    {
        addr = 0x06c0e000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x8);
    }

    for(i=0; i<DRV_MAX_XGMAC_NUM; i++) /*#DsCredit for Xgmac */
    {
        if(datapath_master.xgmac_en[i])
        {
            addr = 0x06c0e000 + i*4*12;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x9);
        }
    }

    for (i=48; i<52; i++) /*#DsCredit for Sgmac */
    {
        if(datapath_master.sgmac_en[i-48])
        {
            addr = 0x06c0e000 + i*4;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x9);
        }
    }

    if(datapath_master.fabric_intf_en) /*fabric enable*/
    {
        for (i=128; i<256; i++) /*#DsCredit for Fabric */
        {
            addr = 0x06c0e000 + i*4;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x7);
        }
    }

    /* ##############################################*/
    /* #BufRetrv (base address: 0x0ac0_0000)*/
    /* ##############################################*/
    /* #bufRetrv credit config */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001e0, 0x00000007); /* #0x10-0x9=0x7 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001e4, 0x0000005b); /* #0x64-0x9=0x5b */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001e8, 0x000000a7); /* #0xb0-0x9=0xa7 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001ec, 0x00000037); /* #0x40-0x9=0x37 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001f0, 0x0000001a); /* #BufRetrvCreditConfig.creditConfig4 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001f4, 0x0000001a); /* #BufRetrvCreditConfig.creditConfig5 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001f8, 0x0000001a); /* #BufRetrvCreditConfig.creditConfig6 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac001fc, 0x0000001a); /* #BufRetrvCreditConfig.creditConfig7 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac00248, 0x00000001); /* #BufRetrvInitCtl.bufInitEn */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac0024c, 0x00000001); /* #BufRetrvInitCtl.creditInitEn */
    /* #config the bufRetrv packetWeight*/
    /*need to calculate the bandwith*/
    /*BufRetrvPktPtrWtConfig*/
    value = (datapath_master.oam_bandwidth <<24)+(datapath_master.fabric_info.bandwidth <<8)+datapath_master.ipe_bandwidth;
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002b8, value); /* #networkPktWeightConfig, */
    value = (datapath_master.sgmac_bandwidth<<16)+datapath_master.gmac_bandwidth;
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002bc, value); /* #networkSGWeightConfig, */
    /*BufRetrvBufPtrWtConfig*/
    value = (datapath_master.oam_bandwidth<<24)+(datapath_master.iloop_bandwidth<<16)
        +(datapath_master.fabric_info.bandwidth<<8)+datapath_master.ipe_bandwidth;
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002c0, value); /* #networkBufWeightConfig, */
    value = (datapath_master.eloop_bandwidth<<16)+(datapath_master.sgmac_bandwidth<<8)+datapath_master.gmac_bandwidth;
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002c4, value); /* #networkBufELoopWeightConfig, */
#if 0
    /*BufRetrvPktPtrWtConfig*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002b8, 0x0100015a); /* #networkPktWeightConfig, */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002bc, 0x00280030); /* #networkSGWeightConfig, */
    /*BufRetrvBufPtrWtConfig*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002c0, 0x0101015a); /* #networkBufWeightConfig, */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac002c4, 0x00012830); /* #networkBufELoopWeightConfig, */
#endif

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac0021c, 0x04040404); /* #cfgOamBurstCnt, */
    /* #########################################*/
    /* ##BufPtr config*/
    /* #fabric interface */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19400, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19404, 0x00080000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c04, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19408, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1940c, 0x00110009);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c0c, 0x00009009);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19410, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19414, 0x001a0012);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c14, 0x00012012);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19418, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1941c, 0x0023001b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c1c, 0x0001b01b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19420, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19424, 0x002c0024);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c24, 0x00024024);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19428, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1942c, 0x0035002d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c2c, 0x0002d02d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19430, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19434, 0x003e0036);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c34, 0x00036036);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19438, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1943c, 0x0047003f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c3c, 0x0003f03f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19440, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19444, 0x00500048);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c44, 0x00048048);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19448, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1944c, 0x00590051);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c4c, 0x00051051);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19450, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19454, 0x0062005a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c54, 0x0005a05a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19458, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1945c, 0x006b0063);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c5c, 0x00063063);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19460, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19464, 0x0074006c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c64, 0x0006c06c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19468, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1946c, 0x007d0075);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c6c, 0x00075075);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19470, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19474, 0x0086007e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c74, 0x0007e07e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19478, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1947c, 0x008e0087);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c7c, 0x00087087);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19480, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19484, 0x0096008f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c84, 0x0008f08f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19488, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1948c, 0x009e0097);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c8c, 0x00097097);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19490, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19494, 0x00a6009f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c94, 0x0009f09f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19498, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1949c, 0x00ae00a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19c9c, 0x000a70a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194a0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194a4, 0x00b600af);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ca0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ca4, 0x000af0af);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194a8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194ac, 0x00be00b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ca8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cac, 0x000b70b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194b0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194b4, 0x00c600bf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cb4, 0x000bf0bf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194b8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194bc, 0x00ce00c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cbc, 0x000c70c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194c0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194c4, 0x00d600cf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cc4, 0x000cf0cf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194c8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194cc, 0x00de00d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ccc, 0x000d70d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194d0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194d4, 0x00e600df);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cd4, 0x000df0df);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194d8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194dc, 0x00ee00e7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cdc, 0x000e70e7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194e0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194e4, 0x00f600ef);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ce0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ce4, 0x000ef0ef);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194e8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194ec, 0x00fe00f7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ce8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cec, 0x000f70f7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194f8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac194fc, 0x010600ff);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cf8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19cfc, 0x000ff0ff);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19500, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19504, 0x010f0107);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d04, 0x00107107);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19508, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1950c, 0x01180110);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d0c, 0x00110110);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19510, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19514, 0x01210119);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d14, 0x00119119);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19518, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1951c, 0x012a0122);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d1c, 0x00122122);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19520, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19524, 0x0133012b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d24, 0x0012b12b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19528, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1952c, 0x013c0134);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d2c, 0x00134134);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19530, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19534, 0x0145013d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d34, 0x0013d13d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19538, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1953c, 0x014e0146);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d3c, 0x00146146);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19540, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19544, 0x0157014f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d44, 0x0014f14f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19548, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1954c, 0x01600158);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d4c, 0x00158158);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19550, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19554, 0x01690161);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d54, 0x00161161);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19558, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1955c, 0x0172016a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d5c, 0x0016a16a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19560, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19564, 0x017b0173);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d64, 0x00173173);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19568, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1956c, 0x0184017c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d6c, 0x0017c17c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19570, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19574, 0x018d0185);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d74, 0x00185185);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19578, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1957c, 0x0195018e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d7c, 0x0018e18e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19580, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19584, 0x019d0196);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d84, 0x00196196);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19588, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1958c, 0x01a5019e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d8c, 0x0019e19e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19590, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19594, 0x01ad01a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d94, 0x001a61a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19598, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1959c, 0x01b501ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19d9c, 0x001ae1ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195a0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195a4, 0x01bd01b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19da0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19da4, 0x001b61b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195a8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195ac, 0x01c501be);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19da8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dac, 0x001be1be);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195b0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195b4, 0x01cd01c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19db0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19db4, 0x001c61c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195b8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195bc, 0x01d501ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19db8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dbc, 0x001ce1ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195c0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195c4, 0x01dd01d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dc4, 0x001d61d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195c8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195cc, 0x01e501de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dcc, 0x001de1de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195d0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195d4, 0x01ed01e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dd4, 0x001e61e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195d8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195dc, 0x01f501ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ddc, 0x001ee1ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195e0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195e4, 0x01fd01f6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19de0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19de4, 0x001f61f6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195e8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195ec, 0x020501fe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19de8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dec, 0x001fe1fe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195f8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac195fc, 0x020d0206);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19df8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19dfc, 0x00206206);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19600, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19604, 0x0216020e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e04, 0x0020e20e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19608, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1960c, 0x021f0217);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e0c, 0x00217217);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19610, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19614, 0x02280220);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e14, 0x00220220);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19618, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1961c, 0x02310229);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e1c, 0x00229229);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19620, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19624, 0x023a0232);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e24, 0x00232232);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19628, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1962c, 0x0243023b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e2c, 0x0023b23b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19630, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19634, 0x024c0244);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e34, 0x00244244);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19638, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1963c, 0x0255024d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e3c, 0x0024d24d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19640, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19644, 0x025e0256);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e44, 0x00256256);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19648, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1964c, 0x0267025f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e4c, 0x0025f25f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19650, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19654, 0x02700268);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e54, 0x00268268);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19658, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1965c, 0x02790271);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e5c, 0x00271271);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19660, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19664, 0x0282027a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e64, 0x0027a27a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19668, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1966c, 0x028b0283);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e6c, 0x00283283);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19670, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19674, 0x0294028c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e74, 0x0028c28c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19678, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1967c, 0x029c0295);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e7c, 0x00295295);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19680, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19684, 0x02a4029d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e84, 0x0029d29d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19688, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1968c, 0x02ac02a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e8c, 0x002a52a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19690, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19694, 0x02b402ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e94, 0x002ad2ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19698, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1969c, 0x02bc02b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19e9c, 0x002b52b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196a0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196a4, 0x02c402bd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ea0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ea4, 0x002bd2bd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196a8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196ac, 0x02cc02c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ea8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19eac, 0x002c52c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196b0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196b4, 0x02d402cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19eb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19eb4, 0x002cd2cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196b8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196bc, 0x02dc02d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19eb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ebc, 0x002d52d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196c0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196c4, 0x02e402dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ec0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ec4, 0x002dd2dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196c8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196cc, 0x02ec02e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ec8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ecc, 0x002e52e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196d0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196d4, 0x02f402ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ed0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ed4, 0x002ed2ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196d8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196dc, 0x02fc02f5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ed8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19edc, 0x002f52f5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196e0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196e4, 0x030402fd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ee0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ee4, 0x002fd2fd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196e8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196ec, 0x030c0305);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ee8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19eec, 0x00305305);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196f8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac196fc, 0x0314030d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ef8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19efc, 0x0030d30d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19700, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19704, 0x031d0315);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f04, 0x00315315);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19708, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1970c, 0x0326031e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f0c, 0x0031e31e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19710, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19714, 0x032f0327);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f14, 0x00327327);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19718, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1971c, 0x03380330);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f1c, 0x00330330);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19720, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19724, 0x03410339);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f24, 0x00339339);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19728, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1972c, 0x034a0342);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f2c, 0x00342342);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19730, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19734, 0x0353034b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f34, 0x0034b34b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19738, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1973c, 0x035c0354);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f3c, 0x00354354);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19740, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19744, 0x0365035d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f44, 0x0035d35d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19748, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1974c, 0x036e0366);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f4c, 0x00366366);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19750, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19754, 0x0377036f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f54, 0x0036f36f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19758, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1975c, 0x03800378);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f5c, 0x00378378);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19760, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19764, 0x03890381);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f64, 0x00381381);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19768, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1976c, 0x0392038a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f6c, 0x0038a38a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19770, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19774, 0x039b0393);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f74, 0x00393393);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19778, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1977c, 0x03a3039c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f7c, 0x0039c39c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19780, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19784, 0x03ab03a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f84, 0x003a43a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19788, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1978c, 0x03b303ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f8c, 0x003ac3ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19790, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19794, 0x03bb03b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f94, 0x003b43b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19798, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1979c, 0x03c303bc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19f9c, 0x003bc3bc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197a0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197a4, 0x03cb03c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fa0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fa4, 0x003c43c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197a8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197ac, 0x03d303cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fa8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fac, 0x003cc3cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197b0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197b4, 0x03db03d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fb4, 0x003d43d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197b8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197bc, 0x03e303dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fbc, 0x003dc3dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197c0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197c4, 0x03eb03e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fc4, 0x003e43e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197c8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197cc, 0x03f303ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fcc, 0x003ec3ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197d0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197d4, 0x03fb03f4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fd4, 0x003f43f4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197d8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197dc, 0x040303fc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fdc, 0x003fc3fc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197e0, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197e4, 0x040b0404);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fe0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fe4, 0x00404404);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197e8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197ec, 0x0413040c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fe8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19fec, 0x0040c40c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197f8, 0x00010001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac197fc, 0x041b0414);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ff8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19ffc, 0x005dc5dc);
    /* #network interface */

    /*BufRetrvBufConfigMem*/
    /*XGMAC0*/
    if(datapath_master.xgmac_en[0])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19000, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19000, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19004, 0x0425041c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19800, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19804, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19008, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1900c, 0x042d0426);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19808, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1980c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19010, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19014, 0x0435042e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19810, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19814, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19018, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1901c, 0x043d0436);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19818, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1981c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19020, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19024, 0x0445043e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19820, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19824, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19028, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1902c, 0x044d0446);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19828, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1982c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19030, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19034, 0x0455044e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19830, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19834, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19038, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1903c, 0x045d0456);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19838, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1983c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19040, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19044, 0x0465045e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19840, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19844, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19048, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1904c, 0x046d0466);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19848, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1984c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19050, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19054, 0x0475046e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19850, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19854, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19058, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1905c, 0x047d0476);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19858, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1985c, 0x00000000);

    /*XGMAC1*/
    if(datapath_master.xgmac_en[1])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19060, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19060, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19064, 0x0487047e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19860, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19864, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19068, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1906c, 0x048f0488);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19868, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1986c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19070, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19074, 0x04970490);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19870, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19874, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19078, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1907c, 0x049f0498);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19878, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1987c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19080, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19084, 0x04a704a0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19880, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19884, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19088, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1908c, 0x04af04a8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19888, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1988c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19090, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19094, 0x04b704b0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19890, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19894, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19098, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1909c, 0x04bf04b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19898, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1989c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190a0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190a4, 0x04c704c0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198a4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190a8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190ac, 0x04cf04c8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198ac, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190b0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190b4, 0x04d704d0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198b4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190b8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190bc, 0x04df04d8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198bc, 0x00000000);

    /*XGMAC2*/
    if(datapath_master.xgmac_en[2])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac190c0, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac190c0, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190c4, 0x04e904e0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198c0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198c4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190c8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190cc, 0x04f104ea);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198c8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198cc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190d0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190d4, 0x04f904f2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198d0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198d4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190d8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190dc, 0x050104fa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198d8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198dc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190e0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190e4, 0x05090502);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198e0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198e4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190e8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190ec, 0x0511050a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198e8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198ec, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190f0, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190f4, 0x05190512);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198f0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198f4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190f8, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac190fc, 0x0521051a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198f8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac198fc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19100, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19104, 0x05290522);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19900, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19904, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19108, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1910c, 0x0531052a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19908, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1990c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19110, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19114, 0x05390532);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19910, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19914, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19118, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1911c, 0x0541053a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19918, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1991c, 0x00000000);

    /*XGMAC3*/
    if(datapath_master.xgmac_en[3])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19120, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19120, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19124, 0x054b0542);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19920, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19924, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19128, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1912c, 0x0553054c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19928, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1992c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19130, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19134, 0x055b0554);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19930, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19934, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19138, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1913c, 0x0563055c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19938, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1993c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19140, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19144, 0x056b0564);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19940, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19944, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19148, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1914c, 0x0573056c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19948, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1994c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19150, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19154, 0x057b0574);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19950, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19954, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19158, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1915c, 0x0583057c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19958, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1995c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19160, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19164, 0x058b0584);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19960, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19964, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19168, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1916c, 0x0593058c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19968, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1996c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19170, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19174, 0x059b0594);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19970, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19974, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19178, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1917c, 0x05a3059c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19978, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1997c, 0x00000000);

    /*SGMAC0*/
    if(datapath_master.sgmac_en[0])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19180, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19180, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19184, 0x05ad05a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19980, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19984, 0x00000000);

    /*SGMAC1*/
    if(datapath_master.sgmac_en[1])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19188, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19188, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1918c, 0x05b705ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19988, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1998c, 0x00000000);

    /*SGMAC2*/
    if(datapath_master.sgmac_en[2])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19190, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19190, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19194, 0x05c105b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19990, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19994, 0x00000000);

    /*SGMAC3*/
    if(datapath_master.sgmac_en[3])
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19198, 0x0000000a);
    }
    else
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x0ac19198, 0x00000001);
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1919c, 0x05cb05c2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac19998, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1999c, 0x00000000);
    /* #iloop, cpu, oam and eloop interface */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191a0, 0x00020028);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191a4, 0x05d305cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199a4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191a8, 0x00030001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191ac, 0x05db05d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199ac, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191b0, 0x00030001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191b4, 0x05e305dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199b4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191b8, 0x00000028);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac191bc, 0x05eb05e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac199bc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f000, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f004, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f008, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f00c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f010, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f014, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f018, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f01c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f020, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f024, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f028, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f02c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f030, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f034, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f038, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f03c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f040, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f044, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f048, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f04c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f050, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f054, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f058, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f05c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f060, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f064, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f068, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f06c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f070, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f074, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f078, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f07c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f080, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f084, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f088, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f08c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f090, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f094, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f098, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f09c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0ac, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0bc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0cc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0dc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0e0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0e4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0e8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0ec, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0f0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0f4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0f8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0fc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f100, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f104, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f108, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f10c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f110, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f114, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f118, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f11c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f120, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f124, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f128, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f12c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f130, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f134, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f138, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f13c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f140, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f144, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f148, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f14c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f150, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f154, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f158, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f15c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f160, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f164, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f168, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f16c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f170, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f174, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f178, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f17c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f180, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f184, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f188, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f18c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f190, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f194, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f198, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f19c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1a4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1ac, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1b4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1bc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1c0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1c4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1c8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1cc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1d0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1d4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1d8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1dc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1e0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1e4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1e8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1ec, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1f0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1f4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1f8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f1fc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f000, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f004, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f008, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f00c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f010, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f014, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f018, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f01c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f020, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f024, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f028, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f02c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f030, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f034, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f038, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f03c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f040, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f044, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f048, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f04c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f050, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f054, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f058, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f05c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f060, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f064, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f068, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f06c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f070, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f074, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f078, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f07c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f080, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f084, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f088, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f08c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f090, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f094, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f098, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f09c, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0ac, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0bc, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c0, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c4, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0c8, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0cc, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d0, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d4, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0d8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f0dc, 0x00000003);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1f200, 0x00000003);
    /* #####################################*/
    /* ##PktMsg config*/
    /* #fabric interface */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18400, 0x00010008);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18404, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c04, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18408, 0x00010011);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1840c, 0x00000009);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c0c, 0x00090009);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18410, 0x0001001a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18414, 0x00000012);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c14, 0x00120012);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18418, 0x00010023);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1841c, 0x0000001b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c1c, 0x001b001b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18420, 0x0001002c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18424, 0x00000024);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c24, 0x00240024);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18428, 0x00010035);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1842c, 0x0000002d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c2c, 0x002d002d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18430, 0x0001003e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18434, 0x00000036);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c34, 0x00360036);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18438, 0x00010047);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1843c, 0x0000003f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c3c, 0x003f003f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18440, 0x00010050);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18444, 0x00000048);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c44, 0x00480048);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18448, 0x00010059);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1844c, 0x00000051);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c4c, 0x00510051);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18450, 0x00010062);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18454, 0x0000005a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c54, 0x005a005a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18458, 0x0001006b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1845c, 0x00000063);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c5c, 0x00630063);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18460, 0x00010074);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18464, 0x0000006c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c64, 0x006c006c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18468, 0x0001007d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1846c, 0x00000075);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c6c, 0x00750075);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18470, 0x00010086);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18474, 0x0000007e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c74, 0x007e007e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18478, 0x0001008e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1847c, 0x00000087);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c7c, 0x00870087);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18480, 0x00010096);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18484, 0x0000008f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c84, 0x008f008f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18488, 0x0001009e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1848c, 0x00000097);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c8c, 0x00970097);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18490, 0x000100a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18494, 0x0000009f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c94, 0x009f009f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18498, 0x000100ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1849c, 0x000000a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18c9c, 0x00a700a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184a0, 0x000100b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184a4, 0x000000af);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ca0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ca4, 0x00af00af);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184a8, 0x000100be);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184ac, 0x000000b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ca8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cac, 0x00b700b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184b0, 0x000100c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184b4, 0x000000bf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cb4, 0x00bf00bf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184b8, 0x000100ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184bc, 0x000000c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cbc, 0x00c700c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184c0, 0x000100d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184c4, 0x000000cf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cc4, 0x00cf00cf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184c8, 0x000100de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184cc, 0x000000d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ccc, 0x00d700d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184d0, 0x000100e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184d4, 0x000000df);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cd4, 0x00df00df);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184d8, 0x000100ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184dc, 0x000000e7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cdc, 0x00e700e7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184e0, 0x000100f6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184e4, 0x000000ef);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ce0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ce4, 0x00ef00ef);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184e8, 0x000100fe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184ec, 0x000000f7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ce8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cec, 0x00f700f7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184f8, 0x00010106);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac184fc, 0x000000ff);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cf8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18cfc, 0x00ff00ff);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18500, 0x0001010f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18504, 0x00000107);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d04, 0x01070107);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18508, 0x00010118);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1850c, 0x00000110);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d0c, 0x01100110);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18510, 0x00010121);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18514, 0x00000119);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d14, 0x01190119);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18518, 0x0001012a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1851c, 0x00000122);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d1c, 0x01220122);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18520, 0x00010133);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18524, 0x0000012b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d24, 0x012b012b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18528, 0x0001013c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1852c, 0x00000134);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d2c, 0x01340134);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18530, 0x00010145);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18534, 0x0000013d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d34, 0x013d013d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18538, 0x0001014e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1853c, 0x00000146);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d3c, 0x01460146);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18540, 0x00010157);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18544, 0x0000014f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d44, 0x014f014f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18548, 0x00010160);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1854c, 0x00000158);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d4c, 0x01580158);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18550, 0x00010169);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18554, 0x00000161);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d54, 0x01610161);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18558, 0x00010172);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1855c, 0x0000016a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d5c, 0x016a016a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18560, 0x0001017b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18564, 0x00000173);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d64, 0x01730173);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18568, 0x00010184);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1856c, 0x0000017c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d6c, 0x017c017c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18570, 0x0001018d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18574, 0x00000185);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d74, 0x01850185);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18578, 0x00010195);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1857c, 0x0000018e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d7c, 0x018e018e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18580, 0x0001019d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18584, 0x00000196);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d84, 0x01960196);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18588, 0x000101a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1858c, 0x0000019e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d8c, 0x019e019e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18590, 0x000101ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18594, 0x000001a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d94, 0x01a601a6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18598, 0x000101b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1859c, 0x000001ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18d9c, 0x01ae01ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185a0, 0x000101bd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185a4, 0x000001b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18da0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18da4, 0x01b601b6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185a8, 0x000101c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185ac, 0x000001be);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18da8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dac, 0x01be01be);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185b0, 0x000101cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185b4, 0x000001c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18db0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18db4, 0x01c601c6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185b8, 0x000101d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185bc, 0x000001ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18db8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dbc, 0x01ce01ce);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185c0, 0x000101dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185c4, 0x000001d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dc4, 0x01d601d6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185c8, 0x000101e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185cc, 0x000001de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dcc, 0x01de01de);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185d0, 0x000101ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185d4, 0x000001e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dd4, 0x01e601e6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185d8, 0x000101f5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185dc, 0x000001ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ddc, 0x01ee01ee);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185e0, 0x000101fd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185e4, 0x000001f6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18de0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18de4, 0x01f601f6);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185e8, 0x00010205);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185ec, 0x000001fe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18de8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dec, 0x01fe01fe);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185f8, 0x0001020d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac185fc, 0x00000206);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18df8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18dfc, 0x02060206);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18600, 0x00010216);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18604, 0x0000020e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e04, 0x020e020e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18608, 0x0001021f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1860c, 0x00000217);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e0c, 0x02170217);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18610, 0x00010228);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18614, 0x00000220);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e14, 0x02200220);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18618, 0x00010231);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1861c, 0x00000229);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e1c, 0x02290229);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18620, 0x0001023a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18624, 0x00000232);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e24, 0x02320232);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18628, 0x00010243);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1862c, 0x0000023b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e2c, 0x023b023b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18630, 0x0001024c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18634, 0x00000244);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e34, 0x02440244);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18638, 0x00010255);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1863c, 0x0000024d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e3c, 0x024d024d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18640, 0x0001025e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18644, 0x00000256);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e44, 0x02560256);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18648, 0x00010267);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1864c, 0x0000025f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e4c, 0x025f025f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18650, 0x00010270);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18654, 0x00000268);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e54, 0x02680268);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18658, 0x00010279);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1865c, 0x00000271);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e5c, 0x02710271);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18660, 0x00010282);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18664, 0x0000027a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e64, 0x027a027a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18668, 0x0001028b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1866c, 0x00000283);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e6c, 0x02830283);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18670, 0x00010294);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18674, 0x0000028c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e74, 0x028c028c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18678, 0x0001029c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1867c, 0x00000295);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e7c, 0x02950295);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18680, 0x000102a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18684, 0x0000029d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e84, 0x029d029d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18688, 0x000102ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1868c, 0x000002a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e8c, 0x02a502a5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18690, 0x000102b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18694, 0x000002ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e94, 0x02ad02ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18698, 0x000102bc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1869c, 0x000002b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18e9c, 0x02b502b5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186a0, 0x000102c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186a4, 0x000002bd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ea0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ea4, 0x02bd02bd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186a8, 0x000102cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186ac, 0x000002c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ea8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18eac, 0x02c502c5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186b0, 0x000102d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186b4, 0x000002cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18eb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18eb4, 0x02cd02cd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186b8, 0x000102dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186bc, 0x000002d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18eb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ebc, 0x02d502d5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186c0, 0x000102e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186c4, 0x000002dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ec0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ec4, 0x02dd02dd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186c8, 0x000102ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186cc, 0x000002e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ec8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ecc, 0x02e502e5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186d0, 0x000102f4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186d4, 0x000002ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ed0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ed4, 0x02ed02ed);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186d8, 0x000102fc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186dc, 0x000002f5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ed8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18edc, 0x02f502f5);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186e0, 0x00010304);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186e4, 0x000002fd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ee0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ee4, 0x02fd02fd);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186e8, 0x0001030c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186ec, 0x00000305);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ee8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18eec, 0x03050305);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186f8, 0x00010314);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac186fc, 0x0000030d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ef8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18efc, 0x030d030d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18700, 0x0001031d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18704, 0x00000315);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f00, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f04, 0x03150315);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18708, 0x00010326);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1870c, 0x0000031e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f08, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f0c, 0x031e031e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18710, 0x0001032f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18714, 0x00000327);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f10, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f14, 0x03270327);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18718, 0x00010338);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1871c, 0x00000330);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f18, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f1c, 0x03300330);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18720, 0x00010341);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18724, 0x00000339);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f20, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f24, 0x03390339);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18728, 0x0001034a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1872c, 0x00000342);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f28, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f2c, 0x03420342);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18730, 0x00010353);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18734, 0x0000034b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f30, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f34, 0x034b034b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18738, 0x0001035c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1873c, 0x00000354);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f38, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f3c, 0x03540354);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18740, 0x00010365);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18744, 0x0000035d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f40, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f44, 0x035d035d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18748, 0x0001036e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1874c, 0x00000366);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f48, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f4c, 0x03660366);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18750, 0x00010377);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18754, 0x0000036f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f50, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f54, 0x036f036f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18758, 0x00010380);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1875c, 0x00000378);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f58, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f5c, 0x03780378);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18760, 0x00010389);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18764, 0x00000381);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f60, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f64, 0x03810381);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18768, 0x00010392);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1876c, 0x0000038a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f68, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f6c, 0x038a038a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18770, 0x0001039b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18774, 0x00000393);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f70, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f74, 0x03930393);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18778, 0x000103a3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1877c, 0x0000039c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f78, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f7c, 0x039c039c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18780, 0x000103ab);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18784, 0x000003a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f80, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f84, 0x03a403a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18788, 0x000103b3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1878c, 0x000003ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f88, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f8c, 0x03ac03ac);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18790, 0x000103bb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18794, 0x000003b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f90, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f94, 0x03b403b4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18798, 0x000103c3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1879c, 0x000003bc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f98, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18f9c, 0x03bc03bc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187a0, 0x000103cb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187a4, 0x000003c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fa0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fa4, 0x03c403c4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187a8, 0x000103d3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187ac, 0x000003cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fa8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fac, 0x03cc03cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187b0, 0x000103db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187b4, 0x000003d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fb0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fb4, 0x03d403d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187b8, 0x000103e3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187bc, 0x000003dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fb8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fbc, 0x03dc03dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187c0, 0x000103eb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187c4, 0x000003e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fc0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fc4, 0x03e403e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187c8, 0x000103f3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187cc, 0x000003ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fc8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fcc, 0x03ec03ec);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187d0, 0x000103fb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187d4, 0x000003f4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fd0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fd4, 0x03f403f4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187d8, 0x00010403);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187dc, 0x000003fc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fd8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fdc, 0x03fc03fc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187e0, 0x0001040b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187e4, 0x00000404);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fe0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fe4, 0x04040404);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187e8, 0x00010413);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187ec, 0x0000040c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fe8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18fec, 0x040c040c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187f8, 0x0001041b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac187fc, 0x00000414);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ff8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18ffc, 0x04140414);
    /* #network interface */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18000, 0x000a0425);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18004, 0x0000041c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18800, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18804, 0x041c041c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18008, 0x0001042d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1800c, 0x00000426);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18808, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1880c, 0x04260426);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18010, 0x00010435);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18014, 0x0000042e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18810, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18814, 0x042e042e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18018, 0x0001043d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1801c, 0x00000436);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18818, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1881c, 0x04360436);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18020, 0x00010445);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18024, 0x0000043e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18820, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18824, 0x043e043e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18028, 0x0001044d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1802c, 0x00000446);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18828, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1882c, 0x04460446);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18030, 0x00010455);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18034, 0x0000044e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18830, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18834, 0x044e044e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18038, 0x0001045d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1803c, 0x00000456);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18838, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1883c, 0x04560456);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18040, 0x00010465);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18044, 0x0000045e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18840, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18844, 0x045e045e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18048, 0x0001046d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1804c, 0x00000466);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18848, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1884c, 0x04660466);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18050, 0x00010475);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18054, 0x0000046e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18850, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18854, 0x046e046e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18058, 0x0001047d);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1805c, 0x00000476);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18858, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1885c, 0x04760476);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18060, 0x000a0487);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18064, 0x0000047e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18860, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18864, 0x047e047e);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18068, 0x0001048f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1806c, 0x00000488);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18868, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1886c, 0x04880488);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18070, 0x00010497);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18074, 0x00000490);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18870, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18874, 0x04900490);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18078, 0x0001049f);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1807c, 0x00000498);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18878, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1887c, 0x04980498);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18080, 0x000104a7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18084, 0x000004a0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18880, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18884, 0x04a004a0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18088, 0x000104af);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1808c, 0x000004a8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18888, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1888c, 0x04a804a8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18090, 0x000104b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18094, 0x000004b0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18890, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18894, 0x04b004b0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18098, 0x000104bf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1809c, 0x000004b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18898, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1889c, 0x04b804b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180a0, 0x000104c7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180a4, 0x000004c0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188a4, 0x04c004c0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180a8, 0x000104cf);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180ac, 0x000004c8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188ac, 0x04c804c8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180b0, 0x000104d7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180b4, 0x000004d0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188b4, 0x04d004d0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180b8, 0x000104df);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180bc, 0x000004d8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188bc, 0x04d804d8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180c0, 0x000a04e9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180c4, 0x000004e0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188c0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188c4, 0x04e004e0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180c8, 0x000104f1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180cc, 0x000004ea);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188c8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188cc, 0x04ea04ea);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180d0, 0x000104f9);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180d4, 0x000004f2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188d0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188d4, 0x04f204f2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180d8, 0x00010501);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180dc, 0x000004fa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188d8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188dc, 0x04fa04fa);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180e0, 0x00010509);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180e4, 0x00000502);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188e0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188e4, 0x05020502);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180e8, 0x00010511);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180ec, 0x0000050a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188e8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188ec, 0x050a050a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180f0, 0x00010519);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180f4, 0x00000512);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188f0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188f4, 0x05120512);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180f8, 0x00010521);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac180fc, 0x0000051a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188f8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac188fc, 0x051a051a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18100, 0x00010529);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18104, 0x00000522);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18900, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18904, 0x05220522);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18108, 0x00010531);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1810c, 0x0000052a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18908, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1890c, 0x052a052a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18110, 0x00010539);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18114, 0x00000532);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18910, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18914, 0x05320532);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18118, 0x00010541);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1811c, 0x0000053a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18918, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1891c, 0x053a053a);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18120, 0x000a054b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18124, 0x00000542);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18920, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18924, 0x05420542);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18128, 0x00010553);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1812c, 0x0000054c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18928, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1892c, 0x054c054c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18130, 0x0001055b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18134, 0x00000554);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18930, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18934, 0x05540554);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18138, 0x00010563);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1813c, 0x0000055c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18938, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1893c, 0x055c055c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18140, 0x0001056b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18144, 0x00000564);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18940, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18944, 0x05640564);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18148, 0x00010573);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1814c, 0x0000056c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18948, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1894c, 0x056c056c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18150, 0x0001057b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18154, 0x00000574);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18950, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18954, 0x05740574);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18158, 0x00010583);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1815c, 0x0000057c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18958, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1895c, 0x057c057c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18160, 0x0001058b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18164, 0x00000584);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18960, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18964, 0x05840584);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18168, 0x00010593);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1816c, 0x0000058c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18968, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1896c, 0x058c058c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18170, 0x0001059b);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18174, 0x00000594);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18970, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18974, 0x05940594);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18178, 0x000105a3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1817c, 0x0000059c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18978, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1897c, 0x059c059c);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18180, 0x000a05ad);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18184, 0x000005a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18980, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18984, 0x05a405a4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18188, 0x000a05b7);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1818c, 0x000005ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18988, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1898c, 0x05ae05ae);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18190, 0x000a05c1);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18194, 0x000005b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18990, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18994, 0x05b805b8);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18198, 0x000a05cb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1819c, 0x000005c2);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac18998, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1899c, 0x05c205c2);
    /* #iloop, cpu, oam and eloop interface */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181a0, 0x002805d3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181a4, 0x000005cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189a0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189a4, 0x05cc05cc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181a8, 0x000105db);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181ac, 0x000005d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189a8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189ac, 0x05d405d4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181b0, 0x000105e3);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181b4, 0x000005dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189b0, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189b4, 0x05dc05dc);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181b8, 0x002805eb);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac181bc, 0x000005e4);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189b8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac189bc, 0x05e405e4);
    /* ##############################################*/
    /* #EpeHdrProc (base address: 0x0088_0000)*/
    /* ##############################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x008800e4, 0x00000001); /* #init */
    DATAPATH_WRITE_CHIP(chip_id, 0x00880058, 0x003a0040); 
    /* ##############################################*/
    /* #EpeStats (base address: 0x0098_0000)*/
    /* ##############################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00980010, 0x00000001); /* #InitEpeOverallFwd-->[0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00980004, 0x00000001); /* #InitEpePhbIntf-->[0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00980030, 0x00000001); /* #InitEpePortLog-->[0] */
    /* ##############################################*/
    /* #NetTx (base address: 0x003f_0000)*/
    /* ##############################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1000, 0x0000003f);  /* #Channel	0 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1004, 0x000e0000);  /* #Channel	0 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1008, 0x0040004f);  /* #Channel	1 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f100c, 0x00020000);  /* #Channel	1 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1010, 0x0050005f);  /* #Channel	2 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1014, 0x00020000);  /* #Channel	2 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1018, 0x0060006f);  /* #Channel	3 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f101c, 0x00020000);  /* #Channel	3 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1020, 0x0070007f);  /* #Channel	4 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1024, 0x00020000);  /* #Channel	4 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1028, 0x0080008f);  /* #Channel	5 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f102c, 0x00020000);  /* #Channel	5 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1030, 0x0090009f);  /* #Channel	6 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1034, 0x00020000);  /* #Channel	6 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1038, 0x00a000af);  /* #Channel	7 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f103c, 0x00020000);  /* #Channel	7 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1040, 0x00b000bf);  /* #Channel	8 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1044, 0x00020000);  /* #Channel	8 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1048, 0x00c000cf);  /* #Channel	9 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f104c, 0x00020000);  /* #Channel	9 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1050, 0x00d000df);  /* #Channel 10 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1054, 0x00020000);  /* #Channel 10 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1058, 0x00e000ef);  /* #Channel 11 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f105c, 0x00020000);  /* #Channel 11 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1060, 0x00f0012f);  /* #Channel 12 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1064, 0x000e0000);  /* #Channel 12 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1068, 0x0130013f);  /* #Channel 13 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f106c, 0x00020000);  /* #Channel 13 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1070, 0x0140014f);  /* #Channel 14 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1074, 0x00020000);  /* #Channel 14 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1078, 0x0150015f);  /* #Channel 15 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f107c, 0x00020000);  /* #Channel 15 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1080, 0x0160016f);  /* #Channel 16 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1084, 0x00020000);  /* #Channel 16 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1088, 0x0170017f);  /* #Channel 17 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f108c, 0x00020000);  /* #Channel 17 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1090, 0x0180018f);  /* #Channel 18 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1094, 0x00020000);  /* #Channel 18 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1098, 0x0190019f);  /* #Channel 19 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f109c, 0x00020000);  /* #Channel 19 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10a0, 0x01a001af);  /* #Channel 20 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10a4, 0x00020000);  /* #Channel 20 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10a8, 0x01b001bf);  /* #Channel 21 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10ac, 0x00020000);  /* #Channel 21 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10b0, 0x01c001cf);  /* #Channel 22 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10b4, 0x00020000);  /* #Channel 22 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10b8, 0x01d001df);  /* #Channel 23 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10bc, 0x00020000);  /* #Channel 23 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10c0, 0x01e0021f);  /* #Channel 24 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10c4, 0x000e0000);  /* #Channel 24 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10c8, 0x0220022f);  /* #Channel 25 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10cc, 0x00020000);  /* #Channel 25 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10d0, 0x0230023f);  /* #Channel 26 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10d4, 0x00020000);  /* #Channel 26 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10d8, 0x0240024f);  /* #Channel 27 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10dc, 0x00020000);  /* #Channel 27 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10e0, 0x0250025f);  /* #Channel 28 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10e4, 0x00020000);  /* #Channel 28 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10e8, 0x0260026f);  /* #Channel 29 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10ec, 0x00020000);  /* #Channel 29 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10f0, 0x0270027f);  /* #Channel 30 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10f4, 0x00020000);  /* #Channel 30 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10f8, 0x0280028f);  /* #Channel 31 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f10fc, 0x00020000);  /* #Channel 31 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1100, 0x0290029f);  /* #Channel 32 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1104, 0x00020000);  /* #Channel 32 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1108, 0x02a002af);  /* #Channel 33 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f110c, 0x00020000);  /* #Channel 33 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1110, 0x02b002bf);  /* #Channel 34 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1114, 0x00020000);  /* #Channel 34 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1118, 0x02c002cf);  /* #Channel 35 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f111c, 0x00020000);  /* #Channel 35 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1120, 0x02d0030f);  /* #Channel 36 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1124, 0x000e0000);  /* #Channel 36 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1128, 0x0310031f);  /* #Channel 37 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f112c, 0x00020000);  /* #Channel 37 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1130, 0x0320032f);  /* #Channel 38 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1134, 0x00020000);  /* #Channel 38 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1138, 0x0330033f);  /* #Channel 39 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f113c, 0x00020000);  /* #Channel 39 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1140, 0x0340034f);  /* #Channel 40 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1144, 0x00020000);  /* #Channel 40 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1148, 0x0350035f);  /* #Channel 41 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f114c, 0x00020000);  /* #Channel 41 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1150, 0x0360036f);  /* #Channel 42 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1154, 0x00020000);  /* #Channel 42 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1158, 0x0370037f);  /* #Channel 43 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f115c, 0x00020000);  /* #Channel 43 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1160, 0x0380038f);  /* #Channel 44 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1164, 0x00020000);  /* #Channel 44 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1168, 0x0390039f);  /* #Channel 45 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f116c, 0x00020000);  /* #Channel 45 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1170, 0x03a003af);  /* #Channel 46 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1174, 0x00020000);  /* #Channel 46 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1178, 0x03b003bf);  /* #Channel 47 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f117c, 0x00020000);  /* #Channel 47 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1180, 0x03c00423);  /* #Channel 48 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1184, 0x00260000);  /* #Channel 48 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1188, 0x04240487);  /* #Channel 49 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f118c, 0x00260000);  /* #Channel 49 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1190, 0x048804eb);  /* #Channel 50 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1194, 0x00260000);  /* #Channel 50 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f1198, 0x04ec054f);  /* #Channel 51 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f119c, 0x00260000);  /* #Channel 51 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f11a0, 0x055005ff);  /* #Channel 52 ChStaticInfo: startPtr[25:16]  endPtr[9:0]	 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f11a4, 0x00020000);  /* #Channel 52 ChStaticInfo: threshold[25:16] fifoDepth[7:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2000, 0x00000000);  /* #Channel	0  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2004, 0x00000000);  /* #Channel	0  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2008, 0x00000000);  /* #Channel	0  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f200c, 0x00000000);  /* #Channel	0  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2010, 0x00000000);  /* #Channel	1  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2014, 0x00000000);  /* #Channel	1  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2018, 0x00000040);  /* #Channel	1  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f201c, 0x00000040);  /* #Channel	1  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2020, 0x00000000);  /* #Channel	2  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2024, 0x00000000);  /* #Channel	2  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2028, 0x00000050);  /* #Channel	2  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f202c, 0x00000050);  /* #Channel	2  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2030, 0x00000000);  /* #Channel	3  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2034, 0x00000000);  /* #Channel	3  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2038, 0x00000060);  /* #Channel	3  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f203c, 0x00000060);  /* #Channel	3  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2040, 0x00000000);  /* #Channel	4  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2044, 0x00000000);  /* #Channel	4  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2048, 0x00000070);  /* #Channel	4  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f204c, 0x00000070);  /* #Channel	4  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2050, 0x00000000);  /* #Channel	5  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2054, 0x00000000);  /* #Channel	5  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2058, 0x00000080);  /* #Channel	5  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f205c, 0x00000080);  /* #Channel	5  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2060, 0x00000000);  /* #Channel	6  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2064, 0x00000000);  /* #Channel	6  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2068, 0x00000090);  /* #Channel	6  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f206c, 0x00000090);  /* #Channel	6  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2070, 0x00000000);  /* #Channel	7  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2074, 0x00000000);  /* #Channel	7  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2078, 0x000000a0);  /* #Channel	7  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f207c, 0x000000a0);  /* #Channel	7  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2080, 0x00000000);  /* #Channel	8  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2084, 0x00000000);  /* #Channel	8  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2088, 0x000000b0);  /* #Channel	8  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f208c, 0x000000b0);  /* #Channel	8  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2090, 0x00000000);  /* #Channel	9  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2094, 0x00000000);  /* #Channel	9  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2098, 0x000000c0);  /* #Channel	9  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f209c, 0x000000c0);  /* #Channel	9  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20a0, 0x00000000);  /* #Channel 10  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20a4, 0x00000000);  /* #Channel 10  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20a8, 0x000000d0);  /* #Channel 10  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20ac, 0x000000d0);  /* #Channel 10  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20b0, 0x00000000);  /* #Channel 11  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20b4, 0x00000000);  /* #Channel 11  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20b8, 0x000000e0);  /* #Channel 11  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20bc, 0x000000e0);  /* #Channel 11  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20c0, 0x00000000);  /* #Channel 12  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20c4, 0x00000000);  /* #Channel 12  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20c8, 0x000000f0);  /* #Channel 12  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20cc, 0x000000f0);  /* #Channel 12  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20d0, 0x00000000);  /* #Channel 13  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20d4, 0x00000000);  /* #Channel 13  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20d8, 0x00000130);  /* #Channel 13  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20dc, 0x00000130);  /* #Channel 13  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20e0, 0x00000000);  /* #Channel 14  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20e4, 0x00000000);  /* #Channel 14  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20e8, 0x00000140);  /* #Channel 14  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20ec, 0x00000140);  /* #Channel 14  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20f0, 0x00000000);  /* #Channel 15  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20f4, 0x00000000);  /* #Channel 15  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20f8, 0x00000150);  /* #Channel 15  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f20fc, 0x00000150);  /* #Channel 15  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2100, 0x00000000);  /* #Channel 16  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2104, 0x00000000);  /* #Channel 16  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2108, 0x00000160);  /* #Channel 16  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f210c, 0x00000160);  /* #Channel 16  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2110, 0x00000000);  /* #Channel 17  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2114, 0x00000000);  /* #Channel 17  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2118, 0x00000170);  /* #Channel 17  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f211c, 0x00000170);  /* #Channel 17  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2120, 0x00000000);  /* #Channel 18  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2124, 0x00000000);  /* #Channel 18  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2128, 0x00000180);  /* #Channel 18  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f212c, 0x00000180);  /* #Channel 18  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2130, 0x00000000);  /* #Channel 19  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2134, 0x00000000);  /* #Channel 19  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2138, 0x00000190);  /* #Channel 19  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f213c, 0x00000190);  /* #Channel 19  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2140, 0x00000000);  /* #Channel 20  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2144, 0x00000000);  /* #Channel 20  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2148, 0x000001a0);  /* #Channel 20  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f214c, 0x000001a0);  /* #Channel 20  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2150, 0x00000000);  /* #Channel 21  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2154, 0x00000000);  /* #Channel 21  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2158, 0x000001b0);  /* #Channel 21  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f215c, 0x000001b0);  /* #Channel 21  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2160, 0x00000000);  /* #Channel 22  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2164, 0x00000000);  /* #Channel 22  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2168, 0x000001c0);  /* #Channel 22  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f216c, 0x000001c0);  /* #Channel 22  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2170, 0x00000000);  /* #Channel 23  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2174, 0x00000000);  /* #Channel 23  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2178, 0x000001d0);  /* #Channel 23  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f217c, 0x000001d0);  /* #Channel 23  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2180, 0x00000000);  /* #Channel 24  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2184, 0x00000000);  /* #Channel 24  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2188, 0x000001e0);  /* #Channel 24  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f218c, 0x000001e0);  /* #Channel 24  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2190, 0x00000000);  /* #Channel 25  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2194, 0x00000000);  /* #Channel 25  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2198, 0x00000220);  /* #Channel 25  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f219c, 0x00000220);  /* #Channel 25  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21a0, 0x00000000);  /* #Channel 26  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21a4, 0x00000000);  /* #Channel 26  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21a8, 0x00000230);  /* #Channel 26  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21ac, 0x00000230);  /* #Channel 26  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21b0, 0x00000000);  /* #Channel 27  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21b4, 0x00000000);  /* #Channel 27  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21b8, 0x00000240);  /* #Channel 27  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21bc, 0x00000240);  /* #Channel 27  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21c0, 0x00000000);  /* #Channel 28  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21c4, 0x00000000);  /* #Channel 28  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21c8, 0x00000250);  /* #Channel 28  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21cc, 0x00000250);  /* #Channel 28  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21d0, 0x00000000);  /* #Channel 29  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21d4, 0x00000000);  /* #Channel 29  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21d8, 0x00000260);  /* #Channel 29  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21dc, 0x00000260);  /* #Channel 29  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21e0, 0x00000000);  /* #Channel 30  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21e4, 0x00000000);  /* #Channel 30  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21e8, 0x00000270);  /* #Channel 30  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21ec, 0x00000270);  /* #Channel 30  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21f0, 0x00000000);  /* #Channel 31  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21f4, 0x00000000);  /* #Channel 31  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21f8, 0x00000280);  /* #Channel 31  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f21fc, 0x00000280);  /* #Channel 31  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2200, 0x00000000);  /* #Channel 32  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2204, 0x00000000);  /* #Channel 32  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2208, 0x00000290);  /* #Channel 32  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f220c, 0x00000290);  /* #Channel 32  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2210, 0x00000000);  /* #Channel 33  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2214, 0x00000000);  /* #Channel 33  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2218, 0x000002a0);  /* #Channel 33  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f221c, 0x000002a0);  /* #Channel 33  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2220, 0x00000000);  /* #Channel 34  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2224, 0x00000000);  /* #Channel 34  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2228, 0x000002b0);  /* #Channel 34  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f222c, 0x000002b0);  /* #Channel 34  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2230, 0x00000000);  /* #Channel 35  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2234, 0x00000000);  /* #Channel 35  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2238, 0x000002c0);  /* #Channel 35  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f223c, 0x000002c0);  /* #Channel 35  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2240, 0x00000000);  /* #Channel 36  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2244, 0x00000000);  /* #Channel 36  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2248, 0x000002d0);  /* #Channel 36  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f224c, 0x000002d0);  /* #Channel 36  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2250, 0x00000000);  /* #Channel 37  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2254, 0x00000000);  /* #Channel 37  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2258, 0x00000310);  /* #Channel 37  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f225c, 0x00000310);  /* #Channel 37  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2260, 0x00000000);  /* #Channel 38  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2264, 0x00000000);  /* #Channel 38  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2268, 0x00000320);  /* #Channel 38  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f226c, 0x00000320);  /* #Channel 38  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2270, 0x00000000);  /* #Channel 39  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2274, 0x00000000);  /* #Channel 39  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2278, 0x00000330);  /* #Channel 39  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f227c, 0x00000330);  /* #Channel 39  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2280, 0x00000000);  /* #Channel 40  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2284, 0x00000000);  /* #Channel 40  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2288, 0x00000340);  /* #Channel 40  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f228c, 0x00000340);  /* #Channel 40  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2290, 0x00000000);  /* #Channel 41  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2294, 0x00000000);  /* #Channel 41  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2298, 0x00000350);  /* #Channel 41  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f229c, 0x00000350);  /* #Channel 41  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22a0, 0x00000000);  /* #Channel 42  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22a4, 0x00000000);  /* #Channel 42  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22a8, 0x00000360);  /* #Channel 42  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22ac, 0x00000360);  /* #Channel 42  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22b0, 0x00000000);  /* #Channel 43  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22b4, 0x00000000);  /* #Channel 43  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22b8, 0x00000370);  /* #Channel 43  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22bc, 0x00000370);  /* #Channel 43  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22c0, 0x00000000);  /* #Channel 44  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22c4, 0x00000000);  /* #Channel 44  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22c8, 0x00000380);  /* #Channel 44  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22cc, 0x00000380);  /* #Channel 44  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22d0, 0x00000000);  /* #Channel 45  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22d4, 0x00000000);  /* #Channel 45  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22d8, 0x00000390);  /* #Channel 45  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22dc, 0x00000390);  /* #Channel 45  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22e0, 0x00000000);  /* #Channel 46  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22e4, 0x00000000);  /* #Channel 46  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22e8, 0x000003a0);  /* #Channel 46  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22ec, 0x000003a0);  /* #Channel 46  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22f0, 0x00000000);  /* #Channel 47  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22f4, 0x00000000);  /* #Channel 47  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22f8, 0x000003b0);  /* #Channel 47  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f22fc, 0x000003b0);  /* #Channel 47  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2300, 0x00000000);  /* #Channel 48  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2304, 0x00000000);  /* #Channel 48  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2308, 0x000003c0);  /* #Channel 48  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f230c, 0x000003c0);  /* #Channel 48  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2310, 0x00000000);  /* #Channel 49  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2314, 0x00000000);  /* #Channel 49  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2318, 0x00000424);  /* #Channel 49  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f231c, 0x00000424);  /* #Channel 49  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2320, 0x00000000);  /* #Channel 50  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2324, 0x00000000);  /* #Channel 50  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2328, 0x00000488);  /* #Channel 50  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f232c, 0x00000488);  /* #Channel 50  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2330, 0x00000000);  /* #Channel 51  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2334, 0x00000000);  /* #Channel 51  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2338, 0x000004ec);  /* #Channel 51  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f233c, 0x000004ec);  /* #Channel 51  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2340, 0x00000000);  /* #Channel 52  ChDynamicInfo: dataUnitCnt[7:0]				 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2344, 0x00000000);  /* #Channel 52  ChDynamicInfo: lastPktUnitCnt[7:0]			 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f2348, 0x00000550);  /* #Channel 52  ChDynamicInfo: rdState[31] rdPtr[9:0]		 */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f234c, 0x00000550);  /* #Channel 52  ChDynamicInfo: wrState[31] wrPtr[9:0]		 */
    /* #config calender walk entry*/
    DATAPATH_WRITE_CHIP(chip_id, 0x003f0180, datapath_master.net_tx_cal_ptr);

    value = 0;
    /*NetTxXgmacAlternativeEn, set coresponding bit to 1 if use XGMAC*/
    for(i=0; i<DRV_MAX_XGMAC_NUM; i++)
    {
        if(datapath_master.xgmac_en[i])
        {
            value |= 1<<i;
        }
    }
    DATAPATH_WRITE_CHIP(chip_id, 0x003f003c, value); /* #alterEn */


    /* #config NetTx Calender*/
    for(i=0; i<176; i++)
    {
        DATAPATH_WRITE_CHIP(chip_id, 0x003f4000+i*4, datapath_master.net_tx_cal_entry[i]);
    }

    /* #NetTx: netTxInit*/
    DATAPATH_WRITE_CHIP(chip_id, 0x003f0020, 0x00000001); /* #NetTxInit[0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x003f0044, 0x00000000); /* #NetTxStallEnable */
    /* ##############################################*/
    /* #CpuMac (base address: 0x0045_0000)*/
    /* ##############################################*/
    /* #Add Mac Sa		   */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450020, 0x00000001); /* #CpuMacSa: sa[31:0]-->[31:0]  */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450024, 0x0000fefd); /* #CpuMacSa: sa[47:32]->[15:0] */
    /* #add type		   */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450018, 0x00005a5a); /* #CpuMacType: reserved[31:16], type[15:0]	 */
    /* #Add Mac Da 16	   */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450040, 0x00000000); /* #CpuMacDA0[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450044, 0x0000fefd); /* #CpuMacDA0[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450048, 0x00000000); /* #CpuMacDA1[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0045004c, 0x0000fefd); /* #CpuMacDA1[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450050, 0x00000000); /* #CpuMacDA2[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450054, 0x0000fefd); /* #CpuMacDA2[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450058, 0x00000000); /* #CpuMacDA3[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0045005c, 0x0000fefd); /* #CpuMacDA3[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450060, 0x00000000); /* #CpuMacDA4[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450064, 0x0000fefd); /* #CpuMacDA4[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450068, 0x00000000); /* #CpuMacDA5[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0045006c, 0x0000fefd); /* #CpuMacDA5[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450070, 0x00000000); /* #CpuMacDA6[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450074, 0x0000fefd); /* #CpuMacDA6[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450078, 0x00000000); /* #CpuMacDA7[31:0]-->[31:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x0045007c, 0x0000fefd); /* #CpuMacDA7[47:32]->[15:0] */
    DATAPATH_WRITE_CHIP(chip_id, 0x00450224, 0x00000001); /* #config */
    /* ##############################################*/
    /* #PBCtl (base address: 0x0b00_0000)*/
    /* ##############################################*/
    /* ##set PbCtlRefreshInterval #### */
    DATAPATH_WRITE_CHIP(chip_id, 0x0b000024, 0x28);
    /* #initialization PBCtl*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0b000010, 0x00000001); /* #PBCTL_Init */
    /* #########################################################*/
    /* ####write HashDsCtlInit*/
    /* #########################################################*/
    /* #init Hash 48K ram*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000064, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000058, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x000004c8, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000050, 0x00000000);
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000054, 0x0000bfff);
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000058, 0x00000001);
    kal_task_sleep(50);
    /* #config cfgHashKeySelect98K	t*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000004c8, 0x00000001);
    /* ##############################################*/
    /* ###read back the hash InitDone of 50K*/
    /* ##############################################*/
    DATAPATH_READ_CHIP(chip_id, 0x0d00005c, &value);
    /* #########################################################*/
    /* ###Hash Init 98K*/
    /* #########################################################*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000064, 0x00000001);
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000058, 0x00000000); /* #reset */
    DATAPATH_WRITE_CHIP(chip_id, 0x000004c8, 0x00000001); /* #cfg Hash Use 98K */
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000050, 0x00000000); /* #start */
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000054, 0x00017fff); /* #end */
    DATAPATH_WRITE_CHIP(chip_id, 0x0d000058, 0x00000001); /* #set init */
    kal_task_sleep(60);
    /* #config cfgHashKeySelect98K	to 0*/
    DATAPATH_WRITE_CHIP(chip_id, 0x000004c8, 0x00000000); /* #cfg */
    /* ##############################################*/
    /* ###read back the hash InitDone of 98K*/
    /* ##############################################*/
    DATAPATH_READ_CHIP(chip_id, 0x0d00005c, &value);
    /* ##############################################*/
    /* #Read Back all of Modules' InitDone*/
    /* ##############################################*/
    kal_task_sleep(10);
    for(i=0; i<DRV_MAX_QMAC_NUM; i++)
    {
        if(datapath_master.qmac_en[i])
        {
            DATAPATH_READ_CHIP(chip_id, 0x00310028+i*0x10000, &value);  /*#[0]->QuadMacApp0InitDone*/
        }
    }
    DATAPATH_READ_CHIP(chip_id, 0x003e001c, &value);  /*#[0]->NetRxFreeListInit.freeListInitDone*/
    DATAPATH_READ_CHIP(chip_id, 0x003e028c, &value);  /*#[0]->NetRxChannelInfoCtl.channelInfoInitDone								*/
    DATAPATH_READ_CHIP(chip_id, 0x08000174, &value);  /*#[0]->IpeHdrAdj.CmpcResWordRamInit.cmpcResWordRamInitDone*/
    DATAPATH_READ_CHIP(chip_id, 0x0940002c, &value);  /*#[0]->IpeAgingInitDone.initDone 						*/
    DATAPATH_READ_CHIP(chip_id, 0x0a40003c, &value);  /*#[0]->MetFifoInitDone.metFifoInitDone										*/
    DATAPATH_READ_CHIP(chip_id, 0x0a8000a8, &value);  /*#[0]->BufStoreLinkListTableCtrl.bufPtrTableInitDone 						*/
    DATAPATH_READ_CHIP(chip_id, 0x0a800034, &value);  /*#[0]->BufStoreChannelInfoCtrl.channelInfoInitDone		*/
    DATAPATH_READ_CHIP(chip_id, 0x0a800054, &value);  /*#[0]->BufStoreResrcCntCtrl.resrcCntRamInitDone								*/
    DATAPATH_READ_CHIP(chip_id, 0x06000018, &value);  /*#[0]->QMgrEnqInitDone.qMgrEnqInitDone										*/
    DATAPATH_READ_CHIP(chip_id, 0x06400024, &value);  /*#[0]->QMgrInitDone.qMgrLinkListInitDone 				*/
    DATAPATH_READ_CHIP(chip_id, 0x06800014, &value);  /*#[0]->QMgrSchInitDone.qMgrSchInitDone										*/
    DATAPATH_READ_CHIP(chip_id, 0x06800014, &value);  /*#[0]->QMgrSubChInitDone.qMgrSubChInitDone				*/
    DATAPATH_READ_CHIP(chip_id, 0x0ac00248, &value);  /*#[16]->BufRetrvInitCtl.bufInitDone						*/
    DATAPATH_READ_CHIP(chip_id, 0x0ac0024c, &value);  /*#[16]->BufRetrvInitCtl.fabricInitDone, [8]->BufRetrvInitCtl.networkInitDone */
    DATAPATH_READ_CHIP(chip_id, 0x003f0024, &value);  /*#[0]->NetTxInitDone.netTxInitDone						*/
    DATAPATH_READ_CHIP(chip_id, 0x00450228, &value);  /*#[0]->CpuMacInitDone.cpuMacInitDone 										*/
    DATAPATH_READ_CHIP(chip_id, 0x0b000014, &value);  /*#[0]->PbCtlInitDone.tabInitDone 						*/
    DATAPATH_READ_CHIP(chip_id, 0x0bc00054, &value);  /*#[24]->TcamCtlIntInitDone.cfgInitDone										*/
    DATAPATH_READ_CHIP(chip_id, 0x008800e0, &value);  /*#[0]->DsL3EditMplsSeqNumMemCtrl.dsL3EditMplsSeqNumMemInitDone				*/
    for(i=0; i<DRV_MAX_SGMAC_NUM; i++)
    {
        if(datapath_master.sgmac_en[i])
        {
            DATAPATH_READ_CHIP(chip_id, 0x004e0030+i*0x10000, &value);  /*#[0]->SgmacStatsInitDone.sgmacInitDone										*/
        }
    }
    DATAPATH_READ_CHIP(chip_id, 0x0d00005c, &value);  /*#HashDsCtl InitDone ->[0]								*/
    DATAPATH_READ_CHIP(chip_id, 0x09800014, &value);  /*#IpeStatsInitDoneIpeOverallFwd												*/
    DATAPATH_READ_CHIP(chip_id, 0x09800008, &value);  /*#IpeStatsInitDoneIpePhbIntf 												*/
    DATAPATH_READ_CHIP(chip_id, 0x09800034, &value);  /*#IpeStatsInitDoneIpePortLog 												*/
    DATAPATH_READ_CHIP(chip_id, 0x00480008, &value);  /*#StatisticsInitDone 														*/
    DATAPATH_READ_CHIP(chip_id, 0x08c0002c, &value);  /*#IpePktProcInit, expect 0x303												*/
    DATAPATH_READ_CHIP(chip_id, 0x0840002c, &value);  /*#IpeIntfMapperInitDone,expect 0x3f		*/
	DATAPATH_READ_CHIP(chip_id, 0x00a80484, &value);  /*#DsMp initDone */
	DATAPATH_READ_CHIP(chip_id, 0x00a8048c, &value);  /*#DaMa initDone */
	DATAPATH_READ_CHIP(chip_id, 0x00a80494, &value);  /*#DsMaName initDone */

    /*##############################################*/
    /*# add extra config*/
    /*##############################################*/
    /* #bufRetrv->QuadSlotEn*/
    /* #config BufRetrvCreditConfigMem*/
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c8c0, 0x00000001); /* #Sgmac0 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c8c4, 0x00000001); /* #Sgmac1 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c8c8, 0x00000001); /* #Sgmac2 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c8cc, 0x00000001); /* #Sgmac3 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c8dc, 0x00000002); /* #ELOOP */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c800, 0x00000003); /* #GMAC0/XGMAC0 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c830, 0x00000003); /* #GMAC12/XGMAC1 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c860, 0x00000003); /* #GMAC24/XGMAC2 */
    DATAPATH_WRITE_CHIP(chip_id, 0x0ac1c890, 0x00000003); /* #GMAC36/XGMAC3 */
    /* ##############################################*/
    /* ######QMgrSubCh*/
    /* ####NetWorkWeight Cfg Mem ###################*/
    /*DsNetworkWrrWeightCfg, XGMAC0~3*/
    for (i=0; i<DRV_MAX_GMAC_NUM; i++)
    {
        addr = 0x06c0cf00 + i*4;

        if(datapath_master.gmac_en[i])
        {
            DATAPATH_WRITE_CHIP(chip_id, addr, 0xa);
        }

        if(i==0 || i==12 || i==24 || i==36)
        {
            if(datapath_master.xgmac_en[i/12])
            {
                DATAPATH_WRITE_CHIP(chip_id, addr, 0x64);
            }
        }
    }

    for(i=48; i<52; i++)
    {
        if(datapath_master.sgmac_en[i-48])
        {
            addr = 0x06c0cf00 + i*4;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x64);
        }
    }

    DATAPATH_WRITE_CHIP(chip_id, 0x00940038, 0x000000ec); /* #configure to 236 for avoid overrun */

	DATAPATH_WRITE_CHIP(chip_id, 0x00450008, 0x00000000); /*#CpuMac: [0]->resetGmac */


	kal_task_sleep(1);
	/* ###########################################################################*/
	/* ###Xgmac/Sgmac soft reset, release mdioSoftRst */
	/* ###########################################################################*/
    /* release xgmac mdio soft reset */
    /*XgmacSoftRst, XGMAC0~3*/
    for(i=0; i<DRV_MAX_XGMAC_NUM; i++)
    {
        if(datapath_master.xgmac_en[i])
        {
            DATAPATH_WRITE_CHIP(chip_id, 0x004a0010+i*0x10000, 0x000000f2);
        }
    }

    /*SgmacSoftRst, SGMAC0~3*/
    for(i=0; i<DRV_MAX_SGMAC_NUM; i++)
    {
        if(datapath_master.sgmac_en[i])
        {
            DATAPATH_WRITE_CHIP(chip_id, 0x004e0010+i*0x10000, 0x000000f2);
        }
    }

    /*if fabric enable*/
    /*config FabricSerLtxLinkEnable
    FabricSerLrxLinkEnable
    FabricSerSoftResetLtx
    FabricSerSoftResetLrx
    FabricSerSoftResetSyn*/
    if(datapath_master.fabric_intf_en) /*fabric enable*/
    {
        for(i=0; i<16; i++)
        {
            if(datapath_master.fabric_en[i])
            {
                value |= 1<<i;
            }
        }

        DATAPATH_WRITE_CHIP(chip_id, 0x00630000, value); 
        DATAPATH_WRITE_CHIP(chip_id, 0x00630018, value); 
        DATAPATH_WRITE_CHIP(chip_id, 0x00630094, ~value); 
        DATAPATH_WRITE_CHIP(chip_id, 0x00630098, ~value);         
        DATAPATH_WRITE_CHIP(chip_id, 0x0063009c, 0x00000000); 
        DATAPATH_WRITE_CHIP(chip_id, 0x00640000, value);
    }

    /*Move to init gmac item.*/
#if 0
    /* Config GmacWrapper */
    for (i=0; i<DRV_MAX_GMAC_NUM; i++)
    {
        if(datapath_master.gmac_en[i])
        {
            addr = 0x00010210 + i*0x10000;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x3); 
        }
    }
#endif   
    DATAPATH_WRITE_CHIP(chip_id, 0x06000034, 0x3); 
    
    return DRV_E_NONE;
}

int32
drv_humber_init_mem(uint32 chip_id)
{
    uint32 addr;
    uint32 i;

    /* #configure the DsPhyPortMapTable (52 entries, 1 words) init the entries to 0*/
    for (i=0; i<52; i++)
    {
        addr = 0x08004000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsSgmacTcMapTable (16 entries, 1 words) init the entries to 0*/
    for (i=0; i<16; i++)
    {
        addr = 0x08005000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsPhyPortTable (256 entries, 1 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x08006000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsMplsCtlTable (256 entries, 2 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x0880f000 + i*8 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x0880f000 + i*8 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #IpePktProc:   base	  addr = 0x08c0_0000*/
    /* #configure the IpeClassificationCosMapTable(32 entries, 1 words) init the entries to 0*/
    for (i=0; i<32; i++)
    {
        addr = 0x08c00b00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsStormCtlMem (320 entries, 3 words) init the entries to 0*/
    for (i=0; i<320; i++)
    {
        addr = 0x08c18000 + i*16;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x08c18000 + i*16 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x08c18000 + i*16 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsFwdExtTable (2048 entries, 1 words) init the entries to 0*/
    for (i=0; i<2048; i++)
    {
        addr = 0x09004000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsApsBridgeTable (2048 entries, 2 words) init the entries to 0*/
    for (i=0; i<2048; i++)
    {
        addr = 0x09008000 + i*8 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x09008000 + i*8 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsSequenceNumberTable (256 entries, 1 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x09002000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsApsSelectTable (64 entries, 1 words) init the entries to 0*/
    for (i=0; i<64; i++)
    {
        addr = 0x09000200 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the MetFifoPriorityMapTable (256 entries, 1 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x0a802000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsMetFifoExcpTable (48 entries, 1 words) init the entries to 0*/
    for (i=0; i<48; i++)
    {
        addr = 0x0a40f400 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsLinkAggMemberNum (128 entries, 1 words) init the entries to 0*/
    for (i=0; i<128; i++)
    {
        addr = 0x0600dc00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsQueDropProf (256 entries, 6 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x06012000 + i*32 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06012000 + i*32 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06012000 + i*32 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06012000 + i*32 + 12;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06012000 + i*32 + 16;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06012000 + i*32 + 20;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsQueNumGenCtl (256 entries, 5 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x06010000 + i*32 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06010000 + i*32 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06010000 + i*32 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06010000 + i*32 + 12;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06010000 + i*32 + 16;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsSgmacMap (256 entries, 1 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x0600d400 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsServiceQueueHashKey (128 entries, 4 words) init the entries to 0*/
    for (i=0; i<128; i++)
    {
        addr = 0x0600c000 + i*16 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x0600c000 + i*16 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x0600c000 + i*16 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x0600c000 + i*16 + 12;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsServiceQueue (512 entries, 1 words) init the entries to 0*/
    for (i=0; i<512; i++)
    {
        addr = 0x0600b000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsGroupShapeProfileId (128 entries, 1 words) init the entries to 0*/
    for (i=0; i<128; i++)
    {
        addr = 0x0681b000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsGroupShapeProfile (256 entries, 2 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x0681a000 + i*8 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x0681a000 + i*8 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsQueueShapeProfileId (512 entries, 1 words) init the entries to 0*/
    for (i=0; i<512; i++)
    {
        addr = 0x06819000 + i*4 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* #configure the DsQueueShapeProfile (256 entries, 4 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x06818000 + i*16 + 0;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06818000 + i*16 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06818000 + i*16 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x06818000 + i*16 + 12;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsVplsPort all entry config 0(base	   address: 0x0085_4000)*/
    /* ##############################################*/
    for (i=0; i<256; i++)
    {
        addr = 0x00854000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsDestPhyPort all entry config 0(base	  address: 0x0080_3000)*/
    /* ##############################################*/
    for (i=0; i<512; i++)
    {
        addr = 0x00803000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #QMgr (base	   address: 0x0600_0000)*/
    /* ##############################################*/
    for (i=0; i<2048; i++)	/*#QMgrEnq.DsLinkAggregation and init the entries*/
    {
        addr = 0x06018000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* ##############################################*/
    /* #DsFabricWrrWeightCfg (base	   address: 32'h06c0df80)*/
    /* ##############################################*/
    /* #configure the DsFabricWrrWeightCfg (32 entries, 1 words) init the entries to 0*/
    for (i=0; i<32; i++)
    {
        addr = 0x06c0df80 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsFabricWrrWeight (base 	address: 32'h06c0de80)*/
    /* ##############################################*/
    /* #configure the DsFabricWrrWeight (32 entries, 1 words) init the entries to 0*/
    for (i=0; i<32; i++)
    {
        addr = 0x06c0de80 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #QMgrFabricOutProfileWrrWeightCfg(base	  address: 32'h06c0c780)*/
    /* ##############################################*/
    /* #configure the QMgrFabricOutProfileWrrWeightCfg(32 entries, 1 words) init the entries to 0*/
    for (i=0; i<32; i++)
    {
        addr = 0x06c0c780 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #QMgrFabricOutProfileWrrWeightCfg(base	  address: 32'h06c0c700)*/
    /* ##############################################*/
    /* #configure the QMgrFabricOutProfileWrrWeightCfg(32 entries, 1 words) init the entries to 0*/
    for (i=0; i<32; i++)
    {
        addr = 0x06c0c700 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsNetworkWrrWeightCfg(base	   address: 32'h06c0cf00)*/
    /* ##############################################*/
    /* #configure the DsNetworkWrrWeightCfg(52 entries, 1 words) init the entries to 0*/
    for (i=0; i<52; i++)
    {
        addr = 0x06c0cf00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsNetworkWrrWeight (base	 address: 32'h06c0ce00)*/
    /* ##############################################*/
    /* #configure the DsNetworkWrrWeight(52 entries, 1 words) init the entries to 0*/
    for (i=0; i<52; i++)
    {
        addr = 0x06c0ce00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #QMgrNetworkOutProfileWrrWeightCfg (base 	address: 32'h06c0cb00)*/
    /* ##############################################*/
    /* #configure the QMgrNetworkOutProfileWrrWeightCfg (52 entries, 1 words) init the entries to 0*/
    for (i=0; i<52; i++)
    {
        addr = 0x06c0cb00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #QMgrNetworkOutProfileWrrWeight (base	 address: 32'h06c0ca00)*/
    /* ##############################################*/
    /* #configure the QMgrNetworkOutProfileWrrWeight (52 entries, 1 words) init the entries to 0*/
    for (i=0; i<52; i++)
    {
        addr = 0x06c0ca00 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsBufRetrvColorMapTable(base	 address: 32'h0ac1f800)*/
    /* ##############################################*/
    /* #configure the DsBufRetrvColorMapTable(128 entries, 1 words) init the entries to 0*/
    for (i=0; i<128; i++)
    {
        addr = 0x0ac1f800 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsBufRetrvExceptionMem(base 	address: 32'h0ac1e000)*/
    /* ##############################################*/
    /* #configure the DsBufRetrvExceptionMem(64 entries, 1 words) init the entries to 0*/
    for (i=0; i<64; i++)
    {
        addr = 0x0ac1e000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsDestPort(base 	address: 32'h00844000)*/
    /* ##############################################*/
    /* #configure the DsDestPort(256 entries, 3 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x00844000 + i*16;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x00844000 + i*16 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0x00844000 + i*16 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #EPEEditPriorityMapTable(base	 address: 32'h0084c000)*/
    /* ##############################################*/
    /* #configure the EPEEditPriorityMapTable(2048 entries, 1 words) init the entries to 0*/
    for (i=0; i<2048; i++)
    {
        addr = 0x0084c000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsDestInterface(base	 address: 32'h00850000)*/
    /* ##############################################*/
    /* #configure the DsDestInterface(1024 entries, 2 words) init the entries to 0*/
    for (i=0; i<2048; i++)
    {
        addr = 0x00850000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsL3EditTunnelV6Ip(base 	address: 32'h00882000)*/
    /* ##############################################*/
    /* #configure the DsL3EditTunnelV6Ip(256 entries, 4 words) init the entries to 0*/
    for (i=0; i<1024; i++)
    {
        addr = 0x00882000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsL3EditSequenceNum (base	  address: 32'h00884000)*/
    /* ##############################################*/
    /* #configure the DsL3EditSequenceNum (64 entries, 4 words) init the entries to 0*/
    for (i=0; i<256; i++)
    {
        addr = 0x00884000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #DsL3EditTunnelV4IpSa(base	  address: 32'h00886000)*/
    /* ##############################################*/
    /* #configure the DsL3EditTunnelV4IpSa(64 entries, 1 words) init the entries to 0*/
    for (i=0; i<64; i++)
    {
        addr = 0x00886000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #EpeHdrEditL2EditLoopbackRam(base	 address: 32'h00944000)*/
    /* ##############################################*/
    /* #configure the EpeHdrEditL2EditLoopbackRam(256 entries, 4 words) init the entries to 0*/
    for (i = 0; i < 1024; i++)
    {
        addr = 0x00944000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    /* ##############################################*/
    /* #EpeHeaderEditSgmacPriorityMapMem(base	  address: 32'h00948800)*/
    /* ##############################################*/
    /* #configure the EpeHeaderEditSgmacPriorityMapMem(256 entries, 1 words) init the entries to 0*/
    for (i = 0; i < 256; i++)
    {
        addr = 0x00948800 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }

    /*# configure the DsOamExcp (32 entries, 1 word) init the entries to 0 */
	for (i=0; i<32; i++)
	{
		addr = 0x00ac4000 + i*4;
		DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
	}

    /* ##############################################  */
    /* # StpStateRam (base address: 32'h00490000)                                      */
    /* ##############################################  */
    /* # configure the StpStateRam(2048 entries, 1 words) init the entries to 0  */
	for (i=0; i<2048; i++)
	{
	    addr = 0x00490000 + i*4;
		DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
	}

    /*##############################################*/
    /*# DsLinkAggBlockMask (base address: 32'h0a409000)*/
    /*##############################################*/
    /*# configure the DsLinkAggBlockMask(64 entries, 2 words) init the entries to 0*/
    for (i = 0; i < 128; i++)
    {
        addr = 0xa409000 + i * 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }

    /*##############################################*/
    /*# DsLinkAggBitmap (base address: 32'h0a409800)*/
    /*##############################################*/
    /*# configure the DsLinkAggBitmap(128 entries, 3 words) init the entries to 0*/
    for (i = 0; i < 128; i++)
    {
        addr = 0xa409800 + i * 16;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0xa409800 + i * 16 + 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
        addr = 0xa409800 + i * 16 + 8;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }

    /*##############################################*/
    /*# DsSgmacMap (base address: 32'h0600d400)*/
    /*##############################################*/
    /*# configure the DsSgmacMap(256 entries, 1 words) init the entries to 0*/
    for (i = 0; i < 256; i++)
    {
        addr = 0x0600d400 + i * 4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }

    /*##############################################*/
    /*##register clear*/
    /*#############################################*/
    /*#IPE_HDR_ADJ_SGMAC_CTL*/
    DATAPATH_WRITE_CHIP(chip_id, 0x08000008, 0x0);


    /*#IPE_FORWARD_SGMAC_CTL*/
    DATAPATH_WRITE_CHIP(chip_id, 0x090000c0, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000c4, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000c8, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000cc, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000d0, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000d4, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x090000d8, 0x0);

    /*#EPE_HDR_ADJUST_SGMAC_CTL*/
    DATAPATH_WRITE_CHIP(chip_id, 0x00800008, 0x0);
    DATAPATH_WRITE_CHIP(chip_id, 0x0080000c, 0x0);

    /*#Q_MGRQ_WRITE_SGMAC_CTL reg*/
    DATAPATH_WRITE_CHIP(chip_id, 0x060001c0, 0x0);

    for(i=0; i<DRV_MAX_SGMAC_NUM; i++)
    {
        if(datapath_master.sgmac_en[i])
        {
            /*#SGMAC*_SGMAC_CTL   */
            addr = 0x004e0320+i*0x10000;
            DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
            DATAPATH_WRITE_CHIP(chip_id, addr+4, 0x0);
        }
    }

   /*configure DsLinkAggreagationGroup(2048 entrys, 1 words) init the entries to 0*/
    for (i = 0; i < 2048; i++)
    {
        addr = 0x0c4c0000 + i*4;
        DATAPATH_WRITE_CHIP(chip_id, addr, 0x0);
    }
    return DRV_E_NONE;
}

int32
drv_humber_init_pci(chip_reset_cb reset_cb, uint32 chip_id, drv_io_type_t io_type)
{
    int32 ret = DRV_E_NONE;

    ret |= reset_cb(CHIP_RESET_PCI, 0);

    kal_task_sleep(1);

    if(CHIP_IO_PCI == io_type)
    {
        ret |= reset_cb(CHIP_RESET_PCI, 1);
        kal_task_sleep(5);
    }

    return ret;
}

int32
drv_humber_get_datapath_info(drv_datapath_info_t* datapath_info)
{
    uint8 index = 0, speed_num=0;
    uint32 value;
    float speed;

    kal_memset(&datapath_master, 0, sizeof(datapath_master));

    datapath_master.cpu_mac_ref_clk = datapath_info->misc_item[DRV_MISC_ITEM_CPU_MAC_REF_CLK].value;
    
    for(index=DRV_SERDES_ITEM_NTSG0; index<=DRV_SERDES_ITEM_NTSG15; index++)
    {
        if(DRV_SERDES_TYPE_GE == datapath_info->serdes_item[index].type)
        {
            datapath_master.gmac_en[index] = 1;
            /*Even just 1 link enable, the related Qmac will be enable.*/
            datapath_master.qmac_en[index/4] = 1;
            /*Even just 1 link enable, the related hss will be enable.*/
            datapath_master.hss_en[index/8] = 1;
        }
        else if(DRV_SERDES_TYPE_XG == datapath_info->serdes_item[index].type)
        {
            datapath_master.xgmac_en[index/4] = 1;
            /*Even just 1 link enable, the related hss will be enable.*/
            datapath_master.hss_en[index/8] = 1;
        }
    }

    for(index=DRV_SERDES_ITEM_NTSG16; index<=DRV_SERDES_ITEM_NTSG31; index++)
    {
        if(DRV_SERDES_TYPE_GE == datapath_info->serdes_item[index].type)
        {
            datapath_master.gmac_en[index] = 1;
            /*Even just 1 link enable, the related Qmac will be enable.*/
            datapath_master.qmac_en[index/4] = 1;
            /*Even just 1 link enable, the related hss will be enable.*/
            datapath_master.hss_en[index/8] = 1;
        }
        else if(DRV_SERDES_TYPE_SG == datapath_info->serdes_item[index].type)
        {
            datapath_master.sgmac_en[(index-DRV_SERDES_ITEM_NTSG16)/4] = 1;
            datapath_master.is_hss4g = 1;
            /*Even just 1 link enable, the related hss will be enable.*/
            //datapath_master.hss_en[(index-DRV_SERDES_ITEM_NTSG16)/8] = 1;
            datapath_master.hss_en[index/8] = 1;
        }
    }

    for(index=DRV_SERDES_ITEM_NTSG32; index<=DRV_SERDES_ITEM_NTSG47; index++)
    {
        if(DRV_SERDES_TYPE_GE == datapath_info->serdes_item[index].type)
        {
            datapath_master.gmac_en[index] = 1;
            /*Even just 1 link enable, the related Qmac will be enable.*/
            datapath_master.qmac_en[index/4] = 1;
            /*Even just 1 link enable, the related hss will be enable.*/
            datapath_master.hss_en[index/8] = 1;
        }
        if((index == DRV_CPUMAC_CLK_SERDES) && drv_humber_cpumac_use_internal_refclk())
        {
            datapath_master.hss_en[index/8] = 1;
        }
    }

    for(index=DRV_SERDES_ITEM_UPSG0; index<=DRV_SERDES_ITEM_UPSG15; index++)
    {
        if(DRV_SERDES_TYPE_SG == datapath_info->serdes_item[index].type)
        {
            datapath_master.sgmac_en[(index-DRV_SERDES_ITEM_UPSG0)/4] = 1;
            datapath_master.is_hss4g = 0;
        }
        else if(DRV_SERDES_TYPE_FB == datapath_info->serdes_item[index].type)
        {
            datapath_master.fabric_en[index-DRV_SERDES_ITEM_UPSG0] = 1;
            datapath_master.fabric_intf_en = 1;
        }
    }

    /*caculate bandwidth*/
    datapath_master.cpu_bandwidth = 1;
    datapath_master.oam_bandwidth = 1;
    datapath_master.iloop_bandwidth = 1;
    datapath_master.eloop_bandwidth = 1;
    for(index=0; index<DRV_MAX_GMAC_NUM; index++)
    {
        if(datapath_master.gmac_en[index])
        {
            datapath_master.ipe_bandwidth++;
            datapath_master.gmac_bandwidth++;
        }
    }
    for(index=0; index<DRV_MAX_XGMAC_NUM; index++)
    {
        if(datapath_master.xgmac_en[index])
        {
            datapath_master.ipe_bandwidth+=10;
            datapath_master.gmac_bandwidth+=10;
        }
    }
    for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
    {
        if(datapath_master.sgmac_en[index])
        {
            datapath_master.ipe_bandwidth+=10;
            datapath_master.sgmac_bandwidth+=10;
        }
    }
    if(datapath_master.sgmac_bandwidth==0)
        datapath_master.sgmac_bandwidth = 1;
    if(datapath_master.gmac_bandwidth==0)
        datapath_master.gmac_bandwidth = 1;

    for(index=0; index<52; index++)
    {
        datapath_master.mac_mux_cal_entry[index] = datapath_info->calendar_entry_item[index].value;
    }
    datapath_master.mac_mux_cal_ptr = datapath_info->calendar_ptr_item[DRV_CALENDAR_PTR_ITEM_MAC_MUX].value;

    for(index=0; index<176; index++)
    {
        datapath_master.net_tx_cal_entry[index] = datapath_info->calendar_entry_item[index+52].value;
    }
    datapath_master.net_tx_cal_ptr = datapath_info->calendar_ptr_item[DRV_CALENDAR_PTR_ITEM_NET_TX].value;

    if(datapath_info->ext_mem_item[DRV_EXT_MEM_ITEM_DDR].is_used)
    {
        datapath_master.ext_ddr_en = 1;
        datapath_master.dl_lock[DRV_EXT_MEM_ITEM_DDR] = 0x00000001;
    }
    if(datapath_info->ext_mem_item[DRV_EXT_MEM_ITEM_QDR].is_used)
    {
        datapath_master.ext_qdr_en = 1;
        datapath_master.dl_lock[DRV_EXT_MEM_ITEM_QDR] = 0x00000100;
    }
    if(datapath_info->ext_mem_item[DRV_EXT_MEM_ITEM_TCAM].is_used)
    {
        datapath_master.ext_tcam_en = 1;
    }

    if(datapath_info->misc_item[DRV_MISC_ITEM_PTP_ENGINE].value)
    {
        datapath_master.ptp_en = 1;
    }

    datapath_master.cpu_mac_speed = datapath_info->misc_item[DRV_MISC_ITEM_CPU_MAC].value;

    if(datapath_master.fabric_intf_en == 1)
    {
        datapath_master.fabric_info.is_master = datapath_info->fabric_item[DRV_FABRIC_ITEM_MASTER].value;
        datapath_master.fabric_info.timer1 = datapath_info->fabric_item[DRV_FABRIC_ITEM_FABRIC_TIMER1].value;
        datapath_master.fabric_info.timer2 = datapath_info->fabric_item[DRV_FABRIC_ITEM_FABRIC_TIMER2].value;
        datapath_master.fabric_info.timer3_expire = datapath_info->fabric_item[DRV_FABRIC_ITEM_FABRIC_TIMER3_EXPIRE].value;
        datapath_master.fabric_info.timer3_check = datapath_info->fabric_item[DRV_FABRIC_ITEM_FABRIC_TIMER3_START_TO_CHECK].value;
        /*calucate fabric bandwidth.*/
        value = 0;
        for(index=DRV_SERDES_ITEM_UPSG0; index<=DRV_SERDES_ITEM_UPSG15; index++)
        {
            if(datapath_master.fabric_en[index-DRV_SERDES_ITEM_UPSG0] == 1)
            {
                value++;
                speed_num = datapath_info->serdes_item[index].speed;
            }
        }
        switch(speed_num)
        {
            case 0:
                speed = 1.25;
                break;
            case 1:
                speed = 3.125;
                break;
            case 2:
                speed = 3.75;
                break;
            case 3:
                speed = 4.0625;
                break;
            case 4:
                speed = 5;
                break;
            case 5:
                speed = 6.25;
                break;
            case 6:
                speed = 1.56;
                break;
            default:
                speed = 0;
                break;
        }
        speed = speed * 0.8 * value;
        /*ASIC require bandwidth should not be 0. */
        datapath_master.fabric_info.bandwidth = 1;
        while(datapath_master.fabric_info.bandwidth < speed)
        {
            datapath_master.fabric_info.bandwidth++;
        }
    }
    else
    {
        /*even not use fabric, should give 1 */
        datapath_master.fabric_info.bandwidth = 1;
    }
    for(index=DRV_PLL_ITEM_CORE; index<DRV_PLL_ITEM_MAX; index++)
    {
        datapath_master.pll_out[index] = datapath_info->pll_item[index].output_a;
    }
    datapath_master.chip_seq = datapath_info->chip_item.seq;

    return DRV_E_NONE;
}

void drv_humber_init_sw_emu()
{
    uint8 index;
    kal_memset(&datapath_master, 0, sizeof(datapath_master));
    datapath_master.ext_ddr_en = 0;
    datapath_master.ext_qdr_en = 0;
    datapath_master.ext_tcam_en = 0;
    datapath_master.pll_out[DRV_PLL_ITEM_CORE] = 625;
    for(index=0; index<DRV_MAX_GMAC_NUM; index++)
    {
        datapath_master.gmac_en[index] = 1;
    }
    for(index=0; index<DRV_MAX_XGMAC_NUM; index++)
    {
        datapath_master.xgmac_en[index]=0;
    }
    for(index=0; index<DRV_MAX_SGMAC_NUM; index++)
    {
        datapath_master.sgmac_en[index]=0;
    }
    for(index=0; index<DRV_MAX_QMAC_NUM; index++)
    {
        datapath_master.qmac_en[index]=1;
    }
}

int32
drv_humber_init_total(chip_reset_cb reset_cb, drv_chip_info_t chip_info, char* datapath_config_file)
{
    uint32 value;
    uint32 upsg_en[2]; /*upsg_en[0] for sg, upsg_en[1] for fabric*/
    uint32 speed[2];    
    uint32 strength[2];
    uint32 index = 0;
    int32 ret = DRV_E_NONE;
    uint8 pll_lock_out_bit[DRV_PLL_ITEM_MAX];
    uint32 pll_lock_status = 0;
    drv_datapath_info_t datapath_info;
    

    DRV_PTR_VALID_CHECK(datapath_config_file);
    if(NULL == reset_cb)
    {
        /* reset cb function not exists */
        return DRV_E_INVALID_PTR;
    }
    kal_memset(&datapath_info, 0, sizeof(datapath_info));
    ret = drv_read_datapath_profile((char*)datapath_config_file, &datapath_info);
    if(ret != 0)
     {
         return ret;
    }

    /*get datapath info*/
    ret =drv_humber_get_datapath_info(&datapath_info);
    if(ret != 0)
     {
         return ret;
    }
    /*For humber Demo board, this is needed because when soft-reboot, this is not done before.*/
    /*reset humber*/
    ret = reset_cb(CHIP_RESET_DLLOCK, 1);
    if(ret != 0)
        return DRV_E_DATAPATH_RELEASE_DLL_LOCK_FAIL;
    ret = reset_cb(CHIP_RESET_SUP, 0);
    if(ret != 0)
        return DRV_E_DATAPATH_RESET_SUP_FAIL;
    ret = reset_cb(CHIP_RESET_GLB, 0);
    if(ret != 0)
        return DRV_E_DATAPATH_RESET_GLB_FAIL;
    /*reset extTcam*/
    if(datapath_master.ext_tcam_en)
    {
        ret = reset_cb(CHIP_RESET_TCAM_CORE, 0);
        if(ret != 0)
            return DRV_E_DATAPATH_RESET_TCAM_CORE_FAIL;
        ret = reset_cb(CHIP_RESET_TCAM_POWER, 0);
        if(ret != 0)
            return DRV_E_DATAPATH_RESET_TCAM_POWER_FAIL;
    }

    kal_task_sleep(1);
    ret = reset_cb(CHIP_RESET_GLB, 1);
    if(ret != 0)
        return DRV_E_DATAPATH_RELEASE_GLB_FAIL;

    kal_task_sleep(1);
    ret = reset_cb(CHIP_RESET_GLB, 0);
    if(ret != 0)
        return DRV_E_DATAPATH_RESET_GLB_FAIL;

    kal_task_sleep(10);

    ret = reset_cb(CHIP_RESET_SUP, 1);
    if(ret != 0)
        return DRV_E_DATAPATH_RELEASE_SUP_FAIL;
    kal_task_sleep(1);

    /* check chip ID */
    DATAPATH_READ_CHIP(chip_info.chip_id, 0x8, &value);
    if(((value != 0x301)&&(value != 0x303)) || (chip_info.chip_type != CHIP_HUMBER))
    {
        return DRV_E_INVALID_CHIP;
    }
    datapath_master.rev = value;
    /*the default quanta 8 is refer to 125MHz ptp ref clk*/
    datapath_master.ptp_quanta[chip_info.chip_id] = chip_info.ptp_quanta;
    if (0x303 == value) /*HUMBER_A*/
    {
        datapath_master.ptp_quanta_ratio[chip_info.chip_id] = 2;
    }
    else
    {
        datapath_master.ptp_quanta_ratio[chip_info.chip_id] = 1;
    }

    pll_lock_out_bit[DRV_PLL_ITEM_CORE] = 0;
    pll_lock_out_bit[DRV_PLL_ITEM_TCAM] = 4;
    pll_lock_out_bit[DRV_PLL_ITEM_TABLE] = 8;
    pll_lock_out_bit[DRV_PLL_ITEM_HSS4G] = 12;
    pll_lock_out_bit[DRV_PLL_ITEM_HSS6G] = 16;
    for(index=DRV_PLL_ITEM_CORE; index<DRV_PLL_ITEM_MAX; index++)
    {
        ret = drv_humber_init_pll_item(chip_info.chip_id, index, (drv_pll_item_t *)(&datapath_info.pll_item[index]));
        if(ret != DRV_E_NONE)
            return ret;
        if(datapath_info.pll_item[index].is_used)
        {
            pll_lock_status |= 1 << (pll_lock_out_bit[index]);
        }
    }

    kal_task_sleep(10);
    /* check pll lock status */
    DATAPATH_READ_CHIP(chip_info.chip_id, 0xc, &value);
    if(value != pll_lock_status)
    {
        kal_task_sleep(100);
        /* check again */
        DATAPATH_READ_CHIP(chip_info.chip_id, 0xc, &value);
        if(value != pll_lock_status)
        {
            return DRV_E_DATAPATH_CHECK_PLL_LOCK_FAIL;
        }
    }

    /* release reset */
    ret = reset_cb(CHIP_RESET_GLB, 1);
    if(ret != 0)
        return DRV_E_DATAPATH_RELEASE_GLB_FAIL;
    kal_task_sleep(10);

    /* Enable and disable clk to let reset take effect.*/
    ret = drv_humber_clk_enable(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_CLK_ENABLE_FAIL;
    
    ret = drv_humber_clk_disable(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_CLK_DISABLE_FAIL;
    
    for(index=DRV_EXT_MEM_ITEM_DDR; index<=DRV_EXT_MEM_ITEM_TCAM; index++)
    {
        ret = drv_humber_init_ext_mem_item(reset_cb, chip_info.chip_id, index, 
                                                (drv_ext_mem_item_t *)(&datapath_info.ext_mem_item[index]));
        if(ret != DRV_E_NONE)
            return DRV_E_DATAPATH_INIT_EXT_MEM_FAIL;
    }
    
    ret = drv_humber_init_ntsg_serdes_item(chip_info.chip_id, &datapath_info);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_INIT_NTSG_SERDES_FAIL;

    kal_task_sleep(10);
    /* check ntsg serdes hss ready */
    ret = drv_humber_check_ntsg_serdes(chip_info.chip_id); 
    if(ret != DRV_E_NONE)
        return ret;

    ret = drv_humber_pwdn_ntsg_serdes_link(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_PWDN_NTSG_SERDES_FAIL;
    
    /* Not for UPSG, not select link, even one link used, must open all 16 links.*/
    /* Support two speed link mix used*/
    upsg_en[0] = 0;
    upsg_en[1] = 0;
    for(index=DRV_SERDES_ITEM_UPSG0; index<=DRV_SERDES_ITEM_UPSG15; index++)
    {
        if(DRV_SERDES_TYPE_SG == datapath_info.serdes_item[index].type)
        {
            upsg_en[0] |= (1<<(index-DRV_SERDES_ITEM_UPSG0));
            speed[0] = datapath_info.serdes_item[index].speed;
            strength[0] = datapath_info.serdes_item[index].drv_strength;
        }
        else if(DRV_SERDES_TYPE_FB == datapath_info.serdes_item[index].type)
        {
            upsg_en[1] |= (1<<(index-DRV_SERDES_ITEM_UPSG0)); 
            speed[1] = datapath_info.serdes_item[index].speed;
            strength[1] = datapath_info.serdes_item[index].drv_strength;            
        }
    }
    
    if((0 != upsg_en[0]) || (0 != upsg_en[1]))
    {
        ret = drv_humber_init_upsg_serdes_item(chip_info.chip_id, upsg_en, speed,strength,(drv_pll_item_t*)(&datapath_info.pll_item[DRV_PLL_ITEM_HSS6G]));
        if(ret != DRV_E_NONE)
            return DRV_E_DATAPATH_INIT_UPSG_SERDES_FAIL;
    }   

    kal_task_sleep(10);
    ret = drv_humber_init_hss(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_INIT_HSS_FAIL;

    /*init cpu mac*/
    ret = drv_humber_init_cpu_mac(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_INIT_CPU_MAC_FAIL;

    /*reset resetSupGmac & resetSupGmacReg*/
    ret = drv_humber_reset_gmac_item(chip_info.chip_id, 1, 0); /*reset logic*/
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_RESET_GMAC_FAIL;
    ret = drv_humber_reset_gmac_item(chip_info.chip_id, 1, 1); /*reset reg*/
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_RESET_GMAC_FAIL;

    /*release resetSupGmacReg*/
    ret = drv_humber_reset_gmac_item(chip_info.chip_id, 0, 1); /*release reg*/
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_RESET_GMAC_FAIL;
    
    /*init gmac/xgmac/sgmac/fabric*/
    for(index=DRV_SERDES_ITEM_NTSG0; index<=DRV_SERDES_ITEM_UPSG15; index++)
    {
        ret = drv_humber_init_mac_item(chip_info.chip_id, index, (drv_serdes_item_t *)(&datapath_info.serdes_item[index]));
        if(ret != DRV_E_NONE)
            return DRV_E_DATAPATH_INIT_GMAC_FAIL;
    }
    /* Not release logic reset in datapath init. This will be done when port enable.*/
#if 0
    /*release resetSupGmac*/
    ret = drv_humber_reset_gmac_item(chip_info.chip_id, 0, 0);  /*release logic*/
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_RESET_GMAC_FAIL;
#endif
    if(datapath_master.fabric_intf_en) 
    {
        ret = drv_humber_init_fabric_item(chip_info.chip_id, &(datapath_master.fabric_info));
        if(ret != DRV_E_NONE)
            return DRV_E_DATAPATH_INIT_FABRIC_FAIL;
    }
    
    ret = drv_humber_init_mem(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_INIT_MEM_FAIL;

    ret = drv_humber_init_datapath(chip_info.chip_id);
    if(ret != DRV_E_NONE)
        return DRV_E_DATAPATH_INIT_DATAPATH_FAIL;

    /*init mdio*/
    for(index=DRV_MDIO_ITEM_MDIO0; index<=DRV_MDIO_ITEM_MDIO2; index++)
    {
        ret = drv_humber_init_mdio_item(chip_info.chip_id, index, (drv_normal_item_t *)(&datapath_info.mdio_item[index]));
        if(ret != DRV_E_NONE)
            return DRV_E_DATAPATH_INIT_MDIO_FAIL;
    }
        
//    fclose(fp);
    return DRV_E_NONE;
}

uint8 drv_humber_xgmac_is_enable(uint8 xgmac_id)
{
    if(xgmac_id >= DRV_MAX_XGMAC_NUM)
        return 0;
    if(datapath_master.xgmac_en[xgmac_id] == 0)
        return 0;
    else 
        return 1;
}

uint8 drv_humber_sgmac_is_enable(uint8 sgmac_id)
{
    if(sgmac_id >= DRV_MAX_SGMAC_NUM)
        return 0;
    if(datapath_master.sgmac_en[sgmac_id] == 0)
        return 0;
    else 
        return 1;
}

uint8 drv_humber_qmac_is_enable(uint8 qmac_id)
{
    if(qmac_id >= DRV_MAX_QMAC_NUM)
        return 0;
    if(datapath_master.qmac_en[qmac_id] == 0)
        return 0;
    else 
        return 1;
}

uint8 drv_humber_gmac_is_enable(uint8 gmac_id)
{
    if(gmac_id >= DRV_MAX_GMAC_NUM)
        return 0;
    if(datapath_master.gmac_en[gmac_id] == 0)
        return 0;
    else 
        return 1;
}
uint8 drv_humber_old_version()
{
    if(datapath_master.rev == 0x301)
        return 1;
    else
        return 0;
}

uint8 drv_humber_cpumac_use_internal_refclk()
{
    if(CPUMAC_REFCLK_INTERNAL == datapath_master.cpu_mac_ref_clk)
        return 1;
    else
        return 0;
}

uint8 drv_humber_fabric_is_enable()
{
    if(datapath_master.fabric_intf_en == 0)
        return 0;
    else 
        return 1;
}

/* return value units: MHz*/
uint32 drv_humber_get_core_freq()
{
    return datapath_master.pll_out[DRV_PLL_ITEM_CORE];
}

uint8 drv_humber_get_ptp_en()
{
    if(datapath_master.ptp_en == 0)
        return 0;
    else
        return 1;
}

uint8 drv_humber_get_ptp_quanta(uint8 lchip)
{
    return datapath_master.ptp_quanta[lchip];
}

uint8 drv_humber_get_ptp_quanta_ratio(uint8 lchip)
{
    return datapath_master.ptp_quanta_ratio[lchip];
}

uint8 drv_humber_ext_tcam_is_enable()
{
    if(datapath_master.ext_tcam_en == 0)
        return 0;
    else 
        return 1;
}

uint8 drv_humber_ext_ddr_is_enable()
{
    if(datapath_master.ext_ddr_en == 0)
        return 0;
    else 
        return 1;
}

uint8 drv_humber_ext_qdr_is_enable()
{
    if(datapath_master.ext_qdr_en == 0)
        return 0;
    else 
        return 1;
}
