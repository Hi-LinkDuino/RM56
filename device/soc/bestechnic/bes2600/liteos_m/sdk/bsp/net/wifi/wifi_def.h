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

#ifndef WIFI_DEF_H
#define WIFI_DEF_H

#ifdef __cplusplus
extern "C" {
#endif
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(wifi_def)

/******** enum ***********************/
enum ENUM_WIFI_RUN_MODE{
    WIFI_RUN_NORMAL_MODE = 0, //signaling
    WIFI_RUN_TEST_MODE   = 1, //nonsignaling
    WIFI_RUN_RESET_MODE  = 2, //wifi reset
    WIFI_RUN_MODE_MAX,
};

enum ENUM_WIFI_BAND{
    BAND_2G4 = 0,
    BAND_5G,
    DUAL_BAND
};

enum ENUM_WIFI_BAND_WIDTH{
    BAND_WIDTH_20M = 0,
    BAND_WIDTH_40M,
    BAND_WIDTH_MAX
};

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#ifndef DPD_CALI_BY_CHANNEL
#define DPD_CALI_BY_CHANNEL               1 //wifi dpd cali by channel (signal mode)
#endif

#ifndef SUP_WIDTH_20_40
#define SUP_WIDTH_20_40                   1 //HT20/40 support
#endif

//fem select
#ifndef WIFI_OUT_FEM
#define WIFI_OUT_FEM                 0 //0: default; 1: outside fem, dpd no table calibration
#endif

#ifdef __cplusplus
}
#endif

#endif /* WIFI_DEF_H */
