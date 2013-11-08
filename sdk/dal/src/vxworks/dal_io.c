/****************************************************************************
 * other_dal.c :    device access abstract for unknow OS
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History :
 * Revision       :         R0.01
 * Author         :         Jian Zhu
 * Date           :         2012-03-21
 * Reason         :         First Create
 ****************************************************************************/
#include "kal.h"
#include "dal_common_io.h"
#include "dal_io.h"

/*****************************************************************************
 * defines
 *****************************************************************************/

/*****************************************************************************
 * typedef
 *****************************************************************************/


/*****************************************************************************
 * macros
 *****************************************************************************/

/*****************************************************************************
 * global variables
 *****************************************************************************/
static dal_op_t p_dal_op;

/*****************************************************************************
 * static functions
 *****************************************************************************/

/*****************************************************************************
 * exported functions
 *****************************************************************************/
int32
dal_usrctrl_init(dal_op_t *dal_op)
{
    if(NULL == dal_op || NULL == dal_op->pci_read || NULL == dal_op->pci_write)
        return -1;

    p_dal_op.pci_read = dal_op->pci_read;
    p_dal_op.pci_write = dal_op->pci_write;

    return 0;
}

int32
dal_usrctrl_read_chip(uint8 chip_id, uint32 reg_offset, uint32 p_value)
{
    uint32 status;
    int32 timeout;
    int32 ret = 0;

    timeout = HUMBER_PCI_ACCESS_TIMEOUT;
    p_dal_op.pci_write(chip_id, HUMBER_PCI_READ_ADDR, reg_offset);

    /* polling status */
    status = p_dal_op.pci_read(chip_id, HUMBER_PCI_STATUS);
    while((!(status&0x1))&&(--timeout))
    {
        status = p_dal_op.pci_read(chip_id, HUMBER_PCI_STATUS);
    }

    if(!timeout)
    {
        ret += -2;
    }

    ret += status&(1<<HUMBER_PCI_STATUS_BAD_PARITY)? -4 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_CPU_ACCESS_ERR)? -8 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_REGISTER_ERR)? -16 : 0;

    *(uint32 *)p_value = p_dal_op.pci_read(chip_id, HUMBER_PCI_READ_DATA);

    return ret;
}


int32
dal_usrctrl_write_chip(uint8 chip_id, uint32 reg_offset, uint32 value)
{
    unsigned int status;
    int timeout;
    int ret = 0;

    timeout = HUMBER_PCI_ACCESS_TIMEOUT;

    p_dal_op.pci_write(chip_id, HUMBER_PCI_WRITE_DATA, value);
    p_dal_op.pci_write(chip_id, HUMBER_PCI_WRITE_ADDR, reg_offset);

    /* polling status */
    status = p_dal_op.pci_read(chip_id, HUMBER_PCI_STATUS);
    while((!(status&0x1))&&(--timeout))
    {
        status = p_dal_op.pci_read(chip_id, HUMBER_PCI_STATUS);
    }

    if(!timeout)
    {
        ret += -2;
    }

    ret += status&(1<<HUMBER_PCI_STATUS_BAD_PARITY)? -4 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_CPU_ACCESS_ERR)? -8 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_REGISTER_ERR)? -16 : 0;

    return ret;
}

