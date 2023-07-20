cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y += ../../tests/rom/pmu_rom.c
obj-y += ../../tests/rom/startup_ARMCM.S

ifneq ($(CHIP_HAS_USB),1)
obj-y := $(filter-out usb_dev_test.c usb_host_test.c,$(obj-y))
endif

ifeq ($(CHIP_SUBSYS),dsp)
obj-y := $(filter-out wifi_test.c,$(obj-y))
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hwtimer_list/

obj-y += ../../utils/hexdump/

obj-y += ../../platform/drivers/ana/

ifeq ($(CHIP_HAS_USB),1)
obj-y += ../../platform/drivers/usb/usb_dev/
ccflags-y += -Iplatform/drivers/usb/usb_dev/inc
endif

ccflags-y += -Itests/rom -Iutils/hwtimer_list -Iutils/hexdump

CFLAGS_af_test.o += -Iservices/audioflinger
CFLAGS_tdm_test.o += -Iservices/audioflinger
CFLAGS_dac2_test.o += -Iservices/audioflinger

CFLAGS_bt_conn_sleep.o += -Iplatform/drivers/ana
CFLAGS_bt_conn_simu.o += -Iplatform/drivers/ana

ifeq ($(AF_SIMU),1)
CFLAGS_bt_conn_simu.o += -Iplatform/drivers/codec
obj-y += ../../services/audioflinger/
obj-y += ../../platform/drivers/codec/
CFLAGS_bt_conn_simu.o += -DAF_SIMU
CFLAGS_af_test.o += -DAF_SIMU
endif

ifeq ($(BTPCM_TEST),1)
CFLAGS_bt_conn_sleep.o += -DBTPCM_TEST
endif

ifeq ($(DPD_BT_TEST),1)
CFLAGS_bt_conn_sleep.o += -DDPD_BT_TEST
endif

ifeq ($(SPI_ROM_ONLY),1)
CFLAGS_hwtest_main.o += -DSPI_ROM_ONLY
CFLAGS_efuse_test.o += -DSPI_ROM_ONLY
else
CFLAGS_efuse_test.o += -Iplatform/drivers/ana
endif

ifeq ($(I2C_TEST_DMA_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_DMA_MODE
endif
ifeq ($(I2C_TEST_INT_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_INT_MODE
endif
ifneq ($(I2C_SPEED),)
CFLAGS_i2c_test.o += -DI2C_SPEED=$(I2C_SPEED)
endif

ifeq ($(NO_CLOCK_INIT),1)
CFLAGS_hwtest_main.o += -DNO_CLOCK_INIT
endif
ifeq ($(NO_PMU),1)
CFLAGS_hwtest_main.o += -DNO_PMU
endif
ifeq ($(NO_LPU_26M),1)
CFLAGS_hwtest_main.o += -DNO_LPU_26M
endif
ifeq ($(NO_PLL),1)
CFLAGS_hwtest_main.o += -DNO_PLL
endif
ifeq ($(SDIO_DEVICE_TEST),1)
CFLAGS_sdio_device_test.o += -DSDIO_DEVICE_TEST
CFLAGS_hwtest_main.o += -DSDIO_DEVICE_TEST
endif

ifeq ($(SDIO_HOST_TEST),1)
CFLAGS_sdio_host_test.o += -DSDIO_HOST_TEST
CFLAGS_hwtest_main.o += -DSDIO_HOST_TEST
endif

CFLAGS_sleep_test.o += -Iplatform/drivers/ana
ifeq ($(NO_WAKEUP),1)
CFLAGS_sleep_test.o += -DNO_WAKEUP
endif
ifeq ($(NO_TIMER),1)
CFLAGS_sleep_test.o += -DNO_TIMER
endif

ifneq ($(CHIP_HAS_I2S),)
CFLAGS_tdm_test.o += -DCHIP_HAS_I2S=$(CHIP_HAS_I2S)
endif

ifeq ($(USB_CLK_SRC_24M_X2),1)
USB_CFG_FLAGS += -DUSB_CLK_SRC_24M_X2
else
ifeq ($(USB_CLK_SRC_48M),1)
USB_CFG_FLAGS += -DUSB_CLK_SRC_48M
else
ifeq ($(USB_CLK_SRC_26M_X4),1)
USB_CFG_FLAGS += -DUSB_CLK_SRC_26M_X4
else
ifeq ($(USB_CLK_SRC_26M_X2),1)
USB_CFG_FLAGS += -DUSB_CLK_SRC_26M_X2
endif
endif
endif
endif

ifeq ($(USB_HIGH_SPEED),1)
USB_TEST_PMU_CFG ?= 1
endif
ifeq ($(USB_TEST_PMU_CFG),1)
USB_CFG_FLAGS += -DUSB_TEST_PMU_CFG -Iplatform/drivers/ana
endif

CFLAGS_usb_dev_test.o += $(USB_CFG_FLAGS)
CFLAGS_usb_host_test.o += $(USB_CFG_FLAGS)

ifneq ($(USB_SERIAL_DIRECT_XFER_SIZE),)
CFLAGS_usb_dev_test.o += -DUSB_SERIAL_DIRECT_XFER_SIZE=$(USB_SERIAL_DIRECT_XFER_SIZE)
endif

TEST_CASE_LIST := $(subst $(comma),$(space),$(TC))

obj-y += ../../platform/drivers/norflash/
ifneq ($(filter NANDFLASH_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../platform/drivers/nandflash/
CFLAGS_nandflash_test.o += -DNANDFLASH_TEST -Iplatform/drivers/nandflash -Iutils/boot_struct
endif

ifneq ($(filter PMU_WDT_TEST,$(TEST_CASE_LIST)),)
CFLAGS_pmu_wdt_test.o += -DPMU_WDT_TEST -Iplatform/drivers/ana
ifeq ($(PMU_WDT_IRQ),1)
CFLAGS_pmu_wdt_test.o += -DPMU_WDT_IRQ
endif
endif

ifneq ($(filter HWFFT_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../platform/drivers/hwfft/
CFLAGS_hwfft_test.o += -Iplatform/drivers/hwfft -DHWFFT_TEST
endif

ifneq ($(filter COREMARK_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../tests/coremark/
endif

ifneq ($(filter AF_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../services/audioflinger/

ifeq ($(AF_INT_CODEC_TEST),1)
CFLAGS_af_test.o += -DAF_INT_CODEC_TEST
obj-y += ../../platform/drivers/codec/
export AF_DEVICE_INT_CODEC ?= 1
export AF_DEVICE_I2S ?= 0
else
export AF_DEVICE_INT_CODEC ?= 0
export AF_DEVICE_I2S ?= 1
endif

ifneq ($(CHAN_NUM_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_NUM_CAPTURE=$(CHAN_NUM_CAPTURE)
endif
ifneq ($(CHAN_SEP_BUF_CAPTURE),)
CFLAGS_af_test.o += -DCHAN_SEP_BUF_CAPTURE=$(CHAN_SEP_BUF_CAPTURE)
endif
ifeq ($(FIXED_BUF_CAPTURE),1)
CFLAGS_af_test.o += -DFIXED_BUF_CAPTURE
endif
endif # AF_TEST

ifneq ($(filter USB_AUDIO_TEST,$(TEST_CASE_LIST)),)
ifneq ($(USB_ISO),1)
$(error USB_AUDIO_TEST requires USB_ISO=1)
endif
endif

CFLAGS_sec_eng_test.o += -Iutils/hexdump
ifneq ($(filter SEC_ENG_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../utils/hexdump/
asflags-y += -Itests/hwtest
ifeq ($(AES_LARGE_DATA),1)
asflags-y += -DAES_LARGE_DATA
CFLAGS_sec_eng_test.o += -DAES_LARGE_DATA
endif
ifeq ($(HASH_LARGE_DATA),1)
asflags-y += -DHASH_LARGE_DATA
CFLAGS_sec_eng_test.o += -DHASH_LARGE_DATA
endif
ifeq ($(HMAC_LARGE_DATA),1)
asflags-y += -DHMAC_LARGE_DATA
CFLAGS_sec_eng_test.o += -DHMAC_LARGE_DATA
endif
endif

ifneq ($(filter A7_DSP_TEST,$(TEST_CASE_LIST)),)
CFLAGS_a7_dsp_test.o += -DA7_DSP_TEST
DSP_BIN_NAME ?= hwtest_a7_dsp
$(obj)/asm_test.o : out/$(T)/$(DSP_BIN_NAME).bin
AFLAGS_asm_test.o += -DA7_DSP_TEST -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -Iout/$(T)
out/$(T)/$(DSP_BIN_NAME).bin : $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin,$(srctree)/$@)
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).elf,$(srctree)/$(@:.bin=.elf))
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).map,$(srctree)/$(@:.bin=.map))
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).lst,$(srctree)/$(@:.bin=.lst))
$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin : FORCE
	$(call echo-help,)
	$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
# We cannot build $(DSP_BIN_NAME) automatically here, because the env inherited by sub-make cannot be cleaned:
# E.g., KBUILD_SRC, TC, CHIP_HAS_CP, ...
	$(call echo-help,)
endif

ifneq ($(filter TRANSQ_TEST,$(TEST_CASE_LIST)),)
ifneq ($(RX_ROLE),)
CFLAGS_transq_test.o += -DRX_ROLE
endif
ifneq  ($(TRANSQ_ID),)
CFLAGS_transq_test.o += -DTRANSQ_ID=$(TRANSQ_ID)
endif
endif

ifneq ($(filter SENSOR_HUB_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../platform/drivers/sensor_hub/
CFLAGS_sensor_hub_test.o += -DSENSOR_HUB_TEST -Iplatform/drivers/sensor_hub -Iplatform/drivers/ana -Itests/sensor_hub/inc
ifeq ($(MCU2SENS_MSG_TEST),1)
CFLAGS_sensor_hub_test.o += -DMCU2SENS_MSG_TEST
endif
ifneq ($(filter SLEEP_TEST,$(TEST_CASE_LIST)),)
CFLAGS_sensor_hub_test.o += -DSLEEP_TEST
endif
ifeq ($(NO_TIMER),1)
CFLAGS_sensor_hub_test.o += -DNO_TIMER
endif
ifeq ($(NO_PMU),1)
CFLAGS_sensor_hub_test.o += -DNO_PMU
endif
ifeq ($(PMU_FULL_INIT),1)
CFLAGS_sensor_hub_test.o += -DPMU_FULL_INIT
endif
ifeq ($(SENS_TRC_TO_MCU),1)
RMT_TRACE := 1
endif
endif

ifneq ($(filter BT_HOST_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../platform/drivers/bt_host/
CFLAGS_bt_host_test.o += -DBT_HOST_TEST -Iplatform/drivers/bt_host -Iplatform/drivers/ana -Itests/bt_host
ifeq ($(SYS2BTH_MSG_TEST),1)
CFLAGS_bt_host_test.o += -DSYS2BTH_MSG_TEST
endif
ifneq ($(filter SLEEP_TEST,$(TEST_CASE_LIST)),)
CFLAGS_bt_host_test.o += -DSLEEP_TEST
endif
ifeq ($(NO_TIMER),1)
CFLAGS_bt_host_test.o += -DNO_TIMER
endif
ifeq ($(NO_PMU),1)
CFLAGS_bt_host_test.o += -DNO_PMU
endif
ifeq ($(PMU_FULL_INIT),1)
CFLAGS_bt_host_test.o += -DPMU_FULL_INIT
endif
ifeq ($(BTH_TRC_TO_SYS),1)
RMT_TRACE := 1
endif
endif

ifeq ($(RMT_TRACE),1)
obj-y += ../../platform/drivers/rmt_trace/
endif

ifneq ($(filter DSI_TEST,$(TEST_CASE_LIST)),)
#ccflags-y += -Itests/hwtest/dsi/inc -DDSI_TEST
#obj-y += dsi/dsi_pkt.o
#obj-y += dsi/dsi_itf_base.o
#obj-y += dsi/dsi_test.o
endif

ifneq ($(filter RSA_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../utils/secure_boot/
subdir-ccflags-y += -DRSA_TEST
endif

ifneq ($(filter RANDOM_TEST,$(TEST_CASE_LIST)),)
obj-y += ../../platform/drivers/random/
# RANDOM_TEST is one set of TRNG_TEST
CFLAGS_trng_test.o += -DTRNG_TEST -Iplatform/drivers/random
endif

CHIP_PSRAM_CTRL_VER ?= 1
CFLAGS_psram_test.o += -DCHIP_PSRAM_CTRL_VER=$(CHIP_PSRAM_CTRL_VER)

ifneq ($(TEST_CASE_LIST),)
CFLAGS_hwtest_main.o += $(addprefix -D,$(TEST_CASE_LIST))
endif

ifneq ($(DEBUG_PORT),)
CFLAGS_hwtest_main.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif
