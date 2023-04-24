
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := a064.o
obj-y += qrcode_main.o
ifeq ($(A7_UART),2)
ccflags-y += -DSTD_PRINTF
endif
ifeq ($(DSI_TEST),1)
obj-y += ../../../../../../tests/hwtest/dsi_test.o
ccflags-y += -DDSI_TEST
else
obj-y += dsi_main.o
endif

# ifeq ($(CSI_TEST),1)
# obj-y += ../../../../../../tests/hwtest/csi_test.o
# ccflags-y += -DCSI_TEST
# else
# obj-y += csi_main.o
# endif
ccflags-y += \
    -Iplatform/drivers/graphic/lcdc/ \
    -Iplatform/drivers/graphic/ \
    -Iplatform/drivers/graphic/ \
    -Iservices/a7_dsp/common/ \
    -Iservices/transq_msg/