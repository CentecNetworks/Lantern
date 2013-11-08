ifeq ($(targetbase),linux)
include $(MK_DIR)/sys.mk
LIB = drv
SRCS =
SRCS += $(shell find src -name "*.c")

CPPFLAGS += -I$(SDK_DIR)/dal/include
CPPFLAGS += -I$(SDK_DIR)/kal/include


ifeq ($(BOARD),ctc-sim)
CPPFLAGS += -I$(SDK_DIR)/mem_model/$(CHIPNAME)/include
endif

ifeq ($(BOARD),linux-sim)
CPPFLAGS += -I$(SDK_DIR)/mem_model/$(CHIPNAME)/include
endif

CPPFLAGS += -I$(SDK_DIR)/driver/humber/include

include $(MK_DIR)/lib.mk

CFLAGS += -Werror
endif

ifeq ($(targetbase),vxworks)
BLD_DIR=$(SDK_DIR)/build.$(CPU).$(VER)
OBJ_DIR=$(BLD_DIR)/obj.$(BOARD)/driver
LIB_DIR=$(BLD_DIR)/lib.$(BOARD)
BIN_DIR=$(BLD_DIR)/bin.$(BOARD)


all_targets = common humber libdrv

all: $(all_targets) FORCE
clean: $(addprefix clean_,$(all_targets)) FORCE

common: FORCE
	make -C $(SDK_DIR)/driver/common -f Makefile.vx

clean_common: FORCE
	make -C $(SDK_DIR)/driver/common -f Makefile.vx  clean

humber: FORCE
	make -C $(SDK_DIR)/driver/humber -f Makefile.vx

clean_humber: FORCE
	make -C $(SDK_DIR)/driver/humber -f Makefile.vx  clean

libdrv: FORCE
	make -f Makefile_libdrv.vx

clean_libdrv: FORCE
	make -f Makefile_libdrv.vx clean


.PHONY: FORCE
FORCE:
endif

