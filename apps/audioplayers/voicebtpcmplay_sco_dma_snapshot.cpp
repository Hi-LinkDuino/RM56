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
// Standard C Included Files
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "plat_types.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "cqueue.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_ring_merge.h"
#include "tgt_hardware.h"
#include "bt_sco_chain.h"
#include "iir_resample.h"
#include "hfp_api.h"
#include "audio_prompt_sbc.h"
#include "speech_utils.h"
#include "bt_sco_codec.h"
#include "bt_drv.h"

#if defined(SPEECH_TX_24BIT)
typedef int     TX_PCM_T;
#else
typedef short   TX_PCM_T;
#endif

#if defined(SPEECH_RX_24BIT)
typedef int     RX_PCM_T;
#else
typedef short   RX_PCM_T;
#endif

extern TX_PCM_T *aec_echo_buf;

// app_bt_stream.cpp::bt_sco_player(), used buffer size
#define APP_BT_STREAM_USE_BUF_SIZE      (1024*2)
#if defined(SCO_OPTIMIZE_FOR_RAM)
uint8_t *sco_overlay_ram_buf = NULL;
int sco_overlay_ram_buf_len = 0;
#endif

static bool resample_vqe_sco_needed_flag = false;
static bool resample_vqe_codec_needed_flag = false;
static int sco_sample_rate;
static int vqe_sample_rate;
static int codec_sample_rate;
static int sco_frame_length;
static int vqe_frame_length;
static int codec_frame_length;
static int16_t *resample_buf = NULL;
static IirResampleState *uplink_resample_codec2vqe_st = NULL;
static IirResampleState *downlink_resample_sco2vqe_st = NULL;
static IirResampleState *uplink_resample_vqe2sco_st = NULL;
static IirResampleState *downlink_resample_vqe2codec_st = NULL;
static IirResampleState *resample_echo_st = NULL;
static bool speech_inited = false;

#define VOICEBTPCM_TRACE(s,...)
//TRACE(s, ##__VA_ARGS__)

static bt_sco_codec_t *g_bt_sco_codec_ptr = NULL;

extern hfp_sco_codec_t bt_sco_codec_get_type(void);

/*
 * possible downlink resample routine
 * msbc: 16k -> 16k -> 16k (normal)
 *       16k -> 16k -> 48k (PASP)
 *       16k -> 8k -> 8k (fake msbc) // need extra buffer to hold downlink data
 *       16k -> 8k -> 48k (fake msbc + PASP)
 * cvsd: 8k -> 8k -> 8k (normal)
 *       8k -> 16k -> 16k (algo only support 16k)
 *       8k -> 8k -> 48k (PASP)
 *       8k -> 16k -> 48k (algo only support 16k + PASP)
 *
 * resample sco2vqe could be upsample or downsample
 * resample vqe2codec should only be upsample, downsample is wasting mips
 */
int process_downlink_bt_voice_frames(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len)
{
    // TRACE(3,"[%s] in_len = %d, out_len = %d", __FUNCTION__, in_len, out_len);

    if (speech_inited == false) {
        TRACE(1, "[%s] warnning: speech init not finished", __FUNCTION__);
        memset(out_buf, 0, out_len);
        return 0;
    }

#if !defined(SPEECH_TX_AEC_CODEC_REF)
#if defined(SPEECH_TX_AEC) || defined(SPEECH_TX_AEC2) || defined(SPEECH_TX_AEC3) || defined(SPEECH_TX_AEC2FLOAT) || defined(SPEECH_TX_THIRDPARTY)
    int ref_pcm_len = out_len / sizeof(RX_PCM_T);
    RX_PCM_T *ref_pcm_buf = (RX_PCM_T *)out_buf;

    if (resample_vqe_codec_needed_flag) {
        iir_resample_process(resample_echo_st, ref_pcm_buf, ref_pcm_buf, ref_pcm_len);
        ref_pcm_len = ref_pcm_len * vqe_sample_rate / codec_sample_rate;
    }

    for (int i = 0; i < ref_pcm_len; i++) {
#if defined(SPEECH_RX_24BIT) && !defined(SPEECH_TX_24BIT)
        aec_echo_buf[i] = (ref_pcm_buf[i] >> 8);
#else
        aec_echo_buf[i] = ref_pcm_buf[i];
#endif
    }
#endif
#endif // #if !defined(SPEECH_TX_AEC_CODEC_REF)

#if defined(SPEECH_RX_24BIT)
    out_len /= 2;
#endif

    int16_t *pcm_buf = (int16_t *)out_buf;
    int pcm_len = sco_frame_length;

    if (resample_vqe_sco_needed_flag && resample_buf != NULL) {
        pcm_buf = resample_buf;
    }

    if (g_bt_sco_codec_ptr) {
        g_bt_sco_codec_ptr->decoder(in_buf, in_len, (uint8_t *)pcm_buf, pcm_len * sizeof(int16_t));
    }

    if (resample_vqe_sco_needed_flag) {
        iir_resample_process(downlink_resample_sco2vqe_st, pcm_buf, (int16_t *)out_buf, pcm_len);
        pcm_buf = (int16_t *)out_buf;
        pcm_len = pcm_len * vqe_sample_rate / sco_sample_rate;
    }

#if defined(SPEECH_RX_24BIT)
    int32_t *buf32 = (int32_t *)out_buf;
    for (int i = pcm_len - 1; i >= 0; i--) {
        buf32[i] = ((int32_t)pcm_buf[i] << 8);
    }
#endif

    speech_rx_process(pcm_buf, &pcm_len);

    if (resample_vqe_codec_needed_flag) {
        ASSERT(codec_sample_rate >= vqe_sample_rate, "[%s] codec sample rate(%d) should be large than vqe sample rate(%d)", __FUNCTION__, codec_sample_rate, vqe_sample_rate);

        iir_resample_process(downlink_resample_vqe2codec_st, pcm_buf, pcm_buf, pcm_len);
    }

#if defined(SPEECH_RX_24BIT)
    out_len *= 2;
#endif

    return 0;
}

/*
 * possible uplink resample routine
 * msbc: 16k -> 16k -> 16k (normal)
 *       8k -> 8k -> 16k (fake msbc)
 *       48k -> 16k -> 16k (PSAP)
 *       48k -> 8k -> 16k (fake msbc + PSAP)
 * cvsd: 8k -> 8k -> 8k (normal)
 *       16k -> 16k -> 8k (algo only support 16k)
 *       48k -> 8k -> 8k (PSAP)
 *       48k -> 16k -> 8k (algo only support 16k + PSAP)
 */
int process_uplink_bt_voice_frames(uint8_t *in_buf, uint32_t in_len, uint8_t *ref_buf, uint32_t ref_len, uint8_t *out_buf, uint32_t out_len)
{
    //TRACE(3,"[%s] in_len = %d, out_len = %d", __FUNCTION__, in_len, out_len);

    if (speech_inited == false) {
        TRACE(1, "[%s] warnning: speech init not finished", __FUNCTION__);
        memset(out_buf, MSBC_MUTE_PATTERN, out_len);
        return 0;
    }

    TX_PCM_T *pcm_buf = (TX_PCM_T *)in_buf;
    int pcm_len = in_len / sizeof(TX_PCM_T);

    if (resample_vqe_codec_needed_flag) {
        ASSERT(codec_sample_rate >= vqe_sample_rate, "[%s] codec sample rate(%d) should be large than vqe sample rate(%d)", __FUNCTION__, codec_sample_rate, vqe_sample_rate);

        iir_resample_process(uplink_resample_codec2vqe_st, pcm_buf, pcm_buf, pcm_len);
        pcm_len = pcm_len * vqe_sample_rate / codec_sample_rate;
    }

#ifndef VAD_IN_BACKGROUND
#if defined(SPEECH_TX_AEC_CODEC_REF)
    ASSERT(pcm_len % (SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1) == 0, "[%s] pcm_len(%d) should be divided by %d", __FUNCTION__, pcm_len, SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1);
    // copy reference buffer
#if defined(SPEECH_TX_AEC) || defined(SPEECH_TX_AEC2) || defined(SPEECH_TX_AEC3) || defined(SPEECH_TX_AEC2FLOAT) || defined(SPEECH_TX_THIRDPARTY)
    for (int i = SPEECH_CODEC_CAPTURE_CHANNEL_NUM, j = 0; i < pcm_len; i += SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1, j++) {
        aec_echo_buf[j] = pcm_buf[i];
    }
#endif
    for (int i = 0, j = 0; i < pcm_len; i += SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1, j += SPEECH_CODEC_CAPTURE_CHANNEL_NUM) {
        for (int k = 0; k < SPEECH_CODEC_CAPTURE_CHANNEL_NUM; k++)
            pcm_buf[j + k] = pcm_buf[i + k];
    }
    pcm_len = pcm_len / (SPEECH_CODEC_CAPTURE_CHANNEL_NUM + 1) * SPEECH_CODEC_CAPTURE_CHANNEL_NUM;
#endif

    speech_tx_process(pcm_buf, aec_echo_buf, &pcm_len);

#if defined(SPEECH_TX_24BIT)
    int32_t *buf24 = (int32_t *)pcm_buf;
    int16_t *buf16 = (int16_t *)pcm_buf;
    for (int i = 0; i < pcm_len; i++)
        buf16[i] = (buf24[i] >> 8);
#endif
#endif

    if (resample_vqe_sco_needed_flag) {
        int16_t *uplink_buf = (int16_t *)pcm_buf;
        if (resample_buf != NULL) {
            uplink_buf = resample_buf;
        }

        iir_resample_process(uplink_resample_vqe2sco_st, pcm_buf, uplink_buf, pcm_len);
        pcm_buf = (TX_PCM_T *)uplink_buf;
        pcm_len = pcm_len * sco_sample_rate / vqe_sample_rate;
    }

    if (g_bt_sco_codec_ptr) {
        g_bt_sco_codec_ptr->encoder(out_buf, out_len, (uint8_t *)pcm_buf, pcm_len * sizeof(int16_t));
    }

    return 0;
}

void *voicebtpcm_get_ext_buff(int size)
{
    uint8_t *pBuff = NULL;
    if (size % 4)
    {
        size = size + (4 - size % 4);
    }
    app_audio_mempool_get_buff(&pBuff, size);
    VOICEBTPCM_TRACE(2,"[%s] len:%d", __func__, size);
    return (void*)pBuff;
}
// sco sample rate: encoder/decoder sample rate
// vqe sample rate: speech algorithm process sample rate
// codec sample rate: hardware sample rate
/*
 *                    downlink_resample_sco2vqe             downlink_resample_vqe2codec
 * bt ---> msbc/cvsd --------------------------> algorithm ----------------------------> codec
 *                     uplink_resample_vqe2sco               uplink_resample_codec2vqe
 * bt <--- msbc/cvsd <-------------------------- algorithm <---------------------------- codec
 */
int voicebtpcm_pcm_audio_init(int _sco_sample_rate, int _vqe_sample_rate, int _codec_sample_rate, int _capture_channel_num)
{
    uint8_t POSSIBLY_UNUSED *speech_buf = NULL;
    int POSSIBLY_UNUSED speech_len = 0;

    sco_sample_rate = _sco_sample_rate;
    vqe_sample_rate = _vqe_sample_rate;
    codec_sample_rate = _codec_sample_rate;

    sco_frame_length = SPEECH_FRAME_MS_TO_LEN(sco_sample_rate, SPEECH_SCO_FRAME_MS);
    vqe_frame_length = SPEECH_FRAME_MS_TO_LEN(vqe_sample_rate, SPEECH_SCO_FRAME_MS);
    codec_frame_length = SPEECH_FRAME_MS_TO_LEN(codec_sample_rate, SPEECH_SCO_FRAME_MS);

    TRACE(3, "[%s] SCO  : sample rate = %d, frame len = %d", __func__, sco_sample_rate, sco_frame_length);
    TRACE(3, "[%s] VQE  : sample rate = %d, frame len = %d", __func__, vqe_sample_rate, vqe_frame_length);
    TRACE(3, "[%s] CODEC: sample rate = %d, frame len = %d", __func__, codec_sample_rate, codec_frame_length);



    resample_vqe_sco_needed_flag = (sco_sample_rate == vqe_sample_rate) ? 0 : 1;
    resample_vqe_codec_needed_flag = (vqe_sample_rate == codec_sample_rate) ? 0 : 1;

    if (resample_vqe_sco_needed_flag && sco_sample_rate < vqe_sample_rate) {
        TRACE(1, "[%s] SCO <-- Resample --> VQE", __func__);
        resample_buf = (int16_t *)voicebtpcm_get_ext_buff(sizeof(int16_t) * sco_frame_length);
    }

#if defined(SCO_OPTIMIZE_FOR_RAM)
    sco_overlay_ram_buf_len = hal_overlay_get_text_free_size((enum HAL_OVERLAY_ID_T)APP_OVERLAY_HFP);
    sco_overlay_ram_buf = (uint8_t *)hal_overlay_get_text_free_addr((enum HAL_OVERLAY_ID_T)APP_OVERLAY_HFP);
#endif

    speech_len = app_audio_mempool_free_buff_size() - APP_BT_STREAM_USE_BUF_SIZE;
    speech_buf = (uint8_t *)voicebtpcm_get_ext_buff(speech_len);

    int tx_frame_ms = SPEECH_PROCESS_FRAME_MS;
    int rx_frame_ms = SPEECH_SCO_FRAME_MS;
    speech_init(vqe_sample_rate, vqe_sample_rate, tx_frame_ms, rx_frame_ms, SPEECH_SCO_FRAME_MS, speech_buf, speech_len);

    // NOTE: Some modules must be created after speech init if they use speech heap

    if (resample_vqe_sco_needed_flag) {
        uplink_resample_vqe2sco_st = iir_resample_init(vqe_frame_length, 16, iir_resample_choose_mode(vqe_sample_rate, sco_sample_rate));
        downlink_resample_sco2vqe_st = iir_resample_init(sco_frame_length, 16, iir_resample_choose_mode(sco_sample_rate, vqe_sample_rate));
    }

    if (resample_vqe_codec_needed_flag) {
        uplink_resample_codec2vqe_st = multi_iir_resample_init(codec_frame_length * _capture_channel_num, sizeof(TX_PCM_T) * 8,  _capture_channel_num, iir_resample_choose_mode(codec_sample_rate, vqe_sample_rate));
        downlink_resample_vqe2codec_st = iir_resample_init(vqe_frame_length, sizeof(RX_PCM_T) * 8, iir_resample_choose_mode(vqe_sample_rate, codec_sample_rate));

        resample_echo_st = iir_resample_init(codec_frame_length, sizeof(RX_PCM_T) * 8, iir_resample_choose_mode(codec_sample_rate, vqe_sample_rate));
    }

    if (g_bt_sco_codec_ptr == NULL) {
        hfp_sco_codec_t sco_codec_type = bt_sco_codec_get_type();
        if (sco_codec_type == BTIF_HF_SCO_CODEC_CVSD) {
            g_bt_sco_codec_ptr = &bt_sco_codec_cvsd;
        } else if (sco_codec_type == BTIF_HF_SCO_CODEC_MSBC) {
            g_bt_sco_codec_ptr = &bt_sco_codec_msbc;
#if defined(SCO_ADD_CUSTOMER_CODEC)
        } else if (sco_codec_type == BTIF_HF_SCO_CODEC_XXXX) {
            g_bt_sco_codec_ptr = &bt_sco_codec_xxxx;
#endif
        } else {
            ASSERT(0, "[%s] %x is invalid sco codec type!", __func__, sco_codec_type);
        }

        g_bt_sco_codec_ptr->init(sco_sample_rate);
    } else {
        ASSERT(0, "[%s] g_bt_sco_codec_ptr != NULL", __func__);
    }

    speech_inited = true;

    return 0;
}

int voicebtpcm_pcm_audio_deinit(void)
{
    TRACE(1,"[%s] Close...", __func__);
    // TRACE(2,"[%s] app audio buffer free = %d", __func__, app_audio_mempool_free_buff_size());

    g_bt_sco_codec_ptr->deinit();
    g_bt_sco_codec_ptr = NULL;

    if (resample_vqe_sco_needed_flag) {
        iir_resample_destroy(uplink_resample_vqe2sco_st);
        iir_resample_destroy(downlink_resample_sco2vqe_st);
    }

    if (resample_vqe_codec_needed_flag) {
        iir_resample_destroy(uplink_resample_codec2vqe_st);
        iir_resample_destroy(downlink_resample_vqe2codec_st);

        iir_resample_destroy(resample_echo_st);
    }

    resample_buf = NULL;

    speech_deinit();

#if defined(SCO_OPTIMIZE_FOR_RAM)
    sco_overlay_ram_buf = NULL;
    sco_overlay_ram_buf_len = 0;
#endif

    speech_inited = false;

    // TRACE(1,"Free buf = %d", app_audio_mempool_free_buff_size());

    return 0;
}
