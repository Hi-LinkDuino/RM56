
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

mmlib_obj :=

mmlib_obj += algorithm/fft/src/

mmlib_obj += audio/process/adp/src/
mmlib_obj += audio/process/anc/src/
mmlib_obj += audio/process/psap/src/
mmlib_obj += audio/process/fir2iir/src/
mmlib_obj += audio/process/common/src/
mmlib_obj += audio/process/drc/src/
mmlib_obj += audio/process/filters/src/
mmlib_obj += audio/process/limiter/src/
mmlib_obj += audio/process/resample/src/
mmlib_obj += audio/process/integer_resampling/src/
mmlib_obj += audio/process/floatlimiter/src/
mmlib_obj += audio/process/adj_mc/src/
mmlib_obj += audio/process/reverb/src/
mmlib_obj += audio/process/sidetone/src/
mmlib_obj += audio/process/sbcplc/src/
mmlib_obj += audio/process/anc/cfg/
mmlib_obj += audio/process/filters/cfg/
mmlib_obj += audio/process/resample/coef/

ifeq ($(FM_ENABLE),1)
mmlib_obj += fm/
endif

ifeq ($(SONIC_EN),1)
mmlib_obj += audio/process/sonic/
endif

ifeq ($(A2DP_OPUS_ON),1)
#mmlib_obj += opus121/src/
endif

ifeq ($(BT_APP),1)
mmlib_obj += audio/codec/sbc/src/
endif

ifeq ($(DSP_CODEC_SUPPORT),0)
ifeq ($(A2DP_LC3_ON),1)
mmlib_obj += audio/codec/lc3/sneak_peek/
endif
endif

ifeq ($(A2DP_AAC_ON),1)
aac_obj :=


ifeq ($(FDKAAC_VERSION), fdkaac_codec_2_0_1_full)
aac_obj += audio/codec/$(FDKAAC_VERSION)/
else ifeq ($(FDKAAC_VERSION), fdkaac_codec_2_0_1)
aac_obj += audio/codec/$(FDKAAC_VERSION)/libAACdec/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libAACenc/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libFDK/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libMpegTPDec/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libMpegTPEnc/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libPCMutils/src/
#aac_obj += audio/codec/$(FDKAAC_VERSION)/libSBRdec/src/
#aac_obj += audio/codec/$(FDKAAC_VERSION)/libSBRenc/src/
#aac_obj += audio/codec/$(FDKAAC_VERSION)/libSACdec/src/
#aac_obj += audio/codec/$(FDKAAC_VERSION)/libSACenc/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libDRCdec/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libArithCoding/src/
aac_obj += audio/codec/$(FDKAAC_VERSION)/libSYS/src/
else
aac_obj += audio/codec/fdkaac_codec/libAACdec/src/
aac_obj += audio/codec/fdkaac_codec/libAACenc/src/
aac_obj += audio/codec/fdkaac_codec/libFDK/src/
aac_obj += audio/codec/fdkaac_codec/libMpegTPDec/src/
aac_obj += audio/codec/fdkaac_codec/libMpegTPEnc/src/
aac_obj += audio/codec/fdkaac_codec/libPCMutils/src/
aac_obj += audio/codec/fdkaac_codec/libSBRdec/src/
aac_obj += audio/codec/fdkaac_codec/libSBRenc/src/
aac_obj += audio/codec/fdkaac_codec/libSYS/src/
endif
ifeq ($(A2DP_CP_ACCEL),1)
fdkaac_codec-y := $(aac_obj)
mmlib_obj += fdkaac_codec.o
else
mmlib_obj += $(aac_obj)
endif
ifeq ($(ROM_UTILS_ON),1)
subdir-ccflags-y += -DFDK_FUNC_ATTR=WEAK
else
subdir-ccflags-y += -DFDK_FUNC_ATTR=
endif
endif

# *****************
# for wifi app
# *****************
ifeq ($(LPLAYER_EN), 1)
ifeq ($(OPUS_CODEC_SUPPORT),1)
mmlib_obj += audio/codec/opus/src/
endif

ifneq ($(MP3_CODEC_MINIMP3_DISENABLE), 1)
ifeq ($(MP3_CODEC_USE_MINIMP3_2), 1)
mmlib_obj += audio/codec/minimp3_2/
else
mmlib_obj += audio/codec/minimp3/
endif
endif

ifeq ($(DBLMP4DEMUX_CODEC_SUPPORT),1)
mmlib_obj += audio/codec/dlb_mp4demux/
endif

ifeq ($(MINIMP4_DEMUXER_SUPPORT),1)
mmlib_obj += audio/codec/minimp4/
endif
ifeq ($(FLAC_SUPPORT),1)
mmlib_obj += audio/codec/flac/
endif
else #LPLAYER_EN
ifeq ($(OPUS_CODEC_SUPPORT),1)
mmlib_obj += audio/codec/opus/src/
endif
endif #LPLAYER_EN
#mmlib_obj += rbcodec/src/

ifeq ($(SPEECH_LIB),1)
mmlib_obj += speech/src/
endif

MULTIMEDIA_LIB_NAME := $(CHIP)_libmultimedia

ifeq ($(ROM_UTILS_ON),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_romaac
endif

ifeq ($(A2DP_CP_ACCEL),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_cp
endif

ifeq ($(ANC_APP),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_anc
endif

ifeq ($(USE_TRACE_ID), 1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_log_compressed
endif

$(MULTIMEDIA_LIB_NAME)-y := $(mmlib_obj)

obj-y := $(MULTIMEDIA_LIB_NAME).a

subdir-ccflags-y += \
	-Iutils/heap \
	-Iinclude/cmsis_dsp \
	-Iinclude/cmsis_nn \
	-Iservices/cp_server
