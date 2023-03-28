# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include $(LITEOSTOPDIR)/config.mk

MODULE_NAME := bsp_config

LOCAL_SRCS += $(wildcard *.c)

LOCAL_SRCS += $(wildcard amba_pl011/*.c)

ifeq ($(LOSCFG_SHELL), y)
LOCAL_SRCS += $(wildcard extcmd/*.c)
endif

ifeq ($(LOSCFG_PLATFORM_ADAPT), y)
LOCAL_SRCS += $(wildcard os_adapt/*.c)
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)

include $(MODULE)
