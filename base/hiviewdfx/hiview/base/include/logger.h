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
#ifndef HIVIEW_BASE_LOGGER_H
#define HIVIEW_BASE_LOGGER_H

#include <cinttypes>
#include <cstdarg>
#include <string>
#include "i_logger.h"

#define PUBLIC "{public}"
// All Log domain in hiview should has the prefix of 0xD002D
// And every src file use this header should define LOG_DOMAIN and LOG_TAG
#define DEFINE_LOG_TAG(name) \
    static unsigned int logLabelDomain = 0xD002D10; \
    static const char *logLabelTag = name

#define DEFINE_LOG_LABEL(region, name) \
    static unsigned int logLabelDomain = region; \
    static const char *logLabelTag = name

#define HIVIEW_LOGD(format, ...) \
    OHOS::HiviewDFX::Logger::GetInstance().Print(0, logLabelDomain, logLabelTag, \
        "%" PUBLIC "s: " format, __func__, ##__VA_ARGS__)
#define HIVIEW_LOGI(format, ...) \
    OHOS::HiviewDFX::Logger::GetInstance().Print(1, logLabelDomain, logLabelTag, \
        "%" PUBLIC "s: " format, __func__, ##__VA_ARGS__)
#define HIVIEW_LOGW(format, ...) \
    OHOS::HiviewDFX::Logger::GetInstance().Print(2, logLabelDomain, logLabelTag, \
        "%" PUBLIC "s: " format, __func__, ##__VA_ARGS__)
#define HIVIEW_LOGE(format, ...) \
    OHOS::HiviewDFX::Logger::GetInstance().Print(3, logLabelDomain, logLabelTag, \
        "%" PUBLIC "s: " format, __func__, ##__VA_ARGS__)
#define HIVIEW_LOGF(format, ...) \
    OHOS::HiviewDFX::Logger::GetInstance().Print(4, logLabelDomain, logLabelTag, \
        "%" PUBLIC "s: " format, __func__, ##__VA_ARGS__)


namespace OHOS::HiviewDFX {
class Logger final {
public:
    static Logger& GetInstance();
    bool SetUserLogger(std::unique_ptr<ILogger> logger);
    void Print(uint32_t level, uint32_t domain, const char* tag, const char*, ...);
private:
    Logger();
    virtual ~Logger() = default;
    std::unique_ptr<ILogger> m_logger;
};
}
#endif // HIVIEW_BASE_LOGGER_H
