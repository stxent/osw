#Copyright (C) 2016 xent
#Project is distributed under the terms of the GNU General Public License v3.0

ifeq ($(CONFIG_OS_LINUX),y)
  LIBRARY_SOURCES := $(shell find libosw/linux -name *.c)
endif

LIBRARY_OBJECTS = $(LIBRARY_SOURCES:%.c=$(OUTPUT_DIR)/%.o)
CSOURCES += $(LIBRARY_SOURCES)

LIBRARY_FILE = $(OUTPUT_DIR)/lib$(PROJECT).a
TARGETS += $(LIBRARY_FILE)

$(LIBRARY_FILE): $(LIBRARY_OBJECTS)
	@echo "$(AR): $@"
	$(Q)$(AR) -c -r $@ $^
