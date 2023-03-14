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
#ifndef OHOS_RESOURCE_MANAGER_LOCALEMATCHER_H
#define OHOS_RESOURCE_MANAGER_LOCALEMATCHER_H
#include "res_locale.h"

namespace OHOS {
namespace Global {
namespace Resource {
class LocaleMatcher {
public:
    static int8_t IsMoreSuitable(const ResLocale *current,
        const ResLocale *other,
        const ResLocale *request);

    /**
     * Whether the current ResLocale same to the other ResLocale
     * @param current the current ResLocale
     * @param other the other ResLocale
     * @return true if the current ResLocale same to the other ResLocale, else false
     */
    static bool Match(const ResLocale *current, const ResLocale *other);

    /**
     * Whether the string is the language tag
     * @param str the tag string
     * @param len the tag length
     * @return true if the str is language tag, else false
     */
    static bool IsLanguageTag(const char *str, int32_t len);

    /**
     * Whether the string is the script tag
     * @param str the tag string
     * @param len the tag length
     * @return true if the str is script tag, else false
     */
    static bool IsScriptTag(const char *str, int32_t len);

    /**
     * Whether the string is the Region tag
     * @param str the tag string
     * @param len the tag length
     * @return true if the str is Region tag, else false
     */
    static bool IsRegionTag(const char *str, int32_t len);

    /**
     * Get the resLocale script
     * @param resLocale the resLocale infomation
     * @return true if get the resLocale script, else false
     */
    static bool Normalize(ResLocale *resLocale);

    /**
     * Whether the current resLocale more specific than target resLocale
     * @param current the current resLocale
     * @param target the target resLocale
     * @return 0 means current resLocale same to target resLocale,
     *         1 means current resLocale more specific than target resLocale, else -1
     */
    static int8_t IsMoreSpecificThan(const ResLocale *current, const ResLocale *target);

public:
    static uint64_t EN_US_ENCODE;
    static uint64_t EN_GB_ENCODE;
    static uint64_t EN_QAAG_ENCODE;
    static uint64_t ZH_HANT_MO_ENCODE;
    static uint64_t ZH_HK_ENCODE;
    static uint32_t HANT_ENCODE;
    static constexpr uint64_t ROOT_LOCALE = 0x0;
    static constexpr uint16_t NULL_LANGUAGE = 0x0;
    static constexpr uint16_t NULL_REGION = 0x0;
    static constexpr uint16_t NULL_SCRIPT = 0x0;
    static constexpr uint64_t NULL_LOCALE = 0x0;
    static constexpr uint8_t TRACKPATH_ARRAY_SIZE = 5;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
