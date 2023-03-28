cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Inet/librws/src/ \
	-Inet/lwip/src/include/ \
    -Inet/lwip/src/include/lwip/   \
	-Inet/lwip/src/include/ipv4/  \
	-Inet/lwip/src/include/ipv6/  \
	-Inet/lwip/src/include/posix/  \
	-Inet/lwip/src/include/lwip/arch/  \
	-Inet/include/linux/ \
	-Iservices/dlna/cgdevicelayer/include/cybergarage/util/ \
	-Iservices/dlna/adaplayer/common/src/ \
	-Inet/ \
	-Inet/mbedtls/include \
	-Inet/mbedtls_client/ \
	-Irtos/rhino/kernel/include/

ifeq ($(NET_MBEDTLS_SUPPORT),1)
ccflags-y += -DCG_USE_MBEDTLS
endif

