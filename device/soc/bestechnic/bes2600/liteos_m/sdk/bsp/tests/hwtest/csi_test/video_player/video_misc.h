/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef __VIDEO_MISC_H__
#define __VIDEO_MISC_H__

#ifdef __cpulsplus
extern "C" {
#endif

enum VIDEO_FPS_STAT_ID_T {
    VIDEO_FPS_STAT_ID_0,
    VIDEO_FPS_STAT_ID_1,

    VIDEO_FPS_STAT_ID_QTY
};

#define VIDEO_FPS_STAT_ID_CSI VIDEO_FPS_STAT_ID_0
#define VIDEO_FPS_STAT_ID_DSI VIDEO_FPS_STAT_ID_1

struct VIDEO_FPS_STAT_T {
    uint32_t open   : 1;
    uint32_t enable : 1;
    uint32_t fps_en : 1;
    uint32_t los_en : 1;
    uint32_t crc_en : 1;
    uint32_t frm_en : 1;
    uint32_t slow_en: 1;
    uint32_t rsvd   : 25;

    uint32_t n;
    uint32_t dt;
    uint32_t bt;
    uint32_t ct;
    uint32_t st;
    uint32_t pt;
    uint32_t p;
    uint32_t fps;

    uint32_t ln; //base cnt
    uint32_t lc; //lost cnt
    uint32_t lr; //lost ratio
    uint32_t lrst;

    uint32_t crc_ec;
    uint32_t crc_er;
    uint32_t crc_n;

    uint32_t frm_ec;
    uint32_t frm_er;
    uint32_t frm_n;

    uint32_t ecode;
};

void video_fps_stat_open(enum VIDEO_FPS_STAT_ID_T id);

void video_fps_stat_close(enum VIDEO_FPS_STAT_ID_T id);

void video_fps_stat_enable(enum VIDEO_FPS_STAT_ID_T id, bool enable);

void video_fps_stat_calc(enum VIDEO_FPS_STAT_ID_T id, uint32_t param);

void video_fps_stat_print(enum VIDEO_FPS_STAT_ID_T id);

///////////////////////// GPIO TRIGGER CSI DRIVER /////////////////////////

enum GPIO_TRIG_STATE_T {
    TRIG_STATE_SYNC_BEGIN = 0,
    TRIG_STATE_SYNC_END,
};

typedef void (*GPIO_TRIG_CALLBACK_T)(int pin, enum GPIO_TRIG_STATE_T state);

struct GPIO_TRIG_CONFIG_T {
    GPIO_TRIG_CALLBACK_T callback;
};

void gpio_trigger_csi_open(struct GPIO_TRIG_CONFIG_T *cfg);

void gpio_trigger_csi_close(void);

void gpio_trigger_csi_enable(bool en);

///////////////////////// MEDIA HEAP API /////////////////////////
void media_heap_init(void);

uint8_t *media_heap_malloc(uint32_t size);

void media_heap_free(uint8_t *ptr);

#ifdef __cpulsplus
}
#endif

#endif
