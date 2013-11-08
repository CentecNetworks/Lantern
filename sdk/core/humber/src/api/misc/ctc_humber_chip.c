/**
 @file ctc_humber_chip.c

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-10-19

 @version v2.0

 The file contains all chip APIs of ctc layer
*/

/****************************************************************************
 *
* Header Files
*
****************************************************************************/
#include "ctc_debug.h"
#include "ctc_error.h"
#include "ctc_const.h"
#include "ctc_humber_chip.h"
#include "sys_humber_chip.h"
/****************************************************************************
 *
* Defines and Macros
*
*****************************************************************************/

/****************************************************************************
 *
* Global and Declaration
*
*****************************************************************************/

/**
 @brief Initialize the chip module and set the local chip number of the linecard

 @param[in] lchip_num may be 1 or 2

 @return CTC_E_XXX

*/
extern int32
ctc_humber_chip_init(uint8 lchip_num)
{
    CTC_ERROR_RETURN(sys_humber_chip_init(lchip_num));

    return CTC_E_NONE;
}

/**
 @brief The function is to init datapath

 @param[in] chip_datapath   the datapatch datastructure

 @return CTC_E_XXX

*/
int32
ctc_humber_data_path_init(ctc_chip_reset_cb reset_cb, ctc_chip_datapath_t* chip_datapath, char* datapath_config_file)
{
    CTC_ERROR_RETURN(sys_humber_data_path_init(reset_cb, chip_datapath, datapath_config_file));

    return CTC_E_NONE;
}

/**
 @brief The function is to initialize the parity error, it should called after datapath init and mem_profile init

 @return CTC_E_XXX

*/
int32
ctc_humber_parity_error_init(void)
{
    CTC_ERROR_RETURN(sys_humber_parity_error_init());

    return CTC_E_NONE;
}

/**
 @brief The function is to get the local chip number of the linecard

 @param[out] lchip_num  value of local chip num

 @return CTC_E_XXX
*/
int32
ctc_humber_get_local_chip_num(uint8* lchip_num)
{
    *lchip_num = sys_humber_get_local_chip_num();

    return CTC_E_NONE;
}

/**
 @brief The function is to set chip's global chip id

 @param[in] local_chip_id, which chip is to set, maybe 0 or 1

 @param[in] global_chip_id, the value set to, max value is 30

 @return CTC_E_XXX

*/
int32
ctc_humber_set_gchip_id(uint8 lchip_id, uint8 gchip_id)
{
    CTC_ERROR_RETURN(sys_humber_set_gchip_id(lchip_id, gchip_id));

    return CTC_E_NONE;
}

/**
 @brief The function is to get chip's global chip id

 @param[in] lchip_id which chip is to set maybe 0 or 1

 @param[in] gchip_id the value set to max value is 30

 @return CTC_E_XXX

*/
extern int32
ctc_humber_get_gchip_id(uint8 lchip_id, uint8* gchip_id)
{
    CTC_ERROR_RETURN(sys_humber_get_gchip_id(lchip_id, gchip_id));

    return CTC_E_NONE;
}

/**
 @brief The function is to set chip's global cnfig

 @param[in] chip_cfg chip's global cnfig

 @return CTC_E_XXX

*/
extern int32
ctc_humber_set_chip_global_cfg(ctc_chip_global_cfg_t * chip_cfg)
{
    CTC_ERROR_RETURN(sys_humber_set_chip_global_cfg(chip_cfg));
    return CTC_E_NONE;
}

/**
 @brief The function is to get chip's frequency

 @param[out] freq frequency of the chip

 @return CTC_E_XXX
*/
int32
ctc_humber_get_chip_clock(uint16* freq)
{
    CTC_ERROR_RETURN(sys_humber_get_chip_clock(freq));

    return CTC_E_NONE;
}

