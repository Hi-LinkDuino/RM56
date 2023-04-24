cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))


obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_main/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_dis/*.c))

ifneq ($(__BES_OTA_MODE__),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_htp/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_datapath/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_hrps/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_hid/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_sec/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_batt/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_voice/app_$(VOICE_DATAPATH_TYPE)/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_ota/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_gfps/*.c))

ifeq ($(ANCS),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_ancs/*.c))
endif

ifeq ($(ANCC),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_ancc/*.c))
endif

ifeq ($(AMSC),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_amsc/*.c))
endif

ifneq ($(VOICE_DATAPATH_TYPE),)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_voice/*.c))
endif
endif

ifeq ($(DCOMMS),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_datacomm/server/*.c))
subdir-ccflags-y += \
    -Iservices/ble_app_v2/app_datacomm/server \
    -Iservices/ble_profiles_v2/datacomm/server/api
endif

ifeq ($(DCOMMC),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_datacomm/client/*.c))
endif

ifeq ($(TEST_OVER_THE_AIR),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/app_tota/*.c))
subdir-ccflags-y += \
    -Iservices/tota/ \
    -Iservices/ble_profiles_v2/tota/\
    -Iservices/ble_app_v2/app_tota/
endif

ifeq ($(SWIFT_ENABLE),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)app_swift/*.c))
subdir-ccflags-y += \
    -Iservices/ble_app_v2/app_swift/
endif

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += \
    -Iservices/audio_process \
    -Iapps/audioplayers/a2dp_decoder \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    $(BLE_APP_INCLUDES) \
    $(BLE_PROFILES_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    $(BT_IF_INCLUDES) \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/overlay \
    -Iservices/nvrecord \
    -Iservices/resources \
    -Iservices/voicepath/ \
    -Iservices/voicepath/$(VOICE_DATAPATH_TYPE) \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/retention_ram \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iutils/heap \
    -Iutils/hsm \
    -Iutils/crc16 \
    -Iutils/hwtimer_list \
    -Iapps/battery \
    -Iapps/key \
    -Iapps/main \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/factory \
    -Iservices/audio_dump/include \
    -Iservices/bridge/ \
    -Iservices/multimedia/speech/inc \
    -Iservices/ble_profiles_v2/voicepath/$(VOICE_DATAPATH_TYPE)/ \
    -Iservices/ble_app_v2/app_voice/app_$(VOICE_DATAPATH_TYPE)/ \
    -Iservices/bt_app \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_custom/src \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/ai_voice/manager \
    -Iservices/ai_voice/transport \
    -Iservices/app_ai/inc \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iservices/multimedia/audio/codec/lc3/fixed_point/inc \
    -Iservices/ibrt_middleware/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/voicepath/gsound/gsound_target \
    -Ithirdparty/tile \
    -Ithirdparty/tile/tile_target \
    -Ithirdparty/tile/tile_common/tile_assert \
    -Ithirdparty/tile/tile_common/tile_features \
    -Ithirdparty/tile/tile_common/tile_gatt_db \
    -Ithirdparty/tile/tile_common/tile_player \
    -Ithirdparty/tile/tile_common/tile_service \
    -Ithirdparty/tile/tile_common/tile_storage \
    -Iservices/ota\
    -Iservices/ble_profiles_v2/hogp \
    -Iservices/ble_profiles_v2/hogp/hogpbh/api \
    -Iservices/ble_profiles_v2/hogp/hogpbh/src \
    -Iservices/ble_profiles_v2/hogp/hogpd/api \
    -Iservices/ble_profiles_v2/hogp/hogpd/src \
    -Iservices/ble_profiles_v2/hogp/hogprh/api \
    -Iservices/ble_profiles_v2/hogp/hogprh/src \
    -Iservices/ble_profiles_v2/bas/basc/api \
    -Iservices/ble_profiles_v2/bas/basc/src \
    -Iservices/ble_profiles_v2/bas/bass/api \
    -Iservices/ble_profiles_v2/bas/bass/src \
    -Iservices/ble_profiles_v2/dis/diss/api  \
    -Iservices/ble_profiles_v2/dis/diss/src  \
    -Iservices/ble_app_v2/app_dis/      \
	-Iservices/ble_stack_v2/ip/ble/hl/api
	

ifeq ($(AUDIO_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -D__AUDIO_RESAMPLE__
endif
ifeq ($(SW_PLAYBACK_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -DSW_PLAYBACK_RESAMPLE
endif
ifeq ($(SW_CAPTURE_RESAMPLE),1)
CFLAGS_voice_over_ble.o += -DSW_CAPTURE_RESAMPLE
endif
ifeq ($(BES_OTA_BASIC),1)
ifeq ($(IBRT_OTA),1)
ccflags-y += -Iservices/ibrt_ota/inc
else
ccflags-y += -Iservices/ota
endif
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
	-Iservices/ibrt_core_v2/inc \
	-Iservices/ibrt_ui_v2/inc \
    -Iservices/app_ibrt_v2/inc

endif
endif

ifeq ($(BES_OTA),1)
ccflags-y += -Iservices/ota/bes_ota/inc
endif

ifeq ($(BISTO_ENABLE),1)
ccflags-y += \
    -Iservices/ble_profiles_v2/voicepath/$(VOICE_DATAPATH_TYPE)/ \
    -Iservices/ble_app_v2/app_voice/app_$(VOICE_DATAPATH_TYPE)/ \
    -Iservices/ble_profiles_v2/anc/ancc/api/ \
    -Iservices/ble_profiles_v2/ams/amsc/api/ \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_custom/src \
    -Iservices/voicepath/gsound/gsound_target_api_read_only
endif
