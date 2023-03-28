
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)bridge_v2.c))

BTBRIDGE_LIB_NAME := $(CHIP)_libbridge

ifeq ($(HFP_1_6_ENABLE), 1)
BTBRIDGE_LIB_NAME := $(BTBRIDGE_LIB_NAME)_sbc_enc
endif

ifeq ($(USE_TRACE_ID), 1)
BTBRIDGE_LIB_NAME := $(BTBRIDGE_LIB_NAME)_log_compressed
endif

$(BTBRIDGE_LIB_NAME)-y := $(obj_c:.c=.o)

obj-y := $(BTBRIDGE_LIB_NAME).a

PROF_INC :=


subdir-ccflags-y += \
	-Iservices/bridge/ \
	-Iutils/intersyshci/ \
	-Iplatform/drivers/bt/ \
	-Iservices/bt_app/ \
	-Iservices/osif/ \
	$(BT_IF_INCLUDES) \
	$(BT_PROFILES_INCLUDES) \
	$(BLE_STACK_INCLUDES)


