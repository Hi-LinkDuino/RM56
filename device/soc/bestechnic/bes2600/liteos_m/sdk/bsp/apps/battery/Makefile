cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

subdir-ccflags-y += -Iplatform/drivers/ana \
                    $(BT_IF_INCLUDES) \
                    -Iservices/ble_app/app_main


ifeq ($(VOICE_PROMPT),1)
CFLAGS_app_battery.o += -DMEDIA_PLAYER_SUPPORT
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
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc
endif
endif

ifeq ($(CHARGER_1802),1)
CFLAGS_app_battery.o += -DCHARGER_1802
endif

