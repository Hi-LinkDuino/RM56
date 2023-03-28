ifdef DEV

BLE_PROFILE_LIB_NAME := libble_profile
obj-y := $(BLE_PROFILE_LIB_NAME).a

obj-y += datapath/datapathps/

else

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)datapath/datapathps/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hrp/hrps/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogpbh/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogpd/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hogp/hogrh/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bas/bass/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)dis/diss/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)prf/*.c))

ifeq ($(BLE_AUDIO_ENABLED),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)csip/csisc/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)csip/csism/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)otp/otc/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)otp/ots/src/*.c))
endif

ifeq ($(BISTO_ENABLE),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)voicepath/$(VOICE_DATAPATH_TYPE)/*.c))
ccflags-y += \
	-Iservices/voicepath/$(VOICE_DATAPATH_TYPE)/ \
	-Iservices/voicepath/gsound/gsound_target/ \
	-Iservices/voicepath/gsound/gsound_custom/inc \
	-Iservices/voicepath/gsound/gsound_target_api_read_only
endif

ifeq ($(BMS),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bms/bmsp/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/bms/bmsp/api \
	-Iservices/ble_profiles_v2/bms/bmsp/src
endif

ifeq ($(BMSC),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bms/bmsc/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/bms/bmsc/api \
	-Iservices/ble_profiles_v2/bms/bmsc/src
endif

ifeq ($(ANCC), 1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)anc/ancc/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/anc/ancc/api \
	-Iservices/ble_profiles_v2/anc/ancc/src
endif

ifeq ($(ANCS), 1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)anc/ancs/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/anc/ancs/api \
	-Iservices/ble_profiles_v2/anc/ancs/src
endif

ifeq ($(AMS), 1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ams/amsp/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/ams/amsp/api \
	-Iservices/ble_profiles_v2/ams/amsp/src
endif

ifeq ($(AMSC),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ams/amsc/src/*.c))
ccflags-y += \
	-Iservices/ble_profiles_v2/ams/amsc/api \
	-Iservices/ble_profiles_v2/ams/amsc/src
endif

ifeq ($(TILE_DATAPATH_ENABLED),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)/tile/*.c))
endif

obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ota/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gfps/gfps_provider/src/*.c))
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)buds/*.c))

ifeq ($(TEST_OVER_THE_AIR),1)
obj_lib += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)tota/*.c))
subdir-ccflags-y +=-Iservices/tota
endif

ifeq ($(USE_BT_ADAPTER),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)btadapter/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)btadapter/client/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)btadapter/server/*.c))
endif


ifeq ($(GFPS_ENABLE),1)
obj_lib += gfps/
endif

obj-y += $(obj_c:.c=.o)

BLE_PROFILE_LIB_NAME := libble_profile

ifeq ($(USE_TRACE_ID), 1)
BLE_PROFILE_LIB_NAME := $(BLE_PROFILE_LIB_NAME)_log_compressed
endif

$(BLE_PROFILE_LIB_NAME)-y := $(obj_lib:.c=.o)
obj-y += $(BLE_PROFILE_LIB_NAME).a

subdir-ccflags-y += \
	-Iservices/audio_process \
	-Iservices/fs/fat \
	-Iservices/fs/sd \
	-Iservices/fs/fat/ChaN \
	$(BLE_APP_INCLUDES) \
	$(BLE_PROFILES_INCLUDES) \
    -Iservices/ble_stack_v2/ip/ahi/api/ \
    -Iservices/ble_stack_v2/ip/ble/hl/api/ \
    -Iservices/ble_stack_v2/ip/ble/hl/inc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/gapc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/gapm/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gatt/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/inc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/l2cap/ \
    -Iservices/ble_stack_v2/ip/hci/api/ \
    -Iservices/ble_stack_v2/ip/hci/src/ \
    -Iservices/ble_stack_v2/modules/common/api/ \
    -Iservices/ble_stack_v2/modules/ecc_p256/api/ \
    -Iservices/ble_stack_v2/modules/ke/api/ \
    -Iservices/ble_stack_v2/modules/rwip/api/ \
         $(BT_IF_INCLUDES) \
         $(BT_PROFILES_INCLUDES) \
        -Iservices/overlay \
        -Iservices/nvrecord \
        -Iservices/resources \
        -Iplatform/drivers/uarthci \
        -Iplatform/drivers/ana \
        -Iplatform/drivers/bt \
        -Iutils/cqueue \
        -Iutils/retention_ram \
        -Iservices/audioflinger \
        -Iutils/lockcqueue \
        -Iutils/intersyshci \
        -Iapps/key \
        -Iapps/main \
        -Iapps/common \
        -Iapps/audioplayers \
        -Iapps/factory \
        -Iservices/bridge/ \
        -Iservices/ai_voice/manager \
        -Iservices/app_ai/inc \
        -Ithirdparty/tile/tile_target/ \
        -Ithirdparty/tile/tile_common/tile_assert \
        -Ithirdparty/tile/tile_common/tile_features \
        -Ithirdparty/tile/tile_common/tile_gatt_db \
        -Ithirdparty/tile/tile_common/tile_player \
        -Ithirdparty/tile/tile_common/tile_service \
        -Ithirdparty/tile/tile_common/tile_storage \
        -Iservices/bt_app \
        -Iservices/bt_app/a2dp_codecs/include \
        -Iservices/ota/bes_ota/inc \
        -Iservices/ota \
        -Iservices/ble_profiles_v2/hogp            \
        -Iservices/ble_profiles_v2/hogp/hogpbh/api \
        -Iservices/ble_profiles_v2/hogp/hogpbh/src \
        -Iservices/ble_profiles_v2/hogp/hogpd/api \
        -Iservices/ble_profiles_v2/hogp/hogpd/src \
        -Iservices/ble_profiles_v2/hogp/hogprh/api \
        -Iservices/ble_profiles_v2/hogp/hogprh/src \
        -Iservices/ble_profiles_v2/bas/basc/api \
        -Iservices/ble_profiles_v2/bas/basc/src \
        -Iservices/ble_profiles_v2/bas/bass/api \
        -Iservices/ble_profiles_v2/bas/bass/src \
        -Iservices/ble_profiles_v2/dis/diss/api \
        -Iservices/ble_app_v2/app_main \
        -Iservices/ble_stack_v2/ip/ble/hl/src/gatt \
        -Iservices/ble_profiles_v2/dis/diss/src
ifeq ($(BES_OTA),1)
ifeq ($(IBRT_OTA),1)
ccflags-y += -Iservices/ibrt_ota/inc
endif
endif

ifeq ($(USE_BT_ADAPTER),1)
subdir-ccflags-y += -Iservices/ble_profiles_v2/btadapter
subdir-ccflags-y += -Iservices/ble_stack_v2/ip/gaf/api
endif

endif #DEV