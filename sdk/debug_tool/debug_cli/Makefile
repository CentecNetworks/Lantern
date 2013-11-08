ifeq ($(targetbase),linux)
include $(MK_DIR)/sys.mk

LIB = debug_cli
SRCS = $(wildcard *.c)

CPPFLAGS += -I$(SDK_DIR)/dal/include
CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/zebra_ctccli/include
CPPFLAGS += -I$(SDK_DIR)/driver/common/include
CPPFLAGS += -I$(SDK_DIR)/mem_model/greatbelt/include
CPPFLAGS += -I$(SDK_DIR)/driver/greatbelt/include

CFLAGS += -Wall

include $(MK_DIR)/lib.mk
endif


