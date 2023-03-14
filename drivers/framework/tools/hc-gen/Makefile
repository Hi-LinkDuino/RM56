# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

Q = @
BUILD_DIR := build
TARGET := $(BUILD_DIR)/hc-gen

CXX := c++
CXX_FLAGS := -std=gnu++14
CXX_LD_FLAGS := -lstdc++

SRCS := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

ifeq ($(LANG),)
  CXX_FLAGS += -DOS_MINGW
else
  CXX_FLAGS += -DOS_UNIX
endif

all: $(TARGET)

$(OBJECTS): $(BUILD_DIR)/%.o : %.cpp
	$(Q) echo CC $<
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CXX) $(CXX_FLAGS)  $(CXX_LD_FLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(Q) echo built $@
	$(Q) $(CXX) $(CXX_FLAGS) $(CXX_LD_FLAGS) $^ -o $@

test: $(TARGET)
	$(Q) python test/hcgen_test.py $(TARGET)

update_testcase: $(TARGET)
	$(Q) python test/update_case.py $(TARGET)

clean:
	$(Q) rm -rf $(BUILD_DIR)

.PHONY: all clean test $(TARGET)