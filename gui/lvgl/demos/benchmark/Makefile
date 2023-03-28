cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)assets/*.c))
obj-y := $(obj-y:.c=.o)