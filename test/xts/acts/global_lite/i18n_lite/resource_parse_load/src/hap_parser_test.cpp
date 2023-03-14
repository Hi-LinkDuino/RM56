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

#include "hap_parser_test.h"

#include <gtest/gtest.h>

#include "test_common.h"
#include "utils/string_utils.h"

#define private public

#include "hap_parser.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class HapParserTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HapParserTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void HapParserTest::TearDownTestCase()
{
    // step 2: input testsuit teardown step
}

void HapParserTest::SetUp()
{
    // step 3: input testcase setup step
}

void HapParserTest::TearDown()
{
    // step 4: input testcase teardown step
}

/*
 * @tc.name: HapParserFuncTest001
 * @tc.desc: Test CreateResConfigFromKeyParams
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest001, TestSize.Level1)
{
    std::vector<KeyParam *> keyParams;
    auto kp = new KeyParam();
    kp->type_ = LANGUAGES;
    kp->value_ = 31336;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = REGION;
    kp->value_ = 17230;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = SCREEN_DENSITY;
    kp->value_ = SCREEN_DENSITY_SDPI;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = DEVICETYPE;
    kp->value_ = DEVICE_CAR;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = DIRECTION;
    kp->value_ = DIRECTION_VERTICAL;
    kp->InitStr();
    keyParams.push_back(kp);
    auto config = HapParser::CreateResConfigFromKeyParams(keyParams);
    if (config != nullptr) {
        EXPECT_EQ(std::string("zh"), config->GetLocaleInfo()->GetLanguage());
        EXPECT_EQ(std::string("CN"), config->GetLocaleInfo()->GetRegion());
        EXPECT_EQ(std::string("Hans"), config->GetLocaleInfo()->GetScript());
        EXPECT_EQ(DEVICE_CAR, config->GetDeviceType());
        EXPECT_EQ(DIRECTION_VERTICAL, config->GetDirection());
        EXPECT_EQ(SCREEN_DENSITY_SDPI, config->GetScreenDensity());
    } else {
        EXPECT_TRUE(false);
    }

    for (auto kp = keyParams.begin(); kp != keyParams.end(); kp++) {
        delete *kp;
    }
    delete config;
}

/*
 * @tc.name: HapParserFuncTest002
 * @tc.desc: Test GetDeviceType
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest002, TestSize.Level1)
{
    ASSERT_EQ(DEVICE_CAR, HapParser::GetDeviceType(DEVICE_CAR));
    ASSERT_EQ(DEVICE_PC, HapParser::GetDeviceType(DEVICE_PC));
    ASSERT_EQ(DEVICE_PHONE, HapParser::GetDeviceType(DEVICE_PHONE));
    ASSERT_EQ(DEVICE_TABLET, HapParser::GetDeviceType(DEVICE_TABLET));
    ASSERT_EQ(DEVICE_TV, HapParser::GetDeviceType(DEVICE_TV));
    ASSERT_EQ(DEVICE_WEARABLE, HapParser::GetDeviceType(DEVICE_WEARABLE));
    ASSERT_EQ(DEVICE_NOT_SET, HapParser::GetDeviceType(1000000));
}

/*
 * @tc.name: HapParserFuncTest003
 * @tc.desc: Test GetScreenDensity
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest003, TestSize.Level1)
{
    ASSERT_EQ(SCREEN_DENSITY_SDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_SDPI));
    ASSERT_EQ(SCREEN_DENSITY_MDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_MDPI));
    ASSERT_EQ(SCREEN_DENSITY_LDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_LDPI));
    ASSERT_EQ(SCREEN_DENSITY_XLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XLDPI));
    ASSERT_EQ(SCREEN_DENSITY_XXLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XXLDPI));
    ASSERT_EQ(SCREEN_DENSITY_XXXLDPI, HapParser::GetScreenDensity(SCREEN_DENSITY_XXXLDPI));
    ASSERT_EQ(SCREEN_DENSITY_NOT_SET, HapParser::GetScreenDensity(10000000));
}

/*
 * @tc.name: HapParserFuncTest004
 * @tc.desc: Test ToFolderPath
 * @tc.type: FUNC
 */
HWTEST_F(HapParserTest, HapParserFuncTest004, TestSize.Level1)
{
    std::vector<KeyParam *> keyParams;
    auto kp = new KeyParam();
    kp->type_ = LANGUAGES;
    kp->value_ = 31336;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = REGION;
    kp->value_ = 17230;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = SCREEN_DENSITY;
    kp->value_ = SCREEN_DENSITY_SDPI;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = DEVICETYPE;
    kp->value_ = DEVICE_CAR;
    kp->InitStr();
    keyParams.push_back(kp);
    kp = new KeyParam();
    kp->type_ = DIRECTION;
    kp->value_ = DIRECTION_VERTICAL;
    kp->InitStr();
    keyParams.push_back(kp);
    std::string folder = HapParser::ToFolderPath(keyParams);
    ASSERT_EQ("zh_CN-vertical-car-sdpi", folder);
    for (auto kp = keyParams.begin(); kp != keyParams.end(); kp++) {
        delete *kp;
    }
}