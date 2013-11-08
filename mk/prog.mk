OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS)) $(EXTRA_OBJS)
DEPS = $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRCS)) $(EXTRA_OBJS:%.o=%.d)

ifeq ($(IS_GCOV),yes)
CPPFLAGS += -fprofile-arcs -ftest-coverage
LD_LIBS += -fprofile-arcs
endif

ifeq ($(IS_GPROF),yes)
CPPFLAGS += -pg
LD_LIBS += -pg 
endif

ifeq ($(ARCH),mips)
#LD_FLAGS += -mabi=n32
else
LD_FLAGS += -m32
endif

$(BIN_DIR)/$(PROG): $(OBJS) $(DEP_LIBS)
	@echo "LD  => $(subst $(BLD_DIR),build.$(CPU).$(VER),$@)" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	$(CC) $(LD_FLAGS) -o $@ $(OBJS) $(LD_LIBS)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/$(PROG) $(OBJS) $(DEPS) $(EXTRA_CLEANS)

ifneq ($(MAKECMDGOALS),clean)
sinclude $(DEPS)
endif

include $(MK_DIR)/obj.mk
