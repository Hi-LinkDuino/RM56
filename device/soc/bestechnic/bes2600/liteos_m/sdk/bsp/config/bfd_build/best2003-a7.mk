include $(srctree)/config/bfd_build/$(CHIP)-a7-mcu.mk

######################################################
# [4] from device/.../sdk/bsp/tools/build_best2600_ohos_release.sh
# make_a7
######################################################

#T=a7_dsp
export A7_UART=4
# PSRAMCP_SIZE=0x100000
export KERNEL=LITEOS_A
export A7_CMSIS_TEST_CASE=0
export A7_SRAM=0
export OPUS_CODEC_SUPPORT=1
export CONFIG_MCU_PSRAMUHS_SIZE=0x300000


######################################################
# [3] from device/.../sdk/tools/build_lib_into_hmos_out.sh
######################################################
export BUILD_TYPE?=DEBUG

######################################################
# [2] from device/.../sdk/tools/config.sh
######################################################
#OPT_SOC_A7

######################################################
# [1] from vendor/.../product/config.sh
######################################################
export OPUS_CODEC_SUPPORT=0



######################################################
# [0] from device/board/fnlink/2700ibp/liteos_m/config.gni
######################################################



######################################################
# ADDED BY BESFD
######################################################
