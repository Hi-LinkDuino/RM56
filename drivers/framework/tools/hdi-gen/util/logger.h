/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_LOGGER_H
#define OHOS_HDI_LOGGER_H

#include <cstdarg>

namespace OHOS {
namespace HDI {
class Logger {
public:
    static void D(const char *tag, const char *format, ...);

    static void E(const char *tag, const char *format, ...);

    static void V(const char *tag, const char *format, ...);

    inline static void SetLevel(int level)
    {
        level_ = level;
    }

    static constexpr int VERBOSE = 0;
    static constexpr int DEBUG = 1;
    static constexpr int ERROR = 2;
    static constexpr int NOLOG = 3;

private:
    Logger();

    ~Logger();

    static void Log(const char *tag, const char *format, va_list args);

    static void Err(const char *tag, const char *format, va_list args);

    static int level_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_LOGGER_H