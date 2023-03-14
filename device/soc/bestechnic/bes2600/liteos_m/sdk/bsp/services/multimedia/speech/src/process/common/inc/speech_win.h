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
#ifndef __SPEECH_WIN_H__
#define __SPEECH_WIN_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

const float *get_conj_win_f32(int win_size, int frame_size, bool cp);

/*
frame_size: 120, 128, 240, 256
*/
const short *get_conj_win_half(int win_size, int frame_size, int *shift);

#ifdef __cplusplus
}
#endif

#endif
