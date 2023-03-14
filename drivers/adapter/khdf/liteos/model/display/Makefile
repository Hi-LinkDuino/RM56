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

ifeq ($(LOSCFG_DRIVERS_HDF_DISP), y)
MODULE_NAME := hdf_display

LOCAL_INCLUDE := $(LITEOSTOPDIR)/../../base/hiviewdfx/hilog_lite/interfaces/native/kits \
                 $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver \
                 $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/backlight


LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/hdf_disp.c \
                 $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/backlight/hdf_bl.c

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_PWM), y)
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/backlight/pwm_bl.c
endif

ifeq ($(LOSCFG_PLATFORM_HI3516DV300), y)
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/adapter_soc/hi35xx_disp.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_LCDKIT), y)
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/lcdkit/lite_lcdkit.c
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/lcdkit/lcdkit_parse_config.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_LCD_ICN9700), y)
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/panel/mipi_icn9700.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_LCD_ST7789), y)
LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/display/driver/panel/ssp_st7789.c
endif

endif
include $(HDF_DRIVER)
