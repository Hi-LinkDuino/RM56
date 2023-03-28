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
#ifndef IIRFILT_H
#define IIRFILT_H

#include <stdint.h>

enum IIR_BIQUARD_TYPE
{
    // pass through
    IIR_BIQUARD_PASS = 0,
    // raw filter
    IIR_BIQUARD_RAW,
    // low pass filter
    IIR_BIQUARD_LPF,
    // high pass filter
    IIR_BIQUARD_HPF,
    // band pass filter, constant skirt gain, peak gain = Q
    IIR_BIQUARD_BPF0,
    // band pass filter, const 0 dB peak gain
    IIR_BIQUARD_BPF1,
    // notch filter
    IIR_BIQUARD_NOTCH,
    // allpass filter
    IIR_BIQUARD_APF,
    // peakingEQ
    IIR_BIQUARD_PEAKINGEQ,
    // low shelf filter
    IIR_BIQUARD_LOWSHELF,
    // high shelf filter
    IIR_BIQUARD_HIGHSHELF,
    IIR_BIQUARD_QTY
};

struct IirBiquardState
{
    float a1, a2, b0, b1, b2;
    float s0, s1, s2;
};

typedef struct IirBiquardState IirBiquardState;

#ifdef __cplusplus
extern "C" {
#endif

void iirfilt_design(IirBiquardState *st, int fs, int f0, float gain, float q, enum IIR_BIQUARD_TYPE type);

void iirfilt_raw(IirBiquardState *st, float b0, float b1, float b2, float a1, float a2);

void iirfilt_reset(IirBiquardState *st, int stages);

void iirfilt_process(IirBiquardState *st, int stages, int16_t *buf, int frame_size);

void iirfilt_process_int24(IirBiquardState *st, int stages, int32_t *buf, int frame_size);

void iirfilt_process_float(IirBiquardState *st, int stages, float *buf, int frame_size);

/* Deal with master gain in iir */
void iirfilt_process2(IirBiquardState *st, int stages, float master_gain, int16_t *buf, int frame_size);

void iirfilt_process2_int24(IirBiquardState *st, int stages, float master_gain, int32_t *buf, int frame_size);

void iirfilt_process2_float(IirBiquardState *st, int stages, float master_gain, float *buf, int frame_size);

void iirfilt_process3(IirBiquardState *st, int stages, float master_gain, int16_t *buf, int frame_size, int stride);

void iirfilt_process3_int24(IirBiquardState *st, int stages, float master_gain, int32_t *buf, int frame_size, int stride);

void iirfilt_process3_float(IirBiquardState *st, int stages, float master_gain, float *buf, int frame_size, int stride);

#ifdef __cplusplus
}
#endif

#endif