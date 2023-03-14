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
#include "UnitNumberRecallTest.h"

using namespace std;
using namespace testing::ext;
namespace OHOS {
using namespace I18N;
using namespace testing;

class UnitNumberRecallTest : public testing::Test {};


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
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_LOCALE_0100
 * @tc.name     test NumberFormat Recall,locale is LocaleInfo en,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(UnitNumberRecallTest, GLOBAL_NumberFormat_LocaleRecall_0100, Function | MediumTest | Level0)
{
    int intNum = 1234567;
    LocaleInfo *localeInfo = new LocaleInfo("en", "ZZ");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(*localeInfo, status);
    std::string numberFormOut = numberForm->Format(intNum, status);
    LocaleInfo *localeRecall = new LocaleInfo("en", "", "US");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(numberFormOut, g_numberFormExpect[localeIndex][0]);
    }
    free(localeRecall);
    free(numberForm);
    free(localeInfo);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_LOCALE_0200
 * @tc.name     test NumberFormat Recall,locale is LocaleInfo zh,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(UnitNumberRecallTest, GLOBAL_NumberFormat_LocaleRecall_0200, Function | MediumTest | Level0)
{
    int intNum = 1234567;
    LocaleInfo *localeInfo = new LocaleInfo("zh", "ZZ");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(*localeInfo, status);
    std::string numberFormOut = numberForm->Format(intNum, status);
    LocaleInfo *localeRecall = new LocaleInfo("zh", "", "");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(numberFormOut, g_numberFormExpect[localeIndex][0]);
    }
    free(localeRecall);
    free(numberForm);
    free(localeInfo);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_LOCALE_0300
 * @tc.name     test NumberFormat Recall,locale is LocaleInfo zh,Hans,ZZ
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(UnitNumberRecallTest, GLOBAL_NumberFormat_LocaleRecall_0300, Function | MediumTest | Level0)
{
    int intNum = 1234567;
    LocaleInfo *localeInfo = new LocaleInfo("zh", "Hans", "ZZ");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(*localeInfo, status);
    std::string numberFormOut = numberForm->Format(intNum, status);
    LocaleInfo *localeRecall = new LocaleInfo("zh", "Hans", "");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(numberFormOut, g_numberFormExpect[localeIndex][0]);
    }
    free(localeRecall);
    free(numberForm);
    free(localeInfo);
}

/* *
 * @tc.number   SUB_GLOBAL_GLOBALIZATIONBUSINESS_NUMBER_FORMAT_LOCALE_0400
 * @tc.name     test NumberFormat Recall,locale is LocaleInfo zz
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(UnitNumberRecallTest, GLOBAL_NumberFormat_LocaleRecall_0400, Function | MediumTest | Level0)
{
    int intNum = 1234567;
    LocaleInfo *localeInfo = new LocaleInfo("zz", "");
    int status = I18nStatus::ISUCCESS;
    NumberFormat *numberForm = new OHOS::I18N::NumberFormat(*localeInfo, status);
    std::string numberFormOut = numberForm->Format(intNum, status);
    LocaleInfo *localeRecall = new LocaleInfo("en", "US");
    int localeIndex = GetLocaleIndex(*localeRecall, g_locales);
    if (localeIndex >= 0) {
        EXPECT_EQ(numberFormOut, g_numberFormExpect[localeIndex][0]);
    }
    free(localeRecall);
    free(numberForm);
    free(localeInfo);
}
} // namespace OHOS