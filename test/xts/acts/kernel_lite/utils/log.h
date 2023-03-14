/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KERNEL_LITE_LOG
#define KERNEL_LITE_LOG

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
    const float SYS_NS_PER_S = 1000 * 1000 * 1000;
    // get current time, for logging only
    static float GetCurTime(void)
    {
        struct timespec time1 = {0, 0};
        clock_gettime(CLOCK_MONOTONIC, &time1);
        return time1.tv_sec + ((float)time1.tv_nsec) / SYS_NS_PER_S;
    }
    #define LOGD(format, ...) fprintf(stdout, "[%.06f] " format "\n", GetCurTime(), ##__VA_ARGS__)
    #define LOG(format, ...)  fprintf(stdout, "[%.06f] " format "\n", GetCurTime(), ##__VA_ARGS__)
#else
    #define LOGD(...)
    #define LOG(format, ...)  fprintf(stdout, format "\n", ##__VA_ARGS__)
#endif

#define LOGE(format, ...)  fprintf(stdout,  "\n%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define PANIC(format, ...)   do {     \
        LOGE(format, ##__VA_ARGS__);  \
        exit(1);                      \
    } while (0)


#ifdef __cplusplus
}
#endif

#endif
