#include "kal.h"
#include "drv_lib.h"
#include "ctc_cli.h"
#include "dbg_tool_cli.h"
#include "dbg_tool_cli_process.h"

static bool _cmd_num(char *cmd)
{
    int32 len = 0, i = 0;
    uint8 b_hex = FALSE;

    if(NULL == cmd)
    {
        return FALSE;
    }

    len = kal_strlen(cmd);
    if(len > 2)
    {
        if(('0' == cmd[0]) && (('x' == cmd[1])||('X' == cmd[1])))
        {
            i = 2;
            b_hex = TRUE;
        }
    }
    for(; i < len ; i++)
    {
        if(b_hex)
        {
            if(((cmd[i] <= '9') && (cmd[i] >= '0')) || ((cmd[i] >= 'a')&&(cmd[i] <= 'f'))
                ||((cmd[i] >= 'A')&&(cmd[i] <= 'F')))
            {

            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            if((cmd[i] > '9') || (cmd[i] < '0'))
            {
                return FALSE;
            }
        }

    }

    return TRUE;
}

/*the following cmd is for emulation only*/
static int32
_write_floodgate(char* fpga_reg, uint32 data)
{
    return ctckal_usrctrl_write_floodgate(fpga_reg, data);
}

static int32
_read_floodgate(char* fpga_reg, uint32 *data)
{
    return ctckal_usrctrl_read_floodgate(fpga_reg, (uint32)data);
}

static int32
_write_mac(uint32 mac_id, uint32 port_id, char *reg_name,  uint32 data)
{
    return ctckal_usrctrl_write_mac(mac_id, port_id, reg_name, data);
}

static int32
_read_mac(uint32 mac_id, uint32 port_id, char *reg_name,  uint32 *data)
{
    return ctckal_usrctrl_read_mac(mac_id, port_id, reg_name, (uint32)data);
}

static int32
_write_phy(uint32 phy_id, char *reg_name,  uint32 data)
{
    return ctckal_usrctrl_write_phy(phy_id, reg_name, data);
}

static int32
_read_phy(uint32 phy_id, char *reg_name,  uint32* data)
{
    return ctckal_usrctrl_read_phy(phy_id, reg_name, (uint32)data);
}



CTC_CLI(cli_dbg_tool_driver_init,
    cli_dbg_tool_driver_init_cmd,
    "init driver <1-2> CHIP_ID_BASE",
    "Driver cmd",
    "Initialize driver information cmd",
    "Initialize chip num <1-2>",
    "Chip ID start base")
{
    uint8 chip_num = 0;
    uint8 chip_id_base = 0;
    int32 ret = DRV_E_NONE;

    CTC_CLI_GET_INTEGER("chip_num", chip_num, argv[0]);
    CTC_CLI_GET_INTEGER("chip_id_base", chip_id_base, argv[1]);

    ret = drv_init(chip_num, chip_id_base);
    if (ret < DRV_E_NONE)
    {
        return CLI_ERROR;
    }

    ctc_dbg_tool_init();

    return CLI_SUCCESS;
}


CTC_CLI(cli_dbg_tool_list_tbl_reg_tips,
    cli_dbg_tool_list_tbl_reg_tips_cmd,
    "list tbl-reg NAME ",
    "list table or register info ",
    "table or register",
    "table/register include string NAME")
{
    char *tip_str = NULL;

    tip_str = argv[0];

    show_tips_info(TIPS_TBL_REG, tip_str, 0);

    return CLI_SUCCESS;
}

/*cli read tbl/reg*/
CTC_CLI(cli_dbg_tool_read_tbl_reg_fld,
    cli_dbg_tool_read_tbl_reg_fld_cmd,
    "read chip CHIP_ID tbl-reg TBL_REG_NAME INDEX FIELD_NAME",
    "read chip table or register info",
    "chip ",
    "chipid value",
    "read table or register information",
    "table/register id name",
    "table/register entry index ",
    "table/register field name(string) or offset(num)")
{
    uint32 chip_id = 0;

    char* id_name = NULL;
    char* field_name = NULL;

    tbls_id_t id = 0;
    fld_id_t field_id = 0;
    uint32 index = 0;
    uint32 value = 0;
    uint32 offset = 0xFFFFFFFF;

    CTC_CLI_GET_INTEGER("chipid", chip_id, argv[0]);

    id_name = argv[1];
    if(!drv_get_tbl_id_by_string(&id, id_name))
    {
        ctc_cli_out("can't find tbl %s \n", id_name);
        return CLI_ERROR;
    }
    CTC_CLI_GET_INTEGER("tbl/reg index", index, argv[2]);
    if(3 == argc)
    {
        field_id = DRV_ENTRY_FLAG;
    }
    else
    {
        field_name = argv[3];
        if(_cmd_num(field_name))
        {
            CTC_CLI_GET_INTEGER("offset", offset, argv[3]);
        }
        else
        {
            if(!drv_get_field_id_by_string(id, &field_id, field_name))
            {
                ctc_cli_out("can't find table %s field %s \n", id_name, field_name);
                return CLI_ERROR;
            }
        }
    }

    if(0xFFFFFFFF != offset)
    {
        read_entry(chip_id, id, index, offset, &value);
        ctc_cli_out("%-20s entry %d offset %d 0x%08x\n", id_name, index, offset, value);
    }
    else
    {
        read_tbl_reg(chip_id, id, index, field_id);
    }
    return CLI_SUCCESS;
}

CTC_CLI(cli_dbg_tool_read_tbl_reg,
    cli_dbg_tool_read_tbl_reg_cmd,
    "read chip CHIP_ID tbl-reg TBL_REG_NAME INDEX",
    "read chip table or register info",
    "chip ",
    "chipid value",
    "read table or register information",
    "table/register id name",
    "table/register entry index ",
    "table/register field name(string) or offset(num)")
{
    uint32 chip_id = 0;

    char* id_name = NULL;
    char* field_name = NULL;

    uint32 id = 0;
    uint32 field_id = 0;
    uint32 index = 0;
    uint32 value = 0;
    uint32 offset = 0xFFFFFFFF;

    CTC_CLI_GET_INTEGER("chipid", chip_id, argv[0]);

    id_name = argv[1];
    if(!drv_get_tbl_id_by_string(&id, id_name))
    {
        ctc_cli_out("can't find tbl %s \n", id_name);
        return CLI_ERROR;
    }
    CTC_CLI_GET_INTEGER("tbl/reg index", index, argv[2]);
    if(3 == argc)
    {
        field_id = DRV_ENTRY_FLAG;
    }
    else
    {
        field_name = argv[3];
        if(_cmd_num(field_name))
        {
            CTC_CLI_GET_INTEGER("offset", offset, argv[3]);
        }
        else
        {
            if(!drv_get_field_id_by_string(id, &field_id, field_name))
            {
                ctc_cli_out("can't find table %s field %s \n", id_name, field_name);
                return CLI_ERROR;
            }
        }
    }

    if(0xFFFFFFFF != offset)
    {
        read_entry(chip_id, id, index, offset, &value);
        ctc_cli_out("%-20s entry %d offset %d 0x%08x\n", id_name, index, offset, value);
    }
    else
    {
        read_tbl_reg(chip_id, id, index, field_id);
    }
    return CLI_SUCCESS;
}


CTC_CLI(cli_dbg_tool_read_tbl_reg_c,
    cli_dbg_tool_read_tbl_reg_c_cmd,
    "read chip CHIP_ID tbl-reg-c TBL_REG_NAME INDEX_BEGIN INDEX_END",
    "read chip table or register info",
    "chip ",
    "chipid value",
    "read table or register information",
    "table/register id name",
    "table/register entry begin index ",
    "table/register entry end index ")
{
    uint32 chip_id = 0;

    char* id_name = NULL;
    uint32 id = 0;
    uint32 begin_index = 0;
    uint32 end_index = 0;
    uint32 value = 0;
    uint32 offset = 0xFFFFFFFF;
    uint32 i = 0, j = 0;
    uint32 entry_size = 0;

    CTC_CLI_GET_INTEGER("chipid", chip_id, argv[0]);

    id_name = argv[1];
    if(!drv_get_tbl_id_by_string(&id, id_name))
    {
        ctc_cli_out("can't find tbl %s \n", id_name);
        return CLI_ERROR;
    }
    CTC_CLI_GET_INTEGER("tbl/reg begin index", begin_index, argv[2]);
    CTC_CLI_GET_INTEGER("tbl/reg end index", end_index, argv[3]);

    for(i = begin_index; i <= end_index; i++)
    {
        entry_size = (TABLE_ENTRY_SIZE(id)/4);
        for (j = 0; j < entry_size; j++)
        {
            offset = j*4;
            read_entry(chip_id, id, i, offset, &value);
            ctc_cli_out("%-20s entry %d offset %d 0x%08x\n", id_name, i, offset, value);
        }
        ctc_cli_out("\n");

    }

    return CLI_SUCCESS;
}


CTC_CLI(cli_dbg_tool_write_tbl_reg,
    cli_dbg_tool_write_tbl_reg_cmd,
    "write chip CHIP_ID tbl-reg TBL_REG_ID INDEX ",
    "write chip table or register info",
    "chip information",
    "chipid value",
    "write table or register",
    "table or register ID name",
    "table or register entry index <0>",
    "table or register field name or offset ",
    "table or register field value")
{
    uint32 chip_id = 0;
    uint8 tips_type = TIPS_NONE;
    char *tip_str = NULL;

    char* id_name = NULL;
    char* field_name = NULL;

    uint32 id = 0;
    uint32 field_id = 0;
    uint32 index = 0;
    uint32 value = 0;
    uint32 offset = 0xFFFFFFFF;

    CTC_CLI_GET_INTEGER("chipid", chip_id, argv[0]);

    id_name = argv[1];
    if(!drv_get_tbl_id_by_string(&id, id_name))
    {
        ctc_cli_out("can't find tbl %s \n", id_name);
        return CLI_ERROR;
    }
    CTC_CLI_GET_INTEGER("tbl/reg index", index, argv[2]);
    if(3 == argc)
    {
        tips_type = TIPS_TBL_REG_FIELD;
    }
    else
    {
        field_name = argv[3];

        if(_cmd_num(field_name))
        {
            CTC_CLI_GET_INTEGER("offset", offset, argv[3]);
        }
        else
        {
            if(!drv_get_field_id_by_string(id, &field_id, field_name))
            {
                ctc_cli_out("can't find table %s field %s \n", id_name, field_name);
                return CLI_ERROR;
            }
        }

        CTC_CLI_GET_INTEGER("field-value", value, argv[4]);
    }

    if(TIPS_NONE != tips_type)
    {
        show_tips_info(tips_type, tip_str, id);
    }
    else
    {
        if(0xFFFFFFFF != offset)
        {
            write_entry(chip_id, id, index, offset, value);
        }
        else
        {
            write_tbl_reg(chip_id, id, index, field_id, value);
        }
    }

    return CLI_SUCCESS;
}


CTC_CLI(cli_dbg_tool_write_tbl_fld_reg,
    cli_dbg_tool_write_tbl_reg_fld_cmd,
    "write chip CHIP_ID tbl-reg TBL_REG_ID INDEX FIELD_ID VAL",
    "write chip table or register info",
    "chip information",
    "chipid value",
    "write table or register",
    "table or register ID name",
    "table or register entry index <0>",
    "table or register field name or offset ",
    "table or register field value")
{
    uint32 chip_id = 0;
    uint8 tips_type = TIPS_NONE;
    char *tip_str = NULL;

    char* id_name = NULL;
    char* field_name = NULL;

    uint32 id = 0;
    uint32 field_id = 0;
    uint32 index = 0;
    uint32 value = 0;
    uint32 offset = 0xFFFFFFFF;

    CTC_CLI_GET_INTEGER("chipid", chip_id, argv[0]);

    id_name = argv[1];
    if(!drv_get_tbl_id_by_string(&id, id_name))
    {
        ctc_cli_out("can't find tbl %s \n", id_name);
        return CLI_ERROR;
    }
    CTC_CLI_GET_INTEGER("tbl/reg index", index, argv[2]);
    if(3 == argc)
    {
        tips_type = TIPS_TBL_REG_FIELD;
    }
    else
    {
        field_name = argv[3];

        if(_cmd_num(field_name))
        {
            CTC_CLI_GET_INTEGER("offset", offset, argv[3]);
        }
        else
        {
            if(!drv_get_field_id_by_string(id, &field_id, field_name))
            {
                ctc_cli_out("can't find table %s field %s \n", id_name, field_name);
                return CLI_ERROR;
            }
        }

        CTC_CLI_GET_INTEGER("field-value", value, argv[4]);
    }

    if(TIPS_NONE != tips_type)
    {
        show_tips_info(tips_type, tip_str, id);
    }
    else
    {
        if(0xFFFFFFFF != offset)
        {
            write_entry(chip_id, id, index, offset, value);
        }
        else
        {
            write_tbl_reg(chip_id, id, index, field_id, value);
        }
    }

    return CLI_SUCCESS;
}


/* Cmd format: read chip CCHIP_ID address <addr_offset> (<replicate_time> | ) */
CTC_CLI(cli_dbg_tool_read_addr,
    cli_dbg_tool_read_addr_cmd,
    "read chip CHIP_ID address ADDR_OFFSET (REPLICATE_TIME |)",
    "read address value cmd",
    "chip information",
    "chip id value",
    "read address 0xXXXXXXXX",
    "address value",
    "replicate time")
{
    int32 ret = DRV_E_NONE;

    uint32 address_offset = 0, rep_time = 0;
    uint32 tmp_i = 0, chip_id = 0;
    uint32 tmp_value = 0;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    CTC_CLI_GET_INTEGER("addressOffset", address_offset, argv[1]);

    if(argc > 2)
    {
        CTC_CLI_GET_INTEGER("replicateTime", rep_time, argv[2]);
    }
    else
    {
        rep_time = 1;
    }

    for(tmp_i = 0 ; tmp_i < rep_time; tmp_i++)
    {
        //ctc_cli_out("Read GreatBelt: chip_id = %d; addr = 0x%08x\n", chip_id, address_offset);

        ret = dal_usrctrl_read_chip(chip_id, 0, address_offset, (uint32)&tmp_value);
        if (ret < DRV_E_NONE)
        {
            //ctc_cli_out("0x%08x address read ERROR!\n", address_offset);
            continue;
        }

        ctc_cli_out("0x%08x:    ", address_offset);
        ctc_cli_out("0x%08x\n", tmp_value);

        address_offset = address_offset + 4;
    }

    return CLI_SUCCESS;
}

/* Cmd format: write address <addr_offset> <value> */
CTC_CLI(cli_dbg_tool_write_addr,
    cli_dbg_tool_write_addr_cmd,
    "write chip CHIP_ID address ADDR_OFFSET WRITE_VALUE",
    "write value to address cmd",
    "chip information",
    "chip id value",
    "write address 0xXXXXXXXX <value>",
    "address value",
    "value to write")
{
    uint32 address_offset = 0, chip_id = 0, value = 0;
    int32 ret = DRV_E_NONE;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    CTC_CLI_GET_INTEGER("address", address_offset, argv[1]);
    CTC_CLI_GET_INTEGER("value", value, argv[2]);

    ret = dal_usrctrl_write_chip(chip_id, 0, address_offset, value);
    if (ret < DRV_E_NONE)
    {
        ctc_cli_out("0x%08x address write ERROR! Value = 0x%08x\n", address_offset, value);
    }

    return CLI_SUCCESS;
}


/* Cmd format: write address <addr_offset> <value> */
CTC_CLI(cli_dbg_tool_integrity_golden,
    cli_dbg_tool_integrity_golden_cmd,
    "integrity-compare chip CHIP_ID SRC_FILE DEST_FILE",
    "integrity-compare generate cmd",
    "chip information",
    "chip id value",
    "template file name",
    "golden file name")
{
    uint32 chip_id = 0;
    int32 ret = DRV_E_NONE;
    char *src_file = NULL;
    char *dest_file = NULL;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    src_file    = argv[1];
    dest_file   = argv[2];

    ret = ctc_dbg_tool_integrity_check_golden(chip_id, src_file, dest_file);
    if (ret < DRV_E_NONE)
    {
        ctc_cli_out("integrity check golden generate ERROR! \r\n");
    }

    return CLI_SUCCESS;
}

/* Cmd format: integrity-check chip CHIP_ID SRC_FILE DEST_FILE */
CTC_CLI(cli_dbg_tool_integrity_check,
    cli_dbg_tool_integrity_check_cmd,
    "integrity-check chip CHIP_ID SRC_FILE DEST_FILE",
    "integrity-check generate cmd",
    "chip information",
    "chip id value",
    "template file name",
    "golden file name")
{
    uint32 chip_id = 0;
    int32 ret = DRV_E_NONE;
    char *src_file = NULL;
    char *dest_file = NULL;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    src_file    = argv[1];
    dest_file   = argv[2];

    ret = ctc_dbg_tool_integrity_check_result(chip_id, src_file, dest_file);
    if (ret < DRV_E_NONE)
    {
        ctc_cli_out("integrity check result generate ERROR! \r\n");
    }

    return CLI_SUCCESS;
}

/* Cmd format: read floodgate REG_NAME */
CTC_CLI(cli_dbg_tool_read_floodgate,
    cli_dbg_tool_read_floodgate_cmd,
    "read floodgate REG_NAME",
    "read floodgate cmd",
    "floodgate",
    "floodgate register name")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 reg_val = 0;

    reg_name    = argv[0];

    ret = _read_floodgate(reg_name, &reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("read floodgate register %s ERROR! \r\n", reg_name);
    }
    else
    {
        kal_printf("floodgate reg %s : 0x%x \n", reg_name, reg_val);
    }
    return CLI_SUCCESS;
}

/* Cmd format: write floodgate REG_NAME REG_VAL */
CTC_CLI(cli_dbg_tool_write_floodgate,
    cli_dbg_tool_write_floodgate_cmd,
    "write floodgate REG_NAME REG_VAL",
    "write floodgate cmd",
    "floodgate",
    "floodgate register name",
    "value to write")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 reg_val = 0;

    reg_name    = argv[0];
    CTC_CLI_GET_INTEGER("REG_VAL", reg_val, argv[1]);

    ret = _write_floodgate(reg_name, reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("write floodgate register %s ERROR! \r\n", reg_name);
    }
    else
    {

    }
    return CLI_SUCCESS;
}

/* Cmd format: read mac MAC_ID PORT_ID REG_NAME */
CTC_CLI(cli_dbg_tool_read_mac,
    cli_dbg_tool_read_mac_cmd,
    "read mac MAC_ID PORT_ID REG_NAME",
    "read mac cmd",
    "mac id",
    "port id",
    "mac register name")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 mac_id = 0;
    uint32 port_id = 0;
    uint32 reg_val = 0;

    CTC_CLI_GET_INTEGER("MAC_ID", mac_id, argv[0]);
    CTC_CLI_GET_INTEGER("PORT_ID", port_id, argv[1]);
    reg_name    = argv[2];

    ret = _read_mac(mac_id, port_id, reg_name, &reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("read mac %d %d reg %s ERROR! \r\n", mac_id, port_id, reg_name);
    }
    else
    {
        kal_printf("mac reg %s : 0x%x \n", reg_name, reg_val);
    }
    return CLI_SUCCESS;
}

/* Cmd format: write mac MAC_ID PORT_ID REG_NAME REG_VAL */
CTC_CLI(cli_dbg_tool_write_mac,
    cli_dbg_tool_write_mac_cmd,
    "write mac MAC_ID PORT_ID REG_NAME REG_VAL",
    "write mac cmd",
    "mac id",
    "port id",
    "mac register name",
    "value to write")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 mac_id = 0;
    uint32 port_id = 0;
    uint32 reg_val = 0;

    CTC_CLI_GET_INTEGER("MAC_ID", mac_id, argv[0]);
    CTC_CLI_GET_INTEGER("PORT_ID", port_id, argv[1]);
    reg_name    = argv[2];
    CTC_CLI_GET_INTEGER("REG_VAL", reg_val, argv[3]);

    ret = _write_mac(mac_id, port_id, reg_name, reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("write mac %d %d reg %s ERROR! \r\n", mac_id, port_id, reg_name);
    }
    else
    {
    }
    return CLI_SUCCESS;
}

/* Cmd format: read phy PHY_ID REG_NAME */
CTC_CLI(cli_dbg_tool_read_phy,
    cli_dbg_tool_read_phy_cmd,
    "read phy PHY_ID REG_NAME",
    "read phy cmd",
    "phy id",
    "phy register name")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 phy_id = 0;
    uint32 reg_val = 0;

    CTC_CLI_GET_INTEGER("PHY_ID", phy_id, argv[0]);
    reg_name    = argv[1];

    ret = _read_phy(phy_id, reg_name, &reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("read phy %d reg %s ERROR! \r\n", phy_id, reg_name);
    }
    else
    {
        kal_printf("phy reg %s : 0x%x \n", reg_name, reg_val);
    }
    return CLI_SUCCESS;
}

/* Cmd format: write phy PHY_ID REG_NAME REG_VAL */
CTC_CLI(cli_dbg_tool_write_phy,
    cli_dbg_tool_write_phy_cmd,
    "write phy PHY_ID REG_NAME REG_VAL",
    "write phy cmd",
    "phy id",
    "phy register name",
    "value to write")
{
    int32 ret = DRV_E_NONE;
    char *reg_name = NULL;
    uint32 phy_id = 0;
    uint32 reg_val = 0;

    CTC_CLI_GET_INTEGER("PHY_ID", phy_id, argv[0]);
    reg_name    = argv[1];
    CTC_CLI_GET_INTEGER("REG_VAL", reg_val, argv[2]);

    ret = _write_phy(phy_id, reg_name, reg_val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("write phy %d %d reg %s ERROR! \r\n", phy_id, reg_name);
    }
    else
    {
    }
    return CLI_SUCCESS;
}

/* Cmd format: read v4fpga FPGA_ID ADDR */
CTC_CLI(cli_dbg_tool_read_fpga,
    cli_dbg_tool_read_fpga_cmd,
    "read v4fpga FPGA_ID ADDR",
    "read v4fpga cmd",
    "v4fpga id",
    "v4fpga address")
{
    int32 ret = DRV_E_NONE;
    uint32 fpga_id = 0;
    uint32 addr = 0;

    uint32 val = 0;

    CTC_CLI_GET_INTEGER("FPGA_ID", fpga_id, argv[0]);
    CTC_CLI_GET_INTEGER("FPGA_ID", addr, argv[1]);

    ret = ctckal_usrctrl_read_fpga(32, fpga_id, addr, (uint32)&val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("read fpga %d addr %s ERROR! \r\n", fpga_id, addr);
    }
    else
    {
        kal_printf("fpga addr 0x%x : 0x%x \n", addr, val);
    }
    return CLI_SUCCESS;
}

/* Cmd format: write v4fpga FPGA_ID ADDR VAL */
CTC_CLI(cli_dbg_tool_write_fpga,
    cli_dbg_tool_write_fpga_cmd,
    "write v4fpga FPGA_ID ADDR VAL",
    "write v4fpga cmd",
    "v4fpga id",
    "v4fpga ADDR",
    "value to write")
{
    int32 ret = DRV_E_NONE;
    uint32 fpga_id = 0;
    uint32 addr = 0;
    uint32 val = 0;

    CTC_CLI_GET_INTEGER("FPGA_ID", fpga_id, argv[0]);
    CTC_CLI_GET_INTEGER("ADDR", addr, argv[1]);
    CTC_CLI_GET_INTEGER("VAL", val, argv[2]);

    ret = ctckal_usrctrl_write_fpga(32, fpga_id, addr, val);
    if (ret < DRV_E_NONE)
    {
        kal_printf("write v4fpga %d 0x%x ERROR! \r\n", fpga_id, addr);
    }
    else
    {
    }
    return CLI_SUCCESS;
}

/* Cmd format: show chip CHIP_ID queue-depth */
CTC_CLI(cli_dbg_tool_queue_depth,
    cli_dbg_tool_queue_depth_cmd,
    "show chip CHIP_ID queue-depth",
    "show cmd",
    "chip information",
    "chip id value",
    "show queueDepth")
{
#if ZHOUW_NOTE
    uint32 chip_id = 0;
    int32 ret = DRV_E_NONE;
    uint32 entry_num = 0, idx = 0;

    fields_t* first_f = NULL;
    fields_t* end_f = NULL;
    uint8 num_f = 0;
    uint32 id = DsQueueDepth_t, filed_id = 0;
    uint32 cmd = 0;
    uint32 value = 0;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);

    entry_num = TABLE_MAX_INDEX(id);

    for(idx = 0; idx < entry_num; idx++)
    {
        first_f = TABLE_FIELD_INFO_PTR(id);
        num_f = TABLE_FIELD_NUM(id);
        end_f = &first_f[num_f];
        filed_id = 0;

        while (first_f < end_f)
        {
            cmd = DRV_IOR(id, filed_id);
            ret = drv_ioctl(chip_id, idx, cmd, &value);
            if (ret < DRV_E_NONE)
            {
                ctc_cli_out(" show queue depth ERROR! \r\n");
                return CLI_ERROR;
            }
            if(0 == value)
            {

            }
            else
            {
                ctc_cli_out("DsQueueDepth 0x%04x  %s  : 0x%x\n", idx, first_f->ptr_field_name, value);
            }

            first_f++;
            filed_id++;
        }
    }
#endif
    return CLI_SUCCESS;
}


CTC_CLI(cli_dbg_tool_queueid_mon,
    cli_dbg_tool_queueid_mon_cmd,
    "show chip CHIP_ID queueid-mon",
    "show cmd",
    "chip information",
    "chip id value",
    "show queueIdMon")
{
#if ZHOUW_NOTE
    uint32 chip_id = 0;
    int32 ret = DRV_E_NONE;

    fields_t* first_f = NULL;
    fields_t* end_f = NULL;
    uint8 num_f = 0;
    uint32 id = QMgrQueueIdMon_t, filed_id = 0;
    uint32 cmd = 0;
    uint32 value = 0;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);

    first_f = TABLE_FIELD_INFO_PTR(id);
    num_f = TABLE_FIELD_NUM(id);
    end_f = &first_f[num_f];
    filed_id = 0;

    while (first_f < end_f)
    {
        cmd = DRV_IOR(id, filed_id);
        ret = drv_ioctl(chip_id, 0, cmd, &value);
        if (ret < DRV_E_NONE)
        {
            ctc_cli_out("show queueid-mon ERROR! \r\n");
            return CLI_ERROR;
        }

        ctc_cli_out("queueid-mon 0x%04x  %s  : 0x%x\n", 0, first_f->ptr_field_name, value);

        first_f++;
        filed_id++;
    }
#endif
    return CLI_SUCCESS;
}

int32 ctc_dbg_tool_cli_init(uint8 cli_tree_mode)
{
    ctc_com_cli_init(cli_tree_mode);
/*
emulation board
*/
    install_element(cli_tree_mode, &cli_dbg_tool_read_floodgate_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_floodgate_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_mac_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_mac_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_phy_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_phy_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_fpga_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_fpga_cmd);

    if (cli_tree_mode == CTC_DBG_TOOL_MODE)
    {
        install_element(cli_tree_mode, &cli_dbg_tool_driver_init_cmd);
    }

    install_element(cli_tree_mode, &cli_dbg_tool_list_tbl_reg_tips_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_tbl_reg_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_tbl_reg_fld_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_read_tbl_reg_c_cmd);

    install_element(cli_tree_mode, &cli_dbg_tool_write_tbl_reg_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_tbl_reg_fld_cmd);

    install_element(cli_tree_mode, &cli_dbg_tool_read_addr_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_write_addr_cmd);

    install_element(cli_tree_mode, &cli_dbg_tool_integrity_golden_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_integrity_check_cmd);

    install_element(cli_tree_mode, &cli_dbg_tool_queueid_mon_cmd);
    install_element(cli_tree_mode, &cli_dbg_tool_queue_depth_cmd);
    //ctc_dbg_tool_init();

    return CLI_SUCCESS;
}

