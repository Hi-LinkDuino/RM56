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

#include "base/utils/string_utils.h"

#ifndef WINDOWS_PLATFORM
#include "securec.h"
#endif

namespace OHOS::Ace::StringUtils {
namespace {

const size_t MAX_STRING_SIZE = 256;

}

const char DEFAULT_STRING[] = "error";
const std::wstring DEFAULT_WSTRING = L"error";
const std::u16string DEFAULT_USTRING = u"error";

const std::string FormatString(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char name[MAX_STRING_SIZE] = { 0 };
    if (vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args) < 0) {
        va_end(args);
        return "";
    }
    va_end(args);
    return name;
}

} // namespace OHOS::Ace::StringUtils