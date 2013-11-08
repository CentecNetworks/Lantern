/**
 * Driver for the Octeon NAND flash controller introduced in CN52XX pass 2.
 *
 * LICENSE:
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Cavium Networks
 */

#include <linux/platform_device.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/irq.h>

#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-nand.h>
#include <asm/octeon/octeon.h>

#define DRIVER_NAME "octeon-nand"

#define DEBUG_INIT		(1<<0)
#define DEBUG_READ		(1<<1)
#define DEBUG_READ_BUFFER	(1<<2)
#define DEBUG_WRITE		(1<<3)
#define DEBUG_WRITE_BUFFER	(1<<4)
#define DEBUG_CONTROL		(1<<5)
#define DEBUG_SELECT		(1<<6)
#define DEBUG_ALL		-1

#define MAX_NAND_NAME_LEN       20

#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probes[] = { "cmdlinepart", NULL };
static int nr_parts;
static struct mtd_partition *mtd_parts = 0;
#endif

#define DEV_DBG(level, dev, format, arg...)			\
	if (unlikely(debug & level))				\
		dev_info(dev , "%s " format , __func__, ## arg)

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Debug bit field. -1 will turn on all debugging.");

static struct platform_device *octeon_nand_pdev;
static struct mtd_info *octeon_nand_open_mtd[8];

struct octeon_nand_priv {
	/* Temporary location to store read data, must be 64 bit aligned */
	uint8_t data[NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE];
	int data_len;		/* Number of byte in the data buffer */
	int data_index;		/* Current read index. Equal to data_len when
					all data has been read */
	int selected_chip;	/* Currently selected NAND chip */
	int selected_page;	/* Last page chosen by SEQIN for PROGRAM */
	struct platform_device *pdev;	/* Pointer to the platform device */
};


/**
 * Read a single byte from the temporary buffer. Used after READID
 * to get the NAND information.
 *
 * @param mtd
 *
 * @return
 */
static uint8_t octeon_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;

	if (priv->data_index < priv->data_len) {
		DEV_DBG(DEBUG_READ, &priv->pdev->dev, "read of 0x%02x\n",
			0xff & priv->data[priv->data_index]);
		return priv->data[priv->data_index++];
	} else {
		dev_err(&priv->pdev->dev, "No data to read\n");
		return 0xff;
	}
}


/**
 * Read two bytes from the temporary buffer. Used after READID to
 * get the NAND information on 16 bit devices.
 *
 * @param mtd
 *
 * @return
 */
static uint16_t octeon_nand_read_word(struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;

	if (priv->data_index + 1 < priv->data_len) {
		uint16_t result = le16_to_cpup((uint16_t *)(priv->data +
			priv->data_index));
		priv->data_index += 2;
		DEV_DBG(DEBUG_READ, &priv->pdev->dev, "read of 0x%04x\n",
			0xffff & result);
		return result;
	} else {
		dev_err(&priv->pdev->dev, "No data to read\n");
		return 0xff;
	}
	return 0;
}


/**
 * Since we have a write page, I don't think this can ever be
 * called.
 *
 * @param mtd
 * @param buf
 * @param len
 */
static void octeon_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf,
				int len)
{
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;

	DEV_DBG(DEBUG_WRITE_BUFFER, &priv->pdev->dev, "len=%d\n", len);

	memcpy(priv->data + priv->data_index, buf, len);
	priv->data_index += len;
	priv->data_len += len;
	/* Linux sometimes thinks there is less OOB data than the chip really
		has. Make sure all OOB is set to 0xff */
	memset(priv->data + priv->data_index, 0xff,
		sizeof(priv->data) - priv->data_index);
}


/**
 * Read a number of pending bytes from the temporary buffer. Used
 * to get page and OOB data.
 *
 * @param mtd
 * @param buf
 * @param len
 */
static void octeon_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;

	DEV_DBG(DEBUG_READ_BUFFER, &priv->pdev->dev, "len=%d\n", len);

	if (len <= priv->data_len - priv->data_index) {
		memcpy(buf, priv->data + priv->data_index, len);
		priv->data_index += len;
	} else {
		dev_err(&priv->pdev->dev,
			"Not enough data for read of %d bytes\n", len);
		priv->data_len = 0;
	}
}


/**
 * Verify the supplied buffer matches the data we last read
 *
 * @param mtd
 * @param buf
 * @param len
 *
 * @return
 */
static int octeon_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf,
				int len)
{
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;
	if (memcmp(buf, priv->data, len)) {
		dev_err(&priv->pdev->dev, "Write verify failed\n");
		return -EFAULT;
	} else
		return 0;
}


/**
 * Select which NAND chip we are working on. A chip of -1
 * represents that no chip should be selected.
 *
 * @param mtd
 * @param chip
 */
static void octeon_nand_select_chip(struct mtd_info *mtd, int chip)
{
	/* We don't need to do anything here */
}


/**
 * Issue a NAND command to the chip. Almost all work is done here.
 *
 * @param mtd
 * @param command
 * @param column
 * @param page_addr
 */
static void octeon_nand_cmdfunc(struct mtd_info *mtd, unsigned command,
				int column, int page_addr)
{
	int status;
	struct nand_chip *nand = mtd->priv;
	struct octeon_nand_priv *priv = nand->priv;

	switch (command) {
	case NAND_CMD_READID:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev, "READID\n");
		priv->data_index = 0;
		/* Read length must be a multiple of 8, so read a little more than
		** we require, */
		priv->data_len = cvmx_nand_read_id(priv->selected_chip, 0,
						virt_to_phys(priv->data), 16);
		if (priv->data_len < 16) {
			dev_err(&priv->pdev->dev, "READID failed with %d\n",
				priv->data_len);
			priv->data_len = 0;
		}
		break;

	case NAND_CMD_READOOB:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev,
			"READOOB page_addr=0x%x\n", page_addr);
		priv->data_index = 8;
		/* Read length must be a multiple of 8, so we start reading
		** 8 bytes from the end of page. */
		priv->data_len = cvmx_nand_page_read(priv->selected_chip,
					(page_addr << nand->page_shift) +
					(1<<nand->page_shift) - priv->data_index,
					virt_to_phys(priv->data),
					mtd->oobsize + priv->data_index);
		if (priv->data_len < mtd->oobsize + priv->data_index) {
			dev_err(&priv->pdev->dev, "READOOB failed with %d\n",
				priv->data_len);
			priv->data_len = 0;
		}
		break;

	case NAND_CMD_READ0:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev,
			"READ0 page_addr=0x%x\n", page_addr);
		priv->data_index = 0;
		/* Here mtd->oobsize _must_ already be a multiple of 8 */
		priv->data_len = cvmx_nand_page_read(priv->selected_chip,
					column +
					(page_addr << nand->page_shift),
					virt_to_phys(priv->data),
					(1 << nand->page_shift) +
					mtd->oobsize);
		if (priv->data_len < (1 << nand->page_shift) + mtd->oobsize) {
			dev_err(&priv->pdev->dev, "READ0 failed with %d\n",
				priv->data_len);
			priv->data_len = 0;
		}
		break;

	case NAND_CMD_ERASE1:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev,
			"ERASE1 page_addr=0x%x\n", page_addr);
		if (cvmx_nand_block_erase(priv->selected_chip,
			page_addr << nand->page_shift)) {
			dev_err(&priv->pdev->dev, "ERASE1 failed\n");
		}
		break;

	case NAND_CMD_ERASE2:
		/* We do all erase processing in the first command, so ignore
			this one */
		break;

	case NAND_CMD_STATUS:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev, "STATUS\n");
		priv->data_index = 0;
		priv->data_len = 2;
		priv->data[0] = cvmx_nand_get_status(priv->selected_chip);
		priv->data[1] = priv->data[0];
		break;

	case NAND_CMD_SEQIN:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev,
			"SEQIN column=%d page_addr=0x%x\n", column, page_addr);
		/* If we don't seem to be doing sequential writes then erase
			all data assuming it is old */
		if (priv->data_index != column)
			memset(priv->data, 0xff, sizeof(priv->data));
		priv->data_index = column;
		priv->data_len = column;
		priv->selected_page = page_addr;
		break;

	case NAND_CMD_PAGEPROG:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev, "PAGEPROG\n");
		status = cvmx_nand_page_write(priv->selected_chip,
			priv->selected_page << nand->page_shift,
			virt_to_phys(priv->data));
		if (status)
			dev_err(&priv->pdev->dev, "PAGEPROG failed with %d\n",
				status);
		break;

	case NAND_CMD_RESET:
		DEV_DBG(DEBUG_CONTROL, &priv->pdev->dev, "RESET\n");
		priv->data_index = 0;
		priv->data_len = 0;
		memset(priv->data, 0xff, sizeof(priv->data));
		status = cvmx_nand_reset(priv->selected_chip);
		if (status)
			dev_err(&priv->pdev->dev, "RESET failed with %d\n",
				status);
		break;

	default:
		dev_err(&priv->pdev->dev, "Unsupported command 0x%x\n",
			command);
		break;
	}
}


/**
 * Determine what NAND devices are available
 *
 * @param pdev
 *
 * @return
 */
static int octeon_nand_probe(struct platform_device *pdev)
{
	struct mtd_info *mtd;
	struct nand_chip *nand;
	struct octeon_nand_priv *priv;
	int chip;
	int active_chips;
	char *name;
	int chip_num = 0; /* Count of detected chips, used for device naming */

	DEV_DBG(DEBUG_INIT, &pdev->dev, "called\n");

#if 0
	/* 
	 * Optionally set defaults to be used for NAND chips that aren't 
	 * recognized by cvmx_nand_initialize() 
	 */
	cvmx_nand_set_defaults(2048, 64, 64, 2048, 2);
#endif
	cvmx_nand_initialize(0 /* | CVMX_NAND_INITIALIZE_FLAGS_DEBUG */, 0xff);

	active_chips = cvmx_nand_get_active_chips();
	for (chip = 0; chip < 8; chip++) {
		/* Skip chip selects that don't have NAND */
		if ((active_chips & (1<<chip)) == 0)
			continue;

		/* 
		 * Allocate and initialize mtd_info, nand_chip and private
		 * structures
		 */
		mtd = kzalloc(sizeof(struct mtd_info) +
				sizeof(struct nand_chip) +
				sizeof(struct octeon_nand_priv), GFP_KERNEL);
		if (!mtd) {
			dev_err(&pdev->dev, "Unable to allocate structures\n");
			return -ENOMEM;
		}
		name = kmalloc(MAX_NAND_NAME_LEN, GFP_KERNEL);
		if (!name)
		{
			kfree(mtd);
			dev_err(&pdev->dev, "Unable to allocate structures\n");
			return -ENOMEM;
		}

		nand = (struct nand_chip *) (mtd + 1);
		mtd->priv = (void *) nand;
		mtd->owner = THIS_MODULE;
		priv = (struct octeon_nand_priv *) (nand + 1);
		nand->priv = (void *) priv;
		memset(priv->data, 0xff, sizeof(priv->data));
		priv->pdev = pdev;
		priv->selected_chip = chip;

		nand->ecc.mode = NAND_ECC_SOFT;

		/* We always identify chips as 8 bit, as the Octeon NAND
		 * layer makes both 8 and 16 bit look the same.
		 * We never set the 16 bit buswidth option.
		 */

		nand->read_byte = octeon_nand_read_byte;
		nand->read_word = octeon_nand_read_word;
		nand->write_buf = octeon_nand_write_buf;
		nand->read_buf = octeon_nand_read_buf;
		nand->verify_buf = octeon_nand_verify_buf;
		nand->select_chip = octeon_nand_select_chip;
		nand->cmdfunc = octeon_nand_cmdfunc;

		if (nand_scan(mtd, 1) != 0) {
			dev_err(&pdev->dev, "NAND scan failed\n");
			kfree(mtd);
			return -ENXIO;
		}

		/* We need to override the name, as the default names
		 * have spaces in them, and this prevents the passing
		 * of partitioning information on the kernel command line.
		 */
		snprintf(name, MAX_NAND_NAME_LEN, "octeon_nand%d", chip_num);
		mtd->name = name;


#ifdef CONFIG_MTD_PARTITIONS
		nr_parts =
		parse_mtd_partitions(mtd, part_probes,
				     &mtd_parts, 0);
		if (nr_parts > 0)
			add_mtd_partitions(mtd, mtd_parts, nr_parts);
		else
		{
			if (add_mtd_device(mtd))
			{
				dev_err(&pdev->dev, "Failed to register MTD\n");
				kfree(mtd);
				return -ENOMEM;
			}
		}
#else
		if (add_mtd_device(mtd)) {
			dev_err(&pdev->dev, "Failed to register MTD\n");
			kfree(mtd);
			return -ENOMEM;
		}
#endif
		octeon_nand_open_mtd[chip] = mtd;
		chip_num++;
	}
	return 0;
}


/**
 * Called when the driver is unloaded. It must clean up all
 * created devices.
 *
 * @param pdev
 *
 * @return
 */
static int octeon_nand_remove(struct platform_device *pdev)
{
	int chip;

	DEV_DBG(DEBUG_INIT, &pdev->dev, "called\n");
	for (chip = 0; chip < 8; chip++) {
		if (octeon_nand_open_mtd[chip]) {
			del_mtd_device(octeon_nand_open_mtd[chip]);
			kfree(octeon_nand_open_mtd[chip]);
			octeon_nand_open_mtd[chip] = NULL;
		}
	}
	return 0;
}

static struct platform_driver octeon_nand_driver = {
	.probe = octeon_nand_probe,
	.remove = octeon_nand_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = DRIVER_NAME,
	},
};

/**
 * Module init
 *
 * @return Zero on success, negative on failure
 */
static int __init octeon_nand_init(void)
{
	struct resource irq_resource;

	/* Some of the initial NIC10e cards does not have NAND flash. */
	if (octeon_bootinfo->board_type == CVMX_BOARD_TYPE_NIC10E)
		return -ENODEV;

	if (!(OCTEON_IS_MODEL(OCTEON_CN6XXX) ||
		OCTEON_IS_MODEL(OCTEON_CN52XX)) ||
		OCTEON_IS_MODEL(OCTEON_CN52XX_PASS1_X))
		return -ENODEV;

	if (platform_driver_register(&octeon_nand_driver)) {
		printk(KERN_ERR "%s: Failed to register driver\n", DRIVER_NAME);
		return -ENOMEM;
	}

	memset(&irq_resource, 0, sizeof(irq_resource));
	irq_resource.start = OCTEON_IRQ_BOOTDMA;
	irq_resource.end = irq_resource.start;
	irq_resource.flags = IORESOURCE_IRQ;

	octeon_nand_pdev = platform_device_register_simple(DRIVER_NAME, 0,
		&irq_resource, 1);
	if (!octeon_nand_pdev) {
		printk(KERN_ERR "%s: Failed to allocate platform device\n",
			DRIVER_NAME);
		platform_driver_unregister(&octeon_nand_driver);
		return -ENOMEM;
	}
	return 0;
}


/**
 * Module unload
 *
 * @return
 */
static void __exit octeon_nand_cleanup(void)
{
	platform_device_unregister(octeon_nand_pdev);
	platform_driver_unregister(&octeon_nand_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon NAND driver.");
/* 
 * We need to call octeon_nand_init late enough that the MTD command line 
 * parser is already registered.  If built into the kernel , use a late 
 * initcall.
*/
/* This works for built-in and modular builds */
late_initcall(octeon_nand_init); 

module_exit(octeon_nand_cleanup);
