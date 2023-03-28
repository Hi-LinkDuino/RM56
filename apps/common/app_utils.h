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
#ifndef __APP_UTILS_H__
#define __APP_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sysfreq.h"

// APP_SYSFREQ_USER_APP_0 is APP_MAIN
#define APP_SYSFREQ_USER_BT_MAIN            APP_SYSFREQ_USER_APP_1
#define APP_SYSFREQ_USER_HCI                APP_SYSFREQ_USER_APP_2
#define APP_SYSFREQ_USER_BT_A2DP            APP_SYSFREQ_USER_APP_3
#define APP_SYSFREQ_USER_UNUSED             APP_SYSFREQ_USER_APP_4
#define APP_SYSFREQ_USER_AI_VOICE           APP_SYSFREQ_USER_APP_5
#define APP_SYSFREQ_USER_BT_SCO             APP_SYSFREQ_USER_APP_6
#define APP_SYSFREQ_USER_OTA                APP_SYSFREQ_USER_APP_7
#define APP_SYSFREQ_USER_PROMPT_MIXER       APP_SYSFREQ_USER_APP_8
#define APP_SYSFREQ_USER_APP_NT             APP_SYSFREQ_USER_APP_9
#define APP_SYSFREQ_USER_VOICE_ASSIST       APP_SYSFREQ_USER_APP_10
#define APP_SYSFREQ_USER_TRIGGER            APP_SYSFREQ_USER_APP_11
#define APP_SYSFREQ_USER_BIS                APP_SYSFREQ_USER_APP_12
#define APP_SYSFREQ_USER_TOTA               APP_SYSFREQ_USER_APP_13

/*
 * Pseudo user, if one of user is belong to qos(quality of service) user,
 * when request cpu freq, it will changed to this user
 */
#define APP_SYSFREQ_USER_QOS             APP_SYSFREQ_USER_APP_12

enum APP_SYSFREQ_USER_T {
    APP_SYSFREQ_USER_APP_INIT = HAL_SYSFREQ_USER_INIT,
    APP_SYSFREQ_USER_WIFI     = HAL_SYSFREQ_USER_WIFI,
    APP_SYSFREQ_USER_APP_0    = HAL_SYSFREQ_USER_APP_0,
    APP_SYSFREQ_USER_APP_1    = HAL_SYSFREQ_USER_APP_1,
    APP_SYSFREQ_USER_APP_2    = HAL_SYSFREQ_USER_APP_2,
    APP_SYSFREQ_USER_APP_3    = HAL_SYSFREQ_USER_APP_3,
    APP_SYSFREQ_USER_APP_4    = HAL_SYSFREQ_USER_APP_4,
    APP_SYSFREQ_USER_APP_5    = HAL_SYSFREQ_USER_APP_5,
    APP_SYSFREQ_USER_APP_6    = HAL_SYSFREQ_USER_APP_6,
    APP_SYSFREQ_USER_APP_7    = HAL_SYSFREQ_USER_APP_7,
    APP_SYSFREQ_USER_APP_8    = HAL_SYSFREQ_USER_APP_8,
    APP_SYSFREQ_USER_APP_9    = HAL_SYSFREQ_USER_APP_9,
    APP_SYSFREQ_USER_APP_10   = HAL_SYSFREQ_USER_APP_10,
    APP_SYSFREQ_USER_APP_11   = HAL_SYSFREQ_USER_APP_11,
    APP_SYSFREQ_USER_APP_12   = HAL_SYSFREQ_USER_APP_12,
    APP_SYSFREQ_USER_APP_13   = HAL_SYSFREQ_USER_APP_13,
    APP_SYSFREQ_USER_APP_14   = HAL_SYSFREQ_USER_APP_14,
    APP_SYSFREQ_USER_APP_15   = HAL_SYSFREQ_USER_APP_15,
    APP_SYSFREQ_USER_QTY
};

enum APP_SYSFREQ_FREQ_T {
    APP_SYSFREQ_32K =  HAL_CMU_FREQ_32K,
    APP_SYSFREQ_26M =  HAL_CMU_FREQ_26M,
    APP_SYSFREQ_52M =  HAL_CMU_FREQ_52M,
    APP_SYSFREQ_78M =  HAL_CMU_FREQ_78M,
    APP_SYSFREQ_104M = HAL_CMU_FREQ_104M,
    APP_SYSFREQ_208M = HAL_CMU_FREQ_208M,
#if defined(CHIP_BEST2001) || defined(CHIP_BEST2002) || defined(CHIP_BEST2003)
    APP_SYSFREQ_390M = HAL_CMU_FREQ_390M,
#endif
    APP_SYSFREQ_FREQ_QTY =  HAL_CMU_FREQ_QTY
};

enum APP_WDT_THREAD_CHECK_USER_T {
    APP_WDT_THREAD_CHECK_USER_APP,
    APP_WDT_THREAD_CHECK_USER_AF,
    APP_WDT_THREAD_CHECK_USER_BT,
    APP_WDT_THREAD_CHECK_USER_3,
    APP_WDT_THREAD_CHECK_USER_4,
    APP_WDT_THREAD_CHECK_USER_5,
    APP_WDT_THREAD_CHECK_USER_6,
    APP_WDT_THREAD_CHECK_USER_7,
    APP_WDT_THREAD_CHECK_USER_8,
    APP_WDT_THREAD_CHECK_USER_9,
    APP_WDT_THREAD_CHECK_USER_10,
    
    APP_WDT_THREAD_CHECK_USER_QTY
};

int app_sysfreq_req(enum APP_SYSFREQ_USER_T user, enum APP_SYSFREQ_FREQ_T freq);

#ifdef __cplusplus
}
#endif

#endif//__FMDEC_H__
