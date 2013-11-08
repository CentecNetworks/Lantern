#ifndef _CTC_DBG_TOOL_DATA_BASE_H_
#define _CTC_DBG_TOOL_DATA_BASE_H_

#include "kal.h"
#include "drv_tbl_reg.h"

/* module id check macro define */
enum dbg_modules_id_e
{
   BufRetrv_m,                                                  /*       0 */
   BufStore_m,                                                  /*       1 */
   CpuMac_m,                                                    /*       2 */
   DsAging_m,                                                   /*       3 */
   DynamicDs_m,                                                 /*       4 */
   ELoop_m,                                                     /*       5 */
   EpeShell_m,                                                  /*       6 */
   GlobalStats_m,                                               /*       7 */
   HashDs_m,                                                    /*       8 */
   IntLkIntf_m,                                                 /*       9 */
   IpeShellHdrAdj_m,                                            /*      10 */
   IpeShell_m,                                                  /*      11 */
   MacLedDriver_m,                                              /*      12 */
   MacMux_m,                                                    /*      13 */
   Mdio_m,                                                      /*      14 */
   MetFifo_m,                                                   /*      15 */
   NetRx_m,                                                     /*      16 */
   NetTx_m,                                                     /*      17 */
   OamFwd_m,                                                    /*      18 */
   OamParser_m,                                                 /*      19 */
   OamProc_m,                                                   /*      20 */
   OobFc_m,                                                     /*      21 */
   Parser_m,                                                    /*      22 */
   PbCtl_m,                                                     /*      23 */
   Policing_m,                                                  /*      24 */
   PtpEngine_m,                                                 /*      25 */
   QMgrDeq_m,                                                   /*      26 */
   QMgrEnq_m,                                                   /*      27 */
   QMgrQueEntry_m,                                              /*      28 */
   Qsgmii_m,                                                    /*      29 */
   QuadMac_m,                                                   /*      30 */
   QuadPcs_m,                                                   /*      31 */
   Sgmac_m,                                                     /*      32 */
   SharedDs_m,                                                  /*      33 */
   ShellSup_m,                                                  /*      34 */
   TcamCtlInt_m,                                                /*      35 */
   TcamDs_m,                                                    /*      36 */

   MAX_MOD_NUM,
};
typedef enum dbg_modules_id_e dbg_mod_id_t;

/* module data stucture */
struct dbg_modules_s
{
    char        *module_name;
    uint32      inst_num;
    tbls_id_t    *reg_list_id;
    uint32      id_num;
};
typedef struct dbg_modules_s dbg_modules_t;

/* block data stucture */
struct dbg_block_s
{
    char* block_name;
    dbg_mod_id_t *module_id_list;
    uint32 module_num;
};
typedef struct dbg_block_s dbg_block_t;

#define DBG_MOD_GET_INFOPTR(mod_id)        (&dbg_modules_list[mod_id])
#define DBG_MOD_GET_INFO(mod_id)           (dbg_modules_list[mod_id])
#define DBG_MOD_GET_NAME(mod_id)           (DBG_MOD_GET_INFO(mod_id).module_name)
#define DBG_MOD_GET_INST_NUM(mod_id)       (DBG_MOD_GET_INFO(mod_id).inst_num)
#define DBG_MOD_GET_LIST_ID(mod_id)        (DBG_MOD_GET_INFO(mod_id).reg_list_id)

extern dbg_modules_t dbg_modules_list[];
extern dbg_block_t dbg_block_list[];

extern uint32
dbg_get_block_list_num(void);

extern uint32
dbg_get_module_list_num(void);

/**
 @brief get module id according to module name
*/
extern bool
dbg_get_module_id_by_string(char *str, uint32 *id);

/**
 @brief register a entry desc to the table directory
*/
extern int32
dbg_register_desc_info(tbls_id_t id, entry_desc_t *entry_desc);

/**
 @brief register a tcam info to the register directory
*/
extern int32
dbg_register_tcam_info(tbls_id_t id, tcam_mem_ext_content_t *tcam_info);

/**
 @brief register a debug stat info to the register directory
*/
extern int32
dbg_register_stat_info(tbls_id_t id);

#endif

