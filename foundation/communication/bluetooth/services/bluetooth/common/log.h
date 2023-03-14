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

#ifndef LOG_H
#define LOG_H

#define LOG_DOMAIN 0xD000102
#define LOG_TAG "Bluetooth"

#include "hilog/log.h"

#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#ifdef HILOGD
#undef HILOGD
#endif

#define FILENAME_SHORT (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define HILOGD(fmt, ...)                                                \
    HILOG_DEBUG(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt,    \
        FILENAME_SHORT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGI(fmt, ...)                                                \
    HILOG_INFO(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt,     \
        FILENAME_SHORT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGW(fmt, ...)                                                \
    HILOG_WARN(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt,     \
        FILENAME_SHORT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGE(fmt, ...)                                                \
    HILOG_ERROR(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt,    \
        FILENAME_SHORT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HILOGF(fmt, ...)                                                \
    HILOG_FATAL(LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt,    \
        FILENAME_SHORT, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif

#ifdef LOG_INFO
#undef LOG_INFO
#endif

#ifdef LOG_WARN
#undef LOG_WARN
#endif

#ifdef LOG_ERROR
#undef LOG_ERROR
#endif

#ifdef LOG_FATAL
#undef LOG_FATAL
#endif

#define LOG_DEBUG(...) HILOG_DEBUG(LOG_CORE, __VA_ARGS__)
#define LOG_INFO(...) HILOG_INFO(LOG_CORE, __VA_ARGS__)
#define LOG_WARN(...) HILOG_WARN(LOG_CORE, __VA_ARGS__)
#define LOG_ERROR(...) HILOG_ERROR(LOG_CORE, __VA_ARGS__)
#define LOG_FATAL(...) HILOG_FATAL(LOG_CORE, __VA_ARGS__)

#endif  // LOG_H
