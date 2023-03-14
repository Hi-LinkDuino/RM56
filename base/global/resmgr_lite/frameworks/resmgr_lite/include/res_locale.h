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

#ifndef OHOS_RESOURCE_MANAGER_RES_LOCALE_H
#define OHOS_RESOURCE_MANAGER_RES_LOCALE_H

#include <cstdint>
#include <cstddef>
#include "rstate.h"
#include "lock.h"
#include "locale_info.h"

using OHOS::I18N::LocaleInfo;
namespace OHOS {
namespace Global {
namespace Resource {
struct ParseResult {
    const char *tempLanguage = nullptr;
    const char *tempScript = nullptr;
    const char *tempRegion = nullptr;
    int16_t languageTagLen = 0;
    int16_t scriptTagLen = 0;
    int16_t regionTagLen = 0;
};

class ResLocale {
public:
    const char *GetLanguage() const;

    const char *GetRegion() const;

    const char *GetScript() const;

    ResLocale();

    /**
     * Copy from other LocaleInfo to this
     * @param other the other LocaleInfo copy to this localeInfo
     * @return SUCCESS if copy other LocaleInfo success, else ERROR
     */
    RState CopyFromLocaleInfo(const LocaleInfo *other);

    /**
     * Copy from other ResLocale to this
     * @param other the other ResLocale copy to this ResLocale
     * @return SUCCESS if copy other ResLocale success, else ERROR
     */
    RState Copy(const ResLocale *other);

    static const LocaleInfo *GetDefault();

    static bool UpdateDefault(const LocaleInfo &localeInfo, bool needNotify);

    /**
     * Build resLocal from string
     * @param bcp47String the target string
     * @param sep the parse string position
     * @param rState the parse status
     * @return the resLocal after parse bcp47String
     */
    static ResLocale *BuildFromString(const char *bcp47String, char sep, RState &rState);

    /**
     * Build resLocal from parts
     * @param language the resLocal language
     * @param script the resLocal script
     * @param region the resLocal region
     * @param rState the Build status
     * @return the resLocal after Build resLocal from parts if success, else return nullptr
     */
    static ResLocale *BuildFromParts(const char *language, const char *script, const char *region, RState &rState);

    ~ResLocale();

    static constexpr uint16_t END_TYPE = 0x0000;
    // language parse
    static constexpr uint16_t LANG_TYPE = 0x0001;
    // script parse
    static constexpr uint16_t SCRIPT_TYPE = 0x0002;
    // region parse
    static constexpr uint16_t REGION_TYPE = 0x0004;

    static constexpr size_t MIN_BCP47_STR_LEN = 2;

private:
    RState SetLanguage(const char *language, size_t len);

    RState SetScript(const char *script, size_t len);

    RState SetRegion(const char *region, size_t len);

    static ResLocale *DoParse(const char *bcp47String, char sep, RState &rState);

    static ResLocale *CreateResLocale(ParseResult &parseResult, RState &rState);

    RState Init(const char *language, size_t languageLen, const char *script, size_t scriptLen,
        const char *region, size_t regionLen);

    const char *language_;

    const char *region_;

    const char *script_;

    static LocaleInfo *defaultLocale_;

    static Lock lock_;

    friend class LocaleMatcher;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
