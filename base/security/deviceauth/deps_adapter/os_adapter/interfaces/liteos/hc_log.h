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

#ifndef HC_LOG_H
#define HC_LOG_H

#include <inttypes.h>

typedef enum {
    DEV_AUTH_LOG_LEVEL_DEBUG = 0,
    DEV_AUTH_LOG_LEVEL_INFO,
    DEV_AUTH_LOG_LEVEL_WARN,
    DEV_AUTH_LOG_LEVEL_ERROR
} DevAuthLogLevel;

#ifdef __cplusplus
extern "C" {
#endif

void DevAuthLogPrint(DevAuthLogLevel level, const char *funName, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#include "log.h"

#define LOGD(fmt, ...) (DevAuthLogPrint(DEV_AUTH_LOG_LEVEL_DEBUG, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGI(fmt, ...) (DevAuthLogPrint(DEV_AUTH_LOG_LEVEL_INFO, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGW(fmt, ...) (DevAuthLogPrint(DEV_AUTH_LOG_LEVEL_WARN, __FUNCTION__, fmt, ##__VA_ARGS__))
#define LOGE(fmt, ...) (DevAuthLogPrint(DEV_AUTH_LOG_LEVEL_ERROR, __FUNCTION__, fmt, ##__VA_ARGS__))

#define DEV_AUTH_LOG_DEBUG(buf) HILOG_DEBUG(HILOG_MODULE_SCY, "%{public}s", buf)
#define DEV_AUTH_LOG_INFO(buf) HILOG_INFO(HILOG_MODULE_SCY, "%{public}s", buf)
#define DEV_AUTH_LOG_WARN(buf) HILOG_WARN(HILOG_MODULE_SCY, "%{public}s", buf)
#define DEV_AUTH_LOG_ERROR(buf) HILOG_ERROR(HILOG_MODULE_SCY, "%{public}s", buf)

#endif
