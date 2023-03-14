# Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
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

MODULE_NAME := hdf_audio_driver

KHDF_AUDIO_BASE_ROOT_DIR = $(LITEOSTOPDIR)/../../
KHDF_AUDIO_ROOT_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/framework/model/audio
KHDF_FRAMEWORK_ROOT_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/framework
KHDF_AUDIO_HI3516DV300_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/drivers/peripheral/audio/chipsets/hi3516dv300

LOCAL_INCLUDE := $(KHDF_AUDIO_BASE_ROOT_DIR)/third_party/bounds_checking_function/include \
                 $(KHDF_AUDIO_KHDF_ROOT_DIR)/osal/include \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/core \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/utils \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/osal \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/platform \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/config \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/include/audio \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/core/include \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/sapm/include \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/dispatch/include \
                 $(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/common/include \
                 $(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/include \
                 $(KHDF_AUDIO_HI3516DV300_DIR)/codec/include \
                 $(KHDF_AUDIO_HI3516DV300_DIR)/soc/include \
                 $(KHDF_AUDIO_HI3516DV300_DIR)/dsp/include \
                 $(LITEOSTOPDIR)/kernel/base/include

LOCAL_SRCS += $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_core.c \
              $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_host.c \
              $(KHDF_AUDIO_ROOT_DIR)/core/src/audio_parse.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_accessory_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_codec_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_platform_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_dsp_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_dai_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/common/src/audio_dma_base.c \
              $(KHDF_AUDIO_ROOT_DIR)/sapm/src/audio_sapm.c \
              $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_stream_dispatch.c \
              $(KHDF_AUDIO_ROOT_DIR)/dispatch/src/audio_control_dispatch.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/src/tfa9879_accessory_adapter.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/../tfa9879/accessory/src/tfa9879_accessory_impl.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_adapter.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_impl.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/codec/src/hi3516_codec_ops.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/dsp/src/dsp_adapter.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/dsp/src/dsp_ops.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_dai_adapter.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_dai_ops.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_aiao_impl.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_dma_ops.c \
              $(KHDF_AUDIO_HI3516DV300_DIR)/soc/src/hi3516_dma_adapter.c

LOCAL_CFLAGS += -Wno-error
include $(HDF_DRIVER)