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

MODULE_NAME := hdf_platform
HDF_FRAMEWORKS = $(LITEOSTOPDIR)/../../drivers/framework

LOCAL_INCLUDE := $(LITEOSTOPDIR)/../../base/hiviewdfx/interfaces/kits \
                 $(LITEOSTOPDIR)/../../third_party/mtd-utils/include \
                 $(LITEOSTOPDIR)/../../drivers/khdf/liteos/model/storage/include \

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM), y)
    LOCAL_SRCS += ./src/plat_common.c \
    $(HDF_FRAMEWORKS)/support/platform/src/fwk/platform_device.c \
    $(HDF_FRAMEWORKS)/support/platform/src/fwk/platform_event.c \
    $(HDF_FRAMEWORKS)/support/platform/src/fwk/platform_manager.c \
    $(HDF_FRAMEWORKS)/support/platform/src/fwk/platform_queue.c \
    $(HDF_FRAMEWORKS)/support/platform/src/fwk/platform_common.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_I2C), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/i2c/i2c_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/i2c/i2c_core.c
    LOCAL_SRCS += ./src/i2c_dev.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_ADC), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/adc/adc_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/adc/adc_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_DAC), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/dac/dac_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/dac/dac_core.c
endif

LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/i3c/i3c_if.c \
$(HDF_FRAMEWORKS)/support/platform/src/i3c/i3c_core.c

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_GPIO), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/gpio/gpio_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/gpio/gpio_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/gpio/gpio_manager.c \
    $(HDF_FRAMEWORKS)/support/platform/src/gpio/gpio_service.c
    LOCAL_SRCS += ./src/gpio_dev.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PIN), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/pin/pin_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/pin/pin_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_WATCHDOG), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/watchdog/watchdog_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/watchdog/watchdog_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_SPI), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/spi/spi_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/spi/spi_core.c
    LOCAL_SRCS += ./src/spi_dev.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PWM), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/pwm/pwm_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/pwm/pwm_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_HDMI), y)
    LOCAL_SRCS += \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_cec.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_common.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_ddc.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_dfm.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_dispatch.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_edid.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_event.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_frl.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_hdcp.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_hdr.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_infoframe.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_ncts.c \
    $(HDF_FRAMEWORKS)/support/platform/src/hdmi/hdmi_scdc.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PCIE), y)
    LOCAL_SRCS += \
    $(HDF_FRAMEWORKS)/support/platform/src/pcie/pcie_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/pcie/pcie_dispatch.c \
    $(HDF_FRAMEWORKS)/support/platform/src/pcie/pcie_if.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_RTC), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/rtc/rtc_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/rtc/rtc_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/rtc/rtc_base.c \
    $(HDF_FRAMEWORKS)/support/platform/src/rtc/rtc_service.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_DSI), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/mipi/mipi_dsi_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/mipi/mipi_dsi_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_CSI), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/mipi/mipi_csi_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/mipi/mipi_csi_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_DMAC), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/dma/dmac_core.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_UART), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/uart/uart_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/uart/uart_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/uart/uart_service.c
    LOCAL_SRCS += ./src/uart_dev.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_I2S), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/i2s/i2s_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/i2s/i2s_core.c
endif


ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_REGULATOR), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/regulator/regulator_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/regulator/regulator_core.c \
    $(HDF_FRAMEWORKS)/support/platform/src/regulator/regulator_tree_mgr.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_TIMER), y)
    LOCAL_SRCS += $(HDF_FRAMEWORKS)/support/platform/src/timer/timer_if.c \
    $(HDF_FRAMEWORKS)/support/platform/src/timer/timer_core.c
endif

include $(HDF_DRIVER)
