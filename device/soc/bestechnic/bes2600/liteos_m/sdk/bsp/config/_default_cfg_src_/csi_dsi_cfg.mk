ifeq ($(CSI_DSI_CONFIG),1)
##########################################
# TOP TEST FLAG
##########################################
export CSI_DSI_LOOP         ?= 0
export CSI_DSI_FT_LOOP_TEST ?= 0
export QRCODE_TEST          ?= 0

ifeq ($(CSI_DSI_LOOP),1)
export CSI_TEST     ?= 1
ifeq ($(NOAPP),1)
export VPLAYER_TEST ?= 1
else ifeq ($(PROGRAMMER),1)
export VPLAYER_TEST        ?= 0
export CAMERA_USE_SOFT_ISP ?= 0
export DSI_CLOCK_TEST      ?= 1
else
export DSI_TEST     ?= 1
endif # NOAPP
endif # CSI_DSI_LOOP

ifeq ($(A7_DSP_ENABLE),1)
export A7_DISPLAY   ?= 1
export DISPLAY_V2   ?= 0
endif # A7_DSP_ENABLE

ifeq ($(CSI_DSI_FT_LOOP_TEST),1)
export CSI_TEST     ?= 1
export DSI_TEST     ?= 0
export WORDROUND_FOR_DSI_CLK_PN_SWAP := 0
export CONFIG_SWAP_DSI_CLK_PN        := 0
endif # CSI_DSI_FT_LOOP_TEST

export DSI_TEST     ?= 0
export CSI_TEST     ?= 0
ifneq ($(VPLAYER_TEST)-$(DSI_TEST),0-0)
export LCD_CSI_DMA_MODE              ?= 1
export CONFIG_DSI_2LANE_MODE         ?= 1
export WORDROUND_FOR_DSI_CLK_PN_SWAP ?= 1
endif

$(info A7_DSP_ENABLE=$(A7_DSP_ENABLE))
$(info A7_DISPLAY=$(A7_DISPLAY))
$(info DISPLAY_V2=$(DISPLAY_V2))
$(info CSI_DSI_LOOP=$(CSI_DSI_LOOP))
$(info CSI_TEST=$(CSI_TEST))
$(info DSI_TEST=$(DSI_TEST))
$(info VPLAYER_TEST=$(VPLAYER_TEST))
$(info WORDROUND_FOR_DSI_CLK_PN_SWAP=$(WORDROUND_FOR_DSI_CLK_PN_SWAP))
$(info CONFIG_DSI_2LANE_MODE=$(CONFIG_DSI_2LANE_MODE))
$(info LCD_CSI_DMA_MODE=$(LCD_CSI_DMA_MODE))

##########################################
# EVB BOARD OPTION
##########################################
CAMERA_EVB_BOARD ?= FN_LINK_V200Z_R_V3P0
ifeq ($(CAMERA_EVB_BOARD),FN_LINK_V200Z_R_V3P0)
export CAMERA_DEVICE       ?= all
export DISPLAY_DEVICE      ?= A064
ifeq ($(DEBUG_PORT),2)
export UART1_IOMUX_INDEX   ?= 32
endif
export I2C0_IOMUX_INDEX    ?= 0
export CLKOUT_IOMUX_INDEX  ?= 34
else
$(error Invalid CAMERA_EVB_BORAD=$(CAMERA_EVB_BORAD) !)
endif # CAMERA_EVB_BOARD

##########################################
# CSI TEST OPTION
##########################################
ifeq ($(CSI_TEST),1)
ifeq ($(NOAPP),1)
export PSRAMUHS_ENABLE          ?= 1
else ifeq ($(PROGRAMMER),1)
export PSRAMUHS_ENABLE          ?= 1
endif
ifeq ($(CSI_SLT_TEST),1)
export CAMERA_TEST_MODE_INIT_ON ?= 1
export CAMERA_TEST_DATA_CHECK   ?= 1
endif
export CAMERA_IMAGE_DUMP        ?= 0
export CAMERA_TEST_MODE_INIT_ON ?= 0
export CSI_PHY_NO_CALIB         ?= 0

export GC2145_IMAGE_WIDTH       ?= 800
export GC2145_IMAGE_HEIGHT      ?= 600

export SC132GS_IMAGE_WIDTH      ?= 480
export SC132GS_IMAGE_HEIGHT     ?= 640
endif # CSI_TEST

##########################################
# DSI TEST OPTION
##########################################
ifeq ($(DSI_TEST),1)
ifeq ($(DISPLAY_DEVICE),A064)
DISPLAY_IMAGE_WIDTH  ?= 480
DISPLAY_IMAGE_HEIGHT ?= 480
else
$(error Invalid DISPLAY_DEVICE=$(DISPLAY_DEVICE) !)
endif
endif # DSI_TEST

endif # CSI_DSI_CONFIG
