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
#ifndef __HAL_TIMER_RAW_H__
#define __HAL_TIMER_RAW_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IGNORE_HAL_TIMER_RAW_API_CHECK
#if !(defined(ROM_BUILD) || defined(SIMU))
#error "Raw timer API can only be used in rom or simulation environment!"
#endif
#endif

#include "hal_timer.h"

void hal_timer_setup(enum HAL_TIMER_TYPE_T type, HAL_TIMER_IRQ_HANDLER_T handler);

#ifdef BSP_NICKNAME_SUPPORT
void hal_timer_start_nickname(uint32_t load);
void hal_timer_stop_nickname(void);
#define hal_timer_start hal_timer_start_nickname
#define hal_timer_stop hal_timer_stop_nickname
#else
void hal_timer_start(uint32_t load);
void hal_timer_stop(void);
#endif
void hal_timer_continue(void);

int hal_timer_is_enabled(void);

void hal_timer_reload(uint32_t load);

uint32_t hal_timer_get_raw_value(void);

int hal_timer_irq_active(void);

int hal_timer_irq_pending(void);

uint32_t hal_timer_get_elapsed_time(void);

#ifdef __cplusplus
}
#endif

#endif
