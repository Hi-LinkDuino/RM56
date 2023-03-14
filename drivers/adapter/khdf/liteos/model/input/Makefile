# Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used
#    to endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos/lite.mk

INPUT_ROOT_DIR = $(LITEOSTOPDIR)/../../drivers/framework/model/input/driver

ifeq ($(LOSCFG_DRIVERS_HDF_INPUT), y)
MODULE_NAME := hdf_input_driver
LOCAL_INCLUDE := $(INPUT_ROOT_DIR) \
                 $(INPUT_ROOT_DIR)/input_bus_ops \
                 $(INPUT_ROOT_DIR)/../../../../../third_party/FreeBSD/sys/dev/evdev
LOCAL_SRCS += $(INPUT_ROOT_DIR)/input_bus_ops/input_i2c_ops.c \
              $(INPUT_ROOT_DIR)/hdf_input_device_manager.c \
              $(INPUT_ROOT_DIR)/input_config_parser.c \
              $(INPUT_ROOT_DIR)/event_hub.c \
              $(INPUT_ROOT_DIR)/hdf_touch.c \
              $(INPUT_ROOT_DIR)/hdf_key.c \
              $(INPUT_ROOT_DIR)/hdf_hid_adapter.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_TP_5P5_GT911), y)
LOCAL_SRCS += $(INPUT_ROOT_DIR)/touchscreen/touch_gt911.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_TP_2P35_FT6236), y)
LOCAL_SRCS += $(INPUT_ROOT_DIR)/touchscreen/touch_ft6336.c
endif

include $(HDF_DRIVER)
