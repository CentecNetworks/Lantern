/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file is the header file of ofp_lib.c
 */

#ifndef __OFP_LIBS_H__
#define __OFP_LIBS_H__
     
/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
 
/**
 * File path to check
 * @param file_path file path
 * @return true or false
 */
bool
ofp_exist_file(char * file_path);

int32_ofp
ctc_swap32(uint32_ofp *data, int32_ofp len, uint8_ofp direction);

uint8_ofp
_ctclib_crc8(uint8_ofp *data, int32_ofp len, uint8_ofp init_crc);

/**
 * Convert string to uint32
 * @param str string
 * @param ret 0: success -1: fail
 * @return value
 */
uint32_ofp
cmd_str2uint(char *str, int32_ofp *ret);

#endif /* __OFP_LIBS_H__ */
