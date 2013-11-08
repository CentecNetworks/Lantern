include Makefile.env.inc

# Setup makefile variable according to the environment configuration of configure.sh
ifndef _V330_TOP_PATH
_V330_TOP_PATH=$(shell pwd)
endif

export ROOT_DIR=$(_V330_TOP_PATH)
export PLAT_TOP_DIR=$(ROOT_DIR)/platform
export TOP_DIR=$(ROOT_DIR)
export OUT_DIR=$(ROOT_DIR)/out

# Compile debug or release version (please use d or r)
ifeq ($(_V330_DEBUG), y)
export VER=d
else
export VER=r
endif

# Cross compile prefix
export CROSS_COMPILE=$(_V330_CROSSCOMPILE_PREFIX)
# Note: This is need for packaging system library from cross compiler link ld-linux/libdl and etc.
export GLIBC_PATH=$(_V330_GLIBC_PATH)

# CPU ARCH settings (this setting is for cavium-octeon)
export ARCH=mips
export CPU=octeon
export targetbase=linux
export BOARD=linux-board
export WITHLIB=FALSE
export IS_UML=n
export GLOBAL_INCLUDE_DIR=$(TOP_DIR)/include
export SDK_DIR=$(TOP_DIR)/sdk
export LC_DIR=$(CENTEC_PRIVATE_DIR)/lc
export DRV_TOP_DIR=$(CENTEC_PRIVATE_DIR)/drvs
export MK_DIR=$(TOP_DIR)/mk
export PRE_LIB_DIR=$(OUT_DIR)/build.$(CPU).$(VER)/lib.$(BOARD)
export BLD_DIR=$(OUT_DIR)/build.$(CPU).$(VER)
export K_DIR=$(OUT_DIR)/kbuild.$(CPU).$(VER)
export CENTEC_PRIVATE_DIR=$(TOP_DIR)/centec_private
export LCADPT_H_DIR=$(CENTEC_PRIVATE_DIR)/lc/lcadpt/include
export THIRD_PARTY_DIR=$(TOP_DIR)/third_party
export PRE_BUILT_DIR=$(TOP_DIR)/prebuilt
export PRE_BUILT_LIB_DIR=$(PRE_BUILT_DIR)/lib
export OS_NAME=VCOS-1.0.bin

export CTCLIBROOT=$(OUT_DIR)/build.$(CPU).$(VER)/lib
export OVSROOT=$(TOP_DIR)/ovs
export IS_GCOV=no

# FOR KERNEL build on opensource mips64 toolchain.
export KCFLAGS=-Wno-error=unused-but-set-variable
# support fast compilation.
export COMPILE_SPEED=-j$(_V330_COMPILE_JOBS)

export KDIR=$(K_DIR)
export BUSYBOX_DIR=$(OUT_DIR)/build.busybox.$(CPU).$(VER)

all_targets = busybox
all_targets += kernel

ifndef CHIPNAME
export CHIPNAME = humber
endif

all_targets += lib
all_targets += libadapt
all_targets += libsdkkal libsdkdal 
all_targets += libsdkdrv
all_targets += libsdkcore

all_targets += openflow

ifeq ($(_V330_OPEN_SOURCE), n)
OVS_DEP = centec_private
all_targets += centec_private
endif

# package should be the final target
all_targets += package

#
# for generate all in one image
#
all: package FORCE

ver_h_file=$(TOP_DIR)/adapt/lib/ofp_version.h
version_num=$(shell grep '\<OPENFLOW_CTC_VERSION\>' $(ver_h_file) | sed -s -e 's/.*OPENFLOW_CTC_VERSION.*\"\(.*\)\"/\1/')

# Import package rules, must import after top variable have been set.
include Makefile.package
build_ramdisk: $(all_targets)

package: build_ramdisk FORCE
	echo "Generate kernel..."; \
	cd $(K_DIR); \
	make vmlinux.64 modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE); \
	gzip vmlinux.64; \
	mv vmlinux.64.gz vmlinux.64; \
	echo "Create all in one image..."; \
	$(TOP_DIR)/build/cfg/mkimage_octeon -A $(ARCH) \
		-O octeon_linux -T kernel -C gzip -a 0xa00000 \
		-e 0 -n "v$(version_num)" -d vmlinux.64 \
		$(BLD_DIR)/../$(OS_NAME); \
	echo "Good Luck!";

kernel:
	@if [ ! -f $(K_DIR)/.config ] ; then \
	    cd $(PLAT_TOP_DIR)/linux; \
		./build_octeon.sh $(K_DIR) $(VER); \
	else \
		cd $(K_DIR); \
		cp $(PLAT_TOP_DIR)/linux/usr/*.bit.gz $(K_DIR)/ ; \
	    make $(COMPILE_SPEED) vmlinux.64 modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE); \
	fi;

clean_kernel:
	@if [ -f $(K_DIR)/.config ]; then \
		cd $(K_DIR); \
		make mrproper; \
	fi;

busybox:
	@if [ ! -f $(BUSYBOX_DIR)/.config ]; then \
		cd $(PLAT_TOP_DIR)/busybox; \
		./build_octeon.sh $(VER) $(BOARD) $(BUSYBOX_DIR); \
	else \
		cd $(BUSYBOX_DIR); \
		make $(COMPILE_SPEED) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE); \
	fi;

clean_busybox:
	@if [ -f $(BUSYBOX_DIR)/.config ]; then \
		cd $(PLAT_TOP_DIR)/busybox; \
		make clean; \
	fi;

sys_clean: $(addprefix clean_,$(all_targets)) FORCE

sys: $(all_targets) FORCE

lib: FORCE
	make -C lib

clean_lib: FORCE
	make -C lib clean

libadapt: FORCE
	make -C adapt -f Makefile

clean_libadapt: FORCE
	make -C adapt -f Makefile clean

libsdkkal: FORCE
	make -C $(SDK_DIR)/kal

clean_libsdkkal: FORCE
	make -C $(SDK_DIR)/kal clean

libsdkdal: FORCE
	make -C $(SDK_DIR)/dal 

clean_libsdkdal: FORCE
	make -C $(SDK_DIR)/dal clean

libsdkdrv: FORCE
	@echo "GEN => $@"
	make -C $(SDK_DIR)/driver/$(CHIPNAME)

clean_libsdkdrv: FORCE
	make -C $(SDK_DIR)/driver/$(CHIPNAME) clean

libsdkcore: FORCE
	make -C $(SDK_DIR)/core

clean_libsdkcore: FORCE
	make -C $(SDK_DIR)/core clean

openflow: libadapt $(OVS_DEP) libsdkcore libsdkkal libsdkdal libsdkdrv lib FORCE
	make -C $(OVSROOT) -f Makefile.ctc

clean_openflow: FORCE
	make -C $(OVSROOT) -f Makefile.ctc clean

clean: FORCE
	@echo "Delete objs and binaries output directory: $(OUT_DIR)"
	@rm -fr $(OUT_DIR)

ovs_package: openflow FORCE
	@./build/build_ovs_package.sh

clean_ovs_package: clean_openflow FORCE
	rm -rf $(_OVS_PKG_TMP_TOP_DIR)

ifeq ($(_V330_OPEN_SOURCE), n)
centec_private: FORCE
	make -C $(CENTEC_PRIVATE_DIR)

clean_centec_private: FORCE
	make -C $(CENTEC_PRIVATE_DIR) clean
endif

.PHONY: FORCE
FORCE:
