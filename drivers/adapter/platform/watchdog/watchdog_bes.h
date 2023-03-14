/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef BES_WATCHDOG_H
#define BES_WATCHDOG_H

#include "hal_wdt.h"
#include "watchdog_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct WatchdogResource {
    enum HAL_WDT_ID_T watchdogId;
};

struct WatchdogDevice {
    struct WatchdogResource resource;
};

#ifdef __cplusplus
}
#endif
#endif
