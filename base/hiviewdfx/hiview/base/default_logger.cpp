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

#include "default_logger.h"
#include <cstdarg>
#include <cstdio>
#include <string>
namespace OHOS {
namespace HiviewDFX {
inline void StringReplace(std::string& sourceStr, const std::string& subStr, const std::string& newStr)
{
    std::string::size_type pos = 0;
    while ((pos = sourceStr.find(subStr)) != std::string::npos) {
        sourceStr.replace(pos, subStr.length(), newStr);
    }
}


void DefaultLogger::Print(uint32_t level, uint32_t domain, const char* tag, const char* format, va_list args)
{
    std::string formatStr("%s: %s ", format);
    StringReplace(formatStr, "%{public}", "%");
    StringReplace(formatStr, "%{private}", "%");
    printf("Level[%d] domain[%d] :", level, domain);
    vprintf(formatStr.c_str(), args);
    printf("\n");
}
}
}