
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/*/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/*/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/*/*.cpp))

ifeq ($(AUTO_TEST),1)
obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/test/*/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/test/*/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)protocols/test/*/*.cpp))
endif

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)common/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besport/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besport/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besport/*.cpp))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci/*.cpp))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)main/bt_schedule.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)applications/hshf/hshf_app.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)applications/hshf/bt_schedule.c))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lib_proc/*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lib_proc/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lib_proc/*.cpp))

ifeq ($(IBRT), 1)
BTPROFILE_LIB_NAME := ibrt_libbt_profiles
else
BTPROFILE_LIB_NAME := $(CHIP)_libbt_profiles
endif

ifeq ($(HFP_1_6_ENABLE), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_sbc_enc
endif

ifeq ($(ROM_UTILS_ON),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_romaac
endif

ifeq ($(BLE),1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_ble
endif

ifeq ($(BT_RF_PREFER), 2M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_2m
endif

ifeq ($(BT_RF_PREFER), 3M)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_3m
endif

ifeq ($(SUPPORT_REMOTE_COD), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_cod
endif

ifeq ($(USE_TRACE_ID), 1)
BTPROFILE_LIB_NAME := $(BTPROFILE_LIB_NAME)_log_compressed
endif

$(BTPROFILE_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(BTPROFILE_LIB_NAME).a

CFLAGS_a2dp.o += -O3

CFLAGS_btm.o += -DBESLIB_INFO=$(BESLIB_INFO)

subdir-ccflags-y += \
    -Iservices/osif/ \
    -Iservices/auto_test \
    $(BT_IF_INCLUDES) \
    $(BT_PROFILES_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iservices/bridge/ \
    -Iplatform/drivers/uarthci \
    -Iplatform/drivers/ana \
    -Iplatform/drivers/bt \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iutils/intersyshci \
    -Iservices/bt_app \
    -Iservices/audioflinger \
    -Iservices/nvrecord \
    -Iservices/resources \
    -Iservices/ibrt_core/inc \
    -Iutils/lockcqueue \
    -Iapps/key \
    -Iservices/multimedia/audio/codec/sbc/inc \
    -Iservices/multimedia/audio/codec/sbc/src/inc \
    -Iapps/audioplayers
