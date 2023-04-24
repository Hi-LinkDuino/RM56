ifeq ($(OPUS_CODEC_SUPPORT),1)
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libopus

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)celt/*.c))
# obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)celt/fixed/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)silk/*.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)silk/fixed/*.c))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
$(LIB_NAME)-y := $(src_obj)

obj-y := $(LIB_NAME).a

ccflags-y += \
	-Iservices/a7_dsp/common/opus/include \
	-Iservices/a7_dsp/common/opus/celt \
	-Iservices/a7_dsp/common/opus/silk \
	-Iservices/a7_dsp/common/opus/src \
	-Iservices/a7_dsp/common/opus/silk/fixed \
	-Iservices/a7_dsp/common/opus/silk/float
	
ccflags-y += -O3 -DOPUS_HAVE_CONFIG_H -DHAVE_CONFIG_H -DHAVE_LRINTF
endif
