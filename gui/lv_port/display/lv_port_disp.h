#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include <stdint.h>
#include "hal_dsi.h"
#include "lvgl.h"

// #define FB_IN_PSRAM

#ifdef CONFIG_DISPLAY_RM69090
#define DISPLAY_XRES 368
#define DISPLAY_YRES 448
#define DISPLAY_NPLANES 1
#define DISPLAY_BPP 24
#elif defined(CONFIG_DISPLAY_RM69330)
#define DISPLAY_XRES 454
#define DISPLAY_YRES 454
#define DISPLAY_NPLANES 1
#define DISPLAY_BPP 24
#elif defined(CONFIG_DISPLAY_ICNA3311)
#define DISPLAY_XRES 280
#define DISPLAY_YRES 456
#define DISPLAY_NPLANES 1
#define DISPLAY_BPP 24
#elif defined(CONFIG_DISPLAY_A064)
#define DISPLAY_XRES 480
#define DISPLAY_YRES 480
#define DISPLAY_NPLANES 2
#define DISPLAY_BPP 24
#elif defined(CONFIG_DISPLAY_ILI9488)
#define DISPLAY_XRES 320
#define DISPLAY_YRES 480
#define DISPLAY_NPLANES 1
#define DISPLAY_BPP 24
#endif
#ifndef DISPLAY_FPS
#ifdef FB_IN_PSRAM
#define DISPLAY_FPS 50
#else
#define DISPLAY_FPS 60
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct dsi_cmd {
    uint16_t delay;
    uint8_t cmd;
    uint8_t len;
    uint8_t *array;
};

uint32_t display_calc_pixel_clk(uint16_t hpixel, uint16_t vline, uint8_t frameRate);
uint32_t display_calc_data_rate(uint16_t hpixel, uint16_t vline, uint8_t frameRate, uint8_t bpp, uint8_t lane_number);

void display_init();
void display_read_id();
void display_get_config(struct HAL_DSI_CFG_T **cfg, enum DSI_MODE_T *mode);

void wait_for_vsync();
void lv_port_disp_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_H*/
