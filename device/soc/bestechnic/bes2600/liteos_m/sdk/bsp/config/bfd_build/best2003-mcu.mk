# included by best2600w_liteos/target.mk

######################################################
# PARAMS PRIORITY
# command line > [0] > [1] > [2] > [3] > [4]
# [0] device/board/fnlink/2700ibp/liteos_m/config.gni
# [1] vendor/xxx/product/config.sh
# [2] device/soc/.../sdk/tools/config.sh
# [3] device/soc/.../sdk/tools/build_lib_into_hmos_out.sh
# [4] device/soc/.../sdk/bsp/build_xxx_into_lib
######################################################


######################################################
# [4] from device/.../sdk/bsp/tools/build_best2600_ohos_release.sh
######################################################
include $(srctree)/config/bfd_build/$(CHIP)-a7-mcu.mk
#T=best2600w_liteos
#KERNEL=FREERTOS
export ARM_CMNS=0
export MODULE_KERNEL_STUB=1
export PSRAM_ENABLE=1
export NET_JANSSON_SUPPORT=0
export NET_CJSON_SUPPORT=0
export RF_TX_CONTROL_IO?=100
export WIFI_RF_OUT_FEM_SW=0
export WIFI_BT_COEX_FDD=0
export WIFI_BT_COEX_HYBRID=0
export PA_ENABLE_IO=26
#PA_ENABLE_IO=21
export UART1_IOMUX_INDEX=2
export NET_FTP_CLIENT_SUPPORT=0
export UTILS_BES_KV=1
export CP_BOOT=1
export FAST_TIMER_COMPENSATE=1 
export WAPI_SUPPORT=0
# export LITTLEFS_PARTITION_SUPPORT=0 
# export LITTLEFS_ENABLE=0
export USB_AUDIO_FS=0 
export AI_VOICE=0
export LLD_PRINT_ENABLE=0
export NET_LWIP_EXTERN=1 
export NET_LWIP_SUPPORT=0 
export SYS_TIME_IF=0 
export HISTREAMER_HTTP_HLS_EN=0
export HTTP_MEDIA_SUPPORT_HTTPS=0
export NET_LIBCURL_SUPPORT=0
export LPLAYER_FRAMEWORK_EN=0 
export MP3_CODEC_MINIMP3_DISENABLE=1
export DSP_BIN_NAME=a7_dsp

#same with CP, from BUILDOPT_CP
export PSRAMCP_SIZE=0x100000
export DUAL_BAND=1
export AP_MODE_SUPPORT=1



######################################################
# [3] from device/.../sdk/tools/build_lib_into_hmos_out.sh
######################################################
export NO_LIBC=1
export BOARD_OS_WRAP_MALLOC=1
export MODULE_KERNEL_STUB_INC=1
export EXTERN_ROOT_PATH=./../../../../../../../
export BUILD_TYPE?=DEBUG

######################################################
# [2] from device/.../sdk/tools/config.sh
######################################################
export NET_LWIP_EXTERN=2
export NET_LWIP_SUPPORT=0

######################################################
# [1] from vendor/.../product/config.sh
######################################################
export LIBC_INIT_AFTER_OS_INIT=1
export LPLAYER_FRAMEWORK_EN=1
export NET_LIBCURL_SUPPORT=1
export NET_MBEDTLS_SUPPORT=1
export HTTP_MEDIA_SUPPORT_HTTPS=1
export MP3_CODEC_MINIMP3_DISENABLE=0
export HISTREAMER_RENDER_SUPPORT=1


######################################################
# [0] from device/board/fnlink/2700ibp/liteos_m/config.gni
######################################################
LCD_SUPPORT ?= a064
ifeq ($(LCD_SUPPORT), rm69330)
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_RM69330
else ifeq ($(LCD_SUPPORT), a064)
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_A064 -DI2C1_IOMUX_INDEX=06 -DCONFIG_DSI_2LANE_MODE
else ifeq ($(LCD_SUPPORT), nt38350)
KBUILD_CPPFLAGS += -DCONFIG_DISPLAY_NT38350
endif


######################################################
# ADDED BY BESFD
######################################################


# use all free PSRAM as OS HEAP instead of OS_HEAP_SIZE
OS_HEAP_USE_ALL=1
ifeq ($(OS_HEAP_USE_ALL),1)
KBUILD_CPPFLAGS += -DOS_HEAP_USE_ALL
LDS_SECTION_FLAGS += -DOS_HEAP_USE_ALL
endif

POWER_ON_HOOK:=1
ifeq ($(POWER_ON_HOOK),1)
KBUILD_CPPFLAGS += -DPOWER_ON_HOOK
endif

#close A7_DSP_ENABLE will build error, so add a new switch
DSP_CLOSED_AT_BOOTUP:=1
ifeq ($(DSP_CLOSED_AT_BOOTUP),1)
KBUILD_CPPFLAGS += -DDSP_CLOSED_AT_BOOTUP
endif

#以下开关用于支持大屏大作小屏用，比如480x480方屏，如果当作240x480竖屏用，则定义LCD_USE_SMALL_SIZE_W=240
LCD_USE_SMALL_SIZE_W=0
ifneq ($(LCD_USE_SMALL_SIZE_W),0)
KBUILD_CPPFLAGS += -DLCD_USE_SMALL_SIZE_W=$(LCD_USE_SMALL_SIZE_W)
endif
LCD_USE_SMALL_SIZE_H=0
ifneq ($(LCD_USE_SMALL_SIZE_H),0)
KBUILD_CPPFLAGS += -DLCD_USE_SMALL_SIZE_H=$(LCD_USE_SMALL_SIZE_H)
endif

DISABLE_CONSOLE_INIT:=1
ifeq ($(DISABLE_CONSOLE_INIT),1)
KBUILD_CPPFLAGS += -DDISABLE_CONSOLE_INIT
endif

OH_UI_PROGRESS_TOUCHABLE := 1
ifeq ($(OH_UI_PROGRESS_TOUCHABLE), 1)
KBUILD_CPPFLAGS += -DOH_UI_PROGRESS_TOUCHABLE
endif