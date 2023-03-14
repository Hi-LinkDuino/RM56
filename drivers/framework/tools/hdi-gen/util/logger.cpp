/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "util/logger.h"
#include <cstdio>

namespace OHOS {
namespace HDI {
int Logger::level_ = DEBUG;

void Logger::D(const char *tag, const char *format, ...)
{
    if (level_ > DEBUG) {
        return;
    }

    va_list args;
    va_start(args, format);
    Log(tag, format, args);
    va_end(args);
}

void Logger::E(const char *tag, const char *format, ...)
{
    if (level_ > ERROR) {
        return;
    }

    va_list args;
    va_start(args, format);
    Err(tag, format, args);
    va_end(args);
}

void Logger::V(const char *tag, const char *format, ...)
{
    if (level_ > VERBOSE) {
        return;
    }

    va_list args;
    va_start(args, format);
    Log(tag, format, args);
    va_end(args);
}

void Logger::Log(const char *tag, const char *format, va_list args)
{
    (void)printf("[%s]: ", tag);
    (void)vprintf(format, args);
    (void)printf("\n");
}

void Logger::Err(const char *tag, const char *format, va_list args)
{
    (void)fprintf(stderr, "[%s]: ", tag);
    (void)vfprintf(stderr, format, args);
    (void)fprintf(stderr, "\n");
}
} // namespace HDI
} // namespace OHOS