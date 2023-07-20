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
 */

#ifndef _M33_GRAPHIC_CONF_H
#define _M33_GRAPHIC_CONF_H

/**
 *
 * LCDC/DMA2D Feature Configuration
 *
 */
// #define CONFIG_LCDC_AUTOREFRESH_MODE
// #define CONFIG_ULPS_MODE_SUPPORT
// #define CONFIG_LCDC_TE_IRQ_ENABLE
// #define CONFIG_LCDC_SLEEP
// #define CONFIG_DSI_SLEEP

// #ifdef CHIP_BEST2003
// #ifndef CONFIG_SWAP_DSI_CLK_PN
// #define CONFIG_SWAP_DSI_CLK_PN
// #endif
// #endif

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
//#define CONFIG_FB_SYNC
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
#if defined(CONFIG_DISPLAY_A064)
#define CONFIG_DSI_VIDEO_MODE 1
#define CONFIG_DSI_2LANE_MODE 1
#define BOARD_LCDC_WIDTH               480
#define BOARD_LCDC_HEIGHT              480
#elif defined(CONFIG_DISPLAY_ICNA3311)
#define BOARD_LCDC_WIDTH               280
#define BOARD_LCDC_HEIGHT              456
#elif defined(CONFIG_DISPLAY_ST7701)
#define CONFIG_DSI_VIDEO_MODE 1
#define CONFIG_DSI_2LANE_MODE 1
#define BOARD_LCDC_WIDTH               480
#define BOARD_LCDC_HEIGHT              640
#elif defined(CONFIG_DISPLAY_HX8394)
#define CONFIG_DSI_VIDEO_MODE 1
#define CONFIG_DSI_2LANE_MODE 1
#define BOARD_LCDC_WIDTH               480
#define BOARD_LCDC_HEIGHT              1280
#elif defined(CONFIG_DISPLAY_GC9503V)
#define CONFIG_DSI_VIDEO_MODE 1
#define CONFIG_DSI_2LANE_MODE 1
#define BOARD_LCDC_WIDTH               720
#define BOARD_LCDC_HEIGHT              1280
#else
#define BOARD_LCDC_WIDTH               454
#define BOARD_LCDC_HEIGHT              454
#endif


#ifdef CONFIG_DSI_VIDEO_MODE
#if defined(CONFIG_DISPLAY_A064)
#define BOARD_LCDC_HFP                  10
#define BOARD_LCDC_HBP                  10
#define BOARD_LCDC_VFP                  20
#define BOARD_LCDC_VBP                  20
#define BOARD_LCDC_HSYNC                8
#define BOARD_LCDC_VSYNC                8
#elif defined(CONFIG_DISPLAY_ICNA3311)
#define BOARD_LCDC_HFP                  12
#define BOARD_LCDC_HBP                  10
#define BOARD_LCDC_VFP                  10
#define BOARD_LCDC_VBP                  10
#define BOARD_LCDC_HSYNC                10
#define BOARD_LCDC_VSYNC                2
#elif defined(CONFIG_DISPLAY_ST7701)
#define BOARD_LCDC_HFP                  40
#define BOARD_LCDC_HBP                  60
#define BOARD_LCDC_VFP                  20
#define BOARD_LCDC_VBP                  20
#define BOARD_LCDC_HSYNC                4
#define BOARD_LCDC_VSYNC                2
#elif defined(CONFIG_DISPLAY_HX8394)
#define BOARD_LCDC_HFP                  26
#define BOARD_LCDC_HBP                  26
#define BOARD_LCDC_VFP                  16
#define BOARD_LCDC_VBP                  14
#define BOARD_LCDC_HSYNC                26
#define BOARD_LCDC_VSYNC                2
#elif defined(CONFIG_DISPLAY_GC9503V)
#define BOARD_LCDC_HFP                  80
#define BOARD_LCDC_HBP                  80
#define BOARD_LCDC_VFP                  20
#define BOARD_LCDC_VBP                  12
#define BOARD_LCDC_HSYNC                20
#define BOARD_LCDC_VSYNC                4
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

#define CONFIG_LCDC_FB_SIZE         (LCDC_HEIGHT*LCDC_WIDTH*4*2)

#define CONFIG_LCDC_FBWB_BPP  32
#if defined(CHIP_BEST2003)
#define CONFIG_LCDC_FB_BASE       0x3C000000
#elif defined(CHIP_BEST1502X)
#define CONFIG_LCDC_FB_BASE       0x20060000
#else
#define CONFIG_LCDC_FB_BASE       M55_SYS_RAM_BASE
#endif

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


#endif //_A7_DSP_GRAPHIC_CONF_H
