
add_if_exists = $(foreach d,$(1),$(if $(wildcard $(srctree)/$(d)),$(d) ,))

# -------------------------------------------
# CHIP selection
# -------------------------------------------

export CHIP

ifneq (,)
else ifeq ($(CHIP),best1000)
KBUILD_CPPFLAGS += -DCHIP_BEST1000
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 1
export CHIP_FLASH_CTRL_VER := 1
export CHIP_PSRAM_CTRL_VER := 1
export CHIP_SPI_VER := 1
export CHIP_HAS_EC_CODEC_REF := 0
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best1305)
KBUILD_CPPFLAGS += -DCHIP_BEST1305
CPU := m33
SHA256_ROM := 0
export CHIP_HAS_SECURE_BOOT := 0
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 0
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 3
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2S := 1
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
export CHIP_HAS_SPDIF := 0
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_CP := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 4
export CHIP_CACHE_VER := 3
export CHIP_SPI_VER := 4
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
else ifeq ($(CHIP),best1400)
ifeq ($(CHIP_SUBTYPE),best1402)
SUBTYPE_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_BEST1402
export CHIP_FLASH_CTRL_VER := 3
else
KBUILD_CPPFLAGS += -DCHIP_BEST1400
export CHIP_FLASH_CTRL_VER := 2
endif
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 1
export CHIP_HAS_SPDIF := 0
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_SPI_VER := 3
export BTDUMP_ENABLE := 1
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best1501)
KBUILD_CPPFLAGS += -DCHIP_BEST1501
ifeq ($(CHIP_SUBSYS),sensor_hub)
SUBSYS_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_SUBSYS_SENS
export CHIP_HAS_DMA := 1
export CHIP_HAS_I2C := 4
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_I2S := 1
else
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_CP := 1
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_I2S := 2
endif
CPU := m33
export CHIP_HAS_FPU := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 4
export CHIP_PSRAM_CTRL_VER := 2
export CHIP_SPI_VER := 5
export CHIP_CACHE_VER := 3
export CHIP_INTERSYS_VER := 2
export CHIP_GPIO_VER := 2
export PSC_GPIO_IRQ_CTRL := 1
export CHIP_RAM_BOOT := 1
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 1
export BLE_V2 :=1
else ifeq ($(CHIP),best1600)
KBUILD_CPPFLAGS += -DCHIP_BEST1600
ifeq ($(BTH_AS_MAIN_MCU),1)
export BOOT_LOADER_ENTRY_HOOK := 1
KBUILD_CPPFLAGS += -DBTH_AS_MAIN_MCU
endif
ifeq ($(CHIP_SUBSYS),bth)
SUBSYS_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_SUBSYS_BTH
CPU := m33
LIBC_ROM := 0
CRC32_ROM := 0
SHA256_ROM := 0
export CHIP_HAS_DMA := 1
export CHIP_HAS_UART := 2
ifeq ($(FPGA),1)
export CHIP_HAS_I2C := 1
export CHIP_HAS_I2S := 1
export CHIP_HAS_TDM := 1
endif
else ifeq ($(CHIP_SUBSYS),sensor_hub)
SUBSYS_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_SUBSYS_SENS
CPU := m33
export CHIP_HAS_DMA := 1
export CHIP_HAS_I2C := 5
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_UART := 3
export CHIP_HAS_I2S := 1
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
else
CPU := m55
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2C := 2
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_I2S := 2
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
endif
export CHIP_HAS_FPU := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 1
export CHIP_CACHE_VER := 4
export CHIP_FLASH_CTRL_VER := 6
export CHIP_PSRAM_CTRL_VER := 2
export CHIP_SPI_VER := 5
export CHIP_INTERSYS_VER := 2
export CHIP_GPIO_VER := 2
export PSC_GPIO_IRQ_CTRL := 1
export CHIP_RAM_BOOT := 0
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 1
export BLE_V2 :=1
else ifeq ($(CHIP),best2000)
KBUILD_CPPFLAGS += -DCHIP_BEST2000
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 2
export CHIP_HAS_TRANSQ := 1
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 1
export CHIP_PSRAM_CTRL_VER := 1
export CHIP_SPI_VER := 1
export CHIP_HAS_EC_CODEC_REF := 0
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best2001)
KBUILD_CPPFLAGS += -DCHIP_BEST2001
ifeq ($(CHIP_SUBSYS),dsp)
SUBSYS_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_BEST2001_DSP
CPU := a7
DSP_ENABLE ?= 1
else
CPU := m33
export CHIP_HAS_CP := 1
endif
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_PSRAMUHS := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_SPIDPD := 1
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
ifeq ($(DSP_ENABLE), 1)
export DSP_USE_GPDMA ?= 1
ifeq ($(LARGE_RAM), 1)
$(error LARGE_RAM conflicts with DSP_ENABLE)
endif
endif
ifeq ($(DSP_USE_GPDMA), 1)
export CHIP_HAS_DMA := 1
KBUILD_CPPFLAGS += -DDSP_USE_GPDMA
else
ifeq ($(CHIP_SUBSYS),dsp)
export CHIP_HAS_DMA := 0
else
export CHIP_HAS_DMA := 2
endif
endif
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 2
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 3
export CHIP_PSRAM_CTRL_VER := 2
export CHIP_SPI_VER := 4
export CHIP_CACHE_VER := 2
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
export FLASH_SIZE ?= 0x1000000
export OSC_26M_X4_AUD2BB := 0
export USB_USE_USBPLL := 1
export CHIP_HAS_A7_DSP := 1
export CHIP_HAS_WIFI := 1
export A7_DSP_SPEED ?= 1100
#780:780M, 1000:1G, 1100:1.1G
else ifeq ($(CHIP),best2002)
KBUILD_CPPFLAGS += -DCHIP_BEST2002
CPU := m33
export CHIP_HAS_CP := 1
export CHIP_INTERSYS_VER := 2
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_SPI := 0
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 1
export CHIP_HAS_I2S := 1
export CHIP_HAS_TRANSQ := 1
export CHIP_HAS_EXT_PMU := 0
export AF_DEVICE_INT_CODEC ?= 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 5
export FLASH_LOCK_CP_ACCESS := 1
export CHIP_SPI_VER := 4
export CHIP_HAS_SPIPHY := 1
export CHIP_CACHE_VER := 4
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export NO_LPU_26M ?= 0
export FLASH_SIZE ?= 0x100000
export OSC_26M_X4_AUD2BB := 0
export USB_USE_USBPLL := 0
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 0
export CHIP_HAS_SDIO_DEVICE := 1
export CHIP_HAS_WIFI := 1
export CHIP_WIFITSF_VER := 2
else ifeq ($(CHIP),best2003)
KBUILD_CPPFLAGS += -DCHIP_BEST2003
# KBUILD_CPPFLAGS += -DEXTEND_TRANSQ_2003
ifeq ($(CHIP_SUBSYS),dsp)
SUBSYS_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_BEST2003_DSP
CPU := a7
DSP_ENABLE ?= 1
else
ifeq ($(CHIP_SUBSYS),cmcp)
SUBSYS_VALID := 1
endif
CPU := m33
export CHIP_HAS_CP := 1
endif
export CHIP_INTERSYS_VER := 2
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_PSRAMUHS := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_SPIDPD := 0
export CHIP_HAS_I2C := 3
export CHIP_HAS_UART := 4
ifeq ($(DSP_ENABLE), 1)
export DSP_USE_GPDMA ?= 1
ifeq ($(LARGE_RAM), 1)
$(error LARGE_RAM conflicts with DSP_ENABLE)
endif
endif
ifeq ($(DSP_USE_GPDMA), 1)
export CHIP_HAS_DMA := 1
KBUILD_CPPFLAGS += -DDSP_USE_GPDMA
else
ifeq ($(CHIP_SUBSYS),dsp)
export CHIP_HAS_DMA := 0
else
export CHIP_HAS_DMA := 2
endif
endif
export CHIP_HAS_I2S := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 2
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 5
export NANDFLASH_SUPPORT ?= 0
export FLASH_LOCK_CP_ACCESS := 1
export CHIP_PSRAM_CTRL_VER := 3
export CHIP_PSRAM_SQPI_MODE := 0
export CHIP_SPI_VER := 4
export CHIP_CACHE_VER := 4
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export NO_LPU_26M ?= 0
export FLASH_SIZE ?= 0x1000000
export OSC_26M_X4_AUD2BB := 0
export USB_USE_USBPLL := 0
export CHIP_HAS_A7_DSP := 1
#780:780M, 1000:1G, 1100:1.1G
export A7_DSP_SPEED ?= 1100
export PSRAMUHS_SPEED ?= 900
export PSRAM_SPEED ?= 200
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_WIFI := 1
else ifeq ($(CHIP),best2300)
KBUILD_CPPFLAGS += -DCHIP_BEST2300
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2S := 1
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 2
export CHIP_SPI_VER := 2
export CHIP_HAS_EC_CODEC_REF := 0
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best2300a)
KBUILD_CPPFLAGS += -DCHIP_BEST2300A
CPU := m33
LIBC_ROM := 0
CRC32_ROM := 0
SHA256_ROM := 0
export LIBC_OVERRIDE ?= 1
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 3
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2S := 2
export CHIP_HAS_TDM := 2
export CHIP_HAS_I2S_TDM_TRIGGER := 1
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_CP := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 3
export CHIP_SPI_VER := 4
export CHIP_CACHE_VER := 2
export CHIP_RAM_BOOT := 1
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 1
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export NO_LPU_26M ?= 1
export BOOT_LOADER_ENTRY_HOOK ?= 1
else ifeq ($(CHIP),best2300p)
KBUILD_CPPFLAGS += -DCHIP_BEST2300P
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 1
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2S := 2
export CHIP_HAS_TDM := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 1
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_CP := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 2
export CHIP_SPI_VER := 3
export CHIP_CACHE_VER := 2
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best3001)
ifeq ($(CHIP_SUBTYPE),best3005)
SUBTYPE_VALID := 1
KBUILD_CPPFLAGS += -DCHIP_BEST3005
export CHIP_CACHE_VER := 2
export CHIP_FLASH_CTRL_VER := 2
else
KBUILD_CPPFLAGS += -DCHIP_BEST3001
export CHIP_FLASH_CTRL_VER := 1
endif
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 1
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_SPI_VER := 3
export CHIP_HAS_EC_CODEC_REF := 0
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best3003)
KBUILD_CPPFLAGS += -DCHIP_BEST3003
CPU := m33
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 0
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 0
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_CACHE_VER := 2
export CHIP_FLASH_CTRL_VER := 2
export CHIP_SPI_VER := 4
export CHIP_HAS_DCO ?= 1
export CHIP_HAS_SECURE_BOOT := 1
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),fpga1000)
KBUILD_CPPFLAGS += -DCHIP_FPGA1000
KBUILD_CPPFLAGS += -DCHIP_BEST1000
CPU := m4
export CHIP_HAS_FPU := 1
export CHIP_HAS_USB := 1
export CHIP_HAS_USBPHY := 0
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_PSRAM := 1
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPILCD := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_I2C := 1
export CHIP_HAS_UART := 2
export CHIP_HAS_DMA := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_EXT_PMU := 0
export CHIP_HAS_AUDIO_CONST_ROM := 1
export CHIP_FLASH_CTRL_VER := 1
export CHIP_PSRAM_CTRL_VER := 1
export CHIP_SPI_VER := 1
export CHIP_HAS_EC_CODEC_REF := 0
export CHIP_HAS_SCO_DMA_SNAPSHOT := 0
export CHIP_ROM_UTILS_VER := 1
export CHIP_HAS_SECURE_BOOT := 1
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 0
export NO_LPU_26M ?= 1
else ifeq ($(CHIP),best1501simu)
KBUILD_CPPFLAGS += -DCHIP_BEST1501SIMU
export CHIP_HAS_USB := 0
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_CP := 0
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 3
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_I2S := 2
CPU ?= m4

ifeq ($(CPU),m4)
KBUILD_CPPFLAGS += -DCPU_M4
endif

ifeq ($(CHIP_SUB),best1501p)
KBUILD_CPPFLAGS += -D__FPGA_1501P__
KBUILD_CPPFLAGS += -DFLASH_ULTRA_LOW_SPEED
endif


export CHIP_HAS_FPU := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 0
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 4
export CHIP_PSRAM_CTRL_VER := 2
export CHIP_SPI_VER := 5
export CHIP_CACHE_VER := 3
export CHIP_INTERSYS_VER := 2
export CHIP_GPIO_VER := 2
export PSC_GPIO_IRQ_CTRL := 1
export CHIP_RAM_BOOT := 1
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 0
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 1
export BLE_V2 :=1
ifeq ($(RAMSTART),1)
KBUILD_CPPFLAGS += -DRAMSTART
endif
ifeq ($(MAGIC_NUM),1)
KBUILD_CPPFLAGS += -DMAGIC_NUM_AUTO
endif
ifeq ($(ASIC_SIMU),1)
KBUILD_CPPFLAGS += -DASIC_SIMU
endif
else ifeq ($(CHIP),best1600simu)
KBUILD_CPPFLAGS += -DCHIP_BEST1600SIMU
export CHIP_HAS_USB := 0
export CHIP_HAS_USBPHY := 1
export CHIP_HAS_SDMMC := 1
export CHIP_HAS_CP := 0
export CHIP_HAS_DMA := 2
export CHIP_HAS_I2C := 2
export CHIP_HAS_UART := 2
export CHIP_HAS_SPDIF := 1
export CHIP_HAS_I2S := 1
CPU ?= m4

ifeq ($(CPU),m4)
KBUILD_CPPFLAGS += -DCPU_M4
endif

export CHIP_HAS_FPU := 1
export CHIP_HAS_PSRAM := 0
export CHIP_HAS_SPI := 1
export CHIP_HAS_SPIPHY := 0
export CHIP_HAS_TDM := 1
export CHIP_HAS_I2S_TDM_TRIGGER := 1
export CHIP_HAS_TRANSQ := 0
export CHIP_HAS_PSC := 0
export CHIP_HAS_EXT_PMU := 1
export CHIP_HAS_AUDIO_CONST_ROM := 0
export CHIP_FLASH_CTRL_VER := 4
export CHIP_PSRAM_CTRL_VER := 2
export CHIP_SPI_VER := 5
export CHIP_CACHE_VER := 3
export CHIP_INTERSYS_VER := 2
export CHIP_GPIO_VER := 2
export PSC_GPIO_IRQ_CTRL := 1
export CHIP_RAM_BOOT := 1
export CHIP_HAS_EC_CODEC_REF := 1
export CHIP_HAS_SCO_DMA_SNAPSHOT := 1
export CHIP_ROM_UTILS_VER := 2
export CHIP_HAS_SECURE_BOOT := 0
export CHIP_HAS_ANC_HW_GAIN_SMOOTHING := 1
export BLE_V2 :=1
ifeq ($(RAMSTART),1)
KBUILD_CPPFLAGS += -DRAMSTART
endif
ifeq ($(MAGIC_NUM),1)
KBUILD_CPPFLAGS += -DMAGIC_NUM_AUTO
endif
else
$(error Invalid CHIP: $(CHIP))
endif

ifneq ($(CHIP_SUBTYPE),)
ifneq ($(SUBTYPE_VALID),1)
$(error Invalid CHIP_SUBTYPE=$(CHIP_SUBTYPE) for CHIP=$(CHIP))
endif
export CHIP_SUBTYPE
endif

ifneq ($(CHIP_SUBSYS),)
ifneq ($(SUBSYS_VALID),1)
$(error Invalid CHIP_SUBSYS=$(CHIP_SUBSYS) for CHIP=$(CHIP))
endif
export CHIP_SUBSYS
endif

ifeq ($(CPU),)
CPU := m33
endif
export CPU

ifneq ($(filter a%,$(CPU)),)
# Override lds file
ifeq ($(PSRAMUHS_ENABLE), 1)
LDS_FILE := armca_psram.lds
else
ifeq ($(FT_TEST_2001),1)
LDS_FILE := armca_ft2001.lds
else
LDS_FILE := armca.lds
endif
endif

ifeq ($(GEN_BOOT_SECTION),1)
CPPFLAGS_${LDS_FILE} += -DGEN_BOOT_SECTION
endif

ifeq ($(EXEC_IN_RAM),1)
CPPFLAGS_${LDS_FILE} += -DEXEC_IN_RAM
else ifeq ($(EXEC_IN_PSRAM),1)
CPPFLAGS_${LDS_FILE} += -DEXEC_IN_PSRAM
endif
endif

ifeq ($(CHIP_HAS_A7_DSP),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_A7_DSP
endif

KBUILD_CPPFLAGS += -DCHIP_HAS_UART=$(CHIP_HAS_UART)
ifneq ($(CHIP_HAS_I2C),)
KBUILD_CPPFLAGS += -DCHIP_HAS_I2C=$(CHIP_HAS_I2C)
endif

ifeq ($(CHIP_HAS_USB),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_USB
endif

ifneq ($(filter-out 0,$(CHIP_HAS_TRANSQ)),)
KBUILD_CPPFLAGS += -DCHIP_HAS_TRANSQ=$(CHIP_HAS_TRANSQ)
endif

ifeq ($(NO_TRUSTZONE),1)
KBUILD_CPPFLAGS += -DNO_TRUSTZONE
endif

ifneq ($(NUTTX_BUILD),1)
ifneq ($(filter 1,$(NO_CP) $(ARM_CMSE)),)
export CHIP_HAS_CP := 0
endif
endif

ifeq ($(CHIP_HAS_CP),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_CP
endif

ifeq ($(CHIP_HAS_AUDIO_CONST_ROM),1)
KBUILD_CPPFLAGS += -DCHIP_HAS_AUDIO_CONST_ROM
endif

ifeq ($(CHIP_HAS_ANC_HW_GAIN_SMOOTHING),1)
KBUILD_CPPFLAGS += -DANC_HW_GAIN_SMOOTHING
endif

ifeq ($(CORE_SLEEP_POWER_DOWN),1)
KBUILD_CPPFLAGS += -DCORE_SLEEP_POWER_DOWN
endif

ifneq ($(SECURE_BOOT_VER),)
KBUILD_CPPFLAGS += -DSECURE_BOOT_VER=$(SECURE_BOOT_VER)
endif

ifeq ($(CHIP_HAS_EXT_PMU),1)
export PMU_IRQ_UNIFIED ?= 1
endif

# -------------------------------------------
# Standard C library
# -------------------------------------------
export NUTTX_BUILD ?= 0

export NOSTD
export LIBC_ROM

ifeq ($(NOSTD),1)

ifeq ($(MBED),1)
$(error Invalid configuration: MBED needs standard C library support)
endif
ifeq ($(RTOS),1)
ifneq ($(NO_LIBC),1)
$(error Invalid configuration: RTOS needs standard C library support)
endif
endif

ifneq ($(NO_LIBC),1)
core-y += utils/libc/
KBUILD_CPPFLAGS += -Iutils/libc/inc
endif

SPECS_CFLAGS :=

LIB_LDFLAGS := $(filter-out -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys,$(LIB_LDFLAGS))

KBUILD_CPPFLAGS += -ffreestanding
ifeq ($(TOOLCHAIN),armclang)
# 1) Avoid -nostdinc
#    CMSIS header files need arm_compat.h, which is one of toolchain's standard header files
# 2) Always -nostdlib for compiling C/C++ files
#    Never convert standard API calls to non-standard library calls, but just emit standard API calls
# 3) Avoid -nostdlib for linking final image
#    Some 64-bit calculations and math functions need toolchain's standard library
KBUILD_CPPFLAGS += -nostdlib
else
KBUILD_CPPFLAGS += -nostdinc
CFLAGS_IMAGE += -nostdlib
endif

KBUILD_CPPFLAGS += -DNOSTD

else # NOSTD != 1

ifneq ($(filter 1,$(LIBC_ROM) $(LIBC_OVERRIDE)),)
core-y += utils/libc/
endif

ifeq ($(TOOLCHAIN),armclang)
LIB_LDFLAGS := $(filter-out -lsupc++,$(LIB_LDFLAGS))
else
ifneq ($(NOSYS),1)
SPECS_CFLAGS := --specs=nano.specs
else
SPECS_CFLAGS := --specs=nosys.specs
endif
LIB_LDFLAGS += -lm -lc -lgcc -lnosys
endif

endif # NOSTD != 1

# -------------------------------------------
# RTOS library
# -------------------------------------------

export RTOS

ifeq ($(RTOS),1)

ifeq ($(CPU),m4)
KERNEL ?= RTX
else
KERNEL ?= RTX5
ifeq ($(KERNEL),RTX)
$(error RTX doesn't support $(CPU))
endif
endif

export KERNEL

VALID_KERNEL_LIST := RTX RTX5 FREERTOS NUTTX RTT LITEOS_M LITEOS_A RHINO

ifeq ($(filter $(VALID_KERNEL_LIST),$(KERNEL)),)
$(error Bad KERNEL=$(KERNEL). Valid values are: $(VALID_KERNEL_LIST))
endif

core-y += rtos/

KBUILD_CPPFLAGS += -DRTOS
KBUILD_CPPFLAGS += -DKERNEL_$(KERNEL)
#CPPFLAGS_${LDS_FILE} += -DKERNEL_$(KERNEL)

ifeq ($(KERNEL),LITEOS_M)
KBUILD_CPPFLAGS += -DPOSIX_SUPPORT
ifeq ($(GEN_LIB),1)
KBUILD_CPPFLAGS += -DOHOS_LIB
endif
ifeq ($(SDK),1)
KBUILD_CPPFLAGS += -DOHOS_SDK
endif
endif

ifeq ($(KERNEL),NUTTX)
LIB_LDFLAGS := $(filter-out -lstdc++ -lc -lnosys,$(LIB_LDFLAGS)) -nodefaultlibs -nostdlib
KBUILD_CPPFLAGS += \
	-Iinclude/rtos/nuttx/
else ifeq ($(KERNEL),RTX)
KBUILD_CPPFLAGS += \
	-Iinclude/rtos/rtx/
KBUILD_CPPFLAGS += -D__RTX_CPU_STATISTICS__=1
#KBUILD_CPPFLAGS += -DTASK_HUNG_CHECK_ENABLED=1
else ifeq ($(KERNEL),RTX5)
OS_IDLESTKSIZE ?= 1024
KBUILD_CPPFLAGS += \
	-Iinclude/rtos/rtx5/
KBUILD_CPPFLAGS += -D__RTX_CPU_STATISTICS__=1
#KBUILD_CPPFLAGS += -DTASK_HUNG_CHECK_ENABLED=1
else ifeq ($(KERNEL),RTT)
KBUILD_CPPFLAGS += -DOS_MAIN_SUPPORT=1
KBUILD_CPPFLAGS += -Irtos/rt_thread/CMSIS/RTOS2/RT_Thread/src/		\
					-Irtos/rt_thread/CMSIS/CMSIS_5/CMSIS/Core/Include/ \
					-Irtos/rt_thread/CMSIS/CMSIS_5/CMSIS/RTOS2/Include/ \
					-Irtos/rt_thread/CMSIS/CMSIS_5/CMSIS/RTOS2/Template/ \
					-Irtos/rt_thread/bsp/best200x_$(CPU)/	\
					-Irtos/rt_thread/include/				\
					-Irtos/rt_thread/components/finsh/      \
					-Irtos/rt_thread/

ifeq ($(CPU),a7)
KBUILD_CPPFLAGS += -Irtos/rt_thread/libcpu/arm/cortex-a/
KBUILD_CPPFLAGS += -Wno-stringop-truncation -Wno-stringop-overflow
else
KBUILD_CPPFLAGS += -Irtos/rt_thread/libcpu/arm/cortex-m33/
endif
cflags-y += -Wno-error=implicit-function-declaration -Wno-error=implicit-function-declaration
else ifeq ($(KERNEL),RHINO)
KBUILD_CPPFLAGS += \
    -Irtos/rhino \
    -Irtos/rhino/include \
    -Irtos/rhino/cmsis
ifneq ($(CPU),a7)
KBUILD_CPPFLAGS += \
    -Irtos/rhino/arch/arm/armv7m/include
else
KBUILD_CPPFLAGS += \
    -Irtos/rhino/arch/arm/armv7a/include \
    -Irtos/rhino/smp
endif
else ifeq ($(KERNEL),FREERTOS)
KBUILD_CPPFLAGS += \
    -Iinclude/rtos/freertos/
ifeq ($(CPU),m33)
KBUILD_CPPFLAGS += -Iinclude/rtos/freertos/ARM_CM33/
else
KBUILD_CPPFLAGS += -Iinclude/rtos/freertos/ARM_CM4F/
endif
#if kernel is LITEOS_M and LITEOS_A
else ifneq (, $(filter $(KERNEL),LITEOS_M LITEOS_A))
KBUILD_CPPFLAGS += -DCMSIS_OS_VER=2
RTOS_NAME := $(shell echo $(KERNEL) | tr A-Z a-z)
KBUILD_CPPFLAGS += -Iinclude/rtos/liteos/$(RTOS_NAME)/kal/cmsis
# KBUILD_CPPFLAGS += -Iinclude/rtos/liteos/$(RTOS_NAME)/kal/posix/include
ifeq ($(KERNEL),LITEOS_A)
CPPFLAGS_${LDS_FILE} += -DOS_HEAP_SIZE=$(OS_HEAP_SIZE)
endif
endif #rtx

ifeq ($(OSTICK_USE_FAST_TIMER), 1)
KBUILD_CPPFLAGS += -DOSTICK_USE_FAST_TIMER
endif

ifeq ($(TWS),1)
OS_TASKCNT ?= 12
OS_SCHEDULERSTKSIZE ?= 768
OS_IDLESTKSIZE ?= 512
else
OS_TASKCNT ?= 20
OS_SCHEDULERSTKSIZE ?= 512
OS_IDLESTKSIZE ?= 256
endif

ifeq ($(CPU),m33)
OS_CLOCK_NOMINAL ?= 16000
else
OS_CLOCK_NOMINAL ?= 32000
endif
OS_FIFOSZ ?= 24

export OS_TASKCNT
export OS_SCHEDULERSTKSIZE
export OS_IDLESTKSIZE
export OS_CLOCK_NOMINAL
export OS_FIFOSZ

ifeq ($(OS_THREAD_TIMING_STATISTICS_ENABLE),1)
export OS_THREAD_TIMING_STATISTICS_ENABLE
KBUILD_CPPFLAGS += -DOS_THREAD_TIMING_STATISTICS_ENABLE
KBUILD_CPPFLAGS += -DOS_THREAD_TIMING_STATISTICS_PEROID_MS=6000
endif

# shall set WATCHER_DOG ?= 0 when enable this feature
ifeq ($(SPECIFIC_FREQ_POWER_CONSUMPTION_MEASUREMENT_ENABLE),1)
export SPECIFIC_FREQ_POWER_CONSUMPTION_MEASUREMENT_ENABLE
KBUILD_CPPFLAGS += -DSPECIFIC_FREQ_POWER_CONSUMPTION_MEASUREMENT_ENABLE
IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 1
endif

endif # RTOS

# -------------------------------------------
# MBED library
# -------------------------------------------

export MBED

ifeq ($(MBED),1)

core-y += mbed/

KBUILD_CPPFLAGS += -DMBED

KBUILD_CPPFLAGS += \
	-Imbed/api \
	-Imbed/common \

endif

# -------------------------------------------
# DEBUG functions
# -------------------------------------------

export DEBUG

ifeq ($(CHIP),best1400)
OPT_LEVEL ?= s
endif

ifneq ($(OPT_LEVEL),)
KBUILD_CFLAGS	+= -O$(OPT_LEVEL)
else
KBUILD_CFLAGS	+= -O2
endif

ifeq ($(NOSTD),1)
export NO_BUF_OVERFLOW_CHECK := 1
endif

ifeq ($(DEBUG),1)

KBUILD_CPPFLAGS	+= -DDEBUG

ifneq ($(NO_BUF_OVERFLOW_CHECK),1)
KBUILD_CFLAGS  	+= -fstack-protector-strong
endif

else # !DEBUG

KBUILD_CPPFLAGS	+= -DNDEBUG

REL_TRACE_ENABLE ?= 1
ifeq ($(REL_TRACE_ENABLE),1)
KBUILD_CPPFLAGS	+= -DREL_TRACE_ENABLE
endif

endif # !DEBUG

ifeq ($(NO_CHK_TRC_FMT),1)
KBUILD_CPPFLAGS	+= -DNO_CHK_TRC_FMT
else
# Typedef int32_t to int, and typedef uint32_t to unsigned int
KBUILD_CPPFLAGS	+= -U__INT32_TYPE__ -D__INT32_TYPE__=int -U__UINT32_TYPE__
endif

ifeq ($(MERGE_CONST),1)
ifeq ($(TOOLCHAIN),armclang)
$(error MERGE_CONST is not supported in $(TOOLCHAIN))
else
KBUILD_CPPFLAGS += -fmerge-constants -fmerge-all-constants
endif
endif

ifeq ($(CORE_DUMP),1)
export CORE_DUMP
core-y += utils/crash_catcher/ utils/xyzmodem/
endif

# -------------------------------------------
# SIMU functions
# -------------------------------------------

export SIMU

ifeq ($(SIMU),1)

KBUILD_CPPFLAGS += -DSIMU

ifeq ($(ROM_SRAM_TEXT_SIMU),1)
KBUILD_CPPFLAGS += -DROM_SRAM_TEXT_SIMU
endif

endif

# -------------------------------------------
# FPGA functions
# -------------------------------------------

export FPGA

ifeq ($(FPGA),1)

KBUILD_CPPFLAGS += -DFPGA

endif

# -------------------------------------------
# ROM_BUILD functions
# -------------------------------------------

export ROM_BUILD

ifeq ($(ROM_BUILD),1)

KBUILD_CPPFLAGS += -DROM_BUILD

endif

# -------------------------------------------
# NANDFLASH_BUILD functions
# -------------------------------------------

export NANDFLASH_BUILD

ifeq ($(NANDFLASH_BUILD),1)

KBUILD_CPPFLAGS += -DNANDFLASH_BUILD

endif

# Limit the length of REVISION_INFO if ROM_BUILD or using rom.lds
ifneq ($(filter 1,$(ROM_BUILD))$(filter rom.lds,$(LDS_FILE)),)
ifeq ($(CHIP),best1000)
REVISION_INFO := x
else
REVISION_INFO := $(GIT_REVISION)
endif
endif

# -------------------------------------------
# PROGRAMMER functions
# -------------------------------------------

export PROGRAMMER

ifeq ($(PROGRAMMER),1)

KBUILD_CPPFLAGS += -DPROGRAMMER

ifeq ($(NO_SIMPLE_TASK_SWITCH),1)
KBUILD_CPPFLAGS += -DNO_SIMPLE_TASK_SWITCH
endif

endif

# -------------------------------------------
# ROM_UTILS functions
# -------------------------------------------

export ROM_UTILS_ON ?= 0
ifeq ($(ROM_UTILS_ON),1)
KBUILD_CPPFLAGS += -DROM_UTILS_ON
core-y += utils/rom_utils/
endif

# -------------------------------------------
# Predefined common features
# -------------------------------------------

ifeq ($(OSC_26M_X4_AUD2BB),1)
export OSC_26M_X4_AUD2BB
export ANA_26M_X4_ENABLE ?= 1
export FLASH_LOW_SPEED ?= 0
endif

ifeq ($(USB_AUDIO_APP),1)
export CODEC_HIGH_QUALITY ?= 1
endif

ifeq ($(BT_ANC),1)
export ANC_APP := 1
endif
ifeq ($(ANC_APP),1)
export CODEC_HIGH_QUALITY ?= 1
endif

ifeq ($(CHIP),best1000)
export AUDIO_OUTPUT_DIFF ?= 1
AUDIO_OUTPUT_DC_CALIB ?= $(AUDIO_OUTPUT_DIFF)
export AUDIO_OUTPUT_SMALL_GAIN_ATTN ?= 1
export AUDIO_OUTPUT_SW_GAIN ?= 1
export ANC_L_R_MISALIGN_WORKAROUND ?= 1
else ifeq ($(CHIP),best2000)
ifeq ($(CODEC_HIGH_QUALITY),1)
export VCODEC_VOLT ?= 2.5V
else
export VCODEC_VOLT ?= 1.6V
endif
ifeq ($(VCODEC_VOLT),2.5V)
AUDIO_OUTPUT_DC_CALIB ?= 0
AUDIO_OUTPUT_DC_CALIB_ANA ?= 1
else
AUDIO_OUTPUT_DC_CALIB ?= 1
AUDIO_OUTPUT_DC_CALIB_ANA ?= 0
endif
ifneq ($(AUDIO_OUTPUT_DIFF),1)
# Class-G module still needs improving
#DAC_CLASSG_ENABLE ?= 1
endif
else ifeq ($(CHIP),best2001)
export VCODEC_VOLT ?= 1.8V
AUDIO_OUTPUT_DC_CALIB ?= 0
AUDIO_OUTPUT_DC_CALIB_ANA ?= 1
else ifneq ($(filter best3001 best3003 best3005,$(CHIP)),)
export VCODEC_VOLT ?= 2.5V
AUDIO_OUTPUT_DC_CALIB ?= 1
AUDIO_OUTPUT_DC_CALIB_ANA ?= 0
else
AUDIO_OUTPUT_DC_CALIB ?= 0
AUDIO_OUTPUT_DC_CALIB_ANA ?= 1
endif

ifeq ($(AUDIO_OUTPUT_DC_CALIB)-$(AUDIO_OUTPUT_DC_CALIB_ANA),1-1)
$(error AUDIO_OUTPUT_DC_CALIB and AUDIO_OUTPUT_DC_CALIB_ANA cannot be enabled at the same time)
endif
export AUDIO_OUTPUT_DC_CALIB
export AUDIO_OUTPUT_DC_CALIB_ANA

ifeq ($(PLAYBACK_USE_I2S),1)
export PLAYBACK_USE_I2S
KBUILD_CPPFLAGS += -DPLAYBACK_USE_I2S
export AF_DEVICE_I2S := 1
ifeq ($(CHIP_HAS_I2S_TDM_TRIGGER),1)
KBUILD_CPPFLAGS += -DHW_I2S_TDM_TRIGGER
endif
endif

ifeq ($(CHIP),best1400)
export AUDIO_RESAMPLE ?= 1
export PMU_IRQ_UNIFIED ?= 1
else ifeq ($(CHIP),best2001)
export AUDIO_RESAMPLE ?= 1
else ifeq ($(CHIP),best2003)
export AUDIO_RESAMPLE := 1
else
export AUDIO_RESAMPLE ?= 0
endif

ifeq ($(AUDIO_RESAMPLE),1)
ifeq ($(CHIP),best1000)
export SW_PLAYBACK_RESAMPLE ?= 1
export SW_CAPTURE_RESAMPLE ?= 1
export NO_SCO_RESAMPLE ?= 1
endif # CHIP is best1000
ifeq ($(CHIP),best2000)
export SW_CAPTURE_RESAMPLE ?= 1
export SW_SCO_RESAMPLE ?= 1
export NO_SCO_RESAMPLE ?= 0
endif # CHIP is best2000
ifeq ($(BT_ANC),1)
ifeq ($(NO_SCO_RESAMPLE),1)
$(error BT_ANC and NO_SCO_RESAMPLE cannot be enabled at the same time)
endif
endif # BT_ANC
endif # AUDIO_RESAMPLE

ifeq ($(SW_IIR_EQ_PROCESS),1)
export SW_IIR_EQ_PROCESS
export A2DP_EQ_24BIT = 1
endif

ifeq ($(HW_DC_FILTER_WITH_IIR),1)
export HW_DC_FILTER_WITH_IIR
KBUILD_CPPFLAGS += -DHW_DC_FILTER_WITH_IIR
export HW_FILTER_CODEC_IIR ?= 1
endif

ifeq ($(USB_AUDIO_APP),1)
export ANDROID_ACCESSORY_SPEC ?= 1
export FIXED_CODEC_ADC_VOL ?= 0

ifneq ($(BTUSB_AUDIO_MODE),1)
NO_PWRKEY ?= 1
NO_GROUPKEY ?= 1
endif
endif

ifneq ($(CHIP),best1000)
ifneq ($(CHIP)-$(TWS),best2000-1)
# For bt
export A2DP_EQ_24BIT ?= 1
# For usb audio
ifeq ($(APP_USB_A2DP_SOURCE),1)
export AUDIO_PLAYBACK_24BIT ?= 0
else
export AUDIO_PLAYBACK_24BIT ?= 1
endif
endif
endif

ifeq ($(PLAYBACK_FORCE_48K),1)
export PLAYBACK_FORCE_48K
endif

ifeq ($(ANC_ASSIST_ENABLED),1)
KBUILD_CPPFLAGS += -DANC_ASSIST_ENABLED
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
export SPEECH_TX_24BIT := 1
export AUDIO_OUTPUT_SW_GAIN := 1
ifeq ($(CHIP),best2300p)
export ANC_ASSIST_USE_INT_CODEC := 1
export PLAYBACK_FORCE_48K := 1
export SPEECH_RX_24BIT := 1
export AF_STREAM_ID_0_PLAYBACK_FADEOUT := 1
endif

ifeq ($(VOICE_ASSIST_WD_ENABLED),1)
export VOICE_ASSIST_WD_ENABLED
export ANC_ASSIST_PILOT_TONE_ALWAYS_ON := 1
endif

ifeq ($(VOICE_ASSIST_PILOT_ANC_ENABLED),1)
export VOICE_ASSIST_PILOT_ANC_ENABLED
export ANC_ASSIST_PILOT_TONE_ALWAYS_ON := 1
endif
endif

ifeq ($(AUDIO_ANC_FB_ADJ_MC),1)
KBUILD_CPPFLAGS += -DAUDIO_ANC_FB_ADJ_MC
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

export ULTRA_LOW_POWER ?= 0
ifeq ($(ULTRA_LOW_POWER),1)
export FLASH_LOW_SPEED ?= 1
export PSRAM_LOW_SPEED ?= 1
endif

ifeq ($(CHIP),best2000)
ifeq ($(USB_HIGH_SPEED),1)
export AUDIO_USE_BBPLL ?= 1
endif
ifeq ($(AUDIO_USE_BBPLL),1)
ifeq ($(MCU_HIGH_PERFORMANCE_MODE),1)
$(error MCU_HIGH_PERFORMANCE_MODE conflicts with AUDIO_USE_BBPLL)
endif
else # !AUDIO_USE_BBPLL
ifeq ($(USB_HIGH_SPEED),1)
$(error AUDIO_USE_BBPLL must be used with USB_HIGH_SPEED)
endif
endif # !AUDIO_USE_BBPLL
endif # best2000

ifeq ($(FAST_TIMER_COMPENSATE),1)
export CALIB_SLOW_TIMER := 1
export TIMER_USE_FPU := 1
endif

### voice compression feature switch
export VOC_ENCODE_ENABLE ?= 0

# -------------------------------------------
# BT features
# -------------------------------------------

ifneq ($(filter apps/ tests/speech_test/ tests/ota_boot/, $(core-y)),)
export BT_APP ?= 1
FULL_APP_PROJECT ?= 1
endif

ifeq ($(BT_APP),1)

export BT_IF_INCLUDES ?=
export BT_PROFILES_INCLUDES ?=

export INTERSYS_NO_THREAD ?= 0

export INTERSYS_DEBUG ?= 1
ifeq ($(INTERSYS_DEBUG),1)
	KBUILD_CPPFLAGS += -DINTERSYS_DEBUG=1
endif

export BT_DEBUG_TPORTS ?= 0
ifneq ($(BT_DEBUG_TPORTS),0)
	KBUILD_CPPFLAGS += -D__BT_DEBUG_TPORTS__
endif

export SNOOP_DATA_EXCHANGE_VIA_BLE ?= 0
ifeq ($(SNOOP_DATA_EXCHANGE_VIA_BLE),1)
	KBUILD_CPPFLAGS += -DSNOOP_DATA_EXCHANGE_VIA_BLE
endif

export SYNC_BT_CTLR_PROFILE ?= 0
ifeq ($(SYNC_BT_CTLR_PROFILE),1)
	KBUILD_CPPFLAGS += -DSYNC_BT_CTLR_PROFILE
endif

export PROFILE_DEBUG ?= 0
ifeq ($(PROFILE_DEBUG),1)
	KBUILD_CPPFLAGS += -DXA_DEBUG=1
endif

BT_IF_INCLUDES += \
	-Iservices/bt_if_enhanced/inc
BT_PROFILES_INCLUDES += \
	-Iservices/bt_profiles_enhanced/inc

#KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__ -D__A2DP_AVDTP_DR__
#KBUILD_CPPFLAGS += -D__A2DP_AVDTP_DR__

ifeq ($(A2DP_AVDTP_CP),1)
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifneq ($(filter-out 2M 3M,$(BT_RF_PREFER)),)
$(error Invalid BT_RF_PREFER=$(BT_RF_PREFER))
endif
ifneq ($(BT_RF_PREFER),)
RF_PREFER := $(subst .,P,$(BT_RF_PREFER))
KBUILD_CPPFLAGS += -D__$(RF_PREFER)_PACK__
endif

export HIGH_EFFICIENCY_TX_PWR_CTRL ?= 0
ifeq ($(HIGH_EFFICIENCY_TX_PWR_CTRL),1)
KBUILD_CPPFLAGS += -DHIGH_EFFICIENCY_TX_PWR_CTRL
endif # ifeq ($(HIGH_EFFICIENCY_TX_PWR_CTRL),1)

export IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING ?= 0
ifeq ($(IS_USE_MIXING_FRQ_FOR_A2DP_STREAMING),1)
KBUILD_CPPFLAGS += -DIS_USE_MIXING_FRQ_FOR_A2DP_STREAMING
endif

export AUDIO_SCO_BTPCM_CHANNEL ?= 1
ifeq ($(AUDIO_SCO_BTPCM_CHANNEL),1)
KBUILD_CPPFLAGS += -D_SCO_BTPCM_CHANNEL_
endif

export BT_ONE_BRING_TWO ?= 0
ifeq ($(BT_ONE_BRING_TWO),1)
KBUILD_CPPFLAGS += -D__BT_ONE_BRING_TWO__
endif

export BT_PAUSE_BG_A2DP ?= 0
ifeq ($(BT_PAUSE_BG_A2DP),1)
KBUILD_CPPFLAGS += -DBT_PAUSE_BG_A2DP
endif

export BT_CLOSE_BG_A2DP ?= 0
ifeq ($(BT_CLOSE_BG_A2DP),1)
KBUILD_CPPFLAGS += -DBT_CLOSE_BG_A2DP
endif

export BT_DONT_AUTO_PLAY_BG_A2DP ?= 0
ifeq ($(BT_DONT_AUTO_PLAY_BG_A2DP),1)
KBUILD_CPPFLAGS += -DBT_DONT_AUTO_PLAY_BG_A2DP
endif

export BT_BLOCK_2ND_SCO_BEFORE_CALL_ACTIVE ?= 0
ifeq ($(BT_BLOCK_2ND_SCO_BEFORE_CALL_ACTIVE),1)
KBUILD_CPPFLAGS += -DBT_BLOCK_2ND_SCO_BEFORE_CALL_ACTIVE
endif

export A2DP_PROMPT_PLAY_ONLY_AVRCP_PLAY_RECEIVED ?= 1
ifeq ($(A2DP_PROMPT_PLAY_ONLY_AVRCP_PLAY_RECEIVED),1)
KBUILD_CPPFLAGS += -DA2DP_PROMPT_PLAY_ONLY_AVRCP_PLAY_RECEIVED
endif

export A2DP_DELAY_PROMPT_PLAY ?= 0
ifeq ($(A2DP_DELAY_PROMPT_PLAY),1)
KBUILD_CPPFLAGS += -DA2DP_DELAY_PROMPT_PLAY
endif

export BT_MUTE_NEW_A2DP ?= 0
ifeq ($(BT_MUTE_NEW_A2DP),1)
KBUILD_CPPFLAGS += -DBT_MUTE_NEW_A2DP
endif

export BT_PAUSE_NEW_A2DP ?= 0
ifeq ($(BT_PAUSE_NEW_A2DP),1)
KBUILD_CPPFLAGS += -DBT_PAUSE_NEW_A2DP
endif

export BT_CLOSE_NEW_A2DP ?= 0
ifeq ($(BT_CLOSE_NEW_A2DP),1)
KBUILD_CPPFLAGS += -DBT_CLOSE_NEW_A2DP
endif

export BT_KEEP_ONE_STREAM_CLOSE_CONNECTED_A2DP ?= 0
ifeq ($(BT_KEEP_ONE_STREAM_CLOSE_CONNECTED_A2DP),1)
KBUILD_CPPFLAGS += -DBT_KEEP_ONE_STREAM_CLOSE_CONNECTED_A2DP
endif

export BT_DONT_AUTO_REPORT_DELAY_REPORT ?= 0
ifeq ($(BT_DONT_AUTO_REPORT_DELAY_REPORT),1)
KBUILD_CPPFLAGS += -DBT_DONT_AUTO_REPORT_DELAY_REPORT
endif

export BT_HFP_DONT_SUPPORT_CLI_FEATURE ?= 0
ifeq ($(BT_HFP_DONT_SUPPORT_CLI_FEATURE),1)
KBUILD_CPPFLAGS += -DBT_HFP_DONT_SUPPORT_CLI_FEATURE
endif

export BT_HFP_DONT_SUPPORT_ENHANCED_CALL_FEATURE ?= 0
ifeq ($(BT_HFP_DONT_SUPPORT_ENHANCED_CALL_FEATURE),1)
KBUILD_CPPFLAGS += -DBT_HFP_DONT_SUPPORT_ENHANCED_CALL_FEATURE
endif

export BT_HFP_SUPPORT_HF_INDICATORS_FEATURE ?= 0
ifeq ($(BT_HFP_SUPPORT_HF_INDICATORS_FEATURE),1)
KBUILD_CPPFLAGS += -DBT_HFP_SUPPORT_HF_INDICATORS_FEATURE
endif

export BT_HFP_DONT_SUPPORT_APPLE_HF_AT_COMMAND ?= 0
ifeq ($(BT_HFP_DONT_SUPPORT_APPLE_HF_AT_COMMAND),1)
KBUILD_CPPFLAGS += -DBT_HFP_DONT_SUPPORT_APPLE_HF_AT_COMMAND
endif

export BT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH ?= 1
ifeq ($(BT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH),1)
KBUILD_CPPFLAGS += -DBT_DONT_PLAY_MUTE_WHEN_A2DP_STUCK_PATCH
endif

export SPP_SERVICE_NUM ?= 5
KBUILD_CPPFLAGS += -DSPP_SERVICE_NUM=$(SPP_SERVICE_NUM)

export BT_DISC_ACL_AFTER_AUTH_KEY_MISSING ?= 0
ifeq ($(BT_DISC_ACL_AFTER_AUTH_KEY_MISSING),1)
KBUILD_CPPFLAGS += -DBT_DISC_ACL_AFTER_AUTH_KEY_MISSING
endif

export USE_PAGE_SCAN_REPETITION_MODE_R1 ?= 0
ifeq ($(USE_PAGE_SCAN_REPETITION_MODE_R1),1)
KBUILD_CPPFLAGS += -DUSE_PAGE_SCAN_REPETITION_MODE_R1
endif

export A2DP_PLAYER_USE_BT_TRIGGER ?= 1
ifeq ($(A2DP_PLAYER_USE_BT_TRIGGER),1)
KBUILD_CPPFLAGS += -D__A2DP_PLAYER_USE_BT_TRIGGER__
endif

export BT_SELECT_PROF_DEVICE_ID ?= 0
ifeq ($(BT_ONE_BRING_TWO),1)
ifeq ($(BT_SELECT_PROF_DEVICE_ID),1)
KBUILD_CPPFLAGS += -D__BT_SELECT_PROF_DEVICE_ID__
endif
endif

export SBC_SELECT_CHANNEL_SUPPORT ?= 1
ifeq ($(SBC_SELECT_CHANNEL_SUPPORT),1)
KBUILD_CPPFLAGS += -DSBC_SELECT_CHANNEL_SUPPORT
endif

export SBC_FUNC_IN_ROM ?= 0
ifeq ($(SBC_FUNC_IN_ROM),1)

KBUILD_CPPFLAGS += -D__SBC_FUNC_IN_ROM__

ifeq ($(CHIP),best2000)
UNALIGNED_ACCESS ?= 1
KBUILD_CPPFLAGS += -D__SBC_FUNC_IN_ROM_VBEST2000_ONLYSBC__
KBUILD_CPPFLAGS += -D__SBC_FUNC_IN_ROM_VBEST2000__
endif
endif

export BT_HID_DEVICE ?= 0
ifeq ($(BT_HID_DEVICE),1)
KBUILD_CPPFLAGS += -DBT_HID_DEVICE
endif

export BT_PBAP_SUPPORT ?= 0
ifeq ($(BT_PBAP_SUPPORT),1)
KBUILD_CPPFLAGS += -DBT_PBAP_SUPPORT
KBUILD_CPPFLAGS += -DBT_OBEX_SUPPORT
endif

export BT_MAP_SUPPORT ?= 0
ifeq ($(BT_MAP_SUPPORT),1)
KBUILD_CPPFLAGS += -DBT_MAP_SUPPORT
KBUILD_CPPFLAGS += -DBT_OBEX_SUPPORT
endif

export BT_ALWAYS_IN_DISCOVERABLE_MODE ?= 0
ifeq ($(BT_ALWAYS_IN_DISCOVERABLE_MODE),1)
KBUILD_CPPFLAGS += -DBT_ALWAYS_IN_DISCOVERABLE_MODE
endif

export HFP_1_6_ENABLE ?= 1
ifeq ($(HFP_1_6_ENABLE),1)
KBUILD_CPPFLAGS += -DHFP_1_6_ENABLE
endif

# Fix codec and vqe sample rate
ifeq ($(SPEECH_BONE_SENSOR),1)
export SPEECH_CODEC_FIXED_SAMPLE_RATE := 16000
export SPEECH_VQE_FIXED_SAMPLE_RATE := 16000
endif

ifeq ($(ANC_ASSIST_ENABLED),1)
export SPEECH_CODEC_FIXED_SAMPLE_RATE := 16000
export SPEECH_VQE_FIXED_SAMPLE_RATE := 16000
endif

ifeq ($(AUDIO_ADAPTIVE_IIR_EQ),1)
export AUDIO_ADAPTIVE_IIR_EQ := 1
export HW_DAC_IIR_EQ_PROCESS := 1
endif

ifeq ($(AUDIO_ADAPTIVE_FIR_EQ),1)
export AUDIO_ADAPTIVE_FIR_EQ := 1
export HW_FIR_EQ_PROCESS := 1
endif

export SPEECH_CODEC_FIXED_SAMPLE_RATE ?= 0
ifneq ($(filter 8000 16000 48000,$(SPEECH_CODEC_FIXED_SAMPLE_RATE)),)
KBUILD_CPPFLAGS += -DSPEECH_CODEC_FIXED_SAMPLE_RATE=$(SPEECH_CODEC_FIXED_SAMPLE_RATE)
#export DSP_LIB ?= 1
endif

export SPEECH_VQE_FIXED_SAMPLE_RATE ?= 0
ifneq ($(filter 8000 16000,$(SPEECH_VQE_FIXED_SAMPLE_RATE)),)
#export DSP_LIB ?= 1
endif

export A2DP_AAC_ON ?= 0
ifeq ($(A2DP_AAC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_AAC_ON
KBUILD_CPPFLAGS += -D__ACC_FRAGMENT_COMPATIBLE__
endif


export FDKAAC_VERSION ?= fdkaac_codec_2_0_1

ifneq ($(FDKAAC_VERSION),)
KBUILD_CPPFLAGS += -DFDKAAC_VERSION=$(FDKAAC_VERSION)
endif

export A2DP_LHDC_ON ?= 0
ifeq ($(A2DP_LHDC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_LHDC_ON
export A2DP_LHDC_V3 ?= 0
ifeq ($(A2DP_LHDC_V3),1)
KBUILD_CPPFLAGS += -DA2DP_LHDC_V3
endif
export A2DP_LHDC_LARC ?= 0
ifeq ($(A2DP_LHDC_LARC),1)
KBUILD_CPPFLAGS += -DA2DP_LHDC_LARC
endif
core-y += thirdparty/audio_codec_lib/liblhdc-dec/
endif

export A2DP_SCALABLE_ON ?= 0
ifeq ($(A2DP_SCALABLE_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SCALABLE_ON
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_KISS_FFT
#KBUILD_CPPFLAGS += -DA2DP_SCALABLE_UHQ_SUPPORT
core-y += thirdparty/audio_codec_lib/scalable/
endif

export A2DP_LDAC_ON ?= 0
ifeq ($(A2DP_LDAC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_LDAC_ON
core-y += thirdparty/audio_codec_lib/ldac/
endif

export A2DP_LC3_ON ?= 0
ifeq ($(A2DP_LC3_ON),1)
KBUILD_CPPFLAGS += -DA2DP_LC3_ON
endif

export A2DP_SBC_PLC_ENABLED ?= 0

export A2DP_CP_ACCEL ?= 0
ifeq ($(A2DP_CP_ACCEL),1)
KBUILD_CPPFLAGS += -DA2DP_CP_ACCEL
endif

export SCO_CP_ACCEL ?= 0
ifeq ($(SCO_CP_ACCEL),1)
KBUILD_CPPFLAGS += -DSCO_CP_ACCEL
# spx fft will share buffer which is not fit for dual cores.
KBUILD_CPPFLAGS += -DUSE_CMSIS_F32_FFT
endif

export SCO_TRACE_CP_ACCEL ?= 0
ifeq ($(SCO_TRACE_CP_ACCEL),1)
KBUILD_CPPFLAGS += -DSCO_TRACE_CP_ACCEL
endif

ifeq ($(BT_XTAL_SYNC),1)
KBUILD_CPPFLAGS += -DBT_XTAL_SYNC_NEW_METHOD
KBUILD_CPPFLAGS += -DFIXED_BIT_OFFSET_TARGET
endif

ifeq ($(FPGA_A2DP_SINK),1)
KBUILD_CPPFLAGS += -DFPGA_A2DP_SINK
endif

ifeq ($(HSP_ENABLE),1)
KBUILD_CPPFLAGS += -D__HSP_ENABLE__
endif

export TX_RX_PCM_MASK ?= 0
ifeq ($(TX_RX_PCM_MASK),1)
KBUILD_CPPFLAGS += -DTX_RX_PCM_MASK
endif

export PCM_PRIVATE_DATA_FLAG ?= 0
ifeq ($(PCM_PRIVATE_DATA_FLAG),1)
KBUILD_CPPFLAGS += -DPCM_PRIVATE_DATA_FLAG
endif

export PCM_FAST_MODE ?= 0
ifeq ($(PCM_FAST_MODE),1)
KBUILD_CPPFLAGS += -DPCM_FAST_MODE
endif

export LOW_DELAY_SCO ?= 0
ifeq ($(LOW_DELAY_SCO),1)
KBUILD_CPPFLAGS += -DLOW_DELAY_SCO
endif

export CVSD_BYPASS ?= 0
ifeq ($(CVSD_BYPASS),1)
KBUILD_CPPFLAGS += -DCVSD_BYPASS
endif

export SCO_FORCE_CVSD ?= 0
ifeq ($(SCO_FORCE_CVSD),1)
KBUILD_CPPFLAGS += -DSCO_FORCE_CVSD
endif

export SCO_DMA_SNAPSHOT ?= 0
ifeq ($(CHIP_HAS_SCO_DMA_SNAPSHOT),1)
export SCO_DMA_SNAPSHOT := 1
KBUILD_CPPFLAGS += -DSCO_DMA_SNAPSHOT
endif

ifeq ($(CHIP_ROM_UTILS_VER),)
$(warning "CHIP_ROM_UTILS_VER must be defined, CHIP after BEST1501")
$(warning "use \"export CHIP_ROM_UTILS_VER := 2\", other use \"export CHIP_ROM_UTILS_VER := 1\"")
export CHIP_ROM_UTILS_VER := 1
endif
KBUILD_CPPFLAGS += -DROM_UTILS_VER=$(CHIP_ROM_UTILS_VER)

export SCO_OPTIMIZE_FOR_RAM ?= 0
ifeq ($(SCO_OPTIMIZE_FOR_RAM),1)
KBUILD_CPPFLAGS += -DSCO_OPTIMIZE_FOR_RAM
endif

export SW_IIR_PROMPT_EQ_PROCESS ?= 0
ifeq ($(SW_IIR_PROMPT_EQ_PROCESS),1)
ifeq ($(SW_IIR_EQ_PROCESS),1)
$(error SW_IIR_PROMPT_EQ_PROCESS and SW_IIR_EQ_PROCESS cannot be enabled at the same time)
endif
endif

export AAC_TEXT_PARTIAL_IN_FLASH ?= 0
ifeq ($(AAC_TEXT_PARTIAL_IN_FLASH),1)
KBUILD_CPPFLAGS += -DAAC_TEXT_PARTIAL_IN_FLASH
endif

export IS_BES_BATTERY_MANAGER_ENABLED ?= 1
ifeq ($(IS_BES_BATTERY_MANAGER_ENABLED),1)
KBUILD_CPPFLAGS += -DIS_BES_BATTERY_MANAGER_ENABLED
endif

ifeq ($(SUPPORT_BATTERY_REPORT),1)
KBUILD_CPPFLAGS += -DSUPPORT_BATTERY_REPORT
endif

ifeq ($(SUPPORT_HF_INDICATORS),1)
KBUILD_CPPFLAGS += -DSUPPORT_HF_INDICATORS
endif

ifeq ($(SUPPORT_SIRI),1)
KBUILD_CPPFLAGS += -DSUPPORT_SIRI
endif

export BQB_PROFILE_TEST ?= 0
ifeq ($(BQB_PROFILE_TEST),1)
KBUILD_CPPFLAGS += -D__BQB_PROFILE_TEST__
endif

export AUDIO_SPECTRUM ?= 0
ifeq ($(AUDIO_SPECTRUM),1)
KBUILD_CPPFLAGS += -D__AUDIO_SPECTRUM__
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_KISS_FFT
endif

ifeq ($(INTERCONNECTION),1)
export INTERCONNECTION
KBUILD_CPPFLAGS += -D__INTERCONNECTION__
endif

export INTERACTION ?= 0
ifeq ($(INTERACTION),1)
KBUILD_CPPFLAGS += -D__INTERACTION__
endif

export INTERACTION_FASTPAIR ?= 0
ifeq ($(INTERACTION_FASTPAIR),1)
KBUILD_CPPFLAGS += -D__INTERACTION_FASTPAIR__
KBUILD_CPPFLAGS += -D__INTERACTION_CUSTOMER_AT_COMMAND__
endif

export TWS_PROMPT_SYNC ?= 0
ifeq ($(TWS_PROMPT_SYNC), 1)
export MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED ?= 1
KBUILD_CPPFLAGS += -DTWS_PROMPT_SYNC
endif

export AUDIO_PROMPT_USE_DAC2_ENABLED ?= 0
ifeq ($(CHIP), best1501)
export AUDIO_PROMPT_USE_DAC2_ENABLED := 1
endif
ifeq ($(ANC_ASSIST_PILOT_TONE_ALWAYS_ON),1)
export AUDIO_PROMPT_USE_DAC2_ENABLED := 0
endif

ifeq ($(AUDIO_PROMPT_USE_DAC2_ENABLED),1)
export MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED := 0
export AUDIO_OUTPUT_DAC2 := 1
endif

ifeq ($(MIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED),1)
KBUILD_CPPFLAGS += -DMIX_AUDIO_PROMPT_WITH_A2DP_MEDIA_ENABLED
export RESAMPLE_ANY_SAMPLE_RATE ?= 1
export AUDIO_OUTPUT_SW_GAIN := 1
endif

export MEDIA_PLAY_24BIT ?= 1

ifeq ($(LBRT),1)
export LBRT
KBUILD_CPPFLAGS += -DLBRT
endif

ifeq ($(IBRT),1)
export IBRT
KBUILD_CPPFLAGS += -DIBRT
KBUILD_CPPFLAGS += -DIBRT_BLOCKED
KBUILD_CPPFLAGS += -DIBRT_NOT_USE
KBUILD_CPPFLAGS += -D__A2DP_AUDIO_SYNC_FIX_DIFF_NOPID__

export IBRT_UI_V1 ?= 0
ifeq ($(IBRT_UI_V1),1)
KBUILD_CPPFLAGS += -DIBRT_UI_V1
else
export TRACE_GLOBAL_TAG ?= 1
endif
endif

export IBRT_TESTMODE ?= 0
ifeq ($(IBRT_TESTMODE),1)
KBUILD_CPPFLAGS += -D__IBRT_IBRT_TESTMODE__
endif

ifeq ($(IBRT),1)
TWS_SYSTEM_ENABLED := 1
endif
ifeq ($(TWS_SYSTEM_ENABLED),1)
export TWS_SYSTEM_ENABLED
KBUILD_CPPFLAGS += -DTWS_SYSTEM_ENABLED
endif

# IBRT_RIGHT_MASTER==1 means right bud is master, otherwise left bud is master
export IBRT_RIGHT_MASTER ?= 1
ifeq ($(IBRT_RIGHT_MASTER),1)
KBUILD_CPPFLAGS += -DIBRT_RIGHT_MASTER
endif

export BES_AUD ?= 0
ifeq ($(BES_AUD),1)
KBUILD_CPPFLAGS += -DBES_AUD
endif

export IBRT_SEARCH_UI ?= 0
ifeq ($(IBRT_SEARCH_UI),1)
KBUILD_CPPFLAGS += -DIBRT_SEARCH_UI
endif

export SEARCH_UI_COMPATIBLE_UI_V2 ?= 0
ifeq ($(SEARCH_UI_COMPATIBLE_UI_V2),1)
KBUILD_CPPFLAGS += -DIBRT_SEARCH_UI
KBUILD_CPPFLAGS += -DSEARCH_UI_COMPATIBLE_UI_V2
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_CORE_V2),1)
export IBRT_V2_MULTIPOINT := 1
KBUILD_CPPFLAGS += -DIBRT_V2_MULTIPOINT
else
export IBRT_V2_MULTIPOINT := 0
endif
else
export IBRT_V2_MULTIPOINT := 0
endif

export IBRT_UI_MASTER_ON_TWS_DISCONNECTED ?= 0
ifeq ($(IBRT_UI_MASTER_ON_TWS_DISCONNECTED),1)
KBUILD_CPPFLAGS += -DIBRT_UI_MASTER_ON_TWS_DISCONNECTED
endif

export AF_STREAM_ID_0_PLAYBACK_FADEOUT ?= 0
ifneq ($(AF_STREAM_ID_0_PLAYBACK_FADEOUT),0)
KBUILD_CPPFLAGS += -DAF_STREAM_ID_0_PLAYBACK_FADEOUT
endif

export POWER_ON_ENTER_TWS_PAIRING_ENABLED ?= 0
ifeq ($(POWER_ON_ENTER_TWS_PAIRING_ENABLED),1)
IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 1
KBUILD_CPPFLAGS += -DPOWER_ON_ENTER_TWS_PAIRING_ENABLED
endif

export POWER_ON_ENTER_FREEMAN_PAIRING_ENABLED ?= 0
ifeq ($(POWER_ON_ENTER_FREEMAN_PAIRING_ENABLED),1)
IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 1
KBUILD_CPPFLAGS += -DPOWER_ON_ENTER_FREEMAN_PAIRING_ENABLED
endif

export PRODUCTION_LINE_PROJECT_ENABLED ?= 0
ifeq ($(PRODUCTION_LINE_PROJECT_ENABLED),1)
POWER_ON_OPEN_BOX_ENABLED := 0
KBUILD_CPPFLAGS += -DPRODUCTION_LINE_PROJECT_ENABLED
endif

export POWER_ON_OPEN_BOX_ENABLED ?= 0
ifeq ($(POWER_ON_OPEN_BOX_ENABLED),1)
IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 1
KBUILD_CPPFLAGS += -DPOWER_ON_OPEN_BOX_ENABLED
endif

export FREEMAN_OTA_ENABLE ?= 0
ifeq ($(FREEMAN_OTA_ENABLE),1)
KBUILD_CPPFLAGS += -DFREEMAN_OTA_ENABLED
endif

export IGNORE_POWER_ON_KEY_DURING_BOOT_UP ?= 0
ifeq ($(IGNORE_POWER_ON_KEY_DURING_BOOT_UP),1)
KBUILD_CPPFLAGS += -DIGNORE_POWER_ON_KEY_DURING_BOOT_UP
endif

ifeq ($(BT_ANC),1)
KBUILD_CPPFLAGS += -D__BT_ANC__
endif

ifeq ($(BTUSB_AUDIO_MODE),1)
export BTUSB_AUDIO_MODE
KBUILD_CPPFLAGS += -DBTUSB_AUDIO_MODE
endif

ifeq ($(BT_USB_AUDIO_DUAL_MODE),1)
export BT_USB_AUDIO_DUAL_MODE
KBUILD_CPPFLAGS += -DBT_USB_AUDIO_DUAL_MODE
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# BT watch related features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

export BT_WATCH_MASTER ?= 0
export BT_WATCH_SLAVE ?= 0

export EPLAYER_ON ?= 0
export EPLAYER_TEST ?= 0
export ECOMM_ON ?= 0
export ESHELL_ON ?= 0
export EAUDIO_ON ?= 0
export EAUDIO_TEST ?= 0
export BT_SERVICE_ON ?= 0
export BT_SERVICE_NATIVE ?= 0
export BT_SERVICE_DISTRIBUTE ?= 0
export BT_SERVICE_DISTRIBUTE_CLIENT ?= 0
export BT_SERVICE_DISTRIBUTE_SERVER ?= 0
export BT_SERVICE_DISTRIBUTE_TRANSPORT_ECOMM ?= 0
export BT_SERVICE_DISTRIBUTE_TRANSPORT_INTERSYS ?= 0
export BT_SERVICE_TEST ?= 0

export EPLAYER_INCLUDES ?=
EPLAYER_INCLUDES += \
    -Iservices/eplayer/eplayer \
    -Iservices/eplayer/eplayer/source \
    -Iservices/eplayer/eplayer/sink \
    -Iservices/eplayer/eplayer/decoder \
    -Iservices/eplayer/eplayer/mediainfo \
    -Iservices/eplayer/eplayer/platform/besrtx \
    -Iutils/eindexfifo \
    -Iutils/evf/ \
    -Iutils/evf/impl

export ECOMM_INCLUDES ?=
ECOMM_INCLUDES += \
    -Iservices/ecomm/ecomm \
    -Iservices/ecomm/ecomm/transport \
    -Iservices/ecomm/ecomm/platform \
    -Iservices/ecomm/ecomm/platform/bes_cmsisos \
    -Iservices/ecomm/api \
    -Iservices/ecomm/component \
    -Iutils/eindexfifo \
    -Iutils/evf/ \
    -Iutils/evf/impl \
    -Iutils/crc32

export ESHELL_INCLUDES ?=
ESHELL_INCLUDES += \
    -Iutils/eshell

export EAUDIO_INCLUDES ?=
EAUDIO_INCLUDES += \
    -Iservices/eaudio/inc \
    -Iservices/eaudio/effect \
    -Iservices/eaudio/device \
    -Iservices/eaudio/policy \
    -Iservices/eaudio/heap \
    -Iservices/eaudio/resample \
    -Iservices/bt_app \
    -Iutils/cqueue \
    -Iutils/heap \
    -Iservices/multimedia/audio/process/resample/include \
    -Iservices/multimedia/audio/process/resample/coef/include

export BT_SERVICE_INCLUDES ?=
BT_SERVICE_INCLUDES += \
    -Iservices/bt_service \
    -Iservices/bt_service/inc \
    -Iservices/bt_service/distribute/inc \
    -Iservices/bt_service/distribute/client/inc \
    -Iservices/bt_service/distribute/server/inc \
    -Iservices/bt_service/distribute/transport/ecomm \
    -Iutils/transport_if \

EINDEXFIFO_ON ?= 0
EVF_ON ?= 0

ifeq ($(EPLAYER_ON),1)
EINDEXFIFO_ON := 1
EVF_ON := 1
KBUILD_CPPFLAGS += -D__BESRTX__
endif

ifeq ($(ECOMM_ON),1)
EINDEXFIFO_ON := 1
EVF_ON := 1
endif

ifeq ($(EINDEXFIFO_ON),1)
core-y += utils/eindexfifo/
endif

ifeq ($(EVF_ON),1)
core-y += utils/evf/
endif

ifeq ($(ESHELL_ON),1)
core-y += utils/eshell/
KBUILD_CPPFLAGS += -DESHELL_ON
endif

ifeq ($(EAUDIO_ON),1)
KBUILD_CPPFLAGS += -DEAUDIO_ON
endif

ifeq ($(BT_SERVICE_ON),1)
KBUILD_CPPFLAGS += -DBT_SERVICE_ON
endif

ifeq ($(BT_WATCH_MASTER),1)
KBUILD_CPPFLAGS += -DBT_WATCH_MASTER
endif

ifeq ($(BT_WATCH_SLAVE),1)
KBUILD_CPPFLAGS += -DBT_WATCH_SLAVE
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# BT source features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ifeq ($(BT_WATCH_APP),1)

export BT_WATCH_APP

export BT_SOURCE := 1

export BT_MULTI_SOURCE ?= 0
export APP_LINEIN_A2DP_SOURCE ?= 0
export A2DP_SOURCE_AAC_ON ?= 0
export A2DP_SOURCE_LHDC_ON ?= 0
export APP_I2S_A2DP_SOURCE ?= 0
export APP_USB_A2DP_SOURCE ?= 0
export HFP_AG_ROLE ?= 0
export HFP_AG_SCO_AUTO_CONN ?= 0
export SOURCE_TRACE_RX ?= 0
export A2DP_SOURCE_TEST ?= 0
export HFP_AG_TEST ?= 0

KBUILD_CPPFLAGS += -DBT_WATCH_APP

ifeq ($(BT_SOURCE),1)
KBUILD_CPPFLAGS += -DBT_SOURCE
endif

ifeq ($(SOURCE_TRACE_RX),1)
KBUILD_CPPFLAGS += -D__SOURCE_TRACE_RX__
endif

ifeq ($(BT_MULTI_SOURCE),1)
KBUILD_CPPFLAGS += -DBT_MULTI_SOURCE
endif

ifeq ($(HFP_AG_ROLE),1)
KBUILD_CPPFLAGS += -DHFP_AG_ROLE

ifeq ($(HFP_AG_SCO_AUTO_CONN),1)
KBUILD_CPPFLAGS += -DHFP_AG_SCO_AUTO_CONN
endif

ifeq ($(HFP_AG_TEST),1)
KBUILD_CPPFLAGS += -DHFP_AG_TEST
endif
endif # HFP_AG_ROLE

ifeq ($(APP_LINEIN_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_LINEIN_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(A2DP_SOURCE_AAC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SOURCE_AAC_ON
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(A2DP_SOURCE_LHDC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SOURCE_LHDC_ON
KBUILD_CPPFLAGS += -DFLASH_UNIQUE_ID
KBUILD_CPPFLAGS += -DA2DP_ENCODE_CP_ACCEL
##KBUILD_CPPFLAGS += -DA2DP_TRACE_ENCODE_CP_ACCEL
core-y += thirdparty/audio_codec_lib/liblhdc-enc/
endif

ifeq ($(APP_I2S_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_I2S_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(APP_USB_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_USB_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
KBUILD_CPPFLAGS += -DA2DP_ENCODE_CP_ACCEL
endif

else # !BT_WATCH_APP

ifeq ($(BT_SOURCE),1)

KBUILD_CPPFLAGS += -DBT_SOURCE

ifeq ($(BT_MULTI_SOURCE),1)
KBUILD_CPPFLAGS += -DBT_MULTI_SOURCE
endif

ifeq ($(SOURCE_TRACE_RX),1)
KBUILD_CPPFLAGS += -D__SOURCE_TRACE_RX__
endif

ifeq ($(HFP_AG_ROLE),1)
KBUILD_CPPFLAGS += -DHFP_AG_ROLE

ifeq ($(HFP_AG_SCO_AUTO_CONN),1)
KBUILD_CPPFLAGS += -DHFP_AG_SCO_AUTO_CONN
endif

ifeq ($(HFP_AG_TEST),1)
KBUILD_CPPFLAGS += -DHFP_AG_TEST
endif
endif # HFP_AG_ROLE

ifeq ($(APP_LINEIN_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_LINEIN_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(A2DP_SOURCE_AAC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SOURCE_AAC_ON
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(A2DP_SOURCE_LHDC_ON),1)
KBUILD_CPPFLAGS += -DA2DP_SOURCE_LHDC_ON
KBUILD_CPPFLAGS += -DFLASH_UNIQUE_ID
KBUILD_CPPFLAGS += -DA2DP_ENCODE_CP_ACCEL
##KBUILD_CPPFLAGS += -DA2DP_TRACE_ENCODE_CP_ACCEL
core-y += thirdparty/audio_codec_lib/liblhdc-enc/
endif

ifeq ($(APP_I2S_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_I2S_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
endif

ifeq ($(APP_USB_A2DP_SOURCE),1)
KBUILD_CPPFLAGS += -DAPP_USB_A2DP_SOURCE
KBUILD_CPPFLAGS += -D__A2DP_AVDTP_CP__
KBUILD_CPPFLAGS += -DA2DP_ENCODE_CP_ACCEL
endif

ifeq ($(A2DP_SOURCE_TEST),1)
KBUILD_CPPFLAGS += -DA2DP_SOURCE_TEST
endif

endif # BT_SOURCE

endif # !BT_WATCH_APP

endif # BT_APP

# -------------------------------------------
# BLE features
# -------------------------------------------
# BLE profiles enable flag
## ANC service
export ANCS ?= 0
export ANCC ?= 0

# AMS service
export AMS ?= 0
export AMSC ?= 0

# BMS service
export BMS ?= 0

## HID service
export BLE_HID ?= 0

## BATT service
export BLE_BATT ?= 0

## DISS service
export BLE_DISS ?= 0

ifeq ($(GOOGLE_SERVICE_ENABLE), 1)
export BISTO_ENABLE := 1
export GFPS_ENABLE := 1
endif

ifeq ($(BISTO_ENABLE),1)
export BISTO_ENABLE
export VOICE_DATAPATH_ENABLED := 1
export CRASH_REBOOT ?= 1
export BLE_SECURITY_ENABLED := 1
ifeq ($(CHIP),best1400)
export DUMP_CRASH_LOG ?= 0
else
export DUMP_CRASH_LOG ?= 0
endif
export VOICE_DATAPATH_TYPE ?= gsound
#export TRACE_DUMP2FLASH ?= 1
export FLASH_SUSPEND := 1
export AI_OTA := 1
# ANC service server enable
ANCS := 1
# ANC service client enable
ANCC := 1

# AMS server enable
AMS := 1
# AMS client enable
AMSC := 1

# BMS server enable
BMS := 1

endif # ifeq ($(BISTO_ENABLE),1)

# 1 to enable BLE, 0 to disable BLE
export BLE ?= 0

BLE_SWITCH := \
    $(BISTO_ENABLE) \
    $(GFPS_ENABLE) \
    $(AMA_VOICE) \
    $(DMA_VOICE) \
    $(CUSTOMIZE_VOICE) \
    $(GMA_VOICE) \
    $(SMART_VOICE) \
    $(TENCENT_VOICE) \
    $(TILE_DATAPATH_ENABLED) \
    $(BLE_ONLY_ENABLED)

ifneq ($(filter 1, $(BLE_SWITCH)),)
export BLE := 1
endif

ifeq ($(BLE),1)

export BLE_APP_INCLUDES ?=
export BLE_PROFILES_INCLUDES ?=
export BLE_STACK_INCLUDES ?=

ifeq ($(BLE_V2),1)
ifeq ($(BLE_AOB_UX_ENABLED),1)
KBUILD_CPPFLAGS += -DAOB_UX_ENABLED
endif

export CUSTOMER_DEFINE_ADV_DATA ?= 0
ifeq ($(CUSTOMER_DEFINE_ADV_DATA),1)
KBUILD_CPPFLAGS += -DCUSTOMER_DEFINE_ADV_DATA
endif

export BLE_SEC_ACCEPT_BY_CUSTOMER ?= 0
ifeq ($(BLE_SEC_ACCEPT_BY_CUSTOMER),1)
KBUILD_CPPFLAGS += -DBLE_SEC_ACCEPT_BY_CUSTOMER
endif

KBUILD_CPPFLAGS += -DBLE_V2
BLE_APP_INCLUDES += \
    -Iservices/ble_app_v2 \
    -Iservices/ble_app_v2/app_batt/ \
    -Iservices/ble_app_v2/app_ble_custom \
    -Iservices/ble_app_v2/app_datapath/ \
    -Iservices/ble_app_v2/app_gfps \
    -Iservices/ble_app_v2/app_hid/ \
    -Iservices/ble_app_v2/app_hrps/ \
    -Iservices/ble_app_v2/app_htp/ \
    -Iservices/ble_app_v2/app_main \
    -Iservices/ble_app_v2/app_ota \
    -Iservices/ble_app_v2/app_sec \
    -Iservices/ble_app_v2/app_tota \
    -Iservices/ble_app_v2/app_tws \
    -Iservices/ble_app_v2/app_vob \
    -Iservices/ble_app_v2/app_voice \
    -Iservices/aob_core/custom \
    -Iservices/aob_core/inc \
    -Iservices/aob_core/inc/event \
    -Iservices/aob_core/inc/stm \
    -Iservices/aob_core/inc/call_ctl \
    -Iutils/hsm
ifeq ($(BLE_AUDIO_ENABLED),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_audio_app \
    -Iservices/ble_audio_app/app_bap \
    -Iservices/ble_audio_app/app_arc \
    -Iservices/ble_audio_app/app_acc \
    -Iservices/ble_audio_app/app_atc \
    -Iservices/ble_audio_app/app_cap
endif

ifeq ($(AMSC),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_app_v2/app_amsc/
endif

ifeq ($(ANCS),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_app_v2/app_ancs/
endif

ifeq ($(ANCC),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_app_v2/app_ancc/
endif

BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles_v2 \
    -Iservices/ble_profiles_v2/buds/ \
    -Iservices/ble_profiles_v2/datapath/datapathps/api/ \
    -Iservices/ble_profiles_v2/gfps/api/ \
    -Iservices/ble_profiles_v2/gfps/gfps_crypto/ \
    -Iservices/ble_profiles_v2/gfps/gfps_provider/api/ \
    -Iservices/ble_profiles_v2/gfps/gfps_provider/src/ \
    -Iservices/ble_profiles_v2/ota \
    -Iservices/ble_profiles_v2/tota \
    -Iservices/ble_profiles_v2/tile \
    -Iservices/ble_profiles_v2/voicepath/gsound
ifeq ($(BLE_AUDIO_ENABLED),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles_v2/otp \
    -Iservices/ble_profiles_v2/otp/otc/api \
    -Iservices/ble_profiles_v2/otp/ots/api \
    -Iservices/ble_profiles_v2/csip \
    -Iservices/ble_profiles_v2/csip/csisc/api \
    -Iservices/ble_profiles_v2/csip/csism/api
endif
ifeq ($(AMSC),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles_v2/ams/ \
    -Iservices/ble_profiles_v2/ams/amsc/ \
    -Iservices/ble_profiles_v2/ams/amsc/api
endif
ifeq ($(ANCS),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles_v2/anc/ancs/api
endif
ifeq ($(ANCC),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles_v2/anc/ \
    -Iservices/ble_profiles_v2/anc/ancc/ \
    -Iservices/ble_profiles_v2/anc/ancc/src \
    -Iservices/ble_profiles_v2/anc/ancc/api
endif

BLE_STACK_INCLUDES += \
    -Iservices/ble_stack_v2/ip/ahi/api/ \
    -Iservices/ble_stack_v2/ip/ble/hl/api/ \
    -Iservices/ble_stack_v2/ip/ble/hl/inc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/gapc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/gapm/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gap/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/gatt/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/inc/ \
    -Iservices/ble_stack_v2/ip/ble/hl/src/l2cap/ \
    -Iservices/ble_stack_v2/ip/hci/api/ \
    -Iservices/ble_stack_v2/ip/hci/src/ \
    -Iservices/ble_stack_v2/modules/aes/api/ \
    -Iservices/ble_stack_v2/modules/common/api/ \
    -Iservices/ble_stack_v2/modules/ecc_p256/api/ \
    -Iservices/ble_stack_v2/modules/ke/api/ \
    -Iservices/ble_stack_v2/modules/rwip/api/
ifeq ($(BLE_AUDIO_ENABLED),1)
BLE_STACK_INCLUDES += \
    -Iservices/ble_stack_v2/ip/ble/iso/data_path/ \
    -Iservices/ble_stack_v2/ip/ble/iso/data_path/isogen/api/ \
    -Iservices/ble_stack_v2/ip/ble/iso/data_path/isoohci/api/ \
    -Iservices/ble_stack_v2/ip/ble/iso/data_path/isogen/src/ \
    -Iservices/ble_stack_v2/ip/ble/iso/data_path/isoohci/src/ \
    -Iservices/ble_stack_v2/ip/gaf/api/ \
    -Iservices/ble_stack_v2/ip/gaf/api/acc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/acc/mc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/acc/ot/ \
    -Iservices/ble_stack_v2/ip/gaf/api/acc/tb/ \
    -Iservices/ble_stack_v2/ip/gaf/api/acc/vc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/arc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/arc/aic/ \
    -Iservices/ble_stack_v2/ip/gaf/api/arc/mic/ \
    -Iservices/ble_stack_v2/ip/gaf/api/arc/vc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/arc/voc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/atc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/atc/csi/ \
    -Iservices/ble_stack_v2/ip/gaf/api/atc/raa/ \
    -Iservices/ble_stack_v2/ip/gaf/api/bap/ \
    -Iservices/ble_stack_v2/ip/gaf/api/bap/bc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/bap/capa/ \
    -Iservices/ble_stack_v2/ip/gaf/api/bap/uc/ \
    -Iservices/ble_stack_v2/ip/gaf/api/cap/ \
    -Iservices/ble_stack_v2/ip/gaf/api/iap/ \
    -Iservices/ble_stack_v2/ip/gaf/inc/ \
	-Iservices/ble_stack_v2/ip/gaf/src/ \
    -Iservices/ble_stack_v2/ip/gaf/src/al/ \
    -Iservices/ble_stack_v2/ip/gaf/src/arc/ \
    -Iservices/ble_stack_v2/ip/gaf/src/bap/bc/ \
    -Iservices/ble_stack_v2/ip/gaf/src/bap/capa/ \
    -Iservices/ble_stack_v2/ip/gaf/src/bap/codec/ \
    -Iservices/ble_stack_v2/ip/gaf/src/iap/
endif
else
BLE_APP_INCLUDES += \
    -Iservices/ble_app \
    -Iservices/ble_app/app_amsc/ \
    -Iservices/ble_app/app_ancc/ \
    -Iservices/ble_app/app_batt/ \
    -Iservices/ble_app/app_ble_custom \
    -Iservices/ble_app/app_datapath/ \
    -Iservices/ble_app/app_gfps \
    -Iservices/ble_app/app_hid/ \
    -Iservices/ble_app/app_hrps/ \
    -Iservices/ble_app/app_htp/ \
    -Iservices/ble_app/app_main \
    -Iservices/ble_app/app_ota \
    -Iservices/ble_app/app_sec \
    -Iservices/ble_app/app_tota \
    -Iservices/ble_app/app_tws \
    -Iservices/ble_app/app_vob \
    -Iservices/ble_app/app_voice
ifeq ($(AMSC),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_app/app_amsc/
endif

ifeq ($(ANCC),1)
BLE_APP_INCLUDES += \
    -Iservices/ble_app/app_ancc/
endif

BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles/bas/bass/api/ \
    -Iservices/ble_profiles/bas/bass/src/ \
    -Iservices/ble_profiles/datapath/datapathps/api/ \
    -Iservices/ble_profiles/gfps/api/ \
    -Iservices/ble_profiles/gfps/gfps_crypto/ \
    -Iservices/ble_profiles/gfps/gfps_provider/api/ \
    -Iservices/ble_profiles/gfps/gfps_provider/src/ \
    -Iservices/ble_profiles/hogp/ \
    -Iservices/ble_profiles/hogp/hogpd/api/ \
    -Iservices/ble_profiles/hrp/ \
    -Iservices/ble_profiles/hrp/hrps/api/ \
    -Iservices/ble_profiles/hrp/hrps/src \
    -Iservices/ble_profiles/htp/ \
    -Iservices/ble_profiles/htp/htpt/api/ \
    -Iservices/ble_profiles/htp/htpt/src/ \
    -Iservices/ble_profiles/ota \
    -Iservices/ble_profiles/tile \
    -Iservices/ble_profiles/voicepath/gsound
ifeq ($(AMSC),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles/ams/ \
    -Iservices/ble_profiles/ams/amsc/
endif
ifeq ($(ANCS),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles/anc/ancs/api
endif
ifeq ($(ANCC),1)
BLE_PROFILES_INCLUDES += \
    -Iservices/ble_profiles/anc/ \
    -Iservices/ble_profiles/anc/ancc/ \
    -Iservices/ble_profiles/anc/ancc/src/
endif

BLE_STACK_INCLUDES += \
    -Iservices/ble_stack/app/api/  \
    -Iservices/ble_stack/app/src/ \
    -Iservices/ble_stack/ble_ip \
    -Iservices/ble_stack/common/api \
    -Iservices/ble_stack/hci/api/ \
    -Iservices/ble_stack/hci/inc/ \
    -Iservices/ble_stack/hci/src/ \
    -Iservices/ble_stack/hl/api \
    -Iservices/ble_stack/hl/inc/ \
    -Iservices/ble_stack/hl/src/gap/ \
    -Iservices/ble_stack/hl/src/gap/gapc/ \
    -Iservices/ble_stack/hl/src/gap/gapm/ \
    -Iservices/ble_stack/hl/src/gap/smpc/ \
    -Iservices/ble_stack/hl/src/gatt/ \
    -Iservices/ble_stack/hl/src/gatt/attc/ \
    -Iservices/ble_stack/hl/src/gatt/attm/ \
    -Iservices/ble_stack/hl/src/gatt/atts/ \
    -Iservices/ble_stack/hl/src/gatt/gattc/ \
    -Iservices/ble_stack/hl/src/gatt/gattm/ \
    -Iservices/ble_stack/hl/src/l2c/l2cc/ \
    -Iservices/ble_stack/hl/src/l2c/l2cm/ \
    -Iservices/ble_stack/ke/api/ \
    -Iservices/ble_stack/ke/src/ \
    -Iservices/ble_stack/profiles/htp/ \
    -Iservices/ble_stack/profiles/htp/htpt/api/ \
    -Iservices/ble_stack/profiles/htp/htpt/src/
endif #BLE_V2

KBUILD_CPPFLAGS += -D__IAG_BLE_INCLUDE__

export BLE_CONNECTION_MAX ?= 2
KBUILD_CPPFLAGS += -DBLE_CONNECTION_MAX=$(BLE_CONNECTION_MAX)

ifeq ($(IS_ENABLE_DEUGGING_MODE),1)
KBUILD_CPPFLAGS += -DIS_ENABLE_DEUGGING_MODE
endif

ifeq ($(BLE_USE_RPA),1)
KBUILD_CPPFLAGS += -DBLE_USE_RPA
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
export BLE_AUDIO_ENABLED
KBUILD_CPPFLAGS += -DBLE_AUDIO_ENABLED=1
KBUILD_CPPFLAGS += -DBLE_AUDIO_CONNECTION_CNT=2
KBUILD_CPPFLAGS += -DBLE_AUDIO_CIS_CONN_CNT=4
#bis sink volume sync source
ifeq ($(BIS_VOL_SYNC_ENABLED),1)
KBUILD_CPPFLAGS += -DBIS_VOL_SYNC_ENABLED
endif
# currently always enable mobile for debug
export AOB_MOBILE_ENABLED ?= 1
ifeq ($(AOB_MOBILE_ENABLED),1)
KBUILD_CPPFLAGS += -DAOB_MOBILE_ENABLED
endif
else
KBUILD_CPPFLAGS += -DBLE_AUDIO_ENABLED=0
endif

ifeq ($(GFPS_ENABLE),1)
BLE_SECURITY_ENABLED := 1
endif
ifeq ($(BLE_SECURITY_ENABLED), 1)
export BLE_SECURITY_ENABLED
KBUILD_CPPFLAGS += -DCFG_APP_SEC
endif

ifeq ($(BISTO_ENABLE),1)
ifneq ($(IBRT),1)
#disbled before IBRT MAP role switch feature is ready
KBUILD_CPPFLAGS += -DBT_MAP_SUPPORT
KBUILD_CPPFLAGS += -DBT_OBEX_SUPPORT
endif
endif

KBUILD_CPPFLAGS += -D__BLE_TX_USE_BT_TX_QUEUE__
KBUILD_CPPFLAGS += -DBES_BLE_ACTIVITY_MAX=9
KBUILD_CPPFLAGS += -DBES_BLE_ADV_DATA_MAX=31

ifeq ($(ANCS),1)
KBUILD_CPPFLAGS += -DANCS_ENABLED
endif

ifeq ($(ANCC),1)
KBUILD_CPPFLAGS += -DANCC_ENABLED
endif

ifeq ($(AMS),1)
KBUILD_CPPFLAGS += -DAMS_ENABLED
endif

ifeq ($(AMSC),1)
KBUILD_CPPFLAGS += -DAMSC_ENABLED
endif

ifeq ($(BMS),1)
KBUILD_CPPFLAGS += -DBMS_ENABLED
endif

ifeq ($(BLE_HID),1)
KBUILD_CPPFLAGS += -DBLE_HID_ENABLE
endif

ifeq ($(BLE_BATT),1)
KBUILD_CPPFLAGS += -DBLE_BATT_ENABLE
endif

ifeq ($(BLE_DISS),1)
KBUILD_CPPFLAGS += -DBLE_DISS_ENABLE
endif

endif # BLE

# -------------------------------------------
# Full application features (BT and/or BLE)
# -------------------------------------------

ifeq ($(FULL_APP_PROJECT),1)

export SPEECH_LIB ?= 1

# make sure the value of GFPS_ENABLE and GMA_VOICE is confirmed above here
ifneq ($(filter 1,$(GFPS_ENABLE) $(GMA_VOICE) $(TOTA) $(TOTA_v2) $(BLE) $(CTKD_ENABLE)),)
core-y += utils/encrypt/
endif

ifeq ($(CTKD_ENABLE),1)
export CTKD_ENABLE
core-y += utils/aes_cmac/
KBUILD_CPPFLAGS += -DCTKD_ENABLE
endif

ifeq ($(SIGNAL_GENERATOR),1)
core-y += utils/signal_generator/
KBUILD_CPPFLAGS += -DSIGNAL_GENERATOR
endif

export BESLIB_INFO := $(GIT_REVISION)

export FLASH_PROTECTION ?= 1

export APP_TEST_AUDIO ?= 0

ifeq ($(APP_TEST_MODE),1)
export APP_TEST_MODE
KBUILD_CPPFLAGS += -DAPP_TEST_MODE
endif

export VOICE_PROMPT ?= 1

export AUDIO_QUEUE_SUPPORT ?= 1

export VOICE_RECOGNITION ?= 0

export FLASH_SUSPEND ?= 1

export ENGINEER_MODE ?= 1
ifeq ($(ENGINEER_MODE),1)
FACTORY_MODE := 1
endif
ifeq ($(FACTORY_MODE),1)
KBUILD_CPPFLAGS += -D__FACTORY_MODE_SUPPORT__
endif

export NEW_NV_RECORD_ENABLED ?= 1
ifeq ($(NEW_NV_RECORD_ENABLED),1)
KBUILD_CPPFLAGS += -DNEW_NV_RECORD_ENABLED
KBUILD_CPPFLAGS += -Iservices/nv_section/userdata_section
else
KBUILD_CPPFLAGS += -Iservices/nvrecord
endif

FILE_SYSTEM_SECTION_SIZE ?= 0

ifeq ($(APP_USE_LED_INDICATE_IBRT_STATUS),1)
KBUILD_CPPFLAGS += -D__APP_USE_LED_INDICATE_IBRT_STATUS__
endif

ifeq ($(HEAR_THRU_PEAK_DET),1)
KBUILD_CPPFLAGS += -D__HEAR_THRU_PEAK_DET__
endif

ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -DBESBT_STACK_SIZE=1024*8+512
else
KBUILD_CPPFLAGS += -DBESBT_STACK_SIZE=1024*5+512
endif

ifeq ($(FREE_TWS_PAIRING_ENABLED),1)
KBUILD_CPPFLAGS += -DFREE_TWS_PAIRING_ENABLED
endif

ifeq ($(VOICE_TX_AEC),1)
export VOICE_TX_AEC
KBUILD_CPPFLAGS += -DVOICE_TX_AEC
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

ifeq ($(APP_NOISE_ESTIMATION),1)
export APP_NOISE_ESTIMATION
KBUILD_CPPFLAGS += -DAPP_NOISE_ESTIMATION
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

ifeq ($(VOICE_PROMPT),1)
KBUILD_CPPFLAGS += -DMEDIA_PLAYER_SUPPORT
endif

ifeq ($(TOTA),1)
KBUILD_CPPFLAGS += -DTOTA
ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -DBLE_TOTA_ENABLED
endif
KBUILD_CPPFLAGS += -DSHOW_RSSI
KBUILD_CPPFLAGS += -DTEST_OVER_THE_AIR_ENANBLED
export TEST_OVER_THE_AIR ?= 1
endif

ifeq ($(TOTA_v2),1)
KBUILD_CPPFLAGS += -DTOTA_v2
ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -DBLE_TOTA_ENABLEDx
endif
KBUILD_CPPFLAGS += -DSHOW_RSSI
KBUILD_CPPFLAGS += -DTEST_OVER_THE_AIR_ENANBLED
export TEST_OVER_THE_AIR_v2 ?= 1
endif

export RESUME_MUSIC_AFTER_CRASH_REBOOT ?= 0
ifeq ($(RESUME_MUSIC_AFTER_CRASH_REBOOT),1)
KBUILD_CPPFLAGS += -DRESUME_MUSIC_AFTER_CRASH_REBOOT
endif

ifeq ($(TEST_OVER_THE_AIR),1)
export TEST_OVER_THE_AIR
KBUILD_CPPFLAGS += -DTEST_OVER_THE_AIR_ENANBLED=1
endif

ifeq ($(USE_THIRDPARTY),1)
KBUILD_CPPFLAGS += -D__THIRDPARTY
core-y += thirdparty/
endif

ifeq ($(SENSOR),1)
export SENSOR
KBUILD_CPPFLAGS += -D__SENSOR__
endif

ifeq ($(APP_UART_MODULE),1)
export APP_UART_MODULE
KBUILD_CPPFLAGS += -DAPP_UART_MODULE
endif

export APP_KEY_ENABLE := 1
ifeq ($(APP_KEY_ENABLE),1)
KBUILD_CPPFLAGS += -DAPP_KEY_ENABLE
endif

ifeq ($(APP_ANC_TEST),1)
export APP_ANC_TEST
KBUILD_CPPFLAGS += -DAPP_ANC_TEST
endif

# AUDIO_DEBUG
ifeq ($(AUDIO_DEBUG_CMD),1)
export AUDIO_DEBUG_CMD
endif

ifeq ($(AUDIO_DEBUG),1)
export AUDIO_DEBUG
export AUDIO_DEBUG_CMD ?= 1
KBUILD_CPPFLAGS += -DAUDIO_DEBUG
endif
# AUDIO_DEBUG END

ifneq ($(filter BT_HFP_AT BT_SPP,$(APP_DEBUG_TOOL)),)
export APP_DEBUG_TOOL
endif

export BT_WIFI_COEX_P12 ?=0
ifeq ($(DEBUG),1)
ifneq ($(filter 1, $(AUDIO_DEBUG_CMD) $(AUTO_TEST) $(BES_AUTOMATE_TEST)),)
export APP_TRACE_RX_ENABLE := 1
else
ifneq ($(BT_DEBUG_TPORTS),0)
ifeq ($(BT_WIFI_COEX_P12),0)
export APP_TRACE_RX_ENABLE := 1
endif
endif
endif
endif
ifeq ($(APP_TRACE_RX_ENABLE),1)
KBUILD_CPPFLAGS += -DAPP_TRACE_RX_ENABLE
endif

ifeq ($(WATCHER_DOG),1)
KBUILD_CPPFLAGS += -D__WATCHER_DOG_RESET__
endif

ifeq ($(AUTO_TEST),1)
export AUTO_TEST
KBUILD_CFLAGS += -D_AUTO_TEST_
endif

ifeq ($(BES_AUTOMATE_TEST),1)
export BES_AUTOMATE_TEST
KBUILD_CFLAGS += -DBES_AUTOMATE_TEST
endif

ifeq ($(SPEECH_BONE_SENSOR),1)
export SPEECH_BONE_SENSOR
# ADC and I2S use PLL
# export NO_SCO_RESAMPLE := 1
KBUILD_CPPFLAGS += -DSPEECH_BONE_SENSOR

# Configure IIC
KBUILD_CPPFLAGS += -DI2C_TASK_MODE
KBUILD_CPPFLAGS += -DLIS25BA_I2C_ID=0

# Configure I2S/TDM
ifneq ($(CHIP_HAS_TDM),)
# 2300p/2300a...
export AF_DEVICE_TDM := 1
export SPEECH_BONE_SENSOR_TDM := 1
KBUILD_CPPFLAGS += -DI2S_MCLK_FROM_SPDIF
KBUILD_CPPFLAGS += -DI2S_MCLK_PIN
KBUILD_CPPFLAGS += -DI2S_MCLK_IOMUX_INDEX=33
else
# 2300
export AF_DEVICE_I2S := 1
export SPEECH_BONE_SENSOR_I2S := 1
KBUILD_CPPFLAGS += -DI2S_MCLK_FROM_SPDIF
KBUILD_CPPFLAGS += -DI2S_MCLK_PIN
KBUILD_CPPFLAGS += -DI2S_MCLK_IOMUX_INDEX=13
KBUILD_CPPFLAGS += -DCLKOUT_IOMUX_INDEX=13
endif

# Sync ADC and I2S
ifeq ($(CHIP_HAS_I2S_TDM_TRIGGER),1)
# Hardware trigger method
KBUILD_CPPFLAGS += -DHW_I2S_TDM_TRIGGER
else
# Software method
export INT_LOCK_EXCEPTION ?= 1
KBUILD_CPPFLAGS += -DAF_ADC_I2S_SYNC
endif
# KBUILD_CPPFLAGS += -DSPEECH_DEBUG_ADC_I2S_SYNC
endif # SPEECH_BONE_SENSOR

export IS_AUTOPOWEROFF_ENABLED ?= 1
ifeq ($(IS_AUTOPOWEROFF_ENABLED),1)
KBUILD_CFLAGS += -D__BTIF_AUTOPOWEROFF__
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# BISTO feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(BISTO_ENABLE),1)

KBUILD_CFLAGS += -DBISTO_ENABLED

KBUILD_CPPFLAGS += -DCFG_SW_KEY_LPRESS_THRESH_MS=1000

KBUILD_CPPFLAGS += -DDEBUG_BLE_DATAPATH=0

KBUILD_CFLAGS += -DCRC32_OF_IMAGE

ASSERT_SHOW_FILE_FUNC ?= 1

#KBUILD_CPPFLAGS += -DSAVING_AUDIO_DATA_TO_SD_ENABLED=1

KBUILD_CPPFLAGS += -DIS_GSOUND_BUTTION_HANDLER_WORKAROUND_ENABLED

ifeq ($(GSOUND_HOTWORD_ENABLE), 1)
export GSOUND_HOTWORD_ENABLE
KBUILD_CFLAGS += -DGSOUND_HOTWORD_ENABLED
ifeq ($(GSOUND_HOTWORD_EXTERNAL), 1)
export GSOUND_HOTWORD_EXTERNAL
KBUILD_CFLAGS += -DGSOUND_HOTWORD_EXTERNAL
endif
endif

REDUCED_GUESTURE_ENABLE ?= 0

ifeq ($(REDUCED_GUESTURE_ENABLE), 1)
KBUILD_CFLAGS += -DREDUCED_GUESTURE_ENABLED
endif
endif # BISTO_ENABLE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# GFPS feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(GFPS_ENABLE),1)
export BLE_SECURITY_ENABLED := 1
export GFPS_ENABLE

KBUILD_CPPFLAGS += -DGFPS_ENABLED

# this macro is used to determain if the resolveable private address is used for BLE
KBUILD_CPPFLAGS += -DBLE_USE_RPA
endif # GFPS

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# AMA feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(AMA_VOICE),1)
export AMA_VOICE

KBUILD_CPPFLAGS += -D__AMA_VOICE__
KBUILD_CPPFLAGS += -DBLE_USE_RPA
KBUILD_CPPFLAGS += -DAMA_ENCODE_USE_OPUS
endif # AMA_VOICE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# DMA feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(DMA_VOICE),1)
export DMA_VOICE
KBUILD_CPPFLAGS += -D__DMA_VOICE__
KBUILD_CPPFLAGS += -D__BES__
KBUILD_CPPFLAGS += -DNVREC_BAIDU_DATA_SECTION
KBUILD_CPPFLAGS += -DBAIDU_DATA_RAND_LEN=8
KBUILD_CPPFLAGS += -DFM_MIN_FREQ=875
KBUILD_CPPFLAGS += -DFM_MAX_FREQ=1079
KBUILD_CPPFLAGS += -DBAIDU_DATA_DEF_FM_FREQ=893
KBUILD_CPPFLAGS += -DBAIDU_DATA_SN_LEN=16
export SHA256_ROM ?= 1
ifeq ($(LIBC_ROM),1)
$(error LIBC_ROM should be 0 when DMA_VOICE=1)
endif
endif # DMA_VOICE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# GMA feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(GMA_VOICE),1)
export GMA_VOICE
KBUILD_CPPFLAGS += -D__GMA_VOICE__

#KBUILD_CPPFLAGS += -DKEYWORD_WAKEUP_ENABLED=0
#KBUILD_CPPFLAGS += -DPUSH_AND_HOLD_ENABLED=1
#KBUILD_CPPFLAGS += -DAI_32KBPS_VOICE=0

KBUILD_CPPFLAGS += -D__GMA_OTA_TWS__
#KBUILD_CPPFLAGS += -DMCU_HIGH_PERFORMANCE_MODE
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# SMART_VOICE feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(SMART_VOICE),1)
export SMART_VOICE
KBUILD_CPPFLAGS += -D__SMART_VOICE__
#SPEECH_CODEC_CAPTURE_CHANNEL_NUM ?= 2
#KBUILD_CPPFLAGS += -DMCU_HIGH_PERFORMANCE_MODE
#KBUILD_CPPFLAGS += -DSPEECH_CAPTURE_TWO_CHANNEL
endif # SMART_VOICE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# TENCENT_VOICE feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(TENCENT_VOICE),1)
export TENCENT_VOICE
KBUILD_CPPFLAGS += -D__TENCENT_VOICE__
endif # TENCENT_VOICE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# CUSTOMIZE feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(CUSTOMIZE_VOICE),1)
export CUSTOMIZE_VOICE
KBUILD_CPPFLAGS += -D__CUSTOMIZE_VOICE__
#SPEECH_CODEC_CAPTURE_CHANNEL_NUM ?= 2
#KBUILD_CPPFLAGS += -DMCU_HIGH_PERFORMANCE_MODE
#KBUILD_CPPFLAGS += -DSPEECH_CAPTURE_TWO_CHANNEL
endif # CUSTOMIZE_VOICE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Dual MIC recording feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(DUAL_MIC_RECORDING),1)
export DUAL_MIC_RECORDING
KBUILD_CPPFLAGS += -DDUAL_MIC_RECORDING
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_KISS_FFT

ifeq ($(STEREO_RECORD_PROCESS),1)
export STEREO_RECORD_PROCESS
endif
endif # DUAL_MIC_RECORDING

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# AI_VOICE feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifneq ($(filter 1,$(BISTO_ENABLE) $(AMA_VOICE) $(DMA_VOICE) $(SMART_VOICE) $(TENCENT_VOICE) $(GMA_VOICE) $(CUSTOMIZE_VOICE) $(DUAL_MIC_RECORDING)),)
AI_VOICE := 1
USE_THIRDPARTY := 1
endif

ifeq ($(AI_VOICE),1)
export AI_VOICE
KBUILD_CPPFLAGS += -D__AI_VOICE__
BTIF_DIP_DEVICE := 1

ifeq ($(BLE),1)
KBUILD_CPPFLAGS += -D__AI_VOICE_BLE_ENABLE__
endif

ifeq ($(ALEXA_WWE),1)
KBUILD_CPPFLAGS += -DAPP_THREAD_STACK_SIZE=3072
endif
endif # AI_VOICE

ifeq ($(THROUGH_PUT),1)
KBUILD_CPPFLAGS += -D__THROUGH_PUT__
endif # THROUGH_PUT

ifeq ($(USE_KNOWLES),1)
KBUILD_CPPFLAGS += -D__KNOWLES
KBUILD_CPPFLAGS += -DIDLE_ALEXA_KWD
export THIRDPARTY_LIB := knowles_uart
endif

AI_CAPTURE_CHANNEL_NUM ?= 0
ifneq ($(AI_CAPTURE_CHANNEL_NUM),0)
KBUILD_CPPFLAGS += -DAI_CAPTURE_CHANNEL_NUM=$(AI_CAPTURE_CHANNEL_NUM)
endif

AI_CAPTURE_DATA_AEC ?= 0
ifeq ($(AI_CAPTURE_DATA_AEC),1)
KBUILD_CPPFLAGS += -DAI_CAPTURE_DATA_AEC
KBUILD_CPPFLAGS += -DAI_ALGORITHM_ENABLE
KBUILD_CPPFLAGS += -DAEC_STERE_ON
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

export AI_AEC_CP_ACCEL ?= 0
ifeq ($(AI_AEC_CP_ACCEL),1)
KBUILD_CPPFLAGS += -DAI_AEC_CP_ACCEL
endif

export BTIF_DIP_DEVICE ?= 0
ifeq ($(BTIF_DIP_DEVICE),1)
KBUILD_CPPFLAGS += -DBTIF_DIP_DEVICE
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Print bt addr config
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
export BT_ADDR_PRINT_CONFIG ?= 0
ifeq ($(BT_ADDR_PRINT_CONFIG),1)
KBUILD_CPPFLAGS += -DBT_ADDR_OUTPUT_PRINT_NUM=2
else
KBUILD_CPPFLAGS += -DBT_ADDR_OUTPUT_PRINT_NUM=6
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# MULTI_AI feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# NOTE: value of AI_VOICE and BISTO_ENABLE must already confirmed above here
ifeq ($(AI_VOICE)-$(BISTO_ENABLE),1-1)
IS_MULTI_AI_ENABLE := 1
endif
ifeq ($(IS_MULTI_AI_ENABLE),1)
export SHA256_ROM ?= 1
KBUILD_CPPFLAGS += -DIS_MULTI_AI_ENABLED
endif # MULTI_AI

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# VOICE COMPRESSION feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUAL_MIC_RECORDING ?= 0
RECORDING_USE_OPUS ?= 0
RECORDING_USE_OPUS_LOWER_BANDWIDTH ?= 0
RECORDING_USE_SCALABLE ?= 0
## determine the value of VOC_ENCODE_ENABLE
ifneq ($(filter 1, $(AMA_VOICE) $(DMA_VOICE) $(GMA_VOICE) $(SMART_VOICE) $(TENCENT_VOICE) $(CUSTOMIZE_VOICE) $(DUAL_MIC_RECORDING)),)
export VOC_ENCODE_ENABLE := 1
KBUILD_CPPFLAGS += -DVOC_ENCODE_ENABLE
endif

## voice compression use ADPCM
# KBUILD_CPPFLAGS += -DVOC_ENCODE_ADPCM=1

## voice compression use OPUS
### record use OPUS
VOC_RECORD_ENCODE_USE_OPUS ?= 0
ifeq ($(filter 0, $(DUAL_MIC_RECORDING) $(RECORDING_USE_OPUS)),)
VOC_RECORD_ENCODE_USE_OPUS := 1
#### recording use opus config
KBUILD_CPPFLAGS += -DRECORDING_USE_OPUS
#### low bandwidth config
ifeq ($(RECORDING_USE_OPUS_LOWER_BANDWIDTH),1)
KBUILD_CPPFLAGS += -DRECORDING_USE_OPUS_LOW_BANDWIDTH
endif
endif
### AI use OPUS
VOC_AI_ENCODE_USE_OPUS ?= 0
ifneq ($(filter 1, $(AMA_VOICE) $(DMA_VOICE) $(GMA_VOICE) $(SMART_VOICE) $(TENCENT_VOICE) $(CUSTOMIZE_VOICE)),)
VOC_AI_ENCODE_USE_OPUS := 1
endif
### voice compression codec type determine
ifneq ($(filter 1, $(VOC_RECORD_ENCODE_USE_OPUS) $(VOC_AI_ENCODE_USE_OPUS)),)
export VOC_ENCODE_OPUS := 1
KBUILD_CPPFLAGS += -DVOC_ENCODE_OPUS=2
KBUILD_CPPFLAGS += -DOPUS_IN_OVERLAY
endif

## voice compression use SBC
ifeq ($(BISTO_ENABLE), 1)
export VOC_ENCODE_SBC := 1
KBUILD_CPPFLAGS += -DVOC_ENCODE_SBC=3
endif

## voice compression use SCALABLE
ifeq ($(filter 0, $(DUAL_MIC_RECORDING) $(RECORDING_USE_SCALABLE)),)
export VOC_ENCODE_SCALABLE := 1
KBUILD_CPPFLAGS += -DVOC_ENCODE_SCALABLE=4
KBUILD_CPPFLAGS += -DRECORDING_USE_SCALABLE
endif


# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# VOICE_DATAPATH feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(VOICE_DATAPATH_ENABLED),1)
export VOICE_DATAPATH_ENABLED
KBUILD_CPPFLAGS += -DVOICE_DATAPATH
endif # VOICE_DATAPATH

export SLAVE_ADV_BLE_ENABLED ?= 0
ifeq ($(SLAVE_ADV_BLE_ENABLED),1)
KBUILD_CPPFLAGS += -DSLAVE_ADV_BLE
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# TILE feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(TILE_DATAPATH_ENABLED),1)
export TILE_DATAPATH_ENABLED
KBUILD_CPPFLAGS += -DTILE_DATAPATH
endif

export CUSTOM_INFORMATION_TILE_ENABLE ?= 0
ifeq ($(CUSTOM_INFORMATION_TILE_ENABLE),1)
KBUILD_CPPFLAGS += -DCUSTOM_INFORMATION_TILE=1
endif # TILE

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# MFI feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(IOS_MFI),1)
KBUILD_CPPFLAGS += -DIOS_IAP2_PROTOCOL
endif # IOS_MFI

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# OTA feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
export VERSION_INFO

export OTA_ENABLE ?= 0
ifneq ($(filter 1,$(BES_OTA) $(AI_OTA) $(INTERCONNECTION) $(GMA_VOICE)),)
OTA_ENABLE := 1
endif

ifeq ($(OTA_ENABLE),1)
ifeq ($(FPGA),1)
export OTA_CODE_OFFSET ?= 0
else
ifneq ($(filter best1400 best2300 best2300p,$(CHIP)),)
export OTA_CODE_OFFSET ?= 0x18000
else
export OTA_CODE_OFFSET ?= 0x20000
endif
endif
KBUILD_CPPFLAGS += -DOTA_ENABLE
KBUILD_CPPFLAGS += -D__APP_IMAGE_FLASH_OFFSET__=$(OTA_CODE_OFFSET)
KBUILD_CPPFLAGS += -DFIRMWARE_REV
ifeq ($(FLASH_REMAP),1)
export FLASH_PROTECTION := 0
# can change this value if 2MB cannot fit the application image
OTA_MAX_IMAGE_SIZE ?= 0x200000
OTA_REMAP_OFFSET ?= $(OTA_MAX_IMAGE_SIZE)
KBUILD_CPPFLAGS += -DOTA_REMAP_OFFSET=$(OTA_REMAP_OFFSET)
KBUILD_CPPFLAGS += -DFLASH_REMAP
KBUILD_CPPFLAGS += -DNEW_IMAGE_FLASH_OFFSET=$(OTA_CODE_OFFSET)
else
KBUILD_CPPFLAGS += -DNEW_IMAGE_FLASH_OFFSET=0x200000
endif # REMAP
endif # OTA

ifeq ($(BES_OTA),1)
export BES_OTA
KBUILD_CPPFLAGS += -DBES_OTA

ifeq ($(IBRT),1)
export IBRT_OTA := 1
KBUILD_CPPFLAGS += -DIBRT_OTA
endif
endif

ifeq ($(AI_OTA),1)
export AI_OTA
KBUILD_CPPFLAGS += -DAI_OTA
endif

# shall not be updated by application project, as it influences the ibrt core library
ifneq ($(filter 1, $(GFPS_ENABLE) $(TILE_DATAPATH_ENABLED) $(GMA_VOICE) $(BES_OTA) $(DUAL_MIC_RECORDING) $(LINEIN_TRANS_ON)),)
export APP_TWS_MTU_SIZE := 672
else
export APP_TWS_MTU_SIZE := 348
endif
KBUILD_CPPFLAGS += -DAPP_TWS_CTRL_BUFFER_MAX_LEN=$(APP_TWS_MTU_SIZE)

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# MIX_MIC_DURING_MUSIC feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(MIX_MIC_DURING_MUSIC_ENABLED),1)
KBUILD_CPPFLAGS += -DMIX_MIC_DURING_MUSIC
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# PROMPT_IN_FLASH feature
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(PROMPT_IN_FLASH),1)
export PROMPT_IN_FLASH
KBUILD_CPPFLAGS += -DPROMPT_IN_FLASH

ifeq ($(PROMPT_EMBEDED),1)
export PROMPT_EMBEDED
KBUILD_CPPFLAGS += -DPROMPT_EMBEDED
endif
endif

ifeq ($(COMBO_CUSBIN_IN_FLASH),1)
KBUILD_CPPFLAGS += -DCOMBO_CUSBIN_IN_FLASH
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Flash suspend features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(FLASH_SUSPEND), 1)
KBUILD_CPPFLAGS += -DFLASH_SUSPEND
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# BLE only enable features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(BLE_ONLY_ENABLED),1)
export BLE_ONLY_ENABLED
KBUILD_CPPFLAGS += -DBLE_ONLY_ENABLED
KBUILD_CPPFLAGS += -DBLE_POWER_LEVEL_0
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# GATT over BR/EDR features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(GATT_OVER_BR_EDR),1)
export GATT_OVER_BR_EDR
KBUILD_CPPFLAGS += -D__GATT_OVER_BR_EDR__
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Sensor Hub features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ifeq ($(SENSOR_HUB),1)
KBUILD_CPPFLAGS += -DSENSOR_HUB
endif

ifneq ($(HW_VERSION_STRING),)
export HW_VERSION_STRING
endif

KBUILD_CPPFLAGS += -DMULTIPOINT_DUAL_SLAVE

endif # FULL_APP_PROJECT

# -------------------------------------------
# Speech features
# -------------------------------------------

ifeq ($(USB_AUDIO_SPEECH),1)
export USB_AUDIO_SPEECH
export USB_AUDIO_DYN_CFG := 0
export KEEP_SAME_LATENCY := 1
export SPEECH_LIB := 1
endif

ifeq ($(SPEECH_LIB),1)

export DSP_LIB ?= 1

ifeq ($(USB_AUDIO_APP),1)
ifneq ($(USB_AUDIO_SEND_CHAN),$(SPEECH_CODEC_CAPTURE_CHANNEL_NUM))
$(info )
$(info CAUTION: Change USB_AUDIO_SEND_CHAN($(USB_AUDIO_SEND_CHAN)) to SPEECH_CODEC_CAPTURE_CHANNEL_NUM($(SPEECH_CODEC_CAPTURE_CHANNEL_NUM)))
$(info )
export USB_AUDIO_SEND_CHAN := $(SPEECH_CODEC_CAPTURE_CHANNEL_NUM)
ifneq ($(USB_AUDIO_SEND_CHAN),$(SPEECH_CODEC_CAPTURE_CHANNEL_NUM))
$(error ERROR: Failed to change USB_AUDIO_SEND_CHAN($(USB_AUDIO_SEND_CHAN)))
endif
endif
endif

#export HFP_DISABLE_NREC ?= 0

ifeq ($(SCO_ADD_CUSTOMER_CODEC),1)
export SCO_ADD_CUSTOMER_CODEC
KBUILD_CPPFLAGS += -DSCO_ADD_CUSTOMER_CODEC
endif

ifeq ($(SPEECH_TX_24BIT),1)
export SPEECH_TX_24BIT
KBUILD_CPPFLAGS += -DSPEECH_TX_24BIT
endif

ifeq ($(SPEECH_TX_DC_FILTER),1)
export SPEECH_TX_DC_FILTER
KBUILD_CPPFLAGS += -DSPEECH_TX_DC_FILTER
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_MIC_CALIBRATION),1)
export SPEECH_TX_MIC_CALIBRATION
KBUILD_CPPFLAGS += -DSPEECH_TX_MIC_CALIBRATION
endif

ifeq ($(SPEECH_TX_MIC_FIR_CALIBRATION),1)
export SPEECH_TX_MIC_FIR_CALIBRATION
KBUILD_CPPFLAGS += -DSPEECH_TX_MIC_FIR_CALIBRATION
endif

#export SPEECH_TX_AEC_CODEC_REF ?= 0

ifeq ($(SPEECH_TX_AEC),1)
export SPEECH_TX_AEC
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC
export HFP_DISABLE_NREC := 1
ifeq ($(CHIP_HAS_EC_CODEC_REF),1)
export SPEECH_TX_AEC_CODEC_REF := 1
endif
endif

ifeq ($(SPEECH_TX_AEC2),1)
export SPEECH_TX_AEC2
$(error SPEECH_TX_AEC2 is not supported now, use SPEECH_TX_AEC2FLOAT instead)
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC2
export HFP_DISABLE_NREC := 1
ifeq ($(CHIP_HAS_EC_CODEC_REF),1)
export SPEECH_TX_AEC_CODEC_REF := 1
endif
endif

ifeq ($(SPEECH_TX_AEC3),1)
export SPEECH_TX_AEC3
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC3
export HFP_DISABLE_NREC := 1
ifeq ($(CHIP_HAS_EC_CODEC_REF),1)
export SPEECH_TX_AEC_CODEC_REF := 1
endif
endif

ifeq ($(SPEECH_TX_AEC2FLOAT),1)
export SPEECH_TX_AEC2FLOAT
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC2FLOAT
export HFP_DISABLE_NREC := 1
#export DSP_LIB ?= 1
ifeq ($(CHIP_HAS_EC_CODEC_REF),1)
export SPEECH_TX_AEC_CODEC_REF := 1
endif
endif

ifeq ($(SPEECH_TX_NS),1)
export SPEECH_TX_NS
KBUILD_CPPFLAGS += -DSPEECH_TX_NS
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_NN_NS),1)
export SPEECH_TX_NN_NS
KBUILD_CPPFLAGS += -DSPEECH_TX_NN_NS
export NN_LIB ?= 1
endif

ifeq ($(SPEECH_TX_NN_NS2),1)
export SPEECH_TX_NN_NS2
KBUILD_CPPFLAGS += -DSPEECH_TX_NN_NS2
export NN_LIB ?= 1
endif

ifeq ($(SPEECH_TX_NS2),1)
export SPEECH_TX_NS2
KBUILD_CPPFLAGS += -DSPEECH_TX_NS2
export HFP_DISABLE_NREC := 1
KBUILD_CPPFLAGS += -DLC_MMSE_FRAME_LENGTH=$(LC_MMSE_FRAME_LENGTH)
endif

ifeq ($(SPEECH_TX_NS2FLOAT),1)
export SPEECH_TX_NS2FLOAT
KBUILD_CPPFLAGS += -DSPEECH_TX_NS2FLOAT
export HFP_DISABLE_NREC := 1
#export DSP_LIB ?= 1
endif

ifeq ($(SPEECH_TX_NS3),1)
export SPEECH_TX_NS3
KBUILD_CPPFLAGS += -DSPEECH_TX_NS3
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_WNR),1)
export SPEECH_TX_WNR
KBUILD_CPPFLAGS += -DSPEECH_TX_WNR
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_CS_VAD),1)
export SPEECH_CS_VAD
KBUILD_CPPFLAGS += -DSPEECH_CS_VAD
export HFP_DISABLE_NREC := 1
endif

export SPEECH_CODEC_CAPTURE_CHANNEL_NUM ?= 1

#export SPEECH_TX_2MIC_SWAP_CHANNELS ?= 0

ifeq ($(SPEECH_TX_2MIC_NS),1)
export SPEECH_TX_2MIC_NS
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS
export HFP_DISABLE_NREC := 1
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_2MIC_NS2),1)
export SPEECH_TX_2MIC_NS2
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS2
export HFP_DISABLE_NREC := 1
KBUILD_CPPFLAGS += -DCOH_FRAME_LENGTH=$(COH_FRAME_LENGTH)
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_2MIC_NS3),1)
export SPEECH_TX_2MIC_NS3
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS3
export HFP_DISABLE_NREC := 1
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_2MIC_NS4),1)
export SPEECH_TX_2MIC_NS4
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS4
export HFP_DISABLE_NREC := 1

ifeq ($(SPEECH_BONE_SENSOR),1)
# Get 1 channel from sensor
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 1
else
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif
endif

ifeq ($(SPEECH_TX_2MIC_NS5),1)
export SPEECH_TX_2MIC_NS5
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS5
export HFP_DISABLE_NREC := 1
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_2MIC_NS6),1)
export SPEECH_TX_2MIC_NS6
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS6
export HFP_DISABLE_NREC := 1
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_2MIC_NS8),1)
export SPEECH_TX_2MIC_NS8
KBUILD_CPPFLAGS += -DSPEECH_TX_2MIC_NS8
KBUILD_CPPFLAGS += -DHFP_DISABLE_NREC
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
endif

ifeq ($(SPEECH_TX_3MIC_NS),1)
export SPEECH_TX_3MIC_NS
KBUILD_CPPFLAGS += -DSPEECH_TX_3MIC_NS
export HFP_DISABLE_NREC := 1
ifeq ($(SPEECH_BONE_SENSOR),1)
# Get 1 channel from sensor
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
else
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 3
endif
endif

ifeq ($(SPEECH_TX_3MIC_NS2),1)
export SPEECH_TX_3MIC_NS2
KBUILD_CPPFLAGS += -DSPEECH_TX_3MIC_NS2
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 3
KBUILD_CPPFLAGS += -DMCU_HIGH_PERFORMANCE_MODE
endif

ifeq ($(SPEECH_TX_3MIC_NS3),1)
export SPEECH_TX_3MIC_NS3
KBUILD_CPPFLAGS += -DSPEECH_TX_3MIC_NS3
export HFP_DISABLE_NREC := 1
# Get 1 channel from sensor
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 3
endif

ifeq ($(SPEECH_TX_THIRDPARTY_ALANGO),1)
export SPEECH_TX_THIRDPARTY_ALANGO
export SPEECH_TX_THIRDPARTY := 1
core-y += thirdparty/alango_lib/
endif

ifeq ($(SPEECH_TX_THIRDPARTY),1)
export SPEECH_TX_THIRDPARTY
KBUILD_CPPFLAGS += -DSPEECH_TX_THIRDPARTY
export HFP_DISABLE_NREC := 1
export SPEECH_CODEC_CAPTURE_CHANNEL_NUM = 2
ifeq ($(CHIP_HAS_EC_CODEC_REF),1)
export SPEECH_TX_AEC_CODEC_REF := 1
endif
endif

ifeq ($(SPEECH_TX_NOISE_GATE),1)
export SPEECH_TX_NOISE_GATE
KBUILD_CPPFLAGS += -DSPEECH_TX_NOISE_GATE
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_COMPEXP),1)
export SPEECH_TX_COMPEXP
KBUILD_CPPFLAGS += -DSPEECH_TX_COMPEXP
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_AGC),1)
export SPEECH_TX_AGC
KBUILD_CPPFLAGS += -DSPEECH_TX_AGC
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_TX_EQ),1)
export SPEECH_TX_EQ
KBUILD_CPPFLAGS += -DSPEECH_TX_EQ
export HFP_DISABLE_NREC := 1
#export DSP_LIB ?= 1
endif

ifeq ($(SPEECH_TX_POST_GAIN),1)
export SPEECH_TX_POST_GAIN
KBUILD_CPPFLAGS += -DSPEECH_TX_POST_GAIN
endif

ifneq ($(SCO_DMA_SNAPSHOT),1)
export SPEECH_TX_AEC_CODEC_REF := 0
endif

# disable codec ref when 2300a enable anc
ifeq ($(CHIP), best2300a)
ifeq ($(ANC_APP), 1)
export SPEECH_TX_AEC_CODEC_REF := 0
endif
endif

# disable codec ref when 1501 enable psap
ifeq ($(CHIP), best1501)
ifeq ($(PSAP_APP), 1)
export SPEECH_TX_AEC_CODEC_REF := 0
endif
endif

# disable codec ref when enable sidetone
ifeq ($(SPEECH_SIDETONE), 1)
export SPEECH_TX_AEC_CODEC_REF := 0
endif

ifeq ($(SPEECH_TX_AEC_CODEC_REF),1)
KBUILD_CPPFLAGS += -DSPEECH_TX_AEC_CODEC_REF
endif

ifeq ($(SPEECH_RX_NS),1)
export SPEECH_RX_NS
KBUILD_CPPFLAGS += -DSPEECH_RX_NS
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_NS2),1)
export SPEECH_RX_NS2
KBUILD_CPPFLAGS += -DSPEECH_RX_NS2
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_NS2FLOAT),1)
export SPEECH_RX_NS2FLOAT
KBUILD_CPPFLAGS += -DSPEECH_RX_NS2FLOAT
export HFP_DISABLE_NREC := 1
#export DSP_LIB ?= 1
endif

ifeq ($(SPEECH_RX_NS3),1)
export SPEECH_RX_NS3
KBUILD_CPPFLAGS += -DSPEECH_RX_NS3
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_AGC),1)
export SPEECH_RX_AGC
KBUILD_CPPFLAGS += -DSPEECH_RX_AGC
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_COMPEXP),1)
export SPEECH_RX_COMPEXP
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_EQ),1)
export SPEECH_RX_EQ
KBUILD_CPPFLAGS += -DSPEECH_RX_EQ
export HFP_DISABLE_NREC := 1
#export DSP_LIB ?= 1
endif

ifeq ($(SPEECH_RX_HW_EQ),1)
export SPEECH_RX_HW_EQ
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_DAC_EQ),1)
export SPEECH_RX_DAC_EQ
export HFP_DISABLE_NREC := 1
endif

ifeq ($(SPEECH_RX_POST_GAIN),1)
export SPEECH_RX_POST_GAIN
KBUILD_CPPFLAGS += -DSPEECH_RX_POST_GAIN
endif

ifeq ($(SPEECH_RX_EQ),1)
# enable 24bit to fix low level signal distortion
export SPEECH_RX_24BIT := 1
endif

export SPEECH_PROCESS_FRAME_MS 	?= 16
ifeq ($(SPEECH_CODEC_CAPTURE_CHANNEL_NUM),1)
export SPEECH_PROCESS_FRAME_MS := 15
endif
ifeq ($(SPEECH_TX_NN_NS),1)
export SPEECH_PROCESS_FRAME_MS := 16
endif
ifeq ($(SPEECH_TX_NN_NS2),1)
export SPEECH_PROCESS_FRAME_MS := 16
endif
ifeq ($(SPEECH_TX_2MIC_NS2),1)
export SPEECH_PROCESS_FRAME_MS := 15
endif
ifeq ($(SPEECH_TX_2MIC_NS4),1)
export SPEECH_PROCESS_FRAME_MS := 15
endif
ifeq ($(SPEECH_TX_2MIC_NS5),1)
export SPEECH_PROCESS_FRAME_MS := 15
endif
ifeq ($(SPEECH_TX_THIRDPARTY),1)
export SPEECH_PROCESS_FRAME_MS := 15
endif
KBUILD_CPPFLAGS += -DSPEECH_PROCESS_FRAME_MS=$(SPEECH_PROCESS_FRAME_MS)

export SPEECH_SCO_FRAME_MS 		?= 15
KBUILD_CPPFLAGS += -DSPEECH_SCO_FRAME_MS=$(SPEECH_SCO_FRAME_MS)

ifeq ($(SPEECH_SIDETONE),1)
export SPEECH_SIDETONE
KBUILD_CPPFLAGS += -DSPEECH_SIDETONE
ifeq ($(HW_SIDETONE_IIR_PROCESS),1)
ifeq ($(ANC_APP),1)
$(error ANC_APP conflicts with HW_SIDETONE_IIR_PROCESS)
endif
export HW_SIDETONE_IIR_PROCESS
KBUILD_CPPFLAGS += -DHW_SIDETONE_IIR_PROCESS
endif
ifeq ($(CHIP),best2000)
# Disable SCO resample
export SW_SCO_RESAMPLE := 0
export NO_SCO_RESAMPLE := 1
endif
endif

ifeq ($(THIRDPARTY_LIB),aispeech)
#export DSP_LIB ?= 1
endif

ifeq ($(THIRDPARTY_LIB),kws/bes)
export AQE_KWS := 1
endif

ifeq ($(AQE_KWS),1)
export AQE_KWS
export NN_LIB := 1
KBUILD_CPPFLAGS += -DAQE_KWS
KBUILD_CPPFLAGS += -DAQE_KWS_$(AQE_KWS_NAME)
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

ifeq ($(AUDIO_HEARING_COMPSATN),1)
export AUDIO_HEARING_COMPSATN
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
KBUILD_CPPFLAGS += -DAUDIO_HEARING_COMPSATN
KBUILD_CPPFLAGS += -DHEARING_MOD_VAL=$(HEARING_MOD_VAL)
endif

ifeq ($(HEARING_USE_STATIC_RAM),1)
KBUILD_CPPFLAGS += -DHEARING_USE_STATIC_RAM
endif

ifeq ($(AUDIO_CUSTOM_EQ),1)
export AUDIO_CUSTOM_EQ
export USE_CMSIS_FFT_LEN_1024 := 1
KBUILD_CPPFLAGS += -DGLOBAL_SRAM_CMSIS_FFT
endif

ifeq ($(USE_CMSIS_FFT_LEN_1024),1)
KBUILD_CPPFLAGS += -DARM_TABLE_TWIDDLECOEF_F32_512
KBUILD_CPPFLAGS += -DARM_TABLE_TWIDDLECOEF_RFFT_F32_1024
KBUILD_CPPFLAGS += -DARM_TABLE_BITREVIDX_FLT_512
endif

KBUILD_CPPFLAGS += -DSPEECH_CODEC_CAPTURE_CHANNEL_NUM=$(SPEECH_CODEC_CAPTURE_CHANNEL_NUM)
KBUILD_CPPFLAGS += -DSPEECH_ASR_CAPTURE_CHANNEL_NUM=$(SPEECH_ASR_CAPTURE_CHANNEL_NUM)

endif # SPEECH_LIB

# -------------------------------------------
# Common features
# -------------------------------------------

export DEBUG_PORT ?= 1

ifneq ($(filter best1000 best2000,$(CHIP)),)
export AUD_SECTION_STRUCT_VERSION ?= 1
else
export AUD_SECTION_STRUCT_VERSION ?= 2
endif
ifneq ($(AUD_SECTION_STRUCT_VERSION),)
KBUILD_CPPFLAGS += -DAUD_SECTION_STRUCT_VERSION=$(AUD_SECTION_STRUCT_VERSION)
endif

ifneq ($(FLASH_CHIP),)
VALID_FLASH_CHIP_LIST := ALL SIMU \
	GD25LE255E GD25LE128E GD25LQ64C GD25LQ32C GD25LQ16C GD25LQ80C GD25Q32C GD25Q80C GD25Q40C GD25D20C \
	P25Q256L P25Q128L P25Q64L P25Q32L P25Q16L P25Q80H P25Q40H P25Q21H P25Q32SL P25Q16SL \
	ZB25VQ128B \
	XM25QU128C XM25QH16C XM25QH80B \
	XT25Q08B \
	EN25S80B \
	W25Q128JW W25Q32FW W25Q128JV\
	SK25LE032
export FLASH_CHIP_LIST := $(subst $(comma),$(space),$(FLASH_CHIP))
ifneq ($(filter-out $(VALID_FLASH_CHIP_LIST),$(FLASH_CHIP_LIST)),)
$(error Invalid FLASH_CHIP: $(filter-out $(VALID_FLASH_CHIP_LIST),$(FLASH_CHIP_LIST)))
endif
endif

ifneq ($(NANDFLASH_CHIP),)
export NANDFLASH_CHIP_LIST := $(subst $(comma),$(space),$(NANDFLASH_CHIP))
endif


ifeq ($(KERNEL),LITEOS_M)
export FAULT_DUMP ?= 0
else ifeq ($(KERNEL),LITEOS_A)
export FAULT_DUMP ?= 0
else
export FAULT_DUMP ?= 1
endif

export CODEC_KEEP_CLOCK ?= 0
ifeq ($(CODEC_KEEP_CLOCK),1)
export CODEC_POWER_DOWN := 0
else
export CODEC_POWER_DOWN ?= 1
endif

export AUDIO_OUTPUT_VOLUME_DEFAULT ?= 10
KBUILD_CPPFLAGS += -DAUDIO_OUTPUT_VOLUME_DEFAULT=$(AUDIO_OUTPUT_VOLUME_DEFAULT)

NV_REC_DEV_VER ?= 2

ifeq ($(NO_PWRKEY),1)
export NO_PWRKEY
endif

ifeq ($(NO_GROUPKEY),1)
export NO_GROUPKEY
endif

ifeq ($(NO_SLEEP),1)
export NO_SLEEP
endif

ifeq ($(CRASH_BOOT),1)
export CRASH_BOOT
endif

ifeq ($(USB_HIGH_SPEED),1)
export USB_HIGH_SPEED
endif

ifeq ($(AUDIO_CODEC_ASYNC_CLOSE),1)
export AUDIO_CODEC_ASYNC_CLOSE
endif

ifeq ($(CODEC_PLAY_BEFORE_CAPTURE),1)
# Enable the workaround for BEST1000 version C & earlier chips
export CODEC_PLAY_BEFORE_CAPTURE
endif

ifeq ($(AUDIO_INPUT_CAPLESSMODE),1)
export AUDIO_INPUT_CAPLESSMODE
endif

ifeq ($(AUDIO_INPUT_LARGEGAIN),1)
export AUDIO_INPUT_LARGEGAIN
endif

ifeq ($(AUDIO_INPUT_MONO),1)
export AUDIO_INPUT_MONO
endif

ifeq ($(AUDIO_OUTPUT_MONO),1)
export AUDIO_OUTPUT_MONO
endif

ifeq ($(AUDIO_OUTPUT_INVERT_RIGHT_CHANNEL),1)
export AUDIO_OUTPUT_INVERT_RIGHT_CHANNEL
endif

ifeq ($(AUDIO_OUTPUT_CALIB_GAIN_MISSMATCH),1)
export AUDIO_OUTPUT_CALIB_GAIN_MISSMATCH
endif

ifeq ($(CODEC_DAC_MULTI_VOLUME_TABLE),1)
export CODEC_DAC_MULTI_VOLUME_TABLE
endif

ifeq ($(AUDIO_OUTPUT_DAC2_SW_GAIN),1)
export AUDIO_OUTPUT_DAC2_SW_GAIN
endif

ifeq ($(AUDIO_OUTPUT_DAC2),1)
export AUDIO_OUTPUT_DAC2
endif

ifeq ($(DAC_CLASSG_ENABLE),1)
export DAC_CLASSG_ENABLE
endif

ifeq ($(HW_FIR_DSD_PROCESS),1)
export HW_FIR_DSD_PROCESS
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
export HW_FIR_EQ_PROCESS
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
export HW_IIR_EQ_PROCESS
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
export HW_DAC_IIR_EQ_PROCESS
endif

ifeq ($(AUDIO_DRC),1)
export AUDIO_DRC
export AUDIO_OUTPUT_SW_GAIN ?= 1
export AUDIO_OUTPUT_SW_GAIN_BEFORE_DRC ?= 1
endif

ifeq ($(AUDIO_LIMITER),1)
export AUDIO_LIMITER
export AUDIO_OUTPUT_SW_GAIN ?= 1
export AUDIO_OUTPUT_SW_GAIN_BEFORE_DRC ?= 1
endif

ifeq ($(AUDIO_REVERB),1)
export AUDIO_REVERB
endif

ifeq ($(AUDIO_HW_LIMITER),1)
export AUDIO_HW_LIMITER
endif

ifeq ($(HW_FIR_EQ_PROCESS_2CH),1)
export HW_FIR_EQ_PROCESS_2CH
KBUILD_CPPFLAGS += -D__HW_FIR_EQ_PROCESS_2CH__
endif

ifeq ($(NO_ISPI),1)
KBUILD_CPPFLAGS += -DNO_ISPI
endif

ifeq ($(USER_SECURE_BOOT),1)
export BOOT_LOADER_POST_INIT_HOOK := 1
core-y += utils/user_secure_boot/ \
               utils/system_info/
KBUILD_CPPFLAGS += -DUSER_SECURE_BOOT
endif

ifeq ($(ASSERT_SHOW_FILE_FUNC),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FILE_FUNC
else
ifeq ($(ASSERT_SHOW_FILE),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FILE
else
ifeq ($(ASSERT_SHOW_FUNC),1)
KBUILD_CPPFLAGS += -DASSERT_SHOW_FUNC
endif
endif
endif

ifeq ($(LARGE_SENS_RAM),1)
KBUILD_CPPFLAGS += -DLARGE_SENS_RAM
endif

ifeq ($(TWS),1)
LARGE_RAM ?= 1
endif
ifeq ($(LARGE_RAM),1)
ifeq ($(DSP_ENABLE), 1)
$(error LARGE_RAM conflicts with DSP_ENABLE)
endif
KBUILD_CPPFLAGS += -DLARGE_RAM
endif

ifeq ($(LARGE_DSP_RAM),1)
KBUILD_CPPFLAGS += -DLARGE_DSP_RAM
endif

ifeq ($(SMALL_RET_RAM),1)
KBUILD_CPPFLAGS += -DSMALL_RET_RAM
endif

ifeq ($(SIMPLE_TASK_SWITCH),1)
KBUILD_CPPFLAGS += -DSIMPLE_TASK_SWITCH
endif

ifeq ($(ALT_BOOT_FLASH),1)
KBUILD_CPPFLAGS += -DALT_BOOT_FLASH
endif

ifeq ($(USB_AUDIO_APP),1)
ifneq ($(BTUSB_AUDIO_MODE),1)
export NO_OVERLAY ?= 1
endif
endif
ifeq ($(NO_OVERLAY),1)
KBUILD_CPPFLAGS +=  -DNO_OVERLAY
endif

ifeq ($(CALIB_SLOW_TIMER),1)
KBUILD_CPPFLAGS += -DCALIB_SLOW_TIMER
endif

ifneq ($(CHIP_SUBSYS),dsp)
export INT_LOCK_EXCEPTION ?= 1
endif

ifeq ($(INT_LOCK_EXCEPTION),1)
KBUILD_CPPFLAGS += -DINT_LOCK_EXCEPTION
endif

ifeq ($(USE_TRACE_ID),1)
export USE_TRACE_ID
export TRACE_STR_SECTION ?= 1
KBUILD_CPPFLAGS += -DUSE_TRACE_ID
endif

ifeq ($(TRACE_STR_SECTION),1)
KBUILD_CPPFLAGS += -DTRACE_STR_SECTION
CPPFLAGS_${LDS_FILE} += -DTRACE_STR_SECTION
endif

ifeq ($(ANC_APP),1)
export ANC_FF_ENABLED ?= 1
ifeq ($(ANC_FB_CHECK),1)
KBUILD_CPPFLAGS += -DANC_FB_CHECK
endif
ifeq ($(ANC_FF_CHECK),1)
KBUILD_CPPFLAGS += -DANC_FF_CHECK
endif
endif

ifeq ($(PSAP_APP),1)
KBUILD_CPPFLAGS += -DPSAP_APP
# KBUILD_CPPFLAGS += -DPSAP_FORCE_STREAM_48K
endif

ifeq ($(AUDIO_PSAP_DEHOWLING_HW),1)
KBUILD_CPPFLAGS += -DAUDIO_PSAP_DEHOWLING_HW
endif

ifeq ($(AUDIO_ANC_SPKCALIB_HW),1)
KBUILD_CPPFLAGS += -DAUDIO_ANC_SPKCALIB_HW
endif

ifeq ($(CHIP),best1000)
ifeq ($(AUD_PLL_DOUBLE),1)
KBUILD_CPPFLAGS += -DAUD_PLL_DOUBLE
endif

ifeq ($(DUAL_AUX_MIC),1)
ifeq ($(AUDIO_INPUT_MONO),1)
$(error Invalid talk mic configuration)
endif
KBUILD_CPPFLAGS += -D_DUAL_AUX_MIC_
endif
endif # best1000

ifeq ($(CAPTURE_ANC_DATA),1)
KBUILD_CPPFLAGS += -DCAPTURE_ANC_DATA
endif

ifeq ($(AUDIO_ANC_TT_HW),1)
KBUILD_CPPFLAGS += -DAUDIO_ANC_TT_HW
endif

ifeq ($(AUDIO_ANC_FB_MC_HW),1)
KBUILD_CPPFLAGS += -DAUDIO_ANC_FB_MC_HW
endif

ifeq ($(AUDIO_ANC_FB_MC),1)
ifeq ($(AUDIO_RESAMPLE),1)
$(error AUDIO_ANC_FB_MC conflicts with AUDIO_RESAMPLE)
endif
KBUILD_CPPFLAGS += -DAUDIO_ANC_FB_MC
ifeq ($(ANC_FB_MC_96KHZ),1)
KBUILD_CPPFLAGS += -DANC_FB_MC_96KHZ
endif
endif

ifeq ($(ANC_NOISE_TRACKER),1)
export ANC_NOISE_TRACKER
ifeq ($(IBRT),1)
KBUILD_CPPFLAGS += -DANC_NOISE_TRACKER_CHANNEL_NUM=1
else
KBUILD_CPPFLAGS += -DANC_NOISE_TRACKER_CHANNEL_NUM=2
endif
endif

export PC_CMD_UART ?= 0
ifneq ($(PC_CMD_UART),0)
KBUILD_CPPFLAGS += -D__PC_CMD_UART__=$(PC_CMD_UART)
KBUILD_CPPFLAGS += -DAUDIO_EQ_TUNING
endif

ifeq ($(TOTA_EQ_TUNING),1)
export TOTA_EQ_TUNING
endif

ifeq ($(VOICE_DETECTOR_EN),1)
KBUILD_CPPFLAGS += -DVOICE_DETECTOR_EN
endif

ifeq ($(VOICE_DETECTOR_SENS_EN),1)
KBUILD_CPPFLAGS += -DVOICE_DETECTOR_SENS_EN
endif

ifneq ($(CODEC_VAD_CFG_BUF_SIZE),)
KBUILD_CPPFLAGS += -DCODEC_VAD_CFG_BUF_SIZE=$(CODEC_VAD_CFG_BUF_SIZE)
endif

ifeq ($(VAD_USE_SAR_ADC),1)
KBUILD_CPPFLAGS += -DVAD_USE_SAR_ADC
endif

ifeq ($(VAD_USE_8K_SAMPLE_RATE),1)
KBUILD_CPPFLAGS += -DVAD_USE_8K_SAMPLE_RATE
endif

ifeq ($(USB_ANC_MC_EQ_TUNING),1)
KBUILD_CPPFLAGS += -DUSB_ANC_MC_EQ_TUNING -DANC_PROD_TEST
endif

ifeq ($(MAX_DAC_OUTPUT),-60db)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_M60DB
else
ifeq ($(MAX_DAC_OUTPUT),3.75mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_3P75MW
else
ifeq ($(MAX_DAC_OUTPUT),5mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_5MW
else
ifeq ($(MAX_DAC_OUTPUT),10mw)
MAX_DAC_OUTPUT_FLAGS := -DMAX_DAC_OUTPUT_10MW
else
ifneq ($(MAX_DAC_OUTPUT),30mw)
ifneq ($(MAX_DAC_OUTPUT),)
$(error Invalid MAX_DAC_OUTPUT value: $(MAX_DAC_OUTPUT) (MUST be one of: -60db 3.75mw 5mw 10mw 30mw))
endif
endif
endif
endif
endif
endif
export MAX_DAC_OUTPUT_FLAGS

export SUPPORT_REMOTE_COD ?= 0
ifeq ($(SUPPORT_REMOTE_COD),1)
KBUILD_CPPFLAGS += -D_SUPPORT_REMOTE_COD_
endif

export HAS_BT_SYNC ?= 0
ifeq ($(HAS_BT_SYNC),1)
KBUILD_CPPFLAGS += -D__BT_SYNC__
endif

ifeq ($(SINGLE_WIRE_UART_PMU_1802),1)
KBUILD_CPPFLAGS += -DSINGLE_WIRE_UART_PMU_1802
endif

ifeq ($(SINGLE_WIRE_UART_PMU_1803),1)
KBUILD_CPPFLAGS += -DSINGLE_WIRE_UART_PMU_1803
endif

ifeq ($(FUZZ_TEST_SUPPORT),1)
KBUILD_CPPFLAGS += -D__FUZZ_TEST_SUPPORT__
endif

ifeq ($(INTERSYS_RX_DMA_SPEEDUP),1)
CPPFLAGS_${LDS_FILE} += -DINTERSYS_RX_DMA_SPEEDUP
endif

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Put customized features above
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# Obsoleted features
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
OBSOLETED_FEATURE_LIST := EQ_PROCESS RB_CODEC AUDIO_EQ_PROCESS MEDIA_PLAYER_RBCODEC
USED_OBSOLETED_FEATURE := $(strip $(foreach f,$(OBSOLETED_FEATURE_LIST),$(if $(filter 1,$($f)),$f)))
ifneq ($(USED_OBSOLETED_FEATURE),)
$(error Obsoleted features: $(USED_OBSOLETED_FEATURE))
endif

# -------------------------------------------
# Size and LDS macros
# -------------------------------------------

ifneq ($(ROM_SIZE),)
KBUILD_CPPFLAGS += -DROM_SIZE=$(ROM_SIZE)
endif

ifneq ($(RAM_SIZE),)
KBUILD_CPPFLAGS += -DRAM_SIZE=$(RAM_SIZE)
endif

ifneq ($(BT_RAMRUN_SIZE),)
KBUILD_CPPFLAGS += -DBT_RAMRUN_SIZE=$(BT_RAMRUN_SIZE)
endif

ifneq ($(BT_RAMRUN_BASE),)
KBUILD_CPPFLAGS += -DBT_RAMRUN_BASE=$(BT_RAMRUN_BASE)
endif

ifneq ($(RAMCP_BASE),)
KBUILD_CPPFLAGS += -DRAMCP_BASE=$(RAMCP_BASE)
endif

ifneq ($(RAMCP_SIZE),)
KBUILD_CPPFLAGS += -DRAMCP_SIZE=$(RAMCP_SIZE)
endif

ifneq ($(RAMCPX_BASE),)
KBUILD_CPPFLAGS += -DRAMCPX_BASE=$(RAMCPX_BASE)
endif

ifneq ($(RAMCPX_SIZE),)
KBUILD_CPPFLAGS += -DRAMCPX_SIZE=$(RAMCPX_SIZE)
endif

export DSP_RAM_SIZE ?= 0
ifneq ($(DSP_RAM_SIZE),)
KBUILD_CPPFLAGS += -DDSP_RAM_SIZE=$(DSP_RAM_SIZE)
endif

export FLASH_SIZE ?= 0x100000
KBUILD_CPPFLAGS += -DFLASH_SIZE=$(FLASH_SIZE)

ifeq ($(CHIP_HAS_PSRAM),1)
export PSRAM_SIZE ?= 0x400000
export PSRAM_ENABLE ?= 0
export PSRAM_SPEED ?= 200
#166:166M, 200:200M, 240:240M
endif
ifneq ($(PSRAM_SIZE),)
KBUILD_CPPFLAGS += -DPSRAM_SIZE=$(PSRAM_SIZE)
endif

ifeq ($(CHIP_HAS_PSRAMUHS),1)
export PSRAMUHS_ENABLE ?= 0
export PSRAMUHS_SPEED ?= 1000
#400:400M, 600:600M, 800:800M, 900:900M, 1000:1000M, 1066:1066M
ifeq ($(PSRAMUHS_DUAL_8BIT), 1)
export PSRAMUHS_SIZE ?= 0x1000000
ifeq ($(PSRAMUHS_DUAL_SWITCH), 1)
$(error PSRAMUHS_DUAL_8BIT conflicts with PSRAMUHS_DUAL_SWITCH)
endif
endif
export PSRAMUHS_SIZE ?= 0x800000
endif

# NOTE: This size cannot be changed so that audio section address is fixed.
#       This rule can be removed once audio tool can set audio section address dynamically.
FACTORY_SECTION_SIZE ?= 0x1000

# NOTE: This size cannot be changed so that audio section address is fixed.
#       This rule can be removed once audio tool can set audio section address dynamically.
RESERVED_SECTION_SIZE ?= 0x1000
ifeq ($(CHIP), best1501simu)
ifeq ($(ASIC_SIMU),1)
RESERVED_SECTION_SIZE := 0x32000
endif
endif

ifeq ($(AUDIO_SECTION_ENABLE),1)
KBUILD_CPPFLAGS += -DAUDIO_SECTION_ENABLE
# depend on length of (ANC + AUDIO + SPEECH) in aud_section.c
AUD_SECTION_SIZE ?= 0x8000
ifeq ($(ANC_APP),1)
$(error Can not enable AUDIO_SECTION_ENABLE and ANC_APP together)
endif
endif

ifeq ($(ANC_APP),1)
ifeq ($(CHIP),best1000)
AUD_SECTION_SIZE ?= 0x8000
else
AUD_SECTION_SIZE ?= 0x10000
endif
else
AUD_SECTION_SIZE ?= 0
endif

USERDATA_SECTION_SIZE ?= 0x1000

export PERSIST_DATA_SECTION_SIZE ?= 0
KBUILD_CPPFLAGS += -DPERSIST_DATA_SECTION_SIZE=$(PERSIST_DATA_SECTION_SIZE)

CUSTOM_PARAMETER_SECTION_SIZE ?= 0x1000

ifeq ($(ENABLE_CALCU_CPU_FREQ_LOG),1)
export ENABLE_CALCU_CPU_FREQ_LOG
endif

ifeq ($(TOTA_CRASH_DUMP_TOOL_ENABLE),1)
export TOTA := 1
CRASH_DUMP_SECTION_SIZE ?= 0x1000
LOG_DUMP_SECTION_SIZE ?= 0x28000
#export DUMP_CRASH_LOG := 1
KBUILD_CPPFLAGS += -DTOTA_CRASH_DUMP_TOOL_ENABLE
endif

ifeq ($(DUMP_CRASH_LOG),1)
CRASH_DUMP_SECTION_SIZE ?= 0x4000
KBUILD_CPPFLAGS += -DDUMP_CRASH_LOG
else
CRASH_DUMP_SECTION_SIZE ?= 0
endif

ifeq ($(A2DP_LHDC_ON),1)
LHDC_LICENSE_SECTION_SIZE ?= 0x1000
else
LHDC_LICENSE_SECTION_SIZE ?= 0
endif

export DUMP_LOG_ENABLE ?= 0
ifeq ($(DUMP_LOG_ENABLE),1)
ifeq ($(FLASH_SIZE),0x40000) # 2M bits
LOG_DUMP_SECTION_SIZE ?= 0x4000
endif
ifeq ($(FLASH_SIZE),0x80000) # 4M bits
LOG_DUMP_SECTION_SIZE ?= 0x8000
endif
ifeq ($(FLASH_SIZE),0x100000) # 8M bits
LOG_DUMP_SECTION_SIZE ?= 0x10000
endif
ifeq ($(FLASH_SIZE),0x200000) # 16M bits
LOG_DUMP_SECTION_SIZE ?= 0x80000
endif
ifeq ($(FLASH_SIZE),0x400000) # 32M bits
LOG_DUMP_SECTION_SIZE ?= 0x200000
endif
ifeq ($(FLASH_SIZE),0x800000) # 64M bits
LOG_DUMP_SECTION_SIZE ?= 0x400000
endif
KBUILD_CPPFLAGS += -DDUMP_LOG_ENABLE
else
LOG_DUMP_SECTION_SIZE ?= 0
endif

ifeq ($(OTA_ENABLE),1)
OTA_UPGRADE_LOG_SIZE ?= 0x1000
else
OTA_UPGRADE_LOG_SIZE ?= 0
endif

ifeq ($(CORE_DUMP_TO_FLASH),1)
CORE_DUMP_SECTION_SIZE ?= 0x100000
KBUILD_CPPFLAGS += -DCORE_DUMP_TO_FLASH
else
CORE_DUMP_SECTION_SIZE ?= 0
endif

ifeq ($(GSOUND_HOTWORD_ENABLE),1)
# used to store hotword model, currently 240KB
# this value is used in link file
HOTWORD_SECTION_SIZE ?= 0x3C000
else
HOTWORD_SECTION_SIZE ?= 0x0
endif

ifeq ($(PROMPT_IN_FLASH),1)
# 300K for prompt package
PROMPT_SECTION_SIZE ?= 0x4B000
else
PROMPT_SECTION_SIZE ?= 0x0
endif

ifeq ($(COMBO_CUSBIN_IN_FLASH),1)
export COMBO_CUSBIN_IN_FLASH
COMBO_SECTION_SIZE ?= 0x1000
else
COMBO_SECTION_SIZE ?= 0
endif

export LDS_SECTION_FLAGS := \
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
	-DPERSIST_DATA_SECTION_SIZE=$(PERSIST_DATA_SECTION_SIZE) \
	-DAUD_SECTION_SIZE=$(AUD_SECTION_SIZE) \
	-DRESERVED_SECTION_SIZE=$(RESERVED_SECTION_SIZE) \
	-DFACTORY_SECTION_SIZE=$(FACTORY_SECTION_SIZE) \
	-DFILE_SYSTEM_SECTION_SIZE=$(FILE_SYSTEM_SECTION_SIZE)

CPPFLAGS_${LDS_FILE} += \
	-DLINKER_SCRIPT \
	-DFLASH_SIZE=$(FLASH_SIZE) \
	-Iplatform/hal

CPPFLAGS_${LDS_FILE} += $(LDS_SECTION_FLAGS)

ifneq ($(PSRAM_SIZE),)
CPPFLAGS_${LDS_FILE} +=-DPSRAM_SIZE=$(PSRAM_SIZE)
endif

ifneq ($(PSRAMCP_SIZE),)
CPPFLAGS_${LDS_FILE} +=-DPSRAMCP_SIZE=$(PSRAMCP_SIZE)
endif

ifneq ($(PSRAMUHS_SIZE),)
CPPFLAGS_${LDS_FILE} +=-DPSRAMUHS_SIZE=$(PSRAMUHS_SIZE)
endif

ifneq ($(RAMV_SIZE),)
CPPFLAGS_${LDS_FILE} +=-DRAMV_SIZE=$(RAMV_SIZE)
endif

ifneq ($(OTA_BOOT_SIZE),)
export OTA_BOOT_SIZE
export OTA_BOOT_OFFSET ?= 0
CPPFLAGS_${LDS_FILE} += -DOTA_BOOT_OFFSET=$(OTA_BOOT_OFFSET) -DOTA_BOOT_SIZE=$(OTA_BOOT_SIZE)
endif

ifneq ($(OTA_CODE_OFFSET),)
export OTA_CODE_OFFSET
CPPFLAGS_${LDS_FILE} += -DOTA_CODE_OFFSET=$(OTA_CODE_OFFSET)
endif

ifneq ($(OTA_REMAP_OFFSET),)
export OTA_REMAP_OFFSET
CPPFLAGS_${LDS_FILE} += -DOTA_REMAP_OFFSET=$(OTA_REMAP_OFFSET)
ifeq ($(OTA_CODE_OFFSET),)
$(error OTA_CODE_OFFSET should be set along with OTA_REMAP_OFFSET)
endif
endif

ifeq ($(FLASH_REMAP),1)
export FLASH_REMAP
ifneq ($(PROGRAMMER),1)
ifeq ($(OTA_REMAP_OFFSET),)
$(error OTA_REMAP_OFFSET should be set along with FLASH_REMAP)
endif
endif
endif

ifneq ($(FLASH_REGION_SIZE),)
CPPFLAGS_${LDS_FILE} += -DFLASH_REGION_SIZE=$(FLASH_REGION_SIZE)
endif

ifneq ($(SLAVE_BIN_FLASH_OFFSET),)
export SLAVE_BIN_FLASH_OFFSET
CPPFLAGS_${LDS_FILE} += -DSLAVE_BIN_FLASH_OFFSET=$(SLAVE_BIN_FLASH_OFFSET)
endif

ifeq ($(BOOT_CODE_IN_RAM),1)
CPPFLAGS_${LDS_FILE} += -DBOOT_CODE_IN_RAM
endif

ifeq ($(GSOUND_HOTWORD_EXTERNAL),1)
CPPFLAGS_${LDS_FILE} += -DGSOUND_HOTWORD_EXTERNAL
endif

ifeq ($(MEM_POOL_IN_CP_RAM),1)
CPPFLAGS_${LDS_FILE} += -DMEM_POOL_IN_CP_RAM
endif

ifeq ($(PROGRAMMER_LOAD_SIMU),1)
CPPFLAGS_${LDS_FILE} += -DPROGRAMMER_LOAD_SIMU
endif

ifeq ($(RB_CODEC),1)
CPPFLAGS_${LDS_FILE} += -DRB_CODEC
endif

ifneq ($(DATA_BUF_START),)
CPPFLAGS_${LDS_FILE} += -DDATA_BUF_START=$(DATA_BUF_START)
endif

ifeq ($(BTHCI_H4),1)
KBUILD_CFLAGS += -DBTHCI_H4
endif

ifeq ($(BT_CLOSE),1)
KBUILD_CFLAGS	+= -D__BT_CLOSE__
endif

ifeq ($(ALSA_WIFI_TO_BT),1)
KBUILD_CFLAGS	+= -DALSA_WIFI_TO_BT
endif

ifeq ($(USE_BT_ADAPTER),1)
KBUILD_CFLAGS	+= -DUSE_BT_ADAPTER
KBUILD_CFLAGS	+= -DBLE_ADAPTER_PROFILES_NUM=10
KBUILD_CFLAGS	+= -DBLE_ADAPTER_CLIENT_NUM=1
KBUILD_CFLAGS	+= -DBLE_ADAPTER_ATTB_NUM=16
else
KBUILD_CFLAGS	+= -DBLE_ADAPTER_PROFILES_NUM=0
KBUILD_CFLAGS	+= -DBLE_ADAPTER_CLIENT_NUM=0
KBUILD_CFLAGS	+= -DBLE_ADAPTER_ATTB_NUM=0
endif

ifeq ($(VAD_IN_BACKGROUND),1)
KBUILD_CFLAGS	+= -DVAD_IN_BACKGROUND
endif

export LC3_AUDIO_MIXED_MONO ?= 1
ifeq ($(LC3_AUDIO_MIXED_MONO),1)
CFLAGS_bap_lc3.o += -DLC3_AUDIO_MIXED_MONO
endif

# -------------------------------------------
# General
# -------------------------------------------

ifneq ($(NO_CONFIG),1)
core-y += config/
endif

ifneq ($(NO_BOOT_STRUCT),1)
core-y += $(call add_if_exists,utils/boot_struct/)
endif

export DEFAULT_CFG_SRC ?= _default_cfg_src_

ifneq ($(wildcard $(srctree)/config/$(T)/tgt_hardware.h $(srctree)/config/$(T)/res/),)
KBUILD_CPPFLAGS += -Iconfig/$(T)
endif
KBUILD_CPPFLAGS += -Iconfig/$(DEFAULT_CFG_SRC)

CPU_EXT :=
ifeq ($(CPU_NO_DSP),1)
CPU_EXT := $(CPU_EXT)+nodsp
endif
ifneq ($(CHIP_HAS_FPU),1)
CPU_EXT := $(CPU_EXT)+nofp
endif

ifneq ($(CPU),a7)
CPU_CFLAGS := -mthumb
endif

ifeq ($(CPU),a7)
CPU_CFLAGS := -marm
CPU_CFLAGS += -mcpu=cortex-a7
else ifeq ($(CPU),m55)
CPU_CFLAGS += -mcpu=cortex-m55
else ifeq ($(CPU),m33)
CPU_CFLAGS += -mcpu=cortex-m33$(CPU_EXT)
ifeq ($(ARM_CMSE),1)
export ARM_CMSE
CPU_CFLAGS += -mcmse
KBUILD_CPPFLAGS += -DARM_CMSE
endif
ifeq ($(ARM_CMNS),1)
export ARM_CMNS
export ARM_CMSE_TARGET ?= cm33_se
KBUILD_CPPFLAGS += -DARM_CMNS
ifeq ($(LIBC_ROM),1)
$(error LIBC_ROM should be 0 when ARM_CMNS=1)
endif
endif
else
CPU_CFLAGS += -mcpu=cortex-m4$(CPU_EXT)
endif

ifeq ($(CHIP_HAS_FPU),1)
ifeq ($(CPU),a7)
CPU_CFLAGS += -mfpu=neon-vfpv4
else ifeq ($(CPU),m33)
CPU_CFLAGS += -mfpu=fpv5-sp-d16
else ifeq ($(CPU),m55)
# For GCC version >= 9
CPU_CFLAGS += -mfpu=auto
else
CPU_CFLAGS += -mfpu=fpv4-sp-d16
endif
ifeq ($(SOFT_FLOAT_ABI),1)
CPU_CFLAGS += -mfloat-abi=softfp
else
CPU_CFLAGS += -mfloat-abi=hard
endif
else # !CHIP_HAS_FPU
CPU_CFLAGS += -mfloat-abi=soft
endif # !CHIP_HAS_FPU

ifneq ($(CPU),a7)
ifeq ($(KERNEL),RTT)
CPU_CFLAGS += -Wa,-mimplicit-it=thumb
endif

ifeq ($(KERNEL),RHINO)
CPU_CFLAGS += -Wa,-mimplicit-it=thumb
endif
endif

export UNALIGNED_ACCESS ?= 1
ifeq ($(UNALIGNED_ACCESS),1)
KBUILD_CPPFLAGS += -DUNALIGNED_ACCESS
else
CPU_CFLAGS += -mno-unaligned-access
endif

ifneq ($(ALLOW_WARNING),1)
KBUILD_CPPFLAGS += -Werror
endif

ifeq ($(NO_STARTFILES), 1)
LIB_LDFLAGS += -nostartfiles
endif

ifeq ($(STACK_USAGE),1)
KBUILD_CPPFLAGS += -fstack-usage
endif

ifeq ($(PIE),1)
ifneq ($(TOOLCHAIN),armclang)
ifneq ($(NOSTD),1)
$(error PIE can only work when NOSTD=1)
endif
KBUILD_CPPFLAGS += -msingle-pic-base
endif
KBUILD_CPPFLAGS += -fPIE
# -pie option will generate .dynamic section
#LDFLAGS += -pie
#LDFLAGS += -z relro -z now
endif

KBUILD_CPPFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS)
LINK_CFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS)
CFLAGS_IMAGE += $(CPU_CFLAGS) $(SPECS_CFLAGS)

# Save 100+ bytes by filling less alignment holes
# TODO: Array alignment?
#LDFLAGS += --sort-common --sort-section=alignment

ifeq ($(CTYPE_PTR_DEF),1)
ifeq ($(TOOLCHAIN),armclang)
$(error CTYPE_PTR_DEF is not supported in $(TOOLCHAIN))
else
LDFLAGS_IMAGE += --defsym __ctype_ptr__=0
endif
endif

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
export NET_FTP_APP_SUPPORT := 1
endif

export AT_CMD ?= 1
ifeq ($(AT_CMD),1)
KBUILD_CPPFLAGS += -D__AT_CMD_SUPPORT__
endif

export HW_MODULE ?= 0
KBUILD_CPPFLAGS += -DHW_MODULE=$(HW_MODULE)

ifeq ($(SDK),1)
KBUILD_CPPFLAGS += -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function \
                   -Wno-unused-value -Wno-unused-parameter -Wno-unused-label #-Werror
ifneq ($(KERNEL),RHINO)
KBUILD_CPPFLAGS += -Wimplicit-fallthrough=1
endif
endif
export LIB_EXT_SUFFIX_FLG ?= -$(FLASH_SIZE)