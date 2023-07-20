cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

ccflags-y += -Itests/hwtest \
	-Itests/hwtest/csi_test \
	-Itests/hwtest/csi_test/video_player \
	-Iplatform/hal \
	-Iplatform/drivers/ana \
	-Iservices/a7_dsp/common \
	-Iutils/hwtimer_list \
	-Iutils/heap \
	-Iutils/cqueue

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)

obj-y += \
	video_player/video_misc.o \
	video_player/video_stream.o \
	video_player/video_stream_rpc.o \
	video_player/video_record.o \
	video_player/camera_drv.o \
	video_player/camera_dump.o \
	video_player/camera_sens_gc2145.o \
	video_player/camera_sens_sc132gs.o \
	video_player/display_drv_a064.o \
	video_player/display_drv.o \


ifeq ($(NOAPP), 1)
obj-y += ../../../utils/cqueue/
obj-y += ../../../utils/heap/
endif

ifeq ($(CHIP_ROLE_DSP),1)
obj-y += ../../../utils/heap/
endif

VIDEO_PLAYER ?= 1
ifeq ($(VIDEO_PLAYER),1)
obj-y += video_player/video_player.o
ccflags-y += -DVIDEO_PLAYER
endif

ifeq ($(RTOS),1)
ccflags-y += -DRTOS
endif

ifeq ($(NOAPP),1)
ccflags-y += -DNOAPP
endif

ifeq ($(SIMU),1)
ccflags-y += -DSIMU
endif

ifeq ($(PSRAMUHS_ENABLE),1)
ccflags-y += -DPSRAMUHS_ENABLE
endif

ifeq ($(CHIP_ROLE_DSP),1)
ccflags-y += -DCHIP_ROLE_DSP
MEDIA_HEAP_ENABLE         ?= 1
MEDIA_HEAP_USE_STATIC_BUF ?= 0
DSP_VPLAYER_APP_USE_HEAP  ?= 1
endif

ifeq ($(CHIP_ROLE_AP),1)
ccflags-y += -DCHIP_ROLE_AP
MEDIA_HEAP_ENABLE         ?= 1
MEDIA_HEAP_USE_STATIC_BUF ?= 0
AP_VPLAYER_APP_USE_HEAP   ?= 1
endif

################ DISPLAY DEVICE OPTION ################
DISPLAY_DEVICE       ?= A064

ifeq ($(DISPLAY_DEVICE),A064)
DISPLAY_IMAGE_WIDTH  ?= 480
DISPLAY_IMAGE_HEIGHT ?= 480
DSI_SENS_A064        ?= 1
DSI_FPS_STAT_ENABLE  ?= 1
else
$(error Invalid DISPLAY_DEVICE=$(DISPLAY_DEVICE))
endif

ifneq ($(DISPLAY_IMAGE_WIDTH),)
ccflags-y += -DDISPLAY_IMAGE_WIDTH=$(DISPLAY_IMAGE_WIDTH)
endif

ifneq ($(DISPLAY_IMAGE_HEIGHT),)
ccflags-y += -DDISPLAY_IMAGE_HEIGHT=$(DISPLAY_IMAGE_HEIGHT)
endif

ifeq ($(DSI_SENS_A064),1)
ccflags-y += -DDSI_SENS_A064
endif

ifeq ($(DSI_FPS_STAT_ENABLE),1)
ccflags-y += -DDSI_FPS_STAT_ENABLE
endif

ifeq ($(DSI_TEST),1)
ccflags-y += -DDSI_TEST
endif

ifeq ($(DSI_IRQ_DEBUG),1)
ccflags-y += -DDSI_IRQ_DEBUG
endif

ifeq ($(DSI_FB_DEBUG),1)
ccflags-y += -DDSI_FB_DEBUG
endif

################ CAMERA DEVICE OPTION ################
#CAMERA_DEVICE ?= GC2145
CAMERA_DEVICE ?= SC132GS

ifeq ($(CAMERA_DEVICE),GC2145)
CAMERA_IMAGE_WIDTH  ?= 800
CAMERA_IMAGE_HEIGHT ?= 600
CSI_SENS_GC2145     ?= 1
CSI_SENS_AEC_ENABLE ?= 1
CSI_USE_GPIO_TRIG   ?= 1
CSI_FPS_STAT_SLOW   ?= 1
CSI_SENS_CALIB      ?= 1
ifneq ($(CAMERA_IMAGE_DUMP),1)
CSI_FPS_STAT_ENABLE ?= 1
endif
GC2145MIPI_2LANE    ?= 1
else ifeq ($(CAMERA_DEVICE),SC132GS)
CAMERA_IMAGE_WIDTH  ?= 480
CAMERA_IMAGE_HEIGHT ?= 640
ifneq ($(CAMERA_IMAGE_WIDTH),1080)
CAMERA_DATA_RAW10_TO_YUV ?=1
endif
SC132GS_300MHZ      ?= 0
ifeq ($(SC132GS_300MHZ),0)
SC132GS_150MHZ      ?= 1
endif
CSI_SENS_CALIB      ?= 0
CSI_SENS_SC132GS    ?= 1
CSI_USE_GPIO_TRIG   ?= 1
CSI_FPS_STAT_SLOW   ?= 1
ifneq ($(CAMERA_IMAGE_DUMP),1)
CSI_FPS_STAT_ENABLE ?= 1
endif
GC2145MIPI_2LANE    ?= 1
else
$(error Invalid CAMERA_DEVICE=$(CAMERA_DEVICE))
endif

ifneq ($(CAMERA_IMAGE_WIDTH),)
ccflags-y += -DCAMERA_IMAGE_WIDTH=$(CAMERA_IMAGE_WIDTH)
endif

ifneq ($(CAMERA_IMAGE_HEIGHT),)
ccflags-y += -DCAMERA_IMAGE_HEIGHT=$(CAMERA_IMAGE_HEIGHT)
endif

ifeq ($(CAMERA_IMAGE_DUMP),1)
ccflags-y += -DCAMERA_IMAGE_DUMP
CAMERA_IMAGE_DUMP_NUM ?= 3
ccflags-y += -DCAMERA_IMAGE_DUMP_NUM=$(CAMERA_IMAGE_DUMP_NUM)
endif

ifeq ($(CAMERA_DATA_DUMP),1)
ccflags-y += -DCAMERA_DATA_DUMP
CAMERA_DATA_DUMP_NUM ?= 512
ccflags-y += -DCAMERA_DATA_DUMP_NUM=$(CAMERA_DATA_DUMP_NUM)
endif

ifeq ($(CAMERA_DATA_RAW10_TO_YUV),1)
ccflags-y += -DCAMERA_DATA_RAW10_TO_YUV
endif

ifeq ($(CAMERA_USE_SOFT_ISP),1)
ccflags-y += -DCAMERA_USE_SOFT_ISP
endif

ifeq ($(CAMERA_DUMMY_DEVICE),1)
ccflags-y += -DCAMERA_DUMMY_DEVICE
endif

ifeq ($(CAMERA_TEST_MODE_INIT_ON),1)
ccflags-y += -DCAMERA_TEST_MODE_INIT_ON
endif

ifeq ($(CAMERA_TEST_MODE_ENABLE),1)
ccflags-y += -DCAMERA_TEST_MODE_ENABLE
endif

ifeq ($(CAMERA_TEST_DATA_CHECK_ENABLE),1)
ccflags-y += -DCAMERA_TEST_DATA_CHECK_ENABLE
endif

ifeq ($(CSI_SENS_CALIB),1)
ccflags-y += -DCSI_SENS_CALIB
endif

ifeq ($(CSI_SENS_GC2145),1)
ccflags-y += -DCSI_SENS_GC2145
endif

ifeq ($(CSI_SENS_SC132GS),1)
ccflags-y += -DCSI_SENS_SC132GS
endif

ifeq ($(CSI_FPS_STAT_ENABLE),1)
ccflags-y += -DCSI_FPS_STAT_ENABLE
endif

ifeq ($(CSI_FPS_STAT_SLOW),1)
ccflags-y += -DCSI_FPS_STAT_SLOW
endif

ifeq ($(CSI_SENS_AEC_ENABLE),1)
ccflags-y += -DCSI_SENS_AEC_ENABLE
endif

ifeq ($(CSI_USE_GPIO_TRIG),1)
ccflags-y += -DCSI_USE_GPIO_TRIG
endif

ifeq ($(CSI_PHY_NO_CALIB),1)
ccflags-y += -DCSI_PHY_NO_CALIB
endif

ifeq ($(SC132GS_150MHZ),1)
ccflags-y += -DSC132GS_150MHZ
endif

ifeq ($(SC132GS_300MHZ),1)
ccflags-y += -DSC132GS_300MHZ
endif

ifeq ($(GC2145MIPI_2LANE),1)
ccflags-y += -DGC2145MIPI_2LANE
endif

ifeq ($(CSI_TEST),1)
ccflags-y += -DCSI_TEST
endif

ifeq ($(CSI_DSI_LOOP),1)
ccflags-y += -DCSI_DSI_LOOP
endif

ifeq ($(CSI_DSI_FT_LOOP_TEST),1)
ccflags-y += -DCSI_DSI_FT_LOOP_TEST
endif

ifeq ($(CSI_DEBUG),1)
ccflags-y += -DCSI_DEBUG
endif

ifeq ($(CSI_IRQ_DEBUG),1)
ccflags-y += -DCSI_IRQ_DEBUG
endif

ifeq ($(CSI_FB_DEBUG),1)
ccflags-y += -DCSI_FB_DEBUG
endif

ifeq ($(CSI_SENS_I2C_DEBUG),1)
ccflags-y += -DCSI_SENS_I2C_DEBUG
endif

CSI_NO_TRACE ?= 1
ifeq ($(CSI_NO_TRACE),1)
ccflags-y += -DCSI_NO_TRACE
endif

ifeq ($(CSI_SENS_TEST_PATT),1)
ccflags-y += -DCSI_SENS_TEST_PATT
endif

ifneq ($(CSI_DATA_DUMP_SIZE),)
ccflags-y += -DCSI_DATA_DUMP_SIZE=$(CSI_DATA_DUMP_SIZE)
endif

################ APPLICATION OPTION ################
VPLAYER_APP_DEBUG         ?= 0

ifeq ($(VIDEO_RECORD_ENABLE),1)
ccflags-y += -DVIDEO_RECORD_ENABLE
endif

ifeq ($(VPLAYER_TEST),1)
ccflags-y += -DVPLAYER_TEST
endif

ifneq ($(VPLAYER_APP_DEBUG),)
ccflags-y += -DVPLAYER_APP_DEBUG=$(VPLAYER_APP_DEBUG)
ccflags-y += -DDSP_VPLAYER_APP_DEBUG=$(VPLAYER_APP_DEBUG)
ccflags-y += -DAP_VPLAYER_APP_DEBUG=$(VPLAYER_APP_DEBUG)
endif

ifeq ($(DSP_VPLAYER_APP_USE_HEAP),1)
ccflags-y += -DDSP_VPLAYER_APP_USE_HEAP
endif

ifeq ($(AP_VPLAYER_APP_USE_HEAP),1)
ccflags-y += -DAP_VPLAYER_APP_USE_HEAP
endif

ifeq ($(MEDIA_HEAP_ENABLE),1)
ccflags-y += -DMEDIA_HEAP_ENABLE
endif

ifeq ($(MEDIA_HEAP_USE_STATIC_BUF),1)
ccflags-y += -DMEDIA_HEAP_USE_STATIC_BUF
endif

