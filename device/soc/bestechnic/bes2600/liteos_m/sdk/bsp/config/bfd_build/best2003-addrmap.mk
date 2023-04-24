#included in the end of common.mk included, since xxx_SECTION_SIZE defined in config/common.mk

##########################################################################
# BOOT1 # BOOT2 A # BOOT2 B # BOOTINFO # RTOS A # RTOS B # FS # SECTIONS #
##########################################################################
# SECTIONS as follows, size all defined in config/common.mk
#-------------------------------+
# COMBO_SECTION_SIZE            +  0/0x1000/COMBO_CUSBIN_IN_FLASH
#-------------------------------+
# PROMPT_SECTION_SIZE           +  0/0x4B000/PROMPT_IN_FLASH
#-------------------------------+
# HOTWORD_SECTION_SIZE          +  0/0x3C000/GSOUND_HOTWORD_ENABLE
#-------------------------------+
# CORE_DUMP_SECTION_SIZE        +  0/0x100000/CORE_DUMP_TO_FLASH
#-------------------------------+
# OTA_UPGRADE_LOG_SIZE          +  0/0x1000/OTA_UPGRADE_LOG_SIZE
#-------------------------------+
# LOG_DUMP_SECTION_SIZE         +  0/0x28000/DUMP_LOG_ENABLE
#-------------------------------+
# CRASH_DUMP_SECTION_SIZE       +  0/0x4000/DUMP_CRASH_LOG
#-------------------------------+
# CUSTOM_PARAMETER_SECTION_SIZE +  0x1000
#-------------------------------+
# LHDC_LICENSE_SECTION_SIZE     +  0/0x1000/A2DP_LHDC_ON
#-------------------------------+
# USERDATA_SECTION_SIZE         +  0x1000
#-------------------------------+
# USERDATA_SECTION_SIZE         +  0x1000
#-------------------------------+
# AUD_SECTION_SIZE              +  0/0x8000/AUDIO_SECTION_ENABLE/ANC_APP
#-------------------------------+
# RESERVED_SECTION_SIZE         +  0x1000
#-------------------------------+
# FACTORY_SECTION_SIZE          +  0x1000
#-------------------------------+


export FACTORY_SECTION_OFFSET = $(shell $(call hex_calc,$(FLASH_SIZE)-$(FACTORY_SECTION_SIZE)))
export RESERVED_SECTION_OFFSET = $(shell $(call hex_calc,$(FACTORY_SECTION_OFFSET)-$(RESERVED_SECTION_SIZE)))
export AUD_SECTION_OFFSET = $(shell $(call hex_calc,$(RESERVED_SECTION_OFFSET)-$(AUD_SECTION_SIZE)))
export USERDATA_SECTION_OFFSET = $(shell $(call hex_calc,$(AUD_SECTION_OFFSET)-$(USERDATA_SECTION_SIZE)-$(USERDATA_SECTION_SIZE)))
export LHDC_LICENSE_SECTION_OFFSET = $(shell $(call hex_calc,$(USERDATA_SECTION_OFFSET)-$(LHDC_LICENSE_SECTION_SIZE)))
export CUSTOM_PARAMETER_SECTION_OFFSET = $(shell $(call hex_calc,$(LHDC_LICENSE_SECTION_OFFSET)-$(CUSTOM_PARAMETER_SECTION_SIZE)))
export CRASH_DUMP_SECTION_OFFSET = $(shell $(call hex_calc,$(CUSTOM_PARAMETER_SECTION_OFFSET)-$(CRASH_DUMP_SECTION_SIZE)))
export LOG_DUMP_SECTION_OFFSET = $(shell $(call hex_calc,$(CRASH_DUMP_SECTION_OFFSET)-$(LOG_DUMP_SECTION_SIZE)))
export OTA_UPGRADE_LOG_OFFSET = $(shell $(call hex_calc,$(LOG_DUMP_SECTION_OFFSET)-$(OTA_UPGRADE_LOG_SIZE)))
export CORE_DUMP_SECTION_OFFSET = $(shell $(call hex_calc,$(OTA_UPGRADE_LOG_OFFSET)-$(CORE_DUMP_SECTION_SIZE)))
export HOTWORD_SECTION_OFFSET = $(shell $(call hex_calc,$(CORE_DUMP_SECTION_OFFSET)-$(HOTWORD_SECTION_SIZE)))
export PROMPT_SECTION_OFFSET = $(shell $(call hex_calc,$(HOTWORD_SECTION_OFFSET)-$(PROMPT_SECTION_SIZE)))
export COMBO_SECTION_OFFSET = $(shell $(call hex_calc,$(PROMPT_SECTION_OFFSET)-$(COMBO_SECTION_SIZE)))


# FLASH_SECTION_START=$(COMBO_SECTION_OFFSET)
FLASH_SECTION_START=$(BOOTINFO_OFFSET)

KBUILD_CPPFLAGS += \
	-DBOOTINFO_SIZE=$(BOOTINFO_SIZE) \
	-DBOOTINFO_OFFSET=$(BOOTINFO_OFFSET) 
LDS_SECTION_FLAGS += \
	-DFLASH_SECTION_START=$(FLASH_SECTION_START) \
	-DBOOTINFO_SIZE=$(BOOTINFO_SIZE) \
	-DBOOTINFO_OFFSET=$(BOOTINFO_OFFSET) 	

KBUILD_CPPFLAGS += \
	-DCOMBO_SECTION_OFFSET=$(COMBO_SECTION_OFFSET) \
	-DPROMPT_SECTION_OFFSET=$(PROMPT_SECTION_OFFSET) \
	-DHOTWORD_SECTION_OFFSET=$(HOTWORD_SECTION_OFFSET) \
	-DCORE_DUMP_SECTION_OFFSET=$(CORE_DUMP_SECTION_OFFSET) \
	-DOTA_UPGRADE_LOG_OFFSET=$(OTA_UPGRADE_LOG_OFFSET) \
	-DLOG_DUMP_SECTION_OFFSET=$(LOG_DUMP_SECTION_OFFSET) \
	-DCRASH_DUMP_SECTION_OFFSET=$(CRASH_DUMP_SECTION_OFFSET) \
	-DCUSTOM_PARAMETER_SECTION_OFFSET=$(CUSTOM_PARAMETER_SECTION_OFFSET) \
	-DLHDC_LICENSE_SECTION_OFFSET=$(LHDC_LICENSE_SECTION_OFFSET) \
	-DUSERDATA_SECTION_OFFSET=$(USERDATA_SECTION_OFFSET) \
	-DAUD_SECTION_OFFSET=$(AUD_SECTION_OFFSET) \
	-DRESERVED_SECTION_OFFSET=$(RESERVED_SECTION_OFFSET) \
	-DFACTORY_SECTION_OFFSET=$(FACTORY_SECTION_OFFSET)

KBUILD_CPPFLAGS += \
	-DCOMBO_SECTION_SIZE=$(COMBO_SECTION_SIZE) \
	-DPROMPT_SECTION_SIZE=$(PROMPT_SECTION_SIZE) \
	-DHOTWORD_SECTION_SIZE=$(HOTWORD_SECTION_SIZE) \
	-DCORE_DUMP_SECTION_SIZE=$(CORE_DUMP_SECTION_SIZE) \
	-DOTA_UPGRADE_LOG_SIZE=$(OTA_UPGRADE_LOG_SIZE) \
	-DLOG_DUMP_SECTION_SIZE=$(LOG_DUMP_SECTION_SIZE) \
	-DCRASH_DUMP_SECTION_SIZE=$(CRASH_DUMP_SECTION_SIZE) \
	-DCUSTOM_PARAMETER_SECTION_SIZE=$(CUSTOM_PARAMETER_SECTION_SIZE) \
	-DLHDC_LICENSE_SECTION_SIZE=$(LHDC_LICENSE_SECTION_SIZE) \
	-DUSERDATA_SECTION_SIZE=$(USERDATA_SECTION_SIZE) \
	-DAUD_SECTION_SIZE=$(AUD_SECTION_SIZE) \
	-DRESERVED_SECTION_SIZE=$(RESERVED_SECTION_SIZE) \
	-DFACTORY_SECTION_SIZE=$(FACTORY_SECTION_SIZE) \

LDS_SECTION_FLAGS += \
	-DCOMBO_SECTION_OFFSET=$(COMBO_SECTION_OFFSET) \
	-DPROMPT_SECTION_OFFSET=$(PROMPT_SECTION_OFFSET) \
	-DHOTWORD_SECTION_OFFSET=$(HOTWORD_SECTION_OFFSET) \
	-DCORE_DUMP_SECTION_OFFSET=$(CORE_DUMP_SECTION_OFFSET) \
	-DOTA_UPGRADE_LOG_OFFSET=$(OTA_UPGRADE_LOG_OFFSET) \
	-DLOG_DUMP_SECTION_OFFSET=$(LOG_DUMP_SECTION_OFFSET) \
	-DCRASH_DUMP_SECTION_OFFSET=$(CRASH_DUMP_SECTION_OFFSET) \
	-DCUSTOM_PARAMETER_SECTION_OFFSET=$(CUSTOM_PARAMETER_SECTION_OFFSET) \
	-DLHDC_LICENSE_SECTION_OFFSET=$(LHDC_LICENSE_SECTION_OFFSET) \
	-DUSERDATA_SECTION_OFFSET=$(USERDATA_SECTION_OFFSET) \
	-DAUD_SECTION_OFFSET=$(AUD_SECTION_OFFSET) \
	-DRESERVED_SECTION_OFFSET=$(RESERVED_SECTION_OFFSET) \
	-DFACTORY_SECTION_OFFSET=$(FACTORY_SECTION_OFFSET)


ifeq ($(T),best2600w_liteos)

#######################################
#little fs
#######################################
A7_LITTLEFS_TEST=0
export LITTLEFS_ENABLE=1
export LFS_ON_FLASH=1

ifeq ($(LITTLEFS_ENABLE)_$(LFS_ON_FLASH),1_1)
# ALWAYS enabled, NEVER changed
export LITTLEFS_PARTITION_SUPPORT=1

#offset base on FLASH_NC_BASE
ifeq ($(BFD_OTA_TYPE),0)
export LFS_FLASH_BASE_ADDR = $(shell $(call hex_calc,$(FW_CODE_OFFSET)+$(FW_CODE_SIZE)))
else ifeq ($(BFD_OTA_TYPE),1)
export LFS_FLASH_BASE_ADDR = $(shell $(call hex_calc,$(FW_MINI_CODE_OFFSET)+$(FW_MINI_CODE_SIZE)))
else 
export LFS_FLASH_BASE_ADDR = $(shell $(call hex_calc,$(FW_CODE_OFFSET)+$(FW_CODE_SIZE)+$(FW_CODE_SIZE)))
endif

export LFS_FLASH_TOTAL_SIZE = $(shell $(call hex_calc,$(FLASH_SECTION_START)-$(LFS_FLASH_BASE_ADDR)))

export LFS_SIZE ?= 0
ifeq ($(LFS_SIZE),0)
export override LFS_SIZE = $(LFS_FLASH_TOTAL_SIZE)
$(info LFS_SIZE=$(LFS_SIZE))
endif

KBUILD_CPPFLAGS += -DLITTLEFS_PARTITION_SUPPORT
KBUILD_CPPFLAGS += -DLFS_FLASH_BASE_ADDR=$(LFS_FLASH_BASE_ADDR)
KBUILD_CPPFLAGS += -DLFS_FLASH_TOTAL_SIZE=$(LFS_FLASH_TOTAL_SIZE)
KBUILD_CPPFLAGS += -DLFS_SIZE=$(LFS_SIZE)

endif

endif #T=cmcp/a7_dsp/bes2600w