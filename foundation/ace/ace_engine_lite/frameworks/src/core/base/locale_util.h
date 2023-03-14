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

#ifndef OHOS_ACELITE_LOCALE_UTIL_H
#define OHOS_ACELITE_LOCALE_UTIL_H
#include "acelite_config.h"
#if (FEATURE_INTL_MODULE == 1)
#include "locale_info.h"
#include "non_copyable.h"
namespace OHOS {
namespace ACELite {
class LocaleUtil final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LocaleUtil);
    LocaleUtil() = delete;
    ~LocaleUtil() = delete;

    /**
     * @brief split the locale info by '-', set the language, region, and script info
     * @param locale the locale string
     * @return the localeInfo object which has set the language, region and script
     */
    static I18N::LocaleInfo *GetLocaleInfo(char *locale);

    /**
     * @brief get the system language and region info
     * @return the locale object which has set the language and region info
     */
    static I18N::LocaleInfo *GetSystemLocaleInfo();
};
}
}
#endif // FEATURE_INTL_MODULE
#endif
