cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj_c:.c=.o)
ccflags-y += -Iutils/hwtimer_list