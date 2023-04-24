cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
ifeq ($(REL_AT_C),1)
obj-y := $(filter-out at.c,$(obj-y))
endif
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)
ccflags-y := -Iutils/hexdump/ -Inet/wpa_supplicant_l/src/utils/ -Inet/wifi/ -Iplatform/drivers/wifi/ -Inet/ftp/ftp_client/ -Iservices/wifi_app/wifi_socket/ -Iutils

ifeq ($(CMSIS_API_TEST),1)
obj-y += ../../../tests/cmsis/
ccflags-y += -Itests/cmsis/
endif

ifeq ($(A7_DSP_ENABLE),1)
ccflags-y += -Iservices/a7_dsp/common/
endif

ifeq ($(MCU_DSP),1)
ccflags-y += -Iservices/mcu_dsp/common/
endif

ifeq ($(CP_DSP),1)
ccflags-y += -Iservices/cp_dsp/common/
endif

ifneq ($(DEBUG_PORT),)
ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifeq ($(BES_OTA_ENABLED),1)
ccflags-y += -Iapps/ota/
endif

ccflags-y += -Iservices/nvrecord/ -Iservices/nv_section/factory_section/
ifeq ($(FREERTOS),FREERTOS)
ccflags-y += -Iinclude/rtos/freertos/
endif

ifeq ($(CHIP_OCD), 1)
ccflags-y += -Iutils/ocd/
endif

ifeq ($(SUPPORT_IR),1)
ccflags-y += -Iapps/bes_test/ir
ccflags-y += -Iapps/ir
ccflags-y += -Iplatform/drivers/ir/inc
endif

ifeq ($(BT_CLOSE),0)
ccflags-y += \
	-Iplatform/drivers/bt
endif

ifeq ($(LPLAYER_SUPPORT_AT_CMD),1)
ccflags-y += -Iservices/lplayer/interface/include/
ccflags-y += -Iservices/lplayer/component/audio_sink/include/
ccflags-y += -Iservices/lplayer/component/include/
endif

ifeq ($(ALSA_EN), 1)
ccflags-y += -Iservices/wifi_app/alsa_adapter/
ccflags-y += -Iservices/wifi_app/alsa_adapter/core/include/config/
endif

ifeq ($(CHIP),best2001)
ccflags-y += -Iplatform/hal/best2001/
endif
ifeq ($(CHIP),best2003)
ccflags-y += -Iplatform/hal/best2003/
endif

ifeq ($(USB_AUDIO_FS),1)
ccflags-y += -Iplatform/drivers/usb/usb_dev/inc/
endif

ifeq ($(USE_BT_ADAPTER),1)
ccflags-y += -Iservices/bluetooth_adapter/harmony
endif

ifeq ($(SPEECH_MSG_TEST_CASE),1)
ccflags-y += -Iservices/a7_dsp/os/test/speech_msg_test/
endif

ifeq ($(NET_TELNET_SERVER_SUPPORT),1)
ccflags-y += -Inet/telnet/ -D__NET_TELNET_SERVER_SUPPORT__
endif

ifeq ($(BLE_WIFICONFIG),1)
ccflags-y +=-Iservices/ble_app_v2/app_datapath \
            -Iservices/ble_stack_v2/modules/rwip/api \
            -Iservices/ble_stack_v2/modules/rwip  \
            -Iservices/ble_stack_v2/ip/ble/hl/inc \
            -Iservices/ble_stack_v2/ip/ble/hl/api \
            -Iservices/ble_stack_v2/modules/ke/api
endif
ccflags-y +=  -Iplatform/hal \
			  -Iutils/cqueue \
			  -Iapps/audioplayers \
			  -Iservices/bt_profiles/inc/ \
			  -Iservices/bt_profiles/inc/sys/ \
			  -Iservices/bluetooth_adapter/ \
			  -Iservices/bt_app \
			  -Iservices/resources \
			  -Iutils/heap \
			  -Iservices/bt_if_enhanced/inc \
			  -Iapps/key \
			  -Iservices/bt_if_enhanced/conmgr \
			  -Iservices/multimedia/audio/codec/sbc/inc \
			  -Iservices/multimedia/audio/codec/sbc/src/inc \
			  -Iservices/bt_app/a2dp_codecs/include \
			  -Iplatform/drivers/bt \
			  -Iservices/nv_section/userdata_section \

ifeq ($(BLE_V2),1)
ccflags-y += -Iservices/ble_stack_v2/hl/api \
			-Iservices/ble_app_v2/app_main \
			-Iservices/ble_stack_v2/modules/common/api \
			-Iservices/app_ibrt_v2/inc \
			-Iservices/ibrt_core_v2/inc \
			-Iservices/ibrt_middleware/inc \
			-Iservices/ble_app_v2 \
			-Iservices/ibrt_ui_v2/inc
endif

ccflags-y += -Iutils/kfifo \
			-Iplatform/drivers/wifi/$(CHIP) \
			-Inet/utils \
			-Inet/lwip_2.1.2/src/include/compat/posix \
			-Iservices/wifi_app/iperf \
			-Iservices/wifi_app/ping \
			-Iservices/wifi_app/sniffer \
			-Iservices/wifi_app/littlefs \
			-Iservices/wifi_app/littlefs/flash_partition \
			-Iapps/main


ifeq ($(BT_MEDIA_SUPPORT_LPLAYER),1)
ccflags-y += -Iservices/lplayer_wrapper
endif

ifeq ($(CHIP_HAS_RGB_LED),1)
ccflags-y += -Iapps/common
endif

ccflags-y += -Iapps/besfd_api -Iservices/wifi_app/wifi_console
