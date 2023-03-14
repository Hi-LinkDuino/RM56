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
#ifndef __SPEECH_3MIC_NS_H__
#define __SPEECH_3MIC_NS_H__

#include <stdint.h>
#include "speech_eq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
	int32_t     blend_en;
	int32_t     authen_en;
    float     	delay_tapsM;            // MIC L/R delay samples. 0: 适用于麦克距离为<2cm; 1: 适用于麦克距离为2cm左右; 2: 适用于麦克距离为4cm左右
    int32_t     delay_tapsS;
	float       denoise_dB;
	int32_t     crossover_freq;
    int32_t     freq_smooth_enable;     // 1: 频域滤波打开; 0: 频域滤波关闭; 默认打开
    int32_t     wnr_enable;             // wind noise reduction enable
	float       noise_supp;
	float       betalow;
	float       betamid;
	float       betahigh;
	float       ref_pwr_threshold;
	// filter
	float       *ff_fb_coeff;
	int32_t     ff_fb_coeff_len;
	EqConfig    eq_cfg;
} Speech3MicNsConfig;

struct Speech3MicNsState_;

typedef struct Speech3MicNsState_ Speech3MicNsState;

Speech3MicNsState *speech_3mic_ns_create(int32_t sample_rate, int32_t frame_size, const Speech3MicNsConfig *cfg);

int32_t speech_3mic_ns_destroy(Speech3MicNsState *st);

int32_t speech_3mic_ns_set_config(Speech3MicNsState *st, const Speech3MicNsConfig *cfg);

void speech_3mic_ns_preprocess(Speech3MicNsState *st, int16_t *pcm_buf, int32_t pcm_len, int16_t *out_buf);

int32_t speech_3mic_ns_process(Speech3MicNsState *st, short *pcm_buf, short *ref_buf, int32_t pcm_len, short *out_buf);

float speech_3mic_get_required_mips(Speech3MicNsState *st);

void speech_3mic_ns_set_ns_state(Speech3MicNsState *st, void *ns);

void speech_3mic_ns_update_snr(Speech3MicNsState *st, float snr);

#ifdef __cplusplus
}
#endif

#endif
