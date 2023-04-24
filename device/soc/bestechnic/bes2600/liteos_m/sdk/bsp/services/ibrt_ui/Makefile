cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

IBRT_LIB_NAME := libtws_ibrt_ui

IBRT_LIB_NAME := $(IBRT_LIB_NAME)_enhanced_stack

ifeq ($(BLE), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_ble
endif

ifeq ($(USE_TRACE_ID), 1)
IBRT_LIB_NAME := $(IBRT_LIB_NAME)_log_compressed
endif

$(IBRT_LIB_NAME)-y := $(src_obj)

obj-y :=  $(IBRT_LIB_NAME).a

subdir-ccflags-y += \
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
    -Iapps/key \
    -Iapps/main \
    -Iapps/battery \
    -Iapps/common \
    -Iapps/audioplayers \
    -Iapps/audioplayers/a2dp_decoder \
    -Iapps/factory \
    -Iservices/ai_voice/manager \
    $(BT_PROFILES_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iservices/osif \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/log_section \
    -Iservices/app_ibrt/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Ithirdparty/audio_codec_lib/liblhdc-dec/inc \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iutils/hwtimer_list \
    -Iservices/voicepath \
    -Iservices/ibrt_middleware/inc \
    -Iservices/app_ai/inc \
    -Iservices/ai_voice/protocol/gma/gma_crypto 

ifeq ($(IBRT),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core/inc \
    -Iservices/ibrt_ui/inc \
    -Iservices/bt_app
endif

