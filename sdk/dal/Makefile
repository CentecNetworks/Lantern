ifeq ($(targetbase),linux)
all_targets = libdal

ifeq ($(BOARD),ctc-board)
all_tareget += lkm_linux_dal_io
endif    

ifeq ($(BOARD),linux-board)
all_tareget += lkm_linux_dal_io
endif    

all: $(all_targets) FORCE
clean: $(addprefix clean_,$(all_targets)) FORCE

libdal: FORCE
	make -f Makefile_libdal

clean_libdal: FORCE
	make -f Makefile_libdal clean

lkm_linux_dal_io: FORCE
	make -C $(SDK_DIR)/dal/src/linux/kernel -f Makefile

clean_lkm_linux_dal_io: FORCE
	make -C $(SDK_DIR)/dal/src/linux/kernel clean  -f Makefile

.PHONY: FORCE
FORCE:

endif

