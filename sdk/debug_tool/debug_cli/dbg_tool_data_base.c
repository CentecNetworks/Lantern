#include "drv_lib.h"
#include "dbg_tool_data_base.h"

/* MOD regs*/
static tbls_id_t dbg_bufretrv_tbl_id_list[]  = {
};

static tbls_id_t dbg_bufstore_tbl_id_list[]  = {
};

static tbls_id_t dbg_cpumac_tbl_id_list[]  = {
};

static tbls_id_t dbg_dsaging_tbl_id_list[]  = {
};

static tbls_id_t dbg_dynamicds_tbl_id_list[]  = {
};

static tbls_id_t dbg_eloop_tbl_id_list[]  = {
};

static tbls_id_t dbg_epeshell_tbl_id_list[]  = {
};

static tbls_id_t dbg_globalstats_tbl_id_list[]  = {
};

static tbls_id_t dbg_hashds_tbl_id_list[]  = {
};

static tbls_id_t dbg_intlkintf_tbl_id_list[]  = {
};

static tbls_id_t dbg_ipeshellhdradj_tbl_id_list[]  = {
};

static tbls_id_t dbg_ipeshell_tbl_id_list[]  = {
};

static tbls_id_t dbg_macleddriver_tbl_id_list[]  = {
};

static tbls_id_t dbg_macmux_tbl_id_list[]  = {
};

static tbls_id_t dbg_mdio_tbl_id_list[]  = {
};

static tbls_id_t dbg_metfifo_tbl_id_list[]  = {
};

static tbls_id_t dbg_netrx_tbl_id_list[]  = {
};

static tbls_id_t dbg_nettx_tbl_id_list[]  = {
};

static tbls_id_t dbg_oamfwd_tbl_id_list[]  = {
};

static tbls_id_t dbg_oamparser_tbl_id_list[]  = {
};

static tbls_id_t dbg_oamproc_tbl_id_list[]  = {
};

static tbls_id_t dbg_oobfc_tbl_id_list[]  = {
};

static tbls_id_t dbg_parser_tbl_id_list[]  = {
};

static tbls_id_t dbg_pbctl_tbl_id_list[]  = {
};

static tbls_id_t dbg_policing_tbl_id_list[]  = {
};

static tbls_id_t dbg_ptpengine_tbl_id_list[]  = {
};

static tbls_id_t dbg_qmgrdeq_tbl_id_list[]  = {
};

static tbls_id_t dbg_qmgrenq_tbl_id_list[]  = {
};

static tbls_id_t dbg_qmgrqueentry_tbl_id_list[]  = {
};

static tbls_id_t dbg_qsgmii_tbl_id_list[]  = {
};

static tbls_id_t dbg_quadmac_tbl_id_list[]  = {
};

static tbls_id_t dbg_quadpcs_tbl_id_list[]  = {
};

static tbls_id_t dbg_sgmac_tbl_id_list[]  = {
};

static tbls_id_t dbg_sharedds_tbl_id_list[]  = {
};

static tbls_id_t dbg_shellsup_tbl_id_list[]  = {
};

static tbls_id_t dbg_tcamctlint_tbl_id_list[]  = {
};

static tbls_id_t dbg_tcamds_tbl_id_list[]  = {
};


/**
 @brief global modules & registers information
*/
dbg_modules_t dbg_modules_list[]={
   {"BufRetrv",              1,   dbg_bufretrv_tbl_id_list,            sizeof(dbg_bufretrv_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"BufStore",              1,   dbg_bufstore_tbl_id_list,            sizeof(dbg_bufstore_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"CpuMac",                1,   dbg_cpumac_tbl_id_list,              sizeof(dbg_cpumac_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"DsAging",               1,   dbg_dsaging_tbl_id_list,             sizeof(dbg_dsaging_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"DynamicDs",             1,   dbg_dynamicds_tbl_id_list,           sizeof(dbg_dynamicds_tbl_id_list)/sizeof(tbls_id_t)                  },
   {"ELoop",                 1,   dbg_eloop_tbl_id_list,               sizeof(dbg_eloop_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"EpeShell",              1,   dbg_epeshell_tbl_id_list,            sizeof(dbg_epeshell_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"GlobalStats",           1,   dbg_globalstats_tbl_id_list,         sizeof(dbg_globalstats_tbl_id_list)/sizeof(tbls_id_t)                },
   {"HashDs",                1,   dbg_hashds_tbl_id_list,              sizeof(dbg_hashds_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"IntLkIntf",             1,   dbg_intlkintf_tbl_id_list,           sizeof(dbg_intlkintf_tbl_id_list)/sizeof(tbls_id_t)                  },
   {"IpeShellHdrAdj",        1,   dbg_ipeshellhdradj_tbl_id_list,      sizeof(dbg_ipeshellhdradj_tbl_id_list)/sizeof(tbls_id_t)             },
   {"IpeShell",              1,   dbg_ipeshell_tbl_id_list,            sizeof(dbg_ipeshell_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"MacLedDriver",          1,   dbg_macleddriver_tbl_id_list,        sizeof(dbg_macleddriver_tbl_id_list)/sizeof(tbls_id_t)               },
   {"MacMux",                1,   dbg_macmux_tbl_id_list,              sizeof(dbg_macmux_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"Mdio",                  1,   dbg_mdio_tbl_id_list,                sizeof(dbg_mdio_tbl_id_list)/sizeof(tbls_id_t)                       },
   {"MetFifo",               1,   dbg_metfifo_tbl_id_list,             sizeof(dbg_metfifo_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"NetRx",                 1,   dbg_netrx_tbl_id_list,               sizeof(dbg_netrx_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"NetTx",                 1,   dbg_nettx_tbl_id_list,               sizeof(dbg_nettx_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"OamFwd",                1,   dbg_oamfwd_tbl_id_list,              sizeof(dbg_oamfwd_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"OamParser",             1,   dbg_oamparser_tbl_id_list,           sizeof(dbg_oamparser_tbl_id_list)/sizeof(tbls_id_t)                  },
   {"OamProc",               1,   dbg_oamproc_tbl_id_list,             sizeof(dbg_oamproc_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"OobFc",                 1,   dbg_oobfc_tbl_id_list,               sizeof(dbg_oobfc_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"Parser",                1,   dbg_parser_tbl_id_list,              sizeof(dbg_parser_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"PbCtl",                 1,   dbg_pbctl_tbl_id_list,               sizeof(dbg_pbctl_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"Policing",              1,   dbg_policing_tbl_id_list,            sizeof(dbg_policing_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"PtpEngine",             1,   dbg_ptpengine_tbl_id_list,           sizeof(dbg_ptpengine_tbl_id_list)/sizeof(tbls_id_t)                  },
   {"QMgrDeq",               1,   dbg_qmgrdeq_tbl_id_list,             sizeof(dbg_qmgrdeq_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"QMgrEnq",               1,   dbg_qmgrenq_tbl_id_list,             sizeof(dbg_qmgrenq_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"QMgrQueEntry",          1,   dbg_qmgrqueentry_tbl_id_list,        sizeof(dbg_qmgrqueentry_tbl_id_list)/sizeof(tbls_id_t)               },
   {"Qsgmii",                12,  dbg_qsgmii_tbl_id_list,              sizeof(dbg_qsgmii_tbl_id_list)/sizeof(tbls_id_t)                     },
   {"QuadMac",               12,  dbg_quadmac_tbl_id_list,             sizeof(dbg_quadmac_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"QuadPcs",               6,   dbg_quadpcs_tbl_id_list,             sizeof(dbg_quadpcs_tbl_id_list)/sizeof(tbls_id_t)                    },
   {"Sgmac",                 12,  dbg_sgmac_tbl_id_list,               sizeof(dbg_sgmac_tbl_id_list)/sizeof(tbls_id_t)                      },
   {"SharedDs",              1,   dbg_sharedds_tbl_id_list,            sizeof(dbg_sharedds_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"ShellSup",              1,   dbg_shellsup_tbl_id_list,            sizeof(dbg_shellsup_tbl_id_list)/sizeof(tbls_id_t)                   },
   {"TcamCtlInt",            1,   dbg_tcamctlint_tbl_id_list,          sizeof(dbg_tcamctlint_tbl_id_list)/sizeof(tbls_id_t)                 },
   {"TcamDs",                1,   dbg_tcamds_tbl_id_list,              sizeof(dbg_tcamds_tbl_id_list)/sizeof(tbls_id_t)                     },

};

static dbg_mod_id_t dbg_ipe_module_list[] = {

};

static dbg_mod_id_t dbg_epe_module_list[] = {

};

static dbg_mod_id_t dbg_oam_module_list[] = {
   OamParser_m,
   OamProc_m,
   OamFwd_m,
};

static dbg_mod_id_t dbg_q_module_list[] = {
   QMgrDeq_m,
   QMgrEnq_m,
   QMgrQueEntry_m,
};

dbg_block_t dbg_block_list[] = {
    {"IPE-B", dbg_ipe_module_list, sizeof(dbg_ipe_module_list)/sizeof(dbg_mod_id_t)},
    {"EPE-B", dbg_epe_module_list, sizeof(dbg_epe_module_list)/sizeof(dbg_mod_id_t)},
    {"OAM-B", dbg_oam_module_list, sizeof(dbg_oam_module_list)/sizeof(dbg_mod_id_t)},
    {"QMGR-B", dbg_q_module_list, sizeof(dbg_q_module_list)/sizeof(dbg_mod_id_t)},
};

uint32
dbg_get_block_list_num(void)
{
    return sizeof(dbg_block_list)/sizeof(dbg_block_t);
}

uint32
dbg_get_module_list_num(void)
{
    return sizeof(dbg_modules_list)/sizeof(dbg_modules_t);
}


bool
dbg_get_module_id_by_string(char *str, uint32 *id)
{
    uint32 cn = 0;

    if((NULL == str)||(NULL == id))
    {
        return FALSE;
    }

    for(cn =0 ; cn < MAX_MOD_NUM; cn++)
    {
        if(0 == kal_strcasecmp(str, dbg_modules_list[cn].module_name))
        {
            *id = cn;
            return TRUE;
        }
    }

    return FALSE;
}

int32
dbg_register_tcam_info(tbls_id_t id, tcam_mem_ext_content_t *tcam_info)
{
    tables_info_t* ptr = NULL;
    tbls_ext_info_t *new_node = NULL, *head_node = NULL;

    DRV_TBL_ID_VALID_CHECK(id);
    DRV_PTR_VALID_CHECK(tcam_info);

    new_node = kal_malloc(sizeof(tbls_ext_info_t));
    if(NULL == new_node)
    {
        return DRV_E_NO_MEMORY;
    }
    ptr = TABLE_INFO_PTR(id);

    new_node->ext_content_type = EXT_INFO_TYPE_TCAM;
    new_node->ptr_ext_content  = tcam_info;
    new_node->ptr_next = NULL;

    if(NULL == ptr->ptr_ext_info)
    {
        ptr->ptr_ext_info = new_node;
    }
    else
    {
        head_node = ptr->ptr_ext_info;
        ptr->ptr_ext_info = new_node;
        new_node->ptr_next = head_node;
    }

    return DRV_E_NONE;
}

int32
dbg_register_desc_info(tbls_id_t id, entry_desc_t *entry_desc)
{
    tables_info_t* ptr = NULL;
    tbls_ext_info_t *new_node = NULL, *temp_node = NULL;

    DRV_TBL_ID_VALID_CHECK(id);
    DRV_PTR_VALID_CHECK(entry_desc);

    new_node = kal_malloc(sizeof(tbls_ext_info_t));
    if(NULL == new_node)
    {
        return DRV_E_NO_MEMORY;
    }
    ptr = TABLE_INFO_PTR(id);

    new_node->ext_content_type = EXT_INFO_TYPE_DESC;
    new_node->ptr_ext_content = entry_desc;
    new_node->ptr_next = NULL;

    if(NULL == ptr->ptr_ext_info)
    {
        ptr->ptr_ext_info = new_node;
    }
    else
    {
        temp_node = ptr->ptr_ext_info;
        while(temp_node->ptr_next)
        {
            temp_node = temp_node->ptr_next;
        }
        temp_node->ptr_next = new_node;
    }

    return DRV_E_NONE;
}

int32
dbg_register_stat_info(tbls_id_t id)
{
    tables_info_t* ptr = NULL;
    tbls_ext_info_t *new_node = NULL, *temp_node = NULL;

    DRV_TBL_ID_VALID_CHECK(id);
    new_node = kal_malloc(sizeof(tbls_ext_info_t));
    if(NULL == new_node)
    {
        return DRV_E_NO_MEMORY;
    }
    ptr = TABLE_INFO_PTR(id);

    new_node->ext_content_type = EXT_INFO_TYPE_DBG;
    new_node->ptr_ext_content = NULL;
    new_node->ptr_next = NULL;

    if(NULL == ptr->ptr_ext_info)
    {
        ptr->ptr_ext_info = new_node;
    }
    else
    {
        temp_node = ptr->ptr_ext_info;
        while(temp_node->ptr_next)
        {
            temp_node = temp_node->ptr_next;
        }
        temp_node->ptr_next= new_node;
    }

    return DRV_E_NONE;
}

