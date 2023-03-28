cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

ifeq ($(BES_TEST), 1)
ccflags-y += -DBES_TEST
obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)
obj-y += \
    wifi/ \
    audio/ \
    ftp/ \
    libcurl/ \
    mbedtls/ \
    posix/ \
    websocket/ \
    platform/ \
    ir/ \
    fs/ \
    heap_test/\
    peripheral_modules/\
    resample/

ifeq ($(SPI_AUDIO_TEST),1)
obj-y += spi_audio/
ccflags-y += -Ispi_audio/
endif

else
obj-y := master_app.o
endif # ifeq ($(BES_TEST), 1)

ifeq ($(CHIP_HAS_EXT_NORFLASH), 1)
# obj-y += flash2_demo.o
endif

ifeq ($(CSI_REPORT), 1)
obj-y += csi_reporter.o
endif

ifeq ($(DLNA_SUPPORT_TEST), 1)
obj-y += dlna/
ccflags-y += -D__DLNA_SUPPORT_TEST__
endif

ifeq ($(AIRPLAY_SUPPORT_APP_TEST), 1)
obj-y += airplay_test/
ccflags-y += -D__AIRPLAY_SUPPORT_TEST__
ccflags-y += -Iapps/bes_test/airplay_test/
endif

ifeq ($(VOW_MICPHONE),1)
obj-y += vow_micphone/
ccflags-y += -D__VOW_MICPHONE__
ccflags-y += -Iapps/bes_test/vow_micphone/
endif

ccflags-y +=  \
    -Iservices/nvrecord/ \
    -Iservices/wifi_app/sniffer \
    -Iservices/sys_time/ \
    -Iservices/nv_section/customparam_section/ \
    -I$(LWIP_ROOT)/src/include/lwip/apps/ \
    -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)/ \
    -Iservices/fs/mem \
    -Iservices/wifi_app/ \
    -Iservices/wifi_app/wifi_console/ \
    -Iservices/norflash_api/

ccflags-y +=  \
    -Inet/wifi \
    -Inet/mbedtls/include/ \
    -Iapps/ \
    -Iservices/wifi_app/littlefs/lfs_adapter/ \
    -Iservices/wifi_app/littlefs/flash_partition/ \
    -Inet/mbedtls/include/

ifeq ($(USB_STACK), 1)
ccflags-y += -Iservices/usb_stack/adapter
endif

ifeq ($(TEST_BLE_SERVICE), 1)
obj-y += bledemo/
endif

