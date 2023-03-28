#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include <stdint.h>
#include "lvgl.h"

#if defined(CONFIG_DISPLAY_RM69330)
#define DISPLAY_XRES 454
#define DISPLAY_YRES 454
#define DISPLAY_NPLANES 1
#define DISPLAY_BPP 24
#define DISPLAY_MODE DSI_MODE_CMD
#elif defined(CONFIG_DISPLAY_A064)
#define DISPLAY_XRES 480
#define DISPLAY_YRES 480
#define DISPLAY_NPLANES 2
#define DISPLAY_BPP 24
#define DISPLAY_MODE DSI_MODE_VIDEO
#endif
#ifndef DISPLAY_FPS
#define DISPLAY_FPS 60
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

void lv_port_disp_init(void);
void display_init();
void wait_for_vsync();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_H*/
