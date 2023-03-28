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
#ifndef __HAL_I2S_H__
#define __HAL_I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_aud.h"
#include "hal_cmu.h"

enum HAL_I2S_MODE_T {
    HAL_I2S_MODE_NULL,
    HAL_I2S_MODE_MASTER,
    HAL_I2S_MODE_SLAVE,
};

enum HAL_I2S_SYNC_TYPE_T {
    HAL_I2S_SYNC_TYPE_NONE,
    HAL_I2S_SYNC_TYPE_BT,
    HAL_I2S_SYNC_TYPE_GPIO,
};

struct HAL_I2S_CONFIG_T {
    bool use_dma;
    bool chan_sep_buf;
    bool sync_start;
    uint8_t cycles;
    uint8_t bits;
    uint8_t channel_num;
    enum AUD_CHANNEL_MAP_T channel_map;
    uint32_t sample_rate;
};

int hal_i2s_open(enum HAL_I2S_ID_T id, enum AUD_STREAM_T stream, enum HAL_I2S_MODE_T mode);
int hal_i2s_close(enum HAL_I2S_ID_T id, enum AUD_STREAM_T stream);
int hal_i2s_start_stream(enum HAL_I2S_ID_T id, enum AUD_STREAM_T stream);
int hal_i2s_stop_stream(enum HAL_I2S_ID_T id, enum AUD_STREAM_T stream);
int hal_i2s_setup_stream(enum HAL_I2S_ID_T id, enum AUD_STREAM_T stream, const struct HAL_I2S_CONFIG_T *cfg);
int hal_i2s_send(enum HAL_I2S_ID_T id, const uint8_t *value, uint32_t value_len);
uint8_t hal_i2s_recv(enum HAL_I2S_ID_T id, uint8_t *value, uint32_t value_len);

void hal_i2s_enable_delay(enum HAL_I2S_ID_T id);
void hal_i2s_disable_delay(enum HAL_I2S_ID_T id);
void hal_i2s_enable(enum HAL_I2S_ID_T id);

//================================================================================
// I2S Packet Mode
//================================================================================

int hal_i2s_packet_open(void);
int hal_i2s_packet_close(void);
int hal_i2s_start_transfer(void);
int hal_i2s_stop_transfer(void);

void hal_i2s_tx_sync_enable(enum HAL_I2S_SYNC_TYPE_T type);
void hal_i2s_tx_sync_disable(void);
void hal_i2s_rx_sync_enable(enum HAL_I2S_SYNC_TYPE_T type);
void hal_i2s_rx_sync_disable(void);
void hal_i2s_clk_sync_enable(enum HAL_I2S_SYNC_TYPE_T type);
void hal_i2s_clk_sync_disable(void);

#ifdef __cplusplus
}
#endif

#endif
