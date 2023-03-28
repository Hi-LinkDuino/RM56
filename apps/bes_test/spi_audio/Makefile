cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y +=  \
	-Iutils/boot_struct \
	-Iutils/crc32 \
	-Iutils/hexdump \
	-Iutils/hwtimer_list \
	-Itests/programmer \
	-Iplatform/drivers/usb/usb_dev/inc	\
	-Iplatform/drivers/ana \
	-Iservices/audio_eq \
	-Iservices/multimedia/speech/inc \
	-Iservices/audio_process \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Itests/anc_usb \
	-Iservices/wifi_app/alsa_adapter \
	-Iutils/kfifo \
	-Iapps/bes_test/audio/pcm \
	-Iutils/cqueue