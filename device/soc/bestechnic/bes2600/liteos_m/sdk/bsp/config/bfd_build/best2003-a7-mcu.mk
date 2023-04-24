include $(srctree)/config/bfd_build/$(CHIP).mk

######################################################
# from device/.../sdk/bsp/tools/build_best2600_ohos_release.sh
# BUILDOPT_M33
######################################################

# used for a7&mcu
DSP_BIN_NAME=a7_dsp
CHIP=best2003
A7_DSP_ENABLE=1
DSP_AUDIO_TASK=1
DSP_CODEC_SUPPORT=0
PSRAMUHS_ENABLE=1
PSRAM_ENABLE=1
SPEECH_MSG_ENABLE=1
SPEECH_MSG_TEST_CASE=0
A7_PROJECT=dummy

