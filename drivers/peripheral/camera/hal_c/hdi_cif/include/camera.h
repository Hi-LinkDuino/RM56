/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HOS_CAMERA_H
#define HOS_CAMERA_H

#include <functional>
#include <cstdio>
#include <cstdint>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "securec.h"

#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
#define gettid() (pid_t)syscall(SYS_gettid)
#endif

namespace OHOS::Camera {
#define GET_CURRENT_TIME_MS                                                                                 \
    struct timeval _tv;                                                                                     \
    gettimeofday(&_tv, NULL);                                                                               \
    struct tm* _tm = localtime(&_tv.tv_sec);                                                                \
    int _ms = _tv.tv_usec / 1000;                                                                           \
    char now[25] = {0};                                                                                     \
    sprintf_s(now, sizeof(now), "%02d-%02d %02d:%02d:%02d.%03d", _tm->tm_mon + 1, _tm->tm_mday, _tm->tm_hour,        \
        _tm->tm_min, _tm->tm_sec, _ms);

#define CAMERA_LOGE(fmt, ...)                                           \
    do {                                                                \
        GET_CURRENT_TIME_MS;                                            \
        pid_t pid = getpid();                                        \
        pid_t tid = gettid();                                        \
        printf("%s %4u %4u E " fmt "\n", now, (uint32_t)pid, (uint32_t)tid, ##__VA_ARGS__); \
        fflush(stdout);                                                 \
    } while (0);

#define CAMERA_LOGW(fmt, ...)                                           \
    do {                                                                \
        GET_CURRENT_TIME_MS;                                            \
        pid_t pid = getpid();                                        \
        pid_t tid = gettid();                                        \
        printf("%s %4u %4u W " fmt "\n", now, (uint32_t)pid, (uint32_t)tid, ##__VA_ARGS__); \
        fflush(stdout);                                                 \
    } while (0);

#define CAMERA_LOGI(fmt, ...)                                           \
    do {                                                                \
        GET_CURRENT_TIME_MS;                                            \
        pid_t pid = getpid();                                        \
        pid_t tid = gettid();                                        \
        printf("%s %4u %4u I " fmt "\n", now, (uint32_t)pid, (uint32_t)tid, ##__VA_ARGS__); \
        fflush(stdout);                                                 \
    } while (0);

#define CAMERA_LOGV(fmt, ...)                                           \
    do {                                                                \
        GET_CURRENT_TIME_MS;                                            \
        pid_t pid = getpid();                                        \
        pid_t tid = gettid();                                        \
        printf("%s %4u %4u V " fmt "\n", now, (uint32_t)pid, (uint32_t)tid, ##__VA_ARGS__); \
        fflush(stdout);                                                 \
    } while (0);

#define CAMERA_LOGD(fmt, ...)                                           \
    do {                                                                \
        GET_CURRENT_TIME_MS;                                            \
        pid_t pid = getpid();                                        \
        pid_t tid = gettid();                                        \
        printf("%s %4u %4u D " fmt "\n", now, (uint32_t)pid, (uint32_t)tid, ##__VA_ARGS__); \
        fflush(stdout);                                                 \
    } while (0);

constexpr uint32_t WATCHDOG_TIMEOUT = 20000;

enum RetCode {
    RC_OK = 0,
    RC_ERROR,
};

} // namespace OHOS::Camera
#endif
