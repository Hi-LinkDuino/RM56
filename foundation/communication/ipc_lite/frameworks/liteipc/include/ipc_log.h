/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef OHOS_IPC_LOG_H
#define OHOS_IPC_LOG_H

#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD001500
#define LOG_TAG "Communication"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define HILOG_X(op, fmt, ...)                                 \
    do {                                                      \
        op(LOG_CORE, fmt, ##__VA_ARGS__);                     \
    } while (0)

#define IPC_LOG(level, fmt, ...)   IPC_LOG_##level(fmt, ##__VA_ARGS__)
#define IPC_LOG_DEBUG(fmt, ...)    HILOG_X(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define IPC_LOG_INFO(fmt, ...)     HILOG_X(HILOG_INFO,  fmt, ##__VA_ARGS__)
#define IPC_LOG_WARN(fmt, ...)     HILOG_X(HILOG_WARN,  fmt, ##__VA_ARGS__)
#define IPC_LOG_ERROR(fmt, ...)    HILOG_X(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define IPC_LOG_FATAL(fmt, ...)    HILOG_X(HILOG_FATAL, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
