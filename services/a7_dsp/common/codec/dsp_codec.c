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
#include "bap_lc3.h"
#include "bap_opus.h"
#include "stdlib.h"
#include "string.h"

#define DSP_CODEC_OUTBUF_LEN    0x2000
uint8_t *dsp_codec_outbuf = NULL;

int dsp_codec_dummy(void * input, int inlen, void * output, uint32_t *outlen)
{
    if (input && inlen && output && outlen) {
        memcpy(output, input, inlen);
        *outlen = inlen;
    }
    return 0;
}

int dsp_codec_process(DSP_CODEC_MSG_T* msg, uint8_t **outbuf, uint32_t *outlen)
{
    int ret = 0;

    *outbuf = NULL;
    *outlen = 0;

    if (!msg)
        return -1;

    switch (msg->cmd) {
        case DSP_CODEC_CMD_LC3_CFG:
            ret = bap_lc3_config(msg->data, msg->len);
            break;
        case DSP_CODEC_CMD_LC3_ENC:
            ret = bap_lc3_encode(msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_LC3_DEC:
            ret = bap_lc3_decode(msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
#ifdef OPUS_CODEC_SUPPORT
        case DSP_CODEC_CMD_OPUS_CFG:
            ret = bap_audio_opus_config(msg->data, msg->len);
            break;
        case DSP_CODEC_CMD_OPUS_ENC:
            ret = bap_audio_opus_enc_write(DSP_CODEC_OPUS_ENC0_ID, msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_OPUS_ENC1:
            ret = bap_audio_opus_enc_write(DSP_CODEC_OPUS_ENC1_ID, msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_OPUS_ENC2:
            ret = bap_audio_opus_enc_write(DSP_CODEC_OPUS_ENC2_ID, msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_OPUS_ENC3:
            ret = bap_audio_opus_enc_write(DSP_CODEC_OPUS_ENC3_ID, msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_OPUS_DEC:
            ret = bap_audio_opus_dec_write(msg->data, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
        case DSP_CODEC_CMD_OPUS_AUDIO_DROP:
            ret = bap_audio_opus_dec_write(NULL, msg->len, dsp_codec_outbuf, (int *)outlen);
            if (ret == 0)
                *outbuf = dsp_codec_outbuf;
            break;
#endif
        default:
            ret = dsp_codec_dummy(msg->data, msg->len, dsp_codec_outbuf, outlen);
            break;
    }
    return ret;
}

int dsp_codec_init(void)
{
    if (bap_lc3_encoder_init() != 0)
        return -1;
    if (bap_lc3_decoder_init() != 0)
        return -1;
#ifdef OPUS_CODEC_SUPPORT
    bap_audio_opus_config();
#endif
    dsp_codec_outbuf = malloc(DSP_CODEC_OUTBUF_LEN);
    if (!dsp_codec_outbuf)
        return -1;
    return 0;
}