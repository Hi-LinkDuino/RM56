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
#ifndef __HAL_SPDIF_H__
#define __HAL_SPDIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CHIP_HAS_SPDIF

#include "plat_types.h"
#include "hal_cmu.h"
#include "hal_aud.h"

struct HAL_SPDIF_CONFIG_T {
    uint32_t bits;
    uint32_t channel_num;
    uint32_t sample_rate;

    uint32_t use_dma;
};

int hal_spdif_open(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream);
int hal_spdif_close(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream);
int hal_spdif_start_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream);
int hal_spdif_stop_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream);
int hal_spdif_setup_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream, struct HAL_SPDIF_CONFIG_T *cfg);
int hal_spdif_send(enum HAL_SPDIF_ID_T id, uint8_t *value, uint32_t value_len);
uint8_t hal_spdif_recv(enum HAL_SPDIF_ID_T id, uint8_t *value, uint32_t value_len);

int hal_spdif_clock_out_enable(enum HAL_SPDIF_ID_T id, uint32_t div);
int hal_spdif_clock_out_disable(enum HAL_SPDIF_ID_T id);

#endif // CHIP_HAS_SPDIF

#ifdef __cplusplus
}
#endif

#endif
