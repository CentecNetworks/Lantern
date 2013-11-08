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
 */

#ifndef __GLB_EEPROM_DEFINE_H__
#define __GLB_EEPROM_DEFINE_H__

enum eeprom_info_list_e
{
    EEPROM_SERIAL_PARAM = 0,
    EEPROM_BOOTVER,
    EEPROM_BOOTCMD,
    EEPROM_BOOTUP,
    EEPROM_SERIALNO,
    EEPROM_SYS_MAC,
    EEPROM_DATAPATH,
    EEPROM_TCAM_PROFILE,
    EEPROM_BR_PROFILE,
    EEPROM_0_MANU_TEST,
    EEPROM_1_MANU_TEST,
#ifdef BOOTUP_DIAG    
    EEPROM_BOOTUP_DIAG_LEVEL,
#endif    
    EEPROM_CHASSIS_TYPE,
    EEPROM_BP_CHASSIS_TYPE,
    EEPROM_BP_SYS_MAC,
    EEPROM_LC_ERR_REBOOT,
    EEPROM_CHASSIS_FAN_MANAGE,
    EEPROM_OEM_INFO,
    EEPROM_FREQ_TYPE,
#ifdef HAVE_SMARTCFG /* added by liuyang 2011-10-27 */
    EEPROM_SMARTCFG,
#endif /* !HAVE_SMARTCFG */
    EEPROM_MAX
};
typedef enum eeprom_info_list_e eeprom_info_list_t;

struct eeprom_info_s
{
   uint8 eeprom_idx;
   uint8 reserved;
   uint16 base_addr;
};
typedef struct eeprom_info_s eeprom_info_t;


#endif
