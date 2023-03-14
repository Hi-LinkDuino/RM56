/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_DHCP_LOGGER_H
#define OHOS_DHCP_LOGGER_H

#ifdef DHCP_HILOG_ENABLE
#include "hilog/log_c.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServer"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0x0000000

#define LOGD(...) ((void)HiLogPrint(LOG_CORE, LOG_DEBUG, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGI(...) ((void)HiLogPrint(LOG_CORE, LOG_INFO, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGW(...) ((void)HiLogPrint(LOG_CORE, LOG_WARN, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGE(...) ((void)HiLogPrint(LOG_CORE, LOG_ERROR, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGF(...) ((void)HiLogPrint(LOG_CORE, LOG_FATAL, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))
#else
#include <stdio.h>
#include <time.h>
#include "common_util.h"

#define LOG_DEBUG "Debug"
#define LOG_TRACE "Trace"
#define LOG_WARN " Warn"
#define LOG_ERROR "Error"
#define LOG_INFO " Info"

#define DEBUG_MODE 1

#define LOG_TAG "DhcpServer"

#define LOGD(log, ...)                          \
    do {                                        \
        LogTime();                              \
        printf(" %s] %s ", LOG_DEBUG, LOG_TAG); \
        printf((log), ##__VA_ARGS__);           \
        printf("(line:%d)\n", __LINE__);        \
    } while (0);

#define LOGW(log, ...)                         \
    do {                                       \
        LogTime();                             \
        printf(" %s] %s ", LOG_WARN, LOG_TAG); \
        printf((log), ##__VA_ARGS__);          \
        printf("\n");                          \
    } while (0);

#define LOGE(log, ...)                          \
    do {                                        \
        LogTime();                              \
        printf(" %s] %s ", LOG_ERROR, LOG_TAG); \
        printf((log), ##__VA_ARGS__);           \
        printf("(line:%d)\n", __LINE__);        \
    } while (0);

#define LOGI(log, ...)                         \
    do {                                       \
        LogTime();                             \
        printf(" %s] %s ", LOG_INFO, LOG_TAG); \
        printf((log), ##__VA_ARGS__);          \
        printf("\n");                          \
    } while (0);

#endif  // DHCP_HILOG_ENABLE
#endif
