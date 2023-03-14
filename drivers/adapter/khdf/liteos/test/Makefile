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
include ./test_lite.mk

MODULE_NAME := hdf_test
HDF_TEST_FRAMWORK_ROOT = $(LITEOSTOPDIR)/../../drivers/framework/test/unittest

LOCAL_SRCS := $(HDF_TEST_FRAMWORK_ROOT)/common/hdf_main_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/manager/sample_driver_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_test_entry.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_all_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_file_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_list_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_work_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/osal/osal_get_case_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/utils/hcs_parser/unittest/hcs_parser_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/utils/hcs_parser/unittest/hcs_config_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/pm/hdf_pm_driver_test.c

ifeq ($(LOSCFG_DRIVERS_HDF_WIFI), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/wifi/hdf_wifi_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/netdevice/net_device_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/module/hdf_module_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/net/hdf_netbuf_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/qos/flow_control_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/message/hdf_queue_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/network/wifi/unittest/message/hdf_single_node_message_test.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_DEVICE), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/model/usb/device/src/hdf_usb_device_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/device/src/usb_device_lite_cdcacm_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/device/src/usb_device_lite_sdk_io_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/device/src/usb_device_lite_sdk_if_test.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/platform_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/platform_event_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/platform_queue_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/platform_device_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/platform_manager_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_platform_core_entry_test.c
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_GPIO), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/gpio_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/gpio_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_gpio_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PIN), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/pin_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/pin_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_pin_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_I2C), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/i2c_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/i2c_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_i2c_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_I2S), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/i2s_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_i2s_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_ADC), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/adc_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/adc_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_adc_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_DAC), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/dac_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/dac_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_dac_entry_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/virtual/dac_virtual.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_I3C), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/i3c_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/i3c_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_i3c_entry_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/virtual/i3c_virtual.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_WATCHDOG), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/watchdog_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/watchdog_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_watchdog_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_SPI), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/spi_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/spi_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_spi_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_UART), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/uart_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/uart_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_uart_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_RTC), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/rtc_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/rtc_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_rtc_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_SDIO), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/sdio_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_sdio_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_DSI), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/mipi_dsi_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_mipi_dsi_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_CSI), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/mipi_csi_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_mipi_csi_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_HDMI), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/hdmi_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_hdmi_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PCIE), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/pcie_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_pcie_entry_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/virtual/pcie_virtual.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_EMMC), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/emmc_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_emmc_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PWM), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/pwm_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/pwm_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_pwm_entry_test.c
endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_TIMER), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/timer_driver_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/timer_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_timer_entry_test.c
endif

endif
ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_REGULATOR), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/common/regulator_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/entry/hdf_regulator_entry_test.c
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/platform/virtual/regulator_virtual.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_SENSOR), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/sensor/hdf_sensor_test.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_USB_DDK_HOST), y)
ifeq ($(LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY), y)
LOCAL_SRCS += $(HDF_TEST_FRAMWORK_ROOT)/model/usb/host/src/usb_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/host/src/usb_raw_test.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/host/src/usb_raw_io.c \
              $(HDF_TEST_FRAMWORK_ROOT)/model/usb/host/src/hdf_usb_entry_test.c
endif
endif

LOCAL_FLAGS += $(LITEOS_GCOV_OPTS)
LOCAL_CFLAGS += -fstack-protector-strong

include $(HDF_DRIVER_TEST)
