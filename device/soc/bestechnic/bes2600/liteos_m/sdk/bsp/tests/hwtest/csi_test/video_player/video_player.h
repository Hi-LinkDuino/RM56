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
#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#ifdef __cpulsplus
extern "C" {
#endif

#include "video_public.h"

enum VP_STREAM_ID_T {
    VP_STREAM_ID_0,
    VP_STREAM_ID_1,

    VP_STREAM_ID_QTY
};

enum VP_STREAM_T {
    VP_STREAM_PLAYBACK, //for LCD display stream
    VP_STREAM_CAPTURE,  //for camera image stream

    VP_STREAM_QTY
};

typedef uint32_t (*VP_STREAM_HANDLER_T)(uint8_t *buf, uint32_t len);

typedef void (*VP_DMA_IRQ_NOTIFY_T)(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint8_t channel, uint32_t error);

struct VP_STREAM_CONFIG_T {
    uint32_t width;
    uint32_t height;
    enum VP_PIXAL_FMT_T fmt;
    enum VP_CHANNEL_NUM_T channel_num;
    enum VP_CHANNEL_MAP_T channel_map;
    enum VP_LAYER_ID_T layer_id; //only for playback stream
    enum VP_DEVICE_T device;

    VP_STREAM_HANDLER_T handler;
    uint8_t *data_ptr;
    uint32_t data_size;
};

int vplayer_open(void);

int vplayer_stream_open(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T *cf);

int vplayer_stream_setup(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T *cfg);

int vplayer_stream_get_cfg(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T **cfg);

int vplayer_stream_start(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_stream_stop(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_stream_pause(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_stream_resume(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_stream_close(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

void vplayer_setup_irq_notify(enum VP_STREAM_T stream, VP_DMA_IRQ_NOTIFY_T notify);

int vplayer_close(void);

void vplayer_stream_calib_init(enum VP_DEVICE_T device, enum VP_STREAM_T stream);

int vplayer_stream_calib_start(enum VP_DEVICE_T device, enum VP_STREAM_T stream,
    uint32_t width, uint32_t heigth, uint8_t fmt, uint8_t *buf, uint32_t len);

void vplayer_stream_update_dma_addr(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream,
    enum VP_CHANNEL_MAP_T channel, uint8_t *addr, bool lock_thread);

enum VP_CHANNEL_MAP_T vplayer_stream_get_channel_by_addr(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint8_t *buf);

void vplayer_stream_dump_cfg(struct VP_STREAM_CONFIG_T *cfg);

#ifdef __cpulsplus
}
#endif

#endif
