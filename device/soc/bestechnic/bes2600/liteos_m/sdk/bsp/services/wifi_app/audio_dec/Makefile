cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c :=
ifeq ($(AAC_CODEC_SUPPORT),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_dec_aac_adts.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_dec_aac_m4a.c))
endif
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_dec_mp3.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_dec.c))
ifeq ($(OPUS_CODEC_SUPPORT),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_dec_opus.c))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_enc_opus.c))
endif
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)audio_player.c))
# obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)fs_adapt.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
# for fs_adapt
# obj-y += ../../fs/

ccflags-y += \
	-Iservices/wifi_app/audio_dec/ \
	-Iservices/multimedia/audio/codec/minimp3/ \
	-Iservices/multimedia/audio/codec/minimp3/minimp3/ \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libAACdec/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libAACenc/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libFDK/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libMpegTPDec/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libMpegTPEnc/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libPCMutils/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libSBRdec/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libSBRenc/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libSYS/include \
	-Iservices/multimedia/audio/codec/fdkaac_codec/libAACdec/src
# ifeq ($(OPUS_CODEC_SUPPORT),1)
ccflags-y += \
	-Iservices/multimedia/audio/codec/opus/include \
	-Iservices/multimedia/audio/codec/opus/src/celt \
	-Iservices/multimedia/audio/codec/opus/src/celt/fixed \
	-Iservices/multimedia/audio/codec/opus/src/silk \
	-Iservices/multimedia/audio/codec/opus/src/silk/fixed \
	-Iservices/multimedia/audio/codec/opus/src/src \
	-Iservices/multimedia/audio/codec/opus/src
# endif

# audio player
ccflags-y += \
    -Iutils/cqueue \
	-Iservices/wifi_app/alsa_adapter/

# fs_adapt
ccflags-y += \
	-Iservices/fs/ \
	-Iservices/fs/mem

ccflags-y += -DBES_MAX_CHANNEL_SUPPORTED=2

ccflags-y += -Iservices/cp_server/