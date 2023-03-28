cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)


ccflags-y += \
    -Iservices/sys_time/ \
    -Iservices/wifi_app/sniffer/ \
    -Iapps/common/ \
    -Iinclude/rtos/freerto/ \
    -Inet/wifi/ \
    -Iservices/wifi_app/
