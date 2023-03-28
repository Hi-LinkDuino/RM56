cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)


ccflags-y += \
    -Inet/libcurl/include/ \
    -Inet/mbedtls/include/ \
    -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)/ \
    -Iutils/kfifo/ \
    -Iservices/wifi_app/http_media/ \
    -Iservices/wifi_app/littlefs/