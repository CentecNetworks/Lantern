/*
 * Common PowerQUICC II code.
 *
 * Author: Scott Wood <scottwood@freescale.com>
 * Copyright (c) 2007 Freescale Semiconductor
 *
 * Based on code by Vitaly Bordug <vbordug@ru.mvista.com>
 * pq2_restart fix by Wade Farnsworth <wfarnsworth@mvista.com>
 * Copyright (c) 2006 MontaVista Software, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <asm/cpm2.h>
#include <asm/io.h>
#include <asm/pci-bridge.h>
#include <asm/system.h>

#include <platforms/82xx/m82xx_pci.h>
#include <platforms/82xx/pq2.h>

#define RMR_CSRE 0x00000001

void pq2_restart(char *cmd)
{
	local_irq_disable();
	setbits32(&cpm2_immr->im_clkrst.car_rmr, RMR_CSRE);

	/* Clear the ME,EE,IR & DR bits in MSR to cause checkstop */
	mtmsr(mfmsr() & ~(MSR_ME | MSR_EE | MSR_IR | MSR_DR));
	in_8(&cpm2_immr->im_clkrst.res[0]);

	panic("Restart failed\n");
}

#ifdef CONFIG_PCI
static int pq2_pci_exclude_device(struct pci_controller *hose,
                                  u_char bus, u8 devfn)
{
	if (bus == 0 && PCI_SLOT(devfn) == 0)
		return PCIBIOS_DEVICE_NOT_FOUND;
	else
		return PCIBIOS_SUCCESSFUL;
}

/* PCI bus configuration registers.
 */
static void
pq2ads_setup_pci(struct pci_controller *hose)
{
	__u32 val;
	volatile cpm2_map_t *immap = cpm2_immr;
	uint time;
    uint freq = 33333333;
		/* PCI int lowest prio */
	/* Each 4 bits is a device bus request	and the MS 4bits
	 is highest priority */
	/* Bus                4bit value
	   ---                ----------
	   CPM high      	0b0000
	   CPM middle           0b0001
	   CPM low       	0b0010
	   PCI reguest          0b0011
	   Reserved      	0b0100
	   Reserved      	0b0101
	   Internal Core     	0b0110
	   External Master 1 	0b0111
	   External Master 2 	0b1000
	   External Master 3 	0b1001
	   The rest are reserved
	 */
#define PPC_ACR_BUS_PARK_CORE 0x6
#define PPC_ACR_BUS_PARK_PCI  0x3

#define PCIBR_ENABLE        0x00000001
#define PCIGCR_PCI_BUS_EN   0x1
#define POCMR_ENABLE        0x80000000
#define POCMR_PCI_IO        0x40000000
#define POCMR_PREFETCH_EN   0x20000000
#define PICMR_ENABLE        0x80000000
#define PICMR_NO_SNOOP_EN   0x40000000
#define PICMR_PREFETCH_EN   0x20000000

	immap->im_siu_conf.siu_82xx.sc_ppc_alrh = 0x61207893;
	/* park bus on core */
	immap->im_siu_conf.siu_82xx.sc_ppc_acr = PPC_ACR_BUS_PARK_CORE;
	/*
	 * Set up master windows that allow the CPU to access PCI space. These
	 * windows are set up using the two SIU PCIBR registers.
	 */

	immap->im_memctl.memc_pcimsk0 = M82xx_PCI_PRIM_WND_SIZE;
	immap->im_memctl.memc_pcibr0  = M82xx_PCI_PRIM_WND_BASE | PCIBR_ENABLE;

#ifdef M82xx_PCI_SEC_WND_SIZE
	immap->im_memctl.memc_pcimsk1 = M82xx_PCI_SEC_WND_SIZE;
	immap->im_memctl.memc_pcibr1  = M82xx_PCI_SEC_WND_BASE | PCIBR_ENABLE;
#endif

	/* Enable PCI  */
	immap->im_pci.pci_gcr = cpu_to_le32(PCIGCR_PCI_BUS_EN);

	time = (int)66666666/freq;

	/* due to PCI Local Bus spec, some devices needs to wait such a long
	time after RST 	deassertion. More specifically, 0.508s for 66MHz & twice more for 33 */
	printk("The PCI bus is %d Mhz.\nWaiting %s after deasserting RST...\n",freq,
	(time==1) ? "0.5 seconds":"1 second" );

	{
		int i;
		for(i=0;i<(500*time);i++)
			udelay(1000);
	}

	/* setup ATU registers */
	immap->im_pci.pci_pocmr0 = cpu_to_le32(POCMR_ENABLE | POCMR_PCI_IO |
				((~(M82xx_PCI_IO_SIZE - 1U)) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar0 = cpu_to_le32(M82xx_PCI_LOWER_IO >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar0 = cpu_to_le32(M82xx_PCI_IO_BASE >> POTA_ADDR_SHIFT);

	/* Set-up non-prefetchable window */
	immap->im_pci.pci_pocmr1 = cpu_to_le32(POCMR_ENABLE | ((~(M82xx_PCI_MMIO_SIZE-1U)) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar1 = cpu_to_le32(M82xx_PCI_LOWER_MMIO >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar1 = cpu_to_le32((M82xx_PCI_LOWER_MMIO - M82xx_PCI_MMIO_OFFSET) >> POTA_ADDR_SHIFT);

	/* Set-up prefetchable window */
	immap->im_pci.pci_pocmr2 = cpu_to_le32(POCMR_ENABLE |POCMR_PREFETCH_EN |
		(~(M82xx_PCI_MEM_SIZE-1U) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar2 = cpu_to_le32(M82xx_PCI_LOWER_MEM >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar2 = cpu_to_le32((M82xx_PCI_LOWER_MEM - M82xx_PCI_MEM_OFFSET) >> POTA_ADDR_SHIFT);

 	/* Inbound transactions from PCI memory space */
	immap->im_pci.pci_picmr0 = cpu_to_le32(PICMR_ENABLE | PICMR_PREFETCH_EN |
					((~(0x10000000-1U)) >> PITA_ADDR_SHIFT));
	immap->im_pci.pci_pibar0 = cpu_to_le32(0x0>> PITA_ADDR_SHIFT);
	immap->im_pci.pci_pitar0 = cpu_to_le32(0x0>> PITA_ADDR_SHIFT);

	/* park bus on PCI */
	immap->im_siu_conf.siu_82xx.sc_ppc_acr = PPC_ACR_BUS_PARK_PCI;

	/* Enable bus mastering and inbound memory transactions */
	early_read_config_dword(hose, hose->first_busno, 0, PCI_COMMAND, &val);
	val &= 0xffff0000;
	val |= PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER;
	early_write_config_dword(hose, hose->first_busno, 0, PCI_COMMAND, val);
}

static void __init pq2_pci_add_bridge(struct device_node *np)
{
	struct pci_controller *hose;
	struct resource r;

	if (of_address_to_resource(np, 0, &r) || r.end - r.start < 0x10b)
		goto err;

	ppc_pci_add_flags(PPC_PCI_REASSIGN_ALL_BUS);

	hose = pcibios_alloc_controller(np);
	if (!hose)
		return;

	hose->dn = np;

	setup_indirect_pci(hose, r.start + 0x100, r.start + 0x104, 0);
	pci_process_bridge_OF_ranges(hose, np, 1);
    pq2ads_setup_pci(hose);
    
	return;

err:
	printk(KERN_ERR "No valid PCI reg property in device tree\n");
}

void __init pq2_init_pci(void)
{
	struct device_node *np = NULL;

	ppc_md.pci_exclude_device = pq2_pci_exclude_device;

	while ((np = of_find_compatible_node(np, NULL, "fsl,pq2-pci")))
		pq2_pci_add_bridge(np);
}
#endif
