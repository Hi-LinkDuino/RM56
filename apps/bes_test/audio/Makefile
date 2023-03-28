cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))

obj-y := $(obj_c:.c=.o) $(obj_cpp:.cpp=.o) $(obj_s:.S=.o)

ccflags-y += \
    -Iservices/wifi_app/audio_dec/ \
    -Iservices/wifi_app/alsa_adapter/ \
    -Iapps/bes_test/audio/ \
    -Iapps/bes_test/audio/aac/ \
    -Iapps/bes_test/audio/mp3/ \
    -Iapps/bes_test/audio/opus/ \
    -Iapps/bes_test/audio/pcm/

ifeq ($(LPLAYER_EN),1)
ccflags-y += \
    -Iservices/lplayer/component/include/ \
    -Iservices/lplayer/component/audio_util/include \
    -Iservices/lplayer/component/audio_format/include/ \
    -Iservices/lplayer/component/audio_codec/include \
    -Iservices/lplayer/component/audio_source/include/ \
    -Iservices/wifi_app/alsa_adapter/aplayer/ \
    -Iservices/lplayer/interface/include/ \
    -Irtos/liteos/liteos_m/kernel/include/ \
    -Irtos/liteos/liteos_m/ \
    -Irtos/liteos/liteos_m/utils/
endif

ifeq ($(BT_MEDIA_SUPPORT_LPLAYER),1)
ccflags-y += -Iservices/lplayer_wrapper/
endif