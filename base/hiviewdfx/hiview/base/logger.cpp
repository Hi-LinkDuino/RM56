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

#include "logger.h"
#include <cstdarg>
#include "default_logger.h"
#ifndef __WITH_NO_HILOG__
#include "hi_logger.h"
#endif
namespace OHOS {
namespace HiviewDFX {
Logger::Logger()
{
    m_logger = std::make_unique<DefaultLogger>();
#ifndef __WITH_NO_HILOG__
    SetUserLogger(std::make_unique<HiLogger>());
#endif
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

bool Logger::SetUserLogger(std::unique_ptr<ILogger> logger)
{
    if (!logger) {
        return false;
    }
    m_logger = std::move(logger);
    return true;
}


void Logger::Print(uint32_t level, uint32_t domain, const char* tag, const char* format, ...)
{
    if (m_logger == nullptr) {
        return;
    }
    va_list args;
    va_start(args, format);
    m_logger->Print(level, domain, tag, format, args);
    va_end(args);
}
}
}