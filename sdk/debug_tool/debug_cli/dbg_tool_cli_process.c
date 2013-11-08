#include "drv_lib.h"
#include "dbg_tool_data_base.h"
#include "dbg_tool_cli_process.h"


#define DBG_CFG_FILE "gb_dbg.cfg"
#define MAX_PER_LINE 512

enum parser_type_e
{
    PARSER_TYPE_INST,
    PARSER_TYPE_STAT,
    PARSER_TYPE_INVALID,
};
typedef enum parser_type_e parser_type_t;

static parser_type_t parser_type = PARSER_TYPE_INVALID;
static uint32 bit_map[1024];

int32 show_tips_info(uint8 tips_type, char* inc_str, tbls_id_t id)
{
    tbls_id_t tbl_reg_id = 0;
    char comment[128] = {0};
    char up_comment[128] = {0};
    char up_str[128] = {0};
    uint8  i = 0;
    fld_id_t field_id = 0;

    fields_t* first_f = NULL;
    fields_t* end_f = NULL;
    uint8 num_f = 0;

    if(((NULL == inc_str)&&(TIPS_TBL_REG == tips_type))
        || (TIPS_NONE == tips_type))
    {
        return CLI_ERROR;
    }

    if(NULL != inc_str)
    {
        for(i = 0; i < kal_strlen(inc_str); i++)
        {
            up_str[i] = kal_toupper(inc_str[i]);
        }
    }

    if(TIPS_TBL_REG == tips_type)
    {
        for(tbl_reg_id = 0; tbl_reg_id  < MaxTblId_t; tbl_reg_id++)
        {
            if (!drv_get_tbl_string_by_id(tbl_reg_id, comment))
            {
                ctc_cli_out(" Get TBL_REG_Name by tbl/RegId error! tbl/RegId: %d, Line = %d\n",
                                      tbl_reg_id, __LINE__);
            }
            else
            {
                for(i = 0; i < kal_strlen(comment); i++)
                {
                    up_comment[i] = kal_toupper(comment[i]);
                }
                if(NULL != kal_strstr(up_comment, up_str))
                {
                    ctc_cli_out("ID: %04d, \tNAME: %s\n", tbl_reg_id, comment);
                }
                kal_memset(comment, 0, sizeof(comment));
                kal_memset(up_comment, 0, sizeof(up_comment));
            }
        }
    }
    else if(TIPS_TBL_REG_FIELD == tips_type)
    {
        if (MaxTblId_t <= id)
        {
            ctc_cli_out("\nERROR! INVALID tbl/Reg ID! tbl/Reg ID: %d, file:%s line:%d function:%s\n",
                        id,__FILE__,__LINE__,__FUNCTION__);
            return CLI_ERROR;
        }

        first_f = TABLE_INFO(id).ptr_fields;
        num_f = TABLE_INFO(id).num_fields;
        end_f = &first_f[num_f];

        while (first_f < end_f)
        {
            ctc_cli_out("FIELD_ID = %d, \tFIELD_NAME: %s\n", field_id, first_f->ptr_field_name);
            first_f++;
            field_id++;
        }
    }

    return CLI_SUCCESS;
}

int read_entry(uint8 chip_id, tbls_id_t id, uint32 index, uint32 offset, uint32*value)
{
    uint32 hw_addr_base = 0;
    uint32 entry_size   = 0;
    uint32 max_entry    = 0;
    uint32 hw_addr_offset      = 0;
    int32  ret = 0;

    if (MaxTblId_t <= id)
    {
        ctc_cli_out("\nERROR! INVALID tbl/RegID! tbl/RegID: %d, file:%s line:%d function:%s\n",
                    id,__FILE__,__LINE__,__FUNCTION__);
        return CLI_ERROR;
    }
    entry_size      = TABLE_ENTRY_SIZE(id);
    max_entry       = TABLE_MAX_INDEX(id);
    hw_addr_base    = TABLE_DATA_BASE(id);
    if(index >= max_entry)
    {
        return CLI_ERROR;
    }
    /*TODO: dynamic table*/
    if((4 == entry_size) || (8 == entry_size))
    {/*1w or 2w*/
        hw_addr_offset  =   hw_addr_base + index * entry_size + offset;
    }
    else if(entry_size <= 4*4)
    {/*4w*/
        hw_addr_offset  =   hw_addr_base + index * 4*4 + offset;
    }
    else if(entry_size <= 4*8)
    {/*8w*/
        hw_addr_offset  =   hw_addr_base + index * 4*8 + offset;
    }
    else if(entry_size <= 4*12)
    {/*12w*/
        hw_addr_offset  =   hw_addr_base + index * 4*12 + offset;
    }
    else if(entry_size <= 4*16)
    {/*16w*/
        hw_addr_offset  =   hw_addr_base + index * 4*16 + offset;
    }
    else if(entry_size <= 4*20)
    {/*20w*/
        hw_addr_offset  =   hw_addr_base + index * 4*20 + offset;
    }
    else if(entry_size <= 4*24)
    {/*24w*/
        hw_addr_offset  =   hw_addr_base + index * 4*24 + offset;
    }
    else if(entry_size <= 4*28)
    {/*28w*/
        hw_addr_offset  =   hw_addr_base + index * 4*28 + offset;
    }
    else if(entry_size <= 4*32)
    {/*32w*/
        hw_addr_offset  =   hw_addr_base + index * 4*32 + offset;
    }
    else if(entry_size <= 4*36)
    {/*36w*/
        hw_addr_offset  =   hw_addr_base + index * 4*36 + offset;
    }
    else if(entry_size <= 4*40)
    {/*40w*/
        hw_addr_offset  =   hw_addr_base + index * 4*40 + offset;
    }

    ret = dal_usrctrl_read_chip(chip_id, 0, hw_addr_offset, (uint32)value);

    if (ret < DRV_E_NONE)
    {
        ctc_cli_out("0x%08x address read ERROR!\n", hw_addr_offset);
        return ret;
    }

    return CLI_SUCCESS;

}

int write_entry(uint8 chip_id, tbls_id_t id, uint32 index, uint32 offset, uint32 value)
{
    uint32 hw_addr_base = 0;
    uint32 entry_size   = 0;
    uint32 max_entry    = 0;
    uint32 hw_addr_offset      = 0;
    int32  ret = 0;

    if (MaxTblId_t <= id)
    {
        ctc_cli_out("\nERROR! INVALID tbl/RegID! tbl/RegID: %d, file:%s line:%d function:%s\n",
                    id,__FILE__,__LINE__,__FUNCTION__);
        return CLI_ERROR;
    }
    entry_size      = TABLE_ENTRY_SIZE(id);
    max_entry       = TABLE_MAX_INDEX(id);
    hw_addr_base    = TABLE_DATA_BASE(id);
    if(index >= max_entry)
    {
        return CLI_ERROR;
    }
    /*TODO: dynamic table*/
    if((4 == entry_size) || (8 == entry_size))
    {/*1w or 2w*/
        hw_addr_offset  =   hw_addr_base + index * entry_size + offset;
    }
    else if(entry_size <= 4*4)
    {/*4w*/
        hw_addr_offset  =   hw_addr_base + index * 4*4 + offset;
    }
    else if(entry_size <= 4*8)
    {/*8w*/
        hw_addr_offset  =   hw_addr_base + index * 4*8 + offset;
    }
    else if(entry_size <= 4*12)
    {/*12w*/
        hw_addr_offset  =   hw_addr_base + index * 4*12 + offset;
    }
    else if(entry_size <= 4*16)
    {/*16w*/
        hw_addr_offset  =   hw_addr_base + index * 4*16 + offset;
    }
    else if(entry_size <= 4*20)
    {/*20w*/
        hw_addr_offset  =   hw_addr_base + index * 4*20 + offset;
    }
    else if(entry_size <= 4*24)
    {/*24w*/
        hw_addr_offset  =   hw_addr_base + index * 4*24 + offset;
    }
    else if(entry_size <= 4*28)
    {/*28w*/
        hw_addr_offset  =   hw_addr_base + index * 4*28 + offset;
    }
    else if(entry_size <= 4*32)
    {/*32w*/
        hw_addr_offset  =   hw_addr_base + index * 4*32 + offset;
    }
    else if(entry_size <= 4*36)
    {/*36w*/
        hw_addr_offset  =   hw_addr_base + index * 4*36 + offset;
    }
    else if(entry_size <= 4*40)
    {/*40w*/
        hw_addr_offset  =   hw_addr_base + index * 4*40 + offset;
    }

    ret = dal_usrctrl_write_chip(chip_id, 0, hw_addr_offset, value);
    if (ret < DRV_E_NONE)
    {
        ctc_cli_out("0x%08x address write ERROR! Value = 0x%08x\n", hw_addr_offset, value);
        return ret;
    }

    return CLI_SUCCESS;
}

int32 read_tbl_reg(uint8 chip_id, tbls_id_t id, uint32 index, fld_id_t field_id)
{
    fields_t* field = NULL;
    uint32 value = 0;
    uint32 cmd = 0;
    uint32 field_id_tmp = 0;

    fields_t* first_f = NULL;
    fields_t* end_f = NULL;
    uint8 num_f = 0;

    if (MaxTblId_t <= id)
    {
        ctc_cli_out("\nERROR! INVALID tbl/RegID! tbl/RegID: %d, file:%s line:%d function:%s\n",
                    id,__FILE__,__LINE__,__FUNCTION__);
        return CLI_ERROR;
    }

    if(DRV_ENTRY_FLAG == field_id)
    {
        first_f = TABLE_INFO(id).ptr_fields;
        num_f = TABLE_INFO(id).num_fields;
        end_f = &first_f[num_f];

        while (first_f < end_f)
        {
            cmd = DRV_IOR(id, field_id_tmp);
            drv_ioctl(chip_id, index, cmd, &value);
            ctc_cli_out("FIELD_ID = %d, FIELD_NAME: %s, \tFIELD_VAL :0x%02x\n", field_id_tmp, first_f->ptr_field_name, value);

            first_f++;
            field_id_tmp++;
        }
    }
    else
    {
        field = drv_find_field(id, field_id);
        cmd = DRV_IOR(id, field_id);
        drv_ioctl(chip_id, index, cmd, &value);
        ctc_cli_out("FIELD_ID = %d, FIELD_NAME: %s, \tFIELD_VAL :0x%02x\n", field_id, field->ptr_field_name, value);
    }

    return CLI_SUCCESS;
}

int32 write_tbl_reg(uint8 chip_id, tbls_id_t id, uint32 index, fld_id_t field_id, uint32 value)
{
    fields_t* field = NULL;
    uint32 cmd = 0;

    if (MaxTblId_t <= id)
    {
        ctc_cli_out("\nERROR! INVALID tbl/RegID! tbl/RegID: %d, file:%s line:%d function:%s\n",
                    id,__FILE__,__LINE__,__FUNCTION__);
        return CLI_ERROR;
    }

    field = drv_find_field(id, field_id);
    cmd = DRV_IOW(id, field_id);
    drv_ioctl(chip_id, index, cmd, &value);

    //ctc_cli_out("FIELD_ID = %d, FIELD_NAME: %s, \tFIELD_VAL :0x%02x\n", sub_id, field->field_name, value);

    return CLI_SUCCESS;
}


static int32 _instance_entry_register(char *module_name, char*tbl_reg_name, int32 entry_index, char*desc)
{
    dbg_mod_id_t module_id = MAX_MOD_NUM;
    tbls_id_t tbl_reg_id = MaxTblId_t;
    tbls_id_t tbl_reg_id_base = MaxTblId_t;
    dbg_modules_t *ptr_module = NULL;
    tables_info_t *ptr_reg = NULL;
    static entry_desc_t *entry_desc = NULL;
    uint32 inst_num = 0;
    uint32 entry_num = 0;
    uint32 real_entry_num = 0;

    uint8 cn = 0;

    if((NULL == module_name) || (NULL == tbl_reg_name) || (NULL == desc))
    {
        return CLI_ERROR;
    }

    if(!dbg_get_module_id_by_string(module_name, &module_id))
    {
        return CLI_ERROR;
    }

    if(!drv_get_tbl_id_by_string(&tbl_reg_id_base, tbl_reg_name))
    {
        return CLI_ERROR;
    }

    ptr_module  = DBG_MOD_GET_INFOPTR(module_id);
    ptr_reg     = TABLE_INFO_PTR(tbl_reg_id_base);
    inst_num    = ptr_module->inst_num;
    real_entry_num = ptr_reg->max_index_num;
#ifdef TEMP_NOTE
    if(tbl_reg_id_base == QuadMacStatsRam0_t)
    {
        entry_num   = real_entry_num/4;
    }
    else
#endif
    {
        entry_num   = real_entry_num;
    }

    if(entry_index >= entry_num)
    {
        return CLI_ERROR;
    }
    else
    {
        if(0 == entry_index)
        {
            entry_desc = kal_malloc(real_entry_num * sizeof(entry_desc_t));
            if(NULL == entry_desc)
            {

            }
            else
            {
                kal_memset(entry_desc, 0, sizeof(real_entry_num * sizeof(entry_desc_t)));
                kal_strncpy(entry_desc[entry_index].desc, desc, 64);
            }
        }
        else if(entry_index == (entry_num -1))
        {
            kal_strncpy(entry_desc[entry_index].desc, desc, 64);

#ifdef TEMP_NOTE
            if(tbl_reg_id_base == QuadMacStatsRam0_t)
            {
                for(cn = 0 ; cn < entry_num; cn ++)
                {
                    kal_strncpy(entry_desc[entry_num*1 + cn].desc, entry_desc[cn].desc, 64);
                    kal_strncpy(entry_desc[entry_num*2 + cn].desc, entry_desc[cn].desc, 64);
                    kal_strncpy(entry_desc[entry_num*3 + cn].desc, entry_desc[cn].desc, 64);
                }
            }
#endif
            /*register to all instance*/
            for(cn = 0; cn < inst_num; cn ++)
            {
                tbl_reg_id = tbl_reg_id_base + cn;
                dbg_register_desc_info(tbl_reg_id, entry_desc);
            }
            entry_desc = NULL;
        }
        else
        {
            kal_strncpy(entry_desc[entry_index].desc, desc, 64);

        }
    }

    return CLI_SUCCESS;

}

static int32 _stat_info_register(char *module_name, char* tbl_reg_name)
{

    dbg_mod_id_t module_id = MAX_MOD_NUM;
    tbls_id_t tbl_reg_id_base = MaxTblId_t;
    tbls_id_t tbl_reg_id = MaxTblId_t;
    dbg_modules_t *ptr_module = NULL;
    tables_info_t *ptr_reg = NULL;
    uint32 inst_num = 0;
    uint32 cn = 0;

    if((NULL == module_name) || (NULL == tbl_reg_name))
    {
        return CLI_ERROR;
    }

    if(!dbg_get_module_id_by_string(module_name, &module_id))
    {
        return CLI_ERROR;
    }

    if(!drv_get_tbl_id_by_string(&tbl_reg_id_base, tbl_reg_name))
    {
        return CLI_ERROR;
    }
    ptr_module  = DBG_MOD_GET_INFOPTR(module_id);
    ptr_reg     = TABLE_INFO_PTR(tbl_reg_id);
    inst_num    = ptr_module->inst_num;

    for(cn = 0; cn < inst_num; cn++)
    {
        tbl_reg_id = tbl_reg_id_base + cn;
        dbg_register_stat_info(tbl_reg_id);
    }

    return CLI_SUCCESS;
}


static bool _parser_line(char *buf)
{
    char module_name[64] = {0};
    char tbl_reg_name[64] = {0};
    char desc[64] = {0};
    int32 entry_index = 0;
    int32 n = 0;

    if(NULL != kal_strstr(buf, "###instance desc###"))
    {
        parser_type = PARSER_TYPE_INST;
    }
    else if(NULL != kal_strstr(buf, "###debug_stats###"))
    {
        parser_type = PARSER_TYPE_STAT;
    }
    else
    {
        if(PARSER_TYPE_INST == parser_type)
        {
            n = kal_sscanf(buf, "%s%s%d%s", module_name, tbl_reg_name, &entry_index, desc);
            if(4 != n)
            {
                return TRUE;
            }
            else
            {
                //kal_printf("%s %s %d %s\r\n", module_name, tbl_reg_name, entry_index, desc);
                _instance_entry_register(module_name, tbl_reg_name, entry_index, desc);
            }
        }
        else if(PARSER_TYPE_STAT == parser_type)
        {
            n = kal_sscanf(buf, "%s%s", module_name, tbl_reg_name);
            if(2 != n)
            {
                return TRUE;
            }
            else
            {
                //kal_printf("%s %s\r\n", module_name, tbl_reg_name);
                _stat_info_register(module_name, tbl_reg_name);

            }
        }
    }
    return TRUE;

}

static int32 _ctc_dbg_tool_get_id_by_poly(char *poly_str, tbls_id_t* tbl_id, fld_id_t* fld_id)
{
    char tbl_str[64] = {0};
    char fld_str[64] = {0};
    char *head = NULL;

    head = kal_strstr(poly_str, "[");

    if(NULL == head)
    {
        return DRV_E_INVALID_PARAMETER;
    }
    else
    {
        kal_sscanf(head, "\x5b%[^.].%[^\x5d]", tbl_str, fld_str);
        if(!drv_get_tbl_id_by_string(tbl_id, tbl_str))
        {
            kal_printf("can find table %s\r\n", tbl_str);
            return DRV_E_INVALID_TBL;
        }
        if(!drv_get_field_id_by_string(*tbl_id, fld_id, fld_str))
        {
            kal_printf("can find field %s\r\n", fld_str);
            return DRV_E_INVALID_FLD;
        }
    }
    return DRV_E_NONE;
}

static int32 _ctc_dbg_tool_dbg_info_init(char *cfg_file)
{
#define MAX_LINE_LEN 512
    FILE *src_file = NULL;
    char buf[MAX_LINE_LEN] = {0};

    if(NULL == cfg_file)
    {
        return CLI_ERROR;
    }
    src_file = fopen(cfg_file, "r");
    if (NULL == src_file)
    {
        ctc_cli_out("fopen %s failed!\n", cfg_file);
        return CLI_ERROR;
    }

    while(NULL != fgets(buf, MAX_LINE_LEN, src_file))
    {
        _parser_line(buf);
        kal_memset(buf, 0, MAX_LINE_LEN);
    }

    if(src_file)
    {
        fclose(src_file);
        src_file = NULL;
    }

    return CLI_SUCCESS;
}


static int _module_do(uint32 chip_id, dbg_modules_t *ptr_module, uint32 index)
{
    tbls_id_t    *reg_list_id    = NULL;
    uint32      id_num          = 0;

    tbls_id_t    reg_id_base     = MaxTblId_t;
    tbls_id_t    reg_id          = MaxTblId_t;
    fld_id_t    field_id        = 0;

    tables_info_t *id_node        = NULL;

    tbls_ext_info_t  *ext_info       = NULL;
    entry_desc_t *desc = NULL;

    fields_t* first_f = NULL;
    fields_t* end_f = NULL;
    uint8 num_f = 0;

    uint32 i = 0;
    uint32 cmd = 0;
    uint32 value = 0;

    uint8 b_show = FALSE;
    uint32 entry_no = 0;
    uint32 entry_cn = 0;

    reg_list_id = ptr_module->reg_list_id;
    id_num      = ptr_module->id_num;


    for(i = 0; i < id_num; i++)
    {
        b_show  = FALSE;
        desc    = NULL;
        reg_id_base  = reg_list_id[i];
        reg_id = reg_id_base + index;

        id_node = TABLE_INFO_PTR(reg_id);

        entry_no = id_node->max_index_num;
        ext_info = id_node->ptr_ext_info;

        while(ext_info)
        {
            switch(ext_info->ext_content_type)
            {
                case EXT_INFO_TYPE_DESC:
                    desc = ext_info->ptr_ext_content;
                    break;
                case EXT_INFO_TYPE_DBG:
                    b_show = TRUE;
                    break;
                case EXT_INFO_TYPE_TCAM:
                default:
                    break;
            }
            ext_info = ext_info->ptr_next;
        }

        if(b_show)
        {
            ctc_cli_out("\r\n#########%s#############\r\n", TABLE_NAME(reg_id));
            if(!desc)
            {
                for(entry_cn = 0; entry_cn < entry_no; entry_cn++)
                {
                    if(entry_no > 1)
                    {
                        ctc_cli_out("\r\n#########index %d#############\r\n", entry_cn);
                    }
                    first_f = TABLE_INFO(reg_id).ptr_fields;
                    num_f = TABLE_INFO(reg_id).num_fields;
                    end_f = &first_f[num_f];
                    field_id = 0;
                    while (first_f < end_f)
                    {
                        cmd = DRV_IOR(reg_id, field_id);
                        drv_ioctl(chip_id, entry_cn, cmd, &value);
                        ctc_cli_out("%-40s  : 0x%08x\n", first_f->ptr_field_name,value);

                        first_f++;
                        field_id++;
                    }
                }
            }
            else
            {
                for(entry_cn = 0; entry_cn < entry_no; entry_cn++)
                {
                    field_id = 0;
                    first_f = TABLE_INFO(reg_id).ptr_fields;
                    num_f = TABLE_INFO(reg_id).num_fields;
#ifdef TEMP_NOTE
                    if((QuadMacStatsRam0_t == reg_id) && (0 == entry_cn%48) )
                    {
                        ctc_cli_out("\r\n##########################Quad MAC index%d##########################\r\n", entry_cn/48);
                    }
#endif
                    end_f = &first_f[num_f];
                    ctc_cli_out("\r\n#########%s#############\r\n", desc[entry_cn].desc);
                    while(first_f < end_f)
                    {
                        cmd = DRV_IOR(reg_id, field_id);
                        drv_ioctl(chip_id, entry_cn, cmd, &value);
                        ctc_cli_out("%-40s  : 0x%08x\n", first_f->ptr_field_name, value);
                        first_f++;
                        field_id++;
                    }
                }
            }
        }

        b_show  = FALSE;
    }

    return CLI_SUCCESS;
}

static int32 _module_process(struct cmd_element *self, struct vty *vty, int argc, char **argv)
{

    uint32 chip_id = 0;
    char *module_name = NULL;
    dbg_mod_id_t module_id = 0;

    dbg_modules_t *ptr_module = NULL;
    uint32 inst_num = 0;

    uint32 index    = 0xFFFF;
    uint32 start_no = 0;
    uint32 end_no   = 0;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    module_name = argv[1];
    if(argc > 2)
    {
        CTC_CLI_GET_INTEGER("index", index, argv[2]);
    }

    /*get module id*/
    if(!dbg_get_module_id_by_string(module_name, (uint32 *)&module_id))
    {
        return CLI_ERROR;
    }

    ptr_module = DBG_MOD_GET_INFOPTR(module_id);
    inst_num   = ptr_module->inst_num;
    if(0xFFFF == index)
    {
        start_no    = 0;
        end_no      = inst_num;
    }
    else if(inst_num < index)
    {
        return CLI_ERROR;
    }
    else
    {
        start_no    = index;
        end_no      = index+1;
    }

    for(; start_no < end_no; start_no++)
    {
        /*process*/
        ctc_cli_out("\nChip[%d] %s %d statistics:\n", chip_id, module_name, start_no);
        _module_do(chip_id, ptr_module, start_no);
    }


    return CLI_SUCCESS;
}

static int _block_process(struct cmd_element *self, struct vty *vty, int argc, char **argv)
{
    uint32 chip_id = 0;
    char *block_name = NULL;
    int i = 0;
    int block_cnt = 0;
    dbg_mod_id_t *list_id = NULL;
    int32 list_num = 0;
    dbg_modules_t *ptr_module = NULL;

    uint32 start_inst = 0;
    uint32 inst_num = 0;

    CTC_CLI_GET_INTEGER("chip_id", chip_id, argv[0]);
    block_name = argv[1];

    block_cnt = dbg_get_block_list_num();

    for(i = 0; i < block_cnt; i++)
    {
        if(0 == kal_strcasecmp(block_name, dbg_block_list[i].block_name))
        {
            list_id = dbg_block_list[i].module_id_list;
            list_num = dbg_block_list[i].module_num;
            break;
        }
    }
    if(list_id)
    {
        ctc_cli_out("\nChip[%d] %s statistics:\n", chip_id, block_name);
        for(i = 0; i < list_num; i++)
        {
            ptr_module = DBG_MOD_GET_INFOPTR(list_id[i]);
            inst_num   = ptr_module->inst_num;

            for(start_inst = 0; start_inst < inst_num; start_inst++)
            {
                /*process*/
                ctc_cli_out("\nChip[%d] %s %d statistics:\n", chip_id, ptr_module->module_name, start_inst);
                _module_do(chip_id, ptr_module, start_inst);
            }
        }
    }

    return CLI_SUCCESS;
}

static int32 _ctc_dbg_tool_init_module(int (*func) (struct cmd_element *, struct vty *, int, char **), uint8 *module_map)
{

#define MAX_CLI_LEN         128
#define MAX_CLI_TIPS_LEN    64

    struct cmd_element *cli_cmd = NULL;
    int32 i = 0, j = 0;
    char *cmd_str = NULL;
    char **cli_help = NULL;
    int cnt = 0, tip_node_num = 0;
    char module_name[MAX_CLI_TIPS_LEN] = {0};

    for(i = 0; i < MAX_MOD_NUM; i++)
    {
        if(module_map[i])
        {
            continue;
        }

        cmd_str = kal_malloc(MAX_CLI_LEN);
        if(NULL == cmd_str)
        {

        }
        kal_memset(cmd_str, 0, MAX_CLI_LEN);

        kal_memset(&module_name, 0, MAX_CLI_TIPS_LEN);
        for(j = 0; j < kal_strlen(dbg_modules_list[i].module_name); j++)
        {
            module_name[j] = kal_tolower(dbg_modules_list[i].module_name[j]);
        }

        if(dbg_modules_list[i].inst_num > 1)
        {
            kal_sprintf(cmd_str,"show chip CHIP_ID (%s) (INDEX|)", module_name);
            tip_node_num = 6;
        }
        else
        {
            kal_sprintf(cmd_str,"show chip CHIP_ID (%s)", module_name);
            tip_node_num = 5;
        }

        cli_help = kal_malloc(sizeof(char*)*tip_node_num);
        for(cnt= 0; cnt < tip_node_num; cnt++)
        {
            cli_help[cnt] = kal_malloc(MAX_CLI_TIPS_LEN);
            kal_memset(cli_help[cnt], 0, MAX_CLI_TIPS_LEN);
        }
        cli_help[cnt] = NULL;

        kal_sprintf(cli_help[0], "show %s cmd", module_name);
        kal_sprintf(cli_help[1], "chip");
        kal_sprintf(cli_help[2], "chip id value");
        kal_sprintf(cli_help[3], "%s debug info", module_name);
        if(dbg_modules_list[i].inst_num > 1)
        {
            kal_sprintf(cli_help[4], "index <0-%d>", dbg_modules_list[i].inst_num - 1);
        }

        cli_cmd = kal_malloc(sizeof(struct cmd_element));
        kal_memset(cli_cmd, 0 ,sizeof(struct cmd_element));

        cli_cmd->func   = func;
        cli_cmd->string = cmd_str;
        cli_cmd->doc    = cli_help;

        install_element(CTC_DBG_TOOL_MODE, cli_cmd);
    }

    return CLI_SUCCESS;
}

static int32 _ctc_dbg_tool_init_block(int (*func) (struct cmd_element *, struct vty *, int, char **), uint8 *module_map)
{
#define MAX_CLI_LEN         128
#define MAX_CLI_TIPS_LEN    64
    struct cmd_element *cli_cmd = NULL;
    int32 i = 0, j = 0;
    char *cmd_str = NULL;
    char **cli_help = NULL;
    int cnt = 0, tip_node_num = 0;
    char block_name[MAX_CLI_TIPS_LEN] = {0};

    dbg_mod_id_t *mod_id_list = NULL;
    uint32    mod_id_num = 0;

    uint32 block_cnt = 0;

    block_cnt = dbg_get_block_list_num();

    for(i = 0; i < block_cnt; i++)
    {
        cmd_str = kal_malloc(MAX_CLI_LEN);
        if(NULL == cmd_str)
        {

        }
        kal_memset(cmd_str, 0, MAX_CLI_LEN);

        kal_memset(&block_name, 0, MAX_CLI_TIPS_LEN);
        for(j = 0; j < kal_strlen(dbg_block_list[i].block_name); j++)
        {
            block_name[j] = kal_tolower(dbg_block_list[i].block_name[j]);
        }

        kal_sprintf(cmd_str,"show chip CHIP_ID (%s)", block_name);
        tip_node_num = 5;

        cli_help = kal_malloc(sizeof(char*)*tip_node_num);
        for(cnt= 0; cnt < tip_node_num; cnt++)
        {
            cli_help[cnt] = kal_malloc(MAX_CLI_TIPS_LEN);
            kal_memset(cli_help[cnt], 0, MAX_CLI_TIPS_LEN);
        }
        cli_help[cnt] = NULL;

        kal_sprintf(cli_help[0], "show %s cmd",block_name);
        kal_sprintf(cli_help[1], "chip");
        kal_sprintf(cli_help[2], "chip id value");
        kal_sprintf(cli_help[3], "%s debug info", block_name);


        cli_cmd = kal_malloc(sizeof(struct cmd_element));
        kal_memset(cli_cmd, 0 ,sizeof(struct cmd_element));

        cli_cmd->func   = func;
        cli_cmd->string = cmd_str;
        cli_cmd->doc    = cli_help;

        install_element(CTC_DBG_TOOL_MODE, cli_cmd);

        mod_id_list = dbg_block_list[i].module_id_list;
        mod_id_num  = dbg_block_list[i].module_num;

        for(j = 0 ;j < mod_id_num; j++)
        {
            module_map[mod_id_list[j]] = TRUE;
        }
    }

    return CLI_SUCCESS;
}

/*the following is integrity check*/
static int32
_ctc_dbg_tool_calc_poly(uint8 chip_id, uint32 index ,char *poly_str)
{
    #define MAX_OP_NUM 7
    uint8 op_time = 0;

    uint32 value[MAX_OP_NUM+1] = {0};
    char op_code[MAX_OP_NUM] = {0};
    char tbl_str[64] = {0};
    char fld_str[64] = {0};

    char* op_str = NULL;
    char *head = NULL;

    tbls_id_t tbl_id = 0;
    fld_id_t fld_id = 0;

    uint32 cmd = 0;
    uint32 result = 0;
    uint32 cnt = 0;

    DRV_PTR_VALID_CHECK(poly_str);
    head = kal_strstr(poly_str, "[");
    if(NULL == head)
    {
        kal_sscanf(poly_str, "%d", &result);
        return result;
    }
    else
    {
        while((op_str = kal_strstr(head, "{op ")))
        {
            op_code[op_time] = op_str[4];
            kal_sscanf(head, "\x5b%[^.].%[^\x5d]", tbl_str, fld_str);

            if(!drv_get_tbl_id_by_string(&tbl_id, tbl_str))
            {
                kal_printf("can find table %s\r\n", tbl_str);
                return DRV_E_INVALID_TBL;
            }

            if(!drv_get_field_id_by_string(tbl_id, &fld_id, fld_str))
            {
                kal_printf("can find field %s\r\n", fld_str);
                return DRV_E_INVALID_FLD;
            }

            cmd = DRV_IOR(tbl_id, fld_id);
            if(DRV_E_NONE != drv_ioctl(chip_id, index, cmd, &value[op_time]))
            {
                kal_printf("read tbl_str %s fld_str %s error\n", tbl_str, fld_str);
            }

            head = kal_strstr(op_str, "[");
            kal_memset(tbl_str, 0, sizeof(tbl_str));
            kal_memset(fld_str, 0, sizeof(fld_str));
            op_str = NULL;
            op_time ++;
            if(op_time > MAX_OP_NUM)
            {

            }
        }

        kal_sscanf(head, "\x5b%[^.].%[^\x5d]", tbl_str, fld_str);

        if(!drv_get_tbl_id_by_string(&tbl_id, tbl_str))
        {
            kal_printf("can find table %s\r\n", tbl_str);
            return DRV_E_INVALID_TBL;
        }

        if(!drv_get_field_id_by_string(tbl_id, &fld_id, fld_str))
        {
            kal_printf("can find field %s\r\n", fld_str);
            return DRV_E_INVALID_FLD;
        }

        cmd = DRV_IOR(tbl_id, fld_id);
        if(DRV_E_NONE != drv_ioctl(chip_id, index, cmd, &value[op_time]))
        {
            kal_printf("read tbl_str %s fld_str %s error\n", tbl_str, fld_str);
        }
        cnt    = 0;
        result = value[cnt];

        while(cnt < op_time)
        {
            switch (op_code[cnt])
            {
                case '+':
                    result += value[cnt+1];
                    break;
                case '-':
                    result -= value[cnt+1];
                    break;
                case '&':
                    result &= value[cnt+1];
                    break;
                case '|':
                    result |= value[cnt+1];
                    break;
                default:
                    break;
            }
            cnt++;
        }
    }
    return result;
}


static int32
_ctc_dbg_tool_calc_inter_mask(uint8 chip_id, char *poly_str)
{
    uint32 value[2] = {0};
    char op_code = 0;
    char tbl_str[64] = {0};

    char* op_str = NULL;
    char *head = NULL;
    tbls_id_t tbl_id = 0;
    uint32 result = 0;
    uint32 offset = 0;
    uint32 index  = 0;
    int32 ret = 0;

    DRV_PTR_VALID_CHECK(poly_str);
    head = kal_strstr(poly_str, "[");


    kal_sscanf(head, "[%[^.].%d.%d]", tbl_str, &index, &offset);

    if(!drv_get_tbl_id_by_string(&tbl_id, tbl_str))
    {
        kal_printf("can find table %s\r\n", tbl_str);
        return DRV_E_INVALID_TBL;
    }

    ret = read_entry(chip_id, tbl_id, index, offset, &value[0]);
    if(DRV_E_NONE != ret)
    {
        kal_printf("read tbl_str %s index %d offset %d error\r\n", tbl_str, index, offset);
        return ret;
    }
    //DRV_IF_ERROR_RETURN(read_entry(chip_id, tbl_id, index, offset, &value[0]));

    op_str = kal_strstr(head, "{op ");
    if(NULL != op_str)
    {
        op_code = op_str[4];
        head = kal_strstr(op_str, "[");
        kal_sscanf(head, "[0x%x]", &value[1]);
    }
    else
    {

    }

    result = value[0];
    switch (op_code)
    {
        case '+':
            result += value[1];
            break;
        case '-':
            result -= value[1];
            break;
        case '&':
            result &= value[1];
            break;
        case '|':
            result |= value[1];
            break;
        default:
            break;
    }

    return result;
}

static int32
_ctc_dbg_tool_integrity_golden_do(uint8 chip_id, char *line, char *result)
{
    char *sub_string = NULL;
    char poly_str[MAX_PER_LINE] = {0};

    int32 value = 0;

    if(('#' == line[0]) || (('\r' == line[0]) && ('\n' == line[1])))
    {
        kal_strncpy(result, line, MAX_PER_LINE);
    }
    else
    {
        sub_string = kal_strstr(line, "=");
        if(NULL == sub_string)
        {
            kal_strncpy(result, line, MAX_PER_LINE);
        }
        else
        {
            if(kal_strstr(sub_string, "[same]" ))
            {
                /*get the field value*/
                kal_sscanf(line, "%[^=]=", poly_str);
                value = _ctc_dbg_tool_calc_poly(chip_id, 0, poly_str);
                /*
                if(DRV_E_NONE != value)
                {
                    kal_printf("read poly_str %s \r\n", poly_str);
                    return value;
                }
                */
                kal_sprintf(result, "%s = %d\n", poly_str, value);
            }
            else
            {
                kal_strncpy(result, line, MAX_PER_LINE);
            }
        }
    }

    return DRV_E_NONE;
}



static int32
_ctc_dbg_tool_integrity_result_do(uint8 chip_id, char *line, char *result, uint8 *check_pass)
{
#define CHECK_NONE      0
#define CHECK_STATUS    1
#define CHECK_LINK_LIST 2
#define CHECK_INTR      3

#define OP_NONE         0
#define OP_E            1
#define OP_BE           2
#define OP_LE           3
#define OP_B            4
#define OP_L            5

    uint8 op_code = OP_NONE;

    char poly_str[MAX_PER_LINE] = {0};
    char poly_str_right[MAX_PER_LINE] = {0};

    int32 exp_val = 0;
    int32 rel_val = 0;

    uint32 counter  = 0;
    uint32 index    = 0;
    uint32 next_index = 0;

    uint32 tbl_id = 0;
    uint32 fld_id = 0;
    uint32 cmd = 0;

    static uint8 integrity_stat = CHECK_NONE;
    static uint32 exp_head = 0;
    static uint32 exp_tail = 0;
    static uint32 exp_cnt = 0;

    if(('\r' == line[0]) && ('\n' == line[1]))
    {
        kal_strncpy(result, line, MAX_PER_LINE);
    }
    else if('#' == line[0])
    {
        if(kal_strstr(line, "Status Check"))
        {
            integrity_stat = CHECK_STATUS;
        }
        else if(kal_strstr(line, "Link List Check Begin"))
        {
            integrity_stat = CHECK_LINK_LIST;
            kal_memset(bit_map, 0, sizeof(bit_map));
        }
        else if(kal_strstr(line, "Link List Check End"))
        {
            integrity_stat = CHECK_NONE;
        }
        else if(kal_strstr(line, "Interrupt Check"))
        {
            integrity_stat = CHECK_INTR;
        }

        kal_strncpy(result, line, MAX_PER_LINE);
    }
    else
    {
        /*get op code*/
        if(kal_strstr(line, ">="))
        {
            op_code = OP_BE;
        }
        else if(kal_strstr(line, "<="))
        {
            op_code = OP_LE;
        }
        else if(kal_strstr(line, ">"))
        {
            op_code = OP_B;
        }
        else if(kal_strstr(line, "<"))
        {
            op_code = OP_L;
        }
        else if(kal_strstr(line, "="))
        {
            op_code = OP_E;
        }
        else
        {
            op_code = OP_NONE;
        }

        /*to calc poly*/
        if(OP_NONE == op_code)
        {
            kal_strncpy(result, line, MAX_PER_LINE);
        }
        else
        {
            /*status check*/
            if(CHECK_STATUS == integrity_stat)
            {
                switch(op_code)
                {
                    case OP_E:
                        kal_sscanf(line, "%[^=]=%s", poly_str, poly_str_right);
                        rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                        exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                        if(rel_val != exp_val)
                        {
                            kal_sprintf(result, "%s = %d@%d\n", poly_str, rel_val, exp_val);
                            kal_printf("%s", result);
                            *check_pass = FALSE;
                        }
                        break;
                    case OP_BE:
                        kal_sscanf(line, "%[^>=]>=%s", poly_str, poly_str_right);
                        rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                        exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                        if(rel_val < exp_val)
                        {
                            kal_sprintf(result, "%s >= %d@%d\n", poly_str, rel_val, exp_val);
                            kal_printf("%s", result);
                            *check_pass = FALSE;
                        }
                        break;
                    case OP_LE:
                        kal_sscanf(line, "%[^<=]<=%s", poly_str, poly_str_right);
                        rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                        exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                        if(rel_val > exp_val)
                        {
                            kal_sprintf(result, "%s <= %d@%d\n", poly_str, rel_val, exp_val);
                            kal_printf("%s", result);
                            *check_pass = FALSE;
                        }
                        break;
                    case OP_B:
                        kal_sscanf(line, "%[^>]>%s", poly_str, poly_str_right);
                        rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                        exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                        if(rel_val <= exp_val)
                        {
                            kal_sprintf(result, "%s > %d@%d\n", poly_str, rel_val, exp_val);
                            kal_printf("%s", result);
                            *check_pass = FALSE;
                        }
                        break;
                    case OP_L:
                        kal_sscanf(line, "%[^<]<%s", poly_str, poly_str_right);
                        rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                        exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                        if(rel_val >= exp_val)
                        {
                            kal_sprintf(result, "%s < %d@%d\n", poly_str, rel_val, exp_val);
                            kal_printf("%s", result);
                            *check_pass = FALSE;
                        }
                        break;
                    default:
                        break;
                }

            }
            /*link list check*/
            else if(CHECK_LINK_LIST == integrity_stat)
            {
                if(kal_strstr(line, "{var"))
                {
                    kal_sscanf(line, "%*[^=]=%s", poly_str);
                }
                else
                {

                }

                if(kal_strstr(line, "{var linkHeadPtr}"))
                {
                    exp_head = _ctc_dbg_tool_calc_poly(chip_id, 0, poly_str);
                }
                else if(kal_strstr(line, "{var linkTailPtr}"))
                {
                    exp_tail = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                }
                else if(kal_strstr(line, "{var linkCnt}"))
                {
                    exp_cnt = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                }
                else if(kal_strstr(line, "{var nextPtr}"))
                {
                    counter   = 0;
                    index     = exp_head;
                    /*get tbl & field id*/
                    _ctc_dbg_tool_get_id_by_poly(poly_str, &tbl_id, &fld_id);

                    for(;;)
                    {
                        if(IS_BIT_SET(bit_map[index / 32], (index % 32)))
                        {
                            kal_sprintf(result, "@index (0x%x) looped\n", index);
                            break;
                        }
                        else
                        {
                            SET_BIT(bit_map[index / 32], (index % 32));
                        }
                        counter ++;
                        if ((index == exp_tail) || (counter == exp_cnt))
                        {
                            //kal_printf("index %d, exp_tail %d\n", index, exp_tail);
                            //kal_printf("counter %d, exp_cnt %d\n", counter, exp_cnt);
                            break;
                        }

                        cmd = DRV_IOR(tbl_id, fld_id);
                        drv_ioctl(chip_id, index, cmd, &next_index);
                        index = next_index;
                    }

                    if (next_index != exp_tail)
                    {
                        kal_sprintf(result, "@actual tail = 0x%x, expetc tail = 0x%x\n", next_index, exp_tail);
                        kal_printf("%s", result);
                        *check_pass = FALSE;
                    }
                    else if (counter != exp_cnt)
                    {
                        kal_sprintf(result, "@actual counter = 0x%x, expetc counter = 0x%x\n", counter, exp_cnt);
                        kal_printf("%s", result);
                        *check_pass = FALSE;
                    }
                }
                else
                {
                    switch(op_code)
                    {
                        case OP_E:
                            kal_sscanf(line, "%[^=]=%s", poly_str, poly_str_right);
                            rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                            exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                            if(rel_val != exp_val)
                            {
                                kal_sprintf(result, "%s = %d@%d\n", poly_str, rel_val, exp_val);
                                kal_printf("%s", result);
                                *check_pass = FALSE;
                            }
                            break;
                        case OP_BE:
                            kal_sscanf(line, "%[^>=]>=%s", poly_str, poly_str_right);
                            rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                            exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                            if(rel_val < exp_val)
                            {
                                kal_sprintf(result, "%s >= %d@%d\n", poly_str, rel_val, exp_val);
                                kal_printf("%s", result);
                                *check_pass = FALSE;
                            }
                            break;
                        case OP_LE:
                            kal_sscanf(line, "%[^<=]<=%s", poly_str, poly_str_right);
                            rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                            exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                            if(rel_val > exp_val)
                            {
                                kal_sprintf(result, "%s <= %d@%d\n", poly_str, rel_val, exp_val);
                                kal_printf("%s", result);
                                *check_pass = FALSE;
                            }
                            break;
                        case OP_B:
                            kal_sscanf(line, "%[^>]>%s", poly_str, poly_str_right);
                            rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                            exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                            if(rel_val <= exp_val)
                            {
                                kal_sprintf(result, "%s > %d@%d\n", poly_str, rel_val, exp_val);
                                kal_printf("%s", result);
                                *check_pass = FALSE;
                            }
                            break;
                        case OP_L:
                            kal_sscanf(line, "%[^<]<%s", poly_str, poly_str_right);
                            rel_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str);
                            exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                            if(rel_val >= exp_val)
                            {
                                kal_sprintf(result, "%s < %d@%d\n", poly_str, rel_val, exp_val);
                                kal_printf("%s", result);
                                *check_pass = FALSE;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            /*interrupt check*/
            else if(CHECK_INTR == integrity_stat)
            {
                kal_sscanf(line, "%[^=]=%s", poly_str, poly_str_right);

                rel_val = _ctc_dbg_tool_calc_inter_mask(chip_id, poly_str);
                exp_val = _ctc_dbg_tool_calc_poly(chip_id, 0,poly_str_right);
                if(rel_val != exp_val)
                {
                    kal_sprintf(result, "%s = %d@%d\n", poly_str, rel_val, exp_val);
                    kal_printf("%s", result);
                    *check_pass = FALSE;
                }
            }
            /*none check*/
            else
            {
                exp_head = 0;
                exp_tail = 0;
                exp_cnt  = 0;
            }
        }

    }

    return DRV_E_NONE;
}


int32 ctc_dbg_tool_integrity_check_golden(uint8 chip_id, char *src_file, char *dest_file)
{
    FILE *src_fp = NULL, *dest_fp = NULL;

    char line[MAX_PER_LINE]         = {0};
    char tmp_line[MAX_PER_LINE]         = {0};
    char result[MAX_PER_LINE]       = {0};
    char *rpt_str = NULL, *rpt_end = NULL;

    char pre_str[64]    = {0};
    char sub_str[64]    = {0};
    char value[64]      = {0};
    char oper[8]        = {0};

    char *repeat_line[64]           = {0};
    uint32 repeat_time = 0;
    uint32 i = 0, cn = 0;
    uint32 line_num = 0;

    DRV_PTR_VALID_CHECK(src_file);
    DRV_PTR_VALID_CHECK(dest_file);

    src_fp = fopen(src_file, "r");
    if (NULL == src_fp)
    {
        kal_printf("open source file %s failed!\n", src_file);
        return DRV_E_FILE_OPEN_FAILED;
    }

    dest_fp = fopen(dest_file, "w+");
    if (NULL == dest_fp)
    {
        fclose(src_fp);
        src_fp = NULL;
        kal_printf("open destination file %s failed!\n", dest_file);
        return DRV_E_FILE_OPEN_FAILED;
    }

    /*read to process until EOF*/
    while(NULL != fgets(line, MAX_PER_LINE, src_fp))
    {
        /*process per line*/
        rpt_str = kal_strstr(line, "#BeginRepeat");
        rpt_end = kal_strstr(line, "#EndRepeat");

        if(rpt_str)
        {
            kal_sscanf(rpt_str, "%*[^ ] %d", &repeat_time);
            kal_fprintf(dest_fp, "%s", line);
            kal_memset(line, 0, MAX_PER_LINE);
            line_num    = 0;
        }
        else if(rpt_end)
        {
            for(i = 0; i < repeat_time; i++)
            {
                cn = 0;
                while(cn < line_num)
                {
                    kal_memset(pre_str, 0, sizeof(pre_str));
                    kal_memset(sub_str, 0, sizeof(sub_str));
                    kal_memset(value, 0, sizeof(value));
                    kal_memset(oper, 0, sizeof(oper));
                    kal_memset(tmp_line, 0, sizeof(tmp_line));

                    kal_sscanf(repeat_line[cn], "%[^.].%s%s%s", pre_str, sub_str, oper, value);

                    kal_sprintf(tmp_line, "%s%d.%s %s %s\n", pre_str,  i, sub_str, oper, value);
                    _ctc_dbg_tool_integrity_golden_do(chip_id, tmp_line, result);

                    kal_fprintf(dest_fp, "%s", result);
                    kal_memset(result, 0, MAX_PER_LINE);
                    cn++;
                }
            }
            /*free */
            for(i = 0;i < line_num; i++)
            {
                if(repeat_line[i])
                {
                    kal_free(repeat_line[i]);
                    repeat_line[i] = NULL;
                }
            }
            line_num    = 0;
            repeat_time = 0;
        }
        else
        {
            if(repeat_time)
            {
                repeat_line[line_num] = kal_malloc(MAX_PER_LINE);
                if(repeat_line[line_num])
                {
                    kal_strcpy(repeat_line[line_num], line);
                    line_num ++;
                }
            }
            else
            {
                _ctc_dbg_tool_integrity_golden_do(chip_id, line, result);
                kal_fprintf(dest_fp, "%s", result);
                kal_memset(result, 0, MAX_PER_LINE);
            }
        }
        kal_memset(line, 0, MAX_PER_LINE);

    }

    /*close file*/
    if(NULL != src_fp)
    {
        fclose(src_fp);
        src_fp = NULL;
    }
    if(NULL != dest_fp)
    {
        fclose(dest_fp);
        dest_fp = NULL;
    }

    return DRV_E_NONE;
}

int32 ctc_dbg_tool_integrity_check_result(uint8 chip_id, char *gold_file, char *rlt_file)
{
    DRV_PTR_VALID_CHECK(gold_file);
    DRV_PTR_VALID_CHECK(rlt_file);

    FILE *gold_fp = NULL;
    FILE *rslt_fp = NULL;
    char line[MAX_PER_LINE]     = {0};
    char result[MAX_PER_LINE]   = {0};
    uint8 check_pass = TRUE;

    gold_fp = fopen(gold_file, "r");
    if (NULL == gold_fp)
    {
        kal_printf("open golden file %s failed!\n", gold_file);
        return DRV_E_FILE_OPEN_FAILED;
    }

    rslt_fp = fopen(rlt_file, "w+");
    if (NULL == rslt_fp)
    {
        fclose(gold_fp);
        rslt_fp = NULL;
        kal_printf("open destination file %s failed!\n", rlt_file);
        return DRV_E_FILE_OPEN_FAILED;
    }

    while(NULL != fgets(line, MAX_PER_LINE, gold_fp))
    {
        _ctc_dbg_tool_integrity_result_do(chip_id, line, result, &check_pass);
        kal_fprintf(rslt_fp, "%s", result);

        kal_memset(line, 0, MAX_PER_LINE);
        kal_memset(result, 0, MAX_PER_LINE);
    }

    if(check_pass)
    {
        kal_printf("integrity check passed!\n");
    }
    /*close file*/
    if(NULL != gold_fp)
    {
        fclose(gold_fp);
        gold_fp = NULL;
    }
    if(NULL != rslt_fp)
    {
        fclose(rslt_fp);
        rslt_fp = NULL;
    }

    return DRV_E_NONE;
}

int32 ctc_dbg_tool_init()
{

    uint8 module_map[MAX_MOD_NUM] = {0};

    _ctc_dbg_tool_dbg_info_init(DBG_CFG_FILE);

    _ctc_dbg_tool_init_block(_block_process, module_map);

    _ctc_dbg_tool_init_module(_module_process, module_map);


    return CLI_SUCCESS;
}



