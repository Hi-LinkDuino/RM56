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
#ifndef __VIDEO_STREAM_H__
#define __VIDEO_STREAM_H__

#ifdef __cpulsplus
extern "C" {
#endif

#include "video_player.h"

enum VIDEO_STREAM_CHAN_ID_T {
    VIDEO_STREAM_CHAN_ID_0,
    VIDEO_STREAM_CHAN_ID_1,
    VIDEO_STREAM_CHAN_ID_2,

    VIDEO_STREAM_CHAN_ID_QTY
};

typedef void (*VIDEO_STREAM_CHAN_HDLR_T)(uint32_t addr, uint32_t len, uint32_t seq);

void video_stream_chan_init(void);

int video_stream_chan_open(enum VP_STREAM_T stream,
    enum VIDEO_STREAM_CHAN_ID_T id, VIDEO_STREAM_CHAN_HDLR_T handler);

int video_stream_chan_close(enum VP_STREAM_T stream, enum VIDEO_STREAM_CHAN_ID_T id);

int video_stream_chan_start(enum VP_STREAM_T stream, enum VIDEO_STREAM_CHAN_ID_T id);

int video_stream_chan_stop(enum VP_STREAM_T stream, enum VIDEO_STREAM_CHAN_ID_T id);

int video_stream_chan_data_handler(enum VP_STREAM_T stream, uint8_t *buf, uint32_t len);

#ifdef __cpulsplus
}
#endif

#endif
