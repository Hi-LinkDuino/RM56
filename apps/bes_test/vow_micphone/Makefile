cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

ifeq ($(NET_KCP_SUPPORT), 1)
ccflags-y += -D__VOW_MIC_KCP__
endif

ccflags-y += \
            -Iutils \
            -Inet/dlna/player/sink \
            -Inet/include \
            -Inet/kcp/ \
            -Iservices/wifi_app/vowlan \
            -Iservices/audioflinger \
            -Iservices/wifi_app/alsa_adapter \
            -Iplatform/hal
