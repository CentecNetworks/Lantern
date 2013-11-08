LKMOBJDIR = $(BLD_DIR)/obj.$(BOARD)/$(subst $(SDK_DIR)/,,$(CURDIR))
obj-m = dal_kernel_io.o

ifeq ($(ARCH),mips)
EXTRA_CFLAGS += -D_CTC_OCTEON_CN50XX_
KDIR = /sw/pub/kbuild/kbuild.octeon.r
endif

ifeq ($(ARCH),powerpc)
KDIR = /sw/pub/kbuild/kbuild.ppc.r
endif

EXTRA_CFLAGS += -I$(SDK_DIR)/dal/include
EXTRA_CFLAGS += -I$(SDK_DIR)/dal/linux/include
EXTRA_CFLAGS += -I$(SDK_DIR)/kal/include

.PHONY: all
all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) LKMOBJDIR=$(LKMOBJDIR)
	if [ ! -d $(dir $(BLD_DIR)/bin.$(BOARD)/) ]; then mkdir -p $(dir $(BLD_DIR)/bin.$(BOARD)/); fi
	cp $(LKMOBJDIR)/dal_kernel_io.ko $(BLD_DIR)/bin.$(BOARD)

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) LKMOBJDIR=$(LKMOBJDIR) clean
	rm -f $(BLD_DIR)/bin.$(BOARD)/dal_kernel_io.ko
