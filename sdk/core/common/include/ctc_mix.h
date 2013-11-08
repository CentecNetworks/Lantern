/**
 @file ctc_mix.h

 @date 2009-10-27

 @version v2.0

The file define  chipset  independent common Macros and constant.
*/


#ifndef _CTC_MIX_H
#define _CTC_MIX_H




#ifdef HUMBER

#define CTC_MAX_CHIP_NUM_PER_LC         2
#define CTC_MAX_LOCAL_CHIP_NUM          2
#define CTC_MAX_PHY_PORT                52
#define CTC_MAX_HUMBER_CHIP_NUM         32
#define CTC_MAX_HUMBER_PHY_PORT         52
#define CTC_MAX_VRFID_ID                65535       /**< Max vrfid id */
#define CTC_MAX_LINKAGG_GROUP_NUM       128         /**< Max Linkagg group num */
#define MAX_PORT_NUM_PER_CHIP           256         /**< Max port num per chip */
#endif




#endif /* _CTC_MIX_H*/

