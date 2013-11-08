include $(MK_DIR)/sys.mk

LIB = sal
SRCS = $(shell find src/linux_um src/common -name "*.c")

CPPFLAGS += -I$(TOP_DIR)/lib/sal/include
CPPFLAGS += -D_GNU_SOURCE

#ifneq ($(ARCH),um)
#include $(MK_DIR)/solib.mk
#else
include $(MK_DIR)/lib.mk
#endif

CFLAGS += -Werror -Wall
