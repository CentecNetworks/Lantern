/**
 @file ctc_linkagg.h

 @author Copyright (C) 2011 Centec Networks Inc.  All rights reserved.

 @date 2011-12-27

 @version v2.0

This file contains all linkagg related data structure, enum, macro and proto.

*/
#ifndef _CTC_LINKAGG_H
#define _CTC_LINKAGG_H


/****************************************************************
 *
 * Header Files
 *
 ***************************************************************/
#include "kal.h"
#include "ctc_const.h"

/****************************************************************
*
* Defines and Macros
*
****************************************************************/

/**
 @defgroup linkagg LINKAGG
 @{
*/


/**
 @brief linkagg group property
*/
struct ctc_linkagg_s
{
    uint8 tid;      /* linkagg Id */
    uint8 rsv0[2];
};
typedef struct ctc_linkagg_s  ctc_linkagg_t;

#endif
/**@} end of @defgroup   linkagg LINKAGG */


