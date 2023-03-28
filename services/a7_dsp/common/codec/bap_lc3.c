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
#include "hal_trace.h"
#include "hal_location.h"
#include "hal_aud.h"
#include "transq_msg.h"
#include "dsp_codec.h"
#include "lc3_process.h"
#include "bap_lc3.h"
#include "stdlib.h"
#include "string.h"

#ifdef  LC3_AUDIO_MIXED_MONO
#define BIS_AUDIO_CHANNEL_NUM               AUD_CHANNEL_NUM_1
#else
#define BIS_AUDIO_CHANNEL_NUM               AUD_CHANNEL_NUM_2
#endif

#define BIS_SAMPLE_RATE                     AUD_SAMPRATE_48000
#define BIS_LC3_BIT_RATE                    192000
#define BIS_LC3_BIT                         AUD_BITS_16
#define BIS_LC3_FRAME_MS                    10

#define BIS_LC3_ENCODE_PCM_LEN_PER_FRAME    480*2*BIS_AUDIO_CHANNEL_NUM //480 samples

#define BAP_LC3_MEM_POOL
#ifdef BAP_LC3_MEM_POOL
static XRAM_BSS_LOC uint32_t lc3_dec_pool[6250];   //24576
static XRAM_BSS_LOC uint32_t lc3_dec_sct[256];     //0
static XRAM_BSS_LOC uint32_t lc3_enc_pool[6250];   //23552
static XRAM_BSS_LOC uint32_t lc3_enc_sct[256];     //0
#endif

//bis_config_para_t bis_config_parameter = {BIS_LC3_BIT_RATE,240,10000,BIS_SEND_FRAME_NUM,BIS_AUDIO_PLAY_INTERVAL_US};
uint32_t  bap_lc3_bitrates = BIS_LC3_BIT_RATE;
LC3_Dec_Info bap_lc3_dec_info = {0};
LC3_Enc_Info bap_lc3_enc_info = {0};

int bap_lc3_encode(void * input, int inlen, void * output, int *outlen)
{
    LC3_API_Error err = LC3_API_OK;
    uint8_t *tmp = (uint8_t *)input;
    int16_t *in[BIS_AUDIO_CHANNEL_NUM];

    if (inlen != BIS_LC3_ENCODE_PCM_LEN_PER_FRAME) {
        TRACE(0, "%s invalid frame len",__func__);
        return -1;
    }

    for (int i = 0; i < BIS_AUDIO_CHANNEL_NUM; i++) {
        in[i] = (int16_t *)&tmp[BIS_LC3_ENCODE_PCM_LEN_PER_FRAME/BIS_AUDIO_CHANNEL_NUM*i];
    }

    err = lc3_api_enc16(bap_lc3_enc_info.encode, bap_lc3_enc_info.scratch,
                        in, output, outlen);
    if (err != LC3_API_OK) {
        TRACE(0, "%s err %d",__func__, err);
        *outlen = 0;
        return -1;
    }
    return 0;
}

int bap_lc3_decode(void * input, int inlen, void * output, int *outlen)
{
    int32_t bfi_ext = 0;
    LC3_API_Error err = LC3_API_OK;
    uint8_t *tmp = (uint8_t *)output;
    int16_t *out[BIS_AUDIO_CHANNEL_NUM];

    for (int i = 0; i < BIS_AUDIO_CHANNEL_NUM; i++) {
        out[i] = (int16_t *)&tmp[BIS_LC3_ENCODE_PCM_LEN_PER_FRAME/BIS_AUDIO_CHANNEL_NUM*i];
    }

    err = lc3_api_dec16(bap_lc3_dec_info.decode, bap_lc3_dec_info.scratch,
                        input, inlen, out, bfi_ext);
    if (err == LC3_API_OK) {
        *outlen = BIS_LC3_ENCODE_PCM_LEN_PER_FRAME;
        return 0;
    } else {
        TRACE(0, "%s err %d",__func__, err);
        *outlen = 0;
        return -1;
    }
}

int bap_lc3_encoder_init(void)
{
    LC3_API_Error err = LC3_API_OK;

    TRACE(0, "%s enter",__func__);

#ifndef BAP_LC3_MEM_POOL
    /* free last context */
    if (bap_lc3_enc_info.encode)
        free(bap_lc3_enc_info.encode);
    if (bap_lc3_enc_info.scratch)
        free(bap_lc3_enc_info.scratch);
#endif

    /* Setup Encoder */
    memset((void*)&bap_lc3_enc_info, 0, sizeof(bap_lc3_enc_info));
    bap_lc3_enc_info.samplerate = BIS_SAMPLE_RATE;
    bap_lc3_enc_info.channels   = BIS_AUDIO_CHANNEL_NUM;
    bap_lc3_enc_info.bitwidth   = BIS_LC3_BIT;
    bap_lc3_enc_info.frame_ms   = BIS_LC3_FRAME_MS;
    bap_lc3_enc_info.bitrate    = bap_lc3_bitrates;
    bap_lc3_enc_info.plcMeth    = LC3_API_PLC_ADVANCED;
    bap_lc3_enc_info.epmode     = LC3_API_EP_OFF;
    bap_lc3_enc_info.dc         = 1;

    bap_lc3_enc_info.encode_size = lc3_api_enc_get_size(bap_lc3_enc_info.samplerate, bap_lc3_enc_info.channels,bap_lc3_enc_info.bitwidth);
#ifndef BAP_LC3_MEM_POOL
    bap_lc3_enc_info.encode = malloc(bap_lc3_enc_info.encode_size);
    if (bap_lc3_enc_info.encode == NULL)
        return -1;
#else
    ASSERT(bap_lc3_enc_info.encode_size <= sizeof(lc3_enc_pool), "%s lc3 enc size %d", __func__, bap_lc3_enc_info.encode_size);
    bap_lc3_enc_info.encode = (void *)lc3_enc_pool;
#endif
    memset((void*)bap_lc3_enc_info.encode, 0, bap_lc3_enc_info.encode_size);
    err = lc3_api_encoder_init(&bap_lc3_enc_info);
    ASSERT(LC3_API_OK == err, "%s err %d", __func__, err);
    bap_lc3_enc_info.scratch_size = lc3_api_enc_get_scratch_size(bap_lc3_enc_info.encode);
#ifndef BAP_LC3_MEM_POOL
    bap_lc3_enc_info.scratch = malloc(bap_lc3_enc_info.scratch_size);
    if (bap_lc3_enc_info.scratch == NULL) {
        free(bap_lc3_enc_info.encode);
        return -1;
    }
#else
    ASSERT(bap_lc3_enc_info.scratch_size <= sizeof(lc3_enc_sct), "%s lc3 enc sct %d", __func__, bap_lc3_enc_info.scratch_size);
    bap_lc3_enc_info.scratch = (void *) lc3_enc_sct;
#endif
    memset((void*)bap_lc3_enc_info.scratch, 0, bap_lc3_enc_info.scratch_size);
    return 0;
}

int bap_lc3_decoder_init(void)
{
    LC3_API_Error err = LC3_API_OK;

    TRACE(0, "%s enter",__func__);

#ifndef BAP_LC3_MEM_POOL
    /* free last context */
    if (bap_lc3_dec_info.decode)
        free(bap_lc3_dec_info.decode);
    if (bap_lc3_dec_info.scratch)
        free(bap_lc3_dec_info.scratch);
#endif

    memset((void*)&bap_lc3_dec_info, 0, sizeof(LC3_Dec_Info));
    bap_lc3_dec_info.samplerate = BIS_SAMPLE_RATE;
    bap_lc3_dec_info.channels   = BIS_AUDIO_CHANNEL_NUM;
    bap_lc3_dec_info.bitwidth   = BIS_LC3_BIT;
    bap_lc3_dec_info.frame_ms   = BIS_LC3_FRAME_MS;
    bap_lc3_dec_info.bitrate    = bap_lc3_bitrates;
    bap_lc3_dec_info.plcMeth    = LC3_API_PLC_ADVANCED;
    bap_lc3_dec_info.epmode     = LC3_API_EP_OFF;
    bap_lc3_dec_info.dc         = 1;

    bap_lc3_dec_info.decode_size = lc3_api_dec_get_size(bap_lc3_dec_info.samplerate, bap_lc3_dec_info.channels, bap_lc3_dec_info.plcMeth);
#ifndef BAP_LC3_MEM_POOL
    bap_lc3_dec_info.decode = malloc(bap_lc3_dec_info.decode_size);
    if (bap_lc3_dec_info.decode == NULL)
        return -1;
#else
    ASSERT(bap_lc3_dec_info.decode_size <= sizeof(lc3_dec_pool), "%s lc3 dec size %d", __func__, bap_lc3_dec_info.decode_size);
    bap_lc3_dec_info.decode = (void *)lc3_dec_pool;
#endif

    memset((void*)bap_lc3_dec_info.decode, 0, bap_lc3_dec_info.decode_size);
    err = lc3_api_decoder_init(&bap_lc3_dec_info);
    ASSERT(LC3_API_OK == err, "%s err %d", __func__, err);
    bap_lc3_dec_info.scratch_size = lc3_api_dec_get_scratch_size(bap_lc3_dec_info.decode);
#ifndef BAP_LC3_MEM_POOL
    bap_lc3_dec_info.scratch = malloc(bap_lc3_dec_info.scratch_size);
    if (bap_lc3_dec_info.scratch == NULL) {
        free(bap_lc3_dec_info.decode);
        return -1;
    }
#else
    ASSERT(bap_lc3_dec_info.scratch_size <= sizeof(lc3_dec_sct), "%s lc3 dec sct %d", __func__, bap_lc3_dec_info.scratch_size);
    bap_lc3_dec_info.scratch = (void *)lc3_dec_sct;
#endif
    memset((void*)bap_lc3_dec_info.scratch, 0, bap_lc3_dec_info.scratch_size);
    return 0;
}

int bap_lc3_config(void * input, int len)
{
    if (len < sizeof(bap_lc3_bitrates))
        return -1;
    bap_lc3_bitrates = *((uint32_t *)input);

    /* reinit lc3 codec */
    bap_lc3_encoder_init();
    bap_lc3_decoder_init();

    return 0;
}