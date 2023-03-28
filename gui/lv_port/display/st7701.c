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

#ifdef CONFIG_DISPLAY_ST7701
static const struct HAL_DSI_CFG_T dsi_cfg = {
    .active_width = DISPLAY_XRES,
    .active_height = DISPLAY_YRES,
    .h_front_porch = 80,
    .h_back_porch = 40,
    .v_front_porch = 20,
    .v_back_porch = 20,
    .total_width = DISPLAY_XRES + 60,
    .total_height = DISPLAY_YRES + 150,

    .g_mem_pitch = DISPLAY_XRES * 4,
    .graphic_h_sa = 30,
    .graphic_v_sa = 20,
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

static uint8_t cmd0[] = {0x11};
static uint8_t cmd1[] = {0xFF,0x77,0x01,0x00,0x00,0x10};
static uint8_t cmd2[] = {0xC0,0x3B,0x00};
static uint8_t cmd3[] = {0xC1,0x0D,0x02};
static uint8_t cmd4[] = {0xC2,0x37,0x05};
static uint8_t cmd5[] = {0xCD,0x0F};
static uint8_t cmd6[] = {0xB0,0x00,0x11,0x18,0x0E,0x11,0x06,0x07,0x08,0x07,0x22,0x04,0x12,0x0F,0xAA,0x31,0x18};
static uint8_t cmd7[] = {0xB1,0x00,0x11,0x19,0x0E,0x12,0x07,0x08,0x08,0x08,0x22,0x04,0x11,0x11,0xA9,0x32,0x18};
static uint8_t cmd8[] = {0xFF,0x77,0x01,0x00,0x00,0x11};
static uint8_t cmd9[] = {0xB0,0x60};
static uint8_t cmd10[] ={0xB1,0x26};
static uint8_t cmd11[] = {0xB2,0x07};
static uint8_t cmd12[] = {0xB3,0x80};
static uint8_t cmd13[] = {0xB5,0x49};
static uint8_t cmd14[] = {0xB7,0x85};
static uint8_t cmd15[] = {0xB8,0x21};
static uint8_t cmd16[] = {0xC1,0x78};
static uint8_t cmd17[] = {0xC2,0x78};
static uint8_t cmd18[] = {0xE0,0x00,0x1B,0x02};
static uint8_t cmd19[] = {0xE1,0x08,0xA0,0x00,0x00,0x07,0xA0,0x00,0x00,0x00,0x44,0x44};
static uint8_t cmd20[] = {0xE2,0x11,0x11,0x44,0x44,0xED,0xA0,0x00,0x00,0xEC,0xA0,0x00,0x00};
static uint8_t cmd21[] = {0xE3,0x00,0x00,0x11,0x11};
static uint8_t cmd22[] = {0xE4,0x44,0x44};
static uint8_t cmd23[] = {0xE5,0x0A,0xE9,0xD8,0xA0,0x0C,0xEB,0xD8,0xA0,0x0E,0xED,0xD8,0xA0,0x10,0xEF,0xD8,0xA0};
static uint8_t cmd24[] = {0xE6,0x00,0x00,0x11,0x11};
static uint8_t cmd25[] = {0xE7,0x44,0x44};
static uint8_t cmd26[] = {0xE8,0x09,0xE8,0xD8,0xA0,0x0B,0xEA,0xD8,0xA0,0x0D,0xEC,0xD8,0xA0,0x0F,0xEE,0xD8,0xA0};
static uint8_t cmd27[] = {0xEB,0x02,0x00,0xE4,0xE4,0x88,0x00,0x40};
static uint8_t cmd28[] = {0xEC,0x3C,0x00};
static uint8_t cmd29[] = {0xED,0xAB,0x89,0x76,0x54,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x20,0x45,0x67,0x98,0xBA};
static uint8_t cmd30[] = {0x36,0x00};
static uint8_t cmd31[] = {0x3A,0x77};//0x66
static uint8_t cmd32[] = {0xFF,0x77,0x01,0x00,0x00,0x13};
static uint8_t cmd33[] = {0xE5,0xE4};
static uint8_t cmd34[] = {0x29};
static uint8_t cmd35[] = {0x2C};
static uint8_t cmd36[] = {0x28};
static uint8_t cmd37[] = {0x10};
static uint8_t cmd38[] = {0x51, 0x1f};

// add for test start
static uint8_t cmd39[] = {0xFF,0x77,0x01,0x00,0x00,0x12};
static uint8_t cmd40[] = {0xD1,0x81,0x08,0x03,0x20,0x08,0x01,0xA0,0x01,0xE0,0xA0,0x01,0xE0,0x03,0x56};
static uint8_t cmd41[] = {0xD2,0x07};
// add for test end

static void display_power_on(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {HAL_GPIO_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te = {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};

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
        {0, cmd16[0], sizeof(cmd16) - 1, &cmd16[1]},
        {0, cmd17[0], sizeof(cmd17) - 1, &cmd17[1]},
        {0, cmd18[0], sizeof(cmd18) - 1, &cmd18[1]},
        {0, cmd19[0], sizeof(cmd19) - 1, &cmd19[1]},
        {0, cmd20[0], sizeof(cmd20) - 1, &cmd20[1]},
        {0, cmd21[0], sizeof(cmd21) - 1, &cmd21[1]},
        {0, cmd22[0], sizeof(cmd22) - 1, &cmd22[1]},
        {0, cmd23[0], sizeof(cmd23) - 1, &cmd23[1]},
        {0, cmd24[0], sizeof(cmd24) - 1, &cmd24[1]},
        {0, cmd25[0], sizeof(cmd25) - 1, &cmd25[1]},
        {0, cmd26[0], sizeof(cmd26) - 1, &cmd26[1]},
        {0, cmd27[0], sizeof(cmd27) - 1, &cmd27[1]},
        {0, cmd28[0], sizeof(cmd28) - 1, &cmd28[1]},
        {0, cmd29[0], sizeof(cmd29) - 1, &cmd29[1]},
        {0, cmd30[0], sizeof(cmd30) - 1, &cmd30[1]},
        {0, cmd31[0], sizeof(cmd31) - 1, &cmd31[1]},
        {0, cmd32[0], sizeof(cmd32) - 1, &cmd32[1]},
        {0, cmd33[0], sizeof(cmd33) - 1, &cmd33[1]},
        {0, cmd38[0],  sizeof(cmd38) - 1, &cmd38[1]},
        // add for test start
        /*{0, cmd39[0], sizeof(cmd39) - 1, &cmd39[1]},
        {0, cmd40[0], sizeof(cmd40) - 1, &cmd40[1]},
        {0, cmd41[0], sizeof(cmd41) - 1, &cmd41[1]},*/
        //add for test end
        {120, cmd0[0],   0, NULL},
        {120, cmd34[0],  0, NULL},
        //{100, cmd35,  0, NULL},
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
