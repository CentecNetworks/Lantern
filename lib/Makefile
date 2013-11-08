all_targets += sal
all_targets += afx
all_targets += ctclib

ifeq ($(CPU),i686)
all_targets += libcmodel
endif

all: $(all_targets) FORCE

clean: $(addprefix clean_,$(all_targets)) FORCE

afx: FORCE
	make -C afx

clean_afx: FORCE
	make -C afx clean

peth: FORCE
	make -C peth

clean_peth: FORCE
	make -C peth clean

ctclib: FORCE
	make -C util

clean_ctclib: FORCE
	make -C util clean

sal: FORCE
	make -C sal

clean_sal: FORCE
	make -C sal clean
	
.PHONY: FORCE
FORCE:
