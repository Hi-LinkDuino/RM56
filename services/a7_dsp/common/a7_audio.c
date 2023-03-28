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
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "audioflinger.h"
#include "hal_location.h"
#include "string.h"
#include "transq_msg.h"
#include "stdio.h"
#include "app_key.h"
#include "mcu_audio.h"
#include "a7_dump.h"

uint32_t a7_audio_task_loop_cnt = 0;

#define CAPTURE_SIZE_RATIO 2
#define ALIGNED4                        ALIGNED(4)

#ifdef A7_ALLOC_TRANSQ_BUF
static SYNC_FLAGS_LOC uint8_t ALIGNED4 capture_buf[_CAPTURE_SIZE*CAPTURE_SIZE_RATIO];
static uint8_t *p_capture_buf = capture_buf;
static SYNC_FLAGS_LOC uint8_t ALIGNED4 playback_buf[PLAYBACK_SIZE*CAPTURE_SIZE_RATIO];
static uint8_t *p_playback_buf = playback_buf;
static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;
#else
static uint8_t *p_capture_buf = NULL;
static uint8_t *p_playback_buf = NULL;
static TRANSQ_MSG_T *p_transq_msg = NULL;
#endif

#define A7_RECORD_BURST_CNT    1
#ifndef A7_RECORD_DIRECT_CB
#define A7_RECORD_BUF_NUM		16
#define A7_RECORD_BUF_SIZE		((_CAPTURE_SIZE*CAPTURE_SIZE_RATIO)>>1)
#define A7_RECORD_CHAN_SIZE     (A7_RECORD_BUF_SIZE/CHAN_NUM_CAPTURE)
static uint8_t record_write_pos = 0;
static uint8_t record_read_pos = 0;
static uint8_t record_buf_cnt = 0;
static uint8_t ALIGNED4 record_buf[A7_RECORD_BUF_SIZE*A7_RECORD_BUF_NUM];
#endif

#define _MULTI_RECORD_NUM 3
static multi_audio_dump_t cb[_MULTI_RECORD_NUM];

static uint8_t allow_wakeup_during_record = 0;

#ifndef RTOS
static int transq_tx_done = 0;
static void set_transq_tx_done(void *param)
{
    transq_tx_done = 1;
}
#else
osSemaphoreDef(a7_audio_transq_tx_sem);
static osSemaphoreId transq_tx_sem = NULL;
static void set_transq_tx_done(void *param)
{
    osSemaphoreRelease(transq_tx_sem);
}
#endif

static int __mic_tick[5];
void _mic_tick(int pos, int tick)
{
    if (pos == 1) { // remember last irq tick
        __mic_tick[0] = __mic_tick[1];
    }
    __mic_tick[pos] = tick;
}

void WEAK a7_record_process_hook(uint8_t * buf, uint32_t len)
{
    int i;

    for (int i=0; i<_MULTI_RECORD_NUM; i++) {
        if (cb[i].func) {
            cb[i].func(buf, len, cb[i].param);
        }
    }
}

int32_t read_channel(uint8_t *buf, int bufLen, int micIndex, uint8_t *rec_buf)
{
    int32_t ret = 0;
    int32_t r=0, w=0;
    if ((rec_buf == NULL) || (buf == NULL)) {
        return 0;
    }
    // read one channel of [ref-mic1-mic2]
    while (w < bufLen) {
        buf[w++] = rec_buf[r+2*micIndex];
        buf[w++] = rec_buf[r+2*micIndex+1];
        r += CHAN_NUM_CAPTURE * 2;
    }

    ret = r;

    return ret;
}

static void a7_audio_record_input(uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0)
        return;

#ifdef A7_RECORD_DIRECT_CB
    a7_record_process_hook(buf, len);
    _mic_tick(2, TICKS_TO_MS(hal_sys_timer_get()));
    //printf("a7 mic-irq irq %d - transq %d\n", __mic_tick[1]-__mic_tick[0], __mic_tick[2]-__mic_tick[1]);
#else
    if (record_buf_cnt >= A7_RECORD_BUF_NUM) {
        TRACE(0, "A7 record overflow");
        return;
    }

    memcpy(&record_buf[record_write_pos * A7_RECORD_BUF_SIZE], buf, len);
    record_buf_cnt++;
    record_write_pos++;
    record_write_pos = (record_write_pos >= A7_RECORD_BUF_NUM) ? 0 : record_write_pos;
#endif

    return;
}

static int recording = 0;

static void a7_audio_record_handler(void)
{
#ifndef A7_RECORD_DIRECT_CB
    uint8_t burst = A7_RECORD_BURST_CNT;
    //static uint32_t cnt;
    uint32_t record_len;

    record_len = (MIC_BITRATE/1000)*CAPTURE_PERIOD_MS*2*CHAN_NUM_CAPTURE;  // 16bit, 3ch
    while (record_buf_cnt && burst--) {
        a7_record_process_hook(&record_buf[record_read_pos * A7_RECORD_BUF_SIZE], record_len);
        record_buf_cnt--;
        record_read_pos++;
        record_read_pos = (record_read_pos >= A7_RECORD_BUF_NUM) ? 0 : record_read_pos;
    }

    //if ((cnt++ % 6000) == 0)
    //    TRACE(0, "%s cnt:%u", __FUNCTION__, cnt);
#endif
}

void aud_record_subscribe(aud_dump_cb_t cbf, void *param)
{
#ifdef _MULTI_RECORD_SUPPORT
    for (int i=0; i<_MULTI_RECORD_NUM; i++) {
        if ((!cb[i].func) && (!cb[i].param)) {
            cb[i].func = cbf;
            cb[i].param = param;
            printf("%s i=%d cb=0x%X param=0x%X.\r\n",__FUNCTION__,i,cb[i].func,cb[i].param);
            break;
        }
    }
#endif
}

void aud_record_unsubscribe(aud_dump_cb_t cbf, void *param)
{
#ifdef _MULTI_RECORD_SUPPORT
    for (int i=0; i<_MULTI_RECORD_NUM; i++) {
        if ((cb[i].func == cbf) && (cb[i].param == param)) {
            printf("%s i=%d cb=0x%X param=0x%X.\r\n",__FUNCTION__,i,cb[i].func,cb[i].param);
            cb[i].func = NULL;
            cb[i].param = NULL;
            break;
        }
    }
#endif
}

static void af_playback_callback(void *param)
{
#ifndef A7_RECORD_DIRECT_CB
    static uint32_t cnt;
    if ((cnt++ % 60) == 0)
        TRACE(0, "A7 playback cnt:%u", cnt);

    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    uint8_t *buf = msg->msg.stream_buf.buf;
    uint32_t len = msg->msg.stream_buf.len;
    //TRACE(0, "%s: buf=%p len=%u", __func__, buf, len);

#ifdef A7_CAPTURE_LOOP_PLAYBACK
    if (record_buf_cnt) {
        memcpy(buf, &record_buf[record_read_pos * A7_RECORD_BUF_SIZE], len);
        record_buf_cnt--;
        record_read_pos++;
        record_read_pos = (record_read_pos >= A7_RECORD_BUF_NUM) ? 0 : record_read_pos;
    }
#endif
    return;
#endif
}

static void af_capture_callback(void *param)
{
    static uint32_t cnt;
    if ((cnt++ % 60) == 0) {
		//TRACE(0, "A7 capture cnt:%u", cnt);
	} 

    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    if (msg->type == TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ) {
        if (recording) {
            uint8_t *buf = msg->msg.stream_buf.buf;
            uint32_t len = msg->msg.stream_buf.len;

            _mic_tick(1, TICKS_TO_MS(hal_sys_timer_get()));
            a7_audio_record_input(buf, len);
        }
    } else if (msg->type == TRANSQ_MSG_TYPE_AF_CAPTURE_START) {
        TRACE(0, "%s TRANSQ_MSG_TYPE_AF_CAPTURE_START", __FUNCTION__);
        recording = 1;
    } else if (msg->type == TRANSQ_MSG_TYPE_AF_CAPTURE_STOP) {
        recording = 0;
        TRACE(0, "%s TRANSQ_MSG_TYPE_AF_CAPTURE_STOP", __FUNCTION__);
    }
}

void a7_af_open(void)
{
    TRACE(0, "a7_af_open\n");
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_OPEN;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg_tx_wait_done(p_transq_msg);
    TRACE(0, "a7_af_open done\n");
}

void a7_af_close(void)
{
    TRACE(0, "a7_af_open\n");
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_OPEN;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg_tx_wait_done(p_transq_msg);
    TRACE(0, "a7_af_open done\n");
}

void a7_af_capture_start(bool sync)
{
    struct TRANSQ_MSG_AF_CONFIG_T *p_stream_cfg = &p_transq_msg->msg.stream_cfg;
    int i;

    TRACE(0, "%s", __FUNCTION__);
    if (recording == 1) {
        TRACE(0, "%s is recording, return", __FUNCTION__);
        return;
    }

    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    // pack stream_cfg
    p_stream_cfg->channel_map = 0;
#ifndef __DIGMIC_USED__
#if defined(HW_AUD_LOOPBACK_MODE)
    p_stream_cfg->channel_map = AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2;
#else
#if defined(CFG_CAPTURE_CHANNEL_MAP) /* add compile option could config it */
    p_stream_cfg->channel_map = CFG_CAPTURE_CHANNEL_MAP;
#else
    for (i = 0; i < CHAN_NUM_CAPTURE; i++) {
        p_stream_cfg->channel_map |= AUD_CHANNEL_MAP_CH0 << i;
    }
#endif
#endif /* HW_AUD_LOOPBACK_MODE */
#else
    p_stream_cfg->channel_map = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_DIGMIC_CH0 | AUD_CHANNEL_MAP_DIGMIC_CH1;
#endif
    p_stream_cfg->bits = MIC_BITS;
    p_stream_cfg->channel_num = CHAN_NUM_CAPTURE; /* add compile option could config it */
    p_stream_cfg->chan_sep_buf = CHAN_SEP_BUF_CAPTURE;
    p_stream_cfg->sample_rate = MIC_BITRATE;
    p_stream_cfg->device = STREAM_DEVICE;
    p_stream_cfg->vol = TGT_ADC_VOL_LEVEL_8;
    if (STREAM_DEVICE == AUD_STREAM_USE_INT_CODEC) {
        p_stream_cfg->io_path = AUD_INPUT_PATH_MAINMIC;
    } else {
        p_stream_cfg->io_path = AUD_IO_PATH_NULL;
    }
    p_stream_cfg->data_ptr = p_capture_buf;
    p_stream_cfg->data_size = CAPTURE_SIZE*CAPTURE_SIZE_RATIO;
    int period_ms = (p_stream_cfg->data_size/((mic_get_samplerate()/1000)*CHAN_NUM_CAPTURE*(MIC_BITS/8)))/2;
    TRACE(0, "%s, %d data_ptr=%p, data_size=%d[%dms]", __func__, __LINE__, p_stream_cfg->data_ptr, p_stream_cfg->data_size, period_ms);

    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_CAPTURE_START;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg_tx_wait_done(p_transq_msg);
    recording = 1;
    TRACE(0, "%s end", __FUNCTION__);
}

void a7_af_capture_stop(bool sync)
{
    TRACE(0, "%s", __FUNCTION__);
    recording = 0;
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_CAPTURE_STOP;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg_tx_wait_done(p_transq_msg);
    TRACE(0, "%s end", __FUNCTION__);
}

void a7_af_playback_start(void)
{
    struct TRANSQ_MSG_AF_CONFIG_T stream_cfg;

    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = AUD_BITS_16;
    stream_cfg.channel_num = AUD_CHANNEL_NUM_1;
    stream_cfg.channel_map = AUD_CHANNEL_MAP_CH0;
    stream_cfg.sample_rate = MIC_BITRATE;
    stream_cfg.device = STREAM_DEVICE;
    stream_cfg.vol = TGT_VOLUME_LEVEL_15;
    stream_cfg.io_path = AUD_IO_PATH_NULL;
    stream_cfg.data_ptr = p_playback_buf;
    stream_cfg.data_size = PLAYBACK_SIZE*CAPTURE_SIZE_RATIO;

    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_PLAYBACK_START;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    memcpy(&p_transq_msg->msg.stream_cfg, &stream_cfg, sizeof(stream_cfg));
    transq_msg_tx_wait_done(p_transq_msg);
}

void a7_af_playback_stop(void)
{
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_PLAYBACK_STOP;
    p_transq_msg->pri = HAL_TRANSQ_PRI_NORMAL;
    transq_msg_tx_wait_done(p_transq_msg);
}

uint32_t a7_audio_record_deliver(uint8_t *buf, uint32_t len)
{
    memset(p_transq_msg, 0, sizeof(TRANSQ_MSG_T));
    p_transq_msg->type = TRANSQ_MSG_TYPE_RECORD_DATA;
    p_transq_msg->pri = HAL_TRANSQ_PRI_HIGH;
    p_transq_msg->msg.stream_buf.buf = buf;
    p_transq_msg->msg.stream_buf.len = len;
    transq_msg_tx_wait_done(p_transq_msg);
    return 0;
}

void a7_audio_task_main(void)
{
    a7_audio_task_loop_cnt++;

#ifndef A7_CAPTURE_LOOP_PLAYBACK
    /*  record handler */
    a7_audio_record_handler();
#endif
}

void a7_audio_task_init(void)
{
    if (p_capture_buf == NULL) {
        p_capture_buf = a7_dsp_heap_alloc(CAPTURE_SIZE*CAPTURE_SIZE_RATIO);
    }
    if (p_playback_buf == NULL) {
        p_playback_buf = a7_dsp_heap_alloc(PLAYBACK_SIZE*CAPTURE_SIZE_RATIO);
    }
    if (p_transq_msg == NULL) {
        p_transq_msg = a7_dsp_heap_alloc(sizeof(TRANSQ_MSG_T));
    }
    printf("%s p_capture_buf=0x%X, p_playback_buf=0x%X, p_transq_msg=0x%X\n", __FUNCTION__, p_capture_buf, p_playback_buf, p_transq_msg);

#ifdef RTOS
    if (transq_tx_sem == NULL) {
        transq_tx_sem = osSemaphoreCreate(osSemaphore(a7_audio_transq_tx_sem), 0);
        ASSERT(transq_tx_sem!=NULL, "a7_audio_task_init osSemaphoreCreate fail!");
    }
#endif

    transq_msg_register(TRANSQ_MSG_TYPE_AF_OPEN, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CLOSE, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_START, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_STOP, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_PLAYBACK_START, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_PLAYBACK_STOP, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_RECORD_DATA, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_ALGORITHM_DATA, set_transq_tx_done, true);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ, af_capture_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_PLAYBACK_IRQ, af_playback_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_START, af_capture_callback, false);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_STOP, af_capture_callback, false);

    //a7_af_open();
    a7_af_capture_start(true);
#ifdef A7_CAPTURE_LOOP_PLAYBACK
    recording = 1;
    a7_af_playback_start();
#endif
}

