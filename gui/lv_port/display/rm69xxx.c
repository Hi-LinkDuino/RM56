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
#include "hal_dsi.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "cmsis_os2.h"
#include "lv_port_disp.h"

#if defined(CONFIG_DISPLAY_RM69090) || defined(CONFIG_DISPLAY_RM69330)
static const struct HAL_DSI_CFG_T dsi_cfg = {
    .active_width = DISPLAY_XRES + 1,
    .active_height = DISPLAY_YRES + 1,
    .h_front_porch = 13,
    .h_back_porch = 13,
    .v_front_porch = 13,
    .v_back_porch = 13,
    .total_width = DISPLAY_XRES + 32,
    .total_height = DISPLAY_YRES + 32,

    .g_mem_pitch = DISPLAY_XRES * 4,
    .graphic_h_sa = 1,
    .graphic_v_sa = 1,
    .graphic_width = DISPLAY_XRES,
    .graphic_height = DISPLAY_YRES,
    .zm_graphic_width = DISPLAY_XRES,
    .zm_graphic_height = DISPLAY_YRES,

    .cursor_h_sa = 90,
    .cursor_v_sa = 90,
    .cursor_width = 28,
    .cursor_height = 28,
    .hwc_color1 = 0x0000ff,
    .hwc_color2 = 0x00ff00,

    .blankcolor = 0x104060,

    .cos0 = 0x3fc2,
    .sin0 = 0x05a4,
    .c_mult_s = 0x3000,
    .saturation = 0x5000,
    .brightness = 0xfff8,
    .contrast = 0x5000,

    .cfg_alpha_y = 0x00,
    .cfg_ckey_y = 0x16,
    .cfg_ckey_y1 = 0x0,
    .cfg_ckey_y2 = 0x60,
    .cfg_alpha_u = 0xff,
    .cfg_ckey_u = 0x80,
    .cfg_ckey_u1 = 0x0,
    .cfg_ckey_u2 = 0x80,
    .cfg_alpha_v = 0xff,
    .cfg_ckey_v = 0x80,
    .cfg_ckey_v1 = 0x0,
    .cfg_ckey_v2 = 0x80,
};

static uint8_t cmd0[] = {0xfe, 0x00};
static uint8_t cmd1[] = {0x35, 0x00};
static uint8_t cmd2[] = {0x36, 0x08}; // fix red/blue mirror
static uint8_t cmd3[] = {0x51, 0xcf};
static uint8_t cmd4[] = {0x2a, 0x00, 0x0C, (DISPLAY_XRES + 11) >> 8, (DISPLAY_XRES + 11) & 0xff};
static uint8_t cmd5[] = {0x2b, 0x00, 0x00, (DISPLAY_YRES - 1) >> 8, (DISPLAY_YRES - 1) & 0xff};
static uint8_t cmd6[] = {0x11};
static uint8_t cmd7[] = {0x29};

void display_power_on(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te = {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {HAL_GPIO_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};

    hal_iomux_init(&pin_rst, 1);
    hal_iomux_init(&pin_te, 1);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_OUT, 0);
    osDelay(20);
    hal_gpio_pin_set(pin_rst.pin);
    osDelay(20);
}

static void display_on(void)
{
    /* Init the hardware and clear the display */
    uint32_t dsi_bitclk = display_calc_data_rate(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS, DISPLAY_BPP, DISPLAY_NPLANES);
    uint32_t dsi_pclk = display_calc_pixel_clk(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS);
    hal_dsi_init_v2(DISPLAY_XRES, DSI_MODE_CMD, DISPLAY_NPLANES, dsi_bitclk, dsi_pclk);
    osDelay(100);

    struct dsi_cmd cmd_lists[] = {
        {0, cmd0[0], sizeof(cmd0) - 1, &cmd0[1]},
        {0, cmd1[0], sizeof(cmd1) - 1, &cmd1[1]},
        {0, cmd2[0], sizeof(cmd2) - 1, &cmd2[1]},
        {0, cmd3[0], sizeof(cmd3) - 1, &cmd3[1]},
        {0, cmd4[0], sizeof(cmd4) - 1, &cmd4[1]},
        {0, cmd5[0], sizeof(cmd5) - 1, &cmd5[1]},
        {150, cmd6[0], 0, NULL},
        {150, cmd7[0], 0, NULL},
    };
    for (int i = 0; i < sizeof(cmd_lists) / sizeof(cmd_lists[0]); i++) {
        hal_dsi_send_cmd_list(cmd_lists[i].cmd, cmd_lists[i].len, cmd_lists[i].array);
        if (cmd_lists[i].delay > 0) {
            osDelay(cmd_lists[i].delay);
        }
    }
}

void display_init(void)
{
    printf("display xres: %d, yres: %d, nplanes: %d, bpp: %d\n",
           DISPLAY_XRES, DISPLAY_YRES, DISPLAY_NPLANES, DISPLAY_BPP);
    display_power_on();
    display_on();
}

void display_get_config(struct HAL_DSI_CFG_T **cfg, enum DSI_MODE_T *mode)
{
    if (cfg) {
        *cfg = &dsi_cfg;
    }
    if (mode) {
        *mode = DSI_MODE_CMD;
    }
}
#endif