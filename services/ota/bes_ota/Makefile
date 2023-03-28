cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

OTA_LIB_NAME := lib_ota

ifeq ($(IBRT), 1)
OTA_LIB_NAME := $(OTA_LIB_NAME)_ibrt
else
OTA_LIB_NAME := $(OTA_LIB_NAME)_stereo
endif

ifeq ($(BLE), 1)
OTA_LIB_NAME := $(OTA_LIB_NAME)_ble
endif

$(OTA_LIB_NAME)-y := $(src_obj)

obj-y += $(OTA_LIB_NAME).a

export WIN_PLAT := n
ifeq ($(OS),Windows_NT)
# Detect Cygwin
ifneq ($(findstring /,$(PWD)),/)
# also $(findstring /,$(HOME)) ?
WIN_PLAT := y
endif
endif

# Git revision
ifeq ($(WIN_PLAT),y)
GIT_REVISION := $(shell (where git >nul 2>&1) && (git rev-parse --short HEAD 2>nul))
else
GIT_REVISION := $(shell (which git >/dev/null 2>&1) && (git rev-parse --short HEAD 2>/dev/null))
endif

ifneq ($(GIT_REVISION),)
ifeq ($(WIN_PLAT),y)
GIT_REVISION := $(GIT_REVISION)$(shell (git diff --quiet && git diff --cached --quiet) >nul 2>&1 || echo -$(VERSION_INFO))
else
GIT_REVISION := $(GIT_REVISION)$(shell (git diff --quiet && git diff --cached --quiet) >/dev/null 2>&1 || echo -$(VERSION_INFO))
endif
endif

CFLAGS_ota_control.o += -DGIT_VERSION=$(GIT_REVISION)

subdir-ccflags-y += \
    -Iplatform/hal \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    $(BT_IF_INCLUDES) \
    $(BT_PROFILES_INCLUDES) \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iservices/nvrecord \
    -Iservices/nv_section/factory_section \
    -Iservices/nv_section/userdata_section \
    -Iservices/bt_app \
    -Iservices/bt_app/a2dp_codecs/include \
    -Iservices/norflash_api \
    -Iservices/ota/bes_ota/inc \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iutils/boot_struct \
    -Iutils/crc32 \
    -Iutils/hwtimer_list \
    -Iutils/heap \
    -Iutils/cqueue \
    -Iapps/key \
    -Iapps/main \
    -Iapps/common \
    -Iapps/factory \
    -Iapps/audioplayers \
    -Iservices/ota

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
ifeq ($(IBRT_UI_V2),1)
ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif
endif

#ifeq ($(AI_OTA),1)
subdir-ccflags-y += -Iservices/ota/ai_ota
#endif
