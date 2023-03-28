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
#include "plat_addr_map.h"
#if defined(DSI_BASE) && defined(LCDC_BASE)
#include "plat_types.h"
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_dsi.h"
#include "reg_dsi.h"
#include "reg_lcdc_v1.h"
#include "reg_lcdc.h"
#include "mipiphy.h"
#include "cmsis_nvic.h"
#include CHIP_SPECIFIC_HDR(pmu)

static bool dsi_init_v2 = false;
static enum DSI_MODE_T dsi_mode = DSI_MODE_CMD;
static HAL_DSI_XFER_COMPLETE_CALLBACK_T dsi_callback;

static struct LCDC_REG_T * const lcdc = (struct LCDC_REG_T *)LCDC_BASE;

static struct DSI_REG_T * const dsi = (struct DSI_REG_T *)DSI_BASE;

static uint8_t  lane_num = 0;

enum DSI_CMD_TYPE_T {
    CLN_ENT_LP = 0,
    CLN_EXIT_LP = 1,
    CLN_ENT_ULPS = 2,
    CLN_EXIT_ULPS = 3,
    DLN_LPT = 4,
    DLN_HST = 5,
    DLN_LPT_BTA = 6,
    DLN_HST_BTA = 7,
};

enum DSI_CMD_T {
    DSC_SHORT_WRITE = 0x05,
    DSC_SHORT_WRITE_1PAR = 0x15,
    DSC_LONG_WRITE = 0x39
};

#define mode_rgb565          0x0
#define mode_rgb1555         0x1
#define mode_rgb888packed    0x2
#define mode_rgb888unpacked  0x3
#define mode_rgbA888         0x4
#define mode_yuv422packed    0x5
#define mode_yuv422planar    0x6
#define mode_yuv420planar    0x7
#define mode_smpncmd         0x8
#define mode_palette4bit     0x9
#define mode_palette8bit     0xa
#define mode_rgb888A         0xb
#define mode_alpha_dma       0x0
#define mode_alpha_gra       0x1
#define mode_alpha_cfg       0x2
#define mode_alpha_non       0x3
#define mode_low16of24       0x0
#define mode_high16of24      0x1
#define mode_low18of24       0x2
#define mode_high18of24      0x3
#define mode_low12of24       0x4
#define mode_high12of24      0x5
#define mode_allof24         0x6
#define mode_smpn3cyc888     0x0
#define mode_smpn3cyc666     0x1
#define mode_smpn2cyc565     0x2
#define mode_smpn1cyc888     0x3
#define mode_smpn1cyc666     0x4
#define mode_smpn1cyc565     0x5
#define smpn8dual_swap       0xf
#define axim_axiconfig	     0x20
#define mode_dumb24          0x0 | axim_axiconfig
#define mode_dumb18spi       0x1 | axim_axiconfig
#define mode_dumb18gpio      0x2 | axim_axiconfig
#define mode_dumb16spi       0x3 | axim_axiconfig
#define mode_dumb16gpio      0x4 | axim_axiconfig
#define mode_dumb12all       0x5 | axim_axiconfig
#define mode_smpn18spi       0x6 | axim_axiconfig
#define mode_smpn16spi       0x7 | axim_axiconfig
#define mode_smpn8all        0x8 | axim_axiconfig
#define mode_dumb18smpn8     0x9 | axim_axiconfig
#define mode_dumb16smpn8spi  0xa | axim_axiconfig
#define mode_dumb16smpn8gpio 0xb | axim_axiconfig
#define mode_pn16tv16        0xc | axim_axiconfig
#define mode_hwc_pass        0x0
#define mode_hwc_color1      0x1
#define mode_hwc_color2      0x2
#define mode_hwc_inv         0x3
#define SRAMID_gamma_yr      0x0
#define SRAMID_gamma_ug      0x1
#define SRAMID_gamma_vb      0x2
#define SRAMID_palette       0x3
#define SRAMID_hwc           0xc
#define SRTVID_gamma_yr      0x4
#define SRTVID_gamma_ug      0x5
#define SRTVID_gamma_vb      0x6
#define SRTVID_palette       0x7
#define SRTVID_tvc           0xb

void hal_display_irq_handler(uint32_t irq_stats)
{
    if (irq_stats & (LCD_TXC_FRAMEDONE | LCD_DUMB_FRAMEDONE)) {
        if ((irq_stats & (LCD_DMA_FRAME_IRQ0|LCD_DMA_FRAME_IRQ1)) ==
            (LCD_DMA_FRAME_IRQ0|LCD_DMA_FRAME_IRQ1)) {
            TRACE(0, "Warning: lcdc lost frame");
        }
        if ((irq_stats & (LCD_GRA_FRAME_IRQ0|LCD_GRA_FRAME_IRQ1)) ==
            (LCD_GRA_FRAME_IRQ0|LCD_GRA_FRAME_IRQ1)) {
            TRACE(0, "Warning: lcdc lost frame");
        }
        if ((irq_stats & (LCD_TVG_FRAME_IRQ0|LCD_TVG_FRAME_IRQ1)) ==
            (LCD_TVG_FRAME_IRQ0|LCD_TVG_FRAME_IRQ1)) {
            TRACE(0, "Warning: lcdc lost frame");
        }
        if (irq_stats & LCD_DMA_FRAME_IRQ0) {
            if (dsi_callback)
                dsi_callback(0, 0, lcdc->REG_0C0);
            else
                hal_lcdc_start();
        }
        if (irq_stats & LCD_DMA_FRAME_IRQ1) {
            if (dsi_callback)
                dsi_callback(0, 1, lcdc->REG_0D0);
            else
                hal_lcdc_start();
        }

        if (irq_stats & LCD_GRA_FRAME_IRQ0) {
            if (dsi_callback)
                dsi_callback(1, 0, lcdc->REG_0F4);
            else
                hal_lcdc_start();
        }
        if (irq_stats & LCD_GRA_FRAME_IRQ1) {
            if (dsi_callback)
                dsi_callback(1, 1, lcdc->REG_0F8);
            else
                hal_lcdc_start();
        }

        if (irq_stats & LCD_TVG_FRAME_IRQ0) {
            uint32_t addr = lcdc->REG_034 + 368*4*2;
            if (addr >= 0x50000000+368*448*4)
                addr = 0x50000000;
            lcdc->REG_034 = addr;
            hal_lcdc_start();
        }
        if (irq_stats & LCD_TVG_FRAME_IRQ1) {
            uint32_t addr = lcdc->REG_038 + 368*4*2;
            if (addr >= 0x50000000+368*448*4)
                addr = 0x50000000;
            lcdc->REG_038 = addr;
            hal_lcdc_start();
        }
    }
}

void hal_dsi0_irq_handler(uint32_t irq_stats)
{
}

//TE irq
void hal_dsi1_irq_handler(uint32_t irq_stats)
{
}

void hal_dsi_unified_irq_handler()
{
    uint32_t irq_stats;
#if 0
    void dsiphy_clock_restore(void);
    static uint64_t irq_count = 0;
    if (irq_count++ == 2)
        dsiphy_clock_restore();
#endif
    //TRACE(0, "irq:%u ms", FAST_TICKS_TO_MS(hal_fast_sys_timer_get()));
    irq_stats = lcdc->REG_1C4;
    //TRACE(0, "lcdc->REG_1C4:0x%x", irq_stats);
    if (irq_stats & 0xffffff00) {
        lcdc->REG_1C4 = 0; //clear irq
        hal_display_irq_handler(irq_stats);
    }

#if 0 //doesn't handler dsi irq
    irq_stats = dsi->REG_010;
    //TRACE(0, "dsi->REG_010:0x%x", irq_stats);
    dsi->REG_010 = irq_stats;//clear irq
    if (irq_stats & DSI_TE_INTR)
        hal_dsi1_irq_handler(irq_stats);
    if (irq_stats & (DSI_HS_CMD_RD_EN | DSI_LPRX_RD_EN | DSI_CMD_DONE_FLAG | DSI_BTA_DONE_FLAG |
        DSI_BTA_FAIL_FLAG | DSI_LP_RX_DONE_FLAG | DSI_RX_ERR_FLAG | DSI_LPCD_FLAG | DSI_RX_TIMEOUT_FLAG))
        hal_dsi0_irq_handler(irq_stats);
#endif
}

/**
 * @brief hal_dsi_read_cmd - read data by lp mode
 * packet format:
 * ShortPacket: DI DATA0 DATA1 ECC
 * LongPacket:  DI WC0   WC1   ECC DATA[0]-DATA[WC-1] CS0 CS1
 * e.g. read id: hal_dsi_read_cmd(0x04, buf, 3)
 * @param cmd : cmd
 * @param read_buf : read buffer
 * @param len : read len
 * @return int
 */
int hal_dsi_read_cmd(uint8_t cmd, uint8_t *recv_buf, uint8_t len)
{
    volatile uint32_t temp;
    uint8_t fifo_len;
    uint8_t buf[32];
    uint8_t buf_len = 0;
    uint32_t time;

    dsi->REG_010 |= DSI_LP_RX_DONE_FLAG;

    dsi->REG_018 = (len << 8) | 0x37;
    dsi->REG_014 =  (5 << 16) | (0xe1 << 8) | DLN_LPT;
    hal_sys_timer_delay_us(20);
    dsi->REG_018 = (cmd << 8)| 0x06;
    dsi->REG_014 = (5 << 16) | (0xe1 << 8) | DLN_LPT_BTA;
    hal_sys_timer_delay_us(10);

    time = hal_sys_timer_get();
    while (1) {
        temp = dsi->REG_010;
        if (temp & DSI_LPRX_SIZE_MASK) {
            buf[buf_len ++] = temp;
        } else if (temp & (DSI_RX_ERR_FLAG | DSI_RX_TIMEOUT_FLAG)) {
            dsi->REG_010 |= (DSI_RX_ERR_FLAG | DSI_RX_TIMEOUT_FLAG);
            TRACE(0, "rx error:0x%x", temp);
            return -1;
        } else if ((temp & DSI_LP_RX_DONE_FLAG) && (temp & DSI_LPRX_SIZE_MASK) == 0) {
            //TRACE(0, "rx done:0x%x", temp);
            break;
        } else if (hal_sys_timer_get() - time > MS_TO_TICKS(1000)) {
            TRACE(0, "rx timerout:0x%x", temp);
            return -2;
        }
        hal_sys_timer_delay_us(1);
    }

    fifo_len = GET_BITFIELD(temp, DSI_LPRX_SIZE);
    if (fifo_len > 1) {
        TRACE(0, "warning: fifo is not empty");
        while (fifo_len > 1) {
            temp = dsi->REG_010;
            fifo_len --;
        }
    }

    if (buf_len >= 2 && buf[1] == 0x02) {
        TRACE(0, "Acknowledge error report");
        return -1;
    } else if (len == 1 && buf_len >= 3 && buf[1] == 0x21) { //short packet
        recv_buf[0] = buf[2];
        return 1;
    } else if (len == 2 && buf_len >= 4 && buf[1] == 0x22) { //short packet
        recv_buf[0] = buf[2];
        recv_buf[1] = buf[3];
        return 2;
    } else if (buf_len >= 10 && buf[1] == 0x1c) { //long packet
        uint16_t word_cnt = (buf[3] >> 8) | buf[2];
        uint16_t i;
        if (word_cnt < len) {
            TRACE(0, "warning: word_cnt:%d < len:%d", word_cnt, len);
            len = word_cnt;
        }
        for (i = 0; i < len; ++i)
            recv_buf[i] = buf[5 + i];
        return len;
    } else {
        TRACE(0, "error, buf_len:%d, len:%d", buf_len, len);
        DUMP8("%02x ", buf, buf_len);
        return -3;
    }

    return 0;
}


/**
 * @brief hal_dsi_send_cmd - send cmd by lp mode
 *
 * @param cmd
 */
void hal_dsi_send_cmd(uint8_t cmd)
{
    dsi->REG_018 = (cmd << 8) | DSC_SHORT_WRITE;
    dsi->REG_014 = (5 << 16) | (0xe1 << 8) | DLN_LPT;
#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(1));
#endif
}

/**
 * @brief hal_dsi_send_cmd_data - send cmd with data by lp mode
 *
 * @param cmd : cmd
 * @param len : send param num
 * @param p0 : param0
 * @param p1 : param1
 * @param p2 : param2
 * @param p3 : param3
 */
void hal_dsi_send_cmd_data(uint8_t cmd, uint32_t len, uint8_t p0,
                           uint8_t p1, uint8_t p2, uint8_t p3)
{
    if (len == 1) {
        dsi->REG_018 = (p0 << 16) | (cmd << 8) | DSC_SHORT_WRITE_1PAR;
        dsi->REG_014 = (5 << 16) | (0xe1 << 8) | DLN_LPT;
    } else if (len == 2) {
        dsi->REG_018 = ((len+1) << 8) | DSC_LONG_WRITE;
        dsi->REG_01C = cmd |
                       (p0 << 8) |
                       (p1 << 16);
        dsi->REG_014 = ((len+1+4+3) << 16) | (0xe1 << 8) | DLN_LPT;
    } else if (len == 3) {
        dsi->REG_018 = ((len+1) << 8) | DSC_LONG_WRITE;
        dsi->REG_01C = cmd |
                       (p0 << 8) |
                       (p1 << 16) |
                       (p2 << 24);
        dsi->REG_014 = ((len+1+4+3) << 16) | (0xe1 << 8) | DLN_LPT;
    } else if (len == 4) {
        dsi->REG_018 = ((len+1) << 8) | DSC_LONG_WRITE;
        dsi->REG_01C = cmd |
                       (p0 << 8) |
                       (p1 << 16) |
                       (p2 << 24);
        dsi->REG_020 = p3;
        dsi->REG_014 = ((len+1+4+3) << 16) | (0xe1 << 8) | DLN_LPT;
    }
#ifndef SIMU
    hal_sys_timer_delay(US_TO_TICKS(len * 20 + 10));
#endif
}

/**
 * @brief hal_dsi_send_long_array - send cmd with data with array
 *
 * @param len : array len
 * @param data : array data
 */
void hal_dsi_send_long_array(uint32_t len, uint32_t *data)
{
    int i;
    dsi->REG_018 = (len << 8) | DSC_LONG_WRITE;
    for(i=0;i<len/4;i++)
        dsi->REG_01C = data[i];
    int remain = len%4;
    if (remain == 1)
        dsi->REG_020 = data[i];
    else if (remain == 2)
        dsi->REG_024 = data[i];
    else if (remain == 3)
        dsi->REG_028 = data[i];
    dsi->REG_014 = ((len+4+3) << 16) | (0xe1 << 8) | DLN_LPT;
#ifndef SIMU
    hal_sys_timer_delay(US_TO_TICKS(len * 20 + 10));
#endif
}

/**
 * @brief hal_dsi_send_cmd_list - send cmd with data list
 *
 * @param cmd
 * @param para_count
 * @param para_list
 */
void hal_dsi_send_cmd_list(unsigned cmd, unsigned char para_count,
                           unsigned char *para_list)
{
    unsigned int item[16]={0};
    unsigned char dsi_cmd = (unsigned char)cmd;
    unsigned char dc;
    int index = 0;
    if (para_count + 1 > 60)
            return;

    if (para_count == 0) {
            hal_dsi_send_cmd(dsi_cmd);
    } else if (para_count == 1) {
            hal_dsi_send_cmd_data(dsi_cmd, 1, para_list[0], 0x0, 0x0, 0x0);
    } else if (para_count == 2) {
            hal_dsi_send_cmd_data(dsi_cmd, 2, para_list[0], para_list[1], 0x0, 0x0);
    } else if (para_count == 3) {
            hal_dsi_send_cmd_data(dsi_cmd, 3, para_list[0], para_list[1], para_list[2], 0x0);
    } else if (para_count == 4) {
            hal_dsi_send_cmd_data(dsi_cmd, 4, para_list[0], para_list[1], para_list[2], para_list[3]);
    } else {
        while (1) {
            if (index == para_count + 1){
                break;
            }
            if (index == 0)
                dc = cmd;
            else
                dc = para_list[index - 1];
            item[index / 4 ] |= (dc << (8 * (index % 4)));
            ++index;
        }
        hal_dsi_send_long_array(index,item);
    }
}

void hal_dsi_init(uint16_t h_res ,uint8_t lane_number)
{
    if (!dsi_init_v2) {
        hal_iomux_set_dsi_te();
        hal_cmu_dsi_clock_enable();
    }
    hal_cmu_dsi_reset_set();
    hal_sys_timer_delay_us(10);
    hal_cmu_dsi_reset_clear();
    hal_sys_timer_delay_us(10);

#ifndef FPGA
    dsiphy_open(0);
#endif
    if (lane_number <= 2) {
        lane_num = lane_number;
    }
    //DSI init-1
    if (dsi_mode == DSI_MODE_VIDEO) {
        dsi->REG_000 = DSI_R_LANE_NUM(lane_num - 1) | DSI_R_LPCD_DLY(1) | DSI_R_VIDEO_MODE|
            DSI_R_EOTP_EN |DSI_R_HSA_LP | DSI_R_HSE_EN | DSI_R_HFP_LP |
            DSI_R_T_LPX(0x6) | DSI_R_CLK_PRE(0x3) | DSI_R_CLK_POST(0x7);
    } else {
        dsi->REG_000 = DSI_R_LANE_NUM(lane_num - 1) | DSI_R_LPCD_DLY(1) |
            DSI_R_HSA_LP | DSI_R_HSE_EN | DSI_R_HFP_LP | DSI_R_EOTP_EN |
            DSI_R_T_LPX(0x6) | DSI_R_CLK_PRE(0x3) | DSI_R_CLK_POST(0x7);
    }
    dsi->REG_004 = DSI_R_HS_EXIT_TIME(0xb) | DSI_R_HS_PRPR_TIME(0xc) |
        DSI_R_HS_ZERO_TIME(0x2) | DSI_R_HS_TRAIL_TIME(0xa) |
        DSI_R_T_WAKEUP(0x27);
    dsi->REG_008 = DSI_R_CLK_EXIT_TIME(1) | DSI_R_CLK_PRPR_TIME(0) |
        DSI_R_CLK_ZERO_TIME(7) | DSI_R_CLK_TRAIL_TIME(0) |
        DSI_R_CLK_T_WAKEUP(0x48);
    if (dsi_mode == DSI_MODE_VIDEO) {
        dsi->REG_00C = DSI_R_DTYPE(0x3e) | DSI_R_VC_CH_ID(0x0) |
            DSI_R_VIDEO_PACKET_LENTH(h_res*3+6) | DSI_R_INPUT_TYPE(0x0) |
            DSI_R_DLANE_AD_TIME(0x12);
    } else {
        dsi->REG_00C = DSI_R_DTYPE(0x39) | DSI_R_VC_CH_ID(0x0) |
            DSI_R_VIDEO_PACKET_LENTH(h_res*3+6+1) | DSI_R_INPUT_TYPE(0x0) |
            DSI_R_DLANE_AD_TIME(0x12);
    }
    dsi->REG_048 = DSI_R_COLOR_BAR_WIDTH(0x100) | DSI_R_HSYNC_DLY_NUM(0x96a);
    //dsi->REG_048 = DSI_R_HSYNC_DLY_NUM(0x96a);
#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(2));
#endif

    //MASK(disable) all irq
    dsi->REG_010 &= ~(DSI_INTR_MASK_MASK | //cisalbe all irq
        DSI_HS_CMD_RD_EN | DSI_LPRX_RD_EN | DSI_CMD_DONE_FLAG | DSI_BTA_DONE_FLAG |
        DSI_BTA_FAIL_FLAG | DSI_LP_RX_DONE_FLAG | DSI_RX_ERR_FLAG | DSI_LPCD_FLAG |
        DSI_RX_TIMEOUT_FLAG | DSI_TE_INTR); //clear irq status
    dsi->REG_010 |= DSI_LPRX_RD_EN;//for DCS read cmd

    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_reset_set(HAL_CMU_MOD_Q_DSI_DSI);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_PIX);
    hal_cmu_reset_clear(HAL_CMU_MOD_Q_DSI_DSI);
}

void hal_dsi_init_v2(uint16_t h_res, enum DSI_MODE_T mode, uint8_t lane_number, uint32_t dsi_bitclk, uint32_t dsi_pclk)
{
    // TODO: add more dsi_clk configurations
    uint32_t dsi_clk = 200;
    if (dsi_bitclk <= 400) {
        dsi_clk = 200;
    } else {
        dsi_clk = 250;
    }
    uint8_t pixel_div = (uint8_t)(dsi_clk * 1000 / dsi_pclk) - 2;
    if (pixel_div > 0x1F) {
        TRACE(0, "invalid pixel_div %u", pixel_div);
        return;
    }
    if (lane_number < 1 || lane_number > 2) {
        TRACE(0, "invalid lane_number %d", lane_number);
        return;
    }
    dsi_mode = mode;
    dsi_init_v2 = true;
    TRACE(0, "dsi_clk %u, pixel_div %u, dsi_mode %u", dsi_clk, pixel_div, dsi_mode);
    pmu_set_dsi_clk(dsi_clk);
    hal_cmu_dsi_clock_enable_v2(pixel_div);
    hal_dsi_init(h_res, lane_number);
}

void hal_dsi_start(void)
{
    if (dsi_mode == DSI_MODE_VIDEO) {
        dsi->REG_000 = DSI_R_LPCD_DLY(1) | DSI_R_HSA_LP |DSI_R_HBP_LP| DSI_R_HSE_EN | DSI_R_HFP_LP |
            DSI_R_EOTP_EN |DSI_R_VIDEO_MODE| DSI_R_T_LPX(0x4) | DSI_R_CLK_PRE(0x3)|
            DSI_R_CLK_POST(0x7) | DSI_R_LANE_NUM(lane_num - 1);
        dsi->REG_000 &=  ~(DSI_R_EOTP_EN |DSI_R_HSA_LP|DSI_R_HBP_LP);
    } else {
        dsi->REG_000 = DSI_R_LPCD_DLY(1) | DSI_R_HSA_LP | DSI_R_HSE_EN | DSI_R_HFP_LP | DSI_R_EOTP_EN |
                        DSI_R_T_LPX(0x4) | DSI_R_CLK_PRE(0x3) | DSI_R_CLK_POST(0x7);
    }

    dsi->REG_004 = DSI_R_HS_EXIT_TIME(0xb) | DSI_R_HS_PRPR_TIME(0x4) | DSI_R_HS_ZERO_TIME(0xb) |
                    DSI_R_HS_TRAIL_TIME(0x9) | DSI_R_T_WAKEUP(0x207);
    dsi->REG_008 = DSI_R_CLK_EXIT_TIME(0xb) | DSI_R_CLK_PRPR_TIME(0x8) | DSI_R_CLK_ZERO_TIME(0x1f) |
                    DSI_R_CLK_TRAIL_TIME(0x8) | DSI_R_CLK_T_WAKEUP(0x66);
    if (dsi_mode == DSI_MODE_VIDEO) {
        dsi->REG_048 = DSI_R_HSYNC_DLY_NUM(0x4b4);
        dsi->REG_048 &= ~(DSI_R_VIDEO_BIST_EN);
    } else {
        dsi->REG_048 = DSI_R_HSYNC_DLY_NUM(0x320);
    }
#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(100));
#endif
}

uint32_t hal_lcdc_addr_check(uint32_t addr)
{
#ifdef CHIP_BEST2003
    //2003 LCDC don't support ROM and MCU SRAM(0x20000000).
    if (addr >= DSP_RAM_BASE && addr <= DSP_RAM_BASE + DSP_RAM_SIZE)
        return 1;
#ifdef PSRAM_ENABLE
    //2003 LCDC access psram die bypas psram'cache
    //App should invalid framebuffer cache before lcdc_start, if it's located in psram
    if (addr >= PSRAM_BASE && addr <= PSRAM_BASE + PSRAM_SIZE)
        return 1;
#endif
#ifdef PSRAMUHS_ENABLE
    if (addr >= PSRAMUHS_BASE && addr <= PSRAMUHS_BASE + PSRAMUHS_SIZE)
        return 1;
#endif
    return 0;
#else
    return 1;
#endif
}

void hal_lcdc_init(const struct HAL_DSI_CFG_T *cfg, const uint8_t *layer0,
    const uint8_t *layer1, const uint8_t *layer2)
{
    hal_cmu_lcdc_clock_enable();

    hal_cmu_lcdc_reset_clear();

    lcdc->REG_200 = BITFIELD_VAL(LCD_WDMA_IMG_PITCH, cfg->active_width*4);
    lcdc->REG_208 = LCD_WDMA_BASE_ADDR(0x20000000+0x80000000);
    lcdc->REG_1A8 = LCD_CLK_INT_DIV(1);

    lcdc->REG_0E0 = LCD_CFG_DMA_PITCH_Y(cfg->y_mem_pitch) | LCD_CFG_DMA_PITCH_C(cfg->c_mem_pitch);
    lcdc->REG_0E4 = LCD_CFG_DMA_PITCH_U(cfg->uv_mem_pitch) | LCD_CFG_DMA_PITCH_V(cfg->uv_mem_pitch);
    lcdc->REG_0E8 = LCD_CFG_DMA_OVSA_HPXL(cfg->image_h_sa) | LCD_CFG_DMA_OVSA_VLN(cfg->image_v_sa);
    lcdc->REG_0EC = LCD_CFG_DMA_H_PIXEL(cfg->image_width) | LCD_CFG_DMA_V_LINE(cfg->image_height);
    lcdc->REG_0F0 = LCD_CFG_DMAZM_H_PIXEL(cfg->zm_image_width) | LCD_CFG_DMAZM_V_LINE(cfg->zm_image_height);
    lcdc->REG_0FC = LCD_CFG_GRA_PITCH(cfg->g_mem_pitch) | LCD_CFG_PN_BKLIGHTDIV(0x4);
    lcdc->REG_100 = LCD_CFG_GRA_OVSA_HPXL(cfg->graphic_h_sa) | LCD_CFG_GRA_OVSA_VLN(cfg->graphic_v_sa);
    lcdc->REG_104 = LCD_CFG_GRA_H_PIXEL(cfg->graphic_width) | LCD_CFG_GRA_V_LINE(cfg->graphic_height);
    lcdc->REG_108 = LCD_CFG_GRAZM_H_PIXEL(cfg->zm_graphic_width) | LCD_CFG_GRAZM_V_LINE(cfg->zm_graphic_height);
    lcdc->REG_10C = LCD_CFG_HWC_OVSA_HPXL(cfg->cursor_h_sa) | LCD_CFG_HWC_OVSA_VLN(cfg->cursor_v_sa);
    lcdc->REG_110 = LCD_CFG_HWC_HPXL(cfg->cursor_width) | LCD_CFG_HWC_VLN(cfg->cursor_height);
    lcdc->REG_114 = LCD_CFG_PN_H_TOTAL(cfg->total_width) | LCD_CFG_PN_V_TOTAL(cfg->total_height);
    lcdc->REG_118 = LCD_CFG_PN_H_ACTIVE(cfg->active_width) | LCD_CFG_PN_V_ACTIVE(cfg->active_height);
    lcdc->REG_11C = LCD_CFG_PN_H_FPORCH(cfg->h_front_porch) | LCD_CFG_PN_H_BPORCH(cfg->h_back_porch);
    lcdc->REG_120 = LCD_CFG_PN_V_FPORCH(cfg->v_front_porch) | LCD_CFG_PN_V_BPORCH(cfg->v_back_porch);
    //The REG_13C config value should < cfg->total_width.Don't set to a fixed value.
    lcdc->REG_13C = LCD_CFG_PN_V_SPXLCNT(cfg->total_width -1) | LCD_CFG_PN_V_EPXLCNT(cfg->total_width -1);
    // lcdc->REG_124 = LCD_CFG_PN_BLANKCOLOR(cfg->blankcolor);
    lcdc->REG_124 = 0; // FIXME
    lcdc->REG_1DC = 1 << 24; // fix red/blue mirror

    lcdc->REG_128 = LCD_CFG_HWC_COLOR1(cfg->hwc_color1);
    lcdc->REG_12C = LCD_CFG_HWC_COLOR2(cfg->hwc_color2);
    lcdc->REG_130 = LCD_CFG_PN_ALPHA_Y(cfg->cfg_alpha_y) | LCD_CFG_PN_CKEY_Y(cfg->cfg_ckey_y) |
        LCD_CFG_PN_CKEY_Y1(cfg->cfg_ckey_y1) | LCD_CFG_PN_CKEY_Y2(cfg->cfg_ckey_y2);
    lcdc->REG_134 = LCD_CFG_PN_ALPHA_U(cfg->cfg_alpha_u) | LCD_CFG_PN_CKEY_U(cfg->cfg_ckey_u) |
        LCD_CFG_PN_CKEY_U1(cfg->cfg_ckey_u1) | LCD_CFG_PN_CKEY_U2(cfg->cfg_ckey_u2);
    lcdc->REG_138 = LCD_CFG_PN_ALPHA_V(cfg->cfg_alpha_v) | LCD_CFG_PN_CKEY_V(cfg->cfg_ckey_v) |
        LCD_CFG_PN_CKEY_V1(cfg->cfg_ckey_v1) | LCD_CFG_PN_CKEY_V2(cfg->cfg_ckey_v2);
    lcdc->REG_1AC = LCD_CFG_PN_CONTRAST(cfg->contrast) | LCD_CFG_PN_BRIGHTNESS(cfg->brightness);
    lcdc->REG_1B0 = LCD_CFG_PN_SATURATION(cfg->saturation) | LCD_CFG_PN_C_MULT_S(cfg->c_mult_s);
    lcdc->REG_1B4 = LCD_CFG_PN_COS0(cfg->cos0) | LCD_CFG_PN_SIN0(cfg->sin0);
    lcdc->REG_11C = LCD_CFG_PN_H_FPORCH(cfg->h_front_porch) | LCD_CFG_PN_H_BPORCH(cfg->h_back_porch);

    lcdc->REG_020 = LCD_CFG_TVD_PITCH_Y(cfg->y_mem_pitch) | LCD_CFG_TVD_PITCH_C(cfg->c_mem_pitch);
    lcdc->REG_024 = LCD_CFG_TVD_PITCH_U(cfg->uv_mem_pitch) | LCD_CFG_TVD_PITCH_V(cfg->uv_mem_pitch);
    lcdc->REG_028 = LCD_CFG_TVD_OVSA_HPXL(cfg->image_h_sa) | LCD_CFG_TVD_OVSA_VLN(cfg->image_v_sa);
    lcdc->REG_02C = LCD_CFG_TVD_H_PIXEL(cfg->image_width) | LCD_CFG_TVD_V_LINE(cfg->image_height);
    lcdc->REG_030 = LCD_CFG_TVDZM_H_PIXEL(cfg->zm_image_width) | LCD_CFG_TVDZM_V_LINE(cfg->zm_image_height);

    lcdc->REG_03C = LCD_CFG_TVG_PITCH(cfg->g_tv_mem_pitch);
    lcdc->REG_040 = LCD_CFG_TVG_OVSA_HPXL(cfg->tvg_h_sa) | LCD_CFG_TVG_OVSA_VLN(cfg->tvg_v_sa);
    lcdc->REG_044 = LCD_CFG_TVG_H_PIXEL(cfg->tvg_width) | LCD_CFG_TVG_V_LINE(cfg->tvg_height);
    lcdc->REG_048 = LCD_CFG_TVGZM_H_PIXEL(cfg->zm_tvg_width) | LCD_CFG_TVGZM_V_LINE(cfg->zm_tvg_height);
    if (layer0) {
        ASSERT(hal_lcdc_addr_check((uint32_t)layer0), "invalid memory addr:%p", layer0);
        lcdc->REG_0C0 = LCD_CFG_DMA_SA_Y0((uint32_t)layer0);
        lcdc->REG_0C4 = LCD_CFG_DMA_SA_U0(0x30000+0x7e000000);
        lcdc->REG_0C8 = LCD_CFG_DMA_SA_V0(0x400000+0x7e000000);
        lcdc->REG_0D0 = LCD_CFG_DMA_SA_Y1((uint32_t)layer0);
		///TODO: yuv422 from camera
        if (0) {
            lcdc->REG_190 = LCD_CFG_YUV2RGB_DMA | LCD_CFG_DMA_SWAPYU | LCD_CFG_DMA_SWAPUV |
                LCD_CFG_DMA_ENA | LCD_CFG_DMA_FTOGGLE |
                LCD_CFG_DMAFORMAT(mode_yuv422packed);
        } else {
            lcdc->REG_190 = LCD_CFG_DMA_ENA | LCD_CFG_DMA_FTOGGLE |
                LCD_CFG_DMAFORMAT(mode_rgb888unpacked);
        }
    }

    if (layer1) {
        ASSERT(hal_lcdc_addr_check((uint32_t)layer1), "invalid memory addr:%p", layer1);
        lcdc->REG_0F4 = LCD_CFG_GRA_SA0((uint32_t)layer1);
        lcdc->REG_0F8 = LCD_CFG_GRA_SA1((uint32_t)layer1);
        lcdc->REG_264 = LCD_CFG_GRA_ENA | LCD_CFG_GRAFORMAT(mode_rgb888unpacked) |
            LCD_CFG_GRA_FTOGGLE;
    }

    if (layer2) {
        ASSERT(hal_lcdc_addr_check((uint32_t)layer2), "invalid memory addr:%p", layer2);
        lcdc->REG_034 = LCD_CFG_TVG_SA0((uint32_t)layer2);
        lcdc->REG_038 = LCD_CFG_TVG_SA1((uint32_t)layer2+368*4);
        lcdc->REG_080 = LCD_CFG_TVG_ENA | LCD_CFG_YUV2RGB_TVG |
            LCD_CFG_TVGFORMAT(mode_rgb888unpacked) | LCD_CFG_TVG_FTOGGLE;
    }

    //lcdc->REG_260 = LCD_CFG_TVD_ENA | LCD_CFG_ARBFAST_ENA | LCD_CFG_PN_CBSH_ENA | LCD_CFG_ARBFAST_ENA | LCD_CFG_PN_CBSH_ENA;

    lcdc->REG_1E8 = 0;
    lcdc->REG_210 = LCD_CFG_VSYNC_TRIG_DISABLE_S | LCD_CFG_VSYNC_TRIG_DISABLE_A |
        LCD_CFG_BACKLIGHT_EN | LCD_CFG_REG_GEN_FRAME | LCD_CFG_DSI_RD_MEM;
    if (dsi_mode == DSI_MODE_VIDEO) {
        lcdc->REG_188 = LCD_CFG_SLV_ENA | LCD_CFG_SMPNMODE(0x3) | LCD_CFG_SLV_ONLY;//disable smart panel
        lcdc->REG_1B8 |= LCD_CFG_DUMB_ENA;//enable dumb panel
    } else {
        lcdc->REG_188 = LCD_CFG_SMPN_ENA | LCD_CFG_SLV_ENA | LCD_CFG_SMPNMODE(0x3) | LCD_CFG_SLV_ONLY;
    }
    lcdc->REG_218 = LCD_CFG_STBY_LENGTH(0x80) | LCD_CFG_VSYNC_START_LINE(0x1) | LCD_CFG_VSYNC_END_LINE(0x5);
    lcdc->REG_21C = LCD_CFG_H_FRONT_PORCH_SMPN(0x20) | LCD_CFG_H_BACK_PORCH_SMPN(0x20) | LCD_CFG_VSYNC_NLINE(0xa);
    lcdc->REG_220 = LCD_CFG_VSYNC_START_PIX(0x1e) | LCD_CFG_VSYNC_END_PIX(0x1f);

    lcdc->REG_1C0 = LCD_TVIF_FRAMEDONE_ENA | LCD_TVG_FF_UNDERFLOW_ENA | LCD_TVG_FRAME_IRQ1_ENA |
        LCD_TVG_FRAME_IRQ0_ENA | LCD_TVSYN_IRQ_ENA | LCD_TVD_FF_UNDERFLOW_ENA |
        LCD_TVD_FRAME_IRQ1_ENA | LCD_TVD_FRAME_IRQ0_ENA | LCD_ERR_IRQ_ENA |
        LCD_PWRND_IRQ_ENA | LCD_SPI_IRQ_ENA | LCD_SLV_IRQ_ENA |
        LCD_HWC_FRAMEDONE_ENA | LCD_TWC_FRAMEDONE_ENA | LCD_DUMB_FRAMEDONE_ENA |
        LCD_VSYNC_IRQ_ENA | LCD_TVC_FRAMEDONE_ENA | LCD_GRA_FF_UNDERFLOW_ENA |
        LCD_GRA_FRAME_IRQ1_ENA | LCD_GRA_FRAME_IRQ0_ENA | LCD_M2A_IRQ_ENA |
        LCD_DMA_FF_UNDERFLOW_ENA | LCD_DMA_FRAME_IRQ1_ENA | LCD_DMA_FRAME_IRQ0_ENA;

    lcdc->REG_194 = LCD_CFG_PXLMD(0x81) | LCD_CFG_PN_ALPHA(0x9f) | LCD_CFG_PWRDN_ENA |
        LCD_CFG_GATED_ENA | LCD_CFG_FRAME_TRIG;
    if (dsi_mode == DSI_MODE_VIDEO) {
        lcdc->REG_194 |= LCD_CFG_PN_VSYNC_MODE(2);//enable VSYNC signal on dumb panel lane
    }
    lcdc->REG_1A8 = LCD_CLK_INT_DIV(1) | LCD_SCLK_SOURCE_SELECT(1);

    //TE trigger
    lcdc->REG_188 = SET_BITFIELD(lcdc->REG_188, LCD_CFG_SMPNVSYNC, 1);
    lcdc->REG_214 = LCD_CFG_HSYNC_LENGTH(5) | LCD_CFG_DEL_NLINE(2);
    lcdc->REG_210 = SET_BITFIELD(lcdc->REG_210, LCD_CFG_EXTRA_DELAY, 4);
    if (dsi_mode == DSI_MODE_VIDEO) {
        lcdc->REG_210 |= LCD_CFG_DSI_SOURCE_SEL;//switch the dumb panel as source
    }
    //lcdc->REG_210 &= ~LCD_CFG_NFRAME_ACTIVE(1);//SET_BITFIELD(lcdc->REG_210, LCD_CFG_NFRAME_ACTIVE, 1);

    //use TE, use 258 as star
    lcdc->REG_210 &= ~LCD_CFG_REG_GEN_FRAME;
    //lcdc->REG_210 |= LCD_CFG_REG_GEN_FRAME;	//manual update 
    lcdc->REG_254 |= LCD_CFG_TE_USE_SW;

    //enable irq whether ther is a TE signal or not
    lcdc->REG_1C0 = LCD_TWC_FRAMEDONE_ENA | LCD_DUMB_FRAMEDONE_ENA;
    lcdc->REG_1C4 = 0; //clear

    NVIC_SetVector(DISPLAY_IRQn, (uint32_t)hal_dsi_unified_irq_handler);
    NVIC_SetPriority(DISPLAY_IRQn, IRQ_PRIORITY_NORMAL);
    NVIC_ClearPendingIRQ(DISPLAY_IRQn);
    NVIC_EnableIRQ(DISPLAY_IRQn);

#ifndef SIMU
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif
}

void hal_lcdc_update_addr(uint8_t layerId, uint8_t channel, uint32_t addr)
{
    ASSERT(hal_lcdc_addr_check(addr), "invalid memory addr:0x%x", addr);
    switch (layerId)
    {
    case 0:
        if (channel == 0) {
            lcdc->REG_0C0 = LCD_CFG_DMA_SA_Y0(addr);
        } else {
            lcdc->REG_0D0 = LCD_CFG_DMA_SA_Y1(addr);
        }
        break;
    case 1:
        if (channel == 0) {
            lcdc->REG_0F4 = LCD_CFG_GRA_SA0(addr);
        } else {
            lcdc->REG_0F8 = LCD_CFG_GRA_SA1(addr);
        }
        break;
    case 2:
        if (channel == 0) {
            lcdc->REG_034 = LCD_CFG_TVG_SA0(addr);
        } else {
            lcdc->REG_038 = LCD_CFG_TVG_SA1(addr);
        }
        break;
    default:
        TRACE(0, "unknown layer %d", layerId);
        break;
    }
}

void hal_lcdc_set_callback(HAL_DSI_XFER_COMPLETE_CALLBACK_T callback)
{
    dsi_callback = callback;
}

void hal_lcdc_gamma_enable(const uint8_t *config_R, const uint8_t *config_G, const uint8_t *config_B)
{
    return ;
    uint32_t i;

    lcdc->REG_260 |= LCD_CFG_PN_GAMMA_ENA;
    for (i = 0; i < 256; ++i) {
        lcdc->REG_198 = LCD_SRAM_INIT_WR_RD(3) |
            LCD_CFG_SRAM_ADDR(i) | LCD_CFG_SRAM_ADDR_LCDID(0x0);//R
        lcdc->REG_19C = LCD_CFG_SRAM_WRDAT(config_R[i]);

        lcdc->REG_198 = LCD_SRAM_INIT_WR_RD(3) |
            LCD_CFG_SRAM_ADDR(i) | LCD_CFG_SRAM_ADDR_LCDID(0x1);//G
        lcdc->REG_19C = LCD_CFG_SRAM_WRDAT(config_G[i]);

        lcdc->REG_198 = LCD_SRAM_INIT_WR_RD(3) |
            LCD_CFG_SRAM_ADDR(i) | LCD_CFG_SRAM_ADDR_LCDID(0x2);//B
        lcdc->REG_19C = LCD_CFG_SRAM_WRDAT(config_B[i]);
    }
}

void hal_lcdc_gamma_disable(void)
{
    lcdc->REG_260 &= ~LCD_CFG_PN_GAMMA_ENA;
}

void hal_lcdc_start(void)
{
    //TRACE(0, "START:%u ms", FAST_TICKS_TO_MS(hal_fast_sys_timer_get()));
    //lcdc->REG_224 = LCD_GEN_FRAME_START(0);
    lcdc->REG_258 = LCD_CFG_FRAME_VALID(0);
}

void hal_lcdc_frame_done_irq_disable(void)
{
    lcdc->REG_188 = SET_BITFIELD(lcdc->REG_188, LCD_CFG_SMPNVSYNC, 3);
}

void hal_lcdc_frame_done_irq_enable(void)
{
    lcdc->REG_188 = SET_BITFIELD(lcdc->REG_188, LCD_CFG_SMPNVSYNC, 1);
}

void hal_dsi_sleep()
{
    dsiphy_sleep();
    hal_cmu_dsi_sleep();
}

void hal_dsi_wakeup()
{
    hal_cmu_dsi_wakeup();
    dsiphy_wakeup();
}

void hal_dsi_start_hs_clock()
{
    dsi->REG_014 = 1;
}

void hal_dsi_stop_hs_clock()
{
    dsi->REG_014 = 0;
}

#endif
