cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

CUSTOM_API_LIB_NAME := libcustom_api

ifeq ($(IBRT_UI_V2),1)
CUSTOM_API_LIB_NAME := $(CUSTOM_API_LIB_NAME)_multipoint
endif

ifeq ($(USE_TRACE_ID), 1)
CUSTOM_API_LIB_NAME := $(CUSTOM_API_LIB_NAME)_log_compressed
endif

$(CUSTOM_API_LIB_NAME)-y := $(obj-y)

obj-y :=  $(CUSTOM_API_LIB_NAME).a

ccflags-y += \
    -Iapps \
    -Iapps/main \
    -Iapps/common \
    -Iapps/key \
    -Iservices/bt_app \
    -Iservices/bt_if_enhanced/inc \
    -Iservices/bt_profiles/inc/sys \
    -Iplatform/drivers/bt \
    -Iservices/bt_client \
    -Iutils/cqueue \
    -Iservices/multimedia/audio/codec/sbc/inc \
	-Iservices/nvrecord	\
	-Iutils/heap	\
	-Iservices/nv_section/factory_section	\
	-Iservices/bt_profiles_enhanced/inc \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/ibrt_ui_v2/inc/ \
	$(BT_PROFILES_INCLUDES) \
	$(BLE_APP_INCLUDES) \
	$(BLE_STACK_INCLUDES)

ifeq ($(IBRT),1) 
subdir-ccflags-y += \
    -Iservices/ibrt_middleware/inc
	
ifeq ($(IBRT_CORE_V2),1)
subdir-ccflags-y += \
    -Iservices/ibrt_core_v2/inc
endif		 
endif