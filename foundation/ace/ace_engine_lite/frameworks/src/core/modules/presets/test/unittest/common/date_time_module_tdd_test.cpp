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

#include "date_time_module_tdd_test.h"
#if (FEATURE_DATE_FORMAT == 1)
#include "global.h"
#include "js_app_context.h"
#include "js_app_environment.h"
namespace OHOS {
namespace ACELite {
void DateTimeModuleTddTest::SetUp()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        // should never happen
        return;
    }
    env->InitJsFramework();
}

void DateTimeModuleTddTest::TearDown()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    JsAppContext *context = JsAppContext::GetInstance();
    if (env == nullptr || context == nullptr) {
        // should never happen
        return;
    }
    context->ReleaseStyles();
    env->Cleanup();
}

jerry_value_t DateTimeModuleTddTest::RunJs(const char* js)
{
    const jerry_char_t *jsContent = reinterpret_cast<const jerry_char_t *>(js);
    jerry_length_t len = strlen(js);
    jerry_value_t result = jerry_eval(jsContent, len, JERRY_PARSE_NO_OPTS);
    return result;
}

char* DateTimeModuleTddTest::FormatDate(const jerry_value_t dateTime,
                                        const jerry_value_t args[],
                                        const jerry_length_t argsNum)
{
    EXPECT_FALSE(IS_UNDEFINED(dateTime));
    jerry_value_t format = jerryx_get_property_str(dateTime, "format");
    EXPECT_FALSE(IS_UNDEFINED(format));
    jerry_value_t result = CallJSFunction(format, dateTime, args, argsNum);
    char* formatRes = MallocStringOf(result);
    ReleaseJerryValue(format, result, VA_ARG_END_FLAG);
    return formatRes;
}

/**
 * @tc.name: DateTime_Locale_Test001
 * @tc.desc: Test the effective value of locale
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Locale_Test001, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    GLOBAL_ConfigLanguage("en-US");

    /**
     * @tc.steps : step1. set the system locale en-US, and create the
     * date time object
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US');");

    /**
     * @tc.steps: step2. set the format time 2020/9/28
     * @tc.expected: step2. the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 9, 4, 5)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3:set the formatter locale zh-CN and create the date time format object
     */
    dateTime = RunJs("new Intl.DateTimeFormat('zh-CN')");

    /**
     * @brief tc.steps:step4.get the format attribute and format the date
     */
    formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2020/10/28")));
    ACE_FREE(formatRes);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Locale_Test002
 * @tc.desc: Test the effective and invalid value of locale
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Locale_Test002, TestSize.Level1)
{
    TDD_CASE_BEGIN();
    GLOBAL_ConfigLanguage("en-US");

    /**
     * @tc.steps:step1:create the dateTime format object and set the locale info en
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en')");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28
     * @tc.expected:step2.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9,28);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) ||(strcmp(formatRes, "10/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTimeFormat object and set the locale info aaaa
     */
    dateTime = RunJs("new Intl.DateTimeFormat('aaaa');");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28
     * @tc.expected:step4.the format result is 10/28/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour12_Group_Test003
 * @tc.desc: test the hour12 style true, hour style numeric, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour12_Group_Test003, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour12 style true, the hour style numeric, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour12:true, hour:'numeric', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step2.the format result is 2:04:05 PM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 14, 4, 5);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style numeric, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                "{hour12:true, hour:'numeric', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step4.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style numeric, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:true, hour:'numeric', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step6.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style numeric, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', "
                    "{hour12:true, hour:'numeric', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step8.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour12_Group_Test004
 * @tc.desc: test the hour12 style true, hour style 2-digit, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour12_Group_Test004, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour12 style true, the hour style 2-digit, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour12:true, hour:'2-digit', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step2.the format result is 02:05:04 PM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 14, 5,4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "02:05:04 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style 2-digit, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:true, hour:'2-digit', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step4.the format result is 02:05:04 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "02:05:04 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style 2-digit, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                "{hour12:true, hour:'2-digit', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step6.the format result is 02:05:04 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "02:05:04 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour12 style true, the hour style numeric, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:true, hour:'2-digit', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step8.the format result is 02:05:04 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "02:05:04 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour12_FALSE_Test005
 * @tc.desc: test the hour12 style true, hour style numeric, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour12_False_Test_005, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour12 style false, the hour style numeric, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour12:false, hour:'numeric', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step2.the format result is 14:05:04
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 14, 5,4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style numeric, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'numeric', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step4.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style numeric, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'numeric', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step6.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style numeric, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'numeric', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step8.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour12_False_Test_006
 * @tc.desc: test the hour12 style true, hour style 2-digit, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour12_False_Test006, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour12 style false, the hour style 2-digit, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour12:false, hour:'2-digit', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step2.the format result is 14:05:04
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 14, 5,4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style 2-digit, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'2-digit', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step4.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style 2-digit, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'2-digit', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step6.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style 2-digit, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{hour12:false, hour:'2-digit', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step8.the format result is 14:05:04
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "14:05:04")));
    ace_free(formatRes);
    formatRes = nullptr;

    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour_Minute_Second_Test007
 * @tc.desc: test hour style numeric, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour_Minute_Second_Test007, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour style numeric, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour:'numeric', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step2.the format result is 2:04:05 PM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 14, 4, 5);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour style numeric, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step4.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour12 style false, the hour style numeric, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step6.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour style numeric, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 14:05:04
     * @tc.expected:step8.the format result is 2:04:05 PM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2:04:05 PM")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour_Minute_Second_Test_008
 * @tc.desc: hour style 2-digit, set different minute and second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour_Minute_Second_Test008, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info en-US,
     * the hour style 2-digit, minute style numeric and
     * second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{hour:'2-digit', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 08:05:04
     * @tc.expected:step2.the format result is 08:05:04 AM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 8, 5,4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * the hour style 2-digit, minute style numeric and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'2-digit', minute:'numeric', second:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 08:05:04
     * @tc.expected:step4.the format result is 08:05:04 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * the hour style 2-digit, minute style 2-digit and the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'2-digit', minute:'2-digit', second:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 08:05:04
     * @tc.expected:step6.the format result is 08:05:04 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * the hour style 2-digit, minute style 2-digit and the second style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'2-digit', minute:'2-digit', second:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 08:05:04
     * @tc.expected:step8.the format result is 08:05:04 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour_Test003
 * @tc.desc: test the different style group about hour and minute
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour_Minute_Test009, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the dateTime format object, set the locale info en-US,
     * set the hour style numeric, the minute style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', minute:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step2.the format result is 8:05 AM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 8, 5, 4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "8:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * set the hour style numeric, the minute style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', minute:'2-digit'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step4.the format result is 8:05 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "8:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the dateTime format object, set the locale info en-US,
     * set the hour style 2-digit, the minute style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'2-digit', minute:'numeric'});");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step6.the format result is 8:05 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step7.create the dateTime format object, set the locale info en-US,
     * set the hour style 2-digit, the minute style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'2-digit', minute:'2-digit'});");

    /**
     * @tc.steps:step8.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step8.the format result is 8:05 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "08:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Time_Invalid_Test010
 * @tc.desc: test the invalid time group
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Time_Invalid_Test010, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the dateTime format object, set the locale info en-US,
     * set the hour style numeric, the second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step2.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step2.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 8, 5, 4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info en-US,
     * set the minute style numeric, the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28 8:05:04
     * @tc.expected:step4.the format result is 10/28/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Weekday_Test011
 * @tc.desc: Test the different value of weekday
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Weekday_Test011, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object, set the weekday style 1111
     * and set the locale info en-US
     * @tc.expected:step1.create dateTime format object, the error info is the weekday style is out of range
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {weekday:'1111'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* formatRes = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "EvalError: the weekday style value is out of range")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step2.create the dateTime format object, set the weekday style narrow
     * and set the locale info en-US
     * @tc.expected:step2.create dateTime format object failed, the error info is the weekday style
     * is out range
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {weekday:'narrow'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    formatRes = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "EvalError: the weekday style value is out of range")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {weekday:'long'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28
     * @tc.expected:step4.the format result is Wednesday
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28);");
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Wednesday")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Test012
 * @tc.desc: Test the different value of year
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Year_Test012, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the dateTime object, set the year style 2-digit
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'short'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_TRUE((errorInfo != nullptr) &&
                (!strcmp(errorInfo, "EvalError: the year style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create date time object, set the locale info zh-CN
     * and set the year style 1111
     * @tc.expected:step2. the date time object create failed, the error info is
     * "RangeError: the year style is out of range"
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year : 1111});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the year style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step3.create dateTime format object, set the locale info en-US,
     * set the year style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric'});");

    /**
     * @tc.steps:step4.get the format attribute and format the date 2020/9/28
     * @tc.expected:step4.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28);");
    char* formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Month_Test013
 * @tc.desc: Test the enum value of month
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Month_Test013, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTimeFormat object, set the locale info zh-CN, and
     * the month style 1111
     * @tc.expected:
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {month:1111});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the month style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the dateTimeFormat object and set the month style narrow
     * @tc.expected: step2.create the dateTimeFormat object failed, the error info is
     * "RangeError:the month style is out of range"
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {month:'narrow'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the month style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, and set the locale info zh-CN,
     * set the month style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {month:'long'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28
     * @tc.expected:step4.the format result is October
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28);");
    char* formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "October")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Day_Test014
 * @tc.desc: Test the invalid value of day style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Day_Test014, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object, set the locale info zh-CN and
     * the day style 1111
     * @tc.expected:step1.create dateTime object failed, return the error info
     * "RangeError: the day style is out of range"
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {day: 11111})");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the day style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps: step2.create dateTime format object, set the locale info en-US,
     * and set the day style long
     * @tc.expected: step2.create the dateTime format object failed, the format result
     * is "RangeError: the day style is out of range"
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {day:'long'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the day style value is out of range")));
    ACE_FREE(errorInfo);

    /**
     * @tc.steps:step3.create the dateTime format object, and set the locale info zh-CN,
     * set the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {day:'numeric'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28
     * @tc.expected:step4.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28);");
    char* formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour_Test015
 * @tc.desc: Test the invalid value of month
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Hour_Test015, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object and set the locale info en-US,
     * set the hour style 1111
     * @tc.expected:step1.create dateTime format object failed, the error info is
     * the hour style value is out of range
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:1111});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the hour style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the dateTime format object failed, and set the locale info en-US,
     * the hour style long
     * @tc.expected:step2.create dateTime format failed, the error info is the hour style is out of range
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'long'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the hour style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, and set the locale info zh-CN,
     * set the hour style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {hour:'numeric'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28 8:05:04
     * @tc.expected:step4.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 8, 5, 4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}


/**
 * @tc.name: DateTime_Minute_Test016
 * @tc.desc: Test the invalid value of minute
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Minute_Test016, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object and set the locale info en-US,
     * set the minute style 1111
     * @tc.expected:step1.create dateTime format object failed, the error info is
     * the hour style value is out of range
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {minute:1111});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the minute style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the dateTime format object failed, and set the locale info en-US,
     * the minute style long
     * @tc.expected:step2.create dateTime format failed, the error info is the minute style is out of range
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {minute:'long'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the minute style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, and set the locale info zh-CN,
     * set the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {minute:'numeric'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28 8:05:04
     * @tc.expected:step4.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 8, 5, 4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Second_Test017
 * @tc.desc: Test the enum and invalid second style
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Second_Test017, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object and set the locale info en-US,
     * set the second style 1111
     * @tc.expected:step1.create dateTime format object failed, the error info is
     * the second style value is out of range
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {second:1111});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    jerry_value_t error = jerry_get_value_from_error(dateTime, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the second style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the dateTime format object failed, and set the locale info en-US,
     * the hour style long
     * @tc.expected:step2.create dateTime format failed, the error info is the hour style is out of range
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {second:'long'});");
    EXPECT_TRUE(jerry_value_is_error(dateTime));
    error = jerry_get_value_from_error(dateTime, true);
    errorInfo = MallocStringOf(error);
    EXPECT_FALSE((errorInfo == nullptr) || (strcmp(errorInfo, "EvalError: the second style value is out of range")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, and set the locale info en-US,
     * set the second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {second:'numeric'});");

    /**
     * @tc.steps:step4.format the date 2020/9/28 9:05:04
     * @tc.expected:step4.the format result is 10/28/20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28, 9, 5, 4);");
    char* formatRes = FormatDate(dateTime, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "10/28/20")));
    ReleaseJerryValue(args[0], dateTime, VA_ARG_END_FLAG);
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Hour_Test018
 * @tc.desc: Test the special locale info
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Special_Time_Test018, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create dateTime format object, set the locale info zh-Hans-HK
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('zh-Hans-HK');");

    /**
     * @tc.steps:step2.get the format attribute, and format the date 2020/9/28
     * @tc.expected:step2.the format result is 2020/10/28
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 9, 28);");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "28/10/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the dateTime format object, set the locale info zh-Hant-TW
     */
    dateTime = RunJs("new Intl.DateTimeFormat('zh-Hant-TW');");

    /**
     * @tc.steps:step4.get the format attribute, and format the date 2020/9/28
     * @tc.expected:step4.the format result is 2020/10/28
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2020/10/28")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps5.create the dateTime format object, set the locale info zh-Hant-CN
     */
    dateTime = RunJs("new Intl.DateTimeFormat('zh-Hant-CN');");

    /**
     * @tc.steps:step6.get the format attribute and format the date 2020/9/28
     * @tc.expected:step6.the format result is 2020/10/28
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "2020/10/28")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test019
 * @tc.desc: Test the different month style when year and day style is numeric
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Year_Month_Day_Test019, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style long, the day style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/28
     * @tc.expected: step2. the format result is October 28, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 28)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "June 28, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/28
     * @tc.expected: step4. the format result is Oct 28, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Jun 28, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/28
     * @tc.expected: step6. the format result is 6/28/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "6/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/28
     * @tc.expected: step8. the format result is 06/28/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "06/28/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test020
 * @tc.desc: Test the different month style when year style is numeric and day style is 2-digit
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Year_Month_Day_Test020, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style long, the day style 2-digit
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'long', day:'2-digit'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is June 04, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "June 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'short', day:'2-digit'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Jun 04, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Jun 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is 6/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "6/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'numeric', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is 06/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "06/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test021
 * @tc.desc: Test the different month style when year style is 2-digit and the day style is numeric
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Year_Month_Day_Test021, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is June 4, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "June 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style short, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Jun 4, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Jun 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style numeric, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is 6/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "6/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is 06/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "06/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test022
 * @tc.desc: Test the different month style when year style is 2-digit and the month style is 2-digit
 */
HWTEST_F(DateTimeModuleTddTest, DateTime_Year_Month_Day_Test022, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'long', day:'2-digit'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is June 04, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "June 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style short, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'short', day:'2-digit'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Jun 04, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Jun 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style numeric, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is 6/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "6/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is 06/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "06/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: Weekday_Year_Month_Day_Test023
 * @tc.desc: Test the different month style when year style is numeric and day style is 2-digit,
 * the weekday style is long
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test023, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style long, the day style 2-digit, set the weekday style long
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{year:'numeric', month:'long', day:'2-digit', weekday:'long'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thursday, June 04, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, June 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style 2-digit,
     * set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{year:'numeric', month:'short', day:'2-digit', weekday:'long'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thursday, Jun 04, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, Jun 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style 2-digit,
     * set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'numeric', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thursday, 6/04/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 6/04/2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style 2-digit,
     * set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'numeric', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thursday, 06/04/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 06/04/2020")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: Weekday_Year_Month_Day_Test024
 * @tc.desc: Test the different month style when year style is 2-digit, the day style is numeric,
 * and the weekday style long
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test024, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style numeric,
     * and set the weekday style long
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'long', year:'2-digit', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thursday, June 4, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, June 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style short, the day style numeric, and the weekday style is long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thursday, Jun 4, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, Jun 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style numeric, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thursday, 6/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 6/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thursday, 06/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 06/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test022
 * @tc.desc: Test the different month style when year style is 2-digit and the month style is 2-digit,
 * and set the weekday style long
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test025, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit,
     * and set the weekday style long
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'long', year:'2-digit', month:'long', day:'2-digit'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thursday, June 04, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, June 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'short', day:'2-digit'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thursday, Jun 04, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, Jun 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/28
     * @tc.expected: step6. the format result is Thursday, 6/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 6/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long', year:'2-digit', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thursday, 06/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 06/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test020
 * @tc.desc: Test the different month style when year style is numeric and day style is 2-digit,
 * set the weekday style long
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test026, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style long, the day style numeric,
     * and set the weekday style long
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'long', year:'numeric', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thursday, June 4, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, June 4, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long',year:'numeric', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thursday, Jun 4, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, Jun 4, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style numeric,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long',year:'numeric', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thursday, 6/4/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 6/4/2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style 2-digit,
     * and set the weekday style long
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'long',year:'numeric', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thursday, 06/4/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thursday, 06/4/2020")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test021
 * @tc.desc: Test the different month style when year style is 2-digit and the day style is numeric,
 * and set the weekday style long
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test027, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style long, the day style numeric,
     * and set the weekday style short
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'short', year:'numeric', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thu, June 4, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 4, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style numeric,
     * and set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thu, Jun 4, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, Jun 4, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style numeric,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/28
     * @tc.expected: step6. the format result is Thu, 6/4/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 6/4/2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style numeric,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thu, 06/4/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 06/4/2020")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test022
 * @tc.desc: Test the different month style when year style is 2-digit and the month style is 2-digit
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test028, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'short', year:'numeric', month:'long', day:'2-digit'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/28
     * @tc.expected: step2. the format result is October 28, 2020
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style short, the day style 2-digit,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'short', day:'2-digit'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thu, Jun 04, 2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, Jun 04, 2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style numeric, the day style 2-digit,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thu, 6/04/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 6/04/2020")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style numeric, the month style 2-digit, the day style 2-digit
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'numeric', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thu, 06/04/2020
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 06/04/2020")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test022
 * @tc.desc: Test the different month style when year style is 2-digit and the month style is 2-digit,
 * set the weekday style short
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test029, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit, the weekday style short
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'short', year:'2-digit', month:'long', day:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thu, June 4, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style short, the day style numeric,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'short', day:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thu, Jun 4, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, Jun 4, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style numeric, the day style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'numeric', day:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thu, 6/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 6/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style numeric,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'2-digit', day:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thu, 06/4/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 06/4/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: DateTime_Year_Month_Day_Test022
 * @tc.desc: Test the different month style when year style is 2-digit and the month style is 2-digit
 * and set the weekday style short
 */
HWTEST_F(DateTimeModuleTddTest, Weekday_Year_Month_Day_Test030, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit,
     * and set the weekday style short
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                                    "{weekday:'short', year:'2-digit', month:'long', day:'2-digit'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4
     * @tc.expected: step2. the format result is Thu, June 04, 20
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style short, the day style 2-digit,
     * and set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'short', day:'2-digit'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4
     * @tc.expected: step4. the format result is Thu, Jun 04, 20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, Jun 04, 20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style numeric, the day style 2-digit,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'numeric', day:'2-digit'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4
     * @tc.expected: step6. the format result is Thu, 6/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 6/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style 2-digit,
     * set the weekday style short
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US',"
                    "{weekday:'short', year:'2-digit', month:'2-digit', day:'2-digit'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4
     * @tc.expected: step8. the format result is Thu, 06/04/20
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, 06/04/20")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

/**
 * @tc.name: Date_Time_Pattern_Test031
 * @tc.desc: Test different date and time group test
 */
HWTEST_F(DateTimeModuleTddTest, Date_Time_Pattern_Test031, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps : step1. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit, set weekday style short,
     * the hour style numeric, minute style numeric and second style numeric
     */
    jerry_value_t dateTime = RunJs("new Intl.DateTimeFormat('en-US', {weekday:'short', year:'2-digit',"
                                "month:'long', day:'2-digit', hour:'numeric', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps: step2. set the format time 2020/5/4 8:05:04
     * @tc.expected: step2. the format result is Thu, June 04, 20 8:05:04 AM
     */
    jerry_value_t args[1];
    args[0] = RunJs("new Date(2020, 5, 4, 8, 5, 4)");
    char* formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 04, 20 8:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step3. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style long, the day style 2-digit,
     * hour style numeric and minute style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {weekday:'short', year:'2-digit', month:'long',"
                    "day:'2-digit', hour:'numeric', minute:'numeric'});");

    /**
     * @tc.steps: step4. set the format time 2020/5/4 8:05:04
     * @tc.expected: step4. the format result is Thu, June 04, 20 8:05 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "Thu, June 04, 20 8:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step5. create dateTime format object, set the locale info en-US,
     * set the year style , the month style numeric, the day style numeric, the hour style numeric,
     * the minute style numeric and second style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'long', day:'2-digit',"
                    "hour:'numeric', minute:'numeric', second:'numeric'});");

    /**
     * @tc.steps: step6. set the format time 2020/5/4 8:05:04
     * @tc.expected: step6. the format result is June 04, 20 8:05:04 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    jerry_release_value(dateTime);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "June 04, 20 8:05:04 AM")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps : step7. create dateTime format object, set the locale info en-US,
     * set the year style 2-digit, the month style 2-digit, the day style 2-digit,
     * hour style numeric and minute style numeric
     */
    dateTime = RunJs("new Intl.DateTimeFormat('en-US', {year:'2-digit', month:'2-digit',"
                    "day:'2-digit', hour:'numeric', minute:'numeric'});");

    /**
     * @tc.steps: step8. set the format time 2020/5/4 8:05:04
     * @tc.expected: step8. the format result is 06/04/20 8:05 AM
     */
    formatRes = FormatDate(dateTime, args, 1);
    ReleaseJerryValue(dateTime, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "06/04/20 8:05 AM")));
    ace_free(formatRes);
    formatRes = nullptr;
    TDD_CASE_END();
}

void DateTimeModuleTddTest::RunTests()
{
#ifndef TDD_ASSERTIONS
    DateTime_Locale_Test001();
    DateTime_Locale_Test002();
    DateTime_Hour12_Group_Test003();
    DateTime_Hour12_Group_Test004();
    DateTime_Hour12_False_Test_005();
    DateTime_Hour12_False_Test006();
    DateTime_Hour_Minute_Second_Test007();
    DateTime_Hour_Minute_Second_Test008();
    DateTime_Hour_Minute_Test009();
    DateTime_Time_Invalid_Test010();
    DateTime_Weekday_Test011();
    DateTime_Year_Test012();
    DateTime_Month_Test013();
    DateTime_Day_Test014();
    DateTime_Hour_Test015();
    DateTime_Minute_Test016();
    DateTime_Second_Test017();
    DateTime_Special_Time_Test018();
    DateTime_Year_Month_Day_Test019();
    DateTime_Year_Month_Day_Test020();
    DateTime_Year_Month_Day_Test021();
    DateTime_Year_Month_Day_Test022();
    Weekday_Year_Month_Day_Test023();
    Weekday_Year_Month_Day_Test024();
    Weekday_Year_Month_Day_Test025();
    Weekday_Year_Month_Day_Test026();
    Weekday_Year_Month_Day_Test027();
    Weekday_Year_Month_Day_Test028();
    Weekday_Year_Month_Day_Test029();
    Weekday_Year_Month_Day_Test030();
    Date_Time_Pattern_Test031();
#endif
}
}
}
#endif
