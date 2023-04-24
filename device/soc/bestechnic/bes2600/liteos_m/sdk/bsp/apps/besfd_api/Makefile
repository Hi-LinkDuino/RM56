cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += console/
ccflags-y +=  \
    $(BT_IF_INCLUDES) \
    -Iservices/a7_dsp/common \
    -Iapps/mic \
    -Iservices/bt_app \
    -Iservices/audioflinger \
    -Iservices/norflash_api \
    -Iservices/nvrecord \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/overlay \
    -Iservices/resources \
    -Iservices/audio_process \
    -Iapps/apptester \
    -Iapps/factory \
    -Iapps/factory_wifi \
    -Iapps/voice_detector \
    -Iutils/crc32 \
    -Iplatform/drivers/bt \
    -Iplatform/drivers/ana \
    -Iapps/audioplayers/rbplay \
    -Itests/anc_usb \
    -Iapps/anc/inc \
    -Ithirdparty/userapi \
    -Iservices/communication \
    -Iutils/cqueue \
    -Iservices/ai_voice/audio \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iservices/interconnection/red \
    -Iservices/interconnection/green \
    -Iservices/bridge/ \
    $(BT_PROFILES_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Ithirdparty/tile \
    -Iservices/ibrt_middleware/inc \
    -Ithirdparty/tile/tile_target \
    -Iinclude/ \
    $(EPLAYER_INCLUDES) \
    $(ECOMM_INCLUDES) \
    -Iapps/btwatch \
    -Iapps/sensorhub \
    -Iplatform/drivers/cp_accel \
    -Inet/mbedtls/include \
    -Inet/mbedtls/include/mbedtls \
    -Iservices/wifi_app/wifi_bt_slot_time
    

ccflags-y += -Iutils/kfifo \
			-Iplatform/drivers/wifi/$(CHIP) \
			-Inet/utils \
			-Inet/lwip_2.1.2/src/include/compat/posix \
			-Iservices/wifi_app/iperf \
			-Iservices/wifi_app/ping \
			-Iservices/wifi_app/sniffer \
			-Iservices/wifi_app/littlefs \
			-Iservices/wifi_app/littlefs/flash_partition \
			-Iservices/wifi_app/wifi_console \
			-Iapps/main


ifeq ($(BT_MEDIA_SUPPORT_LPLAYER),1)
ccflags-y += -Iservices/lplayer_wrapper
endif

ifeq ($(CHIP_HAS_RGB_LED),1)
ccflags-y += -Iapps/common
endif

ccflags-y += -Iservices/lplayer/interface/include -Iapps/common -Iconfig/best2600w_aiot