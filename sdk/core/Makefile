ifeq ($(targetbase),linux)
include $(MK_DIR)/sys.mk

LIB = sdkcore
SRCS = $(shell find api/src -name "*.c")
SRCS += $(shell find $(CHIPNAME)/src -name "*.c")
SRCS += $(shell find common/src -name "*.c")

ifeq ($(CHIPNAME),humber)
CPPFLAGS += -DHUMBER
endif

ifeq ($(CHIPNAME),greatbelt)
CPPFLAGS += -DGREATBELT
endif

CPPFLAGS += -I$(SDK_DIR)/dal/include
CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/core/api/include
CPPFLAGS += -I$(SDK_DIR)/core/$(CHIPNAME)/include
CPPFLAGS += -I$(SDK_DIR)/core/$(CHIPNAME)/include/api
CPPFLAGS += -I$(SDK_DIR)/core/$(CHIPNAME)/include/sys
CPPFLAGS += -I$(SDK_DIR)/core/common/include
CPPFLAGS += -I$(SDK_DIR)/driver/humber/include

ifeq ($(BOARD),ctcuml)
CPPFLAGS += -I$(SDK_DIR)/../cmodel/ctckal/include
CPPFLAGS += -I$(SDK_DIR)/../cmodel/$(CHIPNAME)/include
CPPFLAGS += -I$(SDK_DIR)/../cmodel/mem_model/$(CHIPNAME)/include
endif

include $(MK_DIR)/lib.mk

ifeq ($(CHIPNAME),humber)
CFLAGS += -w
#CFLAGS += -Wall
endif

ifeq ($(CHIPNAME),greatbelt)
CFLAGS += -w
#CFLAGS += -Wall
endif

endif
ifeq ($(targetbase),vxworks)
BLD_DIR=$(SDK_DIR)/build.$(CPU).$(VER)
OBJ_DIR=$(BLD_DIR)/obj.$(BOARD)/core
LIB_DIR=$(BLD_DIR)/lib.$(BOARD)
BIN_DIR=$(BLD_DIR)/bin.$(BOARD)

all_targets = api common $(CHIPNAME) libsdkcore

all: $(all_targets) FORCE
clean: $(addprefix clean_,$(all_targets)) FORCE

api: FORCE
	make -C $(SDK_DIR)/core/api -f Makefile.vx

clean_api: FORCE
	make -C $(SDK_DIR)/core/api -f Makefile.vx clean

common: FORCE
	make -C $(SDK_DIR)/core/common -f Makefile.vx

clean_common: FORCE
	make -C $(SDK_DIR)/core/common -f Makefile.vx clean

$(CHIPNAME): FORCE
	make -C $(SDK_DIR)/core/$(CHIPNAME) -f Makefile.vx

clean_$(CHIPNAME): FORCE
	make -C $(SDK_DIR)/core/$(CHIPNAME) -f Makefile.vx clean

libsdkcore: FORCE
	make -f Makefile_libsdkcore.vx

clean_libsdkcore: FORCE
	make -f Makefile_libsdkcore.vx clean

.PHONY: FORCE
FORCE:
endif
