
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)library/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)library/vauth/*.c))
obj-c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)library/vtls/*.c))

obj-y := $(obj-c:.c=.o)

ccflags-y += \
    -Inet/libcurl/include/\
    -Inet/libcurl/library/ \
    -Inet/mbedtls/include/

ccflags-y += \
    -I$(LWIP_ROOT)/src/include/\
    -I$(LWIP_ROOT)/src/include/lwip/\
    -I$(LWIP_ROOT)/src/include/lwip/arch\
    -I$(LWIP_ROOT)/src/include/ipv4/ \
    -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)/ \
    -Inet/include/os/\
    -Iplatform/hal/\
    -Iplatform/cmsis/inc\
    -Inet/include/\
    -Irtos/rtx/TARGET_CORTEX_M/\
    -Inet/\
    -Inet/include/\
    -Inet/include/byteorder/\
    -Inet/include/os/\
    -Inet/include/unaligned/\
    -Inet/net80211/\
    -Inet/core/\
    -Inet/net_os/ \

ccflags-y += -DBUILDING_LIBCURL   -DHAVE_CONFIG_H -DCURL_STATICLIB 

ifeq ($(NET_MBEDTLS_SUPPORT),1)
ccflags-y += -DUSE_MBEDTLS
endif

ifeq ($(CURL_USE_HAL_TIME), 1)
ccflags-y += -D_CURL_USE_HAL_TIME_
endif