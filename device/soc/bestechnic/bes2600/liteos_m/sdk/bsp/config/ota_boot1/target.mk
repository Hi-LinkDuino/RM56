CHIP		?= best2003
ifdef DEV
include $(srctree)/config/bfd_build/$(CHIP).mk
endif

DEBUG		?= 1

FPGA		?= 0

DEBUG_PORT	?= 1

FLASH_CHIP	?= ALL

NOSTD		?= 1

LARGE_RAM ?= 1

WATCHER_DOG ?= 0

export ALLOW_WARNING ?= 1

export TRACE_BAUD_RATE ?= 1500000

export TRACE_CRLF ?= 1

export POWER_MODE   ?= DIG_DCDC

export CALIB_SLOW_TIMER := 1

FLASH_SIZE ?= 0x1000000
export FLASH_SECURITY_REGISTER ?= 1
export FLASH_UNIQUE_ID ?= 1
OTA_CODE_OFFSET ?= 0

export REMAP_SUPPORT ?= 0
ifeq ($(REMAP_SUPPORT),1)
KBUILD_CFLAGS += -DREMAP_SUPPORT
endif

export BOOT2A_OFFSET ?= 0x10000
ifneq (BOOT2A_OFFSET,)
KBUILD_CFLAGS += -DOTA_BOOT2A_CODE_OFFSET=$(BOOT2A_OFFSET)
endif

export BOOT2B_OFFSET ?= 0x20000
ifneq (BOOT2B_OFFSET,)
KBUILD_CFLAGS += -DOTA_BOOT2B_CODE_OFFSET=$(BOOT2B_OFFSET)
endif

export BOOTINFO_OFFSET ?= 0x30000
ifneq (BOOTINFO_OFFSET,)
KBUILD_CFLAGS += -DOTA_BOOT_INFO_OFFSET=$(BOOTINFO_OFFSET)
endif

KBUILD_CPPFLAGS += -DBUILD_BOOT1=1

init-y		:=
core-y		:= tests/ota_boot_secboot/ platform/cmsis/ platform/hal/ services/transq_msg/ \
	platform/drivers/norflash/  platform/drivers/ana/

KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal -Iplatform/drivers/ana

LDS_FILE	:= best1000.lds

KBUILD_CPPFLAGS +=

KBUILD_CFLAGS +=

LIB_LDFLAGS += -lstdc++ -lsupc++

#CFLAGS_IMAGE += -u _printf_float -u _scanf_float

#LDFLAGS_IMAGE += --wrap main

