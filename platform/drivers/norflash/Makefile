cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y := -I$(obj)/inc -Itests/programmer/inc -Iutils/boot_struct

CFLAGS_norflash_drv.o += -DCHIP_FLASH_CTRL_VER=$(CHIP_FLASH_CTRL_VER)

ifeq ($(SIMU),1)
FLASH_SAMP_DELAY_INDEX ?= 1
endif
ifneq ($(FLASH_SAMP_DELAY_INDEX),)
CFLAGS_norflash_drv.o += -DFLASH_SAMP_DELAY_INDEX=$(FLASH_SAMP_DELAY_INDEX)
endif

ifeq ($(FLASH_HPM),1)
ccflags-y += -DFLASH_HPM
endif

ifeq ($(FLASH_CALIB_DEBUG),1)
CFLAGS_norflash_drv.o += -DFLASH_CALIB_DEBUG
endif
ifeq ($(FLASH_SUSPEND),1)
CFLAGS_norflash_drv.o += -DFLASH_SUSPEND
endif
ifeq ($(FLASH_DTR),1)
CFLAGS_norflash_drv.o += -DFLASH_DTR
endif
ifeq ($(FLASH_SECURITY_REGISTER),1)
CFLAGS_norflash_drv.o += -DFLASH_SECURITY_REGISTER
endif
ifeq ($(FLASH_LOCK_CP_ACCESS),1)
CFLAGS_norflash_drv.o += -DFLASH_LOCK_CP_ACCESS
endif

ifeq ($(PUYA_FLASH_ERASE_PAGE_ENABLE),1)
CFLAGS_norflash_drv.o += -DPUYA_FLASH_ERASE_PAGE_ENABLE
endif

ifeq ($(FLASH_PAGE_MPM_ENABLE),1)
ccflags-y += -DFLASH_PAGE_MPM_ENABLE
endif

ifeq ($(PSRAM_ENABLE),1)
CFLAGS_norflash_drv.o += -DPSRAM_ENABLE
endif
ifeq ($(PSRAMUHS_ENABLE),1)
CFLAGS_norflash_drv.o += -DPSRAMUHS_ENABLE
endif

ifeq ($(FLASH_CHIP_LIST),)
ifeq ($(PROGRAMMER),1)
FLASH_CHIP_LIST := ALL
else
FLASH_CHIP_LIST := GD25Q32C
endif
endif
ifeq ($(FLASH_CHIP_LIST),)
$(error *** Error: FLASH_CHIP_LIST is NOT defined!)
endif

CFLAGS_norflash_drv.o += $(foreach flash,$(FLASH_CHIP_LIST),-D__NORFLASH_$(flash)__)


ifdef DEV

LIB_NAME := libnorflash

ifeq ($(DEV),1)
$(LIB_NAME)-y := $(obj-y)
endif
obj-y := $(LIB_NAME).a

endif  #DEV
