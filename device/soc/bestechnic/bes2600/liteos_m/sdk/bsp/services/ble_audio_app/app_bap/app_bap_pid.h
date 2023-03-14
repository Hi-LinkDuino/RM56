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
/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_BAP_PID_H_
#define APP_BAP_PID_H_
#if BLE_AUDIO_ENABLED

typedef struct
{
    int32_t diff_time0;
    int32_t diff_time1;
    int32_t diff_time2;
    float Kp;
    float Ki;
    float Kd;
    float result;
} app_bap_pid_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_bap_pid_init(void);
float app_bap_pid_adjust(int32_t diff_time);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_PID_H_

/// @} APP_BAP
