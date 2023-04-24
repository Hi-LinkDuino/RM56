cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
obj-y += ftp_client/
endif

ifeq ($(NET_FTPD_SUPPORT),1)
obj-y += ftpd/
endif

ifeq ($(NET_TFTP_SUPPORT),1)
obj-y += tftp/
endif


ccflags-y += \
        -Iplatform/hal \
        -Iplatform/cmsis \
        -Iplatform/cmsis/inc \
        -Inet/ \
        -Inet/mac80211 \
        -Inet/core \
        -Inet/lib \
        -Inet/net_os \
        -Inet/security  \
        -Inet/include/  \
        -Inet/include/byteorder  \
        -Inet/include/linux  \
        -Inet/include/unaligned \
        -Inet/ftp/ftpd \
		-Inet/ftp/ftp_client \
		-Inet/ftp/tftp

#ifneq ($(LITEOS_SOCKET_ENABLE),1)
ccflags-y += \
        -I$(LWIP_ROOT)/src/include/lwip/apps/ \
    	-I$(LWIP_ROOT)/src/include/$(POSIX_PATH)/ \
        -I$(LWIP_ROOT)/src/include/  \
        -I$(LWIP_ROOT)/src/include/lwip/arch/  \
        -I$(LWIP_ROOT)/src/include/ipv4/  \
        -I$(LWIP_ROOT)/src/include/ipv6/  \
        -I$(LWIP_ROOT)/src/include/lwip/
#endif


ccflags-y +=  \
    -Iservices/nvrecord/ \
    -Iservices/wifi_app/sniffer \
    -Iservices/sys_time/ \
    -Iservices/nv_section/customparam_section/ \
    -Iservices/fs/mem \
    -Iservices/wifi_app/