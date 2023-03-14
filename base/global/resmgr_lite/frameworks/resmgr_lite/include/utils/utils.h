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
#ifndef OHOS_RESOURCE_MANAGER_UTILS_H
#define OHOS_RESOURCE_MANAGER_UTILS_H
#include <cstddef>
#include <cstdint>
#include "res_locale.h"
#include "rstate.h"

namespace OHOS {
namespace Global {
namespace Resource {
class Utils {
public:
    static bool IsAlphaString(const char *s, int32_t len);

    static bool IsNumericString(const char *s, int32_t len);

    static bool IsStrEmpty(const char *s);

    static size_t StrLen(const char *s);

    static uint16_t EncodeLanguage(const char *language);

    static uint16_t EncodeLanguageByResLocale(const ResLocale *locale);

    static uint32_t EncodeScript(const char *script);

    static uint32_t EncodeScriptByResLocale(const ResLocale *locale);

    static void DecodeScript(uint32_t encodeScript, char *outValue);

    static uint16_t EncodeRegion(const char *region);

    static uint16_t EncodeRegionByResLocale(const ResLocale *locale);

    static uint64_t EncodeLocale(const char *language,
                                 const char *script,
                                 const char *region);

    static RState ConvertColorToUInt32(const char *s, uint32_t &outValue);

private:

    static uint16_t EncodeLanguageOrRegion(const char *str, char base);

    static bool StrCompare(const char *left, const char *right, size_t len, bool isCaseSensitive);
    static constexpr uint64_t ROOT_LOCALE = 0x0;
    static constexpr uint16_t NULL_LANGUAGE = 0x00;
    static constexpr uint16_t NULL_REGION = 0x00;
    static constexpr uint16_t NULL_SCRIPT = 0x0000;
    static constexpr uint64_t NULL_LOCALE = 0x0;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif