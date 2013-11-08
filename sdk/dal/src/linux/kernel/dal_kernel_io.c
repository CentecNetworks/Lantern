/****************************************************************************
 * linux_kern_dal.c :  linux kernel mode device access abstracts
 *
 * Copyright (C) 2011 Centec Networks Inc.  All rights reserved.
 *
 * Modify History :
 * Revision       :         R0.01
 * Author         :         Jian Zhu
 * Date           :         2012-02-08
 * Reason         :         First Create
 ****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/types.h>
#include <asm/io.h>
#include <linux/pci.h>
#ifdef _CTC_OCTEON_CN50XX_
#include <linux/sched.h>
#include <asm/irq.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#else
#include <asm/uaccess.h>
#endif

#ifndef uint32
typedef uint32_t uint32; /**< 32-bit unsigned integer */
#endif

#ifndef int32
typedef int32_t int32; /**< 32-bit unsigned integer */
#endif

#include "dal_common_io.h"

/*****************************************************************************
 * defines
 *****************************************************************************/
#define CTC_ASIC_VID 0x18e8
#define CTC_ASIC_DID 0x6048

#define CTC_ASIC_CHIP_NUM_MAX 2

/*****************************************************************************
 * typedef
 *****************************************************************************/


/*****************************************************************************
 * macros
 *****************************************************************************/
#ifdef  CTC_LKM_DEBUG
#define CTC_IO_DEBUG(fmt, args...)  printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ## args)
#else
#define CTC_IO_DEBUG(x...)
#endif

/*****************************************************************************
 * global variables
 *****************************************************************************/
static u32 pci_phy_addr[CTC_ASIC_CHIP_NUM_MAX];

static struct pci_device_id linux_dal_table[] =
{
    {PCI_DEVICE(CTC_ASIC_VID, CTC_ASIC_DID)},
    {0,},
};

/*****************************************************************************
 * functions
 *****************************************************************************/
static int
linux_dal_read(unsigned int chip_id, unsigned int reg_offset, unsigned int *val)
{
    unsigned int status;
    int timeout;
    int ret = 0;

    timeout = HUMBER_PCI_ACCESS_TIMEOUT;
    outl(reg_offset, HUMBER_PCI_READ_ADDR + pci_phy_addr[chip_id]);

    /* polling status */
    status = inl(HUMBER_PCI_STATUS + pci_phy_addr[chip_id]);
    while((!(status&0x1))&&(--timeout))
    {
        status = inl(HUMBER_PCI_STATUS + pci_phy_addr[chip_id]);
    }

    if(!timeout)
    {
        printk("Timeout! read chip by pci failed\n");
        ret += -2;
    }

    ret += status&(1<<HUMBER_PCI_STATUS_BAD_PARITY)? -4 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_CPU_ACCESS_ERR)? -8 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_REGISTER_ERR)? -16 : 0;

    if(ret != 0)
    {
        printk("Read chip %d reg 0x%x status  %x, ret %d\n",
                chip_id, reg_offset, status, ret);
    }

    *val = inl(HUMBER_PCI_READ_DATA + pci_phy_addr[chip_id]);

    return ret;
}

static int
linux_dal_write(unsigned int chip_id, unsigned int reg_offset, unsigned int val)
{
    unsigned int status;
    int timeout;
    int ret = 0;

    timeout = HUMBER_PCI_ACCESS_TIMEOUT;
    outl(val, HUMBER_PCI_WRITE_DATA + pci_phy_addr[chip_id]);
    outl(reg_offset, HUMBER_PCI_WRITE_ADDR + pci_phy_addr[chip_id]);

    /* polling status */
    status = inl(HUMBER_PCI_STATUS + pci_phy_addr[chip_id]);
    while((!(status&0x1))&&(--timeout))
    {
        status = inl(HUMBER_PCI_STATUS + pci_phy_addr[chip_id]);
    }

    if(!timeout)
    {
        printk("Timeout! Write chip by PCI failed\n");
        ret += -2;
    }

    ret += status&(1<<HUMBER_PCI_STATUS_BAD_PARITY)? -4 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_CPU_ACCESS_ERR)? -8 : 0;
    ret += status&(1<<HUMBER_PCI_STATUS_REGISTER_ERR)? -16 : 0;
    if(ret != 0)
    {
        printk("Write chip %d reg 0x%x status %x, ret %d\n",
                    chip_id, reg_offset, status, ret);
    }

    return ret;
}

#ifdef _CTC_OCTEON_CN50XX_
static long linux_dal_ioctl (struct file *file,
            unsigned int cmd, unsigned long arg)
#else
static int linux_dal_ioctl (struct inode *inode, struct file *file,
            unsigned int cmd, unsigned long arg)
#endif
{
    int ret = 0;
    cmdpara_chip_t access_para;

    if(copy_from_user(&access_para, (void*)arg, sizeof(cmdpara_chip_t)))
    {
        return -EFAULT;
    }

    if(0 == pci_phy_addr[access_para.chip_id])
    {
        printk("chip %d is not existed\n", access_para.chip_id);
        return -EFAULT;
    }

    switch (cmd)
    {
        case CMD_READ_CHIP:
            linux_dal_read(access_para.chip_id, access_para.reg_addr,
                             &access_para.value);
            break;

        case CMD_WRITE_CHIP:
            linux_dal_write(access_para.chip_id, access_para.reg_addr,
                              access_para.value);
            break;

        default:
            break;
    }

    if(copy_to_user((void*)arg, (void*)&access_para, sizeof(cmdpara_chip_t)))
    {
        return -EFAULT;
    }

    return ret;
}

int linux_dal_probe (struct pci_dev *pdev, const struct pci_device_id *id)
{
    int err;
    u8 bar = 0;

    err = pci_enable_device(pdev);
    if (err)
    {
        printk(KERN_WARNING "Cannot enable PCI device, aborting\n");
        return err;
    }
    if (!(pci_resource_flags(pdev, 0) & IORESOURCE_IO))
    {
        printk(KERN_WARNING "Cannot find proper PCI device phy addr\n");
        err = -ENODEV;
        goto pci_resource_fail;
    }
    err = pci_request_regions(pdev, LINUX_DAL_NAME);
    if (err)
    {
        printk(KERN_WARNING "Cannot obtain PCI resources\n");
        goto pci_resource_fail;
    }

    if(pci_phy_addr[0] != 0)
    {
        pci_phy_addr[1] = pci_resource_start(pdev, bar);
    }
    else
    {
        pci_phy_addr[0] = pci_resource_start(pdev, bar);
    }

    return err;
pci_resource_fail:
    pci_disable_device(pdev);
    return err;
}

void linux_dal_remove (struct pci_dev *pdev)
{
    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
#ifdef _CTC_OCTEON_CN50XX_
    .compat_ioctl = linux_dal_ioctl,
#else
    .ioctl = linux_dal_ioctl,
#endif
};

static struct pci_driver linux_dal =
{
    .name = LINUX_DAL_NAME,
    .id_table = linux_dal_table,
    .probe = linux_dal_probe,
    .remove = linux_dal_remove,
};

static int __init linux_dal_init(void)
{
    int ret;

    ret = register_chrdev(LINUX_DAL_DEV_MAJOR, "linux_dal", &fops);
    if (ret<0)
    {
        printk(KERN_WARNING "Register linux_dal device, ret %d\n", ret);
        return ret;
    }

    ret = pci_register_driver(&linux_dal);
    if (ret<0)
    {
        printk(KERN_WARNING "Register ASIC PCI driver failed, ret %d\n", ret);
        return ret;
    }

    return ret;
}

static void __exit linux_dal_exit(void)
{
    unregister_chrdev(LINUX_DAL_DEV_MAJOR, "linux_dal");
    pci_unregister_driver(&linux_dal);
}

module_init(linux_dal_init);
module_exit(linux_dal_exit);

