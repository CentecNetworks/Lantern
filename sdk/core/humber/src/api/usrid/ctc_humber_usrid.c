/**
 @file ctc_humber_usrid.c

 @date 2009-11-5

 @version v2.0

*/
/***************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "ctc_error.h"
#include "ctc_humber_usrid.h"
#include "sys_humber_usrid.h"
/***************************************************************
 *
 *  Defines and Macros
 *
 ***************************************************************/
/***************************************************************
 *
 *  Functions
 *
 ***************************************************************/

/**
 @brief initialize the usrid module
*/
int32
ctc_humber_usrid_init(void* usrid_global_cfg)
{
    CTC_ERROR_RETURN(sys_humber_usrid_init());

    return CTC_E_NONE;
}

