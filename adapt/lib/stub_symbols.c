/**
 * Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file contains stub symbols in order to build CENTEC binaries
 */

#include <config.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "util.h"
#include "dpif.h"
#include "dummy.h"

#ifdef _OPEN_SOURCE_
char g_current_SerialNo[33] = "OSS_SERIAL_NO";

/*CMODEL function stubs...*/
int16_t generate_mac_hash0(uint8_t * sed, uint32_t bit_num) {return 0;};
int16_t generate_mac_hash1(uint8_t * sed, uint32_t bit_num) {return 0;};
int16_t generate_ipv4_hash0(uint8_t * sed, uint32_t bit_num){return 0;};
int16_t generate_ipv4_hash1(uint8_t * sed, uint32_t bit_num){return 0;};
int16_t generate_ipv6_hash0(uint8_t * sed, uint32_t bit_num){return 0;};
int16_t generate_ipv6_hash1(uint8_t * sed, uint32_t bit_num){return 0;};
int8_t generate_oam_lkup_hash0(uint8_t* sed, uint32_t bit_num,uint8_t crc) {return 0;};
int8_t generate_oam_lkup_hash1(uint8_t* sed, uint32_t bit_num,uint8_t crc) {return 0;};
#endif
