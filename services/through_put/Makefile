cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

subdir-ccflags-y += -Iapps/common \
                    -Iapps/key \
                    -Iapps/main \
                    -Iplatform/drivers/bt \
                    -Iplatform/hal \
                    -Iservices/ai_voice/audio \
                    -Iservices/ai_voice/transport \
                    -Iservices/ai_voice/manager \
                    -Iservices/ai_voice/protocol/ama/ama_stream \
                    -Iservices/app_ai/inc \
                    -Iservices/ibrt_middleware/inc \
                    $(BT_IF_INCLUDES) \
                    $(BLE_APP_INCLUDES) \
                    $(BLE_STACK_INCLUDES) \
                    -Iservices/bt_app \
                    -Iservices/bt_app/a2dp_codecs/include \
                    -Iservices/multimedia/audio/codec/sbc/inc \
                    -Iservices/multimedia/speech/inc \
                    -Iservices/nvrecord \
                    -Iservices/cp_accel \
                    -Iservices/voicepath \
                    -Iservices/voicepath/gsound/gsound_custom/inc \
                    -Iservices/voicepath/gsound/gsound_target \
                    -Iservices/voicepath/gsound/gsound_target_api_read_only \
                    -Ithirdparty/userapi \
                    -Iutils/cqueue \
                    -Iutils/heap

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