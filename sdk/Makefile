# CROSS_COMPILE should set appropriate compiler path to PATH.
# Suggestion: add these two lines to your ~/.bash_profile
#    PATH=$PATH:/sw/pub/xtools.ppc-linux/bin/:/opt/gcc-4.3/bin/
#    export PATH
# In this case, /sw/pub/xtools.ppc-linux/bin/    /opt/gcc-4.3/bin/  is our appropriate compiler path.
#
ifeq ($(targetbase),linux)
export SDK_DIR = $(CURDIR)
export MK_DIR = $(CURDIR)/mk

ifneq ($(VER),r)
override VER = d
endif

ifndef CHIPNAME
export CHIPNAME = humber
endif

# ctcsdk: cli+sdk+asic (hardware platform board)
ifeq ($(BOARD),ctc-board)
ifeq ($(ARCH),powerpc)
CPU = ppc_82xx
CROSS_COMPILE =ppc-linux-
endif
ifeq ($(ARCH),mips)
CPU = octeon
CROSS_COMPILE =mips64-octeon-linux-gnu-
endif
KDIR =
all_targets = libkal libdal libdrv liblibctccli
all_targets += libsdkcore libctccli
endif

# ctcsdk: cli+sdk++asic (hardware platform board)
ifeq ($(BOARD),linux-board)
ifeq ($(ARCH),powerpc)
CPU = ppc_82xx
CROSS_COMPILE =ppc-linux-
endif
ifeq ($(ARCH),mips)
CPU = octeon
CROSS_COMPILE =mips64-octeon-linux-gnu-
endif
KDIR =
all_targets = libkal libdal libdrv liblibctccli
all_targets += libsdkcore libctccli
endif

export BLD_DIR = $(SDK_DIR)/build.$(CPU).$(VER)
export BOARD
export VER
export CPU
export ARCH
export SUBARCH
export CROSS_COMPILE
export KDIR

all: $(all_targets) FORCE
clean: $(addprefix clean_,$(all_targets)) FORCE

libkal: FORCE
	make -C $(SDK_DIR)/kal

clean_libkal: FORCE
	make -C $(SDK_DIR)/kal clean

libdal: FORCE
	make -C $(SDK_DIR)/dal -f Makefile

clean_libdal: FORCE
	make -C $(SDK_DIR)/dal -f Makefile

libdrv: FORCE
	@echo "GEN => $@"
	make -C $(SDK_DIR)/driver/$(CHIPNAME)

clean_libdrv: FORCE
	make -C $(SDK_DIR)/driver/$(CHIPNAME) clean

libsdkcore: FORCE
	make -C $(SDK_DIR)/core

clean_libsdkcore: FORCE
	make -C $(SDK_DIR)/core clean

liblibctccli: FORCE
	make -C $(SDK_DIR)/libctccli

clean_liblibctccli: FORCE
	make -C $(SDK_DIR)/libctccli clean

libctccli: FORCE
	make -C $(SDK_DIR)/ctccli

clean_libctccli: FORCE
	make -C $(SDK_DIR)/ctccli clean


distclean: FORCE
	rm -rf $(BLD_DIR)

.PHONY: FORCE
FORCE:
endif


ifeq ($(targetbase),vxworks)

#include $(TGT_DIR)/h/make/defs.project

## build-configuration info

BLD_DIR = $(SDK_DIR)/build.$(CPU).$(VER)

# ctcsdk: sdk + asic
ifeq ($(BOARD),vxworks)
all_targets = libkal  libdrv libsdkcore liblibctccli libctccli
endif

all: $(all_targets) FORCE
clean: $(addprefix clean_,$(all_targets)) FORCE

libkal: FORCE
	make -C $(SDK_DIR)/kal

clean_libkal: FORCE
	make -C $(SDK_DIR)/kal clean

libdrv: FORCE
	make -C $(SDK_DIR)/driver

clean_libdrv: FORCE
	make -C $(SDK_DIR)/driver clean

libsdkcore: FORCE
	make -C $(SDK_DIR)/core

clean_libsdkcore: FORCE
	make -C $(SDK_DIR)/core clean

liblibctccli: FORCE
	make -C $(SDK_DIR)/libctccli

clean_liblibctccli: FORCE
	make -C $(SDK_DIR)/libctccli clean

libctccli: FORCE
	make -C $(SDK_DIR)/ctccli

clean_libctccli: FORCE
	make -C $(SDK_DIR)/ctccli clean


distclean: FORCE
	rm -rf $(BLD_DIR)

.PHONY: FORCE
FORCE:

endif
