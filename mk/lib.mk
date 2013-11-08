OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS)) $(EXTRA_OBJS)
DEPS = $(patsubst %.c,$(OBJ_DIR)/%.d,$(SRCS)) $(EXTRA_OBJS:%.o=%.d)
ifeq ($(IS_GCOV),yes)
CPPFLAGS += -fprofile-arcs -ftest-coverage
endif

ifeq ($(IS_GPROF),yes)
CPPFLAGS += -pg
endif

$(LIB_DIR)/lib$(LIB).a: $(OBJS)
	@echo "LIB => $(subst $(BLD_DIR),build.$(CPU).$(VER),$@)" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	rm -rf $@ && \
	$(AR) cru $@ $^ && \
	$(RANLIB) $@

.PHONY: clean
clean:
	rm -rf $(LIB_DIR)/lib$(LIB).a $(OBJS) $(DEPS) $(EXTRA_CLEANS)

ifneq ($(MAKECMDGOALS),clean)
# XXX_OSS_TRIM: Add this to prevent building dependency file when exporting open source code, 
# otherwhise when we exporting open source code, gnu make will renegerate .d files 
# because they are makefiles... and because rules defined in obj.mk this will also 
# generate obj files, obviously this behaviour is now expected, so we must also filter dist
# target here.
ifneq ($(MAKECMDGOALS),dist)
sinclude $(DEPS)
endif
endif

include $(MK_DIR)/obj.mk
