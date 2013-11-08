/****************************************************************************
 * linux_user_dal.c :    linux user mode device access abstract
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History :
 * Revision       :         R0.01
 * Author         :         Bo Xiong
 * Date           :         2005-7-28
 * Reason         :         First Create
 * Revision       :         R0.02
 * Author         :         Jian Zhu
 * Date           :         2012-02-08
 * Reason         :         modify for device access abstract
 ****************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/klog.h>

#include "kal.h"
#include "dal_common_io.h"
#include "dal_io.h"

/*****************************************************************************
 * defines
 *****************************************************************************/

#define CTC_ASIC_IO_WRITE_4W 2
#define CTC_ASIC_IO_READ_4W  3

/*****************************************************************************
 * typedef
 *****************************************************************************/

typedef struct para_asic_info_s {
    uint32_t chip_id;
    uint32_t reg_offset;
} para_asic_info_t;

typedef union cmdpara_asic_4w_r_s{
    uint32_t val[4];
    para_asic_info_t asic_info;
}cmdpara_asic_4w_r_t;

typedef struct cmdpara_asic_4w_w_s{
    uint32_t chip_id;
    uint32_t reg_offset;
    uint32_t val[4];
}cmdpara_asic_4w_w_t;

/*****************************************************************************
 * macros
 *****************************************************************************/
#define CMDPARA_ENCODE_CHIP(chip_id_1, reg_addr_1, value_1, para_chip)  \
        para_chip.chip_id	= chip_id_1;                                      \
        para_chip.fpga_id = 0; \
        para_chip.reg_addr = reg_addr_1;                                  \
        para_chip.value = value_1

#define CMDPARA_DECODE_CHIP(chip_id_1, reg_addr_1, value_1, para_chip)  \
        chip_id_1 = para_chip.chip_id;                                      \
        reg_addr_1 = para_chip.reg_addr;                                  \
        value_1 = para_chip.value

#define CHECK_FD(fd)                    if (fd < 0) return E_FD
#define	CHECK_PTR(ptr)                  if (!ptr) return E_PTR
/*****************************************************************************
 * global variables
 *****************************************************************************/
static int32 dal_devfd  = -1;

/*****************************************************************************
 * static functions
 *****************************************************************************/
static int32
dal_usrctrl_do_cmd(uint32 cmd, uint32 p_para)
{
    CHECK_FD(dal_devfd);
    CHECK_PTR(p_para);

    return ioctl(dal_devfd, cmd, p_para);
}

/*****************************************************************************
 * exported functions
 *****************************************************************************/
int32
dal_usrctrl_init(dal_op_t *dal_op)
{
    if (dal_devfd >= 0)
        return 0;

    dal_devfd = open(LINUX_DAL_DEV_NAME, O_RDWR);
    if (dal_devfd < 0)
    {
        perror("Warning: can not open device "LINUX_DAL_DEV_NAME);
        return -1;
    }

    return 0;
}

int32 ctckal_usrctrl_write_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value)
{
#if _GLB_UML_SYSTEM_
        return 0;
#else
    int ret = 0;
    cmdpara_asic_4w_w_t para_asic_4w_w;
    
    para_asic_4w_w.chip_id = chip_id; 
    para_asic_4w_w.reg_offset = reg_offset;
    para_asic_4w_w.val[0] = *(uint32_t *)p_value;
    para_asic_4w_w.val[1] = *((uint32_t *)p_value + 1);
    para_asic_4w_w.val[2] = *((uint32_t *)p_value + 2);
    para_asic_4w_w.val[3] = *((uint32_t *)p_value + 3);
    ret = ioctl(dal_devfd, CTC_ASIC_IO_WRITE_4W, &para_asic_4w_w);
    
    if(ret < 0)
    {
        printf("write chip %d add %x failed\n", chip_id, reg_offset);
    }

    return ret;
#endif    
}

int32
dal_usrctrl_write_chip(uint8 chip_id, uint32 offset, uint32 value)
{
    int32 ret;
    cmdpara_chip_t cmdpara_chip;

    CMDPARA_ENCODE_CHIP(chip_id, offset, value, cmdpara_chip);
    ret = dal_usrctrl_do_cmd(CMD_WRITE_CHIP, (uint32)&cmdpara_chip);
    if (ret)
        KAL_LOG_DEBUG("Write addr :%08X fail, ret:%d\n", offset, ret);

    return ret;
}

int32 ctckal_usrctrl_read_bay_4w(uint32 chip_id, uint32 fpga_id, uint32 reg_offset, uint32 p_value)
{
#if _GLB_UML_SYSTEM_
        return 0;
#else
    int ret;
    cmdpara_asic_4w_r_t para_asic_4w_r;

    para_asic_4w_r.asic_info.chip_id = chip_id;
    para_asic_4w_r.asic_info.reg_offset = reg_offset;        
    ret = ioctl(dal_devfd, CTC_ASIC_IO_READ_4W, &para_asic_4w_r);
    *(uint32*)p_value = para_asic_4w_r.val[0];
    *((uint32*)p_value + 1) = para_asic_4w_r.val[1];
    *((uint32*)p_value + 2) = para_asic_4w_r.val[2];
    *((uint32*)p_value + 3) = para_asic_4w_r.val[3];
    
    if(ret < 0)
    {
        printf("read chip %d add %x failed\n", chip_id, reg_offset);
    }

    return ret;
#endif
}

int32
dal_usrctrl_read_chip(uint8 chip_id, uint32 offset, uint32 p_value)
{
    int32 ret;
    cmdpara_chip_t cmdpara_chip;

    CMDPARA_ENCODE_CHIP(chip_id, offset, 0x0, cmdpara_chip);
    ret = dal_usrctrl_do_cmd(CMD_READ_CHIP, (uint32)&cmdpara_chip);
    *(uint32 *)p_value = cmdpara_chip.value;
    if (ret)
        KAL_LOG_DEBUG("Read addr :%08X fail, ret:%d\n", offset, ret);

    return ret;
}

