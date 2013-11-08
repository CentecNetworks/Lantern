#ifndef _CTC_DBG_TOOL_PROCESS_H_
#define _CTC_DBG_TOOL_PROCESS_H_

#include "kal.h"
#include "ctc_cli.h"

#define TIPS_NONE           0
#define TIPS_TBL_REG        1
#define TIPS_TBL_REG_FIELD  2


int32 show_tips_info(uint8 tips_type, char* inc_str, tbls_id_t id);

int read_entry(uint8 chip_id, tbls_id_t id, uint32 index, uint32 offset, uint32*value);

int write_entry(uint8 chip_id, tbls_id_t id, uint32 index, uint32 offset, uint32 value);

int32 read_tbl_reg(uint8 chip_id, tbls_id_t id, uint32 index, fld_id_t field_id);

int32 write_tbl_reg(uint8 chip_id, tbls_id_t id, uint32 index, fld_id_t field_id, uint32 value);

int32 ctc_dbg_tool_integrity_check_golden(uint8 chip_id, char *src_file, char *dest_file);
int32 ctc_dbg_tool_integrity_check_result(uint8 chip_id, char *gold_file, char *rlt_file);

int32 ctc_dbg_tool_init();

#endif
