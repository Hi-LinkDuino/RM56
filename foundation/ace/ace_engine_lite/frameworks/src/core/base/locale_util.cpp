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

#include "locale_util.h"
#if (FEATURE_INTL_MODULE == 1)
#include "ace_log.h"
#include "global.h"
#include "js_fwk_common.h"
#include "memory_heap.h"
#include "securec.h"
#include "string.h"

namespace OHOS {
namespace ACELite {
using namespace I18N;
LocaleInfo *LocaleUtil::GetLocaleInfo(char* locale)
{
    if (locale == nullptr) {
        return GetSystemLocaleInfo();
    }
    LocaleInfo *localeInfo = nullptr;
    // the locale info is used '-' as separator
    const char *delemeter = "-";
    char *country = nullptr;
    char *next = nullptr;
    if (strstr(locale, delemeter)) {
        country = strtok_s(locale, delemeter, &next);
    }
    if (country == nullptr) {
        localeInfo = new LocaleInfo(locale, nullptr);
    } else {
        char *origion = strtok_s(nullptr, delemeter, &next);
        if ((next == nullptr) || (!strcmp("", next))) {
            // set the language and region info
            localeInfo = new LocaleInfo(country, origion);
        } else {
            // set the language, region and script info
            localeInfo = new LocaleInfo(country, origion, next);
        }
    }
    // if the locale info is nullptr or set locale info failed, set the system locale info
    if ((localeInfo == nullptr) || (localeInfo->GetId() == nullptr)) {
        ACE_DELETE(localeInfo);
        localeInfo = GetSystemLocaleInfo();
    }
    return localeInfo;
}

LocaleInfo *LocaleUtil::GetSystemLocaleInfo()
{
    LocaleInfo *info = nullptr;
    char *language = nullptr;
    char *region = nullptr;
    do {
        language = reinterpret_cast<char *>(ace_malloc(MAX_LANGUAGE_LENGTH));
        region = reinterpret_cast<char *>(ace_malloc(MAX_REGION_LENGTH));
        if ((language == nullptr) || (region == nullptr)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc system info failed");
            break;
        }
        int langRet = GLOBAL_GetLanguage(language, MAX_LANGUAGE_LENGTH);
        int regionRet = GLOBAL_GetRegion(region, MAX_REGION_LENGTH);
        if ((langRet != 0) || (regionRet != 0)) {
            HILOG_ERROR(HILOG_MODULE_ACE, "get system language info failed");
            break;
        }
        info = new LocaleInfo(language, region);
    } while (0);
    ACE_FREE(language);
    ACE_FREE(region);
    return info;
}
} // namespace ACELite
} // namespace OHOS
#endif
