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
include $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos//lite.mk

MODULE_NAME := hdf_wifi_model
HDM_WIFI_ROOT = $(LITEOSTOPDIR)/../../drivers/framework/model/network/wifi
HDM_WIFI_LITE_ROOT = $(LITEOSTOPDIR)/../../drivers/adapter/khdf/liteos/model/network/wifi

##################path of compile file :start###############
##### hdm #####
#WPHY_PATH := $(HDM_WIFI_ROOT)/components/wphy
#WMAC_PATH := $(HDM_WIFI_ROOT)/components/wmac
WAP_PATH := $(HDM_WIFI_ROOT)/core/components/softap
WSTA_PATH := $(HDM_WIFI_ROOT)/core/components/sta
WP2P_PATH := $(HDM_WIFI_ROOT)/core/components/p2p
WEAPOL_PATH := $(HDM_WIFI_ROOT)/core/components/eapol
NETDEV_PATH := $(HDM_WIFI_ROOT)/../common/netdevice
MODULE_PATH := $(HDM_WIFI_ROOT)/core/module
PLATFORM_PATH := $(HDM_WIFI_ROOT)/platform
IBUS_PATH := $(HDM_WIFI_ROOT)/bus
QOS_PATH := $(PLATFORM_PATH)/src/qos
INC_PATH := $(HDM_WIFI_ROOT)/include
INC_WIFI_PATH := $(LITEOSTOPDIR)/../../drivers/framework/include/wifi
##################path of compile file :end###############

WIFI_CFLAGS += -I $(INC_PATH) \
		-I $(WSTA_PATH) \
		-I $(WAP_PATH) \
		-I $(WP2P_PATH) \
		-I $(WEAPOL_PATH) \
		-I $(NETDEV_PATH) \
		-I $(MODULE_PATH) \
		-I $(INC_WIFI_PATH) \
		-I $(IBUS_PATH)


LOCAL_SRCS += $(WAP_PATH)/ap.c \
			$(WEAPOL_PATH)/eapol.c \
			$(WSTA_PATH)/sta.c \
			$(WP2P_PATH)/p2p.c \
			$(NETDEV_PATH)/net_device.c \
			$(MODULE_PATH)/wifi_module.c \
			$(MODULE_PATH)/wifi_base.c \
			$(MODULE_PATH)/wifi_feature.c \
			$(HDM_WIFI_ROOT)/core/hdf_wifi_core.c \
			$(IBUS_PATH)/hdf_ibus_intf.c \
			$(IBUS_PATH)/hdf_sdio_intf.c

MESSAGE_INCLUDE += -I $(PLATFORM_PATH)/include/message \
			-I $(PLATFORM_PATH)/include

LOCAL_SRCS += $(PLATFORM_PATH)/src/message/nodes/local_node.c \
			$(PLATFORM_PATH)/src/message/message_dispatcher.c \
			$(PLATFORM_PATH)/src/message/message_router.c \
			$(PLATFORM_PATH)/src/message/sidecar.c \
			$(PLATFORM_PATH)/src/hdf_wlan_priority_queue.c \
			$(HDM_WIFI_LITE_ROOT)/platform/src/hdf_wlan_queue.c \
			$(QOS_PATH)/flow_control.c \
			$(QOS_PATH)/flow_control_task.c \
			$(PLATFORM_PATH)/src/hdf_wifi_event.c \
			$(PLATFORM_PATH)/src/hdf_wlan_utils.c \
			$(PLATFORM_PATH)/src/hdf_wlan_chipdriver_manager.c \
			$(PLATFORM_PATH)/src/hdf_wlan_power_manager.c \
			$(PLATFORM_PATH)/src/hdf_wlan_reset_manager.c \
			$(PLATFORM_PATH)/src/hdf_wlan_product.c \
			$(PLATFORM_PATH)/src/hdf_wlan_config_parser.c

LOCAL_CFLAGS += $(WIFI_CFLAGS) $(MESSAGE_INCLUDE)

LOCAL_CFLAGS += -fstack-protector-strong -fno-common -fsigned-char -fno-strict-aliasing

include $(HDF_DRIVER)


