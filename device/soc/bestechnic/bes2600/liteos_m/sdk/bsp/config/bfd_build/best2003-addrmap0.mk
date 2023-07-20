#included by sdk.mk

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


BOOT1_SIZE = 0x10000
BOOT2_SIZE = 0x10000
# booinfo put in the end of flash
BOOTINFO_SIZE = 0x8000
TZ_SIZE = 0x26000
TZ_INFO_SIZE = 0x4000

export hex_calc=(echo "obase=16;ibase=16;$(subst 0x,,$(1))" | bc | sed 's/^/0x/')

export FW_CODE_OFFSET := $(shell $(call hex_calc,$(BOOT1_SIZE)+$(BOOT2_SIZE)+$(BOOT2_SIZE)+$(TZ_SIZE)+$(TZ_SIZE)+$(TZ_INFO_SIZE)))

KBUILD_CPPFLAGS += -DBOOT1_SIZE=$(BOOT1_SIZE) -DBOOT2_SIZE=$(BOOT2_SIZE) -DBOOTINFO_SIZE=$(BOOTINFO_SIZE)


ifeq ($(FLASH_TYPE),0)
FLASH_SIZE=0x1000000
FW_CODE_SIZE=0x600000
BOOTINFO_OFFSET=0xFD0000
else ifeq ($(FLASH_TYPE),1)
FLASH_SIZE=0x2000000
FW_CODE_SIZE=0x800000
BOOTINFO_OFFSET=0x1FD0000
endif

ifneq ($(findstring $(T),ota_boot1 ota_boot2a),)
REMAP_SUPPORT=1
ifeq ($(T),ota_boot2a)
RTOS_A_CODE_OFFSET := $(FW_CODE_OFFSET)
RTOS_B_CODE_OFFSET := $(shell $(call hex_calc,$(FW_CODE_OFFSET)+$(FW_CODE_SIZE)))
endif #ota_boot2a

else

export OTA_CODE_OFFSET:=$(FW_CODE_OFFSET)
export OTA_CODE_SIZE:=$(FW_CODE_SIZE)

#OTA类型，0-不支持，1-大小分区，2-AB分区，默认是大小分区，需要设置mini分区地址和大小
export BFD_OTA_TYPE ?= 2
ifeq ($(SECURE_BOOT),1)
export BFD_OTA_TYPE = 2
endif
KBUILD_CPPFLAGS += -DBFD_OTA_TYPE=$(BFD_OTA_TYPE)

ifeq ($(BFD_OTA_TYPE),1)
export FW_MINI_CODE_SIZE=0x300000
else
export FW_MINI_CODE_SIZE=0
endif
export FW_MINI_CODE_OFFSET := $(shell $(call hex_calc,$(FW_CODE_OFFSET)+$(FW_CODE_SIZE)))
KBUILD_CPPFLAGS += -DFW_MINI_CODE_SIZE=$(FW_MINI_CODE_SIZE) -DFW_MINI_CODE_OFFSET=$(FW_MINI_CODE_OFFSET)

# KBUILD_CPPFLAGS += -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET)
KBUILD_CPPFLAGS += -DOTA_CODE_SIZE=$(OTA_CODE_SIZE)


endif #T=cmcp/a7_dsp/bes2600w