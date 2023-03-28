#ifdef CONFIG_DISPLAY_A064
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
#define BACKLIGHT_PIN HAL_GPIO_PIN_P2_6

static uint8_t cmd0[] = {0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00};
static uint8_t cmd1[] = {0xF6, 0x5A, 0x87};
static uint8_t cmd2[] = {0x3A, 0x70};
static uint8_t cmd3[] = {0xC1, 0x3F};
static uint8_t cmd4[] = {0xC2, 0x0E};
static uint8_t cmd5[] = {0xC6, 0xF8};
static uint8_t cmd6[] = {0xC9, 0x10};
static uint8_t cmd7[] = {0xCD, 0x25};
static uint8_t cmd8[] = {0xF8, 0x8A};
static uint8_t cmd9[] = {0xAC, 0x65};
static uint8_t cmd10[] = {0xA0, 0xDD};
static uint8_t cmd11[] = {0xA7, 0x47};
static uint8_t cmd12[] = {0xFA, 0x00, 0x00, 0x00, 0x04};
static uint8_t cmd13[] = {0x86, 0x99, 0xa3, 0xa3, 0x51};
static uint8_t cmd14[] = {0xA3, 0xEE};
static uint8_t cmd15[] = {0xFD, 0x3c, 0x3c, 0x00};
static uint8_t cmd16[] = {0x71, 0x48};
static uint8_t cmd17[] = {0x72, 0x48};
static uint8_t cmd18[] = {0x73, 0x00, 0x44};
static uint8_t cmd19[] = {0x97, 0xEE};
static uint8_t cmd20[] = {0x83, 0x93};
static uint8_t cmd21[] = {0x9A, 0x79};
static uint8_t cmd22[] = {0x9B, 0x62};
static uint8_t cmd23[] = {0x82, 0x34, 0x34};
static uint8_t cmd24[] = {0xB1, 0x90}; // 10 REV-0-BGR-X-X-X-GS-SS   //mipi
static uint8_t cmd25[] = {0x6D, 0x00, 0x1F, 0x19, 0x1A, 0x10, 0x0e, 0x0c, 0x0a, 0x02, 0x07, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x08, 0x01, 0x09, 0x0b, 0x0D, 0x0F, 0x1a, 0x19, 0x1f, 0x00};
static uint8_t cmd26[] = {0x64, 0x38, 0x05, 0x01, 0xdb, 0x03, 0x03, 0x38, 0x04, 0x01, 0xdc, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd27[] = {0x65, 0x38, 0x03, 0x01, 0xdd, 0x03, 0x03, 0x38, 0x02, 0x01, 0xde, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd28[] = {0x66, 0x38, 0x01, 0x01, 0xdf, 0x03, 0x03, 0x38, 0x00, 0x01, 0xe0, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd29[] = {0x67, 0x30, 0x01, 0x01, 0xe1, 0x03, 0x03, 0x30, 0x02, 0x01, 0xe2, 0x03, 0x03, 0x7A, 0x7A, 0x7A, 0x7A};
static uint8_t cmd30[] = {0x68, 0x00, 0x08, 0x15, 0x08, 0x15, 0x7A, 0x7A, 0x08, 0x15, 0x08, 0x15, 0x7A, 0x7A};
static uint8_t cmd31[] = {0x60, 0x38, 0x07, 0x7A, 0x7A, 0x38, 0x08, 0x7A, 0x7A};
static uint8_t cmd32[] = {0x63, 0x31, 0xe4, 0x7A, 0x7A, 0x31, 0xe5, 0x7A, 0x7A};
static uint8_t cmd33[] = {0x69, 0x04, 0x22, 0x14, 0x22, 0x14, 0x22, 0x08};
static uint8_t cmd34[] = {0x6B, 0x07};
static uint8_t cmd35[] = {0x7A, 0x08, 0x13};
static uint8_t cmd36[] = {0x7B, 0x08, 0x13};
static uint8_t cmd37[] = {0xD1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd38[] = {0xD2, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd39[] = {0xD3, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd40[] = {0xD4, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd41[] = {0xD5, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd42[] = {0xD6, 0x00, 0x00, 0x00, 0x04, 0x00, 0x12, 0x00, 0x18, 0x00, 0x21, 0x00, 0x2a, 0x00, 0x35, 0x00, 0x47, 0x00, 0x56, 0x00, 0x90, 0x00, 0xe5, 0x01, 0x68, 0x01, 0xd5, 0x01, 0xd7, 0x02, 0x36, 0x02, 0xa6, 0x02, 0xee, 0x03, 0x48, 0x03, 0xa0, 0x03, 0xba, 0x03, 0xc5, 0x03, 0xd0, 0x03, 0xE0, 0x03, 0xea, 0x03, 0xFa, 0x03, 0xFF};
static uint8_t cmd43[] = {0x11};
static uint8_t cmd44[] = {0x29};

static void a064_reset(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_rst = {
        RST_PIN, HAL_IOMUX_FUNC_AS_GPIO,
        HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pin_rst, 1);
    hal_gpio_pin_set_dir(pin_rst.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(10);
    hal_gpio_pin_clr(pin_rst.pin);
    osDelay(10);
    hal_gpio_pin_set(pin_rst.pin);
    osDelay(10);
}

static void a064_backlight_on()
{
    // turn on backlight
    struct HAL_IOMUX_PIN_FUNCTION_MAP pin_led = {
        BACKLIGHT_PIN, HAL_IOMUX_FUNC_AS_GPIO,
        HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&pin_led, 1);
    hal_gpio_pin_set_dir(pin_led.pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);
}

static void a064_init(void)
{
    struct dsi_cmd cmd_lists[] = {
        {0, cmd0[0], sizeof(cmd0) - 1, &cmd0[1]},
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
        {0, cmd34[0], sizeof(cmd34) - 1, &cmd34[1]},
        {0, cmd35[0], sizeof(cmd35) - 1, &cmd35[1]},
        {0, cmd36[0], sizeof(cmd36) - 1, &cmd36[1]},
        {0, cmd37[0], sizeof(cmd37) - 1, &cmd37[1]},
        {0, cmd38[0], sizeof(cmd38) - 1, &cmd38[1]},
        {0, cmd39[0], sizeof(cmd39) - 1, &cmd39[1]},
        {0, cmd40[0], sizeof(cmd40) - 1, &cmd40[1]},
        {0, cmd41[0], sizeof(cmd41) - 1, &cmd41[1]},
        {0, cmd42[0], sizeof(cmd42) - 1, &cmd42[1]},
        {120, cmd43[0], 0, NULL},
        {20, cmd44[0], 0, NULL},
    };
    for (int i = 0; i < sizeof(cmd_lists) / sizeof(cmd_lists[0]); i++) {
        hal_dsi_send_cmd_list(cmd_lists[i].cmd, cmd_lists[i].len, cmd_lists[i].array);
        if (cmd_lists[i].delay > 0) {
            osDelay(cmd_lists[i].delay);
        }
    }

    a064_backlight_on();

    uint8_t regs[3] = {0};
    hal_dsi_read_cmd(0x4, regs, 3);
    DISP_LOG("READ ID: 0x%x-0x%x-0x%x", regs[0], regs[1], regs[2]);
}

void display_init(void)
{
    DISP_LOG("%s: enter", __func__);
    a064_reset();
    hal_dsi_init(DISPLAY_XRES, DISPLAY_NPLANES);
    osDelay(100);
    a064_init();
    DISP_LOG("%s: leave", __func__);
}
#endif