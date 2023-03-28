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


#ifndef __ANC_MODE_SELECT_H__
#define __ANC_MODE_SELECT_H__

#ifdef __cplusplus
extern "C" {
#endif

// #include "plat_types.h"
// #include "hal_aud.h"

//#define AF_ANC_DUMP_DATA
#include "stdbool.h"
#include "stdint.h"
#define ANC_SELECT_MODE_TEST_MAX_NUM 20


#define ANC_SELECT_MODE_TEST_NUM_MAX 20
#define ANC_SELECT_MODE_BAND_WEITHT_NUM_MAX 65
typedef struct {
    int mode;
    int gain;
} SELECT_MODE_TEST_SETTING_T;

typedef struct {
    int  sample_rate;
    int  frame_size;
    int  process_debug;
    int  snr_debug;
    int  snr_on;

    int  wait_period;
    int  test_period;

    int mode_freq_lower;
    int mode_freq_upper;
    int gain_freq_lower;
    int gain_freq_upper;

    int mode_shift_db;
    int mode_start_point;
    int mode_shift_upper_db;
    int mode_shift_lower_db;

    float gain_fb_power_thd;
    float mode_fb_power_thd;
    int gain_shift_db;
    int gain_start_point;
    int gain_shift_upper_db;
    int gain_shift_lower_db;
    float min_db_diff;
    float gain_ff_power_thd;
    float mode_ff_power_thd;
    float previous_denoise_value;

} SELECT_MODE_CFG_T;

typedef struct {
    int current_mode;
    int current_gain;
    float denoise_score;
    int best_mode;
    int best_gain;
    int normal_stop_flag;
    int snr_stop_flag;
    int fb_pwr_stop_flag;
    int ff_pwr_stop_flag;
    int min_diff_stop_flag;
} SELECT_MODE_RESULT_T;


typedef struct AncSelectModeState_ AncSelectModeState;

AncSelectModeState * anc_select_mode_create(SELECT_MODE_CFG_T * cfg);
AncSelectModeState * anc_select_mode_reset_cfg(AncSelectModeState *st ,SELECT_MODE_CFG_T * config);

void anc_select_mode_destroy(AncSelectModeState* st);


// SELECT_MODE_RESULT_T anc_select_gain_process(AncSelectModeState* st,void *inF, void *inB,int frame_len);
// SELECT_MODE_RESULT_T anc_select_mode_process(AncSelectModeState* st,void *inF, void *inB,int frame_len);

SELECT_MODE_RESULT_T anc_select_process(AncSelectModeState* st,float *inF, float *inB,int frame_len,int outer_pause_flag);


void anc_select_gain_set_auto_status(AncSelectModeState* st,bool state);
void anc_select_mode_set_auto_status(AncSelectModeState* st,bool state);
int32_t anc_select_mode_get_auto_status(AncSelectModeState* st);
int32_t anc_select_gain_get_auto_status(AncSelectModeState* st);
#ifdef __cplusplus
}
#endif

#endif