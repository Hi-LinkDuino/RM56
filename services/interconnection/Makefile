cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)umm_malloc/*.c))

ifeq ($(INTERACTION),1)
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)green/*.cpp))
endif

ifeq ($(INTERCONNECTION),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)red/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)red/*.cpp))
endif

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
					-Iplatform/hal \
					-Iservices/bluedroid/adapter/src \
					-Iapps/key \
					-Iservices/audio_process \
					$(BT_PROFILES_INCLUDES) \
					-Iservices/overlay \
					-Iservices/nvrecord \
					-Iservices/resources \
					-Iplatform/drivers/uarthci \
					-Iplatform/drivers/ana \
					-Iplatform/drivers/bt \
					-Iutils/cqueue \
					-Iutils/retention_ram \
					-Iservices/audioflinger \
					-Iutils/lockcqueue \
					-Iutils/intersyshci \
					-Iapps/main \
					-Iapps/common \
					-Iapps/audioplayers \
					-Iapps/factory \
					-Iservices/voicepath/gsound/gsound_target/ \
					-Iservices/bridge/ \
					$(BT_IF_INCLUDES) \
					$(BLE_APP_INCLUDES) \
					$(BLE_STACK_INCLUDES) \
					-Iservices/bt_app/ \
					-Iservices/bt_app/a2dp_codecs/include \
					-Iservices/interconnection/red \
					-Iservices/interconnection/green \
					-Iservices/interconnection/umm_malloc \
					-Iservices/ota \
					-Iservices/norflash_api \
					-Iservices/multimedia/opus121/include \
					-Iservices/multimedia/speech/application/Pcm8kCvsd/include \
					-Iservices/multimedia/speech/codec/codecs/adpcm \
					-Iservices/multimedia/speech/process/lc_mmse_ns/include \
					-Iservices/multimedia/speech/process/ns_and_ec/include \
					-Iservices/multimedia/speech/process/speech_2mic_ns2_denoise/include \
					-Iservices/multimedia/speech/process/dual_mic_denoise/include \
					-Iservices/multimedia/speech/process/far_field_speech_enhancement/include \
					-Iservices/multimedia/speech/process/plc/include \
					-Iservices/multimedia/speech/process/common/include \
					-Iservices/voicepath \
					-Iservices/nv_section/factory_section \
					-Iservices/voicepath/smartvoice  \
					-Iapps/battery   \
					-Ithirdparty/userapi \
					-Iutils/crc16 \
					-Iutils/crc32 \
					-Iutils/sha256	\
					-Iutils/aes256 \
					-Iutils/heap \
					-Iservices/multimedia/audio/codec/sbc/inc \
					-Iservices/norflash_api

ifeq ($(VOICE_PROMPT),1)
ccflags-y += -DMEDIA_PLAYER_SUPPORT
endif				

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI_V1),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt/inc \
    -Iservices/ibrt_ui/inc \
    -Iservices/ibrt_core/inc
endif
ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
endif