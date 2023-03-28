cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))
obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ccflags-y := -Iplatform/drivers/graphic -Iplatform/drivers/graphic/lcdc \
	-Iinclude/rtos/liteos/liteos_a/kal/cmsis/ -Iplatform/cmsis/inc
