#ifndef _PPC_KERNEL_M82XX_PCI_H
#define _PPC_KERNEL_M82XX_PCI_H

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#define SIU_INT_IRQ1   ((uint)0x13 + CPM_IRQ_OFFSET)

#ifndef M82xx_PCI_LOWER_MEM
#define M82xx_PCI_LOWER_MEM		0x80000000
#define M82xx_PCI_UPPER_MEM		0x9fffffff
#define M82xx_PCI_MEM_OFFSET		0x00000000
#define M82xx_PCI_MEM_SIZE		0x20000000
#endif

#ifndef M82xx_PCI_LOWER_MMIO
#define M82xx_PCI_LOWER_MMIO		0xa0000000
#define M82xx_PCI_UPPER_MMIO		0xafffffff
#define M82xx_PCI_MMIO_OFFSET		0x00000000
#define M82xx_PCI_MMIO_SIZE		0x20000000
#endif

#ifndef M82xx_PCI_LOWER_IO
#define M82xx_PCI_LOWER_IO		0x00000000
#define M82xx_PCI_UPPER_IO		0x01ffffff
#define M82xx_PCI_IO_BASE		0xf6000000
#define M82xx_PCI_IO_SIZE		0x02000000
#endif


#ifndef M82xx_PCI_PRIM_WND_SIZE
#define M82xx_PCI_PRIM_WND_SIZE 	~(M82xx_PCI_IO_SIZE - 1U)
#define M82xx_PCI_PRIM_WND_BASE		(M82xx_PCI_IO_BASE)
#endif

#ifndef M82xx_PCI_SEC_WND_SIZE
#define M82xx_PCI_SEC_WND_SIZE 		~(M82xx_PCI_MEM_SIZE + M82xx_PCI_MMIO_SIZE - 1U)
#define M82xx_PCI_SEC_WND_BASE 		(M82xx_PCI_LOWER_MEM)
#endif

#ifndef PITA_ADDR_SHIFT
#define PITA_ADDR_SHIFT		12
#endif

#ifndef POTA_ADDR_SHIFT
#define POTA_ADDR_SHIFT		12
#endif
#ifndef _IO_BASE
#define _IO_BASE isa_io_base
#endif

#endif				/* _PPC_KERNEL_M8260_PCI_H */
