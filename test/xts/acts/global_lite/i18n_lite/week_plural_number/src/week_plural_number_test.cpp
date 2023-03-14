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

#include "week_plural_number_test.h"
#include <gtest/gtest.h>
#include "date_time_format.h"
#include "iostream"
#include "locale_info.h"
#include "number_format.h"
#include "plural_format.h"
#include "types.h"
#include "week_info.h"

namespace OHOS {
using namespace std;
using namespace testing::ext;
using namespace OHOS::I18N;

class WeekPluralNumberTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
};

void WeekPluralNumberTest::SetUp()
{
}

void WeekPluralNumberTest::TearDown()
{
}

/**
 * @tc.number: WeekPluralNumberTest0100
 * @tc.name: Test PluralFormat GetPluralRuleIndex function with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0100, Function | MediumTest | Level3)
{
    LocaleInfo locale("zh", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);
    double number = 2.1;
    int out = formatter.GetPluralRuleIndex(number, status);
    int expect = PluralRuleType::OTHER;
    EXPECT_TRUE(expect == out);
    number = 10.10;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 100.1234567;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
}

/**
 * @tc.number: WeekPluralNumberTest0200
 * @tc.name: Test PluralFormat GetPluralRuleIndex function with uk locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0200, Function | MediumTest | Level3)
{
    LocaleInfo locale("uk", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);
    double number = 3.3;
    int out = formatter.GetPluralRuleIndex(number, status);
    int expect = PluralRuleType::OTHER;
    EXPECT_TRUE(expect == out);
    number = 10.10;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.7654321;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
}

/**
 * @tc.number: WeekPluralNumberTest0300
 * @tc.name: Test PluralFormat GetPluralRuleIndex function with sr locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0300, Function | MediumTest | Level3)
{
    LocaleInfo locale("sr", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);
    double number = 1.10;
    int out = formatter.GetPluralRuleIndex(number, status);
    int expect = PluralRuleType::ONE;
    EXPECT_TRUE(expect == out);
    number = 1.1210;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.2;
    out = formatter.GetPluralRuleIndex(number, status);
    expect = PluralRuleType::FEW;
    EXPECT_TRUE(expect == out);
    number = 1.20;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.3;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.40;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.22;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.340;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.11;
    out = formatter.GetPluralRuleIndex(number, status);
    expect = PluralRuleType::OTHER;
    EXPECT_TRUE(expect == out);
    number = 1.120;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.13;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 1.140;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
}

/**
 * @tc.number: WeekPluralNumberTest0400
 * @tc.name: Test PluralFormat GetPluralRuleIndex function with lv locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0400, Function | MediumTest | Level3)
{
    LocaleInfo locale("lv", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);
    double number = 1.140;
    int out = formatter.GetPluralRuleIndex(number, status);
    int expect = PluralRuleType::ZERO;
    EXPECT_TRUE(expect == out);
    number = 11.0;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
}

/**
 * @tc.number: WeekPluralNumberTest0500
 * @tc.name: Test PluralFormat GetPluralRuleIndex function with ar locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0500, Function | MediumTest | Level3)
{
    LocaleInfo locale("ar", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);
    double number = 2.0;
    int out = formatter.GetPluralRuleIndex(number, status);
    int expect = PluralRuleType::TWO;
    EXPECT_TRUE(expect == out);
    number = 2.0;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 2.00;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    number = 2.000;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
    expect = PluralRuleType::MANY;
    number = 1011.0;
    out = formatter.GetPluralRuleIndex(number, status);
    EXPECT_TRUE(expect == out);
}                                                                                                           

/**
 * @tc.number: WeekPluralNumberTest0600
 * @tc.name: Test LocaleInfo ForLanguageTag with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0600, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale = LocaleInfo::ForLanguageTag("zh-Hant-CN-u-nu-arab", status);
    EXPECT_TRUE(status == I18nStatus::ISUCCESS);
}

/**
 * @tc.number: WeekPluralNumberTest0700
 * @tc.name: Test LocaleInfo ForLanguageTag with ko locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0700, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale = LocaleInfo::ForLanguageTag("ko-Hani-KR-u-nu-arab", status);
    EXPECT_TRUE(status == I18nStatus::ISUCCESS);
}

/**
 * @tc.number: WeekPluralNumberTest0800
 * @tc.name: Test LocaleInfo GetExtension with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0800, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale = LocaleInfo::ForLanguageTag("zh-Hant-CN-u-nu-arab", status);
    EXPECT_TRUE(status == I18nStatus::ISUCCESS);
    const char *numberDigits = locale.GetExtension("nu");
    EXPECT_TRUE(numberDigits != nullptr);
    EXPECT_TRUE(strcmp("arab", numberDigits) == 0);
    int status_v2 = 0;
    NumberFormat formatter(locale, status_v2);
    int intNum1 = 1234567;
    std::string out = formatter.Format(intNum1, status_v2);
    std::cout << "out: " << out << std::endl;
    EXPECT_TRUE(out == "١٬٢٣٤٬٥٦٧");
}

/**
 * @tc.number: WeekPluralNumberTest0900
 * @tc.name: Test LocaleInfo GetExtension with ko locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest0900, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale = LocaleInfo::ForLanguageTag("ko-Hani-KR-u-nu-arab", status);
    EXPECT_TRUE(status == I18nStatus::ISUCCESS);
    const char *numberDigits = locale.GetExtension("nu");
    EXPECT_TRUE(numberDigits != nullptr);
    EXPECT_TRUE(strcmp("arab", numberDigits) == 0);
    int status_v2 = 0;
    NumberFormat formatter(locale, status_v2);
    int intNum1 = 1234567;
    std::string out = formatter.Format(intNum1, status_v2);
    std::cout << "out: " << out << std::endl;
    EXPECT_TRUE(out == "١٬٢٣٤٬٥٦٧");
}

/**
 * @tc.number: WeekPluralNumberTest1000
 * @tc.name: Test WeekInfo constructor with zh language
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1000, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("zh", "CN");
    WeekInfo weekInfo(locale, status);
    EXPECT_TRUE(status == I18nStatus::ISUCCESS);
}

/**
 * @tc.number: WeekPluralNumberTest1100
 * @tc.name: Test WeekInfo GetFirstDayOfWeek with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1100, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("zh", "CN");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetFirstDayOfWeek();
    EXPECT_TRUE(ret == 1);
}

/**
 * @tc.number: WeekPluralNumberTest1200
 * @tc.name: Test WeekInfo GetFirstDayOfWeek with en locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1200, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("en", "US");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetFirstDayOfWeek();
    EXPECT_TRUE(ret == 1);
}

/**
 * @tc.number: WeekPluralNumberTest1300
 * @tc.name: Test WeekInfo GetMinimalDaysInFirstWeek with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1300, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("zh", "CN");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetMinimalDaysInFirstWeek();
    EXPECT_TRUE(ret == 1);
}

/**
 * @tc.number: WeekPluralNumberTest1400
 * @tc.name: Test WeekInfo GetMinimalDaysInFirstWeek with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1400, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("en", "US");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetMinimalDaysInFirstWeek();
    EXPECT_TRUE(ret == 1);
}

/**
 * @tc.number: WeekPluralNumberTest1500
 * @tc.name: Test WeekInfo GetFirstDayOfWeekend with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1500, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("zh", "CN");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetFirstDayOfWeekend();
    EXPECT_TRUE(ret == 7);
}

/**
 * @tc.number: WeekPluralNumberTest1600
 * @tc.name: Test WeekInfo GetFirstDayOfWeekend with en locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1600, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("en", "US");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetFirstDayOfWeekend();
    EXPECT_TRUE(ret == 7);
}

/**
 * @tc.number: WeekPluralNumberTest1700
 * @tc.name: Test WeekInfo GetLastDayOfWeekend with zh locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1700, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("zh", "CN");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetLastDayOfWeekend();
    EXPECT_TRUE(ret == 1);
}

/**
 * @tc.number: WeekPluralNumberTest1800
 * @tc.name: Test WeekInfo GetLastDayOfWeekend with en locale
 */
HWTEST_F(WeekPluralNumberTest, WeekPluralNumberTest1800, Function | MediumTest | Level3)
{
    I18nStatus status = I18nStatus::ISUCCESS;
    LocaleInfo locale("en", "US");
    WeekInfo weekInfo(locale, status);
    uint8_t ret = weekInfo.GetLastDayOfWeekend();
    EXPECT_TRUE(ret == 1);
}
}