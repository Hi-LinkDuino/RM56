cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y += \
    -Iutils/hexdump \
    -Iapps/key \
    -Iservices/bt_app \
    $(BT_IF_INCLUDES) \
    $(BLE_STACK_INCLUDES) \
    -Iutils/crc32 \
    -Iplatform/drivers/ana \
    -Iservices/nv_section/customparam_section \
    -Iutils/heap \
    -Iutils/encrypt \
    -Iservices/norflash_api \
    -Iservices/nv_setion/userdata_section

CFLAGS_nvrecord.o += -DAUDIO_OUTPUT_VOLUME_DEFAULT=$(AUDIO_OUTPUT_VOLUME_DEFAULT)

ifeq ($(FLASH_SUSPEND),1)
CFLAGS_nvrecord.o += -DFLASH_SUSPEND
endif

ifneq ($(FACTORY_SECTOR_OFFSET),)
CFLAGS_nvrecord.o += -DFACTORY_SECTOR_OFFSET=$(FACTORY_SECTOR_OFFSET)
endif

ifdef DEV

LIB_NAME := libnvrecord

ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif
obj-y := $(LIB_NAME).a

endif  #DEV