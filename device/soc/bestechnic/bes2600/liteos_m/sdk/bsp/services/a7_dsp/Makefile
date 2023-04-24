obj-y := common/
ifeq ($(CPU),a7)
ifneq ($(KERNEL),)
obj-y += os/
endif

ifeq ($(LITTLEFS_ENABLE),1)
obj-y += ../../services/wifi_app/littlefs/
subdir-ccflags-y += -Iservices/wifi_app/littlefs/
ifeq ($(A7_LITTLEFS_TEST),1)
KBUILD_CPPFLAGS += -DA7_LITTLEFS_TEST
endif
endif

else
ifeq ($(SPEECH_MSG_TEST_CASE),1)
obj-y += os/test/speech_msg_test/
endif
ifeq ($(PCM_AUDIO_INSTANCE_ENABLE),1)
obj-y += os/test/pcm_audio_instance/
endif
endif

subdir-ccflags-y += -Iplatform/cmsis/inc/ca
