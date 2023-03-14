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
#ifndef __APP_TIMER_H__
#define __APP_TIMER_H__

#include "plat_types.h"

typedef void (*app_timer_cb)(void *param);

#if defined(__cplusplus)
extern "C" {
#endif

void app_timer_probe_init(app_timer_cb cb, void* usrdata);

void app_timer_probe_deinit(void);

void app_timer_probe_start(uint32_t timeout_val);

void app_timer_probe_stop(void);

void app_timer_probe_tune(uint32_t dest_val, uint32_t current_tick, void* usrdata);

#if defined(__cplusplus)
}
#endif
#endif // end of __APP_TIMER_H__