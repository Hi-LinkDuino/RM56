cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=
ccflags-y :=

ifneq ($(LPLAYER_SUPPORT_ABILITY_CONFIG), 1)
obj-y += component/
else
include $(cur_dir)/lplayer_config.mk
ifeq ($(AUDIO_DEMUXER_DEBUG_EN),1)
ccflags-y += -DAUDIO_DEMUXER_DEBUG_EN
endif

ifeq ($(MP4_AAC_RAW_TO_ADTS_EN),1)
ccflags-y += -DMP4_AAC_RAW_TO_ADTS_EN
endif

ifeq ($(AUDIO_SOURCE_DEBUG_EN), 1)
ccflags-y += -DAUDIO_SOURCE_DEBUG_EN
endif

ifeq ($(HTTP_SOURCE_LOCAL_FILE_TEST_HOOK_EN), 1)
ccflags-y += -DHTTP_SOURCE_LOCAL_FILE_TEST_HOOK_EN
endif

ifeq ($(HTTPS_CA_USE_FS_IO), 1)
HTTP_CA_FS_PATH = \"/music/curl.pem\"
ccflags-y += -DAUDIO_SOURCE_WIFI_CERT_FS_PATH=$(HTTP_CA_FS_PATH)
endif

ifeq ($(LPLAYER_FS_EN), 1)
ccflags-y += -DLPLAYER_FS_EN
ccflags-y += -Iservices/wifi_app/littlefs/flash_partition/
endif

LPLAYER_SOURCE_SUPPORT_WIFI ?= 1
ifeq ($(LPLAYER_SOURCE_SUPPORT_WIFI), 1)
ccflags-y += -DLPLAYER_SOURCE_SUPPORT_WIFI
endif
endif

export AUDIO_SOURCE_DEBUG_EN ?= 0
export AUDIO_DEMUXER_DEBUG_EN ?= 0
export AUDIO_DECODER_DEBUG_EN ?= 0
export AUDIO_BQ_DEBUG_EN ?= 0
export AUDIO_DECODER_MP3_DEBUG_EN ?= 0
export AUDIO_SINK_DEBUG_EN ?= 0
export AUDIO_SINK_SUPPORT_BT_AUDIO ?= 0

export HTTP_SOURCE_LOCAL_FILE_TEST_HOOK_EN ?= 0
export STREAM_SOURCE_SIMULATE ?= 0
ifeq ($(STREAM_SOURCE_SIMULATE), 1)
ccflags-y += -DSTREAM_SOURCE_SIMULATE
ccflags-y += -Iapps/alios/resource/
endif

export MP4_AAC_RAW_TO_ADTS_EN ?= 1

ifeq ($(KERNEL), RHINO)
subdir-ccflags-y += \
    -Irtos/rhino/cmsis/ \
    -Irtos/rhino/kernel/include/
endif

ifeq ($(LPLAYER_FRAMEWORK_EN), 1)
export LPLAYER_FRAMEWORK_PIPELINE_EN := 1
endif

ifeq ($(LPLAYER_FRAMEWORK_EN), 1)
obj-y += interface/
endif

ifeq ($(LPLAYER_FRAMEWORK_EN)_$(LPLAYER_FRAMEWORK_PIPELINE_EN), 1_0)
obj-y += framework_mp/
endif

ifeq ($(LPLAYER_DRIVER_EN), 1)
obj-y += driver/
endif

ifneq ($(LPLAYER_SRC_RELEASE),1)
ifeq ($(LPLAYER_FRAMEWORK_PIPELINE_EN), 1)
obj-y += pipeline/
obj-y += adam_media_impl/
endif
LIB_NAME :=  lib$(subst /,_,$(patsubst ../../%/,%,$(cur_dir)))
$(LIB_NAME)-y := $(obj-y)
obj-y := $(LIB_NAME).a
else  # !LPLAYER_SRC_RELEASE
LIB_NAME :=  lib$(subst /,_,$(patsubst ../../%/,%,$(cur_dir)))
$(LIB_NAME)-y := $(obj-y)
obj-y := $(LIB_NAME).a
ifeq ($(LPLAYER_FRAMEWORK_PIPELINE_EN), 1)
obj-y += pipeline/
obj-y += adam_media_impl/
endif
endif # LPLAYER_SRC_RELEASE

export LPLAYER_MEM_DEBUG ?= 0
ifeq ($(LPLAYER_MEM_DEBUG),1)
obj-y += mem_debug/mem_helper.o
ccflags-y += -Iservices/lplayer/mem_debug/
subdir-ccflags-y += -Iservices/lplayer/mem_debug/
endif