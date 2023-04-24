
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := a2dp_decoder.o
obj-y += a2dp_decoder_sbc.o
CFLAGS_a2dp_decoder_sbc.o += -O3

ifeq ($(A2DP_AAC_ON),1)
obj-y += a2dp_decoder_aac_lc.o
CFLAGS_a2dp_decoder_aac_lc.o += -O3
AAC_INCLUDES =  \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libAACdec/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libAACenc/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libFDK/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libMpegTPDec/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libMpegTPEnc/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libPCMutils/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libSBRdec/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libSBRenc/include \
	-Iservices/multimedia/audio/codec/$(FDKAAC_VERSION)/libSYS/include
else
AAC_INCLUDES =
endif

ifeq ($(A2DP_SCALABLE_ON),1)
obj-y += a2dp_decoder_scalable.o
CFLAGS_a2dp_decoder_scalable.o += -O3
SCALABLE_INCLUDES =  \
	-Ithirdparty/audio_codec_lib/scalable
else
SCALABLE_INCLUDES =
endif

ifeq ($(A2DP_LHDC_ON),1)
obj-y += a2dp_decoder_lhdc.o
CFLAGS_a2dp_decoder_lhdc.o += -O3
LHDC_INCLUDES =  \
	-Ithirdparty/audio_codec_lib/liblhdc-dec/inc
else
LHDC_INCLUDES =
endif

ifeq ($(A2DP_LDAC_ON),1)
obj-y += a2dp_decoder_ldac.o
CFLAGS_a2dp_decoder_ldac.o += -O3
LDAC_INCLUDES =  \
	-Ithirdparty/audio_codec_lib/ldac/inc
else
LDAC_INCLUDES =
endif

ifeq ($(A2DP_LC3_ON),1)
obj-y += a2dp_decoder_lc3.o
#CFLAGS_a2dp_decoder_lc3.o += -O3
LC3_INCLUDES =  \
	-Iservices/multimedia/audio/codec/lc3/inc
else
LC3_INCLUDES =
endif

ccflags-y := \
	$(AAC_INCLUDES) \
	$(SCALABLE_INCLUDES) \
	$(LHDC_INCLUDES) \
	$(LDAC_INCLUDES) \
	$(LC3_INCLUDES) \
	-Iservices/audio_process \
	-Iservices/app_ai/inc \
	-Iservices/resources \
	-Iservices/bt_app \
	-Iservices/bt_app/a2dp_codecs/include \
	$(BT_IF_INCLUDES) \
	-Iplatform/drivers/uarthci \
	-Iutils/cqueue \
	-Iservices/audio_dump/include \
	-Iservices/multimedia/speech/inc \
	-Iservices/multimedia/rbcodec/inc \
	-Iservices/multimedia/audio/process/eq/include \
	-Iservices/multimedia/audio/process/resample/include \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/sbcplc/include \
	-Iservices/multimedia/fm/inc \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iservices/overlay \
	-Iservices/norflash_api \
	-Iservices/nvrecord \
	-Iservices/nv_section/log_section \
	-Iapps/main \
	-Iapps/audioplayers/rbplay/ \
	-Iapps/audioplayers/a2dp_decoder \
	-Iutils/safe_func \
	-Iutils/list \
	-Iutils/heap \
	-Iplatform/drivers/ana \
	-Iapps/apptester \
	-Iapps/key  \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iservices/multimedia/audio/codec/sbc/src/inc \
	-Iplatform/drivers/bt \
	-Iutils/crc32

ifeq ($(A2DP_LHDC_ON),1)
ccflags-y += -Iservices/bt_if_enhanced/lhdc_license
endif

ifeq ($(IBRT),1)
ifeq ($(IBRT_UI_V1),1)
ccflags-y += \
    -Iservices/ibrt_core/inc \
    -Iservices/ibrt_ui/inc \
    -Iservices/app_ibrt/inc
endif

ifeq ($(IBRT_CORE_V2),1)
ccflags-y += \
    -Iservices/app_ibrt_v2/inc \
    -Iservices/ibrt_core_v2/inc \
    -Iservices/custom_api
endif

ifeq ($(IBRT_UI_V2),1)
subdir-ccflags-y += \
    -Iservices/ibrt_ui_v2/inc
endif

subdir-ccflags-y += \
    -Iservices/ibrt_middleware/inc
endif

ifeq ($(A2DP_CP_ACCEL),1)
obj-y += a2dp_decoder_cp.o
ccflags-y += -Iplatform/drivers/cp_accel
endif

ifeq ($(DSP_CODEC_SUPPORT),1)
ccflags-y += -Iservices/a7_dsp/common/codec
endif

ifeq ($(A2DP_TRACE_CP_ACCEL),1)
ccflags-y += -DA2DP_TRACE_CP_ACCEL
endif

ifeq ($(A2DP_TRACE_DEC_TIME),1)
ccflags-y += -DA2DP_TRACE_DEC_TIME
endif

ifeq ($(A2DP_TRACE_CP_DEC_TIME),1)
ccflags-y += -DA2DP_TRACE_CP_DEC_TIME
endif

ifeq ($(A2DP_HEAP_DEBUG),1)
CFLAGS_a2dp_decoder.o += -DA2DP_HEAP_DEBUG
endif
ifeq ($(A2DP_CP_HEAP_DEBUG),1)
CFLAGS_a2dp_decoder_cp.o += -DA2DP_CP_HEAP_DEBUG
endif

ifeq ($(PLAYBACK_FORCE_48K),1)
CFLAGS_a2dp_decoder.o += -DPLAYBACK_FORCE_48K
endif

ifeq ($(A2DP_SBC_PLC_ENABLED),1)
CFLAGS_a2dp_decoder_sbc.o += -DA2DP_SBC_PLC_ENABLED
endif

ifeq ($(AUDIO_REVERB),1)
ccflags-y += -D__AUDIO_REVERB__
endif
