#
# Makefile for Octeon specific Ethernet drivers. 
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2010 Cavium Networks
#
# Rules to build modules for Octeon ethernet driver, management port and 
# POW module.
#

obj-$(CONFIG_OCTEON_MGMT_ETHERNET)	+= octeon_mgmt.o
obj-$(CONFIG_OCTEON_POW_ONLY_ETHERNET)	+= octeon-pow-ethernet.o

obj-${CONFIG_OCTEON_ETHERNET} +=  octeon-ethernet.o

octeon-ethernet-objs := ethernet.o
octeon-ethernet-objs += ethernet-mdio.o
octeon-ethernet-objs += ethernet-mem.o
octeon-ethernet-objs += ethernet-rgmii.o
octeon-ethernet-objs += ethernet-rx.o
octeon-ethernet-objs += ethernet-sgmii.o
octeon-ethernet-objs += ethernet-spi.o
octeon-ethernet-objs += ethernet-tx.o
octeon-ethernet-objs += ethernet-xaui.o
octeon-ethernet-objs += ethernet-proc.o
ifdef CONFIG_RAPIDIO
octeon-ethernet-objs += ethernet-srio.o
endif
