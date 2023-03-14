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

#ifndef UNIT_DATETIME_TEST
#define UNIT_DATETIME_TEST

#include <string>
#include <vector>
#include <algorithm>
#include "locale_info.h"
#include "date_time_format.h"

namespace testing {
using namespace OHOS;
using namespace I18N;
using ::std::string;
using ::std::vector;
vector<LocaleInfo> g_locales = {
    LocaleInfo("ar", "", ""),       LocaleInfo("ar", "AE"),         LocaleInfo("ar", "EG"),
    LocaleInfo("as", "Latn", "IN"), LocaleInfo("de", "DE"),         LocaleInfo("en", "", ""),
    LocaleInfo("en", "GB"),         LocaleInfo("en", "Qaag", ""),   LocaleInfo("en", "US"),
    LocaleInfo("es", "ES"),         LocaleInfo("es", "US"),         LocaleInfo("fr", "FR"),
    LocaleInfo("it", "IT"),         LocaleInfo("ka", "GE"),         LocaleInfo("mai", "Deva", ""),
    LocaleInfo("my", "MM"),         LocaleInfo("pa", "Guru", ""),   LocaleInfo("pt", "BR"),
    LocaleInfo("pt", "PT"),         LocaleInfo("sr", "Cyrl", ""),   LocaleInfo("th", "TH"),
    LocaleInfo("zh", "", ""),       LocaleInfo("zh", "Hans", ""),   LocaleInfo("zh", "Hans", "CN"),
    LocaleInfo("zh", "Hans", "HK"), LocaleInfo("zh", "Hans", "MO"), LocaleInfo("zh", "Hant", "TW")
};


string g_formatAbbrDayNames[][7] = {
    { "الأحد", "الاثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت" },
    { "الأحد", "الاثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت" },
    { "الأحد", "الاثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت" },
    { "দেও", "সোম", "মঙ্গল", "বুধ", "বৃহ", "শুক্ৰ", "শনি" },
    { "So.", "Mo.", "Di.", "Mi.", "Do.", "Fr.", "Sa." },
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
    { "dom.", "lun.", "mar.", "mié.", "jue.", "vie.", "sáb." },
    { "dom.", "lun.", "mar.", "mié.", "jue.", "vie.", "sáb." },
    { "dim.", "lun.", "mar.", "mer.", "jeu.", "ven.", "sam." },
    { "dom", "lun", "mar", "mer", "gio", "ven", "sab" },
    { "კვი", "ორშ", "სამ", "ოთხ", "ხუთ", "პარ", "შაბ" },
    { "रवि", "सोम", "मंगल", "बुध", "बृहस्पति", "शुक्र", "शनि" },
    { "တနင်္ဂနွေ", "တနင်္လာ", "အင်္ဂါ", "ဗုဒ္ဓဟူး", "ကြာသပတေး", "သောကြာ", "စနေ" },
    { "ਐਤ", "ਸੋਮ", "ਮੰਗਲ", "ਬੁੱਧ", "ਵੀਰ", "ਸ਼ੁੱਕਰ", "ਸ਼ਨਿੱਚਰ" },
    { "dom", "seg", "ter", "qua", "qui", "sex", "sáb" },
    { "dom", "seg", "ter", "qua", "qui", "sex", "sáb" },
    { "нед", "пон", "уто", "сре", "чет", "пет", "суб" },
    { "อา.", "จ.", "อ.", "พ.", "พฤ.", "ศ.", "ส." },
    { "周日", "周一", "周二", "周三", "周四", "周五", "周六" },
    { "周日", "周一", "周二", "周三", "周四", "周五", "周六" },
    { "周日", "周一", "周二", "周三", "周四", "周五", "周六" },
    { "周日", "周一", "周二", "周三", "周四", "周五", "周六" },
    { "周日", "周一", "周二", "周三", "周四", "周五", "周六" },
    { "週日", "週一", "週二", "週三", "週四", "週五", "週六" }
};
} // namespace testing

#endif // UNIT_DATETIME_TEST
