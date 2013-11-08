/****************************************************************************
 *file ctc_debug.h

 *author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 *date 2009-11-26

 *version v2.0

  This file contains  debug header file.
 ****************************************************************************/

#ifndef _CTC_DEBUG_H_
#define _CTC_DEBUG_H_

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_linklist.h"


#define CTC_DEBUG_NAME_LEN 31   /**< Name length of debug module */


/* debug struct */
 struct ctc_debug_list_s
{
    unsigned char flags;            		/* debug flag */
    unsigned char level;            		/* debug level */
    unsigned short rsv;              		/* reserved */
    char module[CTC_DEBUG_NAME_LEN+1];      /**< Debug module */
    char submodule[CTC_DEBUG_NAME_LEN+1];   /**< Debug submodule */
};
typedef struct ctc_debug_list_s ctc_debug_list_t;

enum ctc_debug_level_e
{
   CTC_DEBUG_LEVEL_NONE     = 0x00,
   CTC_DEBUG_LEVEL_FUNC     = 0x01,
   CTC_DEBUG_LEVEL_PARAM   	= 0x02,
   CTC_DEBUG_LEVEL_INFO     = 0x04,
   CTC_DEBUG_LEVEL_ERROR    = 0x08,
   CTC_DEBUG_LEVEL_DUMP     = 0x10
};
typedef enum ctc_debug_level_e ctc_debug_level_t;

#define CTC_DEBUG_IMPLEMENT(mod, sub)     \
ctc_debug_list_t ctc_##mod##_##sub##_debug_handle =         \
{                                                   \
    0x00,                              \
    0x00,                              \
    0x00,                              \
    #mod,        \
    #sub                 \
}

#ifdef SDK_IN_DEBUG_VER

#define CTC_CONSOLE_DEBUG_PRINT(fmt, args...)                        \
{                                                              \
    FILE * fp_console = NULL;                                  \
    fp_console = fopen("/dev/console", "a+");                  \
    fprintf(fp_console, fmt, ##args);                      \
    fclose(fp_console);                                        \
}

#define CTC_DEBUG_OUT_DUMP(fmt, args...)            \
{ \
    ;                        \
}

#define CTC_DEBUG_PRINT_TEMP_FILE(fmt, args...)                 \
{                                                               \
    FILE * fp_console = NULL;                                   \
    fp_console = fopen("/tmp/temp_sdk_debug_out.txt", "a+");            \
    kal_fprintf(fp_console, fmt, ##args);                       \
    fclose(fp_console);                                         \
}

#define CTC_DEBUG_OUT_INFO(mod, sub, typeenum, fmt, args...) \
{                                                                               \
    if (ctc_debug_check_flag(ctc_##mod##_##sub##_debug_handle, typeenum,CTC_DEBUG_LEVEL_INFO)) \
    {                                                                           \
        CTC_DEBUG_PRINT_TEMP_FILE(fmt, ##args);\
    }                                                                           \
}

#define CTC_DEBUG_OUT_FUNC(mod, sub, typeenum) \
{                                                                               \
    if (ctc_debug_check_flag(ctc_##mod##_##sub##_debug_handle, typeenum,CTC_DEBUG_LEVEL_INFO)) \
    {                                                                           \
        CTC_DEBUG_PRINT_TEMP_FILE("\n%s()\n", __FUNCTION__);\
    }                                                                           \
}

#define CTC_DEBUG_OUT(mod, sub, typeenum,level, fmt, args...) \
{                                                                               \
    if (ctc_debug_check_flag(ctc_##mod##_##sub##_debug_handle, typeenum,level)) \
    {                                                                           \
         CTC_DEBUG_PRINT_TEMP_FILE(fmt, ##args);\
    }                                                                           \
}

#else
#define CTC_DEBUG_OUT_DUMP(fmt, args...)
#define CTC_DEBUG_OUT_INFO(mod, sub, typeenum, fmt, args...)
#define CTC_DEBUG_OUT_FUNC(mod, sub, typeenum)
#define CTC_DEBUG_OUT(mod, sub, typeenum,level, fmt, args...)

#endif /* CTC_DEBUG */



/**********************************************************************************
                        Define module define debug enum
 * NOTE:
* 1, pattern: CTC_DEBUG_ENUM(module, submodule, debug_enum0, debug_enum1...);
* 2, module and sub_module name length should less than 31 characters
* 3, different module can't define the same enum
 ***********************************************************************************/

/* define debug enum for each mode */
#define CTC_DEBUG_ENUM(mod, sub, typeenum, ...)       \
  extern ctc_debug_list_t ctc_##mod##_##sub##_debug_handle; \
  enum ctc_##mod##_##sub##_debug_enum                          \
  {                                                   \
      typeenum = 0,                                   \
      __VA_ARGS__                                     \
  }

/*acl/qos module*/
CTC_DEBUG_ENUM(qos, process, QOS_PROC_CTC, QOS_PROC_SYS);
CTC_DEBUG_ENUM(qos, queue,  QOS_QUE_CTC, QOS_QUE_SYS);
CTC_DEBUG_ENUM(acl, acl,  ACL_CTC, ACL_SYS);
CTC_DEBUG_ENUM(cpu, traffic,  CPU_TRAFFIC_CTC, CPU_TRAFFIC_SYS);

CTC_DEBUG_ENUM(aclqos, label, ACLQOS_LABEL_CTC, ACLQOS_LABEL_SYS);
CTC_DEBUG_ENUM(aclqos, entry, ACLQOS_ENTRY_CTC, ACLQOS_ENTRY_SYS);
CTC_DEBUG_ENUM(qos, policer,  QOS_PLC_CTC, QOS_PLC_SYS);
CTC_DEBUG_ENUM(qos, class,  QOS_CLASS_CTC, QOS_CLASS_SYS);

/* L3  module*/
CTC_DEBUG_ENUM(l3if, l3if, L3IF_CTC,  L3IF_SYS);
CTC_DEBUG_ENUM(ipuc, ipuc,  IPUC_CTC, IPUC_SYS);
CTC_DEBUG_ENUM(ipmc, ipmc, IPMC_CTC, IPMC_SYS);
CTC_DEBUG_ENUM(mcast, mcast, MCAST_CTC, MCAST_SYS);
CTC_DEBUG_ENUM(mpls, mpls, MPLS_CTC, MPLS_SYS);

/* L2  module*/

CTC_DEBUG_ENUM(l2, fdb,  L2_FDB_CTC, L2_FDB_SYS);
CTC_DEBUG_ENUM(l2, learning_aging,  L2_LEARNING_AGING_CTC, L2_LEARNING_AGING_SYS);
CTC_DEBUG_ENUM(l2, stp,  L2_STP_CTC, L2_STP_SYS);
CTC_DEBUG_ENUM(vlan, vlan,  VLAN_CTC, VLAN_SYS);
CTC_DEBUG_ENUM(vlan, vlan_class, VLAN_CLASS_CTC, VLAN_CLASS_SYS);
CTC_DEBUG_ENUM(vlan, vlan_mapping,  VLAN_MAPPING_CTC, VLAN_MAPPING_SYS);
CTC_DEBUG_ENUM(vlan, vlan_switching, VLAN_SWITCHING_CTC, VLAN_SWITCHING_SYS);
CTC_DEBUG_ENUM(vlan, protocol_vlan, PRO_VLAN_CTC, PRO_VLAN_SYS);
CTC_DEBUG_ENUM(dot1x, dot1x,  DOT1X_CTC, DOT1X_SYS);
CTC_DEBUG_ENUM(mirror, mirror, MIRROR_CTC, MIRROR_SYS);
CTC_DEBUG_ENUM(security, security, SECURITY_CTC, SECURITY_SYS);
CTC_DEBUG_ENUM(oam, cfm, OAM_CFM_CTC, OAM_CFM_SYS);
CTC_DEBUG_ENUM(oam, efm, OAM_EFM_CTC, OAM_EFM_SYS);
CTC_DEBUG_ENUM(oam, pbx, OAM_PBX_CTC, OAM_PBX_SYS);
CTC_DEBUG_ENUM(ptp, ptp, PTP_PTP_CTC, PTP_PTP_SYS);

/*interrupt module*/
CTC_DEBUG_ENUM(interrupt, interrupt, INTERRUPT_CTC, INTERRUPT_SYS);


/* resource module*/
CTC_DEBUG_ENUM(chip, chip, CHIP_CTC, CHIP_SYS);
CTC_DEBUG_ENUM(port, port, PORT_CTC, PORT_SYS);
CTC_DEBUG_ENUM(linkagg, linkagg, LINKAGG_CTC, LINKAGG_SYS);
CTC_DEBUG_ENUM(nexthop, nexthop, NH_CTC, NH_SYS);
CTC_DEBUG_ENUM(sacl, sacl, SACL_CTC,  SACL_SYS);
CTC_DEBUG_ENUM(alloc, alloc, ALLOC_CTC, ALLOC_SYS);
CTC_DEBUG_ENUM(opf, opf, OPF_SYS);
CTC_DEBUG_ENUM(memmngr, memmngr, MEMMNGR_CTC);

CTC_DEBUG_ENUM(stats, stats,  STATS_CTC, STATS_SYS);

CTC_DEBUG_ENUM(parser, parser, PARSER_CTC,  PARSER_SYS);
CTC_DEBUG_ENUM(pdu, pdu, PDU_CTC,  PDU_SYS);
CTC_DEBUG_ENUM(learning_aging, learning_aging, LEARNING_AGING_CTC,  LEARNING_AGING_SYS);

/**********************************************************************************
                      Define API function interfaces
***********************************************************************************/
extern int32
ctc_debug_init(void);
extern int32
ctc_debug_enable (bool enable);
extern bool
ctc_get_debug_enable (void);

extern int32
 ctc_debug_clear_all_flag (void);
extern int32
ctc_debug_set_flag(char *module,char *submodule, uint32 typeenum,uint8 level,bool debug_on);
extern bool
ctc_debug_get_flag(char *module,char *submodule, uint32 typeenum,uint8 *level);

extern bool
ctc_debug_check_flag(ctc_debug_list_t flag_info, uint32 typeenum,uint8 level);

#endif /* _CTC_DEBUG_H_ */

