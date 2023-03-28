cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

ifeq ($(DSP_ENABLE),1)
obj-y += mcu_dummy_main.o
endif

ifeq ($(CHIP_SUBSYS), dsp)
obj-y += a7_dummy_main.o
ifeq ($(A7_ALI_LIB), 1)
obj-y += lib_a7_dummy.a
else
obj-y += dummy/
endif
endif

ifeq ($(A7_LITTLEFS_TEST),1)
KBUILD_CPPFLAGS += -DA7_LITTLEFS_TEST
endif

ccflags-y += \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iservices/a7_dsp/common/ \
             -Iservices/a7_dsp/prj/dummy/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Inet \
             -Inet/utils \
             -Iapps/common \
             -Iservices/a7_dsp/os/test/speech_msg_test
