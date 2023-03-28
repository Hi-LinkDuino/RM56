export DEV ?= 1#
export SDK ?= 1

#0-16M norflash + 8M psram
#1-32M norflash + 32M psram
#2-32M norflash + 8M psram
export FLASH_TYPE ?= 1
KBUILD_CPPFLAGS += -DFLASH_TYPE=$(FLASH_TYPE)

include $(srctree)/config/addrmap_0.mk

#u07 added
export FORCE_TO_USE_TARGET_LIB=1


export SECURE_BOOT ?= 0

ifeq ($(findstring $(T),ota_boot1 ota_boot2a),)


export CP_BIN_NAME=cmcp_${FLASH_SIZE}

# ram fs for A7, LITTLEFS_ENABLE and LFS_ON_RAM MUST be enabled
export FS_BIN_NAME=ramfs
export MCU_SLAVE_FS_EN=0
ifeq ($(MCU_SLAVE_FS_EN),1)
export PSRAMFS_SIZE=0x100000 #tail of psram
else
export PSRAMFS_SIZE=0 #tail of psram
endif
# used for cp&mcu
export CP_XIP = 0

export LIB_BIN_IN_SRC_DIR=1
# LIB_BIN_IN_SRC_DIR=1: archive-bin-path := $(srctree)/$(src)/lib
# LIB_BIN_IN_TOP_WITH_SRC_DIR=1: bin-path := $(srctree)/lib/$(src)
# otherwise, 
# BES_LIB_DIR ?= lib/bes
# LIB_BIN_DIR ?= $(BES_LIB_DIR)
# archive-bin-path := $(srctree)/$(LIB_BIN_DIR)

export CHIP_HAS_EXT_NORFLASH := 0
export TEST_FOR_EXT_NORFLASH := 1

ifeq ($(CHIP_HAS_EXT_NORFLASH), 1)
KBUILD_CPPFLAGS += -DCHIP_HAS_EXT_NORFLASH

ifeq ($(TEST_FOR_EXT_NORFLASH), 1)
KBUILD_CPPFLAGS += -DTEST_FOR_EXT_NORFLASH
endif

endif #CHIP_HAS_EXT_NORFLASH


endif #T=cmcp/a7_dsp/bes2600w


ifeq ($(SECURE_BOOT), 1)
ifeq ($(T),ota_boot1)
KBUILD_CPPFLAGS += -DMCU_STAGE=1
endif
ifeq ($(T),ota_boot2a)
KBUILD_CPPFLAGS += -DMCU_STAGE=2 -DBOOT_LOADER_POST_INIT_HOOK
endif
ifeq ($(T),best2600w_aiot)
KBUILD_CPPFLAGS += -DMCU_STAGE=3
endif
endif
