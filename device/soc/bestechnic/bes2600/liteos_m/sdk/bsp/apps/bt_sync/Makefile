cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))


obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	$(BT_IF_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	$(BLE_APP_INCLUDES) \
	$(BLE_STACK_INCLUDES) \
	-Iservices/bt_app \
	-Iplatform/drivers/bt  \
	-Iplatform/hal  \
	-Iservices/bt_app/a2dp_codecs/include 	\
	-Iservices/multimedia/audio/codec/sbc/inc 	\
	-Iservices/ibrt_middleware/inc  \
	-Iutils/cqueue

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