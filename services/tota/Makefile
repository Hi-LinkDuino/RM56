cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

TOTA_LIB_NAME := libtota

ifeq ($(USE_TRACE_ID), 1)
TOTA_LIB_NAME := $(TOTA_LIB_NAME)_log_compressed
endif

$(TOTA_LIB_NAME)-y := $(src_obj)

obj-y += $(TOTA_LIB_NAME).a

subdir-ccflags-y += -Iplatform/drivers/ana \
					-Iplatform/hal \
					-Iservices/bluedroid/adapter/src \
					-Iapps/key \
					-Iservices/audio_eq \
					$(BT_IF_INCLUDES) \
					$(BLE_APP_INCLUDES) \
					$(BLE_PROFILES_INCLUDES) \
					$(BLE_STACK_INCLUDES) \
					-Iservices/overlay \
					-Iservices/nvrecord \
					-Iservices/resources \
					-Iservices/multimedia/audio/codec/sbc/inc \
					-Iservices/multimedia/audio/codec/sbc/src/inc \
					-Iservices/bt_app/a2dp_codecs/include \
					-Iplatform/drivers/uarthci \
					-Iplatform/drivers/ana \
					-Iplatform/drivers/bt \
					-Iutils/cqueue \
					-Iutils/heap \
					-Iutils/retention_ram \
					-Iservices/audioflinger \
					-Iutils/lockcqueue \
					-Iutils/intersyshci \
                    -Iutils/boot_struct \
					-Iapps/main \
					-Iapps/common \
					-Iapps/audioplayers \
					-Iapps/factory \
					-Iservices/bridge/ \
					-Iservices/bt_app \
					-Iservices/multimedia/speech/codec/codecs/adpcm \
					-Iservices/anc_spp_tool \
                    -Iservices/tws/inc \
					-Iservices/ibrt_core/inc \
					-Iutils/crc32 \
					-Ithirdparty/userapi \
					-Iapps/battery \
					-Iservices/nv_section/factory_section \
					-Iservices/nv_section/log_section
					
ifeq ($(IBRT),1)
ifeq ($(IBRT_UI_V1),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt/inc \
    -Iservices/ibrt_core/inc
endif
ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
endif