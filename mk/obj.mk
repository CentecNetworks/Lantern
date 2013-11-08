$(OBJ_DIR)/%.d: %.c
	@echo "CX  => $(subst $(BLD_DIR),build.$(CPU).$(VER),$(@:%.d=%.o))" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MT $@ -MF $@ -c -o $(@:%.d=%.o) $<

$(OBJ_DIR)/%.o: %.c
	@echo "CC  => $(subst $(BLD_DIR),build.$(CPU).$(VER),$@)" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.d: $(OBJ_DIR)/%.c
	@echo "CX  => $(subst $(BLD_DIR),build.$(CPU).$(VER),$(@:%.d=%.o))" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MT $@ -MF $@ -c -o $(@:%.d=%.o) $<

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c
	@echo "CC  => $(subst $(BLD_DIR),build.$(CPU).$(VER),$@)" && \
	if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi && \
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
