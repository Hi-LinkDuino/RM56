cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y :=

ifeq ($(DSP_ENABLE),1)
export CONFIG_A7_BIN_OFFSET ?= 0x0
ifeq ($(CONFIG_A7_BIN_OFFSET),0x0)
obj-y += mcu_slave_code.o
endif
obj-y += mcu_slave_init.o
ifeq ($(MCU_SLAVE_FS_EN),1)
obj-y += mcu_slave_fs_bin.o
endif
obj-y += mcu_audio.o
obj-y += mcu_cmd.o
obj-y += a7_cmd.o
obj-y += data_dump.o
obj-y += aud_param.o
obj-y += a7_dump.o
obj-y += heart_beat.o
obj-y += pcm_audio.o
# ifeq ($(DSP_IMAGE_COMP),1)
# obj-y += decom/
# endif
ifeq ($(SPEECH_MSG_ENABLE),1)
obj-y += speech_msg.o
endif

ifeq ($(DSP_CODEC_SUPPORT),1)
obj-y += codec/
endif

ifeq ($(DSP_AUDIO_TASK),1)
CFLAGS_mcu_slave_init.o += -D__A7_DSP_AUDIO_TASK__
endif

ifeq ($(DSP_CODEC_SUPPORT),1)
CFLAGS_mcu_slave_init.o += -D__A7_DSP_CODEC__
endif

CFLAGS_mcu_slave_init.o += -DCONFIG_A7_BIN_OFFSET=$(CONFIG_A7_BIN_OFFSET)

ifeq ($(NUTTX_BUILD),1)

export DSP_BIN_NAME ?= nuttx_a7
export DSP_BIN_TEXT_NAME ?= nuttx_a7_text
ifeq ($(DSP_USE_PSRAM),1)
$(obj)/mcu_slave_code.o : $(NUTTX_ROOT)/$(DSP_BIN_NAME).bin $(NUTTX_ROOT)/$(DSP_BIN_TEXT_NAME).bin
AFLAGS_mcu_slave_code.o += -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -DDSP_BIN_TEXT_NAME=$(DSP_BIN_TEXT_NAME).bin -I$(NUTTX_ROOT)
else
$(obj)/mcu_slave_code.o : $(NUTTX_ROOT)/$(DSP_BIN_NAME).bin
AFLAGS_mcu_slave_code.o += -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -I$(NUTTX_ROOT)
endif

else
export DSP_BIN_NAME ?= a7_dsp
$(obj)/mcu_slave_code.o : out/$(T)/$(DSP_BIN_NAME).bin
AFLAGS_mcu_slave_code.o += -DDSP_BIN_NAME=$(DSP_BIN_NAME).bin -Iout/$(T)

out/$(T)/$(DSP_BIN_NAME).bin : $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin
ifeq ($(DSP_IMAGE_COMP), 1)
	$(srctree)/tools/lzma e $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin $(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin.lzma
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin.lzma,$(srctree)/$@)
else
	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin,$(srctree)/$@)
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).elf,$(srctree)/$(@:.bin=.elf))
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).map,$(srctree)/$(@:.bin=.map))
#	$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).lst,$(srctree)/$(@:.bin=.lst))
endif

$(srctree)/out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin : FORCE
	$(call echo-help,)
	$(call echo-help,INFO    Please make sure DSP bin is up to date: out/$(DSP_BIN_NAME)/$(DSP_BIN_NAME).bin)
# We cannot build $(DSP_BIN_NAME) automatically here, because the env inherited by sub-make cannot be cleaned:
# E.g., KBUILD_SRC, TC, CHIP_HAS_CP, ...
	$(call echo-help,)

ifeq ($(MCU_SLAVE_FS_EN),1)
export FS_BIN_NAME ?= littlefs
$(obj)/mcu_slave_fs_bin.o : out/$(T)/$(FS_BIN_NAME).bin

AFLAGS_mcu_slave_fs_bin.o += -DFS_BIN_NAME=$(FS_BIN_NAME).bin -Iout/$(T)
out/$(T)/$(FS_BIN_NAME).bin : $(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin
ifeq ($(DSP_IMAGE_COMP), 1)
	@$(srctree)/tools/lzma e $(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin $(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin.lzma
	@$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin.lzma,$(srctree)/$@)
else
	@$(call CMDCPFILE,$(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin,$(srctree)/$@)
endif

$(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin : FORCE
	@$(srctree)/tools/mklittlefs -c $(srctree)/fsdata/dsp -d 0 -b 4096 -s $(PSRAMFS_SIZE) $(srctree)/out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin
	$(call echo-help,)
	$(call echo-help,INFO    Please make sure fs bin is up to date: out/$(DSP_BIN_NAME)/$(FS_BIN_NAME).bin)
	$(call echo-help,)
endif

endif

endif

ifeq ($(CHIP_SUBSYS), dsp)
obj-y += a7_main.o
obj-y += a7_audio.o
obj-y += data_dump.o
obj-y += aud_param.o
obj-y += a7_dump.o
obj-y += a7_cmd.o
obj-y += pcm_audio.o
ifeq ($(RTOS),1)
#obj-y += my_a7_test.o
obj-y += heart_beat.o
endif
# solo SRAM for alg
ifeq ($(A7_SRAM),1)
obj-y += ../../../net/utils/net_memory.o
endif

ifneq ($(DEBUG_PORT),)
CFLAGS_a7_main.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifeq ($(NO_CLOCK_INIT),1)
CFLAGS_a7_main.o += -DNO_CLOCK_INIT
endif
ifeq ($(NO_PMU),1)
CFLAGS_a7_main.o += -DNO_PMU
endif
ifeq ($(NO_LPU_26M),1)
CFLAGS_a7_main.o += -DNO_LPU_26M
endif
ifeq ($(NO_PLL),1)
CFLAGS_a7_main.o += -DNO_PLL
endif
ifeq ($(NO_SPI),1)
CFLAGS_a7_main.o += -DNO_SPI
endif
ifeq ($(MBW_TEST),1)
CFLAGS_a7_main.o += -DMBW_TEST
endif
ifeq ($(DSP_AUDIO_TASK),1)
CFLAGS_a7_main.o += -D__A7_DSP_AUDIO_TASK__
endif
ifeq ($(DSP_CODEC_SUPPORT),1)
CFLAGS_a7_main.o += -D__A7_DSP_CODEC__
endif
ifeq ($(SPEECH_MSG_ENABLE),1)
obj-y += speech_msg.o
endif
ifeq ($(OPUS_CODEC_SUPPORT),1)
obj-y += opus/
endif
ifeq ($(DSP_CODEC_SUPPORT),1)
obj-y += codec/
endif
endif

ifeq ($(A7_DSP_HEARTBEAT_CHECK), 1)
CFLAGS_heart_beat.o += -DA7_DSP_HEARTBEAT_CHECK
endif

ifneq ($(CFG_CAPTURE_CHANNEL_NUM), )
ccflags-y += -DCHAN_NUM_CAPTURE=$(CFG_CAPTURE_CHANNEL_NUM)
endif

ifneq ($(CFG_CAPTURE_CHANNEL_MAP), )
ccflags-y += -DCFG_CAPTURE_CHANNEL_MAP=$(CFG_CAPTURE_CHANNEL_MAP)
endif

ifeq ($(DSP_CODEC_SUPPORT),1)
CFLAGS_speech_msg.o += -D__A7_DSP_CODEC__
endif

ccflags-y += \
             -Iservices/audioflinger \
             -Iservices/transq_msg \
             -Iservices/sys_time/ \
             -Iutils/hwtimer_list \
             -Iutils/ \
             -Iapps/key \
             -Inet \
             -Inet/utils \
             -Iapps/common \
             -Iapps/mic \
             -Iservices/bt_app \
             -Iservices/a7_dsp/common/codec
