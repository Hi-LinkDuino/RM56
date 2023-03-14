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
#ifndef DATE_TIME_MODULE_TDD_TEST_H
#define DATE_TIME_MODULE_TDD_TEST_H
#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "acelite_config.h"
#if (FEATURE_DATE_FORMAT == 1)
#include "test_common.h"
#include "js_fwk_common.h"
namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class DateTimeModuleTddTest : public testing::Test {
#else
class DateTimeModuleTddTest {
#endif
public:
    void RunTests();

    void SetUp();

    void TearDown();

    void DateTime_Locale_Test001();

    void DateTime_Locale_Test002();

    void DateTime_Hour12_Group_Test003();

    void DateTime_Hour12_Group_Test004();

    void DateTime_Hour12_False_Test_005();

    void DateTime_Hour12_False_Test006();

    void DateTime_Hour_Minute_Second_Test007();

    void DateTime_Hour_Minute_Second_Test008();

    void DateTime_Hour_Minute_Test009();

    void DateTime_Time_Invalid_Test010();

    void DateTime_Weekday_Test011();

    void DateTime_Year_Test012();

    void DateTime_Month_Test013();

    void DateTime_Day_Test014();

    void DateTime_Hour_Test015();

    void DateTime_Minute_Test016();

    void DateTime_Second_Test017();

    void DateTime_Special_Time_Test018();

    void DateTime_Year_Month_Day_Test019();

    void DateTime_Year_Month_Day_Test020();

    void DateTime_Year_Month_Day_Test021();

    void DateTime_Year_Month_Day_Test022();

    void Weekday_Year_Month_Day_Test023();

    void Weekday_Year_Month_Day_Test024();

    void Weekday_Year_Month_Day_Test025();

    void Weekday_Year_Month_Day_Test026();

    void Weekday_Year_Month_Day_Test027();

    void Weekday_Year_Month_Day_Test028();

    void Weekday_Year_Month_Day_Test029();

    void Weekday_Year_Month_Day_Test030();

    void Date_Time_Pattern_Test031();

    jerry_value_t RunJs(const char* js);

    char* FormatDate(const jerry_value_t dateTime, const jerry_value_t args[], const jerry_length_t argsNum);
};
}
}
#endif // FEATURE_DATE_FORMAT
#endif // DATE_TIME_MODULE_TDD_TEST_H
