#ifndef LV_PORT_INDEV_H
#define LV_PORT_INDEV_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lvgl.h"
#include "cmsis_os2.h"

#define TOUCH_EVENT_UP 0x01
#define TOUCH_EVENT_DOWN 0x02

#define TOUCH_DEV_MAX 1
#define TOUCH_MSG_MAX 8

#ifdef __cplusplus
extern "C" {
#endif

struct touch_msg {
    uint16_t x;
    uint16_t y;
    uint8_t event;
};

struct touch_device {
    const char *name;
    osThreadId_t tid;
    osSemaphoreId_t sem;
    osMessageQueueId_t mq;
    int (*init)(struct touch_device *);
    void (*deinit)(void);
    void (*irq_enable)(bool);
    int (*read)(struct touch_device *, struct touch_msg *);
};

typedef void *DevHandle;

void lv_port_indev_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_H*/
