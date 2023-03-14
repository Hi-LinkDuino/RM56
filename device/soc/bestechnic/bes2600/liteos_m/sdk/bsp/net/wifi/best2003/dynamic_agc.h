/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifdef BES_SOFT_AGC_ANTI_NOISE
#ifndef __DYNAMIC_AGC_H__
#define __DYNAMIC_AGC_H__

typedef enum {
    WIFI_HW_AGC = 0,
    WIFI_SOFT_AGC,
    WIFI_INVALID_AGC
} WIFI_AGC_MODE_T;

typedef enum {
    WIFI_AGC_SAMPLE_TIME = 0,
    WIFI_AGC_SELECT_TIME,
    WIFI_AGC_WORK_TIME
} WIFI_AGC_ALG_STATE;

typedef struct _WIFI_AGC_INFO {
    uint8_t alg_state;
    uint8_t soft_agc_index;
    uint8_t work_cnt;
    uint32_t agc_mode;

    int soft_agc_tx_psr;
    int soft_agc_rx_psr;

    int hw_agc_tx_psr;
    int hw_agc_rx_psr;

    uint32_t tx_succ_cnt;
    uint32_t tx_attempt;
    uint32_t rx_succ;
    uint32_t rx_leak;
    bwifi_station_linkinfo info;
} WIFI_AGC_INFO;

/*
 - wifi_agc_init
 - flag: 0 - normal boot; 1 - wifi reset
*/
extern int wifi_agc_init(uint8_t flag);
extern uint32_t *wifi_agc_get_mode_addr();
#endif
#endif
