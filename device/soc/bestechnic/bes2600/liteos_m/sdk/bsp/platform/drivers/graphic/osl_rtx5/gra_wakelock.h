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
#ifndef __INCLUDE_GRA_TIMER_H
#define __INCLUDE_GRA_TIMER_H
#include "cmsis_os.h"
#if defined(KERNEL_RTX5)
#include "rtx5/rtx_lib.h"
#endif

/**
 * gra_wakelock_create - create gra wakelock
 *
 * @ cb : timeout call back, with args
 * @ args : args with timeout call back func
 * @ name : gra wakelock name (length < 5)
 *
 * Returns: handler of gra wakelock .
 *
 */

void* gra_wakelock_create(void* cb, void *args, char* name);

/**
 * gra_wakelock_timeout - setup an active gra wakelock with timeout
 *
 * @ gralock : Semaphore to be initialized
 * @ ms : timeout value (in ms)
 *
 * Returns: NONE.
 *
 */

void gra_wakelock_timeout(void *gralock,uint32_t ms);

/**
 * gra_exit_sleep - keep system active for required time
 *
 */

void gra_exit_sleep(uint16_t active_time);


#endif //__INCLUDE_GRA_TIMER_H

