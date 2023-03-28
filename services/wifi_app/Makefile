
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=
src_obj :=
WIFIAPP_LIB_NAME := libwifiapp
ifneq ($(SDK), 1)

ifeq ($(NET_LWIP_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
src_obj += net_app/
endif
endif

ifeq ($(NET_PING_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
src_obj += ping/
endif
endif

ifneq ($(WIFI_RF_TEST), 1)
ifeq ($(SNIFFER_MODE_SUPPORT),1)
src_obj += sniffer/
#src_obj += airkiss/
#src_obj += bes_airkiss/
endif
endif
ifneq ($(USE_EXT_CONSOLE), 1)
src_obj += wifi_console/
endif

ifeq ($(NET_WIFI_SOCKET_SUPPORT),1)
src_obj += wifi_socket/
endif

ifeq ($(WIFI_VOICE_SUPPORT),1)
src_obj += wifi_voice/
endif

ifneq ($(APP_DUER_SUPPORT),1)
#src_obj += device_vad/
endif

ifeq ($(WIFI_IPERF_SUPPORT),1)
ifneq ($(ALIOS_THINGS_RELEASE),1)
src_obj += iperf/
endif
endif

ifeq ($(WIFI_APP_SUPPORT),1)
src_obj += wifi_bt_slot_time/
endif

ifeq ($(VOWLAN_SUPPORT),1)
src_obj += vowlan/
subdir-ccflags-y +=-Iservices/wifi_app/vowlan/
subdir-ccflags-y +=-Iapps/vowlan
subdir-ccflags-y +=-Iapps/vowlan/i2s
subdir-ccflags-y +=-Iapps/vowlan/wi2s
subdir-ccflags-y +=-Iapps/vowlan/wbt_audio
subdir-ccflags-y +=-Iapps/vowlan/wusb
subdir-ccflags-y +=-Iapps/vowlan/wadc
subdir-ccflags-y +=-Iapps/vowlan/iic_trans_protocol/
endif

$(WIFIAPP_LIB_NAME)-y := $(src_obj)
endif

obj-y := $(WIFIAPP_LIB_NAME).a

#obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
#obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
#obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
#obj-y += $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

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

ifeq ($(LITTLEFS_ENABLE),1)
obj-y += littlefs/
obj-y += vfs/
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
obj-y += http_media/
else
ifeq ($(HISTREAMER_HTTP_HLS_EN), 1)
obj-y += http_media/
subdir-ccflags += DHISTREAMER_LITE_MANAGER_EN
endif
endif

ifeq ($(LVGL_TEST),1)
obj-y += lvgl/
endif

ifeq ($(VOICE_DETECTOR_EN), 1)
obj-y += hw_vad/
endif

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

