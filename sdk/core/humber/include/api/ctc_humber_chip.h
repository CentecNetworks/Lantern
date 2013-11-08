/**
 @file ctc_humber_chip.h

 @author  Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2009-8-24

 @version v2.0

 The file is deal with chip's info
*/

#ifndef _CTC_HUMBER_CHIP_H
#define _CTC_HUMBER_CHIP_H

/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_chip.h"
/****************************************************************
*
* Defines and Macros
*
****************************************************************/
/****************************************************************************
 *
* Function
*
*****************************************************************************/
/**
 @defgroup chip CHIP
 @{
*/

/**
 @brief Initialize the chip module and set the local chip number of the linecard

 @param[in] lchip_num may be 1 or 2

 @return CTC_E_XXX

*/
extern int32
ctc_humber_chip_init(uint8 lchip_num);

/**
 @brief The function is to init datapath

 @param[in] reset_cb   the chip_reset callback function

 @param[in] chip_datapath   the datapatch datastructure

 @param[in] datapath_config_file   the datapatch config path

 @return CTC_E_XXX

*/
extern int32
ctc_humber_data_path_init(ctc_chip_reset_cb reset_cb, ctc_chip_datapath_t* chip_datapath, char* datapath_config_file);

/**
 @brief The function is to initialize the parity error, it should called after datapath init and mem_profile init

 @return CTC_E_XXX

*/
extern int32
ctc_humber_parity_error_init(void);

/**
 @brief The function is to get the local chip number of the linecard

 @param[out] lchip_num  value of local chip num

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_local_chip_num(uint8* lchip_num);

/**
 @brief The function is to set chip's global chip id

 @param[in] lchip_id which chip is to set maybe 0 or 1

 @param[in] gchip_id the value set to max value is 30

 @return CTC_E_XXX

*/
extern int32
ctc_humber_set_gchip_id(uint8 lchip_id, uint8 gchip_id);

/**
 @brief The function is to get chip's global chip id

 @param[in] lchip_id which chip is to set maybe 0 or 1

 @param[out] gchip_id value of global chip

 @return CTC_E_XXX

*/
extern int32
ctc_humber_get_gchip_id(uint8 lchip_id, uint8* gchip_id);

/**
 @brief The function is to set chip's global cnfig

 @param[in] chip_cfg chip's global cnfig

 @return CTC_E_XXX

*/
extern int32
ctc_humber_set_chip_global_cfg(ctc_chip_global_cfg_t* chip_cfg);

/**
 @brief The function is to get chip's clock

 @param[out] freq frequency of the chip

 @return CTC_E_XXX
*/
extern int32
ctc_humber_get_chip_clock(uint16* freq);

/**@} end of @defgroup   */




#endif




