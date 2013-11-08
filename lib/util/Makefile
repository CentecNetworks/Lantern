include $(MK_DIR)/sys.mk

LIB = ctclib
SRCS = $(shell find src -name "*.c")

CPPFLAGS += -D_CTC_OF_

CPPFLAGS += -I$(TOP_DIR)/third_party
CPPFLAGS += -I$(LCADPT_H_DIR)
CPPFLAGS += -I$(TOP_DIR)/lib/afx
CPPFLAGS += -I$(TOP_DIR)/lib/sal/include
CPPFLAGS += -I$(TOP_DIR)/lib/util/include
CPPFLAGS += -I$(TOP_DIR)/include
CPPFLAGS += -I$(SDK_DIR)/kal/include
CPPFLAGS += -I$(SDK_DIR)/libctccli/include
CPPFLAGS += -I$(SDK_DIR)/libctccli/src
#CFLAGS += -Werror -Wall

include $(MK_DIR)/lib.mk
