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
#if (defined(CHIP_BEST2001) || defined(CHIP_BEST2003)) && !defined(__ARM_ARCH_ISA_ARM)
#include "plat_addr_map.h"
#include "cmsis.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_location.h"
#include "hal_psramuhs.h"
#include "string.h"
#include "transq_msg.h"
#include "cmsis_os.h"
#include "audioflinger.h"
#include "hal_sysfreq.h"
#include "string.h"
#include "mcu_audio.h"
#include "app_mic.h"

#define _MULTI_RECORD_NUM 3
static multi_audio_dump_t cb[_MULTI_RECORD_NUM];
static multi_audio_dump_t a7_to_m33_cb = {0};

static SYNC_FLAGS_LOC TRANSQ_MSG_T transq_msg;//must be uncached static parameter
static TRANSQ_MSG_T *p_transq_msg = &transq_msg;

static mcu_record_pre_handler record_pre_handler = NULL;

static uint32_t mcu_af_playback(uint8_t *buf, uint32_t len)
{
    static uint32_t cnt;
    memset(p_transq_msg, 0, sizeof(transq_msg));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_PLAYBACK_IRQ;
    p_transq_msg->pri = HAL_TRANSQ_PRI_HIGH;
    p_transq_msg->msg.stream_buf.buf = buf;
    p_transq_msg->msg.stream_buf.len = len;
    transq_msg_tx(p_transq_msg);
    if ((cnt++ % 60) == 0)
        TRACE(0, "M33 playback:%u", cnt);
    return 0;
}

static int __mic_tick[5];
void _mic_tick(int pos, int tick)
{
    if (pos == 1) { // remember last irq tick
        __mic_tick[0] = __mic_tick[1];
    }
    __mic_tick[pos] = tick;
}
#ifdef USB_AUDIO_ADAPTER
extern unsigned int CaptureDataToUsbNormalMode(unsigned char *pData, unsigned int len);
#endif
static uint32_t mcu_af_capture(uint8_t *buf, uint32_t len)
{
    //static uint32_t cnt;

    if (record_pre_handler)
        record_pre_handler(buf, len);

#ifdef _MULTI_RECORD_SUPPORT
    if (_MULTI_RECORD_DUMP_POINT == 1) {
        for (int i=0; i<_MULTI_RECORD_NUM; i++) {
            if (cb[i].func) {
                cb[i].func(buf, len, cb[i].param);
            }
        }
    }
#endif
#ifdef USB_AUDIO_ADAPTER
    CaptureDataToUsbNormalMode(buf, len);
#endif

    memset(p_transq_msg, 0, sizeof(transq_msg));
    p_transq_msg->type = TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ;
    p_transq_msg->pri = HAL_TRANSQ_PRI_HIGH;
    p_transq_msg->msg.stream_buf.buf = buf;
    p_transq_msg->msg.stream_buf.len = len;
    transq_msg_tx(p_transq_msg);
    _mic_tick(3, TICKS_TO_MS(hal_sys_timer_get()));
    //printf("mic-irq irq %d - thread %d - transq %d\n", __mic_tick[1]-__mic_tick[0], __mic_tick[2]-__mic_tick[1], __mic_tick[3]-__mic_tick[2]);

    //if ((cnt++ % 60) == 0)
    //    TRACE(0, "M33 captrue cnt:%u", cnt);
    return 0;
}

static void mcu_dsp_af_open(void *param)
{
    TRACE(0, "%s, %d", __func__, __LINE__);
    af_open();
    af_set_priority(AF_USER_TEST, osPriorityHigh);
}
#ifdef USB_AUDIO_ADAPTER
extern void UsbaudioSetMicMumNormalMode(uint8_t num);
#endif
void WEAK mcu_dsp_af_capture_start_pre_hook(struct AF_STREAM_CONFIG_T *stream_cfg)
{
    return;
}
static void mcu_dsp_af_capture_start(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    struct TRANSQ_MSG_AF_CONFIG_T *config = &msg->msg.stream_cfg;
    struct AF_STREAM_CONFIG_T stream_cfg;

    TRACE(0, "%s, %d msg=%p, config=%p", __func__, __LINE__, msg, config);
    TRACE(0, "bits=%d, channel_num=%d, channel_map=%d, sample_rate=%d, data_ptr=%p, data_size=%d", config->bits, config->channel_num, config->channel_map, config->sample_rate, config->data_ptr, config->data_size);

    transq_msg_onoff(1); // patch for a7 raw build without heart-beat

    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = config->bits;
    stream_cfg.channel_num = config->channel_num;
    stream_cfg.channel_map = config->channel_map;
    stream_cfg.chan_sep_buf = config->chan_sep_buf;
    stream_cfg.sample_rate = config->sample_rate;
    mic_set_samplerate(stream_cfg.sample_rate);
    stream_cfg.device = config->device;
    stream_cfg.vol = config->vol;
    stream_cfg.handler = mcu_af_capture;
    stream_cfg.io_path = config->io_path;
    stream_cfg.data_ptr = config->data_ptr;
    stream_cfg.data_size = config->data_size;
    int period_ms = (config->data_size/((mic_get_samplerate()/1000)*CHAN_NUM_CAPTURE*(MIC_BITS/8)))/2;
    TRACE(0, "%s, %d data_ptr=%p, data_size=%d[%dms]", __func__, __LINE__, config->data_ptr, config->data_size, period_ms);
    mic_set_period(period_ms);
#ifdef USB_AUDIO_ADAPTER
    UsbaudioSetMicMumNormalMode(stream_cfg.channel_num);
#endif
    mcu_dsp_af_capture_start_pre_hook(&stream_cfg);
    app_mic_init();
    app_mic_register(MIC_APP_MICRECORD, &stream_cfg);
    app_mic_start(MIC_APP_MICRECORD);
}

static void mcu_dsp_af_playback_start(void *param)
{
    int ret;
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    struct TRANSQ_MSG_AF_CONFIG_T *config = &msg->msg.stream_cfg;
    struct AF_STREAM_CONFIG_T stream_cfg;

    TRACE(0, "%s, %d", __func__, __LINE__);

    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = config->bits;
    stream_cfg.channel_num = config->channel_num;
    stream_cfg.channel_map = config->channel_map;
    stream_cfg.chan_sep_buf = config->chan_sep_buf;
    stream_cfg.sample_rate = config->sample_rate;
    stream_cfg.device = config->device;
    stream_cfg.vol = config->vol;
    stream_cfg.handler = mcu_af_playback;
    stream_cfg.io_path = config->io_path;
    stream_cfg.data_ptr = config->data_ptr;
    stream_cfg.data_size = config->data_size;

    ret = af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
    ASSERT(ret == 0, "af_stream_open capture failed: %d", ret);

    ret = af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);
    ASSERT(ret == 0, "af_stream_start capture failed: %d", ret);
}

static void mcu_audio_record_indicate(uint8_t *buf, uint32_t len)
{
    //TRACE(0, "mcu_audio_record_indicate:%u", len);

    if (a7_to_m33_cb.func) {
        a7_to_m33_cb.func(buf, len, a7_to_m33_cb.param);
    }
}

static void mcu_dsp_record_data(void *param)
{
    TRANSQ_MSG_T *msg = (TRANSQ_MSG_T *)param;
    uint8_t *buf = msg->msg.stream_buf.buf;
    uint32_t len = msg->msg.stream_buf.len;

    mcu_audio_record_indicate(buf, len);
}

void a7_to_m33_aud_subscribe(aud_dump_cb_t cbf, void *param)
{
    a7_to_m33_cb.func = cbf;
    a7_to_m33_cb.param = param;
}

void a7_to_m33_aud_unsubscribe(aud_dump_cb_t cbf, void *param)
{
    if ((a7_to_m33_cb.func == cbf) && (a7_to_m33_cb.param == param)) {
        a7_to_m33_cb.func = NULL;
        a7_to_m33_cb.param = NULL;
    }
}

void aud_record_subscribe(aud_dump_cb_t cbf, void *param)
{
#ifdef _MULTI_RECORD_SUPPORT
    for (int i=0; i<_MULTI_RECORD_NUM; i++) {
        if ((!cb[i].func) && (!cb[i].param)) {
            cb[i].func = cbf;
            cb[i].param = param;
            printf("%s i=%d cb=%p param=%p.\r\n", __FUNCTION__, i, cb[i].func, cb[i].param);
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
            printf("%s i=%d cb=%p param=%p.\r\n", __FUNCTION__, i, cb[i].func, cb[i].param);
            cb[i].func = NULL;
            cb[i].param = NULL;
            break;
        }
    }
#endif
}

/*
 *  16 bit buf ,while transform to uint8_t return size need to multiply 2
 */
int record_data_get_mic1_mic2(uint16_t *record_in, uint32_t len_in, uint16_t *buf_out, uint16_t *buf_out2)
{
    int index = 0;
    /* 0-1-2 ref-mic1-mic2 */
    /* 0-1-2-3 mic2-mic3-ref1-ref2 */
    int mic1_index = 0;
#ifndef HW_AUD_LOOPBACK_MODE
    mic1_index = CHAN_NUM_CAPTURE - 2;
#endif
	int ref_index = 0;

    int mic2_index = mic1_index + 1;
    if ((record_in == NULL) || (buf_out == NULL) || (buf_out2 == NULL)) {
        return 0;
    }
    for (uint32_t i = 0; i < len_in; i += CHAN_NUM_CAPTURE)
    {
        // TRACE(0, "L buf[%d] <--> record[%d]",index,mic1_index + i);
		buf_out2[i] = record_in[ref_index + i];
        buf_out[index] = record_in[mic1_index + i];
        index++;
        // TRACE(0, "R buf[%d] <--> record[%d]",index,mic2_index + i);
        buf_out[index] = record_in[mic2_index + i];
        index++;
    }
    return index;
}

/*
    mcu_record_pre_handler_set
    - set a callback to handle audio record before sending it to ww engine
    this handler is used to transfer data format to satisfy the ww engine requirement
*/
void mcu_record_pre_handler_set(mcu_record_pre_handler handler)
{
    record_pre_handler = handler;
}

/* speech_record_config_get
    default config is sample rate 16K, record length 64ms
    you can overide this function to change this config
*/
void WEAK speech_record_config_get(uint32_t *sample_rate, uint32_t *record_period_ms)
{
    TRACE(0, "%s weak sample_rate:%d, record_period_ms:%d", __func__, *sample_rate, *record_period_ms);
    if (sample_rate)
        *sample_rate = 16000;

    if (record_period_ms)
        *record_period_ms = 64;

    TRACE(0, "%s weak set sample_rate:%d, record_period_ms:%d", __func__, *sample_rate, *record_period_ms);
}

#ifndef A7_ALLOC_TRANSQ_BUF
#ifdef DSP_RAM_SHARE_SIZE
#define A7_DSP_FLAGS_LEN DSP_RAM_SHARE_SIZE
#else
#define A7_DSP_FLAGS_LEN (0x1B000)
#endif
static SYNC_FLAGS_LOC unsigned char a7_dsp_flags[A7_DSP_FLAGS_LEN];
#endif

extern int a7_dsp_get_bootnum(void);
static void mcu_dsp_setup(void)
{
    A7_CMD_T a7_cmd;
    uint32_t sample_rate;
    uint32_t record_period_ms;

    /* set capture parameter */
    speech_record_config_get(&sample_rate, &record_period_ms);
    a7_cmd.type = A7_CMD_TYPE_HANDSHAKE;
    a7_cmd.p1 = sample_rate;
    a7_cmd.p2 = record_period_ms;
    mcu_cmd_send(&a7_cmd);
    printf("%s A7_CMD_TYPE_HANDSHAKE to a7: mic_samplerate %d, period_ms %d\n", __FUNCTION__, a7_cmd.p1, a7_cmd.p2);

    /* set dsp heap */
#ifndef A7_ALLOC_TRANSQ_BUF
    a7_cmd.type = A7_CMD_TYPE_HEAP_INIT;
    a7_cmd.p1 = a7_dsp_flags;
    a7_cmd.p2 = A7_DSP_FLAGS_LEN;
    mcu_cmd_send(&a7_cmd);
    printf("%s A7_CMD_TYPE_HEAPINIT to a7: buf 0x%X, len %d\n", __FUNCTION__, a7_dsp_flags, A7_DSP_FLAGS_LEN);
#endif
}

void WEAK mcu_audio_pre_handshake(unsigned char *buf, int len)
{
    TRACE(0, "%s STUB!", __func__);
}

osThreadId mcu_audio_thread_id = NULL;
void mcu_audio_main(void const*argument)
{
    mic_reset_handshake();
    transq_msg_onoff(1);
    osDelay(300); // wait a while for a7 transq ready
#ifndef A7_ALLOC_TRANSQ_BUF
    mcu_audio_pre_handshake(a7_dsp_flags, A7_DSP_FLAGS_LEN);
#endif
    do {
        mcu_dsp_setup();
        osDelay(500);
    } while (!mic_get_handshake_done());

    printf("%s exit\n", __FUNCTION__);
    mcu_audio_thread_id = NULL;
    osThreadExit();
}
osThreadDef(mcu_audio_main, osPriorityAboveNormal, 1, (8*1024), "mcu_audio_main");

void WEAK mcu_audio_task_init_hook(void)
{

}

void mcu_audio_task_init(void)
{
    mcu_audio_task_init_hook();

    printf("%s p_transq_msg=0x%p, boot_num=%d\n", __FUNCTION__, p_transq_msg, a7_dsp_get_bootnum());
    transq_msg_register(TRANSQ_MSG_TYPE_AF_OPEN, mcu_dsp_af_open, false); /* af_open in app_init */
    transq_msg_register(TRANSQ_MSG_TYPE_AF_CAPTURE_START, mcu_dsp_af_capture_start, false);
    transq_msg_register(TRANSQ_MSG_TYPE_AF_PLAYBACK_START, mcu_dsp_af_playback_start, false);
    transq_msg_register(TRANSQ_MSG_TYPE_RECORD_DATA, mcu_dsp_record_data, false);

    // create handshake task
    if (mcu_audio_thread_id) {
        osThreadTerminate(mcu_audio_thread_id);
        mcu_audio_thread_id = NULL;
    }
    mcu_audio_thread_id = osThreadCreate(osThread(mcu_audio_main), NULL);
    if (mcu_audio_thread_id == NULL)
    {
        TRACE(0, "%s %d fail to create mcu_audio_main task",__func__,__LINE__);
        return;
    }
    TRACE(0, "create mcu_audio_main task = 0x%X", (uint32_t)mcu_audio_thread_id);
}
#endif

