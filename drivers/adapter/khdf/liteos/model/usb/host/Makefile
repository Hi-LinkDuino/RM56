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

include $(LITEOSTOPDIR)/config.mk
include $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos/lite.mk

ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_HOST), y)
MODULE_NAME := hdf_usb_ddk_host

PERIPHERAL_USB_DDK_HOST = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/ddk/host

LOCAL_INCLUDE := $(PERIPHERAL_USB_DDK_HOST)/../../interfaces/ddk/common \
                 $(PERIPHERAL_USB_DDK_HOST)/../../interfaces/ddk/host \
                 $(PERIPHERAL_USB_DDK_HOST)/include

LOCAL_SRCS += $(PERIPHERAL_USB_DDK_HOST)/src/liteos_adapter.c \
              $(PERIPHERAL_USB_DDK_HOST)/src/usb_raw_api.c \
              $(PERIPHERAL_USB_DDK_HOST)/src/usb_raw_api_library.c \
              $(PERIPHERAL_USB_DDK_HOST)/src/usb_protocol.c \
              $(PERIPHERAL_USB_DDK_HOST)/src/usb_io_manage.c \
              $(PERIPHERAL_USB_DDK_HOST)/src/usb_interface_pool.c

ifeq ($(LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY), y)
USB_PNP_NOTIFY_ROOT = $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos/model/usb/host
USB_PNP_NOTIFY_SHARED_ROOT = $(LITEOSTOPDIR)/../../drivers/framework/core/shared
FRAMEWORKS_USB_DDK_HOST = $(LITEOSTOPDIR)/../../drivers/framework/model/usb

LOCAL_INCLUDE := $(USB_PNP_NOTIFY_ROOT)/include \
                 $(USB_PNP_NOTIFY_SHARED_ROOT)/include \
                 $(FRAMEWORKS_USB_DDK_HOST)/include

LOCAL_SRCS += $(USB_PNP_NOTIFY_ROOT)/src/usb_pnp_notify.c \
              $(USB_PNP_NOTIFY_ROOT)/src/usb_pnp_manager.c \
              $(FRAMEWORKS_USB_DDK_HOST)/src/usb_ddk_pnp_loader.c \
              $(USB_PNP_NOTIFY_ROOT)/src/usb_test_pnp_notify.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_USB_HOST_DEMO_TEST), y)
FRAMEWORKS_USB_SERIAL_DEMO_ROOT = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/serial
FRAMEWORKS_USB_NET_DEMO_ROOT = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/net
FRAMEWORKS_USB_DDK_SPEED_TEST_ROOT = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/sample/host/liteos_test

LOCAL_INCLUDE := $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/../gadget/function/include \
                 $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/../interfaces/ddk/common \
                 $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/../interfaces/ddk/host \
                 $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/../ddk/host/include \
                 $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/include \
                 $(FRAMEWORKS_USB_NET_DEMO_ROOT)/include \
                 $(FRAMEWORKS_USB_DDK_SPEED_TEST_ROOT)/include

LOCAL_SRCS += $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/src/usb_serial.c \
              $(FRAMEWORKS_USB_SERIAL_DEMO_ROOT)/src/usb_serial_rawapi.c \
              $(FRAMEWORKS_USB_NET_DEMO_ROOT)/src/cdc_ether.c \
              $(FRAMEWORKS_USB_DDK_SPEED_TEST_ROOT)/src/usbhost_sdkapi_speed.c \
              $(FRAMEWORKS_USB_DDK_SPEED_TEST_ROOT)/src/usbhost_sdkraw_speed.c \
              $(FRAMEWORKS_USB_DDK_SPEED_TEST_ROOT)/src/usbhost_nosdk_speed.c
endif

endif
#LOCAL_CFLAGS += -fstack-protector-strong -Wextra -Wall -Werror -fsigned-char -fno-strict-aliasing -fno-common

include $(HDF_DRIVER)
