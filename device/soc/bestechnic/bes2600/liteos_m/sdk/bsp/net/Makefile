cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(NUTTX_BUILD),1)
ifneq ($(DUAL_BAND),1)
LIB_NAME := libnet
else
LIB_NAME := libnet-$(RF_TX_CONTROL_IO)
endif
else
LIB_NAME := libnet
endif

ifeq ($(SDK),1)

obj-y += $(LIB_NAME).a

else
ifneq ($(WIFI_ON_CP),1)
ifneq ($(WIFI_RF_TEST),1)
obj-y += core/ drv/ net80211/
endif
obj-y += wpa_supplicant_l/
else
ifeq ($(CP_BUILD),1)
obj-y := core/ drv/ net80211/ wpa_supplicant_l/
endif
endif
obj-y += wifi/ net_os/ utils/ security/

ifeq ($(NET_MBEDTLS_SUPPORT),1)
obj-y += mbedtls_client/
endif

ifeq ($(IPCAM_SUPPORT),1)
obj-y += ipcam/
endif

ifeq ($(NET_MUSIC_SUPPORT), 1)
obj-y += net_music/

ifeq ($(LWIP_HOOK_SOCKET), 1)
subdir-ccflags-y += -DAIRPLAY_HOOK_SETSOCKOPT_ADD
endif

ifeq ($(LWIP_AUTOIP_SUPPORT), 1)
subdir-ccflags-y += -DCONFIG_AUTOIP
endif
endif # NET_MUSIC_SUPPORT

$(LIB_NAME)-y := $(obj-y)
obj-y := $(LIB_NAME).a

endif

ifeq ($(NET_LWIP_SUPPORT),1)
obj-y += $(LWIP_VER)/
subdir-ccflags-y += -I$(LWIP_VER)/src/include/lwip/
ifeq ($(AP_MODE_SUPPORT),1)
obj-y += dhcpd/
endif
endif

ifeq ($(NET_LWIP_EXTERN),1)
LIB_NAME := ethernetif
lib-y := $(LWIP_VER)/src/netif/ethernetif.o
$(LIB_NAME)-y := $(lib-y)
obj-y += $(LIB_NAME).a
endif

ifeq ($(NUTTX_BUILD),1)
ifneq ($(CHIP),best2001)
obj-y += wifi/$(CHIP)/wifi_power_cfg.o
endif
endif

ifeq ($(NET_NGHTTP2_CLIENT_SUPPORT),1)
obj-y += nghttp2_client/
endif

ifeq ($(NET_LIBRWS_SUPPORT),1)
obj-y += librws/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
obj-y += nghttp2/
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
obj-y += mbedtls/
ifeq ($(NET_MBEDTLS_MOD_ALT_TEST),1)
obj-y += ../tests/zmodp_test/
endif
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

ifeq ($(NET_HTTP_CIENT),1)
obj-y += https_client/
endif

ifeq ($(IMS_SUPPORT),1)
obj-y += ims/
endif

ifeq ($(LTE_SUPPORT),1)
obj-y += lte/
endif

ifeq ($(IPSEC_SUPPORT),1)
obj-y += ipsec/
endif

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

ifeq ($(NET_LWIP_EXTERN),2)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include
ifneq ($(LIBC_ADAPT_EN),1)
subdir-ccflags-y += -I$(LWIP_ROOT)/src/include/$(POSIX_PATH)
endif # !LIBC_ADAPT_EN
endif

ifeq ($(NET_MUSIC_SUPPORT), 1)
ifeq ($(LWIP_HOOK_SOCKET), 1)
subdir-ccflags-y += -DAIRPLAY_HOOK_SETSOCKOPT_ADD
endif
ifeq ($(LWIP_AUTOIP_SUPPORT), 1)
subdir-ccflags-y += -DCONFIG_AUTOIP
endif
endif

subdir-ccflags-y += -Inet/libcurl/library/

ifeq ($(ALIOS_APP_SUPPORT),1)
ccflags-y += -Iapps/alios/genie/include/
endif

ifneq ($(SKB_SRAM), )
subdir-ccflags-y += -DSKB_SRAM=$(SKB_SRAM)
endif

ifeq ($(CHIP),best2002)
subdir-ccflags-y += -DTRANSQ_WIFI_EXTEND
endif

subdir-ccflags-y += -DWIFI_BT_COEXIST_EPTA_ENABLE
subdir-ccflags-y += -DCW1260_DETECTION_LOGIC
subdir-ccflags-y += -DP2P_MULTIVIF
#subdir-ccflags-y += -DCUSTOM_FEATURE
ifeq ($(WIFI_DRV_KEY_FRAME_FILTER),1)
subdir-ccflags-y += -DKEY_FRAME_FILTER
endif
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
#export NET_OPENSSL_PATH ?= openssl-1.1.1m
export NET_OPENSSL_PATH ?= openssl
ifeq ($(OWE_SUPPORT),1)
# when NET_OPENSSL_PATH is openssl, we need set CONFIG_SHRINK_EVP.
# otherwise don't set CONFIG_SHRINK_EVP.
subdir-ccflags-y += -DCONFIG_SHRINK_EVP
endif
subdir-ccflags-y += -Inet/$(NET_OPENSSL_PATH)/ \
                    -Inet/$(NET_OPENSSL_PATH)/include/\
                    -Inet/$(NET_OPENSSL_PATH)/crypto/modes/ \
                    -Inet/$(NET_OPENSSL_PATH)/crypto/ec/curve448 -Inet/$(NET_OPENSSL_PATH)/crypto/ec/curve448/arch_32
endif

ifneq ($(ALLOW_WARNING),1)
KBUILD_CPPFLAGS += -Werror
KBUILD_CFLAGS += -Werror
endif
KBUILD_CFLAGS += -Wno-implicit-fallthrough
