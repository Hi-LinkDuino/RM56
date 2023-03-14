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
#include "locale_info_test.h"

#include <chrono>
#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "hilog_wrapper.h"
#include "locale_info.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class LocaleInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void LocaleInfoTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void LocaleInfoTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void LocaleInfoTest::SetUp()
{
}

void LocaleInfoTest::TearDown()
{
}

/*
 * @tc.name: LocaleInfoFindAndSortTest001
 * @tc.desc: Test FindAndSort
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoFindAndSortTest001, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CN");
    request.push_back("en-US");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-US";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en-US");
    EXPECT_TRUE(outValue.at(1) == "en");
    EXPECT_TRUE(outValue.at(2) == "");
    EXPECT_TRUE(outValue.at(3) == "en-CN");
    EXPECT_TRUE(outValue.at(4) == "en-GB");
}

/*
 * @tc.name: LocaleInfoFindAndSortTest002
 * @tc.desc: Test FindAndSort
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoFindAndSortTest002, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("zh-CN");
    request.push_back("zh-TW");
    request.push_back("zh");
    request.push_back("zh-HK");
    request.push_back("");
    std::string current = "zh-CN";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "zh-CN");
    EXPECT_TRUE(outValue.at(1) == "zh");
    EXPECT_TRUE(outValue.at(2) == "");
}

/*
 * @tc.name: LocaleInfoFindAndSortTest003
 * @tc.desc: Test FindAndSort
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoFindAndSortTest003, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-CN";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en");
    EXPECT_TRUE(outValue.at(1) == "en-CA");
    EXPECT_TRUE(outValue.at(2) == "");
    EXPECT_TRUE(outValue.at(3) == "en-GB");
}

/*
 * @tc.name: LocaleInfoFindAndSortTest004
 * @tc.desc: Test FindAndSort
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoFindAndSortTest004, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-Qaag";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en");
    EXPECT_TRUE(outValue.at(1) == "en-GB");
    EXPECT_TRUE(outValue.at(2) == "en-CA");
    EXPECT_TRUE(outValue.at(3) == "");
}

/*
 * @tc.name: LocaleInfoFindAndSortTest005
 * @tc.desc: Test FindAndSort
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoFindAndSortTest005, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-001");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-AI";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en-001");
    EXPECT_TRUE(outValue.at(1) == "en");
    EXPECT_TRUE(outValue.at(2) == "en-GB");
    EXPECT_TRUE(outValue.at(3) == "en-CA");
    EXPECT_TRUE(outValue.at(4) == "");
}

/*
 * @tc.name: LocaleInfoGetSysDefaultTest001
 * @tc.desc: Test GetSysDefault
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoGetSysDefaultTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    UpdateSysDefault(*localeInfo, false);
    const LocaleInfo* currentLocaleInfo = GetSysDefault();
    if (currentLocaleInfo == nullptr) {
        EXPECT_TRUE(false);
        delete localeInfo;
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", currentLocaleInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("CN", currentLocaleInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoUpdateSysDefaultTest001
 * @tc.desc: Test UpdateSysDefault
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoUpdateSysDefaultTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    UpdateSysDefault(*localeInfo, false);
    const LocaleInfo* currentLocaleInfo = GetSysDefault();
    if (currentLocaleInfo == nullptr) {
        EXPECT_TRUE(false);
        delete localeInfo;
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", currentLocaleInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("CN", currentLocaleInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = BuildFromString("en-US", '-', state);
    UpdateSysDefault(*localeInfo, false);
    currentLocaleInfo = GetSysDefault();
    if (currentLocaleInfo == nullptr) {
        EXPECT_TRUE(false);
        delete localeInfo;
        return;
    }
    EXPECT_TRUE(std::strcmp("en", currentLocaleInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("US", currentLocaleInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = BuildFromString("en-Qaag-US", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    UpdateSysDefault(*localeInfo, false);
    currentLocaleInfo = GetSysDefault();
    if (currentLocaleInfo == nullptr) {
        EXPECT_TRUE(false);
        delete localeInfo;
        return;
    }
    EXPECT_TRUE(std::strcmp("en", currentLocaleInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("US", currentLocaleInfo->GetRegion()) == 0);
    EXPECT_TRUE(std::strcmp("Qaag", currentLocaleInfo->GetScript()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoGetLanguageTest001
 * @tc.desc: Test LocaleInfo GetLanguage
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoGetLanguageTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoGetRegionTest001
 * @tc.desc: Test LocaleInfo GetRegion
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoGetRegionTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("CN", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoGetScriptTest001
 * @tc.desc: Test LocaleInfo GetScript
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoGetScriptTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-Hant-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("Hant", localeInfo->GetScript()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest001
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts("zh", "Hant", "CN", state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Hant", localeInfo->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("CN", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest002
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest002, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts("zh1", "Hant", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest003
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest003, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts("zh", "Hants", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_SCRIPT_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest004
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest004, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts("zh", "Hant", "C", state);
    EXPECT_TRUE(state == INVALID_BCP47_REGION_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest005
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest005, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts(nullptr, "Hants", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromPartsTest006
 * @tc.desc: Test BuildFromParts
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromPartsTest006, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromParts("zh", nullptr, nullptr, state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(localeInfo->GetScript() == nullptr);
    EXPECT_TRUE(localeInfo->GetRegion() == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest001
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-Hant-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Hant", localeInfo->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("CN", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest002
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest002, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh1-Hant-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest003
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest003, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("-Hant-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest004
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest004, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(localeInfo->GetScript() == nullptr);
    EXPECT_TRUE(localeInfo->GetRegion() == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest005
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest005, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("en_US", '_', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("en", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(localeInfo->GetScript() == nullptr);
    EXPECT_TRUE(std::strcmp("US", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest006
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest006, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("en_Latn_US", '&', state);
    EXPECT_TRUE(state == NOT_SUPPORT_SEP);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest007
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest007, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("en_Latn_US", '_', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("en", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Latn", localeInfo->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("US", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest008
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest008, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-Hants-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_SCRIPT_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest009
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest009, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-Hant-C", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_REGION_SUBTAG);
    EXPECT_TRUE(localeInfo == nullptr);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoBuildFromStringTest0010
 * @tc.desc: Test BuildFromString
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoBuildFromStringTest0010, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN-xxxx", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", localeInfo->GetLanguage()) == 0);
    EXPECT_TRUE(localeInfo->GetScript() == nullptr);
    EXPECT_TRUE(std::strcmp("CN", localeInfo->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

/*
 * @tc.name: LocaleInfoPerformanceFuncTest001
 * @tc.desc: Test FindAndSort Performance
 * @tc.type: FUNC
 */
HWTEST_F(LocaleInfoTest, LocaleInfoPerformanceFuncTest001, TestSize.Level1)
{
    unsigned long long total = 0;
    double average = 0;
    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<std::string> request;
        std::vector<std::string> outValue;
        request.push_back("en");
        request.push_back("en-CN");
        request.push_back("en-US");
        request.push_back("en-GB");
        request.push_back("");
        std::string current = "en-US";
        FindAndSort(current, request, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    HILOG_DEBUG("avg cost FindAndSort: %f us", average);
    EXPECT_LT(average, 500);
};