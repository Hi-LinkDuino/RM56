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

/* need close macro OVERRIDE_OPUS_ALLOC_SCRATCH in opus_config.h */

#include "opus.h"
#include "hal_trace.h"
#include "bap_opus.h"
#include "stdlib.h"
#include "string.h"

static OpusEncoder *opus_encoder[4] = {NULL};
static OpusEncoder *opus_decoder = NULL; //4 for 4 channel to encode

static int opus_encoder_inited = 0;
static int opus_decoder_inited = 0;
#if (__WIFI_USB_AUDIO_CH__ == 6)
#define OPUS_MULTI_ENC_NUM      4
#define OPUS_SAMPLE_RATE        48000
#define OPUS_SAMPLE_SIZE        2//24bit 1ch
#define OPUS_CODEC_CH_NUM       1

#define OPUS_BITRATE_1      768000
#define OPUS_BITRATE_4      192000
#define OPUS_BITRATE_8      96000
#define OPUS_BITRATE_16     48000

#define OPUS_COMPARESS_RATION_4    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_4)
#define OPUS_COMPARESS_RATION_8    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_8)
#define OPUS_COMPARESS_RATION_16   (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_16)
#define OPUS_CODEC_LEN (1920) //i2s 48k24bit*2ch*20ms = 7680, usb 48k*16bit*2ch*20ms=3840

#define APP_OPUS_USE_BITRATE            OPUS_BITRATE_8
#define APP_OPUS_USE_COMPRESS_RATION    OPUS_COMPARESS_RATION_8

#elif __VOW_USE_48K_24BIT_CODEC__
#define OPUS_MULTI_ENC_NUM      1
#define OPUS_SAMPLE_RATE        48000
#define OPUS_SAMPLE_SIZE        4//24bit 1ch
#define OPUS_CODEC_CH_NUM       2

#define OPUS_BITRATE_1          1536000
#define OPUS_BITRATE_4          384000
#define OPUS_BITRATE_8          192000
#define OPUS_BITRATE_16         96000

#define OPUS_COMPARESS_RATION_4    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_4)
#define OPUS_COMPARESS_RATION_8    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_8)
#define OPUS_COMPARESS_RATION_16   (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_16)

#define OPUS_CODEC_LEN (7680) //i2s 48k24bit*2ch*20ms = 7680, usb 48k*16bit*2ch*20ms=3840
#define APP_OPUS_USE_BITRATE            OPUS_BITRATE_8
#define APP_OPUS_USE_COMPRESS_RATION    OPUS_COMPARESS_RATION_8

#elif (__WIFI_USB_AUDIO_CH__ == 1)
#define OPUS_MULTI_ENC_NUM      1
#define OPUS_SAMPLE_RATE        48000
#define OPUS_SAMPLE_SIZE        2//24bit 1ch
#define OPUS_CODEC_CH_NUM       1

#define OPUS_BITRATE_1      768000
#define OPUS_BITRATE_4      192000
#define OPUS_BITRATE_8      96000
#define OPUS_BITRATE_16     48000

#define OPUS_COMPARESS_RATION_4    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_4)
#define OPUS_COMPARESS_RATION_8    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_8)
#define OPUS_COMPARESS_RATION_16   (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_16)
#define OPUS_CODEC_LEN (1920) //i2s 48k24bit*2ch*20ms = 7680, usb 48k*16bit*2ch*20ms=3840

#define APP_OPUS_USE_BITRATE            OPUS_BITRATE_8
#define APP_OPUS_USE_COMPRESS_RATION    OPUS_COMPARESS_RATION_8

#else
#define OPUS_MULTI_ENC_NUM      1
#define OPUS_SAMPLE_RATE        48000
#define OPUS_SAMPLE_SIZE        2//24bit 1ch
#define OPUS_CODEC_CH_NUM       2

#define OPUS_BITRATE_4          384000
#define OPUS_BITRATE_8          192000
#define OPUS_BITRATE_16         96000
#define OPUS_BITRATE_32         48000

#define OPUS_COMPARESS_RATION_4    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_4)
#define OPUS_COMPARESS_RATION_8    (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_8)
#define OPUS_COMPARESS_RATION_16   (OPUS_SAMPLE_RATE * OPUS_SAMPLE_SIZE *OPUS_CODEC_CH_NUM *8/ OPUS_BITRATE_16)
#define OPUS_CODEC_LEN (3840) //i2s 48k24bit*2ch*20ms = 7680, usb 48k*16bit*2ch*20ms=3840

#define APP_OPUS_USE_BITRATE            OPUS_BITRATE_8
#define APP_OPUS_USE_COMPRESS_RATION    OPUS_COMPARESS_RATION_8

#endif

static int g_SampleRate = OPUS_SAMPLE_RATE;
static int g_ch = OPUS_CODEC_CH_NUM;

typedef struct
{
    uint32_t sample_rate;
    uint32_t max_data_byte;
    uint32_t app_opus_bitrate;
    uint32_t  opus_codec_type;
    uint32_t  channels_num;
    uint32_t  sample_size;
} audio_opus_codec_t;

static audio_opus_codec_t opus_codec_arg = {
    OPUS_SAMPLE_RATE,
    OPUS_CODEC_LEN / APP_OPUS_USE_COMPRESS_RATION,//maxDataByte = payload len / compress ration;
    APP_OPUS_USE_BITRATE,
    OPUS_APPLICATION_AUDIO,
    OPUS_CODEC_CH_NUM,
    OPUS_SAMPLE_SIZE
};

void bap_log_opus_codec_arg(audio_opus_codec_t *arg)
{
    TRACE(0, "opus codec arg\n");
    TRACE(0, "sample_rate = %d\n", arg->sample_rate);
    TRACE(0, "max_data_byte = %d\n", arg->max_data_byte);
    TRACE(0, "app_opus_bitrate = %d\n", arg->app_opus_bitrate);
    TRACE(0, "opus_codec_type = %d\n", arg->opus_codec_type);
    TRACE(0, "channels_num = %d\n", arg->channels_num);
    TRACE(0, "sample_size = %d\n", arg->sample_size);

    if (arg->opus_codec_type == OPUS_APPLICATION_RESTRICTED_LOWDELAY)
    {
        TRACE(0, "opus_codec_type is OPUS_ENC_APP_RESTRICTED_LOWDELAY\n");
    }
    else if(arg->opus_codec_type == OPUS_APPLICATION_AUDIO)
    {
        TRACE(0, "opus_codec_type is OPUS_APPLICATION_AUDIO\n");
    }
    else if(arg->opus_codec_type == OPUS_APPLICATION_VOIP)
    {
        TRACE(0, "opus_codec_type is OPUS_APPLICATION_VOIP\n");
    }
    else
    {
        TRACE(0, "error:the opus app applicant type is unknow\n");
    }

}

int bap_audio_opus_enc_init(uint32_t sample_rate, uint8_t channels_num)
{
    int i = 0;
    if (opus_encoder_inited)
    {
        return 0;
    }
    TRACE(0, "%s\n", __FUNCTION__);

    opus_codec_arg.sample_rate = sample_rate;
    opus_codec_arg.channels_num = channels_num;

    bap_log_opus_codec_arg(&opus_codec_arg);

    int error = 0 ,size= 0;
    size = opus_encoder_get_size(opus_codec_arg.channels_num);


    for(i =0; i < OPUS_MULTI_ENC_NUM; i++){
        opus_encoder[i] = (OpusEncoder *)malloc(size);
        if (opus_encoder[i] != NULL){
            error = opus_encoder_init(opus_encoder[i], opus_codec_arg.sample_rate, opus_codec_arg.channels_num , opus_codec_arg.opus_codec_type);
            opus_encoder_ctl(opus_encoder[i], OPUS_SET_BITRATE(opus_codec_arg.app_opus_bitrate));
            TRACE(0, "####ENCODEC ID[%d] =%p", i, opus_encoder[i]);
        }else{
            TRACE(0, "opus enc init malloc mem fail");
            return -1;
        }
    }
    opus_encoder_inited = 1;

    return error;
}

static void bap_convert_24bit_to_16bit(int16_t *dst, int32_t *src, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        dst[i] = src[i] >> 8;
    }
}

static void bap_convert_24bit_to_float(float *dst, int32_t *src, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        dst[i] = src[i] / 8388608.f;
    }
}

static void bap_convert_16bit_to_24bit(int32_t *dst, int16_t *src, uint32_t count)
{
    for (int32_t i = count - 1; i >= 0; i--) {
        dst[i] = src[i] << 8;
    }
}

static void bap_convert_float_to_24bit(int32_t *dst, float *src, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        dst[i] = (int32_t)(src[i] * 8388607.f);
    }
}

int bap_audio_opus_enc_write(OPUS_ENC_ID_T encode_id , int32_t *data_in, uint32_t frame_size, uint8_t *data_out, int32_t *out_len)
{
    int encode_len = 0;
    frame_size = frame_size / (OPUS_SAMPLE_SIZE * OPUS_CODEC_CH_NUM);

    if (!opus_encoder_inited)
    {
        return -1;
    }

    //TRACE(0, "###OPUS_ENC_ID_T ID =%d", encode_id);
    if (data_in == NULL || data_out == NULL)
    {
        TRACE(0, "%s: arg err.\n", __func__);
        return -1;
    }
    //TRACE(0, "%s:line =%d encode_len =%d frame_size =%d\n", __func__ ,__LINE__,encode_len,frame_size);

#ifdef OPUS_FLOAT
    bap_convert_24bit_to_float((float *)data_in, data_in, frame_size * opus_codec_arg.channels_num);
    encode_len = opus_encode_float(opus_encoder[encode_id], (float *)data_in, frame_size, data_out, opus_codec_arg.max_data_byte);
#else

#ifdef __VOW_USE_48K_24BIT_CODEC__
    bap_convert_24bit_to_16bit((int16_t *)data_in, data_in, frame_size * opus_codec_arg.channels_num);
#endif
    encode_len = opus_encode(opus_encoder[encode_id], (int16_t *)data_in, frame_size, data_out, opus_codec_arg.max_data_byte);
#endif

    if (encode_len < 0)
    {
        TRACE(0, "opus encode error %d.\n", encode_len);
        return -1;
    }
    //TRACE(0, "%s:line =%d encode_len =%d frame_size =%d", __func__ ,__LINE__,encode_len,frame_size);

    *out_len = encode_len;

    return 0;
}

int bap_audio_opus_enc_close(void)
{
    int i = 0;
    {
        for(i = 0; i< OPUS_MULTI_ENC_NUM; i++){
            opus_encoder_destroy(opus_encoder[i]);
            free(opus_encoder[i]);
            opus_encoder[i] = NULL;
            opus_encoder_inited = 0;
        }
    }
    return 0;
}



int bap_audio_opus_dec_init(uint32_t sample_rate, uint8_t channels_num)
{
    if (opus_decoder_inited)
    {
        TRACE(0, "%s: arg err.\n", __func__);
        return 0;
    }

    opus_codec_arg.sample_rate = sample_rate;
    opus_codec_arg.channels_num = channels_num;

    bap_log_opus_codec_arg(&opus_codec_arg);

    int error = 0;
    uint32_t size = opus_decoder_get_size(opus_codec_arg.channels_num);
    opus_decoder = malloc(size);
    error = opus_decoder_init(opus_decoder, opus_codec_arg.sample_rate, opus_codec_arg.channels_num);

    if (opus_decoder)
    {
        opus_decoder_inited = 1;
        TRACE(1,"####a7 init opus dec finish success\n");
        return 0;
    }
    else
    {
        TRACE(0, "opus encoder init error. %d\n", error);
        return -1;
    }

    return 0;
}


int bap_audio_opus_dec_write(uint8_t *data_in, int input_data_len, int32_t *data_out,int *outlen)
{
    int per_ch_sample_space = OPUS_CODEC_LEN / (OPUS_SAMPLE_SIZE * OPUS_CODEC_CH_NUM);

    if (!opus_decoder_inited)
    {
        TRACE(0, "%s: arg err.\n", __func__);
        return -1;
    }

    if (data_out == NULL)
    {
        TRACE(0, "%s: arg err.\n", __func__);
        return -1;
    }

    int decode_len = 0;

#ifdef OPUS_FLOAT
    decode_len = opus_decode_float(opus_decoder, data_in, input_data_len, (float *)data_out ,per_ch_sample_space, 0);
    bap_convert_float_to_24bit(data_out, (float *)data_out, per_ch_sample_space * opus_codec_arg.channels_num);
#else

    if( data_in )
        decode_len = opus_decode(opus_decoder, data_in, input_data_len, (int16_t *)data_out ,per_ch_sample_space, 0);
    else
    {
        TRACE(0, " #######a 7 opus process audio  dp ....");
        decode_len = opus_decode(opus_decoder, NULL, 0, (int16_t *)data_out ,per_ch_sample_space, 0);
    }

#ifdef __VOW_USE_48K_24BIT_CODEC__
    bap_convert_16bit_to_24bit(data_out, (int16_t *)data_out, per_ch_sample_space * opus_codec_arg.channels_num);
#endif

#endif

    if (decode_len < 0)
    {
        TRACE(0, "opus decode error %d.\n", decode_len);
        return -1;
    }
    *outlen = decode_len * (OPUS_SAMPLE_SIZE * OPUS_CODEC_CH_NUM);
    //TRACE(0, "%s:line =%d decode_len =%d frame_size =%d\n", __func__ ,__LINE__,decode_len,frame_size);

    return 0;
}

int bap_audio_opus_dec_close(void)
{
    if (opus_decoder)
    {
        opus_decoder_destroy(opus_decoder);
        free(opus_decoder);
        opus_decoder = NULL;
        opus_decoder_inited = 0;
    }
    return 0;
}

int bap_audio_opus_config()
{
    /* reinit opus codec */
    bap_audio_opus_enc_init(g_SampleRate, g_ch);
    bap_audio_opus_dec_init(g_SampleRate, g_ch);
    return 0;
}

