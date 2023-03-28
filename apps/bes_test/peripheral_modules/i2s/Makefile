cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

#test source code
obj-y += ../../../vowlan/wi2s_demo/

ccflags-y += -Iutils/cqueue/

# tartget need config
#export AF_DEVICE_I2S ?= 1
#KBUILD_CPPFLAGS += -DAF_DEVICE_I2S
#KBUILD_CPPFLAGS += -DI2S_MCLK_FROM_SPDIF