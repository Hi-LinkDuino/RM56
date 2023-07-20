
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

WIFIAPP_LIB_NAME := libwifiapp

ifeq ($(NET_LWIP_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
obj-y += net_app/ipv4/
endif
ifeq ($(NET_LWIP_IPV6_SUPPORT),1)
obj-y += net_app/ipv6/
endif
ifeq ($(NET_LWIP_SNTP_SUPPORT),1)
obj-y += net_app/sntp/
endif
endif

ifeq ($(NET_PING_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
obj-y += ping/
endif
endif

ifeq ($(WIFI_NET_TOOL_SUPPORT), 1)
obj-y += net_test_tool/
obj-y += net_test_tool/ali_adaptation_layer/
subdir-ccflags-y += -Iservices/wifi_app/net_test_tool/
subdir-ccflags-y += -Iservices/wifi_app/net_test_tool/ali_adaptation_layer/
endif

ifneq ($(WIFI_RF_TEST), 1)
ifeq ($(SNIFFER_MODE_SUPPORT),1)
obj-y += sniffer/
endif
endif
ifneq ($(USE_EXT_CONSOLE), 1)
obj-y += wifi_console/
endif

ifeq ($(NET_WIFI_SOCKET_SUPPORT),1)
obj-y += wifi_socket/
endif

ifeq ($(WIFI_UNIT_SOCKET_TEST),1)
obj-y += wifi_unit_test/test_socket/
subdir-ccflags-y += -Iservices/wifi_app/wifi_unit_test/test_socket/
endif

ifeq ($(BWIFI_UNIT_TEST),1)
obj-y += wifi_unit_test/test_bwifi_interface/
subdir-ccflags-y += -Iservices/wifi_app/wifi_unit_test/test_bwifi_interface/
endif

ifeq ($(WIFI_VOICE_SUPPORT),1)
obj-y += wifi_voice/
endif

ifeq ($(DEVICE_VAD_SUPPORT),1)
obj-y += device_vad/
endif

ifeq ($(WIFI_IPERF_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
obj-y += iperf/
endif
endif

ifeq ($(WIFI_APP_SUPPORT),1)
obj-y += wifi_bt_slot_time/
endif

ifeq ($(VOWLAN_SUPPORT),1)
obj-y += vowlan/
subdir-ccflags-y +=-Iservices/wifi_app/vowlan/
subdir-ccflags-y +=-Iapps/vowlan
subdir-ccflags-y +=-Iapps/vowlan/i2s
subdir-ccflags-y +=-Iapps/vowlan/wi2s
subdir-ccflags-y +=-Iapps/vowlan/wbt_audio
subdir-ccflags-y +=-Iapps/vowlan/wusb
subdir-ccflags-y +=-Iapps/vowlan/wadc
subdir-ccflags-y +=-Iapps/vowlan/iic_trans_protocol/
endif

ifeq ($(WIFI_FAR_FIELD_SUPPORT),1)
obj-y += wifi_far_field/
endif

ifeq ($(ALSA_EN),1)
ifeq ($(ALSA_V1_EN), 1)
obj-y += alsa_adapter_v1/
else
obj-y += alsa_adapter/
endif
endif

ifeq ($(ALIOS_THINGS_RELEASE), 1)
obj-y += audio_dec/
endif

ifeq ($(BES_AIRKISS_SUPPORT), 1)
obj-y += bes_airkiss/
endif

ifeq ($(LITTLEFS_ENABLE),1)
obj-y += littlefs/
obj-y += vfs/
endif

ifeq ($(MMC_DISK_FATFS),1)
obj-y += mmc_disk/
subdir-ccflags-y += DSDMMC_DET_GPIO=SDMMC_DET_GPIO
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
obj-y += http_media/
subdir-ccflags-y += -DHISTREAMER_LITE_MANAGER_EN
else
ifeq ($(HISTREAMER_HTTP_HLS_EN), 1)
obj-y += http_media/
subdir-ccflags-y += -DHISTREAMER_LITE_MANAGER_EN
endif
endif

ifeq ($(LVGL_TEST),1)
obj-y += lvgl/
endif

ifeq ($(VOICE_DETECTOR_EN), 1)
obj-y += hw_vad/
endif

ifeq ($(OPENAI_SUPPORT), 1)
obj-y += openai/
subdir-ccflags-y += -DOPENAI_SUPPORT
endif # OPENAI_SUPPORT

ifeq ($(MAIN_CPU_SLEEP_TEST), 1)
subdir-ccflags-y += -DMAIN_CPU_SLEEP_TEST
endif

subdir-ccflags-y += -Iplatform/hal -Iplatform/cmsis -Iplatform/cmsis/inc -Inet/include/  -Inet/include/byteorder  -Inet/include/os  -Inet/include/unaligned
subdir-ccflags-y += -Inet -Inet/utils/ -Iutils/hwtimer_list -Iplatform/drivers/ana
subdir-ccflags-y += -Inet/wifi -Inet/wpa_supplicant_l/wpa_supplicant -Inet/wpa_supplicant_l/src/utils -Inet/wpa_supplicant_l/src/

ifneq ($(LIBC_ADAPT_EN),1)
subdir-ccflags-y += \
			-I$(LWIP_ROOT)/src/include/\
			-I$(LWIP_ROOT)/src/include/lwip \
			-I$(LWIP_ROOT)/src/include/$(POSIX_PATH)
endif # !LIBC_ADAPT_EN
subdir-ccflags-y += \
			-Iapps/common \
			-Iapps/key \
			-Iutils/kfifo \
			-Iservices/nvrecord\
			-Iservices/wifi_app/wifi_console/\
			-Iservices/wifi_app/wifi_socket/\
			-Iservices/wifi_app/net_app/ipv6/\
			-Iservices/wifi_app/ping/\
			-Iservices/wifi_app/iperf/ \
			-Iservices/wifi_app/sniffer/\
			-Iservices/wifi_app/bes_airkiss/ \
			-Iservices/wifi_app/wifi_voice/ \
			-Iservices/wifi_app/wifi_far_field/ \
			-Iservices/wifi_app/wifi_voice/res/ \
			-Iservices/wifi_app/alsa_adapter/ \
			-Iservices/wifi_app/audio_dec/ \
			-Iplatform/cmsis/inc/		\
			-Iplatform/drivers/wifi/$(CHIP)


$(WIFIAPP_LIB_NAME)-y := $(obj-y)
obj-y := $(WIFIAPP_LIB_NAME).a
