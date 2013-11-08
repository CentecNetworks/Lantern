#
# Makefile for the Cavium Octeon specific kernel interface routines
# under Linux.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2005-2008 Cavium Networks
#

obj-y += cvmx-bootmem.o cvmx-l2c.o cvmx-sysinfo.o octeon-model.o

#obj-y += cvmx-pko.o cvmx-spi.o cvmx-cmd-queue.o \
	cvmx-helper-board.o cvmx-helper.o cvmx-helper-xaui.o \
	cvmx-helper-rgmii.o cvmx-helper-sgmii.o cvmx-helper-npi.o \
	cvmx-helper-loop.o cvmx-helper-spi.o cvmx-helper-util.o \
	cvmx-helper-srio.o cvmx-spi4000.o cvmx-clock.o cvmx-twsi.o \
	cvmx-srio.o cvmx-csr-db.o cvmx-csr-db-support.o cvmx-dma-engine.o

# CENTEC: remove srio because it use saa instruction while the instruction is
# not well supported by opensource toolchain.
obj-y += cvmx-pko.o cvmx-spi.o cvmx-cmd-queue.o \
	cvmx-helper-board.o cvmx-helper.o cvmx-helper-xaui.o \
	cvmx-helper-rgmii.o cvmx-helper-sgmii.o cvmx-helper-npi.o \
	cvmx-helper-loop.o cvmx-helper-spi.o cvmx-helper-util.o \
	cvmx-spi4000.o cvmx-clock.o cvmx-twsi.o \
	cvmx-csr-db.o cvmx-csr-db-support.o cvmx-dma-engine.o

obj-$(CONFIG_CAVIUM_DECODE_RSL) += cvmx-error.o cvmx-error-custom.o cvmx-error-init-cn30xx.o \
	cvmx-error-init-cn31xx.o cvmx-error-init-cn38xx.o \
	cvmx-error-init-cn38xxp2.o cvmx-error-init-cn50xx.o \
	cvmx-error-init-cn52xx.o cvmx-error-init-cn52xxp1.o \
	cvmx-error-init-cn56xx.o cvmx-error-init-cn56xxp1.o \
        cvmx-error-init-cn58xx.o cvmx-error-init-cn58xxp1.o \
        cvmx-error-init-cn63xx.o cvmx-error-init-cn63xxp1.o

obj-y += cvmx-helper-errata.o cvmx-helper-jtag.o cvmx-pcie.o

obj-$(CONFIG_CAVIUM_GDB) += cvmx-debug.o cvmx-debug-handler.o \
	cvmx-debug-uart.o cvmx-core.o cvmx-debug-remote.o cvmx-uart.o

obj-$(CONFIG_CAVIUM_OCTEON_TRA)       += cvmx-tra.o
obj-$(CONFIG_CAVIUM_OCTEON_NAND)      += cvmx-nand.o
obj-$(CONFIG_USB)      		      += cvmx-usbd.o cvmx-usb.o

obj-y += octeon-pci-console.o
