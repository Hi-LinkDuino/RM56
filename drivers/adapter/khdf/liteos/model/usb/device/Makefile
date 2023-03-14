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

ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_DEVICE), y)
MODULE_NAME := hdf_usb_ddk_device

FRAMEWORKS_USB_DDK_DEVICE = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/ddk/device

USB_DEVICE_ROOT = $(LITEOSTOPDIR)/../../third_party/NuttX/drivers/usbdev

LOCAL_INCLUDE := $(USB_DEVICE_ROOT) \
                 $(USB_DEVICE_ROOT)/gadget/fconfig/include \
                 $(FRAMEWORKS_USB_DDK_DEVICE)/../../interfaces/ddk/common \
                 $(FRAMEWORKS_USB_DDK_DEVICE)/../../interfaces/ddk/device \
                 $(FRAMEWORKS_USB_DDK_DEVICE)/include

LOCAL_SRCS += $(FRAMEWORKS_USB_DDK_DEVICE)/src/adapter_if_liteos.c \
              $(FRAMEWORKS_USB_DDK_DEVICE)/src/usbfn_cfg_mgr.c \
              $(FRAMEWORKS_USB_DDK_DEVICE)/src/usbfn_dev_mgr.c \
              $(FRAMEWORKS_USB_DDK_DEVICE)/src/usbfn_io_mgr.c \
              $(FRAMEWORKS_USB_DDK_DEVICE)/src/usbfn_sdk_if.c

ifeq ($(LOSCFG_DRIVERS_HDF_USB_COMPOSITE), y)
LOCAL_SRCS += $(USB_DEVICE_ROOT)/gadget/composite.c \
              $(USB_DEVICE_ROOT)/gadget/composite_desc.c \
              $(USB_DEVICE_ROOT)/gadget/cdcacm.c \
              $(USB_DEVICE_ROOT)/gadget/f_acm_desc.c \
              $(USB_DEVICE_ROOT)/gadget/f_camera.c \
              $(wildcard $(USB_DEVICE_ROOT)/gadget/fconfig/src/*.c)
endif

ifeq ($(LOSCFG_DRIVERS_HDF_USB_DEVICE_DEMO_TEST), y)
FRAMEWORKS_USB_DEVICE_DEMO_ROOT = $(LITEOSTOPDIR)/../../drivers/peripheral/usb/gadget/function

LOCAL_INCLUDE += $(FRAMEWORKS_USB_DDK_DEVICE)/../../interfaces/ddk/common \
                 $(FRAMEWORKS_USB_DDK_DEVICE)/../../interfaces/ddk/device \
                 $(FRAMEWORKS_USB_DEVICE_DEMO_ROOT)/include \
                 $(LITEOSTOPDIR)/../../device/soc/hisilicon/common/hal/usb/hi3516dv300/include

LOCAL_SRCS += $(FRAMEWORKS_USB_DEVICE_DEMO_ROOT)/master/dev_master.c \
              $(FRAMEWORKS_USB_DEVICE_DEMO_ROOT)/acm/cdcacm.c \
              $(FRAMEWORKS_USB_DEVICE_DEMO_ROOT)/ecm/cdcecm.c
endif

endif

include $(HDF_DRIVER)
