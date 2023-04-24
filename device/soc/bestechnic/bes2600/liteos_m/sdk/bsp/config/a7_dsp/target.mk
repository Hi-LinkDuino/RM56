CHIP		?= best2003
ifdef DEV
include $(srctree)/config/bfd_build/${CHIP}-a7.mk
endif

DEBUG		?= 1

NOSTD		?= 0

SIMU		?= 1

ROM_BUILD	?= 1

FLASH_CHIP	?= ALL

PSRAM_ENABLE ?= 1

ifeq ($(MSPEECH_APP_ENABLE), 1)
KBUILD_CPPFLAGS += -DMSPEECH_APP_ENABLE
endif

#export A7_CPU_BUSY_PER ?= 0
ifneq ($(A7_CPU_BUSY_PER),)
KBUILD_CPPFLAGS += -DA7_CPU_BUSY_PER=$(A7_CPU_BUSY_PER)
endif

export NOSYS ?= 0
export FORCE_TO_USE_TARGET_LIB ?= 0

ifneq ($(MIC_SAMPLERATE), )
KBUILD_CFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
else
KBUILD_CFLAGS += -DMIC_SAMPLERATE=48000
KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=48000
endif

ifneq ($(MIC_PERIOD_MS), )
KBUILD_CFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
KBUILD_CPPFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
else
KBUILD_CFLAGS += -DMIC_PERIOD_MS=48
KBUILD_CPPFLAGS += -DMIC_PERIOD_MS=48
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

export COREMARK_TEST ?= 0
ifeq ($(COREMARK_TEST),1)
KBUILD_CFLAGS += -DCOREMARK_TEST
endif

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

export DSP_AUDIO_PROCESS_ENABLE ?= 0

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

export MMC_DISK_FATFS_IN_A7 ?= 0
ifeq ($(MMC_DISK_FATFS_IN_A7),1)
export PSRAMUHS_NC_SIZE ?= 0x80000
else
export PSRAMUHS_NC_SIZE ?= 0
endif

export SRAM_NC_SIZE ?= 0

ifeq ($(KERNEL), NUTTX)
export NO_LIBC := 1
export NUTTX_BUILD := 1
export ALLOW_WARNING := 0
KBUILD_CPPFLAGS += -DCHIP_A7_DSP
export A7_DISPLAY := 0
export NO_CONFIG := 1
export DSP_USE_GPDMA := 1
export CALIB_SLOW_TIMER := 1

include $(NUTTX_CFG_FILE)

ifeq ($(CONFIG_BES_MTDSDMMC),y)
export CHIP_HAS_SDMMC := 1
export SDMMC_DMA_DESC_CNT := 100
export SDMMC_SPEED := 48000000
endif

ifeq ($(CONFIG_LCD_GC9503NP),y)
export DSIPLL_MHZ := 500
endif

ifeq ($(CONFIG_LCD_ST7785M),y)
export DSIPLL_MHZ := 200
endif

ifeq ($(CONFIG_LCD_ST7701S),y)
export DSIPLL_MHZ := 300
endif

ifeq ($(CONFIG_LCD_ST7796U),y)
export DSIPLL_MHZ := 300
endif

ifeq ($(CONFIG_LCD_A064),y)
export DSIPLL_MHZ := 300
#60 fps
export LCDCPIX_MHZ:= 20
#30fps
#export LCDCPIX_MHZ:= 10
endif

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

export DSP_USE_GPDMA ?= 0

ifeq ($(CONFIG_RPTUNMEM_ON_PSRAMUHS),y)
export PSRAMUHS_NC_SIZE := $(CONFIG_BES_UNCACHE_HEAP_SIZE)
else
export SRAM_NC_SIZE := $(CONFIG_BES_UNCACHE_HEAP_SIZE)
KBUILD_CPPFLAGS += -DSRAM_NC_SIZE=$(SRAM_NC_SIZE)
endif

KBUILD_CPPFLAGS += -DRAM_NC_SIZE=$(CONFIG_BES_UNCACHE_HEAP_SIZE)
KBUILD_CPPFLAGS += -DRAM_NC_BASE=$(CONFIG_BES_UNCACHE_HEAP_BASE)

KBUILD_CPPFLAGS += -DBES_RPTUN_HEAP_SIZE=$(CONFIG_BES_RPTUN_HEAP_SIZE)

export DSP_RAM_SIZE := $(CONFIG_A7_SRAM_SIZE)-$(SRAM_NC_SIZE)

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
export DSP_RAM_SIZE ?= 0x80000-$(SRAM_NC_SIZE)
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
export SYS_TIME_SUPPORT ?= 1
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


export LIBC_ADAPT_EN ?= 0
ifeq ($(LIBC_ADAPT_EN),1)
core-y += libc_adapter/
LDFLAGS_IMAGE += --wrap _malloc_r --wrap _calloc_r --wrap _free_r --wrap _realloc_r
LDFLAGS_IMAGE += --wrap _malloc_usable_size_r --wrap _memalign_r
LDFLAGS_IMAGE += --wrap _vsprintf_r --wrap _vfprintf_r --wrap _svfiprintf_r --wrap _svfprintf_r

KBUILD_CPPFLAGS += -isystem $(srctree)/libc_adapter/newlibc/fs/include/
KBUILD_CPPFLAGS += -isystem $(srctree)/libc_adapter/newlibc/machine/include/
KBUILD_CPPFLAGS += -isystem $(srctree)/libc_adapter/newlibc/net/include/
OS_KERNEL := $(shell echo $(KERNEL) | tr '[A-Z]' '[a-z]')
KBUILD_CPPFLAGS += -isystem $(srctree)/libc_adapter/newlibc/os/$(OS_KERNEL)/include/

KBUILD_CPPFLAGS += -DLIBC_ADAPT_EN=1
export OS_HAVE_POSIX ?= 1

endif

ifeq ($(AIOT_VFS_EN),1)
core-y += services/aiot/vfs/
endif
ifeq ($(AIOT_LFS_EN),1)
core-y += services/aiot/littlefs/
endif

export MM_SMF_ENABLE ?= 0
ifeq ($(MM_SMF_ENABLE),1)
core-y += services/multimedia/audio/
export SMF ?= 1
core-y += utils/heap/
endif

export A7_DISPLAY ?= 1
ifeq ($(A7_DISPLAY), 1)
export CHIP_HAS_LCDC ?= 1
core-y += platform/drivers/graphic/ platform/drivers/ana/
KBUILD_CPPFLAGS += -DA7_DISPLAY -DBOARD_NAME=a7_dsp
export DISPLAY_V2 ?= 1
ifeq ($(DISPLAY_V2),1)
KBUILD_CPPFLAGS += -DDISPLAY_V2
endif
export CSI_DSI_LOOP ?= 0
ifeq ($(CSI_DSI_LOOP) ,1)
KBUILD_CFLAGS += -DI2C0_IOMUX_INDEX=0
KBUILD_CFLAGS += -DCLKOUT_IOMUX_INDEX=34
KBUILD_CFLAGS += -DLCD_CSI_DMA_MODE
export CSI_TEST ?= 0
ifeq ($(CSI_TEST) ,1)
KBUILD_CFLAGS += -DCSI_TEST
endif
export DSI_TEST ?= 0
ifeq ($(DSI_TEST) ,1)
KBUILD_CFLAGS += -DDSI_TEST
endif
KBUILD_CFLAGS += -DCONFIG_DSI_2LANE_MODE=1
KBUILD_CFLAGS += -DCSI_DSI_LOOP
endif

export A7_LVGL_TEST ?= 0
ifeq ($(A7_LVGL_TEST), 1)
core-y += services/wifi_app/lvgl/
KBUILD_CPPFLAGS += -DA7_LVGL_TEST -Iservices/wifi_app/lvgl/
endif

export A7_H264_TEST ?= 0
ifeq ($(A7_H264_TEST), 1)
KBUILD_CPPFLAGS += -DA7_H264_TEST -Wno-array-bounds -Wno-unused-but-set-variable -Wno-maybe-uninitialized
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
export MCU_PSRAM_SIZE := 0
export PSRAMDSP_SIZE := $(CONFIG_PSRAM_SIZE)
export PSRAMDSP_BASE := $(CONFIG_PSRAM_BASE)
KBUILD_CPPFLAGS += -DPSRAMDSP_SIZE=$(PSRAMDSP_SIZE) -DPSRAMDSP_BASE=$(PSRAMDSP_BASE)
endif
endif

ifeq ($(CONFIG_PSRAMUHS_AP325608A),y)
export PSRAMUHS_AP325608A := 1
export PSRAMUHS_SIZE := 0x2000000-$(PSRAMUHS_NC_SIZE)
export A7_DSP_SPEED ?= 1200
else
export A7_DSP_SPEED ?= 900
endif

export DSP_USE_PSRAM ?= 0
ifeq ($(DSP_USE_PSRAM),1)
KBUILD_CPPFLAGS += -DDSP_USE_PSRAM
OBJCOPFLAGS := -R .psram_text
endif

ifeq ($(PSRAM_ENABLE),1)
export PSRAM_XCCELA_MODE ?= 0
ifeq ($(PSRAM_XCCELA_MODE),1)
export PSRAM_SPEED := 200
PSRAM_TOTAL_SIZE ?= 0x2000000
else
PSRAM_TOTAL_SIZE ?= 0x800000
endif
export PACK_FS_SIZE ?= 0

export PSRAMCP_SIZE ?= 0
export PSRAMFS_SIZE ?= $(PACK_FS_SIZE)
export PSRAMDSP_SIZE ?= 0
export PSRAM_SIZE ?= $(PSRAM_TOTAL_SIZE)
MCU_PSRAM_SIZE ?= $(PSRAM_TOTAL_SIZE)-$(PSRAMCP_SIZE)-$(PSRAMDSP_SIZE)-$(PSRAMFS_SIZE)
KBUILD_CPPFLAGS += -DMCU_PSRAM_SIZE=$(MCU_PSRAM_SIZE)
KBUILD_CPPFLAGS += -DPSRAMCP_SIZE=$(PSRAMCP_SIZE)
KBUILD_CPPFLAGS += -DPSRAMFS_SIZE=$(PSRAMFS_SIZE)
KBUILD_CPPFLAGS += -DPSRAMDSP_SIZE=$(PSRAMDSP_SIZE)
else
export PSRAM_SIZE ?= 0
endif

ifeq ($(PSRAMUHS_ENABLE), 1)
export EXEC_IN_RAM := 0
KBUILD_CPPFLAGS += -DPSRAMUHS_NC_SIZE=$(PSRAMUHS_NC_SIZE)
ifeq ($(PSRAM_ENABLE), 1)
ifeq ($(PSRAMUHS_AP325608A),1)
export PSRAMUHS_SIZE ?= 0x2000000-$(PSRAMUHS_NC_SIZE)
else
export PSRAMUHS_SIZE ?= 0x800000-$(PSRAMUHS_NC_SIZE)
endif
CONFIG_MCU_PSRAMUHS_SIZE ?= 0
export MCU_PSRAMUHS_SIZE ?= $(CONFIG_MCU_PSRAMUHS_SIZE)
export DSP_PSRAMUHS_SIZE ?= $(PSRAMUHS_SIZE)-$(MCU_PSRAMUHS_SIZE)
else
export PSRAMUHS_SIZE ?= 0x1000000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0x800000
endif
KBUILD_CPPFLAGS += -DCHIP_HAS_PSRAMUHS -DPSRAMUHS_ENABLE -DMCU_PSRAMUHS_SIZE=$(MCU_PSRAMUHS_SIZE) -DDSP_PSRAMUHS_SIZE=$(DSP_PSRAMUHS_SIZE)
KBUILD_CPPFLAGS += -DPSRAMDSP_SIZE=$(PSRAMDSP_SIZE)
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
ifeq ($(PSRAMUHS_AP325608A),1)
export OS_HEAP_SIZE ?= 0x1000000
else
export OS_HEAP_SIZE ?= 0x400000
endif
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

export SYS_TIME_SUPPORT ?= 0
ifeq ($(SYS_TIME_SUPPORT),1)
export SYS_TIME_IF ?= 1
ifeq ($(SYS_TIME_IF),1)
KBUILD_CPPFLAGS += -DSYS_TIME_IF=1
endif
endif

export DSP_MATH_SUPPORT ?= 0
ifeq ($(DSP_MATH_SUPPORT),1)
KBUILD_CFLAGS += -DDSP_MATH_SUPPORT
KBUILD_CPPFLAGS += -DDSP_MATH_SUPPORT
endif

ifeq ($(CONFIG_BOOT_RUNFROMFLASH),y)
LDS_FILE := armca_flash.lds
KBUILD_CPPFLAGS += -DCONFIG_BOOT_RUNFROMFLASH
KBUILD_CPPFLAGS += -DCONFIG_FLASH_START=$(CONFIG_FLASH_START)
export FLASH_REGION_SIZE := $(CONFIG_FLASH_SIZE)
else
LDS_FILE := armca_psram.lds
endif

LIB_LDFLAGS += -lstdc++ -lsupc++

CFLAGS_IMAGE +=

LDFLAGS_IMAGE += --print-memory-usage

C_ONLY_FLAGS    := -std=gnu11 -Wno-incompatible-pointer-types
ifeq ($(MM_SMF_ENABLE),1)
C++_ONLY_FLAGS  := -std=gnu++17
else
C++_ONLY_FLAGS  := -std=gnu++11
endif
ifeq ($(NOSYS),0)
CFLAGS_IMAGE += -u _printf_float -u _scanf_float
endif

ifeq ($(MMC_DISK_FATFS_IN_A7),1)
export MMC_DISK_FATFS ?= 1
KBUILD_CPPFLAGS += -DMMC_DISK_FATFS_IN_A7
KBUILD_CPPFLAGS += -DMMC_DISK_FATFS
core-y += services/fs/
core-y += services/wifi_app/mmc_disk/
endif

#LDFLAGS_IMAGE += --wrap malloc --wrap calloc --wrap free --wrap realloc
