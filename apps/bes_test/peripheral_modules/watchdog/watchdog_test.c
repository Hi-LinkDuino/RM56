/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_wdt.h"

static void watchdog_irq_handler(enum HAL_WDT_ID_T id, enum HAL_WDT_EVENT_T event)
{
    printf("%s: id %d event %d\r\n", __func__, id, event);
}

static int watchdog_feed(void)
{
    hal_wdt_ping(HAL_WDT_ID_0);
    return 0;
}

static int watchdog_init(int seconds)
{
    hal_wdt_set_irq_callback(HAL_WDT_ID_0, watchdog_irq_handler);
    hal_wdt_set_timeout(HAL_WDT_ID_0, seconds);
    hal_wdt_start(HAL_WDT_ID_0);
    hal_sleep_set_deep_sleep_hook(HAL_DEEP_SLEEP_HOOK_USER_WDT, watchdog_feed);
    return 0;
}

void watchdog_test(void)
{
    int watchdog_feed_cnt = 0;
    watchdog_init(4);
    while (1) {
        printf("%s: now %u\r\n", __func__, GET_CURRENT_MS());
        watchdog_feed_cnt++;
        hal_sys_timer_delay(MS_TO_TICKS(watchdog_feed_cnt * 1000));
        watchdog_feed();
        // the system will reboot after watchdog_feed_cnt >= 4
    }
}
