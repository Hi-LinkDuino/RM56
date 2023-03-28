/***************************************************************************
 *
 * Copyright 2019-2025 BES.
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
#ifndef _AUD_PARAM_H_
#define _AUD_PARAM_H_

#include <stdint.h>
#include "a7_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t mic_resample_48_to_16(uint8_t *src, uint8_t *dst, uint32_t len);
void mic_set_samplerate(int mic_samplerate);
int mic_get_samplerate();
int mic_get_handshake_done();
void mic_reset_handshake();
void mic_set_period(int mic_period_ms);
int mic_get_period();

#ifdef __cplusplus
}
#endif

#endif /* _AUD_DUMP_H_ */
