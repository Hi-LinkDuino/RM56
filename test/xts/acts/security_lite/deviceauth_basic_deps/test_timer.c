/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_timer.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// 3861 platform limits that the running time cannot exceed 16 seconds.
// If the running time exceeds 16 seconds, crash occurs.
// The following comments are from at91SAM9_wdt.c in directory
// device/hisilicon/hispark_pegasus/sdk_liteos/third_party/u-boot-v2019.07/u-boot-v2019.07/drivers/watchdog
/*
 * All counting occurs at SLOW_CLOCK / 128 = 256 Hz
 *
 * Since WDV is a 12-bit counter, the maximum period is
 * 4096 / 256 = 16 seconds.
 */

enum {
    MAX_FIBONACCI_OPTION = 0xFFFFFF,
    MIN_FIBONACCI_OPTION = 0xFFFFF,
};

double CalcTimeSpecDiff(struct timespec before, struct timespec after)
{
    double mseconds = (((double)after.tv_sec - (double)before.tv_sec) * 1000000.0f +
        ((double)after.tv_nsec - (double)before.tv_nsec) / 1000.0f) / 1000.0f;
    return mseconds;
}

unsigned long long TimeConsumingOperation(void)
{
    unsigned long long i, n, t1 = 0, t2 = 1, nextTerm = 0;
    n = rand() % (MAX_FIBONACCI_OPTION - MIN_FIBONACCI_OPTION) + MIN_FIBONACCI_OPTION;
    for (i = 1; i <= n; ++i) {
        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;
    }

    return nextTerm;
}

#ifdef __cplusplus
}
#endif
