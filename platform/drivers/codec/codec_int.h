/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __CODEC_INT_H__
#define __CODEC_INT_H__

#include "plat_addr_map.h"
#include "plat_types.h"
#include "hal_codec.h"
#include "hal_aud.h"
#include CHIP_SPECIFIC_HDR(codec)

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*CODEC_CLOSE_HANDLER)(void);

enum CODEC_CLOSE_TYPE_T {
    CODEC_CLOSE_NORMAL,
    CODEC_CLOSE_ASYNC_REAL,
    CODEC_CLOSE_FORCED,
};

uint32_t codec_int_open(void);
uint32_t codec_int_stream_open(enum AUD_STREAM_T stream);
uint32_t codec_int_stream_setup(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T *cfg);
void codec_int_stream_mute(enum AUD_STREAM_T stream, bool mute);
void codec_int_stream_set_chan_vol(enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol);
void codec_int_stream_restore_chan_vol(enum AUD_STREAM_T stream);
uint32_t codec_int_stream_start(enum AUD_STREAM_T stream);
uint32_t codec_int_stream_stop(enum AUD_STREAM_T stream);
uint32_t codec_int_stream_close(enum AUD_STREAM_T stream);
uint32_t codec_int_close(enum CODEC_CLOSE_TYPE_T type);
void codec_int_set_close_handler(CODEC_CLOSE_HANDLER hdlr);

typedef void (*CODEC_ANC_HANDLER)(enum AUD_STREAM_T stream, enum AUD_SAMPRATE_T rate, enum AUD_SAMPRATE_T *new_play, enum AUD_SAMPRATE_T *new_cap);

int codec_anc_open(enum ANC_TYPE_T type, enum AUD_SAMPRATE_T dac_rate, enum AUD_SAMPRATE_T adc_rate, CODEC_ANC_HANDLER hdlr);
int codec_anc_close(enum ANC_TYPE_T type);

int codec_vad_open(const struct AUD_VAD_CONFIG_T *cfg);
int codec_vad_close(void);

uint32_t codec2_int_open(void);
uint32_t codec2_int_stream_open(enum AUD_STREAM_T stream);
uint32_t codec2_int_stream_setup(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T *cfg);
void codec2_int_stream_mute(enum AUD_STREAM_T stream, bool mute);
void codec2_int_stream_set_chan_vol(enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol);
void codec2_int_stream_restore_chan_vol(enum AUD_STREAM_T stream);
uint32_t codec2_int_stream_start(enum AUD_STREAM_T stream);
uint32_t codec2_int_stream_stop(enum AUD_STREAM_T stream);
uint32_t codec2_int_stream_close(enum AUD_STREAM_T stream);
uint32_t codec2_int_close(enum CODEC_CLOSE_TYPE_T type);

#ifdef __cplusplus
}
#endif

#endif
