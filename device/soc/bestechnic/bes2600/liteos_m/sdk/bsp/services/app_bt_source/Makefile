
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/*.cpp))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)src/a2dp_encoder_cp/*.c))

ifeq ($(APP_BT_SOURCE_TEST),1)
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)tests/*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)tests/*.cpp))
endif

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(A2DP_AAC_ON),1)
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

subdir-ccflags-y += \
	-Iservices/overlay \
	-Iservices/resources \
	-Iservices/audioflinger \
	-Iservices/audio_process \
	-Iapps/audioplayers \
	-Iapps/audioplayers/a2dp_decoder \
	-Iservices/multimedia/audio/process/resample/include \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/codec/sbc/inc \
	-Iservices/multimedia/audio/codec/sbc/src/inc \
	$(BT_IF_INCLUDES) \
	-Iplatform/drivers/bt \
	-Iutils/cqueue \
	-Iutils/heap \
	-Iutils/lockcqueue \
	-Iutils/intersyshci \
	-Iapps/main \
	-Iapps/key \
	-Iapps/common \
	-Iapps/battery \
	$(BT_PROFILES_INCLUDES) \
	-Iservices/bt_app/a2dp_codecs/include \
	-Iservices/ibrt_middleware/inc \
	-Iservices/app_bt_source/inc \
	-Iservices/app_bt_source/src/a2dp_encoder_cp \
	-Iservices/bt_app/ \
	-Iservices/osif/ \
	-Iservices/tota/ \
	-Ithirdparty/audio_codec_lib/liblhdc-enc/inc \
	$(AAC_INCLUDES) \
	-Iplatform/drivers/cp_accel \
	-Iservices/norflash_api \
	-Itests/anc_usb

ifeq ($(BT_FA_ECC),1)
subdir-ccflags-y += -D__FASTACK_ECC_ENABLE__
endif

ifeq ($(APP_BT_SOURCE_TEST),1)
subdir-ccflags-y += -DAPP_BT_SOURCE_TEST
subdir-ccflags-y += \
	$(ESHELL_INCLUDES)
endif

