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
#include "plat_types.h"
#include "codec_int.h"
#include "hal_codec.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "stdbool.h"
#include "string.h"
#include "analog.h"
#include "tgt_hardware.h"

#define CODEC_TRACE(n, s, ...)          TRACE(n, s, ##__VA_ARGS__)

#define CODEC_INT_INST                  HAL_CODEC_ID_0

#ifndef CODEC_OUTPUT_DEV
#define CODEC_OUTPUT_DEV                CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV
#endif

#ifdef __CODEC_ASYNC_CLOSE__
#include "cmsis_os.h"

#define CODEC_ASYNC_CLOSE_DELAY (5000)

static void codec_timehandler(void const *param);

osTimerDef (CODEC_TIMER, codec_timehandler);
static osTimerId codec_timer;
static CODEC_CLOSE_HANDLER close_hdlr;

enum CODEC_HW_STATE_T {
    CODEC_HW_STATE_CLOSED,
    CODEC_HW_STATE_CLOSE_PENDING,
    CODEC_HW_STATE_OPENED,
};

enum CODEC_HW_STATE_T codec_hw_state = CODEC_HW_STATE_CLOSED;
#endif

enum CODEC_USER_T {
    CODEC_USER_STREAM   = (1 << 0),
    CODEC_USER_VAD      = (1 << 1),
};

struct CODEC_CONFIG_T {
    enum CODEC_USER_T user_map;
    bool resample_en;
    bool mute_state[AUD_STREAM_NUM];
    bool chan_vol_set[AUD_STREAM_NUM];
    struct STREAM_CONFIG_T {
        bool opened;
        bool started;
        struct HAL_CODEC_CONFIG_T codec_cfg;
    } stream_cfg[AUD_STREAM_NUM];
};

static struct CODEC_CONFIG_T codec_int_cfg = {
    .user_map = 0,
    .resample_en = false,
    .mute_state = { false, false, },
    .chan_vol_set = { false, false, },
    //playback
    .stream_cfg[AUD_STREAM_PLAYBACK] = {
        .opened = false,
        .started = false,
        .codec_cfg = {
            .sample_rate = AUD_SAMPRATE_NULL,
        }
    },
    //capture
    .stream_cfg[AUD_STREAM_CAPTURE] = {
        .opened = false,
        .started = false,
        .codec_cfg = {
            .sample_rate = AUD_SAMPRATE_NULL,
        }
    }
};

#ifdef VOICE_DETECTOR_EN
static enum AUD_VAD_TYPE_T vad_type;
#endif

uint32_t codec_int_stream_open(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);
    codec_int_cfg.stream_cfg[stream].opened = true;
    return 0;
}

uint32_t codec_int_stream_setup(enum AUD_STREAM_T stream, struct HAL_CODEC_CONFIG_T *cfg)
{
    enum AUD_CHANNEL_MAP_T ch_map;

    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    if (codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate == AUD_SAMPRATE_NULL) {
        // Codec uninitialized -- all config items should be set
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_ALL;
    } else {
        // Codec initialized before -- only different config items need to be set
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_NULL;
    }

    // Always config sample rate, for the pll setting might have been changed by the other stream
    CODEC_TRACE(2,"[sample_rate]old=%d new=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate, cfg->sample_rate);

    codec_int_cfg.stream_cfg[stream].codec_cfg.sample_rate = cfg->sample_rate;
    codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_SAMPLE_RATE;

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.bits != cfg->bits)
    {
        CODEC_TRACE(2,"[bits]old=%d new=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.bits, cfg->bits);
        codec_int_cfg.stream_cfg[stream].codec_cfg.bits = cfg->bits;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_BITS;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num != cfg->channel_num)
    {
        CODEC_TRACE(2,"[channel_num]old=%d new=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num, cfg->channel_num);
        codec_int_cfg.stream_cfg[stream].codec_cfg.channel_num = cfg->channel_num;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_NUM;
    }

    ch_map = cfg->channel_map;
    if (ch_map == 0) {
        if (stream == AUD_STREAM_PLAYBACK) {
            ch_map = (enum AUD_CHANNEL_MAP_T)CODEC_OUTPUT_DEV;
        } else {
            ch_map = (enum AUD_CHANNEL_MAP_T)hal_codec_get_input_path_cfg(cfg->io_path);
        }
        ch_map &= AUD_CHANNEL_MAP_ALL;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map != ch_map)
    {
        CODEC_TRACE(2,"[channel_map]old=0x%x new=0x%x", codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, ch_map);
        codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map = ch_map;
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_VOL | HAL_CODEC_CONFIG_BITS;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma != cfg->use_dma)
    {
        CODEC_TRACE(2,"[use_dma]old=%d new=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma, cfg->use_dma);
        codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma = cfg->use_dma;
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.vol != cfg->vol)
    {
        CODEC_TRACE(3,"[vol]old=%d new=%d chan_vol_set=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.vol, cfg->vol, codec_int_cfg.chan_vol_set[stream]);
        codec_int_cfg.stream_cfg[stream].codec_cfg.vol = cfg->vol;
        if (!codec_int_cfg.chan_vol_set[stream]) {
            codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag |= HAL_CODEC_CONFIG_VOL;
        }
    }

    if(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path != cfg->io_path)
    {
        CODEC_TRACE(2,"[io_path]old=%d new=%d", codec_int_cfg.stream_cfg[stream].codec_cfg.io_path, cfg->io_path);
        codec_int_cfg.stream_cfg[stream].codec_cfg.io_path = cfg->io_path;
    }

    CODEC_TRACE(3,"[%s]stream=%d set_flag=0x%x",__func__,stream,codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag);
    hal_codec_setup_stream(CODEC_INT_INST, stream, &(codec_int_cfg.stream_cfg[stream].codec_cfg));

    return 0;
}

void codec_int_stream_mute(enum AUD_STREAM_T stream, bool mute)
{

    CODEC_TRACE(3,"%s: stream=%d mute=%d", __func__, stream, mute);

    if (mute == codec_int_cfg.mute_state[stream]) {
        CODEC_TRACE(2,"[%s] Codec already in mute status: %d", __func__, mute);
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (mute) {
            analog_aud_codec_mute();
            hal_codec_dac_mute(true);
        } else {
            hal_codec_dac_mute(false);
            analog_aud_codec_nomute();
        }
    } else {
        hal_codec_adc_mute(mute);
    }

    codec_int_cfg.mute_state[stream] = mute;
}

void codec_int_stream_set_chan_vol(enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
    CODEC_TRACE(4,"%s: stream=%d ch_map=0x%X vol=%u", __func__, stream, ch_map, vol);

    codec_int_cfg.chan_vol_set[stream] = true;
    hal_codec_set_chan_vol(stream, ch_map, vol);
}

void codec_int_stream_restore_chan_vol(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    if (codec_int_cfg.chan_vol_set[stream]) {
        codec_int_cfg.chan_vol_set[stream] = false;
        // Restore normal volume
        codec_int_cfg.stream_cfg[stream].codec_cfg.set_flag = HAL_CODEC_CONFIG_VOL;
        hal_codec_setup_stream(CODEC_INT_INST, stream, &(codec_int_cfg.stream_cfg[stream].codec_cfg));
    }
}

static void codec_hw_start(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    if (stream == AUD_STREAM_PLAYBACK) {
        // Enable DAC before starting stream (specifically before enabling PA)
        analog_aud_codec_dac_enable(true);
    }

    hal_codec_start_stream(CODEC_INT_INST, stream);
}

static void codec_hw_stop(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    hal_codec_stop_stream(CODEC_INT_INST, stream);

#ifndef CLOSE_PA_DAC_NOT_IN_STOP
    if (stream == AUD_STREAM_PLAYBACK) {
        // Disable DAC after stopping stream (specifically after disabling PA)
        analog_aud_codec_dac_enable(false);
    }
#endif
}

uint32_t codec_int_stream_start(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    codec_int_cfg.stream_cfg[stream].started = true;

    if (stream == AUD_STREAM_CAPTURE) {
        analog_aud_codec_adc_enable(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path,
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, true);
    }

    hal_codec_start_interface(CODEC_INT_INST, stream,
        codec_int_cfg.stream_cfg[stream].codec_cfg.use_dma);

    codec_hw_start(stream);

    return 0;
}

uint32_t codec_int_stream_stop(enum AUD_STREAM_T stream)
{
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);

    hal_codec_stop_interface(CODEC_INT_INST, stream);

    codec_hw_stop(stream);

    if (stream == AUD_STREAM_CAPTURE) {
        analog_aud_codec_adc_enable(codec_int_cfg.stream_cfg[stream].codec_cfg.io_path,
            codec_int_cfg.stream_cfg[stream].codec_cfg.channel_map, false);
    }

    codec_int_cfg.stream_cfg[stream].started = false;

    return 0;
}

uint32_t codec_int_stream_close(enum AUD_STREAM_T stream)
{
    //close all stream
    CODEC_TRACE(2,"%s: stream=%d", __func__, stream);
    if (codec_int_cfg.stream_cfg[stream].started) {
        codec_int_stream_stop(stream);
    }
    codec_int_stream_restore_chan_vol(stream);
    codec_int_cfg.stream_cfg[stream].opened = false;
    return 0;
}

static void codec_hw_open(enum CODEC_USER_T user)
{
    enum CODEC_USER_T old_map;
#ifdef __AUDIO_RESAMPLE__
    bool resample_en = !!hal_cmu_get_audio_resample_status();
#endif

    old_map = codec_int_cfg.user_map;
    codec_int_cfg.user_map |= user;

    if (old_map) {
#ifdef __AUDIO_RESAMPLE__
        ASSERT(codec_int_cfg.resample_en == resample_en,
            "%s: Bad resamp status %d for user 0x%X (old_map=0x%X)", __func__, resample_en, user, old_map);
#endif
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    codec_int_cfg.resample_en = resample_en;
#endif

    CODEC_TRACE(1,"%s", __func__);

#ifdef __CODEC_ASYNC_CLOSE__
    if (codec_timer == NULL) {
        codec_timer = osTimerCreate (osTimer(CODEC_TIMER), osTimerOnce, NULL);
    }
    osTimerStop(codec_timer);
#endif

    // Audio resample: Might have different clock source, so must be reconfigured here
    hal_codec_open(CODEC_INT_INST);

#ifdef __CODEC_ASYNC_CLOSE__
    CODEC_TRACE(2,"%s: codec_hw_state=%d", __func__, codec_hw_state);

    if (codec_hw_state == CODEC_HW_STATE_CLOSED) {
        codec_hw_state = CODEC_HW_STATE_OPENED;
        // Continue to open the codec hardware
    } else if (codec_hw_state == CODEC_HW_STATE_CLOSE_PENDING) {
        // Still opened
        codec_hw_state = CODEC_HW_STATE_OPENED;
        return;
    } else {
        // Already opened
        return;
    }
#endif

    analog_aud_codec_open();
}

static void codec_hw_close(enum CODEC_CLOSE_TYPE_T type, enum CODEC_USER_T user)
{
    codec_int_cfg.user_map &= ~user;

    if (type == CODEC_CLOSE_NORMAL) {
        if (codec_int_cfg.user_map) {
            return;
        }
    }

#ifdef __CODEC_ASYNC_CLOSE__
    CODEC_TRACE(3,"%s: type=%d codec_hw_state=%d", __func__, type, codec_hw_state);
#else
    CODEC_TRACE(2,"%s: type=%d", __func__, type);
#endif

    if (type == CODEC_CLOSE_NORMAL) {
        // Audio resample: Might have different clock source, so close now and reconfigure when open
        hal_codec_close(CODEC_INT_INST);
#ifdef CODEC_POWER_DOWN
        memset(&codec_int_cfg, 0, sizeof(codec_int_cfg));
#endif
#ifdef __CODEC_ASYNC_CLOSE__
        ASSERT(codec_hw_state == CODEC_HW_STATE_OPENED, "%s: (type=%d) Bad codec_hw_state=%d", __func__, type, codec_hw_state);
        // Start a timer to close the codec hardware
        codec_hw_state = CODEC_HW_STATE_CLOSE_PENDING;
        osTimerStop(codec_timer);
        osTimerStart(codec_timer, CODEC_ASYNC_CLOSE_DELAY);
        return;
    } else if (type == CODEC_CLOSE_ASYNC_REAL) {
        if (codec_hw_state != CODEC_HW_STATE_CLOSE_PENDING) {
            // Already closed or reopened
            return;
        }
        codec_hw_state = CODEC_HW_STATE_CLOSED;
#endif
    } else if (type == CODEC_CLOSE_FORCED) {
        hal_codec_crash_mute();
    }

    analog_aud_codec_close();
}

uint32_t codec_int_open(void)
{
    CODEC_TRACE(2,"%s: user_map=0x%X", __func__, codec_int_cfg.user_map);

    codec_hw_open(CODEC_USER_STREAM);

    return 0;
}

uint32_t codec_int_close(enum CODEC_CLOSE_TYPE_T type)
{
    CODEC_TRACE(3,"%s: type=%d user_map=0x%X", __func__, type, codec_int_cfg.user_map);

    if (type == CODEC_CLOSE_NORMAL) {
        if (codec_int_cfg.stream_cfg[AUD_STREAM_PLAYBACK].opened == false &&
                codec_int_cfg.stream_cfg[AUD_STREAM_CAPTURE].opened == false){
            codec_hw_close(type, CODEC_USER_STREAM);
        }
    } else {
        codec_hw_close(type, CODEC_USER_STREAM);
    }

    return 0;
}

#ifdef __CODEC_ASYNC_CLOSE__
void codec_int_set_close_handler(CODEC_CLOSE_HANDLER hdlr)
{
    close_hdlr = hdlr;
}

static void codec_timehandler(void const *param)
{
    if (close_hdlr) {
        close_hdlr();
    }
}
#endif

#ifdef VOICE_DETECTOR_EN
int codec_vad_open(const struct AUD_VAD_CONFIG_T *cfg)
{
    if (cfg->type == AUD_VAD_TYPE_NONE) {
        return codec_vad_close();
    }

    if (vad_type == cfg->type) {
        return 0;
    }

    vad_type = cfg->type;

    if (vad_type == AUD_VAD_TYPE_DIG) {
        hal_chip_wake_lock(HAL_CHIP_WAKE_LOCK_USER_VAD);
    }

    codec_hw_open(CODEC_USER_VAD);

    hal_codec_vad_open(cfg);

    return 0;
}

int codec_vad_close(void)
{
    if (vad_type == AUD_VAD_TYPE_NONE) {
        return 0;
    }

    hal_codec_vad_close();

    codec_hw_close(CODEC_CLOSE_NORMAL, CODEC_USER_VAD);

    if (vad_type == AUD_VAD_TYPE_DIG) {
        hal_chip_wake_unlock(HAL_CHIP_WAKE_LOCK_USER_VAD);
    }

    vad_type = AUD_VAD_TYPE_NONE;

    return 0;
}
#endif

