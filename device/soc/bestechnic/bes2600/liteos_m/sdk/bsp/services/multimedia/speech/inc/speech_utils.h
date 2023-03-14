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
#ifndef __SPEECH_UTILS_H__
#define __SPEECH_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

// len = fs / 1000 * ms
#if defined(LOW_DELAY_SCO)
// len = fs / 1000 * ms
#define SPEECH_FRAME_MS_TO_LEN(fs, ms)      (( (fs) / 1000 * (ms) )/2)
#else
// len = fs / 1000 * ms
#define SPEECH_FRAME_MS_TO_LEN(fs, ms)      ( (fs) / 1000 * (ms) )
#endif
#define SPEECH_FRAME_LEN_TO_MS(fs, len)     ( (len) / ( (fs) / 1000) )


static inline void speech_set_uint8(unsigned char *dst, unsigned char v, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = v;
    }
}

static inline void speech_set_int16(short *dst, short v, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = v;
    }
}

static inline void speech_set_int32(int *dst, int v, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = v;
    }
}

static inline void speech_set_f32(float *dst, float v, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = v;
    }
}

static inline void speech_copy_uint8(unsigned char *dst, unsigned char *src, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = src[i];
    }
}

static inline void speech_copy_int16(short *dst, short *src, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = src[i];
    }
}

static inline void speech_copy_int32(int *dst, int *src, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = src[i];
    }
}

static inline void speech_copy_f32(float *dst, float *src, int len)
{
    for (int i=0; i<len; i++) {
        dst[i] = src[i];
    }
}

static inline float speech_frame_size2time_ms(int sample_rate, int frame_size)
{
    float frame_time;

    frame_time = (float)frame_size / (sample_rate / 1000);

    return frame_time;
}

#ifdef __cplusplus
}
#endif

#endif
