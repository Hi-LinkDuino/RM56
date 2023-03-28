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
#include "hal_trace.h"
#include "hal_aud.h"
#include "plat_types.h"
#include "string.h"
#include "audio_test_cmd.h"
#include "app_trace_rx.h"
#include "app_key.h"
#include "app_media_player.h"
#if defined(ANC_APP)
#include "app_anc.h"
#include "app_anc_utils.h"
#endif

#define AUDIO_TEST_LOG_I(str, ...)      TR_INFO(TR_MOD(TEST), "[AUDIO_TEST]" str, ##__VA_ARGS__)

/**
 * Usage:
 *  1. CMD Format: e.g. [audio_test,anc_switch]
 **/

typedef void (*func_handler_t)(void);

typedef struct {
    const char *name;
    func_handler_t handler;
} audio_test_func_t;

extern void app_debug_tool_printf(const char *fmt, ...);

static void audio_test_anc_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);

#if defined(ANC_APP)
    app_anc_loop_switch();
#endif
}

static void audio_test_anc_gain_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);

#if defined(ANC_APP)
    static bool flag = false;
    if (flag) {
        app_anc_set_gain_f32(ANC_GAIN_USER_TUNING, PSAP_FEEDFORWARD, 1.0, 1.0);
    } else {
        app_anc_set_gain_f32(ANC_GAIN_USER_TUNING, PSAP_FEEDFORWARD, 0.0, 0.0);
    }

    flag = !flag;
#endif
}

extern int32_t audio_test_stream(void);
static void audio_test_stream_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);

    audio_test_stream();
}

#if defined(ANC_ASSIST_ENABLED)
#include "app_anc_assist.h"
#endif
static void audio_test_anc_assist_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#if defined(ANC_ASSIST_ENABLED)
    static bool flag = true;
    if (flag) {
        app_anc_assist_open(ANC_ASSIST_USER_ANC);
    } else {
        app_anc_assist_close(ANC_ASSIST_USER_ANC);
    }

    flag = !flag;
#endif
}

#if defined(ANC_ASSIST_ENABLED)
#include "app_voice_assist_ai_voice.h"
#include "audioflinger.h"
#endif

static void audio_test_kws_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#if defined(ANC_ASSIST_ENABLED)

    struct AF_STREAM_CONFIG_T kws_stream_cfg;
    kws_stream_cfg.sample_rate    = AUD_SAMPRATE_16000;
    kws_stream_cfg.channel_num    = AUD_CHANNEL_NUM_1;
    kws_stream_cfg.bits           = AUD_BITS_16;
    kws_stream_cfg.device         = AUD_STREAM_USE_INT_CODEC;
    kws_stream_cfg.io_path        = AUD_INPUT_PATH_ASRMIC;
    kws_stream_cfg.vol            = 12;
    kws_stream_cfg.handler        = NULL;
    kws_stream_cfg.data_ptr       = NULL;
    kws_stream_cfg.data_size      = 0;
    TRACE(0,"111");
    static bool flag = true;
    if (flag) {
        // FIXME: Create a stream_cfg
        app_voice_assist_ai_voice_open(ASSIST_AI_VOICE_MODE_KWS, &kws_stream_cfg, NULL);
    } else {
        app_voice_assist_ai_voice_close();
    }

    flag = !flag;
#endif
}

#if defined(ANC_ASSIST_ENABLED)
int32_t app_voice_assist_wd_open(void);
int32_t app_voice_assist_wd_close(void);
#endif
static void audio_test_wd_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#if defined(ANC_ASSIST_ENABLED)
    static bool flag = true;
    if (flag) {
        app_voice_assist_wd_open();
    } else {
        app_voice_assist_wd_close();
    }

    flag = !flag;
#endif
}

#if defined(SPEECH_BONE_SENSOR)
#include "speech_bone_sensor.h"
#endif
static void audio_test_bone_sensor_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#if defined(SPEECH_BONE_SENSOR)
    static bool flag = true;
    if (flag) {
        speech_bone_sensor_open();
        speech_bone_sensor_start();
    } else {
        speech_bone_sensor_stop();
        speech_bone_sensor_close();
    }

    flag = !flag;
#endif
}

static void audio_test_prompt_switch(void)
{
    AUDIO_TEST_LOG_I(" %s...", __func__);
#if 1//defined(ANC_ASSIST_ENABLED)
    int prompt_buf[6] = {14, 15, 16, 17, 18, 19};
    static int prompt_cnt = 0;

    AUD_ID_ENUM id_tmp = (AUD_ID_ENUM)prompt_buf[prompt_cnt];
    TRACE(1,"[%s],id = 0x%x",__func__,(int)id_tmp);
    media_PlayAudio(id_tmp, 0);

    prompt_cnt++;

    if(6<= prompt_cnt)
        prompt_cnt = 0;

#endif
}

const audio_test_func_t audio_test_func[]= {
    {"anc_switch",          audio_test_anc_switch},
    {"stream_switch",       audio_test_stream_switch},
    {"anc_assist_switch",   audio_test_anc_assist_switch},
    {"kws_switch",          audio_test_kws_switch},
    {"wd_switch",           audio_test_wd_switch},
    {"anc_gain_switch",     audio_test_anc_gain_switch},
    {"bone_sensor_switch",  audio_test_bone_sensor_switch},
    {"prompt_switch",       audio_test_prompt_switch},
};

static uint32_t audio_test_cmd_callback(uint8_t *buf, uint32_t len)
{
    uint8_t *func_name = buf;
    func_handler_t func_handler = NULL;

    AUDIO_TEST_LOG_I("[%s] len = %d", __func__, len);

    for (uint8_t i = 0; i < ARRAY_SIZE(audio_test_func); i++) {
        if (strcmp((char *)func_name, audio_test_func[i].name) == 0) {
            func_handler = audio_test_func[i].handler;
            break;
        }
    }

    if (func_handler) {
        func_handler();
        AUDIO_TEST_LOG_I("[audio_test] cmd: OK!");
        TRACE(0, "[CMD] res : 0;");
        app_debug_tool_printf("cmd, Process %s", func_name);
    } else {
        AUDIO_TEST_LOG_I("[audio_test] cmd: Can not found cmd: %s", func_name);
        TRACE(0, "[CMD] res : 1; info : Can not found cmd(%s);", func_name);
        app_debug_tool_printf("cmd, Invalid %s", func_name);
    }

    return 0;
}

int32_t audio_test_cmd_init(void)
{
    app_trace_rx_register("audio_test", audio_test_cmd_callback);

    return 0;
}
