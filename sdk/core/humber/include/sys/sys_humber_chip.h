/**
 @file sys_humber_chip.h

 @date 2009-10-19

 @version v2.0

 The file contains all chip related APIs of sys layer
*/

#ifndef _SYS_HUMBER_CHIP_H
#define _SYS_HUMBER_CHIP_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"
#include "ctc_chip.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/
#define SYS_MAP_GPORT_TO_LPORT(gport, lchip, lport)\
    do{ \
        uint8 gchip;\
        gchip = ((gport >> CTC_LOCAL_PORT_LENGTH)); \
        if(gchip >= 0x1e)\
          return CTC_E_INVALID_GLOBAL_CHIPID;\
        lport = (gport & CTC_LOCAL_PORT_MASK);\
        if (FALSE == sys_humber_chip_is_local(gchip, &lchip))\
        {return CTC_E_CHIP_IS_REMOTE;}\
      }while(0);
      
#define SYS_MAP_GPORT_TO_GCHIP(gport ) ((gport >> CTC_LOCAL_PORT_LENGTH) & 0x1f)

#define SYS_LOCAL_CHIPID_CHECK(lchip ) do{\
 uint8 local_chip_num =  sys_humber_get_local_chip_num();\
 if(lchip >=local_chip_num) return CTC_E_INVALID_LOCAL_CHIPID;\
}while(0);

#define SYS_MAP_GSERDESID_TO_LSERDESID(gserdes_id, lchip, lserdes_id)\
    do{ \
        uint8 gchip;\
        gchip = ((gserdes_id >> CTC_LOCAL_PORT_LENGTH)); \
        if(gchip >= 0x1e)\
          return CTC_E_INVALID_GLOBAL_CHIPID;\
        lserdes_id = (gserdes_id & CTC_LOCAL_PORT_MASK);\
        if (FALSE == sys_humber_chip_is_local(gchip, &lchip))\
        {return CTC_E_CHIP_IS_REMOTE;}\
      }while(0);

/*#define SYS_HUMBER_CHIP_CLOCK 625*/


struct sys_chip_master_s
{
    uint8   lchip_num;
    uint8   resv;
    uint8   g_chip_id[CTC_MAX_LOCAL_CHIP_NUM];
};
typedef struct sys_chip_master_s sys_chip_master_t;


/****************************************************************************
 *
* Function
*
*****************************************************************************/
extern int32
sys_humber_chip_init(uint8 lchip_num);

extern uint8
sys_humber_get_local_chip_num(void);

extern int32
sys_humber_set_gchip_id(uint8 lchip_id, uint8 gchip_id);

extern bool
sys_humber_chip_is_local(uint8 gchip_id, uint8* lchip_id);

extern int32
sys_humber_get_gchip_id(uint8 lchip_id, uint8* gchip_id);

extern int32
sys_humber_data_path_init(ctc_chip_reset_cb reset_cb, ctc_chip_datapath_t* chip_datapath, char* datapath_config_file);

extern int32
sys_humber_parity_error_init(void);

extern int32
sys_humber_set_chip_global_cfg(ctc_chip_global_cfg_t  *chip_cfg);

extern int32
sys_humber_get_chip_clock(uint16* freq);

#endif


