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
#ifndef __AUDIO_USER_PROCESS_H__
#define __AUDIO_USER_PROCESS_H__

#include "stdbool.h"

typedef enum
{
    ADUIO_USER_TYPE_A2DP,
    AUDIO_USER_TYPE_SCO,
    ADUIO_USER_TYPE_BIS,
    AUDIO_USER_TYPE_ALSA,
    AUDIO_USER_TYPE_NUM,
} audio_user_process_type;

#define USER_PROCESS_48k_SAMPLE_NUM (256 * 2)

typedef int (*audio_user_process_open)(audio_user_process_type type, int sample_rate, int in_channels, int out_channels, int in_bits, int out_bits, uint32_t timeout);
typedef int (*audio_user_process_close)(audio_user_process_type type);
typedef int (*audio_user_process_run)(uint8_t *data, uint32_t *in_out_length);

typedef struct _audio_user_process_if_s
{
    audio_user_process_open open;
    audio_user_process_close close;
    audio_user_process_run run;
} audio_user_process_if_t;

typedef enum
{
    AUDIO_USER_NOTIFY_FORMAT_S16 = 0,
    AUDIO_USER_NOTIFY_FORMAT_S24 = 1,
    AUDIO_USER_NOTIFY_FORMAT_S32 = 2,
    AUDIO_USER_NOTIFY_FORMAT_CNT,
} audio_user_notify_format_t;

typedef int (*audio_user_notify_open_hook)(int type, void *user_arg, int sample_rate, int channels, int format);
typedef int (*audio_user_notify_start_hook)(void *user_arg);
typedef int (*audio_user_notify_process_hook)(void *user_arg, const uint8_t *input, uint32_t len);
typedef int (*audio_user_notify_stop_hook)(void *user_arg);
typedef int (*audio_user_notify_flush_hook)(void *user_arg);
typedef int (*audio_user_notify_close_hook)(void *user_arg);

struct audio_user_notify_hook_t
{
    audio_user_notify_open_hook open;
    audio_user_notify_start_hook start;
    audio_user_notify_process_hook process;
    audio_user_notify_stop_hook stop;
    audio_user_notify_flush_hook flush;
    audio_user_notify_close_hook close;
    void *user_arg;
};

typedef int (*audio_user_sw_gain_porcess_open)(uint8_t channels, uint8_t bits, uint32_t sample_rate, float f0);
typedef int (*audio_user_sw_gain_process_close)(void);
typedef int (*audio_user_sw_gain_process)(uint8_t *buf, uint32_t len);
typedef int (*audio_user_sw_gain_process_fade_in)(bool skip_zero_data);
typedef int (*audio_user_sw_gain_process_fade_out)(void);

typedef struct
{
    audio_user_sw_gain_porcess_open open;
    audio_user_sw_gain_process_close close;
    audio_user_sw_gain_process process;
    audio_user_sw_gain_process_fade_in fade_in;
    audio_user_sw_gain_process_fade_out fade_out;
} audio_user_sw_gain_process_if_t;

typedef int (*audio_user_dump_raw_data_config_t)(uint32_t sample_rate, uint8_t channels, uint8_t bits);
typedef int (*audio_user_dump_raw_data_t)(uint8_t *data, uint32_t length);
typedef int (*audio_user_dump_before_process_t)(uint8_t *data, uint32_t length);
typedef int (*audio_user_dump_after_process_t)(uint8_t *data, uint32_t length);

typedef struct
{
    audio_user_dump_raw_data_config_t dump_raw_data_config;
    audio_user_dump_raw_data_t dump_raw_data;
    audio_user_dump_before_process_t dump_before_process;
    audio_user_dump_after_process_t dump_after_process;
} audio_user_dump_if_t;

typedef void *(*audio_user_any_resample_open_t)(uint8_t channels, uint8_t bits, uint32_t sample_rate_src, uint32_t sample_rate_dst);
typedef int (*audio_user_any_resample_close_t)(void *handle);
typedef int (*audio_user_any_resample_run_t)(void *handle, uint8_t **in_out_data, uint32_t *in_out_len);
typedef struct
{
    audio_user_any_resample_open_t open;
    audio_user_any_resample_close_t close;
    audio_user_any_resample_run_t run;
} audio_user_any_resample_48k_if_t;

typedef uint8_t (*audio_user_vol_get_t)(void);
typedef int (*audio_user_vol_update_t)(void);
typedef float (*audio_user_float_vol_get_t)(void);

typedef struct
{
    audio_user_vol_get_t vol_get;
    audio_user_vol_update_t vol_update;
#if 0
    audio_user_float_vol_get_t float_vol_get;
#endif
} audio_user_vol_ctrl_if_t;

#ifdef __cplusplus
extern "C"
{
#endif

    void audio_user_process_register_if(audio_user_process_type type, audio_user_process_if_t *ift);
    void audio_user_process_unregister_if(audio_user_process_type type);
    int audio_user_process_if_open(audio_user_process_type type, int sample_rate, int in_channels, int out_channels, int in_bits, int out_bits, uint32_t timeout);
    int audio_user_process_if_close(audio_user_process_type type);
    int audio_user_process_if_run(audio_user_process_type type, uint8_t *data, uint32_t *in_out_length);
    bool audio_user_process_if_is_enable(audio_user_process_type type);

    int audio_user_notify_register(struct audio_user_notify_hook_t *notify_hook);
    int audio_user_notify_unregister(void);
    int audio_user_notify_open(int sample_rate, int channels, int bits);
    int audio_user_notify_start(void);
    int audio_user_notify_process(const uint8_t *input, uint32_t len);
    int audio_user_notify_stop(void);
    int audio_user_notify_flush(void);
    int audio_user_notify_close(void);

    void audio_user_sw_gain_process_register_if(audio_user_sw_gain_process_if_t *ift);
    void audio_user_sw_gain_process_unregister_if(void);
    int audio_user_sw_gain_process_open_if(uint8_t channels, uint8_t bits, uint32_t sample_rate, float f0);
    int audio_user_sw_gain_process_close_if(void);
    int audio_user_sw_gain_process_if(uint8_t *buf, uint32_t len);
    int audio_user_sw_gain_process_fade_in_if(bool skip_zero_data);
    int audio_user_sw_gain_process_fade_out_if(void);
    bool audio_user_sw_gain_process_if_is_enable(void);

    void audio_user_vol_ctrl_register_if(audio_user_vol_ctrl_if_t *ift);
    uint8_t audio_user_get_vol(void);
    float audio_user_get_float_vol(void);
    int audio_user_update_vol(void);

    void audio_user_dump_register_if(audio_user_dump_if_t *ift);
    void audio_user_dump_unreister_if(void);
    int audio_user_dump_raw_data_config(uint32_t sample_rate, uint8_t channels, uint8_t bits);
    int audio_user_dump_raw_data(uint8_t *data, uint32_t length);
    int audio_user_dump_before_process(uint8_t *data, uint32_t length);
    int audio_user_dump_after_process(uint8_t *data, uint32_t length);

#if 0
    void audio_user_any_resample_48k_register_if(audio_user_any_resample_48k_if_t *ift);
    void audio_user_any_resample_48k_unregister_if(void);
    void *audio_user_any_resample_48k_open(uint8_t channels, uint8_t bits, uint32_t sample_rate_src, uint32_t sample_rate_dst);
    int audio_user_any_resample_48k_close(void *handle);
    int audio_user_any_resample_48k_run(void *handle, uint8_t **in_out_data, uint32_t *in_out_len);
    int audio_user_any_resample_48k_is_enable(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
