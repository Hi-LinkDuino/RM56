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

#include "locale_module.h"
#include "global.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
const char * const LocaleModule::TEXT_DIRECTION_LTR = "ltr";
// caller must free the returned value by this function
static char *GetLanguage(void)
{
    char *lang = reinterpret_cast<char *>(ace_malloc(MAX_LANGUAGE_LENGTH));
    if (lang == nullptr) {
        return nullptr;
    }
    (void)memset_s(lang, MAX_LANGUAGE_LENGTH, 0x0, MAX_LANGUAGE_LENGTH);
    if (GLOBAL_GetLanguage(lang, MAX_LANGUAGE_LENGTH) != 0) {
        ace_free(lang);
        return nullptr;
    }
    return lang;
}

// caller must free the returned value by this function
static char *GetRegion(void)
{
    char *region = reinterpret_cast<char *>(ace_malloc(MAX_REGION_LENGTH));
    if (region == nullptr) {
        return nullptr;
    }
    (void)memset_s(region, MAX_REGION_LENGTH, 0x0, MAX_REGION_LENGTH);
    if (GLOBAL_GetRegion(region, MAX_REGION_LENGTH) != 0) {
        ace_free(region);
        return nullptr;
    }
    return region;
}

static const char *GetTextDirection(void)
{
    return LocaleModule::TEXT_DIRECTION_LTR;
}

void InitLocaleModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "getLocale", LocaleModule::GetLocale);
}

JSIValue LocaleModule::GetLocale(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue result = JSI::CreateObject();
    char *lang = GetLanguage();
    if (lang == nullptr) {
        JSI::ReleaseValue(result);
        return JSI::CreateUndefined();
    }
    JSI::SetStringProperty(result, "language", lang);
    ace_free(lang);

    char *region = GetRegion();
    if (region == nullptr) {
        JSI::ReleaseValue(result);
        return JSI::CreateUndefined();
    }
    JSI::SetStringProperty(result, "countryOrRegion", region);
    ace_free(region);

    const char *dir = GetTextDirection();
    if (dir == nullptr) {
        JSI::ReleaseValue(result);
        return JSI::CreateUndefined();
    }
    JSI::SetStringProperty(result, "dir", dir);
    return result;
}
} // namespace ACELite
} // namespace OHOS
