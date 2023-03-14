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

#include <climits>
#include <gtest/gtest.h>
#include "UnitDateTimeRecallTest.h"

using namespace std;
using namespace testing::ext;
namespace OHOS {
using namespace I18N;
using namespace testing;

class DateTimeTest : public ::testing::TestWithParam<LocaleInfo> {};

class DateTimeLocaleRecallTest : public testing::Test {
public:
    static const int INDEX_NEG_ONE = -1;

public:
    static const int INDEX_MAX_WEEK = 7;

public:
    static const int INDEX_MAX_MONTH = 12;
};

class DateTimePreformanceTest : public testing::Test {
public:
    static const int LOOP_NUM = 1000;

public:
    static constexpr float BASELINE = 0.5;
};

/* *
 * get locale index
 *
 * expectResult return locale index
 */
static int GetLocaleIndex(LocaleInfo locale, vector<LocaleInfo> locales)
{
    for (unsigned int index = 0; index < locales.size(); index++) {
        if (locale == locales[index]) {
            return index;
        }
    }
    return -1;
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_DATETIMEFORMAT_LOCALE_0100
 * @tc.name     test datetime locale recall,locale is LocaleInfo en,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DateTimeLocaleRecallTest, GLOBAL_DateTimeFormat_LocaleRecall_0100, Function | MediumTest | Level0)
{
    LocaleInfo *locale = new LocaleInfo("en", "ZZ");
    DateTimeFormat *datetime = new DateTimeFormat(HOUR12_MINUTE_SECOND, *locale);
    std::string formatWeekName = datetime->GetWeekName(0, DateTimeDataType::FORMAT_ABBR);
    LocaleInfo *localeRecall = new LocaleInfo("en", "", "US");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(formatWeekName, g_formatAbbrDayNames[localeIndex][0]);
    }
    delete localeRecall;
    delete datetime;
    delete locale;
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_DATETIMEFORMAT_LOCALE_0200
 * @tc.name     test datetime locale recall,locale is LocaleInfo zh,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DateTimeLocaleRecallTest, GLOBAL_DateTimeFormat_LocaleRecall_0200, Function | MediumTest | Level0)
{
    LocaleInfo *locale = new LocaleInfo("zh", "ZZ");
    DateTimeFormat *datetime = new DateTimeFormat(HOUR12_MINUTE_SECOND, *locale);
    std::string formatWeekName = datetime->GetWeekName(0, DateTimeDataType::FORMAT_ABBR);
    LocaleInfo *localeRecall = new LocaleInfo("zh", "", "");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(formatWeekName, g_formatAbbrDayNames[localeIndex][0]) << "index: " << localeIndex << std::endl;
    }
    delete localeRecall;
    delete datetime;
    delete locale;
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_DATETIMEFORMAT_LOCALE_0300
 * @tc.name     test datetime locale recall,locale is LocaleInfo zh,Hans,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DateTimeLocaleRecallTest, GLOBAL_DateTimeFormat_LocaleRecall_0300, Function | MediumTest | Level0)
{
    LocaleInfo *locale = new LocaleInfo("zh", "Hans", "ZZ");
    DateTimeFormat *datetime = new DateTimeFormat(HOUR12_MINUTE_SECOND, *locale);
    std::string formatWeekName = datetime->GetWeekName(0, DateTimeDataType::FORMAT_ABBR);
    LocaleInfo *localeRecall = new LocaleInfo("zh", "Hans", "");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(formatWeekName, g_formatAbbrDayNames[localeIndex][0]) << "index: " << localeIndex << std::endl;
    }
    delete localeRecall;
    delete datetime;
    delete locale;
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_DATETIMEFORMAT_LOCALE_0400
 * @tc.name     test datetime locale recall,locale is LocaleInfo zz
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(DateTimeLocaleRecallTest, GLOBAL_DateTimeFormat_LocaleRecall_0400, Function | MediumTest | Level0)
{
    LocaleInfo *locale = new LocaleInfo("zz", "");
    DateTimeFormat *datetime = new DateTimeFormat(HOUR12_MINUTE_SECOND, *locale);
    std::string formatWeekName = datetime->GetWeekName(0, DateTimeDataType::FORMAT_ABBR);
    LocaleInfo *localeRecall = new LocaleInfo("en", "US");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(formatWeekName, g_formatAbbrDayNames[localeIndex][0]) << "index: " << localeIndex << std::endl;
    }
    delete localeRecall;
    delete datetime;
    delete locale;
}
} // namespace OHOS