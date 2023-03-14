/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef WATCHDOG_TEST_H
#define WATCHDOG_TEST_H

#include "platform_if.h"

#ifdef __cplusplus
extern "C" {
#endif

enum WatchdogTestCmd {
    WATCHDOG_TEST_SET_GET_TIMEOUT = 0,
    WATCHDOG_TEST_START_STOP,
    WATCHDOG_TEST_FEED,
    WATCHDOG_TEST_RELIABILITY,
    WATCHDOG_TEST_BARK,
    WATCHDOG_IF_PERFORMANCE_TEST,
    WATCHDOG_TEST_MAX,
};

struct WatchdogTestConfig {
    int16_t id;
    uint32_t timeoutSet;
    uint32_t feedTime;
};

struct WatchdogTester {
    struct WatchdogTestConfig config;
    DevHandle handle;
};

int32_t WatchdogTestExecute(int cmd);

#ifdef __cplusplus
}
#endif

#endif /* WATCHDOG_TEST_H */
