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

#ifdef CONFIG_DISPLAY_ST7796S
static const struct HAL_DSI_CFG_T dsi_cfg = {
    .active_width = DISPLAY_XRES,
    .active_height = DISPLAY_YRES,
    .h_front_porch = 38,
    .h_back_porch = 20,
    .v_front_porch = 8,
    .v_back_porch = 4,
    .total_width = DISPLAY_XRES + 70,
    .total_height = DISPLAY_YRES + 16,

    .g_mem_pitch = DISPLAY_XRES * 4,
    .graphic_h_sa = 12,
    .graphic_v_sa = 4,
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

static uint8_t cmd1[] = {0x36,0x48};
static uint8_t cmd2[] = {0x3A,0x77};
static uint8_t cmd3[] = {0xF0,0xc3};
static uint8_t cmd4[] = {0xF0,0x96};
static uint8_t cmd5[] = {0xB4,0x01};
static uint8_t cmd6[] = {0xB7,0xC6};
static uint8_t cmd7[] = {0xB9,0x02};
static uint8_t cmd8[] = {0xC0,0xF0,0x65};
static uint8_t cmd9[] = {0xC1,0x15};
static uint8_t cmd10[] ={0xC2,0xAF};
static uint8_t cmd11[] = {0xC5,0x0b};
static uint8_t cmd12[] = {0xE8,0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33};
static uint8_t cmd13[] = {0xE0,0xD0, 0x04, 0x08, 0x09, 0x08, 0x15, 0x2F, 0x42, 0x46, 0x28, 0x15, 0x16, 0x29, 0x2D};
static uint8_t cmd14[] = {0xE1,0xD0, 0x04, 0x09, 0x09, 0x08, 0x15, 0x2E, 0x46, 0x46, 0x28, 0x15, 0x15, 0x29, 0x2D};
static uint8_t cmd15[] = {0xF0,0x3C};
static uint8_t cmd16[] = {0xF0,0x69};
static uint8_t cmd17[] = {0x21};
static uint8_t cmd18[] = {0x11};
static uint8_t cmd19[] = {0x29};

static void display_power_on(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {HAL_GPIO_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te =  {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};

    hal_iomux_init(&pin_rst, 1);
    hal_iomux_init(&pin_te, 1);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(50);
    hal_gpio_pin_clr(pin_rst.pin);
    osDelay(50);
    hal_gpio_pin_set(pin_rst.pin);
    osDelay(120);
}

static void display_on(void)
{
    /* Init the hardware and clear the display */
    uint32_t dsi_bitclk = display_calc_data_rate(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS, DISPLAY_BPP, DISPLAY_NPLANES);
    uint32_t dsi_pclk = display_calc_pixel_clk(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS);
    hal_dsi_init_v2(DISPLAY_XRES, DSI_MODE_VIDEO, DISPLAY_NPLANES, dsi_bitclk, dsi_pclk);
    osDelay(100);

    struct dsi_cmd cmd_lists[] = {  
        {0, cmd1[0], sizeof(cmd1) - 1, &cmd1[1]},
        {0, cmd2[0], sizeof(cmd2) - 1, &cmd2[1]},
        {0, cmd3[0], sizeof(cmd3) - 1, &cmd3[1]},
        {0, cmd4[0], sizeof(cmd4) - 1, &cmd4[1]},
        {0, cmd5[0], sizeof(cmd5) - 1, &cmd5[1]},
        {0, cmd6[0], sizeof(cmd6) - 1, &cmd6[1]},
        {0, cmd7[0], sizeof(cmd7) - 1, &cmd7[1]},
        {0, cmd8[0], sizeof(cmd8) - 1, &cmd8[1]},
        {0, cmd9[0], sizeof(cmd9) - 1, &cmd9[1]},
        {0, cmd10[0], sizeof(cmd10) - 1, &cmd10[1]},
        {0, cmd11[0], sizeof(cmd11) - 1, &cmd11[1]},
        {0, cmd12[0], sizeof(cmd12) - 1, &cmd12[1]},
        {0, cmd13[0], sizeof(cmd13) - 1, &cmd13[1]},
        {0, cmd14[0], sizeof(cmd14) - 1, &cmd14[1]},
        {0, cmd15[0], sizeof(cmd15) - 1, &cmd15[1]},
        {120, cmd16[0], sizeof(cmd16) - 1, &cmd16[1]},
        {0,   cmd17[0], 0, NULL},
        {120, cmd18[0], 0, NULL},
        {120, cmd19[0], 0, NULL},
    };
    for (int i = 0; i < sizeof(cmd_lists) / sizeof(cmd_lists[0]); i++) {
        hal_dsi_send_cmd_list(cmd_lists[i].cmd, cmd_lists[i].len, cmd_lists[i].array);
        if (cmd_lists[i].delay > 0) {
            osDelay(cmd_lists[i].delay);
        }
    }

    display_read_id();

    // turn on backlight
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_led = {HAL_GPIO_PIN_P2_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pin_led, 1);
    hal_gpio_pin_set_dir(pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);
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
        *mode = DSI_MODE_VIDEO;
    }
}

#endif
