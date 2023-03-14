/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_LOG_H
#define HC_GEN_LOG_H

#include <iostream>
#include <map>
#include <string>

#include "option.h"

namespace OHOS {
namespace Hardware {

class Logger {
public:
    Logger() : level_(INFO) {};

    inline ~Logger()
    {
        if (level_ > INFO) {
            ::std::cout << ERROR_COLOR_END;
        }
        if (level_ <= DEBUG && !Option::Instance().VerboseLog()) {
            return;
        }
        ::std::cout << ::std::endl;
    }

    template <typename T>
    inline Logger &operator<<(const T &v)
    {
        if (level_ <= DEBUG && !Option::Instance().VerboseLog()) {
            return *this;
        }
        ::std::cout << v;
        return *this;
    }

    inline Logger &Debug()
    {
        level_ = DEBUG;
        if (Option::Instance().VerboseLog()) {
            ShowLevel();
        }
        return *this;
    }

    inline Logger &Info()
    {
        level_ = INFO;
        ShowLevel();
        return *this;
    }

    inline Logger &Warning()
    {
        level_ = WARNING;
        ShowLevel();
        return *this;
    }

    inline Logger &Error()
    {
        level_ = ERROR;
        ShowLevel();
        return *this;
    }

    inline Logger &Fatal()
    {
        level_ = FATAL;
        ShowLevel();
        return *this;
    }

private:
    enum LogLevel {
        NONE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL,
    } level_;

    void ShowLevel()
    {
        static ::std::map<LogLevel, ::std::string> levelStrMap = {
            {NONE,    ""       },
            {DEBUG,   "Debug"  },
            {INFO,    "Info"   },
            {WARNING, "Warning"},
            {ERROR,   "Error"  },
            {FATAL,   "Fatal"  }
        };
        if (level_ > INFO) {
            ::std::cout << ERROR_COLOR_PREFIX;
        }
        ::std::cout << "[" << levelStrMap[level_] << "] ";
    }

#ifdef OS_LINUX
    static constexpr const char *ERROR_COLOR_PREFIX = "\033[31m";
    static constexpr const char *ERROR_COLOR_END = "\033[0m";
#else
    static constexpr const char *ERROR_COLOR_PREFIX = "";
    static constexpr const char *ERROR_COLOR_END = "";
#endif
};

} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_LOG_H
