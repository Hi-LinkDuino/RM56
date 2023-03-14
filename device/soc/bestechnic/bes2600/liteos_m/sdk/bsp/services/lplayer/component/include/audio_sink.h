/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __AUDIO_SINK_H__
#define __AUDIO_SINK_H__
#include "stdbool.h"
#include "audio_define.h"

typedef struct audio_sink audio_sink_t;

typedef enum
{
    AUDIO_PCM_8_BIT = 0x1u,  /**< 8-bit PCM */
    AUDIO_PCM_16_BIT = 0x2u, /**< 16-bit PCM */
    AUDIO_PCM_24_BIT = 0x3u, /**< 24-bit PCM */
    AUDIO_PCM_32_BIT = 0x4u, /**< 32-bit PCM */
} audio_pcm_bit_t;

typedef struct
{
    uint8_t category;
    bool interleaved;
    audio_pcm_bit_t bits;
    uint8_t ch_num;
    uint32_t sample_rate;
    uint32_t wanter_line_full;
    uint32_t player_type;
    float playback_speed;
} audio_sink_sample_attr_t;

typedef struct
{
    int64_t tv_sec;
    int64_t tv_nsec;
} audio_timestamp_t;

/**
 * volume
 * **/
int audio_sink_set_mute(audio_sink_t *sink, bool mute);
int audio_sink_get_mute(const audio_sink_t *sink, bool *mute);
int audio_sink_set_volume(const audio_sink_t *sink, uint32_t vol);
int audio_sink_get_volume(const audio_sink_t *sink, uint32_t *vol);
int audio_sink_set_float_volume(const audio_sink_t *sink, float vol);

/**
 * effect
 * **/
int audio_sink_set_effect(int32_t effect_type);
int audio_sink_set_bass_tuning(int32_t bass_tuning_value);

/**
 * ctl
 * **/
int audio_sink_start(audio_sink_t *sink);
int audio_sink_stop(audio_sink_t *sink);
int audio_sink_pause(audio_sink_t *sink);
int audio_sink_pause_without_codec_close(audio_sink_t *sink);
int audio_sink_resume(audio_sink_t *sink);
int audio_sink_flush(audio_sink_t *sink);

/**
 * attr
 * **/
int audio_sink_get_frame_size(audio_sink_t *sink, uint32_t *size);
int audio_sink_get_frame_count(audio_sink_t *sink, uint32_t *count);
int audio_sink_set_sample_attr(audio_sink_t *sink, const audio_sink_sample_attr_t *attr);
int audio_sink_get_sample_attr(const audio_sink_t *sink, audio_sink_sample_attr_t *attr);

/**
 * render
 * **/
int audio_sink_get_latency(audio_sink_t *sink, uint32_t *ms);
int audio_sink_write_frame(audio_sink_t *sink, uint8_t *frame, uint64_t length);
int audio_sink_get_render_position(audio_sink_t *sink, uint8_t *frame, audio_timestamp_t *time);
int audio_sink_set_render_speed(audio_sink_t *sink, float speed, float pitch, float rate);
int audio_sink_get_render_speed(audio_sink_t *sink, float *speed);

/**
 * adapter
 * **/
audio_sink_t *audio_sink_open(const audio_sink_sample_attr_t *attr);
int audio_sink_close(audio_sink_t *sink);

void audio_sink_init(void);

int audio_sink_global_sink_type_set(audio_sink_type_t sink_type);

#endif