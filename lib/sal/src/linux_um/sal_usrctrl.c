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

#include <stropts.h>
#include "sal.h"

#define READ_BAY			1
#define WRITE_BAY			0

/*****************************************************************************
 * typedef
 *****************************************************************************/
/* bay */
struct cmdpara_bay_s
{
    uint32_t     chip_id;
    uint32_t	 fpga_id;
    uint32_t     reg_addr;
    uint32_t     value;
};
typedef struct cmdpara_bay_s cmdpara_bay_t;

/*****************************************************************************
 * macros
 *****************************************************************************/

/*****************************************************************************
 * global variables
 *****************************************************************************/
static int32_t ctc_chip_io_fd;

#ifndef BAY_ACCESS_BY_IOCTL
extern int pci_io_write_bay(uint32 bay_id, uint32 reg_offset, uint32 value);
extern int pci_io_read_bay(uint32 bay_id, uint32 reg_offset, uint32 p_value);
#endif

/*****************************************************************************
 * exported functions
 *****************************************************************************/
int32_t
ctckal_usrctrl_init(void)
{
#if _GLB_UML_SYSTEM_
    (void)ctc_chip_io_fd;
    return 0;
#else
    ctc_chip_io_fd = open("/dev/ctc_allctrl",O_RDWR);
    if(ctc_chip_io_fd < 0)
    {
        return -1;
    }
    return ctc_chip_io_fd;
#endif
}
