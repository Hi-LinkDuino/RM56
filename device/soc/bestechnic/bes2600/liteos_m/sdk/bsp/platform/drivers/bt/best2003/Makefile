cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

CFLAGS_bt_drv_uart_bridge_intsys.o += $(BT_DRV_UART_BRIDGE_FLAGS)

CFLAGS_bt_drv.o += $(BT_DRV_FLAGS)

CFLAGS_bt_drv_patch.o += $(BT_DRV_PATCH_FLAGS)

CFLAGS_bt_drv_config.o += $(BT_DRV_CONFIG_FLAGS)

RX_IQ_CAL ?= 0
ifeq ($(RX_IQ_CAL),1)
subdir-ccflags-y += -DRX_IQ_CAL
endif

TX_PULLING_CAL ?= 0
ifeq ($(TX_PULLING_CAL),1)
subdir-ccflags-y += -DTX_PULLING_CAL
endif

subdir-ccflags-y += -Iutils/crash_catcher/include
subdir-ccflags-y += -Iservices/nv_section/userdata_section
subdir-ccflags-y += -Iservices/nv_section/factory_section
subdir-ccflags-y += -Inet/wifi
ccflags-y += $(BT_IF_INCLUDES)

ifeq ($(IQ_CALI_TEST),1)
ccflags-y += -DIQ_CALI_TEST
endif

ifeq ($(FORCE_NOSIGNALINGMODE),1)
ccflags-y += -DFORCE_NOSIGNALINGMODE
endif

IBRT_DUAL_ANT_CTRL ?= 0
ifeq ($(IBRT_DUAL_ANT_CTRL),1)
ccflags-y += -DIBRT_DUAL_ANT_CTRL
endif

TX_PWR_LOWER_2DBM ?= 0
ifeq ($(TX_PWR_LOWER_2DBM),1)
ccflags-y += -DTX_PWR_LOWER_2DBM
endif

BT_BLE_USING_SAME_TX_LEVEL ?= 0
ifeq ($(BT_BLE_USING_SAME_TX_LEVEL),1)
ccflags-y += -DBT_BLE_USING_SAME_TX_LEVEL
endif

BLE_EXT_ADV_TX_PWR_INDEPEND ?= 0
ifeq ($(BLE_EXT_ADV_TX_PWR_INDEPEND),1)
ccflags-y += -DBLE_EXT_ADV_TX_PWR_INDEPEND
endif

RF_CALIB_ONLY_BT ?= 0
ifeq ($(RF_CALIB_ONLY_BT),1)
ccflags-y += -DRF_CALIB_ONLY_BT
endif