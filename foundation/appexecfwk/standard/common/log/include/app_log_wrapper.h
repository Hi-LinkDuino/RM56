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

#ifndef FOUNDATION_APPEXECFWK_STANDARD_COMMON_LOG_INCLUDE_APP_LOG_WRAPPER_H
#define FOUNDATION_APPEXECFWK_STANDARD_COMMON_LOG_INCLUDE_APP_LOG_WRAPPER_H

#include "hilog/log.h"
#include <string>

namespace OHOS {
namespace AppExecFwk {
#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0xD001100
#endif

#ifndef APP_LOG_TAG
#define APP_LOG_TAG NULL
#endif

enum class AppLogLevel { DEBUG = 0, INFO, WARN, ERROR, FATAL };

static constexpr OHOS::HiviewDFX::HiLogLabel APP_LABEL = {LOG_CORE, LOG_DOMAIN, APP_LOG_TAG};

class AppLogWrapper {
public:
    static bool JudgeLevel(const AppLogLevel &level);

    static void SetLogLevel(const AppLogLevel &level)
    {
        level_ = level;
    }

    static const AppLogLevel &GetLogLevel()
    {
        return level_;
    }

    static std::string GetBriefFileName(const char *str);

private:
    static AppLogLevel level_;
};

#define AFWK_PRINT_LOG(LEVEL, Level, fmt, ...)                  \
    if (OHOS::AppExecFwk::AppLogWrapper::JudgeLevel(OHOS::AppExecFwk::AppLogLevel::LEVEL))     \
    OHOS::HiviewDFX::HiLog::Level(OHOS::AppExecFwk::APP_LABEL,               \
        "[%{public}s(%{public}s):%{public}d] " fmt,        \
        OHOS::AppExecFwk::AppLogWrapper::GetBriefFileName(__FILE__).c_str(), \
        __FUNCTION__,                                      \
        __LINE__,                                          \
        ##__VA_ARGS__)

#define APP_LOGD(fmt, ...) AFWK_PRINT_LOG(DEBUG, Debug, fmt, ##__VA_ARGS__)
#define APP_LOGI(fmt, ...) AFWK_PRINT_LOG(INFO, Info, fmt, ##__VA_ARGS__)
#define APP_LOGW(fmt, ...) AFWK_PRINT_LOG(WARN, Warn, fmt, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...) AFWK_PRINT_LOG(ERROR, Error, fmt, ##__VA_ARGS__)
#define APP_LOGF(fmt, ...) AFWK_PRINT_LOG(FATAL, Fatal, fmt, ##__VA_ARGS__)
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_STANDARD_COMMON_LOG_INCLUDE_APP_LOG_WRAPPER_H
