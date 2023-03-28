cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y += \
	$(BT_IF_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	-Iservices/ibrt_middleware/inc \
	-Iapps/audioplayers/a2dp_decoder \
	-Iservices/bt_app \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iservices/resources \
	-Iplatform/drivers/bt \
	-Iutils/cqueue

ifeq ($(IBRT_UI_V2),1)
ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif