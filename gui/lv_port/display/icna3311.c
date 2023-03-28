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
#include "icna3311.h"

#ifdef CONFIG_DISPLAY_ICNA3311

static const struct HAL_DSI_CFG_T dsi_cfg = {
    .active_width = DISPLAY_XRES + 1,
    .active_height = DISPLAY_YRES + 1,
    .h_front_porch = HFP,
    .h_back_porch = HBP,
    .v_front_porch = VFP,
    .v_back_porch = VBP,
    .total_width = DISPLAY_XRES + HBP + HFP,
    .total_height = DISPLAY_YRES + VBP + VFP,

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

static icna3311_cmd_t init_cmds[] = {
    {ICNA3311_REG_PAGE_SWITCH, ICNA3311_CMD_UCS},               //page switch
    {ICNA3311_REG_TE_ON, ICNA3311_TE_MODE_VBLANK},              //te turn on output signal
    {ICNA3311_REG_MEM_CTRL, ICNA3311_MEM_IMAGE},                //memory data access control
    {ICNA3311_REG_BRIGHTNESS_CTRL, ICNA3311_BRIGHTNESS_ON},     //enable display brightness on
    {ICNA3311_REG_BRIGHTNESS_SET, ICNA3311_BRIGHTNESS_MAX},     //adjust brightness value 100%
    {ICNA3311_REG_HBM_BRIGHTNESS_SET, ICNA3311_BRIGHTNESS_MAX}, //adjust HBM mode brightness value 100%
};
static icna3311_row_column_t tm_row_column[] = {
    {ICNA3311_REG_ROW_ADDR_SET, {0x00, 0x0A, 0x01, 0x21}},
    {ICNA3311_REG_COLUMN_ADDR_SET, {0x00, 0x00, 0x01, 0xC7}}};
static icna3311_row_column_t hh_row_column[] = {
    {ICNA3311_REG_ROW_ADDR_SET, {0x00, 0x14, 0x01, 0x2B}},
    {ICNA3311_REG_COLUMN_ADDR_SET, {0x00, 0x00, 0x01, 0xC7}}};

#if defined(ICNA3311_BIST)
typedef struct bist_group_ {
    uint8_t reg;
    uint8_t value;
} bist_group_t;
//bist - build-in self-test
static bist_group_t bist_cmds[] = {
    {0xfe, 0x20},
    {0xf4, 0x5a},
    {0xf5, 0x59},
    {0x25, 0x05},
    {0x32, 0x04},
    {0xfe, 0xd0},
    {0x4e, 0x80},
    {0x4d, 0x7f},
    {0xfe, 0x00},
    {0xc2, 0x0b}};
void icna3311_bist_mode()
{
    uint32_t total = sizeof(bist_cmds) / sizeof(bist_cmds[0]);
    for (uint32_t i = 0; i < total; i++) {
        hal_dsi_send_cmd_list(bist_cmds[i].reg, 1, &bist_cmds[i].value);
    }
}
#endif

static void display_power_on(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_te = {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_DISPLAY_TE, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {HAL_GPIO_PIN_P0_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_vci = {HAL_GPIO_PIN_P0_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};

    hal_iomux_init(&pin_vci, 1);
    hal_iomux_init(&pin_rst, 1);
    hal_iomux_init(&pin_te, 1);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(pin_vci.pin, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(pin_vci.pin);
    osDelay(20);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_IN, 0);
    osDelay(20);
}

static uint8_t display_check_id(void)
{
    uint8_t id = 0;
    hal_dsi_read_cmd(ICNA3311_REG_ID2, &id, sizeof(id));
    TRACE(0, "read ic id ====%#x\r\n", id);
    return id;
}
static void display_init_code(void)
{
    uint32_t total = sizeof(init_cmds) / sizeof(init_cmds[0]);
    for (uint32_t i = 0; i < total; i++) {
        hal_dsi_send_cmd_list(init_cmds[i].reg, sizeof(init_cmds[i].parameters), &init_cmds[i].parameters);
    }
}
static void display_row_column_addr(void)
{
    uint8_t scr_id = display_check_id();
    icna3311_row_column_t *row_column = NULL;
    uint8_t total = 0;
    switch (scr_id) {
    case TM_SCREEN_ID:
        row_column = tm_row_column;
        total = sizeof(tm_row_column) / sizeof(tm_row_column[0]);
        break;
    case HH_SCREEN_ID:
        row_column = hh_row_column;
        total = sizeof(hh_row_column) / sizeof(hh_row_column[0]);
        break;
    default:
        //ASSERT;
        break;
    }

    for (uint8_t i = 0; i < total; i++) {
        hal_dsi_send_cmd_list(row_column[i].reg, sizeof(row_column[i].parameters), row_column[i].parameters);
    }
}

static void display_show_on(void)
{
    hal_dsi_send_cmd_list(ICNA3311_REG_SLEEP_OUT, 0, NULL);
    osDelay(120);
    hal_dsi_send_cmd_list(ICNA3311_REG_DISPLAY_ON, 0, NULL);
    osDelay(120);
}
static void display_on(void)
{
    /* Init the hardware and clear the display */
    uint32_t dsi_bitclk = display_calc_data_rate(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS, DISPLAY_BPP, DISPLAY_NPLANES);
    uint32_t dsi_pclk = display_calc_pixel_clk(dsi_cfg.total_width, dsi_cfg.total_height, DISPLAY_FPS);
    hal_dsi_init_v2(DISPLAY_XRES, DSI_MODE_CMD, DISPLAY_NPLANES, dsi_bitclk, dsi_pclk);
    osDelay(100);

    display_init_code();
    display_row_column_addr();
    display_show_on();
}

void display_init(void)
{
    TRACE(0, "========display xres: %d, yres: %d, nplanes: %d, bpp: %d\r\n",
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