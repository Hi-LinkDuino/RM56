/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_STRING_UTIL_H
#define OHOS_ACELITE_STRING_UTIL_H

#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
namespace OHOS {
namespace ACELite {
class StringUtil {
public:
    static char *Copy(const char *sequence);
    static char *Malloc(const uint32_t size);
    static char *Slice(const char *sequence, const int32_t start);
    static char *Slice(const char *sequence, const int32_t start, const int32_t end);
    static bool StartsWith(const char *sequence, const char *subsequence);
    static char *Trim(char *sequence);
    static std::string TrimStr(const std::string& str, char cTrim = ' ');
    static void Str2map(const std::string& item, std::map<std::string, float>& kv);
    static void SplitStr(const std::string& str, const std::string& sep, std::map<std::string, float>& out, bool needTrim = true);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_STRING_UTIL_H
