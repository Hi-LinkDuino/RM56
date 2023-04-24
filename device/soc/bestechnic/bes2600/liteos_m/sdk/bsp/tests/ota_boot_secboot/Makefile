cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

obj-y += ../../utils/crc32/ ../../utils/hwtimer_list/ ../../utils/hexdump/ ../../utils/secure_boot/mincrypt/

obj-y += main_hmos.c
obj-y += ota_flash.c

ifneq ($(OTA_BARE_BOOT),1)
obj-y += ../../platform/main/startup_main.S
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
	-Iutils/boot_struct \

ifeq ($(SECURE_BOOT),1)
ccflags-y += -DSECURE_BOOT
endif

ccflags-y += -Iutils/crc32 -Iutils/hwtimer_list -Iutils/hexdump -Iutils/boot_struct -Iutils/secure_boot/mincrypt -Iplatform/drivers/norflash

ccflags-y += -DOTA_BOOT_SIZE=$(OTA_BOOT_SIZE) -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET) -DOTA_REMAP_OFFSET=$(OTA_REMAP_OFFSET)

ifeq ($(OTA_BARE_BOOT),1)
ccflags-y += -DOTA_BARE_BOOT
endif

ifneq ($(FLASH_SIZE),)
ccflags-y += -DFLASH_SIZE=$(FLASH_SIZE)
endif

ifneq ($(DEBUG_PORT),)
ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif

