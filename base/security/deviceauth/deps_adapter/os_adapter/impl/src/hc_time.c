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

#include "hc_time.h"
#include <stdio.h>
#include <time.h>
#include "hc_log.h"

#ifdef __cplusplus
extern "C" {
#endif

int64_t HcGetCurTime()
{
    struct timespec start;
    int res = clock_gettime(CLOCK_MONOTONIC, &start);
    if (res != 0) {
        LOGE("clock_gettime failed, res:%d", res);
        return -1;
    }
    return start.tv_sec;
}

int64_t HcGetIntervalTime(int64_t startTime)
{
    if (startTime < 0) {
        LOGE("Start time is invalid");
        return -1;
    }
    struct timespec end;
    int res = clock_gettime(CLOCK_MONOTONIC, &end);
    if (res != 0) {
        LOGE("clock_gettime failed, res:%d", res);
        return -1;
    }
    if (end.tv_sec < startTime) {
        LOGE("End time is invalid");
        return -1;
    }
    return (end.tv_sec - startTime);
}

#ifdef __cplusplus
}
#endif
