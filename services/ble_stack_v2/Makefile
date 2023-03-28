cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ahi/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/hl/src/gap/gapc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/hl/src/gap/gapm/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/hl/src/gatt/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/hl/src/l2cap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/hl/src/rwble_hl/*.c))
ifeq ($(BLE_AUDIO_ENABLED),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/iso/data_path/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/iso/data_path/isogen/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/ble/iso/data_path/isoohci/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/acc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/acc/mc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/acc/ot/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/acc/tb/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/al/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/arc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/arc/aic/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/arc/mic/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/arc/vc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/arc/voc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/atc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/atc/csi/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/atc/raa/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/bap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/bap/bc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/bap/capa/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/bap/codec/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/bap/uc/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/cap/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/gaf/src/iap/*.c))
endif
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)ip/hci/src/*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)modules/aes/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)modules/common/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)modules/ecc_p256/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)modules/ke/src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)modules/rwip/src/*.c))

BTBLE_LIB_NAME := $(CHIP)_libble_stack

ifeq ($(HFP_1_6_ENABLE), 1)
BTBLE_LIB_NAME := $(BTBLE_LIB_NAME)_sbc_enc
endif

ifeq ($(USE_TRACE_ID), 1)
BTBLE_LIB_NAME := $(BTBLE_LIB_NAME)_log_compressed
endif

$(BTBLE_LIB_NAME)-y := $(obj_c:.c=.o)
obj-y := $(BTBLE_LIB_NAME).a

subdir-ccflags-y += \
    -Iservices/bt_app \
    -Iservices/bridge/ \
    $(BLE_APP_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
	$(BLE_PROFILES_INCLUDES) \
    $(BT_IF_INCLUDES) \
    $(BT_PROFILES_INCLUDES)
