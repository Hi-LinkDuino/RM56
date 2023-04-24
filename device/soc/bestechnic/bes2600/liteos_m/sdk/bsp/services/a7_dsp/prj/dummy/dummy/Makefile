
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(A7_H264_TEST),1)
obj-y += h264/
endif
ifeq ($(A7_QRCODE_TEST),1)
obj-y += qrcode/
ccflags-y += -DA7_QRCODE_TEST
endif
ccflags-y += \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iservices/a7_dsp/common/ \
             -Iservices/a7_dsp/prj/genie/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Inet \
             -Inet/utils \
             -Iapps/common \
             -Iservices/a7_dsp/os/test/pcm_audio_instance \
             -Irtos/liteos/liteos_a/kernel/include/ \
             -Iservices/a7_dsp/prj/dummy/dummy/h264/
