
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s   := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c   := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y :=  $(src_obj)

subdir-ccflags-y += \
    -Iapps/key \
    -Iapps/main \
    -Iplatform/hal \
    -Iplatform/drivers/bt \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    $(BT_PROFILES_INCLUDES) \
    -Iservices/ai_voice/manager \
    -Iservices/ibrt_middleware/inc \
    -Iservices/bt_if_enhanced/inc \
    -Iservices/bt_app \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/voicepath \
    -Iservices/voicepath/gsound/gsound_target_api_read_only \
    -Iservices/voicepath/gsound/gsound_custom/inc \
    -Iservices/voicepath/gsound/gsound_target \
    -Ithirdparty/userapi \
    -Iservices/app_ai/inc \
    -Iutils/cqueue  \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iutils/hsm \
    -Iservices/ota

ifeq ($(BES_OTA), 1)
subdir-ccflags-y += \
    -Iservices/ota/bes_ota/inc
endif

ifeq ($(AI_OTA), 1)
subdir-ccflags-y += \
    -Iservices/ota/ai_ota
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI_V1),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt/inc \
    -Iservices/ibrt_core/inc \
    -Iservices/ibrt_ui/inc
endif
ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/ibrt_ui_v2/inc
endif
ifeq ($(IBRT_UI_V2),1)
subdir-ccflags-y += \
    -Iservices/ibrt_conn_mgr/inc
endif
endif
