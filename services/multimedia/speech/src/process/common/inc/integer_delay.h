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
#ifndef INTEGER_DELAY_H
#define INTEGER_DELAY_H

struct IntegerDelayState_;

typedef struct IntegerDelayState_ IntegerDelayState;

#ifdef __cplusplus
extern "C" {
#endif

IntegerDelayState *integer_delay_init(int frame_size, int delay, int sample_size);

void integer_delay_destroy(IntegerDelayState *st);

void integer_delay_process(IntegerDelayState *st, int16_t *in, int16_t *out);

void integer_delay_process2(IntegerDelayState *st, int16_t *in, int in_stride, int16_t *out, int out_stride);

float integer_delay_get_required_mips(IntegerDelayState *st);

#ifdef __cplusplus
}
#endif

#endif