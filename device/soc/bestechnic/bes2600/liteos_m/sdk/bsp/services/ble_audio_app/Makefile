cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_bap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_arc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_acc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_cap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_atc/*.c))


BLE_AUDIO_LIB_NAME := libble_audio

$(BLE_AUDIO_LIB_NAME)-y := $(obj_c:.c=.o)

obj-y :=  $(BLE_AUDIO_LIB_NAME).a

subdir-ccflags-y += \
	$(BT_IF_INCLUDES) \
	$(BLE_APP_INCLUDES) \
	$(BLE_PROFILES_INCLUDES) \
	$(BLE_STACK_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	-Iservices/bt_app \
	-Iutils/cqueue \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iapps/bt_sync \
	-Iapps/common \
	-Iservices/audio_dump/include \
	-Iservices/multimedia/audio/codec/lc3/inc \
	-Iutils/hwtimer_list \
	-Iutils/lockcqueue \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/bt \
	-Iutils/heap \
	-Iservices/nvrecord \
	-Iapps/key \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iapps/main \
	-Iservices/resources \
	-Iapps/audioplayers \
	-Iapps/audioplayers/a2dp_decoder \
	-Iservices/ble_audio_mgr/inc/audio_policy/ \
	-Iservices/app_ibrt_v2/inc \
	-Iservices/ibrt_ui_v2/inc \
	-Iservices/ibrt_core_v2/inc \
	-Iservices/ibrt_middleware/inc \
	-Iservices/wifi_app/alsa_adapter \
	-Iservices/audio_process \
	-Iservices/aob_core/inc/audio_policy \
	-Iservices/multimedia/audio/process/resample/include \

ifeq ($(DSP_CODEC_SUPPORT),1)
subdir-ccflags-y += -Iservices/a7_dsp/common/codec
endif
