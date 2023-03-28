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

#ifdef CONFIG_DISPLAY_ILI9488
static const struct HAL_DSI_CFG_T dsi_cfg = {
    .active_width = DISPLAY_XRES,
    .active_height = DISPLAY_YRES,
    .h_front_porch = 15,
    .h_back_porch = 15,
    .v_front_porch = 8,
    .v_back_porch = 8,
    .total_width = DISPLAY_XRES + 35,
    .total_height = DISPLAY_YRES + 20,

    .g_mem_pitch = DISPLAY_XRES * 4,
    .graphic_h_sa = 5,
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

static uint8_t cmd0[] = {0xE0, 0x00, 0x13, 0x18, 0x04, 0x0F, 0x06, 0x3A, 0x56, 0x4D, 0x03, 0x0A, 0x06, 0x30, 0x3E, 0x0F};
static uint8_t cmd1[] = {0xE1, 0x00, 0x13, 0x18, 0x01, 0x11, 0x06, 0x38, 0x34, 0x4D, 0x06, 0x0D, 0x0B, 0x31, 0x37, 0x0F};
static uint8_t cmd2[] = {0x35, 0x00};
static uint8_t cmd3[] = {0XC0, 0x18, 0x17};       // Power Control 1
static uint8_t cmd4[] = {0xC1, 0x41};             // Power Control 2
static uint8_t cmd5[] = {0xC5, 0x00, 0x1A, 0x80}; // Power Control 3
static uint8_t cmd6[] = {0x36, 0x48};
static uint8_t cmd7[] = {0x3A, 0x66}; // Interface Pixel Format //18bit
static uint8_t cmd8[] = {0XB0, 0x00};
static uint8_t cmd9[] = {0xB1, 0xA0};                    // Frame rate 60HZ
static uint8_t cmd10[] = {0xB4, 0x02};                   // Display Inversion Control //2-dot
static uint8_t cmd11[] = {0xB6, 0x02, 0x02};             // RGB/MCU Interface Control //MCU //Source,Gate scan dieection
static uint8_t cmd12[] = {0xE9, 0x00};                   // Set Image Function,0x00 disable 24 bit data input
static uint8_t cmd13[] = {0xF7, 0xA9, 0x51, 0x2C, 0x82}; // Adjust Control
static uint8_t cmd14[] = {0x21};                         // Normal Black
static uint8_t cmd15[] = {0x11};                         // Sleep out
static uint8_t cmd16[] = {0x29};                         // Display on

static void display_power_on(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {HAL_GPIO_PIN_P2_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pin_rst, 1);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(10);
    hal_gpio_pin_clr(pin_rst.pin);
    osDelay(50);
    hal_gpio_pin_set(pin_rst.pin);
    osDelay(150);
}

static void display_on(void)
{
    /* Init the hardware and clear the display */
    uint32_t dsi_bitclk = display_calc_data_rate(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS, DISPLAY_BPP, DISPLAY_NPLANES);
    uint32_t dsi_pclk = display_calc_pixel_clk(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS);
    hal_dsi_init_v2(DISPLAY_XRES, DSI_MODE_VIDEO, DISPLAY_NPLANES, dsi_bitclk, dsi_pclk);
    osDelay(100);

    struct dsi_cmd cmd_lists[] = {
        {0, cmd0[0], sizeof(cmd0) - 1, &cmd0[1]},
        {0, cmd1[0], sizeof(cmd1) - 1, &cmd1[1]},
        // {0, cmd2[0], sizeof(cmd2) - 1, &cmd2[1]},
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
        {0, cmd14[0], 0, NULL},
        {150, cmd15[0], 0, NULL},
        {30, cmd16[0], 0, NULL},
    };
    for (int i = 0; i < sizeof(cmd_lists) / sizeof(cmd_lists[0]); i++) {
        hal_dsi_send_cmd_list(cmd_lists[i].cmd, cmd_lists[i].len, cmd_lists[i].array);
        if (cmd_lists[i].delay > 0) {
            osDelay(cmd_lists[i].delay);
        }
    }

    uint8_t regs[] = {0xDA, 0xDB, 0xDC};
    for (uint8_t i = 0; i < ARRAY_SIZE(regs); i++) {
        uint8_t temp = 0;
        hal_dsi_read_cmd(regs[i], &temp, 1);
        printf("READ 0x%x = 0x%x\n", regs[i], temp);
        osDelay(1);
    }

    // turn on backlight
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_led = {HAL_GPIO_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pin_led, 1);
    hal_gpio_pin_set_dir(pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);
    printf("######dsi clk =%d", dsi_pclk);
    // hal_dsi_send_cmd(0x23);
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
