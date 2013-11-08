#
# Makefile for the Cavium Octeon specific kernel interface routines
# under Linux.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2005-2009 Cavium Networks
#

obj-y := setup.o serial.o octeon-platform.o octeon-irq.o csrc-octeon.o
obj-y += dma-octeon.o octeon_info.o
obj-y += octeon-memcpy.o perf_counters.o
obj-y += ctc_fpga.o ctc5010.o
obj-$(CONFIG_MTD)		      += flash_setup.o
obj-$(CONFIG_SMP)                     += smp.o
obj-y				      += pci-console.o
obj-$(CONFIG_CAVIUM_OCTEON_WATCHDOG)  += octeon-wdt.o
octeon-wdt-objs := octeon-wdt-main.o octeon-wdt-nmi.o
obj-$(CONFIG_CAVIUM_OCTEON_TRA)       += octeon-tra.o
obj-$(CONFIG_CAVIUM_OCTEON_NAND)      += octeon-nand.o
obj-$(CONFIG_CAVIUM_OCTEON_RAPIDIO)   += octeon-rapidio.o

EXTRA_CFLAGS += -Werror
