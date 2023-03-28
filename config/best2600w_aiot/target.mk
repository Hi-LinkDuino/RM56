ifdef DEV
include $(srctree)/config/sdk-mcu.mk
endif

CHIP        ?= best2003

DEBUG       ?= 1

FPGA        ?= 0

RTOS        ?= 1

LIBC_ROM    ?= 0

export NOSYS ?= 0
export BGA_TX_TEST ?= 1
export WIFI_QFN_RF_REG ?= 0

export CHIP_OCD ?= 1
ifeq ($(CHIP_OCD), 1)
KBUILD_CPPFLAGS += -D__CHIP_OCD__
endif

PWR_CTRL_8DBM ?= 1
ifeq ($(PWR_CTRL_8DBM), 1)
KBUILD_CPPFLAGS += -DPWR_CTRL_8DBM
endif

export OTA_CODE_OFFSET ?= 0

KBUILD_CPPFLAGS += -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET)

ifeq ($(CPU), m4)
ifeq ($(ARCH_ENHANCE), 1)
export UNALIGNED_ACCESS ?= 0
KBUILD_CPPFLAGS += -D__ARM_ARCH_7EM__ENHANCE__
else
export UNALIGNED_ACCESS ?= 1
KBUILD_CPPFLAGS += -DFORCE_DCACHE_DISABLE
endif
endif

export MCU_HIGH_PERFORMANCE_MODE ?= 320

ifeq ($(CHIP),best2003)
export A7_DSP_SPEED ?= 900
else
export A7_DSP_SPEED ?= 1000
endif

#KERNEL      ?= RTX5
#KERNEL      ?= FREERTOS
#KERNEL      ?= RHINO
KERNEL      ?= LITEOS_M
#KERNEL      ?= RTT


export CONSOLE_CMD_EXT ?= 0
ifeq ($(CONSOLE_CMD_EXT), 1)
KBUILD_CFLAGS += -DCONSOLE_CMD_EXT
endif

export AT_MIC_TEST ?= 1
ifeq ($(AT_MIC_TEST), 1)
KBUILD_CFLAGS += -DAT_MIC_TEST
endif

ifeq ($(MCU_DSP),1)
KBUILD_CFLAGS += -DMCU_DSP
endif

export AT_KEY_TEST ?= 1
ifeq ($(AT_KEY_TEST), 1)
KBUILD_CFLAGS += -DAT_KEY_TEST
endif

export CMSIS_API_TEST ?= 0
ifeq ($(CMSIS_API_TEST), 1)
KBUILD_CFLAGS += -DCMSIS_API_TEST
endif

ifneq ($(MIC_SAMPLERATE), )
KBUILD_CFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
KBUILD_CPPFLAGS += -DMIC_SAMPLERATE=$(MIC_SAMPLERATE)
endif

ifneq ($(MIC_PERIOD_MS), )
KBUILD_CFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
KBUILD_CPPFLAGS += -DMIC_PERIOD_MS=$(MIC_PERIOD_MS)
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

export JTAG_ENABLE ?= 1
export SPEECH_MSG_ENABLE ?= 0
ifeq ($(SPEECH_MSG_ENABLE),1)
KBUILD_CFLAGS += -DSPEECH_MSG_ENABLE
ifeq ($(SPEECH_MSG_TEST_CASE),1)
KBUILD_CFLAGS += -DSPEECH_MSG_TEST_CASE
endif
endif

export WIFI_RF_TEST_MODULE ?=1
ifeq ($(WIFI_RF_TEST_MODULE),1)
KBUILD_CPPFLAGS += -D_WIFI_RF_TEST_MODULE_
KBUILD_CFLAGS   += -D_WIFI_RF_TEST_MODULE_
endif

export RF_TRACE_BY_SPRINTF ?= 0
ifeq ($(RF_TRACE_BY_SPRINTF),1)
KBUILD_CFLAGS   += -D__RF_TRACE_BY_SPRINTF__
endif

KBUILD_CPPFLAGS += -D$(KERNEL)_SUPPORT

export CP_BOOT ?= 0

export PSRAM_ENABLE ?= 1

export PSRAM_XCCELA_MODE ?= 0
ifeq ($(PSRAM_XCCELA_MODE),1)
export PSRAM_SPEED := 200
ifeq ($(FLASH_TYPE),1)
PSRAM_TOTAL_SIZE ?= 0x2000000
else
PSRAM_TOTAL_SIZE ?= 0x800000
endif
else
PSRAM_TOTAL_SIZE ?= 0x800000
endif

ifeq ($(PSRAM_ENABLE),1)
export PSRAMCP_SIZE ?= 0
export PSRAMFS_SIZE ?= 0
export PSRAM_SIZE ?= $(PSRAM_TOTAL_SIZE)-$(PSRAMCP_SIZE)-$(PSRAMFS_SIZE)
KBUILD_CPPFLAGS += -DPSRAMFS_SIZE=$(PSRAMFS_SIZE)
export MCU_SLAVE_FS_EN ?= 0
ifeq ($(MCU_SLAVE_FS_EN),1)
KBUILD_CPPFLAGS += -DMCU_SLAVE_FS_EN
endif
else
export PSRAM_SIZE ?= 0
export PSRAMCP_SIZE ?= 0
endif
export PSRAMCPX_BASE := 0x14000000+$(PSRAM_SIZE)
export PSRAMCP_BASE := 0x34000000+$(PSRAM_SIZE)

export PSRAMUHS_ENABLE ?= 0
ifeq ($(CHIP),best2003)
export PSRAMUHS_SPEED ?= 900
else
export PSRAMUHS_SPEED ?= 1000
endif
ifeq ($(PSRAMUHS_ENABLE),1)
export PSRAMUHS_SIZE ?= 0x800000
export DSP_PSRAMUHS_SIZE ?= 0x800000
export MCU_PSRAMUHS_SIZE ?= 0
else
export PSRAMUHS_SIZE := 0
export DSP_PSRAMUHS_SIZE := 0
export MCU_PSRAMUHS_SIZE := 0
endif

export RF_TX_CONTROL_IO ?= 1
ifneq ($(RF_TX_CONTROL_IO),)
KBUILD_CFLAGS += -DRF_TX_CONTROL_IO=$(RF_TX_CONTROL_IO)
endif

export WIFI_RF_OUT_FEM_SW ?= 0
ifneq ($(WIFI_RF_OUT_FEM_SW),)
KBUILD_CFLAGS += -DWIFI_OUT_FEM=$(WIFI_RF_OUT_FEM_SW)
endif

export WIFI_UART_IOMUX_INDEX ?=
ifneq ($(WIFI_UART_IOMUX_INDEX),)
KBUILD_CFLAGS += -DWIFI_UART_IOMUX_INDEX=$(WIFI_UART_IOMUX_INDEX)
endif

ifeq ($(CHIP), best2001)
export RAM_SIZE ?= 0x200000
else
ifeq ($(ARM_CMNS),1)
# RAM_S_SIZE should match the value in plat_addr_map.h
RAM_S_SIZE := 0x20000
export RAM_SIZE ?= 0x160000
else
RAM_S_SIZE := 0
export RAM_SIZE ?= 0x180000
endif
endif

export BT_RAMRUN ?= 1
ifeq ($(BT_RAMRUN), 1)
NO_SLEEP ?= 1
KBUILD_CPPFLAGS += -D__BT_RAMRUN__
export BT_RAMRUN_SIZE ?= 0x80000
export BT_COMP_BIN ?= 1
else
export BT_RAMRUN_SIZE ?= 0
NO_SLEEP ?= 0
endif

ifeq ($(BT_COMP_BIN), 1)
KBUILD_CPPFLAGS += -D__BT_COMP_BIN__
endif

# total ramv size is 0x30000 in 2003
ifeq ($(BT_RAMRUN), 1)
export RAMV_SIZE ?= 0x30000
else
export RAMV_SIZE ?= 0
endif

export WIFI_NO_IPC ?= 1
ifeq ($(CP_BOOT),1)
KBUILD_CPPFLAGS += -DCP_BOOT

export CP_XIP ?= 0
ifeq ($(CP_XIP),1)
KBUILD_CPPFLAGS += -DCP_XIP -DCP_FLASH_SIZE=0x100000
endif

# total cp sram use 512KB = 0x80000
ifneq ($(PSRAMCP_SIZE),0)
export RAMCP_SIZE := 0x40000
export RAMCPX_SIZE := 0
endif
export WIFI_ON_CP ?= 1
ifeq ($(WIFI_ON_CP),1)
export WIFI_NO_IPC := 0
ifeq ($(CHIP), best2001)
KBUILD_CFLAGS += -DWAGT_SYNC_ENABLE
endif
KBUILD_CFLAGS += -DWIFI_ON_CP
export RAMCP_SIZE ?= 0x0F0000
else
export RAMCP_SIZE ?= 0x040000
endif
export RAM_SIZE := $(RAM_SIZE)-$(RAMCP_SIZE)+$(RAMV_SIZE)-$(BT_RAMRUN_SIZE)
export RAMCP_SIZE := $(RAMCP_SIZE)-$(RAMV_SIZE)
export RAMCPX_SIZE := 0
export RAMCPX_BASE := 0x00200000+$(RAM_SIZE)+$(RAM_S_SIZE)
export RAMCP_BASE := 0x20000000+$(RAM_SIZE)+$(RAM_S_SIZE)
else
export RAMCP_SIZE := 0
export RAMCPX_SIZE := 0
endif

export OSTICK_USE_FAST_TIMER ?= 1
export FAST_TIMER_COMPENSATE ?= 1
ifeq ($(KERNEL), LITEOS_M)
export OSTICK_USE_FAST_TIMER := 0
KBUILD_CPPFLAGS += -DOS_HEAP_LINK_NAME=m_aucSysMem0 -DOS_WRAP_MALLOC
LDFLAGS_IMAGE += \
             --wrap _malloc_r --wrap _calloc_r --wrap _realloc_r \
             --wrap _reallocf_r --wrap _free_r
export FACTORY_SECTION_SIZE ?= 0x20000
export FACTORY_SECTOR_OFFSET ?= 0xF000
endif

ifeq ($(KERNEL), FREERTOS)
KBUILD_CPPFLAGS += -DOS_WRAP_MALLOC
LDFLAGS_IMAGE += \
             --wrap _malloc_r --wrap _calloc_r --wrap _realloc_r \
             --wrap _reallocf_r --wrap _free_r
endif

ifeq ($(KERNEL), RHINO)
export NO_STARTFILES ?= 1
KBUILD_CPPFLAGS += -DOS_WRAP_MALLOC
LDFLAGS_IMAGE += --wrap _malloc_r --wrap _calloc_r --wrap _realloc_r --wrap _free_r --wrap _realloc_r
endif

ifeq ($(KERNEL), RTT)
export NO_STARTFILES ?= 1
#KBUILD_CPPFLAGS += -DBES2300WP_USING_SPI1
KBUILD_CPPFLAGS += -DUSE_FLASH_UNI_CHIPID_GEN_MAC
KBUILD_CPPFLAGS += -Wno-double-promotion -Wno-unused-variable -Wno-unused-but-set-variable  \
             -Wno-error=date-time -Wno-error=format-security -Wno-error=implicit-function-declaration \
             -Wno-implicit-fallthrough -Wno-format -Wno-stringop-overflow -Wno-uninitialized \
             -Wno-unused-value -Wno-unused-parameter -Wno-unused-label -Wno-enum-compare
#KBUILD_CFLAGS += -Wno-incompatible-pointer-types
LDFLAGS_IMAGE += --wrap _malloc_r --wrap _calloc_r --wrap _realloc_r --wrap _free_r --wrap _realloc_r
endif

ifeq ($(NO_STARTFILES),1)
KBUILD_CPPFLAGS += -D__NO_STARTFILES__
endif

export IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 0
export USER_SECURE_BOOT	?= 0
# enable:1
# disable:0

export DEBUG_SPP_LOG ?= 0
ifeq ($(DEBUG_SPP_LOG),1)
KBUILD_CFLAGS += -DDEBUG_SPP_LOG
endif

WATCHER_DOG ?= 0

DEBUG_PORT  ?= 1
# 0: usb
# 1: uart0
# 2: uart1

FLASH_CHIP	?= ALL
# GD25Q80C
# GD25Q32C
# ALL

export FLASH_UNIQUE_ID ?= 1

export FLASH_API_HIGHPERFORMANCE ?= 1

export FLASH_API_GUARD_THREAD ?= 1

export ALLOW_WARNING ?= 1

export NO_PWRKEY ?= 1
ifeq ($(NO_PWRKEY),1)
KBUILD_CPPFLAGS += -D__POWERKEY_CTRL_ONOFF_ONLY__
endif

export FORCE_SIGNALINGMODE ?= 0

export FORCE_NOSIGNALINGMODE ?= 0

AUDIO_OUTPUT_MONO ?= 0

AUDIO_OUTPUT_DIFF ?= 0

HW_FIR_EQ_PROCESS ?= 0

SW_IIR_EQ_PROCESS ?= 1

HW_DAC_IIR_EQ_PROCESS ?= 0

HW_IIR_EQ_PROCESS ?= 0

HW_DC_FILTER_WITH_IIR ?= 0

AUDIO_DRC ?= 1

AUDIO_LIMITER ?= 1

PC_CMD_UART ?= 0

AUDIO_SECTION_ENABLE ?= 0

AUDIO_RESAMPLE ?= 1

export ADC_DC_FILTER ?= 0

export AUDIO_OUTPUT_SW_GAIN ?= 1

ifeq ($(AUDIO_OUTPUT_SW_GAIN), 1)
export DAC_DEFAULT_DIG_DBVAL ?= 0
endif

ifneq ($(DAC_DEFAULT_DIG_DBVAL), )
KBUILD_CPPFLAGS += -DDAC_DEFAULT_DIG_DBVAL=$(DAC_DEFAULT_DIG_DBVAL)
endif

ifneq ($(TGT_VOLUME_LEVEL_USER_MAX), )
KBUILD_CPPFLAGS += -DTGT_VOLUME_LEVEL_USER_MAX=$(TGT_VOLUME_LEVEL_USER_MAX)
endif

export RESAMPLE_ANY_SAMPLE_RATE ?= 1

OSC_26M_X4_AUD2BB ?= 1

export BT_LOG_POWEROFF ?= 0

AUDIO_OUTPUT_VOLUME_DEFAULT ?= 10
# range:1~16

AUDIO_INPUT_CAPLESSMODE ?= 0

AUDIO_INPUT_LARGEGAIN ?= 0

AUDIO_CODEC_ASYNC_CLOSE ?= 0

AUDIO_SCO_BTPCM_CHANNEL ?= 1

export A2DP_CP_ACCEL ?= 0

export SCO_CP_ACCEL ?= 0

export SCO_TRACE_CP_ACCEL ?= 0

# For TWS SCO DMA snapshot and low delay
export PCM_FAST_MODE ?= 1

export PCM_PRIVATE_DATA_FLAG ?= 0

export CVSD_BYPASS ?= 1

export LOW_DELAY_SCO ?= 0

export SPEECH_TX_24BIT ?= 0

SPEECH_TX_DC_FILTER ?= 1

SPEECH_TX_AEC2FLOAT ?= 1

SPEECH_TX_NS3 ?= 0

SPEECH_TX_2MIC_NS2 ?= 0

SPEECH_TX_COMPEXP ?= 1

SPEECH_TX_EQ ?= 0

SPEECH_TX_POST_GAIN ?= 0

SPEECH_RX_NS2FLOAT ?= 0

SPEECH_RX_EQ ?= 0

SPEECH_RX_POST_GAIN ?= 0

export TRACE_BUF_SIZE ?= 16*1024
export TRACE_BAUD_RATE := 1500000

export TRACE_GLOBAL_TAG ?= 1

LARGE_RAM ?= 1

HSP_ENABLE ?= 0

BT_HID_DEVICE ?= 0

BT_PBAP_SUPPORT ?= 0

BT_MAP_SUPPORT ?= 0

SBC_FUNC_IN_ROM ?= 0

ROM_UTILS_ON ?= 0

VOICE_PROMPT ?= 0

#### Google related feature ####
# BISTO is a GVA service on Bluetooth audio device
# BISTO is an isolated service relative to GFPS
export BISTO_ENABLE ?= 0

# GSOUND_HOTWORD is a hotword library running on Bluetooth audio device
# GSOUND_HOTWORD is a subset of BISTO
export GSOUND_HOTWORD_ENABLE ?= 0

# GFPS is google fastpair service
# GFPS is an isolated service relative to BISTO
export GFPS_ENABLE ?= 0
#### Google related feature ####

export AMA_VOICE ?= 0

BLE ?= 1

ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -DBLE_ENABLE
endif

#LE audio macro
export A2DP_LC3_ON ?= 1
export BLE_AUDIO_ENABLED ?= 1
export BIS_VOL_SYNC_ENABLED ?= 1

ifeq ($(DSP_CODEC_SUPPORT),1)
KBUILD_CPPFLAGS += -DLC3_IN_A7_DSP_CODEC
endif

ifeq ($(CHIP), best2003)
export BLE_V2 := 1
export USE_BT_ADAPTER := 1
else
export BLE_V2 := 0
export USE_BT_ADAPTER := 0
endif

export BLE_GATT_CLIENT_TEST ?= 0
ifeq ($(BLE_GATT_CLIENT_TEST),1)
KBUILD_CPPFLAGS += -DBLE_GATT_CLIENT_TEST
endif

ifeq ($(USE_BT_ADAPTER),1)
CUSTOMER_DEFINE_ADV_DATA ?= 1
LINK_KEY_ENCRYPT_BY_CUSTOMER ?= 1
endif

ifeq ($(LINK_KEY_ENCRYPT_BY_CUSTOMER), 1)
KBUILD_CPPFLAGS += -DLINK_KEY_ENCRYPT_BY_CUSTOMER
KBUILD_CPPFLAGS += -DTRNG_STATS
endif

# BLE wificonfig macro
export BLE_WIFICONFIG ?= 1
ifeq ($(BLE_WIFICONFIG),1)
KBUILD_CPPFLAGS += -D__BLE_WIFICONFIG__
endif

BT_ONE_BRING_TWO ?= 0

DSD_SUPPORT ?= 0

A2DP_LHDC_ON ?= 0

A2DP_EQ_24BIT ?= 0

A2DP_AAC_ON ?= 1

A2DP_SCALABLE_ON ?= 0

A2DP_LHDC_ON ?= 0

#For free tws pairing feature
FREE_TWS_PAIRING_ENABLED ?= 1

#get mic data from vendor
VAD_IN_BACKGROUND ?= 0

ifeq ($(A2DP_LHDC_ON),1)
A2DP_LHDC_V3 ?= 0
endif

A2DP_LDAC_ON ?= 0

export TX_RX_PCM_MASK ?= 0

A2DP_SCALABLE_ON ?= 0

FACTORY_MODE ?= 1

ENGINEER_MODE ?= 1

ULTRA_LOW_POWER	?= 0

DAC_CLASSG_ENABLE ?= 0

NO_SLEEP ?= 0

CORE_DUMP ?= 0

CORE_DUMP_TO_FLASH ?= 0

export SYNC_BT_CTLR_PROFILE ?= 0

export A2DP_AVDTP_CP ?= 0

export A2DP_DECODER_VER := 2

export AUDIO_TRIGGER_VER := 1

export IBRT ?= 1

export PLAYBACK_FORCE_48K := 1

ifeq ($(IBRT),1)
export SEARCH_UI_COMPATIBLE_UI_V2 ?= 0

export IBRT_UI_V1 ?= 0
ifeq ($(IBRT_UI_V1),1)
KBUILD_CPPFLAGS += -DIBRT_UI_V1
endif

export IBRT_CORE_V2 ?= 1
ifeq ($(IBRT_CORE_V2),1)
KBUILD_CPPFLAGS += -DIBRT_CORE_V2_ENABLE
endif

export IBRT_UI_V2 ?= 1
ifeq ($(IBRT_UI_V2),1)
KBUILD_CPPFLAGS += -DIBRT_UI_V2
endif

export BES_AUD ?= 1
endif

export FUZZ_TEST_SUPPORT ?= 0
ifeq ($(FUZZ_TEST_SUPPORT), 1)
KBUILD_CPPFLAGS += -D__FUZZ_TEST_SUPPORT__
endif

export POWER_MODE   ?= DIG_DCDC

export SPEECH_CODEC ?= 1

USE_THIRDPARTY ?= 0

export SW_TRIG ?= 1

export SPI_AUDIO_TEST ?= 0
ifeq ($(SPI_AUDIO_TEST),1)
KBUILD_CFLAGS += -DSPI_AUDIO_TEST
KBUILD_CFLAGS += -DSPILCD_DMA_ENABLE
export PERIPH_PLL_FREQ ?= 320000000
export SPI_SLAVE ?= 0
ifeq ($(SPI_SLAVE),1)
KBUILD_CFLAGS += -DSPI_SLAVE
endif
endif

export USB_AUDIO_FS ?= 0
ifeq ($(USB_AUDIO_FS),1)
export USB_AUDIO_APP ?= 1
export NO_LIGHT_SLEEP ?= 0
export USB_ISO ?= 1
export USB_AUDIO_UAC2 ?= 0
export USB_HIGH_SPEED ?= 0
KBUILD_CFLAGS += -DUSB_AUDIO_FS
export USB_AUDIO_DYN_CFG ?= 0
KBUILD_CPPFLAGS += -DUSB_AUDIO_ADAPTER
USB_CAPTURE_16K ?= 1
ifeq ($(USB_CAPTURE_16K), 1)
KBUILD_CFLAGS += -DUSB_CAPTURE_16K
endif #endif USB_CAPTURE_16K
export USB_MUTI_MIC ?= 1
ifneq ($(USB_MUTI_MIC),0)
KBUILD_CFLAGS += -DUSB_MUTI_MIC=$(USB_MUTI_MIC)
endif #endif USB_MUTI_MIC
endif #endif USB_AUDIO_FS

export USB_HOST ?= 0
ifeq ($(USB_HOST),1)
KBUILD_CPPFLAGS += -DUSB_HOST
endif

ifeq ($(DSD_SUPPORT),1)
export BTUSB_AUDIO_MODE     ?= 1
export AUDIO_INPUT_MONO     ?= 1
export USB_ISO              ?= 1
export USB_AUDIO_DYN_CFG    ?= 1
export DELAY_STREAM_OPEN    ?= 0
export KEEP_SAME_LATENCY    ?= 1
export HW_FIR_DSD_PROCESS   ?= 1
ifeq ($(HW_FIR_DSD_PROCESS),1)
ifeq ($(CHIP),best2300)
export HW_FIR_DSD_BUF_MID_ADDR  ?= 0x200A0000
export DATA_BUF_START           ?= 0x20040000
endif
endif


KBUILD_CPPFLAGS += \
    -DHW_FIR_DSD_BUF_MID_ADDR=$(HW_FIR_DSD_BUF_MID_ADDR) \
    -DDATA_BUF_START=$(DATA_BUF_START)
endif

ifeq ($(CURRENT_TEST),1)
export VCODEC_VOLT ?= 1.6V
endif

ifeq ($(SW_TRIG),1)
KBUILD_CPPFLAGS += -D__SW_TRIG__
endif

export RX_IQ_CAL ?= 1

export BT_XTAL_SYNC ?= 1

export BTADDR_FOR_DEBUG ?= 1

AUTO_TEST ?= 0

export DUMP_NORMAL_LOG ?= 0

export DUMP_CRASH_LOG ?= 0

SUPPORT_BATTERY_REPORT ?= 1

SUPPORT_HF_INDICATORS ?= 0

SUPPORT_SIRI ?= 1

export A7_DSP_ENABLE ?= 0
export A7_PROJECT ?= dummy
export DSP_IMAGE_COMP ?= 1
export DSP_AUDIO_TASK ?= 0

ifeq ($(A7_PROJECT),kws)
export AQE_KWS ?= 0
export DSP_LIB ?= 1
AQE_KWS_NAME := HAAS
endif

#file system
export LITTLEFS_ENABLE ?= 1
export LITTLEFS_PARTITION_SUPPORT ?= 1
ifeq ($(LITTLEFS_PARTITION_SUPPORT),1)
LDFLAGS_IMAGE += --wrap fseek --wrap fflush --wrap fgetc
endif

export FILE_SYSTEM_SECTION_SIZE ?= 0x4A8000
export FLASH_SECURITY_REGISTER ?= 1

ifeq ($(NOSYS),0)
export LLD_PRINT_ENABLE ?= 1
endif

# disable toolchain time_t
#KBUILD_CFLAGS += -D__time_t_defined -D_TIME_T_DECLARED -Dtime_t=int32_t -DposixconfigENABLE_TIME_T=0

export NO_OVERLAY ?= 1

KBUILD_CPPFLAGS += -DDSP_PSRAMUHS_SIZE=$(DSP_PSRAMUHS_SIZE)
KBUILD_CPPFLAGS += -DMCU_PSRAM_SIZE=$(PSRAM_SIZE) -DMCU_PSRAMUHS_SIZE=$(MCU_PSRAMUHS_SIZE)
KBUILD_CPPFLAGS += -DPSRAMCP_SIZE=$(PSRAMCP_SIZE)
ifneq ($(RAMV_SIZE),0)
export CODEC_KEEP_CLOCK := 1
KBUILD_CPPFLAGS += -DRAMV_SIZE=$(RAMV_SIZE)
endif

ifeq ($(PSRAM_ENABLE), 1)
KBUILD_CPPFLAGS += -DOS_HEAP_SIZE=0x400000
else
KBUILD_CPPFLAGS += -DOS_HEAP_SIZE=0x80000
endif

export RF_USE_SP3T ?= 0
ifneq ($(RF_USE_SP3T),1)
export USE_EXT_SPDT ?= 1
endif

export WIFI_BT_COEX_FDD ?= 1
export WIFI_BT_COEX_HYBRID ?= 1
ifeq ($(WIFI_BT_COEX_FDD),1)
KBUILD_CPPFLAGS += -D__COEX_MODE_FDD__
KBUILD_CFLAGS += -D__COEX_MODE_FDD__
ifeq ($(WIFI_BT_COEX_HYBRID),1)
KBUILD_CFLAGS += -D__COEX_MODE_HYBRID__
endif
endif

export WIFI_POWERSAVE ?= 0
ifeq ($(WIFI_POWERSAVE),1)
KBUILD_CFLAGS += -D__WIFI_POWERSAVE__
endif


export NET_WIFI_DISABLE ?= 0
ifeq ($(NET_WIFI_DISABLE),1)
KBUILD_CFLAGS += -DIPC_DISABLE=1
endif

export TX_AGGR_ADJUST ?= 1
ifeq ($(TX_AGGR_ADJUST),1)
KBUILD_CPPFLAGS += -D__TX_AGGR_ADJUST__
KBUILD_CFLAGS += -D__TX_AGGR_ADJUST__
endif


#lwip support config
export LWIP_VER ?= lwip_2.1.2
ifeq ($(LWIP_VER),lwip)
export POSIX_PATH ?= posix
KBUILD_CFLAGS += -DLWIP_VER_NUM=200
else ifeq ($(LWIP_VER),lwip_2.1.2)
export POSIX_PATH ?= compat/posix
KBUILD_CFLAGS += -DLWIP_VER_NUM=212
endif

export LWIP_ROOT ?= net/$(LWIP_VER)

export NET_LWIP_SUPPORT ?= 1
ifeq ($(NET_LWIP_SUPPORT),1)
#If enable NET_LWIP_SUPPORT, we should at least be responsible for
#LWIP init and netif add, otherwise turn it off.
KBUILD_CFLAGS += -DLWIP_ETHERNETIF=1
#NET_LWIP_EXTERN indicates whether use customer's LWIP stack.
# 0: use LWIP stack in SDK
# 1: use LWIP stack from customer, and BES build ethernetif.c
# 2: use LWIP stack from customer, and let customer build ethernetif.c
export NET_LWIP_EXTERN ?= 0
ifeq ($(NET_LWIP_EXTERN),0)
KBUILD_CFLAGS += -DLWIP_SUPPORT=1
else
KBUILD_CFLAGS += -DLWIP_SUPPORT=0
endif

ifeq ($(NET_LWIP_EXTERN),2)
# Posix compatibility: support posix  struct sockaddr/struct sockadd_in/struct sockaddr_in6
KBUILD_CFLAGS += -DPOSIX_SOCKET_ENABLE
# Posix compatibility: liteos set 1024
KBUILD_CFLAGS += -DFD_SETSIZE=1024
endif

export NET_LIBCURL_SUPPORT ?= 1
export NET_LIBRWS_SUPPORT ?= 1
export NET_MBEDTLS_SUPPORT ?= 1
export NET_MQTT_SUPPORT ?= 0
export NET_NGHTTP2_SUPPORT ?= 0
export NET_COAP_SUPPORT ?= 0
export NET_JANSSON_SUPPORT ?=0
export NET_CJSON_SUPPORT ?= 0
export NET_FTP_CLIENT_SUPPORT ?= 1
export NET_WIFI_SOCKET_SUPPORT ?=0
export NET_PING_SUPPORT ?=1
export WIFI_IPERF_SUPPORT ?= 1
export NET_TELNET_SERVER_SUPPORT ?= 0
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
KBUILD_CFLAGS += -D__NET_LIBCURL__
export CURL_USE_HAL_TIME ?= 1

#each time write 32K data to flash while ota upgrade
CURL_MAX_WRITE_SIZE ?= 16384
ifneq ($(CURL_MAX_WRITE_SIZE),16384)
KBUILD_CFLAGS += -DCURL_MAX_WRITE_SIZE=$(CURL_MAX_WRITE_SIZE)
endif

endif

ifeq ($(NET_LIBRWS_SUPPORT),1)
KBUILD_CFLAGS += -DRWS_OS_CMSIS
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
KBUILD_CFLAGS += -D__NET_MBEDTLS__
export NET_MBEDTLS_AES_ALT ?= 0
ifeq ($(NET_MBEDTLS_AES_ALT),1)
KBUILD_CFLAGS += -DEN_MBEDTLS_AES_ALT
endif
export ENTROPY_HARDWARE_ALT ?= 0
ifeq ($(ENTROPY_HARDWARE_ALT),1)
KBUILD_CFLAGS += -DEN_HARDWARE_ENTROPY_ALT
KBUILD_CFLAGS += -DMBEDTLS_ENTROPY_MIN_HARDWARE=1
endif
export MBEDTLS_FS_IO ?= 0
ifeq ($(MBEDTLS_FS_IO),1)
KBUILD_CPPFLAGS += -DMBEDTLS_FS_IO
endif
export NET_MBEDTLS_TESTS_SUPPORT ?= 0
ifeq ($(NET_MBEDTLS_TESTS_SUPPORT),1)
KBUILD_CPPFLAGS += -DMBEDTLS_PLATFORM_FPRINTF_ALT
endif
endif

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
export LITTLEFS_ENABLE ?= 1
KBUILD_CPPFLAGS +=  -Inet/ftp/ftp_client -D__NET_FTP_CLIENT_SUPPORT__
endif

ifeq ($(NET_WIFI_SOCKET_SUPPORT),1)
KBUILD_CPPFLAGS +=  -DNET_WIFI_SOCKET_SUPPORT
KBUILD_CPPFLAGS +=  -Iservices/wifi_app/wifi_socket/
endif

export MASTER_APP_SUPPORT ?= 1
ifeq ($(MASTER_APP_SUPPORT),1)
KBUILD_CPPFLAGS += -DMASTER_APP_SUPPORT -DCFG_HW_BES_MASTER
endif

ifeq ($(CHIP), best2003)
export REUSE_WIFI_CALI_RESULT ?= 1
endif

export WIFI_APP_SUPPORT ?= 1

ifeq ($(WIFI_APP_SUPPORT),1)
KBUILD_CFLAGS += -D__WIFI_APP_SUPPORT__
ifeq ($(WIFI_ON_CP),1)
KBUILD_CPPFLAGS += -DNET_HEAP_SIZE=0x10000
else
KBUILD_CPPFLAGS += -DNET_HEAP_SIZE=0x70000
endif
endif

ifeq ($(NET_PING_SUPPORT),1)
KBUILD_CFLAGS += -DNET_PING_SUPPORT
endif

ifeq ($(WIFI_IPERF_SUPPORT),1)
KBUILD_CFLAGS += -DWIFI_IPERF_SUPPORT
ifeq ($(NOSYS),0)
CFLAGS_IMAGE += -u _printf_float -u _scanf_float
endif
endif

#KBUILD_CFLAGS += -DTRANSQ_WIFI_EXTEND
#KBUILD_CFLAGS += -DWIFI_CONNECT_WITHOUT_SCAN
KBUILD_CFLAGS += -DWIFI_CONNECT_RETRIES=3
KBUILD_CFLAGS += -DWIFI_RECONNECT_RETRIES=100
ifneq ($(WIFI_ON_CP),1)
KBUILD_CFLAGS += -DWIFI_TX_LONG_RETRIES=15
KBUILD_CFLAGS += -DWIFI_TX_SHORT_RETRIES=15
endif

ifneq ($(WIFI_ON_CP),1)
#support 802.1X
export IEEE8021X_SUPPORT ?= 0

#support 80211w
export IEEE80211W_SUPPORT ?= 1

#support WPA3
export WPA3_SUPPORT ?= 1
ifeq ($(WPA3_SUPPORT),1)
export SAE_SUPPORT ?= 1
export NET_OPENSSL_SUPPORT ?= 1
endif

#support WAPI
export WAPI_SUPPORT ?= 0
endif

export AP_MODE_SUPPORT ?= 1
ifeq ($(AP_MODE_SUPPORT),1)
KBUILD_CFLAGS += -D__AP_MODE__
endif

export P2P_MODE_SUPPORT ?= 0
ifeq ($(P2P_MODE_SUPPORT),1)
KBUILD_CFLAGS += -D__P2P_MODE__
endif

ifeq ($(WIFI_NO_IPC),1)
KBUILD_CFLAGS += -DWIFI_NO_IPC=1
endif

export SNIFFER_MODE_SUPPORT ?= 1
ifeq ($(SNIFFER_MODE_SUPPORT),1)
KBUILD_CFLAGS += -D__SNIFFER_MODE__
endif

export DUAL_BAND ?= 0
ifeq ($(DUAL_BAND),1)
KBUILD_CPPFLAGS += -D__DUAL_BAND__
KBUILD_CFLAGS += -D__DUAL_BAND__
endif

export USE_RTS ?= 1
ifeq ($(USE_RTS),1)
KBUILD_CPPFLAGS += -D__USE_RTS__
KBUILD_CFLAGS += -D__USE_RTS__
KBUILD_CFLAGS += -DRTS_THRESHOLD=512
endif

export RETRY_1M ?= 1
ifeq ($(RETRY_1M),1)
KBUILD_CPPFLAGS += -D__11B_1M__
KBUILD_CFLAGS += -D__11B_1M__
endif

export SHORT_GI ?= 1
ifeq ($(SHORT_GI),1)
KBUILD_CPPFLAGS += -D__SHORT_GI__
endif

export RX_STBC ?= 0
ifeq ($(RX_STBC),1)
KBUILD_CPPFLAGS += -D__RX_STBC__
endif

export GREEN_FIELD ?= 0
ifeq ($(GREEN_FIELD),1)
KBUILD_CPPFLAGS += -D__GREEN_FIELD__
endif

export BT_CLOSE ?= 0
ifneq ($(BT_CLOSE)_$(USER_BT_STACK),1_0)
#KBUILD_CFLAGS += -D__WIFI_BT_COEX__
#KBUILD_CPPFLAGS += -D__WIFI_BT_COEX__
endif

export BT_USE_UX_VOLUME ?= 0
ifeq ($(BT_USE_UX_VOLUME), 1)
KBUILD_CPPFLAGS += -DBT_USE_UX_VOLUME
endif

export HIECHO_ENABLE ?= 0
ifeq ($(HIECHO_ENABLE),1)
KBUILD_CPPFLAGS += -DHIECHO_ENABLE
KBUILD_CFLAGS += -DHIECHO_ENABLE
endif

ifeq ($(A7_DSP_ENABLE),1)
export DSP_ENABLE := 1
export LARGE_RAM := 0
KBUILD_CFLAGS += -D__A7_DSP_ENABLE__ -D__A7_DSP_TRANSQ_AF__
endif

init-y      :=
core-y      := platform/ services/ apps/ services/multimedia/
core-y      += utils/cqueue/ utils/list/ utils/intersyshci/ thirdparty/ utils/decom/ utils/hexdump/

ifeq ($(BFDGL),1)
core-y += gui/
endif
export LIBC_ADAPT_FS_EN ?= 0
ifeq ($(LIBC_ADAPT_FS_EN),1)
core-y += libc_adapter/newlibc/fs/
LDFLAGS_IMAGE += --wrap _open_r --wrap _close_r --wrap _fstat_r --wrap _stat_r \
    --wrap _lseek_r --wrap _read_r --wrap _unlink_r --wrap _write_r --wrap _rename_r
KBUILD_CPPFLAGS += -Ilibc_adapter/newlibc/fs/include/
KBUILD_CPPFLAGS +=-DLFS_BLOCK_CNT=1024
KBUILD_CPPFLAGS += -DLIBC_ADAPT_FS_EN
endif


# **************************
# services config
# **************************
export GEN_SDK_LIB ?= 0
export GEN_SDK_LIB_NET ?= 1
export GEN_SDK_LIB_SERVICE ?= 1
export ENHANCED_STACK ?= 1
export SYS_TIME_SUPPORT ?= 1
ifeq ($(SYS_TIME_SUPPORT),1)
export SYS_TIME_IF ?= 1
ifeq ($(SYS_TIME_IF),1)
KBUILD_CPPFLAGS += -DSYS_TIME_IF=1
endif
endif
#export EAUDIO_ON ?= 1

export AUDIO_PROCESS_IN_A7 ?= 0
ifeq ($(AUDIO_PROCESS_IN_A7),1)
KBUILD_CPPFLAGS += -DAUDIO_PROCESS_IN_A7
endif

export OPUS_CODEC_SUPPORT ?= 0
ifeq ($(OPUS_CODEC_SUPPORT),1)
KBUILD_CFLAGS += -DOPUS_CODEC_SUPPORT
endif
#export EAUDIO_ON ?= 1
export AAC_CODEC_SUPPORT ?= 1
ifeq ($(AAC_CODEC_SUPPORT),1)
KBUILD_CFLAGS += -DAAC_CODEC_SUPPORT
endif

export LINEIN_TRANS_ON ?= 0
ifeq ($(LINEIN_TRANS_ON),1)
VOICE_PROMPT := 1
KBUILD_CPPFLAGS += -DAUDIO_LINEIN
KBUILD_CPPFLAGS += -DAUDIO_BESAUD
KBUILD_CPPFLAGS += -DLINEIN_TRANS_ON
export POWER_ON_ENTER_TWS_PAIRING_ENABLED ?= 1
endif

export MP3_HOST ?= 0
ifeq ($(MP3_HOST),1)
MBED := 1
KBUILD_CPPFLAGS += -DMP3_HOST
endif


# **************************
# services wifi app config
# **************************
export WIFI_VOICE_SUPPORT ?= 1
export LFS_ON_FLASH ?= 1
export LFS_ON_SD_CARD ?= 0
export LFS_ON_RAM ?= 0

export ALSA_EN ?= 1
ifeq ($(ALSA_EN), 1)
KBUILD_CPPFLAGS += -D__ALSA_EN__
export ALSA_CONFIG_INDEX ?= 0
KBUILD_CPPFLAGS += -DALSA_CONFIG_INDEX=$(ALSA_CONFIG_INDEX)
endif # ALSA_EN

export LPLAYER_EN ?= 1
ifeq ($(LPLAYER_EN), 1)
export LPLAYER_DRIVER_EN ?= 1
export LPLAYER_FRAMEWORK_EN ?= 1
export HTTP_MEDIA_SUPPORT_HTTPS ?= 1
ifeq ($(BLE_AUDIO_ENABLED),1)
export AUDIO_SINK_SUPPORT_LE_AUDIO ?= 1
endif
ifeq ($(LPLAYER_FRAMEWORK_EN),1)
export HISTREAMER_RENDER_SUPPORT ?= 0
export LPLAYER_EFFECT_SUPPORT ?= 1
export LPLAYER_WATER_LINE_EN ?= 1
export LPLAYER_TYPE_EN ?= 1
export LPLAYER_USE_CMSIS ?= 1
export AUDIO_SINK_USE_LE_AUDIO ?= 0
export AUDIO_SINK_USE_BT_AUDIO ?= 0
ifneq ($(LPLAYER_USE_CMSIS), 1)
export LPLAYER_NEED_POSIX ?= 1
endif
ifeq ($(LITTLEFS_PARTITION_SUPPORT), 1)
export LPLAYER_FS_EN ?= 0
endif
export LPLAYER_SUPPORT_AT_CMD ?= 1
KBUILD_CPPFLAGS += -DLPLAYER_FRAMEWORK_EN
ifeq ($(LPLAYER_SUPPORT_AT_CMD),1)
KBUILD_CPPFLAGS += -DLPLAYER_SUPPORT_AT_CMD
endif # LPLAYER_SUPPORT_AT_CMD
export HTTS_MEDIA_SET_MBEDTLS_ALT ?= 1
ifeq ($(AUDIO_SINK_USE_LE_AUDIO), 1)
KBUILD_CPPFLAGS += -DAUDIO_SINK_USE_LE_AUDIO
endif
ifeq ($(AUDIO_SINK_USE_BT_AUDIO), 1)
KBUILD_CPPFLAGS += -DAUDIO_SINK_USE_BT_AUDIO
endif
else
export HISTREAMER_RENDER_SUPPORT ?= 1
export LPLAYER_EFFECT_SUPPORT ?= 0
export LPLAYER_WATER_LINE_EN ?= 0
export LPLAYER_TYPE_EN ?= 0
export HTTPS_CA_USE_FS_IO ?= 1
export HTTS_MEDIA_SET_MBEDTLS_ALT ?= 0
endif # LPLAYER_FRAMEWORK_EN
export MP3_CODEC_USE_MINIMP3_2 ?= 1
export MINIMP4_DEMUXER_SUPPORT ?= 1
export FDKAAC_USE_MALLOC ?= 1
KBUILD_CPPFLAGS += -DFDKAAC_USE_MALLOC
export FLAC_SUPPORT ?= 1
endif

ifeq ($(HISTREAMER_RENDER_SUPPORT),1)
KBUILD_CPPFLAGS += -DHISTREAMER_RENDER_SUPPORT
endif

ifeq ($(LPLAYER_EFFECT_SUPPORT),1)
KBUILD_CPPFLAGS += -DLPLAYER_EFFECT_SUPPORT
endif

ifeq ($(LPLAYER_WATER_LINE_EN),1)
KBUILD_CPPFLAGS += -DLPLAYER_WATER_LINE_EN
endif

ifeq ($(LPLAYER_TYPE_EN),1)
KBUILD_CPPFLAGS += -DLPLAYER_TYPE_EN
endif

export USB_HOST ?= 0
ifeq ($(USB_HOST),1)
core-y      += platform/drivers/usb/usb_host/
endif

export I2C_USE_DMA ?= 1

ifeq ($(A7_DSP_ENABLE),1)
core-y      += services/a7_dsp/prj/$(A7_PROJECT)/
endif

export NET_SUPPORT ?= 1
ifeq ($(NET_SUPPORT),1)
core-y      += net/
endif

export OPENAMP_SUPPORT ?= 0
ifeq ($(OPENAMP_SUPPORT),1)
core-y      += services/amp/
KBUILD_CPPFLAGS += -DOPENAMP_SUPPORT
KBUILD_CPPFLAGS += -DSHARED_MEM_SIZE=0x10000
endif

export VIRTIO_MASTER_ONLY ?= 1
ifeq ($(VIRTIO_MASTER_ONLY),1)
KBUILD_CPPFLAGS += -DVIRTIO_MASTER_ONLY
endif

export DLNA_SUPPORT ?= 0
export DLNA_SUPPORT_TEST ?= 0
ifeq ($(DLNA_SUPPORT),1)
KBUILD_CPPFLAGS += -DDLNA_SUPPORT
endif

export AIRPLAY_SUPPORT ?= 0
export AIRPLAY_SUPPORT_APP_TEST ?= 0
ifeq ($(AIRPLAY_SUPPORT),1)
export ALLOW_WARNING := 1
#export FDKAAC_VERSION := fdkaac_codec_2_0_1_full
KBUILD_CPPFLAGS += -DAIRPLAY_SUPPORT
endif

export MFI_WAC_SUPPORT ?= 0
ifeq ($(MFI_WAC_SUPPORT),1)
KBUILD_CPPFLAGS += -DMFI_WAC_SUPPORT
endif


ifneq ($(PA_ENABLE_IO),)
KBUILD_CFLAGS += -DPA_ENABLE_IO=$(PA_ENABLE_IO)
endif

KBUILD_CPPFLAGS +=  -Iplatform/cmsis/inc \
                    -Iservices/audioflinger \
                    -Iplatform/hal

KBUILD_CPPFLAGS +=  -Inet -Inet/wifi\
		-Iplatform/drivers/wifi \
		-Iplatform/drivers/usb/usb_dev/inc
KBUILD_CPPFLAGS += -Imbed/api -Imbed/common -Imbed/targets/hal/TARGET_BEST/TARGET_BEST100X/TARGET_MBED_BEST1000 \
				   -Imbed/targets/hal/TARGET_BEST/TARGET_BEST100X -Imbed/hal
KBUILD_CPPFLAGS += -I$(LWIP_ROOT)/src/include

KBUILD_CPPFLAGS += \
    -D__APP_KEY_FN_STYLE_A__ \
    -D__EARPHONE_STAY_BOTH_SCAN__

#    -D__PC_CMD_UART__

#KBUILD_CPPFLAGS += -D__3M_PACK__

#-D_AUTO_SWITCH_POWER_MODE__
#-D__APP_KEY_FN_STYLE_A__
#-D__APP_KEY_FN_STYLE_B__
#-D__EARPHONE_STAY_BOTH_SCAN__
#-D__POWERKEY_CTRL_ONOFF_ONLY__
#-DAUDIO_LINEIN

ifeq ($(ROM_BUILD),1)
LDS_FILE	:= rom.lds
ROM_SIZE	:= 0x80000
else ifeq ($(NANDFLASH_BUILD),1)
LDS_FILE	:= best1000_nand.lds
else
LDS_FILE	:= best1000.lds
endif

ifneq ($(PSRAM_ENABLE)-$(PSRAMUHS_ENABLE),0-0)
LDS_FILE	:= best2001.lds
endif


ifeq ($(DSP_IMAGE_COMP), 1)
KBUILD_CPPFLAGS += -DDSP_IMAGE_COMP
endif

ifeq ($(NO_STARTFILES),1)
KBUILD_CPPFLAGS += -D__NO_STARTFILES__
endif

KBUILD_CPPFLAGS +=

KBUILD_CFLAGS +=

LIB_LDFLAGS += -lstdc++ -lsupc++


ifneq ($(A2DP_DECODER_VER), )
KBUILD_CPPFLAGS += -DA2DP_DECODER_VER=$(A2DP_DECODER_VER)
endif

ifneq ($(AUDIO_TRIGGER_VER), )
KBUILD_CPPFLAGS += -DAUDIO_TRIGGER_VER=$(AUDIO_TRIGGER_VER)
endif

#CFLAGS_IMAGE += -u _printf_float -u _scanf_float

#LDFLAGS_IMAGE += --wrap main

ifeq ($(WIFI_TEST),1)
KBUILD_CPPFLAGS += -D__WIFI_TEST__
endif

ifeq ($(WIFI_FW_RAMRUN),1)
KBUILD_CPPFLAGS += -D__WIFI_FW_RAMRUN__
endif

ifeq ($(AT_CMD),1)
KBUILD_CPPFLAGS += -D__AT_CMD_SUPPORT__
endif

export LVGL_TEST ?= 0
ifeq ($(LVGL_TEST),1)
KBUILD_CPPFLAGS += -Iservices/wifi_app/lvgl/ -DLVGL_TEST
#KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_ICNA3311
#KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_RM69330
#KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_ILI9488
#KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_ST7701
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_A064 -DI2C1_IOMUX_INDEX=06
endif

export CALIB_SLOW_TIMER := 1
export USB_STACK ?= 0
export USB_HIGH_SPEED ?= 0
ifeq ($(USB_STACK),1)
core-y += services/usb_stack/
KBUILD_CPPFLAGS += -DUSB_STACK
endif

BOARD_EVB_VERSION ?= 3
ifeq ($(BOARD_EVB_VERSION), 3)
KBUILD_CPPFLAGS += -DEVB_V3
WORDROUND_FOR_DSI_CLK_PN_SWAP ?= 1
else ifeq ($(BOARD_EVB_VERSION), 2)
KBUILD_CPPFLAGS += -DEVB_V2
WORDROUND_FOR_DSI_LANE0_PN_SWAP ?= 1
endif

ifeq ($(WORDROUND_FOR_DSI_CLK_PN_SWAP), 1)
KBUILD_CPPFLAGS += -DWORDROUND_FOR_DSI_CLK_PN_SWAP
endif

ifeq ($(WORDROUND_FOR_DSI_LANE0_PN_SWAP), 1)
KBUILD_CPPFLAGS += -DWORDROUND_FOR_DSI_LANE0_PN_SWAP
endif

ifeq ($(WORDROUND_FOR_DSI_LANE1_PN_SWAP), 1)
KBUILD_CPPFLAGS += -DWORDROUND_FOR_DSI_LANE1_PN_SWAP
endif

export CSI_REPORT ?= 0
ifeq ($(CSI_REPORT), 1)
KBUILD_CPPFLAGS += -DCSI_REPORT
endif

export LC3_AUDIO_MIXED_MONO ?=1
ifeq ($(LC3_AUDIO_MIXED_MONO),1)
KBUILD_CPPFLAGS += -DLC3_AUDIO_MIXED_MONO
endif

export VOW_MICPHONE ?= 0

KBUILD_CPPFLAGS += -Wno-unused-variable -Wno-unused-function

export FORCE_TO_USE_TARGET_LIB ?= 0
