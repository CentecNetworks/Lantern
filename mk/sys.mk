#override SDK_DIR := $(shell cd $(SDK_DIR) && pwd)
OBJ_DIR = $(BLD_DIR)/obj.$(BOARD)/$(subst $(TOP_DIR)/,,$(CURDIR))
LIB_DIR = $(BLD_DIR)/lib.$(BOARD)
BIN_DIR = $(BLD_DIR)/bin.$(BOARD)

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

ifeq ($(ARCH),ppc)
CPPFLAGS = -DHOST_IS_LE=0
endif

ifeq ($(ARCH),mips)
CPPFLAGS = -DHOST_IS_LE=0 -D_CTC_OCTEON_CN50XX_
endif

#################################
#SDK_WORK_ENV
#0 - with no cmodel;
#1 - with  cmodel;
#################################

#################################
#SDK_WORK_PLATFORM
#0 - hardware platform ;
#1 - software simulation platform;
#################################

ifeq ($(BOARD),ctc-sim)
# current cpu is little endian
CPPFLAGS = -DHOST_IS_LE=1
CPPFLAGS += -DSDK_IN_USERMODE
CPPFLAGS += -DUSE_SIM_MEM
CPPFLAGS += -DSDK_WORK_ENV=1
CPPFLAGS += -DSDK_WORK_PLATFORM=1
ifeq ($(WITHLIB),TRUE)
CPPFLAGS += -DWITH_CMODEL_CLI=0
else
CPPFLAGS += -DWITH_CMODEL_CLI=1
endif
CPPFLAGS += -D_OFP_UML_
CPPFLAGS += -D_GLB_UML_SYSTEM_
endif

ifeq ($(BOARD),linux-sim)
CPPFLAGS += -DSDK_IN_USERMODE
CPPFLAGS += -DUSE_SIM_MEM
CPPFLAGS += -DSDK_WORK_ENV=0
CPPFLAGS += -DSDK_WORK_PLATFORM=1
endif

ifeq ($(BOARD),ctc-board)
CPPFLAGS += -DSDK_IN_USERMODE
CPPFLAGS += -DSDK_WORK_ENV=1
CPPFLAGS += -DSDK_WORK_PLATFORM=0
endif

ifeq ($(BOARD),linux-board)
CPPFLAGS += -DSDK_IN_USERMODE
CPPFLAGS += -DSDK_WORK_ENV=0
CPPFLAGS += -DSDK_WORK_PLATFORM=0
endif

CPPFLAGS += -DSDK_IN_DEBUG_VER

#CFLAGS = -Wall 
CFLAGS = -g -fno-strict-aliasing
#CFLAGS = -Wnoerror=unused-but-set-variable
# Let each library/binary choose this option.
# CFLAGS += -Werror
CFLAGS += -D_CENTEC_

CPPFLAGS += -D_GNU_SOURCE
ifeq ($(ARCH),mips)
# ZDY: BIG ENDIAN in 32 bit mode.
CPPFLAGS += 
#CPPFLAGS += -mabi=n32
else
CPPFLAGS += -m32
endif

IS_GCOV = no
IS_GPROF = no

ifeq ($(IS_GPROF),yes)
CFLAGS += -pg
CPPFLAGS += -pg
endif

ifeq ($(IS_GCOV),yes)
CFLAGS += -DISGCOV
#CFLAGS += -D_CTC_MEMWATCH_
#CFLAGS += -DMEMWATCH 
#CFLAGS += -DMEMWATCH_STDIO
endif

ifeq ($(CHIPNAME),humber)
CPPFLAGS += -DHUMBER
endif

ifeq ($(CHIPNAME),greatbelt)
CPPFLAGS += -DGREATBELT
endif

CPPFLAGS += -D_OFP_SDK_ 

ifeq ($(VER),r)
CFLAGS += -O2
endif

