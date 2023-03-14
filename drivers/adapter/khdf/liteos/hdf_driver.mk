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

HC_GEN_DIR = $(abspath $(LITEOSTOPDIR)/../../drivers/framework/tools/hc-gen)
HC_GEN = $(OUT)/hc_gen_build/hc-gen
HDF_CONFIG_DIR = $(or $(LOCAL_HCS_ROOT),.)
CONFIG_OUT_DIR = $(OUT)/hcs_generated/$(MODULE_NAME)
CONFIG_GEN_SRCS = $(addsuffix .c,$(addprefix $(CONFIG_OUT_DIR)/,$(basename $(LOCAL_HCS_SRCS))))
DEPENDS_CONFIG_SRCS = $(addsuffix .c,$(addprefix $(CONFIG_OUT_DIR)/,$(basename $(LOCAL_DEPENDS_HCS_SRCS))))
CONFIG_GEN_HEX_SRC = $(addsuffix _hex.c,$(addprefix $(CONFIG_OUT_DIR)/,$(basename $(LOCAL_PLATFORM_HCS_SRC))))

LOCAL_SRCS += $(CONFIG_GEN_SRCS) $(CONFIG_GEN_HEX_SRC)
LOCAL_INCLUDE += $(dir $(CONFIG_GEN_SRCS) $(DEPENDS_CONFIG_SRCS))
LOCAL_CFLAGS += $(addprefix -I ,$(LOCAL_INCLUDE))

$(HC_GEN):
	$(HIDE)make -j -C $(HC_GEN_DIR) BUILD_DIR=$(dir $@)

$(CONFIG_GEN_HEX_SRC): $(CONFIG_OUT_DIR)/%_hex.c: $(HDF_CONFIG_DIR)/%.hcs | $(HC_GEN) $(dir $(CONFIG_GEN_HEX_SRC))
	$(HIDE)echo gen hdf built-in config
	$(HIDE)$(HC_GEN) -b -i -a -o $(subst _hex.c,,$(@)) $<

$(CONFIG_GEN_SRCS): $(CONFIG_OUT_DIR)/%.c: $(HDF_CONFIG_DIR)/%.hcs | $(HC_GEN) $(dir $(CONFIG_GEN_SRCS))
	$(HIDE)echo gen hdf driver config
	$(HIDE)$(HC_GEN) -t -o $@ $<

$(DEPENDS_CONFIG_SRCS): $(CONFIG_OUT_DIR)/%.c: $(HDF_CONFIG_DIR)/%.hcs | $(HC_GEN) $(dir $(DEPENDS_CONFIG_SRCS))
	$(HIDE)$(HC_GEN) -t -o $@ $<
	$(HIDE)rm $@

include $(MODULE)
