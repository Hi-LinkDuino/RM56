#ifndef _LVGL_APP_H_
#define _LVGL_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void lv_main(void);

uint32_t lv_sys_time(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
