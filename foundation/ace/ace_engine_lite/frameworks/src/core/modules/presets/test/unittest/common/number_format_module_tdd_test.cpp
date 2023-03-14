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

#include "number_format_module_tdd_test.h"
#if (FEATURE_NUMBER_FORMAT == 1)
#include "global.h"
#include "js_app_context.h"
#include "js_app_environment.h"
namespace OHOS {
namespace ACELite {
void NumberFormatModuleTddTest::SetUp()
{
    JsAppEnvironment *env = JsAppEnvironment::GetInstance();
    if (env == nullptr) {
        // should never happen
        return;
    }
    env->InitJsFramework();
}

void NumberFormatModuleTddTest::TearDown()
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

jerry_value_t NumberFormatModuleTddTest::RunJs(const char* js)
{
    const jerry_char_t *jsContent = reinterpret_cast<const jerry_char_t *>(js);
    jerry_length_t len = strlen(js);
    jerry_value_t result = jerry_eval(jsContent, len, JERRY_PARSE_NO_OPTS);
    return result;
}

char* NumberFormatModuleTddTest::FormatNumber(const jerry_value_t numberFormat,
                                              const jerry_value_t *args,
                                              const jerry_length_t argsNum)
{
    if (IS_UNDEFINED(numberFormat)) {
        printf("create dateTime failed\n");
        return nullptr;
    }
    jerry_value_t format = jerryx_get_property_str(numberFormat, "format");
    if (IS_UNDEFINED(format)) {
        printf("get the format attribute failed\n");
        jerry_release_value(numberFormat);
        return nullptr;
    }
    jerry_value_t result = CallJSFunction(format, numberFormat, args, argsNum);
    char* formatRes = MallocStringOf(result);
    ReleaseJerryValue(format, result, VA_ARG_END_FLAG);
    return formatRes;
}

/**
 * @tc.name: NumberFormat_Locale_Test001
 * @tc.desc: test the locale info support
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Locale_Test001, TestSize.Level0)
{
    TDD_CASE_BEGIN();
    GLOBAL_ConfigLanguage("zh-CN");

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-u-nu-hanidec
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-Hans-CN')");

    /**
     * @tc.steps:step2.get the format attribute and format the number 9
     * @tc.expected:step2.the format result is 9
     */
    jerry_value_t args[1];
    int formatNum = 9;
    args[0] = jerry_create_number(formatNum);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "9")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the number format object
     */
    numberFormat = RunJs("new Intl.NumberFormat();");

    /**
     * @tc.steps:step4.get the format attribute and format the number 9
     * @tc.expected:step4.the format result is 9
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "9")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the number format object and set the locale info testaaaa
     */
    numberFormat = RunJs("new Intl.NumberFormat('testaaaa');");

    /**
     * @tc.steps:step6.get the format attribute and format the number 9
     * @tc.expected:step6.the format result is 9
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "9")));
    ReleaseJerryValue(args[0], numberFormat, VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Style_Test002
 * @tc.desc: test the enum style value
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Style_Test002, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @steps:step1.create the number format object, set the locale info zh-CN
     * and set the style decimal
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'decimal'});");

    /**
     * @steps:step2.get the format attribute and format the number 0.9
     * @expected:step2.the format result is 0.9
     */
    jerry_value_t args[1];
    const double num = 0.9;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "0.9")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the locale info zh-CN
     * and the style percent
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent'});");

    /**
     * @tc.steps:step4.get the format attribute and format the number 0.9
     * @tc.expected:step4.the format result is 90%
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "90%")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the numberFormat object, set the locale info zh-CN
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN');");

    /**
     * @tc.steps:step6.get the format attribute and format the number 0.9
     * @tc.expected:step6.the format result is 0.9
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "0.9")));
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Style_Test003
 * @tc.desc: test the invalid value of style
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Style_Test003, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object and set the style percent
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat({style:'percent'});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 0.9
     * @tc.expected:step2.the format result is 0.9
     */
    jerry_value_t args[1];
    double number = 0.9;
    args[0] = jerry_create_number(number);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "0.9")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the locale info zh-CN
     * and the number style testaaaa
     * @tc.expected:step3.the format result is "the style is invalid"
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'testaaa'});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    jerry_value_t error = jerry_get_value_from_error(numberFormat, true);
    formatRes = MallocStringOf(error);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "RangeError: the style is invalid")));
    jerry_release_value(error);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Group_Test004
 * @tc.desc: test the enum value in useGroup
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Group_Test004, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN,
     * set the useGroup attribute false
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {useGrouping:false});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 123456
     * @tc.expected:step2.the format result is 123456
     */
    jerry_value_t args[1];
    double num = 123456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123456")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object and set the locale info zh-CN
     * and set the useGrouping attribute true
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {useGrouping:true});");

    /**
     * @tc.steps:step4.get the format attribute 123456
     * @tc.expected:step4.the format result is 123,456
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123,456")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step5.create the numberFormat object and set the locale info zh-CN
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN');");

    /**
     * @tc.steps:step6.get the format attribute and format the number 123456
     * @tc.expected:step6.the format result is 123,456
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123,456")));
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Group_Test005
 * @tc.desc: test the invalid value of useGroup
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Group_Test005, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object and set the
     * useGrouping attribute false
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat({useGrouping:false})");

    /**
     * @tc.step2:step2.get the format attribute, and format the number 123456
     * @tc.expected:step2.the format result is 123,456
     */
    jerry_value_t args[1];
    double num = 123456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123,456")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the number format object and set the locale info zh-CN
     * the useGrouping testaaaa
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {useGrouping:'testaaa'});");

    /**
     * @tc.steps:get the format attribute and format the number 123456
     * @tc.expected: the format result is 123,456
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123,456")));
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Group_Test006
 * @tc.desc: test the enum value of useGrouping in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Group_Test006, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN
     * and set the style percent and useGrouping true
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', useGrouping:true});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 123.456
     * @tc.expected:step2.the format result is 12,346%
     */
    jerry_value_t args[1];
    const double num = 123.456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "12,346%")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the locale info zh-CN
     * set the number style percent and the useGrouping false
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', useGrouping:false})");
    /**
     * @tc.steps:step4.get the format attribute and format the number 123.456
     * @tc.expected:step4.the format result is 12346%
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "12346%")));
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_Group_Test007
 * @tc.desc: test the invalid value of useGroup in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_Group_Test007, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the style percent
     * and set the useGrouping false
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat({style:'percent', useGrouping:false});");
    /**
     * @tc.steps:step2.get the format attribute and format the number 123.456
     * @tc.expected:step2.the format result is 123.456
     */
    jerry_value_t args[1];
    const double formatNum = 123.456;
    args[0] = jerry_create_number(formatNum);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123.456")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the locale info zh-CN, the percent style
     * is percent and the useGrouping is testaaaa
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', useGrouping:'testaaaa'})");
    /**
     * @tc.steps:step4.get the format attribute and format the number 123.456
     * @tc.expected:step4.the format result is 12,346%
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "12,346%")));
    ReleaseJerryValue(args[0], numberFormat, VA_ARG_END_FLAG);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MinimumFractionDigit_Test008
 * @tc.desc: test the correct value of minimumFeactionDigits
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MinimumFractionDigit_Test008, TestSize.Level0)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN
     * and set the minimumFractionDigits 2
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {minimumFractionDigits:2});");
    /**
     * @tc.steps:step2.get the format attribute and format the number 1.23456
     * @tc.expected:step2.the format result is 1.235
     */
    jerry_value_t args[1];
    const double num = 1.23456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "1.235")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the object numberFormat object, set the locale info zh-CN
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {minimumFractionDigits:6});");
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE(formatRes == nullptr);
    if (formatRes != nullptr) {
        EXPECT_FALSE(strcmp(formatRes, "1.234560") != 0);
    }
    ACE_FREE(formatRes);
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MinimumFractionDigit_Test009
 * @tc.desc: test the invalid value of minimumFeactionDigits
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MinimumFractionDigit_Test009, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN,
     * set the minimumFractionDigits -1
     * @tc.expected:step2.the format result is "the minimumFractionDigit is invalid"
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {minimumFractionDigits:-1});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    jerry_value_t error = jerry_get_value_from_error(numberFormat, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the minimumFractionDigits is invalid")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the numberFormat object, set the locale info zh-CN
     * and set the minimumFractionDigits 21
     * @tc.expected:step2.the format result is "the minimumFractionDigit is invalid"
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {minimumFractionDigits:21});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    error = jerry_get_value_from_error(numberFormat, true);
    errorInfo = MallocStringOf(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the minimumFractionDigits is invalid")));
    jerry_release_value(error);
    ACE_FREE(errorInfo);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MinimumFractionDigit_Test010
 * @tc.desc: test the value of minimumFeactionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MinimumFractionDigit_Test010, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN
     * set the minimumFractionDigits 2, set the style attribute percent
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', minimumFractionDigits:2});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 1.23456
     * @tc.expected:step2.the format result is 123%
     */
    jerry_value_t args[1];
    const double num = 1.23456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123%")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the style percent and the minimumFractionDigit
     * 5
     */
    GLOBAL_ConfigLanguage("zh-CN");
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', minimumFractionDigit:5});");

    /**
     * @tc.steps:step4.get the format attribute and format the number 1.23456
     * @tc.ecpected:step4.the format result is 123%
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123%")));
    jerry_release_value(numberFormat);
    ACE_FREE(formatRes);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MinimumFractionDigit_Test011
 * @tc.desc: test the value of minimumFeactionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MinimumFractionDigit_Test011, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN,
     * and set the style percent and set the minimumFractionDigit -1
     * @tc.expected:step1.numberFormat object create failed, the error info is
     * "the minimumFractionDigits is invalid"
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', minimumFractionDigits:-1});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    jerry_value_t error = jerry_get_value_from_error(numberFormat, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_TRUE((errorInfo != nullptr) &&
                (!strcmp(errorInfo, "RangeError: the minimumFractionDigits is invalid")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the numberFormat object, set the style percent, set the minimumFractionDigit 21
     * @tc.expected:step2.the numberFormat object create failed, the error info is
     * "the minimumFractionDigits is invalid"
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', minimumFractionDigits:21});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    error = jerry_get_value_from_error(numberFormat, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the minimumFractionDigits is invalid")));
    ACE_FREE(errorInfo);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MaxmumFractionDigit_Test012
 * @tc.desc: test the value of maxmumFractionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MaxmumFractionDigit_Test012, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the number format object, set the locale info zh-CN
     * the maxmumFractionDigit 4
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {maximumFractionDigits:4});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 1.23456
     * @tc.expected:step2.the format result is 1.2346
     */
    jerry_value_t args[1];
    const double num = 1.23456;
    args[0] = jerry_create_number(num);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "1.2346")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create the numberFormat object, set the maxmumFractionDigit 4
     */
    GLOBAL_ConfigLanguage("zh-CN");
    numberFormat = RunJs("new Intl.NumberFormat({maxmumFractionDigit:4});");

    /**
     * @tc.steps: step4.get the format attribute and format the number 1.23456
     * @tc.expected:step4.the format result is 1.23456
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "1.235")));
    ACE_FREE(formatRes);
    ReleaseJerryValue(numberFormat, args[0], VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MaxmumFractionDigit_Test013
 * @tc.desc: test the value of maxmumFractionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MaxmumFractionDigit_Test013, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN
     * the maxmumFractionDigit -1
     * @tc.expected:step1.the numberFormat object create failed, the error info is
     * "the maximumFractionDigits is invalid"
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {maximumFractionDigits:-1});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    jerry_value_t error = jerry_get_value_from_error(numberFormat, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the maximumFractionDigits is invalid")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.expected:step2.create the  numberFormat object, set the locale info zh-CN
     * and the maxmumFractionDigit 22
     * @tc.expected:step2.the numberFormat object create failed, the error info is
     * "the maximumFractionDigits is invalid"
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {maximumFractionDigits:21});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    error = jerry_get_value_from_error(numberFormat, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the maximumFractionDigits is invalid")));
    ACE_FREE(errorInfo);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MaxmumFractionDigit_Test014
 * @tc.desc: test the value of maxmumFractionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MaxmumFractionDigit_Test014, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN,
     * and set the style percent and maxmumFractionDigit 2
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', maximumFractionDigits:2});");

    /**
     * @tc.steps:step2.get the format attribute and format the number 1.23456
     * @tc.expected:step2.the format result is 123.46%
     */
    jerry_value_t args[1];
    const double number = 1.23456;
    args[0] = jerry_create_number(number);
    char* formatRes = FormatNumber(numberFormat, args, 1);
    jerry_release_value(numberFormat);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123%")));
    ace_free(formatRes);
    formatRes = nullptr;

    /**
     * @tc.steps:step3.create numberFormat object, set the format style percent,
     * the maxmumFractionDigit 5
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent',maximumFractionDigits:5});");
    /**
     * @tc.steps:step4.get the format attribute and format the number 1.23456
     * @tc.expected: the format result is 1.23456
     */
    formatRes = FormatNumber(numberFormat, args, 1);
    EXPECT_FALSE((formatRes == nullptr) || (strcmp(formatRes, "123%")));
    ACE_FREE(formatRes);
    ReleaseJerryValue(args[0], numberFormat, VA_ARG_END_FLAG);
    TDD_CASE_END();
}

/**
 * @tc.name: NumberFormat_MaxmumFractionDigit_Test015
 * @tc.desc: test the value of maxmumFractionDigits in percent
 */
HWTEST_F(NumberFormatModuleTddTest, NumberFormat_MaxmumFractionDigit_Test015, TestSize.Level1)
{
    TDD_CASE_BEGIN();

    /**
     * @tc.steps:step1.create the numberFormat object, set the locale info zh-CN,
     * set the percent style percent and the maxmumFractionDigit 0
     * @tc.expected:step1.create the numberFormat object failed, the error info is
     * "the maximumFractionDigit is invalid"
     */
    jerry_value_t numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', maximumFractionDigits:-1});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    jerry_value_t error = jerry_get_value_from_error(numberFormat, true);
    char* errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the maximumFractionDigits is invalid")));
    ace_free(errorInfo);
    errorInfo = nullptr;

    /**
     * @tc.steps:step2.create the numberFormat object, set the locale info zh-CN,
     * and the maxmumFractionDigit 21
     * @tc.expected:step2.the numberFormat object create failed, the error info is
     * "the maximumFractionDigits is invalid"
     */
    numberFormat = RunJs("new Intl.NumberFormat('zh-CN', {style:'percent', maximumFractionDigits:21});");
    EXPECT_TRUE(jerry_value_is_error(numberFormat));
    error = jerry_get_value_from_error(numberFormat, true);
    errorInfo = MallocStringOf(error);
    jerry_release_value(error);
    EXPECT_FALSE((errorInfo == nullptr) ||
                (strcmp(errorInfo, "RangeError: the maximumFractionDigits is invalid")));
    ACE_FREE(errorInfo);
    TDD_CASE_END();
}

void NumberFormatModuleTddTest::RunTests()
{
#ifndef TDD_ASSERTIONS
    NumberFormat_Locale_Test001();
    NumberFormat_Style_Test002();
    NumberFormat_Style_Test003();
    NumberFormat_Group_Test004();
    NumberFormat_Group_Test005();
    NumberFormat_Group_Test006();
    NumberFormat_Group_Test007();
    NumberFormat_MinimumFractionDigit_Test008();
    NumberFormat_MinimumFractionDigit_Test009();
    NumberFormat_MinimumFractionDigit_Test010();
    NumberFormat_MinimumFractionDigit_Test011();
    NumberFormat_MaxmumFractionDigit_Test012();
    NumberFormat_MaxmumFractionDigit_Test013();
    NumberFormat_MaxmumFractionDigit_Test014();
    NumberFormat_MaxmumFractionDigit_Test015();
#endif
}
}
}
#endif
