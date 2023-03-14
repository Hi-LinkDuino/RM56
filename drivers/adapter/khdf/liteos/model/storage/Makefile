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

MODULE_NAME := hdf_storage_driver

LOCAL_INCLUDE := $(LITEOSTOPDIR)/../../base/hiviewdfx/interfaces/kits \
                 $(LITEOSTOPDIR)/../../third_party/mtd-utils/include \
                 $(LITEOSTOPDIR)/../../drivers/khdf/liteos/model/storage/include \

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_MMC), y)
    LOCAL_SRCS += src/mmc/mmc_block_lite.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_block.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_core.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_dispatch.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_if.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_protocol.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_sdio.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/mmc_emmc.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_SDIO), y)
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/sdio_if.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_PLATFORM_EMMC), y)
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mmc/emmc_if.c
endif

ifeq ($(LOSCFG_DRIVERS_HDF_STORAGE_MTD), y)
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mtd/mtd_core.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mtd/mtd_spi_common.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mtd/mtd_char.c
    LOCAL_SRCS += $(LITEOSTOPDIR)/../../drivers/framework/model/storage/src/mtd/mtd_block.c
    LOCAL_SRCS += src/mtd/mtd_legacy_lite.c
    LOCAL_SRCS += src/mtd/mtd_char_lite.c
    LOCAL_SRCS += src/mtd/mtd_block_lite.c
endif

include $(HDF_DRIVER)
