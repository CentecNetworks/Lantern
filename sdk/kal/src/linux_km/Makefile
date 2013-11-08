KSRCDIR = /lib/modules/$(shell uname -r)/build

EXTRA_CFLAGS += -Wno-strict-prototypes
EXTRA_CFLAGS += -D_KAL_LINUX_KM
EXTRA_CFLAGS += -I$(M)/../../include

obj-m    = kal.o

kal-objs = kal_lkm.o \
           kal_log.o \
           kal_mem.o \
           kal_task.o \
           kal_event.o \
           kal_mutex.o \
           ../common/kal_timer.o \
           ../common/kal_async_queue.o\
           ../common/kal.o
.PHONY: all
all:
	$(MAKE) -C $(KSRCDIR) M=$(shell pwd)

.PHONY: clean
clean:
	$(MAKE) -C $(KSRCDIR) M=$(shell pwd) clean
