/*
 *   Octeon Bootbus flash setup
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007, 2008 Cavium Networks
 */
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/ctc5010.h>

enum board_flash_type_e
{
    FLASH_TYPE_1 = 1,   /* 16M x 2 */
    FLASH_TYPE_2,       /* 32M x 1 */
    FLASH_TYPE_3,       /* 32M x 2 */
    FLASH_TYPE_4,       /* 128M x 2 bug21815*/
    FLASH_TYPE_MAX
};
typedef enum board_flash_type_e board_flash_type_t;

extern ctc_board_t ctc_bd_info;
static struct map_info flash_map;
static struct mtd_info *mymtd;
#ifdef CONFIG_MTD_PARTITIONS
static int nr_parts = 2;

static struct mtd_partition parts[] =
{
    {
        .name   = "bootloader",
        .offset = 0x00000000,
        .size   = 0x00100000 /* 1M bytes */
    },
    {
        .name   = "filesystem",
        .offset = 0x00100000,
        .size   = 0x01f00000 /* 31M bytes */
    },
    {
        .name   = "filesystem", /*used for external*/
        .offset = 0x00100000,
        .size   = 0x01f00000 
    }
};
#endif

/**
 * Module/ driver initialization.
 *
 * Returns Zero on success
 */
static int __init flash_init(void)
{
    switch(ctc_bd_info.flash_type)
    {
        case FLASH_TYPE_1:
            flash_map.size = 0x2000000;
            break;
        case FLASH_TYPE_2:
            flash_map.size = 0x2000000;
            break;
        case FLASH_TYPE_3:
            flash_map.size = 0x4000000;
            parts[1].size = 0x3f00000;
            break;
        case FLASH_TYPE_4:
            /*bug21815, support 3 parts for 256M flash*/
            nr_parts = 3;
            flash_map.size = 0x10000000;
            parts[1].offset = 0x00100000;
            parts[1].size = 0x3f00000;
            parts[2].offset = 0x4000000;
            parts[2].size = 0xbb00000;
            break;    
        default:
            printk("Unknow flash type parameters, use default.");
            flash_map.size = 0x2000000;
            break;
    }

    flash_map.name = "phys_mapped_flash";
    /*bug21815. support 256M flash*/
    if(ctc_bd_info.bootbus_updated)
    {
        flash_map.phys = GLB_FLASH_NEW_BASE;
    }
    else
    {
        flash_map.phys = GLB_FLASH_BASE;
    }
    flash_map.bankwidth = 2;
    flash_map.virt = ioremap(flash_map.phys, flash_map.size);
    pr_notice("Bootbus flash: Setting flash for %luMB flash at "
        "0x%08llx\n", flash_map.size >> 20, flash_map.phys);
    simple_map_init(&flash_map);
    mymtd = do_map_probe("cfi_probe", &flash_map);
    if (mymtd) {
        mymtd->owner = THIS_MODULE;

#ifdef CONFIG_MTD_PARTITIONS
    if (nr_parts > 0)
        add_mtd_partitions(mymtd, parts, nr_parts);
    else
        add_mtd_device(mymtd);
#else
    add_mtd_device(mymtd);
#endif
    } else {
        pr_err("Failed to register MTD device for flash\n");
    }
    
	return 0;
}

late_initcall(flash_init);
