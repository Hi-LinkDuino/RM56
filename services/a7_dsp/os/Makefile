ifeq ($(CPU),a7)

ifeq ($(A7_CMSIS_TEST_CASE),1)
obj-y := my_a7_test.o
obj-y += a7_mem_test.o
endif

ifeq ($(SPEECH_MSG_TEST_CASE),1)
obj-y += test/speech_msg_test/
endif

ifeq ($(PCM_AUDIO_INSTANCE_ENABLE),1)
obj-y += test/pcm_audio_instance/
endif

ifeq ($(A7_LITTLEFS_TEST),1)
obj-y += test/littlefs/
endif

ifeq ($(KERNEL),RHINO)
obj-y += rhino/
endif

ifeq ($(KERNEL),NUTTX)
obj-y += nuttx/
endif

ifeq ($(KERNEL),RTT)
obj-y += rtt/
endif

ifeq ($(KERNEL),LITEOS_A)
obj-y += liteos_a/
endif
endif
