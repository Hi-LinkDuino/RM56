# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

TARGET := hc-gen
YACC_LEX_PREFIX :=HcsCompiler
C_FLAGS := -std=gnu99 -Wall -Werror -Wno-attributes -Wall
CC := gcc
YACC := bison
LEX := flex
Q = @

BOUNDS_CHECK_LIB := $(abspath ../../../../third_party/bounds_checking_function/)
INCLUDE_DIR := ./include $(BOUNDS_CHECK_LIB)/include
OUT_DIR := build

TEST_CASE := $(abspath ../../../adapter/lite/khdf/test/tools/hc-gen/test/unittest)

ORIGIN_SOURCES := $(wildcard src/*)
ORIGIN_SOURCES += $(wildcard $(BOUNDS_CHECK_LIB)/src/*)

C_SOURCES := $(filter %.c,$(ORIGIN_SOURCES))
YACC_SOURCES := $(filter %.y,$(ORIGIN_SOURCES))
LEX_SOURCES := $(filter %.l,$(ORIGIN_SOURCES))
YACC_GEN_SOURCES := $(patsubst %.y,$(OUT_DIR)/%_tab.c,$(YACC_SOURCES))
LEX_GEN_SOURCES := $(patsubst %.l,$(OUT_DIR)/%_lex.c,$(LEX_SOURCES))

C_OBJECTS := $(patsubst %.c,$(OUT_DIR)/%.o,$(C_SOURCES))
GEN_OBJECTS +=  $(patsubst %.c,%.o,$(YACC_GEN_SOURCES) $(LEX_GEN_SOURCES))
OBJECTS :=  $(GEN_OBJECTS) $(C_OBJECTS)

C_FLAGS += $(addprefix -I,$(INCLUDE_DIR))
INCLUDE_DIR += $(OUT_DIR)

UNAME := $(shell uname -a)
ifneq ($(findstring Linux,$(UNAME)),)
    C_FLAGS += -D OS_LINUX
else
    C_FLAGS += -D OS_WIN
    C_FLAGS += -D MINGW32
endif

ifeq ($(BUILD_TYPE),debug)
	C_FLAGS += -g
else ifeq ($(BUILD_TYPE),asan)
	C_FLAGS += -g -fsanitize=address
else
	# release
	C_FLAGS += -O2 -s -ffunction-sections -fdata-sections -Wl,--gc-sections
endif

all: $(TARGET)

$(YACC_GEN_SOURCES) : $(OUT_DIR)/%_tab.c : %.y
	$(Q)mkdir -p $(dir $(@))
	$(Q)$(YACC) -o $@ -v -d -pHcsCompiler $<

$(LEX_GEN_SOURCES) : $(OUT_DIR)/%_lex.c : %.l | $(YACC_GEN_SOURCES)
	$(Q)mkdir -p $(dir $(@))
	$(Q)$(LEX) -o $@ -PHcsCompiler $<

$(C_OBJECTS) : $(OUT_DIR)/%.o : %.c
	$(Q)mkdir -p $(dir $(@))
	$(Q)$(CC) -c -o $@ $(C_FLAGS) $^

$(GEN_OBJECTS) : %.o : %.c
	$(Q)$(CC) -c -o $@ $(C_FLAGS) $^

$(TARGET) : $(OBJECTS) | $(GEN_OBJECTS)
	$(Q)$(CC) -o $@ $(C_FLAGS) $^

test: all
	python3 $(TEST_CASE)/hcgen_test.py $(TARGET)

test_update: all
	python3 $(TEST_CASE)/update_case.py $(TARGET)

clean:
	$(Q)rm -rf $(OUT_DIR)
	$(Q)rm -f $(TARGET)

.PHONY: clean all test test_update
