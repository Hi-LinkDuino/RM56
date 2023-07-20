#ifndef __DSI_ITF_H__
#define __DSI_ITF_H__

#include <stdio.h>
#include <string.h>

enum DSI_PIXEL_TYPE {
    DSI_PIXEL_RGB565,
    DSI_PIXEL_RGB666,
    DSI_PIXEL_RGB888,
};

void dsi_itf_sw_reset(void);
void dsi_itf_sleep_in(void);
void dsi_itf_sleep_out(void);
void dsi_itf_part_disp_on(void);
void dsi_itf_normal_disp_on(void);
void dsi_itf_inv_disp_off(void);
void dsi_itf_inv_disp_on(void);
void dsi_itf_pixel_off(void);
void dsi_itf_pixel_on(void);
void dsi_itf_disp_off(void);
void dsi_itf_disp_on(void);
void dsi_itf_idle_out(void);
void dsi_itf_idle_in(void);
void dsi_itf_set_pixel_fmt(enum DSI_PIXEL_TYPE type);
void dsi_itf_set_page_address(uint16_t start_page, uint16_t end_page);
void dsi_itf_set_column_address(uint16_t start_address, uint16_t end_address);
#endif
