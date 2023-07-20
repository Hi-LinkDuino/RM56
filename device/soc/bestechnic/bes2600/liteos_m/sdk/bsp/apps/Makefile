ifeq ($(WIFI_RF_TEST_ONLY),1)
obj-y := common/ main/ key/ pwl/ battery/ cmd/ mic/
else
obj-y := common/ main/ key/ pwl/ battery/ cmd/ mic/ audio_test/ bt_sync/
endif
obj-y += besfd_api/
ifneq ($(BT_CLOSE),1)
obj-y += audioplayers/
endif

ifeq ($(ENGINEER_MODE),1)
obj-y += factory/
endif

ifeq ($(WIFI_RF_TEST_MODULE),1)
obj-y += factory_wifi/
endif

ifeq ($(APP_TEST_AUDIO),1)
obj-y += apptester/
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
obj-y += usbaudio/
endif
ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
obj-y += btusbaudio/
obj-y += usbaudio/
endif

ifeq ($(WIFI_USB_AUDIO_SUPPORT),1)
obj-y += usbaudio/
endif

ifeq ($(USB_AUDIO_FS),1)
obj-y += usbaudio/
ccflags-y += -Iapps/usbaudio/
endif

ifeq ($(VOWLAN_SUPPORT),1)
obj-y += vowlan/
endif

ifeq ($(APP_TEST_SDMMC),1)
obj-y += sdmmc/
endif

ifeq ($(ANC_APP),1)
obj-y += anc/
endif

ifeq ($(ALIOS_APP_SUPPORT),1)
obj-y += alios/
endif

ifeq ($(ANC_ASSIST_ENABLED),1)
obj-y += voice_assist/
endif

ifeq ($(VOICE_DETECTOR_EN),1)
obj-y += voice_detector/
endif

ifeq ($(SENSOR_HUB),1)
obj-y += sensorhub/
endif

ifeq ($(AUDIO_HEARING_COMPSATN),1)
obj-y += hearing_detec/
endif

_APP_BT_WATCH ?= 0
ifeq ($(BT_WATCH_MASTER),1)
_APP_BT_WATCH := 1
endif
ifeq ($(BT_WATCH_SLAVE),1)
_APP_BT_WATCH := 1
endif
ifeq ($(_APP_BT_WATCH),1)
obj-y += btwatch/
endif

ifeq ($(HAS_BT_SYNC),1)
obj-y += bt_sync/
endif

ifeq ($(MASTER_APP_SUPPORT),1)
obj-y += bes_test/
endif

ifeq ($(TUYA_APP_SUPPORT),1)
obj-y += tuya/
endif

ifeq ($(APP_SLEEP_ENABLE),1)
obj-y += app_sleep/
endif

ifeq ($(MSMART_ENABLE),1)
obj-y += midea/
endif

ifeq ($(APP_RNN_VAD),1)
obj-y += app_rnn_vad/
endif

ifeq ($(XF_CHATGPT_TEST),1)
obj-y += xf_main_demo/
endif

subdir-ccflags-y += -Iapps/apptester \
					-Iapps/audioplayers \
					-Iapps/anc/inc \
					-Iapps/voice_assist/inc \
					-Iapps/common \
					-Iapps/sdmmc \
					-Iapps/main \
					-Iapps/cmd \
					-Iapps/key \
					-Iapps/pwl \
					-Iapps/master \
					-Iapps/battery \
					-Iservices/voicepath \
					-Iservices/audio_dump/include \
					-Iservices/ble_app/app_datapath \
					-Iservices/bt_app/a2dp_codecs/include \
					-Iutils/list \
					-Iutils/heap \
					-Iservices/multimedia/audio/process/filters/include

ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
subdir-ccflags-y += -Iapps/btusbaudio
endif

ifeq ($(A2DP_LDAC_ON),1)
subdir-ccflags-y += -Ithirdparty/audio_codec_lib/ldac/inc
endif

subdir-ccflags-y += -Iplatform/hal -Iplatform/cmsis -Iplatform/cmsis/inc
subdir-ccflags-y += -Inet/utils -Iutils/hwtimer_list -Iplatform/drivers/ana

ifeq ($(NET_LWIP_SUPPORT),1)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include/
endif
ifeq ($(WIFI_VOICE_SUPPORT),1)
subdir-ccflags-y += -Iservices/wifi_app/wifi_voice
endif
