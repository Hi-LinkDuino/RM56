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
#ifndef CMSIS_FFTWRAP_H
#define CMSIS_FFTWRAP_H

/*
mode: FFT output order
0: This is default roder in CMSIS DSP Lib
1: This is compatible with kiss fft
|-------|--------------------------|--------------------------|
|  mode |            0             |            1             |
|-------|--------------------------|--------------------------|
|       | out[0]~y[0]              | out[0]~y[0]              |
|       | out[1]~y[N/2]            | out[1]~y[1].real         |
|       | out[2]~y[1].real         | out[2]~y[1].imag         |
|       | out[3]~y[1].imag         | out[3]~y[2].real         |
| Order | out[4]~y[2].real         | out[4]~y[2].imag         |
|       | out[5]~y[2].imag         | ……                       |
|       | ……                       | out[N - 3]~y[N/2-1].real |
|       | out[N - 2]~y[N/2-1].real | out[N - 2]~y[N/2-1].imag |
|       | out[N - 1]~y[N/2-1].imag | out[N - 1]~y[N/2]        |
|-------|--------------------------|--------------------------|
*/

#ifdef __cplusplus
extern "C" {
#endif

void *cmsis_f32_fft_init(int size, int mode);

void cmsis_f32_fft_destroy(void *table);

void cmsis_f32_fft(void *table, float *in, float *out);

void cmsis_f32_ifft(void *table, float *in, float *out);

void *cmsis_q15_fft_init(int size, int mode);

void cmsis_q15_fft_destroy(void *table);

void cmsis_q15_fft(void *table, short *in, short *out);

#ifdef __cplusplus
}
#endif

#endif