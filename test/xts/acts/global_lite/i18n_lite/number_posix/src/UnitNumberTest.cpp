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
#include "UnitNumberTest.h"

using namespace std;
using namespace testing::ext;
namespace OHOS {
using namespace I18N;
using namespace testing;

class NumberTest : public ::testing::TestWithParam<LocaleInfo> {
public:
    static const int ILLEGAL_NUMBERFORMATTYPE = 2;
};

class NumberConstructionTest : public testing::Test {};


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
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_CONSTRUCTIION_0100
 * @tc.name     test NumberFormat Construction,locale is null
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(NumberConstructionTest, GLOBAL_NumberFormat_Construction_0100, Function | MediumTest | Level0)
{
    LocaleInfo *localeInfo = new LocaleInfo();
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(*localeInfo, status);
    EXPECT_TRUE(numberForm != nullptr);
    EXPECT_EQ(status, I18nStatus::IERROR);
    free(numberForm);
    free(localeInfo);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_CONSTRUCTIION_0200
 * @tc.name     test NumberFormat Construction,locale is normal,script is Latn
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(NumberConstructionTest, GLOBAL_NumberFormat_Construction_0200, Function | MediumTest | Level0)
{
    LocaleInfo localeInfo("as", "Latn", "IN");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(localeInfo, status);
    EXPECT_TRUE(numberForm != nullptr);
    free(numberForm);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_CONSTRUCTIION_0300
 * @tc.name     test NumberFormat Construction,locale is normal
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(NumberConstructionTest, GLOBAL_NumberFormat_Construction_0300, Function | MediumTest | Level0)
{
    LocaleInfo localeInfo("es", "US");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(localeInfo, status);
    EXPECT_TRUE(numberForm != nullptr);
    free(numberForm);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_CONSTRUCTIION_0400
 * @tc.name     test NumberFormat Construction,locale is copy
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(NumberConstructionTest, GLOBAL_NumberFormat_Construction_0400, Function | MediumTest | Level0)
{
    LocaleInfo *localeInfo = new LocaleInfo("es", "US");
    LocaleInfo &localeInfo1 = *localeInfo;
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new NumberFormat(localeInfo1, status);
    EXPECT_TRUE(numberForm != nullptr);
    free(numberForm);
    free(localeInfo);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_DOUBLEFORMAT_0100
 * @tc.name     test NumberFormat Format API,type is DECIMAL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_DoubleFormat_0100, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    double doubleNum2 = 8.23456789567E8;
    double doubleNum3 = 8234.0;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        std::string numberFormOut2 = numberForm->Format(doubleNum2, NumberFormatType::DECIMAL, status);
        std::string numberFormOut3 = numberForm->Format(doubleNum3, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][6]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][7]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][8]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_DOUBLEFORMAT_0200
 * @tc.name     test NumberFormat Format API,type is PERCENT
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_DoubleFormat_0200, Function | MediumTest | Level0)
{
    double percentNum1 = 0.1234;
    double percentNum2 = 0.123456;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        std::string numberFormOut1 = numberForm->Format(percentNum1, NumberFormatType::PERCENT, status);
        std::string numberFormOut2 = numberForm->Format(percentNum2, NumberFormatType::PERCENT, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][18]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][19]);
        free(numberForm);
    }
}


/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INTFORMAT_0100
 * @tc.name     test NumberFormat Format API
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_IntFormat_0100, Function | MediumTest | Level0)
{
    int intNum1 = 1234567;
    int intNum2 = 123456789;
    int intNum3 = 1234;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        std::string numberFormOut1 = numberForm->Format(intNum1, status);
        std::string numberFormOut2 = numberForm->Format(intNum2, status);
        std::string numberFormOut3 = numberForm->Format(intNum3, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][0]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][1]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][2]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_DOUBLEFORMATNOGROUP_0100
 * @tc.name     test NumberFormat FormatNoGroup API
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_DoubleFormatNoGroup_0100, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    double doubleNum2 = 8.23456789567E8;
    double doubleNum3 = 8234.0;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        std::string numberFormOut1 = numberForm->FormatNoGroup(doubleNum1, NumberFormatType::DECIMAL, status);
        std::string numberFormOut2 = numberForm->FormatNoGroup(doubleNum2, NumberFormatType::DECIMAL, status);
        std::string numberFormOut3 = numberForm->FormatNoGroup(doubleNum3, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][9]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][10]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][11]);
        free(numberForm);
    }
}


/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INTFORMATNOGROUP_0100
 * @tc.name     test NumberFormat FormatNoGroup API,illegal NumberFormatType type
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_IntFormatNoGroup_0100, Function | MediumTest | Level0)
{
    int intNum1 = 1234567;
    int intNum2 = 123456789;
    int intNum3 = 1234;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        std::string numberFormOut1 = numberForm->FormatNoGroup(intNum1, status);
        std::string numberFormOut2 = numberForm->FormatNoGroup(intNum2, status);
        std::string numberFormOut3 = numberForm->FormatNoGroup(intNum3, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][3]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][4]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][5]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMAXDECIMALLENGTH_0100
 * @tc.name     test NumberFormat SetMaxDecimalLength API,length is -1
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMaxDecimalLength_0100, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    double doubleNum2 = 8.23456789567E8;
    double doubleNum3 = 8234.0;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMaxDecimalLength(-1);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        std::string numberFormOut2 = numberForm->Format(doubleNum2, NumberFormatType::DECIMAL, status);
        std::string numberFormOut3 = numberForm->Format(doubleNum3, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][12]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][13]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][14]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMAXDECIMALLENGTH_0200
 * @tc.name     test NumberFormat SetMaxDecimalLength API,length is 0
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMaxDecimalLength_0200, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    double doubleNum2 = 8.23456789567E8;
    double doubleNum3 = 8234.0;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMaxDecimalLength(0);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        std::string numberFormOut2 = numberForm->Format(doubleNum2, NumberFormatType::DECIMAL, status);
        std::string numberFormOut3 = numberForm->Format(doubleNum3, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberMaxLengthZeroExpect[localeIndex][12]);
        EXPECT_EQ(numberFormOut2, g_numberMaxLengthZeroExpect[localeIndex][13]);
        EXPECT_EQ(numberFormOut3, g_numberMaxLengthZeroExpect[localeIndex][14]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMAXDECIMALLENGTH_0300
 * @tc.name     test NumberFormat SetMaxDecimalLength API,length is 10
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMaxDecimalLength_0300, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    double doubleNum2 = 8.23456789567E8;
    double doubleNum3 = 8234.0;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMaxDecimalLength(10);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        std::string numberFormOut2 = numberForm->Format(doubleNum2, NumberFormatType::DECIMAL, status);
        std::string numberFormOut3 = numberForm->Format(doubleNum3, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][12]);
        EXPECT_EQ(numberFormOut2, g_numberFormExpect[localeIndex][13]);
        EXPECT_EQ(numberFormOut3, g_numberFormExpect[localeIndex][14]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMAXDECIMALLENGTH_0400
 * @tc.name     test NumberFormat SetMaxDecimalLength API,length is 2
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMaxDecimalLength_0400, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.5678;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMaxDecimalLength(2);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberMaxLengthTwoExpect[localeIndex][12]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMINDECIMALLENGTH_0100
 * @tc.name     test NumberFormat SetMinDecimalLength API,length is -1
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMinDecimalLength_0100, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMinDecimalLength(-1);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][12]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMINDECIMALLENGTH_0200
 * @tc.name     test NumberFormat SetMinDecimalLength API,length is 0
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMinDecimalLength_0200, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMinDecimalLength(0);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][12]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMINDECIMALLENGTH_0300
 * @tc.name     test NumberFormat SetMinDecimalLength API,length is 3
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMinDecimalLength_0300, Function | MediumTest | Level0)
{
    double doubleNum1 = 8234567.56;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMinDecimalLength(3);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][15]);
        free(numberForm);
    }
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_INT_SETMINDECIMALLENGTH_0400
 * @tc.name     test NumberFormat SetMinDecimalLength API,length is 2
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(NumberTest, GLOBAL_NumberFormat_SetMinDecimalLength_0400, Function | MediumTest | Level0)
{
    double doubleNum1 = 8.23456789567E8;
    LocaleInfo localeInfo = GetParam();
    int localeIndex = GetLocaleIndex(localeInfo, g_locales);
    if (localeIndex >= 0) {
        int status = I18nStatus::ISUCCESS;
        NumberFormat *numberForm = new NumberFormat(localeInfo, status);
        numberForm->SetMinDecimalLength(2);
        std::string numberFormOut1 = numberForm->Format(doubleNum1, NumberFormatType::DECIMAL, status);
        EXPECT_EQ(numberFormOut1, g_numberFormExpect[localeIndex][16]);
        free(numberForm);
    }
}

INSTANTIATE_TEST_CASE_P(AllNumberTest, NumberTest,
    Values(LocaleInfo("ar", "", ""), LocaleInfo("ar", "AE"), LocaleInfo("ar", "EG"), LocaleInfo("as", "Latn", "IN"),
    LocaleInfo("de", "DE"), LocaleInfo("en", "", ""), LocaleInfo("en", "GB"), LocaleInfo("en", "Qaag", ""),
    LocaleInfo("en", "US"), LocaleInfo("es", "ES"), LocaleInfo("es", "US"), LocaleInfo("fr", "FR"),
    LocaleInfo("it", "IT"), LocaleInfo("ka", "GE"), LocaleInfo("mai", "Deva", ""), LocaleInfo("my", "MM"),
    LocaleInfo("pa", "Guru", ""), LocaleInfo("pt", "BR"), LocaleInfo("pt", "PT"), LocaleInfo("sr", "Cyrl", ""),
    LocaleInfo("th", "TH"), LocaleInfo("zh", "", ""), LocaleInfo("zh", "Hans", "CN"), LocaleInfo("zh", "Hans", "HK"),
    LocaleInfo("zh", "Hans", "MO"), LocaleInfo("zh", "Hant", "TW")));
} // namespace OHOS