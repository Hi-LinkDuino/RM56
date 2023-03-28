cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libcodec
obj-y :=

ifeq ($(DSP_ENABLE),1)
obj-y += mcu_codec.o
endif

ifeq ($(CHIP_SUBSYS), dsp)
obj-y += a7_codec.o
obj-y += dsp_codec.o
obj-y += bap_lc3.o
ifneq ($(CODEC_LIB), 1)
$(LIB_NAME)-y += ../../../multimedia/audio/codec/lc3/sneak_peek/
endif
obj-y += $(LIB_NAME).a
endif

ifeq ($(OPUS_CODEC_SUPPORT), 1)
obj-y += bap_opus.o
CFLAGS_dsp_codec.o += -DOPUS_CODEC_SUPPORT
ifeq ($(VOW_USE_48K_24BIT_CODEC), 1)
CFLAGS_bap_opus.o += -D__VOW_USE_48K_24BIT_CODEC__
endif

ifneq ($(VOWLAN_USB_INPUT_CH_NUM), )
CFLAGS_bap_opus.o += -D__WIFI_USB_AUDIO_CH__=$(VOWLAN_USB_INPUT_CH_NUM)
else
CFLAGS_bap_opus.o += -D__WIFI_USB_AUDIO_CH__=2
endif

endif


ccflags-y += \
             -Iservices/transq_msg \
             -Iservices/a7_dsp/common/ \
             -Iutils/ \
             -Iservices/multimedia/audio/codec/lc3/inc/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/Api/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/api_non_overlay/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/api_process/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/Common/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/Decoder/ \
             -Iservices/multimedia/audio/codec/lc3/sneak_peek/Encoder/ \
             -Iservices/a7_dsp/common/opus/include/

