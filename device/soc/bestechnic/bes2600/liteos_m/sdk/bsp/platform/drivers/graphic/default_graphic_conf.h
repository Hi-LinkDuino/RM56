
/***************************************************************************
 *
 * Copyright 2015-2021 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

/**
 * DOC: The LCDC Board Related Configuration.
 * NOTE: For 1600 FPGA TEST ONLY
 */

#ifndef _DEFAULT_GRAPHIC_CONF_H
#define _DEFAULT_GRAPHIC_CONF_H

#define DISPLAY_RM69330     1

//#define DISPLAY_GC9503NP    1
//#define DISPLAY_ST7701SN    1
//#define DISPLAY_ST7785M     1

//#define CONFIG_DSI_VIDEO_MODE

/**
 *
 * LCDC/DMA2D Feature Configuration
 *
 */
#ifndef ohos_build
#define CONFIG_ULPS_MODE_SUPPORT
#define CONFIG_LCDC_SLEEP
#define CONFIG_DSI_SLEEP
#endif

#define CONFIG_DSI_GPIO_TE_IRQ
#define CONFIG_LCDC_TE_IRQ_ENABLE

#ifdef CHIP_BEST2003
#undef CONFIG_DSI_SLEEP
#endif

//#define CONFIG_TRIGGER_FRAME_BY_SW
#ifndef USE_GAMU_FB
#define CONFIG_FB_DOUBLE_BUFFER
#endif

#ifdef CONFIG_FB_PANDISPLAY
#define CONFIG_LCDC_AUTOREFRESH_MODE
#ifndef USE_GAMU_FB
#define CONFIG_FB_DOUBLE_BUFFER
#endif
#define CONFIG_TRIGGER_FRAME_BY_SW
#define CONFIG_FB_DOUBLE_BUFFER
#endif

#define CONFIG_FB_OVERLAY_BLIT
#define CONFIG_FB_OVERLAY

// #define CONFIG_DMA2D_FB_CMAP
// #define CONFIG_LCDC_DITHER
// #define CONFIG_DMA2D_FB_COLORDITHER
// #define CONFIG_FB_CMAP
// #define CONFIG_FB_COLORDITHER
// #define CONFIG_FB_COLORADJCURVE
// #define CONFIG_FG_YUV_FMT

#define CONFIG_FB_UPDATE
//#define CONFIG_FB_PANDISPLAY
// #define SIMU_TEST_CODE
// #define ONFIG_LCDC_L1_CHROMAKEYEN
// #define CONFIG_LCDC_L2_CHROMAKEYEN
// #define CONFIG_LCDC_CHROMAKEY=0x0000fe
// #define CONFIG_PM
// #define CONFIG_FB_VMIRRO

//#define CONFIG_DMA2D /*used by lcdc driver*/

/**
 * Board  Configuration
 */

/* lcdc write back test switch, should removed at release */
//#define  WB_TEST

#ifdef TEST_WITH_GPU //if use gpu,Width should be 16-pixel aligned
#ifdef USE_GAMU_FB
#define BOARD_LCDC_WIDTH               512  //gpu width
#define BOARD_LCDC_HEIGHT              512
#else
#define BOARD_LCDC_WIDTH               464  //gpu width
#define BOARD_LCDC_HEIGHT              454
#endif
#define SCREEN_WIDTH                   454 //screen
#elif defined(PROGRAMMER)
#define BOARD_LCDC_WIDTH               160
#define BOARD_LCDC_HEIGHT              32
#else
#define BOARD_LCDC_WIDTH               454 //360
#define BOARD_LCDC_HEIGHT              454 //360
#endif


#ifdef CONFIG_DSI_VIDEO_MODE
#undef CONFIG_TRIGGER_FRAME_BY_SW
#undef CONFIG_ULPS_MODE_SUPPORT
#undef CONFIG_LCDC_SLEEP
#undef CONFIG_DSI_SLEEP
#undef BOARD_LCDC_WIDTH
#undef BOARD_LCDC_HEIGHT
#define CONFIG_DSI_2LANE_MODE 1
#if DISPLAY_GC9503NP
#define BOARD_LCDC_WIDTH               480 //360
#define BOARD_LCDC_HEIGHT              800 //360
#define BOARD_LCDC_HFP                  80
#define BOARD_LCDC_HBP                  80
#define BOARD_LCDC_VFP                  24
#define BOARD_LCDC_VBP                  24
#define BOARD_LCDC_HSYNC                1
#define BOARD_LCDC_VSYNC                1
#elif DISPLAY_ST7701SN
#define BOARD_LCDC_WIDTH               480
#define BOARD_LCDC_HEIGHT              640
#define BOARD_LCDC_HFP                  60
#define BOARD_LCDC_HBP                  60
#define BOARD_LCDC_VFP                  13
#define BOARD_LCDC_VBP                  13
#define BOARD_LCDC_HSYNC                10
#define BOARD_LCDC_VSYNC                10
#elif DISPLAY_ST7785M
#undef CONFIG_DSI_2LANE_MODE
#define BOARD_LCDC_WIDTH               240
#define BOARD_LCDC_HEIGHT              320
#define BOARD_LCDC_HFP                  50
#define BOARD_LCDC_HBP                  66
#define BOARD_LCDC_VFP                  200 // 30fps
#define BOARD_LCDC_VBP                  200 // 30fps
#define BOARD_LCDC_HSYNC                10
#define BOARD_LCDC_VSYNC                200 // 30fps
#endif
#else
#define BOARD_LCDC_HFP                  12
#define BOARD_LCDC_HBP                  10
#define BOARD_LCDC_VFP                  10
#define BOARD_LCDC_VBP                  10
#define BOARD_LCDC_HSYNC                10
#define BOARD_LCDC_VSYNC                2
#endif

#define BOARD_LCDC_GCR_DEN              1
#define BOARD_LCDC_GCR_DBW              5
#define BOARD_LCDC_GCR_DGW              6
#define BOARD_LCDC_GCR_DRW              5

#if defined(CONFIG_FB_DOUBLE_BUFFER)
#define CONFIG_LCDC_FB_SIZE         (BOARD_LCDC_HEIGHT*BOARD_LCDC_WIDTH*4*2)
#else
#define CONFIG_LCDC_FB_SIZE         (BOARD_LCDC_HEIGHT*BOARD_LCDC_WIDTH*4)
#endif

#define CONFIG_LCDC_FBWB_BPP  32

extern uint32_t get_fb_base(void);
#define CONFIG_LCDC_FB_BASE       get_fb_base()

#define CONFIG_LCDC_FBWB_BASE     (CONFIG_LCDC_FB_BASE +  \
                                  CONFIG_LCDC_FBWB_BPP/4 *BOARD_LCDC_WIDTH*BOARD_LCDC_HEIGHT)

#define CONFIG_DMA2D_NLAYERS  1

#define CONFIG_DMA2D_FB_BASE   CONFIG_LCDC_FBWB_BASE

#define CONFIG_LCDC_L1
//#define CONFIG_LCDC_L2
//#define CONFIG_LCDC_LWB

#define CONFIG_LCDC_L1_ARGB8888
#define CONFIG_LCDC_L2_ARGB8888
//#define CONFIG_LCDC_L1_YUV422

//#define CONFIG_LCDC_L1_RGB565
//#define CONFIG_LCDC_L2_RGB565
//#define CONFIG_LCDC_L2_RGB888

//#define CONFIG_LCDC_LO_RGB565
#define CONFIG_LCDC_LO_RGB888

#define CONFIG_DMA2D_ARGB8888
//#define CONFIG_DMA2D_RGB888
//#define CONFIG_DMA2D_RGB565


#endif //_DEFAULT_GRAPHIC_CONF_H
