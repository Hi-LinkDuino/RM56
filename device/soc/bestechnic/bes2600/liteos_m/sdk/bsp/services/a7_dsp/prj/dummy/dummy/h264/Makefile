
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := a064.o
obj-y += h264_play.o
obj-y += a7_pcm.o

ccflags-y += \
    -Iplatform/drivers/graphic/lcdc/ \
    -Iplatform/drivers/graphic/ \
    -Iplatform/drivers/graphic/ \
    -Iservices/a7_dsp/common/ \
    -Iservices/transq_msg/