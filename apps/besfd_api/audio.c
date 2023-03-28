
/**
 * @file audio.c
 * @author Alvin
 * @brief 
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "besfd_api.h"

#include "audioflinger.h"
#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "cqueue.h"
#include "heap_api.h"
#include "cmsis_os.h"

#define AUDIO_USE_DEVICE_I2S 0
#define AUDIO_STREAM_CH AUD_STREAM_ID_1
#define MIDDLE_BUFFER_SIZE (48 * 2 * 2 * 4 * 8)
#define CAPTURE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define PLAYBACK_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)

static uint8_t *playback_buffer = NULL; //[PLAYBACK_BUFFER_SIZE];
static uint8_t *capture_buffer = NULL;  //[PLAYBACK_BUFFER_SIZE];

#define AUDIO_OUT_QUEUE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 4)
#define AUDIO_SAMPLE_RATE AUD_SAMPRATE_48000

void aud_playback_stop(void)
{
    if (playback_buffer)
    {
        af_stream_stop(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);
        af_stream_close(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);
        vPortFree(playback_buffer);
        playback_buffer = NULL;
    }
}

void aud_playback_start(uint8_t channel, uint32_t sample_rate, uint8_t vol, AudioDataHandler handler)
{
    struct AF_STREAM_CONFIG_T stream_cfg;

    if (channel != 2 && channel != 8)
    {
        TRACE(0, "[EROOR]%s: The i2s channel config error , MUST be 2 or 8", __func__);
        return;
    }
    aud_playback_stop();
    playback_buffer = pvPortMalloc(PLAYBACK_BUFFER_SIZE);

    // enable audio output
    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = AUD_BITS_16;
    if (8 == channel)
    {
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7);
        stream_cfg.channel_num = AUD_CHANNEL_NUM_8;
    }
    else if (2 == channel)
    {
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
        stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    }

    if (vol > TGT_VOLUME_LEVEL_MAX)
        vol = TGT_VOLUME_LEVEL_MAX;
    stream_cfg.sample_rate = sample_rate;
    stream_cfg.vol = vol;
#if AUDIO_USE_DEVICE_I2S
    stream_cfg.device = AUD_STREAM_USE_I2S1_MASTER;
#else
    stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
    stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
    stream_cfg.handler = handler;
    stream_cfg.data_ptr = playback_buffer;
    stream_cfg.data_size = PLAYBACK_BUFFER_SIZE;
    af_stream_open(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK, &stream_cfg);
    af_stream_start(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);
}

void aud_capture_stop(void)
{
    if (capture_buffer)
    {
        af_stream_stop(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
        af_stream_close(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
        vPortFree(capture_buffer);
        capture_buffer = NULL;
    }
}
void aud_capture_start(uint8_t channel, uint32_t sample_rate, uint8_t vol, AudioDataHandler handler)
{
    struct AF_STREAM_CONFIG_T stream_cfg;

    if (channel != 2 && channel != 8)
    {
        TRACE(0, "[EROOR]%s: The i2s channel config error , MUST be 2 or 8", __func__);
        return;
    }
    aud_capture_stop();
    capture_buffer = pvPortMalloc(CAPTURE_BUFFER_SIZE);

    // enable audio input
    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = AUD_BITS_16;
    if (8 == channel)
    {
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7);
        stream_cfg.channel_num = AUD_CHANNEL_NUM_8;
    }
    else if (2 == channel)
    {
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_DIGMIC_CH0 | AUD_CHANNEL_MAP_DIGMIC_CH1);
        stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    }

    if (vol > TGT_VOLUME_LEVEL_MAX)
        vol = TGT_VOLUME_LEVEL_MAX;
    stream_cfg.sample_rate = sample_rate;
    stream_cfg.vol = vol;
#if AUDIO_USE_DEVICE_I2S
    stream_cfg.device = AUD_STREAM_USE_I2S1_MASTER;
#else
    stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
    stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
    stream_cfg.handler = handler;
    stream_cfg.data_ptr = capture_buffer;
    stream_cfg.data_size = CAPTURE_BUFFER_SIZE;
    af_stream_open(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE, &stream_cfg);
    af_stream_start(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
}

static CQueue audio_output_queue;
static uint16_t capture_auido_trace_count = 0;
static uint16_t playback_auido_trace_count = 0;

static uint32_t capture_input_cb(uint8_t *buf, uint32_t len)
{
    uint32_t queue_len = 0;

    static uint8_t enable_log = 0;
    if (CQ_OK != EnCQueue(&audio_output_queue, buf, len))
    {
        TRACE(0, "[%s] en queue error", __func__);
    }
    queue_len = LengthOfCQueue(&audio_output_queue);
    if (capture_auido_trace_count && capture_auido_trace_count % 128 == 0)
    {
        TRACE(0, "[%s] queue_len = %d", __func__, queue_len);
        // DUMP8("%02X ", buf, 16);
    }
    capture_auido_trace_count++;
    return len;
}

static uint32_t playback_output_cb(uint8_t *buf, uint32_t len)
{
    memset(buf, 0, len);
#if 0
    if (CQ_OK != DeCQueue(&audio_output_queue, buf, len))
    {
        TRACE(0, "[%s] dequeue(%d) error, queue_len = %d", __func__, len, LengthOfCQueue(&audio_output_queue));
        return len;
    }
#else
    if (CQ_OK != PeekCQueueToBuf(&audio_output_queue, buf, len))
    {
        TRACE(0, "[%s] dequeue(%d) error, queue_len = %d", __func__, len, LengthOfCQueue(&audio_output_queue));
        return len;
    }
    DeCQueue(&audio_output_queue, NULL, len);
#endif
    if (playback_auido_trace_count && playback_auido_trace_count % 128 == 0)
    {
        TRACE(0, "[%s] dequeue(%d), queue_len = %d", __func__, len, LengthOfCQueue(&audio_output_queue));
    }
    playback_auido_trace_count++;
    return len;
}

#include "app_mic.h"
void aud_loop_test_start(uint8_t channel, uint8_t vol)
{
    uint8_t *audio_output_queue_buf; //[AUDIO_OUT_QUEUE_BUFFER_SIZE];

    TRACE(0, "%s, sample_rate = %d", __func__, AUDIO_SAMPLE_RATE);

	af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    // if(app_mic_status() == MIC_APP_MICRECORD)app_mic_stop(MIC_APP_MICRECORD);
    aud_playback_stop();
    aud_capture_stop();
    capture_auido_trace_count = 0;
    playback_auido_trace_count = 0;
    syspool_init();
    // app_audio_mempool_get_buff((uint8_t **)&app_audioloop_play_cache, BT_AUDIO_FACTORMODE_BUFF_SIZE*2/2/2);
    syspool_get_buff(&audio_output_queue_buf, AUDIO_OUT_QUEUE_BUFFER_SIZE);
    InitCQueue(&audio_output_queue, AUDIO_OUT_QUEUE_BUFFER_SIZE, (CQItemType *)audio_output_queue_buf);

    aud_playback_start(channel, AUDIO_SAMPLE_RATE, vol, playback_output_cb);
    aud_capture_start(channel, AUDIO_SAMPLE_RATE, TGT_VOLUME_LEVEL_MAX, capture_input_cb);
}
void aud_loop_test_stop(void)
{
    aud_playback_stop();
    aud_capture_stop();

    // app_mic_start(MIC_APP_MICRECORD);
}

#include "app_bt_media_manager.h"

static uint32_t __get_media_mask(enum MediaType m) {
    switch(m){
        case MT_HFP:    return BT_STREAM_VOICE;
        case MT_A2DP:   return BT_STREAM_SBC;
        case MT_PROMPT: return BT_STREAM_MEDIA;
#ifdef BT_MEDIA_SUPPORT_LPLAYER
        case MT_LPLAYER:    return BT_STREAM_LPLAYER;
#endif
    }
    return 0;
}
bool media_is_current_playing(enum MediaType m){
    return !!(bt_media_get_current_media() & __get_media_mask(m));
}
bool media_is_playing(void){
    return !!bt_media_get_current_media();
}
bool media_is_active(enum MediaType m){
    return bt_media_is_media_active_by_type(__get_media_mask(m));
}


