ifdef DEV

include $(srctree)/scripts/common.mk


obj_c = $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)client/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)server/*.c))

src_obj += $(obj_c:.c=.o)


ifeq ($(USE_BT_ADAPTER),1)
endif

ccflags-y += \
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


ifeq ($(USE_BT_ADAPTER),1)
ccflags-y += -Iservices/ble_profiles_v2/btadapter
ccflags-y += -Iservices/ble_stack_v2/ip/gaf/api
endif

obj-y := $(src_obj)

LIB_NAME := libbtadapter
ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif
obj-y := $(LIB_NAME).a


endif