ifdef DEV
include $(srctree)/config/sdk-a7.mk
endif

CHIP		?= best2003

DEBUG		?= 1

NOSTD		?= 0

SIMU		?= 1

ROM_BUILD	?= 1

FLASH_CHIP	?= ALL

PSRAM_ENABLE ?= 1


KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

export NOSYS ?= 0
export FORCE_TO_USE_TARGET_LIB ?= 0

ifneq ($(MIC_SAMPLERATE), )
KBUILD_CFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
endif

ifneq ($(MIC_PERIOD_MS), )
KBUILD_CFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
KBUILD_CPPFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
endif

ifneq ($(CFG_CAPTURE_CHANNEL_NUM), )
KBUILD_CFLAGS += -DCHAN_NUM_CAPTURE=$(CFG_CAPTURE_CHANNEL_NUM)
endif

ifneq ($(CFG_CAPTURE_CHANNEL_MAP), )
KBUILD_CFLAGS += -DCFG_CAPTURE_CHANNEL_MAP=$(CFG_CAPTURE_CHANNEL_MAP)
endif

export PCM_AUDIO_INSTANCE_ENABLE ?= 0
ifeq ($(PCM_AUDIO_INSTANCE_ENABLE),1)
KBUILD_CFLAGS += -DPCM_AUDIO_INSTANCE_ENABLE
endif

export A7_PCMAUDIO_TEST_CASE ?= 0
ifeq ($(A7_PCMAUDIO_TEST_CASE),1)
KBUILD_CFLAGS += -DA7_PCMAUDIO_TEST_CASE
endif

export A7_CMSIS_TEST_CASE ?= 0
ifeq ($(A7_CMSIS_TEST_CASE),1)
KBUILD_CFLAGS += -DA7_CMSIS_TEST_CASE
endif	# A7_CMSIS_TEST_CASE

export A7_CPP_TEST ?= 0
ifeq ($(A7_CPP_TEST),1)
KBUILD_CFLAGS += -DA7_CPP_TEST
endif

export SPEECH_MSG_ENABLE ?= 0
ifeq ($(SPEECH_MSG_ENABLE),1)
KBUILD_CFLAGS += -DSPEECH_MSG_ENABLE
ifeq ($(SPEECH_MSG_TEST_CASE),1)
KBUILD_CFLAGS += -DSPEECH_MSG_TEST_CASE
ifeq ($(SPEECH_MSG_TEST_RX),1)
KBUILD_CFLAGS += -DSPEECH_MSG_TEST_RX
endif
ifeq ($(SPEECH_MSG_TEST_TX),1)
KBUILD_CFLAGS += -DSPEECH_MSG_TEST_TX
endif
ifeq ($(SPEECH_MSG_TEST_TXRX_LOOP),1)
KBUILD_CFLAGS += -DSPEECH_MSG_TEST_TXRX_LOOP
endif
endif	# SPEECH_MSG_TEST_CASE
endif   # SPEECH_MSG_ENABLE

export AISPEECH_FFC_ALG_ENABLE ?= 0
ifeq ($(AISPEECH_FFC_ALG_ENABLE),1)
KBUILD_CPPFLAGS += -DAISPEECH_FFC_ALG_ENABLE
endif

#file system
export LITTLEFS_ENABLE ?= 0
export LFS_ON_RAM ?= 0
ifeq ($(LFS_ON_RAM),1)
KBUILD_CPPFLAGS += -DLFS_ON_RAM=1
endif
export LITTLEFS_PARTITION_SUPPORT ?= 0
ifeq ($(LITTLEFS_PARTITION_SUPPORT),1)
LDFLAGS_IMAGE += --wrap fseek --wrap fflush --wrap fgetc
endif

ifeq ($(KERNEL), NUTTX)
export NO_LIBC := 1
export NUTTX_BUILD := 1
export ALLOW_WARNING := 1
KBUILD_CPPFLAGS += -DCHIP_A7_DSP
include $(NUTTX_CFG_FILE)

ifeq ($(CONFIG_BES_LPUART0),y)
DEBUG_PORT	:= 1
export TRACE_BAUD_RATE := $(CONFIG_LPUART0_BAUD)
endif
ifeq ($(CONFIG_BES_LPUART1),y)
DEBUG_PORT	:= 2
export TRACE_BAUD_RATE := $(CONFIG_LPUART1_BAUD)
endif
ifeq ($(CONFIG_BES_LPUART2),y)
DEBUG_PORT	:= 3
export TRACE_BAUD_RATE := $(CONFIG_LPUART2_BAUD)
endif
export DSP_USE_GPDMA := 0
else

export A7_UART ?= 2
#default transq, support 1(uart0), 3(uart2)
ifeq ($(A7_UART),1)
DEBUG_PORT ?= 1
DSP_USE_GPDMA ?= 1
else ifeq ($(A7_UART),2)
DEBUG_PORT ?= 2
DSP_USE_GPDMA ?= 1
KBUILD_CPPFLAGS += -DUART_PRINTF_ID=1
else ifeq ($(A7_UART),3)
DEBUG_PORT ?= 3
DSP_USE_GPDMA ?= 1
else
DEBUG_PORT	?= 4
DSP_USE_GPDMA ?= 1
endif
KBUILD_CPPFLAGS += -DA7_UART=$(A7_UART)
endif

export A7_SRAM ?= 2
# a7 sram mode:
# 0: none, sync flags, stack/ttb use PSRAM, no sram is used
# 1: solo, sync flags, stack/ttb use PSRAM, reserve 512K sram heap
# 2: default, sync flags, stack/ttb share 512KB sram
ifeq ($(A7_SRAM),0)
export DSP_RAM_SIZE ?= 0
else ifeq ($(A7_SRAM),1)
export DSP_RAM_SIZE ?= 0
KBUILD_CPPFLAGS += -DDSP_SRAM_SOLO
else
export DSP_RAM_SIZE ?= 0x80000
endif

ifeq ($(CONFIG_AISPEECH_ALG),y)
export A7_PROJECT ?= aispeech
else ifeq ($(CONFIG_SOUNDPLUS_ALG),y)
export A7_PROJECT ?= soundplus
else
export A7_PROJECT ?= dummy
endif

ifeq ($(A7_PROJECT),soundplus)
export DSP_LIB := 1
export DSP_LIB_ALL_ENABLE := 1
endif

ifeq ($(A7_PROJECT),kws)
export DSP_LIB ?= 1
KBUILD_CFLAGS += -DAQE_KWS_HAAS
endif

export A7_DSP_ENABLE ?= 1
ifeq ($(A7_DSP_ENABLE),1)
KBUILD_CFLAGS += -D__A7_DSP_ENABLE__
endif

export TRACE_BAUD_RATE ?= 1500000

#KERNEL      ?= RHINO
ifeq ($(KERNEL),RHINO)
export RHINO_OSAL_POSIX ?= 1
export NO_STARTFILES ?= 1
endif

ifeq ($(NO_STARTFILES),1)
KBUILD_CPPFLAGS += -D__NO_STARTFILES__
endif

ifneq ($(KERNEL),)
RTOS		?= 1
MBED		?= 0
else
RTOS		?= 0
endif

export CHIP_SUBSYS ?= dsp

export EXEC_IN_RAM ?= 1

export NO_CLOCK_INIT ?= 1

export NO_PMU ?= 1

export NO_SPI ?= 1

export ALLOW_WARNING ?= 0

export DSP_RAM_SIZE ?= 0x80000

export PSRAMUHS_ENABLE ?= 1

export TRACE_BUF_SIZE := 8192

export MBW_TEST ?= 0

export DSP_AUDIO_TASK ?= 1

export TRACE_CRLF ?= 1

export A7_ALI_LIB ?= 0
ifeq ($(A7_ALI_LIB), 1)
KBUILD_CPPFLAGS += -DA7_ALI_LIB
endif

export DIGMIC_EN ?= 0
ifeq ($(DIGMIC_EN),1)
KBUILD_CFLAGS += -D__DIGMIC_USED__
endif

init-y		:=
core-y		:= platform/cmsis/ platform/hal/ services/a7_dsp/ services/transq_msg/ utils/hwtimer_list/ utils/kfifo/

core-y		+= services/a7_dsp/prj/$(A7_PROJECT)/

ifeq ($(DSP_TFLM_ENABLE),1)
core-y += thirdparty/
endif

ifeq ($(KERNEL), NUTTX)
core-y		+= utils/bes_kv/
ifeq ($(CONFIG_LCD),y)
export CHIP_HAS_LCDC := 1
#export CHIP_HAS_DMA2D := 1
core-y      += platform/drivers/graphic/
core-y      += platform/drivers/ana/
endif
else
ifeq ($(RTOS), 1)
core-y		+= services/sys_time/ utils/bes_kv/
endif
endif

export A7_DISPLAY ?= 0
ifeq ($(A7_DISPLAY), 1)
export DISPLAY_V2 ?= 1
ifeq ($(DISPLAY_V2),1)
export CHIP_HAS_LCDC ?= 1
core-y += platform/drivers/graphic/ platform/drivers/ana/
KBUILD_CPPFLAGS += -DA7_DISPLAY -DDISPLAY_V2 -DBOARD_NAME=a7_dsp
endif
export A7_LVGL_TEST ?= 0
ifeq ($(A7_LVGL_TEST), 1)
core-y += services/wifi_app/lvgl/
KBUILD_CPPFLAGS += -DA7_LVGL_TEST -Iservices/wifi_app/lvgl/
endif

ifeq ($(CONFIG_DISPLAY_RM69330), 1)
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_RM69330
else ifeq ($(CONFIG_DISPLAY_ICNA3311), 1)
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_ICNA3311
else
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_A064 -DI2C1_IOMUX_INDEX=06
endif
endif

ifeq ($(MBW_TEST), 1)
core-y += tests/mbw_test/
KBUILD_CPPFLAGS += -DMBW_TEST_NOOS -DMBW_ARRAY_SIZE_KB=256 -DMBW_BLOCK_SIZE_KB=16
endif

ifeq ($(CONFIG_PSRAM_ENABLE),y)
export PSRAM_ENABLE := 1
export PSRAM_SIZE := $(CONFIG_PSRAM_SIZE)
export PSRAM_BASE := $(CONFIG_PSRAM_BASE)
KBUILD_CPPFLAGS += -DPSRAM_BASE=$(PSRAM_BASE)
ifeq ($(CONFIG_DSP_USE_PSRAM),y)
export DSP_USE_PSRAM := 1
endif
endif

ifeq ($(DSP_USE_PSRAM),1)
KBUILD_CPPFLAGS += -DDSP_USE_PSRAM
endif

ifeq ($(PSRAM_ENABLE),1)
export PSRAM_XCCELA_MODE ?= 0
ifeq ($(PSRAM_XCCELA_MODE),1)
export PSRAM_SPEED := 200
PSRAM_TOTAL_SIZE ?= 0x2000000
else
PSRAM_TOTAL_SIZE ?= 0x800000
endif
export PSRAMCP_SIZE ?= 0
export PSRAMFS_SIZE ?= 0
export PSRAM_SIZE ?= $(PSRAM_TOTAL_SIZE)
MCU_PSRAM_SIZE ?= $(PSRAM_TOTAL_SIZE)-$(PSRAMCP_SIZE)-$(PSRAMFS_SIZE)
KBUILD_CPPFLAGS += -DMCU_PSRAM_SIZE=$(MCU_PSRAM_SIZE)
KBUILD_CPPFLAGS += -DPSRAMCP_SIZE=$(PSRAMCP_SIZE)
KBUILD_CPPFLAGS += -DPSRAMFS_SIZE=$(PSRAMFS_SIZE)
else
export PSRAM_SIZE ?= 0
endif

ifeq ($(PSRAMUHS_ENABLE), 1)
export EXEC_IN_RAM := 0
ifeq ($(PSRAM_ENABLE), 1)
export PSRAMUHS_SIZE ?= 0x800000
CONFIG_MCU_PSRAMUHS_SIZE ?= 0
export MCU_PSRAMUHS_SIZE ?= $(CONFIG_MCU_PSRAMUHS_SIZE)
export DSP_PSRAMUHS_SIZE ?= $(PSRAMUHS_SIZE)-$(MCU_PSRAMUHS_SIZE)
else
export PSRAMUHS_SIZE ?= 0x1000000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x800000
endif
KBUILD_CPPFLAGS += -DCHIP_HAS_PSRAMUHS -DPSRAMUHS_ENABLE -DMCU_PSRAMUHS_SIZE=$(MCU_PSRAMUHS_SIZE) -DDSP_PSRAMUHS_SIZE=$(DSP_PSRAMUHS_SIZE)
endif

KBUILD_CPPFLAGS += -Iplatform/cmsis/inc -Iplatform/hal -Iplatform/drivers/usb/usb_dev/inc

KBUILD_CPPFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-format -Wno-error=date-time

ifeq ($(EXEC_IN_RAM), 1)
KBUILD_CPPFLAGS += -DEXEC_IN_RAM
endif

ifeq ($(KERNEL), RHINO)
KBUILD_CPPFLAGS += -DAOS_COMP_CPLUSPLUS
ifeq ($(RHINO_OSAL_POSIX), 1)
KBUILD_CPPFLAGS += -DAOS_COMP_OSAL_POSIX
endif
endif

ifeq ($(KERNEL), RTT)
export NO_STARTFILES ?= 1
KBUILD_CPPFLAGS += -DRTT_SUPPORT
endif

ifeq ($(KERNEL), LITEOS_A)
export OS_HEAP_SIZE ?= 0x400000
KBUILD_CPPFLAGS += -DOS_HEAP_SIZE=$(OS_HEAP_SIZE)
KBUILD_CFLAGS += -fno-omit-frame-pointer
export NO_LIBC ?= 0
export OPT_LIBC ?= 0
export MODULE_KERNEL_STUB ?= 0
ifeq ($(MODULE_KERNEL_STUB),1)
KBUILD_CPPFLAGS += -DMODULE_KERNEL_STUB=1
endif
endif

export HW_AUD_LOOPBACK_MODE ?= 0
ifeq ($(HW_AUD_LOOPBACK_MODE),1)
KBUILD_CFLAGS += -DHW_AUD_LOOPBACK_MODE
endif

export HISTEN_PROCESS_EN ?= 0
ifeq ($(HISTEN_PROCESS_EN), 1)
KBUILD_CFLAGS += -DHISTEN_PROCESS_EN
endif

LIB_LDFLAGS += -lstdc++ -lsupc++

CFLAGS_IMAGE +=

LDFLAGS_IMAGE +=

C_ONLY_FLAGS    := -std=gnu11 -Wno-incompatible-pointer-types
C++_ONLY_FLAGS  := -std=gnu++11

ifeq ($(NOSYS),0)
CFLAGS_IMAGE += -u _printf_float -u _scanf_float
endif

#LDFLAGS_IMAGE += --wrap malloc --wrap calloc --wrap free --wrap realloc

export FORCE_TO_USE_TARGET_LIB ?= 0
