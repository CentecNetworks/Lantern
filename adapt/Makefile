include $(MK_DIR)/sys.mk

LIB = adapt
SRCS = $(shell find api -name "*.c")
SRCS += $(shell find lib -name "*.c")
SRCS += $(shell find adpt -name "*.c")
SRCS += $(shell find hal -name "*.c")
SRCS += $(shell find ovs -name "*.c")

# Enable _GNU_SOURCE will enable __USE_ISOC99 to include <limits.h>,
# OVS timer.h will use LLONG_MAX defined in the header file.
CPPFLAGS += -D_OFP_CENTEC_ -D_OFP_SDK_ -DHAVE_CONFIG_H -D_GNU_SOURCE

ifeq ($(_V330_OPEN_SOURCE), y)
CPPFLAGS += -D_OPEN_SOURCE_
else
CPPFLAGS += -I$(CENTEC_PRIVATE_DIR)/vtysh/libctccli/include
CPPFLAGS += -I$(CENTEC_PRIVATE_DIR)/vtysh/libctccli/src
CPPFLAGS += -I$(CENTEC_PRIVATE_DIR)/lc/lcadpt/include
endif

CPPFLAGS += -I$(TOP_DIR)/include
CPPFLAGS += -I$(TOP_DIR)/lib/util/include
CPPFLAGS += -I$(TOP_DIR)/lib

CPPFLAGS += -I$(OVSROOT)
CPPFLAGS += -I$(OVSROOT)/lib
CPPFLAGS += -I$(OVSROOT)/include
CPPFLAGS += -I$(OVSROOT)/ofproto
CPPFLAGS += -I$(OVSROOT)/vswitchd

CPPFLAGS += -I$(TOP_DIR)/adapt/api/include
CPPFLAGS += -I$(TOP_DIR)/adapt/lib
CPPFLAGS += -I$(TOP_DIR)/adapt/adpt/include
CPPFLAGS += -I$(TOP_DIR)/adapt/hal/include
CPPFLAGS += -I$(TOP_DIR)/adapt/ovs/include

CPPFLAGS += -I$(TOP_DIR)/lib/afx
CPPFLAGS += -I$(TOP_DIR)/lib/sal/include

CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/core/api/include
CPPFLAGS += -I$(SDK_DIR)/core/humber/include
CPPFLAGS += -I$(SDK_DIR)/core/humber/include/api
CPPFLAGS += -I$(SDK_DIR)/core/humber/include/sys
CPPFLAGS += -I$(SDK_DIR)/driver/common/include
CPPFLAGS += -I$(SDK_DIR)/mem_model/include
CPPFLAGS += -I$(SDK_DIR)/driver/humber/include
CPPFLAGS += -I$(SDK_DIR)/core/common/include 

CPPFLAGS += -I$(SDK_DIR)/dal/include

include $(MK_DIR)/lib.mk
