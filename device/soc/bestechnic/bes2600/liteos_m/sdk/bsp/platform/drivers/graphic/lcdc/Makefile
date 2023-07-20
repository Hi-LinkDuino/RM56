
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y += -Iutils/hwtimer_list \
		-Iplatform/drivers/graphic/osl_liteos_a

ifeq ($(CONFIG_DSI_2LANE_MODE),1)
ccflags-y += -DCONFIG_DSI_2LANE_MODE
endif
