cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

DRIVER_LIB_NAME := libdriver
ifneq ($(NUTTX_BUILD),1)
ifneq ($(SDK), 1)
src_obj :=
ifeq ($(CHIP_HAS_USB),1)
src_obj += usb/
endif

ifeq ($(CHIP_HAS_WIFI),1)
src_obj += wifi/
endif

$(DRIVER_LIB_NAME)-y := $(src_obj)

endif # SDK != 1

obj-y := $(DRIVER_LIB_NAME).a

else
ifeq ($(CHIP_HAS_WIFI),1)
obj-y += wifi/
endif
endif

obj-y += ana/ norflash/ spi_norflash/
ifneq ($(OTA_BOOT),1)
ifneq ($(BT_CLOSE),1)
obj-y += codec/ bt/ btpcm/
else
ifneq ($(CHIP),best2002)
obj-y += codec/
endif
ifeq ($(BTHCI_H4),1)
obj-y += bthci/
endif
endif
endif

ifeq ($(BT_CLOSE),1)
ifeq ($(NUTTX_BUILD),1)
obj-y += bt/ btpcm/
else
obj-y += bt/
endif
endif

# obj-y += sbcacc/

ifneq ($(filter-out 1 2 3 4,$(CHIP_FLASH_CTRL_VER)),)
ifeq ($(NANDFLASH_SUPPORT),1)
obj-y += nandflash/
endif
endif


ifeq ($(CHIP_HAS_BCM),1)
obj-y += security_engine/
endif


subdir-ccflags-y += -Iplatform/drivers/ana  \
		-Iplatform/drivers/wifi\
		-Iplatform/drivers/wifi/$(CHIP) \
		-Iplatform/main\
		-Iplatform/drivers/bt

ifeq ($(CHIP_HAS_FM),1)
obj-y += fm/
subdir-ccflags-y += -Iplatform/drivers/fm
endif

VCO_TEST_TOOL ?= 1
ifeq ($(VCO_TEST_TOOL),1)
subdir-ccflags-y += -DVCO_TEST_TOOL
endif

ifeq ($(SUPPORT_IR),1)
obj-y += ir/
subdir-ccflags-y += -Iplatform/drivers/ir/inc
endif


ifeq ($(CHIP_HAS_CP),1)
obj-y += cp_accel/
endif

ifeq ($(SENSOR_HUB),1)
obj-y += sensor_hub/
endif

ifeq ($(SENSOR),1)
obj-y += sensor/
endif

ifeq ($(MINIMA_TEST),1)
obj-y += minima/
endif
