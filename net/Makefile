ifneq ($(GEN_SDK_LIB_NET),1)
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

# NET_LIB_NAME := libnet
# ifneq ($(SDK),1)
ifneq ($(WIFI_ON_CP),1)
ifneq ($(WIFI_RF_TEST),1)
obj-y := core/ drv/ net80211/
endif
obj-y += wpa_supplicant_l/
else
ifeq ($(CP_BUILD),1)
obj-y := core/ drv/ net80211/ wpa_supplicant_l/
endif
endif
obj-y += wifi/ net_os/ utils/ security/


ifeq ($(NET_LWIP_SUPPORT),1)
ifeq ($(NET_LWIP_EXTERN),0)
obj-y += $(LWIP_VER)/
else ifeq ($(NET_LWIP_EXTERN),1)
obj-y += $(LWIP_VER)/src/netif/ethernetif.o
endif
ifeq ($(AP_MODE_SUPPORT),1)
obj-y += dhcpd/
endif
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
obj-y += mbedtls_client/
endif

ifeq ($(NET_NGHTTP2_CLIENT_SUPPORT),1)
obj-y += nghttp2_client/
endif

# $(NET_LIB_NAME)-y := $(src_obj)
# endif

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y += $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
# obj-y += $(NET_LIB_NAME).a

ifeq ($(NET_LIBRWS_SUPPORT),1)
obj-y += librws/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
obj-y += nghttp2/
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
obj-y += mbedtls/
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
obj-y += libcurl/
endif

ifeq ($(NET_TINYCBOR_SUPPORT),1)
obj-y += tinycbor/
endif

ifeq ($(NET_MQTT_SUPPORT),1)
obj-y += mqtt/
endif

ifeq ($(NET_COAP_SUPPORT),1)
obj-y += coap/
endif

ifeq ($(NET_JANSSON_SUPPORT),1)
obj-y += jansson/
endif

ifeq ($(NET_JSMN_SUPPORT),1)
obj-y += jsmn/
endif

ifeq ($(NET_CJSON_SUPPORT),1)
obj-y += cJSON/
endif

ifeq ($(NET_JSONC_SUPPORT),1)
obj-y += json-c/
endif

ifeq ($(NET_FTP_APP_SUPPORT),1)
obj-y += ftp/
endif

ifeq ($(NET_OPENSSL_SUPPORT),1)
obj-y += openssl/
endif

ifeq ($(NET_TELNET_SERVER_SUPPORT),1)
obj-y += telnet/
endif

ifeq ($(NET_COAP_SUPPORT),1)
obj-y += coap/
endif

else #ifneq ($(GEN_SDK_LIB_NET),1)


cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libnet
ifneq ($(SDK),1)
ifneq ($(WIFI_ON_CP),1)
ifneq ($(WIFI_RF_TEST),1)
src_obj := core/ drv/ net80211/
endif
src_obj += wpa_supplicant_l/
else
ifeq ($(CP_BUILD),1)
src_obj := core/ drv/ net80211/ wpa_supplicant_l/
endif
endif
src_obj += wifi/ net_os/ utils/ security/

ifeq ($(NET_LWIP_SUPPORT),1)
ifeq ($(NET_LWIP_EXTERN),0)
src_obj += $(LWIP_VER)/
else ifeq ($(NET_LWIP_EXTERN),1)
src_obj += $(LWIP_VER)/src/netif/ethernetif.o
endif
ifeq ($(AP_MODE_SUPPORT),1)
src_obj += dhcpd/
endif
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
src_obj += mbedtls_client/
endif

ifeq ($(NET_NGHTTP2_CLIENT_SUPPORT),1)
src_obj += nghttp2_client/
endif

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
src_obj += $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(NET_LIBRWS_SUPPORT),1)
src_obj += librws/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
src_obj += nghttp2/
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
src_obj += mbedtls/
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
src_obj += libcurl/
endif

ifeq ($(NET_TINYCBOR_SUPPORT),1)
src_obj += tinycbor/
endif

ifeq ($(NET_MQTT_SUPPORT),1)
src_obj += mqtt/
endif

ifeq ($(NET_COAP_SUPPORT),1)
src_obj += coap/
endif

ifeq ($(NET_JANSSON_SUPPORT),1)
src_obj += jansson/
endif

ifeq ($(NET_JSMN_SUPPORT),1)
src_obj += jsmn/
endif

ifeq ($(NET_CJSON_SUPPORT),1)
src_obj += cJSON/
endif

ifeq ($(NET_JSONC_SUPPORT),1)
src_obj += json-c/
endif

ifeq ($(NET_FTP_APP_SUPPORT),1)
src_obj += ftp/
endif

# src_obj += json-c_wrap/
# only used for BEST2001 METAL_ID_0

ifeq ($(NET_KCP_SUPPORT),1)
src_obj += kcp/
endif

ifeq ($(NET_OPENSSL_SUPPORT),1)
src_obj += openssl/
endif

ifeq ($(NET_TELNET_SERVER_SUPPORT),1)
src_obj += telnet/
endif

ifeq ($(DLNA_SUPPORT),1)
src_obj += dlna/
endif

ifeq ($(AIRPLAY_SUPPORT),1)
src_obj += airplay/
endif

ifeq ($(MFI_WAC_SUPPORT),1)
src_obj += wac/
endif

$(LIB_NAME)-y := $(src_obj)
endif #ifneq ($(SDK),1)

obj-y :=

ifeq ($(NET_LWIP_SUPPORT)_$(NET_LWIP_EXTERN),1_1)
LIB_NAME_LWIP := libnetlwip
ifneq ($(SDK),1)
src_obj := $(LWIP_VER)/
$(LIB_NAME_LWIP)-y := $(src_obj)
endif
obj-y += $(LIB_NAME_LWIP).a
endif

ifeq ($(NET_LIBRWS_SUPPORT),1)
obj-y += librws/
endif

obj-y += $(LIB_NAME).a
endif #ifneq ($(GEN_SDK_LIB_NET),1)

subdir-ccflags-y +=\
			-Inet\
			-Inet/net_os\
			-Inet/utils\
			-Inet/include/os/\
			-Inet/coap/libcoap/include/coap2/ \
			-Inet/coap/port/include/ \
			-Iservices/bt_profiles_enhanced/inc/\
			-Iservices/nvrecord/\
			-Iservices/sys_time/ \
			-Iservices/wifi_app/airkiss/ \
			-Iservices/cp_boot/ \
			-Iservices/nv_section/factory_section \
			-Iplatform/drivers/ana \
			-Iplatform/drivers/wifi/$(CHIP) \
			-Iutils/heap/ \

ifeq ($(NET_LWIP_SUPPORT),1)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include
ifneq ($(LIBC_ADAPT_EN),1)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)
endif # !LIBC_ADAPT_EN
endif

subdir-ccflags-y += -Inet/libcurl/library/

ifeq ($(ALIOS_APP_SUPPORT),1)
ccflags-y += -Iapps/alios/genie/include/
endif

ifneq ($(SKB_SRAM), )
subdir-ccflags-y += -DSKB_SRAM=$(SKB_SRAM)
endif

subdir-ccflags-y += -DCW1260_DETECTION_LOGIC
subdir-ccflags-y += -DP2P_MULTIVIF
#subdir-ccflags-y += -DCUSTOM_FEATURE
#subdir-ccflags-y += -DKEY_FRAME_SW_RETRY
#subdir-ccflags-y += -DKEY_FRAME_FILTER
#subdir-ccflags-y += -DBES_MINSTREL_ANTI_NOISE
#subdir-ccflags-y += -DBES_SOFT_AGC_ANTI_NOISE
#subdir-ccflags-y += -DMCAST_FWDING
subdir-ccflags-y += -DCW1200_SUSPEND_RESUME_FILTER_ENABLE
#subdir-ccflags-y += -DAP_AGGREGATE_FW_FIX
subdir-ccflags-y += -DAP_HT_CAP_UPDATE
subdir-ccflags-y += -DCONFIG_CW1200_USE_STE_EXTENSIONS
#subdir-ccflags-y += -DCONFIG_CW1200_5GHZ_SUPPORT

ifeq ($(ALIOS_SUPPORT),1)
subdir-ccflags-y += -DQUICK_SCAN_ENABLED
endif

ifeq ($(P2P_MODE_SUPPORT),1)
subdir-ccflags-y += -DP2P_MLME_SUPPORT
endif

ifeq ($(ALSA_RESAMPLE_USER_HOOK_EN),1)
ccflags-y += -Iservices/wifi_app/alsa_adapter/
ccflags-y += -DALSA_RESAMPLE_USER_HOOK_EN
endif

ifeq ($(NET_OPENSSL_SUPPORT),1)
subdir-ccflags-y += -Inet/openssl/include/
endif

ifneq ($(ALLOW_WARNING),1)
KBUILD_CPPFLAGS += -Werror
KBUILD_CFLAGS += -Werror
endif

ifdef DEV

ifeq ($(NET_LWIP_SUPPORT)_$(NET_LWIP_EXTERN),1_0)
obj-y += $(LWIP_VER)/

subdir-ccflags-y += -I$(LWIP_ROOT)/src/include -Inet/utils \
				-I$(LWIP_ROOT)/src/include/byteorder \
				-I$(LWIP_ROOT)/src/include/os \
				-I$(LWIP_ROOT)/src/include/unaligned

#ifneq ($(LIBC_ADAPT_EN),1)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)
#endif # !LIBC_ADAPT_EN

endif

endif