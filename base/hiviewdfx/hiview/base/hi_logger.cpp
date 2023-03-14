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

#include "hi_logger.h"
#include <cstdarg>
#include <cstdio>
#include <string>
#include "hilog/log.h"
extern "C" {
    int HiLogPrintArgs(LogType type, LogLevel level, unsigned int domain, const char* tag, const char* fmt, va_list ap);
}
namespace OHOS {
namespace HiviewDFX {
const ::LogLevel LOG_LEVELS[] = {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
};
void HiLogger::Print(uint32_t level, uint32_t domain, const char* tag, const char* format, va_list args)
{
    HiLogPrintArgs(LOG_CORE, LOG_LEVELS[level], domain, tag, format, args);
}
}
}