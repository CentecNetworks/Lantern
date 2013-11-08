######################################set linux compile env #############################
ifeq ($(targetbase),linux)
include $(MK_DIR)/sys.mk

__LIB_DIR := $(LIB_DIR)

PROG = dtcli

SRCS = $(wildcard *.c)

CPPFLAGS += -DGREATBELT
CPPFLAGS += -I$(SDK_DIR)/dal/include
CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/driver/greatbelt/include/
CPPFLAGS += -I$(SDK_DIR)/driver/common/include/
CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/zebra_ctccli/include
CPPFLAGS += -I$(SDK_DIR)/mem_model/greatbelt/include
CPPFLAGS += -I$(SDK_DIR)/debug_tool/debug_cli

LIBKAL    = $(LIB_DIR)/libkal.a
LIBDRV    = $(LIB_DIR)/libdrv.a
LIBZEBRACTCCLI  = $(LIB_DIR)/libzebra_ctccli.a
DEBUG_CLI = $(LIB_DIR)/libdebug_cli.a

ifeq ($(ARCH),mips)
LD_FLAGS += -mabi=n32
else
LD_FLAGS += -m32
endif

ifeq ($(BOARD),ctc-board)
DEP_LIBS = $(LIBZEBRACTCCLI) $(LIBDRV) $(LIBKAL) $(DEBUG_CLI)
LD_LIBS = -L$(__LIB_DIR) -L$(LIB_DIR) -ldebug_cli -lzebra_ctccli -ldrv -lkal -lpthread
endif

include $(MK_DIR)/prog.mk
endif

