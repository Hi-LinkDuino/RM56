/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __APP_A2DP_SOURCE_H__
#define __APP_A2DP_SOURCE_H__
#include "a2dp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SOURCE_TRACE_RX__)
typedef enum
{
    SEARCH_DEVIECE = 0x01,
    CONNECT_DEVICE,
    MOBILE_LAURENT_THRESHOLD,
    HDR_OLD_CORR_THR,
    MOBILE_RX_GAIN_SEL,
    MOBILE_RX_GAIN_IDX,
    FA_RX_GAIN_SEL,
    FA_RX_GAIN_IDX,
    FA_RX_LUR_THR,
    FA_OLD_CORR_THR,
    ECC_BLOCK,
    ECC_ENABLE,
    MOBILE_TX_POWER,
    FA_TX_POWER,
    FAST_LOCK_ON_OFF,
#ifdef A2DP_SOURCE_TEST
    CONNECT_DEVICE_TEST,
    SEND_SBC_PKT_TEST,
    TOGGLE_STREAM_TEST,
    SOURCE_SNIFF_TEST,
#endif
}SOURCE_TRACE_RX_COMMAND;
#endif

void app_a2dp_source_start_stream(uint8_t device_id);

void app_a2dp_source_suspend_stream(uint8_t device_id);

void app_a2dp_source_toggle_stream(uint8_t device_id);

uint8_t a2dp_source_get_play_status(bt_bdaddr_t* remote_addr);

void a2dp_source_start_pcm_capture(uint8_t device_id);

void a2dp_source_stop_pcm_capture(uint8_t device_id);

#if defined(OPEN_BT_SOURCE_CONNECT)
void bt_source_stream_test(void);
#endif

uint32_t a2dp_source_write_pcm_data(uint8_t * pcm_buf, uint32_t len);

#if defined(APP_LINEIN_A2DP_SOURCE)
//////////start the audio linein stream for capure the pcm data
int app_a2dp_source_linein_on(bool on);
#endif

#if defined(APP_I2S_A2DP_SOURCE)
int app_a2dp_source_I2S_onoff(bool onoff);
#endif

void app_a2dp_source_audio_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_A2DP_SOURCE_H__ */

