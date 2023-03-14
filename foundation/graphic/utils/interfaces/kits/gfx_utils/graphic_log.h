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

#ifndef GRAPHIC_LITE_GRAPHIC_LOG_H
#define GRAPHIC_LITE_GRAPHIC_LOG_H

#include "graphic_config.h"
#if ENABLE_GRAPHIC_LOG
#if defined(__LITEOS__) || defined(_LITEOS)
#include "log.h"
#include <cstring>
#else
#include <stdio.h>
#endif
#endif

namespace OHOS {
#if ENABLE_GRAPHIC_LOG
#ifdef __LITEOS__
typedef enum {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX
} LOG_LEVEL;

static constexpr HiviewDFX::HiLogLabel GRPHIC_LABEL = {LOG_CORE, 0xD001400, "Graphic"};

#define FILE_NAME(x) (strrchr(x, '/') ? (strrchr(x, '/') + 1) : x)

#define GRAPHIC_DECORATOR_HILOG(level, op, fmt, args...)                                                    \
do {                                                                                                        \
    if ((level < GRAPHIC_LOG_LEVEL) && (level < LOG_LEVEL_MAX)) {                                           \
        op(GRPHIC_LABEL, "{%s()-%s:%d} " fmt, __FUNCTION__, FILE_NAME(__FILE__), __LINE__, ##args);         \
    }                                                                                                       \
} while (0)

#define GRAPHIC_LOGF(fmt, args...) GRAPHIC_DECORATOR_HILOG(LOG_LEVEL_FATAL, HiviewDFX::HiLog::Fatal, fmt, ##args)
#define GRAPHIC_LOGE(fmt, args...) GRAPHIC_DECORATOR_HILOG(LOG_LEVEL_ERROR, HiviewDFX::HiLog::Error, fmt, ##args)
#define GRAPHIC_LOGW(fmt, args...) GRAPHIC_DECORATOR_HILOG(LOG_LEVEL_WARN, HiviewDFX::HiLog::Warn, fmt, ##args)
#define GRAPHIC_LOGI(fmt, args...) GRAPHIC_DECORATOR_HILOG(LOG_LEVEL_INFO, HiviewDFX::HiLog::Info, fmt, ##args)
#define GRAPHIC_LOGD(fmt, args...) GRAPHIC_DECORATOR_HILOG(LOG_LEVEL_DEBUG, HiviewDFX::HiLog::Debug, fmt, ##args)

#elif defined(_LITEOS)
#define GRAPHIC_LOGF(fmt, args...) HILOG_FATAL(HILOG_MODULE_UIKIT, fmt, ##args)
#define GRAPHIC_LOGE(fmt, args...) HILOG_ERROR(HILOG_MODULE_UIKIT, fmt, ##args)
#define GRAPHIC_LOGW(fmt, args...) HILOG_WARN(HILOG_MODULE_UIKIT, fmt, ##args)
#define GRAPHIC_LOGI(fmt, args...) HILOG_INFO(HILOG_MODULE_UIKIT, fmt, ##args)
#define GRAPHIC_LOGD(fmt, args...) HILOG_DEBUG(HILOG_MODULE_UIKIT, fmt, ##args)

#else
#define GRAPHIC_LOGF(fmt, ...) printf("[%d]" fmt "\n", __LINE__, ##__VA_ARGS__)
#define GRAPHIC_LOGE(fmt, ...) GRAPHIC_LOGF(fmt, __VA_ARGS__)
#define GRAPHIC_LOGW(fmt, ...) GRAPHIC_LOGF(fmt, __VA_ARGS__)
#define GRAPHIC_LOGI(fmt, ...) GRAPHIC_LOGF(fmt, __VA_ARGS__)
#define GRAPHIC_LOGD(fmt, ...) GRAPHIC_LOGF(fmt, __VA_ARGS__)
#endif // __LITEOS__

#else // ENABLE_GRAPHIC_LOG
#define GRAPHIC_LOGF(...)
#define GRAPHIC_LOGE(...)
#define GRAPHIC_LOGW(...)
#define GRAPHIC_LOGI(...)
#define GRAPHIC_LOGD(...)

#endif // ENABLE_GRAPHIC_LOG
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_LOG_H
