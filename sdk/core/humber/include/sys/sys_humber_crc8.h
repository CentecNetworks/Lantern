/**
 @file sys_humber_crc8.h

 @date 2010-03-23

 @version v2.0
*/

#ifndef _SYS_HUMBER_CRC8_H_
#define _SYS_HUMBER_CRC8_H_

/****************************************************************************
 *
* Header Files
*
****************************************************************************/

extern uint8
sys_humber_calculate_crc8(uint8 *data, int32 len, uint8 init_crc);

#endif

