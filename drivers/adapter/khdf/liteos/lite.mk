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

ifeq ($(filter $(LITEOSTOPDIR)/config.mk,$(MAKEFILE_LIST)),)
include $(LITEOSTOPDIR)/config.mk
endif

HDF_ROOT_DIR = $(LITEOSTOPDIR)/../../drivers

HDF_CFLAGS := -fstack-protector-strong -fno-common -fsigned-char -fno-strict-aliasing

HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/osal
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/utils
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/utils/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/platform
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/core
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/net
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/include/wifi
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/model/network/common/netdevice
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/model/storage/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/model/storage/include/mmc
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/model/storage/include/mtd
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/fwk
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/host/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/manager/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/shared/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/common/include/manager
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/common/include/host
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/network/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/osal/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/platform/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/adc
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/dac
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/dma
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/gpio
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/hdmi
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/i2c
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/i2s
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/i3c
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/mipi
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/pcie
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/pwm
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/pin
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/rtc
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/regulator
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/spi
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/uart
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/watchdog
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/support/platform/include/timer
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/model/storage/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/model/storage/include/mmc
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/adapter/khdf/liteos/model/storage/include/mtd
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/adapter/vnode/include
HDF_INCLUDE += -I $(HDF_ROOT_DIR)/framework/core/adapter/syscall/include
HDF_INCLUDE += -I $(LITEOSTOPDIR)/bsd/compat/linuxkpi/include
LOCAL_CFLAGS += $(HDF_INCLUDE)
LOCAL_CFLAGS += $(HDF_CFLAGS)

HDF_DRIVER = $(HDF_ROOT_DIR)/adapter/khdf/liteos/hdf_driver.mk
