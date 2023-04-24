export DEV ?= 1#
export SDK ?= 1

export BEST2003_TYPE ?= AX4D
ifeq ($(BEST2003_TYPE),AX4D)
export FLASH_TYPE=0

PSRAM_XCCELA_MODE=0
PSRAMUHS_AP325608A=0
PSRAMULS_MODE=0
RF_TX_CONTROL_IO=16

else ifeq ($(BEST2003_TYPE),AX4F)
export FLASH_TYPE=0

ALT_BOOT_FLASH=1
PSRAMUHS_AP325608A=1
RF_TX_CONTROL_IO=100

else ifeq ($(BEST2003_TYPE),GX6D)
export FLASH_TYPE=1
PSRAM_XCCELA_MODE=1
RF_TX_CONTROL_IO=100

else
$(error Invalid BEST2003_TYPE=$(BEST2003_TYPE))
endif


# export FLASH_TYPE ?= 0
KBUILD_CPPFLAGS += -DFLASH_TYPE=$(FLASH_TYPE)

export SECURE_BOOT ?= 0

include $(srctree)/config/bfd_build/$(CHIP)-addrmap0.mk

ifeq ($(findstring $(T),ota_boot1 ota_boot2a),)

ifeq ($(FLASH_TYPE),0)
export CP_BIN_NAME=cmcp_16
else
export CP_BIN_NAME=cmcp_32
endif

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

export LIB_BIN_IN_SRC_DIR=0
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


