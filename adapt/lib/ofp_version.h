/**
 *  Copyright (C) 2011, 2012 CentecNetworks, Inc. All Rights Reserved.
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
 * @brief This file defines the version information of the software package
 */

#ifndef _OFP_VERSION_H_
#define _OFP_VERSION_H_

#include "glb_hw_define.h"

extern uint8 g_current_SerialNo[MAX_BOOTROM_VER_LEN + 1];

#define OPENFLOW_HW_MFR_DESC "Centec Networks Inc., (SIP, Suzhou, China)"
#define OPENFLOW_HW_DESC "V330-52TX-RD"
#define OPENFLOW_CTC_VERSION_STR "Centec VCOS"
#define OPENFLOW_CTC_VERSION          "1.0"
#define OPENFLOW_CTC_DP_DESC "ASIC based data plane  "
#define OPENFLOW_CTC_SERIAL_DESC g_current_SerialNo
#define CLI_OFP_COPYRIGHT_INFO "Copyright (C) 2013 Centec Networks Inc.  All rights reserved."

#endif /* _OFP_VERSION_H_ */
