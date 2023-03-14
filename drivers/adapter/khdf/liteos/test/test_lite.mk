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

HDF_ROOT_TEST_DIR = $(LITEOSTOPDIR)/../../drivers

HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/osal
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/utils
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/utils/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/platform
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/core
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/support/platform/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/support/platform/include/rtc
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/support/platform/include/pcie
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/host/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/master/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/shared/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/manager/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/common/include/host
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/adapter/khdf/liteos/osal/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/adapter/khdf/liteos/test/osal/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/adapter/vnode/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/core/adapter/syscall/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/adapter/khdf/liteos/network/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/common
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/manager
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/config
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/platform
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/platform/common
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/platform/entry
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/hdfframe
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/osal
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/adapter/osal/unittest
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/adapter/osal/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/utils/hcs_parser/unittest
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest
HDF_TEST_INCLUDE += -I $(LITEOSTOPDIR)/bsd/compat/linuxkpi/include
HDF_TEST_INCLUDE += -I $(LITEOSTOPDIR)/../../base/hiviewdfx/hilog_lite/interfaces/native/innerkits

ifeq ($(LOSCFG_DRIVERS_HDF_WIFI), y)
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/network/wifi/unittest/netdevice
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/network/wifi/unittest/module
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/network/wifi/unittest/net
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/network/wifi/unittest/qos
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/common/netdevice
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/platform/src/qos
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/network/wifi/unittest/message
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/core/module
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/platform/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/platform/include/message
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/platform/src
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/core/components/softap
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/network/wifi/core/components/sta
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/wifi
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/include/net
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/wifi
endif
ifeq ($(LOSCFG_DRIVERS_HDF_SENSOR), y)
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/sensor
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/sensor/interfaces/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/sensor/hal/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/sensor/driver/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/model/sensor/driver/common/include
endif
ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_DEVICE), y)
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/usb/device/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/ddk/common/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/ddk/device/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/interfaces/ddk/common
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/interfaces/ddk/device
endif
ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_HOST), y)
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/framework/test/unittest/model/usb/host/include
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/interfaces/ddk/common
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/interfaces/ddk/host
HDF_TEST_INCLUDE += -I $(HDF_ROOT_TEST_DIR)/peripheral/usb/gadget/function/include
endif
LOCAL_CFLAGS += $(HDF_TEST_INCLUDE)
HDF_DRIVER_TEST = $(HDF_ROOT_TEST_DIR)/adapter/khdf/liteos/hdf_driver.mk
