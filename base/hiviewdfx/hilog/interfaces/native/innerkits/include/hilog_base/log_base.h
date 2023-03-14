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

#ifndef HIVIEWDFX_HILOG_BASE_C_H
#define HIVIEWDFX_HILOG_BASE_C_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Log domain
#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0
#endif

// Log tag
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

// Log type
typedef enum {
    LOG_TYPE_MIN = 0,
    LOG_APP = 0,
    // Log to kmsg, only used by init phase.
    LOG_INIT = 1,
    // Used by core service, framework.
    LOG_CORE = 3,
    LOG_TYPE_MAX
} LogType;

// Log level
typedef enum {
    LOG_LEVEL_MIN = 0,
    LOG_DEBUG = 3,
    LOG_INFO = 4,
    LOG_WARN = 5,
    LOG_ERROR = 6,
    LOG_FATAL = 7,
    LOG_LEVEL_MAX,
} LogLevel;

int HiLogBasePrint(LogType type, LogLevel level, unsigned int domain, const char *tag, const char *fmt, ...)
    __attribute__((__format__(os_log, 5, 6)));

#define HILOG_BASE_DEBUG(type, ...) ((void)HiLogBasePrint((type), LOG_DEBUG, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define HILOG_BASE_INFO(type, ...) ((void)HiLogBasePrint((type), LOG_INFO, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define HILOG_BASE_WARN(type, ...) ((void)HiLogBasePrint((type), LOG_WARN, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define HILOG_BASE_ERROR(type, ...) ((void)HiLogBasePrint((type), LOG_ERROR, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define HILOG_BASE_FATAL(type, ...) ((void)HiLogBasePrint((type), LOG_FATAL, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

bool HiLogBaseIsLoggable(unsigned int domain, const char *tag, LogLevel level);

#ifdef __cplusplus
}
#endif

#endif  // HIVIEWDFX_HILOG_BASE_C_H
