cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
#obj-y := $(filter-out audio_const_rom.c,$(obj-y))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

ccflags-y += -Iutils/crc32 -Iutils/boot_struct -Iservices/cp_boot -Inet/utils
CFLAGS_main.o += -DDEBUG_PORT=$(DEBUG_PORT)
