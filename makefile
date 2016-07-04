#Copyright (C) 2016 xent
#Project is distributed under the terms of the GNU General Public License v3.0

PROJECT := osw
PROJECT_DIR := $(shell pwd)

CONFIG_FILE ?= .config
CROSS_COMPILE ?= arm-none-eabi-

-include $(CONFIG_FILE)
BUILD_FLAGS += PLATFORM

#Expand build flags
define process-flag
  $(1) := $$(CONFIG_$(1):"%"=%)
endef

$(foreach entry,$(BUILD_FLAGS),$(eval $(call process-flag,$(entry))))

#Process build flags
ifneq ($(findstring x86,$(PLATFORM)),)
  AR := ar
  CC := gcc
  CXX := g++
  ifeq ($(CONFIG_TARGET),"x86")
    CPU_FLAGS += -m32
  else
    CPU_FLAGS += -m64
  endif
else ifneq ($(findstring cortex-m,$(PLATFORM)),)
  AR := $(CROSS_COMPILE)ar
  CC := $(CROSS_COMPILE)gcc
  CXX := $(CROSS_COMPILE)g++
  CPU_FLAGS += -mcpu=$(PLATFORM) -mthumb
  CPU_FLAGS += -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections
  ifeq ($(CONFIG_FPU),y)
    CPU_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
  endif
else ifneq ($(MAKECMDGOALS),menuconfig)
  $(error Target architecture is undefined)
endif

ifeq ($(CONFIG_LTO),y)
  OPT_FLAGS += -flto -ffat-lto-objects
endif

ifeq ($(CONFIG_OPTIMIZATIONS_SIZE),y)
  OPT_FLAGS += -Os
else ifeq ($(CONFIG_OPTIMIZATIONS_FULL),y)
  OPT_FLAGS += -O3
else ifeq ($(CONFIG_OPTIMIZATIONS_DEBUG),y)
  OPT_FLAGS += -Og -g3
else
  OPT_FLAGS += -O0 -g3
endif

ifneq ($(CONFIG_ASSERTIONS),y)
  OPT_FLAGS += -DNDEBUG
endif

ifeq ($(VERBOSE),)
  Q := @
else
  Q :=
endif

#Configure common paths and libraries
INCLUDE_PATH += -Iinclude
OUTPUT_DIR := build_$(PLATFORM)
OPTION_FILE := $(OUTPUT_DIR)/.options

#Configure compiler options
CFLAGS += -std=c11 -Wall -Wextra -Winline -pedantic -Wshadow -Wcast-qual
CFLAGS += $(OPT_FLAGS) $(CPU_FLAGS) @$(OPTION_FILE)

#Other makefiles of the project
include lib$(PROJECT)/makefile

#External libraries
XCORE_PATH ?= $(PROJECT_DIR)/../xcore
INCLUDE_PATH += -I"$(XCORE_PATH)/include"
XCORE_PATH ?= $(PROJECT_DIR)/../xcore
INCLUDE_PATH += -I"$(XCORE_PATH)/include"

#Process auxiliary project options
define append-flag
  ifeq ($$($(1)),y)
    OPTION_STRING += -D$(1)
  else ifneq ($$($(1)),)
    OPTION_STRING += -D$(1)=$$($(1))
  endif
endef

$(foreach entry,$(PROJECT_FLAGS),$(eval $(call append-flag,$(entry))))

COBJECTS = $(CSOURCES:%.c=$(OUTPUT_DIR)/%.o)

#Define default targets
.PHONY: all clean menuconfig
.SUFFIXES:
.DEFAULT_GOAL = all

all: $(TARGETS)

$(OUTPUT_DIR)/%.o: %.c $(OPTION_FILE)
	@echo "$(CC): $@"
	@mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) $(INCLUDE_PATH) -MMD -MF $(@:%.o=%.d) -MT $@ $< -o $@

$(OPTION_FILE): $(CONFIG_FILE)
	@mkdir -p $(@D)
	$(Q)echo '$(OPTION_STRING)' > $@

clean:
	rm -f $(COBJECTS:%.o=%.d) $(COBJECTS)
	rm -f $(TARGETS)
	rm -f $(OPTION_FILE)

menuconfig:
	kconfig-mconf kconfig

ifneq ($(MAKECMDGOALS),clean)
  -include $(COBJECTS:%.o=%.d)
endif
