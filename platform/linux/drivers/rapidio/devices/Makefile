#
# Makefile for RIO devices
#

obj-$(CONFIG_RAPIDIO_CN6XXX)	+= cn6xxx.o

ifeq ($(CONFIG_RAPIDIO_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif
