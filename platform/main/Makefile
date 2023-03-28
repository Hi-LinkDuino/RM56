cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := mpu_cfg.c

ifeq ($(ROM_BUILD),1)
obj-y += ../../tests/rom/startup_ARMCM.S
else
obj-y += startup_main.S
ifeq ($(NO_STARTFILES),1)
AFLAGS_startup_main.o += -D__NO_STARTFILES__
endif
endif

ifeq ($(SDIO_DEVICE_TEST),1)
obj-y += ../../tests/hwtest/sdio_device_test.c
ccflags-y += -DSDIO_DEVICE_TEST
endif

ifeq ($(FACTORY_SUITE),1)
obj-y += ../../tests/factory_suite/
else ifeq ($(RELIABILITY_TEST),1)
obj-y += ../../tests/reliability_test/
else ifeq ($(NOSTD),1)
obj-y += nostd_main.c
else ifeq ($(NUTTX_BUILD),1)
obj-y += nuttx_main.cpp
else ifeq ($(ARM_CMSE),1)
obj-y += cmse_main.c
ifeq ($(SECURE_BOOT),1)
ccflags-y += -DSECURE_BOOT
obj-y += bootinfo_flash.c
obj-y += ../../utils/secure_boot/mincrypt/
ccflags-y += -Iutils/secure_boot/mincrypt -Iutils/crc32
endif
else ifeq ($(NOAPP),1)
obj-y += noapp_main.cpp
else
ifeq ($(USB_WIFI), 1)
obj-y += ../../tests/wifi_usb/main.c
else
obj-y += main.cpp
endif
endif

ifeq ($(ARM_CMNS),1)
obj-y += cmse/tz_trace_ns.c
endif

ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_SERIAL_DIRECT_XFER_TEST) $(USB_AUDIO_TEST) $(I2C_TEST) $(DSI_TEST) $(CSI_TEST) $(QSPI_DISPLAY_TEST)  \
					$(I2S_TEST) $(AF_TEST) $(VD_TEST) $(CP_TEST) $(TDM_TEST) $(SEC_ENG_TEST) $(A7_DSP_TEST) \
					$(TRANSQ_TEST) $(FLASH_TEST) $(PSRAM_TEST) $(PSRAMUHS_TEST) $(SPI_NORFLASH_TEST) $(UART_TEST) \
					$(SENSOR_HUB_TEST) $(SENSOR_TEST) $(MBW_TEST) $(BECO_TEST) $(TZ_TEST) $(FT_TEST)\
					$(BT_CONN_SIMU) $(MINIMA_TEST) $(LIS25BA_TEST) $(PWM_TEST) $(TRNG_TEST) $(IR_TEST)\
					$(RSA_TEST) $(DAC2_TEST) $(WIFI_TEST) $(BTHCI_H4) $(VOW_SLAVE_2002) $(NANDFLASH_TEST) $(SDMMC_TEST) $(SDIO_HOST_TEST)),)

ccflags-y += -Itests/hwtest -DHWTEST

ifeq ($(NANDFLASH_TEST),1)
obj-y += ../../utils/hexdump/ ../../platform/drivers/nandflash/
obj-y += ../../tests/hwtest/nandflash_test.c
subdir-ccflags-y += -Iplatform/drivers/nandflash -Iutils/hexdump
subdir-ccflags-y += -DNANDFLASH_TEST
endif

ifeq ($(RSA_TEST),1)
obj-y += ../../utils/secure_boot/
subdir-ccflags-y += -DRSA_TEST
endif

ifneq ($(filter 1,$(USB_SERIAL_TEST) $(USB_SERIAL_DIRECT_XFER_TEST) $(USB_AUDIO_TEST)),)
obj-y += ../../tests/hwtest/usb_dev_test.c
ifeq ($(USB_SERIAL_TEST),1)
ccflags-y += -DUSB_SERIAL_TEST
else ifeq ($(USB_SERIAL_DIRECT_XFER_TEST),1)
ccflags-y += -DUSB_SERIAL_DIRECT_XFER_TEST
else
ccflags-y += -DUSB_AUDIO_TEST
endif
endif

ifneq ($(USB_SERIAL_DIRECT_XFER_SIZE),)
ccflags-y += -DUSB_SERIAL_DIRECT_XFER_SIZE=$(USB_SERIAL_DIRECT_XFER_SIZE)
endif

ifeq ($(I2C_TEST),1)
obj-y += ../../tests/hwtest/i2c_test.c
ccflags-y += -DI2C_TEST

ifeq ($(I2C_TEST_DMA_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_DMA_MODE
endif
ifeq ($(I2C_TEST_INT_MODE),1)
CFLAGS_i2c_test.o += -DI2C_TEST_INT_MODE
endif
ifneq ($(I2C_SPEED),)
CFLAGS_i2c_test.o += -DI2C_SPEED=$(I2C_SPEED)
endif
endif # I2C_TEST

ifeq ($(DSI_TEST),1)
obj-y += ../../tests/hwtest/dsi_test.c
ccflags-y += -DDSI_TEST
endif

ifeq ($(CSI_TEST),1)
obj-y += ../../tests/hwtest/csi_test.c
ccflags-y += -DCSI_TEST
endif

ifeq ($(AF_TEST),1)
obj-y += ../../services/audioflinger/  ../../tests/hwtest/af_test.c
ccflags-y += -Iservices/audioflinger -DAF_TEST

ifeq ($(AF_INT_CODEC_TEST),1)
obj-y += ../../platform/drivers/codec/
CFLAGS_af_test.o += -DAF_INT_CODEC_TEST
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

ifeq ($(VD_TEST),1)
ifneq ($(VOICE_DETECTOR_EN),1)
$(error VOICE_DETECTOR_EN=1 missing when VD_TEST=1)
endif
obj-y += \
	../../apps/voice_detector/ \
    ../../apps/common/app_utils.c \
    ../../services/audioflinger/ \
    ../../services/audio_dump/ \
    ../../platform/drivers/codec/
ccflags-y += -Iapps/voice_detector -Iservices/audioflinger -DVD_TEST
endif

ifeq ($(CP_TEST),1)
obj-y += ../../tests/hwtest/cp_test.c
ccflags-y += -DCP_TEST
endif

ifeq ($(TDM_TEST),1)
obj-y += ../../tests/hwtest/tdm_test.c
ccflags-y += -DTDM_TEST
ccflags-y += -DCHIP_HAS_I2S=$(CHIP_HAS_I2S)
ifneq ($(CHIP_HAS_TDM),)
ccflags-y += -DCHIP_HAS_TDM=$(CHIP_HAS_TDM)
endif
ifeq ($(LIS25_TEST), 1)
ccflags-y += -DLIS25_TEST
obj-y += ../../tests/codec_vad/lis25ba.c
ccflags-y += -Itests/codec_vad/
endif
ifeq ($(TDM_LOOPBACK_TEST), 1)
ccflags-y += -DTDM_LOOPBACK_TEST
endif
ifeq ($(TDM_PLAYBACK_ONLY_TEST), 1)
ccflags-y += -DTDM_PLAYBACK_ONLY_TEST
endif
endif #TDM_TEST

ifeq ($(SPI_NORFLASH_TEST),1)
obj-y += ../../tests/hwtest/spi_norflash_test.c
ccflags-y += -DSPI_NORFLASH_TEST
endif #SPI_NORFLASH_TEST

ifeq ($(UART_TEST),1)
obj-y += ../../tests/hwtest/uart_test.c
#obj-y += ../../utils/crc32/
#ccflags-y += -Iutils/crc32
ccflags-y += -DUART_TEST
endif

ifeq ($(SEC_ENG_TEST),1)
obj-y += ../../tests/hwtest/sec_eng_test.c ../../utils/hexdump/
ccflags-y += -DSEC_ENG_TEST -Iutils/hexdump
ifneq ($(filter 1,$(AES_LARGE_DATA) $(HASH_LARGE_DATA) $(HMAC_LARGE_DATA)),)
obj-y += ../../tests/hwtest/asm_test.S
asflags-y += -Itests/hwtest
ifeq ($(AES_LARGE_DATA),1)
asflags-y += -DAES_LARGE_DATA
ccflags-y += -DAES_LARGE_DATA
endif
ifeq ($(HASH_LARGE_DATA),1)
asflags-y += -DHASH_LARGE_DATA
ccflags-y += -DHASH_LARGE_DATA
endif
ifeq ($(HMAC_LARGE_DATA),1)
asflags-y += -DHMAC_LARGE_DATA
ccflags-y += -DHMAC_LARGE_DATA
endif
endif
endif

ifeq ($(A7_DSP_TEST),1)
$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
obj-y += ../../tests/hwtest/a7_dsp_test.c
obj-y += ../../tests/hwtest/asm_test.S
ccflags-y += -DA7_DSP_TEST
AFLAGS_asm_test.o += -DA7_DSP_TEST -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -Iout/$(T)
DSP_BIN_NAME ?= hwtest_a7_dsp
$(obj)/../../tests/hwtest/asm_test.o : out/$(T)/$(DSP_BIN_NAME).bin
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
endif #A7_DSP_TEST

ifeq ($(TRANSQ_TEST),1)
obj-y += ../../tests/hwtest/transq_test.c
ccflags-y += -DTRANSQ_TEST
ifneq ($(RX_ROLE),)
CFLAGS_transq_test.o += -DRX_ROLE
endif
ifneq  ($(TRANSQ_ID),)
CFLAGS_transq_test.o += -DTRANSQ_ID=$(TRANSQ_ID)
endif
endif #TRANSQ_TEST

ifeq ($(FLASH_TEST),1)
obj-y += ../../tests/hwtest/flash_test.c
ccflags-y += -DFLASH_TEST
endif

ifeq ($(PSRAM_TEST),1)
obj-y += ../../tests/hwtest/psram_test.c
ccflags-y += -DPSRAM_TEST -DCHIP_PSRAM_CTRL_VER=$(CHIP_PSRAM_CTRL_VER)
endif #PSRAM_TEST

ifeq ($(PSRAMUHS_TEST),1)
obj-y += ../../tests/hwtest/psram_test.c
ccflags-y += -DPSRAMUHS_TEST
endif #PSRAMUHS_TEST

ifeq ($(MBW_TEST),1)
obj-y += ../../tests/hwtest/mbw_test.c
ccflags-y += -DMBW_TEST
endif #MBW_TEST

ifeq ($(SENSOR_HUB_TEST),1)
obj-y += ../../tests/hwtest/sensor_hub_test.c ../../platform/drivers/sensor_hub/
ccflags-y += -DSENSOR_HUB_TEST
CFLAGS_sensor_hub_test.o += -DSENSOR_HUB_TEST -Iplatform/drivers/sensor_hub -Iplatform/drivers/ana
ifeq ($(NO_PMU),1)
CFLAGS_sensor_hub_test.o += -DNO_PMU
endif
ifeq ($(PMU_FULL_INIT),1)
CFLAGS_sensor_hub_test.o += -DPMU_FULL_INIT
endif
ifeq ($(SENS_TRC_TO_MCU),1)
RMT_TRACE := 1
endif
ifeq ($(MCU2SENS_MSG_TEST),1)
CFLAGS_sensor_hub_test.o += -DMCU2SENS_MSG_TEST
endif
ifeq ($(VAD_CODEC_TEST),1)
obj-y += ../../services/audioflinger/
obj-y += ../../platform/drivers/codec/
export AF_DEVICE_INT_CODEC ?= 1
export AF_DEVICE_I2S ?= 1
CFLAGS_sensor_hub_test.o += -DVAD_CODEC_TEST -Iservices/audioflinger -Itests/sensor_hub/inc
ifeq ($(VAD_CODEC_LOOP),1)
CFLAGS_sensor_hub_test.o += -DVAD_CODEC_LOOP
endif
ifeq ($(VAD_PWRKEY_EVENT),1)
CFLAGS_sensor_hub_test.o += -DVAD_PWRKEY_EVENT
obj-y += \
	../../apps/sensorhub/ \
	../../utils/heap/ \
	../../utils/cqueue/ \
	../../apps/common/ \

ccflags-y += -Iapps/sensorhub
endif # VAD_PWRKEY_EVENT
endif # VAD_CODEC_TEST
endif # SENSOR_HUB_TEST

ifeq ($(RMT_TRACE),1)
obj-y += ../../platform/drivers/rmt_trace/
endif

ifeq ($(CODEC_SIMU),1)
obj-y += ../../tests/hwtest/codec_test.o
ccflags-y += -DCODEC_SIMU -Itests/hwtest
endif

ifeq ($(I2S_TEST),1)
obj-y += ../../tests/hwtest/i2s_test.o
ccflags-y += -DI2S_TEST -Itests/hwtest
endif

ifeq ($(SENSOR_TEST),1)
obj-y += ../../tests/sensor_test/lis3dsh_test.c     ../../platform/drivers/sensor/st/lis3dsh_reg.c
obj-y += ../../tests/sensor_test/lsm6dsl_test.c     ../../platform/drivers/sensor/st/lsm6dsl_reg.c
obj-y += ../../tests/sensor_test/lsm6dsox_test.c    ../../platform/drivers/sensor/st/lsm6dsox_reg.c
obj-y += ../../tests/sensor_test/bmp280_test.c      ../../platform/drivers/sensor/bosch/bmp280.c
obj-y += ../../tests/sensor_test/bmx055_test.c      ../../platform/drivers/sensor/bosch/bma2x2.c
obj-y += ../../tests/sensor_test/sensor_test.c
ccflags-y += -DSENSOR_TEST
ccflags-y += -Iplatform/drivers/sensor/st/
ccflags-y += -Iplatform/drivers/sensor/bosch/
endif #SENSOR_TEST

ifeq ($(BECO_TEST),1)
# obj-y += ../../platform/cmsis/DSP_Lib/
# obj-y += ../../platform/cmsis/NN_Lib/
obj-y += ../../tests/hwtest/beco_test/
subdir-ccflags-y +=  -DBECO_TEST -Iinclude/beco -Iinclude/cmsis_dsp -Iinclude/cmsis_nn
endif

ifeq ($(TZ_TEST),1)
obj-y += ../../tests/hwtest/tz_test.c
CFLAGS_tz_test.o += -Iplatform/main/cmse
ccflags-y += -DTZ_TEST
endif

ifeq ($(LIS25BA_TEST),1)
obj-y += \
	../../services/audioflinger/ \
	../../platform/drivers/codec/ \
	../../services/audio_dump/ \
	../../services/bone_sensor/lis25ba_v2/ \

ccflags-y += -DLIS25BA_TEST -Iservices/bone_sensor/lis25ba_v2 -Iservices/audioflinger
endif

ifeq ($(DAC2_TEST),1)
obj-y += \
	../../services/audioflinger/ \
	../../platform/drivers/codec/ \
	../../tests/hwtest/dac2_test.o \

ccflags-y += -DDAC2_TEST -Itests/hwtest -Iservices/audioflinger
endif

endif # hwtest cases

ifeq ($(FT_TEST),1)
obj-y += ../../tests/hwtest/ft_test.c
ccflags-y += -DFT_TEST
endif

ifeq ($(BT_CONN_SIMU),1)
obj-y += ../../tests/hwtest/bt_conn_simu.o
ccflags-y += -DBT_CONN_SIMU -Itests/hwtest
endif

ifeq ($(WIFI_TEST),1)
ifeq ($(VOW_SLAVE_2002),1)
obj-y += ../../tests/hwtest/vow_slave/vow_wifi_init.o
obj-y += ../../tests/hwtest/vow_slave/wsink.o
else
obj-y += ../../tests/hwtest/wifi_test.o
endif
ccflags-y += -Inet/wifi/\
             -Inet/wifi/$(CHIP)\
             -Inet/utils\
             -Inet/include/os/\
             -I$(LWIP_ROOT)/src/include/\
             -Iplatform/drivers/wifi \
             -Iplatform/drivers/wifi/$(CHIP)\
             -Iplatform/drivers/ana/$(CHIP)\
             -Iutils/ \
             -Iservices/wifi_app/vowlan/ \
             -Iservices/nv_section/factory_section
ifeq ($(WIFI_NONSIGNALING_MODE),1)
CFLAGS_wifi_test.o += -DWIFI_NONSIGNALING_MODE
endif
ifeq ($(WIFI_SIGNALING_MODE),1)
CFLAGS_wifi_test.o += -DWIFI_SIGNALING_MODE
endif
ifeq ($(BEST2002_BOOT_FOR_DPD),1)
CFLAGS_wifi_test.o += -DBEST2002_BOOT_FOR_DPD
CFLAGS_noapp_main.o += -DBEST2002_BOOT_FOR_DPD
endif
endif

CFLAGS_af_test.o += -Iservices/audioflinger

ifeq ($(AF_SIMU),1)
obj-y += ../../tests/hwtest/af_test.o
CFLAGS_bt_conn_simu.o += -Iplatform/drivers/codec
obj-y += ../../services/audioflinger/
obj-y += ../../platform/drivers/codec/
CFLAGS_bt_conn_simu.o += -DAF_SIMU
CFLAGS_af_test.o += -DAF_SIMU
endif

ifeq ($(MINIMA_TEST),1)
ccflags-y += -DMINIMA_TEST
endif

ifeq ($(PWM_TEST),1)
obj-y += ../../tests/hwtest/pwm_test.c
ccflags-y += -DPWM_TEST
endif

ifeq ($(IR_TEST),1)
obj-y += ../../tests/hwtest/ir_test.c
ccflags-y += -DIR_TEST
ifeq ($(IR_DONOT_USE_DMA),1)
ccflags-y += -DIR_DONOT_USE_DMA
endif
endif

ifeq ($(TRNG_TEST),1)
obj-y += ../../tests/hwtest/trng_test.c
ccflags-y += -DTRNG_TEST
ifeq ($(RANDOM_TEST),1)
obj-y += ../../platform/drivers/random/
CFLAGS_trng_test.o += -DRANDOM_TEST -Iplatform/drivers/random
endif
endif

ifeq ($(QSPI_DISPLAY_TEST),1)
obj-y += ../../tests/hwtest/qspi_display_test.c ../../platform/drivers/qspi_display/
ccflags-y += -DQSPI_DISPLAY_TEST -Iplatform/drivers/qspi_display
endif

ifeq ($(SDMMC_TEST),1)
obj-y += ../../tests/hwtest/sdmmc_test.c
ccflags-y += -DSDMMC_TEST
endif

ifeq ($(USBHOSTFAT_TEST),1)
obj-y += ../../tests/usbFat/
obj-y += ../../platform/drivers/usb/usb_host/
obj-y += ../../services/fs/
ccflags-y += -DUSBHOSTFAT_TEST
endif

ifeq ($(SDIO_HOST_TEST),1)
obj-y += ../../tests/hwtest/sdio_host_test.c
ccflags-y += -DSDIO_HOST_TEST
endif

ifeq ($(BTHCI_H4),1)
ccflags-y += -Iplatform/drivers/bthci
endif

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hwtimer_list/

ifeq ($(CHIP_OCD),1)
obj-y += ../../utils/ocd/
endif

ifeq ($(ARM_CMSE),1)
obj-y += cmse/
CFLAGS_cmse_main.o += -Iplatform/drivers/mpc
CFLAGS_nuttx_main.o += -Iplatform/drivers/mpc
endif

ccflags-y +=  \
	-Iutils/hwtimer_list \
	-Iplatform/drivers/ana \
	-Iplatform/drivers/security_engine \
	-Iplatform/drivers/usb/usb_dev/inc \
    -Itests/sensor_test/               \
	-Iapps/main \
	-Iapps/factory \
	-Iutils/list  \
	-Iutils/boot_struct  \
	-Iapps/audioplayers  \
	-Iservices/bt_app  \
	-Iapps/common  \
	-Iservices/norflash_api  \
	-Iservices/nv_section/log_section \
	-Iinclude/ \
	-Iservices/a7_dsp/common/ \
	-Iservices/wifi_app/wifi_console

ifeq ($(BOOT_LOADER_ENTRY_HOOK),1)
asflags-y += -DBOOT_LOADER_ENTRY_HOOK
endif
ifeq ($(BOOT_LOADER_PRE_INIT_HOOK),1)
asflags-y += -DBOOT_LOADER_PRE_INIT_HOOK
endif
ifeq ($(BOOT_LOADER_POST_INIT_HOOK),1)
asflags-y += -DBOOT_LOADER_POST_INIT_HOOK
endif
ifeq ($(NO_NVIC_INIT),1)
asflags-y += -DNO_NVIC_INIT
endif
ifeq ($(NO_BOOT_INIT),1)
asflags-y += -DNO_BOOT_INIT
endif
ifeq ($(NO_SYSTEM_INIT),1)
asflags-y += -DNO_SYSTEM_INIT
endif

ifneq ($(FLASH_FILL),)
ccflags-y += -DFLASH_FILL=$(FLASH_FILL)
endif

ifneq ($(DEBUG_PORT),)
ccflags-y += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifneq ($(FLASH_SIZE),)
ccflags-y += -DFLASH_SIZE=$(FLASH_SIZE)
endif

ifeq ($(ROM_UTILS_ON),1)
ccflags-y += -DROM_UTILS_ON
endif

ifeq ($(ULTRA_LOW_POWER),1)
ccflags-y += -DULTRA_LOW_POWER
endif

ifeq ($(NO_PMU),1)
ccflags-y += -DNO_PMU
endif

ifeq ($(NO_TIMER),1)
ccflags-y += -DNO_TIMER
endif

ifneq ($(SLEEP_TEST),)
ccflags-y += -DSLEEP_TEST=$(SLEEP_TEST)
endif

ifeq ($(SLEEP_SIMU),1)
ccflags-y += -DSLEEP_SIMU
endif

ifeq ($(CHIP_WAKE_TEST),1)
ccflags-y += -DCHIP_WAKE_TEST
endif

ifeq ($(RTOS),1)
ifeq ($(KERNEL),RTX)
ccflags-y += -Irtos/rtx/TARGET_CORTEX_M
endif
endif

ifeq ($(DEBUG_MODE_USB_DOWNLOAD),1)
ccflags-y += -DDEBUG_MODE_USB_DOWNLOAD
endif

ifeq ($(ARM_CMNS),1)
ccflags-y += -Iplatform/main/cmse
endif

ifeq ($(PSRAM_INIT_CHECK),1)
ccflags-y += -DPSRAM_INIT_CHECK
endif
ifeq ($(PSRAM_ENABLE),1)
ccflags-y += -DPSRAM_ENABLE
endif
ifeq ($(PSRAMUHS_ENABLE),1)
ccflags-y += -DPSRAMUHS_ENABLE
endif
