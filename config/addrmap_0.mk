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
BOOTINFO_SIZE = 0x8000

export hex_calc = (echo "obase=16;ibase=16;$(subst 0x,,$(1))" | bc | sed 's/^/0x/')

export FW_CODE_OFFSET := $(shell $(call hex_calc,$(BOOT1_SIZE)+$(BOOT2_SIZE)+$(BOOT2_SIZE)+$(BOOTINFO_SIZE)))
# export FW_CODE_OFFSET := $(shell expr $(BOOT1_SIZE) + $(BOOT2_SIZE) + $(BOOT2_SIZE) + $(BOOTINFO_SIZE))
# export FW_CODE_OFFSET := $(shell awk 'BEGIN{printf("%\#x",'$BOOT1_SIZE'+'$BOOT2_SIZE'+'$BOOT2_SIZE'+'$BOOTINFO_SIZE')}')
# export FW_CODE_OFFSET := $(shell echo $$($(BOOT1_SIZE) + $(BOOT2_SIZE) + $(BOOT2_SIZE) + $(BOOTINFO_SIZE)))

KBUILD_CPPFLAGS += -DBOOT1_SIZE=$(BOOT1_SIZE) -DBOOT2_SIZE=$(BOOT2_SIZE) -DBOOTINFO_SIZE=$(BOOTINFO_SIZE)

ifeq ($(FLASH_TYPE),0)
PSRAM_XCCELA_MODE = 0
FLASH_SIZE=0x1000000
FW_CODE_SIZE=0x680000
else ifeq ($(FLASH_TYPE),1)
PSRAM_XCCELA_MODE = 1
FLASH_SIZE=0x2000000
FW_CODE_SIZE=0xC00000
else 
$(error Invalid FLASH_TYPE=$(FLASH_TYPE))
endif


ifneq ($(findstring $(T),ota_boot1 ota_boot2a),)
REMAP_SUPPORT=1
ifeq ($(T),ota_boot2a)
RTOS_A_CODE_OFFSET := $(FW_CODE_OFFSET)
RTOS_B_CODE_OFFSET := $(shell $(call hex_calc,$(FW_CODE_OFFSET)+$(FW_CODE_SIZE)))
endif #ota_boot2a

else

export OTA_CODE_OFFSET=$(FW_CODE_OFFSET)
export OTA_CODE_SIZE=$(FW_CODE_SIZE)

# KBUILD_CPPFLAGS += -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET)
KBUILD_CPPFLAGS += -DOTA_CODE_SIZE=$(OTA_CODE_SIZE)


endif #T=cmcp/a7_dsp/bes2600w