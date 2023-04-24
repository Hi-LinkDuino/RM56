
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

IBRT_LIB_NAME := libtws_ibrt

ifeq ($(ENHANCED_STACK),1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_enhanced_stack
endif

ifeq ($(BLE), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ble
endif

ifeq ($(SUPPORT_REMOTE_COD), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_cod
endif

ifeq ($(USE_TRACE_ID), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_log_compressed
endif

$(IBRT_LIB_NAME)-y := $(src_obj)

obj-y :=  $(IBRT_LIB_NAME).a

subdir-ccflags-y += \
    -Iservices/fs/fat \
    -Iservices/fs/sd \
    -Iservices/fs/fat/ChaN \
    -Iservices/overlay \
    -Iservices/nvrecord \
    -Iservices/resources \
    -Iservices/multimedia/audio/process/resample/include \
    $(BT_IF_INCLUDES) \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/audioflinger \
    -Iutils/lockcqueue \
    -Iutils/intersyshci \
    -Iapps \
    -Iapps/key \
    -Iapps/main \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/factory \
    -Iapps/bt_sync \
    -Iservices/ai_voice/manager \
    -Iservices/ibrt_middleware/inc \
    $(BT_PROFILES_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/bt_app/a2dp_codecs/include \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iutils/hwtimer_list \
    -Iservices/voicepath	\
    -Iservices/custom_api \
    -Iservices/app_ai/inc \
    -Iservices/tota_v2 \
    -Iservices/osif
	
ifeq ($(IBRT),1)
subdir-ccflags-y += \
    -Iservices/bt_app \
    -Iservices/app_ibrt_v2/inc
ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/custom_api \
    -Iservices/ibrt_ui_v2/inc \
    -Iutils/hsm
endif
endif

CFLAGS_app_tws_ibrt.o += -DBESLIB_INFO=$(BESLIB_INFO)

ifeq ($(AUDIO_PROMPT_USE_DAC2_ENABLED),1)
CFLAGS_app_ibrt_voice_report.o += -DAUDIO_PROMPT_USE_DAC2_ENABLED
endif
