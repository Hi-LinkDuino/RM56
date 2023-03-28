#ifdef CONFIG_DISPLAY_RM69330
#include "cmsis_os2.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_dsi.h"
#include "hal_lcdc.h"
#include "lcdc.h"
#include "lv_port_disp.h"

#define DISP_LOG(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)
#define RST_PIN HAL_GPIO_PIN_P0_3

static uint8_t cmd0[] = {0xfe, 0x00};
static uint8_t cmd1[] = {0x35, 0x00};
static uint8_t cmd2[] = {0x51, 0xcf};
static uint8_t cmd3[] = {0x2a, 0x00, 0x0e, 0x01, 0xd3};
static uint8_t cmd4[] = {0x2b, 0x00, 0x00, 0x01, 0xc5};
static uint8_t cmd5[] = {0x11};
static uint8_t cmd6[] = {0x29};

static void rm69330_init()
{
    struct dsi_cmd cmd_lists[] = {
        {0, cmd0[0], sizeof(cmd0) - 1, &cmd0[1]},
        {0, cmd1[0], sizeof(cmd1) - 1, &cmd1[1]},
        {0, cmd2[0], sizeof(cmd2) - 1, &cmd2[1]},
        {0, cmd3[0], sizeof(cmd3) - 1, &cmd3[1]},
        {0, cmd4[0], sizeof(cmd4) - 1, &cmd4[1]},
        {150, cmd5[0], 0, NULL},
        {150, cmd6[0], 0, NULL},
    };
    for (int i = 0; i < sizeof(cmd_lists) / sizeof(cmd_lists[0]); i++) {
        hal_dsi_send_cmd_list(cmd_lists[i].cmd, cmd_lists[i].len, cmd_lists[i].array);
        if (cmd_lists[i].delay > 0) {
            osDelay(cmd_lists[i].delay);
        }
    }

    // just for test
    DISP_LOG("READ CMD:");
    for (uint8_t i = 0xA; i <= 0xF; i++) {
        uint8_t temp = 0;
        hal_dsi_read_cmd(i, &temp, 1);
        DISP_LOG("READ 0x%x = 0x%x", i, temp);
        osDelay(1);
    }
}

static void rm69330_reset()
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {RST_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(RST_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(RST_PIN);
    osDelay(100);
    hal_gpio_pin_clr(RST_PIN);
    osDelay(1);
    hal_gpio_pin_set(RST_PIN);
    osDelay(100);
}

void display_init(void)
{
    DISP_LOG("%s: enter", __func__);
    rm69330_reset();

    hal_dsi_init(DISPLAY_XRES, DISPLAY_NPLANES);
    osDelay(100);

    rm69330_init();
    osDelay(100);
    DISP_LOG("%s: leave", __func__);
}
#endif
