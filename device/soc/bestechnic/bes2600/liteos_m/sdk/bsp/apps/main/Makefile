cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

ifneq ($(APP_TEST_MODE),1)
obj_y := $(filter-out apps_tester.c,$(obj_y))
endif

ifneq ($(wildcard $(cur_dir)../../config/$(T)/app_status_ind.c),)
obj-y += ../../config/$(T)/app_status_ind.c
else
obj-y += ../../config/$(DEFAULT_CFG_SRC)/app_status_ind.c
endif

ifeq ($(AUDIO_OUTPUT_DC_AUTO_CALIB),1)
obj-y += ../../utils/codec_calib/
ccflags-y += -Iutils/codec_calib
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
    $(BT_IF_INCLUDES) \
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

ifeq ($(ALSA_EN)_$(ALSA_V1_EN),1_1)
ccflags-y += \
    -Iservices/wifi_app/alsa_adapter_v1/
else
ccflags-y += \
    -Iservices/wifi_app/alsa_adapter/
endif

ifeq ($(IBRT),1)

ifeq ($(IBRT_UI_V1),1)
ccflags-y += \
    -Iservices/ibrt_core/inc \
    -Iservices/ibrt_ui/inc \
    -Iservices/app_ibrt/inc
endif
ifeq ($(IBRT_CORE_V2),1)
ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/custom_api
endif
ifeq ($(IBRT_UI_V2),1)
ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif

endif

ifeq ($(BISTO_ENABLE),1)
ccflags-y += \
    -Iservices/voicepath \
    -Iservices/voicepath/gsound/gsound_service \
    -Iservices/voicepath/gsound/gsound_target \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target_api_read_only
endif

ifeq ($(APP_TEST_AUDIO),1)
CFLAGS_apps_tester.o += -DAPP_TEST_AUDIO
endif

ifeq ($(OTA_ENABLE),1)
ccflags-y += -Iservices/ota
endif

ifeq ($(BES_OTA),1)
ccflags-y += -Iservices/ota/bes_ota/inc
endif

ifeq ($(AI_OTA),1)
ccflags-y += -Iservices/ota/ai_ota
endif

ifeq ($(WIFI_APP_SUPPORT),1)
ccflags-y += -Iservices/wifi_app/wifi_bt_slot_time/
endif

ifeq ($(DEBUG_SPP_LOG),1)
ccflags-y += -Iservices/wifi_app/wifi_console/
endif

CFLAGS_apps.o += $(LDS_SECTION_FLAGS)

ifeq ($(ANC_APP),1)
CFLAGS_apps_tester.o += -DANC_APP
CFLAGS_apps.o += -DANC_APP
endif

ifeq ($(VOICE_ASSIST_WD_ENABLED),1)
CFLAGS_apps.o += -DVOICE_ASSIST_WD_ENABLED
endif

ifeq ($(VOICE_ASSIST_PILOT_ANC_ENABLED),1)
CFLAGS_apps.o += -DVOICE_ASSIST_PILOT_ANC_ENABLED
endif

ifeq ($(RB_CODEC),1)
CFLAGS_apps.o += -DRB_CODEC
endif

ifeq ($(VOICE_PROMPT),1)
CFLAGS_apps.o += -DMEDIA_PLAYER_SUPPORT
endif

ifeq ($(ENGINEER_MODE),1)
CFLAGS_apps.o += -D__ENGINEER_MODE_SUPPORT__
endif

ifeq ($(MCU_HIGH_PERFORMANCE_MODE),1)
CFLAGS_apps.o += -DMCU_HIGH_PERFORMANCE_MODE
endif

ifeq ($(BTADDR_FOR_DEBUG),1)
CFLAGS_apps.o += -DBTADDR_FOR_DEBUG
endif

ifeq ($(FORCE_SIGNALINGMODE),1)
CFLAGS_apps.o += -DFORCE_SIGNALINGMODE
endif

ifeq ($(FORCE_NOSIGNALINGMODE),1)
CFLAGS_apps.o += -DFORCE_NOSIGNALINGMODE
endif

ifeq ($(POWERKEY_I2C_SWITCH),1)
CFLAGS_apps.o += -DPOWERKEY_I2C_SWITCH
endif

ifeq ($(ESHELL_ON),1)
ccflags-y += \
    $(ESHELL_INCLUDES)
endif

ifeq ($(LINEIN_TRANS_ON),1)
ccflags-y += -Iservices/besaud_service/
endif

ifeq ($(AUDIO_PROMPT_USE_DAC2_ENABLED),1)
CFLAGS_apps.o += -DAUDIO_PROMPT_USE_DAC2_ENABLED
endif

ifeq ($(AUDIO_ADAPTIVE_IIR_EQ),1)
CFLAGS_apps.o += -DAUDIO_ADAPTIVE_IIR_EQ
CFLAGS_apps.o += -DAUDIO_ADAPTIVE_EQ
endif

ifeq ($(AUDIO_ADAPTIVE_FIR_EQ),1)
CFLAGS_apps.o += -DAUDIO_ADAPTIVE_FIR_EQ
CFLAGS_apps.o += -DAUDIO_ADAPTIVE_EQ
endif

ifeq ($(AUDIO_DEBUG_CMD),1)
CFLAGS_apps.o += -DAUDIO_DEBUG_CMD
endif

ifeq ($(REMOVE_WEAK_SYMBOL),1)
CFLAGS_apps.o += -DREMOVE_WEAK_SYMBOL=1
endif